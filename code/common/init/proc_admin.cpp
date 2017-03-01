/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：proc_admin.cpp
* 文件标识：见配置管理计划书
* 内容摘要：ProcAdmin类实现文件
* 当前版本：1.0
* 作    者：袁进坤
* 完成日期：2011年11月12日
*
* 修改记录1：
*     修改日期：2011/11/12
*     版 本 号：V1.0
*     修 改 人：袁进坤
*     修改内容：创建
*******************************************************************************/
#include "proc_admin.h"
#include "event.h"
#include "mid.h"
#include "registered_system.h"
#include "log_agent.h"
using namespace zte_tecs;
namespace zte_tecs
{

ProcAdmin *ProcAdmin::_instance = NULL;

/******************************************************************************/
ProcAdmin *ProcAdmin::GetInstance()
{
    if (NULL == _instance)
    {
        _instance = new ProcAdmin();
    }
    return _instance;
}

/******************************************************************************/
STATUS ProcAdmin::Init()
{
    STATUS ret = Start(MU_PROC_ADMIN);
    if (SUCCESS != ret)
    {
        return ret;
    }
    
    MessageFrame message(SkyInt32(0), EV_POWER_ON);
    return _instance->Receive(message);
}
    
/******************************************************************************/
void ProcAdmin::MessageEntry(const MessageFrame &message)
{
    switch (m->get_state())
    {
    case S_Startup:
    {
        switch (message.option.id())
        {
        case EV_POWER_ON:
        {
            DoStartUp();
            m->set_state(S_Work);
            OutPut(SYS_NOTICE, "%s power on!\n",m->name().c_str());
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
        case EV_REGISTERED_SYSTEM:
        {
            DoRegisterSystem(message);
            break;
        }
        case EV_REGISTERED_SYSTEM_QUERY:
        {
            break;
        }
        default:
        {
            cout << "ProcAdmin::MessageEntry: receive an unkown message!"
            << endl;
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
STATUS ProcAdmin::DoStartUp()
{
    // 加入组播组
    STATUS ret = m->JoinMcGroup(string(ApplicationName()) + MUTIGROUP_SUF_REG_SYSTEM);
    if (SUCCESS != ret)
    {
        SkyAssert(false);
        string lastwords("ProcAdmin failed to join group! ret = ");
        lastwords.append(to_string(ret,dec));
        SkyExit(-1,lastwords);
    }

    // 组播询问归属信息
    MID receiver("group", string(ApplicationName()) + MUTIGROUP_SUF_REG_SYSTEM);
    MessageOption option(receiver, EV_REGISTERED_SYSTEM_QUERY, 0, 0);
    return m->Send(SkyInt32(0), option);
}

/******************************************************************************/
void ProcAdmin::DoRegisterSystem(const MessageFrame &message)
{
    // 对消息发送方进行合法性检查
    if (message.option.sender()._application != ApplicationName())
    {
        SkyAssert(0);
        return;
    }

    MessageRegisteredSystem msg;
    msg.deserialize(message.data);
    SetRegisteredSystem(msg._system_name);
}

} // namespace zte_tecs

