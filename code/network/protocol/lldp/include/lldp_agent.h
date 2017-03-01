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

#ifndef AGENT_H
#define AGENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lldp_in.h"

enum agentmode
{
    NearestBridge  = 0,
    NearestNonTPMRBridge,
    NearestCustomerBridge
};

void start_lldp_agent(void);
void stop_lldp_agent(void);
void clean_lldp_agent(void);
void lldp_agent_mode(u32 local_port, u8 agent_mode);
void lldp_timeout(void);

#ifdef V4_VERSION
#include "../../storage/fcoe/include/fcoe.h"

void v4_lldp_port_init(void);
void v4_lldp_tm_init(void);
void v4_lldp_tm_set(void);
void v4_lldp_tm_stop(void);
int v4_lldp_send_pkt(u32 local_port, u8 *pkt, u16 len);
void v4_lldp_timeout(struct timer *ptTm, void *cookie);
void lldp_port_status_change(T_PORT_STATUS *ptPortStatus);
void lldp_rcv(packet *p);


#define HBA_82599_LLDP_PORT_ID  2
void hba_82599_lldp_port_init(void);
int  hba_82599_lldp_send_pkt(u32 local_port, u8 *pkt, u16 len);
void hba_82599_lldp_rcv (FCOE_PACKET *p);
void hba_82599_lldp_init();

extern void fcoe_rawsocket_send(BYTE* buffer, UINT16 len);
#endif

#ifdef SWITCH_VERSION
#include "iss_switch.h"

u32 switch_slot2port(u32 dwSlot);
u32 switch_port2slot(u32 dwPort);
u32 switch_lldp_port_up(void *pMsgPara);
u32 switch_lldp_port_down(void *pMsgPara);
void switch_lldp_port_init(void);
void switch_lldp_tm_set(void);
void switch_lldp_tm_stop(void);
int switch_lldp_send_pkt(u32 local_port, u8 *pkt, u16 len);

extern T_ISSVTIO g_atIssVtIo[ISS_ALL];
extern SWORD32 issio_port2logic(WORD32 dwPort, T_LOGIC_PORT *ptLogicPort);

#endif

#ifdef __cplusplus
}
#endif


#endif /* AGENT_H */

