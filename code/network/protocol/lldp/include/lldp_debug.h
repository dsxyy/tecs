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

#ifndef _LLDP_DEBUG_H
#define _LLDP_DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lldp_in.h"

void lldp_set_print(u8 flag);
void lldp_show_rx_sm(u32 local_port);
void lldp_show_tx_sm(u32 local_port);
void lldp_show_stt(u32 local_port);
void lldp_show_porttimer_sm(u32 local_port);
void lldp_show_info_one(u32 local_port);
void lldp_show_info(u32 local_port);
void lldp_show_port(void);
void set_lldp_mode(u32 local_port, u8 admin);
void set_dcbx_enable(u32 local_port, bool val);
void set_qcn_enable(u32 local_port, bool val);
void lldp_show_ets(u32 local_port);
void lldp_show_pfc(u32 local_port);
void lldp_show_qcn(u32 local_port);
void lldp_show_lossless(u32 local_port);
void set_ets_willing(u32 local_port,u8 willing);
void set_pfc_mode(u32 local_port, u8 mode);
void set_pfc(u32 local_port, u8 priority, u8 value);
void set_pfc_cap(u32 local_port, u8 tc_num);
void set_mbc(u32 local_port, u8 value);
void create_tc(u32 local_port, u8 tc, u8 tc_type);
void delete_tc(u32 local_port, u8 tc, u8 tc_type);
void join_pri_to_tc(u32 local_port, u8 tc, u8 pri, u8 tc_type);
void quit_pri_from_tc(u32 local_port, u8 tc, u8 pri, u8 tc_type);
int find_tc_by_pri(u32 local_port, u8 pri, u8 tc_type);
void set_tc_tsa(u32 local_port, u8 tc, u8 tsa, u8 tc_type);
int is_strict_tc(u32 local_port, u8 tc, u8 tc_type);
int find_strict_tc(u32 local_port, u8 tc_type);
void set_tc_bw(u32 local_port, u8 tc, u8 bw, u8 tc_type);
int get_sumbw_of_tcs(u32 local_port, u8 tc_type);
void set_cnpv(u32 local_port, u8 pri, u8 status);
void set_cnpv_mode(u32 local_port, u8 pri, u8 mode);
void set_lldp_agent_mode(u32 local_port, u8 agent_mode);
void lldp_help(void);

extern u8 g_print;


#ifdef __cplusplus
}
#endif


#endif /* _LLDP_DEBUG_H */
