/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�light_manager.h
* �ļ���ʶ����
* ����ժҪ����ƹ������ͷ�ļ�
* ��ǰ�汾��1.0
* ��    �ߣ���ΰ
* ������ڣ�2011��8��22��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/08/016
*     �� �� �ţ�V1.0
*     �� �� �ˣ���ΰ
*     �޸����ݣ�����
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
    //��סlightprocess list
    void Lock()
    {
        _mutex.Lock();
    };

    //����lightprocess list
    void UnLock()
    {
        _mutex.Unlock();
    };
    Mutex _mutex;    
    static LightManager *_instance;
    list<LightProcess> _proc_list;
};
#endif


