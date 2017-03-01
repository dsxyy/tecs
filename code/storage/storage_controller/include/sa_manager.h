#ifndef SA_MANAGER_H
#define SA_MANAGER_H
#include "sky.h"
#include "mid.h"
#include "event.h"
#include "sc_db.h"
#include "pub.h"

namespace zte_tecs
{

#define SA_PERIOD_TIME_LEN            10000 // sa老化定时器时长，10s

class SaResourceSynReq 
{
public:
        MID    _sender;
        int64  _start_index;
        int64  _query_count;
};

class SaManager: public MessageHandlerTpl<MessageUnit>
{
public:
    static SaManager *GetInstance()
    {
        if(NULL == _instance)
        {
            _instance = new SaManager();
        }
        
        return _instance;
    };

    STATUS Init();
   
    ~SaManager(){};
    
    void MessageEntry(const MessageFrame& message);

private:
    SaManager()
    {
        _manager_period_timer = INVALID_TIMER_ID; 
        _db_set = NULL;
        _regedit_keep_time = -1;
    };    
    
    TIMER_ID            _manager_period_timer;
    ScDbOperationSet    *_db_set;
    int                 _regedit_keep_time;   

    static SaManager    *_instance;
    map<string, string>  _ifs_info;
    
    void DoSaDiscover(const MessageFrame& message);
    void DoSaInfoReport(const MessageFrame& message);
    void RefreshSa(void);
    void SetSaKeepTimer();    
    void DoEnableSa(const MessageFrame& message);
    void DoForgetSaById(const MessageFrame& message);   
    void DoMapCluster(const MessageFrame& message);
    void SendAlarm(string &ip, uint32 code, byte flag);
};

}

#endif
    
