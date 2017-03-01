/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�vna_controler.cpp
* �ļ���ʶ��
* ����ժҪ��CVNAController��ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�
* ������ڣ�2013��1��16��
*
* �޸ļ�¼1��
*     �޸����ڣ�2013/1/16
*     �� �� �ţ�V1.0
*     �� �� �ˣ�lvech
*     �޸����ݣ�����
******************************************************************************/
#include "registered_system.h"
#include "vna_common.h"
#include "dev_monitor.h"
#include "dev_mgr.h"
#include "dev_mon_reg_func.h"
#include "dev_mon_mgr.h"
#include "vna_switch_module.h"
#include "vna_switch_agent.h"
#include "vna_agent.h"
#include "vna_vnic_pool.h"
#include "vna_vm_agent.h"
#include "vnet_phyport_class.h"
#include "vnet_phyport_manager.h"
#include "vnet_trunkport_class.h"
#include "vnet_trunkport_manager.h"
#include "vnetlib_common.h"
#include "vnet_portagent.h"
#include "vna_heartbeat.h"
#include "vnet_monitor.h"
#include "vna_vxlan_agent.h"
#include "vna_controler.h"


namespace zte_tecs
{

TMsgProcReg s_VNetMsgProcReg[] = 
{ 
   /*NICMGR ģ����Ϣ*/
    {VNET_MODULE_VNICMGR, EV_VNETLIB_STARTNETWORK},
    {VNET_MODULE_VNICMGR, EV_VNETLIB_STOPNETWORK},
    {VNET_MODULE_VNICMGR, EV_VNETLIB_NOTIFY_DOMID},
    {VNET_MODULE_VNICMGR, EV_VNM_NOTIFY_VNA_ADD_VMVSI},
    {VNET_MODULE_VNICMGR, EV_VNM_NOTIFY_VNA_DEL_VMVSI},
    {VNET_MODULE_VNICMGR, EV_VNETLIB_NOTIFY_ALL_VMNIC},
    {VNET_MODULE_VNICMGR, EV_VNM_NOTIFY_VNA_ALL_VMVSI},
    {VNET_MODULE_VNICMGR, EV_VNA_CHECK_VM_VSI_INFO},
    {VNET_MODULE_VNICMGR, EV_VNETLIB_WDG_OPER_ACK},
    {VNET_MODULE_VNICMGR, EV_VNETLIB_WDG_TIMER_OPER},
    {VNET_MODULE_VNICMGR, EV_VNETLIB_WATCHDOG_OPER},
    {VNET_MODULE_VNICMGR, EV_VNETLIB_WDG_TIMER_OPER_ACK},
    {VNET_MODULE_VNICMGR, EV_VNETLIB_WATCHDOG_NOTIFY_HC_RESTART_ACK},
    {VNET_MODULE_VNICMGR, EV_VNETLIB_NOTIFY_VM_RR}, 
    {VNET_MODULE_VNICMGR, EV_VNA_REQUEST_VMVSI_ACK},
    
   /*MONITORģ����Ϣ*/
   {VNET_MODULE_MONITOR, TIMER_VNA_DEV_MON},
   {VNET_MODULE_MONITOR, EV_VNET_VNA_REPORT_INFO},

   /*SWITCHAGENTģ����Ϣ*/
    {VNET_MODULE_SWITCH_AGENT, EV_VNETLIB_SWITCH_CFG},
    {VNET_MODULE_SWITCH_AGENT, EV_VNETLIB_SWITCH_PORT_CFG},

    /*PORTAGENTģ����Ϣ*/
    {VNET_MODULE_PORT_AGENT, EV_VNETLIB_KERNALPORT_CFG},
    {VNET_MODULE_PORT_AGENT, EV_VNETLIB_CFG_LOOPBACK},
    {VNET_MODULE_PORT_AGENT, TIMER_VNA_RESET_PORTINFO},

   /*VXLAN ģ����Ϣ*/
    {VNET_MODULE_VXLAN_AGENT, EV_VXLAN_MCGROUP_ACK},
   
};

bool enable_evb = false; 

CVNAController* CVNAController::s_pInstance = NULL;
CPortManager* CPortManager::s_pInstance = NULL;
//CBridgeManager* CBridgeManager::s_pInstance = NULL;
CDevMonMgr* CDevMonMgr::s_pInstance = NULL;
CVNetVmAgent* CVNetVmAgent::s_pInstance = NULL;
CVNAVnicPool* CVNAVnicPool::s_pInstance = NULL;

/************************************************************
* �������ƣ� CVNAController
* ���������� CVNAController���캯��
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
CVNAController::CVNAController()
{
    m_bOpenDbgInf = VNET_FALSE;
    m_nStateMachines = 0;
    m_tReqCfgTimerID = INVALID_TIMER_ID;
    m_pMU = NULL;
    m_pEvbMgr = NULL;
    m_pVNetVmAgent = NULL;
    m_pVNAVnicPool = NULL;
    m_pPortMgr = NULL;
    m_pDevMonMgr = NULL;
    m_pPortAgent = NULL;
    m_pVxlanAgent = NULL;
};

/************************************************************
* �������ƣ� ~CVNAController
* ���������� CVNAController��������
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
CVNAController::~CVNAController()
{
    if (INVALID_TIMER_ID != m_tReqCfgTimerID)
    {
        m_pMU->KillTimer(m_tReqCfgTimerID);
        m_tReqCfgTimerID = INVALID_TIMER_ID;
    }

    if (NULL != m_pMU)
    {
        delete m_pMU;
    }

    if (NULL != s_pInstance)
    {
        delete s_pInstance;
    }

    pthread_mutex_destroy(&m_mutex);
    m_pEvbMgr = NULL;
    m_pVNetVmAgent = NULL;
    m_pVNAVnicPool = NULL;
    m_pPortMgr = NULL;
    m_pDevMonMgr = NULL;
    m_pPortAgent = NULL;
    m_pVxlanAgent = NULL;
    
};

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
STATUS CVNAController::StartMu(const string& name)
{
    if(m_pMU)
    {
        SkyAssert(false);
        return ERROR_DUPLICATED_OP;
    }
    
    // ��Ϣ��Ԫ�Ĵ����ͳ�ʼ��
    m_pMU = new MessageUnit(name);
    if (!m_pMU)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNAController::StartMu: Create mu %s failed!\n",name.c_str());
        return ERROR_NO_MEMORY;
    }

    STATUS ret = m_pMU->SetHandler(this);
    if (SUCCESS != ret)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNAController::StartMu: SetHandler mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }

    ret = m_pMU->Run();
    if (SUCCESS != ret)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNAController::StartMu: Run mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }

    ret = m_pMU->Register();
    if (SUCCESS != ret)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNAController::StartMu: Register mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }

    //���Լ������ϵ���Ϣ����ʹ��Ϣ��Ԫ״̬�л�������̬
    MessageFrame message(SkyInt32(0), EV_VNET_POWER_ON);
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
* �������ƣ� Receive
* ���������� ������Ϣ����
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
STATUS CVNAController::Receive(const MessageFrame& message)
{
    return m_pMU->Receive(message);
};

/************************************************************
* �������ƣ� InitVNetCtl
* ���������� ��ʼ������ҵ��ģ��ĺ���
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
int32 CVNAController::InitVNetCtl()
{
    m_pMU->set_state(S_Startup);
    m_nStateMachines = VNET_STATE_MACHINE_START;
    
    // ��ʼ�����⺯���ӿ���
    if (0 != pthread_mutex_init(&m_mutex, NULL))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNAController::InitVNetCtl: call pthread_mutex_init failed.\n");
        return -1;
    }

    m_pPortMgr = CPortManager::GetInstance();
    if (NULL == m_pPortMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNAController::InitVNetCtl: NULL == m_pPortMgr.\n");
        return -1;
    }
    
    //��ʼ��VNIC��Ϣ��������
    m_pVNAVnicPool =  CVNAVnicPool::GetInstance();
    if (NULL == m_pVNAVnicPool)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNAController::Init: call CVNAVnicPool::GetInstance() failed.\n");
        return -1;
    }

    if (m_pVNAVnicPool->Init(m_pMU))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNAController::Init: call m_pVNAVnicPool->Init() failed.\n");
        return -1;
    }

    //��ʼ��VNIC����ģ��
    m_pVNetVmAgent =  CVNetVmAgent::GetInstance();
    if (NULL == m_pVNetVmAgent)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNAController::Init: call CVNetVmAgent::GetInstance() failed.\n");
        return -1;
    }

    if (m_pVNetVmAgent->Init(m_pMU))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNAController::Init: call m_pVNetVmAgent->Init() failed.\n");
        return -1;
    }

    //��ʼ����ع�������
    m_pDevMonMgr = CDevMonMgr::GetInstance();
    if (NULL == m_pDevMonMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNAController::InitVNetCtl: NULL == m_pDevMonMgr.\n");
        return -1;
    }

    if (SUCCESS != m_pDevMonMgr->Init(m_pMU) )
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNAController::InitVNetCtl: call m_pDevMonMgr->Init()failed.\n");
        return -1;
    }

    // bond Monitor 
    m_pMonMgr = CVNetMonitorMgr::GetInstance();
    if(NULL == m_pMonMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetController::InitVNetCtl: NULL == m_pMonMgr.\n");
        return -1;
    }    
    /*Init Monitor*/
    if(SUCCESS != m_pMonMgr->init() )
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetController::InitVNetCtl: call m_pVmNicMgr->Init()failed.\n");
        return -1;
    }
    
    //��ʼ������ģ��
    m_pSwitchAgent = CSwitchAgent::GetInstance();
    if (NULL == m_pSwitchAgent)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNAController::InitVNetCtl: NULL == m_pSwitchAgent.\n");
        return -1;
    }

    if (SUCCESS != m_pSwitchAgent->Init(m_pMU) )
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNAController::InitVNetCtl: call m_pSwitchAgent->Init()failed.\n");
        return -1;
    }

    //��ʼ���˿�ģ��
    m_pPortAgent = CPortAgent::GetInstance();
    if (NULL == m_pPortAgent)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNAController::InitVNetCtl: NULL == m_pPortAgent.\n");
        return -1;
    }

    if (SUCCESS != m_pPortAgent->Init(m_pMU) )
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNAController::InitVNetCtl: call m_pPortAgent->Init()failed.\n");
        return -1;
    }

     //��ʼ��EVBģ��
    m_pEvbMgr = CEvbMgr::GetInstance();
    if(NULL == m_pEvbMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNAController::InitVNetCtl: NULL == m_pEvbMgr.\n");
        return -1;
    }

    if (m_pEvbMgr->Init())
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNAController::Init: call m_pEvbMgr->Init() failed.\n");
        return -1;
    }

    //��ʼ��VXLANģ��
    m_pVxlanAgent = CVxlanAgent::GetInstance();
    if (NULL == m_pVxlanAgent)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNAController::InitVNetCtl: NULL == m_pVxlanAgent.\n");
        return -1;
    }

    if (SUCCESS != m_pVxlanAgent->Init(m_pMU) )
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNAController::InitVNetCtl: call m_pVxlanAgent->Init()failed.\n");
        return -1;
    }
    

    //�����������
    StartMonitor();

    VNET_LOG(VNET_LOG_INFO, "Init vna controller mod successfully.\n");

    return 0;
}

/************************************************************
* �������ƣ� MessageEntry
* ���������� ��Ϣ��������ں���
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
void CVNAController::MessageEntry(const MessageFrame& message)
{
    TimeOut  tTmOut;
    if (VNET_TRUE == m_bOpenDbgInf)
    {
        cout << "StateMachines: " << m_pMU->get_state() <<" receive msg id: " << message.option.id() << endl;
    }
    
    switch (m_pMU->get_state())
    {
    case S_Startup:
    {
        switch (message.option.id())
        {
        case EV_VNET_POWER_ON:
        {
            if(0 != InitVNetCtl())
            {
                SkyExit(-1,"CVNAController power on InitVNetCtl failed!");
            }

            //����һ��״̬
            m_nStateMachines = VNET_STATE_MACHINE_WORK;
            m_pMU->set_state(S_Work);
            OutPut(SYS_NOTICE, "%s power on!\n",m_pMU->name().c_str());
            VNET_LOG(VNET_LOG_INFO,
                     "CVNAController::MessageEntry: VNetController: entering VNET_STATE_MACHINE_WORK.\n");
        }
        break;
        }// end VNET_STATE_MACHINE_START
        break;
    }

    case S_Work:
    {
        if (0 != pthread_mutex_lock(&m_mutex))
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNAController::MessageEntry: pthread_mutex_lock(0x%x) failed.\n",
                     &m_mutex);
            break;
        }
                
        // ����WORK״̬ ��ģ�鶼�Ѿ��ϵ��ʼ���ɹ�
        int num = sizeof(s_VNetMsgProcReg)/sizeof(TMsgProcReg);
        for (int i =0; i<num; i++)
        {           
            if (s_VNetMsgProcReg[i].msg_id == message.option.id())
            {
                //PORTAGENTģ��
                if (0 == s_VNetMsgProcReg[i].mod_name.compare(VNET_MODULE_PORT_AGENT))
                {
                    m_pPortAgent->MessageEntry(message);
                    break;
                }

                //VNICMGRģ��
                if (0 == s_VNetMsgProcReg[i].mod_name.compare(VNET_MODULE_VNICMGR))
                {
                    m_pVNetVmAgent->MessageEntry(message);
                    break;
                }

                //MONITORģ��
                if (0 == s_VNetMsgProcReg[i].mod_name.compare(VNET_MODULE_MONITOR))
                {
                    m_pDevMonMgr->MessageEntry(message);
                    break;
                }
                                   
                //switchģ��
                if (0 == s_VNetMsgProcReg[i].mod_name.compare(VNET_MODULE_SWITCH_AGENT))
                {
                    m_pSwitchAgent->MessageEntry(message);
                    break;
                }     

                //VXLANģ��
                if (0 == s_VNetMsgProcReg[i].mod_name.compare(VNET_MODULE_VXLAN_AGENT))
                {
                    m_pVxlanAgent->MessageEntry(message);
                    break;
                }                
            }
        }

        if (0 != pthread_mutex_unlock(&m_mutex))
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNAController::MessageEntry: pthread_mutex_unlock(0x%x) failed.\n",
                     &m_mutex);
        }
    }
     // POWERON
    break;

    default:
        break;

    }
}

/************************************************************
* �������ƣ� DoMonitor
* ���������� �����غ���
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
void CVNAController::StartMonitor()
{
    CDevMonMTU *cDevMonMTU = new CDevMonMTU();
    CPortMonLoop *cPortMonLoop = new CPortMonLoop();
    CPortMonBridge *cPortMon = new CPortMonBridge();
    CBridgeMonUplink *cBridgeMonNic = new CBridgeMonUplink();
    CVnicVisitor *cVnicMon = new CVnicVisitor();

    //PhyPortManager *pPhyPortMgr= PhyPortManager::GetInstance();
    //TrunkPortManager *pTrunkPortMgr = TrunkPortManager::GetInstance();
    CPortManager *pPortMgr = CPortManager::GetInstance();
    CSwitchAgent *pBridgeMgr = CSwitchAgent::GetInstance();
    CVNAVnicPool *pVnicMgr = CVNAVnicPool::GetInstance();

    m_pDevMonMgr->AddMonitorTask(pPortMgr, cDevMonMTU);
    m_pDevMonMgr->AddMonitorTask(pPortMgr, cPortMonLoop);
    m_pDevMonMgr->AddMonitorTask(pPortMgr, cPortMon);
    m_pDevMonMgr->AddMonitorTask(pBridgeMgr, cDevMonMTU);
    //m_cDevMonMgr->AddMonitorTask(pBridgeMgr, cPortMonLoop);
    m_pDevMonMgr->AddMonitorTask(pBridgeMgr, cBridgeMonNic);
    m_pDevMonMgr->AddMonitorTask(pVnicMgr, cVnicMon);
};

/************************************************************
* �������ƣ� DbgShowData
* ���������� ������Ϣ��ӡ����
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
int32 CVNAController::DbgShowData(void)
{
    if (VNET_STATE_MACHINE_WORK != m_nStateMachines)
    {
        cout << "DbgShowData: VNET_STATE_MACHINE_WORK != m_nStateMachines." << endl;
        return -1;
    }

    if (0 != pthread_mutex_lock(&m_mutex))
    {
        cout << "pthread_mutex_lock failed." << endl;
        return -1;
    }

    // Show VMNic info
    m_pVNetVmAgent->DbgShowData();

    // Show HC_GetAllPhyPort��iss port
    m_pEvbMgr->DbgShowISSPort();

    // show monitor 
    m_pDevMonMgr->DbgShowData();

    // show monitor 
    m_pMonMgr->DbgShowData();

    if (0 != pthread_mutex_unlock(&m_mutex))
    {
        cout << "pthread_mutex_unlock failed." << endl;
        return -1;
    }

    return 0;
};

/**************************************************************************
* hc ���Ժ���start
**************************************************************************/

/************************************************************
* �������ƣ� VNA_DBG_SET_DBG_FLAG
* ���������� ������Ϣ���غ���
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
void VNA_DBG_SET_DBG_FLAG(const char * cModeName, int bSet)
{
    if ((NULL == cModeName)||(0 == strcmp("",cModeName)))
    {
        cout << "Invalid input parameter, please input such as:" <<endl;
        cout << "   VNA_DBG_SET_DBG_FLAG(module_ame, debug_flag)" <<endl;
        cout << "   *module_ame should be:" <<endl;
        cout << "       vna_controller" <<endl;
        cout << "       vna_agent" <<endl;
        cout << "       vna_heartbeat" <<endl;        
        cout << "       vna_monitor" <<endl;
        cout << "       vna_vm" <<endl;
        cout << "       vna_vnic_pool" <<endl;
        cout << "       vna_switch" <<endl;
        cout << "       vna_port" <<endl;
        cout << "       vna_vxlan" <<endl;        
        cout << "   *debug_flag should be 1(open) or 0(close)!" <<endl;
        return ;
    }
    
    if ((0 != bSet) && (1 != bSet))
    {
        cout << "Invalid flag parameter, please input 0 or 1." <<endl;
        return ;
    }
    
    string strModeName = cModeName;
    
    if ("vna_controller" == strModeName)
    {
        CVNAController *pCVNAController = CVNAController::GetInstance();
        if (!pCVNAController)
        {
            VNET_LOG(VNET_LOG_ERROR, "SetModeDbgFlag::Init: call CVNAController::GetInstance() failed.\n");
            return;
        }
        pCVNAController->SetDbgFlag(bSet);
    }
    else if ( "vna_heartbeat" == strModeName)
    {
        CVNetHeartbeat *pCVNetHeart = CVNetHeartbeat::GetInstance();
        if(!pCVNetHeart)
        {
            VNET_LOG(VNET_LOG_ERROR, "SetModeDbgFlag::Init: call CVNetHeartbeat::GetInstance() failed.\n");
            return;
        }
        pCVNetHeart->SetDbgFlag(bSet);
    }
    else if ( "vna_vm" == strModeName)
    {
        CVNetVmAgent *pVNetVmAgent =  CVNetVmAgent::GetInstance();
        if (NULL == pVNetVmAgent)
        {
            VNET_LOG(VNET_LOG_ERROR, "SetModeDbgFlag::Init: call CVNetVmAgent::GetInstance() failed.\n");
            return;
        }
        pVNetVmAgent->SetDbgFlag(bSet);
    }
    else if ( "vna_vnic_pool" == strModeName)
    {
        CVNAVnicPool *pVNAVnicPool =  CVNAVnicPool::GetInstance();
        if (NULL == pVNAVnicPool)
        {
            VNET_LOG(VNET_LOG_ERROR, "SetModeDbgFlag::Init: call CVNAVnicPool::GetInstance() failed.\n");
            return;
        }
        pVNAVnicPool->SetDbgFlag(bSet);
    }
    else if ("vna_agent" == strModeName)
    {
        CVNetAgent *pVnetAgentInst =  CVNetAgent::GetInstance();
        if (NULL == pVnetAgentInst)
        {
            VNET_LOG(VNET_LOG_ERROR, "SetModeDbgFlag::Init: call CVNetAgent::GetInstance() failed.\n");
            return;
        }
        pVnetAgentInst->SetDbgFlag(bSet);
    }
    else if ("vna_monitor" == strModeName)
    {
        CDevMonMgr *pDevMonMgr = CDevMonMgr::GetInstance();
        if (NULL == pDevMonMgr)
        {
            VNET_LOG(VNET_LOG_ERROR, "SetModeDbgFlag::Init: call CDevMonMgr::GetInstance() failed.\n");
            return;
        }
        pDevMonMgr->SetDbgFlag(bSet);
    } 
    else if ( "vna_switch" == strModeName)
    {
        CSwitchAgent *pSwitchAgent = CSwitchAgent::GetInstance();
        if (NULL == pSwitchAgent)
        {
            VNET_LOG(VNET_LOG_ERROR, "SetModeDbgFlag::Init: call CSwitchAgent::GetInstance() failed.\n");
            return;
        }
        //pSwitchAgent->SetDbgFlag(bSet);
    } 
    else if ( "vna_port" == strModeName)
    {
        CPortManager *pPortMgr = CPortManager::GetInstance();
        if (NULL == pPortMgr)
        {
            VNET_LOG(VNET_LOG_ERROR, "SetModeDbgFlag::Init: call CPortManager::GetInstance() failed.\n");
            return;
        }
        //pPortMgr->SetDbgFlag(bSet);
    }
    else if ( "vna_vxlan" == strModeName)
    {
        CVxlanAgent *pVxlanAgent = CVxlanAgent::GetInstance();
        if (NULL == pVxlanAgent)
        {
            VNET_LOG(VNET_LOG_ERROR, "SetModeDbgFlag::Init: call CVxlanAgents::GetInstance() failed.\n");
            return;
        }
        //pVxlanAgent->SetDbgFlag(bSet);
    }    
    else
    {
        cout << "Invalid input parameter, please input such as:" <<endl;
        cout << "   VNA_DBG_SET_DBG_FLAG(module_ame, debug_flag)" <<endl;
        cout << "   *module_ame should be:" <<endl;
        cout << "       vna_controller" <<endl;
        cout << "       vna_agent" <<endl;
        cout << "       vna_heartbeat" <<endl;        
        cout << "       vna_monitor" <<endl;
        cout << "       vna_vnic" <<endl;
        cout << "       vna_vnic_pool" <<endl;
        cout << "       vna_switch" <<endl;
        cout << "       vna_port" <<endl;
        cout << "       vna_vxlan" <<endl;        
        cout << "   *debug_flag should be 1(open) or 0(close)!" <<endl;
    }
}
DEFINE_DEBUG_FUNC(VNA_DBG_SET_DBG_FLAG);

int VNET_DEBUG_DOMONITOR(void)
{
    CVNAController *pVNetHandle = CVNAController::GetInstance();
    if (pVNetHandle)
    {
        pVNetHandle->StartMonitor();
        cout << endl;
    }

    CDevMonMgr *pMon= CDevMonMgr::GetInstance();
    if (pMon)
    {
        pMon->DoMonitorTask();
        cout << endl;
    }
    return 0;
}
DEFINE_DEBUG_FUNC(VNET_DEBUG_DOMONITOR);

/**************************************************************************
 * hc ���Ժ���end
 **************************************************************************/
}


