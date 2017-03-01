/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：message_agent.cpp
* 文件标识：见配置管理计划书
* 内容摘要：消息代理相关接口实现文件
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
#include "message_agent.h"

MessageAgent *MessageAgent::instance = NULL;
static const string UnknownApplication = "null";

MessageAgent::MessageAgent()
{
    impl = MessageAgentImplFactory::GetImpl(Qpid);
}

const string& ApplicationName()
{
    MessageAgent *agent = MessageAgent::GetInstance();
    if (!agent)
    {
        SkyAssert(false);
        return UnknownApplication;
    }
    return agent->application();
}

const string& ProcessName()
{
    MessageAgent *agent = MessageAgent::GetInstance();
    if (!agent)
    {
        SkyAssert(false);
        return UnknownApplication;
    }
    return agent->process();
}

STATUS AddKeyMu(const string& name)
{
    if(name.empty())
    {
        SkyAssert(false);
        return ERROR_INVALID_ARGUMENT;
    }
    
    MessageAgent *agent = MessageAgent::GetInstance();
    if (!agent)
    {
        SkyAssert(false);
        return ERROR_NOT_READY;
    }
    return agent->AddKeyUnit(name);
}

STATUS DelKeyMu(const string& name)
{
    if(name.empty())
    {
        SkyAssert(false);
        return ERROR_INVALID_ARGUMENT;
    }
    
    MessageAgent *agent = MessageAgent::GetInstance();
    if (!agent)
    {
        SkyAssert(false);
        return ERROR_NOT_READY;
    }
    return agent->DelKeyUnit(name);
}

void DestroyMessageAgent()
{
    MessageAgent *agent = MessageAgent::GetInstance();
    if (!agent)
    {
        return;
    }
    agent->Stop();
    delete agent;
}

//打印消息代理概况
void DbgShowMessageAgent()
{
    MessageAgent *agent = MessageAgent::GetInstance();
    if (!agent)
    {
        return;
    }
    agent->Summary();
}
DEFINE_DEBUG_FUNC(DbgShowMessageAgent);
DEFINE_DEBUG_CMD(ma,DbgShowMessageAgent);

//打印消息单元表
void DbgShowUnitTable()
{
    MessageAgent *agent = MessageAgent::GetInstance();
    if (!agent)
    {
        return;
    }
    agent->ShowUnitTable();
}
DEFINE_DEBUG_FUNC(DbgShowUnitTable);
DEFINE_DEBUG_CMD(mu,DbgShowUnitTable);

//打印消息单元收到的消息，第二个参数不填表示跟踪所有
void DbgTraceMu(const char* name,uint32 message_id)
{
    if(!name || strlen(name) == 0)
    {
        MessageAgent *agent = MessageAgent::GetInstance();
        if (!agent)
            printf("failed to get MessageAgent instance!\n");
        else
            agent->ShowTraceUnit();

        return;
    }
    
    //如果第二个参数不填，或者为0，表示跟踪打印该消息单元所有收到的消息
    uint32 traced_id = message_id;
    if(0 == traced_id)
        traced_id = 0xFFFFFFFF;
    
    MessageAgent *agent = MessageAgent::GetInstance();
    if (!agent)
    {
        printf("failed to get MessageAgent instance!\n");
        return;
    }

    STATUS ret = agent->TraceUnit(name,traced_id);
    if(ret != SUCCESS)
    {
        if(ret == ERROR_OBJECT_NOT_EXIST)
        {
            printf("message unit %s does not exist!\n",name);
        }
        
        printf("failed to trace unit %s with message id %u!\n",name,traced_id);
        return;
    }

    if(traced_id == 0xFFFFFFFF)
    {
        printf("OK, All messages of %s will be traced!\n",name);
    }
    else
    {
        printf("OK, Message id %u of %s will be traced!\n",traced_id,name);
    }
    return;
}
DEFINE_DEBUG_FUNC(DbgTraceMu);
DEFINE_DEBUG_FUNC_ALIAS(tracemu,DbgTraceMu);

void DbgUnTraceMu(const char* name)
{   
    STATUS ret=0;
    string tmpname;

    
    MessageAgent *agent = MessageAgent::GetInstance();
    if (!agent)
    {
        printf("failed to get MessageAgent instance!\n");
        return;
    }
    if(!name || strlen(name) == 0)
    {
        printf("Cancel all message trace.\n");
        ret = agent->TraceUnit(tmpname,0);
    }
    else
    {
        ret = agent->TraceUnit(name,0);
    }

    if(ret != SUCCESS)
    {
        if(ret == ERROR_OBJECT_NOT_EXIST)
        {
            printf("message unit %s does not exist!\n",name);
        }
        
        printf("failed to cancel message trace %s!\n",name);
        return;
    }
    //printf("Message trace of %s is canceld.\n",name);
    return;
}
DEFINE_DEBUG_FUNC(DbgUnTraceMu);
DEFINE_DEBUG_FUNC_ALIAS(untracemu,DbgUnTraceMu);

//打印组播成员关系表
void DbgShowMcTable()
{
    MessageAgent *agent = MessageAgent::GetInstance();
    if (!agent)
    {
        printf("failed to get MessageAgent instance!\n");
        return;
    }
    agent->ShowMcTable();
}
DEFINE_DEBUG_FUNC(DbgShowMcTable);

void DbgShowRouteTable()
{
    MessageAgent *agent = MessageAgent::GetInstance();
    if (!agent)
    {
        printf("failed to get MessageAgent instance!\n");
        return;
    }
    agent->ShowRouteTable();
}
DEFINE_DEBUG_FUNC(DbgShowRouteTable);

void DbgShowTraceUnit()
{
    MessageAgent *agent = MessageAgent::GetInstance();
    if (!agent)
    {
        printf("failed to get MessageAgent instance!\n");
        return;
    }
    agent->ShowTraceUnit();
}
DEFINE_DEBUG_FUNC(DbgShowTraceUnit);

void DbgAddKeyUnit(const char* unit)
{
    if(!unit)
    {
        printf("Please give an unit name!\n");
        return;
    }
    
    MessageAgent *agent = MessageAgent::GetInstance();
    if (!agent)
    {
        printf("failed to get MessageAgent instance!\n");
        return;
    }
    
    STATUS ret = agent->AddKeyUnit(unit);
    if(SUCCESS != ret)
    {
        printf("failed to add key mu! ret = %d\n", ret);
        return;
    }
    
    agent->ShowKeyUnit();
}
DEFINE_DEBUG_FUNC(DbgAddKeyUnit);

void DbgDelKeyUnit(const char* unit)
{
    if(!unit)
    {
        printf("Please give an unit name!\n");
        return;
    }
    
    MessageAgent *agent = MessageAgent::GetInstance();
    if (!agent)
    {
        printf("failed to get MessageAgent instance!\n");
        return;
    }
    agent->DelKeyUnit(unit);
}
DEFINE_DEBUG_FUNC(DbgDelKeyUnit);

void DbgShowKeyUnit()
{
    MessageAgent *agent = MessageAgent::GetInstance();
    if (!agent)
    {
        printf("failed to get MessageAgent instance!\n");
        return;
    }
    agent->ShowKeyUnit();
}
DEFINE_DEBUG_FUNC(DbgShowKeyUnit);
DEFINE_DEBUG_CMD(kmu,DbgShowKeyUnit);

//销毁消息代理
void DbgDestroyMessageAgent()
{
    DestroyMessageAgent();
}
DEFINE_DEBUG_FUNC(DbgDestroyMessageAgent);






