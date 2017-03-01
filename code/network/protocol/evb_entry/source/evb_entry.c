#ifdef __cplusplus
extern "C" {
#endif

#include "evb_comm.h"
#include "evb_msg_def.h"
#include "evb_port_mgr.h"
#include "evb_thr.h"
#include "evb_main_thr.h"
#include "evb_drv_thr.h"
#include "evb_msg.h"
#include "evb_entry.h"
#include "evb_timer.h"
#include "evb_lock.h"

#include "lldp_in.h"
#include "evb_cfg.h"
#include "ecp.h"
#include "vdp.h"
#include "vsi_profile.h"

#define EVB_INIT_STATE      (0x0)   /* EVB 模块初始态; */
#define EVB_WORKING_STATE   (0x1)   /* EVB 模块工作态; */

    TMutex g_tMainThrdLock;     /* 主控线程与报文接收线程的资源锁; */
    u8 g_ucPrintMsg = 0;

    typedef enum tagEnumThrd
    {
        ENM_MAIN_THRD = 0,
        ENM_PKT_RCV_THRD,
        /* ENM_TIMER_THRD, */
        ENM_ALL_THRD
    }TEnumThrd;
    
    static TEvbThrdMgr s_atThread[ENM_ALL_THRD] = 
    {
        /* 主控线程; */
        {INVALID_THREAD_ID, 45, 1000, "EvbMainThread", EVB_THREAD_STOP, 0, NULL, (EVB_THRD_ENTRY)EvbMainThrdEntry, NULL, NULL, NULL, INVALID_THREAD_ID},
        /* 接收报文线程; */
        {INVALID_THREAD_ID, 45, 1000, "EvbPktRcvThread", EVB_THREAD_STOP, 0, NULL, (EVB_THRD_ENTRY)EvbDrvThrdEntry, NULL, NULL, NULL, INVALID_THREAD_ID},
        #if 0
        /* 时钟线程; */
        {INVALID_THREAD_ID, 45, 1000, "EvbTimerThread", EVB_THREAD_STOP, 0, NULL, (EVB_THRD_ENTRY)EvbTimerThrdEntry, NULL, NULL, NULL, INVALID_THREAD_ID},
        #endif
    };
    static u32 s_dwEvbWorkingState = EVB_INIT_STATE;

    int32 EvbPostMainThreadMsg(u32 dwMsgID, void *pMsg, u32 dwMsgLen);

    int32 SetVdpDom0MacVsi(u32 port,BYTE mac[EVB_PORT_MAC_LEN])
    {
        BYTE vsi[VDPTLV_VSIID_LEN]={0};
        BYTE mgrid[VDPTLV_VSIMGRID_LEN]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1};
        BYTE vsitypeid[VDPTLV_VSITYPEID_LEN]={0,0,1};
        BYTE groupid[EVB_VDP_GROUPID_LEN]={0};
        u16  vlan = 1;

        //memcpy(vsi,port,sizeof(u32));
        vsi[10]=mac[0];
        vsi[11]=mac[1];
        vsi[12]=mac[2];
        vsi[13]=mac[3];
        vsi[14]=mac[4];
        vsi[15]=mac[5];

        if( 0 != vsiProfile_addvsivlanmac(0x05,vsi,mgrid,vsitypeid,0x01,groupid,vlan,mac) )
        {
            EVB_LOG(EVB_LOG_ERROR,"SetVdpDom0MacVsi vsiProfile_addvsivlanmac failed.\n");
            return -1; 
        }

        /*cmdpool*/
        if( 0 != vsiCmd_pool_add(port,0x05,vsi,VDPTLV_TYPE_ASSOCIATE) )
        {
            EVB_LOG(EVB_LOG_ERROR,"SetVdpDom0MacVsi vsiCmd_pool_add failed.\n");
            return -1;
        }
        return 0;
    }

    void EvbMainEntry(u32 dwMessage, void *pMsg, u32 dwMsgLen)
    {
        struct tagEvbIssPort *pEvbIssPort = NULL;
        struct tagEvbPort *pEvbPort = NULL;
        
        if(0 != g_ucPrintMsg)
        {
            PRINTF("EvbMainEntry: receive msg: %u.\n", dwMessage);
        }
        
        switch(s_dwEvbWorkingState)
        {
            /************************************************************************/
            /*  INIT STATE                                                          */
            /************************************************************************/
        case EVB_INIT_STATE:
            {
                switch (dwMessage)
                {
                case EVB_WORKING_MSG:
                    {
                        s_dwEvbWorkingState = EVB_WORKING_STATE;
                        break;
                    }
                default:
                    {
                        EVB_LOG(EVB_LOG_WARN, 
                            "EvbMainEntry: Can't process the message[%d]@EVB_INIT_STATE state", 
                            dwMessage);
                    }
                }/* end switch (dwMessage) */
            }
            break;

            /************************************************************************/
            /*  WORKING STATE                                                       */
            /************************************************************************/
        case EVB_WORKING_STATE:
            {
                switch (dwMessage)
                {
                #if 0
                case EVB_20MS_TIMER_MSG:
                    {                    
                        EvbLock(&g_tMainThrdLock);
                        Evb20msTimerHandler();                        
                        EvbUnlock(&g_tMainThrdLock);
                        break;
                    }
                #endif
                case EVB_ADD_ISS_PORT_MSG:
                    {
                        pEvbIssPort = NULL;
                        pEvbPort = NULL;
                        TEvbAddIssPortMsg *pMsgData = (TEvbAddIssPortMsg *)pMsg;

                        if(NULL == pMsg || 0 == dwMsgLen)
                        {
                            EVB_LOG(EVB_LOG_ERROR, "Receive EVB_ADD_ISS_PORT_MSG: msg error[pMsg: %p, len: %d]", pMsgData, dwMsgLen);
                            break;
                        }

                        EVB_LOG(EVB_LOG_WARN, "Receive EVB_ADD_ISS_PORT_MSG:[Port: %s, No: %d, State: %s, EvbPortType: %d].\n", 
                            pMsgData->acName,
                            pMsgData->uiPortNo,
                            (EVB_PORT_STATE_UP == pMsgData->ucState) ? "UP":"DOWN",
                            pMsgData->ucEvbPortType);

                        EvbLock(&g_tMainThrdLock);
                        pEvbIssPort = CreateEvbIssPort(pMsgData->uiPortNo, pMsgData->acName, pMsgData->aucMac);
                        if(NULL != pEvbIssPort)
                        {
                            EVB_LOG(EVB_LOG_INFO, "call CreateEvbIssPort successfully.\n");
                        }
                        else
                        {
                            EVB_LOG(EVB_LOG_ERROR, "call CreateEvbIssPort(%d, %s, %p) failed.\n", 
                                pMsgData->uiPortNo, pMsgData->acName, pMsgData->aucMac);
                            EvbUnlock(&g_tMainThrdLock);
                            break ;
                        }

                        if(0 != SetEvbPortIssStat(pMsgData->uiPortNo, pMsgData->ucState))
                        {
                            EVB_LOG(EVB_LOG_ERROR, "call SetEvbPortIssStat(%d, %d) failed.\n", 
                                pMsgData->uiPortNo, pMsgData->ucState); 
                        }

                        /* 创建逻辑端口; */
                        pEvbPort = CreateEvbPort(pMsgData->ucEvbPortType, pEvbIssPort);
                        if(NULL != pEvbPort)
                        {
                            EVB_LOG(EVB_LOG_INFO, "call CreateEvbPort successfully.\n");

                            if(EM_EVB_PT_UAP == pEvbPort->ucEvbPortType)
                            {
                                create_lldp_port(pEvbPort->uiPortNo, (u8 *)(pEvbIssPort->acName), pEvbIssPort->aucMac, NearestNonTPMRBridge);
                            }

                            if(EM_EVB_PT_URP == pEvbPort->ucEvbPortType || EM_EVB_PT_SBP == pEvbPort->ucEvbPortType)
                            {
                                create_lldp_port(pEvbPort->uiPortNo, (u8 *)(pEvbIssPort->acName), pEvbIssPort->aucMac, NearestCustomerBridge);
                                ecp_port_init(pEvbPort->uiPortNo);
                                vdp_port_init(pEvbPort->uiPortNo);

                                /* add default vsiid, station */
                                if( EM_EVB_PT_URP == pEvbPort->ucEvbPortType )
                                {
                                    if( 0 != SetVdpDom0MacVsi(pEvbPort->uiPortNo,pEvbIssPort->aucMac) )
                                    {
                                        EVB_LOG(EVB_LOG_ERROR, "EvbMainEntry: SetVdpDom0MacVsi failed.\n");
                                    }
                                }
                            }

                            if(EVB_PORT_STATE_UP == pMsgData->ucState)
                            {
                                lldp_port_up(pEvbPort->uiPortNo);
                                ecp_port_up(pEvbPort->uiPortNo);
                                vdp_port_up(pEvbPort->uiPortNo);
                            }
                            else
                            {
                                lldp_port_down(pEvbPort->uiPortNo);
                                ecp_port_down(pEvbPort->uiPortNo);
                                vdp_port_down(pEvbPort->uiPortNo);
                            }

                        }
                        else
                        {
                            EVB_LOG(EVB_LOG_ERROR, "call CreateEvbPort(%d, %p) failed.\n", pMsgData->ucEvbPortType, pEvbIssPort);
                        }

                        
                        
                        EvbUnlock(&g_tMainThrdLock);
                        break;
                    }
                case EVB_DEL_ISS_PORT_MSG:
                    {
                        pEvbIssPort = NULL;
                        pEvbPort = NULL;
                        TEvbDelIssPortMsg *pMsgData = (TEvbDelIssPortMsg *)pMsg;

                        if(NULL == pMsg || 0 == dwMsgLen)
                        {
                            EVB_LOG(EVB_LOG_ERROR, "Receive EVB_DEL_ISS_PORT_MSG: msg error[pMsg: %p, len: %d]", pMsgData, dwMsgLen);
                            break;
                        }

                        EVB_LOG(EVB_LOG_WARN, "Receive EVB_DEL_ISS_PORT_MSG:[PortNo: %u].\n", 
                            pMsgData->uiPortNo);

                        EvbLock(&g_tMainThrdLock);
                        pEvbIssPort = GetEvbIssPort(pMsgData->uiPortNo);
                        if(NULL != pEvbIssPort)
                        {
                            EVB_LOG(EVB_LOG_INFO, "call GetEvbIssPort successfully.\n");
                        }
                        else
                        {
                            EVB_LOG(EVB_LOG_ERROR, "call GetEvbIssPort(%d) failed.\n", pMsgData->uiPortNo);
                            EvbUnlock(&g_tMainThrdLock);
                            break ;
                        }

                        for(pEvbPort = Get1stEvbPort(); pEvbPort;)
                        {
                            if(pEvbPort->pIssPort)
                            {   
                                if(pEvbPort->pIssPort->uiPortNo == pMsgData->uiPortNo)
                                {
                                    struct tagEvbPort * pTmp = pEvbPort;
                                    pEvbPort = GetNextEvbPort(pEvbPort);
                                    if(EM_EVB_PT_UAP == pTmp->ucEvbPortType)
                                    {
                                        destroy_lldp_port(pTmp->uiPortNo);
                                    }
                                    if(EM_EVB_PT_URP == pTmp->ucEvbPortType || EM_EVB_PT_SBP == pTmp->ucEvbPortType)
                                    {
                                        destroy_lldp_port(pTmp->uiPortNo);
                                        ecp_port_shutdown(pTmp->uiPortNo);
                                        vdp_port_shutdown(pTmp->uiPortNo); 
                                    }
                                    DeleteEvbPort(pTmp->uiPortNo);                           
                                    continue;                                    
                                }
                            }                            
                            pEvbPort = GetNextEvbPort(pEvbPort);            
                        }
                        
                        
                        DeleteEvbIssPort(pMsgData->uiPortNo);       
                        EvbUnlock(&g_tMainThrdLock);
                        break;
                    }
                case EVB_CHG_ISS_PORT_STATE_MSG:
                    {
                        pEvbPort = NULL;
                        pEvbIssPort = NULL;
                        TEvbChgPortStatMsg *pMsgData = (TEvbChgPortStatMsg *)pMsg;

                        if(NULL == pMsg || 0 == dwMsgLen)
                        {
                            EVB_LOG(EVB_LOG_ERROR, "Receive EVB_CHG_ISS_PORT_STATE_MSG: msg error[pMsg: %p, len: %d]", pMsgData, dwMsgLen);
                            break;
                        }

                        EVB_LOG(EVB_LOG_WARN, "Receive EVB_CHG_ISS_PORT_STATE_MSG:[Port: %d, State: %s].\n", 
                            pMsgData->uiPortNo, 
                            (EVB_PORT_STATE_UP == pMsgData->ucState) ? "UP":"DOWN");

                        pEvbIssPort = GetEvbIssPort(pMsgData->uiPortNo);
                        if(NULL == pEvbIssPort)
                        {
                            EVB_LOG(EVB_LOG_ERROR, "Chg ISS Port state failed. GetEvbIssPort(%d) == NULL.\n", 
                                pMsgData->uiPortNo);
                            break;
                        }

                        EvbLock(&g_tMainThrdLock);
                        if(0 == SetEvbPortIssStat(pMsgData->uiPortNo, pMsgData->ucState))
                        {
                            /* 底层端口状态上报给logic口，且影响协议状态; */
                            if(EVB_PORT_STATE_UP == pMsgData->ucState)
                            {
                                for(pEvbPort = Get1stEvbPort(); pEvbPort; pEvbPort = GetNextEvbPort(pEvbPort))
                                {
                                    if(pEvbPort->pIssPort == pEvbIssPort)
                                    {
                                        lldp_port_up(pEvbPort->uiPortNo);
                                        ecp_port_up(pEvbPort->uiPortNo);
                                        vdp_port_up(pEvbPort->uiPortNo);
                                    }
                                }/* end for(pEvbPort...; */
                            }/* end if(EVB_PORT_STATE_UP...; */
                            else
                            {
                                for(pEvbPort = Get1stEvbPort(); pEvbPort; pEvbPort = GetNextEvbPort(pEvbPort))
                                {
                                    if(pEvbPort->pIssPort == pEvbIssPort)
                                    {
                                        lldp_port_down(pEvbPort->uiPortNo);
                                        ecp_port_down(pEvbPort->uiPortNo);
                                        vdp_port_down(pEvbPort->uiPortNo);
                                    }
                                }/* end for(pEvbPort...; */
                            }/* end else; */
                            EVB_LOG(EVB_LOG_WARN, "Change ISS Port state successfully.\n"); 
                        }  
                        EvbUnlock(&g_tMainThrdLock);
                        break;
                    }
                case EVB_CHG_ISS_PORT_MODE:
                    {
                        pEvbPort = NULL;
                        pEvbIssPort = NULL;
                        TEvbChgIssPortEvbModeMsg *pMsgData = (TEvbChgIssPortEvbModeMsg *)pMsg;

                        if(NULL == pMsg || 0 == dwMsgLen)
                        {
                            EVB_LOG(EVB_LOG_ERROR, "Receive EVB_CHG_ISS_PORT_MODE: msg error[pMsg: %p, len: %d]", pMsgData, dwMsgLen);
                            break;
                        }

                        EVB_LOG(EVB_LOG_WARN, "Receive EVB_CHG_ISS_PORT_MODE:[Port: %d, Mode: %s].\n", 
                            pMsgData->uiPortNo, 
                            (EN_VEB_MODE == pMsgData->ucMode) ? "VEB":"VEPA");

                        pEvbIssPort = GetEvbIssPort(pMsgData->uiPortNo);
                        if(NULL == pEvbIssPort)
                        {
                            EVB_LOG(EVB_LOG_ERROR, "Chg ISS Port Mode failed. GetEvbIssPort(%d) == NULL.\n", 
                                pMsgData->uiPortNo);
                            break;
                        }

                        EvbLock(&g_tMainThrdLock);

                        for(pEvbPort = Get1stEvbPort(); pEvbPort; pEvbPort = GetNextEvbPort(pEvbPort))
                        {
                            if(pEvbPort->pIssPort == pEvbIssPort)
                            {
                                if(EM_EVB_PT_URP == pEvbPort->ucEvbPortType)
                                {
                                    TEvbUrpAttr *pUrpAttr = (TEvbUrpAttr*)(pEvbPort->ptPortAttr);
                                    if(pUrpAttr)
                                    {
                                        if(EN_VEPA_MODE == pMsgData->ucMode)
                                        {
                                            pUrpAttr->bRRReq = true;
                                        }
                                        else
                                        {
                                            pUrpAttr->bRRReq = false;
                                        }
                                    }
                                    
                                }
                            }
                        }/* end for(pEvbPort...; */                          
                        EvbUnlock(&g_tMainThrdLock);
                        break;
                    }
                case EVB_CDCP_CMD:
                    {

                        break;
                    }
                case EVB_CFG_ISS_PORT_MSG:
                    {
                        pEvbIssPort = NULL;
                        pEvbPort = NULL;
                        TEvbPortMsg *pMsgData = (TEvbPortMsg *)pMsg;

                        if(NULL == pMsg || 0 == dwMsgLen)
                        {
                            EVB_LOG(EVB_LOG_ERROR, "Receive EVB_CFG_ISS_PORT_MSG: msg error[pMsg: %p, len: %d]", pMsgData, dwMsgLen);
                            break;
                        }               

                        EVB_LOG(EVB_LOG_WARN, "Receive EVB_CFG_ISS_PORT_MSG:[IssPortNo: %d, type: %d].\n", 
                            pMsgData->uiIssPortNo,
                            pMsgData->ucPortType);

                        pEvbIssPort = GetEvbIssPort(pMsgData->uiIssPortNo);
                        if(NULL == pEvbIssPort)
                        {
                            EVB_LOG(EVB_LOG_ERROR, "Handle EVB_CFG_ISS_PORT_MSG failed.EVB ISS port [%d] is not existed.\n", 
                                pMsgData->uiIssPortNo);
                            break;
                        }

                        EvbLock(&g_tMainThrdLock);
                        pEvbPort = CreateEvbPort(pMsgData->ucPortType, pEvbIssPort);
                        if(NULL != pEvbPort)
                        {
                            EVB_LOG(EVB_LOG_INFO, "call CreateEvbPort successfully.\n");

                            if(EM_EVB_PT_UAP == pEvbPort->ucEvbPortType)
                            {
                                create_lldp_port(pEvbPort->uiPortNo, (u8 *)(pEvbIssPort->acName), pEvbIssPort->aucMac, NearestNonTPMRBridge);
                            }

                            if(EM_EVB_PT_URP == pEvbPort->ucEvbPortType || EM_EVB_PT_SBP == pEvbPort->ucEvbPortType)
                            {
                                create_lldp_port(pEvbPort->uiPortNo, (u8 *)(pEvbIssPort->acName), pEvbIssPort->aucMac, NearestCustomerBridge);
                                ecp_port_init(pEvbPort->uiPortNo);
                                vdp_port_init(pEvbPort->uiPortNo);

                                /* add default vsiid, station */
                                if( EM_EVB_PT_URP == pEvbPort->ucEvbPortType )
                                {
                                    if( 0 != SetVdpDom0MacVsi(pEvbPort->uiPortNo,pEvbIssPort->aucMac) )
                                    {
                                        EVB_LOG(EVB_LOG_ERROR, "EvbMainEntry: SetVdpDom0MacVsi failed.\n");
                                    }
                                }
                            }

                            if(EVB_PORT_STATE_UP == pEvbIssPort->ucPortState)
                            {
                                lldp_port_up(pEvbPort->uiPortNo);
                                ecp_port_up(pEvbPort->uiPortNo);
                                vdp_port_up(pEvbPort->uiPortNo);
                            }
                            else
                            {
                                lldp_port_down(pEvbPort->uiPortNo);
                                ecp_port_down(pEvbPort->uiPortNo);
                                vdp_port_down(pEvbPort->uiPortNo);
                            }
                        } 
                        EvbUnlock(&g_tMainThrdLock);
                        break;
                    }
                default:
                    {
                        EVB_LOG(EVB_LOG_WARN, 
                            "EvbMainEntry: Can't process the message[%d]@EVB_WORKING_STATE state", 
                            dwMessage);
                    }
                }/* end switch(dwMessage) */
            }/* end case EVB_WORKING_STATE */
            break;
        default:
            {
                EVB_ASSERT(0);
                EVB_LOG(EVB_LOG_WARN, "EvbMainEntry: Can't process the message[%d]@state[%d]", 
                    dwMessage, s_dwEvbWorkingState);
            }
        }/* end switch(s_dwEvbWorkingState) */

        return ;
    }

    int32 StartEvbModule(u8 ucAdminRole)
    {
        char strLockName[] = "EvbMainLock";
        int nCnt = 0;

        /* 设置主控状态; */
        s_dwEvbWorkingState = EVB_INIT_STATE;
        g_ucPrintMsg = 0;
        
        /* 设置AdminRole; */
        EvbcfgSetSysType(ucAdminRole);

        /* 初始化锁; */
        MEMSET(&g_tMainThrdLock, 0x0, sizeof(g_tMainThrdLock));
        if(0 != CreateLock(strLockName, &g_tMainThrdLock))
        {
            EVB_LOG(EVB_LOG_ERROR, "StartEvbModule: call CreateLock(EvbMainLock) failed!!!\n");
            return -1;
        }

        /* EVB port init */
        InitEvbPortModule();
        
        /* 启动主控线程; */
        if(0 != StartEvbThread(&s_atThread[ENM_MAIN_THRD]))
        {
            EVB_LOG(EVB_LOG_ERROR, "StartEvbModule: call StartEvbThread(ENM_MAIN_THRD) failed!!!\n");
            goto START_EVB_ERR;
        }

        /* 启动端口报文守护线程; */
        if(0 != StartEvbThread(&s_atThread[ENM_PKT_RCV_THRD]))
        {
            EVB_LOG(EVB_LOG_ERROR, "StartEvbModule: call StartEvbThread(ENM_PKT_RCV_THRD) failed!!!\n");
            EvbPostMainThreadMsg(EVB_SHUTDOWN_MSG, NULL, 0);
            KillEvbThread(&s_atThread[ENM_MAIN_THRD]);
            goto START_EVB_ERR;
        }       
#if 0
        /* 启动EVB定时器线程; */
        if(0 != StartEvbThread(&s_atThread[ENM_TIMER_THRD]))
        {
            EVB_LOG(EVB_LOG_ERROR, "StartEvbModule: call StartEvbThread(ENM_TIMER_THRD) failed!!!\n");
            KillEvbThread(&s_atThread[ENM_PKT_RCV_THRD]);
            EvbPostMainThreadMsg(EVB_SHUTDOWN_MSG, NULL, 0);
            KillEvbThread(&s_atThread[ENM_MAIN_THRD]);
            goto START_EVB_ERR;
        }        
#endif      
        while (1)
        {
            if (nCnt>100)
            {
                EVB_LOG(EVB_LOG_ERROR,
                         "CEvbMgr::Init: Start EVB time out.\n");
                         
                KillEvbThread(&s_atThread[ENM_PKT_RCV_THRD]);
                EvbPostMainThreadMsg(EVB_SHUTDOWN_MSG, NULL, 0);
                KillEvbThread(&s_atThread[ENM_MAIN_THRD]);
                
                goto START_EVB_ERR;
            }
            
            if (EvbMainThreadIsWorking())
            {
                EVB_LOG(EVB_LOG_ERROR,
                         "CEvbMgr::Init: Start EVB successfully.\n");
                break;
            }
            
            ++nCnt;
            EvbDelay(50);
        }

        return 0;

START_EVB_ERR:
        DestroyEvbPortModule();
        DestroyLock(&g_tMainThrdLock);        
        s_dwEvbWorkingState = EVB_INIT_STATE;
        return -1;
    }

    int32 ShutdownEvbModule(void)
    {
    #if 0
        KillEvbThread(&s_atThread[ENM_TIMER_THRD]);
    #endif
        KillEvbThread(&s_atThread[ENM_PKT_RCV_THRD]);
        EvbPostMainThreadMsg(EVB_SHUTDOWN_MSG, NULL, 0);
        KillEvbThread(&s_atThread[ENM_MAIN_THRD]);        
        DestroyEvbPortModule();
        DestroyLock(&g_tMainThrdLock);
        return 0;
    }

    int32 EvbMainThreadIsWorking(void)
    {
        return (EVB_WORKING_STATE == s_dwEvbWorkingState) ? 1 : 0;
    }

    int32 EvbPostMainThreadMsg(u32 dwMsgID, void *pMsg, u32 dwMsgLen)
    {
        return EvbPostThrdMsg(&s_atThread[ENM_MAIN_THRD], dwMsgID, pMsg, dwMsgLen);
    }

    int32 Evb20msTimerLoop(void)
    {
        if(EvbMainThreadIsWorking())
        {
            #if 0
            return EvbPostMainThreadMsg(EVB_20MS_TIMER_MSG, NULL, 0);
            #endif
            
            EvbLock(&g_tMainThrdLock);
            Evb20msTimerHandler();                        
            EvbUnlock(&g_tMainThrdLock);

            return 0;
        }
        return -1;
    }

    int32 AddEvbIssPort(u32 uiPortNo, const char *pName, u8 *aucMac, u8 ucState, u8 ucEvbPortType)
    {
        int nRet = 0;

        if(NULL == pName || NULL == aucMac)
        {
            EVB_ASSERT(0);
            EVB_LOG(EVB_LOG_ERROR, "AddEvbIssPort: %x == pName, %x == aucMac!\n", pName, aucMac);
            return -1;
        }

        if(EvbMainThreadIsWorking())
        {
            TEvbAddIssPortMsg *pMsgData = (TEvbAddIssPortMsg *)GLUE_ALLOC(sizeof(TEvbAddIssPortMsg));
            if(NULL == pMsgData)
            {
                EVB_LOG(EVB_LOG_ERROR, "AddEvbIssPort: GLUE_ALLOC failed.\n");
                return -1;
            }
            MEMSET(pMsgData, 0x0, sizeof(TEvbAddIssPortMsg));

            pMsgData->uiPortNo = uiPortNo;
            sprintf(pMsgData->acName, "%s", pName);
            MEMCPY(pMsgData->aucMac, aucMac, EVB_PORT_MAC_LEN);

            if(1 == ucState)
            {
                pMsgData->ucState = EVB_PORT_STATE_UP;
            }
            else
            {
                pMsgData->ucState = EVB_PORT_STATE_DW;
            }
            pMsgData->ucEvbPortType = ucEvbPortType;

            nRet = EvbPostMainThreadMsg(EVB_ADD_ISS_PORT_MSG, pMsgData, sizeof(TEvbAddIssPortMsg));

            GLUE_FREE(pMsgData);
            pMsgData = NULL;

            PRINTF("AddEvbIssPort : %d. %s\n", uiPortNo,pName);

            return nRet;
        }
        return -1;
    }

    int32 DelEvbIssPort(u32 uiPortNo)
    {
        int nRet = 0;
        if(EvbMainThreadIsWorking())
        {
            TEvbDelIssPortMsg *pMsgData = (TEvbDelIssPortMsg *)GLUE_ALLOC(sizeof(TEvbDelIssPortMsg));
            if(NULL == pMsgData)
            {
                EVB_LOG(EVB_LOG_ERROR, "CEvbMgr::DelEvbIssPort: GLUE_ALLOC failed.\n");
                return -1;
            }
            MEMSET(pMsgData, 0x0, sizeof(TEvbDelIssPortMsg));
            pMsgData->uiPortNo = uiPortNo;
            nRet = EvbPostMainThreadMsg(EVB_DEL_ISS_PORT_MSG, pMsgData, sizeof(TEvbDelIssPortMsg));
            GLUE_FREE(pMsgData);
            pMsgData = NULL;

            PRINTF("DelEvbIssPort : %d.\n", uiPortNo);
            return nRet;
        }        
        return -1;
    }

    int32 ChgEvbIssPortMode(u32 uiPortNo, u8 ucMode)
    {
        
        int nRet = 0;
        if(EvbMainThreadIsWorking())
        {
            TEvbChgIssPortEvbModeMsg *pMsgData = (TEvbChgIssPortEvbModeMsg *)GLUE_ALLOC(sizeof(TEvbChgIssPortEvbModeMsg));
            if(NULL == pMsgData)
            {
                EVB_LOG(EVB_LOG_ERROR, "CEvbMgr::SetEvbMode: GLUE_ALLOC failed.\n");
                return -1;
            }
            MEMSET(pMsgData, 0x0, sizeof(TEvbChgIssPortEvbModeMsg));
            pMsgData->uiPortNo = uiPortNo;
            pMsgData->ucMode = ucMode;      
            nRet = EvbPostMainThreadMsg(EVB_CHG_ISS_PORT_MODE, pMsgData, sizeof(TEvbChgIssPortEvbModeMsg));
            GLUE_FREE(pMsgData);
            pMsgData = NULL;
            return nRet;
        }
        return -1;
    }

    int32 ConfEvbIssPort(u32 uiIssPortNo, u8 ucPortType)
    {
        int nRet = 0;

        if(EVB_INVALID_PORT_NO == uiIssPortNo)
        {
            EVB_ASSERT(0);
            EVB_LOG(EVB_LOG_ERROR, "CreateEvbUrpPort: EVB_INVALID_PORT_NO == uiIssPortNo!\n");
            return -1;
        }

        if(EvbMainThreadIsWorking())
        {
            TEvbPortMsg *pMsgData = (TEvbPortMsg *)GLUE_ALLOC(sizeof(TEvbPortMsg));
            if(NULL == pMsgData)
            {
                EVB_LOG(EVB_LOG_ERROR, "AddEvbIssPort: GLUE_ALLOC failed.\n");
                return -1;
            }
            MEMSET(pMsgData, 0x0, sizeof(TEvbPortMsg));

            pMsgData->uiIssPortNo = uiIssPortNo;
            pMsgData->ucPortType = ucPortType;
          
            nRet = EvbPostMainThreadMsg(EVB_CFG_ISS_PORT_MSG, pMsgData, sizeof(TEvbPortMsg));

            GLUE_FREE(pMsgData);
            pMsgData = NULL;

            return nRet;
        }
        return -1;
    }

    int32 ChgEvbIssPortState(u32 uiPortNo, u8 ucState)
    {
        int nRet = 0;
        if(EvbMainThreadIsWorking())
        {
            TEvbChgPortStatMsg *pMsgData = (TEvbChgPortStatMsg *)GLUE_ALLOC(sizeof(TEvbChgPortStatMsg));
            if(NULL == pMsgData)
            {
                EVB_LOG(EVB_LOG_ERROR, "SetEvbPortState: GLUE_ALLOC failed.\n");
                return -1;
            }
            pMsgData->uiPortNo = uiPortNo;
            if(1 == ucState)
            {
                pMsgData->ucState = EVB_PORT_STATE_UP;
            }
            else
            {
                pMsgData->ucState = EVB_PORT_STATE_DW;
            }
            
            nRet = EvbPostMainThreadMsg(EVB_CHG_ISS_PORT_STATE_MSG, pMsgData, sizeof(TEvbChgPortStatMsg));

            GLUE_FREE(pMsgData);
            pMsgData = NULL;

            return nRet;
        }

        return -1;
    }

    int32 ConfCdcpCmd(void)
    {
        if(EvbMainThreadIsWorking())
        {
            return EvbPostMainThreadMsg(EVB_CDCP_CMD, NULL, 0);
        }

        return -1;
    }

    void DbgShowEvbThrdInfo(void)
    {
        TEvbThrdMgr *pTrd = NULL;
        u32 i = 0; 

        PRINTF("--------------------------------------------------------------------------\n");
        PRINTF("%-10s %-4s %-5s %-16s %-6s %-14s %-6s\n", 
            "ID", "PRO", "STA_SZ", "NAME", "STATE", "PF_MQ", "TID"); 
        PRINTF("--------------------------------------------------------------------------\n"); 
        
        for(i = 0; i < sizeof(s_atThread)/sizeof(TEvbThrdMgr); i++)
        {
            pTrd = &s_atThread[i];
            PRINTF("0x%-10x %-4u %-4u %-16s %-6s %-14p %-6u\n", 
                pTrd->tThreadID,
                pTrd->dwPriority,
                pTrd->dwStackSize,
                pTrd->acName,
                (EVB_THREAD_STOP == pTrd->wThrdFlag) ? "INIT":"WORK",
                pTrd->pMsgQue,
                pTrd->tid);
        }

        return ;
    }

#ifdef __cplusplus
}
#endif

