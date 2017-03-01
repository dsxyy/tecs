#if !defined(EVB_CFG_INCLUDE_H__)
#define EVB_CFG_INCLUDE_H__

#ifdef __cplusplus
extern "C"{
#endif 

#include "evb_comm.h"

    typedef enum {
        EVBCFG_TYPE_NOSUPPORT =0,
        EVBSYS_TYPE_BRIDGE =1,
        EVBSYS_TYPE_STATION
    }EVBSYS_TYPE;

// EVB CFG default value
#define EVBSYSCFG_DFTNUMEXTERNPORTS      4095
#define EVBSYSCFG_DFTECPACKTIMERINIT     20 //17 //14    
#define EVBSYSCFG_DFTECPMAXTRIES         3 
#define EVBSYSCFG_DFTVDPRSRCWAITDELAY    20 
#define EVBSYSCFG_DFTVDPREINITKEEPALIVE  20 

// for vdp ecp timeout count
#define EVB_TIMERVALUE_COEFFICIENT      (100)  /* Millisecond */

/* evbtlv change flag */
#define EVBTLV_CHG_BGID     (0x01)
#define EVBTLV_CHG_RRCAP    (0x01 << 1)
#define EVBTLV_CHG_RRCTR    (0x01 << 2)
#define EVBTLV_CHG_SGID     (0x01 << 3)
#define EVBTLV_CHG_RRREQ    (0x01 << 4)
#define EVBTLV_CHG_RRSTAT   (0x01 << 5)
#define EVBTLV_CHG_R        (0x01 << 6)
#define EVBTLV_CHG_RTE      (0x01 << 7)
#define EVBTLV_CHG_EVB_MODE (0x01 << 8)
#define EVBTLV_CHG_ROL1     (0x01 << 9)
#define EVBTLV_CHG_RWD      (0x01 << 10)
#define EVBTLV_CHG_ROL2     (0x01 << 11)
#define EVBTLV_CHG_RKA      (0x01 << 12)
#define EVBTLV_CHG_VIDCONTROL (0x01 << 13)
#define EVBTLV_ISSUPPORT       (0x01 << 14)
#define EVBTLV_CHG_ALL      (0xffffffff)
    /*EVB CFG interface */
    typedef struct tagEvbCfgSysBase
    {
        bool evbSysEvbLldpTxEnable;   // TRUE
        bool evbSysEvbLldpGidCapable;
        bool evbSysEvbLldpManual;     // FALSE
        UINT16 evbSysNumExternalPorts; // [0~4095]
        BYTE evbSysType;
        BYTE evbSysEcpDftAckTimerInit;// 14
        BYTE evbSysEcpDftMaxTries;      // 3 

        BYTE evbSysVdpDftRsrcWaitDelay; // 20
        BYTE evbSysVdpDftReinitKeepAlive; //20 

        BYTE ucPad0;

        /* evbtlv config */
        bool bBGID;    /* use the GroupID in VDP exchanges@Bridge; */
        bool bRRCAP;   /* EVB Bridge’s reflectiveRelayCapable parameter; */
        bool bRRCTR;   /* EVB Bridge’s operReflectiveRelayControl; */
        bool bSGrpID;  /* station can support the use of the GroupID; */
        bool bRRReq;   /* EVB station’s adminReflectiveRelayRequest; */
        u8   ucR;      /* the maximum number of times that the ECP transmit state machine will retry a transmission if no ACK;*/
        u8   ucRTE;    /* RTE is an EVB link or S-channel attribute used to calculate the minimum ECPDU retransmission time; */
        u8   ucRKA;    /* by the EVB station indicates the exponent value in use by the EVB station for determining the value of the reinitKeepAlive variable; */
        u8   ucRWD;    /* by the EVB Bridge and EVB station indicate the exponent value that each device proposes for determining the value of the resourceWaitDelay; */
        /* rol和rrstat对端暂且不关心，暂不添加*/
        BYTE ucPad1[3];
    }T_EVBCFG_SYSBASE; 

    typedef struct tagEvbCfgProfile
    {
        int profile;
    }T_EVBCFG_PROFILE;
    
    /* evb cfg global */
    extern T_EVBCFG_SYSBASE g_evbcfg_sys;

    /* evb cfg function */
    void evbcfg_init();
    void evbcfg_shutdown();
    
    u32 evbcfg_getEcpAckInit();
    u8 evbcfg_getEcpMaxTries();
    u32 evbcfg_getVdpRsrcWaitDelay();
    u32 evbcfg_getVdpReinitKeepAlive();

    int32 EvbcfgGetSysType();
    void  EvbcfgSetSysType(BYTE sysType);
    int32 EvbCfgSetRRReq(bool bRR);
    
    void evbcfg_show();

    /* evb tlv notify interface */
    int32  evbtlv_chg_notify(u32 port,u32 flag);
#ifdef __cplusplus
}
#endif 

#endif


