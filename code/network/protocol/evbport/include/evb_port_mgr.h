#if !defined(EVB_PORT_MGR_INCLUDE_H__)
#define EVB_PORT_MGR_INCLUDE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "evb_stt.h"
#include "evb_port_drv.h"

#define EVB_INVALID_PORT_NO         (0x0)

#define EVB_PORT_STATE_INIT         (0x0)
#define EVB_PORT_STATE_UP           (0x1)
#define EVB_PORT_STATE_DW           (0x2)

    /* Internal Sublayer Service Port(or Enhanced Internal Sublayer Service port[VLAN support]). */
    struct tagEvbIssPort
    {
        u32 uiPortNo;
        char acName[EVB_PORT_NAME_MAX_LEN];
        u8 aucMac[EVB_PORT_MAC_LEN];
        u8 ucPortState;
        u8 ucPad;

        struct tagEvbPortDriver *pDriver;

        struct tagEvbIssPort *pPortHashNext;
        LIST_ENTRY(tagEvbIssPort) lstEntry;

        /* Packet statistics; */
        TEvbPktStt *pStt;
    };

    /* EVB port type; */
    typedef enum tagEmEvbPortType
    {
        EM_EVB_PT_VBP = 0,          /* C-VLAN Bridge Port; */
        EM_EVB_PT_PNP,              /* Provider Network Port; */
        EM_EVB_PT_CNP,              /* Customer Network Port; */
        EM_EVB_PT_CEP,              /* Customer Edge Port; */
        EM_EVB_PT_CBP,              /* Customer Backbone Port; */
        EM_EVB_PT_VIP,              /* Virtual Instance Port (VIP); */
        EM_EVB_PT_DBP,              /* D-Bridge Port; */
        EM_EVB_PT_RCAP,             /* Remote Customer Access Port; */
        EM_EVB_PT_SBP,              /* Station-facing Bridge Port; */
        EM_EVB_PT_UAP,              /* Uplink Access Port; */
        EM_EVB_PT_URP,              /* Uplink Relay Port; */
        EM_EVB_PT_ALL
    }EM_EVB_PORT_TYPE;

    /* UAP port data structure; */
    typedef enum tagEnmCdcpRole
    {
        CDCP_B = 0,
        CDCP_S,
    }EMCdcpRole;

    typedef enum tagEnmCdcpState
    {
        CDCP_STATE_STOP = 0,
        CDCP_STATE_P_RUN,
    }EMCdcpState;

    /* EVB TLV协商出的数据@端口; */
    typedef struct tagEvbTlvNegResult
    {
        u8 ucVIDorGID;    /* the use of the GroupID orVlanID; */
        u8 ucEcpMaxRetries; /* ECP max retries;'ucR' */
        u8 ucRTE;         /* Retransmission Exponent used to ECP; ackTimerinit */
        u8 ucRWD;         /* Resource Wait Delay used to VDP; */
        u8 ucRKA;         /* Reinit Keep Alive used to VDP; */
        u8 ucStatus;      /* init:0   Success:1; */
        u8 ucRRCTR;       /* the state of the EVB Bridge’s operReflectiveRelayControl parameter; */
        u8 ucRRSTAT;      /* indicates the state of the EVB stations operReflectiveRelayStatus; */		
        /*u8 aucPad[2];*/
    }TEvbTlvNegResult;

    /* UAP port attribute; */
    typedef struct tagEvbUapAttr
    {
        bool bSchCdcpAdminEnable;
        EMCdcpRole emSchCdcpAdminRoleEnumerated;        /* {cdcpB,cdcpS} */
        u16 wSchCdcpAdminChnCap;                        /* 1-167 */
        u16 wSchCdcpOperChnCap;                         /* 1-167 */
        u32 uiSchAdminCdcpSvidPoolLow;                  /* [0,2...4094] */
        u32 SchAdminCdcpSvidPoolHigh;                   /* [0,2...4094] */
        EMCdcpState emSchOperStateEnumerated;           /* {cdcpStop,cdcpRun} */ 
        bool bSchCdcpRemoteEnabled;
        EMCdcpRole emSchCdcpRemoteRoleEnumerated;       /* {cdcpB,cdcpS} */
    }TEvbUapAttr;

    /* S-channel interface; */
    typedef struct tagSchInterface
    {
        u32 uiSchUapISSPortNo;
        u32 uiSchScid;
        u32 uiSchSvid;
        u32 uiSbpOrUrpPortNo;
    }TSchInterface;

    /* URP port attribute; */
    typedef struct tagEvbUrpAttr
    {
        /* EVB TLV; */
        bool bSGrpID;   /* station can support the use of the GroupID; */
        bool bRRReq;    /* EVB station’s adminReflectiveRelayRequest; */
        u8   ucR;      /* ECP max retries;'ucR'; */
        u8   ucRTE;    /* Retransmission Exponent used to ECP; for ackTimerinit; */
        u8   ucRKA;    /* Reinit Keep Alive used to VDP; */
        u8   ucRWD;    /* Resource Wait Delay used to VDP; */

        TEvbTlvNegResult tNegRes;
         /* LLDP; */
        bool bLldpManual;
        /* VDP; */
        u32  uiVdpOperRespWaitDelay;
        u32  uiVdpOperReinitKeepAlive;
        u32  uiVdpOperRsrcWaitDelay;
        /* ECP; */
        u8   ucEcpOperAckTimerInit;
        u8   ucEcpOperMaxTries;
        u64  ullEcpTxFrameCount;
        u64  ullEcpTxRetryCount;
        u64  ullEcpTxFailuret;
        u64  ullEcpRxFrameCount;
    }TEvbUrpAttr;

    /* SBP port attribute; */
    typedef struct tagEvbSbpAttr
    {
        /* EVB TLV; */
        bool bBGID;     /* use the GroupID in VDP exchanges@Bridge; */
        bool bRRCAP;    /* EVB Bridge’s reflectiveRelayCapable parameter; */
        bool bRRCTR;    /* EVB Bridge’s operReflectiveRelayControl; */
        u8   ucR;      /* ECP max retries;'ucR'; */
        u8   ucRTE;    /* Retransmission Exponent used to ECP; for ackTimerinit; */
        u8   ucRKA;    /* Reinit Keep Alive used to VDP; */
        u8   ucRWD;    /* Resource Wait Delay used to VDP; */

        TEvbTlvNegResult tNegRes;
        /* LLDP; */
        bool bLldpManual;
        /* VDP; */
        u32  uiVdpOperToutKeepAlive;
        u32  uiVdpOperRsrcWaitDelay;
        u32  uiVdpOperReinitKeepAlive;
        /* ECP; */
        u32  ucEcpOperAckTimerInit;
        u32  ucEcpOperMaxTries;
        u64  ullEcpTxFrameCount;
        u64  ullEcpTxRetryCount;
        u64  ullEcpTxFailuret;
        u64  ullEcpRxFrameCount;
    }TEvbSbpAttr;

    /* Evb port; */
    struct tagEvbPort
    {
        u32 uiPortNo;
        char acName[EVB_PORT_NAME_MAX_LEN];
        u8 ucEvbPortType;
        u8 aucPad[3];
        void *ptPortAttr;                       /* 端口属性数据; */

        struct tagEvbIssPort *pIssPort;         /* ISS port pointer; */ 

        struct tagEvbPort *pPortHashNext;
        LIST_ENTRY(tagEvbPort) lstEntry;

        /*ECP VDP */
        struct tagEcp * pEcp;

        struct tagVdp_b  * vdp_b;
        struct tagVdp_s  * vdp_s;

        /* Packet statistics; */
        TEvbPktStt *pStt;

        /* create evb port timestamp*/
        time_t tCreateTimeStamp;
    };
 
    /* EVB port functions declare; */
    int32 InitEvbIssPortModule(void);
    struct tagEvbIssPort *CreateEvbIssPort(u32 uiPortNo, const char *pPortName, u8 *pMac);
    int32 DeleteEvbIssPort(u32 uiPortNo);
    int32 DestroyEvbIssPortModule(void);
    struct tagEvbIssPort *Get1stEvbIssPort(void);
    struct tagEvbIssPort *GetNextEvbIssPort(struct tagEvbIssPort *pListPort);
    struct tagEvbIssPort *GetEvbIssPort(u32 uiPortNo);
    int32 SetEvbPortIssStat(u32 uiPortNo, u8 ucPortState);

    int32 InitEvbPortModule(void);
    struct tagEvbPort *CreateEvbPort(u8 ucEvbPortType, struct tagEvbIssPort *pIssPort);
    int32 DeleteEvbPort(u32 uiPortNo);
    int32 DestroyEvbPortModule(void);
    struct tagEvbPort *Get1stEvbPort(void);
    struct tagEvbPort *GetNextEvbPort(struct tagEvbPort *pListPort);
    struct tagEvbPort *GetEvbPort(u32 uiPortNo);

    struct tagEvbIssPort *GetIssPortFromEvbPort(u32 uiEvbPortNo);

#ifdef __cplusplus
}
#endif

#endif
