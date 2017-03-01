
#ifdef __cplusplus
extern "C" {
#endif

#include "evb_cfg.h"
#include "ecp.h"
#include "vdp.h"
#include "evb_port_mgr.h"

    T_EVBCFG_SYSBASE g_evbcfg_sys;

#if 0
    static int32 evbcfg_portIsValid(u32 port)
    {
        struct tagEvbPort *pPort = NULL;
        pPort = Get1stEvbPort();

        if( NULL == pPort)
        {
            return -1;
        }
        for(; pPort; pPort = GetNextEvbPort(pPort))
        {
            if(pPort->uiPortNo == port)
            {
                return 0;
            }
        }
        return -1;
    }
#endif 
    void evbcfg_init()
    {
        //MEMSET(&g_evbcfg_sys,0, sizeof(g_evbcfg_sys));

        // default value
        //g_evbcfg_sys.evbSysType = EVBCFG_TYPE_NOSUPPORT;
        g_evbcfg_sys.evbSysNumExternalPorts = EVBSYSCFG_DFTNUMEXTERNPORTS;
        g_evbcfg_sys.evbSysEvbLldpTxEnable = true;
        g_evbcfg_sys.evbSysEvbLldpManual = false;
        g_evbcfg_sys.evbSysEvbLldpGidCapable = false;  // sys depend

        g_evbcfg_sys.evbSysEcpDftAckTimerInit = EVBSYSCFG_DFTECPACKTIMERINIT;
        g_evbcfg_sys.evbSysEcpDftMaxTries = EVBSYSCFG_DFTECPMAXTRIES;
        
        /*  */
        g_evbcfg_sys.evbSysVdpDftRsrcWaitDelay = EVBSYSCFG_DFTVDPRSRCWAITDELAY;
        g_evbcfg_sys.evbSysVdpDftReinitKeepAlive = EVBSYSCFG_DFTVDPREINITKEEPALIVE;       
      
        /* evbtlv config init */
        /* bridge */		
        g_evbcfg_sys.bBGID = true; 
        g_evbcfg_sys.bRRCAP = true; 
        /* 放在配置中，主要考虑RRCTR需要发送给station，bridge侧协商后需要设置*/
        g_evbcfg_sys.bRRCTR = false; 

        /* station */
        g_evbcfg_sys.bSGrpID = true; 
        g_evbcfg_sys.bRRReq = false; 

        /* bridge and station */
        g_evbcfg_sys.ucR = 3;
        g_evbcfg_sys.ucRTE = 20;
        g_evbcfg_sys.ucRKA = 20;
        g_evbcfg_sys.ucRWD = 20;

        return;

    }

    int32 EvbCfgSetRRReq(bool bRR)
    {
        g_evbcfg_sys.bRRReq = bRR; 
        return 0;
    }
    
    u32 evbcfg_getEcpAckInit()
    {
        return g_evbcfg_sys.evbSysEcpDftAckTimerInit;
    }

    u8 evbcfg_getEcpMaxTries()
    {
        return g_evbcfg_sys.evbSysEcpDftMaxTries;
    }

    u32 evbcfg_getVdpRsrcWaitDelay()
    {
        return g_evbcfg_sys.evbSysVdpDftRsrcWaitDelay;
    }

    u32 evbcfg_getVdpReinitKeepAlive()
    {
        return g_evbcfg_sys.evbSysVdpDftReinitKeepAlive;
    }

    void  evbcfg_shutdown()
    {
        return ;
    }
      
    int32 EvbcfgGetSysType()
    { 
        return g_evbcfg_sys.evbSysType;
    }
    void  EvbcfgSetSysType(BYTE sysType)
    {
        switch(sysType)
        {
        case EVBCFG_TYPE_NOSUPPORT:
        case EVBSYS_TYPE_BRIDGE:
        case EVBSYS_TYPE_STATION:
            {
                g_evbcfg_sys.evbSysType = sysType;
            }
            break;
        default:break;
        }
    }

    void evbcfg_show()
    {
        PRINTF("evb sys cfg\n");
        PRINTF("  Type  %d\n",g_evbcfg_sys.evbSysType);
        PRINTF("  Port  %d\n",g_evbcfg_sys.evbSysNumExternalPorts);
        PRINTF("  lldpTx %d\n",g_evbcfg_sys.evbSysEvbLldpTxEnable);
        PRINTF("  lldpManual %d\n",g_evbcfg_sys.evbSysEvbLldpManual);
        PRINTF("  lldpGid %d\n",g_evbcfg_sys.evbSysEvbLldpGidCapable);
        PRINTF("  AckTimer %d\n",g_evbcfg_sys.evbSysEcpDftAckTimerInit);
        PRINTF("  maxTries %d\n",g_evbcfg_sys.evbSysEcpDftMaxTries);
        PRINTF("  rsrcWaitDelay %d\n",g_evbcfg_sys.evbSysVdpDftRsrcWaitDelay);
        PRINTF("  initKeepAlive %d\n",g_evbcfg_sys.evbSysVdpDftReinitKeepAlive);

        return ;
    }
    
    int32  evbtlv_chg_notify(u32 port,u32 flag)
    {
        // PRINTF("evbtlv_chg_set(port:%u, flag:%u)... \n",port,flag);
        // notify vdp ecp 
        evbtlv_chg_set_vdp(port, flag);
        evbtlv_chg_set_ecp(port, flag);

        return 0;
    }

#ifdef __cplusplus
}
#endif


