/*******************************************************************************

  LLDP Agent Daemon (LLDPAD) Software 
  Copyright(c) 2007-2010 Intel Corporation.

  Substantially modified from:
  hostapd-0.5.7
  Copyright (c) 2002-2007, Jouni Malinen <jkmaline@cc.hut.fi> and
  contributors

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

T_LLDP  gtLLDP;

T_LLDP *lldp_init(void (*pfPortInit)(void),
                  void (*pfTmInit)(void),
                  void (*pfTmSet)(void),
                  void (*pfTmStop)(void),
                  int  (*pfSndPkt)(u32, u8*, u16))
{   
    /*init回调函数用来初始化各个端口的lldp属性*/
    lldp_deinit();

    memset(&gtLLDP, 0, sizeof(T_LLDP));
    gtLLDP.pfPortInit = pfPortInit;
    gtLLDP.pfTmInit   = pfTmInit;
    gtLLDP.pfTmSet    = pfTmSet;
    gtLLDP.pfTmStop   = pfTmStop;
    gtLLDP.pfSndPkt   = pfSndPkt;

    gtLLDP.pfRcvPkt   = &rxReceiveFrame;
    gtLLDP.pfTmout    = &lldp_timeout;
    
    LIST_INIT(&gtLLDP.mod_head);
    lldp_mod_register(&mand_register);

    if (gtLLDP.pfTmInit)
    {
        gtLLDP.pfTmInit();
    }
    
    start_lldp_agent();

    return &gtLLDP;

}

void lldp_deinit(void)
{
    stop_lldp_agent();
    lldp_deinit_ports();
    lldp_all_mod_unregister();

    memset(&gtLLDP, 0, sizeof(T_LLDP));
}

struct lldp_module *find_module_by_id(struct lldp_head *head, int id)
{
    struct lldp_module *mod;

    LIST_FOREACH(mod, head, lldp) 
    {
        if (mod->id == id)
        {
            return mod;
        }
    }
    
    return NULL;
}

void *find_module_user_data_by_id(struct lldp_head *head, int id)
{
    struct lldp_module *mod;

    mod = find_module_by_id(head, id);
    if (mod)
    {
        return mod->data;
    }
    
    return NULL;
}

void lldp_mod_register(struct lldp_module *(pf)(void))
{
    struct lldp_module *module = NULL;
    struct lldp_module *premod = NULL;
    struct lldp_module *mod;
    struct lldp_port   *port   = NULL;

    if (NULL==pf)
    {
        return;
    }

    module = pf();
    if (!module)
    {
        LLDPAD_DBG("module is NULL\n");        
        return;
    }

    /*判断是否已经注册过*/
    LIST_FOREACH(mod, &gtLLDP.mod_head, lldp)
    {
        if (mod->id == module->id)
        {
            LLDPAD_DBG("module->id %d has been registered\n",module->id);
            /*释放pf函数申请module的空间*/
            module->ops->lldp_mod_unregister(module);
            return;
        }
    }

    lldp_deinit_ports();    

    /*找到最后一个模块的结构*/
    LIST_FOREACH(mod, &gtLLDP.mod_head, lldp)
    {
        premod = mod;
    }

    /*挂链*/
    if (premod)
    {
        LIST_INSERT_AFTER(premod, module, lldp);
    }
    else
    {
        LIST_INSERT_HEAD(&gtLLDP.mod_head, module, lldp);
    }

    /*由于模块发生变化，重新初始化端口*/
    lldp_init_ports();

    for (port = porthead; port; port=port->next)
    {
        somethingChangedLocal(port->local_port);
    }
    
}

void lldp_mod_unregister(int dwModId)
{
    struct lldp_module *module = NULL;
    struct lldp_port   *port   = NULL;    

    LIST_FOREACH(module, &gtLLDP.mod_head, lldp)
    {
        if (dwModId == module->id)
        {
            break;
        }
    }

    if (!module)
    {
        return;
    }

    lldp_deinit_ports();    

    LIST_REMOVE(module, lldp);
    module->ops->lldp_mod_unregister(module); 
    
    /*由于模块发生变化，重新初始化端口*/
    lldp_init_ports();

    for (port = porthead; port; port=port->next)
    {
        somethingChangedLocal(port->local_port);
    }    
    
}


void lldp_all_mod_unregister(void)
{
    struct lldp_module *module;

    while (!LIST_EMPTY(&gtLLDP.mod_head)) 
    {
        module = LIST_FIRST(&gtLLDP.mod_head);
        LIST_REMOVE(module, lldp);
        module->ops->lldp_mod_unregister(module);
    }
}


#ifdef __cplusplus
}
#endif



