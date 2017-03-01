#ifdef __cplusplus
extern "C"{
#endif 

#include "evb_comm.h"
#include "evb_dbg_func.h"
#include "evb_port_mgr.h"

    static void DbgShowEvbPort(struct tagEvbPort *pPort)
    {
        if(NULL == pPort)
        {
            return ;
        }

        switch(pPort->ucEvbPortType)
        {
        case EM_EVB_PT_SBP:
            {
                TEvbSbpAttr *pSbpAttr = (TEvbSbpAttr *)pPort->ptPortAttr;
                if(pSbpAttr)
                {
                    PRINTF("No:%-4u, Type:%-3u, Name: %-7s, ISS No:%u, ISSptr: %p, ECPptr: %p, VDP(b,s)ptr: [%p, %p], STTptr: %p.\n", 
                        pPort->uiPortNo,  pPort->ucEvbPortType, pPort->acName, 
                        (NULL != pPort->pIssPort) ? pPort->pIssPort->uiPortNo : 0,
                        pPort->pIssPort, pPort->pEcp, pPort->vdp_b, pPort->vdp_s, pPort->pStt);
                    PRINTF("--------------------------------------------------------------------------\nEVB TLV config:\n"); 
                    PRINTF("BGID: %s, RRCAP: %s, RRCTR: %s.\nR: %u, RTE: %u, RKA: %u, RWD: %u.\n",
                        (true == pSbpAttr->bBGID) ? "TRUE":"FALSE", 
                        (true == pSbpAttr->bRRCAP) ? "TRUE":"FALSE", 
                        (true == pSbpAttr->bRRCTR) ? "TRUE":"FALSE",  
                        pSbpAttr->ucR, pSbpAttr->ucRTE, pSbpAttr->ucRKA, pSbpAttr->ucRWD);
                    PRINTF("--------------------------------------------------------------------------\nEVB TLV negotiated result:\n"); 
                    PRINTF("VGID: %u, RRCTR: %u, RRSTAT: %u, R: %u, RTE: %u, RKA: %u, RWD: %u, Status: %s.\n",
                        pSbpAttr->tNegRes.ucVIDorGID, pSbpAttr->tNegRes.ucRRCTR, pSbpAttr->tNegRes.ucRRSTAT, 
                        pSbpAttr->tNegRes.ucEcpMaxRetries, pSbpAttr->tNegRes.ucRTE, pSbpAttr->tNegRes.ucRKA, 
                        pSbpAttr->tNegRes.ucRWD, 
                        (1 == pSbpAttr->tNegRes.ucStatus) ? "Success":"Initiate");
                    PRINTF("--------------------------------------------------------------------------\nECP Param:\n"); 
                    PRINTF("AckTimerInit: %u, OperMaxTries: %u.\n",
                        pSbpAttr->ucEcpOperAckTimerInit, 
                        pSbpAttr->ucEcpOperMaxTries);
                    PRINTF("--------------------------------------------------------------------------\nVDP Param:\n");
                    PRINTF("OperToutKeepAlive: %u, OperRsrcWaitDelay: %u, OperReinitKeepAlive: %u.\n",
                        pSbpAttr->uiVdpOperToutKeepAlive, 
                        pSbpAttr->uiVdpOperRsrcWaitDelay,
                        pSbpAttr->uiVdpOperReinitKeepAlive);
                    PRINTF("--------------------------------------------------------------------------\nLLDP Param:\n");
                    PRINTF("LldpManual: %s.\n", (true == pSbpAttr->bLldpManual) ? "TRUE":"FALSE");    
                }
            }
            break;
        case EM_EVB_PT_URP:
            {
                TEvbUrpAttr *pUrpAttr = (TEvbUrpAttr *)pPort->ptPortAttr;
                if(pUrpAttr)
                {
                    PRINTF("No:%-4u, Type:%-3u, Name: %-7s, ISS No:%u, ISSptr: %p, ECPptr: %p, VDP(b,s)ptr: [%p, %p], STTptr: %p.\n", 
                        pPort->uiPortNo,  pPort->ucEvbPortType, pPort->acName, 
                        (NULL != pPort->pIssPort) ? pPort->pIssPort->uiPortNo : 0,
                        pPort->pIssPort, pPort->pEcp, pPort->vdp_b, pPort->vdp_s, pPort->pStt);
                    PRINTF("--------------------------------------------------------------------------\nEVB TLV config:\n"); 
                    PRINTF("BGID: %s, RRReq: %s.\nR: %u, RTE: %u, RKA: %u, RWD: %u.\n",
                        (true == pUrpAttr->bSGrpID) ? "TRUE":"FALSE", 
                        (true == pUrpAttr->bRRReq) ? "TRUE":"FALSE", 
                        pUrpAttr->ucR, pUrpAttr->ucRTE, pUrpAttr->ucRKA, pUrpAttr->ucRWD);
                    PRINTF("--------------------------------------------------------------------------\nEVB TLV negotiated result:\n"); 
                    PRINTF("VGID: %u, RRCTR: %u, RRSTAT: %u, R: %u, RTE: %u, RKA: %u, RWD: %u, Status: %s.\n",
                        pUrpAttr->tNegRes.ucVIDorGID, pUrpAttr->tNegRes.ucRRCTR, pUrpAttr->tNegRes.ucRRSTAT, 
                        pUrpAttr->tNegRes.ucEcpMaxRetries, pUrpAttr->tNegRes.ucRTE, pUrpAttr->tNegRes.ucRKA, 
                        pUrpAttr->tNegRes.ucRWD, 
                        (1 == pUrpAttr->tNegRes.ucStatus) ? "Success":"Initiate");
                    PRINTF("--------------------------------------------------------------------------\nECP Param:\n"); 
                    PRINTF("AckTimerInit: %u, OperMaxTries: %u.\n",
                        pUrpAttr->ucEcpOperAckTimerInit, 
                        pUrpAttr->ucEcpOperMaxTries);
                    PRINTF("--------------------------------------------------------------------------\nVDP Param:\n");
                    PRINTF("OperRespWaitDelay: %u, OperRsrcWaitDelay: %u, OperReinitKeepAlive: %u.\n",
                        pUrpAttr->uiVdpOperRespWaitDelay, 
                        pUrpAttr->uiVdpOperRsrcWaitDelay,
                        pUrpAttr->uiVdpOperReinitKeepAlive);
                    PRINTF("--------------------------------------------------------------------------\nLLDP Param:\n");
                    PRINTF("LldpManual: %s.\n", (true == pUrpAttr->bLldpManual) ? "TRUE":"FALSE");    
                }
            }
            break;
        case EM_EVB_PT_UAP:
            {
                TEvbUapAttr *pUapAttr = (TEvbUapAttr *)pPort->ptPortAttr;
                if(pUapAttr)
                {
                    PRINTF("No:%-4u, Type:%-3u, Name: %-7s, ISS No:%u, ISSptr: %p, ECPptr: %p, VDP(b,s)ptr: [%p, %p], STTptr: %p.\n", 
                        pPort->uiPortNo,  pPort->ucEvbPortType, pPort->acName, 
                        (NULL != pPort->pIssPort) ? pPort->pIssPort->uiPortNo : 0,
                        pPort->pIssPort, pPort->pEcp, pPort->vdp_b, pPort->vdp_s, pPort->pStt);
                }
            }
            break;
        default:
            PRINTF("pPort->ucEvbPortType(%d) is unknown.\n", pPort->ucEvbPortType);
        }
        return ;
    }

    const char * DbgPortStateToString(u8 ucState)
    {
        if(EVB_PORT_STATE_INIT == ucState)
        {
            return "Init";
        }

        if(EVB_PORT_STATE_UP == ucState)
        {
            return "Up";
        }

        if(EVB_PORT_STATE_DW == ucState)
        {
            return "Down";
        }
        
        return "Unknown";
    }

    void DbgShowEvbPortInfo(u32 uiPortNo)
    {
        struct tagEvbPort *pPort = NULL;
        struct tagEvbIssPort *pIssPort = NULL;

        pPort = GetEvbPort(uiPortNo);
        if(NULL != pPort)
        {
            PRINTF("==========================================================================\n");
            DbgShowEvbPort(pPort);
            PRINTF("--------------------------------------------------------------------------\n"); 
            if(pPort->pIssPort)
            {
                pIssPort = pPort->pIssPort;
                PRINTF("%-10s %-14s %-8s %-14s\n", "IssPortNo", "Name", "State", "MAC address");
                PRINTF("--------------------------------------------------------------------------\n"); 
                PRINTF("%-10u %-14s %-8s 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x\n", 
                     pIssPort->uiPortNo, pIssPort->acName, DbgPortStateToString(pIssPort->ucPortState),
                     pIssPort->aucMac[0], pIssPort->aucMac[1], pIssPort->aucMac[2],
                     pIssPort->aucMac[3], pIssPort->aucMac[4], pIssPort->aucMac[5]);
            }
            else
            {
                PRINTF("NULL == pPort->pIssPort.\n");
            }
            PRINTF("==========================================================================\n"); 
        }
        else
        {
            PRINTF("==========================================================================\n");
            PRINTF("%-14s %-12s %-6s %-14s\n", "EvbPortNo", "Name", "Type", "IssPortNo");
            PRINTF("\n--------------------------------------------------------------------------\n");
            for(pPort = Get1stEvbPort(); pPort; pPort = GetNextEvbPort(pPort))
            {
                PRINTF("%-14u %-12s %-6u %-14u\n", 
                    pPort->uiPortNo, pPort->acName, pPort->ucEvbPortType, 
                    (NULL != pPort->pIssPort) ? pPort->pIssPort->uiPortNo : 0);              

            }
            
            PRINTF("\n--------------------------------------------------------------------------\n"); 
            PRINTF("%-10s %-14s %-8s %-14s\n", "IssPortNo", "Name", "State", "MAC address");

            for(pIssPort = Get1stEvbIssPort(); pIssPort; pIssPort = GetNextEvbIssPort(pIssPort))
            {
                PRINTF("%-10u %-14s %-8s 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x\n", 
                    pIssPort->uiPortNo, pIssPort->acName, DbgPortStateToString(pIssPort->ucPortState),
                    pIssPort->aucMac[0], pIssPort->aucMac[1], pIssPort->aucMac[2],
                    pIssPort->aucMac[3], pIssPort->aucMac[4], pIssPort->aucMac[5]);
            }
            PRINTF("==========================================================================\n");
        }

        return ;
    }

    void DbgShowEvbPortPktStt(u32 uiPortNo)
    {
        struct tagEvbIssPort *pPort = NULL;
        pPort = GetEvbIssPort(uiPortNo);
        if(NULL == pPort)
        {
            PRINTF("DbgShowEvbPortPktStt:NULL == pPort.\n");
            return ;
        }

        if(NULL == pPort->pStt)
        {
            PRINTF("DbgShowEvbPortPktStt:NULL == pPort->pStt.\n");
            return ;
        }

        EvbPktSttShow(pPort->pStt);

        return ;       
    }

#ifdef __cplusplus
}
#endif 

