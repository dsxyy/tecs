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

#ifndef _LLDP_MOD_H
#define _LLDP_MOD_H

/*
#include <sys/queue.h>
#include <sys/un.h>
#include "lldp_util.h"
#include "ctrl_iface.h"
*/

#ifdef __cplusplus
extern "C" {
#endif

#include "lldp_in.h"

/* 
 * Ops structure for lldp module callbacks.
 *
 * @lldp_mod_register: return lldp_module struct with tlv or out+subtype 
 *             match types
 * @lldp_mod_unregister: cleanup
 * @lldp_mod_gettlv: return packed_tlv for core to append and xmit,
 *           module is responsible for sanity checks the core
 *           will only verify length.
 * @lldp_mod_rchange: core recv function passing changed tlv to module
 * @lldp_mod_utlv: update tlv called before each xmit
 * @lldp_mod_ifup: notification of rtnetlink LINK_UP event 
 * @lldp_mod_ifdown: notification of rtnetlink LINK_DOWN event 
 * @lldp_mod_recvrt: core passes raw rtnetlink messages 
 * @client_register: any setup required for client interface
 * @client_cmd: process client commands from core lldp
 * @print_tlv: routine for client to pretty print TLVs
 * @lookup_tlv_name: find tlvid given a tlv 'name'
 * @get_arg_handler: return an arg handler list
 */
struct lldp_mod_ops {
    struct lldp_module *    (* lldp_mod_register)(void);
    void                    (* lldp_mod_unregister)(struct lldp_module *);
    struct packed_tlv *     (* lldp_mod_gettlv)(struct lldp_port *);
    int                     (* lldp_mod_rchange)(struct lldp_port *, struct unpacked_tlv *, bool *);
    void                    (* lldp_mod_utlv)(struct lldp_port *);
    void                    (* lldp_mod_ifup)(u32); 
    void                    (* lldp_mod_ifdown)(u32);
    void                    (* lldp_mod_ifadd)(u32);
    void                    (* lldp_mod_ifdel)(u32);
    u8                      (* lldp_mod_mibdelete)(struct lldp_port *);
    u32                     (* client_register)(void);
    int                     (* client_cmd)(void *, void *, socklen_t, char *, int, char *, int);
    int                     (* print_tlv)(u32, u16, char *);
    u32                     (* lookup_tlv_name)(char *);
    int                     (* print_help)(void);
    int                     (* timer)(struct lldp_port *);
    struct arg_handlers *   (* get_arg_handler)(void);
};

/*
 *  The lldp module structure
 *
 *  lldp module per instance structure.  Used by lldp core to 
 *  track available modules.  Expect lldp core to create link
 *  list of modules types per port.
 *
 */
struct lldp_module {
    int     id;            /* match tlv or oui+subtype */
    u8      enable;        /* TX only, RX only, TX+RX, Disabled */
    void    *data;         /* module specific data */
    const   struct lldp_mod_ops *ops;
    LIST_ENTRY(lldp_module) lldp;
};

LIST_HEAD(lldp_head, lldp_module);

typedef struct _lldp {
    struct lldp_head mod_head;
    void ( *pfPortInit)(void);
    void ( *pfTmInit)(void);
    void ( *pfTmSet)(void);
    void ( *pfTmStop)(void);
    void ( *pfTmout)(void);    
    int  ( *pfSndPkt)(u32, u8*, u16);
    void ( *pfRcvPkt)(u32, u8*, u16);
}T_LLDP;

extern T_LLDP gtLLDP;
extern struct lldp_module *find_module_by_id(struct lldp_head *head, int id);
extern void *find_module_user_data_by_id(struct lldp_head *head, int id);
extern void lldp_mod_register(struct lldp_module *(pf)(void));
extern void lldp_mod_unregister(int dwModId);
extern void lldp_all_mod_unregister(void);
extern T_LLDP *lldp_init(void (*pfPortInit)(void),
                         void (*pfTmInit)(void),
                         void (*pfTmSet)(void),
                         void (*pfTmStop)(void),
                         int  (*pfSndPkt)(u32, u8*, u16));
extern void lldp_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* _LLDP_MOD_H */
