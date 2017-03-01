/*******************************************************************************

  LLDP Agent Daemon (LLDPAD) Software
  Copyright(c) 2007-2011 Intel Corporation.

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

#ifndef _LLDP_8021QAU_H
#define _LLDP_8021QAU_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lldp_in.h"

#define IEEE8021QAU_QCN_TLV 8
#define LLDP_MOD_8021QAU    ((OUI_IEEE_8021 << 8) | IEEE8021QAU_QCN_TLV)

/* Received TLV types */
#define RCVD_IEEE8021QAU_TLV_QCN    0x0016

/* Duplicate TLV types */
#define DUP_IEEE8021QAU_TLV_QCN     0x0016

#pragma pack(1)
/* Packed TLVs */
struct ieee8021qau_cntlv {
    u8 oui[OUI_SIZE];
    u8 subtype;
    u8 perPriCNPV;
    u8 perPriReady;
};
#pragma pack()

enum  qcn_mode {
    QCN_ECP        = 0,
    QCN_ICP        = 1,
    QCN_RP         = 2,
    QCN_AUTO       = 3,
};

typedef struct qcn_cnpv {
    u8 used;
    u8 mode;
    u8 ready;
    u8 pad[1];
}CNPV;

struct qcn_obj {
    CNPV cnpv[MAX_USER_PRIORITIES];
};

struct qcn_attrib {
    struct qcn_obj local;
    struct qcn_obj cur;
    struct qcn_obj remote;
};

struct ieee8021qau_data {
    bool active;
    bool pending;    
    u16 ieee8021qaudu;
    u32 local_port;    
    struct ieee8021qau_unpkd_tlvs *rx;
    struct qcn_attrib *qcn;
    LIST_ENTRY(ieee8021qau_data) entry;
};

struct ieee8021qau_unpkd_tlvs {
    struct unpacked_tlv *qcn;
};

struct ieee8021qau_user_data {
    LIST_HEAD(ieee8021qau_head,ieee8021qau_data) head;
};

typedef struct _QCN 
{  
    int  ( *pfSetHwQcnEnable)(u32, u8, u8);
    int  ( *pfSetHwQcnMode)(u32, u8, u8);
}T_QCN;

struct lldp_module *ieee8021qau_register(void);
void ieee8021qau_unregister(struct lldp_module *mod);
u8 ieee8021qau_mibDeleteObject(struct lldp_port *port);
struct packed_tlv *ieee8021qau_gettlv(struct lldp_port *port);
struct ieee8021qau_data *ieee8021qau_data(u32 local_port);
void set_default_qcn_cfg(struct ieee8021qau_data *data);
void ieee8021qau_free_rx(struct ieee8021qau_unpkd_tlvs *rx);
void clear_ieee8021qau_rx(struct ieee8021qau_data *data);
int ieee8021qau_rchange(struct lldp_port *port, struct unpacked_tlv *tlv, bool *tlv_store);
void ieee8021qau_mibUpdateObjects(struct lldp_port *port);
void process_ieee8021qau_qcn_tlv(struct lldp_port *port);
void qcn_sm(struct ieee8021qau_data *data);
void run_qcn_sm(struct lldp_port *port);
struct unpacked_tlv *bld_ieee8021qau_qcn_tlv(struct ieee8021qau_data *data);
void ieee8021qau_ifdown(u32 local_port);
void ieee8021qau_ifup(u32 local_port);
void ieee8021qau_ifadd(u32 local_port);
void ieee8021qau_ifdel(u32 local_port);
const char* get_cnpv_mode_string(u8 mode);
int ieee8021qau_check_active(u32 local_port);
int ieee8021qau_tlvs_rxed(u32 local_port);
void ieee8021qau_free_tlv(struct ieee8021qau_data *data);
int ieee8021qau_check_pending(struct lldp_port *port);
int get_qcn_tlv_info(u32 local_port, u8 *perPriCNPV, u8 *perPriReady);
int is_cp_mode(u32 local_port);
void init_hw_qau_interface(
    int  ( *pfSetHwQcnEnable)(u32, u8, u8),
    int  ( *pfSetHwQcnMode)(u32, u8, u8)
    );

#ifdef __cplusplus
}
#endif

#endif  /* _LLDP_8021QAU_H */


