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

#ifndef _LLDP_8021QAZ_H
#define _LLDP_8021QAZ_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/lldp_in.h"

#define LLDP_MOD_8021QAZ    ((OUI_IEEE_8021 << 8) | IEEE8021QAZ_ETSCFG_TLV)

#define MAX_USER_PRIORITIES 8
#define MAX_TCS         8
#define BW_PERCENT      100


/* IEEE 802.1Qaz std supported values */
#define IEEE_8021QAZ_MAX_TCS    8

#define IEEE_8021QAZ_TSA_STRICT     0
#define IEEE_8021QAZ_TSA_CB_SHAPER  1
#define IEEE_8021QAZ_TSA_ETS        2
#define IEEE_8021QAZ_TSA_VENDOR     255

/* maximum number of application entries allowed to be
 * configured in an application TLV.
 */
#define MAX_APP_ENTRIES     32

#define DEFAULT_SUBTYPE     0

/* IEEE8021QAZ TLV Definitions */
#define IEEE8021QAZ_ETSCFG_TLV  9
#define IEEE8021QAZ_ETSREC_TLV  10
#define IEEE8021QAZ_PFC_TLV 11
#define IEEE8021QAZ_APP_TLV 12

#define IEEE8021QAZ_SETTING "ieee8021qaz"
#define TLV_HEADER_LENGTH   2

/* Received TLV types */
#define RCVD_IEEE8021QAZ_TLV_ETSCFG 0x0001
#define RCVD_IEEE8021QAZ_TLV_ETSREC 0x0002
#define RCVD_IEEE8021QAZ_TLV_PFC    0x0004
#define RCVD_IEEE8021QAZ_TLV_APP    0x0008
#define RCVD_LLDP_IEEE8021QAZ_TLV   0x0200

/* Duplicate TLV types */
#define DUP_IEEE8021QAZ_TLV_ETSCFG  0x0001
#define DUP_IEEE8021QAZ_TLV_ETSREC  0x0002
#define DUP_IEEE8021QAZ_TLV_PFC     0x0004
#define DUP_IEEE8021QAZ_TLV_APP     0x0008

/* Transmission selection algorithm identifiers */
#define IEEE8021Q_TSA_STRICT    0x0
#define IEEE8021Q_TSA_CBSHAPER  0x1
#define IEEE8021Q_TSA_ETS   0x2
#define IEEE8021Q_TSA_VENDOR    0xFF

/* Flags */
#define IEEE8021QAZ_SET_FLAGS(_FlagsVar, _BitsToSet)    \
            ((_FlagsVar) = (_FlagsVar) | (_BitsToSet))

#define IEEE8021QAZ_TEST_FLAGS(_FlagsVar, _Mask, _BitsToCheck)  \
            (((_FlagsVar) & (_Mask)) == (_BitsToCheck))

/* APP internal state */
#define IEEE_APP_SET 0
#define IEEE_APP_DEL 1
#define IEEE_APP_DONE 2

/* ETSCFG WCRT field's Shift values */
#define ETS_WILLING_SHIFT   7
#define ETS_CBS_SHIFT       6


/* This structure contains the IEEE 802.1Qaz ETS managed object
 *
 * @willing: willing bit in ETS configuration TLV
 * @ets_cap: indicates supported capacity of ets feature
 * @cbs: credit based shaper ets algorithm supported
 * @tc_tx_bw: tc tx bandwidth indexed by traffic class
 * @tc_rx_bw: tc rx bandwidth indexed by traffic class
 * @tc_tsa: TSA Assignment table, indexed by traffic class
 * @prio_tc: priority assignment table mapping 8021Qp to traffic class
 * @tc_reco_bw: recommended tc bandwidth indexed by traffic class for TLV
 * @tc_reco_tsa: recommended tc bandwidth indexed by traffic class for TLV
 * @reco_prio_tc: recommended tc tx bandwidth indexed by traffic class for TLV
 *
 * Recommended values are used to set fields in the ETS recommendation TLV
 * with hardware offloaded LLDP.
 *
 * ----
 *  TSA Assignment 8 bit identifiers
 *  0   strict priority
 *  1   credit-based shaper
 *  2   enhanced transmission selection
 *  3-254   reserved
 *  255 vendor specific
 */
struct ieee_ets {
    __u8    willing;
    __u8    ets_cap;
    __u8    cbs;
    __u8    tc_tx_bw[IEEE_8021QAZ_MAX_TCS];
    __u8    tc_rx_bw[IEEE_8021QAZ_MAX_TCS];
    __u8    tc_tsa[IEEE_8021QAZ_MAX_TCS];
    __u8    prio_tc[IEEE_8021QAZ_MAX_TCS];
    __u8    tc_reco_bw[IEEE_8021QAZ_MAX_TCS];
    __u8    tc_reco_tsa[IEEE_8021QAZ_MAX_TCS];
    __u8    reco_prio_tc[IEEE_8021QAZ_MAX_TCS];
};

/* This structure contains the IEEE 802.1Qaz PFC managed object
 *
 * @pfc_cap: Indicates the number of traffic classes on the local device
 *       that may simultaneously have PFC enabled.
 * @pfc_en: bitmap indicating pfc enabled traffic classes
 * @mbc: enable macsec bypass capability
 * @delay: the allowance made for a round-trip propagation delay of the
 *     link in bits.
 * @requests: count of the sent pfc frames
 * @indications: count of the received pfc frames
 */
struct ieee_pfc {
    __u8    pfc_cap;
    __u8    pfc_en;
    __u8    mbc;
    __u16   delay;
    __u64   requests[IEEE_8021QAZ_MAX_TCS];
    __u64   indications[IEEE_8021QAZ_MAX_TCS];
};

/* This structure contains the IEEE 802.1Qaz APP managed object. This
 * object is also used for the CEE std as well. There is no difference
 * between the objects.
 *
 * @selector: protocol identifier type
 * @protocol: protocol of type indicated
 * @priority: 3-bit unsigned integer indicating priority
 *
 * ----
 *  Selector field values
 *  0   Reserved
 *  1   Ethertype
 *  2   Well known port number over TCP or SCTP
 *  3   Well known port number over UDP or DCCP
 *  4   Well known port number over TCP, SCTP, UDP, or DCCP
 *  5-7 Reserved
 */
struct dcb_app {
    __u8    selector;
    __u8    priority;
    __u16   protocol;
};

enum  state {
    STATE_INIT,
    RX_RECOMMEND
};

enum  tc_type {
    CFG,
    REC
};

#pragma pack(1)
/* Packed TLVs */
struct ieee8021qaz_tlv_etscfg {
    u8 oui[OUI_SIZE];
    u8 subtype;
    u8 wcrt;        /* Willing-Cbs-Reserved-maxTc fields */
    u32 prio_map;       /* Priority Assignment Table */
    u8 tc_bw[MAX_TCS];  /* TC Bandwidth Table */
    u8 tsa_map[MAX_TCS];    /* Transmission Selection Algorithm Table */
};

struct ieee8021qaz_tlv_etsrec {
    u8 oui[OUI_SIZE];
    u8 subtype;
    u8 reserved;
    u32 prio_map;       /* Priority Assignment Table */
    u8 tc_bw[MAX_TCS];  /* TC Bandwidth Table */
    u8 tsa_map[MAX_TCS];    /* Transmission Selection Algorithm Table */
};

struct ieee8021qaz_tlv_pfc {
    u8 oui[OUI_SIZE];
    u8 subtype;
    u8 wmrc;        /* Willing-Mbc-Reserved-pfcCap fields */
    u8 pfc_enable;      /* PFC Enable */
};

struct ieee8021qaz_tlv_app {
    u8 oui[OUI_SIZE];
    u8 subtype;
    u8 reserved;
};

struct app_prio {
    u8 prs;         /* Priority-Reserved-Selection fields */
    u16 pid;
};
#pragma pack()


typedef struct _tc {
    u8 used;
    u8 pri;
    u8 bw;
    u8 tsa;
}TC;

/* ETS Configuration Object */
struct ets_obj {
    bool willing;
    bool cbs;
    u8   max_tcs;
    u8   pad[1];
    TC   tc_cfg[MAX_TCS];
    TC   tc_rec[MAX_TCS];
    TC   tc_cur[MAX_TCS];    
};

/* PFC Object */
struct pfc_obj {
    /* DCBX PFC Params */
    bool willing;
    bool mbc;
    u8 pfc_cap;
    u8 pfc_enable;
    u32 delay;
};

/* Application Objects */
struct app_obj {
    struct dcb_app app;
    bool peer;
    u8 pad[3];
    int hw;
    LIST_ENTRY(app_obj) entry;
};

typedef struct store_cfg
{
    struct ets_obj    ets;
    struct pfc_obj    pfc;
    struct app_obj    app;
}t_store_cfg;

/* @oper_param - 0: local_params, 1: remote_params
 * @remote_param - 0: NULL */
struct ets_attrib {
    u8 current_state;
    u8 pad[3];
    struct ets_obj *cfgl;  
    struct ets_obj *cfgr;
};

struct pfc_attrib {
    u8 current_state;
    bool remote_param;
    u8 pad[2];
    struct pfc_obj local;
    struct pfc_obj remote;
    struct pfc_obj cur;
};

struct ieee8021qaz_unpkd_tlvs {
    struct unpacked_tlv *ieee8021qaz;
    struct unpacked_tlv *etscfg;
    struct unpacked_tlv *etsrec;
    struct unpacked_tlv *pfc;
    struct unpacked_tlv *app;
};

/*gongxf add*/
LIST_HEAD(app_tlv_head, app_obj);

struct ieee8021qaz_tlvs {
    bool active;
    bool pending;
    u16 ieee8021qazdu;
    u8 local_mac[ETH_ALEN];
    u8 remote_mac[ETH_ALEN];
    u32 local_port;
    struct ieee8021qaz_unpkd_tlvs *rx;
    struct ets_attrib *ets;
    struct pfc_attrib *pfc;
    struct app_tlv_head app_head;
    struct lldp_port *port;
    LIST_ENTRY(ieee8021qaz_tlvs) entry;
};

struct ieee8021qaz_user_data {
    LIST_HEAD(ieee8021qaz_head, ieee8021qaz_tlvs) head;
};

typedef struct _ETS 
{  
    int  ( *pfSetHwTc)(u32, u8, u8, u8, u8);
    int  ( *pfSetHwPri2Tc)(u32, u8, u8, u8);
    int  ( *pfSetHwPfc)(u32, u8, u8, u8, u8);
}T_ETS;

int ieee8021qaz_check_pending(struct lldp_port *port);
void run_all_sm(struct lldp_port *port);
void ieee8021qaz_mibUpdateObjects(struct lldp_port *port);
void ieee8021qaz_app_reset(struct app_tlv_head *head);

int ieee8021qaz_mod_app(struct app_tlv_head *head, int peer, u8 prio, u8 sel, u16 proto, u32 ops);
int ieee8021qaz_app_sethw(u32 local_port, struct app_tlv_head *head);

u8 get_tc_by_pri(TC *ptc, u8 pri);
int get_priomap_by_tcs(TC *ptc);
int set_tc_pri(TC *ptc, u8 prio, int tc);
int unset_tc_pri(TC *ptc, u8 prio, int tc);

int get_prio_map(u32 prio_map, int tc);
void set_prio_map(u32 *prio_map, u8 prio, int tc);
const char* get_tsa_string(u8 tsa);

struct ieee8021qaz_tlvs *ieee8021qaz_data(u32 local_port);

int ieee8021qaz_tlvs_rxed(u32 local_port);
int ieee8021qaz_check_active(u32 local_port);
struct lldp_module *ieee8021qaz_register(void);
void ieee8021qaz_unregister(struct lldp_module *mod);
struct packed_tlv *ieee8021qaz_gettlv(struct lldp_port *port);
void clear_ieee8021qaz_rx(struct ieee8021qaz_tlvs *tlvs);
int ieee8021qaz_rchange(struct lldp_port *port, struct unpacked_tlv *tlv, bool *tlv_store);
void ieee8021qaz_ifup(u32 local_port);
void ieee8021qaz_ifdown(u32 local_port);
void ieee8021qaz_ifadd(u32 local_port);
void ieee8021qaz_ifdel(u32 local_port);
void ieee8021qaz_free_rx(struct ieee8021qaz_unpkd_tlvs *rx);
void ieee8021qaz_free_tlv(struct ieee8021qaz_tlvs *tlvs);
u8 ieee8021qaz_mibDeleteObject(struct lldp_port *port);
/*int ieee8021qaz_clif_cmd(void *data, struct sockaddr_un *from, socklen_t fromlen, char *ibuf, int ilen, char *rbuf);*/
int ieee8021qaz_check_operstate(void);
void init_hw_qaz_interface(
    int  ( *pfSetHwTc)(u32, u8, u8, u8, u8),
    int  ( *pfSetHwPri2Tc)(u32, u8, u8, u8),
    int  ( *pfSetHwPfc)(u32, u8, u8, u8, u8)
    );

#ifdef __cplusplus
}
#endif

#endif  /* _LLDP_8021QAZ_H */
