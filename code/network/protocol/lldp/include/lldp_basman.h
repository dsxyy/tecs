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

#ifndef _LLDP_BASMAN_H
#define _LLDP_BASMAN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lldp_in.h"

#define LLDP_MOD_BASIC  2
#define SYSNAME_DEFAULT "ZTE Corpration, FCF Switch 2011.09.29"
#define IFNUM_SUBTYPE_UNKNOWN 1
#define IFNUM_SUBTYPE_IFINDEX 2
#define IFNUM_SUBTYPE_PORTNUM 3

struct tlv_info_maddr {
    u8 len;
    u8 sub;
    union {
        u8 in[4];
        u8 in6[4];
        u8 mac[6];
        u8 addr[31];
    }a;
};

struct tlv_info_maif {
    u8 sub;
    u32 num;
};

struct tlv_info_maoid {
    u8 len;
    u8 oid[128];
};

struct tlv_info_manaddr {
    struct tlv_info_maddr m;
    struct tlv_info_maif i;
    struct tlv_info_maoid o;
};

struct basman_data {
    u32 local_port;
    struct unpacked_tlv *portdesc;
    struct unpacked_tlv *sysname;
    struct unpacked_tlv *sysdesc;
    struct unpacked_tlv *syscaps;
    struct unpacked_tlv *manaddr[MANADDR_MAX];
    int macnt;
    LIST_ENTRY(basman_data) entry;
};

typedef enum {
    INET = 0,
    INET6,
    UNSPEC,
}nettype;

struct basman_user_data {
    LIST_HEAD(basman_head, basman_data) head;
};

struct lldp_module *basman_register(void);
void basman_unregister(struct lldp_module *mod);
struct packed_tlv *basman_gettlv(struct lldp_port *port);
void basman_ifdown(u32 local_port);
void basman_ifup(u32 local_port);

#ifdef __cplusplus
}
#endif

#endif /* _LLDP_BASMAN_H */
