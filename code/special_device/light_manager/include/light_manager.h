/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：light_manager.h
* 文件标识：无
* 内容摘要：点灯管理进程头文件
* 当前版本：1.0
* 作    者：颜伟
* 完成日期：2011年8月22日
*
* 修改记录1：
*     修改日期：2011/08/016
*     版 本 号：V1.0
*     修 改 人：颜伟
*     修改内容：创建
*******************************************************************************/

#ifndef LIGHT_MANAGER_H
#define LIGHT_MANAGER_H
#include "light_common.h"

class LightManager: public UdsMessenger
{
public:    
    static LightManager *GetInstance()
    {
        if (NULL == _instance)
        {
            _instance = new LightManager("LightManager"); 
        }
        return _instance;
    }
    void get_proc_list(list<LightProcess> &list)
    {
        list = _proc_list;
    }
    STATUS LightLed(const LightInfo &info);
    STATUS PowerOn(void);
    STATUS SendLightInfoReq(void);
    STATUS RecvLightInfoAck(const LightInfo &info);
    STATUS CheckLightProcess(void);
    STATUS UpdateLightProcess(void);
    void MessageEntry(uint32 id,void *data,uint32 length,uint32 version);
    LightManager(const string& name):UdsMessenger(name)
    {
        _mutex.SetName("LightManagerMutex");
        _mutex.Init();

    };
    ~LightManager()
    {
    };
private:
    //锁住lightprocess list
    void Lock()
    {
        _mutex.Lock();
    };

    //解锁lightprocess list
    void UnLock()
    {
        _mutex.Unlock();
    };
    Mutex _mutex;    
    static LightManager *_instance;
    list<LightProcess> _proc_list;
};
#endif


