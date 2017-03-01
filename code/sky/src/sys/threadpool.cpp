/*******************************************************************************
* Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：thread.cpp
* 文件标识：见配置管理计划书
* 内容摘要：pthread线程池实现文件
* 当前版本：1.0
* 作    者：张文剑
* 完成日期：2012年03月06日
*
* 修改记录1：
*     修改日期：2012/03/06
*     版 本 号：V1.0
*     修 改 人：张文剑
*     修改内容：创建
*******************************************************************************/
#include "sys.h"
#include "exception.h"

ThreadPool::ThreadPool()
{
    ThreadPool(2);
}

ThreadPool::ThreadPool(int num_of_threads)
{
    _log = Disable;
    
    pthread_mutex_init(&_mutex_sync, NULL); 
    //pthread_mutex_init(&_mutex_work_completion, NULL); 
    
    pthread_mutex_lock(&_mutex_sync);
    _num_of_threads = num_of_threads;
    if (_num_of_threads < 1)  
        _num_of_threads = 1;
    //sem_init(&_available_work, 0, 0);
    pthread_cond_init(&_cond_available_work, NULL);
    pthread_mutex_unlock(&_mutex_sync);

    
}

STATUS ThreadPool::CreateThreads(const char *name,int priority)
{
    THREAD_ID tid = INVALID_THREAD_ID;

    _name = string(name);

    if (_log == Enable)
    {
        printf("Create threads %s ...\n",name);
    }
    
    if(!_works_in_runing.empty())
    {
        if (_log == Enable)
        {
            printf("Create threads %s error! threads is already created!\n",name);
        }
    }
    
    for (int i = 0; i<_num_of_threads; ++i)
    {
        tid = StartThread(name,&ThreadPool::ExecuteEntry, (void *) this);
        if(tid == INVALID_THREAD_ID)
        {
            return ERROR;
        }
        _works_in_runing[tid] = NULL;
    }
    return SUCCESS;
}

ThreadPool::~ThreadPool()
{
    cerr << "Warning! ~ThreadPool is Not implemented! Please call DestroyPool first!" << endl;
}

void ThreadPool::DestroyPool(int maxPollSecs)
{

    int max_count = 100; //  最多等待次数
    while ( (CountOfWorks() > 0) &&  ( (--max_count) > 0) )  
    {
        //cout << "Work is still incomplete=" << incompleteWork << endl;
        sleep(maxPollSecs);
    }

    pthread_mutex_lock(&_mutex_sync);
    
    map<pthread_t, Worktodo *>::iterator it;
    for(it = _works_in_runing.begin(); it != _works_in_runing.end(); it++)
    {
        if(it->second != NULL)
        {
            delete it->second;
        }
        KillThread(it->first);
    }

    Worktodo *work = NULL;
    while(!_works_of_waiting.empty())
    {
        work = _works_of_waiting.front();
        _works_of_waiting.pop_front();
        delete work;
    }
    
    pthread_mutex_unlock(&_mutex_sync);

        
    //sem_destroy(&_available_work);
    pthread_cond_destroy(&_cond_available_work);
    pthread_mutex_destroy(&_mutex_sync);
    //pthread_mutex_destroy(&_mutex_work_completion);
}

STATUS ThreadPool::AssignWork(Worktodo *work)
{
    pthread_mutex_lock(&_mutex_sync);
    _works_of_waiting.push_back(work);
    pthread_mutex_unlock(&_mutex_sync);
    pthread_cond_signal(&_cond_available_work); // LINUX中可以放在前面, 放在后面会优先强占的(应该是合理,高优先级的先处理) 
    return SUCCESS;
}
int  ThreadPool::CountOfWorks()
{
    int sum;
    pthread_mutex_lock(&_mutex_sync);
    sum = 0;
    map<pthread_t, Worktodo *>::const_iterator it;
    for( it = _works_in_runing.begin(); it != _works_in_runing.end(); it ++)
    {
        if( it->second != NULL)
        {
            sum ++;
        }
    }
    sum += _works_of_waiting.size();
    pthread_mutex_unlock(&_mutex_sync);
    return sum;
}

STATUS ThreadPool::FetchWork(Worktodo **pwork)
{
    // sem_wait(&_available_work); //等到有工作出来

    pthread_mutex_lock(&_mutex_sync);

    while (_works_of_waiting.empty())
    {
        pthread_cond_wait(&_cond_available_work, &_mutex_sync);
    }

    *pwork = _works_of_waiting.front();

    _works_in_runing[pthread_self()] = *pwork;
    
    _works_of_waiting.pop_front();    
    pthread_mutex_unlock(&_mutex_sync);
    
    return SUCCESS;
}

STATUS ThreadPool::CancelWork(const string &name)
{
    pthread_mutex_lock(&_mutex_sync);

    list <Worktodo *> ::iterator lit;
    for ( lit=_works_of_waiting.begin() ; lit != _works_of_waiting.end(); lit++)
    {
        if((*lit)->getName() == name)
        {
            _works_of_waiting.erase(lit);
            pthread_mutex_unlock(&_mutex_sync);
            return SUCCESS;
        }
    }

    map<pthread_t,Worktodo*>::iterator mit; 
    for ( mit=_works_in_runing.begin() ; mit != _works_in_runing.end(); mit++)
    {
        if (NULL == mit->second)
        {
            continue;
        }
        if(mit->second->getName() == name)
        {
            mit->second->Cancel();
            pthread_mutex_unlock(&_mutex_sync);
            return SUCCESS;
        }
    }
 
    pthread_mutex_unlock(&_mutex_sync);
    
    return ERROR_OBJECT_NOT_EXIST;
}

bool ThreadPool::FindWork(const string &name)
{
    pthread_mutex_lock(&_mutex_sync);

    list <Worktodo *> ::iterator lit;
    for ( lit=_works_of_waiting.begin() ; lit != _works_of_waiting.end(); lit++)
    {
        if((*lit)->getName() == name)
        {
            pthread_mutex_unlock(&_mutex_sync);
            return true;
        }
    }

    map<pthread_t,Worktodo*>::iterator mit; 
    for ( mit=_works_in_runing.begin() ; mit != _works_in_runing.end(); mit++)
    {
        if (NULL == mit->second)
        {
            continue;
        }
        if(mit->second->getName() == name)
        {
            pthread_mutex_unlock(&_mutex_sync);
            return true;
        }
    }
 
    pthread_mutex_unlock(&_mutex_sync);
    
    return false;
}

void ThreadPool::ShowInfo(string filter)
{
    int k;
    cout << "Thread Pool: Name " << _name << endl;
    
    pthread_mutex_lock(&_mutex_sync);

    if( filter.empty() 
        || filter.find("i") != string::npos
        || filter.find("r") != string::npos)
    {
        cout << "\tThreads : Total number " << _works_in_runing.size() << endl;
        map<pthread_t, Worktodo *>::const_iterator it;
        k = 0;
        for( it = _works_in_runing.begin(); it != _works_in_runing.end(); it ++)
        {
            k ++;
            
            if( it->second != NULL) // 运行中的任务
            {
                if( filter.empty() || filter.find("r") != string::npos)
                {
                    cout << "\t\t" << k << ": " ;
                    printf("0x%lx", it->first);
                    cout << ": "   << "Runing work : " << it->second->getName() << endl;
                }
            }
            else if( filter.empty() || filter.find("i") != string::npos) // 空闲的
            { 
                cout << "\t\t" << k << ": " ;
                printf("0x%lx", it->first);
                cout << ": "   << "Waiting for new work!" << endl;
            }
        }
    }

    if( filter.empty() 
        || filter.find("w") != string::npos)
    {
    
        cout << "\tworks in waiting: Total number " <<  _works_of_waiting.size() << endl;

        list< Worktodo * >::iterator it_w;
        k = 0;
        for( it_w = _works_of_waiting.begin(); it_w != _works_of_waiting.end(); it_w ++)
        {
            k ++;
            cout << "\t\t" <<  k <<  ": Name: " << (*it_w)->getName() << endl;
        }
    }
    pthread_mutex_unlock(&_mutex_sync);
    cout << "-------- End of Thread pool " << _name << ". --------" << endl;
    
}

void *ThreadPool::ExecuteEntry(void *arg)
{
    STATUS ret = ERROR;
    Worktodo *work = NULL;
    ThreadPool *tpool = (ThreadPool *)arg;
    SkyAssert(tpool != NULL);

    pthread_t thread_id = pthread_self();

    if (tpool->_log == Enable)
    {
        printf("Worker thread 0x%lx is running ...\n",pthread_self());
    }
    while (SUCCESS == tpool->FetchWork(&work))
    {
        if (work)
        {
            if (tpool->_log == Enable)
            {
                printf("work [%s] begin! ret = %d.\n",work->getName().c_str(),ret);
            }
            ret = work->Execute();
            
            if (tpool->_log == Enable)
            {
                printf("work [%s] finished! ret = %d.\n",work->getName().c_str(),ret);
            }
            
            pthread_mutex_lock(&(tpool->_mutex_sync));
            tpool->_works_in_runing[thread_id] = NULL; // 删除任务
            pthread_mutex_unlock(&(tpool->_mutex_sync));

            delete work;
            work = NULL;
            
        }
    }

    SkyAssert(false);
    return 0;
}

