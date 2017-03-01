/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：message_queue.cpp
* 文件标识：见配置管理计划书
* 内容摘要：消息队列实现文件
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
#include "message_queue.h"
#include "common.h"

static int mq_print_on = 0;  
DEFINE_DEBUG_VAR(mq_print_on);
#define Debug(fmt,arg...) \
        do \
        { \
            if(mq_print_on) \
            { \
                printf("[%s:%d]"fmt,__func__,__LINE__, ##arg); \
            } \
        }while(0)
        
#define CHECK_WORKTHREAD_STOP \
    if(INVALID_THREAD_ID != _workthread_id && _will_be_stopped) \
    { \
        return ERROR_THREAD_WILL_EXIT; \
    }
        
#define IF_WORKTHREAD_WILL_STOP \
    if(INVALID_THREAD_ID != _workthread_id && _will_be_stopped)     

extern   STATUS StartThreadDllMonitor(THREAD_ID workthread_id,const string &name);

extern STATUS EndThreadDllMonitor(THREAD_ID workthread_id); 
       
extern "C" void *mqWorkLoopTaskEntry(void *arg)
{
    STATUS ret = ERROR;
    MessageFrame message;
    MessageQueue *pmq = NULL;
    if ( arg == 0 )
    {
        //todo: 记录日志
        fprintf(stderr,"mqWorkLoopTaskEntry failed to get arg!\n");
        SkyExit(-1, "mqWorkLoopTaskEntry failed to get arg!");
        return 0;
    }
    pmq = static_cast<MessageQueue *>(arg);
    cout << pmq->name() <<" is now running!" << endl;
    pid_t tid = (pid_t)syscall(__NR_gettid);
    pmq->SaveTid(tid);
    while (1)
    {
        ret = pmq->Wait(&message,WAIT_FOREVER);
        if(ERROR_THREAD_WILL_EXIT == ret)
        {
            Debug("mqWorkLoopTaskEntry will exit!\n");
            return 0;
        }
        
        if(SUCCESS != ret)
        {
            continue;
        }
        
        /* 将消息交由消息处理器去处理 */
        MessageHandler *handler = pmq->GetHandler();
        if (handler)
        {
            Debug("Message %u will be processed by handler %s under state %d\n",
                message.option.id(),
                pmq->name().c_str(),
                pmq->get_state());
            pmq->set_handle_msg_state(S_Rcv_Msg);
            pmq->TraceMsgTime(message,true);
            StartThreadDllMonitor(pmq->get_task_id(),pmq->name());
            handler->MessageEntry(message);
            EndThreadDllMonitor(pmq->get_task_id());
            pmq->TraceMsgTime(message,false);
        }
    }
    return 0;
}

STATUS MessageQueue::Wait(MessageFrame *pm,uint32 timeout)
{
    //检测是否要求线程退出，如果为true，返回特定的错误码
    CHECK_WORKTHREAD_STOP;
    set_handle_msg_state(S_Wait_Msg);
    if(timeout != WAIT_FOREVER)
    {
        if(_wait_timer == INVALID_TIMER_ID)
        {
            _wait_timer = CreateTimer();
            if(_wait_timer == INVALID_TIMER_ID)
            {
                return ERROR_NO_TIMER;
            }
        }
        TimeOut to;
        to.duration = timeout;
        to.msgid = EV_SKY_SYNC_TIMER;
        SetTimer(_wait_timer,to);
    }
    
    pthread_mutex_lock(&_queue_mutex);
    while (_waiting_frames.empty() == true)
    {          
        IF_WORKTHREAD_WILL_STOP
        {
            //检测是否要求线程退出，如果为true，就释放锁并返回特定的错误码
            pthread_mutex_unlock(&_queue_mutex);
            return ERROR_THREAD_WILL_EXIT;
        }
        
        //pthread_cond_wait的内部流程是先释放mutex，然后等待cond变量的变化，
        //如果条件满足了，就返回，并且在返回之前重新对mutex进行加锁。
        pthread_cond_wait(&_queue_cond,&_queue_mutex);
        
        IF_WORKTHREAD_WILL_STOP
        {
            //检测是否要求线程退出，如果为true，就释放锁并返回特定的错误码
            pthread_mutex_unlock(&_queue_mutex);
            return ERROR_THREAD_WILL_EXIT;
        }
    }

    *pm = _waiting_frames.front();
    _waiting_frames.pop();
    _pop_count++;
    //消息已经取出，就释放保护_waiting_frames的锁，以供消息发送方继续push消息进来
    pthread_mutex_unlock(&_queue_mutex);
    
    CHECK_WORKTHREAD_STOP;
    
    //消息调试跟踪打印
    TracePrint(pm);
    
    //处理定时器消息
    if(mq_print_on)
    {
        if (pm->option.id() == 1502)
        {
            syslog(LOG_CRIT,"sky:Message type %d pid = %d!\n", pm->option.type(),(pid_t)syscall(__NR_gettid));
        }
    }
    if(mq_print_on==2)
    {
        printf("sky:msg type %d id %d\n",pm->option.type(),pm->option.id());
    }
    if(pm->option.type() == MESSAGE_TIMER)
    {
        Debug("A timer message!\n");
        TimerMessage tm;
        tm.deserialize(pm->data);
        TraceWaite(tm.tid,&tm);
        //定时器消息有效性验证
        if(ValidateTimer(&tm) == false)
        {
            syslog(LOG_CRIT,"sky: invalid timer flow pid = %d\n", (pid_t)syscall(__NR_gettid));
            TimerManager *pmut = TimerManager::GetInstance();
            if(pmut)
            {
                syslog(LOG_CRIT,"sky:%d'Message flow %d !=%d pid = %d!\n",
                     tm.tid,tm.flow,pmut->GetFlowNumber(tm.tid), (pid_t)syscall(__NR_gettid));
            }
            else
            {
                syslog(LOG_CRIT,"sky:Timer manager is null pid = %d!\n", (pid_t)syscall(__NR_gettid));
            }

            return ERROR_TIMER_MESSAGE_INVALID;
        }

        if(tm.type == LOOP_TIMER)
        {
            //如果是循环定时器，重新触发循环
            LoopTimer(tm.tid);        
        }
    }

    //如果是带有超时限制的wait操作，要处理超时的情况
    if(timeout != WAIT_FOREVER)
    {   
        if(pm->option.id() == EV_SKY_SYNC_TIMER)
        {
            Debug("Wait time out!\n");
            return ERROR_TIME_OUT;
        }
        else
        {
            Debug("Wait timer stopped!\n");
            StopTimer(_wait_timer);
        }
    }

    _current_msg = pm;
    return SUCCESS;
}

STATUS MessageQueue::Receive(const MessageFrame& message)
{
    //这里需要首先检查是否有专用工作线程并且线程已经准备退出
    //如果是，则直接返回错误
    CHECK_WORKTHREAD_STOP;
    pthread_mutex_lock(&_queue_mutex);
    _waiting_frames.push(message);
    _push_count++;
    pthread_cond_signal(&_queue_cond);
    pthread_mutex_unlock(&_queue_mutex);
    return SUCCESS;
}

STATUS MessageQueue::Run(int priority)
{
    if(INVALID_THREAD_ID != _workthread_id)
    {
        return ERROR_DUPLICATED_OP;
    }
    
    //线程必须是PTHREAD_CREATE_JOINABLE的!!! Stop时要通过join的方式回收
    _workthread_id = StartThread(name().c_str(),mqWorkLoopTaskEntry,(void *)this,true);
    if(INVALID_THREAD_ID == _workthread_id)
    {
        fprintf(stderr,"MessageQueue %s failed to start work thread!\n",name().c_str());
        return ERROR;
    }

    return SUCCESS;
}

STATUS MessageQueue::Stop()
{
    int ret = 0;
    if(INVALID_THREAD_ID == _workthread_id)
    {
        //本函数和Run函数是对应的关系，只是负责停止Run函数创建的工作线程，用户自己的线程不会去管
        //没有用Run创建工作线程时，例如临时消息单元，需要由用户自己保证此时没有调用并阻塞在Wait函数中
        return SUCCESS;
    }
        
    ret = pthread_kill(_workthread_id,0);
    if(0 != ret)
    {
        _workthread_id = INVALID_THREAD_ID;
        fprintf(stderr,"%s work thread 0x%lx does not exist! pthread_kill returns %d!\n",
            name().c_str(),_workthread_id,ret);
        return SUCCESS;
    }
    
    //如果有工作线程，该线程此时应该处于wait或messageentry中，
    //这里要设置并等待线程自己主动退出
    _will_be_stopped = true; //首先设置工作线程停止标记位

    pthread_mutex_lock(&_queue_mutex);
    pthread_cond_signal(&_queue_cond); //伪造有消息达到的条件变量，使得工作线程wait返回
    pthread_mutex_unlock(&_queue_mutex);
    
    Debug("%s is waiting for work thread 0x%lx to exit ...\n",
        name().c_str(),_workthread_id);

    //等待回收工作线程资源
    ret = pthread_join(_workthread_id, NULL); 
    if(0 != ret)
    {
        fprintf(stderr,"%s failed to join work thread 0x%lx! ret = %d!\n",
            name().c_str(),_workthread_id,ret);
        return ERROR;
    }
    
    _workthread_id = INVALID_THREAD_ID;
    Debug("%s work thread is stopped! ret = %d\n",
        name().c_str(),ret);
    return SUCCESS;
}

const MessageFrame* MessageQueue::CurrentMessageFrame()
{
    return _current_msg;
}

void MessageQueue::DoTimer(const TimerMessage& timeout)
{
    MessageOption option;
    option.setId(timeout.msgid);
    option.setType(MESSAGE_TIMER);
    
    MessageFrame message(timeout,option);
    Receive(message);
}

TIMER_ID MessageQueue::GetTimerId()
{
    TimerMessage m;
    if(!_current_msg)
    {
        return INVALID_TIMER_ID;
    }

    if(_current_msg->option.type() != MESSAGE_TIMER)
    {
        return INVALID_TIMER_ID;
    }

    m.deserialize(_current_msg->data);
    return m.tid;
}

uintptr MessageQueue::GetTimerArg()
{
    TimerMessage m;
    if(!_current_msg)
    {
        return 0;
    }

    if(_current_msg->option.type() != MESSAGE_TIMER)
    {
        return 0;
    }

    m.deserialize(_current_msg->data);
    return m.arg;
}

string MessageQueue::GetTimerStrArg()
{
    TimerMessage m;
    if(!_current_msg)
    {
        return 0;
    }

    if(_current_msg->option.type() != MESSAGE_TIMER)
    {
        return 0;
    }

    m.deserialize(_current_msg->data);
    return m.str_arg;
}
void MessageQueue::PrintTraceMsg()
{
    MutexLock lock_trace(_trace_mutex);       
    if (_top_dur_msgs.size() <=0)
    {
        return;
    }
    printf("%-6s %-36s %-10s %-24s %-24s %-12s\n",
        "Index","Name","MessageId","BeginTime","EndTime","Duration(us)");

    int j = 0;
    multimap<uint64,MessageTraceTime>::iterator it;
    it = _top_dur_msgs.begin();
    for (;it !=_top_dur_msgs.end();it++)
    {
        printf("%-6u %-36s %-10u %-24s %-24s %-12lu\n",
                ++j,
                name().c_str(),
                it->second._id,
                it->second._begin_time.tostr().c_str(),
                it->second._end_time.tostr().c_str(),
                it->second._time_span.GetSpan());
    }
}

void MessageQueue::TraceMsgTime(const MessageFrame &msg,bool is_start)
{
    if(0 == _trace_message_id)
    {
        return;
    }

    if (is_start)
    {
        Debug("start process msg\n");
        Datetime btime(time(0));
        _trace_begin_time = btime;
        _trace_time_span.Begin();        
        return;
    }
    else
    {
        Debug("end process msg\n");
	Datetime etime(time(0));
        _trace_end_time = etime;		
        _trace_time_span.End();
    }

    MutexLock lock_trace(_trace_mutex);
    if (10 <= _top_dur_msgs.size())
    {
        multimap<uint64,MessageTraceTime>::iterator it;
        it = _top_dur_msgs.begin();
        if (it->second._time_span.GetSpan() >= _trace_time_span.GetSpan())
        {
            return;
        }
        _top_dur_msgs.erase(it);
    }
    Debug("insert msg:%d\n",_trace_time_span.GetSpan());
    MessageTraceTime trace_msg(msg.option.id(),_trace_begin_time,_trace_end_time,_trace_time_span);
    _top_dur_msgs.insert(pair<uint64,MessageTraceTime>(_trace_time_span.GetSpan(),trace_msg));

    return;
}

void MessageQueue::TracePrint(const MessageFrame* pm)
{
    if(0 == _trace_message_id)
    {
        return;
    }

    SkyAssert(NULL != pm);

    if(_trace_message_id == 0xFFFFFFFF || _trace_message_id == pm->option.id())
    {
    	Datetime btime(time(0));
        printf("\n%s\n%s\n[%s] receives a message, id = %u, type = %u, from application = %s ,process = %s, unit = %s,\n"
                "content: \n%s\n",
                "-----------------------message trace-----------------------",
                btime.tostr().c_str(),
                name().c_str(),
                pm->option.id(),
                pm->option.type(),
                pm->option.sender()._application.c_str(),
       		pm->option.sender()._process.c_str(),
                pm->option.sender()._unit.c_str(),
                pm->data.c_str());
    }
}

