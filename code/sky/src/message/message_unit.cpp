/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：message_unit.cpp
* 文件标识：见配置管理计划书
* 内容摘要：消息单元实现文件
* 当前版本：1.0
* 作    者：张文剑
* 完成日期：2011年8月16日
*
* 修改记录1：
*     修改日期：2011/07/01
*     版 本 号：V1.0
*     修 改 人：张文剑
*     修改内容：创建
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
        //todo:这里要告警或记录日志
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

    //补充填写消息发送方地址
    MID self_mid(agent->application(),agent->process(),name());
    option.setSender(self_mid);

    //补充完整消息接收方地址
    MID final_receiver_mid = option.receiver();
    if (final_receiver_mid._unit.empty())
    {
        //接收方消息单元名称必须填写
        //当接收方消息单元名称为"*"时，表示接收方是消息代理自身，而不是指所有消息单元
        //星号这个特殊名称通常用于sky模块内部
        return ERROR_BAD_ADDRESS;
    }

    if (final_receiver_mid._process.empty())
    {
        //process没填时，认为是在向进程内某个消息单元发送消息
        if(final_receiver_mid._application.empty() == false)
        {
            //此时application如果填了，应该等于本进程的application
            cout << "invalid mid " << final_receiver_mid << endl;
            SkyAssert(final_receiver_mid._application == self_mid._application);
            return false;
        }
        final_receiver_mid._process = self_mid._process;
    }

    if (final_receiver_mid._application.empty())
    {
        //application没填时，认为是在向本进程所属的application发送消息
        final_receiver_mid._application = self_mid._application;
    }

    //更新补充完毕的接收方地址
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
    //组织完成后一次性打印，可以避免启动期间多线程打印交叉显示的混乱现象
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
    
    //使用线程的tid和当前的本地时间生成随机的名称
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


