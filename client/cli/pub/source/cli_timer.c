#include <stdio.h>
#include <signal.h>
#include <string.h>
#include "cli_timer.h"
#include "includes.h"

typedef struct tagTimerInfo
{
    WORD16 wJno;
    WORD32 dwTimerNo;
    XOS_TIMER_ID wTimerId;
    WORD16 wTimerType;
    WORD32 dwTimerEvent;
    SWORD32 sdwDurationOrg;
    SWORD32 sdwDuration;
    WORD32 dwParam;
}T_TimerInfo;

#define MAX_TIMER_NO       100
T_TimerInfo g_tTimerSet[MAX_TIMER_NO];
static XOS_TIMER_ID g_TimerId = 1;
WORD32 tTimerSetMutexSem;

XOS_TIMER_ID NewTimerId()
{
    return g_TimerId++;
}

INT GetIdleTimerIdx()
{
    int i;
    for (i = 1; i < MAX_TIMER_NO; i++)
    {
        if ( g_tTimerSet[i].wJno == JOB_TYPE_INVALID)
        {
            break;
        }
    }

    return (i == MAX_TIMER_NO) ? 0 : i;
}

INT GetTimerIdxBySetInfo(WORD32 dwTimerNo, WORD16 wJno,  WORD32 dwParam)
{
    int i;
    for (i = 1; i < MAX_TIMER_NO; i++)
    {
        if (( g_tTimerSet[i].wJno == wJno) &&
            ( g_tTimerSet[i].dwTimerNo == dwTimerNo) &&
            ( g_tTimerSet[i].dwParam == dwParam) )
        {
            break;
        }
    }

    return (i == MAX_TIMER_NO) ? 0 : i;
}

XOS_TIMER_ID SetTimer(WORD32 dwTimerNo, WORD16 wJno, WORD16 wTimerType, SWORD32 sdwDuration, WORD32 dwParam)
{
    int iIdx;
    XOS_TIMER_ID timerRet;
    if (dwTimerNo >= MAX_TIMER_NO)
    {
        return INVALID_TIMER_ID;
    }

    XOS_SemaphoreP((WORD32)tTimerSetMutexSem, (WORD32)WAIT_FOREVER); 
    /*printf("--------- SetTimer 1 dwTimerNo= %u\n ", dwTimerNo);*/
    /*查看定时器是否存在*/
    iIdx = GetTimerIdxBySetInfo(dwTimerNo, wJno, dwParam);
    /*如果不存在，找一个空闲的定时器*/
    if (iIdx == 0)
    {
         iIdx = GetIdleTimerIdx();
         if (iIdx > 0)
         {
             g_tTimerSet[iIdx].wTimerId = NewTimerId();
             g_tTimerSet[iIdx].wJno = wJno;
             g_tTimerSet[iIdx].dwTimerNo = dwTimerNo;
             g_tTimerSet[iIdx].dwTimerEvent = EV_TIMER_1 + dwTimerNo - 1;
             g_tTimerSet[iIdx].wTimerType= wTimerType;
             g_tTimerSet[iIdx].dwParam = dwParam;
        }
    }
	
    if (iIdx == 0)
    {
        printf("ERROR:: set timer failed ! timerno: %u, jno: %u, param: %u \n", dwTimerNo, wJno, dwParam);
        XOS_SemaphoreV(tTimerSetMutexSem);
        return INVALID_TIMER_ID;
    }
	
    /*如果定时器存在，重新设置值为最新的值*/
    g_tTimerSet[iIdx].sdwDurationOrg = sdwDuration/1000;
    g_tTimerSet[iIdx].sdwDuration = sdwDuration/1000;

    timerRet = g_tTimerSet[iIdx].wTimerId;
    XOS_SemaphoreV(tTimerSetMutexSem);

    return timerRet;
}

void KillTimer(WORD16 wJno, WORD32 dwTimerNo,WORD32 dwParam)
{
    int iIdx;

    XOS_SemaphoreP((WORD32)tTimerSetMutexSem, (WORD32)WAIT_FOREVER);
    iIdx = GetTimerIdxBySetInfo(dwTimerNo, wJno, dwParam);
    if (iIdx > 0)
    {
        memset(&g_tTimerSet[iIdx], 0, sizeof(T_TimerInfo));
    }
    XOS_SemaphoreV(tTimerSetMutexSem);
}
	
void KillTimerByTimerId(WORD32 dwTimerId)
{
    int i;
    XOS_SemaphoreP((WORD32)tTimerSetMutexSem, (WORD32)WAIT_FOREVER);
    for (i = 1; i < MAX_TIMER_NO; i++)
    {
        if (( g_tTimerSet[i].wTimerId== dwTimerId) )
        {
            memset(&g_tTimerSet[i], 0, sizeof(T_TimerInfo));
            break;
        }
    }
    XOS_SemaphoreV(tTimerSetMutexSem);
}

void TimerInitial()
{	
    memset(g_tTimerSet, 0, sizeof(g_tTimerSet));
    
    if (FALSE == XOS_CreateSemaphore(
                0,
                0,
                MUTEX_STYLE,
                &tTimerSetMutexSem)
       )
    {
        printf("ERROR:: TimerInitial create timerset mutex semaphore failed \n");
        return;
    }
	
    timer_task();
}

void sigroutine(int signo)
{
    int i;
    switch(signo)
    {
    case SIGALRM:
        XOS_SemaphoreP((WORD32)tTimerSetMutexSem, (WORD32)WAIT_FOREVER);
        for (i = 0; i < MAX_TIMER_NO; i++)
        {
            if (g_tTimerSet[i].sdwDuration > 0)
            {
                g_tTimerSet[i].sdwDuration--;
                if (g_tTimerSet[i].sdwDuration == 0)
                {
                    JID tReceiver;
                    tReceiver.dwJno = g_tTimerSet[i].wJno;
                    /*printf("--------- sigroutine 1 g_tTimerSet[i].dwTimerEvent = %u,  g_tTimerSet[i].wJno = %u\n",g_tTimerSet[i].dwTimerEvent,  g_tTimerSet[i].wJno);*/
                    XOS_SendAsynMsg(g_tTimerSet[i].dwTimerEvent, (BYTE *)&g_tTimerSet[i].dwParam, sizeof(g_tTimerSet[i].dwParam), XOS_MSG_VER0, XOS_MSG_MEDIUM, &tReceiver);
                             
                    if (g_tTimerSet[i].wTimerType == LOOP_TIMER)
                    {
                        g_tTimerSet[i].sdwDuration = g_tTimerSet[i].sdwDurationOrg;
                    }
                    else
                    {
                        memset(&g_tTimerSet[i], 0, sizeof(T_TimerInfo));
                    } 
                }
            }
        }
        XOS_SemaphoreV(tTimerSetMutexSem);
        break;
    default:
        printf("catch a signal: %d\n", signo);
         break;
    }
}

void timer_task()
{
    struct itimerval value, ovalue;

    /*1秒检查一次*/
    signal(SIGALRM, sigroutine);
    value.it_value.tv_sec = 1;
    value.it_value.tv_usec = 0;
    value.it_interval.tv_sec = 1;
    value.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &value, &ovalue);
}

