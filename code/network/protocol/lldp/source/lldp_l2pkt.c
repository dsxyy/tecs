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

/*
#include "l2_packet.h"
#include "lldp_util.h"
#include "dcb_types.h"
#include "lldp/states.h"
#include "lldp_dcbx_nl.h"
*/

#include "../include/lldp_in.h"
#if 0
static struct lldp_port *bond_porthead = NULL;
#endif

int l2_packet_get_own_src_addr(struct l2_packet_data *l2, u8 *addr)
{
#if 0
    if (is_san_mac(l2->san_mac_addr))
        memcpy(addr, l2->san_mac_addr, ETH_ALEN);
    else {
        /* get an appropriate src MAC to use if the port is
        * part of a bond.
        */
        struct port *bond_port = bond_porthead;
        while (bond_port != NULL) {
            if (get_src_mac_from_bond(bond_port, l2->ifname, addr))
                return 0;

            bond_port = bond_port->next;
        }
        memcpy(addr, l2->curr_mac_addr, ETH_ALEN);
    }
#endif

    memcpy(addr, l2->curr_mac_addr, ETH_ALEN);

    return 0;
}


/*
 * Extracts the remote peer's MAC address from the rx frame  and
 * puts it in the l2_packet_data
 */
void get_remote_peer_mac_addr(struct lldp_port *port)
{
    int offset = ETH_ALEN;  /* points to remote MAC address in RX frame */
    memcpy(port->l2->remote_mac_addr, &port->rx.framein[offset], ETH_ALEN);
}

void l2_packet_get_remote_addr(struct l2_packet_data *l2, u8 *addr)
{
    if (NULL==l2 || NULL==addr)
    {
        return;
    }    
    memcpy(addr, l2->remote_mac_addr, ETH_ALEN);
}

void l2_packet_clear_remote_addr(struct l2_packet_data *l2)
{
    if (NULL==l2)
    {
        return;
    }
    memset(l2->remote_mac_addr, 0, ETH_ALEN);
}

int l2_packet_get_own_addr(struct l2_packet_data *l2, u8 *addr)
{
    memcpy(addr, l2->perm_mac_addr, ETH_ALEN);
    return 0;
}

int l2_packet_send(struct l2_packet_data *l2, const u8 *dst_addr, u16 proto, const u8 *buf, u16 len)
{
    u8 *p = NULL;
    
    if (l2 == NULL || NULL==dst_addr || NULL==buf || NULL==gtLLDP.pfSndPkt)
    {
        return -1;
    }

    if (NULL==(p=(u8*)GLUE_ALLOC(len + 18)))
    {
        return -1;
    }

    if (!l2->l2_hdr)
    {
        proto = EVB_HTONS(proto);
        memcpy(p,  dst_addr, ETH_ALEN);
        memcpy(p + ETH_ALEN, l2->curr_mac_addr, ETH_ALEN);
        memcpy(p + 2*ETH_ALEN, &proto, 2);
        memcpy(p + 14,(u8*)buf, len);
        len = len + 14;
    }
    else
    {
        memcpy(p, (u8*)buf, len);
    }

    if (0!=gtLLDP.pfSndPkt(l2->local_port, (u8*)p, (u16)len))
    {
        GLUE_FREE(p);
        return -1;
    }

    GLUE_FREE(p);    
    return 0;
}

#if 0

static void l2_packet_receive(int sock, void *eloop_ctx, void *sock_ctx)
{
    struct l2_packet_data *l2 = eloop_ctx;
    u8 buf[2300];
    int res;
    struct sockaddr_ll ll;
    socklen_t fromlen;

    memset(&ll, 0, sizeof(ll));
    fromlen = sizeof(ll);
    res = recvfrom(sock, buf, sizeof(buf), 0, (struct sockaddr *) &ll,
               &fromlen);

    if (res < 0) {
        LLDPAD_INFO("receive ERROR = %d\n", res);
        perror("l2_packet_receive - recvfrom");
        return;
    }

    l2->rx_callback(l2->rx_callback_ctx, ll.sll_ifindex, buf, res);

}
#endif
struct l2_packet_data * l2_packet_init(u32 local_port, const u8 *own_addr, unsigned short protocol, int l2_hdr)
{
    struct l2_packet_data *l2;
    struct lldp_port *port;

    port = find_by_port(local_port);
    if (NULL==port)
    {
        return NULL;
    }    

    l2 = (struct l2_packet_data *)GLUE_ALLOC(sizeof(struct l2_packet_data));
    if (l2 == NULL)
    {
        return NULL;
    }
    
    memset(l2, 0, sizeof(*l2));
    l2->local_port = local_port;
    l2->l2_hdr     = l2_hdr;
    
    memcpy(l2->perm_mac_addr, port->smac, ETH_ALEN);
    memcpy(l2->curr_mac_addr, port->smac, ETH_ALEN);  
    memcpy(l2->san_mac_addr,  port->smac, ETH_ALEN);    

    return l2;
 
}


void l2_packet_deinit(struct l2_packet_data *l2)
{
    if (l2 == NULL)
    {
        return;
    }
        
    GLUE_FREE(l2);
}

void l2_packet_get_port_state(struct l2_packet_data *l2, u8  *portEnabled)
{
#if 0

    int s;
    struct ifreq ifr;

    s = socket(PF_INET, SOCK_DGRAM, 0);
    if (s < 0) {
        perror("socket");
        return;
    }
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, l2->ifname, sizeof(ifr.ifr_name));

    if (ioctl(s, SIOCGIFFLAGS, &ifr) < 0) {
        perror("ioctl[SIOCGIFFLAGS]");
        close(s);
        *portEnabled = 0;
        return;
    }
        
    *portEnabled = ifr.ifr_flags & IFF_UP;
    close(s);
#endif
    return;
}


int add_bond_port(const char *ifname)
{
#if 0
    struct port *bond_newport;

    bond_newport = bond_porthead;
    while (bond_newport != NULL) {
        if(!strncmp(ifname, bond_newport->ifname,
            MAX_DEVICE_NAME_LEN))
            return 0;
        bond_newport = bond_newport->next;
    }

    bond_newport  = (struct port *)GLUE_ALLOC(sizeof(struct port));
    if (bond_newport == NULL) {
        syslog(LOG_ERR, "failed to GLUE_ALLOC bond port %s", ifname);
        return -1;
    }
    memset(bond_newport,0,sizeof(struct port)); 
    bond_newport->next = NULL;
    bond_newport->ifname = strdup(ifname);
    if (bond_newport->ifname == NULL) {
        syslog(LOG_ERR, "failed to strdup bond name %s", ifname);
        goto fail1;
    }
    
    bond_newport->l2 = l2_packet_init(bond_newport->ifname, NULL,
        ETH_P_LLDP, recv_on_bond, bond_newport, 1);

    if (bond_newport->l2 == NULL) {
        syslog(LOG_ERR, "failed to open register layer 2 access to "
               "ETH_P_LLDP");
        goto fail2;
    }

    if (bond_porthead)
        bond_newport->next = bond_porthead;
    bond_porthead = bond_newport;

    return 0;

fail2:
    GLUE_FREE(bond_newport->ifname);
    bond_newport->ifname = NULL;
fail1:
    GLUE_FREE(bond_newport);
    bond_newport = NULL;
#endif
    return -1;
}


void recv_on_bond(void *ctx, unsigned int local_port, const u8 *buf, u16 len)
{
    #if 0
    struct port *port;
    

    /* Find the originating slave port object */
    for (port = porthead; port != NULL && port->l2->local_port != (int)local_port;
        port = port->next)
        ;

    if (port)
        rxReceiveFrame(local_port, buf, len);
    #endif
}


int remove_bond_port(const char *ifname)
{
    #if 0
    struct lldp_port *bond_port = bond_porthead;
    struct lldp_port *bond_parent = NULL;

    while (bond_port != NULL) {
        if (!strncmp(ifname, bond_port->ifname, MAX_DEVICE_NAME_LEN)) {
            LLDPAD_DBG("In remove_bond_port: Found bond port  %s\n",
                bond_port->ifname);
            break;
        }
        bond_parent = bond_port;
        bond_port = bond_port->next;
    }

    if (bond_port == NULL)
        return -1;

    l2_packet_deinit(bond_port->l2);

    if (bond_parent == NULL)
        bond_porthead = bond_port->next;
    else if (bond_parent->next == bond_port)     /* sanity check */
        bond_parent->next = bond_port->next;    
    else
        return -1;

    if (bond_port->ifname)
        GLUE_FREE(bond_port->ifname);
    
    GLUE_FREE(bond_port);
    #endif
    return 0;
}

void remove_all_bond_ports(void)
{
    #if 0
    struct port *bond_port = bond_porthead;
    struct port *next_bond_port = bond_porthead;

    while (bond_port != NULL) {
        next_bond_port = bond_port->next;

        l2_packet_deinit(bond_port->l2);
        if (bond_port->ifname)
            GLUE_FREE(bond_port->ifname);
        GLUE_FREE(bond_port);

        bond_port = next_bond_port;
    }
    #endif
}

#ifdef __cplusplus
}
#endif


