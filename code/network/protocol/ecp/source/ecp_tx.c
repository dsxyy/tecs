#ifdef __cplusplus
extern "C"{
#endif 
#include "evb_comm.h"
#include "ecp_in.h"
#include "ecp.h"
#include "evb_port_mgr.h"

const char *ecp_trace_tx_sm_status[] =
{
    "idle",
    "initTransmit",
    "transmitECDPU",
    "waitForRequest",
    "countErros",
};

const char *ecp_trace_tx_sm_events[] =
{
    "begin",
    "req_recved",
    "retransmit",
    "not_retransmit",
    "ack_recv_ok",
    "uct",
    "port_up",
    "port_down",
};

extern T_ECP gtECP;

extern void DbgMemShow(void *pMem, WORD32 dwSz);
//void ecp_tx_sm(struct tagEcpULPDU * ecp, ECP_TX_SM_EVENT event);
void ecp_tx_sm(struct tagEcpULPDU * ecp, ECP_TX_SM_EVENT event);

static void Tx_err(struct tagEcpULPDU * ecp)
{
}

/*
static void port_dis(struct tagEcpULPDU * ecp)
{
    ecp->portEnabled = FALSE;
    ecp->tx_state = ECP_TX_INIT_TRANSMIT;   
}
*/
static void port_en(struct tagEcpULPDU * ecp)
{
    ecp->portEnabled = TRUE;
}

static void init_tr(struct tagEcpULPDU * ecp)
{
    ecp->tx_state = ECP_TX_INIT_TRANSMIT;   

    ecp->retries = 0;
    ecp->requestReceived = FALSE;
    ecp->ackReceived = FALSE;
    ecp->txErrors = 0;

    // stats 
}

static int inc(int num)
{
    return ++num;
}

static int ackTimerInit(u32 port)
{
    return getEcpAckTimerInit(port);
}

static int transmitECPDU(struct tagEcpULPDU * ecp)
{
    /*发送报文 */
    struct ecp_ethhdr l2_head;
    int len = 0;
    u8 *p = NULL;

    if ( NULL==ecp || NULL== ecp->ecpdu || NULL==gtECP.pfSndPkt)
    {
        EVB_LOG(EVB_LOG_ERROR, "transmitECPDU:Point is NULL.\n");
        return -1;
    }
    
    MEMSET(&l2_head, 0, sizeof(l2_head));

    len = ecp->ecpduLen;
    if( len < EVB_ECP_ECPDU_HEAD_LEN)
    {
        EVB_LOG(EVB_LOG_ERROR, "transmitECPDU: ecp->ecpduLen(%u) is invalid.\n",len);
        return -1;
    }

    l2_head.h_proto = EVB_HTONS(ECP_PROTOCOL_TYPE);
    memcpy(l2_head.h_dest, ECP_MULTICAST_MAC, EVB_ECP_MAC_LEN);
    memcpy(l2_head.h_source, ecp->PORT_MAC, EVB_ECP_MAC_LEN);
    // l2_head.h_souce;
    
    // 最小报文长度;
    if( len < ECP_MIN_DATA_LEN )
    {
        len = ECP_MIN_DATA_LEN;
    }

    if (NULL==(p=(u8*)GLUE_ALLOC(len + EVB_ECP_ETHHEAD_LEN))) // 为啥要申请18,14就够了；
    {
        EVB_LOG(EVB_LOG_ERROR, "transmitECPDU:GLUE_ALLOC failed.\n");
        return -1;
    }
    MEMSET(p,0,len + EVB_ECP_ETHHEAD_LEN);

    memcpy(p,  l2_head.h_dest, EVB_ECP_MAC_LEN);
    memcpy(p + EVB_ECP_MAC_LEN, l2_head.h_source, EVB_ECP_MAC_LEN);
    memcpy(p + 2*EVB_ECP_MAC_LEN, &l2_head.h_proto, 2);
    // memcpy(p + 14,(u8*)ecp->ecpdu, len);
    memcpy(p + EVB_ECP_ETHHEAD_LEN,(u8*)ecp->ecpdu, EVB_ECP_ECPDU_HEAD_LEN);
    if( len > EVB_ECP_ECPDU_HEAD_LEN )
    {
        //memcpy(p + EVB_ECP_ETHHEAD_LEN + EVB_ECP_ECPDU_HEAD_LEN ,(u8*)ecp->ecpdu->ulpdu, len-EVB_ECP_ECPDU_HEAD_LEN);
        memcpy(p + EVB_ECP_ETHHEAD_LEN + EVB_ECP_ECPDU_HEAD_LEN ,(u8*)ecp->ecpdu->ulpdu, ecp->ecpduLen-EVB_ECP_ECPDU_HEAD_LEN);
    }
    len = len + EVB_ECP_ETHHEAD_LEN; // 后面将此14修改成 ECP_ETHHEAD_LEN 
    
    if (0!=gtECP.pfSndPkt(ecp->PORT_NUM, (u8*)p, (u16)len))
    {
        if( GetDbgEcpPrintStatus())
        {
            PRINTF("ecp transmitECPDU: gtECP.pfSndPkt failed.- seq %d \n", EVB_NTOHS(ecp->ecpdu->sequence) );
        }

        EVB_LOG(EVB_LOG_ERROR, "ecp transmitECPDU: gtECP.pfSndPkt failed.\n");
        GLUE_FREE(p);
        return -1;
    }

    if( GetDbgEcpPrintStatus())
    {
        PRINTF("ecp transmitECPDU: gtECP.pfSndPkt  success- seq %d \n", EVB_NTOHS(ecp->ecpdu->sequence ));
        //PRINTF("DbgMemShow(%p %u\n)",p+EVB_ECP_ETHHEAD_LEN, len-EVB_ECP_ETHHEAD_LEN);
        //DbgMemShow(p+EVB_ECP_ETHHEAD_LEN,len-EVB_ECP_ETHHEAD_LEN);
        PRINTF("DbgMemShow(%p %u\n)",p, len);
        DbgMemShow(p,len);
    }

    ecp_packet_print(p, len);

    if(GetDbgEcpPacket())
    {
        PRINTF("ECP Send  port:%u,",ecp->PORT_NUM);
        PrintLocalTimer();
        DbgMemShow(p,len);
    }

    GLUE_FREE(p);    

    return 0;
}

static void tr_ecpdu(struct tagEcpULPDU * ecp)
{
    ecp->tx_state = ECP_TX_TRANSMIT_ECPDU;

    ecp->requestReceived = TRUE;

    /*  填写 ecp 所在端口编号;  */
    transmitECPDU(ecp); 
    ecp->ackTimer = ackTimerInit(ecp->PORT_NUM);
    ecp->retries = inc(ecp->retries); 
}

static void cnt_err(struct tagEcpULPDU * ecp)
{
    ecp->tx_state = ECP_TX_COUNT_ERR;

    ecp->txErrors = inc(ecp->txErrors); 

    ecp_tx_sm(ecp, ECP_TX_EVENT_UCT);

    // 是否需要通知vdp; 
}

static void wt_for_req(struct tagEcpULPDU * ecp)
{
    ecp->tx_state = ECP_TX_WAIT_FOR_REQ;

    // ecp->sequence = inc(ecp->sequence);  
    ecp->retries = 0;
    ecp->requestReceived = FALSE;
    ecp->ackReceived = FALSE;
}

void (* ecp_tx_actions[NUM_ECP_TX_EVNETS][NUM_ECP_TX_SM_STATES]) (struct tagEcpULPDU * ecp) =
{
    /*
    * event/state         IDLE  INIT_TRANSMIT TRANSMIT_ECPDU WAIT_FOR_REQ TX_COUNT_ERR
    */
    /* BEGIN  */        { init_tr, Tx_err,    Tx_err,        Tx_err,      Tx_err },
    /* REQ_RECV  */     { Tx_err,  tr_ecpdu,  Tx_err,        tr_ecpdu,    Tx_err },
    /* RETRANSMIT  */   { Tx_err,  Tx_err,    tr_ecpdu,      Tx_err,      Tx_err },
    /* NOT_RETRANSMIT*/ { Tx_err,  Tx_err,    cnt_err,       Tx_err,      Tx_err },
    /* ACK_RECV_OK */   { Tx_err,  Tx_err,    wt_for_req,    Tx_err,      Tx_err },
    /* UCT */           { Tx_err,  Tx_err,    Tx_err,        Tx_err,      wt_for_req },
    /* PORT_UP */       { port_en, Tx_err,    Tx_err,        Tx_err,      Tx_err },
    /* PORT_DOWN*/      { init_tr, init_tr,   init_tr,       init_tr,     init_tr },
    
};

void ecp_tx_sm(struct tagEcpULPDU * ecp, ECP_TX_SM_EVENT event)
{
    struct tagEvbPort* port = NULL;

    if( ( NULL == ecp) || !ecp_tx_sm_valid_event(event) )
    {
        EVB_LOG(EVB_LOG_ERROR, "ecp_tx_sm: param[%p %d] invalid.\n",  ecp,event);
        return;
    }

    if( !ecp_tx_sm_valid_state(ecp->tx_state))
    {
        EVB_LOG(EVB_LOG_ERROR, "ecp_tx_sm: tx_state[%d] invalid.\n",  ecp->tx_state);
        return;
    }

    // 1 ifindex is valid 
    port = GetEvbPort(ecp->PORT_NUM); 
    if( NULL == port)
    {
        EVB_LOG(EVB_LOG_ERROR, "ecp_rx_sm: port[%u] invalid.\n",  ecp->PORT_NUM);
        return;
    }

    if( GetDbgEcpPrintStatus())
    {
        PRINTF("ecp_tx_sm - port:%u,[%s] [%s]\n", ecp->PORT_NUM, ecp_trace_tx_sm_status[ecp->tx_state],ecp_trace_tx_sm_events[event] );
    }
    (*(ecp_tx_actions[event][ecp->tx_state])) (ecp);

    if( GetDbgEcpPrintStatus())
    {
        if( !ecp_tx_sm_valid_state(ecp->tx_state))
        {
            EVB_LOG(EVB_LOG_ERROR, "ecp_tx_sm: port(%u) tx_state[%d] invalid.\n",ecp->PORT_NUM,  ecp->tx_state);
            return;
        }
        else
        {
            PRINTF("ecp_tx_sm: port(%u) current tx_state - [%s] \n",ecp->PORT_NUM, ecp_trace_tx_sm_status[ecp->tx_state] );
        }
    }

    return;
}

void ecp_tx_loop_timer_handler(struct tagEcp * pEcp)
{
    if( NULL == pEcp )
    {
        EVB_LOG(EVB_LOG_ERROR, "ecp_tx_loop_timer_handler: param is NULL.\n");
        return;
    }

    if( NULL == pEcp->port )
    {
        EVB_LOG(EVB_LOG_ERROR, "ecp_tx_loop_timer_handler: port is NULL.\n");
        return;
    }

    // evbtlv no support 
    if(pEcp->evbtlvStatus != EVB_TLV_STATUS_SUCC)
    {
        struct tagEcpULPDU *pTemp = NULL;
        while (!LIST_EMPTY(&pEcp->lstEcpULPDUtx))
        {
            pTemp = LIST_FIRST(&pEcp->lstEcpULPDUtx);
            LIST_REMOVE(pTemp, lstEntry);
            GLUE_FREE(pTemp);
            pTemp = NULL;
        }
        LIST_INIT(&pEcp->lstEcpULPDUtx);

        return;
    }

    // 缓存队列在接口上;
    if( LIST_EMPTY (&pEcp->lstEcpULPDUtx))
    {
    }
    else
    {
        struct tagEcpULPDU *pULDPU = NULL;
        // LIST_FOREACH(pULDPU,&pEcp->port->lstEcpULPDUtx,lstEntry )
        pULDPU = LIST_FIRST(&pEcp->lstEcpULPDUtx);
        {
            // seq = 0, is new 
            if(0 == pULDPU->sequence)
            {
                ecp_tx_sm(pULDPU,ECP_TX_EVENT_BEGIN);
                pULDPU->sequence = EVB_NTOHS( pULDPU->ecpdu->sequence);
                ecp_tx_sm(pULDPU,ECP_TX_EVENT_REQ_RECV);
            }
            else
            {
                // remove it
                if( pULDPU->tx_state == ECP_TX_WAIT_FOR_REQ)
                {
                    list_EcpULPDUtx_remove(pULDPU->port->uiPortNo,pULDPU->ecpdu);
                    return;
                }

                /* ack timeout 在此实现; */
                if(pULDPU->ackTimer > ECP_LOOP_TIMER_INTERVAL )
                {
                    pULDPU->ackTimer -= ECP_LOOP_TIMER_INTERVAL;
                }
                else
                {
                    pULDPU->ackTimer = 0;
                }

                if( 0 == pULDPU->ackTimer)
                {
                    if( pULDPU->retries < pULDPU->maxTries )
                    {
                        ecp_tx_sm(pULDPU,ECP_TX_EVENT_RETRANSMIT);
                    }
                    else
                    {
                        ecp_tx_sm(pULDPU,ECP_TX_EVENT_NOT_RETRANSMIT);
                    }
                }
            }           
        }
    }
}

#ifdef __cplusplus
}
#endif 

