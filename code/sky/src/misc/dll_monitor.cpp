/*******************************************************************************
* Copyright (c) 2013������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�dll_monitor.cpp
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ���߳��������ʵ��
* ��ǰ�汾��1.0
* ��    �ߣ���ҵ��
* ������ڣ�2013��4��1��
*
* �޸ļ�¼1��
*     �޸����ڣ�2013/04/01
*     �� �� �ţ�V1.0
*     �� �� �ˣ���ҵ��
*     �޸����ݣ�����
*******************************************************************************/
//#include "dll_monitor.h"
#include "common.h"
#include "timer.h"
#include "datetime.h"
#include "clock.h"

class DllMonitorData
{
public:
    DllMonitorData(const string &name,uint32 over_time=120,bool disable_monitor=false):
        _name(name),_over_time(over_time),_disable_monitor(disable_monitor)
    {
        _tid = syscall(__NR_gettid);
    }
    DllMonitorData(uint32 over_time=120,bool disable_monitor=false):
        _over_time(over_time),_disable_monitor(disable_monitor)
    {
        _tid = syscall(__NR_gettid);
    }
    string _name;
    pid_t  _tid;
    uint32 _over_time;
    bool   _disable_monitor;    
    Timespan _monitor_time_span;
};
class DllMonitorControl
{
public:
    DllMonitorControl()
    {
        _workthread_id = INVALID_THREAD_ID;
        _enable_check = false;
        _check_time = 120;
        _monitor_mutex.SetName("_dll_monitor_mutex");
        _monitor_mutex.Init();
    }
    map<THREAD_ID,DllMonitorData> _threads;
    THREAD_ID _workthread_id;
    Mutex _monitor_mutex;
    bool _enable_check;
    uint32 _check_time;
};
static DllMonitorControl check_control;

extern "C" void *dmWorkLoopTaskEntry(void*)
{
    while (1)
    {
        {
            MutexLock lock (check_control._monitor_mutex);
            map<THREAD_ID,DllMonitorData>::iterator it;
            for (it = check_control._threads.begin();it!= check_control._threads.end(); it++)
            {
                if (it->second._disable_monitor)
                {
                    continue;
                }
                uint64  span;
                it->second._monitor_time_span.End();
                span = it->second._monitor_time_span.GetSpan();
                if (span <= (it->second._over_time*1000*1000))
                {
                    continue;
                }

                pthread_kill(it->first,SIGUSR1);
                check_control._threads.erase(it->first);
            }
        }
        Delay(1000);
    }  
    return 0;
}

STATUS ThreadDllMonitorInit(bool enable_check,uint32 check_time)
{
    check_control._enable_check = enable_check;

    if (!check_control._enable_check)
    {
        return ERROR_PERMISSION_DENIED;
    }
    check_control._check_time = check_time;

    if (check_control._workthread_id != INVALID_THREAD_ID)
        return ERROR_DUPLICATED_OP;

    check_control._workthread_id = StartThread("ThreadMonitor",dmWorkLoopTaskEntry,NULL);
    if(INVALID_THREAD_ID == check_control._workthread_id)
        return ERROR;
    else
        return SUCCESS;
}

STATUS StartThreadDllMonitor(THREAD_ID workthread_id,const string &name="")
{
    if (!check_control._enable_check)
    {
        return ERROR_PERMISSION_DENIED;
    }

    MutexLock lock (check_control._monitor_mutex);
    if (check_control._workthread_id == INVALID_THREAD_ID)
    {
        return ERROR_NOT_READY;
    }

    map<THREAD_ID,DllMonitorData>::iterator it;
    it = check_control._threads.find(workthread_id);
    if (it == check_control._threads.end())
    {
        DllMonitorData md(name,check_control._check_time);
        md._monitor_time_span.Begin();
        check_control._threads[workthread_id] = md;
        return SUCCESS;
    }
    check_control._threads[workthread_id]._disable_monitor = false;
    check_control._threads[workthread_id]._monitor_time_span.Begin();

    return SUCCESS;
}

STATUS EndThreadDllMonitor(THREAD_ID workthread_id)
{
    if (!check_control._enable_check)
    {
        return ERROR_PERMISSION_DENIED;
    }

    MutexLock lock (check_control._monitor_mutex);

    if (check_control._workthread_id == INVALID_THREAD_ID)
    {
        return ERROR_NOT_READY;
    }
    
    map<THREAD_ID,DllMonitorData>::iterator it;
    it = check_control._threads.find(workthread_id );
    if (it == check_control._threads.end())
    {
        return ERROR_OBJECT_NOT_EXIST;
    }
    check_control._threads[workthread_id ]._disable_monitor = true;

    return SUCCESS;
}

STATUS DeleteThreadDllMonitor(THREAD_ID workthread_id)
{
    MutexLock lock (check_control._monitor_mutex);

    map<THREAD_ID,DllMonitorData>::iterator it;
    it = check_control._threads.find(workthread_id );
    if (it == check_control._threads.end())
    {
        return ERROR_OBJECT_NOT_EXIST;
    }
    check_control._threads.erase(workthread_id);

    return SUCCESS;
}

STATUS DbgShowMonitorTable()
{
    printf("--------------------------------------Monitor Units--------------------------------------\n");
    printf("%-6s %-24s %-8s %-18s %-8s %-8s %-8s\n",
           "Index","Name","Tid","Thread","Flag","Time","Span");
    int i = 0;
    string name;
    map<THREAD_ID,DllMonitorData>::const_iterator it;
    MutexLock lock (check_control._monitor_mutex);
    for ( it=check_control._threads.begin() ; it != check_control._threads.end(); it++)
    {
        Timespan monitor_time_span = it->second._monitor_time_span;
        printf("%-6d %-24s %-8d 0x%-16lx  %-8d %-8d %-8llu\n",
                ++i,
                it->second._name.c_str(),
                it->second._tid,
                it->first,
                it->second._disable_monitor,
                it->second._over_time,
                monitor_time_span.GetSpan());
    }
    return SUCCESS;
}

DEFINE_DEBUG_FUNC(DbgShowMonitorTable);


