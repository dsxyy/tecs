/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：message_unit.cpp
* 文件标识：见配置管理计划书
* 内容摘要：定时器管理模块实现文件
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
#include "timer.h"
#include "datetime.h"
#include "clock.h"


#define SIGTICK             39      //定时器tick实时信号值
#define TICK_DURTAION       100     //定时器精度=100毫秒
#define ONCE_ALLOC_COUNT    32      //每次批量分配的定时器控制块数量

#define TRACE(ptTMCB)        Trace(ptTMCB,__FUNCTION__,__LINE__,NULL)
#define TRACE_REMOVE(ptTMCB) Trace(ptTMCB,"remove",__LINE__,NULL)
#define TRACE_CREATE(ptTMCB) Trace(ptTMCB,"create",__LINE__,NULL)
#define TRACE_SET(ptTMCB)    Trace(ptTMCB,"set",__LINE__,NULL)
#define TRACE_LOOP(ptTMCB)   Trace(ptTMCB,"loop",__LINE__,NULL)
#define TRACE_STOP(ptTMCB)   Trace(ptTMCB,"stop",__LINE__,NULL)
#define TRACE_KILL(ptTMCB)   Trace(ptTMCB,"kill",__LINE__,NULL)
#define TRACE_SCAN(ptTMCB)   Trace(ptTMCB,"scan",__LINE__,NULL)
#define TRACE_WAITE(ptTMCB,ptTM)   Trace(ptTMCB,"wait",__LINE__,ptTM)


static int timer_print_on = 0;  
DEFINE_DEBUG_VAR(timer_print_on);
#define Debug(fmt,arg...) \
        do \
        { \
            if(timer_print_on) \
            { \
                printf("[%s:%d]"fmt,__func__,__LINE__, ##arg); \
            } \
        }while(0)

class SignalMask
{
public:
    SignalMask()
    {
        sigset_t waitset;
        sigemptyset(&waitset);
        sigaddset(&waitset, SIGTICK);
        sigprocmask(SIG_BLOCK, &waitset, NULL);
    };
};

//利用全局对象在main函数执行之前就被构造的特点
//在创建任何线程之前先屏蔽定时器信号
static SignalMask signal_mask;
static T_TimeValue s_tPoweronMonoTime;
static struct tm s_tPoweronLocalTime;

class TimerTrace
{
public:
    TimerTrace():
    _create_time(time(0))
    {
        _client_id = INVALID_TIMER_ID;
        _set_num = 0;
        _loop_num = 0;
        _stop_num = 0;     
        _kill_num = 0;
        _remove_num = 0;
        _scan_snd_num = 0;
        _waite_rcv_num = 0;      
        _set_time_span.Begin();
        _loop_time_span.Begin();
        _stop_time_span.Begin();       
        _kill_time_span.Begin();
        _remove_time_span.Begin(); 
        _scan_snd_time_span.Begin();
        _waite_rcv_time_span.Begin();

    };
    TimerTrace(pid_t client_id,const string &name):
    _create_time(time(0))
    {
        _client_id = client_id;
        _name = name;
        _set_num = 0;
        _loop_num = 0;
        _stop_num = 0;     
        _kill_num = 0;
        _remove_num = 0;
        _scan_snd_num = 0;
        _waite_rcv_num = 0;  
        _set_time_span.Begin();
        _loop_time_span.Begin();
        _stop_time_span.Begin();       
        _kill_time_span.Begin();
        _remove_time_span.Begin();        
        _scan_snd_time_span.Begin();
        _waite_rcv_time_span.Begin();
    };
    string   _name;
    pid_t    _client_id;
    Datetime _create_time;
    Timespan _set_time_span;
    Timespan _loop_time_span;
    Timespan _stop_time_span;
    Timespan _kill_time_span;   
    Timespan _remove_time_span;
    Timespan _scan_snd_time_span;
    Timespan _waite_rcv_time_span;  
    uint32   _set_num;
    uint32   _loop_num;
    uint32   _stop_num;
    uint32   _kill_num;
    uint32   _remove_num;
    uint32   _scan_snd_num;
    uint32   _waite_rcv_num;
    list<TimerMessage> _last_scan_snd_msgs;
    list<TimerMessage> _last_waite_rcv_msgs;
};

class TimerBlock
{
public:
    TimerBlock()
    {
        timer_id = INVALID_TIMER_ID;
        msgid = 0;
        abstime = 0;
        timer_duration = 0;
        timer_type = RELATIVE_TIMER;
        timer_arg = 0;
        flow_number = 0;
        timing = false;
        traced = false;
        client = NULL;
    };
    
    TIMER_ID timer_id;
    uint32   msgid;     
    uint32   timer_type;
    uint32   timer_duration;     
    uintptr  timer_arg;
    string   timer_str_arg;
    time_t   abstime;
    T_TimeValue   timeout;
    TimerClient *client;
    uint32   flow_number;
    bool     timing;  //是否正在计时中
    bool     traced;
    pid_t    client_tid;
    list<TimerTrace> _traces;
};

//重载T_TimeKey的<操作符，用于扫描map
bool operator <(const T_TimeKey& one, const T_TimeKey& other)
{
    if(one.second != other.second)
        return (one.second < other.second);
    else if(one.milisecond != other.milisecond)
        return (one.milisecond < other.milisecond);
    else 
        return (one.tid < other.tid);
}

static void GetMonoTime(T_TimeValue* now)
{
    struct timespec tMonoTime;
    clock_gettime(CLOCK_MONOTONIC,&tMonoTime);
    now->second = tMonoTime.tv_sec;
    now->milisecond = tMonoTime.tv_nsec / (1000*1000);
}

static void Trace(TimerBlock *ptTMCB,const char* op,int lineno,const TimerMessage *tm)
{

    if (ptTMCB->_traces.size() > 10)
    {
        ptTMCB->_traces.pop_front();
    }

    string op_string=op;
    if (op_string == "create")
    {
        TimerTrace ttrace(ptTMCB->client_tid,ptTMCB->client->name());
        ptTMCB->_traces.push_back(ttrace);
    }
    else if (op_string == "set")
    {
        ptTMCB->_traces.back()._set_time_span.End();
        ptTMCB->_traces.back()._set_num++;
    }
    else if (op_string == "loop")
    {
        ptTMCB->_traces.back()._loop_time_span.End();
        ptTMCB->_traces.back()._loop_num++;    
    }
    else if (op_string == "stop")
    {
        ptTMCB->_traces.back()._stop_time_span.End();
        ptTMCB->_traces.back()._stop_num++;
    }
    else if (op_string == "kill")
    {
        ptTMCB->_traces.back()._kill_time_span.End();
        ptTMCB->_traces.back()._kill_num++;
    }
    else if (op_string == "remove")
    {
        ptTMCB->_traces.back()._remove_time_span.End();
        ptTMCB->_traces.back()._remove_num++;
    }
    else if (op_string == "scan")
    {
        ptTMCB->_traces.back()._scan_snd_time_span.End();
        ptTMCB->_traces.back()._scan_snd_num++;
        if (ptTMCB->timer_type == 2)
        {
            if (ptTMCB->_traces.back()._last_scan_snd_msgs.size() > 2)
            {
                ptTMCB->_traces.back()._last_scan_snd_msgs.pop_front();
            }

            TimerMessage tmsg;
            tmsg.tid = ptTMCB->timer_id;
            tmsg.msgid = ptTMCB->msgid;
            tmsg.type = ptTMCB->timer_type;
            tmsg.flow = ptTMCB->flow_number;
            tmsg.arg = ptTMCB->timer_arg;
            tmsg.str_arg = ptTMCB->timer_str_arg;
            ptTMCB->_traces.back()._last_scan_snd_msgs.push_back(tmsg);
        }

    }
    else if (op_string == "wait")
    {
        ptTMCB->_traces.back()._waite_rcv_time_span.End();
        ptTMCB->_traces.back()._waite_rcv_num++;
        if (ptTMCB->timer_type == 2)
        {
            if (ptTMCB->_traces.back()._last_waite_rcv_msgs.size() > 2)
            {
                ptTMCB->_traces.back()._last_waite_rcv_msgs.pop_front();
            }
            ptTMCB->_traces.back()._last_waite_rcv_msgs.push_back(*tm);
    }

    }
    if(!ptTMCB->traced)
        return;

    T_TimeValue now;
    GetMonoTime(&now);
    printf("Operation %s(line %d) on timer %d (owner: %s) at mono time %d.%d.\n",
            op,
            lineno,
            ptTMCB->timer_id,
            ptTMCB->client->name().c_str(),
            now.second,
            now.milisecond);
}

extern "C" void *TimerScanTaskEntry(void *arg)
{
    timer_t tPosixTimerId = 0;
    struct sigevent sigev;
    struct itimerspec timeout;
    int sig;
    sigset_t mask;
    TimerManager *p = NULL;
    
    if (!arg)
    {
        //todo: 记录日志
        printf("TimerScanTaskEntry failed to get arg!\n");
        SkyExit(-1, "TimerScanTaskEntry failed to get arg.");
        return 0;
    }

    //创建posix定时器，由内核周期性发送信号给定时器扫描任务
    sigev.sigev_signo = SIGTICK;
    sigev.sigev_notify = SIGEV_SIGNAL;
    sigev.sigev_notify_function = NULL;
    sigev.sigev_notify_attributes = NULL;
    if (0 != timer_create(CLOCK_MONOTONIC, &sigev, &tPosixTimerId))
    {
        //todo: 记录日志
        printf("timer_create failed! errno=%d\n",errno);
        SkyExit(-1, "TimerScanTaskEntry: call timer_create(CLOCK_MONOTONIC...) failed.");
        return 0;
    }
    p = static_cast<TimerManager *>(arg);
    TimerConfig time;
    p->GetTimerConfig(&time);
    timeout.it_interval.tv_sec = time.tv_msec/1000;
    timeout.it_interval.tv_nsec = (time.tv_msec%1000)*1000*1000;
    timeout.it_value = timeout.it_interval;
    if (0 != timer_settime(tPosixTimerId, 0, &timeout, NULL))
    {
        //todo: 记录日志
        printf("timer_settime failed! errno=%d\n",errno);
        SkyExit(-1, "TimerScanTaskEntry: call timer_settime(tPosixTimerId...) failed.");
        return 0;
    }

    //开始等待周期性信号的到来并扫描所有定时器
    while(1)
    {
        sigemptyset(&mask);
        sigaddset(&mask,SIGTICK);
        sigwait(&mask,&sig);
        p->Scan();
    }
    return 0;
}

TimerManager* TimerManager::instance = NULL;
STATUS TimerManager::Start(int priority)
{
    SkyAssert(NULL != instance);
    if(_workthread_id != INVALID_THREAD_ID)
        return ERROR_DUPLICATED_OP;
    
    if(SUCCESS != AllocateTimerBlock())
    {
        Debug("AllocateTimerBlock failed!");
        return ERROR;
    }
    
    //启动定时器扫描任务
    _workthread_id = StartThread("TimerManager",TimerScanTaskEntry,(void *)this);
    if(INVALID_THREAD_ID == _workthread_id)
        return ERROR;
    else
        return SUCCESS;
}

STATUS TimerManager::AllocateTimerBlock()
{
    //每次分配一批固定数量的定时器控制块
    TimerBlock *ptTMCB = new TimerBlock[ONCE_ALLOC_COUNT];
    if (NULL == ptTMCB)
    {
        Debug("new TimerBlock failed!");
        return ERROR_RESOURCE_UNAVAILABLE;
    }    
    for(int i=0;i<ONCE_ALLOC_COUNT;i++)
    {
        //将新分配的定时器控制块逐个加入pool vector
        _pool.push_back(ptTMCB);
        //在vector中的下标就是定时器控制块的id
        ptTMCB->timer_id = (TIMER_ID)(_pool.size());
        //同时也放入空闲定时器控制块列表
        _freelist.push_back(ptTMCB->timer_id);
        //cout << "AllocateTimerBlock add a tmcb, id = " << ptTMCB->timer_id << endl;
        ptTMCB++;
    }
    
    return SUCCESS;
}

STATUS TimerManager::InsertTimer(TimerBlock *ptTMCB)
{
    //锁定扫描表
    MutexLock lock(rel_scan_table_mutex); 
    //将定时器控制块放入扫描表
    T_TimeKey key(ptTMCB->timeout,ptTMCB->timer_id);
    std::pair< std::map<T_TimeKey,TIMER_ID>::iterator,bool > ret;
    ret=rel_scan_table.insert(make_pair(key,ptTMCB->timer_id));
    if( ret.second )
    {
        //syslog(LOG_CRIT,"zyb:rel_scan_table.insert success timeid %d pid = %d!\n",
            //ptTMCB->timer_id,getpid());
    }
    else
    {
        syslog(LOG_CRIT,"sky:rel_scan_table.insert fail timeid %d pid = %d !\n",
            ptTMCB->timer_id,(pid_t)syscall(__NR_gettid));
    }

    ptTMCB->timing = true;
    TRACE(ptTMCB);
    Debug("insert timer %d at mono time: %d.%d\n",ptTMCB->timer_id,key.second,key.milisecond);
    return SUCCESS;
}

STATUS TimerManager::RemoveTimer(TimerBlock *ptTMCB)
{
    //锁定扫描表
    MutexLock lock(rel_scan_table_mutex);
    if(ptTMCB->timing == false)
    {
        TRACE(ptTMCB);
        return SUCCESS;
    }
    
    //首先查找控制块
    map<T_TimeKey,TIMER_ID>::iterator it;
    T_TimeKey key(ptTMCB->timeout,ptTMCB->timer_id);
    
    it = rel_scan_table.find(key);
    if(it == rel_scan_table.end())
    {
        TRACE(ptTMCB);
        return ERROR_OBJECT_NOT_EXIST;
    }
    
    //移除本定时器控制块
    rel_scan_table.erase(it);
    ptTMCB->timing = false;
    ptTMCB->flow_number ++;
    Debug("remove timer %d at mono time: %d.%d\n",ptTMCB->timer_id,key.second,key.milisecond);
    TRACE_REMOVE(ptTMCB);
    return SUCCESS;
}

void TimerManager::Scan()
{
    //对定时器表加锁
    MutexLock lock (rel_scan_table_mutex);

    //如果定时器扫描表是空的，直接返回
    if (rel_scan_table.empty())
    {
        return;
    }

    //获取当前系统mono时间
    T_TimeValue now;
    GetMonoTime(&now);
    //tid字段设置为最大值，使得本时刻超时的定时器key都比nowkey小
    T_TimeKey nowkey(now,uint32(-1));

    map<T_TimeKey,TIMER_ID>::iterator it = rel_scan_table.begin();
    while (it != rel_scan_table.end())
    {
        if (nowkey < it->first)
        {
            break;
        }

        Debug("Scanner finds timers at mono time: %d.%d!\n",now.second,now.milisecond);
        TimerBlock *ptTMCB = ID_TO_POINTER(it->second);
        if(!ptTMCB)
        {
            syslog(LOG_CRIT,"Scanner can not find timer block of tid %d!\n",it->second);
            SkyAssert(false);
            SkyExit(-1, "Timer Scanner ot find timer block of tid!");
            continue;
        }

        ptTMCB->timing = false;
        //调用其超时函数钩子
        try
        {
            TRACE_SCAN(ptTMCB);
            Debug("Scanner will call hook of timer %d.\n",ptTMCB->timer_id);
            ptTMCB->client->TimeoutHook(ptTMCB->timer_id,
                                        ptTMCB->msgid,
                                        ptTMCB->timer_type,
                                        ptTMCB->flow_number,
                                        ptTMCB->timer_arg,
                                        ptTMCB->timer_str_arg);
        }
        catch (...)
        {
            TRACE(ptTMCB);
            SkyAssert(false);
            rel_scan_table.erase(it++);
            continue;
        }
        
        //将已经超时的定时器删除
        Debug("Scanner will erase timer %d.\n",ptTMCB->timer_id);
        rel_scan_table.erase(it++);
    }
}

TIMER_ID TimerManager::CreateTimer(TimerClient *client)
{
    if(!instance)
        return ERROR_NOT_READY;
    
    //当空闲队列中没有控制块可用时，重新分配一批
    MutexLock lock(free_list_mutex);
    while(true == _freelist.empty())
    {
        if(SUCCESS != AllocateTimerBlock())
        {
            Debug("AllocateTimerBlock failed!");
            SkyAssert(false);
            return INVALID_TIMER_ID;
        }
    }

    //从空闲队列中取出一个控制块
    TIMER_ID tid = _freelist.front();
    _freelist.pop_front();
    TimerBlock *ptTMCB = ID_TO_POINTER(tid);
    if(!ptTMCB)
    {
        SkyAssert(false);
        return INVALID_TIMER_ID;
    }
    

    ptTMCB->client = client;
    ptTMCB->client_tid = (pid_t)syscall(__NR_gettid);
    TRACE_CREATE(ptTMCB);
    return tid;
}

STATUS TimerManager::SetTimer(TIMER_ID tid,const TimeOut& timeout)
{
    if(!instance)
    {        
        syslog(LOG_CRIT,"sky:instance is null pid = %d in SetTimer!\n", (pid_t)syscall(__NR_gettid));
        SkyAssert(false);
        return ERROR_NOT_READY;
    }
    //验证参数是否合法
    if(!timeout.Validate())
    {
        syslog(LOG_CRIT,"sky:paramater is error %d: %d in SetTimer!\n", (pid_t)syscall(__NR_gettid),timeout.type);
        SkyAssert(false);
        return ERROR_INVALID_ARGUMENT;
    }


    //从id得到控制块地址
    TimerBlock *ptTMCB = ID_TO_POINTER(tid);
    if(!ptTMCB || ptTMCB->client == NULL)
    {
        syslog(LOG_CRIT,"sky:ptTMCB is error pid = %d in SetTimer!\n", (pid_t)syscall(__NR_gettid));
        SkyAssert(false);
        return ERROR_INVALID_ARGUMENT;
    }

    MutexLock lock(rel_scan_table_mutex);
    //如果定时器已经设置过，先将其停止
    if(true == ptTMCB->timing)
    {
        if(SUCCESS != RemoveTimer(ptTMCB))
        {
            SkyAssert(false);
            return ERROR;
        }
    }
 
    //重新设置控制块    
    ptTMCB->msgid = timeout.msgid;
    ptTMCB->timer_type = timeout.type;
    ptTMCB->timer_duration = timeout.duration;
    ptTMCB->timer_arg = timeout.arg;
    ptTMCB->timer_str_arg = timeout.str_arg;
    T_TimeValue now;
    GetMonoTime(&now);
    now.milisecond += ptTMCB->timer_duration;
    if (now.milisecond > 1000)
    {
        now.second += now.milisecond/1000;
        now.milisecond = now.milisecond%1000;
    }
    //超时时刻等于当前mono时刻加上超时间隔
    ptTMCB->timeout = now;
    ptTMCB->flow_number++;
    TRACE_SET(ptTMCB);
    //将重新设置完毕的定时器控制块放入扫描表
    return InsertTimer(ptTMCB);
}

STATUS TimerManager::LoopTimer(TIMER_ID tid)
{
    if(!instance)
    {
        syslog(LOG_CRIT,"sky:instance is null pid = %d in LoopTimer!\n", (pid_t)syscall(__NR_gettid));
        SkyAssert(false);
        return ERROR_NOT_READY;
    }
    
    //从id得到控制块地址
    TimerBlock *ptTMCB = ID_TO_POINTER(tid);
    if(!ptTMCB || ptTMCB->client == NULL)
    {
        syslog(LOG_CRIT,"sky:instance is null timing %d pid = %d in LoopTimer!\n",
            ptTMCB->timing,(pid_t)syscall(__NR_gettid));
        SkyAssert(false);
        return ERROR_INVALID_ARGUMENT;
    }
  
    MutexLock lock(rel_scan_table_mutex);
	//如果定时器已经设置过，先将其停止，由于有的应用会多线程操作同一定时器
    if(true == ptTMCB->timing)
    {
        if(SUCCESS != RemoveTimer(ptTMCB))
        {
            SkyAssert(false);
            return ERROR;
        }
    }
	
    //在原有的mono时间上加上超时间隔，得到下次超时的mono时间
    ptTMCB->timeout.milisecond += ptTMCB->timer_duration;
    if (ptTMCB->timeout.milisecond > 1000)
    {
        ptTMCB->timeout.second += ptTMCB->timeout.milisecond/1000;
        ptTMCB->timeout.milisecond = ptTMCB->timeout.milisecond%1000;
    }
    
    if(SUCCESS != InsertTimer(ptTMCB))
    {
        syslog(LOG_CRIT,"sky:InsertTimer fail pid = %d in LoopTimer!\n",(pid_t)syscall(__NR_gettid));
        SkyAssert(false);
        return ERROR;
    }
    
    ptTMCB->flow_number++;
    TRACE_LOOP(ptTMCB);
    return SUCCESS;
}

STATUS TimerManager::StopTimer(TIMER_ID tid)
{
    if(!instance)
    {
        syslog(LOG_CRIT,"sky:instance is null pid = %d in StopTimer!\n", (pid_t)syscall(__NR_gettid));
        SkyAssert(false);
        return ERROR_NOT_READY;
    }
    
    TimerBlock *ptTMCB = ID_TO_POINTER(tid);
    if(!ptTMCB || ptTMCB->client == NULL)
    {
        syslog(LOG_CRIT,"sky:ptTMCB is error pid = %d in StopTimer!\n", (pid_t)syscall(__NR_gettid));
        SkyAssert(false);
        return ERROR_INVALID_ARGUMENT;
    }
    
	MutexLock lock(rel_scan_table_mutex);    
    // 如果定时器没有START，执行STOP，也返回成功
    if (ptTMCB->timing == false)
    {
        return SUCCESS;
    }

    if(SUCCESS != RemoveTimer(ptTMCB))
    {
        SkyAssert(false);
        return ERROR;
    }  
    
    ptTMCB->flow_number++;
    TRACE_STOP(ptTMCB);
    return SUCCESS;
}

STATUS TimerManager::KillTimer(TIMER_ID tid)
{
    if(!instance)
    {
        syslog(LOG_CRIT,"sky:instance is null pid = %d in KillTimer!\n", (pid_t)syscall(__NR_gettid));
        SkyAssert(false);
        return ERROR_NOT_READY;
    }
    
    TimerBlock *ptTMCB = ID_TO_POINTER(tid);
    if(!ptTMCB || ptTMCB->client == NULL)
    {
        syslog(LOG_CRIT,"sky:ptTMCB is error pid = %d in KillTimer!\n", (pid_t)syscall(__NR_gettid));
		SkyAssert(false);
        return ERROR_INVALID_ARGUMENT;
    }

    if(true == ptTMCB->timing)
    {
        //先将定时器停止，即从扫描表中拿出
        if(SUCCESS != RemoveTimer(ptTMCB))
        {
            SkyAssert(false);
            return ERROR;
        }
    }

    MutexLock lock(free_list_mutex);
    //释放定时器控制块到空闲列表中
    _freelist.push_back(tid);
    ptTMCB->client = NULL;
    ptTMCB->flow_number ++;
    TRACE_KILL(ptTMCB);
    return SUCCESS;
}

uint32 TimerManager::GetFlowNumber(TIMER_ID tid)
{
    if(!instance)
    {
        syslog(LOG_CRIT,"sky:instance is null pid = %d in GetFlowNumber!\n", (pid_t)syscall(__NR_gettid));
		SkyAssert(false);
        return 0;
    }
    
    TimerBlock *ptTMCB = ID_TO_POINTER(tid);
    if(!ptTMCB || ptTMCB->client == NULL)
    {
        //SkyAssert(false);
        syslog(LOG_CRIT,"sky:ptTMCB is error pid = %d in GetFlowNumber!\n", (pid_t)syscall(__NR_gettid));
        return 0;
    }
    
    //获取定时器流水号
    TRACE(ptTMCB);
    return ptTMCB->flow_number;
}

STATUS TimerManager::GetRemainedTime(TIMER_ID tid,T_TimeValue *remained)
{
    if(!instance)
    {
        syslog(LOG_CRIT,"sky:instance is null pid = %d in GetRemainedTime!\n", (pid_t)syscall(__NR_gettid));
        SkyAssert(false);
        return ERROR_NOT_READY;
    }
    
    TimerBlock *ptTMCB = ID_TO_POINTER(tid);
    if(!ptTMCB || ptTMCB->client == NULL)
    {
        syslog(LOG_CRIT,"sky:ptTMCB is error pid = %d in GetRemainedTime!\n", (pid_t)syscall(__NR_gettid));
	SkyAssert(false);
        return ERROR_OBJECT_NOT_EXIST;
    }

    T_TimeValue timeout;
    timeout.second = ptTMCB->timeout.second;
    timeout.milisecond = ptTMCB->timeout.milisecond;

    T_TimeValue now;
    GetMonoTime(&now);

    remained->second = timeout.second - now.second;
    if(timeout.milisecond < now.milisecond)
    {
        remained->second -= 1;
        timeout.milisecond += 1000;
    }
    remained->milisecond = timeout.milisecond - now.milisecond;
    return SUCCESS;
}

void TimerManager::ShowAllTimers(int print_scan_table_only)
{
    if(!instance)
    {
        SkyAssert(false);
        return;
    }
    
    MutexLock lock(rel_scan_table_mutex);
    map<T_TimeKey,TIMER_ID>::iterator it;
    if(rel_scan_table.empty())
    {
        cout << "No timers." << endl;
        return;
    }

    T_TimeValue now;
    GetMonoTime(&now);
    cout << "Current Mono time: " << now.second << "," << now.milisecond << endl;
    cout << "==========Active Timer List==========" << endl;
    for ( it=rel_scan_table.begin() ; it != rel_scan_table.end(); it++ )
    {
        cout << "Timer " << it->first.tid << " at mono time: [" << it->first.second << "," << it->first.milisecond << "]"<< endl;
        TimerBlock *ptTMCB = ID_TO_POINTER(it->second);
        if(!ptTMCB)
        {
            printf("can not find timer block of tid %d!\n",it->second);
            SkyAssert(false);
            continue;
        }
        cout << "Id = " << ptTMCB->timer_id << ", Owner = " << ptTMCB->client->name() << endl;
        cout << "Type  = " << ptTMCB->timer_type << ", Dur   = " << ptTMCB->timer_duration << endl;
        cout << "MsgId = " << ptTMCB->msgid      << ", flow  = " << ptTMCB->flow_number << endl;
    }
    if (print_scan_table_only)
    {
        return;
    }
    vector<TimerBlock*>::iterator pool_it;
    cout << "pool num = " << _pool.size()<< endl;
    for ( pool_it=_pool.begin() ; pool_it != _pool.end(); pool_it++ )
    {

        TimerBlock *ptTMCB=*pool_it;
        cout << "Timer id = "<< ptTMCB->timer_id << ",total client num = " << ptTMCB->_traces.size();
        cout << ",msgid = "<< ptTMCB->msgid << ",type = " << ptTMCB->timer_type;
        if (ptTMCB->client)
        {
            cout << ",current client = " << ptTMCB->client->name() << endl;
        }
        else
        {
            cout << endl;
        }

        list<TimerTrace>::iterator lit=ptTMCB->_traces.begin();
        for ( ; lit != ptTMCB->_traces.end(); lit++ )
        {
            cout << " client name:    " << (*lit)._name << endl;
            cout << " client id:      " << (*lit)._client_id << endl;
            cout << " create time:    " << (*lit)._create_time.tostr().c_str() << endl;
            cout << " set at time:    " << (*lit)._set_time_span.GetSpan() << "(usecond)"<< endl;
            cout << " loop at time:   " << (*lit)._loop_time_span.GetSpan() << "(usecond)"<< endl;
            cout << " stop at time:   " << (*lit)._stop_time_span.GetSpan() << "(usecond)"<< endl;
            cout << " remove at time:   " << (*lit)._remove_time_span.GetSpan() << "(usecond)"<< endl;
            cout << " kill at time:   " << (*lit)._kill_time_span.GetSpan() << "(usecond)"<< endl;
            cout << " scan at time:     " << (*lit)._scan_snd_time_span.GetSpan() << "(usecond)"<< endl;
            cout << " recv at time:     " << (*lit)._waite_rcv_time_span.GetSpan() << "(usecond)"<< endl;
            cout << " set number:     " << (*lit)._set_num << endl;
            cout << " loop number:    " << (*lit)._loop_num << endl;
            cout << " stop number:    " << (*lit)._stop_num << endl;            
            cout << " kill number:      " << (*lit)._kill_num << endl;
            cout << " remove number:    " << (*lit)._remove_num << endl;
            cout << " scan number:      " << (*lit)._scan_snd_num << endl;
            cout << " recv number:      " << (*lit)._waite_rcv_num << endl;
            list<TimerMessage>::iterator mit=(*lit)._last_scan_snd_msgs.begin();
            for ( ; mit != (*lit)._last_scan_snd_msgs.end(); mit++ )
            {
                cout << " last scan tid:    " << (*mit).tid << endl;
                cout << " last scan msgid:  " << (*mit).msgid << endl;
                cout << " last scan type:   " << (*mit).type << endl;
                cout << " last scan flow:   " << (*mit).flow << endl;
                cout << " last scan arg:    " << (*mit).arg << endl << endl;
            }
            mit=(*lit)._last_waite_rcv_msgs.begin();
            for ( ; mit != (*lit)._last_waite_rcv_msgs.end(); mit++ )
            {
                cout << " last rcvd tid:    " << (*mit).tid << endl;
                cout << " last rcvd msgid:  " << (*mit).msgid << endl;
                cout << " last rcvd type:   " << (*mit).type << endl;
                cout << " last rcvd flow:   " << (*mit).flow << endl;
                cout << " last rcvd arg:    " << (*mit).arg << endl << endl;;
            }
        }
    }
    MutexLock free_lock(free_list_mutex);
    deque<TIMER_ID>::iterator freelist_it=_freelist.begin();
    cout << "free list num = " << _freelist.size()<< ",Id=";
    for ( freelist_it=_freelist.begin() ; freelist_it != _freelist.end(); freelist_it++ )
    {
        TimerBlock *ptTMCB = ID_TO_POINTER(*freelist_it);
        cout << " " << ptTMCB->timer_id;
    }
    cout << endl;

    return;
}

void TimerManager::TraceTimer(TIMER_ID tid)
{  
    if(!instance)
        return;
    
    TimerBlock *ptTMCB = ID_TO_POINTER(tid);
    if(ptTMCB)
        ptTMCB->traced = true;
    else
        printf("Invalid tid %d!\n",tid);
}

void TimerManager::UnTraceTimer(TIMER_ID tid)
{
    if(!instance)
        return;
    
    TimerBlock *ptTMCB = ID_TO_POINTER(tid);
    if(ptTMCB)
        ptTMCB->traced = false;
    else
        printf("Invalid tid %d!\n",tid);
}

void TimerManager::TraceWaite(TIMER_ID tid,const TimerMessage *tm)
{
    if(!instance)
    {
        syslog(LOG_CRIT,"sky:instance is null pid = %d in TraceWaite!\n", (pid_t)syscall(__NR_gettid));
	SkyAssert(false);
        return;
    }

    TimerBlock *ptTMCB = ID_TO_POINTER(tid);
    if(!ptTMCB || ptTMCB->client == NULL)
    {
        //SkyAssert(false);
        syslog(LOG_CRIT,"sky:ptTMCB is error pid = %d in TraceWaite!\n", (pid_t)syscall(__NR_gettid));
        return;
    }

    TRACE_WAITE(ptTMCB,tm);
}

STATUS TimerManager::SetTimerConfig(const TimerConfig& config)
{
    if(!instance)
        return ERROR_NOT_READY;

    if(config.tv_msec < 10)
    {
        return ERROR_INVALID_ARGUMENT;
    }
    
    _config = config;

    return SUCCESS;
}

STATUS TimerManager::GetTimerConfig(TimerConfig *ptimercfg)
{
    if(!instance)
    {
        printf("Timer manager instance is null!");
        return ERROR_NOT_READY;
    }

    *ptimercfg = _config;
    return SUCCESS;
}

STATUS TimerInit(const TimerConfig& config)
{
    STATUS ret = ERROR;
    if(true != TimerManager::CreateInstance())
    {
        return ERROR_NO_MEMORY;
    }
    
    TimerManager *pmut = TimerManager::GetInstance();
    if(!pmut)
    {
        printf("TimerManager::GetInstance failed!\n");
        return ERROR_OBJECT_NOT_EXIST;
    }
    printf("timer scan period: %u in TimerInit\n",config.tv_msec);

    ret = pmut->SetTimerConfig(config);
    if(SUCCESS != ret)
    {
        printf("TimerManager SetTimerConfig failed! ret = %d\n",ret);
        return ret;
    }

    ret = pmut->Start();
    if(SUCCESS != ret)
    {
        printf("TimerManager start failed! ret = %d\n",ret);
        return ret;
    }
    
    time_t now = time(NULL);
    localtime_r(&now, &s_tPoweronLocalTime);
    GetMonoTime(&s_tPoweronMonoTime);
    
    return SUCCESS;
}  

STATUS GetTimeFromPowerOn(T_TimeValue* ptTime)
{
    if(!ptTime)
    {
        SkyAssert(false);
        return ERROR_INVALID_ARGUMENT;
    }
    
    T_TimeValue now;
    GetMonoTime(&now);
    ptTime->second = now.second - s_tPoweronMonoTime.second;
    if(now.milisecond < s_tPoweronMonoTime.milisecond)
    {
        ptTime->second -= 1;
        ptTime->milisecond = now.milisecond + 1000 - s_tPoweronMonoTime.milisecond;
    }
    else
    {
        ptTime->milisecond = now.milisecond - s_tPoweronMonoTime.milisecond;
    }
    return SUCCESS;
}

void DestroyTimerManager()
{
    TimerManager *pmut = TimerManager::GetInstance();
    if(!pmut)
        return;
    
    delete pmut;
}
    
void DbgShowUpTime()
{
    stringstream ss;
    ss << dec << s_tPoweronLocalTime.tm_year + 1900
       << "-" << dec << setfill('0') << setw(2) << s_tPoweronLocalTime.tm_mon + 1
       << "-" << dec << setfill('0') << setw(2) << s_tPoweronLocalTime.tm_mday
       << " " << dec << setfill('0') << setw(2) << s_tPoweronLocalTime.tm_hour
       << ":" << dec << setfill('0') << setw(2) << s_tPoweronLocalTime.tm_min
       << ":" << dec << setfill('0') << setw(2) << s_tPoweronLocalTime.tm_sec;
    
    T_TimeValue uptime;
    GetTimeFromPowerOn(&uptime);

    cout << "process is started from local time: " << ss.str() 
         << ", and has been running for " 
         << uptime.second << " seconds, " 
         << uptime.milisecond << " miliseconds." << endl;
    
    cout << "current version was built at " 
        << __DATE__ << "," << __TIME__ << "." << endl;
    return;
}
DEFINE_DEBUG_CMD(uptime,DbgShowUpTime);

void DbgShowAllTimers(int print_scan_table_only)
{
    TimerManager *pmut = TimerManager::GetInstance();
    if(!pmut)
        return;
    pmut->ShowAllTimers(print_scan_table_only);
}
DEFINE_DEBUG_CMD(timers,DbgShowAllTimers);

void DbgTraceTimer(TIMER_ID tid)
{
    TimerManager *pmut = TimerManager::GetInstance();
    if(!pmut)
        return;
    pmut->TraceTimer(tid);
}
DEFINE_DEBUG_FUNC(DbgTraceTimer);

void DbgUnTraceTimer(TIMER_ID tid)
{
    TimerManager *pmut = TimerManager::GetInstance();
    if(!pmut)
        return;
    pmut->UnTraceTimer(tid);
}
DEFINE_DEBUG_FUNC(DbgUnTraceTimer);

