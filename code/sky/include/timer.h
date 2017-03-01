#ifndef SKY_TIMER_H
#define SKY_TIMER_H
#include "pub.h"
#include "serialization.h"
#include "skytype.h"
#include "sys.h"
#include "exception.h"

typedef uint32 TIMER_ID;
#define INVALID_TIMER_ID ((TIMER_ID)0)
class TimerManager;
class TimerClient;

#define    RELATIVE_TIMER     1
#define    LOOP_TIMER         2
#define    ABSOLUTE_TIMER     3

#define DEFAULT_TIMER_OUT_VALUE_MSEC    100

class TimerConfig
{
public:
    TimerConfig()
    {
        tv_msec = DEFAULT_TIMER_OUT_VALUE_MSEC;
    }
    
    int tv_msec; //定时器扫描周期，单位是毫秒，默认等于100毫秒
};

class TimerMessage: public Serializable
{
public:
    TimerMessage()
    {
        tid = INVALID_TIMER_ID;
        msgid = 0;
        type = RELATIVE_TIMER;
        flow = 0;
        arg = 0;
    };
#if 0
    string serialize() const 
    {
        stringstream oss;
        string tmp;
        tmp = to_string<TIMER_ID>(tid,dec);
        oss << tmp << ",";
        tmp = to_string<uint32>(msgid,dec);
        oss << tmp << ",";
        tmp = to_string<uint32>(type,dec);
        oss << tmp << ",";
        tmp = to_string<uint32>(flow,dec);
        oss << tmp << ",";
        tmp = to_string<uintptr>(arg,hex);
        oss << tmp;
        return oss.str();
    };
    
    bool deserialize(const string &input) 
    {
        stringstream iss(input);    
        string tmp;
        
        getline(iss, tmp, ',');
        if(!from_string<TIMER_ID>(tid,tmp,dec))            
            return false;
            
        getline(iss, tmp, ',');
        if(!from_string<uint32>(msgid,tmp,dec))
            return false;

        getline(iss, tmp, ',');
        if(!from_string<uint32>(type,tmp,dec))
            return false;
            
        getline(iss, tmp, ',');
        if(!from_string<uint32>(flow,tmp,dec))
            return false;

        getline(iss, tmp, ',');
        if(!from_string<uintptr>(arg,tmp,hex))
            return false;

        return true;
    };
#endif
    SERIALIZE
    {
        SERIALIZE_BEGIN(TimerMessage);
        WRITE_DIGIT(tid);
        WRITE_DIGIT(msgid);
        WRITE_DIGIT(type);
        WRITE_DIGIT(flow);
        WRITE_DIGIT(arg);
        WRITE_STRING(str_arg);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(TimerMessage);
        READ_DIGIT(tid);
        READ_DIGIT(msgid);
        READ_DIGIT(type);
        READ_DIGIT(flow);
        READ_DIGIT(arg);
        READ_STRING(str_arg);
        DESERIALIZE_END();
    };
    
    TIMER_ID tid;
    uint32   msgid;
    uint32   type;
    uint32   flow;
    uintptr  arg;
    string   str_arg;
};

class TimeOut
{
public:
    uint32    type;
    uint32    duration;
    uint32    msgid;
    uintptr   arg;
    string    str_arg;
    
    TimeOut()
    {
        type = RELATIVE_TIMER;
        duration = 0;
        msgid = 0;
        arg = 0;
    };
    
    TimeOut(uint32 _msgid, uint32 _duration)
    {
        type = RELATIVE_TIMER;
        duration = _duration;
        msgid = _msgid;
        arg = 0;
    };
    
    bool Validate() const
    {
        if(RELATIVE_TIMER != type && LOOP_TIMER != type && ABSOLUTE_TIMER != type)
        {
            return false;
        }

        return true;
    }
};

class T_TimeValue
{
public:
    T_TimeValue():second(0),milisecond(0) {};
    T_TimeValue(uint32 sec,uint32 minisec):second(sec),milisecond(minisec){};
    T_TimeValue(const T_TimeValue& t):second(t.second),milisecond(t.milisecond){};
    //void Zero() {second = 0;milisecond = 0;};
    //bool IsZero() {return (second == 0 && milisecond == 0);};
    uint32  second;      
    uint32  milisecond;     
}; 

class T_TimeKey
{
public:
    T_TimeKey():second(0),milisecond(0),tid(0) {};
    T_TimeKey(const T_TimeValue& t,TIMER_ID id):second(t.second),milisecond(t.milisecond),tid(id) {};
    uint32  second;      
    uint32  milisecond;  
    TIMER_ID tid;
}; 

class TimerBlock;
class TimerManager
{
public:
    inline TimerBlock* ID_TO_POINTER(TIMER_ID tid) 
    {
        if(tid == INVALID_TIMER_ID || tid > (TIMER_ID)_pool.size())
            return NULL;
        
        return (_pool[tid-1]);
    }
    
    static bool CreateInstance()
    {
        if(NULL == instance)
        {
            try
            {
                instance = new TimerManager();
            }
            catch(...)
            {
                printf("exception happens when constructing TimerManager!\n");
                instance = NULL;
                return false;
            }
        }

        return true;
    };

    static TimerManager *GetInstance()
    {
        return instance;
    };
    
    ~TimerManager()
    {
        instance = NULL;
        if(INVALID_THREAD_ID != _workthread_id)
            KillThread(_workthread_id);
    };
    
    STATUS Start(int priority = 1);
    void Scan();
    TIMER_ID CreateTimer(TimerClient *client);
    STATUS SetTimer(TIMER_ID tid,const TimeOut& timeout);
    STATUS LoopTimer(TIMER_ID tid);
    STATUS StopTimer(TIMER_ID tid);
    STATUS KillTimer(TIMER_ID tid);
    void ShowAllTimers(int print_scan_table_only);
    uint32 GetFlowNumber(TIMER_ID tid);
    STATUS GetRemainedTime(TIMER_ID tid,T_TimeValue *remained);
    void TraceTimer(TIMER_ID tid);
    void UnTraceTimer(TIMER_ID tid);
    STATUS SetTimerConfig(const TimerConfig& time);
    STATUS GetTimerConfig(TimerConfig *ptimercfg);
    void TraceWaite(TIMER_ID tid,const TimerMessage *tm);

private:
    TimerManager()
    {
        _workthread_id = INVALID_THREAD_ID;
        rel_scan_table_mutex.Init();
        free_list_mutex.Init();
    };
    DISALLOW_COPY_AND_ASSIGN(TimerManager);
    STATUS InsertTimer(TimerBlock *ptTMCB);
    STATUS RemoveTimer(TimerBlock *ptTMCB);
    STATUS AllocateTimerBlock();

    static TimerManager *instance;  
    Mutex rel_scan_table_mutex;
    Mutex free_list_mutex;
    THREAD_ID _workthread_id;
    vector<TimerBlock*> _pool;
    map<T_TimeKey,TIMER_ID> rel_scan_table;
    deque<TIMER_ID> _freelist;
    TimerConfig _config;
};

class TimerClient
{
public:
    TimerClient():_name("unknown") {};
    TimerClient(const string& name):_name(name) {};
    
    virtual ~TimerClient()
    {
        TimerManager *pmut = TimerManager::GetInstance();
        if(!pmut)
            return;
            
        list<TIMER_ID>::iterator it;
        for ( it=timers.begin() ; it != timers.end(); it++ )
        {
            //当整个进程都已经退出时，TimerManager可能已经析构
            pmut->KillTimer(*it);
        }
        timers.clear();
    };
    
    virtual void DoTimer(const TimerMessage&) = 0;
    virtual TIMER_ID GetTimerId() { return INVALID_TIMER_ID; };
    virtual uintptr GetTimerArg() { return 0; };

    const string& name()
    {
        return _name;
    };

    void TimeoutHook(TIMER_ID tid,uint32 msgid,uint32 type,uint32 flow,uintptr arg,const string& str_arg)
    {
        TimerMessage current;
        current.tid = tid;
        current.msgid = msgid;
        current.type = type;
        current.flow = flow;
        current.arg = arg;
        current.str_arg = str_arg;
        DoTimer(current);
    };

    bool ValidateTimer(TimerMessage *message)
    {
        TimerManager *pmut = TimerManager::GetInstance();
        if(!pmut)
            return false;
            
        uint32 current_flow = pmut->GetFlowNumber(message->tid);
        return (current_flow == message->flow);
    };
    
    STATUS GetRemainedTime(TIMER_ID tid,T_TimeValue *remained)
    {
        if(!remained)
        {
            SkyAssert(0);
            return ERROR_INVALID_ARGUMENT;
        }
        
        TimerManager *pmut = TimerManager::GetInstance();
        if(!pmut)
            return false;
            
        return pmut->GetRemainedTime(tid,remained);
    };

    uint32 GetTimerCount()
    {           
        return timers.size();
    };
    
    TIMER_ID CreateTimer()
    {
        TimerManager *pmut = TimerManager::GetInstance();
        if(!pmut) 
        {
            syslog(LOG_CRIT,"sky:Timer is null pid = %d in TimerClient CreateTimer!\n", (pid_t)syscall(__NR_gettid));
            SkyAssert(false);
            return INVALID_TIMER_ID;
        }
            
        TIMER_ID tid = pmut->CreateTimer(this);
        if(INVALID_TIMER_ID != tid)
        {
            timers.push_back(tid);
        }
        return tid;
    };
    
    STATUS SetTimer(TIMER_ID tid,const TimeOut& timeout)
    {
        TimerManager *pmut = TimerManager::GetInstance();
        if(!pmut)
        {
            syslog(LOG_CRIT,"sky:Timer is null pid = %d in TimerClient SetTimer!\n", (pid_t)syscall(__NR_gettid));
            SkyAssert(false);
            return ERROR_NOT_READY;
        }

        if (!IsMyTimerId(tid))
        {
            return ERROR_PERMISSION_DENIED;
        }
        return pmut->SetTimer(tid,timeout);
    };

    STATUS LoopTimer(TIMER_ID tid)
    {
        TimerManager *pmut = TimerManager::GetInstance();
        if(!pmut)
        {
            syslog(LOG_CRIT,"sky:Timer is null pid = %d in TimerClient LoopTimer!\n", (pid_t)syscall(__NR_gettid));
            SkyAssert(false);
            return ERROR_NOT_READY;
        }
        if (!IsMyTimerId(tid))
        {
            return ERROR_PERMISSION_DENIED;
        }     
        return pmut->LoopTimer(tid);
    };
    
    STATUS StopTimer(TIMER_ID tid)
    {
        TimerManager *pmut = TimerManager::GetInstance();
        if(!pmut)
        {
            syslog(LOG_CRIT,"sky:Timer is null pid = %d in TimerClient StopTimer!\n", (pid_t)syscall(__NR_gettid));
            SkyAssert(false);
            return ERROR_NOT_READY;
        }
        if (!IsMyTimerId(tid))
        {
            return ERROR_PERMISSION_DENIED;
        }
        return pmut->StopTimer(tid);
    };
    
    STATUS KillTimer(TIMER_ID tid)
    {
        TimerManager *pmut = TimerManager::GetInstance();
        if(!pmut)
        {
            syslog(LOG_CRIT,"sky:Timer is null pid = %d in TimerClient KillTimer!\n", (pid_t)syscall(__NR_gettid));
            SkyAssert(false);
            return ERROR_NOT_READY;
        }
        if (!IsMyTimerId(tid))
        {
            return ERROR_PERMISSION_DENIED;
        }

        timers.remove(tid);
        return pmut->KillTimer(tid);
    };

    void TraceWaite(TIMER_ID tid,const TimerMessage *tm)
    {
        TimerManager *pmut = TimerManager::GetInstance();
        if(!pmut)
        {
            syslog(LOG_CRIT,"sky:Timer is null pid = %d in TimerClient KillTimer!\n", (pid_t)syscall(__NR_gettid));
            SkyAssert(false);
            return;
        }
        pmut->TraceWaite(tid,tm);
    };

private:
    bool IsMyTimerId(TIMER_ID tid)
    {
        list<TIMER_ID>::iterator result;
        result = find( timers.begin(), timers.end(), tid );        
        if( result == timers.end() ) 
        {             
            syslog(LOG_CRIT,"sky:Timer id %d is illegal tid!\n",tid);
            SkyAssert(false);
            return false;
        }
        return true;
    }
    string _name;
    list <TIMER_ID> timers;
};

STATUS TimerInit(const TimerConfig& config);
STATUS GetTimeFromPowerOn(T_TimeValue* ptTime);
void DestroyTimerManager();
#endif


