/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�message_unit.cpp
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ����Ϣ��Ԫʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ����Ľ�
* ������ڣ�2011��8��16��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/07/01
*     �� �� �ţ�V1.0
*     �� �� �ˣ����Ľ�
*     �޸����ݣ�����
*******************************************************************************/
#include "common.h"
#include "message_unit.h"
#include "message_agent.h"

static int mu_print_on = 0;  
DEFINE_DEBUG_VAR(mu_print_on);
#define Debug(fmt,arg...) \
        do \
        { \
            if(mu_print_on) \
            { \
                printf("[%s:%d]"fmt,__func__,__LINE__, ##arg); \
            } \
        }while(0)
        
MessageUnit::~MessageUnit() 
{
    MessageAgent *agent = MessageAgent::GetInstance();
    if (!agent)
    {
        //todo:����Ҫ�澯���¼��־
        return;
    }
    agent->DeregisterUnit(this);
    Debug("message unit %s destroyed!\n",name().c_str());
};

STATUS MessageUnit::Register()
{
    MessageAgent *agent = MessageAgent::GetInstance();
    if (!agent)
    {
        return ERROR_NOT_READY;
    }
    return agent->RegisterUnit(this);
}

STATUS MessageUnit::CompleteOption(MessageOption& option)
{
    MessageAgent *agent = MessageAgent::GetInstance();
    if (!agent)
    {
        return ERROR_NOT_READY;
    }

    //������д��Ϣ���ͷ���ַ
    MID self_mid(agent->application(),agent->process(),name());
    option.setSender(self_mid);

    //����������Ϣ���շ���ַ
    MID final_receiver_mid = option.receiver();
    if (final_receiver_mid._unit.empty())
    {
        //���շ���Ϣ��Ԫ���Ʊ�����д
        //�����շ���Ϣ��Ԫ����Ϊ"*"ʱ����ʾ���շ�����Ϣ��������������ָ������Ϣ��Ԫ
        //�Ǻ������������ͨ������skyģ���ڲ�
        return ERROR_BAD_ADDRESS;
    }

    if (final_receiver_mid._process.empty())
    {
        //processû��ʱ����Ϊ�����������ĳ����Ϣ��Ԫ������Ϣ
        if(final_receiver_mid._application.empty() == false)
        {
            //��ʱapplication������ˣ�Ӧ�õ��ڱ����̵�application
            cout << "invalid mid " << final_receiver_mid << endl;
            SkyAssert(final_receiver_mid._application == self_mid._application);
            return false;
        }
        final_receiver_mid._process = self_mid._process;
    }

    if (final_receiver_mid._application.empty())
    {
        //applicationû��ʱ����Ϊ�����򱾽���������application������Ϣ
        final_receiver_mid._application = self_mid._application;
    }

    //���²�����ϵĽ��շ���ַ
    option.setReceiver(final_receiver_mid);
    return SUCCESS;
}

STATUS MessageUnit::SendFromTmp(const string& data, MessageOption& option)
{
    MessageUnit temp_mu(TempUnitName("message_unit_tmp")); 
    temp_mu.Register(); 
    return temp_mu.Send(data, option); 
}

STATUS MessageUnit::SendFromTmp(const Serializable& data,MessageOption &option)
{
    MessageUnit temp_mu(TempUnitName("message_unit_tmp")); 
    temp_mu.Register(); 
    return temp_mu.Send(data, option); 
}

STATUS MessageUnit::Send(const Serializable& message,MessageOption& option)
{
    string data;
    MessageAgent *agent = MessageAgent::GetInstance();
    if (!agent)
    {
        return ERROR_NOT_READY;
    }

    if(SUCCESS != CompleteOption(option))
    {
         return ERROR_BAD_ADDRESS;
    }

    try
    {
        data = message.serialize();
    }
    catch(...)
    {
        return ERROR_SERIALIZATION;
    }
    STATUS ret=SUCCESS;
    int    state=get_handle_msg_state();
    set_handle_msg_state(S_Snd_Msg);
    ret=agent->SendMessage(data,option);
    set_handle_msg_state(state);
    return ret;
}

STATUS MessageUnit::Send(const string& data,MessageOption& option)
{
    MessageAgent *agent = MessageAgent::GetInstance();
    if (!agent)
    {
        return ERROR_NOT_READY;
    }

    if(SUCCESS != CompleteOption(option))
    {
         return ERROR_BAD_ADDRESS;
    }
    STATUS ret=SUCCESS;
    int    state=get_handle_msg_state();
    set_handle_msg_state(S_Snd_Msg);
    ret=agent->SendMessage(data,option);
    set_handle_msg_state(state);
    return ret;
}

STATUS MessageUnit::JoinMcGroup(const string& group)
{
    MessageAgent *agent = MessageAgent::GetInstance();
    if (!agent)
    {
        return ERROR_NOT_READY;
    }
    return agent->JoinMcGroup(group, this->name());
}

STATUS MessageUnit::ExitMcGroup(const string& group)
{
    MessageAgent *agent = MessageAgent::GetInstance();
    if (!agent)
    {
        return ERROR_NOT_READY;
    }
    return agent->ExitMcGroup(group, this->name());
}

STATUS MessageUnit::GetSelfMID(MID &self)
{
    self._unit = name();
    self._process.assign(ProcessName());
    self._application.assign(ApplicationName());
    return SUCCESS;
}

string MessageUnit::GetSelfMID() 
{
    MID  self;
    self._unit = name();
    self._process.assign(ProcessName());
    self._application.assign(ApplicationName());
    
    return  self.serialize();
}
void MessageUnit::Print()
{
    MessageAgent *agent = MessageAgent::GetInstance();
    if (!agent)
    {
        return;
    }
    ostringstream oss;
    oss <<"-----------Messsage Unit Information-----------"<< endl;
    oss <<"application = " << agent->application() << endl;
    oss <<"process = " << agent->process() << endl;
    oss <<"unit = " << name() << endl;
    oss <<"work thread tid = " << get_task_tid() << endl;
    oss <<"-----------------------------------------------"<< endl;
    //��֯��ɺ�һ���Դ�ӡ�����Ա��������ڼ���̴߳�ӡ������ʾ�Ļ�������
    printf(oss.str().c_str());
}

string TempUnitName(const string& prefix)
{
    string _prefix;
    if(NULL != index(prefix.c_str(), '.'))
    {
        cerr << "warning! illeage unit name prefix: " << prefix << endl;
        SkyAssert(false);
        _prefix.assign("badprefix");
    }
    else
    {
        _prefix = prefix;
    }
    
    //ʹ���̵߳�tid�͵�ǰ�ı���ʱ���������������
    pid_t tid = (pid_t)syscall(__NR_gettid);
    struct timeval tv;
    gettimeofday(&tv,NULL);
    time_t now = tv.tv_sec;
    struct tm dt;
    localtime_r(&now, &dt);
                 
    stringstream ss;
    ss << _prefix 
        << "-" << dec << tid << "-" 
        << dec << dt.tm_year + 1900
        << dec << setfill('0') << setw(2) << dt.tm_mon + 1
        << dec << setfill('0') << setw(2) << dt.tm_mday
        << dec << setfill('0') << setw(2) << dt.tm_hour
        << dec << setfill('0') << setw(2) << dt.tm_min
        << dec << setfill('0') << setw(2) << dt.tm_sec
        << "-" << dec << tv.tv_usec;
    
    return ss.str();
}

void DbgTempUnitName()
{
    cout << TempUnitName() << endl;
}

DEFINE_DEBUG_FUNC(DbgTempUnitName);


