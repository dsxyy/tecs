#if !defined(WD_RSOCK_DRV_INCLUDE_H__)
#define WD_RSOCK_DRV_INCLUDE_H__

#ifdef __cplusplus
extern "C" {
#endif

/*  包含一些必要的头文件 */

    typedef struct tagLnxRSockPort
    {
        #define LNX_IF_NAME_MAX_LEN  (16)
        int uiPortNo;
        char strIfName[LNX_IF_NAME_MAX_LEN];
        int nSock; 
    }TLnxRSockPort;

    int InitLnxRsockDrv(void);
    int CloseLnxRsockDrv(void);
    int LnxRsockInitEvbPort(int uiPortNo);
    int LnxRsockDestroyEvbPort(int uiPortNo);
    int wd_LnxRsockSendPkt(unsigned char * pPkt, unsigned int dwLen);
    int wd_LnxRsockRcvPkt(unsigned char **ppPkt, unsigned int *pdwLen);
    TLnxRSockPort * GetLnxRsockDevArray(int *pOutArrSize);	
    int tecs_wd_rcv(unsigned char **ppPkt, unsigned int *pdwLen);
    int tecs_wd_feed(unsigned int time_out);
    #define ioctlsocket(a,b,c)      ioctl(a,b,c)
#ifdef __cplusplus
}
#endif

#endif
