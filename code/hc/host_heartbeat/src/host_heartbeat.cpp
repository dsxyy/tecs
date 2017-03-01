/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：host_agent.cpp
* 文件标识：见配置管理计划书
* 内容摘要：HostAgent类实现文件
* 当前版本：1.0
* 作    者：袁进坤
* 完成日期：2011年8月9日
*
* 修改记录1：
*     修改日期：2011/8/9
*     版 本 号：V1.0
*     修 改 人：袁进坤
*     修改内容：创建
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

// 定时器消息号
#define TIMER_HEARTBEAT_REPORT      EV_TIMER_1  // 周期上报心跳消息给CC

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
* 函数名称： StartMu
* 功能描述： 启动mu的函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2012/5         V1.0       lixiaocheng         创建
***************************************************************/
STATUS HostHeartbeat::StartMu(const string& name)
{
    if(_mu)
    {
        SkyAssert(false);
        return ERROR_DUPLICATED_OP;
    }
    
    // 消息单元的创建和初始化
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

    //给自己发送上电消息，促使消息单元状态切换到工作态
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
    // 获取归属的集群名称
    GetRegisteredSystem(_cluster_name);

    // 设置归属集群信息周期组播定时器
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

    // 发送消息
    MID receiver;
    receiver._application = _cluster_name;
    receiver._process = PROC_CC;
    receiver._unit = MU_HOST_MANAGER;
    MessageOption option(receiver, EV_HOST_HEARTBEAT_REPORT, 0, 0);
    _mu->Send("", option);
}

} // namespace zte_tecs

