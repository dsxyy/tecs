#if !defined(EVB_THR_INCLUDE_H__)
#define EVB_THR_INCLUDE_H__

#ifdef __cplusplus
extern "C" {
#endif

#define EVB_THREAD_NAME_MAX_LEN                 (32)
#define EVB_THREAD_RUNNING                      (1)
#define EVB_THREAD_STOP                         (0)

#ifdef _WIN32
#define INVALID_THREAD_ID                       (0)
    typedef u32 TEvbThreadID;
    typedef HANDLE TEvbTaskID;
#else
#define INVALID_THREAD_ID                       (0)
    typedef pthread_t TEvbThreadID;
    typedef pid_t TEvbTaskID;
#endif

    typedef void* (* EVB_THRD_ENTRY)(void *pParam);
    typedef int32 (* EVB_THRD_INIT)(void *pParam);
    typedef struct tagEvbThrdMgr{
        TEvbThreadID tThreadID;             /* 线程ID; */
        u32 dwPriority;                     /* 线程优先级; */
        u32 dwStackSize;                    /* 线程栈大小; */
        char acName[EVB_THREAD_NAME_MAX_LEN];   /* 线程名称; */
        u16 wThrdFlag;                      /* 线程运行标志; */
        u16 wExitCode;                      /* 线程退出代码，1表示可以退出; */
        EVB_THRD_INIT pfInit;               /* 线程的初始化函数; */
        EVB_THRD_ENTRY pfEntry;             /* 线程入口; */
        void *pMsgQue;                      /* 线程消息队列; */
        char *pRcvMsgBuff;                  /* 线程接收消息体缓冲;默认8192B; */
        void *pData;                        /* 线程数据; */
        TEvbTaskID tid;                     /* linux task id输出id 便于调试; */
    }TEvbThrdMgr;

    int32 StartEvbThread(TEvbThrdMgr *pThrd);
    int32 KillEvbThread(TEvbThrdMgr *pThrd);
    

#ifdef __cplusplus
}
#endif

#endif


