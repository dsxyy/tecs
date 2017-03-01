#if !defined(EVB_LOCK_INCLUDE_H__)
#define EVB_LOCK_INCLUDE_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
    typedef HANDLE  TMutex;

#else
    typedef pthread_mutex_t TMutex;

#endif

    int32 CreateLock(char *pMutexName, TMutex* pMutex);
    int32 DestroyLock(TMutex* ptMutex);
    int32 EvbLock(TMutex* pMutex);
    int32 EvbUnlock(TMutex* pMutex);


#ifdef __cplusplus
}
#endif

#endif

