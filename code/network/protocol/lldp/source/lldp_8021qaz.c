/******************************************************************************

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

******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/lldp_in.h"

T_ETS  gtETS = {
   NULL,
   NULL,
   NULL,
};

/* ETS Configuration Object */
t_store_cfg g_store_cfg[10] = 
{
    {
        {
            true,false,8,{0},
            {{1,0xf,40,2},{1,0x30,60,2},{1,0xc0,0,0}},
            {{1,0xf,40,2},{1,0x30,60,2},{1,0xc0,0,0}},               
            {{0}}
        },
        {
            true,false,8,0x0,2000
        },
    },    

    {
        {
            false,false,7,{0},
            {{1,0x1,50,2},{1,0x80,50,2},{1,0x7e,0,0}},
            {{1,0x1,50,2},{1,0x80,50,2},{1,0x7e,0,0}},
            {{0}}
        },
        {
            false,false,7,0x22,2000
        },
    },    
    
    {
        {
            true,true,7,{0},
            {{1,0x7,50,2},{0,0,0,0},{0,0,0,0},{1,0x38,50,2},{1,0xc0,0,0}},
            {{1,0x7,50,2},{0,0,0,0},{0,0,0,0},{1,0x38,50,2},{1,0xc0,0,0}},
            {{0}}                
        },        
        {
            false,true,7,0x33,2000
        },
    },    

    {
        {
            false,true,7,{0},
            {{1,0x30,40,2},{1,0x0c,60,2},{0,0,0,0},{1,0xc3,0,0}},
            {{1,0x30,40,2},{1,0x0c,60,2},{0,0,0,0},{1,0xc3,0,0}},
            {{0}}                
        },
        {
            false,true,7,0x44,2000
        },
    },    

    {
        {
            true,false,7,{0},
            {{0,0,0,0},{1,0x11,30,2},{1,0x88,70,2},{1,0x66,0,0}},
            {{0,0,0,0},{1,0x11,30,2},{1,0x88,70,2},{1,0x66,0,0}},
            {{0}}                
        },
        {
            false,false,7,0x55,2000
        },
    },    
};

/*struct config_t lldpad_cfg;*/
static const struct lldp_mod_ops ieee8021qaz_ops = {
    ieee8021qaz_register,
    ieee8021qaz_unregister,
    ieee8021qaz_gettlv,
    ieee8021qaz_rchange,
    0,
    ieee8021qaz_ifup,
    ieee8021qaz_ifdown,
    ieee8021qaz_ifadd,
    ieee8021qaz_ifdel,    
    ieee8021qaz_mibDeleteObject,
    0,
    0,
    0,
    0,
    0,
    ieee8021qaz_check_pending,
};

int ieee8021qaz_check_pending(struct lldp_port *port)
{
    struct ieee8021qaz_user_data *iud;
    struct ieee8021qaz_tlvs *tlv = NULL;

    if ((NULL==port) || !port->portEnabled)
    {
        return 0;
    }    

    iud = (struct ieee8021qaz_user_data *)find_module_user_data_by_id(&gtLLDP.mod_head, LLDP_MOD_8021QAZ);
    if (iud) 
    {
        LIST_FOREACH(tlv, &iud->head, entry) 
        {
            if (tlv->local_port == port->local_port) 
            {
                if (tlv->active && tlv->pending &&
                    port->timers.dormantDelay == 1) 
                {
                    tlv->pending = false;
                    /*ieee8021qaz_app_reset(&tlv->app_head);*/
                    run_all_sm(port);
                    /*somethingChangedLocal(port->local_port);*/
                }
                break;
            }
        }
    }

    return 0;
}

/* LLDP_8021QAZ_MOD_OPS - REGISTER */
struct lldp_module *ieee8021qaz_register(void)
{
    struct lldp_module *mod;
    struct ieee8021qaz_user_data *iud;

    mod = (struct lldp_module *)GLUE_ALLOC(sizeof(struct lldp_module));
    if (!mod) 
    {
        LLDPAD_ERR("Failed to GLUE_ALLOC LLDP-8021QAZ module data");
        goto out_err;
    }

    iud = (struct ieee8021qaz_user_data *)GLUE_ALLOC(sizeof(struct ieee8021qaz_user_data));
    if (!iud) 
    {
        GLUE_FREE(mod);
        LLDPAD_ERR("Failed to GLUE_ALLOC LLDP-8021QAZ module user data");
        goto out_err;
    }
    memset((void *) iud, 0, sizeof(struct ieee8021qaz_user_data));

    LIST_INIT(&iud->head);

    mod->id   = LLDP_MOD_8021QAZ;
    mod->ops  = &ieee8021qaz_ops;
    mod->data = iud;

    LLDPAD_DBG("%s: ieee8021qaz_register SUCCESS\n", __func__);
    return mod;

out_err:
    LLDPAD_DBG("%s: ieee8021qaz_register FAILED\n", __func__);
    return NULL;
}

struct ieee8021qaz_tlvs *ieee8021qaz_data(u32 local_port)
{
    struct ieee8021qaz_user_data *iud;
    struct ieee8021qaz_tlvs *tlv = NULL;

    iud = (struct ieee8021qaz_user_data *)find_module_user_data_by_id(&gtLLDP.mod_head, LLDP_MOD_8021QAZ);
    if (iud) 
    {
        LIST_FOREACH(tlv, &iud->head, entry) 
        {
            if (tlv->local_port==local_port)
            {
                return tlv;
            }
        }
    }

    return NULL;
}

int read_cfg(u32 local_port)
{
    int i;
    struct ieee8021qaz_tlvs *tlvs = NULL; 
    struct lldp_port * port = NULL;      

    tlvs = ieee8021qaz_data(local_port);    
    if (NULL==tlvs)
    {
        return 0;
    }

    port = find_by_port(local_port);
    if (!port)
    {
        return 0;
    }        

    i = evb_random()%5;

    i = 0;

    if (tlvs->ets)
    {
        if (tlvs->ets->cfgl)
        {
            memcpy(tlvs->ets->cfgl,&(g_store_cfg[i].ets), sizeof(struct ets_obj));
        }
    }

    if (tlvs->pfc)
    {
        tlvs->pfc->local.willing = g_store_cfg[i].pfc.willing;
        tlvs->pfc->local.pfc_enable = g_store_cfg[i].pfc.pfc_enable;
        tlvs->pfc->local.delay = g_store_cfg[i].pfc.delay;    
        tlvs->pfc->local.pfc_cap = g_store_cfg[i].pfc.pfc_cap; 
    } 

    run_all_sm(port);  
    somethingChangedLocal(local_port);

    return 1;
     
}

u8 get_tc_by_pri(TC *ptc, u8 pri)
{
    u8 i;

    if (pri > 7)
    {
        return 0;    
    }
    
    if (NULL==ptc)
    {
        return 0;
    }    

    for (i=0;i<MAX_TCS;i++)
    {
        if (ptc[i].pri & (1<<pri))
        {
            return i;
        }
    }

    return 0;

}

int get_priomap_by_tcs(TC *ptc)
{
    int i;
    u8 j;
    int prio_map = 0;
    
    if (NULL==ptc)
    {
        return 0;
    }     

    for (i=0;i<MAX_TCS;i++)
    {
        for (j=0;j<MAX_USER_PRIORITIES;j++)
        {
            if (ptc[i].pri & (1<<j))
            {
                set_prio_map((u32 *)&prio_map,j,i);                
            }
        }
    }

    return prio_map;
}

int set_tc_pri(TC *ptc, u8 prio, int tc)
{   
    if (NULL==ptc)
    {
        return 0;
    }    

    if (tc < 0 || tc > MAX_TCS)
    {
        return 0;
    }

    if (prio > MAX_USER_PRIORITIES)
    {
        return 0;
    }    

    ptc[tc].pri |= (1<<prio);

    return 1;
}

void set_default_ets_cfg(struct ieee8021qaz_tlvs *tlvs)
{
    if (NULL==tlvs || NULL==tlvs->ets)
    {
        return;
    }

    tlvs->ets->current_state = 0;

    if (tlvs->ets->cfgl)
    {
        memset(tlvs->ets->cfgl,0,sizeof(struct ets_obj));
        
        tlvs->ets->cfgl->willing = false;
        tlvs->ets->cfgl->cbs     = false;
        tlvs->ets->cfgl->max_tcs = 0;

        tlvs->ets->cfgl->tc_cfg[0].used = 1;
        tlvs->ets->cfgl->tc_cfg[0].pri  = 0xff;  

        tlvs->ets->cfgl->tc_rec[0].used = 1;
        tlvs->ets->cfgl->tc_rec[0].pri  = 0xff;         
        
        tlvs->ets->cfgl->tc_cur[0].used = 1;
        tlvs->ets->cfgl->tc_cur[0].pri  = 0xff;  
    }

}

void set_default_pfc_cfg(struct ieee8021qaz_tlvs *tlvs)
{
    if (NULL==tlvs || NULL==tlvs->ets)
    {
        return;
    }

    tlvs->pfc->current_state = 0;
    tlvs->pfc->remote_param = 0;    
}

int unset_tc_pri(TC *ptc, u8 prio, int tc)
{   
    if (NULL==ptc)
    {
        return 0;
    }    

    if (tc < 0 || tc > MAX_TCS)
    {
        return 0;
    }

    if (prio > MAX_USER_PRIORITIES)
    {
        return 0;
    }    

    ptc[tc].pri &= ~(1<<prio);

    return 1;
}


int get_prio_map(u32 prio_map, int prio)
{
    if (prio > 7)
    {
        return 0;
    }

    return (prio_map >> (4 * (7-prio))) & 0xF;
}

void set_prio_map(u32 *prio_map, u8 prio, int tc)
{
    u32 mask = ~(0xffffffff & (0xF << (4 * (7-prio))));
    *prio_map &= mask;
    *prio_map |= tc << (4 * (7-prio));
}

const char* get_tsa_string(u8 tsa)
{
    switch (tsa)
    {
    case IEEE8021Q_TSA_STRICT:
        return "strict";
    case IEEE8021Q_TSA_CBSHAPER:
        return "cb_shaper";
    case IEEE8021Q_TSA_ETS:
        return "ets";
    case IEEE8021Q_TSA_VENDOR:
        return "vendor";            
    default:;
        break;
    }

    return " ";
}

/*
 * LLDP_8021QAZ_MOD_OPS - IFDOWN
 */
void ieee8021qaz_ifdown(u32 local_port)
{
    struct ieee8021qaz_tlvs *tlvs;

    tlvs = ieee8021qaz_data(local_port);
    if (!tlvs) 
    {
        return;
    }

    clear_ieee8021qaz_rx(tlvs);    
}

/*
 * LLDP_8021QAZ_MOD_OPS - IFUP
 *
 * Load TLV values (either from config file, command prompt or defaults),
 * set up adapter, initialize FSMs and build tlvs
 *
 * Check if a 'config' file exists (to load tlv values). If YES, load save them
 * as new defaults. If NO, load defaults. Also, check for TLV values via cmd
 * prompt. Then initialize FSMs for each tlv and finally build the tlvs
 */
void ieee8021qaz_ifup(u32 local_port)
{
    struct ieee8021qaz_tlvs *tlvs;
    /*feature_support dcb_support;
    dcb_support.dcbx = true;*/

    tlvs = ieee8021qaz_data(local_port);
    if (tlvs)
    {
        if (0/*!dcb_support.dcbx ||*/
           /*(dcbx_get_legacy_version() & ~MASK_DCBX_FORCE)*/) 
        {
            tlvs->active = false;
        }
        else 
        {
            tlvs->active = true;
            tlvs->pending = true;
        }
    }

    return;
}

/*
 * LLDP_8021QAZ_MOD_OPS - IFDOWN
 */
void ieee8021qaz_ifdel(u32 local_port)
{
    struct ieee8021qaz_tlvs *tlvs;

    tlvs = ieee8021qaz_data(local_port);
    if (tlvs) 
    {
        LIST_REMOVE(tlvs, entry);
        ieee8021qaz_free_tlv(tlvs);
        clear_ieee8021qaz_rx(tlvs);
        GLUE_FREE(tlvs);
    }
}

void ieee8021qaz_ifadd(u32 local_port)
{
    struct lldp_port *port = NULL;
    struct ieee8021qaz_tlvs *tlvs;
    struct ieee8021qaz_user_data *iud;

    /* If 802.1Qaz is already configured no need to continue */
    tlvs = ieee8021qaz_data(local_port);
    if (tlvs)
    {
        goto initialized;
    }
    
    /* lookup port data */     
    port = find_by_port(local_port);
    if (!port)
    {
        LLDPAD_DBG("%s: port %d find_by_port failed.\n", __func__, local_port);
        return;
    }

    /* Initializing TLV structs */
    tlvs = (struct ieee8021qaz_tlvs *)GLUE_ALLOC(sizeof(struct ieee8021qaz_tlvs));
    if (!tlvs)
    {
        LLDPAD_DBG("%s: port %d tlvs GLUE_ALLOC failed.\n", __func__, local_port);
        return;
    }
    memset(tlvs, 0, sizeof(*tlvs));

    tlvs->rx = (struct ieee8021qaz_unpkd_tlvs *)GLUE_ALLOC(sizeof(struct ieee8021qaz_unpkd_tlvs));
    if (!tlvs->rx) 
    {
        GLUE_FREE(tlvs);
        LLDPAD_DBG("%s: port %d rx GLUE_ALLOC failed.\n", __func__, local_port);
        return;
    }
    memset(tlvs->rx, 0, sizeof(struct ieee8021qaz_unpkd_tlvs));

    /* Initializing the ieee8021qaz_tlvs struct */
    /*strncpy(tlvs->ifname, ifname, IFNAMSIZ);*/
    tlvs->local_port = local_port;
    tlvs->port = port;
    tlvs->ieee8021qazdu = 0;
    l2_packet_get_own_src_addr(port->l2, tlvs->local_mac);
    memset(tlvs->remote_mac, 0, ETH_ALEN);

    /* Initialize all TLVs */
    tlvs->ets = (struct ets_attrib *)GLUE_ALLOC(sizeof(struct ets_attrib));
    if (!tlvs->ets)
    {
        goto err;
    }
    memset(tlvs->ets, 0, sizeof(struct ets_attrib));

    tlvs->ets->cfgl = (struct ets_obj *)GLUE_ALLOC(sizeof(struct ets_obj));
    if (!tlvs->ets->cfgl)
    {
        goto err;
    }

    tlvs->pfc = (struct pfc_attrib *)GLUE_ALLOC(sizeof(struct pfc_attrib));
    if (!tlvs->pfc)
    {
        goto err_pfc;
    }

    memset(tlvs->ets->cfgl, 0, sizeof(struct ets_obj));
    memset(tlvs->pfc, 0, sizeof(struct pfc_attrib));

    set_default_ets_cfg(tlvs);
    set_default_pfc_cfg(tlvs);

    LIST_INIT(&tlvs->app_head);

    iud = (struct ieee8021qaz_user_data *)find_module_user_data_by_id(&gtLLDP.mod_head, LLDP_MOD_8021QAZ);
    LIST_INSERT_HEAD(&iud->head, tlvs, entry);

    read_cfg(port->local_port);

initialized:
    /* if the dcbx field is filled in by the capabilities
     * query, then the kernel is supports
     * IEEE mode, so make IEEE DCBX active by default.
     */
    if (0/*!dcb_support.dcbx ||*/
       /*(dcbx_get_legacy_version() & ~MASK_DCBX_FORCE)*/) 
    {
        tlvs->active = false;
    } 
    else 
    {
        tlvs->active = true;
        tlvs->pending = true;
    }

    return;
err_pfc:
    GLUE_FREE(tlvs->ets->cfgl);
err:
    GLUE_FREE(tlvs->ets);
    GLUE_FREE(tlvs->rx);
    GLUE_FREE(tlvs);
    LLDPAD_WARN("%s: port %d GLUE_ALLOC failed.\n", __func__, local_port);
    return;
}


/*
 * ets_sm - Asymmetric State Machine for the ETS tlv
 */
void ets_sm(struct ets_obj *localAdminParam,
           struct ets_obj *remoteParam,
           u8 *state)
{
    bool willing;
    int remote_param = 0;
    TC  tcs[MAX_TCS] = {{0}};
    
    if (NULL==localAdminParam)
    {
        return;
    }

    if (NULL==remoteParam)
    {
        *state = STATE_INIT;
        return;        
    }

    willing = localAdminParam->willing;
    remote_param = (0==memcmp(remoteParam->tc_rec,tcs,MAX_TCS*sizeof(TC)))?0:1;

    if (willing && remote_param)
        *state = RX_RECOMMEND;
    else if (!willing || !remote_param)
        *state = STATE_INIT;
}

/*
 * cmp_mac_addrs - Compares 2 MAC addresses.
 * returns 1, if first_mac > second_mac; else 0
 */
bool cmp_mac_addrs(u8 first_mac[], u8 second_mac[])
{
    int i;

    for (i = 0; i < ETH_ALEN; i++) 
    {
        if (first_mac[i] == second_mac[i])
        {
            continue;
        }
        
        if (first_mac[i] < second_mac[i])
        {
            return 0;
        }
        
        return 1;
    }
    
    return 0;
}

/*
 * pfc_sm - Symmetric State Machine for the PFC tlv
 */
static void pfc_sm(struct ieee8021qaz_tlvs *tlvs)
{
    bool local_willing, remote_willing;
    bool remote_param, cmp_mac;

    if (NULL==tlvs || NULL==tlvs->pfc)
    {
        return;
    }

    local_willing = tlvs->pfc->local.willing;
    remote_willing = tlvs->pfc->remote.willing;
    remote_param = tlvs->pfc->remote_param;
    cmp_mac = cmp_mac_addrs(tlvs->local_mac, tlvs->remote_mac);

    if ((local_willing && !remote_willing && remote_param) ||
        (local_willing && remote_willing && !cmp_mac))
    {
        tlvs->pfc->current_state = RX_RECOMMEND;
    }
    else if (!local_willing || !remote_param ||
             (local_willing && remote_willing && cmp_mac))
    {
        tlvs->pfc->current_state = STATE_INIT;
    }
}

#ifdef LLDPAD_8021QAZ_DEBUG
void print_ets(struct ieee_ets *ets)
{
    int i;

    printf("ETS:\n");
    printf("\tcap %2x cbs %2x\n", ets->ets_cap, ets->cbs);

    printf("\tets tc_tx_bw: ");
    for (i = 0; i < 8; i++)
        printf("%i ", ets->tc_tx_bw[i]);
    printf("\n");

    printf("\tets tc_rx_bw: ");
    for (i = 0; i < 8; i++)
        printf("%i ", ets->tc_rx_bw[i]);
    printf("\n");

    printf("\tets tc_tsa: ");
    for (i = 0; i < 8; i++)
        printf("%i ", ets->tc_tsa[i]);
    printf("\n");

    printf("\tets prio_tc: ");
    for (i = 0; i < 8; i++)
        printf("%i ", ets->prio_tc[i]);
    printf("\n");
}

void print_pfc(struct ieee_pfc *pfc)
{
    int i;

    printf("PFC:\n");
    printf("\t cap %2x en %2x\n", pfc->pfc_cap, pfc->pfc_en);
    printf("\t mbc %2x delay %i\n", pfc->mbc, pfc->delay);

    printf("\t requests: ");
    for (i = 0; i < 8; i++)
        printf("%i ", pfc->requests[i]);
    printf("\n");

    printf("\t indications: ");
    for (i = 0; i < 8; i++)
        printf("%i ", pfc->indications[i]);
    printf("\n");
}
#endif

#if 0
static struct app_prio *get_ieee_app(const char *ifname, int *cnt)
{
    int err = 0;
    int rem;
    int itr = 0;
    struct app_prio *app = NULL;
    struct sockaddr_nl dest_addr;
    static struct nl_handle *nlhandle;
    struct nl_msg *nlm;
    unsigned char *msg = NULL;
    struct nlmsghdr *hdr;
    struct nlattr *app_attr, *attr, *nattr;
    struct dcbmsg d = {
               .dcb_family = AF_UNSPEC,
               .cmd = DCB_CMD_IEEE_GET,
               .dcb_pad = 0
              };

    if (!nlhandle) {
        nlhandle = nl_handle_alloc();
        if (!nlhandle) {
            LLDPAD_WARN("%s: %s: nl_handle_alloc failed, %s\n",
                    __func__, ifname, nl_geterror());
            *cnt = 0;
            return NULL;
        }
        nl_socket_set_local_port(nlhandle, 0);
    }

    if (nl_connect(nlhandle, NETLINK_ROUTE) < 0) {
        LLDPAD_WARN("%s: %s nlconnect failed abort get ieee, %s\n",
                __func__, ifname, nl_geterror());
        goto out1;
    }

    nlm = nlmsg_alloc_simple(RTM_GETDCB, NLM_F_REQUEST);
    if (!nlm)
        goto out1;

    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    nlmsg_set_dst(nlm, &dest_addr);

    err = nlmsg_append(nlm, &d, sizeof(d), NLMSG_ALIGNTO);
    if (err < 0)
        goto out;

    err = nla_put(nlm, DCB_ATTR_IFNAME, strlen(ifname)+1, ifname);
    if (err < 0)
        goto out;

    err = nl_send_auto_complete(nlhandle, nlm);
    if (err <= 0) {
        LLDPAD_WARN("%s: %s 802.1Qaz get app attributes failed\n",
                __func__, ifname);
        goto out;
    }

    err = nl_recv(nlhandle, &dest_addr, &msg, NULL);
    if (err <= 0) {
        LLDPAD_WARN("%s: %s: nl_recv returned %d\n", __func__, ifname,
                err);
        goto out;
    }

    hdr = (struct nlmsghdr *) msg;

    attr = nlmsg_find_attr(hdr, sizeof(d), DCB_ATTR_IEEE);
    if (!attr) {
        LLDPAD_WARN("%s: %s: nlmsg_find_attr failed\n",
                __func__, ifname);
        goto out;
    }

    app = GLUE_ALLOC(sizeof(struct app_prio));
    nla_for_each_nested(nattr, attr, rem) {
        if (nla_type(nattr) != DCB_ATTR_IEEE_APP_TABLE)
            continue;

        nla_for_each_nested(app_attr, nattr, rem) {
            struct dcb_app *data = nla_data(app_attr);
            LLDPAD_DBG("app %i %i %i\n",
                data->selector,
                data->protocol,
                data->priority);
            app = realloc(app,
                      sizeof(struct app_prio) * itr +
                      sizeof(struct app_prio));
            if (!app) {
                LLDPAD_WARN("%s: %s: realloc failed\n",
                        __func__, ifname);
                goto out;
            }
            app[itr].prs = (data->priority << 5) | data->selector;
            app[itr].pid = htons(data->protocol);
            itr++;
        }
    }

out:
    nlmsg_GLUE_FREE(nlm);
    GLUE_FREE(msg);
    nl_close(nlhandle);
out1:
    *cnt = itr;
    return app;
}


static int del_ieee_hw(const char *ifname, struct dcb_app *app_data)
{
    int err = 0;
    struct nlattr *ieee, *app;
    struct sockaddr_nl dest_addr;
    static struct nl_handle *nlhandle;
    struct nl_msg *nlm;
    struct dcbmsg d = {
               .dcb_family = AF_UNSPEC,
               .cmd = DCB_CMD_IEEE_DEL,
               .dcb_pad = 0
              };

    if (!nlhandle) {
        nlhandle = nl_handle_alloc();
        if (!nlhandle) {
            LLDPAD_WARN("%s: %s: nl_handle_alloc failed, %s\n",
                    __func__, ifname, nl_geterror());
            return -ENOMEM;
        }
        nl_socket_set_local_port(nlhandle, 0);
    }

    if (nl_connect(nlhandle, NETLINK_ROUTE) < 0) {
        LLDPAD_WARN("%s: %s nlconnect failed abort hardware set, %s\n",
                __func__, ifname, nl_geterror());
        err = -EIO;
        goto out1;
    }

    nlm = nlmsg_alloc_simple(RTM_SETDCB, NLM_F_REQUEST);
    if (!nlm) {
        err = -ENOMEM;
        goto out2;
    }

    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    nlmsg_set_dst(nlm, &dest_addr);

    err = nlmsg_append(nlm, &d, sizeof(d), NLMSG_ALIGNTO);
    if (err < 0)
        goto out;

    err = nla_put(nlm, DCB_ATTR_IFNAME, strlen(ifname)+1, ifname);
    if (err < 0)
        goto out;

    ieee = nla_nest_start(nlm, DCB_ATTR_IEEE);
    if (!ieee) {
        err = -ENOMEM;
        goto out;
    }
    if (app_data) {
        app = nla_nest_start(nlm, DCB_ATTR_IEEE_APP_TABLE);
        if (!app) {
            err = -ENOMEM;
            goto out;
        }

        err = nla_put(nlm, DCB_ATTR_IEEE_APP,
                  sizeof(*app_data), app_data);
        if (err < 0)
            goto out;
        nla_nest_end(nlm, app);
    }
    nla_nest_end(nlm, ieee);
    err = nl_send_auto_complete(nlhandle, nlm);
    if (err <= 0)
        LLDPAD_WARN("%s: %s 802.1Qaz set attributes failed\n",
                __func__, ifname);

out:
    nlmsg_GLUE_FREE(nlm);
out2:
    nl_close(nlhandle);
out1:
    return err;


}

static int set_ieee_hw(const char *ifname, struct ieee_ets *ets_data,
               struct ieee_pfc *pfc_data, struct dcb_app *app_data)
{
    int err = 0;
    struct nlattr *ieee, *app;
    struct sockaddr_nl dest_addr;
    static struct nl_handle *nlhandle;
    struct nl_msg *nlm;
    struct dcbmsg d = {
               .dcb_family = AF_UNSPEC,
               .cmd = DCB_CMD_IEEE_SET,
               .dcb_pad = 0
              };

    if (!nlhandle) {
        nlhandle = nl_handle_alloc();
        if (!nlhandle) {
            LLDPAD_WARN("%s: %s: nl_handle_alloc failed, %s\n",
                    __func__, ifname, nl_geterror());
            return -ENOMEM;
        }
        nl_socket_set_local_port(nlhandle, 0);
    }

    if (!ets_data && !pfc_data && !app_data) {
        err = 0;
        goto out1;
    }

#ifdef LLDPAD_8021QAZ_DEBUG
    if (ets_data)
        print_ets(ets_data);
    if (pfc_data)
        print_pfc(pfc_data);
#endif

    if (nl_connect(nlhandle, NETLINK_ROUTE) < 0) {
        LLDPAD_WARN("%s: %s nlconnect failed abort hardware set, %s\n",
                __func__, ifname, nl_geterror());
        err = -EIO;
        goto out1;
    }

    nlm = nlmsg_alloc_simple(RTM_SETDCB, NLM_F_REQUEST);
    if (!nlm) {
        err = -ENOMEM;
        goto out2;
    }

    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    nlmsg_set_dst(nlm, &dest_addr);

    err = nlmsg_append(nlm, &d, sizeof(d), NLMSG_ALIGNTO);
    if (err < 0)
        goto out;

    err = nla_put(nlm, DCB_ATTR_IFNAME, strlen(ifname)+1, ifname);
    if (err < 0)
        goto out;

    ieee = nla_nest_start(nlm, DCB_ATTR_IEEE);
    if (!ieee) {
        err = -ENOMEM;
        goto out;
    }

    if (ets_data) {
        err = nla_put(nlm, DCB_ATTR_IEEE_ETS,
                  sizeof(*ets_data), ets_data);
        if (err < 0)
            goto out;
    }

    if (pfc_data) {
        err = nla_put(nlm, DCB_ATTR_IEEE_PFC,
                  sizeof(*pfc_data), pfc_data);
        if (err < 0)
            goto out;
    }
    if (app_data) {
        app = nla_nest_start(nlm, DCB_ATTR_IEEE_APP_TABLE);
        if (!app) {
            err = -ENOMEM;
            goto out;
        }

        err = nla_put(nlm, DCB_ATTR_IEEE_APP,
                  sizeof(*app_data), app_data);
        if (err < 0)
            goto out;
        nla_nest_end(nlm, app);
    }
    nla_nest_end(nlm, ieee);
    err = nl_send_auto_complete(nlhandle, nlm);
    if (err <= 0)
        LLDPAD_WARN("%s: %s 802.1Qaz set attributes failed\n",
                __func__, ifname);

out:
    nlmsg_GLUE_FREE(nlm);
out2:
    nl_close(nlhandle);
out1:
    return err;
}
#endif


void ets_tc_to_ieee(struct ieee_ets *ieee, TC *ptc)
{
    u8 i;

    if (NULL==ptc)
    {
        return;
    }

    for (i=0;i<MAX_TCS;i++)
    {
        ieee->tc_tx_bw[i]    = ptc[i].bw;
        ieee->tc_rx_bw[i]    = ptc[i].bw; 
        ieee->tc_tsa[i]      = ptc[i].tsa;
        ieee->tc_reco_bw[i]  = ptc[i].bw;
        ieee->tc_reco_tsa[i] = ptc[i].tsa;
    }

    for (i = 0; i < MAX_USER_PRIORITIES; i++)
    {
        ieee->prio_tc[i] = get_tc_by_pri(ptc, i);  
        ieee->reco_prio_tc[i] = get_tc_by_pri(ptc, i);  
    }

    return;
}

#if 0
static void ets_cfg_to_ieee(struct ieee_ets *ieee, struct ets_obj *cfg)
{
    u8 i;

    for (i=0;i<MAX_TCS;i++)
    {
        ieee->tc_tx_bw[i]    = cfg->tc_cfg[i].bw;
        ieee->tc_rx_bw[i]    = cfg->tc_cfg[i].bw; 
        ieee->tc_tsa[i]      = cfg->tc_cfg[i].tsa;
        ieee->tc_reco_bw[i]  = cfg->tc_cfg[i].bw;
        ieee->tc_reco_tsa[i] = cfg->tc_cfg[i].tsa;
    }

    for (i = 0; i < MAX_USER_PRIORITIES; i++)
    {
        ieee->prio_tc[i] = get_tc_by_pri(cfg->tc_cfg, i);  
        ieee->reco_prio_tc[i] = get_tc_by_pri(cfg->tc_cfg, i);  
    }

    return;
}

static void ets_rec_to_ieee(struct ieee_ets *ieee, struct etsrec_obj *rec)
{
    u8 i;

    for (i=0;i<MAX_TCS;i++)
    {
        ieee->tc_tx_bw[i]    = rec->tc[i].bw;
        ieee->tc_rx_bw[i]    = rec->tc[i].bw; 
        ieee->tc_tsa[i]      = rec->tc[i].tsa;
        ieee->tc_reco_bw[i]  = rec->tc[i].bw;
        ieee->tc_reco_tsa[i] = rec->tc[i].tsa;
    }

    for (i = 0; i < MAX_USER_PRIORITIES; i++)
    {
        ieee->prio_tc[i] = get_tc_by_pri(rec->tc, i);  
        ieee->reco_prio_tc[i] = get_tc_by_pri(rec->tc, i);  
    }

    return;    
}
#endif

void set_hw_pfc(struct ieee8021qaz_tlvs *tlvs)
{
   int ret = 0;
   u8  qos = 0;

    if (NULL==tlvs || NULL==tlvs->pfc)
    {
        return;
    }    
    
    for (qos=0; qos<MAX_USER_PRIORITIES; qos++)
    {  
        if (gtETS.pfSetHwPfc)
        {
            ret = gtETS.pfSetHwPfc(tlvs->local_port,qos,(u8)tlvs->pfc->cur.mbc,(u8)((tlvs->pfc->cur.pfc_enable & (1<<qos))?1:0),1);
        }
        
        if (ret == -1)
        {
            LLDPAD_INFO("%s: port %d: set_hw_pfc  failed\n",  __func__, tlvs->local_port);
        }  
        else
        {  
            LLDPAD_INFO("%s: port %d: set_hw_pfc  failed\n",  __func__, tlvs->local_port);
        }
    }
}   

void set_hw_pri_to_tc(u32 local_port,u8 tc)
{
    u8  pri = 0;    
    int ret = 0;    
    struct ieee8021qaz_tlvs *tlvs;
    struct lldp_port * port = NULL;
    
    port = find_by_port(local_port);
    if (!port)
    {
        return;
    }     
    
    tlvs = ieee8021qaz_data(local_port);
    if (NULL==tlvs || NULL==tlvs->ets || NULL==tlvs->ets->cfgl)
    {
        return;
    }  

    for (pri=0; pri<MAX_USER_PRIORITIES; pri++)
    {
        if (tlvs->ets->cfgl->tc_cur[tc].pri & (1<<pri))
        {
            if (gtETS.pfSetHwPri2Tc)
            {
                ret = gtETS.pfSetHwPri2Tc(local_port,tc,pri,1);
            }            
            
            if (ret == -1)
            {
                LLDPAD_INFO("%s: port %d: set_hw_pri_to_tc add failed,qos:%d,tc:%d\n",  __func__, tlvs->local_port,pri,tc);
            }  
            else
            {  
                LLDPAD_INFO("%s: port %d: set_hw_pri_to_tc add ok,qos:%d,tc:%d\n",  __func__, tlvs->local_port,pri,tc);
            }                    
        }
        else
        {   
            if (gtETS.pfSetHwPri2Tc)
            {
                ret = gtETS.pfSetHwPri2Tc(local_port,tc,pri,0);
            } 
            
            if (ret == -1)
            {
                LLDPAD_INFO("%s: port %d: set_hw_pri_to_tc del failed,qos:%d,tc:%d\n",  __func__, tlvs->local_port,pri,tc);
            }  
            else
            {  
                LLDPAD_INFO("%s: port %d: set_hw_pri_to_tc del ok,qos:%d\n,tc:%d",  __func__, tlvs->local_port,pri,tc);
            }                    
            
        }
    }
}


void set_hw_tc(struct ieee8021qaz_tlvs *tlvs)
{
    u8 tc;
    int ret = 0;

    if (NULL==tlvs || NULL==tlvs->ets || NULL==tlvs->ets->cfgl)
    {
        return;
    }

    for (tc=0; tc<MAX_TCS; tc++)
    {        
        if (tlvs->ets->cfgl->tc_cur[tc].used)
        {   
            if (gtETS.pfSetHwTc)
            {
                ret = gtETS.pfSetHwTc(tlvs->local_port,tc,tlvs->ets->cfgl->tc_cur[tc].bw,tlvs->ets->cfgl->tc_cur[tc].tsa,1);
            }

            if (ret == -1)
            {
                LLDPAD_INFO("%s: port %d: set_hw_tc add failed,tc:%d\n",  __func__, tlvs->local_port,tc);
            }  
            else
            {  
                LLDPAD_INFO("%s: port %d: set_hw_tc add ok,tc:%d\n",  __func__, tlvs->local_port,tc);
            }

            set_hw_pri_to_tc(tlvs->local_port,tc);
        }
        else
        {
            if (gtETS.pfSetHwTc)
            {
                ret = gtETS.pfSetHwTc(tlvs->local_port,tc,tlvs->ets->cfgl->tc_cur[tc].bw,tlvs->ets->cfgl->tc_cur[tc].tsa,0);
            }
            
            if (ret == -1)
            {
                LLDPAD_INFO("%s: port %d: set_hw_tc  del failed,tc:%d\n",  __func__, tlvs->local_port,tc);
            }  
            else
            {  
                LLDPAD_INFO("%s: port %d: set_hw_tc  del ok,tc:%d\n",  __func__, tlvs->local_port,tc);      
            }
        }        
    } 
}

void set_cur_tc_cfg(struct ieee8021qaz_tlvs *tlvs)
{    
    if (NULL==tlvs || NULL==tlvs->ets || NULL==tlvs->ets->cfgl)
    {
        return;
    }  

    if ((NULL!=tlvs->ets->cfgr) && (tlvs->ets->current_state == RX_RECOMMEND))
    {
        if (0!=memcmp(tlvs->ets->cfgl->tc_cur,tlvs->ets->cfgr->tc_rec,MAX_TCS*sizeof(TC)))
        {
            memcpy(tlvs->ets->cfgl->tc_cur,tlvs->ets->cfgr->tc_rec,MAX_TCS*sizeof(TC));
            set_hw_tc(tlvs);
        }
    }
    else
    {
        if (0!=memcmp(tlvs->ets->cfgl->tc_cur,tlvs->ets->cfgl->tc_cfg,MAX_TCS*sizeof(TC)))
        {
            memcpy(tlvs->ets->cfgl->tc_cur,tlvs->ets->cfgl->tc_cfg,MAX_TCS*sizeof(TC));
            set_hw_tc(tlvs);
        }
    }
}

void set_cur_pfc_cfg(struct ieee8021qaz_tlvs *tlvs)
{
    if (NULL==tlvs || NULL==tlvs->pfc)
    {
        return;
    }

    tlvs->pfc->cur.willing = tlvs->pfc->local.willing;
    tlvs->pfc->cur.mbc     = tlvs->pfc->local.mbc;    
    tlvs->pfc->cur.delay   = tlvs->pfc->local.delay;   

    if (tlvs->pfc->current_state == RX_RECOMMEND)
    {
        if ((tlvs->pfc->cur.pfc_cap!=tlvs->pfc->remote.pfc_cap) ||
            (tlvs->pfc->cur.pfc_enable!=tlvs->pfc->remote.pfc_enable))
        {
            tlvs->pfc->cur.pfc_cap    = tlvs->pfc->remote.pfc_cap;
            tlvs->pfc->cur.pfc_enable = tlvs->pfc->remote.pfc_enable;
            set_hw_pfc(tlvs);
        }
    }
    else
    {
        if ((tlvs->pfc->cur.pfc_cap!=tlvs->pfc->local.pfc_cap) ||
            (tlvs->pfc->cur.pfc_enable!=tlvs->pfc->local.pfc_enable))
        {
            tlvs->pfc->cur.pfc_cap    = tlvs->pfc->local.pfc_cap;
            tlvs->pfc->cur.pfc_enable = tlvs->pfc->local.pfc_enable;
            set_hw_pfc(tlvs);            
        }
    }    

}

void run_all_sm(struct lldp_port *port)
{
    struct ieee8021qaz_tlvs *tlvs;
    struct ieee_ets *ets;
    struct ieee_pfc *pfc;
    struct pfc_obj *pfc_obj;


    tlvs = ieee8021qaz_data(port->local_port);
    if (!tlvs || !tlvs->ets || !tlvs->ets->cfgl || !tlvs->pfc)
    {
        return;
    }

    if (is_tlv_enabled(port->local_port, TLVID_8021(LLDP_8021QAZ_ETSCFG))) 
    {
        ets_sm(tlvs->ets->cfgl, tlvs->ets->cfgr, &tlvs->ets->current_state);
    }

    ets = (struct ieee_ets *)GLUE_ALLOC(sizeof(struct ieee_ets));
    if (!ets) 
    {
        LLDPAD_WARN("%s: port %d: ets GLUE_ALLOC failed\n",  __func__, port->local_port);
        return;
    }

    memset(ets, 0, sizeof(struct ieee_ets));

    if (tlvs->ets->cfgr && (tlvs->ets->current_state == RX_RECOMMEND))
    {
        ets_tc_to_ieee(ets, tlvs->ets->cfgr->tc_rec);
    }
    else
    {
        ets_tc_to_ieee(ets, tlvs->ets->cfgl->tc_cfg);
    }

    set_cur_tc_cfg(tlvs);

    if (is_tlv_enabled(port->local_port, TLVID_8021(LLDP_8021QAZ_PFC)))    
    {
        pfc_sm(tlvs);
    }

    if (tlvs->pfc->current_state == RX_RECOMMEND)
    {
        pfc_obj = &tlvs->pfc->remote;
    }
    else
    {
        pfc_obj = &tlvs->pfc->local;
    }

    pfc = (struct ieee_pfc *)GLUE_ALLOC(sizeof(struct ieee_pfc));
    if (!pfc) 
    {
        LLDPAD_WARN("%s: port %d: pfc GLUE_ALLOC failed\n", __func__, port->local_port);
        goto out;
    }

    set_cur_pfc_cfg(tlvs);

    memset(pfc, 0, sizeof(struct ieee_pfc));

    pfc->pfc_en = pfc_obj->pfc_enable;
    pfc->mbc = pfc_obj->mbc;
    pfc->delay = pfc_obj->delay;

    if (ieee8021qaz_check_active(port->local_port)) 
    {
        #if 0
        set_dcbx_mode(port->ifname,
                  DCB_CAP_DCBX_VER_IEEE | DCB_CAP_DCBX_HOST);
        set_ieee_hw(port->ifname, ets, pfc, NULL);
        #endif
        ieee8021qaz_app_sethw(port->local_port, &tlvs->app_head);
    }

out:
    GLUE_FREE(pfc);
    GLUE_FREE(ets);
}

/*
 * bld_ieee8021qaz_etscfg_tlv - builds the ETS Configuration TLV
 * Returns 1 on success, NULL if the TLV fail to build correctly.
 */
static struct unpacked_tlv *
bld_ieee8021qaz_etscfg_tlv(struct ieee8021qaz_tlvs *tlvs)
{
    struct ieee8021qaz_tlv_etscfg *etscfg;
    struct unpacked_tlv *tlv = NULL;
    int i = 0;

    if (NULL==tlvs || NULL==tlvs->ets)
    {
        return NULL;
    }

    tlv = create_tlv();
    if (!tlv)
    {
        return NULL;
    }

    etscfg = (struct ieee8021qaz_tlv_etscfg *)GLUE_ALLOC(sizeof(struct ieee8021qaz_tlv_etscfg));
    if (!etscfg) 
    {
        LLDPAD_WARN("%s: Failed to GLUE_ALLOC etscfg\n", __func__);
        GLUE_FREE(tlv);        
        return NULL;
    }
    memset(etscfg, 0, sizeof(struct ieee8021qaz_tlv_etscfg));

    hton24(etscfg->oui, OUI_IEEE_8021);
    etscfg->subtype = LLDP_8021QAZ_ETSCFG;
    etscfg->wcrt = tlvs->ets->cfgl->willing << 7 |
                   tlvs->ets->cfgl->cbs << 6 |
                  (tlvs->ets->cfgl->max_tcs & 0x7);

    if (tlvs->ets->current_state == STATE_INIT) 
    {
        if (NULL==tlvs->ets->cfgl)
        {
            GLUE_FREE(tlv);
            GLUE_FREE(etscfg);
            return NULL;
        }
        
        etscfg->prio_map = EVB_HTONL(get_priomap_by_tcs(tlvs->ets->cfgl->tc_cfg));
        for (i = 0; i < MAX_TCS; i++)
        {
            etscfg->tc_bw[i] = tlvs->ets->cfgl->tc_cfg[i].bw;
            etscfg->tsa_map[i] = tlvs->ets->cfgl->tc_cfg[i].tsa;
        }
    } 
    else 
    {
        if (NULL==tlvs->ets->cfgr)
        {
            GLUE_FREE(tlv);
            GLUE_FREE(etscfg);
            return NULL;
        }
        
        etscfg->prio_map = EVB_HTONL(get_priomap_by_tcs(tlvs->ets->cfgr->tc_rec));
        for (i = 0; i < MAX_TCS; i++)
        {
            etscfg->tc_bw[i] = tlvs->ets->cfgr->tc_rec[i].bw;
            etscfg->tsa_map[i] = tlvs->ets->cfgr->tc_rec[i].tsa;
        }
    }

    tlv->type = ORG_SPECIFIC_TLV;
    tlv->length = sizeof(struct ieee8021qaz_tlv_etscfg);
    tlv->info = (u8 *)etscfg;

    if (OUI_SUB_SIZE > tlv->length)
    {
        goto error;
    }

    return tlv;

error:
    if (tlv) 
    {
        if (tlv->info)
        {
            GLUE_FREE(tlv->info);
        }
        
        GLUE_FREE(tlv);
    }

    LLDPAD_WARN("%s: Failed\n", __func__);
    return NULL;
}

/*
 * bld_ieee8021qaz_etsrec_tlv - builds the ETS Recommendation TLV
 * Returns 1 on success, NULL if the TLV fail to build correctly.
 */
struct unpacked_tlv *
bld_ieee8021qaz_etsrec_tlv(struct ieee8021qaz_tlvs *tlvs)
{
    struct ieee8021qaz_tlv_etsrec *etsrec;
    struct unpacked_tlv *tlv = NULL;
    int i = 0;
    
    if (NULL==tlvs || NULL==tlvs->ets || NULL==tlvs->ets->cfgl)
    {
        return NULL;
    }

    tlv = create_tlv();
    if (NULL==tlv)
    {
        return NULL;
    }

    etsrec = (struct ieee8021qaz_tlv_etsrec *)GLUE_ALLOC(sizeof(struct ieee8021qaz_tlv_etsrec));
    if (!etsrec) 
    {
        LLDPAD_WARN("%s: Failed to GLUE_ALLOC etscfg\n", __func__);
        GLUE_FREE(tlv);
        return NULL;
    }
    
    memset(etsrec, 0, sizeof(struct ieee8021qaz_tlv_etsrec));

    hton24(etsrec->oui, OUI_IEEE_8021);
    etsrec->subtype = LLDP_8021QAZ_ETSREC;
    etsrec->prio_map = EVB_HTONL(get_priomap_by_tcs(tlvs->ets->cfgl->tc_rec));

    for (i = 0; i < MAX_TCS; i++) 
    {
        etsrec->tc_bw[i] = tlvs->ets->cfgl->tc_rec[i].bw;
        etsrec->tsa_map[i] = tlvs->ets->cfgl->tc_rec[i].tsa;
    }

    tlv->type = ORG_SPECIFIC_TLV;
    tlv->length = sizeof(struct ieee8021qaz_tlv_etsrec);
    tlv->info = (u8 *)etsrec;

    if (OUI_SUB_SIZE > tlv->length)
    {
        goto error;
    }

    return tlv;

error:
    if (tlv) 
    {
        if (tlv->info)
        {
            GLUE_FREE(tlv->info);
        }
        
        GLUE_FREE(tlv);
    }
    LLDPAD_WARN("%s: Failed\n", __func__);
    return NULL;
}

/*
 * bld_ieee8021qaz_pfc_tlv - builds the PFC Control Configuration TLV
 * Returns unpacket tlv or NULL if the TLV fails to build correctly.
 */
static struct unpacked_tlv *
bld_ieee8021qaz_pfc_tlv(struct ieee8021qaz_tlvs *tlvs)
{
    struct ieee8021qaz_tlv_pfc *pfc;
    struct unpacked_tlv *tlv = NULL;

    if (NULL==tlvs || NULL==tlvs->pfc)
    {
        return NULL;
    }    

    tlv = create_tlv();
    if (NULL==tlv)
    {
        return NULL;
    }

    pfc = (struct ieee8021qaz_tlv_pfc *)GLUE_ALLOC(sizeof(struct ieee8021qaz_tlv_pfc));
    if (!pfc) 
    {
        LLDPAD_WARN("%s: Failed to GLUE_ALLOC pfc\n", __func__);
        GLUE_FREE(tlv);
        return NULL;
    }

    memset(pfc, 0, sizeof(struct ieee8021qaz_tlv_pfc));

    hton24(pfc->oui, OUI_IEEE_8021);
    pfc->subtype = LLDP_8021QAZ_PFC;

    pfc->wmrc = tlvs->pfc->local.willing << 7 |
                tlvs->pfc->local.mbc << 6 |
                tlvs->pfc->local.pfc_cap;

    if (tlvs->pfc->current_state == STATE_INIT) 
    {
        pfc->pfc_enable = tlvs->pfc->local.pfc_enable;
    }
    else 
    {
        pfc->pfc_enable = tlvs->pfc->remote.pfc_enable;
    }

    tlv->type = ORG_SPECIFIC_TLV;
    tlv->length = sizeof(struct ieee8021qaz_tlv_pfc);
    tlv->info = (u8 *)pfc;

    if (OUI_SUB_SIZE > tlv->length)
    {
        goto error;
    }

    return tlv;
error:
    if (tlv) 
    {
        if (tlv->info)
        {
            GLUE_FREE(tlv->info);
        }
        
        GLUE_FREE(tlv);
    }
    LLDPAD_WARN("%s: Failed\n", __func__);
    return NULL;
}

/*
 * bld_ieee8021qaz_app_tlv - builds the APP TLV
 * Returns unacked tlv or NULL if the TLV fails to build correctly.
 */
static struct unpacked_tlv *bld_ieee8021qaz_app_tlv(u32 local_port)
{
#if 0    
    struct ieee8021qaz_tlv_app *app = NULL;
    struct unpacked_tlv *tlv;
    struct app_prio *data = NULL;
    __u8 *ptr;
    int cnt;

    tlv = create_tlv();
    if (!tlv)
        return NULL;

    data = get_ieee_app(ifname, &cnt);
    if (!data) {
        LLDPAD_WARN("%s: %s: get_ieee_app failed\n", __func__, ifname);
        goto error;
    }

    app = GLUE_ALLOC(sizeof(*app) + (sizeof(*data) * cnt));
    if (!app) {
        LLDPAD_WARN("%s: Failed to GLUE_ALLOC app\n", __func__);
        goto error;
    }

    memset(app, 0, sizeof(*app));
    hton24(app->oui, OUI_IEEE_8021);
    app->subtype = LLDP_8021QAZ_APP;

    ptr = (u8 *) app + sizeof(*app);
    memcpy(ptr, data, sizeof(*data) * cnt);

    tlv->type = ORG_SPECIFIC_TLV;
    tlv->length = sizeof(struct ieee8021qaz_tlv_app) + (cnt * 3);
    tlv->info = (u8 *)app;

    if (OUI_SUB_SIZE > tlv->length) {
        LLDPAD_WARN("%s: %s: tlv->length = %d, cnt=%d\n", __func__,
                ifname, tlv->length, cnt);
        goto error;
    }

    GLUE_FREE(data);

    return tlv;

error:
    GLUE_FREE(tlv);
    GLUE_FREE(app);
    GLUE_FREE(data);
    LLDPAD_WARN("%s: Failed\n", __func__);
#endif    
    return NULL;
}

/*
 * ieee8021qaz_bld_tlv - builds all IEEE8021QAZ TLVs
 * Returns 1 on success, NULL if any of the TLVs fail to build correctly.
 */
static struct packed_tlv *ieee8021qaz_bld_tlv(struct lldp_port *port)
{
    struct ieee8021qaz_tlvs *data;
    struct packed_tlv *ptlv = NULL;
    struct unpacked_tlv *etscfg_tlv = NULL;
    struct unpacked_tlv *etsrec_tlv = NULL;
    struct unpacked_tlv *pfc_tlv = NULL;
    struct unpacked_tlv *app_tlv = NULL;
    u16 size;

    data = ieee8021qaz_data(port->local_port);
    if (!data)
    {
        goto err;
    }

#if 0
    if (!data->active)
    {
        goto err;
    }
#endif    

    if (is_tlv_enabled(port->local_port, TLVID_8021(LLDP_8021QAZ_ETSCFG)))
    {
        etscfg_tlv = bld_ieee8021qaz_etscfg_tlv(data);
    }
    
    if (is_tlv_enabled(port->local_port, TLVID_8021(LLDP_8021QAZ_ETSREC)))
    {
        etsrec_tlv = bld_ieee8021qaz_etsrec_tlv(data);
    }
    
    if (is_tlv_enabled(port->local_port, TLVID_8021(LLDP_8021QAZ_PFC)))
    {
        pfc_tlv = bld_ieee8021qaz_pfc_tlv(data);
    }
    
    if (is_tlv_enabled(port->local_port, TLVID_8021(LLDP_8021QAZ_APP)))
    {
        app_tlv = bld_ieee8021qaz_app_tlv(port->local_port);
    }

    size = TLVSIZE(etscfg_tlv)
         + TLVSIZE(etsrec_tlv)
         + TLVSIZE(pfc_tlv)
         + TLVSIZE(app_tlv);

    if (0==size)
    {
        goto err;
    }

    ptlv = create_ptlv();
    if (!ptlv)
    {
        goto err;
    }

    ptlv->tlv = (u8 *)GLUE_ALLOC(size);
    if (!ptlv->tlv)
    {
        goto err;
    }

    ptlv->size = 0;
    PACK_TLV_AFTER(etscfg_tlv, ptlv, size, err);
    PACK_TLV_AFTER(etsrec_tlv, ptlv, size, err);
    PACK_TLV_AFTER(pfc_tlv, ptlv, size, err);
    PACK_TLV_AFTER(app_tlv, ptlv, size, err);

    free_unpkd_tlv(etscfg_tlv);
    free_unpkd_tlv(etsrec_tlv);
    free_unpkd_tlv(pfc_tlv);
    free_unpkd_tlv(app_tlv);    

    return ptlv;
    
err:
    free_pkd_tlv(ptlv); 
    free_unpkd_tlv(etscfg_tlv);
    free_unpkd_tlv(etsrec_tlv);
    free_unpkd_tlv(pfc_tlv);
    free_unpkd_tlv(app_tlv);

    return NULL;
    
}

/* LLDP_8021QAZ_MOD_OPS - GETTLV */
struct packed_tlv *ieee8021qaz_gettlv(struct lldp_port *port)
{
    struct packed_tlv *ptlv = NULL;

    /* Update TLV State Machines */
    run_all_sm(port);
    /* Build TLVs */
    ptlv = ieee8021qaz_bld_tlv(port);
    return ptlv;
}

static bool unpack_ieee8021qaz_tlvs(struct lldp_port *port, struct unpacked_tlv *tlv, bool *tlv_store)
{
    /* Unpack tlvs and store in rx */
    struct ieee8021qaz_tlvs *tlvs;

    if (NULL==port || NULL==tlv || NULL==tlv_store)
    {
        return false;
    }    

    *tlv_store = false;    

    tlvs = ieee8021qaz_data(port->local_port);
    if (NULL==tlvs || NULL==tlvs->rx)
    {
        return false;
    }    

    /* Process */
    switch (tlv->info[OUI_SIZE]) {
    case IEEE8021QAZ_ETSCFG_TLV:
        if (!is_tlv_enabled(port->local_port, TLVID_8021(LLDP_8021QAZ_ETSCFG)))    
        {
            return true;            
        }
        
        if (tlvs->rx->etscfg) 
        {
            LLDPAD_WARN("%s: port %d: 802.1Qaz Duplicate ETSCFG TLV\n", __func__, port->local_port);
            port->rx.dupTlvs |= DUP_IEEE8021QAZ_TLV_ETSCFG;
            return false;
        }
        
        tlvs->ieee8021qazdu |= RCVD_IEEE8021QAZ_TLV_ETSCFG;
        tlvs->rx->etscfg = tlv;        
        break;
    case IEEE8021QAZ_ETSREC_TLV:
        if (!is_tlv_enabled(port->local_port, TLVID_8021(LLDP_8021QAZ_ETSREC)))    
        {
            return true;                        
        }    
        
        if (tlvs->rx->etsrec)
        {
            LLDPAD_WARN("%s: port %d: 802.1Qaz Duplicate ETSREC TLV\n", __func__, port->local_port);
            port->rx.dupTlvs |= DUP_IEEE8021QAZ_TLV_ETSREC;
            return false;            
        }
        
        tlvs->ieee8021qazdu |= RCVD_IEEE8021QAZ_TLV_ETSREC;
        tlvs->rx->etsrec = tlv;        
        break;
    case IEEE8021QAZ_PFC_TLV:
        if (!is_tlv_enabled(port->local_port, TLVID_8021(LLDP_8021QAZ_PFC)))    
        {
            return true;
        }        
        
        if (tlvs->rx->pfc)
        {
            LLDPAD_WARN("%s: port %d: 802.1Qaz Duplicate PFC TLV\n",    __func__, port->local_port);
            port->rx.dupTlvs |= DUP_IEEE8021QAZ_TLV_PFC;
            return false;            
        }
        
        tlvs->ieee8021qazdu |= RCVD_IEEE8021QAZ_TLV_PFC;
        tlvs->rx->pfc = tlv;        
        break;
    case IEEE8021QAZ_APP_TLV:
        if (!is_tlv_enabled(port->local_port, TLVID_8021(LLDP_8021QAZ_APP)))    
        {
            return true;            
        }        
        
        if (tlvs->rx->app)
        {
            LLDPAD_WARN("%s: port %d: 802.1Qaz Duplicate APP TLV\n", __func__, port->local_port);
            port->rx.dupTlvs |= DUP_IEEE8021QAZ_TLV_APP;
            return false;            
        }
        
        tlvs->ieee8021qazdu |= RCVD_IEEE8021QAZ_TLV_APP;
        tlvs->rx->app = tlv;        
        break;
    default:
        return true;
    }

    *tlv_store = true;
    return true;
}

void clear_ieee8021qaz_rx(struct ieee8021qaz_tlvs *tlvs)
{
    if (!tlvs)
    {
        return;
    }

    if (!tlvs->rx)
    {
        return;
    }

    ieee8021qaz_free_rx(tlvs->rx);

    GLUE_FREE(tlvs->rx);
    tlvs->rx = NULL;
}

static void process_ieee8021qaz_etscfg_tlv(struct lldp_port *port)
{
    struct ieee8021qaz_tlvs *tlvs;
    u8 offset = 0;
    int i = 0;

    if (NULL==port)
    {
        LLDPAD_WARN("%s: port is null\n", __func__);        
        return;
    }

    if (!is_tlv_enabled(port->local_port, TLVID_8021(LLDP_8021QAZ_ETSCFG)))
    {
        LLDPAD_WARN("%s: not support LLDP_8021QAZ_ETSCFG\n", __func__);        
        return;
    }

    tlvs = ieee8021qaz_data(port->local_port);
    offset = OUI_SUB_SIZE;

    if (tlvs->ets->cfgr)
    {
        GLUE_FREE(tlvs->ets->cfgr);
    }
    
    tlvs->ets->cfgr = (struct ets_obj *)GLUE_ALLOC(sizeof(struct ets_obj));
    if (!tlvs->ets->cfgr) 
    {
        LLDPAD_WARN("%s: port %d: cfgr GLUE_ALLOC failed\n", __func__, port->local_port);
        return;
    }

    memset(tlvs->ets->cfgr,0,sizeof(struct ets_obj));

    if (tlvs->rx->etscfg->info[offset] & BIT7)
    {
        tlvs->ets->cfgr->willing = true;
    }
    else
    {
        tlvs->ets->cfgr->willing = false;
    }

    if (tlvs->rx->etscfg->info[offset] & BIT6)
    {
        tlvs->ets->cfgr->cbs = true;
    }
    else
    {
        tlvs->ets->cfgr->cbs = false;
    }

    tlvs->ets->cfgr->max_tcs = tlvs->rx->etscfg->info[offset] & 0x07;

    /*Moving offset to PRIO_MAP */
    offset += 1;
    /*tlvs->ets->cfgr->prio_map = 0;*/
    for (i = 0; i < 4; i++) 
    {
        u8 temp1 = 0, temp2 = 0;

        temp1 = (tlvs->rx->etscfg->info[offset] >> 4) & 0x0F;
        temp2 = tlvs->rx->etscfg->info[offset] & 0x0F;

        set_tc_pri(tlvs->ets->cfgr->tc_cfg,(u8)(2*i), (int)temp1);
        set_tc_pri(tlvs->ets->cfgr->tc_cfg,(u8)((2*i)+1),(int)temp2);
        tlvs->ets->cfgr->tc_cfg[temp1].used = 1;
        tlvs->ets->cfgr->tc_cfg[temp2].used = 1;        

        set_tc_pri(tlvs->ets->cfgr->tc_cur,(u8)(2*i), (int)temp1);
        set_tc_pri(tlvs->ets->cfgr->tc_cur,(u8)((2*i)+1),(int)temp2);
        tlvs->ets->cfgr->tc_cur[temp1].used = 1;
        tlvs->ets->cfgr->tc_cur[temp2].used = 1;    
        
        offset += 1;
    }

    for (i = 0; i < MAX_TCS; i++) 
    {
        tlvs->ets->cfgr->tc_cfg[i].bw = tlvs->rx->etscfg->info[offset];
        tlvs->ets->cfgr->tc_cur[i].bw = tlvs->rx->etscfg->info[offset];
        offset += 1;
    }

    for (i = 0; i < MAX_TCS; i++) 
    {
        tlvs->ets->cfgr->tc_cfg[i].tsa = tlvs->rx->etscfg->info[offset];
        tlvs->ets->cfgr->tc_cur[i].tsa = tlvs->rx->etscfg->info[offset];
        offset += 1;
    }

}

static void process_ieee8021qaz_etsrec_tlv(struct lldp_port *port)
{
    struct ieee8021qaz_tlvs *tlvs = NULL;
    u8 offset = 0;
    int i = 0;

    if (NULL==port)
    {
        LLDPAD_WARN("%s: port is null\n", __func__);        
        return;
    }

    if (!is_tlv_enabled(port->local_port, TLVID_8021(LLDP_8021QAZ_ETSREC)))
    {
        LLDPAD_WARN("%s: not support LLDP_8021QAZ_ETSREC\n", __func__);        
        return;
    }    

    tlvs = ieee8021qaz_data(port->local_port);

    /* Bypassing OUI, SUBTYPE fields */
    offset = OUI_SUB_SIZE + 1;

    if (NULL==tlvs || NULL==tlvs->ets ||NULL==tlvs->ets->cfgr)
    {
        return;
    }

    memset(tlvs->ets->cfgr->tc_rec, 0, MAX_TCS*sizeof(TC));

    /*tlvs->ets->recr->prio_map = 0;*/
    for (i = 0; i < 4; i++) 
    {
        u8 temp1 = 0, temp2 = 0;

        temp1 = (tlvs->rx->etsrec->info[offset] >> 4) & 0x0F;
        temp2 = tlvs->rx->etsrec->info[offset] & 0x0F;

        set_tc_pri(tlvs->ets->cfgr->tc_rec,(u8)(2*i), (int)temp1);
        set_tc_pri(tlvs->ets->cfgr->tc_rec,(u8)((2*i)+1),(int)temp2);
        tlvs->ets->cfgr->tc_rec[temp1].used = 1;        
        tlvs->ets->cfgr->tc_rec[temp2].used = 1;         

        offset += 1;
    }

    for (i = 0; i < MAX_TCS; i++) 
    {
        tlvs->ets->cfgr->tc_rec[i].bw = tlvs->rx->etsrec->info[offset];
        offset += 1;
    }

    for (i = 0; i < MAX_TCS; i++) 
    {
        tlvs->ets->cfgr->tc_rec[i].tsa = tlvs->rx->etsrec->info[offset];
        offset += 1;
    }
}

static void process_ieee8021qaz_pfc_tlv(struct lldp_port *port)
{
    struct ieee8021qaz_tlvs *tlvs;
    u8 offset = 0;

    if (NULL==port)
    {
        LLDPAD_WARN("%s: port is null\n", __func__);        
        return;
    }

    if (!is_tlv_enabled(port->local_port, TLVID_8021(LLDP_8021QAZ_PFC)))
    {
        LLDPAD_WARN("%s: not support LLDP_8021QAZ_PFC\n", __func__);        
        return;
    }        

    tlvs = ieee8021qaz_data(port->local_port);

    /* Bypassing OUI, SUBTYPE fields */
    offset = OUI_SUB_SIZE;
    if (tlvs->rx->pfc->info[offset] & BIT7)
    {
        tlvs->pfc->remote.willing = true;
    }
    else
    {
        tlvs->pfc->remote.willing = false;
    }

    if (tlvs->rx->pfc->info[offset] & BIT6)
    {
        tlvs->pfc->remote.mbc = true;
    }
    else
    {
        tlvs->pfc->remote.mbc = false;
    }

    tlvs->pfc->remote.pfc_cap = tlvs->rx->pfc->info[offset] & 0x0F;

    offset += 1;
    tlvs->pfc->remote.pfc_enable = tlvs->rx->pfc->info[offset];

    tlvs->pfc->remote_param = true;
}

int ieee8021qaz_mod_app(struct app_tlv_head *head, int peer,
            u8 prio, u8 sel, u16 proto, u32 ops)
{
    struct app_obj *np;

    /* Search list for existing match and abort
     * Mark entry for deletion if delete option supplied
     */
    LIST_FOREACH(np, head, entry) 
    {
        if (np->app.selector == sel &&
            np->app.protocol == proto &&
            np->app.priority == prio) 
        {
            if (ops & op_delete)
            {
                np->hw = IEEE_APP_DEL;
            }
            return 1;
        }
    }

    if (ops & op_delete)
    {
        return 1;
    }

    /* Add new entry for APP data */
    np = (struct app_obj *)calloc(1, sizeof(*np));
    if (!np) 
    {
        LLDPAD_WARN("%s: memory alloc failure.\n", __func__);
        return -1;
    }

    np->peer = peer;
    np->hw = IEEE_APP_SET;
    np->app.priority = prio;
    np->app.selector = sel;
    np->app.protocol = proto;

    LIST_INSERT_HEAD(head, np, entry);
    return 0;
}

void ieee8021qaz_app_reset(struct app_tlv_head *head)
{
    struct app_obj *np;

    LIST_FOREACH(np, head, entry) 
    {
        if (np->peer)
        {
            np->hw = IEEE_APP_DEL;
        }
    }
}

static int __ieee8021qaz_app_sethw(u32 local_port, struct app_tlv_head *head)
{
    struct app_obj *np, *np_tmp;
    int set = 0;

    LIST_FOREACH(np, head, entry)
    {
        if (np->hw != IEEE_APP_SET)
        {
            continue;
        }
        #if 0
        set = set_ieee_hw(ifname, NULL, NULL, &np->app);
        #endif
        np->hw = IEEE_APP_DONE;
    }

    np = LIST_FIRST(head);
    while (np)
    {
        if (np->hw == IEEE_APP_DEL) 
        {
            np_tmp = np;
            np = LIST_NEXT(np, entry);
            LIST_REMOVE(np_tmp, entry);
            #if 0
            set = del_ieee_hw(ifname, &np_tmp->app);
            #endif
            GLUE_FREE(np_tmp);
        } 
        else 
        {
            np = LIST_NEXT(np, entry);
        }
    }
    return set;
}

int ieee8021qaz_app_sethw(u32 local_port, struct app_tlv_head *head)
{
    if (ieee8021qaz_check_active(local_port)) 
    {
        #if 0
        set_dcbx_mode(ifname,
                  DCB_CAP_DCBX_VER_IEEE | DCB_CAP_DCBX_HOST);
        #endif
        return __ieee8021qaz_app_sethw(local_port, head);
    }
    return 0;
}

static void process_ieee8021qaz_app_tlv(struct lldp_port *port)
{
    struct ieee8021qaz_tlvs *tlvs;
    int offset = OUI_SUB_SIZE + 1;

    if (NULL==port)
    {
        LLDPAD_WARN("%s: port is null\n", __func__);        
        return;
    }

    if (!is_tlv_enabled(port->local_port, TLVID_8021(LLDP_8021QAZ_APP)))
    {
        LLDPAD_WARN("%s: not support LLDP_8021QAZ_APP\n", __func__);        
        return;
    }  

    tlvs = ieee8021qaz_data(port->local_port);

    /* clear app priorities so old data is flushed */
    ieee8021qaz_app_reset(&tlvs->app_head);

    while (offset < tlvs->rx->app->length) 
    {
        struct app_obj *np;
        int set = 0;
        u8 prio  = (tlvs->rx->app->info[offset] & 0xE0) >> 5;
        u8 sel = (tlvs->rx->app->info[offset] & 0x03);
        u16 proto = (tlvs->rx->app->info[offset + 1] << 8) |
                 tlvs->rx->app->info[offset + 2];

        /* Search list for existing match and mark set */
        LIST_FOREACH(np, &tlvs->app_head, entry) 
        {
            if (np->app.selector == sel &&
                np->app.protocol == proto &&
                np->app.priority == prio) 
            {
                np->hw = IEEE_APP_SET;
                set = 1;
                break;
            }
        }

        /* If APP data not found in LIST add APP entry */
        if (!set)
        {
            ieee8021qaz_mod_app(&tlvs->app_head, 1, prio, sel, proto, 0);
        }
        offset += 3;
    }
}

void ieee8021qaz_mibUpdateObjects(struct lldp_port *port)
{
    struct ieee8021qaz_tlvs *tlvs;

    if (NULL==port)
    {
        return;
    }    

    tlvs = ieee8021qaz_data(port->local_port);
    if (NULL==tlvs || NULL==tlvs->rx)
    {
        return;
    }    

    if (tlvs->rx->etscfg)
    {
        process_ieee8021qaz_etscfg_tlv(port);
    }
    else
    {
        if (tlvs->ets && tlvs->ets->cfgr) 
        {
            GLUE_FREE(tlvs->ets->cfgr);
            tlvs->ets->cfgr = NULL;
        }
    }        

    if (tlvs->rx->etsrec)
    {
        process_ieee8021qaz_etsrec_tlv(port);
    }
    else
    {
        if (tlvs->ets && tlvs->ets->cfgr) 
        {        
            memset(tlvs->ets->cfgr->tc_rec, 0, MAX_TCS*sizeof(TC));
        }
    }

    if (tlvs->rx->pfc)
    {
        process_ieee8021qaz_pfc_tlv(port);
    }
    else
    {
        if (tlvs->pfc) 
        {
            GLUE_FREE(tlvs->pfc);
            tlvs->pfc = NULL;
        }        
    }

    if (tlvs->rx->app)
    {
        process_ieee8021qaz_app_tlv(port);
    }
    else
    {
       /* Clear peer Application data */
        ieee8021qaz_app_reset(&tlvs->app_head); 
    }
}

/*
 * LLDP_8021_QAZ_MOD_OPS - RCHANGE
 *
 * TLVs not consumed on error otherwise it is either free'd or stored
 * internally in the module.
 */
int ieee8021qaz_rchange(struct lldp_port *port, struct unpacked_tlv *tlv, bool *tlv_store)
{
    u8 oui[OUI_SIZE] = INIT_IEEE8021_OUI;
    struct ieee8021qaz_tlvs *qaz_tlvs;
    struct ieee8021qaz_unpkd_tlvs *rx;

    qaz_tlvs = ieee8021qaz_data(port->local_port);
    if (!qaz_tlvs)
    {
        return SUBTYPE_INVALID;
    }

    /*对于TYPE_0，TYPE_1需要处理*/
    if (tlv->type == TYPE_0) 
    {
        if (qaz_tlvs->active && !qaz_tlvs->ieee8021qazdu) 
        {
            qaz_tlvs->active  = false;
            qaz_tlvs->pending = false;
            LLDPAD_INFO("IEEE DCBX on port %d going INACTIVE\n", qaz_tlvs->local_port);
            ieee8021qaz_mibDeleteObject(port);
        }

        if (!qaz_tlvs->active && qaz_tlvs->ieee8021qazdu)
        {
            qaz_tlvs->active  = true;
            qaz_tlvs->pending = true;
            LLDPAD_INFO("IEEE DCBX on port %d going ACTIVE\n", qaz_tlvs->local_port);
        }

        if (qaz_tlvs->active) 
        {
            /* If peer is DCBX, then go into RXTX mode
             * if current configuration is RXOnly and
             * not persistant (i.e. default)
             */
            #if 0
            long adminstatus;
            if (qaz_tlvs->ieee8021qazdu &&
                #if 0
                get_config_setting(qaz_tlvs->local_port,
                           ARG_ADMINSTATUS,
                           (void *)&adminstatus,
                           CONFIG_TYPE_INT) &&
                #endif
                get_lldp_port_admin(qaz_tlvs->local_port) ==  enabledRxOnly) 
            {
                adminstatus = enabledRxTx;
                #if 0
                if (set_config_setting(qaz_tlvs->local_port, ARG_ADMINSTATUS, (void *)&adminstatus, CONFIG_TYPE_INT) == cmd_success)
                #endif
                    set_lldp_port_admin(qaz_tlvs->local_port, (u8)adminstatus);
            }
            #endif
            
            if (qaz_tlvs->ieee8021qazdu)
            {
                qaz_tlvs->pending = false;
            }

            /* Update TLV State Machines */
            ieee8021qaz_mibUpdateObjects(port);
            clear_ieee8021qaz_rx(qaz_tlvs);
            run_all_sm(port);
            somethingChangedLocal(port->local_port);            
        }
      
    }
    
    /*
     * TYPE_1 mandatory and always before IEEE8021QAZ tlvs so
     * we can use it to make the beginning of a IEEE8021QAZ PDU.
     * Verifies that only a single IEEE8021QAZ tlv is present.
     */
    if (tlv->type == TYPE_1) 
    {
        clear_ieee8021qaz_rx(qaz_tlvs);
        rx = (struct ieee8021qaz_unpkd_tlvs *)GLUE_ALLOC(sizeof(struct ieee8021qaz_unpkd_tlvs));
        memset(rx, 0, sizeof(struct ieee8021qaz_unpkd_tlvs));
        qaz_tlvs->rx = rx;
        qaz_tlvs->ieee8021qazdu = 0;
    }

    /*存储过就不往下走了*/
    if (true==*tlv_store)
    {
        return TLV_OK;
    }        

    /*
     * TYPE_127 is for the Organizationally Specific TLVS
     * More than 1 of this type is allowed.
     */
    if (tlv->type == TYPE_127) 
    {
        int res;

        if (tlv->length < (OUI_SUB_SIZE))
        {
            return TLV_ERR;
        }

        if ((memcmp(tlv->info, &oui, OUI_SIZE) != 0))
        {
            return SUBTYPE_INVALID;
        }

        l2_packet_get_remote_addr(port->l2, qaz_tlvs->remote_mac);
        res = unpack_ieee8021qaz_tlvs(port, tlv, tlv_store);
        if (!res)
        {
            LLDPAD_WARN("Error unpacking 8021 tlvs\n");
            return TLV_ERR;
        }

        return TLV_OK;
    }

    return TLV_OK;
}

void ieee8021qaz_free_rx(struct ieee8021qaz_unpkd_tlvs *rx)
{
    if (!rx)
    {
        return;
    }

    if (rx->ieee8021qaz)
    {
        rx->ieee8021qaz = free_unpkd_tlv(rx->ieee8021qaz);
    }
    if (rx->etscfg)
    {
        rx->etscfg = free_unpkd_tlv(rx->etscfg);
    }
    if (rx->etsrec)
    {
        rx->etsrec = free_unpkd_tlv(rx->etsrec);
    }
    if (rx->pfc)
    {
        rx->pfc = free_unpkd_tlv(rx->pfc);
    }
    if (rx->app)
    {
        rx->app = free_unpkd_tlv(rx->app);
    }

    return;
}

/*
 * LLDP_8021QAZ_MOD_OPS - MIBDELETEOBJECT
 *
 * ieee8021qaz_mibDeleteObject - deletes MIBs
 * Check if peer has ETS enabled
 *   - If yes, check if ETS TLV is present
 *     - If yes, set it as absent (delete it?)
 * Same for PFC and APP.
 */
u8 ieee8021qaz_mibDeleteObject(struct lldp_port *port)
{
    struct ieee8021qaz_tlvs *tlvs;

    tlvs = ieee8021qaz_data(port->local_port);
    if (!tlvs)
    {
        return 0;
    }

    memset(tlvs->remote_mac,0,ETH_ALEN);
    tlvs->ieee8021qazdu = 0;
    
    ieee8021qaz_free_rx(tlvs->rx);

    /* Reseting ETS Remote params */
    if (tlvs->ets) 
    {
        if (tlvs->ets->cfgr) 
        {
            GLUE_FREE(tlvs->ets->cfgr);
            tlvs->ets->cfgr = NULL;
        }
    }

    if (tlvs->pfc)
    {
        /* Reseting PFC Remote params */
        tlvs->pfc->remote_param = 0;
        tlvs->pfc->remote.willing = false;
        tlvs->pfc->remote.mbc = false;
        tlvs->pfc->remote.pfc_cap = 0;
        tlvs->pfc->remote.pfc_enable = 0;
    }

    /* Clear peer Application data */
    ieee8021qaz_app_reset(&tlvs->app_head);

    /* Kick Tx State Machine */
    run_all_sm(port);
    somethingChangedLocal(port->local_port);
    return 0;
}

static struct ets_attrib *free_ets_tlv(struct ets_attrib *ets)
{
    if (ets) 
    {
        GLUE_FREE(ets->cfgl);
        GLUE_FREE(ets->cfgr);
        GLUE_FREE(ets);
        ets = NULL;
    }

    return NULL;
}

static struct pfc_attrib *free_pfc_tlv(struct pfc_attrib *pfc)
{
    if (pfc) 
    {
        GLUE_FREE(pfc);
        pfc = NULL;
    }

    return NULL;
}

void ieee8021qaz_free_tlv(struct ieee8021qaz_tlvs *tlvs)
{
    struct app_obj *np;

    if (!tlvs)
    {
        return;
    }

    if (tlvs->ets)
    {
        tlvs->ets = free_ets_tlv(tlvs->ets);
    }
    
    if (tlvs->pfc)
    {
        tlvs->pfc = free_pfc_tlv(tlvs->pfc);
    }

    /* Remove _all_ existing application data */
    LIST_FOREACH(np, &tlvs->app_head, entry)
    {
        np->hw = IEEE_APP_DEL;
    }

    __ieee8021qaz_app_sethw(tlvs->local_port, &tlvs->app_head);

    return;
}

static void ieee8021qaz_free_data(struct ieee8021qaz_user_data *iud)
{
    struct ieee8021qaz_tlvs *id;
    if (iud) 
    {
        while (!LIST_EMPTY(&iud->head)) 
        {
            id = LIST_FIRST(&iud->head);
            LIST_REMOVE(id, entry);
            ieee8021qaz_free_tlv(id);
            clear_ieee8021qaz_rx(id);
            GLUE_FREE(id);
        }
    }
}

/* LLDP_8021QAZ_MOD_OPS - UNREGISTER */
void ieee8021qaz_unregister(struct lldp_module *mod)
{
    if (mod->data) 
    {
        ieee8021qaz_free_data((struct ieee8021qaz_user_data *)mod->data);
        GLUE_FREE(mod->data);
    }

    GLUE_FREE(mod);
}

/*
 * LLDP_8021QAZ_MOD_OPS - TLVS_RXED
 */
int ieee8021qaz_tlvs_rxed(u32 local_port)
{
    struct ieee8021qaz_user_data *iud;
    struct ieee8021qaz_tlvs *tlv = NULL;

    iud = (struct ieee8021qaz_user_data *)find_module_user_data_by_id(&gtLLDP.mod_head, LLDP_MOD_8021QAZ);
    if (iud) 
    {
        LIST_FOREACH(tlv, &iud->head, entry) 
        {
            if (local_port==tlv->local_port)
            {
                return !!tlv->ieee8021qazdu;
            }
        }
    }

    return 0;
}

/*
 * LLDP_8021QAZ_MOD_OPS - CHECK_ACTIVE
 */
int ieee8021qaz_check_active(u32 local_port)
{
    struct ieee8021qaz_user_data *iud;
    struct ieee8021qaz_tlvs *tlv = NULL;

    iud = (struct ieee8021qaz_user_data *)find_module_user_data_by_id(&gtLLDP.mod_head, LLDP_MOD_8021QAZ);
    if (iud) 
    {
        LIST_FOREACH(tlv, &iud->head, entry) 
        {
            if (local_port==tlv->local_port)
            {
                return tlv->active && !tlv->pending;
            }
        }
    }

    return 0;
}


void init_hw_qaz_interface(
    int  ( *pfSetHwTc)(u32, u8, u8, u8, u8),
    int  ( *pfSetHwPri2Tc)(u32, u8, u8, u8),
    int  ( *pfSetHwPfc)(u32, u8, u8, u8, u8)
    )
{
    gtETS.pfSetHwTc     = pfSetHwTc;
    gtETS.pfSetHwPri2Tc = pfSetHwPri2Tc;       
    gtETS.pfSetHwPfc    = pfSetHwPfc;
}

#ifdef V4_VERSION
int v4_set_hw_tc(u32 dwPort, u8  ucTcId, u8  ucBandwith, u8  ucTsa, u8 ucOpt)
{    
    int ret;
    T_TcCfgInfo          t_Info;    
    T_LossnessPortTcIdx  Idx;

    memset(&Idx,0,sizeof(Idx));            
    memset(&t_Info,0,sizeof(t_Info));

    Idx.dwPort        = dwPort;
    Idx.ucTc          = ucTcId;
    t_Info.ucTcNo     = ucTcId;
    t_Info.ucBandwith = ucBandwith;
    t_Info.ucAl_Id    = ucTsa;

    if (ucOpt)
    {
        ret = VtIoTCAdd(&Idx, &t_Info);
    }
    else
    {
        ret = VtIoTCDel(&Idx);
    }

    return ret;
}

int v4_set_hw_pri2tc(u32 dwPort, u8 ucTcId, u8 ucQos, u8 ucOpt)
{
    int ret;    
    T_Qos2TcInfo          t_Qos2TcInfo;    
    T_LossnessPortQosIdx  Qos2TcIdx;

    memset(&Qos2TcIdx,0,sizeof(Qos2TcIdx));            
    memset(&t_Qos2TcInfo,0,sizeof(t_Qos2TcInfo));    
    
    Qos2TcIdx.dwPort    = dwPort;
    Qos2TcIdx.ucTcNo    = ucTcId;
    Qos2TcIdx.ucQos     = ucQos;
    
    t_Qos2TcInfo.ucTcNo = ucTcId;
    t_Qos2TcInfo.ucQos  = ucQos;

    if (ucOpt)
    {
        ret = VtIoQos2TCAdd(&Qos2TcIdx, &t_Qos2TcInfo);   
    }
    else
    {
        ret = VtIoQos2TCDel(&Qos2TcIdx);
    }

    return ret;
    
}

int v4_set_hw_pfc(u32 dwPort, u8 ucQos, u8 ucMbc, u8 ucEnabled, u8 ucOpt)
{    
    int ret;        
    T_LossnessPortCfgIdx Idx;
    T_LossnessCfgInfo t_Info;

    memset(&Idx,0,sizeof(Idx));       
    memset(&t_Info,0,sizeof(t_Info));

    Idx.dwPort        = dwPort;
    t_Info.ucQos      = ucQos;
    t_Info.ucMbc      = ucMbc;
    t_Info.ucPfc_Flag = ucEnabled;

    ret = VtIoPFCSet(&Idx, &t_Info);

    return ret;    
    
}
#endif 



#ifdef __cplusplus
}
#endif

