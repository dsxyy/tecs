/**************************************************************************
* 版权所有 (C)2002, 深圳市中兴通讯股份有限公司。
*
* 文件名称：linkstate.c
* 文件标识：
* 内容摘要：操作linkstate的相关结构
* 其它说明：
            
* 当前版本：
* 作    者：殷浩
* 完成日期：2008.10.30
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
#include <assert.h>
#include "linkstate.h"
#include "interpret.h"
#include "pub_addition.h"

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
static void InitMatchResult(TYPE_MATCH_RESULT *pMatchResult);


/**************************************************************************
*                          局部函数实现                                   *
**************************************************************************/
/**************************************************************************
*                          全局函数实现                                   *
**************************************************************************/
void InitMatchResult(TYPE_MATCH_RESULT *pMatchResult)
{

    pMatchResult->bHasNo = FALSE;
    pMatchResult->wSum = 0;
    pMatchResult->wDepth = 0;
    pMatchResult->wLength = 0;
    pMatchResult->wFirst = 0;
    pMatchResult->wCmdAttrPos = 0;
    pMatchResult->wParaSum = 0;
    pMatchResult->bParaNum = 0;
    pMatchResult->wCurParaPos = 0;
    pMatchResult->wMaxDepth = 0;
    pMatchResult->wMaxLength = 0;
    pMatchResult->wMaxFirst = 0;
    pMatchResult->wMaxCurParaPos= 0;
    pMatchResult->wHintStackTop = 0;
    pMatchResult->bCanExecute = FALSE;
    pMatchResult->wParaBeginPos = 0;
    pMatchResult->wParaEndPos = 0;
    pMatchResult->bMoreLayerTop = 0;
    memset(pMatchResult->wMoreBeginPos, 0, sizeof(WORD16) * MAX_MORE_LAYER);
    memset(pMatchResult->wMoreEndPos, 0, sizeof(WORD16) * MAX_MORE_LAYER);
    memset(pMatchResult->bType, 0, MAX_MORE_LAYER);
    memset(pMatchResult->bContinueSearch, 0, MAX_MORE_LAYER);
    memset(pMatchResult->bMoreFlags, 0, MAX_MORE_LAYER);
    pMatchResult->wMatchedPathTop= 0;
    pMatchResult->bPrammarRecycle= 0;
    pMatchResult->wParseState = 0;
    pMatchResult->wPathStackTop = 0;
    pMatchResult->wCliMatchLen = 0;
    pMatchResult->dExeID = 0;

    {
        WORD16 i;
        for (i = 0; i < pMatchResult->wParaSum; i++)
        {
            if (pMatchResult->tPara[i].pValue == NULL) 
            {
                continue;
            }
			
            pMatchResult->tPara[i].Len    = 0;
            pMatchResult->tPara[i].ParaNo = 0;
            pMatchResult->tPara[i].Type   = 0;
            pMatchResult->tPara[i].pValue = NULL;
        }
    }
	
    return;
}

void Oam_LinkInitializeForNewConn(TYPE_LINK_STATE *pLinkState)
{
    TYPE_MATCH_RESULT   *pMatchResult = NULL;
    
    assert(pLinkState);
    if (!pLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "Null input parameter: plinkstate");
        return;
    }
    pMatchResult = &(pLinkState->tMatchResult);
    InitMatchResult(pMatchResult);

    pLinkState->dwExeSeqNo        = 0;

    memset(pLinkState->tnIpAddr, 0, LEN_IP_ADDR_STR);
    memset(&(pLinkState->tCmdLine), 0, sizeof(TYPE_CMD_LINE));
    memset(&(pLinkState->tInputControl), 0, sizeof(TYPE_TEXT_INPUT));

    memset(pLinkState->sUserName, 0, MAX_USERNAME_LEN);
    memset(pLinkState->sPassword, 0, MAX_OAM_USER_PWD_ENCRYPT_LEN);
    pLinkState->ucUserRight        = 0;
    memset(pLinkState->sSavedUser, 0, MAX_USERNAME_LEN);
    pLinkState->ucSavedRight       = 0;
    pLinkState->ucInputErrTimes    = 0;

    memset(&pLinkState->LastData, 0, sizeof(pLinkState->LastData));
    memset(&pLinkState->BUF, 0, sizeof(pLinkState->BUF));

    pLinkState->bChangeMode       = FALSE;
    pLinkState->bModeStackTop    = 0;;
    memset(pLinkState->sPrompt, 0, MAX_PROMPT_LEN);

    pLinkState->bInputStateChanged = FALSE;
    pLinkState->bOutputMode       = OUTPUT_MODE_NORMAL;
    pLinkState->bOutputResult     = FALSE;
    pLinkState->wSendPos          = 0;

    pLinkState->bOutputCmdLine    = TRUE;

    pLinkState->bTelnetQuit       = FALSE;
    pLinkState->bIsSupperUsrMode = FALSE;
    /*防止恢复时候用的不支持空格字符串，新建连接时候都重设一下*/
    g_ptOamIntVar->bSupportStringWithBlank = TRUE;
	
    memset(g_ptOamIntVar->sOutputResultBuf, 0, MAX_OUTPUT_RESULT);
    memset(g_ptOamIntVar->sCmdLineBuf, 0, MAX_CMDLINE_LEN);
    pLinkState->sOutputResult = g_ptOamIntVar->sOutputResultBuf;
    pLinkState->sCmdLine = g_ptOamIntVar->sCmdLineBuf;
    pLinkState->tDispGlobalPara.pDisplayBuffer = pLinkState->sOutputResult;

    pLinkState->bTelInputModeIsSyn = TRUE;
    pLinkState->bTelInputModeChanged = FALSE;
    
    pLinkState->wAppMsgReturnCode = SUCC_AND_HAVEPARA;

    memset(&pLinkState->tCmdLog, 0, sizeof(T_CliCmdLogRecord));

    pLinkState->bSSHConnRslt = FALSE;
    pLinkState->bSSHAuthMsg = FALSE;
    pLinkState->bSSHAuthRslt = FALSE;

    memset(&(pLinkState->tUserLog), 0, sizeof(TYPE_USER_LOG));
    
}

BOOLEAN XML_CheckCanExecSaveCmd(TYPE_XMLLINKSTATE *pLinkState,BYTE ucCurRunState)
{
    WORD16 wLoop = 0;
    WORD32 dwLinkChannel = 0;
    BYTE ucState = 0;
    
    for(wLoop =0;wLoop < MAX_SIZE_LINKSTATE;wLoop++)
    {
        dwLinkChannel = gt_XmlLinkState[wLoop].dwLinkChannel;
        ucState = (gt_XmlLinkState[wLoop].ucCurRunState)&ucCurRunState;
        if ((ucCurRunState == ucState) &&
	     (dwLinkChannel != pLinkState->dwLinkChannel))
        {
            return FALSE;
        }
    }
    /*需要检查Telnet端是否有存盘*/
    for (wLoop = MIN_CLI_VTY_NUMBER; wLoop <= MAX_CLI_VTY_NUMBER; wLoop++)
    {
        if (pec_MsgHandle_EXECUTE == gtLinkState[wLoop-MIN_CLI_VTY_NUMBER].ucCurRunState)
        {
            return FALSE;
        }
    }
    return TRUE;
}

BYTE XML_LinkGetCurRunState(TYPE_XMLLINKSTATE *pLinkState)
{
    assert(pLinkState);
    if (!pLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_DEBUG, "Null input parameter: plinkstate");
        return 0;
    }
    return pLinkState->ucCurRunState;
}

void XML_LinkSetCurRunState(TYPE_XMLLINKSTATE *pLinkState,BYTE ucState)
{
    assert(pLinkState);
    if (!pLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_DEBUG, "Null input parameter: plinkstate");
        return;
    }
    pLinkState->ucCurRunState = ucState;
    return;
}

/**********************************************************************
* 函数名称：VOID CheckLinkIsExecutingCmd
* 功能描述：检查是否有链路执行命令
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：   TRUE/FALSE
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2009-04-08             齐龙兆      创建
************************************************************************/
BOOLEAN CheckLinkIsExecutingCmd(void)
{
    WORD16 wLoop = 0;
    WORD16 wTelLinkNum = 0;
    WORD16 wOmmLinkNum = 0;
    TYPE_LINK_STATE *ptTelLinkState = NULL;	
    TYPE_XMLLINKSTATE *ptXmlLinkState = NULL;
	
    wTelLinkNum = sizeof(gtLinkState)/sizeof(TYPE_LINK_STATE);
    wOmmLinkNum = sizeof(gt_XmlLinkState)/sizeof(TYPE_XMLLINKSTATE);

    /*检查是否有Telnet端在执行命令操作*/
    for(wLoop = 0;wLoop < wTelLinkNum;wLoop++)
    {
        ptTelLinkState = &(gtLinkState[wLoop]); 
        if(pec_MsgHandle_EXECUTE == ptTelLinkState->ucCurRunState)
        {
            break;
        }
    }
    if (wLoop < wTelLinkNum)
    {        
        return TRUE;
    }
    /*检查是否有OMM端正在执行命令操作*/
    for (wLoop = 0;wLoop < wOmmLinkNum;wLoop++)
    {
        ptXmlLinkState = &(gt_XmlLinkState[wLoop]);
        if (pec_MsgHandle_EXECUTE == ptXmlLinkState->ucCurRunState)
        {
            break;
        }
    }
    if (wLoop < wOmmLinkNum)
    {	 
        return TRUE;
    }
    return FALSE;
}



/**********************************************************************
* 函数名称：VOID CheckLinkIsExecutingSaveCmd
* 功能描述：检查是否有链路执行存盘命令
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：   TRUE/FALSE
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2009-04-08             齐龙兆      创建
************************************************************************/
BOOLEAN CheckLinkIsExecutingSaveCmd(void)
{
    WORD16 wLoop = 0;
    WORD16 wTelLinkNum = 0;
    WORD16 wOmmLinkNum = 0;
    TYPE_LINK_STATE *ptTelLinkState = NULL;	
    TYPE_XMLLINKSTATE *ptXmlLinkState = NULL;
	
    wTelLinkNum = sizeof(gtLinkState)/sizeof(TYPE_LINK_STATE);
    wOmmLinkNum = sizeof(gt_XmlLinkState)/sizeof(TYPE_XMLLINKSTATE);

    /*检查是否有Telnet端在执行存盘操作*/
    for(wLoop = 0;wLoop < wTelLinkNum;wLoop++)
    {
        ptTelLinkState = &(gtLinkState[wLoop]); 
        if((pec_MsgHandle_EXECUTE == ptTelLinkState->ucCurRunState) &&
	    (CMD_DEF_SET_SAVE == ptTelLinkState->tMatchResult.dExeID) )
        {
            break;
        }
    }
    if (wLoop < wTelLinkNum)
    {        
        return TRUE;
    }
    /*检查是否有OMM端正在执行存盘命令*/
    for (wLoop = 0;wLoop < wOmmLinkNum;wLoop++)
    {
        ptXmlLinkState = &(gt_XmlLinkState[wLoop]);
        if ((pec_MsgHandle_EXECUTE == ptXmlLinkState->ucCurRunState) &&
	     (CMD_DEF_SET_SAVE == ptXmlLinkState->dwCmdId) )
        {
            break;
        }
    }
    if (wLoop < wOmmLinkNum)
    {	 
        return TRUE;
    }
    return FALSE;
}

WORD16 Xml_GetIndexOfSaveLinkState(void)
{
    WORD16 wLoop = 0;
    WORD16 wOmmLinkNum = 0;    
    TYPE_XMLLINKSTATE *ptXmlLinkState = NULL;
    wOmmLinkNum = sizeof(gt_XmlLinkState)/sizeof(TYPE_XMLLINKSTATE);
    /*检查是否有其他OMM端正在执行存盘命令*/
    for (wLoop = 0;wLoop < wOmmLinkNum;wLoop++)
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
        return wLoop;
    }
    return 0;
}

/**********************************************************************
* 函数名称：VOID Xml_CheckIsExecSaveing
* 功能描述：检查是否有网管端在存盘
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：   无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2009-04-08             齐龙兆      创建
************************************************************************/
BOOLEAN Xml_CheckIsExecSaving(void)
{
    WORD16 wLoop = 0;
    WORD16 wOmmLinkNum = 0;    
    TYPE_XMLLINKSTATE *ptXmlLinkState = NULL;
    wOmmLinkNum = sizeof(gt_XmlLinkState)/sizeof(TYPE_XMLLINKSTATE);
	
    /*检查是否有其他OMM端正在执行存盘命令*/
    for (wLoop = 0;wLoop < wOmmLinkNum;wLoop++)
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
        return TRUE;
    }
    return FALSE;
}

void Oam_LinkSetCurRunState(TYPE_LINK_STATE *pLinkState, BYTE ucCurRunState)
{
    assert(pLinkState);
    if (!pLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "Null input parameter: plinkstate");
        return;
    }

    pLinkState->ucCurRunState = ucCurRunState;
}

BYTE Oam_LinkGetCurRunState(TYPE_LINK_STATE *pLinkState)
{
    assert(pLinkState);
    if (!pLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "Null input parameter: plinkstate");
        return 0;
    }

    return pLinkState->ucCurRunState;
}

void Oam_LinkSetLinkId(TYPE_LINK_STATE *pLinkState, BYTE ucLinkId)
{
    assert(pLinkState);
    if (!pLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "Null input parameter: plinkstate");
        return;
    }

    pLinkState->ucLinkID = ucLinkId;
}

BYTE Oam_LinkGetLinkId(TYPE_LINK_STATE *pLinkState)
{
    assert(pLinkState);
    if (!pLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "Null input parameter: plinkstate");
        return 0;
    }

    return pLinkState->ucLinkID;
}

/**************************************************************************
* 函数名称：VOID Oam_SetIdleTimer
* 功能描述：设置空闲超时定时器
* 访问的表：无
* 修改的表：无
* 输入参数：
  TYPE_LINK_STATE *pLinkState
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
void Oam_SetIdleTimer(TYPE_LINK_STATE *pLinkState)
{
    pLinkState->wIdleTimer = XOS_SetRelativeTimer(PROCTYPE_OAM_CLIMANAGER, TIMER_IDLE_TIMEOUT, DURATION_IDLE_TIMEOUT, pLinkState->ucLinkID);
    return;
}

/* 614000678048 (去掉24小时绝对超时) */
#if 0
/**************************************************************************
* 函数名称：VOID Oam_SetAbsoluteTimer
* 功能描述：设置绝对超时定时器
* 访问的表：无
* 修改的表：无
* 输入参数：
  TYPE_LINK_STATE *pLinkState
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
VOID Oam_SetAbsTimer(TYPE_LINK_STATE *pLinkState)
{
    pLinkState->wAbsTimer = XOS_SetRelativeTimer(TIMER_ABS_TIMEOUT, DURATION_ABS_TIMEOUT, pLinkState->ucLinkID);
    return;
}
#endif

/**************************************************************************
* 函数名称：VOID Oam_SetExeTimer
* 功能描述：接收来自定时器的消息
* 访问的表：无
* 修改的表：无
* 输入参数：
  TYPE_LINK_STATE *pLinkState
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
void Oam_SetExeTimer(TYPE_LINK_STATE *pLinkState)
{
    pLinkState->wExeTimer = XOS_SetRelativeTimer(PROCTYPE_OAM_CLIMANAGER, TIMER_EXECUTE_NORMAL, DURATION_EXECUTE_NORMAL, pLinkState->ucLinkID);
    return;
}

/**************************************************************************
* 函数名称：VOID Xml_SetExeTimer
* 功能描述：启动XML Adapter与前台应用链路定时器
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_XMLLINKSTATE *pLinkState  
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/04/30    V1.0    齐龙兆      创建
**************************************************************************/
void Xml_SetExeTimer(TYPE_XMLLINKSTATE *pLinkState)
{
    assert(pLinkState);
    if (!pLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "Null input parameter: plinkstate");
        return;
    }    
    pLinkState->wExeTimer = XOS_SetRelativeTimer(PROCTYPE_OAM_CLIMANAGER, TIMER_EXECUTE_XMLADAPTER, DURATION_EXECUTE_XMLADAPTER, pLinkState->dwLinkChannel);
}

/**************************************************************************
* 函数名称：VOID Oam_KillIdleTimer
* 功能描述：杀掉空闲超时端定时器
* 访问的表：无
* 修改的表：无
* 输入参数：
  TYPE_LINK_STATE *pLinkState
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
void Oam_KillIdleTimer(TYPE_LINK_STATE *pLinkState)
{
    if (INVALID_TIMER_ID != pLinkState->wIdleTimer)
    {
        XOS_KillTimerByTimerId(pLinkState->wIdleTimer);
        pLinkState->wIdleTimer = INVALID_TIMER_ID;
    }
    pLinkState->wIdleTimeout = 0;
    return;
}

/* 614000678048 (去掉24小时绝对超时) */
#if 0
/**************************************************************************
* 函数名称：VOID Oam_KillAbsTimer
* 功能描述：杀掉绝对超时端定时器
* 访问的表：无
* 修改的表：无
* 输入参数：
  TYPE_LINK_STATE *pLinkState
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
VOID Oam_KillAbsTimer(TYPE_LINK_STATE *pLinkState)
{
    if (INVALID_TIMER_ID != pLinkState->wAbsTimer)
    {
        XOS_KillTimerByTimerId(pLinkState->wAbsTimer);
        pLinkState->wAbsTimer = INVALID_TIMER_ID;
    }
    pLinkState->wAbsTimeout = 0;
    return;
}
#endif

/**************************************************************************
* 函数名称：VOID Oam_KillExeTimer
* 功能描述：杀掉Execute端定时器
* 访问的表：无
* 修改的表：无
* 输入参数：
  TYPE_LINK_STATE *pLinkState
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
void Oam_KillExeTimer(TYPE_LINK_STATE *pLinkState)
{
    if (INVALID_TIMER_ID != pLinkState->wExeTimer)
    {
        XOS_KillTimerByTimerId(pLinkState->wExeTimer);
        pLinkState->wExeTimer = INVALID_TIMER_ID;
    }
    return;
}

/**************************************************************************
* 函数名称：VOID Xml_KillExeTimer
* 功能描述：杀死XML Adapter与前台应用链路定时器
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_XMLLINKSTATE *pLinkState  
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/04/30    V1.0    齐龙兆      创建
**************************************************************************/
void Xml_KillExeTimer(TYPE_XMLLINKSTATE *pLinkState)
{
    assert(pLinkState);
    if (!pLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "Null input parameter: plinkstate");
        return;
    }
    if (INVALID_TIMER_ID != pLinkState->wExeTimer)
    {
        XOS_KillTimerByTimerId(pLinkState->wExeTimer);
        pLinkState->wExeTimer = INVALID_TIMER_ID;
    } 
    return;
}

/**************************************************************************
* 函数名称：Oam_GetCurModeId
* 功能描述：获取指定的linkstate当前配置模式id
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_LINK_STATE *pLinkState
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2008/04/23    V1.0    殷浩      创建
**************************************************************************/
BYTE Oam_GetCurModeId(TYPE_LINK_STATE *pLinkState)
{
    assert(pLinkState);
    if (!pLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "Null input parameter: plinkstate");
        return 0;
    }
	
    if (pLinkState->bModeStackTop <= 0)
    {
        return 0;
    }
	
    return pLinkState->bModeID[pLinkState->bModeStackTop - 1];
}

TYPE_MODE* Oam_GetCurMode(TYPE_LINK_STATE *pLinkState)
{
    TYPE_MODE *pRet = NULL;
    WORD32 i = 0;
    BYTE ucModeID = Oam_GetCurModeId(pLinkState);
	
    if ((0 == g_dwModeCount) || (!gtMode))
    {
        return NULL;		
    }

    for (i = 0; i <= g_dwModeCount; i++)
    {
        if (gtMode[i].bModeID == ucModeID)
        {
            pRet = &gtMode[i];
            break;
        }
    }

    return pRet;
}

void Oam_LinkSetInputState(TYPE_LINK_STATE *pLinkState, BYTE ucInputState)
{
    assert(pLinkState);
    if (!pLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "Null input parameter: plinkstate");
        return;
    }

    pLinkState->ucInputState = ucInputState;
    pLinkState->bInputStateChanged = TRUE;
}

BYTE Oam_LinkGetInputState(TYPE_LINK_STATE *pLinkState)
{
    assert(pLinkState);
    if (!pLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "Null input parameter: plinkstate");
        return 0;
    }

    return pLinkState->ucInputState;
}

void Oam_LinkModeStack_Push(TYPE_LINK_STATE *pLinkState,  BYTE ucModeId)
{
    assert(pLinkState);
    if (!pLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "Null input parameter: plinkstate");
        return;
    }

    pLinkState->bModeID[pLinkState->bModeStackTop] = ucModeId;
    pLinkState->bModeStackTop++;
}

void Oam_LinkModeStack_Pop(TYPE_LINK_STATE *pLinkState)
{
    assert(pLinkState);
    if (!pLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "Null input parameter: plinkstate");
        return;
    }

    /*模式栈顶是一个没有赋值的空模式*/
    if (pLinkState->bModeStackTop > 0)
    {
        pLinkState->bModeStackTop--;
    }
	
    pLinkState->bModeID[pLinkState->bModeStackTop] = CMD_MODE_INVALID;
}

void Oam_LinkModeStack_Init(TYPE_LINK_STATE *pLinkState)
{
    assert(pLinkState);
    if (!pLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "Null input parameter: plinkstate");
        return;
    }

    pLinkState->bModeStackTop = 0;
	
    memset(pLinkState->bModeID, CMD_MODE_INVALID, sizeof(pLinkState->bModeID));
}

BOOLEAN Oam_CheckIsHasMode(TYPE_LINK_STATE *pLinkState,BYTE bModeId)
{
    BYTE bIndex = 0;
    if (NULL == pLinkState)
    {
        return FALSE;
    }   
    for (bIndex = 0;bIndex <= pLinkState->bModeStackTop;bIndex++)
    {
        if (bModeId == pLinkState->bModeID[bIndex])
        {
            return TRUE;
        }
    }
    return FALSE;
}

BOOLEAN Oam_LinkModeStatck_IsEmpty(TYPE_LINK_STATE *pLinkState)
{
    assert(pLinkState);
    if (!pLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "Null input parameter: plinkstate");
        return FALSE;
    }

    return pLinkState->bModeStackTop == 0;
}

/**********************************************************************
* 函数名称：BOOLEAN CheckCurCmdIsNo
* 功能描述：检查当前命令是否为NO命令
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_XMLLINKSTATE *ptLinkState:链路状态指针
* 输出参数：无   
* 返 回 值：    RETURN_ERROR-失败 RETURN_OK-成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
BOOLEAN XML_CheckCurCmdIsNo(TYPE_XMLLINKSTATE *ptLinkState)
{
    T_OAM_Cfg_Dat* cfgDat = NULL;    
    BOOL bRet = FALSE;	
    /*参数检查*/
    assert(ptLinkState);
    if (NULL == ptLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_DEBUG, "Null input parameter: plinkstate");
        return TRUE;
    }

    cfgDat = ptLinkState->tCurCMDInfo.cfgDat;
    /*检查是否为NO命令*/
    if (CMD_ATTR_HASNO(&cfgDat->tCmdAttr[ptLinkState->tCurCMDInfo.wCmdAttrPos]) && 
         ptLinkState->tCurCMDInfo.bIsNoCmd)
    {
        bRet = TRUE;
    }
    else
    {
        bRet = FALSE;
    }
		
    return bRet;
}

/**************************************************************************
* 函数名称：VOID XML_ChangeMode
* 功能描述：改变当前模式
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_XMLLINKSTATE *pLinkState
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
**************************************************************************/
void XML_ChangeMode(TYPE_XMLLINKSTATE *pLinkState)
{
    BYTE bNextModeID;
    T_OAM_Cfg_Dat * cfgDat = pLinkState->tCurCMDInfo.cfgDat;    
    
    /*参数检查*/
    if (NULL == pLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "pLinkState is NULL! can't change mode to \n");
        return;
    }
    if (NULL == cfgDat)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "pLinkState->tMatchResult.cfgDat is NULL! can't change mode to  \n");
        return;
    }
	
    bNextModeID = cfgDat->tCmdAttr[pLinkState->tCurCMDInfo.wCmdAttrPos].bNextModeID;       

    /*当前为no命令时不处理*/
    if (XML_CheckCurCmdIsNo(pLinkState))
        return;

    /*当前模式和下一模式相同时不处理*/
    if ((bNextModeID == CMD_MODE_SAME) ||
         (bNextModeID == XML_GetCurModeId(pLinkState)))
        return;   

    if (bNextModeID != CMD_MODE_FORWARD)  /*下一模式非前一模式*/
    {
        if (bNextModeID > XML_GetCurModeId(pLinkState))
        {
            XML_LinkModeStack_Push(pLinkState, bNextModeID);
        }
        else
        {
            /*后退到指定模式*/
            do
            {
                 XML_LinkModeStack_Pop(pLinkState);
            }while (!XML_LinkModeStatck_IsEmpty(pLinkState) && bNextModeID < XML_GetCurModeId(pLinkState));
        }
    }
    else /*下一模式为前一模式*/
    {
        /*后退到前一模式*/
        XML_LinkModeStack_Pop(pLinkState);
    }

    return;

}


/**********************************************************************
* 函数名称：BYTE Oam_GetCurModeId
* 功能描述：获取指定的linkstate当前配置模式id
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_XMLLINKSTATE *ptLinkState:链路状态指针
* 输出参数：无   
* 返 回 值：    RETURN_ERROR-失败 RETURN_OK-成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
BYTE XML_GetCurModeId(TYPE_XMLLINKSTATE *ptLinkState)
{
    assert(ptLinkState);
    if (!ptLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_DEBUG, "Null input parameter: plinkstate");
        return 0;
    }
    if (0 == ptLinkState->bModeStackTop)
    {
        return 0;
    }
	
    return ptLinkState->bModeID[ptLinkState->bModeStackTop - 1];
}

/**********************************************************************
* 函数名称：VOID XML_LinkModeStack_Push
* 功能描述：模式ID压栈
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_XMLLINKSTATE *ptLinkState:链路状态指针
                              BYTE ucModeId:模式ID
* 输出参数：无   
* 返 回 值：    RETURN_ERROR-失败 RETURN_OK-成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
void XML_LinkModeStack_Push(TYPE_XMLLINKSTATE *pLinkState,  BYTE ucModeId)
{
    assert(pLinkState);
    if (!pLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_DEBUG, "Null input parameter: plinkstate");
        return;
    }

    pLinkState->bModeID[pLinkState->bModeStackTop] = ucModeId;
    pLinkState->bModeStackTop++;
}

/**********************************************************************
* 函数名称：VOID XML_LinkModeStack_Pop
* 功能描述：模式ID出栈
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_XMLLINKSTATE *ptLinkState:链路状态指针                              
* 输出参数：无   
* 返 回 值：    RETURN_ERROR-失败 RETURN_OK-成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
void XML_LinkModeStack_Pop(TYPE_XMLLINKSTATE *pLinkState)
{
    assert(pLinkState);
    if (!pLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_DEBUG, "Null input parameter: plinkstate");
        return;
    }

    /*模式栈顶是一个没有赋值的空模式*/
    if (pLinkState->bModeStackTop > 1)
    {
        pLinkState->bModeStackTop--;
    }
	
    pLinkState->bModeID[pLinkState->bModeStackTop] = CMD_MODE_INVALID;
}

/**********************************************************************
* 函数名称：BOOLEAN XML_LinkModeStatck_IsEmpty
* 功能描述：检查模式栈是否为空
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_XMLLINKSTATE *ptLinkState:链路状态指针                              
* 输出参数：无   
* 返 回 值：    RETURN_ERROR-失败 RETURN_OK-成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
BOOLEAN XML_LinkModeStatck_IsEmpty(TYPE_XMLLINKSTATE *pLinkState)
{
    assert(pLinkState);
    if (!pLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_DEBUG, "Null input parameter: plinkstate");
        return FALSE;
    }

    return pLinkState->bModeStackTop == 1;
}

/**********************************************************************
* 函数名称：VOID  Xml_InitLinkState
* 功能描述：初始化链路状态
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：   无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
void Xml_InitLinkState(void)
{
    WORD16 wLoop = 0;
    TYPE_XMLLINKSTATE *ptLinkState = NULL;
    for (wLoop = 0; wLoop < MAX_SIZE_LINKSTATE;wLoop++)
    {
        ptLinkState = &gt_XmlLinkState[wLoop];
        if (NULL != ptLinkState->ptLastSendMsg)
        {
            OAM_RETUB(ptLinkState->ptLastSendMsg);
            ptLinkState->ptLastSendMsg = NULL;
        }
        memset(ptLinkState,0,sizeof(TYPE_XMLLINKSTATE));
    }    
}

/**********************************************************************
* 函数名称：INT  Xml_GetIndexOfLinkState
* 功能描述：获取链路状态下标
* 访问的表：无
* 修改的表：无
* 输入参数：WORD16 wLinkChannel:终端号 
* 输出参数：无
* 返 回 值：   链路状态下标
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
INT Xml_GetIndexOfLinkState(WORD32 dwLinkChannel)
{
    INT iLoop = 0;
    
    for(iLoop = 0;iLoop < MAX_SIZE_LINKSTATE;iLoop++)
    {
        if (dwLinkChannel == gt_XmlLinkState[iLoop].dwLinkChannel)
        {
            return iLoop;
        }
    }
    return -1;
}


/**********************************************************************
* 函数名称：XML_AGENT_STAT  *Xml_GetLinkStateByIndex
* 功能描述：获取链路状态
* 访问的表：无
* 修改的表：无
* 输入参数：WORD16 wLinkStateIndex:终端号下标 
* 输出参数：无
* 返 回 值：   TYPE_XMLLINKSTATE *链路状态
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
TYPE_XMLLINKSTATE *Xml_GetLinkStateByIndex(WORD16 wLinkStateIndex)
{
    if (wLinkStateIndex >= MAX_SIZE_LINKSTATE)
    {
        return NULL;
    }
    return &(gt_XmlLinkState[wLinkStateIndex]);
}
BOOLEAN XML_LinkGetModeID(WORD16 wLinkStateIndex,BYTE *pModeID)
{
    /*入参判断*/
    if (NULL == pModeID)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return FALSE;
    }
    if (wLinkStateIndex >= MAX_SIZE_LINKSTATE)
    {
        return FALSE;
    }
    memset(pModeID,0,MAX_MODESTACK);
    memcpy(pModeID,gt_XmlLinkState[wLinkStateIndex].bModeID,MAX_MODESTACK);	
    return TRUE;
}
BOOLEAN XML_LinkSetModeID(WORD16 wLinkStateIndex,BYTE *pModeID)
{
    /*入参判断*/
    if (NULL == pModeID)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return FALSE;
    }
    if (wLinkStateIndex >= MAX_SIZE_LINKSTATE)
    {
        return FALSE;
    }
    memcpy(gt_XmlLinkState[wLinkStateIndex].bModeID,pModeID,MAX_MODESTACK);
    return TRUE;
}
BOOLEAN XML_LinkGetModeStackTop(WORD16 wLinkStateIndex,BYTE *pModeStackTop)
{
    /*入参判断*/
    if (NULL == pModeStackTop)
{
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return FALSE;
    }
    if (wLinkStateIndex >= MAX_SIZE_LINKSTATE)
    {
        return FALSE;
    }
    *pModeStackTop = gt_XmlLinkState[wLinkStateIndex].bModeStackTop;	
    return TRUE;
}
BOOLEAN XML_LinkSetModeStackTop(WORD16 wLinkStateIndex,BYTE *pModeStackTop)
{
    /*入参判断*/
    if (NULL == pModeStackTop)
{
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return FALSE;
    }
    if (wLinkStateIndex >= MAX_SIZE_LINKSTATE)
    {
        return FALSE;
    }
    gt_XmlLinkState[wLinkStateIndex].bModeStackTop = *pModeStackTop;
    return TRUE;
}
/**********************************************************************
* 函数名称：XML_AGENT_STAT  Xml_GetLinkStateByLinkChannel
* 功能描述：根据链路号获取链路状态
* 访问的表：无
* 修改的表：无
* 输入参数：WORD16 wLinkChannel:终端号 
* 输出参数：无
* 返 回 值：   TYPE_XMLLINKSTATE *链路状态
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
TYPE_XMLLINKSTATE *Xml_GetLinkStateByLinkChannel(WORD32 dwLinkChannel)
{
    WORD16 wLoop = 0;
    
    for(wLoop = 0;wLoop < MAX_SIZE_LINKSTATE;wLoop++)
    {
        if ((dwLinkChannel == gt_XmlLinkState[wLoop].dwLinkChannel)&&
	     (XML_LINKSTATE_USED == gt_XmlLinkState[wLoop].bUsed))
        {
            return &(gt_XmlLinkState[wLoop]);
        }
    }
    return NULL;
}

/**********************************************************************
* 函数名称：XML_AGENT_STAT  Xml_GetLinkStateByLinkChannel
* 功能描述：根据链路号获取链路状态
* 访问的表：无
* 修改的表：无
* 输入参数：WORD16 wLinkChannel:终端号 
* 输出参数：无
* 返 回 值：   TYPE_XMLLINKSTATE *链路状态
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
TYPE_XMLLINKSTATE *Xml_GetCurUsedLinkState(void)
{
    WORD16 wLoop = 0;
    
    for(wLoop = 0;wLoop < MAX_SIZE_LINKSTATE;wLoop++)
    {
        if ((XML_LINKSTATE_USED == gt_XmlLinkState[wLoop].bUsed) &&
            (pec_MsgHandle_EXECUTE == gt_XmlLinkState[wLoop].ucCurRunState))
        {
            return &(gt_XmlLinkState[wLoop]);
        }
    }
    return NULL;
}

/**********************************************************************
* 函数名称：XML_AGENT_STAT  XML_GetUsableLinkState
* 功能描述：获取可用链路状态
* 访问的表：无
* 修改的表：无
* 输入参数：无 
* 输出参数：无 
* 返 回 值：   XML链路指针
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
TYPE_XMLLINKSTATE *XML_GetUsableLinkState(void)
{
    WORD16 wLoop = 0;
    for (wLoop = 0;wLoop < MAX_SIZE_LINKSTATE;wLoop++)
    {
        if (XML_LINKSTATE_UNUSED == gt_XmlLinkState[wLoop].bUsed)
	 {
	     memset(&gt_XmlLinkState[wLoop],0,sizeof(TYPE_XMLLINKSTATE));
	     return &(gt_XmlLinkState[wLoop]);
	 }
    }    
    return NULL;
}

/**********************************************************************
* 函数名称：XML_AGENT_STAT  Xml_AssignLinkState
* 功能描述：分配链路状态
* 访问的表：无
* 修改的表：无
* 输入参数：WORD16 wLinkChannel:终端号 
* 输出参数：无
* 返 回 值：   TYPE_XMLLINKSTATE *链路状态
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
TYPE_XMLLINKSTATE *Xml_AssignLinkState(WORD32 dwLinkChannel)
{
    TYPE_XMLLINKSTATE *ptLinkState = NULL;/*链路状态*/
    BYTE bModeId = CMD_MODE_PRIV;/*特权模式ID*/

    /*根据链路号获取链路状态*/
    ptLinkState = Xml_GetLinkStateByLinkChannel(dwLinkChannel);
    if (NULL == ptLinkState)
    {
        return NULL;	 
    }
    /*链路状态模式栈空则默认添加特权模式*/
    if (0 == ptLinkState->bModeStackTop)
    {
        /*压栈*/
        XML_LinkModeStack_Push(ptLinkState, bModeId);
    }
    return ptLinkState;
}


void Oam_LinkMarkLogout(TYPE_LINK_STATE *pLinkState, BOOLEAN bLogOut)
{
    assert(pLinkState);
    if (!pLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "Null input parameter: plinkstate");
        return;
    }

    pLinkState->bTelnetQuit = bLogOut;
}

BOOLEAN Oam_LinkNeedLogout(TYPE_LINK_STATE *pLinkState)
{
    assert(pLinkState);
    if (!pLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "Null input parameter: plinkstate");
        return FALSE;
    }

    return pLinkState->bTelnetQuit;
}

WORD32 Oam_LinkGetCurCmdId(TYPE_LINK_STATE *pLinkState)
{
    assert(pLinkState);
    if (!pLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "Null input parameter: plinkstate");
        return 0;
    }

    return pLinkState->tMatchResult.dExeID;
}

void Oam_LinkSetTelSvrInputModeAsSyn(TYPE_LINK_STATE *pLinkState, BOOLEAN bOpen)
{
    assert(pLinkState);
    if (!pLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "Null input parameter: plinkstate");
        return;
    }

    pLinkState->bTelInputModeIsSyn = bOpen;
    pLinkState->bTelInputModeChanged = TRUE;
}

BOOLEAN Oam_LinkIfTelSvrInputModeIsSyn(TYPE_LINK_STATE *pLinkState)
{
    assert(pLinkState);
    if (!pLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "Null input parameter: plinkstate");
        return TRUE;
    }

    return pLinkState->bTelInputModeIsSyn;
}

BOOLEAN Oam_LinkIfRecvAsyncMsg(TYPE_LINK_STATE *pLinkState)
{
    assert(pLinkState);
    if (!pLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "Null input parameter: plinkstate");
        return FALSE;
    }

    return pLinkState->bRecvAsynMsg;
}

BOOLEAN Oam_LinkIfRecvLastPktOfApp(TYPE_LINK_STATE *pLinkState)
{
    assert(pLinkState);
    if (!pLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "Null input parameter: plinkstate");
        return TRUE;
    }

   return ((pLinkState->wAppMsgReturnCode != SUCC_CMD_NOT_FINISHED) && 
               (pLinkState->wAppMsgReturnCode != SUCC_CMD_DISPLAY_AND_KILLTIMER) && 
               (pLinkState->wAppMsgReturnCode != SUCC_CMD_DISPLAY_NO_KILLTIMER) && 
               (pLinkState->wAppMsgReturnCode != SUCC_WAIT_AND_HAVEPARA) );
}

BOOLEAN Oam_LinkIfCurCmdIsNo(TYPE_LINK_STATE *pLinkState)
{
    T_OAM_Cfg_Dat* cfgDat = NULL;
    TYPE_MATCH_RESULT *pMatchResult = NULL;
    BOOL bRet = FALSE;
	
    assert(pLinkState);
    if (!pLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "Null input parameter: plinkstate");
        return TRUE;
    }

    pMatchResult = &(pLinkState->tMatchResult);
    cfgDat = pMatchResult->cfgDat;
   
    if (cfgDat)
    {
        /* deal NO command */
        if (pLinkState->tCmdLine.bHasNo)
        {
            bRet = TRUE;
        }
        else
        {
            bRet = FALSE;
        }
    }
		
    return bRet;
}

void Oam_InnerGetRtnMsgInfo(LPVOID ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg)
{
    TYPE_LINK_STATE *pLinkState = NULL;
    /*入参判断*/
    if ((NULL == ptOamMsg) || (NULL == ptRtnMsg))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return;
    }
    pLinkState = OamCfgGetLinkState(dwMsgId,ptOamMsg);
    if (!pLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                                    "[Oam_Interpret] Failled to get linkstate while processing telnet msg");                         
        return;					 
    }
    ptRtnMsg->CmdID = pLinkState->tMatchResult.dExeID;
    ptRtnMsg->LinkChannel= pLinkState->ucLinkID;
    ptRtnMsg->SeqNo= pLinkState->dwExeSeqNo;	
    return;		
}
BOOLEAN Oam_LinkGetUserRight(LPVOID ptOamMsg,WORD32 dwMsgId,BYTE *pUserRight)
{
    TYPE_LINK_STATE *pLinkState = NULL;
    /*入参判断*/
    if ((NULL == ptOamMsg) || (NULL == pUserRight))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return FALSE;
    }
    pLinkState = OamCfgGetLinkState(dwMsgId,ptOamMsg);
    if (!pLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                                    "[Oam_Interpret] Failled to get linkstate while processing telnet msg");                         
        return FALSE;					 
    }
    *pUserRight = pLinkState->ucUserRight;
    return TRUE;	
}
BOOLEAN Oam_LinkSetUserRight(LPVOID ptOamMsg,WORD32 dwMsgId,BYTE *pUserRight)	
{
    TYPE_LINK_STATE *pLinkState = NULL;
    /*入参判断*/
    if ((NULL == ptOamMsg) || (NULL == pUserRight))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return FALSE;
    }
    pLinkState = OamCfgGetLinkState(dwMsgId,ptOamMsg);
    if (!pLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                                    "[Oam_Interpret] Failled to get linkstate while processing telnet msg");                         
        return FALSE;					 
    }
    pLinkState->ucUserRight = *pUserRight;
    return TRUE;	
}
BOOLEAN Oam_LinkGetCmdLine(LPVOID ptOamMsg,WORD32 dwMsgId,TYPE_CMD_LINE  *pCmdLine)
{
    TYPE_LINK_STATE *pLinkState = NULL;
    /*入参判断*/
    if ((NULL == ptOamMsg) || (NULL == pCmdLine))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return FALSE;
    }
    pLinkState = OamCfgGetLinkState(dwMsgId,ptOamMsg);
    if (!pLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                                    "[Oam_Interpret] Failled to get linkstate while processing telnet msg");                         
        return FALSE;					 
    }
    memset(pCmdLine,0,sizeof(TYPE_CMD_LINE));
    memcpy(pCmdLine,&(pLinkState->tCmdLine),sizeof(TYPE_CMD_LINE));
    return TRUE;	
}
BOOLEAN Oam_LinkSetCmdLine(LPVOID ptOamMsg,WORD32 dwMsgId,TYPE_CMD_LINE  *pCmdLine)
{
    TYPE_LINK_STATE *pLinkState = NULL;
    /*入参判断*/
    if ((NULL == ptOamMsg) || (NULL == pCmdLine))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return FALSE;
    }
    pLinkState = OamCfgGetLinkState(dwMsgId,ptOamMsg);
    if (!pLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                                    "[Oam_Interpret] Failled to get linkstate while processing telnet msg");                         
        return FALSE;					 
    }
    memcpy(&(pLinkState->tCmdLine),pCmdLine,sizeof(TYPE_CMD_LINE));
    return TRUE;	
}
BOOLEAN Oam_LinkGetCmdLog(LPVOID ptOamMsg,WORD32 dwMsgId,T_CliCmdLogRecord *pCmdLog)
{
    TYPE_LINK_STATE *pLinkState = NULL;
    /*入参判断*/
    if ((NULL == ptOamMsg) || (NULL == pCmdLog))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return FALSE;
    }
    pLinkState = OamCfgGetLinkState(dwMsgId,ptOamMsg);
    if (!pLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                                    "[Oam_Interpret] Failled to get linkstate while processing telnet msg");                         
        return FALSE;					 
    }
    memset(pCmdLog,0,sizeof(T_CliCmdLogRecord));
    memcpy(pCmdLog,&(pLinkState->tCmdLog),sizeof(T_CliCmdLogRecord));
    return TRUE;	
}
BOOLEAN Oam_LinkSetCmdLog(LPVOID ptOamMsg,WORD32 dwMsgId,T_CliCmdLogRecord *pCmdLog)
{
    TYPE_LINK_STATE *pLinkState = NULL;
    /*入参判断*/
    if ((NULL == ptOamMsg) || (NULL == pCmdLog))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return FALSE;
    }
    pLinkState = OamCfgGetLinkState(dwMsgId,ptOamMsg);
    if (!pLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                                    "[Oam_Interpret] Failled to get linkstate while processing telnet msg");                         
        return FALSE;					 
    }
    memcpy(&(pLinkState->tCmdLog),pCmdLog,sizeof(T_CliCmdLogRecord));
    return TRUE;	
}
BOOLEAN Oam_LinkGetUserName(LPVOID ptOamMsg,WORD32 dwMsgId,CHAR *pUserName)
{
    TYPE_LINK_STATE *pLinkState = NULL;
    /*入参判断*/
    if ((NULL == ptOamMsg) || (NULL == pUserName))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return FALSE;
    }
    pLinkState = OamCfgGetLinkState(dwMsgId,ptOamMsg);
    if (!pLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                                    "[Oam_Interpret] Failled to get linkstate while processing telnet msg");                         
        return FALSE;					 
    }
    memset(pUserName,0,MAX_USERNAME_LEN);
    memcpy(pUserName,pLinkState->sUserName,MAX_USERNAME_LEN);
    return TRUE;	
}
BOOLEAN Oam_LinkSetUserName(LPVOID ptOamMsg,WORD32 dwMsgId,CHAR *pUserName)
{
    TYPE_LINK_STATE *pLinkState = NULL;
    /*入参判断*/
    if ((NULL == ptOamMsg) || (NULL == pUserName))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return FALSE;
    }
    pLinkState = OamCfgGetLinkState(dwMsgId,ptOamMsg);
    if (!pLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                                    "[Oam_Interpret] Failled to get linkstate while processing telnet msg");                         
        return FALSE;					 
    }
    memcpy(pLinkState->sUserName,pUserName,MAX_USERNAME_LEN);
    return TRUE;
}
BOOLEAN Oam_LinkGetUserNameByIndex(BYTE index,CHAR *pUserName)
{
    if (index > MAX_CLI_VTY_NUMBER)
    {
        return 0;
    }    
    memset(pUserName,0,MAX_USERNAME_LEN);
    memcpy(pUserName,gtLinkState[index].sUserName,MAX_USERNAME_LEN);
    return TRUE;	
}
BOOLEAN Oam_LinkGetIdleTimeoutByIndex(BYTE index,WORD16 *wIdleTimeout)
{
    if (index > MAX_CLI_VTY_NUMBER)
    {
        return 0;
    }    
    *wIdleTimeout = gtLinkState[index].wIdleTimeout;	
    return TRUE;	
}
BOOLEAN Oam_LinkGetTnIpAddrByIndex(BYTE index,CHAR *tnIpAddr)
{
    if (index > MAX_CLI_VTY_NUMBER)
    {
        return 0;
    }   
    memset(tnIpAddr,0,LEN_IP_ADDR_STR);
    memcpy(tnIpAddr, gtLinkState[index].tnIpAddr,LEN_IP_ADDR_STR);	
    return TRUE;	
}
BOOLEAN Oam_LinkGetModeID(LPVOID ptOamMsg,WORD32 dwMsgId,BYTE *pModeID)
{
    TYPE_LINK_STATE *pLinkState = NULL;
    /*入参判断*/
    if ((NULL == ptOamMsg) || (NULL == pModeID))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return FALSE;
    }
    pLinkState = OamCfgGetLinkState(dwMsgId,ptOamMsg);
    if (!pLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                                    "[Oam_Interpret] Failled to get linkstate while processing telnet msg");                         
        return FALSE;					 
    }
    memset(pModeID,0,MAX_MODESTACK);
    memcpy(pModeID,pLinkState->bModeID,MAX_MODESTACK);
    return TRUE;
}
BOOLEAN Oam_LinkSetModeID(LPVOID ptOamMsg,WORD32 dwMsgId,BYTE *pModeID)
{
    TYPE_LINK_STATE *pLinkState = NULL;
    /*入参判断*/
    if ((NULL == ptOamMsg) || (NULL == pModeID))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return FALSE;
    }
    pLinkState = OamCfgGetLinkState(dwMsgId,ptOamMsg);
    if (!pLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                                    "[Oam_Interpret] Failled to get linkstate while processing telnet msg");                         
        return FALSE;					 
    }
    memcpy(pLinkState->bModeID,pModeID,MAX_MODESTACK);
    return TRUE;
}
BOOLEAN Oam_LinkGetModeStackTop(LPVOID ptOamMsg,WORD32 dwMsgId,BYTE *pModeStackTop)
{
    TYPE_LINK_STATE *pLinkState = NULL;
    /*入参判断*/
    if ((NULL == ptOamMsg) || (NULL == pModeStackTop))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return FALSE;
    }
    pLinkState = OamCfgGetLinkState(dwMsgId,ptOamMsg);
    if (!pLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                                    "[Oam_Interpret] Failled to get linkstate while processing telnet msg");                         
        return FALSE;					 
    }
    *pModeStackTop = pLinkState->bModeStackTop;
    return TRUE;	

}
BOOLEAN Oam_LinkSetModeStackTop(LPVOID ptOamMsg,WORD32 dwMsgId,BYTE *pModeStackTop)
{
    TYPE_LINK_STATE *pLinkState = NULL;
    /*入参判断*/
    if ((NULL == ptOamMsg) || (NULL == pModeStackTop))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return FALSE;
    }
    pLinkState = OamCfgGetLinkState(dwMsgId,ptOamMsg);
    if (!pLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                                    "[Oam_Interpret] Failled to get linkstate while processing telnet msg");                         
        return FALSE;					 
    }
     pLinkState->bModeStackTop = *pModeStackTop;
    return TRUE;	
}
BYTE Oam_LinkGetCurRunStateByIndex(BYTE index)
{
    if (index > MAX_CLI_VTY_NUMBER)
    {
        return 0;
    }    
    return Oam_LinkGetCurRunState(&gtLinkState[index]);
}
void Oam_LinkSetCurRunStateByIndex(BYTE index,BYTE ucState)
{
    if (index > MAX_CLI_VTY_NUMBER)
    {
        return;
    } 
    Oam_LinkSetCurRunState(&gtLinkState[index],ucState);
    return ;
}
BOOLEAN Oam_LinkGetIdleTimeout(LPVOID ptOamMsg,WORD32 dwMsgId,WORD16 *wIdleTimeout)
{
    TYPE_LINK_STATE *pLinkState = NULL;
    /*入参判断*/
    if ((NULL == ptOamMsg) || (NULL == wIdleTimeout))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return FALSE;
    }
    pLinkState = OamCfgGetLinkState(dwMsgId,ptOamMsg);
    if (!pLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                                    "[Oam_Interpret] Failled to get linkstate while processing telnet msg");                         
        return FALSE;					 
    }
    *wIdleTimeout = pLinkState->wIdleTimeout;
    return TRUE;	
}
BOOLEAN Oam_LinkSetIdleTimeout(LPVOID ptOamMsg,WORD32 dwMsgId,WORD16 *wIdleTimeout)
{
    TYPE_LINK_STATE *pLinkState = NULL;
    /*入参判断*/
    if ((NULL == ptOamMsg) || (NULL == wIdleTimeout))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return FALSE;
    }
    pLinkState = OamCfgGetLinkState(dwMsgId,ptOamMsg);
    if (!pLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                                    "[Oam_Interpret] Failled to get linkstate while processing telnet msg");                         
        return FALSE;					 
    }
    pLinkState->wIdleTimeout = *wIdleTimeout;	
    return TRUE;
}
BOOLEAN Oam_LinkGetTnIpAddr(LPVOID ptOamMsg,WORD32 dwMsgId,CHAR *tnIpAddr)
{
    TYPE_LINK_STATE *pLinkState = NULL;
    /*入参判断*/
    if ((NULL == ptOamMsg) || (NULL == tnIpAddr))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return FALSE;
    }
    pLinkState = OamCfgGetLinkState(dwMsgId,ptOamMsg);
    if (!pLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                                    "[Oam_Interpret] Failled to get linkstate while processing telnet msg");                         
        return FALSE;					 
    }
    memset(tnIpAddr,0,LEN_IP_ADDR_STR);
    memcpy(tnIpAddr,pLinkState->tnIpAddr,LEN_IP_ADDR_STR);
    return TRUE;
}
BOOLEAN Oam_LinkSetTnIpAddr(LPVOID ptOamMsg,WORD32 dwMsgId,CHAR *tnIpAddr)
{
    TYPE_LINK_STATE *pLinkState = NULL;
    /*入参判断*/
    if ((NULL == ptOamMsg) || (NULL == tnIpAddr))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return FALSE;
    }
    pLinkState = OamCfgGetLinkState(dwMsgId,ptOamMsg);
    if (!pLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                                    "[Oam_Interpret] Failled to get linkstate while processing telnet msg");                         
        return FALSE;					 
    }
    memcpy(pLinkState->tnIpAddr,tnIpAddr,LEN_IP_ADDR_STR);
    return TRUE;
}


