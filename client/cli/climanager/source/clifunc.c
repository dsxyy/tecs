/********************************************************************************
*   FileName:   CLIFunc.c
*   Project:    CLI Parsing Engine
*   Author:     Wangwei
*   CreateTime: 2001/9/13
*   RenameTime: 2001/9/13
*   Note:       This file describes the functions parsing CLI.
********************************************************************************/
#include "includes.h"
#include "saveprtclcfg.h"
#include "oam_execute.h"

/*lint -e794*/
/*lint -e171*/
/*lint -e744*/
/*lint -e539*/
/*lint -e661*/
/*lint -e578*/
/*lint -e676*/
/*lint -e671*/
/*lint -e578*/
/*lint -e922*/
/*lint -e744*/

static CHAR pTempStr1[MAX_STRING_LEN];
static CHAR pTempStr2[MAX_STRING_LEN];

#define MORE_LAYER_TOP ((pMatchResult->bMoreLayerTop > 0) ? (pMatchResult->bMoreLayerTop - 1) : 0)

static void CleanMatchResult(TYPE_MATCH_RESULT *pMatchResult);
static void PerformMatchResult(TYPE_LINK_STATE *pLinkState);
static void MatchCmdLine(TYPE_LINK_STATE *pLinkState, TYPE_CMD_LINE *pCmdLine, TYPE_MATCH_RESULT *pMatchResult);
static void GatherTreeNode(TYPE_LINK_STATE *pLinkState, WORD16 wFatherPos);
static BOOL HasTreeNodeNoCmd(WORD16 wCheckPos);
static void DealCmdDeterminer(TYPE_LINK_STATE *pLinkState);
static void DealCmdException(TYPE_LINK_STATE *pLinkState);
static BOOL NeedMatchPara(TYPE_LINK_STATE *pLinkState, WORD16 wCmdAttrPos, T_OAM_Cfg_Dat* cfgDat);
static void ClearMoreLayer(TYPE_MATCH_RESULT *pMatchResult);
static void MatchCmdPara(TYPE_LINK_STATE *pLinkState);
static int MatchOneParameter(TYPE_LINK_STATE *pLinkState);
static int GetMapDeterminer(TYPE_LINK_STATE *pLinkState, int nPos, int nMaxLen);
static void DealParaDeterminer(TYPE_LINK_STATE *pLinkState);
static void DealParaException(TYPE_LINK_STATE *pLinkState);
static void GetParaFormat(TYPE_CMD_PARA *pCmdParaInfo, CHAR *pOutStr,WORD32 dwBufSize);
static void CombineCmdLine(TYPE_CMD_LINE *pCmdLine, BYTE bIncludeLast, CHAR *pOutStr, WORD32 dwBufSize);
static void CombineNameComment(TYPE_LINK_STATE *pLinkState,WORD16 wFatherPos, CHAR *pOutStr);
static BOOL EndMoreAngle(TYPE_MATCH_RESULT *pMatchResult, WORD16 wParaBeginPos, WORD16 wParaEndPos, WORD16 wFirstParaPos);
static BOOL ForceMoreOne(TYPE_MATCH_RESULT *pMatchResult, WORD16 wFirstParaPos);
static void GatherParameter(TYPE_LINK_STATE *pLinkState, WORD16 wForwardPos, WORD16 wParaEndPos, BYTE bQuestion);
static void CalculateWordTabMax(TYPE_MATCH_RESULT *pMatchResult, CHAR *pOutStr);
static int SkipCurPrammar(TYPE_MATCH_RESULT *pMatchResult);
static void JumpNestingPrammar(TYPE_MATCH_RESULT *pMatchResult, WORD16 wParaBeginPos, WORD16 wParaEndPos);
static int FindDefaultParameter(TYPE_LINK_STATE *pLinkState);
static void SplitCmdLine(CHAR *pInString, TYPE_CMD_LINE *pCmdLine);
static  BOOLEAN IfNoSymbolPrammarInMoreAngleIsMatched(WORD16 wSubParaBeginPos, WORD16 wSubParaEndPos, TYPE_MATCH_RESULT *pMatchResult);
static BOOLEAN HasPermissionExecuteCmd(TYPE_LINK_STATE *pLinkState);

/**************************************************************************
* 函数名称：ParseCmdLine
* 功能描述：解析执行输入命令字符串
* 访问的表：无
* 修改的表：无
* 输入参数：pLinkState :当前连接
                              pInString:要解析的字符串
                              bDeterminer: 限定符(回车、问号、tab等)
                              bIsRestoreProcess:是否是恢复操作
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 
**************************************************************************/
/* parse the command line */
void ParseCmdLine(TYPE_LINK_STATE   * pLinkState, CHAR *pInString, BYTE bDeterminer, BOOL bIsRestoreProcess)
{
    TYPE_CMD_LINE       * pCmdLine      = &(pLinkState->tCmdLine);
    TYPE_MATCH_RESULT   * pMatchResult  = &(pLinkState->tMatchResult);
    T_CliCmdLogRecord *pCmdLog = &(pLinkState->tCmdLog);

    /* 初始化ptCmdLine */
    memset(pCmdLine,0,sizeof(TYPE_CMD_LINE));
    pCmdLine->bHasNo      = FALSE;
    pCmdLine->wWordSum    = 0;
    pCmdLine->bDeterminer = bDeterminer;

    /* 记录命令行 */
    memset(pCmdLog->szCmdLine, 0, MAX_CMDLINE_LEN_LOG);
    if (MAX_CMDLINE_LEN_LOG < strlen(pInString))
        memcpy(pCmdLog->szCmdLine, pInString, MAX_CMDLINE_LEN_LOG-1);
    else
    memcpy(pCmdLog->szCmdLine, pInString, strlen(pInString));
    /* 获取系统时间 */
//    memset(&(pCmdLog->tSysTime), 0, sizeof(T_SysSoftClock));
//    XOS_GetSysClock(&(pCmdLog->tSysTime));
    
    /*g_ptOamIntVar->bSupportStringWithBlank = !(Oam_CheckIsHasMode(pLinkState,CMD_MODE_PROTOCOL));*/
    SplitCmdLine(pInString, pCmdLine);

    /*防止误用上一个命令的dat(切换版本后可能为空指针) */
    if (pec_MsgHandle_EXECUTE != Oam_LinkGetCurRunState(pLinkState)) 
    {
        pMatchResult->cfgDat = NULL;
    }

    /* initialize the match result */
    CleanMatchResult(pMatchResult);

    pMatchResult->wParseState = PARSESTATE_BEGINPARSE;

    /* Is the input NO command */
    if (pCmdLine->bHasNo)
    {
        pMatchResult->bHasNo = TRUE;
    }

    /* match a single command line in the command tree */
    MatchCmdLine(pLinkState, pCmdLine, pMatchResult);

    /* calculate the position of commmand parameters */
    if (pMatchResult->wCmdAttrPos)
    {
        WORD16 wCmdAttrPos = pMatchResult->wCmdAttrPos;
        T_OAM_Cfg_Dat* cfgDat = pMatchResult->cfgDat;
        pLinkState->wCmdAttrPos = wCmdAttrPos;
        if (!pCmdLine->bHasNo) /* command */
        {
            pMatchResult->wParaBeginPos = (WORD16)cfgDat->tCmdAttr[wCmdAttrPos].dwParaPos;
            pMatchResult->wParaEndPos = (WORD16)(cfgDat->tCmdAttr[wCmdAttrPos].dwParaPos + cfgDat->tCmdAttr[wCmdAttrPos].wCmdParaCount);
        }
        else /* No command */
        {
            pMatchResult->wParaBeginPos = (WORD16)(cfgDat->tCmdAttr[wCmdAttrPos].dwParaPos + cfgDat->tCmdAttr[wCmdAttrPos].wCmdParaCount);
            pMatchResult->wParaEndPos = (WORD16)(cfgDat->tCmdAttr[wCmdAttrPos].dwParaPos + cfgDat->tCmdAttr[wCmdAttrPos].wAllParaCount);
        }
    }

    /* deal the ? and TAB after the commmand word */
    if (pMatchResult->wParseState == PARSESTATE_WORD_DETERMINER)
        DealCmdDeterminer(pLinkState);
    if (pMatchResult->wParseState == PARSESTATE_WORD_EXCEPTION)
        DealCmdException(pLinkState);

    /* Match the command parameters */
    if (pMatchResult->wParseState == PARSESTATE_WORD_PARSED)
    {
        T_OAM_Cfg_Dat* cfgDat = pMatchResult->cfgDat;
        if (pMatchResult->wCmdAttrPos != INVALID_CMDINDEX && cfgDat->tCmdAttr[pMatchResult->wCmdAttrPos].dExeID != INVALID_EXEID)
        {
            if (NeedMatchPara(pLinkState, pMatchResult->wCmdAttrPos, pMatchResult->cfgDat))
                MatchCmdPara(pLinkState);
            else
            {
                if (pMatchResult->wDepth == pCmdLine->wWordSum && pCmdLine->bDeterminer == DM_NORMAL)
                {
                    pMatchResult->dExeID = cfgDat->tCmdAttr[pMatchResult->wCmdAttrPos].dExeID;
                    pMatchResult->wParseState = PARSESTATE_FINISH_PARSE;
                }
            }
        }
    }

    /* deal the ? and TAB after the commmand parameter */
    if (pMatchResult->wParseState == PARSESTATE_PARA_DETERMINER)
        DealParaDeterminer(pLinkState);
    if (pMatchResult->wParseState == PARSESTATE_PARA_EXCEPTION)
        DealParaException(pLinkState);

    /* deal the execute command ID */
    if (pMatchResult->wParseState == PARSESTATE_FINISH_PARSE &&
        pCmdLine->bDeterminer == DM_NORMAL)
    {
            PerformMatchResult(pLinkState);
    }

    if (pMatchResult->wParseState == PARSESTATE_ASK_TELNET)
    {
        pLinkState->bOutputResult = FALSE;
        pLinkState->bOutputCmdLine = FALSE;
    }

    if (pMatchResult->dExeID == INVALID_EXEID && !bIsRestoreProcess)
    {
        if (Oam_LinkGetCurRunState(pLinkState) == pec_MsgHandle_EXECUTE)
        {
            pLinkState->bOutputResult = FALSE;
        }
        SendPacketToTelnet(pLinkState, FALSE);
    }
}

/* split the command line and initial the tCmdLine */
static void SplitCmdLine(CHAR *pInString, TYPE_CMD_LINE *pCmdLine)
{
    CHAR *pCurChar = pInString;
    WORD16 wWordSum = 0;
    WORD16 wCharIndex = 0;
    WORD16 wBlankSum = 0;
    BOOLEAN bIsWord = FALSE;

    /* calculate the blanks and split the command line into words */
    while (*pCurChar != MARK_STRINGEND)
    {
        /* calculate the blanks */
        bIsWord = FALSE; /* false */
        wBlankSum = 0;
        while (*pCurChar != MARK_STRINGEND
                && *pCurChar == MARK_BLANK)
        {
            wBlankSum++;
            pCurChar++;
        }
	  if(wWordSum >= MAX_LAYER)
	  {
             pCmdLine->wWordSum = wWordSum;
             return;
	  }
        pCmdLine->wBlankSum[wWordSum] = wBlankSum;

        /* split */
        wCharIndex = 0;
        /*读到引号的时候，一直往后读，
        直到读到下一个非转义引号加空格或结束为止；
        中间要处理转义字符*/
        if ((MARK_TYPE_STRING == *pCurChar) && g_ptOamIntVar->bSupportStringWithBlank)
        {
            BOOLEAN bPreCharIsTrans = FALSE;  /*前一个字符是否是转义*/
            BOOLEAN bPreCharIsEndMark = FALSE; /*前一个字符是否是引号*/

            /*先读入开头的引号*/
            pCmdLine->sCmdWord[wWordSum][wCharIndex] = *pCurChar;
            pCurChar++;
            wCharIndex++;

            if (! bIsWord)
            {
                bIsWord = TRUE; /*true */
            }

            while (*pCurChar != MARK_STRINGEND
                    && wCharIndex < (MAX_STRING_LEN-1))
            {
                /*如果读到非转义的引号加空格，结束当前单词读取*/
                if (bPreCharIsEndMark)
                {
                    if (*pCurChar ==  MARK_BLANK) 
                    {
                        break;
                    }
                    else
                    {/*如果引号接的不是空格，继续读*/
                        bPreCharIsEndMark = FALSE;
                    }
                }
                
                pCmdLine->sCmdWord[wWordSum][wCharIndex] = *pCurChar;

                if (MARK_TYPE_STRING == *pCurChar)
                {/*如果引号前不是转义符，作为引号*/
                    bPreCharIsEndMark = (bPreCharIsTrans) ? FALSE : TRUE;
                    if (bPreCharIsTrans)
                    {
                        bPreCharIsTrans = FALSE;
                    }
                }
                else if (MARK_STRING_TRANS == *pCurChar)
                {/*如果转义符之前不是转义符，作为转义符*/
                    bPreCharIsTrans = (bPreCharIsTrans) ? FALSE : TRUE;
                }
                else 
                {
                    /*读到其他普通字符，转义符复位*/
                    if (bPreCharIsTrans)
                    {
                        bPreCharIsTrans = FALSE;
                    }
                }
                
                wCharIndex++;
                pCurChar++;
            }
            if (*pCurChar == MARK_TYPE_STRING && wCharIndex < MAX_STRING_LEN)
            {
                pCurChar++;
            }
        }
        else
        {
            while (*pCurChar != MARK_STRINGEND
                    && *pCurChar != MARK_BLANK
                    && wCharIndex < (MAX_STRING_LEN - 1))
            {
                if((wWordSum >= MAX_LAYER) || (wCharIndex >= MAX_STRING_LEN))
                {
                  pCmdLine->wWordSum = wWordSum;
		    return;
	        }
                pCmdLine->sCmdWord[wWordSum][wCharIndex] = *pCurChar;
                wCharIndex++;
                pCurChar++;
                if (! bIsWord)
                    bIsWord = TRUE; /*true */
            }
        }
        if((wWordSum >= MAX_LAYER) || (wCharIndex >= MAX_STRING_LEN))
	 {		
              pCmdLine->wWordSum = wWordSum;
		return;
	 }
        if (wCharIndex == MAX_STRING_LEN)
            pCmdLine->sCmdWord[wWordSum][MAX_STRING_LEN - 1] = MARK_STRINGEND;
        else
            pCmdLine->sCmdWord[wWordSum][wCharIndex] = MARK_STRINGEND;
        if (bIsWord)
            wWordSum++; /* ???? if "    " or word + ' ' */
    }

    if (wWordSum > 0)
        /**此处是全字匹配, 即用户输入的第一个命令字是no, 则该命令一定是no命令*/
        /**特别注意: 用户输入的第一个命令字不是no却并不意味着该命令一定不是no命令(例如n),
                     这需要等到命令字匹配完成以后才能确定是否真的no命令*/
        if (StrCmpNoCase(pCmdLine->sCmdWord[0], STR_CMD_NO) == 0)
            pCmdLine->bHasNo = TRUE;
    pCmdLine->wWordSum = wWordSum;
}

/* clear the match result */
void CleanMatchResult(TYPE_MATCH_RESULT *pMatchResult)
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

/* deal with the matching result */
void PerformMatchResult(TYPE_LINK_STATE *pLinkState)
{
    TYPE_MATCH_RESULT *pMatchResult = &(pLinkState->tMatchResult);
    WORD16 wCmdAttrPos = pMatchResult->wCmdAttrPos;
    T_OAM_Cfg_Dat* cfgDat = pMatchResult->cfgDat;
    BYTE bStopAttr = CEASE_ATTR_CANCEASE_NOACK;
    TYPE_CEASE_MSG tCeaseMsg;

    pLinkState->wCmdAttrPos = wCmdAttrPos;

    /* 查看用户不能执行SET类命令及存盘命令 */
    if ((EXEPOS_MP == cfgDat->tCmdAttr[wCmdAttrPos].bExePos) ||
         IsSaveCmd(cfgDat->tCmdAttr[wCmdAttrPos].dExeID))
    {
        if (!HasPermissionExecuteCmd(pLinkState))
        {
            SendStringToTelnet(pLinkState, Hdr_szNoPermission);
            return;
        }
    }

    /*将当前命令的中止属性发往TELSERVER*/
    if (CMD_ATTR_CANCEASE(&cfgDat->tCmdAttr[pLinkState->wCmdAttrPos]))
    {
        if (CMD_ATTR_CANCEASE_NEEDACK(&cfgDat->tCmdAttr[pLinkState->wCmdAttrPos]))
            bStopAttr = CEASE_ATTR_CANCEASE_NEEDACK;
        else
            bStopAttr = CEASE_ATTR_CANCEASE_NOACK;
    }
    else
    {
        bStopAttr = CEASE_ATTR_CANNOTCEASE;
    }
    memset(&tCeaseMsg, 0, sizeof(TYPE_CEASE_MSG));
    tCeaseMsg.bVtyNum = pLinkState->ucLinkID;
    tCeaseMsg.bStopAttr = bStopAttr;
    XOS_SendAsynMsg(OAM_MSG_TO_TELSERVER_OF_CEASE_ATTR, (BYTE*)&tCeaseMsg, sizeof(TYPE_CEASE_MSG),
        0, 0,&g_ptOamIntVar->jidTelnet);

    if(cfgDat->tCmdAttr[wCmdAttrPos].bExePos == EXEPOS_XMLRPC)
    {
        pLinkState->CmdRestartFlagForXmlRpc = TRUE;
        DisposeXmlRpcCmd(pLinkState);
    }
    else if(cfgDat->tCmdAttr[wCmdAttrPos].bExePos == EXEPOS_INTERPRETATION)
    {
        DisposeInterpretPosCmd(pLinkState);
    }
    else if(pLinkState->tMatchResult.cfgDat->tJid.dwJno   == g_ptOamIntVar->jidSelf.dwJno)
    {
        DisposeInnerCmd(pLinkState);    
    }
    else
    {
        DisposeMPCmd(pLinkState);    
    }    
	#if 0
    switch(cfgDat->tCmdAttr[wCmdAttrPos].bExePos)
    {
    case EXEPOS_INTERPRETATION:
        DisposeInnerCmd(pLinkState);
        break;
    case EXEPOS_MP:
        DisposeMPCmd(pLinkState);
        break;
    default:
        break;
    }
	#endif
    return;
}

/* match a single command line in the command tree */
void MatchCmdLine(  TYPE_LINK_STATE *pLinkState,
                    TYPE_CMD_LINE   *pCmdLine,
                    TYPE_MATCH_RESULT *pMatchResult)
{
    WORD16 i = 0;
    WORD16 wSearchPos     = 0;
    WORD16 wTreeNodePos   = 0;
    WORD16 wCmdAttrPos    = 0;
    WORD16 wTraceDepth    = 0; /* trace start depth */

    int nCmpResult = 0; /* 0, -1, 1 */
    WORD16 wMatchSum = 0;
    WORD16 wCurDepth = 0;
    WORD16 wMatchFirstIndex = 0;
    WORD16 wOldWordMatchLen = 0;
    WORD16 wWordMatchLen = 0;

    TYPE_MODE *pCurMode = NULL;

    /* change parsing state of matching result */
    pMatchResult->wParseState = PARSESTATE_WORD_PARSING;

    /* determine the command tree of the current mode */
    pCurMode = (TYPE_MODE *)Oam_GetCurMode(pLinkState);
    if (!pCurMode)
        return ; /* current mode in the link state is corruptted ????*/

    wSearchPos = pCurMode->wTreeRootPos;
	
    /* Match the command words */
    /* ..matching algorithm */
    while (wSearchPos)
    {
        if (wCurDepth >= pCmdLine->wWordSum)
            break;
        /* get the tree node that need to match */
        /* follow function use it to determine whether insert "no" */
        pMatchResult->wDepth = wCurDepth;
        GatherTreeNode(pLinkState, wSearchPos);
        if( pMatchResult->wParseState == PARSESTATE_WORD_EXCEPTION)
        {
            return;
        }
        /* reset match position, old and new match len */
        wMatchFirstIndex = 0;
        wOldWordMatchLen = 0;
        wWordMatchLen = 0;
        /* no subtree node need to match and not change wCurDepth */
        if (pMatchResult->wHintStackTop == 0)
            break;
        /* next layer */
        if (wMatchSum == 1)
        {
            wMatchSum = 0;
            wMatchFirstIndex = 0;
        }

        /* match in a layer */
        for (i = 0; i < pMatchResult->wHintStackTop; i++)
        {
            /* match */
            wSearchPos = pMatchResult->tHint[i].wPosition;
            nCmpResult = MatchTwoString(pCmdLine->sCmdWord[wCurDepth],
                                        pMatchResult->tHint[i].pName, &wWordMatchLen);
            /* record match result */
            if (wOldWordMatchLen < wWordMatchLen)
            {
                wOldWordMatchLen = wWordMatchLen;
                if (wMatchSum == 0)
                    wMatchFirstIndex = i;
            }
            /* deal according to the match result */
            if (nCmpResult < 0)
                break; /* stop matching */
            /* else (nCmpResult >= 0) */
            if (nCmpResult == 0)
                wMatchSum++;
        }/* end of matching in a layer */
        /* next layer or break */
        if (wMatchSum == 0)
        {
            break;
        }
        else if (wMatchSum > 1)
        {
            switch(pCmdLine->bDeterminer)
            {
            case DM_QUESTION:
            case DM_TAB:
            case DM_NORMAL:
            case DM_BLANKQUESTION:
                if (pCmdLine->bDeterminer == DM_QUESTION && wCurDepth == pCmdLine->wWordSum - 1)  /**word?需要模糊匹配*/
                    break;

                /**word ?/word+TAB/word 都需要完全匹配*/
                nCmpResult = StrCmpNoCase(pCmdLine->sCmdWord[wCurDepth], pMatchResult->tHint[wMatchFirstIndex].pName);
                if (nCmpResult == 0) /* the first matchs absolutely */
                    wMatchSum = 1;
                break;
            default:
                break;
            }
        }
		
        if (wMatchSum == 1)
        {
            /* for "n ?" */
            if (StrCmpNoCase(pMatchResult->tHint[wMatchFirstIndex].pName, STR_CMD_NO) == 0)
                pCmdLine->bHasNo = TRUE;

            /* next searching position */
            wSearchPos = pMatchResult->tHint[wMatchFirstIndex].wPosition;
            /* save the matching path */
            pMatchResult->tPath[pMatchResult->wPathStackTop].wPosition  = wSearchPos;
            pMatchResult->tPath[pMatchResult->wPathStackTop].wType      = PATH_COMMAND;
            pMatchResult->tPath[pMatchResult->wPathStackTop].cfgDat     = pMatchResult->tHint[wMatchFirstIndex].cfgDat;
            pMatchResult->wPathStackTop++;
            /* new matching depth */
            wCurDepth++;
        }
        else /* wMatchSum > 1 */
        {
            wCurDepth++;
            break;
        }
    }/* end of "while (wSearchPos)"*/

    /* check the match result */
    /* trace the matching paths and decide whether it has parameters to match */
    if ( (wMatchSum == 1 && wCurDepth < pCmdLine->wWordSum)  || 
          (wMatchSum == 0 /*&& wWordMatchLen <= 1 */&& 
            wCurDepth > 0 && 
            wCurDepth <= pCmdLine->wWordSum) )
    {
        wTraceDepth = pMatchResult->wPathStackTop;
        while(wTraceDepth > 0)
        {
            T_OAM_Cfg_Dat* cfgDat = pMatchResult->tPath[wTraceDepth - 1].cfgDat;
            wTreeNodePos = pMatchResult->tPath[wTraceDepth - 1].wPosition;
            if (gtTreeNode[wTreeNodePos].cfgDat && gtTreeNode[wTreeNodePos].cfgDat->bIsValid)
            {
                /* ??? need check the type of the node of the path */
                wCmdAttrPos = gtTreeNode[wTreeNodePos].treenode.wCmdAttrPos;
            }
            else
            {
                /*如果碰到的是中间节点或因为注销降级来的中间节点*/
                wCmdAttrPos = INVALID_CMDINDEX;
            }
            /* should match parameters */
            if (wCmdAttrPos != INVALID_CMDINDEX
                    && cfgDat->tCmdAttr[wCmdAttrPos].dExeID != INVALID_EXEID
                    && NeedMatchPara(pLinkState, wCmdAttrPos, cfgDat))
                break;
            wTraceDepth--;
        }
        if (wTraceDepth == 0)
        {
            wMatchSum = 0;
            /* wWordMatchLen = 1;*/
            /* wCurDepth = 0; */
        }
        else
        {
            /* calculate wCliMatchLen */
            pMatchResult->wDepth    = wCurDepth;
            pMatchResult->wLength   = wWordMatchLen;
            pMatchResult->wCliMatchLen = CalculateMatchPosition(pLinkState);
            /* change match result */
            wMatchSum = 1;
            wCurDepth = wTraceDepth; /* ???? need change wMatchFirstIndex */
            pMatchResult->wPathStackTop = wTraceDepth;
        }
    }
    /* ..file the match result */
    pMatchResult->wSum      = wMatchSum;
    pMatchResult->wDepth    = wCurDepth;
    pMatchResult->wLength   = wWordMatchLen;
    pMatchResult->wFirst    = wMatchFirstIndex;

    /* save the wCmdAttrPos and change the parsing state */
    if (wMatchSum == 1)
    {
        wTreeNodePos = pMatchResult->tPath[pMatchResult->wPathStackTop - 1].wPosition;
        pMatchResult->wCmdAttrPos = gtTreeNode[wTreeNodePos].treenode.wCmdAttrPos;
        pMatchResult->cfgDat = pMatchResult->tPath[pMatchResult->wPathStackTop - 1].cfgDat;
    }
    switch(pCmdLine->bDeterminer)
    {
    case DM_NORMAL:
        /* ??? need check the type of the node of the path */
        if (wMatchSum == 1)
        {
            if (pMatchResult->wCmdAttrPos == INVALID_CMDINDEX)
                pMatchResult->wParseState = PARSESTATE_WORD_EXCEPTION;
            else
            {
                if (pMatchResult->wDepth == pCmdLine->wWordSum)
                {
                    T_OAM_Cfg_Dat* cfgDat = pMatchResult->cfgDat;
                    DWORD dExeID = cfgDat->tCmdAttr[pMatchResult->wCmdAttrPos].dExeID;
                    if (dExeID == INVALID_EXEID)
                        pMatchResult->wParseState = PARSESTATE_WORD_EXCEPTION;
                    else
                    {
                        if (NeedMatchPara(pLinkState, pMatchResult->wCmdAttrPos, pMatchResult->cfgDat))
                        {
                            pMatchResult->wParseState = PARSESTATE_WORD_PARSED;
                        }
                        else
                        {
                            pMatchResult->dExeID = dExeID;
                            pMatchResult->wParseState = PARSESTATE_FINISH_PARSE;
                        }
                    }
                }
                else
                    pMatchResult->wParseState = PARSESTATE_WORD_PARSED;
            }
        }
        else
            pMatchResult->wParseState = PARSESTATE_WORD_EXCEPTION;
        break;
    case DM_TAB:
    case DM_QUESTION:
    case DM_BLANKQUESTION:
        if (pMatchResult->wDepth == pCmdLine->wWordSum)
            pMatchResult->wParseState = PARSESTATE_WORD_DETERMINER;
        else
        {
            if (pMatchResult->wSum == 1) /* need match the parameter */
                pMatchResult->wParseState = PARSESTATE_WORD_PARSED;
            else
                pMatchResult->wParseState = PARSESTATE_WORD_EXCEPTION;
        }
        break;
    default:
        break;
    }
    /* others */
    if (pMatchResult->wParseState == PARSESTATE_WORD_PARSING)
        pMatchResult->wParseState = PARSESTATE_WORD_EXCEPTION;
    return;
}

/*判断节点是否合法；如果节点属性里面是无效的，还要
进一步判断是否有合法的子节点，如果有，判断是否有合法子节点
有的话，当前节点合法，但是不是命令节点，是中间节点*/
static BOOLEAN TreeNodeIsValid(T_TreeNode *pTreeNode)
{
    BOOLEAN bRet = FALSE;
    TYPE_CMD_ATTR *pCmdAttr = NULL;

    if ((!pTreeNode) || (!pTreeNode->cfgDat) || (!pTreeNode->cfgDat->tCmdAttr))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                           "TreeNodeIsValid: NULL pointer in pTreeNode!\n");
        return FALSE;
    }

    pCmdAttr = &(pTreeNode->cfgDat->tCmdAttr[pTreeNode->treenode.wCmdAttrPos]);
    if (!pCmdAttr)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                           "TreeNodeIsValid: pCmdAttr is NULL pointer !\n");
        return FALSE;
    }
    
    if (pTreeNode->cfgDat->bIsValid)
    {
        bRet = TRUE;
    }
    else if (EXEPOS_INTERPRETATION == pCmdAttr->bExePos)
    {
        bRet = TRUE;
    }
    else if (INVALID_NODE == pTreeNode->treenode.wNextSonPos)
    {
        bRet = FALSE;
    }
    else
    {
        WORD16 wTmpPos = 0;
        wTmpPos = pTreeNode->treenode.wNextSonPos;
        while (INVALID_NODE != wTmpPos)
        {
            if (TreeNodeIsValid(&gtTreeNode[wTmpPos]))
            {
                break;
            }
            wTmpPos = gtTreeNode[wTmpPos].treenode.wNextNodePos;
        }
        if (INVALID_NODE == wTmpPos)
        {
            bRet = FALSE;
        }
        else
        {
            bRet = TRUE;
        }
    }

    return bRet;
}

/* get the tree node that need to match */
/* The main purpose is to get the match tree from the command script */
/* In order to this purpose, need insert the "no", "defaut" into it */
void GatherTreeNode(TYPE_LINK_STATE *pLinkState, WORD16 wFatherPos)
{
    TYPE_MATCH_RESULT *pMatchResult = &(pLinkState->tMatchResult);
    TYPE_CMD_LINE *pCmdLine = (&pLinkState->tCmdLine);
    int i = 0, j = 0;
    WORD16 wGatherPos = 0;
    BOOLEAN bNeedInsertNo = FALSE;
               

    /* reset the tHint */
    for (i = 0; i < pMatchResult->wHintStackTop; i++)
    {
        pMatchResult->tHint[i].pName        = NULL;
        pMatchResult->tHint[i].pComment     = NULL;
        pMatchResult->tHint[i].wType        = PATH_INVALID;
        pMatchResult->tHint[i].wPosition    = INVALID_NODE;
        pMatchResult->tHint[i].cfgDat       = NULL;
    }
    pMatchResult->wHintStackTop = 0;

    /* check the wFatherPos */
    if (wFatherPos == INVALID_NODE)
        return;
    /* set the wGatherPos */
    wGatherPos = gtTreeNode[wFatherPos].treenode.wNextSonPos;

    /* gather tree node when cmd */
    if ((!pCmdLine->bHasNo) || (pMatchResult->wDepth == 0 && pMatchResult->bHasNo))
    {
        /* gather */
        i = 0;
        while(wGatherPos != INVALID_NODE)
        {
            if (TreeNodeIsValid(&gtTreeNode[wGatherPos]))
            {
                 /* save */
                pMatchResult->tHint[i].pName        = gtTreeNode[wGatherPos].treenode.sName;
                pMatchResult->tHint[i].pComment     = gtTreeNode[wGatherPos].treenode.sComment;
                pMatchResult->tHint[i].wType        = PATH_COMMAND;
                pMatchResult->tHint[i].wPosition    = wGatherPos;
                pMatchResult->tHint[i].cfgDat       = gtTreeNode[wGatherPos].cfgDat;
                
                if (!bNeedInsertNo &&
                     (pMatchResult->wDepth == 0)  &&
                     (HasTreeNodeNoCmd(wGatherPos)))
                {
                    bNeedInsertNo = TRUE;
                }
		
                 /* next */
                i++;
            }
            wGatherPos = gtTreeNode[wGatherPos].treenode.wNextNodePos;
        }

        pMatchResult->wHintStackTop = i;
        if (pMatchResult->wHintStackTop > MAX_HINTSUM)
        {
            pMatchResult->wParseState = PARSESTATE_WORD_EXCEPTION;
            return;
        }

        /* insert "no" into the set */
        /* for "no" */
        if (bNeedInsertNo)
        {
            /* find the insert position */
            for (j = 0; j < i; j++)
            {
                if (StrCmpNoCase(pMatchResult->tHint[j].pName, STR_CMD_NO) > 0)
                    break;
            }
            /* move the hints */
            pMatchResult->wHintStackTop++; /**增加1个节点*/
            if (pMatchResult->wHintStackTop > MAX_HINTSUM)
            {
                pMatchResult->wParseState = PARSESTATE_WORD_EXCEPTION;
                return;
            }
			
            for (i = pMatchResult->wHintStackTop - 1; i > j; i--) /**将比STR_CMD_NO大的节点向后移*/
            {
                pMatchResult->tHint[i].pName        = pMatchResult->tHint[i - 1].pName;
                pMatchResult->tHint[i].pComment     = pMatchResult->tHint[i - 1].pComment;
                pMatchResult->tHint[i].wType        = pMatchResult->tHint[i - 1].wType;
                pMatchResult->tHint[i].wPosition    = pMatchResult->tHint[i - 1].wPosition;
                pMatchResult->tHint[i].cfgDat       = pMatchResult->tHint[i - 1].cfgDat;
            }
            /* insert */
            pMatchResult->tHint[j].pName        = STR_CMD_NO;
            pMatchResult->tHint[j].pComment     = Hdr_szCmdNoCmdTip;
            pMatchResult->tHint[i].wType        = PATH_COMMAND;
            pMatchResult->tHint[j].wPosition    = wFatherPos;
            pMatchResult->tHint[i].cfgDat       = NULL;
        }/* end of "if (bNeedNo)" */
    }/* end of "if (!pCmdLine->bHasNo)" */

    /* gather tree node when NO cmd */
    if (pCmdLine->bHasNo && pMatchResult->wDepth >= 1)
    {
        i = 0;
        while (wGatherPos != INVALID_NODE)
        {
            /* check and save */
            if (HasTreeNodeNoCmd(wGatherPos))
            {
                if (TreeNodeIsValid(&gtTreeNode[wGatherPos]))
                {
                    /* save */
                    pMatchResult->tHint[i].pName       = gtTreeNode[wGatherPos].treenode.sName;
                    pMatchResult->tHint[i].pComment    = gtTreeNode[wGatherPos].treenode.sComment;
                    pMatchResult->tHint[i].wType       = PATH_COMMAND;
                    pMatchResult->tHint[i].wPosition   = wGatherPos;
                    pMatchResult->tHint[i].cfgDat      = gtTreeNode[wGatherPos].cfgDat;
                    /* next */
                    i++;
                }
            }
            /* next */
            wGatherPos = gtTreeNode[wGatherPos].treenode.wNextNodePos;
        }
        pMatchResult->wHintStackTop = i;
        if (pMatchResult->wHintStackTop > MAX_HINTSUM)
        {
            pMatchResult->wParseState = PARSESTATE_WORD_EXCEPTION;
            return;
        }
    }
}

/* check the tree node whether it has no cmd */
BOOL HasTreeNodeNoCmd(WORD16 wCheckPos)
{
    BOOL bReturnVal = FALSE;
    WORD16 wCmdAttrPos = gtTreeNode[wCheckPos].treenode.wCmdAttrPos;
    WORD16 wNextCheckPos = INVALID_CMDINDEX;

    /* check this node */
    if (wCmdAttrPos != INVALID_CMDINDEX)
    {
        T_OAM_Cfg_Dat * cfgDat = gtTreeNode[wCheckPos].cfgDat;
        if (cfgDat->tCmdAttr[wCmdAttrPos].dExeID != INVALID_EXEID &&
                CMD_ATTR_HASNO(&cfgDat->tCmdAttr[wCmdAttrPos]))
            return TRUE;
    }

    /* check the sons of this node */
    wNextCheckPos = gtTreeNode[wCheckPos].treenode.wNextSonPos;
    while(wNextCheckPos != INVALID_NODE)
    {
        bReturnVal = HasTreeNodeNoCmd(wNextCheckPos);
        if (bReturnVal)
            break;
        wNextCheckPos = gtTreeNode[wNextCheckPos].treenode.wNextNodePos;
    }

    return bReturnVal;
}


/* deal with the determiners such as "?" and "TAB" flowering after a command word */
void DealCmdDeterminer(TYPE_LINK_STATE  *pLinkState)
{
    TYPE_CMD_LINE *pCmdLine = &(pLinkState->tCmdLine);
    TYPE_MATCH_RESULT *pMatchResult = &(pLinkState->tMatchResult);
    int i = 0;
    WORD16 wTreeNodePos = INVALID_NODE;

    *pTempStr1 = MARK_STRINGEND;
    switch(pCmdLine->bDeterminer)
    {
    case DM_QUESTION:   /* word?    */
        pLinkState->bOutputMode     = OUTPUT_MODE_NORMAL;
        /* reconform the command line and send to telnet */
        switch(pMatchResult->wSum)
        {
        case 0:
            if (pCmdLine->wWordSum == 0) /* prompt + ?*/
            {
                TYPE_MODE *pCurMode = (TYPE_MODE *)Oam_GetCurMode(pLinkState);
                if (!pCurMode)
                {
                    return;
                }
                /* list the name and the comment of the commands */
                CombineNameComment(pLinkState,
                                   pCurMode->wTreeRootPos, pLinkState->sOutputResult);
                pLinkState->bOutputResult = TRUE;
                /* reconform the command line */
                memset(pLinkState->sCmdLine, MARK_BLANK, (size_t)pCmdLine->wBlankSum[0]);
                pLinkState->sCmdLine[pCmdLine->wBlankSum[0]] = MARK_STRINGEND;
                pLinkState->bOutputCmdLine = TRUE;
            }
            else
            {

                /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
                XOS_snprintf(pLinkState->sOutputResult,
                             MAX_OUTPUT_RESULT - 1,
                             "%s\n",
                             CLI_szCLIHelp0);

                pLinkState->bOutputResult = TRUE;
            }
            break;
        default: /* 1 and more */
            for (i = 0; i < pMatchResult->wSum; i++)
            {
                /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
                XOS_snprintf(pLinkState->sOutputResult,
                             MAX_OUTPUT_RESULT - 1,
                             "%s%s  ",
                             pLinkState->sOutputResult,
                             pMatchResult->tHint[pMatchResult->wFirst + i].pName);
            }
            XOS_snprintf(pLinkState->sOutputResult,
                         MAX_OUTPUT_RESULT - 1,
                         "%s\n",
                         pLinkState->sOutputResult);

            pLinkState->bOutputResult = TRUE;
            /* reconform the command line */
            CombineCmdLine(pCmdLine, TRUE, pLinkState->sCmdLine, MAX_CMDLINE_LEN);
            pLinkState->bOutputCmdLine = TRUE;
            break;
        }
        break;

    case DM_BLANKQUESTION:  /* word ?   */
    	  pLinkState->bOutputMode     = OUTPUT_MODE_NORMAL;
        /* list the next layer command word and their comment */
        if (pCmdLine->wWordSum == 0  ||
                (pCmdLine->bHasNo && pMatchResult->wDepth == 1) ) /* prompt + ?*/
        {
            TYPE_MODE *pCurMode = (TYPE_MODE *)Oam_GetCurMode(pLinkState);
            if (!pCurMode)
            {
                return;
            }
            /* list the name and the comment of the commands */
            CombineNameComment(pLinkState,
                               pCurMode->wTreeRootPos, pLinkState->sOutputResult);
            pLinkState->bOutputResult = TRUE;
            /* reconform the command line */
            if (!pCmdLine->bHasNo)
            {
                memset(pLinkState->sCmdLine, MARK_BLANK, (size_t)pCmdLine->wBlankSum[0]);
                pLinkState->sCmdLine[pCmdLine->wBlankSum[0]] = MARK_STRINGEND;
            }
            else
                CombineCmdLine(pCmdLine, TRUE, pLinkState->sCmdLine, MAX_CMDLINE_LEN);
            pLinkState->bOutputCmdLine = TRUE;
            break;
        }
        switch(pMatchResult->wSum)
        {
        case 0:
            break;
        case 1:
            wTreeNodePos = pMatchResult->tHint[pMatchResult->wFirst].wPosition;
            /* list the name and the comment of the commands */
            CombineNameComment(pLinkState,
                               wTreeNodePos, pLinkState->sOutputResult);   /*????无wFirst的值 */
            pLinkState->bOutputResult = TRUE;
            /* reconform the command line */
            CombineCmdLine(pCmdLine, TRUE, pLinkState->sCmdLine, MAX_CMDLINE_LEN);
            pLinkState->bOutputCmdLine = TRUE;
            break;
        default:    /* Ambiguous */
            /*对于多匹配，wDepth应该回退(在匹配时wDepth多加1)*/
            if (pMatchResult->wSum > 1 && pMatchResult->wDepth > 0)
                pMatchResult->wDepth --;
            i = CalculateMatchPosition(pLinkState);
            i += strlen(pLinkState->sPrompt);
            memset(pTempStr1, MARK_BLANK, (size_t)(i - 1));
            pTempStr1[i - 1] = '^';
            pTempStr1[i] = MARK_STRINGEND;
            /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
            XOS_snprintf(pLinkState->sOutputResult,
                         MAX_OUTPUT_RESULT - 1,
                         "%s%s\n%s\n",
                         pLinkState->sOutputResult,
                         pTempStr1,
                         CLI_szCLIHelp2);

            pLinkState->tMatchResult.wParseState = PARSESTATE_ASK_TELNET;
            return;

        }
        break;

    case DM_TAB:        /* word+TAB */
        switch(pMatchResult->wSum)
        {
        case 0:
            break;
        case 1:
            CombineCmdLine(pCmdLine, FALSE, pLinkState->sCmdLine, MAX_CMDLINE_LEN);
            /* for "prompt>no + TAB" */
            XOS_snprintf(pLinkState->sCmdLine, 
                         MAX_CMDLINE_LEN - 1, 
                         "%s%s ", 
                         pLinkState->sCmdLine,
                         pMatchResult->tHint[pMatchResult->wFirst].pName);
            
            pLinkState->bOutputCmdLine = TRUE;
            break;
        default:
            CombineCmdLine(pCmdLine, FALSE, pLinkState->sCmdLine, MAX_CMDLINE_LEN);
            /* for "prompt>no + TAB" */
            CalculateWordTabMax(pMatchResult, pTempStr1);
            XOS_snprintf(pLinkState->sCmdLine, 
                         MAX_CMDLINE_LEN - 1, 
                         "%s%s", 
                         pLinkState->sCmdLine,
                         pTempStr1);
            
            pLinkState->bOutputCmdLine = TRUE;
            break;
        }
        break;

    case DM_NORMAL: /* process error */
        switch(pMatchResult->wSum)
        {
        case 0:     /* Translating \"   ???? */
            i = CalculateMatchPosition(pLinkState);
            i += strlen(pLinkState->sPrompt);
            memset(pTempStr1, MARK_BLANK, (size_t)(i - 1));
            pTempStr1[i - 1] = '^';
            pTempStr1[i] = MARK_STRINGEND;

            /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
            XOS_snprintf(pLinkState->sOutputResult,
                         MAX_OUTPUT_RESULT - 1,
                         "%s%s\n%s\n",
                         pLinkState->sOutputResult,
                         pTempStr1,
                         CLI_szCLIHelp2);

            break;
        case 1:     /* match but not a command  */
            if (pMatchResult->wCmdAttrPos == 0)
            {
                CombineCmdLine(pCmdLine, TRUE, pTempStr1,sizeof(pTempStr1));
                XOS_snprintf(pTempStr1, MAX_STRING_LEN - 1, "%s ?", pTempStr1);
                XOS_snprintf(pLinkState->sOutputResult, MAX_OUTPUT_RESULT, CLI_szCLIHelp3, pTempStr1);

                /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
                XOS_snprintf(pLinkState->sOutputResult,
                             MAX_OUTPUT_RESULT - 1,
                             "%s\n",
                             pLinkState->sOutputResult);

            }
            break;
        default:    /* Ambiguous */
            CombineCmdLine(pCmdLine, TRUE, pTempStr1,sizeof(pTempStr1));
            /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
            XOS_snprintf(pLinkState->sOutputResult,
                         MAX_OUTPUT_RESULT - 1,
                         "%s%s \"%s\"\n",
                         pLinkState->sOutputResult,
                         CLI_szCLIHelp1,
                         pTempStr1);

            break;
        }
        if (*(pLinkState->sOutputResult) != MARK_STRINGEND)
            pLinkState->bOutputResult = TRUE;
        break;

    default:
        break;
    }

    /* change the parse state */
    pMatchResult->wParseState = PARSESTATE_FINISH_DEAL;
}

/* deal the exception about the command word */
void DealCmdException(TYPE_LINK_STATE *pLinkState)
{
    TYPE_CMD_LINE *pCmdLine = &(pLinkState->tCmdLine);
    TYPE_MATCH_RESULT *pMatchResult = &(pLinkState->tMatchResult);
    int i = 0;

    *pTempStr1 = MARK_STRINGEND;
    if (pCmdLine->wWordSum > 0)
    {
        if ((NULL == CLI_szCLIHelp4) || (NULL == CLI_szCLIHelp2))
            return;
        
        switch(pCmdLine->bDeterminer)
        {
        case DM_QUESTION:
        case DM_BLANKQUESTION:
        case DM_TAB:
        case DM_NORMAL:
            switch(pMatchResult->wSum)
            {

            case 1:
                /* an incomplete command */
                /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
                XOS_snprintf(pLinkState->sOutputResult,
                             MAX_OUTPUT_RESULT - 1,
                             "%s\n",
                             CLI_szCLIHelp4);

                pLinkState->bOutputResult = TRUE;
                break;
            default:
                /**对于多匹配，wDepth应该回退(在匹配时wDepth多加1)*/
                if (pMatchResult->wSum > 1 && pMatchResult->wDepth > 0)
                    pMatchResult->wDepth --;

                i = CalculateMatchPosition(pLinkState);
                i += strlen(pLinkState->sPrompt);
                memset(pTempStr1, MARK_BLANK, (size_t)(i - 1));
                pTempStr1[i - 1] = '^';
                pTempStr1[i] = MARK_STRINGEND;
                /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
                XOS_snprintf(pLinkState->sOutputResult,
                             MAX_OUTPUT_RESULT - 1,
                             "%s%s\n%s\n",
                             pLinkState->sOutputResult,
                             pTempStr1,
                             CLI_szCLIHelp2);

                pLinkState->tMatchResult.wParseState = PARSESTATE_ASK_TELNET;
                break;
            }
            if (*(pLinkState->sOutputResult) != MARK_STRINGEND)
                pLinkState->bOutputResult = TRUE;
            break;
        default:
            break;
        }
    }
        
    /* change the parse state */
    pMatchResult->wParseState = PARSESTATE_FINISH_DEAL;
}

/* whether need to match the parameter */
BOOL NeedMatchPara(TYPE_LINK_STATE *pLinkState, WORD16 wCmdAttrPos, T_OAM_Cfg_Dat* cfgDat)
{
    TYPE_CMD_LINE *pCmdLine = &(pLinkState->tCmdLine);

    /* cfgDat为NULL表示不是命令 */
    if (cfgDat == NULL)
        return FALSE;
    else if ( (!pCmdLine->bHasNo && cfgDat->tCmdAttr[wCmdAttrPos].wCmdParaCount != 0)
      || (pCmdLine->bHasNo && cfgDat->tCmdAttr[wCmdAttrPos].wNoParaCount != 0) )
        return TRUE;
    else
        return FALSE;
}

/* 清除MoreAngle嵌套当前层 */
void ClearMoreLayer(TYPE_MATCH_RESULT *pMatchResult)
{
    if (pMatchResult->bMoreLayerTop > 0)
    {
        --pMatchResult->bMoreLayerTop;
        pMatchResult->wMoreBeginPos[pMatchResult->bMoreLayerTop] = 0;
        pMatchResult->wMoreEndPos[pMatchResult->bMoreLayerTop] = 0;
        pMatchResult->bType[pMatchResult->bMoreLayerTop] = 0;
        pMatchResult->bContinueSearch[pMatchResult->bMoreLayerTop] = FALSE;
        pMatchResult->bMoreFlags[pMatchResult->bMoreLayerTop] = 0;
    }
}

/* deal with the parameters of the command */
void MatchCmdPara(TYPE_LINK_STATE *pLinkState)
{
    TYPE_MATCH_RESULT *pMatchResult = &(pLinkState->tMatchResult);
    TYPE_CMD_LINE *pCmdLine = &(pLinkState->tCmdLine);
    T_OAM_Cfg_Dat * cfgDat = pMatchResult->cfgDat;

    int nResultVal = PARA_SUCCESS;
    WORD16 wCmdAttrPos = pMatchResult->wCmdAttrPos;
    WORD16 wParaBeginPos = pMatchResult->wParaBeginPos;
    WORD16 wParaEndPos = pMatchResult->wParaEndPos;
    BOOLEAN bCmdNoParameter    = TRUE; /* no parameter behind cmd  */

    TYPE_CMD_PARA *pCmdParaInfo = NULL;
    TYPE_PARA     *pParameterInfo  = NULL;

    /* change parsing state of matching result */
    pMatchResult->wParseState = PARSESTATE_PARA_PARSING;

    /* Match the command parameters and convert the string to the parameter value */
    pMatchResult->wCurParaPos = wParaBeginPos;
    if (pMatchResult->wCurParaPos < wParaEndPos)
        pMatchResult->wFirst = 0;
    pMatchResult->wLength   = 1;
    pMatchResult->wMaxDepth     = pMatchResult->wDepth;
    pMatchResult->wMaxLength    = pMatchResult->wLength;
    pMatchResult->wMaxFirst     = pMatchResult->wFirst;
    pMatchResult->wMaxCurParaPos= pMatchResult->wCurParaPos;

    while (pMatchResult->wCurParaPos < wParaEndPos && pMatchResult->wDepth < pCmdLine->wWordSum)
    {
        bCmdNoParameter = FALSE;
        pCmdParaInfo = (TYPE_CMD_PARA *)&(cfgDat->tCmdPara[pMatchResult->wCurParaPos]);
        if(pCmdParaInfo->bStructType == STRUCTTYPE_PRAMMAR )
        {
            if((pCmdParaInfo->uContent.tPrammar.bPrammarType == PRAMMAR_RECYCLE)
                    || (pCmdParaInfo->uContent.tPrammar.bPrammarType == PRAMMAR_MORE_ANGLE))
            {
                pMatchResult->bType[pMatchResult->bMoreLayerTop] = pCmdParaInfo->uContent.tPrammar.bPrammarType;
                /* 记下PRAMMAR_MORE_ANGLE的是否至少选择一项的标志 */
                if(pMatchResult->bType[pMatchResult->bMoreLayerTop] == PRAMMAR_MORE_ANGLE)
                    pMatchResult->bMoreFlags[pMatchResult->bMoreLayerTop] = pCmdParaInfo->uContent.tPrammar.bMoreFlags;
                /* 记下语法{[][][]} 或 recycle的范围 */
                pMatchResult->wMoreBeginPos[pMatchResult->bMoreLayerTop] = pMatchResult->wCurParaPos;
                SkipCurPrammar(pMatchResult);
                pMatchResult->wMoreEndPos[pMatchResult->bMoreLayerTop] = pMatchResult->wCurParaPos;
                pMatchResult->wCurParaPos = pMatchResult->wMoreBeginPos[pMatchResult->bMoreLayerTop];
                ++pMatchResult->bMoreLayerTop;
            }
        }
        else
            pParameterInfo = &(pCmdParaInfo->uContent.tPara);

        nResultVal = MatchOneParameter(pLinkState);
        if(nResultVal != PARA_SUCCESS)
            break;

        if(pMatchResult->wCurParaPos != pMatchResult->wMoreBeginPos[MORE_LAYER_TOP])
            if(pMatchResult->wDepth < pCmdLine->wWordSum)  /* 其他情况 */
                pMatchResult->wCurParaPos++;

        if(pMatchResult->wDepth == pCmdLine->wWordSum)
        {
            pMatchResult->wFirst = pMatchResult->wCurParaPos;
            /* For MORE_ANGLE语法,不在{[][][]}内清栈,防止重进入匹配*/
            if((pMatchResult->wFirst > pMatchResult->wMoreEndPos[MORE_LAYER_TOP]
                    || cfgDat->tCmdPara[pMatchResult->wFirst + 1].uContent.tPrammar.bPrammarType == PRAMMAR_MORE_ANGLE)
                    && pMatchResult->wMatchedPathTop)
                pMatchResult->wMatchedPathTop = 0;
            break;
        }
    }

    /* all parameter are matched */
    /* save the dExeID and change the parsing state */
    switch(pCmdLine->bDeterminer)
    {
    case DM_NORMAL:
        nResultVal = PARA_SUCCESS;
        if(bCmdNoParameter) /* no parameters behind the command */
            pMatchResult->wCurParaPos--;
        if(pMatchResult->bPrammarRecycle && pMatchResult->wCurParaPos == pMatchResult->wMoreBeginPos[0])
            JumpNestingPrammar(pMatchResult, wParaBeginPos, wParaEndPos);

        if (pMatchResult->wDepth == pCmdLine->wWordSum)
        {
            while((pMatchResult->wCurParaPos + 1) < wParaEndPos)
            {
                pMatchResult->wCurParaPos++;
                nResultVal = FindDefaultParameter(pLinkState);
                if(nResultVal != PARA_SUCCESS)
                    break;
                else
                    JumpNestingPrammar(pMatchResult,wParaBeginPos,wParaEndPos);
            }
            if(nResultVal == PARA_SUCCESS && pMatchResult->wSum == 1)
            {
                pMatchResult->dExeID      = cfgDat->tCmdAttr[wCmdAttrPos].dExeID;
                pMatchResult->wParseState = PARSESTATE_FINISH_PARSE;
            }
        }
        else
            pMatchResult->wParseState = PARSESTATE_PARA_EXCEPTION;
        break;

    case DM_BLANKQUESTION:
        if (pMatchResult->wDepth == pCmdLine->wWordSum)
            pMatchResult->wParseState = PARSESTATE_PARA_DETERMINER;
        else
        {
            pMatchResult->wParseState = PARSESTATE_PARA_EXCEPTION;
            pMatchResult->wSum = 0;
        }
        break;

    case DM_TAB:
        if(pMatchResult->wSum == 1)
            pMatchResult->wFirst = pMatchResult->tPath[pMatchResult->wPathStackTop - 1].wPosition;
    case DM_QUESTION:		
        if (!pCmdParaInfo)
		{
            return;
		}
        /* IP 地址（掩码）处理 ，显示该字符串*/
        if(pCmdParaInfo->bStructType == STRUCTTYPE_PARA && nResultVal == PARAERR_NOFIND &&
                pMatchResult->wDepth == pCmdLine->wWordSum - 1)
        {
            pMatchResult->wDepth++;
        }

        if (pMatchResult->wDepth == pCmdLine->wWordSum)
            pMatchResult->wParseState = PARSESTATE_PARA_DETERMINER;
        else
            pMatchResult->wParseState = PARSESTATE_PARA_EXCEPTION;
        break;
    default:
        break;
    }
    if (pMatchResult->wParseState == PARSESTATE_PARA_PARSING)
        pMatchResult->wParseState = PARSESTATE_PARA_EXCEPTION;
}

int MatchOneParameter(TYPE_LINK_STATE *pLinkState)
{
    TYPE_MATCH_RESULT *pMatchResult = &(pLinkState->tMatchResult);
    TYPE_CMD_LINE *pCmdLine = &(pLinkState->tCmdLine);
    T_OAM_Cfg_Dat* cfgDat = pMatchResult->cfgDat;

    CMDPARA       *pCmdPara = NULL; /* store one matched parameter */
    TYPE_CMD_PARA *pCmdParaInfo = NULL;

    WORD16 wParaBeginPos = pMatchResult->wParaBeginPos;
    WORD16 wParaEndPos = pMatchResult->wParaEndPos;
    WORD16 wPosition = 0;         /* 匹配正确的参数位置 */
    WORD16 wFirstMatchIndex = 0;  /* 记录(pMatchResult->wSum > 1) 的第一个位置 */
    BOOLEAN bMapFullMatch = FALSE;
    WORD16 wIntableValue = 0;     /* 记录匹配表中的数据值 */

    int i = 0, j = 0, k = 0, tmp = 0;
    int nCmpResult;
    int nResultVal = PARA_INIT;
    WORD16 wMatchedParaPos = 0;
    WORD16 wLayerParaSum = 0, wPrevParaPos;

    pMatchResult->wSum      = 0;
    pMatchResult->wLength   = 1;
    pMatchResult->wMaxLength= 1;
    /* reset the tHint */
    for (i = 0; i < pMatchResult->wHintStackTop; i++)
    {
        pMatchResult->tHint[i].pName        = NULL;
        pMatchResult->tHint[i].pComment = NULL;
        pMatchResult->tHint[i].wType        = PATH_INVALID;
        pMatchResult->tHint[i].wPosition    = INVALID_NODE;
    }
    pMatchResult->wHintStackTop = 0;

    pCmdParaInfo = (TYPE_CMD_PARA *)&(cfgDat->tCmdPara[pMatchResult->wCurParaPos]);
    /* gather current layer's parameters */
    GatherParameter(pLinkState, (WORD16)(pMatchResult->wCurParaPos - 1), wParaEndPos, FALSE);

    /* begin match parameter in current layer*/
    for (i = 0; i < pMatchResult->wHintStackTop; i++)
    {
        wPrevParaPos = pMatchResult->wParaSum;
        pMatchResult->tHint[i].bMatched = FALSE;

        pMatchResult->wCurParaPos = pMatchResult->tHint[i].wPosition;
        if(pMatchResult->tHint[i].wType == 0)   /* not DATA_TYPE_MAP */
        {
            pCmdParaInfo = (TYPE_CMD_PARA *) &(cfgDat->tCmdPara[pMatchResult->wCurParaPos]);
            if(pCmdParaInfo->bStructType == STRUCTTYPE_PARA)
                nResultVal = ConvertStrToCmdPara(pCmdLine->sCmdWord[pMatchResult->wDepth],
                                                 pLinkState, cfgDat);
            else /*  prammar [](or WHOLE),and sSymbol's length > 0 */
                nResultVal = MatchTwoString(pCmdLine->sCmdWord[pMatchResult->wDepth],
                                            pCmdParaInfo->uContent.tPrammar.sSymbol,&(pMatchResult->wLength));
        }
        else /* DATA_TYPE_MAP */
        {
            nResultVal = MatchTwoString(pCmdLine->sCmdWord[pMatchResult->wDepth],
                                        pMatchResult->tHint[i].pName,&(pMatchResult->wLength));

            if(pMatchResult->wMaxLength < pMatchResult->wLength)
                pMatchResult->wMaxLength = pMatchResult->wLength;
        }

        if (nResultVal == PARA_SUCCESS)
        {
            pMatchResult->tHint[i].bMatched = TRUE;
            pMatchResult->wSum++;
            wPosition = pMatchResult->tHint[i].wPosition;
            if(pMatchResult->wSum == 1)
                wFirstMatchIndex = i;

            if(pMatchResult->tHint[i].wType == 0)
            {
                /* save the matching path */
                pMatchResult->tPath[pMatchResult->wPathStackTop].wPosition = wPosition;
                pMatchResult->tPath[pMatchResult->wPathStackTop].wType  = PATH_PARAMETER;
                pMatchResult->wPathStackTop++;
                for(j = i + 1;j < pMatchResult->wHintStackTop;j++ ) /* jump the DATA_TYPE_MAP */
                {
                    if(pMatchResult->tHint[j].wType == 0)
                        j++;
                    else
                    {
                        for(k = j;k < pMatchResult->wHintStackTop;k++)
                        {
                            pMatchResult->tHint[k].pName     = pMatchResult->tHint[k + 1].pName;
                            pMatchResult->tHint[k].pComment  = pMatchResult->tHint[k + 1].pComment;
                            pMatchResult->tHint[k].wPosition = pMatchResult->tHint[k + 1].wPosition;
                            pMatchResult->tHint[k].wType     = pMatchResult->tHint[k + 1].wType;
                        }
                        pMatchResult->wHintStackTop--;
                        --j;
                    }
                }
                /* if the next parameters is DATA_TYPE_STRING ,end matching */
                if(cfgDat->tCmdPara[pMatchResult->tHint[i + 1].wPosition].bStructType == STRUCTTYPE_PARA &&
                        cfgDat->tCmdPara[pMatchResult->tHint[i + 1].wPosition].uContent.tPara.bParaType == DATA_TYPE_STRING)
                    i = pMatchResult->wHintStackTop - 1;
            }
            else
            {
                /* reset wHintStackTop,only matched in the INTABLE,omit other parameters */
                for(j = i + 1; j < pMatchResult->wHintStackTop; j++) /* matching only in the DATA_TYPE_MAP */
                {
                    if(pMatchResult->tHint[j].wType == 0)
                        break;
                }
                pMatchResult->wHintStackTop = j;

                pMatchResult->tPath[pMatchResult->wPathStackTop].wPosition  = wPosition;
                pMatchResult->tPath[pMatchResult->wPathStackTop].wType  = pMatchResult->tHint[i].wType;
                pMatchResult->wPathStackTop++;
            }
        }
        else if(pCmdParaInfo->bStructType == STRUCTTYPE_PARA)
        {
            /* get the false position */
            if(pMatchResult->tHint[i].wType != 0)   /* DATA_TYPE_MAP */
                pMatchResult->wLength = pMatchResult->wMaxLength;
            /* 对于CONST/STRING参数，匹配长度为实际匹配的长度,而其它则为1 */
            else if(pCmdParaInfo->uContent.tPara.bParaType != DATA_TYPE_CONST
                    && pCmdParaInfo->uContent.tPara.bParaType != DATA_TYPE_STRING)
                pMatchResult->wLength = 1;  /* set the false position as the first character of the false word */
        }

        /* 计算本层匹配参数个数 */
        if (wPrevParaPos != pMatchResult->wParaSum)
        {
            if (wMatchedParaPos == 0)
                wMatchedParaPos = wPosition;
            wLayerParaSum++;
        }
    }

    if(pMatchResult->wSum == 0)
        return nResultVal;
    else if (wLayerParaSum > 1)
        return PARAERR_DETERMINER;

    if (pMatchResult->wSum > 1)
    {
        switch(pCmdLine->bDeterminer)
        {
        case DM_QUESTION:
        case DM_TAB:
            if(pMatchResult->wDepth == pCmdLine->wWordSum - 1)
            {
                pMatchResult->wDepth++;
                break;
            }
        case DM_NORMAL:
        case DM_BLANKQUESTION:
            {
                CHAR *pMatchStr;
                WORD16 wMatchAgainSum = 0;
                DWORD dwFirstItemPos = 0;

                for(i = pMatchResult->wSum; i > 0; i--)
                {
                    pMatchStr = NULL;
                    nCmpResult = 1;

                    wPosition = pMatchResult->tPath[pMatchResult->wPathStackTop - i].wPosition;
                    pCmdParaInfo =  (TYPE_CMD_PARA *) &(cfgDat->tCmdPara[wPosition]);
                    if(pCmdParaInfo->bStructType == STRUCTTYPE_PARA)
                    {
                        if(pMatchResult->tHint[wFirstMatchIndex].wType == 0)
                        {
                            if (pCmdParaInfo->uContent.tPara.bParaType == DATA_TYPE_CONST)
                                pMatchStr = pCmdParaInfo->uContent.tPara.sName;
                        }
                        else /* DATA_TYPE_MAP */
                        {
                            for (j = dwFirstItemPos; j < (int)pMatchResult->wHintStackTop; j++)
                            {
                                if (pMatchResult->tHint[j].bMatched)
                                {
                                    pMatchStr = (CHAR *)pMatchResult->tHint[j].pName;
                                    dwFirstItemPos = j + 1;
                                    break;
                                }
                            }
                        }
                    }
                    else
                        pMatchStr = pCmdParaInfo->uContent.tPrammar.sSymbol;

                    if (pMatchStr)
                        nCmpResult = StrCmpNoCase(pCmdLine->sCmdWord[pMatchResult->wDepth], pMatchStr);

                    if (nCmpResult == 0)
                    {
                        if (wMatchAgainSum == 0)
                            pMatchResult->wCurParaPos = wPosition;
                        wMatchAgainSum++;

                        if(pMatchResult->tHint[wFirstMatchIndex].wType != 0)
                        {
                            DWORD dwTypePos = FindMapTypePos(cfgDat,pCmdParaInfo->uContent.tPara.wLen);
                            DWORD dwEndItemPos = cfgDat->tMapType[dwTypePos].dwItemPos + cfgDat->tMapType[dwTypePos].wCount;

                            for (j = cfgDat->tMapType[dwTypePos].dwItemPos; j < (int)dwEndItemPos; j++)
                            {
                                pMatchStr = (CHAR *) &cfgDat->tMapString[cfgDat->tMapItem[j].dwKeyPos];
                                if(StrCmpNoCase(pCmdLine->sCmdWord[pMatchResult->wDepth], pMatchStr) == 0)
                                {
                                    wIntableValue = cfgDat->tMapItem[j].wValue;
                                    bMapFullMatch = TRUE;
                                    break;
                                }
                            }
                        }
                    }
                }
                if (wMatchAgainSum > 1)
                    return PARAERR_DETERMINER;
                else if (wMatchAgainSum == 1)
                {
                    if (wLayerParaSum > 0)
                    {
                        WORD16 wLastPos = pMatchResult->wPathStackTop - pMatchResult->wSum;
                        pMatchResult->tPath[wLastPos].wPosition = pMatchResult->wCurParaPos;
                        pMatchResult->tPath[wLastPos].wType = PATH_PARAMETER;
                        for(i = wLastPos + 1; i < pMatchResult->wPathStackTop; i++)
                        {
                            pMatchResult->tPath[i].wPosition = 0;
                            pMatchResult->tPath[i].wType    = 0;
                        }
                        pMatchResult->wPathStackTop = wLastPos + 1;

                        --pMatchResult->wParaSum;
                    }
                    pMatchResult->wSum = 1;
                }
                else if (wLayerParaSum == 1)
                {
                    if (pMatchResult->wSum > 2)
                        return PARAERR_DETERMINER;

                    if (pMatchResult->tPath[pMatchResult->wPathStackTop - 1].wPosition == wMatchedParaPos)
                    {
                        pMatchResult->wSum = 1;
                        pMatchResult->wCurParaPos = pMatchResult->tPath[pMatchResult->wPathStackTop - 2].wPosition;
                        --pMatchResult->wParaSum;
                    }
                    else
                    {
                        pMatchResult->wSum = 1;
                        pMatchResult->wCurParaPos = wMatchedParaPos;
                    }

                    /* 调整匹配路径 */
                    --pMatchResult->wPathStackTop;
                    pMatchResult->tPath[pMatchResult->wPathStackTop].wPosition = 0;
                    pMatchResult->tPath[pMatchResult->wPathStackTop].wType = 0;
                }
                else
                    return PARAERR_DETERMINER;
            }
            break;
        default:
            break;
        }
        /* 赋值给下面使用 */
        wPosition = pMatchResult->wCurParaPos;
    }

    if(pMatchResult->wSum == 1)
    {
        /* save parameter if success from function MatchTowString() */
        /* if success from ConvertStrToCmdPara(),save is in the ConvertStrToCmdPara() */
        pCmdParaInfo = (TYPE_CMD_PARA *) &(cfgDat->tCmdPara[wPosition]);
        if(pCmdParaInfo->bStructType == STRUCTTYPE_PRAMMAR &&
                ((pCmdParaInfo->uContent.tPrammar.bPrammarType == PRAMMAR_BRACKET_SQUARE
                  && (*(pCmdParaInfo->uContent.tPrammar.sSymbol)) != MARK_STRINGEND)
                 || pCmdParaInfo->uContent.tPrammar.bPrammarType == PRAMMAR_WHOLE_SYMBOL))
        {
            if(cfgDat->tCmdAttr[pLinkState->wCmdAttrPos].bExeModeID != EXEC_PROTOCOL)/*EXEC_PROTOCOL*/
            {
                pCmdPara = (CMDPARA *) &(pMatchResult->tPara[pMatchResult->wParaSum]);
                pCmdPara->ParaNo            = pCmdParaInfo->ParaNo;
				
                memset(pCmdPara->sName, 0, sizeof(pCmdPara->sName));
                /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
                XOS_snprintf(pCmdPara->sName,
                             sizeof(pCmdPara->sName) - 1,
                             "%s",
                             pCmdParaInfo->uContent.tPrammar.sSymbol);

                /* omit by wangmh for Script ParaNo Match with ZXR10
                pCmdPara->Type          = DATA_TYPE_CONST;
                */
                pCmdPara->Type          = DATA_TYPE_PARA;

                pCmdPara->Len               = sizeof(BYTE);
                pCmdPara->pValue            = (BYTE *)Oam_InptGetBuf(sizeof(BYTE),pLinkState->ucLinkID);
                *((BYTE *)pCmdPara->pValue) = TRUE;
                /*pCmdPara->wValue = TRUE;*/
                pMatchResult->wParaSum++;
            }
            else
            {
                if(pCmdParaInfo->uContent.tPrammar.bItemCount == 0 &&
                        pCmdParaInfo->uContent.tPrammar.bPrammarType != PRAMMAR_WHOLE_SYMBOL)
                {
                    pCmdPara = (CMDPARA *) &(pMatchResult->tPara[pMatchResult->wParaSum]);
                    pCmdPara->ParaNo            = pCmdParaInfo->ParaNo;
                    
                    memset(pCmdPara->sName, 0, sizeof(pCmdPara->sName));
                    /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
                    XOS_snprintf(pCmdPara->sName,
                                 sizeof(pCmdPara->sName) -1,
                                 "%s",
                                 pCmdParaInfo->uContent.tPara.sName);
			
                    pCmdPara->Type              = DATA_TYPE_CONST;
                    pCmdPara->Len               = sizeof(BYTE);
                    pCmdPara->pValue            = (BYTE *)Oam_InptGetBuf(sizeof(BYTE),pLinkState->ucLinkID);
                    *((BYTE *)pCmdPara->pValue) = TRUE;
                    /*pCmdPara->wValue = TRUE;*/
                    pMatchResult->wParaSum++;
                }
            }
        }
        else if(pCmdParaInfo->bStructType == STRUCTTYPE_PARA)
        {
            if(pCmdParaInfo->uContent.tPara.bParaType == DATA_TYPE_CONST)
            {
                pCmdPara = (CMDPARA *) &(pMatchResult->tPara[pMatchResult->wParaSum]);
                pCmdPara->ParaNo        = pCmdParaInfo->ParaNo;
				
                memset(pCmdPara->sName, 0, sizeof(pCmdPara->sName));
                /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
                XOS_snprintf(pCmdPara->sName,
                             sizeof(pCmdPara->sName) -1,
                             "%s",
                             pCmdParaInfo->uContent.tPara.sName);
			
                pCmdPara->Type          = DATA_TYPE_CONST;
                pCmdPara->Len               = sizeof(BYTE);
                pCmdPara->pValue            = (BYTE *)Oam_InptGetBuf(sizeof(BYTE),pLinkState->ucLinkID);
                *((BYTE *)pCmdPara->pValue) = TRUE;
                /*pCmdPara->wValue = TRUE;*/
                pMatchResult->wParaSum++;
            }
            else if(pCmdParaInfo->uContent.tPara.bParaType == DATA_TYPE_MAP)    /* data_type_map */
            {
                /* 匹配表数据不一定是从第一项值开始 */
                /* 因此时按名称比较 */

                /* 得到对应的内部整数值 */
                WORD16 wValue = 0;
                if (bMapFullMatch)  /* 匹配表中有完全包含的字符串 */
                    wValue = wIntableValue;
                else
                {
                    WORD16 wMatchLen = 1;
                    int iRetTmp = PARA_SUCCESS;
                    iRetTmp = ConvertStringToWord(&(pCmdParaInfo->uContent.tPara),
                                        pCmdLine->sCmdWord[pMatchResult->wDepth],
                                        &wValue, &wMatchLen, cfgDat);
                    if (iRetTmp != PARA_SUCCESS)
                    {                        
                        return PARAERR_DETERMINER;
                    }
                }

                pCmdPara = (CMDPARA *) &(pMatchResult->tPara[pMatchResult->wParaSum]);
                pCmdPara->ParaNo = pCmdParaInfo->ParaNo;
				
                memset(pCmdPara->sName, 0, sizeof(pCmdPara->sName));
                /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
                XOS_snprintf(pCmdPara->sName,
                             sizeof(pCmdPara->sName) -1,
                             "%s",
                             pCmdParaInfo->uContent.tPara.sName);

                pCmdPara->Type   = DATA_TYPE_MAP;
                pCmdPara->Len    = sizeof(WORD16);
                pCmdPara->pValue = (BYTE *)Oam_InptGetBuf(sizeof(WORD16),pLinkState->ucLinkID);
                *((WORD16 *)pCmdPara->pValue) = wValue;
                pMatchResult->wParaSum++;
            }
			
        }

        /*for MORE_ANGLE,回溯到原来的MOREPOS匹配处*/
        for(tmp = 0; tmp < pMatchResult->bMoreLayerTop; tmp++)
            if(wPosition >= pMatchResult->wMoreBeginPos[tmp]
                    && wPosition <= pMatchResult->wMoreEndPos[tmp])
                break;
        /* 如果匹配参数在MORE堆栈中找到，说明还存在于MORE语法的解析范围之内；
            否则，表示该参数已经在MORE的范围之外，此时可以清除所有的MORE语法
            涉及到的关键变量*/
        if(tmp < pMatchResult->bMoreLayerTop)
        {
            while(wPosition < pMatchResult->wMoreBeginPos[pMatchResult->bMoreLayerTop - 1]
                    ||wPosition > pMatchResult->wMoreEndPos[pMatchResult->bMoreLayerTop - 1])
                ClearMoreLayer(pMatchResult);
        }
        else
        {
            pMatchResult->bMoreLayerTop = 0;
            pMatchResult->wMatchedPathTop = 0;
        }
        /* 参数在recycle或{[][][]}内 */
        if((wPosition >= pMatchResult->wMoreBeginPos[MORE_LAYER_TOP])
                && (wPosition <= pMatchResult->wMoreEndPos[MORE_LAYER_TOP]))
        {
            pMatchResult->wCurParaPos = wPosition;
            JumpNestingPrammar(pMatchResult, wParaBeginPos, wParaEndPos);
            if(pMatchResult->wCurParaPos >= pMatchResult->wMoreEndPos[MORE_LAYER_TOP])
                pMatchResult->wCurParaPos = pMatchResult->wMoreBeginPos[MORE_LAYER_TOP];

            if(pMatchResult->bType[MORE_LAYER_TOP] == PRAMMAR_RECYCLE)
            {
                pMatchResult->bPrammarRecycle = TRUE;
                pMatchResult->bContinueSearch[MORE_LAYER_TOP] = FALSE;
            }
            else if(pMatchResult->bType[MORE_LAYER_TOP] == PRAMMAR_MORE_ANGLE)
            {
                pMatchResult->bContinueSearch[MORE_LAYER_TOP] = TRUE;
                pMatchResult->bPrammarRecycle = FALSE;

                /* 记下{[][][][]}中已经匹配过的参数位置，在下一次匹配时不再出项 */
                /* 不记录非语法的参数，如数值参数 */
                /*if(cfgDat->tCmdPara[wPosition].bStructType != STRUCTTYPE_PARA)*/
                {
                    pMatchResult->tMatchedPath[pMatchResult->wMatchedPathTop].wPosition = wPosition;
                    pMatchResult->wMatchedPathTop++;
                }
            }
        }
        else    /* 参数不在recycle和{[][][]}中 */
        {
            pMatchResult->bContinueSearch[MORE_LAYER_TOP] = FALSE;
            pMatchResult->bPrammarRecycle = FALSE;

            pMatchResult->wCurParaPos = wPosition;
            JumpNestingPrammar(pMatchResult,wParaBeginPos,wParaEndPos);
        }

        pMatchResult->wDepth++;
    }
    return PARA_SUCCESS;
}

/* 加入MAP类型的?帮助 */
int GetMapDeterminer(TYPE_LINK_STATE *pLinkState, int nPos, int nMaxLen)
{
    TYPE_CMD_LINE *pCmdLine = &(pLinkState->tCmdLine);
    TYPE_MATCH_RESULT *pMatchResult = &(pLinkState->tMatchResult);
    T_OAM_Cfg_Dat* cfgDat = pMatchResult->cfgDat;
    CHAR *pString = NULL;
    int j, nLen, nSplitPos;
    WORD16 wMatchLen = 0;
    DWORD dwTypePos = FindMapTypePos(cfgDat,pMatchResult->tPath[pMatchResult->wPathStackTop - nPos].wType);
    DWORD dwEndItemPos = cfgDat->tMapType[dwTypePos].dwItemPos + cfgDat->tMapType[dwTypePos].wCount;

    for (j = cfgDat->tMapType[dwTypePos].dwItemPos; j < (int)dwEndItemPos; j++)
    {
        pString = (CHAR *)&cfgDat->tMapString[cfgDat->tMapItem[j].dwKeyPos];
        if(MatchTwoString(pCmdLine->sCmdWord[pMatchResult->wDepth - 1], pString, &wMatchLen) == 0)
        {
            /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
            XOS_snprintf(pLinkState->sOutputResult,
                         MAX_OUTPUT_RESULT - 1,
                         "%s%s",
                         pLinkState->sOutputResult,
                         pString);

            if (cfgDat->tMapItem[j].dwCommentPos)
            {
                nLen = nMaxLen - strlen(pString);
                memset(pTempStr1, MARK_BLANK, (size_t)nLen);
                pTempStr1[nLen] = MARK_STRINGEND;
                /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
                XOS_snprintf(pLinkState->sOutputResult,
                             MAX_OUTPUT_RESULT - 1,
                             "%s%s",
                             pLinkState->sOutputResult,
                             pTempStr1);

                pString = (CHAR *)&cfgDat->tMapString[cfgDat->tMapItem[j].dwCommentPos];

                if (strlen(pString) + nMaxLen >= MAX_LINEWIDTH)
                {
                    nSplitPos = MAX_LINEWIDTH - nMaxLen;
                    while (pString[nSplitPos] != MARK_BLANK)
                        nSplitPos--;
                    memcpy(pTempStr1,pString, (size_t)nSplitPos);
                    pTempStr1[nSplitPos] = MARK_STRINGEND;
                    /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
                    XOS_snprintf(pLinkState->sOutputResult,
                                 MAX_OUTPUT_RESULT - 1,
                                 "%s%s\n",
                                 pLinkState->sOutputResult,
                                 pTempStr1);

                    memset(pTempStr1, MARK_BLANK, (size_t)nMaxLen);
                    pTempStr1[nMaxLen] = MARK_STRINGEND;
                    /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
                    XOS_snprintf(pLinkState->sOutputResult,
                                 MAX_OUTPUT_RESULT - 1,
                                 "%s%s%s",
                                 pLinkState->sOutputResult,
                                 pTempStr1,
                                 (CHAR *)&(pString[nSplitPos + 1]));

                }
                else
                    /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
                    XOS_snprintf(pLinkState->sOutputResult,
                                 MAX_OUTPUT_RESULT - 1,
                                 "%s%s",
                                 pLinkState->sOutputResult,
                                 pString);

            }
            /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
            XOS_snprintf(pLinkState->sOutputResult,
                         MAX_OUTPUT_RESULT - 1,
                         "%s\n",
                         pLinkState->sOutputResult);

            nPos--;
        }
    }

    return nPos;
}

/* deal with the determiners such as "?" and "TAB" flowering after a parameter */
void DealParaDeterminer(TYPE_LINK_STATE *pLinkState)
{
    TYPE_CMD_LINE *pCmdLine = &(pLinkState->tCmdLine);
    TYPE_MATCH_RESULT *pMatchResult = &(pLinkState->tMatchResult);
    T_OAM_Cfg_Dat* cfgDat = pMatchResult->cfgDat;
    int nSplitPos = 0;
    int nWordLen = 0;
    int nMaxLen = 0;
    int nLen = 0;
    int i = 0, j = 0;
    WORD16 wCurParaPos = pMatchResult->wMaxCurParaPos;

    TYPE_PARA *pParaInfo = NULL;        /* information about the command parameter */
    TYPE_PRAMMAR *pPrammarInfo = NULL;      /* information about the prammar */
    TYPE_CMD_PARA *pCmdParaInfo = NULL;

    if (wCurParaPos == INVALID_PARAINDEX)
        return;

    *pTempStr1 = MARK_STRINGEND;
    *pTempStr2 = MARK_STRINGEND;
    switch(pCmdLine->bDeterminer)
    {
    case DM_QUESTION:   /* parameter?       */
        /* calculate the max len of the name */
#if 0
        for (i = pMatchResult->wSum; i > 0; i--)
        {
            for(j = 0; j < pMatchResult->wHintStackTop; j++)
                if(pMatchResult->tHint[j].wPosition == pMatchResult->tPath[pMatchResult->wPathStackTop - i].wPosition)
                {
                    nWordLen = strlen(pMatchResult->tHint[j].pName);
                    break;
                }
            if (nWordLen > nMaxLen)
                nMaxLen = nWordLen;
        }
#endif
        for (j = 0; j < pMatchResult->wHintStackTop; j++)
        {
            nWordLen = strlen(pMatchResult->tHint[j].pName);
            if (nWordLen > nMaxLen)
                nMaxLen = nWordLen;
        }
        nMaxLen += 4;

        /* output the name and comment of the parameter */
        for(i = pMatchResult->wSum ; i > 0; i--)
        {
            pCmdParaInfo = &cfgDat->tCmdPara[pMatchResult->tPath[pMatchResult->wPathStackTop - i].wPosition];
            switch(pCmdParaInfo->bStructType)
            {
            case STRUCTTYPE_PARA:
                pParaInfo = &(pCmdParaInfo->uContent.tPara);
                if(pParaInfo->bParaType == DATA_TYPE_MAP) /* 处理MAP类型 */
                    i = GetMapDeterminer(pLinkState, i, nMaxLen);
                else
                {
                    /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
                    XOS_snprintf(pLinkState->sOutputResult,
                                 MAX_OUTPUT_RESULT - 1,
                                 "%s%s",
                                 pLinkState->sOutputResult,
                                 pParaInfo->sName);

                    nLen = nMaxLen - strlen(pParaInfo->sName);
                    memset(pTempStr1, MARK_BLANK, (size_t)nLen);
                    pTempStr1[nLen] = MARK_STRINGEND;

                    /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
                    XOS_snprintf(pLinkState->sOutputResult,
                                 MAX_OUTPUT_RESULT - 1,
                                 "%s%s",
                                 pLinkState->sOutputResult,
                                 pTempStr1);

                    /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
                    XOS_snprintf(pTempStr2,
                                 sizeof(pTempStr2) -1,
                                 "%s",
                                 pParaInfo->sComment);

                    GetParaFormat(pCmdParaInfo, pTempStr2, sizeof(pTempStr2));

                    if (strlen(pTempStr2) + nMaxLen >= MAX_LINEWIDTH)
                    {
                        nSplitPos = MAX_LINEWIDTH - nMaxLen;
                        while (pTempStr2[nSplitPos] != MARK_BLANK)
                            nSplitPos--;
                        memcpy(pTempStr1, pTempStr2, (size_t)nSplitPos);
                        pTempStr1[nSplitPos] = MARK_STRINGEND;
                        /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
                        XOS_snprintf(pLinkState->sOutputResult,
                                     MAX_OUTPUT_RESULT - 1,
                                     "%s%s\n",
                                     pLinkState->sOutputResult,
                                     pTempStr1);

                        memset(pTempStr1, MARK_BLANK, (size_t)nMaxLen);
                        pTempStr1[nMaxLen] = MARK_STRINGEND;
                        /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
                        XOS_snprintf(pLinkState->sOutputResult,
                                     MAX_OUTPUT_RESULT - 1,
                                     "%s%s%s",
                                     pLinkState->sOutputResult,
                                     pTempStr1,
                                     (CHAR *)&(pTempStr2[nSplitPos + 1]));
                    }
                    else
                        /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
                        XOS_snprintf(pLinkState->sOutputResult,
                                     MAX_OUTPUT_RESULT - 1,
                                     "%s%s",
                                     pLinkState->sOutputResult,
                                     pTempStr2);
                    /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
                    XOS_snprintf(pLinkState->sOutputResult,
                                 MAX_OUTPUT_RESULT - 1,
                                 "%s\n",
                                 pLinkState->sOutputResult);
                }
                break;

            case STRUCTTYPE_PRAMMAR:
                pPrammarInfo = (TYPE_PRAMMAR *) &cfgDat->tCmdPara[pMatchResult->tPath[pMatchResult->wPathStackTop - i].wPosition].uContent.tPrammar;
                if (strlen(pPrammarInfo->sSymbol) != 0)
                {
                    /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
                    XOS_snprintf(pLinkState->sOutputResult,
                                 MAX_OUTPUT_RESULT - 1,
                                 "%s%s    %s\n",
                                 pLinkState->sOutputResult,
                                 pPrammarInfo->sSymbol,
                                 pPrammarInfo->sComment);
                }
                break;
            default:
                break;
            }
        }
        if(pMatchResult->wSum == 0)
        {
            pParaInfo = (TYPE_PARA *) &cfgDat->tCmdPara[pMatchResult->wCurParaPos].uContent.tPara;
            if(pParaInfo->bParaType == DATA_TYPE_IPADDR || pParaInfo->bParaType == DATA_TYPE_MASK ||
                    pParaInfo->bParaType == DATA_TYPE_IMASK || pParaInfo->bParaType == DATA_TYPE_MACADDR||
                    pParaInfo->bParaType == DATA_TYPE_IPV6PREFIX ||pParaInfo->bParaType == DATA_TYPE_IPV6ADDR||
                    pParaInfo->bParaType == DATA_TYPE_VPN_ASN ||pParaInfo->bParaType == DATA_TYPE_VPN_IP)
            {
                /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
                XOS_snprintf(pLinkState->sOutputResult,
                             MAX_OUTPUT_RESULT - 1,
                             "%s%s    %s\n",
                             pLinkState->sOutputResult,
                             pParaInfo->sName,
                             pParaInfo->sComment);
            }
        }
        if (*(pLinkState->sOutputResult) != MARK_STRINGEND)
            pLinkState->bOutputResult = TRUE;
        /* return the input string */
        CombineCmdLine(pCmdLine, TRUE, pLinkState->sCmdLine, MAX_CMDLINE_LEN);
        pLinkState->bOutputCmdLine = TRUE;
        break;

    case DM_BLANKQUESTION:  /* parameter ?      */
        switch(pMatchResult->wSum)
        {
        case 0:
            break;
        case 1:
            /* clear the stack */
            pMatchResult->wHintStackTop = 0;
            /* get the combination of the name and the comment of the parameter */
            CombineNameComment(pLinkState, pMatchResult->wFirst, pLinkState->sOutputResult);
            pLinkState->bOutputResult = TRUE;
            /* return the input string */
            CombineCmdLine(pCmdLine, TRUE, pLinkState->sCmdLine, MAX_CMDLINE_LEN);
            pLinkState->bOutputCmdLine = TRUE;
            break;
        default:
            /**对于多匹配，wDepth应该回退(在匹配时wDepth多加1)*/
            if(pMatchResult->wSum > 1 && pMatchResult->wDepth > 0)
                pMatchResult->wDepth --;

            i = CalculateMatchPosition(pLinkState);
            i += strlen(pLinkState->sPrompt);
            memset(pTempStr1, MARK_BLANK, (size_t)(i - 1));
            pTempStr1[i - 1] = '^';
            pTempStr1[i] = MARK_STRINGEND;
            /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
            XOS_snprintf(pLinkState->sOutputResult,
                         MAX_OUTPUT_RESULT - 1,
                         "%s%s\n%s\n",
                         pLinkState->sOutputResult,
                         pTempStr1,
                         CLI_szCLIHelp2);

            pLinkState->tMatchResult.wParseState = PARSESTATE_ASK_TELNET;
            return;
        }
        break;

    case DM_TAB:        /* parameter+TAB    */
        /* ???? */
        /* return the input string */
        CombineCmdLine(pCmdLine, FALSE, pLinkState->sCmdLine, MAX_CMDLINE_LEN);
        /* find the whole string */
        switch(pMatchResult->wSum)
        {
        case 0:
            /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
            XOS_snprintf(pTempStr1,
                         sizeof(pTempStr1) -1,
                         "%s",
                         pCmdLine->sCmdWord[pMatchResult->wDepth - 1]);
            break;

        case 1:
            switch(cfgDat->tCmdPara[pMatchResult->wFirst].bStructType)
            {
            case STRUCTTYPE_PARA:
                pParaInfo = (TYPE_PARA *) &cfgDat->tCmdPara[pMatchResult->wFirst].uContent.tPara;
                switch(pParaInfo->bParaType)
                {
                case DATA_TYPE_MAP:
                    {
                        WORD16 wTempVal = 0;
                        WORD16 wLen = 0;

                        /* 一般处理 */
                        {
                            int iRetTmp = PARA_SUCCESS;
                            iRetTmp = ConvertStringToWord(pParaInfo,
                                                pCmdLine->sCmdWord[pMatchResult->wDepth - 1], &wTempVal, &wLen, cfgDat);
                            
                            if (iRetTmp != PARA_SUCCESS)
                            {     
                                pMatchResult->wParseState = PARSESTATE_PARA_EXCEPTION;
                                DealParaException(pLinkState);
                                return;
                            }
                    
                            /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
                            XOS_snprintf(pTempStr1,
                                         sizeof(pTempStr1) -1,
                                         "%s",
                                         OamCfgConvertWordToString(cfgDat,pParaInfo->wLen, wTempVal));
                        }
                        {
                            /*add by kongyj*/
                            WORD16 wStrLen1 = strlen(pTempStr1);
                            WORD16 wStrLen2;
                            CHAR *pReMarkStr = strstr(pTempStr1, " ");
                            if (pReMarkStr != NULL)
                            {
                                wStrLen2 = strlen(pReMarkStr);
                                *(pTempStr1 + (wStrLen1 - wStrLen2)) = '\0';
                            }
                            else
                            {
                                pReMarkStr = strstr(pTempStr1, "\t");
                                if(pReMarkStr != NULL)
                                {
                                    wStrLen2 = strlen(pReMarkStr);
                                    *(pTempStr1 + (wStrLen1 - wStrLen2)) = '\0';
                                }
                            }
                        }
                        break;
                    }
                case DATA_TYPE_CONST:
                    /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
                    XOS_snprintf(pTempStr1,
                                 sizeof(pTempStr1) -1,
                                 "%s",
                                 pParaInfo->sName);
                    break;
                default:
                    /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
                    XOS_snprintf(pTempStr1,
                                 sizeof(pTempStr1) -1,
                                 "%s",
                                 pCmdLine->sCmdWord[pMatchResult->wDepth - 1]);
                    break;
                }
                XOS_snprintf(pTempStr1, MAX_STRING_LEN - 1, "%s ", pTempStr1);
                break;
            case STRUCTTYPE_PRAMMAR:
                pPrammarInfo = (TYPE_PRAMMAR *) &cfgDat->tCmdPara[pMatchResult->wFirst].uContent.tPrammar;
                if (strlen(pPrammarInfo->sSymbol) != 0)
                {
                    /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
                    XOS_snprintf(pTempStr1,
                                 sizeof(pTempStr1) -1,
                                 "%s ",
                                 pPrammarInfo->sSymbol);
                }
                break;
            default:
                break;
            }
            break;
        default:
            {
                static TYPE_HINT tHint[MAX_HINTSUM];
                WORD16 wMatchLen = 0;

                for(i = pMatchResult->wSum; i > 0; i--)
                    for(j = 0; j < pMatchResult->wHintStackTop; j++)
                    {
                        if(pMatchResult->tHint[j].wPosition == pMatchResult->tPath[pMatchResult->wPathStackTop - i].wPosition)
                        {
                            if(pMatchResult->tHint[j].wType > 0) /* data_type_map */
                            {
                                if(MatchTwoString(pCmdLine->sCmdWord[pMatchResult->wDepth -1], pMatchResult->tHint[j].pName, &wMatchLen) != 0)
                                    continue;
                            }
                            tHint[pMatchResult->wSum - i].wPosition= pMatchResult->tHint[j].wPosition;
                            tHint[pMatchResult->wSum - i].pName    = pMatchResult->tHint[j].pName;
                            tHint[pMatchResult->wSum - i].pComment = pMatchResult->tHint[j].pComment;
                            tHint[pMatchResult->wSum - i].wType    = pMatchResult->tHint[j].wType;
                            i--;
                            if(i == 0)
                                break;
                        }
                    }

                /* reset the tHint */
                for (i = 0; i < pMatchResult->wHintStackTop; i++)
                {
                    pMatchResult->tHint[i].pName     = NULL;
                    pMatchResult->tHint[i].pComment  = NULL;
                    pMatchResult->tHint[i].wType     = PATH_INVALID;
                    pMatchResult->tHint[i].wPosition = INVALID_NODE;
                }
                pMatchResult->wHintStackTop = 0;

                for (i = 0; i < pMatchResult->wSum; i++)
                {
                    pMatchResult->tHint[i].wPosition = tHint[i].wPosition;
                    if ((cfgDat->tCmdPara[tHint[i].wPosition].bStructType == STRUCTTYPE_PARA)
                            && (cfgDat->tCmdPara[tHint[i].wPosition].uContent.tPara.bParaType == DATA_TYPE_STRING))
                        pMatchResult->tHint[i].pName = pCmdLine->sCmdWord[pMatchResult->wDepth - 1];
                    else
                        pMatchResult->tHint[i].pName = tHint[i].pName;
                    pMatchResult->tHint[i].pComment  = tHint[i].pComment;
                    pMatchResult->tHint[i].wType     = PATH_PARAMETER;
                    pMatchResult->wHintStackTop++;
                    if (pMatchResult->wHintStackTop > MAX_HINTSUM)
                    {
                        pMatchResult->wParseState = PARSESTATE_PARA_EXCEPTION;
                        DealParaException(pLinkState);
                        return;
                    }
                }

                pMatchResult->wFirst = 0;
                CalculateWordTabMax(pMatchResult, pTempStr1);
                break;
            }
        }
        /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
        /*消除pcLint告警 if ((strlen(pLinkState->sCmdLine) + strlen(pTempStr1)) > (MAX_CMDLINE_LEN - 1))*/
        {
            int cmdlinelen =strlen(pLinkState->sCmdLine);
            int str1len =strlen(pTempStr1);
            int totallen = cmdlinelen+str1len;
            if(totallen > (MAX_CMDLINE_LEN - 1))
            {
                XOS_snprintf(pLinkState->sOutputResult,
                         MAX_OUTPUT_RESULT -1,
                         "The max length of a command is %u bytes, can not input more characters!\n\r", 
                         MAX_CMDLINE_LEN - 1);

                pLinkState->bOutputResult = TRUE;
                SendPacketToTelnet(pLinkState, TRUE);
            }
            else
            {
                XOS_snprintf(pLinkState->sCmdLine,
                         MAX_CMDLINE_LEN - 1,
                         "%s%s",
                         pLinkState->sCmdLine,
                         pTempStr1);
            }
        }

        pLinkState->bOutputCmdLine = TRUE;
        break;

    case DM_NORMAL:
        /* ???? */
        if (!pCmdLine->bHasNo)
        {
            if (pMatchResult->wCurParaPos !=
                    (cfgDat->tCmdAttr[pMatchResult->wCmdAttrPos].dwParaPos +
                     cfgDat->tCmdAttr[pMatchResult->wCmdAttrPos].wCmdParaCount - 1))
                DealParaException(pLinkState);
        }
        else
            if (pMatchResult->wCurParaPos !=
                    (cfgDat->tCmdAttr[pMatchResult->wCmdAttrPos].dwParaPos +
                     cfgDat->tCmdAttr[pMatchResult->wCmdAttrPos].wAllParaCount - 1))
                DealParaException(pLinkState);
        break;

    default:
        break;
    }
    /* change the parse state */
    pMatchResult->wParseState = PARSESTATE_FINISH_DEAL;
}

/* deal the exception about the command parameter */
void DealParaException(TYPE_LINK_STATE *pLinkState)
{
    TYPE_CMD_LINE *pCmdLine = &(pLinkState->tCmdLine);
    TYPE_MATCH_RESULT *pMatchResult = &(pLinkState->tMatchResult);
    int i = 0;

    *pTempStr1 = MARK_STRINGEND;
    if (pCmdLine->wWordSum > 0)
    {
        if ((NULL == CLI_szCLIHelp4) || (NULL == CLI_szCLIHelp2))
            return;
        
        if (pMatchResult->wHintStackTop >= MAX_HINTSUM)
        {
            XOS_snprintf(pLinkState->sOutputResult, MAX_OUTPUT_RESULT, "%%Error: in function DealParaException ,pMatchResult->wHintStackTop value too big(max value is %d)\n"
                      "make sure DAT file`s format  is right!!\n",MAX_HINTSUM);
        }
        else if(pMatchResult->wSum == 1 && pMatchResult->wDepth == pCmdLine->wWordSum)
        {
            /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
            XOS_snprintf(pLinkState->sOutputResult,
                         MAX_OUTPUT_RESULT - 1,
                         "%s\n",
                         CLI_szCLIHelp4);
        }
        else
        {
            /**对于多匹配，wDepth应该回退(在匹配时wDepth多加1)*/
            if (pMatchResult->wSum > 1 && pMatchResult->wDepth > 0)
            {
                /*CRDCM00049462 去掉这段处理
                pMatchResult->wDepth --;
                */
            }
            else if (pMatchResult->wSum == 1)
            {
                pMatchResult->wLength = 1;
            }

            i = CalculateMatchPosition(pLinkState);
            i += strlen(pLinkState->sPrompt);
            i = (MAX_STRING_LEN > i && i > 0) ? i : (MAX_STRING_LEN - 1);
            memset(pTempStr1, MARK_BLANK, (size_t)(i - 1));
            pTempStr1[i - 1] = '^';
            pTempStr1[i] = MARK_STRINGEND;
            /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
            XOS_snprintf(pLinkState->sOutputResult,
                         MAX_OUTPUT_RESULT - 1,
                         "%s%s\n%s\n",
                         pLinkState->sOutputResult,
                         pTempStr1,
                         CLI_szCLIHelp2);

            pLinkState->tMatchResult.wParseState = PARSESTATE_ASK_TELNET;
            return;
        }
        if (*(pLinkState->sOutputResult) != MARK_STRINGEND)
            pLinkState->bOutputResult = TRUE;
    }
    /* change the parse state */
    pMatchResult->wParseState = PARSESTATE_FINISH_DEAL;
}

/* link para's format to out string */
void GetParaFormat(TYPE_CMD_PARA *pCmdParaInfo, CHAR *pOutStr, WORD32 dwBufSize)
{
    switch(pCmdParaInfo->uContent.tPara.bParaType)
    {
    case DATA_TYPE_BYTE:
    case DATA_TYPE_WORD:
    case DATA_TYPE_DWORD:
    case DATA_TYPE_INT:
    case DATA_TYPE_WORD64:
        {
            TYPE_PARA *pParameterInfo  = (TYPE_PARA *)&pCmdParaInfo->uContent.tPara;
            XOS_snprintf(pOutStr, dwBufSize, "%s ", pOutStr);
            XOS_snprintf(pOutStr + strlen(pOutStr), dwBufSize - strlen(pOutStr), CLI_szCLIHelp7, pParameterInfo->Min.qdwMinVal, pParameterInfo->Max.qdwMaxVal);
        }
        break;
    case DATA_TYPE_SWORD:
        { 
            TYPE_PARA *pParameterInfo  = (TYPE_PARA *)&pCmdParaInfo->uContent.tPara;
            strncat(pOutStr, " ", dwBufSize);
            XOS_snprintf(pOutStr + strlen(pOutStr), dwBufSize - strlen(pOutStr), CLI_szCLIHelp14, pParameterInfo->Min.qdwMinVal, pParameterInfo->Max.qdwMaxVal);
        }
        break;
    case DATA_TYPE_FLOAT:
        {
            TYPE_PARA *pParameterInfo  = (TYPE_PARA *)&pCmdParaInfo->uContent.tPara;
            CHAR Min[50] = {0}, Max[50] = {0};
            XOS_snprintf(pOutStr, dwBufSize, "%s ", pOutStr);
            XOS_snprintf(Min, sizeof(Min),"%.3f", pParameterInfo->Min.fMinVal);
            XOS_snprintf(Max, sizeof(Max),"%.3f", pParameterInfo->Max.fMaxVal);
            DeleteNouseZero(Min);
            DeleteNouseZero(Max);
            XOS_snprintf(pOutStr, dwBufSize, "%s(Range:%s~%s)", pOutStr, Min, Max);
        }
        break;
    case DATA_TYPE_STRING:
    case DATA_TYPE_TEXT:
        {
            TYPE_PARA *pParameterInfo  = (TYPE_PARA *)&pCmdParaInfo->uContent.tPara;
            XOS_snprintf(pOutStr, dwBufSize, "%s ", pOutStr);
            XOS_snprintf(pOutStr + strlen(pOutStr), dwBufSize - strlen(pOutStr), CLI_szCLIHelp8, pParameterInfo->Min.qdwMinVal, pParameterInfo->Max.qdwMaxVal);
        }
        break;
    case DATA_TYPE_IPADDR:
    case DATA_TYPE_MASK:
    case DATA_TYPE_IMASK:
        XOS_snprintf(pOutStr, dwBufSize, "%s %s", pOutStr, CLI_szCLIHelp9);
        break;
    case DATA_TYPE_IPV6PREFIX:
        XOS_snprintf(pOutStr, dwBufSize, "%s %s", pOutStr, CLI_szCLIHelp16);
        break;
    case DATA_TYPE_IPV6ADDR:
        XOS_snprintf(pOutStr, dwBufSize, "%s %s", pOutStr, CLI_szCLIHelp17);
        break;
    case DATA_TYPE_DATE:
        XOS_snprintf(pOutStr, dwBufSize, "%s %s", pOutStr, CLI_szCLIHelp10);
        break;
    case DATA_TYPE_TIME:
        XOS_snprintf(pOutStr, dwBufSize, "%s %s", pOutStr, CLI_szCLIHelp11);
        break;
    case DATA_TYPE_HEX:
        XOS_snprintf(pOutStr, dwBufSize, "%s %s", pOutStr, CLI_szCLIHelp12);
        break;
    case DATA_TYPE_MACADDR:
        XOS_snprintf(pOutStr, dwBufSize, "%s %s", pOutStr, CLI_szCLIHelp13);
        break;
    case DATA_TYPE_VPN_ASN:
        /* 复合类型的注释不再由OAM提供，EC611001680296修改 */
        /*XOS_snprintf(pOutStr, dwBufSize, "%s %s", pOutStr, CLI_szCLIHelp18);*/
        break;
    case DATA_TYPE_VPN_IP:
        /* 复合类型的注释不再由OAM提供，EC611001680296修改 */
        /*XOS_snprintf(pOutStr, dwBufSize, "%s %s", pOutStr, CLI_szCLIHelp19);*/
        break;
    case DATA_TYPE_PHYADDRESS:
        XOS_snprintf(pOutStr, dwBufSize, "%s %s", pOutStr, CLI_szCLIHelp20);
        break;
    case DATA_TYPE_LOGICADDRESS:
        XOS_snprintf(pOutStr, dwBufSize, "%s %s", pOutStr, CLI_szCLIHelp21);
        break;
    case DATA_TYPE_LOGIC_ADDR_U:
        XOS_snprintf(pOutStr, dwBufSize, "%s %s", pOutStr, CLI_szCLIHelp22);
        break;
    case DATA_TYPE_LOGIC_ADDR_M:
        XOS_snprintf(pOutStr, dwBufSize, "%s %s", pOutStr, CLI_szCLIHelp23);
        break;
    case DATA_TYPE_IPVXADDR:
        XOS_snprintf(pOutStr, dwBufSize, "%s %s", pOutStr, CLI_szCLIHelp24);
        break;
    case DATA_TYPE_SYSCLOCK:
        XOS_snprintf(pOutStr, dwBufSize, "%s %s", pOutStr, CLI_szCLIHelp25);
        break;
    case DATA_TYPE_VMMDATE:
    case DATA_TYPE_VMMDATETIME:
        /*这两种类型只用于显示*/
        break;
    default:
        return;
    }
}

/* combine the blanks and the words to construct the rude command line */
void CombineCmdLine(TYPE_CMD_LINE *pCmdLine, BYTE bIncludeLast, CHAR *pOutStr, WORD32 dwBufSize)
{
    int i = 0;

    *pTempStr1 = MARK_STRINGEND;
    /* combine the command line */
    while (i < pCmdLine->wWordSum - 1)
    {
        memset(pTempStr1, MARK_BLANK, (size_t)pCmdLine->wBlankSum[i]);
        pTempStr1[pCmdLine->wBlankSum[i]] = MARK_STRINGEND;
        XOS_snprintf(pOutStr, dwBufSize, "%s%s%s", pOutStr, pTempStr1, pCmdLine->sCmdWord[i]);
        i++;
    }
    memset(pTempStr1, MARK_BLANK, (size_t)pCmdLine->wBlankSum[i]);
    pTempStr1[pCmdLine->wBlankSum[i]] = MARK_STRINGEND;
    XOS_snprintf(pOutStr, dwBufSize, "%s%s", pOutStr, pTempStr1);
    /* add the last word and blanks */
    if (bIncludeLast)
    {
        XOS_snprintf(pOutStr, dwBufSize, "%s%s", pOutStr, pCmdLine->sCmdWord[i]);
        i++;
        memset(pTempStr1, MARK_BLANK, (size_t)pCmdLine->wBlankSum[i]);
        pTempStr1[pCmdLine->wBlankSum[i]] = MARK_STRINGEND;
        XOS_snprintf(pOutStr, dwBufSize, "%s%s", pOutStr, pTempStr1);
    }
}

/* combine the name and the comment */
void CombineNameComment(TYPE_LINK_STATE *pLinkState,
                        WORD16 wFatherPos, CHAR *pOutStr)
{
    TYPE_MATCH_RESULT *pMatchResult = &(pLinkState->tMatchResult);
    T_OAM_Cfg_Dat* cfgDat = pLinkState->tMatchResult.cfgDat;/*gtTreeNode[wFatherPos].cfgDat;*/
    TYPE_CMD_LINE *pCmdLine = &(pLinkState->tCmdLine);
    int i = 0;
    int nMaxLen = 0;
    int nWordLen = 0;
    int nLen = 0;
    WORD16 wCmdAttrPos = pMatchResult->wCmdAttrPos;
    WORD16 wParaBeginPos = pMatchResult->wParaBeginPos;
    WORD16 wParaEndPos = pMatchResult->wParaEndPos;

    TYPE_CMD_PARA *pCmdParaInfo = NULL;

    *pTempStr1 = MARK_STRINGEND;
    *pTempStr2 = MARK_STRINGEND;

    /* get the tree node or parameters */
    if (pMatchResult->wParseState == PARSESTATE_WORD_DETERMINER)
    {
        GatherTreeNode(pLinkState, wFatherPos);

        /*如果节点是降级(dat无效),不搜集参数部分信息*/
        if (cfgDat && cfgDat->bIsValid)
        {
            if (NeedMatchPara(pLinkState, wCmdAttrPos, cfgDat))
                GatherParameter(pLinkState, (WORD16)(wParaBeginPos - 1), wParaEndPos, TRUE);
        }
    }
    else if (pMatchResult->wParseState == PARSESTATE_PARA_DETERMINER)
    {
        if(pMatchResult->bPrammarRecycle)
            GatherParameter(pLinkState, (WORD16)(wFatherPos - 1), wParaEndPos, TRUE);
        else
            GatherParameter(pLinkState, wFatherPos, wParaEndPos, TRUE);
    }

    if (pMatchResult->wParseState == PARSESTATE_PARA_EXCEPTION)
    {
        DealParaException(pLinkState);
        return;
    }

    /* calculate the max len of the name */
    for (i = 0; i < pMatchResult->wHintStackTop; i++)
    {
        if(pMatchResult->tHint[i].wType == PATH_TITLE)    /* 抬头 */
            continue;
        nWordLen = strlen(pMatchResult->tHint[i].pName);
        if (nWordLen > nMaxLen)
            nMaxLen = nWordLen;
    }
    nMaxLen += 4;
    /* combine */
    /* 1 head */
    if (!pMatchResult->bHasNo && pMatchResult->wDepth == 0)
    {
        TYPE_MODE *pCurMode = (TYPE_MODE *)Oam_GetCurMode(pLinkState);
        if (pCurMode)
        {
            XOS_snprintf(pOutStr, MAX_OUTPUT_RESULT - 1, "%s%s", pOutStr, pCurMode->sModeHelp);
        }
    }
    /* 2 combine the name and the comment */
    for (i = 0; i < pMatchResult->wHintStackTop; i++)
    {
        if(pMatchResult->tHint[i].wType == PATH_TITLE)    /* 抬头 */
        {
            /* 修改为有注解才显示抬头, by majunhui */
            if (pMatchResult->tHint[i].pComment != NULL && strlen(pMatchResult->tHint[i].pComment) != 0)
            {
                int nLenTmp = XOS_snprintf(pTempStr1, sizeof(pTempStr1), "  ");
                if (cfgDat && cfgDat->tCmdPara[pMatchResult->tHint[i].wPosition].bStructType == STRUCTTYPE_PARA)
                    nLenTmp += XOS_snprintf(pTempStr1 + nLenTmp, sizeof(pTempStr1) - nLenTmp, CLI_szCLIHelp6, pMatchResult->tHint[i].pComment);
                else
                {
                    nLenTmp += XOS_snprintf(pTempStr1 + nLenTmp, sizeof(pTempStr1) - nLenTmp, CLI_szCLIHelp5);
                    /*对于列表选择参数，亦需要列出该参数的注解；如果无注解，则不列出*/
                    if (pMatchResult->tHint[i].pComment != NULL && strlen(pMatchResult->tHint[i].pComment) != 0)
                    {
                        /* 加左括号'['*/
                        nLenTmp += XOS_snprintf(pTempStr1 + nLenTmp, sizeof(pTempStr1) - nLenTmp,"[");

                        nLenTmp += XOS_snprintf(pTempStr1 + nLenTmp, sizeof(pTempStr1) - nLenTmp, pMatchResult->tHint[i].pComment);
                        /* 加右括号']'*/
                        nLenTmp += XOS_snprintf(pTempStr1 + nLenTmp, sizeof(pTempStr1) - nLenTmp, "]");
                    }
                }
                XOS_snprintf(pTempStr1 + nLenTmp, sizeof(pTempStr1) - nLenTmp, "\n");
                XOS_snprintf(pOutStr, MAX_OUTPUT_RESULT - 1, "%s%s", pOutStr, pTempStr1);
            }
            continue;
        }
        /* other case */
        /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
        XOS_snprintf(pTempStr2,
                     sizeof(pTempStr2) -1,
                     "%s",
                     pMatchResult->tHint[i].pComment);

        if(pMatchResult->tHint[i].wType != PATH_COMMAND && cfgDat)    /* parameter */
        {
            pCmdParaInfo = (TYPE_CMD_PARA *) &(cfgDat->tCmdPara[pMatchResult->tHint[i].wPosition]);
            if(pCmdParaInfo->bStructType == STRUCTTYPE_PARA)
                GetParaFormat(pCmdParaInfo, pTempStr2, sizeof(pTempStr2));
        }

        XOS_snprintf(pTempStr1, sizeof(pTempStr1),"  %s", pMatchResult->tHint[i].pName);
        XOS_snprintf(pOutStr, MAX_OUTPUT_RESULT - 1, "%s%s", pOutStr, pTempStr1);

        /*if the max name string len more than 80*/
        if(nMaxLen > MAX_LINEWIDTH)
        {
            int nTemp = strlen(pTempStr1);
            if( nTemp > MAX_LINEWIDTH - 1)
            {
                nLen = 0 ;
            }
            else
            {
                nLen = MAX_LINEWIDTH - 1 - strlen(pTempStr1);
            }
        }
        else
            nLen = nMaxLen - strlen(pTempStr1);
        /*if name is map context, outstring no need append space;
          else calc length*/
        /*
        {
            CHAR *pBarstr = strstr(pTempStr1, "-");
            if(pBarstr != NULL) nLen = 0;
            else 
            {
                nLen = nMaxLen - strlen(pTempStr1);
                if(nLen >= 40 )nLen =4;
            }
        }
        if(nLen == 0)nLen =4;
        */
        memset(pTempStr1, MARK_BLANK, (size_t)nLen);
        pTempStr1[nLen] = MARK_STRINGEND;
        XOS_snprintf(pOutStr, MAX_OUTPUT_RESULT - 1, "%s%s", pOutStr, pTempStr1);

        if (strlen(pTempStr2) + nMaxLen > MAX_LINEWIDTH)
        {
            int nSplitPos = MAX_LINEWIDTH - nMaxLen;
            if (nSplitPos >= 0)
            {
                while (pTempStr2[nSplitPos] != MARK_BLANK)
                {
                    nSplitPos--;
                    if ( 0 == nSplitPos)
                    {
                        break;
                    }
                }
                memcpy(pTempStr1, pTempStr2, (size_t)nSplitPos);
                pTempStr1[nSplitPos] = MARK_STRINGEND;
                XOS_snprintf(pTempStr1, MAX_STRING_LEN - 1, "%s\n", pTempStr1);
                XOS_snprintf(pOutStr, MAX_OUTPUT_RESULT - 1, "%s%s", pOutStr, pTempStr1);
                memset(pTempStr1, MARK_BLANK, (size_t)nMaxLen);
                pTempStr1[nMaxLen] = MARK_STRINGEND;
                if (nSplitPos == 0)
                    XOS_snprintf(pTempStr1, MAX_STRING_LEN - 1, "%s%s", pTempStr1, (CHAR *)&(pTempStr2[nSplitPos]));
                else
                    XOS_snprintf(pTempStr1, MAX_STRING_LEN - 1, "%s%s", pTempStr1, (CHAR *)&(pTempStr2[nSplitPos + 1]));
                    
                XOS_snprintf(pOutStr, MAX_OUTPUT_RESULT - 1, "%s%s", pOutStr, pTempStr1);
            }
        }
        else
            XOS_snprintf(pOutStr, MAX_OUTPUT_RESULT - 1, "%s%s", pOutStr, pTempStr2);

        XOS_snprintf(pOutStr, MAX_OUTPUT_RESULT - 1, "%s\n", pOutStr);
        
        /* 当参数很多或者MAP值很多，导致超过最大长度时，需要分包显示 */
        if (strlen(pOutStr) >= (MAX_OUTPUT_RESULT - 512)) /* 这里减掉512是为了尽量保证小于MAX_OUTPUT_RESULT */
        {
            TYPE_DISP_GLOBAL_PARA *pGlobal = NULL;
            
            pGlobal = &(pLinkState->tDispGlobalPara);
            pLinkState->bOutputResult = TRUE;
            SendPacketToTelnet(pLinkState, TRUE);
            *(pGlobal->pDisplayBuffer) = MARK_STRINGEND;    /* 初始化显示缓冲区 */
            memset(pGlobal->pDisplayBuffer, 0, MAX_OUTPUT_RESULT);
            pGlobal->pDispBuff = pGlobal->pDisplayBuffer;
            pLinkState->wSendPos = 0;
        }
        
    }
    /* add <CR> */
    if ((!pMatchResult->bCanExecute) && (wCmdAttrPos != 0) &&
            (!NeedMatchPara(pLinkState, pMatchResult->wCmdAttrPos, pMatchResult->cfgDat)) &&
            cfgDat)
    {
        pMatchResult->bCanExecute = TRUE;
        /*对于其下子节点有no命令，其本身却不支持no命令，需要重新判断*/
        if( pCmdLine->bHasNo && !CMD_ATTR_HASNO(&cfgDat->tCmdAttr[wCmdAttrPos]))
            pMatchResult->bCanExecute = FALSE;
    }

    if (pMatchResult->bCanExecute)
    {
        XOS_snprintf(pOutStr, MAX_OUTPUT_RESULT - 1, "%s  %s\n", pOutStr, Hdr_szEnterString);
    }
}
/* 结束MoreAngle语法 */
BOOL EndMoreAngle(TYPE_MATCH_RESULT *pMatchResult,
                  WORD16 wParaBeginPos, WORD16 wParaEndPos, WORD16 wFirstParaPos)
{
    TYPE_PRAMMAR *pPrammarInfo;
    T_OAM_Cfg_Dat* cfgDat = pMatchResult->cfgDat;

    if (pMatchResult->bContinueSearch[MORE_LAYER_TOP]
            && (pMatchResult->wMoreEndPos[MORE_LAYER_TOP] > 0)
            && (wFirstParaPos > pMatchResult->wMoreEndPos[MORE_LAYER_TOP]))
    {
        ClearMoreLayer(pMatchResult);
        /* 语法结束 */
        JumpNestingPrammar(pMatchResult, wParaBeginPos, wParaEndPos);
        if ((pMatchResult->wCurParaPos + 1) >= wParaEndPos)
            pMatchResult->bCanExecute = TRUE;
        /* 对于多个MORE_ANGLE语法的引入，这是很关键的一步。
        其意义在于不让语法解析在第一个MORE_ANGLE后结束。
        */
        /* 对于计算(JumpNestingPrammar)后的参数位置处于结束位置，则终止命令解析 */
        if(pMatchResult->wCurParaPos + 1 == wParaEndPos)
            return TRUE;

        pPrammarInfo = (TYPE_PRAMMAR *) &(cfgDat->tCmdPara[wFirstParaPos].uContent.tPrammar);
        if(pPrammarInfo->bPrammarType  == PRAMMAR_MORE_ANGLE
                || pPrammarInfo->bPrammarType  == PRAMMAR_RECYCLE)
        {
            WORD16 wPos = pMatchResult->wCurParaPos;
            pMatchResult->bType[pMatchResult->bMoreLayerTop] = pPrammarInfo->bPrammarType;
            if(pPrammarInfo->bPrammarType  == PRAMMAR_MORE_ANGLE)
                pMatchResult->bContinueSearch[pMatchResult->bMoreLayerTop] = TRUE;
            /* 记下语法的范围 */
            pMatchResult->wMoreBeginPos[pMatchResult->bMoreLayerTop] = wFirstParaPos;
            pMatchResult->wCurParaPos = pMatchResult->wMoreBeginPos[pMatchResult->bMoreLayerTop];
            SkipCurPrammar(pMatchResult);
            pMatchResult->wMoreEndPos[pMatchResult->bMoreLayerTop] = pMatchResult->wCurParaPos;
            pMatchResult->wCurParaPos = wPos;
            ++pMatchResult->bMoreLayerTop;
        }
        return FALSE;
    }
    return FALSE;
}

BOOL ForceMoreOne(TYPE_MATCH_RESULT *pMatchResult, WORD16 wFirstParaPos)
{
    WORD16 wLoop;
    WORD16 wMoreBeginPos = pMatchResult->wMoreBeginPos[MORE_LAYER_TOP];
    WORD16 wMoreEndPos = pMatchResult->wMoreEndPos[MORE_LAYER_TOP];

    if (pMatchResult->bMoreLayerTop == 0)
        return FALSE;
    if (!pMatchResult->bContinueSearch[MORE_LAYER_TOP])
        return TRUE;
    if (pMatchResult->wMoreEndPos[MORE_LAYER_TOP] == 0)
        return TRUE;
    if (wFirstParaPos <= pMatchResult->wMoreEndPos[MORE_LAYER_TOP])
        return FALSE;

    if (!MORE_FLAG_ONE(pMatchResult->bMoreFlags[MORE_LAYER_TOP]))
        return FALSE;

    for (wLoop = 0; wLoop < pMatchResult->wMatchedPathTop; wLoop++)
    {
        if ((pMatchResult->tMatchedPath[wLoop].wPosition >= wMoreBeginPos)
                && (pMatchResult->tMatchedPath[wLoop].wPosition <= wMoreEndPos))
            return FALSE;
    }

    return TRUE;
}

/* combine the name and the comment of parameter */
void GatherParameter(TYPE_LINK_STATE *pLinkState, WORD16 wForwardPos,
                     WORD16 wParaEndPos, BYTE bQuestion)
{
    TYPE_MATCH_RESULT *pMatchResult = &(pLinkState->tMatchResult);
    T_OAM_Cfg_Dat* cfgDat = pMatchResult->cfgDat;
    TYPE_HINT *pHint = &(pMatchResult->tHint[pMatchResult->wHintStackTop]);

    WORD16 wFirstParaPos = wForwardPos + 1;
    WORD16 wNextParaPos = 0;
    WORD16 wGatherPos = wFirstParaPos;
    WORD16 wParaBeginPos = pMatchResult->wParaBeginPos;
    int i =  0;
    int j = 0;

    /* save the current parameter position */
    WORD16 wCurParaPos = pMatchResult->wCurParaPos;

    TYPE_PARA *pParameterInfo       = NULL;     /* information about the parameter */
    TYPE_PRAMMAR *pPrammarInfo  = NULL;     /* information about the prammar */
    TYPE_CMD_PARA *pNextCmdParaInfo = NULL;

    WORD16 wMatchedPathTop = pMatchResult->wMatchedPathTop;

    memset(pHint, 0, sizeof(TYPE_HINT));
    
    /* check */
    if (wFirstParaPos >= wParaEndPos)
    {
        pMatchResult->bCanExecute = TRUE;
        return;
    }
    else
        pMatchResult->bCanExecute = FALSE;

    /*For MORE_ANGLE语法,不在当前{[][][]}内清栈*/

    if(bQuestion && pMatchResult->wMatchedPathTop
            && cfgDat->tCmdPara[wFirstParaPos].uContent.tPrammar.bPrammarType == PRAMMAR_MORE_ANGLE
            && wFirstParaPos > pMatchResult->wMoreEndPos[MORE_LAYER_TOP])
        pMatchResult->wMatchedPathTop = 0;

    /* gather name and comment */
    switch(cfgDat->tCmdPara[wFirstParaPos].bStructType)
    {
    case STRUCTTYPE_PARA:
        if (EndMoreAngle(pMatchResult, wParaBeginPos, wParaEndPos, wFirstParaPos))
            break;
        pParameterInfo = (TYPE_PARA *) &(cfgDat->tCmdPara[wGatherPos].uContent.tPara);
        /* save name and comment */
        if (pParameterInfo->bParaType != DATA_TYPE_MAP)
        {
            pHint->pName     = pParameterInfo->sName;
            pHint->pComment  = pParameterInfo->sComment;
            pHint->wPosition = wFirstParaPos;
            pHint->wType     = 0;
            /* change the stack */
            pMatchResult->wHintStackTop++;
            if (pMatchResult->wHintStackTop > MAX_HINTSUM)
            {
                pMatchResult->wParseState = PARSESTATE_PARA_EXCEPTION;
                pMatchResult->bCanExecute = FALSE;
                return;
            }
        }
        else/* DATA_TYPE_MAP */
        {
            int iTmp = 0;
            /* title */
            if (bQuestion)
            {
                pHint->pName     = pParameterInfo->sName;
                pHint->pComment  = pParameterInfo->sComment;
                pHint->wPosition = wFirstParaPos;
                pHint->wType     = PATH_TITLE;
                /* change the stack */
                pMatchResult->wHintStackTop++;
				
                if (pMatchResult->wHintStackTop > MAX_HINTSUM)
                {
                    pMatchResult->wParseState = PARSESTATE_PARA_EXCEPTION;
                    pMatchResult->bCanExecute = FALSE;
                    return;
                }

                /* get next hint */
                pHint = &(pMatchResult->tHint[pMatchResult->wHintStackTop]);
            }
            /* find */
            {
                static CHAR szBlank[] = " ";
                DWORD dwTypePos = FindMapTypePos(cfgDat,pParameterInfo->wLen);
                DWORD dwEndItemPos = cfgDat->tMapType[dwTypePos].dwItemPos + cfgDat->tMapType[dwTypePos].wCount;
                /*区分DATA_TYPE_FLOAT*/
                WORD64 qdwMin = 0;
                WORD64 qdwMax = 0;
                if(pParameterInfo->bParaType == DATA_TYPE_FLOAT)
                {
                    /*qdwMin = pParameterInfo->Min.fMinVal;*/
                    /*qdwMax = pParameterInfo->Max.fMaxVal;*/
                }
                else
                {
                    qdwMin = pParameterInfo->Min.qdwMinVal;
                    qdwMax = pParameterInfo->Max.qdwMaxVal;
                }
                for (iTmp = cfgDat->tMapType[dwTypePos].dwItemPos; iTmp < (int)dwEndItemPos; iTmp++)
                {
                    if (cfgDat->tMapItem[iTmp].wValue >= (WORD16)qdwMin
                            && cfgDat->tMapItem[iTmp].wValue <= (WORD16)qdwMax)
                    {
                        pMatchResult->tHint[pMatchResult->wHintStackTop].pName =
                            (CHAR *) &cfgDat->tMapString[cfgDat->tMapItem[iTmp].dwKeyPos];
                        if (cfgDat->tMapItem[iTmp].dwCommentPos)
                            pMatchResult->tHint[pMatchResult->wHintStackTop].pComment =
                                (CHAR *) &cfgDat->tMapString[cfgDat->tMapItem[iTmp].dwCommentPos];
                        else
                            pMatchResult->tHint[pMatchResult->wHintStackTop].pComment =
                                szBlank;
                        pMatchResult->tHint[pMatchResult->wHintStackTop].wType =
                            cfgDat->tMapType[dwTypePos].wType;
                        pMatchResult->tHint[pMatchResult->wHintStackTop].wPosition = wGatherPos;
                        pMatchResult->wHintStackTop++;
                        if (pMatchResult->wHintStackTop > MAX_HINTSUM)
                        {
                            pMatchResult->wParseState = PARSESTATE_PARA_EXCEPTION;
                            pMatchResult->bCanExecute = FALSE;
                            return;
                        }
                    }
                }
            }
        }
        break;

    case STRUCTTYPE_PRAMMAR:
        pPrammarInfo = (TYPE_PRAMMAR *) &(cfgDat->tCmdPara[wFirstParaPos].uContent.tPrammar);
        switch(pPrammarInfo->bPrammarType)
        {
        case PRAMMAR_BRACKET_SQUARE:    /* [] */
            /* 至少选择MoreAngle的一项，不可以提示下面的选项 */
            if (ForceMoreOne(pMatchResult, wFirstParaPos))
            {
                /* 语法结束 */
                ClearMoreLayer(pMatchResult);
                break;
            }

            /* gather this one */
            if (*(pPrammarInfo->sSymbol))
            {
                /* save name and comment */
                pHint->pName     = pPrammarInfo->sSymbol;
                pHint->pComment  = pPrammarInfo->sComment;
                pHint->wPosition = wFirstParaPos;
                pHint->wType     = 0;
                /* change the stack top */
                pMatchResult->wHintStackTop++;
                if (pMatchResult->wHintStackTop > MAX_HINTSUM)
                {
                    pMatchResult->wParseState = PARSESTATE_PARA_EXCEPTION;
                    pMatchResult->bCanExecute = FALSE;
                    return;
                }
            }
            else
            {
                if (IfNoSymbolPrammarInMoreAngleIsMatched(wGatherPos+1, wGatherPos+pPrammarInfo->bItemCount, pMatchResult))
                {
                    pMatchResult->wCurParaPos = wGatherPos;
                    SkipCurPrammar(pMatchResult);
                    wGatherPos = pMatchResult->wCurParaPos;
                    /* gather the name and comment of the first paramter of this prammar */
                 //   GatherParameter(pLinkState, (WORD16)pMatchResult->wCurParaPos, wParaEndPos, bQuestion);
                }
                else
                {
                    /* gather the name and comment of the first paramter of this prammar */
                    GatherParameter(pLinkState, (WORD16)wGatherPos, wParaEndPos, bQuestion);
                }
            }
            /* get wNextParaPos */
            pMatchResult->wCurParaPos = wGatherPos;
            SkipCurPrammar(pMatchResult);
            wGatherPos = pMatchResult->wCurParaPos;

            if(pMatchResult->bContinueSearch[MORE_LAYER_TOP]) /* {[][][]} */
            {
                if(pMatchResult->wMatchedPathTop > 0)
                {
                    for(j = 0; j < pMatchResult->wMatchedPathTop; j++)
                    {
                        if(pHint->wPosition == pMatchResult->tMatchedPath[j].wPosition)
                        {
                            pMatchResult->tHint[pMatchResult->wHintStackTop - 1].pName      = NULL;
                            pMatchResult->tHint[pMatchResult->wHintStackTop - 1].pComment   = NULL;
                            pMatchResult->tHint[pMatchResult->wHintStackTop - 1].wPosition  = INVALID_NODE;
                            pMatchResult->wHintStackTop--;

                            pMatchResult->wCurParaPos = wGatherPos;
                            SkipCurPrammar(pMatchResult);
                            wGatherPos = pMatchResult->wCurParaPos;
                            break;
                        }
                    }
                    /* 继续向后搜索 */
                    GatherParameter(pLinkState, wGatherPos, wParaEndPos, bQuestion);
                    pMatchResult->wCurParaPos = wGatherPos;
                    SkipCurPrammar(pMatchResult);
                }
                else
                {
                    if (wGatherPos < pMatchResult->wMoreEndPos[MORE_LAYER_TOP])
                        GatherParameter(pLinkState, (WORD16)wGatherPos, wParaEndPos, bQuestion);
                    else if ((wGatherPos < pMatchResult->wParaEndPos) && !MORE_FLAG_ONE(pMatchResult->bMoreFlags[MORE_LAYER_TOP]))
                        GatherParameter(pLinkState, (WORD16)wGatherPos, wParaEndPos, bQuestion);
                    else
                        ClearMoreLayer(pMatchResult);
                }
            }
            else /* 其他情况 */
            {
                JumpNestingPrammar(pMatchResult, wParaBeginPos, wParaEndPos);
                wGatherPos = pMatchResult->wCurParaPos;
                if ((wGatherPos + 1) < wParaEndPos)
                    GatherParameter(pLinkState, (WORD16)wGatherPos, wParaEndPos, bQuestion);
                else
                    pMatchResult->bCanExecute = TRUE;
            }
            /* get the saved value */
            pMatchResult->wCurParaPos = wCurParaPos;
            /*for MORE_ANGLE,恢复原来的匹配栈*/
            pMatchResult->wMatchedPathTop = wMatchedPathTop;
            break;
        case PRAMMAR_BRACKET_ANGLE:     /* {} */
            if (EndMoreAngle(pMatchResult, wParaBeginPos, wParaEndPos, wFirstParaPos))
                break;
            /* title */
            if (bQuestion)
            {
                pHint->pName     = pPrammarInfo->sSymbol;
                pHint->pComment  = pPrammarInfo->sComment;
                pHint->wPosition = wFirstParaPos;
                pHint->wType     = PATH_TITLE;
                /* change the stack */
                pMatchResult->wHintStackTop++;

                if (pMatchResult->wHintStackTop > MAX_HINTSUM)
                {
                    pMatchResult->wParseState = PARSESTATE_PARA_EXCEPTION;
                    pMatchResult->bCanExecute = FALSE;
                    return;
                }
                /* get next hint */
                pHint = &(pMatchResult->tHint[pMatchResult->wHintStackTop]);
            }
            /* gather one by one */
            for (i = 0; i < pPrammarInfo->bItemCount; i++)
            {
                pNextCmdParaInfo = (TYPE_CMD_PARA *) &(cfgDat->tCmdPara[wGatherPos + 1]);
                /* get wNextParaPos (new wForwardPos) */
                if (pNextCmdParaInfo->bStructType == STRUCTTYPE_PRAMMAR)
                {
                    /* get wNextParaPos */
                    pMatchResult->wCurParaPos = wGatherPos + 1;
                    SkipCurPrammar(pMatchResult);
                    wNextParaPos = pMatchResult->wCurParaPos; /* ???? */
                }
                else/* get wNextParaPos */
                    wNextParaPos = wGatherPos + 1;
                /* gather */
                GatherParameter(pLinkState, wGatherPos, wParaEndPos, bQuestion);
                wGatherPos = wNextParaPos;
            }
            /* get the saved value */
            pMatchResult->wCurParaPos = wCurParaPos;
            /*for MORE_ANGLE,恢复原来的匹配栈*/
            pMatchResult->wMatchedPathTop = wMatchedPathTop;
            break;

        case PRAMMAR_WHOLE_SYMBOL:              /* WHOLE: 连续的若干参数做一个整体 */
            if (EndMoreAngle(pMatchResult, wParaBeginPos, wParaEndPos, wFirstParaPos))
                break;
            /* save name and comment */
            pHint->pName = pPrammarInfo->sSymbol;
            pHint->pComment = pPrammarInfo->sComment;
            pHint->wPosition = wFirstParaPos;
            pHint->wType     = 0;
            /* change the stack top */
            pMatchResult->wHintStackTop++;
            if (pMatchResult->wHintStackTop > MAX_HINTSUM)
            {
                pMatchResult->wParseState = PARSESTATE_PARA_EXCEPTION;
                pMatchResult->bCanExecute = FALSE;
                return;
            }
            break;

        case PRAMMAR_MORE_ANGLE:        /* {[][][]} */
            if (EndMoreAngle(pMatchResult, wParaBeginPos, wParaEndPos, wFirstParaPos))
                break;
            pMatchResult->bContinueSearch[pMatchResult->bMoreLayerTop] = TRUE;
            /* 记下PRAMMAR_MORE_ANGLE的是否至少选择一项的标志 */
            pMatchResult->bMoreFlags[pMatchResult->bMoreLayerTop] = pPrammarInfo->bMoreFlags;
            if (bQuestion)
            {
                /*由于MORE_ANGLE没有symbol和comment,故置空*/
                pPrammarInfo->sSymbol[0] = '\0';
                pPrammarInfo->sComment[0] = '\0';

                pHint->pName     = pPrammarInfo->sSymbol;
                pHint->pComment  = pPrammarInfo->sComment;
                pHint->wPosition = wFirstParaPos;
                pHint->wType     = PATH_TITLE;
                /* change the stack */
                pMatchResult->wHintStackTop++;
                if (pMatchResult->wHintStackTop > MAX_HINTSUM)
                {
                    pMatchResult->wParseState = PARSESTATE_PARA_EXCEPTION;
                    pMatchResult->bCanExecute = FALSE;
                    return;
                }

                /* get next hint */
                pHint = &(pMatchResult->tHint[pMatchResult->wHintStackTop]);
            }
        case PRAMMAR_RECYCLE:
            if ((pPrammarInfo->bPrammarType == PRAMMAR_RECYCLE)
                    && EndMoreAngle(pMatchResult, wParaBeginPos, wParaEndPos, wFirstParaPos))
                break;
            {
                WORD16 wPos = pMatchResult->wCurParaPos;
                pMatchResult->bType[pMatchResult->bMoreLayerTop] = pPrammarInfo->bPrammarType;
                /* 记下语法的范围 */
                pMatchResult->wMoreBeginPos[pMatchResult->bMoreLayerTop] = wFirstParaPos;
                pMatchResult->wCurParaPos = pMatchResult->wMoreBeginPos[pMatchResult->bMoreLayerTop];
                SkipCurPrammar(pMatchResult);
                pMatchResult->wMoreEndPos[pMatchResult->bMoreLayerTop] = pMatchResult->wCurParaPos;
                pMatchResult->wCurParaPos = wPos;
                ++pMatchResult->bMoreLayerTop;
            }
        case PRAMMAR_WHOLE_NOSYM:
            if ((pPrammarInfo->bPrammarType == PRAMMAR_WHOLE_NOSYM)
                    && EndMoreAngle(pMatchResult, wParaBeginPos, wParaEndPos, wFirstParaPos))
                break;
            GatherParameter(pLinkState, wGatherPos, wParaEndPos, bQuestion);
            if(pMatchResult->bPrammarRecycle)   /* 若recycle已匹配过，则继续向后搜索 */
            {
                pMatchResult->wCurParaPos = pMatchResult->wMoreEndPos[MORE_LAYER_TOP];
                JumpNestingPrammar(pMatchResult, wParaBeginPos, wParaEndPos);
                GatherParameter(pLinkState, pMatchResult->wCurParaPos, wParaEndPos, bQuestion);
            }
            break;

        default:
            break;
        }
        break;
    default:
        /* error: no such structure type */
        break;
    }
}

BOOLEAN IfNoSymbolPrammarInMoreAngleIsMatched(WORD16 wSubParaBeginPos, WORD16 wSubParaEndPos, TYPE_MATCH_RESULT *pMatchResult)
{
    T_OAM_Cfg_Dat* cfgDat = NULL;
    WORD16 wIndex = 0;
    
    if (!pMatchResult)
    {
        return FALSE;
    }
    cfgDat = pMatchResult->cfgDat;

    for (wIndex = wSubParaBeginPos; wIndex <= wSubParaEndPos; wIndex++)
    {
        TYPE_CMD_PARA *pCmdPara =  &(cfgDat->tCmdPara[wIndex]);
        if (pCmdPara->bStructType == STRUCTTYPE_PRAMMAR)
        {
            TYPE_PRAMMAR *pPrammarInfo = (TYPE_PRAMMAR *) &(pCmdPara->uContent.tPrammar);
            return IfNoSymbolPrammarInMoreAngleIsMatched(wIndex+1, wIndex+pPrammarInfo->bItemCount, pMatchResult);
        }
        else
        {
            WORD16 j = 0;
            /* TYPE_PARA *pParaInfo =  (TYPE_PARA *) &(pCmdPara->uContent.tPara); */
            for(j = 0; j < pMatchResult->wMatchedPathTop; j++)
            {
                if(wIndex == pMatchResult->tMatchedPath[j].wPosition)
                {
                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}

/* calculate the position of matching last */
int CalculateMatchPosition(TYPE_LINK_STATE *pLinkState)
{
    TYPE_CMD_LINE *pCmdLine = &(pLinkState->tCmdLine);
    TYPE_MATCH_RESULT *pMatchResult = &(pLinkState->tMatchResult);

    int nPos = 0;
    int nIndex = 0;

    for (nIndex = 0; nIndex < pMatchResult->wDepth; nIndex++)
    {
        nPos += pCmdLine->wBlankSum[nIndex];
        nPos += strlen(pCmdLine->sCmdWord[nIndex]);
    }
    nPos += pCmdLine->wBlankSum[nIndex];
    nPos += pMatchResult->wLength;

    return nPos;
}

/* calculate the max length of TAB */
void CalculateWordTabMax(TYPE_MATCH_RESULT *pMatchResult,
                         CHAR *pOutStr)
{
    WORD16 wFirst = pMatchResult->wFirst;
    WORD16 wSum = pMatchResult->wSum;
    int i = 0;
    int j = 0;
    int nMaxLen = 0;
    int nTempLen = 0;

    *pOutStr = MARK_STRINGEND;
    /* check */
    if (wSum <= 1)
        return;
    /* get the mix len of string */
    nMaxLen = strlen(pMatchResult->tHint[wFirst].pName);
    for (i = 1; i < wSum; i++)
    {
        nTempLen = strlen(pMatchResult->tHint[wFirst + i].pName);
        if (nMaxLen > nTempLen)
            nMaxLen = nTempLen;
    }
    /* calculate */
    for (j = 0; j < nMaxLen; j++)
    {
        i = 0;
        while (i < wSum - 1 &&
                pMatchResult->tHint[wFirst + i].pName[j] ==
                pMatchResult->tHint[wFirst + i + 1].pName[j] )
            i++;
        if (i < wSum - 1)
            break;
    }
    if(pMatchResult->wDepth > 1)
        nMaxLen = j;
    /* else if (j > 1) majunhui 2007-08-06 */ /* the first cmd word at least has two identical CHAR */
    else if (j >= 1)
        nMaxLen = j;
    else
        nMaxLen = 0;
    memcpy(pOutStr, pMatchResult->tHint[wFirst].pName, (size_t)nMaxLen);
    pOutStr[nMaxLen] = MARK_STRINGEND;
}

/* skip parameters in the square or angle bracket */
/* change the wCurParaPos and bParaNum in the pMatchResult */
int SkipCurPrammar(TYPE_MATCH_RESULT *pMatchResult)
{
    int nParaSum = 0;
    WORD16 wCurPos = pMatchResult->wCurParaPos;
    T_OAM_Cfg_Dat* cfgDat = pMatchResult->cfgDat;

    /* if not prammar(include [] or {}) then return */
    if (cfgDat->tCmdPara[wCurPos].bStructType != STRUCTTYPE_PRAMMAR)
        return PARAERR_MISUSESTRUCT;
    nParaSum = cfgDat->tCmdPara[wCurPos].uContent.tPrammar.bItemCount;
    while (nParaSum > 0)
    {
        wCurPos++;  /* skip this para/[]/{} and go to the next */
        /* if wCurPos + 1 > maximum the return 0; */
        if (wCurPos > cfgDat->nCmdParaSum)
            return PARAERR_MAXIMUM;
        switch(cfgDat->tCmdPara[wCurPos].bStructType)
        {
        case STRUCTTYPE_PARA:
            nParaSum--;
            break;
        case STRUCTTYPE_PRAMMAR:
            nParaSum--;
            nParaSum += cfgDat->tCmdPara[wCurPos].uContent.tPrammar.bItemCount;
            break;
        default:
            /* error */
            pMatchResult->wCurParaPos = 0;
            return PARAERR_STRUCTTYPE;
        }
    }
    /* if wCurPos + 1 > = maximum then return 0; */
    if (wCurPos + 1 <= (WORD16)cfgDat->nCmdParaSum)
        pMatchResult->wCurParaPos = wCurPos;
    return PARA_SUCCESS;
}

/* from inside to outside, judge whether can jump the nesting prammars */
/* and set the pMatchResult->wCurParaPos as the position after jumping */
void JumpNestingPrammar(TYPE_MATCH_RESULT *pMatchResult, WORD16 wParaBeginPos, WORD16 wParaEndPos)
{
    WORD16 wCurParaPos = pMatchResult->wCurParaPos;
    WORD16 wTempPos = wCurParaPos;
    T_OAM_Cfg_Dat* cfgDat = pMatchResult->cfgDat;

    TYPE_PRAMMAR *pPrammarInfo     = NULL;
    TYPE_CMD_PARA *pCmdParaInfo    = (TYPE_CMD_PARA *) &(cfgDat->tCmdPara[wCurParaPos]);

    BYTE bEndWhile = FALSE;

    /* for [symbol parameter1 parameter2...],if the wCurParaPos is at symbol,return */
    /* prammar WHOLE_SYMBOL, return */
    if(pCmdParaInfo->bStructType == STRUCTTYPE_PRAMMAR &&
            ((pCmdParaInfo->uContent.tPrammar.bPrammarType == PRAMMAR_BRACKET_SQUARE && pCmdParaInfo->uContent.tPrammar.bItemCount > 0) ||
             pCmdParaInfo->uContent.tPrammar.bPrammarType == PRAMMAR_WHOLE_SYMBOL))
        return;

    if((pMatchResult->wCurParaPos + 1) == wParaEndPos) /* the last parameter */
        return;

    while( wTempPos >= wParaBeginPos && !bEndWhile )
    {
        pCmdParaInfo = (TYPE_CMD_PARA *) &(cfgDat->tCmdPara[wTempPos]);
        if( pCmdParaInfo->bStructType == STRUCTTYPE_PRAMMAR )
        {
            pPrammarInfo = (TYPE_PRAMMAR *)&(cfgDat->tCmdPara[wTempPos].uContent.tPrammar);

            pMatchResult->wCurParaPos = wTempPos;
            SkipCurPrammar(pMatchResult);

            --wTempPos;
            if (pMatchResult->wCurParaPos < wCurParaPos)
                continue;

            switch(pPrammarInfo->bPrammarType)
            {
            case PRAMMAR_BRACKET_SQUARE:
            case PRAMMAR_WHOLE_SYMBOL:
            case PRAMMAR_WHOLE_NOSYM:
            case PRAMMAR_RECYCLE:
                if (wCurParaPos < pMatchResult->wCurParaPos)
                {
                    pMatchResult->wCurParaPos = wCurParaPos;
                    bEndWhile = TRUE;
                }
                break;
            case PRAMMAR_BRACKET_ANGLE:
            case PRAMMAR_MORE_ANGLE:
                wCurParaPos = pMatchResult->wCurParaPos;
                break;
            default:
                break;
            }
        }
        else
            wTempPos--;
    }
    pMatchResult->wCurParaPos = wCurParaPos;
}

/* find default parameter,judge whether the cmd can execute. */
int FindDefaultParameter(TYPE_LINK_STATE *pLinkState)
{
    TYPE_MATCH_RESULT *pMatchResult = &(pLinkState->tMatchResult);
    T_OAM_Cfg_Dat* cfgDat = pMatchResult->cfgDat;
    WORD16 wCurParaPos = pMatchResult->wCurParaPos;
    int i = 0;
    int nResultVal = PARA_SUCCESS;

    TYPE_PRAMMAR *pPrammarInfo  = NULL; /* information about the prammar */
    TYPE_CMD_PARA *pCmdParaInfo = (TYPE_CMD_PARA *) &(cfgDat->tCmdPara[wCurParaPos]);  /* information about the command parameter */

    switch(pCmdParaInfo->bStructType)
    {
    case STRUCTTYPE_PARA:   /* parameter */
        nResultVal = PARAERR_NOFIND;
        if(pMatchResult->bPrammarRecycle)return 0;
        return nResultVal;

    case STRUCTTYPE_PRAMMAR:    /* prammar */
        pPrammarInfo = (TYPE_PRAMMAR *)&(pCmdParaInfo->uContent.tPrammar);
        switch(pPrammarInfo->bPrammarType)
        {
        case PRAMMAR_BRACKET_SQUARE:    /* [] */
            pMatchResult->wCurParaPos = wCurParaPos;
            SkipCurPrammar(pMatchResult);
            return nResultVal;

        case PRAMMAR_BRACKET_ANGLE: /* {} */
            /* match one by one, return PARA_SUCCESS only when these is one matched. */
            for (i = 0; i < pPrammarInfo->bItemCount; i++)
            {
                pMatchResult->wCurParaPos++;
                nResultVal = FindDefaultParameter(pLinkState);
                if (nResultVal == PARA_SUCCESS)
                {
                    pMatchResult->wCurParaPos = wCurParaPos;
                    SkipCurPrammar(pMatchResult);
                    break;
                }
            }
            return nResultVal;

        case PRAMMAR_WHOLE_NOSYM:   /* 特例：([][][]) */
            /* match one by one, return PARA_SUCCESS only when all are matched. */
            for (i = 0; i < pPrammarInfo->bItemCount; i++)
            {
                pMatchResult->wCurParaPos++;
                nResultVal = FindDefaultParameter(pLinkState);
                if (nResultVal != PARA_SUCCESS)
                {
                    pMatchResult->wCurParaPos = wCurParaPos;
                    SkipCurPrammar(pMatchResult);
                    return PARAERR_NOFIND;
                }
            }
            return nResultVal;

        case PRAMMAR_WHOLE_SYMBOL:
        case PRAMMAR_RECYCLE:
            pMatchResult->wCurParaPos = wCurParaPos;
            SkipCurPrammar(pMatchResult);
            nResultVal = PARAERR_NOFIND;
            return nResultVal;

        case PRAMMAR_MORE_ANGLE:        /* {[][][]} */
            pMatchResult->wCurParaPos = wCurParaPos;
            SkipCurPrammar(pMatchResult);
            if (MORE_FLAG_ONE(pPrammarInfo->bMoreFlags))
                nResultVal = PARAERR_NOFIND;
            return nResultVal;

        default:
            return PARAERR_PRAMMARTYPE;
        }
    default:
        return PARAERR_STRUCTTYPE;
    }
}

BOOLEAN HasPermissionExecuteCmd(TYPE_LINK_STATE *pLinkState)
{
    TYPE_MATCH_RESULT *pMatchResult = NULL;
    T_OAM_Cfg_Dat * cfgDat = NULL;
    DWORD dwCmdID = 0;
    CMDID *pGetGSFlag = NULL;
    BYTE ucOptType = 0;
    WORD32 dwCmdGrp = 0;

    if (NULL == pLinkState)
        return FALSE;

    pMatchResult = &pLinkState->tMatchResult;
    if (NULL == pMatchResult)
        return FALSE;
    
    cfgDat = pMatchResult->cfgDat;
    if (NULL == cfgDat)
        return FALSE;

    if(NULL == cfgDat->tCmdAttr)
        return FALSE;

    dwCmdID = cfgDat->tCmdAttr[pMatchResult->wCmdAttrPos].dExeID;
    
    dwCmdGrp = dwCmdID & 0xFF000000;
    if (dwCmdGrp == OAM_CLI_DDM_CMDGRP_SET)        // DDM命令SET类
        ucOptType = IS_SET;
    else if ((dwCmdGrp == OAM_CLI_DDM_CMDGRP_GET) ||
                (dwCmdGrp == OAM_CLI_DDM_CMDGRP_GETTABLE))   // DDM命令GET类
        ucOptType = IS_GET;
    else                                           // 普通CLI命令
    {
    pGetGSFlag = (CMDID *)(&dwCmdID);
    ucOptType = (BYTE)pGetGSFlag->OPM;
    }

    /* 查看用户只能查看业务配置 */
    if (USER_ROLE_VIEW == pLinkState->ucUserRight)
    {
        if ((IS_SET == ucOptType) || (IS_SAVE == ucOptType))
            return FALSE;
        else
            return TRUE;
    }
    
    return TRUE;
}

static void PrintValidTreeNode(T_TreeNode *pTreeNode, BYTE ucSonNum)
{
    WORD16 wTmpPos = 0;
    WORD16 wTmpPos2 = 0;
    BYTE ucSonCnt = ucSonNum;
    BYTE ucSonCnt2 = ucSonNum;
    WORD32 dwBlankCnt = 0;
    WORD32 i = 0;
    CHAR szBlank[128] = {'\0'};

    /* 计算空格的个数 */
    dwBlankCnt = ucSonCnt * 4;
    for (i = 0; i < dwBlankCnt; i++)
    {
        sprintf(szBlank, "%s ", szBlank);
    }
    /* 将当前节点打印出来 */
    if (TreeNodeIsValid(pTreeNode))
    {
        printf("%s%s\n", szBlank, pTreeNode->treenode.sName);
        if (pTreeNode->treenode.wNextSonPos != INVALID_NODE)
        {
            ucSonCnt++;
            PrintValidTreeNode(&gtTreeNode[pTreeNode->treenode.wNextSonPos], ucSonCnt);
        }
    }
    else
        return;

    /* 查找下一个节点 */
    wTmpPos = pTreeNode->treenode.wNextNodePos;
    while (INVALID_NODE != wTmpPos)
    {
        if (TreeNodeIsValid(&gtTreeNode[wTmpPos]))
        {
            printf("%s%s\n", szBlank, gtTreeNode[wTmpPos].treenode.sName);
            
            wTmpPos2 = gtTreeNode[wTmpPos].treenode.wNextSonPos;
            if (INVALID_NODE != wTmpPos2)
            {
                ucSonCnt2 = ucSonCnt;
                ucSonCnt++;
                PrintValidTreeNode(&gtTreeNode[wTmpPos2], ucSonCnt);
            }
        }

        wTmpPos = gtTreeNode[wTmpPos].treenode.wNextNodePos;
        ucSonCnt = ucSonCnt2;
    }
    
    return;
}

/*****************************************************************************/
/**
@brief 显示CLIMANAGER上内存中的cli命令树，用于定位命令丢失问题
@verbatim
无
@endverbatim

@li @b 返回值列表：
@verbatim
无
@endverbatim

@li @b 适用范围：
\n适用于OMP/CMM2单板，在oammanager下运行

@li @b 风险提示：
\n无
 
@li @b 其它说明：
\n1、命令树不同层次节点用缩进来表示
@li @b 输出示例：
\n(config-acl6)#
    end
    list
    show
        acl
        acl6
        aclgroup
        aclgroup6
        arp

@li @b 修改记录：
-#  ： 
*/
/*****************************************************************************/
void OAM_DbgShowCliCmdTree(void)
{
    WORD16 i = 0;
    WORD16 wTmpPos = 0;
    
    /* mode */
    printf("g_dwModeCount = %d\n", g_dwModeCount);
    for (i = 0; i < g_dwModeCount; i++)
    {
#if 0
        printf("============Mode Info==============\n");
        printf("      bModeID = %d\n", gtMode[i].bModeID);
        printf("      wModeClass = %d\n", gtMode[i].wModeClass);
        printf("      wTreeRootPos = %d\n", gtMode[i].wTreeRootPos);
        printf("      bHasNo = %d\n", gtMode[i].bHasNo);
        printf("      bReserved = %d\n", gtMode[i].bReserved);
        printf("      sOddPrompt is: %s\n", gtMode[i].sOddPrompt);
        printf("      sModeHelp is: %s\n", gtMode[i].sModeHelp);
#endif
        printf("\n%s\n", gtMode[i].sOddPrompt);
        wTmpPos = gtTreeNode[gtMode[i].wTreeRootPos].treenode.wNextSonPos;
        PrintValidTreeNode(&gtTreeNode[wTmpPos], 1);
    }

#if 0
    /* treenode */
    printf("### g_wFreeNode = %d\n", g_wFreeNode);
    for (i = 0; i < g_wFreeNode; i++)
    {
        printf("============TreeNode Info==============\n");
        printf("      wNextNodePos = %d\n", gtTreeNode[i].treenode.wNextNodePos);
        printf("      wNextSonPos = %d\n", gtTreeNode[i].treenode.wNextSonPos);
        printf("      wCmdAttrPos = %d\n", gtTreeNode[i].treenode.wCmdAttrPos);
        printf("      sName is: %s\n", gtTreeNode[i].treenode.sName);
        printf("      sComment is: %s\n", gtTreeNode[i].treenode.sComment);
        if (NULL != gtTreeNode[i].cfgDat)
            printf("      tJid.dwJno = %d\n", gtTreeNode[i].cfgDat->tJid.dwJno);
    }
#endif

    return;
}

/*lint +e794*/
/*lint +e171*/
/*lint +e744*/
/*lint +e539*/
/*lint +e661*/
/*lint +e578*/
/*lint +e676*/
/*lint +e671*/
/*lint +e578*/
/*lint +e922*/
/*lint +e744*/




