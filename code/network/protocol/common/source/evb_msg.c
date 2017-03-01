#ifdef __cplusplus
extern "C" {
#endif

#include "evb_comm.h"
#include "evb_thr.h"

#define  EVB_MSG_QUE_MAX_SIZE   (10) /* 最大消息个数; */
#define  EVB_MSG_MAX_SIZE       (8192) /* 包含消息头; */

    typedef struct tagEvbMqdMsgData
    {
        u32 dwMsgID;
        u32 dwMsgLen;
        u8  aucMsg[0];
    }TEvbMsgData;

#ifdef _WIN32
    int32 EvbPostThrdMsg(TEvbThrdMgr *pThread, u32 dwMsgID, void *pMsg, u32 dwMsgLen)
    {
        TEvbMsgData *pMsgBuff = NULL;
        int32 nBufLen = 0;

        if(NULL == pThread)
        {
            EVB_ASSERT(0);
            return -1;
        }

        if(dwMsgLen > 0)
        {
            nBufLen = sizeof(TEvbMsgData) + dwMsgLen;
            if(nBufLen > EVB_MSG_MAX_SIZE)
            {
                EVB_LOG(EVB_LOG_ERROR, "EvbPostThrdMsg: msg size is over flow[MAX SIZE = %dB].\n", 
                    EVB_MSG_MAX_SIZE);
                return -1;
            }

            pMsgBuff = (TEvbMsgData *)GLUE_ALLOC(nBufLen);
            if(NULL == pMsgBuff)
            {
                EVB_LOG(EVB_LOG_ERROR, "EvbPostThrdMsg: msg buff alloc failed.\n");
                return -1;
            }
            MEMSET(pMsgBuff, 0x0, nBufLen);
            pMsgBuff->dwMsgID = dwMsgID;
            pMsgBuff->dwMsgLen = dwMsgLen;            
            MEMCPY(pMsgBuff->aucMsg, pMsg, dwMsgLen);
        }/* end if */

        if(0 == PostThreadMessage(pThread->tThreadID, dwMsgID, (WPARAM)pMsgBuff, (LPARAM)dwMsgLen))
        {
            EVB_LOG(EVB_LOG_ERROR, "EvbPostThrdMsg: Send to the thread(name: %s, id: 0x%x) msg failed.errno:%d.\n", 
                pThread->acName, pThread->tThreadID, GetLastError());
            return -1;
        }
        return 0;
    }

    int32 EvbCreateThrdMsgQue(TEvbThrdMgr *pThread)
    {
        if(NULL == pThread)
        {
            EVB_ASSERT(0);
            return -1;
        }

        if(NULL != pThread->pMsgQue)
        {
            EVB_LOG(EVB_LOG_ERROR, "EvbCreateThrdMsgQue: the thread's(name: %s, id: 0x%x) msg queue is existed.\n", 
                pThread->acName, pThread->tThreadID);
            return -1;
        }

        pThread->pMsgQue = (void *)GLUE_ALLOC(sizeof(MSG));
        if(NULL == pThread->pMsgQue)
        {
            EVB_LOG(EVB_LOG_ERROR, "EvbCreateThrdMsgQue: alloc the thread's(name: %s, id: 0x%x) msg queue failed.\n", 
                pThread->acName, pThread->tThreadID);
            return -1;
        }
        MEMSET(pThread->pMsgQue, 0x0, sizeof(MSG));

        pThread->pRcvMsgBuff = NULL;

        PeekMessage((MSG *)pThread->pMsgQue, NULL, WM_USER, WM_USER, PM_NOREMOVE);

        return 0;
    }

    int32 EvbDestroyThrdMsgQue(TEvbThrdMgr *pThread)
    {
        if(NULL == pThread)
        {
            EVB_ASSERT(0);
            return -1;
        }

        if(NULL != pThread->pMsgQue)
        {
            GLUE_FREE(pThread->pMsgQue);
            pThread->pMsgQue = NULL;
        }

        pThread->pRcvMsgBuff = NULL;

        return 0;
    }

    int32 EvbGetThrdMsg(TEvbThrdMgr *pThread, u32 *pMsgID, void **ppMsg, u32 *pdwMsgLen)
    {
        MSG *pQueMsg = NULL;
        TEvbMsgData *pMsgData = NULL;

        if(NULL == pThread || NULL == pMsgID || NULL == pdwMsgLen)
        {
            EVB_ASSERT(0);
            return -1;
        }
        pQueMsg = (MSG *)pThread->pMsgQue;

        if(NULL == pQueMsg)
        {
            EVB_LOG(EVB_LOG_ERROR, "EvbGetThrdMsg: the thread's(name: %s, id: 0x%x) msg queue is not existed.\n", 
                pThread->acName, pThread->tThreadID);
            return -1;
        }

        if(GetMessage(pQueMsg, NULL,  0, 0)) /* 阻塞式(GetMessage(pQueMsg, 0, 0, 0)); PeekMessage高响应式; */
        {
            *pMsgID = pQueMsg->message;
            pThread->pRcvMsgBuff = (char *)pQueMsg->wParam;
            pMsgData = (TEvbMsgData *)pQueMsg->wParam;
            *pdwMsgLen = (u32)pQueMsg->lParam;
            if(pMsgData && *pdwMsgLen > 0)
            {
                *ppMsg = pMsgData->aucMsg;
            }
            else
            {
                *ppMsg = NULL;
            }
        }
        return 0;
    }

    int32 EvbClrThrdMsgBuff(TEvbThrdMgr *pThread)
    {
        if(NULL == pThread)
        {
            EVB_ASSERT(0);
            return -1;
        }

        if(pThread->pRcvMsgBuff)
        {
            GLUE_FREE(pThread->pRcvMsgBuff);
            pThread->pRcvMsgBuff = NULL;
        }

        return 0;
    }

#else
    #define EVB_MSG_QUE_NAME    ("/EVB_MSG_QUEUE")
    int32 EvbPostThrdMsg(TEvbThrdMgr *pThread, u32 dwMsgID, void *pMsg, u32 dwMsgLen)
    {
        mqd_t *pMqd = NULL;        
        TEvbMsgData *pMsgBuff = NULL;
        int32 nBufLen = 0;
        int32 ret = 0;

        if(NULL == pThread)
        {
            EVB_ASSERT(0);
            return -1;
        }

        pMqd = (mqd_t *)pThread->pMsgQue;

        if(NULL == pMqd)
        {
            EVB_LOG(EVB_LOG_ERROR, "EvbPostThrdMsg: The thread's(name: %s, id: 0x%x) msg queue descriptor is null.\n", 
                pThread->acName, pThread->tThreadID);
            return -1;
        }

        nBufLen = sizeof(TEvbMsgData) + dwMsgLen;
        if(nBufLen > EVB_MSG_MAX_SIZE)
        {
            EVB_LOG(EVB_LOG_ERROR, "EvbPostThrdMsg: msg size is over flow[MAX SIZE = %dB].\n", 
                EVB_MSG_MAX_SIZE);
            return -1;
        }

        pMsgBuff = (TEvbMsgData *)GLUE_ALLOC(nBufLen);
        if(NULL == pMsgBuff)
        {
            EVB_LOG(EVB_LOG_ERROR, "EvbPostThrdMsg: msg buff alloc failed.\n");
            return -1;
        }
        MEMSET(pMsgBuff, 0x0, nBufLen);
        pMsgBuff->dwMsgID = dwMsgID;
        pMsgBuff->dwMsgLen = dwMsgLen;
        if(dwMsgLen > 0)
        {
            MEMCPY(pMsgBuff->aucMsg, pMsg, dwMsgLen);
        }
        
        /* mqd_t mq_send(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned msg_prio); */
        ret = mq_send(*pMqd, (const char *)pMsgBuff, nBufLen, 0);

        GLUE_FREE(pMsgBuff);
        pMsgBuff = NULL;

        if(0 != ret)
        {
            EVB_LOG(EVB_LOG_ERROR, "EvbPostThrdMsg: call mq_send(%p, %p, %d, 0) failed.\n", 
                pMqd, pMsg, dwMsgLen);
            return -1;
        }

        return ret;       
    }

    int32 EvbCreateThrdMsgQue(TEvbThrdMgr *pThread)
    {
        int  flags;
        mqd_t mqd;
        struct mq_attr attr;
        struct rlimit  rlim_new;

        if(NULL == pThread)
        {
            EVB_ASSERT(0);
            return -1;
        }

        if(NULL != pThread->pMsgQue)
        {
            EVB_LOG(EVB_LOG_ERROR, "EvbCreateThrdMsgQue: the thread's(name: %s, id: 0x%x) msg queue descriptor is existed.\n", 
                pThread->acName, pThread->tThreadID);
            return -1;
        }

        pThread->pMsgQue = (void *)GLUE_ALLOC(sizeof(mqd_t));
        if(NULL == pThread->pMsgQue)
        {
            EVB_LOG(EVB_LOG_ERROR, "EvbCreateThrdMsgQue: alloc the thread's(name: %s, id: 0x%x) msg queue descriptor memory failed.\n", 
                pThread->acName, pThread->tThreadID);
            return -1;
        }
        MEMSET(pThread->pMsgQue, 0x0, sizeof(mqd_t));

        pThread->pRcvMsgBuff = (char *)GLUE_ALLOC(EVB_MSG_MAX_SIZE);
        if(NULL == pThread->pRcvMsgBuff)
        {
            EVB_LOG(EVB_LOG_ERROR, "EvbCreateThrdMsgQue: alloc the thread's(name: %s, id: 0x%x) receive msg buffer memory failed.\n", 
                pThread->acName, pThread->tThreadID);

            GLUE_FREE(pThread->pMsgQue);
            pThread->pMsgQue = NULL;

            return -1;
        }
        MEMSET(pThread->pRcvMsgBuff, 0x0, EVB_MSG_MAX_SIZE);

        rlim_new.rlim_cur = rlim_new.rlim_max = RLIM_INFINITY;
        if (setrlimit(RLIMIT_MSGQUEUE , &rlim_new)!=0)  
        {
            EVB_LOG(EVB_LOG_ERROR, "EvbCreateThrdMsgQue: call setrlimit failed \n");
        }

        flags = O_RDWR|O_CREAT;

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IROTH | S_IRGRP)
        attr.mq_flags = O_CREAT | O_RDWR;
        attr.mq_maxmsg = EVB_MSG_QUE_MAX_SIZE;
        attr.mq_msgsize = EVB_MSG_MAX_SIZE;
        attr.mq_curmsgs = 0;
        mqd = mq_open(EVB_MSG_QUE_NAME, flags, FILE_MODE, &attr);

        if (mqd == -1)
        {  
            EVB_LOG(EVB_LOG_ERROR, "EvbCreateThrdMsgQue: mq_open mqd : %d \n", mqd);  
            GLUE_FREE(pThread->pMsgQue);
            pThread->pMsgQue = NULL;
            return -1;
        }

        MEMCPY(pThread->pMsgQue, &mqd, sizeof(mqd_t));

        return 0;
    }

    int32 EvbDestroyThrdMsgQue(TEvbThrdMgr *pThread)
    {
        if(NULL == pThread)
        {
            EVB_ASSERT(0);
            return -1;
        }

        if(NULL != pThread->pMsgQue)
        {
            mq_close(*(mqd_t *)pThread->pMsgQue);
            mq_unlink(EVB_MSG_QUE_NAME);
            GLUE_FREE(pThread->pMsgQue);
            pThread->pMsgQue = NULL;
        }

        if(NULL != pThread->pRcvMsgBuff)
        {
            GLUE_FREE(pThread->pRcvMsgBuff);
            pThread->pRcvMsgBuff = NULL;
        }

        return 0;
    }

    int32 EvbGetThrdMsg(TEvbThrdMgr *pThread, u32 *pMsgID, void **ppMsg, u32 *pdwMsgLen)
    {
        mqd_t *pMqd = NULL;
        TEvbMsgData *pMsgBuff = NULL;
        int32 iMsgLen = 0;
        
        if(NULL == pThread || NULL == pMsgID)
        {
            EVB_ASSERT(0);
            return -1;
        }
        
        pMqd = (mqd_t *)pThread->pMsgQue;
        if(NULL == pMqd)
        {
            EVB_LOG(EVB_LOG_ERROR, "EvbGetThrdMsg: the thread's(name: %s, id: 0x%x) msg queue descriptor is not existed.\n", 
                pThread->acName, pThread->tThreadID);
            return -1;
        }
 
        if(NULL == pThread->pRcvMsgBuff)
        {
            EVB_LOG(EVB_LOG_ERROR, "EvbGetThrdMsg: The thread's(name: %s, id: 0x%x) receive msg buff is null.\n", 
                pThread->acName, pThread->tThreadID);
            return -1;
        }
        MEMSET(pThread->pRcvMsgBuff, 0x0, EVB_MSG_MAX_SIZE);   

        iMsgLen = mq_receive(*pMqd, pThread->pRcvMsgBuff, EVB_MSG_MAX_SIZE, NULL); 
        if (iMsgLen < (int32)sizeof(TEvbMsgData))        
        {
            EVB_LOG(EVB_LOG_ERROR, "EvbGetThrdMsg: The thread's(name: %s, id: 0x%x) call mq_receive failed.\n", 
                pThread->acName, pThread->tThreadID);  
            return -1;
        }
        else
        {
            pMsgBuff = (TEvbMsgData *)pThread->pRcvMsgBuff;  
            
            *pMsgID = pMsgBuff->dwMsgID;
            *pdwMsgLen = pMsgBuff->dwMsgLen;
            if(0 == pMsgBuff->dwMsgLen || EVB_MSG_MAX_SIZE < pMsgBuff->dwMsgLen)
            {
                *ppMsg = NULL;
            }
            else
            {
                *ppMsg = pMsgBuff->aucMsg;
            } 
        }/* end if (iMsgLen... */

        return 0;
    }

    int32 EvbClrThrdMsgBuff(TEvbThrdMgr *pThread)
    {
        if(NULL == pThread)
        {
            EVB_ASSERT(0);
            return -1;
        }

        if(pThread->pRcvMsgBuff)
        {
            MEMSET(pThread->pRcvMsgBuff, 0x0, EVB_MSG_MAX_SIZE);
        }

        return 0;
    }

#endif

#ifdef __cplusplus
}
#endif
