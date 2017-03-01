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
        TEvbThreadID tThreadID;             /* �߳�ID; */
        u32 dwPriority;                     /* �߳����ȼ�; */
        u32 dwStackSize;                    /* �߳�ջ��С; */
        char acName[EVB_THREAD_NAME_MAX_LEN];   /* �߳�����; */
        u16 wThrdFlag;                      /* �߳����б�־; */
        u16 wExitCode;                      /* �߳��˳����룬1��ʾ�����˳�; */
        EVB_THRD_INIT pfInit;               /* �̵߳ĳ�ʼ������; */
        EVB_THRD_ENTRY pfEntry;             /* �߳����; */
        void *pMsgQue;                      /* �߳���Ϣ����; */
        char *pRcvMsgBuff;                  /* �߳̽�����Ϣ�建��;Ĭ��8192B; */
        void *pData;                        /* �߳�����; */
        TEvbTaskID tid;                     /* linux task id���id ���ڵ���; */
    }TEvbThrdMgr;

    int32 StartEvbThread(TEvbThrdMgr *pThrd);
    int32 KillEvbThread(TEvbThrdMgr *pThrd);
    

#ifdef __cplusplus
}
#endif

#endif


