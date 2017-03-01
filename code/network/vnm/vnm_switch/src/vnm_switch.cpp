/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vnm_switch.cpp
* 文件标识： 
* 内容摘要：交换管理，操作数据库方法
* 当前版本：1.0
* 作    者：钟春山 
* 完成日期： 
*******************************************************************************/
#include "vnet_error.h"
#include "vnet_db_vswitch.h"
#include "vnet_db_port.h"
#include "vnet_db_quantum_cfg.h"
#include "vnet_db_mgr.h"
#include "vnm_pub.h"
#include "vnet_portmgr.h"
#include "vnet_vnic.h"
#include "vnet_comm.h"
#include "vnetlib_msg.h"
#include "vnet_msg.h"
#include "vnm_switch.h"
#include "vnm_schedule.h"

namespace zte_tecs
{

CSwitch::CSwitch()
{

};  

CSwitch::~CSwitch()
{

}

CDBOperateVSwitch * CVirtualSwitch::GetDBVSwitch()
{
    CVNetDbMgr *pDbMgr = CVNetDbMgr::GetInstance();
    if(NULL == pDbMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::GetDBVSwitch: GetInstance() is NULL.\n");
        return NULL;
    }

    CDBOperateVSwitch * pVSwitch = pDbMgr->GetIVSwitch();
    if(NULL == pVSwitch)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::GetDBVSwitch: GetDbIVSwitch() is NULL.\n");
        return NULL;
    }
    return pVSwitch;
}

CDBOperateVSwitchMap * CVirtualSwitch::GetDBVSwitchMap()
{
    CVNetDbMgr *pDbMgr = CVNetDbMgr::GetInstance();
    if(NULL == pDbMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::GetDBVSwitchMap: GetInstance() is NULL.\n");
        return NULL;
    }

    CDBOperateVSwitchMap * pVSMap = pDbMgr->GetIVSwitchMap();
    if(NULL == pVSMap)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::GetDBVSwitchMap: GetDbIVSwitchMap() is NULL.\n");
        return NULL;
    } 
    return pVSMap;
}

int32 CVirtualSwitch::GetDVSBondMember(const string &strVNA, const string &strBond, int32 &nMode, vector<string> &vecSlaves)
{
    CPortMgr *pCPortMgr = CPortMgr::GetInstance();
    if(NULL == pCPortMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::GetBondMember:CPortMgr::GetInstance() is NULL.\n");   
        return -1;
    }
    
    if(0 != pCPortMgr->GetBondMem(strVNA, strBond, nMode, vecSlaves))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::GetBondMember: GetBondMem <%s> fail.\n", strBond.c_str()); 
        return -1;
    }
    return 0;
}

int32 CVirtualSwitch::IsVectorEqua(vector<string> new_vec, vector<string> old_vec)
{
    if(new_vec.size() != old_vec.size())
    {
        return -1;
    }
    vector<string>::iterator it = new_vec.begin();
    for(; it != new_vec.end(); ++it)
    {
        if(find(old_vec.begin(),old_vec.end(),*it) == old_vec.end())
        {
            return -1;
        }
    }
    return 0;
}

int32 CVirtualSwitch::IsVectorMixed(vector<string> new_vec, vector<string> old_vec)
{
    vector<string>::iterator itNew = new_vec.begin();
    for(; itNew != new_vec.end(); ++itNew)
    {
        if(find(old_vec.begin(),old_vec.end(),*itNew) != old_vec.end())
        {
            //存在交集
            return 0;
        }
    }
    return -1;
}
int32 CVirtualSwitch::GetAllocReqInfo(vector<CVNetVnicMem> &vecVnicCfg, vector<CScheduleInfo> &vecScheduleInfo)
{
    CVNMSchedule *pVNMSchedule = CVNMSchedule::GetInstance();
    if (NULL == pVNMSchedule)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::GetAllocReqInfo: NULL == pVNMSchedule.\n");
        return -1;
    }

    if (SWITCH_OPER_SUCCESS!=pVNMSchedule->CountMtus(vecVnicCfg, vecScheduleInfo))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::GetAllocReqInfo: pVNMSchedule->CountMtus.\n");
        return -1;
    }
    
    return SWITCH_OPER_SUCCESS;
}

//资源分配接口
int32 CVirtualSwitch::GetDVSResource(CVNetVmMem &cVnetVmMem)
{
    vector<CVNetVnicMem> vecVnic;
    cVnetVmMem.GetVecVnicMem(vecVnic);

    //没有配网卡
    if(vecVnic.empty())
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::GetDVSResource: No Exist Vnics.\n");
        return SWITCH_OPER_SUCCESS;
    }
    
    //是否所有网卡已经分配过?
    int32 nFind = 0;
    vector<CVNetVnicMem>::iterator itVnic = vecVnic.begin();
    for(; itVnic != vecVnic.end(); ++itVnic)
    {
        if((itVnic->GetVSwitchUuid()).empty())
        {
            nFind = 1;
            break;
        }
    }
    if(0 == nFind)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::GetDVSResource: Vnics has alloced the vswitch already.\n");
        return SWITCH_OPER_SUCCESS;
    }

    //获取VNA uuid
    CVNAManager * pVNAMgr = CVNAManager::GetInstance();
    if (NULL == pVNAMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::GetDVSResource: call CVNAManager::GetInstance() failed.\n");
        return ERROR_SWITCH_QUERY_VNAUUID_FAIL;
    }
    string strVNAUuid;
    int32 nRet = pVNAMgr->QueryVNAUUID(cVnetVmMem.GetClusterName(), cVnetVmMem.GetHostName(), strVNAUuid);
    if (0 != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::GetDVSResource: call QueryVNAUUID failed, ret=%d.\n", nRet);
        return ERROR_SWITCH_QUERY_VNAUUID_FAIL;
    }
    
    //汇总网卡信息
    vector<CScheduleInfo> vecScheduleInfo; 
    nRet = GetAllocReqInfo(vecVnic, vecScheduleInfo);
    if(SWITCH_OPER_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::GetDVSResource: call GetAllocReqInfo failed.\n");
        return ERROR_SWITCH_MERGE_REQINFO_FAIL;
    }

    CScheduleTaskInfo cSchTaskInfo;
    cSchTaskInfo.SetAppointVNAUuid(strVNAUuid);  
    cSchTaskInfo.SetWatchDogEnable(cVnetVmMem.GetIsHasWatchdog());  
    map< string, set<int32> > mapVnic; //<port, set<index>> 便于统一申请pci号
    vector<CScheduleInfo>::iterator itrSch = vecScheduleInfo.begin();
    for ( ; itrSch != vecScheduleInfo.end(); ++itrSch)
    {
        //一个虚拟网卡申请交换资源
        nRet = itrSch->GetDVSAllocResult(strVNAUuid, cVnetVmMem.GetProjectId(), cSchTaskInfo, mapVnic);  
        if(SCHEDULE_OPER_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::GetDVSResource: call GetDVSAllocResult failed.\n");
            return nRet;
        }
    }

    //SR-IOV资源分配PCI
    if(!mapVnic.empty())
    {
        if(0 != GetSriovVFRes(mapVnic, vecVnic))
        {
            VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::GetDVSResource: call GetSriovVFRes failed.\n");
            return ERROR_SWITCH_GET_SRIOV_VFS_FAIL;
        }
    }

    cVnetVmMem.SetVecVnicMem(vecVnic);
    return SWITCH_OPER_SUCCESS;
}

int32 CVirtualSwitch::GetSriovVFRes(map< string, set<int32> > &mapVnic, vector<CVNetVnicMem> &vecVnic)
{
    CPortMgr *pPortMgr = CPortMgr::GetInstance();
    if(NULL == pPortMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::GetSriovVFRes: CPortMgr::GetInstance() is NULL.\n");
        return -1;
    } 
    
    map< string, set<int32> >::iterator itrMap = mapVnic.begin();
    for(; itrMap != mapVnic.end(); ++itrMap)
    {
        vector<string> vecVFPort; //一起申请多个VF
        if(0 != pPortMgr->GetPhyMultiSriovVF(itrMap->first, (itrMap->second).size() , vecVFPort))
        {
            VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::GetSriovVFRes: GetPhyMultiSriovVF<%s> fail.\n", (itrMap->first).c_str());
            return -1;
        }
        
        //将PCI写到VSI中
        vector<string> vecUsedVF; //已经被使用VF
        set<int32>::iterator itrSet = (itrMap->second).begin();
        for(; itrSet != (itrMap->second).end(); ++itrSet)
        {
            vector<string>::iterator iterVF = vecVFPort.begin();
            for(; iterVF != vecVFPort.end(); ++iterVF)
            {
                if(find(vecUsedVF.begin(), vecUsedVF.end(), *iterVF) == vecUsedVF.end()) 
                {
                    //找到index对应的VSI
                    vector<CVNetVnicMem>::iterator itrVnic = vecVnic.begin();
                    for(; itrVnic != vecVnic.end(); ++itrVnic)
                    {
                        if(itrVnic->GetNicIndex() == *itrSet)
                        {
                            itrVnic->SetSriovVfPortUuid(*iterVF);
                            vecUsedVF.push_back(*iterVF);
                            break;
                        }
                    }
                    //分配下一个index
                    break;
                }
            }
        } 
    }

    return 0;
}

//获取调度详细信息
int32 CVirtualSwitch::GetDVSInfo(const string &strVNA, CVNetVnicDetail &cVNicDetail)
{
    if(cVNicDetail.GetSwitchUuid().empty() || strVNA.empty())
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::GetDVSInfo: GetSwitchUuid() is NULL.\n");
        return -1;
    }
    
    CDBOperateVSwitchMap * pVSMap = GetDBVSwitchMap();
    if(NULL == pVSMap)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::GetDVSInfo: GetDBVSwitchMap() is NULL.\n");
        return -1;
    }

    vector<CDbVSwitchMap> vecVSInfo;
    int32 nRet = pVSMap->QueryBySwitchVna(cVNicDetail.GetSwitchUuid().c_str(), strVNA.c_str(), vecVSInfo);
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::GetDVSInfo: QueryBySwitchVna failed, ret = %d\n", nRet);
        return nRet;
    }

    CPortMgr *pPortMgr = CPortMgr::GetInstance();
    if(NULL == pPortMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::GetDVSInfo CPortMgr::GetInstance() is NULL.\n");
        return -1;
    }
            
    vector<CDbVSwitchMap>::iterator iter = vecVSInfo.begin();
    for(; iter != vecVSInfo.end(); ++iter)
    {
        cVNicDetail.SetSwitchName(iter->GetVSwitchName()); 
        if(cVNicDetail.GetIsSriov())
        {
            string strPCI;
            nRet = pPortMgr->GetPciByVF(cVNicDetail.GetVportUuid(), strPCI);
            if(0 != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::GetDVSInfo GetPciByVF return fail. ret = %d\n", nRet);
                return -1;
            }
            cVNicDetail.SetPciOrder(strPCI);
            cVNicDetail.SetPortName(iter->GetPortName());
        }
        else
        {
            cVNicDetail.SetPortName(iter->GetVSwitchName());
        }
    }
    return 0;
}

int32 CVirtualSwitch::GetSwitchDetail(const string & strSwUuid, const string &strVNAUuid, 
                      vector<CSwitchCfgMsgToVNA> &vecVNAMsg, string &strRetInfo)
{
    int32 nRet = SWITCH_OPER_SUCCESS;
    
    CDBOperateVSwitchMap * pVSMap = GetDBVSwitchMap();
    if(NULL == pVSMap)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::GetSwitchDetail: GetDBVSwitchMap() is NULL.\n");
        VNetFormatString(strRetInfo, "Get the handler of DBVSwitchMap (NULL) failed.");
        return -1;
    }  
    
    CPortMgr *pPortMgr = CPortMgr::GetInstance();
    if(NULL == pPortMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::GetSwitchDetail CPortMgr::GetInstance() is NULL.\n");
        VNetFormatString(strRetInfo, "Get the handler of CPortMgr (NULL) failed.");
        return -1;
    }   
    
    CPortGroupMgr *pPGMgr = CPortGroupMgr::GetInstance();
    if((NULL == pPGMgr))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::GetSwitchDetail CPortGroupMgr::GetInstance() is NULL.\n");
        VNetFormatString(strRetInfo, "Get the handler of CPortGroupMgr (NULL) failed.");
        return -1;
    }   
    
    vector<CDbVSwitchMap> outVInfo;
    if(!strVNAUuid.empty())
    {
        nRet = pVSMap->QueryByVna(strVNAUuid.c_str(), outVInfo);
    }
    else if(!strSwUuid.empty())
    {
        nRet = pVSMap->QueryBySwitch(strSwUuid.c_str(), outVInfo);
    }
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::GetSwitchDetail: Query switch failed<%s, %s>, ret = %d\n",
                  strVNAUuid.c_str(), strSwUuid.c_str(), nRet);
        VNetFormatString(strRetInfo, "Query switch info in database failed.");
        return -1;
    }    

    vector<CDbVSwitchMap>::iterator iter = outVInfo.begin();
    for(;iter != outVInfo.end(); ++iter)
    {
        if(EN_SWITCH_MAP_IS_CFG == iter->GetSwitchMapIsCfg()) //iter->GetSwitchMapIsConsistency() == 0
        {
            CSwitchCfgMsgToVNA cMsg;
            cMsg.m_strUuid     = iter->GetVSwitchUuid();
            cMsg.m_strVnaUuid  = iter->GetVnaUuid();
            
            //查询switch基本信息 
            nRet = QuerySwitch(cMsg);
            if(SWITCH_OPER_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::GetSwitchDetail: QuerySwitch failed, ret = %d\n", nRet);
                VNetFormatString(strRetInfo, "Fail to query switch info!");
                return -1;
            }

            //查询Openflow controller
            nRet = QuerySdnCfg(cMsg);
            if(SWITCH_OPER_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::GetSwitchDetail: QuerySdnCfg failed, ret = %d\n", nRet);
                return -1;
            }

            //获取PG信息
            cMsg.m_cPGInfo.m_strUUID = cMsg.m_strPGUuid;
            nRet = pPGMgr->GetPGDetail(cMsg.m_cPGInfo);
            if(0 != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::GetSwitchDetail: GetPGDetail failed, ret = %d.\n", nRet);
                VNetFormatString(strRetInfo, "Fail to get portgroup info!");
                return -1;        
            }

            //Vxlan属性，生成vtep端口
            if(cMsg.m_cPGInfo.m_nIsolateType)
            {
                nRet = pPortMgr->QueryVtepInfo(cMsg.m_strVnaUuid,  cMsg.m_strUuid, 
                             cMsg.m_strVtepPort, cMsg.m_strVtepIp, cMsg.m_strVtepMask);
                if(0 != nRet)
                {
                    VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::GetSwitchDetail: QueryVtepInfo failed, ret = %d\n", nRet);
                    VNetFormatString(strRetInfo, "Fail to query vtep port info!");
                    return -1;       
                }
            }
            
            //获取端口信息    
            cMsg.m_strSwIf   = iter->GetPortName();
            cMsg.m_nSwIfType = iter->GetPortType();
            if(EN_DB_PORT_TYPE_BOND == iter->GetPortType())
            {
                cMsg.m_strBondName = iter->GetPortName();
                GetDVSBondMember(cMsg.m_strVnaUuid, iter->GetPortName(), cMsg.m_nBondMode, cMsg.m_vecPort);
                pPortMgr->SetBondCfg(iter->GetPortUuid());
            }
            else
            {
                cMsg.m_vecPort.push_back(iter->GetPortName());
            }

            //获取混用端口信息
            vector<CDbVSwitchMap> vecMixInfo;
            nRet = pVSMap->QueryByPort((iter->GetPortUuid()).c_str(), vecMixInfo);
            if(VNET_DB_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::GetSwitchDetail: call QueryByPort failed, ret = %d\n", nRet);
                VNetFormatString(strRetInfo, "Query all switch of port's failed.");
                return -1;
            }  
            vector<CDbVSwitchMap>::iterator itMix = vecMixInfo.begin();
            for(; itMix != vecMixInfo.end(); ++itMix)
            {
                if((vecMixInfo.size() > 1) && (itMix->GetVSwitchUuid() != iter->GetVSwitchUuid()))
                {
                    cMsg.m_strMixedBridge = itMix->GetVSwitchName();
                    cMsg.m_nIsMixed       = 1;  
                }
            }
           
            vecVNAMsg.push_back(cMsg);
            
            //清空配置标记
            nRet = pVSMap->Modify(iter->GetVSwitchUuid().c_str(), iter->GetPortUuid().c_str(), 
                                  EN_SWITCH_MAP_UPDATE_CFG, iter->GetSwitchMapIsSelected(), iter->GetSwitchMapIsConsistency());
            if(VNET_DB_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::GetSwitchDetail: Modify<DVS: %s, PORT:%s>failed, ret = %d\n", 
                          iter->GetVSwitchUuid().c_str(), iter->GetPortUuid().c_str(), nRet);
            }          
        }
    }
        
    return SWITCH_OPER_SUCCESS;
}

int32 CVirtualSwitch::GetNeedCfgSwitch(const string &strSwUuid, const string &strVNAUuid, 
                                         vector<CSwitchCfgMsgToVNA> &vecVNAMsg)
{
    int32 nRet = SWITCH_OPER_SUCCESS;
    
    CDBOperateVSwitchMap * pVSMap = GetDBVSwitchMap();
    if(NULL == pVSMap)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::GetNeedCfgSwitch: GetDBVSwitchMap() is NULL.\n");
        return -1;
    }  
    
    CPortGroupMgr *pPGMgr = CPortGroupMgr::GetInstance();
    if((NULL == pPGMgr))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::GetNeedCfgSwitch CPortGroupMgr::GetInstance().\n");
        return -1;
    }   
    
    CPortMgr *pPortMgr = CPortMgr::GetInstance();
    if(NULL == pPortMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::GetNeedCfgSwitch CPortMgr::GetInstance() is NULL.\n");
        return -1;
    }       

    vector<CDbVSwitchMap> outVInfo;
    if(!strVNAUuid.empty())
    {
        nRet = pVSMap->QueryByVna(strVNAUuid.c_str(), outVInfo);
    }
    else if(!strSwUuid.empty())
    {
        nRet = pVSMap->QueryBySwitch(strSwUuid.c_str(), outVInfo);
    }
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::GetNeedCfgSwitch: Query switch failed<%s, %s>, ret = %d\n",
                  strVNAUuid.c_str(), strSwUuid.c_str(), nRet);
        return nRet;
    }   
    
    vector<CDbVSwitchMap>::iterator iter = outVInfo.begin();
    for(;iter != outVInfo.end(); ++iter)
    {
        if(EN_SWITCH_MAP_UPDATE_CFG == iter->GetSwitchMapIsCfg()) //配置未生效
        {
            CSwitchCfgMsgToVNA cMsg;
            cMsg.m_strUuid    = iter->GetVSwitchUuid();
            cMsg.m_strVnaUuid = iter->GetVnaUuid();
            
            //查询switch基本信息
            nRet = QuerySwitch(cMsg);
            if(SWITCH_OPER_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::GetNeedCfgSwitch: QuerySwitch failed, ret = %d\n", nRet);
                return -1;
            }

            //查询Openflow controller
            nRet = QuerySdnCfg(cMsg);
            if(SWITCH_OPER_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::GetNeedCfgSwitch: QuerySdnCfg failed, ret = %d\n", nRet);
                return nRet;
            }
            
            //获取PG信息
            cMsg.m_cPGInfo.m_strUUID = cMsg.m_strPGUuid;
            nRet = pPGMgr->GetPGDetail(cMsg.m_cPGInfo);
            if(0 != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::GetNeedCfgSwitch: GetPGDetail failed, ret = %d\n", nRet);
                return -1;                
            }
            
            //Vxlan属性
            if(cMsg.m_cPGInfo.m_nIsolateType)
            {
                nRet = pPortMgr->QueryVtepInfo(cMsg.m_strVnaUuid,  cMsg.m_strUuid, 
                     cMsg.m_strVtepPort, cMsg.m_strVtepIp, cMsg.m_strVtepMask);
                if(0 != nRet)
                {
                    VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::GetNeedCfgSwitch: QueryVtepInfo failed, ret = %d\n", nRet);
                    return -1;       
                }
            }           
            
            //获取端口信息
            cMsg.m_strSwIf   = iter->GetPortName();
            cMsg.m_nSwIfType = iter->GetPortType();
            if(EN_DB_PORT_TYPE_BOND == iter->GetPortType())
            {
                cMsg.m_strBondName = iter->GetPortName();
                GetDVSBondMember(cMsg.m_strVnaUuid, iter->GetPortName(), cMsg.m_nBondMode, cMsg.m_vecPort);
            }
            else
            {
                cMsg.m_vecPort.push_back(iter->GetPortName());
            }
            
            //获取混用端口信息
            vector<CDbVSwitchMap> vecMixInfo;
            nRet = pVSMap->QueryByPort((iter->GetPortUuid()).c_str(), vecMixInfo);
            if(VNET_DB_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::GetNeedCfgSwitch: call QueryByPort failed, ret = %d\n", nRet);
                return -1;
            }  
            vector<CDbVSwitchMap>::iterator itMix = vecMixInfo.begin();
            for(; itMix != vecMixInfo.end(); ++itMix)
            {
                if((vecMixInfo.size() > 1) && (itMix->GetVSwitchUuid() != iter->GetVSwitchUuid()))
                {
                    cMsg.m_strMixedBridge = itMix->GetVSwitchName();
                    cMsg.m_nIsMixed       = 1;  
                }
            } 
            
            vecVNAMsg.push_back(cMsg);
        }
    }
        
    return SWITCH_OPER_SUCCESS;
}

//交换是否有主机有环回口
int32 CVirtualSwitch::IsSwitchLoop(const string &strSwUuid, int32 &nIsLoop)
{
    int32 nTmpLoop = 0;
    CDBOperateVSwitchMap * pVSMap = GetDBVSwitchMap();
    if(NULL == pVSMap)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::IsSwitchLoop: GetDBVSwitchMap() is NULL.\n");
        return ERROR_SWITCH_GET_VSWITCHMAP_DB_FAIL;
    }  
    
    CPortMgr *pPortMgr = CPortMgr::GetInstance();
    if(NULL == pPortMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::IsSwitchLoop: CPortMgr::GetInstance() is NULL.\n");   
        return ERROR_SWITCH_GET_PORTMGR_FAIL;
    }  
    
    vector<CDbVSwitchMap> outVInfo;
    int32 nRet = pVSMap->QueryBySwitch(strSwUuid.c_str(), outVInfo);
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::IsSwitchLoop: QueryBySwitch failed, ret = %d\n", nRet);
        return ERROR_SWITCH_OPER_DB_FAIL;
    }

    //交换下还没有端口
    if(outVInfo.empty())
    {
        return ERROR_SWTICH_PORT_INVALID;
    }
    
    vector<CDbVSwitchMap>::iterator iter = outVInfo.begin();
    for(; iter != outVInfo.end(); ++iter)
    {
        if(PORT_TYPE_UPLINKLOOP == iter->GetPortType())
        {
            nTmpLoop = 1;
            break;
        }
        else if(0 == pPortMgr->IsSriovPort(iter->GetVnaUuid(), iter->GetPortName()))
        {
            pPortMgr->IsSriovPortLoop(iter->GetVnaUuid(), iter->GetPortName(), nTmpLoop);
            if(nTmpLoop)
            {
                break;
            }
        }
    }

    nIsLoop = nTmpLoop;
    return SWITCH_OPER_SUCCESS;
}

int32 CVirtualSwitch::IsSwitchUsed(const string &strSwUuid, string &strRetInfo)
{
    CDBOperateVSwitch * pVSwitch = GetDBVSwitch();
    if(NULL == pVSwitch)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::IsSwitchUsed: GetDBVSwitch() is NULL.\n");
        VNetFormatString(strRetInfo, "Get the handler of DBVSwitch (NULL) failed.");
        return -1;
    } 
    
    //虚拟机在运行时，不能删除交换
    int32 nOutVsiNum = 0;
    int32 nRet = pVSwitch->QueryVsiNum(strSwUuid.c_str(), nOutVsiNum);
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::IsSwitchUsed: QueryVsiNum failed, ret = %d\n", nRet);
        VNetFormatString(strRetInfo, "Query the numbers of switch vsi in database failed.");
        return -1;
    }
    if(nOutVsiNum)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::IsSwitchUsed: Switch has been used in VM.\n");
        VNetFormatString(strRetInfo, "Switch(%s) has been used in virtual machine.", strSwUuid.c_str());
        return -1;
    }

    //交换下带有kernel端口，不能删除
    nOutVsiNum = 0;
    nRet = pVSwitch->QueryKernelNum(strSwUuid.c_str(), nOutVsiNum);
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::IsSwitchUsed: QueryKernelNum failed, ret = %d\n", nRet);
        VNetFormatString(strRetInfo, "Query the numbers of kernel of switch in database failed.");
        return -1;
    }
    if(nOutVsiNum)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::IsSwitchUsed: Switch has been used in kernel.\n");
        VNetFormatString(strRetInfo, "Switch(%s) has been used in kernel.", strSwUuid.c_str());
        return -1;
    }

    return 0;    
}

int32 CVirtualSwitch::IsSwitchPortUsed(CSwitchCfgMsgToVNA &cMsg)
{
    CDBOperateVSwitch * pVSwitch = GetDBVSwitch();
    if(NULL == pVSwitch)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::IsSwitchPortUsed: GetDBVSwitch() is NULL.\n");
        VNetFormatString(cMsg.m_strRetInfo, "Get the handler of DBVSwitch (NULL) failed.");
        return -1;
    } 
    
    //虚拟机在运行时，不能添加交换
    int32 nOutVsiNum = 0;
    int32 nRet = pVSwitch->QueryVsiNumBySwitchVna(cMsg.m_strUuid.c_str(), cMsg.m_strVnaUuid.c_str(), nOutVsiNum);
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::IsSwitchPortUsed: QueryVsiNumBySwitchVna failed, ret = %d\n", nRet);
        VNetFormatString(cMsg.m_strRetInfo, "Query the numbers of switch vsi in database failed.");
        return -1;
    }
    if(nOutVsiNum)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::IsSwitchPortUsed: DVS<%s> is used in vm.\n", cMsg.m_strUuid.c_str());
        VNetFormatString(cMsg.m_strRetInfo, "Switch(%s) has been used in virtual machine.", cMsg.m_strUuid.c_str());
        return -1;
    }  
    
    //存在kernel时，不能添加或删除端口
    nOutVsiNum = 0;
    nRet = pVSwitch->QueryKernelNumBySwitchVna(cMsg.m_strUuid.c_str(), cMsg.m_strVnaUuid.c_str(), nOutVsiNum);
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::IsSwitchPortUsed: QueryKernelNumBySwitchVna failed, ret = %d\n", nRet);
        VNetFormatString(cMsg.m_strRetInfo, "Query the numbers of kernel of switch in database failed.");
        return -1;
    }
    if(nOutVsiNum)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::IsSwitchPortUsed: Exist kernel port in DVS<%s>.\n",cMsg.m_strUuid.c_str());
        VNetFormatString(cMsg.m_strRetInfo, "Switch(%s) has been used in kernel.", cMsg.m_strUuid.c_str());
        return -1;
    }
    
    return 0;
}

int32 CVirtualSwitch::IsMixedPortValid(CSwitchCfgMsgToVNA &cMsg)
{
    CDBOperateVSwitch * pVSwitch = GetDBVSwitch();
    if(NULL == pVSwitch)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::IsMixedPortValid: GetDBVSwitch() is NULL.\n");
        VNetFormatString(cMsg.m_strRetInfo, "Get the handler of DBVSwitch (NULL) failed.");
        return -1;
    } 
                        
    CDBOperateVSwitchMap * pVSMap = GetDBVSwitchMap();
    if(NULL == pVSMap)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::IsMixedPortValid: GetDBVSwitchMap() is NULL.\n");
        VNetFormatString(cMsg.m_strRetInfo, "Get the handler of DBVSwitchMap (NULL) failed.");
        return -1;
    }   
    
    CPortMgr *pPortMgr = CPortMgr::GetInstance();
    if(NULL == pPortMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::IsMixedPortValid: CPortMgr::GetInstance() is NULL.\n");   
        VNetFormatString(cMsg.m_strRetInfo, "Get the handler of CPortMgr (NULL) failed.");
        return -1;
    }

    vector<CDbVSwitchMap> outVInfo;
    int32 nRet = pVSMap->QueryByVna(cMsg.m_strVnaUuid.c_str(), outVInfo);
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::IsMixedPortValid: QueryByVna failed, ret = %d\n", nRet);
        VNetFormatString(cMsg.m_strRetInfo, "Query all switch in VNA(%s) failed.", 
                   cMsg.m_strVnaUuid.c_str());
        return -1;
    }  
    
    //待添加端口只有一个
    if(1 == cMsg.m_vecPort.size())
    {
        string strBondName;
        string strPort = cMsg.m_vecPort.front();
        pPortMgr->IsJoinBond(cMsg.m_strVnaUuid, strPort, strBondName);
        if(!strBondName.empty())
        {
            VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::IsMixedPortValid: Port<%s> Join %s.\n", 
                      strPort.c_str(), strBondName.c_str());
            VNetFormatString(cMsg.m_strRetInfo, "Port(%s) has been joined (%s).", 
                      strPort.c_str(), strBondName.c_str());
            return -1;
        }
    }    

    vector<CDbVSwitchMap>::iterator iter;
    for(iter = outVInfo.begin(); iter != outVInfo.end(); ++iter)
    {
        int32 nBondMode = 1; //默认为1，和web保持一致
        vector<string> vecPort;
        if(PORT_TYPE_BOND == iter->GetPortType())
        {
            GetDVSBondMember(cMsg.m_strVnaUuid, iter->GetPortName(), nBondMode, vecPort);
        }
        else
        {
            vecPort.push_back(iter->GetPortName());
        }
            
        //端口加入了相同类型其他交换
        if(0 == IsVectorMixed(cMsg.m_vecPort, vecPort))
        {
            if(iter->GetSwitchType() == cMsg.m_nType)
            {
                if(iter->GetVSwitchUuid() != cMsg.m_strUuid)
                {
                    VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::IsMixedPortValid: Ports has been used.\n");
                    VNetFormatString(cMsg.m_strRetInfo, "Ports have been used for other switch(%s).", 
                               iter->GetVSwitchName().c_str());
                    return -1;
                }
            }
            else
            {
                if(0 != IsVectorEqua(vecPort, cMsg.m_vecPort))
                {
                    //端口匹配
                    VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::IsMixedPortValid: Mixed port cannot match.\n");
                    VNetFormatString(cMsg.m_strRetInfo, "The ports is mixed, but isn't consistent with switch(%s)'s.",
                               iter->GetVSwitchName().c_str());
                    return -1;
                }
                else
                {
                    //bond mode匹配
                    if((cMsg.m_vecPort.size() > 1) && (nBondMode != cMsg.m_nBondMode))
                    {
                        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::IsMixedPortValid: Bond mode cannot match!\n");
                        VNetFormatString(cMsg.m_strRetInfo, "BOND mode of the mixed switch isn't consistent with (%s).",
                                   iter->GetVSwitchName().c_str());
                        return -1;
                    }

                    CDbVSwitch cDbInfo;
                    cDbInfo.SetUuid(iter->GetVSwitchUuid().c_str());
                    nRet = pVSwitch->Query(cDbInfo);
                    if(VNET_DB_SUCCESS != nRet)
                    {
                        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::IsMixedPortValid: Query failed, ret = %d\n", nRet);
                        VNetFormatString(cMsg.m_strRetInfo, "Query switch(%s) information failed.", 
                                   iter->GetVSwitchName().c_str());                        
                        return -1;
                    }     
        
                    int32  nEvbMode  = cDbInfo.GetEvbMode();
                    string strPGUuid = cDbInfo.GetPgUuid();
                    //端口组匹配
                    if(cMsg.m_strPGUuid != strPGUuid)
                    {
                        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::IsMixedPortValid: Current PG<%s> not match mixed DVS<%s>.\n",
                                  cMsg.m_strPGUuid.c_str(), iter->GetVSwitchUuid().c_str());
                        VNetFormatString(cMsg.m_strRetInfo, "PortGroup of the mixed switch isn't consistent with (%s).",
                                   iter->GetVSwitchName().c_str());
                        return -1;
                    }                        
                    //EVB模式匹配
                    if(cMsg.m_nEvbMode != nEvbMode)
                    {
                        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::IsMixedPortValid: Current EVB mode<%d> not match mixed DVS<%s>.\n",
                                  cMsg.m_nEvbMode, iter->GetVSwitchUuid().c_str());
                        VNetFormatString(cMsg.m_strRetInfo, "EVB mode of the mixed switch isn't consistent with (%s).",
                                   iter->GetVSwitchName().c_str());
                        return -1;
                    }
                    if(PORT_TYPE_BOND == iter->GetPortType())
                    {
                        cMsg.m_strBondName    = iter->GetPortName();
                        cMsg.m_strMixedBridge = iter->GetVSwitchName();
                    }
                    cMsg.m_strSwIf   = iter->GetPortName();
                    cMsg.m_nSwIfType = iter->GetPortType();
                    cMsg.m_nIsMixed  = 1;
                    return 0;
                }
            }
        }        
    }          
    
    return 0;
}

//端口有效性检查
int32 CVirtualSwitch::IsSwitchPortValid(CSwitchCfgMsgToVNA &cMsg)
{
    CPortMgr *pPortMgr = CPortMgr::GetInstance();
    if(NULL == pPortMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::IsSwitchPortValid: CPortMgr::GetInstance() is NULL.\n");   
        VNetFormatString(cMsg.m_strRetInfo, "Get the handler of CPortMgr (NULL) failed.");
        return -1;
    }  
    
    if(cMsg.m_vecPort.empty() || (cMsg.m_vecPort.size() > 8))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::IsSwitchPortValid: port is invalid(zero/over eight).\n");  
        VNetFormatString(cMsg.m_strRetInfo, "The numbers of port is invalid.");
        return -1;
    }

    int32  nIsSriov;  //记录第一个端口是否sriov属性，SDVS有效
    int32  nPortType; //记录第一个端口类型 
    int32  nVFNum;    //记录第一个sriov网卡vf个数，EDVS有效
    
    vector<string>::iterator iter = cMsg.m_vecPort.begin();
    for( ; iter != cMsg.m_vecPort.end(); ++iter)
    {
        int32 nTmpPortType = 0;
        int32 nTmpVFNum = 0;
        int32 nTmpIsSriov = 0;
        int32 nIsLoop = 0;
        string strPortUuid;
        
        if(0 != pPortMgr->GetPortType(cMsg.m_strVnaUuid, *iter, nTmpPortType, strPortUuid))
        {
            VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::IsSwitchPortValid: call GetPortType <%s> failed.\n", (*iter).c_str());   
            VNetFormatString(cMsg.m_strRetInfo, "Get the type of port(%s) failed.", (*iter).c_str());
            return -1;
        } 

        if(0 == pPortMgr->IsSriovPort(cMsg.m_strVnaUuid, *iter))
        {   
            //nTmpIsSriov表示是否sriov网卡
            nTmpIsSriov = 1; 
            
            //nIsLoop表示是否sriov loop
            if(0 != pPortMgr->IsSriovPortLoop(cMsg.m_strVnaUuid, *iter, nIsLoop))
            {
                VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::IsSwitchPortValid: call IsSriovPortLoop <%s> failed.\n",(*iter).c_str());
                VNetFormatString(cMsg.m_strRetInfo, "Get sriov port(%s)'s attribute of loopback failed.", (*iter).c_str());
                return -1;
            }
            
            //nTmpVFNum表示VF个数
            if(0 != pPortMgr->GetSriovPortVfNum(cMsg.m_strVnaUuid, *iter, nTmpPortType, nTmpVFNum))
            {
                VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::IsSwitchPortValid: call GetSriovPortVfNum <%s> failed.\n",(*iter).c_str()); 
                VNetFormatString(cMsg.m_strRetInfo, "Get sriov port(%s)'s vf numbers failed.", (*iter).c_str());
                return -1;
            }  
        }
            
        if(EMB_VIR_SWITCH_TYPE == cMsg.m_nType)
        {   
            //是否SR-IOV网卡
            if(1 != nTmpIsSriov)
            {
                VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::IsSwitchPortValid: the port<%s> is not support sr-iov.\n", (*iter).c_str()); 
                VNetFormatString(cMsg.m_strRetInfo, "Port(%s) isn't support SR-IOV.", (*iter).c_str());
                return -1;
            }
        }
        else if(SOFT_VIR_SWITCH_TYPE == cMsg.m_nType)
        {
            //端口类型检查
            if((PORT_TYPE_PHYSICAL != nTmpPortType) && (PORT_TYPE_UPLINKLOOP != nTmpPortType))
            {
                VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::IsSwitchPortValid: Port<%s--%d> is not physical/loop port.\n", (*iter).c_str(), nTmpPortType); 
                VNetFormatString(cMsg.m_strRetInfo, "Port(%s) isn't the physical or loop port.", (*iter).c_str());
                return -1;
            }
            
            if(PORT_TYPE_UPLINKLOOP == nTmpPortType)
            {
                nIsLoop = 1;
            }
            
            //环回SR-IOV网卡不能再加入SDVS
            if(nTmpIsSriov && nIsLoop)
            {
                VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::IsSwitchPortValid: SR-IOV Loop port<%s> can't join SDVS.\n",(*iter).c_str());
                VNetFormatString(cMsg.m_strRetInfo, "Port(%s) of loopback in SR-IOV dont join sdvs.", (*iter).c_str());
                return -1;
            } 
        }
        
        //环回口只能有一个网卡
        if(nIsLoop && (1 != cMsg.m_vecPort.size()))
        {
            VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::IsSwitchPortValid: Exist multi-port in loopback mode, failed.\n");  
            VNetFormatString(cMsg.m_strRetInfo, "Dont join multi-port in loopback mode.");
            return -1;
        }
        
        //DVS上VNA必须都为环回
        int32 nSwLoop = 0;
        if(SWITCH_OPER_SUCCESS == IsSwitchLoop(cMsg.m_strUuid, nSwLoop))
        {
            if(nSwLoop != nIsLoop)
            {
                VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::IsSwitchPortValid: all VNA switch port should be in loopmode.\n");  
                VNetFormatString(cMsg.m_strRetInfo, "Make sure switchs of all VNA is in loopback mode.");
                return -1;
            }
        }

        //设置初始值
        if(iter == cMsg.m_vecPort.begin())
        {
            nPortType = nTmpPortType;
            nIsSriov  = nTmpIsSriov;
            nVFNum    = nTmpVFNum;
        }
        else
        {
            //端口类型一致性检查
            if(nPortType != nTmpPortType)
            {
                VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::IsSwitchPortValid: Port type not match.\n"); 
                VNetFormatString(cMsg.m_strRetInfo, "The type of ports isn't consistent.");
                return -1;
            }
            //bond成员口不能同时包含sriov和非sriov
            if(nIsSriov != nTmpIsSriov)
            {
                VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::IsSwitchPortValid: Bond slaves type is different.\n"); 
                VNetFormatString(cMsg.m_strRetInfo, "BOND slaves's type isn't consistent.");
                return -1;
            }
            //SR-IOV网卡VF个数一致性检查
            if(nVFNum != nTmpVFNum)
            {
                VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::IsSwitchPortValid: SR-IOV vf nums is unequal.\n"); 
                VNetFormatString(cMsg.m_strRetInfo, "The numbers of VF(SR-IOV) isn't consistent.");
                return -1;
            }
        }
    }

    //暂存端口类型
    cMsg.m_nSwIfType = nPortType;

    //端口混用检查
    if(0 != IsMixedPortValid(cMsg))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::IsSwitchPortValid: call IsMixedPortValid failed.\n");   
        return -1;
    }
    
    return 0;
}

int32 CVirtualSwitch::CanSwitchAdd(CSwitchCfgMsgToVNA &cMsg)
{
    CDBOperateVSwitch * pVSwitch = GetDBVSwitch();
    if(NULL == pVSwitch)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::CanSwitchAdd: GetDBVSwitch() is NULL.\n");
        return ERROR_SWITCH_GET_VSWITCH_DB_FAIL;
    } 
    
    //交换是否被创建过?
    vector<CDbSwitchSummary> outVInfo;
    int32 nRet = pVSwitch->QuerySummary(outVInfo);
    if(VNET_DB_SUCCESS == nRet)
    {   
        vector<CDbSwitchSummary>::iterator iter = outVInfo.begin();
        for( ; iter != outVInfo.end(); ++iter)
        {
            if((cMsg.m_strName == iter->GetName())&&(cMsg.m_nType == iter->GetSwitchType()))
            {
                VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::CanSwitchAdd: Switch(%s) is already exist.\n",
                    cMsg.m_strName.c_str());
                cMsg.m_strUuid = iter->GetUuid();
                return ERROR_SWITCH_ALREADY_EXIST;
            }
        }
    }  

    return SWITCH_OPER_SUCCESS;
}

/*******************************************************************************
* 函数名称： CanSwitchDel
* 功能描述： 检查switch是否可以被删除
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhongchsh     创 建
*******************************************************************************/
int32 CVirtualSwitch::CanSwitchDel(const string &strSwUuid, vector<CSwitchCfgMsgToVNA> &vecVNAMsg, string &strRetInfo)
{
    CDBOperateVSwitch * pVSwitch = GetDBVSwitch();
    if(NULL == pVSwitch)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::CanSwitchDel: GetDBVSwitch() is NULL.\n");
        VNetFormatString(strRetInfo, "GetDBVSwitch() is NULL.");
        return ERROR_SWITCH_GET_VSWITCH_DB_FAIL;
    } 
    
    CDBOperateVSwitchMap * pVSMap = GetDBVSwitchMap();
    if(NULL == pVSMap)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::CanSwitchDel: GetDBVSwitchMap() is NULL.\n");
        VNetFormatString(strRetInfo, "GetDBVSwitchMap() is NULL.");
        return ERROR_SWITCH_GET_VSWITCHMAP_DB_FAIL;
    }    
    
    int32 nRet = pVSwitch->CheckDel(strSwUuid.c_str());
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::CanSwitchDel: CheckDel failed, ret = %d\n", nRet);
        VNetFormatString(strRetInfo, "Check that if switch can delete in database failed.");
        return ERROR_SWITCH_OPER_DB_FAIL;
    }
    
    nRet = IsSwitchUsed(strSwUuid, strRetInfo);
    if(0 != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::CanSwitchDel: call <IsSwitchUsed> failed.\n");  
        return ERROR_SWITCH_IS_USING;
    }
    
    vector<CDbVSwitchMap> outVInfo;
    nRet = pVSMap->QueryBySwitch(strSwUuid.c_str(), outVInfo);
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::CanSwitchDel: QueryBySwitch failed, ret = %d\n", nRet);
        VNetFormatString(strRetInfo, "Query switch info in database failed.");
        return ERROR_SWITCH_OPER_DB_FAIL;
    }     

    CSwitchMgr *pSwitchMgr = CSwitchMgr::GetInstance();
    if(NULL==pSwitchMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::CanSwitchDel: pSwitchMgr is NULL\n");
        VNetFormatString(strRetInfo, "Get the handler of pSwitchMgr (NULL) failed.");
        return ERROR_SWITCH_INSTANCE_FAIL;
    }   

    CPortMgr *pPortMgr = CPortMgr::GetInstance();
    if(NULL == pPortMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::DelSwitchPortMap: CPortMgr::GetInstance() is NULL.\n");   
        VNetFormatString(strRetInfo, "Get the handler of CPortMgr (NULL) failed.");
        return ERROR_SWITCH_GET_PORTMGR_FAIL;
    }         

    vector<CDbVSwitchMap>::iterator iter = outVInfo.begin();
    for(;iter != outVInfo.end(); ++iter)
    {
        CSwitchCfgMsgToVNA cDVSCfg;
        cDVSCfg.m_strName = iter->GetVSwitchName();
        cDVSCfg.m_strUuid = iter->GetVSwitchUuid();
        cDVSCfg.m_nType   = iter->GetSwitchType();
        cDVSCfg.m_strVnaUuid = iter->GetVnaUuid();

        /* 获取PG信息 */
        nRet = pSwitchMgr->GetSwitchPGInfo(cDVSCfg.m_strName, cDVSCfg.m_cPGInfo);
        if(0 != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::CanSwitchDel: GetSwitchPGInfo failed, switch name %s, ret = %d\n", 
                cDVSCfg.m_strName.c_str(), nRet);
            VNetFormatString(strRetInfo, "GetSwitchPGInfo failed.");
            return ERROR_SWITCH_OPER_DB_FAIL;
        }
        cDVSCfg.m_strPGUuid = cDVSCfg.m_cPGInfo.m_strUUID;

        /* Vxlan属性，生成vtep端口*/
        if(cDVSCfg.m_cPGInfo.m_nIsolateType)
        {
            nRet = pPortMgr->QueryVtepInfo(cDVSCfg.m_strVnaUuid,  cDVSCfg.m_strUuid, 
                         cDVSCfg.m_strVtepPort, cDVSCfg.m_strVtepIp, cDVSCfg.m_strVtepMask);
            if(0 != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::CanSwitchDel: QueryVtepInfo failed, ret = %d\n", nRet);
                VNetFormatString(strRetInfo, "Fail to query vtep port info!");
                return ERROR_SWITCH_OPER_DB_FAIL;       
            }
        }
        
        int32 nBondMode = 0;
        if(PORT_TYPE_BOND == iter->GetPortType())
        {
            cDVSCfg.m_strBondName = iter->GetPortName();
            GetDVSBondMember(iter->GetVnaUuid(), iter->GetPortName(), nBondMode, cDVSCfg.m_vecPort);
        }
        else
        {
            cDVSCfg.m_vecPort.push_back(iter->GetPortName());
        }
        vecVNAMsg.push_back(cDVSCfg);
    }    
    return SWITCH_OPER_SUCCESS;
}

int32 CVirtualSwitch::CanSwitchPortAdd(CSwitchCfgMsgToVNA &cMsg)
{
    //交换被虚拟机占用或被kernel占用
    int32 nRet = IsSwitchPortUsed(cMsg);
    if(0 != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::CanSwitchPortAdd: call <IsSwitchPortUsed> failed, ret = %d.\n", nRet);  
        return ERROR_SWITCH_IS_USING;
    }   
    
    //端口有效性检查
    nRet = IsSwitchPortValid(cMsg);
    if(0 != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::CanSwitchPortAdd: call <IsSwitchPortValid> failed, ret = %d.\n", nRet);  
        return ERROR_SWTICH_PORT_INVALID;
    }   

    return SWITCH_OPER_SUCCESS;
}

//端口删除检查，混用时要一起删除
int32 CVirtualSwitch::CanSwitchPortDel(CSwitchCfgMsgToVNA &cMsg)
{
    CDBOperateVSwitchMap * pVSMap = GetDBVSwitchMap();
    if(NULL == pVSMap)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::CanSwitchPortDel: GetDBVSwitchMap() is NULL.\n");
        VNetFormatString(cMsg.m_strRetInfo, "Get the handler of DBVSwitchMap (NULL) failed.");
        return ERROR_SWITCH_GET_VSWITCHMAP_DB_FAIL;
    }   
    
    int32 nRet = IsSwitchPortUsed(cMsg);
    if(0 != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::CanSwitchPortDel: call <IsSwitchPortUsed> failed.\n");  
        return ERROR_SWITCH_IS_USING;
    }
    
    vector<CDbVSwitchMap> outVInfo;
    nRet = pVSMap->QueryByVna(cMsg.m_strVnaUuid.c_str(), outVInfo);
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::CanSwitchPortDel: QueryByVna failed, ret = %d\n", nRet);
        VNetFormatString(cMsg.m_strRetInfo, "Query all switch info in VNA(%s) failed.", cMsg.m_strVnaUuid.c_str());
        return ERROR_SWITCH_PORT_OPER_DB_FAIL;
    }  

    string strBondPort; //当前交换使用的bond口
    vector<string> vecPort; //当前交换加入的所有物理口
    map<string, string> mapSwitchPort; //VNA下所有的交换加入的端口
    vector<CDbVSwitchMap>::iterator iter = outVInfo.begin();
    for(; iter != outVInfo.end(); ++iter)
    {
        if(iter->GetVSwitchUuid() == cMsg.m_strUuid) 
        {
            if(0 == iter->GetSwitchMapIsConsistency())
            {
                cMsg.m_nIsOffline = 1; //离线标记
            }
            if(PORT_TYPE_BOND == iter->GetPortType())
            {
                strBondPort = iter->GetPortUuid();
                int32 nBondMode = 0;
                GetDVSBondMember(cMsg.m_strVnaUuid, iter->GetPortName(), nBondMode, vecPort);
                cMsg.m_strBondName = iter->GetPortName(); //保存bond name
                cMsg.m_nBondMode   = nBondMode;
            }
            continue;
        }

        //收集VNA下交换端口，不包含当前交换
        mapSwitchPort.insert(make_pair(iter->GetVSwitchName(), iter->GetPortUuid()));
    }

    //存在端口(bond)混用，必须所有成员口一起删除
    map<string, string>::iterator itMapPort = mapSwitchPort.begin();
    for(; itMapPort != mapSwitchPort.end(); ++itMapPort)
    {
        if(itMapPort->second == strBondPort) //bond混用
        {
            if(0 != IsVectorEqua(vecPort, cMsg.m_vecPort))
            {
                VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::CanSwitchPortDel: call IsVectorEqua failed.\n");
                VNetFormatString(cMsg.m_strRetInfo, "Mixed ports must delete together.");
                return ERROR_SWITCH_PORT_ISMIXED;
            }
            cMsg.m_nIsMixed = 1;//设置混用标记
            cMsg.m_strMixedBridge = itMapPort->first;
        }
    }
    
    return SWITCH_OPER_SUCCESS;
}

void CVirtualSwitch::AddReportSwitch(const string &strVNAUuid, CSwitchReportInfo& cReport)
{
    CDBOperateVSwitch * pVSwitch = GetDBVSwitch();
    if(NULL == pVSwitch)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::AddReportSwitch: GetDBVSwitch() is NULL.\n");
        return;
    } 

    CDBOperateVSwitchMap * pVSMap = GetDBVSwitchMap();
    if(NULL == pVSMap)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::AddReportSwitch: GetDBVSwitchMap() is NULL.\n");
        return;
    } 
    
    CPortMgr *pPortMgr = CPortMgr::GetInstance();
    if(NULL == pPortMgr)
    {   
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::AddReportSwitch: CPortMgr::GetInstance() is NULL.\n");
        return;
    }
    
    //没有端口的交换不入库
    if(cReport.m_strSwIf.empty())
    {
        return;
    }
    
    vector<CDbSwitchSummary> outVInfo;
    int32 nRet = pVSwitch->QuerySummary(outVInfo);
    if(VNET_DB_SUCCESS == nRet)
    {   
        vector<CDbSwitchSummary>::iterator iter = outVInfo.begin();  
        for( ; iter != outVInfo.end(); ++iter)
        {
            //交换表存在记录
            if((cReport.m_strSwName == iter->GetName()) && (cReport.m_nSwType == iter->GetSwitchType()))
            {
                //map表不存在，不入库
                vector <CDbVSwitchMap> vecDBMap;
                nRet = pVSMap->QueryBySwitchVna(iter->GetUuid().c_str(), strVNAUuid.c_str(), vecDBMap);
                if((VNET_DB_SUCCESS != nRet) || vecDBMap.empty())
                {
                    continue;
                }
                
                //获取端口UUID失败，不入库
                int32  nPortType;
                string strPortUuid;
                nRet = pPortMgr->GetPortType(strVNAUuid, cReport.m_strSwIf, nPortType, strPortUuid);
                if(nRet || strPortUuid.empty())
                {
                    VNET_LOG(VNET_LOG_INFO, "CVirtualSwitch::AddReportSwitch: call GetPortType failed, nRet = %d.\n", nRet);
                    continue;
                }
                
                /* 存在记录，比较交换端口是否一致: 
                   * 上报交换端口和数据库一致，更新IsSelected和IsConsistency标记；
                   * 上报交换端口和数据库不一致，清空数据库IsSelected和IsConsistency标记；
                   * !!不会新增任何条目；
                */
                if(strPortUuid == vecDBMap.front().GetPortUuid())
                {
                    if(vecDBMap.front().GetSwitchMapIsCfg())
                    {
                        nRet = pVSMap->Modify(iter->GetUuid().c_str(), strPortUuid.c_str(), vecDBMap.front().GetSwitchMapIsCfg(), 1, 1);
                    }
                    else
                    {
                        nRet = pVSMap->Modify(iter->GetUuid().c_str(), strPortUuid.c_str(), vecDBMap.front().GetSwitchMapIsCfg(), 1, 0);
                    }  
                }
                else
                {
                    nRet = pVSMap->Modify(iter->GetUuid().c_str(), strPortUuid.c_str(), vecDBMap.front().GetSwitchMapIsCfg(), 0, 0);
                }
                if(VNET_DB_SUCCESS != nRet)
                {
                    VNET_LOG(VNET_LOG_INFO, "CVirtualSwitch::AddReportSwitch: Modify failed, ret = %d\n", nRet);
                }
            }
        }
    }
}

int32 CVirtualSwitch::CheckDVSParam(CSwitchCfgMsg &cMsg, string &strRetInfo)
{
    //名字长度不超过64
    if(cMsg.m_strName.length() > MAX_SWITCH_NAME_LEN)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::CheckDVSParam switch name <%d> is invalid.\n", cMsg.m_strName.c_str());
        VNetFormatString(strRetInfo, "Switch's name(%s) is invalid.", cMsg.m_strName.c_str());
        return -1;
    }

    //EVB模式检查
    if((cMsg.m_nEvbMode > EN_EVB_MODE_VEPA) && (cMsg.m_nEvbMode < EN_EVB_MODE_NORMAL))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::CheckDVSParam EVB Mode<%d> is invalid.\n", cMsg.m_nEvbMode);
        VNetFormatString(strRetInfo, "Switch's EVB mode(%s) is invalid.", cMsg.m_nEvbMode);
        return -1;        
    }
    
    //端口数检查
    if((cMsg.m_nType == SOFT_VIR_SWITCH_TYPE) && (cMsg.m_nMaxVnic > MAX_SWITCH_VNIC_NUMS))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::CheckDVSParam MaxVnic<%d> overruns 1024.\n", cMsg.m_nMaxVnic);
        VNetFormatString(strRetInfo, "SDVS maxvnics(%s) is invalid.", cMsg.m_nMaxVnic);
        return -1;        
    }

    CPortGroupMgr *pPGMgr = CPortGroupMgr::GetInstance();
    if((NULL == pPGMgr) || cMsg.m_strPGUuid.empty())
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::CheckDVSParam CPortGroupMgr::GetInstance() is NULL.\n");
        VNetFormatString(strRetInfo, "Get the handler of CPortGroupMgr (%s) failed.", cMsg.m_strPGUuid.c_str());
        return -1;
    }    

    CPortGroup cPGInfo;
    cPGInfo.m_strUUID = cMsg.m_strPGUuid;
    if(0 != pPGMgr->GetPGDetail(cPGInfo))
    {   
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::CheckDVSParam: call<GetPGDetail> failed.\n");
        VNetFormatString(strRetInfo, "Get switch portgroup's detail failed.");
        return -1;
    }  

    //PG类型必须为UPLINK PG
    if(EN_PORTGROUP_TYPE_UPLINK != cPGInfo.GetPgType())
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::CheckDVSParam PG<%s> is not UplinkPG.\n", cMsg.m_strPGUuid.c_str());
        VNetFormatString(strRetInfo, "Switch's portgroup(%s) is not uplink pg.", cMsg.m_strPGUuid.c_str());
        return -1;
    }      

    /*检查一下网络平面的id是否合法，可能存在升级后id的值大于10000，此时VXLAN不能正常使用*/    
    if (ISOLATE_VLAN != cPGInfo.GetIsolateType())
    {
        //EDVS暂不支持vxlan
        if(SOFT_VIR_SWITCH_TYPE != cMsg.m_nType)
        {
            VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::CheckDVSParam: not support vxlan in edvs.\n");
            VNetFormatString(strRetInfo, "not support vxlan in edvs.");
            return -1;
        }
        
        CNetplaneMgr *pMgr = CNetplaneMgr::GetInstance();
        if (NULL == pMgr)
        {
            VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::CheckDVSParam: CNetplaneMgr::GetInstance() fail.\n");
            VNetFormatString(strRetInfo, "Get netplane instance failed.");
            return -1;
        }

        if (0!=pMgr->IsValidId(cPGInfo.GetNetplaneUuid()))
        {
            VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::CheckDVSParam: CNetplaneMgr::IsValidId() fail.\n");
            VNetFormatString(strRetInfo, "netplaneid is not valid, maybe greater then 10000.");
            return -1;            
        }
    }
    
    return 0;
}

/*******************************************************************************
* 函数名称： AddSwitch
* 功能描述： 向数据库中添加交换条目
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhongchsh     创 建
*******************************************************************************/
int32 CVirtualSwitch::AddSwitch(CSwitchCfgMsg& cMsg, string &strRetInfo)
{
    //入参检查
    int32 nRet = CheckDVSParam(cMsg, strRetInfo);
    if(0 != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::AddSwitch: CheckDVSParam <%s> failed.\n", cMsg.m_strName.c_str());
        return ERROR_SWITCH_CHECK_PARAM_FAIL;
    }

    CDBOperateVSwitch * pVSwitch = GetDBVSwitch();
    if(NULL == pVSwitch)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::AddSwitch: GetDBVSwitch() is NULL.\n");
        VNetFormatString(strRetInfo, "Get the handler of DBVSwitch failed.");
        return ERROR_SWITCH_GET_VSWITCH_DB_FAIL;
    } 

    vector<CDbSwitchSummary> outVInfo;
    nRet = pVSwitch->QuerySummary(outVInfo);
    if(VNET_DB_SUCCESS == nRet)
    {   
        vector<CDbSwitchSummary>::iterator iter = outVInfo.begin();
        for( ; iter != outVInfo.end(); ++iter)
        {
            if(cMsg.m_strName == iter->GetName())
            {
                cMsg.m_strUuid = iter->GetUuid();
                VNetFormatString(strRetInfo, "Switch(%s) is already exist.",cMsg.m_strName.c_str());
                return ERROR_SWITCH_ALREADY_EXIST;
            }
        }
    }
    
    CDbVSwitch cDbVSInfo;
    cDbVSInfo.SetSwitchType(cMsg.m_nType);
    cDbVSInfo.SetName(cMsg.m_strName.c_str());
    cDbVSInfo.SetState(1);
    cDbVSInfo.SetMaxMtu(cMsg.m_nMTU);
    cDbVSInfo.SetEvbMode(cMsg.m_nEvbMode);
    cDbVSInfo.SetPgUuid(cMsg.m_strPGUuid.c_str());
    cDbVSInfo.SetNicMaxNumCfg(cMsg.m_nMaxVnic);
    cDbVSInfo.SetIsActive(1);

    CPortGroupMgr *pPGMgr = CPortGroupMgr::GetInstance();
    if (NULL != pPGMgr)
    {
        if(TRUE == pPGMgr->IsPortGroupSdn(cMsg.m_strPGUuid))
        {
            CVNetDbMgr * pMgr = CVNetDbMgr::GetInstance();
            if(NULL != pMgr)
            {
                CDBOperateSdnCfg * pSdn = pMgr->GetISdnCfg();
                if(NULL != pSdn)
                {
                    CDbSdnCtrlCfg cSdn;
                    if(VNET_DB_SUCCESS == pSdn->QuerySdnCtrl(cSdn))
                    {
                        cDbVSInfo.SetSdnCfgUuid(cSdn.GetUuid().c_str());
                    }
                }
            }
        }
    }

    nRet = pVSwitch->Add(cDbVSInfo);
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::AddSwitch: Add <%s> to DB failed, ret = %d\n",cMsg.m_strName.c_str(), nRet);
        VNetFormatString(strRetInfo, "Add switch(%s) to database failed.", cMsg.m_strName.c_str());
        return ERROR_SWITCH_PORT_OPER_DB_FAIL;
    }
    cMsg.m_strUuid = cDbVSInfo.GetUuid();
    
    return SWITCH_OPER_SUCCESS;
}

/*******************************************************************************
* 函数名称： DelSwitch
* 功能描述： 删除数据库中条目
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhongchsh     创 建
*******************************************************************************/
int32 CVirtualSwitch::DelSwitch(const string &strUuid, string &strRetInfo)
{
    CDBOperateVSwitch * pVSwitch = GetDBVSwitch();
    if(NULL == pVSwitch)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::DelSwitch: GetDBVSwitch() is NULL.\n");
        VNetFormatString(strRetInfo, "Get the handler of DBVSwitch failed.");
        return ERROR_SWITCH_GET_VSWITCH_DB_FAIL;
    } 

    int32 nRet = pVSwitch->Del(strUuid.c_str());
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::DelSwitch: Del switch<%s> failed. ret = %d\n", strUuid.c_str(), nRet);
        VNetFormatString(strRetInfo, "Delete switch(%s) in database failed.", strUuid.c_str());
        return ERROR_SWITCH_PORT_OPER_DB_FAIL;
    }
    
    return SWITCH_OPER_SUCCESS;
}

/*******************************************************************************
* 函数名称： DelSwitchPortMap
* 功能描述： 删除交换端口表
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhongchsh     创 建
*******************************************************************************/
int32 CVirtualSwitch::DelSwitchPortMap(CSwitchCfgMsgToVNA& cMsg)
{
    CDBOperateVSwitchMap * pVSMap = GetDBVSwitchMap();
    if(NULL == pVSMap)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::DelSwitchPortMap: GetDBVSwitchMap() is NULL.\n");
        VNetFormatString(cMsg.m_strRetInfo, "Get the handler of DBVSwitchMap (NULL) failed.");
        return ERROR_SWITCH_GET_VSWITCHMAP_DB_FAIL;
    }
    
    CPortMgr *pPortMgr = CPortMgr::GetInstance();
    if(NULL == pPortMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::DelSwitchPortMap: CPortMgr::GetInstance() is NULL.\n");   
        VNetFormatString(cMsg.m_strRetInfo, "Get the handler of CPortMgr (NULL) failed.");
        return ERROR_SWITCH_GET_PORTMGR_FAIL;
    } 
            
    vector<CDbVSwitchMap> outVInfo;
    int32 nRet = pVSMap->QueryBySwitchVna(cMsg.m_strUuid.c_str(), cMsg.m_strVnaUuid.c_str(), outVInfo);
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::DelSwitchPortMap: QueryBySwitchVna failed, ret = %d\n", nRet);
        VNetFormatString(cMsg.m_strRetInfo, "Query switch(%s) info by vna failed.",cMsg.m_strUuid.c_str());
        return ERROR_SWITCH_PORT_OPER_DB_FAIL;
    }

    vector<CDbVSwitchMap>::iterator iter = outVInfo.begin();
    for(;iter != outVInfo.end(); ++iter)
    {
        if(iter->GetVSwitchUuid() == cMsg.m_strUuid)
        {
            int32  nPortType   = iter->GetPortType();
            string strPortName = iter->GetPortName();

            //先删除dvs map表            
            nRet = pVSMap->Del(cMsg.m_strUuid.c_str(), iter->GetPortUuid().c_str());
            if(VNET_DB_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::DelSwitchPortMap: Del<DVS: %s, Port: %s> failed, ret = %d\n",
                                       cMsg.m_strUuid.c_str(), iter->GetPortName().c_str(), nRet);
                VNetFormatString(cMsg.m_strRetInfo, "Delete switch_port map failed.");
                return ERROR_SWITCH_PORT_OPER_DB_FAIL;
            } 
            //类型是bond，解除bond关系
            if((0 == cMsg.m_nIsMixed) && (PORT_TYPE_BOND == nPortType))
            {
                nRet = pPortMgr->DelBond2Db(cMsg.m_strVnaUuid, strPortName);
                if(0 != nRet)
                {
                    VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::DelSwitchPortMap: DelBond2Db<%s> failed.\n", strPortName.c_str());  
                    VNetFormatString(cMsg.m_strRetInfo, "Delete bond(%s) map failed.", strPortName.c_str());
                    return ERROR_SWITCH_PORT_OPER_DB_FAIL;
                }  
            }            
        }
    }
    
    return SWITCH_OPER_SUCCESS;
}

/*******************************************************************************
* 函数名称： ModifySwitch
* 功能描述： 修改交换属性
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhongchsh     创 建
*******************************************************************************/
int32 CVirtualSwitch::ModifySwitch(CSwitchCfgMsg& cMsg, string &strRetInfo)
{
    CDBOperateVSwitch * pVSwitch = GetDBVSwitch();
    if(NULL == pVSwitch)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::ModifySwitch: GetDBVSwitch() is NULL.\n");
        VNetFormatString(strRetInfo, "Get the handler of DBVSwitch failed.");
        return ERROR_SWITCH_GET_VSWITCH_DB_FAIL;
    } 

    CDBOperateVSwitchMap * pVSMap = GetDBVSwitchMap();
    if(NULL == pVSMap)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::ModifySwitch: GetDBVSwitchMap() is NULL.\n");
        VNetFormatString(strRetInfo, "Get the handler of DBVSwitchMap (NULL) failed.");
        return ERROR_SWITCH_GET_VSWITCHMAP_DB_FAIL;
    }
    
    CPortMgr *pPortMgr = CPortMgr::GetInstance();
    if(NULL == pPortMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::ModifySwitch: CPortMgr::GetInstance() is NULL.\n");   
        VNetFormatString(strRetInfo, "Get the handler of DBVSwitchMap (NULL) failed.");
        return ERROR_SWITCH_GET_PORTMGR_FAIL;
    }       
    
    CPortGroupMgr *pPGMgr = CPortGroupMgr::GetInstance();
    if((NULL == pPGMgr))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::ModifySwitch CPortGroupMgr::GetInstance() is NULL.\n");
        VNetFormatString(strRetInfo, "Get the handler of CPortGroupMgr (NULL) failed.");
        return ERROR_SWITCH_MODIFY_FAIL;
    }       

    //入参检查
    int32 nRet = CheckDVSParam(cMsg, strRetInfo);
    if(0 != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::ModifySwitch: CheckDVSParam <%s> failed.\n", cMsg.m_strName.c_str());
        return ERROR_SWITCH_CHECK_PARAM_FAIL;
    }    

    CDbVSwitch cDbVSInfo;
    cDbVSInfo.SetUuid(cMsg.m_strUuid.c_str());
    nRet = pVSwitch->Query(cDbVSInfo);
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::ModifySwitch: Query return failed, ret = %d\n", nRet);
        VNetFormatString(strRetInfo, "Query vswitch(%s) info in database failed.", cMsg.m_strUuid.c_str());
        return ERROR_SWITCH_OPER_DB_FAIL;
    }

    BOOL bFind = FALSE;
    if(cDbVSInfo.GetSwitchType() != cMsg.m_nType)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::ModifySwitch: Don't modify switch type! \n");
        VNetFormatString(strRetInfo, "Don't modify switch's type.");
        return ERROR_SWITCH_CHECK_PARAM_FAIL;
    }
    if(cDbVSInfo.GetName() != cMsg.m_strName)
    {   
        cDbVSInfo.SetName(cMsg.m_strName.c_str());
        bFind = TRUE;
    }
    if(cDbVSInfo.GetEvbMode() != cMsg.m_nEvbMode)
    {
        cDbVSInfo.SetEvbMode(cMsg.m_nEvbMode);
        bFind = TRUE;
    }
    if(cDbVSInfo.GetNicMaxNumCfg() != cMsg.m_nMaxVnic)
    {
        if(SOFT_VIR_SWITCH_TYPE == cDbVSInfo.GetSwitchType())
        {
            cDbVSInfo.SetNicMaxNumCfg(cMsg.m_nMaxVnic);
            bFind = TRUE;
        }
    }
    if(cDbVSInfo.GetPgUuid() != cMsg.m_strPGUuid)
    {
        //端口混用时不能修改PG
       // cDbVSInfo.SetPgUuid(cMsg.m_strPGUuid.c_str());
        bFind = TRUE;
    }
    //暂不支持修改
    //cDbVSInfo.SetMaxMtu(cMsg.m_nMTU);
    //cDbVSInfo.SetState(1); 
    //cDbVSInfo.SetIsActive(1);
    
    if(bFind)
    {
        nRet = pVSwitch->CheckModify(cDbVSInfo);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::ModifySwitch: CheckModify return failed, ret = %d\n", nRet);
            VNetFormatString(strRetInfo, "Check that if switch can modify in database failed.");
            return ERROR_SWITCH_MODIFY_FAIL;
        }

        //是否被虚拟机使用
        int32 nOutVsiNum = 0;
        nRet = pVSwitch->QueryVsiNum(cMsg.m_strUuid.c_str(), nOutVsiNum);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::ModifySwitch: QueryVsiNum failed, ret = %d\n", nRet);
            VNetFormatString(strRetInfo, "Query switch's vsi in database failed.");
            return ERROR_SWITCH_MODIFY_FAIL;
        }
        if(nOutVsiNum)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::ModifySwitch: DVS(%s) is used in vm.\n", cMsg.m_strUuid.c_str());
            VNetFormatString(strRetInfo, "Fail to modify, because switch has been used in virtual machine.");
            return ERROR_SWITCH_IS_USING;
        }   

        if(cDbVSInfo.GetPgUuid() != cMsg.m_strPGUuid)
        {
            //PG变化，如果之前交换是VXLAN的，那么就要把原来的vtep删除，其他情况保留。
            int32 nOldIsolateType;
            nRet = pPGMgr->GetPGIsolateType(cDbVSInfo.GetPgUuid(), nOldIsolateType);
            if(0 != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::ModifySwitch: GetPGIsolateType failed, ret = %d\n", nRet);
                VNetFormatString(strRetInfo, "Fail to get isolate type of old portgroup!");
                return ERROR_SWITCH_MODIFY_FAIL;                
            }     
            
            int32 nNewIsolateType;
            nRet = pPGMgr->GetPGIsolateType(cMsg.m_strPGUuid, nNewIsolateType);
            if(0 != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::ModifySwitch: GetPGIsolateType failed, ret = %d\n", nRet);
                VNetFormatString(strRetInfo, "Fail to get isolate type of new portgroup!");
                return ERROR_SWITCH_MODIFY_FAIL;                
            }     
            
            vector<CDbVSwitchMap> vecVSwMap;
            vector<CDbVSwitchMap>::iterator itMap;
            nRet = pVSMap->QueryBySwitch(cMsg.m_strUuid.c_str(), vecVSwMap);
            if(0 != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::ModifySwitch: QueryBySwitch failed, ret = %d\n", nRet);
                VNetFormatString(strRetInfo, "Fail to query switch port info!");
                return ERROR_SWITCH_MODIFY_FAIL;                
            }
                
            if(EN_ISOLATE_TYPE_VLAN != nOldIsolateType)
            {
                for(itMap = vecVSwMap.begin(); itMap != vecVSwMap.end(); ++itMap)
                {
                    string strVtep;
                    string strIP;
                    string strMask;
                    nRet = pPortMgr->QueryVtepInfo(itMap->GetVnaUuid(), cMsg.m_strUuid, strVtep, strIP, strMask);
                    if(0 == nRet)
                    {
                        //删除老的Vtep信息
                        nRet = pPortMgr->DelVtepPort(itMap->GetVnaUuid(), strVtep);
                        if(0 != nRet)
                        {
                            VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::ModifySwitch: DelVtepPort<%s> failed.\n", strVtep.c_str());
                            VNetFormatString(strRetInfo, "Fail to delete vtep port in database!");
                            return ERROR_SWITCH_MODIFY_FAIL;
                        }       
                    }
                }
            }
            
            if(EN_ISOLATE_TYPE_VLAN != nNewIsolateType)
            {
                for(itMap = vecVSwMap.begin(); itMap != vecVSwMap.end(); ++itMap)
                {
                    string strVtep;
                    string strIP;
                    string strMask;

                    //创建新的Vtep端口
                    nRet = pPortMgr->GetVtepInfo(itMap->GetVnaUuid(), cMsg.m_strUuid, cMsg.m_strPGUuid, strVtep, strIP, strMask);
                    if(0 != nRet)
                    {
                        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::ModifySwitch: GetVtepInfo failed, ret = %d.\n", nRet);
                        VNetFormatString(strRetInfo, "Fail to create vtep port info!");
                        return ERROR_SWITCH_MODIFY_FAIL;    
                    }
                }
            }

            cDbVSInfo.SetPgUuid(cMsg.m_strPGUuid.c_str());
        }
    
        nRet = pVSwitch->Modify(cDbVSInfo);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::ModifySwitch: Modify return failed, ret = %d\n", nRet);
            VNetFormatString(strRetInfo, "Modify the switch in database failed.");
            return ERROR_SWITCH_OPER_DB_FAIL;
        }    
    }

    return SWITCH_OPER_SUCCESS;
}

int32 CVirtualSwitch::ModifySwitch(CSwitchCfgMsgToVNA &cMsg)
{
    CDBOperateVSwitchMap * pVSMap = GetDBVSwitchMap();
    if(NULL == pVSMap)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::ModifySwitch: GetDBVSwitchMap() is NULL.\n");
        VNetFormatString(cMsg.m_strRetInfo, "Get the handler of DBVSwitchMap failed.");
        return ERROR_SWITCH_PORT_OPER_DB_FAIL;
    } 
    
    CPortMgr *pPortMgr = CPortMgr::GetInstance();
    if(NULL == pPortMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::ModifySwitch: CPortMgr::GetInstance() is NULL.\n");   
        VNetFormatString(cMsg.m_strRetInfo, "Get the handler of CPortMgr failed.");
        return ERROR_SWITCH_GET_PORTMGR_FAIL;
    }    
    //vxlan vtep port 前面已经删除了不存在vtep，这里不需要再删除
    if(cMsg.m_cPGInfo.m_nIsolateType)
    {
        if(0 != pPortMgr->SetVtepCfg(cMsg.m_strVnaUuid, cMsg.m_strVtepPort))
        {
            VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::ModifySwitch: SetVtepCfg<%s> failed.\n", cMsg.m_strVtepPort.c_str());
            VNetFormatString(cMsg.m_strRetInfo, "Fail to set vtep config flag.");
            return ERROR_SWITCH_PORT_OPER_DB_FAIL;
        }
    }   

    vector<CDbVSwitchMap> outVInfo;
    int32 nRet = pVSMap->QueryBySwitchVna(cMsg.m_strUuid.c_str(), cMsg.m_strVnaUuid.c_str(), outVInfo);
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::ModifySwitch: QueryBySwitchVna failed, ret = %d\n", nRet);
        VNetFormatString(cMsg.m_strRetInfo, "Query switch info by vna failed.");
        return ERROR_SWITCH_PORT_OPER_DB_FAIL;
    }  
    
    vector<CDbVSwitchMap>::iterator iter = outVInfo.begin();
    for(; iter != outVInfo.end(); ++iter)
    {
        if(EN_SWITCH_MAP_UPDATE_CFG == iter->GetSwitchMapIsCfg())
        {
            nRet = pVSMap->Modify(iter->GetVSwitchUuid().c_str(), iter->GetPortUuid().c_str(), EN_SWITCH_MAP_IS_CFG, 
                                  iter->GetSwitchMapIsSelected(),iter->GetSwitchMapIsConsistency());
            if(VNET_DB_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::ModifySwitch: Modify DB failed, ret = %d\n", nRet);
                VNetFormatString(cMsg.m_strRetInfo, "Modify the map of switchport failed.");
                return ERROR_SWITCH_PORT_OPER_DB_FAIL;
            }             
        }
    }
    return SWITCH_OPER_SUCCESS;
}

int32 CVirtualSwitch::QuerySwitch(CSwitchCfgMsgToVNA& cMsg)
{
    CDBOperateVSwitch * pVSwitch = GetDBVSwitch();
    if(NULL == pVSwitch)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::QuerySwitch: GetDBVSwitch() is NULL.\n");
        VNetFormatString(cMsg.m_strRetInfo, "Get the handler of DBVSwitch failed.");
        return ERROR_SWITCH_OPER_DB_FAIL;
    } 

    CDbVSwitch cDbVSInfo;
    cDbVSInfo.SetUuid(cMsg.m_strUuid.c_str());
    int32 nRet = pVSwitch->Query(cDbVSInfo);
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::QuerySwitch: Query switchDB failed, ret = %d\n", nRet);
        VNetFormatString(cMsg.m_strRetInfo, "Query switch(%s) info failed.", cMsg.m_strUuid.c_str());
        return ERROR_SWITCH_OPER_DB_FAIL;
    }
    
    cMsg.m_strName   = cDbVSInfo.GetName();
    cMsg.m_nType     = cDbVSInfo.GetSwitchType();
    cMsg.m_nMTU      = cDbVSInfo.GetMaxMtu();
    cMsg.m_nEvbMode  = cDbVSInfo.GetEvbMode();
    cMsg.m_nMaxVnic  = cDbVSInfo.GetNicMaxNumCfg();
    cMsg.m_strPGUuid = cDbVSInfo.GetPgUuid();
    cMsg.m_strSdnCfgUuid = cDbVSInfo.GetSdnCfgUuid();
    return SWITCH_OPER_SUCCESS;
}

int32 CVirtualSwitch::QuerySdnCfg(CSwitchCfgMsgToVNA &cMsg)
{
    CVNetDbMgr * pMgr = CVNetDbMgr::GetInstance();
    if(NULL == pMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::QuerySdnCfg: GetInstance() is NULL.\n");
        return DB_ERROR_GET_INST_FAILED;
    }

    CDBOperateSdnCfg * pSdn = pMgr->GetISdnCfg();
    if(NULL != pSdn)
    {
        CDbSdnCtrlCfg cSdn;
        int32 nRet = pSdn->QuerySdnCtrl(cSdn);
        if(VNET_DB_SUCCESS == nRet)
        {
            cMsg.m_strProto = cSdn.GetProtocolType();
            cMsg.m_strIP    = cSdn.GetIp();
            cMsg.m_nPort    = cSdn.GetPort();
        }
    }

    return SWITCH_OPER_SUCCESS;
}

//添加交换端口
int32 CVirtualSwitch::AddSwitchPort(CSwitchCfgMsgToVNA &cMsg)
{
    CDBOperateVSwitchMap * pVSMap = GetDBVSwitchMap();
    if(NULL == pVSMap)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::AddSwitchPort: GetDBVSwitchMap() is NULL.\n");
        VNetFormatString(cMsg.m_strRetInfo, "Get the handler of DBVSwitchMap (NULL) failed.");
        return ERROR_SWITCH_GET_VSWITCHMAP_DB_FAIL;
    } 

    CPortMgr *pPortMgr = CPortMgr::GetInstance();
    if(NULL == pPortMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::AddSwitchPort: CPortMgr::GetInstance() is NULL.\n");   
        VNetFormatString(cMsg.m_strRetInfo, "Get the handler of CPortMgr (NULL) failed.");
        return ERROR_SWITCH_GET_PORTMGR_FAIL;
    }

    vector<CDbVSwitchMap> outVInfo;
    int32 nRet = pVSMap->QueryBySwitchVna(cMsg.m_strUuid.c_str(), cMsg.m_strVnaUuid.c_str(), outVInfo);
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::AddSwitchPort: QueryBySwitchVna failed, ret = %d\n", nRet);
        VNetFormatString(cMsg.m_strRetInfo, "Query switch info by vna in database failed.");
        return ERROR_SWITCH_PORT_OPER_DB_FAIL;
    }  
    
    //vxlan vtep port
    if(cMsg.m_cPGInfo.m_nIsolateType)
    {
        if(0 != pPortMgr->SetVtepCfg(cMsg.m_strVnaUuid, cMsg.m_strVtepPort))
        {
            VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::AddSwitchPort: SetVtepCfg<%s> failed.\n", cMsg.m_strVtepPort.c_str());
            VNetFormatString(cMsg.m_strRetInfo, "Set vtep port's state failed.");
            return ERROR_SWITCH_PORT_OPER_DB_FAIL;
        }
    }    
    
    string strTmpUuid;
    if(cMsg.m_nSwIfType == EN_DB_PORT_TYPE_BOND)
    {
        nRet = pPortMgr->GetPortUUID(cMsg.m_strVnaUuid, cMsg.m_strSwIf, cMsg.m_nSwIfType, strTmpUuid);
    }
    else
    {
        nRet = pPortMgr->GetPortType(cMsg.m_strVnaUuid, cMsg.m_strSwIf, cMsg.m_nSwIfType, strTmpUuid);
    }
    if(0 != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::AddSwitchPort: GetPortUUID failed, ret = %d\n", nRet);
        VNetFormatString(cMsg.m_strRetInfo, "Get port(%s) info failed.", cMsg.m_strSwIf.c_str());
        return ERROR_SWITCH_GET_PORTINFO_FAIL;
    }
    //strTmpUuid=NULL表示bond没有入库
    
    vector<CDbVSwitchMap>::iterator iter = outVInfo.begin();
    for(; iter != outVInfo.end(); ++iter)
    {
        if(iter->GetPortUuid() == strTmpUuid)
        {
            //数据库已经存在，修改三个标记
            if(iter->GetSwitchMapIsCfg()) //不会有0的情况?
            {
                if(iter->GetSwitchType() == EMB_VIR_SWITCH_TYPE)
                {
                    nRet = pVSMap->Modify(iter->GetVSwitchUuid().c_str(), 
                                          iter->GetPortUuid().c_str(), 
                                          EN_SWITCH_MAP_IS_CFG, 
                                          iter->GetSwitchMapIsSelected(), 
                                          1);
                }
                else
                {
                    nRet = pVSMap->Modify(iter->GetVSwitchUuid().c_str(), 
                                          iter->GetPortUuid().c_str(), 
                                          EN_SWITCH_MAP_IS_CFG, 
                                          1, 
                                          1);
                }
                if(VNET_DB_SUCCESS != nRet)
                {
                    VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::AddSwitchPort: Modify DB <DVS: %s, PORT: %s> failed, ret = %d\n", 
                              iter->GetVSwitchUuid().c_str(),iter->GetPortUuid().c_str(), nRet);
                    VNetFormatString(cMsg.m_strRetInfo, "Modify the existed map<DVS: %s, port: %s> failed.", 
                              iter->GetVSwitchUuid().c_str(),iter->GetPortUuid().c_str());
                    return ERROR_SWITCH_PORT_OPER_DB_FAIL;
                } 
            }
            //更新bond db
            if(iter->GetPortType() == EN_DB_PORT_TYPE_BOND)
            {
                if(0 != pPortMgr->AddBond2Db(cMsg.m_strVnaUuid, cMsg.m_strBondName, cMsg.m_vecPort, cMsg.m_nBondMode, strTmpUuid))
                {
                    VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::AddSwitchPort: AddBond2Db<%s> failed.\n", cMsg.m_strBondName.c_str());
                    VNetFormatString(cMsg.m_strRetInfo, "Add bond(%s) to database failed.", cMsg.m_strBondName.c_str());
                    return ERROR_SWITCH_PORT_OPER_DB_FAIL;
                }     
            }    
            return SWITCH_OPER_SUCCESS;
        }
        else
        {
            //数据库不匹配，先删除老数据
            nRet = pVSMap->Del(iter->GetVSwitchUuid().c_str(), iter->GetPortUuid().c_str());
            if(VNET_DB_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::AddSwitchPort: Del DB<DVS: %s, PORT: %s> failed, ret = %d\n", 
                            iter->GetVSwitchUuid().c_str(), iter->GetPortUuid().c_str(), nRet);
                VNetFormatString(cMsg.m_strRetInfo, "Delete the old data in database failed.");
                return ERROR_SWITCH_PORT_OPER_DB_FAIL;
            }              
        }
    }

    if((!cMsg.m_strBondName.empty())&&(cMsg.m_vecPort.size() != 1))
    {
        if(0 != pPortMgr->AddBond2Db(cMsg.m_strVnaUuid, cMsg.m_strBondName, cMsg.m_vecPort, cMsg.m_nBondMode, strTmpUuid))
        {
            VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::AddSwitchPort: AddBond2Db<%s, mode:%d> failed.\n", 
                      cMsg.m_strBondName.c_str(), cMsg.m_nBondMode);
            VNetFormatString(cMsg.m_strRetInfo, "Add bond(%s) to database failed.", cMsg.m_strBondName.c_str());
            return ERROR_SWITCH_PORT_OPER_DB_FAIL;
        }     
    }

    //直接添加
    if(EMB_VIR_SWITCH_TYPE == cMsg.m_nType)
    {
        nRet = pVSMap->Add(cMsg.m_strUuid.c_str(), strTmpUuid.c_str(), 1, 0, 1); //sriov不上报, consist=1
    }
    else
    {
        nRet = pVSMap->Add(cMsg.m_strUuid.c_str(), strTmpUuid.c_str(), 1, 1, 1);
    }
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::AddSwitchPort: Add DB failed, ret = %d\n", nRet);
        VNetFormatString(cMsg.m_strRetInfo, "Add switchport map<DVS:%s, port:%s> to database failed.",
                cMsg.m_strUuid.c_str(), strTmpUuid.c_str());
        return ERROR_SWITCH_PORT_OPER_DB_FAIL;
    }     

    return SWITCH_OPER_SUCCESS;
}

/*******************************************************************************
* 函数名称： DelSwitchPort
* 功能描述： 删除主机端口
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhongchsh     创 建
*******************************************************************************/
int32 CVirtualSwitch::DelSwitchPort(CSwitchCfgMsgToVNA &cMsg)
{
    CDBOperateVSwitchMap * pVSMap = GetDBVSwitchMap();
    if(NULL == pVSMap)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::DelSwitchPort: GetDBVSwitchMap() is NULL.\n");
        VNetFormatString(cMsg.m_strRetInfo, "Get the handler of DBVSwitchMap (NULL) failed.");
        return ERROR_SWITCH_GET_VSWITCHMAP_DB_FAIL;
    }   
    
    CPortMgr *pPortMgr = CPortMgr::GetInstance();
    if(NULL == pPortMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::DelSwitchPort: CPortMgr::GetInstance() is NULL.\n");   
        VNetFormatString(cMsg.m_strRetInfo, "Get the handler of CPortMgr (NULL) failed.");
        return ERROR_SWITCH_GET_PORTMGR_FAIL;
    }       

    vector<CDbVSwitchMap> outVInfo;
    int32 nRet = pVSMap->QueryBySwitchVna(cMsg.m_strUuid.c_str(), cMsg.m_strVnaUuid.c_str(), outVInfo);
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::DelSwitchPort: QueryBySwitchVna failed, ret = %d\n", nRet);
        VNetFormatString(cMsg.m_strRetInfo, "Query switch info by vna in database failed.");
        return ERROR_SWITCH_PORT_OPER_DB_FAIL;
    }

    vector<CDbVSwitchMap>::iterator iter = outVInfo.begin();
    for(; iter != outVInfo.end(); ++iter)
    {
        if(iter->GetVSwitchUuid() == cMsg.m_strUuid)
        {
            //端口混用时，只删除网桥
            if(cMsg.m_nIsMixed)
            {
                nRet = pVSMap->Del(cMsg.m_strUuid.c_str(), iter->GetPortUuid().c_str());
                if(VNET_DB_SUCCESS != nRet)
                {
                    VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::DelSwitchPort: Del failed, ret = %d\n", nRet);
                    VNetFormatString(cMsg.m_strRetInfo, "Delete switchport map<DVS:%s, port:%s> in database failed.",
                             cMsg.m_strUuid.c_str(), iter->GetPortUuid().c_str());
                    return ERROR_SWITCH_PORT_OPER_DB_FAIL;
                } 
                
                if((EN_ISOLATE_TYPE_VLAN != cMsg.m_cPGInfo.GetIsolateType()) && 
                    (cMsg.m_nType == SOFT_VIR_SWITCH_TYPE))
                {
                    if(0 != pPortMgr->DelVtepPort(cMsg.m_strVnaUuid, cMsg.m_strVtepPort))
                    {
                        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::DelSwitchPort: DelVtepPort failed.\n");
                        VNetFormatString(cMsg.m_strRetInfo, "Delete vtep(%s) failed.",cMsg.m_strVtepPort.c_str());
                        return ERROR_SWITCH_DEL_VTEP_FAIL;
                    }
                }
                return SWITCH_OPER_SUCCESS;
            }
            
            //找到对应的网口
            vector<string> vecUplink;
            int32 nBondMode = 0;
            if(EN_DB_PORT_TYPE_BOND == iter->GetPortType())
            {
                GetDVSBondMember(cMsg.m_strVnaUuid, iter->GetPortName(), nBondMode, vecUplink);
            }
            else
            {
                vecUplink.push_back(iter->GetPortName());
            }
            
            //删除端口
            vector<string>::iterator it_del = cMsg.m_vecPort.begin();
            for(; it_del != cMsg.m_vecPort.end(); ++it_del)
            {
                vector<string>::iterator it_uplink = vecUplink.begin();
                for(; it_uplink != vecUplink.end(); ++it_uplink)
                {
                    if(*it_del == *it_uplink)
                    {
                        it_uplink = vecUplink.erase(it_uplink); //it_uplink指向下一个元素的迭代器
                    }
                    if(it_uplink == vecUplink.end())
                    {
                        break;
                    }
                }
            }

            if(vecUplink.size() > 1)
            {
                //bond不变，修改bond表
                if(0 != pPortMgr->DelBondMem2Db(iter->GetVnaUuid(), iter->GetPortName(), cMsg.m_vecPort))
                {
                    VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::DelSwitchPort: DelBondMem2Db<%s> failed.\n", iter->GetPortName().c_str()); 
                    VNetFormatString(cMsg.m_strRetInfo, "Delete bond(%s) slaves failed.", iter->GetPortName().c_str());
                    return ERROR_SWITCH_DEL_BONDSLAVE_FAIL;
                }
            }
            else
            {
                //先删除switch map表
                int32  nSwitchType = iter->GetSwitchType();
                int32  nPortType   = iter->GetPortType();
                string strPortName = iter->GetPortName();
                nRet = pVSMap->Del(cMsg.m_strUuid.c_str(), iter->GetPortUuid().c_str());
                if(VNET_DB_SUCCESS != nRet)
                {
                    VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::DelSwitchPort: Del failed, ret = %d\n", nRet);
                    VNetFormatString(cMsg.m_strRetInfo, "Delete switchport map<DVS:%s, port:%s> in database failed.",
                             cMsg.m_strUuid.c_str(), iter->GetPortUuid().c_str());
                    return ERROR_SWITCH_PORT_OPER_DB_FAIL;
                } 
                
                //如果是bond，先删除
                if(EN_DB_PORT_TYPE_BOND == nPortType)
                {
                    if(0 != pPortMgr->DelBond2Db(cMsg.m_strVnaUuid, strPortName))
                    {
                        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::DelSwitchPort: DelBond2Db<%s> failed.\n", strPortName.c_str());  
                        VNetFormatString(cMsg.m_strRetInfo, "Delete bond(%s) in database failed.", strPortName.c_str());
                        return ERROR_SWITCH_DEL_BOND_FAIL;
                    }  
                }
                //还剩一个成员
                if(vecUplink.size() == 1)
                {
                    string strPortUuid;
                    int32  nType;
                    if(0 != pPortMgr->GetPortType(cMsg.m_strVnaUuid, vecUplink.front(), nType, strPortUuid))
                    {
                        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::DelSwitchPort: GetPortType<%s> failed.\n", vecUplink.front().c_str());
                        VNetFormatString(cMsg.m_strRetInfo, "Get port type failed.");
                        return ERROR_SWITCH_GET_PORTINFO_FAIL;
                    }    
                    if(EMB_VIR_SWITCH_TYPE == nSwitchType)
                    {
                        nRet = pVSMap->Add(cMsg.m_strUuid.c_str(), strPortUuid.c_str(), 1, 0, 1);
                    }
                    else
                    {
                        nRet = pVSMap->Add(cMsg.m_strUuid.c_str(), strPortUuid.c_str(), 1, 1, 1);
                    }
                    if(VNET_DB_SUCCESS != nRet)
                    {
                        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::DelSwitchPort: Add failed, ret = %d\n", nRet);
                        VNetFormatString(cMsg.m_strRetInfo, "Add switchport map<DVS:%s, port:%s> in database failed.",
                             cMsg.m_strUuid.c_str(), strPortUuid.c_str());
                        return ERROR_SWITCH_PORT_OPER_DB_FAIL;
                    }                    
                }
                else
                {
                    if((EN_ISOLATE_TYPE_VLAN != cMsg.m_cPGInfo.GetIsolateType()) && 
                        (cMsg.m_nType == SOFT_VIR_SWITCH_TYPE))
                    {
                        if(0 != pPortMgr->DelVtepPort(cMsg.m_strVnaUuid, cMsg.m_strVtepPort))
                        {
                            VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::DelSwitchPort: call DelVtepPort to delete %s failed.\n", 
                                      cMsg.m_strVtepPort.c_str());
                            VNetFormatString(cMsg.m_strRetInfo, "Delete vtep(%s) failed.",cMsg.m_strVtepPort.c_str());
                            return ERROR_SWITCH_DEL_VTEP_FAIL;
                        }
                    }
                }
            }
            break;  
        }
    }

    return SWITCH_OPER_SUCCESS;
}

/*******************************************************************************
* 函数名称： ModifySwitchPort
* 功能描述： VNA离线时，清除数据库中consist字段
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhongchsh     创 建
*******************************************************************************/
int32 CVirtualSwitch::ModifySwitchPort(const string &strVNAUuid)
{
    CDBOperateVSwitchMap * pVSMap = GetDBVSwitchMap();
    if(NULL == pVSMap)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::ModifySwitchPort: GetDBVSwitchMap() is NULL.\n");
        return ERROR_SWITCH_GET_VSWITCHMAP_DB_FAIL;
    }   
    
    vector<CDbVSwitchMap> outVInfo;
    int32 nRet = pVSMap->QueryByVna(strVNAUuid.c_str(), outVInfo);
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::ModifySwitchPort: QueryByVna<VNA: %s> failed, ret = %d\n", strVNAUuid.c_str(), nRet);
        return ERROR_SWITCH_PORT_OPER_DB_FAIL;
    }    
    
    vector<CDbVSwitchMap>::iterator iter = outVInfo.begin();
    for(; iter != outVInfo.end(); ++iter)    
    {
        //VNA离线，将consist清零，iscfg恢复成1
        if(EN_SWITCH_MAP_NO_CFG != iter->GetSwitchMapIsCfg())
        {
            nRet = pVSMap->Modify(iter->GetVSwitchUuid().c_str(), iter->GetPortUuid().c_str(), 
                                      EN_SWITCH_MAP_IS_CFG, 0, 0);
            if(VNET_DB_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::ModifySwitchPort: Modify<DVS: %s, PORT:%s>failed, ret = %d\n", 
                          iter->GetVSwitchUuid().c_str(), iter->GetPortUuid().c_str(), nRet);
                continue;
            }         
        }
    }
    return SWITCH_OPER_SUCCESS;
}
    
int32 CVirtualSwitch::ModifySwitchPort()
{
    CDBOperateVSwitch * pVSwitch = GetDBVSwitch();
    if(NULL == pVSwitch)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::ModifySwitchPort: GetDBVSwitch() is NULL.\n");
        return ERROR_SWITCH_GET_VSWITCH_DB_FAIL;
    } 
    
    CDBOperateVSwitchMap * pVSMap = GetDBVSwitchMap();
    if(NULL == pVSMap)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::ModifySwitchPort: GetDBVSwitchMap() is NULL.\n");
        return ERROR_SWITCH_GET_VSWITCHMAP_DB_FAIL;
    }   

    //先找到所有交换
    vector<CDbSwitchSummary> vecDbSwitch;
    int32 nRet = pVSwitch->QuerySummary(vecDbSwitch);
    if(VNET_DB_SUCCESS != nRet)
    {   
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::ModifySwitchPort: call QuerySummary fail.\n");
        return ERROR_SWITCH_GET_VSWITCHMAP_DB_FAIL;
    }
    //清除所有交换端口consist信息
    vector<CDbSwitchSummary>::iterator itSwitch = vecDbSwitch.begin();
    for(; itSwitch != vecDbSwitch.end(); ++itSwitch)
    {
        vector<CDbVSwitchMap> vecDbSwitchPort;
        nRet = pVSMap->QueryBySwitch(itSwitch->GetUuid().c_str(), vecDbSwitchPort);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::ModifySwitchPort: QueryBySwitch<swich: %s> failed, ret = %d\n", 
                      itSwitch->GetUuid().c_str(), nRet);
            continue;
        }  
        
        vector<CDbVSwitchMap>::iterator itPort = vecDbSwitchPort.begin();
        for(; itPort != vecDbSwitchPort.end(); ++itPort)    
        {
            //将consist清零，iscfg恢复成1
            if(EN_SWITCH_MAP_NO_CFG != itPort->GetSwitchMapIsCfg())
            {
                nRet = pVSMap->Modify(itPort->GetVSwitchUuid().c_str(), itPort->GetPortUuid().c_str(), 
                                          EN_SWITCH_MAP_IS_CFG, 0, 0);
                if(VNET_DB_SUCCESS != nRet)
                {
                    VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::ModifySwitchPort: Modify<DVS: %s, PORT:%s>failed, ret = %d\n", 
                              itPort->GetVSwitchUuid().c_str(), itPort->GetPortUuid().c_str(), nRet);
                    continue;
                }         
            }
        }
    }

    return SWITCH_OPER_SUCCESS;
}    

int32 CVirtualSwitch::QuerySwitchPort(CSwitchCfgMsgToVNA &cMsg)
{
    CDBOperateVSwitchMap * pVSMap = GetDBVSwitchMap();
    if(NULL == pVSMap)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::QuerySwitchPort: GetDBVSwitchMap() is NULL.\n");
        VNetFormatString(cMsg.m_strRetInfo, "Get the handler of DBVSwitchMap failed.");
        return ERROR_SWITCH_PORT_OPER_DB_FAIL;
    }   
    
    vector<CDbVSwitchMap> outVInfo;
    int32 nRet = pVSMap->QueryBySwitchVna(cMsg.m_strUuid.c_str(), cMsg.m_strVnaUuid.c_str(), outVInfo);
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::QuerySwitchPort: QueryBySwitchVna failed, ret = %d\n", nRet);
        VNetFormatString(cMsg.m_strRetInfo, "Query switch info by vna failed.");
        return ERROR_SWITCH_PORT_OPER_DB_FAIL;
    }    
    
    vector<CDbVSwitchMap>::iterator iter = outVInfo.begin();
    for(; iter != outVInfo.end(); ++iter)
    {
        if(iter->GetVSwitchUuid() == cMsg.m_strUuid)
        {
            if(iter->GetPortType() == EN_DB_PORT_TYPE_BOND)
            {
                cMsg.m_strBondName = iter->GetPortName();
                cMsg.m_strSwIf     = iter->GetPortName();
                cMsg.m_nSwIfType   = iter->GetPortType();
                if(0 != GetDVSBondMember(iter->GetVnaUuid(), cMsg.m_strBondName, cMsg.m_nBondMode, cMsg.m_vecPort))
                {
                    VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::QuerySwitchPort: call <GetDVSBondMember> %s failed.\n", cMsg.m_strBondName.c_str());
                    VNetFormatString(cMsg.m_strRetInfo, "Get bond(%s) slaves failed.", cMsg.m_strBondName.c_str());
                    return ERROR_SWITCH_PORT_OPER_DB_FAIL;
                }
            }
            else
            {
                cMsg.m_strSwIf   = iter->GetPortName();
                cMsg.m_nSwIfType = iter->GetPortType();
                cMsg.m_vecPort.push_back(iter->GetPortName());
            }
            break;
        }
    }
    return SWITCH_OPER_SUCCESS;
}

int32 CVirtualSwitch::AddKernelDVS(const string &strVNAUuid, const string &strDVSName, 
          const vector<string> &vecPortName, const string &strBondName, const int32 &nBondMode)
{
    CDBOperateVSwitch * pVSwitch = GetDBVSwitch();
    if(NULL == pVSwitch)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::AddKernelDVS: GetDBVSwitch() is NULL.\n");
        return ERROR_SWITCH_GET_VSWITCH_DB_FAIL;
    } 
    
    CDBOperateVSwitchMap * pVSMap = GetDBVSwitchMap();
    if(NULL == pVSMap)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::AddKernelDVS: GetDBVSwitchMap() is NULL.\n");
        return ERROR_SWITCH_GET_VSWITCHMAP_DB_FAIL;
    } 
    
    CPortMgr *pPortMgr = CPortMgr::GetInstance();
    if(NULL == pPortMgr)
    {   
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::AddKernelDVS: CPortMgr::GetInstance() is NULL.\n");
        return ERROR_SWITCH_GET_PORTMGR_FAIL;
    }   
    
    //查找交换
    string strDVSUuid;
    int32 nRet = GetDVSUuid(strDVSName, strDVSUuid);    
    if(nRet < 0)
    {   
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::AddKernelDVS: GetDVSUuid() fail.\n");
        return ERROR_SWITCH_GET_VSWITCH_DB_FAIL;
    }  

    if(0 != nRet)
    {
        //不存在，先创建交换
        CSwitchCfgMsg cDVSCfg;
        CPortGroupMgr *pPGMgr = CPortGroupMgr::GetInstance();
        if((NULL == pPGMgr) || (0 != pPGMgr->QueryPGByName("DEFAULT_UPLINK_PG", cDVSCfg.m_strPGUuid)))
        {
            VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::AddKernelDVS: call QueryPGByName failed.\n");
            return ERROR_SWTICH_GET_PGINFO_FAIL;
        }
        cDVSCfg.m_strName    = strDVSName;
        cDVSCfg.m_nType      = SOFT_VIR_SWITCH_TYPE;
        cDVSCfg.m_nMTU       = 1500;
        cDVSCfg.m_nEvbMode   = 0;
        cDVSCfg.m_nMaxVnic   = MAX_SWITCH_VNIC_NUMS;
        cDVSCfg.m_nOper      = EN_ADD_SWITCH;
        string strRetInfo;
        nRet = AddSwitch(cDVSCfg, strRetInfo);
        if((SWITCH_OPER_SUCCESS != nRet) && (ERROR_SWITCH_ALREADY_EXIST != nRet))
        {
            VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::AddKernelDVS: call AddSwitch fail, ret = %d\n", nRet);
            return nRet;
        } 
        strDVSUuid = cDVSCfg.m_strUuid;
    }
    
    //生成交换对应端口
    string strDVSPortName;
    string strDVSPortUuid;
    if(strBondName.empty())
    {
        strDVSPortName = vecPortName.front();
        if(pPortMgr->GetPortUUID(strVNAUuid, strDVSPortName, EN_DB_PORT_TYPE_PHYSICAL, strDVSPortUuid))
        {
            VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::AddKernelDVS: call GetPortUUID<%s> fail.\n", strDVSPortName.c_str());
            return ERROR_SWITCH_GET_PORTINFO_FAIL;
        }
    }
    else
    {
        strDVSPortName = strBondName;
    }
    
    vector<CDbVSwitchMap> outVInfo;
    nRet = pVSMap->QueryBySwitchVna(strDVSUuid.c_str(), strVNAUuid.c_str(), outVInfo);
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::AddKernelDVS: QueryBySwitchVna failed, ret = %d\n", nRet);
        return ERROR_SWITCH_PORT_OPER_DB_FAIL;
    }    

    vector<CDbVSwitchMap>::iterator iter = outVInfo.begin();
    for(; iter != outVInfo.end(); ++iter)
    {
        int32 nFindEqual = 0; //端口是否完全一致标记
        if((iter->GetVSwitchName() == strDVSName) && (iter->GetPortName() == strDVSPortName))
        {
            nFindEqual = 1; //完全匹配
            if(EN_DB_PORT_TYPE_BOND == iter->GetPortType())
            {
                int32 nOldMode = 0; 
                vector<string> vecSlaves;
                GetDVSBondMember(strVNAUuid, iter->GetPortName(), nOldMode, vecSlaves);
                if((0 != IsVectorEqua(vecPortName, vecSlaves)) || (nBondMode != nOldMode))
                {
                    nFindEqual = 0;
                } 
            }
        }
        
        if(1 == nFindEqual)
        {
            return SWITCH_OPER_SUCCESS;
        }
        else
        {
            //不完全匹配，可能端口不对，可能bond成员口或模式不对
            int32  nPortType   = iter->GetPortType();
            string strPortName = iter->GetPortName();
            nRet = pVSMap->Del(strDVSUuid.c_str(), iter->GetPortUuid().c_str());
            if(VNET_DB_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::AddKernelDVS: Del failed, ret = %d\n", nRet);
                return ERROR_SWITCH_PORT_OPER_DB_FAIL;
            } 
            
            //如果是bond，先删除
            if(EN_DB_PORT_TYPE_BOND == nPortType)
            {
                if(0 != pPortMgr->DelBond2Db(strVNAUuid, strPortName))
                {
                    VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::AddKernelDVS: DelBond2Db<%s> failed.\n", strPortName.c_str());  
                    return ERROR_SWITCH_DEL_BOND_FAIL;
                }  
            }
            break;
        }
    }
    
    //switch map表无记录，新增条目
    if(!strBondName.empty())
    {
        if(0 != pPortMgr->AddBond2Db(strVNAUuid, strBondName, vecPortName, nBondMode, strDVSPortUuid))
        {
            VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::AddKernelDVS: AddBond2Db<%s, mode:%d> failed.\n", strBondName.c_str(), nBondMode);
            return ERROR_SWITCH_ADD_BOND_FAIL;
        }      
    }
    //添加新的数据
    nRet = pVSMap->Add(strDVSUuid.c_str(), strDVSPortUuid.c_str(), 1, 1, 1);
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::AddKernelDVS: Add failed, ret = %d\n", nRet);
        return ERROR_SWITCH_PORT_OPER_DB_FAIL;
    }     
    
    return SWITCH_OPER_SUCCESS;
}

int32 CVirtualSwitch::GetDVSUuid(const string &strSwName, string &strSwUuid)
{
    CDBOperateVSwitch * pVSwitch = GetDBVSwitch();
    if(NULL == pVSwitch)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::GetDVSUuid: GetDBVSwitch() is NULL.\n");
        return -1;
    }  
    
    vector<CDbSwitchSummary> outVInfo;
    int32 nRet = pVSwitch->QuerySummary(outVInfo);
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::GetDVSUuid: QuerySummary failed, ret = %d\n", nRet);
        return -1;
    }    

    vector<CDbSwitchSummary>::iterator iter = outVInfo.begin();
    for(;iter != outVInfo.end(); ++iter)
    {    
        if(iter->GetName() == strSwName)
        {
            strSwUuid = iter->GetUuid();
            return 0;
        }
    }   
    
    //不存在该交换
    return 1;
}

int32 CVirtualSwitch::GetDVSName(const string &strSwUuid, string &strSwName)
{
    CDBOperateVSwitch * pVSwitch = GetDBVSwitch();
    if(NULL == pVSwitch)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::GetDVSName: GetDBVSwitch() is NULL.\n");
        return -1;
    }  
    
    vector<CDbSwitchSummary> outVInfo;
    int32 nRet = pVSwitch->QuerySummary(outVInfo);
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::GetDVSName: QuerySummary failed, ret = %d\n", nRet);
        return -1;
    }   
    
    vector<CDbSwitchSummary>::iterator iter = outVInfo.begin();
    for(;iter != outVInfo.end(); ++iter)
    {    
        if(iter->GetUuid() == strSwUuid)
        {
            strSwName = iter->GetName();
            return 0;
        }   
    }
    return -1;
}
//获取在线的VNA
int32 CVirtualSwitch::GetDVSVNA(const string &strDVSUuid, vector<string> &vecVNAUuid)
{
    CDBOperateVSwitchMap * pVSMap = GetDBVSwitchMap();
    if(NULL == pVSMap)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::GetDVSVNA: GetDBVSwitchMap() is NULL.\n");
        return ERROR_SWITCH_GET_VSWITCHMAP_DB_FAIL;
    }
    
    vector<CDbVSwitchMap> outVInfo;
    int32 nRet = pVSMap->QueryBySwitch(strDVSUuid.c_str(),outVInfo);
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::GetDVSVNA: QueryBySwitch failed, ret = %d\n", nRet);
        return ERROR_SWITCH_OPER_DB_FAIL;
    }     

    vector<CDbVSwitchMap>::iterator iter = outVInfo.begin();
    for(;iter != outVInfo.end(); ++iter)
    {
        if(iter->GetSwitchMapIsConsistency())
        {
            vecVNAUuid.push_back(iter->GetVnaUuid());
        }
    }
    return SWITCH_OPER_SUCCESS;
}

int32 CVirtualSwitch::GetDVSPGUuid(const string &strDVSUuid, string &strPGUuid)
{
    CDBOperateVSwitch * pVSwitch = GetDBVSwitch();
    if(NULL == pVSwitch)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::GetDVSPGUuid: GetDBVSwitch() is NULL.\n");
        return ERROR_SWITCH_GET_VSWITCH_DB_FAIL;
    }  

    CDbVSwitch cDbInfo;
    cDbInfo.SetUuid(strDVSUuid.c_str());
    int32 nRet = pVSwitch->Query(cDbInfo);
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::GetDVSPGUuid: Query failed, ret = %d\n", nRet);
        return ERROR_SWITCH_OPER_DB_FAIL;
    }     

    strPGUuid = cDbInfo.GetPgUuid();
    return SWITCH_OPER_SUCCESS;
}

int32 CVirtualSwitch::GetDVSUplinkPort(const string &strVNAUuid, const string &strSwName, string &strPortName)
{
    CDBOperateVSwitchMap * pVSMap = GetDBVSwitchMap();
    if(NULL == pVSMap)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::GetDVSUplinkPort: GetDBVSwitchMap() is NULL.\n");
        return -1;
    }

    vector<CDbVSwitchMap> outVInfo;
    int32 nRet = pVSMap->QueryByVna(strVNAUuid.c_str(), outVInfo);
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::GetDVSUplinkPort: QueryByVna failed, ret = %d\n", nRet);
        return -1;
    }    

    vector<CDbVSwitchMap>::iterator iter = outVInfo.begin();
    for(;iter != outVInfo.end(); ++iter)
    {    
        if(iter->GetVSwitchName() == strSwName)
        {
            strPortName = iter->GetPortName();
            return 0;
        }
    } 
    //不存在
    return 1;
}

void CVirtualSwitch::DbgShow()
{
    CDBOperateVSwitch * pVSwitch = GetDBVSwitch();
    if(NULL == pVSwitch)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::DbgShow: GetDBVSwitch() is NULL.\n");
        return;
    }
    
    CDBOperateVSwitchMap * pVSMap = GetDBVSwitchMap();
    if(NULL == pVSMap)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVirtualSwitch::DbgShow: GetDBVSwitchMap() is NULL.\n");
        return;
    }
    
    vector<CDbSwitchSummary> outVInfo;
    int32 nRet = pVSwitch->QuerySummary(outVInfo);
    if(VNET_DB_SUCCESS == nRet)
    {   
        cout << "************************************************************" << endl;
        vector<CDbSwitchSummary>::iterator iter = outVInfo.begin();
        for( ; iter != outVInfo.end(); ++iter)
        {
            CDbVSwitch cDbVSInfo;
            cDbVSInfo.SetUuid(iter->GetUuid().c_str());
            nRet = pVSwitch->Query(cDbVSInfo);
            if(VNET_DB_SUCCESS == nRet)
            {
                cout << "----------------------" << cDbVSInfo.GetName() << "----------------------"<<endl;
                cout << "Switch Uuid: " << cDbVSInfo.GetUuid() << endl;
                cout << "Switch Type: " << cDbVSInfo.GetSwitchType() << endl;
                cout << "Max MTU    : " << cDbVSInfo.GetMaxMtu()  << endl;
                cout << "Evb Mode   : " << cDbVSInfo.GetEvbMode()  << endl;
                cout << "Max Vnic   : " << cDbVSInfo.GetNicMaxNumCfg()  << endl;
                cout << "PG Uuid    : " << cDbVSInfo.GetPgUuid()  << endl;
            }
            vector<CDbVSwitchMap> vecVSMap;
            nRet = pVSMap->QueryBySwitch(iter->GetUuid().c_str(), vecVSMap);
            if(VNET_DB_SUCCESS == nRet)
            {
                vector<CDbVSwitchMap>::iterator it = vecVSMap.begin();
                for(; it != vecVSMap.end(); ++it)
                {
                    cout << "VNA Uuid   : " << it->GetVnaUuid() << endl;
                    cout << "Port Uuid  : " << it->GetPortUuid() << endl;
                    cout << "Port Name  : " << it->GetPortName() << endl;
                }
            }
            cout << endl;
        }
        cout << "************************************************************" << endl;
    }
}


}

