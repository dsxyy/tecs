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

void rxInitializeLLDP(struct lldp_port *port)
{

    port->rx.rcvFrame = false;
    port->rx.badFrame = false;
    port->rx.tooManyNghbrs = false;
    port->rx.rxInfoAge = false;
    if (port->rx.framein)
    {
        GLUE_FREE(port->rx.framein);
        port->rx.framein = NULL;
    }
    port->rx.sizein = 0;

    mibDeleteObjects(port);
    return;
}

void rxReceiveFrame(u32 local_port, u8 *buf, u16 len)
{
    struct lldp_port * port;
    struct l2_ethhdr *hdr;
    struct l2_ethhdr example_hdr,*ex;

    if (NULL==buf)
    {
        LLDPAD_DBG("ERROR - rxReceiveFrame, buf is null,portno:%d!\n",local_port);
        return;
    }

    port = find_by_port(local_port);
    if (NULL==port)
    {
        LLDPAD_DBG("ERROR - can't find port,portno:%d!\n",local_port);
        return;    
    }

    if (port->adminStatus == disabled || port->adminStatus == enabledTxOnly)
    {
        port->stats.statsFramesDiscardedTotal++;
        LLDPAD_DBG("ERROR - port->adminStatus can not support rx,portno:%d!\n",local_port);
        return;
    }

    if ((port->rx.framein && port->rx.sizein == len) && (memcmp(buf, port->rx.framein, len) == 0))
    {
        port->timers.rxTTL = port->timers.lastrxTTL;
        port->stats.statsFramesInTotal++;
        return;
    }

    if (port->rx.framein)
    {
        GLUE_FREE(port->rx.framein);
    }

    port->rx.framein = (u8 *)GLUE_ALLOC(len);
    if (port->rx.framein == NULL)
    {
        LLDPAD_DBG("ERROR - could not allocate memory for rx'ed frame\n");
        return;
    }
    memcpy(port->rx.framein, buf, len);

    port->rx.sizein = (u16)len;
    ex = &example_hdr;
    memcpy(ex->h_dest, port->dmac, ETH_ALEN);
    ex->h_proto = EVB_HTONS(ETH_P_LLDP);
    hdr = (struct l2_ethhdr *)port->rx.framein;

    if ((memcmp(hdr->h_dest,ex->h_dest, ETH_ALEN) != 0))
    {
        LLDPAD_INFO("ERROR LLDP multicast address error in incoming frame.Dropping frame.\n");
        port->stats.statsFramesInErrorsTotal++;        
        GLUE_FREE(port->rx.framein);
        port->rx.framein = NULL;
        port->rx.sizein = 0;
        return;
    }

    if (hdr->h_proto != example_hdr.h_proto)
    {
        LLDPAD_INFO("ERROR Ethertype not LLDP ethertype but ethertype'%x' in incoming frame.\n",
                    EVB_HTONS(hdr->h_proto));
        port->stats.statsFramesInErrorsTotal++;        
        GLUE_FREE(port->rx.framein);
        port->rx.framein = NULL;
        port->rx.sizein = 0;
        return;
    }

    if ((memcmp(hdr->h_source,port->smac, ETH_ALEN) == 0))
    {
        LLDPAD_INFO("ERROR LLDP mac address conflict in incoming frame.Dropping frame.\n");
        port->stats.statsFramesInErrorsTotal++;        
        GLUE_FREE(port->rx.framein);
        port->rx.framein = NULL;
        port->rx.sizein = 0;
        return;
    }    

    port->stats.statsFramesInTotal++;
    port->rx.rcvFrame = true;

    run_rx_sm(port);
}

void rxProcessFrame(struct lldp_port * port)
{
    u16 tlv_cnt = 0;
    u8  tlv_type = 0;
    u16 tlv_length = 0;
    u16 tlv_offset = 0;
    u16 *tlv_head_ptr = NULL;
    u8  frame_error = 0;
    bool msap_compare_1 = false;
    bool msap_compare_2 = false;
    bool good_neighbor  = false;
    bool tlv_stored     = false;
    int err;
    struct lldp_module *np;
    u16 tmp_offset;
    u8 *info = NULL;
    struct unpacked_tlv *tlv = NULL;

    if (NULL==port->rx.framein || 0==port->rx.sizein)
    {
        assert(0);
        return;        
    }
    
    port->lldpdu     = 0;
    port->rx.dupTlvs = 0;

    port->rx.dcbx_st = 0;
    port->rx.manifest = (rxmanifest *)GLUE_ALLOC(sizeof(rxmanifest));
    if (port->rx.manifest == NULL)
    {
        LLDPAD_DBG("ERROR - could not allocate memory for receive " "manifest\n");
        return;
    }
    
    memset(port->rx.manifest,0, sizeof(rxmanifest));
    get_remote_peer_mac_addr(port);
    tlv_offset = sizeof(struct l2_ethhdr);  /* Points to 1st TLV */

    do 
    {
        tlv_cnt++;
        if (tlv_offset > port->rx.sizein)
        {
            LLDPAD_INFO("ERROR: Frame overrun!\n");
            frame_error++;
            goto out;
        }

        tlv_head_ptr = (u16 *)&port->rx.framein[tlv_offset];
        tlv_length = EVB_NTOHS(*tlv_head_ptr) & 0x01FF;
        tlv_type = (u8)(EVB_NTOHS(*tlv_head_ptr) >> 9);

        if (tlv_cnt <= 3)
        {
            if (tlv_cnt != tlv_type)
            {
                LLDPAD_INFO("ERROR:TLV missing or TLVs out " "of order!\n");
                frame_error++;
                goto out;
            }
        }

        if (tlv_cnt > 3)
        {
            if ((tlv_type == CHASSIS_ID_TLV) || (tlv_type == PORT_ID_TLV) || (tlv_type == TIME_TO_LIVE_TLV))
            {
                LLDPAD_INFO("ERROR: Extra CHASSIS_ID_TLV, PORT_ID_TLV, or TIME_TO_LIVE_TLV!\n");
                frame_error++;
                goto out;
            }
        }

        if ((tlv_type == TIME_TO_LIVE_TLV) && (tlv_length != 2))
        {
            LLDPAD_INFO("ERROR:TTL TLV validation error! \n");
            frame_error++;
            goto out;
        }

        tmp_offset = tlv_offset + tlv_length;
        if (tmp_offset > port->rx.sizein)
        {
            LLDPAD_INFO("ERROR: Frame overflow error: offset=%d, rx.size=%d \n", tmp_offset, port->rx.sizein);
            frame_error++;
            goto out;
        }

        info = (u8 *)&port->rx.framein[tlv_offset + sizeof(*tlv_head_ptr)];

        tlv = create_tlv();

        if (!tlv)
        {
            LLDPAD_DBG("ERROR: Failed to GLUE_ALLOC space for incoming TLV. \n");
            goto out;
        }
    
        if ((tlv_length == 0) && (tlv->type != TYPE_0))
        {
            LLDPAD_INFO("ERROR: tlv_length == 0\n");
            free_unpkd_tlv(tlv);
            goto out;
        }
        tlv->type = tlv_type;
        tlv->length = tlv_length;
        tlv->info = (u8 *)GLUE_ALLOC(tlv_length);
        if (tlv->info) 
        {
            memset(tlv->info,0, tlv_length);
            memcpy(tlv->info, info, tlv_length);
        }
        else
        {
            if (tlv->type != TYPE_0)
            {
                LLDPAD_DBG("ERROR: Failed to GLUE_ALLOC space for incoming TLV info! tlv_length:%d\n",tlv_length);
                free_unpkd_tlv(tlv);
                goto out;
            }
        }

        /* Validate the TLV */
        tlv_offset += sizeof(*tlv_head_ptr) + tlv_length;
        /* Get MSAP info */
        if (tlv->type == TYPE_1)
        { /* chassis ID */
            if (port->lldpdu & RCVD_LLDP_TLV_TYPE1)
            {
                LLDPAD_INFO("Received multiple Chassis ID TLVs in this LLDPDU\n");
                frame_error++;
                free_unpkd_tlv(tlv);
                goto out;
            }
            else
            {
                port->lldpdu |= RCVD_LLDP_TLV_TYPE1;
                port->rx.manifest->chassis = tlv;
                tlv_stored = true;
            }

            if (port->msap.msap1 == NULL)
            {
                port->msap.length1 = tlv->length;
                port->msap.msap1 = (u8 *)GLUE_ALLOC(tlv->length);
                if (!(port->msap.msap1))
                {
                    LLDPAD_DBG("ERROR: Failed to GLUE_ALLOC space for msap1\n");
                    free_unpkd_tlv(tlv);
                    goto out;
                }
                memcpy(port->msap.msap1, tlv->info, tlv->length);
            }
            else
            {
                if (tlv->length == port->msap.length1)
                {
                    if ((memcmp(tlv->info,port->msap.msap1, tlv->length) == 0))
                    {
                        msap_compare_1 = true;
                    }
                }
            }
        }
        
        if (tlv->type == TYPE_2)
        { /* port ID */
            if (port->lldpdu & RCVD_LLDP_TLV_TYPE2)
            {
                LLDPAD_INFO("Received multiple Port ID TLVs in this LLDPDU\n");
                frame_error++;
                free_unpkd_tlv(tlv);
                goto out;
            }
            else
            {
                port->lldpdu |= RCVD_LLDP_TLV_TYPE2;
                port->rx.manifest->portid = tlv;
                tlv_stored = true;
            }

            if (port->msap.msap2 == NULL)
            {
                port->msap.length2 = tlv->length;
                port->msap.msap2 = (u8 *)GLUE_ALLOC(tlv->length);
                if (!(port->msap.msap2))
                {
                    LLDPAD_DBG("ERROR: Failed to GLUE_ALLOC space for msap2\n");
                    free_unpkd_tlv(tlv);
                    goto out;
                }
                memcpy(port->msap.msap2, tlv->info, tlv->length);
                port->rx.newNeighbor = true;
            }
            else
            {
                if (tlv->length == port->msap.length2)
                {
                    if ((memcmp(tlv->info,port->msap.msap2, tlv->length) == 0))
                    {
                        msap_compare_2 = true;
                    }
                }
                
                if ((msap_compare_1 == true) && (msap_compare_2 == true))
                {
                    msap_compare_1 = false;
                    msap_compare_2 = false;
                    good_neighbor = true;
                } 
                else
                {
                    /* New neighbor */
                    port->rx.tooManyNghbrs = true;
                    port->rx.newNeighbor = true;
                    LLDPAD_INFO("** TOO_MANY_NGHBRS\n");
                }
            }
        }
        
        if (tlv->type == TYPE_3) 
        { /* time to live */
            if (port->lldpdu & RCVD_LLDP_TLV_TYPE3) 
            {
                LLDPAD_INFO("Received multiple TTL TLVs in this LLDPDU\n");
                frame_error++;
                free_unpkd_tlv(tlv);
                goto out;
            }
            else 
            {
                port->lldpdu |= RCVD_LLDP_TLV_TYPE3;
                port->rx.manifest->ttl = tlv;
                tlv_stored = true;
            }
            
            if ((port->rx.tooManyNghbrs == true) && (good_neighbor == false)) 
            {
                LLDPAD_INFO("** set tooManyNghbrsTimer\n");
                port->timers.tooManyNghbrsTimer = MAX(EVB_NTOHS(*(u16 *)tlv->info), port->timers.tooManyNghbrsTimer);
                msap_compare_1 = false;
                msap_compare_2 = false;
            } 
            else 
            {
                port->timers.rxTTL = EVB_NTOHS(*(u16 *)tlv->info);
                port->timers.lastrxTTL = port->timers.rxTTL;
                good_neighbor = false;
            }
        }
        
        if (tlv->type == TYPE_4) 
        { /* port description */
            port->lldpdu |= RCVD_LLDP_TLV_TYPE4;
            port->rx.manifest->portdesc = tlv;
            tlv_stored = true;
        }
        
        if (tlv->type == TYPE_5) 
        { /* system name */
            port->lldpdu |= RCVD_LLDP_TLV_TYPE5;
            port->rx.manifest->sysname = tlv;
            tlv_stored = true;
        }
        
        if (tlv->type == TYPE_6) 
        { /* system description */
            port->lldpdu |= RCVD_LLDP_TLV_TYPE6;
            port->rx.manifest->sysdesc = tlv;
            tlv_stored = true;
        }
        
        if (tlv->type == TYPE_7) 
        { /* system capabilities */
            port->lldpdu |= RCVD_LLDP_TLV_TYPE7;
            port->rx.manifest->syscap = tlv;
            tlv_stored = true;
        }
        
        if (tlv->type == TYPE_8) 
        { /* mgmt address */
            port->lldpdu |= RCVD_LLDP_TLV_TYPE8;
            port->rx.manifest->mgmtadd = tlv;
            tlv_stored = true;
        }

        /* rx per lldp module */
        LIST_FOREACH(np, &gtLLDP.mod_head, lldp) 
        {
            if (!np->ops || !np->ops->lldp_mod_rchange)
            {
                continue;
            }

            err = np->ops->lldp_mod_rchange(port, tlv, &tlv_stored);
            if (err == TLV_ERR)
            {
                frame_error++;
                tlv = free_unpkd_tlv(tlv);
                goto out;
            } 
        }

        if (tlv->type == TYPE_0) 
        { /* end tlv */
            tlv = free_unpkd_tlv(tlv);            
            tlv_stored = true;
        }        

        if (!tlv_stored)
        {
            LLDPAD_WARN("\n%s: port %d: Unknown TLV 0x%04x\n", __func__, port->local_port, tlv->info[OUI_SIZE]);
            LLDPAD_INFO("rxProcessFrame: allocated TLV (%d) was not stored! (0x%x)\n", tlv->type, tlv);
            tlv = free_unpkd_tlv(tlv);
            port->stats.statsTLVsUnrecognizedTotal++;
        }

        tlv = NULL;
        tlv_stored = false;
    } while(tlv_type != TYPE_0);

out:
    if (frame_error) 
    {
        /* discard the frame because of errors. */
        port->stats.statsFramesDiscardedTotal++;
        port->stats.statsFramesInErrorsTotal++;
        port->rx.badFrame = true;
        
        GLUE_FREE(port->rx.framein);
        port->rx.framein  = NULL;
        port->rx.sizein   = 0;        
    }

    clear_manifest(port);

    return;
}

u8 mibDeleteObjects(struct lldp_port *port)
{
    struct lldp_module *np;

    if (NULL==port)
    {
        return 1;
    }
    
    LIST_FOREACH(np, &gtLLDP.mod_head, lldp) {
        if (!np->ops || !np->ops->lldp_mod_mibdelete)
            continue;
        np->ops->lldp_mod_mibdelete(port);
    }

    /* Clear history */
    port->msap.length1 = 0;
    if (port->msap.msap1) {
        GLUE_FREE(port->msap.msap1);
        port->msap.msap1 = NULL;
    }

    port->msap.length2 = 0;
    if (port->msap.msap2) {
        GLUE_FREE(port->msap.msap2);
        port->msap.msap2 = NULL;
    }

    l2_packet_clear_remote_addr(port->l2);
    port->lldpdu = 0;

    return 0;
}

const char* get_rx_sm_string(u16 state)
{
    switch (state)
    {
    case LLDP_WAIT_PORT_OPERATIONAL:
        return "LLDP_WAIT_PORT_OPERATIONAL";
    case DELETE_AGED_INFO:
        return "DELETE_AGED_INFO";
    case RX_LLDP_INITIALIZE:
        return "RX_LLDP_INITIALIZE";
    case RX_WAIT_FOR_FRAME:
        return "RX_WAIT_FOR_FRAME";      
    case RX_FRAME:
        return "RX_FRAME";
    case DELETE_INFO:
        return "DELETE_INFO";
    case UPDATE_INFO:
        return "UPDATE_INFO";      
    default:;
        break;
    }

    return " ";
}

void run_rx_sm(struct lldp_port *port)
{
    while (set_rx_state(port) == true)
    {
        switch(port->rx.state) 
        {
        case LLDP_WAIT_PORT_OPERATIONAL:
            process_wait_port_operational(port);
            break;
        case DELETE_AGED_INFO:
            process_delete_aged_info(port);
            break;
        case RX_LLDP_INITIALIZE:
            process_rx_lldp_initialize(port);
            break;
        case RX_WAIT_FOR_FRAME:
            process_wait_for_frame(port);
            break;
        case RX_FRAME:
            process_rx_frame(port);
            break;
        case DELETE_INFO:
            process_delete_info(port);
            break;
        case UPDATE_INFO:
            process_update_info(port);
            break;
        default:
            LLDPAD_DBG("ERROR: The RX State Machine is broken!\n");
        }
    }
}

bool set_rx_state(struct lldp_port *port)
{
    u8 ucOldRxState;
    
    if (NULL==port)
    {
        return false;
    }

    ucOldRxState = port->rx.state;
    
    if ((port->rx.rxInfoAge == false) && (port->portEnabled == false)) 
    {
        rx_change_state(port, LLDP_WAIT_PORT_OPERATIONAL);
    }

    switch (port->rx.state) 
    {
    case LLDP_WAIT_PORT_OPERATIONAL:
        if (port->rx.rxInfoAge == true)
        {
            rx_change_state(port, DELETE_AGED_INFO);
        }
        else if (port->portEnabled == true) 
        {
            rx_change_state(port, RX_LLDP_INITIALIZE);
        }
        break;
        
    case DELETE_AGED_INFO:
        rx_change_state(port, LLDP_WAIT_PORT_OPERATIONAL);
        break;
        
    case RX_LLDP_INITIALIZE:
        if ((port->adminStatus == enabledRxTx) ||
            (port->adminStatus == enabledRxOnly))
        {
            rx_change_state(port, RX_WAIT_FOR_FRAME);
        }
        break;
        
    case RX_WAIT_FOR_FRAME:
        if ((port->adminStatus == disabled) ||
            (port->adminStatus == enabledTxOnly)) 
        {
            rx_change_state(port, RX_LLDP_INITIALIZE);
        }        
        if (port->rx.rxInfoAge == true) 
        {
            rx_change_state(port, DELETE_INFO);
        }
        else if (port->rx.rcvFrame == true) 
        {
            rx_change_state(port, RX_FRAME);
        }
        break;
        
    case DELETE_INFO:
        rx_change_state(port, RX_WAIT_FOR_FRAME);
        break;
        
    case RX_FRAME:
        if (port->timers.rxTTL == 0) 
        {
            rx_change_state(port, DELETE_INFO);
        }
        else if ((port->timers.rxTTL != 0) && (port->rxChanges == true)) 
        {
            rx_change_state(port, UPDATE_INFO);
        }
        else
        {
            rx_change_state(port, RX_WAIT_FOR_FRAME);
        }
        break;
        
    case UPDATE_INFO:
        rx_change_state(port, RX_WAIT_FOR_FRAME);
        break;
        
    default:
        LLDPAD_DBG("ERROR: The RX State Machine is broken!\n");
    }

    if (ucOldRxState==port->rx.state)
    {
        return false;
    }

    return true;
}

void process_wait_port_operational(struct lldp_port *port)
{
    return;
}

void process_delete_aged_info(struct lldp_port *port)
{
    mibDeleteObjects(port);
    port->rx.rxInfoAge = false;
    port->rx.remoteChange = true;
    return;
}

void process_rx_lldp_initialize(struct lldp_port *port)
{
    rxInitializeLLDP(port);
    port->rx.rcvFrame = false;
    return;
}

void process_wait_for_frame(struct lldp_port *port)
{
    port->rx.badFrame  = false;
    port->rx.rxInfoAge = false;
    return;
}

void process_rx_frame(struct lldp_port *port)
{
    port->rx.remoteChange = false;
    port->rxChanges = false;
    port->rx.rcvFrame = false;
    rxProcessFrame(port);
    return;
}

void process_delete_info(struct lldp_port *port)
{
    mibDeleteObjects(port);

    if (port->rx.framein) {
        GLUE_FREE(port->rx.framein);
        port->rx.framein = NULL;
    }

    port->rx.sizein = 0;
    port->rx.remoteChange = true;
    return;
}

void process_update_info(struct lldp_port *port)
{
    port->rx.remoteChange = true;
    return;
}

void update_rx_timers(struct lldp_port *port)
{

    if (port->timers.rxTTL) {
        port->timers.rxTTL--;
        if (port->timers.rxTTL == 0) {
            port->rx.rxInfoAge = true;
            if (port->timers.tooManyNghbrsTimer != 0) {
                LLDPAD_DBG("** clear tooManyNghbrsTimer\n");
                port->timers.tooManyNghbrsTimer = 0;
                port->rx.tooManyNghbrs = false;
            }
        }
    }
    if (port->timers.tooManyNghbrsTimer) {
        port->timers.tooManyNghbrsTimer--;
        if (port->timers.tooManyNghbrsTimer == 0) {
            LLDPAD_DBG("** tooManyNghbrsTimer timeout\n");
            port->rx.tooManyNghbrs = false;
        }
    }
}

void rx_change_state(struct lldp_port *port, u8 newstate) {
    switch(newstate) {
        case LLDP_WAIT_PORT_OPERATIONAL:
            break;
        case RX_LLDP_INITIALIZE:
            assert((port->rx.state == LLDP_WAIT_PORT_OPERATIONAL) ||
                   (port->rx.state == RX_WAIT_FOR_FRAME));
            break;
        case DELETE_AGED_INFO:
            assert(port->rx.state ==
                LLDP_WAIT_PORT_OPERATIONAL);
            break;
        case RX_WAIT_FOR_FRAME:
            if (!(port->rx.state == RX_LLDP_INITIALIZE ||
                port->rx.state == DELETE_INFO ||
                port->rx.state == UPDATE_INFO ||
                port->rx.state == RX_FRAME)) {
                assert(port->rx.state !=
                    RX_LLDP_INITIALIZE);
                assert(port->rx.state != DELETE_INFO);
                assert(port->rx.state != UPDATE_INFO);
                assert(port->rx.state != RX_FRAME);
            }
            break;
        case RX_FRAME:
            assert(port->rx.state == RX_WAIT_FOR_FRAME);
            break;
        case DELETE_INFO:
            if (!(port->rx.state == RX_WAIT_FOR_FRAME ||
                port->rx.state == RX_FRAME)) {
                assert(port->rx.state == RX_WAIT_FOR_FRAME);
                assert(port->rx.state == RX_FRAME);
            }
            break;
        case UPDATE_INFO:
            assert(port->rx.state == RX_FRAME);
            break;
        default:
            LLDPAD_DBG("ERROR: The RX State Machine is broken!\n");
    }
    port->rx.state = newstate;
}

void clear_manifest(struct lldp_port *port) {
    if (port->rx.manifest->mgmtadd)
        port->rx.manifest->mgmtadd =
            free_unpkd_tlv(port->rx.manifest->mgmtadd);
    if (port->rx.manifest->syscap)
        port->rx.manifest->syscap =
            free_unpkd_tlv(port->rx.manifest->syscap);
    if (port->rx.manifest->sysdesc)
        port->rx.manifest->sysdesc =
            free_unpkd_tlv(port->rx.manifest->sysdesc);
    if (port->rx.manifest->sysname)
        port->rx.manifest->sysname =
            free_unpkd_tlv(port->rx.manifest->sysname);
    if (port->rx.manifest->portdesc)
        port->rx.manifest->portdesc =
            free_unpkd_tlv(port->rx.manifest->portdesc);
    if (port->rx.manifest->ttl)
        port->rx.manifest->ttl =
            free_unpkd_tlv(port->rx.manifest->ttl);
    if (port->rx.manifest->portid)
        port->rx.manifest->portid =
            free_unpkd_tlv(port->rx.manifest->portid);
    if (port->rx.manifest->chassis)
        port->rx.manifest->chassis =
            free_unpkd_tlv(port->rx.manifest->chassis);
    GLUE_FREE(port->rx.manifest);
    port->rx.manifest = NULL;
}

#ifdef __cplusplus
}
#endif




