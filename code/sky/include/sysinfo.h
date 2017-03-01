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
    
    string name;   //mu����
    bool exist;  //��Ϣ��Ԫ�Ƿ���ڣ�����Ϣ�Ƿ�ɹ���ȡ��
    int64 age; //����Ϣ��Ԫ�Դ�ע��Ϊkey��Ϣ֮���Ѿ����˶೤ʱ���ˣ���λ�Ǻ���
    int state;     //mu״̬���ϵ磬�������˳��ȵȡ�������
    pid_t pid;     //mu�����̵߳�pid
    THREAD_ID tid; //mu�����̵߳�thread id
    uint64 push_count; //mu���յ���Ϣ����
    uint64 pop_count;  //mu�������Ϣ����
    uint32 queued_count; //mu�ȴ������е���Ϣ����
    uint32 timer_count; //mu����Ķ�ʱ����Դ����
    uint32 last_message;  //mu���һ���������Ϣid

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
    
    string name;   //process����
    bool   exist;  //�����Ƿ���ڣ�����Ϣ�Ƿ�ɹ���ȡ��
    string file;   //�����̵Ŀ�ִ���ļ�·��
    pid_t pid;     //������pid
    int running_seconds;  //�������Ѿ������˶����룿
    int exceptions; //�����̷��������ٴ��쳣
    vector<string> asserts; //�����̷������Ķ��Լ�¼
    //vector<TimerRunningInfo> timerinfo //�������еĶ�ʱ����Ϣ����ѡ��
    //vector<ThreadRunningInfo> threadinfo; //�������е��߳���Ϣ����ѡ��
    vector<MuRuntimeInfo> muinfo;  //�������е���Ϣ��Ԫ��Ϣ����
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
    
    string name;    //application����
    Datetime dt;                       //������Ϣ�ɼ���ʱ��
    vector<ProcRuntimeInfo> procinfo;  //��application�����н�����Ϣ����
    
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

