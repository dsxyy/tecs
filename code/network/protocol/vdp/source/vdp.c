#include "evb_comm.h"
#include "ecp_in.h"
#include "ecp.h"
#include "vdp.h"
#include "evb_cfg.h"
#include "vsi_profile.h"
#include "vsi_b_outFilter.h"
#include "evb_port_mgr.h"

#ifdef __cplusplus
extern "C"{
#endif 

T_VDP gtVDP;
struct tagVsiCmdPoolItem_head g_lstVsiCmdPool;
const char* vdpresponse_err_types[]=
{
    "success",
    "invalid formate",
    "insuff resource",
    "unable conn vsi mgr",
    "other fail",
    "invalid_vid_grpid_mac",
};

const char * vsiid_format_values[]=
{
    "null"
    "ipv4",
    "ipv6",
    "mac",
    "local",
    "uuid",
};

const char * fltInfo_format_values[]=
{
    "null"
    "VID",
    "MACVID",
    "GROUPVID",
    "MACGROUPVID",
};

const char* vdp_b_sm_state[]=
{
    "idle",
    "init",
    "bridge_processing",
    "success",
    "keep",
    "deassoc",
    "deassociated",
    "release_all",
    "wait_station_cmd",
    "process_keepalive",
};

const char* vdp_b_sm_event[]=
{
    "begin",
    "bridge_exit",
    "rxcmd_valid",
    "rr_success",
    "rr_keep",
    "rr_deassociate",
    "rr_timeout",
    "uct",
    "rxcmd_notequ_opercmd",
    "rxcmd_equ_opercmd",
    "waitwhile_equ_zero",
    "port_up",
    "port_down",
    "evbtlv_no_support",
};


const char * vdp_s_sm_state[]=
{
    "idle",
    "init",
    "transmit_deassoc",
    "station_proc",
    "preassoc_new",
    "assoc_new",
    "assoc_complete",
    "wait_sys_cmd",
    "transmit_keepalive",
};


const char* vdp_s_sm_event[] = 
{
    "begin",
    "newcmd",
    "preassoc_new",
    "assoc_new",
    "assoc_complete",
    "uct",
    "waitwhile_equ_zero",
    "rxresp_tlvtype_equ_deassoc",
    //"transmit_deassoc",
    "rxresp_tvltype_equ_assoc_and_keep",
    "rxresp_reason_equ_zero",
    "opercmd_exceptreason_else",
    "syscmd_exceptreason_else",
    "rxresp is not null",
    "port_up",
    "port_down",
    //"cmd_cancel",
};

// vdp tlv 是否有效 =1有效; =0 无效;
int  VdpTlvInvalid(UINT16 type)
{
    switch(type)
    {
    case VDPTLV_TYPE_PRE_ASSOCIATE:
    case VDPTLV_TYPE_PRE_ASSOCIATE_WITH_RR:
    case VDPTLV_TYPE_ASSOCIATE:
    case VDPTLV_TYPE_DEASSOCIATE:
    case VDPTLV_TYPE_VSI_MGRID:
    case VDPTLV_TYPE_ORG_DEFINE_TLV:
        {
            return 1;
        }
        break;
    default:
        {
            return 0;
        }
        break;
    }
    return 0;
}

void vdp_loop_timeout_handler(void);

// 获取 vdptlv type string 信息;
const char* GetVdpTlvTypeString(UINT16 type)
{
    switch(type)
    {
    case VDPTLV_TYPE_PRE_ASSOCIATE:
        {
            return "Pre_associate";
        }
        break;
    case VDPTLV_TYPE_PRE_ASSOCIATE_WITH_RR:
        {
            return "Pre_associate_with_rr";
        }
        break;
    case VDPTLV_TYPE_ASSOCIATE:
        {
            return "associate";
        }
        break;
    case VDPTLV_TYPE_DEASSOCIATE:
        {
            return "De-associate";
        }
        break;
    case VDPTLV_TYPE_VSI_MGRID:
        {
            return "VSI_manager_ID";
        }
        break;
    case VDPTLV_TYPE_ORG_DEFINE_TLV:
        {
            return "Org_define_TLV";
        }
        break;
    default:
        {
            return "TLV_Type_not_recognized";
        }
        break;
    }
}

struct ecp_module * vdp_register(void)
{
    struct ecp_module * ecp = NULL;
    
    return ecp;
}

void vdp_unregister(struct ecp_module *mod)
{

}

void vdp_ecpdu(DWORD32 ifIndex,struct ecp_ecpdu * in_ecp,struct ecp_ecpdu * out_ecpdu, DWORD32 * len)
{

}

void vdp_ifdown(DWORD32 ifIndex)
{

}
void vdp_ifup(DWORD32 ifIndex)
{

}

static const struct ecp_mod_ops vdp_ops =  {
    vdp_register,
    vdp_unregister,
    vdp_ecpdu,
    vdp_ifup,
    vdp_ifdown,
};

INT vdp_port_init_s(DWORD32 ifIndex)
{
    struct tagEvbPort* port = NULL;
    //TEvbUrpAttr* urp = NULL;

    port = GetEvbPort(ifIndex);
    if( NULL == port )
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_init_s: port(%u) is invalid.\n", ifIndex);
        return -1;
    }
    if( port->vdp_s )
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp station has existed:port(%u)\n", ifIndex);
        return -1;
    }
    port->vdp_s = (struct tagVdp_s *)GLUE_ALLOC(sizeof(struct tagVdp_s));
    if( port->vdp_s  == NULL)
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_init_s port(%u) alloc memory failed\n", ifIndex);
        return -1;
    }
    MEMSET(port->vdp_s, 0 ,sizeof(struct tagVdp_s ));
    port->vdp_s->port = port;

    // set port timeout
    port->vdp_s->respWaitDelay = vdpGetSRespWaitDelay(ifIndex);
    port->vdp_s->reinitKeepAlive = vdpGetSReinitKeepAlive(ifIndex);

    LIST_INIT(&port->vdp_s->lstVsi);

    if( GetDbgVdpPrintStatus())
    {
        PRINTF("vdp_init_s port %u success \n", ifIndex);
    }
    return 0;
}

INT vdp_port_shutdown_s(DWORD32 ifIndex)
{
    struct tagEvbPort* port = NULL;
    struct tagVsi_instance_s * vsi = NULL;

    port = GetEvbPort(ifIndex);
    if( NULL == port )
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_init_s:port(%u) is invalid.\n", ifIndex);
        return -1;
    }
    if( NULL == port->vdp_s )
    {
        return 0;
    }

    while(NULL != (vsi = LIST_FIRST(&port->vdp_s->lstVsi)))
    {
        LIST_REMOVE(vsi,lstEntry);

        // free memory
        if(vsi->operCmd)
        {
            GLUE_FREE(vsi->operCmd);
        }
        if(vsi->rxResp)
        {
            GLUE_FREE(vsi->rxResp);
        }
        if(vsi->sysCmd)
        {
            GLUE_FREE(vsi->sysCmd);
        }
        GLUE_FREE(vsi);
        vsi = NULL;
    }
    GLUE_FREE(port->vdp_s);
    port->vdp_s = NULL;

    return 0;
}

INT vdp_port_init_b(DWORD32 ifIndex)
{
    struct tagEvbPort* port = NULL;

    port = GetEvbPort(ifIndex);
    if( NULL == port)
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_init_b:port(%u) is invalid.\n", ifIndex);
        return -1;
    }
    if( port->vdp_b )
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp bridge has existed:port(%u)\n", ifIndex);
        return -1;
    }
    port->vdp_b = (struct tagVdp_b *)GLUE_ALLOC(sizeof(struct tagVdp_b));
    if( port->vdp_b  == NULL)
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_init_b port(%u) alloc memory failed\n", ifIndex);
        return -1;
    }
    MEMSET(port->vdp_b, 0 ,sizeof(struct tagVdp_b ));
    port->vdp_b->port = port;

    // set port timeout
    port->vdp_b->resouceWaitDelay = vdpGetBRourceWaitDelay(ifIndex);
    port->vdp_b->toutKeepAlive = vdpGetBtoutKeepAlive(ifIndex);
    
    LIST_INIT(&port->vdp_b->lstVsi);

    if( GetDbgVdpPrintStatus())
    {
        PRINTF("vdp_init_b port(%u) success \n", ifIndex);
    }
    return 0;
}

INT vdp_port_shutdown_b(DWORD32 ifIndex)
{
    struct tagEvbPort* port = NULL;
    struct tagVsi_instance_b * vsi = NULL;

    port = GetEvbPort(ifIndex);
    if( NULL == port )
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_init_b: port(%u) is invalid.\n", ifIndex);
        return -1;
    }
    if( NULL == port->vdp_b )
    {
        return 0;
    }

    while(NULL != (vsi = LIST_FIRST(&port->vdp_b->lstVsi)) )
    {
        LIST_REMOVE(vsi,lstEntry);

        // free memory
        if(vsi->operCmd)
        {
            GLUE_FREE(vsi->operCmd);
        }
        if(vsi->rxCmd)
        {
            GLUE_FREE(vsi->rxCmd);
        }
        if(vsi->Resp)
        {
            GLUE_FREE(vsi->Resp);
        }
        GLUE_FREE(vsi);
        vsi = NULL;
    }
    GLUE_FREE(port->vdp_b);
    port->vdp_b = NULL;
    
    return 0;
}

void start_vdp_agent(void)
{
    if (gtVDP.pfTmSet)
    {
        gtVDP.pfTmSet();
    }
}

void stop_vdp_agent(void)
{
    if (gtVDP.pfTmStop)
    {
        gtVDP.pfTmStop();
    }    
}

void vsiCmd_pool_init()
{
    LIST_INIT(&g_lstVsiCmdPool);

    if( GetDbgVdpCmdPool())
    {
        PRINTF("vsiCmd_pool_init.\n");
    }
}

void vsiCmd_pool_shutdown()
{
    struct tagVsiCmdPoolItem* pItem = NULL; 
    while(NULL != (pItem = LIST_FIRST(&g_lstVsiCmdPool)))
    {
        LIST_REMOVE(pItem,lstEntry);
        
        GLUE_FREE(pItem);
        pItem = NULL;
    }
    LIST_INIT(&g_lstVsiCmdPool);

    if( GetDbgVdpCmdPool())
    {
        PRINTF("vsiCmd_pool_shutdown.\n");
    }
}

int32 vsiCmd_pool_add(u32 port,BYTE vsiidFormat,BYTE vsiID[VDPTLV_VSIID_LEN],BYTE oper )
{
    struct tagVsiCmdPoolItem * pItem = NULL;
    struct tagVsiCmdPoolItem * pNewItem = NULL;
    u32 bFind = 0;
    int i = 0;

    if( !vsiid_format_values_valid(vsiidFormat) )
    {
        EVB_LOG(EVB_LOG_ERROR, "vsiCmd_pool_add: vsiid format(%u) is invalid.\n",vsiidFormat);
        return -1;
    }

    if(!vdp_opertype_is_valid(oper))
    {
        EVB_LOG(EVB_LOG_ERROR, "vsiCmd_pool_add:oper (%u) is invalid.\n", oper);
        return -1;
    }

    if( GetDbgVdpCmdPool())
    {
        PRINTF("vsiCmd_pool add (port:%u, vsiID:");
        for(i =0;i<VDPTLV_VSIID_LEN;i++)
        {
           PRINTF("%2.2x ",vsiID[i]);
        }
        PRINTF(", oper:%u)\n",oper);        
    }

    LIST_FOREACH(pItem,&g_lstVsiCmdPool,lstEntry)
    {
        //  
        if( pItem->port == port && \
            pItem->vsiIDFormat == vsiidFormat && \
            (0 == MEMCMP(pItem->vsiID, vsiID,VDPTLV_VSIID_LEN) ) )
        {
            bFind = 1;
            break;
        }
    }

    if(bFind)
    {
        if( oper == pItem->cmdTlvType)
        {
            return 0;
        }
        else
        {
            // 更新cmdpool; 
            pItem->cmdTlvType = oper;
            pItem->newFlag = TRUE;
            
            // CancelCmd;

            return 0;
        }
    }

    // new item
    pNewItem = (struct tagVsiCmdPoolItem *)GLUE_ALLOC(sizeof(struct tagVsiCmdPoolItem));
    if( NULL == pNewItem )
    {
        EVB_LOG(EVB_LOG_ERROR, "vsiCmd_pool_add: alloc memory failed.\n");
        return -1;
    }
    pNewItem->port = port;
    pNewItem->vsiIDFormat = vsiidFormat;
    pNewItem->cmdTlvType = oper;
    MEMCPY(pNewItem->vsiID,vsiID,VDPTLV_VSIID_LEN);

    if(NULL == pItem)
    {
        LIST_INSERT_HEAD(&g_lstVsiCmdPool, pNewItem, lstEntry);
    }
    else
    {
        LIST_INSERT_AFTER(pItem,pNewItem,lstEntry);
    }
    return 0;
}

int32 vsiCmd_pool_del(u32 port,BYTE vsiidFormat,BYTE vsiID[VDPTLV_VSIID_LEN],BYTE oper )
{
    struct tagVsiCmdPoolItem* pItem = NULL; 
    int i = 0;
    if( !vsiid_format_values_valid(vsiidFormat) )
    {
        EVB_LOG(EVB_LOG_ERROR, "vsiCmd_pool_add: vsiid format(%u) is invalid.\n",vsiidFormat);
        return -1;
    }

    if(!vdp_opertype_is_valid(oper))
    {
        EVB_LOG(EVB_LOG_ERROR, "vsiCmd_pool_add:oper (%u) is invalid.\n", oper);
        return -1;
    }

    if( GetDbgVdpCmdPool())
    {
        PRINTF("vsiCmd_pool del (port:%u, vsiID:");
        for(i =0;i<VDPTLV_VSIID_LEN;i++)
        {
           PRINTF("%2.2x ",vsiID[i]);
        }
        PRINTF(", oper:%u)\n",oper);        
    }
    
    LIST_FOREACH(pItem,&g_lstVsiCmdPool,lstEntry)
    {
        if( pItem->port == port && \
            pItem->vsiIDFormat == vsiidFormat && \
            pItem->cmdTlvType == oper && \
            (0 == MEMCMP(pItem->vsiID, vsiID,VDPTLV_VSIID_LEN) ) )
        {
            LIST_REMOVE(pItem,lstEntry);

            GLUE_FREE(pItem);

            return 0;
        }
    }

    return 0;
}

int32 vsiCmd_pool_del_notlvtype(u32 port,BYTE vsiidFormat,BYTE vsiID[VDPTLV_VSIID_LEN] )
{
    struct tagVsiCmdPoolItem* pItem = NULL; 
    int i = 0;
    if( !vsiid_format_values_valid(vsiidFormat) )
    {
        EVB_LOG(EVB_LOG_ERROR, "vsiCmd_pool_add: vsiid format(%u) is invalid.\n",vsiidFormat);
        return -1;
    }
    
    if( GetDbgVdpCmdPool())
    {
        PRINTF("vsiCmd_pool del (port:%u, vsiID:");
        for(i =0;i<VDPTLV_VSIID_LEN;i++)
        {
           PRINTF("%2.2x ",vsiID[i]);
        }
        PRINTF(")\n");        
    }
    
    LIST_FOREACH(pItem,&g_lstVsiCmdPool,lstEntry)
    {
        if( pItem->port == port && \
            pItem->vsiIDFormat == vsiidFormat && \
            (0 == MEMCMP(pItem->vsiID, vsiID,VDPTLV_VSIID_LEN) ) )
        {
            LIST_REMOVE(pItem,lstEntry);

            GLUE_FREE(pItem);

            return 0;
        }
    }

    return 0;
}

int32 vsiCmd_pool_lookup(u32 port,BYTE vsiidFormat,BYTE vsiID[VDPTLV_VSIID_LEN])
{
    struct tagVsiCmdPoolItem* pItem = NULL; 
    int i = 0;
    if( !vsiid_format_values_valid(vsiidFormat) )
    {
        EVB_LOG(EVB_LOG_ERROR, "vsiCmd_pool_lookup: vsiid format(%u) is invalid.\n",vsiidFormat);
        return -1;
    }
    
    LIST_FOREACH(pItem,&g_lstVsiCmdPool,lstEntry)
    {
        if( pItem->port == port && \
            pItem->vsiIDFormat == vsiidFormat && \
            (0 == MEMCMP(pItem->vsiID, vsiID,VDPTLV_VSIID_LEN) ) )
        {           
            return 0;
        }
    }

    if( GetDbgVdpCmdPool())
    {
        PRINTF("vsiCmd_pool lookup (port:%u, vsiID:");
        for(i =0;i<VDPTLV_VSIID_LEN;i++)
        {
           PRINTF("%2.2x ",vsiID[i]);
        }
        PRINTF(", not exist)\n");        
    }
    
    return -1;
}

T_VDP *vdp_mod_init(void (*pfTmInit)(void),
    void (*pfTmSet)(void),
    void (*pfTmStop)(void))
{
    memset(&gtVDP, 0, sizeof(T_VDP));
    gtVDP.pfTmInit   = pfTmInit;
    gtVDP.pfTmSet    = pfTmSet;
    gtVDP.pfTmStop   = pfTmStop;
    gtVDP.pfTmout    = &vdp_loop_timeout_handler;
    
    if (gtVDP.pfTmInit)
    {
        gtVDP.pfTmInit();
    }

    start_vdp_agent();

    // vsicmd_pool
    vsiCmd_pool_init();

    // vsi profile 
    vsiProfile_init();

    // vsi BOut Flt
    vsiBOutFlt_init();

    return &gtVDP;
}

void vdp_mod_shutdown()
{
    struct tagEvbPort *pPort = NULL;

    stop_vdp_agent();
    vsiProfile_shutdown();
    vsiBOutFlt_shutdown();
    vsiCmd_pool_shutdown();

    // release port info
    pPort = Get1stEvbPort();
    if(NULL == pPort)
    {
        return ;
    }

    for(; pPort; pPort = GetNextEvbPort(pPort))
    {
        vdp_port_shutdown_b(pPort->uiPortNo);
        vdp_port_shutdown_s(pPort->uiPortNo);
    }
}

INT vdp_port_init(DWORD32 ifIndex)
{
    struct tagEvbPort* port = NULL;

    // 1 check 
    port = GetEvbPort(ifIndex);
    if( NULL == port )
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_init_if:ifIndex (%u) is invalid.\n", ifIndex);
        return -1;
    }

    switch( EvbcfgGetSysType() )
    {
    case EVBSYS_TYPE_BRIDGE:
        {
            vdp_port_init_b(ifIndex);
        }
        break;
    case EVBSYS_TYPE_STATION:
        {
            vdp_port_init_s(ifIndex);
        }
        break;
    default:
        {
            EVB_LOG(EVB_LOG_ERROR, "EvbcfgGetSysType : type (%d) is invalid.\n", EvbcfgGetSysType() );
        }
        break;
    }
    return 0;
}

INT vdp_port_up(u32 ifIndex)
{

    return 0;
}

INT vdp_port_down(u32 ifIndex)
{

    return 0;
}

INT vdp_port_shutdown(DWORD32 ifIndex)
{
    struct tagEvbPort* port = NULL;

    // 1 check 
    port = GetEvbPort(ifIndex);
    if( NULL == port )
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_init_if:ifIndex (%u) is invalid.\n", ifIndex);
        return -1;
    }

    switch( EvbcfgGetSysType() )
    {
    case EVBSYS_TYPE_BRIDGE:
        {
            vdp_port_shutdown_b(ifIndex);
        }
        break;
    case EVBSYS_TYPE_STATION:
        {
            vdp_port_shutdown_s(ifIndex);
        }
        break;
    default:
        {
            EVB_LOG(EVB_LOG_ERROR, "EvbcfgGetSysType : type (%d) is invalid.\n", ifIndex, EvbcfgGetSysType() );
        }
        break;
    }
    return 0;
}

UINT16 vdpTlvHead_HTON(UINT16 type, UINT16 len )
{
    UINT16 tl = 0;
    tl = type;
    tl = tl << 9;
    tl |= len & 0x01ff;
    tl = EVB_HTONS(tl);
    return tl;
}

UINT16 vdpTlvHead_NTOH(struct tagVdpTlv_head * inout)
{
    UINT16 tl = 0;
    BYTE * pTmp = (BYTE *)inout;
    EVB_ASSERT(inout);

    MEMCPY(&tl,pTmp, sizeof(tl) );
    tl = EVB_NTOHS(tl);

    inout->tlvType = (tl >> 9) & 0x7f;
    inout->tlvLen  = (tl) & 0x1ff;

    return 0;
}

void vdp_loop_timeout_handler(void)
{
    // 1 将遍历port->vdp_b;
    // 2 将遍历port->vdp_s;    
    struct tagEvbPort *pPort = NULL;
    pPort = Get1stEvbPort();

    if(NULL == pPort)
    {
        return ;
    }

    switch(EvbcfgGetSysType())
    {
    case EVBSYS_TYPE_BRIDGE:
        {
            for(; pPort; pPort = GetNextEvbPort(pPort))
            {
                if( NULL == pPort->vdp_b)
                {
                    continue;
                }
                vdp_b_loop_timer_handler(pPort->vdp_b);
            }
        }
        break;
    case EVBSYS_TYPE_STATION:
        {
            for(; pPort; pPort = GetNextEvbPort(pPort))
            {
                if( NULL == pPort->vdp_s)
                {
                    continue;
                }
                vdp_s_loop_timer_handler(pPort->vdp_s);
            }
        }
        break;
    default:
        {
            // PRINTF("evb sysType (%d) is valid.\n ", EvbcfgGetSysType() );
        }
        break;
    }
}

void vdp_rx_ReceiveFrame(u32 local_port, u8* buf, u16 len)
{
    struct tagEvbPort* port = NULL;

    // 1 check 
    port = GetEvbPort(local_port);
    if( NULL == port )
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_rx_ReceiveFrame:ifIndex (%u) is invalid.\n", local_port);
        return;
    }

    if( GetDbgVdpPrintStatus())
    {
        PRINTF("port:%u,vdp_rx_ReceiveFrame - %p %u \n",local_port,buf, len );
    }

    if(GetDbgVdpPacket())
    {
        PRINTF("port:%u, VDP RX  ", local_port);
        PrintLocalTimer();
        DbgMemShow(buf,len);
    }

    switch(EvbcfgGetSysType())
    {
    case EVBSYS_TYPE_BRIDGE:
        {
            if(NULL == port->vdp_b )
            {
                return;
            }
            vdp_b_rx_ReceiveFrame(port->vdp_b,local_port, buf, len);
        }
        break;
    case EVBSYS_TYPE_STATION:
        {
            if(NULL == port->vdp_s )
            {
                return;
            }
            vdp_s_rx_ReceiveFrame(port->vdp_s,local_port, buf, len);
        }
        break;
    default:
        {
            if( GetDbgVdpPrintStatus())
            {
                PRINTF("evb sysType (%d) is valid.\n ", EvbcfgGetSysType() );
            }
        }
        break;
    }
}

int32 initDftVsiB(struct tagVsi_instance_b * vsi)
{
    struct tagEvbPort* sbp = NULL;
    if( NULL == vsi)
    {
        EVB_LOG(EVB_LOG_ERROR, "initDftVsiB:param is invalid.\n");
        return -1;
    }

    sbp = GetEvbPort(vsi->B_VDPPORTNUM);
    if( NULL == sbp || NULL == sbp->ptPortAttr)
    {
        EVB_LOG(EVB_LOG_ERROR, "initDftVsiB: alloc memory failed.\n");
        return -1;
    }
    
    vsi->resouceWaitDelay = vdpGetBRourceWaitDelay(vsi->B_VDPPORTNUM);
    vsi->toutKeepAlive = vdpGetBtoutKeepAlive(vsi->B_VDPPORTNUM);

    return 0;
}
int32 initDftVsiS(struct tagVsi_instance_s * vsi)
{
    struct tagEvbPort* urp = NULL;
    if( NULL == vsi)
    {
        EVB_LOG(EVB_LOG_ERROR, "initDftVsiS:param is invalid.\n");
        return -1;
    }

    urp = GetEvbPort(vsi->S_VDPPORTNUM);
    if( NULL == urp)
    {
        EVB_LOG(EVB_LOG_ERROR, "initDftVsiS: alloc memory failed.\n");
        return -1;
    }
    vsi->respWaitDelay = vdpGetSRespWaitDelay(vsi->S_VDPPORTNUM);
    vsi->reinitKeepAlive = vdpGetSReinitKeepAlive(vsi->S_VDPPORTNUM);
        
    return 0;
}

/* calc timer */
u32 evbcalc_vdpSRespWaitDelay(u32 vdpResourceWaitDly,u32 ecpMaxTries,u32 ecpAckTimerInit)
{
    u32 ret = 0;
    u32 tmp = 0;
    /* 1.5*(2^vdpResourceWaitDly + (2*ecpMaxTries +1)*2^ecpAckTimerInit */
    tmp = evbcalc_2_exponent(vdpResourceWaitDly) + ( (ecpMaxTries << 1 ) +1 ) * evbcalc_2_exponent(ecpAckTimerInit);

    ret = tmp;
    ret += (tmp >> 1);
    return ret;
}
 
u32 evbcalc_vdpSReinitKeepAlive(u32 vdpReinitKeepAlive)
{
    return evbcalc_2_exponent(vdpReinitKeepAlive);
}

u32 evbcalc_vdpBRourceWaitDelay(u32 vdpResourceWaitDly)
{
    return evbcalc_2_exponent(vdpResourceWaitDly);
}

u32 evbcalc_vdpBtoutKeepAlive(u32 vdpReinitKeepalive,u32 ecpMaxTries,u32 ecpAckTimerInit)
{
    /* 1.5*(2^vdpReinitKeepalive + (2*ecpMaxTries+1)*2^ecpAckTimerInit)*/
    u32 ret = 0;
    u32 tmp = 0;
    tmp = evbcalc_2_exponent(vdpReinitKeepalive) + ( (ecpMaxTries << 1 ) +1 ) * evbcalc_2_exponent(ecpAckTimerInit);

    ret = tmp;
    ret += (tmp >> 1);
    return ret;
}

static void * vdpGetPortAttr(u32 port,u8 * outPortType)
{
    struct tagEvbPort *pPort = NULL;

    pPort = GetEvbPort(port);

    if(NULL == pPort)
    {
        EVB_LOG(EVB_LOG_ERROR, "vdpGetPortAttr:GetEvbPort(%u) failed.\n",port);
        return NULL;
    }

    if(NULL == pPort->ptPortAttr)
    {
        EVB_ASSERT(0);
        EVB_LOG(EVB_LOG_ERROR, "vdpGetPortAttr:EvbPort(%u) no attribute data.\n",port);
        return NULL;
    }
    *outPortType = pPort->ucEvbPortType;
    return pPort->ptPortAttr;
}
u32 vdpGetSRespWaitDelay(u32 port)
{
    u8 PortType = 0;
//	struct tagEvbPort *pPort = NULL;
    void * pPortAttr = NULL;
    TEvbUrpAttr* pUrpAttr = NULL;
    TEvbSbpAttr* pSbpAttr = NULL;

    pPortAttr = vdpGetPortAttr(port, &PortType);
    if(pPortAttr)
    {
        switch(PortType)
        {
        case EM_EVB_PT_URP:
            {
                pUrpAttr = (TEvbUrpAttr* )(pPortAttr);
                if(EVB_TLV_STATUS_SUCC == pUrpAttr->tNegRes.ucStatus)
                {
                    return evbcalc_vdpSRespWaitDelay(pUrpAttr->tNegRes.ucRWD,pUrpAttr->tNegRes.ucEcpMaxRetries,pUrpAttr->tNegRes.ucRTE);
                }
            }
            break;
        case EM_EVB_PT_SBP:
            {
                pSbpAttr = (TEvbSbpAttr* )(pPortAttr);
                if(EVB_TLV_STATUS_SUCC == pSbpAttr->tNegRes.ucStatus)
                {
                    return evbcalc_vdpSRespWaitDelay(pSbpAttr->tNegRes.ucRWD,pSbpAttr->tNegRes.ucEcpMaxRetries,pSbpAttr->tNegRes.ucRTE);
                }
            }
            break;
        default:
            {				
            }
            break;
        }
    }

    EVB_LOG(EVB_LOG_ERROR, "vdpGetSRespWaitDelay:EvbPort(%u) attr(%p) type[%u] using default Value.\n",port,pPortAttr,PortType);
    // default value 
    return evbcalc_vdpSRespWaitDelay(evbcfg_getVdpRsrcWaitDelay(),evbcfg_getEcpMaxTries(),evbcfg_getEcpAckInit());
}
u32 vdpGetSReinitKeepAlive(u32 port)
{
    u8 PortType = 0;
//	struct tagEvbPort *pPort = NULL;
    void * pPortAttr = NULL;
    TEvbUrpAttr* pUrpAttr = NULL;
    TEvbSbpAttr* pSbpAttr = NULL;

    pPortAttr = vdpGetPortAttr(port, &PortType);
    if(pPortAttr)
    {
        switch(PortType)
        {
        case EM_EVB_PT_URP:
            {
                pUrpAttr = (TEvbUrpAttr* )(pPortAttr);
                if(EVB_TLV_STATUS_SUCC == pUrpAttr->tNegRes.ucStatus)
                {
                    return evbcalc_vdpSReinitKeepAlive(pUrpAttr->tNegRes.ucRKA);
                }
            }
            break;
        case EM_EVB_PT_SBP:
            {
                pSbpAttr = (TEvbSbpAttr* )(pPortAttr);
                if(EVB_TLV_STATUS_SUCC == pSbpAttr->tNegRes.ucStatus)
                {
                    return evbcalc_vdpSReinitKeepAlive(pSbpAttr->tNegRes.ucRKA);
                }
            }
            break;
        default:
            {				
            }
            break;
        }
    }

    EVB_LOG(EVB_LOG_ERROR, "vdpGetSReinitKeepAlive:EvbPort(%u) attr(%p) type[%u] using default Value.\n",port,pPortAttr,PortType);
    // default value 
    return evbcalc_vdpSReinitKeepAlive(evbcfg_getVdpReinitKeepAlive());
}
u32 vdpGetBRourceWaitDelay(u32 port)
{
    u8 PortType = 0;
//	struct tagEvbPort *pPort = NULL;
    void * pPortAttr = NULL;
    TEvbUrpAttr* pUrpAttr = NULL;
    TEvbSbpAttr* pSbpAttr = NULL;

    pPortAttr = vdpGetPortAttr(port, &PortType);
    if(pPortAttr)
    {
        switch(PortType)
        {
        case EM_EVB_PT_URP:
            {
                pUrpAttr = (TEvbUrpAttr* )(pPortAttr);
                if(EVB_TLV_STATUS_SUCC == pUrpAttr->tNegRes.ucStatus)
                {
                    return evbcalc_vdpBRourceWaitDelay(pUrpAttr->tNegRes.ucRWD);
                }
            }
            break;
        case EM_EVB_PT_SBP:
            {
                pSbpAttr = (TEvbSbpAttr* )(pPortAttr);
                if(EVB_TLV_STATUS_SUCC == pSbpAttr->tNegRes.ucStatus)
                {
                    return evbcalc_vdpBRourceWaitDelay(pSbpAttr->tNegRes.ucRWD);
                }
            }
            break;
        default:
            {				
            }
            break;
        }
    }

    EVB_LOG(EVB_LOG_ERROR, "vdpGetBRourceWaitDelay:EvbPort(%u) attr(%p) type[%u] using default Value.\n",port,pPortAttr,PortType);
    // default value 
    return evbcalc_vdpBRourceWaitDelay(evbcfg_getVdpRsrcWaitDelay());
}
u32 vdpGetBtoutKeepAlive(u32 port)
{
    u8 PortType = 0;
//	struct tagEvbPort *pPort = NULL;
    void * pPortAttr = NULL;
    TEvbUrpAttr* pUrpAttr = NULL;
    TEvbSbpAttr* pSbpAttr = NULL;

    pPortAttr = vdpGetPortAttr(port, &PortType);
    if(pPortAttr)
    {
        switch(PortType)
        {
        case EM_EVB_PT_URP:
            {
                pUrpAttr = (TEvbUrpAttr* )(pPortAttr);
                if(EVB_TLV_STATUS_SUCC == pUrpAttr->tNegRes.ucStatus)
                {
                    return evbcalc_vdpBtoutKeepAlive(pUrpAttr->tNegRes.ucRKA,pUrpAttr->tNegRes.ucEcpMaxRetries,pUrpAttr->tNegRes.ucRTE);
                }
            }
            break;
        case EM_EVB_PT_SBP:
            {
                pSbpAttr = (TEvbSbpAttr* )(pPortAttr);
                if(EVB_TLV_STATUS_SUCC == pSbpAttr->tNegRes.ucStatus)
                {
                    return evbcalc_vdpBtoutKeepAlive(pSbpAttr->tNegRes.ucRKA,pSbpAttr->tNegRes.ucEcpMaxRetries,pSbpAttr->tNegRes.ucRTE);
                }
            }
            break;
        default:
            {				
        
            }
            break;
        }
    }

    EVB_LOG(EVB_LOG_ERROR, "vdpGetBtoutKeepAlive:EvbPort(%u) attr(%p) type[%u] using default Value.\n",port,pPortAttr,PortType);
    // default value 
    return evbcalc_vdpBtoutKeepAlive(evbcfg_getVdpReinitKeepAlive(),evbcfg_getEcpMaxTries(),evbcfg_getEcpAckInit());
}

INT vdp_GetEvbTlvNegResult(u32 port)
{
    u8 PortType = 0;
    void * pPortAttr = NULL;
    TEvbUrpAttr* pUrpAttr = NULL;
    TEvbSbpAttr* pSbpAttr = NULL;

    pPortAttr = vdpGetPortAttr(port, &PortType);
    if(pPortAttr)
    {
        switch(PortType)
        {
        case EM_EVB_PT_URP:
            {
                pUrpAttr = (TEvbUrpAttr* )(pPortAttr);
                return pUrpAttr->tNegRes.ucStatus;
            }
            break;
        case EM_EVB_PT_SBP:
            {
                pSbpAttr = (TEvbSbpAttr* )(pPortAttr);
                return  pSbpAttr->tNegRes.ucStatus;
            }
            break;
        default:
            {				
                return EVB_TLV_STATUS_INIT;
            }
            break;
        }
    }
    return EVB_TLV_STATUS_INIT;
}

INT vdp_opertype_is_valid(BYTE type)
{
    switch(type)
    {
    case VDPTLV_TYPE_PRE_ASSOCIATE:
    case VDPTLV_TYPE_PRE_ASSOCIATE_WITH_RR:
    case VDPTLV_TYPE_ASSOCIATE:
    case VDPTLV_TYPE_DEASSOCIATE:
        {
            return 1;
        }
        break;
    default:
        {
            return 0;
        }
        break;
    }
    return 0;
}

static INT vsiProfile_to_mgrid(struct tagVsiProfile* vsiPro, BYTE * outTlv,DWORD32 * outLen)
{
    struct tagVdpTlv_VSIMgrID * pMgrID = NULL;
    UINT16 tlv_head = 0;

    if(NULL == vsiPro || NULL == outTlv)
    {
        EVB_LOG(EVB_LOG_ERROR,"vsiProfile_to_mgrid : param is NULL.\n" );
        return -1;
    }

    pMgrID = (struct tagVdpTlv_VSIMgrID *)(outTlv);

    MEMCPY(pMgrID->mgr_id, vsiPro->vsiMgrID, VDPTLV_VSIMGRID_LEN);
    tlv_head = vdpTlvHead_HTON(VDPTLV_TYPE_VSI_MGRID,VDPTLV_VSIMGRID_LEN);
    MEMCPY(&pMgrID->tlvHead,&tlv_head ,sizeof(tlv_head));
    *outLen = VDPTLV_VSIMGRID_LEN + VDPTLV_HEAD_LEN;

    return 0;
}
INT32 vdpfltinfo_PsPcpVid_ntohs(UINT16 ps,UINT16 pcp,UINT16 vid, UINT16 *out)
{
    UINT16 tmp = 0;

    if(NULL == out)
    {
        return -1;
    }
    tmp = vid;
    tmp |= ((pcp & 0x8 ) << 12);
    tmp |= ((ps & 0x01 ) << 15);
    tmp = EVB_NTOHS(tmp);

    MEMCPY(out, &tmp, sizeof(tmp));
    return 0;
}

INT32 vdpfltVID_ntohs(struct tagVDP_FLT_VID * pV)
{
    UINT16 tl = 0;

    if(NULL == pV)
    {
        return -1;
    }

    if( vdpfltinfo_PsPcpVid_ntohs(pV->ps,pV->pcp,pV->vid,&tl) )
    {
        return -1;
    }

    MEMCPY(pV,&tl,sizeof(tl));
    return 0;
}

INT32 vdpfltMVID_ntohs(struct tagVDP_FLT_MACVID * pV)
{
    UINT16 tl = 0;
    BYTE * pTmp= NULL;

    if(NULL == pV)
    {
        return -1;
    }

    if( vdpfltinfo_PsPcpVid_ntohs(pV->ps,pV->pcp,pV->vid,&tl) )
    {
        return -1;
    }
    pTmp = (BYTE *)pV;
    pTmp += EVB_VDP_MAC_LEN;
    MEMCPY(pTmp ,&tl,sizeof(tl));
    return 0;
}

INT32 vdpfltGROUPVID_ntohs(struct tagVDP_FLT_GROUPVID * pV)
{
    UINT16 tl = 0;
    BYTE * pTmp= NULL;

    if(NULL == pV)
    {
        return -1;
    }

    if( vdpfltinfo_PsPcpVid_ntohs(pV->ps,pV->pcp,pV->vid,&tl) )
    {
        return -1;
    }
    pTmp = (BYTE *)pV;
    pTmp += EVB_VDP_GROUPID_LEN;
    MEMCPY(pTmp ,&tl,sizeof(tl));
    return 0;
}

INT32 vdpfltGROUPMACVID_ntohs(struct tagVDP_FLT_GROUPMACVID * pV)
{
    UINT16 tl = 0;
    BYTE * pTmp= NULL;

    if(NULL == pV)
    {
        return -1;
    }

    if( vdpfltinfo_PsPcpVid_ntohs(pV->ps,pV->pcp,pV->vid,&tl) )
    {
        return -1;
    }
    pTmp = (BYTE *)pV;
    pTmp += EVB_VDP_GROUPID_LEN;
    pTmp += EVB_VDP_MAC_LEN;
    MEMCPY(pTmp ,&tl,sizeof(tl));
    return 0;
}

INT32 vdpfltinfo_PsPcpVid_htons(UINT16 ps,UINT16 pcp,UINT16 vid, UINT16 *out)
{
    UINT16 tmp = 0;

    if(NULL == out)
    {
        return -1;
    }
    tmp = vid;
    tmp |= ((pcp & 0x8 ) << 12);
    tmp |= ((ps & 0x01 ) << 15);
    tmp = EVB_HTONS(tmp);

    MEMCPY(out, &tmp, sizeof(tmp));
    return 0;
}

INT32 vdpfltVID_htons(struct tagVDP_FLT_VID * pV)
{
    UINT16 tl = 0;

    if(NULL == pV)
    {
        return -1;
    }

    if( vdpfltinfo_PsPcpVid_htons(pV->ps,pV->pcp,pV->vid,&tl) )
    {
        return -1;
    }

    MEMCPY(pV,&tl,sizeof(tl));
    return 0;
}

INT32 vdpfltMVID_htons(struct tagVDP_FLT_MACVID * pV)
{
    UINT16 tl = 0;
    BYTE * pTmp= NULL;

    if(NULL == pV)
    {
        return -1;
    }

    if( vdpfltinfo_PsPcpVid_htons(pV->ps,pV->pcp,pV->vid,&tl) )
    {
        return -1;
    }
    pTmp = (BYTE *)pV;
    pTmp += EVB_VDP_MAC_LEN;
    MEMCPY(pTmp ,&tl,sizeof(tl));
    return 0;
}

INT32 vdpfltGROUPVID_htons(struct tagVDP_FLT_GROUPVID * pV)
{
    UINT16 tl = 0;
    BYTE * pTmp= NULL;

    if(NULL == pV)
    {
        return -1;
    }

    if( vdpfltinfo_PsPcpVid_htons(pV->ps,pV->pcp,pV->vid,&tl) )
    {
        return -1;
    }
    pTmp = (BYTE *)pV;
    pTmp += EVB_VDP_GROUPID_LEN;
    MEMCPY(pTmp ,&tl,sizeof(tl));
    return 0;
}

INT32 vdpfltGROUPMACVID_htons(struct tagVDP_FLT_GROUPMACVID * pV)
{
    UINT16 tl = 0;
    BYTE * pTmp= NULL;

    if(NULL == pV)
    {
        return -1;
    }

    if( vdpfltinfo_PsPcpVid_htons(pV->ps,pV->pcp,pV->vid,&tl) )
    {
        return -1;
    }
    pTmp = (BYTE *)pV;
    pTmp += EVB_VDP_GROUPID_LEN;
    pTmp += EVB_VDP_MAC_LEN;
    MEMCPY(pTmp ,&tl,sizeof(tl));
    return 0;
}

static INT vsiProfile_to_vdpasstlv(struct tagVsiProfile* vsiPro,BYTE vdptlvType, BYTE * outTlv,DWORD32 * outLen)
{
    struct tagVdpTlv_VDPAss  *pVdpAss = NULL;
    struct tagVsiGMVInfo* pGMV = NULL;
    BYTE * pFltInfo = NULL;
    BYTE * pFltNum = NULL;
    UINT16 nFltNum = 0;
    UINT16 tlv_head = 0;
    UINT16 tlv_len = 0;

    struct tagVDP_FLT_GROUPMACVID * pTmpGMV = NULL;
    struct tagVDP_FLT_MACVID * pTmpMV = NULL;
    struct tagVDP_FLT_GROUPVID * pTmpGV = NULL;
    struct tagVDP_FLT_VID * pTmpV = NULL;

    if( NULL == vsiPro || NULL == outTlv)
    {
        EVB_LOG(EVB_LOG_ERROR,"vsiProfile_to_vdpasstlv : param is NULL.\n" );
        return -1;
    }

    pVdpAss = (struct tagVdpTlv_VDPAss *)(outTlv);
    MEMCPY(pVdpAss->vsiTypeID,vsiPro->vsitypeID, sizeof(vsiPro->vsitypeID));
    pVdpAss->vsiTypeVer= vsiPro->vsitypeVer;
    pVdpAss->vsiIDFormat = vsiPro->vsiIDFormat;
    MEMCPY(pVdpAss->vsiID,vsiPro->vsiID, sizeof(vsiPro->vsiID));
    pVdpAss->fltFormat =  vsiPro->fltFormat;
    
    // flt info
    pFltNum = pVdpAss->fltInfo;
    //MEMCPY(pFltNum, EVB_HTONS(vsiPro->fltNumber), sizeof(vsiPro->fltNumber));
    pFltInfo = pFltNum + sizeof(vsiPro->fltNumber);
    LIST_FOREACH(pGMV,&vsiPro->lstFlt,lstEntry)
    {
        switch(vsiPro->fltFormat)
        {
        case VDP_FLTINFO_FORMAT_VID:
            {
                pTmpV = (struct tagVDP_FLT_VID *)pFltInfo;
                pTmpV->vid = pGMV->gmv.vid;
                pTmpV->pcp = pGMV->gmv.pcp;
                pTmpV->ps = pGMV->gmv.ps;

                vdpfltVID_htons(pTmpV);

                nFltNum++;
                pFltInfo += sizeof(*pTmpV);
            }
            break;
        case VDP_FLTINFO_FORMAT_MACVID:
            {
                pTmpMV = (struct tagVDP_FLT_MACVID *)pFltInfo;
                pTmpMV->vid = pGMV->gmv.vid;
                pTmpMV->pcp = pGMV->gmv.pcp;
                pTmpMV->ps = pGMV->gmv.ps;
                MEMCPY(pTmpMV->mac,pGMV->gmv.mac,sizeof(pTmpMV->mac));

                vdpfltMVID_htons(pTmpMV);
                nFltNum++;
                pFltInfo += sizeof(*pTmpMV);
            }
            break;
        case VDP_FLTINFO_FORMAT_GROUPVID:
            {
                pTmpGV = (struct tagVDP_FLT_GROUPVID *)pFltInfo;
                pTmpGV->vid = pGMV->gmv.vid;
                pTmpGV->pcp = pGMV->gmv.pcp;
                pTmpGV->ps = pGMV->gmv.ps;
                MEMCPY(pTmpGV->groupID,pGMV->gmv.groupID,sizeof(pTmpGV->groupID));

                vdpfltGROUPVID_htons(pTmpGV);
                nFltNum++;
                pFltInfo += sizeof(*pTmpGV);
            }
            break;
        case VDP_FLTINFO_FORMAT_MACGROUPVID:
            {
                pTmpGMV = (struct tagVDP_FLT_GROUPMACVID *)pFltInfo;
                pTmpGMV->vid = pGMV->gmv.vid;
                pTmpGMV->pcp = pGMV->gmv.pcp;
                pTmpGMV->ps = pGMV->gmv.ps;
                MEMCPY(pTmpGMV->groupID,pGMV->gmv.groupID,sizeof(pTmpGMV->groupID));
                MEMCPY(pTmpGMV->mac,pGMV->gmv.mac,sizeof(pTmpGMV->mac));

                vdpfltGROUPMACVID_htons(pTmpGMV);
                nFltNum++;
                pFltInfo += sizeof(*pTmpGMV);
            }
            break;
        default: 
            {
                EVB_LOG(EVB_LOG_ERROR,"vsiProfile_to_vdpasstlv : fltFormat(%d) is invalid.",vsiPro->fltFormat );
                return -1;
            }
            break;
        }
    }
    nFltNum = EVB_HTONS(nFltNum);
    MEMCPY(pFltNum,&nFltNum , sizeof(vsiPro->fltNumber));

    tlv_len = VDPTLV_VDPTLV_NOFLT_LEN + (pFltInfo - pFltNum);
    tlv_head = vdpTlvHead_HTON(vdptlvType,tlv_len);

    MEMCPY(&pVdpAss->tlvHead,&tlv_head ,sizeof(tlv_head));
    *outLen = VDPTLV_HEAD_LEN + tlv_len;
      
    return 0;
}

INT vsiProfile_to_vsitlv(struct tagVsiProfile* vsiPro, BYTE vdptlvType,BYTE * outTlv,DWORD32 * outLen)
{
    BYTE * pTmp = outTlv;
    DWORD32 len = 0;
    DWORD32 len_total = 0;
    if(NULL == vsiPro || NULL == outTlv)
    {
        EVB_LOG(EVB_LOG_ERROR,"vsiProfile_to_vsitlv : param is NULL.\n" );
        return -1;
    }

    /* mgr id tlv */
    if( 0 != vsiProfile_to_mgrid(vsiPro, pTmp,&len) )
    {
        EVB_LOG(EVB_LOG_ERROR, "vsiProfile_to_vsitlv: vsiProfile_to_mgrid failed.\n");
        return -1;
    }
    //PRINTF("vsiProfile_to_mgrid DbgMemShow(%p %u\n)",pTmp, len);
    //DbgMemShow(pTmp,len);

    pTmp += len;
    len_total += len;

    /* vdp ass tlv */
    if( 0 != vsiProfile_to_vdpasstlv(vsiPro,vdptlvType,pTmp,&len))
    {
        EVB_LOG(EVB_LOG_ERROR, "vsiProfile_to_vsitlv: vsiProfile_to_vdpasstlv failed.\n");
        return -1;
    }

    
    //PRINTF("vsiProfile_to_vdpasstlv DbgMemShow(%p %u\n)",pTmp, len);
    //DbgMemShow(pTmp,len);

    pTmp += len;
    len_total += len;
    
    /* org tlv */
    
    *outLen = len_total;
    return 0;
}

DWORD32 GetVdpMgrIDTlvLen()
{
    return VDPTLV_HEAD_LEN + VDPTLV_VSIMGRID_LEN;
}

DWORD32 GetVdpOrgDfnTlvLen()
{
    return (sizeof(struct tagVdpTlv_OrgDfn));  /* 需要调整 暂时这样处理; */
}

INT buildMgrIDTlv(BYTE mgrID[VDPTLV_VSIMGRID_LEN],BYTE * outTlv, DWORD32 * outLen)
{
    struct tagVdpTlv_VSIMgrID * pMgrID = NULL;
    UINT16 tlv_head = 0;

    EVB_ASSERT(outTlv);
    pMgrID = (struct tagVdpTlv_VSIMgrID *)outTlv;

    MEMCPY(pMgrID->mgr_id, mgrID, VDPTLV_VSIMGRID_LEN);
    tlv_head = vdpTlvHead_HTON(VDPTLV_TYPE_VSI_MGRID,VDPTLV_VSIMGRID_LEN);
    MEMCPY(&pMgrID->tlvHead,&tlv_head ,sizeof(tlv_head));
    *outLen = VDPTLV_VSIMGRID_LEN + VDPTLV_HEAD_LEN;

    return 0;
}

INT buildOrgDfntlv(BYTE oui[VDP_TLV_ORG_OUI_LEN],BYTE * outTlv, DWORD32 * outLen)
{
    struct tagVdpTlv_OrgDfn * pOrgDfn = NULL;
    UINT16 tlv_head = 0;

    EVB_ASSERT(outTlv);
    pOrgDfn = (struct tagVdpTlv_OrgDfn *)outTlv;

    MEMCPY(pOrgDfn->oui, oui, VDP_TLV_ORG_OUI_LEN);
    tlv_head = vdpTlvHead_HTON(VDPTLV_TYPE_ORG_DEFINE_TLV,VDP_TLV_ORG_OUI_LEN);
    MEMCPY(&pOrgDfn->tlvHead,&tlv_head ,sizeof(tlv_head));
    *outLen = sizeof(struct tagVdpTlv_OrgDfn);//VDPTLV_VSIMGRID_LEN + VDPTLV_HEAD_LEN;

    return 0;
}

/* vdp evb tlv interface  */
int32 evbtlv_chg_set_vdp(u32 ifIndex, u32 flag)
{
    struct tagEvbPort* port = NULL;
    struct tagVsi_instance_b * pVsiB = NULL;
    struct tagVsi_instance_s * pVsiS = NULL;
    //u32 newWaitWhile = 0;
    u32 oldWaitWhile = 0;

    port = GetEvbPort(ifIndex);
    if( NULL == port)
    {
        return 0;
    }

    // evb no support
    if(!EVB_BIT_TEST(flag,EVBTLV_ISSUPPORT))
    {
        // 是否需要做哪些事...;
        // 在vdp timer 中会获取evbtlv neg result,进行处理;
    }

    /* flag */
    if( !EVB_BIT_TEST(flag,EVBTLV_CHG_R) && \
        !EVB_BIT_TEST(flag,EVBTLV_CHG_RTE) && \
        !EVB_BIT_TEST(flag,EVBTLV_CHG_RWD) && \
        !EVB_BIT_TEST(flag,EVBTLV_CHG_RKA) )
    {
        return 0;
    }
    
    switch(port->ucEvbPortType)
    {
    case EM_EVB_PT_SBP:
        {
            if(NULL == port->vdp_b)
            {
                return 0;
            }

            if( NULL == port->ptPortAttr)
            {
                EVB_LOG(EVB_LOG_ERROR, "evbtlv_chg_set_vdp: evbcfg_getSbp(port:%u) failed.\n",ifIndex);
                return -1;
            }
            port->vdp_b->resouceWaitDelay = vdpGetBRourceWaitDelay(ifIndex);
            port->vdp_b->toutKeepAlive = vdpGetBtoutKeepAlive(ifIndex);

            LIST_FOREACH(pVsiB,&port->vdp_b->lstVsi,lstEntry )
            {
                // 待coding;
                switch(pVsiB->state)
                {
                case VDP_B_SM_WAIT_STATION_CMD:
                    {
                        if((pVsiB->waitWhile + port->vdp_b->toutKeepAlive) >= pVsiB->toutKeepAlive )
                        {
                            pVsiB->waitWhile = (pVsiB->waitWhile + port->vdp_b->toutKeepAlive - pVsiB->toutKeepAlive);
                        }
                        else
                        {
                            pVsiB->waitWhile = 0;
                        }
                    }
                    break;
                default:break;
                }
                pVsiB->resouceWaitDelay = port->vdp_b->resouceWaitDelay;
                pVsiB->toutKeepAlive = port->vdp_b->toutKeepAlive;
            }
        }
        break;
    case EM_EVB_PT_URP:
        {
            if(NULL == port->vdp_s)
            {
                return 0;
            }

            if( NULL == port->ptPortAttr)
            {
                EVB_LOG(EVB_LOG_ERROR, "evbtlv_chg_set_vdp: alloc memory failed(port:%u).\n",ifIndex);
                return -1;
            }

            port->vdp_s->respWaitDelay = vdpGetSRespWaitDelay(ifIndex);
            port->vdp_s->reinitKeepAlive = vdpGetSReinitKeepAlive(ifIndex);

            LIST_FOREACH(pVsiS,&port->vdp_s->lstVsi,lstEntry )
            {
                
                oldWaitWhile = pVsiS->waitWhile;
                switch(pVsiS->state)
                {
                case VDP_S_SM_STATION_PROC:
                case VDP_S_SM_TRANSMIT_KEEPALIVE:
                    {
                        if((pVsiS->waitWhile +  port->vdp_s->respWaitDelay) >= pVsiS->respWaitDelay)
                        {
                            pVsiS->waitWhile = (pVsiS->waitWhile +port->vdp_s->respWaitDelay- pVsiS->respWaitDelay );
                        }
                        else
                        {
                            pVsiS->waitWhile = 0;
                        }
                    }
                    break;
                case VDP_S_SM_WAIT_SYS_CMD:
                    {
                        if(pVsiS->waitWhile + port->vdp_s->reinitKeepAlive >=  pVsiS->reinitKeepAlive)
                        {
                            pVsiS->waitWhile = (pVsiS->waitWhile + port->vdp_s->reinitKeepAlive - pVsiS->reinitKeepAlive  );
                        }
                        else
                        {
                            pVsiS->waitWhile = 0;
                        }
                    }
                    break;
                default:break;
                }
                pVsiS->respWaitDelay = port->vdp_s->respWaitDelay;
                pVsiS->reinitKeepAlive = port->vdp_s->reinitKeepAlive;
            }			
        }
        break;
    default:break;
    }	

    EVB_LOG(EVB_LOG_INFO, "evbtlv_chg_notify_vdp(port:%u, evbtlv support:%u,R:%u,RTE:%u,RWD:%u,RKA:%u): success.\n",
    ifIndex,EVB_BIT_TEST(flag,EVBTLV_ISSUPPORT),EVB_BIT_TEST(flag,EVBTLV_CHG_R),EVB_BIT_TEST(flag,EVBTLV_CHG_RTE),EVB_BIT_TEST(flag,EVBTLV_CHG_RWD),EVB_BIT_TEST(flag,EVBTLV_CHG_RKA));

    return 0;
}

char * GetVsiidstr(BYTE v[VDPTLV_VSIID_LEN])
{
    static char gs_vsiid[MAX_VSIID_STR_LEN]={0};

    MEMSET(gs_vsiid, 0, MAX_VSIID_STR_LEN);
#if 0 
     sprintf(gs_vsiid,"%2.2x %2.2x %2.2x %2.2x %2.2x %2.2x %2.2x %2.2x %2.2x %2.2x %2.2x %2.2x %2.2x %2.2x %2.2x %2.2x",\
     v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7],v[8],v[9],v[10],v[11],v[12],v[13],v[14],v[15]);
#endif 
    sprintf(gs_vsiid,VSIIDSPRINTF,\
      v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7],v[8],v[9],v[10],v[11],v[12],v[13],v[14],v[15]);

    return gs_vsiid;
}

#ifdef __cplusplus
}
#endif 


