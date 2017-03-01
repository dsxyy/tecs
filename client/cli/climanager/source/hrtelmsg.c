/**************************************************************************
* 版权所有 (C)2002, 深圳市中兴通讯股份有限公司。
*
* 文件名称：HrTelMsg.c
* 文件标识：
* 内容摘要：处理来自Telnet的消息
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
#include "xmlagent.h"
#include "oam_execute.h"
#include "xmlrpc.h"

/*lint -e661*/
/*lint -e420*/
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
/*static BOOLEAN GlobalPointersIsOk(VOID);*/
/**************************************************************************
*                          局部函数实现                                   *
**************************************************************************/
/**************************************************************************
* 函数名称：VOID Process_Input_String
* 功能描述：处理输入字符串
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_LINK_STATE *pLinkState,MSG_COMM_OAM * ptMsg
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
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
        /* 1.验证用户 */
        if (pInString)
        {
            USER_NODE *ppNode = NULL;
            USER_NODE *pNode = NULL;

            if (strcmp(pInString, pLinkState->sUserName) == 0)
            {
                SendUserNameErrToTelnet(pLinkState,Hdr_szCanNotLogin); /* 显示提示信息,结束 */
                break;
            }
            else if (FindUserFromLink(pInString,&ppNode,&pNode))
            {
                /* 备份用户名/权限 */
                strncpy(pLinkState->sSavedUser, pLinkState->sUserName);
                pLinkState->ucSavedRight = pLinkState->ucUserRight;

                /* 保存用户名/密码/权限 */
                /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
                /*
                strncpy(pLinkState->sUserName, pInString);
                */
                XOS_snprintf(pLinkState->sUserName,
                             sizeof(pLinkState->sUserName) -1,
                             "%s",
                             pInString);

                /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
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

        /* 2.验证失败处理 */
        if (!bChkResult)
        {
            pLinkState->ucInputErrTimes++;
            /* 判断是否超过允许重试次数 */
            if (pLinkState->ucInputErrTimes >= MAX_VERIFYFAILURE)
            {
                SendUserNameErrToTelnet(pLinkState,Hdr_szOsBadUserName); /* 显示错误信息,结束 */
            }
            else
            {
                SendStringToTelnet(pLinkState,Hdr_szOsBadUserName); /* 显示错误信息,提示用户重新输入用户名 */
            }
            break;
        }

        /* 3.发送密码提示符, 提示用户输入密码 */
        SendPasswordPromptToTelnet(pLinkState,NULL);

        /* 4.改变当前状态到TELNET */
        Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_TELNET);

        break;

    case INMODE_PASSWORD:
        /* 1.比较密码 */
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
            /* 判断是否超过允许重试次数 */
            if (pLinkState->ucInputErrTimes >= MAX_VERIFYFAILURE)
            {
                SendPasswordErrToTelnet(pLinkState, Hdr_szOsBadPassword); /* 显示错误信息,结束 */
            }
            else
            {
                SendStringToTelnet(pLinkState, Hdr_szOsBadPassword); /* 显示错误信息,提示用户重新输入密码 */
            }
            break;
        }

        /* 2.切换到相应模式 */
        pLinkState->ucInputErrTimes  = 0;

        /* 改变输入模式 */
        Oam_LinkSetInputState(pLinkState, INMODE_NORMAL);
        switch(cfgDat->tCmdAttr[pLinkState->wCmdAttrPos].dExeID)
        {
#if 0            
        case CMD_INTERPT_LOGIN:
            /* 设置Telnet端绝对超时定时器 */
            Oam_KillAbsTimer(pLinkState);
            Oam_SetAbsTimer(pLinkState);

            /* 改变提示符 */
            pLinkState->bChangeMode = TRUE;
            ChangePrompt(pLinkState,Oam_GetCurModeId(pLinkState));
            break;
#endif            
        case CMD_INTERPT_ENABLE:
            /* 备份当前用户及权限并保存为超级用户及权限 */
            pLinkState->ucSavedRight = pLinkState->ucUserRight;
            pLinkState->ucUserRight  = DEFAULT_ENABLE_RITHT;
            pLinkState->bIsSupperUsrMode = TRUE;

            /* 改变模式和提示符 */
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
        /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
        XOS_snprintf(szTnTempBuf,
                     sizeof(szTnTempBuf),
                     "%s\n",
                     szTnTempBuf);

        /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
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
                /* 保存sBanner */
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
        /* 匹配命令行 */
        Oam_InptFreeBuf(pLinkState->ucLinkID);
		
        /* 如果当前有其他命令正在执行，则丢弃当前命令 */
        if (Oam_LinkGetCurRunState(pLinkState) == pec_MsgHandle_EXECUTE)
            return;

        ParseCmdLine(pLinkState, pInString,ptMsg->bDeterminer,FALSE);
        /*解析成功，保存配置记录*/
        if (PARSESTATE_FINISH_PARSE == pLinkState->tMatchResult.wParseState)
        {
            /*??日志功能暂不实现*/
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

    /* 登录日志记录信息 */
    strncpy(pLinkState->tUserLog.sUserName, pString, sizeof(pLinkState->tUserLog.sUserName));
    memset(pLinkState->tUserLog.tnIpAddr, 0, LEN_IP_ADDR_STR);
    memcpy(pLinkState->tUserLog.tnIpAddr, pLinkState->tnIpAddr, LEN_IP_ADDR_STR);
    XOS_GetSysClock(&(pLinkState->tUserLog.tSysClockLogin));

    /* 命令行日志记录信息 */
    strncpy(pLinkState->tCmdLog.szUserName, pString, sizeof(pLinkState->tCmdLog.szUserName));
    memset(pLinkState->tCmdLog.szIpAddr, 0, LEN_IP_ADDR_STR);
    memcpy(pLinkState->tCmdLog.szIpAddr, pLinkState->tnIpAddr, LEN_IP_ADDR_STR);

    memset(&tUserInfo, 0, sizeof(tUserInfo));
    if (0 != strlen(pString))
    {
        if (FindUserInfoFromLink(pString, &tUserInfo))
        {
            /* 保存用户名/密码/权限 */
            strncpy(pLinkState->sUserName, pString, sizeof(pLinkState->sUserName));
            strncpy(pLinkState->sPassword, tUserInfo.szPassWord, sizeof(pLinkState->sPassword));
            pLinkState->ucUserRight = tUserInfo.ucRole;
            bUserExist = TRUE;
        }
    }

    /* 验证失败处理 */
    if (!bUserExist)
    {
        /* 登录失败，记录日志 */
        pLinkState->tUserLog.ucLoginRslt = USER_LOGIN_FAIL;
        /* 写日志文件 */
        if(!InsertOneUseLog(pLinkState))
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, "[CheckUserNameOfTelnet] InsertOneUseLog failed! line = %d\n", __LINE__);
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

        pLinkState->ucInputErrTimes++;
        /* 判断是否超过允许重试次数 */
        if (pLinkState->ucInputErrTimes >= MAX_VERIFYFAILURE)
        {
            SendLogoutToTelnet(pLinkState,Hdr_szOsBadUserName); /* 显示错误信息,退出 */
        }
        else
        {
            SendStringToTelnet(pLinkState,Hdr_szOsBadUserName); /* 显示错误信息,提示用户重新输入用户名 */
        }
        return;
    }
#endif
    
    /* 登录日志记录信息 */
    strncpy(pLinkState->tUserLog.sUserName, pString, sizeof(pLinkState->tUserLog.sUserName));
    memset(pLinkState->tUserLog.tnIpAddr, 0, LEN_IP_ADDR_STR);
    memcpy(pLinkState->tUserLog.tnIpAddr, pLinkState->tnIpAddr, LEN_IP_ADDR_STR);
    XOS_GetSysClock(&(pLinkState->tUserLog.tSysClockLogin));

    /* 命令行日志记录信息 */
    strncpy(pLinkState->tCmdLog.szUserName, pString, sizeof(pLinkState->tCmdLog.szUserName));
    memset(pLinkState->tCmdLog.szIpAddr, 0, LEN_IP_ADDR_STR);
    memcpy(pLinkState->tCmdLog.szIpAddr, pLinkState->tnIpAddr, LEN_IP_ADDR_STR);

    if (0 != strlen(pString))
    {
        strncpy(pLinkState->sUserName, pString, sizeof(pLinkState->sUserName));
    }

    /* 发送密码提示符, 提示用户输入密码 */
    SendPasswordPromptToTelnet(pLinkState,NULL);

    /* 改变当前状态到PASSWORD */
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
	
    /* 鉴权及鉴权失败处理 */
    //if (strcmp(pLinkState->sPassword, pUserPwd) != 0)
    //if (CliLoginAuth(pLinkState->sUserName, pLinkState->sPasswordOrg) != true)
    if (CliLoginAuth(pLinkState) != true)
    {
        /* 登录失败，记录日志 */
        pLinkState->tUserLog.ucLoginRslt = USER_LOGIN_FAIL;
        /* 写日志文件 */
        if(!InsertOneUseLog(pLinkState))
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, "[CheckUserPwdOfTelnet] InsertOneUseLog failed! line = %d\n", __LINE__);
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
        
        pLinkState->ucInputErrTimes++;
        /* 判断是否超过允许重试次数 */
        if (pLinkState->ucInputErrTimes >= MAX_VERIFYFAILURE)
        {
            SendLogoutToTelnet(pLinkState,Hdr_szOsBadPassword); /* 显示错误信息,退出 */
        }
        else
        {
            pLinkState->bOutputCmdLine = FALSE;
            SendStringToTelnet(pLinkState,Hdr_szOsBadPassword); /* 显示错误信息,提示用户重新输入密码 */
            /* 发送用户名提示符, 提示用户输入用户名 */
            SendUserNamePromptToTelnet(pLinkState,NULL);

            /* 改变当前状态到username */
            Oam_LinkSetCurRunState(pLinkState, pec_Init_USERNAME);     
        }
        return;
    }
    /* 设置Telnet端绝对超时定时器 */
    /* 614000678048 (去掉24小时绝对超时) */
    /*Oam_KillAbsTimer(pLinkState);
    Oam_SetAbsTimer(pLinkState);*/

    /* 登录成功，记录日志 */
    pLinkState->tUserLog.ucLoginRslt = USER_LOGIN_SUCCESS;
    /* 写入文件 */
    if(!InsertOneUseLog(pLinkState))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[CheckUserPwdOfTelnet]InsertOneUseLog failed! line = %d\n", __LINE__);
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

    /* 初始化完成,向Telnet发送信息完成初试化 */
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
    
    /* SSHServer过来的登录用户名和密码用字符'?'连接 */
    pStrTemp = strchr(pString, '?');
    if (!pStrTemp)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[ProcessSSHLogin]:Input string is error! \n");
        return;
    }
    else
    {
        /* 提取用户名 */
        memcpy(pUserName, pString, pStrTemp - pString);
    }
    /* 提取密码 */
    pStrTemp++;
    memcpy(pPassword, pStrTemp, strlen(pStrTemp));

    /* 登录日志记录信息 */
    strncpy(pLinkState->tUserLog.sUserName, pUserName, sizeof(pLinkState->tUserLog.sUserName));
    memset(pLinkState->tUserLog.tnIpAddr, 0, LEN_IP_ADDR_STR);
    memcpy(pLinkState->tUserLog.tnIpAddr, pLinkState->tnIpAddr, LEN_IP_ADDR_STR);
    XOS_GetSysClock(&(pLinkState->tUserLog.tSysClockLogin));

    /* 命令行日志记录信息 */
    strncpy(pLinkState->tCmdLog.szUserName, pUserName, sizeof(pLinkState->tCmdLog.szUserName));
    memset(pLinkState->tCmdLog.szIpAddr, 0, LEN_IP_ADDR_STR);
    memcpy(pLinkState->tCmdLog.szIpAddr, pLinkState->tnIpAddr, LEN_IP_ADDR_STR);
    
    /* 保存用户名/密码/权限 */
    strncpy(pLinkState->sUserName, pUserName, sizeof(pLinkState->sUserName));
    strncpy(pLinkState->sPasswordOrg, pPassword, sizeof(pLinkState->sPasswordOrg));
    strncpy(pLinkState->sPassword, MDString(pPassword), sizeof(pLinkState->sPassword));
	
    /* 1.验证用户 */ 
    //bChkResult = CliLoginAuth(pLinkState->sUserName, pLinkState->sPasswordOrg);
    bChkResult = CliLoginAuth(pLinkState);
#if 0
    if (FindUserInfoFromLink(pUserName, &tUserInfo))
    {
        bUserExists = TRUE;
        /* 校验密码 */
        XOS_snprintf(szPwdMD5, sizeof(szPwdMD5), "%s", MDString(pPassword));
        if (strcmp(tUserInfo.szPassWord, szPwdMD5) == 0)
        {
             bPassRight = TRUE;       
            /* 保存用户名/密码/权限 */
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
                /* 如果是管理员用户并且在指定IP机器上登录，则自动解锁 */
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

    /* 验证失败处理 */ 
    if (!bChkResult)
    {
        /* 登录失败，记录日志 */
        pLinkState->tUserLog.ucLoginRslt = USER_LOGIN_FAIL;
        /* 写日志文件 */
        if(!InsertOneUseLog(pLinkState))
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, "InsertOneUseLog failed! \n");
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
        
        if (bUserExists)
        {
            /* 错误密码登录三次以上 */
            if (pLinkState->ucInputErrTimes >= MAX_VERIFYFAILURE)
            {
                /* 管理员用户且从指定IP机器上登录不受限制，其余都要被锁定 */
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
            SendStringToTelnet(pLinkState,map_szUserOrPwdErr); /* 显示错误信息,提示用户重新输入用户名 */	 
	 }
	 else
        {
            SendStringToTelnet(pLinkState,map_szUserBeenLocked); /* 显示错误信息,提示用户重新输入用户名 */
            pLinkState->ucInputErrTimes = 0;
        }


        /* 登录失败之后，将当前状态置为pec_Init_USERNAME，等待下一次登录 */
        Oam_LinkSetCurRunState(pLinkState, pec_Init_USERNAME);
        return;
    }

    pLinkState->tUserLog.ucLoginRslt = USER_LOGIN_SUCCESS;
    /* 写入文件 */
    if(!InsertOneUseLog(pLinkState))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "InsertOneUseLog failed! \n");
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

    /* 设置Telnet端绝对超时定时器 */
    /* 614000678048 (去掉24小时绝对超时) */
    /*Oam_KillAbsTimer(pLinkState);
    Oam_SetAbsTimer(pLinkState); */

    /* 初始化完成,向Telnet发送信息完成初试化 */
    pLinkState->bSSHAuthRslt = TRUE;
    SendStartupToTelnet(pLinkState);
}


/**************************************************************************
* 函数名称：VOID Telnet_Msg_Handler
* 功能描述：处理来自Telnet的消息
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_LINK_STATE *pLinkState,TYPE_TELNET_MSGHEAD *ptMsg
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
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

    /************************* 预处理 ***********************************/
    /* 保存消息流水号 */
    pLinkState->wTelSeqNo = ptMsg->wSeqNo;

    /* 添加结束符\0 */
    *(ptMsg->pData + ptMsg->wDataLen) = MARK_STRINGEND;
    ++(ptMsg->wDataLen);

    /* 判断是否为SSH接入(SSH连接的链路号范围:5-7) */
    if (ptMsg->bVtyNum >= 5 && ptMsg->bVtyNum <= 7)
        bSSHMsg = TRUE;

    /********************* 优先处理退出消息 *****************************/
    if (ptMsg->bMsgID == MSGID_TERMINATE)
    {
        if ((strlen(pLinkState->sUserName) > 0) && 
            (pLinkState->tUserLog.tSysClockLogout.wSysYear == 0) &&
            (pLinkState->tUserLog.ucLoginRslt == USER_LOGIN_SUCCESS))
        {
            XOS_GetSysClock(&(pLinkState->tUserLog.tSysClockLogout));
            pLinkState->tUserLog.ucQuitReason = USER_LOGOUT_COMMON;
            /* 写日志文件 */
            if(!UpdateOneUseLog(pLinkState))
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR, "[Telnet_Msg_Handler] UpdateOneUseLog failed!\n");
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

        /*CRDCM00175124 退出时候要停止所有telnet正在执行的命令*/
        Oam_CfgStopExcutingCmd(pLinkState, NULL);
        Oam_InnerCmdLogout(pLinkState);

        return;
    }

    /*611000766546 如果已经存在的连接又发来新建连接请求，
	说明是climanager和telnetserver连接状态不一致了，断开climanager连接，已telnetserver为准*/
    if (ptMsg->bMsgID == MSGID_NEWCONNECT)
    {
        if (Oam_LinkGetCurRunState(pLinkState)  != pec_MsgHandle_IDLE)
        {
            Oam_CfgStopExcutingCmd(pLinkState, NULL);
            
            /* 1.杀掉Telnet端定时器 */
            Oam_KillIdleTimer(pLinkState);
            /* 614000678048 (去掉24小时绝对超时) */
            /*Oam_KillAbsTimer(pLinkState);*/
            /* 2.杀掉Execute端定时器, 不再等待Execute端执行命令返回结果 */
            Oam_KillExeTimer(pLinkState);

            Oam_KillCmdPlanTimer(pLinkState);
            
            /* 3.置为空闲状态 */
            Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_IDLE);
        }
    }

    /*********************** 用户登录阶段 *******************************/
    if (Oam_LinkGetCurRunState(pLinkState) <= pec_MsgHandle_INIT)
    {
        switch(Oam_LinkGetCurRunState(pLinkState))
        {
        case pec_MsgHandle_IDLE:
            if (ptMsg->bMsgID != MSGID_NEWCONNECT)
                break;     

            Oam_LinkInitializeForNewConn(pLinkState);

            /* 保存Telnet端用户标识和IP地址 */
            Oam_LinkSetLinkId(pLinkState, ptMsg->bVtyNum);
			
            if (ptMsg->wDataLen <= LEN_IP_ADDR_STR)
            {
                memcpy(pLinkState->tnIpAddr, ptMsg->pData, ptMsg->wDataLen);
                memset(pLinkState->tCmdLog.szIpAddr, 0, LEN_IP_ADDR_STR);
                memcpy(pLinkState->tCmdLog.szIpAddr, ptMsg->pData, ptMsg->wDataLen);
            }

            /* 保存连接类型 */
            if (1 == ptMsg->bVtyNum)
                pLinkState->tUserLog.ucConnType = OAM_CONN_TYPE_SERIAL;
            else if ((ptMsg->bVtyNum >= 2) && (ptMsg->bVtyNum <= 4))
                pLinkState->tUserLog.ucConnType = OAM_CONN_TYPE_TELNET;
            else
                pLinkState->tUserLog.ucConnType = OAM_CONN_TYPE_SSH;

            pLinkState->tCmdLog.ucConnType = pLinkState->tUserLog.ucConnType;

            if (!bSSHMsg)
            {
                /* 3.发送系统初始化信息及用户名提示符, 提示用户输入用户名 */
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

    /********************* 命令处理匹配阶段 *****************************/
    switch(ptMsg->bMsgID)
    {
    case MSGID_REQUESTDATA: /* 再次请求数据 */

        /* 上次Execute返回结果还没有发送完成则继续发送 */
        if (pLinkState->wSendPos != 0)
        {
            SendPacketToTelnet(pLinkState, FALSE);
            break;
        }

        /* 上次Execute返回结果已经发送完成且数据包结束, 则输出命令行 */
        /* ??如果收到最后一包，会自动显示命令行，这里好像重复了*/
/*
        if (Oam_LinkIfRecvLastPktOfApp(pLinkState))
        {
            pLinkState->bOutputCmdLine = TRUE;
            SendPacketToTelnet(pLinkState, FALSE);
            break;
        }
*/
        /* 判断输出模式是否正确 
        if (pLinkState->bOutputMode != OUTPUT_MODE_REQUEST)
        {
            SendStringToTelnet(pLinkState,Hdr_szTelRequestOutMode);
            Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_TELNET);
            break;
        }
        */
        /*如果应用消息等待再次请求*/
        if (SUCC_CMD_NOT_FINISHED == pLinkState->wAppMsgReturnCode &&
            !pLinkState->bRestartReqSended)
        {
            ExecSendCmdAgain(pLinkState);
        }
        break;

    case MSGID_CEASEDATA: /*终止命令*/
        Oam_CfgStopExcutingCmd(pLinkState, NULL);
        break;

    case MSGID_ASKTELACK: /* 请求错误提示信息 */

        /* 计算错误提示的正确位置 */
        memset(&tExtraInfo,0,sizeof(EXTRAINFO));
        if (ptMsg->wDataLen<= sizeof(EXTRAINFO))
            memcpy(&tExtraInfo, (BYTE *)ptMsg->pData, ptMsg->wDataLen);
        else
            memcpy(&tExtraInfo, (BYTE *)ptMsg->pData, sizeof(EXTRAINFO));
        wDisp_pos = tExtraInfo.disp_pos;
        iPos = CalculateMatchPosition(pLinkState);
        iPos = ((INT32)pMatchResult->wCliMatchLen > iPos) ? pMatchResult->wCliMatchLen : iPos;
        /* 注意wDisp_pos开始显示命令行, 但是前两个字符被".."覆盖,
           也就是实际从wDisp_pos+2开始显示iPos是从1开始编号的, 
           同wDisp_pos从0开始编号不同 */

        if(wDisp_pos != 0)/* 单行文字回滚，需要特殊处理 */
        {
            if(iPos < wDisp_pos + 2)
                iPos = 2;                /*匹配不正确位置被回滚隐藏,提示位置在回滚点处".."*/
            else
                iPos = iPos - wDisp_pos; /*提示位置为当前位置去掉-命令行显示起始处*/
        }
        iPos += strlen(pLinkState->sPrompt);

        /* 显示错误提示信息 */
        SendAskInfoToTelnet(pLinkState,iPos);
        break;

    case MSGID_SENDSTRING:
        /*??下面限制不合理，先去掉，输出模式是命令属性*/
        /* 在Request输出模式的情况下不允许用户sendstring 
        if (pLinkState->bOutputMode == OUTPUT_MODE_REQUEST)
            break;
       */        
         
        pLinkState->bOutputMode     = OUTPUT_MODE_NORMAL;
        /* 处理输入字符串 */
        Process_Input_String(pLinkState,ptMsg);
        break;

    default:
        break;
    }
    return;
}

/**************************************************************************
* 函数名称：INT32 CheckTelMsgHead
* 功能描述：检查Telnet消息头
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_LINK_STATE *pLinkState,TYPE_TELNET_MSGHEAD *ptMsg
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
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
*                          全局函数实现                                   *
**************************************************************************/
/**************************************************************************
* 函数名称：VOID RecvFromTelnet
* 功能描述：接收来自Telnet的消息
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_LINK_STATE *pLinkState,PVOID pMsgPara
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
void RecvFromTelnet(TYPE_LINK_STATE *pLinkState, LPVOID pMsgPara)
{
    TYPE_TELNET_MSGHEAD *ptRcvMsg = (TYPE_TELNET_MSGHEAD *)g_ptOamIntVar->aucRecvBuf;
    WORD16 wMsgLen = 0;
    INT32 iErrCode = MSG_SUCCESS;

    /*~需要处理返回值*/
    XOS_GetMsgDataLen(PROCTYPE_OAM_CLIMANAGER, &wMsgLen);

    /* 1.重置Telnet端空闲超时定时器 */
    Oam_KillIdleTimer(pLinkState);
    Oam_SetIdleTimer(pLinkState);
	
    /* 2.保存消息到本地缓存 */
    if (wMsgLen > MAX_OAMMSG_LEN)
        wMsgLen = MAX_OAMMSG_LEN;
    memcpy(ptRcvMsg, pMsgPara, wMsgLen);

    /*CRDCM00178927 对所有telnet连接发来的消息都检查全局数据是否可用*/
    if (!Oam_CfgGlobalPointersIsOk())
    {
        TYPE_TELNET_MSGHEAD *ptMsg = ptRcvMsg;
         /*防止第一次登录时候出错
         这里要先保存Telnet端用户标识和IP地址 */
        Oam_LinkSetLinkId(pLinkState, ptMsg->bVtyNum);
        /* 如果OAM脚本读取失败直接Logout */
        SendLogoutToTelnet(pLinkState,READ_CMD_SCRIPT_ERR);
        return;
    }

    /* 3.检查消息头 */
    iErrCode = CheckTelMsgHead(pLinkState,ptRcvMsg);
    if (iErrCode !=  MSG_SUCCESS)
    {
        SendTelMsgErrToTelnet(pLinkState,iErrCode);
        return;
    }

    /* 4.处理消息 */
    Telnet_Msg_Handler(pLinkState,ptRcvMsg);
    return;
}

/*检查OAM脚本相关的全局变量是否合法*/
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
* 函数名称：VOID Xml_CfgStopExcutingCmd
* 功能描述：停止当前Telnet连接在执行命令
* 访问的表：无
* 修改的表：无
* 输入参数：pLinkState -- 要停止的连接
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/06/10   V1.0    齐龙兆      创建
**************************************************************************/
void Omm_CfgStopExcutingCmd(TYPE_XMLLINKSTATE *pLinkState)
{
    CHAR *ptrOutputResult = NULL;
    if (!pLinkState)
    {
        return;
    }
    /*当前链路有命令在执行*/
    if (pec_MsgHandle_EXECUTE == XML_LinkGetCurRunState(pLinkState))
    {
        T_OAM_Cfg_Dat* cfgDat = NULL;
        cfgDat = pLinkState->tCurCMDInfo.cfgDat;
        if ((!cfgDat) || (!cfgDat->tCmdAttr))
        {
            return;
        }
        /*需要发送终止命令*/
        if (CMD_ATTR_CANCEASE(&cfgDat->tCmdAttr[pLinkState->tCurCMDInfo.wCmdAttrPos]))
        {
            ExecCeaseOmmCfgCmd(pLinkState);
        }
        else
        {
            ptrOutputResult = ReadMapValueOfOamDat(MAP_TYPE_XMLADAPTER,MAP_KEY_STOP_CMDFORUPDATEDAT);
//            SendErrStringToOmm(pLinkState, ptrOutputResult);                    
            /*杀掉XML链路定时器*/
  //          Xml_KillExeTimer(pLinkState);
	     /*恢复状态为空闲*/
     //       XML_LinkSetCurRunState(pLinkState,pec_MsgHandle_IDLE);
        }
    }        
}


/**************************************************************************
* 函数名称：VOID Oam_CfgStopExcutingCmd
* 功能描述：停止当前Telnet连接在执行命令
* 访问的表：无
* 修改的表：无
* 输入参数：pLinkState -- 要停止的连接
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/06/10   V1.0    殷浩      创建
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
    /*杀掉Execute端定时器*/
    Oam_KillExeTimer(pLinkState);

    /*611001629079 添加版本的时候，切换脚本语言，会中止添加版本命令*/
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


