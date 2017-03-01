#if !defined(EVB_MSG_INCLUDE_H__)
#define EVB_MSG_INCLUDE_H__

#ifdef __cplusplus
extern "C" {
#endif

    int32 EvbPostThrdMsg(TEvbThrdMgr *pThread, u32 dwMsgID, void *pMsg, u32 dwMsgLen);
    int32 EvbCreateThrdMsgQue(TEvbThrdMgr *pThread);
    int32 EvbDestroyThrdMsgQue(TEvbThrdMgr *pThread);
    int32 EvbGetThrdMsg(TEvbThrdMgr *pThread, u32 *pMsgID, void **ppMsg, u32 *pdwMsgLen);
    int32 EvbClrThrdMsgBuff(TEvbThrdMgr *pThread);

#ifdef __cplusplus
}
#endif

#endif
