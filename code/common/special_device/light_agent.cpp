/*********************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司
* All rights reserved.
* 
* 文件名称：light_agent.cpp
* 文件标识：
* 内容摘要：点灯代理实现文件
* 当前版本：1.0
* 作    者：颜伟
* 完成日期：2011年7月25日
*
* 修改记录1：
*    修改日期：2011年7月25日
*    版 本 号：V1.0
*    修 改 人：颜伟
*    修改内容：创建
**********************************************************************/
#include "event.h"
#include "light_agent.h"

LightAgent *LightAgent::_instance = NULL;

static int light_agent_print_on = 0;  
DEFINE_DEBUG_VAR(light_agent_print_on);
#define light_agent_debug(fmt,arg...) \
        do \
        { \
            if(light_agent_print_on) \
            { \
                printf("[%s:%d]"fmt,__func__,__LINE__, ##arg); \
            } \
        }while(0)
 
/**********************************************************************
@brief 功能描述: 添加点灯信息 LightAgent公共成员函数

@li @b 入参列表：
@verbatim
LightInfo info 点灯信息
@endverbatim

@li @b 出参列表：
@verbatim
无
@endverbatim

@li @b 返回值列表：
@verbatim
SUCCESS 成功
ERROR   失败
@endverbatim

@li @b 接口限制：无
@li @b 其它说明：接口函数
************************************************************************/
STATUS LightAgent::TurnOn(const LightInfo &info)
{
    info.print();
    if (!info.valid())
    {
        return ERROR;
    }
    Lock();
    list<LightInfo>::iterator it;
    for (it = _info_list.begin(); it != _info_list.end(); it++)
    {
        if (info.compare(*it))
        {
            break;
        }
    }
    if (it == _info_list.end())
    {
        _info_list.push_back(info);
    }
    UnLock();
    return SUCCESS;
}
/**********************************************************************
@brief 功能描述: 撤消点灯信息 LightAgent公共成员函数

@li @b 入参列表：
@verbatim
LightInfo info 点灯信息
@endverbatim

@li @b 出参列表：
@verbatim
无
@endverbatim

@li @b 返回值列表：
@verbatim
SUCCESS 成功
ERROR   失败
@endverbatim

@li @b 接口限制：无
@li @b 其它说明：接口函数
************************************************************************/
STATUS LightAgent::TurnOff(const LightInfo &info)
{
    info.print();
    if (!info.valid())
    {
        return ERROR;
    }
    Lock();
    list<LightInfo>::iterator it;
    for (it = _info_list.begin(); it != _info_list.end(); it++)
    {
        if (info.compare(*it))
        {
            break;
        }
    }
    if (it != _info_list.end())
    {
        _info_list.erase(it);
    }
    UnLock();
    return SUCCESS;
}
/**********************************************************************
@brief 功能描述: 点灯代理线程入口 LightAgent公共成员函数

@li @b 入参列表：
@verbatim
uint32 id      消息id
void *data     消息体
uint32 length  消息长度
uint32 version 消息版本号
@endverbatim

@li @b 出参列表：
@verbatim
无
@endverbatim

@li @b 返回值列表：
@verbatim
无
@endverbatim

@li @b 接口限制：无
@li @b 其它说明：内部函数
************************************************************************/
void LightAgent::MessageEntry(uint32 id,void *data,uint32 length,uint32 version)
{
    light_agent_debug("LightAgent::MessageEntry id:%ld,length:%ld,version:%ld\r\n",id,length,version);
    switch(id)
    {
        case EV_LIGHT_INFO_REQ:
            DoLightInfo();
            break;
        default:
            break;
    }
}
/**********************************************************************
@brief 功能描述: 点灯代理线程处理请求消息 LightAgent私有成员函数

@li @b 入参列表：
@verbatim
无
@endverbatim

@li @b 出参列表：
@verbatim
无
@endverbatim

@li @b 返回值列表：
@verbatim
SUCCESS 成功
ERROR   失败
@endverbatim

@li @b 接口限制：无
@li @b 其它说明：内部函数
************************************************************************/
STATUS LightAgent::DoLightInfo(void)
{
    int index = 0;
    int aiMaxPri[LIGHT_TYPE_MAX];
    LightInfo atLightInfo[LIGHT_TYPE_MAX];

    for (index = 0; index < LIGHT_TYPE_MAX; index++)
    {
        aiMaxPri[index] = LIGHT_INFO_MIN_PRIORITY;
        atLightInfo[index]._type = (LightType)index;
    }

    Lock();
    list<LightInfo>::iterator it;
    for (it = _info_list.begin(); it != _info_list.end(); it++)
    {/*找到每种灯点灯信息优先级最高的*/
        if (aiMaxPri[it->_type] < it->_priority)
        {
            aiMaxPri[it->_type] = it->_priority;
            it->copy(atLightInfo[it->_type]);
        }
    }
    UdsParam uds(EV_LIGHT_INFO_ACK,NULL,0,0);
    for (index = 0; index < LIGHT_TYPE_MAX; index++)
    {
        uds.msgdata = (void *)&atLightInfo[index];
        uds.msgsize = sizeof(LightInfo); 
        light_agent_debug("send uds:%d,%d,%d,%lx to %s\n",uds.msgid,uds.msgver,uds.msgsize,uds.msgdata,GetPeerPath());
        if (SUCCESS != Send(GetPeerPath(),uds))
        {
            light_agent_debug("send uds to %s failed errno:%d\n",GetPeerPath(),errno);
        }
    }
    UnLock();
    return SUCCESS;
}
void DbgTurnOn(LightType type,LightState state,int pri,char *setter,char *remark)
{
    LightAgent *pla = LightAgent::GetInstance();
    if(!pla)
    {
        printf("LightAgent::GetInstance Failed\n");
        return;
    }
    LightInfo info(type,state,pri,setter,remark);
    pla->TurnOn(info);
    return;
}
DEFINE_DEBUG_FUNC(DbgTurnOn);
void DbgTurnOff(LightType type,LightState state,int pri,char *setter,char *remark)
{
    LightAgent *pla = LightAgent::GetInstance();
    if(!pla)
    {
        printf("LightAgent::GetInstance Failed\n");
        return;
    }
    LightInfo info(type,state,pri,setter,remark);
    pla->TurnOff(info);
    return;
}
DEFINE_DEBUG_FUNC(DbgTurnOff);
void DbgShowLightInfo(void)
{
    LightAgent *pla = LightAgent::GetInstance();
    if(!pla)
    {
        printf("LightAgent::GetInstance Failed\n");
        return;
    }
    list<LightInfo> infolist;
    list<LightInfo>::iterator it;
    pla->get_info_list(infolist);
    for (it = infolist.begin(); it != infolist.end(); it++)
    {
        it->print();
    }
    return;
}
DEFINE_DEBUG_FUNC(DbgShowLightInfo);


