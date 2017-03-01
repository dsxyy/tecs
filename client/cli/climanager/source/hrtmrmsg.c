/**************************************************************************
* 版权所有 (C)2002, 深圳市中兴通讯股份有限公司。
*
* 文件名称：HrTmrMsg.c
* 文件标识：
* 内容摘要：处理来自定时器的超时消息
* 其它说明：
            
* 当前版本：
* 作    者：马俊辉
* 完成日期：2007.6.5
*
* 修改记录1：// 修改历史记录，包括修改日期、修改者及修改内容
*    修改日期：
*    版 本 号：
*    修 改 人：
*    修改内容：
* 修改记录2：
**************************************************************************/
/**************************************************************************
*                           头文件                                        *
*************************************************************************/
#include "includes.h"
#include "saveprtclcfg.h"
#include "xmlagent.h"
#include "offline_to_online.h"
#include "oam_execute.h"

/**************************************************************************
*                          常量                                           *
**************************************************************************/
/**************************************************************************
*                          宏                                             *
**************************************************************************/
/**************************************************************************
*                          数据类型                                       *
**************************************************************************/
/**************************************************************************
*                          全局变量                                       *
**************************************************************************/
/**************************************************************************
*                          本地变量                                       *
**************************************************************************/
/**************************************************************************
*                          局部函数原型                                   *
**************************************************************************/
/**************************************************************************
*                          局部函数实现                                   *
**************************************************************************/
/**************************************************************************
*                          全局函数实现                                   *
**************************************************************************/
/**************************************************************************
* 函数名称：VOID RecvFromXmlAdapterTimer
* 功能描述：接收来自XML适配器定时器的消息
* 访问的表：无
* 修改的表：无
* 输入参数：
  TYPE_XMLLINKSTATE *pLinkState,WORD16 wMsgId
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/05/05    V1.0    齐龙兆     创建
**************************************************************************/
void RecvFromXmlAdapterTimer(TYPE_XMLLINKSTATE *pLinkState,WORD16 wMsgId)
{
    switch (wMsgId)
    {
        case EV_TIMER_XML_OMM_HANDSHAKE:/*与网管端握手定时器*/
	     Xml_InitLinkState();/*初始化所有XML链路*/
            break;
        case EV_TIMER_EXECUTE_XMLADAPTER:/*XML适配器与前台应答定时器*/
//        XML_KillCmdPlanTimer(pLinkState);
	     /*向网管发送执行命令超时*/
   //         SendErrStringToOmm(pLinkState, Hdr_szExeTimeout);
            /*设置XML链路状态为空闲*/
  //          XML_LinkSetCurRunState(pLinkState, pec_MsgHandle_IDLE);
            break;
        default:
	     break;
    }
    return;
}

/**************************************************************************
* 函数名称：VOID RecvFromTimer
* 功能描述：接收来自定时器的消息
* 访问的表：无
* 修改的表：无
* 输入参数：
  TYPE_LINK_STATE *pLinkState,WORD16 wMsgId
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
void RecvFromTimer(TYPE_LINK_STATE *pLinkState,WORD16 wMsgId)
{
    WORD16 wAbsTimeout;
    WORD16 wIdleTimeout;
    CHAR szSysLog[128] = {0};
    CHAR *pSysLog = szSysLog;

    if (Oam_LinkGetLinkId(pLinkState) == MIN_CLI_VTY_NUMBER)
    {
        wAbsTimeout = gtCommonInfo.wConsoleAbsTimeout;
        wIdleTimeout = gtCommonInfo.wConsoleIdleTimeout;
    }
    else
    {
        wAbsTimeout = gtCommonInfo.wTelnetAbsTimeout;
        wIdleTimeout = gtCommonInfo.wTelnetIdleTimeout;
    }
    
    switch (wMsgId)
    {
    case EV_TIMER_IDLE_TIMEOUT:
        pLinkState->wIdleTimer = INVALID_TIMER_ID;

        pLinkState->wIdleTimeout++;
        if (pLinkState->wIdleTimeout >= wIdleTimeout)
        {
            /* 通知Telnet结束本终端 */
            pLinkState->wIdleTimeout = 0;

            if (Oam_LinkGetCurRunState(pLinkState) > pec_MsgHandle_INIT)
            {
                if ((USER_LOGOUT_NONE == pLinkState->tUserLog.ucQuitReason) && 
                    (0 == pLinkState->tUserLog.tSysClockLogout.wSysYear))
                {
                XOS_GetSysClock(&(pLinkState->tUserLog.tSysClockLogout));
                pLinkState->tUserLog.ucQuitReason = USER_LOGOUT_TIMEOUT;
                if (!UpdateOneUseLog(pLinkState))
                    {
                        XOS_SysLog(OAM_CFG_LOG_ERROR, "[RecvFromTimer] UpdateOneUseLog exec failed! line = %d\n", __LINE__);
                    }

                /* SYSLOG上报 */
                memset(pSysLog, 0, sizeof(szSysLog));
                XOS_snprintf(pSysLog, sizeof(szSysLog) - 1,"%d, %d, %s, %s, %d-%d-%d %d:%d, %d, %d-%d-%d %d:%d, %d\n",
                        pLinkState->tUserLog.wNo, pLinkState->tUserLog.ucConnType, pLinkState->tUserLog.sUserName, pLinkState->tUserLog.tnIpAddr, 
                        pLinkState->tUserLog.tSysClockLogin.wSysYear, pLinkState->tUserLog.tSysClockLogin.ucSysMon, pLinkState->tUserLog.tSysClockLogin.ucSysDay,
                        pLinkState->tUserLog.tSysClockLogin.ucSysHour, pLinkState->tUserLog.tSysClockLogin.ucSysMin,
                        pLinkState->tUserLog.ucLoginRslt,
                        pLinkState->tUserLog.tSysClockLogout.wSysYear, pLinkState->tUserLog.tSysClockLogout.ucSysMon, pLinkState->tUserLog.tSysClockLogout.ucSysDay,
                        pLinkState->tUserLog.tSysClockLogout.ucSysHour, pLinkState->tUserLog.tSysClockLogout.ucSysMin,
                        pLinkState->tUserLog.ucQuitReason);
                XOS_SysLog(OAM_CFG_LOG_NOTICE, pSysLog);
                }
            }
            
            SendLogoutToTelnet(pLinkState,NULL);
        }
        else
        {
            Oam_SetIdleTimer(pLinkState);
        }
        break;
/* 614000678048 (去掉24小时绝对超时) */
#if 0
    case EV_TIMER_ABS_TIMEOUT:
        pLinkState->wAbsTimer = INVALID_TIMER_ID;

        pLinkState->wAbsTimeout++;
        if (pLinkState->wAbsTimeout >= wAbsTimeout)
        {
            /* 通知Telnet结束本终端 */
            pLinkState->wAbsTimeout = 0;

            if (Oam_LinkGetCurRunState(pLinkState) > pec_MsgHandle_INIT)
            {
                XOS_GetSysClock(&(pLinkState->tUserLog.tSysClockLogout));
                pLinkState->tUserLog.ucQuitReason = USER_LOGOUT_TIMEOUT;
                if (!UpdateOneUseLog(pLinkState))
                {
                    XOS_SysLog(OAM_CFG_LOG_ERROR, "[RecvFromTimer] UpdateOneUseLog exec failed! line = %d\n", __LINE__);
                }

                /* SYSLOG上报 */
                memset(pSysLog, 0, sizeof(szSysLog));
                XOS_snprintf(pSysLog, sizeof(szSysLog) - 1,"%d, %d, %s, %s, %d-%d-%d %d:%d, %d, %d-%d-%d %d:%d, %d\n",
                        pLinkState->tUserLog.wNo, pLinkState->tUserLog.ucConnType, pLinkState->tUserLog.sUserName, pLinkState->tUserLog.tnIpAddr, 
                        pLinkState->tUserLog.tSysClockLogin.wSysYear, pLinkState->tUserLog.tSysClockLogin.ucSysMon, pLinkState->tUserLog.tSysClockLogin.ucSysDay,
                        pLinkState->tUserLog.tSysClockLogin.ucSysHour, pLinkState->tUserLog.tSysClockLogin.ucSysMin,
                        pLinkState->tUserLog.ucLoginRslt,
                        pLinkState->tUserLog.tSysClockLogout.wSysYear, pLinkState->tUserLog.tSysClockLogout.ucSysMon, pLinkState->tUserLog.tSysClockLogout.ucSysDay,
                        pLinkState->tUserLog.tSysClockLogout.ucSysHour, pLinkState->tUserLog.tSysClockLogout.ucSysMin,
                        pLinkState->tUserLog.ucQuitReason);
                XOS_SysLog(OAM_CFG_LOG_NOTICE, pSysLog);
                
            }
            
            SendLogoutToTelnet(pLinkState,NULL);
        }
        else
        {
            /* 614000678048 (去掉24小时绝对超时) */
            /*Oam_SetAbsTimer(pLinkState);*/
        }
        break;
#endif        
    case EV_TIMER_EXECUTE_NORMAL:
        {            
            pLinkState->wExeTimer = INVALID_TIMER_ID;
        
            Oam_KillCmdPlanTimer(pLinkState);
        
            /*if (Oam_LinkGetCurRunState(pLinkState) != pec_MsgHandle_EXECUTE)
                break;*/

            /*如果离线转在线中有命令超时，中止处理*/
            if (Oam_CfgCurLinkIsOffline2Online(pLinkState))
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR,
                               "RecvFromTimer: cmd execute time out, offline to online process exit!\n");
                Oam_CfgHandleMSGOffline2OnlineFinish();
                break;
            }

            /*修改返回码，以保证是最后一包*/
            pLinkState->wAppMsgReturnCode = ERR_AND_HAVEPARA;
            pLinkState->bOutputCmdLine = TRUE;
	     SendErrStringToClient(pLinkState,Hdr_szExeTimeout);       
            pLinkState->bOutputMode = OUTPUT_MODE_NORMAL;
            Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_TELNET);

                if (S_CfgSave == XOS_GetCurState(PROCTYPE_OAM_CLIMANAGER))
                {
                XOS_SetNextState(PROCTYPE_OAM_CLIMANAGER, S_Work);
                }
            break;
        }
    default:
        break;
    }
    return;
}


