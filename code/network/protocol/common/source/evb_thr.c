#ifdef __cplusplus
extern "C" {
#endif

#include "evb_comm.h"
#include "evb_thr.h"
#include "evb_msg.h"

#ifdef _WIN32
    /* 封装此函数为了在线程空间可以加入监控代码; */
    void ThreadProc(void *pParam)
    {
        TEvbThrdMgr *pThrd = (TEvbThrdMgr *)pParam;
        if(NULL == pParam)
        {
            return ;
        }

        if(NULL == pThrd->pfEntry)
        {
            return ;
        }
        
        pThrd->wExitCode = 1;        
        pThrd->wThrdFlag = EVB_THREAD_RUNNING; 
        
        pThrd->pfEntry(pParam);
        
        pThrd->wThrdFlag = EVB_THREAD_STOP;
        pThrd->wExitCode = 0;
        pThrd->tThreadID = INVALID_THREAD_ID;
        pThrd->tid= INVALID_THREAD_ID;

        return ;
    }
    
    int32 StartEvbThread(TEvbThrdMgr *pThrd)
    {
        if(NULL == pThrd)
        {
            EVB_ASSERT(0);
            EVB_LOG(EVB_LOG_ERROR, "StartEvbThread: NULL == pThrd\n");
            return -1;
        }

        if(NULL == pThrd->pfEntry)
        {
            EVB_ASSERT(0);
            EVB_LOG(EVB_LOG_ERROR, "StartEvbThread: NULL == pThrd->pfEntry\n");
            return -1;
        }

        /* Thread data init */
        if(pThrd->pfInit)
        {
            pThrd->pfInit(pThrd);
        }

        /* Start thread */
        pThrd->wThrdFlag = EVB_THREAD_STOP;
        pThrd->wExitCode = 0;
        pThrd->tid = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProc, pThrd, 0, (LPDWORD)&pThrd->tThreadID);
        if(NULL == pThrd->tid)
        {
            EVB_LOG(EVB_LOG_ERROR, "StartEvbThread: call CreateThread failed. last error: %d.\n", GetLastError());
            return -1;
        }

        return 0;
    }

    int32 KillEvbThread(TEvbThrdMgr *pThrd)
    {    
        u32 dwExit = 0;
        u32 dwCnt = 0;
        if(NULL == pThrd)
        {
            EVB_ASSERT(0);
            EVB_LOG(EVB_LOG_ERROR, "KillEvbThread: NULL == pThrd\n");
            return -1;
        }

        if(INVALID_THREAD_ID == pThrd->tThreadID)
        {
            EVB_LOG(EVB_LOG_ERROR, "KillEvbThread: INVALID_THREAD_ID == pThrd->tThreadID\n");
            return -1;
        }

        /* TerminateThread */
        if(EVB_THREAD_RUNNING == pThrd->wThrdFlag)
        {
            pThrd->wThrdFlag = EVB_THREAD_STOP;
            if(1 == pThrd->wExitCode)
            {
                while(1)
                {
                    if(dwCnt > 50)
                    {                        
                        EVB_LOG(EVB_LOG_ERROR, "KillEvbThread: Kill thread(id: 0x%x) time out.\n", 
                            pThrd->tThreadID);
                        break;
                    }
                    if(0 == pThrd->wExitCode )
                    {
                        break;
                    }
                    ++dwCnt;
                    EvbDelay(100);
                }
            }/* end if(1 == pThrd->wExitCode) */
        }

        /* Destroy message queue; */
        EvbDestroyThrdMsgQue(pThrd);

        if(GetExitCodeThread(pThrd->tid, &dwExit))
        {
            TerminateThread(pThrd->tid, dwExit);
        }

        pThrd->wThrdFlag = EVB_THREAD_STOP;
        pThrd->wExitCode = 0;
        pThrd->tThreadID = INVALID_THREAD_ID;
        pThrd->tid= INVALID_THREAD_ID;
        
        return 0;
    }

#else    
    /* 封装此函数为了在线程空间可以加入监控代码; */
    void ThreadProc(void *pParam)
    {
        TEvbThrdMgr *pThrd = (TEvbThrdMgr *)pParam;
        if(NULL == pParam)
        {
            return ;
        }

        if(NULL == pThrd->pfEntry)
        {
            return ;
        }
        
        pThrd->tThreadID = pthread_self();
        pThrd->tid = (TEvbTaskID)syscall(__NR_gettid);
        pThrd->wExitCode = 1;        
        pThrd->wThrdFlag = EVB_THREAD_RUNNING; 

        pThrd->pfEntry(pParam);   
      
        pThrd->wThrdFlag = EVB_THREAD_STOP;
        pThrd->wExitCode = 0;
        pThrd->tThreadID = INVALID_THREAD_ID;
        pThrd->tid= INVALID_THREAD_ID;
        pThrd->tThreadID = INVALID_THREAD_ID;
        
        return ;
    }

    int32 StartEvbThread(TEvbThrdMgr *pThrd)
    {
        struct sched_param scheparam;
        pthread_attr_t attr;

        if(NULL == pThrd)
        {
            EVB_ASSERT(0);
            EVB_LOG(EVB_LOG_ERROR, "StartEvbThread: NULL == pThrd\n");
            return -1;
        }

        if(NULL == pThrd->pfEntry)
        {
            EVB_ASSERT(0);
            EVB_LOG(EVB_LOG_ERROR, "StartEvbThread: NULL == pThrd->pfEntry\n");
            return -1;
        }

        if (pthread_attr_setinheritsched(&attr,PTHREAD_EXPLICIT_SCHED) != 0 )
        {
            EVB_LOG(EVB_LOG_ERROR, "StartEvbThread: set thread attr failed! errno: %d.\n",errno);
            return -1;
        }

        /* Thread data init */
        if(pThrd->pfInit)
        {
            pThrd->pfInit(pThrd);
        }


        pthread_attr_init(&attr);
        /* 线程设置为detached模式; */
        pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);
        /* 使用实时优先级; */
        pthread_attr_setschedpolicy(&attr,SCHED_FIFO);
        pthread_attr_getschedparam(&attr,&scheparam);
        scheparam.__sched_priority  = pThrd->dwPriority; 
        pthread_attr_setschedparam(&attr,&scheparam);

        pThrd->wThrdFlag = EVB_THREAD_STOP;
        if(0 != pthread_create(&pThrd->tThreadID, &attr, (EVB_THRD_ENTRY)ThreadProc, pThrd))
        {
            EVB_LOG(EVB_LOG_ERROR, "StartEvbThread: call CreateThread failed. last error: %d.\n", errno);
            return -1;
        }

        return 0;
    }

    int32 KillEvbThread(TEvbThrdMgr *pThrd)
    {
        int32 iRet = 0;
        u32 dwCnt = 0;
        
        if(NULL == pThrd)
        {
            EVB_ASSERT(0);
            EVB_LOG(EVB_LOG_ERROR, "KillEvbThread: NULL == pThrd\n");
            return -1;
        }

        if(INVALID_THREAD_ID == pThrd->tThreadID)
        {
            EVB_LOG(EVB_LOG_ERROR, "KillEvbThread: INVALID_THREAD_ID == pThrd->tThreadID\n");
            return -1;
        }

        /* TerminateThread */
        if(EVB_THREAD_RUNNING == pThrd->wThrdFlag)
        {
            pThrd->wThrdFlag = EVB_THREAD_STOP;
            if(1 == pThrd->wExitCode)
            {
                while(1)
                {
                    if(dwCnt > 50)
                    {                        
                        EVB_LOG(EVB_LOG_ERROR, "KillEvbThread: Kill thread(id: 0x%lx) time out.\n", 
                            pThrd->tThreadID);
                        break;
                    }
                    
                    if(0 == pThrd->wExitCode)
                    {
                        break;
                    }
                    ++dwCnt;
                    EvbDelay(100);
                }
            }/* end if(1 == pThrd->wExitCode) */
        }

        if(INVALID_THREAD_ID != pThrd->tThreadID)
        {
            iRet = pthread_kill(pThrd->tThreadID,0);
            if(iRet == ESRCH)
            {
                EVB_LOG(EVB_LOG_ERROR, "thread 0x%lx does not exist.\n", pThrd->tThreadID);
            }
            else if(iRet == EINVAL)
            {
                EVB_LOG(EVB_LOG_ERROR, "pthread_kill failed! errno=%d\n", errno);
            }
            else if(0 != pthread_cancel(pThrd->tThreadID))
            {
                EVB_LOG(EVB_LOG_ERROR, "pthread_cancel 0x%lx failed! errno=%d\n", 
                    pThrd->tThreadID, errno);
            }
        }

        /* Destroy message queue; */
        EvbDestroyThrdMsgQue(pThrd);
        
        pThrd->wThrdFlag = EVB_THREAD_STOP;
        pThrd->wExitCode = 0;
        pThrd->tThreadID = INVALID_THREAD_ID;
        pThrd->tid= INVALID_THREAD_ID;

        return iRet;
    }

#endif

#ifdef __cplusplus
}
#endif
