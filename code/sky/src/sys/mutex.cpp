/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�mutex.cpp
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ���ź�������ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ����Ľ�
* ������ڣ�2011��8��16��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/07/01
*     �� �� �ţ�V1.0
*     �� �� �ˣ����Ľ�
*     �޸����ݣ�����
*******************************************************************************/
#include "sys.h"
#include "exception.h"
STATUS Mutex::Init()
{
    pthread_mutexattr_t _attr;
    pthread_mutexattr_init(&_attr);
    pthread_mutexattr_settype(&_attr, PTHREAD_MUTEX_RECURSIVE);
    if(0 != pthread_mutex_init(&_mutex, &_attr))
    {
        pthread_mutexattr_destroy(&_attr);
        return ERROR;
    }
    pthread_mutexattr_destroy(&_attr);
    _init = true;
    return SUCCESS;
}

bool Mutex::TryLock(void)
{
    if(!_init)
        return false;
    
    return (pthread_mutex_trylock(&_mutex) == 0) ? true : false;
}

void Mutex::Lock(void)
{
    if(!_init)
        return;
       
    pthread_mutex_lock(&_mutex);
    if(_debug)
    {
        printf("mutex %s locked by thread 0x%lx.\n",_name.c_str(),pthread_self());
    }
}

void Mutex::Unlock(void)
{
    if(!_init)
        return;
       
    pthread_mutex_unlock(&_mutex);
    if(_debug)
    {
        printf("mutex %s unlocked by thread 0x%lx.\n",_name.c_str(),pthread_self());
    }
}

Semaphore::Semaphore(unsigned int initial)
{
    if(0 != sem_init(&_sem, 0, initial))
        throw SystemError(SKY_ERROR_MSG("sem_init failed"));
}

Semaphore::~Semaphore()
{
    sem_destroy( &_sem );
}

void Semaphore::wait()
{
    if(0 != sem_wait(&_sem))
        throw SystemError(SKY_ERROR_MSG("sem_wait failed"));
}

bool Semaphore::tryWait()
{
    if(0 != sem_trywait( &_sem ))
    {
        if(errno == EAGAIN)
            return false;
        throw SystemError(SKY_ERROR_MSG("sem_trywait failed"));
    }
    return true;
}

void Semaphore::post()
{
    again:
    if( 0 != sem_post(&_sem) )
    {
        if(errno == EINTR)
            goto again;
        throw SystemError(SKY_ERROR_MSG("sem_post failed"));
    }
}


