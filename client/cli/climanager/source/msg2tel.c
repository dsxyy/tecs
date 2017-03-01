/**************************************************************************
* ��Ȩ���� (C)2002, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ�HrTelMsg.c
* �ļ���ʶ��
* ����ժҪ��������Ϣ��Telnet
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
**************************************************************************/
#include "includes.h"
#include "xmlagent.h"
#include "oam_execute.h"
#include "cliapp.h"
#include <string.h> 
#include <math.h>
#include "offline_to_online.h"
#include "cliapp.h"
/*lint -e721*/
/*lint -e539*/
/*lint -e548*/
/*lint -e669*/
/**************************************************************************
*                          ����                                           *
**************************************************************************/
/**************************************************************************
*                          ��                                             *
**************************************************************************/
#define MAX_SIZE_SAVEACK_BUFFER                      (WORD16)512
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
static void SendSaveAckToClient(TYPE_LINK_STATE *pLinkState,CHAR *pStrOutResult,BOOLEAN isNeedTxtSave,BOOLEAN bOR);
static void SendSaveAckToOmm(CHAR *sOutputResult,BOOLEAN isNeedTxtSave,BOOLEAN bOR);
/**************************************************************************
*                          �ֲ�����ʵ��                                   *
**************************************************************************/

static BOOLEAN char_is_chinese(BYTE ucIn)
{
	return ((ucIn & 0x80) == 0x80);
}

/**************************************************************************
* �������ƣ�VOID PermitChangeMode
* ������������֤�Ƿ���Ըı䵱ǰģʽ
* ���ʵı���
* �޸ĵı���
* ���������TYPE_LINK_STATE *pLinkState
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
static BOOLEAN PermitChangeMode(TYPE_LINK_STATE *pLinkState)
{
/*����ת��������ȥ�������Ҫ�ſ���Ҫע������ת�����õ�Ĭ��linkstate���û�Ȩ������*/
#if 0
    T_OAM_Cfg_Dat * cfgDat = pLinkState->tMatchResult.cfgDat;
    BYTE bNextModeID = cfgDat->tCmdAttr[pLinkState->wCmdAttrPos].bNextModeID;
    WORD16 wModeClass = 0;
    DWORD i;
    BOOLEAN bRet = FALSE;

    /*������һģʽ�Ƿ����*/
    for (i=0; i<=g_dwModeCount; i++)
    {
        if (gtMode[i].bModeID == bNextModeID)
            break;
    }
    if (i > g_dwModeCount)
        return bRet;

    /*��֤Ȩ��*/
    if (pLinkState->ucUserRight >=1 && pLinkState->ucUserRight <= 16)
        wModeClass = 1 << (pLinkState->ucUserRight - 1);
    else
        wModeClass = 0;
    if (gtMode[i].wModeClass & wModeClass)
        bRet= TRUE;
    else
        bRet= FALSE;

    return bRet;
#endif 
    return TRUE;
}

#if 0
/**************************************************************************
* �������ƣ�VOID InnerCmdLogin
* ������������¼
* ���ʵı���
* �޸ĵı���
* ���������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
static VOID InnerCmdLogin(TYPE_LINK_STATE * pLinkState)
{
    SendUserNamePromptToTelnet(pLinkState,NULL);

    pLinkState->ucInputErrTimes  = 0;

    Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_TELNET);

    return;
}
#endif

/**************************************************************************
* �������ƣ�VOID Oam_InnerCmdLogout
* �����������˳�
* ���ʵı���
* �޸ĵı���
* ���������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
void Oam_InnerCmdLogout(TYPE_LINK_STATE *pLinkState)
{
    CHAR szSysLog[128] = {0};
    CHAR *pSysLog = szSysLog;

    if (SaveOamCfgInfo() != WRITEERR_SUCCESS)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "SaveOamCfgInfo failed, user info was not saved!\n");
    }

    if ((USER_LOGOUT_NONE == pLinkState->tUserLog.ucQuitReason) && 
        (0 == pLinkState->tUserLog.tSysClockLogout.wSysYear))
    {
    XOS_GetSysClock(&(pLinkState->tUserLog.tSysClockLogout));
    pLinkState->tUserLog.ucQuitReason = USER_LOGOUT_COMMON;
    if (!UpdateOneUseLog(pLinkState))
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, "[Oam_InnerCmdLogout] UpdateOneUseLog exec failed! line = %d\n", __LINE__);
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

    return;
}

/**************************************************************************
* �������ƣ�VOID InnerCmdEnable
* ����������������Ȩģʽ
* ���ʵı���
* �޸ĵı���
* ���������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
static void InnerCmdEnable(TYPE_LINK_STATE * pLinkState)
{
    WORD16 wIdx = 0;
    /*�ж��Ƿ��Ѿ��������û�����Ȩģʽ*/
    for (wIdx = 0; wIdx < MAX_CLI_VTY_NUMBER; wIdx++)
    {
        if ((TRUE ==  gtLinkState[wIdx].bIsSupperUsrMode) && 
             (pec_MsgHandle_IDLE != Oam_LinkGetCurRunState(&gtLinkState[wIdx])))
        {
            break;
        }
    }

    if( wIdx < MAX_CLI_VTY_NUMBER)
    {
        SendStringToTelnet(pLinkState,  map_szGotoSupperModeErr);
    }
    else
    {
        /* �ж��Ƿ���Ȩ���л�����Ȩģʽ */
        OAM_ChangeMode(pLinkState,NEXTMODE_NORMAL);
        if (pLinkState->bChangeMode)
        {
            /* ֱ�ӽ��� */
            /* ���ݵ�ǰ�û�Ȩ�� */
            pLinkState->ucSavedRight = pLinkState->ucUserRight;
            SendPacketToTelnet(pLinkState, FALSE);
        }
        else
        {
            /* ��Ҫ���볬���û����� */
            SendPasswordPromptToTelnet(pLinkState,NULL);
        }
    }
    Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_TELNET);

    return;
}

/**************************************************************************
* �������ƣ�VOID InnerCmdDisable
* �����������˳���Ȩģʽ
* ���ʵı���
* �޸ĵı���
* ���������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
static void InnerCmdDisable(TYPE_LINK_STATE * pLinkState)
{
    /* �ı�ģʽ */
    OAM_ChangeMode(pLinkState,NEXTMODE_NORMAL);

    SendPacketToTelnet(pLinkState, FALSE);

    Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_TELNET);

    /* �ָ�Ȩ�� */
    pLinkState->ucUserRight = pLinkState->ucSavedRight;

    pLinkState->bIsSupperUsrMode = FALSE;

    return;
}

#if 0
/**************************************************************************
* �������ƣ�VOID InnerCmdConfigure
* ��������������ģʽ
* ���ʵı���
* �޸ĵı���
* ���������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
static VOID InnerCmdConfigure(TYPE_LINK_STATE * pLinkState)
{
    WORD16 ConfigUser = 0;
    BYTE i = 0;
    CHAR TempStr[MAX_STRING_LEN] = {0};

    /*����ÿ���ն�,����м����û�������ģʽ��*/
    for (i = MIN_CLI_VTY_NUMBER; i <= MAX_CLI_VTY_NUMBER; i++)
    {
        BYTE k = i - MIN_CLI_VTY_NUMBER;
        if (Oam_LinkGetCurRunState(&gtLinkState[k]) != pec_MsgHandle_IDLE &&
            Oam_GetCurModeId(&gtLinkState[k]) > CMD_MODE_PRIV)
        {
            ConfigUser++;
        }
    }

    if ( (ConfigUser == 0)  || /*���û����÷�ʽ, �ҵ�ǰ���û�������ģʽ��*/
        Oam_LinkGetLinkId(pLinkState) == MIN_CLI_VTY_NUMBER) /*Console���ڿ���ֱ�ӽ�������ģʽ��������Flag��־*/
    {
        OAM_ChangeMode(pLinkState,NEXTMODE_NORMAL);
        if (pLinkState->bChangeMode == TRUE)
        {
            /*���ն��ϴ�ӡ�澯��ʾ*/
            pLinkState->bOutputResult = TRUE;
            strncpy(pLinkState->sOutputResult,Hdr_szEnterConfigTip,MAX_OUTPUT_RESULT);

            strncat(pLinkState->sOutputResult,"\n",MAX_OUTPUT_RESULT);
        }
    }
    else /*�������������ģʽ����ӡ��ʾ��Ϣ*/
    {
        pLinkState->bOutputResult = TRUE;
        strncpy(pLinkState->sOutputResult,Hdr_szCanNotEnterConfig,MAX_OUTPUT_RESULT);
        for (i = MIN_CLI_VTY_NUMBER; i <= MAX_CLI_VTY_NUMBER; i++)
        {
            BYTE k = i - MIN_CLI_VTY_NUMBER;
            if (Oam_LinkGetCurRunState(&gtLinkState[k]) != pec_MsgHandle_IDLE &&
                 Oam_GetCurModeId(&gtLinkState[k]) > CMD_MODE_PRIV)
            {
                if (i==MIN_CLI_VTY_NUMBER)
                    XOS_snprintf(TempStr,MAX_STRING_LEN, "\ncon0(%s)\n", gtLinkState[k].tnIpAddr);
                else
                    XOS_snprintf(TempStr,MAX_STRING_LEN, "\nvty%u(%s)\n", (Oam_LinkGetLinkId(&gtLinkState[k]) -1), gtLinkState[k].tnIpAddr);
                break;
            }
        }
        strncat(pLinkState->sOutputResult,TempStr,MAX_OUTPUT_RESULT);
    }

    SendPacketToTelnet(pLinkState, FALSE);

    return;
}
#endif

/**************************************************************************
* �������ƣ�VOID InnerCmdExit
* �����������˳�ģʽ
* ���ʵı���
* �޸ĵı���
* ���������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
static void InnerCmdExit(TYPE_LINK_STATE * pLinkState)
{
    OAM_ChangeMode(pLinkState,NEXTMODE_NORMAL);
    
    /*CRDCM00096832 �˳�ģʽ���Ӧ�ð�Ӧ���Լ�������ֶ���� */
/*    
    if (CMD_MODE_PROTOCOL == Oam_GetCurModeId(pLinkState))
    {
        memset(&pLinkState->LastData, 0, sizeof(pLinkState->LastData));
        memset(&pLinkState->BUF, 0, sizeof(pLinkState->BUF));
    }
*/
    SendPacketToTelnet(pLinkState, FALSE);

    return;
}
/*
static OPR_DATA *TestGetNextOprData(OPR_DATA *pOprData)
{
    return (OPR_DATA *)((BYTE *)pOprData->Data + pOprData->Len);
}
*/
typedef struct cli_map
{
    WORD16 wType;
    WORD16 wValue;
}T_Cli_Map;


 void AddParaToMsg(MSG_COMM_OAM *pMsg, BYTE ucType, BYTE *pData, WORD16 wcLen)
{
    OPR_DATA *pRtnOprData = (OPR_DATA *)pMsg->Data;
    int iCurParaLen;
    int i = 0;
    for (i = 0; i<pMsg->Number; i++)
    {
        pRtnOprData = GetNextOprData(pRtnOprData);//TestGetNextOprData(pRtnOprData);
    }

    pRtnOprData->ParaNo = pMsg->Number;
    pRtnOprData->Type = ucType;
    if (ucType == DATA_TYPE_STRING)
    {
        //�ַ������Ͱ���ʵ�ʳ��ȿ���
        memcpy(pRtnOprData->Data, pData, wcLen+1);
        pRtnOprData->Data[wcLen] = 0;
        iCurParaLen = wcLen + 1;
        if (iCurParaLen > 0xFF)
        {
            pRtnOprData->Len = 0;
        }
        else
        {
            pRtnOprData->Len = (BYTE)iCurParaLen;
        }
    }
    else
    {
        memcpy(pRtnOprData->Data, pData, wcLen);
        iCurParaLen = wcLen;
        pRtnOprData->Len = (BYTE)iCurParaLen;
    }

    pMsg->Number++;
    pMsg->DataLen = (BYTE *)pRtnOprData->Data + iCurParaLen - (BYTE *)pMsg;
}
 
void AddMapParaToMsg(MSG_COMM_OAM *pMsg, WORD16 wType, WORD16 wValue)
{
    T_Cli_Map tMap;
    tMap.wType = wType;
    tMap.wValue = wValue;

    AddParaToMsg(pMsg, DATA_TYPE_MAP, (BYTE *)&tMap, sizeof(tMap));
}

void AddXmlRpcMapParaToCliMsg(MSG_COMM_OAM *pMsg, WORD32 dwMap)
{
    T_Cli_Map tMap;
    memcpy(&tMap, (BYTE *)&dwMap, sizeof(dwMap));

    AddMapParaToMsg(pMsg,  tMap.wType, tMap.wValue);
}

/*
VOID AddMapParaToMsg(MSG_COMM_OAM *pMsg,WORD16 wMapType, WORD16 wMapValue)
{
    OPR_DATA *pRtnOprData = (OPR_DATA *)pMsg->Data;	
    int i = 0;
    for (i = 0; i<pMsg->Number; i++)
    {
        pRtnOprData = TestGetNextOprData(pRtnOprData);
    }		
    SetMapOprData(pMsg, pRtnOprData, wMapType, wMapValue);
    pMsg->DataLen = (BYTE *)pRtnOprData->Data + pRtnOprData->Len - (BYTE *)pMsg;

}
*/
/**************************************************************************
* �������ƣ�VOID ConstructShowModeAckMsgParam
* ����������������ѯģʽջӦ����Ϣ����
* ���ʵı���
* �޸ĵı���
* ���������BYTE bModeStackTop:ģʽջ��ָ��
                             BYTE *paucModeId:ģʽջ
* ���������MSG_COMM_OAM *ptRtnMsg:OAM��Ϣ��
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/01/07    V1.0    ������      ����
**************************************************************************/
void ConstructShowModeAckMsgParam(BYTE bModeStackTop,BYTE *paucModeId,MSG_COMM_OAM *ptRtnMsg)
{
    BYTE bFlag = 0;/*���Ʊ��*/
    BYTE iLoop = 0; 
    BYTE *piLoopCnt = NULL; /*ѭ�����ص��ַ�����Ŀ*/
    BYTE bModeId = 0;/*ģʽID*/
    DWORD dwModeLoop = 0;/*ģʽ�����±�*/
    BYTE aucOutResult[MAX_STRING_LEN] = {0};

    /*�������*/
    if ((NULL == paucModeId) || (NULL == ptRtnMsg))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return;
    }
    /*��·״̬Ϊ��*/    
    if ((0 == bModeStackTop) || (1 == bModeStackTop))
    {
        bFlag = 1;
        AddParaToMsg(ptRtnMsg , DATA_TYPE_BYTE,&bFlag, (BYTE)1);
    }
    /*��ӡ���Խű�*/
    else
    {
        bFlag = 2;
        AddParaToMsg(ptRtnMsg , DATA_TYPE_BYTE,&bFlag, (BYTE)1);

        /*�����һ����ʾѭ�������Ļ��Բ���*/    
	 iLoop = 0;
        AddParaToMsg(ptRtnMsg, DATA_TYPE_BYTE, (BYTE *)&iLoop, (BYTE)1);    
	 piLoopCnt = (BYTE *)ptRtnMsg->Data[ptRtnMsg->Number - 1].Data;
        for (iLoop = bModeStackTop-1;iLoop >= 1;iLoop--)
        {
            bModeId = paucModeId[iLoop];
            AddParaToMsg(ptRtnMsg , DATA_TYPE_BYTE,&bModeId, (BYTE)1);
	     for (dwModeLoop = 0; dwModeLoop <= g_dwModeCount; dwModeLoop++)
            {
                if (gtMode[dwModeLoop].bModeID == bModeId)
                break;
            }
	     memset(aucOutResult,0,MAX_STRING_LEN);
	     if (dwModeLoop <= g_dwModeCount)
	     {
	         strncat((CHAR *)aucOutResult, gtMode[dwModeLoop].sOddPrompt, sizeof(aucOutResult) - 1);
	     }	 
	     AddParaToMsg(ptRtnMsg, DATA_TYPE_STRING, aucOutResult, strlen((CHAR *)aucOutResult)+1);
	     *piLoopCnt += 1;
        }
        ptRtnMsg->BUF[0] = 0;
        ptRtnMsg->ReturnCode = SUCC_AND_HAVEPARA;
    }
}

/**************************************************************************
* �������ƣ�VOID InnerCmdProcessForXmlAgent
* �������������������(XML������ר��)
* ���ʵı���
* �޸ĵı���
* ���������MSG_COMM_OAM *ptOamMsg:OAM��������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/01/07    V1.0    ������      ����
**************************************************************************/
static void InnerCmdProcessForXmlAgent(MSG_COMM_OAM *ptOamMsg)
{    
    JID tDtJid;                                               /*Ŀ��JID*/
    MSG_COMM_OAM *ptRtnMsg = (MSG_COMM_OAM *)g_ptOamIntVar->aucRecvBuf;/*���ؽ��*/
    /*�������*/
    if (NULL == ptOamMsg)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return;
    }
    /*��ʼ������*/
    memset(&tDtJid,0,sizeof(JID));
    memcpy(ptRtnMsg,ptOamMsg,ptOamMsg->DataLen);
    ptRtnMsg->Number = 0;
    ptRtnMsg->DataLen  = sizeof(MSG_COMM_OAM);
    ptRtnMsg->OutputMode = OUTPUT_MODE_NORMAL;
    ptRtnMsg->ReturnCode = SUCC_AND_NOPARA;
    /*��ȡ����JID*/
    if (XOS_SUCCESS != XOS_GetSelfJID(&tDtJid))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XOS_GetSelfJID Error! ....................",__func__,__LINE__);
        return;
    }
    /*��OAM���ؽ��*/
    XOS_SendAsynMsg(EV_OAM_CFG_CFG, (BYTE *)ptRtnMsg, ptRtnMsg->DataLen, XOS_MSG_VER0, XOS_MSG_MEDIUM, &tDtJid);
    XOS_SysLog(OAM_CFG_LOG_NOTICE,
                       "....................FUNC:%s LINE:%d SendInnerCmdAck Successfully! ....................",__func__,__LINE__);
    return;
}

/**************************************************************************
* �������ƣ�VOID InnerCmdSetBanner
* ��������������Banner
* ���ʵı���
* �޸ĵı���
* ���������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
static void InnerCmdSetBanner(TYPE_LINK_STATE *pLinkState)
{
    TYPE_MATCH_RESULT * pMatchResult = &(pLinkState->tMatchResult);

    if (pMatchResult->bHasNo == FALSE)
    {
        /* �޸�ΪText������ʾ�� */
        SendTextPromptToTelnet(pLinkState);
    }
    else
    {
        /* ����sBanner */
        strncpy(gtCommonInfo.sBanner, DEFAULT_BANNER, sizeof(gtCommonInfo.sBanner));
        gtCommonInfo.sBanner[MAX_BANNER_LEN - 1] = MARK_STRINGEND;

        SendPacketToTelnet(pLinkState, FALSE);
    }

    return;
}

/**************************************************************************
* �������ƣ�VOID InnerCmdSetPrompt
* ��������������Prompt
* ���ʵı���
* �޸ĵı���
* ���������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
static void InnerCmdSetPrompt(TYPE_LINK_STATE *pLinkState)
{
    TYPE_MATCH_RESULT * pMatchResult = &(pLinkState->tMatchResult);

    /* ����sPrompt */
    if (pMatchResult->bHasNo == FALSE)
    {
        strncpy(gtCommonInfo.sPrompt, (CHAR *)pMatchResult->tPara[0].pValue, sizeof(gtCommonInfo.sPrompt));
        gtCommonInfo.sPrompt[MAX_PROMPT_LEN - 1] = MARK_STRINGEND;
    }
    else
    {
        strncpy(gtCommonInfo.sPrompt, DEFAULT_PROMPT, sizeof(gtCommonInfo.sPrompt));
        gtCommonInfo.sPrompt[MAX_PROMPT_LEN - 1] = MARK_STRINGEND;
    }

    SendNormalPromptToTelnet(pLinkState,NULL);

    return;
}
 BOOL CheckRuleOfPwd(CHAR *pPassWord)
{
    BYTE ucUpperCnt = 0;
    BYTE ucLowerCnt = 0;
    BYTE ucSpecial = 0;
    CHAR *pPwd = pPassWord;
    
    if (!pPassWord)
        return FALSE;

    if (strlen(pPassWord) >= MAX_OAM_USER_PWD_LEN ||
        strlen(pPassWord) < MIN_OAM_USER_PWD_LEN)
    {
        return FALSE;
    }

    while (pPwd != NULL && *pPwd != MARK_STRINGEND)
    {
        if (*pPwd >= 'a' &&
            *pPwd <= 'z')
            ucLowerCnt++;

        if (*pPwd >= 'A' &&
            *pPwd <= 'Z')
            ucUpperCnt++;


        if (*pPwd == '~' || *pPwd == '!' || *pPwd == '@' || *pPwd == '#' || *pPwd == '$' ||
            *pPwd == '%' || *pPwd == '^' || *pPwd == '&' || *pPwd == '*' || *pPwd == '(' ||
            *pPwd == ')' || *pPwd == '_' || *pPwd == '+' || *pPwd == '|' || *pPwd == '`' ||
            *pPwd == '-' || *pPwd == '=' || *pPwd == '\\' || *pPwd == '{' || *pPwd == '}' ||
            *pPwd == '[' || *pPwd == ']' || *pPwd == ':' || *pPwd == '"' || *pPwd == ';' ||
            *pPwd == '\'' || *pPwd == '<' || *pPwd == '>' || *pPwd == ',' || *pPwd == '.' || *pPwd == '/')
            ucSpecial++;

        pPwd++;
        
    }

    if (ucLowerCnt == 0 ||
        ucUpperCnt == 0 ||
        ucSpecial == 0)
    {
        return FALSE;
    }
    
    return TRUE;
}


/**************************************************************************
*                          ȫ�ֺ���ʵ��                                   *
**************************************************************************/
/**************************************************************************
* �������ƣ�VOID ChangePrompt
* �����������ı���ʾ��
* ���ʵı���
* �޸ĵı���
* ���������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
void ChangePrompt(TYPE_LINK_STATE *pLinkState, BYTE bModeId)
{
    CHAR pOutStr[MAX_PROMPT_LEN] = {0};
    DWORD k = 0;

    *pOutStr = MARK_STRINGEND;

    if (!gtMode)
    { 
        return;
    }
				
    for (k = 0; k <= g_dwModeCount; k++)
    {
        if (gtMode[k].bModeID == bModeId)
            break;
    }

    if (k <= g_dwModeCount)
    {
        WORD16 wPromptLen    = strlen(gtCommonInfo.sPrompt);
        WORD16 wOddPromptLen = strlen(gtMode[k].sOddPrompt);
		
        XOS_SysLog(OAM_CFG_LOG_DEBUG, "611002030455 ChangePrompt called, gtCommonInfo.sPrompt:[%s], gtMode[%u].sOddPrompt:[%s] \n",
	                      gtCommonInfo.sPrompt, k, gtMode[k].sOddPrompt);
        /* �����ʾ�����ȳ���32,����ʾΪ: RPU...(config)#
           ����: wOddPromptLen Ӧ������� */
        if ((wPromptLen+wOddPromptLen) > (WORD16)(MAX_PROMPT_LEN-1-strlen(pOutStr)))
        {
            if (wOddPromptLen < (unsigned)(MAX_PROMPT_LEN-1-strlen(pOutStr)-3))
            {
                strncat(pOutStr, gtCommonInfo.sPrompt, sizeof(pOutStr));
            }
            strncat(pOutStr, "...", sizeof(pOutStr));
        }
        else
        {
            strncat(pOutStr, gtCommonInfo.sPrompt, sizeof(pOutStr));
        }
        strncat(pOutStr, gtMode[k].sOddPrompt, sizeof(pOutStr));
    }
    else
    {
        strncat(pOutStr, gtCommonInfo.sPrompt, sizeof(pOutStr));
    }

    strncpy(pLinkState->sPrompt, pOutStr, sizeof(pLinkState->sPrompt));
}
/**************************************************************************
* �������ƣ�VOID OAM_ChangeMode
* �����������ı䵱ǰģʽ
* ���ʵı���
* �޸ĵı���
* ���������TYPE_LINK_STATE *pLinkState,BYTE bNextMode
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
void OAM_ChangeMode(TYPE_LINK_STATE *pLinkState,BYTE bModeId)
{
    BYTE bNextModeID;
    T_OAM_Cfg_Dat * cfgDat = pLinkState->tMatchResult.cfgDat;
    BOOLEAN bPermitChangeMode = FALSE;

    if (!cfgDat)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,"pLinkState->tMatchResult.cfgDat is NULL! can't change mode to %d \n", bModeId);
        return;
    }
	
    if (pLinkState->wCmdAttrPos >= cfgDat->nCmdAttrSum)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,"cmd has been reloaded, can't change mode to %d \n", bModeId);
        return;
    }

    bNextModeID = cfgDat->tCmdAttr[pLinkState->wCmdAttrPos].bNextModeID;
    bPermitChangeMode = FALSE;
    pLinkState->bChangeMode = FALSE;

    /*��ǰΪno����ʱ������*/
    if (Oam_LinkIfCurCmdIsNo(pLinkState))
        return;

    if (bModeId == NEXTMODE_NORMAL)
    {
        bNextModeID = cfgDat->tCmdAttr[pLinkState->wCmdAttrPos].bNextModeID;

        /*��ǰģʽ����һģʽ��ͬʱ������*/
        if ((bNextModeID == CMD_MODE_SAME) ||
                (bNextModeID == Oam_GetCurModeId(pLinkState)))
            return;

        /*��һģʽΪǰһģʽ������֤Ȩ��*/
        if (bNextModeID == CMD_MODE_FORWARD)
            bPermitChangeMode = TRUE;
        else
             /*��֤Ȩ��*/
            bPermitChangeMode = PermitChangeMode(pLinkState);
    }
    else
    {
        /*��һģʽΪ�ض�ģʽ������֤Ȩ��*/
        bNextModeID = bModeId;
        bPermitChangeMode = TRUE;
    }

    /*��Ȩ�޲����л�����һģʽ*/
    if (bPermitChangeMode == FALSE)
    {
        if (bNextModeID != CMD_MODE_PRIV)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR,
                 "OAM_ChangeMode: can not change to next mode !\n");
            SendStringToTelnet(pLinkState,Hdr_szCanNotChangMode);  /*���ش�����Ϣ*/
        }
        return;
    }

    if (bNextModeID != CMD_MODE_FORWARD)  /*��һģʽ��ǰһģʽ*/
    {
        if (bNextModeID > Oam_GetCurModeId(pLinkState))
        {
            Oam_LinkModeStack_Push(pLinkState, bNextModeID);
        }
        else
        {
            /*���˵�ָ��ģʽ*/
            do
            {
                 Oam_LinkModeStack_Pop(pLinkState);
            }while (!Oam_LinkModeStatck_IsEmpty(pLinkState) && bNextModeID < Oam_GetCurModeId(pLinkState));
        }
    }
    else /*��һģʽΪǰһģʽ*/
    {
        /*���˵�ǰһģʽ*/
        Oam_LinkModeStack_Pop(pLinkState);
    }

    if (Oam_LinkGetInputState(pLinkState) == INMODE_NORMAL)
        ChangePrompt(pLinkState,Oam_GetCurModeId(pLinkState));

    pLinkState->bChangeMode = TRUE;

    return;

}

/**************************************************************************
* �������ƣ�VOID SendLogoutToTelnet
* �����������˳�
* ���ʵı���
* �޸ĵı���
* ���������TYPE_LINK_STATE *pLinkState,CHAR *sOutputResult
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
void SendLogoutToTelnet(TYPE_LINK_STATE *pLinkState,CHAR *sOutputResult)
{
    if (!pLinkState)
    {
        return;
    }

    /* 1.ɱ��Telnet�˶�ʱ�� */
    Oam_KillIdleTimer(pLinkState);
    /* 614000678048 (ȥ��24Сʱ���Գ�ʱ) */
    /*Oam_KillAbsTimer(pLinkState);*/

    /* 2.ɱ��Execute�˶�ʱ��, ���ٵȴ�Execute��ִ������ؽ�� */
    Oam_KillExeTimer(pLinkState);

    Oam_KillCmdPlanTimer(pLinkState);

    /* 3.�����˳���Ϣ */
    Oam_LinkMarkLogout(pLinkState, TRUE);

    SendStringToTelnet(pLinkState,sOutputResult);

    /* 4.��Ϊ����״̬ */
    Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_IDLE);

    return;
}

/**************************************************************************
* �������ƣ�VOID SendExeMsgErrToTelnet
* ��������������Execute��Ϣͷ������Ϣ
* ���ʵı���
* �޸ĵı���
* ���������TYPE_LINK_STATE *pLinkState,INT32 iErrCode
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
void SendExeMsgErrToTelnet(TYPE_LINK_STATE *pLinkState,INT32 iErrCode)
{
    CHAR strErrInfo[MAX_STRING_LEN];

    /* 1.���ݴ��������ɴ�����Ϣ */
    /*CRDCM00313101 ��XOS_snprintf�滻strcpy��strcat����*/
    XOS_snprintf(strErrInfo,
                 sizeof(strErrInfo) -1,
                 "%s",
                 Err_szExeMsgHead);

    switch(iErrCode)
    {
    case MSGERR_RETURNCODE:
        /*CRDCM00313101 ��XOS_snprintf�滻strcpy��strcat����*/
        XOS_snprintf(strErrInfo,
                     sizeof(strErrInfo) -1,
                     "%s%s",
                     strErrInfo,
                     Err_szReturnCode);
        break;
    case MSGERR_LINKCHANNEL:
        /*CRDCM00313101 ��XOS_snprintf�滻strcpy��strcat����*/
        XOS_snprintf(strErrInfo,
                     sizeof(strErrInfo) -1,
                     "%s%s",
                     strErrInfo,
                     Err_szLinkChannel);
        break;
    case MSGERR_SEQNO:
        /*CRDCM00313101 ��XOS_snprintf�滻strcpy��strcat����*/
        XOS_snprintf(strErrInfo,
                     sizeof(strErrInfo) -1,
                     "%s%s",
                     strErrInfo,
                     Err_szSeqNo);
        break;
    case MSGERR_LASTPACKET:
        /*CRDCM00313101 ��XOS_snprintf�滻strcpy��strcat����*/
        XOS_snprintf(strErrInfo,
                     sizeof(strErrInfo) -1,
                     "%s%s",
                     strErrInfo,
                     Err_szLastPacket);
        break;
    case MSGERR_OUTPUTMODE:
        /*CRDCM00313101 ��XOS_snprintf�滻strcpy��strcat����*/
        XOS_snprintf(strErrInfo,
                     sizeof(strErrInfo) -1,
                     "%s%s",
                     strErrInfo,
                     Err_szOutputMode);
        break;
    case MSGERR_CMDID:
        /*CRDCM00313101 ��XOS_snprintf�滻strcpy��strcat����*/
        XOS_snprintf(strErrInfo,
                     sizeof(strErrInfo) -1,
                     "%s%s",
                     strErrInfo,
                     Err_szCmdID);
        break;
    case MSGERR_NUMBER:
        /*CRDCM00313101 ��XOS_snprintf�滻strcpy��strcat����*/
        XOS_snprintf(strErrInfo,
                     sizeof(strErrInfo) -1,
                     "%s%s",
                     strErrInfo,
                     Err_szNumber);
        break;
    case MSGERR_EXECMODEID:
        /*CRDCM00313101 ��XOS_snprintf�滻strcpy��strcat����*/
        XOS_snprintf(strErrInfo,
                     sizeof(strErrInfo) -1,
                     "%s%s",
                     strErrInfo,
                     Err_szExecModeID);
        break;
    case MSGERR_IFNO:
        /*CRDCM00313101 ��XOS_snprintf�滻strcpy��strcat����*/
        XOS_snprintf(strErrInfo,
                     sizeof(strErrInfo) -1,
                     "%s%s",
                     strErrInfo,
                     Err_szIfNo);
        break;
    case MSGERR_FROMINTPR:
        /*CRDCM00313101 ��XOS_snprintf�滻strcpy��strcat����*/
        XOS_snprintf(strErrInfo,
                     sizeof(strErrInfo) -1,
                     "%s%s",
                     strErrInfo,
                     Err_szFromIntpr);
        break;
    case MSGERR_NONEEDTHECMD:
        /*CRDCM00313101 ��XOS_snprintf�滻strcpy��strcat����*/
        XOS_snprintf(strErrInfo,
                     sizeof(strErrInfo) -1,
                     "%s%s",
                     strErrInfo,
                     Err_szNoNeedTheCmd);
        break;
    default:
        /*CRDCM00313101 ��XOS_snprintf�滻strcpy��strcat����*/
        XOS_snprintf(strErrInfo,
                     sizeof(strErrInfo) -1,
                     "%s%s",
                     strErrInfo,
                     Err_szUndefined);
        break;
    }

    /* 2.��Telnet������Ϣ */
    SendStringToTelnet(pLinkState,strErrInfo);

    /* 3.�������� */
    Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_TELNET);
    if (Oam_LinkGetCurRunState(pLinkState) == pec_MsgHandle_EXECUTE)
    {
        Oam_SetExeTimer(pLinkState);
    }
}

/**************************************************************************
* �������ƣ�VOID SendTelMsgErrToTelnet
* ��������������Telnet��Ϣͷ������Ϣ
* ���ʵı���
* �޸ĵı���
* ���������TYPE_LINK_STATE *pLinkState,INT32 iErrCode
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
void SendTelMsgErrToTelnet(TYPE_LINK_STATE *pLinkState,INT32 iErrCode)
{
    CHAR strErrInfo[MAX_STRING_LEN];

    /* 1.���ݴ��������ɴ�����Ϣ */
    /*CRDCM00313101 ��XOS_snprintf�滻strcpy��strcat����*/
    XOS_snprintf(strErrInfo,
                 sizeof(strErrInfo) -1,
                 "%s",
                 Err_szTelMsgHead);
    switch(iErrCode)
    {
    case MSGERR_MSGID:
        /*CRDCM00313101 ��XOS_snprintf�滻strcpy��strcat����*/
        XOS_snprintf(strErrInfo,
                     sizeof(strErrInfo) -1,
                     "%s%s",
                     strErrInfo,
                     Err_szMsgId);
        break;
    case MSGERR_VTYNUM:
        /*CRDCM00313101 ��XOS_snprintf�滻strcpy��strcat����*/
        XOS_snprintf(strErrInfo,
                     sizeof(strErrInfo) -1,
                     "%s%s",
                     strErrInfo,
                     Err_szVtyNum);
        break;
    case MSGERR_DETERMINER:
        /*CRDCM00313101 ��XOS_snprintf�滻strcpy��strcat����*/
        XOS_snprintf(strErrInfo,
                     sizeof(strErrInfo) -1,
                     "%s%s",
                     strErrInfo,
                     Err_szDeterminer);
        break;
    default:
        /*CRDCM00313101 ��XOS_snprintf�滻strcpy��strcat����*/
        XOS_snprintf(strErrInfo,
                     sizeof(strErrInfo) -1,
                     "%s%s",
                     strErrInfo,
                     Err_szUndefined);
        break;
    }

    /* 2.��Telnet������Ϣ */
    SendStringToTelnet(pLinkState,strErrInfo);

    Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_TELNET);

    return;
}

/**************************************************************************
* �������ƣ�VOID SendUserNameErrToTelnet
* �����������û�������
* ���ʵı���
* �޸ĵı���
* ���������TYPE_LINK_STATE *pLinkState,CHAR *sOutputResult
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
void SendUserNameErrToTelnet(TYPE_LINK_STATE *pLinkState,CHAR *sOutputResult)
{
    SendNormalPromptToTelnet(pLinkState,sOutputResult);

 //   pLinkState->ucInputErrTimes = 0;

    Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_TELNET);

    return;
}

/**************************************************************************
* �������ƣ�VOID SendPasswordErrToTelnet
* �����������������
* ���ʵı���
* �޸ĵı���
* ���������TYPE_LINK_STATE *pLinkState,CHAR *sOutputResult
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
void SendPasswordErrToTelnet(TYPE_LINK_STATE *pLinkState,CHAR *sOutputResult)
{
    T_OAM_Cfg_Dat * cfgDat = pLinkState->tMatchResult.cfgDat;
    switch(cfgDat->tCmdAttr[pLinkState->wCmdAttrPos].dExeID)
    {
#if 0        
    case CMD_INTERPT_LOGIN:
        /* �ָ��û�����Ȩ�� */
        strncpy(pLinkState->sUserName,pLinkState->sSavedUser);
        pLinkState->ucUserRight = pLinkState->ucSavedRight;
        break;
#endif        
    case CMD_INTERPT_ENABLE:
        break;
    default:
        break;
    }

    SendNormalPromptToTelnet(pLinkState,sOutputResult);

   // pLinkState->ucInputErrTimes = 0;

    Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_TELNET);

    return;
}

/**************************************************************************
* �������ƣ�VOID SendSysInitInfoToTelnet
* ��������������ϵͳ��ʼ����Ϣ���û�����ʾ��
* ���ʵı���
* �޸ĵı���
* ���������TYPE_LINK_STATE *pLinkState
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
void SendSysInitInfoToTelnet(TYPE_LINK_STATE *pLinkState)
{
    SendUserNamePromptToTelnet(pLinkState, gtCommonInfo.sBanner);

    pLinkState->ucInputErrTimes = 0;

    Oam_LinkSetCurRunState(pLinkState, pec_Init_USERNAME);

    return;
}

/**************************************************************************
* �������ƣ�VOID SendStartupToTelnet
* ��������������������Ϣ
* ���ʵı���
* �޸ĵı���
* ���������TYPE_LINK_STATE *pLinkState
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
void SendStartupToTelnet(TYPE_LINK_STATE *pLinkState)
{
    /* ��ʼ��Ϊ�û�ģʽ */
    Oam_LinkModeStack_Init(pLinkState);
    Oam_LinkModeStack_Push(pLinkState, CMD_MODE_PRIV/*CMD_MODE_USER*/);

    SendNormalPromptToTelnet(pLinkState,NULL);

    pLinkState->ucInputErrTimes = 0;

    Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_TELNET);

    return;
}

/**************************************************************************
* �������ƣ�VOID SendUserNamePromptToTelnet
* ���������������û���ʾ��
* ���ʵı���
* �޸ĵı���
* ���������TYPE_LINK_STATE *pLinkState, CHAR *sOutputResult
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
void SendUserNamePromptToTelnet(TYPE_LINK_STATE *pLinkState, CHAR *sOutputResult)
{
    /* �ı���ʾ�� */
    pLinkState->bChangeMode     = TRUE;
    /*CRDCM00313101 ��XOS_snprintf�滻strcpy��strcat����*/
    XOS_snprintf(pLinkState->sPrompt,
                 sizeof(pLinkState->sPrompt) -1,
                 "%s",
                 Hdr_szUserName);

    /* �ı�����ģʽ */
    Oam_LinkSetInputState(pLinkState, INMODE_USERNAME);

    SendStringToTelnet(pLinkState,sOutputResult);

//    pLinkState->ucInputErrTimes = 0;

    return;
}

/**************************************************************************
* �������ƣ�VOID SendPasswordPromptToTelnet
* ��������������������ʾ��
* ���ʵı���
* �޸ĵı���
* ���������TYPE_LINK_STATE *pLinkState, CHAR *sOutputResult
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
void SendPasswordPromptToTelnet(TYPE_LINK_STATE *pLinkState, CHAR *sOutputResult)
{
    /* �ı���ʾ�� */
    pLinkState->bChangeMode     = TRUE;
    /*CRDCM00313101 ��XOS_snprintf�滻strcpy��strcat����*/
    XOS_snprintf(pLinkState->sPrompt,
                 sizeof(pLinkState->sPrompt) -1,
                 "%s",
                 Hdr_szPassword);
    /*OSS_Printf(PRINT_OAM_PLATCFGAGT, PRN_NORMAL, PRN_LEVEL_NORMAL,
                  "pLinkState->sPrompt=%s\n",Hdr_szPassword);*/

    /* �ı�����ģʽ */
    Oam_LinkSetInputState(pLinkState, INMODE_PASSWORD);

    pLinkState->bOutputCmdLine = TRUE;
    SendStringToTelnet(pLinkState,sOutputResult);
    /*OSS_Printf(PRINT_OAM_PLATCFGAGT, PRN_NORMAL, PRN_LEVEL_NORMAL,
                  "SendPasswordPromptToTelnet:sOutputResult=%s\n",sOutputResult);*/


//    pLinkState->ucInputErrTimes = 0;

    return;
}

/**************************************************************************
* �������ƣ�VOID SendNormalPromptToTelnet
* ��������������Normal��ʾ��
* ���ʵı���
* �޸ĵı���
* ���������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
void SendNormalPromptToTelnet(TYPE_LINK_STATE *pLinkState, CHAR *sOutputResult)
{
    /* �ı���ʾ�� */
    pLinkState->bChangeMode   = TRUE;
    ChangePrompt(pLinkState,Oam_GetCurModeId(pLinkState));

    /* �ı�����ģʽ */
    Oam_LinkSetInputState(pLinkState, INMODE_NORMAL);

    pLinkState->bOutputCmdLine = TRUE;
    SendStringToTelnet(pLinkState,sOutputResult);

    Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_TELNET);

    return;

}

/**************************************************************************
* �������ƣ�VOID SendTextPromptToTelnet
* ��������������Text��ʾ��
* ���ʵı���
* �޸ĵı���
* ���������TYPE_LINK_STATE * pLinkState
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
void SendTextPromptToTelnet(TYPE_LINK_STATE * pLinkState)
{
    TYPE_MATCH_RESULT *pMatchResult = &(pLinkState->tMatchResult);
    TYPE_TEXT_INPUT *pTextInput = &(pLinkState->tInputControl);

    /* ����TextInput��EndMark�ַ� */
    memset(pTextInput, 0, sizeof(TYPE_TEXT_INPUT));
    pTextInput->cEndMark = *(pMatchResult->tPara[0].pValue);

    /* �����ʾ�ַ��� */
    pLinkState->bOutputResult = TRUE;
    XOS_snprintf(pLinkState->sOutputResult, MAX_OUTPUT_RESULT, Hdr_szTelInputText, pTextInput->cEndMark);
    XOS_snprintf(pLinkState->sOutputResult, MAX_OUTPUT_RESULT - 1, "%s\n", pLinkState->sOutputResult);

    /* �ı���ʾ�� */
    pLinkState->bChangeMode     = TRUE;
    strncpy(pLinkState->sPrompt, ">", sizeof(pLinkState->sPrompt));

    /* �ı�����ģʽ */
    Oam_LinkSetInputState(pLinkState, INMODE_TEXT);

    SendPacketToTelnet(pLinkState, FALSE);

    Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_TELNET);

    return;
}

/**************************************************************************
* �������ƣ�VOID SendCeaseToTelnet
* ����������ȡ������
* ���ʵı���
* �޸ĵı���
* ���������TYPE_LINK_STATE * pLinkState
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
void SendCeaseToTelnet(TYPE_LINK_STATE *pLinkState)
{
    CHAR aucTmpCeaseTip[100] = {0};
    /*CRDCM00313101 ��XOS_snprintf�滻strcpy��strcat����*/
    XOS_snprintf(aucTmpCeaseTip,
                 sizeof(aucTmpCeaseTip) -1,
                 Hdr_szCeasedTheCmdTip);


    pLinkState->bOutputMode     = OUTPUT_MODE_NORMAL;
    pLinkState->bOutputCmdLine  = TRUE;
    pLinkState->bRecvAsynMsg = FALSE;
    pLinkState->wAppMsgReturnCode = ERR_AND_HAVEPARA;
    SendStringToTelnet(pLinkState, aucTmpCeaseTip);
    Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_TELNET);
    /*pLinkState->bOutputCmdLine  = TRUE;*/

#if 0    
    if (pLinkState->wSendPos)
    {
        pLinkState->bOutputResult    = FALSE;
        *(pLinkState->sOutputResult) = MARK_STRINGEND;
        pLinkState->wSendPos         = 0;
    }


    SendPacketToTelnet(pLinkState, FALSE);
    if ((pLinkState->tMatchResult.dExeID == CMD_LPTL_PING) ||
        (pLinkState->tMatchResult.dExeID == CMD_LPTL_IPV6_PING))
        Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_EXECUTE);
    else
        Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_TELNET);
#endif
    return;
}

/**************************************************************************
* �������ƣ�VOID SendAskInfoToTelnet
* ��������������AskInfo
* ���ʵı���
* �޸ĵı���
* ���������TYPE_LINK_STATE *pLinkState,INT32 iPos
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
void SendAskInfoToTelnet(TYPE_LINK_STATE *pLinkState,INT32 iPos)
{
    /* �����ʾ�ַ��� */
    pLinkState->bOutputResult   = TRUE;
    memset(pLinkState->sOutputResult, MARK_BLANK, iPos - 1);
    pLinkState->sOutputResult[iPos - 1] = MARK_POS;
    pLinkState->sOutputResult[iPos]     = 0;
    /*CRDCM00313101 ��XOS_snprintf�滻strcpy��strcat����*/
    XOS_snprintf(pLinkState->sOutputResult,
                 MAX_OUTPUT_RESULT -1,
                 "%s\n%s\n",
                 pLinkState->sOutputResult,
                 CLI_szCLIHelp2);

    SendPacketToTelnet(pLinkState, FALSE);

    Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_TELNET);

    return;
}

/**************************************************************************
* �������ƣ�VOID SendStringToTelnet
* ���������������ַ���
* ���ʵı���
* �޸ĵı���
* ���������TYPE_LINK_STATE *pLinkState,CHAR *sOutputResult
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
void SendStringToTelnet(TYPE_LINK_STATE *pLinkState,CHAR *sOutputResult)
{
    BOOLEAN bSSHMsg = FALSE;
 
    if (!pLinkState || !pLinkState->sOutputResult)
    {
        return;
    }

    if (pLinkState->ucLinkID >= 5 && pLinkState->ucLinkID <= 7)
        bSSHMsg = TRUE;
    /* ����������ʾ�ַ�������� */
    if (sOutputResult)
    {
        pLinkState->bOutputResult = TRUE;
        /*CRDCM00313101 ��XOS_snprintf�滻strcpy��strcat����*/
        XOS_snprintf(pLinkState->sOutputResult,
                     MAX_OUTPUT_RESULT -1,
                     "%s",
                     sOutputResult);
        if (pLinkState->bOutputMode != OUTPUT_MODE_ROLL)
        {
            if (pLinkState->sOutputResult[strlen(pLinkState->sOutputResult)-1] != MARK_RETURN)
            {
                XOS_snprintf(pLinkState->sOutputResult, 
                             MAX_OUTPUT_RESULT - 1, 
                             "%s\n", 
                             pLinkState->sOutputResult);
            }
        }
    }
    else
    {
        pLinkState->bOutputResult = FALSE;
    }

    if ((pLinkState->bSSHAuthMsg && !pLinkState->bSSHAuthRslt) ||
        (bSSHMsg && !pLinkState->bSSHConnRslt))
    {
        SendPacketToTelnet(pLinkState, TRUE);
        memset(g_ptOamIntVar->sOutputResultBuf, 0, MAX_OUTPUT_RESULT);
        pLinkState->wSendPos = 0;
    }
    else
        SendPacketToTelnet(pLinkState, FALSE);

    return;
}

/**************************************************************************
* �������ƣ�VOID SendErrStringToClient
* �������������ش�����Ϣ���ͻ���
* ���ʵı���
* �޸ĵı���
* ���������TYPE_LINK_STATE *pLinkState,CHAR *pStrOutResult
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/04/7    V1.0    ������     ����
**************************************************************************/
void SendErrStringToClient(TYPE_LINK_STATE *pLinkState,CHAR *pStrOutResult)
{
    BOOLEAN isNeedTxtSave = FALSE;
    BOOLEAN bOR = FALSE;
    if ((NULL == pStrOutResult) ||
	 (NULL == pLinkState))
    {
        XOS_SysLog(OAM_CFG_LOG_NOTICE,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__); 
        return;
    }    
    SendSaveAckToClient(pLinkState, pStrOutResult, isNeedTxtSave, bOR);
}

/**************************************************************************
* �������ƣ�VOID SendZDBSaveAckToClient
* ��������������ZDB����Ӧ��
* ���ʵı���
* �޸ĵı���
* ���������TYPE_LINK_STATE *pLinkState
                              WORD32 dwMsgId
                              CHAR *pStrOutResult
                              BOOLEAN isNeedTxtSave
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/04/12    V1.0    ������      ����
**************************************************************************/
void SendZDBSaveAckToClient(TYPE_LINK_STATE *pLinkState,WORD32 dwMsgId,CHAR *pStrOutResult,BOOLEAN isNeedTxtSave)
{
    BOOLEAN bOR = FALSE;
    if ((NULL == pLinkState) ||
        (NULL == pStrOutResult))
    {
        XOS_SysLog(OAM_CFG_LOG_NOTICE,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__); 
        return;
    }
    switch(dwMsgId)
    {
        case  EV_SAVEOK:
            bOR = TRUE;
            break;        
        case  EV_SAVEFAIL:
            bOR = FALSE;
            break;
        default:
            bOR = FALSE;
            break;
    }
    SendSaveAckToClient(pLinkState,pStrOutResult, isNeedTxtSave,bOR);
    return;
}

/**************************************************************************
* �������ƣ�VOID SendTxtSaveAckToClient
* ��������������TXT������Ϣ���ͻ���
* ���ʵı���
* �޸ĵı���
* ���������TYPE_LINK_STATE *pLinkState
                             CHAR *pStrOutResult
                             BOOLEAN bOR
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/04/7    V1.0    ������     ����
**************************************************************************/
void SendTxtSaveAckToClient(TYPE_LINK_STATE *pLinkState,CHAR *pStrOutResult,BOOLEAN bOR)
{    
    BOOLEAN isNeedTxtSave = FALSE;
    if ((NULL == pLinkState) ||
	 (NULL == pStrOutResult))
    {
        XOS_SysLog(OAM_CFG_LOG_NOTICE,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__); 
        return;
    }
    SendSaveAckToClient(pLinkState, pStrOutResult, isNeedTxtSave, bOR);
    return;
}


/**************************************************************************
* �������ƣ�VOID SendSaveAckToClient
* �������������ش�����Ϣ���ͻ���
* ���ʵı���
* �޸ĵı���
* ���������CHAR *sOutputResult
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/04/7    V1.0    ������     ����
**************************************************************************/
static void SendSaveAckToClient(TYPE_LINK_STATE *pLinkState,CHAR *pStrOutResult,BOOLEAN isNeedTxtSave,BOOLEAN bOR)
{
    TYPE_LINK_STATE *pSaveLinkState = &gt_SaveLinkState;
    if ((NULL == pLinkState) ||
	 (NULL == pStrOutResult))
    {
        
        XOS_SysLog(OAM_CFG_LOG_NOTICE,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__); 
        return;    
    }
    if (pSaveLinkState == pLinkState)/*�����ܷ��ͳ�ʱӦ��*/
    {
        SendSaveAckToOmm(pStrOutResult, isNeedTxtSave, bOR);
    }
    else
    {
        SendStringToTelnet(pLinkState, pStrOutResult);
    }
    return;
}

/**************************************************************************
* �������ƣ�VOID SendSaveAckToOmm
* �������������ش�����Ϣ������
* ���ʵı���
* �޸ĵı���
* ���������CHAR *sOutputResult
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/04/7    V1.0    ������     ����
**************************************************************************/
static void SendSaveAckToOmm(CHAR *sOutputResult,BOOLEAN isNeedTxtSave,BOOLEAN bOR)
{
#if 0
    static CHAR g_ssSaveAckBuf[MAX_SIZE_SAVEACK_BUFFER] = {0};
    static BOOLEAN g_sbSaveOR = TRUE;
    WORD16 wLoop = 0;
    WORD16 wOmmLinkNum = sizeof(gt_XmlLinkState)/sizeof(TYPE_XMLLINKSTATE);
    TYPE_XMLLINKSTATE *ptXmlLinkState = NULL;
    
    if (!sOutputResult)
    {
        XOS_SysLog(OAM_CFG_LOG_NOTICE,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__); 
        return;
    }
    /*�������ڴ��̵����ܶ�*/
    for(wLoop = 0;wLoop < wOmmLinkNum;wLoop++)
    {
        ptXmlLinkState = &(gt_XmlLinkState[wLoop]);
        if ((pec_MsgHandle_EXECUTE == ptXmlLinkState->ucCurRunState) &&
	     (CMD_DEF_SET_SAVE == ptXmlLinkState->dwCmdId))
        {            
            break;
        }
    }
    if (wLoop < wOmmLinkNum)
    {
        XML_KillCmdPlanTimer(ptXmlLinkState);
        strncat(g_ssSaveAckBuf,sOutputResult,sizeof(g_ssSaveAckBuf));
	 if (g_sbSaveOR)/*���浱ǰ���̲������*/
	 {
	     g_sbSaveOR = bOR;
	 }
        if (!isNeedTxtSave)/*����ҪTXT����*/
        {
            SendStringToOmm(ptXmlLinkState, g_ssSaveAckBuf,g_sbSaveOR,TRUE);
            memset(g_ssSaveAckBuf,0,MAX_SIZE_SAVEACK_BUFFER);
	     g_sbSaveOR = TRUE;
            XML_LinkSetCurRunState(ptXmlLinkState, pec_MsgHandle_IDLE);
            /*ɱ��XML��·��ʱ��*/
	     Xml_KillExeTimer(ptXmlLinkState);
        }
        else/*���ö�ʱ��*/
        {	     
	     Xml_KillExeTimer(ptXmlLinkState);
	     Xml_SetExeTimer(ptXmlLinkState);
	 }
    }
    else
    {
        XOS_SysLog(OAM_CFG_LOG_NOTICE,
                       "....................FUNC:%s LINE:%d No Xml Link To Save! ....................",__func__,__LINE__); 
    }
#endif
    return;
}

/**************************************************************************
* �������ƣ�VOID SendPacketToTelnet
* ����������������Ϣ��Telnet
* ���ʵı���
* �޸ĵı���
* ���������
  pLinkState --Ҫ���������
  bSendResultOnly -- �Ƿ�ֻ��������������������Ϣ
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
void SendPacketToTelnet(TYPE_LINK_STATE *pLinkState, BOOLEAN bSendResultOnly)
{
    WORD16 wPacketSize = sizeof(TYPE_INTERPRET_MSGHEAD);
    BYTE *pPacket = g_ptOamIntVar->aucSendBuf;
    TYPE_INTERPRET_MSGHEAD *tMsgHead = (TYPE_INTERPRET_MSGHEAD *)pPacket;
    BYTE bSendCmdLine = 0;
    WORD16 wTotalOutputLen = strlen(pLinkState->sOutputResult);
    BOOLEAN  bLastPacketOfApp = TRUE; /*�Ƿ��յ�Ӧ�����һ��*/
    BOOLEAN  bNeedSendCtrlMsg = FALSE; /*�Ƿ���Ҫ���ͷ���ʾ���ݵĿ�����Ϣ*/
	
    /* ��� */
	
    /*telnet��cli֮���Ƿ����һ�����ݣ����������Ƿ��зְ��޸�*/
    bLastPacketOfApp  =  Oam_LinkIfRecvLastPktOfApp(pLinkState) ? TRUE : FALSE;
    tMsgHead->bLastPacket  =  bLastPacketOfApp;

    /* Output the execute result */
    if (pLinkState->bOutputResult)
    {
        Oam_SendOutputToTelnet(pLinkState, &wPacketSize, pPacket);
    }

    if (bSendResultOnly)
    {
        return;
    }
    else if (Oam_LinkNeedLogout(pLinkState))
    {
        /* telnet user quit */
        bNeedSendCtrlMsg = TRUE;
        Oam_ConstructTerminateMsg(&wPacketSize, pPacket);
    }
    else
    {
        /* change of mode */
        if (pLinkState->bChangeMode)
        {
            bNeedSendCtrlMsg = TRUE;
            Oam_ConstructChgPromptMsg(pLinkState, &wPacketSize, pPacket);
        }
    
        /*�ı�����ģʽΪ�첽����ͬ�� yinhao*/
        if (pLinkState->bTelInputModeChanged)
        {
            bNeedSendCtrlMsg = TRUE;
            Oam_ConstructChgInputModeMsg(pLinkState, &wPacketSize, pPacket);
        }
    	
        /* change input mode */
        if (pLinkState->bInputStateChanged)
        {
            bNeedSendCtrlMsg = TRUE;
            Oam_ConstructPasswordMsg(pLinkState, &wPacketSize, pPacket);
        }
        Oam_ConstructShowCmdLineMsg(pLinkState, &wPacketSize, pPacket, &bSendCmdLine);
        if (bSendCmdLine)
            bNeedSendCtrlMsg = TRUE;
      
        /*Ask Telnet״̬*/
        if (  /* !Oam_LinkIfRecvLastPktOfApp(pLinkState) && */
                pLinkState->bOutputResult == FALSE &&
                pLinkState->tMatchResult.wParseState == PARSESTATE_ASK_TELNET)
        {
            bNeedSendCtrlMsg = TRUE;
            Oam_ConstructNeedExtrInfoMsg(&wPacketSize, pPacket);
        }
    }

    /* set message header */
    tMsgHead->bVtyNum     = Oam_LinkGetLinkId(pLinkState);
    tMsgHead->bOutputMode = pLinkState->bOutputMode;
    tMsgHead->wSeqNo      = Oam_LinkIfRecvAsyncMsg(pLinkState) ? 0 : pLinkState->wTelSeqNo;
    if (pLinkState->bSSHAuthMsg)
        tMsgHead->bSSHAuthRslt = pLinkState->bSSHAuthRslt;
    tMsgHead->bSSHConnRslt = pLinkState->bSSHConnRslt;
    
    tMsgHead->wDataLen    = wPacketSize -sizeof(TYPE_INTERPRET_MSGHEAD);

    /* ���� */
    if (bNeedSendCtrlMsg)
    {
        XOS_SendAsynMsg(OAM_MSG_INTERPRET_TO_TELNET, (BYTE*)tMsgHead, (WORD16)wPacketSize,
                        XOS_MSG_VER0, XOS_MSG_MEDIUM,&g_ptOamIntVar->jidTelnet);
    }

    /* ��λ */
    Oam_LinkMarkLogout(pLinkState, FALSE);
    pLinkState->bRecvAsynMsg = FALSE;
    if (pLinkState->wSendPos >= wTotalOutputLen)
    {
        pLinkState->bOutputResult    = FALSE;
        *(pLinkState->sOutputResult) = MARK_STRINGEND;
        pLinkState->wSendPos         = 0;

        if (bSendCmdLine)
        {
            pLinkState->bOutputCmdLine = TRUE; /*Ĭ����������һ��������ʾcmdline����Ҫʱ���ͨ���������Ϊfalse������ʾcmdline*/
            *(pLinkState->sCmdLine)    = MARK_STRINGEND;
        }
    }

    return;
}

/**************************************************************************
* �������ƣ�VOID Oam_Cfg_SendKeepAliveMsgToTelnet(TYPE_LINK_STATE *pLinkState)
* �����������յ�Ӧ�ñ�����Ϣ��,���ͱ�����Ϣ��telnetserver
* ���ʵı���
* �޸ĵı���
* ���������TYPE_LINK_STATE *pLinkState
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2010/05/27    V1.0      ���       ����
**************************************************************************/
void Oam_Cfg_SendKeepAliveMsgToTelnet(TYPE_LINK_STATE *pLinkState)
{
    WORD16 wPacketSize = sizeof(TYPE_INTERPRET_MSGHEAD);
    BYTE *pPacket = g_ptOamIntVar->aucSendBuf;
    TYPE_INTERPRET_MSGHEAD *tMsgHead = (TYPE_INTERPRET_MSGHEAD *)pPacket;
    WORD16 wMsgDataLen = 0;
 
    *(pPacket + wPacketSize) = MSGID_KEEPALIVE;
    wPacketSize += sizeof(BYTE);

    memcpy(pPacket + wPacketSize, &wMsgDataLen, sizeof(WORD16));
    wPacketSize += sizeof(WORD16);

    wPacketSize += wMsgDataLen;
	
     /* set message header */
    tMsgHead->bVtyNum     = Oam_LinkGetLinkId(pLinkState);
    tMsgHead->bOutputMode = pLinkState->bOutputMode;
    tMsgHead->wSeqNo      = Oam_LinkIfRecvAsyncMsg(pLinkState) ? 0 : pLinkState->wTelSeqNo;
    tMsgHead->wDataLen    = wPacketSize -sizeof(TYPE_INTERPRET_MSGHEAD);

	
    /* ���� */
    XOS_SendAsynMsg(OAM_MSG_INTERPRET_TO_TELNET, (BYTE*)tMsgHead, (WORD16)wPacketSize,
                        XOS_MSG_VER0, XOS_MSG_MEDIUM,&g_ptOamIntVar->jidTelnet);

}

/**************************************************************************
* �������ƣ�VOID DisposeInnerCmdForXmlAgent
* ����������ΪXML����������Inner�ڲ�����
* ���ʵı���
* �޸ĵı���
* ���������MSG_COMM_OAM *ptOamMsg
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/01/07    V1.0    ������      ����
**************************************************************************/
void DisposeInnerCmdForXmlAgent(MSG_COMM_OAM *ptOamMsg)
{
    MSG_COMM_OAM *ptOamMsgTmp = NULL;
    MSG_COMM_OAM *ptRtnMsg = (MSG_COMM_OAM *)g_ptOamIntVar->aucRecvBuf;/*ģʽջ��ѯ���*/	
    DWORD dwExeID = 0;
    DWORD dwMsgId = 0;		
    /*�������*/
    if (NULL == ptOamMsg)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return;
    }    
    /*��ʼ������*/
    ptOamMsgTmp = ptOamMsg;
    dwExeID = ptOamMsgTmp->CmdID;
    switch(dwExeID)
    {
	 case CMD_LDB_GET_MODESTATCK:/*��ѯģʽջ*/
	 {
            dwMsgId=EV_OAM_CFG_CFG;
	     Oam_InnerApp((LPVOID)ptOamMsg,dwMsgId,ptRtnMsg); 
            Oam_RecvFromCliApp(dwMsgId,(LPVOID)ptRtnMsg);										 
	     break;
	 }
	 default:
	     InnerCmdProcessForXmlAgent(ptOamMsgTmp);	     
	     break;
    }
    
}
/**************************************************************************
* �������ƣ�VOID DisposeInterpretPosCmd
* ��������������Inner�ڲ�����
* ���ʵı���
* �޸ĵı���
* ���������TYPE_LINK_STATE *pLinkState
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
void DisposeInterpretPosCmd(TYPE_LINK_STATE *pLinkState)
{
    TYPE_MATCH_RESULT *pMatchResult = &(pLinkState->tMatchResult);
    T_OAM_Cfg_Dat * cfgDat = pMatchResult->cfgDat;
    DWORD dwExeID = cfgDat->tCmdAttr[pMatchResult->wCmdAttrPos].dExeID;
    switch(dwExeID)
    {
    case CMD_TELNET_LOGOUT:           /*�˳�*/
        Oam_InnerCmdLogout(pLinkState);
        break;
#if 0        
    case CMD_INTERPT_LOGIN:           /*��¼����*/
        InnerCmdLogin(pLinkState);
        break;
#endif        
    case CMD_INTERPT_ENABLE:          /*������Ȩģʽ����*/
        InnerCmdEnable(pLinkState);
        break;
    case CMD_INTERPT_DISABLE:         /*�˳���Ȩģʽ����*/
        InnerCmdDisable(pLinkState);
        break;
#if 0
    case CMD_INTERPT_CONFIG_TERMINAL: /*��������ģʽ*/
        InnerCmdConfigure(pLinkState);
        break;
#endif
    case CMD_INTERPT_EXIT:            /*�˳�ģʽ*/
        InnerCmdExit(pLinkState);
        break;
    case CMD_LDB_SET_BANNER:          /*����Banner����*/
        InnerCmdSetBanner(pLinkState);
        break;
    case CMD_LDB_SET_HOSTNAME:        /*����HostName����*/
        InnerCmdSetPrompt(pLinkState);
        break;				
    default:
	 if(IsSaveCmd(dwExeID))
        {
            DisposeInnerCmd(pLinkState);
	 }
	 else
      /*  if (CMD_MODE_USER == Oam_GetCurModeId(pLinkState) ||
             CMD_MODE_PRIV == Oam_GetCurModeId(pLinkState))*/
        {
            /* Ĭ�ϵ���Ϊ��ҵ����ӵĽ����Լ�ģʽ������ */
            OAM_ChangeMode(pLinkState,NEXTMODE_NORMAL);
            Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_TELNET);
        SendPacketToTelnet(pLinkState, FALSE);
        }
        break;
    }
	
    return;
}
/**************************************************************************
* �������ƣ�VOID DisposeInnerCmd
* ��������������Inner�ڲ�����
* ���ʵı���
* �޸ĵı���
* ���������TYPE_LINK_STATE *pLinkState
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
void DisposeInnerCmd(TYPE_LINK_STATE *pLinkState)
{
    MSG_COMM_OAM *ptMsg = OamCfgConstructMsgData(pLinkState, NULL);
    MSG_COMM_OAM *ptRtnMsg = (MSG_COMM_OAM *)g_ptOamIntVar->aucRecvBuf;
    CHAR *ptrOutputResult = NULL;	
    WORD32 dwMsgId=0;
    int iLinkStateType = 0;	
    #if 0	
    /*who�������������ⲻ���䴦��*/	

    if(ptMsg->CmdID == CMD_TELNET_WHO)
    {
        Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_EXECUTE);
        if (Oam_CfgCurLinkIsOffline2Online(pLinkState))
        {
            ptMsg->ucOamTag = OAMCFG_TAG_OFFLINE_TO_ONLINE;
        }
        else 
        {
            ptMsg->ucOamTag = OAMCFG_TAG_NORMAL;        		
        }
        pLinkState->ucPlanValuePrev = 0;
        SendPacketToExecute(pLinkState, ptMsg, pLinkState->tMatchResult.cfgDat->tJid);	
    }
    else
    #endif
    if(IsSaveCmd(ptMsg->CmdID))
    {
        if (CheckMpExecuteMutex(pLinkState, ptMsg))
        {
            if (!Xml_CfgCurLinkIsExcutingSave(pLinkState))/*��ǰ���������ڴ���*/
            {
                /*����Ƿ�����·���ڴ���*/
                if (CheckLinkIsExecutingSaveCmd() )
                {
                    SendStringToTelnet(pLinkState, Hdr_szSaveMutexTip);
                    return;
                }
                /*����Ǵ���������������·�Ƿ���ִ������*/
       	        else if (CheckLinkIsExecutingCmd())
       	        {
       	            ptrOutputResult = ReadMapValueOfOamDat(MAP_TYPE_XMLADAPTER,MAP_KEY_CONFLICT_EXECUTECMD);
                    if (NULL == ptrOutputResult)
                    {
                        ptrOutputResult = STRING_NOTFINDMAPKEY;
                    }
       	            SendStringToTelnet(pLinkState, ptrOutputResult);
       	            return;
       	        }
            }

            /*����Execute�˶�ʱ��*/
            Oam_KillExeTimer(pLinkState);
            Oam_SetExeTimer(pLinkState);
       	 
            Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_EXECUTE);
   
            if (Oam_CfgCurLinkIsOffline2Online(pLinkState))
            {
                ptMsg->ucOamTag = OAMCFG_TAG_OFFLINE_TO_ONLINE;
            }
            else
            {
                ptMsg->ucOamTag = OAMCFG_TAG_NORMAL;
            }
            pLinkState->ucPlanValuePrev = 0;
        }
        else
        {
            /*��ʾ�û��������û�����ִ������Ժ�����*/
            SendStringToTelnet(pLinkState, Hdr_szCommandMutexTip);
            return;
        }
        iLinkStateType = OamCfgGetLinkStateType(ptMsg);
        if (LINKSTATE_TYPE_ERR == iLinkStateType)
        {
            return;
        }
        else if (LINKSTATE_TYPE_CLI == iLinkStateType)
        {
            dwMsgId = EV_OAM_CFG_CFG;
        }
        else if (LINKSTATE_TYPE_XML == iLinkStateType)
        {
            dwMsgId = OAM_MSG_XMLAGNET_TO_INTERPRET;
        }				
        Oam_InnerApp((LPVOID)ptMsg,dwMsgId,ptRtnMsg);
    }
    else
    {
        dwMsgId =EV_OAM_CFG_CFG;
        Oam_InnerApp((LPVOID)ptMsg,dwMsgId,ptRtnMsg);
        Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_EXECUTE);
        Oam_RecvFromCliApp(dwMsgId,(LPVOID)ptRtnMsg);		   
    }
    return;
}

/**************************************************************************
* �������ƣ�VOID Oam_CfgCloseAllLink(CHAR *pcReason)
* �����������Ͽ����е�telnet����
* ���ʵı���
* �޸ĵı���
* ���������CHAR *pcReason - �Ͽ�ԭ�򣬿���Ϊ��
* ���������
* �� �� ֵ���μ�ͷ�ļ�����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2008/04/08   V1.0    ���      ����
**************************************************************************/
void Oam_CfgCloseAllLink(CHAR *pcReason)
{
    int i = 0;
    CHAR aucOutTmp[200] = {0};

    if(pcReason)
    {
        /*CRDCM00313101 ��XOS_snprintf�滻strcpy��strcat����*/
        XOS_snprintf(aucOutTmp,
                     sizeof(aucOutTmp) -1,
                     "\n\n\r%s",
                     pcReason);
    }
    /*OSS_Printf(PRINT_OAM_PLATCFGAGT, PRN_NORMAL, PRN_LEVEL_NORMAL, "Close all link!");*/

    for (i = 0; i < MAX_CLI_VTY_NUMBER; i++)
    {
        if (pec_MsgHandle_IDLE != Oam_LinkGetCurRunState(&gtLinkState[i]))
        {
            Oam_LinkSetCurRunState(&gtLinkState[i], pec_MsgHandle_TELNET);
            gtLinkState[i].bOutputCmdLine = FALSE;
            if (pcReason)
            {
                SendStringToTelnet(&gtLinkState[i], aucOutTmp);
            }
            Oam_InnerCmdLogout(&gtLinkState[i]);
        }
    }
    /*��XML�����������ܶ���*/
    for (i = 0;i < MAX_SIZE_LINKSTATE;i++)
    {
        if(pec_MsgHandle_IDLE != XML_LinkGetCurRunState(&gt_XmlLinkState[i]))
        {
            XML_LinkSetCurRunState(&gt_XmlLinkState[i], pec_MsgHandle_IDLE);
            Xml_KillExeTimer(&gt_XmlLinkState[i]);
            if (pcReason)
            {
//                SendStringToOmm(&gt_XmlLinkState[i], pcReason, FALSE,TRUE);
            }           
        }
	  if (NULL != gt_XmlLinkState[i].ptLastSendMsg)
        {
            OAM_RETUB(gt_XmlLinkState[i].ptLastSendMsg);
            gt_XmlLinkState[i].ptLastSendMsg = NULL;
        }
        memset(&gt_XmlLinkState[i],0,sizeof(TYPE_XMLLINKSTATE));
    }
}

void Oam_SendOutputToTelnet(TYPE_LINK_STATE *pLinkState, WORD16 *pwPacketSize, BYTE *pPacket)
{
    BOOLEAN bOutputNotFinish = TRUE;
    WORD16 wPacketSize = *pwPacketSize;
    WORD16 wTotalOutputLen = strlen(pLinkState->sOutputResult);
    WORD16 wStrLen = 0;
    TYPE_INTERPRET_MSGHEAD *tMsgHead = (TYPE_INTERPRET_MSGHEAD *)pPacket;
    BOOLEAN  bLastPacketOfApp = TRUE; /*�Ƿ��յ�Ӧ�����һ��*/
    WORD16 wHeadSize = sizeof(WORD16) + sizeof(BYTE);

    /*telnet��cli֮���Ƿ����һ�����ݣ����������Ƿ��зְ��޸�*/
    bLastPacketOfApp  =  Oam_LinkIfRecvLastPktOfApp(pLinkState) ? TRUE : FALSE;
    tMsgHead->bLastPacket  =  bLastPacketOfApp;

    XOS_SysLog(OAM_CFG_LOG_DEBUG, "Oam_SendOutputToTelnet:: pLinkState->sOutputResult length=%u \n", wTotalOutputLen);
    /*ѭ������һ�ν��յ���Ӧ������*/
    while (bOutputNotFinish)
    {
        if (*(pLinkState->sOutputResult) != MARK_STRINGEND)
        {
            /*���㻹�ж����ַ�û�з���*/
            wStrLen = wTotalOutputLen - pLinkState->wSendPos;

            /*?? �����Ҫ��ʾ�����ݴ����������ȣ�
            ����Ҫ�ְ�������������Ϣ�Ƿ���Ҫ��������? */
            if ((wStrLen + wPacketSize) > MAX_OAMMSG_LEN)
            {
                BYTE ucTmp = 0;
                wStrLen = 0;
                while (wStrLen < (MAX_OAMMSG_LEN - wPacketSize))
                {
                    ucTmp = *(pLinkState->sOutputResult + pLinkState->wSendPos + wStrLen);
                    wStrLen++; 
                    /*��������ģ������������ֽ�*/
                    if (char_is_chinese(ucTmp))
                    {
                        if (wStrLen == (MAX_OAMMSG_LEN - wPacketSize))
                        {/*�����ȡ�ַ����Ѿ��ﵽ��󳤶ȣ��˻��Ѷ��İ�������ַ�*/
                            wStrLen--;
                            break;
                        }
                        else
                        {/*��ȡ��һ�������ַ�*/
                            wStrLen++;    	
                        }
                    }
                }

                tMsgHead->bLastPacket = FALSE;
                bOutputNotFinish = TRUE;
            }
            else
            {
                /*��Ӧ�ð��������lastpacket����Ӧ�ð�����*/
                tMsgHead->bLastPacket = bLastPacketOfApp;
                bOutputNotFinish = FALSE;
            }
            
            memcpy(pPacket + wPacketSize + wHeadSize,
                    pLinkState->sOutputResult + pLinkState->wSendPos, wStrLen);
            pLinkState->wSendPos += wStrLen;
        }
        else
        {
            wStrLen = 0;
            bOutputNotFinish = FALSE;
        }

        *(pPacket + wPacketSize) = MSGID_OUTPUTRESULT;
        wPacketSize += sizeof(BYTE);

        memcpy(pPacket + wPacketSize, &wStrLen, sizeof(WORD16));
        wPacketSize += sizeof(WORD16);

        wPacketSize += wStrLen;

        if (pLinkState->wSendPos <= wTotalOutputLen)
        {
            tMsgHead->bVtyNum     = Oam_LinkGetLinkId(pLinkState);
            tMsgHead->bOutputMode = pLinkState->bOutputMode;
            tMsgHead->wSeqNo      = Oam_LinkIfRecvAsyncMsg(pLinkState) ? 0 : pLinkState->wTelSeqNo;
            tMsgHead->wDataLen    = wPacketSize -sizeof(TYPE_INTERPRET_MSGHEAD);
            tMsgHead->bSSHConnRslt = pLinkState->bSSHConnRslt;
            if (pLinkState->bSSHAuthMsg)
                tMsgHead->bSSHAuthRslt = pLinkState->bSSHAuthRslt;

            /* ���� */
            XOS_SendAsynMsg(OAM_MSG_INTERPRET_TO_TELNET, (BYTE*)tMsgHead, ( WORD16)wPacketSize,
                            XOS_MSG_VER0, XOS_MSG_MEDIUM,&g_ptOamIntVar->jidTelnet);
            wPacketSize = sizeof(TYPE_INTERPRET_MSGHEAD);

        }
    }

    *pwPacketSize = wPacketSize;
}

void Oam_ConstructTerminateMsg(WORD16 *pwPacketSize, BYTE *pPacket)
{
    WORD16 wStrLen = 0;
    WORD16 wPacketSize = *pwPacketSize;

    *(pPacket + wPacketSize) = MSGID_TERMINATE;
    wPacketSize += sizeof(BYTE);

    memcpy(pPacket + wPacketSize, &wStrLen, sizeof(WORD16));
    wPacketSize += sizeof(WORD16);

    wPacketSize += wStrLen;
    *pwPacketSize = wPacketSize;
}

void Oam_ConstructChgPromptMsg(TYPE_LINK_STATE *pLinkState, WORD16 *pwPacketSize, BYTE *pPacket)
{
    WORD16 wStrLen = 0;
    WORD16 wPacketSize = *pwPacketSize;

    *(pPacket + wPacketSize) = MSGID_CHANGEPROMPT;
    wPacketSize += sizeof(BYTE);

    wStrLen = strlen(pLinkState->sPrompt) + 1;
    memcpy(pPacket + wPacketSize, &wStrLen, sizeof(WORD16));
    wPacketSize += sizeof(WORD16);
		
    memcpy(pPacket + wPacketSize, pLinkState->sPrompt, wStrLen);
    wPacketSize += wStrLen;
    *pwPacketSize = wPacketSize;
    
    /*��ԭ�仯���*/	
    pLinkState->bChangeMode   = FALSE;
    
}

void Oam_ConstructChgInputModeMsg(TYPE_LINK_STATE *pLinkState, WORD16 *pwPacketSize, BYTE *pPacket)
{
    WORD16 wStrLen = 0;
    WORD16 wPacketSize = *pwPacketSize;

    *(pPacket + wPacketSize) = MSGID_CHANGE_INPUTMODE;
    wPacketSize += sizeof(BYTE);

    wStrLen = 1;
    memcpy(pPacket + wPacketSize, &wStrLen, sizeof(WORD16));
    wPacketSize += sizeof(WORD16);
		
    *(pPacket + wPacketSize) = Oam_LinkIfTelSvrInputModeIsSyn(pLinkState) ? OAM_INPUT_MODE_CHANGE_SYN : OAM_INPUT_MODE_CHANGE_ASYN;;
    wPacketSize += sizeof(BYTE);
    *pwPacketSize = wPacketSize;

    /*��ԭ�仯���*/
    pLinkState->bTelInputModeChanged = FALSE;
}

void Oam_ConstructPasswordMsg(TYPE_LINK_STATE *pLinkState, WORD16 *pwPacketSize, BYTE *pPacket)
{
    WORD16 wStrLen = 0;
    WORD16 wPacketSize = *pwPacketSize;

    if (Oam_LinkGetInputState(pLinkState) == INMODE_PASSWORD)
        *(pPacket + wPacketSize) = MSGID_ISPASSWORD;
    else
        *(pPacket + wPacketSize) = MSGID_NOTPASSWORD;
    wPacketSize += sizeof(BYTE);

    memcpy(pPacket + wPacketSize, &wStrLen, sizeof(WORD16));
    wPacketSize += sizeof(WORD16);

    wPacketSize += wStrLen;	   
    *pwPacketSize = wPacketSize;

    /*��ԭ�仯���*/
    pLinkState->bInputStateChanged = FALSE;
}

void Oam_ConstructShowCmdLineMsg(TYPE_LINK_STATE *pLinkState, WORD16 *pwPacketSize, BYTE *pPacket, BYTE *pbSendCmdLine)
{
    WORD16 wStrLen = 0;
    WORD16 wPacketSize = *pwPacketSize;
    BYTE bSendCmdLine = *pbSendCmdLine;
    WORD16 wHeadSize = sizeof(WORD16) + sizeof(BYTE);

    /* ����Ǻ�app�����һ�������Ҳ����첽������������*/
    if ( Oam_LinkIfRecvLastPktOfApp(pLinkState) && 
         !Oam_LinkIfRecvAsyncMsg(pLinkState))
    {
        bSendCmdLine = 1;
        wStrLen = 0;
        if (pLinkState->bOutputCmdLine && *(pLinkState->sCmdLine) != MARK_STRINGEND)
        {
            wStrLen = strlen(pLinkState->sCmdLine);
            if ((wStrLen + wPacketSize) > MAX_OAMMSG_LEN)
            {
                bSendCmdLine = 0;
            }
            else
            {
                bSendCmdLine = 2;
            }
        }
        else if (!pLinkState->bOutputCmdLine)
        {
            bSendCmdLine = 0;
            /*����Ĭ����������һ������Ҫ��ʾcmdline��
            �������ָ���˲���ʾcmdline��Ҳֻ��һ�Σ��ָ�һֱ����ʾ*/
            pLinkState->bOutputCmdLine = TRUE;
        }
    }
	
    if (bSendCmdLine)
    {
        if (bSendCmdLine == 2)
            memcpy(pPacket + wPacketSize + wHeadSize, pLinkState->sCmdLine, (size_t)wStrLen);

        *(pPacket + wPacketSize) = MSGID_SHOWCMDLINE;
        wPacketSize += sizeof(BYTE);

        memcpy(pPacket + wPacketSize, &wStrLen, sizeof(WORD16));
        wPacketSize += sizeof(WORD16);

        wPacketSize += wStrLen;
    }

    *pwPacketSize = wPacketSize;
    *pbSendCmdLine = bSendCmdLine;

}

void Oam_ConstructNeedExtrInfoMsg(WORD16 *pwPacketSize, BYTE *pPacket)
{
    WORD16 wStrLen = 0;
    WORD16 wPacketSize = *pwPacketSize;

    *(pPacket + wPacketSize) = MSGID_NEEDEXTRINFO;
    wPacketSize += sizeof(BYTE);

    memcpy(pPacket + wPacketSize, &wStrLen, sizeof(WORD16));
    wPacketSize += sizeof(WORD16);

    wPacketSize += wStrLen;
    *pwPacketSize = wPacketSize;
}

/*lint +e721*/
/*lint +e539*/
/*lint +e548*/
/*lint +e669*/




