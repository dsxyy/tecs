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

T_QCN gtQCN = {
    NULL,
    NULL
};

const struct lldp_mod_ops ieee8021qau_ops =  {
    ieee8021qau_register,
    ieee8021qau_unregister,
    ieee8021qau_gettlv,
    ieee8021qau_rchange,
    0,  
    ieee8021qau_ifup,
    ieee8021qau_ifdown,
    ieee8021qau_ifadd,
    ieee8021qau_ifdel,
    ieee8021qau_mibDeleteObject,
    0,
    0,
    0,
    0,
    0,
    ieee8021qau_check_pending,
};

int ieee8021qau_check_pending(struct lldp_port *port)
{
    struct ieee8021qau_user_data *iud;
    struct ieee8021qau_data *data = NULL;

    if ((NULL==port) || !port->portEnabled)
    {
        return 0;
    }

    iud = (struct ieee8021qau_user_data *)find_module_user_data_by_id(&gtLLDP.mod_head, LLDP_MOD_8021QAU);
    if (iud) 
    {
        LIST_FOREACH(data, &iud->head, entry) 
        {
            if (data->local_port == port->local_port) 
            {
                if (data->active && data->pending &&
                    port->timers.dormantDelay == 1) 
                {
                    data->pending = false;
                    run_qcn_sm(port);
                    /*somethingChangedLocal(port->local_port);*/
                }
                break;
            }
        }
    }

    return 0;
}

struct ieee8021qau_data *ieee8021qau_data(u32 local_port)
{
    struct ieee8021qau_user_data *iud;
    struct ieee8021qau_data *id = NULL;

    iud = (struct ieee8021qau_user_data *)find_module_user_data_by_id(&gtLLDP.mod_head, LLDP_MOD_8021QAU);
    if (iud) 
    {
        LIST_FOREACH(id, &iud->head, entry) 
        {
            if (local_port==id->local_port)
            {
                return id;
            }
        }
    }
    
    return NULL;
}

struct qcn_attrib *free_qcn_tlv(struct qcn_attrib *qcn)
{
    if (qcn)
    {
        GLUE_FREE(qcn);
        qcn = NULL;
    }

    return NULL;
}

void ieee8021qau_free_tlv(struct ieee8021qau_data *data)
{
    if (data->qcn)
    {
        data->qcn = free_qcn_tlv(data->qcn);
    }
}

static void ieee8021qau_free_data(struct ieee8021qau_user_data *iud)
{
    struct ieee8021qau_data *data;
    if (iud) 
    {
        while (!LIST_EMPTY(&iud->head)) 
        {
            data = LIST_FIRST(&iud->head);
            LIST_REMOVE(data, entry);
            ieee8021qau_free_tlv(data);
            clear_ieee8021qau_rx(data);
            GLUE_FREE(data);
        }
    }
}

struct packed_tlv *ieee8021qau_gettlv(struct lldp_port *port)
{
    int size;
    struct ieee8021qau_data *data;
    struct packed_tlv   *ptlv = NULL;
    struct unpacked_tlv *cn_tlv = NULL;
    
    data = ieee8021qau_data(port->local_port);
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

    if (is_tlv_enabled(port->local_port, TLVID_8021(LLDP_8021QAU_QCN)))
    {
        cn_tlv = bld_ieee8021qau_qcn_tlv(data);    
    }

    size = TLVSIZE(cn_tlv);

    if (!size)
    {
        goto err;
    }

    ptlv = create_ptlv();
    if (!ptlv) 
    {
        LLDPAD_DBG("%s:port %d GLUE_ALLOC(ptlv) failed\n", __func__, port->local_port);
        goto err;
    }

    ptlv->tlv = (u8 *)GLUE_ALLOC(size);
    if (!ptlv->tlv)
    {
        goto err;
    }

    /* pack all pre-built tlvs */
    ptlv->size = 0;
    PACK_TLV_AFTER(cn_tlv, ptlv, size, err);
    
    free_unpkd_tlv(cn_tlv);
    return ptlv;
    
err:
    free_pkd_tlv(ptlv);
    free_unpkd_tlv(cn_tlv);    
    return NULL;
}

void ieee8021qau_ifdown(u32 local_port)
{
    struct ieee8021qau_data *data;

    data = ieee8021qau_data(local_port);
    if (!data)
    {
        LLDPAD_DBG("%s:port %d ieee8021qau_ifdown failed\n", __func__, local_port);
        return;
    }

    clear_ieee8021qau_rx(data);
}

void ieee8021qau_ifup(u32 local_port)
{    
    return;
}

void ieee8021qau_ifdel(u32 local_port)
{
    struct ieee8021qau_data *data;

    data = ieee8021qau_data(local_port);
    if (data)
    {
        LIST_REMOVE(data, entry);
        ieee8021qau_free_tlv(data);
        clear_ieee8021qau_rx(data);
        GLUE_FREE(data); 
    }
}

void ieee8021qau_ifadd(u32 local_port)
{
    struct ieee8021qau_data *data;
    struct ieee8021qau_user_data *iud;

    data = ieee8021qau_data(local_port);
    if (data) 
    {
        LLDPAD_DBG("%s:port %d exists\n", __func__, local_port);
        return;
    }

    /* not found, alloc/init per-port tlv data */
    data = (struct ieee8021qau_data *)GLUE_ALLOC(sizeof(struct ieee8021qau_data));
    if (!data) 
    {
        LLDPAD_DBG("%s:port %d GLUE_ALLOC %ld failed\n", __func__, local_port, sizeof(struct ieee8021qau_data));
        return;
    }
    memset(data, 0, sizeof(struct ieee8021qau_data));
    
    data->local_port = local_port;

    data->rx = (struct ieee8021qau_unpkd_tlvs *)GLUE_ALLOC(sizeof(struct ieee8021qau_unpkd_tlvs));
    if (!data->rx) 
    {
        GLUE_FREE(data);
        LLDPAD_DBG("%s: port %d rx GLUE_ALLOC failed.\n", __func__, local_port);
        return;
    }
    memset(data->rx, 0, sizeof(struct ieee8021qau_unpkd_tlvs));
    
    data->qcn = (struct qcn_attrib *)GLUE_ALLOC(sizeof(struct qcn_attrib));
    if (!data->qcn)
    {
        GLUE_FREE(data);
        LLDPAD_DBG("%s: port %d qcn LUE_ALLOC failed.\n", __func__, local_port);
        return;        
    }
    memset(data->qcn, 0, sizeof(struct qcn_attrib));

    data->active  = true;
    data->pending = true;
    
    set_default_qcn_cfg(data);
 
    iud = (struct ieee8021qau_user_data *)find_module_user_data_by_id(&gtLLDP.mod_head, LLDP_MOD_8021QAU);
    LIST_INSERT_HEAD(&iud->head, data, entry);
    
    LLDPAD_DBG("%s:port %d added\n", __func__, local_port);
    
    return;
}


struct lldp_module *ieee8021qau_register(void)
{
    struct lldp_module *mod;
    struct ieee8021qau_user_data *iud;

    mod = (struct lldp_module *)GLUE_ALLOC(sizeof(struct lldp_module));
    if (!mod) 
    {
        LLDPAD_ERR("failed to GLUE_ALLOC LLDP ieee8021qau module data\n");
        goto out_err;
    }
    
    iud = (struct ieee8021qau_user_data *)GLUE_ALLOC(sizeof(struct ieee8021qau_user_data));
    if (!iud) 
    {
        GLUE_FREE(mod);
        LLDPAD_ERR("failed to GLUE_ALLOC LLDP ieee8021qau module user data\n");
        goto out_err;
    }
    
    LIST_INIT(&iud->head);
    mod->id = LLDP_MOD_8021QAU;
    mod->ops = &ieee8021qau_ops;
    mod->data = iud;
    LLDPAD_DBG("%s:done\n", __func__);
    
    return mod;
out_err:
    LLDPAD_DBG("%s:failed\n", __func__);
    return NULL;
}

void ieee8021qau_unregister(struct lldp_module *mod)
{
    if (mod->data)
    {
        ieee8021qau_free_data((struct ieee8021qau_user_data *) mod->data);
        GLUE_FREE(mod->data);
    }
    
    GLUE_FREE(mod);
    LLDPAD_DBG("%s:done\n", __func__);
}

u8 ieee8021qau_mibDeleteObject(struct lldp_port *port)
{
    struct ieee8021qau_data *data;

    data = ieee8021qau_data(port->local_port);
    if (NULL==data || NULL==data->qcn)
    {
        return 0;
    }

    data->ieee8021qaudu = 0;
    
    ieee8021qau_free_rx(data->rx);

    /* Reseting QCN Remote params */
    memset(&data->qcn->remote,0,sizeof(struct qcn_obj));

    /* Kick Tx State Machine */
    run_qcn_sm(port);
    somethingChangedLocal(port->local_port);
    return 0;
}

u8 get_cnpv_octet(struct qcn_obj *qcn)
{
    int i;
    u8 cnpv = 0;
    
    if (NULL==qcn)
    {
        return cnpv;
    }

    for (i=0;i<MAX_USER_PRIORITIES;i++)
    {
        cnpv |= (qcn->cnpv[i].used)?(1<<i):0;
    }

    return cnpv;   
}

u8 get_ready_octet(struct qcn_obj *qcn)
{
    int i;
    u8 ready = 0;
    
    if (NULL==qcn)
    {
        return ready;
    }

    for (i=0;i<MAX_USER_PRIORITIES;i++)
    {
        ready |= (qcn->cnpv[i].ready)?(1<<i):0;
    }

    return ready;   
}


struct unpacked_tlv *bld_ieee8021qau_qcn_tlv(struct ieee8021qau_data *data)
{
    struct ieee8021qau_cntlv *cn;
    struct unpacked_tlv *tlv = NULL;

    if (NULL==data || NULL==data->qcn)
    {
        return NULL;
    }

    tlv = create_tlv();
    if (!tlv)
        return NULL;

    cn = (struct ieee8021qau_cntlv *)GLUE_ALLOC(sizeof(struct ieee8021qau_cntlv));
    if (!cn) {
        LLDPAD_WARN("%s: Failed to GLUE_ALLOC cntlv\n", __func__);
        GLUE_FREE(tlv);
        return NULL;
    }
    memset(cn, 0, sizeof(struct ieee8021qau_cntlv));

    hton24(cn->oui, OUI_IEEE_8021);
    cn->subtype = LLDP_8021QAU_QCN;
    if (0==get_qcn_tlv_info(data->local_port, &cn->perPriCNPV, &cn->perPriReady))
    {
        goto error;
    }

    tlv->type = ORG_SPECIFIC_TLV;
    tlv->length = sizeof(struct ieee8021qau_cntlv);
    tlv->info = (u8 *)cn;

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


bool unpack_ieee8021qau_tlvs(struct lldp_port *port, struct unpacked_tlv *tlv, bool *tlv_store)
{
    /* Unpack tlvs and store in rx */
    struct ieee8021qau_data *data; 

    if (NULL==port || NULL==tlv || NULL==tlv_store)
    {
        return false;
    }    

    *tlv_store = false;  

    data = ieee8021qau_data(port->local_port);
    if (NULL==data || NULL==data->rx)
    {
        return false;
    }

    /* Process */
    switch (tlv->info[OUI_SIZE]) 
    {
    case IEEE8021QAU_QCN_TLV:
        if (!is_tlv_enabled(port->local_port, TLVID_8021(LLDP_8021QAU_QCN)))    
        {
            return true;            
        }
        
        if (data->rx->qcn)
        {
            LLDPAD_WARN("%s: port %d: 802.1Qau Duplicate ETSCFG TLV\n", __func__, port->local_port);
            port->rx.dupTlvs |= DUP_IEEE8021QAU_TLV_QCN;
            return false;
        }
        
        data->ieee8021qaudu |= RCVD_IEEE8021QAU_TLV_QCN;
        data->rx->qcn = tlv;        
        break;
    default:
        return true;
    }

    *tlv_store = true;  
    return true;
}

void set_default_qcn_cfg(struct ieee8021qau_data *data)
{
    int i;
    
    if (NULL==data || NULL==data->qcn)
    {
        return;
    }

    for (i=0;i<MAX_USER_PRIORITIES;i++)
    {          
        if (i!=MAX_USER_PRIORITIES-1)
        {
            data->qcn->local.cnpv[i].used  = 1;
            data->qcn->local.cnpv[i].mode  = QCN_AUTO;
            data->qcn->local.cnpv[i].ready = 1;
        }
    }

    memcpy(&data->qcn->cur,&data->qcn->local,sizeof(struct qcn_obj));
}

void ieee8021qau_free_rx(struct ieee8021qau_unpkd_tlvs *rx)
{
    if (!rx)
    {
        return;
    }

    if (rx->qcn)
    {
        rx->qcn = free_unpkd_tlv(rx->qcn);
    }

    return;
}

void clear_ieee8021qau_rx(struct ieee8021qau_data *data)
{
    if (!data)
    {
        return;
    }

    if (!data->rx)
    {
        return;    
    }

    ieee8021qau_free_rx(data->rx);

    GLUE_FREE(data->rx);
    data->rx = NULL;
}

int ieee8021qau_rchange(struct lldp_port *port, struct unpacked_tlv *tlv, bool *tlv_store)
{
    u8 oui[OUI_SIZE] = INIT_IEEE8021_OUI;
    struct ieee8021qau_data *data;
    struct ieee8021qau_unpkd_tlvs *rx;

    data = ieee8021qau_data(port->local_port);
    if (!data)
    {
        return SUBTYPE_INVALID;
    }

    /*对于TYPE_0，TYPE_1需要处理*/
    if (tlv->type == TYPE_0) 
    {
        if (data->active && !data->ieee8021qaudu) 
        {
            data->active  = false;
            data->pending = false;
            LLDPAD_INFO("QCN on port %d going INACTIVE\n", data->local_port);            
            ieee8021qau_mibDeleteObject(port);
        }

        if (!data->active && data->ieee8021qaudu)
        {
            data->active  = true;
            data->pending = true;
            LLDPAD_INFO("QCN on port %d going ACTIVE\n", data->local_port);          
        }

        if (data->active)
        {            
            if (data->ieee8021qaudu)
            {
                data->pending = false;
            }
            
            /* Update TLV State Machines */
            ieee8021qau_mibUpdateObjects(port);
            clear_ieee8021qau_rx(data);
            run_qcn_sm(port);
            somethingChangedLocal(port->local_port);              
        }
    }    

    /*
     * TYPE_1 mandatory and always before IEEE8021QAu tlvs so
     * we can use it to make the beginning of a IEEE8021QAu PDU.
     * Verifies that only a single IEEE8021QAu tlv is present.
     */
    if (tlv->type == TYPE_1) {
        clear_ieee8021qau_rx(data);
        rx = (struct ieee8021qau_unpkd_tlvs *)GLUE_ALLOC(sizeof(struct ieee8021qau_unpkd_tlvs));
        memset(rx, 0, sizeof(struct ieee8021qau_unpkd_tlvs));
        data->rx = rx;
        data->ieee8021qaudu = 0;
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

        res = unpack_ieee8021qau_tlvs(port, tlv, tlv_store);
        if (!res)
        {
            LLDPAD_WARN("Error unpacking 8021 tlvs");
            return TLV_ERR;
        }

        return TLV_OK;
    }

    return TLV_OK;
}

void ieee8021qau_mibUpdateObjects(struct lldp_port *port)
{
    struct ieee8021qau_data *data;

    if (NULL==port)
    {
        return;
    }

    data = ieee8021qau_data(port->local_port);
    if (NULL==data || NULL==data->rx || NULL==data->qcn)
    {
        return;
    }    

    if (data->rx->qcn)
    {
        process_ieee8021qau_qcn_tlv(port);
    }
    else
    {
        memset(&data->qcn->remote,0,sizeof(struct qcn_obj));        
    }

}

void process_ieee8021qau_qcn_tlv(struct lldp_port *port)
{
    struct ieee8021qau_data *data = NULL;
    u8 offset = 0;
    int i;
    u8 cnpv  = 0;
    u8 ready = 0;

    data = ieee8021qau_data(port->local_port);
    if (NULL==data || NULL==data->rx || NULL==data->qcn)
    {
        return;
    } 

    /* Bypassing OUI, SUBTYPE fields */
    offset = OUI_SUB_SIZE;

    cnpv  = data->rx->qcn->info[offset];
    ready = data->rx->qcn->info[offset+1];
    
    for (i=0;i<MAX_USER_PRIORITIES;i++)
    {
        data->qcn->remote.cnpv[i].used  = (cnpv  & (1<<i))?1:0;
        data->qcn->remote.cnpv[i].ready = (ready & (1<<i))?1:0;
    }

}

void run_qcn_sm(struct lldp_port *port)
{
    struct ieee8021qau_data *data;

    data = ieee8021qau_data(port->local_port);
    if (!data)
    {
        return;
    }

    qcn_sm(data);    
#if 0
    if (is_tlv_enabled(port->local_port, TLVID_8021(LLDP_8021QAU_QCN)))    
    {
        qcn_sm(data);
    }
#endif

}

void set_hw_qcn(struct ieee8021qau_data *data)
{
    int ret = 0;
    u8  qos = 0;

    if (NULL==data || NULL==data->qcn)
    {
        return;
    }    

    for (qos=0; qos<MAX_USER_PRIORITIES; qos++)
    {  
        if (gtQCN.pfSetHwQcnEnable)
        {
            ret = gtQCN.pfSetHwQcnEnable(data->local_port,qos,data->qcn->cur.cnpv[qos].used);
        }
        
        if (ret == -1)
        {
            LLDPAD_INFO("%s: port %d: set_hw_qcn pfSetHwQcnEnable failed,qos:%d\n",  __func__, data->local_port,qos);
        }  
        else
        {  
            LLDPAD_INFO("%s: port %d: set_hw_qcn pfSetHwQcnEnable success,qos:%d\n",  __func__, data->local_port,qos);
        }

        if (gtQCN.pfSetHwQcnMode)
        {
            ret = gtQCN.pfSetHwQcnMode(data->local_port,qos,data->qcn->cur.cnpv[qos].mode);
        }

        if (ret == -1)
        {
            LLDPAD_INFO("%s: port %d: set_hw_qcn pfSetHwQcnMode failed,qos:%d\n",  __func__, data->local_port,qos);
        }  
        else
        {  
            LLDPAD_INFO("%s: port %d: set_hw_qcn pfSetHwQcnMode success,qos:%d\n",  __func__, data->local_port,qos);
        }       
    }       
}   


void qcn_sm(struct ieee8021qau_data *data)
{
    int i;
    
    if (NULL==data || NULL==data->qcn)
    {
        return;
    }

    memcpy(&data->qcn->cur,&data->qcn->local,sizeof(struct qcn_obj));

    for (i=0;i<MAX_USER_PRIORITIES;i++)
    {
        if (0==data->qcn->local.cnpv[i].used)
        {
            continue;
        }

        if (QCN_AUTO!=data->qcn->local.cnpv[i].mode)
        {
            continue;
        }

        if (data->qcn->remote.cnpv[i].used)
        {
            data->qcn->cur.cnpv[i].mode  = QCN_ICP;
        }
        else
        {
            data->qcn->cur.cnpv[i].mode  = QCN_ECP;
        }
    }

    set_hw_qcn(data);
}


const char* get_cnpv_mode_string(u8 mode)
{
    switch (mode)
    {
    case QCN_ECP:
        return "ecp";
    case QCN_ICP:
        return "icp";
    case QCN_RP:
        return "rp";        
    case QCN_AUTO:
        return "auto";         
    default:;
        break;
    }

    return " ";
}

int ieee8021qau_check_active(u32 local_port)
{
    struct ieee8021qau_user_data *iud;
    struct ieee8021qau_data *data = NULL;

    iud = (struct ieee8021qau_user_data *)find_module_user_data_by_id(&gtLLDP.mod_head, LLDP_MOD_8021QAU);
    if (iud) 
    {
        LIST_FOREACH(data, &iud->head, entry) 
        {
            if (local_port==data->local_port)
            {
                return data->active && !data->pending;
            }
        }
    }

    return 0;
}

int ieee8021qau_tlvs_rxed(u32 local_port)
{
    struct ieee8021qau_user_data *iud;
    struct ieee8021qau_data *data = NULL;

    iud = (struct ieee8021qau_user_data *)find_module_user_data_by_id(&gtLLDP.mod_head, LLDP_MOD_8021QAU);
    if (iud) 
    {
        LIST_FOREACH(data, &iud->head, entry)
        {
            if (local_port==data->local_port)
            {
                return !!data->ieee8021qaudu;
            }
        }
    }

    return 0;
}

int get_qcn_tlv_info(u32 local_port, u8 *perPriCNPV, u8 *perPriReady)
{
    if (NULL==perPriCNPV || NULL==perPriReady)
    {
        return 0;
    }

    if (is_cp_mode(local_port))
    {
        struct ieee8021qau_data *data; 
        
        data = ieee8021qau_data(local_port);
        if (NULL==data)
        {
            return 0;
        }
        
        *perPriCNPV  = get_cnpv_octet(&data->qcn->cur);
        *perPriReady = get_ready_octet(&data->qcn->cur);
    }
    else
    {
        #ifdef V4_VERSION
        T_LossnessPortCfgIdx Idx;
        WORD16 LQcnFlag;
        WORD16 LReadyFlag;

        Idx.dwPort = local_port;

        if (0!=VtIoQCNGet(&Idx, &LQcnFlag, &LReadyFlag))
        {
            return 0;
        }
        
        *perPriCNPV  = (u8)LQcnFlag;
        *perPriReady = (u8)LReadyFlag;
        #endif
    }   

    return 1;    
}

int is_cp_mode(u32 local_port)
{
    #ifdef V4_VERSION
    T_LossnessPortCfgIdx Idx;
    WORD16 LQcnFlag;
    WORD16 LReadyFlag;

    Idx.dwPort = local_port;

    if (0==VtIoQCNGet(&Idx, &LQcnFlag, &LReadyFlag))
    {
        return 0;         
    }
    #endif
    
    return 1;    
}

void init_hw_qau_interface(
    int  ( *pfSetHwQcnEnable)(u32, u8, u8),
    int  ( *pfSetHwQcnMode)(u32, u8, u8)
    )
{
    gtQCN.pfSetHwQcnEnable = pfSetHwQcnEnable;
    gtQCN.pfSetHwQcnMode   = pfSetHwQcnMode;       
}

#ifdef V4_VERSION
int v4_set_hw_qcn_enable(u32 dwPort, u8 ucQos, u8 ucEnabled)
{
    T_LossnessPortCfgIdx Idx;
    T_LossnessCfgInfo t_Info;
    u8 perPriCNPV;
    u8 perPriReady;

    memset(&Idx,0,sizeof(Idx));
    memset(&t_Info,0,sizeof(t_Info));

    Idx.dwPort   = dwPort;
    t_Info.ucQos = ucQos;

    get_qcn_tlv_info(dwPort, &perPriCNPV, &perPriReady);    
    
    if (is_cp_mode(dwPort))
    {
        t_Info.ucLQcn_Flag   = ucEnabled;
        #if 0
        t_Info.ucLReady_Flag = data->qcn->cur.cnpv[ucQos].ready;
        #endif
    }
    else
    {
        t_Info.ucLQcn_Flag   = perPriCNPV  & (1<<ucQos);
        t_Info.ucLReady_Flag = perPriReady & (1<<ucQos);       
    }
    #if 0
    t_Info.ucRQcn_Flag   = data->qcn->remote.cnpv[ucQos].used;
    t_Info.ucRReady_Flag = data->qcn->remote.cnpv[ucQos].ready;
    #endif


    return VtIoQCNSet(&Idx, &t_Info);
}
#endif




#ifdef __cplusplus
}
#endif


