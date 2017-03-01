#ifndef SKY_MESSAGE_QUEUE_H
#define SKY_MESSAGE_QUEUE_H
#include "message.h"
#include "timer.h"
#include "datetime.h"
#include "clock.h"


/* 消息状态统一使用以下定义，禁止任何其他形式的自定义 */
#define S_Startup     1   /* 初始化态 */
#define S_Work     2   /* 工作态 */
#define S_Exit        3   /* 退出 */

#define S_Wait_Msg      1   /* 等待消息 */
#define S_Rcv_Msg       2   /* 接受消息 */
#define S_Snd_Msg       3   /* 发送消息 */

class MessageTraceTime
{
public:
    MessageTraceTime()
    {
        _id=0;
    };
    MessageTraceTime(uint32 id,
    const Datetime &begin_time,
    const Datetime &end_time,
    const Timespan &time_span):
    _id(id),
    _begin_time(begin_time),
    _end_time(end_time),
    _time_span(time_span)
    {
    };
    uint32  _id;
    Datetime _begin_time;
    Datetime _end_time;
    Timespan _time_span;
private:
};

class MessageQueue:public TimerClient
{
public:
    MessageQueue(const string& name):TimerClient(name)
    {
        _handler = NULL;
        _current_msg = NULL;
        _wait_timer = INVALID_TIMER_ID;
        _workthread_id = 0;
        _workthread_tid= 0;
        _push_count = 0;
        _pop_count = 0;
        _state = S_Startup;
        _trace_message_id = 0;
        _will_be_stopped = false;
        _handle_msg_state = S_Wait_Msg;
        pthread_mutex_init(&_queue_mutex,0);
        pthread_cond_init(&_queue_cond,0);
        _trace_mutex.Init();
    };
    
    virtual ~MessageQueue() 
    {
        Stop();
        pthread_mutex_unlock(&_queue_mutex);
        pthread_mutex_destroy(&_queue_mutex);
        pthread_cond_destroy(&_queue_cond);
        //cout << "message queue " << name() << " destroyed!" << endl;
    };

    THREAD_ID get_task_id() { return _workthread_id;};
    pid_t get_task_tid() { return _workthread_tid;};
    int get_handle_msg_state() { return _handle_msg_state;};
    int get_state() { return _state;};
    uint64 get_push_count() { return _push_count;};
    uint64 get_pop_count() { return _pop_count;};
    size_t get_queued_count() { return _waiting_frames.size();};
    void set_state(int state) { _state = state;};
    void set_handle_msg_state(int state) { _handle_msg_state = state;};

    void SaveTid(pid_t tid) { _workthread_tid = tid; };
    STATUS SetHandler(MessageHandler *h)
    {
        //此处不用判断入参，这样用户可以传入NULL来反注册handler
        _handler = h;
        return SUCCESS;
    };

    MessageHandler *GetHandler() const
    {
        return _handler;
    };
    
    virtual STATUS Register() { return SUCCESS;};
    STATUS Receive(const MessageFrame&);
    STATUS Run(int priority = 1);
    STATUS Stop();
    STATUS Wait(MessageFrame *,uint32 timeout); 
    const MessageFrame* CurrentMessageFrame();
    void DoTimer(const TimerMessage&);
    TIMER_ID GetTimerId();
    uintptr GetTimerArg();
    string GetTimerStrArg();
    STATUS TraceRcvMessage(uint32 id)
    {
        _trace_message_id = id;
        if (0 == id)
        {
            _top_dur_msgs.clear();
        }
        return SUCCESS;
    };
    void TracePrint(const MessageFrame*);
    virtual void Print() {};

    void TraceMsgTime(const MessageFrame &msg,bool is_start);

    void PrintTraceMsg();
    
private:
    DISALLOW_COPY_AND_ASSIGN(MessageQueue);
    
    //_will_be_destroyed为true时表示当前MessageQueue类的对象实例即将被销毁析构，
    //工作线程中如果检测到该标志为true了，则不应再使用该对象
    bool _will_be_stopped;
    
    MessageHandler *_handler;
    MessageFrame *_current_msg;
    int _state;
    TIMER_ID _wait_timer;
    pthread_mutex_t _queue_mutex;
    pthread_cond_t _queue_cond;
    THREAD_ID _workthread_id;
    pid_t _workthread_tid;
    uint32 _trace_message_id;
    queue<MessageFrame> _waiting_frames;
    uint64 _push_count;
    uint64 _pop_count;
    Mutex _trace_mutex; ;
    Datetime _trace_begin_time;
    Datetime _trace_end_time;
    Timespan _trace_time_span;
    multimap<uint64,MessageTraceTime>_top_dur_msgs;
    int _handle_msg_state;
    //vector<MessageFrame> temp_frames;
};
#endif


