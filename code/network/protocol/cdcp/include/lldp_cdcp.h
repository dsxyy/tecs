#if !defined(LLDP_CDCP_INCLUDE_H__)
#define LLDP_CDCP_INCLUDE_H__

#ifdef __cplusplus
extern "C" {
#endif

#define LLDP_MOD_CDCP	    ((OUI_IEEE_8021Qbg << 8) | CDCP_TLV_OUI_SUBTYPE)
#define CDCP_SM_NOT_RUNNING (0)
#define CDCP_SM_RUNNING     (1)
#define CDCP_VERSION_0      (0x000b)
#define CDCP_VERSION_1      (0x001b)

    /* s-channel 属性描述; */
    typedef struct tagCdcpSchn
    {
        u32 uiUapISSPortNum;
        u16 wScid;              /* 1-167 */
        u16 wSvid;
        u32 uiCapPortNum;
        u32 uiSbpOrUrpPortNum;
    }TCdcpSchn;

    typedef struct tagCdcpPktStat
    {
        u64 ullCdcpModInitErr;
        u64 ullCdcpRcv;
        u64 ullCdcpRcvErr;
        u64 ullCdcpAllocRcvMemErr;
        u64 ullCdcpRcvLenErr;
        u64 ullCdcpRcvTypeErr;
        u64 ullUnknowPkt;
        u64 ullCdcpSnd;
        u64 ullCdcpSndErr;
    }TCdcpPktStat;

    /* CDCP 模块初始化; */
    int32 InitCdcp(void);
    int32 ShutdownCdcp(u32 uiLocalPort);
    int32 InitCdcpRunningData(struct tagCdcpRunningData *pData);
    int32 ClearCdcpRunningData(struct tagCdcpRunningData *pData);

    /* CDCP state machine 主要函数; */
    int32 SetSVIDRequest(u8 ucOperRole, struct tagListScidSvidHead *pAdminSVIDWantsHead, struct tagListScidSvidHead *pOperSVIDListHead);
    int32 RxSVIDConfig(struct tagListScidSvidHead *pOperSVIDListHead, struct tagListScidSvidHead *pLastRemoteSVIDListHead);
    int32 TxSVIDConfig(u16 wOperChnCap, u16 wRemoteChnCap, struct tagCdcpVlanPool *pLastLocalSVIDPool, 
        struct tagListScidSvidHead *pRemoteSVIDListHead, struct tagListScidSvidHead *pOperSVIDListHead);
    void RunCdcpSM(struct lldp_port *pPort);
    
    /* CDCP 辅助实现函数; */
    int32 CompareScidSvidList(struct tagListScidSvidHead *pListHead1, struct tagListScidSvidHead *pListHead2);
    int32 CopyScidSvidList(struct tagListScidSvidHead *pSrc, struct tagListScidSvidHead *pDest);
    int32 IsInScidSvidList(struct tagCdcpScidSvidPair *pElm, struct tagListScidSvidHead *pListHead);
    int32 IsInLocalSVIDPool(u16 wSvid, u16 wPoolSz, u16 *pLocalSVIDPool);
    int32 GetScidSvidListCount(struct tagListScidSvidHead *pListHead);
    u16   AllocSVID(struct tagCdcpVlanPool *pVlanPool);
    int32 FreeSVID(u16 wVlanID, struct tagCdcpVlanPool *pVlanPool);
    void  FreeScidSvidList(struct tagListScidSvidHead *pListHead);
    void  InsertElmByScidAscendingOrder(struct tagCdcpScidSvidPair *pElm, struct tagListScidSvidHead *pListHead);
    struct tagCdcpScidSvidPair *GetScidSvidElem(u16 wScid, struct tagListScidSvidHead *pListHead);
    int32 DestroyVlanPool(struct tagCdcpVlanPool *pPool);
    int32 CreateVlanPool(struct tagCdcpVlanPool *pPool);
    int32 SetVlanPoolRange(struct tagCdcpVlanPool *pPool, u16 wMinVID, u16 wMaxVID);
    int32 CompareVlanPool(struct tagCdcpVlanPool *pPool1, struct tagCdcpVlanPool *pPool2);
    int32 CopyVlanPool(struct tagCdcpVlanPool *pSrc, struct tagCdcpVlanPool *pDest);
    int32 SetVlanUsed(struct tagCdcpVlanPool *pPool, u16 wVID);

    /* CDCP functions for LLDP */
    struct lldp_module *CdcpRegister(void);
    void CdcpUnregister(struct lldp_module *mod);
    void CdcpIfAdd(u32 uiLocalPort);
    void CdcpIfDel(u32 uiLocalPort);
    void CdcpIfDown(u32 uiLocalPort);
    void CdcpIfup(u32 uiLocalPort);
    int  CdcpRChange(struct lldp_port *pPort,  struct unpacked_tlv *pTlv, bool *pIsStoredTlv);
    u8   CdcpMibDeleteObjects(struct lldp_port *pPort);
    int32 CdcpCheckPending(struct lldp_port *pPort);
   
    struct packed_tlv *CdcpGetTlv(struct lldp_port *pPort);
    void CdcpFreeUserData(struct tagCdcpUserData *pUserData);
    bool UnpackCdcpTlv(struct lldp_port *pPort, 
                       struct unpacked_tlv *pTlv, 
                       bool *pIsStoredTlv, 
                       struct tagCdcpRunningData *pCdcpRunningData, 
                       struct tagEvbPort *pEvbPort);
    struct tagCdcpRunningData *GetCdcpRunningData(u32 uiLocalPort);

#ifdef __cplusplus
}
#endif

#endif
