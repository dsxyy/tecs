/**************************************************************************
* 版权所有 (C)2002, 深圳市中兴通讯股份有限公司。
*
* 文件名称：Interpret.c
* 文件标识：
* 内容摘要：OAM解释进程入口
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
#include "saveprtclcfg.h"
#include "oam_execute.h"
#include "clirecvreg.h"
#include "offline_to_online.h"

/**************************************************************************
*                          常量                                           *
**************************************************************************/
/**************************************************************************
*                          宏                                             *
**************************************************************************/
#define OAM_TO_4BYTE_EDGE(X) (((int)(X) & ~3) + 4)  /*对齐到四字节边缘*/
/**************************************************************************
*                          数据类型                                       *
**************************************************************************/
/**************************************************************************
*                          全局变量                                       *
**************************************************************************/
T_DAT_LINK *g_tDatLink = NULL;
WORD16    g_wRegDatCount;

/*连接信息*/
TYPE_LINK_STATE gtLinkState[MAX_CLI_VTY_NUMBER - MIN_CLI_VTY_NUMBER + 1] = {{0}};
TYPE_LINK_STATE gt_SaveLinkState = {0};/*存盘命令链路信息*/
TYPE_XMLLINKSTATE  gt_XmlLinkState[MAX_SIZE_LINKSTATE] = {{0}};

/*配置脚本*/
COMMON_INFO gtCommonInfo = {{0}};
USER_NODE *pUserLinkHead = NULL;
USER_NODE *pUserLinkTail = NULL;

T_OAM_INTERPRET_VAR *g_ptOamIntVar = NULL;

CHAR g_szSpecifyIP[LEN_IP_ADDR_STR] = {0};
BYTE   gnPasswordIntensity;     /*是否为强密码*/
BYTE   gnNorPasslesslen;          /*设置弱密码的最小程度*/
CHAR  g_aucScriptLanguage[OAM_CLI_LANGTYPE_LEN] = {0}; /* 语言类型字符串*/
DWORD gnUserNum = 0;
TYPE_USER_INFO gtUserInfo[MAX_USER_NUM_ROLE_ADMIN + MAX_USER_NUM_ROLE_CONFIG + MAX_USER_NUM_ROLE_VIEW] = {{0}};

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
*                          全局函数声明                                   *
**************************************************************************/

BOOL EqualJID(const JID* jid1, const JID* jid2)
{
    if ((jid1 == NULL) || (jid2 == NULL))
        return FALSE;

    if ((jid1->dwJno        == jid2->dwJno       ) &&
        (jid1->dwDevId      == jid2->dwDevId     ) &&
        (jid1->wModule      == jid2->wModule     ) &&
        (jid1->wUnit        == jid2->wUnit       ) &&
        (jid1->ucSUnit      == jid2->ucSUnit     ) &&
        (jid1->ucSubSystem  == jid2->ucSubSystem ) &&
        (jid1->ucRouteType  == jid2->ucRouteType ) &&
        (jid1->ucExtendFlag == jid2->ucExtendFlag))
        return TRUE;
    else
        return FALSE;
}

/**************************************************************************
* 函数名称：BOOL CheckIsXMLAdapterLinkState()
* 功能描述：检查当前链路是否XML适配器专用链路
* 访问的表：无
* 修改的表：无
* 输入参数：*ptLinkState:链路信息
* 输出参数：
* 返 回 值：BOOL
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/04/26    V1.0    齐龙兆    创建
**************************************************************************/
BOOL CheckIsXMLAdapterLinkState(TYPE_LINK_STATE *ptLinkState)
{
    if(ptLinkState == &gtLinkState[MAX_CLI_VTY_NUMBER])
    {
        return TRUE;
    }
    return FALSE;
}

/**************************************************************************
* 函数名称：VOID XmlShowLinkState()
* 功能描述：调试函数:察看当前和OMM的连接情况
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/04/26    V1.0    齐龙兆    创建
**************************************************************************/
void XmlShowLinkState(void)
{
    WORD16 vty = 0;
    TYPE_XMLLINKSTATE *pLinkState = NULL;
    CHAR aucState[25] = {0};    

    printf("LINKID    STATE\n");
    printf("---------------\n");
    for (vty = 0; vty < MAX_SIZE_LINKSTATE; vty++)
    {
        pLinkState = &gt_XmlLinkState[vty];
        memset(aucState, 0, sizeof(aucState));        
        switch(XML_LinkGetCurRunState(pLinkState))
        {
        case pec_MsgHandle_IDLE: /* 空状态 */
            strncpy(aucState, "Idle", sizeof(aucState));
            break;        
        case pec_MsgHandle_EXECUTE: /* Execute端执行状态 */
            strncpy(aucState, "Work", sizeof(aucState));
            break;
        default:
            strncpy(aucState, "Unknown", sizeof(aucState));
            break;
        }
        printf("%-10d%-9s\n",
               vty,
               aucState);
    }
}


/*****************************************************************************/
/**
@brief 显示CLIMANAGER已连接的连接信息，用于定位连接异常情况
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
\n
LINKID    STATE    USERNAME
==============================
0             Idle
1             Idle
2             Idle
3             Idle
4             Work     root
5             Idle
6             Idle
@li @b 修改记录：
-#  ： 
*/
/*****************************************************************************/
void OAM_DbgShowCliAllLink(void)
{
    BYTE vty = MIN_CLI_VTY_NUMBER;
    TYPE_LINK_STATE *pLinkState = NULL;
    CHAR aucState[25] = {0};
    CHAR aucUserName[MAX_USERNAME_LEN] = {0};

    printf("[Linkstate information table]\n");
    printf("LINKID,    STATE,    USERNAME\n");
    printf("==============================\n");
    for (vty = MIN_CLI_VTY_NUMBER; vty <= MAX_CLI_VTY_NUMBER; vty++)
    {
        pLinkState = &gtLinkState[vty-MIN_CLI_VTY_NUMBER];
        memset(aucState, 0, sizeof(aucState));
        memset(aucUserName, 0, sizeof(aucUserName));
        switch(Oam_LinkGetCurRunState(pLinkState))
        {
        case pec_MsgHandle_IDLE: /* 空状态 */
            strncpy(aucState, "Idle", sizeof(aucState));
            break;
        case pec_Init_USERNAME:  /* 用户名输入及验证状态 */
        case pec_Init_PASSWORD: /* 密码输入及鉴权状态 */
            strncpy(aucState, "Logging", sizeof(aucState));
            break;
        case pec_MsgHandle_INIT:
            strncpy(aucState, "Initialize", sizeof(aucState));
            break;
        case pec_MsgHandle_TELNET: /* Telnet端执行状态 */
        case pec_MsgHandle_EXECUTE: /* Execute端执行状态 */
            strncpy(aucState, "Work", sizeof(aucState));
            break;
        default:
            strncpy(aucState, "Unknown", sizeof(aucState));
            break;
        }

        if(Oam_LinkGetCurRunState(pLinkState) != pec_MsgHandle_IDLE)
        {
            strncpy(aucUserName, pLinkState->sUserName, sizeof(aucUserName));
        }
        else
        {
            strncpy(aucUserName, "-", sizeof("-"));
        }

        printf("%6d,%9s,%12s\n",
               vty-MIN_CLI_VTY_NUMBER,
               aucState,
               aucUserName);
    }
    printf("==============================\n");
}

/**************************************************************************
* 函数名称：VOID OamInterpretCheckPtr
* 功能描述：检查函数指针型入参
* 访问的表：无
* 修改的表：无
* 输入参数：
            BYTE *InputPtr
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
void OamInterpretCheckPtr(BYTE *InputPtr, WORD32 dwLine)
{
    assert(InputPtr != NULL);
    if(NULL == InputPtr)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "OamInterpret:OamInterpretCheckPtr(): NULL ptr; line=%d!", dwLine);

        /* 如果入参为空，直接退出 */
        XOS_SetDefaultNextState();
    }
}

/**************************************************************************
* 函数名称：Xml_CfgCurLinkIsExcutingSave
* 功能描述：当前连接是否在存盘
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：   TRUE - 是
                              FALSE - 否
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/05/05   V1.0    齐龙兆      创建
**************************************************************************/
BOOLEAN Xml_CfgCurLinkIsExcutingSave(TYPE_LINK_STATE *pLinkState)
{
    return (pLinkState == &gt_SaveLinkState);
}


INT OamCfgGetLinkStateType(LPVOID pMsgPara)
{
    MSG_COMM_OAM *pOamMsgBufTmp = NULL;
    INT iType = 0;
    if (NULL == pMsgPara)
    {
        return LINKSTATE_TYPE_ERR;
    }
    pOamMsgBufTmp = (MSG_COMM_OAM *)pMsgPara;
    /*判断BIT31标志位*/
    iType = ((pOamMsgBufTmp->LinkChannel)&0X00000200)>>9; /*BIT9 1-表示XML应答0-CLI应答*/   
    if (iType == LINKSTATE_TYPE_CLI)
    {
        return LINKSTATE_TYPE_CLI;
    }    
    return LINKSTATE_TYPE_XML;    
}

INT OamGetLinkStateTypeForPlan(LPVOID pMsgPara)
{
    T_Cmd_Plan_Cfg *pPlanMsgBuf = NULL;
    INT iType = 0;
    if (NULL == pMsgPara)
    {
        return LINKSTATE_TYPE_ERR;
    }
    pPlanMsgBuf = (T_Cmd_Plan_Cfg *)pMsgPara;
    /*判断BIT31标志位*/
    iType = ((pPlanMsgBuf->dwLinkChannel)&0X00000200)>>9; /*BIT9 1-表示XML应答0-CLI应答*/   
    if (iType == LINKSTATE_TYPE_CLI)
    {
        return LINKSTATE_TYPE_CLI;
    }    
    return LINKSTATE_TYPE_XML;    
}

/**************************************************************************
* 函数名称：TYPE_XMLLINKSTATE *XmlCfgGetLinkState
* 功能描述：获取XML连接
* 访问的表：无
* 修改的表：无
* 输入参数：无
            WORD16 dwMsgId, LPVOID pMsgPara
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/05/05    V1.0    齐龙兆      创建
**************************************************************************/
TYPE_XMLLINKSTATE *XmlCfgGetLinkState(WORD16 dwMsgId, LPVOID pMsgPara)
{
    WORD16 wLinkChannel = 0;
    TYPE_XMLLINKSTATE *ptLinkState = NULL;
    switch(dwMsgId)
    {
        case EV_TIMER_EXECUTE_XMLADAPTER:
    case EV_TIMER_CMD_PLAN_XML:
            wLinkChannel = XOS_GetConstParaOfCurTimer(pMsgPara);
            ptLinkState = Xml_GetLinkStateByLinkChannel(wLinkChannel);
            break;
	 default:
	     break;
    }
    return ptLinkState;    
}

/**************************************************************************
* 函数名称：static TYPE_LINK_STATE *OamCfgGetLinkState
* 功能描述：获取连接
* 访问的表：无
* 修改的表：无
* 输入参数：
            WORD16 dwMsgId, LPVOID pMsgPara
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
TYPE_LINK_STATE *OamCfgGetLinkState(WORD32 dwMsgId, LPVOID pMsgPara)
{
    BYTE vty = MIN_CLI_VTY_NUMBER;
    TYPE_LINK_STATE *pSaveLinkState = NULL;/*模拟Telnet存盘链路*/

    switch(dwMsgId)
    {
    case OAM_MSG_TELNET_TO_INTERPRET:
        vty = ((TYPE_TELNET_MSGHEAD *)pMsgPara)->bVtyNum;
        break;
    case OAM_MSG_XMLAGNET_TO_INTERPRET:
	 pSaveLinkState =  &gt_SaveLinkState;
	 break;	
    case OAM_MSG_EXECUTION_TO_INTERPRET :
    case EV_OAM_CFG_CFG:
    case EV_OAM_CLI_SCRIPT_LANGUAGE_CHANGED:
        if (Xml_CheckIsExecSaving())/*OMM正在执行存盘命令*/	 
        {
            pSaveLinkState = &gt_SaveLinkState;            
        }
        else if (Oam_CfgCurCmdIsOffline2Online((MSG_COMM_OAM *)pMsgPara))
        {
            /* 专用的linkstate 用于离线转在线处理*/
            return &g_tLinkStateForOfflineToOnline;
        }
        else
        {
            vty = ((MSG_COMM_OAM *)pMsgPara)->LinkChannel;
	 }
        break;
    case EV_OAM_CFG_PLAN:
        if (Xml_CheckIsExecSaving())/*OMM正在执行存盘命令*/     
        {
            pSaveLinkState = &gt_SaveLinkState;            
        }
        else
        {
            if (((T_Cmd_Plan_Cfg *)pMsgPara)->dwCmdID == CMD_DEF_SET_SAVE)
            {
                for(vty = MIN_CLI_VTY_NUMBER; vty <=MAX_CLI_VTY_NUMBER; vty++)
                {
                    if (Oam_LinkGetCurRunState(&gtLinkState[vty-MIN_CLI_VTY_NUMBER]) != pec_MsgHandle_EXECUTE)
                    {
                        continue;
                    }
                    if (CMD_DEF_SET_SAVE == gtLinkState[vty-MIN_CLI_VTY_NUMBER].tMatchResult.dExeID)
                    {
                        break;
                    }
                }
                
            }
            else
                vty = ((T_Cmd_Plan_Cfg *)pMsgPara)->dwLinkChannel;
        }
        break;
    case EV_SAVEOK:
    case EV_SAVEFAIL:
        /*dbs接口中没有vtyid*/
        {
            WORD32 dwCmdId = 0;
            if (Xml_CheckIsExecSaving())/*OMM正在执行存盘命令*/	 
            {
                pSaveLinkState = &gt_SaveLinkState;            
            }
            else
            {
                for(vty = MIN_CLI_VTY_NUMBER; vty <=MAX_CLI_VTY_NUMBER; vty++)
                {
                    if (Oam_LinkGetCurRunState(&gtLinkState[vty-MIN_CLI_VTY_NUMBER]) != pec_MsgHandle_EXECUTE)
                    {
                        continue;
                    }
                    dwCmdId = gtLinkState[vty-MIN_CLI_VTY_NUMBER].tMatchResult.dExeID;
                    if (((dwCmdId & 0xFF000000) >> 24) == OAM_CFG_CMDGRP_DEFEND )
                    {
                        break;
                    }
                }
            }
        }
        break;
    case EV_TIMER_IDLE_TIMEOUT:
    /* 614000678048 (去掉24小时绝对超时) */
    /*case EV_TIMER_ABS_TIMEOUT:*/
    case EV_TIMER_CMD_PLAN:
        vty = XOS_GetConstParaOfCurTimer(pMsgPara);
        break;
    case EV_TIMER_EXECUTE_NORMAL:
        if (Xml_CheckIsExecSaving())/*OMM正在执行存盘命令*/	 
        {
            pSaveLinkState = &gt_SaveLinkState;            
        }
        else
        {
            vty = XOS_GetConstParaOfCurTimer(pMsgPara);
	 }
        break;
    case OAM_MSG_SAVE_BEGIN:
	 if (Xml_CheckIsExecSaving())/*OMM正在执行存盘命令*/	 
        {
            pSaveLinkState = &gt_SaveLinkState;            
        }
	 else
	 {
	     vty = (BYTE)(*(BYTE *)pMsgPara);
	 }
        break;
    case OAM_MSG_RESTORE_PREPARE_OK:
    case OAM_MSG_RESTORE_CONTINUE:
    case EV_TXTCFG_REQ:
    case EV_OAMS_VER_UPDATE_NOTIFY:
        vty = MIN_CLI_VTY_NUMBER;
        break;
    case MSG_OFFLINE_CFG_TO_INTERPRET:
    case OAM_CFG_FINISH_ACK:
        /* 专用的linkstate 用于离线转在线处理*/
        return &g_tLinkStateForOfflineToOnline;
        break;
    default:
        return NULL;
    }
    if (NULL != pSaveLinkState)
    {
        return pSaveLinkState;
    }
    else if ((vty < MIN_CLI_VTY_NUMBER) || (vty > MAX_CLI_VTY_NUMBER))
        return NULL;
    else
        return &gtLinkState[vty-MIN_CLI_VTY_NUMBER];

}


/**************************************************************************
*                          全局函数实现                                   *
**************************************************************************/

/**************************************************************************
* 函数名称：OPR_DATA *GetNextOprData
* 功能描述：获取下一个数据
* 访问的表：无
* 修改的表：无
* 输入参数：
  OPR_DATA *pOprData
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
OPR_DATA *GetNextOprData(OPR_DATA *pOprData)
{
    BYTE *pRtnOprData;

    if (DATA_TYPE_TABLE == pOprData->Type)
    {        
        TABLE_DATA *pTableData = (TABLE_DATA *)pOprData;
        int iRecordLen = 0;
        int m;
        for (m = 0; m < pTableData->ItemNumber; m++)
        {
            iRecordLen += pTableData->Item[m].Len;
        }
        if (pOprData->Len == 0)
        {
            pRtnOprData = pTableData->Data + 1;
        }
        else
        {
            pRtnOprData = pTableData->Data + iRecordLen * pOprData->Len;
        }
    }
    else if (DATA_TYPE_LIST == pOprData->Type)
    {
        TABLE_DATA *pTableData = (TABLE_DATA *)pOprData;
        if (pOprData->Len == 0)
        {
            pRtnOprData = (BYTE *)pTableData->Data + 1;
        }
        else
        { 
            int i, m; 
            pRtnOprData = pTableData->Data;
            for (i = 0; i < pOprData->Len; i++)
            {
                for (m = 0; m < pTableData->ItemNumber; m++)
                {
                    if (pTableData->Item[m].Type == DATA_TYPE_STRING)
                    {
                        pRtnOprData += strlen((CHAR *)pRtnOprData) + 1;
                    }
                    else
                    {
                        pRtnOprData += pTableData->Item[m].Len;
                    }
                }
            }
        }
    }
    else if (DATA_TYPE_STRING == pOprData->Type)
    {
        pRtnOprData = (BYTE *)pOprData->Data + strlen((CHAR *)pOprData->Data) + 1;
    }
    else
    {
        pRtnOprData = (BYTE *)pOprData->Data + pOprData->Len;
    }

    return (OPR_DATA *)pRtnOprData;
}

/**************************************************************************
* 函数名称：VOID Oam_InptGetBuf
* 功能描述：本地内存分配
* 访问的表：无
* 修改的表：无
* 输入参数：bufSize:内存大小
            ucLinkID:链路号
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
void *Oam_InptGetBuf(DWORD bufSize,BYTE ucLinkID)
{
    CHAR *buf = NULL;
    if(ucLinkID >= sizeof(g_ptOamIntVar->pMemBufOri)/sizeof(g_ptOamIntVar->pMemBufOri[0]))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
				"....................FUNC: %s,Line:%d ucLinkID Error!%d....................",__func__,__LINE__,ucLinkID);
        return NULL;
    }
    if (NULL == g_ptOamIntVar->pMemBufOri[ucLinkID])
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
				"....................FUNC: %s,Line:%d NULL PTR!....................",__func__,__LINE__);
        return NULL;
    }
    buf = g_ptOamIntVar->pMemBufCur[ucLinkID];
    g_ptOamIntVar->pMemBufCur[ucLinkID] += bufSize;
    return buf;
}

/**************************************************************************
* 函数名称：VOID Oam_InptFreeBuf
* 功能描述：本地内存指针复位
* 访问的表：无
* 修改的表：无
* 输入参数：ucLinkID:连接标识
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
void Oam_InptFreeBuf(BYTE ucLinkID)
{
    if(ucLinkID >= sizeof(g_ptOamIntVar->pMemBufOri)/sizeof(g_ptOamIntVar->pMemBufOri[0]))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
				"....................FUNC: %s,Line:%d ucLinkID Error!%d....................",__func__,__LINE__,ucLinkID);
        return;
    }
    if (NULL == g_ptOamIntVar->pMemBufOri[ucLinkID])
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
				"....................FUNC: %s,Line:%d NULL PTR!....................",__func__,__LINE__);
        return;
    }
    g_ptOamIntVar->pMemBufCur[ucLinkID] = g_ptOamIntVar->pMemBufOri[ucLinkID];
    memset(g_ptOamIntVar->pMemBufCur[ucLinkID], 0, MAX_MEM_BUF);
    return;
}

/**************************************************************************
* 函数名称：VOID Oam_SetDATUpdateWaitTimer
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
void Oam_SetDATUpdateWaitTimer(void)
{
    Oam_KillDATUpdateWaitTimer();
    g_ptOamIntVar->wDatUpdateWaitTimer = XOS_SetRelativeTimer(PROCTYPE_OAM_CLIMANAGER, TIMER_DAT_UPDATE_WAIT, DURATION_DAT_UPDATE_WAIT, 1);
    return;
}

/**************************************************************************
* 函数名称：VOID Oam_SetDATUpdateWaitTimerInvalid
* 功能描述：设置dat update 超时定时器无效
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2011/02/11    V1.0    齐龙兆      创建
**************************************************************************/
void Oam_SetDATUpdateWaitTimerInvalid(void)
{
    g_ptOamIntVar->wDatUpdateWaitTimer = INVALID_TIMER_ID;
    return;
}

/**************************************************************************
* 函数名称：VOID Oam_KillDATUpdateWaitTimer
* 功能描述：杀掉dat update 超时定时器
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
void Oam_KillDATUpdateWaitTimer(void)
{
    if ( INVALID_TIMER_ID !=  g_ptOamIntVar->wDatUpdateWaitTimer)
    {
        XOS_KillTimerByTimerId( g_ptOamIntVar->wDatUpdateWaitTimer);
         g_ptOamIntVar->wDatUpdateWaitTimer = INVALID_TIMER_ID;
    }
    return;
}

void Oam_SetGetCfgVerInfoTimer(void)
{
    Oam_KillGetCfgVerInfoTimer();
    g_ptOamIntVar->wGetCfgVerInfoTimer = XOS_SetRelativeTimer(PROCTYPE_OAM_CLIMANAGER, TIMER_GET_CFGVERINFO, DURATION_GET_CFGVERINFO, 1);
    return;
}

void Oam_KillGetCfgVerInfoTimer(void)
{
    if (INVALID_TIMER_ID !=  g_ptOamIntVar->wGetCfgVerInfoTimer)
    {
        XOS_KillTimerByTimerId( g_ptOamIntVar->wGetCfgVerInfoTimer);
         g_ptOamIntVar->wGetCfgVerInfoTimer = INVALID_TIMER_ID;
    }
    return;
}

/**************************************************************************
* 函数名称：VOID Oam_SetGetCfgVerInfoTimerInvalid
* 功能描述：设置get cfg verinfo 超时定时器无效
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2011/02/11    V1.0    齐龙兆      创建
**************************************************************************/
void Oam_SetGetCfgVerInfoTimerInvalid(void)
{
    g_ptOamIntVar->wGetCfgVerInfoTimer = INVALID_TIMER_ID;
    return;
}

void Oam_SetRegOamsNotifyTimer(void)
{
    Oam_KillRegOamsNotifyTimer();
    g_ptOamIntVar->wRegOamsNotifyTimer = XOS_SetRelativeTimer(PROCTYPE_OAM_CLIMANAGER, TIMER_REGOAMS_NTF_ACK, DURATION_REGOAMS_NTF_ACK, 1);
    return;
}

void Oam_KillRegOamsNotifyTimer(void)
{
    if (INVALID_TIMER_ID !=  g_ptOamIntVar->wRegOamsNotifyTimer)
    {
        XOS_KillTimerByTimerId( g_ptOamIntVar->wRegOamsNotifyTimer);
         g_ptOamIntVar->wRegOamsNotifyTimer = INVALID_TIMER_ID;
    }
    return;
}

/**************************************************************************
* 函数名称：VOID Oam_SetRegOamsNotifyTimerInvalid
* 功能描述：设置reg oams notify 超时定时器无效
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2011/02/11    V1.0    齐龙兆      创建
**************************************************************************/
void Oam_SetRegOamsNotifyTimerInvalid(void)
{
    g_ptOamIntVar->wRegOamsNotifyTimer = INVALID_TIMER_ID;
    return;
}

/**************************************************************************
* 函数名称：BOOL FindUserFromLink
* 功能描述：获取用户
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
BOOL FindUserFromLink(CHAR *pUserName, USER_NODE **ppNode,USER_NODE **pNode)
{
    USER_NODE *ppNodeTmp = NULL;
    USER_NODE *pNodeTmp = pUserLinkHead;
    while(pNodeTmp)
    {
        if (strcmp(pNodeTmp->tUserInfo.sUsername, pUserName) == 0)
        {
            *ppNode = ppNodeTmp;
            *pNode = pNodeTmp;
            return TRUE;
        }
        else
        {
            ppNodeTmp = pNodeTmp;
            pNodeTmp = pNodeTmp->pNext;
        }
    }
    *ppNode = NULL;
    *pNode = NULL;
    return FALSE;
}

/**************************************************************************
* 函数名称：BOOL AddUserToLink
* 功能描述：增加用户
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
BOOL AddUserToLink(USER_NODE *pNode)
{
    USER_NODE *ppNodeTmp = NULL;
    USER_NODE *pNodeTmp = NULL;

    if (!pNode)
        return FALSE;

    if (FindUserFromLink(pNode->tUserInfo.sUsername,&ppNodeTmp,&pNodeTmp))
    {
        /* 修改用户 */
        strncpy(pNodeTmp->tUserInfo.sPassword, pNode->tUserInfo.sPassword, sizeof(pNodeTmp->tUserInfo.sPassword));
        if (pNode->tUserInfo.ucRight != 0)
            pNodeTmp->tUserInfo.ucRight = pNode->tUserInfo.ucRight;

        /* 如果配置设置权限, 则保留原来的权限 */
        return TRUE;
    }
    else
    {
        /* 增加用户 */
        /* 如果配置设置权限, 则默认为1 */
        if (pNode->tUserInfo.ucRight == 0)
            pNode->tUserInfo.ucRight = 1;

        if (pUserLinkHead == NULL)
        {
            pUserLinkHead = pNode;
            pUserLinkTail = pUserLinkHead;

        }
        else
        {
            pUserLinkTail->pNext = pNode;
            pUserLinkTail = pNode;
        }

        return TRUE;
    }
}

/**************************************************************************
* 函数名称：BOOL DelUserFromLink
* 功能描述：删除用户
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
BOOL DelUserFromLink(CHAR *pUserName)
{
    USER_NODE *ppNode = NULL;
    USER_NODE *pNode = NULL;
    BYTE i;

    if (!pUserName)
        return FALSE;

    /*用户不存在*/
    if (!FindUserFromLink(pUserName,&ppNode,&pNode))
        return FALSE;

    /*默认用户不能删除*/
    if (strcmp(pUserName, DEFAULT_USERNAME) == 0)
        return FALSE;

    /*在线用户不能删除*/
    for (i = MIN_CLI_VTY_NUMBER; i <= MAX_CLI_VTY_NUMBER; i++)
    {
        BYTE k = i - MIN_CLI_VTY_NUMBER;
        if (Oam_LinkGetCurRunState(&gtLinkState[k]) != pec_MsgHandle_IDLE)
        {
            if (strcmp(pUserName, gtLinkState[k].sUserName) == 0)
                return FALSE;
        }
    }

    if (pNode == pUserLinkHead)
    {
        pUserLinkHead = pUserLinkHead->pNext;
        if (pUserLinkHead == NULL) pUserLinkTail = NULL;
        OAM_RETUB(pNode);
        pNode = NULL;
    }
    else
    {
        ppNode->pNext = pNode->pNext;
        if (pNode == pUserLinkTail) pUserLinkTail = ppNode;
        OAM_RETUB(pNode);
        pNode = NULL;
    }
    return TRUE;
}

/**************************************************************************
* 函数名称：VOID GetTerminalCfgData
* 功能描述：获取当前用户终端信息
* 访问的表：无
* 修改的表：无
* 输入参数：MSG_COMM_OAM **pRtnMsg
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
void GetTerminalCfgData(MSG_COMM_OAM **pRtnMsg)
{
    MSG_COMM_OAM *ptRtnMsg = *pRtnMsg;
    OPR_DATA *pRtnOprData = NULL;
    TABLE_DATA *pTab = NULL;
    BYTE *pData = NULL;
    CHAR TempStr[MAX_STRING_LEN] = {0};
    WORD16 wDay,wHour,wMinute,wSecond;

    ptRtnMsg->ReturnCode = SUCC_AND_HAVEPARA;

    ptRtnMsg->Number = 1;

    pRtnOprData = (OPR_DATA *)ptRtnMsg->Data;
    pRtnOprData->ParaNo = 0;
    pRtnOprData->Type = DATA_TYPE_TABLE;
    pRtnOprData->Len = 0;

    pTab = (TABLE_DATA *)pRtnOprData->Data;
    pTab->ItemNumber = 1;

    pTab->Item[0].Type = DATA_TYPE_STRING;
    pTab->Item[0].Len = 80;

    pData = (BYTE *) pTab->Data;

    if (ptRtnMsg->LinkChannel == MIN_CLI_VTY_NUMBER)
    {
        wHour = gtCommonInfo.wConsoleIdleTimeout/60;
        wMinute = gtCommonInfo.wConsoleIdleTimeout%60;
        wSecond = 0;
        XOS_snprintf(TempStr, MAX_STRING_LEN, " Console idle-timeout is: %02u:%02u:%02u",wHour,wMinute,wSecond);
        strncpy((CHAR *)pData,TempStr, (WORD32)(MAX_OAMMSG_LEN - (WORD32)((BYTE *)pData - (BYTE *)ptRtnMsg)));
        pData += pTab->Item[0].Len;
        pRtnOprData->Len++;

        wDay = gtCommonInfo.wConsoleAbsTimeout/1440;
        wHour = (gtCommonInfo.wConsoleAbsTimeout%1440)/60;
        wMinute = (gtCommonInfo.wConsoleAbsTimeout%1440)%60;
        XOS_snprintf(TempStr, MAX_STRING_LEN, " Console absolute-timeout is: %ud%02uh%02um",wDay,wHour,wMinute);
        strncpy((CHAR *)pData,TempStr, (WORD32)(MAX_OAMMSG_LEN - (WORD32)((BYTE *)pData - (BYTE *)ptRtnMsg)));
        strncat((CHAR *)pData,"\n", (WORD32)(MAX_OAMMSG_LEN - (WORD32)((BYTE *)pData - (BYTE *)ptRtnMsg)));
        pData += pTab->Item[0].Len;
        pRtnOprData->Len++;
    }
    else
    {
        wHour = gtCommonInfo.wTelnetIdleTimeout/60;
        wMinute = gtCommonInfo.wTelnetIdleTimeout%60;
        wSecond = 0;
        XOS_snprintf(TempStr, MAX_STRING_LEN, " Idle-timeout is: %02u:%02u:%02u",wHour,wMinute,wSecond);
        strncpy((CHAR *)pData,TempStr, (WORD32)(MAX_OAMMSG_LEN - (WORD32)((BYTE *)pData - (BYTE *)ptRtnMsg)));
        strncat((CHAR *)pData,"\n", (WORD32)(MAX_OAMMSG_LEN - (WORD32)((BYTE *)pData - (BYTE *)ptRtnMsg)));
        pData += pTab->Item[0].Len;
        pRtnOprData->Len++;
#if 0
        wDay = gtCommonInfo.wTelnetAbsTimeout/1440;
        wHour = (gtCommonInfo.wTelnetAbsTimeout%1440)/60;
        wMinute = (gtCommonInfo.wTelnetAbsTimeout%1440)%60;
        XOS_snprintf(TempStr, MAX_STRING_LEN, " Telnet absolute-timeout is: %ud%02uh%02um",wDay,wHour,wMinute);
        strncpy((CHAR *)pData,TempStr);
        strncat((CHAR *)pData,"\n");
        pData += pTab->Item[0].Len;
        pRtnOprData->Len++;
#endif
    }

    ptRtnMsg->DataLen = (DWORD)((BYTE *)pData - (BYTE *)ptRtnMsg);
    return;
}

/**************************************************************************
* 函数名称：VOID GetUserNameData
* 功能描述：获取用户列表
* 访问的表：无
* 修改的表：无
* 输入参数：MSG_COMM_OAM **pRtnMsg
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
void GetUserNameData(MSG_COMM_OAM **pRtnMsg)
{
    MSG_COMM_OAM *ptRtnMsg = *pRtnMsg;
    OPR_DATA *pRtnOprData = NULL;
    TABLE_DATA *pTab = NULL;
    BYTE *pData = NULL;
    USER_NODE *pNodeTmp = NULL;
    CHAR TempStr[MAX_STRING_LEN] = {0};

    ptRtnMsg->ReturnCode = SUCC_AND_HAVEPARA;
    ptRtnMsg->Number = 1;

    pRtnOprData = (OPR_DATA *)ptRtnMsg->Data;
    pRtnOprData->ParaNo = 0;
    pRtnOprData->Type = DATA_TYPE_TABLE;
    pRtnOprData->Len = 0;

    pTab = (TABLE_DATA *)pRtnOprData->Data;
    pTab->ItemNumber = 2;

    pTab->Item[0].Type=DATA_TYPE_STRING;
    pTab->Item[0].Len=MAX_STRING_LEN;

    pTab->Item[1].Type=DATA_TYPE_BYTE;
    pTab->Item[1].Len=sizeof(BYTE);

    pData = (BYTE *)pTab->Data;

    pNodeTmp = pUserLinkHead;
    while(pNodeTmp)
    {
        XOS_snprintf(TempStr, MAX_STRING_LEN, "  %s",pNodeTmp->tUserInfo.sUsername);
        strncpy((CHAR *)pData,TempStr, (WORD32)(MAX_OAMMSG_LEN - (WORD32)((BYTE *)pData - (BYTE *)ptRtnMsg)));
        pData += pTab->Item[0].Len;

        memcpy(pData,&pNodeTmp->tUserInfo.ucRight,sizeof(BYTE));
        pData += pTab->Item[1].Len;

        pRtnOprData->Len++;

        pNodeTmp = pNodeTmp->pNext;
    }

    ptRtnMsg->DataLen = (DWORD)((BYTE *)pData - (BYTE *)ptRtnMsg);
    return;
}

/**************************************************************************
* 调试函数开始区域
**************************************************************************/
/**************************************************************************
* 函数名称：VOID OAM_DbgShowCfgAgtPriData
* 功能描述：获取用户列表
* 访问的表：无
* 修改的表：无
* 输入参数：MSG_COMM_OAM **pRtnMsg
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
void OAM_DbgShowCfgAgtPriData(void)
{


    return;
}

/*****************************************************************************/
/**
@brief  CLIMANAGER向自己发送其他子系统的消息，用于定位和其他子系统交互的问题
@verbatim
dwMsgId: WORD16 类型，要发送的消息在调试函数中的编号
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
\n1、只能发送帮助信息中打印出来的这些消息
@li @b 输出示例：
\n无

@li @b 修改记录：
-#  ： 
*/
/*****************************************************************************/
void OAM_DbgSendCliMsg (WORD16 dwMsgId)
{
    WORD32 tmpMsgId = 0;
	
    printf("Message Type Selection\n");
    printf("Protocol Messages \n");
    printf("1--OAM_MSG_PRO_PING_TO_EXECUTION ; 2--OAM_MSG_PRO_TRACE_TO_EXECUTION \n");
    printf("3--OAM_MSG_PROTOCOL_TO_EXECUTION ;\n");
    printf("DBS Messages \n");
    printf("4--EV_MasterSaveOK ; 5--EV_MasterSaveFail \n");
    printf("6--EV_CycleSaveEnableAck ; 7--EV_CycleSaveDisableAck\n");
    printf("8--EV_ImmediatelySaveEnableAck ;9--EV_ImmediatelySaveDisableAck\n");
    printf("10--EV_ERRBACKCMDONMPSTART_ACK ;11--EV_SAVESTATE_QUERY");
    printf("Vmm Messages\n");
    printf("15--EV_CFG_VERINFO_UPDATE_NOTIFY\n");
    printf("SCS Messages\n");
    printf("16--EV_MASTER_TO_SLAVE\n");
    printf("17--EV_SLAVE_TO_MASTER\n");



    switch(dwMsgId)
    {
    case 1:
        tmpMsgId= OAM_MSG_PRO_PING_TO_EXECUTION;
        break;
    case 2:
        tmpMsgId= OAM_MSG_PRO_TRACE_TO_EXECUTION;
        break;
    case 3:
        tmpMsgId= OAM_MSG_PROTOCOL_TO_EXECUTION;
        break;
    case 4:
        tmpMsgId= EV_MasterSaveOK;
        break;
    case 5:
        tmpMsgId= EV_MasterSaveFail;
        break;
    case 6:
        tmpMsgId= EV_CycleSaveEnableAck;
        break;
    case 7:
        tmpMsgId= EV_CycleSaveDisableAck;
        break;
    case 8:
        tmpMsgId= EV_ImmediatelySaveEnableAck;
        break;
    case 9:
        tmpMsgId= EV_ImmediatelySaveDisableAck;
        break;
    case 10:
        tmpMsgId= EV_ERRBACKCMDONMPSTART_ACK;
        break;
    case 11:
        tmpMsgId= EV_SAVESTATE_QUERY;
        break;
    case 14:
        tmpMsgId= EV_TXTCFG_REQ;
        break;
    case 15:
        tmpMsgId= EV_OAMS_VER_UPDATE_NOTIFY;
        break;
    case 16:
        tmpMsgId= EV_MASTER_TO_SLAVE;
        break;
    case 17:
        tmpMsgId= EV_SLAVE_TO_MASTER;
        break;
    default:
        break;
    }

     OSS_SendFromTask(tmpMsgId,NULL,0,NULL,&g_ptOamIntVar->jidSelf);
/*
    if(EV_MasterSaveOK==tmpMsgId)
    {
        OSS_SendFromTask(EV_MasterSaveOK,NULL,0,NULL,&g_ptOamIntVar->jidSelf);
    }


    if(EV_TXTCFG_REQ == tmpMsgId)
    {
        OSS_SendFromTask(EV_TXTCFG_REQ,NULL,0,NULL,&g_ptOamIntVar->jidSelf);
    }
    */

}

void Oam_ShowAllVerInfo(void)
{
    WORD16 wLoopCnt = 0;

    XOS_SysLog(OAM_CFG_LOG_NOTICE, "All DAT file number is %d\n", g_ptOamIntVar->wDatSum);
    for (wLoopCnt = 0; wLoopCnt < g_ptOamIntVar->wDatSum; wLoopCnt++)
    {
        XOS_SysLog(OAM_CFG_LOG_NOTICE, "Record %d: function type = %d filename = %s/%s Using language type = %s\n",
                 wLoopCnt,
                 g_ptOamIntVar->tDatVerInfo[wLoopCnt].tFileInfo.wFuncType,
                 g_ptOamIntVar->tDatVerInfo[wLoopCnt].tFileInfo.acVerFilePath,
                 g_ptOamIntVar->tDatVerInfo[wLoopCnt].tFileInfo.acVerFileName,
                 g_ptOamIntVar->tDatVerInfo[wLoopCnt].tLang.aucLangType);
    }
}

/*****************************************************************************/
/**
@brief 显示CLI所收到的VMM发来的全部DAT版本信息，用于定位CLI界面命令不全的情况
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
\nRecord 0: function type = 1, filename = /IDE0/setup/omp.pkg/210c0000//oam-NLS.dat, Using language type = chinese
Record 1: function type = 2, filename = /IDE0/setup/omp.pkg/210c0000//phy-NLS.dat, Using language type = chinese
Record 2: function type = 4, filename = /IDE0/setup/omp.pkg/210c0000//sig-NLS.dat, Using language type = chinese
Record 3: function type = 5, filename = /IDE0/setup/omp.pkg/210c0000//brs-NLS.dat, Using language type = chinese
@li @b 修改记录：
-#  ： 
*/
/*****************************************************************************/
void OAM_DbgShowCliDatVerInfo(void)
{
    WORD16 wLoopCnt = 0;

    printf("All DAT file number is %d\n", g_ptOamIntVar->wDatSum);
    printf("                [DAT version infomation table]\n");
    printf( "Index,  Function type,  Using language type,  Filename\n");
    printf( "===========================================================\n");
    for (wLoopCnt = 0; wLoopCnt < g_ptOamIntVar->wDatSum; wLoopCnt++)
    {
        CHAR aucTmp[OAM_CLI_LANGTYPE_LEN] = {0};
        strcpy(aucTmp, "-");
        if (strlen(g_ptOamIntVar->tDatVerInfo[wLoopCnt].tLang.aucLangType) > 0)
        {
            strcpy(aucTmp, g_ptOamIntVar->tDatVerInfo[wLoopCnt].tLang.aucLangType);
        }
		
        printf( "%5d,%15d,%21s,  %s/%s\n",
                 wLoopCnt,
                 g_ptOamIntVar->tDatVerInfo[wLoopCnt].tFileInfo.wFuncType,
                 aucTmp,
                 g_ptOamIntVar->tDatVerInfo[wLoopCnt].tFileInfo.acVerFilePath,
                 g_ptOamIntVar->tDatVerInfo[wLoopCnt].tFileInfo.acVerFileName);
    }
    printf( "===========================================================\n");
}

/**************************************************************************
* 函数名称：Oam_CfgIfCurVersionIsPC
* 功能描述：判断当前运行版本是pc机版本还是刀片版本
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：    TRUE  -- 是PC机版本
                                 FALSE -- 不是pc机版本
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/06/08   V1.0     殷浩     创建
**************************************************************************/
BOOLEAN Oam_CfgIfCurVersionIsPC(void)
{
    BOOLEAN bRet = TRUE;
    
#ifdef  OAM_CFG_DAT_GET_FROM_VMM
    bRet = FALSE;
#else
    bRet = TRUE;
#endif

    return bRet;
}

/**************************************************************************
* 函数名称：Oam_CfgCheckRecvMsgMinLen
* 功能描述：判断当前收到的消息长度是否合法
* 访问的表：无
* 修改的表：无
* 输入参数：wMinLen -- 该类型消息最小合法长度
* 输出参数：无
* 返 回 值：    TRUE  -- 长度合法
                                 FALSE -- 长度不合法
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/06/08   V1.0     殷浩     创建
**************************************************************************/
BOOLEAN Oam_CfgCheckRecvMsgMinLen(WORD16 wMinLen)
{
    WORD16 wRecvLen = 0;
    BOOLEAN bRet = FALSE;

    if (XOS_GetMsgDataLen(PROCTYPE_OAM_CLIMANAGER, &wRecvLen) != XOS_SUCCESS)
    {
        /*如果获取收到的消息长度失败，返回false*/
        XOS_SysLog(OAM_CFG_LOG_ERROR, "Oam_CfgCheckRecvMsgMinLen:call XOS_GetMsgDataLen failed! ");
                    
        bRet = FALSE;
    }
    else
    {
        if (wRecvLen < wMinLen)
        {
            bRet = FALSE;
            XOS_SysLog(OAM_CFG_LOG_ERROR,
                "Oam_CfgCheckRecvMsgMinLen: receive message length is %d, less than minimum length %d! ",
                wRecvLen, wMinLen);
        }
        else
        {
            bRet = TRUE;
        }
    }

    return bRet;
}

/* end of add */

BOOL FindUserInfoFromLink(CHAR *pUserName, TYPE_USER_INFO *pSSHUser)
{
    BYTE i = 0;

    if (pUserName == NULL)
        return FALSE;
    
    if (0 == strlen(pUserName))
        return FALSE;
    
    for (i = 0; i < sizeof(gtUserInfo)/sizeof(TYPE_USER_INFO); i++)
    {
        if (strcmp(pUserName, gtUserInfo[i].szUserName) == 0)
        {
            strncpy(pSSHUser->szUserName, pUserName, sizeof(pSSHUser->szUserName));
            strncpy(pSSHUser->szPassWord, gtUserInfo[i].szPassWord, sizeof(pSSHUser->szPassWord));
            pSSHUser->ucRole = gtUserInfo[i].ucRole;
            pSSHUser->bLocked = gtUserInfo[i].bLocked;
            return TRUE;
        }
    }
    
    return FALSE;
}
/**************************************************************************
* 函数名称：CheckBlfOnline
* 功能描述：判断用户是否在线，在线返回true，不在线返回false
* 访问的表：无
* 修改的表：无
* 输入参数：sUserName --用户名
* 输出参数：无
* 返 回 值：    TRUE  -- 在线
                                 FALSE --不在线
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/06/08   V1.0     殷浩     创建
**************************************************************************/
BOOL CheckUsernameBlfOnline(CHAR *sUserName)
{
    if (sUserName == NULL)
    {
        return FALSE;    
    }
    int i=0;
    for (i = 0; i < MAX_CLI_VTY_NUMBER; i++)
    {
        if (strcmp(gtLinkState[i].sUserName, sUserName) == 0)
	{
	    if(pec_MsgHandle_IDLE != Oam_LinkGetCurRunState(&gtLinkState[i]))
	    {
	        return  TRUE;
	    }
	    else
	    {
	        return FALSE;
	    }
	}
    }
    return FALSE;
}

BOOL AddOneUserToLink(TYPE_USER_INFO *pSSHUser)
{
    BYTE i = 0;
    TYPE_USER_INFO tSSHUserInfo;
    
    if (!pSSHUser)
        return FALSE;

    memset(&tSSHUserInfo, 0, sizeof(TYPE_USER_INFO));
    if (FindUserInfoFromLink(pSSHUser->szUserName, &tSSHUserInfo))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[AddOneUserToLink]: The user is exists!\n");
        return FALSE;
    }
    else
    {
        if (gnUserNum >= MAX_USER_NUM_ROLE_ADMIN + MAX_USER_NUM_ROLE_CONFIG + MAX_USER_NUM_ROLE_VIEW)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, "[AddOneUserToLink]: The number of all user is full!\n");
            return FALSE;
        }
        
        /* 增加用户 */
        for (i = 0; i < sizeof(gtUserInfo)/sizeof(TYPE_USER_INFO); i++)
        {
            if (gtUserInfo[i].ucRole == 0)
            {
                memcpy(&gtUserInfo[i], pSSHUser, sizeof(TYPE_USER_INFO));
                gnUserNum++;
                return TRUE;
            }
        }
    }

    return FALSE;
}
 BOOL UpdateOneUserToLink(TYPE_USER_INFO *pSSHUser)
 {
    WORD16 i=0;
    if (!pSSHUser)
    {
        return FALSE;
    }
    for (i = 0; i < sizeof(gtUserInfo)/sizeof(TYPE_USER_INFO); i++)
    {
        if (strcmp(pSSHUser->szUserName, gtUserInfo[i].szUserName) == 0)
        {
            break;
        }
    }
    if(i<sizeof(gtUserInfo)/sizeof(TYPE_USER_INFO))
    {
        memcpy(&gtUserInfo[i],pSSHUser,sizeof(TYPE_USER_INFO));
        return TRUE;
    }
    else
    {
        return FALSE;
    }
 }

/* 将一条新日志写入文件 */
BOOL InsertOneUseLog(TYPE_LINK_STATE *pLinkState)
{
    XOS_FD fd;
    XOS_FD fdTemp;
    XOS_STATUS ulRet = 0;
    TYPE_USER_LOG_HEAD tFileHeadTemp;
    TYPE_USER_LOG_HEAD tFileHead;
    TYPE_USER_LOG tUserLog;
    WORD16 i = 0;
    DWORD dwOffset = 0;
    SWORD32 sdwByteCount;
    SWORD32 sdwFileLen = 0;
    BOOL bFileTooBig = FALSE;
    BYTE ucVty = 0;
    BOOL bIsFileExists = FALSE;
    CHAR  acOamDir[FILE_PATH_LEN] = {'\0'}; 
    CHAR  acMateDir[FILE_PATH_LEN] = {'\0'}; 
    CHAR  acUserFile[MAX_ABS_PATH_LEN] = {'\0'};
    CHAR  acUserFileTmp[MAX_ABS_PATH_LEN] = {'\0'};

    if (!OamGetPath(acOamDir, acMateDir))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[InsertOneUseLog]OamGetPath fail!\n");
        return FALSE;
    }
    XOS_snprintf(acUserFile, MAX_ABS_PATH_LEN, "%s%s", acOamDir, OAM_USER_LOG_FILE);
    XOS_snprintf(acUserFileTmp, MAX_ABS_PATH_LEN, "%s%s", acOamDir, OAM_USER_LOG_FILE_TEMP);

    /* 首先判断日志文件是否存在 */
    bIsFileExists = OamUserFileExists(acUserFile);
    /* 如果文件不存在，则创建该文件 */
    if (!bIsFileExists)
    {
        ulRet = XOS_OpenFile(acUserFile, XOS_CREAT, &fd);
        if (ulRet != XOS_SUCCESS)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, "[InsertOneUseLog] XOS_OpenFile exec failed! ulRet = %d, line = %d\n", ulRet, __LINE__);
            return FALSE;
        }

        /* 写文件头 */
        tFileHead.dwFormatDef = USER_LOG_FILE_FORMAT;
        tFileHead.dwVersion = USER_LOG_FILE_VERSION;
        tFileHead.dwLogBegin = sizeof(TYPE_USER_LOG_HEAD);
        tFileHead.dwLogNum = 0;

        ulRet = XOS_WriteFile(fd, (CHAR *)&tFileHead,sizeof(TYPE_USER_LOG_HEAD), &sdwByteCount);
        if (ulRet != XOS_SUCCESS)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, "[InsertOneUseLog] XOS_WriteFile exec failed! ulRet = %d, line = %d\n", ulRet, __LINE__);
            XOS_CloseFile(fd);
            return FALSE;
        }

        if (sdwByteCount != sizeof(TYPE_USER_LOG_HEAD))
        {
            XOS_CloseFile(fd);
            return FALSE;
        }
        
        XOS_CloseFile(fd);
    }

    /* 获取文件长度 */
    ulRet = XOS_GetFileLength(acUserFile, &sdwFileLen);
    if (ulRet != XOS_SUCCESS)
    {
        return FALSE;
    }

    if ((sdwFileLen + sizeof(TYPE_USER_LOG)) > MAX_USER_LOG_FILE_SIZE)
        bFileTooBig = TRUE;

    /* 打开原日志文件 */
    ulRet = XOS_OpenFile(acUserFile, XOS_RDONLY, &fd);
    if (ulRet != XOS_SUCCESS)
    {
        return FALSE;
    }
    /* 读取文件头 */
    ulRet = XOS_ReadFile(fd, (CHAR *)&tFileHead,sizeof(TYPE_USER_LOG_HEAD), &sdwByteCount);
    if (ulRet != XOS_SUCCESS)
    {
        XOS_CloseFile(fd);
        return FALSE;
    }
    if (sdwByteCount != sizeof(TYPE_USER_LOG_HEAD))
    {
        XOS_CloseFile(fd);
        return FALSE;
    }
    if (tFileHead.dwFormatDef != USER_LOG_FILE_FORMAT)
    {
        XOS_CloseFile(fd);
        return FALSE;
    }
    if (tFileHead.dwVersion != USER_LOG_FILE_VERSION)
    {
        XOS_CloseFile(fd);
        return FALSE;
    }
    if (!(sizeof(TYPE_USER_LOG_HEAD)<= tFileHead.dwLogBegin))
    {
        XOS_CloseFile(fd);
        return FALSE;
    }

    if(bFileTooBig)
        pLinkState->tUserLog.wNo = tFileHead.dwLogNum;
    else
        pLinkState->tUserLog.wNo = tFileHead.dwLogNum + 1;

    /* 新建临时文件 */
    ulRet = XOS_OpenFile(acUserFileTmp, XOS_CREAT, &fdTemp);
    if (ulRet != XOS_SUCCESS)
    {
        XOS_CloseFile(fd);
        return FALSE;
    }

    /* 写临时文件文件头 */
    tFileHeadTemp.dwFormatDef = USER_LOG_FILE_FORMAT;
    tFileHeadTemp.dwVersion = USER_LOG_FILE_VERSION;
    tFileHeadTemp.dwLogBegin = sizeof(TYPE_USER_LOG_HEAD);
    tFileHeadTemp.dwLogNum = pLinkState->tUserLog.wNo;

    ulRet = XOS_WriteFile(fdTemp, (CHAR *)&tFileHeadTemp,sizeof(TYPE_USER_LOG_HEAD), &sdwByteCount);
    if (ulRet != XOS_SUCCESS)
    {
        XOS_CloseFile(fdTemp);
        XOS_CloseFile(fd);
        return FALSE;
    }
    if (sdwByteCount != sizeof(TYPE_USER_LOG_HEAD))
    {
        XOS_CloseFile(fdTemp);
        XOS_CloseFile(fd);
        return FALSE;
    }

    if (bFileTooBig)
        ulRet = XOS_SeekFile(fd, XOS_SEEK_SET, tFileHead.dwLogBegin + sizeof(TYPE_USER_LOG));
    else
        ulRet = XOS_SeekFile(fd, XOS_SEEK_SET, tFileHead.dwLogBegin);
        
    if (ulRet != XOS_SUCCESS)
    {
        XOS_CloseFile(fdTemp);
        XOS_CloseFile(fd);
        return FALSE;
    }

    /* 如果文件超过最大容量，则需要将其他链路中的记录序列号减1，因为下面的循环中会将所有原记录序列号减1 */
    if(bFileTooBig)
    {
        for (ucVty = MIN_CLI_VTY_NUMBER; ucVty <= MAX_CLI_VTY_NUMBER; ucVty++)
        {
            if ((Oam_LinkGetCurRunState(pLinkState) != pec_MsgHandle_IDLE) &&
                (gtLinkState[ucVty-MIN_CLI_VTY_NUMBER].tUserLog.wNo > 0) &&
                (gtLinkState[ucVty-MIN_CLI_VTY_NUMBER].ucLinkID != pLinkState->ucLinkID))
            {
                gtLinkState[ucVty-MIN_CLI_VTY_NUMBER].tUserLog.wNo--;
            }
        }
    }

    dwOffset = tFileHeadTemp.dwLogBegin;
    for (i=0; i<tFileHead.dwLogNum; i++)
    {
        /* 从原文件中读取每条记录 */
        memset(&tUserLog, 0, sizeof(TYPE_USER_LOG));
        ulRet = XOS_ReadFile(fd, (CHAR *)&tUserLog, sizeof(TYPE_USER_LOG), &sdwByteCount);
        if (ulRet != XOS_SUCCESS)
        {
            XOS_CloseFile(fdTemp);
            XOS_CloseFile(fd);
            return FALSE;
        }
        if (sdwByteCount != sizeof(TYPE_USER_LOG))
        {
            XOS_CloseFile(fdTemp);
            XOS_CloseFile(fd);
            return FALSE;
        }

        /* 如果文件大小超过最大值，需要重新排列记录序列号 */
        if (bFileTooBig)
            tUserLog.wNo--;

        /* 写入临时文件 */
        if (XOS_SeekFile(fdTemp, XOS_SEEK_SET, dwOffset) != XOS_SUCCESS)
        {
            XOS_CloseFile(fdTemp);
            XOS_CloseFile(fd);
            return FALSE;
        }

        ulRet = XOS_WriteFile(fdTemp, (CHAR *)&tUserLog,sizeof(TYPE_USER_LOG), &sdwByteCount);
        if (ulRet != XOS_SUCCESS)
        {
            XOS_CloseFile(fdTemp);
            XOS_CloseFile(fd);
            return FALSE;
        }
        if (sdwByteCount != sizeof(TYPE_USER_LOG))
        {
            XOS_CloseFile(fdTemp);
            XOS_CloseFile(fd);
            return FALSE;
        }
        dwOffset = dwOffset + sizeof(TYPE_USER_LOG);

        /* 如果超过文件最大容量，实际是从第二条记录开始读取的，所以应该少计算一个i */
        if (bFileTooBig)
        {
            if (i >= (tFileHead.dwLogNum - 2))
                break;
        }

    }

    /* 开始将当前记录写入文件末尾 */
    if (strlen(pLinkState->tUserLog.sUserName) == 0)
    {
        XOS_CloseFile(fdTemp);
        XOS_CloseFile(fd);
        /* 删除临时文件 */
        XOS_DeleteFile(acUserFileTmp);
        return FALSE;
    }
    
    if (XOS_SeekFile(fdTemp, XOS_SEEK_SET, dwOffset) != XOS_SUCCESS)
    {
        XOS_CloseFile(fdTemp);
        XOS_CloseFile(fd);
        /* 删除临时文件 */
        XOS_DeleteFile(acUserFileTmp);
        return FALSE;
    }

    ulRet = XOS_WriteFile(fdTemp, (CHAR *)&pLinkState->tUserLog,sizeof(TYPE_USER_LOG), &sdwByteCount);
    if (ulRet != XOS_SUCCESS)
    {
        XOS_CloseFile(fdTemp);
        XOS_CloseFile(fd);
        /* 删除临时文件 */
        XOS_DeleteFile(acUserFileTmp);
        return FALSE;
    }
    if (sdwByteCount != sizeof(TYPE_USER_LOG))
    {
        XOS_CloseFile(fdTemp);
        XOS_CloseFile(fd);
        /* 删除临时文件 */
        XOS_DeleteFile(acUserFileTmp);
        return FALSE;
    }
        
    XOS_CloseFile(fdTemp);
    XOS_CloseFile(fd);

    /* 删除原文件，将临时文件更名为正式文件 */
    XOS_DeleteFile(acUserFile);
    XOS_RenameFile(acUserFileTmp, acUserFile);

    return TRUE;
}

BOOL UpdateOneUseLog(TYPE_LINK_STATE *pLinkState)
{
    XOS_FD fd;
    XOS_FD fdTemp;
    XOS_STATUS dwRet = XOS_SUCCESS;
    TYPE_USER_LOG_HEAD tFileHead;
    SWORD32 sdwByteCount;
    DWORD dwOffset = 0;
    WORD16 i = 0;
    TYPE_USER_LOG tUserLog;
    CHAR  acOamDir[FILE_PATH_LEN] = {'\0'}; 
    CHAR  acMateDir[FILE_PATH_LEN] = {'\0'}; 
    CHAR  acUserFile[MAX_ABS_PATH_LEN] = {'\0'};
    CHAR  acUserFileTmp[MAX_ABS_PATH_LEN] = {'\0'};

    if (!OamGetPath(acOamDir, acMateDir))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[UpdateOneUseLog]OamGetPath fail!\n");
        return FALSE;
    }
    XOS_snprintf(acUserFile, MAX_ABS_PATH_LEN, "%s%s", acOamDir, OAM_USER_LOG_FILE);
    XOS_snprintf(acUserFileTmp, MAX_ABS_PATH_LEN, "%s%s", acOamDir, OAM_USER_LOG_FILE_TEMP);

    /* 打开原文件 */
    dwRet = XOS_OpenFile(acUserFile, XOS_RDONLY, &fd);
    if (dwRet != XOS_SUCCESS)
    {
        return FALSE;
    }
    /* 读取文件头 */
    dwRet = XOS_ReadFile(fd, (CHAR *)&tFileHead,sizeof(TYPE_USER_LOG_HEAD), &sdwByteCount);
    if (dwRet != XOS_SUCCESS)
    {
        XOS_CloseFile(fd);
        return FALSE;
    }
    if (sdwByteCount != sizeof(TYPE_USER_LOG_HEAD))
    {
        XOS_CloseFile(fd);
        return FALSE;
    }
    if (tFileHead.dwFormatDef != USER_LOG_FILE_FORMAT)
    {
        XOS_CloseFile(fd);
        return FALSE;
    }
    if (tFileHead.dwVersion != USER_LOG_FILE_VERSION)
    {
        XOS_CloseFile(fd);
        return FALSE;
    }
    if (!(sizeof(TYPE_USER_LOG_HEAD)<= tFileHead.dwLogBegin))
    {
        XOS_CloseFile(fd);
        return FALSE;
    }

    /* 新建临时文件 */
    dwRet = XOS_OpenFile(acUserFileTmp, XOS_CREAT, &fdTemp);
    if (dwRet != XOS_SUCCESS)
    {
        XOS_CloseFile(fd);
        return FALSE;
    }

    /* 写临时文件文件头 */
    dwRet = XOS_WriteFile(fdTemp, (CHAR *)&tFileHead,sizeof(TYPE_USER_LOG_HEAD), &sdwByteCount);
    if (dwRet != XOS_SUCCESS)
    {
        XOS_CloseFile(fdTemp);
        XOS_CloseFile(fd);
        return FALSE;
    }
    if (sdwByteCount != sizeof(TYPE_USER_LOG_HEAD))
    {
        XOS_CloseFile(fdTemp);
        XOS_CloseFile(fd);
        return FALSE;
    }

    /* 从原文件中读取内容，写入临时文件 */
    dwRet = XOS_SeekFile(fd, XOS_SEEK_SET, tFileHead.dwLogBegin);
    if (dwRet != XOS_SUCCESS)
    {
        XOS_CloseFile(fdTemp);
        XOS_CloseFile(fd);
        return FALSE;
    }

    dwOffset = tFileHead.dwLogBegin;
    for (i=0; i<tFileHead.dwLogNum; i++)
    {
        /* 从原文件中读取每条记录 */
        memset(&tUserLog, 0, sizeof(TYPE_USER_LOG));
        dwRet = XOS_ReadFile(fd, (CHAR *)&tUserLog, sizeof(TYPE_USER_LOG), &sdwByteCount);
        if (dwRet != XOS_SUCCESS)
        {
            XOS_CloseFile(fdTemp);
            XOS_CloseFile(fd);
            return FALSE;
        }
        if (sdwByteCount != sizeof(TYPE_USER_LOG))
        {
            XOS_CloseFile(fdTemp);
            XOS_CloseFile(fd);
            return FALSE;
        }

        if (tUserLog.wNo == pLinkState->tUserLog.wNo)
        {
            memcpy(&tUserLog.tSysClockLogout, &pLinkState->tUserLog.tSysClockLogout, sizeof(T_SysSoftClock));
            tUserLog.ucQuitReason = pLinkState->tUserLog.ucQuitReason;
        }

        /* 写入临时文件 */
        if (XOS_SeekFile(fdTemp, XOS_SEEK_SET, dwOffset) != XOS_SUCCESS)
        {
            XOS_CloseFile(fdTemp);
            XOS_CloseFile(fd);
            return FALSE;
        }

        dwRet = XOS_WriteFile(fdTemp, (CHAR *)&tUserLog,sizeof(TYPE_USER_LOG), &sdwByteCount);
        if (dwRet != XOS_SUCCESS)
        {
            XOS_CloseFile(fdTemp);
            XOS_CloseFile(fd);
            return FALSE;
        }
        if (sdwByteCount != sizeof(TYPE_USER_LOG))
        {
            XOS_CloseFile(fdTemp);
            XOS_CloseFile(fd);
            return FALSE;
        }
        dwOffset = dwOffset + sizeof(TYPE_USER_LOG);

    }

    XOS_CloseFile(fdTemp);
    XOS_CloseFile(fd);

    XOS_DeleteFile(acUserFile);
    XOS_RenameFile(acUserFileTmp, acUserFile);
    
    return TRUE;
}

void OamMateRequestSubScr(void)
{
    JID tMateJID = {0};
    XOS_STATUS dwRtn = XOS_SUCCESS;
    BYTE        ucBoardPosition = 0;

    /* 获取对板JID */
    dwRtn = XOS_GetSelfJID(&tMateJID);
    if(XOS_SUCCESS != dwRtn)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[OamMateRequestSubScr]XOS_GetSelfJID fail! dwRtn = %d\n", dwRtn);
        return;
    }

    /* 判断左板还是右板 */
    dwRtn = XOS_GetBoardPosition(&ucBoardPosition);
    if(XOS_SUCCESS != dwRtn)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[OamMateRequestSubScr]XOS_GetBoardPosition fail! dwRtn = %d\n", dwRtn);
        return;
    }
    if(XOS_LEFT_BOARD == ucBoardPosition)
    {
        tMateJID.ucRouteType = COMM_RIGHT_CONTROL;
    }
    else if(XOS_RIGHT_BOARD == ucBoardPosition)
    {
        tMateJID.ucRouteType = COMM_LEFT_CONTROL;
    }
    else
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[OamMateRequestSubScr]board positon error!\n");
        return;	
    }

    /* 订阅对板上电消息
    dwRtn = XOS_RequestSubScr((VOID*)&tMateJID, OAM_CLI_REQUESTSUBSCR_TIME);
    if (XOS_SUCCESS != dwRtn)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[OamMateRequestSubScr]XOS_RequestSubScr fail! dwRtn = %d\n", dwRtn);
        return;
    }
    else
    {
        XOS_SysLog(OAM_CFG_LOG_DEBUG, "[OamMateRequestSubScr]XOS_RequestSubScr success!\n");
    } */
}

void OamRecvMateSubScr(LPVOID pMsgPara)
{
#if 0
    WORD32 dwJno = 0;      
    XOS_STATUS dwRtn = XOS_SUCCESS;
    T_SUBSCR_EVENT_TOAPP *ptMsg = NULL;
    WORD16 wRecvMsgLen = 0;
    CHAR acOamDir[FILE_PATH_LEN] = {'\0'};
    CHAR acMateDir[FILE_PATH_LEN] = {'\0'};

    if (!OamGetPath(acOamDir, acMateDir))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[OamRecvMateSubScr]OamGetPath fail!\n");
        return;
    }

    dwRtn = XOS_GetMsgDataLen(&wRecvMsgLen);
    if (XOS_SUCCESS != dwRtn)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[OamRecvMateSubScr]XOS_GetMsgDataLen fail! dwRtn = %d\n", dwRtn);
        return;
    }
    if (wRecvMsgLen != sizeof(T_SUBSCR_EVENT_TOAPP))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[OamRecvMateSubScr] length of recv msg error!\n");
        return;
    }

    ptMsg = (T_SUBSCR_EVENT_TOAPP *)pMsgPara;

    /* 构造Manager的Jno */
    dwJno = XOS_ConstructJNO(PROCTYPE_OAM_CLIMANAGER, 1);
    if(dwJno == ptMsg->tSubscrptJID.dwJno)
    {
        /* JOB退出 */
        if(TIPC_WITHDRAWN == ptMsg->event)
        {
        }
        /* JOB重启 */
        else if(TIPC_PUBLISHED == ptMsg->event)
        {

        }
        else if(TIPC_SUBSCR_TIMEOUT == ptMsg->event)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, "[OamRecvMateSubScr] SubSrc timeout occured!\n");
        }
        else
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, "[OamRecvMateSubScr] SubSrc other exception occured!\n");
        }
    }
    else
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[OamRecvMateSubScr] dwJno is error!\n");
    }

    return;
#endif
}

/*****************************************************************************/
/**
@brief  打印已经设定的用于用户解锁定的IP地址
@verbatim
\n无
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
\n
@li @b 输出示例：
\nThe specified IP address: 129.0.0.1
@li @b 修改记录：
-#  ： 
*/
/*****************************************************************************/
void OAM_DbgShowCliSpecIP(void)
{
    printf("The specified IP address: %s\n", g_szSpecifyIP);
}

/* 调试函数:设定指定IP */
void Oam_DbgSetSpecIP(CHAR *pIPAddr)
{
    if (NULL == pIPAddr)
    {
        printf("%%The input ip is NULL!\n");
        return;
    }
    
    memset(g_szSpecifyIP, 0, LEN_IP_ADDR_STR);
    strncpy(g_szSpecifyIP, pIPAddr, sizeof(g_szSpecifyIP));

    printf("%%Set special IP success!\n");
    
}
    
/*****************************************************************************/
/**
@brief 显示CLIMANAGER上命令日志中记录的命令执行日志
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
\n无
@li @b 输出示例：
\n2010-12-22 15:57:47, SSH, root, 192.168.1.200, install_remove sev.pkg 285278208, FAILED
2010-12-22 15:58:0, SSH, root, 192.168.1.200, install_deactive_dft sev.pkg 285278208, SUCCESS
2010-12-22 15:58:15, SSH, root, 192.168.1.200, install_remove /IDE0/oam/cli/CliCmdLog.txtsev.pkg 285278208, SUCCESS
2010-12-22 15:59:8, SSH, root, 192.168.1.200, install_deactive_dft SBCV4.00.10_PC_D_0x0B0213.pkg 721427, FAILED
2010-12-22 15:59:20, SSH, root, 192.168.1.200, install_remove SBCV4.00.10_PC_D_0x0B0213.pkg 721427, SUCCESS

@li @b 修改记录：
-#  ： 
*/
/*****************************************************************************/
void OAM_DbgShowCliCmdLog(void)
{
    XOS_FD fd;
    XOS_STATUS dwRet = XOS_SUCCESS;
    CHAR  acOamDir[FILE_PATH_LEN] = {'\0'}; 
    CHAR  acMateDir[FILE_PATH_LEN] = {'\0'}; 
    CHAR  acCmdLogFile[MAX_ABS_PATH_LEN] = {'\0'};
    WORD32 sdwLeftFileSize = 0;
    SWORD32 sdwByteCount;
    CHAR acReadBuf[MAX_LOG_FILE_READ_BUF] = {0};

    if (!OamGetPath(acOamDir, acMateDir))
    {
        printf("%%OamGetPath exec failed!\n");
        return;
}

    XOS_snprintf(acCmdLogFile, MAX_ABS_PATH_LEN, "%s%s", acOamDir, OAM_CMDLINE_LOG_FILENAME);
    
    dwRet = XOS_GetFileLength(acCmdLogFile, (SWORD32 *)&sdwLeftFileSize);
    if (dwRet != XOS_SUCCESS)
{
        printf("%%XOS_GetFileLength exec failed! dwRet = %d\n", dwRet);
        return;
    }
    printf("%%Length of command log file is: %d\n", sdwLeftFileSize);

    /* 打开原文件 */
    dwRet = XOS_OpenFile(acCmdLogFile, XOS_RDONLY, &fd);
    if (dwRet != XOS_SUCCESS)
    {
        printf("%%XOS_OpenFile exec failed! dwRet = %d\n", dwRet);
        return;
    }
    
    while (sdwLeftFileSize > 0)
    {
        dwRet = XOS_ReadFile(fd, (VOID *)acReadBuf, MAX_LOG_FILE_READ_BUF, (SWORD32 *)&sdwByteCount);
        if (dwRet != XOS_SUCCESS)
        {
            printf("%%XOS_ReadFile exec failed! dwRet = %d\n", dwRet);
            XOS_CloseFile(fd);
            return;
        }

        if (sdwLeftFileSize > MAX_LOG_FILE_READ_BUF)
        {
            if (sdwByteCount != MAX_LOG_FILE_READ_BUF)
            {
                printf("%%Size of read is error!\n");
                XOS_CloseFile(fd);
                return;
            }
            sdwLeftFileSize = sdwLeftFileSize - MAX_LOG_FILE_READ_BUF;
        }
        else
        {
            if ((WORD32)sdwByteCount != sdwLeftFileSize)
            {
                printf("%%Size of last read is error!\n");
                XOS_CloseFile(fd);
                return;
            }
            sdwLeftFileSize = 0;
        }
        printf("%s", acReadBuf);
    }
    XOS_CloseFile(fd);

}

BOOL IsSameLogicAddr(JID *ptJid1, JID *ptJid2)
{
    BOOLEAN bRet = FALSE;

    if ((ptJid1->wModule == ptJid2->wModule) && 
        (ptJid1->wUnit == ptJid2->wUnit) &&
        (ptJid1->ucSubSystem == ptJid2->ucSubSystem) &&
        (ptJid1->ucSUnit == ptJid2->ucSUnit))
        bRet = TRUE;

    return bRet;
}
BOOL LogoutLinkStateToTelnet(CHAR *pUserName)
{
    int i =0;
    /*入参判断*/
    if (NULL == pUserName)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return FALSE;
    }
    for (i = MIN_CLI_VTY_NUMBER; i <= MAX_CLI_VTY_NUMBER; i++)
    {
        if ((strcmp(gtLinkState[i-MIN_CLI_VTY_NUMBER].sUserName, pUserName) == 0) &&
            (strcmp(pUserName, DEFAULT_USER_NAME) != 0))
        {
            SendLogoutToTelnet(&gtLinkState[i-MIN_CLI_VTY_NUMBER], Hdr_szUserBeenDeleted);
        }
    }
    if(i<=MAX_CLI_VTY_NUMBER)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
} 

BOOL DeleteUserInfo(CHAR *pUserName)
{
    WORD16 i=0;
    /*入参判断*/
    if (NULL == pUserName)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return FALSE;
    }
    for (i = 0; i < sizeof(gtUserInfo)/sizeof(TYPE_USER_INFO); i++)
    {
        if (strcmp(gtUserInfo[i].szUserName, pUserName) == 0)
        {
            break;
        }
    }
    if(i > sizeof(gtUserInfo)/sizeof(TYPE_USER_INFO))
    {
        return FALSE;
    }
    else
    {
        gnUserNum--;   
        gtUserInfo[i].ucRole = 0;
        gtUserInfo[i].bLocked = FALSE;
        memset(gtUserInfo[i].szUserName, 0, MAX_OAM_USER_NAME_LEN);
        memset(gtUserInfo[i].szPassWord, 0, MAX_OAM_USER_PWD_ENCRYPT_LEN);    
        return FALSE;
    }	

}
BOOL GetUserConfViewNum(BYTE *pUserConfCnt,BYTE *pUserViewCnt)
{
    WORD16 i=0;
    BYTE ucUserConfcnt=0;
    BYTE ucUserViewCnt=0;
    /*入参判断*/
    if ((NULL == pUserConfCnt) ||(NULL == pUserViewCnt))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return FALSE;
    }
    for (i = 0; i < sizeof(gtUserInfo)/sizeof(TYPE_USER_INFO); i++)
    {
        if (gtUserInfo[i].ucRole == USER_ROLE_CONFIG)
        {
            ucUserConfcnt++;        
        }
        if (gtUserInfo[i].ucRole == USER_ROLE_VIEW)
        {
            ucUserViewCnt++;        
        }
    }
    *pUserConfCnt	 = ucUserConfcnt;
    *pUserViewCnt = ucUserViewCnt;
    return TRUE;
}
BOOL Oam_GetUserInfo(TYPE_USER_INFO *pUserInfo)
{
    if (NULL == pUserInfo)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                   "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return FALSE;	
    }
    memcpy(pUserInfo,gtUserInfo,sizeof(gtUserInfo));
    return TRUE;
}
BOOL Oam_GetSpecifyIP(CHAR *pSpecifyIp)
{
    if (NULL == pSpecifyIp)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                   "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return FALSE;	
    }
    memset(pSpecifyIp, 0, LEN_IP_ADDR_STR);		
    memcpy(pSpecifyIp,g_szSpecifyIP,sizeof(g_szSpecifyIP));  
    return TRUE;
}
BOOL Oam_SetSpecifyIp(CHAR *pSpecifyIp)
{
    if (NULL == pSpecifyIp)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                   "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return FALSE;	
    }
    memset(g_szSpecifyIP, 0, LEN_IP_ADDR_STR);	
    memcpy(g_szSpecifyIP,pSpecifyIp,sizeof(g_szSpecifyIP));  
    return TRUE;
}
BOOL Oam_InnerCmdGetPtVarJidinfo(JID *pjidSelf)
{
    if (NULL == pjidSelf)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                   "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return FALSE;	
    }
    memset(pjidSelf, 0,sizeof(JID));	
    memcpy(pjidSelf,&(g_ptOamIntVar->jidSelf),sizeof(JID));  
    return TRUE;
}
BOOL Oam_GetCommonInfo(COMMON_INFO *pCommonInfo)
{
    if (NULL == pCommonInfo)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                   "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return FALSE;	
    }
    memset(pCommonInfo, 0, sizeof(COMMON_INFO));	
    memcpy(pCommonInfo,&gtCommonInfo,sizeof(COMMON_INFO));  
    return TRUE;
}
BOOL Oam_SetCommonInfo(COMMON_INFO *pCommonInfo)
{
    if (NULL == pCommonInfo)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                   "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return FALSE;	
    }
    memset(&gtCommonInfo, 0, sizeof(COMMON_INFO));	
    memcpy(&gtCommonInfo,pCommonInfo,sizeof(COMMON_INFO));  
    return TRUE;
}
BOOL Oam_GetPasswordIntensity(BYTE *pPasswordIntensity)
{
    if (NULL == pPasswordIntensity)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                   "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return FALSE;	
    }
    *pPasswordIntensity = gnPasswordIntensity;
    return TRUE;   
}
BOOL Oam_SetPasswordIntensity(BYTE *pPasswordIntensity)
{
    if (NULL == pPasswordIntensity)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                   "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return FALSE;	
    }
    gnPasswordIntensity = *pPasswordIntensity;
    return TRUE;   
}
BOOL Oam_GetNorPasslesslen(BYTE *pNorPasslesslen)
{
    if (NULL == pNorPasslesslen)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                   "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return FALSE;	
    }
    *pNorPasslesslen = gnNorPasslesslen;
    return TRUE;   
}	
BOOL Oam_SetNorPasslesslen(BYTE *pNorPasslesslen)
{
    if (NULL == pNorPasslesslen)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                   "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return FALSE;	
    }
    gnNorPasslesslen = *pNorPasslesslen;
    return TRUE;   
}

/**************************************************************************
* 函数名称：VOID OAM_CliChangeAllLinkToStartMode
* 功能描述：把所有连接的配置模式都切换到初始的用户模式
* 访问的表：无
* 修改的表：无
* 输入参数：pcReason:切换原因
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/02/12    V1.0    殷浩      创建
**************************************************************************/
void OAM_CliChangeAllLinkToStartMode(CHAR *pcReason)
{    
    WORD16 i = 0;
    WORD16 wLinkNum = sizeof(gtLinkState)/sizeof(TYPE_LINK_STATE);
    BYTE bCurModeId = 0;
    TYPE_LINK_STATE * pLinkState = NULL;
    TYPE_XMLLINKSTATE *pXmlLinkState = NULL;

    /*处理XML链路状态*/
    for (i = 0;i < MAX_SIZE_LINKSTATE;i++)
    {
        pXmlLinkState = Xml_GetLinkStateByIndex(i);
        if (NULL != pXmlLinkState)
        {
            if (XML_LINKSTATE_USED == pXmlLinkState->bUsed)
            {
                /*获取当前模式ID*/
                bCurModeId = XML_GetCurModeId(pXmlLinkState);
                while(bCurModeId > CMD_MODE_INVALID)
                {
                    /*遇到特权模式则退出*/
                    if (CMD_MODE_PRIV == bCurModeId)
                    {                    
                        break;
                    }
                    XML_LinkModeStack_Pop(pXmlLinkState);
                    bCurModeId = XML_GetCurModeId(pXmlLinkState);
                }     
                pXmlLinkState->bVerChanged = XML_LINKSTATE_VERCHANGED;          
            }
        }     
    }
    bCurModeId = 0;
      
    for (i = 0; i < wLinkNum; i++)
    {
        pLinkState = &gtLinkState[i];
        if (pec_MsgHandle_INIT < Oam_LinkGetCurRunState(pLinkState))
        {
            BOOLEAN bModeChanged = FALSE;
            bCurModeId = Oam_GetCurModeId(pLinkState);
            while(bCurModeId > CMD_MODE_INVALID)
            {
                /*611002035007 易用性改进六组（3）：cli>后需要输入en才能进入cli#*/
                if (/*CMD_MODE_USER*/CMD_MODE_PRIV == bCurModeId)
                {
                    ChangePrompt(pLinkState, bCurModeId);
                    pLinkState->bChangeMode = TRUE;
                     pLinkState->bIsSupperUsrMode = FALSE;
                    break;
                }
                bModeChanged = TRUE;
                Oam_LinkModeStack_Pop(pLinkState);
                bCurModeId = Oam_GetCurModeId(pLinkState);
            }            
			
            if (bModeChanged && (pcReason != NULL))
            {
                SendStringToTelnet(pLinkState, pcReason);
            }         
        }
    }
}

/**************************************************************************
* 函数名称： Oam_CfgIfNoLinkIsExecutingCmd
* 功能描述：检查是否没有连接在执行命令
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：TRUE-没有连接在执行命令 
                           FALSE - 有连接在执行命令
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/02/12    V1.0    殷浩      创建
**************************************************************************/
BOOLEAN Oam_CfgIfNoLinkIsExecutingCmd(void)
{
    WORD16 i = 0;    
    WORD16 wTelLinkNum = sizeof(gtLinkState)/sizeof(TYPE_LINK_STATE);
    WORD16 wXmlLinkNum = sizeof(gt_XmlLinkState)/sizeof(TYPE_XMLLINKSTATE);
    
    /*由于目前是单用户，不考虑有ping等长时间执行的命令
    导致dat一直不能切换的情况，但是xml要考虑*/
    for (i = 0; i < wTelLinkNum; i++)
    {
        if (pec_MsgHandle_EXECUTE == Oam_LinkGetCurRunState(&gtLinkState[i]))
        {
            return FALSE;
        }
    }
    for(i = 0;i < wXmlLinkNum;i++)
    {
        if (pec_MsgHandle_EXECUTE == XML_LinkGetCurRunState(&gt_XmlLinkState[i]))
        {
            return FALSE;
        }
    }

    return TRUE;
}
BOOL Oam_GetLangType(BYTE ucDatVerInfoNum,CHAR *aucLangType)
{
    /*入参判断*/
    if (NULL == aucLangType)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return FALSE;
    }
    if(ucDatVerInfoNum >=MAX_VER_INFO_NUM)
    {
        return FALSE;
    }
    memset(aucLangType,0,OAM_CLI_LANGTYPE_LEN);
    memcpy(aucLangType,g_ptOamIntVar->tDatVerInfo[ucDatVerInfoNum].tLang.aucLangType,OAM_CLI_LANGTYPE_LEN);
    return TRUE;
}
BOOL Oam_SetLangType(BYTE ucDatVerInfoNum,CHAR *aucLangType)
{
    /*入参判断*/
    if (NULL == aucLangType)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return FALSE;
    }
    if(ucDatVerInfoNum >=MAX_VER_INFO_NUM)
    {
        return FALSE;
    }
    memcpy(g_ptOamIntVar->tDatVerInfo[ucDatVerInfoNum].tLang.aucLangType,aucLangType,OAM_CLI_LANGTYPE_LEN);
    return TRUE;
}
BOOL Oam_GetVerFileName(BYTE ucDatVerInfoNum,CHAR *aucVerFileName)
{
    /*入参判断*/
    if (NULL == aucVerFileName)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return FALSE;
    }
    if(ucDatVerInfoNum >=MAX_VER_INFO_NUM)
    {
        return FALSE;
    }
    memset(aucVerFileName,0,FILE_NAME_LEN);
    memcpy(aucVerFileName,g_ptOamIntVar->tDatVerInfo[ucDatVerInfoNum].tFileInfo.acVerFileName,FILE_NAME_LEN);
    return TRUE;
}
BOOL Oam_SetVerFileName(BYTE ucDatVerInfoNum,CHAR *aucVerFileName)
{
    /*入参判断*/
    if (NULL == aucVerFileName)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return FALSE;
    }
    if(ucDatVerInfoNum >=MAX_VER_INFO_NUM)
    {
        return FALSE;
    }
    memcpy(g_ptOamIntVar->tDatVerInfo[ucDatVerInfoNum].tFileInfo.acVerFileName,aucVerFileName,FILE_NAME_LEN);
    return TRUE;
}
BOOL Oam_GetVerFilePath(BYTE ucDatVerInfoNum,CHAR *aucVerFilePath)
{
    /*入参判断*/
    if (NULL == aucVerFilePath)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return FALSE;
    }
    if(ucDatVerInfoNum >=MAX_VER_INFO_NUM)
    {
        return FALSE;
    }
    memset(aucVerFilePath,0,FILE_PATH_LEN);
    memcpy(aucVerFilePath, g_ptOamIntVar->tDatVerInfo[ucDatVerInfoNum].tFileInfo.acVerFilePath,FILE_PATH_LEN);
    return TRUE;
}
BOOL Oam_SetVerFilePath(BYTE ucDatVerInfoNum,CHAR *aucVerFilePath)
{
    /*入参判断*/
    if (NULL == aucVerFilePath)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return FALSE;
    }
    if(ucDatVerInfoNum >=MAX_VER_INFO_NUM)
    {
        return FALSE;
    }
    memcpy(g_ptOamIntVar->tDatVerInfo[ucDatVerInfoNum].tFileInfo.acVerFilePath,aucVerFilePath,FILE_PATH_LEN);
    return TRUE;
}
BOOL Oam_GetDatSum(WORD16 *wDatSum)
{
    /*入参判断*/
    if (NULL == wDatSum)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return FALSE;
    }
    *wDatSum = g_ptOamIntVar->wDatSum;
    return TRUE;
}
BOOL Oam_SetDatSum(WORD16 *wDatSum)
{   
    /*入参判断*/
    if (NULL == wDatSum)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return FALSE;
    }
    g_ptOamIntVar->wDatSum = *wDatSum;	
    return TRUE;
}
/*
向用户文件中配置cli脚本语言类型
*/
BOOLEAN OAM_CliSetLanguageType(CHAR *pStrLangType)
{
    /*入参判断*/
    if (NULL == pStrLangType)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return FALSE;
    }
    XOS_snprintf(g_aucScriptLanguage, sizeof(g_aucScriptLanguage), "%s", pStrLangType);
    WriteUserScriptFile();
    return TRUE;
}
/*
获取cli脚本语言类型
*/
BOOLEAN OAM_CliGetLanguageType(T_Cli_LangType *ptLangType)
{
    /*入参判断*/
    if (NULL == ptLangType)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return FALSE;
    }
    XOS_snprintf(ptLangType->aucLangType, sizeof(ptLangType->aucLangType), "%s", g_aucScriptLanguage);

    return TRUE;
}


/**************************************************************************
* 函数名称：CHAR *OAM_RTrim(CHAR *pStr)
* 功能描述：去除字符串右边的空格tab回车换行字符
* 访问的表：无
* 修改的表：无
* 输入参数：CHAR *pStr
* 输出参数：
* 返 回 值：null- 错误
            其他-去除后的新串
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2008/03/17   V1.0    殷浩      创建
**************************************************************************/
CHAR *OAM_RTrim(CHAR *pStr)
{
    CHAR *pRet = pStr;
    WORD16 wLen = 0;
    SWORD16 i = 0;
    if (NULL == pStr)/*输入的空串，直接返回*/
    {
        return NULL;
    }
    wLen = strlen(pStr);
    for(i = wLen - 1; i >= 0; i--)
    {
        if((*(pStr + i) == ' ') ||
                (*(pStr + i) == '\t') ||
                (*(pStr + i) == '\r') ||
                (*(pStr + i) == '\n'))
        {
            *(pRet + i) = '\0';
        }
        else
        {
            break;
        }
    }

    return pStr;
}

/**************************************************************************
* 函数名称：CHAR *OAM_LTrim(CHAR *pStr )
* 功能描述：去除字符串左边的空格tab回车换行字符
* 访问的表：无
* 修改的表：无
* 输入参数：CHAR *pStr
* 输出参数：
* 返 回 值：null- 错误
            其他-去除后的新串
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2008/03/17   V1.0    殷浩      创建
**************************************************************************/
CHAR *OAM_LTrim(CHAR *pStr )
{
    CHAR *pRet = pStr;
    WORD16 wLen = 0;
    SWORD16 i = 0;
    if (NULL == pStr)/*输入的空串，直接返回*/
    {
        return NULL;
    }
    wLen = strlen(pStr);
    for(i = 0; i < wLen; i++)
    {
        if((*(pStr + i) == ' ') ||
                (*(pStr + i) == '\t') ||
                (*(pStr + i) == '\r') ||
                (*(pStr + i) == '\n'))
        {
            pRet++;
        }
        else
        {
            break;
        }
    }

    return pRet;
}


