/******************************************************************************
* Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：ssh_trust.cpp
* 文件标识：
* 内容摘要：ssh 认证进程的消息处理函数
* 当前版本：1.0
* 作    者：zhangyb
* 完成日期：2012年7月26日
*
* 修改记录1：
*     修改日期：2012年7月26日
*     版 本 号：V1.0
*     修 改 人：zhangyb
*     修改内容：创建
******************************************************************************/
#include "ssh_trust.h"
#include "event.h"
#include "mid.h"
#include "tecs_errcode.h"
#include "tecs_config.h"
#include "log_agent.h"

namespace zte_tecs
{
static int ssh_auth_print_on = 0;
DEFINE_DEBUG_VAR(ssh_auth_print_on);
#define Debug(fmt,arg...) \
        do \
        { \
            if(ssh_auth_print_on) \
            { \
                    OutPut(SYS_DEBUG,fmt,##arg); \
            } \
        }while(0)

#define TIMER_MULTICAST_SSH_KEY      EV_TIMER_1  // 广播ssh key的定时器

SShTrust* SShTrust::_instance = NULL;

/******************************************************************************/
STATUS SShTrust::StartMu(const string& name)
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
void SShTrust::MessageEntry(const MessageFrame &message)
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
            case TIMER_MULTICAST_SSH_KEY:
            {
                Debug("Ssh muticast key\n");
                MultiCastMyKey();
                break;
            }
            case EV_PUBLISH_SSH_PUBKEY:
            {
                Debug("Ssh rcv pub key\n");
                JoinFriendKey(message.data);
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
void SShTrust::MultiCastMyKey()
{
    //定时组播自己的key给同组的成员，不做应答确认的可靠性保证，通过定时5秒组播来实现可靠性 
    MID receiver("group", ApplicationName()+MUTIGROUP_SSH_TRUST); 
    MessageOption option(receiver, EV_PUBLISH_SSH_PUBKEY, 0, 0); 
    _mu->Send(_my_key, option); 
}

STATUS SShTrust::JoinFriendKey(const string &key)
{
    STATUS ret;

    //查找是否已经存在，存在则表示已经加入，不再做处理
    vector<string>::iterator it = find(_all_keys.begin(),_all_keys.end(),key);
    if(it != _all_keys.end())
    {
        Debug("key exist:%s !\n",key.c_str());
        return SUCCESS;
    }

    //插入成功，才加入，否则本次处理失败，依赖下次的处理
    ret = SshTrust(key);
    if (SUCCESS == ret)
    { 
        _all_keys.push_back(key);
        OutPut(SYS_DEBUG, "key join:%s !\n",key.c_str());
        return ret;
    }

    OutPut(SYS_EMERGENCY, "join ssh key fail %u!\n",ret);

    return ret;
}

void SShTrust::SetMultiCastTimer()
{ 
    //做定时5秒的组播，5秒是一个大概的经验值，防止过度组播以及组播不及时
    STATUS ret = 0;
    TIMER_ID    timer;
    // 设置定时器
    if (INVALID_TIMER_ID == (timer = _mu->CreateTimer()))
    {
        OutPut(SYS_EMERGENCY, " Ssh auth create timer failed!\n");
        SkyAssert(INVALID_TIMER_ID != timer);
        SkyExit(-1, "Ssh auth creat timer failed.");
    }

    TimeOut timeout;
    timeout.type = LOOP_TIMER;
    timeout.duration = 5000;
    timeout.msgid = TIMER_MULTICAST_SSH_KEY;
    if (SUCCESS != (ret = _mu->SetTimer(timer,timeout)))
    {
        OutPut(SYS_EMERGENCY, "Ssh auth set mulitcast ssh key timer fail:%u!\n",ret);
        SkyAssert(0);
        SkyExit(-1, "Ssh auth set mulitcast ssh key timer fail!");
    }

    return;
}

void SShTrust::JoinGroup(const string &group)
{
    STATUS ret = _mu->JoinMcGroup(group);
    Debug("join  group %s!\n",group.c_str());
    if (SUCCESS != ret)
    {
        SkyAssert(false);
        string lastwords("Ssh auth failed to join group! ret = ");
        lastwords.append(to_string(ret,dec));
        SkyExit(-1,lastwords);
    }

    return;
}

void SShTrust::DoStartUp()
{
    JoinGroup(string(ApplicationName()) + MUTIGROUP_SSH_TRUST);
    SetMultiCastTimer();

    //获取自己的key,在组播的时候，组播给同组的成员
    //同时加入，组播表中 ，避免接受到自己的组播key时，也做处理   
    SshGetPubkey(_my_key);
    //_all_keys.push_back(_my_key);

    return ;
}

} //namespace zte_tecs


