#ifndef SKY_SYS_RUNNING_INFO_H
#define SKY_SYS_RUNNING_INFO_H
#include "message_unit.h"
#include "datetime.h"
#include "exception.h"

class MuRuntimeInfo: public Serializable
{
public:
    MuRuntimeInfo()
    {
        exist = false;
        age = 0;
        state = 0;
        pid = 0;
        tid = INVALID_THREAD_ID;
        push_count = 0;
        pop_count = 0;
        queued_count = 0;
        timer_count = 0;
        last_message = 0;
    };
    
    MuRuntimeInfo(const string& _name):name(_name)
    {
        exist = false;
        state = 0;
        age = 0;
        pid = 0;
        tid = INVALID_THREAD_ID;
        push_count = 0;
        pop_count = 0;
        queued_count = 0;
        timer_count = 0;
        last_message = 0;
    };
    
    string name;   //mu名称
    bool exist;  //消息单元是否存在，即信息是否成功获取到
    int64 age; //本消息单元自从注册为key消息之后已经过了多长时间了，单位是毫秒
    int state;     //mu状态，上电，启动，退出等等。。。。
    pid_t pid;     //mu处理线程的pid
    THREAD_ID tid; //mu处理线程的thread id
    uint64 push_count; //mu接收的消息数量
    uint64 pop_count;  //mu处理的消息数量
    uint32 queued_count; //mu等待队列中的消息数量
    uint32 timer_count; //mu申请的定时器资源数量
    uint32 last_message;  //mu最后一条处理的消息id

    SERIALIZE
    {
        SERIALIZE_BEGIN(MuRuntimeInfo);
        WRITE_VALUE(name);
        WRITE_VALUE(exist);
        WRITE_VALUE(age);
        WRITE_VALUE(state);
        WRITE_VALUE(pid);
        WRITE_VALUE(tid);
        WRITE_VALUE(push_count);
        WRITE_VALUE(pop_count);
        WRITE_VALUE(queued_count);
        WRITE_VALUE(timer_count);
        WRITE_VALUE(last_message);
        SERIALIZE_END();
    }

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MuRuntimeInfo);
        READ_VALUE(name);
        READ_VALUE(exist);
        READ_VALUE(age);
        READ_VALUE(state);
        READ_VALUE(pid);
        READ_VALUE(tid);
        READ_VALUE(push_count);
        READ_VALUE(pop_count);
        READ_VALUE(queued_count);
        READ_VALUE(timer_count);
        READ_VALUE(last_message);
        DESERIALIZE_END();
    }
};

class ProcRuntimeInfo: public Serializable
{
public:
    ProcRuntimeInfo()
    {
        exist = false;
        pid = 0;
        running_seconds = 0;
        exceptions = 0;
    };

    ProcRuntimeInfo(const string& _name):name(_name)
    {
        exist = false;
        pid = 0;
        running_seconds = 0;
        exceptions = 0;
    };
    
    string name;   //process名称
    bool   exist;  //进程是否存在，即信息是否成功获取到
    string file;   //本进程的可执行文件路径
    pid_t pid;     //本进程pid
    int running_seconds;  //本进程已经运行了多少秒？
    int exceptions; //本进程发生过过少次异常
    vector<string> asserts; //本进程发生过的断言记录
    //vector<TimerRunningInfo> timerinfo //本进程中的定时器信息（可选）
    //vector<ThreadRunningInfo> threadinfo; //本进程中的线程信息（可选）
    vector<MuRuntimeInfo> muinfo;  //本进程中的消息单元信息数组
    vector<MID> message_route;
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(ProcRuntimeInfo);
        WRITE_VALUE(name);
        WRITE_VALUE(exist);
        WRITE_VALUE(file);
        WRITE_VALUE(pid);
        WRITE_VALUE(running_seconds);
        WRITE_VALUE(exceptions);
        WRITE_VALUE(asserts);
        WRITE_OBJECT_ARRAY(muinfo);
        WRITE_OBJECT_ARRAY(message_route);
        SERIALIZE_END();
    }

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(ProcRuntimeInfo);
        READ_VALUE(name);
        READ_VALUE(exist);
        READ_VALUE(file);
        READ_VALUE(pid);
        READ_VALUE(running_seconds);
        READ_VALUE(exceptions);
        READ_VALUE(asserts);
        READ_OBJECT_ARRAY(muinfo);
        READ_OBJECT_ARRAY(message_route);
        DESERIALIZE_END();
    }
};

class AppRuntimeInfo: public Serializable
{
public:
    AppRuntimeInfo()
    {

    };
    
    AppRuntimeInfo(const string& _name):name(_name)
    {

    };
    
    string name;    //application名称
    Datetime dt;                       //本次信息采集的时间
    vector<ProcRuntimeInfo> procinfo;  //本application的所有进程信息数组
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(AppRuntimeInfo);
        WRITE_VALUE(name);
        WRITE_OBJECT(dt);
        WRITE_OBJECT_ARRAY(procinfo);
        SERIALIZE_END();
    }

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(AppRuntimeInfo);
        READ_VALUE(name);
        READ_OBJECT(dt);
        READ_OBJECT_ARRAY(procinfo);
        DESERIALIZE_END();
    }
};

class AppRuntimeInfoReq: public Serializable
{
public:
    vector<MID> message_route;
    vector<string> processes;

    SERIALIZE
    {
        SERIALIZE_BEGIN(AppRuntimeInfoReq);
        WRITE_OBJECT_ARRAY(message_route);
        WRITE_VALUE(processes);
        SERIALIZE_END();
    }

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(AppRuntimeInfoReq);
        READ_OBJECT_ARRAY(message_route);
        READ_VALUE(processes);
        DESERIALIZE_END();
    }
};
#endif

