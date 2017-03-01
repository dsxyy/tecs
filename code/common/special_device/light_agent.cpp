/*********************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾
* All rights reserved.
* 
* �ļ����ƣ�light_agent.cpp
* �ļ���ʶ��
* ����ժҪ����ƴ���ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ���ΰ
* ������ڣ�2011��7��25��
*
* �޸ļ�¼1��
*    �޸����ڣ�2011��7��25��
*    �� �� �ţ�V1.0
*    �� �� �ˣ���ΰ
*    �޸����ݣ�����
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
@brief ��������: ��ӵ����Ϣ LightAgent������Ա����

@li @b ����б�
@verbatim
LightInfo info �����Ϣ
@endverbatim

@li @b �����б�
@verbatim
��
@endverbatim

@li @b ����ֵ�б�
@verbatim
SUCCESS �ɹ�
ERROR   ʧ��
@endverbatim

@li @b �ӿ����ƣ���
@li @b ����˵�����ӿں���
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
@brief ��������: ���������Ϣ LightAgent������Ա����

@li @b ����б�
@verbatim
LightInfo info �����Ϣ
@endverbatim

@li @b �����б�
@verbatim
��
@endverbatim

@li @b ����ֵ�б�
@verbatim
SUCCESS �ɹ�
ERROR   ʧ��
@endverbatim

@li @b �ӿ����ƣ���
@li @b ����˵�����ӿں���
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
@brief ��������: ��ƴ����߳���� LightAgent������Ա����

@li @b ����б�
@verbatim
uint32 id      ��Ϣid
void *data     ��Ϣ��
uint32 length  ��Ϣ����
uint32 version ��Ϣ�汾��
@endverbatim

@li @b �����б�
@verbatim
��
@endverbatim

@li @b ����ֵ�б�
@verbatim
��
@endverbatim

@li @b �ӿ����ƣ���
@li @b ����˵�����ڲ�����
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
@brief ��������: ��ƴ����̴߳���������Ϣ LightAgent˽�г�Ա����

@li @b ����б�
@verbatim
��
@endverbatim

@li @b �����б�
@verbatim
��
@endverbatim

@li @b ����ֵ�б�
@verbatim
SUCCESS �ɹ�
ERROR   ʧ��
@endverbatim

@li @b �ӿ����ƣ���
@li @b ����˵�����ڲ�����
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
    {/*�ҵ�ÿ�ֵƵ����Ϣ���ȼ���ߵ�*/
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


