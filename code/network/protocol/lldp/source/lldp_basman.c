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

static const struct lldp_mod_ops basman_ops =  {
    basman_register,
    basman_unregister,
    basman_gettlv,
    0,
    0,  
    basman_ifup,
    basman_ifdown,
};

int get_addr(u32 local_port, int domain, void *buf)
{
    struct lldp_port *port;

    port = find_by_port(local_port);
    if (NULL==port)
    {
        return -1;
    }    
    
    if (domain == INET)
    {
        return -1;
    }

    if (domain == INET6)
    {
        return -1;
    }

    if (domain == UNSPEC)
    {
        memcpy(buf,port->smac,6);
        return 0;
    }

    return -1;

}

u16 get_caps(u32 local_port)
{
    u16 caps = 0;

    /* how to find TPID to determine C-VLAN vs. S-VLAN ? */
    if (is_vlan(local_port))
        caps |= SYSCAP_CVLAN; 

    if (is_bridge(local_port))
        caps |= SYSCAP_BRIDGE;

    if (is_router(local_port))
        caps |= SYSCAP_ROUTER;

    if (is_wlan(local_port))
        caps |= SYSCAP_WLAN;
    
    if (is_phone(local_port))
        caps |= SYSCAP_PHONE;
    
    if (is_docsis(local_port))
        caps |= SYSCAP_DOCSIS;
    
    if (is_repeater(local_port))
        caps |= SYSCAP_REPEATER;
    
    if (is_tpmr(local_port))
        caps |= SYSCAP_TPMR;
    
    if (is_other(local_port))
        caps |= SYSCAP_OTHER;


    if (!caps)
        caps = SYSCAP_STATION;

    return caps;
}

u8 is_active(u32 local_port)
{
    return 1;    
}

static struct basman_data *basman_data(u32 local_port)
{
    struct basman_user_data *bud;
    struct basman_data *bd = NULL;

    bud = (struct basman_user_data *)find_module_user_data_by_id(&gtLLDP.mod_head, LLDP_MOD_BASIC);
    if (bud) {
        LIST_FOREACH(bd, &bud->head, entry) {
            if (local_port==bd->local_port)
                return bd;
        }
    }
    return NULL;
}

/*
 * basman_bld_portdesc_tlv - build port description TLV
 * @bd: the basman data struct
 *
 * Returns 0 for success or error code for failure
 */
static int basman_bld_portdesc_tlv(struct basman_data *bd)
{
    unsigned int length;
    int rc = 0;
    char desc[256];
    struct unpacked_tlv *tlv = NULL;
    struct lldp_port *port;

    if (NULL==bd)
    {
        goto out_err;
    }

    port = find_by_port(bd->local_port);
    if (NULL==port)
    {
        goto out_err;
    }    

    /* free old if it's there */
    FREE_UNPKD_TLV(bd, portdesc);

    if (!is_tlv_enabled(bd->local_port, PORT_DESCRIPTION_TLV)) {
        LLDPAD_DBG("%s:port %d:Port Description disabled\n", __func__, bd->local_port);
        goto out_err;
    }
    #if 0    

    /* load from config */
    if (!get_config_tlvinfo_str(bd->ifname, TLVID_NOUI(PORT_DESCRIPTION_TLV),
                    desc, sizeof(desc))) {
        /* use what's in the config */
        length = strlen(desc);
        LLDPAD_DBG("%s:%s:configed as %s\n", __func__, bd->ifname, desc);
    } else {
        length = snprintf(desc, sizeof(desc), "Interface %3d as %s",
              if_nametoindex(bd->ifname), bd->ifname);
        LLDPAD_DBG("%s:%s:built as %s\n", __func__, bd->ifname, desc);
    }

    #endif

    length = snprintf(desc, sizeof(desc), "Interface %3d as %s", bd->local_port, port->name);
    
    if (length >= sizeof(desc))
        length = sizeof(desc) - 1;

    tlv = create_tlv();
    if (!tlv)
        goto out_err;

    tlv->type = PORT_DESCRIPTION_TLV;
    tlv->length = length;
    tlv->info = (u8 *)GLUE_ALLOC(tlv->length);
    if(!tlv->info) {
        GLUE_FREE(tlv);
        tlv = NULL;
        goto out_err;
    }
    memcpy(tlv->info, desc, tlv->length);
    bd->portdesc = tlv;
    rc = 0;
out_err:
    return rc;
}


/*
 * basman_bld_sysname_tlv - build port description TLV
 * @bd: the basman data struct
 *
 * Returns 0 for success or error code for failure
 */
static int basman_bld_sysname_tlv(struct basman_data *bd)
{
    unsigned int length;
    int rc = 0;
    char desc[256];
    struct unpacked_tlv *tlv = NULL;  

    if (NULL==bd)
    {
        goto out_err;
    }   

    /* free old if it's there */
    FREE_UNPKD_TLV(bd, sysname);

    if (!is_tlv_enabled(bd->local_port, SYSTEM_NAME_TLV)) {
        LLDPAD_DBG("%s:%s:System Name disabled\n", __func__, bd->local_port);
        goto out_err;
    }

    #if 0  
    /* load from config */
    if (!get_config_tlvinfo_str(bd->ifname, TLVID_NOUI(SYSTEM_NAME_TLV),
                    desc, sizeof(desc))) {
        /* use what's in the config */
        length = strlen(desc);
        LLDPAD_DBG("%s:%s:configed as %s\n",
            __func__, bd->ifname, desc);
    } else {
        if (uname(&uts))
            length = snprintf(desc, sizeof(desc), SYSNAME_DEFAULT);
        else
            length = snprintf(desc, sizeof(desc), uts.nodename);
        LLDPAD_DBG("%s:%s:built as %s\n",
            __func__, bd->ifname, desc);
    }
    #endif

    length = snprintf(desc, sizeof(desc), SYSNAME_DEFAULT);
    
    if (length >= sizeof(desc))
        length = sizeof(desc) - 1;

    tlv = create_tlv();
    if (!tlv)
        goto out_err;

    tlv->type = SYSTEM_NAME_TLV;
    tlv->length = length;
    tlv->info = (u8 *)GLUE_ALLOC(tlv->length);
    if(!tlv->info){
        GLUE_FREE(tlv);
        tlv = NULL;
        goto out_err;
    }
    memcpy(tlv->info, desc, tlv->length);
    bd->sysname = tlv;
    rc = 0;
out_err:
    return rc;
}


/*
 * basman_bld_sysdesc_tlv - build port description TLV
 * @bd: the basman data struct
 *
 * Returns 0 for success or error code for failure
 *
 * net-snmp-utils: snmptest returns the following for sysDesr: `uname-a`
 */
static int basman_bld_sysdesc_tlv(struct basman_data *bd)
{
    unsigned int length;
    int rc = 0;
    char desc[256];
    struct unpacked_tlv *tlv = NULL;

    /* free old if it's there */
    FREE_UNPKD_TLV(bd, sysdesc);

    if (!is_tlv_enabled(bd->local_port, SYSTEM_DESCRIPTION_TLV)) {
        LLDPAD_DBG("%s:%s:System Description disabled\n", __func__, bd->local_port);
        goto out_err;
    }

    #if 0  
    /* load from config */
    if (!get_config_tlvinfo_str(bd->ifname, TLVID_NOUI(SYSTEM_DESCRIPTION_TLV),
                    desc, sizeof(desc))) {
        /* use what's in the config */
        length = strlen(desc);
        LLDPAD_DBG("%s:%s:configed as %s\n",
            __func__, bd->ifname, desc);
    } else {
        if (uname(&uts)) {
            length = snprintf(desc, sizeof(desc), "Unknown system");
        } else {
            length = snprintf(desc, sizeof(desc), "%s %s %s %s %s",
                      uts.sysname, uts.nodename, uts.release,
                      uts.version, uts.machine);
        }
        LLDPAD_DBG("%s:%s:built as %s\n",
            __func__, bd->ifname, desc);
    }

    #endif

    length = snprintf(desc, sizeof(desc), SYSNAME_DEFAULT ",FCF Switch, V1.0");    
    
    if (length >= sizeof(desc))
        length = sizeof(desc) - 1;

    tlv = create_tlv();
    if (!tlv)
        goto out_err;

    tlv->type = SYSTEM_DESCRIPTION_TLV;
    tlv->length = length;
    tlv->info = (u8 *)GLUE_ALLOC(tlv->length);
    if(!tlv->info){
        GLUE_FREE(tlv);
        tlv = NULL;
        goto out_err;
    }
    memcpy(tlv->info, desc, tlv->length);
    bd->sysdesc = tlv;
    rc = 0;
out_err:
    return rc;
}

/*
 * basman_bld_syscaps_tlv - build port description TLV
 * @bd: the basman data struct
 *
 * Returns 0 for success or error code for failure
 *
 * TODO:
 *  - This is mandatory for LLDP-MED Class III
 *  - TPID to determine C-VLAN vs. S-VLAN ?
 */
static int basman_bld_syscaps_tlv(struct basman_data *bd)
{
    int rc = 0;
    u16 syscaps[2];
    struct unpacked_tlv *tlv = NULL;

    /* free old if it's there */
    FREE_UNPKD_TLV(bd, syscaps);

    if (!is_tlv_enabled(bd->local_port, SYSTEM_CAPABILITIES_TLV)) {
        LLDPAD_DBG("%s:%s:System Capabilities disabled\n", __func__, bd->local_port);
        goto out_err;
    }

    #if 0    
    /* load from config */
    if (get_config_tlvinfo_bin(bd->ifname, TLVID_NOUI(SYSTEM_CAPABILITIES_TLV),
                  (void *)&syscaps, sizeof(syscaps))) {
        LLDPAD_DBG("%s:%s:Build System Caps from scratch\n",
            __func__, bd->ifname);
        syscaps[0] = htons(get_caps(bd->ifname));
        syscaps[1] = (is_active(bd->ifname)) ? syscaps[0] : 0;
    }
    #endif

    syscaps[0] = EVB_HTONS(get_caps(bd->local_port));
    syscaps[1] = (is_active(bd->local_port)) ? syscaps[0] : 0;
    

    tlv = create_tlv();
    if (!tlv)
        goto out_err;

    tlv->type = SYSTEM_CAPABILITIES_TLV;
    tlv->length = sizeof(syscaps);
    tlv->info = (u8 *)GLUE_ALLOC(tlv->length);
    if(!tlv->info) {
        GLUE_FREE(tlv);
        tlv = NULL;
        goto out_err;
    }
    memcpy(tlv->info, &syscaps, tlv->length);
    bd->syscaps = tlv;
    rc = 0;
out_err:
    return rc;
}

/*
 * basman_get_manaddr_sub - build management address TLV by subtype
 * @bd: the basman data struct
 * @masub: IETF RFC 3232 ianaAddressFamilyNumbers
 *
 * Returns 0 for success or error code for failure
 *
 * Currently supports only IPv4, IPv6, and MAC address types.
 *
 */
static int basman_get_manaddr_sub(struct basman_data *bd, u8 masub)
{
    int domain;
    int length = 0;
    int rc = -1;
    u8 *data = NULL;
    char maddr[128];
    const char *field = NULL;
    struct tlv_info_maif *i = NULL;
    struct tlv_info_maddr *m = NULL;
    struct tlv_info_maoid *o = NULL;
    struct tlv_info_manaddr manaddr;
    struct unpacked_tlv *tlv = NULL;

    if (bd->macnt >= MANADDR_MAX) {
        LLDPAD_DBG("%s:port %d:reached max %d Management Address\n",    __func__, bd->local_port, bd->macnt);
        goto out_err;
    }

    memset(maddr, 0, sizeof(maddr));
    memset(&manaddr, 0, sizeof(manaddr));
    m = &manaddr.m;
    m->sub = masub;
    switch(m->sub) {
    case MANADDR_IPV4:
        field = "ipv4";
        domain = INET;
        m->len = sizeof(m->a.in);
        break;
    case MANADDR_IPV6:
        field = "ipv6";
        domain = INET6;
        m->len = sizeof(m->a.in6);
        break;
    case MANADDR_ALL802:
        field = "mac";
        domain = UNSPEC;
        m->len = sizeof(m->a.mac);
        break;
    default:
        LLDPAD_DBG("%s:port %d:unsupported sub type %d\n",__func__, bd->local_port, masub);
        goto out_err;
    }
    
    m->len += sizeof(m->sub);

    #if 0
    /* read from the config first */
    if (get_config_tlvfield_str(bd->ifname,
                    TLVID_NOUI(MANAGEMENT_ADDRESS_TLV),
                    field, (void *)maddr, sizeof(maddr))) {
        LLDPAD_DBG("%s:%s:failed to get %s from config\n",
            __func__, bd->ifname, field);
        goto out_bld;
    }

    
    if (!str2addr(domain, maddr, &m->a, sizeof(m->a))) {
        goto out_set;
    }

out_bld:
    /* failed to get from config, so build from scratch */
    if (get_addr(bd->ifname, domain, &m->a)) {
        LLDPAD_DBG("%s:%s:get_addr() for domain %d failed\n",
            __func__, bd->ifname, domain);
        goto out_err;
    }
    if (addr2str(domain, &m->a, maddr, sizeof(maddr))) {
        LLDPAD_DBG("%s:%s:get_addr() for domain %d failed\n",
            __func__, bd->ifname, domain);
        goto out_err;
    }

out_set:

    set_config_tlvfield_str(bd->ifname,
                TLVID_NOUI(MANAGEMENT_ADDRESS_TLV),
                field, (void *)maddr, sizeof(maddr));
    #endif

    if (get_addr(bd->local_port, domain, &m->a)) {
        LLDPAD_DBG("%s:port %d:get_addr() for domain %d failed\n",__func__, bd->local_port, domain);
        goto out_err;
    }    

    /* build ifnum and oid:
     *  mlen + msub + maddr  + ifsub + ifidx + oidlen + oid
     *  1    + 1    + [1-31] + 1     + 4     + 1       + [1-128]
     */
    data = (u8 *)&manaddr;
    length = sizeof(manaddr.m.len) + manaddr.m.len;
    i = (struct tlv_info_maif *)&data[length];
    i->sub = IFNUM_SUBTYPE_IFINDEX;
    i->num = EVB_HTONL(bd->local_port);

    length += sizeof(struct tlv_info_maif);
    o = (struct tlv_info_maoid *)&data[length];
    o->len = 0;
    length += sizeof(o->len);

    tlv = create_tlv();
    if (!tlv)
        goto out_err;

    tlv->length = length;
    tlv->type = MANAGEMENT_ADDRESS_TLV;
    tlv->info = (u8 *)GLUE_ALLOC(tlv->length);
    if(!tlv->info) {
        GLUE_FREE(tlv);
        tlv = NULL;
        goto out_err;
    }

    memcpy(tlv->info, &manaddr, tlv->length);
    bd->manaddr[bd->macnt] = tlv;
    bd->macnt++;

    LLDPAD_DBG("%s:port %d:maddr[%d]:tlv->len %d bytes \n",__func__, bd->local_port, bd->macnt, tlv->length);

    rc = 0;
out_err:
    return rc;
}

/*
 * basman_bld_manddrr_tlv - build management address TLV
 * @bd: the basman data struct
 *
 * Returns 0 for success or error code for failure
 *
 * Use as many existing as possible
 * Preference is config > ipv6 > ipv4 > mac > default
 *
 * Max info length is = 1 + 1 + 31 + 1 + 4 + 1 + 128 = 167
 *
 * TODO:
 *  - No support for OID yet
 */
static int basman_bld_manaddr_tlv(struct basman_data *bd)
{
    int i;
    int rc = 0;

    /* free all existing manaddr TLVs */
    for (i = 0; i < bd->macnt; i++)
        FREE_UNPKD_TLV(bd, manaddr[i]);
    bd->macnt = 0;

    /* ignore manaddr if it's not enabled for tx */
    if (!is_tlv_enabled(bd->local_port, MANAGEMENT_ADDRESS_TLV)) {
        LLDPAD_DBG("%s:%s:Management Address disabled\n", __func__, bd->local_port);
        goto out_err;
    }

    /* management addr preference: ipv4, ipv6, mac */
    rc = basman_get_manaddr_sub(bd, MANADDR_IPV4);
    if (rc) {
        rc = basman_get_manaddr_sub(bd, MANADDR_IPV6);
        if (rc)
            rc = basman_get_manaddr_sub(bd, MANADDR_ALL802);
    }
out_err:
    return rc;
}

static void basman_free_tlv(struct basman_data *bd)
{
    int i = 0;

    if (bd) {
        FREE_UNPKD_TLV(bd, portdesc);
        FREE_UNPKD_TLV(bd, sysname);
        FREE_UNPKD_TLV(bd, sysdesc);
        FREE_UNPKD_TLV(bd, syscaps);
        for (i = 0; i < bd->macnt; i++)
            FREE_UNPKD_TLV(bd, manaddr[i]);
        bd->macnt = 0;
    }
}

/* build unpacked tlvs */
static int basman_bld_tlv(struct basman_data *bd)
{
    int rc = -1;

    if (!find_by_port(bd->local_port)) {
        rc = -1;
        goto out_err;
    }

    if (basman_bld_portdesc_tlv(bd)) {
        LLDPAD_DBG("%s:port %d:basman_bld_portdesc_tlv() failed\n",__func__, bd->local_port);
        goto out_err;
    }
    if (basman_bld_sysname_tlv(bd)) {
        LLDPAD_DBG("%s:port %d:basman_bld_sysname_tlv() failed\n",__func__, bd->local_port);
        goto out_err;
    }
    if (basman_bld_sysdesc_tlv(bd)) {
        LLDPAD_DBG("%s:port %d:basman_bld_sysdesc_tlv() failed\n",__func__, bd->local_port);
        goto out_err;
    }
    if (basman_bld_syscaps_tlv(bd)) {
        LLDPAD_DBG("%s:port %d:basman_bld_syscaps_tlv() failed\n",__func__, bd->local_port);
        goto out_err;
    }
    if (basman_bld_manaddr_tlv(bd)) {
        LLDPAD_DBG("%s:port %d:basman_bld_manaddr_tlv() failed\n",__func__, bd->local_port);
        goto out_err;
    }
    rc = 0;

out_err:
    return rc;
}

static void basman_free_data(struct basman_user_data *bud)
{
    struct basman_data *bd;
    if (bud) {
        while (!LIST_EMPTY(&bud->head)) {
            bd = LIST_FIRST(&bud->head);
            LIST_REMOVE(bd, entry);
            basman_free_tlv(bd);
            GLUE_FREE(bd);
        }
    }
}

struct packed_tlv *basman_gettlv(struct lldp_port *port)
{
    int i;
    int size;
    struct basman_data *bd;
    struct packed_tlv *ptlv = NULL;

    bd = basman_data(port->local_port);
    if (!bd)
        goto out_err;

    /* free and rebuild the TLVs */
    basman_free_tlv(bd);
    if (basman_bld_tlv(bd)) {
        LLDPAD_DBG("%s:port %d basman_bld_tlv failed\n",    __func__, port->local_port);
        goto out_err;
    }

    size = TLVSIZE(bd->portdesc)
         + TLVSIZE(bd->sysname)
         + TLVSIZE(bd->sysdesc)
         + TLVSIZE(bd->syscaps);

    for (i = 0; i < bd->macnt; i++)
        size += TLVSIZE(bd->manaddr[i]);

    if (!size)
        goto out_err;

    ptlv = create_ptlv();
    if (!ptlv) {
        LLDPAD_DBG("%s:port %d GLUE_ALLOC(ptlv) failed\n", __func__, port->local_port);
        goto out_err;
    }

    ptlv->tlv = (u8 *)GLUE_ALLOC(size);
    if (!ptlv->tlv)
        goto out_free;

    /* pack all pre-built tlvs */
    ptlv->size = 0;
    PACK_TLV_AFTER(bd->portdesc, ptlv, size, out_free);
    PACK_TLV_AFTER(bd->sysname, ptlv, size, out_free);
    PACK_TLV_AFTER(bd->sysdesc, ptlv, size, out_free);
    PACK_TLV_AFTER(bd->syscaps, ptlv, size, out_free);
    for (i = 0; i < bd->macnt; i++)
        PACK_TLV_AFTER(bd->manaddr[i], ptlv, size, out_free);
    return ptlv;
out_free:
    ptlv = free_pkd_tlv(ptlv);
out_err:
    LLDPAD_DBG("%s:port %d: failed\n", __func__, port->local_port);
    return NULL;
}

void basman_ifdown(u32 local_port)
{
    struct basman_data *bd;

    bd = basman_data(local_port);
    if (!bd)
        goto out_err;

    LIST_REMOVE(bd, entry);
    basman_free_tlv(bd);
    GLUE_FREE(bd);
    LLDPAD_DBG("%s:port %d basman_ifdown ok!\n", __func__, local_port);
    return;
out_err:
    LLDPAD_DBG("%s:port %d basman_ifdown error!\n", __func__, local_port);
    return;
}

void basman_ifup(u32 local_port)
{
    struct basman_data *bd;
    struct basman_user_data *bud;

    bd = basman_data(local_port);
    if (bd) {
        LLDPAD_DBG("%s:port %d basman_ifup exists\n", __func__, local_port);
        goto out_err;
    }

    /* not found, alloc/init per-port tlv data */
    bd = (struct basman_data *) GLUE_ALLOC(sizeof(struct basman_data));
    if (!bd) {
        LLDPAD_DBG("%s:port %d GLUE_ALLOC %ld failed\n", __func__, local_port, sizeof(struct basman_data));
        goto out_err;
    }
    memset(bd, 0, sizeof(struct basman_data));
    bd->local_port = local_port;
    if (basman_bld_tlv(bd)) {
        LLDPAD_DBG("%s:port %d mand_bld_tlv failed\n", __func__, local_port);
        GLUE_FREE(bd);
        goto out_err;
    }

    bud = (struct basman_user_data *)find_module_user_data_by_id(&gtLLDP.mod_head, LLDP_MOD_BASIC);
    LIST_INSERT_HEAD(&bud->head, bd, entry);
    LLDPAD_DBG("%s:port %d basman_ifup ok!\n", __func__, local_port);
    return;
out_err:
    LLDPAD_DBG("%s:port %d basman_ifup error!\n", __func__, local_port);
    return;
}

struct lldp_module *basman_register(void)
{
    struct lldp_module *mod;
    struct basman_user_data *bud;

    mod = (struct lldp_module *)GLUE_ALLOC(sizeof(struct lldp_module));
    if (!mod) {
        LLDPAD_ERR("failed to GLUE_ALLOC LLDP Basic Management module data\n");
        goto out_err;
    }
    bud = (struct basman_user_data *)GLUE_ALLOC(sizeof(struct basman_user_data));
    if (!bud) {
        GLUE_FREE(mod);
        LLDPAD_ERR("failed to GLUE_ALLOC LLDP Basic Management module user data\n");
        goto out_err;
    }
    LIST_INIT(&bud->head);
    mod->id = LLDP_MOD_BASIC;
    mod->ops = &basman_ops;
    mod->data = bud;
    LLDPAD_DBG("%s:done\n", __func__);
    return mod;
out_err:
    LLDPAD_DBG("%s:failed\n", __func__);
    return NULL;
}

void basman_unregister(struct lldp_module *mod)
{
    if (mod->data) {
        basman_free_data((struct basman_user_data *) mod->data);
        GLUE_FREE(mod->data);
    }
    GLUE_FREE(mod);
    LLDPAD_DBG("%s:done\n", __func__);
}

#ifdef __cplusplus
}
#endif


