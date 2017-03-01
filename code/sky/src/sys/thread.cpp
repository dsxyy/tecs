/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：thread.cpp
* 文件标识：见配置管理计划书
* 内容摘要：pthread库封装实现文件
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
#include "sys.h"
#include "shell.h"
#include "exception.h"

class ThreadRecord
{
public:
    string name;
    int priority;
    int stacksize;
    ThreadEntry entry;
    void *arg;
    THREAD_ID id;
    pid_t tid;
    
    ThreadRecord(const char *name,int priority,int stacksize,ThreadEntry entry,void *arg)
    {
        this->name.assign(name);
        this->priority = priority;
        this->stacksize = stacksize;
        this->entry = entry;
        this->arg = arg;
        this->id = INVALID_THREAD_ID;
        this->tid = -1;
    };
};

class ThreadList
{
public:
    ThreadList()
    {
        _m.Init();
    };

    void AddRecord(ThreadRecord *ptr)
    {
        _m.Lock();
        _threads.push_back(ptr);
        _m.Unlock();
    };
    
    void RemoveRecord(ThreadRecord *ptr)
    {
        _m.Lock();
        _threads.remove(ptr);
        _m.Unlock();
    };

    void RemoveAndFreeRecord(THREAD_ID id)
    {
        ThreadRecord *ptr = NULL;
        _m.Lock();
        list<ThreadRecord *>::iterator it;
        for( it = _threads.begin(); it != _threads.end(); ) 
        {
            if((*it)->id == id)
            {
                ptr = *it;
                it = _threads.erase(it);
                break;
            }
            else
            {
                ++it;
            }
        }
        _m.Unlock();
        if(ptr)
            delete ptr;
    };
    
    bool GetName(THREAD_ID id,string &name)
    {
        bool found = false;
        _m.Lock();
        list<ThreadRecord *>::iterator it;
        for( it = _threads.begin(); it != _threads.end(); ++it ) 
        {
            if((*it)->id == id)
            {
                name = (*it)->name;
                found = true;
                break;
            }
        }
        _m.Unlock();
        return found;
    };
    
    void Show()
    {
        ThreadRecord *ptr = NULL;
        _m.Lock();
        list<ThreadRecord *>::iterator it;
        printf("----------------------Threads-----------------------\n");
        printf("%-16s %-6s %-18s %-3s\n","Name","Tid","Id","Priority");
        for( it = _threads.begin(); it != _threads.end(); ++it ) 
        {
            ptr = *it;
            printf("%-16s %-6d 0x%-16lx %-3d\n",ptr->name.c_str(),ptr->tid,ptr->id,ptr->priority);
        }
        _m.Unlock();
    };
    
private:
    list<ThreadRecord *> _threads;
    Mutex _m;
};

static ThreadList s_threads;
static void *CommonThreadEntry(void *thread)
{
    ThreadRecord *ptr = static_cast<ThreadRecord *>(thread);
    ptr->id = pthread_self();
    ptr->tid = (pid_t)syscall(__NR_gettid);
    s_threads.AddRecord(ptr);   
    void *res = ptr->entry(ptr->arg);
    s_threads.RemoveRecord(ptr);   
    delete ptr;
    return res;
}

//简化版，创建非实时线程，使用默认优先级和堆栈大小
THREAD_ID StartThread(const char *name,ThreadEntry entry,void *arg, bool joinable)
{
    if(!name)
    {
        fprintf(stderr,"StartThread: no thread name!\n");
        SkyAssert(false);
        return INVALID_THREAD_ID;
    }

    int detachstate = PTHREAD_CREATE_DETACHED;
    if(joinable)
    {
        detachstate = PTHREAD_CREATE_JOINABLE;
    }

    THREAD_ID thread_id;
    pthread_attr_t attr;

    pthread_attr_init(&attr);   
    //线程设置为detached模式
    pthread_attr_setdetachstate (&attr, detachstate);
    ThreadRecord *ptr = new ThreadRecord(name,0,0,entry,arg);
    if(0 != pthread_create(&thread_id,&attr,(ThreadEntry)CommonThreadEntry,(void *)ptr))
    {
        delete ptr;
        fprintf(stderr,"StartThread call pthread_create failed, errno = %d!\n",errno);
        return INVALID_THREAD_ID;
    }
    return thread_id;
}

//加强版，创建实时线程，支持自定义优先级和堆栈大小
THREAD_ID StartThreadEx(const char *name,int priority,int stacksize,ThreadEntry entry,void *arg)
{
    THREAD_ID thread_id;
    pthread_attr_t attr;
    
    pthread_attr_init(&attr);
    //Specifies that the scheduling policy and associated attributes 
    //are to be set to the corresponding values from this attribute object
    if ( pthread_attr_setinheritsched(&attr,PTHREAD_EXPLICIT_SCHED) != 0 )
    {
        printf("StartThread: set attr failed! errno: %d.\n",errno);
        return INVALID_THREAD_ID;
    }
    //线程设置为detached模式
    pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);

    //使用实时优先级
    struct sched_param scheparam;
    pthread_attr_setschedpolicy(&attr,SCHED_FIFO);
    pthread_attr_getschedparam(&attr,&scheparam);
    scheparam.__sched_priority  = priority;
    pthread_attr_setschedparam(&attr,&scheparam);

    if(stacksize > 0)
    {
        //自定义堆栈大小
        int mem_page_size = getpagesize(); /* 页大小 */
        stacksize = ((stacksize + mem_page_size - 1 )/mem_page_size)*mem_page_size;
        pthread_attr_setstacksize(&attr,stacksize);
    }

    ThreadRecord *ptr = new ThreadRecord(name,priority,stacksize,entry,arg);
    if(0 != pthread_create(&thread_id,&attr,(ThreadEntry)CommonThreadEntry,(void *)ptr))
    {
        delete ptr;
        fprintf(stderr,"StartThread call pthread_create failed, errno = %d!\n",errno);
        return INVALID_THREAD_ID;
    }
    return thread_id;
}

STATUS KillThread(THREAD_ID id)
{
    s_threads.RemoveAndFreeRecord(id);

    if (id == pthread_self())
    {
        pthread_exit(0);
        return SUCCESS;
    }

    int err = pthread_kill(id,0);
    if(err == ESRCH)
    {
        printf("thread 0x%lx does not exist./n",id);
        return SUCCESS;
    }
    else if(err == EINVAL)
    {
        fprintf(stderr,"pthread_kill failed! errno=%d\n",errno);
        return ERROR;
    }
    else if(0 != pthread_cancel(id))
    {
        fprintf(stderr,"pthread_cancel 0x%lx failed! errno=%d\n",id,errno);
        return ERROR;
    }

    return SUCCESS;
}

STATUS GetThreadName(THREAD_ID id,string& name)
{
    if(false == s_threads.GetName(id,name))
    {
        name.assign("unknown");
        return ERROR_OBJECT_NOT_EXIST;
    }
    return SUCCESS;
}

THREAD_ID Thread::Id()
{
    return tid;
}

void* Thread::EntryPoint(void * p)
{
    Thread *pthis = (Thread*)p;
    ThreadRecord *ptr = new ThreadRecord(pthis->name.c_str(),
                                            pthis->priority,
                                            0,
                                            (ThreadEntry)(Thread::EntryPoint),
                                            pthis->m_arg);
    ptr->id = pthis->Id();
    ptr->tid = (pid_t)syscall(__NR_gettid);
    s_threads.AddRecord(ptr);
    pthis->Run(pthis->Arg());
    s_threads.RemoveAndFreeRecord(pthis->tid);
    pthis->tid = INVALID_THREAD_ID;
    return NULL;
}

int Thread::Start(void *arg)
{
    Arg(arg); // store user data
    struct sched_param scheparam;
    pthread_attr_t  attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
    pthread_attr_setinheritsched(&attr,PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&attr,SCHED_FIFO);
    pthread_attr_getschedparam(&attr,&scheparam);
    scheparam.__sched_priority  = priority;
    pthread_attr_setschedparam(&attr,&scheparam);
    return pthread_create(&tid,&attr,EntryPoint, this);
}

int Thread::Stop()
{
    if(INVALID_THREAD_ID == tid)
    {
        return 0;
    }
    
    s_threads.RemoveAndFreeRecord(tid);
    pthread_cancel(tid);
    tid = INVALID_THREAD_ID;
    return 0;
}

int Thread::Run(void * arg)
{
   Setup();
   Execute(arg);
   tid = INVALID_THREAD_ID;
   return 0;
}

void DbgShowThreads()
{
    s_threads.Show();
}
DEFINE_DEBUG_FUNC(DbgShowThreads);
DEFINE_DEBUG_CMD(threads,DbgShowThreads);
