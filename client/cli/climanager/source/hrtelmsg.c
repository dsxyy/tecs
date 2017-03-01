/**************************************************************************
* ��Ȩ���� (C)2002, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ�HrTelMsg.c
* �ļ���ʶ��
* ����ժҪ����������Telnet����Ϣ
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
#include "xmlagent.h"
#include "oam_execute.h"
#include "xmlrpc.h"

/*lint -e661*/
/*lint -e420*/
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
/*static BOOLEAN GlobalPointersIsOk(VOID);*/
/**************************************************************************
*                          �ֲ�����ʵ��                                   *
**************************************************************************/
/**************************************************************************
* �������ƣ�VOID Process_Input_String
* �������������������ַ���
* ���ʵı���
* �޸ĵı���
* ���������TYPE_LINK_STATE *pLinkState,MSG_COMM_OAM * ptMsg
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
static void Process_Input_String(TYPE_LINK_STATE *pLinkState, TYPE_TELNET_MSGHEAD *ptMsg)
{
    CHAR * pInString = (CHAR *)ptMsg->pData;
    /*WORD16 nCmpResult  = 1;*/
    /*BOOL bChkResult = FALSE;*/

    T_OAM_Cfg_Dat * cfgDat = pLinkState->tMatchResult.cfgDat;

    TYPE_TEXT_INPUT * pTextInput = NULL;
    static CHAR szTnTempBuf[MAX_STRING_LEN] = {0};
    BOOLEAN bHasEndMark = FALSE;
    CHAR * pTempChar = NULL;
    WORD16 nPos = 0;

    switch (Oam_LinkGetInputState(pLinkState))
    {
#if 0
    case INMODE_USERNAME:
        /* 1.��֤�û� */
        if (pInString)
        {
            USER_NODE *ppNode = NULL;
            USER_NODE *pNode = NULL;

            if (strcmp(pInString, pLinkState->sUserName) == 0)
            {
                SendUserNameErrToTelnet(pLinkState,Hdr_szCanNotLogin); /* ��ʾ��ʾ��Ϣ,���� */
                break;
            }
            else if (FindUserFromLink(pInString,&ppNode,&pNode))
            {
                /* �����û���/Ȩ�� */
                strncpy(pLinkState->sSavedUser, pLinkState->sUserName);
                pLinkState->ucSavedRight = pLinkState->ucUserRight;

                /* �����û���/����/Ȩ�� */
                /*CRDCM00313101 ��XOS_snprintf�滻strcpy��strcat����*/
                /*
                strncpy(pLinkState->sUserName, pInString);
                */
                XOS_snprintf(pLinkState->sUserName,
                             sizeof(pLinkState->sUserName) -1,
                             "%s",
                             pInString);

                /*CRDCM00313101 ��XOS_snprintf�滻strcpy��strcat����*/
                /*
                strncpy(pLinkState->sPassword, pNode->tUserInfo.sPassword);
                */
                XOS_snprintf(pLinkState->sPassword,
                             sizeof(pLinkState->sPassword) -1,
                             "%s",
                             pNode->tUserInfo.sPassword);

                pLinkState->ucUserRight = pNode->tUserInfo.ucRight;
                bChkResult = TRUE;
            }
        }

        /* 2.��֤ʧ�ܴ��� */
        if (!bChkResult)
        {
            pLinkState->ucInputErrTimes++;
            /* �ж��Ƿ񳬹��������Դ��� */
            if (pLinkState->ucInputErrTimes >= MAX_VERIFYFAILURE)
            {
                SendUserNameErrToTelnet(pLinkState,Hdr_szOsBadUserName); /* ��ʾ������Ϣ,���� */
            }
            else
            {
                SendStringToTelnet(pLinkState,Hdr_szOsBadUserName); /* ��ʾ������Ϣ,��ʾ�û����������û��� */
            }
            break;
        }

        /* 3.����������ʾ��, ��ʾ�û��������� */
        SendPasswordPromptToTelnet(pLinkState,NULL);

        /* 4.�ı䵱ǰ״̬��TELNET */
        Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_TELNET);

        break;

    case INMODE_PASSWORD:
        /* 1.�Ƚ����� */
        switch(cfgDat->tCmdAttr[pLinkState->wCmdAttrPos].dExeID)
        {
        case CMD_INTERPT_ENABLE:
            nCmpResult = strcmp(gtCommonInfo.sEnablePassword, pInString);
            break;
        default:
            break;
        }

        if (nCmpResult != 0)
        {
            pLinkState->ucInputErrTimes++;
            /* �ж��Ƿ񳬹��������Դ��� */
            if (pLinkState->ucInputErrTimes >= MAX_VERIFYFAILURE)
            {
                SendPasswordErrToTelnet(pLinkState, Hdr_szOsBadPassword); /* ��ʾ������Ϣ,���� */
            }
            else
            {
                SendStringToTelnet(pLinkState, Hdr_szOsBadPassword); /* ��ʾ������Ϣ,��ʾ�û������������� */
            }
            break;
        }

        /* 2.�л�����Ӧģʽ */
        pLinkState->ucInputErrTimes  = 0;

        /* �ı�����ģʽ */
        Oam_LinkSetInputState(pLinkState, INMODE_NORMAL);
        switch(cfgDat->tCmdAttr[pLinkState->wCmdAttrPos].dExeID)
        {
#if 0            
        case CMD_INTERPT_LOGIN:
            /* ����Telnet�˾��Գ�ʱ��ʱ�� */
            Oam_KillAbsTimer(pLinkState);
            Oam_SetAbsTimer(pLinkState);

            /* �ı���ʾ�� */
            pLinkState->bChangeMode = TRUE;
            ChangePrompt(pLinkState,Oam_GetCurModeId(pLinkState));
            break;
#endif            
        case CMD_INTERPT_ENABLE:
            /* ���ݵ�ǰ�û���Ȩ�޲�����Ϊ�����û���Ȩ�� */
            pLinkState->ucSavedRight = pLinkState->ucUserRight;
            pLinkState->ucUserRight  = DEFAULT_ENABLE_RITHT;
            pLinkState->bIsSupperUsrMode = TRUE;

            /* �ı�ģʽ����ʾ�� */
            OAM_ChangeMode(pLinkState,NEXTMODE_NORMAL);
            break;
        default:
            break;
        }
        SendPacketToTelnet(pLinkState, FALSE);
        break;
#endif
    case INMODE_TEXT:
        bHasEndMark = FALSE;
        pTempChar   = pInString;
        pTextInput  = &(pLinkState->tInputControl);
        nPos        = 0;

        while (pTempChar != NULL && *pTempChar != MARK_STRINGEND)
        {
            if (*pTempChar != pTextInput->cEndMark)
            {
                pTempChar++;
                nPos++;
            }
            else
            {
                bHasEndMark = TRUE;
                break;
            }
        }


        if (nPos >= MAX_STRING_LEN - 2 - strlen(pTextInput->sTextBuf)) /* \n\0 */
        {
            nPos = MAX_STRING_LEN - 2 - strlen(pTextInput->sTextBuf);
            bHasEndMark = TRUE;
        }

        memset(szTnTempBuf, 0, MAX_STRING_LEN);
        memcpy(szTnTempBuf, pInString, nPos);
        szTnTempBuf[nPos] = MARK_STRINGEND;
        /*CRDCM00313101 ��XOS_snprintf�滻strcpy��strcat����*/
        XOS_snprintf(szTnTempBuf,
                     sizeof(szTnTempBuf),
                     "%s\n",
                     szTnTempBuf);

        /*CRDCM00313101 ��XOS_snprintf�滻strcpy��strcat����*/
        XOS_snprintf(pTextInput->sTextBuf,
                     sizeof(pTextInput->sTextBuf),
                     "%s%s",
                     pTextInput->sTextBuf,
                     szTnTempBuf);

        if (bHasEndMark)
        {
            switch(cfgDat->tCmdAttr[pLinkState->wCmdAttrPos].dExeID)
            {
            case CMD_LDB_SET_BANNER:
                /* ����sBanner */
                strncpy(gtCommonInfo.sBanner, "\n\r", sizeof(gtCommonInfo.sBanner));
                strncat(gtCommonInfo.sBanner, pTextInput->sTextBuf, sizeof(gtCommonInfo.sBanner));
                gtCommonInfo.sBanner[MAX_BANNER_LEN - 1] = MARK_STRINGEND;

                SendNormalPromptToTelnet(pLinkState,NULL);
                break;
            default:
                SendPacketToTelnet(pLinkState, FALSE);
                break;
            }
        }
        else
        {
            SendPacketToTelnet(pLinkState, FALSE);
        }
        break;

    case INMODE_NORMAL:
        /* ƥ�������� */
        Oam_InptFreeBuf(pLinkState->ucLinkID);
		
        /* �����ǰ��������������ִ�У�������ǰ���� */
        if (Oam_LinkGetCurRunState(pLinkState) == pec_MsgHandle_EXECUTE)
            return;

        ParseCmdLine(pLinkState, pInString,ptMsg->bDeterminer,FALSE);
        /*�����ɹ����������ü�¼*/
        if (PARSESTATE_FINISH_PARSE == pLinkState->tMatchResult.wParseState)
        {
            /*??��־�����ݲ�ʵ��*/
        }
        break;

    default:
        SendPacketToTelnet(pLinkState, FALSE);
        break;
    }

    return;
}

static void CheckUserNameOfTelnet(TYPE_LINK_STATE *pLinkState, CHAR *pString)
{
#if 0
    TYPE_USER_INFO tUserInfo;
    BOOL bUserExist = FALSE;
    CHAR szSysLog[128] = {0};
    CHAR *pSysLog = szSysLog;

    if (pString == NULL)
    {
        return;
    }

    /* ��¼��־��¼��Ϣ */
    strncpy(pLinkState->tUserLog.sUserName, pString, sizeof(pLinkState->tUserLog.sUserName));
    memset(pLinkState->tUserLog.tnIpAddr, 0, LEN_IP_ADDR_STR);
    memcpy(pLinkState->tUserLog.tnIpAddr, pLinkState->tnIpAddr, LEN_IP_ADDR_STR);
    XOS_GetSysClock(&(pLinkState->tUserLog.tSysClockLogin));

    /* ��������־��¼��Ϣ */
    strncpy(pLinkState->tCmdLog.szUserName, pString, sizeof(pLinkState->tCmdLog.szUserName));
    memset(pLinkState->tCmdLog.szIpAddr, 0, LEN_IP_ADDR_STR);
    memcpy(pLinkState->tCmdLog.szIpAddr, pLinkState->tnIpAddr, LEN_IP_ADDR_STR);

    memset(&tUserInfo, 0, sizeof(tUserInfo));
    if (0 != strlen(pString))
    {
        if (FindUserInfoFromLink(pString, &tUserInfo))
        {
            /* �����û���/����/Ȩ�� */
            strncpy(pLinkState->sUserName, pString, sizeof(pLinkState->sUserName));
            strncpy(pLinkState->sPassword, tUserInfo.szPassWord, sizeof(pLinkState->sPassword));
            pLinkState->ucUserRight = tUserInfo.ucRole;
            bUserExist = TRUE;
        }
    }

    /* ��֤ʧ�ܴ��� */
    if (!bUserExist)
    {
        /* ��¼ʧ�ܣ���¼��־ */
        pLinkState->tUserLog.ucLoginRslt = USER_LOGIN_FAIL;
        /* д��־�ļ� */
        if(!InsertOneUseLog(pLinkState))
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, "[CheckUserNameOfTelnet] InsertOneUseLog failed! line = %d\n", __LINE__);
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

        pLinkState->ucInputErrTimes++;
        /* �ж��Ƿ񳬹��������Դ��� */
        if (pLinkState->ucInputErrTimes >= MAX_VERIFYFAILURE)
        {
            SendLogoutToTelnet(pLinkState,Hdr_szOsBadUserName); /* ��ʾ������Ϣ,�˳� */
        }
        else
        {
            SendStringToTelnet(pLinkState,Hdr_szOsBadUserName); /* ��ʾ������Ϣ,��ʾ�û����������û��� */
        }
        return;
    }
#endif
    
    /* ��¼��־��¼��Ϣ */
    strncpy(pLinkState->tUserLog.sUserName, pString, sizeof(pLinkState->tUserLog.sUserName));
    memset(pLinkState->tUserLog.tnIpAddr, 0, LEN_IP_ADDR_STR);
    memcpy(pLinkState->tUserLog.tnIpAddr, pLinkState->tnIpAddr, LEN_IP_ADDR_STR);
    XOS_GetSysClock(&(pLinkState->tUserLog.tSysClockLogin));

    /* ��������־��¼��Ϣ */
    strncpy(pLinkState->tCmdLog.szUserName, pString, sizeof(pLinkState->tCmdLog.szUserName));
    memset(pLinkState->tCmdLog.szIpAddr, 0, LEN_IP_ADDR_STR);
    memcpy(pLinkState->tCmdLog.szIpAddr, pLinkState->tnIpAddr, LEN_IP_ADDR_STR);

    if (0 != strlen(pString))
    {
        strncpy(pLinkState->sUserName, pString, sizeof(pLinkState->sUserName));
    }

    /* ����������ʾ��, ��ʾ�û��������� */
    SendPasswordPromptToTelnet(pLinkState,NULL);

    /* �ı䵱ǰ״̬��PASSWORD */
    Oam_LinkSetCurRunState(pLinkState, pec_Init_PASSWORD);
    
}

static void CheckUserPwdOfTelnet(TYPE_LINK_STATE *pLinkState, CHAR *pString)
{
    CHAR szPwdMD5[MAX_OAM_USER_PWD_ENCRYPT_LEN] = {0};
    CHAR *pUserPwd = szPwdMD5;
    CHAR szSysLog[128] = {0};
    CHAR *pSysLog = szSysLog;
    
    if (pString == NULL)
        return;

    memset(szPwdMD5, 0, MAX_OAM_USER_PWD_ENCRYPT_LEN);
    XOS_snprintf(szPwdMD5, sizeof(szPwdMD5), "%s", MDString(pString));
    XOS_SysLog(OAM_CFG_LOG_DEBUG, "[CheckUserPwdOfTelnet]:password : <%s> \n", pString);
    XOS_SysLog(OAM_CFG_LOG_DEBUG, "[CheckUserPwdOfTelnet]:password after md5: <%s> \n", pUserPwd);
    
    strncpy(pLinkState->sPasswordOrg, pString, sizeof(pLinkState->sPasswordOrg));
    strncpy(pLinkState->sPassword, pUserPwd, sizeof(pLinkState->sPassword));
	
    /* ��Ȩ����Ȩʧ�ܴ��� */
    //if (strcmp(pLinkState->sPassword, pUserPwd) != 0)
    //if (CliLoginAuth(pLinkState->sUserName, pLinkState->sPasswordOrg) != true)
    if (CliLoginAuth(pLinkState) != true)
    {
        /* ��¼ʧ�ܣ���¼��־ */
        pLinkState->tUserLog.ucLoginRslt = USER_LOGIN_FAIL;
        /* д��־�ļ� */
        if(!InsertOneUseLog(pLinkState))
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, "[CheckUserPwdOfTelnet] InsertOneUseLog failed! line = %d\n", __LINE__);
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
        
        pLinkState->ucInputErrTimes++;
        /* �ж��Ƿ񳬹��������Դ��� */
        if (pLinkState->ucInputErrTimes >= MAX_VERIFYFAILURE)
        {
            SendLogoutToTelnet(pLinkState,Hdr_szOsBadPassword); /* ��ʾ������Ϣ,�˳� */
        }
        else
        {
            pLinkState->bOutputCmdLine = FALSE;
            SendStringToTelnet(pLinkState,Hdr_szOsBadPassword); /* ��ʾ������Ϣ,��ʾ�û������������� */
            /* �����û�����ʾ��, ��ʾ�û������û��� */
            SendUserNamePromptToTelnet(pLinkState,NULL);

            /* �ı䵱ǰ״̬��username */
            Oam_LinkSetCurRunState(pLinkState, pec_Init_USERNAME);     
        }
        return;
    }
    /* ����Telnet�˾��Գ�ʱ��ʱ�� */
    /* 614000678048 (ȥ��24Сʱ���Գ�ʱ) */
    /*Oam_KillAbsTimer(pLinkState);
    Oam_SetAbsTimer(pLinkState);*/

    /* ��¼�ɹ�����¼��־ */
    pLinkState->tUserLog.ucLoginRslt = USER_LOGIN_SUCCESS;
    /* д���ļ� */
    if(!InsertOneUseLog(pLinkState))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[CheckUserPwdOfTelnet]InsertOneUseLog failed! line = %d\n", __LINE__);
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

    /* ��ʼ�����,��Telnet������Ϣ��ɳ��Ի� */
    SendStartupToTelnet(pLinkState);
    
}

static void ProcessSSHLogin(TYPE_LINK_STATE *pLinkState, CHAR *pString)
{
    CHAR sUsername[MAX_USERNAME_LEN] = {0};
    CHAR sPassword[MAX_PASSWORD_LEN] = {0};
    CHAR *pUserName = sUsername;
    CHAR *pPassword = sPassword;
    CHAR *pStrTemp = NULL;
    BOOL bChkResult = FALSE;
    /*BYTE i = 0;*/
//    TYPE_USER_INFO tUserInfo = {0};
    BOOL bUserExists = FALSE;
    BOOL bPassRight = FALSE;		
    CHAR szSysLog[128] = {0};
    CHAR *pSysLog = szSysLog;
//    CHAR szPwdMD5[MAX_OAM_USER_PWD_ENCRYPT_LEN] = {0};

    if (NULL == pString)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[ProcessSSHLogin]:Input string is NULL! \n");
        return;
    }
    
    /* SSHServer�����ĵ�¼�û������������ַ�'?'���� */
    pStrTemp = strchr(pString, '?');
    if (!pStrTemp)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[ProcessSSHLogin]:Input string is error! \n");
        return;
    }
    else
    {
        /* ��ȡ�û��� */
        memcpy(pUserName, pString, pStrTemp - pString);
    }
    /* ��ȡ���� */
    pStrTemp++;
    memcpy(pPassword, pStrTemp, strlen(pStrTemp));

    /* ��¼��־��¼��Ϣ */
    strncpy(pLinkState->tUserLog.sUserName, pUserName, sizeof(pLinkState->tUserLog.sUserName));
    memset(pLinkState->tUserLog.tnIpAddr, 0, LEN_IP_ADDR_STR);
    memcpy(pLinkState->tUserLog.tnIpAddr, pLinkState->tnIpAddr, LEN_IP_ADDR_STR);
    XOS_GetSysClock(&(pLinkState->tUserLog.tSysClockLogin));

    /* ��������־��¼��Ϣ */
    strncpy(pLinkState->tCmdLog.szUserName, pUserName, sizeof(pLinkState->tCmdLog.szUserName));
    memset(pLinkState->tCmdLog.szIpAddr, 0, LEN_IP_ADDR_STR);
    memcpy(pLinkState->tCmdLog.szIpAddr, pLinkState->tnIpAddr, LEN_IP_ADDR_STR);
    
    /* �����û���/����/Ȩ�� */
    strncpy(pLinkState->sUserName, pUserName, sizeof(pLinkState->sUserName));
    strncpy(pLinkState->sPasswordOrg, pPassword, sizeof(pLinkState->sPasswordOrg));
    strncpy(pLinkState->sPassword, MDString(pPassword), sizeof(pLinkState->sPassword));
	
    /* 1.��֤�û� */ 
    //bChkResult = CliLoginAuth(pLinkState->sUserName, pLinkState->sPasswordOrg);
    bChkResult = CliLoginAuth(pLinkState);
#if 0
    if (FindUserInfoFromLink(pUserName, &tUserInfo))
    {
        bUserExists = TRUE;
        /* У������ */
        XOS_snprintf(szPwdMD5, sizeof(szPwdMD5), "%s", MDString(pPassword));
        if (strcmp(tUserInfo.szPassWord, szPwdMD5) == 0)
        {
             bPassRight = TRUE;       
            /* �����û���/����/Ȩ�� */
            strncpy(pLinkState->sUserName, pUserName, sizeof(pLinkState->sUserName));
            strncpy(pLinkState->sPasswordOrg, pPassword, sizeof(pLinkState->sPasswordOrg));
            strncpy(pLinkState->sPassword, tUserInfo.szPassWord, sizeof(pLinkState->sPassword));
            pLinkState->ucUserRight = tUserInfo.ucRole;
            bChkResult = TRUE;
        }
        else
            pLinkState->ucInputErrTimes++;

        if (bChkResult)
        {
            if (tUserInfo.bLocked)
            {
                /* ����ǹ���Ա�û�������ָ��IP�����ϵ�¼�����Զ����� */
                if ((0 == strcmp(pUserName, DEFAULT_USER_NAME)) && 
                    (0 == strcmp(g_szSpecifyIP, (CHAR *)pLinkState->tnIpAddr)))
                {
                    if(!UnLockUser(pUserName))
                        XOS_SysLog(OAM_CFG_LOG_ERROR, "[Process_SSH_Login]:UnLockUser failed! \n");
                    else
                    {
                        XOS_SysLog(OAM_CFG_LOG_DEBUG, "[Process_SSH_Login]:UnLockUser success! \n");
                        tUserInfo.bLocked = FALSE;
                    }
                }
                else
                    bChkResult = FALSE;
            }
        }
    }
#endif

    /* ��֤ʧ�ܴ��� */ 
    if (!bChkResult)
    {
        /* ��¼ʧ�ܣ���¼��־ */
        pLinkState->tUserLog.ucLoginRslt = USER_LOGIN_FAIL;
        /* д��־�ļ� */
        if(!InsertOneUseLog(pLinkState))
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, "InsertOneUseLog failed! \n");
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
        
        if (bUserExists)
        {
            /* ���������¼�������� */
            if (pLinkState->ucInputErrTimes >= MAX_VERIFYFAILURE)
            {
                /* ����Ա�û��Ҵ�ָ��IP�����ϵ�¼�������ƣ����඼Ҫ������ */
                if (!((strcmp(pUserName, DEFAULT_USER_NAME) == 0) && 
                       (strcmp(g_szSpecifyIP, (CHAR *)pLinkState->tnIpAddr) == 0)))
                {
                    if(!LockUser(pUserName))
                        XOS_SysLog(OAM_CFG_LOG_ERROR, "[Process_SSH_Login]:LockUser failed! \n");
                    else
                        XOS_SysLog(OAM_CFG_LOG_ERROR, "[Process_SSH_Login]:LockUser success! \n");
                }
            }
        }
        
        pLinkState->bSSHAuthRslt = FALSE;
	 if((!bUserExists) || (!bPassRight))
	 {
            SendStringToTelnet(pLinkState,map_szUserOrPwdErr); /* ��ʾ������Ϣ,��ʾ�û����������û��� */	 
	 }
	 else
        {
            SendStringToTelnet(pLinkState,map_szUserBeenLocked); /* ��ʾ������Ϣ,��ʾ�û����������û��� */
            pLinkState->ucInputErrTimes = 0;
        }


        /* ��¼ʧ��֮�󣬽���ǰ״̬��Ϊpec_Init_USERNAME���ȴ���һ�ε�¼ */
        Oam_LinkSetCurRunState(pLinkState, pec_Init_USERNAME);
        return;
    }

    pLinkState->tUserLog.ucLoginRslt = USER_LOGIN_SUCCESS;
    /* д���ļ� */
    if(!InsertOneUseLog(pLinkState))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "InsertOneUseLog failed! \n");
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

    /* ����Telnet�˾��Գ�ʱ��ʱ�� */
    /* 614000678048 (ȥ��24Сʱ���Գ�ʱ) */
    /*Oam_KillAbsTimer(pLinkState);
    Oam_SetAbsTimer(pLinkState); */

    /* ��ʼ�����,��Telnet������Ϣ��ɳ��Ի� */
    pLinkState->bSSHAuthRslt = TRUE;
    SendStartupToTelnet(pLinkState);
}


/**************************************************************************
* �������ƣ�VOID Telnet_Msg_Handler
* ������������������Telnet����Ϣ
* ���ʵı���
* �޸ĵı���
* ���������TYPE_LINK_STATE *pLinkState,TYPE_TELNET_MSGHEAD *ptMsg
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
static void Telnet_Msg_Handler(TYPE_LINK_STATE *pLinkState,TYPE_TELNET_MSGHEAD *ptMsg)
{
    TYPE_MATCH_RESULT *pMatchResult = &(pLinkState->tMatchResult);
    EXTRAINFO tExtraInfo;
    WORD16 wDisp_pos;
    INT32 iPos;
    CHAR * pInString = NULL;
    /*BOOL bChkResult = FALSE;*/
    BOOL bSSHMsg = FALSE;
    CHAR szSysLog[128] = {0};
    CHAR *pSysLog = szSysLog;

    /************************* Ԥ���� ***********************************/
    /* ������Ϣ��ˮ�� */
    pLinkState->wTelSeqNo = ptMsg->wSeqNo;

    /* ��ӽ�����\0 */
    *(ptMsg->pData + ptMsg->wDataLen) = MARK_STRINGEND;
    ++(ptMsg->wDataLen);

    /* �ж��Ƿ�ΪSSH����(SSH���ӵ���·�ŷ�Χ:5-7) */
    if (ptMsg->bVtyNum >= 5 && ptMsg->bVtyNum <= 7)
        bSSHMsg = TRUE;

    /********************* ���ȴ����˳���Ϣ *****************************/
    if (ptMsg->bMsgID == MSGID_TERMINATE)
    {
        if ((strlen(pLinkState->sUserName) > 0) && 
            (pLinkState->tUserLog.tSysClockLogout.wSysYear == 0) &&
            (pLinkState->tUserLog.ucLoginRslt == USER_LOGIN_SUCCESS))
        {
            XOS_GetSysClock(&(pLinkState->tUserLog.tSysClockLogout));
            pLinkState->tUserLog.ucQuitReason = USER_LOGOUT_COMMON;
            /* д��־�ļ� */
            if(!UpdateOneUseLog(pLinkState))
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR, "[Telnet_Msg_Handler] UpdateOneUseLog failed!\n");
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

        /*CRDCM00175124 �˳�ʱ��Ҫֹͣ����telnet����ִ�е�����*/
        Oam_CfgStopExcutingCmd(pLinkState, NULL);
        Oam_InnerCmdLogout(pLinkState);

        return;
    }

    /*611000766546 ����Ѿ����ڵ������ַ����½���������
	˵����climanager��telnetserver����״̬��һ���ˣ��Ͽ�climanager���ӣ���telnetserverΪ׼*/
    if (ptMsg->bMsgID == MSGID_NEWCONNECT)
    {
        if (Oam_LinkGetCurRunState(pLinkState)  != pec_MsgHandle_IDLE)
        {
            Oam_CfgStopExcutingCmd(pLinkState, NULL);
            
            /* 1.ɱ��Telnet�˶�ʱ�� */
            Oam_KillIdleTimer(pLinkState);
            /* 614000678048 (ȥ��24Сʱ���Գ�ʱ) */
            /*Oam_KillAbsTimer(pLinkState);*/
            /* 2.ɱ��Execute�˶�ʱ��, ���ٵȴ�Execute��ִ������ؽ�� */
            Oam_KillExeTimer(pLinkState);

            Oam_KillCmdPlanTimer(pLinkState);
            
            /* 3.��Ϊ����״̬ */
            Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_IDLE);
        }
    }

    /*********************** �û���¼�׶� *******************************/
    if (Oam_LinkGetCurRunState(pLinkState) <= pec_MsgHandle_INIT)
    {
        switch(Oam_LinkGetCurRunState(pLinkState))
        {
        case pec_MsgHandle_IDLE:
            if (ptMsg->bMsgID != MSGID_NEWCONNECT)
                break;     

            Oam_LinkInitializeForNewConn(pLinkState);

            /* ����Telnet���û���ʶ��IP��ַ */
            Oam_LinkSetLinkId(pLinkState, ptMsg->bVtyNum);
			
            if (ptMsg->wDataLen <= LEN_IP_ADDR_STR)
            {
                memcpy(pLinkState->tnIpAddr, ptMsg->pData, ptMsg->wDataLen);
                memset(pLinkState->tCmdLog.szIpAddr, 0, LEN_IP_ADDR_STR);
                memcpy(pLinkState->tCmdLog.szIpAddr, ptMsg->pData, ptMsg->wDataLen);
            }

            /* ������������ */
            if (1 == ptMsg->bVtyNum)
                pLinkState->tUserLog.ucConnType = OAM_CONN_TYPE_SERIAL;
            else if ((ptMsg->bVtyNum >= 2) && (ptMsg->bVtyNum <= 4))
                pLinkState->tUserLog.ucConnType = OAM_CONN_TYPE_TELNET;
            else
                pLinkState->tUserLog.ucConnType = OAM_CONN_TYPE_SSH;

            pLinkState->tCmdLog.ucConnType = pLinkState->tUserLog.ucConnType;

            if (!bSSHMsg)
            {
                /* 3.����ϵͳ��ʼ����Ϣ���û�����ʾ��, ��ʾ�û������û��� */
                SendSysInitInfoToTelnet(pLinkState);
            }
            else
            {
                pLinkState->bSSHConnRslt = TRUE;
                SendStringToTelnet(pLinkState,NULL);
                Oam_LinkSetCurRunState(pLinkState, pec_Init_USERNAME);
            }

            break;

        case pec_Init_USERNAME:
            if (ptMsg->bMsgID != MSGID_SENDSTRING)
                break;

            pInString = (CHAR *)ptMsg->pData;
            pInString = OAM_LTrim(OAM_RTrim(pInString));

            if (!bSSHMsg)
            {
                CheckUserNameOfTelnet(pLinkState, pInString);
            }
            else
            {
                pLinkState->bSSHAuthMsg = ptMsg->bSSHAuth;
                ProcessSSHLogin(pLinkState, pInString);
            }
            
            break;

        case pec_Init_PASSWORD:
            if (ptMsg->bMsgID != MSGID_SENDSTRING)
                break;

            pInString = (CHAR *)ptMsg->pData;
            pInString = OAM_LTrim(OAM_RTrim(pInString));

            CheckUserPwdOfTelnet(pLinkState, pInString);

            break;

        default:
            break;
        }
        return;
    }

    /********************* �����ƥ��׶� *****************************/
    switch(ptMsg->bMsgID)
    {
    case MSGID_REQUESTDATA: /* �ٴ��������� */

        /* �ϴ�Execute���ؽ����û�з��������������� */
        if (pLinkState->wSendPos != 0)
        {
            SendPacketToTelnet(pLinkState, FALSE);
            break;
        }

        /* �ϴ�Execute���ؽ���Ѿ�������������ݰ�����, ����������� */
        /* ??����յ����һ�������Զ���ʾ�����У���������ظ���*/
/*
        if (Oam_LinkIfRecvLastPktOfApp(pLinkState))
        {
            pLinkState->bOutputCmdLine = TRUE;
            SendPacketToTelnet(pLinkState, FALSE);
            break;
        }
*/
        /* �ж����ģʽ�Ƿ���ȷ 
        if (pLinkState->bOutputMode != OUTPUT_MODE_REQUEST)
        {
            SendStringToTelnet(pLinkState,Hdr_szTelRequestOutMode);
            Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_TELNET);
            break;
        }
        */
        /*���Ӧ����Ϣ�ȴ��ٴ�����*/
        if (SUCC_CMD_NOT_FINISHED == pLinkState->wAppMsgReturnCode &&
            !pLinkState->bRestartReqSended)
        {
            ExecSendCmdAgain(pLinkState);
        }
        break;

    case MSGID_CEASEDATA: /*��ֹ����*/
        Oam_CfgStopExcutingCmd(pLinkState, NULL);
        break;

    case MSGID_ASKTELACK: /* ���������ʾ��Ϣ */

        /* ���������ʾ����ȷλ�� */
        memset(&tExtraInfo,0,sizeof(EXTRAINFO));
        if (ptMsg->wDataLen<= sizeof(EXTRAINFO))
            memcpy(&tExtraInfo, (BYTE *)ptMsg->pData, ptMsg->wDataLen);
        else
            memcpy(&tExtraInfo, (BYTE *)ptMsg->pData, sizeof(EXTRAINFO));
        wDisp_pos = tExtraInfo.disp_pos;
        iPos = CalculateMatchPosition(pLinkState);
        iPos = ((INT32)pMatchResult->wCliMatchLen > iPos) ? pMatchResult->wCliMatchLen : iPos;
        /* ע��wDisp_pos��ʼ��ʾ������, ����ǰ�����ַ���".."����,
           Ҳ����ʵ�ʴ�wDisp_pos+2��ʼ��ʾiPos�Ǵ�1��ʼ��ŵ�, 
           ͬwDisp_pos��0��ʼ��Ų�ͬ */

        if(wDisp_pos != 0)/* �������ֻع�����Ҫ���⴦�� */
        {
            if(iPos < wDisp_pos + 2)
                iPos = 2;                /*ƥ�䲻��ȷλ�ñ��ع�����,��ʾλ���ڻع��㴦".."*/
            else
                iPos = iPos - wDisp_pos; /*��ʾλ��Ϊ��ǰλ��ȥ��-��������ʾ��ʼ��*/
        }
        iPos += strlen(pLinkState->sPrompt);

        /* ��ʾ������ʾ��Ϣ */
        SendAskInfoToTelnet(pLinkState,iPos);
        break;

    case MSGID_SENDSTRING:
        /*??�������Ʋ�������ȥ�������ģʽ����������*/
        /* ��Request���ģʽ������²������û�sendstring 
        if (pLinkState->bOutputMode == OUTPUT_MODE_REQUEST)
            break;
       */        
         
        pLinkState->bOutputMode     = OUTPUT_MODE_NORMAL;
        /* ���������ַ��� */
        Process_Input_String(pLinkState,ptMsg);
        break;

    default:
        break;
    }
    return;
}

/**************************************************************************
* �������ƣ�INT32 CheckTelMsgHead
* �������������Telnet��Ϣͷ
* ���ʵı���
* �޸ĵı���
* ���������TYPE_LINK_STATE *pLinkState,TYPE_TELNET_MSGHEAD *ptMsg
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
static INT32 CheckTelMsgHead(TYPE_LINK_STATE *pLinkState,TYPE_TELNET_MSGHEAD *ptMsg)
{
    switch(ptMsg->bMsgID)
    {
    case MSGID_SENDSTRING:
        switch(ptMsg->bDeterminer)
        {
        case DM_NORMAL:
        case DM_TAB:
        case DM_QUESTION:
        case DM_BLANKQUESTION:
            break;
        default:
            return MSGERR_DETERMINER;
        }
        break;
    case MSGID_NEWCONNECT:
    case MSGID_TERMINATE:
    case MSGID_REQUESTDATA:
    case MSGID_CEASEDATA:
    case MSGID_ASKTELACK:
        break;
    default:
        return MSGERR_MSGID;
    }

    if (Oam_LinkGetCurRunState(pLinkState) > pec_MsgHandle_IDLE && ptMsg->bVtyNum != Oam_LinkGetLinkId(pLinkState))
        return MSGERR_VTYNUM;

    return MSG_SUCCESS;
}

/**************************************************************************
*                          ȫ�ֺ���ʵ��                                   *
**************************************************************************/
/**************************************************************************
* �������ƣ�VOID RecvFromTelnet
* ������������������Telnet����Ϣ
* ���ʵı���
* �޸ĵı���
* ���������TYPE_LINK_STATE *pLinkState,PVOID pMsgPara
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
void RecvFromTelnet(TYPE_LINK_STATE *pLinkState, LPVOID pMsgPara)
{
    TYPE_TELNET_MSGHEAD *ptRcvMsg = (TYPE_TELNET_MSGHEAD *)g_ptOamIntVar->aucRecvBuf;
    WORD16 wMsgLen = 0;
    INT32 iErrCode = MSG_SUCCESS;

    /*~��Ҫ������ֵ*/
    XOS_GetMsgDataLen(PROCTYPE_OAM_CLIMANAGER, &wMsgLen);

    /* 1.����Telnet�˿��г�ʱ��ʱ�� */
    Oam_KillIdleTimer(pLinkState);
    Oam_SetIdleTimer(pLinkState);
	
    /* 2.������Ϣ�����ػ��� */
    if (wMsgLen > MAX_OAMMSG_LEN)
        wMsgLen = MAX_OAMMSG_LEN;
    memcpy(ptRcvMsg, pMsgPara, wMsgLen);

    /*CRDCM00178927 ������telnet���ӷ�������Ϣ�����ȫ�������Ƿ����*/
    if (!Oam_CfgGlobalPointersIsOk())
    {
        TYPE_TELNET_MSGHEAD *ptMsg = ptRcvMsg;
         /*��ֹ��һ�ε�¼ʱ�����
         ����Ҫ�ȱ���Telnet���û���ʶ��IP��ַ */
        Oam_LinkSetLinkId(pLinkState, ptMsg->bVtyNum);
        /* ���OAM�ű���ȡʧ��ֱ��Logout */
        SendLogoutToTelnet(pLinkState,READ_CMD_SCRIPT_ERR);
        return;
    }

    /* 3.�����Ϣͷ */
    iErrCode = CheckTelMsgHead(pLinkState,ptRcvMsg);
    if (iErrCode !=  MSG_SUCCESS)
    {
        SendTelMsgErrToTelnet(pLinkState,iErrCode);
        return;
    }

    /* 4.������Ϣ */
    Telnet_Msg_Handler(pLinkState,ptRcvMsg);
    return;
}

/*���OAM�ű���ص�ȫ�ֱ����Ƿ�Ϸ�*/
BOOLEAN Oam_CfgGlobalPointersIsOk(void)
{
    if (!(gtMode &&
            gtTreeNode &&
            CLI_szCLIHelp0 &&
            CLI_szCLIHelp1 &&
            CLI_szCLIHelp2 &&
            CLI_szCLIHelp3 &&
            CLI_szCLIHelp4 &&
            CLI_szCLIHelp5 &&
            CLI_szCLIHelp6 &&
            CLI_szCLIHelp7 &&
            CLI_szCLIHelp8 &&
            CLI_szCLIHelp9 &&
            CLI_szCLIHelp10 &&
            CLI_szCLIHelp11 &&
            CLI_szCLIHelp12 &&
            CLI_szCLIHelp13 &&
            CLI_szCLIHelp14 &&
            CLI_szCLIHelp15 &&
            CLI_szCLIHelp16 &&
            CLI_szCLIHelp17 &&
            CLI_szCLIHelp18 &&
            CLI_szCLIHelp19 &&
            CLI_szCLIHelp20 &&
            CLI_szCLIHelp21 &&
            CLI_szCLIHelp22 &&
            CLI_szCLIHelp23 &&
            CLI_szCLIHelp24 &&
            CLI_szCLIHelp25 &&
            Hdr_szEnterString &&
            Hdr_szUserName &&
            Hdr_szPassword &&
            Hdr_szCmdNoCmdTip &&
            Hdr_szTelInputText &&
            Hdr_szOsBadUserName &&
            Hdr_szOsBadPassword &&
            Hdr_szCanNotEnterConfig &&
            Hdr_szEnterConfigTip &&
            Hdr_szEnterConfigWarning &&
            Hdr_szCanNotLogin &&
            Hdr_szExeNoReturn &&
            Hdr_szExeNoDisplay &&
            Hdr_szExeTimeout  &&
            Hdr_szSlaveAckTimeout &&
            Hdr_szTelRequestOutMode &&
            Hdr_szCanNotChangMode &&
            Hdr_szDisplayScriptErr &&
            Hdr_szCeasedTheCmdTip &&
            Hdr_szUpdateDatTip &&
            Hdr_szNeNotSupportOnlineCfg  &&
            Hdr_szSavePrtclCfgOK     &&
            Hdr_szSavePrtclCfgFail    &&
            Hdr_szOpenSaveFileErr     &&
            Hdr_szInsertCrcErr &&
            Hdr_szMSChangeoverTip     &&
            Hdr_szCommandMutexTip     &&
            Hdr_szSaveMutexTip        &&
            Hdr_szDatUpdateMutexTip   &&
            Hdr_szSlaveSaveOk         &&
            Hdr_szSlaveSaveFail       &&
            Err_szExeMsgHead          &&
            Err_szReturnCode          &&
            Err_szLinkChannel         &&
            Err_szSeqNo               &&
            Err_szLastPacket          &&
            Err_szOutputMode          &&
            Err_szCmdID               &&
            Err_szNumber              &&
            Err_szExecModeID          &&
            Err_szIfNo                &&
            Err_szFromIntpr           &&
            Err_szNoNeedTheCmd        &&
            Err_szTelMsgHead          &&
            Err_szMsgId               &&
            Err_szVtyNum              &&
            Err_szDeterminer          &&
            Err_szUndefined           &&
            Hdr_szSaveMasterOnSlaveErr &&
            Hdr_szCannotSaveOnSlave   &&
            Hdr_szUnknownSaveType     &&
            Hdr_szSaveZDBOK           &&
            Hdr_szSaveZDBFail         &&
            Hdr_szSaveZDBErrSaveBothPath &&
            Hdr_szSaveZDBErrMainPath  &&
            Hdr_szSaveZDBErrBackup    &&
            Hdr_szSaveZDBErrLocalSaving &&
            Hdr_szSaveZDBErrMSSaving  &&
            Hdr_szSaveZDBErrOther     &&
            Hdr_szNotAdmin            &&
            Hdr_szCannotDelAdmin  &&
            Hdr_szUserNotExists   &&
            Hdr_szUserBeenDeleted  &&
            Hdr_szCmdExecSucc  &&
            Hdr_szCmdExecFailed  &&
            Hdr_szCannotCreateAdmin  &&
            Hdr_szUserDoesExists  &&
            Hdr_szPasswordRule  &&
            Hdr_szPwdNotMeetRule  &&
            Hdr_szTwoInputPwdNotSame  &&
            Hdr_szConfUserFull  &&
            Hdr_szViewUserFull  &&
            Hdr_szUserLocked  &&
            Hdr_szCannotSetAdminRole  &&
            Hdr_szUserOnline  &&
            Hdr_szNeedNotSetRole  &&
            Hdr_szCannotSetOtherUserPwd  &&
            Hdr_szUserNotLocked  &&
            Hdr_szErrInvalidModule &&
            Hdr_szNoPermission  &&      
            Hdr_szSaveZDBErrNoTblSave  &&
            Hdr_szSaveZDBErrReadFileErr  &&
            Hdr_szSaveZDBErrTblNotNeedSave  &&
            Hdr_szSaveZDBErrPowerOnLoading  &&
            Hdr_szSaveZDBErrFtpLoading  &&
            Hdr_szSaveZDBErrAppFtpLoading  &&
            Hdr_szSaveZDBErrIOBusy  &&
            Hdr_szSaveZDBErrInvalidHandle &&
            map_szAppMsgDatalengthErr &&
            map_szUserBeenLocked &&
            map_szUserOrPwdErr &&
            map_szGotoSupperModeErr &&
            map_szExePlanFmtStr &&
            map_szScriptReloaded &&
            map_szCeaseCmdFailed &&
            map_szCmdSendAgainFailed &&
            map_szDisposeCmdFailed &&
            map_szExeCmdFailed &&
            map_szPrtclRestoreNotify))
    {
        return FALSE;
    }

    return TRUE;
}

/**************************************************************************
* �������ƣ�VOID Xml_CfgStopExcutingCmd
* ����������ֹͣ��ǰTelnet������ִ������
* ���ʵı���
* �޸ĵı���
* ���������pLinkState -- Ҫֹͣ������
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/06/10   V1.0    ������      ����
**************************************************************************/
void Omm_CfgStopExcutingCmd(TYPE_XMLLINKSTATE *pLinkState)
{
    CHAR *ptrOutputResult = NULL;
    if (!pLinkState)
    {
        return;
    }
    /*��ǰ��·��������ִ��*/
    if (pec_MsgHandle_EXECUTE == XML_LinkGetCurRunState(pLinkState))
    {
        T_OAM_Cfg_Dat* cfgDat = NULL;
        cfgDat = pLinkState->tCurCMDInfo.cfgDat;
        if ((!cfgDat) || (!cfgDat->tCmdAttr))
        {
            return;
        }
        /*��Ҫ������ֹ����*/
        if (CMD_ATTR_CANCEASE(&cfgDat->tCmdAttr[pLinkState->tCurCMDInfo.wCmdAttrPos]))
        {
            ExecCeaseOmmCfgCmd(pLinkState);
        }
        else
        {
            ptrOutputResult = ReadMapValueOfOamDat(MAP_TYPE_XMLADAPTER,MAP_KEY_STOP_CMDFORUPDATEDAT);
//            SendErrStringToOmm(pLinkState, ptrOutputResult);                    
            /*ɱ��XML��·��ʱ��*/
  //          Xml_KillExeTimer(pLinkState);
	     /*�ָ�״̬Ϊ����*/
     //       XML_LinkSetCurRunState(pLinkState,pec_MsgHandle_IDLE);
        }
    }        
}


/**************************************************************************
* �������ƣ�VOID Oam_CfgStopExcutingCmd
* ����������ֹͣ��ǰTelnet������ִ������
* ���ʵı���
* �޸ĵı���
* ���������pLinkState -- Ҫֹͣ������
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/06/10   V1.0    ���      ����
**************************************************************************/
void Oam_CfgStopExcutingCmd(TYPE_LINK_STATE *pLinkState, CHAR *pstrReason)
{
    TYPE_MATCH_RESULT *pMatchResult = NULL;
    T_OAM_Cfg_Dat* cfgDat = NULL;

    if (NULL == pLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "Oam_CliStopExcutingCmd: pLinkState is NULL \n");
        return;	
    }
	
    if (pec_MsgHandle_EXECUTE != Oam_LinkGetCurRunState(pLinkState))
    {
        SendCeaseToTelnet(pLinkState);
        return;	
    }
	
    pMatchResult = &(pLinkState->tMatchResult);
    cfgDat = pMatchResult->cfgDat;
            
    if (NULL == cfgDat)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "Oam_CliStopExcutingCmd: cfgDat is NULL \n");
        return;	
    }	
            
    Oam_KillCmdPlanTimer(pLinkState);           
    /*ɱ��Execute�˶�ʱ��*/
    Oam_KillExeTimer(pLinkState);

    /*611001629079 ��Ӱ汾��ʱ���л��ű����ԣ�����ֹ��Ӱ汾����*/
    if (CMD_ATTR_CANCEASE(&cfgDat->tCmdAttr[pLinkState->wCmdAttrPos]) )		
    {
        if (CMD_ATTR_CANCEASE_NEEDACK(&cfgDat->tCmdAttr[pLinkState->wCmdAttrPos]))
        {
            pLinkState->bOutputMode     = OUTPUT_MODE_NORMAL;
            pLinkState->bOutputCmdLine  = TRUE;
            pLinkState->bRecvAsynMsg = FALSE;
            ExecCeaseProtocolCmd(pLinkState);
        }
        else
        {
            if (pstrReason != NULL)
            {
                SendStringToTelnet(pLinkState, pstrReason);
            }
            SendCeaseToTelnet(pLinkState);
        }
    }
}
/*lint +e661*/
/*lint +e420*/


