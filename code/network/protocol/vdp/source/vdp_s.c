
#ifdef __cplusplus
extern "C"{
#endif 

#include "evb_comm.h"
#include "ecp_in.h"
#include "vdp.h"
#include "vsi_profile.h"
#include "evb_port_mgr.h"

extern const char* vdp_s_sm_state[];
extern const char* vdp_s_sm_event[];

void vdp_s_sm(struct tagVsi_instance_s * vsi, VDP_S_SM_EVENT event);
#if 0
static DWORD32 GetVdpTlvLen_ORGDFN(struct tagVsi_instance_s * vsi)
{
    return (sizeof(struct tagVdpTlv_OrgDfn));  /* 需要调整 暂时这样处理; */
}
#endif 
INT vdp_s_build_Tlv(struct tagVsi_instance_s *vsi,BYTE vdptlvType, BYTE * * outTlv,DWORD32* outLen)
{
    BYTE *pSys = NULL;
    BYTE *pSysTemp = NULL;
    DWORD32 lenTotal = 0;
    DWORD32 len = 0;
    struct tagVsiProfile* vsiPro = NULL;

    EVB_ASSERT(vsi);
    
    /* alloc memory */
    pSys = (BYTE *)GLUE_ALLOC(VDP_PACKET_LEN_MAX);
    if( NULL == pSys)
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_s_build_Tlv: tlv (%u, %s) allow memory failed.\n",lenTotal,GetVsiidstr(vsi->vsiID));
        return -1;
    }
    MEMSET(pSys, 0, VDP_PACKET_LEN_MAX);

    /* vsi profile */
    vsiPro = vsiProfile_getVsi(vsi->vsiIDFormat,vsi->vsiID);
    if(NULL == vsiPro)
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_s_build_Tlv: get vsi profile(%u, %s) failed.\n",vsi->vsiIDFormat,GetVsiidstr(vsi->vsiID));
        return -1;
    }

    /* mgr id tlv */
    /* vdp ass tlv */
    if( 0 != vsiProfile_to_vsitlv(vsiPro,vdptlvType,pSys,&len))
    //if( 0 != vdp_tlv_buildMgrID(vsiPro->vsiMgrID, pSys,&len) )
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_s_build_Tlv: vsiProfile_to_vsitlv(%s) failed.\n",GetVsiidstr(vsiPro->vsiID));
        GLUE_FREE(pSys);
        return -1;
    }
    pSysTemp = pSys + len;
    lenTotal += len;
    /* org tlv */

    *outTlv = pSys;
    *outLen = lenTotal;

    // profile 
    vsi->profile_chg = FALSE;

    return 0;
}

INT vdp_s_tlv_insert_ecplst(DWORD32 ifIndex,BYTE * pTlv, DWORD32 tlvLen)
{
    struct ecp_ecpdu * pEcpdu = NULL;
    UINT16 seq = 0;
    BYTE * pULP = NULL;

    struct tagEvbPort* port = NULL;

    port = GetEvbPort(ifIndex);
    if( NULL == port )
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_s_tlv_insert_ecplst : ifIndex(%u) param invalid.\n",ifIndex);
        return -1;
    }

    /* 放入ecp缓存; */
    pEcpdu = (struct  ecp_ecpdu *)GLUE_ALLOC(sizeof(struct  ecp_ecpdu)); 
    if(NULL == pEcpdu)
    {
        EVB_LOG(EVB_LOG_ERROR,"vdp s TxTLV alloc memory failed.");
        return -1;
    }   
    MEMSET(pEcpdu, 0x0, sizeof(struct ecp_ecpdu));

    pULP = (BYTE *)GLUE_ALLOC(tlvLen); 
    if(NULL == pULP)
    {
        EVB_LOG(EVB_LOG_ERROR,"vdp_s_tlv_insert_ecplst TxTLV alloc memory (%u) failed.\n", tlvLen);
        GLUE_FREE(pEcpdu);
        return -1;
    }   
    MEMCPY(pULP, pTlv, tlvLen);

    GetECPSequenceNum(port->uiPortNo,&seq);
    ecpduHead_HTON(ECP_VERSION,ECP_REQUEST,ECP_SUBTYPE_VDP,seq,(BYTE*)pEcpdu);
    
    pEcpdu->ulpdu = pULP;

    list_EcpULPDUtx_insert(ifIndex,pEcpdu,tlvLen + EVB_ECP_ECPDU_HEAD_LEN);
    
    return 0;
}

static void s_err(struct tagVsi_instance_s * vsi)
{
}

static void init(struct tagVsi_instance_s * vsi)
{
    vsi->state = VDP_S_SM_INIT;

    if( vsi->operCmd)
    {
        GLUE_FREE(vsi->operCmd);
    }
    vsi->operCmd = NULL;

    if( vsi->rxResp)
    {
        GLUE_FREE(vsi->rxResp);
    }
    vsi->rxResp = NULL;

    if( vsi->sysCmd)
    {
        GLUE_FREE(vsi->sysCmd);
    }
    vsi->sysCmd = NULL;

    vsi->operCmd_Len = 0;
    vsi->vsiState = VDP_VSISTATE_DEASSOCIATE;   
}

static void TxTLV_sysCmd(struct tagVsi_instance_s * vsi)
{
    /* vdp_s->sysCmd; */
    DWORD32 tlvLen = 0;
    BYTE *  pTlv = NULL;

    /* 这边应该将此内存释放; */
    if( vsi->sysCmd )
    {
        GLUE_FREE(vsi->sysCmd);
        vsi->sysCmd = NULL;
        vsi->sysCmd_len = 0;
    }
    if( 0 != vdp_s_build_Tlv(vsi, vsi->cfg_state,&pTlv, &tlvLen))
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_s_sm TxTLV_sysCmd: vdp_s_build_Tlv(%s) failed.\n",GetVsiidstr(vsi->vsiID));
        return;
    }
    
    vsi->sysCmd = pTlv;
    vsi->sysCmd_len = tlvLen;
    vsi->sysCmd_TlvType = vsi->cfg_state;
    vsi->profile_chg = FALSE;

    if( 0 != vdp_s_tlv_insert_ecplst(vsi->S_VDPPORTNUM,pTlv,tlvLen) )
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_s_sm TxTLV_sysCmd: vdp_s_tlv_insert_ecplst(port:%u,vsiid:%s) failed.\n",
            vsi->S_VDPPORTNUM,GetVsiidstr(vsi->vsiID));
        return;
    }

    if( GetDbgVdpPrintStatus())
    {
        PRINTF("vdp_s_sm TxTLV_sysCmd success - (port:%u,vsiID: %s) \n", vsi->S_VDPPORTNUM, GetVsiidstr(vsi->vsiID) );
    }
    
    if(GetDbgVdpPacket())
    {
        PRINTF("VDP Send  port:%u,",vsi->S_VDPPORTNUM);
        PrintLocalTimer();
        DbgMemShow(pTlv,tlvLen);
    }
}

static void TxTLV_buildDea(struct tagVsi_instance_s * vsi)
{
    // 
    DWORD32 tlvLen = 0;
    BYTE * pTlv = NULL;
    if( 0 != vdp_s_build_Tlv(vsi,VDPTLV_TYPE_DEASSOCIATE,&pTlv, &tlvLen))
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_s_sm TxTLV_sysCmd: vdp_s_build_Tlv(port:%u,vsiid:%s) failed.\n",
            vsi->S_VDPPORTNUM,GetVsiidstr(vsi->vsiID));
        return;
    }
    
    if( 0 != vdp_s_tlv_insert_ecplst(vsi->S_VDPPORTNUM,pTlv,tlvLen) )
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_s_sm TxTLV_sysCmd: vdp_s_tlv_insert_ecplst(port:%u,vsiid:%s) failed.\n",
            vsi->S_VDPPORTNUM,GetVsiidstr(vsi->vsiID));;
        return;
    }

    if( GetDbgVdpPrintStatus())
    {
        PRINTF("vdp_s_sm TxTLV_buildDea success -(port:%u, vsiID: %s) \n", vsi->S_VDPPORTNUM, GetVsiidstr(vsi->vsiID) );
    }

    if(GetDbgVdpPacket())
    {
        PRINTF("VDP Send port:%u,",vsi->S_VDPPORTNUM);
        PrintLocalTimer();
        DbgMemShow(pTlv,tlvLen);
    }

    /*为啥这边需要释放内存;*/
    if( pTlv)
    {
        GLUE_FREE(pTlv);
    }
}

static void TxTLV_operCmd(struct tagVsi_instance_s * vsi)
{
    if(NULL == vsi->operCmd)
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_s_sm: TxTLV_operCmd(port:%u,vsiid:%s) is NULL.\n", vsi->S_VDPPORTNUM, GetVsiidstr(vsi->vsiID) );
        return;
    }

    if(vsi->operCmd_Len <= 0 )
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_s_sm: TxTLV_operCmd(port:%u,vsiid:%s) len is 0.\n", vsi->S_VDPPORTNUM, GetVsiidstr(vsi->vsiID) );
        return;
    }

    if( 0 != vdp_s_tlv_insert_ecplst(vsi->S_VDPPORTNUM, vsi->operCmd,vsi->operCmd_Len) )
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_s_sm TxTLV_sysCmd: vdp_s_tlv_insert_ecplst(port:%u,vsiid:%s) failed.\n", vsi->S_VDPPORTNUM, GetVsiidstr(vsi->vsiID));
        return;
    }

    if( GetDbgVdpPrintStatus())
    {
        PRINTF("vdp_s_sm TxTLV_sysCmd success - (port:%u,vsiID: %s) \n", vsi->S_VDPPORTNUM, GetVsiidstr(vsi->vsiID) );
    }

    if(GetDbgVdpPacket())
    {
        PRINTF("VDP Send  port:%u,",vsi->S_VDPPORTNUM);
        PrintLocalTimer();
        DbgMemShow(vsi->operCmd,vsi->operCmd_Len);
    }
}

static void s_proc(struct tagVsi_instance_s * vsi)
{
    // s_proc 存在直接->init 的情况;
    vsi->newCmd = FALSE;

    vsi->state = VDP_S_SM_STATION_PROC;

    if(vsi->rxResp)
    {
        GLUE_FREE(vsi->rxResp);
    }

    vsi->rxResp = NULL;
    TxTLV_sysCmd(vsi); 
    vsi->waitWhile = vsi->respWaitDelay;//VDP_S_RESPWAITDELAY_DEFAULT;
}

static void pre_new(struct tagVsi_instance_s * vsi)
{
    vsi->state = VDP_S_SM_PREASSOC_NEW;

    if( vsi->operCmd )
    {
        GLUE_FREE(vsi->operCmd);
        vsi->operCmd = NULL;
        vsi->operCmd_Len = 0;
    }
    
    if( vsi->sysCmd_len <= 0)
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_s_sm pre_new:sysCmd_len (len %u, port:%u,vsiid:%s) <= 0.\n", vsi->sysCmd_len,vsi->S_VDPPORTNUM, GetVsiidstr(vsi->vsiID));
        return;
    }

    vsi->operCmd = (BYTE *)GLUE_ALLOC(vsi->sysCmd_len);
    if( NULL == vsi->operCmd )
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_s_sm pre_new:allow memory (len %u,port:%u,vsiid:%s) failed.\n", vsi->sysCmd_len,vsi->S_VDPPORTNUM, GetVsiidstr(vsi->vsiID));
        return;
    }
    MEMCPY(vsi->operCmd, vsi->sysCmd, vsi->sysCmd_len);
    vsi->operCmd_Len = vsi->sysCmd_len;

    vsi->operCmd_TlvType = vsi->sysCmd_TlvType;// 这版需要确认;

    vsi->vsiState =vsi->sysCmd_TlvType; /* 确保正确 不用从syscmd中进行解析;*/

    if( GetDbgVdpPrintStatus())
    {
        PRINTF("port:%u,vsi:%s,vsiState - %s \n", vsi->S_VDPPORTNUM,GetVsiidstr(vsi->vsiID),GetVdpTlvTypeString(vsi->vsiState) );
    }

    vsi->newCmd = FALSE;

    vdp_s_sm(vsi,VDP_S_EVENT_UCT);
}

static void ass_new(struct tagVsi_instance_s * vsi)
{
    vsi->state = VDP_S_SM_ASSOC_NEW;

    if( vsi->operCmd )
    {
        GLUE_FREE(vsi->operCmd);
        vsi->operCmd = NULL;
        vsi->operCmd_Len = 0;
    }

    if( vsi->sysCmd_len <= 0)
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_s_sm ass_new:sysCmd_len (len %u, port:%u, vsiid:%s) <= 0.\n",
        vsi->sysCmd_len,vsi->S_VDPPORTNUM,GetVsiidstr(vsi->vsiID));
        return;
    }

    vsi->operCmd = (BYTE *)GLUE_ALLOC(vsi->sysCmd_len);
    if( NULL == vsi->operCmd )
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_s_sm pre_new:allow memory (len %u,  port:%u, vsiid:%s) failed.\n", 
        vsi->sysCmd_len,vsi->S_VDPPORTNUM,GetVsiidstr(vsi->vsiID));
        return;
    }
    MEMCPY(vsi->operCmd, vsi->sysCmd, vsi->sysCmd_len);
    vsi->operCmd_Len = vsi->sysCmd_len;

    vsi->operCmd_TlvType = vsi->sysCmd_TlvType;
    
    vdp_s_sm(vsi,VDP_S_EVENT_UCT);
}

static void ass_cmplt(struct tagVsi_instance_s * vsi)
{
    vsi->state = VDP_S_SM_ASSOC_COMPLETE;

    vsi->vsiState = vsi->operCmd_TlvType;
    if( GetDbgVdpPrintStatus())
    {
        PRINTF("port:%u,vsi:%s,vsiState - %s \n", vsi->S_VDPPORTNUM,GetVsiidstr(vsi->vsiID),GetVdpTlvTypeString(vsi->vsiState) );
    }
    vsi->newCmd = FALSE;
    vdp_s_sm(vsi,VDP_S_EVENT_UCT);
}

static void wt_s_cmd(struct tagVsi_instance_s * vsi)
{
    vsi->state = VDP_S_SM_WAIT_SYS_CMD;

    vsi->waitWhile = vsi->reinitKeepAlive; //VDP_S_REINITKEEPALIVE_DEFAULT;
}

static void t_keep(struct tagVsi_instance_s * vsi)
{
    DWORD32 tlvLen = 0;
    BYTE *  pTlv = NULL;
    vsi->state = VDP_S_SM_TRANSMIT_KEEPALIVE;

    if(vsi->rxResp )
    {
        GLUE_FREE(vsi->rxResp);
    }
    vsi->rxResp = NULL;

    // profile chg
    if( vsi->profile_chg)
    {
        if( vsi->operCmd )
        {
            GLUE_FREE(vsi->operCmd);
        }
        vsi->operCmd = NULL;

        if( 0 != vdp_s_build_Tlv(vsi, vsi->operCmd_TlvType,&pTlv, &tlvLen))
        {
            EVB_LOG(EVB_LOG_ERROR, "vdp_s_sm TxTLV_sysCmd: vdp_s_build_Tlv(port:%u,vsiid:%s) failed.\n",
                vsi->S_VDPPORTNUM,GetVsiidstr(vsi->vsiID));
            return;
        }

        vsi->operCmd = pTlv;
        vsi->operCmd_Len = tlvLen;
        // vsi->operCmd_TlvType;
        vsi->profile_chg = FALSE;

        if( 0 != vdp_s_tlv_insert_ecplst(vsi->S_VDPPORTNUM,pTlv,tlvLen) )
        {
            EVB_LOG(EVB_LOG_ERROR, "vdp_s_sm t_keep: vdp_s_tlv_insert_ecplst(port:%u,vsiid:%s) failed.\n",
                vsi->S_VDPPORTNUM,GetVsiidstr(vsi->vsiID));
        }

        if( GetDbgVdpPrintStatus())
        {
            PRINTF("vdp_s_sm TxTLV_operCmd success - (port:%u,vsiID %s) \n", vsi->S_VDPPORTNUM, GetVsiidstr(vsi->vsiID) );
        }
    }
    else
    {
        TxTLV_operCmd(vsi);
    }
    vsi->waitWhile = vsi->respWaitDelay; // VDP_S_RESPWAITDELAY_DEFAULT;
}

static void t_deass(struct tagVsi_instance_s * vsi)
{
    vsi->state = VDP_S_SM_TRANSMIT_DEASSOC;

    // buildDea(vsi);
    TxTLV_buildDea(vsi);
    vsi->waitWhile = vsi->respWaitDelay;//VDP_S_RESPWAITDELAY_DEFAULT;
}

/* station sm */
void (* vdp_s_actions[NUM_VDP_S_SM_EVENTS][NUM_VDP_S_SM_STATES]) (struct tagVsi_instance_s * vdp_s) =
{
    /*
    * event/state         IDLE   INIT   T_DEASSOC  S_PROC    PREASSOC_NEW  ASSOC_NEW  ASSOC_CMPLT WAIT_S_CMD T_KEEPALIVE 
    */
    /* BEGIN  */         {init,  s_err, s_err,     s_err,    s_err,        s_err,     s_err,      s_err,     s_err,},
    /* NEWCMD  */        {s_err, s_proc,s_err,     s_err,    s_err,        s_err,     s_err,      s_proc,    s_err,},
    /* PREASSOC_NEW*/    {s_err, s_err, s_err,     pre_new,  s_err,        s_err,     s_err,      s_err,     s_err,},
    /* ASSOC_NEW*/       {s_err, s_err, s_err,     ass_new,  s_err,        s_err,     s_err,      s_err,     s_err,},
    /* ASSOC_COMPLETE*/  {s_err, s_err, s_err,     ass_cmplt,  s_err,        s_err,     s_err,      s_err,     s_err,},
    /* UCT */            {s_err, s_err, s_err,     ass_cmplt,wt_s_cmd,     ass_cmplt, wt_s_cmd,   s_err,     s_err,},
    /* WAITWHILE=0 */    {s_err, s_err, init,      init,     s_err,        s_err,     s_err,      t_keep,    init,},
    /* RXRESP_TLV=DEASS*/{s_err, s_err, s_err,     init,     s_err,        s_err,     s_err,      init,      init,},
    /* TRANSMIT_DEASSOC*//*{s_err, s_err, s_err,     s_err,    s_err,        s_err,     s_err,      s_err,     s_err,},*/
    /* RXRESP_TLV=ASS&K*/{s_err, s_err, s_err,     s_err,    s_err,        s_err,     s_err,      s_err,     t_deass,},
    /* RXRESP_REASON==0*/{s_err, s_err, s_err,     s_err,    s_err,        s_err,     s_err,      s_err,     wt_s_cmd,},
    /* OPERCMD_ELSE */   {s_err, s_err, s_err,     s_err,    s_err,        s_err,     s_err,      s_err,     init,},
    /* SYSCMD_ELSE */    {s_err, s_err, s_err,     init,     s_err,        s_err,     s_err,      s_err,     s_err,},   
    /* RXRESP !=NULL */  {s_err, s_err, init,      s_err,    s_err,        s_err,     s_err,      s_err,     s_err,},
    /* PORT_UP */        {s_err, s_err, s_err,     s_err,    s_err,        s_err,     s_err,      s_err,     s_err,},
    /* PORT_DOWN*/       {s_err, s_err, s_err,     s_err,    s_err,        s_err,     s_err,      s_err,     s_err,},
    /* CMD_CANCEL*/      //{init, init,   init,      init,     init,         init,      init,      init,     init,},
};

void vdp_s_sm(struct tagVsi_instance_s * vsi, VDP_S_SM_EVENT event)
{
    if( ( NULL == vsi) || !vdp_s_sm_event_valid(event) )
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_s_sm: param[%p %d] invalid.\n",  vsi,event);
        return;
    }
    if( !vdp_s_sm_state_valid(vsi->state))
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_s_sm: vsi state[port:%u,vsiid:%s,state:%d] invalid.\n",
            vsi->S_VDPPORTNUM, GetVsiidstr(vsi->vsiID), vsi->state);
        return;
    }

    if( GetDbgVdpEvent())
    {
        PrintLocalTimer();
        PRINTF("vdp_s_sm event (port:%u,vsiid:%s) - [%s] [%s] [%s]\n", vsi->S_VDPPORTNUM,GetVsiidstr(vsi->vsiID),GetVdpTlvTypeString(vsi->vsiState),vdp_s_sm_state[vsi->state],vdp_s_sm_event[event] );
    }

    (*(vdp_s_actions[event][vsi->state])) (vsi);

    if( GetDbgVdpEvent())
    {
        PrintLocalTimer();
        EVB_ASSERT(vdp_s_sm_state_valid(vsi->state));
        PRINTF("vdp_s_sm event (port:%u,vsiid:%s), currentstate - [%s] [%s] \n",vsi->S_VDPPORTNUM,GetVsiidstr(vsi->vsiID),GetVdpTlvTypeString(vsi->vsiState), vdp_s_sm_state[vsi->state] );
    }

    return;
}

void static vdp_find_cancelCmd_vsi(struct tagVdp_s * vdp)
{
    struct tagVsiCmdPoolItem * pItem = NULL;
    struct tagVsi_instance_s * pVsi = NULL;

    LIST_FOREACH(pItem,&g_lstVsiCmdPool,lstEntry)
    {
        if(pItem->port != vdp->PORT_NUM )
        {
            continue;
        }
        LIST_FOREACH(pVsi, &vdp->lstVsi,lstEntry)
        {
            if( (0 == MEMCMP(pVsi->vsiID,pItem->vsiID,VDPTLV_VSIID_LEN)) &&
                (pItem->port == pVsi->S_VDPPORTNUM ) &&
                (pItem->vsiIDFormat == pVsi->vsiIDFormat))
            {
                if(pItem->cmdTlvType != pVsi->cfg_state )
                {
                    pVsi->cancelCmd = TRUE;
                }
            }
        }
    }

    return;
}

void static vdp_s_can_destory_vsi(struct tagVdp_s * vdp)
{
    struct tagVsi_instance_s * pVsi = NULL;
    struct tagVsi_instance_s * pVsiTmp = NULL;
    char strVsiid[64]={0};
    int i = 0;
    char * pTmp = strVsiid;

    pVsi = LIST_FIRST(&vdp->lstVsi);
    while(NULL != pVsi)
    {
        if(((VDP_S_SM_INIT == pVsi->state) && (FALSE == pVsi->newCmd)) || pVsi->cancelCmd ||( EVB_TLV_STATUS_SUCC != vdp->evbtlvStatus))
        {
            memset(strVsiid,0,64);
            pTmp = strVsiid;
            for(i = 0; i<VDPTLV_VSIID_LEN;i++)
            {
                sprintf(pTmp,"%2.2x ",pVsi->vsiID[i]);
                pTmp += 3;
            }
            EVB_LOG(EVB_LOG_INFO, "vdp_s_loop_timer_handler: state[%u],newCmd[%u],cancelCmd[%u] remove vsi(%s)\n", pVsi->state,pVsi->newCmd,pVsi->cancelCmd,strVsiid);
            
            
            pVsiTmp = pVsi;
            pVsi = LIST_NEXT(pVsi,lstEntry);

            LIST_REMOVE(pVsiTmp,lstEntry);

            // free
            if(pVsiTmp->operCmd)
            {
                GLUE_FREE(pVsiTmp->operCmd);
            }
            if(pVsiTmp->rxResp)
            {
                GLUE_FREE(pVsiTmp->rxResp);
            }
            if(pVsiTmp->sysCmd)
            {
                GLUE_FREE(pVsiTmp->sysCmd);
            }

            GLUE_FREE(pVsiTmp);
            pVsiTmp = NULL;
        }
        else
        {
            pVsi = LIST_NEXT(pVsi,lstEntry);
        }		
    }
}

void static vdp_cmdPool_newvsi(struct tagVdp_s * vdp)
{
    struct tagVsiCmdPoolItem * pItem = NULL;
    struct tagVsi_instance_s * pVsi = NULL;
    BYTE bFind = FALSE;

    LIST_FOREACH(pItem,&g_lstVsiCmdPool,lstEntry)
    {
        if(pItem->port != vdp->PORT_NUM )
        {
            continue;
        }

        bFind = FALSE;
        LIST_FOREACH(pVsi, &vdp->lstVsi,lstEntry)
        {			
            if( (0 == MEMCMP(pVsi->vsiID,pItem->vsiID,VDPTLV_VSIID_LEN)) &&
                (pItem->vsiIDFormat == pVsi->vsiIDFormat))
            {
                bFind = TRUE;
            }
        }

        if( FALSE == bFind)
        {
            vdp_vm_api(pItem->port,pItem->vsiIDFormat,pItem->vsiID,pItem->cmdTlvType);
        }

        pItem->newFlag = FALSE;/*SET FALSE*/
    }
}

void static vdp_can_destroy_cmdPool(struct tagVdp_s * vdp)
{
    struct tagVsiCmdPoolItem * pItem = NULL;
    struct tagVsiCmdPoolItem * pItemTmp = NULL;
    struct tagVsi_instance_s * pVsi = NULL;
    BYTE bFind = FALSE;

    pItem = LIST_FIRST(&g_lstVsiCmdPool);
    while(NULL != pItem)
    {
        if(pItem->port != vdp->PORT_NUM )
        {
            pItem = LIST_NEXT(pItem,lstEntry);
            continue;		
        }

        if( (pItem->cmdTlvType != VDPTLV_TYPE_DEASSOCIATE) || pItem->newFlag )
        {
            pItem = LIST_NEXT(pItem,lstEntry);
            continue;
        }

        bFind = FALSE;
        LIST_FOREACH(pVsi, &vdp->lstVsi,lstEntry)
        {			
            if( (0 == MEMCMP(pVsi->vsiID,pItem->vsiID,VDPTLV_VSIID_LEN)) &&
                (pItem->vsiIDFormat == pVsi->vsiIDFormat))
            {
                bFind = TRUE;
            }
        }

        if( FALSE == bFind)
        {
            pItemTmp = pItem;
            pItem = LIST_NEXT(pItem,lstEntry);

            LIST_REMOVE(pItemTmp,lstEntry);

            GLUE_FREE(pItemTmp);
            pItemTmp = NULL;
        }
        else
        {
            pItem = LIST_NEXT(pItem,lstEntry);
        }
    }

}

void static vdp_can_destroy_profile(struct tagVdp_s* vdp)
{
    struct tagVsiProfile * pVsi = NULL;
    struct tagVsiProfile * pVsiTmp = NULL;
    struct tagVsiCmdPoolItem * pItem = NULL;
    BYTE bFind = FALSE;

    pVsi = LIST_FIRST(&g_lstVsiProfile);
    while(NULL != pVsi)
    {
        bFind = FALSE;
        LIST_FOREACH(pItem,&g_lstVsiCmdPool,lstEntry)
        {
            if( (0 == MEMCMP(pVsi->vsiID,pItem->vsiID,VDPTLV_VSIID_LEN)) &&
                (pItem->vsiIDFormat == pVsi->vsiIDFormat))
            {
                bFind = TRUE;
            }
        }

        if( FALSE == bFind)
        {
            pVsiTmp = pVsi;
            pVsi = LIST_NEXT(pVsi,lstEntry);

            LIST_REMOVE(pVsiTmp,lstEntry);

            GLUE_FREE(pVsiTmp);
            pVsiTmp = NULL;
        }
        else
        {
            pVsi = LIST_NEXT(pVsi,lstEntry);
        }		
    }
}

void vdp_s_loop_timer_handler(struct tagVdp_s * vdp)
{
    // 遍历队列  进行处理; 
    struct tagVsi_instance_s * pVsi = NULL;
    // PRINTF_LOOPVAR_DEFINE
    //struct tagVsi_instance_s * pVsiTmp = NULL;

    if( NULL == vdp)
    {
        return;
    }
    
    vdp->evbtlvStatus = vdp_GetEvbTlvNegResult(vdp->PORT_NUM);
    if( EVB_TLV_STATUS_SUCC != vdp->evbtlvStatus)
    {
        vdp_s_can_destory_vsi(vdp);
        return;
    }

    // cancel Cmd
    vdp_find_cancelCmd_vsi(vdp);

    // can destory vsi
    vdp_s_can_destory_vsi(vdp);

    // cmd pool and vsi profile destory
    // if vsi no exist && cmd pool is DEASSICATE 
    vdp_can_destroy_cmdPool(vdp);
    vdp_can_destroy_profile(vdp);

    // Cmd pool 处理;
    vdp_cmdPool_newvsi(vdp);
    
    // vsi timer 处理;
    LIST_FOREACH(pVsi, &vdp->lstVsi,lstEntry)
    {
        if(VDP_S_SM_IDLE == pVsi->state )
        {
            vdp_s_sm(pVsi,VDP_S_EVENT_BEGIN);
            continue;
        }

        if( pVsi->newCmd )
        {
            if( VDP_S_SM_INIT == pVsi->state || \
                VDP_S_SM_WAIT_SYS_CMD == pVsi->state)
            {
                vdp_s_sm(pVsi, VDP_S_EVENT_NEWCMD);
                continue;
            }
        }

        if( pVsi->waitWhile > VDP_LOOP_TIMER_INTREVAL )
        {
            pVsi->waitWhile -= VDP_LOOP_TIMER_INTREVAL;
        }
        else
        {
            pVsi->waitWhile = 0;
        }

        if( 0 == pVsi->waitWhile)
        {
            // 暂时将之屏蔽  便于调试 防止重发报文;
            pVsi->newCmd = FALSE;

            if( VDP_S_SM_STATION_PROC== pVsi->state || \
                VDP_S_SM_WAIT_SYS_CMD== pVsi->state || \
                VDP_S_SM_TRANSMIT_KEEPALIVE== pVsi->state )
            {
                vdp_s_sm(pVsi,VDP_S_EVENT_WAITWHILE_EQU_ZERO);
                continue;
            }
        }
    }

}

static INT vdp_s_vsi_cmp_vdptlv(struct tagVsi_instance_s * vsi,struct tagVdpTlv_VDPAss* tlv )
{
    INT ret = 0;
    if( vsi == NULL || tlv == NULL)
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_s_vsi_cmp_vdptlv: param is null.\n");
        return -1;
    }

    ret = MEMCMP(vsi->vsiID,tlv->vsiID, VDPTLV_VSIID_LEN);
    ret |= (vsi->vsiIDFormat == tlv->vsiIDFormat ) ? 0:1;
    
    if( 0 != ret)
    {
        return -1;
    }

    return 0;
}

static INT vdp_tlv_reason_is_zero(struct tagVdpTlv_VDPAss_Reason * reason)
{
    // BYTE zero = 0;
    if(NULL == reason )
    {
        return -1;
    }

    /* 仅仅判断错误码信息; */
    if( 0 != reason->error )
    {
        EVB_LOG(EVB_LOG_ERROR,"vdp recv reason (%u) is not equal zero\n",reason->error);
        return -1;
    }
    return 0;
}

static INT vdp_tlv_cmp_operCmd_exceptReason(struct tagVdpTlv_VDPAss* vdpAss,struct tagVsi_instance_s * vsi)
{
    u32 mgrid_len = 0;
    if(NULL == vdpAss || NULL == vsi)
    {
        return -1;
    }

    // 后面整改成operCmd
    if( vdpAss->tlvHead.tlvType != vsi->operCmd_TlvType)
    {
        return -1;
    }

    mgrid_len = VDPTLV_VSIMGRID_LEN + VDPTLV_HEAD_LEN;
#if 0    
    if(vsi->operCmd_Len <= mgrid_len )
    {
        EVB_LOG(EVB_LOG_ERROR,"vdp_tlv_cmp_operCmd_exceptReason < mgrid length.\n");
        return -1;
    }

    if( 0 != MEMCPY(vdpAss,vsi->operCmd + mgrid_len,vsi->operCmd_Len - mgrid_len) )
    {
        EVB_LOG(EVB_LOG_ERROR,"vdp recv vdpass not equal operCmd.\n");
        return -1;
    }
#endif     
    return 0;
}

static INT vdp_tlv_cmp_sysCmd_exceptReason(struct tagVdpTlv_VDPAss* vdpAss,struct tagVsi_instance_s * vsi)
{
    u32 mgrid_len = 0;
    if(NULL == vdpAss || NULL == vsi)
    {
        return -1;
    }

    // 后面整改成sysCmd
    if( vdpAss->tlvHead.tlvType != vsi->sysCmd_TlvType)
    {
        return -1;
    }

    mgrid_len = VDPTLV_VSIMGRID_LEN + VDPTLV_HEAD_LEN;
#if 0
    if(vsi->operCmd_Len <= mgrid_len )
    {
        EVB_LOG(EVB_LOG_ERROR,"vdp_tlv_cmp_operCmd_exceptReason < mgrid length.\n");
        return -1;
    }

    if( 0 != MEMCPY(vdpAss,vsi->operCmd + mgrid_len,vsi->operCmd_Len - mgrid_len) )
    {
        EVB_LOG(EVB_LOG_ERROR,"vdp recv vdpass not equal operCmd.\n");
        return -1;
    }
#endif 
    return 0;
}

void vdp_s_rx_ReceiveFrame(struct tagVdp_s * vdp,u32 local_port, u8* buf, u16 len)
{
    // 1 mgr id
    struct tagVsi_instance_s * pVsi = NULL;
    struct tagVdpTlv_head tlvHead;
    struct tagVdpTlv_VSIMgrID mgrID;
    struct tagVdpTlv_VDPAss * vdpAss = NULL;
    BYTE   *pTmpVdpAss = NULL;
    struct tagVdpTlv_OrgDfn  OrgDfn;
    BYTE * pTmp = NULL;
    UINT16 _len = 0;
    BYTE  bExist = FALSE;

    if( vdp == NULL ||  buf == NULL || len < (VDPTLV_VSIMGRID_LEN + VDPTLV_HEAD_LEN) )
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_s_rx_ReceiveFrame: param is invalid (port:%u,buf:%p, len:%u).\n",local_port,buf, len);
        return ;
    }

    MEMSET(&tlvHead, 0, sizeof(tlvHead) );
    MEMSET(&mgrID, 0 , sizeof(mgrID));
    //MEMSET(&vdpAss, 0 , sizeof(vdpAss));
    MEMSET(&OrgDfn, 0 , sizeof(OrgDfn));

    pTmp = buf;
    _len = len;

    //MEMCPY(&mgrID, pTmp, VDPTLV_VSIMGRID_LEN + VDPTLV_HEAD_LEN);
    MEMCPY(&mgrID, pTmp, sizeof(mgrID));
    vdpTlvHead_NTOH(&mgrID.tlvHead);
    // check len valid
    if(mgrID.tlvHead.tlvType != VDPTLV_TYPE_VSI_MGRID)
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_s_rx_ReceiveFrame:(port:%u) first tlv (%u) is not VSI_MGRID.\n",local_port,mgrID.tlvHead.tlvType );
        return ;
    }

    pTmp += (VDPTLV_VSIMGRID_LEN + VDPTLV_HEAD_LEN);
    _len -= (VDPTLV_VSIMGRID_LEN + VDPTLV_HEAD_LEN);

    while(_len >= VDPTLV_HEAD_LEN)
    {
        bExist = FALSE; 

        MEMCPY(&tlvHead, pTmp,VDPTLV_HEAD_LEN );
        vdpTlvHead_NTOH(&tlvHead);
        if( _len < (VDPTLV_HEAD_LEN + tlvHead.tlvLen))
        {
            EVB_LOG(EVB_LOG_ERROR, "vdp_s_rx_ReceiveFrame: (port:%u) tlv packet is invalid.\n",local_port );
            return ;
        }
        switch(tlvHead.tlvType)
        {
        case VDPTLV_TYPE_PRE_ASSOCIATE:
        case VDPTLV_TYPE_PRE_ASSOCIATE_WITH_RR:
        case VDPTLV_TYPE_ASSOCIATE:
        case VDPTLV_TYPE_DEASSOCIATE:
            {
                if((UINT32)(VDPTLV_HEAD_LEN+tlvHead.tlvLen) < sizeof(struct tagVdpTlv_VDPAss))
                {
                    EVB_LOG(EVB_LOG_ERROR, "vdp_s_rx_ReceiveFrame:(port:%u) tlv packet < VdpAss struct is invalid.\n",local_port );
                    break;
                }

                vdpAss = (struct tagVdpTlv_VDPAss *)GLUE_ALLOC(VDPTLV_HEAD_LEN+tlvHead.tlvLen);
                pTmpVdpAss = (BYTE *)vdpAss;
                if( NULL == pTmpVdpAss)
                {
                    EVB_LOG(EVB_LOG_ERROR, "vdp_s_rx_ReceiveFrame:(port:%u) new tmp vdpAss alloc memory failed.\n",local_port );
                    break;
                }

                MEMCPY(vdpAss, pTmp,VDPTLV_HEAD_LEN+tlvHead.tlvLen );
                
               /*if not ack, continue;*/
               if(!EVB_BIT_TEST(vdpAss->reason.ReqAck,VDPASSTLV_REASON_ACK))
               {
                   EVB_LOG(EVB_LOG_ERROR, "vdp_s_rx_ReceiveFrame: (port:%u) vdpAss status ReqAck(%u) is not Ack.\n",local_port,vdpAss->reason.ReqAck );
                   break;
               }

                vdpTlvHead_NTOH(&vdpAss->tlvHead);
                LIST_FOREACH(pVsi, &vdp->lstVsi,lstEntry)
                {
                    if( 0 != vdp_s_vsi_cmp_vdptlv(pVsi, vdpAss) )
                    {
                        continue;
                    }
                    switch(pVsi->state)
                    {
                    case VDP_S_SM_TRANSMIT_DEASSOC:
                        {
                            vdp_s_sm(pVsi,VDP_S_EVENT_RXRESP_NOT_NULL);
                        }
                        break;
                    case VDP_S_SM_STATION_PROC:
                        {
#if 1
                            if(VDPTLV_TYPE_DEASSOCIATE == vdpAss->tlvHead.tlvType )
                            {
                                vdp_s_sm(pVsi,VDP_S_EVENT_RXRESP_TLVTYPE_EQU_DEASSOC);
                                break;
                            }

                            if( 0 == vdp_tlv_cmp_sysCmd_exceptReason(vdpAss,pVsi))
                            {
                                if(0 == vdp_tlv_reason_is_zero(&vdpAss->reason) )
                                {
                                    switch(vdpAss->tlvHead.tlvType)
                                    {
                                    case VDPTLV_TYPE_PRE_ASSOCIATE:
                                    case VDPTLV_TYPE_PRE_ASSOCIATE_WITH_RR:
                                        {
                                            vdp_s_sm(pVsi,VDP_S_EVENT_PREASSOC_NEW);
                                        }
                                        break;
                                    case VDPTLV_TYPE_ASSOCIATE:
                                        {
                                            vdp_s_sm(pVsi,VDP_S_EVENT_ASSOC_NEW);
                                        }
                                        break;
                                    case VDPTLV_TYPE_DEASSOCIATE:
                                        {
                                            vdp_s_sm(pVsi,VDP_S_EVENT_RXRESP_TLVTYPE_EQU_DEASSOC);
                                        }
                                        break;
                                    default: 
                                        {
                                            /* 是否应该记录下 后续考虑; */
                                        }
                                        break;
                                    }
                                }
                                else
                                {
                                    EVB_LOG(EVB_LOG_ERROR,"vdp_s_rx_ReceiveFrame vdp_tlv_reason_is_zero(port:%u,vsiid:%s) is not equal zero\n",
                                         pVsi->S_VDPPORTNUM,GetVsiidstr(pVsi->vsiID));
                                    if( (1 == vdpAss->reason.s_keep) && \
                                        (VDPTLV_TYPE_ASSOCIATE == vdpAss->tlvHead.tlvType) && \
                                        (pVsi->vsiState == VDPTLV_TYPE_ASSOCIATE ) )
                                    {
                                        vdp_s_sm(pVsi,VDP_S_EVENT_ASSOC_COMPLETE );
                                    }
                                }
                            }
                            else
                            {
                                // 目前暂考虑reason != 0 情况，后面需考虑和SYSCMD comp
                                vdp_s_sm(pVsi,VDP_S_EVENT_SYSCMD_EXCEPTREASON_ELSE);
                            }
#endif 
                        }                   
                        break;
                    case VDP_S_SM_WAIT_SYS_CMD:
                        {
                            if( VDPTLV_TYPE_DEASSOCIATE == vdpAss->tlvHead.tlvType )
                            {
                                vdp_s_sm(pVsi,VDP_S_EVENT_RXRESP_TLVTYPE_EQU_DEASSOC);
                            }
                        }
                        break;
                    case VDP_S_SM_TRANSMIT_KEEPALIVE:
                        {
                            // if( vdpAss.tlvHead.tlvType == pVsi->)

                            if(VDPTLV_TYPE_DEASSOCIATE == vdpAss->tlvHead.tlvType )
                            {
                                vdp_s_sm(pVsi,VDP_S_EVENT_RXRESP_TLVTYPE_EQU_DEASSOC);
                                break;
                            }

                            if( 0 == vdp_tlv_cmp_operCmd_exceptReason(vdpAss,pVsi))
                            {
                                if(0 == vdp_tlv_reason_is_zero(&vdpAss->reason) )
                                {
                                    vdp_s_sm(pVsi,VDP_S_EVENT_RXRESP_REASON_EQU_ZERO);
                                    break;
                                }
                                else
                                {
                                    if( (VDPTLV_TYPE_ASSOCIATE == vdpAss->tlvHead.tlvType ) && \
                                        (1 == vdpAss->reason.s_keep))
                                    {
                                        vdp_s_sm(pVsi,VDP_S_EVENT_RXRESP_TVLTYPE_EQU_ASSOC_AND_KEEP);
                                        break;
                                    }

                                    vdp_s_sm(pVsi,VDP_S_EVENT_OPERCMD_EXCEPTREASON_ELSE);
                                    break;
                                }
                            }
                        }
                        break;
                    default:break;
                    }
                    
                }
            
                GLUE_FREE(vdpAss);
                vdpAss = NULL;
                pTmpVdpAss = NULL;
            }
            break;
        case VDPTLV_TYPE_ORG_DEFINE_TLV:
            {

            }
            break;
        default: 
            {

            }
            break;
        }
        _len -= VDPTLV_HEAD_LEN;
        _len -= tlvHead.tlvLen;
        pTmp += VDPTLV_HEAD_LEN;
        pTmp += tlvHead.tlvLen;
    }

}

INT list_vdp_s_insert(DWORD32 ifIndex, struct tagVsi_instance_s * pNewVsi)
{
    /**/
    struct tagVsi_instance_s * pVsi = NULL;
    struct tagEvbPort* port = NULL;

    port = GetEvbPort(ifIndex);
    if( NULL == port )
    {
        EVB_LOG(EVB_LOG_ERROR, "list_vdp_s_insert : (port:%u,) param invalid.\n",ifIndex);
        return -1;
    }

    EVB_ASSERT(pNewVsi);
    if( NULL == pNewVsi )
    {
        EVB_LOG(EVB_LOG_ERROR, "list_vdp_s_insert : (port:%u) NewVsi is NULL.\n",ifIndex);
        return -1;
    }
    
    if( !vdpecp_ifIndex_is_valid(ifIndex) )
    {
        EVB_LOG(EVB_LOG_ERROR, "list_vdp_s_insert:(port:%u,vsiid:%s) is invalid.\n",ifIndex,GetVsiidstr(pNewVsi->vsiID));
        return -1;
    }

    LIST_FOREACH(pVsi, &port->vdp_s->lstVsi,lstEntry)
    {
    }

    if(NULL == pVsi)
    {
        LIST_INSERT_HEAD(&port->vdp_s->lstVsi, pNewVsi, lstEntry);
    }
    else
    {
        LIST_INSERT_AFTER(pVsi,pNewVsi,lstEntry);
    }

    if( GetDbgVdpPrintStatus())
    {
        PRINTF("vdp list_vdp_s_insert - (port:%u,vsiid %s) \n", ifIndex, GetVsiidstr(pNewVsi->vsiID) );
    }
    return 0;
}

INT list_vdp_s_remove(DWORD32 ifIndex, struct tagVsi_instance_s * pNewVsi)
{
    struct tagVsi_instance_s * pVsi = NULL;
    struct tagEvbPort* port = NULL;

    port = GetEvbPort(ifIndex);
    if( NULL == port )
    {
        EVB_LOG(EVB_LOG_ERROR, "list_vdp_s_remove : (port:%u) param invalid.\n",ifIndex);
        return -1;
    }

    EVB_ASSERT(pNewVsi);
    if( !vdpecp_ifIndex_is_valid(ifIndex) )
    {
        EVB_LOG(EVB_LOG_ERROR, "list_vdp_s_remove:(port:%u) is invalid.\n", ifIndex);
        return -1;
    }

    LIST_FOREACH(pVsi, &port->vdp_s->lstVsi,lstEntry)
    {
        if( 0 == MEMCMP(pVsi->vsiID,pNewVsi->vsiID,VDPTLV_VSIID_LEN))
        {
            if( GetDbgVdpPrintStatus())
            {
                PRINTF("list_vdp_s_remove - (port:%u,vsi %s)\n", ifIndex,GetVsiidstr(pVsi->vsiID) );
            }

            LIST_REMOVE(pVsi,lstEntry);
            return 0;
        }
    }

    return 0;
}

INT vdp_s_sm_state_valid(UINT32 state)
{
    if(((state) >= VDP_S_SM_IDLE) && ((state) <= VDP_S_SM_TRANSMIT_KEEPALIVE)) 
    {
        return TRUE;
    }
    return FALSE;
}

int32 vsiProfile_chg_notify_vdp(BYTE vsiIDFormat, BYTE vsiID[VDPTLV_VSIID_LEN])
{
    struct tagVsi_instance_s * vsi = NULL;
    struct tagEvbPort *pPort = NULL;
    pPort = Get1stEvbPort();

    if(NULL == pPort)
    {
        return 0;
    }

    for(; pPort; pPort = GetNextEvbPort(pPort))
    {
        if( NULL == pPort->vdp_s)
        {
            continue;
        }

        LIST_FOREACH(vsi, &pPort->vdp_s->lstVsi,lstEntry)
        {
            /* 后续考虑是否需要添加; vsi_type id, mgr id*/
            if(0 == MEMCMP(vsiID,vsi->vsiID, VDPTLV_VSIID_LEN) && \
                vsi->vsiIDFormat == vsiIDFormat)
            {
                vsi->profile_chg = TRUE;
                return 0;
            }
        }
    }
        
    return 0;
}

#ifdef __cplusplus
}
#endif 
