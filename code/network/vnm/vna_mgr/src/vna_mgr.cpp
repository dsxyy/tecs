/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vna_mgr.cpp
* 文件标识：
* 内容摘要：CVNAManager类实现文件
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
#include "vnet_event.h"
#include "vnet_msg.h"
#include "vnetlib_common.h"
#include "ipv4_addr.h"
#include "ipv4_addr_range.h"
#include "ipv4_addr_pool.h"
#include "ipv4_pool_mgr.h"
#include "logic_network.h"
#include "logic_network_mgr.h"
#include "vnet_portmgr.h"
#include "vnet_db_reg_module.h"
#include "vnet_db_vna.h"
#include "vnet_db_mgr.h"
#include "vnet_db_schedule.h"

#include "vnet_portmgr.h"
#include "vnm_schedule.h"
#include "vnm_switch_manager.h"

#include "wildcast_mgr.h"

#include "vnetlib_event.h"
#include "vnet_error.h"

#include "vna_reg_mod.h"
#include "vna_info.h"
#include "vna_mgr.h"
#include "vnm_vxlan_mgr.h"

namespace zte_tecs
{
#define MONI_VNA_INTERVAL    (5000)  // 5s扫描更新一次VNA注册信息;

    CVNAManager *CVNAManager::s_pInstance = NULL;
    
    CVNAManager::CVNAManager()
    {
        m_tVNAMoniTimerID = INVALID_TIMER_ID;
        m_bOpenDbgInf = VNET_FALSE;
        m_pMU = NULL;
    }
    
    CVNAManager::~CVNAManager()
    {
        if(INVALID_TIMER_ID != m_tVNAMoniTimerID)
        {
            if(m_pMU)
            {
                m_pMU->KillTimer(m_tVNAMoniTimerID);  // 析构中此操作是否集中到一个下电函数中???;
                m_tVNAMoniTimerID = INVALID_TIMER_ID;
            }
        }

        ClearAllVNAInfo();

        m_pMU = NULL;
    }
    
    int32 CVNAManager::Init(MessageUnit *pMu, const string &cstrVNMApp)
    {
        if (pMu == NULL)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNAManager::Init: pMu is NULL.\n");
            return -1;
        }        
        m_pMU = pMu;

        if (cstrVNMApp.empty())
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNAManager::Init: cstrVNMApp is NULL.\n");
            return -1;
        }
        m_strVNMApp = cstrVNMApp;

        m_tVNAMoniTimerID = m_pMU->CreateTimer();
        if (INVALID_TIMER_ID == m_tVNAMoniTimerID)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNAManager::Init: call CreateTimer failed.\n");
            return -1;
        }

        TimeOut timeout;
        timeout.type = LOOP_TIMER;
        timeout.duration = MONI_VNA_INTERVAL;
        timeout.msgid = EV_VNA_MONI_TIMER;
        timeout.arg = 0;
        if(SUCCESS != m_pMU->SetTimer(m_tVNAMoniTimerID, timeout))
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNAManager::Init: call SetTimer failed.\n");
            m_pMU->KillTimer(m_tVNAMoniTimerID);
            m_tVNAMoniTimerID = INVALID_TIMER_ID;
            return -1;
        }

        // Load the VNM's VNAs info from DB;
        if(0 != LoadVNAInfoFromDB())
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNAManager::Init: call LoadVNAInfoFromDB failed.\n");
            return -1;
        }

        return 0;
    }

    int32 CVNAManager::QueryVNAApp(const string strCCApp, const string &strHCApp, string &strVNAApp)
    {
        CVNAInfo cVNA;
        if(0 == QueryVNA(strCCApp, strHCApp, cVNA))
        {
            strVNAApp = cVNA.GetApp();
            return 0;
        }        
        return -1;
    }
    
    int32 CVNAManager::QueryVNAUUID(const string strCCApp, const string &strHCApp, string &strVNAUUID)
    {
        CVNAInfo cVNA;
        if(0 == QueryVNA(strCCApp, strHCApp, cVNA))
        {
            strVNAUUID = cVNA.GetUUID();
            return 0;
        }        
        return -1;
    }

    int32 CVNAManager::QueryVNA(const string strCCApp, const string &strHCApp, CVNAInfo &cVNAInfo)
    {
        CVNetDbMgr * pMgr = CVNetDbMgr::GetInstance();
        if(NULL == pMgr)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNAManager::QueryVNAUUID: CVNetDbMgr::GetInstance() is NULL.\n");
            return -1;
        }
        CDBOperateVna *pOperVNA = pMgr->GetIVna();
        if(NULL == pOperVNA )
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNAManager::QueryVNAUUID: CDBOperateVna is NULL.\n");
            return -1;
        }

        CDBOperateVnaRegModule *pOperVNARegMod = pMgr->GetIVnaRegModule();
        if(NULL == pOperVNARegMod )
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNAManager::QueryVNAUUID: GetIVnaRegModule() == NULL.\n");
            return -1;
        }

        vector<CDbVnaSummary> outVNAInfo;
        int32 nRet = pOperVNA->QueryVnaSummary(outVNAInfo);
        if(VNET_DB_SUCCESS == nRet)
        {
            vector<CDbVnaSummary>::iterator itrVNA = outVNAInfo.begin();
            for(; itrVNA != outVNAInfo.end(); ++itrVNA)
            {
                CDbVna cDbVnaInfo;
                cDbVnaInfo.SetUuid(itrVNA->GetUuid().c_str());
                if(0 != pOperVNA->QueryVna(cDbVnaInfo))
                {
                    continue ;
                }
                // 如果不在线就忽略,调度需求;
                if(EN_STATE_ONLINE != cDbVnaInfo.GetIsOnline())
                {                    
                    continue ;
                } 
                // 查询接口优化后修改;
                vector<CDbVnaRegModuleSummary> outVInfo;
                if(VNET_DB_SUCCESS == pOperVNARegMod->QueryRegModuleSummary(outVInfo))
                {
                    vector<CDbVnaRegModuleSummary>::iterator it = outVInfo.begin();
                    for(; it != outVInfo.end(); ++it)
                    {
                        if(cDbVnaInfo.GetUuid() != it->GetVnaUuid())
                        {
                            continue ;
                        }
                
                        CDbVnaRegistModule cDbRegMod;
                        cDbRegMod.SetModuleUuid(it->GetModuleUuid().c_str());
                        if(VNET_DB_SUCCESS == pOperVNARegMod->QueryRegModule(cDbRegMod))
                        {
                            if(cDbRegMod.GetRole() == VNA_MODULE_ROLE_HC &&
                               cDbRegMod.GetIsOnline() == EN_STATE_ONLINE &&
                               cDbRegMod.GetModuleName() == strHCApp &&
                               cDbRegMod.GetClusterName() == strCCApp)
                            {
                                cVNAInfo.SetApp(cDbVnaInfo.GetVnaApp());
                                cVNAInfo.SetHost(cDbVnaInfo.GetHostName());
                                cVNAInfo.SetUUID(cDbVnaInfo.GetUuid());
                                cVNAInfo.SetVNMApp(cDbVnaInfo.GetVnmApp());
                                return 0;
                            }
                        }
                    }// end for(; ...
                }
            }// end for(; itrVNA...
        }
        
        return -1;
    }
    
    int32 CVNAManager::QueryVNAModInfo(const string &strVNAUUID, string &strHCApp, string &strCCApp)
    {
        CVNetDbMgr * pMgr = CVNetDbMgr::GetInstance();
        if(NULL == pMgr)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNAManager::QueryVNAModInfo: CVNetDbMgr::GetInstance() is NULL.\n");
            return -1;
        }
        
        CDBOperateVnaRegModule *pOperVNARegMod = pMgr->GetIVnaRegModule();
        if(NULL == pOperVNARegMod )
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNAManager::QueryVNAModInfo: GetIVnaRegModule() == NULL.\n");
            return -1;
        }

        vector<CDbVnaRegModuleSummary> outVInfo;
        if(VNET_DB_SUCCESS == pOperVNARegMod->QueryRegModuleSummary(outVInfo))
        {
            vector<CDbVnaRegModuleSummary>::iterator it = outVInfo.begin();
            for(; it != outVInfo.end(); ++it)
            {
                if(strVNAUUID != it->GetVnaUuid())
                {
                    continue ;
                }
        
                CDbVnaRegistModule cDbRegMod;
                cDbRegMod.SetModuleUuid(it->GetModuleUuid().c_str());
                if(VNET_DB_SUCCESS == pOperVNARegMod->QueryRegModule(cDbRegMod))
                {
                    if(cDbRegMod.GetRole() == VNA_MODULE_ROLE_HC)
                    {
                        strHCApp = cDbRegMod.GetModuleName();
                        strCCApp = cDbRegMod.GetClusterName();
                        return 0;
                    }
                }
            }
        }

        return -1;
    }
    
    int32 CVNAManager::QueryVNAModRole(const string &strVNAUUID, const uint8 &ucRole)
    {
        CVNAInfo *pVNAInfo = GetVNAInfo(strVNAUUID);        
        if(NULL == pVNAInfo)
        {
            VNET_LOG(VNET_LOG_WARN, "CVNAManager::IsSupportVNAWDG: The VNA[APP = %s] is not registed.\n", strVNAUUID.c_str());
            // 没有注册;            
            return -1;
        }
        
        vector<CVNARegModInfo>::iterator iter =  pVNAInfo->GetRegistedModInfo().begin();
        for(; iter != pVNAInfo->GetRegistedModInfo().end(); ++iter)
        {
            if(iter->GetRegModRole() == ucRole)
            {
                return 0;
            }
        }
        return -1;
    }
    
    void CVNAManager::MessageEntry(const MessageFrame &message)
    {
        switch (message.option.id())
        {
        case EV_VNA_REGISTER_TO_VNM:
            {
                ProcVNARegReq(message);
            }
            break;
        case EV_VNA_INFO_REPORT:
            {
                ProcVNAReport(message);
            }
            break;
        case EV_VNA_MONI_TIMER:
            {
                ProcVNAMoniTmOut();                
            }
            break; 
        case EV_VNA_HEARTBEAT_REPORT:
            {
                ProcVNAHeartbeat(message);
            }
            break;
        case EV_VNETLIB_DEL_VNA:
            {
                ProcVNADelete(message);
            }
            break;            
        default:
            break;            
        }
        
        return ;
    }

    CVNAInfo* CVNAManager::GetVNAInfo(const string &cstrApp)
    {
        map<string, CVNAInfo *>::iterator pos;
        pos = m_mapVNAInfo.find(cstrApp);
        if(pos != m_mapVNAInfo.end())
        {
            return pos->second;
        }
        
        return NULL;
    }

    int32 CVNAManager::DelVNAInfo(const string &cstrApp)
    {
        map<string, CVNAInfo *>::iterator pos;
        pos = m_mapVNAInfo.find(cstrApp);
        if(pos != m_mapVNAInfo.end())
        {
            CVNAInfo *p = pos->second;
            m_mapVNAInfo.erase(pos);
            delete p;
            p = NULL;
            return 0;
        }
        
        VNET_LOG(VNET_LOG_WARN, "CVNAManager::DelVNAInfo: The VNA[app = %s] is not existed.\n", 
            cstrApp.c_str()); 
            
        return 0;
    }

    int32 CVNAManager::UpdateVNAInfo(CVNAInfo *pVNA, CVNAReportMsg *pMsg)
    {
        if(NULL == pVNA || NULL == pMsg)
        {
            VNET_ASSERT(0);
            return -1;
        }
        
        if(pVNA->GetHost() != pMsg->m_strHostName)
        {
            pVNA->SetHost(pMsg->m_strHostName);
        }
        
        vector<CVNARegModInfo>::iterator itrVNAStorMod = pVNA->GetRegistedModInfo().begin();
        for(; itrVNAStorMod != pVNA->GetRegistedModInfo().end(); ++itrVNAStorMod)
        {
            // 搜索VNA中注册的模块是否出现在消息上报中;
            CVNARegModMsg *pMsgRegMod = pMsg->FindRegMod(itrVNAStorMod->GetRegModApp());
            if(NULL == pMsgRegMod)
            {
                itrVNAStorMod->SetRegModState(EN_STATE_OFFLINE);
            }
        }
        
        vector<CVNARegModMsg>::iterator itrVNARepoMod = pMsg->m_vecRegistedModInfo.begin();
        for(; itrVNARepoMod != pMsg->m_vecRegistedModInfo.end(); ++itrVNARepoMod)
        {
            // 搜索上报消息中的模块是否出现在本地VNA模块注册表中;
            CVNARegModInfo *pRegModInfo = pVNA->GetRegModInfo(itrVNARepoMod->m_strModApp, itrVNARepoMod->m_ucModRole);
            if(NULL == pRegModInfo)
            {
                CVNARegModInfo cRegMod;
                
                if(itrVNARepoMod->m_strModApp.empty())
                {                 
                    VNET_ASSERT(0);                    
                    VNET_LOG(VNET_LOG_ERROR, "CVNAManager::UpdateVNAInfo: Msg error, VNA registed moduler name is NULL.\n");
                    pMsg->Print();
                    continue ;
                }
                cRegMod.SetRegModRole(itrVNARepoMod->m_ucModRole);                
                cRegMod.SetRegModState(itrVNARepoMod->m_ucModState);
                cRegMod.SetRegModApp(itrVNARepoMod->m_strModApp);
                cRegMod.SetExtInfo(itrVNARepoMod->m_strExtInfo);
                pVNA->AddRegModInfo(cRegMod);
            }
            else
            {
                // 更新模块注册信息
                pRegModInfo->SetRegModRole(itrVNARepoMod->m_ucModRole);
                pRegModInfo->SetRegModState(itrVNARepoMod->m_ucModState);
                pRegModInfo->SetExtInfo(itrVNARepoMod->m_strExtInfo);
            }
        }
        
        return 0;
    }

    int32 CVNAManager::LoadVNAInfoFromDB(void)
    {
        CVNetDbMgr * pMgr = CVNetDbMgr::GetInstance();
        if(NULL == pMgr)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNAManager::LoadVNAInfoFromDB: CVNetDbMgr::GetInstance() is NULL.\n");
            return -1;
        }
        CDBOperateVna *pOperVNA = pMgr->GetIVna();
        if(NULL == pOperVNA )
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNAManager::LoadVNAInfoFromDB: CDBOperateVna is NULL.\n");
            return -1;
        }

        CDBOperateVnaRegModule *pOperVNARegMod = pMgr->GetIVnaRegModule();
        if(NULL == pOperVNARegMod )
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNAManager::LoadVNAInfoFromDB: GetIVnaRegModule() == NULL.\n");
            return -1;
        }

        if(!m_mapVNAInfo.empty())
        {
            ClearAllVNAInfo();
        }
        
        vector<CDbVnaSummary> outVNAInfo;
        int32 nRet = pOperVNA->QueryVnaSummary(outVNAInfo);
        if(VNET_DB_SUCCESS == nRet)
        {
            vector<CDbVnaSummary>::iterator itrVNA = outVNAInfo.begin();
            for(; itrVNA != outVNAInfo.end(); ++itrVNA)
            {
                CDbVna cDbVnaInfo;
                cDbVnaInfo.SetUuid(itrVNA->GetUuid().c_str());
                if(0 != pOperVNA->QueryVna(cDbVnaInfo))
                {
                    continue ;
                }                
                // UUID == APP;
                if(cDbVnaInfo.GetUuid() != cDbVnaInfo.GetVnaApp())
                {
                    VNET_LOG(VNET_LOG_ERROR, "CVNAManager::LoadVNAInfoFromDB: VNA.UUID[%s] != VNA.APP[%s].\n", 
                        cDbVnaInfo.GetUuid().c_str(), cDbVnaInfo.GetVnaApp().c_str());
                    VNET_ASSERT(0);
                    continue ;
                }

                if(m_strVNMApp != cDbVnaInfo.GetVnmApp())
                {
                    cDbVnaInfo.SetIsOnline(EN_STATE_INIT);
                    int32 nModifyRet = pOperVNA->ModifyVna(cDbVnaInfo);
                    if(0 != nModifyRet)
                    {
                        VNET_LOG(VNET_LOG_ERROR, "CVNAManager::LoadVNAInfoFromDB: call ModifyVna failed. ret = %d.\n",
                            nModifyRet);
                        return -1;
                    }
                    continue ;
                }
                
                /*
                 * VNM重启或则新上电，VNA的状态应该为NOT ONLINE; 
                 * 此处可能需要回写数据库，保持查询一致;
                 */
                if(EN_STATE_ONLINE == cDbVnaInfo.GetIsOnline())
                {
                    cDbVnaInfo.SetIsOnline(EN_STATE_INIT);
                    int32 nModifyRet = pOperVNA->ModifyVna(cDbVnaInfo);
                    if(0 != nModifyRet)
                    {
                        VNET_LOG(VNET_LOG_ERROR, "CVNAManager::LoadVNAInfoFromDB: call ModifyVna failed. ret = %d.\n",
                            nModifyRet);
                        return -1;
                    }
                }

                CVNAInfo *pVNAInfo = new CVNAInfo();
                if(NULL == pVNAInfo)
                {
                    VNET_LOG(VNET_LOG_ERROR, "CVNAManager::LoadVNAInfoFromDB: failed to allocate memory!\n");
                    return -1;
                } 
                pVNAInfo->SetUUID(cDbVnaInfo.GetUuid());                
                pVNAInfo->SetApp(cDbVnaInfo.GetVnaApp());
                pVNAInfo->SetVNMApp(cDbVnaInfo.GetVnmApp());
                pVNAInfo->SetHost(cDbVnaInfo.GetHostName());  
                pVNAInfo->SetOSName(cDbVnaInfo.GetOsName());  
                pVNAInfo->SetState(cDbVnaInfo.GetIsOnline()); 

                // 查询接口优化后修改;
                vector<CDbVnaRegModuleSummary> outVInfo;
                if(0 == pOperVNARegMod->QueryRegModuleSummary(outVInfo))
                {
                    vector<CDbVnaRegModuleSummary>::iterator it = outVInfo.begin();
                    for(; it != outVInfo.end(); ++it)
                    {
                        if(pVNAInfo->GetUUID() != it->GetVnaUuid())
                        {
                            continue ;
                        }
                
                        CDbVnaRegistModule cDbRegMod;
                        cDbRegMod.SetModuleUuid(it->GetModuleUuid().c_str());
                        if(VNET_DB_SUCCESS == pOperVNARegMod->QueryRegModule(cDbRegMod))
                        {
                            CVNARegModInfo cRegMod;
                            cRegMod.SetUUID(cDbRegMod.GetModuleUuid());
                            cRegMod.SetRegModRole(cDbRegMod.GetRole());
                            cRegMod.SetRegModState(cDbRegMod.GetIsOnline());
                            cRegMod.SetRegModApp(cDbRegMod.GetModuleName());
                            if(0 == cDbRegMod.GetIsHasHcInfo())
                            {
                                cRegMod.SetExtInfo(string(""));
                            }
                            else
                            {
                                cRegMod.SetExtInfo(cDbRegMod.GetClusterName());
                            }
                            
                            pVNAInfo->AddRegModInfo(cRegMod);
                        }
                    }
                }

                m_mapVNAInfo.insert(make_pair(pVNAInfo->GetUUID(), pVNAInfo));                
            }// end for(; itrVNA...
        }

        return 0;       
    }

    int32 CVNAManager::AddVNA2DB(CVNAInfo *pVNAInfo, int32 &bNewVna)
    {
        if(NULL == pVNAInfo)
        {
            VNET_ASSERT(0);
            return -1;
        }
        CVNetDbMgr * pMgr = CVNetDbMgr::GetInstance();
        if(NULL == pMgr)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNAManager::AddVNA2DB: CVNetDbMgr::GetInstance() is NULL.\n");
            return -1;
        }
        CDBOperateVna *pOperVNA = pMgr->GetIVna();
        if(NULL == pOperVNA)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNAManager::AddVNA2DB: CDBOperateVna is NULL.\n");
            return -1;
        }

        CDBOperateVnaRegModule *pOperVNARegMod = pMgr->GetIVnaRegModule();
        if(NULL == pOperVNARegMod)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNAManager::AddVNA2DB: GetIVnaRegModule() == NULL.\n");
            return -1;
        }
        
        // 由于VNM集群中节点变更，注册迁移到此VNM的VNA处理;
        vector<CDbVnaSummary> outVNAInfo;
        int32 nRet = pOperVNA->QueryVnaSummary(outVNAInfo);
        if(VNET_DB_SUCCESS == nRet)
        {
            vector<CDbVnaSummary>::iterator itr = outVNAInfo.begin();
            for(; itr != outVNAInfo.end(); ++itr)
            {
                if(itr->GetApp() == pVNAInfo->GetApp())
                {
                    VNET_LOG(VNET_LOG_WARN, "CVNAManager::AddVNA2DB: The registered VNA[App:%s] migrate to the VNM[App:%s].\n",
                        pVNAInfo->GetApp().c_str(), m_strVNMApp.c_str());
                    return 0;
                }
            }
        }

        // 需要事务支持;
        CDbVna cDbVNAInfo;
        cDbVNAInfo.SetUuid(pVNAInfo->GetUUID().c_str());
        cDbVNAInfo.SetVnaApp(pVNAInfo->GetApp().c_str());
        cDbVNAInfo.SetVnmApp(pVNAInfo->GetVNMApp().c_str());
        cDbVNAInfo.SetHostName(pVNAInfo->GetHost().c_str());
        cDbVNAInfo.SetOsName(pVNAInfo->GetOSName().c_str());
        cDbVNAInfo.SetIsOnline((int32)(pVNAInfo->GetState()));
        
        nRet = pOperVNA->AddVna(cDbVNAInfo);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNAManager::AddVNA2DB: call AddVna failed. ret = %d.\n",
                nRet);
            return -1;
        }

        vector<CVNARegModInfo>::iterator itr = pVNAInfo->GetRegistedModInfo().begin();        
        for(; itr != pVNAInfo->GetRegistedModInfo().end(); ++itr)
        {
            CDbVnaRegistModule cDbRegModInfo;
            cDbRegModInfo.SetVnaUuid(pVNAInfo->GetUUID().c_str());
            cDbRegModInfo.SetModuleName(itr->GetRegModApp().c_str());
            cDbRegModInfo.SetRole((int32)(itr->GetRegModRole()));
            cDbRegModInfo.SetIsOnline((int32)(itr->GetRegModState()));
            if(itr->GetExtInfo().empty())
            {
                cDbRegModInfo.SetIsHasHcInfo(0);
            }
            else
            {                
                cDbRegModInfo.SetIsHasHcInfo(1);
                cDbRegModInfo.SetClusterName(itr->GetExtInfo().c_str());
            }
            nRet = pOperVNARegMod->AddRegModule(cDbRegModInfo);
            if(VNET_DB_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, "CVNAManager::AddVNA2DB: call AddRegModule[Name: %s] failed. ret = %d.\n",
                    itr->GetRegModApp().c_str(), nRet);
                return -1;
            }
        }

        //该VNA 第一次注册
        bNewVna = TRUE;
        
        return 0;
    }

    int32 CVNAManager::UpdateVNA2DB(CVNAInfo *pVNAInfo)
    {
        if(NULL == pVNAInfo)
        {
            VNET_ASSERT(0);
            return -1;
        }     

        CVNetDbMgr * pMgr = CVNetDbMgr::GetInstance();
        if( NULL == pMgr)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNAManager::UpdateVNA2DB: CVNetDbMgr::GetInstance() is NULL.\n");
            return -1;
        }
        CDBOperateVna *pOperVNA = pMgr->GetIVna();
        if( NULL == pOperVNA )
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNAManager::UpdateVNA2DB: CDBOperateVna is NULL.\n");
            return -1;
        }

        CDBOperateVnaRegModule *pOperVNARegMod = pMgr->GetIVnaRegModule();
        if( NULL == pOperVNARegMod )
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNAManager::UpdateVNA2DB: GetIVnaRegModule() == NULL.\n");
            return -1;
        }
        
        CVNAInfo cVNA;
        if(0 != GetVNAInfoFromDB(pVNAInfo->GetUUID(), cVNA))
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNAManager::UpdateVNA2DB: call GetVNAInfoFromDB(%s) failed.\n",
                pVNAInfo->GetUUID().c_str());
            return -1;
        }
        
        // 需要事务支持;
        int32 nRet = 0;
        // Update VNA
        if(cVNA.GetHost() != pVNAInfo->GetHost() ||
           cVNA.GetVNMApp() != pVNAInfo->GetVNMApp() ||
           cVNA.GetState() != pVNAInfo->GetState())
        {
            CDbVna cDbVNAInfo;
            cDbVNAInfo.SetUuid(pVNAInfo->GetUUID().c_str());
            cDbVNAInfo.SetVnaApp(pVNAInfo->GetApp().c_str());
            cDbVNAInfo.SetVnmApp(pVNAInfo->GetVNMApp().c_str());
            cDbVNAInfo.SetHostName(pVNAInfo->GetHost().c_str());
            cDbVNAInfo.SetOsName(pVNAInfo->GetOSName().c_str());
            cDbVNAInfo.SetIsOnline((int32)(pVNAInfo->GetState()));
            
            nRet = pOperVNA->ModifyVna(cDbVNAInfo);
            if(VNET_DB_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, "CVNAManager::UpdateVNA2DB: call ModifyVna failed. ret = %d.\n",
                    nRet);
                return -1;
            }
        }
        
        // Update VNA's Modulers
        vector<CVNARegModInfo>::iterator itr = pVNAInfo->GetRegistedModInfo().begin();
        for( ; itr != pVNAInfo->GetRegistedModInfo().end(); ++itr)
        {
            CDbVnaRegistModule cDbVnaRegModinfo;
            CVNARegModInfo *pRegMod = cVNA.GetRegModInfo(itr->GetRegModApp(), itr->GetRegModRole());
            if(NULL == pRegMod)
            {
                // Add to DB
                cDbVnaRegModinfo.SetVnaUuid(pVNAInfo->GetUUID().c_str());
                cDbVnaRegModinfo.SetModuleName(itr->GetRegModApp().c_str());
                cDbVnaRegModinfo.SetRole((int32)(itr->GetRegModRole()));
                cDbVnaRegModinfo.SetIsOnline((int32)(itr->GetRegModState()));
                if(itr->GetExtInfo().empty())
                {
                    cDbVnaRegModinfo.SetIsHasHcInfo(0);
                }
                else
                {                
                    cDbVnaRegModinfo.SetIsHasHcInfo(1);
                    cDbVnaRegModinfo.SetClusterName(itr->GetExtInfo().c_str());
                }
                nRet = pOperVNARegMod->AddRegModule(cDbVnaRegModinfo);
                if(VNET_DB_SUCCESS != nRet)
                {
                    VNET_LOG(VNET_LOG_ERROR, "CVNAManager::UpdateVNA2DB: Add VNA(%s) reg module(%s) failed. ret = %d.\n",
                        cDbVnaRegModinfo.GetVnaUuid().c_str(), 
                        cDbVnaRegModinfo.GetModuleName().c_str(),
                        nRet);
                    if(m_bOpenDbgInf)
                    {
                        cDbVnaRegModinfo.DbgShow();
                    }
                    return -1;
                }
                itr->SetUUID(cDbVnaRegModinfo.GetModuleUuid());
            }
            else
            {
                // Update to DB
                if(pRegMod->GetRegModApp() != itr->GetRegModApp() ||
                   pRegMod->GetRegModRole() != itr->GetRegModRole() ||
                   pRegMod->GetRegModState() != itr->GetRegModState() ||
                   pRegMod->GetExtInfo() != itr->GetExtInfo())
                {
                    cDbVnaRegModinfo.SetVnaUuid(pVNAInfo->GetUUID().c_str());
                    cDbVnaRegModinfo.SetModuleUuid(itr->GetUUID().c_str());
                    cDbVnaRegModinfo.SetModuleName(itr->GetRegModApp().c_str());
                    cDbVnaRegModinfo.SetRole((int32)(itr->GetRegModRole()));
                    cDbVnaRegModinfo.SetIsOnline((int32)(itr->GetRegModState()));
                    if(itr->GetExtInfo().empty())
                    {
                        cDbVnaRegModinfo.SetIsHasHcInfo(0);
                    }
                    else
                    {                
                        cDbVnaRegModinfo.SetIsHasHcInfo(1);
                        cDbVnaRegModinfo.SetClusterName(itr->GetExtInfo().c_str());
                    }
                    
                    nRet = pOperVNARegMod->ModifyRegModule(cDbVnaRegModinfo);
                    if(VNET_DB_SUCCESS != nRet)
                    {
                        VNET_LOG(VNET_LOG_ERROR, "CVNAManager::UpdateVNA2DB: call ModifyRegModule failed. ret = %d.\n",
                            nRet);
                        if(m_bOpenDbgInf)
                        {
                            cDbVnaRegModinfo.DbgShow();
                        }
                        return -1;
                    }
                } 
            }// end else
        }// end for( ;...
        
        return 0;
    }
    
    int32 CVNAManager::DeleteVNA2DB(CVNAInfo *pVNAInfo)
    {
        // 当HC被人为移除CC时考虑这个接口;
        // CC+HC信息=>VNA APP=>进行通知VNA删除网络资源;
        // 需要工作流支持;vnetlib新增消息接口后处理;
        // 需要事务支持;

        CDBOperateVna *pOperVNA = GetDbIVna();
        if( NULL == pOperVNA )
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNAManager::UpdateVNA2DB: CDBOperateVna is NULL.\n");
            return DB_ERROR_GET_INST_FAILED;
        }
        
        // 删除DB
        int32 dbRet = pOperVNA->DelVna(pVNAInfo->GetUUID().c_str());
        if( (dbRet != VNET_DB_SUCCESS) && !VNET_DB_IS_ITEM_NO_EXIST(dbRet))
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNAManager::DeleteVNA2DB: The VNA[uuid = %s] delvna failed. ret:%d.\n", 
                    pVNAInfo->GetUUID().c_str(), dbRet);
            return ERROR_VNA_DELETE_DB_OPER_FAILED; 
        }
        
        return 0;
    }
    
    int32 CVNAManager::GetVNAInfoFromDB(const string &cstrUUID, CVNAInfo &cVNAInfo)
    {
        if(cstrUUID.empty())
        {
            return -1;
        }
        
        // Find VNA Info from DB.        
        CVNetDbMgr * pMgr = CVNetDbMgr::GetInstance();
        if( NULL == pMgr)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNAManager::GetVNAInfoFromDB: CVNetDbMgr::GetInstance() is NULL.\n");
            return -1;
        }
        CDBOperateVna *pOperVNA = pMgr->GetIVna();
        if( NULL == pOperVNA )
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNAManager::GetVNAInfoFromDB: CDBOperateVna is NULL.\n");
            return -1;
        }

        CDBOperateVnaRegModule *pOperVNARegMod = pMgr->GetIVnaRegModule();
        if( NULL == pOperVNARegMod )
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNAManager::GetVNAInfoFromDB: GetIVnaRegModule() == NULL.\n");
            return -1;
        }
        
        CDbVna cDbVnaInfo;
        cDbVnaInfo.SetUuid(cstrUUID.c_str());
        int32 nRet = pOperVNA->QueryVna(cDbVnaInfo);
        if(VNET_DB_SUCCESS != nRet)
        {
            return -1;
        }

        cVNAInfo.SetUUID(cstrUUID);
        cVNAInfo.SetApp(cDbVnaInfo.GetVnaApp());
        cVNAInfo.SetVNMApp(cDbVnaInfo.GetVnmApp());
        cVNAInfo.SetHost(cDbVnaInfo.GetHostName());        
        cVNAInfo.SetState(cDbVnaInfo.GetIsOnline());
        cVNAInfo.SetOSName(cDbVnaInfo.GetOsName());

        // 查询接口优化后修改;
        vector<CDbVnaRegModuleSummary> outVInfo;
        nRet = pOperVNARegMod->QueryRegModuleSummary(outVInfo);
        if(0 == nRet)
        {
            vector<CDbVnaRegModuleSummary>::iterator itr = outVInfo.begin();
            for( ; itr != outVInfo.end(); ++itr)
            {
                if(cstrUUID != itr->GetVnaUuid())
                {
                    continue ;
                }

                CDbVnaRegistModule cDbRegMod;
                cDbRegMod.SetModuleUuid(itr->GetModuleUuid().c_str());
                if(0 == pOperVNARegMod->QueryRegModule(cDbRegMod))
                {
                    CVNARegModInfo cRegMod;
                    cRegMod.SetUUID(cDbRegMod.GetModuleUuid());
                    cRegMod.SetRegModRole(cDbRegMod.GetRole());
                    cRegMod.SetRegModState(cDbRegMod.GetIsOnline());
                    cRegMod.SetRegModApp(cDbRegMod.GetModuleName());
                    if(0 == cDbRegMod.GetIsHasHcInfo())
                    {
                        cRegMod.SetExtInfo(string(""));
                    }
                    else
                    {
                        cRegMod.SetExtInfo(cDbRegMod.GetClusterName());
                    }
                    
                    cVNAInfo.AddRegModInfo(cRegMod);
                }
            }
        }
        
        return 0;
    }

    int32 CVNAManager::ClearAllVNAInfo(void)
    {
        map<string, CVNAInfo *>::iterator pos = m_mapVNAInfo.begin();
        for (; pos != m_mapVNAInfo.end(); ) 
        {
            CVNAInfo *p = pos->second;
            m_mapVNAInfo.erase(pos++);
            delete p;
            p = NULL;
        }
        return 0;
    }
    
    int32 CVNAManager::ProcVNARegReq(const MessageFrame &message)
    {
        CVNAReportMsg cMsg;
        CVNARegModReqMsg cAckMsg;
        cMsg.deserialize(message.data);

        if(cMsg.m_strVNAApp.empty())
        {
            VNET_LOG(VNET_LOG_WARN, "CVNAManager::ProcVNARegReq: VNA application is null.\n");
            return -1;
        }
        
        CVNAInfo *pVNAInfo = GetVNAInfo(cMsg.m_strVNAApp);        
        if(pVNAInfo)
        {
            // 重复注册            
            pVNAInfo->SetState(EN_STATE_INIT);
            UpdateVNAInfo(pVNAInfo, &cMsg);
            if(m_bOpenDbgInf)
            {
                VNET_LOG(VNET_LOG_WARN, "CVNAManager::ProcVNARegReq: Update VNA regist request successfully.\n");
                cMsg.Print();
            }
            // 回复应答
            cAckMsg.m_strVNAName = cMsg.m_strVNAApp;
            MessageOption option(message.option.sender(),EV_VNA_REGISTER_REQ,0,0);
            m_pMU->Send(cAckMsg, option);
            return 0;
        }
 
        // 新VNA注册;
        pVNAInfo = new CVNAInfo();
        if(NULL == pVNAInfo)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNAManager::ProcVNARegReq: failed to allocate memory!\n");
            return -1;
        }        
           
        pVNAInfo->SetApp(cMsg.m_strVNAApp);
        pVNAInfo->SetUUID(cMsg.m_strVNAApp);
        pVNAInfo->SetState(EN_STATE_INIT);
        pVNAInfo->SetHost(cMsg.m_strHostName);
        string strOSName = "NA";
        pVNAInfo->SetOSName(strOSName);
        pVNAInfo->SetDelayUpdateTime(0);        
        pVNAInfo->SetAgeingTime(0);
        vector<CVNARegModMsg>::iterator itr = cMsg.m_vecRegistedModInfo.begin();
        for(; itr != cMsg.m_vecRegistedModInfo.end(); ++itr)
        {
            CVNARegModInfo cRegMod;
            if(itr->m_strModApp.empty())
            {                 
                VNET_ASSERT(0);                    
                VNET_LOG(VNET_LOG_ERROR, "CVNAManager::ProcVNARegReq: Msg error, VNA registed moduler name is NULL.\n");
                cMsg.Print();
                delete pVNAInfo;
                pVNAInfo = NULL;        
                return -1;
            }  
            cRegMod.SetRegModRole(itr->m_ucModRole);                
            cRegMod.SetRegModState(itr->m_ucModState);
            cRegMod.SetRegModApp(itr->m_strModApp);
            cRegMod.SetExtInfo(itr->m_strExtInfo);
            pVNAInfo->AddRegModInfo(cRegMod);
        }

        m_mapVNAInfo.insert(make_pair(cMsg.m_strVNAApp, pVNAInfo));
        if(m_bOpenDbgInf)
        {
            VNET_LOG(VNET_LOG_INFO, "CVNAManager::ProcVNARegReq: Process VNA regist request successfully.\n");
            cMsg.Print();
        }

        // 回复注册;
        cAckMsg.m_strVNAName = cMsg.m_strVNAApp;
        MessageOption option(message.option.sender(),EV_VNA_REGISTER_REQ,0,0);
        m_pMU->Send(cAckMsg, option);
        
        return 0;  
    }

    int32 CVNAManager::ProcVNAReport(const MessageFrame &message)
    {
        CVNAReportMsg cMsg;
        cMsg.deserialize(message.data);

        if(cMsg.m_strVNAApp.empty())
        {
            VNET_LOG(VNET_LOG_WARN, "CVNAManager::ProcVNAReport: VNA application is null.\n");
            cMsg.Print();
            return -1;
        }
        
        CVNAInfo *pVNAInfo = GetVNAInfo(cMsg.m_strVNAApp);        
        if(NULL == pVNAInfo)
        {
            VNET_LOG(VNET_LOG_WARN, "CVNAManager::ProcVNAReport: The VNA[APP = %s] is not registed.\n", 
                cMsg.m_strVNAApp.c_str());
            // 没有注册;            
            return -1;
        }
        
        int32 bNewVna = FALSE;
        
        // 首次确定为这个VNM为管理者
        if(pVNAInfo->GetVNMApp().empty())
        {            
            pVNAInfo->SetVNMApp(m_strVNMApp);
            if(0 != AddVNA2DB(pVNAInfo,bNewVna))
            {
                VNET_LOG(VNET_LOG_ERROR, "CVNAManager::ProcVNAReport: call AddVNA2DB[App: %s] failed!\n", 
                    pVNAInfo->GetApp().c_str());
                return -1;
            }
        }
        // 更新信息;
        int32 bStateChg = 0;
        if(pVNAInfo->GetState() != EN_STATE_ONLINE)
        {
            /* Online 以后资源才可用; */
            pVNAInfo->SetState(EN_STATE_ONLINE); 
            bStateChg = 1;
        }  
        CVNAInfo cVNA;
        cVNA.Copy(*pVNAInfo);
        // 消息处理
        UpdateVNAInfo(pVNAInfo, &cMsg);
        // 分发消息到各模块
        DispatchMsg(&cMsg);
        
        // 触发通配流程;
        if(TRUE == bNewVna)
        {
            // 将消息发送给port switch 上报处理mu，
            CNewVnaNotify msgNotify;
            msgNotify.m_strNewVnaUuid = pVNAInfo->GetUUID();
           
            MID mid;
            mid._application = ApplicationName();
            mid._process = PROC_VNM;
            mid._unit = MU_VNM_VNA_REPORT;
            
            MessageOption option(mid, EV_VNM_NEWVNA_NOTIFY,0,0);
            m_pMU->Send(msgNotify,option);
            
        }
        
        // Save To DB
        if(0 != UpdateVNA2DB(pVNAInfo))
        {
            pVNAInfo->SetState(EN_STATE_INIT);
            bStateChg = 0;
            if(m_bOpenDbgInf)
            {
                VNET_LOG(VNET_LOG_INFO, "CVNAManager::ProcVNAReport: Process VNA report msg failed.\n");
                cMsg.Print();
            }
            pVNAInfo->Copy(cVNA);
            return -1;
        }

        if(bStateChg)
        {
            CUpdateCfgToVNA msgUpdateVNA;
            msgUpdateVNA.m_strVnaUuid = pVNAInfo->GetUUID();
            MID mid;
            mid._application = ApplicationName();
            mid._process = PROC_VNM;
            mid._unit    = MU_VNM_VNA_REPORT;
            
            MessageOption option_vna(mid, EV_VNM_CFGINFO_TO_VNA,0,0);
            m_pMU->Send(msgUpdateVNA, option_vna);
            //NotifyStateChanged(pVNAInfo->GetUUID(), EN_STATE_ONLINE);
        }
        
        if(m_bOpenDbgInf)
        {
            VNET_LOG(VNET_LOG_INFO, "CVNAManager::ProcVNAReport: Process VNA report msg successfully.\n");
            cMsg.Print();
        }
        
        return 0;
    }
    
    int32 CVNAManager::ProcVNAHeartbeat(const MessageFrame &message)
    { 
        MID sender = message.option.sender();
        CVNAInfo *pVNAInfo = GetVNAInfo(sender._application);        
        if(NULL == pVNAInfo)
        {
            if(m_bOpenDbgInf)
            {
                VNET_LOG(VNET_LOG_WARN, "CVNAManager::ProcVNAHeartbeat: The VNA[APP = %s] is not registed.\n", 
                    sender._application.c_str());
            }
            // 没有注册;            
            return -1;
        }
        
        if(pVNAInfo->GetState() != EN_STATE_ONLINE)
        {
            if(m_bOpenDbgInf)
            {
                VNET_LOG(VNET_LOG_WARN, "CVNAManager::ProcVNAHeartbeat: The VNA[APP = %s] is not online.\n", 
                    sender._application.c_str());
            }
            return -1;
        }
        pVNAInfo->SetDelayUpdateTime(0);
        
        return 0;
    }

    int32 CVNAManager::ProcVNADelete(const MessageFrame &message)    
    {
        CVNetVnaDelete    cMsg;
        CVNetVnaDeleteAck cAck;
        cMsg.deserialize(message.data);
        MID   mid  = message.option.sender();
        MessageOption option(message.option.sender(), EV_VNETLIB_DEL_VNA_ACK, 0, 0);
    
        if(m_bOpenDbgInf)
        {
            cout << "recv delete vna msg......" << endl;
            cMsg.Print();
        }
        
        int32 nRet = 0;        
        int32 dbRet =0;
        CVNAInfo cVNA;

        //CVNAInfo *pVNAInfo = GetVNAInfo(sender._application);        
        CVNAInfo *pVNAInfo = GetVNAInfo(cMsg.m_strUuid);                
        if(NULL != pVNAInfo)
        {
            if(pVNAInfo->GetState() == EN_STATE_ONLINE)
            {
                if(m_bOpenDbgInf)
                {
                    VNET_LOG(VNET_LOG_WARN, "CVNAManager::ProcVNADelete: The VNA[uuid = %s] is online.\n", 
                        cMsg.m_strUuid.c_str());
                }
                nRet = ERROR_VNA_AGENT_MODULE_ONLINE;            
                goto PROC_DEL_VNA_ACK;
            }
        }
       
        // 删除内存 
        DelVNAInfo(cMsg.m_strUuid);
        
        // 删除DB
        if( 0 != GetVNAInfoFromDB(cMsg.m_strUuid, cVNA) )
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNAManager::ProcVNADelete: The VNA[uuid = %s] delvna failed.\n", 
                    cMsg.m_strUuid.c_str());
            nRet = DB_ERROR_GET_INST_FAILED;                                 
            goto PROC_DEL_VNA_ACK;
        }
        
        dbRet = DeleteVNA2DB(&cVNA);
        if( dbRet != VNET_DB_SUCCESS )
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNAManager::ProcVNADelete: The VNA[uuid = %s] delvna failed. ret:%d.\n", 
                    cMsg.m_strUuid.c_str(), dbRet);
            nRet = ERROR_VNA_DELETE_DB_OPER_FAILED;                                 
            goto PROC_DEL_VNA_ACK;
        }
        
PROC_DEL_VNA_ACK:
        cAck.m_strUuid  = cMsg.m_strUuid;
        cAck.SetResult(nRet);
        cAck.SetResultinfo(GetVnetErrorInfo(nRet));
        if(m_bOpenDbgInf)
        {
            cout << "send delete vna ack msg......" << endl;
            cAck.Print();
        }
        return m_pMU->Send(cAck, option);
    }

    int32 CVNAManager::ProcVNAMoniTmOut(void)
    {
        map<string, CVNAInfo *>::iterator itr = m_mapVNAInfo.begin();
        for(; itr != m_mapVNAInfo.end();)
        {
            CVNAInfo *pVNAInfo = itr->second;        
            uint32 dwTime = pVNAInfo->GetDelayUpdateTime();
            if(EN_STATE_ONLINE == pVNAInfo->GetState())
            {
                if(dwTime > 3)
                {
                    ENObjState enState = EN_STATE_OFFLINE;
                    pVNAInfo->SetState(EN_STATE_OFFLINE);
                    // Save To DB
                    if(0 != UpdateVNA2DB(pVNAInfo))
                    {
                        pVNAInfo->SetState(EN_STATE_INIT);
                        enState = EN_STATE_INIT;
                    }
                    NotifyStateChanged(pVNAInfo->GetUUID(), enState);
                }
                else
                {
                    pVNAInfo->SetDelayUpdateTime(++dwTime);
                }
            }

            // 老化没有向此VNM注册的VNA信息;
            dwTime = pVNAInfo->GetAgeingTime();
            if(pVNAInfo->GetVNMApp() != m_strVNMApp)
            {
                if(dwTime > 10)
                {
                    m_mapVNAInfo.erase(itr++);
                    delete pVNAInfo;
                    pVNAInfo = NULL;
                    continue ;
                }
                else
                {
                    pVNAInfo->SetAgeingTime(++dwTime);
                }
            }
            
            ++itr;
        }

        /*定时同步vxlan隧道表*/
        CVxlanMgr *pVxlanMgr = CVxlanMgr::GetInstance();
        if (pVxlanMgr)
        {
            for(itr = m_mapVNAInfo.begin(); itr != m_mapVNAInfo.end(); itr++)
            {
                CVNAInfo *pVNAInfo = itr->second;                
                pVxlanMgr->SendMcGroupInfo(pVNAInfo->GetUUID());
            }
        }        
        
        return 0;
    }

    int32 CVNAManager::NotifyStateChanged(const string &cstrVNAUUID, ENObjState enState)
    {
        if(cstrVNAUUID.empty())
        {
            VNET_LOG(VNET_LOG_WARN, "CVNAManager::NotifyStateChanged: The VNA uuid is empty.\n");
            return -1;
        }
        
        CSwitchMgr *pSwitchMgr;
        CPortMgr *pPortMgr;
        switch(enState)
        {
        case EN_STATE_INIT:
            {
                //通知DVS状态变更
                pSwitchMgr = CSwitchMgr::GetInstance();
                if(pSwitchMgr)
                {
                    pSwitchMgr->UpdateSwitchToOffline(cstrVNAUUID);
                }

               pPortMgr = CPortMgr::GetInstance();
               if(pPortMgr)
               {
                  pPortMgr->SetPortCfgOffline(cstrVNAUUID);
               }

            }
            break;
        case EN_STATE_OFFLINE:
            {
                //通知DVS状态变更
                pSwitchMgr = CSwitchMgr::GetInstance();
                if(pSwitchMgr)
                {
                    pSwitchMgr->UpdateSwitchToOffline(cstrVNAUUID);
                }

               pPortMgr = CPortMgr::GetInstance();
               if(pPortMgr)
               {
                  pPortMgr->SetPortCfgOffline(cstrVNAUUID);
               }

            }
            break;            
        case EN_STATE_ONLINE:
            {
                //下发DVS数据配置
                pSwitchMgr = CSwitchMgr::GetInstance();
                if(pSwitchMgr)
                {
                    pSwitchMgr->UpdateVNASwitch(cstrVNAUUID);
                }

               pPortMgr = CPortMgr::GetInstance();
               if(pPortMgr)
               {
                  pPortMgr->SetPortCfgToVnaTimer(cstrVNAUUID);
               }
				
            }
            break;
        default:
            {
                VNET_LOG(VNET_LOG_WARN, "CVNAManager::NotifyStateChanged: The VNA state(%d) is unkown.\n", 
                    enState);
                return -1;
            }
        }
        
        return 0;
    }
    
    int32 CVNAManager::DispatchMsg(CVNAReportMsg *pMsg)
    {
        if(NULL == pMsg)
        {
            VNET_ASSERT(0);
            return -1;
        }
        
        // 此消息转发给vna report 消息单元处理(仅处理switch, port) 
        MID mid;
        mid._application = ApplicationName();
        mid._process = PROC_VNM;
        mid._unit = MU_VNM_VNA_REPORT;

        MessageOption option(mid, EV_VNA_INFO_REPORT,0,0);
        m_pMU->Send(*pMsg,option);
        
        return 0;
    }

    void CVNAManager::DbgShowVNAApps(void)
    {
        if(m_mapVNAInfo.empty())
        {
            return ;
        }
        
        cout << "----------------------------------------------------------------------------" << endl;
        map<string, CVNAInfo *>::iterator itr = m_mapVNAInfo.begin();
        for(; itr != m_mapVNAInfo.end(); ++itr)
        {
            cout <<"VNA application: {" << itr->first <<"}." << endl;
        }        
        cout << "----------------------------------------------------------------------------" << endl;
        
        return ;
    }

    
    void CVNAManager::DbgPrintVNA(CVNAInfo *pVNA)
    {
        if(NULL == pVNA)
        {
            return ;
        }
        
        cout << "----------------------------------------------------------------------------" << endl;
        cout << "VNA application: {" << pVNA->GetApp() << "}." <<endl;        
        cout << "VNM application: {" << pVNA->GetVNMApp() << "}." <<endl;
        if(EN_STATE_ONLINE == pVNA->GetState())
        {
            cout << "State: online";
        }
        else
        {
            cout << "State: offline";
        }
        cout << ", HostName: "<< pVNA->GetHost() 
             << ", OSName: " << pVNA->GetOSName()
             << ", DelayUpdateTime: " << pVNA->GetDelayUpdateTime()*MONI_VNA_INTERVAL/1000 << "s." << endl;

        if(pVNA->GetRegistedModInfo().empty())
        {
            return ;
        }
        
        vector<CVNARegModInfo>::iterator itr = pVNA->GetRegistedModInfo().begin();        
        for(; itr != pVNA->GetRegistedModInfo().end(); ++itr)
        {
            cout << "ModID: " << itr->GetUUID();
            cout << ", Name: " << itr->GetRegModApp();
            cout << ", RoleID: " << (int32)(itr->GetRegModRole());
            if(EN_STATE_ONLINE == itr->GetRegModState())
            {
                cout << ", online";
            }
            else
            {
                cout << ", offline";
            }

            if(!itr->GetExtInfo().empty())     
            {
                cout << ", Ext Info: " << itr->GetExtInfo() <<".";
            }
            else
            {
                cout << ", NO Ext Info.";
            }          
   
            cout << endl;
        }

        return ;        
    }

    void CVNAManager::DbgShowVNA(const string &strVNAApp)
    {
        CVNAInfo *p = GetVNAInfo(strVNAApp);
        if(p)
        {
            cout << "@Memory cache:" << endl;
            DbgPrintVNA(p);
        }

        CVNAInfo cVNA;
        if(0 == GetVNAInfoFromDB(strVNAApp, cVNA))
        {
            cout << "----------------------------------------------------------------------------" << endl;
            cout << "@Database:" << endl;            
            DbgPrintVNA(&cVNA);
        }
        return ;
    }

    void VNetDbgShowVNAApps(void)
    {
        CVNAManager *pVNAMgr = CVNAManager::GetInstance();
        if(pVNAMgr)
        {
            pVNAMgr->DbgShowVNAApps();
        }
        
        return ;
    }
    DEFINE_DEBUG_FUNC(VNetDbgShowVNAApps);

    void VNetDbgShowVNA(const char* cstrVNAApp)
    {
        if(NULL == cstrVNAApp)
        {
            return ;
        }
        
        CVNAManager *pVNAMgr = CVNAManager::GetInstance();
        string strApp;
        strApp.assign(cstrVNAApp);
        if(pVNAMgr)
        {
            pVNAMgr->DbgShowVNA(strApp);
        }
    }
    DEFINE_DEBUG_FUNC(VNetDbgShowVNA);

    void VNetDbgSetVNARegPrint(BOOL bOpen)
    {
        CVNAManager *pVNAMgr = CVNAManager::GetInstance();
        if(pVNAMgr)
        {
            pVNAMgr->SetDbgInfFlg(bOpen);
        }
    }
    DEFINE_DEBUG_FUNC(VNetDbgSetVNARegPrint);

    void VNetDbgTestVNAOper(const char* cstrVNAApp, 
                            const char* cstrHostName, 
                            const char* cstrModName,
                            const char* cstrModExtInfo,
                            uint32 dwOper = 1)
    {
        if(NULL == cstrVNAApp || 
           NULL == cstrHostName)
        {
            cout << "cstrVNAApp or cstrHostName is NULL." <<endl;
            return ;
        }
        
        cout << "Operation Code: reg[1], report[2]." <<endl;
        if((dwOper < 1) || (dwOper > 2))
        {
            return ;
        }

        if(2 == dwOper && NULL == cstrModName)
        {
            cout << "cstrModName is NULL." <<endl;
            return ;
        }

        MessageUnit tempmu(TempUnitName("TestVNAOper"));
        tempmu.Register();
        CVNAReportMsg cMsg;
        string strTmp;
        strTmp.assign(cstrVNAApp);
        cMsg.m_strVNAApp = strTmp;
        strTmp.clear();
        strTmp.assign(cstrHostName);
        cMsg.m_strHostName = strTmp;
        strTmp.clear();
        if(1 == dwOper)
        {
            MessageOption option(MU_VNM, EV_VNA_REGISTER_TO_VNM, 0, 0);            
            tempmu.Send(cMsg,option);
            return ;
        }
        
        if(2 == dwOper)
        {
            CVNARegModMsg cModMsg;
            cModMsg.m_ucModRole = VNA_MODULE_ROLE_HC; 
            cModMsg.m_ucModState = EN_STATE_ONLINE; 
            strTmp.assign(cstrModName);
            cModMsg.m_strModApp = strTmp;
            strTmp.clear();
            if(NULL != cstrModExtInfo)
            {
                strTmp.assign(cstrModExtInfo);
                cModMsg.m_strExtInfo = strTmp;
                strTmp.clear();        
            }        
            cMsg.m_vecRegistedModInfo.push_back(cModMsg);
            MessageOption option(MU_VNM, EV_VNA_INFO_REPORT, 0, 0);            
            tempmu.Send(cMsg,option);
        }
        
        return ;
    }
    DEFINE_DEBUG_FUNC(VNetDbgTestVNAOper);

}// end namespace zte_tecs

