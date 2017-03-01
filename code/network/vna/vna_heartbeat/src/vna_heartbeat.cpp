/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�vna_heartbeat.cpp
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��CVNetHeartbeat��ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�lvech
* ������ڣ�2013��1��9��
*
* �޸ļ�¼1��
*     �޸����ڣ�2013/1/9
*     �� �� �ţ�V1.0
*     �� �� �ˣ�lvech
*     �޸����ݣ�����
*******************************************************************************/
#include "tecs_config.h"
#include "registered_system.h"
#include "vna_common.h"
#include "vna_heartbeat.h"

namespace zte_tecs
{

CVNetHeartbeat *CVNetHeartbeat::_instance = NULL;

/************************************************************
* �������ƣ� CVNetHeartbeat
* ���������� CVNetHeartbeat���캯��
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
CVNetHeartbeat::CVNetHeartbeat():
        m_strVNMName()
{
    m_bOpenDbgInf = VNET_FALSE;
    m_pMU = NULL;
    m_nHeartTimer = INVALID_TIMER_ID;
}

/************************************************************
* �������ƣ� ~CVNetHeartbeat
* ���������� CVNetHeartbeat��������
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
CVNetHeartbeat::~CVNetHeartbeat()
{
    if (NULL != m_pMU)
    {
        delete m_pMU;
    }

    m_nHeartTimer = INVALID_TIMER_ID;
}

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
STATUS CVNetHeartbeat::Init()
{
        return StartMu(MU_VNA_HEARTBEAT);
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
STATUS CVNetHeartbeat::StartMu(const string& name)
{
    if (m_pMU)
    {
        SkyAssert(false);
        return ERROR_DUPLICATED_OP;
    }
    
    // ��Ϣ��Ԫ�Ĵ����ͳ�ʼ��
    m_pMU = new MessageUnit(name);
    if (!m_pMU)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetHeartbeat::StartMu Create mu %s failed!\n", name.c_str());
        return ERROR_NO_MEMORY;
    }

    STATUS ret = m_pMU->SetHandler(this);
    if (SUCCESS != ret)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetHeartbeat::StartMu SetHandler mu %s failed! ret = %d\n", name.c_str(),ret);
        return ret;
    }

    ret = m_pMU->Run();
    if (SUCCESS != ret)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetHeartbeat::StartMu Run mu %s failed! ret = %d\n", name.c_str(),ret);
        return ret;
    }

    ret = m_pMU->Register();
    if (SUCCESS != ret)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetHeartbeat::StartMu Register mu %s failed! ret = %d\n", name.c_str(),ret);
        return ret;
    }

    m_nHeartTimer = m_pMU->CreateTimer();
    if (INVALID_TIMER_ID == m_nHeartTimer)
    {
        return ERROR_NO_TIMER;
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
void CVNetHeartbeat::MessageEntry(const MessageFrame &message)
{
    switch (m_pMU->get_state())
    {
    case S_Startup:
    {
        switch (message.option.id())
        {
        case EV_POWER_ON:
        {
            DoStartUp();
            m_pMU->set_state(S_Work);
            VNET_LOG(VNET_LOG_INFO, "CVNetHeartbeat::MessageEntry %s power on!\n", m_pMU->name().c_str());
            break;
        }
        default:
            VNET_LOG(VNET_LOG_INFO, "CVNetHeartbeat::MessageEntry Ignore any message except power on!\n");
            break;
        }
        break;
    }

    case S_Work:
    {
        switch (message.option.id())
        {
        case TIMER_VNA_HEARTBEAT_REPORT:
        {
            DoHeartbeatReport();
            break;
        }
        
        case EV_REGISTERED_SYSTEM:
        {
            DoRegisterSystem(message);
            break;
        }
                
        default:
        {
            cout << "CVNetHeartbeat::MessageEntry: receive an unkown message!"
            << endl;
            VNET_LOG(VNET_LOG_INFO, "CVNetHeartbeat::MessageEntry Unknown message: %d.\n", message.option.id());
            break;
        }
        }
        break;
    }

    default:
        break;
    }
}

/************************************************************
* �������ƣ� DoStartUp
* ���������� ��������
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
void CVNetHeartbeat::DoStartUp()
{
    // ���ù�����Ⱥ��Ϣ�����鲥��ʱ��
    TimeOut timeout;
    timeout.type = LOOP_TIMER;
    timeout.duration = TIMER_INTERVAL_VNA_HEARTBEAT_REPORT;
    timeout.msgid = TIMER_VNA_HEARTBEAT_REPORT;
    if (SUCCESS != m_pMU->SetTimer(m_nHeartTimer, timeout))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetHeartbeat::MessageEntry Create timer for send registered vnm failed!\n");
        SkyAssert(0);
        SkyExit(-1, "CVNetHeartbeat::DoStartUp: call _mu->SetTimer failed.");
    }

    // �����鲥��
    STATUS ret = m_pMU->JoinMcGroup(string(ApplicationName()) + MUTIGROUP_SUF_REG_SYSTEM);
    if (SUCCESS != ret)
    {
        SkyAssert(false);
        string lastwords("CVNetHeartbeat failed to join group! ret = ");
        lastwords.append(to_string(ret,dec));
        SkyExit(-1,lastwords);
    }

    // ��ȡ������VNM����
    GetRegisteredSystem(m_strVNMName);

    if (m_strVNMName.empty())
    {
        return;
    }
    else
    {
        VNET_LOG(VNET_LOG_INFO, "CVNetHeartbeat::MessageEntry Goto report, have registered into %s.\n",
               m_strVNMName.c_str());
        DoHeartbeatReport();
    }
}

/************************************************************
* �������ƣ� DoRegisterSystem
* ���������� ����ע���VNM��Ϣ
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
void CVNetHeartbeat::DoRegisterSystem(const MessageFrame &message)
{
    // ����Ϣ���ͷ����кϷ��Լ��
    if (message.option.sender()._application != ApplicationName())
    {
        SkyAssert(0);
        return;
    }

    MessageRegisteredSystem msg;
    msg.deserialize(message.data);
    m_strVNMName = msg._system_name;
}

/************************************************************
* �������ƣ� DoHeartbeatReport
* ���������� �ϱ�����
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
void CVNetHeartbeat::DoHeartbeatReport()
{
    GetRegisteredSystem(m_strVNMName);

    if (m_strVNMName.empty())
    {
        VNET_LOG(VNET_LOG_INFO, "CVNetHeartbeat::DoHeartbeatReport  VNM is NULL.\n");   
        return;
    }

    // ������Ϣ
    MID receiver;
    receiver._application = m_strVNMName;
    receiver._process = PROC_VNM;
    receiver._unit = MU_VNM;
    MessageOption option(receiver, EV_VNA_HEARTBEAT_REPORT, 0, 0);
    m_pMU->Send("", option);

    if (m_bOpenDbgInf)
    {
        cout << "CVNetHeartbeat::DoHeartbeatReport send msg to VNM(" << m_strVNMName << ")" <<endl;  
    }
}

//�����������ã�ֹͣ����������ʱ��
//nSwitchFlag 1:������0 :�ر�
void CVNetHeartbeat::SetReportSwitch(int32 nSwitchFlag)
{
    if (0 == nSwitchFlag)
    {
        m_pMU->StopTimer(m_nHeartTimer);
    }
    else
    {
        // ���ù�����Ⱥ��Ϣ�����鲥��ʱ��
        TimeOut timeout;
        timeout.type = LOOP_TIMER;
        timeout.duration = TIMER_INTERVAL_VNA_HEARTBEAT_REPORT;
        timeout.msgid = TIMER_VNA_HEARTBEAT_REPORT;
        if (SUCCESS != m_pMU->SetTimer(m_nHeartTimer, timeout))
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetHeartbeat::MessageEntry Create timer for send registered vnm failed!\n");
            SkyAssert(0);
            SkyExit(-1, "CVNetHeartbeat::DoStartUp: call _mu->SetTimer failed.");
        }
    }  
}

int VNET_DBG_SET_VNA_HEART_SWITCH(int32 nSwitchFlag)
{
    if ((0 != nSwitchFlag) && (1 != nSwitchFlag))
    {
        cout << "Invalid input parameter, only 0 and 1 is valid!" << endl;
        return -1;
    }
        
    CVNetHeartbeat *pVNetHandle = CVNetHeartbeat::GetInstance();
    if (pVNetHandle)
    {
        pVNetHandle->SetReportSwitch(nSwitchFlag);
    }
    
    return 0;
}
DEFINE_DEBUG_FUNC(VNET_DBG_SET_VNA_HEART_SWITCH);
} // namespace zte_tecs

