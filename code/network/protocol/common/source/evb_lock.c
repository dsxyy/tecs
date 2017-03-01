#ifdef __cplusplus
extern "C" {
#endif

#include "evb_comm.h"
#include "evb_lock.h"

#ifdef _WIN32
    int32 CreateLock(char *pMutexName, TMutex* pMutex)
    {
        if(NULL == pMutex)
        {
            EVB_ASSERT(0);
            return -1;
        }

        *pMutex = CreateMutex(NULL, FALSE, NULL);
        if(NULL == *pMutex)  
        {
            EVB_LOG(EVB_LOG_ERROR, "CreateLock: create mutex failed, errno: %d.\n", GetLastError());
            return -1;
        }

        return 0;
    }
    int32 DestroyLock(TMutex* pMutex)
    {
        int32 nRet = 0;
        
        if(NULL == pMutex)
        {
            EVB_ASSERT(0);
            return -1;
        }

        nRet = CloseHandle(*pMutex);
        if(0 == nRet)
        {
            EVB_LOG(EVB_LOG_ERROR, "DestroyLock: destroy mutex failed, errno: %d.\n", GetLastError());
            return -1;
        }
        return 0;
    }

    int32 EvbLock(TMutex* pMutex)
    {
        if(NULL == pMutex)
        {
            EVB_ASSERT(0);
            return -1;
        }
        WaitForSingleObject(*pMutex, INFINITE);
        return 0;
    }

    int32 EvbUnlock(TMutex* pMutex)
    {
        if(NULL == pMutex)
        {
            EVB_ASSERT(0);
            return -1;
        }

        ReleaseMutex(*pMutex);
        return 0;
    }

#else
    int32 CreateLock(char *pMutexName, TMutex* pMutex)
    {
        pthread_mutexattr_t attr;
        int32 nRet = 0;

        if(NULL == pMutex)
        {
            EVB_ASSERT(0);
            return -1;
        }

        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE); 
        nRet = pthread_mutex_init(pMutex, &attr);
        pthread_mutexattr_destroy(&attr);
        if(0 != nRet)
        {
            EVB_LOG(EVB_LOG_ERROR, "CreateLock: create mutex failed, errno: %d.\n", nRet);
            return -1;
        }

        return 0;
    }

    int32 DestroyLock(TMutex* pMutex)
    {
        int32 nRet = 0;

        if(NULL == pMutex)
        {
            EVB_ASSERT(0);
            return -1;
        }

        nRet = pthread_mutex_destroy(pMutex);
        if(0 != nRet)
        {
            EVB_LOG(EVB_LOG_ERROR, "DestroyLock: destroy mutex failed, errno: %d.\n", nRet);
            return nRet;
        }

        return 0;
    }

    int32 EvbLock(TMutex* pMutex)
    {
        int32 nRet = 0;

        if(NULL == pMutex)
        {
            EVB_ASSERT(0);
            return -1;
        }

        nRet = pthread_mutex_lock(pMutex);
        if(0 != nRet)
        {
            EVB_LOG(EVB_LOG_ERROR, "EvbLock: mutex lock failed, errno: %d.\n", nRet);
        }
        return nRet;
    }

    int32 EvbUnlock(TMutex* pMutex)
    {
        int32 nRet = 0;

        if(NULL == pMutex)
        {
            EVB_ASSERT(0);
            return -1;
        }

        nRet = pthread_mutex_unlock(pMutex);
        if(0 != nRet)
        {
            EVB_LOG(EVB_LOG_ERROR, "EvbUnlock: mutex unlock failed, errno: %d.\n", nRet);
        }
        return nRet;        
    }

#endif

#ifdef __cplusplus
}
#endif

