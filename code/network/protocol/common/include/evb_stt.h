#if !defined(EVB_STT_INCLUDE_H__)
#define EVB_STT_INCLUDE_H__

#ifdef __cplusplus
extern "C"{
#endif 

    typedef struct tagEvbPktStt
    {
        /* cdcp stt; */
        u64 ullCdcpModInitErr;
        u64 ullCdcpRcv;
        u64 ullCdcpRcvErr;
        u64 ullCdcpAllocRcvMemErr;
        u64 ullCdcpRcvLenErr;
        u64 ullCdcpRcvTypeErr;
        u64 ullCdcpRcvUnknowPkt;
        u64 ullCdcpSnd;
        u64 ullCdcpSndErr;

        /* evbtlv stt; */

        /* ecp stt; */

        /* vdp stt; */

        /* lldp stt; */
        u64 ullLldpSnd;
        u64 ullLldpRcv;
        /* Driver stt; */
        u64 ullDrvSnd;
        u64 ullDrvRcv;

    }TEvbPktStt;

    void EvbPktSttShow(TEvbPktStt *pStt);

#ifdef __cplusplus
}
#endif 

#endif
