/**************************************************************************
* 版权所有 (C)2002, 深圳市中兴通讯股份有限公司。
*
* 文件名称：oam_execute.c
* 文件标识：
* 内容摘要：OAM执行进程
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
* 修改记录2：…
**************************************************************************/
/**************************************************************************
*                           头文件                                        *
**************************************************************************/
#include <ctype.h>

#include "includes.h"

#include "oam_execute.h"
#include "xmlagent.h"
#include "offline_to_online.h"

/* added by fls on 2009-06-29 (CRDCM00197912 SSC2单板联调，存盘ZDB失败，经定位是因为OAM与DBS定义的结构体不一致导致) */
//#include "db.h"
/* end of add */

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
extern COMMON_INFO gtCommonInfo;
BOOL bDebugPrint = FALSE/*FALSE*/;
/**************************************************************************
*                          本地变量                                       *
**************************************************************************/
extern T_OAM_INTERPRET_VAR *g_ptOamIntVar;

/**************************************************************************
*                          局部函数原型                                   *
**************************************************************************/

static void PrintDataType(BYTE bType);
static BYTE *PrintTable(BYTE *pData, BYTE bRecord, BYTE bTableType);

/*DB模式*/
static void ExecCLIProc(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *ptRcvMsg, MSG_COMM_OAM *ptRtnMsg);
static void ExecCLIGet(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *ptRcvMsg);
static void ExecCLISet(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *ptRcvMsg, MSG_COMM_OAM *ptRtnMsg);

static void GetOnlineUsers(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *ptRtnMsg);
static void GetUserName(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *ptRtnMsg);

static void SetUserName(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *ptRcvMsg, MSG_COMM_OAM *ptRtnMsg);
static void SetConsoleIdleTimeout(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *ptRcvMsg, MSG_COMM_OAM *ptRtnMsg);
static void SetConsoleAbsTimeout(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *ptRcvMsg, MSG_COMM_OAM *ptRtnMsg);
static void SetTelnetIdleTimeout(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *ptRcvMsg, MSG_COMM_OAM *ptRtnMsg);
static void SetTelnetAbsTimeout(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *ptRcvMsg, MSG_COMM_OAM *ptRtnMsg);

/*Protocol模式*/
static void ExecSaveCmdProc(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *ptRcvMsg);
static void SaveZDBProc(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *ptMsg);

/*检查消息头是否合法*/
static BOOL CheckMsgHead(MSG_COMM_OAM *ptMsg);


/*填充回显数据*/
static void ProcessAsycMsg(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *pMsg);
static void Oam_CmdReturnMsgHandler(TYPE_LINK_STATE *pLinkState,MSG_COMM_OAM *ptMsg);
static INT32 CheckExeMsgHead(TYPE_LINK_STATE *pLinkState,MSG_COMM_OAM * ptMsg);
static void ParseReturnData(TYPE_LINK_STATE *pLinkState,MSG_COMM_OAM * ptMsg);


/*static VOID CfgMsgTransAddrByteOder(MSG_COMM_OAM *ptr);*/

/**************************************************************************
*                          全局函数声明                                   *
**************************************************************************/
extern BOOL AddUserToLink(USER_NODE *pNode);
extern BOOL DelUserFromLink(CHAR *pUserName);

extern void GetTerminalCfgData(MSG_COMM_OAM **pRtnMsg);
extern void GetUserNameData(MSG_COMM_OAM **pRtnMsg);
/* added by fls on 2009-06-29 (CRDCM00197905 SSC2单板联调，OAM发送字节序固定为小尾字节序，没有按照OMP的字节序发送) */
/*extern BOOL Oam_CfgIsNeedInvertOrder(VOID);*/
/* end of add */

/**************************************************************************
*                          全局函数实现                                   *
**************************************************************************/

/**************************************************************************
* 函数名称：VOID Oam_RecvFromApp
* 功能描述：接收来自Protocol的消息
* 访问的表：无
* 修改的表：无
* 输入参数：
  LPVOID pMsgPara
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
void Oam_RecvFromApp(TYPE_LINK_STATE *pLinkState,LPVOID pMsgPara)
{
    MSG_COMM_OAM *ptMsgBuf = (MSG_COMM_OAM *)pMsgPara;    
    MSG_COMM_OAM *ptRcvMsg = (MSG_COMM_OAM *)g_ptOamIntVar->aucRecvBuf;
    WORD16 wMsgLen = (WORD16)ptMsgBuf->DataLen;
    BYTE ucCmdType = 0;
    BOOL bCmdExecRslt = FALSE;
    WORD32 dwCmdGrp = 0;

    memset(ptRcvMsg, 0, sizeof(g_ptOamIntVar->aucRecvBuf));
    wMsgLen = OAM_CFG_MIN(wMsgLen ,MAX_APPMSG_LEN);
    memcpy(ptRcvMsg, ptMsgBuf, wMsgLen);
 
    /*打印收发消息*/
    if (bDebugPrint)
    {
        PrintMSGCOMMOAMStruct(ptRcvMsg,"MSG_APP_TO_CLIMANAGER");
    }

    if (ptRcvMsg->ReturnCode == SUCC_AND_NOPARA ||
        ptRcvMsg->ReturnCode == SUCC_AND_HAVEPARA ||
        ptRcvMsg->ReturnCode == SUCC_CMD_NOT_FINISHED)
        bCmdExecRslt = TRUE;
    else
        bCmdExecRslt = FALSE;
    
    dwCmdGrp = ptRcvMsg->CmdID & 0xFF000000;
    if (dwCmdGrp == OAM_CLI_DDM_CMDGRP_SET)        /* DDM命令SET类*/
        ucCmdType = IS_SET;
    else if ((dwCmdGrp == OAM_CLI_DDM_CMDGRP_GET) ||
                (dwCmdGrp == OAM_CLI_DDM_CMDGRP_GETTABLE))   /* DDM命令GET类*/
        ucCmdType = IS_GET;
    else                                           // 普通CLI命令
        ucCmdType = CheckGetOrSet(ptRcvMsg->CmdID);
    
    if ((ptRcvMsg->ucOamTag == OAMCFG_TAG_NORMAL) && 
        (ucCmdType == IS_SET) &&
        (OAM_MSG_IS_LAST_PACKET(ptRcvMsg)) &&
        (ptRcvMsg->ReturnCode != CLI_RET_KEEP_ALIVE_NOPARA))
    {
        /* 写命令行日志 */
        if (!WriteLogOfCmdLine(&pLinkState->tCmdLog, bCmdExecRslt))
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, "[Oam_RecvFromApp] WriteLogOfCmdLine exec fail! line = %d\n", __LINE__);
        }
    }

    if (!CheckMsgHead(ptRcvMsg))
    {
        ptRcvMsg->ReturnCode = ERR_MSG_FROM_INTPR;
        ptRcvMsg->DataLen    = sizeof(MSG_COMM_OAM);
        Oam_CmdReturnMsgHandler(pLinkState,ptRcvMsg);
        return;
    }

    /*对特殊命令做预处理*/
	
    /*debug等异步消息seq返回为0，
       后面异步消息处理和同步消息不同*/
    pLinkState->bRecvAsynMsg = (0 == ptRcvMsg->SeqNo) ? TRUE : FALSE;
    if (Oam_LinkIfRecvAsyncMsg(pLinkState))
    {
        ProcessAsycMsg(pLinkState, ptRcvMsg);
        return;
    }
    
	
    switch(ptRcvMsg->CmdID)
    {
    case CMD_LPTL_SET_TEMINAL_DISPLAY:
        /*如果是terminaldisplay命令，需要打开或关闭显示开关*/
        if (ptRcvMsg->bIfNo)
        {
            Oam_LinkSetTelSvrInputModeAsSyn(pLinkState, TRUE);
        }
        else
        {
            Oam_LinkSetTelSvrInputModeAsSyn(pLinkState, FALSE);
        }
        break;
    default :
        break;
    }

    /*?? brs的返回码大于10时候要转换为错误码map类型*/
    if (ptRcvMsg->ReturnCode >= 10 &&
        ptRcvMsg->ReturnCode != SUCC_WAIT_AND_HAVEPARA) 
    {
        ConstructBrsMapPara(ptRcvMsg);
    }
	
    if (ptRcvMsg->ReturnCode == SUCC_CMD_NOT_FINISHED)
    {
        pLinkState->bRestartReqSended = FALSE;
    }
    Oam_CmdReturnMsgHandler(pLinkState,ptRcvMsg);

    return;
}

/**************************************************************************
* 函数名称：VOID ConstructBrsMapPara
* 功能描述：构建BRS应答消息中MAP类型参数
* 访问的表：无
* 修改的表：无
* 输入参数：MSG_COMM_OAM *ptMsg
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/04/02    V1.0    齐龙兆      创建
**************************************************************************/
void ConstructBrsMapPara(MSG_COMM_OAM *ptMsgBuf)
{
    WORD16 wMapValue = 0;
    OPR_DATA *pRtnOprData = NULL;
    if (NULL == ptMsgBuf)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__); 
        return;
    }

    pRtnOprData = (OPR_DATA *)ptMsgBuf->Data;
    wMapValue = ptMsgBuf->ReturnCode;
    ptMsgBuf->ReturnCode = ERR_AND_HAVEPARA;
    ptMsgBuf->Number = 0;
    SetMapOprData(ptMsgBuf, pRtnOprData, MAP_TYPE_PROTOCOL_ERROR, wMapValue);
    ptMsgBuf->DataLen = GetMsgStructSize(ptMsgBuf, pRtnOprData);
    return;
}

/**************************************************************************
* 函数名称：VOID CmdReturn_Msg_Handler
* 功能描述：处理来自命令执行返回消息
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_LINK_STATE *pLinkState,MSG_COMM_OAM *ptMsg
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
void Oam_CmdReturnMsgHandler(TYPE_LINK_STATE *pLinkState,MSG_COMM_OAM *ptMsg)
{
    MSG_COMM_OAM *ptRcvMsg = ptMsg;
    WORD16 wMsgLen = 0;
    INT32 iErrCode = MSG_SUCCESS;

    if (!ptRcvMsg)
    {
        XOS_SysLog(OAM_CFG_LOG_NOTICE,
                   "Oam_CmdReturnMsgHandler: Receive MSG_COMM_OAM is NULL!");
        return;			
    }

    wMsgLen = ptRcvMsg->DataLen;

    /* 用户登录阶段等非执行阶段，不需要处理 */
    if (Oam_LinkGetCurRunState(pLinkState) != pec_MsgHandle_EXECUTE)
    {
        XOS_SysLog(OAM_CFG_LOG_NOTICE,
                   "Oam_CmdReturnMsgHandler: Receive MSG_COMM_OAM msg, but current linkstate is not pec_MsgHandle_EXECUTE, discarded!");
        return;
    }

    /* 2.检查消息头 */
    iErrCode = CheckExeMsgHead(pLinkState, ptRcvMsg);
    if (iErrCode != MSG_SUCCESS)
    {
        /*如果接收到的消息中的SeqNo小于当前执行的SeqNo，则丢弃此消息(CRDCM00287137)*/
        if (iErrCode == MSGERR_SEQNO && ptRcvMsg->SeqNo < pLinkState->dwExeSeqNo)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR,
                   "Oam_CmdReturnMsgHandler: SeqNo of RevMsg is less than current ExeSeqNo!");
        }
        else if (MSGERR_DISCARDMSG == iErrCode)
        {
            return;
        }
        else
            SendExeMsgErrToTelnet(pLinkState,iErrCode);

        return;
    }

    /*********************** 命令处理阶段 *******************************/
    /*根据返回值进行 定时器处理 */
    switch(ptRcvMsg->ReturnCode)
    {
    case SUCC_CMD_DISPLAY_NO_KILLTIMER:
        /* 重置Execute端定时器处理 */
        Oam_KillExeTimer(pLinkState);
        if (Oam_LinkGetCurRunState(pLinkState) == pec_MsgHandle_EXECUTE)
            Oam_SetExeTimer(pLinkState);
        break;

    case SUCC_WAIT_AND_HAVEPARA:
        /* 每次ping回应需要重置Execute端定时器 
        if (CMD_IS_PING_OR_TRACE(ptMsg->CmdID))
        {
            Oam_KillExeTimer(pLinkState);
            if (Oam_LinkGetCurRunState(pLinkState) == pec_MsgHandle_EXECUTE)
                Oam_SetExeTimer(pLinkState);
        }*/        
        /*重置Execute端定时器*/
        Oam_KillExeTimer(pLinkState);
        Oam_SetExeTimer(pLinkState);
        break;
    /*大包模式下，收到一包就杀掉定时器，在下一次发起请求的时候再设定时器*/
    case SUCC_CMD_NOT_FINISHED:
        Oam_KillExeTimer(pLinkState);
        break;
    case CLI_RET_KEEP_ALIVE_NOPARA:    
        /*重置Execute端定时器后，直接返回*/
        Oam_KillExeTimer(pLinkState);
        Oam_SetExeTimer(pLinkState);
        /*611000748643 添加保活消息*/
        Oam_Cfg_SendKeepAliveMsgToTelnet(pLinkState);
        return;
    default:
        /* 包结束杀掉Execute端定时器 */
        if (OAM_MSG_IS_LAST_PACKET(ptRcvMsg) &&  (ptRcvMsg->SeqNo != 0))
        {
            Oam_KillExeTimer(pLinkState);
        }
        break;
    }

    /* 保存消息部分数据 */
    pLinkState->wAppMsgReturnCode = ptRcvMsg->ReturnCode;
    
    if (!Oam_LinkIfRecvAsyncMsg(pLinkState))
    {
        memcpy(&pLinkState->LastData, &ptRcvMsg->LastData, sizeof(ptRcvMsg->LastData));
        memcpy(&pLinkState->BUF, &ptRcvMsg->BUF, sizeof(ptRcvMsg->BUF));
    }

    /* 根据返回码做不同的处理 */
    switch(ptRcvMsg->ReturnCode)
    {
    case SUCC_TERMINATE_INTERPT_TIMER:
        break;

    case SUCC_AND_NOPARA:
        pLinkState->bOutputResult    = FALSE;
        *(pLinkState->sOutputResult) = MARK_STRINGEND;

        OAM_ChangeMode(pLinkState, NEXTMODE_NORMAL);

        /*如果是离线转在线或是恢复，不需要telnet显示*/
        if (Oam_CfgCurLinkIsOffline2Online(pLinkState))
        { 
            XOS_SysLog(OAM_CFG_LOG_NOTICE,
                   "OFFLINE TO ONLINE [brs return] cmdid = 0x%x, returncode = %u\n", (unsigned)ptRcvMsg->CmdID, (unsigned)ptRcvMsg->ReturnCode);
            Oam_CfgGetNextCmdFromBrs();
        }
        else if(S_Restore != XOS_GetCurState(PROCTYPE_OAM_CLIMANAGER))
        {
            SendPacketToTelnet(pLinkState, FALSE);
        }

        /* 改变当前状态 */
        if (Oam_LinkIfRecvLastPktOfApp(pLinkState))
        {
            Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_TELNET);
        }
        break;

    case SUCC_CMD_DISPLAY_AND_KILLTIMER:
    case SUCC_CMD_DISPLAY_NO_KILLTIMER:
    case SUCC_CMD_NOT_FINISHED:
    case SUCC_AND_HAVEPARA:
    case ERR_AND_HAVEPARA:
    case SUCC_WAIT_AND_HAVEPARA:
        /*如果是离线转在线或是恢复，不需要telnet显示*/
        if (Oam_CfgCurLinkIsOffline2Online(pLinkState))
        { 
            XOS_SysLog(OAM_CFG_LOG_NOTICE,
                   "OFFLINE TO ONLINE [brs return] cmdid = 0x%x, returncode = %u\n", (unsigned)ptRcvMsg->CmdID, (unsigned)ptRcvMsg->ReturnCode);
            Oam_CfgGetNextCmdFromBrs();
        }
        else if(S_Restore != XOS_GetCurState(PROCTYPE_OAM_CLIMANAGER))
        {
            /* 处理返回数据 */
            ParseReturnData(pLinkState, ptRcvMsg);
        }

        /* 改变当前状态 */
        if (Oam_LinkIfRecvLastPktOfApp(pLinkState))
        {
            /* 如果当前是存盘状态，当BRS返回最后一包之后，存盘命令还没有完全结束(后面还有同步txt到备板等操作)
               此时，不能将链路状态置为Telnet，应该保留Execute状态*/
            if (S_CfgSave != XOS_GetCurState(PROCTYPE_OAM_CLIMANAGER))
                Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_TELNET);
        }

        break;

    default:
        if(S_Restore != XOS_GetCurState(PROCTYPE_OAM_CLIMANAGER))
        {
            SendStringToTelnet(pLinkState,Hdr_szExeNoReturn);
            Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_TELNET);
        }
        break;
    }
}

/**************************************************************************
* 函数名称：INT32 CheckExeMsgHead
* 功能描述：检查Execute消息头
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
static INT32 CheckExeMsgHead(TYPE_LINK_STATE *pLinkState,MSG_COMM_OAM * ptMsg)
{
    switch(ptMsg->ReturnCode)
    {
    case SUCC_AND_NOPARA :
    case SUCC_AND_HAVEPARA :
    case SUCC_CMD_DISPLAY_AND_KILLTIMER:
    case SUCC_CMD_DISPLAY_NO_KILLTIMER:
    case SUCC_CMD_NOT_FINISHED:
    case SUCC_TERMINATE_INTERPT_TIMER:
    case ERR_AND_HAVEPARA:
    case CLI_RET_KEEP_ALIVE_NOPARA:
    case SUCC_WAIT_AND_HAVEPARA:
        /* Link Channel */
        if (ptMsg->LinkChannel != Oam_LinkGetLinkId(pLinkState))
            return MSGERR_LINKCHANNEL;

        /* SeqNo */
        if ( !Oam_LinkIfRecvAsyncMsg(pLinkState)&&
           ptMsg->SeqNo != pLinkState->dwExeSeqNo)
        {
            /*同一命令ID*/
            if (ptMsg->CmdID == Oam_LinkGetCurCmdId(pLinkState))
            {
                return MSGERR_SEQNO;
            }
            /*不同命令-消息丢弃处理*/
            else
            {
                XOS_SysLog(OAM_CFG_LOG_DEBUG,
                                  "....................FUNC:%s LINE:%d Cur Cmd:0x%0x,Msg Cmd:0x%0x,Cur SeqNo:%d,Msg No:%d ,discard! ....................\n",
                                   __func__,
                                   __LINE__,
                                   Oam_LinkGetCurCmdId(pLinkState),
                                   ptMsg->CmdID,
                                   pLinkState->dwExeSeqNo,
                                   ptMsg->SeqNo); 
                 return MSGERR_DISCARDMSG;
            }
        }

        /* Output Mode */
        if (ptMsg->OutputMode != OUTPUT_MODE_NORMAL &&
                ptMsg->OutputMode != OUTPUT_MODE_ROLL &&
                ptMsg->OutputMode != OUTPUT_MODE_REQUEST)
            return MSGERR_OUTPUTMODE;

        /* 1.非法命令ID */
        if (ptMsg->CmdID == 0xFFFFFFFF)
            return MSGERR_CMDID;
        /* 2.普通命令的回应 */
        else if (ptMsg->CmdID == Oam_LinkGetCurCmdId(pLinkState))
        {
            /* IsNoCmd */
            if (ptMsg->bIfNo != Oam_LinkIfCurCmdIsNo(pLinkState))
                return MSGERR_IFNO;
        }

        return MSG_SUCCESS;
    case ERR_MSG_FROM_INTPR:
        Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_TELNET);
        return MSGERR_FROMINTPR;
    case ERR_NO_NEED_THE_CMD:
        if (Oam_LinkGetCurRunState(pLinkState) > pec_MsgHandle_INIT)
            Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_TELNET);
        return MSGERR_NONEEDTHECMD;
    default:
        if (Oam_LinkGetCurRunState(pLinkState) > pec_MsgHandle_INIT)
            Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_TELNET);
        return MSGERR_RETURNCODE;
    }

}

/**************************************************************************
* 函数名称：VOID ParseReturnData
* 功能描述：根据回显脚本解析返回数据
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
static void ParseReturnData(TYPE_LINK_STATE *pLinkState,MSG_COMM_OAM * ptMsg)
{
    CHAR *pFormatStr = NULL;
    pLinkState->bOutputMode = ptMsg->OutputMode;

    /* 获取回显格式串 */
    pFormatStr = GetFormatByCmdAttrPos(pLinkState->wCmdAttrPos, pLinkState->tMatchResult.cfgDat);
    if (!pFormatStr)
    {
        SendErrStringToClient(pLinkState,Hdr_szExeNoDisplay);
        return;
    }

    /* 根据回显格式串生成回显字符串 */
    if (DISPLAY_SUCCESS == OamCfgintpSendToBuffer(pLinkState, ptMsg, pFormatStr,&pLinkState->tDispGlobalPara))
    {
        pLinkState->bOutputResult = TRUE;
    }
    else
    {
        /*添加下面这行是因为有的命令会因为回显改变outputmode = OUTPUT_MODE_REQUEST，影响后续命令执行*/
        pLinkState->bOutputMode = OUTPUT_MODE_NORMAL;
	 SendErrStringToClient(pLinkState,Hdr_szDisplayScriptErr);
        return;
    }

    {
        T_OAM_Cfg_Dat * cfgDat = pLinkState->tMatchResult.cfgDat;
        if (cfgDat->tCmdAttr[pLinkState->wCmdAttrPos].dExeID == ptMsg->CmdID)
            SendPacketToTelnet(pLinkState, FALSE);
    }

    return;
}
/**************************************************************************
* 函数名称：VOID PrintDataType
* 功能描述：打印数据类型
* 访问的表：无
* 修改的表：无
* 输入参数：
  BYTE bType
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
void PrintDataType(BYTE bType)
{
    CHAR sDataType[MAX_STRING_LEN];
    switch(bType)
    {
    case DATA_TYPE_BYTE:
        strncpy(sDataType,"DATA_TYPE_BYTE", sizeof(sDataType));
        break;
    case DATA_TYPE_WORD:
        strncpy(sDataType,"DATA_TYPE_WORD", sizeof(sDataType));
        break;
    case DATA_TYPE_DWORD:
        strncpy(sDataType,"DATA_TYPE_DWORD", sizeof(sDataType));
        break;
    case DATA_TYPE_INT:
        strncpy(sDataType,"DATA_TYPE_INT", sizeof(sDataType));
        break;
    case DATA_TYPE_CHAR:
        strncpy(sDataType,"DATA_TYPE_CHAR", sizeof(sDataType));
        break;
    case DATA_TYPE_STRING:
        strncpy(sDataType,"DATA_TYPE_STRING", sizeof(sDataType));
        break;
    case DATA_TYPE_TEXT:
        strncpy(sDataType,"DATA_TYPE_TEXT", sizeof(sDataType));
        break;
    case DATA_TYPE_TABLE:
        strncpy(sDataType,"DATA_TYPE_TABLE", sizeof(sDataType));
        break;
    case DATA_TYPE_IPADDR:
        strncpy(sDataType,"DATA_TYPE_IPADDR", sizeof(sDataType));
        break;
    case DATA_TYPE_DATE:
        strncpy(sDataType,"DATA_TYPE_DATE", sizeof(sDataType));
        break;
    case DATA_TYPE_TIME:
        strncpy(sDataType,"DATA_TYPE_TIME", sizeof(sDataType));
        break;
    case DATA_TYPE_MAP:
        strncpy(sDataType,"DATA_TYPE_MAP", sizeof(sDataType));
        break;
    case DATA_TYPE_LIST:
        strncpy(sDataType,"DATA_TYPE_LIST", sizeof(sDataType));
        break;
    case DATA_TYPE_CONST:
        strncpy(sDataType,"DATA_TYPE_CONST", sizeof(sDataType));
        break;
    case DATA_TYPE_IFPORT:
        strncpy(sDataType,"DATA_TYPE_IFPORT", sizeof(sDataType));
        break;
    case DATA_TYPE_HEX:
        strncpy(sDataType,"DATA_TYPE_HEX", sizeof(sDataType));
        break;
    case DATA_TYPE_MASK:
        strncpy(sDataType,"DATA_TYPE_MASK", sizeof(sDataType));
        break;
    case DATA_TYPE_IMASK:
        strncpy(sDataType,"DATA_TYPE_IMASK", sizeof(sDataType));
        break;
    case DATA_TYPE_MACADDR:
        strncpy(sDataType,"DATA_TYPE_MACADDR", sizeof(sDataType));
        break;
    case DATA_TYPE_FLOAT:
        strncpy(sDataType,"DATA_TYPE_FLOAT", sizeof(sDataType));
        break;
    case DATA_TYPE_BOARDNAME:
        strncpy(sDataType,"DATA_TYPE_BOARDNAME", sizeof(sDataType));
        break;
    case DATA_TYPE_IPV6PREFIX:
        strncpy(sDataType,"DATA_TYPE_IPV6PREFIX", sizeof(sDataType));
        break;
    case DATA_TYPE_IPV6ADDR:
        strncpy(sDataType,"DATA_TYPE_IPV6ADDR", sizeof(sDataType));
        break;
    case DATA_TYPE_VPN_ASN:
        strncpy(sDataType,"DATA_TYPE_VPN_ASN", sizeof(sDataType));
        break;
    case DATA_TYPE_VPN_IP:
        strncpy(sDataType,"DATA_TYPE_VPN_IP", sizeof(sDataType));
        break;
    case DATA_TYPE_PARA:
        strncpy(sDataType,"DATA_TYPE_PARA", sizeof(sDataType));
        break;
    case DATA_TYPE_PHYADDRESS:
        strncpy(sDataType,"DATA_TYPE_PHYADDRESS", sizeof(sDataType));
        break;
    case DATA_TYPE_LOGICADDRESS:
        strncpy(sDataType,"DATA_TYPE_LOGICADDRESS", sizeof(sDataType));
        break;
    case DATA_TYPE_WORD64:
        strncpy(sDataType,"DATA_TYPE_WORD64", sizeof(sDataType));
        break;
    case DATA_TYPE_SWORD:
        strncpy(sDataType,"DATA_TYPE_SWORD", sizeof(sDataType));
        break;
    case DATA_TYPE_LOGIC_ADDR_M:
        strncpy(sDataType,"DATA_TYPE_LOGIC_ADDR_M", sizeof(sDataType));
        break;
    case DATA_TYPE_LOGIC_ADDR_U:
        strncpy(sDataType,"DATA_TYPE_LOGIC_ADDR_U", sizeof(sDataType));
        break;
    case DATA_TYPE_IPVXADDR:
        strncpy(sDataType,"DATA_TYPE_IPVXADDR", sizeof(sDataType));
        break;
    case DATA_TYPE_T_TIME:
        strncpy(sDataType,"DATA_TYPE_T_TIME", sizeof(sDataType));
        break;
    case DATA_TYPE_SYSCLOCK:
        strncpy(sDataType,"DATA_TYPE_SYSCLOCK", sizeof(sDataType));
        break;
    case DATA_TYPE_VMMDATE:
        strncpy(sDataType,"DATA_TYPE_VMMDATE", sizeof(sDataType));
        break;
    case DATA_TYPE_VMMDATETIME:
        strncpy(sDataType,"DATA_TYPE_VMMDATETIME", sizeof(sDataType));
        break;
    default:
        XOS_snprintf(sDataType,sizeof(sDataType), "UNKNOWN TYPE(%u)", bType);
        break;
    }

    printf("<Type>%s ",sDataType);

    return;
}

/**************************************************************************
* 函数名称：BYTE *PrintTable
* 功能描述：打印表
* 访问的表：无
* 修改的表：无
* 输入参数：
  BYTE *pData, BYTE bRecord
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
BYTE *PrintTable(BYTE *pData, BYTE bRecord, BYTE bTableType)
{
    TABLE_DATA *pTableData = (TABLE_DATA *)pData;
    BYTE *pCurPos = pTableData->Data;
    WORD16 n,m,k;
    BYTE bTemp;
    BOOL bContinue;
    for (n = 0; n < bRecord; n++)
    {
        printf("\n");
        for (m = 0; m < pTableData->ItemNumber; m++)
        {
            printf("\n");
            PrintDataType(pTableData->Item[m].Type);
            printf("<Len>%u ",pTableData->Item[m].Len);
            printf("<Data>\n");
            bContinue = TRUE;
            for (k=0; k<pTableData->Item[m].Len && bContinue; k++)
            {
                bTemp = *(pCurPos+k);
                switch (pTableData->Item[m].Type)
                {
                case DATA_TYPE_CHAR:
                case DATA_TYPE_STRING:
                case DATA_TYPE_TEXT:
                    printf("%c",bTemp);
                    if (k<pTableData->Item[m].Len)
                    {
                        if (*(pCurPos+k+1) == '\0')
                            bContinue = FALSE;
                    }
                    break;
                default:
                    if ((k+1)%16 == 0)
                        printf("%02x\n",bTemp);
                    else
                        printf("%02x ",bTemp);
                    break;
                }
            }
            
            if (bTableType == DATA_TYPE_LIST && pTableData->Item[m].Type == DATA_TYPE_STRING)
            {
                pCurPos += strlen((CHAR *)pCurPos) + 1;
            }
            else
            {
            pCurPos += pTableData->Item[m].Len;
        }
    }
    }
    
    if (bRecord == 0)
    {
        pCurPos++;	
    }
    return pCurPos;
}

/**************************************************************************
* 函数名称：VOID PrintMSGCOMMOAMStruct
* 功能描述：打印MSG_COMM_OAM结构
* 访问的表：无
* 修改的表：无
* 输入参数：
  MSG_COMM_OAM *ptMsg, CHAR* pDesc
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
void PrintMSGCOMMOAMStruct(MSG_COMM_OAM *ptMsg, CHAR *pDesc)
{
    OPR_DATA *pOprData = NULL;
    WORD16 i = 0, j = 0;
    BOOL bContinue = 0;

    if (g_iSyslogLevel > OAM_CFG_LOG_DEBUG)
    {
        return;
    }

    if (!ptMsg && !pDesc)
    {
        printf("\nInput parameter invalid(ptMsg or pDesc is NULL)\n");
        return;
    }
    pOprData = ptMsg->Data;

    printf("\n<%s>\n",pDesc);

    /*print struct info*/
    printf("SeqNo:          %u\n",     (unsigned int)ptMsg->SeqNo);
    printf("DataLen:        %u\n",     (unsigned int)ptMsg->DataLen);
    printf("CmdID:          0x%08x\n", (unsigned int)ptMsg->CmdID);
    printf("Number:         %u\n",     (unsigned int)ptMsg->Number);
    printf("ReturnCode:     %u\n",     (unsigned int)ptMsg->ReturnCode);
    printf("LastData:       \n");
    for (i=0; i<MAX_STORE_LEN; i++)
    {
        if (((i+1)%16) == 0)
            printf("%02x\n", ptMsg->LastData[i]);
        else
            printf("%02x ", ptMsg->LastData[i]);
    }
    printf("BUF:       \n");
    for (i=0; i<MAX_STORE_LEN; i++)
    {
        if (((i+1)%16) == 0)
            printf("%02x\n", ptMsg->BUF[i]);
        else
            printf("%02x ", ptMsg->BUF[i]);
    }
    printf("bIfNo:          %u\n", (unsigned int)ptMsg->bIfNo);
    printf("CmdRestartFlag: %u\n", (unsigned int)ptMsg->CmdRestartFlag);
    printf("OutputMode:     %u\n", (unsigned int)ptMsg->OutputMode);
    printf("LinkChannel:    %u\n", (unsigned int)ptMsg->LinkChannel);
    printf("ucOamTag:          %u\n", (unsigned int)ptMsg->ucOamTag);
    printf("Data:           ");
    /*print data*/
    for(i=0; i<ptMsg->Number; i++)
    {
        printf("\n");

        /*print parano*/
        printf("<ParaNo>%u ",(unsigned int)pOprData->ParaNo);

        /*print paraname
        printf("<ParaName>%s ",pOprData->sName);*/

        /*print type*/
        PrintDataType(pOprData->Type);

        /*print len*/
        printf("<Len>%u ",(unsigned int)pOprData->Len);

        /*print value*/
        printf("<Data>");

        /*table类型*/
        if ((DATA_TYPE_TABLE == pOprData->Type) || (DATA_TYPE_LIST == pOprData->Type))
        {
            pOprData = (OPR_DATA *)PrintTable(pOprData->Data, pOprData->Len, pOprData->Type);
        }
        /*其它类型*/
        else
        {
            int iLen;

            if (pOprData->Type == DATA_TYPE_STRING)
            {
                iLen = strlen((CHAR *)pOprData->Data) + 1;
            }
            else
            {
                iLen = pOprData->Len;
            }
			
            printf("\n");
            bContinue = TRUE;
     
            for (j=0; j<iLen && bContinue; j++)
            {
                switch (pOprData->Type)
                {
                case DATA_TYPE_CHAR:
                case DATA_TYPE_STRING:
                case DATA_TYPE_TEXT:
                    printf("%c",pOprData->Data[j]);
                    if (j < iLen)
                    {
                        if (pOprData->Data[j+1] == '\0')
                            bContinue = FALSE;
                    }
                    break;
                default:
                    if ((j+1)%16 == 0)
                        printf("%02x\n",pOprData->Data[j]);
                    else
                        printf("%02x ",pOprData->Data[j]);
                    break;
                }
            }
            pOprData = GetNextOprData(pOprData);
            if ((WORD32)((BYTE *)pOprData - (BYTE *)ptMsg) > ptMsg->DataLen)
            {
                printf("datalen error\n");
                return;
            }
        }
    }
    printf("\n</%s>\n",pDesc);
}

/*****************************************************************************/
/**
@brief 打开CLI 和应用之间收发消息的打印开关，用于CLI命令参数错误等定位
@li @b 入参列表：
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
\n1、只显示CLI和应用直接的EV_OAM_CFG_CFG消息，也就是MSG_COMM_OAM结构内容
2、CLI发给应用的还是应用发给CLI的根据消息打印时候的开头和结尾<>括号中描述区分
3、该函数只是打开开关，具体打印在应用执行命令时候才有
@li @b 输出示例：
\n
<MSG_CLIMANAGER_TO_APP>
SeqNo:          29
DataLen:        287
CmdID:          0x04010022
Number:         1
ReturnCode:     0
LastData:
04 00 00 00 d1 07 f3 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
BUF:
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
bIfNo:          0
CmdRestartFlag: 1
OutputMode:     2
LinkChannel:    5
ucOamTag:          0
Data:
<ParaNo>0 <Type>DATA_TYPE_DWORD <Len>4 <Data>
01 00 00 00
</MSG_CLIMANAGER_TO_APP>
@li @b 修改记录：
-#  ： 
*/
/*****************************************************************************/
void OAM_DbgOpenCliMsgPrint(void)
{
    bDebugPrint = TRUE;
}

/*****************************************************************************/
/**
@brief 关闭OAM_DbgShowCliMsg函数打开的CLI 和应用之间收发消息的打印开关
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
\n无
@li @b 输出示例：
\n无
@li @b 修改记录：
-#  ： 
*/
/*****************************************************************************/
void OAM_DbgCloseCliMsgPrint(void)
{
    bDebugPrint = FALSE;
}

/**************************************************************************
* 函数名称：VOID ShowOAMDebugMode
* 功能描述：显示OAMDebugMode开关
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
void ShowOAMDebugMode(void)
{
    if (bDebugPrint)
        printf("bDebugPrint = TRUE\n");
    else
        printf("bDebugPrint = FALSE\n");
}

/**************************************************************************
* 函数名称：VOID ExecAppProc
* 功能描述：应用业务处理函数
* 访问的表：无
* 修改的表：无
* 输入参数：
    MSG_COMM_OAM *ptRcvMsg, 
    JID tJid
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2008/08/08    V1.0    傅龙锁      创建
**************************************************************************/
void ExecAppProc(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *ptRcvMsg, JID tJid)
{
/*如果是离线转在线的命令，需要特殊处理一下
       使用brs请求时候的jid*/
    if (Oam_CfgCurCmdIsOffline2Online(ptRcvMsg))
    {
        /*获取brs pid*/
        Oam_CfgGetBrsPidForOffline2Online(&tJid);
    }
        
    {
#if 0
/*611000761985 在参数转换时候用位运算保证输出字节序，不用判断本地字节序来转换*/
        /*对于地址、掩码等类型的参数，由于解析程序内部是写死的固定的字节序，所以在发送之前需要转化为
        与当前模块一致的字节序*/
        if (Oam_CfgIsNeedInvertOrder())
        {
            CfgMsgTransAddrByteOder(ptRcvMsg);
        }
#endif
        /* 命令发送，设置进度信息上报等待定时器 */
        Oam_KillCmdPlanTimer(pLinkState);
        Oam_SetCmdPlanTimer(pLinkState);

        XOS_SendAsynMsg(EV_OAM_CFG_CFG, (BYTE *)ptRcvMsg, ptRcvMsg->DataLen, XOS_MSG_VER0, XOS_MSG_MEDIUM, &tJid);
    }

    return;
}

/**************************************************************************
* 函数名称：VOID ExecCLIProc
* 功能描述：数据库执行模式处理函数
* 访问的表：无
* 修改的表：无
* 输入参数：
    MSG_COMM_OAM *ptRcvMsg, 
    MSG_COMM_OAM *ptRtnMsg
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
void ExecCLIProc(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *ptRcvMsg, MSG_COMM_OAM *ptRtnMsg)
{
    BYTE ucRet = CheckGetOrSet(ptRcvMsg->CmdID);

    if (ucRet == IS_GET)                     /* Get操作 */
        ExecCLIGet(pLinkState, ptRcvMsg);
    else if(ucRet == IS_SET)                 /* Set操作 */
        ExecCLISet(pLinkState, ptRcvMsg, ptRtnMsg);

    return;
}

/**************************************************************************
* 函数名称：VOID GetTerminalCfg
* 功能描述：获取当前用户终端信息
* 访问的表：无
* 修改的表：无
* 输入参数：
    MSG_COMM_OAM *ptRcvMsg, 
    MSG_COMM_OAM *ptRtnMsg
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
void GetTerminalCfg(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *ptRtnMsg)
{
    GetTerminalCfgData(&ptRtnMsg);
    Oam_CmdReturnMsgHandler(pLinkState, ptRtnMsg);

    return;
}

/**************************************************************************
* 函数名称：VOID ExecCLIGet
* 功能描述：数据库执行模式GET操作
* 访问的表：无
* 修改的表：无
* 输入参数：
    MSG_COMM_OAM *ptRcvMsg, 
    MSG_COMM_OAM *ptRtnMsg
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
void ExecCLIGet(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *ptRcvMsg)
{
    switch(ptRcvMsg->CmdID)
    {
    case CMD_TELNET_WHO:
        GetOnlineUsers(pLinkState,ptRcvMsg);
        break;
    case CMD_TELNET_SHOW_TERMINAL:
        GetTerminalCfg(pLinkState, ptRcvMsg);
        break;
    case CMD_LDB_GET_USERNAME:
        GetUserName(pLinkState, ptRcvMsg);
        break;
    default:
        break;
    }
    return;
}

/**************************************************************************
* 函数名称：VOID ExecCLISet
* 功能描述：数据库执行模式SET操作
* 访问的表：无
* 修改的表：无
* 输入参数：
    MSG_COMM_OAM *ptRcvMsg, 
    MSG_COMM_OAM *ptRtnMsg
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
void ExecCLISet(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *ptRcvMsg, MSG_COMM_OAM *ptRtnMsg)
{
    switch(ptRcvMsg->CmdID)
    {
    case CMD_LDB_SET_USERNAME:
        SetUserName(pLinkState, ptRcvMsg,ptRtnMsg);
        break;
#if 0        
    case CMD_LDB_SET_ENABLE_PASSWORD:
        SetEnablePassword(pLinkState,ptRcvMsg,ptRtnMsg);
        break;
#endif        
    case CMD_LDB_SET_CONSOLE_IDLE_TIMEOUT:
        SetConsoleIdleTimeout(pLinkState,ptRcvMsg,ptRtnMsg);
        break;
    case CMD_LDB_SET_CONSOLE_ABS_TIMEOUT:
        SetConsoleAbsTimeout(pLinkState,ptRcvMsg,ptRtnMsg);
        break;
    case CMD_LDB_SET_TELNET_IDLE_TIMEOUT:
        SetTelnetIdleTimeout(pLinkState,ptRcvMsg,ptRtnMsg);
        break;
    case CMD_LDB_SET_TELNET_ABS_TIMEOUT:
        SetTelnetAbsTimeout(pLinkState,ptRcvMsg,ptRtnMsg);
        break;
    default:
        break;
    }
    return;
}

/**************************************************************************
* 函数名称：VOID GetOnlineUsers
* 功能描述：获取在线用户列表
* 访问的表：无
* 修改的表：无
* 输入参数：
    MSG_COMM_OAM *ptRcvMsg, 
    MSG_COMM_OAM *ptRtnMsg
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
void GetOnlineUsers(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *ptRtnMsg)
{
    MSG_COMM_OAM tRtnMsgHead;
    OPR_DATA *pRtnOprData = NULL;
    TABLE_DATA *pTab = NULL;
    BYTE *pData = NULL;
    BYTE i;
    CHAR TempStr[MAX_STRING_LEN] = {0};
    WORD16 wRecordLen = 0;
    WORD32 dwCurMsgLen = 0;

    ptRtnMsg->ReturnCode = SUCC_AND_HAVEPARA;
    ptRtnMsg->Number = 1;

    pRtnOprData = (OPR_DATA *)ptRtnMsg->Data;
    pRtnOprData->ParaNo = 0;
    pRtnOprData->Type = DATA_TYPE_TABLE;
    pRtnOprData->Len = 0;

    pTab = (TABLE_DATA *)pRtnOprData->Data;
    pTab->ItemNumber = 5;

    pTab->Item[0].Type=DATA_TYPE_STRING;
    pTab->Item[0].Len=MAX_STRING_LEN;

    pTab->Item[1].Type=DATA_TYPE_STRING;
    pTab->Item[1].Len=MAX_STRING_LEN;

    pTab->Item[2].Type=DATA_TYPE_STRING;
    pTab->Item[2].Len=MAX_STRING_LEN;

    pTab->Item[3].Type=DATA_TYPE_STRING;
    pTab->Item[3].Len=MAX_STRING_LEN;

    pTab->Item[4].Type=DATA_TYPE_STRING;
    pTab->Item[4].Len=MAX_STRING_LEN;

    wRecordLen = MAX_STRING_LEN * pTab->ItemNumber;
    
    pData = (BYTE *)pTab->Data;

    memcpy(&tRtnMsgHead, ptRtnMsg, sizeof(tRtnMsgHead));
    for (i = MIN_CLI_VTY_NUMBER; i <= MAX_CLI_VTY_NUMBER; i++)
    {
        WORD16 wHour,wMinute,wSecond;
        BYTE k = i - MIN_CLI_VTY_NUMBER;
        
        if (Oam_LinkGetCurRunState(&gtLinkState[k]) == pec_MsgHandle_IDLE)
        {
            continue;
        }
        
        if (i == ptRtnMsg->LinkChannel)
            strncpy((CHAR *)pData," * ",(WORD32)(MAX_OAMMSG_LEN - (WORD32)((BYTE *)pData - (BYTE *)ptRtnMsg) ));
        else
            strncpy((CHAR *)pData,"   ",(WORD32)(MAX_OAMMSG_LEN - (WORD32)((BYTE *)pData - (BYTE *)ptRtnMsg) ));
        if (i == MIN_CLI_VTY_NUMBER)
            XOS_snprintf(TempStr, MAX_STRING_LEN, "%d  con %d",i,i-MIN_CLI_VTY_NUMBER);
        else
            XOS_snprintf(TempStr, MAX_STRING_LEN, "%d  vty %d",i,i-MIN_CLI_VTY_NUMBER);
        strncat((CHAR *)pData,TempStr, (WORD32)(MAX_OAMMSG_LEN - (WORD32)((BYTE *)pData - (BYTE *)ptRtnMsg)));
        pData += pTab->Item[0].Len;

        strncpy((CHAR *)pData,gtLinkState[k].sUserName, (WORD32)(MAX_OAMMSG_LEN - (WORD32)((BYTE *)pData - (BYTE *)ptRtnMsg) ));
        pData += pTab->Item[1].Len;

        strncpy((CHAR *)pData,"", (WORD32)(MAX_OAMMSG_LEN - (WORD32)((BYTE *)pData - (BYTE *)ptRtnMsg)));
        pData += pTab->Item[2].Len;

        wHour = gtLinkState[k].wIdleTimeout/60;
        wMinute = gtLinkState[k].wIdleTimeout%60;
        wSecond = 0;
        XOS_snprintf(TempStr, MAX_STRING_LEN, "%02u:%02u:%02u",wHour,wMinute,wSecond);
        strncpy((CHAR *)pData,TempStr, (WORD32)(MAX_OAMMSG_LEN - (WORD32)((BYTE *)pData - (BYTE *)ptRtnMsg) ));
        pData += pTab->Item[3].Len;

        strncpy((CHAR *)pData,(CHAR *)gtLinkState[k].tnIpAddr, (WORD32)(MAX_OAMMSG_LEN - (WORD32)((BYTE *)pData - (BYTE *)ptRtnMsg) ));
        pData += pTab->Item[4].Len;

        pRtnOprData->Len++;

        dwCurMsgLen = (DWORD)((BYTE *)pData - (BYTE *)ptRtnMsg);
        
        /*看是否需要分包发送*/
        if ((dwCurMsgLen + wRecordLen) >= MAX_OAMMSG_LEN)
        {
            ptRtnMsg->DataLen = dwCurMsgLen;
            pRtnOprData->ParaNo = 255;
            ptRtnMsg->ReturnCode = SUCC_WAIT_AND_HAVEPARA;
            
            Oam_CmdReturnMsgHandler(pLinkState, ptRtnMsg);
            
            memcpy(ptRtnMsg, &tRtnMsgHead, sizeof(tRtnMsgHead));
            pData = (BYTE *)pTab->Data;
        }
    }
    ptRtnMsg->DataLen = dwCurMsgLen;
    
    pRtnOprData->ParaNo = 0;
    ptRtnMsg->ReturnCode = SUCC_AND_HAVEPARA;

    Oam_CmdReturnMsgHandler(pLinkState, ptRtnMsg);

    return;
}

/**************************************************************************
* 函数名称：VOID GetUserName
* 功能描述：获取用户列表
* 访问的表：无
* 修改的表：无
* 输入参数：
    MSG_COMM_OAM *ptRcvMsg, 
    MSG_COMM_OAM *ptRtnMsg
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
void GetUserName(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *ptRtnMsg)
{

    GetUserNameData(&ptRtnMsg);
    Oam_CmdReturnMsgHandler(pLinkState, ptRtnMsg);

    return;
}

/**************************************************************************
* 函数名称：VOID SetUserName
* 功能描述：增加/删除/修改用户
* 访问的表：无
* 修改的表：无
* 输入参数：
    MSG_COMM_OAM *ptRcvMsg, 
    MSG_COMM_OAM *ptRtnMsg
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
void SetUserName(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *ptRcvMsg, MSG_COMM_OAM *ptRtnMsg)
{
    OPR_DATA *pRevOprData = (OPR_DATA *)ptRcvMsg->Data;
    OPR_DATA *pRtnOprData = (OPR_DATA *)ptRtnMsg->Data;
    WORD16 i = 0;
    BOOL bSucceed = FALSE;

    if(!ptRcvMsg->bIfNo)
    {
        USER_NODE *pNode = (USER_NODE *)XOS_GetUB(sizeof(USER_NODE));
        assert(pNode);
        if (pNode)
        {
            memset(pNode,0,sizeof(USER_NODE));
            for (i = 0; i < ptRcvMsg->Number; i++)
            {
                SWITCH(pRevOprData->ParaNo)
                {
                    CASE(1)
                    memcpy(pNode->tUserInfo.sUsername, pRevOprData->Data, pRevOprData->Len);
                    BREAK
                    CASE(1)
                    BREAK
                    CASE(1)
                    memcpy(pNode->tUserInfo.sPassword, pRevOprData->Data, pRevOprData->Len);
                    BREAK
                    CASE(1)
                    BREAK
                    CASE(1)
                    memcpy(&pNode->tUserInfo.ucRight, pRevOprData->Data, pRevOprData->Len);
                    BREAK
                    DEFAULT
                    BREAK
                }
                END
                pRevOprData = GetNextOprData(pRevOprData);
            }
        }
        bSucceed = AddUserToLink(pNode);
    }
    else
    {
        CHAR sUsername[MAX_USERNAME_LEN];
        for (i = 0; i < ptRcvMsg->Number; i++)
        {
            SWITCH(pRevOprData->ParaNo)
            {
                CASE(1)
                memcpy(sUsername, pRevOprData->Data, pRevOprData->Len);
                BREAK
                DEFAULT
                BREAK
            }
            END
            pRevOprData = GetNextOprData(pRevOprData);
        }
        bSucceed = DelUserFromLink(sUsername);
    }

    if (bSucceed)
    {
        ptRtnMsg->ReturnCode = SUCC_AND_NOPARA;
    }
    else
    {
        ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;
        SetMapOprData(ptRtnMsg, pRtnOprData, MAP_TYPE_EXE_INNER_ERROR, ERR_EXE_INNER_CFG_FAIL);
    }
    ptRtnMsg->DataLen = GetMsgStructSize(ptRtnMsg, pRtnOprData);
    Oam_CmdReturnMsgHandler(pLinkState, ptRtnMsg);

    return;
}

#if 0
/**************************************************************************
* 函数名称：VOID SetEnablePassword
* 功能描述：删除/修改特权密码
* 访问的表：无
* 修改的表：无
* 输入参数：
    MSG_COMM_OAM *ptRcvMsg, 
    MSG_COMM_OAM *ptRtnMsg
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
VOID SetEnablePassword(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *ptRcvMsg, MSG_COMM_OAM *ptRtnMsg)
{
    OPR_DATA *pRevOprData = (OPR_DATA *)ptRcvMsg->Data;
    OPR_DATA *pRtnOprData = (OPR_DATA *)ptRtnMsg->Data;
    WORD16 i = 0;

    if(!ptRcvMsg->bIfNo)
    {
        for (i = 0; i < ptRcvMsg->Number; i++)
        {
            SWITCH(pRevOprData->ParaNo)
            {
                CASE(1)
                memcpy(gtCommonInfo.sEnablePassword, pRevOprData->Data, pRevOprData->Len);
                BREAK
                DEFAULT
                BREAK
            }
            END
            pRevOprData = GetNextOprData(pRevOprData);
        }
    }
    else
    {
        memcpy(gtCommonInfo.sEnablePassword, DEFAULT_ENABLE_PASSWORD, MAX_PASSWORD_LEN);
    }

    ptRtnMsg->ReturnCode = SUCC_AND_NOPARA;
    ptRtnMsg->DataLen = GetMsgStructSize(ptRtnMsg, pRtnOprData);
    Oam_CmdReturnMsgHandler(pLinkState, ptRtnMsg);

    return;
}
#endif

/**************************************************************************
* 函数名称：VOID SetConsoleIdleTimeout
* 功能描述：删除/修改wConsoleIdleTimeout
* 访问的表：无
* 修改的表：无
* 输入参数：
    MSG_COMM_OAM *ptRcvMsg, 
    MSG_COMM_OAM *ptRtnMsg
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
void SetConsoleIdleTimeout(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *ptRcvMsg, MSG_COMM_OAM *ptRtnMsg)
{
    OPR_DATA *pRevOprData = (OPR_DATA *)ptRcvMsg->Data;
    OPR_DATA *pRtnOprData = (OPR_DATA *)ptRtnMsg->Data;
    WORD16 i = 0;

    if(!ptRcvMsg->bIfNo)
    {
        for (i = 0; i < ptRcvMsg->Number; i++)
        {
            SWITCH(pRevOprData->ParaNo)
            {
                CASE(1)
                memcpy(&gtCommonInfo.wConsoleIdleTimeout, pRevOprData->Data, sizeof(WORD16));
                BREAK
                DEFAULT
                BREAK
            }
            END
            pRevOprData = GetNextOprData(pRevOprData);
        }
    }
    else
    {
        gtCommonInfo.wConsoleIdleTimeout = DEFAULT_CONSOLE_IDLE_TIMEOUT;
    }

    ptRtnMsg->ReturnCode = SUCC_AND_NOPARA;
    ptRtnMsg->DataLen = GetMsgStructSize(ptRtnMsg, pRtnOprData);
    Oam_CmdReturnMsgHandler(pLinkState,ptRtnMsg);

    return;
}

/**************************************************************************
* 函数名称：VOID SetConsoleAbsTimeout
* 功能描述：删除/修改wConsoleAbsTimeout
* 访问的表：无
* 修改的表：无
* 输入参数：
    MSG_COMM_OAM *ptRcvMsg, 
    MSG_COMM_OAM *ptRtnMsg
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
void SetConsoleAbsTimeout(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *ptRcvMsg, MSG_COMM_OAM *ptRtnMsg)
{
    OPR_DATA *pRevOprData = (OPR_DATA *)ptRcvMsg->Data;
    OPR_DATA *pRtnOprData = (OPR_DATA *)ptRtnMsg->Data;
    WORD16 i = 0;

    if(!ptRcvMsg->bIfNo)
    {
        for (i = 0; i < ptRcvMsg->Number; i++)
        {
            SWITCH(pRevOprData->ParaNo)
            {
                CASE(1)
                memcpy(&gtCommonInfo.wConsoleAbsTimeout, pRevOprData->Data, sizeof(WORD16));
                BREAK
                DEFAULT
                BREAK
            }
            END
            pRevOprData = GetNextOprData(pRevOprData);
        }
    }
    else
    {
        gtCommonInfo.wConsoleAbsTimeout = DEFAULT_CONSOLE_ABS_TIMEOUT;
    }

    ptRtnMsg->ReturnCode = SUCC_AND_NOPARA;
    ptRtnMsg->DataLen = GetMsgStructSize(ptRtnMsg, pRtnOprData);
    Oam_CmdReturnMsgHandler(pLinkState,ptRtnMsg);

    return;
}

/**************************************************************************
* 函数名称：VOID SetTelnetIdleTimeout
* 功能描述：删除/修改wTelnetIdleTimeout
* 访问的表：无
* 修改的表：无
* 输入参数：
    MSG_COMM_OAM *ptRcvMsg, 
    MSG_COMM_OAM *ptRtnMsg
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
void SetTelnetIdleTimeout(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *ptRcvMsg, MSG_COMM_OAM *ptRtnMsg)
{
    OPR_DATA *pRevOprData = (OPR_DATA *)ptRcvMsg->Data;
    OPR_DATA *pRtnOprData = (OPR_DATA *)ptRtnMsg->Data;
    WORD16 i = 0;

    if(!ptRcvMsg->bIfNo)
    {
        for (i = 0; i < ptRcvMsg->Number; i++)
        {
            SWITCH(pRevOprData->ParaNo)
            {
                CASE(1)
                memcpy(&gtCommonInfo.wTelnetIdleTimeout, pRevOprData->Data, sizeof(WORD16));
                BREAK
                DEFAULT
                BREAK
            }
            END
            pRevOprData = GetNextOprData(pRevOprData);
        }
    }
    else
    {
        gtCommonInfo.wTelnetIdleTimeout = DEFAULT_TELNET_IDLE_TIMEOUT;
    }

    if (SaveOamCfgInfo() == WRITEERR_SUCCESS)
    {
        ptRtnMsg->ReturnCode = SUCC_AND_HAVEPARA;
        SetMapOprData(ptRtnMsg, pRtnOprData, MAP_TYPE_EXE_INNER_ERROR, ERR_EXE_INNER_CFG_SUCC);
    }
    else
    {
        ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;
        SetMapOprData(ptRtnMsg, pRtnOprData, MAP_TYPE_EXE_INNER_ERROR, ERR_EXE_INNER_CFG_FAIL);
    }
    ptRtnMsg->DataLen = GetMsgStructSize(ptRtnMsg, pRtnOprData);
    Oam_CmdReturnMsgHandler(pLinkState, ptRtnMsg);

    return;
}

/**************************************************************************
* 函数名称：VOID SetTelnetAbsTimeout
* 功能描述：删除/修改wTelnetAbsTimeout
* 访问的表：无
* 修改的表：无
* 输入参数：
    MSG_COMM_OAM *ptRcvMsg, 
    MSG_COMM_OAM *ptRtnMsg
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
void SetTelnetAbsTimeout(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *ptRcvMsg, MSG_COMM_OAM *ptRtnMsg)
{
    OPR_DATA *pRevOprData = (OPR_DATA *)ptRcvMsg->Data;
    OPR_DATA *pRtnOprData = (OPR_DATA *)ptRtnMsg->Data;
    WORD16 i = 0;

    if(!ptRcvMsg->bIfNo)
    {
        for (i = 0; i < ptRcvMsg->Number; i++)
        {
            SWITCH(pRevOprData->ParaNo)
            {
                CASE(1)
                memcpy(&gtCommonInfo.wTelnetAbsTimeout, pRevOprData->Data, sizeof(WORD16));
                BREAK
                DEFAULT
                BREAK
            }
            END
            pRevOprData = GetNextOprData(pRevOprData);
        }
    }
    else
    {
        gtCommonInfo.wTelnetAbsTimeout = DEFAULT_TELNET_ABS_TIMEOUT;
    }

    ptRtnMsg->ReturnCode = SUCC_AND_NOPARA;
    ptRtnMsg->DataLen = GetMsgStructSize(ptRtnMsg, pRtnOprData);
    Oam_CmdReturnMsgHandler(pLinkState,ptRtnMsg);

    return;
}

void SaveZDBProc(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *ptMsg)
{
    JID                  tAppJid;
    _db_t_back_save_req  tProDB;

    WORD16 wModule = 0;
    WORD16 wValue = 0;
    WORD16 wPara1 = 0;
    WORD16 wPara2 = 0;
    int i = 0;
    OPR_DATA *pRevOprData =  (OPR_DATA *)ptMsg->Data;

    for (i = 0; i < ptMsg->Number; i++)
    {
        SWITCH(pRevOprData->ParaNo)
        {
            CASE(1)
            memcpy(&wPara1, pRevOprData->Data, sizeof(WORD16));
            BREAK
            CASE(1)
            memcpy(&wPara2, pRevOprData->Data, sizeof(WORD16));
            BREAK
            DEFAULT
            BREAK
        }
        END
        pRevOprData = GetNextOprData(pRevOprData);
    }
    
    if (ptMsg->CmdID == CMD_RPU_SAVE)
        wValue = wPara1;
    else
    {
        wModule = wPara1;
        wValue = wPara2;
    }
    
    /*构造DBS的JID*/
    memset(&tAppJid, 0, sizeof(JID));
    if (ptMsg->CmdID == CMD_RPU_SAVE)
        tAppJid.wModule = 2;
    else
        tAppJid.wModule = wModule;
    tAppJid.dwJno = XOS_ConstructJNO(pNZDB_IO,1);
    tAppJid.wUnit   = 65535;
    tAppJid.ucSUnit = 255;
    tAppJid.ucSubSystem = 255;
    if (0 == wValue)
        tAppJid.ucRouteType = COMM_MASTER_SERVICE;
    else
        tAppJid.ucRouteType = COMM_SLAVE_SERVICE;
    
    memset(&tProDB, 0, sizeof(_db_t_back_save_req));
    
    if (ptMsg->CmdID == CMD_RPU_SAVE)
    {
        tProDB.isAllDbSave = FALSE;
        tProDB.wDbNum = 1;
        XOS_snprintf(tProDB.tDatabaseSave[0].dbName,
                             sizeof(tProDB.tDatabaseSave[0].dbName) -1,
                             "%s",
                             "IPCONF_DB");
    }
    else
    {
        tProDB.isAllDbSave = TRUE;
    }
    tProDB.isReport = TRUE;

    XOS_GetSelfJID(&g_ptOamIntVar->jidSelf);
    memcpy(&tProDB.BackMgtJid, &g_ptOamIntVar->jidSelf, sizeof(JID));

    /* 命令发送，设置进度信息上报等待定时器 */
    Oam_KillCmdPlanTimer(pLinkState);
    Oam_SetCmdPlanTimer(pLinkState);

    XOS_SendAsynMsg(EV_SAVETABLE, (BYTE*)&tProDB, (WORD16)sizeof(tProDB),
                      XOS_MSG_VER0, XOS_MSG_MEDIUM,(void*)&tAppJid);
    return;
}

void Oam_ExecSaveCmd(TYPE_LINK_STATE *pLinkState)
{
    MSG_COMM_OAM *ptSendMsg = NULL;
    WORD16 wPara1 = 0;
    WORD16 wPara2 = 0;
    WORD16 wModule = 0;
    WORD16 wValue = 0;
    OPR_DATA *pOprData = NULL;
    int i = 0;

    ptSendMsg = OamCfgConstructMsgData(pLinkState, NULL);
    if (!ptSendMsg)
    {            
        SendErrStringToClient(pLinkState, map_szExeCmdFailed);
        
        Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_TELNET);
        XOS_SetNextState(PROCTYPE_OAM_CLIMANAGER, S_Work);
        
        return;
    }

    pOprData =  (OPR_DATA *)ptSendMsg->Data;

    if (ptSendMsg->Number > 0)
    {
        for (i = 0; i < ptSendMsg->Number; i++)
        {
            SWITCH(pOprData->ParaNo)
            {
            CASE(1)
                memcpy(&wPara1, pOprData->Data, sizeof(WORD16));
                BREAK
            CASE(1)
                memcpy(&wPara2, pOprData->Data, sizeof(WORD16));
                BREAK
            DEFAULT
                BREAK
            }
            END
            pOprData = GetNextOprData(pOprData);
        }
    }
    
    /* RPU存盘只带主备参数 */
    if (ptSendMsg->CmdID == CMD_RPU_SAVE)
        wValue = wPara1;
    else
    {
        wModule = wPara1;
        wValue = wPara2;
    }

    if (S_Work == XOS_GetCurState(PROCTYPE_OAM_CLIMANAGER))
    {
            XOS_SysLog(OAM_CFG_LOG_DEBUG,
                   "[SAVE_PROC] Send msg: <OAM_MSG_SAVE_BEGIN> to self and change to S_CfgSave state!\n");
            XOS_SendAsynMsg(OAM_MSG_SAVE_BEGIN, (BYTE *)&pLinkState->ucLinkID, sizeof(pLinkState->ucLinkID), XOS_MSG_VER0, XOS_MSG_MEDIUM, &g_ptOamIntVar->jidSelf);
            XOS_SetNextState(PROCTYPE_OAM_CLIMANAGER, S_CfgSave);
        }
    else if (S_CfgSave == XOS_GetCurState(PROCTYPE_OAM_CLIMANAGER))
    {
        ExecSaveCmdProc(pLinkState, ptSendMsg);

        /*重置Execute端定时器*/
        Oam_KillExeTimer(pLinkState);
        Oam_SetExeTimer(pLinkState);

        Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_EXECUTE);
    }
    return;
}

/**************************************************************************
* 函数名称：VOID ExecSaveCmdProc
* 功能描述：处理存盘命令
* 访问的表：无
* 修改的表：无
* 输入参数：
    MSG_COMM_OAM *ptRcvMsg, 
    JID tJid
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2008/08/08    V1.0    傅龙锁      创建
**************************************************************************/
void ExecSaveCmdProc(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *ptRcvMsg)
{
    WORD16 wModule = 0;
    WORD16 wValue = 0;
    WORD16 wPara1 = 0;
    WORD16 wPara2 = 0;
    int i = 0;
    OPR_DATA *pRevOprData =  (OPR_DATA *)ptRcvMsg->Data;
    BOOL bIsOmp = FALSE;
    BOOL bIsLomp = FALSE;

    if (ptRcvMsg == NULL)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,"ptRcvMsg is NULL!!! \n");
        XOS_SetNextState(PROCTYPE_OAM_CLIMANAGER, S_Work);
        return;
    }

    for (i = 0; i < ptRcvMsg->Number; i++)
    {
        SWITCH(pRevOprData->ParaNo)
        {
            CASE(1)
            memcpy(&wPara1, pRevOprData->Data, sizeof(WORD16));
            BREAK
            CASE(1)
            memcpy(&wPara2, pRevOprData->Data, sizeof(WORD16));
            BREAK
            DEFAULT
            BREAK
        }
        END
        pRevOprData = GetNextOprData(pRevOprData);
    }

    /*判断是否是OMP*/
    if (ptRcvMsg->CmdID == CMD_RPU_SAVE)
    {
        wValue = wPara1;
        bIsOmp = TRUE;
    }
    else
    {
        wModule = wPara1;
        wValue = wPara2;
        
        if (g_ptOamIntVar->jidSelf.wModule == wModule)
        {
            bIsOmp = TRUE;
        }

        /*判断是否是LOMP*/
        if (!bIsOmp)
        {
            bIsLomp = IfModuleIsLomp(wModule);
        }
    }

    if ((!bIsOmp) && (!bIsLomp))
    {
        BOOLEAN bOR = FALSE;
        SendTxtSaveAckToClient(pLinkState,Hdr_szErrInvalidModule,bOR);
        Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_TELNET);
        XOS_SetNextState(PROCTYPE_OAM_CLIMANAGER, S_Work);
        return;
    }

    if (bDebugPrint)
        PrintMSGCOMMOAMStruct(ptRcvMsg,"MSG_CLIMANAGER_TO_APP_SAVE");

    /*先存ZDB，等到DBS返回结果之后再判断是否需要存TXT*/
    SaveZDBProc(pLinkState, ptRcvMsg);
    
    return;
}

/**************************************************************************
* 函数名称：IfModuleIsLomp
* 功能描述：根据模块号检查一个模块是否是LOMP
* 访问的表：无
* 修改的表：无
* 输入参数：
    MSG_COMM_OAM *ptRcvMsg, 
    JID tJid
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/05/27    V1.0    殷浩      创建
**************************************************************************/
BOOLEAN IfModuleIsLomp(WORD16 wModule)
{
    WORD32 wLoop = 0;
    BOOLEAN bRet = FALSE;

    if (g_ptOamIntVar)
    {        
        /*检查数据区中注册信息*/
        for(wLoop=0; wLoop< g_ptOamIntVar->tLompCliAppRegInfo.dwRegCount; wLoop++)
        {
            if(wLoop >= OAM_CLI_APP_SHM_NUM)
            {
                bRet = FALSE;
                break;
            }

            /* 已经注册过了 */
            if(wModule == g_ptOamIntVar->tLompCliAppRegInfo.tCliReg[wLoop].tAppJid.wModule)
            {
                bRet = TRUE;
                break;
            }
        }
    }

    /*如果数据区注册表中没有，检查共享内存*/
    if(!bRet)
    {
        T_CliAppAgtRegEx *ptOamCliReg = NULL;
        /*必需在使用后调用Oam_UnLockRegTable释放锁*/
        ptOamCliReg = Oam_GetRegExTableAndLock();
        if (ptOamCliReg)
        {
            for(wLoop = 0; wLoop < ptOamCliReg->dwRegCount; wLoop++)
            {
                if(wLoop >= OAM_CLI_APP_SHM_NUM)
                {
                    bRet = FALSE;
                    break;
                }

                if (wModule == ptOamCliReg->tCliReg[wLoop].tAppJid.wModule)
                {
                    bRet = TRUE;
                    break;
                }
            }

            Oam_UnLockRegExTable(ptOamCliReg);
        }
    }

    return bRet;
}

/**************************************************************************
* 函数名称：BOOL CheckMsgHead
* 功能描述：检查消息头是否合法
* 访问的表：无
* 修改的表：无
* 输入参数：
    MSG_COMM_OAM *ptMsg
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
BOOL CheckMsgHead(MSG_COMM_OAM *ptMsg)
{
    WORD16 wMsgLenTmp = 0;
    
    XOS_GetMsgDataLen(PROCTYPE_OAM_CLIMANAGER, &wMsgLenTmp);
    
    if (ptMsg->CmdID   <= 0 ||                    /*命令ID不能为非正*/
            ptMsg->DataLen < sizeof(MSG_COMM_OAM) ||  /*数据长度不能小于结构长度*/
            ptMsg->DataLen > MAX_APPMSG_LEN||        /*数据长度不能超过规定长度*/
            ptMsg->DataLen != wMsgLenTmp)             /*数据长度要等于消息长度*/
        return FALSE;
    else
        return TRUE;
}

/**************************************************************************
* 函数名称：BYTE CheckGetOrSet
* 功能描述：检查是Get还是Set操作: 0 代表是Get操作  1 代表是Set操作
                                  2 TRAP 3 ADD 4 DEC 5 PROCESS 6 INVALID-MODE        
* 访问的表：无
* 修改的表：无
* 输入参数：
    DWORD CmdID
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
BYTE CheckGetOrSet(DWORD CmdID)
{
    CMDID *pGetGSFlag;

    pGetGSFlag = (CMDID *)(&CmdID);

    return (BYTE)pGetGSFlag->OPM;
}

/**************************************************************************
* 函数名称：BOOL IsSaveCmd
* 功能描述：检查是否存盘命令
                                  0 GET 1 SET 2 SAVE 3 DEBUG 4 PROCESS        
* 访问的表：无
* 修改的表：无
* 输入参数：
    DWORD CmdID
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2008/08/07    V1.0    傅龙锁      创建
**************************************************************************/
BOOL IsSaveCmd(DWORD CmdID)
{
    CMDID *pGetGSFlag;
    BYTE ucOptType = 0;

    pGetGSFlag = (CMDID *)(&CmdID);

    ucOptType = (BYTE)pGetGSFlag->OPM;
    if (ucOptType != IS_SAVE)
        return FALSE;

    return TRUE;
}

/**************************************************************************
* 函数名称：DWORD GetMsgStructSize
* 功能描述：
* 访问的表：无
* 修改的表：无
* 输入参数：
    MSG_COMM_OAM *ptMsg
    OPR_DATA *pOprData
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
DWORD GetMsgStructSize(MSG_COMM_OAM *ptMsg, OPR_DATA *pOprData)
{
    if (pOprData)
        return (DWORD)((BYTE *)pOprData->Data + pOprData->Len - (BYTE *)ptMsg);
    else
        return sizeof(MSG_COMM_OAM);
}

#if 0
/**************************************************************************
* 函数名称：VOID SetOprData
* 功能描述：
* 访问的表：无
* 修改的表：无
* 输入参数：
    MSG_COMM_OAM *ptMsg,
    OPR_DATA *pOprData,
    BYTE bType,
    BYTE *value
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
VOID SetOprData(MSG_COMM_OAM *ptMsg, OPR_DATA *pOprData, BYTE bType, BYTE *value)
{
    pOprData->Len = 0;
    switch(bType)
    {
    case DATA_TYPE_BYTE:
    case DATA_TYPE_CHAR:
    case DATA_TYPE_CONST:
        *(pOprData->Data) = *((BYTE *)value);
        pOprData->Len = sizeof(BYTE);
        break;
    case DATA_TYPE_WORD:
        pOprData->Len = sizeof(WORD16);
        memcpy(pOprData->Data, value, pOprData->Len);
        break;
    case DATA_TYPE_DWORD:
    case DATA_TYPE_INT:
    case DATA_TYPE_FLOAT:
        pOprData->Len = sizeof(DWORD);
        memcpy(pOprData->Data, value, pOprData->Len);
        break;
    case DATA_TYPE_TABLE:
    case DATA_TYPE_LIST:
        break;
    case DATA_TYPE_IPADDR:
    case DATA_TYPE_MASK:
    case DATA_TYPE_IMASK:
        pOprData->Len = LEN_IPADDR;
        memcpy(pOprData->Data, value, pOprData->Len);
        break;
    case DATA_TYPE_IPV6ADDR:
        pOprData->Len = LEN_IPV6ADDR;
        memcpy(pOprData->Data, value, pOprData->Len);
        break;
    case DATA_TYPE_MACADDR:
        pOprData->Len = LEN_MACADDR;
        memcpy(pOprData->Data, value, pOprData->Len);
        break;
    case DATA_TYPE_DATE:
    case DATA_TYPE_TIME:
        pOprData->Len = sizeof(DWORD);
        memcpy(pOprData->Data, value, pOprData->Len);
        break;
    case DATA_TYPE_IFPORT:
        break;
    case DATA_TYPE_HEX:
        break;
    default:
        return;
    }

    pOprData->Type = bType;
    pOprData->ParaNo = ptMsg->Number;
    ++ptMsg->Number;
    return;
}
#endif

/**************************************************************************
* 函数名称：VOID SetMapOprData
* 功能描述：
* 访问的表：无
* 修改的表：无
* 输入参数：
    MSG_COMM_OAM *ptMsg,
    OPR_DATA *pOprData,
    WORD16 v1, WORD16 v2
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
void SetMapOprData(MSG_COMM_OAM *ptMsg, OPR_DATA *pOprData, WORD16 v1, WORD16 v2)
{
    pOprData->Len = sizeof(DWORD);
    memcpy(pOprData->Data, &v1, sizeof(WORD16));
    memcpy(pOprData->Data + sizeof(WORD16), &v2, sizeof(WORD16));

    pOprData->Type = DATA_TYPE_MAP;
    pOprData->ParaNo = ptMsg->Number;
    ++ptMsg->Number;
    return;
}

#if 0
/**************************************************************************
* 函数名称：VOID SetStringOprData
* 功能描述：
* 访问的表：无
* 修改的表：无
* 输入参数：
    MSG_COMM_OAM *ptMsg,
    OPR_DATA *pOprData,
    BYTE *szValue,
    WORD16 wMaxLen
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
VOID SetStringOprData(MSG_COMM_OAM *ptMsg, OPR_DATA *pOprData, BYTE *szValue, WORD16 wMaxLen)
{
    for (pOprData->Len = 0; (pOprData->Len < wMaxLen) && (szValue[pOprData->Len] != 0); pOprData->Len++)
        pOprData->Data[pOprData->Len] = 0;

    memcpy(pOprData->Data, szValue, pOprData->Len);
    pOprData->Data[pOprData->Len++] = 0;

    pOprData->Type = DATA_TYPE_STRING;
    pOprData->ParaNo = ptMsg->Number;
    ++ptMsg->Number;
    return;
}
#endif

void RecvFromDBS(WORD16 wMsgId, LPVOID pMsgPara, CHAR *errStr, WORD32 dwBufSize,BOOLEAN bIsSameEndian)
{
    _db_t_saveresult *pSaveResult  = NULL;
    WORD16 wDbNum = 0;
    
    pSaveResult = (_db_t_saveresult  *)pMsgPara;

    /*字节序转换*/
    if (!bIsSameEndian)
    {
        pSaveResult->wDbNum = XOS_InvertWord16(pSaveResult->wDbNum);
        pSaveResult->wTranId = XOS_InvertWord16(pSaveResult->wTranId);
        for (wDbNum = 0; wDbNum < pSaveResult->wDbNum; wDbNum++)
        {
            pSaveResult->tDatabaseSaveResult[wDbNum].wReason = XOS_InvertWord16(pSaveResult->tDatabaseSaveResult[wDbNum].wReason);
            pSaveResult->tDatabaseSaveResult[wDbNum].wSlaveRes = XOS_InvertWord16(pSaveResult->tDatabaseSaveResult[wDbNum].wSlaveRes);
        }
    }
    
    switch(wMsgId)
    {
    case  EV_SAVEOK:
        strncpy(errStr, Hdr_szSaveZDBOK, dwBufSize);
        break;
        
    case  EV_SAVEFAIL:
        strncpy(errStr, Hdr_szSaveZDBFail, dwBufSize);

        for (wDbNum = 0; wDbNum < pSaveResult->wDbNum; wDbNum++)
        {
            strncat(errStr, "\n", dwBufSize);
            strncat(errStr, pSaveResult->tDatabaseSaveResult[wDbNum].dbName, dwBufSize);
            strncat(errStr, " : ", dwBufSize);
#if 0
            /** @brief 主备目录存盘都失败*/
            if (pSaveResult->tDatabaseSaveResult[wDbNum].wReason == _DB_E_SAVE_BOTHPATH)
                strncat(errStr, Hdr_szSaveZDBErrSaveBothPath, dwBufSize);
            /** @brief 主目录存盘失败*/
            else if (pSaveResult->tDatabaseSaveResult[wDbNum].wReason == _DB_E_SAVE_MAINPATH)
                strncat(errStr, Hdr_szSaveZDBErrMainPath, dwBufSize);
            /** @brief备目录存盘失败*/
            else if (pSaveResult->tDatabaseSaveResult[wDbNum].wReason == _DB_E_SAVE_BACKUP)
                strncat(errStr, Hdr_szSaveZDBErrBackup, dwBufSize);
            /** @brief没有表需要存盘*/
            else if (pSaveResult->tDatabaseSaveResult[wDbNum].wReason == _DB_NOT_TABLE_SAVE)
                strncat(errStr, Hdr_szSaveZDBErrNoTblSave, dwBufSize);
            /** @brief读取文件长度错误*/
            else if (pSaveResult->tDatabaseSaveResult[wDbNum].wReason == _DB_READ_FILE_ERROR)
                strncat(errStr, Hdr_szSaveZDBErrReadFileErr, dwBufSize);
            /** @brief表不需要存盘*/
            else if (pSaveResult->tDatabaseSaveResult[wDbNum].wReason == _DB_TABLE_NOT_NEED_SAVE)
                strncat(errStr, Hdr_szSaveZDBErrTblNotNeedSave, dwBufSize);
            /** @brief 库正在进行本机存盘 */
            else if (pSaveResult->tDatabaseSaveResult[wDbNum].wReason == _DB_E_LOCAL_SAVING)
                strncat(errStr, Hdr_szSaveZDBErrLocalSaving, dwBufSize);
            /** @brief 库正在进行主备存盘 */
            else if (pSaveResult->tDatabaseSaveResult[wDbNum].wReason == _DB_E_MS_SAVING)
                strncat(errStr, Hdr_szSaveZDBErrMSSaving, dwBufSize);
            /** @brief 库正在进行上电加载 */
            else if (pSaveResult->tDatabaseSaveResult[wDbNum].wReason == _DB_E_POWERON_LOADING)
                strncat(errStr, Hdr_szSaveZDBErrPowerOnLoading, dwBufSize);
            /** @brief 库正在进行RX FTP加载 */
            else if (pSaveResult->tDatabaseSaveResult[wDbNum].wReason == _DB_E_FTP_LOADING)
                strncat(errStr, Hdr_szSaveZDBErrFtpLoading, dwBufSize);
            /** @brief 库正在进行APP FTP加载 */
            else if (pSaveResult->tDatabaseSaveResult[wDbNum].wReason == _DB_E_APPFTP_LOADING)
                strncat(errStr, Hdr_szSaveZDBErrAppFtpLoading, dwBufSize);
            /** @brief IO正在忙 */
            else if (pSaveResult->tDatabaseSaveResult[wDbNum].wReason == _DB_E_IOBUSY)
                strncat(errStr, Hdr_szSaveZDBErrIOBusy, dwBufSize);
            /** @brief 非法句柄 */
            else if (pSaveResult->tDatabaseSaveResult[wDbNum].wReason == _DB_E_INVALIDHANDLE)
                strncat(errStr, Hdr_szSaveZDBErrInvalidHandle, dwBufSize);
            else
                strncat(errStr, Hdr_szSaveZDBErrOther, dwBufSize);
                
    #endif            
        }
        
        break;
    default:
        XOS_snprintf(errStr, dwBufSize, "%%Unknown MsgID!\n");
        break;
    }

    return;
}

/**************************************************************************
* 函数名称：VOID SendPacketToExecute
* 功能描述：发送消息到Execute
* 访问的表：无
* 修改的表：无
* 输入参数：MSG_COMM_OAM *ptMsg
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
void SendPacketToExecute(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *ptMsg, JID tJid)
{
    WORD16 wMsgHeadLen = (WORD16)((BYTE *)ptMsg->Data - (BYTE *)ptMsg);
    /*由于这里的ptMsg已经用了aucSendBuf，因此这里给解释回消息用aucRecvBuf*/
    MSG_COMM_OAM *ptRtnMsg = (MSG_COMM_OAM *)g_ptOamIntVar->aucRecvBuf;

    TYPE_MATCH_RESULT *pMatchResult = &(pLinkState->tMatchResult);
    WORD16 wCmdAttrPos = pMatchResult->wCmdAttrPos;
    T_OAM_Cfg_Dat* cfgDat = pMatchResult->cfgDat;

    memcpy(ptRtnMsg, ptMsg, wMsgHeadLen);

    /*  ptRtnMsg->PacketNo   = INIT_PACKET_NO;       包号从1开始 */
    ptRtnMsg->OutputMode = OUTPUT_MODE_REQUEST; /*OUTPUT_MODE_NORMAL;     初始时Telnet输出模式为request */
    ptRtnMsg->Number     = 0;
/* 解释和执行已经合一，发送不需要检查
    if (!CheckMsgHead(ptMsg))
    {
        ptRtnMsg->ReturnCode = ERR_MSG_FROM_INTPR;
        ptRtnMsg->DataLen    = sizeof(MSG_COMM_OAM);
        Oam_CmdReturnMsgHandler(pLinkState,ptRtnMsg);
        return;
    }
*/
    /*如果是备板存盘命令，先发送消息到备板看备板是否有应答*/
    if (IsSaveCmd(ptMsg->CmdID))
    {
        Oam_ExecSaveCmd(pLinkState);        
        return;
    }

    if (bDebugPrint)
        PrintMSGCOMMOAMStruct(ptMsg,"MSG_CLIMANAGER_TO_APP");

    /*if (EqualJID(&tJid, &g_ptOamIntVar->jidSelf))*/
    if(cfgDat->tCmdAttr[wCmdAttrPos].bExePos == EXEPOS_XMLRPC)
    {
        DisposeXmlRpcCmd(pLinkState);
    }
    else if (tJid.dwJno == g_ptOamIntVar->jidSelf.dwJno)
    {
        ExecCLIProc(pLinkState, ptMsg, ptRtnMsg);
    }
    else
    {
        ExecAppProc(pLinkState, ptMsg, tJid);
    }

    return;
}

/*处理异步消息，debug等*/
static void ProcessAsycMsg(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *pMsg)
{ 
    /*611001435262 debug消息接口改为可以返回n个字符串类型*/
    OPR_DATA *pRtnOprData = NULL;
    WORD16 i = 0;
    WORD16 iBufSize = MAX_OAMMSG_LEN - sizeof(TYPE_INTERPRET_MSGHEAD) -3;
    static CHAR ucaBuf[MAX_OAMMSG_LEN];
    memset(ucaBuf, 0, sizeof(ucaBuf));
	
    /*如果目前telsvr输入方式不是异步就不显示debug消息*/ 
    if (TRUE== Oam_LinkIfTelSvrInputModeIsSyn(pLinkState))
    {
        pLinkState->bRecvAsynMsg = FALSE;
        XOS_SysLog(OAM_CFG_LOG_NOTICE, "receive async msg, but terminal-display not opened, discarded!");
	 return;	
    }
	
    /*debug消息在新行显示*/
    ucaBuf[0] = '\n';
    /*拼接收到的字符串类型参数*/
    pRtnOprData = (OPR_DATA *)pMsg->Data;
    for (i = 0; i < pMsg->Number; i++)
    {  
        CHAR aucTmpData[256] = {0};
        
        if (pRtnOprData->Type != DATA_TYPE_STRING)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, "receive async msg, but have para number is not DATA_TYPE_STRING!!");
            return;
        }
		
        memcpy(aucTmpData, pRtnOprData->Data, pRtnOprData->Len);
        if ((strlen(ucaBuf) + strlen(aucTmpData)) > iBufSize)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, "receive async msg, but return string is longer than %d bytes, has been truncated!", iBufSize);
            break;
        }

        XOS_snprintf(ucaBuf, sizeof(ucaBuf), "%s%s", ucaBuf, aucTmpData);
        pRtnOprData = (OPR_DATA *)(pRtnOprData->Data + pRtnOprData->Len);
    }

    SendStringToTelnet(pLinkState,ucaBuf);
   	
    return;
}

void CfgMsgTransByteOder(MSG_COMM_OAM *ptr)
{
    MSG_COMM_OAM * pMsgHead = (MSG_COMM_OAM *)ptr;
    OPR_DATA * pData;
    BYTE *pCurPos  = NULL;
    BYTE ucTableVarc= 0;
    INT i = 0;
    BYTE ucParaCount = 0;

    pMsgHead->SeqNo = XOS_InvertWord32(pMsgHead->SeqNo);             /*DWORD SeqNo;      */
    pMsgHead->DataLen = XOS_InvertWord32(pMsgHead->DataLen);         /*DWORD DataLen;    */
    pMsgHead->CmdID = XOS_InvertWord32(pMsgHead->CmdID);             /*DWORD CmdID;      */
    pMsgHead->LinkChannel = XOS_InvertWord32(pMsgHead->LinkChannel); /*DWORD LinkChannel;*/
    pMsgHead->ReturnCode = XOS_InvertWord16(pMsgHead->ReturnCode);   /*WORD16 ReturnCode;*/

    ucParaCount = pMsgHead->Number;
    if (ucParaCount <= 0)
    {
        return;
    }

    pCurPos = (BYTE *)pMsgHead->Data;
    pData = (OPR_DATA *)pCurPos;
    if (!pData)
    {
        return;
    }

    for (i = 0; i < ucParaCount; i++)
    {
        switch (pData->Type)
        {
        case DATA_TYPE_IPADDR:
        case DATA_TYPE_MASK:
        case DATA_TYPE_IMASK:
        case DATA_TYPE_DWORD:
        case DATA_TYPE_VMMDATE:
        case DATA_TYPE_VMMDATETIME:
            {
                DWORD value;
                memcpy(&value, pData->Data, (size_t)sizeof(DWORD));
                value = XOS_InvertWord32(value);
                memcpy(pData->Data, &value, (size_t)sizeof(DWORD));
            }
            break;
        case DATA_TYPE_WORD:
            {
                WORD16 value;
                memcpy(&value, pData->Data, (size_t)sizeof(WORD16));
                value = XOS_InvertWord16(value);
                memcpy(pData->Data, &value, (size_t)sizeof(WORD16));
            }
            break;
        case DATA_TYPE_WORD64: 
            {
                WORD64 value;
                memcpy(&value, pData->Data, (size_t)sizeof(WORD64));
                value = XOS_InvertWord64(value);
                memcpy(pData->Data, &value, (size_t)sizeof(WORD64));
            }
            break;
        case DATA_TYPE_VPN_IP:
            {
                VPN_IP *value = (VPN_IP *)(pData->Data);
                value->ipaddr = XOS_InvertWord32(value->ipaddr);
                value->nn     = XOS_InvertWord16(value->nn);
            }
            break;
        case DATA_TYPE_VPN_ASN:
            {
                VPN_ASN *value = (VPN_ASN *)(pData->Data);
                value->asn = XOS_InvertWord16(value->asn);
                value->nn  = XOS_InvertWord32(value->nn);
            }
            break;
        case DATA_TYPE_MAP:
            {
                WORD16 wType, wValue;
                BYTE *pParaBuffer = (BYTE *)(pData->Data);
                memcpy(&wType, pParaBuffer, sizeof(WORD16));
                memcpy(&wValue, (BYTE *)pParaBuffer + sizeof(WORD16), sizeof(WORD16));
                wType = XOS_InvertWord16(wType);
                wValue = XOS_InvertWord16(wValue);
                memcpy(pData->Data, &wType, (size_t)sizeof(WORD16));
                memcpy((BYTE *)(pData->Data)+sizeof(WORD16), &wValue, (size_t)sizeof(WORD16));
            }
            break;
        case DATA_TYPE_TABLE:
        case DATA_TYPE_LIST:
            {
                BYTE m = 0;
                BYTE n = 0;
                TABLE_DATA *pTableData = NULL;

                ucTableVarc = 1;

                pTableData = (TABLE_DATA *)pData->Data;
                pCurPos += sizeof(TABLE_DATA) + sizeof(OPR_DATA) - sizeof(BYTE) - sizeof(BYTE);
                for(n = 0; n < pData->Len; n++)
                {
                    for(m = 0; m < pTableData->ItemNumber; m++)
                    {
                        ucTableVarc = 2;
                        switch(pTableData->Item[m].Type)
                        {
                        case DATA_TYPE_IPADDR:
                        case DATA_TYPE_MASK:
                        case DATA_TYPE_IMASK:
                        case DATA_TYPE_DWORD:
                        case DATA_TYPE_VMMDATE:
                        case DATA_TYPE_VMMDATETIME:
                            {
                                DWORD value;
                                memcpy(&value, pCurPos, (size_t)sizeof(DWORD));
                                value = XOS_InvertWord32(value);
                                memcpy(pCurPos, &value, (size_t)sizeof(DWORD));
                            }
                            break;
                        case DATA_TYPE_WORD:
                            {
                                WORD16 value;
                                memcpy(&value, pCurPos, (size_t)sizeof(WORD16));
                                value = XOS_InvertWord16(value);
                                memcpy(pCurPos, &value, (size_t)sizeof(WORD16));
                            }
                            break;
                        case DATA_TYPE_WORD64:
                            {
                                WORD64 value;
                                memcpy(&value, pData->Data, (size_t)sizeof(WORD64));
                                value = XOS_InvertWord64(value);
                                memcpy(pData->Data, &value, (size_t)sizeof(WORD64));
                            }
                            break;
                        case DATA_TYPE_VPN_IP:
                            {
                                VPN_IP *value = (VPN_IP *)pCurPos;
                                value->ipaddr = XOS_InvertWord32(value->ipaddr);
                                value->nn     = XOS_InvertWord16(value->nn);
                            }
                            break;
                        case DATA_TYPE_VPN_ASN:
                            {
                                VPN_ASN *value = (VPN_ASN *)pCurPos;
                                value->asn = XOS_InvertWord16(value->asn);
                                value->nn  = XOS_InvertWord32(value->nn);
                            }
                            break;
                        case DATA_TYPE_MAP:
                            {
                                WORD16 wType, wValue;
                                BYTE *pParaBuffer = (BYTE *)pCurPos;
                                memcpy(&wType, pParaBuffer, sizeof(WORD16));
                                memcpy(&wValue, (BYTE *)pParaBuffer + sizeof(WORD16), sizeof(WORD16));
                                wType = XOS_InvertWord16(wType);
                                wValue = XOS_InvertWord16(wValue);
                                memcpy(pCurPos, &wType, (size_t)sizeof(WORD16));
                                memcpy((BYTE *)pCurPos+sizeof(WORD16), &wValue, (size_t)sizeof(WORD16));
                            }
                            break;
                        default:
                            /*pCurPos += pTableData->Item[m].Len;*/
                            break;
                        }

                        if ((pData->Type == DATA_TYPE_LIST) && 
                            (pTableData->Item[m].Type == DATA_TYPE_STRING))
                        {
                            pCurPos += strlen((CHAR *)pCurPos) + 1;
                        }
                        else
                        {
                            pCurPos += pTableData->Item[m].Len;
                        }
                    }
                }
            }
            break;

        default:
            break;
        }

        if(ucTableVarc == 2)
        {
            pData = (OPR_DATA *)pCurPos;
            ucTableVarc = 0;
        }
        else if(ucTableVarc == 1)
        {
            pCurPos += sizeof(BYTE);
            pData = (OPR_DATA *)pCurPos;
            ucTableVarc = 0;
        }
        else if(ucTableVarc == 0)
        {
            pCurPos += sizeof(OPR_DATA) + pData->Len - sizeof(BYTE);
            pData = (OPR_DATA *)pCurPos;
        }
    }
}
#if 0
VOID CfgMsgTransAddrByteOder(MSG_COMM_OAM *ptr)
{
    MSG_COMM_OAM * pMsgHead = (MSG_COMM_OAM *)ptr;
    OPR_DATA * pData;
    BYTE *pCurPos  = NULL;
    BYTE ucTableVarc= 0;
    INT i = 0;

    WORD wParaCount = pMsgHead->Number;

    pCurPos = (BYTE *)pMsgHead->Data;

    if (wParaCount <= 0)
    {
        return;
    }

    pData = (OPR_DATA *)pCurPos;

    if (!pData)
    {
        return;
    }

    for (i = 0; i < wParaCount; i++)
    {
        switch (pData->Type)
        {
        case DATA_TYPE_IPADDR:
        case DATA_TYPE_MASK:
        case DATA_TYPE_IMASK:
            {
                DWORD value;
                memcpy(&value, pData->Data, (size_t)sizeof(DWORD));
                value = XOS_InvertWord32(value);
                memcpy(pData->Data, &value, (size_t)sizeof(DWORD));
            }
            break;
        case DATA_TYPE_TABLE:
        case DATA_TYPE_LIST:
            {
                BYTE m = 0;
                BYTE n = 0;
                TABLE_DATA *pTableData = NULL;

                ucTableVarc = 1;

                pTableData = (TABLE_DATA *)pData->Data;
                pCurPos += sizeof(TABLE_DATA) + sizeof(OPR_DATA) - sizeof(BYTE) - sizeof(BYTE);
                for(n = 0; n < pData->Len; n++)
                {
                    for(m = 0; m < pTableData->ItemNumber; m++)
                    {
                        ucTableVarc = 2;
                        switch(pTableData->Item[m].Type)
                        {

                        case DATA_TYPE_IPADDR:
                        case DATA_TYPE_MASK:
                        case DATA_TYPE_IMASK:
                            {
                                DWORD value;
                                /*memcpy(&value, pData->Data, (size_t)sizeof(DWORD));*/
                                memcpy(&value, pCurPos, (size_t)sizeof(DWORD));
                                value = XOS_InvertWord32(value);
                                /*memcpy(pData->Data, &value, (size_t)sizeof(DWORD));*/
                                memcpy(pCurPos, &value, (size_t)sizeof(DWORD));
                            }
                            break;

                        default:
                            /*pCurPos += pTableData->Item[m].Len;*/
                            break;
                        }

                        if ((pData->Type == DATA_TYPE_LIST) && 
                            (pTableData->Item[m].Type == DATA_TYPE_STRING))
                        {
                            pCurPos += strlen((CHAR *)pCurPos) + 1;
                        }
                        else
                        {
                            pCurPos += pTableData->Item[m].Len;
                        }
                    }
                }
            }
            break;

        default:
            break;
        }

        if(ucTableVarc == 2)
        {
            pData = (OPR_DATA *)pCurPos;
            ucTableVarc = 0;
        }
        else if(ucTableVarc == 1)
        {
            pCurPos += sizeof(BYTE);
            pData = (OPR_DATA *)pCurPos;
            ucTableVarc = 0;
        }
        else if(ucTableVarc == 0)
        {
            pCurPos += sizeof(OPR_DATA) + pData->Len - sizeof(BYTE);
            pData = (OPR_DATA *)pCurPos;
        }
    }
}

#endif

void Oam_SetCmdPlanTimer(TYPE_LINK_STATE *pLinkState)
{
    pLinkState->wCmdPlanTimer = XOS_SetRelativeTimer(PROCTYPE_OAM_CLIMANAGER, TIMER_CMD_PLAN, DURATION_TIMER_CMD_PLAN, pLinkState->ucLinkID);
    return;
}

void Oam_KillCmdPlanTimer(TYPE_LINK_STATE *pLinkState)
{
    if (INVALID_TIMER_ID != pLinkState->wCmdPlanTimer)
    {
        XOS_KillTimerByTimerId(pLinkState->wCmdPlanTimer);
        pLinkState->wCmdPlanTimer = INVALID_TIMER_ID;
    }
    return;
}

void Oam_RecvPlanInfoFromApp(TYPE_LINK_STATE *pLinkState, LPVOID pMsgPara, BOOLEAN bIsSameEndian)
{
    T_Cmd_Plan_Cfg *pPlanInfo =  NULL;
    CHAR szPlanStr[MAX_STRING_LEN] = {0};
    CHAR *pPlanStr = szPlanStr;

    Oam_KillCmdPlanTimer(pLinkState);
    Oam_SetCmdPlanTimer(pLinkState);

    Oam_KillExeTimer(pLinkState);
    Oam_SetExeTimer(pLinkState);

    pPlanInfo = (T_Cmd_Plan_Cfg *)pMsgPara;
    if (!bIsSameEndian)
    {
        pPlanInfo->wPlanNo = XOS_InvertWord16(pPlanInfo->wPlanNo);
        pPlanInfo->dwCmdID = XOS_InvertWord32(pPlanInfo->dwCmdID);
        pPlanInfo->dwLinkChannel = XOS_InvertWord32(pPlanInfo->dwLinkChannel);
    }

    if ((pPlanInfo->ucPlanValue == 0) || (pPlanInfo->ucPlanValue >= 100))
        return;
    
    if ((pLinkState->ucPlanValuePrev > 0) && (pLinkState->ucPlanValuePrev == pPlanInfo->ucPlanValue))
        return;

    
    pLinkState->ucPlanValuePrev = pPlanInfo->ucPlanValue;

    /* 将进度信息发送到telnet显示 */
    XOS_snprintf(pPlanStr, MAX_STRING_LEN, map_szExePlanFmtStr,pPlanInfo->ucPlanValue);
    /*pLinkState->bOutputCmdLine = FALSE;*/
    pLinkState->bOutputMode = OUTPUT_MODE_ROLL;
    pLinkState->wAppMsgReturnCode = SUCC_WAIT_AND_HAVEPARA;
    /*如果进度是100，补充一个换行符，并复位为0*/
    if (pPlanInfo->ucPlanValue == 100)
    {
        strncat(pPlanStr, "\n\r", MAX_STRING_LEN);
        pLinkState->ucPlanValuePrev = 0;
    }
    SendStringToTelnet(pLinkState, pPlanStr);

    pLinkState->bOutputMode = OUTPUT_MODE_NORMAL;
}

void Oam_DealPlanTimer(TYPE_LINK_STATE *pLinkState)
{
    int iType = 0;
    CHAR szPlanStr[MAX_STRING_LEN] = {0};
    CHAR *pPlanStr = szPlanStr;
    BOOLEAN bOutputCmdLine = TRUE;

    /*判断BIT31标志位*/
    iType = ((pLinkState->ucLinkID)&0X00000200)>>9; /*BIT9 1-表示XML应答0-CLI应答*/
    if (iType == LINKSTATE_TYPE_CLI)
    {
        XOS_SysLog(OAM_CFG_LOG_DEBUG, "Oam_DealPlanTimer: LINKSTATE_TYPE_CLI == iLinkStateType!");

        Oam_KillCmdPlanTimer(pLinkState);
        Oam_SetCmdPlanTimer(pLinkState);

        /* 将进度信息发送到telnet显示 */
        XOS_snprintf(pPlanStr, MAX_STRING_LEN, map_szExePlanFmtStr,pLinkState->ucPlanValuePrev);
        /* 记录下原先的值，在发送进度之前将其赋值为FALSE，发送完之后再恢复原值 */
        bOutputCmdLine = pLinkState->bOutputCmdLine;
        pLinkState->bOutputCmdLine = FALSE;
        pLinkState->wAppMsgReturnCode = SUCC_WAIT_AND_HAVEPARA;
        SendStringToTelnet(pLinkState, pPlanStr);
        pLinkState->bOutputCmdLine = bOutputCmdLine;
    }
    else if (iType == LINKSTATE_TYPE_XML)
    {
    }
    else
    {
        return;
    }
    
}

BOOL WriteLogOfCmdLine(T_CliCmdLogRecord *pCmdLog, BOOL bCmdExecRslt)
{
    XOS_STATUS dwRtn = XOS_SUCCESS;
    XOS_FD   dwFileFD = OAM_INVALID_FD;
    SWORD32 swCnt = 0;
    SWORD32 swLen = 0;
    CHAR *pLogBuf = (CHAR *)g_ptOamIntVar->aucSendBuf;
    CHAR szConnType[10] = {0};
    CHAR *pConnType = szConnType;
    CHAR  acOamDir[FILE_PATH_LEN] = {'\0'}; 
    CHAR  acMateDir[FILE_PATH_LEN] = {'\0'}; 
    CHAR  acCmdLogFile[MAX_ABS_PATH_LEN] = {'\0'};
    CHAR  acCmdLogFileBak[MAX_ABS_PATH_LEN] = {'\0'};

    if (!OamGetPath(acOamDir, acMateDir))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[WriteLogOfCmdLine]OamGetPath fail!\n");
        return FALSE;
    }
    XOS_snprintf(acCmdLogFile, MAX_ABS_PATH_LEN, "%s%s", acOamDir, OAM_CMDLINE_LOG_FILENAME);
    XOS_snprintf(acCmdLogFileBak, MAX_ABS_PATH_LEN, "%s%s", acOamDir, OAM_CMDLINE_LOG_FILENAME_BAK);
    
    /* 打开文件，如果文件不存在则创建该文件 */
    dwRtn = XOS_OpenFile(acCmdLogFile, XOS_CREAT, (XOS_FD *)&dwFileFD);
    if (XOS_SUCCESS != dwRtn)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[WriteLogOfCmdLine]Open file fail! dwRtn = %d\n", dwRtn);
        return FALSE;
    }

    /* 定位到文件尾部 */
    dwRtn = XOS_SeekFile(dwFileFD, XOS_SEEK_END, 0);
    if (XOS_SUCCESS != dwRtn)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[WriteLogOfCmdLine]Seek file fail! dwRtn = %d\n", dwRtn);
        XOS_CloseFile(dwFileFD);
        return FALSE;
    }

    /* 组织LOG字符串 */
    memset(g_ptOamIntVar->aucSendBuf, 0, sizeof(g_ptOamIntVar->aucSendBuf));

    if (OAM_CONN_TYPE_SERIAL == pCmdLog->ucConnType)
        XOS_snprintf(pConnType, sizeof(szConnType), "%s", "SERIAL");
    else if (OAM_CONN_TYPE_TELNET == pCmdLog->ucConnType)
        XOS_snprintf(pConnType, sizeof(szConnType), "%s", "TELNET");
    else
        XOS_snprintf(pConnType, sizeof(szConnType), "%s", "SSH");

    XOS_snprintf(pLogBuf,
                 MAX_OAMMSG_LEN,
                 "%d-%d-%d %d:%d:%d, %s, %s, %s, %s",
                 pCmdLog->tSysTime.wSysYear,pCmdLog->tSysTime.ucSysMon,pCmdLog->tSysTime.ucSysDay,
                 pCmdLog->tSysTime.ucSysHour,pCmdLog->tSysTime.ucSysMin,pCmdLog->tSysTime.ucSysSec,pConnType,
                 pCmdLog->szUserName, pCmdLog->szIpAddr, pCmdLog->szCmdLine);
    
    if (bCmdExecRslt)
        XOS_snprintf(pLogBuf, MAX_OAMMSG_LEN, "%s, SUCCESS\n", pLogBuf);
    else
        XOS_snprintf(pLogBuf, MAX_OAMMSG_LEN, "%s, FAILED\n", pLogBuf);

    swLen = strlen(pLogBuf);
    dwRtn =  XOS_WriteFile(dwFileFD, (void *)pLogBuf, swLen, &swCnt);
    if (XOS_SUCCESS !=dwRtn )
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[WriteLogOfCmdLine]Write log file fail! dwRtn = %d\n", dwRtn);
        XOS_CloseFile(dwFileFD);
        return FALSE;
    }

    /* 写完之后，关闭文件FD */
            XOS_CloseFile(dwFileFD);

    /* 写入成功之后，将日志上报SYSLOG服务器 */
    XOS_SysLog(OAM_CFG_LOG_NOTICE, pLogBuf);

    /* 写入一条日志成功之后，将日志行数加1 */
    g_ptOamIntVar->wLinesOfCmdLogFile++;

    /* 如果超过文件最大行数，则将当前文件更名为备份文件，后面的新的命令行日志写入新的文件中 */
    if (g_ptOamIntVar->wLinesOfCmdLogFile >= OAM_MAX_CMD_LOG_LINES)
            {
        /*删除备份文件*/
        dwRtn = XOS_DeleteFile(acCmdLogFileBak);
        if (XOS_SUCCESS != dwRtn)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, 
                "[WriteLogOfCmdLine]Delete file <%s> fail! dwRtn = %d\n", acCmdLogFileBak, dwRtn);
        }

        /* 更名为备份文件 */
        dwRtn = XOS_RenameFile(acCmdLogFile, acCmdLogFileBak);
        if (XOS_SUCCESS != dwRtn)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, 
                "[WriteLogOfCmdLine]Rename file fail! dwRtn = %d\n", dwRtn);
            return FALSE;
        }

        /* 将全局记录数置为0 */
        g_ptOamIntVar->wLinesOfCmdLogFile = 0;
    }

    return TRUE;
}

BOOL GetCmdLogFileLines(void)
{
    XOS_FD  dwFileFD = OAM_INVALID_FD;
    XOS_STATUS dwRtn = XOS_SUCCESS;
    CHAR szReadBuf[MAX_LOG_FILE_READ_BUF] = {0};
    SWORD32 sdwCountReaded = 0;
    int i = 0;
    CHAR  acOamDir[FILE_PATH_LEN] = {'\0'}; 
    CHAR  acMateDir[FILE_PATH_LEN] = {'\0'}; 
    CHAR  acCmdLogFile[MAX_ABS_PATH_LEN] = {'\0'};

    if (!OamGetPath(acOamDir, acMateDir))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[GetCmdLogFileLines]OamGetPath fail!\n");
        return FALSE;
    }
    XOS_snprintf(acCmdLogFile, MAX_ABS_PATH_LEN, "%s%s", acOamDir, OAM_CMDLINE_LOG_FILENAME);

    /* 打开文件，只读 */
    dwRtn = XOS_OpenFile(acCmdLogFile, XOS_RDONLY, (XOS_FD *)&dwFileFD);
    if (XOS_SUCCESS != dwRtn)
    {
        dwFileFD = OAM_INVALID_FD;
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[GetCmdLogFileLines]Open file fail! dwRtn = %d\n", dwRtn);
        return FALSE;
    }

    do
    {
        memset(szReadBuf, 0, sizeof(szReadBuf));
        dwRtn = XOS_ReadFile(dwFileFD, (VOID *)szReadBuf, sizeof(szReadBuf), &sdwCountReaded);
        if (dwRtn != XOS_SUCCESS)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, "[GetCmdLogFileLines]Read file fail! dwRtn = %d\n", dwRtn);
            XOS_CloseFile(dwFileFD);
            return FALSE;
        }

        for(i = 0; i < MAX_LOG_FILE_READ_BUF; i++)
        {
            if (szReadBuf[i] == MARK_RETURN)
                g_ptOamIntVar->wLinesOfCmdLogFile++;
        }
    }while (!((WORD32)sdwCountReaded < sizeof(szReadBuf)) && dwRtn == XOS_SUCCESS);

    XOS_CloseFile(dwFileFD);
    return TRUE;
}
/**************************************************************************
* 函数名称：VOID Oam_RecvFromCliApp
* 功能描述：接收来自解释内部的消息
* 访问的表：无
* 修改的表：无
* 输入参数：
  LPVOID pMsgPara
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
void Oam_RecvFromCliApp(WORD32 dwMsgId,LPVOID pMsgPara)
{
    int iLinkStateType = 0;
    /*判断LinkState类型*/
    iLinkStateType = OamCfgGetLinkStateType(pMsgPara);
    if (LINKSTATE_TYPE_ERR == iLinkStateType)
    {
        return;
    }
    else if(LINKSTATE_TYPE_XML == iLinkStateType)
    { 
	 Oam_ProRecMsgToOmm(pMsgPara);	
    }
    else if (LINKSTATE_TYPE_CLI == iLinkStateType)
    {
         TYPE_LINK_STATE *pLinkState = NULL;
        pLinkState = OamCfgGetLinkState(dwMsgId,pMsgPara);
        if (!pLinkState)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "failled to get linkstate while processing application msg, vtyno in msg is %d",
                       ((MSG_COMM_OAM *)pMsgPara)->LinkChannel);
            return;
        }	   
        /* 收到应用的返回结果之后，杀掉进度上报等待定时器 */
        Oam_KillCmdPlanTimer(pLinkState);
        pLinkState->ucPlanValuePrev = 0;
        Oam_ProRecMsgFromCliApp(pLinkState,pMsgPara);
    }
    return;
}
void Oam_ProRecMsgFromCliApp(TYPE_LINK_STATE *pLinkState,LPVOID pMsgPara)
{
    MSG_COMM_OAM *ptRcvMsg = (MSG_COMM_OAM *)pMsgPara;    
    BOOL bCmdExecRslt = FALSE; 
    BYTE ucCmdType = 0;	
    WORD32 dwCmdGrp = 0;	
    /*打印收发消息*/
    if (bDebugPrint)
    {
        PrintMSGCOMMOAMStruct(ptRcvMsg,"MSG_INNERAPP_TO_CLIMANAGER");
    }

    if (ptRcvMsg->ReturnCode == SUCC_AND_NOPARA ||
        ptRcvMsg->ReturnCode == SUCC_AND_HAVEPARA ||
        ptRcvMsg->ReturnCode == SUCC_CMD_NOT_FINISHED)
        bCmdExecRslt = TRUE;
    else
        bCmdExecRslt = FALSE;
    
    dwCmdGrp = ptRcvMsg->CmdID & 0xFF000000;
    if (dwCmdGrp == OAM_CLI_DDM_CMDGRP_SET)        // DDM命令SET类
        ucCmdType = IS_SET;
    else if (dwCmdGrp == OAM_CLI_DDM_CMDGRP_GET)   // DDM命令GET类
        ucCmdType = IS_GET;
    else                                           // 普通CLI命令
        ucCmdType = CheckGetOrSet(ptRcvMsg->CmdID);
    
    if ((ptRcvMsg->ucOamTag == OAMCFG_TAG_NORMAL) && 
        ((ucCmdType == IS_SET) || (ucCmdType == IS_SAVE))&&
        (OAM_MSG_IS_LAST_PACKET(ptRcvMsg)) &&
        (ptRcvMsg->ReturnCode != CLI_RET_KEEP_ALIVE_NOPARA))
    {
        /* 写命令行日志 */
        if (!WriteLogOfCmdLine(&pLinkState->tCmdLog, bCmdExecRslt))
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, "[Oam_RecvFromApp] WriteLogOfCmdLine exec fail! line = %d\n", __LINE__);
        }
    }

    /*检查消息头是否合法*/
    if (ptRcvMsg->CmdID   <= 0 || ptRcvMsg->DataLen < sizeof(MSG_COMM_OAM) ||ptRcvMsg->DataLen > MAX_APPMSG_LEN) 
    {
         ptRcvMsg->ReturnCode = ERR_MSG_FROM_INTPR;
         ptRcvMsg->DataLen    = sizeof(MSG_COMM_OAM);
         Oam_CmdReturnMsgHandler(pLinkState,ptRcvMsg);
         return;   
    }
	
    /*?? brs的返回码大于10时候要转换为错误码map类型*/
    if (ptRcvMsg->ReturnCode >= 10 &&
        ptRcvMsg->ReturnCode != SUCC_WAIT_AND_HAVEPARA) 
    {
        ConstructBrsMapPara(ptRcvMsg);
    }
	
    if (ptRcvMsg->ReturnCode == SUCC_CMD_NOT_FINISHED)
    {
        pLinkState->bRestartReqSended = FALSE;
    }

    Oam_CmdReturnMsgHandler(pLinkState,ptRcvMsg);

    return;
}
void Oam_ProRecMsgToOmm(LPVOID pMsgPara)
{
    MSG_COMM_OAM *ptOamMsgBuf = NULL;                       /*OAM消息体临时指针*/  	
    /*入参判断*/
    if(pMsgPara == NULL)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__); 
        return;
    }
    ptOamMsgBuf =(MSG_COMM_OAM *)pMsgPara;
    if (bDebugPrint)
    {
        PrintMSGCOMMOAMStruct(ptOamMsgBuf, "MSG_CLIAPP_TO_XMLAGNET");
    }
    /*?? brs的返回码大于10时候要转换为错误码map类型*/
    if (ptOamMsgBuf->ReturnCode >= 10 &&
        ptOamMsgBuf->ReturnCode != SUCC_WAIT_AND_HAVEPARA) 
    {
        ConstructBrsMapPara(ptOamMsgBuf);
    }
    /*发送平台应答到网管*/
//    SendPLATAckToOMM(ptOamMsgBuf);        
    return;
}

/**************************************************************************
* 函数名称：Oam_SaveCmdLompErr
* 功能描述：存盘过程中判断lomp与omp错误，由内部函数库调用
                              将错误信息返回，执行状态修改为telnet
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：   无
* 其它说明：如果	无网管构造消息，可将其依照匹配形式返回
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/06/08   V1.0     殷浩     创建
**************************************************************************/           
void Oam_InnerSaveCmdLompErr(LPVOID ptOamMsg,WORD32 dwMsgId, CHAR *pStrOutResult)
{
    BOOLEAN bOR = FALSE;
    TYPE_LINK_STATE *pLinkState = NULL;
    
    /*入参判断*/
    if ((NULL == ptOamMsg) || (NULL == pStrOutResult))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return;
    }
    
    pLinkState = OamCfgGetLinkState(dwMsgId,ptOamMsg);
    if (NULL == pLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                "[Oam_InnerSaveCmdLompErr] Failled to get linkstate while processing telnet msg");                         
        return;					 
    }
    /* 杀掉定时器 */
    Oam_KillExeTimer(pLinkState);
    SendTxtSaveAckToClient(pLinkState,pStrOutResult,bOR);
    Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_TELNET);
    return;
}
void  Oam_InnerKillCmdPlanTimer(LPVOID ptOamMsg,WORD32 dwMsgId)
{
    TYPE_LINK_STATE *pLinkState = NULL;
    /*入参判断*/
    if (NULL == ptOamMsg)
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
    Oam_KillCmdPlanTimer(pLinkState);
    return;	
}
void Oam_InnerSetCmdPlanTimer(LPVOID ptOamMsg,WORD32 dwMsgId)
{
    TYPE_LINK_STATE *pLinkState = NULL;
    /*入参判断*/
    if (NULL == ptOamMsg)
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
    Oam_SetCmdPlanTimer(pLinkState);	
    return;		
}

/*
功能:比较两个字符串，不区分大小写
参数: s1和s2 -- 参与比较的字符串、
             n --参与比较的字符数量
返回值: 0 -- 两个字符串相等
                  大于 0 -- s1 > s2
                  小于 0 -- s1 < s2
*/
INT Oam_String_Compare_Nocase(const CHAR *s1, const CHAR *s2,INT n)
{
    INT i;
    if ((NULL == s1) || (NULL == s2))
    {
        return -1;
    }

    for (i = 0; i < n; i++)
    {
        if (toupper(s1[i]) != toupper(s2[i]))
        {
            break;
        }
		
        if (s1[i] == '\0')
        {
            break;
        }
    }
    
    return (i == n) ? 0 : ((INT)s1[i] - (INT)s2[i]);
}


