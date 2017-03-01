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

struct lldp_port *porthead = NULL; /* Head pointer */
struct lldp_port *portcurrent = NULL; /* Working  pointer loaded from ports or
                  * port->next */

void agent_receive(void *, const u8 *, const u8 *, u16);

int destroy_port(struct lldp_port *port)
{
    struct lldp_module *np;
    
    if (NULL==port)
    {
        return -1;
    }
    
    /* Close down the socket */
    l2_packet_deinit(port->l2);

    /* Re-initialize relevant port variables */
    port->tx.state = TX_LLDP_INITIALIZE;
    port->timers.state = TX_TIMER_BEGIN;
    port->rx.state = LLDP_WAIT_PORT_OPERATIONAL;
    port->portEnabled  = false;
    port->adminStatus  = disabled;
    port->tx.txTTL = 0;

    /* Remove the tlvs */
    if (port->msap.msap1)
    {
        GLUE_FREE(port->msap.msap1);
        port->msap.msap1 = NULL;
    }

    if (port->msap.msap2)
    {
        GLUE_FREE(port->msap.msap2);
        port->msap.msap2 = NULL;
    }

    if (port->rx.framein)
    {
        GLUE_FREE(port->rx.framein);
    }

    if (port->tx.frameout)
    {
        GLUE_FREE(port->tx.frameout);
    }

    LIST_FOREACH(np, &gtLLDP.mod_head, lldp) 
    {
        if (np->ops->lldp_mod_ifdel)
        {
            np->ops->lldp_mod_ifdel(port->local_port);
        }
    }

    GLUE_FREE(port);

    return 0;
    
}

const char* get_lldp_port_admin_string(u8 ucAdminStatus)
{
    switch (ucAdminStatus)
    {
    case disabled:
        return "disabled";
    case enabledTxOnly:
        return "TxOnly";
    case enabledRxOnly:
        return "RxOnly";
    case enabledRxTx:
        return "RxTx";
    default:;
        break;
    }

    return " ";
}

/* Port routines used for command processing -- return cmd_xxx codes */

int get_lldp_port_statistics(u32 local_port, struct portstats *stats)
{
    struct lldp_port *port;

    port = find_by_port(local_port);
    if (!port)
        return cmd_device_not_found;
    memcpy((void *)stats, (void *)&port->stats, sizeof(struct portstats));
    return cmd_success;
}

int get_local_tlvs(u32 local_port, unsigned char *tlvs, int *size)
{
    struct lldp_port *port;

    port = find_by_port(local_port);
    if (!port)
        return cmd_device_not_found;

    if (port->tx.frameout == NULL) {
        *size = 0;
        return cmd_success;
    }

    *size = port->tx.sizeout - sizeof(struct l2_ethhdr);
    if (*size < 0)
        return cmd_invalid;

    memcpy((unsigned int *)tlvs,(unsigned int *)port->tx.frameout + sizeof(struct l2_ethhdr), *size);

    return cmd_success;
}

int get_neighbor_tlvs(u32 local_port, unsigned char *tlvs, int *size)
{
    struct lldp_port *port;

    port = find_by_port(local_port);
    if (!port)
        return cmd_device_not_found;

    if (port->rx.framein == NULL) {
        *size = 0;
        return cmd_success;
    }

    *size = port->rx.sizein - sizeof(struct l2_ethhdr);
    if (*size < 0)
        return cmd_invalid;
    memcpy((unsigned int *)tlvs,(unsigned int *)port->rx.framein + sizeof(struct l2_ethhdr), *size);
    return cmd_success;
}

/* Routines used for managing interfaces -- return std C return values */

int get_lldp_port_admin(u32 local_port)
{
    struct lldp_port *port = NULL;

    port = find_by_port(local_port);
    if (!port)
    {
        return disabled;
    }

    return port->adminStatus;    
}

void set_lldp_port_admin(u32 local_port, u8 admin)
{
    struct lldp_port *port = NULL;

    port = find_by_port(local_port);
    if (!port)
    {
        return;
    }

    if (admin==disabled)
    {
        port->tx.txTTL = 0;
    }

    if (port->adminStatus != admin)
    {
        port->adminStatus = admin;
        somethingChangedLocal(local_port);
        run_tx_sm(port);
        run_rx_sm(port);
    }
}

void set_lldp_port_enable_state(u32 local_port, u8 enable)
{
    struct lldp_port *port = NULL;

    port = find_by_port(local_port);
    if (!port)
    {
        return;
    }

    port->portEnabled = enable;

    if (!enable) /* port->adminStatus = disabled; */
    {
        port->tx.txTTL = 0;
        port->rx.rxInfoAge = false;
    }

    run_tx_sm(port);
    run_rx_sm(port);
}

void set_port_oper_delay(u32 local_port)
{
    struct lldp_port *port = NULL;

    port = find_by_port(local_port);
    if (!port)
    {
        return;
    }

    port->timers.dormantDelay = DORMANT_DELAY;
    return;
}

int set_port_hw_resetting(u32 local_port, int resetting)
{
    struct lldp_port *port = NULL;

    port = find_by_port(local_port);
    if (!port)
    {
        return -1;
    }

    port->hw_resetting = (u8)resetting;

    return port->hw_resetting;
}

int get_port_hw_resetting(u32 local_port)
{
    struct lldp_port *port = NULL;

    port = find_by_port(local_port);
    if (!port)
    {
        return 0;
    }

    return port->hw_resetting;
}

int lldp_reinit_port(u32 local_port)
{
    struct lldp_port *port;

    port = find_by_port(local_port);
    if (!port)
    {
        return -1;
    }

    /* Reset relevant port variables */
    port->tx.state  = TX_LLDP_INITIALIZE;
    port->timers.state  = TX_TIMER_BEGIN;
    port->rx.state = LLDP_WAIT_PORT_OPERATIONAL;
    port->hw_resetting = false;
    /*port->portEnabled = false; */
    port->tx.txTTL = 0;
    port->msap.length1 = 0;
    port->msap.msap1 = NULL;
    port->msap.length2 = 0;
    port->msap.msap2 = NULL;
    port->lldpdu = 0;
    port->timers.dormantDelay = DORMANT_DELAY;

    /* init & enable RX path */
    rxInitializeLLDP(port);

    /* init TX path */
    txInitializeTimers(port);
    txInitializeLLDP(port);

    return 0;
}

void lldp_port_up(u32 local_port)
{
    struct lldp_module *np;   

    LIST_FOREACH(np, &gtLLDP.mod_head, lldp) 
    {
        if (np->ops->lldp_mod_ifup)
        {
            np->ops->lldp_mod_ifup(local_port);
        }
    }

    set_lldp_port_enable_state(local_port, 1);

    LLDPAD_DBG("port:%d, lldp_port_up!\n",local_port);
    
}

void lldp_port_down(u32 local_port)
{
    struct lldp_module *np;   

    LIST_FOREACH(np, &gtLLDP.mod_head, lldp) 
    {
        if (np->ops->lldp_mod_ifdown)
        {
            np->ops->lldp_mod_ifdown(local_port);
        }
    }

    set_lldp_port_enable_state(local_port, 0);

    LLDPAD_DBG("port:%d, lldp_port_down!\n",local_port);
    
}


void lldp_init_ports(void)
{
    if (gtLLDP.pfPortInit)
    {
        gtLLDP.pfPortInit();   
    }
}

void lldp_deinit_ports(void)
{
    struct lldp_port *pDelPort = NULL;

    while (porthead)
    {
        pDelPort = porthead;
        porthead = porthead->next;
        destroy_port(pDelPort);
    }    
}

int create_lldp_port(u32 local_port, u8 *name, u8 *mac, u8 agent_mode)
{
    struct lldp_port *newport;
    struct lldp_module *np;

    newport = find_by_port(local_port);
    if (newport)
    {
        return 0;
    }

    if (NULL==name || NULL==mac)
    {
        return 0;
    }

    if (agent_mode > NearestCustomerBridge)
    {
        return 0;
    }
    
    newport  = (struct lldp_port *)GLUE_ALLOC(sizeof(struct lldp_port));
    if (newport == NULL)
    {
        LLDPAD_DBG("new lldp_port GLUE_ALLOC failed\n");
        goto fail;
    }
    
    memset(newport,0,sizeof(struct lldp_port));
    newport->next = NULL;
    newport->local_port = local_port;

    /* Initialize relevant port variables */
    newport->tx.state  = TX_LLDP_INITIALIZE;
    newport->timers.state = TX_TIMER_BEGIN;
    newport->rx.state = LLDP_WAIT_PORT_OPERATIONAL;
    newport->hw_resetting = false;
    newport->portEnabled = false;

    newport->adminStatus = enabledRxTx;
    newport->tx.txTTL = 0;
    newport->msap.length1 = 0;
    newport->msap.msap1 = NULL;
    newport->msap.length2 = 0;
    newport->msap.msap2 = NULL;
    newport->extlv.app      = true;
    newport->extlv.port_des = true;
    newport->extlv.sys_name = true;
    newport->extlv.sys_des  = true;
    newport->extlv.sys_cap  = true;
    newport->extlv.man_addr = true;
    newport->extlv.etscfg   = true;
    newport->extlv.etsrec   = true;
    newport->extlv.pfc      = true;
    newport->extlv.app      = true;
    newport->extlv.qcn      = true;
    newport->extlv.evbtlv   = true;
    newport->extlv.cdcp     = true;

    newport->lldpdu = 0;
    newport->timers.dormantDelay = DORMANT_DELAY;
    newport->agent_mode     = agent_mode;

    switch (newport->agent_mode)
    {
        case NearestBridge:
            memcpy(newport->dmac, multi_cast_mac1, ETH_ALEN);
            break;
        case NearestNonTPMRBridge:
            memcpy(newport->dmac, multi_cast_mac2, ETH_ALEN);
            break;
        case NearestCustomerBridge:
            memcpy(newport->dmac, multi_cast_mac3, ETH_ALEN);
            break;
        default:
            memcpy(newport->dmac, multi_cast_mac1, ETH_ALEN);
    }    
    
    memcpy(newport->name, name, IFNAMSIZ);
    memcpy(newport->smac, mac, ETH_ALEN);

    /* init & enable RX path */
    rxInitializeLLDP(newport);

    /* init TX path */
    txInitializeTimers(newport);
    txInitializeLLDP(newport);

    /* enable TX path */
    if (NULL==porthead)
    {
        porthead = newport;
    }
    else
    {
        struct lldp_port *p;

        p = porthead;
        while (p->next)
        {
            p = p->next;
        }
        p->next = newport;
    }
    
    newport->l2 = l2_packet_init(local_port, NULL, ETH_P_LLDP,1);
    if (newport->l2 == NULL)
    {
        LLDPAD_DBG("Failed to open register layer 2 access to " "ETH_P_LLDP\n");
        goto fail;
    }    

    LIST_FOREACH(np, &gtLLDP.mod_head, lldp) 
    {
        if (np->ops->lldp_mod_ifadd)
        {
            np->ops->lldp_mod_ifadd(local_port);
        }
    }
    
    return 0;

fail:
    if(newport) {
        GLUE_FREE(newport);
    }
    return -1;
}

int destroy_lldp_port(u32 local_port)
{
    struct lldp_port *port = NULL;    /* Pointer to port to remove */
    struct lldp_port *parent = NULL;  /* Pointer to previous on port stack */

    port = porthead;
    while (port != NULL)
    {
        if (port->local_port == local_port) 
        {
            LLDPAD_DBG("In destroy_lldp_port: Found port %x\n",port->local_port);
            break;
        }
        parent = port;
        port = port->next;
    }

    if (port == NULL) 
    {
        LLDPAD_DBG("destroy_lldp_port: port not present\n");
        return -1;
    }

    /* Take this port out of the chain */
    if (parent == NULL)
    {
        porthead = port->next;
    }
    else if (parent->next == port) /* Sanity check */
    {
        parent->next = port->next;
    }
    else
    {
        return -1;
    }

    return destroy_port(port);

}

/*
 * port_needs_shutdown - check if we need send LLDP shutdown frame on this port
 * @port: the port struct
 *
 * Return 1 for yes and 0 for no.
 *
 * No shutdown frame for port that has dcb enabled
 */
int port_needs_shutdown(struct lldp_port *port)
{
#if 0
    return !check_port_dcb_mode(port->ifname);
#endif
    return 1;
}

struct lldp_port *find_by_port(u32 local_port)
{
    struct lldp_port *port = porthead;

    while (port) {
        if (local_port==port->local_port)
        {
            return port;
        }
        port = port->next;
    }
    return NULL;
}

int is_support_lldp(u32 local_port)
{
    #ifdef V4_VERSION   
    net_if *pNet = NULL;

    if (NULL==(pNet = getLocal_OuterNetIf(local_port)))
    {
        return -1;
    }

    if((IF_LOGTYPE(pNet) == PORT_OUTER_TYPE)
         && ((IF_PHYTYPE(pNet) == PORT_FEI_TYPE)
         ||  (IF_PHYTYPE(pNet) == PORT_GEI_TYPE)
         ||  (IF_PHYTYPE(pNet) == PORT_FEIF_TYPE)
         ||  (IF_PHYTYPE(pNet) == PORT_GEIL_TYPE)
         ||  (IF_PHYTYPE(pNet) == PORT_CHNL_TYPE)
         ))
    {
        return 1;
    }
    else
    {
        return 0;
    }
    #else
    return 1;
    #endif
}


#ifdef __cplusplus
}
#endif



