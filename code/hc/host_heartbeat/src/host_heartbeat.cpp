/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�host_agent.cpp
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��HostAgent��ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�Ԭ����
* ������ڣ�2011��8��9��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/8/9
*     �� �� �ţ�V1.0
*     �� �� �ˣ�Ԭ����
*     �޸����ݣ�����
*******************************************************************************/
#include "host_heartbeat.h"
#include "event.h"
#include "mid.h"
#include "tecs_errcode.h"
#include "msg_host_manager_with_host_agent.h"
#include "msg_host_manager_with_host_handler.h"
#include "vm_instance.h"
#include "instance_pool.h"
#include "storage.h"
#include "tecs_config.h"
#include "registered_system.h"
#include "log_agent.h"
//#include "light_agent.h"
#include <fstream>

namespace zte_tecs
{

// ��ʱ����Ϣ��
#define TIMER_HEARTBEAT_REPORT      EV_TIMER_1  // �����ϱ�������Ϣ��CC

HostHeartbeat *HostHeartbeat::_instance = NULL;

/******************************************************************************/
HostHeartbeat::HostHeartbeat():
        _cluster_name(),
        _timer_id(INVALID_TIMER_ID),
        _timer_duration(0)
{
    _mu = NULL;
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
* 2012/5         V1.0       lixiaocheng         ����
***************************************************************/
STATUS HostHeartbeat::StartMu(const string& name)
{
    if(_mu)
    {
        SkyAssert(false);
        return ERROR_DUPLICATED_OP;
    }
    
    // ��Ϣ��Ԫ�Ĵ����ͳ�ʼ��
    _mu = new MessageUnit(name);
    if (!_mu)
    {
        OutPut(SYS_EMERGENCY, "Create mu %s failed!\n",name.c_str());
        return ERROR_NO_MEMORY;
    }

    STATUS ret = _mu->SetHandler(this);
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "SetHandler mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }

    ret = _mu->Run();
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "Run mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }

    ret = _mu->Register();
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "Register mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }

    //���Լ������ϵ���Ϣ����ʹ��Ϣ��Ԫ״̬�л�������̬
    MessageFrame message(SkyInt32(0), EV_POWER_ON);
    ret = _mu->Receive(message);
    if (SUCCESS != ret)
    {
        SkyAssert(0);
        return ret;
    }
    _mu->Print();
    return SUCCESS;
}

/******************************************************************************/
void HostHeartbeat::MessageEntry(const MessageFrame &message)
{
    switch (_mu->get_state())
    {
    case S_Startup:
    {
        switch (message.option.id())
        {
        case EV_POWER_ON:
        {
            DoStartUp();
            _mu->set_state(S_Work);
            OutPut(SYS_NOTICE, "%s power on!\n",_mu->name().c_str());
            break;
        }
        default:
            OutPut(SYS_NOTICE, "Ignore any message except power on!\n");
            break;
        }
        break;
    }

    case S_Work:
    {
        switch (message.option.id())
        {
        case TIMER_HEARTBEAT_REPORT:
        {
            DoHeartbeatReport();
            break;
        }
        default:
        {
            cout << "HostAgent::MessageEntry: receive an unkown message!"
            << endl;
            OutPut(SYS_NOTICE, "Unknown message: %d.\n", message.option.id());
            break;
        }
        }
        break;
    }

    default:
        break;
    }
}

/******************************************************************************/
void HostHeartbeat::DoStartUp()
{
    // ��ȡ�����ļ�Ⱥ����
    GetRegisteredSystem(_cluster_name);

    // ���ù�����Ⱥ��Ϣ�����鲥��ʱ��
    TimeOut timeout;
    timeout.type = LOOP_TIMER;
    timeout.duration = 5 * 1000;
    timeout.msgid = TIMER_HEARTBEAT_REPORT;
    if (SUCCESS != _mu->SetTimer(_mu->CreateTimer(), timeout))
    {
        OutPut(SYS_EMERGENCY, "Create timer for send registered cluster!\n");
        SkyAssert(0);
        SkyExit(-1, "HostHaertbeat::DoStartUp: call _mu->SetTimer failed.");
    }

    if (_cluster_name.empty())
    {
        return;
    }
    else
    {
        OutPut(SYS_INFORMATIONAL, "Goto report, have registered into %s.\n",
               _cluster_name.c_str());
        DoHeartbeatReport();
    }
}

/******************************************************************************/
void HostHeartbeat::DoHeartbeatReport()
{
    GetRegisteredSystem(_cluster_name);

    if (_cluster_name.empty())
    {
        return;
    }

    // ������Ϣ
    MID receiver;
    receiver._application = _cluster_name;
    receiver._process = PROC_CC;
    receiver._unit = MU_HOST_MANAGER;
    MessageOption option(receiver, EV_HOST_HEARTBEAT_REPORT, 0, 0);
    _mu->Send("", option);
}

} // namespace zte_tecs

