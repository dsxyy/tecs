/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：netplane_mgr.cpp
* 文件标识：
* 内容摘要：CNetplaneMgr类实现文件
* 当前版本：1.0
* 作    者：
* 完成日期：2013年1月18日
*
* 修改记录1：
*     修改日期：2013/1/18
*     版 本 号：V1.0
*     修 改 人：Wang.Lule
*     修改内容：创建
******************************************************************************/

#include "vnet_comm.h"
#include "vnet_mid.h"
#include "vnet_error.h"
#include "vnetlib_event.h"
#include "vnetlib_msg.h"

#include "vnet_db_mgr.h"
#include "vnet_db_proc.h"
#include "vnet_db_logicnetwork.h"
#include "vnet_db_logicnetwork_iprange.h"
#include "vnet_db_quantum_cfg.h"
#include "vnet_db_quantum_network.h"
#include "vnet_db_quantum_port.h"

#include "vnet_vnic.h"
#include "port_group_mgr.h"

#include "ipv4_addr.h"
#include "ipv4_addr_range.h"
#include "logic_network.h"
#include "logic_network_mgr.h"
#include "netplane_mgr.h"

#include <restrpc-c/girerr.hpp>
#include <restrpc-c/base.hpp>
#include <restrpc-c/client_simple.hpp>
#include <restrpc-c/json.h>

using namespace restrpc_c;

namespace zte_tecs
{
    CLogicNetworkMgr *CLogicNetworkMgr::s_pInstance = NULL;
    
    CLogicNetworkMgr::CLogicNetworkMgr()
    {
        m_pMU = NULL;
        m_bOpenDbgInf = VNET_FALSE;
    }
    
    CLogicNetworkMgr::~CLogicNetworkMgr()
    {
        m_pMU = NULL;
    }
    
    int32 CLogicNetworkMgr::GetNetplaneByLogiNetID(const string &cstrLogiNetUUID, string &strNetplaneUUID)
    {
        string strPGID;
        
        CPortGroupMgr *pPGMgr = CPortGroupMgr::GetInstance();
        if(NULL == pPGMgr)
        {        
            VNET_LOG(VNET_LOG_ERROR, "CVlanPoolMgr::GetNetplaneByLogiNetID: CPortGroupMgr instance is NULL.\n");
            return -1;
        } 
        
        if(0 != GetPortGroupUUID(cstrLogiNetUUID, strPGID))
        {
            VNET_LOG(VNET_LOG_ERROR, "CLogicNetworkMgr::GetNetplaneByLogiNetID: Get PG uuid(LogiNetID:%s) failed.\n",
                cstrLogiNetUUID.c_str());
            return -1;
        }
        if(PORTGROUP_OPER_SUCCESS != pPGMgr->GetPGNetplane(strPGID, strNetplaneUUID))
        {        
            VNET_LOG(VNET_LOG_ERROR, "CLogicNetworkMgr::GetNetplaneByLogiNetID: Call GetPGNetplane(%s) failed.\n", 
                strPGID.c_str());            
            return -1;
        }  
        
        return 0;
    }

    int32 CLogicNetworkMgr::CheckAllVlanTrunkOfSRIOV(vector<CVNetVnicConfig> &vecVNetVnic)
    {
        CPortGroupMgr *pPGMgr = CPortGroupMgr::GetInstance();
        if(NULL == pPGMgr)
        {        
            VNET_LOG(VNET_LOG_ERROR, "CLogicNetworkMgr::IsAllVlanTrunkOfSRIOV: CPortGroupMgr instance is NULL.\n");
            return -1;
        }      
        
        vector<CVNetVnicConfig>::iterator itr = vecVNetVnic.begin();
        for( ; itr != vecVNetVnic.end(); ++itr)
        {
            if(!itr->m_nIsSriov)
            {
                continue ;
            }
            
            string strPGID;
            if(0 != GetPortGroupUUID(itr->m_strLogicNetworkID, strPGID))
            {
                VNET_LOG(VNET_LOG_ERROR, "CLogicNetworkMgr::IsAllVlanTrunkOfSRIOV: Call GetPortGroupUUID(%s, ...) failed.\n",
                    itr->m_strLogicNetworkID.c_str());
                return -1;
            }

            int32 nRet = pPGMgr->IsMaxTrunkVlanRange(strPGID);
            if(0 != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, "CLogicNetworkMgr::IsAllVlanTrunkOfSRIOV: Call IsMaxTrunkVlanRange(%s, ...) failed, ret: %d.\n",
                    strPGID.c_str(), nRet);
                return nRet;
            }    
        }
        
        return 0;
    }

    int32 CLogicNetworkMgr::CreateQuantumNetwork(CVNetVmMem &cVnetVmMem)
    {
        int32 iRet = LOGIC_NETWORK_OPER_SUCCESS;
        string strSvrIP = "";

        vector<CVNetVnicMem> vecVNetVnic;
        cVnetVmMem.GetVecVnicMem(vecVNetVnic);

        CPortGroupMgr *pPGMgr = CPortGroupMgr::GetInstance();
        if (NULL == pPGMgr)
        {
            VNET_LOG(VNET_LOG_ERROR, "CLogicNetworkMgr::CreateQuantumNetwork: call CPortGroupMgr::GetInstance() failed.\n");
            return ERR_SCHEDULE_GET_PG_MGR_INST_FAIL;
        }

        CVNetDbMgr * pMgr = CVNetDbMgr::GetInstance();
        if(NULL == pMgr)
        {
            VNET_LOG(VNET_LOG_ERROR, "CLogicNetworkMgr::AddLogicNetwork: GetInstance() is NULL.\n");
            return DB_ERROR_GET_INST_FAILED;
        }

        vector<CVNetVnicMem>::iterator itr = vecVNetVnic.begin();
        for( ; itr != vecVNetVnic.end(); ++itr)
        {
            CPortGroup cPortGroup;
            string strPortGroupUuid = itr->GetVmPgUuid();
            cPortGroup.SetUuid(strPortGroupUuid);
            iRet = pPGMgr->GetPGDetail(cPortGroup);
            if(0 != iRet)
            {
                VNET_LOG(VNET_LOG_ERROR, "CLogicNetworkMgr::CreateQuantumNetwork: call GetPGDetail(%s) failed.\n", strPortGroupUuid.c_str());
                return ERR_SCHEDULE_GET_PG_DETAIL_FAILED;
            }            

            if((1 == cPortGroup.GetIsSdn())
                && (EN_PORTGROUP_TYPE_VM_PRIVATE == cPortGroup.GetPgType()))            
            {
                /* todo 调用RESTful接口创建quantum network,并将quantum返回的Uuid保存 */
                string strQuantumNetworkUuid = "";
                CDBOperateSdnCfg * pSdn = pMgr->GetISdnCfg();
                if(NULL != pSdn)
                {
                    CDbQuantumRestfulCfg cQuantumRestful;
                    int32 nRet = pSdn->QueryQuantum(cQuantumRestful);
                    if(VNET_DB_SUCCESS == nRet)
                    {
                        strSvrIP = cQuantumRestful.GetIp();
                    }
                }

                CLogicNetwork cLogicNetwork("");
                if(0 != GetLogicNetwork(itr->GetLogicNetworkUuid(), cLogicNetwork))
                {
                    continue;
                }

                CDBOperateQuantumNetwork *pQuantumNetwork = pMgr->GetIQuantumNetwork();
                if(NULL == pQuantumNetwork)
                {
                    return 0;
                }

                CDbQuantumNetwork cDbQuantumNetwork;
                cDbQuantumNetwork.SetLnUuid(itr->GetLogicNetworkUuid().c_str());
                cDbQuantumNetwork.SetProjectId(cVnetVmMem.GetProjectId());
                pQuantumNetwork->QueryByLnProjectid(cDbQuantumNetwork);
                if(0 == cDbQuantumNetwork.GetUuid().size())
                {
                    RESTfulCreateNetwork(strSvrIP, cLogicNetwork.GetName(), cVnetVmMem.GetProjectId(), strQuantumNetworkUuid);

                    /* quantum返回的信息存数据库 */                    
                    cDbQuantumNetwork.SetUuid(strQuantumNetworkUuid.c_str());
                    pQuantumNetwork->Add(cDbQuantumNetwork);

                    itr->SetQuantumNetworkUuid(strQuantumNetworkUuid);
                }
                else
                {
                    itr->SetQuantumNetworkUuid(cDbQuantumNetwork.GetUuid());
                }                
            }
        }

        cVnetVmMem.SetVecVnicMem(vecVNetVnic);

        return 0;
    }

    int32 CLogicNetworkMgr::CreateQuantumNetwork(const CLogicNetworkMsg &cMsg, const string &cstrLnUuid)
    {
        int32 iRet = LOGIC_NETWORK_OPER_SUCCESS;
        string strSvrIP = "";

        CVNetDbMgr * pMgr = CVNetDbMgr::GetInstance();
        if(NULL == pMgr)
        {
            VNET_LOG(VNET_LOG_ERROR, "CLogicNetworkMgr::CreateQuantumNetwork: GetInstance() is NULL.\n");
            return DB_ERROR_GET_INST_FAILED;
        }

        CPortGroupMgr *pPGMgr = CPortGroupMgr::GetInstance();
        if (NULL == pPGMgr)
        {
            VNET_LOG(VNET_LOG_ERROR, "CLogicNetworkMgr::CreateQuantumNetwork: call CPortGroupMgr::GetInstance() failed.\n");
            return ERR_SCHEDULE_GET_PG_MGR_INST_FAIL;
        }

        CPortGroup cPortGroup;
        cPortGroup.SetUuid(cMsg.m_strPortGroupUUID);
        iRet = pPGMgr->GetPGDetail(cPortGroup);
        if(0 != iRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CLogicNetworkMgr::CreateQuantumNetwork: call GetPGDetail(%s) failed.\n", cMsg.m_strPortGroupUUID.c_str());
            return ERR_SCHEDULE_GET_PG_DETAIL_FAILED;
        }

        if((1 == cPortGroup.GetIsSdn())
            && (EN_PORTGROUP_TYPE_VM_SHARE == cPortGroup.GetPgType()))
        {
            //调用RESTful接口创建quantum network,并将quantum返回的Uuid保存,创建之前要判断下,是否已经创建过了
            string strQuantumNetworkUuid = "";

            CDBOperateSdnCfg * pSdn = pMgr->GetISdnCfg();
            if(NULL != pSdn)
            {
                CDbQuantumRestfulCfg cQuantumRestful;
                int32 nRet = pSdn->QueryQuantum(cQuantumRestful);
                if(VNET_DB_SUCCESS == nRet)
                {
                    strSvrIP = cQuantumRestful.GetIp();
                }
            }

            CDBOperateQuantumNetwork *pQuantumNetwork = pMgr->GetIQuantumNetwork();
            if(NULL == pQuantumNetwork)
            {
                return 0;
            }

            CDbQuantumNetwork cDbQuantumNetwork;
            cDbQuantumNetwork.SetLnUuid(cstrLnUuid.c_str());
            cDbQuantumNetwork.SetProjectId(0);
            pQuantumNetwork->QueryByLnProjectid(cDbQuantumNetwork);
            if(0 == cDbQuantumNetwork.GetUuid().size())
            {
                RESTfulCreateNetwork(strSvrIP, cMsg.m_strName, 0, strQuantumNetworkUuid);
                
                /* quantum返回的信息存数据库 */
                cDbQuantumNetwork.SetUuid(strQuantumNetworkUuid.c_str());             
                
                CDBOperateQuantumNetwork *pQuantumNetwork = pMgr->GetIQuantumNetwork();
                if(NULL != pQuantumNetwork)
                {
                    pQuantumNetwork->Add(cDbQuantumNetwork);
                }
            }
        }

        return 0;
    }

    int32 CLogicNetworkMgr::DeleteQuantumNetwork(CVNetVmMem &cVnetVmMem)
    {
        int32 iRet = LOGIC_NETWORK_OPER_SUCCESS;
        string strSvrIP = "";

        vector<CVNetVnicMem> vecVNetVnic;
        cVnetVmMem.GetVecVnicMem(vecVNetVnic);

        CPortGroupMgr *pPGMgr = CPortGroupMgr::GetInstance();
        if (NULL == pPGMgr)
        {
            VNET_LOG(VNET_LOG_ERROR, "CLogicNetworkMgr::DeleteQuantumNetwork: call CPortGroupMgr::GetInstance() failed.\n");
            return ERR_SCHEDULE_GET_PG_MGR_INST_FAIL;
        }

        CVNetDbMgr * pMgr = CVNetDbMgr::GetInstance();
        if(NULL == pMgr)
        {
            VNET_LOG(VNET_LOG_ERROR, "CLogicNetworkMgr::DeleteQuantumNetwork: GetInstance() is NULL.\n");
            return DB_ERROR_GET_INST_FAILED;
        }

        vector<CVNetVnicMem>::iterator itr = vecVNetVnic.begin();
        for( ; itr != vecVNetVnic.end(); ++itr)
        {
            CPortGroup cPortGroup;
            string strPortGroupUuid = itr->GetVmPgUuid();
            cPortGroup.SetUuid(strPortGroupUuid);
            iRet = pPGMgr->GetPGDetail(cPortGroup);
            if(0 != iRet)
            {
                VNET_LOG(VNET_LOG_ERROR, "CLogicNetworkMgr::DeleteQuantumNetwork: call GetPGDetail(%s) failed.\n", strPortGroupUuid.c_str());
                return -1;
            }

            /* 共享类型网络在删除逻辑网络的时候删除quantum network */
            if(EN_PORTGROUP_TYPE_VM_SHARE == cPortGroup.GetPgType())
            {
                continue;
            }

            if(1 == cPortGroup.GetIsSdn() && (EN_PORTGROUP_TYPE_VM_PRIVATE == cPortGroup.GetPgType()))            
            {
                //删除网络之前,要判断下该网络下是否还有端口
                vector<CDbQuantumPort> vecQPInfo;
                CDBOperateQuantumPort *pQuantumPort = pMgr->GetIQuantumPort();
                if(NULL != pQuantumPort)
                {
                    pQuantumPort->QueryAll(vecQPInfo);                        
                }

                vector<CDbQuantumPort>::iterator itrQuantumPort = vecQPInfo.begin();
                for( ; itrQuantumPort != vecQPInfo.end(); ++itrQuantumPort)
                {
                    //quantum网络下还有其他端口,不删除直接返回
                    if(itr->GetQuantumNetworkUuid() == itrQuantumPort->GetQNUuid())
                    {
                        return 0;
                    }
                }
                
                /* 调用RESTful接口删除quantum network */
                CDBOperateSdnCfg * pSdn = pMgr->GetISdnCfg();
                if(NULL != pSdn)
                {
                    CDbQuantumRestfulCfg cQuantumRestful;
                    int32 nRet = pSdn->QueryQuantum(cQuantumRestful);
                    if(VNET_DB_SUCCESS == nRet)
                    {
                        strSvrIP = cQuantumRestful.GetIp();
                    }
                }

                cout<<"delete Quantum network uuid: "<<itr->GetQuantumNetworkUuid()<<endl;                

                CDBOperateQuantumNetwork *pQuantumNetwork = pMgr->GetIQuantumNetwork();
                if(NULL != pQuantumNetwork)
                {
                    pQuantumNetwork->Del(itr->GetQuantumNetworkUuid().c_str());
                }

                RESTfulDeleteNetwork(strSvrIP, itr->GetQuantumNetworkUuid());

                itr->SetQuantumNetworkUuid("");
            }            
        }

        cVnetVmMem.SetVecVnicMem(vecVNetVnic);

        return 0;
    }

    int32 CLogicNetworkMgr::DeleteQuantumNetwork(const string &cstrLNUuid)
    {
        string strSvrIP("");
        string strQuantumNetwork("");
        string strPGUuid("");

        CVNetDbMgr * pMgr = CVNetDbMgr::GetInstance();
        if(NULL == pMgr)
        {
            VNET_LOG(VNET_LOG_ERROR, "CLogicNetworkMgr::DeleteQuantumNetwork: GetInstance() is NULL.\n");
            return DB_ERROR_GET_INST_FAILED;
        }

        CPortGroupMgr *pPGMgr = CPortGroupMgr::GetInstance();
        if (NULL == pPGMgr)
        {
            VNET_LOG(VNET_LOG_ERROR, "CLogicNetworkMgr::DeleteQuantumNetwork: call CPortGroupMgr::GetInstance() failed.\n");
            return -1;
        }

        GetPortGroupUUID(cstrLNUuid, strPGUuid);

        CPortGroup cPortGroup;
        cPortGroup.SetUuid(strPGUuid);
        if(0 != pPGMgr->GetPGDetail(cPortGroup))
        {
            VNET_LOG(VNET_LOG_ERROR, "CLogicNetworkMgr::DeleteQuantumNetwork: call GetPGDetail(%s) failed.\n", strPGUuid.c_str());
            return -2;
        }

        /* 私有类型网络在撤销虚拟机的时候删除quantum network */
        if(EN_PORTGROUP_TYPE_VM_PRIVATE == cPortGroup.GetPgType())
        {
            return 0;
        }

        CDBOperateSdnCfg * pSdn = pMgr->GetISdnCfg();
        if(NULL != pSdn)
        { 
            CDbQuantumRestfulCfg cQuantumRestful;
            int32 nRet = pSdn->QueryQuantum(cQuantumRestful);
            if(VNET_DB_SUCCESS == nRet)
            {
                strSvrIP = cQuantumRestful.GetIp();
            }
        }

        vector<CDbQuantumNetwork> vecQNInfo;
        CDBOperateQuantumNetwork *pQuantumnetwork = pMgr->GetIQuantumNetwork();
        if(NULL != pQuantumnetwork)
        {
            pQuantumnetwork->QueryAll(vecQNInfo);                        
        }

        vector<CDbQuantumNetwork>::iterator itrQuantumNetwork = vecQNInfo.begin();
        for( ; itrQuantumNetwork != vecQNInfo.end(); ++itrQuantumNetwork)
        {
            // 获取对应逻辑网络对应的Quantum network
            if(cstrLNUuid == itrQuantumNetwork->GetLnUuid())
            {
                strQuantumNetwork = itrQuantumNetwork->GetUuid();
                break;
            }
        }

        if(0 != strQuantumNetwork.size())
        {
            RESTfulDeleteNetwork(strSvrIP, strQuantumNetwork);

            if(NULL != pQuantumnetwork)
            {
                pQuantumnetwork->Del(strQuantumNetwork.c_str());
            }
        }

        return 0;
    }

    int32 CLogicNetworkMgr::RESTfulCreateNetwork(const string &cstrQuantumSvrIP, const string &cstrNetworkName, const int64 cnProjectID, string &QuantumNetworkUuid)
    {
        restrpc_c::clientSimple myClient;
        stringstream strStream;
        bool bIsTrue;

        if(0 == cstrQuantumSvrIP.size() || 0 == cstrNetworkName.size())
        {
            return 0;
        }
            
        string serverUrl("http://");        
        serverUrl += cstrQuantumSvrIP;
        serverUrl += ":9696/v2.0/networks.json";

        cout<<"URL: "<<serverUrl<<endl;

        strStream << cnProjectID;

        value result;
        /* 构造入参，注意JSON最外层是一个struct */
        map<string, value> paramList;
        map<string, value> substructData;

        pair<string, value> mbr_name("name", value_string(cstrNetworkName));
        pair<string, value> mbr_state("admin_state_up", value_boolean(true));
        pair<string, value> mbr_tenantid("tenant_id", value_string(strStream.str()));
        if(0 == cnProjectID)
        {
            bIsTrue = true;
        }
        else
        {
            bIsTrue = false;
        }
        pair<string, value> mbr_shared("shared", value_boolean(bIsTrue));

        substructData.insert(mbr_name);
        substructData.insert(mbr_state);
        substructData.insert(mbr_tenantid);
        substructData.insert(mbr_shared);

        carray arrayData;
        arrayData.push_back(value_struct(substructData));

        value_array array(arrayData);
        pair<string, value> mbr_network("networks",value_array(array));
        paramList.insert(mbr_network);

        myClient.call(serverUrl, HTTP_POST, value_struct(paramList), &result);

        map<string, value> returnValue = value_struct(result);
        value_array networks = value_array(returnValue["networks"]);

        vector<value> dataReadBack(networks.vectorValueValue());
        map<string, value>data = value_struct(dataReadBack[0]);
        
        QuantumNetworkUuid = value_string(data["id"]);

        return 0;
    }

    int32 CLogicNetworkMgr::RESTfulDeleteNetwork(const string &cstrQuantumSvrIP, const string &QuantumNetworkUuid)
    {
        restrpc_c::clientSimple myClient;

        if(0 == cstrQuantumSvrIP.size() || 0 == QuantumNetworkUuid.size())
        {
            return 0;
        }
            
        string serverUrl("http://");        
        serverUrl += cstrQuantumSvrIP;
        serverUrl += ":9696/v2.0/networks/";

        cout<<"URL: "<<serverUrl+QuantumNetworkUuid<<endl;

        value result;
        myClient.call(serverUrl+QuantumNetworkUuid, HTTP_DELETE, &result);

        return 0;
    }
    
    int32 CLogicNetworkMgr::GetPortGroupUUID(const string &cstrLogiNetUUID, string &strPortGroupUUID)
    {
        if(cstrLogiNetUUID.empty())
        {
            VNET_LOG(VNET_LOG_ERROR, "CLogicNetworkMgr::GetPortGroupUUID: cstrLogiNetUUID is empty.\n");
            return -1;
        }
        
        CVNetDbMgr * pMgr = CVNetDbMgr::GetInstance();
        if(NULL == pMgr)
        {
            VNET_LOG(VNET_LOG_ERROR, "CLogicNetworkMgr::GetPortGroupUUID: GetInstance() is NULL.\n");
            return -1;
        }
      
        CDBOperateLogicNetwork * pOperLogicNetwork = pMgr->GetILogicNetwork();
        if(NULL == pOperLogicNetwork)
        {
            VNET_LOG(VNET_LOG_ERROR, "CLogicNetworkMgr::GetPortGroupUUID: GetDbILogicNetwork() is NULL.\n");
            return -1;
        }

        // Query DB;
        CDbLogicNetwork info;
        info.SetUuid(cstrLogiNetUUID.c_str());
        int32 nRet = pOperLogicNetwork->Query(info);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CLogicNetworkMgr::GetPortGroupUUID: DB query logic network (UUID:%s) failed, ret:%d.\n",
                cstrLogiNetUUID.c_str(), nRet);
            return -1;
        }
        strPortGroupUUID = info.GetPgUuid();        
 
        return 0;
    }
    
    int32 CLogicNetworkMgr::GetLogicNetwork(const string &cstrUUID, CLogicNetwork &cLogicNetwork)
    {
        if(cstrUUID.empty())
        {
            VNET_LOG(VNET_LOG_ERROR, "CLogicNetworkMgr::GetLogicNetwork: cstrUUID is empty.\n");
            return -1;
        }
        
        CVNetDbMgr * pMgr = CVNetDbMgr::GetInstance();
        if(NULL == pMgr)
        {
            VNET_LOG(VNET_LOG_ERROR, "CLogicNetworkMgr::GetLogicNetwork: GetInstance() is NULL.\n");
            return -1;
        }
      
        CDBOperateLogicNetwork * pOperLogicNetwork = pMgr->GetILogicNetwork();
        if(NULL == pOperLogicNetwork)
        {
            VNET_LOG(VNET_LOG_ERROR, "CLogicNetworkMgr::GetLogicNetwork: GetDbILogicNetwork() is NULL.\n");
            return -1;
        }

        CDBOperateLogicNetworkIpRange * pOperIPRange = pMgr->GetILogicNetworkIpRange();
        if(NULL == pOperIPRange)
        {        
            VNET_LOG(VNET_LOG_ERROR, "CLogicNetworkMgr::GetLogicNetwork: GetDbILogicNetwork() is NULL.\n");
            return -1;
        }

        // Query DB;
        CDbLogicNetwork info;
        info.SetUuid(cstrUUID.c_str());
        int32 nRet = pOperLogicNetwork->Query(info);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CLogicNetworkMgr::GetLogicNetwork: DB query logic network (UUID:%s) failed, ret:%d.\n",
                cstrUUID.c_str(), nRet);
            return -1;
        }
        vector<CDbLogicNetworkIpRange> outVInfo;
        nRet = pOperIPRange->Query(cstrUUID.c_str(), outVInfo);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CLogicNetworkMgr::GetLogicNetwork: DB query logic network (UUID:%s) IP Range failed, ret:%d.\n",
                cstrUUID.c_str(), nRet);
            return -1;
        }
        
        cLogicNetwork.SetName(info.GetName());
        cLogicNetwork.SetUUID(cstrUUID);
        cLogicNetwork.SetPortGroupUUID(info.GetPgUuid());
        cLogicNetwork.SetDescription(info.GetDes());
        cLogicNetwork.SetIPBootProtoMode(info.GetIpMode());
        if(!outVInfo.empty())
        {
            CIPv4AddrRange &cIPv4AddrRange = cLogicNetwork.GetIPv4AddrRange();
            vector<CDbLogicNetworkIpRange>::iterator itr = outVInfo.begin();
            for(; itr != outVInfo.end(); ++itr)
            {
                TIPV4AddrRangeItem item;
                item.tIPStart.uIP.dwAddr = CIPv4Addr::H2NL((int32)(itr->GetBeginNum()));
                item.tIPStart.uMask.dwAddr = CIPv4Addr::H2NL((int32)(itr->GetMaskNum()));
                item.tIPEnd.uIP.dwAddr = CIPv4Addr::H2NL((int32)(itr->GetEndNum()));
                item.tIPEnd.uMask.dwAddr = item.tIPStart.uMask.dwAddr;
                
                cIPv4AddrRange.AddValidRange(item);
            }
        }
        return 0;
    }

    int32 CLogicNetworkMgr::GetLogiNetByNetplane(const string &cstrNetplaneUUID, vector<CLogicNetwork> &vecLogiNet)
    {
        return 0;
    }
    
    int32 CLogicNetworkMgr::Init(MessageUnit *pMu)
    {
        if (pMu == NULL)
        {
            VNET_LOG(VNET_LOG_ERROR, "CLogicNetworkMgr::Init: pMu is NULL.\n");
            return -1;
        }
        
        m_pMU = pMu;
        
        return 0;
    }
        
    void CLogicNetworkMgr::MessageEntry(const MessageFrame &message)
    {
        switch (message.option.id())
        {
        case EV_VNETLIB_LOGIC_NETWORK_CFG:
            {
                ProcLogicNetworkMsg(message);
            }
            break;            
        case EV_VNETLIB_LOGIC_NETWORK_NAME2UUID:
            {
                ProcLogicNetworkName2UuidMsg(message);
            }
            break;            
        default:
            break;            
        }
        
        return ;
    }
    
    int32 CLogicNetworkMgr::ProcLogicNetworkMsg(const MessageFrame &message)
    {
        if(NULL == m_pMU)
        {
            VNET_ASSERT(0);
            return -1;
        }
        
        CLogicNetworkMsg cMsg;
        cMsg.deserialize(message.data);
        MessageOption option(message.option.sender(), EV_VNETLIB_LOGIC_NETWORK_CFG, 0, 0);
        string strRetInfo;
        int32 nRet = 0;
        
        if(m_bOpenDbgInf)
        {
            cMsg.Print();
        }

        if(!cMsg.IsValidOper())
        {
            VNET_LOG(VNET_LOG_WARN, "CNetplaneMgr::ProcLogicNetworkMsg: Msg operation code (%d) is invalid.\n", 
                cMsg.m_dwOper);
            VNetFormatString(strRetInfo, "Msg operation code (%d) is invalid.\n", cMsg.m_dwOper);
            cMsg.SetResult(LOGIC_NETWORK_CFG_PARAM_ERROR);
            cMsg.SetResultinfo(strRetInfo);
            m_pMU->Send(cMsg, option);
            return -1;
        }
        
        switch(cMsg.m_dwOper)
        {
        case EN_ADD_LOGIC_NETWORK:
            {
                nRet = AddLogicNetwork(cMsg, strRetInfo);
            }
            break;
        case EN_DEL_LOGIC_NETWORK:
            {
                nRet = DelLogicNetwork(cMsg, strRetInfo);
            }
            break;
        case EN_MOD_LOGIC_NETWORK:
            {
                nRet = ModLogicNetwork(cMsg, strRetInfo);
            }
            break;
        default:
            {
                VNET_ASSERT(0);
                strRetInfo = "The logic network config param is invalid.";
                nRet = LOGIC_NETWORK_CFG_PARAM_ERROR;
            }
        }
        
        cMsg.SetResult(nRet);
        cMsg.SetResultinfo(strRetInfo);
        m_pMU->Send(cMsg, option);
        
        return 0;
    }
    
    int32 CLogicNetworkMgr::ProcLogicNetworkName2UuidMsg(const MessageFrame &message)
    {
        if(NULL == m_pMU)
        {
            VNET_ASSERT(0);
            return -1;
        }
        
        CVNetLGNetID2Name cMsg;
        cMsg.deserialize(message.data);
        MessageOption option(message.option.sender(), EV_VNETLIB_LOGIC_NETWORK_NAME2UUID, 0, 0);
        string strRetInfo;
        int32 nRet = 0;
        
        if(m_bOpenDbgInf)
        {
            cMsg.Print();
        }

        cMsg.m_strUuid="";
        nRet = GetLogicNetworkByName(cMsg.m_strLGName,cMsg.m_strUuid,strRetInfo);
                
        cMsg.SetResult(nRet);
        cMsg.SetResultinfo(strRetInfo);
        m_pMU->Send(cMsg, option);
        
        return 0;
    }
    
    int32 CLogicNetworkMgr::GetLogicNetworkByName(const string &strLnName,string & strUuid, string &strRetInfo)
    {
        if(strLnName.empty())
        {
            VNET_ASSERT(0);
            strRetInfo = "The logic network config param is invalid.";
            return LOGIC_NETWORK_CFG_PARAM_ERROR;
        }
              
        CDBOperateLogicNetwork * pOperLogicNetwork = GetDbILogicNetwork();
        if(NULL == pOperLogicNetwork)
        {
            VNET_LOG(VNET_LOG_ERROR, "CLogicNetworkMgr::GetLogicNetworkByName: GetDbILogicNetwork() is NULL.\n");
            strRetInfo = "The logic network database interface is NULL.";
            return LOGIC_NETWORK_GET_DB_INTF_ERROR;
        }

        // Conflict checking
        CDbLogicNetwork info;
        info.SetName(strLnName.c_str());
        int32 nRet = pOperLogicNetwork->QueryByName(info);
        if(VNET_DB_SUCCESS != nRet)
        {
            strRetInfo = "QueryByName the logical network to the database check failed.";
            if(VNET_DB_IS_ITEM_NO_EXIST(nRet))
            {
                return LOGIC_NETWORK_DATA_NOT_EXIST;
            }
            return nRet;
        }

        strUuid = info.GetUuid();
        return LOGIC_NETWORK_OPER_SUCCESS;
    }
    
    int32 CLogicNetworkMgr::AddLogicNetwork(CLogicNetworkMsg &cMsg, string &strRetInfo)
    {
        if(cMsg.m_strPortGroupUUID.empty() ||
           cMsg.m_strName.empty() ||
           !cMsg.IsValidIPBootProtoMode())
        {
            VNET_ASSERT(0);
            strRetInfo = "The logic network config param is invalid.";
            return LOGIC_NETWORK_CFG_PARAM_ERROR;
        }
        
        CVNetDbMgr * pMgr = CVNetDbMgr::GetInstance();
        if(NULL == pMgr)
        {
            VNET_LOG(VNET_LOG_ERROR, "CLogicNetworkMgr::AddLogicNetwork: GetInstance() is NULL.\n");
            strRetInfo = "The database handler is NULL.";
            return DB_ERROR_GET_INST_FAILED;
        }
      
        CDBOperateLogicNetwork * pOperLogicNetwork = pMgr->GetILogicNetwork();
        if(NULL == pOperLogicNetwork)
        {
            VNET_LOG(VNET_LOG_ERROR, "CLogicNetworkMgr::AddLogicNetwork: GetDbILogicNetwork() is NULL.\n");
            strRetInfo = "The logic network database interface is NULL.";
            return LOGIC_NETWORK_GET_DB_INTF_ERROR;
        }       
        
        CPortGroupMgr *pPGMgr = CPortGroupMgr::GetInstance();
        if(NULL == pPGMgr)
        {
            VNET_LOG(VNET_LOG_ERROR, "CLogicNetworkMgr::AddLogicNetwork: CPortGroupMgr::GetInstance is NULL.\n");
            strRetInfo = "CPortGroupMgr::GetInstance database interface is NULL.";
            return LOGIC_NETWORK_GET_DB_INTF_ERROR;            
        } 

        /*检查一下网络平面的id是否合法，可能存在升级后id的值大于10000，此时VXLAN不能正常使用*/
        CPortGroup cPGInfo;
        cPGInfo.m_strUUID = cMsg.m_strPortGroupUUID;        
        if(0 != pPGMgr->GetPGDetail(cPGInfo))
        {   
            VNET_LOG(VNET_LOG_ERROR, "CLogicNetworkMgr::AddLogicNetwork: call<GetPGDetail> failed.\n");
            strRetInfo = "Get switch portgroup's detail failed.";
            return LOGIC_NETWORK_GET_DB_INTF_ERROR;
        }        
        if (ISOLATE_VLAN != cPGInfo.GetIsolateType())
        {
            CNetplaneMgr *pMgr = CNetplaneMgr::GetInstance();
            if (NULL == pMgr)
            {
                VNET_LOG(VNET_LOG_ERROR, "CLogicNetworkMgr::AddLogicNetwork: CNetplaneMgr::GetInstance() fail.\n");
                strRetInfo = "Get netplane instance failed.";
                return DB_ERROR_GET_INST_FAILED;
            }

            if (0!=pMgr->IsValidId(cPGInfo.GetNetplaneUuid()))
            {
                VNET_LOG(VNET_LOG_ERROR, "CLogicNetworkMgr::AddLogicNetwork: CNetplaneMgr::IsValidId() fail.\n");
                strRetInfo = "netplaneid is not valid, maybe greater then 10000.";
                return LOGIC_NETWORK_GET_DB_INTF_ERROR;
            }
        }     

        // Conflict checking
        CDbLogicNetwork info;
        info.SetName(cMsg.m_strName.c_str());
        info.SetPgUuid(cMsg.m_strPortGroupUUID.c_str());
        info.SetDes(cMsg.m_strDescription.c_str());
        info.SetIpMode(cMsg.m_nIPBootProtoMode);
        int32 nRet = pOperLogicNetwork->CheckAdd(info);
        if(VNET_DB_SUCCESS != nRet)
        {
            strRetInfo = "Add the logical network to the database check failed.";
            return nRet;
        }
        
        nRet = pOperLogicNetwork->Add(info);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CLogicNetworkMgr::AddLogicNetwork: Add Logic Network(%s) to db failed. ret = %d.\n", 
                cMsg.m_strName.c_str(), nRet);
            strRetInfo = "Add the logical network to the database failed.";
            return nRet;
        }

        cMsg.m_strUUID = info.GetUuid();

        // 创建quantum网络
        nRet = CreateQuantumNetwork(cMsg, info.GetUuid());
        if(LOGIC_NETWORK_OPER_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CLogicNetworkMgr::AddLogicNetwork: Create Quantum Network(%s) failed. ret = %d.\n", 
                cMsg.m_strName.c_str(), nRet);
            strRetInfo = "Create Quantum network failed.";
            return nRet;
        }

        return LOGIC_NETWORK_OPER_SUCCESS;
    }
    
    int32 CLogicNetworkMgr::DelLogicNetwork(const CLogicNetworkMsg &cMsg, string &strRetInfo)
    {
        if(cMsg.m_strUUID.empty())
        {
            VNET_ASSERT(0);
            strRetInfo = "The deleted logic network ID is NULL.";
            return LOGIC_NETWORK_CFG_PARAM_ERROR;
        }

        CVNetDbMgr * pMgr = CVNetDbMgr::GetInstance();
        if(NULL == pMgr)
        {
            VNET_LOG(VNET_LOG_ERROR, "CLogicNetworkMgr::DelLogicNetwork: GetInstance() is NULL.\n");
            strRetInfo = "The database handler is NULL.";
            return DB_ERROR_GET_INST_FAILED;
        }
  
        CDBOperateLogicNetwork * pOperLogicNetwork = pMgr->GetILogicNetwork();
        if(NULL == pOperLogicNetwork)
        {
            VNET_LOG(VNET_LOG_ERROR, "CLogicNetworkMgr::DelLogicNetwork: GetDbILogicNetwork() is NULL.\n");
            strRetInfo = "The logic network database interface is NULL.";
            return LOGIC_NETWORK_GET_DB_INTF_ERROR;
        }

        // Check record 
        int32 nRet = pOperLogicNetwork->CheckDel(cMsg.m_strUUID.c_str());
        if(VNET_DB_SUCCESS != nRet)
        {
            strRetInfo = "Delete the logical network from the database check failed.";
            return nRet;
        }

        // 删除quantum网络
        nRet = DeleteQuantumNetwork(cMsg.m_strUUID);
        if(LOGIC_NETWORK_OPER_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CLogicNetworkMgr::DelLogicNetwork: Del Quantum Network(%s) failed. ret = %d.\n", 
                cMsg.m_strName.c_str(), nRet);
            strRetInfo = "Delete Quantum network failed.";
            return nRet;
        }

        nRet = pOperLogicNetwork->Del(cMsg.m_strUUID.c_str());
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CNetplaneMgr::DelNetplane: Del the logic network from db failed. ret = %d.\n", 
                nRet);
            strRetInfo = "Delete the logical network from the database failed.";
            return nRet;
        }
        return LOGIC_NETWORK_OPER_SUCCESS;
    }
    
    int32 CLogicNetworkMgr::ModLogicNetwork(const CLogicNetworkMsg &cMsg, string &strRetInfo)
    {
        if(cMsg.m_strUUID.empty() || 
           cMsg.m_strName.empty() ||
           cMsg.m_strPortGroupUUID.empty()||
           !cMsg.IsValidIPBootProtoMode())
        {
            VNET_ASSERT(0);
            strRetInfo = "The logic network config param is invalid.";
            return LOGIC_NETWORK_CFG_PARAM_ERROR;
        }
        CVNetDbMgr * pMgr = CVNetDbMgr::GetInstance();
        if(NULL == pMgr)
        {
            VNET_LOG(VNET_LOG_ERROR, "CNetplaneMgr::ModNetplaneName: GetInstance() is NULL.\n");
            strRetInfo = "The database handler is NULL.";
            return DB_ERROR_GET_INST_FAILED;
        }
        CDBOperateLogicNetwork * pOperLogicNetwork = pMgr->GetILogicNetwork();
        if(NULL == pOperLogicNetwork)
        {
            VNET_LOG(VNET_LOG_ERROR, "CLogicNetworkMgr::ModLogicNetwork: GetDbILogicNetwork() is NULL.\n");
            strRetInfo = "The logic network database interface is NULL.";
            return LOGIC_NETWORK_GET_DB_INTF_ERROR;
        }

        CDbLogicNetwork info;
        info.SetName(cMsg.m_strName.c_str());
        info.SetUuid(cMsg.m_strUUID.c_str());
        info.SetPgUuid(cMsg.m_strPortGroupUUID.c_str());
        info.SetDes(cMsg.m_strDescription.c_str());
        info.SetIpMode(cMsg.m_nIPBootProtoMode);
        // Checked Record 
        // 后续对于与数据库相同的值修改，可以此处判断返回，不需要执行更新操作;
        int32 nRet = pOperLogicNetwork->CheckModify(info);
        if(VNET_DB_SUCCESS != nRet)
        {
            strRetInfo = "Modify the logical network to the database check failed.";
            return nRet;
        }
        
        nRet = pOperLogicNetwork->Modify(info);      
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CNetplaneMgr::ModLogicNetwork: Modify the logic network to db failed. ret = %d.\n", 
                nRet);
            strRetInfo = "Modify the logical network to the database failed.";
            return nRet;
        }       
        return LOGIC_NETWORK_OPER_SUCCESS;
    }
    
    void CLogicNetworkMgr::DbgShowData(const string &cstrPortGroupUUID)
    {
        if(cstrPortGroupUUID.empty())
        {
            return ;
        }
        
        CVNetDbMgr * pMgr = CVNetDbMgr::GetInstance();
        if(NULL == pMgr)
        {
            return ;
        }
        
        CDBOperateLogicNetwork * pOperLogicNetwork = pMgr->GetILogicNetwork();
        if(NULL == pOperLogicNetwork)
        {
            return ;
        }
        
        vector<CDbLogicNetworkPgNetplane> outVInfo;
        int32 nRet = pOperLogicNetwork->QueryByPortGroup(cstrPortGroupUUID.c_str(), outVInfo);
        if(VNET_DB_SUCCESS == nRet)
        {
            vector<CDbLogicNetworkPgNetplane>::iterator itr = outVInfo.begin();
            for( ; itr != outVInfo.end(); ++itr)
            {
                cout << "----------------------------------------------------------------------------" << endl;
                itr->DbgShow();
                cout << "----------------------------------------------------------------------------" << endl;
            }
        }
        
        return ;
    }

    void VNetDbgTestLogicNetworkOper(const char* cstrName, 
                                 const char* cstrUUID, 
                                 const char* cstrDescr, 
                                 const char* cstrPortGroupUUID,
                                 int32 nIPBootProtoMode,
                                 uint32 dwOper)
    {
        if(NULL == cstrName || 
           NULL == cstrDescr ||
           NULL == cstrUUID ||
           NULL == cstrPortGroupUUID)
        {
            return ;
        }

        if((dwOper <= EN_INVALID_LOGIC_NETWORK_OPER) || (dwOper >= EN_LOGIC_NETWORK_OPER_ALL))
        {
            string strError;
            VNetFormatString(strError, "Operation Code: Add[%d], Del[%d], Mod[%d].\n", EN_ADD_LOGIC_NETWORK, EN_DEL_LOGIC_NETWORK, EN_MOD_LOGIC_NETWORK);
            cout << strError;
            return ;
        }

        MessageUnit tempmu(TempUnitName("TestLogicNetworkOper"));
        tempmu.Register();
        CLogicNetworkMsg cMsg;
        string strTmp;
        strTmp.assign(cstrName);
        cMsg.m_strName = strTmp;
        strTmp.clear();
        strTmp.assign(cstrUUID);
        cMsg.m_strUUID = strTmp;
        strTmp.clear();
        strTmp.assign(cstrDescr);
        cMsg.m_strDescription = strTmp;
        strTmp.clear();  
        strTmp.assign(cstrPortGroupUUID);
        cMsg.m_strPortGroupUUID = strTmp;
        strTmp.clear();  
        cMsg.m_nIPBootProtoMode = nIPBootProtoMode;
        cMsg.m_dwOper = dwOper;
        MessageOption option(MU_VNM, EV_VNETLIB_LOGIC_NETWORK_CFG, 0, 0);
        tempmu.Send(cMsg,option);
        return ;
    }
    DEFINE_DEBUG_FUNC(VNetDbgTestLogicNetworkOper);

    void VNetDbgShowLogicNetwork(const char *cstrPortGroupUUID)
    {
        if(NULL == cstrPortGroupUUID)
        {
            return ;
        }
        
        CLogicNetworkMgr *pMgr = CLogicNetworkMgr::GetInstance();
        if(pMgr)
        {
            string strTmp;
            strTmp.assign(cstrPortGroupUUID);
            pMgr->DbgShowData(strTmp);
        }
    }
    DEFINE_DEBUG_FUNC(VNetDbgShowLogicNetwork);

    void VNetDbgSetLogicNetworkPrint(BOOL bOpen)
    {
        CLogicNetworkMgr *pMgr = CLogicNetworkMgr::GetInstance();
        if(pMgr)
        {
            pMgr->SetDbgInfFlg(bOpen);
        }
    }
    DEFINE_DEBUG_FUNC(VNetDbgSetLogicNetworkPrint);

    void VnetDbgRestful(const uint32 nOper, const char* value = NULL, const char* Lnvalue = NULL)
    {
        string strSvrIP = "10.43.168.25";
        string strName  = "xubb";
        string strNetworkUuid = "";
        
        CLogicNetworkMgr *pMgr = CLogicNetworkMgr::GetInstance();
        if(pMgr)
        {
            if(0 == nOper)
            {
                string strUuid = value;                
                pMgr->RESTfulDeleteNetwork(strSvrIP, strUuid);
            }
            else
            {
                CLogicNetworkMsg cMsg;
                cMsg.m_strPortGroupUUID = value;
                cMsg.m_strUUID = Lnvalue;
                cMsg.m_strName = "xubb_quantum";
                
                pMgr->CreateQuantumNetwork(cMsg, cMsg.m_strUUID);
            }
        }        
    }
    DEFINE_DEBUG_FUNC(VnetDbgRestful);
}// end namespace zte_tecs

