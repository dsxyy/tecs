#ifdef __cplusplus
extern "C" {
#endif
#include "evb_comm.h"
#include "evb_thr.h"
#include "evb_port_mgr.h"
#include "evb_drv_thr.h"
#include "evb_lock.h"
#include "evb_entry.h"

    extern TMutex g_tMainThrdLock;
    void EvbDrvThrdEntry(void *pParam)
    {
        TEvbThrdMgr *pThrd = NULL;
        u8 *pPkt = NULL;
        u32 dwLen = 0;
        int32 nRet = 0;
        struct tagEvbIssPort *pIssPort = NULL;
        int32 nReadErr = 1;
        
        pThrd = (TEvbThrdMgr *)pParam;
        if(NULL == pThrd)
        {
            EVB_ASSERT(0);
            return ;
        }

        while(1)
        {
            if(EVB_THREAD_STOP == pThrd->wThrdFlag)
            {
                break;
            }

            nReadErr = 1;

            if(!EvbMainThreadIsWorking())
            {
                EvbDelay(20); /* delay 20 ms */
                continue;
            }

            EvbLock(&g_tMainThrdLock);

            for(pIssPort = Get1stEvbIssPort(); pIssPort; pIssPort = GetNextEvbIssPort(pIssPort))
            {
                dwLen = 0;

                if(NULL == pIssPort->pDriver)
                {
                    continue ;
                }

                nRet = pIssPort->pDriver->pRead(pIssPort->uiPortNo, &pPkt, &dwLen);
                /* Error */
                if(nRet <= 0)
                {
                    continue ;
                }

                if (NULL == pPkt)
                {
                    continue ;
                }

                if(0 == ReceivePktFromEvbIssPort(pIssPort, pPkt, dwLen))
                {
                    nReadErr = 0;
                }

                /* ÊÍ·ÅPKTÄÚ´æ */
                pIssPort->pDriver->pFreePkt(pPkt);
                
            }/* end for(pIssPort = Get1stEvbIssPort(); */

            EvbUnlock(&g_tMainThrdLock);

            if(1 == nReadErr)
            {
                EvbDelay(1); /* delay 1 ms */
            }
        }/* end while */

        return ;
    }

#ifdef __cplusplus
}
#endif

