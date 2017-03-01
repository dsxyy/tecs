/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�vnm_schedule.cpp
* �ļ���ʶ��
* ����ժҪ��CVNAReportMgr��ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�
* ������ڣ�2013��3��16��
*
* �޸ļ�¼1��
*     �޸����ڣ�2013/3/16
*     �� �� �ţ�V1.0
*     �� �� �ˣ�lvech
*     �޸����ݣ�����
******************************************************************************/
#include "vnet_error.h"
#include "db_config.h"
#include "vnm_pub.h"
#include "ipv4_addr.h"
#include "ipv4_addr_range.h"
#include "logic_network.h"
#include "logic_network_mgr.h"
#include "vnet_db_schedule.h"
#include "vnm_schedule.h"
#include "vna_report_mgr.h"
#include "vnet_msg.h"
namespace zte_tecs
{

#if 1
int g_vnareport_dbg_flag = 0;
int g_vnareport_msg_during_flag = 0;
CVNAReportMgr *CVNAReportMgr::s_pInstance = NULL;

CVNAReportMgr::CVNAReportMgr()
{
    m_pMU = NULL;
    m_bOpenDbgInf = FALSE;
    m_pVNetVmDB = NULL;
    m_pVNetVnicDB = NULL;
    m_pVSIProfileMgr = NULL;
    m_pSwitchMgr = NULL;
    m_pVNAMgr = NULL;
    m_pLogicNetworkMgr = NULL;
};

CVNAReportMgr::~CVNAReportMgr()
{
    if (NULL != m_pMU)
    {
        delete m_pMU;
    }
    m_pMU = NULL;
    FreeAdoDB();
    
    m_bOpenDbgInf = 0;
    m_pVNetVmDB = NULL;
    m_pVNetVnicDB = NULL;
    m_pVSIProfileMgr = NULL;
    m_pSwitchMgr = NULL;
    m_pVNAMgr = NULL;
    m_pLogicNetworkMgr = NULL;    
};

/************************************************************
* �������ƣ� Init
* ���������� CVNetHeartbeat�����ʼ������
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1        V1.0       lvech         ����
***************************************************************/
int32 CVNAReportMgr::Init()
{
    m_pVNetVmDB = CVNetVmDB::GetInstance();
    if( NULL == m_pVNetVmDB)
    {
        cout << "CVNAReportMgr::Init CVNetVmDB::GetInstance() is NULL" << endl;
        return -1;
    }
    
    m_pVNetVnicDB = CVNetVnicDB::GetInstance();
    if( NULL == m_pVNetVnicDB)
    {
        cout << "CVNAReportMgr::Init CVNetVnicDB::GetInstance() is NULL" << endl;
        return -1;
    }

    //vsi
    m_pVSIProfileMgr = CVSIProfileMgr::GetInstance();
    if (NULL == m_pVSIProfileMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNAReportMgr::Init CVSIProfileMgr::GetInstance().\n");
        return -1;
    }
        
    m_pSwitchMgr = CSwitchMgr::GetInstance();
    if( NULL == m_pSwitchMgr)
    {
        cout << "CVNAReportMgr::Init  CSwitchMgr::GetInstance() is NULL" << endl;
        return -1;
    }

    m_pVNAMgr = CVNAManager::GetInstance();
    if( NULL == m_pVNAMgr)
    {
        cout << "CVNAReportMgr::Init  CVNAManager::GetInstance() is NULL" << endl;
        return -1;
    }

    m_pLogicNetworkMgr = CLogicNetworkMgr::GetInstance();
    if( NULL == m_pLogicNetworkMgr)
    {
        cout << "CVNAReportMgr::Init  CLogicNetworkMgr::GetInstance() is NULL" << endl;
        return -1;
    }

    m_pPortMgr = CPortMgr::GetInstance();
    if( NULL == m_pPortMgr)
    {
        cout << "CVNAReportMgr::Init CPortMgr::GetInstance() is NULL" << endl;
        return -1;
    }
    
    if (StartMu(MU_VNM_VNA_REPORT))
    {
        cout << "CVNAReportMgr::StartMu failed!" << endl;
        return -1;
    }

    return 0;
}        
/************************************************************
* �������ƣ� StartMu
* ���������� ����mu�ĺ���
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1        V1.0       lvech         ����
***************************************************************/
STATUS CVNAReportMgr::StartMu(string strMsgUnitName)
{
    if (m_pMU)
    {
        return ERROR_DUPLICATED_OP;
    }

    // ��Ϣ��Ԫ�Ĵ����ͳ�ʼ��
    m_pMU = new MessageUnit(strMsgUnitName);
    if (!m_pMU)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNAReportMgr::StartMu Create mu %s failed!\n", strMsgUnitName.c_str());
        return ERROR_NO_MEMORY;
    }

    STATUS ret = m_pMU->SetHandler(this);
    if (SUCCESS != ret)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNAReportMgr::StartMu SetHandler mu %s failed! ret = %d\n", strMsgUnitName.c_str(),ret);
        return ret;
    }

    ret = m_pMU->Run();
    if (SUCCESS != ret)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNAReportMgr::StartMu Run mu %s failed! ret = %d\n", strMsgUnitName.c_str(),ret);
        return ret;
    }

    ret = m_pMU->Register();
    if (SUCCESS != ret)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNAReportMgr::StartMu Register mu %s failed! ret = %d\n", strMsgUnitName.c_str(),ret);
        return ret;
    }
    
    //���Լ������ϵ���Ϣ����ʹ��Ϣ��Ԫ״̬�л�������̬
    MessageFrame message(SkyInt32(0), EV_POWER_ON);
    ret = m_pMU->Receive(message);
    if (SUCCESS != ret)
    {
        SkyAssert(0);
        return ret;
    }
    m_pMU->Print();
    
    return SUCCESS;
}

/************************************************************
* �������ƣ� MessageEntry
* ���������� ��Ϣ���ַ�����
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1        V1.0       lvech         ����
***************************************************************/
void CVNAReportMgr::MessageEntry(const MessageFrame &message)
{
    if( g_vnareport_dbg_flag )
    {
        cout << "StateMachines: " << m_pMU->get_state() <<" receive msg id: " << message.option.id() << endl;
    }
    
    switch (m_pMU->get_state())
    {
    case S_Startup:
    {
        switch (message.option.id())
        {
        case EV_POWER_ON:
        {
            m_pMU->set_state(S_Work);
            if (NULL == GetAdoDB())
            {
                cout << "CVNAReportMgr::GetAdoDB failed!" << endl;
            }

            cout << "CVNAReportMgr:GetAdoDB()==================="<< pthread_self() <<endl;


            VNET_LOG(VNET_LOG_INFO, "CVNAReportMgr::MessageEntry %s power on!\n", m_pMU->name().c_str());
            break;
        }
        default:
            VNET_LOG(VNET_LOG_INFO, "CVNAReportMgr::MessageEntry Ignore any message except power on!\n");
            break;
        }
        break;
    }

    case S_Work:
    {
        switch (message.option.id())
        {
        case EV_VNA_INFO_REPORT:
            {
                ProcVNAReport(message);
            }
        break;
        case EV_VNM_NEWVNA_NOTIFY:
        {
            // ����Ϣ��Ȼ��EV_VNA_INFO_REPORT֮�� 
            ProcNewVnaNotify(message);
        }
        break;
        case EV_VNM_CFGINFO_TO_VNA:
        {
            ProcUpdateVNA(message);
        }
        break;
        default:
        {
            cout << "CVNAReportMgr::MessageEntry: receive an unkown message!"
            << endl;
            VNET_LOG(VNET_LOG_INFO, "CVNAReportMgr::MessageEntry Unknown message: %d.\n", message.option.id());
            break;
        }
        }
        break;
    }

    default:
        break;
    }
}


void CVNAReportMgr::ProcNewVnaNotify(const MessageFrame &message)
{
    CNewVnaNotify cMsg;
    cMsg.deserialize(message.data);

    if(cMsg.m_strNewVnaUuid.empty())
    {
        VNET_LOG(VNET_LOG_WARN, "CVNAReportMgr::ProcNewVnaNotify: VNA uuid is null.\n");
        cMsg.Print();
        return ;
    }
       
    // ֪ͨwildcast 
    CWildcastMgr *pWildcastMgrInst = CWildcastMgr::GetInstance();
    if(pWildcastMgrInst)
    {
        pWildcastMgrInst->NewVnaRegist(cMsg.m_strNewVnaUuid);
    }
    else
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNAManager::ProcVNAReport: call CWildcastMgr::GetInstance() failed.\n");
    } 
    return;
}

void CVNAReportMgr::ProcVNAReport(const MessageFrame &message)
{
    CVNAReportMsg cMsg;
    cMsg.deserialize(message.data);

    if(cMsg.m_strVNAApp.empty())
    {
        VNET_LOG(VNET_LOG_WARN, "CVNAReportMgr::ProcVNAReport: VNA application is null.\n");
        cMsg.Print();
        return ;
    }
       
    // �ַ���Ϣ����ģ��
    DispatchMsg(&cMsg);
        
    if(g_vnareport_dbg_flag)
    {
        VNET_LOG(VNET_LOG_INFO, "CVNAReportMgr::ProcVNAReport: Process VNA report msg successfully.\n");
        cMsg.Print();
    }       
    
    return;
}

void CVNAReportMgr::ProcUpdateVNA(const MessageFrame &message)
{    
    CUpdateCfgToVNA cMsg;
    cMsg.deserialize(message.data);

    if(cMsg.m_strVnaUuid.empty())
    {
        VNET_LOG(VNET_LOG_WARN, "CVNAReportMgr::ProcUpdateVNA: VNA application is null.\n");
        cMsg.Print();
        return ;
    }
       
    //�·�DVS��Kernel���õ�VNA
    m_pSwitchMgr->UpdateVNASwitch(cMsg.m_strVnaUuid);
    m_pPortMgr->SetPortCfgToVnaTimer(cMsg.m_strVnaUuid);
   
    return;
}

int32 CVNAReportMgr::DispatchMsg(CVNAReportMsg *pMsg)
{
    if(NULL == pMsg)
    {
        VNET_ASSERT(0);
        return -1;
    }

    if( g_vnareport_msg_during_flag || g_vnareport_dbg_flag )
    {
        cout << "vna uuid : " << pMsg->m_strVNAApp  << endl;
    }
    
    //�ַ���Ϣ
    CPortMgr *pPortMgr = CPortMgr::GetInstance();
    if(pPortMgr)
    {
#if 0    
        Datetime _trace_begin_time;
        Datetime _trace_end_time;
        Timespan _trace_time_span;
    
        Datetime btime(time(0));
        _trace_begin_time = btime;
        _trace_time_span.Begin();  
#endif 
        VNET_TIME_DEFINE("port");
        if( g_vnareport_msg_during_flag )
        {
            VNET_TIME_BEGIN();
        }
        pPortMgr->ProcVnaPortInfo(pMsg->m_cPortInfo);
        
        if( g_vnareport_msg_during_flag )
        {
            VNET_TIME_END();
        }
#if 0
        Datetime etime(time(0));
        _trace_end_time = etime;
        _trace_time_span.End();

        cout << "port duration(us) : " << _trace_time_span.GetSpan() << 
           "(" << _trace_begin_time.tostr() << " - " <<  _trace_end_time.tostr() <<")" << endl;
#endif         
    }

    CSwitchMgr *pSwitchMgr = CSwitchMgr::GetInstance();
    if(pSwitchMgr)
    {
        VNET_TIME_DEFINE("switch");
        if( g_vnareport_msg_during_flag )
        {
            VNET_TIME_BEGIN();
        }
        
        pSwitchMgr->UpdateVNMSwitch(pMsg->m_strVNAApp, pMsg->m_vecSwitchInfo);
        
        if( g_vnareport_msg_during_flag )
        {
            VNET_TIME_END();
        }
    } 
    
    return 0;
}
    
void VNM_VNAREPORT_DBG_FLAG(int bSet)
{    
    if ((0 != bSet) && (1 != bSet))
    {
        cout << "Invalid flag parameter, please input 0 or 1." <<endl;
        return ;
    }

   g_vnareport_dbg_flag = bSet;
};
DEFINE_DEBUG_FUNC(VNM_VNAREPORT_DBG_FLAG);

void VNM_VNAREPORT_MSG_DURING_FLAG(int bSet)
{    
    if ((0 != bSet) && (1 != bSet))
    {
        cout << "Invalid flag parameter, please input 0 or 1." <<endl;
        return ;
    }

   g_vnareport_msg_during_flag = bSet;
};
DEFINE_DEBUG_FUNC(VNM_VNAREPORT_MSG_DURING_FLAG);

#endif 
}

