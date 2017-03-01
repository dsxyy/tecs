#ifdef __cplusplus
extern "C" {
#endif

#include "evb_comm.h"
#include "evb_thr.h"
#include "evb_msg.h"
#include "evb_msg_def.h"
#include "evb_entry.h"
#include "evb_port_mgr.h"
#include "evb_timer.h"
#include "evb_lock.h"

#include "lldp_in.h"
#include "ecp.h"
#include "vdp.h"

    int32 InitEvbProto(void);
    int32 ShutdownEvbProto(void);

    void EvbMainThrdEntry(void *pParam)
    {
        TEvbThrdMgr *pThread = NULL;
        u32 dwMessage = EVB_INVALID_MESSAGE;
        void *pMsg = NULL;
        u32 dwMsgLen = 0;

        pThread = (TEvbThrdMgr *)pParam;
        if(NULL == pThread)
        {
            EVB_LOG(EVB_LOG_ERROR, "EvbMainEntry: input null pParam.\n");
            EVB_ASSERT(0);
            return ;
        }

        if(0 != EvbCreateThrdMsgQue(pThread))
        {
            EVB_LOG(EVB_LOG_ERROR, "EvbMainEntry: create thread's(name: %s, id: %d) msg queue failed.\n", 
                pThread->acName, pThread->tThreadID);
            return ;
        }
        InitEvbProto();
        EvbPostThrdMsg(pThread, EVB_WORKING_MSG, NULL, 0);

        while(1)
        {
            if(EVB_THREAD_STOP == pThread->wThrdFlag)
            {
                break;
            }

            dwMessage = EVB_INVALID_MESSAGE;
            pMsg = NULL;
            dwMsgLen = 0;
            if(0 != EvbGetThrdMsg(pThread, &dwMessage, &pMsg, &dwMsgLen))
            {
                continue;
            }

            if(EVB_SHUTDOWN_MSG == dwMessage)
            {
                break;
            }

            EvbMainEntry(dwMessage, pMsg, dwMsgLen);

            EvbClrThrdMsgBuff(pThread);
        }
        ShutdownEvbProto();
        
        return ;
    }

    void EvbTimerThrdEntry(void *pParam)
    {
        TEvbThrdMgr *pThread = (TEvbThrdMgr *)pParam;
        if(NULL == pThread)
        {
            EVB_LOG(EVB_LOG_ERROR, "EvbMainEntry: input null pParam.\n");
            EVB_ASSERT(0);
            return ;
        }
        
        while(1)
        {
            if(EVB_THREAD_STOP == pThread->wThrdFlag)
            {
                break;
            }

            Evb20msTimerLoop();

            EvbDelay(20);
        }
        
        return ;
    }

    /* Start timer define;各个模块的定时器后续移到自己的模块中; */
    static TEvbTimer s_tVdpTimer;
    void VdpTmOut(TEvbTimer *tm, void *cookie)
    {
        if(NULL == tm)
        {
            EVB_ASSERT(0);
            return ;
        }

        if (gtVDP.pfTmout)
        {
            gtVDP.pfTmout();
        }

        EvbTmSet(tm, VDP_TIMER_FREQUENCY);
        return ;
    }

    static TEvbTimer s_tEcpTimer;
    void EcpTmOut(TEvbTimer *tm, void *cookie)
    {
        if(NULL == tm)
        {
            EVB_ASSERT(0);
            return ;
        }

        if (gtECP.pfTmout)
        {
            gtECP.pfTmout();
        }
        EvbTmSet(tm, ECP_TIMER_FREQUENCY);
        return ;
    }

    static TEvbTimer s_tLldpTimer;
    void LldpTmOut(TEvbTimer *tm, void *cookie)
    {
        if(NULL == tm)
        {
            EVB_ASSERT(0);
            return ;
        }

        if (gtLLDP.pfTmout)
        {
            gtLLDP.pfTmout();
        }

        EvbTmSet(tm, LLDP_TIMER_FREQUENCY);
        return ;
    }
    /* End of timer; */

    int32 InitEvbProto(void)
    {
        /* system init */
        evbcfg_init();

        /* Timer Init */
        InitEvbTimerModule();

        /* LLDP init */
        lldp_init(NULL, NULL, NULL, NULL, &SendEvbPacket);        
        EvbTmInit(&s_tLldpTimer);
        s_tLldpTimer.handler = LldpTmOut;
        s_tLldpTimer.cookie  = NULL;
        EvbTmSet(&s_tLldpTimer, LLDP_TIMER_FREQUENCY);

        /* CDCP init */
        InitCdcp();  
        /* lldp_mod_register(&CdcpRegister); */
        /* EVB Tlv init; */
        lldp_mod_register(&evbtlv_register);

        /* ECP init */
        ecp_mod_init(NULL, NULL, NULL, NULL, &SendEvbPacket);
        EvbTmInit(&s_tEcpTimer);
        s_tEcpTimer.handler = EcpTmOut;
        s_tEcpTimer.cookie  = NULL;
        EvbTmSet(&s_tEcpTimer, ECP_TIMER_FREQUENCY);

        /* VDP init */
        vdp_mod_init(NULL, NULL, NULL);
        EvbTmInit(&s_tVdpTimer);
        s_tVdpTimer.handler = VdpTmOut;
        s_tVdpTimer.cookie  = NULL;
        EvbTmSet(&s_tVdpTimer, VDP_TIMER_FREQUENCY);

        /* Packet receive thread init */

        return 0;
    }

    int32 ShutdownEvbProto(void)
    {
        vdp_mod_shutdown();
        ecp_mod_shutdown();
        lldp_deinit();
        EvbTmCancel(&s_tLldpTimer);
        EvbTmCancel(&s_tVdpTimer);
        EvbTmCancel(&s_tEcpTimer);
        evbcfg_shutdown();
        return 0;
    }

#ifdef __cplusplus
}
#endif

