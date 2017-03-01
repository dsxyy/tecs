/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�vnet_agent.cpp
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��CVNetAgent��ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�lvech
* ������ڣ�2012��12��15��
*
* �޸ļ�¼1��
*     �޸����ڣ�2012/12/15
*     �� �� �ţ�V1.0
*     �� �� �ˣ�lvech
*     �޸����ݣ�����
*******************************************************************************/
#include "tecs_config.h"
#include "registered_system.h"
#include "vnet_portagent.h"
#include "dev_monitor.h"
#include "dev_mgr.h"
#include "vna_switch_module.h"
#include "vna_switch_agent.h"
#include "vna_common.h"
#include "vnetlib_common.h"
#include "vna_agent.h"
#include <stdlib.h>

namespace zte_tecs
{

#define DEFAULT_HOST_NAME    ""
#define DEFAULT_VNA_APP      ""
#define DEFAULT_VNM_APP      ""

string g_strHostName = DEFAULT_HOST_NAME;
string g_strVNAApplication = DEFAULT_VNA_APP;
string g_strVNMApplication = DEFAULT_VNM_APP;

CVNetAgent *CVNetAgent::m_pInstance = NULL;


/************************************************************
* �������ƣ� CVNetAgent
* ���������� CVNetAgent���캯��
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
CVNetAgent::CVNetAgent()
{
    m_bOpenDbgInf = VNET_FALSE;
    m_nStateMachines = 0;
    m_pMU = NULL;
    m_nTimerID = INVALID_TIMER_ID;
    m_nModuleCheckTimerID = INVALID_TIMER_ID;
}

/************************************************************
* �������ƣ� ~CVNetAgent
* ���������� CVNetAgent��������
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
CVNetAgent::~CVNetAgent()
{
    if (INVALID_TIMER_ID != m_nTimerID)
    {
        m_pMU->KillTimer(m_nTimerID);
        m_nTimerID = INVALID_TIMER_ID;
    }
    
    if (INVALID_TIMER_ID != m_nModuleCheckTimerID)
    {
        m_pMU->KillTimer(m_nModuleCheckTimerID);
        m_nModuleCheckTimerID = INVALID_TIMER_ID;
    }
    
    if (NULL != m_pInstance)
    {
        delete m_pInstance;
    }
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
STATUS CVNetAgent::StartMu(const string& name)
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
        VNET_LOG(VNET_LOG_ERROR, "CVNetAgent::StartMu: Create mu %s failed!\n",name.c_str());
        return ERROR_NO_MEMORY;
    }

    STATUS ret = m_pMU->SetHandler(this);
    if (SUCCESS != ret)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetAgent::StartMu: SetHandler mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }

    ret = m_pMU->Run();
    if (SUCCESS != ret)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetAgent::StartMu: Run mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }
    
    ret = m_pMU->Register();
    if (SUCCESS != ret)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetAgent::StartMu: Register mu %s failed! ret = %d\n",name.c_str(),ret);
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
* �������ƣ� Init
* ���������� ��ʼ������
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2012/5         V1.0       lvech         ����
***************************************************************/
STATUS CVNetAgent::DoStartUp()
{
    // ���������Ϣ�鲥��
    STATUS ret = m_pMU->JoinMcGroup(string(ApplicationName()) + 
                                  MUTIGROUP_VNA_REG_SYSTEM);
    if (SUCCESS != ret)
    {
        SkyAssert(false);
        string lastwords("VNetAgent failed to join group! ret = ");
        lastwords.append(to_string(ret, dec));
        SkyExit(-1,lastwords);
    }

    // ���ù�����Ⱥ��Ϣ�����鲥��ʱ��
    TimeOut timeout;
    timeout.type = LOOP_TIMER;
    timeout.duration = TIMER_INTERVAL_VNA_REGISTERED_VNM;
    timeout.msgid = TIMER_VNA_REGISTERED_VNM;
    if (SUCCESS != m_pMU->SetTimer(m_pMU->CreateTimer(), timeout))
    {
        VNET_LOG(VNET_LOG_ERROR, "Create timer for send registered VNM!\n");
        SkyAssert(0);
    }

    // ����VNA���ֻ���Ϣ�ϱ����õĶ�ʱ��
    m_nTimerID = m_pMU->CreateTimer();
    if (INVALID_TIMER_ID == m_nTimerID)
    {
        VNET_LOG(VNET_LOG_ERROR, 
               "Create timer failed for send vna discover or report!\n");
        SkyAssert(0);
    }
    timeout.type = LOOP_TIMER;
    timeout.duration = TIMER_INTERVAL_VNA_REGISTER;
    timeout.msgid = TIMER_VNA_REGISTER;
    m_pMU->SetTimer(m_nTimerID, timeout);    

    // ���ö�ʱ��: ����ά��HC/CC/DHCP��ҵ����̵�����״̬�Ƿ�����
    m_nModuleCheckTimerID = m_pMU->CreateTimer();
    if (INVALID_TIMER_ID == m_nModuleCheckTimerID)
    {
        VNET_LOG(VNET_LOG_ERROR, "Create timer for vna offline check failed!\n");
        SkyAssert(INVALID_TIMER_ID != m_nModuleCheckTimerID);
    }
    timeout.type = LOOP_TIMER;
    timeout.duration = TIMER_INTERVAL_MODULE_STATE_CHECK;
    timeout.msgid = TIMER_MODULE_STATE_CHECK;
    if (SUCCESS != (ret = m_pMU->SetTimer(m_nModuleCheckTimerID, timeout)))
    {
        VNET_LOG(VNET_LOG_ERROR, "Set timer for module offline check failed!\n");
        SkyAssert(SUCCESS == ret);
    }
    
    // ��VNA����������Ҳ��֪������ȺΪ��
    BroadcastRegedVNM();

    // ����VNA����
    SendRegToVNM();

    return 0;
}

/************************************************************
* �������ƣ� MessageEntry
* ���������� ��Ϣ����ں���
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2012/5         V1.0       lvech         ����
***************************************************************/
void CVNetAgent::MessageEntry(const MessageFrame &message)
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
            if(0 != DoStartUp())
            {
                SkyExit(-1,"CVNetAgent power on DoStartUp failed!");
            }

            //����һ��״̬
            m_nStateMachines = VNET_STATE_MACHINE_WORK;
            m_pMU->set_state(S_Work);
            OutPut(SYS_NOTICE, "%s power on!\n",m_pMU->name().c_str());
            VNET_LOG(VNET_LOG_INFO,
                     "CVNetAgent::MessageEntry: entering VNET_STATE_MACHINE_WORK.\n");
        }
        break;
        }// end VNET_STATE_MACHINE_START
        break;
    }

    case S_Work:
    {
        
        switch (message.option.id())
        {
            //��VNM�����鲥ע����Ϣ
            case TIMER_VNA_REGISTER:
            {
                SendRegToVNM();
                break;
            }

            //VNM�յ��鲥ע����Ϣ����VNAӦ��ע��
            case EV_VNA_REGISTER_REQ:
            {
                RecvRegReqFromVNM(message);
                break;
            }

            //VNMȡ��VNA��ע��
            case EV_VNA_UNREGISTER_REQ:
            {
                RecvUnRegReqFromVNM(message);
                break;
            }

            //VNA��ʱͨ���Լ�ע��VNM��Ϣ
            case TIMER_VNA_REGISTERED_VNM:
            {
                BroadcastRegedVNM();
                break;
            }

            //��ѯVNA��ע���VNM��Ϣ
            case EV_REGISTERED_VNM_QUERY:
            {
                RecvRegedVNMReq(message);
                break;
            }
            
            case EV_REGISTERED_SYSTEM:
            {
                break;
            }  
            
            //��VNM�ϱ���Ϣ
            case TIMER_VNA_INFO_REPORT:
            {
                SendVNAReport();
                break;
            }

            case TIMER_VNA_INFO_REPORT_FIRST:
            {
                SendVNAReportFirst();
                break;
            }

            //CC/HC��ҵ�������VNA���͵ĵǼ���Ϣ
            case EV_VNETLIB_REGISTER_MSG:
            {
                RecvRegFromModule(message);
                break;
            }

            //CC/HC��ҵ�������VNA���͵�ȡ���Ǽ���Ϣ
            case EV_MODULE_UNREGISTER_TO_VNA:
            {
                RecvUnRegFromModule(message);
                break;
            }

            //����CC/HC��ҵ�������VNA���͵�������Ϣ
            case EV_VNETLIB_HEART_BEAT_MSG:
            {
                RecvHeartFromModule(message);
                break;
            }

            //���CC/HC��ҵ����̵�״̬
            case TIMER_MODULE_STATE_CHECK:
            {
                CheckModuleState();
                break;
            }
        
            //��HC��CCע��ɹ�����HC���̵���NETWORK API��NotifyRegisteredInfoToVNA�ӿ�֪ͨVNA��ؽ���
            case EV_VNETLIB_HC_REGISTER_MSG:
            {
                HCRegisteredSystem(message);
                break;
            }

            default:
            {
                VNET_LOG(VNET_LOG_INFO, "Unknown message: %d.\n", message.option.id());
                break;
            }
        }
    }
    // POWERON
    break;

    default:
        break;

    }
}

/************************************************************
* �������ƣ� ExitVNMGroup
* ���������� �˳�VNM�����鲥��
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2012/5         V1.0       lvech         ����
***************************************************************/
void CVNetAgent::ExitVNMGroup()
{
    STATUS ret = 0;
    
    //�ֶ�Ϊ��ʱ����ֹ�ϵ��ʱ�����Ϊ�գ����ʱ����Ҫ���в���
    if (m_strVNMApp.empty())
    {
        return;
    }
    
    ret = m_pMU->ExitMcGroup(m_strVNMApp+MUTIGROUP_SSH_TRUST);
    if (SUCCESS != ret)
    {
        VNET_LOG(VNET_LOG_ERROR, "VNetAgent exit tecs group %s failed!\n",
            (m_strVNMApp+MUTIGROUP_SSH_TRUST).c_str());
    }
    
    VNET_LOG(VNET_LOG_DEBUG, "VNetAgent exit tecs group %s successfully!\n",
        (m_strVNMApp+MUTIGROUP_SSH_TRUST).c_str());
}

/************************************************************
* �������ƣ� JoinVNMGroup
* ���������� ����VNM�����鲥��
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2012/5         V1.0       lvech         ����
***************************************************************/
void CVNetAgent::JoinVNMGroup()
{
    STATUS ret = 0;
    
    //�ֶ�Ϊ��ʱ����ֹ�ϵ��ʱ�����Ϊ�գ����ʱ����Ҫ���в���
    if (m_strVNMApp.empty())
    {
        return;
    }
    
    ret = m_pMU->JoinMcGroup(m_strVNMApp+MUTIGROUP_SSH_TRUST);
    if (SUCCESS != ret)
    {
        VNET_LOG(VNET_LOG_ERROR, "VNetAgent join VNM group %s failed!\n",
               (m_strVNMApp+MUTIGROUP_SSH_TRUST).c_str());
        SkyAssert(SUCCESS == ret);
    }
    
    VNET_LOG(VNET_LOG_INFO, "VNetAgent join tecs group %s successfully!\n",
        (m_strVNMApp+MUTIGROUP_SSH_TRUST).c_str());
}


/************************************************************
* �������ƣ� GoToSendRegToVNM
* ���������� ����VNM�������̣�������ʱ����������Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
***************************************************************/
void CVNetAgent::GoToSendRegToVNM(void)
{
    // ������������ѭ����ʱ��
    //m_pMU->StopTimer(m_nTimerID);
    
    TimeOut timeout;
    timeout.type = LOOP_TIMER;
    timeout.duration = TIMER_INTERVAL_VNA_REGISTER;
    timeout.msgid = TIMER_VNA_REGISTER;
    m_pMU->SetTimer(m_nTimerID, timeout);    

    // ����VNM��������
    SendRegToVNM();
}

/************************************************************
* �������ƣ� SendRegToVNM
* ���������� ��VNM����ע����Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
***************************************************************/
void CVNetAgent::SendRegToVNM()
{
    CVNAReportMsg msg;
    msg.m_strHostName = g_strHostName;
    msg.m_strVNAApp = g_strVNAApplication;

    //����host_manager�ӿڻ�ȡ���ж˿���Ϣ
    //HC_GetPortInitInfo(msg._portinfo);

    // �����������鲥�鷢������������Ϣ
    MID receiver("group", MUTIGROUP_VNA_REG);
    MessageOption option(receiver, EV_VNA_REGISTER_TO_VNM, 0, 0);
    int32 ret = m_pMU->Send(msg, option);
    if (SUCCESS != ret)
    {
        VNET_LOG(VNET_LOG_WARN, "SendRegToVNM: Send discover failed!ret=%d\n", ret);
        SkyAssert(SUCCESS == ret);
    }

    if (m_bOpenDbgInf)
    {
        cout << "Send SendRegToVNM message:" <<endl;  
        msg.Print();
    }
}

/************************************************************
* �������ƣ� GoToSendVNAReport
* ���������� ������VNM�ϱ���Ϣ�����������ϱ���ʱ�����ϱ���Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
***************************************************************/
void CVNetAgent::GoToSendVNAReport(void)
{
    //��Ҫ����ϱ���ʱ���Ƿ��Ѿ��������Ѵ����򷵻�
    //if (INVALID_TIMER_ID != m_nTimerID)
    //{
    //    return ;
    //}

    // ����������Ϣ�ϱ�ѭ����ʱ��
    //if (INVALID_TIMER_ID == (m_nTimerID = m_pMU->CreateTimer()))
    //{
    //    VNET_LOG(VNET_LOG_ERROR, 
    //           "Create timer failed for send VNA information report!\n");
    //    SkyAssert(0);
    //}

    // ������������ѭ����ʱ��
    m_pMU->StopTimer(m_nTimerID);
    
    TimeOut timeout;
    timeout.type = LOOP_TIMER;

    //��һ���ϱ���Ϣ����Ҫ���ƫ��ֵ��������VNM�ϱ��籩
    timeout.duration = TIMER_INTERVAL_VNA_REGISTER + rand() % 60;
    timeout.msgid = TIMER_VNA_INFO_REPORT_FIRST;
    m_pMU->SetTimer(m_nTimerID, timeout);

    // ����������Ϣ�ϱ����̣���DoReport���޶�ʱ������
    SendVNAReport();
}



/************************************************************
* �������ƣ� SendVNAReport
* ���������� ��VNM�ϱ���Ϣ ��һ�� 
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
***************************************************************/
void CVNetAgent::SendVNAReportFirst(void)
{
    // ������������ѭ����ʱ��
    m_pMU->StopTimer(m_nTimerID);
    
    TimeOut timeout;
    timeout.type = LOOP_TIMER;
    timeout.duration = TIMER_INTERVAL_VNA_REGISTER;
    timeout.msgid = TIMER_VNA_INFO_REPORT;
    m_pMU->SetTimer(m_nTimerID, timeout);

    // ����������Ϣ�ϱ����̣���DoReport���޶�ʱ������
    SendVNAReport();
}

/************************************************************
* �������ƣ� SendVNAReport
* ���������� ��VNM�ϱ���Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
***************************************************************/
void CVNetAgent::SendVNAReport()
{
    CVNAReportMsg msg;

    msg.m_strHostName = g_strHostName;
    msg.m_strVNAApp = g_strVNAApplication;

    //��Ҫ�����Լ��Ľ�ɫ��Ϣ
    GetModuleReportInfo(msg.m_vecRegistedModInfo);

    // ������Ϣ
    MID receiver;
    receiver._application = ApplicationName();
    receiver._process = PROC_VNA;
    receiver._unit = MU_VNA_CONTROLLER;
    MessageOption option(receiver, EV_VNET_VNA_REPORT_INFO, 0, 0);
    m_pMU->Send(msg, option);

    if (m_bOpenDbgInf)
    {
        cout << "CVNetAgent::Send CVNAReportMsg message:\n" << endl;  
        msg.Print();
    }
}

/************************************************************
* �������ƣ� RecvRegFromModule
* ���������� ����CC/HC/DHCP��ҵ�������VNA�ĵǼ���Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
***************************************************************/
void CVNetAgent::RecvRegFromModule(const MessageFrame &message)
{
    time_t cur_time = time(0);

    MID sender = message.option.sender();
    
    SkyAssert(!sender._application.empty());

    CVNetModuleRegMsg req;
    req.deserialize(message.data);

    if (m_bOpenDbgInf)
    {
        cout << "Receive CVNetModuleRegMsg message:\n" << endl;  
        req.Print();
    }
    
    //����MODULE��Ϣ
    vector<TModuleInfo>::iterator it;
    for (it = m_vecModuleInfo.begin(); it != m_vecModuleInfo.end(); it++)
    {
        if ((req.m_module_app == it->GetModuleName())
            && (req.m_module_role == it->GetRole()))
        {
            it->SetRole(req.m_module_role);
            it->SetState(VNET_MODULE_ONLINE);
            it->SetLastHeartTime(cur_time);
            it->SetExtInfo(req.m_module_ext_info);
            it->SetModuleLibMid(sender);
            break;
        }
    }

    if (it == m_vecModuleInfo.end())
    {
        TModuleInfo module_node;
        module_node.SetModuleName(req.m_module_app);
        module_node.SetRole(req.m_module_role);
        module_node.SetState(VNET_MODULE_ONLINE);
        module_node.SetLastHeartTime(cur_time);
        module_node.SetExtInfo(req.m_module_ext_info);
        module_node.SetModuleLibMid(sender);

        m_vecModuleInfo.push_back(module_node);
    }
    
    CVNetModuleACKRegMsg cAck;
    cAck.m_module_app = req.m_module_app;

    // �ظ�ע��;
    MessageOption option(message.option.sender(),EV_VNETLIB_REGISTER_MSG_ACK,0,0);
    m_pMU->Send(cAck, option);    
}
/************************************************************
* �������ƣ� RecvUnRegFromModule
* ���������� ����CC/HC/DHCP��ҵ�������VNA��ȡ���Ǽ���Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
***************************************************************/
void CVNetAgent::RecvUnRegFromModule(const MessageFrame &message)
{
    MID sender = message.option.sender();
    
    SkyAssert(!sender._application.empty());

    CModuleUnRegisterToVNAMsg req;
    req.deserialize(message.data);

    if (m_bOpenDbgInf)
    {
        cout << "Receive CModuleUnRegisterToVNAMsg message:\n" << endl;  
        req.Print();
    }
    
    //ɾ��MODULE��Ϣ
    vector<TModuleInfo>::iterator it;
    for (it = m_vecModuleInfo.begin(); it != m_vecModuleInfo.end(); it++)
    {
        if ((req.m_strModuleApp == it->GetModuleName())
            && (req.m_nModuleRole == it->GetRole()))
        {
            m_vecModuleInfo.erase(it);
            break;
        }
    }
}

/************************************************************
* �������ƣ� RecvRegReqFromVNM
* ���������� ����VNM����VNA��ע��Ӧ����Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
***************************************************************/
void CVNetAgent::RecvRegReqFromVNM(const MessageFrame &message)
{
    MID sender = message.option.sender();
    
    VNET_LOG(VNET_LOG_INFO, "CVNetAgent::DoRegister receive msg from VNM.\n");   
    
    SkyAssert(!sender._application.empty());
    SkyAssert(sender._process == PROC_VNM);
    SkyAssert(sender._unit == MU_VNM);

    CVNARegModReqMsg req;
    req.deserialize(message.data);

    if (m_bOpenDbgInf)
    {
        cout << "Receive CVNARegModReqMsg message:\n" << endl;  
        req.Print();
    }
    
    // ����Ϣ���ݽ��кϷ��Լ��
    if (req.m_strVNAName != string(ApplicationName()))
    {
        VNET_LOG(VNET_LOG_WARN, "DoRegister: vna name is err, %s != %s!\n",
               req.m_strVNAName.c_str(), ApplicationName().c_str());
        SkyAssert(req.m_strVNAName == string(ApplicationName()));
        return;
    }

    // ���û�й�����������Ϊ���ͷ�
    if (m_strVNMApp.empty())
    {
        SetVNM(sender._application);
        GoToSendVNAReport();
    }
}

/************************************************************
* �������ƣ� RecvUnRegReqFromVNM
* ���������� ����VNM����VNA��ȡ��ע��Ӧ����Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
***************************************************************/
void CVNetAgent::RecvUnRegReqFromVNM(const MessageFrame &message)
{
    MID sender = message.option.sender();

    SkyAssert(!sender._application.empty());
    SkyAssert(sender._process == PROC_VNM);
    SkyAssert(sender._unit == MU_VNM);

    CVNAUnRegModReqMsg req;
    req.deserialize(message.data);

    if (m_bOpenDbgInf)
    {
        cout << "Receive CVNAUnRegModReqMsg message:\n" << endl;  
        req.Print();
    }
    
    // ����Ϣ���ݽ��кϷ��Լ��
    if (req.m_strVNAName != string(ApplicationName()))
    {
        VNET_LOG(VNET_LOG_WARN, "DoUnregister: vna name is err, %s != %s!\n",
               req.m_strVNAName.c_str(), ApplicationName().c_str());
        SkyAssert(req.m_strVNAName == string(ApplicationName()));
        return;
    }

    // ������ǹ����ļ�Ⱥ����������Ϣ������˵�
    if (m_strVNMApp != sender._application)
    {
        return;
    }

    SetVNM("");
    
    // HC�����˳������·����Ը���ϵͳ�Ĺ�����Ϣ
    SkyExit(-1, "VNA is unregistered from the vnm, exit!\n");
}

/************************************************************
* �������ƣ� SetVNM
* ���������� ����VNA����VNM��Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
***************************************************************/
void CVNetAgent::SetVNM(const string &vnm_name)
{
    if (m_strVNMApp != vnm_name)
    {
        //�������ļ�Ⱥ�仯ʱ����Ҫ�˳�ԭ���ļ�Ⱥ�������µļ�Ⱥ
        ExitVNMGroup();
        m_strVNMApp = vnm_name;
        JoinVNMGroup();
        VNET_LOG(VNET_LOG_INFO, "Set regitered VNM %s.\n", vnm_name.c_str());
    }

    g_strVNMApplication = vnm_name;

    BroadcastRegedVNM();
}

/************************************************************
* �������ƣ� RecvHeartFromModule
* ���������� ����MODULEģ����VNA����������Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
***************************************************************/
void CVNetAgent::RecvHeartFromModule(const MessageFrame &message)
{
    time_t cur_time = time(0);
    int32 iRet = 0;

    CVNetVNAHeartBeatMsg msg;
    msg.deserialize(message.data);

    if (m_bOpenDbgInf)
    {
        cout << "Receive CVNetVNAHeartBeatMsg message:\n" << endl;  
        msg.print();
    }
    
    // ����MODULE����״̬
    vector<TModuleInfo>::iterator it;
    for (it = m_vecModuleInfo.begin(); it != m_vecModuleInfo.end(); it++)
    {
        if ((msg.m_module_app == it->GetModuleName())
            && (msg.m_module_role == it->GetRole()))
        {
           it->SetLastHeartTime(cur_time); 
           it->SetState(VNET_MODULE_ONLINE); 
           break;
        }
    }

    //�Ҳ�����Ӧmoduleģ��
    if (it == m_vecModuleInfo.end())
    {
        iRet = 1;
    }
    
    CVNetVNAHeartBeatAckMsg cAck;
    cAck.m_ret_code = iRet;
    cAck.m_module_app = msg.m_module_app;

    // �ظ�ע��;
    MessageOption option(message.option.sender(), EV_VNETLIB_HEART_BEAT_MSG_ACK,0,0);
    m_pMU->Send(cAck, option);

    if (m_bOpenDbgInf)
    {
        cout << "Send CVNetVNAHeartBeatMsg ack:\n" << endl;  
        msg.print();
    }
}

/************************************************************
* �������ƣ� CheckModuleState
* ���������� ���MODULEģ�������״̬
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
***************************************************************/
void CVNetAgent::CheckModuleState()
{
    time_t cur_time = time(0);

    // ��������ʱ��MODULE��Ϊ����״̬
    vector<TModuleInfo>::iterator it;
    for (it = m_vecModuleInfo.begin(); it != m_vecModuleInfo.end(); it++)
    {
        if (cur_time - it->GetLastHeartTime() > VNET_MODULE_MAX_OFFLINE_TIME)
        {
            it->SetState(VNET_MODULE_OFFLINE); 
        }
    }
}

/************************************************************
* �������ƣ� BroadcastRegedVNM
* ���������� ͨ�汾VNA������VNM��Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
***************************************************************/
void CVNetAgent::BroadcastRegedVNM()
{
    MessageRegisteredSystem msg(m_strVNMApp);
    MID receiver("group", string(ApplicationName()) + MUTIGROUP_SUF_REG_SYSTEM);
    MessageOption option(receiver, EV_REGISTERED_SYSTEM, 0, 0);
    m_pMU->Send(msg, option);
}

/************************************************************
* �������ƣ� RecvRegedVNMReq
* ���������� ��������VNA������VNM��ѯ��Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
***************************************************************/
void CVNetAgent::RecvRegedVNMReq(const MessageFrame &message)
{
    if (m_bOpenDbgInf)
    {
        cout << "Receive RecvRegedVNMReq message:\n" << endl;  
    }
    
    MessageRegisteredSystem msg(m_strVNMApp);
    MessageOption option(message.option.sender(), EV_REGISTERED_SYSTEM, 0, 0);
    m_pMU->Send(msg, option);
}

/************************************************************
* �������ƣ� HCRegisteredSystem
* ���������� HC֪ͨע��ɹ�����
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
***************************************************************/
int CVNetAgent::HCRegisteredSystem(const MessageFrame &message)
{
    time_t cur_time = time(0);

    MID sender = message.option.sender();
    
    SkyAssert(!sender._application.empty());
    
    CvnetVNAClusterMsg msg;
    msg.deserialize(message.data);

    if (m_bOpenDbgInf)
    {
        cout << "Receive HCRegisteredSystem message:\n" << endl;  
        msg.print();
    }
    
    //����MODULE��Ϣ
    vector<TModuleInfo>::iterator it;
    for (it = m_vecModuleInfo.begin(); it != m_vecModuleInfo.end(); it++)
    {
        if (msg.m_module_app == it->GetModuleName())
        {
            it->SetExtInfo(msg.m_cluster_app);
            it->SetState(VNET_MODULE_ONLINE);
            it->SetLastHeartTime(cur_time);
            break;
        }
    }

    return 0;
}

string _hc_app = "";
string _cc_app = "";

/************************************************************
* �������ƣ� GetModuleReportInfo
* ���������� ��ȡmodule�ϱ���Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
***************************************************************/
int CVNetAgent::GetModuleReportInfo(vector<CVNARegModMsg> &vecModInfo)
{
    vecModInfo.clear();
    
    //���MODULE��Ϣ
    vector<TModuleInfo>::iterator it;
    for (it = m_vecModuleInfo.begin(); it != m_vecModuleInfo.end(); it++)
    {
        CVNARegModMsg module;
        module.m_ucModRole = it->GetRole();     
        module.m_ucModState = it->GetState(); 
        module.m_strModApp = it->GetModuleName(); 
        module.m_strExtInfo = it->GetExtInfoe(); 

        vecModInfo.push_back(module);
    }

    //���Դ���
    if ("" != _hc_app)
    {
        CVNARegModMsg module;
        module.m_ucModRole = VNA_MODULE_ROLE_HC;     
        module.m_ucModState = 1; 
        module.m_strModApp = _hc_app; 
        module.m_strExtInfo = _cc_app; 

        vecModInfo.push_back(module);
    }

    return 0;
};

int32 CVNetAgent::GetModuleInfo(int32 m_nRole, TModuleInfo &cModule)
{
    //���MODULE��Ϣ
    vector<TModuleInfo>::iterator it;
    for (it = m_vecModuleInfo.begin(); it != m_vecModuleInfo.end(); it++)
    {
        if (m_nRole == it->GetRole())
        {
            break;
        }
    }

    if (it != m_vecModuleInfo.end())
    {
        if (VNET_MODULE_ONLINE != it->GetState())
        {
            return ERROR_VNA_AGENT_MODULE_OFFLINE;
        }
        else
        {
            cModule = *it;
            return 0;
        }
    }
    else
    {
        return ERROR_VNA_AGENT_MODULE_NOT_REG;
    }
};

void CVNetAgent::DbgShowData(void)
{
    string strVNAApp;
    
    if (GetVNAUUIDByVNA(strVNAApp))
    {
        return;
    }

    //��ӡVNA�����Ϣ
    cout <<endl;    
    cout << "++++++++++++++++++++++VNA Agent Information BEGIN ++++++++++++++++++++++++" << endl;
    cout << "VNA Agent Information:" << endl;
    cout << "   VNA Application:" << strVNAApp << endl;
    cout << "   Registered VNM App:" << m_strVNMApp << endl;
    cout << endl;

    //��ӡMODULE�����Ϣ
    cout << "Module Information:" << endl;
    vector<TModuleInfo>::iterator itModule = m_vecModuleInfo.begin();
    for (; itModule != m_vecModuleInfo.end(); itModule++)
    {
        itModule->Print();
    }
    cout << endl;

    cout << "++++++++++++++++++++++VNA Agent Information END ++++++++++++++++++++++++" << endl;
    cout << endl;
    return;
};

/************************************************************
* �������ƣ� SetDbgFlag
* ���������� ������Ϣ����
* ���ʵı� ��
* �޸ĵı� ��
* ��������� 
* ��������� 
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
***************************************************************/
void CVNetAgent::SetDbgFlag(int bDbgFlag)
{
    m_bOpenDbgInf = bDbgFlag;
};

//�����������ã�ֹͣ����������ʱ��
//nSwitchFlag 1:������0 :�ر�
void CVNetAgent::SetReportSwitch(int32 nSwitchFlag)
{
    if (0 == nSwitchFlag)
    {
        m_pMU->StopTimer(m_nTimerID);
    }
    else
    {
        // ���ù�����Ⱥ��Ϣ�����鲥��ʱ��
        TimeOut timeout;
        timeout.type = LOOP_TIMER;
        timeout.duration = TIMER_INTERVAL_VNA_REGISTER;
        timeout.msgid = TIMER_VNA_INFO_REPORT;
        if (SUCCESS != m_pMU->SetTimer(m_nTimerID, timeout))
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetHeartbeat::MessageEntry Create timer for send registered vnm failed!\n");
            SkyAssert(0);
            SkyExit(-1, "CVNetHeartbeat::DoStartUp: call _mu->SetTimer failed.");
        }
    }  
}

int VNET_DBG_SET_VNA_REPORT_SWITCH(int32 nSwitchFlag)
{
    if ((0 != nSwitchFlag) && (1 != nSwitchFlag))
    {
        cout << "Invalid input parameter, only 0 and 1 is valid!" << endl;
        return -1;
    }
        
    CVNetAgent *pVNetHandle = CVNetAgent::GetInstance();
    if (pVNetHandle)
    {
        pVNetHandle->SetReportSwitch(nSwitchFlag);
    }
    
    return 0;
}
DEFINE_DEBUG_FUNC(VNET_DBG_SET_VNA_REPORT_SWITCH);

int VNET_DBG_SHOW_VNA_AGENT_INFO(void)
{
    CVNetAgent *pVNetHandle = CVNetAgent::GetInstance();
    if (pVNetHandle)
    {
        pVNetHandle->DbgShowData();
        cout << endl;
    }
    
    return 0;
}
DEFINE_DEBUG_FUNC(VNET_DBG_SHOW_VNA_AGENT_INFO);

void VNET_DBG_SET_VNA_CLUSTER_INFO(char * strHC, char *  strCC)
{
     _hc_app = strHC;
     _cc_app = strCC;
}
DEFINE_DEBUG_FUNC(VNET_DBG_SET_VNA_CLUSTER_INFO);


void VNetAgentDbgPrint(int32 i)
{
    CVNetAgent* pInstance =  CVNetAgent::GetInstance();
    if( NULL == pInstance )
    {
        cout << "CVNetAgent::GetInstance() is NULL" << endl;
        return;
    }
    pInstance->SetDbgFlag(i);
}
DEFINE_DEBUG_FUNC(VNetAgentDbgPrint);

} // namespace zte_tecs

