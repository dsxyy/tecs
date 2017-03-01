/**************************************************************************
* 版权所有 (C)2002, 深圳市中兴通讯股份有限公司。
*
* 文件名称：HrTelMsg.c
* 文件标识：
* 内容摘要：发送消息到Telnet
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
*                          常量                                           *
**************************************************************************/
/**************************************************************************
*                          宏                                             *
**************************************************************************/
#define MAX_SIZE_SAVEACK_BUFFER                      (WORD16)512
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
static void SendSaveAckToClient(TYPE_LINK_STATE *pLinkState,CHAR *pStrOutResult,BOOLEAN isNeedTxtSave,BOOLEAN bOR);
static void SendSaveAckToOmm(CHAR *sOutputResult,BOOLEAN isNeedTxtSave,BOOLEAN bOR);
/**************************************************************************
*                          局部函数实现                                   *
**************************************************************************/

static BOOLEAN char_is_chinese(BYTE ucIn)
{
	return ((ucIn & 0x80) == 0x80);
}

/**************************************************************************
* 函数名称：VOID PermitChangeMode
* 功能描述：验证是否可以改变当前模式
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_LINK_STATE *pLinkState
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
static BOOLEAN PermitChangeMode(TYPE_LINK_STATE *pLinkState)
{
/*离线转在线任务去除，如果要放开，要注意离线转在线用的默认linkstate中用户权限问题*/
#if 0
    T_OAM_Cfg_Dat * cfgDat = pLinkState->tMatchResult.cfgDat;
    BYTE bNextModeID = cfgDat->tCmdAttr[pLinkState->wCmdAttrPos].bNextModeID;
    WORD16 wModeClass = 0;
    DWORD i;
    BOOLEAN bRet = FALSE;

    /*检索下一模式是否存在*/
    for (i=0; i<=g_dwModeCount; i++)
    {
        if (gtMode[i].bModeID == bNextModeID)
            break;
    }
    if (i > g_dwModeCount)
        return bRet;

    /*验证权限*/
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
* 函数名称：VOID InnerCmdLogin
* 功能描述：登录
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
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
* 函数名称：VOID Oam_InnerCmdLogout
* 功能描述：退出
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
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

    return;
}

/**************************************************************************
* 函数名称：VOID InnerCmdEnable
* 功能描述：进入特权模式
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
static void InnerCmdEnable(TYPE_LINK_STATE * pLinkState)
{
    WORD16 wIdx = 0;
    /*判断是否已经有其他用户在特权模式*/
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
        /* 判断是否有权限切换到特权模式 */
        OAM_ChangeMode(pLinkState,NEXTMODE_NORMAL);
        if (pLinkState->bChangeMode)
        {
            /* 直接进入 */
            /* 备份当前用户权限 */
            pLinkState->ucSavedRight = pLinkState->ucUserRight;
            SendPacketToTelnet(pLinkState, FALSE);
        }
        else
        {
            /* 需要输入超级用户密码 */
            SendPasswordPromptToTelnet(pLinkState,NULL);
        }
    }
    Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_TELNET);

    return;
}

/**************************************************************************
* 函数名称：VOID InnerCmdDisable
* 功能描述：退出特权模式
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
static void InnerCmdDisable(TYPE_LINK_STATE * pLinkState)
{
    /* 改变模式 */
    OAM_ChangeMode(pLinkState,NEXTMODE_NORMAL);

    SendPacketToTelnet(pLinkState, FALSE);

    Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_TELNET);

    /* 恢复权限 */
    pLinkState->ucUserRight = pLinkState->ucSavedRight;

    pLinkState->bIsSupperUsrMode = FALSE;

    return;
}

#if 0
/**************************************************************************
* 函数名称：VOID InnerCmdConfigure
* 功能描述：配置模式
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
static VOID InnerCmdConfigure(TYPE_LINK_STATE * pLinkState)
{
    WORD16 ConfigUser = 0;
    BYTE i = 0;
    CHAR TempStr[MAX_STRING_LEN] = {0};

    /*遍历每个终端,检查有几个用户在配置模式下*/
    for (i = MIN_CLI_VTY_NUMBER; i <= MAX_CLI_VTY_NUMBER; i++)
    {
        BYTE k = i - MIN_CLI_VTY_NUMBER;
        if (Oam_LinkGetCurRunState(&gtLinkState[k]) != pec_MsgHandle_IDLE &&
            Oam_GetCurModeId(&gtLinkState[k]) > CMD_MODE_PRIV)
        {
            ConfigUser++;
        }
    }

    if ( (ConfigUser == 0)  || /*单用户配置方式, 且当前无用户在配置模式下*/
        Oam_LinkGetLinkId(pLinkState) == MIN_CLI_VTY_NUMBER) /*Console串口可以直接进入配置模式，不考虑Flag标志*/
    {
        OAM_ChangeMode(pLinkState,NEXTMODE_NORMAL);
        if (pLinkState->bChangeMode == TRUE)
        {
            /*在终端上打印告警提示*/
            pLinkState->bOutputResult = TRUE;
            strncpy(pLinkState->sOutputResult,Hdr_szEnterConfigTip,MAX_OUTPUT_RESULT);

            strncat(pLinkState->sOutputResult,"\n",MAX_OUTPUT_RESULT);
        }
    }
    else /*不允许进入配置模式，打印提示信息*/
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
* 函数名称：VOID InnerCmdExit
* 功能描述：退出模式
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
static void InnerCmdExit(TYPE_LINK_STATE * pLinkState)
{
    OAM_ChangeMode(pLinkState,NEXTMODE_NORMAL);
    
    /*CRDCM00096832 退出模式命令不应该把应用自己处理的字段清空 */
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
        //字符串类型按照实际长度拷贝
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
* 函数名称：VOID ConstructShowModeAckMsgParam
* 功能描述：构建查询模式栈应答消息参数
* 访问的表：无
* 修改的表：无
* 输入参数：BYTE bModeStackTop:模式栈顶指针
                             BYTE *paucModeId:模式栈
* 输出参数：MSG_COMM_OAM *ptRtnMsg:OAM消息体
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/01/07    V1.0    齐龙兆      创建
**************************************************************************/
void ConstructShowModeAckMsgParam(BYTE bModeStackTop,BYTE *paucModeId,MSG_COMM_OAM *ptRtnMsg)
{
    BYTE bFlag = 0;/*控制标记*/
    BYTE iLoop = 0; 
    BYTE *piLoopCnt = NULL; /*循环返回的字符串数目*/
    BYTE bModeId = 0;/*模式ID*/
    DWORD dwModeLoop = 0;/*模式数组下标*/
    BYTE aucOutResult[MAX_STRING_LEN] = {0};

    /*参数检查*/
    if ((NULL == paucModeId) || (NULL == ptRtnMsg))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return;
    }
    /*链路状态为空*/    
    if ((0 == bModeStackTop) || (1 == bModeStackTop))
    {
        bFlag = 1;
        AddParaToMsg(ptRtnMsg , DATA_TYPE_BYTE,&bFlag, (BYTE)1);
    }
    /*打印回显脚本*/
    else
    {
        bFlag = 2;
        AddParaToMsg(ptRtnMsg , DATA_TYPE_BYTE,&bFlag, (BYTE)1);

        /*先添加一个表示循环次数的回显参数*/    
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
* 函数名称：VOID InnerCmdProcessForXmlAgent
* 功能描述：解释命令处理(XML适配器专用)
* 访问的表：无
* 修改的表：无
* 输入参数：MSG_COMM_OAM *ptOamMsg:OAM配置请求
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/01/07    V1.0    齐龙兆      创建
**************************************************************************/
static void InnerCmdProcessForXmlAgent(MSG_COMM_OAM *ptOamMsg)
{    
    JID tDtJid;                                               /*目标JID*/
    MSG_COMM_OAM *ptRtnMsg = (MSG_COMM_OAM *)g_ptOamIntVar->aucRecvBuf;/*返回结果*/
    /*参数检查*/
    if (NULL == ptOamMsg)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return;
    }
    /*初始化变量*/
    memset(&tDtJid,0,sizeof(JID));
    memcpy(ptRtnMsg,ptOamMsg,ptOamMsg->DataLen);
    ptRtnMsg->Number = 0;
    ptRtnMsg->DataLen  = sizeof(MSG_COMM_OAM);
    ptRtnMsg->OutputMode = OUTPUT_MODE_NORMAL;
    ptRtnMsg->ReturnCode = SUCC_AND_NOPARA;
    /*获取自身JID*/
    if (XOS_SUCCESS != XOS_GetSelfJID(&tDtJid))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XOS_GetSelfJID Error! ....................",__func__,__LINE__);
        return;
    }
    /*向OAM返回结果*/
    XOS_SendAsynMsg(EV_OAM_CFG_CFG, (BYTE *)ptRtnMsg, ptRtnMsg->DataLen, XOS_MSG_VER0, XOS_MSG_MEDIUM, &tDtJid);
    XOS_SysLog(OAM_CFG_LOG_NOTICE,
                       "....................FUNC:%s LINE:%d SendInnerCmdAck Successfully! ....................",__func__,__LINE__);
    return;
}

/**************************************************************************
* 函数名称：VOID InnerCmdSetBanner
* 功能描述：设置Banner
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
static void InnerCmdSetBanner(TYPE_LINK_STATE *pLinkState)
{
    TYPE_MATCH_RESULT * pMatchResult = &(pLinkState->tMatchResult);

    if (pMatchResult->bHasNo == FALSE)
    {
        /* 修改为Text输入提示符 */
        SendTextPromptToTelnet(pLinkState);
    }
    else
    {
        /* 保存sBanner */
        strncpy(gtCommonInfo.sBanner, DEFAULT_BANNER, sizeof(gtCommonInfo.sBanner));
        gtCommonInfo.sBanner[MAX_BANNER_LEN - 1] = MARK_STRINGEND;

        SendPacketToTelnet(pLinkState, FALSE);
    }

    return;
}

/**************************************************************************
* 函数名称：VOID InnerCmdSetPrompt
* 功能描述：设置Prompt
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
static void InnerCmdSetPrompt(TYPE_LINK_STATE *pLinkState)
{
    TYPE_MATCH_RESULT * pMatchResult = &(pLinkState->tMatchResult);

    /* 保存sPrompt */
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
*                          全局函数实现                                   *
**************************************************************************/
/**************************************************************************
* 函数名称：VOID ChangePrompt
* 功能描述：改变提示符
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
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
        /* 如果提示符长度超过32,则显示为: RPU...(config)#
           提醒: wOddPromptLen 应尽量简短 */
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
* 函数名称：VOID OAM_ChangeMode
* 功能描述：改变当前模式
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_LINK_STATE *pLinkState,BYTE bNextMode
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
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

    /*当前为no命令时不处理*/
    if (Oam_LinkIfCurCmdIsNo(pLinkState))
        return;

    if (bModeId == NEXTMODE_NORMAL)
    {
        bNextModeID = cfgDat->tCmdAttr[pLinkState->wCmdAttrPos].bNextModeID;

        /*当前模式和下一模式相同时不处理*/
        if ((bNextModeID == CMD_MODE_SAME) ||
                (bNextModeID == Oam_GetCurModeId(pLinkState)))
            return;

        /*下一模式为前一模式则不用验证权限*/
        if (bNextModeID == CMD_MODE_FORWARD)
            bPermitChangeMode = TRUE;
        else
             /*验证权限*/
            bPermitChangeMode = PermitChangeMode(pLinkState);
    }
    else
    {
        /*下一模式为特定模式则不用验证权限*/
        bNextModeID = bModeId;
        bPermitChangeMode = TRUE;
    }

    /*无权限不能切换到下一模式*/
    if (bPermitChangeMode == FALSE)
    {
        if (bNextModeID != CMD_MODE_PRIV)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR,
                 "OAM_ChangeMode: can not change to next mode !\n");
            SendStringToTelnet(pLinkState,Hdr_szCanNotChangMode);  /*返回错误信息*/
        }
        return;
    }

    if (bNextModeID != CMD_MODE_FORWARD)  /*下一模式非前一模式*/
    {
        if (bNextModeID > Oam_GetCurModeId(pLinkState))
        {
            Oam_LinkModeStack_Push(pLinkState, bNextModeID);
        }
        else
        {
            /*后退到指定模式*/
            do
            {
                 Oam_LinkModeStack_Pop(pLinkState);
            }while (!Oam_LinkModeStatck_IsEmpty(pLinkState) && bNextModeID < Oam_GetCurModeId(pLinkState));
        }
    }
    else /*下一模式为前一模式*/
    {
        /*后退到前一模式*/
        Oam_LinkModeStack_Pop(pLinkState);
    }

    if (Oam_LinkGetInputState(pLinkState) == INMODE_NORMAL)
        ChangePrompt(pLinkState,Oam_GetCurModeId(pLinkState));

    pLinkState->bChangeMode = TRUE;

    return;

}

/**************************************************************************
* 函数名称：VOID SendLogoutToTelnet
* 功能描述：退出
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_LINK_STATE *pLinkState,CHAR *sOutputResult
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
void SendLogoutToTelnet(TYPE_LINK_STATE *pLinkState,CHAR *sOutputResult)
{
    if (!pLinkState)
    {
        return;
    }

    /* 1.杀掉Telnet端定时器 */
    Oam_KillIdleTimer(pLinkState);
    /* 614000678048 (去掉24小时绝对超时) */
    /*Oam_KillAbsTimer(pLinkState);*/

    /* 2.杀掉Execute端定时器, 不再等待Execute端执行命令返回结果 */
    Oam_KillExeTimer(pLinkState);

    Oam_KillCmdPlanTimer(pLinkState);

    /* 3.发送退出消息 */
    Oam_LinkMarkLogout(pLinkState, TRUE);

    SendStringToTelnet(pLinkState,sOutputResult);

    /* 4.置为空闲状态 */
    Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_IDLE);

    return;
}

/**************************************************************************
* 函数名称：VOID SendExeMsgErrToTelnet
* 功能描述：返回Execute消息头错误信息
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_LINK_STATE *pLinkState,INT32 iErrCode
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
void SendExeMsgErrToTelnet(TYPE_LINK_STATE *pLinkState,INT32 iErrCode)
{
    CHAR strErrInfo[MAX_STRING_LEN];

    /* 1.根据错误码生成错误信息 */
    /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
    XOS_snprintf(strErrInfo,
                 sizeof(strErrInfo) -1,
                 "%s",
                 Err_szExeMsgHead);

    switch(iErrCode)
    {
    case MSGERR_RETURNCODE:
        /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
        XOS_snprintf(strErrInfo,
                     sizeof(strErrInfo) -1,
                     "%s%s",
                     strErrInfo,
                     Err_szReturnCode);
        break;
    case MSGERR_LINKCHANNEL:
        /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
        XOS_snprintf(strErrInfo,
                     sizeof(strErrInfo) -1,
                     "%s%s",
                     strErrInfo,
                     Err_szLinkChannel);
        break;
    case MSGERR_SEQNO:
        /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
        XOS_snprintf(strErrInfo,
                     sizeof(strErrInfo) -1,
                     "%s%s",
                     strErrInfo,
                     Err_szSeqNo);
        break;
    case MSGERR_LASTPACKET:
        /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
        XOS_snprintf(strErrInfo,
                     sizeof(strErrInfo) -1,
                     "%s%s",
                     strErrInfo,
                     Err_szLastPacket);
        break;
    case MSGERR_OUTPUTMODE:
        /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
        XOS_snprintf(strErrInfo,
                     sizeof(strErrInfo) -1,
                     "%s%s",
                     strErrInfo,
                     Err_szOutputMode);
        break;
    case MSGERR_CMDID:
        /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
        XOS_snprintf(strErrInfo,
                     sizeof(strErrInfo) -1,
                     "%s%s",
                     strErrInfo,
                     Err_szCmdID);
        break;
    case MSGERR_NUMBER:
        /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
        XOS_snprintf(strErrInfo,
                     sizeof(strErrInfo) -1,
                     "%s%s",
                     strErrInfo,
                     Err_szNumber);
        break;
    case MSGERR_EXECMODEID:
        /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
        XOS_snprintf(strErrInfo,
                     sizeof(strErrInfo) -1,
                     "%s%s",
                     strErrInfo,
                     Err_szExecModeID);
        break;
    case MSGERR_IFNO:
        /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
        XOS_snprintf(strErrInfo,
                     sizeof(strErrInfo) -1,
                     "%s%s",
                     strErrInfo,
                     Err_szIfNo);
        break;
    case MSGERR_FROMINTPR:
        /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
        XOS_snprintf(strErrInfo,
                     sizeof(strErrInfo) -1,
                     "%s%s",
                     strErrInfo,
                     Err_szFromIntpr);
        break;
    case MSGERR_NONEEDTHECMD:
        /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
        XOS_snprintf(strErrInfo,
                     sizeof(strErrInfo) -1,
                     "%s%s",
                     strErrInfo,
                     Err_szNoNeedTheCmd);
        break;
    default:
        /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
        XOS_snprintf(strErrInfo,
                     sizeof(strErrInfo) -1,
                     "%s%s",
                     strErrInfo,
                     Err_szUndefined);
        break;
    }

    /* 2.向Telnet发送信息 */
    SendStringToTelnet(pLinkState,strErrInfo);

    /* 3.后续处理 */
    Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_TELNET);
    if (Oam_LinkGetCurRunState(pLinkState) == pec_MsgHandle_EXECUTE)
    {
        Oam_SetExeTimer(pLinkState);
    }
}

/**************************************************************************
* 函数名称：VOID SendTelMsgErrToTelnet
* 功能描述：返回Telnet消息头错误信息
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_LINK_STATE *pLinkState,INT32 iErrCode
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
void SendTelMsgErrToTelnet(TYPE_LINK_STATE *pLinkState,INT32 iErrCode)
{
    CHAR strErrInfo[MAX_STRING_LEN];

    /* 1.根据错误码生成错误信息 */
    /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
    XOS_snprintf(strErrInfo,
                 sizeof(strErrInfo) -1,
                 "%s",
                 Err_szTelMsgHead);
    switch(iErrCode)
    {
    case MSGERR_MSGID:
        /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
        XOS_snprintf(strErrInfo,
                     sizeof(strErrInfo) -1,
                     "%s%s",
                     strErrInfo,
                     Err_szMsgId);
        break;
    case MSGERR_VTYNUM:
        /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
        XOS_snprintf(strErrInfo,
                     sizeof(strErrInfo) -1,
                     "%s%s",
                     strErrInfo,
                     Err_szVtyNum);
        break;
    case MSGERR_DETERMINER:
        /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
        XOS_snprintf(strErrInfo,
                     sizeof(strErrInfo) -1,
                     "%s%s",
                     strErrInfo,
                     Err_szDeterminer);
        break;
    default:
        /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
        XOS_snprintf(strErrInfo,
                     sizeof(strErrInfo) -1,
                     "%s%s",
                     strErrInfo,
                     Err_szUndefined);
        break;
    }

    /* 2.向Telnet发送信息 */
    SendStringToTelnet(pLinkState,strErrInfo);

    Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_TELNET);

    return;
}

/**************************************************************************
* 函数名称：VOID SendUserNameErrToTelnet
* 功能描述：用户名错误
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_LINK_STATE *pLinkState,CHAR *sOutputResult
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
void SendUserNameErrToTelnet(TYPE_LINK_STATE *pLinkState,CHAR *sOutputResult)
{
    SendNormalPromptToTelnet(pLinkState,sOutputResult);

 //   pLinkState->ucInputErrTimes = 0;

    Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_TELNET);

    return;
}

/**************************************************************************
* 函数名称：VOID SendPasswordErrToTelnet
* 功能描述：密码错误
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_LINK_STATE *pLinkState,CHAR *sOutputResult
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
void SendPasswordErrToTelnet(TYPE_LINK_STATE *pLinkState,CHAR *sOutputResult)
{
    T_OAM_Cfg_Dat * cfgDat = pLinkState->tMatchResult.cfgDat;
    switch(cfgDat->tCmdAttr[pLinkState->wCmdAttrPos].dExeID)
    {
#if 0        
    case CMD_INTERPT_LOGIN:
        /* 恢复用户名和权限 */
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
* 函数名称：VOID SendSysInitInfoToTelnet
* 功能描述：返回系统初始化信息及用户名提示符
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_LINK_STATE *pLinkState
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
void SendSysInitInfoToTelnet(TYPE_LINK_STATE *pLinkState)
{
    SendUserNamePromptToTelnet(pLinkState, gtCommonInfo.sBanner);

    pLinkState->ucInputErrTimes = 0;

    Oam_LinkSetCurRunState(pLinkState, pec_Init_USERNAME);

    return;
}

/**************************************************************************
* 函数名称：VOID SendStartupToTelnet
* 功能描述：返回启动信息
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_LINK_STATE *pLinkState
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
void SendStartupToTelnet(TYPE_LINK_STATE *pLinkState)
{
    /* 初始化为用户模式 */
    Oam_LinkModeStack_Init(pLinkState);
    Oam_LinkModeStack_Push(pLinkState, CMD_MODE_PRIV/*CMD_MODE_USER*/);

    SendNormalPromptToTelnet(pLinkState,NULL);

    pLinkState->ucInputErrTimes = 0;

    Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_TELNET);

    return;
}

/**************************************************************************
* 函数名称：VOID SendUserNamePromptToTelnet
* 功能描述：返回用户提示符
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_LINK_STATE *pLinkState, CHAR *sOutputResult
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
void SendUserNamePromptToTelnet(TYPE_LINK_STATE *pLinkState, CHAR *sOutputResult)
{
    /* 改变提示符 */
    pLinkState->bChangeMode     = TRUE;
    /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
    XOS_snprintf(pLinkState->sPrompt,
                 sizeof(pLinkState->sPrompt) -1,
                 "%s",
                 Hdr_szUserName);

    /* 改变输入模式 */
    Oam_LinkSetInputState(pLinkState, INMODE_USERNAME);

    SendStringToTelnet(pLinkState,sOutputResult);

//    pLinkState->ucInputErrTimes = 0;

    return;
}

/**************************************************************************
* 函数名称：VOID SendPasswordPromptToTelnet
* 功能描述：返回密码提示符
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_LINK_STATE *pLinkState, CHAR *sOutputResult
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
void SendPasswordPromptToTelnet(TYPE_LINK_STATE *pLinkState, CHAR *sOutputResult)
{
    /* 改变提示符 */
    pLinkState->bChangeMode     = TRUE;
    /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
    XOS_snprintf(pLinkState->sPrompt,
                 sizeof(pLinkState->sPrompt) -1,
                 "%s",
                 Hdr_szPassword);
    /*OSS_Printf(PRINT_OAM_PLATCFGAGT, PRN_NORMAL, PRN_LEVEL_NORMAL,
                  "pLinkState->sPrompt=%s\n",Hdr_szPassword);*/

    /* 改变输入模式 */
    Oam_LinkSetInputState(pLinkState, INMODE_PASSWORD);

    pLinkState->bOutputCmdLine = TRUE;
    SendStringToTelnet(pLinkState,sOutputResult);
    /*OSS_Printf(PRINT_OAM_PLATCFGAGT, PRN_NORMAL, PRN_LEVEL_NORMAL,
                  "SendPasswordPromptToTelnet:sOutputResult=%s\n",sOutputResult);*/


//    pLinkState->ucInputErrTimes = 0;

    return;
}

/**************************************************************************
* 函数名称：VOID SendNormalPromptToTelnet
* 功能描述：返回Normal提示符
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
void SendNormalPromptToTelnet(TYPE_LINK_STATE *pLinkState, CHAR *sOutputResult)
{
    /* 改变提示符 */
    pLinkState->bChangeMode   = TRUE;
    ChangePrompt(pLinkState,Oam_GetCurModeId(pLinkState));

    /* 改变输入模式 */
    Oam_LinkSetInputState(pLinkState, INMODE_NORMAL);

    pLinkState->bOutputCmdLine = TRUE;
    SendStringToTelnet(pLinkState,sOutputResult);

    Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_TELNET);

    return;

}

/**************************************************************************
* 函数名称：VOID SendTextPromptToTelnet
* 功能描述：返回Text提示符
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_LINK_STATE * pLinkState
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
void SendTextPromptToTelnet(TYPE_LINK_STATE * pLinkState)
{
    TYPE_MATCH_RESULT *pMatchResult = &(pLinkState->tMatchResult);
    TYPE_TEXT_INPUT *pTextInput = &(pLinkState->tInputControl);

    /* 保存TextInput的EndMark字符 */
    memset(pTextInput, 0, sizeof(TYPE_TEXT_INPUT));
    pTextInput->cEndMark = *(pMatchResult->tPara[0].pValue);

    /* 输出提示字符串 */
    pLinkState->bOutputResult = TRUE;
    XOS_snprintf(pLinkState->sOutputResult, MAX_OUTPUT_RESULT, Hdr_szTelInputText, pTextInput->cEndMark);
    XOS_snprintf(pLinkState->sOutputResult, MAX_OUTPUT_RESULT - 1, "%s\n", pLinkState->sOutputResult);

    /* 改变提示符 */
    pLinkState->bChangeMode     = TRUE;
    strncpy(pLinkState->sPrompt, ">", sizeof(pLinkState->sPrompt));

    /* 改变输入模式 */
    Oam_LinkSetInputState(pLinkState, INMODE_TEXT);

    SendPacketToTelnet(pLinkState, FALSE);

    Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_TELNET);

    return;
}

/**************************************************************************
* 函数名称：VOID SendCeaseToTelnet
* 功能描述：取消命令
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_LINK_STATE * pLinkState
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
void SendCeaseToTelnet(TYPE_LINK_STATE *pLinkState)
{
    CHAR aucTmpCeaseTip[100] = {0};
    /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
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
* 函数名称：VOID SendAskInfoToTelnet
* 功能描述：返回AskInfo
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_LINK_STATE *pLinkState,INT32 iPos
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
void SendAskInfoToTelnet(TYPE_LINK_STATE *pLinkState,INT32 iPos)
{
    /* 输出提示字符串 */
    pLinkState->bOutputResult   = TRUE;
    memset(pLinkState->sOutputResult, MARK_BLANK, iPos - 1);
    pLinkState->sOutputResult[iPos - 1] = MARK_POS;
    pLinkState->sOutputResult[iPos]     = 0;
    /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
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
* 函数名称：VOID SendStringToTelnet
* 功能描述：返回字符串
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_LINK_STATE *pLinkState,CHAR *sOutputResult
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
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
    /* 如果有输出提示字符串则输出 */
    if (sOutputResult)
    {
        pLinkState->bOutputResult = TRUE;
        /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
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
* 函数名称：VOID SendErrStringToClient
* 功能描述：返回错误消息到客户端
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_LINK_STATE *pLinkState,CHAR *pStrOutResult
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/04/7    V1.0    齐龙兆     创建
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
* 函数名称：VOID SendZDBSaveAckToClient
* 功能描述：发送ZDB存盘应答
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_LINK_STATE *pLinkState
                              WORD32 dwMsgId
                              CHAR *pStrOutResult
                              BOOLEAN isNeedTxtSave
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/04/12    V1.0    齐龙兆      创建
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
* 函数名称：VOID SendTxtSaveAckToClient
* 功能描述：返回TXT存盘消息到客户端
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_LINK_STATE *pLinkState
                             CHAR *pStrOutResult
                             BOOLEAN bOR
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/04/7    V1.0    齐龙兆     创建
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
* 函数名称：VOID SendSaveAckToClient
* 功能描述：返回存盘消息到客户端
* 访问的表：无
* 修改的表：无
* 输入参数：CHAR *sOutputResult
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/04/7    V1.0    齐龙兆     创建
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
    if (pSaveLinkState == pLinkState)/*向网管发送超时应答*/
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
* 函数名称：VOID SendSaveAckToOmm
* 功能描述：返回存盘消息到网管
* 访问的表：无
* 修改的表：无
* 输入参数：CHAR *sOutputResult
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/04/7    V1.0    齐龙兆     创建
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
    /*查找正在存盘的网管端*/
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
	 if (g_sbSaveOR)/*保存当前存盘操作结果*/
	 {
	     g_sbSaveOR = bOR;
	 }
        if (!isNeedTxtSave)/*不需要TXT存盘*/
        {
            SendStringToOmm(ptXmlLinkState, g_ssSaveAckBuf,g_sbSaveOR,TRUE);
            memset(g_ssSaveAckBuf,0,MAX_SIZE_SAVEACK_BUFFER);
	     g_sbSaveOR = TRUE;
            XML_LinkSetCurRunState(ptXmlLinkState, pec_MsgHandle_IDLE);
            /*杀掉XML链路定时器*/
	     Xml_KillExeTimer(ptXmlLinkState);
        }
        else/*重置定时器*/
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
* 函数名称：VOID SendPacketToTelnet
* 功能描述：发送消息到Telnet
* 访问的表：无
* 修改的表：无
* 输入参数：
  pLinkState --要输出的连接
  bSendResultOnly -- 是否只输出结果，不发送其他消息
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
void SendPacketToTelnet(TYPE_LINK_STATE *pLinkState, BOOLEAN bSendResultOnly)
{
    WORD16 wPacketSize = sizeof(TYPE_INTERPRET_MSGHEAD);
    BYTE *pPacket = g_ptOamIntVar->aucSendBuf;
    TYPE_INTERPRET_MSGHEAD *tMsgHead = (TYPE_INTERPRET_MSGHEAD *)pPacket;
    BYTE bSendCmdLine = 0;
    WORD16 wTotalOutputLen = strlen(pLinkState->sOutputResult);
    BOOLEAN  bLastPacketOfApp = TRUE; /*是否收到应用最后一包*/
    BOOLEAN  bNeedSendCtrlMsg = FALSE; /*是否需要发送非显示数据的控制消息*/
	
    /* 组包 */
	
    /*telnet和cli之间是否最后一包数据，后面会根据是否有分包修改*/
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
    
        /*改变输入模式为异步还是同步 yinhao*/
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
      
        /*Ask Telnet状态*/
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

    /* 发送 */
    if (bNeedSendCtrlMsg)
    {
        XOS_SendAsynMsg(OAM_MSG_INTERPRET_TO_TELNET, (BYTE*)tMsgHead, (WORD16)wPacketSize,
                        XOS_MSG_VER0, XOS_MSG_MEDIUM,&g_ptOamIntVar->jidTelnet);
    }

    /* 复位 */
    Oam_LinkMarkLogout(pLinkState, FALSE);
    pLinkState->bRecvAsynMsg = FALSE;
    if (pLinkState->wSendPos >= wTotalOutputLen)
    {
        pLinkState->bOutputResult    = FALSE;
        *(pLinkState->sOutputResult) = MARK_STRINGEND;
        pLinkState->wSendPos         = 0;

        if (bSendCmdLine)
        {
            pLinkState->bOutputCmdLine = TRUE; /*默认如果是最后一包，都显示cmdline；需要时候才通过这个变量为false，不显示cmdline*/
            *(pLinkState->sCmdLine)    = MARK_STRINGEND;
        }
    }

    return;
}

/**************************************************************************
* 函数名称：VOID Oam_Cfg_SendKeepAliveMsgToTelnet(TYPE_LINK_STATE *pLinkState)
* 功能描述：收到应用保活消息后,发送保活消息给telnetserver
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_LINK_STATE *pLinkState
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2010/05/27    V1.0      殷浩       创建
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

	
    /* 发送 */
    XOS_SendAsynMsg(OAM_MSG_INTERPRET_TO_TELNET, (BYTE*)tMsgHead, (WORD16)wPacketSize,
                        XOS_MSG_VER0, XOS_MSG_MEDIUM,&g_ptOamIntVar->jidTelnet);

}

/**************************************************************************
* 函数名称：VOID DisposeInnerCmdForXmlAgent
* 功能描述：为XML适配器处理Inner内部命令
* 访问的表：无
* 修改的表：无
* 输入参数：MSG_COMM_OAM *ptOamMsg
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/01/07    V1.0    齐龙兆      创建
**************************************************************************/
void DisposeInnerCmdForXmlAgent(MSG_COMM_OAM *ptOamMsg)
{
    MSG_COMM_OAM *ptOamMsgTmp = NULL;
    MSG_COMM_OAM *ptRtnMsg = (MSG_COMM_OAM *)g_ptOamIntVar->aucRecvBuf;/*模式栈查询结果*/	
    DWORD dwExeID = 0;
    DWORD dwMsgId = 0;		
    /*参数检查*/
    if (NULL == ptOamMsg)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return;
    }    
    /*初始化变量*/
    ptOamMsgTmp = ptOamMsg;
    dwExeID = ptOamMsgTmp->CmdID;
    switch(dwExeID)
    {
	 case CMD_LDB_GET_MODESTATCK:/*查询模式栈*/
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
* 函数名称：VOID DisposeInterpretPosCmd
* 功能描述：处理Inner内部命令
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_LINK_STATE *pLinkState
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
void DisposeInterpretPosCmd(TYPE_LINK_STATE *pLinkState)
{
    TYPE_MATCH_RESULT *pMatchResult = &(pLinkState->tMatchResult);
    T_OAM_Cfg_Dat * cfgDat = pMatchResult->cfgDat;
    DWORD dwExeID = cfgDat->tCmdAttr[pMatchResult->wCmdAttrPos].dExeID;
    switch(dwExeID)
    {
    case CMD_TELNET_LOGOUT:           /*退出*/
        Oam_InnerCmdLogout(pLinkState);
        break;
#if 0        
    case CMD_INTERPT_LOGIN:           /*登录命令*/
        InnerCmdLogin(pLinkState);
        break;
#endif        
    case CMD_INTERPT_ENABLE:          /*进入特权模式命令*/
        InnerCmdEnable(pLinkState);
        break;
    case CMD_INTERPT_DISABLE:         /*退出特权模式命令*/
        InnerCmdDisable(pLinkState);
        break;
#if 0
    case CMD_INTERPT_CONFIG_TERMINAL: /*进入配置模式*/
        InnerCmdConfigure(pLinkState);
        break;
#endif
    case CMD_INTERPT_EXIT:            /*退出模式*/
        InnerCmdExit(pLinkState);
        break;
    case CMD_LDB_SET_BANNER:          /*设置Banner命令*/
        InnerCmdSetBanner(pLinkState);
        break;
    case CMD_LDB_SET_HOSTNAME:        /*设置HostName命令*/
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
            /* 默认的认为是业务添加的进入自己模式的命令 */
            OAM_ChangeMode(pLinkState,NEXTMODE_NORMAL);
            Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_TELNET);
        SendPacketToTelnet(pLinkState, FALSE);
        }
        break;
    }
	
    return;
}
/**************************************************************************
* 函数名称：VOID DisposeInnerCmd
* 功能描述：处理Inner内部命令
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_LINK_STATE *pLinkState
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
void DisposeInnerCmd(TYPE_LINK_STATE *pLinkState)
{
    MSG_COMM_OAM *ptMsg = OamCfgConstructMsgData(pLinkState, NULL);
    MSG_COMM_OAM *ptRtnMsg = (MSG_COMM_OAM *)g_ptOamIntVar->aucRecvBuf;
    CHAR *ptrOutputResult = NULL;	
    WORD32 dwMsgId=0;
    int iLinkStateType = 0;	
    #if 0	
    /*who单独处理，函数库不对其处理*/	

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
            if (!Xml_CfgCurLinkIsExcutingSave(pLinkState))/*当前不是网管在存盘*/
            {
                /*检查是否有链路正在存盘*/
                if (CheckLinkIsExecutingSaveCmd() )
                {
                    SendStringToTelnet(pLinkState, Hdr_szSaveMutexTip);
                    return;
                }
                /*如果是存盘命令检查其他链路是否在执行命令*/
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

            /*重置Execute端定时器*/
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
            /*提示用户有其他用户正在执行命令，稍后再试*/
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
* 函数名称：VOID Oam_CfgCloseAllLink(CHAR *pcReason)
* 功能描述：断开所有的telnet连接
* 访问的表：无
* 修改的表：无
* 输入参数：CHAR *pcReason - 断开原因，可以为空
* 输出参数：
* 返 回 值：参见头文件定义
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2008/04/08   V1.0    殷浩      创建
**************************************************************************/
void Oam_CfgCloseAllLink(CHAR *pcReason)
{
    int i = 0;
    CHAR aucOutTmp[200] = {0};

    if(pcReason)
    {
        /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
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
    /*将XML适配器与网管断链*/
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
    BOOLEAN  bLastPacketOfApp = TRUE; /*是否收到应用最后一包*/
    WORD16 wHeadSize = sizeof(WORD16) + sizeof(BYTE);

    /*telnet和cli之间是否最后一包数据，后面会根据是否有分包修改*/
    bLastPacketOfApp  =  Oam_LinkIfRecvLastPktOfApp(pLinkState) ? TRUE : FALSE;
    tMsgHead->bLastPacket  =  bLastPacketOfApp;

    XOS_SysLog(OAM_CFG_LOG_DEBUG, "Oam_SendOutputToTelnet:: pLinkState->sOutputResult length=%u \n", wTotalOutputLen);
    /*循环发完一次接收到的应用数据*/
    while (bOutputNotFinish)
    {
        if (*(pLinkState->sOutputResult) != MARK_STRINGEND)
        {
            /*计算还有多少字符没有发送*/
            wStrLen = wTotalOutputLen - pLinkState->wSendPos;

            /*?? 如果需要显示的内容大于最大包长度，
            除了要分包，后面其他消息是否需要继续发送? */
            if ((wStrLen + wPacketSize) > MAX_OAMMSG_LEN)
            {
                BYTE ucTmp = 0;
                wStrLen = 0;
                while (wStrLen < (MAX_OAMMSG_LEN - wPacketSize))
                {
                    ucTmp = *(pLinkState->sOutputResult + pLinkState->wSendPos + wStrLen);
                    wStrLen++; 
                    /*如果是中文，连续读两个字节*/
                    if (char_is_chinese(ucTmp))
                    {
                        if (wStrLen == (MAX_OAMMSG_LEN - wPacketSize))
                        {/*如果读取字符数已经达到最大长度，退回已读的半个中文字符*/
                            wStrLen--;
                            break;
                        }
                        else
                        {/*读取另一半中文字符*/
                            wStrLen++;    	
                        }
                    }
                }

                tMsgHead->bLastPacket = FALSE;
                bOutputNotFinish = TRUE;
            }
            else
            {
                /*本应用包处理完后，lastpacket按照应用包的来*/
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

            /* 发送 */
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
    
    /*复原变化标记*/	
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

    /*复原变化标记*/
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

    /*复原变化标记*/
    pLinkState->bInputStateChanged = FALSE;
}

void Oam_ConstructShowCmdLineMsg(TYPE_LINK_STATE *pLinkState, WORD16 *pwPacketSize, BYTE *pPacket, BYTE *pbSendCmdLine)
{
    WORD16 wStrLen = 0;
    WORD16 wPacketSize = *pwPacketSize;
    BYTE bSendCmdLine = *pbSendCmdLine;
    WORD16 wHeadSize = sizeof(WORD16) + sizeof(BYTE);

    /* 如果是和app的最后一包，并且不是异步命令，输出命令行*/
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
            /*这里默认如果是最后一包，都要显示cmdline，
            如果外面指定了不显示cmdline，也只是一次；恢复一直都显示*/
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




