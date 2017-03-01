#ifdef __cplusplus
extern "C" {
#endif

#include "evb_comm.h"
#include "evb_port_mgr.h"
#include "evb_cfg.h"
#include <time.h>

    LIST_HEAD(tagEvbPortListHead, tagEvbPort) s_tEvbPortListHead;

#define EVB_PORT_HASH_COUNT (1023)
#define EVB_PORT_MAX_NUM    (4096)

    static struct tagEvbPort *s_atEvbPortHash[EVB_PORT_HASH_COUNT];

    static u8 *s_pEvbPortNoState = NULL; /* 使用bit来记录端口号的分配情况，s_pEvbPortNoState:[uiPort-1] : 1表示占用， 0表示未占用; */
    static u32 s_uiPortNoArrySize = 0;

    static const char *pcstrEvbPortType[EM_EVB_PT_ALL] = 
    {
        "VBP",
        "PNP",
        "CNP",
        "CEP",
        "CBP",
        "VIP",
        "DBP",
        "RCAP",
        "SBP",
        "UAP",
        "URP"
    };

    static u32 AllocEvbPortNo(u8 ucEvbPortType, struct tagEvbIssPort *pIssPort)
    {
        u32 uiPortNo = EVB_INVALID_PORT_NO;
        u8 *pucData = NULL;
        u32 i = 0;
        u8 j = 0;

        if(NULL == s_pEvbPortNoState)
        {
            EVB_ASSERT(0);
            return EVB_INVALID_PORT_NO;
        }

        for(i = 0, pucData = s_pEvbPortNoState; i < s_uiPortNoArrySize; i++, pucData++)
        {
            if(*pucData != 0xFF)
            {
                u8 ucTmp = 0x80;
                for(j = 0; j < 8; j++)
                {
                    if(0 == (ucTmp & (*pucData)))
                    {
                        *pucData = (*pucData) | ucTmp;
                        break;
                    }
                    ucTmp = ucTmp >> 1;
                }
                uiPortNo = i * 8 + j + 1;
                break;
            }
        }

        return uiPortNo;
    }

    static int32 FreeEvbPortNo(u32 uiPortNo)
    {
        u8 *pucData = NULL;
        u32 i = 0;
        u8 j = 0;
        u32 uiIdx = 0;
        u8 ucMask = 0;

        if(NULL == s_pEvbPortNoState)
        {
            EVB_ASSERT(0);
            return -1;
        }

        if(EVB_INVALID_PORT_NO == uiPortNo)
        {
            EVB_LOG(EVB_LOG_ERROR, 
                "FreeEvbPortNo: EVB_INVALID_PORT_NO == uiPortNo.\n");
            return -1;
        }

        uiIdx = uiPortNo - 1;
        i = uiIdx >> 3;
        j = uiIdx % 8;
        if(i >= s_uiPortNoArrySize)
        {
            EVB_LOG(EVB_LOG_ERROR, 
                "FreeEvbPortNo: (uiPortNo - 1) >> 3 >= s_uiPortNoArrySize[%d].\n", 
                uiPortNo, s_uiPortNoArrySize);
            return -1;
        }

        pucData = s_pEvbPortNoState;
        pucData = pucData + i;
        
        ucMask = 0x80 >> j;
        ucMask = ~ucMask;
        *pucData = (*pucData) & ucMask;

        return 0;
    }

    static int32 InsertEvbPort2HashArray(struct tagEvbPort *pPort)
    {
        int32 nHashVal = -1;
        struct tagEvbPort *pTmp = NULL;

        if(NULL == pPort)
        {
            EVB_ASSERT(0);
            return -1;
        }

        nHashVal = pPort->uiPortNo % EVB_PORT_HASH_COUNT;

        if(NULL == s_atEvbPortHash[nHashVal])
        {
            s_atEvbPortHash[nHashVal] = pPort;
        }
        else
        {
            pTmp = s_atEvbPortHash[nHashVal];
            while (pTmp->pPortHashNext != NULL)
            {
                pTmp = pTmp->pPortHashNext;
            }
            pTmp->pPortHashNext= pPort;
            pPort->pPortHashNext = NULL;
        }

        return 0;
    }

    static int32 DeleteEvbPortFromHashArray(struct tagEvbPort *pPort)
    {
        int32 nHashVal = -1;
        struct tagEvbPort *pTmp = NULL;
        struct tagEvbPort *pPrev = NULL;

        if(NULL == pPort)
        {
            EVB_ASSERT(0);
            return -1;
        }

        nHashVal = pPort->uiPortNo % EVB_PORT_HASH_COUNT;
        pTmp = s_atEvbPortHash[nHashVal];
        if(NULL == pTmp)
        {
            EVB_LOG(EVB_LOG_WARN, 
                "DeleteEvbPortFromHashArray: Delete EVB port [Port No. %d] failed, not in the hash array.\n", 
                pPort->uiPortNo);
            return -1;
        }

        if(pTmp->uiPortNo == pPort->uiPortNo)
        {
            if(NULL == pTmp->pPortHashNext)
            {
                s_atEvbPortHash[nHashVal] = NULL;
            }
            else
            {
                s_atEvbPortHash[nHashVal] = pTmp->pPortHashNext;
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

    struct tagEvbPort *Get1stEvbPort(void)
    {
        return LIST_EMPTY(&s_tEvbPortListHead) ? NULL : LIST_FIRST(&s_tEvbPortListHead);
    }

    struct tagEvbPort *GetNextEvbPort(struct tagEvbPort *pListPort)
    {
        if(NULL == pListPort)
        {
            EVB_ASSERT(0);
            return NULL;
        }
        return LIST_NEXT(pListPort, lstEntry);
    }

    int32 InitEvbPortModule(void)
    {
        LIST_INIT(&s_tEvbPortListHead);
        MEMSET(s_atEvbPortHash, 0x0, sizeof(s_atEvbPortHash));

        s_uiPortNoArrySize = (EVB_PORT_MAX_NUM + 7) >> 3;
        s_pEvbPortNoState = (u8 *)GLUE_ALLOC(s_uiPortNoArrySize);
        if(NULL == s_pEvbPortNoState)
        {
            EVB_LOG(EVB_LOG_ERROR, "InitEvbPortModule: init s_pEvbPortNoState failed, not enough memory.\n");
            return -1;
        }
        MEMSET(s_pEvbPortNoState, 0x0, s_uiPortNoArrySize);

        InitEvbIssPortModule();

        return 0;
    }

    struct tagEvbPort *GetEvbPort(u32 uiPortNo)
    {
        struct tagEvbPort *pTmp = NULL;
        int32 nHashVal = -1;

        nHashVal = uiPortNo % EVB_PORT_HASH_COUNT;
        pTmp = s_atEvbPortHash[nHashVal];

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

    struct tagEvbPort *CreateEvbPort(u8 ucEvbPortType, struct tagEvbIssPort *pIssPort)
    {
        struct tagEvbPort *pPort = NULL;
        u32 uiPortNo = EVB_INVALID_PORT_NO;

        if(NULL == pIssPort)
        {
            EVB_ASSERT(0);
            return NULL;
        }

        /* 端口类型判断; */
        if(ucEvbPortType >= EM_EVB_PT_ALL)
        {
            EVB_LOG(EVB_LOG_ERROR, 
                "CreateEvbPort: Create EVB port failed, ucEvbPortType [value: %d] is invalid.\n", ucEvbPortType);
            return NULL;
        }
        /* 避免重复创建; */
        for(pPort = Get1stEvbPort(); pPort; pPort = GetNextEvbPort(pPort))
        {
            if( (pPort->ucEvbPortType == ucEvbPortType) && 
                (pPort->pIssPort->uiPortNo == pIssPort->uiPortNo))
            {
                EVB_LOG(EVB_LOG_ERROR, 
                    "CreateEvbPort: Create EVB port [type: %s, ISS port No: %d] failed, the port is existed in EVB port list.\n", 
                    ucEvbPortType, pIssPort->uiPortNo);
                return NULL;
            }
        }

        /* 创建端口; */
        uiPortNo = AllocEvbPortNo(ucEvbPortType, pIssPort);
        if(EVB_INVALID_PORT_NO == uiPortNo)
        {
            EVB_LOG(EVB_LOG_ERROR, 
                "CreateEvbPort: Create EVB port failed, call AllocEvbPortNo failed.\n");
            return NULL;
        }

        pPort = (struct tagEvbPort *)GLUE_ALLOC(sizeof(struct tagEvbPort));
        if(NULL == pPort)
        {
            EVB_LOG(EVB_LOG_ERROR, "CreateEvbPort: Create EVB port failed, not enough memory.\n");
            return NULL;
        }
        MEMSET(pPort, 0x0, sizeof(struct tagEvbPort));

        /* 置上时间戳 */
        pPort->tCreateTimeStamp = time(0);

        pPort->pStt = (TEvbPktStt *)GLUE_ALLOC(sizeof(TEvbPktStt));
        if(NULL == pPort->pStt)
        {
            EVB_LOG(EVB_LOG_ERROR, "CreateEvbPort: Create EVB port failed, not enough memory for packet statistics.\n");
            GLUE_FREE(pPort);
            pPort = NULL;
            return NULL;
        }
        MEMSET(pPort->pStt, 0x0, sizeof(TEvbPktStt));

        pPort->ucEvbPortType = ucEvbPortType;
        pPort->pIssPort = pIssPort;
        sprintf(pPort->acName, "%s%d", pcstrEvbPortType[ucEvbPortType], uiPortNo);
        pPort->uiPortNo = uiPortNo;

        switch(ucEvbPortType)
        {
        case EM_EVB_PT_UAP:
            {
                TEvbUapAttr *pUapAttr = NULL;
                pPort->ptPortAttr = GLUE_ALLOC(sizeof(TEvbUapAttr));
                if(NULL == pPort->ptPortAttr)
                {
                    EVB_LOG(EVB_LOG_ERROR, "CreateEvbPort: Create EVB port [Name: %s] failed, not enough memory for TEvbUapAttr.\n", 
                        pPort->acName);
                    goto CRT_EVB_PT_ERR;
                }
                MEMSET(pPort->ptPortAttr, 0x0, sizeof(TEvbUapAttr));
                pUapAttr = (TEvbUapAttr *)pPort->ptPortAttr;
                /* default value; */
                pUapAttr->bSchCdcpAdminEnable = TRUE;
                pUapAttr->emSchCdcpAdminRoleEnumerated = (EVBSYS_TYPE_STATION == EvbcfgGetSysType()) ?  CDCP_S : CDCP_B;
                pUapAttr->wSchCdcpAdminChnCap = 1;
                break;
            }

        case EM_EVB_PT_URP:
            {
                TEvbUrpAttr *pUrpAttr = NULL;
                pPort->ptPortAttr = GLUE_ALLOC(sizeof(TEvbUrpAttr));
                if(NULL == pPort->ptPortAttr)
                {
                    EVB_LOG(EVB_LOG_ERROR, "CreateEvbPort: Create EVB port [Name: %s] failed, not enough memory for TEvpUrpAttr.\n", 
                        pPort->acName);
                    goto CRT_EVB_PT_ERR;
                }
                MEMSET(pPort->ptPortAttr, 0x0, sizeof(TEvbUrpAttr));
                pUrpAttr = (TEvbUrpAttr *)pPort->ptPortAttr;
                /* default value; */
                pUrpAttr->bSGrpID = g_evbcfg_sys.bSGrpID;
                pUrpAttr->bRRReq = g_evbcfg_sys.bRRReq;
                pUrpAttr->ucR = g_evbcfg_sys.ucR;
                pUrpAttr->ucRTE = g_evbcfg_sys.ucRTE;
                pUrpAttr->ucRKA = g_evbcfg_sys.ucRKA;
                pUrpAttr->ucRWD = g_evbcfg_sys.ucRWD;

                pUrpAttr->bLldpManual = g_evbcfg_sys.evbSysEvbLldpManual;

                break;
            }

        case EM_EVB_PT_SBP:
            {
                TEvbSbpAttr *pSbpAttr = NULL;
                pPort->ptPortAttr = GLUE_ALLOC(sizeof(TEvbSbpAttr));
                if(NULL == pPort->ptPortAttr)
                {
                    EVB_LOG(EVB_LOG_ERROR, "CreateEvbPort: Create EVB port [Name: %s] failed, not enough memory for TEvbSbpAttr.\n", 
                        pPort->acName);
                    goto CRT_EVB_PT_ERR;
                }
                MEMSET(pPort->ptPortAttr, 0x0, sizeof(TEvbSbpAttr));

                pSbpAttr = (TEvbSbpAttr *)pPort->ptPortAttr;
                /* default value; */
                pSbpAttr->bBGID = g_evbcfg_sys.bBGID;
                pSbpAttr->bRRCAP = g_evbcfg_sys.bRRCAP;
                pSbpAttr->bRRCTR = g_evbcfg_sys.bRRCTR;
                pSbpAttr->ucR = g_evbcfg_sys.ucR;
                pSbpAttr->ucRTE = g_evbcfg_sys.ucRTE;
                pSbpAttr->ucRKA = g_evbcfg_sys.ucRKA;
                pSbpAttr->ucRWD = g_evbcfg_sys.ucRWD;
                pSbpAttr->bLldpManual = g_evbcfg_sys.evbSysEvbLldpManual;
                break;
            }

        default:
            {
                EVB_ASSERT(0);
                goto CRT_EVB_PT_ERR;
            }  
        }/* end switch; */


        LIST_INSERT_HEAD(&s_tEvbPortListHead, pPort, lstEntry);
        InsertEvbPort2HashArray(pPort);

        return pPort;

CRT_EVB_PT_ERR:
        FreeEvbPortNo(uiPortNo);
        if(pPort && pPort->pStt)
        {
             GLUE_FREE(pPort->pStt);
            return NULL;
        }
        if(NULL != pPort)
        {
            GLUE_FREE(pPort);
            pPort = NULL;
        }
        return NULL;
    }

    int32 DeleteEvbPort(u32 uiPortNo)
    {
        struct tagEvbPort *pPort = NULL;

        LIST_FOREACH(pPort, &s_tEvbPortListHead, lstEntry)
        {
            if(pPort->uiPortNo == uiPortNo)
            {
                LIST_REMOVE(pPort, lstEntry);

                DeleteEvbPortFromHashArray(pPort);

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

        FreeEvbPortNo(uiPortNo);

        return 0;
    }

    int32 DestroyEvbPortModule(void)
    {
        struct tagEvbPort *pPort = NULL;

        while (!LIST_EMPTY(&s_tEvbPortListHead))
        {
            pPort = LIST_FIRST(&s_tEvbPortListHead);
            LIST_REMOVE(pPort, lstEntry);

            DeleteEvbPortFromHashArray(pPort);

            if(pPort->ptPortAttr)
            {
                GLUE_FREE(pPort->ptPortAttr);
                pPort->ptPortAttr = NULL;
            }

            if(pPort->pStt)
            {
                GLUE_FREE(pPort->pStt);
                pPort->pStt = NULL;
            }

            GLUE_FREE(pPort);
            pPort = NULL;
        }

        if(NULL != s_pEvbPortNoState)
        {
            GLUE_FREE(s_pEvbPortNoState);
            s_pEvbPortNoState = NULL;
        }
        s_uiPortNoArrySize = 0;

        return 0;
    }

    struct tagEvbIssPort *GetIssPortFromEvbPort(u32 uiEvbPortNo)
    {
        struct tagEvbPort *pEvbPort = NULL;

        pEvbPort = GetEvbPort(uiEvbPortNo);
        if(NULL == pEvbPort)
        {
            return NULL;
        }

        return pEvbPort->pIssPort;
    }

#ifdef __cplusplus
}
#endif

