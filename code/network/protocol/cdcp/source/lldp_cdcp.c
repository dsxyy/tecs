#include "evb_cfg.h"
#include "evb_comm.h"
#include "lldp_in.h"
#include "cdcp_tlv.h"
#include "lldp_cdcp.h"
#include "evb_port_mgr.h"

#ifdef __cplusplus
extern "C" {
#endif 

    int32 g_iOpenSendPktPrint = 0;
    int32 g_iOpenRcvPktPrint = 0;

    int32 InitCdcp(void)
    {
        g_iOpenSendPktPrint = 0;
        g_iOpenRcvPktPrint = 0;
        /* init configure data */

        return 0;
    }

    int32 ShutdownCdcp(u32 uiLocalPort)
    {
        struct tagCdcpRunningData *pRunningData = NULL;

        pRunningData = GetCdcpRunningData(uiLocalPort);
        if(NULL == pRunningData) 
        {
            EVB_LOG(EVB_LOG_ERROR, "ShutdownCdcp:Protocol running data @[%d]port is not exists\n", uiLocalPort);
            return -1;
        }
        
        ClearCdcpRunningData(pRunningData);
        pRunningData->ucAdminRole = CDCP_B;
        pRunningData->ucOperRole = CDCP_B;

        FreeScidSvidList(&pRunningData->lstLastRemoteSVIDListHead);
        LIST_INIT(&pRunningData->lstLastRemoteSVIDListHead);
        FreeScidSvidList(&pRunningData->lstRemoteSVIDListHead);
        LIST_INIT(&pRunningData->lstRemoteSVIDListHead);
        FreeScidSvidList(&pRunningData->lstAdminSVIDWantsHead);
        LIST_INIT(&pRunningData->lstAdminSVIDWantsHead);
        FreeScidSvidList(&pRunningData->lstOperSVIDListHead);
        LIST_INIT(&pRunningData->lstOperSVIDListHead);
        FreeScidSvidList(&pRunningData->lstLastSVIDWantsHead);
        LIST_INIT(&pRunningData->lstLastSVIDWantsHead);   
        /* clear bridge vlan pool resource; */
        DestroyVlanPool(&pRunningData->tLocalSVIDPool);
        DestroyVlanPool(&pRunningData->tLastSVIDPool);

        if(pRunningData->pRxTlv)
        {
            pRunningData->pRxTlv = free_unpkd_tlv(pRunningData->pRxTlv);
            pRunningData->pRxTlv = NULL;
        } 

        return 0;
    }

    /* This function creates the OperSVIDList placed in the Local LLDP database */
    int32 SetSVIDRequest(u8 ucOperRole, 
                        struct tagListScidSvidHead *pAdminSVIDWantsHead, 
                        struct tagListScidSvidHead *pOperSVIDListHead)
    {
        struct tagCdcpScidSvidPair *pElm = NULL;
        struct tagCdcpScidSvidPair *pNewElm = NULL;

        if(NULL == pAdminSVIDWantsHead || NULL == pOperSVIDListHead)
        {
            EVB_ASSERT(0);
            return -1;
        }

        if(CDCP_S == ucOperRole)
        {
            /*
             * OperRole for the equipment is S, the  function compares the AdminSVIDWants with the
             * OperSVIDList and amends the OperSVIDList, as follows.
             *  1)All active S-channels in the OperSVIDList that are in the AdminSVIDWants are kept active,
             *    and in addition, any channels not currently  in the OperSVIDList are requested by including
             *    them in the OperSVIDList along with a 0 S-VID number. The OperSVIDList S-channel order
             *    is changed to match the AdminSVIDWants.
             *   2)Any S-channels in the OperSVIDList that are not in AdminSVIDWants are made inactive and
             *     are removed from the OperSVIDList.
             */
            if(LIST_EMPTY(pOperSVIDListHead))
            {
                CopyScidSvidList(pAdminSVIDWantsHead, pOperSVIDListHead);
            }
            else
            {
                /* check the OperSVID list. */
                pElm = LIST_FIRST(pOperSVIDListHead);
                while(pElm)                
                {
                    if(0 == IsInScidSvidList(pElm, pAdminSVIDWantsHead))
                    {
                        /* inactive S-channel */
                        LIST_REMOVE(pElm, lstEntry);
                        GLUE_FREE(pElm);
                        pElm = LIST_FIRST(pOperSVIDListHead); /* 后续性能提升时，此处可以优化,可以考虑按通道号排序; */
                        continue ;
                    }
                    pElm = LIST_NEXT(pElm, lstEntry);
                }

                /* check the AdminSvidWants list. */
                pElm = NULL;
                LIST_FOREACH(pElm, pAdminSVIDWantsHead, lstEntry)
                {
                    if(0 == IsInScidSvidList(pElm, pOperSVIDListHead))
                    {
                        /* request a new S-channel to match the AdminSvidWants. */
                        pNewElm = (struct tagCdcpScidSvidPair *)GLUE_ALLOC(sizeof(struct tagCdcpScidSvidPair));
                        if(NULL == pNewElm)
                        {
                            EVB_LOG(EVB_LOG_ERROR, "SetSVIDRequest: alloc memory failed.\n");
                            return -1;
                        }
                        MEMSET(pNewElm, 0x0, sizeof(struct tagCdcpScidSvidPair));
                        pNewElm->wScid = pElm->wScid;
                        InsertElmByScidAscendingOrder(pNewElm, pOperSVIDListHead);/* 后续性能提升时，此处可以优化; */
                    }
                }
            }/* end else */
        }/* end if(CDCP_S == ucOperRole) */
        else if(CDCP_B == ucOperRole)
        {
            /* OperRole for the equipment is B, then the OperSVIDList remains unchanged */
        }
        else
        {
            EVB_LOG(EVB_LOG_ERROR, "SetSVIDRequest: Input param error, ucOperRole = %d.\n", ucOperRole);
            EVB_ASSERT(0);
            return -1;
        }

        return 0;
    }
    
    int32 RxSVIDConfig(struct tagListScidSvidHead *pOperSVIDListHead, struct tagListScidSvidHead *pLastRemoteSVIDListHead)
    {
        struct tagCdcpScidSvidPair *pElm = NULL;
        struct tagCdcpScidSvidPair *pTmpElm = NULL;

        LIST_FOREACH(pElm, pOperSVIDListHead, lstEntry)
        {
            pTmpElm = GetScidSvidElem(pElm->wScid, pLastRemoteSVIDListHead);
            if(NULL == pTmpElm)
            {
                pElm->wSvid = 0; /* [注意]修改了本地tlv中的svid配置; */
            }
            else
            {
                pElm->wSvid = pTmpElm->wSvid;
            }
        }

        return 0;
    }

    /* This function creates the OperSVIDList placed in the Local LLDP database for a B role port. */
    /*
     * OperSVIDList is created as follows:
     *  a) For each S-channel in the RemoteSVIDList with a (SCID, S-VID) pair in the OperSVIDList the
     *     (SCID, S-VID) remains unchanged unless the S-VID is no longer part of the LastLocalSVIDPool. If
     *     the S-VID is no longer in the pool a new one is selected if available. If no S-VID is available the
     *     (SCID, S-VID) pair will be deleted from the OperSVIDList.
     *  b)For each S-channel in the RemoteSVIDList without a (SCID, S-VID) pair the (SCID, S-VID) pair
     *     will be deleted from the OperSVIDList.
     *  c)For a (SCID, SVID) pair in the remote list, where the S-VID is zero, an S-VID is assigned if it is
     *    available and the pair is inserted in the OperSVIDList. If an S-VID is not available, the pair is not
     *    inserted in the OperSVIDList.
     */
    int32 TxSVIDConfig(u16 wOperChnCap, 
                    u16 wRemoteChnCap,  
                    struct tagCdcpVlanPool *pLastLocalSVIDPool, 
                    struct tagListScidSvidHead *pRemoteSVIDListHead, 
                    struct tagListScidSvidHead *pOperSVIDListHead)
    {
        struct tagCdcpScidSvidPair *pElm = NULL;
        struct tagCdcpScidSvidPair *pNewElm = NULL;
        u16 wSmallerChnCap = 0;
        u16 wNewSVID = 0;

        if(NULL == pLastLocalSVIDPool || 
           NULL == pRemoteSVIDListHead ||
           NULL == pOperSVIDListHead)
        {
            EVB_ASSERT(0);
            return -1;
        }

        /* 
         * First the function takes the smaller of the OperChnCap and RemoteChnCap and truncates the
         * RemoteSVIDList to the smaller of the two. 
         * 需要考虑如何缩减？;
         */
        wSmallerChnCap = (wOperChnCap > wRemoteChnCap) ? wRemoteChnCap : wOperChnCap;

        if(LIST_EMPTY(pOperSVIDListHead))
        {
            /* 此处插入default pair 就不依赖对端发来的报文中是否带默认通道;
             * 对于默认通道的检查最好后续能够检查一下对端是否正确;
             */
            pNewElm = (struct tagCdcpScidSvidPair *)GLUE_ALLOC(sizeof(struct tagCdcpScidSvidPair));
            if(NULL == pNewElm)
            {
                EVB_LOG(EVB_LOG_ERROR, "TxSVIDConfig: No enough memory.\n");
                return -1;
            }
            MEMSET(pNewElm, 0x0, sizeof(struct tagCdcpScidSvidPair));
            pNewElm->wScid = 1;
            pNewElm->wSvid = 1;
            /* OperSVIDList must include default S-channel pair (1,1); */
            InsertElmByScidAscendingOrder(pNewElm, pOperSVIDListHead);   
            return -1;
        }

        /* check OperSVIDList; */
        pElm = LIST_FIRST(pOperSVIDListHead);
        while(pElm) 
        {
            if(1 == pElm->wScid && 1 == pElm->wSvid)
            {
                /* Default S-channel pair (1,1) not free; */
                pElm = LIST_NEXT(pElm, lstEntry);
                continue;
            }

            if(0 == IsInScidSvidList(pElm, pRemoteSVIDListHead))
            {
                /* Remove inactive S-channel; */
                LIST_REMOVE(pElm, lstEntry);
                /* free SVID to local SVID pool  */
                FreeSVID(pElm->wSvid, pLastLocalSVIDPool); 
                GLUE_FREE(pElm);
                pElm = LIST_FIRST(pOperSVIDListHead);

                continue ;
            }/* end if(0 == IsInScidSvidList...) */

            pElm = LIST_NEXT(pElm, lstEntry);
        }

        /* check RemoteSVIDList; */
        LIST_FOREACH(pElm, pRemoteSVIDListHead, lstEntry)
        {
            if(0 == IsInScidSvidList(pElm, pOperSVIDListHead))
            {
                if(1 == pElm->wScid)
                {
                    /* RemoteSVIDList should have the default S-channel pair ScidSvid(1,1); */    
                    EVB_LOG(EVB_LOG_ERROR, "TxSVIDConfig: RemoteSVIDList default S-channel pair (1,%d) error.\n", pElm->wSvid);
                }
                else
                {
                    /* active S-channel; */
                    wNewSVID = AllocSVID(pLastLocalSVIDPool);
                    if(0 == wNewSVID)
                    {
                        /* no enough svid resource; */
                        EVB_LOG(EVB_LOG_ERROR, "TxSVIDConfig: No enough SVID resource.\n");
                        break;
                    }

                    pNewElm = (struct tagCdcpScidSvidPair *)GLUE_ALLOC(sizeof(struct tagCdcpScidSvidPair));
                    if(NULL == pNewElm)
                    {
                        EVB_LOG(EVB_LOG_ERROR, "TxSVIDConfig: No enough memory.\n");
                        FreeSVID(wNewSVID, pLastLocalSVIDPool);
                        break;
                    }
                    MEMSET(pNewElm, 0x0, sizeof(struct tagCdcpScidSvidPair));
                    pNewElm->wScid = pElm->wScid;
                    pNewElm->wSvid = wNewSVID;
                    InsertElmByScidAscendingOrder(pNewElm, pOperSVIDListHead);
                }
            }/* end if(0 == IsInScidSvidList...) */
        }

        return 0;
    }

    /* list中通道号不能有重复的; */
    int32 CompareScidSvidList(struct tagListScidSvidHead *pListHead1, struct tagListScidSvidHead *pListHead2)
    {
        struct tagCdcpScidSvidPair *pScidSvid = NULL;
        int32 iList1Sz = 0;
        int32 iList2Sz = 0;

        if(NULL == pListHead1 || NULL == pListHead2)
        {
            EVB_ASSERT(0);
            return -1;
        }

        LIST_FOREACH(pScidSvid, pListHead1, lstEntry)
        {
            ++iList1Sz;
        }

        LIST_FOREACH(pScidSvid, pListHead2, lstEntry)
        {
            ++iList2Sz;
        }

        if(iList1Sz != iList2Sz)
        {
            return -1;
        }
                
        LIST_FOREACH(pScidSvid, pListHead1, lstEntry)
        {
            if(!IsInScidSvidList(pScidSvid, pListHead2))
            {
                return -1;
            }
        }

        return 0;
    }

    int32 IsInScidSvidList(struct tagCdcpScidSvidPair *pElm, struct tagListScidSvidHead *pListHead)
    {
        struct tagCdcpScidSvidPair *pListElm = NULL;

        if(NULL == pElm || NULL == pListHead)
        {
            EVB_ASSERT(0);
            return 0;
        }

        if(LIST_EMPTY(pListHead))
        {
            return 0;
        }

        LIST_FOREACH(pListElm, pListHead, lstEntry)
        {
            if(pElm->wScid == pListElm->wScid)
            {
                return 1;
            }
        }
        return 0;
    }

    int32 CopyScidSvidList(struct tagListScidSvidHead *pSrc, struct tagListScidSvidHead *pDest)
    {
        struct tagCdcpScidSvidPair *pPreElm = NULL;
        struct tagCdcpScidSvidPair *pElm = NULL;
        struct tagCdcpScidSvidPair *pNewElm = NULL;

        if(NULL == pSrc || NULL == pDest)
        {
            EVB_ASSERT(0);
            return -1;
        }

        if(!LIST_EMPTY(pDest))
        {
            FreeScidSvidList(pDest);
            LIST_INIT(pDest);
        }

        LIST_FOREACH(pElm, pSrc, lstEntry)
        {
            pNewElm = (struct tagCdcpScidSvidPair *)GLUE_ALLOC(sizeof(struct tagCdcpScidSvidPair));
            if(NULL == pNewElm)
            {
                EVB_LOG(EVB_LOG_ERROR, "CopyScidSvidList: GLUE_ALLOC memory error.\n");
                FreeScidSvidList(pDest);
                return -1;
            }

            MEMSET(pNewElm, 0x0, sizeof(struct tagCdcpScidSvidPair));
            pNewElm->wScid = pElm->wScid;
            pNewElm->wSvid = pElm->wSvid;
            if(NULL == pPreElm)
            {
                LIST_INSERT_HEAD(pDest, pNewElm, lstEntry);
            }
            else
            {
                LIST_INSERT_AFTER(pPreElm,pNewElm,lstEntry);
            }
            pPreElm = pNewElm;
        }

        return 0;
    }

    void FreeScidSvidList(struct tagListScidSvidHead *pListHead)
    {
        struct tagCdcpScidSvidPair  *pElm = NULL;

        if(NULL == pListHead)
        {
            EVB_ASSERT(0);
            return ;
        }

        while (!LIST_EMPTY(pListHead))
        {
            pElm = LIST_FIRST(pListHead);
            LIST_REMOVE(pElm, lstEntry);
            GLUE_FREE(pElm);
            pElm = NULL;
        }

        return ;
    }
    void InsertElmByScidAscendingOrder(struct tagCdcpScidSvidPair *pElm, struct tagListScidSvidHead *pListHead)
    {
        bool bInserted = false;
        struct tagCdcpScidSvidPair *pLstElm = NULL;
        struct tagCdcpScidSvidPair *pLstPreElm = NULL;

        if(NULL == pElm || NULL == pListHead)
        {
            EVB_ASSERT(0);
            return ;
        }
        /* 按照S-channel id从小到大插入; */
        if(LIST_EMPTY(pListHead))
        {
            LIST_INSERT_HEAD(pListHead, pElm, lstEntry);
            return ;
        }
        else
        {
            pLstPreElm = NULL;
            LIST_FOREACH(pLstElm, pListHead, lstEntry)
            {
                if(pElm->wScid < pLstElm->wScid)
                {
                    if(NULL == pLstPreElm)
                    {
                        LIST_INSERT_BEFORE(pLstElm, pElm, lstEntry);
                    }
                    else
                    {
                        LIST_INSERT_AFTER(pLstPreElm, pElm, lstEntry);
                    }                        
                    bInserted = true;
                    break;
                }
                pLstPreElm = pLstElm;
            }

            if(false == bInserted)
            {/* max-S-cid in the list; */
                LIST_INSERT_AFTER(pLstPreElm, pElm, lstEntry);
            }
        }/* end if(LIST_EMPTY(&s_tCdcpConfData.lstAdminSVIDWantsHead))...else ... */

        return ;
    }

    struct tagCdcpScidSvidPair *GetScidSvidElem(u16 wScid, struct tagListScidSvidHead *pListHead)
    {
        struct tagCdcpScidSvidPair *pLstElm = NULL;

        if(NULL == pListHead)
        {
            EVB_ASSERT(0);
            return NULL;
        }

        LIST_FOREACH(pLstElm, pListHead, lstEntry)
        {
            if(wScid == pLstElm->wScid)
            {
                return pLstElm;
            }
        }

        return NULL;
    }

    int32 GetScidSvidListCount(struct tagListScidSvidHead *pListHead)
    {
        struct tagCdcpScidSvidPair *pLstElm = NULL;
        int32 iCount = 0;

        if(NULL == pListHead)
        {
            EVB_ASSERT(0);
            return 0;
        }

        LIST_FOREACH(pLstElm, pListHead, lstEntry)
        {
            ++iCount;
        }

        return iCount;
    }

    int32 IsInLocalSVIDPool(u16 wSvid, u16 wPoolSz, u16 *pLocalSVIDPool)
    {
        u16 i = 0;

        if(NULL == pLocalSVIDPool)
        {
            EVB_ASSERT(0);
            return 0;
        }
        for(i = 0; i < wPoolSz; ++i)
        {
            if(wSvid == pLocalSVIDPool[i])
            {
                return 1;
            }
        }

        return 0;
    }

    u16 AllocSVID(struct tagCdcpVlanPool *pVlanPool)
    {
        struct tagVlanInfo *pVlanElm = NULL;

        if(NULL == pVlanPool)
        {
            EVB_ASSERT(0);
            return 0;
        }

        if(LIST_EMPTY(&pVlanPool->tVlanPoolHead))
        {
            return 0;
        }

        LIST_FOREACH(pVlanElm, &pVlanPool->tVlanPoolHead,lstEntry)
        {
            if(0 == pVlanElm->ucIsUse)
            {
                pVlanElm->ucIsUse = 1;
                return pVlanElm->wVlanID;
            }
        }
        return 0;
    }

    int32 FreeSVID(u16 wVlanID, struct tagCdcpVlanPool *pVlanPool)
    {
        struct tagVlanInfo *pVlanElm = NULL;

        if(NULL == pVlanPool)
        {
            EVB_ASSERT(0);
            return -1;
        }

        if(LIST_EMPTY(&pVlanPool->tVlanPoolHead))
        {
            return 0;
        }

        LIST_FOREACH(pVlanElm, &pVlanPool->tVlanPoolHead,lstEntry)
        {
            if(0 == pVlanElm->ucIsUse)
            {
                pVlanElm->ucIsUse = 0;
                return 0;
            }
        }
        return -1;
    }

    int32 DestroyVlanPool(struct tagCdcpVlanPool *pPool)
    {
        struct tagVlanInfo *pElm = NULL;
        if(NULL == pPool)
        {
            EVB_ASSERT(0);
            return -1;
        }

        pPool->wMaxVlanID = 0;
        pPool->wMinVlanID = 0;

        while(!LIST_EMPTY(&pPool->tVlanPoolHead))
        {
            pElm = LIST_FIRST(&pPool->tVlanPoolHead);
            LIST_REMOVE(pElm, lstEntry);
            GLUE_FREE(pElm);
            pElm = NULL;
        }

        return 0;
    }

    int32 CreateVlanPool(struct tagCdcpVlanPool *pPool)
    {
        struct tagVlanInfo *pVlan = NULL;
        struct tagVlanInfo *pVlanElm = NULL;

        u16 i = 0;
        if(NULL == pPool)
        {
            EVB_ASSERT(0);
            return -1;
        }

        if(!LIST_EMPTY(&pPool->tVlanPoolHead))
        {
            EVB_LOG(EVB_LOG_ERROR, "CreateVlanPool: the SVlan pool is existed.\n");
            return -1;
        }

        for(i = pPool->wMinVlanID; i < pPool->wMaxVlanID; i++)
        {
            pVlan = (struct tagVlanInfo *)GLUE_ALLOC(sizeof(struct tagVlanInfo));
            if(NULL == pVlan)
            {
                EVB_LOG(EVB_LOG_ERROR, "CreateVlanPool: alloc memory @step[i] failed,not enough memory space.\n", i);
                break;
            }
            MEMSET(pVlan, 0x0, sizeof(struct tagVlanInfo));
            pVlan->wVlanID = i;

            if(NULL == pVlanElm)
            {
                LIST_INSERT_HEAD(&pPool->tVlanPoolHead, pVlan, lstEntry);
            }
            else
            {
                LIST_INSERT_AFTER(pVlanElm, pVlan, lstEntry);
            }
            pVlanElm = pVlan;
        }
        return 0;
    }

    int32 SetVlanPool(struct tagCdcpVlanPool *pPool, u16 wMinVID, u16 wMaxVID)
    {
        if(NULL == pPool)
        {
            EVB_ASSERT(0);
            return -1;
        }

        if(wMinVID == pPool->wMinVlanID && wMaxVID == pPool->wMaxVlanID)
        {
            EVB_LOG(EVB_LOG_WARN, "SetVlanPool: set vlan range == old pool range.\n");
            return 0;
        }

        if(!LIST_EMPTY(&pPool->tVlanPoolHead))
        {            
            DestroyVlanPool(pPool);
        }

        pPool->wMaxVlanID = wMaxVID;
        pPool->wMinVlanID = wMinVID;

        return CreateVlanPool(pPool);
    }

    int32 CompareVlanPool(struct tagCdcpVlanPool *pPool1, struct tagCdcpVlanPool *pPool2)
    {
        if(NULL == pPool1 || NULL == pPool2)
        {
            EVB_ASSERT(0);
            return -1;
        }

        if(pPool1->wMaxVlanID != pPool2->wMaxVlanID || 
            pPool1->wMinVlanID != pPool2->wMinVlanID)
        {
            return -1;
        }

        return 0;
    }

    int32 CopyVlanPool(struct tagCdcpVlanPool *pSrc, struct tagCdcpVlanPool *pDest)
    {
        if(NULL == pSrc || NULL == pDest)
        {
            EVB_ASSERT(0);
            return -1;
        }

        if(!LIST_EMPTY(&pDest->tVlanPoolHead))
        {
            DestroyVlanPool(pDest);
            LIST_INIT(&pDest->tVlanPoolHead);
        }
        
        pDest->wMaxVlanID = pSrc->wMaxVlanID;
        pDest->wMinVlanID = pSrc->wMinVlanID;
        CreateVlanPool(pDest);

        return 0;
    }

    int32 SetVlanUsed(struct tagCdcpVlanPool *pPool, u16 wVID)
    {
        struct tagVlanInfo *pVlanElm = NULL;

        if(NULL == pPool)
        {
            EVB_ASSERT(0);
            return -1;
        }

        if(LIST_EMPTY(&pPool->tVlanPoolHead))
        {            
            return -1;
        }

        LIST_FOREACH(pVlanElm, &pPool->tVlanPoolHead, lstEntry)
        {
            if(wVID == pVlanElm->wVlanID)
            {
                pVlanElm->ucIsUse = 1;
                return 0;
            }
        }

        return -1;
    }

    void RunCdcpSM(struct lldp_port *pPort)
    {
        struct tagCdcpRunningData *pCdcpRunningData = NULL;
        int32 nIsNeedTxConf = 0;

        if(NULL == pPort)
        {
            EVB_ASSERT(0);
            return ;
        }

        pCdcpRunningData = GetCdcpRunningData(pPort->local_port);
        if(NULL == pCdcpRunningData)
        {
            EVB_LOG(EVB_LOG_ERROR, "RunCdcpSM: protocol running data @[%d]port is exists\n", pPort->local_port);
            return ;
        }

        /* for station; */
        if(CDCP_S == pCdcpRunningData->ucAdminRole)
        {
            switch(pCdcpRunningData->ucSchState)
            {
            case CDCP_SM_NOT_RUNNING:
                {
                    if(pCdcpRunningData->ucOperRole != pCdcpRunningData->ucAdminRole)
                    {
                        /* Init; */
                        break;
                    }

                    /* ChannelRequest; */
                    if (pCdcpRunningData->wOperChnCap != pCdcpRunningData->wAdminChnCap ||
                        0 != CompareScidSvidList(&pCdcpRunningData->lstAdminSVIDWantsHead, &pCdcpRunningData->lstLastSVIDWantsHead))
                    {
                        pCdcpRunningData->wOperChnCap = pCdcpRunningData->wAdminChnCap;
                        CopyScidSvidList(&pCdcpRunningData->lstAdminSVIDWantsHead, &pCdcpRunningData->lstLastSVIDWantsHead);
                        /* 感觉此处应该使用lstLastSVIDWantsHead为入参，lstAdminSVIDWantsHead作为外部配置结果存放; */
                        SetSVIDRequest(pCdcpRunningData->ucOperRole, &pCdcpRunningData->lstAdminSVIDWantsHead, 
                            &pCdcpRunningData->lstOperSVIDListHead);
                    }

                    if(CDCP_B == pCdcpRunningData->ucRemoteRole)
                    {
                        /* RxSVIDs; */
                        if(0 != CompareScidSvidList(&pCdcpRunningData->lstLastRemoteSVIDListHead, &pCdcpRunningData->lstRemoteSVIDListHead))
                        {
                            CopyScidSvidList(&pCdcpRunningData->lstRemoteSVIDListHead, &pCdcpRunningData->lstLastRemoteSVIDListHead);

                            RxSVIDConfig(&pCdcpRunningData->lstOperSVIDListHead, &pCdcpRunningData->lstRemoteSVIDListHead);

                            pCdcpRunningData->ucSchState = CDCP_SM_RUNNING;
                        }
                    }
                }
                break;
            case  CDCP_SM_RUNNING:
                {
                    if (pCdcpRunningData->ucOperRole != pCdcpRunningData->ucAdminRole || 
                        CDCP_B != pCdcpRunningData->ucRemoteRole)
                    {
                        /* Init; */
                        ClearCdcpRunningData(pCdcpRunningData);
                        break;
                    }

                    /* ChannelRequest; */
                    if( pCdcpRunningData->wOperChnCap != pCdcpRunningData->wAdminChnCap ||
                        0 != CompareScidSvidList(&pCdcpRunningData->lstAdminSVIDWantsHead, &pCdcpRunningData->lstLastSVIDWantsHead))
                    {
                        pCdcpRunningData->wOperChnCap = pCdcpRunningData->wAdminChnCap;
                        CopyScidSvidList(&pCdcpRunningData->lstAdminSVIDWantsHead, &pCdcpRunningData->lstLastSVIDWantsHead);
                        /* 感觉此处应该使用lstLastSVIDWantsHead为入参，lstAdminSVIDWantsHead作为外部配置结果存放; */
                        SetSVIDRequest(pCdcpRunningData->ucOperRole, &pCdcpRunningData->lstAdminSVIDWantsHead, 
                            &pCdcpRunningData->lstOperSVIDListHead);
                    }

                    /* RxSVIDs; */
                    if(0 != CompareScidSvidList(&pCdcpRunningData->lstLastRemoteSVIDListHead, &pCdcpRunningData->lstRemoteSVIDListHead))
                    {
                        CopyScidSvidList(&pCdcpRunningData->lstRemoteSVIDListHead, &pCdcpRunningData->lstLastRemoteSVIDListHead);
                        RxSVIDConfig(&pCdcpRunningData->lstOperSVIDListHead, &pCdcpRunningData->lstRemoteSVIDListHead);
                    }
                }
                break;
            default:
                {
                    EVB_ASSERT(0);
                    EVB_LOG(EVB_LOG_ERROR, "RunCdcpSM: Station CDCP SM state[%d] is unknown.\n", pCdcpRunningData->ucSchState);
                }
            }
        }/* if(CDCP_S == pCdcpRunningData->ucAdminRole); */

        /* for bridge */
        if(CDCP_B == pCdcpRunningData->ucAdminRole)
        {
            switch(pCdcpRunningData->ucSchState)
            {
            case CDCP_SM_NOT_RUNNING:
                {
                    if(pCdcpRunningData->ucOperRole != pCdcpRunningData->ucAdminRole)
                    {
                        /* Init; */
                        break;
                    }

                    if (pCdcpRunningData->wOperChnCap != pCdcpRunningData->wAdminChnCap)
                    {
                        pCdcpRunningData->wOperChnCap = pCdcpRunningData->wAdminChnCap;
                        /* ChannelRequest; 在bridge上此处感觉是无用功; */
#if 0
                        SetSVIDRequest(pCdcpRunningData->ucOperRole, &pCdcpRunningData->lstAdminSVIDWantsHead, 
                            &pCdcpRunningData->lstOperSVIDListHead);
#endif
                    }

                    if(CDCP_S == pCdcpRunningData->ucRemoteRole)
                    {
                        /* TxSVIDs; */
                        nIsNeedTxConf = 0;
                        if(0 != CompareVlanPool(&pCdcpRunningData->tLastSVIDPool, &pCdcpRunningData->tLocalSVIDPool))
                        {
                            CopyVlanPool(&pCdcpRunningData->tLocalSVIDPool, &pCdcpRunningData->tLastSVIDPool);
                            nIsNeedTxConf = 1;
                        } 

                        if(0 != CompareScidSvidList(&pCdcpRunningData->lstLastRemoteSVIDListHead, &pCdcpRunningData->lstRemoteSVIDListHead))
                        {
                            CopyScidSvidList(&pCdcpRunningData->lstRemoteSVIDListHead, &pCdcpRunningData->lstLastRemoteSVIDListHead); 
                            nIsNeedTxConf = 1;
                        }

                        if(1 == nIsNeedTxConf)
                        {
                            struct tagVlanInfo *pVlanElm = NULL;

                            TxSVIDConfig(pCdcpRunningData->wOperChnCap, 
                                pCdcpRunningData->wRemoteChnCap, 
                                &pCdcpRunningData->tLastSVIDPool,
                                &pCdcpRunningData->lstRemoteSVIDListHead,
                                &pCdcpRunningData->lstOperSVIDListHead
                                );
                            /* 更新LocalSVIDPool,后续考虑优化为数组实现; */
                            LIST_FOREACH(pVlanElm, &pCdcpRunningData->tLastSVIDPool.tVlanPoolHead, lstEntry)
                            {
                                if(1 == pVlanElm->ucIsUse)
                                {
                                    SetVlanUsed(&pCdcpRunningData->tLocalSVIDPool, pVlanElm->wVlanID);
                                }
                            }

                            pCdcpRunningData->ucSchState = CDCP_SM_RUNNING;
                        } 
                    }/* if(CDCP_S == pCdcpRunningData->ucRemoteRole) */
                }
                break;
            case  CDCP_SM_RUNNING:
                {                    
                    if (pCdcpRunningData->ucOperRole != pCdcpRunningData->ucAdminRole || 
                        CDCP_S != pCdcpRunningData->ucRemoteRole)
                    {
                        /* Init; */
                        ClearCdcpRunningData(pCdcpRunningData);
                        break;
                    }

                    if( pCdcpRunningData->wOperChnCap != pCdcpRunningData->wAdminChnCap)
                    {
                        pCdcpRunningData->wOperChnCap = pCdcpRunningData->wAdminChnCap;
                        /* ChannelRequest; 在bridge上此处感觉是无用功;  */
#if 0
                        SetSVIDRequest(pCdcpRunningData->ucOperRole, &pCdcpRunningData->lstAdminSVIDWantsHead, 
                            &pCdcpRunningData->lstOperSVIDListHead);
#endif
                    }

                    /* TxSVIDs; */   
                    nIsNeedTxConf = 0;
                    if(0 != CompareVlanPool(&pCdcpRunningData->tLastSVIDPool, &pCdcpRunningData->tLocalSVIDPool))
                    {
                        CopyVlanPool(&pCdcpRunningData->tLocalSVIDPool, &pCdcpRunningData->tLastSVIDPool);
                        nIsNeedTxConf = 1;
                    } 

                    if(0 != CompareScidSvidList(&pCdcpRunningData->lstLastRemoteSVIDListHead, &pCdcpRunningData->lstRemoteSVIDListHead))
                    {
                        CopyScidSvidList(&pCdcpRunningData->lstRemoteSVIDListHead, &pCdcpRunningData->lstLastRemoteSVIDListHead); 
                        nIsNeedTxConf = 1;
                    }

                    if(1 == nIsNeedTxConf)
                    {
                        struct tagVlanInfo *pVlanElm = NULL;

                        TxSVIDConfig(pCdcpRunningData->wOperChnCap, 
                            pCdcpRunningData->wRemoteChnCap, 
                            &pCdcpRunningData->tLastSVIDPool,
                            &pCdcpRunningData->lstRemoteSVIDListHead,
                            &pCdcpRunningData->lstOperSVIDListHead
                            );
                        /* 更新LocalSVIDPool,后续考虑优化为数组实现; */
                        LIST_FOREACH(pVlanElm, &pCdcpRunningData->tLastSVIDPool.tVlanPoolHead, lstEntry)
                        {
                            if(1 == pVlanElm->ucIsUse)
                            {
                                SetVlanUsed(&pCdcpRunningData->tLocalSVIDPool, pVlanElm->wVlanID);
                            }
                        }

                        pCdcpRunningData->ucSchState = CDCP_SM_RUNNING;
                    } 
                }
                break;
            default:
                {
                    EVB_ASSERT(0);
                    EVB_LOG(EVB_LOG_ERROR, "RunCdcpSM: Station CDCP SM state[%d] is unknown.\n", pCdcpRunningData->ucSchState);
                }
            }
        }/* if(CDCP_B == pCdcpRunningData->ucAdminRole); */  

        return ;
    }

    /*
    lldp_mod_ops members:
    {
        struct lldp_module *    (* lldp_mod_register)(void);
        void            (* lldp_mod_unregister)(struct lldp_module *);
        struct packed_tlv *     (* lldp_mod_gettlv)(struct lldp_port *);
        int             (* lldp_mod_rchange)(struct lldp_port *,
        struct unpacked_tlv *,
        bool *);
        void            (* lldp_mod_utlv)(struct lldp_port *);
        void            (* lldp_mod_ifup)(u32); 
        void            (* lldp_mod_ifdown)(u32);
        void            (* lldp_mod_ifadd)(u32);
        void            (* lldp_mod_ifdel)(u32);
        u8          (* lldp_mod_mibdelete)(struct lldp_port *port);
        u32         (* client_register)(void);
        int             (* client_cmd)(void *data, void *from,  socklen_t fromlen, char *ibuf,
                                        int ilen, char *rbuf, int rlen);
        int             (* print_tlv)(u32, u16, char *);
        u32         (* lookup_tlv_name)(char *);
        int         (* print_help)();
        int         (* timer)();
        struct arg_handlers *   (* get_arg_handler)(void);
    }
    */
    /*struct config_t lldpad_cfg;*/
    static const struct lldp_mod_ops cdcp_ops = {
        CdcpRegister,
        CdcpUnregister,
        CdcpGetTlv,
        CdcpRChange,
        0,
        CdcpIfup,
        CdcpIfDown,        
        CdcpIfAdd,
        CdcpIfDel,
        CdcpMibDeleteObjects,
        0,
        0,
        0,
        0,
        0,
        CdcpCheckPending,
        0
    };

    /* CDCP functions for LLDP */
    struct tagCdcpRunningData *GetCdcpRunningData(u32 uiPort)
    {
        struct tagCdcpUserData *pUserData = NULL;
        struct tagCdcpRunningData *pCdcpRunningData = NULL;

        pUserData = (struct tagCdcpUserData *)find_module_user_data_by_id(&gtLLDP.mod_head, LLDP_MOD_CDCP);
        if (pUserData) 
        {
            LIST_FOREACH(pCdcpRunningData, &pUserData->head, lstEntry) 
            {
                if (pCdcpRunningData->uiLocalPort == uiPort)
                {
                    return pCdcpRunningData;
                }
            }
        }

        return NULL;
    }

    void CdcpIfAdd(u32 uiLocalPort)
    {
        struct tagCdcpUserData *pUserData;
        struct tagCdcpRunningData *pData = NULL;

        if(EVBSYS_TYPE_BRIDGE != EvbcfgGetSysType() && 
            EVBSYS_TYPE_STATION != EvbcfgGetSysType())
        {
            EVB_LOG(EVB_LOG_ERROR, "CdcpIfAdd: protocol running data @[%d]port not support CDCP.\n", uiLocalPort);
            return ;
        }

        pData = GetCdcpRunningData(uiLocalPort);
        if(pData) 
        {
            EVB_LOG(EVB_LOG_ERROR, "CdcpIfAdd: protocol running data @[%d]port is exists\n", uiLocalPort);
            return ;
        }

        pUserData = (struct tagCdcpUserData *)find_module_user_data_by_id(&gtLLDP.mod_head, LLDP_MOD_CDCP);
        if(NULL == pUserData)
        {
            EVB_LOG(EVB_LOG_ERROR, "CdcpIfAdd:CDCP MOD is not registed to LLDP.\n");
            return ;
        }

        /* init cdcp running data; */
        pData = (struct tagCdcpRunningData *)GLUE_ALLOC(sizeof(struct tagCdcpRunningData));
        if(NULL == pData)
        {
            EVB_LOG(EVB_LOG_ERROR, "CdcpIfAdd: alloc %d port CDCP running data failed.\n", uiLocalPort);
            return ;
        }
        MEMSET(pData, 0x0, sizeof(struct tagCdcpRunningData));
        pData->uiLocalPort = uiLocalPort;
        if(0 != InitCdcpRunningData(pData))
        {
            EVB_LOG(EVB_LOG_ERROR, "CdcpIfAdd: Init CDCP running data @[%d]port failed.\n", uiLocalPort);
            GLUE_FREE(pData);
            pData = NULL;
            return ;
        }

        /* add protocol data based on port to list; */
        LIST_INSERT_HEAD(&pUserData->head, pData, lstEntry);

        return ;
    }

    void CdcpIfDel(u32 uiLocalPort)
    {
        ShutdownCdcp(uiLocalPort);
        return ;
    }
    
    int32 InitCdcpRunningData(struct tagCdcpRunningData *pData)
    {
        if(EVBSYS_TYPE_BRIDGE != EvbcfgGetSysType() && 
            EVBSYS_TYPE_STATION != EvbcfgGetSysType())
        {
            EVB_ASSERT(0);
            return -1;
        }      

        LIST_INIT(&pData->lstAdminSVIDWantsHead);
        LIST_INIT(&pData->lstLastRemoteSVIDListHead);
        LIST_INIT(&pData->lstLastSVIDWantsHead);
        LIST_INIT(&pData->lstOperSVIDListHead);
        LIST_INIT(&pData->lstRemoteSVIDListHead);        

        if(EVBSYS_TYPE_BRIDGE == EvbcfgGetSysType())
        {
            pData->ucAdminRole = CDCP_B;
            pData->tLocalSVIDPool.wMinVlanID = 0;
            pData->tLocalSVIDPool.wMaxVlanID = 0;
        }
        if(EVBSYS_TYPE_STATION == EvbcfgGetSysType())
        {
            pData->ucAdminRole = CDCP_S;
        }
        pData->ucOperRole = pData->ucAdminRole;
        pData->ucRemoteRole = pData->ucAdminRole;
        pData->wAdminChnCap = 0;
        pData->wOperChnCap = 0;

        pData->ucSchState = CDCP_SM_NOT_RUNNING; 
        pData->ucOperVersion = CDCP_VERSION_0;      /* 需要看后续的标准具体怎么使用; */
        pData->ucRemoteVersion = CDCP_VERSION_0;
        pData->pRxTlv = NULL;        
        pData->bSCompEnable = false;

        return 0;
    }

    int32 ClearCdcpRunningData(struct tagCdcpRunningData *pData)
    {
        struct tagCdcpScidSvidPair *pScidSvid = NULL;

        if(NULL == pData)
        {
            EVB_ASSERT(0);
            return -1;
        } 

        FreeScidSvidList(&pData->lstLastRemoteSVIDListHead);
        LIST_INIT(&pData->lstLastRemoteSVIDListHead);
        FreeScidSvidList(&pData->lstRemoteSVIDListHead);
        LIST_INIT(&pData->lstRemoteSVIDListHead);
        FreeScidSvidList(&pData->lstOperSVIDListHead);
        LIST_INIT(&pData->lstOperSVIDListHead);

        if(CDCP_B == pData->ucAdminRole)
        {
            /* clear operSvidList; */
            LIST_FOREACH(pScidSvid, &pData->lstOperSVIDListHead, lstEntry)
            {
                if(pScidSvid->wScid != 1 ||  pScidSvid->wSvid != 1)
                {
                    /* todo:call delete vp port; */
                    /* free SVID to local SVID pool  */
                    FreeSVID(pScidSvid->wSvid, &pData->tLocalSVIDPool); 
                }
            }
            DestroyVlanPool(&pData->tLastSVIDPool);
        }

        if(CDCP_S == pData->ucAdminRole)
        {
            /* 清理S通道; */
            LIST_FOREACH(pScidSvid, &pData->lstAdminSVIDWantsHead, lstEntry)
            {
                if(pScidSvid->wScid != 1 ||  pScidSvid->wSvid != 1)
                {
                    /* todo: call free s-channel port function; */
                    /* todo:call delete vp port; */
                    pScidSvid->wSvid = 0;
                }
            }
            FreeScidSvidList(&pData->lstLastSVIDWantsHead);
            LIST_INIT(&pData->lstLastSVIDWantsHead);

        } 

        pData->wOperChnCap = 0;
        pData->ucOperRole = pData->ucAdminRole;
        pData->bRemoteSCompEnable = false;
        pData->ucRemoteRole = pData->ucAdminRole;
        pData->wRemoteChnCap = 0;  
        pData->ucSchState = CDCP_SM_NOT_RUNNING; 

        if(pData->pRxTlv)
        {
            pData->pRxTlv = free_unpkd_tlv(pData->pRxTlv);
            pData->pRxTlv = NULL;
        }

        return 0;
    }

    void CdcpIfDown(u32 uiLocalPort)
    {
        struct tagCdcpRunningData *pData = NULL;

        pData = GetCdcpRunningData(uiLocalPort);
        if (NULL == pData)
        {
            EVB_LOG(EVB_LOG_ERROR, "CdcpIfDown: protocol running data @[%d]port is not exists\n", uiLocalPort);
            return ;
        }
        EVB_LOG(EVB_LOG_WARN, "CdcpIfDown:[%d]port is down\n", uiLocalPort);

        ClearCdcpRunningData(pData);

        return ;
    }

    void CdcpIfup(u32 uiLocalPort)
    {
        struct tagCdcpRunningData *pData = NULL;

        pData = GetCdcpRunningData(uiLocalPort);
        if (NULL == pData)
        {
            EVB_LOG(EVB_LOG_ERROR, "CdcpIfAdd: protocol running data @[%d]port is exists\n", uiLocalPort);
            return ;
        }

        /* set last SVIDwants list; */
        if(!LIST_EMPTY(&pData->lstLastSVIDWantsHead))
        {
            FreeScidSvidList(&pData->lstLastSVIDWantsHead);
        }
        CopyScidSvidList(&pData->lstAdminSVIDWantsHead, &pData->lstLastSVIDWantsHead);
        /* s-channel request; */
        SetSVIDRequest(pData->ucOperRole, &pData->lstAdminSVIDWantsHead, &pData->lstOperSVIDListHead);

        return ;
    }

    struct lldp_module *CdcpRegister(void)
    {
        struct lldp_module *pMod = NULL;
        struct tagCdcpUserData *pUserdata = NULL;

        pMod = (struct lldp_module *)GLUE_ALLOC(sizeof(struct lldp_module));
        if (!pMod) {
            EVB_LOG(EVB_LOG_ERROR, "Failed to GLUE_ALLOC LLDP_CDCP module data.");
            return NULL;
        }

        pUserdata = (struct tagCdcpUserData *)GLUE_ALLOC(sizeof(struct tagCdcpUserData));
        if (!pUserdata) {
            GLUE_FREE(pMod);
            pMod = NULL;

            EVB_LOG(EVB_LOG_ERROR, "Failed to GLUE_ALLOC LLDP_CDCP user data.");
            return NULL;
        }
        MEMSET(pUserdata, 0, sizeof(struct tagCdcpUserData));

        LIST_INIT(&pUserdata->head);

        pMod->id   = LLDP_MOD_CDCP;
        pMod->ops  = &cdcp_ops;
        pMod->data = pUserdata;

        EVB_LOG(EVB_LOG_INFO, "cdcp_register: LLDP_CDCP module register SUCCESS.\n");
        return pMod;
    }

    void CdcpFreeUserData(struct tagCdcpUserData *pUserData)
    {
        struct tagCdcpRunningData *pCdcpRunningData = NULL;
        if (pUserData) {
            while (!LIST_EMPTY(&pUserData->head))
            {
                pCdcpRunningData = LIST_FIRST(&pUserData->head);
                LIST_REMOVE(pCdcpRunningData, lstEntry);
                GLUE_FREE(pCdcpRunningData);
                pCdcpRunningData = NULL;
            }
        }

        return ;
    }

    void CdcpUnregister(struct lldp_module *pMod)
    {
        if(NULL == pMod)
        {
            return ;
        }

        if (pMod->data) 
        {
            CdcpFreeUserData((struct tagCdcpUserData *)pMod->data);
            GLUE_FREE(pMod->data);
        }
        GLUE_FREE(pMod);
        pMod = NULL;

        return ;
    }

    static struct packed_tlv *BuildCdcpTlv(struct lldp_port *pPort)
    {
        TCdcpTlv *pCdcpTlv = NULL;
        struct unpacked_tlv *pUnpackedTlv = NULL;
        struct packed_tlv *pPackedTlv = NULL;
        struct tagCdcpRunningData *pCdcpRunningData = NULL;
        struct tagCdcpScidSvidPair *pScidSvid = NULL;
        TScidSvid *pTlvScidSvid = NULL;
        u32 uiTlvLen = 0;
        int32 iListCount = 0;
        int32 iTlvSize = 0;

        if(NULL == pPort)
        {
            EVB_LOG(EVB_LOG_ERROR, "BuildCdcpTlv: Input param pPort == NULL.\n");
            return NULL;
        }

        pCdcpRunningData = GetCdcpRunningData(pPort->local_port);
        if(NULL == pCdcpRunningData)
        {
            EVB_LOG(EVB_LOG_ERROR, "BuildCdcpTlv: GetCdcpRunningData == NULL.\n");
            return NULL;
        }

        if(pCdcpRunningData->wAdminChnCap == 0 || 
           pCdcpRunningData->wOperChnCap == 0 ||
           pCdcpRunningData->ucAdminRole != pCdcpRunningData->ucOperRole || 
           (pCdcpRunningData->ucOperRole != CDCP_B && pCdcpRunningData->ucOperRole != CDCP_S))
        {
            return NULL;
        }

        if(pCdcpRunningData->wOperChnCap > 0x0FFF)
        {
            EVB_LOG(EVB_LOG_ERROR, "BuildCdcpTlv: S-channel number %d go over the top[0x0FFF].\n", pCdcpRunningData->wOperChnCap);
            return NULL;
        }

        /* 必须保证SChnCap通道个数与链表中的元素个数是相等的????; 
         * 若要考虑性能，可以在进行队列创建的时候进行这项保证验证，这样此处判断可以删除;
         * 在bridge需要仔细考虑wOperChnCap与RemoteChnCap和AdminChnCap的赋值关系.
         * 在Bridge上它是Min(RemoteChnCap,AdminChnCap)与lstOperSVIDListHead链中个数不相等;
         */
        iListCount = GetScidSvidListCount(&pCdcpRunningData->lstOperSVIDListHead);
        if(iListCount > pCdcpRunningData->wOperChnCap)
        {
            EVB_LOG(EVB_LOG_ERROR, "BuildCdcpTlv:wOperChnCap[%d] is not equal list count[%d].\n", 
                pCdcpRunningData->wOperChnCap, iListCount);
            return NULL;
        }
        if(0 == iListCount)
        {
            return NULL;
        }

        /* alloc unpacked tlv memory space; */
        pUnpackedTlv = create_tlv();
        if (NULL == pUnpackedTlv)
        {
            EVB_LOG(EVB_LOG_ERROR, "BuildCdcpTlv: Failed to GLUE_ALLOC pUnpackedTlv tlv.\n");
            return NULL;
        }

        /* alloc packed tlv memory space; */
        pPackedTlv = create_ptlv();
        if(NULL == pPackedTlv)
        {
            EVB_LOG(EVB_LOG_ERROR, "BuildCdcpTlv: Failed to GLUE_ALLOC pPackedTlv tlv.\n");
            pUnpackedTlv = free_unpkd_tlv(pUnpackedTlv);
            return NULL;
        }

        uiTlvLen = sizeof(TCdcpTlv) + iListCount * sizeof(TScidSvid);
        pCdcpTlv = (TCdcpTlv *)GLUE_ALLOC(uiTlvLen);
        if(NULL == pCdcpTlv) 
        {
            EVB_LOG(EVB_LOG_ERROR, "BuildCdcpTlv: Failed to GLUE_ALLOC cdcp tlv.\n");
            pUnpackedTlv = free_unpkd_tlv(pUnpackedTlv);
            pPackedTlv = free_pkd_tlv(pPackedTlv);
            return NULL;
        }
        MEMSET(pCdcpTlv, 0, uiTlvLen);

        /* 组织cdcp 报文; */
        hton24(pCdcpTlv->ucOUI, OUI_IEEE_8021);
        pCdcpTlv->ucSubType = LLDP_CDCP_SUBTYPE;

        pCdcpTlv->bitRole = (CDCP_S == pCdcpRunningData->ucOperRole) ? 1 : 0;
        pCdcpTlv->bitSComp = (true == pCdcpRunningData->bSCompEnable) ? 1 : 0;
        
        pCdcpTlv->bitChnCap = (pCdcpRunningData->wOperChnCap) & 0x0FFF; 
        pTlvScidSvid = (TScidSvid *)(pCdcpTlv+1);
        LIST_FOREACH(pScidSvid, &pCdcpRunningData->lstOperSVIDListHead, lstEntry)
        {
            /* pCdcpTlv->atScidSvid需要网络序; */;            
            pTlvScidSvid->aucScidSvid[0]  = (u8)(pScidSvid->wScid & 0x00FF);
            pTlvScidSvid->aucScidSvid[1]  = (u8)(pScidSvid->wSvid & 0x00F);
            pTlvScidSvid->aucScidSvid[1] |= (u8)((pScidSvid->wScid >> 8) & 0x00F);
            pTlvScidSvid->aucScidSvid[2]  = (u8)((u8)(pScidSvid->wSvid >> 4) & 0x00FF);
            ++pTlvScidSvid;
        }

        pUnpackedTlv->type = ORG_SPECIFIC_TLV;
        pUnpackedTlv->length = uiTlvLen;
        pUnpackedTlv->info = (u8 *)pCdcpTlv;

        if (OUI_SUB_SIZE > pUnpackedTlv->length)
        {
            EVB_LOG(EVB_LOG_ERROR, "BuildCdcpTlv: cdcp tlv packet len error.\n");
            pUnpackedTlv = free_unpkd_tlv(pUnpackedTlv);
            pPackedTlv = free_pkd_tlv(pPackedTlv);
            return NULL;            
        }

        /* 组织为packed tlv; */
        iTlvSize = TLVSIZE(pUnpackedTlv);
        pPackedTlv->tlv = (u8 *)GLUE_ALLOC(iTlvSize);
        if (NULL == pPackedTlv->tlv)
        {
            EVB_LOG(EVB_LOG_ERROR, "BuildCdcpTlv: cdcp tlv packet alloc memory failed.\n");
            pUnpackedTlv = free_unpkd_tlv(pUnpackedTlv);
            pPackedTlv = free_pkd_tlv(pPackedTlv);
            return NULL;
        }

        pPackedTlv->size = 0;
        PACK_TLV_AFTER(pUnpackedTlv, pPackedTlv, iTlvSize, CDCP_BLD_TLV_ERR);    
        pUnpackedTlv = free_unpkd_tlv(pUnpackedTlv);
        return pPackedTlv; 

CDCP_BLD_TLV_ERR:
        pUnpackedTlv = free_unpkd_tlv(pUnpackedTlv);
        pPackedTlv = free_pkd_tlv(pPackedTlv);
        return NULL;  
    }

    bool UnpackCdcpTlv(struct lldp_port *pPort, 
                       struct unpacked_tlv *pTlv, 
                       bool *pIsStoredTlv,
                       struct tagCdcpRunningData *pCdcpRunningData, 
                       struct tagEvbPort *pEvbPort)
    {
        TCdcpTlv *pCdcpTlv = NULL;
        struct tagCdcpScidSvidPair *pScidSvid = NULL;
        struct tagCdcpScidSvidPair *pPrevPair = NULL;
        TScidSvid *pTlvScidSvid = NULL;
        int32 iListCount = 0;
        int32 iListIdx = 0;

        *pIsStoredTlv = false;

        if (NULL == pPort || NULL == pTlv || 
            NULL == pIsStoredTlv || NULL == pCdcpRunningData || 
            NULL == pEvbPort)
        {
            EVB_ASSERT(0);
            return false;
        }

        pCdcpTlv = (TCdcpTlv *)pTlv->info;
        if(NULL == pCdcpTlv)
        {
            EVB_PKT_COUNT_INC(pEvbPort->pStt->ullCdcpRcvErr);
            return false;
        }

        if(!(LLDP_CDCP_SUBTYPE == pCdcpTlv->ucSubType))
        {            
            return SUBTYPE_INVALID;
        }

        if(NULL != pCdcpRunningData->pRxTlv)
        {
            EVB_ASSERT(0);
            pCdcpRunningData->pRxTlv = free_unpkd_tlv(pCdcpRunningData->pRxTlv);
        }
        pCdcpRunningData->pRxTlv = pTlv;

        if(g_iOpenRcvPktPrint)
        {
            PRINTF("\nRcv CDCP packet:\n");
            DbgMemShow(pTlv->info, pTlv->length);
        }

        EVB_PKT_COUNT_INC(pEvbPort->pStt->ullCdcpRcv);

        pCdcpRunningData->ucRemoteRole = (1 == pCdcpTlv->bitRole) ? CDCP_S : CDCP_B;
        pCdcpRunningData->bRemoteSCompEnable = (1 == pCdcpTlv->bitSComp) ? true : false;
        pCdcpRunningData->wRemoteChnCap = pCdcpTlv->bitChnCap;

        if(pCdcpRunningData->wRemoteChnCap > 0x0FFF)
        {
            EVB_PKT_COUNT_INC(pEvbPort->pStt->ullCdcpRcvErr);
            return false;
        }
        if(!LIST_EMPTY(&pCdcpRunningData->lstRemoteSVIDListHead))
        {
            FreeScidSvidList(&pCdcpRunningData->lstRemoteSVIDListHead);
            LIST_INIT(&pCdcpRunningData->lstRemoteSVIDListHead);
        }
        
        if((pTlv->length - sizeof(TCdcpTlv)) % sizeof(TScidSvid))
        {
            EVB_PKT_COUNT_INC(pEvbPort->pStt->ullCdcpRcvLenErr);
            return false;
        }

        iListCount = (pTlv->length - sizeof(TCdcpTlv)) / sizeof(TScidSvid);
        pTlvScidSvid = (TScidSvid *)(pCdcpTlv+1);
        pPrevPair = NULL;
        for(iListIdx = 0; iListIdx < iListCount; ++iListIdx)
        {
            u16 wScid = 0;
            u16 wSvid = 0;
            u16 wTmp = 0;
            wTmp = (pTlvScidSvid->aucScidSvid[1] >> 4) & 0x000F;
            wScid = (wTmp << 8) | pTlvScidSvid->aucScidSvid[0];
            wTmp = (pTlvScidSvid->aucScidSvid[2] << 4) & 0xFFF0;
            wSvid = wTmp | (pTlvScidSvid->aucScidSvid[1] & 0x000F);

            pScidSvid = (struct tagCdcpScidSvidPair *)GLUE_ALLOC(sizeof(struct tagCdcpScidSvidPair));
            if(NULL == pScidSvid)
            {
                EVB_PKT_COUNT_INC(pEvbPort->pStt->ullCdcpAllocRcvMemErr);
                return false;
            }
            MEMSET(pScidSvid, 0x0, sizeof(struct tagCdcpScidSvidPair));

            pScidSvid->wScid = wScid;
            pScidSvid->wSvid = wSvid;
            if(NULL == pPrevPair)
            {
                LIST_INSERT_HEAD(&pCdcpRunningData->lstRemoteSVIDListHead, pScidSvid, lstEntry);
            }
            else
            {
                LIST_INSERT_AFTER(pPrevPair, pScidSvid, lstEntry);
            }
            pPrevPair = pScidSvid;
            ++pTlvScidSvid;
        }

        *pIsStoredTlv = true;

        return true;
    }

    struct packed_tlv *CdcpGetTlv(struct lldp_port *pPort)
    {
        struct packed_tlv *ptlv = NULL;

        if (!is_tlv_enabled(pPort->local_port, TLVID_8021Qbg(LLDP_CDCP_SUBTYPE)))    
        {
            return NULL;            
        }

        /* Build TLVs */
        ptlv = BuildCdcpTlv(pPort);

        if(g_iOpenSendPktPrint && ptlv)
        {
            PRINTF("\nSendPkt:\n");
            DbgMemShow(ptlv->tlv, ptlv->size);
        }

        return ptlv;
    }

    int CdcpRChange(struct lldp_port *pPort,  struct unpacked_tlv *pTlv, bool *pIsStoredTlv)
    {
        u8 oui[OUI_SIZE] = INIT_IEEE8021_OUI;
        struct tagCdcpRunningData *pCdcpRunningData;
        struct tagEvbPort *pEvbPort = NULL;

        if(NULL == pPort)
        {
            EVB_ASSERT(0);
            return SUBTYPE_INVALID;
        }

        pEvbPort = GetEvbPort(pPort->local_port);
        if(NULL == pEvbPort || NULL == pEvbPort->pStt)
        {
            EVB_ASSERT(0);
            return SUBTYPE_INVALID;
        }

        pCdcpRunningData = GetCdcpRunningData(pPort->local_port);
        if (NULL == pCdcpRunningData)
        {
            EVB_PKT_COUNT_INC(pEvbPort->pStt->ullCdcpModInitErr);
            return SUBTYPE_INVALID;
        }

        /* TYPE_1: Beginning of a IEEE8021Qbg PDU. */
        if(TYPE_1 == pTlv->type) 
        {
            if(NULL != pCdcpRunningData->pRxTlv)
            {
                pCdcpRunningData->pRxTlv = free_unpkd_tlv(pCdcpRunningData->pRxTlv);
            }
        }

        /* TYPE_0: End Of LLDPDU; */
        if(pTlv->type == TYPE_0) 
        {
            if(NULL != pCdcpRunningData->pRxTlv)
            {
                RunCdcpSM(pPort);
                pCdcpRunningData->pRxTlv = free_unpkd_tlv(pCdcpRunningData->pRxTlv);
            }
        } 

        /* 其他模块处理过的报文忽略; */
        if(true == *pIsStoredTlv)
        {
            return TLV_OK;
        }   

        /*
        * TYPE_127 is for the Organizationally Specific TLVS
        * More than 1 of this type is allowed.
        */
        if (ORG_SPECIFIC_TLV == pTlv->type) 
        {
            int res;

            if (pTlv->length < (OUI_SUB_SIZE))
            {
                EVB_PKT_COUNT_INC(pEvbPort->pStt->ullCdcpRcvLenErr);
                pCdcpRunningData->pRxTlv = NULL;
                return TLV_ERR;
            }

            if (MEMCMP(pTlv->info, &oui, OUI_SIZE) != 0)
            {
                EVB_PKT_COUNT_INC(pEvbPort->pStt->ullCdcpRcvTypeErr);
                return SUBTYPE_INVALID;
            }

            if (!is_tlv_enabled(pPort->local_port, TLVID_8021Qbg(LLDP_CDCP_SUBTYPE)))    
            {
                return TLV_OK;            
            }

            res = UnpackCdcpTlv(pPort, pTlv, pIsStoredTlv, pCdcpRunningData, pEvbPort);
            if (!res)
            {         
                pCdcpRunningData->pRxTlv = NULL;
                return TLV_ERR;
            }

            return TLV_OK;
        }

        return TLV_OK;
    }

    u8 CdcpMibDeleteObjects(struct lldp_port *pPort)
    {
        struct tagCdcpRunningData *pData = NULL;

        if(NULL == pPort)
        {
            EVB_ASSERT(0);
            return -1;
        }

        pData = GetCdcpRunningData(pPort->local_port);
        if (NULL == pData)
        {
            EVB_LOG(EVB_LOG_ERROR, "CdcpMibDeleteObjects: protocol running data @[%d]port is not exists\n", pPort->local_port);
            return -1;
        }

        ClearCdcpRunningData(pData);

        EVB_LOG(EVB_LOG_WARN, "CdcpMibDeleteObjects:[%d]port MIB is deleted.\n", pPort->local_port);

        return 0;
    }

    int32 CdcpCheckPending(struct lldp_port *pPort)
    {
        struct tagCdcpRunningData *pData = NULL;

        if(NULL == pPort)
        {
            EVB_ASSERT(0);
            return -1;
        }

        pData = GetCdcpRunningData(pPort->local_port);
        if (NULL == pData)
        {
            EVB_LOG(EVB_LOG_ERROR, "CdcpCheckPending: protocol running data @[%d]port is not exists\n", pPort->local_port);
            return -1;
        }
        
        if(NULL == pData->pRxTlv ||
           (pData->ucOperRole != CDCP_B && pData->ucOperRole != CDCP_S))
        {
            return -1;
        }

        /* todp: 需要加入判断配置变更条件; */
        RunCdcpSM(pPort);

        return 0;
    }

#ifdef __cplusplus
}
#endif
