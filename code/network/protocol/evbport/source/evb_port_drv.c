#ifdef __cplusplus
extern "C" {
#endif

#include "evb_comm.h"
#include "evb_packet.h"
#include "evb_port_mgr.h"
#include "evb_port_drv.h"

#include "lldp_in.h"
#include "ecp.h"

    static const u8 s_aLldpDstMac[EM_8021AB_BR_TYPE_ALL][EVB_PORT_MAC_LEN] = 
    {
        {0x01,0x80,0xc2,0x00,0x00,0x0e}, /* Nearest bridge */
        {0x01,0x80,0xc2,0x00,0x00,0x03}, /* Nearest non-TPMR bridge */
        {0x01,0x80,0xc2,0x00,0x00,0x00}, /* Nearest Customer Bridge */
    };

    /* 后续全部整改为pkt结构发送报文; */
    int32 SendEvbPacket(u32 uiPortNo, u8 *pkt, u16 len)
    {

        struct tagEvbPort *pPort = NULL;
        struct tagEvbIssPort *pSenderPort = NULL;
        int32 nRet = 0;

        if(NULL == pkt || len < 18)
        {
            EVB_ASSERT(0);
            return -1;
        }

        for(pPort = Get1stEvbPort(); pPort; pPort = GetNextEvbPort(pPort))
        {
            if(pPort->uiPortNo == uiPortNo)
            {
                pSenderPort = pPort->pIssPort;
                break;
            }        
        }

        if(NULL == pSenderPort || NULL == pSenderPort->pDriver)
        {
            return -1;
        }

//        nRet = pSenderPort->pDriver->pWrite(uiPortNo, pkt, len);
        nRet = pSenderPort->pDriver->pWrite(pSenderPort->uiPortNo, pkt, len);		
        if(pSenderPort->pStt && 0 == nRet)
        {
            EVB_PKT_COUNT_INC(pSenderPort->pStt->ullDrvSnd);
        }
        return 0;
    }
    


    int32 ReceivePktFromEvbIssPort(struct tagEvbIssPort *pIssPort, u8 *pPkt, u32 dwLen)
    {
        int32 nRet = -1;
        u8 *pData = pPkt;
        struct tagEvbPort *pEvbPort = NULL;

        if(NULL == pIssPort || NULL == pPkt)
        {
            EVB_ASSERT(0);
            return nRet;
        }

        if(dwLen < 18)
        {
            /* packet length err; */
            return nRet; 
        }

        pData = pPkt;

        if(0 == MEMCMP(pData+6, pIssPort->aucMac, EVB_PORT_MAC_LEN))
        {
            /* SRC MAC == this port MAC; */
            return nRet;
        }

        /* 报文送给逻辑口【URP、UAP、SBP等】 */
        for(pEvbPort = Get1stEvbPort(); pEvbPort; pEvbPort = GetNextEvbPort(pEvbPort))
        {
            if(pEvbPort->pIssPort == pIssPort)
            {
                switch(pEvbPort->ucEvbPortType)
                {
                case EM_EVB_PT_URP:
                case EM_EVB_PT_SBP:
                    {
                        if(0x88 == *(pData+12) && 0xCC == *(pData+13))
                        {
                            /* LLDP pkt;
                            *|++++++++++++++++++++++|+++++++++++|+++++++|++++++++|+++|
                            *|---------DA-----------|-----SA----|EthType|--Data--|---|
                            *|LLDP_Multicast address|MAC address| 88-CC |LLDP PDU|FCS|
                            *|----------6-----------|------6----|---2---|--1500--|-4-|(octets)
                            *|++++++++++++++++++++++|+++++++++++|+++++++|++++++++|+++|
                            */
                            if(0 != MEMCMP(pData, s_aLldpDstMac[EM_NEAREST_CUSTOMER_BRIDGE], EVB_PORT_MAC_LEN))
                            {   
                                /* Only process EM_NEAREST_NON_TPMR_BRIDGE lldp packets; */
                                break ;
                            }
                            gtLLDP.pfRcvPkt(pEvbPort->uiPortNo, pPkt, dwLen); 
                            nRet = 0;
                        }

                        if(0x88 == *(pData+12) && 0xDD == *(pData+13))
                        {
                            /* ECP packets */
                            gtECP.pfRcvPkt(pEvbPort->uiPortNo, pPkt, dwLen);
                            nRet = 0;
                        }
                        break;
                    }
                case EM_EVB_PT_UAP:
                    {
                        if(0x88 == *(pData+12) && 0xCC == *(pData+13))
                        {
                            /* LLDP pkt;
                            *|++++++++++++++++++++++|+++++++++++|+++++++|++++++++|+++|
                            *|---------DA-----------|-----SA----|EthType|--Data--|---|
                            *|LLDP_Multicast address|MAC address| 88-CC |LLDP PDU|FCS|
                            *|----------6-----------|------6----|---2---|--1500--|-4-|(octets)
                            *|++++++++++++++++++++++|+++++++++++|+++++++|++++++++|+++|
                            */
                            if(0 != MEMCMP(pData, s_aLldpDstMac[EM_NEAREST_NON_TPMR_BRIDGE], EVB_PORT_MAC_LEN))
                            {   
                                /* Only process EM_NEAREST_NON_TPMR_BRIDGE lldp packets; */
                                break ;
                            }
                            gtLLDP.pfRcvPkt(pEvbPort->uiPortNo, pPkt, dwLen);  
                            nRet = 0;
                        }
                        break;
                    }
                default:
                    {
                        EVB_ASSERT(0);
                        /* err:unknown logic evb port; */
                    }
                }/* end switch; */

            }
        }/* end for(pEvbPort = Get1stEvbPort()...;  */  

        if(0 == nRet && pIssPort->pStt)
        {
            EVB_PKT_COUNT_INC(pIssPort->pStt->ullDrvRcv);
        }

        return nRet;
    }

#ifdef __cplusplus
}
#endif
