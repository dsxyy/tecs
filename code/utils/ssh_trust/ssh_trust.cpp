/******************************************************************************
* Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�ssh_trust.cpp
* �ļ���ʶ��
* ����ժҪ��ssh ��֤���̵���Ϣ������
* ��ǰ�汾��1.0
* ��    �ߣ�zhangyb
* ������ڣ�2012��7��26��
*
* �޸ļ�¼1��
*     �޸����ڣ�2012��7��26��
*     �� �� �ţ�V1.0
*     �� �� �ˣ�zhangyb
*     �޸����ݣ�����
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

#define TIMER_MULTICAST_SSH_KEY      EV_TIMER_1  // �㲥ssh key�Ķ�ʱ��

SShTrust* SShTrust::_instance = NULL;

/******************************************************************************/
STATUS SShTrust::StartMu(const string& name)
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
    //��ʱ�鲥�Լ���key��ͬ��ĳ�Ա������Ӧ��ȷ�ϵĿɿ��Ա�֤��ͨ����ʱ5���鲥��ʵ�ֿɿ��� 
    MID receiver("group", ApplicationName()+MUTIGROUP_SSH_TRUST); 
    MessageOption option(receiver, EV_PUBLISH_SSH_PUBKEY, 0, 0); 
    _mu->Send(_my_key, option); 
}

STATUS SShTrust::JoinFriendKey(const string &key)
{
    STATUS ret;

    //�����Ƿ��Ѿ����ڣ��������ʾ�Ѿ����룬����������
    vector<string>::iterator it = find(_all_keys.begin(),_all_keys.end(),key);
    if(it != _all_keys.end())
    {
        Debug("key exist:%s !\n",key.c_str());
        return SUCCESS;
    }

    //����ɹ����ż��룬���򱾴δ���ʧ�ܣ������´εĴ���
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
    //����ʱ5����鲥��5����һ����ŵľ���ֵ����ֹ�����鲥�Լ��鲥����ʱ
    STATUS ret = 0;
    TIMER_ID    timer;
    // ���ö�ʱ��
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

    //��ȡ�Լ���key,���鲥��ʱ���鲥��ͬ��ĳ�Ա
    //ͬʱ���룬�鲥���� ��������ܵ��Լ����鲥keyʱ��Ҳ������   
    SshGetPubkey(_my_key);
    //_all_keys.push_back(_my_key);

    return ;
}

} //namespace zte_tecs


