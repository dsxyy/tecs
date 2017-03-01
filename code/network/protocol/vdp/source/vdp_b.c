

#include "ecp_in.h"
#include "vdp.h"
#include "vsi_b_outFilter.h"
#include "evb_port_mgr.h"

#ifdef __cplusplus
extern "C"{
#endif 

extern const char* vdp_b_sm_state[];
extern const char* vdp_b_sm_event[];

void vdp_b_sm(struct tagVsi_instance_b * vsi, VDP_B_SM_EVENT event);

INT vdp_b_tlv_insert_ecplst(DWORD32 ifIndex,BYTE * pTlv, DWORD32 tlvLen)
{
    struct ecp_ecpdu * pEcpdu = NULL;
    UINT16 seq = 0;
    BYTE * pULP = NULL;
    struct tagEvbPort* port = NULL;

    port = GetEvbPort(ifIndex);
    if( NULL == port )
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_b_tlv_insert_ecplst : port(%u) param invalid.\n",ifIndex);
        return -1;
    }

    /* 放入ecp缓存; */
    pEcpdu = (struct  ecp_ecpdu *)GLUE_ALLOC(sizeof(struct  ecp_ecpdu)); 
    if(NULL == pEcpdu)
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_b_tlv_insert_ecplst :port(%u) vdp b TxTLV alloc memory failed.\n",ifIndex);
        return -1;
    }   
    MEMSET(pEcpdu, 0x0, sizeof(struct ecp_ecpdu));

    pULP = (BYTE *)GLUE_ALLOC(tlvLen); 
    if(NULL == pULP)
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_b_tlv_insert_ecplst :port(%u) TxTLV alloc memory (len %u) failed.\n",ifIndex, tlvLen);
        GLUE_FREE(pEcpdu);
        return -1;
    }   
    MEMCPY(pULP, pTlv, tlvLen);

    GetECPSequenceNum(port->uiPortNo,&seq);
    //ecpduHead_HTON(ECP_ETHER_TYPE,ECP_VERSION,ECP_ACK,ECP_SUBTYPE_VDP,seq,(BYTE*)pEcpdu);
    ecpduHead_HTON(ECP_VERSION,ECP_REQUEST,ECP_SUBTYPE_VDP,seq,(BYTE*)pEcpdu);

    pEcpdu->ulpdu = pULP;

    list_EcpULPDUtx_insert(ifIndex,pEcpdu,tlvLen + EVB_ECP_ECPDU_HEAD_LEN);

    return 0;
}

static void b_err(struct tagVsi_instance_b * vsi)
{
}
static void b_end(struct tagVsi_instance_b * vsi)
{
    // 该删除vsi;
    EVB_ASSERT(vsi);
    if( GetDbgVdpEvent())
    {
        PRINTF("vdp_b_sm b_end - (port:%u,vsiID %s) \n", vsi->B_VDPPORTNUM, GetVsiidstr(vsi->vsiID) );
    }
}

static void init(struct tagVsi_instance_b * vsi)
{
    /*1 vdp_b下指针 空间释放; */
    /*2 */
    vsi->state = VDP_B_SM_INIT;
    vsi->operCmd = NULL;
    vsi->vsiState = VDP_VSISTATE_DEASSOCIATE;   
}

static void resouceFree(struct tagVsi_instance_b * vsi)
{
    //釋放資源;
    vsiBOutFlt_delvsiID(vsi->B_VDPPORTNUM,vsi->vsiIDFormat,vsi->vsiID);
}

static void rel_all(struct tagVsi_instance_b * vsi)
{
    vsi->state = VDP_B_SM_RELEASE_ALL;
    resouceFree(vsi);
}

static INT resouceCmd(struct tagVsi_instance_b * vsi, UINT16 delay, BYTE * result)
{
    // 直接成功;
    // 后续将resourceWaitDelay功能实现;暂不实现

    BYTE * pReason = NULL;

    EVB_ASSERT(vsi);

    if( vsi->Resp )
    {
        GLUE_FREE(vsi->Resp);
    }
    vsi->Resp = NULL;
    vsi->Resp_Len = 0;
    
    if(NULL == vsi->rxCmd || (vsi->rxCmd_Len <= 0) )
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_b_sm resouceCmd: (port:%u,vsiid:%s) rxCmd (%p %u) param invalid.\n",
        vsi->B_VDPPORTNUM,GetVsiidstr(vsi->vsiID),vsi->rxCmd, vsi->rxCmd_Len);
        return -1;
    }

    // out vsi b filter 
    vsi->Resp = (BYTE *)GLUE_ALLOC(vsi->rxCmd_Len);
    if( NULL == vsi->Resp)
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_b_sm resouceCmd: (port:%u,vsiid:%s) new Resp allow memory failed.\n",
            vsi->B_VDPPORTNUM,GetVsiidstr(vsi->vsiID));
        return -1;
    }

    MEMCPY(vsi->Resp, vsi->rxCmd, vsi->rxCmd_Len);
    vsi->Resp_Len = vsi->rxCmd_Len;
    vsi->Resp_TlvType = vsi->rxCmd_TlvType;

    /* 将reason bit 7 set 1; */
    pReason =  vsi->Resp + VDPTLV_HEAD_LEN;
    EVB_BIT_SET(*pReason, VDPASSTLV_REASON_RA);

    switch(vsi->Resp_TlvType)
    {
    case VDPTLV_TYPE_PRE_ASSOCIATE:
    case VDPTLV_TYPE_PRE_ASSOCIATE_WITH_RR:
    case VDPTLV_TYPE_ASSOCIATE:
        {
            *result = VDP_B_RESOUCECMD_RESULT_SUCCESS;
            return 0;
        }
        break;
    case VDPTLV_TYPE_DEASSOCIATE:
        {
            *result = VDP_B_RESOUCECMD_RESULT_DEASSOCIATE;
            return 0;
        }
        break;
    default:
        {
            EVB_LOG(EVB_LOG_ERROR, "vdp_b_sm resouceCmd: (port:%u,vsiid:%s) vsi.Resp_TlvType(%u) is invalid .\n",
            vsi->B_VDPPORTNUM,GetVsiidstr(vsi->vsiID),vsi->Resp_TlvType);
        }
        break;
    }

    *result = VDP_B_RESOUCECMD_RESULT_SUCCESS;
    return 0;
}

static void SetRespError(struct tagVsi_instance_b * vsi, BYTE err)
{
    BYTE * pReason = NULL;
    BYTE tmp = 0;
    
    err = (err & 0x0f);
    
    pReason =  vsi->Resp + VDPTLV_HEAD_LEN;

    tmp = *pReason;
    tmp |= err;

    *pReason = tmp;
}

static void b_proc(struct tagVsi_instance_b * vsi)
{
    BYTE result = 0;

    vsi->state = VDP_B_SM_BRIDGE_PROC;

    if( vsi->Resp )
    {
        GLUE_FREE(vsi->Resp);
    }
    vsi->Resp = NULL;
    vsi->Resp_Len = 0;
    vsi->resouceResult = VDP_B_RESOUCECMD_RESULT_NULL;
    if( 0 != resouceCmd(vsi, 0,&result))
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_b_sm b_proc (port:%u,vsiid:%s) resouceCmd failed.\n",
            vsi->B_VDPPORTNUM,GetVsiidstr(vsi->vsiID));
        return;
    }
    switch(result)
    {
    case VDP_B_RESOUCECMD_RESULT_SUCCESS:
        {
            // out vsi b filter 
            switch(vsi->Resp_TlvType )
            {
            //case VDPTLV_TYPE_PRE_ASSOCIATE_WITH_RR:
            case VDPTLV_TYPE_ASSOCIATE:
                {
                    if( 0 != vsiBOutFlt_addvsiID(vsi->B_VDPPORTNUM,vsi->vsimgrID,(struct tagVdpTlv_VDPAss *)vsi->rxCmd) )
                    {
                        EVB_LOG(EVB_LOG_ERROR, "vdp_b_sm b_proc: (port:%u,vsiid:%s) vsiBOutFlt_addvsiID failed.\n",
                             vsi->B_VDPPORTNUM,GetVsiidstr(vsi->vsiID));
                        //return ;
                        SetRespError(vsi,VDPRESPONSE_ERR_INSUFF_RESOURCE);
                        vdp_b_sm(vsi,VDP_B_EVENT_RR_DEASSOCIATE);
                    }
                }
                break;
            default:break;
            }
            
            vdp_b_sm(vsi,VDP_B_EVENT_RR_SUCCESS);
        }
        break;
    case VDP_B_RESOUCECMD_RESULT_KEEP:
        {
            vdp_b_sm(vsi,VDP_B_EVENT_RR_KEEP);
        }
        break;
    case VDP_B_RESOUCECMD_RESULT_DEASSOCIATE:
        {
            vdp_b_sm(vsi,VDP_B_EVENT_RR_DEASSOCIATE);
        }
        break;
    case VDP_B_RESOUCECMD_RESULT_TIMEOUT:
        {
            vdp_b_sm(vsi,VDP_B_EVENT_RR_TIMEOUT);
        }
        break;
    default:
        {
            EVB_LOG(EVB_LOG_ERROR, "vdp_b_sm b_proc (port:%u,vsiid:%s) resouceCmdResult (%u) is invalid.\n", 
                vsi->B_VDPPORTNUM,GetVsiidstr(vsi->vsiID),result);
            return;
        }
        break;
    }
}

INT vdp_b_build_Tlv(struct tagVsi_instance_b *vsi,BYTE * vdpTlv, UINT16 vdpTlv_len,BYTE * * outTlv,DWORD32* outLen)
{
    BYTE *pSys = NULL;
    BYTE *pSysTemp = NULL;
    DWORD32 lenTotal = 0;
    DWORD32 len = 0;

    EVB_ASSERT(vsi);
    if( NULL == vdpTlv || vdpTlv_len <= 0 )
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_b_build_Tlv: (port:%u,vsiid:%s) vdptlv (%p %u) is invalid.\n",
            vsi->B_VDPPORTNUM,GetVsiidstr(vsi->vsiID),vdpTlv,vdpTlv_len);
        return -1;
    }

    // 目前不考虑org dfn tlv，后面统一考虑;
    lenTotal =( GetVdpMgrIDTlvLen() + vdpTlv_len ); //+  GetVdpOrgDfnTlvLen());

    pSys = (BYTE *)GLUE_ALLOC(lenTotal);
    if( NULL == pSys)
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_b_build_Tlv: (port:%u,vsiid:%s)  tlv (%u) allow memory failed.\n",
            vsi->B_VDPPORTNUM,GetVsiidstr(vsi->vsiID),lenTotal);
        return -1;
    }
    MEMSET(pSys, 0, lenTotal);

    /*mgr id*/
    if( 0 != buildMgrIDTlv(vsi->vsimgrID, pSys,&len) )
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_b_build_Tlv: (port:%u,vsiid:%s) buildMgrIDTlv failed.\n",
            vsi->B_VDPPORTNUM,GetVsiidstr(vsi->vsiID));
        GLUE_FREE(pSys);
        return -1;
    }
    pSysTemp = pSys + GetVdpMgrIDTlvLen();

    /* vdp tlv */
    MEMCPY(pSysTemp,vdpTlv,vdpTlv_len );

    /*确保vdp b 发送消息是ack*/
    /* 将reason bit 7 set 1; */
    EVB_BIT_SET(*(pSysTemp+VDPTLV_HEAD_LEN), VDPASSTLV_REASON_RA); 

    pSysTemp = pSysTemp + vdpTlv_len;

    /*Org_dfn*/

    *outTlv = pSys;
    *outLen = lenTotal;

    return 0;
}

static void TxTLV_Resp(struct tagVsi_instance_b * vsi)
{
    DWORD32 tlvLen = 0;
    BYTE *  pTlv = NULL;

    if( 0 != vdp_b_build_Tlv(vsi,vsi->Resp, vsi->Resp_Len,&pTlv, &tlvLen))
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_b_sm TxTLV_Resp: (port:%u,vsiid:%s) vdp_b_build_Tlv failed.\n",
            vsi->B_VDPPORTNUM,GetVsiidstr(vsi->vsiID));
        return;
    }
    
    if( 0 != vdp_b_tlv_insert_ecplst(vsi->B_VDPPORTNUM,pTlv,tlvLen) )
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_b_sm TxTLV_Resp: (port:%u,vsiid:%s) vdp_b_tlv_insert_ecplst failed.\n",
            vsi->B_VDPPORTNUM,GetVsiidstr(vsi->vsiID));
        return;
    }

    if( GetDbgVdpPrintStatus())
    {
        PRINTF("vdp_b_sm TxTLV_Resp success - (port:%u,vsiID: %s) \n", vsi->B_VDPPORTNUM, GetVsiidstr(vsi->vsiID ));
    }

    if(GetDbgVdpPacket())
    {
        PRINTF("VDP Send  port:%u,",vsi->B_VDPPORTNUM);
        PrintLocalTimer();
        DbgMemShow(pTlv,tlvLen);
    }
}

static void TxTLV_operCmd(struct tagVsi_instance_b * vsi)
{
    DWORD32 tlvLen = 0;
    BYTE *  pTlv = NULL;

    if( NULL == vsi->operCmd || vsi->operCmd_Len <= 0)
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_b_sm TxTLV_operCmd: (port:%u,vsiid:%s) operCmd (%p %u) param invalid.\n",
          vsi->B_VDPPORTNUM, GetVsiidstr(vsi->vsiID),vsi->operCmd, vsi->operCmd_Len);
        return ;
    }

    if( 0 != vdp_b_build_Tlv(vsi,vsi->operCmd, vsi->operCmd_Len,&pTlv, &tlvLen))
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_b_sm TxTLV_Resp: (port:%u,vsiid:%s) vdp_b_build_Tlv failed.\n",
            vsi->B_VDPPORTNUM, GetVsiidstr(vsi->vsiID));
        return;
    }

    if( 0 != vdp_b_tlv_insert_ecplst(vsi->B_VDPPORTNUM,pTlv,tlvLen) )
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_b_sm TxTLV_Resp: (port:%u,vsiid:%s) vdp_b_tlv_insert_ecplst failed.\n",
            vsi->B_VDPPORTNUM, GetVsiidstr(vsi->vsiID));
        return;
    }

    if( GetDbgVdpPrintStatus())
    {
        PRINTF("vdp_b_sm TxTLV_Resp success - (port:%u,vsiID %s) \n", vsi->B_VDPPORTNUM, GetVsiidstr(vsi->vsiID ));
    }

    if(GetDbgVdpPacket())
    {
        PRINTF("VDP Send  port:%u,",vsi->B_VDPPORTNUM);
        PrintLocalTimer();
        DbgMemShow(pTlv,tlvLen);
    }
}

static INT buildDeaTlv(struct tagVsi_instance_b *vsi, BYTE * * outTlv,DWORD32* outLen)
{
    BYTE *pSys = NULL;
    BYTE *pSysTemp = NULL;
    DWORD32 lenTotal = 0;
    DWORD32 len = 0;
    struct tagVsiBOutFilter* vsiBOut = NULL; /* 从vsiBOut 获取vsi相关信息; */
    struct tagVdpTlv_VDPAss  *pVdpAss = NULL;
    UINT16 tlv_head = 0;

    if(NULL == vsi)
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_b_build_Tlv: param is NULL.\n");
        return -1;
    }

    vsiBOut = vsiBOutFlt_getVsi(vsi->B_VDPPORTNUM,vsi->vsiIDFormat,vsi->vsiID);
    if(NULL == vsiBOut)
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_b_build_Tlv: (port:%u,vsiID %s) vsiBOutFlt_getVsi failed.\n",
            vsi->B_VDPPORTNUM, GetVsiidstr(vsi->vsiID ));
        return -1;
    }

    // 目前不考虑org dfn tlv，后面统一考虑;
    // vdp ass tlv 不包括flt信息;
    lenTotal =( GetVdpMgrIDTlvLen() + VDPTLV_VDPTLV_NOFLT_LEN + VDPTLV_HEAD_LEN); //+  GetVdpOrgDfnTlvLen());

    pSys = (BYTE *)GLUE_ALLOC(lenTotal);
    if( NULL == pSys)
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_b_build_Tlv: (port:%u,vsiID %s) tlv (%u) allow memory failed.\n",
            vsi->B_VDPPORTNUM, GetVsiidstr(vsi->vsiID ),lenTotal);
        return -1;
    }
    MEMSET(pSys, 0, lenTotal);
     
    /*mgr id*/
    if( 0 != buildMgrIDTlv(vsiBOut->vsiMgrID, pSys,&len) )
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_b_build_Tlv: (port:%u,vsiID %s) buildMgrIDTlv failed.\n",
            vsi->B_VDPPORTNUM, GetVsiidstr(vsi->vsiID ));
        GLUE_FREE(pSys);
        return -1;
    }
    pSysTemp = pSys + len;

    /* vdp tlv */
    pVdpAss = (struct tagVdpTlv_VDPAss  *)pSysTemp;

    MEMCPY(pVdpAss->vsiTypeID,vsiBOut->vsitypeID, sizeof(vsiBOut->vsitypeID));
    
    /*确保vdp b 发送消息是ack*/
    /* 将reason bit 7 set 1; */
    pVdpAss->reason.ReqAck = 1;

    pVdpAss->vsiTypeVer= vsiBOut->vsitypeVer;
    pVdpAss->vsiIDFormat= vsiBOut->vsiIDFormat;
    MEMCPY(pVdpAss->vsiID,vsiBOut->vsiID, sizeof(vsiBOut->vsiID));
    pVdpAss->fltFormat =  vsiBOut->fltFormat;

    tlv_head = vdpTlvHead_HTON(VDPTLV_TYPE_DEASSOCIATE, VDPTLV_VDPTLV_NOFLT_LEN);
    MEMCPY(&pVdpAss->tlvHead,&tlv_head ,sizeof(tlv_head));
    
    /*Org_dfn*/

    *outTlv = pSys;
    *outLen = lenTotal;

    return 0;
}

static void TxTLV_buildDea(struct tagVsi_instance_b * vsi)
{
    // 
    DWORD32 tlvLen = 0;
    BYTE * pTlv = NULL;
    
    if( 0 != buildDeaTlv(vsi,&pTlv, &tlvLen))
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_b_sm TxTLV_buildDea: (port:%u,vsiID %s) buildDeaTlv failed.\n",
            vsi->B_VDPPORTNUM, GetVsiidstr(vsi->vsiID ));
        return;
    }

    if( 0 != vdp_b_tlv_insert_ecplst(vsi->S_VDPPORTNUM,pTlv,tlvLen) )
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_s_sm TxTLV_sysCmd: (port:%u,vsiID %s) vdp_s_tlv_insert_ecplst failed.\n",
            vsi->B_VDPPORTNUM, GetVsiidstr(vsi->vsiID ));
        return;
    }

    if( GetDbgVdpPrintStatus())
    {
        PRINTF("vdp_b_sm TxTLV_buildDea success - (port:%u,vsiID %s) \n", vsi->B_VDPPORTNUM, GetVsiidstr(vsi->vsiID));
    }

    if(GetDbgVdpPacket())
    {
        PRINTF("VDP Send  port:%u,",vsi->B_VDPPORTNUM);
        PrintLocalTimer();
        DbgMemShow(pTlv,tlvLen);
    }

    if( pTlv)
    {
        GLUE_FREE(pTlv);
    }
}

static void suc(struct tagVsi_instance_b * vsi)
{
    vsi->state = VDP_B_SM_SUCCESS;

    if( vsi->operCmd)
    {
        GLUE_FREE(vsi->operCmd);
        vsi->operCmd = NULL;
        vsi->operCmd_Len = 0;
    }

    if(NULL == vsi->Resp || (vsi->Resp_Len <= 0) )
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_b_sm suc: (port:%u,vsiID %s) Resp (%p %u) param invalid.\n",
            vsi->B_VDPPORTNUM, GetVsiidstr(vsi->vsiID),vsi->Resp, vsi->Resp_Len);
        return;
    }

    vsi->operCmd = (BYTE *)GLUE_ALLOC(vsi->Resp_Len);
    if( NULL == vsi->operCmd)
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_b_sm suc: (port:%u,vsiID %s) new operCmd allow memory failed.\n",
            vsi->B_VDPPORTNUM, GetVsiidstr(vsi->vsiID));
        return;
    }

    MEMCPY(vsi->operCmd, vsi->Resp, vsi->Resp_Len);
    vsi->operCmd_Len = vsi->Resp_Len;
    vsi->operCmd_TlvType = vsi->Resp_TlvType;

    if( vsi->rxCmd)
    {
        GLUE_FREE(vsi->rxCmd);      
    }
    vsi->rxCmd = NULL;
    vsi->rxCmd_Len = 0;

    TxTLV_Resp(vsi);

    vdp_b_sm(vsi, VDP_B_EVENT_UCT);
}

static void keep(struct tagVsi_instance_b * vsi)
{
    vsi->state = VDP_B_SM_KEEP;

    if( vsi->rxCmd)
    {
        GLUE_FREE(vsi->rxCmd);      
    }
    vsi->rxCmd = NULL;
    vsi->rxCmd_Len = 0;

    TxTLV_Resp(vsi);
    vdp_b_sm(vsi, VDP_B_EVENT_UCT);
}

static void deassoc(struct tagVsi_instance_b * vsi)
{
    vsi->state = VDP_B_SM_DEASSOC;

    if( vsi->rxCmd)
    {
        GLUE_FREE(vsi->rxCmd);      
    }
    vsi->rxCmd = NULL;
    vsi->rxCmd_Len = 0;

    TxTLV_Resp(vsi);
    vdp_b_sm(vsi, VDP_B_EVENT_UCT);
}

static void deassoced(struct tagVsi_instance_b * vsi)
{
    vsi->state = VDP_B_SM_DEASSOCIATED;

    TxTLV_buildDea(vsi);
    vdp_b_sm(vsi, VDP_B_EVENT_UCT);
}

static void wt_s_cmd(struct tagVsi_instance_b * vsi)
{
    vsi->state = VDP_B_SM_WAIT_STATION_CMD;

    vsi->vsiState = vsi->operCmd_TlvType;
    vsi->waitWhile = vsi->toutKeepAlive; // VDP_B_TOUT_KEEPALIVE_DEFAULT;  
}

static void proc_k(struct tagVsi_instance_b * vsi)
{
    vsi->state = VDP_B_SM_PROCESS_KEEPALIVE;

    if( vsi->rxCmd)
    {
        GLUE_FREE(vsi->rxCmd);
    }
    vsi->rxCmd = NULL;
    vsi->rxCmd_Len = 0;
    TxTLV_operCmd(vsi);
    vdp_b_sm(vsi, VDP_B_EVENT_UCT);
}

/* bridge sm */
void (* vdp_b_actions[NUM_VDP_B_SM_EVENTS][NUM_VDP_B_SM_STATES]) (struct tagVsi_instance_b * vsi) =
{
    /*
    * event/state         IDLE   INIT   B_PROC     SUCCESS  KEEP  DEASSOC DEASSOCIATED RELAEASE WAIT_S_CMD PROC_K
    */
    /* BEGIN  */         {init,  b_err, b_err,     b_err,  b_err, b_err,  b_err,       b_err,   b_err,     b_err, },
    /* B_EXIT  */        {b_err, rel_all,deassoced,b_err,  b_err, b_err,  b_err,       b_err,   b_err,     b_err, },
    /* RXCMD_VALID*/     {b_err, b_proc, b_err,    b_err,  b_err, b_err,  b_err,       b_err,   b_err,     b_err, },
    /* RR_SUCCESS*/      {b_err, b_err, suc,       b_err,  b_err, b_err,  b_err,       b_err,   b_err,     b_err, },
    /* RR_KEEP */        {b_err, b_err, keep,      b_err,  b_err, b_err,  b_err,       b_err,   b_err,     b_err, },
    /* RR_DEASSOCIATE */ {b_err, b_err, deassoc,   b_err,  b_err, b_err,  b_err,       b_err,   b_err,     b_err, },
    /* RR_TIMEOUT */     {b_err, b_err, deassoced, b_err,  b_err, b_err,  b_err,       b_err,   b_err,     b_err, },
    /* UCT*/             {b_err, b_err, b_err,     wt_s_cmd,wt_s_cmd,rel_all,rel_all,  b_end,   b_err,     wt_s_cmd,},
    /* RXCMD!=OPERCMD */ {b_err, b_err, b_err,     b_err,  b_err, b_err,  b_err,       b_err,   b_proc,    b_err, },
    /* RXCMD==OPERCMD */ {b_err, b_err, b_err,     b_err,  b_err, b_err,  b_err,       b_err,   proc_k,    b_err, },
    /* WAITWHILE==0 */   {b_err, b_err, b_err,     b_err,  b_err, b_err,  b_err,       b_err,   deassoced, b_err, },
    /* PORT_UP */        {b_err, b_err, b_err,     b_err,  b_err, b_err,  b_err,       b_err,   b_err,     b_err, },
    /* PORT_DOWN*/       {b_err, b_err, b_err,     b_err,  b_err, b_err,  b_err,       b_err,   b_err,     b_err, },
    /* EVBTLV_NO_SUPP*/  {rel_all,rel_all,rel_all,rel_all, rel_all,rel_all,rel_all,   rel_all,  rel_all,   rel_all,},
};

void vdp_b_sm(struct tagVsi_instance_b * vsi, VDP_B_SM_EVENT event)
{
    if( ( NULL == vsi) || !vdp_b_sm_event_valid(event) ) 
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_b_sm: param[%p %d] invalid.\n",  vsi,event);
        return;
    }
    
    if(!vdp_b_sm_state_valid(vsi->state) )
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_b_sm: (port:%u,vsiID %s) state[%d] invalid.\n", 
             vsi->B_VDPPORTNUM, GetVsiidstr(vsi->vsiID),vsi->state);
        return;
    }
        
    if( GetDbgVdpEvent())
    {
        PRINTF("vdp_b_sm event- (port:%u,vsiID %s) [%s] [%s] [%s]\n", vsi->B_VDPPORTNUM,GetVsiidstr(vsi->vsiID),GetVdpTlvTypeString(vsi->vsiState),vdp_b_sm_state[vsi->state],vdp_b_sm_event[event] );
    }

    (*(vdp_b_actions[event][vsi->state])) (vsi);

    if( GetDbgVdpEvent())
    {
        if(!vdp_b_sm_state_valid(vsi->state) )
        {
            PRINTF("vdp_b_sm event: (port:%u,vsiID %s) state[%d] invalid.\n", vsi->B_VDPPORTNUM,GetVsiidstr(vsi->vsiID), vsi->state);
        }
        else
        {
            PRINTF(" vdp_b_sm event: (port:%u,vsiID %s),current state - [%s] [%s] \n",vsi->B_VDPPORTNUM,GetVsiidstr(vsi->vsiID),GetVdpTlvTypeString(vsi->vsiState), vdp_b_sm_state[vsi->state] );
        }
    }

    return;
}

static INT vdp_b_vsi_cmp_vdptlv(struct tagVsi_instance_b * vsi,struct tagVdpTlv_VDPAss* tlv )
{
    INT ret = 0;
    if( vsi == NULL || tlv == NULL)
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_b_vsi_cmp_vdpasstlv: param is null.\n");
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

static void vdp_b_vdptlv_to_vsi( struct tagVdpTlv_VSIMgrID * mgr,struct tagVdpTlv_VDPAss * vdpass, struct tagVsi_instance_b * vsi)
{    
    EVB_ASSERT(mgr);
    EVB_ASSERT(vdpass);
    EVB_ASSERT(vsi);
    if( NULL == mgr || NULL == vdpass || NULL == vsi)
    {
        return;
    }

    MEMCPY(vsi->vsimgrID, mgr->mgr_id, sizeof(mgr->mgr_id));
    vsi->vsiIDFormat = vdpass->vsiIDFormat;
    MEMCPY(vsi->vsiID, vdpass->vsiID, sizeof(vdpass->vsiID));
}

void vdp_b_rx_ReceiveFrame(struct tagVdp_b * vdp,u32 local_port, u8* buf, u16 len)
{
    struct tagVsi_instance_b * pVsi = NULL;
    struct tagVsi_instance_b * vsiNew = NULL;
    struct tagVdpTlv_head tlvHead;
    struct tagVdpTlv_VSIMgrID mgrID;    
    struct tagVdpTlv_VDPAss * vdpAss = NULL;
    BYTE   *pTmpVdpAss = NULL;
    struct tagVdpTlv_OrgDfn  OrgDfn;
    BYTE * pTmp = NULL;
    UINT16 _len = 0;
    BYTE  bExist = FALSE;
    struct tagEvbPort* port = NULL;

    port = GetEvbPort(local_port);
    if( NULL == port)
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_b_rx_ReceiveFrame: param is invalid (port:%u).\n",local_port);
        return ;
    }

    if( vdp == NULL ||  buf == NULL || len < (VDPTLV_VSIMGRID_LEN + VDPTLV_HEAD_LEN) )
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_b_rx_ReceiveFrame: param is invalid (vdp:%p,buf:%p, len:%u).\n",vdp,buf, len);
        return ;
    }
    
    MEMSET(&tlvHead, 0, sizeof(tlvHead) );
    MEMSET(&mgrID, 0 , sizeof(mgrID));
    MEMSET(&OrgDfn, 0 , sizeof(OrgDfn));

    pTmp = buf;
    _len = len;

    MEMCPY(&mgrID, pTmp, VDPTLV_VSIMGRID_LEN + VDPTLV_HEAD_LEN);
    vdpTlvHead_NTOH(&mgrID.tlvHead);

    // check len valid
    if(mgrID.tlvHead.tlvType != VDPTLV_TYPE_VSI_MGRID)
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_rx_ReceiveFrame: (port:%u) first tlv (%u) is not VSI_MGRID.\n",local_port,mgrID.tlvHead.tlvType );
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
            EVB_LOG(EVB_LOG_ERROR, "vdp_b_rx_ReceiveFrame: tlv packet is invalid (port:%u).\n",local_port );
            return ;
        }
        switch(tlvHead.tlvType)
        {
        case VDPTLV_TYPE_PRE_ASSOCIATE:
        case VDPTLV_TYPE_PRE_ASSOCIATE_WITH_RR:
        case VDPTLV_TYPE_ASSOCIATE:
        case VDPTLV_TYPE_DEASSOCIATE:
            {           
                if((UINT32)(tlvHead.tlvLen+VDPTLV_HEAD_LEN) < sizeof(struct tagVdpTlv_VDPAss))
                {
                    EVB_LOG(EVB_LOG_ERROR, "vdp_b_rx_ReceiveFrame: tlv packet < VdpAss struct is invalid (port:%u).\n",local_port );
                    break;
                }
#if 1
                vdpAss = (struct tagVdpTlv_VDPAss *)GLUE_ALLOC(VDPTLV_HEAD_LEN+tlvHead.tlvLen);
                pTmpVdpAss = (BYTE *)vdpAss;
                if( NULL == pTmpVdpAss)
                {
                    EVB_LOG(EVB_LOG_ERROR, "vdp_b_rx_ReceiveFrame: new tmp vdpAss alloc memory failed (port:%u).\n",local_port );
                    break;
                }

                MEMCPY(vdpAss, pTmp,VDPTLV_HEAD_LEN+tlvHead.tlvLen ); 
                
                /*if not req, continue;*/
                if(EVB_BIT_TEST(vdpAss->reason.ReqAck,VDPASSTLV_REASON_ACK))
                {
                    EVB_LOG(EVB_LOG_ERROR, "vdp_b_rx_ReceiveFrame: (port:%u) vdpAss status ReqAck(%u) is not Ack.\n",local_port,vdpAss->reason.ReqAck );
                    break;
                }
                //vdpAss = (struct tagVdpTlv_VDPAss* )pTmpVdpAss;
#endif 
                // MEMCPY(&pTmpVdpAss, pTmp,VDPTLV_HEAD_LEN+tlvHead.tlvLen );
                vdpTlvHead_NTOH(&vdpAss->tlvHead);

                LIST_FOREACH(pVsi, &vdp->lstVsi,lstEntry)
                {
                    if( 0 == vdp_b_vsi_cmp_vdptlv(pVsi, vdpAss) )
                    {
                        bExist = TRUE;
                        break;
                    }
                }
#if 1
                if(bExist)
                {
                    vdp_b_vdptlv_to_vsi(&mgrID,vdpAss, pVsi);
                    pVsi->vdp = port->vdp_b;

                    // 仅仅在wait_station_cmd proc_keepalive 
                    // 后面需要考虑如果用户快速DEACCOS 如何处理，如果不在上面两个状态;

                    if( pVsi->rxCmd)
                    {
                        GLUE_FREE(pVsi->rxCmd);
                        pVsi->rxCmd = NULL;
                        pVsi->rxCmd_Len = 0;
                    }
                    pVsi->rxCmd = (BYTE *)GLUE_ALLOC(VDPTLV_HEAD_LEN+tlvHead.tlvLen);
                    if( pVsi->rxCmd == NULL)
                    {
                        EVB_LOG(EVB_LOG_ERROR, "vdp_b_rx_ReceiveFrame vsi_b:alloc memory failed(port:%u,vsiid:%s).\n",
                            pVsi->B_VDPPORTNUM,GetVsiidstr(pVsi->vsiID));

                        GLUE_FREE(pTmpVdpAss);
                        //pTmpVdpAss = NULL;
                        //vdpAss = NULL;
                        return;
                    }
                    MEMCPY(pVsi->rxCmd,pTmp,VDPTLV_HEAD_LEN+tlvHead.tlvLen );
                    pVsi->rxCmd_Len = VDPTLV_HEAD_LEN+tlvHead.tlvLen;
                    pVsi->rxCmd_TlvType = tlvHead.tlvType;
                    /*if(pVsi->state != VDP_B_SM_IDLE &&  pVsi->state != VDP_B_SM_INIT)
                    {
                        pVsi->state = VDP_B_SM_BRIDGE_PROC;
                    }*/
                    // 是否应该立刻驱动vdp_b_sm;
                }
                else
                {
                    vsiNew = (struct tagVsi_instance_b *)GLUE_ALLOC(sizeof(struct tagVsi_instance_b));
                    if( vsiNew == NULL)
                    {
                        EVB_LOG(EVB_LOG_ERROR, "vdp_b_rx_ReceiveFrame vsi_b:alloc memory failed (port:%u).\n",local_port);
                        GLUE_FREE(pTmpVdpAss);
                        //pTmpVdpAss = NULL;
                        //vdpAss = NULL;
                        return;
                    }
                    MEMSET(vsiNew, 0, sizeof(struct tagVsi_instance_b ));
                    
                    vdp_b_vdptlv_to_vsi(&mgrID,vdpAss, vsiNew);
                    vsiNew->vdp = port->vdp_b;

                    initDftVsiB(vsiNew);

                    vsiNew->rxCmd = NULL;
                    vsiNew->rxCmd = (BYTE *)GLUE_ALLOC(VDPTLV_HEAD_LEN+tlvHead.tlvLen);
                    if( vsiNew->rxCmd == NULL)
                    {
                        EVB_LOG(EVB_LOG_ERROR, "vdp_b_rx_ReceiveFrame vsi_b:alloc memory failed (port:%u,vsiid:%s).\n",
                            pVsi->B_VDPPORTNUM,GetVsiidstr(pVsi->vsiID));
                        GLUE_FREE(pTmpVdpAss);
                        //pTmpVdpAss = NULL;
                        //vdpAss = NULL;
                        return;
                    }
                    MEMCPY(vsiNew->rxCmd,pTmp,VDPTLV_HEAD_LEN+tlvHead.tlvLen );
                    vsiNew->rxCmd_Len = VDPTLV_HEAD_LEN+tlvHead.tlvLen;
                    vsiNew->rxCmd_TlvType = tlvHead.tlvType;

                    LIST_FOREACH(pVsi, &vdp->lstVsi,lstEntry)
                    {                       
                    }
                    if(NULL == pVsi)
                    {
                        LIST_INSERT_HEAD(&vdp->lstVsi, vsiNew, lstEntry);
                    }
                    else
                    {
                        LIST_INSERT_AFTER(pVsi,vsiNew,lstEntry);
                    }

                    // 是否应该立刻驱动vdp_b_sm;
                    // vdp_b_sm(vsiNew,VDP_B_EVENT_BEGIN);
                }
#endif 
                GLUE_FREE(vdpAss);
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

static INT vdp_b_cmp_operCmd_rxCmd_exceptZero(struct tagVsi_instance_b * vsi)
{
    BYTE * pOperCmdtmp = NULL;
    BYTE * pRxCmdtmp = NULL;    
    BYTE * pRxCmdReason = NULL;
    if( NULL == vsi)
    {
        return -1;
    }

    if( NULL == vsi->rxCmd || NULL == vsi->operCmd )
    {
        EVB_LOG(EVB_LOG_ERROR, "vdp_b_cmp_operCmd_rxCmd_exceptZero (port:%u,vsi:%s) (rxCmd:%p,operCmd:%p) is NULL.\n",
            vsi->B_VDPPORTNUM,GetVsiidstr(vsi->vsiID),vsi->rxCmd,vsi->operCmd);
        return -1;
    }

    // 目前直接MEMCPY进行判断,需将reason中5~8位忽略; 
    if(vsi->rxCmd_Len > VDPTLV_HEAD_LEN )
    {
        if( 0 != MEMCMP(vsi->operCmd, vsi->rxCmd,VDPTLV_HEAD_LEN) )
        {
            return -1;
        }

        pOperCmdtmp = vsi->operCmd + VDPTLV_HEAD_LEN + 1;
        pRxCmdtmp = vsi->rxCmd + VDPTLV_HEAD_LEN + 1;        
        if( 0 != MEMCMP(pOperCmdtmp, pRxCmdtmp,(vsi->rxCmd_Len - VDPTLV_HEAD_LEN -1)) )
        {
            return -1;
        }

        pRxCmdReason = vsi->rxCmd + VDPTLV_HEAD_LEN;
        if( (*pRxCmdReason) & 0xf0 )
        {
            return -1;
        }

    }
    else
    {
        if( 0 != MEMCMP(vsi->operCmd, vsi->rxCmd,vsi->rxCmd_Len) )
        {
            return -1;
        }
    }

    return 0;
}

static void vdp_b_can_destory_vsi(struct tagVdp_b * vdp)
{
    struct tagVsi_instance_b * pVsi = NULL;
    struct tagVsi_instance_b * pVsiTmp = NULL;

    if( NULL == vdp)
    {
        return;
    }

    pVsi = LIST_FIRST(&vdp->lstVsi);
    while(NULL != pVsi)
    {
        if(VDP_B_SM_RELEASE_ALL == pVsi->state)
        {
            pVsiTmp = pVsi;
            pVsi = LIST_NEXT(pVsi,lstEntry);

            LIST_REMOVE(pVsiTmp,lstEntry);

            // free memory
            if(pVsiTmp->operCmd)
            {
                GLUE_FREE(pVsiTmp->operCmd);
            }
            if(pVsiTmp->rxCmd)
            {
                GLUE_FREE(pVsiTmp->rxCmd);
            }
            if(pVsiTmp->Resp)
            {
                GLUE_FREE(pVsiTmp->Resp);
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

void vdp_b_loop_timer_handler(struct tagVdp_b * vdp)
{
    // 遍历队列  进行处理; 
    struct tagVsi_instance_b * pVsi = NULL;
    //struct tagVsi_instance_b * pVsiTmp = NULL;

    if( NULL == vdp)
    {
        return;
    }
    
    // EVB TLV NO SUPPORT 在sm中实现;
    vdp->evbtlvStatus = vdp_GetEvbTlvNegResult(vdp->PORT_NUM);
        
    // vsi timer
    LIST_FOREACH(pVsi, &vdp->lstVsi,lstEntry)
    {
        if( EVB_TLV_STATUS_SUCC != vdp->evbtlvStatus)
        {
            vdp_b_sm(pVsi,VDP_B_EVENT_EVBTLV_NO_SUPPORT);
            continue;
        }

        if(VDP_B_SM_IDLE == pVsi->state )
        {
            vdp_b_sm(pVsi,VDP_B_EVENT_BEGIN);
            continue;
        }

        if(VDP_B_SM_INIT == pVsi->state)
        {
            if( pVsi->bridgeExit)
            {
                vdp_b_sm(pVsi,VDP_B_EVENT_BRIDGE_EXIT);
            }
            else
            {
                if( pVsi->rxCmd)
                {
                    vdp_b_sm(pVsi,VDP_B_EVENT_RXCMD_VALID);
                }
            }
            continue;
        }
        if(VDP_B_SM_BRIDGE_PROC == pVsi->state)
        {
            if( pVsi->bridgeExit)
            {
                vdp_b_sm(pVsi,VDP_B_EVENT_BRIDGE_EXIT);
            }
            continue;
        }
        
        if( pVsi->waitWhile > VDP_LOOP_TIMER_INTREVAL )
        {
            pVsi->waitWhile -= VDP_LOOP_TIMER_INTREVAL;
        }
        else
        {
            pVsi->waitWhile = 0;
        }

        if( VDP_B_SM_WAIT_STATION_CMD == pVsi->state )
        {
            if( pVsi->bridgeExit)
            {
                vdp_b_sm(pVsi,VDP_B_EVENT_BRIDGE_EXIT);
                continue;
            }

            if( 0 == pVsi->waitWhile)
            {
                vdp_b_sm(pVsi,VDP_B_EVENT_WAITWHILE_EQU_ZERO);
                continue;
            }
            
            if( NULL != pVsi->rxCmd )
            {
                if(0 == vdp_b_cmp_operCmd_rxCmd_exceptZero(pVsi))
                {
                    vdp_b_sm(pVsi, VDP_B_EVENT_RXCMD_EQU_OPERCMD);
                    continue;
                }   
                else
                {
                    if( pVsi->rxCmd)
                    {
                        vdp_b_sm(pVsi, VDP_B_EVENT_RXCMD_NOTEQU_OPERCMD);
                        continue;
                    }
                }
            }
        }
        
    }

    // 释放资源;
    vdp_b_can_destory_vsi(vdp);
}

INT vdp_b_sm_state_valid(UINT32 state)
{
    if(((state) >= VDP_B_SM_IDLE) && ((state) <= VDP_B_SM_PROCESS_KEEPALIVE))
    {
        return TRUE;
    }
    return FALSE;
}

#ifdef __cplusplus
}
#endif 

