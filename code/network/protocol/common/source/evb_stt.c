#ifdef __cplusplus
extern "C"{
#endif 

#include "evb_comm.h"
#include "evb_stt.h"

    void EvbPktSttShow(TEvbPktStt *pStt)
    {
        if(NULL == pStt)
        {
            EVB_ASSERT(0);
            return ;
        }

        PRINTF("\n============== CDCP Statistics ==============\n");
        /* cdcp stt; */
        PRINTF("  |__(CdcpModInitErr):\n");
        EvbPrintPktStt(3, pStt->ullCdcpModInitErr, "CDCP ModInitErr:");

        PRINTF("  |__(CdcpReceive):\n");
        EvbPrintPktStt(3, pStt->ullCdcpRcv, "CDCP receive:");
        PRINTF("  |__(CdcpReceiveErr):\n");
        EvbPrintPktStt(3, pStt->ullCdcpRcvErr, "CDCP receive failed:");
        EvbPrintPktStt(3, pStt->ullCdcpAllocRcvMemErr, "CDCP memory error:");
        EvbPrintPktStt(3, pStt->ullCdcpRcvLenErr, "CDCP length error:");
        EvbPrintPktStt(3, pStt->ullCdcpRcvTypeErr, "CDCP type error:");
        EvbPrintPktStt(3, pStt->ullCdcpRcvUnknowPkt, "CDCP receive failed:");

        PRINTF("  |__(CdcpSend):\n");
        EvbPrintPktStt(3, pStt->ullCdcpSnd, "CdcpSend:");
        PRINTF("  |__(CdcpSendErr):\n");
        EvbPrintPktStt(3, pStt->ullCdcpSndErr, "CdcpSendErr:");

        /* evbtlv stt; */

        /* ecp stt; */

        /* vdp stt; */

        /* lldp stt; */
        PRINTF("\n============== LLDP Statistics ==============\n");
        PRINTF("  |__(LldpSend):\n");
        EvbPrintPktStt(3, pStt->ullLldpSnd, "LldpSend:");
        PRINTF("  |__(LldpRcvErr):\n");
        EvbPrintPktStt(3, pStt->ullLldpRcv, "LldpRcvErr:");

        /* Driver stt; */
        PRINTF("\n============= Driver Statistics =============\n");
        PRINTF("  |__(DriverSend):\n");
        EvbPrintPktStt(3, pStt->ullDrvSnd, "DriverSend:");
        PRINTF("  |__(DriverRcvErr):\n");
        EvbPrintPktStt(3, pStt->ullDrvRcv, "DriverRcvErr:");

        PRINTF("\n============= End All Statistics ============\n");

        return ;
    }

#ifdef __cplusplus
}
#endif 
