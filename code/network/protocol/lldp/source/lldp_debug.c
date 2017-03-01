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

u8 g_print = 0;
void lldp_set_print(u8 flag)
{
    g_print = flag;    
}

void lldp_show_rx_sm(u32 local_port)
{
    struct lldp_port *port; 
    
    port = find_by_port(local_port);
    if (!port)
    {
        printf("port error!\n");            
        return;
    }      
    
    printf("--------------------------------------------------------------------\n");            
    printf("port %d rx state machine:\n",local_port);
    printf("state:%s\n",get_rx_sm_string(port->rx.state));
    printf("framein:0x%x\n",port->rx.framein);
    printf("badFrame:%-21srcvFrame:%s\n",(port->rx.badFrame)?"true":"false",(port->rx.rcvFrame)?"true":"false");
    printf("rxInfoAge:%-20sremoteChange:%s\n",(port->rx.rxInfoAge)?"true":"false",(port->rx.remoteChange)?"true":"false");
    printf("tooManyNghbrs:%-16sdupTlvs:%d\n",(port->rx.tooManyNghbrs)?"true":"false",port->rx.dupTlvs);
    printf("newNeighbor:%-18smanifest:0x%x\n\n",(port->rx.newNeighbor)?"true":"false",port->rx.manifest);      
    
    
}

void lldp_show_tx_sm(u32 local_port)
{
    struct lldp_port *port; 
    
    port = find_by_port(local_port);
    if (!port)
    {
        printf("port error!\n");            
        return;
    }  
    printf("--------------------------------------------------------------------\n");            
    printf("port %d tx state machine:\n",local_port);
    printf("state:%s\n",get_tx_sm_string(port->tx.state));
    printf("frameout:0x%x\n",port->tx.frameout);
    printf("localChange:%-18dtxTTL:%d\n",port->tx.localChange,port->tx.txTTL);
    printf("txNow:%-24dtxFast:%d\n\n",port->tx.txNow,port->tx.txFast);    
    
    
}

void stt_show_lldp(u32 local_port)
{
    struct lldp_port *port; 
    
    port = find_by_port(local_port);
    if (!port)
    {
        printf("port error!\n");            
        return;
    }  

    printf("--------------------------------------------------------------------\n");            
    printf("port %d statistic state:\n",local_port);
    printf("statsFramesOutTotal:%-10dstatsAgeoutsTotal:%d\n",port->stats.statsFramesOutTotal,port->stats.statsAgeoutsTotal);
    printf("statsFramesOutErrorsTotal:%-4dstatsTLVsUnrecognizedTotal:%d\n",port->stats.statsFramesOutErrorsTotal,port->stats.statsTLVsUnrecognizedTotal);    
    printf("statsFramesDiscardedTotal:%-4dstatsFramesInErrorsTotal:%d\n",port->stats.statsFramesDiscardedTotal,port->stats.statsFramesInErrorsTotal);
    printf("statsFramesInTotal:%-11dstatsTLVsDiscardedTotal:%d\n",port->stats.statsFramesInTotal,port->stats.statsTLVsDiscardedTotal); 
}

void stt_clear_lldp(u32 local_port)
{
    struct lldp_port *port; 
    
    port = find_by_port(local_port);
    if (!port)
    {
        printf("port error!\n");            
        return;
    } 

    memset(&port->stats,0,sizeof(struct portstats));   
}


void lldp_show_porttimer_sm(u32 local_port)
{
    struct lldp_port *port; 
    
    port = find_by_port(local_port);
    if (!port)
    {
        printf("port error!\n");            
        return;
    }  

    printf("--------------------------------------------------------------------\n");            
    printf("port %d timer state machine:\n",local_port);
    printf("state:%s\n",get_txtimer_sm_string(port->timers.state));
    printf("dormantDelay:%-17dreinitDelay:%d\n",port->timers.dormantDelay,port->timers.reinitDelay);
    printf("msgTxHold:%-20dmsgTxInterval:%d\n",port->timers.msgTxHold,port->timers.msgTxInterval);
    printf("msgFastTx:%-20dtxFastInit:%d\n",port->timers.msgFastTx,port->timers.txFastInit);
    printf("txTTR:%-24dtxShutdownWhile:%d\n",port->timers.txTTR,port->timers.txShutdownWhile);
    printf("txCredit:%-21dtxMaxCredit:%d\n",port->timers.txCredit,port->timers.txMaxCredit);
    printf("txTick:%-23dtooManyNghbrsTimer:%d\n",port->timers.txTick,port->timers.tooManyNghbrsTimer);
    printf("rxTTL:%-24dlastrxTTL:%d\n\n",port->timers.rxTTL,port->timers.lastrxTTL);
    
    
}
        

void lldp_show_info_one(u32 local_port)
{
    struct lldp_port *port; 
    
    port = find_by_port(local_port);
    if (!port)
    {
        printf("port error!\n");            
        return;
    }  

    printf("====================================================================\n");      
    printf("port %d basic info:\n",port->local_port);
    printf("--------------------------------------------------------------------\n");
    printf("hw_resetting:%-17dportEnabled:%d\n",port->hw_resetting,port->portEnabled);
    printf("prevPortEnabled:%-14dadminStatus:%s\n",port->prevPortEnabled,get_lldp_port_admin_string(port->adminStatus));
    printf("rxChanges:%-20dlldpdu:%d\n\n",port->rxChanges,port->lldpdu); 
    printf("perm_mac_addr:" MACSTR "\n", MAC2STR(port->l2->perm_mac_addr));
    printf("curr_mac_addr:" MACSTR "\n", MAC2STR(port->l2->curr_mac_addr));
    printf("san_mac_addr:" MACSTR "\n", MAC2STR(port->l2->san_mac_addr));
    printf("remote_mac_addr:" MACSTR "\n\n", MAC2STR(port->l2->remote_mac_addr)); 

    lldp_show_rx_sm(port->local_port);
    lldp_show_tx_sm(port->local_port);
    lldp_show_porttimer_sm(port->local_port);
    stt_show_lldp(port->local_port); 

    printf("--------------------------------------------------------------------\n"); 
    
    
}

void lldp_show_info(u32 local_port)
{
    struct lldp_port *lldppt = NULL;
    
    if (0==local_port)
    {
        for (lldppt = porthead;lldppt;lldppt=lldppt->next)
        {
            lldp_show_info_one(lldppt->local_port);                       
        }
    }
    else
    {
        lldp_show_info_one(local_port);         
    }    
    
}

void lldp_show_port(void)
{
    struct lldp_port *lldppt = NULL;

    printf("==========================================================================\n");      
    printf("Port  PortEnabled   AdminStatus   LocalMacAddr        RemoteMacAddr       \n");
    printf("--------------------------------------------------------------------------\n");
    
    for (lldppt = porthead;lldppt;lldppt=lldppt->next)
    {        
        printf("%-6d%-14s%-14s" MACSTR "   " MACSTR "\n",lldppt->local_port,(lldppt->portEnabled?"enabled":"disabled"),
                get_lldp_port_admin_string(lldppt->adminStatus),MAC2STR(lldppt->l2->curr_mac_addr),MAC2STR(lldppt->l2->remote_mac_addr));                      
    }
    printf("--------------------------------------------------------------------------\n");     
    
}

void set_lldp_mode(u32 local_port, u8 admin)
{
    struct lldp_port * port = NULL;
    
    port = find_by_port(local_port);
    if (!port)
    {
        printf("port error!\n");            
        return;
    }
    
    set_lldp_port_admin(local_port,admin);

    printf("port %d set admin status %s!\n",local_port,get_lldp_port_admin_string(admin));    
}


void set_dcbx_enable(u32 local_port, bool val)
{
    struct ieee8021qaz_tlvs *tlvs;
    struct lldp_port * port = NULL;
    
    port = find_by_port(local_port);
    if (!port)
    {
        printf("port error!\n");            
        return;
    }         

    tlvs = ieee8021qaz_data(local_port);
    if (NULL==tlvs)
    {
        printf("tlvs is NULL, port %d\n",local_port);
        return;
    }
    
    if (val==true)
    {
        tlvs->active  = true;
        tlvs->pending = true;
        set_port_oper_delay(local_port);
        /*清除收包信息*/
        if (port->rx.framein)
        {
            GLUE_FREE(port->rx.framein);
            port->rx.framein = NULL;
        }        
    }  
    
    set_tlv_enabled(local_port,TLVID_8021(LLDP_8021QAZ_ETSCFG),val);
    set_tlv_enabled(local_port,TLVID_8021(LLDP_8021QAZ_ETSREC),val);
    set_tlv_enabled(local_port,TLVID_8021(LLDP_8021QAZ_PFC),val);
    set_tlv_enabled(local_port,TLVID_8021(LLDP_8021QAZ_APP),val);

    run_all_sm(port);
    somethingChangedLocal(local_port);    

    printf("port %d set dcbx %s!\n",local_port,(val?"enabled":"disabled"));
    
}

void set_qcn_enable(u32 local_port, bool val)
{
    struct ieee8021qau_data *data;
    struct lldp_port * port = NULL;
    
    port = find_by_port(local_port);
    if (!port)
    {
        printf("port error!\n");            
        return;
    }         

    data = ieee8021qau_data(local_port);
    if (NULL==data)
    {
        printf("data is NULL, port %d\n",local_port);
        return;
    }
    
    if (val==true)
    {
        data->active  = true;
        data->pending = true;
        set_port_oper_delay(local_port);
        /*清除收包信息*/
        if (port->rx.framein)
        {
            GLUE_FREE(port->rx.framein);
            port->rx.framein = NULL;
        }
    }

    set_tlv_enabled(local_port,TLVID_8021(LLDP_8021QAU_QCN),val);   
    somethingChangedLocal(local_port);        

    printf("port %d set qcn %s!\n",local_port,(val?"enabled":"disabled"));
    
}

void show_tc(TC *tc)
{
    u8 i,j;
    
    if (NULL==tc)
    {
        return;
    }
    
    for (i=0;i<MAX_TCS;i++)
    {
        if (tc[i].used)
        {
            printf("      tc:%-11dbw:%-10dtsa:%-10spri:", i, tc[i].bw, get_tsa_string(tc[i].tsa));  
            for (j=0;j<MAX_USER_PRIORITIES;j++)
            {
                if (tc[i].pri & (1<<j))
                {
                    printf("%d,",j);
                }
            }
            printf("\n");
        }
    }    
}

void lldp_show_ets(u32 local_port)
{
    struct ieee8021qaz_tlvs *tlvs;
    
    tlvs = ieee8021qaz_data(local_port);
    if (!tlvs)
    {
        printf("port error!\n");            
        return;    
    }  
   
    if (tlvs->ets)
    {
        printf("ETS   cur_state:%s\n", tlvs->ets->current_state?"rx_recommond":"state_init");   

        if (tlvs->ets->cfgl)
        {
            printf("local    willing:%-6scbs:%-8s", tlvs->ets->cfgl->willing?"true":"false", tlvs->ets->cfgl->cbs?"true":"false");
            printf("max_tcs:%-6dprio_map:%0x\n", tlvs->ets->cfgl->max_tcs, get_priomap_by_tcs(tlvs->ets->cfgl->tc_cur));

            printf("cfg:\n");
            show_tc(tlvs->ets->cfgl->tc_cfg);
            printf("cur:\n");
            show_tc(tlvs->ets->cfgl->tc_cur);            
            printf("rec:\n");
            show_tc(tlvs->ets->cfgl->tc_rec);
        }  

        printf("\n");

        if (tlvs->ets->cfgr)
        {
            printf("remote   willing:%-6scbs:%-8s", tlvs->ets->cfgr->willing?"true":"false", tlvs->ets->cfgr->cbs?"true":"false");
            printf("max_tcs:%-6dprio_map:%0x\n", tlvs->ets->cfgr->max_tcs, get_priomap_by_tcs(tlvs->ets->cfgr->tc_cur));

            printf("cur:\n");
            show_tc(tlvs->ets->cfgr->tc_cur);
            printf("rec:\n");
            show_tc(tlvs->ets->cfgr->tc_rec);
        }

    }
    
}

void show_pfc(struct pfc_obj *p_pfc)
{
    int i;
    
    if (NULL==p_pfc)
    {
        return;
    }

    printf("        willing:%-10smbc:%-10spfc_cap:%d\n",
           ((p_pfc->willing)?"true":"false"),((p_pfc->mbc)?"true":"false"),p_pfc->pfc_cap);
    printf("        support pfc:");

    for (i=0;i<MAX_USER_PRIORITIES;i++)
    {
        if (p_pfc->pfc_enable & (1<<i))
        {
            printf("%d,",i);
        }
    }

    printf("\n");
}

void lldp_show_pfc(u32 local_port)
{
    struct ieee8021qaz_tlvs *tlvs;
    
    tlvs = ieee8021qaz_data(local_port);
    if (!tlvs)
    {
        printf("port error!\n");            
        return;    
    }
   
    if (tlvs->pfc)
    {    
        printf("PFC     cur_state:%s\n",((tlvs->pfc->current_state)?"rx_recommond":"state_init"));
        printf("local:\n");
        show_pfc(&tlvs->pfc->local);
        printf("curl:\n");
        show_pfc(&tlvs->pfc->cur);
        printf("remote\n");
        show_pfc(&tlvs->pfc->remote);
    }    

}

void show_qcn(struct qcn_attrib *qcn, u8 type)
{
    int i;
    
    if (NULL==qcn)
    {
        return;
    }

    if (0==type)
    {
        for (i=0;i<MAX_USER_PRIORITIES;i++)
        {
            if (qcn->local.cnpv[i].used)
            {
                printf("pri:%-10dready:%-10dmode:%s\n",
                       i,qcn->local.cnpv[i].ready,get_cnpv_mode_string(qcn->local.cnpv[i].mode));
            }
        }
    }
    
    if (1==type)
    {
        for (i=0;i<MAX_USER_PRIORITIES;i++)
        {
            if (qcn->cur.cnpv[i].used)
            {
                printf("pri:%-10dready:%-10dmode:%s\n",
                       i,qcn->cur.cnpv[i].ready,get_cnpv_mode_string(qcn->cur.cnpv[i].mode));
            }
        }        
    }
    
    if (2==type)
    {
        for (i=0;i<MAX_USER_PRIORITIES;i++)
        {
            if (qcn->remote.cnpv[i].used)
            {
                printf("pri:%-10dready:%d\n",i,qcn->remote.cnpv[i].ready);
            }
        }        
    }

    printf("\n");
}

void lldp_show_qcn(u32 local_port)
{
    struct ieee8021qau_data *data;
    
    data = ieee8021qau_data(local_port);
    if (!data)
    {
        printf("port error!\n");            
        return;    
    }

    if (!data->qcn)
    {
        printf("no qcn info!\n");
        return;
    }   
  
    printf("QCN    active:%-15spending:%-13s8021qaudu:0x%-10x\n",
       (data->active?"true":"false"),(data->pending?"true":"false"),data->ieee8021qaudu); 

    if (is_cp_mode(local_port))
    {
        printf("local:\n");
        show_qcn(data->qcn,0);
        printf("curl:\n");
        show_qcn(data->qcn,1);
        printf("remote\n");
        show_qcn(data->qcn,2);
    }
    else
    {
        struct qcn_attrib qcn;
        u8 perPriCNPV;
        u8 perPriReady;
        u8 i;
            
        memset(&qcn, 0, sizeof(struct qcn_attrib));

        get_qcn_tlv_info(local_port, &perPriCNPV, &perPriReady);

        for (i=0;i<MAX_USER_PRIORITIES;i++)
        {
            if (perPriCNPV & (1<<i))
            {
                qcn.local.cnpv[i].used = 1;
                qcn.local.cnpv[i].mode = QCN_RP;
                qcn.cur.cnpv[i].used = 1;
                qcn.cur.cnpv[i].mode = QCN_RP;
            }

            if (perPriReady & (1<<i))
            {
                qcn.local.cnpv[i].ready = 1;
                qcn.cur.cnpv[i].ready = 1;
            }
        }

        printf("local:\n");
        show_qcn(&qcn,0);
        printf("curl:\n");
        show_qcn(&qcn,1);
        printf("remote\n");
        show_qcn(data->qcn,2);
    }
}

void lldp_show_lossless(u32 local_port)
{
    struct ieee8021qaz_tlvs *tlvs;
    struct ieee8021qau_data *data;

    tlvs = ieee8021qaz_data(local_port);
    if (!tlvs)
    {
        printf("port error!\n");    
        return;    
    }

    data = ieee8021qau_data(local_port);
    if (!data)
    {
        printf("port error!\n");    
        return;    
    }

    printf("====================================================================\n");      
    printf("PORT %d LOSSLESS INFO:\n",tlvs->local_port);
    printf("--------------------------------------------------------------------\n");    
    printf("active:%-15spending:%-13s8021qazdu:0x%-10x\n",
           (tlvs->active?"true":"false"),(tlvs->pending?"true":"false"),tlvs->ieee8021qazdu);
    printf("local_mac:" MACSTR "         ", MAC2STR(tlvs->local_mac));
    printf("remote_mac:" MACSTR "\n", MAC2STR(tlvs->remote_mac));
    printf("--------------------------------------------------------------------\n");    
    lldp_show_ets(local_port);
    printf("--------------------------------------------------------------------\n");        
    lldp_show_pfc(local_port);
    printf("--------------------------------------------------------------------\n");    
    lldp_show_qcn(local_port);
    printf("--------------------------------------------------------------------\n");       
    
}

void set_ets_willing(u32 local_port,u8 willing)
{
    struct ieee8021qaz_tlvs *tlvs;
    struct lldp_port * port = NULL;
    
    port = find_by_port(local_port);
    if (!port)
    {
        printf("port error!\n");            
        return;
    }   

    tlvs = ieee8021qaz_data(local_port);
    if (!tlvs)
    {
        return;    
    }    

    if (NULL==tlvs->ets || NULL==tlvs->ets->cfgl)
    {
        printf("NULL pointer!\n");
        return;
    }   

    tlvs->ets->cfgl->willing = (willing)?true:false;        

    run_all_sm(port);
    somethingChangedLocal(port->local_port);
    
    printf("ets willing set ok!\n");
    
}

void set_pfc_mode(u32 local_port, u8 mode)
{
    struct ieee8021qaz_tlvs *tlvs;
    struct lldp_port * port = NULL;
    
    port = find_by_port(local_port);
    if (!port)
    {
        printf("port error!\n");            
        return;
    }     
    
    tlvs = ieee8021qaz_data(local_port);
    if (!tlvs)
    {
        return;    
    }   

    if (NULL==tlvs->pfc)
    {
        printf("NULL pointer!\n");
        return;
    }      

    tlvs->pfc->local.willing = (mode)?true:false;

    run_all_sm(port);  
    somethingChangedLocal(port->local_port);    
    
    printf("pfc willing set ok!\n");    
    
}


void set_pfc(u32 local_port, u8 priority, u8 value)
{
    struct ieee8021qaz_tlvs *tlvs;
    struct lldp_port * port = NULL;
    
    port = find_by_port(local_port);
    if (!port)
    {
        printf("port error!\n");            
        return;
    }     
    
    tlvs = ieee8021qaz_data(local_port);
    if (!tlvs)
    {
        return;    
    }

    if (priority > 7)
    {
        printf("priority error!\n");
        return;
    }   

    if (NULL==tlvs->pfc)
    {
        printf("NULL pointer!\n");
        return;
    }   
 
    if (value)
    {
        tlvs->pfc->local.pfc_enable |= 1<<priority;
    }
    else
    {
        tlvs->pfc->local.pfc_enable &= ~(1<<priority);
    }   

    run_all_sm(port);  
    somethingChangedLocal(port->local_port);
    
    printf("pfc value set ok!\n");    
    
}

void set_pfc_cap(u32 local_port, u8 tc_num)
{
    struct ieee8021qaz_tlvs *tlvs;
    struct lldp_port * port = NULL;
    
    port = find_by_port(local_port);
    if (!port)
    {
        printf("port error!\n");            
        return;
    }     
    
    tlvs = ieee8021qaz_data(local_port);
    if (!tlvs)
    {
        return;    
    }

    if (tc_num < 1 || tc_num > 7)
    {
        printf("tc_num error!\n");
        return;
    }    

    if (NULL==tlvs->pfc)
    {
        printf("NULL pointer!\n");
        return;
    }      

    tlvs->pfc->local.pfc_cap = tc_num;

    run_all_sm(port);  
    somethingChangedLocal(port->local_port);
    
    printf("pfc cap set ok!\n");    
    
}

void set_mbc(u32 local_port, u8 value)
{
    struct ieee8021qaz_tlvs *tlvs;
    struct lldp_port * port = NULL;
    
    port = find_by_port(local_port);
    if (!port)
    {
        printf("port error!\n");            
        return;
    }     
    
    tlvs = ieee8021qaz_data(local_port);
    if (!tlvs)
    {
        return;    
    }

    if (NULL==tlvs->pfc)
    {
        printf("NULL pointer!\n");
        return;
    }    

    tlvs->pfc->local.mbc = (value?1:0);

    run_all_sm(port);  
    somethingChangedLocal(port->local_port);
    
    printf("pfc mbc set ok!\n");    
    
}

void create_tc(u32 local_port, u8 tc, u8 tc_type)
{
    struct ieee8021qaz_tlvs *tlvs;
    struct lldp_port * port = NULL;
    
    port = find_by_port(local_port);
    if (!port)
    {
        printf("port error!\n");            
        return;
    }     
    
    tlvs = ieee8021qaz_data(local_port);
    if (!tlvs)
    {
        return;    
    }

    if (tc >= MAX_TCS)
    {
        printf("tc is error!\n");
        return;
    }    

    if (NULL==tlvs->ets || NULL==tlvs->ets->cfgl)
    {
        printf("NULL pointer!\n");
        return;
    }

    if (!(CFG==tc_type || REC==tc_type))
    {
        printf("tc_type is error!\n");
        return;
    }

    if (CFG==tc_type)
    {
        tlvs->ets->cfgl->tc_cfg[tc].used = 1;
        tlvs->ets->cfgl->tc_cfg[tc].tsa  = IEEE8021Q_TSA_ETS;
    }
    
    if (REC==tc_type)
    {
        tlvs->ets->cfgl->tc_rec[tc].used = 1;
        tlvs->ets->cfgl->tc_rec[tc].tsa  = IEEE8021Q_TSA_ETS;       
    }
    
    run_all_sm(port);  
    somethingChangedLocal(port->local_port);
    
    printf("create tc %d ok!\n",tc);    
    
}

void delete_tc(u32 local_port, u8 tc, u8 tc_type)
{
    struct ieee8021qaz_tlvs *tlvs;
    struct lldp_port * port = NULL;
    
    port = find_by_port(local_port);
    if (!port)
    {
        printf("port error!\n");            
        return;
    }     
    
    tlvs = ieee8021qaz_data(local_port);
    if (!tlvs)
    {
        printf("port tlvs error!\n");            
        return;    
    }

    if (tc >= MAX_TCS)
    {
        printf("tc is error!\n");
        return;
    }

    if (NULL==tlvs->ets || NULL==tlvs->ets->cfgl)
    {
        printf("NULL pointer!\n");
        return;
    }

    if (!(CFG==tc_type || REC==tc_type))
    {
        printf("tc_type is error!\n");
        return;
    }

    if (CFG==tc_type)
    {
        memset(&(tlvs->ets->cfgl->tc_cfg[tc]),0,sizeof(TC));
    }
    
    if (REC==tc_type)
    {
        memset(&(tlvs->ets->cfgl->tc_rec[tc]),0,sizeof(TC));   
    }
    
    run_all_sm(port);  
    somethingChangedLocal(port->local_port);
    
    printf("delete tc %d ok!\n",tc);    
    
}

void join_pri_to_tc(u32 local_port, u8 tc, u8 pri, u8 tc_type)
{
    struct ieee8021qaz_tlvs *tlvs;
    struct lldp_port * port = NULL;
    int old_tc;
    
    port = find_by_port(local_port);
    if (!port)
    {
        printf("port error!\n");            
        return;
    }     
    
    tlvs = ieee8021qaz_data(local_port);
    if (!tlvs)
    {
        return;    
    }

    if (pri >= MAX_USER_PRIORITIES)
    {
        printf("priority is error!\n");
        return;
    }    

    if (NULL==tlvs->ets || NULL==tlvs->ets->cfgl)
    {
        printf("NULL pointer!\n");
        return;
    }    

    if (!(CFG==tc_type || REC==tc_type))
    {
        printf("tc_type is error!\n");
        return;
    }    

    if (tc >= MAX_TCS)
    {
        printf("tc is error!\n");
        return;
    }

    if (((CFG==tc_type) && (!tlvs->ets->cfgl->tc_cfg[tc].used))
        || ((REC==tc_type) && (!tlvs->ets->cfgl->tc_rec[tc].used))) 
    {
        printf("tc %d is not exist!\n",tc);
        return;
    }    

    if (-1!=(old_tc=find_tc_by_pri(local_port, pri, tc_type)))
    {
        if (old_tc == tc)
        {
            return;
        }
    }

    quit_pri_from_tc(local_port, (u8)old_tc, pri, tc_type);

    if (CFG==tc_type)
    {
        tlvs->ets->cfgl->tc_cfg[tc].pri |= (1<<pri);
    }
    
    if (REC==tc_type)
    {
        tlvs->ets->cfgl->tc_rec[tc].pri |= (1<<pri);
    }
    
    run_all_sm(port);  
    somethingChangedLocal(port->local_port);
    
    printf("join pri %d to tc %d ok!\n",pri,tc);    
    
}

void quit_pri_from_tc(u32 local_port, u8 tc, u8 pri, u8 tc_type)
{
    struct ieee8021qaz_tlvs *tlvs;
    struct lldp_port * port = NULL;
    
    port = find_by_port(local_port);
    if (!port)
    {
        printf("port error!\n");            
        return;
    }     
    
    tlvs = ieee8021qaz_data(local_port);
    if (!tlvs)
    {
        return;    
    }

    if (pri >= MAX_USER_PRIORITIES)
    {
        printf("priority is error!\n");
        return;
    }    
    
    if (NULL==tlvs->ets || NULL==tlvs->ets->cfgl)
    {
        printf("NULL pointer!\n");
        return;
    }    

    if (!(CFG==tc_type || REC==tc_type))
    {
        printf("tc_type is error!\n");
        return;
    }    

    if (tc >= MAX_TCS)
    {
        printf("tc is error!\n");
        return;
    }

    if (((CFG==tc_type) && (!tlvs->ets->cfgl->tc_cfg[tc].used))
        || ((REC==tc_type) && (!tlvs->ets->cfgl->tc_rec[tc].used))) 
    {
        printf("tc %d is not exist!\n",tc);
        return;
    }        

    if (CFG==tc_type)
    {
        tlvs->ets->cfgl->tc_cfg[tc].pri &= ~(1<<pri);
    }
    
    if (REC==tc_type)
    {
        tlvs->ets->cfgl->tc_rec[tc].pri &= ~(1<<pri);
    }
    
    run_all_sm(port);  
    somethingChangedLocal(port->local_port);
    
    printf("quit pri %d from tc %d ok!\n",pri,tc);    
    
}

int find_tc_by_pri(u32 local_port, u8 pri, u8 tc_type)
{
    struct ieee8021qaz_tlvs *tlvs;
    struct lldp_port * port = NULL;
    u8 i;
    
    port = find_by_port(local_port);
    if (!port)
    {
        printf("port error!\n");            
        return -1;
    }     
    
    tlvs = ieee8021qaz_data(local_port);
    if (!tlvs)
    {
        return -1;    
    }

    if (pri >= MAX_USER_PRIORITIES)
    {
        printf("priority is error!\n");
        return -1;
    }

    if (!(CFG==tc_type || REC==tc_type))
    {
        printf("tc_type is error!\n");
        return -1;
    }      

    if (CFG==tc_type)
    {
        for (i=0;i<MAX_TCS;i++)
        {   
            if (0==tlvs->ets->cfgl->tc_cfg[i].used)
            {
                continue;
            }
            
            if (tlvs->ets->cfgl->tc_cfg[i].pri & (1<<pri))
            {
                return i;
            }
        }
    }
    
    if (REC==tc_type)
    {
        for (i=0;i<MAX_TCS;i++)
        {       
            if (0==tlvs->ets->cfgl->tc_rec[i].used)
            {
                continue;
            }
            
            if (tlvs->ets->cfgl->tc_rec[i].pri & (1<<pri))
            {
                return i;
            }
        }
    }

    return -1;  
    
}

void set_tc_tsa(u32 local_port, u8 tc, u8 tsa, u8 tc_type)
{
    struct ieee8021qaz_tlvs *tlvs;
    struct lldp_port * port = NULL;
    int old_tc;
    
    port = find_by_port(local_port);
    if (!port)
    {
        printf("port error!\n");            
        return;
    }     
    
    tlvs = ieee8021qaz_data(local_port);
    if (!tlvs)
    {
        return;    
    } 

    if (tc >= MAX_TCS)
    {
        printf("tc is error!\n");
        return;
    }    

    if (NULL==tlvs->ets || NULL==tlvs->ets->cfgl)
    {
        printf("NULL pointer!\n");
        return;
    }    

    if (!(CFG==tc_type || REC==tc_type))
    {
        printf("tc_type is error!\n");
        return;
    }    
    
    if (IEEE8021Q_TSA_STRICT==tsa)
    {
        if (-1!=(old_tc=find_strict_tc(local_port, tc_type)))
        {
            if (old_tc == tc)
            {
                return;
            }

            if (CFG==tc_type)
            {
                tlvs->ets->cfgl->tc_cfg[old_tc].tsa = IEEE8021Q_TSA_ETS;
                tlvs->ets->cfgl->tc_cfg[tc].bw  = 0;
            }
            
            if (REC==tc_type)
            {
                tlvs->ets->cfgl->tc_rec[old_tc].tsa = IEEE8021Q_TSA_ETS;
                tlvs->ets->cfgl->tc_rec[tc].bw  = 0;
            }
        }
    }
    else
    {
        if (-1!=(old_tc=find_strict_tc(local_port, tc_type)))
        {
            if (old_tc == tc)
            {
                printf("tc %d tsa is strict,set fail!\n",tc);            
                return;
            }
        }    
    }

    if (CFG==tc_type)
    {
        if (0==tlvs->ets->cfgl->tc_cfg[tc].used)
        {
            printf("tc %d is not exist!\n",tc);
            return;        
        }      
        tlvs->ets->cfgl->tc_cfg[tc].tsa = tsa;
    }
    
    if (REC==tc_type)
    {
        if (0==tlvs->ets->cfgl->tc_rec[tc].used)
        {
            printf("tc %d is not exist!\n",tc);
            return;        
        }     
        
        tlvs->ets->cfgl->tc_rec[tc].tsa = tsa;
    }   
    
    run_all_sm(port);  
    somethingChangedLocal(port->local_port);
    
    printf("set tc %d tsa %d ok!\n",tc,tsa);    
    
}

int find_strict_tc(u32 local_port, u8 tc_type)
{
    struct ieee8021qaz_tlvs *tlvs;
    struct lldp_port * port = NULL;
    u8 i;
    
    port = find_by_port(local_port);
    if (!port)
    {
        printf("port error!\n");            
        return -1;
    }     
    
    tlvs = ieee8021qaz_data(local_port);
    if (!tlvs)
    {
        return -1;    
    }

    if (NULL==tlvs->ets || NULL==tlvs->ets->cfgl)
    {
        printf("NULL pointer!\n");
        return -1;
    }    

    if (!(CFG==tc_type || REC==tc_type))
    {
        printf("tc_type is error!\n");
        return -1;
    }  

    if (CFG==tc_type)
    {
        for (i=0;i<MAX_TCS;i++)
        {   
            if (0==tlvs->ets->cfgl->tc_cfg[i].used)
            {
                continue;
            }            
            
            if (IEEE8021Q_TSA_STRICT==tlvs->ets->cfgl->tc_cfg[i].tsa)
            {
                return i;
            }
        }
    }
    
    if (REC==tc_type)
    {
        for (i=0;i<MAX_TCS;i++)
        {  
            if (0==tlvs->ets->cfgl->tc_rec[i].used)
            {
                continue;
            }
            
            if (IEEE8021Q_TSA_STRICT==tlvs->ets->cfgl->tc_rec[i].tsa)
            {
                return i;
            }
        }
    }   

    return -1;
    
}

int is_strict_tc(u32 local_port, u8 tc, u8 tc_type)
{
    struct ieee8021qaz_tlvs *tlvs;
    
    tlvs = ieee8021qaz_data(local_port);
    if (!tlvs)
    {
        return -1;    
    }

    if (NULL==tlvs->ets || NULL==tlvs->ets->cfgl)
    {
        printf("NULL pointer!\n");
        return -1;
    }    

    if (!(CFG==tc_type || REC==tc_type))
    {
        printf("tc_type is error!\n");
        return -1;
    }  

    if (0==tlvs->ets->cfgl->tc_cfg[tc].used)
    {
        printf("tc %d is not exist!\n",tc);
        return -1;        
    }      

    if (CFG==tc_type)
    {
        if (IEEE8021Q_TSA_STRICT==tlvs->ets->cfgl->tc_cfg[tc].tsa)
        {
            return 1;
        }
    }
    
    if (REC==tc_type)
    {
        if (IEEE8021Q_TSA_STRICT==tlvs->ets->cfgl->tc_rec[tc].tsa)
        {
            return 1;
        }
    }   

    return 0;
    
}


void set_tc_bw(u32 local_port, u8 tc, u8 bw, u8 tc_type)
{
    struct ieee8021qaz_tlvs *tlvs;
    struct lldp_port * port = NULL;
    int bw_sum = 0;
    int total_bw = 0;
    
    port = find_by_port(local_port);
    if (!port)
    {
        printf("port error!\n");            
        return;
    }     
    
    tlvs = ieee8021qaz_data(local_port);
    if (!tlvs)
    {
        return;    
    } 

    if (tc >= MAX_TCS)
    {
        printf("tc is error!\n");
        return;
    }  

    if (bw > 100)
    {
        printf("bw must be in between 0~100!\n");
        return;
    }    

    if (NULL==tlvs->ets || NULL==tlvs->ets->cfgl)
    {
        printf("NULL pointer!\n");
        return;
    }    

    if (!(CFG==tc_type || REC==tc_type))
    {
        printf("tc_type is error!\n");
        return;
    }    

    if (0==tlvs->ets->cfgl->tc_cfg[tc].used)
    {
        printf("tc %d is not exist!\n",tc);
        return;
    }

    if (0!=is_strict_tc(local_port,tc,tc_type))
    {
        printf("tc %d is a strict tc,can not modify the bw!\n",tc);
        return;
    }

    if (-1==(bw_sum=get_sumbw_of_tcs(local_port,tc_type)))
    {
        printf("get bw_sum error!\n");        
        return;        
    }

    if (CFG==tc_type)
    {
        total_bw = bw_sum - tlvs->ets->cfgl->tc_cfg[tc].bw;
    }
    
    if (REC==tc_type)
    {
        total_bw = bw_sum - tlvs->ets->cfgl->tc_rec[tc].bw;
    }

    if (total_bw + bw > 100)
    {
        printf("total bw has high then 100!\n");        
        return; 
    }

    if (CFG==tc_type)
    {
        tlvs->ets->cfgl->tc_cfg[tc].bw = bw;
    }
    
    if (REC==tc_type)
    {
        tlvs->ets->cfgl->tc_rec[tc].bw = bw;
    }   
    
    run_all_sm(port);  
    somethingChangedLocal(port->local_port);
    
    printf("set tc %d bw %d ok!\n",tc,bw);    
    
}

int get_sumbw_of_tcs(u32 local_port, u8 tc_type)
{
    struct ieee8021qaz_tlvs *tlvs;
    int i;
    int bw_sum = 0;
        
    tlvs = ieee8021qaz_data(local_port);
    if (!tlvs)
    {
        return -1;    
    }

    if (NULL==tlvs->ets || NULL==tlvs->ets->cfgl)
    {
        printf("NULL pointer!\n");
        return -1;
    }    

    if (!(CFG==tc_type || REC==tc_type))
    {
        printf("tc_type is error!\n");
        return -1;
    } 

    if (CFG==tc_type)
    {
        for (i=0;i<MAX_TCS;i++)
        {   
            if (0==tlvs->ets->cfgl->tc_cfg[i].used)
            {
                continue;
            }    

            bw_sum += tlvs->ets->cfgl->tc_cfg[i].bw;
        }  
    }
    
    if (REC==tc_type)
    {
        for (i=0;i<MAX_TCS;i++)
        {   
            if (0==tlvs->ets->cfgl->tc_rec[i].used)
            {
                continue;
            }    

            bw_sum += tlvs->ets->cfgl->tc_rec[i].bw;
        }  
    }   
    
    return bw_sum;    
}

int cnpv_num_ok(u32 local_port,u8 opt)
{
    u8 i;
    u8 count = 0;    
    struct ieee8021qau_data *data;

    data = ieee8021qau_data(local_port);
    if (!data)
    {
        return 0;    
    }

    if (NULL==data->qcn)
    {
        printf("NULL pointer!\n");
        return 0;
    }

    if (1==opt)
    {
        for (i=0;i<MAX_USER_PRIORITIES;i++)
        {
            if (data->qcn->local.cnpv[i].used)
            {
                count++;
            }
        }

        if (count >= MAX_USER_PRIORITIES - 1 )
        {
            printf("The number of cnpv has reached the maximum(7)!\n");
            return 0;
        }
    }

    return 1;    
}


void set_cnpv(u32 local_port, u8 pri, u8 status)
{
    struct ieee8021qau_data *data;
    struct lldp_port * port = NULL;
    
    port = find_by_port(local_port);
    if (!port)
    {
        printf("port error!\n");            
        return;
    }     
    
    data = ieee8021qau_data(local_port);
    if (!data)
    {
        return;    
    }

    if (pri >= MAX_USER_PRIORITIES)
    {
        printf("priority is error!\n");
        return;
    }      

    if (NULL==data->qcn)
    {
        printf("NULL pointer!\n");
        return;
    }
    
    if (( status && (data->qcn->local.cnpv[pri].used)) ||
        (!status && (!data->qcn->local.cnpv[pri].used)))
    {
        printf("qcn cnpv %d set ok!\n",pri);
        return;
    }

    if (1!=cnpv_num_ok(local_port,status))
    {
        printf("The num of cnpv is error!\n");
        return;
    }    

    data->qcn->local.cnpv[pri].used  = (status?1:0);
    data->qcn->local.cnpv[pri].mode  = QCN_AUTO;
    data->qcn->local.cnpv[pri].ready = 1;
    
    run_qcn_sm(port);  
    somethingChangedLocal(port->local_port);    
    
    printf("qcn cnpv %d set ok!\n",pri);    
    
}

void set_cnpv_mode(u32 local_port, u8 pri, u8 mode)
{
    struct ieee8021qau_data *data;
    struct lldp_port * port = NULL;
    
    port = find_by_port(local_port);
    if (!port)
    {
        printf("port error!\n");            
        return;
    }     
    
    data = ieee8021qau_data(local_port);
    if (!data)
    {
        printf("port error!\n");
        return;    
    }

    if (pri >= MAX_USER_PRIORITIES)
    {
        printf("priority is error!\n");
        return;
    }      

    if (NULL==data->qcn)
    {
        printf("NULL pointer!\n");
        return;
    }

    if (mode > QCN_AUTO)
    {
        printf("mode is error!\n");
        return;
    }  
    
    data->qcn->local.cnpv[pri].mode = mode;
    data->qcn->local.cnpv[pri].ready = (QCN_ECP==mode)?0:1;    

    run_qcn_sm(port);  
    somethingChangedLocal(port->local_port);
    
    printf("qcn cnpv %d mode set to %d ok!\n",pri,mode);    
    
}

void set_lldp_agent_mode(u32 local_port, u8 agent_mode)
{
    struct lldp_port *port;

    port = find_by_port(local_port);
    if (NULL==port)
    {
        printf("port error!\n");        
        return;
    }

    if (agent_mode > NearestCustomerBridge)
    {
        printf("agent mode must be: 0---NearestBridge,  1---NearestNonTPMRBridge,   2---NearestCustomerBridge\n");
        return;
    }

    lldp_agent_mode(local_port,agent_mode);

    printf("port %d lldp agent mode set to %d ok!\n",local_port,agent_mode);

}


void lldp_help(void)
{
    printf("===============================================================================\n"); 
    printf("------LLDP Config--------------------------------------------------------------\n");
    printf("| lldp_port_up(u32 local_port)\n");
    printf("| lldp_port_down(u32 local_port)\n");
    printf("| set_lldp_mode(u32 local_port)\n");
    printf("| set_dcbx_enable(u32 local_port,bool val)\n");
    printf("| set_qcn_enable(u32 local_port,bool val)\n");
    printf("| set_ets_willing(u32 local_port,u8 willing)\n");
    printf("| set_pfc_mode(u32 local_port, u8 mode)\n");
    printf("| set_pfc(u32 local_port, u8 priority, u8 value)\n");
    printf("| set_pfc_cap(u32 local_port, u8 tc_num)\n");
    printf("| set_mbc(u32 local_port, u8 value)\n");
    printf("| create_tc(u32 local_port, u8 tc, u8 tc_type)\n");
    printf("| delete_tc(u32 local_port, u8 tc, u8 tc_type)\n");
    printf("| join_pri_to_tc(u32 local_port, u8 tc, u8 pri, u8 tc_type)\n");
    printf("| quit_pri_from_tc(u32 local_port, u8 tc, u8 pri, u8 tc_type)\n");
    printf("| find_tc_by_pri(u32 local_port, u8 pri, u8 tc_type)\n");
    printf("| set_tc_tsa(u32 local_port, u8 tc, u8 tsa, u8 tc_type)\n");
    printf("| find_strict_tc(u32 local_port, u8 tc_type)\n");
    printf("| set_tc_bw(u32 local_port, u8 tc, u8 bw, u8 tc_type)\n");
    printf("| get_sumbw_of_tcs(u32 local_port, u8 tc_type)\n");
    printf("| cnpv_num_ok(u32 local_port,u8 opt)\n");
    printf("| set_cnpv(u32 local_port, u8 pri, u8 status)\n");
    printf("| set_lldp_agent_mode(u32 local_port, u8 agent_mode)\n");
    printf("------LLDP Debug-------------------------------------------------------------\n");
    printf("| lldp_show_rx_sm(u32 local_port)\n");
    printf("| lldp_show_tx_sm(u32 local_port)\n");
    printf("| stt_show_lldp(u32 local_port)\n");
    printf("| stt_clear_lldp(u32 local_port)\n");
    printf("| lldp_show_porttimer_sm(u32 local_port)\n");
    printf("| lldp_show_info_one(u32 local_port)\n");
    printf("| lldp_show_info(u32 local_port)\n");
    printf("| lldp_show_port(void)\n");
    printf("| lldp_show_ets(u32 local_port)\n");
    printf("| lldp_show_pfc(u32 local_port)\n");
    printf("| lldp_show_qcn(u32 local_port)\n");
    printf("| lldp_show_lossless(u32 local_port)\n"); 
    printf("===============================================================================\n");
}

#ifdef __cplusplus
}
#endif



