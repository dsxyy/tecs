

#ifdef __cplusplus
extern "C"{
#endif 

#include "evb_comm.h"
#include "ecp.h"
#include "vdp.h"
#include "ecp_in.h"
#include "evb_port_mgr.h"

int ecp_dbg_print = 0;
int ecp_dbg_packet_print = 0;

/* print pack */
int gDbgEcpPacket = 0;
int gDbgEcpEvent = 0;

void DbgEcpPacket(int i)
{
    gDbgEcpPacket = i;
}

int GetDbgEcpPacket()
{
    return gDbgEcpPacket;
}

void DbgEcpEvent(int i)
{
    gDbgEcpEvent = i;
}

int GetDbgEcpEvent()
{
    return gDbgEcpEvent;
}


void DbgEcpPrintOpen()
{
    ecp_dbg_print = 1;
}

void DbgEcpPrintClose()
{
    ecp_dbg_print = 0;
}

int GetDbgEcpPrintStatus()
{
    return ecp_dbg_print;
}

extern const char *ecp_trace_tx_sm_status[];
extern const char *ecp_trace_rx_sm_status[];
void ShowEcpInfo(u32 port)
{
    struct tagEvbPort *pPort = NULL;
    pPort = Get1stEvbPort();

    if(NULL == pPort)
    {
        return ;
    }

    PRINTF(" ===============  EVB ECP Information  ===============\n");
    for(; pPort; pPort = GetNextEvbPort(pPort))
    {
        if( 0 != port )
        {
            if( port != pPort->uiPortNo )
            {
                continue;
            }
        }
        if( NULL != pPort->pEcp)
        {
            PRINTF("No.%u, Name: %s\nMac:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x\n", 
            pPort->uiPortNo, pPort->acName,
            pPort->pIssPort->aucMac[0], pPort->pIssPort->aucMac[1], pPort->pIssPort->aucMac[2],
            pPort->pIssPort->aucMac[3], pPort->pIssPort->aucMac[4], pPort->pIssPort->aucMac[5]);

            PRINTF("sSequence   : %u\n", pPort->pEcp->sSequence);
            PRINTF("acktimer    : %u\n", pPort->pEcp->ackTimer);
            ShowEcpTxListInfo(pPort);
            ShowEcpRxListInfo(pPort);
        }
       
    }
    PRINTF(" ====================================================\n");
    return ;
}

void ShowEcpTxListInfo(struct tagEvbPort *pPort)
{
    struct tagEcpULPDU *pRx = NULL;
    
    if(NULL == pPort || NULL == pPort->pEcp)
    {
        return ;
    }

    PRINTF("----------------------Ecp Tx List-------------------\n");
    LIST_FOREACH(pRx,&pPort->pEcp->lstEcpULPDUtx,lstEntry )
    {
        PRINTF("..................................................\n");
        PRINTF("sequence       : %u\n",pRx->sequence);
        PRINTF("port           : %u\n",pRx->PORT_NUM);
        PRINTF("tx_state       : %u\n",pRx->tx_state);
        PRINTF("tx_state       : %s\n",ecp_trace_tx_sm_status[pRx->tx_state]);
        PRINTF("ackTimer       : %u\n",pRx->ackTimer);
        PRINTF("retries        : %u\n",pRx->retries);
        PRINTF("requestReceived: %u\n",pRx->requestReceived);
        PRINTF("ackReceived    : %u\n",pRx->ackReceived);
        PRINTF("txErrors       : %u\n",pRx->txErrors);
        PRINTF("ecpduLen       : %u\n",pRx->ecpduLen);
        PRINTF("ecpdu          : %p\n",pRx->ecpdu);        
    }
}

void ShowEcpRxListInfo(struct tagEvbPort *pPort)
{
    struct tagEcpULPDU *pRx = NULL;
    
    if(NULL == pPort || NULL == pPort->pEcp)
    {
        return ;
    }
    PRINTF("----------------------Ecp Rx List-------------------\n");
    LIST_FOREACH(pRx,&pPort->pEcp->lstEcpULPDUrx,lstEntry )
    {
        PRINTF("..................................................\n");
        PRINTF("sequence       : %u\n",pRx->sequence);
        PRINTF("port           : %u\n",pRx->PORT_NUM);
        PRINTF("tx_state       : %u\n",pRx->tx_state);
        PRINTF("tx_state       : %s\n",ecp_trace_tx_sm_status[pRx->tx_state]);
        PRINTF("ackTimer       : %u\n",pRx->ackTimer);
        PRINTF("retries        : %u\n",pRx->retries);
        PRINTF("requestReceived: %u\n",pRx->requestReceived);
        PRINTF("ackReceived    : %u\n",pRx->ackReceived);
        PRINTF("txErrors       : %u\n",pRx->txErrors);
        PRINTF("ecpduLen       : %u\n",pRx->ecpduLen);
        PRINTF("ecpdu          : %p\n",pRx->ecpdu);  
    }
}

void ecp_mem_printf(u8 * p, u32 lenIn)
{
    u32 i = 0;
    if( NULL == p)
    {
        return;
    }
    PRINTF("%p ",p);
    for(i=0; i < lenIn; i++)
    {
        PRINTF("%.02x ", ((BYTE *)p)[i]);
    }
    PRINTF("\n");
}

static void ecp_packet_print_mgrid(u8 * pIn, u32 lenIn)
{
    struct tagVdpTlv_VSIMgrID mgrid;
    u8 * p = pIn;
    if(NULL == p)
    {
        return;
    }
    MEMCPY(&mgrid,p,sizeof(mgrid));
    p += VDPTLV_HEAD_LEN;

    PRINTF("mgrId : ");
    ecp_mem_printf(p, VDPTLV_VSIMGRID_LEN);
}

static void ecp_packet_print_vdpass_fltinfo(u8 fltFormat,u8 * pIn, u32 lenIn)
{
    //UINT16 len_tlv = 0;
    struct tagVDP_FLT_GROUPMACVID * pTmpGMV = NULL;
    struct tagVDP_FLT_MACVID * pTmpMV = NULL;
    struct tagVDP_FLT_GROUPVID * pTmpGV = NULL;
    struct tagVDP_FLT_VID * pTmpV = NULL;
    BYTE * pFltInfo = NULL;
    UINT16 nFltNum = 0;
    struct tagVDP_FLT_GROUPMACVID GMV;
    BYTE  * pCpy = NULL;
    u32 len = lenIn;
    u32 groupid = 0;

    if(NULL == pIn )
    {
        return;
    }		
    pFltInfo = pIn;

    MEMCPY(&nFltNum,pFltInfo,sizeof(nFltNum));
    nFltNum = EVB_NTOHS(nFltNum);
    PRINTF("Filter information\n");
    PRINTF("number of entries : %u; ",nFltNum);
    ecp_mem_printf(pFltInfo, sizeof(nFltNum));

    pFltInfo += sizeof(nFltNum);
    len -= sizeof(nFltNum);

    while(len > 0 && nFltNum > 0)
    {
        MEMSET(&GMV,0,sizeof(GMV));

        switch(fltFormat)
        {
        case VDP_FLTINFO_FORMAT_VID:
            {
                pTmpV = (struct tagVDP_FLT_VID *)pFltInfo;
                pCpy = (BYTE *)&GMV;
                pCpy += EVB_VDP_GROUPID_LEN;
                pCpy += EVB_VDP_MAC_LEN;
                MEMCPY(pCpy,pTmpV,sizeof(*pTmpV));
                vdpfltGROUPMACVID_ntohs(&GMV);	

                PRINTF("-------------------\n");
                PRINTF("---- ps :%u; ",GMV.ps);
                ecp_mem_printf(pFltInfo, sizeof(*pTmpV));
                PRINTF("---- pcp:%u\n",GMV.pcp);
                PRINTF("---- vid:%u\n",GMV.vid);

                if( len <= sizeof(*pTmpV))
                {
                    return;
                }
                len -= sizeof(*pTmpV);
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
                
                PRINTF("-------------------\n");
                PRINTF("---- mac:%x:%x:%x:%x:%x:%x; ",GMV.mac[0],GMV.mac[1],GMV.mac[2],GMV.mac[3],GMV.mac[4],GMV.mac[5]);
                ecp_mem_printf(pFltInfo, sizeof(GMV.mac));
                PRINTF("---- ps :%u; ",GMV.ps);
                ecp_mem_printf(pFltInfo + EVB_VDP_MAC_LEN, sizeof(struct tagVDP_FLT_VID));
                PRINTF("---- pcp:%u\n ",GMV.pcp);
                PRINTF("---- vid:%u\n ",GMV.vid);

                if( len <= sizeof(*pTmpMV))
                {
                    return ;
                }
                len -= sizeof(*pTmpMV);
                nFltNum--;
                pFltInfo += sizeof(*pTmpMV);
            }
            break;
        case VDP_FLTINFO_FORMAT_GROUPVID:
            {
                pTmpGV = (struct tagVDP_FLT_GROUPVID *)pFltInfo;
                pCpy = (BYTE *)&GMV;
                pCpy += EVB_VDP_MAC_LEN;
                MEMCPY(pCpy,pTmpGV,sizeof(*pTmpGV));
                vdpfltGROUPMACVID_ntohs(&GMV);					
                
                PRINTF("-------------------\n");
                MEMCPY(&groupid,GMV.groupID,sizeof(GMV.groupID));
                PRINTF("---- grd;%u ",groupid);
                ecp_mem_printf(pFltInfo, sizeof(GMV.groupID));
                PRINTF("---- mac:%x:%x:%x:%x:%x:%x; ",GMV.mac[0],GMV.mac[1],GMV.mac[2],GMV.mac[3],GMV.mac[4],GMV.mac[5]);
                PRINTF("---- ps :%u; ",GMV.ps);
                ecp_mem_printf(pFltInfo + EVB_VDP_GROUPID_LEN, sizeof(struct tagVDP_FLT_VID));
                PRINTF("---- pcp:%u\n ",GMV.pcp);
                PRINTF("---- vid:%u\n ",GMV.vid);

                if( len <= sizeof(*pTmpGV))
                {
                    return ;
                }
                len -= sizeof(*pTmpGV);
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
                
                PRINTF("-------------------\n");
                MEMCPY(&groupid,GMV.groupID,sizeof(GMV.groupID));
                PRINTF("---- grd;%u ",groupid);
                ecp_mem_printf(pFltInfo, sizeof(GMV.groupID));
                PRINTF("---- mac:%x:%x:%x:%x:%x:%x; ",GMV.mac[0],GMV.mac[1],GMV.mac[2],GMV.mac[3],GMV.mac[4],GMV.mac[5]);
                ecp_mem_printf(pFltInfo+EVB_VDP_MAC_LEN, sizeof(GMV.mac));
                PRINTF("---- ps :%u; ",GMV.ps);
                ecp_mem_printf(pFltInfo + EVB_VDP_MAC_LEN + EVB_VDP_GROUPID_LEN, sizeof(struct tagVDP_FLT_VID));
                PRINTF("---- pcp:%u\n ",GMV.pcp);
                PRINTF("---- vid:%u\n ",GMV.vid);

                if( len <= sizeof(*pTmpGMV))
                {
                    return ;
                }
                len -= sizeof(*pTmpGMV);
                nFltNum--;
                pFltInfo += sizeof(*pTmpGMV);
            }
            break;
        default:
            {
                // 不认识的filter info,直接退出;
                PRINTF( "vsiBOutFlt_addVdpassFltInfo: vdpass->fltFormat(%u) is invalid.\n",fltFormat);
                len = 0;
            }
            break;
        }
    }

    return ;
}

static void ecp_packet_print_vdpass(u8 * pIn, u32 lenIn)
{
    struct tagVdpTlv_VDPAss vdpass;
    u32 vsiid_ipv4 = 0;
    u8 * p = pIn;
    if(NULL == p || (lenIn < VDPTLV_VDPTLV_NOFLT_LEN + VDPTLV_HEAD_LEN))
    {
        return;
    }
    MEMCPY(&vdpass,p,sizeof(vdpass));
    p += VDPTLV_HEAD_LEN;
    
    PRINTF("reason :%u; ",vdpass.reason);
    ecp_mem_printf(p, sizeof(vdpass.reason));
    p += sizeof(vdpass.reason);

    PRINTF("vsi type id :; ",vdpass.vsiTypeID);
    ecp_mem_printf(p, sizeof(vdpass.vsiTypeID));
    p += sizeof(vdpass.vsiTypeID);

    PRINTF("vsi type ver :%u; ",vdpass.vsiTypeVer);
    ecp_mem_printf(p, sizeof(vdpass.vsiTypeVer));
    p += sizeof(vdpass.vsiTypeVer);

    PRINTF("vsiid format :%u; ",vdpass.vsiIDFormat);
    ecp_mem_printf(p, sizeof(vdpass.vsiIDFormat));
    p += sizeof(vdpass.vsiIDFormat);

    switch(vdpass.vsiIDFormat )
    {
    case VSIID_FORMAT_IPV4:
        {
            MEMCPY(&vsiid_ipv4,p+VDPTLV_VSIID_LEN-EVB_IPV4_ADDR_LEN,sizeof(u32));
            PRINTF("vsiID       : %s; ",evb_ipv4_to_string(vsiid_ipv4));
            ecp_mem_printf(p, sizeof(vdpass.vsiID));
            p += sizeof(vdpass.vsiID);
        }
        break;
    default:
        {
            PRINTF("vsiID       : ;");
            ecp_mem_printf(p, sizeof(vdpass.vsiID));
            p += sizeof(vdpass.vsiID);
        }
        break;
    }

    PRINTF("flt format  :%u; ",vdpass.fltFormat);
    ecp_mem_printf(p, sizeof(vdpass.fltFormat));
    p += sizeof(vdpass.fltFormat);

    // flt info
    if( lenIn <= VDPTLV_VDPTLV_NOFLT_LEN)
    {
        return;
    }
    lenIn -= VDPTLV_VDPTLV_NOFLT_LEN;
    ecp_packet_print_vdpass_fltinfo(vdpass.fltFormat,p,lenIn);
}

// 网络序;
void ecp_packet_print(u8 *p, u32 lenIn)
{	
    // u32 len_total = lenIn;
    struct ecp_ethhdr l2_head;
    struct ecp_ecpdu ecp_head;
    struct tagVdpTlv_head vdptlv_head;
    u8 * pTmp = p;

    if(0 == ecp_dbg_packet_print)
    {
        return;
    }

    if(NULL == p)
    {
        return;
    }

    DbgMemShow(p, lenIn);

    if( lenIn < EVB_ECP_ETHHEAD_LEN)
    {
        return;
    }
    memcpy(&l2_head, pTmp, EVB_ECP_ETHHEAD_LEN);
    
    PRINTF("dest Mac  : %x:%x:%x:%x:%x:%x; ",l2_head.h_dest[0],l2_head.h_dest[1],l2_head.h_dest[2],l2_head.h_dest[3],l2_head.h_dest[4],l2_head.h_dest[5]);
    ecp_mem_printf(pTmp, EVB_ECP_MAC_LEN);
    pTmp += EVB_ECP_MAC_LEN;
    PRINTF("dest Mac  : %x:%x:%x:%x:%x:%x; ",l2_head.h_source[0],l2_head.h_source[1],l2_head.h_source[2],l2_head.h_source[3],l2_head.h_source[4],l2_head.h_source[5]);
    ecp_mem_printf(pTmp, EVB_ECP_MAC_LEN);
    pTmp += EVB_ECP_MAC_LEN;
    PRINTF("etherType : %x; ",EVB_NTOHS(l2_head.h_proto));
    ecp_mem_printf(pTmp, sizeof(l2_head.h_proto));
    pTmp += sizeof(l2_head.h_proto);

    if( lenIn < EVB_ECP_ETHHEAD_LEN + EVB_ECP_ECPDU_HEAD_LEN)
    {
        return;
    }

    memcpy(&ecp_head,pTmp,EVB_ECP_ECPDU_HEAD_LEN);
    ecpduHead_NTOH(&ecp_head);
    PRINTF("ecp ver     : %u;",ecp_head.version);
    ecp_mem_printf(pTmp, EVB_ECP_ECPDU_HEAD_LEN);
    pTmp += EVB_ECP_ECPDU_HEAD_LEN;
    PRINTF("ecp operType: %u\n",ecp_head.operType);
    PRINTF("ecp subType : %u\n",ecp_head.subType);
    PRINTF("ecp sequence: %u\n",ecp_head.sequence);

    // ulp 解析
    lenIn -= (EVB_ECP_ETHHEAD_LEN + EVB_ECP_ECPDU_HEAD_LEN );
    while(lenIn > VDPTLV_HEAD_LEN)
    {
        memcpy(&vdptlv_head,pTmp,VDPTLV_HEAD_LEN);
        vdpTlvHead_NTOH(&vdptlv_head);
        PRINTF("vdp tlv type:%u;",vdptlv_head.tlvType);
        ecp_mem_printf(pTmp, VDPTLV_HEAD_LEN);
        //pTmp += VDPTLV_HEAD_LEN;
        PRINTF("vdp tlv len :%u\n",vdptlv_head.tlvLen);

        if( lenIn  < (u32)(VDPTLV_HEAD_LEN + vdptlv_head.tlvLen ))
        {
            return;
        }

        switch(vdptlv_head.tlvType)
        {
        case VDPTLV_TYPE_PRE_ASSOCIATE:
        case VDPTLV_TYPE_PRE_ASSOCIATE_WITH_RR:
        case VDPTLV_TYPE_ASSOCIATE:
        case VDPTLV_TYPE_DEASSOCIATE:
            {
                PRINTF("vdp tlv type: %s\n",GetVdpTlvTypeString(vdptlv_head.tlvType));
                ecp_packet_print_vdpass(pTmp,vdptlv_head.tlvLen + VDPTLV_HEAD_LEN);
            }
            break;
        case VDPTLV_TYPE_VSI_MGRID:
            {
                PRINTF("vdp tlv type: %s\n",GetVdpTlvTypeString(vdptlv_head.tlvType));
                ecp_packet_print_mgrid(pTmp,vdptlv_head.tlvLen + VDPTLV_HEAD_LEN);
            }
            break;
        default:break;
        }
        pTmp += VDPTLV_HEAD_LEN;
        pTmp += vdptlv_head.tlvLen;

        if( lenIn  >= (u32)(VDPTLV_HEAD_LEN + vdptlv_head.tlvLen ))
        {
            lenIn -= (VDPTLV_HEAD_LEN + vdptlv_head.tlvLen );
        }
        else
        {
            lenIn = 0;  
        }
    }
}

#ifdef __cplusplus
}
#endif 
