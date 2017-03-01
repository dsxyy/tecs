#if !defined(CDCP_TLV_INCLUDE_H__)
#define CDCP_TLV_INCLUDE_H__

#define CDCP_TLV_OUI_SUBTYPE	(0x00)
#define CDCP_TLV_OUI {00, 0x80, 0xc2}

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)
    typedef struct tagScidSvid
    {
        u8 aucScidSvid[3];
    }TScidSvid;

    typedef struct tagCdcpTlv
    {
        u8  ucOUI[OUI_SIZE];
        u8  ucSubType;
        u32 bitRole     :1;
        u32 bitRes1     :3;
        u32 bitSComp    :1;
        u32 bitRes2     :15;
        u32 bitChnCap   :12;
        TScidSvid atScidSvid[0];
    }TCdcpTlv;
#pragma pack()

    struct tagCdcpScidSvidPair
    {
        u16 wScid;
        u16 wSvid;
        LIST_ENTRY(tagCdcpScidSvidPair) lstEntry;
    };

    struct tagVlanInfo
    {
        u16 wVlanID;
        u8  ucIsUse;    /* 0: not using, 1: using; */
        u8  ucReserve;
        LIST_ENTRY(tagVlanInfo) lstEntry;
    };

    LIST_HEAD(tagVlanPoolHead, tagVlanInfo);
    struct tagCdcpVlanPool
    {
        struct tagVlanPoolHead tVlanPoolHead;
        u16 wMaxVlanID;
        u16 wMinVlanID;
    };

    /* CDCP 协议运行数据; */
    LIST_HEAD(tagListScidSvidHead, tagCdcpScidSvidPair);
    struct tagCdcpRunningData
    {
        u32 uiLocalPort;
        u16 wAdminChnCap;
        u16 wOperChnCap;
        u8 ucAdminRole;
        u8 ucOperRole;
        u8 ucOperVersion;
        u8 ucRemoteVersion;
        u16 wRemoteChnCap;
        u8 ucRemoteRole;
        u8 ucSchState;
        bool bSCompEnable;    /* 本地是否支持S-VLAN component; */
        bool bRemoteSCompEnable;
        struct tagCdcpVlanPool tLocalSVIDPool;
        struct tagCdcpVlanPool tLastSVIDPool;
        struct tagListScidSvidHead lstAdminSVIDWantsHead;
        struct tagListScidSvidHead lstOperSVIDListHead;
        struct tagListScidSvidHead lstRemoteSVIDListHead;
        struct tagListScidSvidHead lstLastRemoteSVIDListHead;
        struct tagListScidSvidHead lstLastSVIDWantsHead;
        //TCdcpTlv *pRxTlvData;
        struct unpacked_tlv *pRxTlv;

        LIST_ENTRY(tagCdcpRunningData) lstEntry;
    };

    struct tagCdcpUserData
    {
        LIST_HEAD(CdcpDataHead, tagCdcpRunningData) head;
    };


#ifdef __cplusplus
}
#endif

#endif
