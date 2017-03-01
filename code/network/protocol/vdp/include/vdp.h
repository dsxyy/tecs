

#ifndef _EVB_VDP_H_
#define _EVB_VDP_H_

#ifdef __cplusplus
extern "C"{
#endif 

#include "evb_comm.h"
#include "ecp.h"
#include "evb_cfg.h"

#define VDP_TIMER_FREQUENCY        200//1000
#define VDP_LOOP_TIMER_INTREVAL      (VDP_TIMER_FREQUENCY*EVB_TIMERVALUE_COEFFICIENT )

#define VDP_PACKET_LEN_MAX        (1490)

#define VDPASSTLV_REASON_M   (0x10)
#define VDPASSTLV_REASON_S   (0x20)
#define VDPASSTLV_REASON_RA  (0x40)

#define VDPASSTLV_REASON_REQ (0)
#define VDPASSTLV_REASON_ACK (1)
    
typedef enum {
    VSISTATE_FAILED = 0,
    VSISTATE_SUCCESS,
    VSISTATE_RUNNING,
}VDP_VSISTATE_VALUE;

typedef enum {
    VDP_B_RESOUCECMD_RESULT_NULL = 0,
    VDP_B_RESOUCECMD_RESULT_SUCCESS,
    VDP_B_RESOUCECMD_RESULT_KEEP,
    VDP_B_RESOUCECMD_RESULT_DEASSOCIATE,
    VDP_B_RESOUCECMD_RESULT_TIMEOUT,
}VDP_B_RESOUCECMD_RESULT;

typedef enum {
    VDPTLV_TYPE_PRE_ASSOCIATE=1,
    VDPTLV_TYPE_PRE_ASSOCIATE_WITH_RR,
    VDPTLV_TYPE_ASSOCIATE,
    VDPTLV_TYPE_DEASSOCIATE,
    VDPTLV_TYPE_VSI_MGRID,
    VDPTLV_TYPE_ORG_DEFINE_TLV=0x7F
}VDPTLV_TYPE;

typedef enum {
    VDP_VSISTATE_PREASSOCIATE=VDPTLV_TYPE_PRE_ASSOCIATE,
    VDP_VSISTATE_PREASSOCIATE_RR=VDPTLV_TYPE_PRE_ASSOCIATE_WITH_RR,
    VDP_VSISTATE_ASSOCIATE = VDPTLV_TYPE_ASSOCIATE,
    VDP_VSISTATE_DEASSOCIATE=VDPTLV_TYPE_DEASSOCIATE
}VDP_VSISTATE;

// VDP ERROR TYPES OF VDP RESPONSE
typedef enum {
    VDPRESPONSE_ERR_SUC=0,
    VDPRESPONSE_ERR_INVALID_FORMAT,
    VDPRESPONSE_ERR_INSUFF_RESOURCE,
    VDPRESPONSE_ERR_UNABLE_CONN_VSI_MGR,
    VDPRESPONSE_ERR_OTHER_FAIL,
    VDPRESPONSE_ERR_INVALID_VID_GRPID_MAC,
    NUM_VDPRESPONSE_ERR_TYPES,
}VDPRESPONSE_ERR_TYPE;

#define vdpresponse_err_type_valid(S)\
    ((S) >= VDPRESPONSE_ERR_SUC && (S) <= VDPRESPONSE_ERR_INVALID_VID_GRPID_MAC)

// VSIID FORMAT VALUES
typedef enum {
    VSIID_FORMAT_IPV4=0x01,
    VSIID_FORMAT_IPV6,
    VSIID_FORMAT_MAC,
    VSIID_FORMAT_LOCAL,
    VSIID_FORMAT_UUID,
    NUM_VSIID_FORMAT_VALUES,
}VSIID_FORMAT;

#define vsiid_format_values_valid(S)\
    ((S) >= VSIID_FORMAT_IPV4 && (S) <= VSIID_FORMAT_UUID)

// FILTER INFO FORMAT VALUES
typedef enum {
    VDP_FLTINFO_FORMAT_VID=0x01,
    VDP_FLTINFO_FORMAT_MACVID,
    VDP_FLTINFO_FORMAT_GROUPVID,
    VDP_FLTINFO_FORMAT_MACGROUPVID,
    NUM_VDP_FLTINFO_FORMAT_VALUES,
}VDP_FLTINFO_FORMAT;

#define vdp_fltinfo_format_values_valid(S)\
    ((S) >= VDP_FLTINFO_FORMAT_VID && (S) <= VDP_FLTINFO_FORMAT_MACGROUPVID)


#pragma pack(1)
// VID FILTER INFO FORMAT
struct tagVDP_FLT_VID{
    // UINT16 number;
    UINT16 ps:1;
    UINT16 pcp:3;
    UINT16 vid:12;
};

// MAC/VID FILTER INFO FORMAT
#define EVB_VDP_MAC_LEN (6)
struct tagVDP_FLT_MACVID{
    // UINT16 number;
    BYTE mac[EVB_VDP_MAC_LEN];
    UINT16 ps:1;
    UINT16 pcp:3;
    UINT16 vid:12;
};
// GROUP/VID FILTER INFO FORMAT
#define EVB_VDP_GROUPID_LEN (4)
struct tagVDP_FLT_GROUPVID{
    // UINT16 number;
    BYTE groupID[EVB_VDP_GROUPID_LEN];
    UINT16 ps:1;
    UINT16 pcp:3;
    UINT16 vid:12;
};
 
// GROUPMACVID FILTER INFO FORMAT
struct tagVDP_FLT_GROUPMACVID{
    // UINT16 number;
    BYTE groupID[EVB_VDP_GROUPID_LEN];
    BYTE mac[EVB_VDP_MAC_LEN];
    UINT16 ps:1;
    UINT16 pcp:3;
    UINT16 vid:12;
};

#define VSI_MGR_ID_ZERO (0)

#define VDPTLV_HEAD_LEN (2)
#define VDPTLV_VSIMGRID_LEN (16)
struct tagVdpTlv_head{
    UINT16 tlvType:7;
    UINT16 tlvLen:9;
};

struct tagVdpTlv_VSIMgrID{
    struct tagVdpTlv_head tlvHead;
    BYTE   mgr_id[VDPTLV_VSIMGRID_LEN];
};

#define VDPTLV_VSIID_LEN (16)
#define VDPTLV_VSITYPEID_LEN (3)

struct tagVdpTlv_VDPAss_Reason{
    BYTE error:4;
    /*
    union b5{
        BYTE m:1;
        BYTE hard:1;
    }_b5;
    union b6{
        BYTE s:1;
        BYTE keep:1;
    }_b6;*/
    BYTE m_hard:1;
    BYTE s_keep:1;
    BYTE ReqAck:1;
    BYTE reserved:1;
};

/*VDP TLV = 23 + M octoer*/
#define VDPTLV_VDPTLV_NOFLT_LEN (23)
struct tagVdpTlv_VDPAss{
    struct tagVdpTlv_head tlvHead;
    struct tagVdpTlv_VDPAss_Reason  reason;
    BYTE  vsiTypeID[VDPTLV_VSITYPEID_LEN];
    //BYTE  vsiTypeIDVer;
    BYTE  vsiTypeVer;
    BYTE  vsiIDFormat;
    BYTE  vsiID[VDPTLV_VSIID_LEN];
    BYTE  fltFormat;
    // BYTE * fltInfo;
    BYTE  fltInfo[0];
};

#define VDP_TLV_ORG_OUI_LEN (3)
struct tagVdpTlv_OrgDfn{
    struct tagVdpTlv_head tlvHead;
    BYTE oui[VDP_TLV_ORG_OUI_LEN];
    // BYTE *orgInfo;
    BYTE orgInfo[0];
};

#pragma pack()
struct tagVdp_s;
struct tagVsi_instance_s
{
    struct tagVdp_s *vdp;
#define S_VDPPORTNUM vdp->PORT_NUM
    LIST_ENTRY(tagVsi_instance_s) lstEntry;
    BYTE vsiIDFormat;
    BYTE vsiID[VDPTLV_VSIID_LEN];

    /* evb cfg */
    u32  respWaitDelay;
    u32  reinitKeepAlive;

    /*prf chg*/
    u32  profile_chg;

    // BYTE cur_state;
    BYTE cfg_state;

    BYTE state;
    BYTE * operCmd;
    DWORD32 operCmd_Len;
    BYTE  operCmd_TlvType;
    UINT16 vsiState;
    UINT16 newCmd;
    UINT16 cancelCmd;
    BYTE * rxResp;
    BYTE * sysCmd;
    DWORD32 sysCmd_len;
    BYTE  sysCmd_TlvType;
    DWORD32 waitWhile;
};

LIST_HEAD(tagVsi_inst_s_head, tagVsi_instance_s);

struct tagVsi_instance_b
{
    struct tagVdp_b *vdp;
#define B_VDPPORTNUM vdp->PORT_NUM
    LIST_ENTRY(tagVsi_instance_b) lstEntry;
    BYTE vsiIDFormat;
    BYTE vsiID[VDPTLV_VSIID_LEN];
    BYTE vsimgrID[VDPTLV_VSIMGRID_LEN];

    /*vdp cfg*/
    u32 resouceWaitDelay;
    u32 toutKeepAlive;

    // BYTE cfg_state;
    BYTE state;
    BYTE * operCmd;
    DWORD32 operCmd_Len;
    UINT16  operCmd_TlvType;

    UINT16 vsiState;
    BYTE * rxCmd;
    DWORD32 rxCmd_Len;
    UINT16  rxCmd_TlvType;
    BYTE   bridgeExit;
    BYTE * Resp;
    DWORD32 Resp_Len;
    UINT16  Resp_TlvType;
    UINT16 resouceResult;
    DWORD32 waitWhile;
};

LIST_HEAD(tagVsi_inst_b_head, tagVsi_instance_b);

struct tagVdp_b
{
    struct tagEvbPort * port;
#define  PORT_NUM  port->uiPortNo
#define  PORT_MAC  port->pIssPort->aucMac
    struct tagVsi_inst_b_head lstVsi;

    u32 resouceWaitDelay;
    u32 toutKeepAlive;

    BYTE evbtlvStatus; // evb tlv neg result
};

struct tagVdp_s
{
    struct tagEvbPort * port;
#define  PORT_NUM  port->uiPortNo
#define  PORT_MAC  port->pIssPort->aucMac

    struct tagVsi_inst_s_head lstVsi;
 
    u32 respWaitDelay;   // for evb tlv chg
    u32 reinitKeepAlive;

    BYTE evbtlvStatus; // evb tlv neg result
};

typedef enum {
    VDP_B_SM_IDLE=0,
    VDP_B_SM_INIT,
    VDP_B_SM_BRIDGE_PROC,
    VDP_B_SM_SUCCESS,
    VDP_B_SM_KEEP,
    VDP_B_SM_DEASSOC,
    VDP_B_SM_DEASSOCIATED,
    VDP_B_SM_RELEASE_ALL,
    VDP_B_SM_WAIT_STATION_CMD,
    VDP_B_SM_PROCESS_KEEPALIVE,
    NUM_VDP_B_SM_STATES,
}VDP_B_SM_State;

typedef enum {
    VDP_B_EVENT_BEGIN=0,
    VDP_B_EVENT_BRIDGE_EXIT,
    VDP_B_EVENT_RXCMD_VALID,
    VDP_B_EVENT_RR_SUCCESS,
    VDP_B_EVENT_RR_KEEP,
    VDP_B_EVENT_RR_DEASSOCIATE,
    VDP_B_EVENT_RR_TIMEOUT,  
    VDP_B_EVENT_UCT,
    VDP_B_EVENT_RXCMD_NOTEQU_OPERCMD, /*rxCmd!=NULL && rxCmd.exceptZero!=operCmd*/
    VDP_B_EVENT_RXCMD_EQU_OPERCMD,   /* rxCmd.exceptZero==operCmd*/
    VDP_B_EVENT_WAITWHILE_EQU_ZERO,
    VDP_B_EVENT_PORT_UP,
    VDP_B_EVENT_PORT_DOWN,
    VDP_B_EVENT_EVBTLV_NO_SUPPORT,
    NUM_VDP_B_SM_EVENTS,
}VDP_B_SM_EVENT;

#define vdp_b_sm_event_valid(S)\
    (((S) >= VDP_B_EVENT_BEGIN ) && ((S) <= VDP_B_EVENT_EVBTLV_NO_SUPPORT))

typedef enum {
    VDP_S_SM_IDLE=0,
    VDP_S_SM_INIT,
    VDP_S_SM_TRANSMIT_DEASSOC,
    VDP_S_SM_STATION_PROC,
    VDP_S_SM_PREASSOC_NEW,
    VDP_S_SM_ASSOC_NEW,
    VDP_S_SM_ASSOC_COMPLETE,
    VDP_S_SM_WAIT_SYS_CMD,
    VDP_S_SM_TRANSMIT_KEEPALIVE,
    NUM_VDP_S_SM_STATES,
}VDP_S_SM_State;

typedef enum {
    VDP_S_EVENT_BEGIN=0,
    VDP_S_EVENT_NEWCMD,
    VDP_S_EVENT_PREASSOC_NEW, /*STATION_PROCESSING rxResp==sysCmd.exceptReason.ExceptZero && rxResp.Reason==0 && tlvType==PreAssoc,==PreAssocR*/
    VDP_S_EVENT_ASSOC_NEW, /*rxResp==sysCmd.exceptReason.ExceptZero && rxResp.Reason==0 && tlvType==Assoc*/
    VDP_S_EVENT_ASSOC_COMPLETE,  /*rxResp==sysCmd.exceptReason.ExceptZero && rxResp.Keep==TRYE && tlvType==Assoc && visState==ASSOC*/
    VDP_S_EVENT_UCT,
    VDP_S_EVENT_WAITWHILE_EQU_ZERO,  /*waitWhile==0*/
    VDP_S_EVENT_RXRESP_TLVTYPE_EQU_DEASSOC, /*rxResp.TLVtype==DEASSOC*/
    //VDP_S_EVENT_TRANSMIT_DEASSOC, /*rxResp==operCmd.exceptReason.ExceptZero && rxResp.TLVtype==ASSOC && rxResp.keep==TRUE */
    VDP_S_EVENT_RXRESP_TVLTYPE_EQU_ASSOC_AND_KEEP, /*rxResp==operCmd.exceptReason.ExceptZero && rxResp.TLVtyp==ASSOC && rxResp.keep=TRUE */
    VDP_S_EVENT_RXRESP_REASON_EQU_ZERO, /*rxResp==operCmd.exceptReason.ExceptZero && rxResp.Reason==0*/
    VDP_S_EVENT_OPERCMD_EXCEPTREASON_ELSE,/*rxResp==operCmd.exceptReason && ELSE*/
    VDP_S_EVENT_SYSCMD_EXCEPTREASON_ELSE,/*rxResp==sysCmd.exceptReason && ELSE*/
    VDP_S_EVENT_RXRESP_NOT_NULL, /*rxResp != NULL*/
    VDP_S_EVENT_PORT_UP,
    VDP_S_EVENT_PORT_DOWN,
    //VDP_S_EVENT_CMD_CANCEL,
    NUM_VDP_S_SM_EVENTS,
}VDP_S_SM_EVENT;

#define vdp_s_sm_event_valid(S)\
    (((S) >= VDP_S_EVENT_BEGIN) && ((S) <= VDP_S_EVENT_PORT_DOWN))

#define MAX_VDP_B_PERPORT_VSI_NUM (10)
#define MAX_VDP_S_PERPORT_VSI_NUM (10)

typedef struct _vdp {
    void ( *pfTmInit)(void);
    void ( *pfTmSet)(void);
    void ( *pfTmStop)(void);
    void ( *pfTmout)(void);
}T_VDP;

/*CmdPool*/
struct tagVsiCmdPoolItem
{
    LIST_ENTRY(tagVsiCmdPoolItem) lstEntry;
    BYTE vsiIDFormat;
    BYTE vsiID[VDPTLV_VSIID_LEN];
    BYTE cmdTlvType; /*Pre,Pre_RR,Ass,DeAss*/
    u32 port;
    BYTE newFlag; /*DeAss 需要确保VSI执行一次后能释放;*/
};

LIST_HEAD(tagVsiCmdPoolItem_head, tagVsiCmdPoolItem);

/* marco */
#define PRINTF_LOOPVAR_DEFINE int i = 0;
#define PRINTF_VSIID(vsi) { \
for(i =0;i<VDPTLV_VSIID_LEN;i++)\
PRINTF("%2.2x ",vsi[i]);\
PRINTF("\n");\
}

#define PRINTF_MGRID(mgrid) { \
    for(i =0;i<VDPTLV_VSIMGRID_LEN;i++)\
    PRINTF("%2.2x ",mgrid[i]);\
    PRINTF("\n");\
}

#define PRINTF_TYPEID(typeID) { \
    for(i =0;i<VDPTLV_VSITYPEID_LEN;i++)\
    PRINTF("%2.2x ",typeID[i]);\
    PRINTF("\n");\
}

#define VSIIDSPRINTF "%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x"
#define MAX_VSIID_STR_LEN (VDPTLV_VSIID_LEN*3)

/* global var */
extern T_VDP gtVDP;
extern struct tagVsiCmdPoolItem_head g_lstVsiCmdPool;

/* vdp function  */
extern struct ecp_module * vdp_register(void);
extern void vdp_unregister(struct ecp_module *mod);

void vdp_s_loop_timer_handler(struct tagVdp_s * vdp_s);
void vdp_b_loop_timer_handler(struct tagVdp_b * vdp_b);
INT list_vdp_s_insert(DWORD32 ifIndex, struct tagVsi_instance_s * pNewVsi);
INT list_vdp_s_remove(DWORD32 ifIndex, struct tagVsi_instance_s * pVsi);

void vdp_rx_ReceiveFrame(u32 local_port, u8* buf, u16 len);
void vdp_s_rx_ReceiveFrame(struct tagVdp_s *,u32 local_port, u8* buf, u16 len);
void vdp_b_rx_ReceiveFrame(struct tagVdp_b *,u32 local_port, u8* buf, u16 len);

T_VDP *vdp_mod_init(void (*pfTmInit)(void),
    void (*pfTmSet)(void),
    void (*pfTmStop)(void));
void vdp_mod_shutdown();
INT vdp_port_init(DWORD32 ifIndex);
INT vdp_port_up(u32 ifIndex);
INT vdp_port_down(u32 ifIndex);
INT vdp_port_shutdown(DWORD32 ifIndex);

const char* GetVdpTlvTypeString(UINT16 type);

/* check */
INT vdp_b_sm_state_valid(UINT32 state);
INT vdp_s_sm_state_valid(UINT32 state);
INT vdp_opertype_is_valid(BYTE type);
struct tagVsiProfile;
INT vsiProfile_to_vsitlv(struct tagVsiProfile* vsiPro, BYTE vdptlvType,BYTE * outTlv,DWORD32 * outLen);

int32 initDftVsiB(struct tagVsi_instance_b * vsi);
int32 initDftVsiS(struct tagVsi_instance_s * vsi);

/* calc timer  */
u32 vdpGetSRespWaitDelay(u32 port);
u32 vdpGetSReinitKeepAlive(u32 port);
u32 vdpGetBRourceWaitDelay(u32 port);
u32 vdpGetBtoutKeepAlive(u32 port);

#if 0
u32 evbcalc_vdpSRespWaitDelay(u32 vdpResourceWaitDly,u32 ecpMaxTries,u32 ecpAckTimerInit);
u32 evbcalc_vdpSReinitKeepAlive(u32 vdpReinitKeepAlive);
u32 evbcalc_vdpBRourceWaitDelay(u32 vdpResourceWaitDly);
u32 evbcalc_vdpBtoutKeepAlive(u32 vdpReinitKeepalive,u32 ecpMaxTries,u32 ecpAckTimerInit);
#endif 

DWORD32 GetVdpMgrIDTlvLen();
DWORD32 GetVdpOrgDfnTlvLen();
INT buildMgrIDTlv(BYTE mgrID[VDPTLV_VSIMGRID_LEN],BYTE * outTlv, DWORD32 * outLen);
INT buildOrgDfntlv(BYTE oui[VDP_TLV_ORG_OUI_LEN],BYTE * outTlv, DWORD32 * outLen);

/* tlv head hton */
UINT16 vdpTlvHead_HTON(UINT16 type, UINT16 len );
UINT16 vdpTlvHead_NTOH(struct tagVdpTlv_head * inout);

/* flt hton */
INT32 vdpfltVID_htons(struct tagVDP_FLT_VID * pV);
INT32 vdpfltMVID_htons(struct tagVDP_FLT_MACVID * pV);
INT32 vdpfltGROUPVID_htons(struct tagVDP_FLT_GROUPVID * pV);
INT32 vdpfltGROUPMACVID_htons(struct tagVDP_FLT_GROUPMACVID * pV);

INT32 vdpfltVID_ntohs(struct tagVDP_FLT_VID * pV);
INT32 vdpfltMVID_ntohs(struct tagVDP_FLT_MACVID * pV);
INT32 vdpfltGROUPVID_ntohs(struct tagVDP_FLT_GROUPVID * pV);
INT32 vdpfltGROUPMACVID_ntohs(struct tagVDP_FLT_GROUPMACVID * pV);

/* vdp evb tlv interface  */
int32 evbtlv_chg_set_vdp(u32 port, u32 flag);
INT vdp_GetEvbTlvNegResult(u32 port);

/*vdp profile interface */
int32 vsiProfile_chg_notify_vdp(BYTE vsiIDFormat, BYTE vsiID[VDPTLV_VSIID_LEN]);

/*cmd pool*/
int32 vsiCmd_pool_add(u32 port,BYTE vsiidFormat,BYTE vsiID[VDPTLV_VSIID_LEN],BYTE oper);
int32 vsiCmd_pool_del(u32 port,BYTE vsiidFormat,BYTE vsiID[VDPTLV_VSIID_LEN],BYTE oper);
int32 vsiCmd_pool_del_notlvtype(u32 port,BYTE vsiidFormat,BYTE vsiID[VDPTLV_VSIID_LEN]);
int32 vsiCmd_pool_lookup(u32 port,BYTE vsiidFormat,BYTE vsiID[VDPTLV_VSIID_LEN]);

/* vdp vm interface */
void vdp_vm_api(UINT32 ifIndex,BYTE vsiIDFormat, BYTE vsiID[VDPTLV_VSIID_LEN], BYTE oper);
int32 vdp_lookup_vsiState(u32 ifIndex,BYTE vsiIDFormat, BYTE vsiID[VDPTLV_VSIID_LEN]);
int32 vdp_delete_vsi_and_cmdPool(u32 uiURPNo,BYTE vsiIDFormat, BYTE vsiID[VDPTLV_VSIID_LEN]);

/* dbg */
int GetDbgVdpPrintStatus();
void ShowVdpInfo(u32 port);
void DbgShowCmdPool(u32 port);

void DbgVdpPacket(int i);
int GetDbgVdpPacket();

void DbgVdpEvent(int i);
int GetDbgVdpEvent();
void DbgVdpCmdPool(int i);
int GetDbgVdpCmdPool();
void DbgVdpProfile(int i);
int GetDbgVdpProfile();

char * GetVsiidstr(BYTE vsiID[VDPTLV_VSIID_LEN]);

/* test  */
void vdp_print(int i);
void t_cfg_SB(BYTE sb);
void t_vsiProfile_init();
void t_vsiProfile_AddDelflt(u32 tNo, BYTE mac0,BYTE mac1,BYTE mac2,BYTE mac3,BYTE mac4,BYTE mac5,u32 grpid,u16 vlan,BYTE opt);

void t_vdp_init(u32 port);

void t_vdp_init_S(u32 port);
void t_vdp_init_B(u32 port);
void t_vsi_preAss(u32 port,BYTE vidFormat,u32 vidIn);
void t_vsi_preAssR(u32 port,BYTE vidFormat,u32 vidIn);
void t_vsi_Ass(u32 port,BYTE vidFormat,u32 vidIn);
void t_vsi_DeAss(u32 port,BYTE vidFormat,u32 vidIn);

void t_byteorder();

void t_newCmd(u32 port, u32 tVidNo, u16 vlan, BYTE gid,u32 tMacNo,BYTE oper);

int32 test_del_vsi(u32 uiURPNo);

#ifdef __cplusplus
}
#endif 

#endif 

