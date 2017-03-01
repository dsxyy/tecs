#ifdef __cplusplus
extern "C" {
#endif

#include "evb_comm.h"
#include "evb_port_mgr.h"
#include "wpcap_drv.h"
#include "lnxrsock_drv.h"

    LIST_HEAD(tagEvbIssPortListHead, tagEvbIssPort) s_tEvbIssPortListHead;

#define EVB_ISS_PORT_HASH_COUNT (1023)

    static struct tagEvbIssPort *s_atEvbIssPortHash[EVB_ISS_PORT_HASH_COUNT];

    struct tagEvbPortDriver s_tEvbIssPortDrv = {NULL, NULL, NULL, NULL,NULL};

    static int32 InsertIssPort2HashArray(struct tagEvbIssPort *pPort)
    {
        int32 nHashVal = -1;
        struct tagEvbIssPort *pTmp = NULL;

        if(NULL == pPort)
        {
            EVB_ASSERT(0);
            return -1;
        }

        nHashVal = pPort->uiPortNo % EVB_ISS_PORT_HASH_COUNT;

        if(NULL == s_atEvbIssPortHash[nHashVal])
        {
            s_atEvbIssPortHash[nHashVal] = pPort;
        }
        else
        {
            pTmp = s_atEvbIssPortHash[nHashVal];
            while (pTmp->pPortHashNext != NULL)
            {
                pTmp = pTmp->pPortHashNext;
            }
            pTmp->pPortHashNext= pPort;
            pPort->pPortHashNext = NULL;
        }

        return 0;
    }

    static int32 DeleteIssPortFromHashArray(struct tagEvbIssPort *pPort)
    {
        int32 nHashVal = -1;
        struct tagEvbIssPort *pTmp = NULL;
        struct tagEvbIssPort *pPrev = NULL;

        if(NULL == pPort)
        {
            EVB_ASSERT(0);
            return -1;
        }

        nHashVal = pPort->uiPortNo % EVB_ISS_PORT_HASH_COUNT;
        pTmp = s_atEvbIssPortHash[nHashVal];
        if(NULL == pTmp)
        {
            EVB_LOG(EVB_LOG_WARN, 
                "DeleteIssPortFromHashArray: Delete EVB port [Port No. %d] failed, not in the hash array.\n", 
                pPort->uiPortNo);
            return -1;
        }

        if(pTmp->uiPortNo == pPort->uiPortNo)
        {
            if(NULL == pTmp->pPortHashNext)
            {
                s_atEvbIssPortHash[nHashVal] = NULL;
            }
            else
            {
                s_atEvbIssPortHash[nHashVal] = pTmp->pPortHashNext;
            }

            return 0;
        }

        pPrev = pTmp;
        pTmp = pTmp->pPortHashNext;
        while(pTmp)
        {
            if(pTmp->uiPortNo == pPort->uiPortNo)
            {
                pPrev->pPortHashNext = pTmp->pPortHashNext;
                break;
            }
            pPrev = pTmp;
            pTmp = pTmp->pPortHashNext;
        }

        return 0;
    }


    struct tagEvbIssPort *Get1stEvbIssPort(void)
    {
        return LIST_EMPTY(&s_tEvbIssPortListHead) ? NULL : LIST_FIRST(&s_tEvbIssPortListHead);
    }

    struct tagEvbIssPort *GetNextEvbIssPort(struct tagEvbIssPort *pListPort)
    {
        if(NULL == pListPort)
        {
            EVB_ASSERT(0);
            return NULL;
        }
        return LIST_NEXT(pListPort, lstEntry);
    }

    int32 InitEvbIssPortModule(void)
    {
        LIST_INIT(&s_tEvbIssPortListHead);
        MEMSET(s_atEvbIssPortHash, 0x0, sizeof(s_atEvbIssPortHash));
        MEMSET(&s_tEvbIssPortDrv, 0x0, sizeof(s_tEvbIssPortDrv));

#ifdef _WIN32        
        if(0 != InitWpcapDev())
        {
            EVB_LOG(EVB_LOG_ERROR, "InitEvbPortModule: call InitWpcapDev failed.\n");
            return -1;
        }
        s_tEvbIssPortDrv.pInit = WpcapInitEvbPort;
        s_tEvbIssPortDrv.pRead = WpcapRcvPkt;
        s_tEvbIssPortDrv.pWrite = WpcapSendPkt;
        s_tEvbIssPortDrv.pDestroy = WpcapDestroyEvbPort;
        s_tEvbIssPortDrv.pFreePkt = WpcapFreePkt;
#endif
        /* For linux raw socket Or Soda Queue; */
#ifdef _EVB_TECS 
        if(0 != InitLnxRsockDrv())
        {
            EVB_LOG(EVB_LOG_ERROR, "InitEvbPortModule: call InitLnxRsockDrv failed.\n");
            return -1;
        }
        s_tEvbIssPortDrv.pInit = LnxRsockInitEvbPort;
        s_tEvbIssPortDrv.pRead = LnxRsockRcvPkt;
        s_tEvbIssPortDrv.pWrite = LnxRsockSendPkt;
        s_tEvbIssPortDrv.pDestroy = LnxRsockDestroyEvbPort;
        s_tEvbIssPortDrv.pFreePkt = LnxRsockFreePkt;

#endif
        return 0;
    }

    struct tagEvbIssPort *GetEvbIssPort(u32 uiPortNo)
    {
        struct tagEvbIssPort *pTmp = NULL;
        int32 nHashVal = -1;

        nHashVal = uiPortNo % EVB_ISS_PORT_HASH_COUNT;
        pTmp = s_atEvbIssPortHash[nHashVal];

        while(pTmp)
        {
            if(uiPortNo == pTmp->uiPortNo)
            {
                return pTmp;
            }
            pTmp = pTmp->pPortHashNext;
        }

        return NULL;
    }

    struct tagEvbIssPort *CreateEvbIssPort(u32 uiPortNo, const char *pPortName, u8 *pMac)
    {
        struct tagEvbIssPort *pPort = NULL;

        if(NULL == pMac || NULL == pPortName)
        {
            EVB_ASSERT(0);
            return NULL;
        }

        /* 端口编号判断; */
        if(EVB_INVALID_PORT_NO == uiPortNo)
        {
            EVB_LOG(EVB_LOG_ERROR, "CreateEvbIssPort: Create EVB port [Name: %s] failed, uiPortNo is EVB_INVALID_PORT_NO .\n", pPortName);
            return NULL;
        }

        /* 避免重复创建; */
        if(NULL != GetEvbIssPort(uiPortNo))
        {
            EVB_LOG(EVB_LOG_ERROR, "CreateEvbIssPort: Create EVB port [Name: %s] failed, the port is existed in ISS port list.\n", pPortName);
            return NULL;
        }
        /* 创建端口; */
        pPort = (struct tagEvbIssPort *)GLUE_ALLOC(sizeof(struct tagEvbIssPort));
        if(NULL == pPort)
        {
            EVB_LOG(EVB_LOG_ERROR, "CreateEvbIssPort: Create EVB port [Name: %s] failed, not enough memory.\n", pPortName);
            return NULL;
        }
        MEMSET(pPort, 0x0, sizeof(struct tagEvbIssPort));

        pPort->pStt = (TEvbPktStt *)GLUE_ALLOC(sizeof(TEvbPktStt));
        if(NULL == pPort->pStt)
        {
            EVB_LOG(EVB_LOG_ERROR, "CreateEvbIssPort: Create EVB port [Name: %s] failed, not enough memory for packet statistics.\n", 
                pPortName);
            GLUE_FREE(pPort);
            pPort = NULL;
            return NULL;
        }
        MEMSET(pPort->pStt, 0x0, sizeof(TEvbPktStt));

        pPort->uiPortNo = uiPortNo;
        MEMCPY(pPort->aucMac, pMac, EVB_PORT_MAC_LEN);

        sprintf(pPort->acName, "%s", pPortName);

        pPort->ucPortState = EVB_PORT_STATE_INIT;

        LIST_INSERT_HEAD(&s_tEvbIssPortListHead, pPort, lstEntry);
        InsertIssPort2HashArray(pPort);

        /* 初始化驱动; */
        pPort->pDriver = &s_tEvbIssPortDrv;
        if(0 != pPort->pDriver->pInit(uiPortNo))
        {
            DeleteEvbIssPort(uiPortNo);
            EVB_LOG(EVB_LOG_ERROR, "CreateEvbIssPort: Create EVB port [Name: %s] failed, init driver error.\n", pPortName);
            return NULL;
        }

        return pPort;
    }

    int32 DeleteEvbIssPort(u32 uiPortNo)
    {
        struct tagEvbIssPort *pPort = NULL;
        LIST_FOREACH(pPort, &s_tEvbIssPortListHead, lstEntry)
        {
            if(pPort->uiPortNo == uiPortNo)
            {
                LIST_REMOVE(pPort, lstEntry);
                if(pPort->pDriver && pPort->pDriver->pDestroy)
                {
                    pPort->pDriver->pDestroy(pPort->uiPortNo);
                }

                DeleteIssPortFromHashArray(pPort);

                if(pPort->pStt)
                {
                    GLUE_FREE(pPort->pStt);
                    pPort->pStt = NULL;
                }
                else
                {
                    EVB_ASSERT(0);
                }

                GLUE_FREE(pPort);
                pPort = NULL;

                break;
            }
        }

        return 0;
    }

    int32 DestroyEvbIssPortModule(void)
    {
        struct tagEvbIssPort *pPort = NULL;

        while (!LIST_EMPTY(&s_tEvbIssPortListHead))
        {
            pPort = LIST_FIRST(&s_tEvbIssPortListHead);
            LIST_REMOVE(pPort, lstEntry);
            if(pPort->pDriver && pPort->pDriver->pDestroy)
            {
                pPort->pDriver->pDestroy(pPort->uiPortNo);
            }

            DeleteIssPortFromHashArray(pPort);

            if(pPort->pStt)
            {
                GLUE_FREE(pPort->pStt);
                pPort->pStt = NULL;
            }
            else
            {
                EVB_ASSERT(0);
            }

            GLUE_FREE(pPort);
            pPort = NULL;
        }

#ifdef _WIN32   
        MEMSET(&s_tEvbIssPortDrv, 0x0, sizeof(s_tEvbIssPortDrv));
        if(0 != CloseWpcapDev())
        {
            EVB_LOG(EVB_LOG_ERROR, "DestroyEvbPortModule: call CloseWpcapDev failed.\n");
            return -1;
        }
#endif 

        /* For linux raw socket Or Soda Queue; */
#ifdef _EVB_TECS 
        MEMSET(&s_tEvbIssPortDrv, 0x0, sizeof(s_tEvbIssPortDrv));
        if(0 != CloseLnxRsockDrv())
        {
            EVB_LOG(EVB_LOG_ERROR, "DestroyEvbPortModule: call CloseLnxRsockDrv failed.\n");
            return -1;
        } 
#endif
        return 0;
    }

    int32 SetEvbPortIssStat(u32 uiPortNo, u8 ucPortState)
    {
        struct tagEvbIssPort *pPort = NULL;

        if (EVB_PORT_STATE_INIT != ucPortState && 
            EVB_PORT_STATE_DW != ucPortState &&
            EVB_PORT_STATE_UP != ucPortState)
        {
            EVB_ASSERT(0);
            return -1;
        }

        pPort = GetEvbIssPort(uiPortNo);
        if(NULL == pPort)
        {
            EVB_LOG(EVB_LOG_WARN, "SetEvbPortIssStat: the port[PortNo: %d] is not created.\n", uiPortNo);
            return -1;
        }

        if(pPort->ucPortState == ucPortState)
        {
            EVB_LOG(EVB_LOG_WARN, "SetEvbPortIssStat: Repeat settings[PortNo: %d, State: %d]\n", uiPortNo, ucPortState);
            return 0;
        }

        pPort->ucPortState = ucPortState;

        return 0;
    }

#ifdef __cplusplus
}
#endif

