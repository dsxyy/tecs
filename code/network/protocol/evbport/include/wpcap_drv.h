#if !defined(WPCAP_DRV_INCLUDE_H__)
#define WPCAP_DRV_INCLUDE_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#define WPCAP_DEV_NAME_MAX_LEN      (64)
#define WPCAP_DEV_DES_NAME_MAX_LEN  (128)

    typedef struct tagWPcapDev
    {
        u32 uiPortNo;
        void *pWpcapAdapter;
        char strPcapDevName[WPCAP_DEV_NAME_MAX_LEN];
        char acPcapDevDescriptor[WPCAP_DEV_DES_NAME_MAX_LEN];
    }TWPcapDev;

    int32 InitWpcapDev(void);
    int32 CloseWpcapDev(void);
    int32 WpcapInitEvbPort(u32 uiPortNo);
    int32 WpcapDestroyEvbPort(u32 uiPortNo);
    int32 WpcapSendPkt(u32 uiPortNo, u8 *pPkt, u32 dwLen);
    int32 WpcapRcvPkt(u32 uiPortNo, u8 **ppPkt, u32 *pdwLen);
    int32 WpcapFreePkt(u8 *ppPkt);

    TWPcapDev * GetWpcapDevArray(u32 *pOutArrSize);
    void  SetWpcapEvbPortNo(u32 uiPortNo, TWPcapDev * pWpcapDev);

#endif

#ifdef __cplusplus
}
#endif

#endif
