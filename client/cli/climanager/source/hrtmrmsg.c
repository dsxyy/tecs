/**************************************************************************
* ��Ȩ���� (C)2002, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ�HrTmrMsg.c
* �ļ���ʶ��
* ����ժҪ���������Զ�ʱ���ĳ�ʱ��Ϣ
* ����˵����
            
* ��ǰ�汾��
* ��    �ߣ�����
* ������ڣ�2007.6.5
*
* �޸ļ�¼1��// �޸���ʷ��¼�������޸����ڡ��޸��߼��޸�����
*    �޸����ڣ�
*    �� �� �ţ�
*    �� �� �ˣ�
*    �޸����ݣ�
* �޸ļ�¼2��
**************************************************************************/
/**************************************************************************
*                           ͷ�ļ�                                        *
*************************************************************************/
#include "includes.h"
#include "saveprtclcfg.h"
#include "xmlagent.h"
#include "offline_to_online.h"
#include "oam_execute.h"

/**************************************************************************
*                          ����                                           *
**************************************************************************/
/**************************************************************************
*                          ��                                             *
**************************************************************************/
/**************************************************************************
*                          ��������                                       *
**************************************************************************/
/**************************************************************************
*                          ȫ�ֱ���                                       *
**************************************************************************/
/**************************************************************************
*                          ���ر���                                       *
**************************************************************************/
/**************************************************************************
*                          �ֲ�����ԭ��                                   *
**************************************************************************/
/**************************************************************************
*                          �ֲ�����ʵ��                                   *
**************************************************************************/
/**************************************************************************
*                          ȫ�ֺ���ʵ��                                   *
**************************************************************************/
/**************************************************************************
* �������ƣ�VOID RecvFromXmlAdapterTimer
* ������������������XML��������ʱ������Ϣ
* ���ʵı���
* �޸ĵı���
* ���������
  TYPE_XMLLINKSTATE *pLinkState,WORD16 wMsgId
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/05/05    V1.0    ������     ����
**************************************************************************/
void RecvFromXmlAdapterTimer(TYPE_XMLLINKSTATE *pLinkState,WORD16 wMsgId)
{
    switch (wMsgId)
    {
        case EV_TIMER_XML_OMM_HANDSHAKE:/*�����ܶ����ֶ�ʱ��*/
	     Xml_InitLinkState();/*��ʼ������XML��·*/
            break;
        case EV_TIMER_EXECUTE_XMLADAPTER:/*XML��������ǰ̨Ӧ��ʱ��*/
//        XML_KillCmdPlanTimer(pLinkState);
	     /*�����ܷ���ִ�����ʱ*/
   //         SendErrStringToOmm(pLinkState, Hdr_szExeTimeout);
            /*����XML��·״̬Ϊ����*/
  //          XML_LinkSetCurRunState(pLinkState, pec_MsgHandle_IDLE);
            break;
        default:
	     break;
    }
    return;
}

/**************************************************************************
* �������ƣ�VOID RecvFromTimer
* �����������������Զ�ʱ������Ϣ
* ���ʵı���
* �޸ĵı���
* ���������
  TYPE_LINK_STATE *pLinkState,WORD16 wMsgId
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
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
            /* ֪ͨTelnet�������ն� */
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

                /* SYSLOG�ϱ� */
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
/* 614000678048 (ȥ��24Сʱ���Գ�ʱ) */
#if 0
    case EV_TIMER_ABS_TIMEOUT:
        pLinkState->wAbsTimer = INVALID_TIMER_ID;

        pLinkState->wAbsTimeout++;
        if (pLinkState->wAbsTimeout >= wAbsTimeout)
        {
            /* ֪ͨTelnet�������ն� */
            pLinkState->wAbsTimeout = 0;

            if (Oam_LinkGetCurRunState(pLinkState) > pec_MsgHandle_INIT)
            {
                XOS_GetSysClock(&(pLinkState->tUserLog.tSysClockLogout));
                pLinkState->tUserLog.ucQuitReason = USER_LOGOUT_TIMEOUT;
                if (!UpdateOneUseLog(pLinkState))
                {
                    XOS_SysLog(OAM_CFG_LOG_ERROR, "[RecvFromTimer] UpdateOneUseLog exec failed! line = %d\n", __LINE__);
                }

                /* SYSLOG�ϱ� */
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
            /* 614000678048 (ȥ��24Сʱ���Գ�ʱ) */
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

            /*�������ת�����������ʱ����ֹ����*/
            if (Oam_CfgCurLinkIsOffline2Online(pLinkState))
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR,
                               "RecvFromTimer: cmd execute time out, offline to online process exit!\n");
                Oam_CfgHandleMSGOffline2OnlineFinish();
                break;
            }

            /*�޸ķ����룬�Ա�֤�����һ��*/
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


