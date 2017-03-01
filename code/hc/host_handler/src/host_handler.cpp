/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：host_handler.cpp
* 文件标识：见配置管理计划书
* 内容摘要：HostHandler类实现文件
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
#include "host_handler.h"
#include "event.h"
#include "mid.h"
#include "tecs_errcode.h"
#include "msg_host_manager_with_host_handler.h"
#include "msg_host_manager_with_host_agent.h"
#include "tecs_config.h"
#include "registered_system.h"
#include "log_agent.h"
#include <fstream>

namespace zte_tecs
{

HostHandler* HostHandler::_instance = NULL;

/******************************************************************************/
HostHandler::HostHandler(): _cluster_name()
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
STATUS HostHandler::StartMu(const string& name)
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
void HostHandler::MessageEntry(const MessageFrame &message)
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
#if 0
        case EV_HOST_RAW_HANDLE_REQ:
        {
            DoRawHandle(message);
            break;
        }
#endif
        case EV_HOST_DEF_HANDLE_REQ:
        {
            DoDefHandle(message);
            break;
        }
        default:
        {
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
void HostHandler::DoStartUp()
{
    GetRegisteredSystem(_cluster_name);
    STATUS ret = _mu->JoinMcGroup(_cluster_name + MUTIGROUP_SUF_HOST_HANDLE);
    if (SUCCESS != ret)
    {
        SkyAssert(false);
        string lastwords("HostHandler failed to join group! ret = ");
        lastwords.append(to_string(ret,dec));
        SkyExit(-1,lastwords);
    }
}

#if 0
/******************************************************************************/
void HostHandler::DoRawHandle(const MessageFrame &message)
{
    // 判断消息发送方是否合法
    MID sender = message.option.sender();
    if ((sender._application != _cluster_name) &&
            (sender._application != ApplicationName()))
    {
        SkyAssert(0);
        return;
    }

    // 构造应答消息
    MessageHostRawHandleReq req;
    req.deserialize(message.data);
    MessageHostRawHandleAck ack;
    ack._command = req._command;
    ack._append = req._append;

    // 命令为空
    if (req._command.empty())
    {
        ack._ret_code = ERR_HANDLE_EMPTY;
    }
    // 执行命令并获取应答
    else
    {
        string command = req._command + " &> " + HANDLE_RESULT_FILE;
        if (-1 == system(command.c_str()))
        {
            ack._ret_code = ERR_HANDLE_FAILED;
        }
        else
        {
            ack._ret_code = ERR_HANDLE_OK;
            ifstream fin(HANDLE_RESULT_FILE);
            string line;
            while (getline(fin, line))
            {
                ack._ret_info += line + "\n";
            }
        }
    }

    // 发送应答
    MessageOption option(sender, EV_HOST_RAW_HANDLE_ACK, 0, 0);
    _mu->Send(ack, option);
}
#endif

/******************************************************************************/
void HostHandler::DoDefHandle(const MessageFrame &message)
{
    // 判断消息发送方是否合法
    MID sender = message.option.sender();
    if ((sender._application != _cluster_name) &&
            (sender._application != ApplicationName()))
    {
        OutPut(SYS_ALERT, "DoDefHandle: sender is err, %s != %s or %s!\n",
               sender._application.c_str(),
               _cluster_name.c_str(),
               ApplicationName().c_str());
        //SkyAssert(0);
        return;
    }

    // 构造应答消息
    MessageHostDefHandleReq req;
    req.deserialize(message.data);
    MessageHostDefHandleAck ack;
    ack._command = req._command;
    ack._argument = req._argument;
    ack._append = req._append;

    // 命令为空
    if (req._command.empty())
    {
        ack._ret_code = ERR_HANDLE_EMPTY;
        OutPut(SYS_ALERT, "Req command is empty!\n");
    }
    // 执行命令并获取应答
    else
    {
        string command = "./host_handle.sh";
        if (0 != access("./host_handle.sh", X_OK))
        {
            TecsConfig *config = TecsConfig::Get();
            command = config->get_scripts_path() + "/host_handle.sh";
        }
        command += " " + req._command + " " + req._argument;

        FILE *pp;
        if (NULL == (pp = popen(command.c_str(), "r")))
        {
            ack._ret_code = ERR_HANDLE_FAILED;
            OutPut(SYS_ALERT, "Run host_handle.sh failed!\n");
        }
        else
        {
            ack._ret_code = ERR_HOST_OK;

            char line[1024];
            while (fgets(line, sizeof(line), pp))
            {
                ack._ret_info += string(line) + "\n";
            }
            pclose(pp);

            OutPut(SYS_INFORMATIONAL,
                   "Run defined command = %s, argument = %s, result = %s.\n",
                   req._command.c_str(),
                   req._argument.c_str(),
                   ack._ret_info.c_str());
        }
    }

    // 发送应答
    MessageOption option(sender, EV_HOST_DEF_HANDLE_ACK, 0, 0);
    _mu->Send(ack, option);
}

} //namespace zte_tecs

void showscriptspath(void)
{
    TecsConfig *config = TecsConfig::Get();
    string path = config->get_scripts_path();
    cout<<"scripts path is:"<<path<<endl;
}
DEFINE_DEBUG_FUNC(showscriptspath);

