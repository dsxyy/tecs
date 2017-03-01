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

#include "vnet_db_netplane.h"
#include "vnet_db_mgr.h"

#include "netplane.h"
#include "netplane_mgr.h"

namespace zte_tecs
{
    CNetplaneMgr *CNetplaneMgr::s_pInstance = NULL;
    
    CNetplaneMgr::CNetplaneMgr()
    {
        int i;
        
        m_pMU = NULL;
        for(i=0; i<NETPLANE_MAX_NUM; i++)
        {
            m_mapNetplaneIdPool.insert(pair<int32, BOOL> (i, false));
        }
        
        m_bOpenDbgInf = VNET_FALSE;
    }
    
    CNetplaneMgr::~CNetplaneMgr()
    {
        m_pMU = NULL;
    }
    
    int32 CNetplaneMgr::Init(MessageUnit *pMu)
    {
        if (pMu == NULL)
        {
            VNET_LOG(VNET_LOG_ERROR, "CNetplaneMgr::Init: pMu is NULL.\n");
            return -1;
        }
        
        m_pMU = pMu;
        
        return 0;
    }
        
    void CNetplaneMgr::MessageEntry(const MessageFrame &message)
    {
        switch (message.option.id())
        {
        case EV_VNETLIB_NETPLANE_CFG:
            {
                ProcNetplaneMsg(message);
            }
            break;            
        default:
            break;            
        }
        
        return ;
    }
    
    int32 CNetplaneMgr::ProcNetplaneMsg(const MessageFrame &message)
    {
        if(NULL == m_pMU)
        {
            VNET_ASSERT(0);
            return -1;
        }
        
        CNetplaneMsg cMsg;
        cMsg.deserialize(message.data);
        MessageOption option(message.option.sender(), EV_VNETLIB_NETPLANE_CFG, 0, 0);
        string strRetInfo;
        int32 nRet = 0;

        if(m_bOpenDbgInf)
        {
            cMsg.Print();
        }
        
        if(!cMsg.IsValidOper())
        {
            VNET_LOG(VNET_LOG_WARN, "CNetplaneMgr::ProcNetplaneMsg: Msg operation code (%d) is invalid.\n", 
                cMsg.m_dwOper);
            VNetFormatString(strRetInfo, "Msg operation code (%d) is invalid.\n", cMsg.m_dwOper);
            cMsg.SetResult(NETPLANE_CFG_PARAM_ERROR);
            cMsg.SetResultinfo(strRetInfo);
            m_pMU->Send(cMsg, option);
            return -1;
        }
        
        switch(cMsg.m_dwOper)
        {
        case EN_ADD_NETPLANE:
            {
                nRet = AddNetplane(cMsg, strRetInfo);
            }
            break;
        case EN_DEL_NETPLANE:
            {
                nRet = DelNetplane(cMsg, strRetInfo);
            }
            break;
        case EN_MOD_NETPLANE:
            {
                nRet = ModNetplane(cMsg, strRetInfo);
            }
            break;
        default:
            {
                VNET_ASSERT(0);
                nRet = NETPLANE_CFG_PARAM_ERROR;
                strRetInfo = "The net plane config param is invalid.";
            }
        }
        
        cMsg.SetResult(nRet);
        cMsg.SetResultinfo(strRetInfo);
        m_pMU->Send(cMsg, option);
        
        return 0;
    }
    
    int32 CNetplaneMgr::AddNetplane(CNetplaneMsg &cMsg, string &strRetInfo)
    {
        CVNetDbMgr * pMgr = CVNetDbMgr::GetInstance();
        if(NULL == pMgr)
        {
            VNET_LOG(VNET_LOG_ERROR, "CNetplaneMgr::AddNetplane: GetInstance() is NULL.\n");
            strRetInfo = "The database handler is NULL.";
            return DB_ERROR_GET_INST_FAILED;
        }
        
        CDBOperateNetplane * pNetplane = pMgr->GetINetplane();
        if(NULL == pNetplane)
        {
            VNET_LOG(VNET_LOG_ERROR, "CNetplaneMgr::AddNetplane: GetINetplane() is NULL.\n");
            strRetInfo = "The net plane database interface is NULL.";
            return NETPLANE_GET_DB_INTF_ERROR;
        }

        // Conflict checking
        vector<CDBObjectBase> outVInfo;
        int32 nRet = pNetplane->QueryNetplaneSummary(outVInfo);
        if(VNET_DB_SUCCESS == nRet)
        {
            vector<CDBObjectBase>::iterator itr = outVInfo.begin();
            for( ; itr != outVInfo.end(); ++itr)
            {
                if(itr->GetName() == cMsg.m_strName)
                {                
                    VNET_LOG(VNET_LOG_ERROR, "CNetplaneMgr::AddNetplane: Netplane(%s) is already exist.\n",
                        cMsg.m_strName.c_str());
                    VNetFormatString(strRetInfo, "The net plane(%s) is already exist.\n", cMsg.m_strName.c_str());
                    return NETPLANE_DATA_ALREADY_EXIST;
                }
            }
        }

        int32 nId;
        if(0!=ApplyNetplaneId(nId))
        {
            VNET_LOG(VNET_LOG_ERROR, "CNetplaneMgr::AddNetplane: Netplane(%s) GetUnuseNetplaneId fail.\n",
                cMsg.m_strName.c_str());
            return NETPLANE_GET_DB_INTF_ERROR;           
        }

        CDbNetplane cDbNetplane;
        cDbNetplane.SetName(cMsg.m_strName.c_str());
        cDbNetplane.SetDesc(cMsg.m_strDescription.c_str());
        cDbNetplane.SetMtu((int32)(cMsg.m_dwMTU));
        cDbNetplane.SetId(nId);
        nRet = pNetplane->AddNetplane(cDbNetplane);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CNetplaneMgr::AddNetplane: Add netplane(%s) to db failed. ret = %d.\n", 
                cMsg.m_strName.c_str(), nRet);
            VNetFormatString(strRetInfo, "Add the net plane(%s) to the database failed.\n", cMsg.m_strName.c_str());
            return nRet;
        }
        
        cMsg.m_strUUID = cDbNetplane.GetUuid();

        return NETPLANE_OPER_SUCCESS;
    }
    
    int32 CNetplaneMgr::DelNetplane(const CNetplaneMsg &cMsg, string &strRetInfo)
    {
        CVNetDbMgr * pMgr = CVNetDbMgr::GetInstance();
        if(NULL == pMgr)
        {
            VNET_LOG(VNET_LOG_ERROR, "CNetplaneMgr::DelNetplane: GetInstance() is NULL.\n");
            strRetInfo = "The database handler is NULL.";
            return NETPLANE_GET_DB_INTF_ERROR;
        }
        
        CDBOperateNetplane * pNetplane = pMgr->GetINetplane();
        if(NULL == pNetplane)
        {
            VNET_LOG(VNET_LOG_ERROR, "CNetplaneMgr::DelNetplane: GetINetplane() is NULL.\n");
            strRetInfo = "The net plane database interface is NULL.";
            return NETPLANE_GET_DB_INTF_ERROR;
        }

        // find record 
        vector<CDBObjectBase> outVInfo;
        int32 bFind = 0;
        int32 nRet = pNetplane->QueryNetplaneSummary(outVInfo);
        if(VNET_DB_SUCCESS == nRet)
        {
            vector<CDBObjectBase>::iterator itr = outVInfo.begin();
            for( ; itr != outVInfo.end(); ++itr)
            {
                if(itr->GetUuid() == cMsg.m_strUUID)
                {
                    bFind = 1;
                    break;
                }
            }
        }
        
        if(0 == bFind)
        {
            VNET_LOG(VNET_LOG_ERROR, "CNetplaneMgr::DelNetplane: Netplane(%s) is not exist.\n",
                cMsg.m_strName.c_str());
            VNetFormatString(strRetInfo, "The net plane(%s) is not exist.\n", cMsg.m_strName.c_str());
            return NETPLANE_DATA_NOT_EXIST;
        }

        nRet = pNetplane->DelNetplane(cMsg.m_strUUID.c_str());
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CNetplaneMgr::DelNetplane: Del netplane from db failed. ret = %d.\n", 
                nRet);
            VNetFormatString(strRetInfo, "Delete the net plane(%s) from the database failed.\n", cMsg.m_strName.c_str());
            return nRet;
        }
        
        return NETPLANE_OPER_SUCCESS;
    }
    
    int32 CNetplaneMgr::ModNetplane(const CNetplaneMsg &cMsg, string &strRetInfo)
    {
        CVNetDbMgr * pMgr = CVNetDbMgr::GetInstance();
        if(NULL == pMgr)
        {
            VNET_LOG(VNET_LOG_ERROR, "CNetplaneMgr::ModNetplane: GetInstance() is NULL.\n");
            strRetInfo = "The database handler is NULL.";
            return NETPLANE_GET_DB_INTF_ERROR;
        }
        
        CDBOperateNetplane * pNetplane = pMgr->GetINetplane();
        if(NULL == pNetplane)
        {
            VNET_LOG(VNET_LOG_ERROR, "CNetplaneMgr::ModNetplane: GetINetplane() is NULL.\n");
            strRetInfo = "The net plane database interface is NULL.";
            return NETPLANE_GET_DB_INTF_ERROR;
        }

        // find record 
        vector<CDBObjectBase> outVInfo;
        int32 bFind = 0;
        int32 nRet = pNetplane->QueryNetplaneSummary(outVInfo);
        if(VNET_DB_SUCCESS == nRet)
        {
            vector<CDBObjectBase>::iterator itr = outVInfo.begin();
            for( ; itr != outVInfo.end(); ++itr)
            {
                if(itr->GetUuid() == cMsg.m_strUUID)
                {
                    bFind = 1;
                    break;
                }
            }
        }
        
        if(0 == bFind)
        {
            VNET_LOG(VNET_LOG_ERROR, "CNetplaneMgr::ModNetplane: Netplane(%s) is not exist.\n",
                cMsg.m_strName.c_str());
            VNetFormatString(strRetInfo, "The net plane(%s) is not exist.\n", cMsg.m_strName.c_str());
            return NETPLANE_DATA_NOT_EXIST;
        }
        
        CDbNetplane cDbNetplane;
        cDbNetplane.SetUuid(cMsg.m_strUUID.c_str());
        nRet = pNetplane->QueryNetplane(cDbNetplane);
        if(VNET_DB_SUCCESS != nRet)
        {
            return nRet;
        }

        int32 bUpdate = 0;
        if((uint32)cDbNetplane.GetMtu() != cMsg.m_dwMTU)
        {
            cDbNetplane.SetMtu(cMsg.m_dwMTU);
            bUpdate = 1;
        }
        if(cDbNetplane.GetName() != cMsg.m_strName)
        {
            cDbNetplane.SetName(cMsg.m_strName.c_str());
            bUpdate = 1;
        }
        if(cDbNetplane.GetDesc() != cMsg.m_strDescription)
        {
            cDbNetplane.SetDesc(cMsg.m_strDescription.c_str());
            bUpdate = 1;
        }
        if(0 == bUpdate)
        {
            return NETPLANE_OPER_SUCCESS;
        }
        
        nRet = pNetplane->ModifyNetplane(cDbNetplane);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNetFormatString(strRetInfo, "modify the net plane(%s) to the database failed.\n", cMsg.m_strName.c_str());
            return nRet;
        }

        return NETPLANE_OPER_SUCCESS;
    }  

    void CNetplaneMgr::DbgShowAllData(void)
    {
        CVNetDbMgr * pMgr = CVNetDbMgr::GetInstance();
        if(NULL == pMgr)
        {
            return ;
        }
        
        CDBOperateNetplane * pNetplane = pMgr->GetINetplane();
        if(NULL == pNetplane)
        {
            return ;
        }

        // find record 
        vector<CDBObjectBase> outVInfo;
        int32 nRet = pNetplane->QueryNetplaneSummary(outVInfo);
        if(VNET_DB_SUCCESS == nRet)
        {
            if(outVInfo.empty())
            {
                return ;
            }
            cout << "----------------------------------------------------------------------------" << endl;
            vector<CDBObjectBase>::iterator itr = outVInfo.begin();
            for( ; itr != outVInfo.end(); ++itr)
            {
                CDbNetplane cDbNetplane;
                cDbNetplane.SetUuid(itr->GetUuid().c_str());
                nRet = pNetplane->QueryNetplane(cDbNetplane);
                if(VNET_DB_SUCCESS == nRet)
                {
                    cout << "UUID: " << setw(32) << cDbNetplane.GetUuid() << " |"
                         << "Name: " << setw(16) << cDbNetplane.GetName() << " |"
                         << "MTU : " << setw(6)  << cDbNetplane.GetMtu()  << endl;
                    cout << "DESCRIPTION: "  << cDbNetplane.GetDesc() << "." << endl;
                    cout << endl;
                }
            }
            cout << "----------------------------------------------------------------------------" << endl;
        }
        
        return ;
    }

    int32 CNetplaneMgr::GetAllNetplane(vector<CNetplane> &vecNetplane)
    {
        CVNetDbMgr * pMgr = CVNetDbMgr::GetInstance();
        if(NULL == pMgr)
        {
            return -1;
        }
        
        CDBOperateNetplane * pNetplane = pMgr->GetINetplane();
        if(NULL == pNetplane)
        {
            return -1;
        }

        // find record 
        vector<CDBObjectBase> outVInfo;
        int32 nRet = pNetplane->QueryNetplaneSummary(outVInfo);
        if(VNET_DB_SUCCESS == nRet)
        {
            if(outVInfo.empty())
            {
                return 0;
            }
            vector<CDBObjectBase>::iterator itr = outVInfo.begin();
            for( ; itr != outVInfo.end(); ++itr)
            {
                CDbNetplane cDbNetplane;
                cDbNetplane.SetUuid(itr->GetUuid().c_str());
                nRet = pNetplane->QueryNetplane(cDbNetplane);
                if(VNET_DB_SUCCESS != nRet)
                {
                    return -1;
                }
                CNetplane cNetplane;
                cNetplane.SetUUID(cDbNetplane.GetUuid());
                cNetplane.SetMTU(cDbNetplane.GetMtu());
                cNetplane.SetDescription(cDbNetplane.GetDesc());
                cNetplane.SetName(cDbNetplane.GetName());
                cNetplane.SetId(cDbNetplane.GetId());
                vecNetplane.push_back(cNetplane);
            }            
        }
        
        return 0;
    }

    int32 CNetplaneMgr::ApplyNetplaneId(int32 &nID)
    {
        vector<CNetplane> vecNetplane;
        int i;

        if(0!=GetAllNetplane(vecNetplane))
        {
            return -1;
        }

        if (vecNetplane.empty())
        {
            nID = 1;
            return 0;
        }

        m_mapNetplaneIdPool.clear();
        for(i=1; i<=NETPLANE_MAX_NUM; i++)
        {
            m_mapNetplaneIdPool.insert(pair<int32, BOOL> (i, false));
        }        
        vector<CNetplane>::iterator it = vecNetplane.begin();
        for( ; it != vecNetplane.end(); ++it)
        {
            map<int32, BOOL>::iterator pos = m_mapNetplaneIdPool.find(it->GetId());
            if (m_mapNetplaneIdPool.end() != pos)
            {
                pos->second = true;
            }
        }

        map<int32, BOOL>::iterator itr = m_mapNetplaneIdPool.begin();
        for( ; itr != m_mapNetplaneIdPool.end(); ++itr)
        {
            if (itr->second==false)
            {
                break;
            }
        }

        if (itr==m_mapNetplaneIdPool.end())
        {
            return -1;
        }

        nID = itr->first;
        VNET_LOG(VNET_LOG_INFO, "CNetplaneMgr::ApplyNetplaneId: OK! nID:%d\n",nID);
        return 0; 

    }
    
    int32 CNetplaneMgr::GetNetplaneMTU(const string &strNetplaneID, int32 &nMTU)
    {
        if(strNetplaneID.empty())
        {
            return -1;
        }
        CDBOperateNetplane * pNetplane = GetDbINetplane();
        if(NULL == pNetplane)
        {
            return -1;
        }
        
        CDbNetplane cDbNetplane;
        cDbNetplane.SetUuid(strNetplaneID.c_str());
        if(VNET_DB_SUCCESS != pNetplane->QueryNetplane(cDbNetplane))
        {
            return -1;
        }
        nMTU = cDbNetplane.GetMtu();
        
        return 0;
    }

    int32 CNetplaneMgr::GetNetplaneId(const string &strNetplaneUuid, int32 &nId)
    {
        if(strNetplaneUuid.empty())
        {
            return -1;
        }
        CDBOperateNetplane * pNetplane = GetDbINetplane();
        if(NULL == pNetplane)
        {
            return -1;
        }
        
        CDbNetplane cDbNetplane;
        cDbNetplane.SetUuid(strNetplaneUuid.c_str());
        if(VNET_DB_SUCCESS != pNetplane->QueryNetplane(cDbNetplane))
        {
            return -1;
        }
        nId = cDbNetplane.GetId();
        
        return 0;
    }
    
    int32 CNetplaneMgr::IsValidId(const string &strNetplaneUuid)
    {
        int32 nId;
        
        if(0!=GetNetplaneId(strNetplaneUuid, nId))
        {
            VNET_LOG(VNET_LOG_ERROR, "CNetplaneMgr::IsValidId: (Netplane %s)GetNetplaneId fail.\n", strNetplaneUuid.c_str());
            return -1;
        }   

        if (nId > NETPLANE_MAX_NUM)
        {
            VNET_LOG(VNET_LOG_ERROR, "CNetplaneMgr::IsValidId: netplane id can not be greater then 10000!.\n");
            return -1;
        }
        
        return 0;
    }

    int32 CNetplaneMgr::ConvertIdToNickName(int32 nId, string &strNickName)
    {
        if(nId > NETPLANE_MAX_NUM)
        {
            return -1;
        }
        strNickName = "";
        map<int32, char> mapTable;
        uint32 i = 0;
        char   c = 'a';
        
        for (i=0; i<26; i++)
        {
            mapTable.insert(pair<int32, char> (i, c+i));
        }
        
        do 
        {
            map<int32, char>::iterator pos = mapTable.find((nId%26));
            if (mapTable.end() == pos)
            {
                return -1;
            }
            strNickName = (char)(pos->second) + strNickName;            
            nId = nId/26;
        }while(nId);
        
        return 0;
    }    

    void VNetDbgTestNetplaneOper(const char* cstrName, 
                                 const char* cstrUUID, 
                                 const char* cstrDescr, 
                                 int32 dwMTU,
                                 int32 dwOper)
    {
        if(NULL == cstrName  || 
           NULL == cstrDescr ||
           NULL == cstrUUID)           
        {
            return ;
        }
        
        if((dwOper <= EN_INVALID_NETPLANE_OPER) || (dwOper >= EN_NETPLANE_OPER_ALL))
        {
            cout << "Operation Code: Add[" << EN_ADD_NETPLANE
                 << "], Del[" << EN_DEL_NETPLANE
                 << "], Mod[" << EN_MOD_NETPLANE
                 << "]." <<endl;
            return ;
        }

        MessageUnit tempmu(TempUnitName("TestNetplaneOper"));
        tempmu.Register();
        CNetplaneMsg cMsg;
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
        cMsg.m_dwMTU = dwMTU;
        cMsg.m_dwOper = dwOper;
        MessageOption option(MU_VNM, EV_VNETLIB_NETPLANE_CFG, 0, 0);
        tempmu.Send(cMsg,option);
        return ;
    }
    DEFINE_DEBUG_FUNC(VNetDbgTestNetplaneOper);

    void VNetDbgShowNetplane(void)
    {
        CNetplaneMgr *pMgr = CNetplaneMgr::GetInstance();
        if(pMgr)
        {
            pMgr->DbgShowAllData();
        }
        return ;
    }
    DEFINE_DEBUG_FUNC(VNetDbgShowNetplane);

    void VNetDbgSetNetplanePrint(BOOL bOpen)
    {
        CNetplaneMgr *pMgr = CNetplaneMgr::GetInstance();
        if(pMgr)
        {
            pMgr->SetDbgInfFlg(bOpen);
        }
        return ;
    }
    DEFINE_DEBUG_FUNC(VNetDbgSetNetplanePrint);

}// end namespace zte_tecs

