/*********************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾
* All rights reserved.
* 
* �ļ����ƣ�light_agent.h
* �ļ���ʶ��
* ����ժҪ����ƴ���ͷ�ļ�
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

#ifndef LIGHT_AGENT_H
#define LIGHT_AGENT_H
#include "light_common.h"
#include "event.h"

class LightAgent: public UdsMessenger
{
public:   
    static LightAgent *GetInstance()
    {
        if (NULL == _instance)
        {
            _instance = new LightAgent("LightAgent"); 
        }
        return _instance;
    }
    void get_info_list(list<LightInfo> &list)
    {
        list = _info_list;
    }
    STATUS TurnOn(const LightInfo &info);
    STATUS TurnOff(const LightInfo &info);
    void MessageEntry(uint32 id,void *data,uint32 length,uint32 version);
    LightAgent(const string& name):UdsMessenger(name)
    {
        _mutex.SetName("LightAgentMutex");
        _mutex.Init();
    };
    ~LightAgent()
    {
    };
private:
    void Lock()
    {
        _mutex.Lock();
    };
    void UnLock()
    {
        _mutex.Unlock();
    };
    STATUS DoLightInfo(void);
    static LightAgent *_instance;
    Mutex _mutex;
    list<LightInfo> _info_list;
};
#endif


