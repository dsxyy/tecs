/*********************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司
* All rights reserved.
* 
* 文件名称：light_agent.h
* 文件标识：
* 内容摘要：点灯代理头文件
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


