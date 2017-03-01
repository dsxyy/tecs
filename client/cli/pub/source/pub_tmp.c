
#include <pthread.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h> 
#include <errno.h>
#include <assert.h>
#include "pub_tmp.h"
#include "cli_timer.h"

extern T_CliJobInfo * GetJobInfoByJno(WORD16 wJno);
extern pthread_mutex_t  s_tMutexLock; /* g_atMutex的互斥锁 */
extern WORD32   g_dwMaxSem;
extern WORD32   g_dwSemCount;

#define MQ_MSG_PRIV          18

int XOS_SendAsynMsg(WORD32   dwMsgId,
                            BYTE     *pucData,
                            WORD16   wMsgLen,
                            BYTE     ucVer,
                            BYTE     ucPriority,
                            void     *pReceiver)
{
    T_CliMsg tMsg;
    JID *pDest = (JID *)pReceiver;
    T_CliJobInfo *pDestJob;
    size_t msglen;
    int iRet;

    if (pReceiver == NULL)
    {
        printf("XOS_SendAsynMsg::pReceiver == NULL \n");
        return -1;
    }

    tMsg.wDataLen = (wMsgLen > MAX_OAMMSG_LEN) ? MAX_OAMMSG_LEN : wMsgLen;
    tMsg.dwMsgId = dwMsgId;
    memcpy(tMsg.aucMsg,  pucData, tMsg.wDataLen);

    pDestJob = GetJobInfoByJno(pDest->dwJno);
    if (pDestJob == NULL)
    {
        printf("XOS_SendAsynMsg::pDestJob == NULL\n");
        return -2;
    }

    msglen = tMsg.wDataLen + (sizeof(tMsg) - sizeof(tMsg.aucMsg));
    iRet = mq_send(pDestJob->mq_Recv, (char *)&tMsg, msglen, MQ_MSG_PRIV);
    if (iRet < 0)
    {
        printf("call mq_send failed errno = %d \n", errno);
    }
	
    return iRet;
}

XOS_STATUS XOS_SendAsynMsgNJ(WORD32   dwMsgId,
                             BYTE     *pucData,
                             WORD16   wMsgLen,
                             BYTE     ucVer,
                             BYTE     ucPriority,
                             void     *pSender,
                             void     *pReceiver
                             )
{
    return XOS_SendAsynMsg(dwMsgId, pucData, wMsgLen, ucVer, ucPriority, pReceiver);
}

XOS_STATUS XOS_GetMsgDataLen(WORD32 dwJno, WORD16 *pwMsgLen)
{
    T_CliJobInfo *pDestJob = GetJobInfoByJno(dwJno);
    if (pDestJob == NULL)
    {
        printf("XOS_GetMsgDataLen::pDestJob == NULL\n");
        return XOS_FAILED;
    }

    *pwMsgLen = pDestJob->dwCurMsgLen;
	
    return XOS_SUCCESS;
}

XOS_STATUS  XOS_Sender(void *pSender){return XOS_SUCCESS;}

int  XOS_Msend(WORD32    dwMsgId,
                      BYTE      *pucData,
                      WORD16    wMsgLen,
                      BYTE      ucVer,
                      BYTE      ucPriority,
                      void      *pReceiver)
{return 1;}

int XOS_SysLog(BYTE ucLevel,CHAR* pcFmt, ...)
{
    if (ucLevel < g_iSyslogLevel)
    {
        return 0;
    }
    va_list args; 
    int i; 
    char printbuf[1024];
     
    va_start(args, pcFmt); 
    write(1,printbuf,i=vsprintf(printbuf, pcFmt, args)); 
    va_end(args); 
    printf("\n");
    return i; 
}

int XOS_GetBoardMSState() {return BOARD_MASTER;}

void XOS_SetNextState(WORD16 wJno, WORD16 wState)
{ 
    T_CliJobInfo *pJob = GetJobInfoByJno(wJno);
    if (pJob != NULL)
    {
        pJob->wState = wState;
    }
}

void XOS_SetDefaultNextState(){};

WORD16 XOS_GetCurState(WORD32 dwJno)
{
    T_CliJobInfo *pJob = GetJobInfoByJno(dwJno);
    if (pJob == NULL)
    {
        printf("XOS_GetCurState::pJob == NULL\n");
        return XOS_FAILED;
    }
	
    return pJob->wState;
}


WORD16  XOS_InvertWord16(WORD16 wWord16) 
{
    return XOS_INVERT_WORD16(wWord16);
}

WORD32  XOS_InvertWord32(WORD32 dwWord32)
{
    return XOS_INVERT_WORD32(dwWord32);
}

WORD64  XOS_InvertWord64(WORD64 qwWord64)
{
    return XOS_INVERT_WORD64(qwWord64);
}

XOS_STATUS XOS_GetSelfJID(void *pvJid) { return XOS_SUCCESS;}
WORD32     XOS_ConstructJNO(WORD16 wJobType, WORD16 wInstanceNo) {return wJobType;}

XOS_STATUS XOS_CreateDirectoryEx(CHAR *pcDirName) 
{
    int iRet = 0;
    struct stat statbuf;
    if ( 0 != stat(pcDirName, &statbuf))   //If failed to get the status of this directory
    {
        if (ENOENT == errno) //If folder  not exist
        {
            iRet = mkdir(pcDirName, S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH|S_IXOTH);
        }
    }

    return  (iRet <  0) ? XOS_FAILED : XOS_SUCCESS;
}

XOS_TIMER_ID XOS_SetLoopTimer(WORD16 wJno,WORD32 dwTimerNo, SWORD32 sdwDuration, WORD32 dwParam)
{
    return SetTimer(dwTimerNo, wJno, LOOP_TIMER, sdwDuration, dwParam);
}

XOS_TIMER_ID  XOS_SetRelativeTimer(WORD16 wJno, WORD32 dwTimerNo, SWORD32 sdwDuration, WORD32 dwParam)
{
    return SetTimer(dwTimerNo, wJno, RELATIVE_TIMER, sdwDuration, dwParam);
}

XOS_STATUS XOS_KillTimer(WORD16 wJno, WORD32 dwTimerNo,WORD32 dwParam)
{
    KillTimer(wJno, dwTimerNo,dwParam);
    return XOS_SUCCESS;
}

XOS_STATUS XOS_KillTimerByTimerId(XOS_TIMER_ID dwTimerId)
{
    KillTimerByTimerId(dwTimerId);
    return XOS_SUCCESS;
}

WORD32 XOS_GetConstParaOfCurTimer(LPVOID pMsgPara)
{
    return *((WORD32*)pMsgPara);
}

void XOS_GetCurrentTime(T_Time *ptTime)
{
    if (ptTime)
    {
        time((time_t *)&(ptTime->dwSecond)); 
    }
}

XOS_STATUS XOS_Delay(WORD32 dwTimeout)
{
    struct timespec delay;
    delay.tv_sec    = dwTimeout/MSINSECOND;
    delay.tv_nsec   = (dwTimeout - delay.tv_sec*MSINSECOND)*1000*1000;
    if (dwTimeout == 0)
    {
        delay.tv_nsec  = 1;
    }
    nanosleep(&delay, NULL);
    return XOS_SUCCESS;
}
XOS_STATUS XOS_GetSysClock(T_SysSoftClock *ptSysClock)
{
    time_t now;

    if (!ptSysClock)
    {
        return XOS_FAILED;
    }

    now = time(0);
    struct tm *tnow = localtime(&now);
    if (!tnow)
    {
        return XOS_FAILED;
    }

    ptSysClock->wSysYear = 1900+tnow->tm_year;
    ptSysClock->ucSysMon = tnow->tm_mon+1;
    ptSysClock->ucSysDay = tnow->tm_mday;
    ptSysClock->ucSysHour = tnow->tm_hour;
    ptSysClock->ucSysMin = tnow->tm_min;
    ptSysClock->ucSysSec = tnow->tm_sec;

    return XOS_SUCCESS;
}

XOS_STATUS XOS_TimeToClock(T_Time *ptSecMillisec,T_SysSoftClock *ptClock){return XOS_SUCCESS;}

XOS_STATUS XOS_GetBoardStyle(BYTE*  ucBoardStyle){return XOS_SUCCESS;}
WORD32 XOS_CreateProcessSem(WORD32 index,WORD32 dwInitVal,WORD32 dwFlag ){return 1;}
XOS_STATUS XOS_CreateShmByIndex(WORD32   index, WORD32 tShareSize, WORD32 dwFlag, BYTE **ppucShareBuf){return XOS_SUCCESS;}
BOOLEAN XOS_ProcessSemP(WORD32 semid,WORD32 dwTimeOut){return 1;}
BOOLEAN XOS_ProcessSemV(WORD32 semid){return 1;}
XOS_STATUS XOS_OpenFile(CHAR* pcFileName,CHAR * sdwFlag,XOS_FD *ptFd)
{
    *ptFd = fopen(pcFileName, sdwFlag);

    if (*ptFd == NULL)
    {
        printf("XOS_OpenFile errno: %d \n", errno);
        return XOS_FAILED;
    }
    else
    {
        return XOS_SUCCESS;
    }
}
XOS_STATUS XOS_WriteFile(XOS_FD sdwFd, void *pBuffer, SWORD32 sdwCount, SWORD32 *psdwCountBeWrite)
{
     *psdwCountBeWrite = fwrite(pBuffer, 1, sdwCount,sdwFd); 

    if (*psdwCountBeWrite < 0)
    {
        return XOS_FAILED;
    }
    else
    {
        return XOS_SUCCESS;
    }
}
XOS_STATUS XOS_GetFileLength(CHAR *pcFileName,SWORD32 *psdwLen )
{
    struct stat buf;

    if (!pcFileName ||
        !psdwLen)
    {
        return XOS_FAILED;
    }
	
    if(stat(pcFileName, &buf)<0)
    {
        return XOS_FAILED;
    }

    *psdwLen = (unsigned long)buf.st_size;
    return XOS_SUCCESS;
}

XOS_STATUS XOS_ReadFile(XOS_FD sdwFd, void *pBuffer, SWORD32 sdwCount, SWORD32 *psdwCountBeRead)
{
    *psdwCountBeRead = fread(pBuffer, 1, sdwCount,sdwFd);
	
    if (*psdwCountBeRead < 0)
    {
        return XOS_FAILED;
    }
    else
    {
        return XOS_SUCCESS;
    }
}

XOS_STATUS XOS_SeekFile(XOS_FD sdwFd, SWORD32 sdwOrigin, SWORD32 sdwOffset)
{
    int iRet = fseek(sdwFd, sdwOffset, sdwOrigin);
    if (iRet < 0)
    {
        return XOS_FAILED;
    }
    else
    {
        return XOS_SUCCESS;
    }

}

XOS_STATUS XOS_RenameFile(CHAR *pcOldFileName, CHAR *pcNewFileName)
{
    rename(pcOldFileName, pcNewFileName);
    return XOS_SUCCESS;
}

XOS_STATUS XOS_CloseFile(XOS_FD sdwFd)
{
    int iRet = fclose(sdwFd);

    if (iRet < 0)
    {
        return XOS_SUCCESS;
    }
    else
    {
        return XOS_FAILED;
    }
}

XOS_STATUS XOS_DeleteFile(CHAR *pcFileName)
{
    unlink(pcFileName);
    return XOS_SUCCESS;
}

XOS_STATUS XOS_GetBoardPosition(BYTE *pucBoardPosition){return XOS_SUCCESS;}
XOS_STATUS XOS_OpenDir(CHAR *pcDirName,XOS_DIR *ptDir){return XOS_SUCCESS;}
XOS_STATUS XOS_CreateDirectory(CHAR *pcDirName){return XOS_SUCCESS;}
XOS_STATUS XOS_CloseDir(XOS_DIR sdwDirIndex){return XOS_SUCCESS;}
XOS_STATUS XOS_TakeOverConsole(T_ShellCallBack *ptShellCallBack,BOOLEAN bEnableUshell){return XOS_SUCCESS;}

XOS_TASK_T XOS_StartTask(
                     CHAR           *pucName,
                     WORD16         wPriority,
                     WORD32         dwStacksize,
                     SWORD32        sdwOptions,
                     TaskEntryProto tTaskEntry,
                     WORDPTR        ptTaskPara1
                     )
{
    XOS_TASK_T tTaskId = INVALID_SYS_TASKID;
    pthread_attr_t        attr;
    pthread_t             threadID;
    struct sched_param    scheparam;
    INT              errcode   = 0;

    pthread_attr_init(&attr);

    /* 新线程进程中其他线程脱离同步，在退出时自行释放所占用的资源 */
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
    if( pthread_attr_setinheritsched(&attr,PTHREAD_EXPLICIT_SCHED) != 0 )
    {
        printf("Vos_CreateAffinityTask: set attr failed!\n");
        return INVALID_SYS_TASKID;
    }
    
    pthread_attr_setschedpolicy(&attr,SCHED_FIFO);
    pthread_attr_getschedparam(&attr,&scheparam);
    scheparam.__sched_priority  = wPriority;
    pthread_attr_setschedparam(&attr,&scheparam);
    pthread_attr_setstacksize(&attr,dwStacksize);

    errcode = pthread_create(&threadID, &attr, (void* (*)(void*))tTaskEntry, (void *)ptTaskPara1);

    if (0 != errcode)
    {
        printf("Vos_CreateAffinityTask: create thread<%s> failed!errcode:%d\n",pucName,errcode);
    }
    else
    {
        tTaskId  = threadID;
    }

    return tTaskId;
}

BOOLEAN XOS_DeleteTask(XOS_TASK_T tThreadId)
{
#if 0
    T_ThreadInfo* pThreadInfo = NULL;

    /* 删除threadtable中的对应项 */
    pThreadInfo = Vos_GetThreadInfo(tThreadId);
    if (NULL != pThreadInfo)
    {
        if (pthread_mutex_lock(&g_threadTableMutex)!=0)
        {
            R_Printf(OSS_ERR"pthread_mutex_lock return -1!\n");
            R_PERROR(pthread_mutex_lock);
            return FALSE;
        }

        pThreadInfo->flag       = 0;
        pThreadInfo->thread_id  = 0;
        pThreadInfo->thread_tid = 0;
        pThreadInfo->IsSuspend  = FALSE;
	 memset(&(pThreadInfo->tTskMoniInfo),  0, sizeof(pThreadInfo->tTskMoniInfo));
		g_dwTaskCount--;
        if (pthread_mutex_unlock(&g_threadTableMutex)!=0)
        {
            R_Printf(OSS_ERR"pthread_mutex_unlock return -1!\n");
            R_PERROR(pthread_mutex_unlock);
            return FALSE;
        }
    }
    else
    {
        R_Printf(OSS_ERR"Vos_GetThreadInfo return NULL!\n");
        return FALSE;
    }
#endif
    if (tThreadId == pthread_self())
    {
        pthread_exit(0);
        return TRUE;
    }
    else if (0 != pthread_cancel(tThreadId))
    {
        return FALSE;
    }

    return TRUE;
}

BOOLEAN XOS_CreateSemaphore(
                            WORD32 dwInitVal,
                            WORD32 dwMaxVal,
                            WORD32 dwFlag,
                            WORD32 *pdwSemID
                            )

{
    WORD32           dwLoop   = 0;
    WORD32           dwSemId  = 0;
    T_Obj_MutexType  *ptMutex = NULL;

    if (           (SYNC_SEM_STYLE != dwFlag)
               && (MUTEX_STYLE != dwFlag)
               )
    {
        assert((SYNC_SEM_STYLE == dwFlag)
               || (MUTEX_STYLE == dwFlag) );
        printf("Vos_CreateSemaphore: flag error!\n");
        return FALSE;
    }
    if (NULL == pdwSemID)
    {
        assert (NULL != pdwSemID);
        return FALSE;
    }
    *pdwSemID=0;
    /* 访问g_atMutex时增加互斥保护 */
    if (pthread_mutex_lock(&s_tMutexLock)!=0)
    {

        printf("Vos_CreateSemaphore:mutex_lock failed!\n");
        return FALSE;
    }
    /* 查找空闲信号量控制块 */
    for (dwLoop = 0; dwLoop < g_dwMaxSem; dwLoop ++)
    {
        if (g_atMutex[dwLoop].ucUsed == VOS_SEM_CTL_NO_USED)
        {
            /* 找到可用的信号量控制块 */
            dwSemId  =  dwLoop;
            ptMutex  =  &g_atMutex[dwLoop];
            break;
        }
    }

    if (g_dwMaxSem == dwLoop)
    {
        /* 说明没有找到可用的信号量控制块 */
        pthread_mutex_unlock(&s_tMutexLock);
        printf("Vos_CreateSemaphore: No more mutex  control block is available!\n");
        return FALSE;
    }

    /*make sure the procedure self mutex */


    switch (dwFlag)
    {
    case SYNC_SEM_STYLE:
        if (0 == sem_init(&(ptMutex->obj.sem),0,0))
        {
            *pdwSemID = dwLoop+1;
        }
        else
        {
            pthread_mutex_unlock(&s_tMutexLock);
            return FALSE;
        }
        break;
    case MUTEX_STYLE:
    {
        /* lizl, 让互斥锁可重入 */
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);
        pthread_mutexattr_setrobust_np(&attr, PTHREAD_MUTEX_ROBUST_NP);
	/*成研对优先级反转支持还有问题，暂时去掉*/
	/*pthread_mutexattr_setprotocol(&attr,PTHREAD_PRIO_INHERIT);*/

        if (0 == pthread_mutex_init(&(ptMutex->obj.mutex),&attr))
        {
            pthread_mutexattr_destroy(&attr);
            *pdwSemID   = dwLoop+1;
        }
        else
        {
            pthread_mutexattr_destroy(&attr);
            pthread_mutex_unlock(&s_tMutexLock);
            return FALSE;
        }
    }
    break;
    default:
        *pdwSemID = 0;
        pthread_mutex_unlock(&s_tMutexLock);
        printf("Vos_CreateSemaphore: Flag of semaphore is invalid!\n");
        return FALSE;
        break;
    }

//	ptMutex->dwCreate = t_TaskInfo.dwTaskId;
    ptMutex->dwFlag     = dwFlag;
    ptMutex->ucUsed  = VOS_SEM_CTL_USED;
	g_dwSemCount++;
    pthread_mutex_unlock(&s_tMutexLock);

    return TRUE;
}

BOOLEAN XOS_SemaphoreP(WORD32 dwSemID, WORD32 dwTimeOut)
{

    WORD16 ret = 0;
    if ((dwSemID > g_dwMaxSem)||(dwSemID ==0))
    {
        assert(FALSE);
        return FALSE;
    }
    dwSemID--;
    T_Obj_MutexType  *ptMutex = &g_atMutex[dwSemID];

    if (ptMutex->ucUsed  != VOS_SEM_CTL_USED)
    {
        return FALSE;
    }

//	t_TaskInfo.dwSemID=dwSemID+1; 
    switch (ptMutex->dwFlag)
    {
    case SYNC_SEM_STYLE:
        /* 如果阻塞时被信号打断，重新执行操作。张文剑 */
        /* 如果阻塞时被信号打断，重新执行操作。张文剑 */
        if (WAIT_FOREVER == dwTimeOut)
        {
            /* 如果阻塞时被信号打断，重新执行操作。张文剑 */
            do
            {
                ret =  sem_wait(&(ptMutex->obj.sem));
            }
            while ((ret != 0) && (errno == EINTR));
        }
        else if (NO_WAIT == dwTimeOut)
        {
            do
            {
                ret =  sem_trywait(&(ptMutex->obj.sem));
            }
            while ((ret != 0) && (errno == EINTR));
        }
        else
        {
            /* errno==EINTR屏蔽其他信号事件引起的等待中断 罗斌 20081115 */
            struct timespec ts;
            WORD32   dwTmpnsec = 0; 
	
            clock_gettime(CLOCK_REALTIME, &ts );           /* 获取当前时间       */
            ts.tv_sec += (dwTimeOut / 1000 );              /* 加上等待时间的秒数 */
	     dwTmpnsec =  (dwTimeOut%1000)*1000*1000 + ts.tv_nsec ;
            ts.tv_sec += dwTmpnsec/(1000*1000*1000);  
            ts.tv_nsec = dwTmpnsec%(1000*1000*1000);   /* 加上等待时间纳秒数 */
            do
            {
                ret =  sem_timedwait(&(ptMutex->obj.sem), &ts);
            }while ((ret != 0) && (errno == EINTR));

        }
//		t_TaskInfo.dwSemID=0;
        if (ret != 0)
        {
            ptMutex->iErrno=errno;            
		   return	FALSE;
        }

        break;
    case MUTEX_STYLE:
        if (WAIT_FOREVER == dwTimeOut)
        {
            ret =  pthread_mutex_lock(&(ptMutex->obj.mutex));
        }
        else if (NO_WAIT == dwTimeOut)
        {
            ret =  pthread_mutex_trylock(&(ptMutex->obj.mutex));
        }
        else
        {
            struct timespec ts;
	     WORD32   dwTmpnsec = 0; 
		 
            clock_gettime(CLOCK_REALTIME, &ts );           /* 获取当前时间       */
            ts.tv_sec += (dwTimeOut / 1000 );              /* 加上等待时间的秒数 */
	     dwTmpnsec =  (dwTimeOut%1000)*1000*1000 + ts.tv_nsec ;
	     ts.tv_sec += dwTmpnsec/(1000*1000*1000);  
            ts.tv_nsec =  dwTmpnsec % (1000 * 1000*1000);     /* 加上等待时间纳秒数 */
            ret =  pthread_mutex_timedlock(&(ptMutex->obj.mutex), &ts);
        }
//		t_TaskInfo.dwSemID=0;
		if(ret==EOWNERDEAD)
		{			
			pthread_mutex_consistent_np(&(ptMutex->obj.mutex));
			
		}
        else if (ret != 0)
        {
        	ptMutex->iErrno=ret;
            return   FALSE;
        }

        /* 对互斥信号量操作进行记录 */
 //       RecordSemaphoreInfo(ptMutex);
        break;
    default:
        printf("Vos_SemaphoreP: Flag of semaphore is invalid!\n");
        return FALSE;
        break;
    }

    return TRUE;
}
BOOLEAN XOS_SemaphoreV(WORD32 dwSemID)
{
    INT ret=0;
    if ((dwSemID > g_dwMaxSem)||(dwSemID ==0))
    {
        assert(FALSE);
        return FALSE;
    }
	dwSemID--;
    T_Obj_MutexType  *ptMutex = &g_atMutex[dwSemID];

    if (ptMutex->ucUsed  != VOS_SEM_CTL_USED)
    {
        return FALSE;
    }
    switch (ptMutex->dwFlag)
    {
    case SYNC_SEM_STYLE:
        if (0 != (ret=sem_post(&(ptMutex->obj.sem))))
        {
            ptMutex->iErrno=ret;
            printf("sem_post error \n");
            return FALSE;
        }
        break;
    case MUTEX_STYLE:
        if (0!=(ret=pthread_mutex_unlock(&(ptMutex->obj.mutex))))
        {
            ptMutex->iErrno=ret;
            printf("pthread_mutex_unlock error \n");
			return FALSE;
        }
		if(ptMutex->obj.mutex.__data.__owner==0)
		{
	        ptMutex->dwPtick = 0; /* 本信号量已被释放 */
	        ptMutex->SvrId = 0;
	        ptMutex->dwMsgId = 0;
	        ptMutex->dwTaskId = 0;
		}
        break;
    default:
        printf("Vos_SemaphoreV: Flag of semaphore is invalid!\n");
        return FALSE;
        break;
    }
    return TRUE;
}

BYTE *XOS_GetUB(WORD32 dwSize)
{
    return (BYTE *)malloc(dwSize);
}

XOS_STATUS XOS_RetUB(BYTE *pucBuf)
{
    free(pucBuf);
        pucBuf=NULL;
        return XOS_SUCCESS; 
    }


#define   BUF_SIZE                (WORD32)100      /* 定义存储输出数字（十进制、八进制等）字符串的缓冲区大小 */
#define   DECIMAL                 10
#define   OCTAL                   8
#define   HEX                     16
#define   HEXPREFIX              2
#define   TODIGIT(c)              ((c) - '0')
#define   TOCHAR(n)               ((n) + '0')
#define   ISDIGIT(c)              ((WORD16)TODIGIT(c) <= 9)
static CHAR BIG_HEX_DIGIT[]     = "0123456789ABCDEF";
static CHAR LIT_HEX_DIGIT[]     = "0123456789abcdef";
static CHAR NULLSTRING[]        = "(null)";

#define PRINT_F_MINUS           (1 << 0)
#define PRINT_F_PLUS            (1 << 1)
#define PRINT_F_SPACE           (1 << 2)
#define PRINT_F_NUM             (1 << 3)
#define PRINT_F_ZERO            (1 << 4)
#define PRINT_F_QUOTE           (1 << 5)
#define PRINT_F_UP              (1 << 6)
#define PRINT_F_UNSIGNED        (1 << 7)
#define PRINT_F_TYPE_G          (1 << 8)
#define PRINT_F_TYPE_E          (1 << 9)


#define OUTCHAR(pcStr, dwLen, dwSize, uChar) \
do {              \
    if ((dwLen) < (dwSize)) \
    { \
        (pcStr)[(dwLen)] = (uChar); \
       (dwLen)++; \
    } \
    else \
    { \
        return; \
    } \
} while (0)
/* 支持浮点数所加 */
#define   MAX_CONVERT_LENGTH      43
#define   ISNAN(x) (x != x)
#define   ISINF(x) (x != 0.0 && x + x == x)


/**********************************************************************
* 函数名称：INT  R_GetExponent(LDOUBLE ldFltValue)
* 功能描述：获得小数的指数部分
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：无；
* 其它说明：
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008/8/1    V1.0    wtl      创建
************************************************************************/
INT  R_GetExponent(LDOUBLE ldFltValue)
{
    LDOUBLE ldTmp = (0.0 < ldFltValue) ? ldFltValue : -ldFltValue;
    INT iExponent = 0;

    while (ldTmp < 1.0 && ldTmp > 0.0 && --iExponent > -99)
    {
        ldTmp *= 10;
    }

    while (ldTmp >= 10.0 && ++iExponent < 99)
    {
        ldTmp /= 10;
    }

    return iExponent;
}

/**********************************************************************
* 函数名称：LDOUBLE R_Pow10(INT iExponent)
* 功能描述：获得iExponent的幂
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：无；
* 其它说明：
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008/8/1    V1.0    wtl      创建
************************************************************************/
LDOUBLE R_Pow10(INT iExponent)
{
    LDOUBLE lsResult = 1;

    while (iExponent > 0)
    {
        lsResult *= 10;
        iExponent--;
    }

    while (iExponent < 0)
    {
        lsResult /= 10;
        iExponent++;
    }

    return lsResult;
}

/**********************************************************************
* 函数名称：R_Flt2Int
* 功能描述：获得小数的整数部分
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：无；
* 其它说明：
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008/8/1    V1.0    wtl      创建
************************************************************************/
WORDPTR R_Flt2Int(LDOUBLE ldFltValu)
{
    WORDPTR dwResult = (WORDPTR)ldFltValu;
    return (dwResult <= (WORDPTR)ldFltValu) ? dwResult : dwResult - 1;
}

/**********************************************************************
* 函数名称：R_Round
* 功能描述：获得小数的整数部分
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：无；
* 其它说明：
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008/8/1    V1.0    wtl      创建
************************************************************************/
WORDPTR R_Round(LDOUBLE ldFltValu)
{
    WORDPTR dwIntPart = R_Flt2Int(ldFltValu);

    return ((ldFltValu -= dwIntPart) < 0.5) ? dwIntPart : dwIntPart + 1;
}

/**********************************************************************
* 函数名称：R_ConVert
* 功能描述：数字转化成字符串
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：无；
* 其它说明：
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008/8/1    V1.0    wtl      创建
************************************************************************/
INT R_ConVert(WORDPTR dwValue,
              CHAR    *pcBuf,
              WORD32  dwSize,
              WORD32  dwBase,
              BOOLEAN bCaps)
{
    CHAR *pcDigits = (CHAR *)(bCaps ? "0123456789ABCDEF" : "0123456789abcdef");
    WORD32  iPos = 0;

    do
    {
        pcBuf[iPos++] = pcDigits[dwValue % dwBase];
        dwValue /= dwBase;
    } while (dwValue != 0 && iPos < dwSize);

    return iPos;
}


void R_FmtFlt2Str(CHAR    *pcStrBuf,
                  WORD32  *dwLen,
                  WORD32  dwSize,
                  LDOUBLE ldFltValue,
                  WORD32  dwPrecision,
                  WORD32  dwWidth,
                  WORD32  dwFlags)
{
    INT  iEpos          = 0;
    INT  iFpos          = 0;
    INT  iIpos          = 0;
    INT  iExponent      = 0;
    INT  iOmitZeros     = 0;
    INT  iEmitpoint     = 0;
    INT  iEstyle        = 0;
    INT  iLeadFracZeros = 0;
    INT  iOmitCnt       = 0;
    INT  iPadLen        = 0;
    CHAR *pcInfnan      = NULL;
    CHAR cSign          = 0;
    CHAR cEsign         = 0;
    CHAR acIConvert[MAX_CONVERT_LENGTH];
    CHAR acFConvert[MAX_CONVERT_LENGTH];
    CHAR acEConvert[4];

    LDOUBLE lduFvalue   = 0.0;

    WORDPTR dwIntPart   = 0;
    WORDPTR dwFracPart  = 0;
    WORDPTR dwMask      = 0;

    if ((WORD32)-1 == dwPrecision)
    {
        dwPrecision = 6;
    }

    if (0.0 > ldFltValue)
    {
        cSign = '-';
    }
    else if (dwFlags & PRINT_F_PLUS)
    {
        cSign = '+';
    }
    else if (dwFlags & PRINT_F_SPACE)
    {
        cSign = ' ';
    }

    /* 这个检查还看不懂什么意思 */
    if (ISNAN(ldFltValue))
    {
        pcInfnan = (CHAR *)(dwFlags & PRINT_F_UP) ? (CHAR *)"NAN" : (CHAR *)"nan";
    }
    else if (ISINF(ldFltValue))
    {
        pcInfnan = (CHAR *)(dwFlags & PRINT_F_UP) ? (CHAR *)"INF" : (CHAR *)"inf";
    }

    /* 这中情况下输出字符串,所有的格式修饰符号不起作用 */
    if (NULL != pcInfnan)
    {
        while (*pcInfnan != '\0')
        {
            OUTCHAR(pcStrBuf, *dwLen, dwSize, *pcInfnan++);
        }
        return;
    }

    /* 指数的处理 */
    if (dwFlags & PRINT_F_TYPE_E ||
        dwFlags & PRINT_F_TYPE_G)
    {
        if (dwFlags & PRINT_F_TYPE_G)
        {
            dwPrecision--;
            if (!(dwFlags & PRINT_F_NUM))
            {
                iOmitZeros = 1;
            }
        }
        iExponent = R_GetExponent(ldFltValue);
        iEstyle = 1;
    }

again:

    /* 我们支持的最大精度为20位 */
    if (20 < dwPrecision)
    {
        dwPrecision = 20;
    }

    lduFvalue = (0.0 < ldFltValue) ? ldFltValue : -ldFltValue;
    if (iEstyle)
    {
        lduFvalue /= R_Pow10(iExponent );
    }

    /* 计算出整数部分 */
    dwIntPart = R_Flt2Int(lduFvalue);

    /* 计算出小数部分 */
    dwMask     = (WORDPTR)R_Pow10(dwPrecision);
    dwFracPart = R_Round((lduFvalue - dwIntPart) * dwMask);
    if (dwFracPart >= dwMask)
    {
        dwIntPart++;
        dwFracPart = 0;
        if (iEstyle &&
            (10 == dwIntPart))
        {
            dwIntPart = 1;
            iExponent++;
        }
    }

    /* C99规定 g 和 G 的处理 */
    if ((dwFlags & PRINT_F_TYPE_G) &&
        iEstyle &&
        (dwPrecision + 1 > (WORD32)iExponent) &&
        (iExponent >= -4))
    {
        dwPrecision -= iExponent;
        iEstyle = 0;
        goto again;
    }

    /* 指数形式的处理 */
    if (iEstyle)
    {
        if (iExponent < 0)
        {
            iExponent = -iExponent;
            cEsign = '-';
        }
        else
        {
            cEsign = '+';
        }

        iEpos = R_ConVert(iExponent, acEConvert, 2, 10, 0);
        if (iEpos == 1)
        {
            acEConvert[iEpos++] = '0';
        }
        acEConvert[iEpos++] = cEsign;
        acEConvert[iEpos++] = ((dwFlags & PRINT_F_UP) ? 'E' : 'e');
    }

    /* 整数部分的转换 */
    iIpos = R_ConVert(dwIntPart,
                      acIConvert,
                      sizeof(acIConvert),
                      10,
                      0);

    /* 小数部分的转换 */
    if (0 != dwFracPart)
    {
        iFpos = R_ConVert(dwFracPart,
                          acFConvert,
                          sizeof(acFConvert),
                          10,
                          0);
    }

    /* 前面不足的地方补充0 */
    iLeadFracZeros = dwPrecision - iFpos;

    /* 小数后面不能补充0 */
    if (iOmitZeros)
    {
        if (0 < iFpos)
        {
            while (iOmitCnt < iFpos && acFConvert[iOmitCnt] == '0')
            {
                iOmitCnt++;
            }
        }
        else
        {
            iOmitCnt = dwPrecision;
            iLeadFracZeros = 0;
        }
        dwPrecision -= iOmitCnt;
    }

    /* 是否要打印小数点 */
    if (0 < dwPrecision || dwFlags & PRINT_F_NUM)
    {
        iEmitpoint = 1;
    }

    /* 计算要补齐的字符的数量 */
    iPadLen = dwWidth - iIpos - iEpos - dwPrecision -
              (iEmitpoint ? 1 : 0) - ((cSign != 0) ? 1 : 0);

    if (0 > iPadLen)
    {
        iPadLen = 0;
    }

    if (dwFlags & PRINT_F_MINUS)
    {   /* 左对齐 */
        iPadLen = -iPadLen;
    }
    else if (dwFlags & PRINT_F_ZERO && iPadLen > 0)
    {
        if (cSign != 0)
        {
            OUTCHAR(pcStrBuf, *dwLen, dwSize, cSign);
            cSign = 0;
        }

        while (0 < iPadLen)
        {
            OUTCHAR(pcStrBuf, *dwLen, dwSize, '0');
            iPadLen--;
        }
    }

    /* 左对齐的处理 */
    while (0 < iPadLen)
    {
        OUTCHAR(pcStrBuf, *dwLen, dwSize, ' ');
        iPadLen--;
    }

    /* 符号的输出 */
    if (cSign != 0)
    {
        OUTCHAR(pcStrBuf, *dwLen, dwSize, cSign);
    }

    /* 输出整数部分 */
    while (iIpos > 0)
    {
        iIpos--;
        OUTCHAR(pcStrBuf, *dwLen, dwSize, acIConvert[iIpos]);
    }

    /* 输出小数点部分 */
    if (iEmitpoint)
    {
        OUTCHAR(pcStrBuf, *dwLen, dwSize, '.');
    }

    /* 输出小数部分前面的0 */
    while (iLeadFracZeros > 0)
    {
        OUTCHAR(pcStrBuf, *dwLen, dwSize, '0');
        iLeadFracZeros--;
    }

    /* 输出小数部分 */
    while (iFpos > iOmitCnt)
    {
        iFpos--;
        OUTCHAR(pcStrBuf, *dwLen, dwSize, acFConvert[iFpos]);
    }

    /* 指数部分 */
    while (iEpos > 0)
    {
        iEpos--;
        OUTCHAR(pcStrBuf, *dwLen, dwSize, acEConvert[iEpos]);
    }

    /* 右对齐处理 */
    while (iPadLen < 0)
    {
        OUTCHAR(pcStrBuf, *dwLen, dwSize, ' ');
        iPadLen++;
    }
}

SWORD32 log_vsnprintf(CHAR        *pcBuffer,
                    WORD32      dwMaxCount,
                    const CHAR  *pcParamFormat,
                    va_list     vaList)
{
    SWORD16     swCh;               /* 取自pcParamFormat的字符 */
    SWORD32     n;                  /* 处理整数 */
    CHAR        *pch;               /* 处理字符的指针 */
    SWORD32     sdwWdth;            /* %格式的宽度 */
    CHAR        cSgn;               /* 符号前缀(' ', '+', '-', \0) */
    unsigned    long long   ptVal;  /* 整数参数 %[diouxX] */
    SWORD32     sdwprec;            /* 格式精度(%.3d) */
    SWORD32     sdwSize;            /* 要输出参数的长度 */
    SWORD32     sdwExSize;          /* 扩展后输出参数的长度（含符号） */
    SWORD32     sdwRealSize;        /* 参数的实际输出长度 */
    BOOLEAN     bLong;              /* 长整型标志 */
    BOOLEAN     bLongLong;          /* 长整型标记 */
    BOOLEAN     bShort;             /* 短整型标志 */
    BOOLEAN     bPrefix;            /* 前缀标志 */
    BOOLEAN     bLAdj;              /* 左对齐标志 */
    BOOLEAN     bZeroPad;           /* 填0标志 */
    BOOLEAN     bHexPrefix;         /* 16进制前缀标志（0x或0X） */
    CHAR        acBuf[BUF_SIZE];    /* 存放整数参数的缓存 */
    CHAR        *pcxdigits;         /* 输出为16进制时的转换数字串 */
    SWORD32     sdwRet  = 0;        /* 输出计数 */
    SWORD16     swBase  = DECIMAL;  /* 整形参数的输出的进制 */
    WORD32      dwCount = 0;        /* 统计输出缓冲区当前的字节数 */
    CHAR        *pcOut  = pcBuffer; /* 输出缓冲区指针 */
    CHAR        *pcTemp = NULL;
    /* 支持浮点打印所加 */
    WORD32      dwFltFlg   = 0;        /* 打印标志 */
    LDOUBLE     ldFltValue = 0;
    BOOLEAN     bLDouble   = FALSE;    /* 是否是long double类型 */

    if ((NULL == pcBuffer) || (NULL == pcParamFormat))
    {
        return -1;
    }

    if (0 == dwMaxCount)
    {
        return -1;
    }
    else
    {
        dwMaxCount--;               /* 为字符串结束符保留一个字节 */
    }

    pcxdigits = LIT_HEX_DIGIT;

    for(;;)        /* 扫描格式串中的%字符 */
    {
Nxttype:
        /* 搜索%字符 */
        for(pch = (CHAR *)pcParamFormat;
            ((swCh = *pcParamFormat) != '\0') && (swCh != '%');
            pcParamFormat++)
        ;

        /* 输出%前的其它字符 */
        if ((n = pcParamFormat - pch) != 0)
        {
            dwCount += n;
            if (dwCount > dwMaxCount)
            {
                /* 输出直至缓冲区满 */
                n -= (dwCount - dwMaxCount);
                bcopy(pch, pcOut, n);
                pcOut[n] = '\0';         /* 补上NULL结束符 */
                return -1;
            }

            bcopy(pch, pcOut, n);
            pcOut += n;
            sdwRet += n;
        }
        if ('%' == swCh)
        {/* 看看后面一个是不是%,如果是仍然要输出 */
           pcTemp = (CHAR *)(pcParamFormat + 1);
           if (*pcTemp == '%')
           {
               pcParamFormat+=2;
               bcopy("%", pcOut, 1);
               pcOut += 1;
               dwCount ++;
               sdwRet += 1;
               continue;
           }
        }
        if ('\0' == swCh)
        {
            *pcOut = '\0';               /* 补上NULL结束符 */
            return sdwRet;
        }

        pcParamFormat++;                /* 跳过 '%' */

        bLong       = FALSE;
        bLongLong   = FALSE;
        bShort      = FALSE;
        bPrefix     = FALSE;
        bLAdj       = FALSE;
        bZeroPad    = FALSE;
        bHexPrefix  = FALSE;

        /* 支持浮点格式的转换 */
        bLDouble    = FALSE;
        dwFltFlg    = 0;

        sdwprec     = -1;
        sdwWdth     = 0;
        cSgn        = '\0';      /* 输出格式为%di时的前缀 */

        /* 循环处理当前%的输出格式 */
        for (;;)
        {
            swCh = *pcParamFormat++;
            switch (swCh)
            {
            case ' ':
                if ('\0' == cSgn)
                {
                    cSgn = ' ';
                }
                dwFltFlg |= PRINT_F_SPACE;
                continue;

            case '#':
                bPrefix = TRUE;
                dwFltFlg |= PRINT_F_NUM;
                continue;

            case '*':
                if ((sdwWdth = va_arg(vaList, int)) >= 0)
                {
                    continue;
                }
                /* 宽度小于0，相当于有一个'-'的flag，Go on! */
                sdwWdth = -sdwWdth;

            case '-':
                bLAdj = TRUE;
                dwFltFlg |= PRINT_F_MINUS;
                continue;

            case '+':
                cSgn = '+';
                dwFltFlg |= PRINT_F_PLUS;
                continue;

            case '.':
                if ((swCh = *pcParamFormat++) == '*')
                {
                    n = va_arg(vaList, int);
                    sdwprec = (n < 0) ? -1 : n;
                    continue;
                }

                pch = (CHAR *)pcParamFormat;
                n = 0;
                while (ISDIGIT(swCh))
                {
                    n = DECIMAL * n + TODIGIT(swCh);
                    swCh = *pcParamFormat++;
                }

                if (pch != pcParamFormat)
                {
                    pcParamFormat--;       /* 回退指向刚才的非数字字符 */
                }
                sdwprec = n;
                continue;

            case '0':
                bZeroPad = TRUE;
                dwFltFlg |= PRINT_F_ZERO;
                continue;

            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                n = 0;
                do
                {
                    n = DECIMAL * n + TODIGIT(swCh);
                    swCh = *pcParamFormat++;
                } while (ISDIGIT(swCh));

                sdwWdth = n;
                pcParamFormat--;           /* 回退指向刚才的非数字字符 */
                continue;

            case 'h':
                bShort = TRUE;
                continue;

            case 'l':
                if(bLong == TRUE)
                {
                    bLongLong = TRUE;
                }
                bLong = TRUE;
                continue;

            case 'L':  /* 支持浮点打印:long double */
                bLDouble = TRUE;
                continue;

            case 'n':
                if (bLong)
                {
                    if(bLongLong)
                    {
                        *(long long*)(va_arg(vaList, long long*)) = sdwRet;
                    }
                    else
                    {
                        *(long *)(va_arg(vaList, long *)) = sdwRet;
                    }
                }
                else if (bShort)
                {
                    *(long *)(va_arg(vaList, short *)) = sdwRet;
                }
                else
                {
                    *(long *)(va_arg(vaList, int *)) = sdwRet;
                }
                goto Nxttype;   /* 返回搜索下一个 '%' */

            case 'c':
                *(pch = acBuf) = va_arg(vaList, int);
                sdwSize = 1;
                cSgn = '\0';
                goto PutFmtStr;

            case 's':
                if ((pch = (CHAR*)va_arg(vaList, char *)) == NULL)
                {
                    pch = NULLSTRING;
                }

                if (sdwprec >= 0)
                {
                    /* 如果指定了精度，则实际输出字符串的长度要修正 */
                    CHAR *pstr = (CHAR *)memchr(pch, 0, sdwprec);
                    if (NULL != pstr)
                    {
                        sdwSize = pstr - pch;
                        if (sdwSize > sdwprec)
                        {
                            sdwSize = sdwprec;
                        }
                    }
                    else
                    {
                        sdwSize = sdwprec;
                    }
                }
                else
                {
                    sdwSize = strlen(pch);
                }

                cSgn = '\0';
                goto PutFmtStr;

            case 'u':
                swBase = DECIMAL;
                cSgn = '\0';
                goto IntValue;

            case 'o':
                swBase = OCTAL;
                cSgn = '\0';
                goto IntValue;

            case 'X':
            case 'x':
                if ('X' == swCh)
                {
                    pcxdigits = BIG_HEX_DIGIT;
                }
                else
                {
                    pcxdigits = LIT_HEX_DIGIT;
                }

                swBase = HEX;
                cSgn = '\0';
                goto IntValue;

            case 'd':
            case 'i':
                /* 需要处理符号前缀cSgn */
                swBase = DECIMAL;
                goto IntValue;

            case 'p':
                ptVal = (WORDPTR)va_arg(vaList, void *);
                swBase = HEX;
                pcxdigits = LIT_HEX_DIGIT;
                bHexPrefix = TRUE;
                swCh = 'x';
                cSgn = '\0';
                goto number;

            case 'F':
                dwFltFlg |= PRINT_F_UP;
                /* FALLTHROUGH */
            case 'f':
                if (bLDouble)
                {        /* 暂时不支持 long double */
                    ldFltValue = va_arg(vaList, LDOUBLE);
                }
                else
                {
                    ldFltValue = va_arg(vaList, double);
                }

                R_FmtFlt2Str(pcBuffer,
                             &dwCount,
                             dwMaxCount,
                             ldFltValue,
                             sdwprec,
                             sdwWdth,
                             dwFltFlg);
                pcOut = pcBuffer + dwCount;
                sdwRet = dwCount;
                if (dwCount == dwMaxCount)
                {
                    *pcOut = '\0';
                    return -1;
                }
                goto Nxttype;   /* 返回搜索下一个 '%' */

            case 'G':           /* 这几个格式暂时不支持，等支持浮点了在打开 */
#if 0
                dwFltFlg |= PRINT_F_UP;
                /* FALLTHROUGH */
#endif
            case 'g':
#if 0
                dwFltFlg |= PRINT_F_TYPE_G;
                if (bLDouble)
                {        /* 暂时不支持 long double */
                    ldFltValue = va_arg(vaList, LDOUBLE);
                }
                else
                {
                    ldFltValue = va_arg(vaList, double);
                }

                /* C99规定  */
                if (0 == sdwprec)
                {
                    sdwprec = 1;
                }
                R_FmtFlt2Str(pcBuffer,
                             &dwCount,
                             dwMaxCount,
                             ldFltValue,
                             sdwprec,
                             sdwWdth,
                             dwFltFlg);
                pcOut = pcBuffer + dwCount;
                if (dwCount == dwMaxCount)
                {
                    *pcOut = '\0';
                    return -1;
                }
                goto Nxttype;   /* 返回搜索下一个 '%' */
#endif
            case 'E':
#if 0
                dwFltFlg |= PRINT_F_UP;
                /* FALLTHROUGH */
#endif
            case 'e':
#if 0
            dwFltFlg |= PRINT_F_TYPE_E;
            if (bLDouble)
            {        /* 暂时不支持 long double */
                ldFltValue = va_arg(vaList, LDOUBLE);
            }
            else
            {
                ldFltValue = va_arg(vaList, double);
            }

            R_FmtFlt2Str(pcBuffer,
                         &dwCount,
                         dwMaxCount,
                         ldFltValue,
                         sdwprec,
                         sdwWdth,
                         dwFltFlg);
            pcOut = pcBuffer + dwCount;
            if (dwCount == dwMaxCount)
            {
                *pcOut = '\0';
                return -1;
            }
            goto Nxttype;   /* 返回搜索下一个 '%' */
#endif
            default:
                if (swCh == '\0')
                {
                    *pcOut = '\0';           /* 补上NULL结束符 */
                    return sdwRet;
                }
                /* 打印%后的非法字符 */
                *(pch = acBuf) = (char)(swCh);
                sdwSize = 1;
                cSgn = '\0';
                goto PutFmtStr;
            }
        }

IntValue:   /* 获取变参中的整型参数 */
        if (bLong)
        {
            if(bLongLong)
            {
                if (('d' == swCh) || ('i' == swCh))
                {
                    ptVal = (long long)va_arg(vaList,long long);
                }
                else
                {
                    ptVal = va_arg(vaList,unsigned long long);
                }
            }
            else
            {
                if (('d' == swCh) || ('i' == swCh))
                {
                    ptVal = (long long)va_arg(vaList,long);
                }
                else
                {
                    ptVal = va_arg(vaList, unsigned long);
                }
            }
        }
        else if (bShort)
        {
            if (('d' == swCh) || ('i' == swCh))
            {
                ptVal = (long long)((short)va_arg(vaList,int));
            }
            else
            {
                ptVal = (unsigned short)va_arg(vaList,int);
            }
        }
        else
        {
            if (('d' == swCh) || ('i' == swCh))
            {
                ptVal = (long long)va_arg(vaList,int);
            }
            else
            {
                ptVal = va_arg(vaList, unsigned int);
            }
        }

        if (('d' == swCh) || ('i' == swCh))
        {
            if ((long long)ptVal < 0)
            {
                ptVal = -ptVal;
                cSgn = '-';
            }
        }

        if (bPrefix && (HEX == swBase) && (0 != ptVal))
        {
            bHexPrefix = TRUE;
        }

number:     /* 整型参数转为数字串 */
        if (sdwprec >= 0)
        {
            bZeroPad = FALSE;
        }

        pch = acBuf + BUF_SIZE;
        if ((0 != ptVal) || (0 != sdwprec))
        {
            switch (swBase)
            {
            case OCTAL:     /* 8进制输出 */
                do
                {
                    *--pch = (char)TOCHAR(ptVal & 7);
                    ptVal >>= 3;
                } while (ptVal);

                if (bPrefix && ('0' != *pch))
                {
                    *--pch = '0';
                }
                break;

            case DECIMAL:    /* 10进制输出 */
                while (ptVal >= DECIMAL)
                {
                    *--pch = (char)TOCHAR(ptVal % DECIMAL);
                    ptVal /= DECIMAL;
                }

                *--pch = (char)TOCHAR(ptVal);
                break;

            case HEX:    /* 16进制输出 */
                do
                {
                    *--pch = pcxdigits[ptVal & 15];
                    ptVal >>= 4;
                } while (ptVal);
                break;
        default:
            break;
            }
        }
        sdwSize = acBuf + BUF_SIZE - pch;

PutFmtStr:      /* 输出格式串及填充字符 */
        sdwExSize = sdwSize;
        if ('\0' != cSgn)
        {
            sdwExSize++;
            sdwprec++;
        }
        else if (bHexPrefix)
        {
            sdwExSize += HEXPREFIX;
        }

        sdwRealSize = (sdwprec > sdwExSize) ? sdwprec : sdwExSize;

        if (!bLAdj && !bZeroPad)
        {
            /* 缺省输出右对齐，左边填充空格 */
            if ((n = sdwWdth - sdwRealSize) > 0)
            {
                dwCount += n;
                if (dwCount > dwMaxCount)
                {
                    *pcOut = '\0';           /* 补上NULL结束符 */
                    return -1;
                }
                for (; n > 0; n--)
                {
                    *pcOut++ = ' ';
                }
            }
        }

        /* 前缀输出 */
        if ('\0' != cSgn)
        {
            dwCount += 1;
            if (dwCount > dwMaxCount)
            {
                *pcOut = '\0';               /* 补上NULL结束符 */
                return -1;
            }
            *pcOut++ = cSgn;
        }
        else if (bHexPrefix)
        {
            dwCount += HEXPREFIX;
            if (dwCount > dwMaxCount)
            {
                *pcOut = '\0';               /* 补上NULL结束符 */
                return -1;
            }
            *pcOut++ = '0';
            *pcOut++ = (char)swCh;
        }

        if (!bLAdj && bZeroPad)
        {
            /* 缺省输出右对齐，左边填充'0' */
            if ((n = sdwWdth - sdwRealSize) > 0)
            {
                dwCount += n;
                if (dwCount > dwMaxCount)
                {
                    *pcOut = '\0';           /* 补上NULL结束符 */
                    return -1;
                }
                for (; n > 0; n--)
                {
                    *pcOut++ = '0';
                }
            }
        }

        if ((n = sdwprec - sdwExSize) > 0)
        {
            /* 输出串长小于精度值，左边填充'0' */
            dwCount += n;
            if (dwCount > dwMaxCount)
            {
                *pcOut = '\0';               /* 补上NULL结束符 */
                return -1;
            }
            for (; n > 0; n--)
            {
                *pcOut++ = '0';
            }
        }

        /* 输出实际的格式串净荷 */
        dwCount += sdwSize;
        if (dwCount > dwMaxCount)
        {
            /* 输出直至缓冲区满 */
            sdwSize -= (dwCount - dwMaxCount);
            bcopy(pch, pcOut, sdwSize);
            pcOut[sdwSize] = '\0';           /* 补上NULL结束符 */
            return -1;
        }

        bcopy(pch, pcOut, sdwSize);
        pcOut += sdwSize;

        if (bLAdj)
        {
            /* 左对齐输出，判断是否需要填充空格 */
            if ((n = sdwWdth - sdwRealSize) > 0)
            {
                dwCount += n;
                if (dwCount > dwMaxCount)
                {
                    *pcOut = '\0';
                    return -1;                  /* 补上NULL结束符 */
                }
                for (; n > 0; n--)
                {
                    *pcOut++ = ' ';
                }
            }
        }

        sdwRet += (sdwWdth > sdwRealSize) ? sdwWdth : sdwRealSize;
    }
}

SWORD32 XOS_snprintf(CHAR       *pcBuffer,
                          WORD32     dwMaxCount,
                          const char *pcFmt,
                          ...)
{
    va_list vaList;
    SWORD32 sdwRet;

    assert(NULL != pcBuffer);
    assert(NULL != pcFmt);
    /*lint -e530*/
    (void)va_start(vaList, pcFmt);
    /*lint +e530*/
    sdwRet = log_vsnprintf(pcBuffer, dwMaxCount, pcFmt, vaList);
    va_end(vaList);

    return sdwRet;
}


