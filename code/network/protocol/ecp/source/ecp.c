
#include "evb_comm.h"
#include "ecp.h"
#include "ecp_in.h"
#include "vdp.h"
#include "evb_cfg.h"
#include "evb_port_mgr.h"

#ifdef __cplusplus
extern "C"{
#endif 

// 红祥临时要求 将ecp报文组播地址修改下 
//const u8 ECP_MULTICAST_MAC[EVB_ECP_MAC_LEN] = {0x01,0x80,0xc2,0x00,0x00,0x2e};
const u8 ECP_MULTICAST_MAC[EVB_ECP_MAC_LEN] = {0x01,0x80,0xc2,0x00,0x00,0x00};

T_ECP gtECP;

struct ecp_module *(*register_ecp_table[])(void) = {
    vdp_register, 
};

extern void DbgMemShow(void *pMem, WORD32 dwSz);
extern void ecp_tx_loop_timer_handler(struct tagEcp * pEcp);
extern void ecp_rx_loop_timer_handler(struct tagEcp * pEcp);

void init_ecpULDPU(u32 port,struct tagEcpULPDU * pEcpULPDU);

// 从层2接受到报文接口
void ecp_rx_ReceiveFrame(u32 local_port, u8* buf, u16 len)
{   
    /* 内部分为request 和 ack，分别调用不同的状态机; */
    struct ecp_ethhdr *hdr = NULL;
    struct tagEcpULPDU * pTmp = NULL;
    struct tagEcpULPDU * pNewEcpULPDU = NULL;
    struct ecp_ecpdu * pNewEcpdu = NULL;

    BYTE * buf_ecpdu = NULL;
    u16  len_ecpdu = 0;
    struct ecp_ecpdu ed;

    struct tagEvbPort * port = NULL;
            
    if(NULL == buf)
    {
        return;
    }
    hdr = (struct ecp_ethhdr *)buf;

    if(GetDbgEcpPacket())
    {
        PRINTF("port %u, ECP RX  ",local_port);
        PrintLocalTimer();
        DbgMemShow(buf,len);
    }

    MEMSET(&ed,0, sizeof(ed));
    
    // check 
    if( EVB_NTOHS(hdr->h_proto) != ECP_PROTOCOL_TYPE)
    {
        return;
    }

    port = GetEvbPort(local_port);
    if( NULL == port)
    {
        EVB_LOG(EVB_LOG_ERROR, "ecp_rx_ReceiveFrame:GetEvbPort(%u) failed.\n", local_port);
        return;
    }
    
    EVB_ASSERT(len >= (EVB_ECP_ECPDU_HEAD_LEN +EVB_ECP_ETHHEAD_LEN));
    if( len < (EVB_ECP_ECPDU_HEAD_LEN +EVB_ECP_ETHHEAD_LEN) )
    {
        EVB_LOG(EVB_LOG_ERROR, "ecp_rx_ReceiveFrame:packet len < (%u) failed.\n", (EVB_ECP_ECPDU_HEAD_LEN +EVB_ECP_ETHHEAD_LEN));
        return;
    }
    
    buf_ecpdu = (buf + EVB_ECP_ETHHEAD_LEN);
    //pEcpdu = (struct ecp_ecpdu *)(buf_ecpdu);
    // ecpduHead_HTON()

    MEMCPY(&ed, buf_ecpdu,EVB_ECP_ECPDU_HEAD_LEN);
    ecpduHead_NTOH(&ed);

    len_ecpdu = len - EVB_ECP_ETHHEAD_LEN;
        
    // 0 check ecpdu pack info (version subtype ...) 
    if( ed.version != ECP_VERSION)
    {
        EVB_LOG(EVB_LOG_ERROR, "ecp_rx_ReceiveFrame:ecp packet format(ver:%d) error.\n",ed.version);
        return;
    }

    if(ed.subType != ECP_SUBTYPE_VDP && ed.subType != ECP_SUBTYPE_PECSP )
    {
        EVB_LOG(EVB_LOG_ERROR, "ecp_rx_ReceiveFrame:ecp packet format(subType:0x%x) error.\n", ed.subType);
        return;
    }

    switch(ed.operType)
    {
    case ECP_REQUEST:
        {
            // 1 request check 后面添加; 
            if( GetDbgEcpPrintStatus())
            {
                PRINTF("port:%u, ecp rcv request - seq %d \n", local_port,ed.sequence );
            }

            if( 0 != list_EcpULPDUrx_insert(local_port,buf_ecpdu, len_ecpdu ) )
            {
                EVB_LOG(EVB_LOG_ERROR, "ecp_rx_ReceiveFrame:list_EcpULPDUrx_insert failed.\n");
                return;
            }

            /* ack */
            pNewEcpULPDU = (struct tagEcpULPDU *)GLUE_ALLOC(sizeof(struct tagEcpULPDU ));
            if( NULL == pNewEcpULPDU)
            {
                EVB_LOG(EVB_LOG_ERROR, "ecp_rx_ReceiveFrame:ecp_request allow memory failed.\n");
                return;
            }
            pNewEcpdu = (struct ecp_ecpdu *)GLUE_ALLOC(sizeof(struct ecp_ecpdu ));
            if( NULL == pNewEcpdu)
            {
                EVB_LOG(EVB_LOG_ERROR, "ecp_rx_ReceiveFrame:ecp_request allow memory failed.\n");
                return;
            }

            init_ecpULDPU(local_port,pNewEcpULPDU);
            MEMCPY(pNewEcpdu,buf_ecpdu,sizeof(struct ecp_ecpdu));
            //MEMCPY(pNewULP,buf_ecpdu + EVB_ECP_ECPDU_HEAD_LEN, len_ecpdu - EVB_ECP_ECPDU_HEAD_LEN );

            ecpduHead_HTON(ECP_VERSION,ECP_ACK,ed.subType,ed.sequence,(BYTE*)pNewEcpdu);
            pNewEcpdu->ulpdu = NULL; 
            pNewEcpULPDU->ecpdu = pNewEcpdu;
            pNewEcpULPDU->ecpduLen = EVB_ECP_ECPDU_HEAD_LEN; //len_ecpdu;
            pNewEcpULPDU->sequence = ed.sequence;
            pNewEcpULPDU->port = port; 
            
            ecp_rx_sm(pNewEcpULPDU, ECP_RX_EVENT_BEGIN);

            // 释放缓存;
            if( NULL != pNewEcpULPDU->ecpdu->ulpdu)
            {
                GLUE_FREE(pNewEcpULPDU->ecpdu->ulpdu);
            }
            GLUE_FREE(pNewEcpULPDU->ecpdu);
            GLUE_FREE(pNewEcpULPDU);
        }
        break;
    case ECP_ACK:
        {
            if( GetDbgEcpPrintStatus())
            {
                PRINTF("ecp rcv ack - port %u, seq %d \n", local_port,ed.sequence );
            }

            // 遍历port的缓存队列,是否ack;
            LIST_FOREACH(pTmp, &port->pEcp->lstEcpULPDUtx, lstEntry)
            {
                if( pTmp->sequence == ed.sequence  )
                {
                    if( GetDbgEcpPrintStatus())
                    {
                        PRINTF("ecp rcv ack seq %d match list_tx success\n", ed.sequence );
                    }

                    pTmp->ackReceived = TRUE;
                    
                    ecp_tx_sm(pTmp, ECP_TX_EVENT_ACK_RECV_OK);
                    return;
                }
            }

        }
        break;
    default: break;
    }
}

static void * ecpGetPortAttr(u32 port,u8 * outPortType)
{
    struct tagEvbPort *pPort = NULL;

    pPort = GetEvbPort(port);

    if(NULL == pPort)
    {
        EVB_LOG(EVB_LOG_ERROR, "ecpGetPortAttr:GetEvbPort(%u) failed.\n",port);
        return NULL;
    }

    if(NULL == pPort->ptPortAttr)
    {
        EVB_LOG(EVB_LOG_ERROR, "ecpGetPortAttr:EvbPort(%u) no attribute data.\n",port);
        return NULL;
    }
    *outPortType = pPort->ucEvbPortType;
    return pPort->ptPortAttr;
}
INT ecp_GetEvbTlvNegResult(u32 port)
{
    u8 PortType = 0;
    void * pPortAttr = NULL;
    TEvbUrpAttr* pUrpAttr = NULL;
    TEvbSbpAttr* pSbpAttr = NULL;

    pPortAttr = ecpGetPortAttr(port, &PortType);
    if(pPortAttr)
    {
        switch(PortType)
        {
        case EM_EVB_PT_URP:
            {
                pUrpAttr = (TEvbUrpAttr* )(pPortAttr);
                return pUrpAttr->tNegRes.ucStatus;
            }
            break;
        case EM_EVB_PT_SBP:
            {
                pSbpAttr = (TEvbSbpAttr* )(pPortAttr);
                return  pSbpAttr->tNegRes.ucStatus;
            }
            break;
        default:
            {				
                return EVB_TLV_STATUS_INIT;
            }
            break;
        }
    }
    return EVB_TLV_STATUS_INIT;
}

//void ecp_loop_timeout_handler(void *eloop_data, void *cookie)
void ecp_loop_timeout_handler(void)
{
    struct tagEvbPort *pPort = NULL;
    pPort = Get1stEvbPort();

    if(NULL == pPort)
    {
        return ;
    }

    for(; pPort; pPort = GetNextEvbPort(pPort))
    {
        if( pPort->pEcp)
        {
            pPort->pEcp->evbtlvStatus = ecp_GetEvbTlvNegResult(pPort->uiPortNo);
            ecp_tx_loop_timer_handler(pPort->pEcp);
            ecp_rx_loop_timer_handler(pPort->pEcp);
        }
    }
    return ;
}

void start_ecp_agent(void)
{
    if (gtECP.pfTmSet)
    {
        gtECP.pfTmSet();
    }
}

void stop_ecp_agent(void)
{
    if (gtECP.pfTmStop)
    {
        gtECP.pfTmStop();
    }    
}
void ecp_init_ports(void)
{
    if (gtECP.pfPortInit)
    {
        gtECP.pfPortInit();   
    }
}

void destory_ecp_port(struct tagEcp *pEcp)
{
    
}
void ecp_deinit_ports(void)
{
    struct tagEcp *pEcp = NULL;
    LIST_FOREACH(pEcp,&gtECP.ecpif_head, lstEntry)
    {
        LIST_REMOVE(pEcp, lstEntry);
        // 资源释放; 
        destory_ecp_port(pEcp);
    }
}

int ecp_init_modules()
{
    struct ecp_module *module;
    struct ecp_module *premod = NULL;
    int i = 0;
    u16  wMaxMod = sizeof(register_ecp_table)/sizeof(register_ecp_table[0]);

    LIST_INIT(&gtECP.ecpmod_head);
    for (i = 0; i<wMaxMod; i++) {
        module = register_ecp_table[i]();
        if (!module)
            continue;
        if (premod)
            LIST_INSERT_AFTER(premod, module, ecp);
        else
            LIST_INSERT_HEAD(&gtECP.ecpmod_head, module, ecp);
        premod = module;
    }

    LIST_INIT(&gtECP.ecpif_head);

    // ecp_start_loop_timer();

    return 0;
}

int ecp_deinit_modules()
{
    struct ecp_module *module;

    while (gtECP.ecpmod_head.lh_first != NULL) {
        module = gtECP.ecpmod_head.lh_first;
        LIST_REMOVE(gtECP.ecpmod_head.lh_first, ecp);
        module->ops->ecp_mod_unregister(module);
    }
    
    // ecp_stop_loop_timer();

    return 0;
}

int ecp_port_init(DWORD32 ifIndex)
{
    struct tagEvbPort* port = NULL;
    struct tagEcp * pEcp = NULL;

    // 1 check 
    port = GetEvbPort(ifIndex);
    if(NULL == port )
    {
        EVB_LOG(EVB_LOG_ERROR, "ecp_init_if:port (%u) is invalid.\n", ifIndex);
        return -1;
    }
    
    if(NULL != port->pEcp )
    {
        EVB_LOG(EVB_LOG_ERROR, "ecp_init_if: port (%u) ecp instance has existed.\n", ifIndex);
        return -1;
    }

    // 2 alloc memory
    pEcp = (struct  tagEcp *)GLUE_ALLOC(sizeof(struct  tagEcp)); 
    if(NULL == pEcp)
    {
        EVB_LOG(EVB_LOG_ERROR, "ecp_init_if: port (%u) alloc memory failed.",ifIndex);
        return -1;
    }
    MEMSET(pEcp, 0x0, sizeof(struct tagEcp));

    // LIST_INSERT_HEAD(&gtECP.ecpif_head, pEcp, lstEntry);
    pEcp->ackTimer = getEcpAckTimerInit(ifIndex);
    pEcp->port = port;    
    port->pEcp = pEcp;

    LIST_INIT(&port->pEcp->lstEcpULPDUrx);
    LIST_INIT(&port->pEcp->lstEcpULPDUtx);

    return 0;
}

int ecp_port_up(u32 ifIndex)
{
    return 0;
}

int ecp_port_down(u32 ifIndex)
{
    return 0;
}

int ecp_port_shutdown(DWORD32 ifIndex)
{
    struct tagEvbPort* port = NULL;
    struct tagEcpULPDU * pULDPU = NULL; 

    // 1 check 
    port = GetEvbPort(ifIndex);
    if(NULL == port )
    {
        EVB_LOG(EVB_LOG_ERROR, "ecp_port_shutdown: port (%u) is invalid.\n", ifIndex);
        return -1;
    }
    
    if(port->pEcp)
    {
        // txlist
        while (!LIST_EMPTY(&port->pEcp->lstEcpULPDUtx))
        {
            pULDPU = LIST_FIRST(&port->pEcp->lstEcpULPDUtx);
            LIST_REMOVE(pULDPU, lstEntry);
            GLUE_FREE(pULDPU);
            pULDPU = NULL;
        }
        // rxlist
        while (!LIST_EMPTY(&port->pEcp->lstEcpULPDUrx))
        {
            pULDPU = LIST_FIRST(&port->pEcp->lstEcpULPDUrx);
            LIST_REMOVE(pULDPU, lstEntry);
            GLUE_FREE(pULDPU);
            pULDPU = NULL;
        }
        LIST_INIT(&port->pEcp->lstEcpULPDUtx);
        LIST_INIT(&port->pEcp->lstEcpULPDUrx);

        GLUE_FREE(port->pEcp);
        port->pEcp = NULL;
    }

    return 0;
}

T_ECP *ecp_mod_init(void (*pfPortInit)(void),
    void (*pfTmInit)(void),
    void (*pfTmSet)(void),
    void (*pfTmStop)(void),
    int  (*pfSndPkt)(u32, u8*, u16))
{
    memset(&gtECP, 0, sizeof(T_ECP));
    gtECP.pfPortInit = pfPortInit;
    gtECP.pfTmInit   = pfTmInit;
    gtECP.pfTmSet    = pfTmSet;
    gtECP.pfTmStop   = pfTmStop;
    gtECP.pfSndPkt   = pfSndPkt;

    gtECP.pfRcvPkt   = &ecp_rx_ReceiveFrame;
    gtECP.pfTmout    = &ecp_loop_timeout_handler;

    
    ecp_init_modules();

    ecp_init_ports();

    if (gtECP.pfTmInit)
    {
        gtECP.pfTmInit();
    }

    start_ecp_agent();

    return &gtECP;
}

void ecp_mod_shutdown(void)
{
    struct tagEvbPort *pPort = NULL;

    stop_ecp_agent();
    ecp_deinit_ports();
    ecp_deinit_modules();

    memset(&gtECP, 0, sizeof(T_ECP));

    pPort = Get1stEvbPort();
    if(NULL == pPort)
    {
        return ;
    }

    for(; pPort; pPort = GetNextEvbPort(pPort))
    {
        ecp_port_shutdown(pPort->uiPortNo);
    }
}

void init_ecpULDPU(u32 port, struct tagEcpULPDU * pEcpULPDU)
{
    MEMSET(pEcpULPDU, 0, sizeof(struct tagEcpULPDU));

    pEcpULPDU->maxTries = getEcpMaxTries(port);
}

static UINT16 _EcpSeqNumInc(UINT16 num)
{
    if( num >= (ECP_SEQ_NUM_MAX-1))
    {
        num = ECP_SEQ_NUM_MIN;
    }
    else
    {
        num++;
    }
    return num;
}

// void ecp_send_

static UINT16 ecpduHeadVerOpSub(UINT16 ver, UINT16 ope, UINT16 sub )
{
    UINT16 tl = 0;
    UINT16 _op = 0;
    tl = ver;
    tl = tl << 12;
    _op = (ope << 10 );
    tl |= _op;
    tl |= sub;
    return tl;
}

// for ecp using 
UINT16 ecpduHead_HTON( UINT16 ver, UINT16 ope, UINT16 sub,UINT16 seq, BYTE* outHead )
{
    UINT16 verOpSub = 0;
    UINT16 s = 0;

    EVB_ASSERT(outHead);
    
    verOpSub= ecpduHeadVerOpSub(ver,ope,sub);
    verOpSub = EVB_HTONS(verOpSub);
    s =EVB_HTONS(seq);

    MEMCPY(outHead ,&verOpSub, sizeof(verOpSub));
    MEMCPY(outHead +sizeof(verOpSub),&s, sizeof(s));
    return 0;
}

UINT16 ecpduHead_NTOH( struct ecp_ecpdu * inoutHead)
{
    UINT16 verOpSub = 0;
    UINT16 s = 0; 
    BYTE * pTmp = (BYTE *)inoutHead;
    
    EVB_ASSERT(inoutHead);

    MEMCPY(&verOpSub,pTmp,sizeof(verOpSub));
    verOpSub = EVB_NTOHS(verOpSub);
    s =EVB_NTOHS(inoutHead->sequence);

    MEMCPY(pTmp,&verOpSub, sizeof(verOpSub));

    inoutHead->version = (verOpSub >> 12 ) & 0xf;
    inoutHead->operType = (verOpSub >> 10) & 0x3;
    inoutHead->subType  = (verOpSub )      & 0x3ff;

    MEMCPY(pTmp +sizeof(verOpSub),&s, sizeof(s));
    return 0;
}

UINT16 ecpduHeadVerOpSub_HTON(UINT16 ver, UINT16 ope, UINT16 sub )
{
    UINT16 tl = 0;
    UINT16 _op = 0;
    tl = ver;
    tl = tl << 12;
    _op = (ope << 10 )& 0x1e00;
    tl |= _op;
    tl |= sub & 0x03ff;
    tl = EVB_HTONS(tl);
    return tl;
}

int GetECPSequenceNum(DWORD32 ifIndex, UINT16 *num)
{
    struct tagEvbPort* port = NULL;

    // 1 check 
    port = GetEvbPort(ifIndex);
    if( NULL == port)
    {
        EVB_LOG(EVB_LOG_ERROR, "GetECPSequenceNum: port[%u] invalid.\n", ifIndex);
        return -1;
    }
    // 2 get tagEcp 
    // 3 get ecp seqNum
    if( NULL == port->pEcp )
    {
        EVB_LOG(EVB_LOG_ERROR, "GetECPSequenceNum: port[%u] ecp instance is NULL.\n", ifIndex);
        return -1;
    }

    port->pEcp->sSequence = _EcpSeqNumInc(port->pEcp->sSequence);

    *num = port->pEcp->sSequence;
    return 0;
}

int list_EcpULPDUtx_insert(DWORD32 ifIndex, struct ecp_ecpdu * pIn, DWORD32 len)
{
    struct tagEcpULPDU * pNewULPDU = NULL; 
    struct tagEcpULPDU * pULDPU = NULL; 

    struct tagEvbPort* port = NULL;

    // 1 check 
    if( NULL == pIn)
    {
        EVB_LOG(EVB_LOG_ERROR, "list_EcpULPDUtx_insert: port[%u] pIn is NULL.\n",ifIndex);
        return -1;
    }

    port = GetEvbPort(ifIndex);
    if( NULL == port || NULL == port->pEcp)
    {
        EVB_LOG(EVB_LOG_ERROR, "list_EcpULPDUtx_insert: port[%u] invalid.\n", ifIndex);
        return -1;
    }

    pNewULPDU =  (struct tagEcpULPDU *)GLUE_ALLOC(sizeof(struct tagEcpULPDU));
    if(NULL == pNewULPDU)
    {
        EVB_LOG(EVB_LOG_ERROR, "list_EcpULPDUtx_insert: alloc memory failed.\n");
        return -1;
    }

    init_ecpULDPU(ifIndex,pNewULPDU);
    
    // pEcpULPDU->sequence = pIn->sequence;
    pNewULPDU->ecpduLen = len;
    pNewULPDU->ecpdu = pIn;  
    pNewULPDU->port = port;          

    /* insert tx list */
    LIST_FOREACH(pULDPU,&port->pEcp->lstEcpULPDUtx,lstEntry )
    {
    }
    
    if(NULL == pULDPU)
    {
        LIST_INSERT_HEAD(&port->pEcp->lstEcpULPDUtx, pNewULPDU, lstEntry);
    }
    else
    {
        LIST_INSERT_AFTER(pULDPU,pNewULPDU,lstEntry);
    }
    
    if( GetDbgEcpPrintStatus())
    {
        PRINTF("ecp list_tx_insert - port:%u,seq %u ecpdu %p ecpdulen %u\n", ifIndex, EVB_NTOHS(pNewULPDU->ecpdu->sequence),pNewULPDU->ecpdu,pNewULPDU->ecpduLen );
    }

    return 0;
}

int list_EcpULPDUtx_remove(DWORD32 ifIndex,struct ecp_ecpdu * pIn)
{
    struct tagEcpULPDU *pULDPU = NULL;
    struct tagEvbPort* port = NULL;

    // 1 check 
    if( NULL == pIn)
    {
        EVB_LOG(EVB_LOG_ERROR, "list_EcpULPDUtx_remove: port[%u] pIn is NULL.\n",ifIndex);
        return -1;
    }

    port = GetEvbPort(ifIndex);
    if( NULL == port || NULL == port->pEcp)
    {
        EVB_LOG(EVB_LOG_ERROR, "list_EcpULPDUtx_remove: port[%u] invalid.\n", ifIndex);
        return -1;
    }

    LIST_FOREACH(pULDPU,&port->pEcp->lstEcpULPDUtx,lstEntry )
    {
        if( pULDPU->sequence == EVB_NTOHS(pIn->sequence) )
        {
            if( GetDbgEcpPrintStatus())
            {
                PRINTF("ecp list_tx_remove - port:%u,seq %u\n", ifIndex,EVB_NTOHS(pIn->sequence) );
            }

            LIST_REMOVE(pULDPU,lstEntry);

            // 资源释放; 
            if( NULL != pULDPU->ecpdu->ulpdu)
            {
                GLUE_FREE(pULDPU->ecpdu->ulpdu);
            }
            GLUE_FREE(pULDPU->ecpdu);
            GLUE_FREE(pULDPU);
            return 0;
        }
    }

    return 0;
}

int list_EcpULPDUrx_insert(DWORD32 ifIndex, BYTE * pIn, DWORD32 len)
{
    struct tagEcpULPDU * pNewEcpULPDU = NULL;
    struct ecp_ecpdu * pNewEcpdu = NULL;
    BYTE * pNewULP = NULL;
    struct ecp_ecpdu * pEcpduIn = NULL;
    struct tagEcpULPDU * pULDPU = NULL; 
    struct tagEvbPort* port = NULL;

    // 1 check 
    if( NULL == pIn)
    {
        EVB_LOG(EVB_LOG_ERROR, "list_EcpULPDUrx_insert: port[%u] buf is NULL.\n",ifIndex);
        return -1;
    }
    pEcpduIn = (struct ecp_ecpdu *)pIn;
    
    port = GetEvbPort(ifIndex);
    if( NULL == port || NULL == port->pEcp)
    {
        EVB_LOG(EVB_LOG_ERROR, "list_EcpULPDUrx_insert: port[%u] invalid.\n", ifIndex);
        return -1;
    }
    
    pNewEcpULPDU =(struct tagEcpULPDU *) GLUE_ALLOC(sizeof(struct tagEcpULPDU));
    if( NULL == pNewEcpULPDU)
    {
        EVB_LOG(EVB_LOG_ERROR, "list_EcpULPDUrx_insert:ecp_request allow memory failed port[%u].\n",ifIndex);
        return -1;
    }

    pNewEcpdu =(struct ecp_ecpdu *) GLUE_ALLOC(sizeof(struct ecp_ecpdu));
    if( NULL == pNewEcpdu)
    {
        EVB_LOG(EVB_LOG_ERROR, "list_EcpULPDUrx_insert:ecp_request allow memory failed port[%u].\n",ifIndex);
        GLUE_FREE(pNewEcpULPDU);
        return -1;
    }

    if( len > EVB_ECP_ECPDU_HEAD_LEN )
    {
        pNewULP =(BYTE *) GLUE_ALLOC(len - EVB_ECP_ECPDU_HEAD_LEN);
        if( NULL == pNewULP)
        {
            EVB_LOG(EVB_LOG_ERROR, "list_EcpULPDUrx_insert:ecp_request allow memory failed port[%u].\n",ifIndex);
            GLUE_FREE(pNewEcpULPDU);
            GLUE_FREE(pNewEcpdu);
            return -1;
        }
    }
    
    init_ecpULDPU(ifIndex,pNewEcpULPDU);
    MEMCPY(pNewEcpdu,pIn,sizeof(struct ecp_ecpdu));
    if(len >  EVB_ECP_ECPDU_HEAD_LEN)
    {
        MEMCPY(pNewULP,pIn + EVB_ECP_ECPDU_HEAD_LEN, len - EVB_ECP_ECPDU_HEAD_LEN );
    }
    pNewEcpdu->ulpdu = pNewULP; 
    pNewEcpULPDU->ecpdu = pNewEcpdu;
    pNewEcpULPDU->ecpduLen = len;

    pNewEcpULPDU->sequence = pEcpduIn->sequence;

    pNewEcpULPDU->port = port; 
    
    LIST_FOREACH(pULDPU,&port->pEcp->lstEcpULPDUrx,lstEntry )
    {
    }

    if(NULL == pULDPU)
    {
        LIST_INSERT_HEAD(&port->pEcp->lstEcpULPDUrx, pNewEcpULPDU, lstEntry);
    }
    else
    {
        LIST_INSERT_AFTER(pULDPU,pNewEcpULPDU,lstEntry);
    }

    if( GetDbgEcpPrintStatus())
    {
        PRINTF("ecp list_rx_insert success - port:%u,seq %u ecpdu %p ecpdulen %u\n", ifIndex, EVB_NTOHS(pNewEcpULPDU->ecpdu->sequence),pNewEcpULPDU->ecpdu,pNewEcpULPDU->ecpduLen );
    }

    return 0;
}

int list_EcpULPDUrx_remove(DWORD32 ifIndex,struct ecp_ecpdu * pIn)
{
    struct tagEcpULPDU *pULDPU = NULL;
    struct tagEvbPort* port = NULL;

    // 1 check 
    if( NULL == pIn)
    {
        EVB_LOG(EVB_LOG_ERROR, "list_EcpULPDUrx_remove: port[%u] pIn is NULL.\n",ifIndex);
        return -1;
    }

    port = GetEvbPort(ifIndex);
    if( NULL == port || NULL == port->pEcp)
    {
        EVB_LOG(EVB_LOG_ERROR, "list_EcpULPDUrx_remove: port[%u] invalid.\n", ifIndex);
        return -1;
    }

    LIST_FOREACH(pULDPU,&port->pEcp->lstEcpULPDUrx,lstEntry )
    {
        if( pULDPU->sequence == pIn->sequence )
        {
            if( GetDbgEcpPrintStatus())
            {
                PRINTF("ecp list_rx_remove - port:%u,seq %u\n", ifIndex,EVB_NTOHS(pIn->sequence ));
            }

            LIST_REMOVE(pULDPU,lstEntry);

            // 资源释放; 
            if( NULL != pULDPU->ecpdu->ulpdu)
            {
                GLUE_FREE(pULDPU->ecpdu->ulpdu);
            }
            GLUE_FREE(pULDPU->ecpdu);
            GLUE_FREE(pULDPU);
            return 0;
        }
    }

    return 0;
}

int vdpecp_ifIndex_is_valid(DWORD32 ifIndex)
{
    struct tagEvbPort* port = NULL;

    // 1 check 
    port = GetEvbPort(ifIndex);
    if( NULL == port)
    {
        return FALSE;
    }
    return TRUE;
}

INT ecp_rx_sm_valid_state(UINT32 state)
{
    if(((state) >= ECP_RX_IDLE) && ((state) <= ECP_RX_SEND_ACK))
    {
        return TRUE;
    }
    return FALSE;
}

INT ecp_tx_sm_valid_state(UINT32 state)
{
    if(((state) >= ECP_TX_IDLE) && ((state) <= ECP_TX_COUNT_ERR))
    {
        return TRUE;
    }
    return FALSE;
}

u32 getEcpAckTimerInit(u32 port)
{
    struct tagEvbPort *pPort = NULL;

    pPort = GetEvbPort(port);

    if(NULL == pPort)
    {
        EVB_LOG(EVB_LOG_ERROR, "getEcpAckTimerInit:GetEvbPort(%u) failed.\n",port);
        return EVBSYSCFG_DFTECPACKTIMERINIT;
    }

    if(NULL == pPort->ptPortAttr)
    {
        EVB_ASSERT(0);
        EVB_LOG(EVB_LOG_ERROR, "getEcpAckTimerInit:EvbPort(%u) no attribute data.\n",port);
        return EVBSYSCFG_DFTECPACKTIMERINIT;
    }

    switch(pPort->ucEvbPortType)
    {
    case EM_EVB_PT_URP:
        {
            if(EVB_TLV_STATUS_SUCC == ((TEvbUrpAttr* )(pPort->ptPortAttr))->tNegRes.ucStatus)
            {
                return evbcalc_2_exponent(((TEvbUrpAttr* )(pPort->ptPortAttr))->tNegRes.ucRTE);
            }
        }
        break;
    case EM_EVB_PT_SBP:
        {
            if(EVB_TLV_STATUS_SUCC == ((TEvbSbpAttr* )(pPort->ptPortAttr))->tNegRes.ucStatus)
            {
                return evbcalc_2_exponent(((TEvbSbpAttr* )(pPort->ptPortAttr))->tNegRes.ucRTE);
            }
        }
        break;
    default:
        {
            
        }
        break;
    }
    EVB_LOG(EVB_LOG_ERROR, "getEcpAckTimerInit:EvbPort(%u) attr(%p) type[%u] default value.\n",port,pPort->ptPortAttr, pPort->ucEvbPortType);
    return evbcalc_2_exponent(evbcfg_getEcpAckInit());
}

BYTE getEcpMaxTries(u32 port)
{
    struct tagEvbPort *pPort = NULL;

    pPort = GetEvbPort(port);

    if(NULL == pPort)
    {
        EVB_LOG(EVB_LOG_ERROR, "getEcpAckTimerInit:GetEvbPort(%u) failed.\n",port);
        return EVBSYSCFG_DFTECPMAXTRIES;
    }

    if(NULL == pPort->ptPortAttr)
    {
        EVB_ASSERT(0);
        EVB_LOG(EVB_LOG_ERROR, "getEcpAckTimerInit:EvbPort(%u) no attribute data.\n",port);
        return EVBSYSCFG_DFTECPMAXTRIES;
    }

    switch(pPort->ucEvbPortType)
    {
    case EM_EVB_PT_URP:
        {
            if(EVB_TLV_STATUS_SUCC == ((TEvbUrpAttr* )(pPort->ptPortAttr))->tNegRes.ucStatus)
            {
                return ((TEvbUrpAttr* )(pPort->ptPortAttr))->tNegRes.ucEcpMaxRetries;
            }
        }
        break;
    case EM_EVB_PT_SBP:
        {
            if(EVB_TLV_STATUS_SUCC == ((TEvbSbpAttr* )(pPort->ptPortAttr))->tNegRes.ucStatus)
            {
                return ((TEvbSbpAttr* )(pPort->ptPortAttr))->tNegRes.ucEcpMaxRetries;
            }
        }
        break;
    default:
        {
            
        }
        break;
    }
    EVB_LOG(EVB_LOG_ERROR, "getEcpMaxTries:EvbPort(%u) attr(%p) type[%u] default value.\n",port, pPort->ptPortAttr, pPort->ucEvbPortType);
    return evbcfg_getEcpMaxTries();
}

int32 evbtlv_chg_set_ecp(u32 ifIndex, u32 flag)
{
    struct tagEvbPort* port = NULL;
    struct tagEcpULPDU * pULPDU = NULL;
    u32  newMaxTries = 0;
    u32  newAckTimer = 0;
    u32  oldAckTimer = 0;

    port = GetEvbPort(ifIndex);
    if( NULL == port)
    {
        return 0;
    }

    // evb no support
    if(!EVB_BIT_TEST(flag,EVBTLV_ISSUPPORT))
    {
        // 是否需要做哪些事...;
    }
    /* flag */
    if( EVB_BIT_TEST(flag,EVBTLV_CHG_R) || \
        EVB_BIT_TEST(flag,EVBTLV_CHG_RTE) )
    {	
        if( NULL == port->pEcp)
        {
            return 0;
        }
        oldAckTimer = port->pEcp->ackTimer ;
        newMaxTries = getEcpMaxTries(ifIndex);
        newAckTimer = getEcpAckTimerInit(ifIndex);
        port->pEcp->ackTimer = newAckTimer;

        // tx list	
        LIST_FOREACH(pULPDU,&port->pEcp->lstEcpULPDUtx,lstEntry )
        {
            pULPDU->maxTries = newMaxTries;

            if( (pULPDU->ackTimer + newAckTimer ) >= oldAckTimer )
            {
                pULPDU->ackTimer = (pULPDU->ackTimer + newAckTimer - oldAckTimer );
            }
            else
            {
                pULPDU->ackTimer = 0;
            }
        }
        // rx list
        LIST_FOREACH(pULPDU,&port->pEcp->lstEcpULPDUrx,lstEntry )
        {
            pULPDU->maxTries = newMaxTries;

            if( (pULPDU->ackTimer + newAckTimer ) >= oldAckTimer )
            {
                pULPDU->ackTimer = (pULPDU->ackTimer + newAckTimer - oldAckTimer );
            }
            else
            {
                pULPDU->ackTimer = 0;
            }
        }

        EVB_LOG(EVB_LOG_INFO, "evbtlv_chg_set_ecp(port:%u, evbtlv support:%u,R:%u,RTE:%u ): success.\n",
             ifIndex,EVB_BIT_TEST(flag,EVBTLV_ISSUPPORT),EVB_BIT_TEST(flag,EVBTLV_CHG_R),EVB_BIT_TEST(flag,EVBTLV_CHG_RTE));
    }

    return 0;
}

int32 GetEcpVdp_evbtlvStatus(u32 port)
{
    struct tagEvbPort *pPort = NULL;

    pPort = GetEvbPort(port);
    if(NULL == pPort)
    {
        EVB_LOG(EVB_LOG_INFO, "GetEcpVdp_evbtlvStatus:GetEvbPort(%u) failed.\n",port);
        return EVB_TLV_STATUS_INIT;
    }

    if(NULL == pPort->ptPortAttr)
    {
        EVB_LOG(EVB_LOG_INFO, "vdpGetPortAttr:EvbPort(%u) no attribute data.\n",port);
        return EVB_TLV_STATUS_INIT;
    }

    switch(pPort->ucEvbPortType)
    {
    case EM_EVB_PT_URP:
        {
            return ((TEvbUrpAttr* )(pPort->ptPortAttr))->tNegRes.ucStatus;
        }
        break;
    case EM_EVB_PT_SBP:
        {
            return ((TEvbSbpAttr* )(pPort->ptPortAttr))->tNegRes.ucStatus;
        }
        break;
    default:
        {
            EVB_LOG(EVB_LOG_INFO, "vdpGetPortAttr:EvbPort(%u) Type (%u) is isnot URP or SBP.\n",port,pPort->ucEvbPortType);
            return EVB_TLV_STATUS_INIT;
        }
        break;
    }
    
    return EVB_TLV_STATUS_INIT;
}


#ifdef __cplusplus
}
#endif 
