
#include "vdp.h"
#include "vsi_b_outFilter.h"

#ifdef __cplusplus
extern "C"{
#endif 

struct tagVsiBOutFilter_head g_lstVsiBOutFlt;

static int32 vsiBOutFlt_Notify_MacVlan(u32 port,struct tagVDP_FLT_GROUPMACVID* pGMVIn,BYTE opt)
{
    // EVB_ISS_MAC_ADD/EVB_ISS_MAC_DEL
    if( EVB_ISS_MAC_ADD != opt && EVB_ISS_MAC_DEL != opt)
    {
        EVB_LOG(EVB_LOG_ERROR, "vsiBOutFlt_Notify_MacVlan: (port:%u) opt(%u) is invalid.\n",port,opt);
        return -1;
    }
    if( NULL == pGMVIn )
    {
        EVB_LOG(EVB_LOG_ERROR, "vsiBOutFlt_Notify_MacVlan: param is NULL.\n");
        return -1;
    }
    /* group id 暂不处理 */
    if( 0 != evb_SetMacVlan(port,pGMVIn->mac,pGMVIn->vid,opt) )
    {
        EVB_LOG(EVB_LOG_ERROR, "vsiBOutFlt_Notify_MacVlan: evb_SetMacVlan(port:%u,opt:%u,%x:%x:%x:%x:%x:%x,%u,%u,%u) failed.\n",\
            port,opt,pGMVIn->mac[0],pGMVIn->mac[1],pGMVIn->mac[2],pGMVIn->mac[3],pGMVIn->mac[4],pGMVIn->mac[5],port,pGMVIn->vid);
        return -1;
    }

    EVB_LOG(EVB_LOG_INFO, "vsiBOutFlt_Notify_MacVlan: evb_SetMacVlan(port:%u,opt:%u,%x:%x:%x:%x:%x:%x,%u) success.\n",\
            port,opt,pGMVIn->mac[0],pGMVIn->mac[1],pGMVIn->mac[2],pGMVIn->mac[3],pGMVIn->mac[4],pGMVIn->mac[5],pGMVIn->vid);

    return 0;
}

void vsiBOutFlt_init()
{
    LIST_INIT(&g_lstVsiBOutFlt);
}

void vsiBOutFlt_shutdown()
{
    struct tagVsiBOutFilter* pOutFlt = NULL; 
    struct tagVsiGMVInfo *pGmv = NULL;
    while(NULL != (pOutFlt = LIST_FIRST(&g_lstVsiBOutFlt)))
    {
        LIST_REMOVE(pOutFlt,lstEntry);

        while(NULL != (pGmv = LIST_FIRST(&pOutFlt->lstFlt) ))
        {
            LIST_REMOVE(pGmv,lstEntry);

            // DEL
            vsiBOutFlt_Notify_MacVlan(pOutFlt->port,&pGmv->gmv,EVB_ISS_MAC_DEL);

            GLUE_FREE(pGmv);
            pGmv = NULL;
        }

        GLUE_FREE(pOutFlt);
        pOutFlt = NULL;
    }
    LIST_INIT(&g_lstVsiBOutFlt);
}

static int32 vsiBOutFlt_addfltGMV(struct tagVsiBOutFilter * pVsi,struct tagVDP_FLT_GROUPMACVID* pGMVIn )
{
    struct tagVsiGMVInfo* pGMV = NULL;
    struct tagVsiGMVInfo* pGMVNew = NULL;
    if( NULL == pVsi || NULL == pGMVIn )
    {
        EVB_LOG(EVB_LOG_ERROR, "vsiProfile_addflt: param is NULL.\n");
        return -1;
    }

    LIST_FOREACH(pGMV,&pVsi->lstFlt,lstEntry)
    {
        // 直接比较，以后考虑PSP之类的;
        //if( 0 == MEMCMP(&pGMV->gmv,pGMVIn,sizeof(struct tagVDP_FLT_GROUPMACVID)) )
        if( !MEMCMP(pGMV->gmv.groupID,pGMVIn->groupID,EVB_VDP_GROUPID_LEN) && \
                !MEMCMP(pGMV->gmv.mac,pGMVIn->mac,EVB_VDP_MAC_LEN) && \
                (pGMV->gmv.vid == pGMVIn->vid)  )
        {
            return 0;
        }
    }

    pGMVNew = (struct tagVsiGMVInfo *)GLUE_ALLOC(sizeof(struct tagVsiGMVInfo));
    if( NULL == pGMVNew)
    {
        EVB_LOG(EVB_LOG_ERROR, "vsiProfile_addflt:(port:%u,vsiid:%s) alloc memory failed.\n",pVsi->port,GetVsiidstr(pVsi->vsiID));
        return -1;
    }
    MEMCPY(&pGMVNew->gmv,pGMVIn,sizeof(pGMVNew->gmv));

    if(NULL == pGMV)
    {
        LIST_INSERT_HEAD(&pVsi->lstFlt, pGMVNew, lstEntry);
    }
    else
    {
        LIST_INSERT_AFTER(pGMV,pGMVNew,lstEntry);
    }

    // ADD
    if( 0 != vsiBOutFlt_Notify_MacVlan(pVsi->port,&pGMVNew->gmv,EVB_ISS_MAC_ADD) )
    {
        EVB_LOG(EVB_LOG_ERROR, "vsiBOutFlt_addfltGMV: vsiBOutFlt_Notify_MacVlan(port:%u,vsi:%s)  failed.\n",
            pVsi->port,GetVsiidstr(pVsi->vsiID));
        return -1;
    }
    return 0;
}

static int32 vsiBOutFlt_addGMVtoLstFlt(struct tagVsiGMVInfo_head * OutLstFlt,struct tagVDP_FLT_GROUPMACVID* pGMVIn)
{
    struct tagVsiGMVInfo* pGMV = NULL;
    struct tagVsiGMVInfo* pGMVNew = NULL;
    if(NULL == OutLstFlt || NULL == pGMVIn)
    {
        EVB_LOG(EVB_LOG_ERROR, "vsiBOutFlt_addGMVtoLstFlt: param is NULL.\n");
        return -1;
    }
    LIST_FOREACH(pGMV,OutLstFlt,lstEntry)
    {
        // 直接比较，以后考虑PSP之类的;
        if( 0 == MEMCMP(&pGMV->gmv,pGMVIn,sizeof(struct tagVDP_FLT_GROUPMACVID)) )
        {
            return 0;
        }
    }

    pGMVNew = (struct tagVsiGMVInfo *)GLUE_ALLOC(sizeof(struct tagVsiGMVInfo));
    if( NULL == pGMVNew)
    {
        EVB_LOG(EVB_LOG_ERROR, "vsiProfile_addflt: alloc memory failed.\n");
        return -1;
    }
    MEMCPY(&pGMVNew->gmv,pGMVIn,sizeof(pGMVNew->gmv));

    if(NULL == pGMV)
    {
        LIST_INSERT_HEAD(OutLstFlt, pGMVNew, lstEntry);
    }
    else
    {
        LIST_INSERT_AFTER(pGMV,pGMVNew,lstEntry);
    }

    return 0;
}

static int32 vsiBOutFlt_getVdpassLstFlt(struct tagVdpTlv_VDPAss * vdpass,struct tagVsiGMVInfo_head * OutLstFlt)
{
    struct tagVdpTlv_head  tlvHead;
    //UINT16 len_tlv = 0;
    struct tagVDP_FLT_GROUPMACVID * pTmpGMV = NULL;
    struct tagVDP_FLT_MACVID * pTmpMV = NULL;
    struct tagVDP_FLT_GROUPVID * pTmpGV = NULL;
    struct tagVDP_FLT_VID * pTmpV = NULL;
    BYTE * pFltInfo = NULL;
    UINT16 nFltNum = 0;
    struct tagVDP_FLT_GROUPMACVID GMV;
    BYTE  * pCpy = NULL;

    if(NULL == vdpass || NULL == OutLstFlt)
    {
        EVB_LOG(EVB_LOG_ERROR, "vsiBOutFlt_getVdpassLstFlt: param is NULL.\n");
        return -1;
    }
    MEMCPY(&tlvHead, &vdpass->tlvHead,sizeof(tlvHead));

    vdpTlvHead_NTOH(&tlvHead);
    if( tlvHead.tlvLen < VDPTLV_VDPTLV_NOFLT_LEN)
    {
        EVB_LOG(EVB_LOG_ERROR, "vsiBOutFlt_getVdpassLstFlt: tlvHead len(%u) is invalid.\n",tlvHead.tlvLen);
        return -1;
    }
    tlvHead.tlvLen -= VDPTLV_VDPTLV_NOFLT_LEN;
    pFltInfo = vdpass->fltInfo;

    MEMCPY(&nFltNum,pFltInfo,sizeof(nFltNum));
    nFltNum = EVB_NTOHS(nFltNum);

    pFltInfo += sizeof(nFltNum);
    tlvHead.tlvLen -= sizeof(nFltNum);

    while(tlvHead.tlvLen > 0 && nFltNum > 0)
    {
        MEMSET(&GMV,0,sizeof(GMV));

        switch(vdpass->fltFormat)
        {
        case VDP_FLTINFO_FORMAT_VID:
            {
                pTmpV = (struct tagVDP_FLT_VID *)pFltInfo;
                pCpy = (BYTE *)&GMV;
                pCpy += EVB_VDP_GROUPID_LEN;
                pCpy += EVB_VDP_MAC_LEN;
                MEMCPY(pCpy,pTmpV,sizeof(*pTmpV));
                vdpfltGROUPMACVID_ntohs(&GMV);		

                //vsiBOutFlt_addfltGMV(pVsi,&GMV);
                vsiBOutFlt_addGMVtoLstFlt(OutLstFlt,&GMV);

                if( tlvHead.tlvLen <= sizeof(*pTmpV))
                {
                    return 0;
                }
                tlvHead.tlvLen -= sizeof(*pTmpV);
                nFltNum--;
                pFltInfo += sizeof(*pTmpV);
            }
            break;
        case VDP_FLTINFO_FORMAT_MACVID:
            {
                pTmpMV = (struct tagVDP_FLT_MACVID *)pFltInfo;
                pCpy = (BYTE *)&GMV;
                pCpy += EVB_VDP_GROUPID_LEN;
                MEMCPY(pCpy,pTmpMV,sizeof(*pTmpMV));
                vdpfltGROUPMACVID_ntohs(&GMV);		
                vsiBOutFlt_addGMVtoLstFlt(OutLstFlt,&GMV);
                //vsiBOutFlt_addfltGMV(pVsi,&GMV);

                if( tlvHead.tlvLen <= sizeof(*pTmpMV))
                {
                    return 0;
                }
                tlvHead.tlvLen -= sizeof(*pTmpMV);
                nFltNum--;
                pFltInfo += sizeof(*pTmpMV);
            }
            break;
        case VDP_FLTINFO_FORMAT_GROUPVID:
            {
                pTmpGV = (struct tagVDP_FLT_GROUPVID *)pFltInfo;
                pCpy = (BYTE *)&pTmpGV;
                pCpy += EVB_VDP_MAC_LEN;
                MEMCPY(pCpy,pTmpGV,sizeof(*pTmpGV));
                vdpfltGROUPMACVID_ntohs(&GMV);	
                vsiBOutFlt_addGMVtoLstFlt(OutLstFlt,&GMV);
                //vsiBOutFlt_addfltGMV(pVsi,&GMV);

                if( tlvHead.tlvLen <= sizeof(*pTmpGV))
                {
                    return 0;
                }
                tlvHead.tlvLen -= sizeof(*pTmpGV);
                nFltNum--;
                pFltInfo += sizeof(*pTmpGV);
            }
            break;
        case VDP_FLTINFO_FORMAT_MACGROUPVID:
            {
                pTmpGMV = (struct tagVDP_FLT_GROUPMACVID *)pFltInfo;
                pCpy = (BYTE *)&GMV;
                MEMCPY(pCpy,pTmpGMV,sizeof(*pTmpGMV));
                vdpfltGROUPMACVID_ntohs(&GMV);		
                vsiBOutFlt_addGMVtoLstFlt(OutLstFlt,&GMV);
                //vsiBOutFlt_addfltGMV(pVsi,&GMV);

                if( tlvHead.tlvLen <= sizeof(*pTmpGMV))
                {
                    return 0;
                }
                tlvHead.tlvLen -= sizeof(*pTmpGMV);
                nFltNum--;
                pFltInfo += sizeof(*pTmpGMV);
            }
            break;
        default:
            {
                // 不认识的filter info,直接退出;
                EVB_LOG(EVB_LOG_ERROR, "vsiBOutFlt_getVdpassLstFlt: vdpass->fltFormat(%u) is invalid.\n",vdpass->fltFormat);
                tlvHead.tlvLen = 0;
            }
            break;
        }
    }

    return 0;
}

static int32 vsiBOutFlt_cmpdelLstFlt(struct tagVsiBOutFilter * pVsi,struct tagVsiGMVInfo_head * LstFlt)
{
    struct tagVsiGMVInfo * pGMVVsi = NULL;
    struct tagVsiGMVInfo * pGMVVsiTmp = NULL;
    struct tagVsiGMVInfo * pGMV = NULL;
    BYTE bFind = FALSE;

    if(NULL == pVsi || NULL == LstFlt)
    {
        EVB_LOG(EVB_LOG_ERROR, "vsiBOutFlt_delBOutFltInfo: param is NULL.\n");
        return -1;
    }

    pGMVVsi = LIST_FIRST(&pVsi->lstFlt);
    while(NULL != pGMVVsi )
    {		
        bFind = FALSE;
        LIST_FOREACH(pGMV,LstFlt,lstEntry)
        {
            // 直接比较，以后考虑PSP之类的;
            if( !MEMCMP(pGMV->gmv.groupID,pGMVVsi->gmv.groupID,EVB_VDP_GROUPID_LEN) && \
                !MEMCMP(pGMV->gmv.mac,pGMVVsi->gmv.mac,EVB_VDP_MAC_LEN) && \
                (pGMV->gmv.vid == pGMVVsi->gmv.vid)  )
            {
                bFind = TRUE;
                break;
            }
        }
                
        if( FALSE == bFind)
        {
            pGMVVsiTmp = pGMVVsi;
            pGMVVsi = LIST_NEXT(pGMVVsi,lstEntry);
            LIST_REMOVE(pGMVVsiTmp,lstEntry);
            
            // delete GMV 
            vsiBOutFlt_Notify_MacVlan(pVsi->port,&pGMVVsiTmp->gmv,EVB_ISS_MAC_DEL);

            GLUE_FREE(pGMVVsiTmp);			
        }
        else
        {
            pGMVVsi = LIST_NEXT(pGMVVsi,lstEntry);
        }
    }
    return 0;
}

static int32 vsiBOutFlt_addLstFlt(struct tagVsiBOutFilter * pVsi,struct tagVsiGMVInfo_head * LstFlt)
{
    struct tagVsiGMVInfo * pGMV = NULL;

    if(NULL == pVsi || NULL == LstFlt)
    {
        EVB_LOG(EVB_LOG_ERROR, "vsiBOutFlt_addBOutFltInfo: param is NULL.\n");
        return -1;
    }
    LIST_FOREACH(pGMV,LstFlt,lstEntry)
    {
        // 直接比较，以后考虑PSP之类的;
        if( 0 != vsiBOutFlt_addfltGMV(pVsi,&pGMV->gmv) )
        {
            EVB_LOG(EVB_LOG_ERROR, "vsiBOutFlt_addLstFlt: vsiBOutFlt_addfltGMV failed(port:%u,vsiid:%s,vlan:%u,mac:%2x:%2x%2x:%2x%2x:%2x.\n",
               pVsi->port,GetVsiidstr(pVsi->vsiID),pGMV->gmv.vid,pGMV->gmv.mac[0],pGMV->gmv.mac[1],pGMV->gmv.mac[2],pGMV->gmv.mac[3],pGMV->gmv.mac[4],pGMV->gmv.mac[5]);
            return -1;
        }
    }

    return 0;
}

/* Bridge 获取的信息从station处来，存在flt变化的情况; */
int32 vsiBOutFlt_addvsiID(u32 port,BYTE mgrid[VDPTLV_VSIMGRID_LEN],struct tagVdpTlv_VDPAss * vdpass)
{
    struct tagVsiBOutFilter * pVsi = NULL;
    struct tagVsiBOutFilter * pVsiNew = NULL;
    struct tagVsiGMVInfo * pTmpGMV = NULL;
    BYTE bFind = FALSE;
    struct tagVdpTlv_head tlv_head;
    struct tagVsiGMVInfo_head tmpGMV_lstFlt;

    LIST_INIT(&tmpGMV_lstFlt);

    if(NULL == vdpass )
    {
        EVB_LOG(EVB_LOG_ERROR, "vsiBOutFlt_addvsiID: (port:%u) param is NULL.\n",port);
        return -1;
    }

    // 1 check 是否已存在;
    LIST_FOREACH(pVsi,&g_lstVsiBOutFlt,lstEntry)
    {
        if( (port == pVsi->port) && \
            (pVsi->vsiIDFormat == vdpass->vsiIDFormat) && \
            (0 == MEMCMP(pVsi->vsiID,vdpass->vsiID,VDPTLV_VSIID_LEN)) )
        {
            // 将flt删除 在后面执行;

            bFind = TRUE;
            break;
        }
    }	

    MEMCPY(&tlv_head,&vdpass->tlvHead,sizeof(tlv_head));
    vdpTlvHead_NTOH(&tlv_head);

    if(FALSE == bFind)
    {
        pVsiNew = (struct tagVsiBOutFilter *)GLUE_ALLOC(sizeof(struct tagVsiBOutFilter));
        if( NULL == pVsiNew )
        {
            EVB_LOG(EVB_LOG_ERROR, "vsiBOutFlt_addvsiID:(port:%u,vsiid:%s) alloc memory failed.\n",port,GetVsiidstr(vdpass->vsiID));
            return -1;
        }

        MEMSET(pVsiNew, 0, sizeof(struct tagVsiProfile));
        pVsiNew->port = port;
        pVsiNew->vsiIDFormat = vdpass->vsiIDFormat;
        MEMCPY(pVsiNew->vsiID,vdpass->vsiID,VDPTLV_VSIID_LEN);

        pVsiNew->tlvType = (BYTE)tlv_head.tlvType;
        MEMCPY(pVsiNew->vsiMgrID,mgrid,sizeof(pVsiNew->vsiMgrID));
        MEMCPY(pVsiNew->vsitypeID,vdpass->vsiTypeID,sizeof(pVsiNew->vsitypeID));
        pVsiNew->vsitypeVer = vdpass->vsiTypeVer;
        pVsiNew->fltFormat = vdpass->fltFormat;

        LIST_INIT(&pVsiNew->lstFlt);
        // insert list
        if(NULL == pVsi)
        {
            LIST_INSERT_HEAD(&g_lstVsiBOutFlt, pVsiNew, lstEntry);
        }
        else
        {
            LIST_INSERT_AFTER(pVsi,pVsiNew,lstEntry);
        }

        if( 0 != vsiBOutFlt_getVdpassLstFlt(vdpass,&tmpGMV_lstFlt) )
        {
            EVB_LOG(EVB_LOG_ERROR, "vsiBOutFlt_addvsiID: vsiBOutFlt_getVdpassLstFlt failed (port:%u,vsiid:%s).\n",port,GetVsiidstr(vdpass->vsiID));
            return -1;
        }
        
        if( 0 != vsiBOutFlt_addLstFlt(pVsiNew,&tmpGMV_lstFlt) )
        {
            EVB_LOG(EVB_LOG_ERROR, "vsiBOutFlt_addvsiID: vsiBOutFlt_addLstFlt failed (port:%u,vsiid:%s).\n",port,GetVsiidstr(vdpass->vsiID));
            return -1;
        }
    }
    else
    {
        pVsiNew = pVsi;

        pVsiNew->port = port;
        pVsiNew->tlvType = (BYTE)tlv_head.tlvType;
        MEMCPY(pVsiNew->vsiMgrID,mgrid,sizeof(pVsiNew->vsiMgrID));
        MEMCPY(pVsiNew->vsitypeID,vdpass->vsiTypeID,sizeof(pVsiNew->vsitypeID));
        pVsiNew->vsitypeVer = vdpass->vsiTypeVer;
        pVsiNew->fltFormat = vdpass->fltFormat;

        //LIST_INIT(&pVsiNew->lstFlt);

        if( 0 != vsiBOutFlt_getVdpassLstFlt(vdpass,&tmpGMV_lstFlt) )
        {
            EVB_LOG(EVB_LOG_ERROR, "vsiBOutFlt_addvsiID: vsiBOutFlt_getVdpassLstFlt failed (port:%u,vsiid:%s).\n",port,GetVsiidstr(vdpass->vsiID));
            return -1;
        }

        if( 0 != vsiBOutFlt_cmpdelLstFlt(pVsiNew,&tmpGMV_lstFlt))
        {
            EVB_LOG(EVB_LOG_ERROR, "vsiBOutFlt_addvsiID: vsiBOutFlt_cmpdelLstFlt failed (port:%u,vsiid:%s).\n",port,GetVsiidstr(vdpass->vsiID));
            return -1;
        }

        if( 0 != vsiBOutFlt_addLstFlt(pVsiNew,&tmpGMV_lstFlt) )
        {
            EVB_LOG(EVB_LOG_ERROR, "vsiBOutFlt_addvsiID: vsiBOutFlt_addLstFlt failed (port:%u,vsiid:%s).\n",port,GetVsiidstr(vdpass->vsiID));
            return -1;
        }
    }
        
    // 释放tmpGMV lstFlt;	
    while(NULL != (pTmpGMV = LIST_FIRST(&tmpGMV_lstFlt)))
    {
        LIST_REMOVE(pTmpGMV,lstEntry);
        GLUE_FREE(pTmpGMV);
        pTmpGMV = NULL;
    }
        
    return 0;
}

int32 vsiBOutFlt_delvsiID(u32 port,BYTE vsiidFormat,BYTE vsiID[VDPTLV_VSIID_LEN])
{
    struct tagVsiBOutFilter * pVsi = NULL;
    struct tagVsiGMVInfo *pGmv = NULL;
    LIST_FOREACH(pVsi,&g_lstVsiBOutFlt,lstEntry)
    {
        if( (port == pVsi->port) && \
            (pVsi->vsiIDFormat == vsiidFormat) && \
            (0 == MEMCMP(pVsi->vsiID,vsiID,VDPTLV_VSIID_LEN)) )
        {
            LIST_REMOVE(pVsi,lstEntry);

            while(NULL != (pGmv = LIST_FIRST(&pVsi->lstFlt) ) )
            {
                // delete GMV 
                vsiBOutFlt_Notify_MacVlan(pVsi->port,&pGmv->gmv,EVB_ISS_MAC_DEL);

                LIST_REMOVE(pGmv,lstEntry);
                GLUE_FREE(pGmv);
                pGmv = NULL;
            }

            GLUE_FREE(pVsi);
            return 0;
        }
    }	

    return 0;
}

struct tagVsiBOutFilter* vsiBOutFlt_getVsi(u32 port,BYTE vsiidFormat,BYTE vsiID[VDPTLV_VSIID_LEN])
{
    struct tagVsiBOutFilter * pVsi = NULL;

    LIST_FOREACH(pVsi,&g_lstVsiBOutFlt,lstEntry)
    {
        if( (port == pVsi->port) && \
            (pVsi->vsiIDFormat == vsiidFormat) && \
            (0 == MEMCMP(pVsi->vsiID,vsiID,VDPTLV_VSIID_LEN)) )
        {
            return pVsi;
        }
    }
    return NULL;
}

int32 DbgShowVsiOutFlt(u32 port)
{
    struct tagVsiBOutFilter * pVsi = NULL;
    struct tagVsiGMVInfo* pGMV = NULL;
    // u32 vsiid_ipv4  = 0;
    u32 groupid = 0;
    PRINTF_LOOPVAR_DEFINE;
    PRINTF("========================vdp bridge out filter information========================\n");
    LIST_FOREACH(pVsi,&g_lstVsiBOutFlt,lstEntry)
    {
        if( 0 != port )
        {
            if( port == pVsi->port)
            {
                continue;
            }
        }
        PRINTF("---------------------------------------------------\n");
        /*switch(pVsi->vsiIDFormat )
        {
        case VSIID_FORMAT_IPV4:
            {
                MEMCPY(&vsiid_ipv4,pVsi->vsiID+VDPTLV_VSIID_LEN-EVB_IPV4_ADDR_LEN,sizeof(u32));
                PRINTF("vsiID       : %s\n",evb_ipv4_to_string(vsiid_ipv4));
            }
            break;
        default:
            {
                PRINTF("vsiID       : ");
                PRINTF_VSIID(pVsi->vsiID);
            }
            break;
        }*/
        PRINTF("port        : %u\n",pVsi->port);
        PRINTF("vsiID       : ");
        PRINTF_VSIID(pVsi->vsiID);
        PRINTF("vsiID format: %u\n",pVsi->vsiIDFormat);
        PRINTF("mgrID       : ");
        PRINTF_MGRID(pVsi->vsiMgrID);
        PRINTF("typeID      : ");
        PRINTF_TYPEID(pVsi->vsitypeID);
        PRINTF("typeVer     : %u\n",pVsi->vsitypeVer);
        PRINTF("flt format  : %u\n",pVsi->fltFormat);
        PRINTF("flt number  : %u\n",pVsi->fltNumber);
        PRINTF("---- pcp ps grpid vid mac\n");
        LIST_FOREACH(pGMV,&pVsi->lstFlt,lstEntry)
        {
            MEMCPY(&groupid,pGMV->gmv.groupID,sizeof(pGMV->gmv.groupID));
            PRINTF("---- %3u,%2u,%5u,",pGMV->gmv.pcp,pGMV->gmv.ps,groupid);
            PRINTF("%u,%x:%x:%x:%x:%x:%x\n",pGMV->gmv.vid,pGMV->gmv.mac[0],pGMV->gmv.mac[1],pGMV->gmv.mac[2],pGMV->gmv.mac[3],pGMV->gmv.mac[4],pGMV->gmv.mac[5]);
        }
    }
    return 0;
}

#ifdef __cplusplus
}
#endif 

