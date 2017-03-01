/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vnm_controller.cpp
* 文件标识：
* 内容摘要：CVNMCtrl类实现文件
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
#include "vnm_pub.h"
#include "vnet_comm.h"
#include "vnet_event.h"
#include "vnet_msg.h"
#include "vnet_mid.h"
#include "vnm_controller.h"
#include "vnetlib_event.h"
#include "vnet_error.h"
#include "vnet_db_quantum_cfg.h"
#include "vnet_function.h"


namespace zte_tecs
{
#define MC_VNM_INFO_INTERVAL    (5000)  // 5s扫描主播一次VNM信息;

    CVNMCtrl *CVNMCtrl::s_pInstance = NULL;
    
    CVNMCtrl::CVNMCtrl()
    {
        m_tMcVNMInfoTimerID = INVALID_TIMER_ID;
        m_ucMasterOrSlave = VNM_SLAVE_STATE;
        m_ucPriority = 0;
        m_pMU = NULL;
    }
    
    CVNMCtrl::~CVNMCtrl()
    {
        if(INVALID_TIMER_ID != m_tMcVNMInfoTimerID)
        {
            if(m_pMU)
            {
                m_pMU->KillTimer(m_tMcVNMInfoTimerID);  // 析构中此操作是否集中到一个下电函数中???;
                m_tMcVNMInfoTimerID = INVALID_TIMER_ID;
            }
        }
        m_ucMasterOrSlave = VNM_SLAVE_STATE;
        m_ucPriority = 0;
        m_pMU = NULL;
    }
    
    int32 CVNMCtrl::Init(MessageUnit *pMu, const string &cstrVNMApp)
    {
        if (pMu == NULL)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNMCtrl::Init: pMu is NULL.\n");
            return -1;
        }

        if(cstrVNMApp.empty())
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNMCtrl::Init: cstrVNMApp is empty.\n");
            return -1;
        }
        
        m_pMU = pMu;

        // Init VNM information Muticast timer
        m_tMcVNMInfoTimerID = m_pMU->CreateTimer();
        if (INVALID_TIMER_ID == m_tMcVNMInfoTimerID)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNMCtrl::Init: call CreateTimer failed.\n");
            return -1;
        }

        TimeOut timeout;
        timeout.type = LOOP_TIMER;
        timeout.duration = MC_VNM_INFO_INTERVAL;
        timeout.msgid = EV_MC_VNM_INFO_TIMER;
        timeout.arg = 0;
        if(SUCCESS != m_pMU->SetTimer(m_tMcVNMInfoTimerID, timeout))
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNMCtrl::Init: call SetTimer failed.\n");
            m_pMU->KillTimer(m_tMcVNMInfoTimerID);
            m_tMcVNMInfoTimerID = INVALID_TIMER_ID;
            return -1;
        }

        m_strApp = cstrVNMApp;        
        // m_ucMasterOrSlave
        m_ucMasterOrSlave = VNM_MASTER_STATE; // 后续通过竞争得出。
        m_ucPriority = 0; // 需要通过配置文件中读出。
        
        return 0;
    }
        
    void CVNMCtrl::MessageEntry(const MessageFrame &message)
    {
        switch (message.option.id())
        {
        case EV_MC_VNM_INFO_TIMER:
            {
                ProcMcVNMInfoTmOut();
            }
            break;
        case EV_MG_VNM_INFO:
            {
                ProcVNMInfo(message);
            }
            break;  
        case EV_VNETLIB_SDN_CFG:
            {
                ProcSdnInfo(message);
            }
            break;        
        default:
            break;            
        }
        
        return ;
    } 
    
    int32 CVNMCtrl::ProcVNMInfo(const MessageFrame &message)
    {

        
        return 0;        
    }  

    int32 CVNMCtrl::ProcMcVNMInfoTmOut(void)
    {
        // 定时组播发送VNM信息
        CVNMInfoMsg msg;
        msg.m_ucMasterOrSlave = m_ucMasterOrSlave;
        msg.m_ucPriority = m_ucPriority;
        msg.m_strApp = m_strApp;
        
        MID receiver("group", MUTIGROUP_VNM_MS);
        MessageOption option(receiver, EV_MG_VNM_INFO, 0, 0);
        m_pMU->Send(msg, option);
        return 0;
    }

    int32 CVNMCtrl::ProcSdnInfo(const MessageFrame &message)
    {
        string strRetInfo;
        int32 nRet = 0;

        if(NULL == m_pMU)
        {
            VNET_ASSERT(0);
            return -1;
        }
        
        CSdnCfgMsg cMsg;
        cMsg.deserialize(message.data);
        cMsg.Print();

        MessageOption option(message.option.sender(), EV_VNETLIB_SDN_CFG, 0, 0);
#if 0
        if(!cMsg.IsValidOper())
        {
            VNET_LOG(VNET_LOG_WARN, "CVlanPoolMgr::ProcVlanRangeMsg: Msg operation code (%d) is invalid.\n", 
                cMsg.m_dwOper);
            VNetFormatString(strRetInfo, "Msg operation code (%d) is invalid.\n", cMsg.m_dwOper);
            cMsg.SetResult(VLAN_RANGE_CFG_PARAM_ERROR);
            cMsg.SetResultinfo(strRetInfo);
            m_pMU->Send(cMsg, option);
            return -1;
        }
#endif
        switch(cMsg.m_nSdnOper)
        {
        case EN_ADD_SDNCTRL:
            {
                nRet = AddSdnCtrl(cMsg, strRetInfo);
            }
            break;
        case EN_DEL_SDNCTRL:
            {
                nRet = DelSdnCtrl(cMsg, strRetInfo);
            }
            break;
        case EN_MOD_SDNCTRL:
            {
                nRet = ModSdnCtrl(cMsg, strRetInfo);
            }
            break;
#if 0
        case EN_ADD_QUANTUMRESTFUL:
            {
                nRet = AddQuantumRestful(cMsg, strRetInfo);
            }
            break;
        case EN_DEL_QUANTUMRESTFUL:
            {
                nRet = DelQuantumRestful(cMsg, strRetInfo);
            }
            break;
        case EN_MOD_QUANTUMRESTFUL:
            {
                nRet = ModQuantumRestful(cMsg, strRetInfo);
            }
            break;
#endif
        default:
            {
                VNET_ASSERT(0);
                strRetInfo = "The sdn controller param is invalid.";
                nRet = SDN_CFG_PARAM_ERROR;                
            }
        }
        
        cMsg.SetResult(nRet);
        cMsg.SetResultinfo(strRetInfo);
        m_pMU->Send(cMsg, option);
        
        return 0;
    }

    int32 CVNMCtrl::AddSdnCtrl(const CSdnCfgMsg &cMsg, string &strRetInfo)
    {
        CVNetDbMgr * pMgr = CVNetDbMgr::GetInstance();
        if(NULL == pMgr)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNMCtrl::AddSdnCtrl: GetInstance() is NULL.\n");
            strRetInfo = "The database handler is NULL.";
            return DB_ERROR_GET_INST_FAILED;
        }
        
        CDBOperateSdnCfg * pSdn = pMgr->GetISdnCfg();
        if(NULL == pSdn)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNMCtrl::AddSdnCtrl: GetISdn() is NULL.\n");
            strRetInfo = "The sdn database interface is NULL.";
            return SDN_GET_DB_INTF_ERROR;
        }

        CDBOperateVSwitch * pVSwitch = pMgr->GetIVSwitch();
        if(NULL == pVSwitch)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNMCtrl::AddSdnCtrl: GetIVSwitch() is NULL.\n");
            strRetInfo = "The vsitch database interface is NULL.";
            return SDN_GET_DB_INTF_ERROR;
        }

        // Conflict checking
        CDbSdnCtrlCfg outSdnInfo;
        int32 nRet = pSdn->QuerySdnCtrl(outSdnInfo);
        if(VNET_DB_SUCCESS == nRet)
        {
            if(NULL != outSdnInfo.GetUuid().c_str())
            {                
                VNET_LOG(VNET_LOG_ERROR, "CVNMCtrl::AddSdnCtrl: SDN is already configured.\n");
                VNetFormatString(strRetInfo, "The sdn is already configured.\n");
                return SDN_DATA_ALREADY_EXIST;
            }
        }        

        CDbSdnCtrlCfg cDbSdn;
        cDbSdn.SetProtocolType(cMsg.m_strSdnConnType.c_str());        
        cDbSdn.SetPort(cMsg.m_nSdnConnPort);
        cDbSdn.SetIP(cMsg.m_strSdnConnIP.c_str());
        nRet = pSdn->AddSdnCtrl(cDbSdn);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CVNMCtrl::AddSdnCtrl: Add sdn to db failed. ret = %d.\n", nRet);
            VNetFormatString(strRetInfo, "Add the sdn to the database failed.\n");
            return nRet;
        }

        //查询下是否有交换,如果存在,直接设置controller信息
        vector<CDbSwitchSummary> vecSdnSwitch;
        pVSwitch->SdnQuerySummary(vecSdnSwitch);

        vector<CDbSwitchSummary>::iterator itrVSwitch = vecSdnSwitch.begin();
        for( ; itrVSwitch != vecSdnSwitch.end(); ++itrVSwitch)
        {
            if(0 != itrVSwitch->GetName().size() &&
                0 != cDbSdn.GetProtocolType().size() &&
                0 != cDbSdn.GetIp().size() &&
                0 != cDbSdn.GetPort())
            {
                VNetSetSdnController(itrVSwitch->GetName(), cDbSdn.GetProtocolType(), cDbSdn.GetIp(), cDbSdn.GetPort());
            }
        }

        CDbQuantumRestfulCfg outQuantumInfo;
        nRet = pSdn->QueryQuantum(outQuantumInfo);
        if(VNET_DB_SUCCESS == nRet)
        {
            if(NULL != outQuantumInfo.GetUuid().c_str())
            {                
                VNET_LOG(VNET_LOG_ERROR, "CVNMCtrl::AddQuantumRestful: SDN is already configured.\n");
                VNetFormatString(strRetInfo, "The quantum restful is already configured.\n");
                return SDN_DATA_ALREADY_EXIST;
            }
        }

        CDbQuantumRestfulCfg cDbQuantum;
        cDbQuantum.SetIP(cMsg.m_strQuantumSvrIP.c_str());        
        nRet = pSdn->AddQuantum(cDbQuantum);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CVNMCtrl::AddQuantumRestful: Add Quantum to db failed. ret = %d.\n", nRet);
            VNetFormatString(strRetInfo, "Add the Quantum to the database failed.\n");
            return nRet;
        }
        
        return SDN_OPER_SUCCESS;
    }

    int32 CVNMCtrl::DelSdnCtrl(const CSdnCfgMsg &cMsg, string &strRetInfo)
    {
        CVNetDbMgr * pMgr = CVNetDbMgr::GetInstance();
        if(NULL == pMgr)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNMCtrl::DelSdnCtrl: GetInstance() is NULL.\n");
            strRetInfo = "The database handler is NULL.";
            return SDN_GET_DB_INTF_ERROR;
        }
        
        CDBOperateSdnCfg * pSdn = pMgr->GetISdnCfg();
        if(NULL == pSdn)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNMCtrl::DelSdnCtrl: GetISdn() is NULL.\n");
            strRetInfo = "The sdn database interface is NULL.";
            return SDN_GET_DB_INTF_ERROR;
        }

        CDBOperateVSwitch * pVSwitch = pMgr->GetIVSwitch();
        if(NULL == pVSwitch)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNMCtrl::DelSdnCtrl: GetIVSwitch() is NULL.\n");
            strRetInfo = "The vsitch database interface is NULL.";
            return SDN_GET_DB_INTF_ERROR;
        }

        // find record 
        CDbSdnCtrlCfg outSdnInfo;
        int32 nRet = pSdn->QuerySdnCtrl(outSdnInfo);

        if(VNET_DB_SUCCESS == nRet)
        {
            if(outSdnInfo.GetUuid() == cMsg.m_strSdnUuid)
            {
                nRet = pSdn->DelSdnCtrl(cMsg.m_strSdnUuid.c_str());
                if(VNET_DB_SUCCESS != nRet)
                {
                    VNET_LOG(VNET_LOG_ERROR, "CVNMCtrl::DelSdnCtrl: Del sdn from db failed. ret = %d.\n", 
                        nRet);
                    VNetFormatString(strRetInfo, "Delete the sdn from the database failed.\n");
                    return nRet;
                }
            }
        }

        
        //查询下是否有交换,如果存在,直接设置controller信息
        vector<CDbSwitchSummary> vecSdnSwitch;
        pVSwitch->SdnQuerySummary(vecSdnSwitch);

        vector<CDbSwitchSummary>::iterator itrVSwitch = vecSdnSwitch.begin();
        for( ; itrVSwitch != vecSdnSwitch.end(); ++itrVSwitch)
        {
            if(0 != itrVSwitch->GetName().size())
            {
                VNetDelSdnController(itrVSwitch->GetName());
            }
        }

        // find record 
        CDbQuantumRestfulCfg outQuantumInfo;        
        nRet = pSdn->QueryQuantum(outQuantumInfo);
        if(VNET_DB_SUCCESS == nRet)
        {
            if(outQuantumInfo.GetUuid() == cMsg.m_strQuantumUuid)
            {
                nRet = pSdn->DelQuantum(cMsg.m_strQuantumUuid.c_str());
                if(VNET_DB_SUCCESS != nRet)
                {
                    VNET_LOG(VNET_LOG_ERROR, "CVNMCtrl::DelQuantumRestful: Del sdn from db failed. ret = %d.\n", 
                        nRet);
                    VNetFormatString(strRetInfo, "Delete the quantum from the database failed.\n");
                    return nRet;
                }
            }
        }
        
        return SDN_OPER_SUCCESS;
    }

    int32 CVNMCtrl::ModSdnCtrl(const CSdnCfgMsg &cMsg, string &strRetInfo)
    {
        int32 bSdnUpdate = 0;
        
        CVNetDbMgr * pMgr = CVNetDbMgr::GetInstance();
        if(NULL == pMgr)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNMCtrl::ModSdnCtrl: GetInstance() is NULL.\n");
            strRetInfo = "The database handler is NULL.";
            return SDN_GET_DB_INTF_ERROR;
        }
        
        CDBOperateSdnCfg * pSdn = pMgr->GetISdnCfg();
        if(NULL == pSdn)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNMCtrl::ModSdnCtrl: GetISdn() is NULL.\n");
            strRetInfo = "The sdn database interface is NULL.";
            return SDN_GET_DB_INTF_ERROR;
        }

        CDBOperateVSwitch * pVSwitch = pMgr->GetIVSwitch();
        if(NULL == pVSwitch)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNMCtrl::ModSdnCtrl: GetIVSwitch() is NULL.\n");
            strRetInfo = "The vsitch database interface is NULL.";
            return SDN_GET_DB_INTF_ERROR;
        }

        // find record 
        CDbSdnCtrlCfg cDbSdn;
        int32 nRet = pSdn->QuerySdnCtrl(cDbSdn);
        
        if(VNET_DB_SUCCESS == nRet)
        {
            if(cDbSdn.GetUuid() == cMsg.m_strSdnUuid)
            {
                if(cDbSdn.GetProtocolType() != cMsg.m_strSdnConnType)
                {
                    cDbSdn.SetProtocolType(cMsg.m_strSdnConnType.c_str());
                    bSdnUpdate = 1;
                }
                if(cDbSdn.GetPort() != cMsg.m_nSdnConnPort)
                {
                    cDbSdn.SetPort(cMsg.m_nSdnConnPort);
                    bSdnUpdate = 1;
                }
                if(cDbSdn.GetIp() != cMsg.m_strSdnConnIP)
                {
                    cDbSdn.SetIP(cMsg.m_strSdnConnIP.c_str());
                    bSdnUpdate = 1;
                }                
                if(0 == bSdnUpdate)
                {
                    return SDN_OPER_SUCCESS;
                }
                
                nRet = pSdn->ModifySdnCtrl(cDbSdn);
                if(VNET_DB_SUCCESS != nRet)
                {
                    VNetFormatString(strRetInfo, "modify the sdn to the database failed.\n");
                    return nRet;
                }
            }
        }

        if(1 == bSdnUpdate)
        {
            //查询下是否有交换,如果存在,直接设置controller信息
            vector<CDbSwitchSummary> vecSdnSwitch;
            pVSwitch->SdnQuerySummary(vecSdnSwitch);

            vector<CDbSwitchSummary>::iterator itrVSwitch = vecSdnSwitch.begin();
            for( ; itrVSwitch != vecSdnSwitch.end(); ++itrVSwitch)
            {
                if(0 != itrVSwitch->GetName().size() &&
                    0 != cDbSdn.GetProtocolType().size() &&
                    0 != cDbSdn.GetIp().size() &&
                    0 != cDbSdn.GetPort())
                {
                    VNetSetSdnController(itrVSwitch->GetName(), cDbSdn.GetProtocolType(), cDbSdn.GetIp(), cDbSdn.GetPort());
                }
            }
        }

        // find record 
        CDbQuantumRestfulCfg cDbQuantum;
        nRet = pSdn->QueryQuantum(cDbQuantum);        
        if(VNET_DB_SUCCESS == nRet)
        {
            if(cDbQuantum.GetUuid() == cMsg.m_strSdnUuid)
            {
                int32 bUpdate = 0;
                if(cDbQuantum.GetIp() != cMsg.m_strQuantumSvrIP)
                {
                    cDbQuantum.SetIP(cMsg.m_strQuantumSvrIP.c_str());
                    bUpdate = 1;
                }                
                if(0 == bUpdate)
                {
                    return SDN_OPER_SUCCESS;
                }
                
                nRet = pSdn->ModifyQuantum(cDbQuantum);
                if(VNET_DB_SUCCESS != nRet)
                {
                    VNetFormatString(strRetInfo, "modify the quantum to the database failed.\n");
                    return nRet;
                }
            }
        }
        
        return SDN_OPER_SUCCESS;
    }
#if 0
    int32 CVNMCtrl::AddQuantumRestful(const CSdnCfgMsg &cMsg, string &strRetInfo)
    {
        CVNetDbMgr * pMgr = CVNetDbMgr::GetInstance();
        if(NULL == pMgr)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNMCtrl::AddQuantumRestful: GetInstance() is NULL.\n");
            strRetInfo = "The database handler is NULL.";
            return DB_ERROR_GET_INST_FAILED;
        }
        
        CDBOperateSdnCfg * pSdn = pMgr->GetISdn();
        if(NULL == pSdn)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNMCtrl::AddQuantumRestful: GetISdn() is NULL.\n");
            strRetInfo = "The quantum restful database interface is NULL.";
            return SDN_GET_DB_INTF_ERROR;
        }

        // Conflict checking
        CDbQuantumRestfulCfg outVInfo;
        int32 nRet = pSdn->QueryQuantum(outVInfo);
        if(VNET_DB_SUCCESS == nRet)
        {
            if(NULL != outVInfo.GetUuid().c_str())
            {                
                VNET_LOG(VNET_LOG_ERROR, "CVNMCtrl::AddQuantumRestful: SDN is already configured.\n");
                VNetFormatString(strRetInfo, "The quantum restful is already configured.\n");
                return SDN_DATA_ALREADY_EXIST;
            }
        }

        CDbQuantumRestfulCfg cDbSdn;
        cDbSdn.SetIP(cMsg.m_strQuantumSvrIP.c_str());        
        nRet = pSdn->AddQuantum(cDbSdn);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CVNMCtrl::AddQuantumRestful: Add Quantum to db failed. ret = %d.\n", nRet);
            VNetFormatString(strRetInfo, "Add the Quantum to the database failed.\n");
            return nRet;
        }
        
        return SDN_OPER_SUCCESS;
    }

    int32 CVNMCtrl::DelQuantumRestful(const CSdnCfgMsg &cMsg, string &strRetInfo)
    {
        CVNetDbMgr * pMgr = CVNetDbMgr::GetInstance();
        if(NULL == pMgr)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNMCtrl::DelQuantumRestful: GetInstance() is NULL.\n");
            strRetInfo = "The database handler is NULL.";
            return SDN_GET_DB_INTF_ERROR;
        }
        
        CDBOperateSdnCfg * pSdn = pMgr->GetISdn();
        if(NULL == pSdn)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNMCtrl::DelQuantumRestful: GetISdn() is NULL.\n");
            strRetInfo = "The quantum database interface is NULL.";
            return SDN_GET_DB_INTF_ERROR;
        }

        // find record 
        CDbQuantumRestfulCfg outVInfo;        
        int32 nRet = pSdn->QueryQuantum(outVInfo);
        if(VNET_DB_SUCCESS == nRet)
        {
            if(outVInfo.GetUuid() == cMsg.m_strSdnUuid)
            {
                nRet = pSdn->DelSdnCtrl(cMsg.m_strSdnUuid.c_str());
                if(VNET_DB_SUCCESS != nRet)
                {
                    VNET_LOG(VNET_LOG_ERROR, "CVNMCtrl::DelQuantumRestful: Del sdn from db failed. ret = %d.\n", 
                        nRet);
                    VNetFormatString(strRetInfo, "Delete the quantum from the database failed.\n");
                    return nRet;
                }
            }
        }
        
        return SDN_OPER_SUCCESS;
    }

    int32 CVNMCtrl::ModQuantumRestful(const CSdnCfgMsg &cMsg, string &strRetInfo)
    {
        CVNetDbMgr * pMgr = CVNetDbMgr::GetInstance();
        if(NULL == pMgr)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNMCtrl::ModQuantumRestful: GetInstance() is NULL.\n");
            strRetInfo = "The database handler is NULL.";
            return SDN_GET_DB_INTF_ERROR;
        }
        
        CDBOperateSdnCfg * pSdn = pMgr->GetISdn();
        if(NULL == pSdn)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNMCtrl::ModQuantumRestful: GetISdn() is NULL.\n");
            strRetInfo = "The quantum database interface is NULL.";
            return SDN_GET_DB_INTF_ERROR;
        }

        // find record 
        CDbQuantumRestfulCfg cDbSdn;
        int32 nRet = pSdn->QueryQuantum(cDbSdn);        
        if(VNET_DB_SUCCESS == nRet)
        {
            if(cDbSdn.GetUuid() == cMsg.m_strSdnUuid)
            {
                int32 bUpdate = 0;
                if(cDbSdn.GetIp() != cMsg.m_strQuantumSvrIP)
                {
                    cDbSdn.SetIP(cMsg.m_strQuantumSvrIP.c_str());
                    bUpdate = 1;
                }                
                if(0 == bUpdate)
                {
                    return SDN_OPER_SUCCESS;
                }
                
                nRet = pSdn->ModifyQuantum(cDbSdn);
                if(VNET_DB_SUCCESS != nRet)
                {
                    VNetFormatString(strRetInfo, "modify the quantum to the database failed.\n");
                    return nRet;
                }
            }
        }
        
        return SDN_OPER_SUCCESS;
    }
#endif

    void CVNMCtrl::DbgShowVNMCtrlInfo(void)
    {
        cout << "----------------------------------------------------------------------------" << endl;
        cout <<"VNM application: {" << m_strApp <<"}." << endl;
        cout << "Priority: " << (uint32)m_ucPriority;
        if(VNM_MASTER_STATE == m_ucMasterOrSlave)
        {
            cout << ", State: Master." << endl;
        }
        else
        {
            cout << ", State: Slave." << endl;
        }
        cout << "----------------------------------------------------------------------------" << endl;
        
        return ;
    }

 

    void VNetDbgShowVNMCtrlInfo(void)
    {
        CVNMCtrl *pVNMCtrl = CVNMCtrl::GetInstance();
        if(pVNMCtrl)
        {
            pVNMCtrl->DbgShowVNMCtrlInfo();
        }
        
        return ;
    }
    DEFINE_DEBUG_FUNC(VNetDbgShowVNMCtrlInfo);

    void CVNMCtrl::DbgShowSdnInfo(void)
    {
        cout << "----------------------------------------------------------------------------" << endl;

        CVNetDbMgr * pMgr = CVNetDbMgr::GetInstance();
        if(NULL != pMgr)
        {
            CDBOperateSdnCfg * pSdn = pMgr->GetISdnCfg();
            if(NULL != pSdn)
            {
                CDbSdnCtrlCfg outSdnInfo;
                if(VNET_DB_SUCCESS == pSdn->QuerySdnCtrl(outSdnInfo))
                {
                    outSdnInfo.DbgShow();
                }

                cout << "----------------------------------------------------------------------------" << endl;

                CDbQuantumRestfulCfg outQuantumInfo;
                if(VNET_DB_SUCCESS == pSdn->QueryQuantum(outQuantumInfo))
                {
                    outQuantumInfo.DbgShow();
                }
            }
        }

        cout << "----------------------------------------------------------------------------" << endl;
    }

    void VNetDbgShowSdnInfo(void)
    {
        CVNMCtrl *pVNMCtrl = CVNMCtrl::GetInstance();
        if(pVNMCtrl)
        {
            pVNMCtrl->DbgShowSdnInfo();
        }
        
        return ;
    }
    DEFINE_DEBUG_FUNC(VNetDbgShowSdnInfo);

}// end namespace zte_tecs

