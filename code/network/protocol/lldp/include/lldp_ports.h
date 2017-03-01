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

#ifndef PORTS_H
#define PORTS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lldp_in.h"

#ifndef ETH_ALEN
#define ETH_ALEN    6
#endif
#ifndef IFNAMSIZ
#define IFNAMSIZ    31  /* must match MAX_DEVICE_NAME_LEN */
#endif
#ifndef ETH_P_ALL
#define ETH_P_ALL   0x0003
#endif

#ifndef BIT
#define BIT(x) (1 << (x))
#endif

#define MAX_INTERFACES          16

#define DEFAULT_TX_HOLD         4
#define DEFAULT_TX_INTERVAL     30
#define FAST_TX_INTERVAL        1
#define TX_FAST_INIT            4
#define DEFAULT_TX_DELAY        1
#define FASTSTART_TX_DELAY      1
#define REINIT_DELAY            2
#define TX_CREDIT_MAX           5

#define DORMANT_DELAY   15

struct porttimers {
    u16 dormantDelay;
/* Tx */
    u16 state;
    u16 reinitDelay;
    u16 msgTxHold;
    u16 msgTxInterval;
    u16 msgFastTx;
    u16 txFastInit;
    u16 txTTR;
    u16 txShutdownWhile;
    u16 txCredit;
    u16 txMaxCredit;
    bool txTick;
/* Rx */
    u16 tooManyNghbrsTimer;
    u16 rxTTL;
    u16 lastrxTTL;  /* cache last received */
};

struct porttx {
    u8 *frameout;
    u32 sizeout;
    u8 state;
    u8 localChange;
    u16 txTTL;
    bool txNow;
    u16 txFast;
};

struct portstats {
/* Tx */
    u32 statsFramesOutTotal;
    u32 statsFramesOutErrorsTotal;
/* Rx */
    u32 statsAgeoutsTotal;
    u32 statsFramesDiscardedTotal;
    u32 statsFramesInErrorsTotal;
    u32 statsFramesInTotal;
    u32 statsTLVsDiscardedTotal;
    u32 statsTLVsUnrecognizedTotal;
};

typedef struct rxmanifest{
    struct unpacked_tlv *chassis;
    struct unpacked_tlv *portid;
    struct unpacked_tlv *ttl;
    struct unpacked_tlv *portdesc;
    struct unpacked_tlv *sysname;
    struct unpacked_tlv *sysdesc;
    struct unpacked_tlv *syscap;
    struct unpacked_tlv *mgmtadd;
}rxmanifest;

struct portrx {
    u8 *framein;
    u16 sizein;
    u8 state;
    u8 badFrame;
    u8 rcvFrame;
    u8 rxInfoAge;
    u8 remoteChange;
    u8 tooManyNghbrs;
    u8 dupTlvs;
    u8 dcbx_st;
    bool newNeighbor;
    rxmanifest *manifest;
};

struct lldp_eth_hdr {
    char dst[6];
    char src[6];
    u16 ethertype;
};

enum portAdminStatus {
    disabled,
    enabledTxOnly,
    enabledRxOnly,
    enabledRxTx,
};

struct extlvtype {
    bool port_des;
    bool sys_name;
    bool sys_des;
    bool sys_cap;
    bool man_addr;
    bool etscfg;
    bool etsrec;
    bool pfc;
    bool app;
    bool qcn;
    bool evbtlv;
    bool cdcp;   
};

struct lldp_port {
    u32 local_port;
    u8  name[IFNAMSIZ+1];
    u8  dmac[6];
    u8  smac[6];
    u8  agent_mode;
    u8  hw_resetting;
    u8  portEnabled;
    u8  prevPortEnabled;
    u8  adminStatus;
    u8  pad1[3];
    
    /* protocol specific */
    struct l2_packet_data *l2;
    struct portrx rx;
    struct porttx tx;
    struct portstats stats;
    struct porttimers timers;
    u8     rxChanges;
    u8     pad2[1];
    u16    lldpdu;
    struct msap msap;
    struct extlvtype extlv;
    struct lldp_port *next;
};

extern struct lldp_port *porthead;
extern struct lldp_port *portcurrent;
extern struct lldp_port *porttail;

int create_lldp_port(u32 local_port, u8 *name, u8 *mac, u8 agent_mode);
int destroy_lldp_port(u32 local_port);
void lldp_port_up(u32 local_port);
void lldp_port_down(u32 local_port);
void lldp_init_ports(void);
void lldp_deinit_ports(void);
int set_port_hw_resetting(u32 local_port, int resetting);
int get_port_hw_resetting(u32 local_port);
void set_lldp_port_enable_state(u32 local_port, u8 enable);
void set_lldp_port_admin(u32 local_port, u8 enable);
int get_lldp_port_admin(u32 local_port);

int get_lldp_port_statistics(u32 local_port, struct portstats *stats);

int get_local_tlvs(u32 local_port, unsigned char *tlvs, int *size);
int get_neighbor_tlvs(u32 local_port, unsigned char *tlvs, int *size);

int port_needs_shutdown(struct lldp_port *port);

void set_port_operstate(u32 local_port, int operstate);
int get_port_operstate(u32 local_port);

int lldp_reinit_port(u32 local_port);
void set_port_oper_delay(u32 local_port);

struct lldp_port *find_by_port(u32 local_port);
int is_support_lldp(u32 local_port);
const char* get_lldp_port_admin_string(u8 ucAdminStatus);


#ifdef __cplusplus
}
#endif


#endif /* PORTS_H */


