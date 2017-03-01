
#include "evb_comm.h"
#include "ecp.h"
#include "ecp_in.h"
#include "vdp.h"
#include "vsi_profile.h"
#include "evb_port_mgr.h"

#ifdef __cplusplus
extern "C"{
#endif 

int32 vsiProfile_to_vsi( BYTE vsiIDFormat, BYTE vsiID[VDPTLV_VSIID_LEN],struct tagVsi_instance_s * vsi)
{
    struct tagVsiProfile* vsiPro = NULL;
    if(NULL == vsi)
    {
        EVB_LOG(EVB_LOG_ERROR, "vsiProfile_to_vsi:param is NULL.\n");
        return -1;
    }

    vsiPro = vsiProfile_getVsi(vsiIDFormat,vsiID);
    if(NULL == vsiPro)
    {
        EVB_LOG(EVB_LOG_ERROR, "vsiProfile_to_vsi: get vsi profile (port:%u,vsiid:%s) failed.\n",vsiIDFormat,GetVsiidstr(vsiID));
        return -1;
    }

    MEMCPY(vsi->vsiID, vsiID, sizeof(vsi->vsiID));
    vsi->vsiIDFormat = vsiIDFormat;

    return 0;
}

// vdp station oper interface 
static INT vdp_s_oper( UINT32 ifIndex,BYTE vsiIDFormat, BYTE vsiID[VDPTLV_VSIID_LEN], BYTE oper )
{
    struct tagVsi_instance_s * vsiNew = NULL;
    struct tagVsi_instance_s * vsi = NULL;
    BYTE bExist = FALSE;
    struct tagEvbPort* port = NULL;

    // 1 check 
    port = GetEvbPort(ifIndex);
    if( NULL == port )
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_s_oper:(port:%u,vsiid:%s) is invalid.\n",ifIndex,GetVsiidstr(vsiID));
        return -1;
    }

    if( NULL == port->vdp_s )
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_s_oper:(port:%u,vsiid:%s) vdp_s is NULL.\n",ifIndex,GetVsiidstr(vsiID));
        return -1;
    }
    
    LIST_FOREACH(vsi, &port->vdp_s->lstVsi,lstEntry)
    {
        /* 后续考虑是否需要添加; vsi_type id, mgr id*/
        if(0 == MEMCMP(vsiID,vsi->vsiID, VDPTLV_VSIID_LEN) && \
            vsi->vsiIDFormat == vsiIDFormat)
        {
            bExist = TRUE;
            break;
        }
    }

    if(!bExist)
    {
        vsiNew = (struct tagVsi_instance_s *)GLUE_ALLOC(sizeof(struct tagVsi_instance_s));
        if( vsiNew == NULL)
        {
            EVB_LOG(EVB_LOG_ERROR, "vdp_s_oper:alloc memory failed(port:%u,vsiid:%s).\n",ifIndex,GetVsiidstr(vsiID));
            return -1;
        }
        MEMSET(vsiNew, 0, sizeof(struct tagVsi_instance_s ));
        MEMCPY(vsiNew->vsiID, vsiID, sizeof(vsiNew->vsiID));
        vsiNew->vsiIDFormat = vsiIDFormat;

        vsiNew->vdp = port->vdp_s;
        vsiNew->cfg_state = oper;
        vsiNew->sysCmd_TlvType = oper;
        vsiNew->newCmd = TRUE;

        initDftVsiS(vsiNew);

        list_vdp_s_insert(ifIndex,vsiNew);
    }
    else
    {
        // 后面考虑其他东东变化 将如何操作;
        vsi->cfg_state = oper; // 以后考虑 判断是否有效;
        vsi->sysCmd_TlvType = oper;
        vsi->newCmd = TRUE;
    }
    // g_port.vdp_s->newCmd = TRUE;
    return 0;
}


/***********************************************************************
 vdp 提供外部模块（VM）接口;
 1 通过vsiID oper 
 ***********************************************************************/
void vdp_vm_api(UINT32 ifIndex,BYTE vsiIDFormat, BYTE vsiID[VDPTLV_VSIID_LEN], BYTE oper)
{
    // 1 check 
    if( !vdpecp_ifIndex_is_valid(ifIndex) )
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_vm_api:(port:%u,vsiid:%s) is invalid.\n", ifIndex,GetVsiidstr(vsiID));
        return;
    }
    if(!vdp_opertype_is_valid(oper))
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_vm_api:(port:%u,vsiid:%s) oper (%u) is invalid.\n",ifIndex,GetVsiidstr(vsiID), oper);
        return;
    }

    switch(EvbcfgGetSysType())
    {
    case EVBSYS_TYPE_BRIDGE:
        {
            EVB_LOG(EVB_LOG_ERROR,"vdp_vm_api: (port:%u,vsiid:%s) oper failed (local is BRIDGE).\n ",ifIndex,GetVsiidstr(vsiID));
        }
        break;
    case EVBSYS_TYPE_STATION:
        {
            vdp_s_oper(ifIndex,vsiIDFormat,vsiID,oper);
        }
        break;
    default:
        {
            EVB_LOG(EVB_LOG_ERROR,"EvbcfgGetSysType:(port:%u,vsiid:%s) type(%d) is invalid.\n ",ifIndex,GetVsiidstr(vsiID), EvbcfgGetSysType());
        }
        break;
    }
}

int32 vdp_lookup_vsiState(u32 ifIndex,BYTE vsiIDFormat, BYTE vsiID[VDPTLV_VSIID_LEN])
{ 
    struct tagEvbPort* port = NULL;
    struct tagVsi_instance_s * vsi = NULL;

    // 1 check 
    port = GetEvbPort(ifIndex);
    if( NULL == port )
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_lookup_vsiState:(port:%u, vsiid:%s)port is invalid.\n",ifIndex,GetVsiidstr(vsiID));
        return VSISTATE_FAILED;
    }

    if(EVBSYS_TYPE_STATION != EvbcfgGetSysType() )
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_lookup_vsiState:(port:%u, vsiid:%s) local(%u) is not station.\n",ifIndex,GetVsiidstr(vsiID),EvbcfgGetSysType() );
        return VSISTATE_FAILED;
    }

    if( NULL == port->vdp_s )
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_lookup_vsiState:(port:%u, vsiid:%s) port->vdp_s is NULL.\n",ifIndex,GetVsiidstr(vsiID));
        return VSISTATE_FAILED;
    }

    /* 如果evbtlv 一直处于no support状态; */
    if( EVB_TLV_STATUS_SUCC != GetEcpVdp_evbtlvStatus(ifIndex))
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_lookup_vsiState: (port:%u, vsiid:%s)  GetEcpVdp_evbtlvStatus() is not SUCC.\n",ifIndex,GetVsiidstr(vsiID));
        return VSISTATE_FAILED;
    }

    /* cmdpool 中存在，但list中没有; 调用者已经延时1秒; 
       cmdpool 中也不存在，才认为是不存在
    */
    

    LIST_FOREACH(vsi, &port->vdp_s->lstVsi,lstEntry)
    {
        /* 后续考虑是否需要添加; vsi_type id, mgr id*/
        if(0 == MEMCMP(vsiID,vsi->vsiID, VDPTLV_VSIID_LEN) && \
            vsi->vsiIDFormat == vsiIDFormat)
        {
            switch(vsi->state)
            {
            case VDP_S_SM_INIT:
                {
                    /* 出现异常回到init; */
#if 1                    
                    if( FALSE == vsi->newCmd )
                    {
                        EVB_LOG(EVB_LOG_ERROR, "vdp_lookup_vsiState:(port:%u, vsiid:%s) state(INIT),newCmd(FALSE) .\n",ifIndex,GetVsiidstr(vsiID));
                        return VSISTATE_FAILED;
                    }
                    else
                    {
                        return VSISTATE_RUNNING;
                    }
#endif 
                    return VSISTATE_RUNNING;
                }
                break;
            case VDP_S_SM_WAIT_SYS_CMD:
            case VDP_S_SM_TRANSMIT_KEEPALIVE:
                {
                    return VSISTATE_SUCCESS;    
                }
                break;
            default:
                {
                    return VSISTATE_RUNNING;
                }
                break;        
            }
        }
    }

    if(0 == vsiCmd_pool_lookup(ifIndex,vsiIDFormat,vsiID) )
    {
        return VSISTATE_RUNNING;
    }

    return VSISTATE_FAILED;
}


int32 static vdp_delete_vsi(u32 uiURPNo,BYTE vsiIDFormat, BYTE vsiID[VDPTLV_VSIID_LEN])
{    
    struct tagEvbPort* port = NULL;
    struct tagVsi_instance_s * vsi = NULL;

    // 1 check 
    port = GetEvbPort(uiURPNo);
    if( NULL == port )
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_lookup_vsiState:(port:%u, vsiid:%s)port is invalid.\n",uiURPNo,GetVsiidstr(vsiID));
        return VSISTATE_FAILED;
    }

    if(EVBSYS_TYPE_STATION != EvbcfgGetSysType() )
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_lookup_vsiState:(port:%u, vsiid:%s) local(%u) is not station.\n",uiURPNo,GetVsiidstr(vsiID),EvbcfgGetSysType() );
        return VSISTATE_FAILED;
    }

    if( NULL == port->vdp_s )
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_lookup_vsiState:(port:%u, vsiid:%s) port->vdp_s is NULL.\n",uiURPNo,GetVsiidstr(vsiID));
        return VSISTATE_FAILED;
    }

    if( vsiID == NULL)
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_lookup_vsiState: vsiID is NULL.\n");
        return 0;
    }

    LIST_FOREACH(vsi, &port->vdp_s->lstVsi,lstEntry)
    {
        if( vsi == NULL)
        {
            EVB_LOG(EVB_LOG_ERROR, "vdp_lookup_vsiState: vsi is NULL.\n");
            return 0;
        }
        
        /* 后续考虑是否需要添加; vsi_type id, mgr id*/
        if(0 == MEMCMP(vsiID,vsi->vsiID, VDPTLV_VSIID_LEN) && \
            vsi->vsiIDFormat == vsiIDFormat)
        {            
            LIST_REMOVE(vsi,lstEntry);

            GLUE_FREE(vsi);
            return 0;
        }
    }
    
    return 0;
}

// 测试用
int32 test_del_vsi(u32 uiURPNo)
{    
    struct tagEvbPort* port = NULL;
    struct tagVsi_instance_s * vsi = NULL;
    BYTE vsiID[VDPTLV_VSIID_LEN] = {0};
    int32 i = 0;

    // 1 check 
    port = GetEvbPort(uiURPNo);
    if( NULL == port )
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_lookup_vsiState:(port:%u)port is invalid.\n",uiURPNo);
        return VSISTATE_FAILED;
        }

    if(EVBSYS_TYPE_STATION != EvbcfgGetSysType() )
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_lookup_vsiState:(port:%u) local(%u) is not station.\n",uiURPNo,EvbcfgGetSysType() );
        return VSISTATE_FAILED;
    }

    if( NULL == port->vdp_s )
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_lookup_vsiState:(port:%u) port->vdp_s is NULL.\n",uiURPNo);
        return VSISTATE_FAILED;
    }

    EVB_LOG(EVB_LOG_ERROR, "LIST_FOREACH begin.\n");
    
    LIST_FOREACH(vsi, &port->vdp_s->lstVsi,lstEntry)
    {
        /* 后续考虑是否需要添加; vsi_type id, mgr id
        if(0 == MEMCMP(vsiID,vsi->vsiID, VDPTLV_VSIID_LEN) && \
            vsi->vsiIDFormat == vsiIDFormat) */
        EVB_LOG(EVB_LOG_ERROR, "LIST_FOREACH ++ %d.\n",i);
        
        if( vsi == NULL )
        {
            EVB_LOG(EVB_LOG_ERROR, "vsi == NULL.\n");
        }
        
        MEMCMP(vsiID,vsi->vsiID, VDPTLV_VSIID_LEN);
        
        {            
            LIST_REMOVE(vsi,lstEntry);

            GLUE_FREE(vsi);
        }

        i++;
    }
    
    return 0;
}


/*
    删除vsi profile接口，内部暴露给evb mgr使用 
    内部实现, 1 删除port->vdp_s->lstVsi 2 删除cmd_pool
*/
int32 vdp_delete_vsi_and_cmdPool(u32 uiURPNo,BYTE vsiIDFormat, BYTE vsiID[VDPTLV_VSIID_LEN])
{
    /*cmdpool*/
    if ( 0 != vsiCmd_pool_del_notlvtype(uiURPNo,vsiIDFormat,vsiID) )
    {
        EVB_LOG(EVB_LOG_ERROR,"vdp_delete_vsi_and_cmdPool vsiCmd_pool_del_notlvtype failed.\n");
        return -1;
    }

    /* vdp_s vsi */
    if( 0 != vdp_delete_vsi(uiURPNo, vsiIDFormat, vsiID) )
    {
        EVB_LOG(EVB_LOG_ERROR,"vdp_delete_vsi_and_cmdPool vdp_delete_vsi failed.\n");
        return -1;
    }

    return 0;
}



#ifdef __cplusplus
}
#endif 
