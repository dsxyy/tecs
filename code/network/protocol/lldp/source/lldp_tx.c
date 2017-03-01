/*******************************************************************************

  LLDP Agent Daemon (LLDPAD) Software
  Copyright(c) 2007-2010 Intel Corporation.

  This program is free software; you can redistribute it and/or modify it
  under the terms and conditions of the GNU General Public License,
  version 2, as published by the Free Software Foundation.

  This program is distributed in the hope it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

  The full GNU General Public License is included in this distribution in
  the file called "COPYING".

  Contact Information:
  open-lldp Mailing List <lldp-devel@open-lldp.org>

*******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/lldp_in.h"

bool mibConstrInfoLLDPDU(struct lldp_port *port)
{
    struct l2_ethhdr eth;
    u8  own_addr[ETH_ALEN];
    u32 fb_offset = 0;
    u32 datasize = 0;
    struct packed_tlv *ptlv =  NULL;
    struct lldp_module *np;

    if (NULL==port)
    {
        return false;
    }

    if (port->tx.frameout)
    {
        GLUE_FREE(port->tx.frameout);
        port->tx.frameout = NULL;
    }

    memcpy(eth.h_dest, port->dmac, ETH_ALEN);
    l2_packet_get_own_src_addr(port->l2,(u8 *)&own_addr);
    memcpy(eth.h_source, &own_addr, ETH_ALEN);
    eth.h_proto = EVB_HTONS(ETH_P_LLDP);
    port->tx.frameout =  (u8 *)GLUE_ALLOC(ETH_FRAME_LEN);
    if (port->tx.frameout == NULL) {
        LLDPAD_DBG("InfoLLDPDU: Failed to GLUE_ALLOC frame buffer \n");
        return false;
    }
    memset(port->tx.frameout,0,ETH_FRAME_LEN);
    memcpy(port->tx.frameout, (void *)&eth, sizeof(struct l2_ethhdr));
    fb_offset += sizeof(struct l2_ethhdr);

    /* Generic TLV Pack */
    LIST_FOREACH(np, &gtLLDP.mod_head, lldp) {
        if (!np->ops || !np->ops->lldp_mod_gettlv)
            continue;

        ptlv = np->ops->lldp_mod_gettlv(port);
        if (ptlv) {
            if ((ptlv->size+fb_offset) > ETH_MAX_DATA_LEN)
                goto error;
            memcpy(port->tx.frameout+fb_offset,
                   ptlv->tlv, ptlv->size);
            datasize += ptlv->size;
            fb_offset += ptlv->size;
            ptlv =  free_pkd_tlv(ptlv);
        }
    }

    /* The End TLV marks the end of the LLDP PDU */
    ptlv = pack_end_tlv();
    if (!ptlv || ((ptlv->size + fb_offset) > ETH_MAX_DATA_LEN))
        goto error;
    memcpy(port->tx.frameout + fb_offset, ptlv->tlv, ptlv->size);
    datasize += ptlv->size;
    fb_offset += ptlv->size;
    ptlv =  free_pkd_tlv(ptlv);

    if (datasize < ETH_MIN_DATA_LEN)
        port->tx.sizeout = ETH_MIN_PKT_LEN;
    else
        port->tx.sizeout = fb_offset;

    return true;

error:
    ptlv = free_pkd_tlv(ptlv);
    if (port->tx.frameout)
        GLUE_FREE(port->tx.frameout);
    port->tx.frameout = NULL;
    LLDPAD_DBG("InfoLLDPDU: packed TLV too large for tx frame\n");
    return false;
}

void txInitializeLLDP(struct lldp_port *port)
{
    if (port->tx.frameout) {
        GLUE_FREE(port->tx.frameout);
        port->tx.frameout = NULL;
    }
    port->tx.localChange = 0;
    port->timers.reinitDelay   = REINIT_DELAY;
    port->timers.msgTxHold     = DEFAULT_TX_HOLD;
    port->timers.msgTxInterval = DEFAULT_TX_INTERVAL;
    port->timers.msgFastTx     = FAST_TX_INTERVAL;
    return;
}

void txInitializeTimers(struct lldp_port *port)
{
    port->timers.txTick = false;
    port->tx.txNow = false;
    port->tx.localChange = 0;
    port->timers.txTTR = 0;
    port->tx.txFast = 0;
    port->timers.txShutdownWhile = 0;
    port->rx.newNeighbor = false;
    port->timers.txMaxCredit = TX_CREDIT_MAX;
    port->timers.txCredit = TX_CREDIT_MAX;
    port->timers.txFastInit = TX_FAST_INIT;
    return;
}

bool mibConstrShutdownLLDPDU(struct lldp_port *port)
{
    struct l2_ethhdr eth;
    u8  own_addr[ETH_ALEN];
    u32 fb_offset = 0;
    u32 datasize = 0;
    struct packed_tlv *ptlv =  NULL;
    struct lldp_module *np;

    if (NULL==port)
    {
        return false;
    }

    if (port->tx.frameout)
    {
        GLUE_FREE(port->tx.frameout);
        port->tx.frameout = NULL;
    }

    memcpy(eth.h_dest, port->dmac, ETH_ALEN);
    l2_packet_get_own_src_addr(port->l2,(u8 *)&own_addr);
    memcpy(eth.h_source, &own_addr, ETH_ALEN);
    eth.h_proto = EVB_HTONS(ETH_P_LLDP);
    port->tx.frameout =  (u8 *)GLUE_ALLOC(ETH_FRAME_LEN);
    if (port->tx.frameout == NULL) {
        LLDPAD_DBG("ShutdownLLDPDU: Failed to GLUE_ALLOC frame buffer \n");
        return false;
    }
    memset(port->tx.frameout,0,ETH_FRAME_LEN);
    memcpy(port->tx.frameout, (void *)&eth, sizeof(struct l2_ethhdr));
    fb_offset += sizeof(struct l2_ethhdr);

    np = find_module_by_id(&gtLLDP.mod_head, LLDP_MOD_MAND);
    if (!np)
        goto error;
    if (!np->ops || !np->ops->lldp_mod_gettlv)
        goto error;
    ptlv = np->ops->lldp_mod_gettlv(port);
    if (ptlv) {
        if ((ptlv->size + fb_offset) > ETH_MAX_DATA_LEN)
            goto error;
        /* set the TTL to be 0 TTL TLV */
        memset(&ptlv->tlv[ptlv->size - 2], 0, 2);
        memcpy(port->tx.frameout + fb_offset, ptlv->tlv, ptlv->size);
        datasize += ptlv->size;
        fb_offset += ptlv->size;
        ptlv =  free_pkd_tlv(ptlv);
    }

    /* The End TLV marks the end of the LLDP PDU */
    ptlv = pack_end_tlv();
    if (!ptlv || ((ptlv->size + fb_offset) > ETH_MAX_DATA_LEN))
        goto error;
    memcpy(port->tx.frameout + fb_offset, ptlv->tlv, ptlv->size);
    datasize += ptlv->size;
    fb_offset += ptlv->size;
    ptlv = free_pkd_tlv(ptlv);

    if (datasize < ETH_MIN_DATA_LEN)
        port->tx.sizeout = ETH_MIN_PKT_LEN;
    else
        port->tx.sizeout = fb_offset;
    return true;

error:
    ptlv = free_pkd_tlv(ptlv);
    if (port->tx.frameout)
        GLUE_FREE(port->tx.frameout);
    port->tx.frameout = NULL;
    LLDPAD_DBG("InfoLLDPDU: packed TLV too large for tx frame\n");
    return false;
}

void txFrame(struct lldp_port *port)
{
    if (NULL==port)
    {
        return;
    }
    
    if (0!=l2_packet_send(port->l2, (const u8 *)port->dmac,(u16)(EVB_HTONS(ETH_P_LLDP)),(const u8 *)port->tx.frameout,(u16)port->tx.sizeout))
    {
        port->stats.statsFramesOutErrorsTotal++;
        return;
    }
    
    port->stats.statsFramesOutTotal++;
}

const char* get_tx_sm_string(u16 state)
{
    switch (state)
    {
    case TX_LLDP_INITIALIZE:
        return "TX_LLDP_INITIALIZE";
    case TX_IDLE:
        return "TX_IDLE";
    case TX_SHUTDOWN_FRAME:
        return "TX_SHUTDOWN_FRAME";
    case TX_INFO_FRAME:
        return "TX_INFO_FRAME";
    default:;
        break;
    }

    return " ";
}


void run_tx_sm(struct lldp_port *port)
{
    while (set_tx_state(port) == true)
    {
        switch(port->tx.state) 
        {
        case TX_LLDP_INITIALIZE:
            txInitializeLLDP(port);
            break;
        case TX_IDLE:
            process_tx_idle(port);
            break;
        case TX_SHUTDOWN_FRAME:
            process_tx_shutdown_frame(port);
            break;
        case TX_INFO_FRAME:
            process_tx_info_frame(port);
            break;
        default:
            LLDPAD_DBG("ERROR The TX State Machine is broken!\n");
        }
    }
}

bool set_tx_state(struct lldp_port *port)
{
    u8 ucOldTxState;

    if (NULL==port)
    {
        return false;
    }

    ucOldTxState = port->tx.state;
    
    if ((port->portEnabled == false) && (port->prevPortEnabled == true)) 
    {
        LLDPAD_DBG("set_tx_state: port was disabled\n");
        tx_change_state(port, TX_LLDP_INITIALIZE);
    }
    port->prevPortEnabled = port->portEnabled;

    switch (port->tx.state) 
    {
    case TX_LLDP_INITIALIZE:
        if (port->portEnabled && 
            ((port->adminStatus == enabledRxTx) || (port->adminStatus == enabledTxOnly))) 
        {
            tx_change_state(port, TX_IDLE);
        }
        break;
        
    case TX_IDLE:
        if ((port->adminStatus == disabled) || (port->adminStatus == enabledRxOnly))
        {
            tx_change_state(port, TX_SHUTDOWN_FRAME);
        }
        else if ((port->tx.txNow) && ((port->timers.txCredit > 0))) 
        {
            tx_change_state(port, TX_INFO_FRAME);
        }
        break;
        
    case TX_SHUTDOWN_FRAME:
        if (port->timers.txShutdownWhile == 0)
        {
            tx_change_state(port, TX_LLDP_INITIALIZE);
        }
        break;
        
    case TX_INFO_FRAME:
        tx_change_state(port, TX_IDLE);
        break;
        
    default:
        LLDPAD_DBG("ERROR: The TX State Machine is broken!\n");
    }

    if (ucOldTxState==port->tx.state)
    {
        return false;
    }

    return true;
}

void process_tx_idle(struct lldp_port *port)
{
    u32 tmpTTL;

    tmpTTL = port->timers.msgTxInterval * port->timers.msgTxHold;  

    if (tmpTTL > 65535)
        port->tx.txTTL = 65535;
    else
        port->tx.txTTL = (u16)tmpTTL;

    return;
}

void process_tx_shutdown_frame(struct lldp_port *port)
{
    if (port->timers.txShutdownWhile == 0) {
        if (mibConstrShutdownLLDPDU(port))
            txFrame(port);
        port->timers.txShutdownWhile = port->timers.reinitDelay;
    }
    return;
}

void process_tx_info_frame(struct lldp_port *port)
{
    mibConstrInfoLLDPDU(port);

    txFrame(port);
    if (port->timers.txCredit > 0)
        port->timers.txCredit--;
    port->tx.txNow = false;
    return;
}

void    update_tx_timers(struct lldp_port *port)
{
    if (port->timers.txShutdownWhile)
        port->timers.txShutdownWhile--;

    if (port->timers.txTTR)
        port->timers.txTTR--;

    port->timers.txTick = true;
    return;
}

const char* get_txtimer_sm_string(u16 state)
{
    switch (state)
    {
    case TX_TIMER_BEGIN:
        return "TX_TIMER_BEGIN";
    case TX_TIMER_INITIALIZE:
        return "TX_TIMER_INITIALIZE";
    case TX_TIMER_IDLE:
        return "TX_TIMER_IDLE";
    case TX_TIMER_EXPIRES:
        return "TX_TIMER_EXPIRES";
    case TX_TICK:
        return "TX_TICK";
    case SIGNAL_TX:
        return "SIGNAL_TX";
    case TX_FAST_START:
        return "TX_FAST_START";      
    default:
        break;
    }

    return " ";
}

void    tx_timer_change_state(struct lldp_port *port, u8 newstate)
{
    switch(newstate) {
    case TX_TIMER_INITIALIZE:
        break;
    case TX_TIMER_IDLE:
        break;
    case TX_TIMER_EXPIRES:
        break;
    case TX_TICK:
        break;
    case SIGNAL_TX:
        break;
    case TX_FAST_START:
        break;
    default:
        LLDPAD_DBG("ERROR: tx_timer_change_state:  default\n");
    }

    port->timers.state = newstate;
    return;
}

bool set_tx_timers_state(struct lldp_port *port)
{
    u16 ucOldTxTmrState;
    
    if (NULL==port)
    {
        return false;
    }

    ucOldTxTmrState = port->timers.state;
    
    if ((port->timers.state == TX_TIMER_BEGIN) ||
        (port->portEnabled == false) || (port->adminStatus == disabled) ||
        (port->adminStatus == enabledRxOnly)) 
    {
        tx_timer_change_state(port, TX_TIMER_INITIALIZE);
    }

    switch (port->timers.state) 
    {
    case TX_TIMER_INITIALIZE:
        if (port->portEnabled && ((port->adminStatus == enabledRxTx) ||
            (port->adminStatus == enabledTxOnly))) 
        {
            tx_timer_change_state(port, TX_TIMER_IDLE);
        }
        break;
        
    case TX_TIMER_IDLE:
        if (port->tx.localChange)
        {
            tx_timer_change_state(port, TX_FAST_START);
            break;
        }

        if (port->timers.txTTR == 0) 
        {
            tx_timer_change_state(port, TX_TIMER_EXPIRES);
            break;
        }

        if (port->rx.newNeighbor) 
        {
            tx_timer_change_state(port, TX_FAST_START);
            break;
        }

        if (port->timers.txTick)
        {
            tx_timer_change_state(port, TX_TICK);
            break;
        }
        break;
        
    case TX_TIMER_EXPIRES:
        tx_timer_change_state(port, SIGNAL_TX);
        break;
        
    case SIGNAL_TX:
    case TX_TICK:
        tx_timer_change_state(port, TX_TIMER_IDLE);
        break;
        
    case TX_FAST_START:
        tx_timer_change_state(port, TX_TIMER_EXPIRES);
        break;
        
    default:
        LLDPAD_DBG("ERROR: The TX State Machine is broken!\n");
    }

    if (ucOldTxTmrState==port->timers.state)
    {
        return false;
    }

    return true;

}

void    run_tx_timers_sm(struct lldp_port *port)
{
    while (set_tx_timers_state(port))
    {
        switch(port->timers.state) 
        {
        case TX_TIMER_INITIALIZE:
            txInitializeTimers(port);
            break;
        case TX_TIMER_IDLE:
            break;
        case TX_TIMER_EXPIRES:
            if (port->tx.txFast)
                port->tx.txFast--;
            break;
        case TX_TICK:
            port->timers.txTick = false;
            if (port->timers.txCredit < port->timers.txMaxCredit)
                port->timers.txCredit++;
            break;
        case SIGNAL_TX:
            port->tx.txNow = true;
            port->tx.localChange = 0;
            if (port->tx.txFast)
                port->timers.txTTR = port->timers.msgFastTx;
            else
                port->timers.txTTR = port->timers.msgTxInterval;
            break;
        case TX_FAST_START:
            port->rx.newNeighbor = false;
            if (port->tx.txFast == 0)
                port->tx.txFast = port->timers.txFastInit;
            break;
        default:
            LLDPAD_DBG("ERROR The TX Timer State Machine "
                   "is broken!\n");
        }
    }

    return;
}

void tx_change_state(struct lldp_port *port, u8 newstate)
{
    switch(newstate) {
    case TX_LLDP_INITIALIZE:
        if ((port->tx.state != TX_SHUTDOWN_FRAME) &&
            port->portEnabled) {
            assert(port->portEnabled);
        }
        break;
    case TX_IDLE:
        if (!(port->tx.state == TX_LLDP_INITIALIZE ||
            port->tx.state == TX_INFO_FRAME)) {
            assert(port->tx.state == TX_LLDP_INITIALIZE);
            assert(port->tx.state == TX_INFO_FRAME);
        }
        break;
    case TX_SHUTDOWN_FRAME:
    case TX_INFO_FRAME:
        assert(port->tx.state == TX_IDLE);
        break;
    default:
        LLDPAD_DBG("ERROR: tx_change_state:  default\n");
    }

    port->tx.state = newstate;
    return;
}

#ifdef __cplusplus
}
#endif



