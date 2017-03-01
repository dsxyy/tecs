
#ifdef __cplusplus
extern "C"{
#endif 
#include "evb_comm.h"
#include "ecp_in.h"
#include "ecp.h"
#include "vdp.h"
#include "evb_port_mgr.h"

const char *ecp_trace_rx_sm_status[] =
{
    "idle",
    "receiveWait",
    "receiveFirst",
    "receiveECPDU",
    "newECPDU",
    "sendACK",
};

const char *ecp_trace_rx_sm_events[] =
{
    "begin",
    "ecpdu_recved",
    "seq_equ_last",
    "seq_notequ_last",
    "uct",
    "port_up",
    "port_down",
};

extern struct tagEcpmod_head ecpmod_head;

extern void DbgMemShow(void *pMem, WORD32 dwSz);
void ecp_rx_sm(struct tagEcpULPDU * ecpU, ECP_RX_SM_EVENT event);

static void Rx_err(struct tagEcpULPDU * ecpU)
{
}

static void rcv_wait(struct tagEcpULPDU * ecpU)
{
    ecpU->rx_state = ECP_RX_RECEVIE_WAIT;
    ecpU->ecpduReceived = FALSE;

    ecp_rx_sm(ecpU, ECP_RX_EVENT_ECPDU_RECV);
}

static DWORD32 seqNum(struct ecp_ecpdu * ecpdu)
{
    return ecpdu->sequence;
}
static void rcv_first(struct tagEcpULPDU * ecpU)
{
    ecpU->rx_state = ECP_RX_RECEIVE_FIRST;
    ecpU->lastSeq = seqNum(ecpU->ecpdu)-1;

    ecp_rx_sm(ecpU, ECP_RX_EVENT_UCT);
}

static void rcv_ECPDU(struct tagEcpULPDU * ecpU)
{
    ecpU->rx_state = ECP_RX_RECEIVE_ECPDU;
    // 待确认; 
    ecpU->seqECPDU = seqNum(ecpU->ecpdu);

    if(ecpU->seqECPDU == ecpU->lastSeq )
    {
        ecp_rx_sm(ecpU,ECP_RX_EVENT_SEQ_EQU_LAST);
    }
    else
    {
        ecp_rx_sm( ecpU,ECP_RX_EVENT_SEQ_NOTEQU_LAST);
    }
}

// 从vdp组包; 
int invokeindication(struct ecp_ecpdu * in_ecp,struct ecp_ecpdu * out_ecpdu, DWORD32 * len )
{
#if 0
    struct ecp_module  *pMod = NULL;
    LIST_FOREACH(pMod,&gtECP.ecpmod_head, ecp)
    {
        pMod->ops->ecp_mod_ecpdu(1, in_ecp,out_ecpdu,len );
    }
#endif 
    MEMCPY(out_ecpdu, in_ecp, sizeof(struct ecp_ecpdu));
    //out_ecpdu->ulpdu = NULL;
    out_ecpdu->ulpdu = NULL;

    return 0;
}

int sendAck(struct tagEcpULPDU * ecpU,struct ecp_ecpdu * out_ecpdu, DWORD32 * outlen)
{
    /*发送报文 */
    struct ecp_ethhdr l2_head;
    int len = 0;

    u8 *p = NULL;

    if ( NULL== out_ecpdu || NULL==gtECP.pfSndPkt)
    {
        EVB_LOG(EVB_LOG_ERROR, "sendAck:Point is NULL.\n");
        return -1;
    }

    MEMSET(&l2_head, 0, sizeof(l2_head));

    len = EVB_ECP_ECPDU_HEAD_LEN;

    l2_head.h_proto = EVB_HTONS(ECP_PROTOCOL_TYPE);
    memcpy(l2_head.h_dest, ECP_MULTICAST_MAC, EVB_ECP_MAC_LEN);
    memcpy(l2_head.h_source, ecpU->PORT_MAC, EVB_ECP_MAC_LEN);
    // l2_head.h_souce;

    // 最小报文长度;
    if( len < ECP_MIN_DATA_LEN )
    {
        len = ECP_MIN_DATA_LEN;
    }

    if (NULL==(p=(u8*)GLUE_ALLOC(len + EVB_ECP_ETHHEAD_LEN)))// 为啥要申请18,14就够了；
    {
        EVB_LOG(EVB_LOG_ERROR, "sendAck:GLUE_ALLOC failed.\n");
        return -1;
    }
    MEMSET(p,0,len + EVB_ECP_ETHHEAD_LEN);

    memcpy(p,  l2_head.h_dest, EVB_ECP_MAC_LEN);
    memcpy(p + EVB_ECP_MAC_LEN, l2_head.h_source, EVB_ECP_MAC_LEN);
    memcpy(p + 2*EVB_ECP_MAC_LEN, &l2_head.h_proto, 2);
    //memcpy(p + EVB_ECP_ETHHEAD_LEN,(u8*)out_ecpdu, len);
    memcpy(p + EVB_ECP_ETHHEAD_LEN,(u8*)out_ecpdu, EVB_ECP_ECPDU_HEAD_LEN);
    
    len = len + EVB_ECP_ETHHEAD_LEN;

    if (0!=gtECP.pfSndPkt(ecpU->PORT_NUM, (u8*)p, (u16)len))
    {
        EVB_LOG(EVB_LOG_ERROR, "sendAck: gtECP.pfSndPkt failed.\n");
        GLUE_FREE(p);
        return -1;
    }

    if(GetDbgEcpPacket())
    {
        PRINTF("port:%u, ECP Send  ",ecpU->PORT_NUM);
        PrintLocalTimer();
        DbgMemShow(p,len);
    }

    GLUE_FREE(p);    

    return 0;
}

static void new_ECPDU(struct tagEcpULPDU * ecpU)
{
    ecpU->rx_state = ECP_RX_NEW_ECPDU;
    ecpU->lastSeq = ecpU->seqECPDU;

    ecp_rx_sm(ecpU,ECP_RX_EVENT_UCT);   
}

static void s_Ack(struct tagEcpULPDU * ecpU)
{
    struct ecp_ecpdu _out_ecpdu;
    DWORD32 len = 0;
    int ret = 0;

    MEMSET(&_out_ecpdu, 0x0, sizeof(_out_ecpdu));

    ecpU->rx_state = ECP_RX_SEND_ACK;
    ret = invokeindication(ecpU->ecpdu,&_out_ecpdu,&len);
    if( ret != 0 )
    {
        EVB_LOG(EVB_LOG_ERROR, "s_Ack: invokeindication failed.\n");
        return;
    }
    if( 0 != sendAck(ecpU,&_out_ecpdu,&len) )
    {
        EVB_LOG(EVB_LOG_ERROR, "s_Ack: sendAck failed.\n");
        return;
    }

    ecpU->ecpduReceived = FALSE;

    if( GetDbgEcpPrintStatus ())
    {
        PRINTF("s_Ack success.\n");
    }
}

void (*ecp_rx_actions[NUM_ECP_RX_EVNETS][NUM_ECP_RX_SM_STATES]) (struct tagEcpULPDU * ecpU) =
{
    /*
    * event/state         IDLE     RECV_WAIT    RECV_FIRST    RECV_ECPDU   NEW_ECPDU   SEND_ACK
    */
    /* BEGIN  */        { rcv_wait,Rx_err,      Rx_err,      Rx_err,      Rx_err,    Rx_err },
    /* ECPDU_RECV  */   { Rx_err,  rcv_first,   Rx_err,      Rx_err,      Rx_err,    rcv_ECPDU },
    /* SEQ_EQU_LAST  */ { Rx_err,  Rx_err,      Rx_err,      s_Ack,       Rx_err,    Rx_err },
    /* SEQ_NOTEQU_LAST*/{ Rx_err,  Rx_err,      Rx_err,      new_ECPDU,   Rx_err,    Rx_err },
    /* UCT */           { Rx_err,  Rx_err,      rcv_ECPDU,   Rx_err,      s_Ack,     Rx_err },
    /* PORT_UP */       { Rx_err,  Rx_err,      Rx_err,      Rx_err,      Rx_err,    Rx_err },
    /* PORT_DOWN*/      { rcv_wait,rcv_wait,    rcv_wait,    rcv_wait,    rcv_wait,  rcv_wait },
    
};

// 后续修改成和draft协议一致  先调通 
void ecp_rx_sm(struct tagEcpULPDU * ecpU, ECP_RX_SM_EVENT event)
{
    struct tagEvbPort* port = NULL;
    if( ( NULL == ecpU) || !ecp_rx_sm_valid_event(event) )
    {
        EVB_LOG(EVB_LOG_ERROR, "ecp_rx_sm: param[%p %d] invalid.\n",  ecpU,event);
        return;
    }

    if( !ecp_rx_sm_valid_state(ecpU->rx_state))
    {
        EVB_LOG(EVB_LOG_ERROR, "ecp_rx_sm: rx_state[%d] invalid.\n",  ecpU->rx_state);
        return;
    }

    // 1 check 
    port = GetEvbPort(ecpU->PORT_NUM);
    if( NULL == port)
    {
        EVB_LOG(EVB_LOG_ERROR, "ecp_rx_sm: port[%u] invalid.\n",  ecpU->PORT_NUM);
        return;
    }
    
    if( GetDbgEcpEvent())
    {
        PRINTF("ecp_rx_sm - port:%u,[%s] [%s]\n", ecpU->PORT_NUM, ecp_trace_rx_sm_status[ecpU->rx_state],ecp_trace_rx_sm_events[event] );
    }
    (*(ecp_rx_actions[event][ecpU->rx_state])) (ecpU);

    if( GetDbgEcpEvent())
    {
        if( !ecp_rx_sm_valid_state(ecpU->rx_state))
        {
            PRINTF("ecp_rx_sm: port:%u, rx_state[%d] invalid.\n", ecpU->PORT_NUM, ecpU->rx_state);
            return;
        }
        else
        {
            PRINTF("ecp_rx_sm: port:%u,current rx_state - [%s] \n", ecpU->PORT_NUM,ecp_trace_rx_sm_status[ecpU->rx_state] );
        }
    }
}

void ecp_rx_loop_timer_handler(struct tagEcp * pEcp)
{
    struct ecp_ecpdu ecpdu;
    MEMSET(&ecpdu,0,sizeof(struct ecp_ecpdu ));

    if( NULL == pEcp )
    {
        EVB_LOG(EVB_LOG_ERROR, "ecp_rx_loop_timer_handler: param[%p] invalid.\n",  pEcp);
        return;
    }
    
    if( NULL == pEcp->port )
    {
        EVB_LOG(EVB_LOG_ERROR, "ecp_rx_loop_timer_handler: port[%p] invalid.\n",  pEcp->port);
        return;
    }
    
    // evbtlv no support 
    if(pEcp->evbtlvStatus != EVB_TLV_STATUS_SUCC)
    {
        struct tagEcpULPDU *pTemp = NULL;
        while (!LIST_EMPTY(&pEcp->lstEcpULPDUrx))
        {
            pTemp = LIST_FIRST(&pEcp->lstEcpULPDUrx);
            LIST_REMOVE(pTemp, lstEntry);
            GLUE_FREE(pTemp);
            pTemp = NULL;
        }
        LIST_INIT(&pEcp->lstEcpULPDUrx);

        return;
    }

    // 缓存队列在接口上;
    if( LIST_EMPTY (&pEcp->lstEcpULPDUrx))
    {
    }
    else
    {
        struct tagEcpULPDU *pULDPU = NULL;
        // LIST_FOREACH(pULDPU,&pEcp->port->lstEcpULPDUrx,lstEntry )
        pULDPU = LIST_FIRST(&pEcp->lstEcpULPDUrx);
        {
            if(NULL == pULDPU->ecpdu)
            {
                LIST_REMOVE(pULDPU,lstEntry);

                // 资源释放; 
                GLUE_FREE(pULDPU);
                return;
            }
                                    
            if( GetDbgEcpPrintStatus())
            {
                PRINTF("\n Ecp list_rx ecpdu: seq %u ",EVB_NTOHS(pULDPU->ecpdu->sequence) );
                PRINTF("\n ulp_len %u ulp ",pULDPU->ecpduLen -EVB_ECP_ECPDU_HEAD_LEN);
                DbgMemShow(pULDPU->ecpdu, EVB_ECP_ECPDU_HEAD_LEN);
            
                if( pULDPU->ecpdu->ulpdu)
                {
                    if( pULDPU->ecpduLen > EVB_ECP_ECPDU_HEAD_LEN)
                    {
                        DbgMemShow(pULDPU->ecpdu->ulpdu, pULDPU->ecpduLen -EVB_ECP_ECPDU_HEAD_LEN );
                    }
                }
                PRINTF("\n");
            }

            // 以后修改成 LIST_FOREACH gtECP.ecpmod_head
            MEMCPY(&ecpdu,pULDPU->ecpdu,sizeof(struct ecp_ecpdu ));
            ecpduHead_NTOH(&ecpdu);
            
            // Vdp 模块处理;
            if(ecpdu.subType == ECP_SUBTYPE_VDP)
            {
#if 1
                if( pULDPU->ecpdu->ulpdu)
                {
                    if( pULDPU->ecpduLen > EVB_ECP_ECPDU_HEAD_LEN)
                    {
                        vdp_rx_ReceiveFrame(pEcp->PORT_NUM, pULDPU->ecpdu->ulpdu, pULDPU->ecpduLen -EVB_ECP_ECPDU_HEAD_LEN);
                    }
                    else
                    {
                        if( GetDbgEcpPrintStatus())
                        {
                            PRINTF("ecp_rx_loop_timer_handler: pULDPU->ecpduLen (%u) is invalid.");
                        }
                    }
                }
#endif          
            }
            // 删除缓存; 
            list_EcpULPDUrx_remove(pEcp->PORT_NUM,pULDPU->ecpdu);
        }
    }
}

#ifdef __cplusplus
}
#endif 
