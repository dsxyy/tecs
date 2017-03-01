

#ifndef _ECP_H
#define _ECP_H

#ifdef __cplusplus
extern "C"{
#endif 

#include "evb_comm.h"
#include "evb_cfg.h"
    
#define ECP_PROTOCOL_TYPE (0x88DD)  // 临时写一个
#define ECP_ETHER_TYPE    (0x88DD)  // 临时写一个
#define ECP_VERSION       (0x01)
    
// ecp 报文 USCC 报文不能小于64个字节    
// #define ECP_MIN_DATA_LEN    (46)
#define ECP_MIN_DATA_LEN    (46+4)

#define EVB_ECP_MAC_LEN (6)
extern const u8 ECP_MULTICAST_MAC[EVB_ECP_MAC_LEN];

typedef enum {
    ECP_REQUEST = 0,
    ECP_ACK
} ecp_oper_type;

typedef enum {
    ECP_SUBTYPE_VDP = 0x01,
    ECP_SUBTYPE_PECSP
}ecp_sub_type;

#define ECP_SEQ_NUM_MIN (1)
#define ECP_SEQ_NUM_MAX (65536)

// 以下两个值为了方便调试;
#define ECP_TIMER_FREQUENCY        (160)  // (1000) expend 14
#define ECP_LOOP_TIMER_INTERVAL  (ECP_TIMER_FREQUENCY*EVB_TIMERVALUE_COEFFICIENT) // (1) //(100*MSECS)

typedef enum {
    ECP_TX_IDLE=0,
    ECP_TX_INIT_TRANSMIT,
    ECP_TX_TRANSMIT_ECPDU,
    ECP_TX_WAIT_FOR_REQ,
    ECP_TX_COUNT_ERR,
    NUM_ECP_TX_SM_STATES
}ECP_TX_SM_State;


typedef enum {
    ECP_TX_EVENT_BEGIN=0,
    ECP_TX_EVENT_REQ_RECV,
    ECP_TX_EVENT_RETRANSMIT,     /*acktimer == 0 && retries < maxRetries*/
    ECP_TX_EVENT_NOT_RETRANSMIT, /*acktimer == 0 && retries == maxRetries*/
    ECP_TX_EVENT_ACK_RECV_OK,    /*ackReceived && sequance == seqNum(ackECPDU)*/
    ECP_TX_EVENT_UCT,
    ECP_TX_EVENT_PORT_UP,
    ECP_TX_EVENT_PORT_DOWN,
    NUM_ECP_TX_EVNETS
}ECP_TX_SM_EVENT;
#define ecp_tx_sm_valid_event(E)\
    (((E) >= ECP_TX_EVENT_BEGIN) && ((E) <= ECP_TX_EVENT_PORT_DOWN))

typedef enum {
    ECP_RX_IDLE=0,
    ECP_RX_RECEVIE_WAIT,
    ECP_RX_RECEIVE_FIRST,
    ECP_RX_RECEIVE_ECPDU,
    ECP_RX_NEW_ECPDU,
    ECP_RX_SEND_ACK,
    NUM_ECP_RX_SM_STATES
}ECP_RX_SM_State;

typedef enum {
    ECP_RX_EVENT_BEGIN=0,
    ECP_RX_EVENT_ECPDU_RECV,
    ECP_RX_EVENT_SEQ_EQU_LAST,
    ECP_RX_EVENT_SEQ_NOTEQU_LAST,
    ECP_RX_EVENT_UCT,
    ECP_RX_EVENT_PORT_UP,
    ECP_RX_EVENT_PORT_DOWN,
    NUM_ECP_RX_EVNETS
}ECP_RX_SM_EVENT;

#define ecp_rx_sm_valid_event(E)\
    (((E) >= ECP_RX_EVENT_BEGIN) && ((E) <= ECP_RX_EVENT_PORT_DOWN))


struct tagEcpStats {
    /* Tx */
    DWORD32 statsFramesOutTotal;
    /* Rx */
    DWORD32 statsAgeoutsTotal;
    DWORD32 statsFramesDiscardedTotal;
    DWORD32 statsFramesInErrorsTotal;
    DWORD32 statsFramesInTotal;
    DWORD32 statsTLVsDiscardedTotal;
    DWORD32 statsTLVsUnrecognizedTotal;
};

struct tagEcpULPDU 
{
    LIST_ENTRY(tagEcpULPDU) lstEntry;
    struct tagEvbPort * port;
#define  PORT_NUM  port->uiPortNo
#define  PORT_MAC  port->pIssPort->aucMac
    UINT16 sequence;
    DWORD32 ackTimer;
    UINT16 retries;
    UINT16 requestReceived;
    UINT16 ackReceived;
    DWORD32 txErrors;

    UINT16 tx_state; 

    UINT16 ecpduReceived;
    UINT16 lastSeq;
    UINT16 seqECPDU;
    UINT16 rx_state;

    u32   maxTries; /* 最大传输次数; */

    struct tagEcpStats stats;

    UINT16 portEnabled;

    /* 报文; */
    DWORD32 ecpduLen;
    struct ecp_ecpdu * ecpdu;  
};

LIST_HEAD(tagEcpULPDU_head, tagEcpULPDU);

struct tagEvbIssPort;
struct tagEcp {
    struct tagEvbPort * port;
    LIST_ENTRY(tagEcp) lstEntry;

    struct tagEcpStats stats;
    // UINT16 portEnabled;
    UINT16 sSequence; /* 用于分配seq; */
    
    u32 ackTimer;  /* for RTE chg */

    struct tagEcpULPDU_head lstEcpULPDUtx;  
    struct tagEcpULPDU_head lstEcpULPDUrx;  

    BYTE evbtlvStatus;
};

LIST_HEAD(tagEcp_head, tagEcp);

#pragma pack(1)
#define EVB_ECP_ETHHEAD_LEN  (14)
struct ecp_ethhdr {
    u8 h_dest[EVB_ECP_MAC_LEN];
    u8 h_source[EVB_ECP_MAC_LEN];
    u16 h_proto;
};

#define EVB_ECP_ECPDU_HEAD_LEN (4)  /*ethType放入ecp_ethhdr中*/
struct ecp_ecpdu {
    //UINT16 ethType;
    UINT16 version : 4;
    UINT16 operType: 2;
    UINT16 subType : 10;
    UINT16 sequence;
    BYTE * ulpdu;  // 采用指针方式;
};

#pragma pack()

struct ecp_mod_ops {
    struct ecp_module *     (* ecp_mod_register)(void);
    void            (* ecp_mod_unregister)(struct ecp_module *);
    void            (* ecp_mod_ecpdu)(DWORD32 ifIndex,struct ecp_ecpdu * in_ecp,struct ecp_ecpdu * out_ecpdu, DWORD32 * len);
    void            (* ecp_mod_ifup)(DWORD32 ifIndex); 
    void            (* ecp_mod_ifdown)(DWORD32 ifIndex);
};

struct ecp_module {
    int id;     /* match tlv or oui+subtype */
    u8 enable;  /* TX only, RX only, TX+RX, Disabled */
    char *path;     /* shared library path */
    void *dlhandle; /* dlopen handle for closing */
    void *data; /* module specific data */
    const struct ecp_mod_ops *ops;
    LIST_ENTRY(ecp_module) ecp;
};

LIST_HEAD(tagEcpmod_head, ecp_module);


typedef struct _ecp {
    struct tagEcpmod_head ecpmod_head;
    struct tagEcp_head ecpif_head; 
    //struct edp_head mod_head;
    void ( *pfPortInit)(void);
    void ( *pfTmInit)(void);
    void ( *pfTmSet)(void);
    void ( *pfTmStop)(void);
    void ( *pfTmout)(void);    
    int  ( *pfSndPkt)(u32, u8*, u16);
    void ( *pfRcvPkt)(u32, u8*, u16);
}T_ECP;

extern T_ECP gtECP;

/*ecp function */
void ecp_tx_sm(struct tagEcpULPDU * ecp, ECP_TX_SM_EVENT event);
void ecp_rx_sm(struct tagEcpULPDU * ecp, ECP_RX_SM_EVENT event);

INT ecp_rx_sm_valid_state(UINT32 state);
INT ecp_tx_sm_valid_state(UINT32 state);

int list_EcpULPDUtx_insert(DWORD32 ifIndex,struct ecp_ecpdu * pIn, DWORD32 len);
int list_EcpULPDUtx_remove(DWORD32 ifIndex,struct ecp_ecpdu * pIn);
int list_EcpULPDUrx_insert(DWORD32 ifIndex,BYTE  * pIn, DWORD32 len);
int list_EcpULPDUrx_remove(DWORD32 ifIndex,struct ecp_ecpdu * pIn);

extern T_ECP *ecp_mod_init(void (*pfPortInit)(void),
    void (*pfTmInit)(void),
    void (*pfTmSet)(void),
    void (*pfTmStop)(void),
    int  (*pfSndPkt)(u32, u8*, u16));
extern void ecp_mod_shutdown(void);
int ecp_port_init(DWORD32 ifIndex);
int ecp_port_up(u32 ifIndex);
int ecp_port_down(u32 ifIndex);
int ecp_port_shutdown(DWORD32 ifIndex);
int GetECPSequenceNum(DWORD32 ifIndex, UINT16 *num);

int vdpecp_ifIndex_is_valid(DWORD32 ifIndex);
UINT16 ecpduHead_HTON(UINT16 ver, UINT16 ope, UINT16 sub,UINT16 seq, BYTE* outHead );
UINT16 ecpduHead_NTOH( struct ecp_ecpdu * inoutHead);
UINT16 ecpduHeadVerOpSub_HTON(UINT16 ver, UINT16 ope, UINT16 sub );

// get evbtlv 
int32 GetEcpVdp_evbtlvStatus(u32 port);

// ecp cfg acktimer maxtries;
u32 getEcpAckTimerInit(u32 port);
BYTE getEcpMaxTries(u32 port);

/* ecp evb tlv interface  */
int32 evbtlv_chg_set_ecp(u32 port, u32 flag);

// dbg
int GetDbgEcpPrintStatus();
void ecp_packet_print(u8 *p, u32 lenIn);
void ecp_mem_printf(u8 * p, u32 lenIn);
void DbgEcpEvent(int i);
int GetDbgEcpEvent();

// show 
void ShowEcpInfo(u32 port);
void ShowEcpRxListInfo(struct tagEvbPort *pPort);
void ShowEcpTxListInfo(struct tagEvbPort *pPort);

// test 
void t_ecp_init(u32 port);
void t_ecp_tx_sm(int e);
void t_ecp_list_tx_insert(u32 port);

void ecp_print(int i);
void ecp_pack_print(int i);

void DbgEcpPacket(int i);
int GetDbgEcpPacket();

#ifdef __cplusplus
}
#endif 
#endif // _ECP_H
