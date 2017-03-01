#ifdef __cplusplus
extern "C"{
#endif 

#include "evb_comm.h"
#include "evb_dbg_func.h"
#include "lldp_in.h"
#include "cdcp_tlv.h"
#include "lldp_cdcp.h"
#include "evb_port_mgr.h"

    extern int32 g_iOpenSendPktPrint;
    extern int32 g_iOpenRcvPktPrint;

    void DbgOpenSndCdcpPktPrint(void)
    {
        g_iOpenSendPktPrint = 1;
        return ;
    }

    void DbgCloseSndCdcpPktPrint(void)
    {
        g_iOpenSendPktPrint = 0;
        return ;
    }

    void DbgOpenRcvCdcpPktPrint(void)
    {
        g_iOpenRcvPktPrint = 1;
        return ;
    }

    void DbgCloseRcvCdcpPktPrint(void)
    {
        g_iOpenRcvPktPrint = 0;
        return ;
    }

    void DbgShowCdcpConf(u32 uiLocalPort)
    {
        struct tagCdcpRunningData *pRunningData = NULL;
        struct tagCdcpScidSvidPair *pScidSvid = NULL;
        struct tagVlanInfo *pVlan = NULL;

        pRunningData = GetCdcpRunningData(uiLocalPort);
        if(NULL == pRunningData) 
        {
            PRINTF("Protocol running data @[%d]port is not exists\n", uiLocalPort);
            return ;
        }

        PRINTF("=============================== CDCP configure data ===============================\n");
        PRINTF("wAdminChnCap: %d, ucAdminRole: %s.wOperChnCap: %d, ucOperRole: %s \n\
               wRemoteChnCap: %d, ucRemoteRole: %s.ucRemoteVersion: %d, ucSchState: %s \n", 
               pRunningData->wAdminChnCap, 
               (CDCP_B == pRunningData->ucAdminRole) ? "Bridge" : "Station",
               pRunningData->wOperChnCap, 
               (CDCP_B == pRunningData->ucOperRole) ? "Bridge" : "Station",
               pRunningData->wRemoteChnCap, 
               (CDCP_B == pRunningData->ucRemoteRole) ? "Bridge" : "Station",
               pRunningData->ucRemoteVersion, 
               (CDCP_SM_NOT_RUNNING ==  pRunningData->ucSchState)? "NOT RUNNING" : "RUNNING");

        PRINTF("AdminSVIDWants: [SCID, SVID]\n");        
        LIST_FOREACH(pScidSvid, &pRunningData->lstAdminSVIDWantsHead, lstEntry) 
        {
            PRINTF("[%d, %d]", pScidSvid->wScid, pScidSvid->wSvid);
        }

        PRINTF("\nLastSVIDWants: [SCID, SVID]\n");
        LIST_FOREACH(pScidSvid, &pRunningData->lstLastSVIDWantsHead, lstEntry) 
        {
            PRINTF("[%d, %d]", pScidSvid->wScid, pScidSvid->wSvid);
        }

        PRINTF("\nOperSVIDList: [SCID, SVID]\n");
        LIST_FOREACH(pScidSvid, &pRunningData->lstOperSVIDListHead, lstEntry) 
        {
            PRINTF("[%d, %d]", pScidSvid->wScid, pScidSvid->wSvid);
        }

        PRINTF("\nRemoteSVIDList: [SCID, SVID]\n");
        LIST_FOREACH(pScidSvid, &pRunningData->lstRemoteSVIDListHead, lstEntry) 
        {
            PRINTF("[%d, %d]", pScidSvid->wScid, pScidSvid->wSvid);
        }

        PRINTF("\nLastRemoteSVIDList: [SCID, SVID]\n");
        LIST_FOREACH(pScidSvid, &pRunningData->lstLastRemoteSVIDListHead, lstEntry) 
        {
            PRINTF("[%d, %d]", pScidSvid->wScid, pScidSvid->wSvid);
        }

        PRINTF("\nLocalSVIDPool: [VID, Used], 0:unused,1:used.\n");
        LIST_FOREACH(pVlan, &pRunningData->tLocalSVIDPool.tVlanPoolHead, lstEntry) 
        {
            PRINTF("[%d, %d]", pVlan->wVlanID, pVlan->ucIsUse);
        }

        PRINTF("======================================== end ======================================\n");
        return ;
    }

    /* simulate station one port configure; */
#define DBG_OPER_CHN_CAP    (5)
    void DbgSetStationCdcpData(u32 uiLocalPort)
    {
        int32 i = 0;
        struct tagCdcpRunningData *pRunningData = NULL;
        struct tagCdcpScidSvidPair *pNewElm = NULL;
        struct lldp_port *pPort = NULL;

        pPort = find_by_port(uiLocalPort);
        if(NULL == pPort) 
        {
            PRINTF("Can't find lldp port[%d].\n", uiLocalPort);
            return ;
        }

        pRunningData = GetCdcpRunningData(uiLocalPort);
        if(NULL == pRunningData) 
        {
            PRINTF("Protocol running data @[%d]port is not exists.\n", uiLocalPort);
            return ;
        }
        /* ClearCdcpRunningData(pRunningData); */
        /* set AdminWants list; */            
        for(i = 0; i < DBG_OPER_CHN_CAP; i++)
        {
            pNewElm = (struct tagCdcpScidSvidPair *)GLUE_ALLOC(sizeof(struct tagCdcpScidSvidPair));
            if(NULL == pNewElm)
            {
                EVB_ASSERT(0);
                break;
            }
            MEMSET(pNewElm, 0x0, sizeof(struct tagCdcpScidSvidPair));
            pNewElm->wScid = i+1;/* 通道号不能有重复的; */
            if(1 == pNewElm->wScid)
            {
                 pNewElm->wSvid = 1; /* default s-channel pair<1,1>; */
            }
            InsertElmByScidAscendingOrder(pNewElm, &pRunningData->lstAdminSVIDWantsHead);
        }  
        pRunningData->ucAdminRole = CDCP_S;
        pRunningData->wAdminChnCap = DBG_OPER_CHN_CAP;
        pRunningData->ucOperVersion = CDCP_VERSION_1;      /* 需要看后续的标准具体怎么使用; */
        pRunningData->bSCompEnable = true;
        RunCdcpSM(pPort);

        somethingChangedLocal(uiLocalPort);
        return ;
    }

    /* simulate bridge one port configure; */
#define LOCAL_SVID_SIZE     (10)
    void DbgSetBridgeCdcpData(u32 uiLocalPort)
    {
        struct tagCdcpRunningData *pRunningData = NULL;
        struct lldp_port *pPort = NULL;

        pPort = find_by_port(uiLocalPort);
        if(NULL == pPort) 
        {
            PRINTF("Can't find lldp port[%d].\n", uiLocalPort);
            return ;
        }

        pRunningData = GetCdcpRunningData(uiLocalPort);
        if(NULL == pRunningData) 
        {
            PRINTF("Protocol running data @[%d]port is not exists\n", uiLocalPort);
            return ;
        }
        /* ClearCdcpRunningData(pRunningData); */
        pRunningData->ucAdminRole = CDCP_B;
        pRunningData->wAdminChnCap = LOCAL_SVID_SIZE;
        pRunningData->bSCompEnable = true;
        pRunningData->ucOperVersion = CDCP_VERSION_1;      /* 需要看后续的标准具体怎么使用; */

        /* init local svid pool; */
        pRunningData->tLocalSVIDPool.wMinVlanID = 2;
        pRunningData->tLocalSVIDPool.wMaxVlanID = 12;
        CreateVlanPool(&pRunningData->tLocalSVIDPool);

        RunCdcpSM(pPort);

        somethingChangedLocal(uiLocalPort);

        return ;
    }

#ifdef __cplusplus
}
#endif 
