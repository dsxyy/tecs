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


#ifdef V4_VERSION

struct timer lldp_timer;
/*四个lldp模块必须实现的回调函数*/
void v4_lldp_port_init(void)
{
    net_if *pNet = NULL;

    for (pNet = If_list; pNet; pNet = pNet->link)
    {
        if (!is_support_lldp(IF_PORT(pNet)))
        {
            continue;
        }
        
        create_lldp_port(IF_PORT(pNet),pNet->s_name,pNet->h_address,NearestBridge);
    }
}

void v4_lldp_tm_init(void)
{
    if (!IN_USE(&lldp_timer)) 
    {
        /* 产生 Tick定时器*/
        etc_tm_init(&lldp_timer);
        lldp_timer.handler = v4_lldp_timeout;
        lldp_timer.cookie  = NULL;
    }
}

void v4_lldp_tm_set(void)
{
    etc_tm_set(&lldp_timer, (bits32_t)LLDP_TIMER_FREQUENCY);
}

void v4_lldp_tm_stop(void)
{
    etc_tm_cancel(&lldp_timer);
}

int v4_lldp_send_pkt(u32 local_port, u8 *pkt, u16 len)
{
    net_if *pNet = NULL; 
    packet *p = NULL;

    if (NULL==pkt || len < 64)
    {
        return -1;
    }

    if (NULL==(pNet = getLocal_OuterNetIf(local_port)))
    {
        return -1;
    }

    p = pkt_alloc("lldp",1518);
    if (NULL == p)
    { 
        return -1;
    }      

    p->pkt_n          = pNet;
    p->localport      = local_port;
    p->pkt_datalen    = len;
    p->real_send_port = local_port;

    MEMCPY(p->pkt_data,(BYTE *)pkt, len);

    if (pNet->driver && pNet->driver->send)
    {
        (*pNet->driver->send)(p);
    }

    pkt_free(p);

    return 0;
}


/*以下是V4需要的逻辑处理函数*/
void lldp_port_status_change(T_PORT_STATUS *ptPortStatus)
{  
    net_if *pNet = NULL;
    
    if (NULL == ptPortStatus)
    {
        return;
    }

    pNet = getLocal_OuterNetIf(ptPortStatus->ucPortNO);
    if (NULL == pNet)
    {
        return;
    } 

    if (PORT_WORKING_STATUS==ptPortStatus->ucPortStatus)
    {
        lldp_port_up(IF_PORT(pNet));
    }
    else
    {
        lldp_port_down(IF_PORT(pNet));
    }    
}

void lldp_rcv(packet *p)
{
    if (NULL==p)
    {
        return;
    }

    if (NULL == p->pkt_data||NULL == p->pkt_n)
    {             
        pkt_free(p);
        return;
    }

    if(!IsMainJob())
    {
        pkt_free(p);
        return;
    }

    if (gtLLDP.pfRcvPkt)
    {
        gtLLDP.pfRcvPkt(IF_PORT(p->pkt_n),(BYTE *)p->pkt_data,(WORD)p->pkt_datalen);
    }    
    
    pkt_free(p);
}

void v4_lldp_timeout(struct timer *ptTm, void *cookie)
{
    if (gtLLDP.pfTmout)
    {
        gtLLDP.pfTmout();
    }
}



void hba_82599_lldp_port_init(void)
{
    BYTE mac[ETH_ALEN] = {0x00,0xd0,0xd0,0xaa,0xbb,0xcc};
    create_lldp_port(HBA_82599_LLDP_PORT_ID,"lldp_port",mac,NearestBridge);
}

int hba_82599_lldp_send_pkt(u32 local_port, u8 *pkt, u16 len)
{
    if (NULL==pkt || len < 64)
    {
        return -1;
    }

    if(FCOE_DEBUG_PKT_LLDP)
    {
        fcoe_print_pkt(pkt, local_port, len, ETHERNET_TYPE_is_LLDP, FCOE_PKT_SEND);
    }

    fcoe_rawsocket_send(pkt, len);

    return 0;
}

void hba_82599_lldp_init()
{
    lldp_init(&hba_82599_lldp_port_init,&v4_lldp_tm_init,&v4_lldp_tm_set,&v4_lldp_tm_stop,&hba_82599_lldp_send_pkt);
}

void hba_82599_lldp_rcv (FCOE_PACKET *p)
{
    if (NULL==p)
    {
        return;
    }

    if (NULL == p->pkt_data)
    {             
        FCOE_GLUE_FREE(p);
        return;
    }
    if(FCOE_DEBUG_PKT_LLDP)
    {
        fcoe_print_pkt(p->pkt_buffer, p->localport, p->pkt_datalen, ETHERNET_TYPE_is_LLDP, FCOE_PKT_RCV);
    }

    if(!IsMainJob())
    {
        FCOE_GLUE_FREE(p);
        return;
    }

    if (gtLLDP.pfRcvPkt)
    {
        gtLLDP.pfRcvPkt(HBA_82599_LLDP_PORT_ID, (BYTE *)p->pkt_data,(WORD)p->pkt_datalen);
    }    
    
    FCOE_GLUE_FREE(p);
}
#endif

#ifdef SWITCH_VERSION
u32 switch_slot2port(u32 dwSlot)
{
    WORD32         i = 0;
    PA_STATUS      dwRet = PA_ERR;    
    T_PhysAddress  tSelfPa;
    T_PA_PhyAddr   tAsidePa;
    WORD16         wPortNum = 0;
    WORD32         adwPortId[PA_MAX_PHYPORT] = {0};   /* 后续要用宏取代 */
 
    if (dwSlot > MAX_SLOT)
    {
        return 0;
    }
    
    memset(&tSelfPa, 0 , sizeof(T_PhysAddress));
    XOS_GetPhysAddress(&tSelfPa);       

    /* 获取物理端口个数 */
    tAsidePa.ucRackId  = tSelfPa.ucRackId;
    tAsidePa.ucShelfId = tSelfPa.ucShelfId;
    tAsidePa.ucSlotId  = dwSlot;
    tAsidePa.ucCpuId   = 1;
    wPortNum = PA_IF_GetPortNum(&tSelfPa, PA_PT_PHY, PA_PHY_ATTR_ASIDEPA, &tAsidePa);
    if (0 == wPortNum)
    {
        return 0;
    }
    
    /* 获取物理端口 id */
    dwRet = PA_IF_GetPorts(&tSelfPa, PA_PT_PHY, PA_PHY_ATTR_ASIDEPA, &tAsidePa, adwPortId, wPortNum);
    if (PA_OK != dwRet)
    {
        return 0;
    } 
    
    for (i = 0;i < wPortNum;i++)
    {
        WORD32 dwTrunkId = 0;

        /* 获取端口TRUNK id */
        PA_IF_ReadPort(&tSelfPa, adwPortId[i], PA_PHY_ATTR_TRUNKID, &dwTrunkId);
        if ((PA_INVALID_TRUNKID != dwTrunkId) && (0 != dwTrunkId))
        {
            return dwTrunkId;
        }
        
        if (iss_switch_isportup(adwPortId[i]))
        {
            return adwPortId[i];
        } 
    }

    return 0;
    
}

u32 switch_port2slot(u32 dwPort)
{    
    PA_STATUS    dwRet = PA_ERR;
    T_PA_PhyAddr tPhyAddr;
    
    /* 获取端口 架框槽 */   
    dwRet = PA_IF_ReadPort(&g_tPhysAddr, dwPort, PA_PHY_ATTR_ASIDEPA, &tPhyAddr);
    if(PA_OK != dwRet)
    {
        return 0;
    }
    
    return tPhyAddr.ucSlotId;
}

u32 switch_lldp_port_up(void *pMsgPara)
{
    WORD16 wPortType = 0;
    T_PA_TrunkPort *ptNTrunkPort = NULL;
    T_PA_PortChgNoti *ptPortChgNoti = (T_PA_PortChgNoti *)pMsgPara;
    if (NULL == ptPortChgNoti)
    {
        return RET_FAILURE;
    }

    ISS_LOG(ISS_LOG_INFO, ISS_LOG_MOD_LD, 0, "switch_lldp_port_up\n");
    
    wPortType = PA_IF_GetPortTypeFromPortID(ptPortChgNoti->dwPortID);
    if (PA_PT_PHY == wPortType)
    {
        ISS_LOG(ISS_LOG_INFO, ISS_LOG_MOD_LD, 0, "switch_lldp_port_up,phy port up\n");

        /* 物理口不处理 */
        return RET_SUCCESS;
    }
    
    ptNTrunkPort = (T_PA_TrunkPort *)ptPortChgNoti->pucPort;
    if (NULL == ptNTrunkPort)
    {
        return RET_FAILURE;
    }    

    /* 堆叠口过滤掉 */
    if (ISS_ETH_PORTFUNC_STACK == ptNTrunkPort->port.tTrunkPortPre.tOrdPortAttr.dwFuncType)
    {
        ISS_LOG(ISS_LOG_INFO, ISS_LOG_MOD_LD, 0, "switch_lldp_port_up,duidiekou port up\n");
        return RET_SUCCESS;
    }
            
    lldp_port_up((WORD32)ptNTrunkPort->port.tTrunkPortPre.tAsidePhyAddr.ucSlotId);

    ISS_LOG(ISS_LOG_INFO, ISS_LOG_MOD_LD, 0, 
            "switch_lldp_port_up,trunk up,slot %d\n",(WORD32)ptNTrunkPort->port.tTrunkPortPre.tAsidePhyAddr.ucSlotId);

    return RET_SUCCESS;    
    
}

u32 switch_lldp_port_down(void *pMsgPara)
{
    WORD16 wPortType = 0;
    T_PA_TrunkPort *ptNTrunkPort = NULL;
    T_PA_PortChgNoti *ptPortChgNoti = (T_PA_PortChgNoti *)pMsgPara;
    if (NULL == ptPortChgNoti)
    {
        return RET_FAILURE;
    }
    
    wPortType = PA_IF_GetPortTypeFromPortID(ptPortChgNoti->dwPortID);
    if (PA_PT_PHY == wPortType)
    {
        ISS_LOG(ISS_LOG_INFO, ISS_LOG_MOD_LD, 0, "switch_lldp_port_down,phy port up\n");
        /* 物理口不处理 */
        return RET_SUCCESS;
    }
    
    ptNTrunkPort = (T_PA_TrunkPort *)ptPortChgNoti->pucPort;
    if (NULL == ptNTrunkPort)
    {
        return RET_FAILURE;
    }    

    /* 堆叠口过滤掉 */
    if (ISS_ETH_PORTFUNC_STACK == ptNTrunkPort->port.tTrunkPortPre.tOrdPortAttr.dwFuncType)
    {
        ISS_LOG(ISS_LOG_INFO, ISS_LOG_MOD_LD, 0, "switch_lldp_port_down,duidiekou port up\n");
        return RET_SUCCESS;
    }
            
    lldp_port_down((WORD32)ptNTrunkPort->port.tTrunkPortPre.tAsidePhyAddr.ucSlotId);

    ISS_LOG(ISS_LOG_INFO, ISS_LOG_MOD_LD, 0, 
             "switch_lldp_port_down,trunk down,slot %d\n",(WORD32)ptNTrunkPort->port.tTrunkPortPre.tAsidePhyAddr.ucSlotId);    

    return RET_SUCCESS;    
   
}

void switch_lldp_port_init(void)
{
    WORD32 dwIndex = 0;    
    u8 name[32] = {0};
    WORD32 dwRet = 0;  

    for (dwIndex = 0; dwIndex < MAX_SLOT; dwIndex++)
    {
        XOS_snprintf((char *)name, 32, "interface 0x%x", dwIndex + 1);
        create_lldp_port(dwIndex + 1,name, g_PhyInfo.ucSelfmac, NearestBridge);
    }  

    /* 调用端口子代理的注册函数 */
    dwRet = PA_IF_REGF_Callback(switch_lldp_port_up,PA_PT_TRUNK, PA_CALLBACK_UP);
    if(PA_OK != dwRet)
    {
        ISS_LOG(ISS_LOG_INFO, ISS_LOG_MOD_LD, 0, "switch_lldp_port_up reg fail!\n");    
        return;
    }

    ISS_LOG(ISS_LOG_INFO, ISS_LOG_MOD_LD, 0, "switch_lldp_port_up reg ok!\n");    

    dwRet = PA_IF_REGF_Callback(switch_lldp_port_down,PA_PT_TRUNK, PA_CALLBACK_DOWN);
    if(PA_OK != dwRet)
    {
        ISS_LOG(ISS_LOG_INFO, ISS_LOG_MOD_LD, 0, "switch_lldp_port_down reg fail!\n");    
        return;
    }

    ISS_LOG(ISS_LOG_INFO, ISS_LOG_MOD_LD, 0, "switch_lldp_port_down reg ok!\n");    
    
}

void switch_lldp_tm_set(void)
{
    iss_switch_relativetimerset(ISS_SW_TIMER_FAST, ISS_SW_TIMER_FAST_PERIOD, PARAM_LLDP);
}

void switch_lldp_tm_stop(void)
{
    iss_switch_killrelativetimer(ISS_SW_TIMER_FAST, PARAM_LLDP);
}

int switch_lldp_send_pkt(u32 local_port, u8 *pkt, u16 len)
{
    SWORD32 nRet = 0;
    WORD32  dwPort = 0;

    if (0==(dwPort=switch_slot2port(local_port)))
    {
        MIB_COUNTER_INC(g_tMedia.dwLLDPSndPkgFail);
        return RET_FAILURE;        
    }

    /* 报文发送 */
    nRet = issio_write(ISS_DRV_SWITCH, 0, pkt, len, dwPort, 0, 0);
    if(-1 == nRet)
    {
        MIB_COUNTER_INC(g_tMedia.dwLLDPSndPkgFail);
        return RET_FAILURE;
    }

    MIB_COUNTER_INC(g_tMedia.dwLLDPSndPkg);

    return RET_SUCCESS;    
}

/**********************************************************************
* 函数名称：SWORD32 issio_pfcset
* 功能描述：acl规则设置
* 访问的表：无
* 修改的表：无
* 输入参数：T_HW_PFC *pHwPfc, u8 ucOpt
* 输出参数：
* 返回值     ：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2011\09\19    1.0     xu.binbin   独立交换支持流控
************************************************************************/
SWORD32 issio_pfcset(WORD32 dwPort, BYTE ucQos, BYTE ucMbc, BYTE ucEnabled, BYTE ucOpt)
{
    WORD32 dwRet = 0;
    T_LOGIC_PORT tLogicPort;

    if(NULL == g_atIssVtIo[ISS_BSP].pfIssSwitchSetPfc)
    {
        ISS_LOG(ISS_LOG_INFO, MUX, 0, 
                "issio_pfcsets: func is null!\n");

        return E_ISS_INTF_MUX_NULL; 
    }

    if (0==(dwPort=switch_slot2port(dwPort)))
    {
        return E_ISS_INTF_PARA;        
    }    

    dwRet = issio_port2logic(dwPort, &tLogicPort);
    if(ISS_INTF_SUC != dwRet)
    {
        ISS_LOG(ISS_LOG_INFO, MUX, 0, 
                "issio_pfcset: port(%x) to logic failed(%x)!\n", dwPort, dwRet);    
        return dwRet;
    }

    dwRet = g_atIssVtIo[ISS_BSP].pfIssSwitchSetPfc(&tLogicPort, ucQos, ucMbc, ucEnabled, ucOpt);
    if(0 != dwRet)
    {
        ISS_LOG(ISS_LOG_INFO, MUX, 0, 
                "issio_pfcset: port(%x) ucQos(%u) ucEnabled(%d) failed(0x%x)!\n", \
                 dwPort, ucQos, ucEnabled, dwRet);

        return dwRet;
    }

    return ISS_INTF_SUC;
}

/**********************************************************************
* 函数名称：SWORD32 issio_pfcset
* 功能描述：acl规则设置
* 访问的表：无
* 修改的表：无
* 输入参数：T_HW_PFC *pHwPfc, u8 ucOpt
* 输出参数：
* 返回值     ：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2011\09\19    1.0     xu.binbin   独立交换支持流控
************************************************************************/
SWORD32 issio_qcn_enable_set(WORD32 dwPort, BYTE ucQos, BYTE ucEnabled)
{
    WORD32 dwRet = 0;
    T_LOGIC_PORT tLogicPort;    

    if(NULL == g_atIssVtIo[ISS_BSP].pfIssSwitchSetQcnEnable)
    {
        ISS_LOG(ISS_LOG_INFO, MUX, 0, 
                "issio_qcn_enable_set: func is null!\n");

        return E_ISS_INTF_MUX_NULL; 
    }

    if (0==(dwPort=switch_slot2port(dwPort)))
    {
        return E_ISS_INTF_PARA;        
    }    

    dwRet = issio_port2logic(dwPort, &tLogicPort);
    if(ISS_INTF_SUC != dwRet)
    {
        ISS_LOG(ISS_LOG_INFO, MUX, 0, 
                "issio_pfcset: port(%x) to logic failed(%x)!\n", dwPort, dwRet);    
        return dwRet;
    }


    dwRet = g_atIssVtIo[ISS_BSP].pfIssSwitchSetQcnEnable(&tLogicPort, ucQos, ucEnabled);
    if(0 != dwRet)
    {
        ISS_LOG(ISS_LOG_INFO, MUX, 0, 
                "issio_qcn_enable_set: port(%x) ucQos(%u) ucEnabled(%d) failed(0x%x)!\n", \
                 dwPort, ucQos, ucEnabled, dwRet);

        return dwRet;
    }

    return ISS_INTF_SUC;
}

/**********************************************************************
* 函数名称：SWORD32 issio_pfcset
* 功能描述：acl规则设置
* 访问的表：无
* 修改的表：无
* 输入参数：T_HW_PFC *pHwPfc, u8 ucOpt
* 输出参数：
* 返回值     ：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2011\09\19    1.0     xu.binbin   独立交换支持流控
************************************************************************/
SWORD32 issio_qcn_mode_set(WORD32 dwPort, BYTE ucQos, BYTE ucMode)
{
    WORD32 dwRet = 0;
    T_LOGIC_PORT tLogicPort;        

    if(NULL == g_atIssVtIo[ISS_BSP].pfIssSwitchSetQcnMode)
    {
        ISS_LOG(ISS_LOG_INFO, MUX, 0, 
                "issio_qcn_mode_set: func is null!\n");

        return E_ISS_INTF_MUX_NULL; 
    }

    if (0==(dwPort=switch_slot2port(dwPort)))
    {
        return E_ISS_INTF_PARA;        
    }    

    dwRet = issio_port2logic(dwPort, &tLogicPort);
    if(ISS_INTF_SUC != dwRet)
    {
        ISS_LOG(ISS_LOG_INFO, MUX, 0, 
                "issio_pfcset: port(%x) to logic failed(%x)!\n", dwPort, dwRet);    
        return dwRet;
    }    

    dwRet = g_atIssVtIo[ISS_BSP].pfIssSwitchSetQcnMode(&tLogicPort, ucQos, ucMode);
    if(0 != dwRet)
    {
        ISS_LOG(ISS_LOG_INFO, MUX, 0, 
                "issio_qcn_mode_set: port(%x) ucQos(%u) ucEnabled(%d) failed(0x%x)!\n", \
                 dwPort, ucQos, ucMode, dwRet);

        return dwRet;
    }

    return ISS_INTF_SUC;
}

#endif



void lldp_timeout(void)
{
    struct lldp_port *port = porthead;
    struct lldp_module *n;

    if (gtLLDP.pfTmSet)
    {
        gtLLDP.pfTmSet();
    }
    
    while (port != NULL) 
    {
        update_tx_timers(port);
        run_tx_timers_sm(port);
        run_tx_sm(port);
        update_rx_timers(port);        
        run_rx_sm(port);
        LIST_FOREACH(n, &gtLLDP.mod_head, lldp) 
        {
            if (n->ops && n->ops->timer)
            {
                n->ops->timer(port);
            }
        }
        
        if (port->timers.dormantDelay)
        {
            LLDPAD_DBG("port->timers.dormantDelay: %d\n",port->timers.dormantDelay);
            port->timers.dormantDelay--;            
        }
        port = port->next;
    };
}

void start_lldp_agent(void)
{
    if (gtLLDP.pfTmSet)
    {
        gtLLDP.pfTmSet();
    }
}

void stop_lldp_agent(void)
{
    if (gtLLDP.pfTmStop)
    {
        gtLLDP.pfTmStop();
    }    
}

void clean_lldp_agent(void)
{
    struct lldp_port *port = porthead;

    while (port != NULL) 
    {
        if (port_needs_shutdown(port))
        {
            LLDPAD_DBG("Send shutdown frame on port %d\n",port->local_port);
            process_tx_shutdown_frame(port);
        } 
        else
        {
            LLDPAD_DBG("No shutdown frame is sent on port %d\n",port->local_port);
        }
        port = port->next;
    }
}

void lldp_agent_mode(u32 local_port, u8 agent_mode)
{
    struct lldp_port *port;

    port = find_by_port(local_port);
    if (NULL==port)
    {
        return;
    }

    if (port->agent_mode == agent_mode)
    {
        return;
    }

    port->agent_mode = agent_mode;

    switch (port->agent_mode)
    {
        case NearestBridge:
            memcpy(port->dmac, multi_cast_mac1, ETH_ALEN);
            break;
        case NearestNonTPMRBridge:
            memcpy(port->dmac, multi_cast_mac2, ETH_ALEN);
            break;
        case NearestCustomerBridge:
            memcpy(port->dmac, multi_cast_mac3, ETH_ALEN);
            break;
        default:
            memcpy(port->dmac, multi_cast_mac1, ETH_ALEN);
    }

    lldp_reinit_port(port->local_port);
    somethingChangedLocal(port->local_port);
    run_tx_sm(port);
    run_rx_sm(port);    
   
}

#ifdef __cplusplus
}
#endif


