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

struct tlv_info_chassis {
    u8 sub;
    union {
        u8 ccomp[255];
        u8 ifalias[255];
        u8 pcomp[255];
        u8 mac[6];
        struct  {
            u8 type;
            union {
                u8 v4[4];
                u8 v6[4];
            }ip;
        }na;
        u8 ifname[255];
        u8 local[255];
    }id;
};

struct tlv_info_portid {
    u8 sub;
    union {
        u8 ifalias[255];
        u8 pcomp[255];
        u8 mac[6];
        struct  {
            u8 type;
            union {
                u8 v4[4];
                u8 v6[4];
            }ip;
        }na;
        u8 ifname[255];
        u8 circuit[255];
        u8 local[255];
    }id;
};

static const struct lldp_mod_ops mand_ops = {
    mand_register,
    mand_unregister,
    mand_gettlv,
    0,
    0,
    mand_ifup,
    mand_ifdown,
};

struct mand_data *mand_data(u32 local_port)
{
    struct mand_user_data *mud;
    struct mand_data *md = NULL;

    mud = (struct mand_user_data *)find_module_user_data_by_id(&gtLLDP.mod_head, LLDP_MOD_MAND);
    if (mud) {
        LIST_FOREACH(md, &mud->head, entry) {
            if (local_port==md->local_port)
                return md;
        }
    }
    return NULL;
}

/*
 * mand_bld_end_tlv - build mandatory End TLV
 * @md: the mand data struct
 * 
 * Returns 0 for success or error code for failure
 *
 */
static int mand_bld_end_tlv(struct mand_data *md)
{
    int rc = -1;
    struct unpacked_tlv *tlv;

    tlv = create_tlv();
    if(tlv) {
        tlv->type = END_OF_LLDPDU_TLV;
        tlv->length = 0;
        tlv->info = NULL;
        md->end = tlv;
        rc = 0;
    }
    return rc;
}

/*
 * mand_bld_chassis_tlv - build mandatory End TLV
 * @md: the mand data struct
 * 
 * Returns 0 for success or error code for failure
 *
 * Load from config if is configured, otherwise build from
 * scratc. Note that for LLDP-MED, 
 *  - Mandatory for LLDP-MED Network Connectivity w/ default to MAC
 *  - Mandatory for LLDP-MED Endpoint w/ default to network addr
 *
 * In the case of MED being enabled w/ undefined or invalid devtype?
 * we will just use network addr, assuming Endpoint device.
 * 
 * If MED is not enabled, the order, as spec says, is:
 * - CHASSIS_ID_CHASSIS_COMPONENT: only from config
 * - CHASSIS_ID_INTERFACE_ALIAS : only from config
 * - CHASSIS_ID_PORT_COMPONENT  : only from config
 * - CHASSIS_ID_MAC_ADDRESS : first from config, then built from scratch
 * - CHASSIS_ID_NETWORK_ADDRESS : first from config, then built from scratch
 * - CHASSIS_ID_INTERFACE_NAME  : first from config, then built from scratch
 * - CHASSIS_ID_LOCALLY_ASSIGNED: only load from config
 *
 * TODO: 
 * - Specs says chassis should remain constant for all LLDPUs
 * while the connection remains operational, so this is built only
 * once.
 * - No validation on data loaded from config other than the subtype
 *
 */

int is_valid_mac(const u8 *mac)
{
    return !!(mac[0] | mac[1] | mac[2] | mac[3] | mac[4] | mac[5]);
}

int mand_bld_chassis_tlv(struct mand_data *md)
{
    u8 length;
    struct lldp_port *port;    
    struct unpacked_tlv *tlv;
    struct tlv_info_chassis chassis;

    /* build only once */
    if ((!md->rebuild_chassis) && (md->chassis)) 
    {
        return 0;
    }

    port = find_by_port(md->local_port);
    if (NULL==port)
    {
        return -1;
    }

    /* free before building it */
    md->rebuild_chassis = 1;
    FREE_UNPKD_TLV(md, chassis);
    memset(&chassis, 0, sizeof(chassis));

    /* if build from scratch, try mac, then ip, then ifname */
    memcpy(chassis.id.mac, port->smac, 6);
    if (!is_valid_mac(chassis.id.mac)) 
    {
        return -1;
    }

    chassis.sub = CHASSIS_ID_MAC_ADDRESS;
    length      = sizeof(chassis.id.mac) + sizeof(chassis.sub);
    tlv = create_tlv();
    if (!tlv)
    {
        return -1;
    }
    tlv->type   = CHASSIS_ID_TLV;
    tlv->length = length;
    tlv->info   = (u8 *)GLUE_ALLOC(length);
    if(!tlv->info)
    {
        GLUE_FREE(tlv);
        return -1;
    }
    memset(tlv->info, 0, tlv->length);
    memcpy(tlv->info, &chassis, tlv->length);
    md->chassis = tlv;
    md->rebuild_chassis = 0;

    return 0;
}

/*
 * mand_bld_portid_tlv - build mandatory End TLV
 * @md: the mand data struct
 * 
 * Returns 0 for success or error code for failure
 *
 * Load from config if is configured, otherwise build from
 * scratc. Note that for LLDP-MED, 
 *  - Mandatory and default to MAC for Network Connectivity, and 
 *  Endpoint Devices
 *
 * In the case of MED being enabled w/ undefined or invalid devtype?
 * we will just use mac
 * 
 * If MED is not enabled, the order, as spec says, is:
 * - PORT_ID_INTERFACE_ALIAS    : only from config
 * - PORT_ID_PORT_COMPONENT : only from config
 * - PORT_ID_MAC_ADDRESS    : first from config, then built from scratch
 * - PORT_ID_NETWORK_ADDRESS    : first from config, then built from scratch
 * - PORT_ID_INTERFACE_NAME : first from config, then built from scratch
 * - PORT_ID_AGENT_CIRCUIT_ID   : only from config
 * - PORT_ID_LOCALLY_ASSIGNED   : only load from config
 *
 * TODO: 
 * - The port id should remain constant for all LLDPUs while the connection
 *   remains operational, so this is built only once.
 * - No validation on data loaded from config other than the subtype
 *
 */
static int mand_bld_portid_tlv(struct mand_data *md)
{
    u8 length;
    struct lldp_port *port;    
    struct unpacked_tlv *tlv;
    struct tlv_info_portid portid;

    /* build only once */
    if ((!md->rebuild_portid) && (md->portid)) 
    {
        return 0;
    }

    port = find_by_port(md->local_port);
    if (NULL==port)
    {
        return -1;
    }
    
    /* free before building it */
    md->rebuild_portid = 1;
    FREE_UNPKD_TLV(md, portid);
    memset(&portid, 0, sizeof(portid));

    portid.sub = PORT_ID_INTERFACE_NAME;
    strncpy((char *)portid.id.ifname, (char *)port->name, IFNAMSIZ);
    length = strlen((char *)port->name) + sizeof(portid.sub);

    tlv = create_tlv();
    if (!tlv)
    {
       return -1;
    }
    
    tlv->type   = PORT_ID_TLV;
    tlv->length = length;
    tlv->info   = (u8 *)GLUE_ALLOC(length);
    if(!tlv->info)
    {
        GLUE_FREE(tlv);
        return -1;
    }
    
    memset(tlv->info, 0, tlv->length);
    memcpy(tlv->info, &portid, tlv->length);
    md->portid = tlv;
    md->rebuild_portid = 0;

    return 0;
}

static int mand_bld_ttl_tlv(struct mand_data *md)
{
    int rc = -1;
    u16 ttl;
    struct unpacked_tlv *tlv;
    struct lldp_port *port;

    tlv = create_tlv();
    if (!tlv)
        goto out_err;

    tlv->type = TIME_TO_LIVE_TLV;
    tlv->length = 2;
    tlv->info = (u8 *)GLUE_ALLOC(tlv->length);
    if(!tlv->info) {
        GLUE_FREE(tlv);
        goto out_err;
    }
    memset(tlv->info, 0, tlv->length);

    port = find_by_port(md->local_port);
    if (port->tx.txTTL)
        ttl = EVB_HTONS(port->tx.txTTL);
    else
        ttl = EVB_HTONS(DEFAULT_TX_HOLD * DEFAULT_TX_INTERVAL);

    memcpy(tlv->info, &ttl, tlv->length);
    LLDPAD_DBG("%s:port %d:done:type=%d length=%d ttl=%d\n", __func__, md->local_port, tlv->type, tlv->length, EVB_NTOHS(ttl));
    md->ttl = tlv;
    rc = 0;
out_err:
    return rc;
}

struct packed_tlv *mand_gettlv(struct lldp_port *port)
{
    struct mand_data *md;
    struct packed_tlv *ptlv = NULL;
    u16 size;

    md = mand_data(port->local_port);
    if (!md) {
        LLDPAD_DBG("%s:port %d: not found port\n", __func__, port->local_port);
        goto out_err;
    }

    size = TLVSIZE(md->chassis)
        + TLVSIZE(md->portid)
        + TLVSIZE(md->ttl);
    if (!size)
        goto out_err;

    ptlv = create_ptlv();
    if (!ptlv)
        goto out_err;

    ptlv->tlv = (u8 *)GLUE_ALLOC(size);
    if (!ptlv->tlv)
        goto out_free;

    ptlv->size = 0;
    PACK_TLV_AFTER(md->chassis, ptlv, size, out_free);
    PACK_TLV_AFTER(md->portid, ptlv, size, out_free);
    PACK_TLV_AFTER(md->ttl, ptlv, size, out_free);
    return ptlv;
out_free:
    ptlv = free_pkd_tlv(ptlv);
out_err:
    LLDPAD_DBG("%s:port %d: failed\n", __func__, port->local_port);
    return NULL;

}

static void mand_free_tlv(struct mand_data *md)
{
    if (md) {
        FREE_UNPKD_TLV(md, chassis);
        FREE_UNPKD_TLV(md, portid);
        FREE_UNPKD_TLV(md, ttl);
        FREE_UNPKD_TLV(md, end);
    }
}

/* build unpacked tlvs */
int mand_bld_tlv(struct mand_data *md)
{
    int rc = 0;

    if (!find_by_port(md->local_port)) {
        rc = -1;
        goto out_err;
    }

    if (mand_bld_chassis_tlv(md)) {
        LLDPAD_DBG("%s:port %d:mand_bld_chassis_tlv() failed\n",__func__, md->local_port);
        goto out_err;
    }
    if (mand_bld_portid_tlv(md)) {
        LLDPAD_DBG("%s:port %s:mand_bld_portid_tlv() failed\n", __func__, md->local_port);
        goto out_err;
    }
    if (mand_bld_ttl_tlv(md)) {
        LLDPAD_DBG("%s:port %s:mand_bld_ttl_tlv() failed\n",__func__, md->local_port);
        goto out_err;
    }
    if (mand_bld_end_tlv(md)) {
        LLDPAD_DBG("%s:port %s:mand_bld_end_tlv() failed\n",__func__, md->local_port);
        goto out_err;
    }
    rc = 0;

out_err:
    return rc;

}

static void mand_free_data(struct mand_user_data *mud)
{
    struct mand_data *md;
    if (mud) {
        while (!LIST_EMPTY(&mud->head)) {
            md = LIST_FIRST(&mud->head);
            LIST_REMOVE(md, entry);
            mand_free_tlv(md);
            GLUE_FREE(md);
        }
    }
}

void mand_ifdown(u32 local_port)
{
    struct mand_data *md;

    md = mand_data(local_port);
    if (!md)
        goto out_err;

    LIST_REMOVE(md, entry);
    mand_free_tlv(md);
    GLUE_FREE(md);
    LLDPAD_INFO("%s:port %d mand_ifdown ok!\n", __func__, local_port); 
    return;
out_err:
    LLDPAD_INFO("%s:port %d mand_ifdown error!\n", __func__, local_port); 
    return;
}

void mand_ifup(u32 local_port)
{
    struct mand_data *md;
    struct mand_user_data *mud;

    md = mand_data(local_port);
    if (md) {
        LLDPAD_INFO("%s:port %d exists\n", __func__, local_port); 
        goto out_err;
    }
    /* not found, alloc/init per-port tlv data */
    md = (struct mand_data *) GLUE_ALLOC(sizeof(*md));
    if (!md) {
        LLDPAD_INFO("%s:port %d GLUE_ALLOC %lu failed\n", __func__, local_port, sizeof(*md)); 
        goto out_err;
    }
    memset(md, 0, sizeof(struct mand_data));
    md->local_port = local_port;
    if (mand_bld_tlv(md)) {
        LLDPAD_INFO("%s:port %d mand_bld_tlv failed\n", __func__, local_port); 
        GLUE_FREE(md);
        goto out_err;
    }

    /* chassis is built once and remains constant */
    md->rebuild_chassis = 1; 
    /* portid is built once and remains constant */
    md->rebuild_portid = 1; 
    mud = (struct mand_user_data *)find_module_user_data_by_id(&gtLLDP.mod_head, LLDP_MOD_MAND);
    LIST_INSERT_HEAD(&mud->head, md, entry);
    LLDPAD_INFO("%s:port %d mand_ifup ok!\n", __func__, local_port); 
    return;
out_err:
    LLDPAD_INFO("%s:port %d mand_ifup error!\n", __func__, local_port); 
    return;
}

struct lldp_module *mand_register(void)
{
    struct lldp_module *mod;
    struct mand_user_data *mud;

    mod = (struct lldp_module *)GLUE_ALLOC(sizeof(struct lldp_module));
    if (!mod) {
        LLDPAD_ERR("failed to GLUE_ALLOC LLDP Mandatory module data\n");
        goto out_err;
    }
    mud = (struct mand_user_data *)GLUE_ALLOC(sizeof(struct mand_user_data));
    if (!mud) {
        GLUE_FREE(mod);
        LLDPAD_ERR("failed to GLUE_ALLOC LLDP Mandatory module user data\n");
        goto out_err;
    }
    LIST_INIT(&mud->head);
    mod->id = LLDP_MOD_MAND;
    mod->ops = &mand_ops;
    mod->data = mud;
    LLDPAD_INFO("%s:done\n", __func__);
    return mod;
out_err:
    LLDPAD_INFO("%s:failed\n", __func__);
    return NULL;
}

void mand_unregister(struct lldp_module *mod)
{
    if (mod->data) {
        mand_free_data((struct mand_user_data *) mod->data);
        GLUE_FREE(mod->data);
    }
    GLUE_FREE(mod);
    LLDPAD_INFO("%s:done\n", __func__); 
}

#ifdef __cplusplus
}
#endif




