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

#ifndef L2_PACKET_H
#define L2_PACKET_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lldp_in.h"

#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"

#define IP2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define IPSTR "%02x:%02x:%02x:%02x:%02x:%02x"

#define ETH_P_LLDP 0x88cc

/* TODO: use extended ethertype until final ethertype is available */
#define ETH_P_ECP 0x88b7

#define ETH_FRAME_LEN   1514

#ifndef ETH_MAX_DATA_LEN
#define ETH_MAX_DATA_LEN    1500
#endif

/* 6 source + 6 dest + 2 type */
#ifndef ETH_HDR_LEN
#define ETH_HDR_LEN         14
#endif

#ifndef ETH_MIN_DATA_LEN
#define ETH_MIN_DATA_LEN    46
#endif

#ifndef ETH_MIN_PKT_LEN
#define ETH_MIN_PKT_LEN     (ETH_MIN_DATA_LEN + ETH_HDR_LEN)
#endif

#define LLDP_MULTICAST_MAC 0x0180c200000e


static const u8 multi_cast_mac1[ETH_ALEN] = {0x01,0x80,0xc2,0x00,0x00,0x0e};
static const u8 multi_cast_mac2[ETH_ALEN] = {0x01,0x80,0xc2,0x00,0x00,0x03};
static const u8 multi_cast_mac3[ETH_ALEN] = {0x01,0x80,0xc2,0x00,0x00,0x00};

/**
 * struct l2_packet_data - Internal l2_packet data structure
 *
 * This structure is used by the l2_packet implementation to store its private
 * data. Other files use a pointer to this data when calling the l2_packet
 * functions, but the contents of this structure should not be used directly
 * outside l2_packet implementation.
 */
struct l2_packet_data;


typedef struct l2_ethhdr {
    u8 h_dest[ETH_ALEN];
    u8 h_source[ETH_ALEN];
    u16 h_proto;
} STRUCT_PACKED;

struct l2_packet_data {
    u32 local_port;
    u8  perm_mac_addr[ETH_ALEN];
    u8  curr_mac_addr[ETH_ALEN];
    u8  san_mac_addr[ETH_ALEN];
    u8  remote_mac_addr[ETH_ALEN];
    int l2_hdr; /* whether to include layer 2 (Ethernet) header data * buffers */
};

/**
 * l2_packet_init - Initialize l2_packet interface
 * @ifname: Interface name
 * @own_addr: Optional own MAC address if available from driver interface or
 *  %NULL if not available
 * @protocol: Ethernet protocol number in host byte order
 * @rx_callback: Callback function that will be called for each received packet
 * @rx_callback_ctx: Callback data (ctx) for calls to rx_callback()
 * @l2_hdr: 1 = include layer 2 header, 0 = do not include header
 * Returns: Pointer to internal data or %NULL on failure
 *
 * rx_callback function will be called with ifindex pointing to the ifindex
 * of the receive interface.  If l2_hdr is set to 0, buf
 * points to len bytes of the payload after the layer 2 header and similarly,
 * TX buffers start with payload. This behavior can be changed by setting
 * l2_hdr=1 to include the layer 2 header in the data buffer.
 */
struct l2_packet_data * l2_packet_init(u32 local_port, const u8 *own_addr, unsigned short protocol, int l2_hdr);

/**
 * l2_packet_deinit - Deinitialize l2_packet interface
 * @l2: Pointer to internal l2_packet data from l2_packet_init()
 */
void l2_packet_deinit(struct l2_packet_data *l2);

/**
 * l2_packet_get_own_src_addr - Get own src layer 2 address
 * Checks to see if the port is part of a bond and makes and
 * appropriate selection for the layer 2 src address to use.
 * @l2: Pointer to internal l2_packet data from l2_packet_init()
 * @addr: Buffer for the own address (6 bytes)
 * Returns: 0 on success, -1 on failure
 */
int l2_packet_get_own_src_addr(struct l2_packet_data *l2, u8 *addr);

/**
 * l2_packet_get_own_addr - Get own layer 2 address
 * @l2: Pointer to internal l2_packet data from l2_packet_init()
 * @addr: Buffer for the own address (6 bytes)
 * Returns: 0 on success, -1 on failure
 */
int l2_packet_get_own_addr(struct l2_packet_data *l2, u8 *addr);

void get_remote_peer_mac_addr(struct lldp_port *port);
void l2_packet_get_remote_addr(struct l2_packet_data *l2, u8 *addr);
void l2_packet_clear_remote_addr(struct l2_packet_data *l2);

/**
 * l2_packet_send - Send a packet
 * @l2: Pointer to internal l2_packet data from l2_packet_init()
 * @dst_addr: Destination address for the packet (only used if l2_hdr == 0)
 * @proto: Protocol/ethertype for the packet in host byte order (only used if
 * l2_hdr == 0)
 * @buf: Packet contents to be sent; including layer 2 header if l2_hdr was
 * set to 1 in l2_packet_init() call. Otherwise, only the payload of the packet
 * is included.
 * @len: Length of the buffer (including l2 header only if l2_hdr == 1)
 * Returns: >=0 on success, <0 on failure
 */
int l2_packet_send(struct l2_packet_data *l2, const u8 *dst_addr, u16 proto, const u8 *buf, u16 len);

void l2_packet_get_port_state(struct l2_packet_data *, u8 *);

int add_bond_port(const char *ifname);
int remove_bond_port(const char *ifname);
void recv_on_bond(void *ctx, unsigned int local_port, const u8 *buf, u16 len);
void remove_all_bond_ports(void);

#ifdef __cplusplus
}
#endif


#endif /* L2_PACKET_H */
