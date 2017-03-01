/**************************************************************************
* 版权所有 (C)2002, 深圳市中兴通讯股份有限公司。
*
* 文件名称：HrExeMsg.c
* 文件标识：
* 内容摘要：发送消息到Execute
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
#include "offline_to_online.h"
#include "oam_execute.h"
#include "cliapp.h"

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
/*extern  void RecvFromInterpret(LPVOID pMsgPara, JID);*/
extern BOOL bDebugPrint;
extern void SendPacketToExecute(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *ptMsg, JID tJid);
/**************************************************************************
* 函数名称：MSG_COMM_OAM *OamCfgConstructMsgData
* 功能描述：构造消息体
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_LINK_STATE *pLinkState
* 输出参数：
* 返 回 值：    NULL - 构建出错
                                非NULL - 构建成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
MSG_COMM_OAM *OamCfgConstructMsgData(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *ptMsgBuf)
{
    TYPE_MATCH_RESULT *pMatchResult = NULL;
    T_OAM_Cfg_Dat * cfgDat = NULL;
    MSG_COMM_OAM *ptSendMsg = (ptMsgBuf == NULL) ? (MSG_COMM_OAM *)g_ptOamIntVar->aucSendBuf : ptMsgBuf;
    WORD16 wPacketSize = sizeof(MSG_COMM_OAM);
    OPR_DATA * pData = NULL;
    WORD16 i = 0;
    BYTE * pCurPos  = NULL;
    WORD16 wOmmLinkNum = sizeof(gt_XmlLinkState)/sizeof(TYPE_XMLLINKSTATE);	
    WORD16 wLoop = 0;	
    
    if (!pLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return NULL;
    }
    
    pMatchResult = &pLinkState->tMatchResult;
    cfgDat = pMatchResult->cfgDat;

    if ((!cfgDat) || (!ptSendMsg))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return NULL;
    }
    
    if(!cfgDat->tCmdAttr)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return NULL;
    }

    /* calculate the new sequence number */
    pLinkState->dwExeSeqNo++;

    /* fill the message head */
    ptSendMsg->CmdID              = cfgDat->tCmdAttr[pMatchResult->wCmdAttrPos].dExeID;
    ptSendMsg->ReturnCode     = RETURNCODE_INVALID;
    ptSendMsg->SeqNo          = pLinkState->dwExeSeqNo;
    ptSendMsg->LinkChannel    = Oam_LinkGetLinkId(pLinkState);
    memcpy(&ptSendMsg->LastData, &pLinkState->LastData, sizeof(pLinkState->LastData));
    memcpy(&ptSendMsg->BUF, &pLinkState->BUF, sizeof(pLinkState->BUF));
    ptSendMsg->bIfNo          = Oam_LinkIfCurCmdIsNo(pLinkState) ? TRUE: FALSE;
    ptSendMsg->OutputMode     = OUTPUT_MODE_REQUEST; /*OUTPUT_MODE_NORMAL;*/
    ptSendMsg->Number         = pMatchResult->wParaSum;
    ptSendMsg->CmdRestartFlag = TRUE;

    /*网管存盘链路*/
    if(pLinkState == &gt_SaveLinkState)
    {
        TYPE_XMLLINKSTATE *ptXmlLinkState = NULL;
        /*查找正在存盘的网管端*/
        for(wLoop = 0;wLoop < wOmmLinkNum;wLoop++)
        {
            ptXmlLinkState = &(gt_XmlLinkState[wLoop]);
            if ((pec_MsgHandle_EXECUTE == ptXmlLinkState->ucCurRunState) &&
    	          (IsSaveCmd(ptXmlLinkState->dwCmdId)))
            {            
                break;
            }
        }    
        ptSendMsg->LinkChannel =wLoop | 0x200;
    }
    /* fill the datas */
    wPacketSize = sizeof(MSG_COMM_OAM);
    if (pMatchResult->wParaSum)
    {
        wPacketSize -= sizeof(OPR_DATA);
        pCurPos = (BYTE *) ptSendMsg->Data;
        for (i = 0; i < pMatchResult->wParaSum; i++)
        {
            /* set packet size */
            wPacketSize += sizeof(OPR_DATA) - sizeof(BYTE) + pMatchResult->tPara[i].Len;  
            /* copy data */
            pData = (OPR_DATA *) pCurPos;
            pData->ParaNo = pMatchResult->tPara[i].ParaNo;
            pData->Type = pMatchResult->tPara[i].Type;
            pData->Len  = pMatchResult->tPara[i].Len;
            memcpy(pData->Data, pMatchResult->tPara[i].pValue, pData->Len);
            pCurPos = pCurPos + sizeof(OPR_DATA) - sizeof(BYTE) + pData->Len;
        }
    }
  
    /* return */
    ptSendMsg->DataLen  = wPacketSize;
    
    return ptSendMsg;
}

/**************************************************************************
*                          全局函数实现                                   *
**************************************************************************/

/**************************************************************************
* 函数名称：VOID ExecCeaseOmmCfgCmd
* 功能描述：取消网管配置命令
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_XMLLINKSTATE *pLinkState
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/04/18    V1.0    齐龙兆      创建
**************************************************************************/
void ExecCeaseOmmCfgCmd(TYPE_XMLLINKSTATE *pXmlLinkState)
{
    MSG_COMM_OAM *ptSendMsg = NULL;
    JID *ptDtJid = NULL;
    if ((NULL == pXmlLinkState) || 
	 (NULL == pXmlLinkState->ptLastSendMsg) ||
	 (NULL == pXmlLinkState->tCurCMDInfo.cfgDat))
    {
        XOS_SysLog(OAM_CFG_LOG_NOTICE,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
	 return;
    }
    ptSendMsg = pXmlLinkState->ptLastSendMsg;
    ptDtJid = &(pXmlLinkState->tCurCMDInfo.cfgDat->tJid);    
    ptSendMsg->CmdID = CMD_IP_TERMINAL_CMD;
    if (bDebugPrint)
    {
        PrintMSGCOMMOAMStruct(ptSendMsg, "MSG_CLIMANAGER_TO_APP");
    }
    /*重置定时器*/
    Xml_KillExeTimer(pXmlLinkState);
    Xml_SetExeTimer(pXmlLinkState);
    /*向平台应用发送终止命令*/
    XOS_SendAsynMsg(EV_OAM_CFG_CFG, (BYTE *)ptSendMsg, ptSendMsg->DataLen, XOS_MSG_VER0, XOS_MSG_MEDIUM,ptDtJid);
    return;
}
/**************************************************************************
* 函数名称：VOID ExecCeaseProtocolCmd
* 功能描述：取消协议栈配置命令
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
void ExecCeaseProtocolCmd(TYPE_LINK_STATE *pLinkState)
{
    MSG_COMM_OAM *ptSendMsg = NULL;
    TYPE_MATCH_RESULT *pMatchResult = &(pLinkState->tMatchResult);  
    T_OAM_Cfg_Dat* cfgDat = NULL;
    
    ptSendMsg = OamCfgConstructMsgData(pLinkState, NULL);
    if (!ptSendMsg)
    {            
        SendErrStringToClient(pLinkState, map_szCeaseCmdFailed);
    }
    else
    {
        cfgDat = pMatchResult->cfgDat;
        Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_EXECUTE);
        XOS_SendAsynMsg(EV_OAM_CEASE_TO_APP, (BYTE *)ptSendMsg, ptSendMsg->DataLen, XOS_MSG_VER0, XOS_MSG_MEDIUM, &(cfgDat->tJid));
    }

    return;
}

/**************************************************************************
* 函数名称：VOID ExecSendCmdAgain
* 功能描述：重发命令
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
void ExecSendCmdAgain(TYPE_LINK_STATE *pLinkState)
{
    MSG_COMM_OAM *ptRtnMsg = (MSG_COMM_OAM *)g_ptOamIntVar->aucRecvBuf;/*模式栈查询结果*/	
    DWORD dwMsgId = 0;	
    MSG_COMM_OAM *ptSendMsg = NULL;
    
    ptSendMsg = OamCfgConstructMsgData(pLinkState, NULL);
    if (!ptSendMsg)
    {            
        SendErrStringToClient(pLinkState, map_szCmdSendAgainFailed);
        return;
    }   
    ptSendMsg->CmdRestartFlag = FALSE;
    pLinkState->CmdRestartFlagForXmlRpc = FALSE;
    if(ptSendMsg->CmdID ==CMD_DEF_GET_SHOWUSERLOG )
    {
        dwMsgId=EV_OAM_CFG_CFG;
        Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_EXECUTE);		
        Oam_InnerApp((LPVOID)ptSendMsg,dwMsgId,ptRtnMsg); 
	/* InnerCmdShowUserLog(ptSendMsg,dwMsgId,ptRtnMsg);*/
        Oam_RecvFromCliApp(dwMsgId,(LPVOID)ptRtnMsg);	
	 return;
    }	
    SendPacketToExecute(pLinkState, ptSendMsg, pLinkState->tMatchResult.cfgDat->tJid);
    pLinkState->bRestartReqSended = TRUE;
    
    /*重置Execute端定时器*/
    Oam_KillExeTimer(pLinkState);
    Oam_SetExeTimer(pLinkState);

    Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_EXECUTE);
    
    return;
}

/**************************************************************************
* 函数名称：VOID DisposeMPCmd
* 功能描述：处理MP命令
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
void DisposeMPCmd(TYPE_LINK_STATE *pLinkState)
{
    CHAR *ptrOutputResult = NULL;
    MSG_COMM_OAM *ptSendMsg = (MSG_COMM_OAM *)g_ptOamIntVar->aucSendBuf;
    
    ptSendMsg = OamCfgConstructMsgData(pLinkState, NULL);
    if (!ptSendMsg)
    {            
        SendErrStringToClient(pLinkState, map_szDisposeCmdFailed);
        return;
    }
    
    /*多个客户端不能同时执行前台命令*/
    if (CheckMpExecuteMutex(pLinkState, ptSendMsg))
    {
        if (!Xml_CfgCurLinkIsExcutingSave(pLinkState))/*当前不是网管在存盘*/
        {
            /*检查是否有链路正在存盘*/
            if (CheckLinkIsExecutingSaveCmd() && (S_CfgSave == XOS_GetCurState(PROCTYPE_OAM_CLIMANAGER)))
            {
                SendStringToTelnet(pLinkState, Hdr_szSaveMutexTip);
                return;
            }
            /*如果是存盘命令检查其他链路是否在执行命令*/
	     else if (IsSaveCmd(ptSendMsg->CmdID) && CheckLinkIsExecutingCmd())
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
            ptSendMsg->ucOamTag = OAMCFG_TAG_OFFLINE_TO_ONLINE;
        }
        else 
        {
            ptSendMsg->ucOamTag = OAMCFG_TAG_NORMAL;        		
        }

        if (S_Restore == XOS_GetCurState(PROCTYPE_OAM_CLIMANAGER))
        {
            ptSendMsg->ucOamTag = OAMCFG_TAG_TXT_RESTORE;
        }

        pLinkState->ucPlanValuePrev = 0;
        SendPacketToExecute(pLinkState, ptSendMsg, pLinkState->tMatchResult.cfgDat->tJid);
    }
    else
    {
        /*提示用户有其他用户正在执行命令，稍后再试*/
        SendStringToTelnet(pLinkState, Hdr_szCommandMutexTip);
        return;
    }

    return; 
}

/**************************************************************************
* 函数名称：BOOL CheckMpExecuteMutex
* 功能描述：检查多客户端间在线配置命令执行互斥情况
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_LINK_STATE *pLinkState
* 输出参数：TRUE - 没有其他客户端在执行
*                             FALSE - 有其他客户端正在执行
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2008/03/12    V1.0    殷浩      创建
**************************************************************************/
BOOL CheckMpExecuteMutex(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *ptMsg)
{
    WORD32 dwCmdType = ptMsg->CmdID & 0x00FF0000;
    WORD32 dwTmpCmdType = 0;
    int i = 0;

    /*DDM 命令不做限制*/
    WORD32 dwCmdGrp = ptMsg->CmdID & 0xFF000000;
    if (dwCmdGrp == OAM_CLI_DDM_CMDGRP_SET ||
        dwCmdGrp == OAM_CLI_DDM_CMDGRP_GET ||
        dwCmdGrp == OAM_CLI_CMM_CMDGRP ||
        dwCmdGrp == OAM_CLI_DDM_CMDGRP_GETTABLE)
    {
        return TRUE;
    }
	
    /*同一种类型的配置命令只能有一个用户再执行*/
    for (i = 0; i < MAX_CLI_VTY_NUMBER; i++)
    {
        if ((pec_MsgHandle_EXECUTE == Oam_LinkGetCurRunState(&gtLinkState[i])) &&
                (Oam_LinkGetLinkId(pLinkState) != Oam_LinkGetLinkId(&gtLinkState[i])))
        {
            dwTmpCmdType = (gtLinkState[i].tMatchResult.dExeID) & 0x00FF0000;
            if (dwTmpCmdType == dwCmdType)
        {
            return FALSE;
        }
    }
    }
	
    return TRUE;
}


