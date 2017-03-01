/**************************************************************************
* 版权所有 (C)2002, 深圳市中兴通讯股份有限公司。
*
* 文件名称：offline_to_online.c
* 文件标识：
* 内容摘要：处理brs离线转在线的配置命令请求(V05.03.80.2_3GPF_TDRNC_003)
* 其它说明：
            
* 当前版本：
* 作    者       ：殷浩
* 完成日期：20090116
*
* 修改记录1 ：// 修改历史记录，包括修改日期、修改者及修改内容
*    修改日期：
*    版 本 号    ：
*    修 改 人    ：
*    修改内容 :
* 修改记录2：
**************************************************************************/
/**************************************************************************
*                           头文件                                        *
**************************************************************************/
#include "includes.h"
#include "offline_to_online.h"
#include "saveprtclcfg.h"
#include "hrtelmsg.h"
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
/*记录是否是一批转换操作的第一个命令*/
static BOOLEAN g_bIsFirstCmd = TRUE;
static JID g_tProtocolPid;
static BYTE g_aucLastData[MAX_STORE_LEN]; 
TYPE_LINK_STATE g_tLinkStateForOfflineToOnline;

/**************************************************************************
*                          本地变量                                       *
**************************************************************************/

/**************************************************************************
*                          全局函数原型                                   *
**************************************************************************/
extern BOOLEAN Oam_CfgGlobalPointersIsOk(void);

/**************************************************************************
*                          局部函数原型                                   *
**************************************************************************/
static BOOLEAN FirstCharIsSpecialCmd(CHAR *strCmdLine);
static BOOLEAN GetCmdLine(CHAR *pcCmdLineOut, WORD16 wMaxCmdLen, LPVOID pMsgPara);
static void ExecuteSpecialCmd(TYPE_LINK_STATE *pLinkState, CHAR *strCmdLine);
static void ExecuteNormalCmd(TYPE_LINK_STATE *pLinkState, CHAR *strCmdLine);
static void ReturnParseErrToBrs(TYPE_LINK_STATE *pLinkState);

/**************************************************************************
*                         函数定义                                               *
**************************************************************************/

/**************************************************************************
* 函数名称：Oam_CfgHandleMSGOffline2Online
* 功能描述：处理brs发来的离线转在线命令
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_LINK_STATE *pLinkState --全局变量
                              LPVOID pMsgPara -- brs发来的数据
* 输出参数：
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/02/01   V1.0    殷浩      创建
**************************************************************************/
void Oam_CfgHandleMSGOffline2Online(TYPE_LINK_STATE *pLinkState, LPVOID pMsgPara)
{
    CHAR aucCmdLineBuf[OAM_MAX_OFFLINE_CMD_LEN  + 1] = {0};
    CHAR *pcCmdlineTmp = NULL;
    T_OfflineCfgCmd *ptMsgData = (T_OfflineCfgCmd *)pMsgPara;
    WORD16 wMsgLenTmp = 0;    

    if (!ptMsgData)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                   "Oam_cfgHandleMSGOffline2Online: input parameter error!\n");
        return;
    }

    if (!Oam_CfgCheckRecvMsgMinLen(sizeof(T_OfflineCfgCmd)))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "Oam_CfgHandleMSGOffline2Online:The length of message received is less than the length of interface struct! ");
        return;
    }      

    /*获取专用linkstate*/
    pLinkState = OamCfgGetLinkState(MSG_OFFLINE_CFG_TO_INTERPRET, pMsgPara);
    if (!pLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                   "Oam_cfgHandleMSGOffline2Online: Warnning GetLinkState Fail!\n");
        return;
    }

    /*CRDCM00238058 对消息长度进行检查，防止由于不兼容版本出现问题*/
    XOS_GetMsgDataLen(PROCTYPE_OAM_CLIMANAGER, &wMsgLenTmp);
    if (wMsgLenTmp != sizeof(T_OfflineCfgCmd))
    {
        CHAR *pcMsg = "Oam_cfgHandleMSGOffline2Online: MsgLen error, please make sure version of RPU and OAMMANAGER is compatible!\n";
        XOS_SysLog(OAM_CFG_LOG_ERROR, "%s\n", pcMsg);
        ReturnParseErrToBrs(pLinkState);
        return;        	
    }
    
    /*保存离线转在线的brspid*/
    XOS_Sender(&g_tProtocolPid);
    
    /*保存消息中的lastdata,命令执行完发送MSG_OFFLINE_CFG_TO_INTERPRET时候带给brs*/
    memset(g_aucLastData, 0, sizeof(g_aucLastData));
    memcpy(g_aucLastData, ptMsgData->LastData, sizeof(g_aucLastData));

    /*检查dat文件是否可用*/
    if (!Oam_CfgGlobalPointersIsOk())
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                   "Oam_cfgHandleMSGOffline2Online: Some global pointer is NULL! can`t execute message [%d]\n"
                   "check global pointers listed in function GlobalPtrIsOk()", MSG_OFFLINE_CFG_TO_INTERPRET);
        ReturnParseErrToBrs(pLinkState);

        return;
    }

    /*如果是一批命令中的第一条，切换到协议配置模式*/
    if (g_bIsFirstCmd)
    {
        Oam_LinkInitializeForNewConn(pLinkState);

        pLinkState->ucLinkID =  MAX_CLI_VTY_NUMBER + 1;
        
        /*切换模式到协议栈配置模式*/
        Oam_LinkModeStack_Init(pLinkState);
        Oam_LinkModeStack_Push(pLinkState, CMD_MODE_PROTOCOL);

        g_bIsFirstCmd = FALSE;

        g_ptOamIntVar->bSupportStringWithBlank = TRUE;
    }

    /*获取命令字符串*/
    if (!GetCmdLine(aucCmdLineBuf, sizeof(aucCmdLineBuf), pMsgPara))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                   "Oam_cfgHandleMSGOffline2Online: Warnning GetCmdLine Fail!\n");
        /*??如果获取不到brs发来的命令字符串，通知brs执行下一条*/
        Oam_CfgGetNextCmdFromBrs();
        return;
    }
    
    XOS_SysLog(OAM_CFG_LOG_NOTICE,
               "Oam_cfgHandleMSGOffline2Online: Execute cmd [%s]!\n", aucCmdLineBuf);

    pcCmdlineTmp = aucCmdLineBuf;
    
    /*执行命令*/
    /*
    命令字符串有两种:
    1. 开头是一个特殊命令的两个命令组合，比如"!\ninterface 0 1 2 1 1"
    2. 单独的一条普通，比如 "interface 0 1 5 1 1 "
    */
    if (FirstCharIsSpecialCmd(aucCmdLineBuf))
    {/*首字母是特殊命令*/
        ExecuteSpecialCmd(pLinkState, aucCmdLineBuf);

        /*如果是包含一条特殊命令的字符串，跳过特殊部分取下一条*/
        while(*pcCmdlineTmp == '\n'  || *pcCmdlineTmp == '\r' || FirstCharIsSpecialCmd(pcCmdlineTmp))
        {
            pcCmdlineTmp++;
            if (*pcCmdlineTmp == 0)
            {
                break;
            }
        }
    }

    /*特殊命令检查执行以后，看是否有普通命令*/
    if (strlen(pcCmdlineTmp) > 0)
    {/*普通命令,要等brs返回执行结果再请求下一条命令*/
        ExecuteNormalCmd(pLinkState, pcCmdlineTmp);
    }
    else
    {
        /* ??如果只有一条特殊命令 通知brs执行下一条*/
        Oam_CfgGetNextCmdFromBrs();
    }
}

/**************************************************************************
* 函数名称：Oam_CfgHandleMSGOffline2OnlineFinish
* 功能描述：处理brs发来的离线转在线处理结束消息
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：null- 错误
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/02/01   V1.0    殷浩      创建
**************************************************************************/
void Oam_CfgHandleMSGOffline2OnlineFinish(void)
{
    g_bIsFirstCmd = TRUE;
}

/**************************************************************************
* 函数名称：FirstCharIsSpecialCmd
* 功能描述：判断输入命令第一个字符是否是特殊命令
* 访问的表：无
* 修改的表：无
* 输入参数：CHAR *strCmdLine -- 命令行字符串
* 输出参数：
* 返 回 值：   TRUE- 有
                              FALSE- 无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/02/01   V1.0    殷浩      创建
**************************************************************************/
static BOOLEAN FirstCharIsSpecialCmd(CHAR *strCmdLine)
{
    BOOLEAN bRet = FALSE;

    if (!strCmdLine)
    {
        bRet = FALSE;
    }
    else if (('!' == strCmdLine[0]) ||
                ('$' == strCmdLine[0]))
    {
        bRet = TRUE;
    }

    return bRet;
}

/**************************************************************************
* 函数名称：GetCmdLine
* 功能描述：从brs消息中获取命令行字符串，做trim处理
* 访问的表：无
* 修改的表：无
* 输入参数：WORD16 wMaxCmdLen --输出字符串最大长度
                              LPVOID pMsgPara -- BRS发来的消息体
* 输出参数：CHAR *pcCmdLineOut -- 命令字符串
* 返 回 值：   TRUE- 获取成功
                              FALSE- 获取失败
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/02/01   V1.0    殷浩      创建
**************************************************************************/
static BOOLEAN GetCmdLine(CHAR *pcCmdLineOut, WORD16 wMaxCmdLen, LPVOID pMsgPara)
{
    T_OfflineCfgCmd *ptMsgData = (T_OfflineCfgCmd *)pMsgPara;
    CHAR *pcTmp = NULL;
    WORD16 wStrLenTmp = 0;

    if (!pMsgPara)
    {
        return FALSE;
    }
    
    pcTmp = (CHAR *)ptMsgData->CmdBuffer;
    pcTmp = OAM_LTrim(OAM_RTrim(pcTmp));

    if (!pcCmdLineOut || !pcTmp)
    {
        return FALSE;
    }

    wStrLenTmp = strlen(pcTmp) + 1;
    wStrLenTmp = OAM_CFG_MIN(wStrLenTmp, wMaxCmdLen);
    strncpy(pcCmdLineOut, pcTmp, wMaxCmdLen);

    return TRUE;
}

/**************************************************************************
* 函数名称：ExecuteSpecialCmd
* 功能描述：执行不需要解释的特殊命令
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_LINK_STATE *pLinkState -- 全局变量
                              CHAR *strCmdLine -- 命令行字符串
* 输出参数：无
* 返 回 值：   无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/02/01   V1.0    殷浩      创建
**************************************************************************/
static void ExecuteSpecialCmd(TYPE_LINK_STATE *pLinkState, CHAR *strCmdLine)
{
    if (!strCmdLine)
    {
        return;
    }

    if (strlen(strCmdLine) == 0)
    {

    }
    else if ('!' == strCmdLine[0])
    {
        /*存盘文件遇到!，回到协议配置模式*/
        while (pLinkState->bModeStackTop > 1)
        {
            if (pLinkState->bModeID[pLinkState->bModeStackTop - 1] == CMD_MODE_CONFIG)
            {
                break;
            }

            pLinkState->bModeID[pLinkState->bModeStackTop] = CMD_MODE_INVALID;
            pLinkState->bModeStackTop--;
        }

        memset(&pLinkState->BUF, 0, sizeof(pLinkState->BUF));
        memset(&pLinkState->LastData, 0, sizeof(pLinkState->LastData));
    }
    else if('$' == strCmdLine[0])
    {
 #if 0       
 /*从恢复程序看没有$ 符号的处理*/
       if((pLinkState->bModeStackTop > 1)&&((pLinkState->bModeID[pLinkState->bModeStackTop - 1]==30)||(pLinkState->bModeID[pLinkState->bModeStackTop - 1]==31)))
        {
            if((pLinkState->bModeStackTop>2)&&((pLinkState->bModeID[pLinkState->bModeStackTop - 2])==CMD_MODE_BGP))
            {
                /*根据恢复来写的，不知道干嘛的*/
                BYTE bTmpLastData=2;

                pLinkState->ucBureauNo = 0;
                pLinkState->wInterfaceNo = 0;
                memset(&pLinkState->tPortLocation, 0, sizeof(pLinkState->tPortLocation));
                memset(&pLinkState->LastData, 0, sizeof(pLinkState->LastData));
                memcpy(&pLinkState->LastData,(BYTE*)&bTmpLastData,sizeof(BYTE));
            }
        }
        while (pLinkState->bModeStackTop > 1)
        {
            if (pLinkState->bModeID[pLinkState->bModeStackTop - 1] == CMD_MODE_BGP)
            {
                break;
            }
            pLinkState->bModeID[pLinkState->bModeStackTop] = CMD_MODE_INVALID;
            pLinkState->bModeStackTop--;
        } 
#endif
    }

}

/**************************************************************************
* 函数名称：ExecuteNormalCmd
* 功能描述：执行需要解释的普通命令
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_LINK_STATE *pLinkState -- 全局变量
                              CHAR *strCmdLine -- 命令行字符串
* 输出参数：无
* 返 回 值：   无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/02/01   V1.0    殷浩      创建
**************************************************************************/
static void ExecuteNormalCmd(TYPE_LINK_STATE *pLinkState, CHAR *strCmdLine)
{
    TYPE_MATCH_RESULT *pMatchResult  = NULL;

    if (!strCmdLine)
    {
        return;
    }
    pMatchResult  = &(pLinkState->tMatchResult);
    
    Oam_InptFreeBuf(pLinkState->ucLinkID);
    pLinkState->dwExeSeqNo++;

    ParseCmdLine(pLinkState, strCmdLine, DM_NORMAL, TRUE);

    /*?? 如果解析命令行失败，打印一下，给出告警*/
    if (pMatchResult->wParseState != PARSESTATE_FINISH_PARSE)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                   "ExecuteNormalCmd: %s \n[Parse error, check whether this command match with dat file!]\n",
                strCmdLine);
        /*?? 需要给brs回一个解析失败通知*/
        ReturnParseErrToBrs(pLinkState);
        return;
    }

}

/**************************************************************************
* 函数名称：Oam_CfgGetNextCmdFromBrs()
* 功能描述：通知brs执行下一条命令
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：   无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/02/01   V1.0    殷浩      创建
**************************************************************************/
void Oam_CfgGetNextCmdFromBrs(void)
{
    MSG_COMM_OAM tRetMsg;

    /*用这个结构返回lastdata给brs*/
    memset(&tRetMsg, 0, sizeof(tRetMsg));
    memcpy(tRetMsg.LastData, g_aucLastData, sizeof(tRetMsg.LastData));
    
//    OSS_SendAsynMsg(MSG_OFFLINE_CFG_TO_INTERPRET, (BYTE *)&tRetMsg, sizeof(tRetMsg), COMM_RELIABLE,&g_tProtocolPid);
}

/**************************************************************************
* 函数名称：Oam_CfgCurLinkIsOffline2Online
* 功能描述：当前连接是否是处理离线转在线
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：   TRUE - 是
                              FALSE - 否
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/02/01   V1.0    殷浩      创建
**************************************************************************/
BOOLEAN Oam_CfgCurLinkIsOffline2Online(TYPE_LINK_STATE *pLinkState)
{
    return (pLinkState == &g_tLinkStateForOfflineToOnline);
}

/**************************************************************************
* 函数名称：Oam_CfgGetBrsPidForOffline2Online
* 功能描述：获取离线转在线的brs pid
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：   TRUE - 是
                              FALSE - 否
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/02/01   V1.0    殷浩      创建
**************************************************************************/
void Oam_CfgGetBrsPidForOffline2Online(JID *ptPid)
{
    if (!ptPid)
    {
        return;
    }

    memset(ptPid, 0, sizeof(JID));
    memcpy(ptPid, &g_tProtocolPid, sizeof(JID));
}

/**************************************************************************
* 函数名称：Oam_CfgSetLastDataForOffline2Online
* 功能描述：设置离线转在线消息的lastdata
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：   TRUE - 是
                              FALSE - 否
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/02/01   V1.0    殷浩      创建
**************************************************************************/
void Oam_CfgSetLastDataForOffline2Online(MSG_COMM_OAM *ptMsg)
{
    if (!ptMsg)
    {
        return;
    }

    memcpy(ptMsg->LastData, g_aucLastData, sizeof(ptMsg->LastData));
}

/**************************************************************************
* 函数名称：Oam_CfgCurCmdIsOffline2Online
* 功能描述：当前命令是否是处理离线转在线
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：   TRUE - 是
                              FALSE - 否
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/02/01   V1.0    殷浩      创建
**************************************************************************/
BOOLEAN Oam_CfgCurCmdIsOffline2Online(MSG_COMM_OAM *ptMsg)
{
    if (!ptMsg)
    {
        return FALSE;
    }

    return (OAMCFG_TAG_OFFLINE_TO_ONLINE == ptMsg->ucOamTag);
}

/**************************************************************************
* 函数名称：ReturnParseErrToBrs
* 功能描述：回解析失败通知给brs
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：   TRUE - 是
                              FALSE - 否
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/02/01   V1.0    殷浩      创建
**************************************************************************/
void ReturnParseErrToBrs(TYPE_LINK_STATE *pLinkState)
{
    MSG_COMM_OAM *ptSendMsg = (MSG_COMM_OAM *)g_ptOamIntVar->aucSendBuf;
    JID tJidBrs;
   
    if (!pLinkState)
    {
         XOS_SysLog(OAM_CFG_LOG_ERROR,
                   "ReturnParseErrToBrs: pLinkState is NULL, send parse error to brs failed!\n");
        return;
    }
    /* calculate the new sequence number */
    pLinkState->dwExeSeqNo++;
    
    /* 构造解析失败的消息*/
    ptSendMsg->ReturnCode     = RETURNCODE_INVALID;
    ptSendMsg->SeqNo             = pLinkState->dwExeSeqNo;
    ptSendMsg->LinkChannel     = pLinkState->ucLinkID;
    memcpy(&ptSendMsg->BUF, &pLinkState->BUF, sizeof(pLinkState->BUF));
    memcpy(&ptSendMsg->LastData, &pLinkState->LastData, sizeof(pLinkState->LastData));
    ptSendMsg->CmdID          = CMD_ID_OFFLINE_PARSEFAIL;
    ptSendMsg->bIfNo          = Oam_LinkIfCurCmdIsNo(pLinkState) ? TRUE: FALSE;
    ptSendMsg->OutputMode     = OUTPUT_MODE_NORMAL;
    ptSendMsg->Number         = 0;
    ptSendMsg->CmdRestartFlag = TRUE;

    ptSendMsg->ucOamTag = OAMCFG_TAG_OFFLINE_TO_ONLINE;

    ptSendMsg->DataLen = sizeof(MSG_COMM_OAM);

    /*重置Execute端定时器*/
    Oam_KillExeTimer(pLinkState);
    Oam_SetExeTimer(pLinkState);

    Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_EXECUTE);

    /*填充离线转在线的专用lastdata*/
    Oam_CfgSetLastDataForOffline2Online(ptSendMsg);
    
    Oam_CfgGetBrsPidForOffline2Online(&tJidBrs);
    
    SendPacketToExecute(pLinkState, ptSendMsg, tJidBrs);
}


