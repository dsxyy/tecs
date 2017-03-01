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

void somethingChangedLocal(u32 local_port)
{
    struct lldp_port *port;

    port = find_by_port(local_port);
    if (!port)
    {
        return;
    }

    port->tx.localChange = 1;

    port->tx.txFast = port->timers.txFastInit;

    return;
}

int tlv_ok(struct unpacked_tlv *tlv)
{
    if (!tlv || (!tlv->length && tlv->type))
        return 0;
    else
        return 1;
}

struct packed_tlv *pack_tlv(struct unpacked_tlv *tlv)
{
    u16 tl = 0;
    struct packed_tlv *pkd_tlv = NULL;

    if (!tlv_ok(tlv))
        return NULL;

    tl = tlv->type;
    tl = tl << 9;
    tl |= tlv->length & 0x01ff;
    tl = EVB_HTONS(tl);

    pkd_tlv = (struct packed_tlv *)GLUE_ALLOC(sizeof(struct packed_tlv));
    if(!pkd_tlv) {
        LLDPAD_DBG("pack_tlv: Failed to GLUE_ALLOC pkd_tlv\n");
        return NULL;
    }
    memset(pkd_tlv,0,sizeof(struct packed_tlv));
    pkd_tlv->size = tlv->length + sizeof(tl);
    pkd_tlv->tlv = (u8 *)GLUE_ALLOC(pkd_tlv->size);
    if(pkd_tlv->tlv) {
        memset(pkd_tlv->tlv,0, pkd_tlv->size);
        memcpy(pkd_tlv->tlv, &tl, sizeof(tl));
        if (tlv->length)
            memcpy(&pkd_tlv->tlv[sizeof(tl)], tlv->info,
                tlv->length);
    } else {
        LLDPAD_DBG("pack_tlv: Failed to GLUE_ALLOC tlv\n");
        GLUE_FREE(pkd_tlv);
        pkd_tlv = NULL;
        return NULL;
    }
    return pkd_tlv;
}

/*
 * pack the input tlv and put it at the end of the already packed tlv mtlv
 * update the total size of the out put mtlv
 */
int pack_tlv_after(struct unpacked_tlv *tlv, struct packed_tlv *mtlv, int length)
{
    struct packed_tlv *ptlv;

    if (!tlv || !mtlv)
        return 0;  /* no TLV is ok */

    if (!tlv_ok(tlv))
        return -1;

    ptlv =  pack_tlv(tlv);
    if (!ptlv)
        return -1;

    if (ptlv->size + mtlv->size > length) {
        ptlv = free_pkd_tlv(ptlv);
        return -1;
    }

    memcpy(&mtlv->tlv[mtlv->size], ptlv->tlv, ptlv->size);
    mtlv->size += ptlv->size;
    ptlv = free_pkd_tlv(ptlv);
    return 0;
}


struct unpacked_tlv *unpack_tlv(struct packed_tlv *tlv)
{
    u16 tl = 0;
    struct unpacked_tlv *upkd_tlv = NULL;

    if(!tlv) {
        return NULL;
    }

    memcpy(&tl,tlv->tlv, sizeof(tl));
    tl = EVB_NTOHS(tl);

    upkd_tlv = (struct unpacked_tlv *)GLUE_ALLOC(sizeof(struct unpacked_tlv));
    if(upkd_tlv) {
        memset(upkd_tlv,0, sizeof(struct unpacked_tlv));
        upkd_tlv->length = tl & 0x01ff;
        upkd_tlv->info = (u8 *)GLUE_ALLOC(upkd_tlv->length);
        if(upkd_tlv->info) {
            memset(upkd_tlv->info,0, upkd_tlv->length);
            tl = tl >> 9;
            upkd_tlv->type = (u8)tl;
            memcpy(upkd_tlv->info, &tlv->tlv[sizeof(tl)],
                upkd_tlv->length);
        } else {
            LLDPAD_DBG("unpack_tlv: Failed to GLUE_ALLOC info\n");
            free (upkd_tlv);
            return NULL;
        }
    } else {
        LLDPAD_DBG("unpack_tlv: Failed to GLUE_ALLOC upkd_tlv\n");
        return NULL;
    }
    return upkd_tlv;
}

struct unpacked_tlv *free_unpkd_tlv(struct unpacked_tlv *tlv)
{
    if (tlv != NULL) {
        if (tlv->info != NULL) {
            GLUE_FREE(tlv->info);
            tlv->info = NULL;
        }
        GLUE_FREE(tlv);
        tlv = NULL;
    }
    return NULL;
}

struct packed_tlv *free_pkd_tlv(struct packed_tlv *tlv)
{
    if (tlv != NULL) {
        if (tlv->tlv != NULL) {
            GLUE_FREE(tlv->tlv);
            tlv->tlv = NULL;
        }
        GLUE_FREE(tlv);
        tlv = NULL;
    }
    return NULL;
}

struct packed_tlv *create_ptlv()
{
    struct packed_tlv *ptlv =
        (struct packed_tlv *)GLUE_ALLOC(sizeof(struct packed_tlv));

    if(ptlv)
        memset(ptlv, 0, sizeof(struct packed_tlv));
    return ptlv;
}

struct unpacked_tlv *create_tlv()
{
    struct unpacked_tlv *tlv =
        (struct unpacked_tlv *)GLUE_ALLOC(sizeof(struct unpacked_tlv));

    if(tlv) {
        memset(tlv,0, sizeof(struct unpacked_tlv));
        return tlv;
    } else {
        LLDPAD_DBG("create_tlv: Failed to GLUE_ALLOC tlv\n");
        return NULL;
    }
}

struct unpacked_tlv *bld_end_tlv()
{
    struct unpacked_tlv *tlv = create_tlv();

    if(tlv) {
        tlv->type = END_OF_LLDPDU_TLV;
        tlv->length = 0;
        tlv->info = NULL;
    }
    return tlv;
}

struct packed_tlv *pack_end_tlv()
{
    struct unpacked_tlv *tlv;
    struct packed_tlv *ptlv;

    tlv = bld_end_tlv();
    ptlv = pack_tlv(tlv);
    GLUE_FREE(tlv);
    return ptlv;
}

bool *find_txtlv_support(u32 local_port, u32 tlvid)
{
    bool *penabletx = NULL;

    struct lldp_port *port;

    port = find_by_port(local_port);
    if (!port)
    {
        return NULL;
    }
    
    switch (tlvid)
    {
        case PORT_DESCRIPTION_TLV:
            penabletx = &port->extlv.port_des;
            break;            
        case SYSTEM_NAME_TLV:
            penabletx = &port->extlv.sys_name;
            break;
        case SYSTEM_DESCRIPTION_TLV:
            penabletx = &port->extlv.sys_des;
            break;
            
        case SYSTEM_CAPABILITIES_TLV:
            penabletx = &port->extlv.sys_cap;
            break;
            
        case MANAGEMENT_ADDRESS_TLV:
            penabletx = &port->extlv.man_addr;
            break;

        case TLVID_8021(LLDP_8021QAZ_ETSCFG):
            penabletx = &port->extlv.etscfg;
            break;
            
        case TLVID_8021(LLDP_8021QAZ_ETSREC):
            penabletx = &port->extlv.etsrec;
            break;
            
        case TLVID_8021(LLDP_8021QAZ_PFC):
            penabletx = &port->extlv.pfc;
            break;

        case TLVID_8021(LLDP_8021QAZ_APP):
            penabletx = &port->extlv.app;
            break;
            
        case TLVID_8021(LLDP_8021QAU_QCN):
            penabletx = &port->extlv.qcn;
            break;

        case TLVID_8021Qbg(LLDP_EVB_SUBTYPE):
            penabletx = &port->extlv.evbtlv;
            break;

        case TLVID_8021Qbg(LLDP_CDCP_SUBTYPE):
            penabletx = &port->extlv.cdcp;
            break;
        default:;
            
    }    

    return penabletx;
}

bool is_tlv_enabled(u32 local_port, u32 tlvid)
{
    bool *penable = NULL;

    penable = find_txtlv_support(local_port,tlvid);
    if (!penable)
    {
        return false;
    }

    return *penable;
}

bool set_tlv_enabled(u32 local_port, u32 tlvid, bool val)
{
    bool *penable = NULL;
    struct lldp_port * port = NULL;
    
    port = find_by_port(local_port);
    if (!port)
    {
        return false;
    }    

    penable = find_txtlv_support(local_port,tlvid);
    if (!penable)
    {
        return false;
    }
    
    *penable = val;   

    return true;
}


#ifdef __cplusplus
}
#endif


