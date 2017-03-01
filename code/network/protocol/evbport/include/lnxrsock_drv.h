#if !defined(LNXRAWSOCKET_INCLUDE_H__)
#define LNXRAWSOCKET_INCLUDE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "evb_comm.h"


#ifdef _EVB_TECS

#define LNX_IF_NAME_MAX_LEN      (16)

    typedef struct tagLnxRSockPort
    {
        u32 uiPortNo;
        char strIfName[LNX_IF_NAME_MAX_LEN];
        int32 nSock; 
        LIST_ENTRY(tagLnxRSockPort) lstEntry;
    }TLnxRSockPort;

    LIST_HEAD(tagLnxRSockPort_head, tagLnxRSockPort);

    int32 InitLnxRsockDrv(void);
    int32 CloseLnxRsockDrv(void);
    int32 LnxRsockInitEvbPort(u32 uiPortNo);
    int32 LnxRsockDestroyEvbPort(u32 uiPortNo);
    int32 LnxRsockSendPkt(u32 uiPortNo, u8 *pPkt, u32 dwLen);
    int32 LnxRsockRcvPkt(u32 uiPortNo, u8 **ppPkt, u32 *pdwLen);
    int32 LnxRsockFreePkt(u8 *pPkt);    

    void DbgShowEvbRawSocket(u32 uiPort);

    TLnxRSockPort * GetLnxRsockDevArray(u32 *pOutArrSize);
    // void  SetLnxRsockEvbPortNo(u32 uiPortNo, TLnxRSockPort * pLnxDev);

    int32 AddLnxRsockEvbPortNo(u32 uiPort,char * acIfName);
    int32 DelLnxRsockEvbPortNo(u32 uiPort);

    #define ioctlsocket(a,b,c)      ioctl(a,b,c)

    void DbgShowEvbRawSocket(u32 uiPort);
#endif

#ifdef __cplusplus
}
#endif

#endif
