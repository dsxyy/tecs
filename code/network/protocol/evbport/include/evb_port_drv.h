#if !defined(EVB_PORT_DRV_INCLUDE_H__)
#define EVB_PORT_DRV_INCLUDE_H__

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct tagDrvPktStat
    {
        u64 ullPortStateErr;
        u64 ullDrvRcv;
        u64 ullDrvRcvErr;       
        u64 ullDrvSnd;
        u64 ullDrvSndErr;
    }TDrvPktStat;

    struct tagEvbPortDriver
    {
        int32 (*pInit)(u32 uiPortNo);
        int32 (*pWrite)(u32 uiPortNo, u8 *pPkt, u32 dwLen);
        int32 (*pRead)(u32 uiPortNo, u8 **ppPkt, u32 *pdwLen);
        int32 (*pDestroy)(u32 uiPortNo);
        int32 (*pFreePkt)(u8 *pPkt);
    };

    int SendEvbPacket(u32 uiPortNo, u8 *pkt, u16 len);
    int32 ReceivePktFromEvbIssPort(struct tagEvbIssPort *pIssPort, u8 *pPkt, u32 dwLen);
#ifdef __cplusplus
}
#endif

#endif

