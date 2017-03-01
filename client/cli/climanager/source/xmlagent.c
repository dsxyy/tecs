#if 0
/*********************************************************************
* 版权所有 (C)2007, 深圳市中兴通讯股份有限公司。
*
* 文件名称： xmlagent.c
* 文件标识： 
* 其它说明： XML适配器模块 
* 当前版本： V1.0
* 作    者： 
* 完成日期： 
*
* 修改记录1：
*    修改日期：2008年9月10日
*    版 本 号：V1.0
*    修 改 人：
*    修改内容：创建
**********************************************************************/
#include "includes.h"
#include "xmlagent.h"
#include "oam_execute.h"
#include "convertencoding.h"
#include "rdtelnet.h"
#include <math.h>
#include <time.h> 
#include <string.h> 

/**************************************************************************
*                            常量                                        *
**************************************************************************/


/**************************************************************************
*                            宏                                          *
**************************************************************************/
#define MAX_SIZE_OMMMSG                                  (WORD16)(32*1024)
#define MAX_SIZE_OMMACKMSG                            (WORD16)(16*1024)
#define MAX_SIZE_OMMACKMSG_PLAN                        (WORD16)1024
#define MAX_SIZE_LINKCFGACKMSG                      (WORD16)(4*1026)
#define MAX_SIZE_OMMREGISTERREQ_BUFFER      (WORD16)516
#define MAX_SIZE_OMMREGISTERACK_BUFFER       (WORD16)1028
#define MAX_SIZE_OMMUNREGISTERREQ_BUFFER  (WORD16)516
#define MAX_SIZE_OMMUNREGISTERACK_BUFFER   (WORD16)1028
#define MAX_SIZE_OMMHANDSHAKEREQ_BUFFER    (WORD16)2
#define MAX_SIZE_OMMACKMSG_FRTBUFFER          (WORD16)512
#define MAX_SIZE_COMPONENTID                           (WORD16)2
#define XML_AGENT_OK                                          (WORD16)0  /*操作成功*/
#define XML_AGENT_ERR                                        (WORD16)1 /*操作失败*/
#define LINKCFG_OK                                               (WORD16)1 /*链路配置成功*/
#define LINKCFG_ERR                                               (WORD16)0 /*链路配置失败*/
#define NAME_T_NODE                                            "T"
#define NAME_D_NODE                                            "D"
#define NAME_OT_NODE                                           "OT"
#define NAME_OR_NODE                                           "OR"
#define NAME_DC_NODE                                            "DC"
#define NAME_EOF_NODE                                          "EOF"
#define NAME_MD_NODE                                           "MD"
#define NAME_RSTR_NODE                                      "RSTR"
#define TEXT_OT_NODE                                          "RP"
#define TEXT_DC_NODE                                          "0"
#define TEXT_OR_NODE_OK                                    "OK"
#define TEXT_OR_NODE_NOK                                  "NOK"
#define TEXT_OR_NODE_NS                                     "NS"
#define TEXT_EOF_NODE_Y                                     "Y"
#define TEXT_EOF_NODE_N                                     "N"
#define TEXT_RSTR_NODE_SUCCESS                       "Success"
#define NAME_TYPE_NODE                                      "TYPE"
#define NAME_TRM_NODE                                        "TRM"
#define NAME_ID_NODE                                           "ID"
#define NAME_ID_ATTR                                           "or"
#define NAME_ROOT_NODE_ACK                              "LINKCFGACK"
#define LINKCFG_NAME_LOGIN                                "Login"
#define LINKCFG_NAME_LOGOUT                             "Logout"
#define LINKCFG_NAME_HANDSHAKE                       "HandShake"
#define XMLMSGCFG_NAME_CEASECMDACK              "CEASECMDACK"
#define NAME_RLST_NODE                                       "RLST"
/**************************************************************************
*                          数据类型                                       *
**************************************************************************/

typedef struct tagT_TYPE_XMLMODEL
{
    WORD16 wComponentId;
    CHAR ucXML[1];
    CHAR ucPad[1];
}_PACKED_1_  TYPE_XMLMODEL;

/*lint -e749*/
typedef enum
{    
    RETURN_OK = 0, /*操作成功*/
    NULL_PTR,/*入参有空指针*/ 
    LOAD_XML_ERR,/*加载XML失败*/
    NEW_XML_ERR,/*生成XML失败*/
    GET_NODE_ERR,/*获得节点失败*/ 
    GET_NODE_TEXT_ERR,/*获取节点文本失败*/
    GET_NODE_CNT_ERR,/*获取节点个数*/
    GET_NODE_ATTR_ERR,/*获取属性节点失败*/
    GET_NODE_ARR_VALUE_ERR,/*获取属性节点值失败*/
    SET_NODE_ERR,/*设置节点失败*/
    SET_NODE_TEXT_ERR,/*设置节点文本失败*/
    SET_NODE_ATTR_ERR,/*设置属性节点失败*/
    SHOW_MODESTACK_ERR,/*查询模式栈信息错误*/
    SaveMutexTip,/*有链路正在存盘,不能执行命令*/
    DatUpdateMutex,/*当前正在切换版本,不能执行命令*/
    CmdExecutMutex,/*执行命令互斥*/
    NoRegXmlLinkState,/*未注册XML链路*/
    NO_EXECUTE_CMD_ON_LINKCHANNEL,/*链路上无命令执行*/
    GET_DOCTEXTERR,/*获取DOC文档文本错误*/
    RETURN_ERROR/*其他错误*/
}XML_AGENT_STAT;
/*lint -e749*/

/**************************************************************************
*                           全局变量                                      *
**************************************************************************/
extern BOOL bDebugPrint;
static JID gt_OpJid[MAX_SIZE_OPJID] = {{0}};
static CHAR pRecvOMMMsgBuf[MAX_SIZE_OMMMSG];/*OMM请求数据区*/
static CHAR pRecvPlatMsgBuf[MAX_APPMSG_LEN];/*接收平台应答请求*/
static DWORD  g_dwSeqNo = 1;/*发送序号*/
static XOS_TIMER_ID  wXML_OMM_HANDSHAKE;

/**************************************************************************
*                           本地变量                                      *
**************************************************************************/
/**************************************************************************
*                           外部函数原型                                  *
**************************************************************************/
/**************************************************************************
*                           局部函数原型                                  *
**************************************************************************/
static int Convert_atoi(CHAR * pString, WORD32 * pDword);
static VOID PrintMsgXml(TYPE_XMLMODEL *ptXmlModel,CHAR *pDesc);
static VOID ChangeXmlLinkStateMode(WORD16 wRtnCode,TYPE_XMLLINKSTATE *ptLinkState);
static BOOLEAN IsNeedInvertOrder(VOID);
static VOID Xml_ExecSaveCmd(TYPE_XMLLINKSTATE *ptLinkState);
static XML_AGENT_STAT CreateTSecNode(const LPXmlNode ptOriRtNode,const LPXmlNode ptDstRtNode);
static XML_AGENT_STAT CreateRSTRNode(const CHAR *pucRSTRText,const LPXmlNode ptCurNode);
static XML_AGENT_STAT CreateDSecNode(const CHAR *pucORText,
	                                                             const CHAR *pucEOFText,
	                                                             const CHAR *pucRSTRText,
	                                                             const LPXmlNode ptOriRtNode,
	                                                             const LPXmlNode ptDstRtNode);
static XML_AGENT_STAT CreateDstXmlDoc(const CHAR *pucORText,
                                                                    const CHAR *pucEOFText,
                                                                    const CHAR *pucRSTRText,
                                                                    const LPXmlNode pOriRtNode,
                                                                    XmlDocPtr *pptDstXmlDoc);
static XML_AGENT_STAT ConstructAckMsg2OMM(const CHAR *pucORText,
                                                                                const CHAR *pucEOFText,
                                                                                const CHAR *pucRSTRText,
                                                                                const CHAR *pucComponentId,
                                                                                const LPXmlNode pOriRtNode,
                                                                                WORD32 dwMsgId,
                                                                                JID *ptOmmJid);
static XML_AGENT_STAT SendAckMsg2OMM(const CHAR *pucMsg,
                                    WORD32 dwMsgId,
                                    JID *ptOmmJid);
static CHAR *GetRSTRText(WORD32 dwCode);
static VOID ProcErrPlatCfgMsg(const CHAR *ptrComponentId,const LPXmlNode ptOriRtNode,const CHAR *ptrOutputResult,JID *ptDtJid);
static BOOLEAN Xml_ConstructTelnetSaveCmd(MSG_COMM_OAM *ptSendMsg,TYPE_TELNET_MSGHEAD  *pTelMsg);
static VOID Oam_InitSaveLink(VOID);
static XML_AGENT_STAT SetOMMHandShakeAck(OMMHandShankReq *ptOMMHandShankReq,OMMHandShankAck *ptOMMHandShankAck);
static XML_AGENT_STAT LinkCfg_LoginProcess(BYTE *pucComponentId,LPXmlNode ptNodeIn);
static XML_AGENT_STAT LinkCfg_LogoutProcess(BYTE *pucComponentId,LPXmlNode ptNodeIn);
static XML_AGENT_STAT LinkCfg_HandShakeProcess(BYTE *pucComponentId,LPXmlNode ptNodeIn);
static BOOLEAN LoginTrmId(WORD32 dwTrmId);
static BOOLEAN LogOutTrmId(WORD32 dwTrmId);
static XML_AGENT_STAT GetNodeText(LPXmlNode ptrNodeIn,CHAR *strNodeName,WORD32 dwNodeIndex,WORD32 wNodeTextLen,CHAR *strNodeText);
static XML_AGENT_STAT RegisterTrmId(WORD16 wLinkChannel);
static XML_AGENT_STAT UnRegisterTrmId(WORD16 wLinkChannel);
static XML_AGENT_STAT XML_ParaseDoc(CHAR *xml,WORD32 len,int options,XmlDocPtr *ppXmlDocPtr,LPXmlNode ptRoot);
static XML_AGENT_STAT GetChlidByNode(LPXmlNode ptrNodeIn,CHAR *ptrNodeName,WORD32 dwNodeIndex,LPXmlNode ptrNodeOut);
static XML_AGENT_STAT GetChildByName(LPXmlNode ptrNodeIn,CHAR *ptrSecName,CHAR *ptrNodeName,WORD32 dwNodeIndex,LPXmlNode ptrNodeOut);
static XML_AGENT_STAT GetNodeAttrValueByName(LPXmlNode ptrNode,CHAR *ptrAttrName,WORD16 wLen,CHAR * ptrAttrBuf);
static XML_AGENT_STAT GetChildTextByNode(LPXmlNode ptrNode,WORD16 wLen,CHAR *ptrNodeText_Buf);
static XML_AGENT_STAT GetChildCountByName(LPXmlNode ptrNode,CHAR *ptrNodeName,WORD32 *dwNumber);
static XML_AGENT_STAT GetTextByName(CHAR *ptrHeadName,CHAR *ptrlEndName,WORD32 dwLength,CHAR *ptrInBuf,CHAR *ptrOutBuf);
static XML_AGENT_STAT GetDeclTextByName(CHAR *ptrXmlBuf,CHAR *ptrDeclBuf);
static XML_AGENT_STAT GetTSectionTextByName(CHAR *ptrXmlBuf,CHAR *ptrSecBuf);
static XML_AGENT_STAT GetCMDNode(LPXmlNode ptrNodeIn,LPXmlNode ptrNodeOut);
static XML_AGENT_STAT GetNIDNode(LPXmlNode ptrNodeIn,LPXmlNode ptrNodeOut);
static XML_AGENT_STAT GetOTNode(LPXmlNode ptrNodeIn,LPXmlNode ptrNodeOut);
static XML_AGENT_STAT GetNTPNode(LPXmlNode ptrNodeIn,LPXmlNode ptrNodeOut);
static XML_AGENT_STAT GetTRMNode(LPXmlNode ptrNodeIn,LPXmlNode ptrNodeOut);
static XML_AGENT_STAT GetPARANode(LPXmlNode ptrNodeIn,WORD32 dwNodeIndex,LPXmlNode ptrNodeOut);
static XML_AGENT_STAT GetCMDNodeAttr_ID(LPXmlNode ptrNode,WORD16 wLen,CHAR *ptrNodeAttr_Buf);
static XML_AGENT_STAT SetCMDNodeAttr_ID(LPXmlNode ptrNode,DWORD *dwCmdId);
static XML_AGENT_STAT GetParaNodeAttr_ID(LPXmlNode ptrNode,WORD32 dwNumber,WORD16 wLen,CHAR *ptrNodeAttr_Buf);
static XML_AGENT_STAT SetParaNodeAttr_ID(LPXmlNode ptrNode,BYTE bNumber,BYTE *bParaNo);
static XML_AGENT_STAT GetTRMNodeText(LPXmlNode ptrNode,WORD16 wLen,CHAR *ptrNodeText_Buf);
static XML_AGENT_STAT SetTRMNodeText(LPXmlNode ptrNode,WORD32 *pdwLinkChannel);
static XML_AGENT_STAT GetOTNodeText(LPXmlNode ptrNode,WORD16 wLen,CHAR *ptrNodeText_Buf);
static XML_AGENT_STAT GetNIDNodeText(LPXmlNode ptrNode,WORD16 wLen,CHAR *ptrNodeText_Buf);
static XML_AGENT_STAT SetOTNodeText(LPXmlNode ptrNode,BYTE *pucIfNo);
static XML_AGENT_STAT GetCMDParaNodeCount(LPXmlNode ptrNode,BYTE *pucNumber);
static INT ConvertXmlStrToCmdPara(TYPE_CUR_CMDINFO *ptCurCMDInfo,CHAR * ptrNodeText,WORD32 dwParaNo,OPR_DATA * ptrData);
static XML_AGENT_STAT ParaseValidPrammer(TYPE_CMD_PARA *ptCmdParaInfo,TYPE_CMD_ATTR *ptCmdAttr,CHAR *ptrNodeText,BOOLEAN *pbIsValidParmar,OPR_DATA * ptrData);
static VOID ConvertValidPrammerToCmdPara(TYPE_CMD_PARA *ptCmdParaInfo,BYTE bParaType,WORD16 wValue,CMDPARA *ptCmdPara);
static BOOLEAN CheckIsValidPrammar(TYPE_CMD_PARA *ptTypeCmdPara,TYPE_CMD_ATTR *ptCmdAttr);
static XML_AGENT_STAT GetCurTypeCmdPara(TYPE_CUR_CMDINFO *ptCurCMDInfo,DWORD dwParaNo,TYPE_CMD_PARA *ptTypeCmdPara);
static XML_AGENT_STAT GetParaNodeText(LPXmlNode ptrNode,WORD32 dwParaNo,WORD16 wLen,CHAR *ptrNodeText_Buf);
static XML_AGENT_STAT SetParaNodeAttr(TYPE_CUR_CMDINFO *ptCurCMDInfo,LPXmlNode ptrNode,BYTE bParaNo,OPR_DATA * pData);
static XML_AGENT_STAT CompareChildTextByName(LPXmlNode ptrNode,CHAR *ptrCompText,BOOLEAN *pbIs3GPL);
static XML_AGENT_STAT CheckNtpNodeTextIs3GPL(LPXmlNode ptrNodeIn,BOOLEAN *pbIs3GPL);
static VOID SendOMMReqToPlat(TYPE_XMLLINKSTATE *ptLinkState,LPXmlNode ptrNodeNode);
static VOID SendOMMReqToOP(CHAR *ptrOMMReq,WORD16 wMsgLen,LPXmlNode ptrNodeNode);
static VOID SendOPAckToOMM(CHAR *ptrOPAck,WORD16 wMsgLen,LPXmlNode ptRootNode);
static XML_AGENT_STAT CeaseCmdReqProc(WORD32 dwTrmId);
static XML_AGENT_STAT ProcCeaseCmd(const LPXmlNode ptOriRtNode);
static CHAR* XML_GetTextForUTF(XmlDocPtr pXmlDocPtr,const CHAR *pucComponentId);
static XML_AGENT_STAT ReseachTree(WORD16 wNodePos,DWORD dwCmdId,WORD16 **ppOutNodePos);
static XML_AGENT_STAT GetTreeNode(BYTE bModeId,DWORD dwCmdId,WORD16 *pCmdAttrPos);
static XML_AGENT_STAT SetXmlLinkState(TYPE_XMLMODEL *ptXmlModel,LPXmlNode ptrNode,TYPE_XMLLINKSTATE **pptLinkState);
static XML_AGENT_STAT XmlMgt2OamMgt(LPXmlNode ptrNode,TYPE_XMLLINKSTATE *ptLinkState,MSG_COMM_OAM *ptrMsgBuf);
static XML_AGENT_STAT OamMgt2XmlMgt(MSG_COMM_OAM *ptrMsgBuf,TYPE_XMLLINKSTATE *ptLinkState,WORD16 wOMMMsgBufLen,WORD16 *pwMsgLen,CHAR *ptrOMMMsgBuf);
static XML_AGENT_STAT PlanMgt2XmlMgt(T_Cmd_Plan_Cfg *ptrMsgBuf,TYPE_XMLLINKSTATE *ptLinkState,WORD16 *pwMsgLen,CHAR *ptrOMMMsgBuf);
static XML_AGENT_STAT SetOTNode(CHAR *ptrXmlBuf,WORD16 wXmlBufLen);
static XML_AGENT_STAT SetORNode(MSG_COMM_OAM *ptrMsgBuf,CHAR *ptrXmlBuf,WORD16 wXmlBufLen);
static XML_AGENT_STAT SetORNodeForPlan(CHAR *ptrXmlBuf);
static XML_AGENT_STAT SetDCNode(MSG_COMM_OAM *ptrMsgBuf,CHAR *ptrXmlBuf,WORD16 wXmlBufLen);
static XML_AGENT_STAT SetDCNodeForPlan(CHAR *ptrXmlBuf);
static XML_AGENT_STAT SetEOFNode(MSG_COMM_OAM *ptrMsgBuf,CHAR *ptrXmlBuf,WORD16 wXmlBufLen);
static XML_AGENT_STAT SetEOFNodeForPlan(CHAR *ptrXmlBuf);
static XML_AGENT_STAT SetMDNode(CHAR *ptrXmlBuf,WORD16 wXmlBufLen);
static XML_AGENT_STAT SetMDNodeForPlan(T_Cmd_Plan_Cfg *ptrMsgBuf,TYPE_XMLLINKSTATE *ptLinkState,CHAR *ptrXmlBuf);
static XML_AGENT_STAT SetPGNodeForPlan(T_Cmd_Plan_Cfg *ptrMsgBuf,TYPE_XMLLINKSTATE *ptLinkState,CHAR *ptrXmlBuf);
static XML_AGENT_STAT GetOpJid(LPXmlNode ptrNodeIn,JID *ptJid);
static XML_AGENT_STAT GetOMMJid(LPXmlNode ptrNodeIn);
static XML_AGENT_STAT SetDSecText(MSG_COMM_OAM *ptrMsgBuf,CHAR *ptrXmlBuf,WORD16 wXmlBufLen);
static XML_AGENT_STAT SetDSecTextForPlan(T_Cmd_Plan_Cfg *ptrMsgBuf,TYPE_XMLLINKSTATE *ptLinkState,CHAR *ptrXmlBuf);
static XML_AGENT_STAT SetCurCMDInfo(TYPE_XMLLINKSTATE *ptLinkState);
static VOID Xml_SetHandShakeTimer(VOID);
static VOID Xml_KillHandShakeTimer(VOID);

/**************************************************************************
* 函数名称：VOID Convert_atoi
* 功能描述：类型转换String->WORD32
* 访问的表：无
* 修改的表：无
* 输入参数：pString
* 输出参数：pDword
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/03/05    V1.0    齐龙兆      创建
**************************************************************************/
static int Convert_atoi(CHAR * pString, WORD32 * pDword)
{
    CHAR *pChar = pString;
    WORD32 dVal = 0;
    int nDigit = 0;
    int nLength = 0;

    *pDword = 0;
    nLength = strlen(pString);
    if (nLength == 0)
        return -1;

    nLength = 0;

    while (pChar != NULL && *pChar != '\0')
    {
        /* not a digit */
        if (*pChar < '0' || *pChar > '9')
            return -1;

        nDigit = *pChar - '0';

        /* overflow */
        if (dVal > (0xffffffff - nDigit) / 10)
            return -1;
        dVal = dVal * 10 + nDigit;
        if (dVal > 0)
            nLength++;

        /* too many digits */
        if (nLength > 10)
            return -1;
        pChar++;
    }

    *pDword = dVal;
    return 0;
}

/**************************************************************************
* 函数名称：VOID PrintMsgXml
* 功能描述：打印XML消息
* 访问的表：无
* 修改的表：无
* 输入参数：CHAR *pXmlBuf
                             CHAR *pDesc
* 输出参数：无
* 返 回 值：    无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/03/05    V1.0    齐龙兆      创建
**************************************************************************/
static VOID PrintMsgXml(TYPE_XMLMODEL *ptXmlModel,CHAR *pDesc)
{
    if((NULL == ptXmlModel) || (NULL == ptXmlModel->ucXML) ||(NULL == pDesc))
    {
        return;
    }
    printf("\n<%s>\n",pDesc);
    printf("ComponentId=%d\n",ptXmlModel->wComponentId);
    printf("%s",ptXmlModel->ucXML);
    printf("\n</%s>\n",pDesc);
}

/**************************************************************************
* 函数名称：VOID ChangeXmlLinkStateMode
* 功能描述：改变链路信息模式栈
* 访问的表：无
* 修改的表：无
* 输入参数：WORD16 wRtnCode
* 输出参数：TYPE_XMLLINKSTATE *ptLinkState
* 返 回 值：    无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/03/05    V1.0    齐龙兆      创建
**************************************************************************/
static VOID ChangeXmlLinkStateMode(WORD16 wRtnCode,TYPE_XMLLINKSTATE *ptLinkState)
{
    assert(ptLinkState);
    if (NULL == ptLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC: %s,Line:%d Null PTR!....................",__func__,__LINE__);
        return;
    }
    if (SUCC_AND_NOPARA == wRtnCode)/*只有返回结果正确才改变模式栈*/
    {
        XML_ChangeMode(ptLinkState);
    }
    return;
}

/**************************************************************************
* 函数名称：VOID Xml_SetHandShakeTimer
* 功能描述：设置平台与网管握手定时器
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：    无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/03/05    V1.0    齐龙兆      创建
**************************************************************************/
static VOID Xml_SetHandShakeTimer(VOID)
{
    wXML_OMM_HANDSHAKE = XOS_SetRelativeTimer(TIMER_XML_OMM_HANDSHAKE, DURATION_XML_OMM_HANDSHAKE,1);
    return;
}

/**************************************************************************
* 函数名称：VOID Xml_KillHandShakeTimer
* 功能描述：杀掉平台与网管握手定时器
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/03/05    V1.0    齐龙兆      创建
**************************************************************************/
static VOID Xml_KillHandShakeTimer(VOID)
{
    if (INVALID_TIMER_ID != wXML_OMM_HANDSHAKE)
    {
        XOS_KillTimerByTimerId(wXML_OMM_HANDSHAKE);
        wXML_OMM_HANDSHAKE = INVALID_TIMER_ID;
    }
    return;
}

/**************************************************************************
* 函数名称：BOOL IsNeedInvertOrder
* 功能描述：判断是否需要字节序转换
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
static BOOLEAN IsNeedInvertOrder(VOID)
{
    WORD16 wOrderTest = 0x1234;
    if (0x12 == *(BYTE*)&wOrderTest)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/**********************************************************************
* 函数名称：XML_AGENT_STAT  SetOMMHandShakeAck
* 功能描述：设置链路握手应答消息
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：   RETURN_ERROR-失败 RETURN_OK-成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT SetOMMHandShakeAck(OMMHandShankReq *ptOMMHandShankReq,OMMHandShankAck *ptOMMHandShankAck)
{
    OMMHandShankReq *ptOMMHandShankReqTmp = NULL;
    TYPE_XMLLINKSTATE *ptXmlLinkState = NULL;
    WORD16 wLoop = 0;
    WORD16 wTRMNum = 0;
    WORD16 wTRMIndex = 0;
    /*参数检查*/
    if ((NULL == ptOMMHandShankReq) || (NULL == ptOMMHandShankAck))
    {
        return NULL_PTR;
    }
    ptOMMHandShankReqTmp = ptOMMHandShankReq;	
    for (wLoop = 0;wLoop < MAX_SIZE_LINKSTATE;wLoop++)
    {
        ptXmlLinkState = Xml_GetLinkStateByIndex(wLoop);
	 if (NULL == ptXmlLinkState)
	 {
	     continue;
	 }
        if (XML_LINKSTATE_USED == ptXmlLinkState->bUsed)
        {
            wTRMNum++;
	     ptOMMHandShankAck->aucTRMId[wTRMIndex] = (WORD16)ptXmlLinkState->dwLinkChannel;
	     wTRMIndex++;
	     if (wTRMIndex >= MAX_SIZE_LINKSTATE)
	     {
	         return RETURN_ERROR;
	     }
        }
    }
    ptOMMHandShankAck->wComponentId = ptOMMHandShankReqTmp->wComponentId;
    ptOMMHandShankAck->wTRMNum = wTRMNum;
    return RETURN_OK;
}

/**********************************************************************
* 函数名称：XML_AGENT_STAT LinkCfg_LoginProcess
* 功能描述：链路配置-注册
* 访问的表：无
* 修改的表：无
* 输入参数：strComponentId-组件号
                             ptCurNode-当前节点
* 输出参数：无
* 返 回 值：   XML_AGENT_STAT
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2009-11-25             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT LinkCfg_LoginProcess(BYTE *pucComponentId,LPXmlNode ptNodeIn)
{
    INT iRet = RETURN_OK;                                   /*返回值*/
    CHAR *pOMMMsgBuf = NULL;                          /*OMM应答消息体*/
    WORD16 wMsgLen = 0;
    WORD32 dwLoop = 0;
    WORD32 dwTrmCnt = 0;                                 /*链路个数*/
    WORD32 dwTrmId = 0;
    JID tDtJid;
    LPXmlNode ptNodeTmp = NULL;
    XmlNode tTrmNode;                                       /*Trm节点*/
    XmlNode tChildNode;                                      /*子节点*/
    XmlNode tCurNode;                                        /*当前节点*/
    XmlDocPtr ptrDoc = NULL;                               /*xml文档指针*/
    TYPE_XMLMODEL *ptXmlModel = NULL;
    
    /*入参检查*/
    if ((NULL == pucComponentId) || (NULL == ptNodeIn))
    {
        return NULL_PTR;
    }
    ptNodeTmp = ptNodeIn;
    memset(&tTrmNode,0,sizeof(tTrmNode));
    memset(&tChildNode,0,sizeof(tChildNode));
    memset(&tCurNode,0,sizeof(tCurNode));
    memset(&tDtJid,0,sizeof(tDtJid));
    iRet = XOS_Sender(&tDtJid);
    /*获取发送端JID*/
    if (XOS_SUCCESS != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                               "....................FUNC:%s LINE:%d XOS_Sender Error,Error code %d! ....................",__func__,__LINE__,iRet);
	 return RETURN_ERROR;
    }
    /*生成XML文本*/
    iRet = XML_NewDoc(NAME_ROOT_NODE_ACK,&ptrDoc);
    if (XML_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_NewDoc Error,Error code %d! ....................",__func__,__LINE__,iRet);
	 return iRet;
    }
    /*获取根节点*/
    iRet = XML_GetRootNode(ptrDoc,&tCurNode);
    if (XML_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_GetRootNode Error,Error code %d! ....................",__func__,__LINE__,iRet);
        XML_FreeXml(ptrDoc);
	 return iRet;
    }
    /*插入<Type>节点*/
    iRet = XML_InsertChild(&tCurNode,0,NAME_TYPE_NODE,&tChildNode);
    if (XML_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_InsertChild Error,Error code %d! ....................",__func__,__LINE__,iRet);
	 XML_FreeXml(ptrDoc);
	 return iRet;
    }
    /*设置<Type>节点文本*/
    iRet = XML_SetChildText(&tChildNode,LINKCFG_NAME_LOGIN);
    if (XML_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_InsertChild Error,Error code %d! ....................",__func__,__LINE__,iRet);
	 XML_FreeXml(ptrDoc);
	 return iRet;
    }
    /*插入<Trm>节点*/
    iRet = XML_InsertChild(&tCurNode,1,NAME_TRM_NODE,&tChildNode);
    if (XML_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_InsertChild Error,Error code %d! ....................",__func__,__LINE__,iRet);
	 XML_FreeXml(ptrDoc);
	 return iRet;
    }
    /*获取<Trm>节点*/
    iRet = GetChlidByNode(ptNodeTmp,NAME_TRM_NODE,0,&tTrmNode);
    if(RETURN_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d GetChlidByNode Error,Error code %d! ....................",__func__,__LINE__,iRet);
        XML_FreeXml(ptrDoc);
        return iRet;
    }
    /*获取<Trm>子节点个数*/
    iRet = GetChildCountByName(&tTrmNode,NAME_ID_NODE,&dwTrmCnt);
    if(RETURN_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d GetChildCountByName Error,Error code %d! ....................",__func__,__LINE__,iRet);
        XML_FreeXml(ptrDoc);
        return iRet;
    }
    /*遍历注册TRM ID*/
    for(dwLoop = 0;dwLoop < dwTrmCnt;dwLoop++)
    {
        CHAR aucNodeText[MAX_NODETEXT_BUF_SIZE] = {0};/*节点文本*/
        XmlNode tIdNode;
        memset(&tIdNode,0,sizeof(tIdNode));
        /*获取<id>节点文本*/
        iRet = GetNodeText(&tTrmNode, NAME_ID_NODE,dwLoop, sizeof(aucNodeText) - 1, aucNodeText);
        if(RETURN_OK != iRet)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d GetNodeText Error,Error code %d! ....................",__func__,__LINE__,iRet);
            continue;
        }
        iRet = Convert_atoi(aucNodeText,&dwTrmId);
        if(0 != iRet)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d Convert_atoi Error,Error code %d! ....................",__func__,__LINE__,iRet);
            continue;
        }
        /*插入<id>节点*/
        iRet = XML_InsertChild(&tChildNode,dwLoop,NAME_ID_NODE,&tIdNode);
	 if (XML_OK != iRet)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR,
                               "....................FUNC:%s LINE:%d XML_InsertChild Error,Error code %d! ....................",__func__,__LINE__,iRet);
	     continue;
        }
	 /*设置<ID>节点文本*/
	 iRet = XML_SetChildText(&tIdNode,aucNodeText);
	 if(XML_OK != iRet)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR,
                               "....................FUNC:%s LINE:%d XML_SetChildText Error,Error code %d! ....................",__func__,__LINE__,iRet);
	     continue;
        }
	 memset(aucNodeText,0,sizeof(aucNodeText));
	 /*注册TRM ID*/
        if (!LoginTrmId(dwTrmId))
        {
            snprintf(aucNodeText,sizeof(aucNodeText),"%d",LINKCFG_ERR);
        }
        else
        {
            snprintf(aucNodeText,sizeof(aucNodeText),"%d",LINKCFG_OK);            
        }
        iRet = XML_SetAttribute(&tIdNode,NAME_ID_ATTR,aucNodeText);
        if(XML_OK != iRet)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR,
                               "....................FUNC:%s LINE:%d XML_SetAttribute Error,Error code %d! ....................",__func__,__LINE__,iRet);
	     continue;
        }
    }
    pOMMMsgBuf = XML_GetTextForUTF(ptrDoc, (const CHAR *)pucComponentId);
    /*获取文本失败*/
    if (NULL == pOMMMsgBuf)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XOS_GetUB Error! ....................",__func__,__LINE__);
        XML_FreeXml(ptrDoc);
        return NULL_PTR;
    }
    ptXmlModel = (TYPE_XMLMODEL *)pOMMMsgBuf;
    wMsgLen = sizeof(ptXmlModel->wComponentId) + strlen(ptXmlModel->ucXML) + 1;
    /*消息打印*/
    if (bDebugPrint)
    {
        PrintMsgXml(ptXmlModel, "MSG_XMLAGNET_TO_OMM");
    }
    iRet = XOS_SendAsynMsg(EV_XML_LINKCFG_ACK,
                            (BYTE *)pOMMMsgBuf,
                            wMsgLen, 
                            0, 
                            0, 
                            &tDtJid);
    /*向网管返回链路注册应答失败*/
    if (XOS_SUCCESS != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                               "....................FUNC:%s LINE:%d XOS_SendAsynMsg Error,Error code %d! ....................",__func__,__LINE__,iRet);	 
    }
    /*释放资源*/
    OAM_RETUB(pOMMMsgBuf);
    XML_FreeXml(ptrDoc);
    return iRet;
}

/**********************************************************************
* 函数名称：XML_AGENT_STAT LinkCfg_LoginProcess
* 功能描述：链路配置-注销
* 访问的表：无
* 修改的表：无
* 输入参数：strComponentId-组件号
                             ptCurNode-当前节点
* 输出参数：无
* 返 回 值：   XML_AGENT_STAT
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2009-11-25             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT LinkCfg_LogoutProcess(BYTE *pucComponentId,LPXmlNode ptNodeIn)
{
    INT iRet = RETURN_OK;                                   /*返回值*/
    CHAR *pOMMMsgBuf = NULL;                          /*OMM应答消息体*/
    WORD16 wMsgLen = 0;
    WORD32 dwLoop = 0;
    WORD32 dwTrmCnt = 0;                                 /*链路个数*/
    WORD32 dwTrmId = 0;
    JID tDtJid;
    LPXmlNode ptNodeTmp = NULL;
    XmlNode tTrmNode;                              /*Trm节点*/
    XmlNode tChildNode;                             /*子节点*/
    XmlNode tCurNode;                               /*当前节点*/
    XmlDocPtr ptrDoc = NULL;                               /*xml文档指针*/
    TYPE_XMLMODEL *ptXmlModel = NULL;
    
    /*入参检查*/
    if ((NULL == pucComponentId) || (NULL == ptNodeIn))
    {
        return NULL_PTR;
    }
    ptNodeTmp = ptNodeIn;
    memset(&tTrmNode,0,sizeof(tTrmNode));
    memset(&tChildNode,0,sizeof(tChildNode));
    memset(&tCurNode,0,sizeof(tCurNode));
    memset(&tDtJid,0,sizeof(tDtJid));
    iRet = XOS_Sender(&tDtJid);
    /*获取发送端JID失败*/
    if (XOS_SUCCESS != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                               "....................FUNC:%s LINE:%d XOS_Sender Error,Error code %d! ....................",__func__,__LINE__,iRet);
	 return iRet;
    }
    /*生成XML文本*/
    iRet = XML_NewDoc(NAME_ROOT_NODE_ACK,&ptrDoc);
    if (XML_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_NewDoc Error,Error code %d! ....................",__func__,__LINE__,iRet);
	 return iRet;
    }
    /*获取根节点*/
    iRet = XML_GetRootNode(ptrDoc,&tCurNode);
    if (XML_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_GetRootNode Error,Error code %d! ....................",__func__,__LINE__,iRet);
        XML_FreeXml(ptrDoc);
	 return iRet;
    }
    /*插入<Type>节点*/
    iRet = XML_InsertChild(&tCurNode,0,NAME_TYPE_NODE,&tChildNode);
    if (XML_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_InsertChild Error,Error code %d! ....................",__func__,__LINE__,iRet);
	 XML_FreeXml(ptrDoc);
	 return iRet;
    }
    /*设置<Type>节点文本*/
    iRet = XML_SetChildText(&tChildNode,LINKCFG_NAME_LOGOUT);
    if (XML_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_InsertChild Error,Error code %d! ....................",__func__,__LINE__,iRet);
	 XML_FreeXml(ptrDoc);
	 return iRet;
    }
    /*插入<Trm>节点*/
    iRet = XML_InsertChild(&tCurNode,1,NAME_TRM_NODE,&tChildNode);
    if (XML_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_InsertChild Error,Error code %d! ....................",__func__,__LINE__,iRet);
	 XML_FreeXml(ptrDoc);
	 return iRet;
    }
    /*获取<Trm>节点*/
    iRet = GetChlidByNode(ptNodeTmp,NAME_TRM_NODE,0,&tTrmNode);
    if(RETURN_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d GetChlidByNode Error,Error code %d! ....................",__func__,__LINE__,iRet);
	 XML_FreeXml(ptrDoc);
        return iRet;
    }
    /*获取<Trm>子节点个数*/
    iRet = GetChildCountByName(&tTrmNode,NAME_ID_NODE,&dwTrmCnt);
    if(RETURN_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d GetChildCountByName Error,Error code %d! ....................",__func__,__LINE__,iRet);
	 XML_FreeXml(ptrDoc);
        return iRet;
    }
    /*遍历注销TRM ID*/
    for(dwLoop = 0;dwLoop < dwTrmCnt;dwLoop++)
    {
        CHAR aucNodeText[MAX_NODETEXT_BUF_SIZE] = {0};/*节点文本*/
        XmlNode tIdNode;
        memset(&tIdNode,0,sizeof(tIdNode));
        /*获取<id>节点文本*/
        iRet = GetNodeText(&tTrmNode, NAME_ID_NODE,dwLoop, sizeof(aucNodeText) - 1, aucNodeText);
        if(RETURN_OK != iRet)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d GetNodeText Error,Error code %d! ....................",__func__,__LINE__,iRet);
            continue;
        }
        iRet = Convert_atoi(aucNodeText,&dwTrmId);
        if(0 != iRet)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d Convert_atoi Error,Error code %d! ....................",__func__,__LINE__,iRet);
            continue;
        }
        /*插入<id>节点*/
        iRet = XML_InsertChild(&tChildNode,dwLoop,NAME_ID_NODE,&tIdNode);
	 if (XML_OK != iRet)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR,
                               "....................FUNC:%s LINE:%d XML_InsertChild Error,Error code %d! ....................",__func__,__LINE__,iRet);
	     continue;
        }
	 /*设置<ID>节点文本*/
	 iRet = XML_SetChildText(&tIdNode,aucNodeText);
	 if(XML_OK != iRet)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR,
                               "....................FUNC:%s LINE:%d XML_SetChildText Error,Error code %d! ....................",__func__,__LINE__,iRet);
	     continue;
        }
	 memset(aucNodeText,0,sizeof(aucNodeText));
	 /*注册TRM ID*/
        if (!LogOutTrmId(dwTrmId))
        {
            snprintf(aucNodeText,sizeof(aucNodeText),"%d",LINKCFG_ERR);
        }
        else
        {
            snprintf(aucNodeText,sizeof(aucNodeText),"%d",LINKCFG_OK);            
        }
        iRet = XML_SetAttribute(&tIdNode,NAME_ID_ATTR,aucNodeText);
        if(XML_OK != iRet)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR,
                               "....................FUNC:%s LINE:%d XML_SetAttribute Error,Error code %d! ....................",__func__,__LINE__,iRet);
	     continue;
        }
    }
    pOMMMsgBuf = XML_GetTextForUTF(ptrDoc, (const CHAR *)pucComponentId);
    /*获取文本失败*/
    if (NULL == pOMMMsgBuf)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XOS_GetUB Error! ....................",__func__,__LINE__);
        XML_FreeXml(ptrDoc);
        return NULL_PTR;
    }
    ptXmlModel = (TYPE_XMLMODEL *)pOMMMsgBuf;
    wMsgLen = sizeof(ptXmlModel->wComponentId) + strlen(ptXmlModel->ucXML) + 1;
    /*消息打印*/
    if (bDebugPrint)
    {
        PrintMsgXml(ptXmlModel, "MSG_XMLAGNET_TO_OMM");
    }
    iRet = XOS_SendAsynMsg(EV_XML_LINKCFG_ACK,
                            (BYTE *)pOMMMsgBuf,
                            wMsgLen, 
                            0, 
                            0, 
                            &tDtJid);
    /*向网管返回链路注册应答失败*/
    if (XOS_SUCCESS != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                               "....................FUNC:%s LINE:%d XOS_SendAsynMsg Error,Error code %d! ....................",__func__,__LINE__,iRet);	 
    }
    /*释放资源*/
    OAM_RETUB(pOMMMsgBuf);
    XML_FreeXml(ptrDoc);
    return iRet;
}

VOID Oam_DbgLinkCfgGetNodeText(VOID)
{
    CHAR aucNodeText[MAX_NODETEXT_BUF_SIZE] = {0};
    CHAR aucXmlBuf[] = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?><LINKCFGREQ><TYPE>HandShake</TYPE></LINKCFGREQ>";
    INT iRet = 0;
    INT iOptions = XML_STAT_PARSE_NOBLANKS;
    XmlNode tRootNode;
    XmlDocPtr ptrDoc = NULL;
    WORD32 dwXmlLen = strlen(aucXmlBuf) + 1;
    memset(&tRootNode,0,sizeof(tRootNode));
    /*解析XML文档并返回根节点*/
    iRet = XML_ParaseDoc(aucXmlBuf,dwXmlLen,iOptions,&ptrDoc,&tRootNode);
    if (RETURN_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_ParaseDoc Error,Error code %d! ....................",__func__,__LINE__,iRet);        
	 return ;
    }
    iRet = GetNodeText(&tRootNode, NAME_TYPE_NODE,0,sizeof(aucNodeText)-1, aucNodeText);
    if (RETURN_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d GetNodeText Error,Error code %d! ....................",__func__,__LINE__,iRet);        
    }
    XML_FreeXml(ptrDoc);
}

VOID Oam_DbgShowMapValue(VOID)
{
    CHAR *pStr = NULL;
    pStr = ReadMapValueOfOamDat(MAP_TYPE_XMLADAPTER,MAP_KEY_NO_FINDCMDINMODE);
    printf("%s\n",pStr);
}
VOID Oam_TestOmmMaintainMode(VOID)
{
    CHAR *pucXmlBuf = NULL;
     CHAR *pucXml = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?><CONF><T><VER>0</VER><NTP>3GPL</NTP><NID>0</NID><TRM>3288860</TRM><TID>65535</TID><SID>0</SID><MID>0</MID></T><D><OT>S</OT><OR>OK</OR><DC>1</DC><EOF>Y</EOF><MD><CMD id=\"0x82040001\" optype=\"S\"></CMD></MD></D></CONF>";
    WORD16 wMsgLen = 0;
    gt_XmlLinkState[0].dwLinkChannel = 3288860;
    gt_XmlLinkState[0].bUsed = XML_LINKSTATE_USED;
    /*分配最近使用OAM结构体空间*/
    if (NULL != gt_XmlLinkState[0].ptLastSendMsg)
    {
        OAM_RETUB(gt_XmlLinkState[0].ptLastSendMsg);
    }
    gt_XmlLinkState[0].ptLastSendMsg = (MSG_COMM_OAM *)XOS_GetUB(MAX_OAMMSG_LEN);
    memset(gt_XmlLinkState[0].ptLastSendMsg,0,MAX_OAMMSG_LEN);

    wMsgLen = strlen(pucXml) + 2 + 1;
    pucXmlBuf = (CHAR *)XOS_GetUB(wMsgLen);
    if (NULL == pucXmlBuf)
    {
        printf("XOS_GetUB Error!\n");
        return;
    }
    memset(pucXmlBuf,0,wMsgLen);
    memcpy(pucXmlBuf+2,pucXml,strlen(pucXml));
    OSS_SendFromTask(EV_XML_OMMCFG_REQ,(BYTE *)pucXmlBuf, (WORD16)wMsgLen,NULL,&g_ptOamIntVar->jidSelf);			
    XOS_RetUB(pucXmlBuf);
    return;
}
VOID Oam_TestOmmEnableMode(VOID)
{
    CHAR *pucXmlBuf = NULL;
     CHAR *pucXml = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?><CONF><T><VER>0</VER><NTP>3GPL</NTP><NID>0</NID><TRM>3288860</TRM><TID>65535</TID><SID>0</SID><MID>0</MID></T><D><OT>S</OT><OR>OK</OR><DC>1</DC><EOF>Y</EOF><MD><CMD id=\"0x80040001\" optype=\"S\"></CMD></MD></D></CONF>";
    WORD16 wMsgLen = 0;
    gt_XmlLinkState[0].dwLinkChannel = 3288860;
    gt_XmlLinkState[0].bUsed = XML_LINKSTATE_USED;
    /*分配最近使用OAM结构体空间*/
    if (NULL != gt_XmlLinkState[0].ptLastSendMsg)
    {
        OAM_RETUB(gt_XmlLinkState[0].ptLastSendMsg);
    }
    gt_XmlLinkState[0].ptLastSendMsg = (MSG_COMM_OAM *)XOS_GetUB(MAX_OAMMSG_LEN);
    memset(gt_XmlLinkState[0].ptLastSendMsg,0,MAX_OAMMSG_LEN);

    wMsgLen = strlen(pucXml) + 2 + 1;
    pucXmlBuf = (CHAR *)XOS_GetUB(wMsgLen);
    if (NULL == pucXmlBuf)
    {
        printf("XOS_GetUB Error!\n");
        return;
    }
    memset(pucXmlBuf,0,wMsgLen);
    memcpy(pucXmlBuf+2,pucXml,strlen(pucXml));
    OSS_SendFromTask(EV_XML_OMMCFG_REQ,(BYTE *)pucXmlBuf, (WORD16)wMsgLen,NULL,&g_ptOamIntVar->jidSelf);			
    XOS_RetUB(pucXmlBuf);
    return;
}
VOID Oam_TestOmmProtoMode(VOID)
{
    CHAR *pucXmlBuf = NULL;
     CHAR *pucXml = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?><CONF><T><VER>0</VER><NTP>3GPL</NTP><NID>0</NID><TRM>3288860</TRM><TID>65535</TID><SID>0</SID><MID>0</MID></T><D><OT>S</OT><OR>OK</OR><DC>1</DC><EOF>Y</EOF><MD><CMD id=\"0x00050020\" optype=\"S\"></CMD></MD></D></CONF>";
    WORD16 wMsgLen = 0;
    gt_XmlLinkState[0].dwLinkChannel = 3288860;
    gt_XmlLinkState[0].bUsed = XML_LINKSTATE_USED;
    /*分配最近使用OAM结构体空间*/
    if (NULL != gt_XmlLinkState[0].ptLastSendMsg)
    {
        OAM_RETUB(gt_XmlLinkState[0].ptLastSendMsg);
    }
    gt_XmlLinkState[0].ptLastSendMsg = (MSG_COMM_OAM *)XOS_GetUB(MAX_OAMMSG_LEN);
    memset(gt_XmlLinkState[0].ptLastSendMsg,0,MAX_OAMMSG_LEN);

    wMsgLen = strlen(pucXml) + 2 + 1;
    pucXmlBuf = (CHAR *)XOS_GetUB(wMsgLen);
    if (NULL == pucXmlBuf)
    {
        printf("XOS_GetUB Error!\n");
        return;
    }
    memset(pucXmlBuf,0,wMsgLen);
    memcpy(pucXmlBuf+2,pucXml,strlen(pucXml));
    OSS_SendFromTask(EV_XML_OMMCFG_REQ,(BYTE *)pucXmlBuf, (WORD16)wMsgLen,NULL,&g_ptOamIntVar->jidSelf);			
    XOS_RetUB(pucXmlBuf);
    return;
}
VOID Oam_TestOmmExitMode(VOID)
{
    CHAR *pucXmlBuf = NULL;
     CHAR *pucXml = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?><CONF><T><VER>0</VER><NTP>3GPL</NTP><NID>0</NID><TRM>3288860</TRM><TID>65535</TID><SID>0</SID><MID>0</MID></T><D><OT>S</OT><OR>OK</OR><DC>1</DC><EOF>Y</EOF><MD><CMD id=\"0x80040005\" optype=\"S\"></CMD></MD></D></CONF>";
    WORD16 wMsgLen = 0;
    gt_XmlLinkState[0].dwLinkChannel = 3288860;
    gt_XmlLinkState[0].bUsed = XML_LINKSTATE_USED;
    /*分配最近使用OAM结构体空间*/
    if (NULL != gt_XmlLinkState[0].ptLastSendMsg)
    {
        OAM_RETUB(gt_XmlLinkState[0].ptLastSendMsg);
    }
    gt_XmlLinkState[0].ptLastSendMsg = (MSG_COMM_OAM *)XOS_GetUB(MAX_OAMMSG_LEN);
    memset(gt_XmlLinkState[0].ptLastSendMsg,0,MAX_OAMMSG_LEN);

    wMsgLen = strlen(pucXml) + 2 + 1;
    pucXmlBuf = (CHAR *)XOS_GetUB(wMsgLen);
    if (NULL == pucXmlBuf)
    {
        printf("XOS_GetUB Error!\n");
        return;
    }
    memset(pucXmlBuf,0,wMsgLen);
    memcpy(pucXmlBuf+2,pucXml,strlen(pucXml));
    OSS_SendFromTask(EV_XML_OMMCFG_REQ,(BYTE *)pucXmlBuf, (WORD16)wMsgLen,NULL,&g_ptOamIntVar->jidSelf);			
    XOS_RetUB(pucXmlBuf);
    return;
}
VOID Oam_TestOmmShowModeStack(VOID)
{
    CHAR *pucXmlBuf = NULL;
     CHAR *pucXml = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?><CONF><T><VER>0</VER><NTP>3GPL</NTP><NID>0</NID><TRM>3288860</TRM><TID>65535</TID><SID>0</SID><MID>0</MID></T><D><OT>S</OT><OR>OK</OR><DC>1</DC><EOF>Y</EOF><MD><CMD id=\"0x8100000A\" optype=\"S\"></CMD></MD></D></CONF>";
    WORD16 wMsgLen = 0;
    gt_XmlLinkState[0].dwLinkChannel = 3288860;
    gt_XmlLinkState[0].bUsed = XML_LINKSTATE_USED;
    /*分配最近使用OAM结构体空间*/
    if (NULL != gt_XmlLinkState[0].ptLastSendMsg)
    {
        OAM_RETUB(gt_XmlLinkState[0].ptLastSendMsg);
    }
    gt_XmlLinkState[0].ptLastSendMsg = (MSG_COMM_OAM *)XOS_GetUB(MAX_OAMMSG_LEN);
    memset(gt_XmlLinkState[0].ptLastSendMsg,0,MAX_OAMMSG_LEN);

    wMsgLen = strlen(pucXml) + 2 + 1;
    pucXmlBuf = (CHAR *)XOS_GetUB(wMsgLen);
    if (NULL == pucXmlBuf)
    {
        printf("XOS_GetUB Error!\n");
        return;
    }
    memset(pucXmlBuf,0,wMsgLen);
    memcpy(pucXmlBuf+2,pucXml,strlen(pucXml));
    OSS_SendFromTask(EV_XML_OMMCFG_REQ,(BYTE *)pucXmlBuf, (WORD16)wMsgLen,NULL,&g_ptOamIntVar->jidSelf);			
    XOS_RetUB(pucXmlBuf);
    return;
}
VOID Oam_TestOmmMsSave(VOID)
{
    CHAR *pucXmlBuf = NULL;
     CHAR *pucXml = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?><CONF><T><VER>0</VER><NTP>3GPL</NTP><NID>0</NID><TRM>3288860</TRM><TID>65535</TID><SID>0</SID><MID>0</MID></T><D><OT>S</OT><OR>OK</OR><DC>1</DC><EOF>Y</EOF><MD><CMD id=\"0x82020002\" optype=\"S\"><P id=\"0\">1</P><P id=\"1\">0</P></CMD></MD></D></CONF>";
    WORD16 wMsgLen = 0;
    gt_XmlLinkState[0].dwLinkChannel = 3288860;
    gt_XmlLinkState[0].bUsed = XML_LINKSTATE_USED;
    /*分配最近使用OAM结构体空间*/
    if (NULL != gt_XmlLinkState[0].ptLastSendMsg)
    {
        OAM_RETUB(gt_XmlLinkState[0].ptLastSendMsg);
    }
    gt_XmlLinkState[0].ptLastSendMsg = (MSG_COMM_OAM *)XOS_GetUB(MAX_OAMMSG_LEN);
    memset(gt_XmlLinkState[0].ptLastSendMsg,0,MAX_OAMMSG_LEN);

    wMsgLen = strlen(pucXml) + 2 + 1;
    pucXmlBuf = (CHAR *)XOS_GetUB(wMsgLen);
    if (NULL == pucXmlBuf)
    {
        printf("XOS_GetUB Error!\n");
        return;
    }
    memset(pucXmlBuf,0,wMsgLen);
    memcpy(pucXmlBuf+2,pucXml,strlen(pucXml));
    OSS_SendFromTask(EV_XML_OMMCFG_REQ,(BYTE *)pucXmlBuf, (WORD16)wMsgLen,NULL,&g_ptOamIntVar->jidSelf);			
    XOS_RetUB(pucXmlBuf);
    return;
}
VOID Oam_TestLoopBack2(VOID)
{
    CHAR *pucXmlBuf = NULL;
    CHAR *pucXml = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?><CONF><T><VER>0</VER><NTP>3GPL</NTP><NID>0</NID><TRM>3288860</TRM><TID>65535</TID><SID>0</SID><MID>0</MID></T><D><OT>S</OT><OR>OK</OR><DC>1</DC><EOF>Y</EOF><MD><CMD id=\"0x04010022\" optype=\"S\"><P id=\"0\">2</P></CMD></MD></D></CONF>";
    WORD16 wMsgLen = 0;
    gt_XmlLinkState[0].dwLinkChannel = 3288860;
    gt_XmlLinkState[0].bUsed = XML_LINKSTATE_USED;
    /*分配最近使用OAM结构体空间*/
    if (NULL != gt_XmlLinkState[0].ptLastSendMsg)
    {
        OAM_RETUB(gt_XmlLinkState[0].ptLastSendMsg);
    }
    gt_XmlLinkState[0].ptLastSendMsg = (MSG_COMM_OAM *)XOS_GetUB(MAX_OAMMSG_LEN);
    memset(gt_XmlLinkState[0].ptLastSendMsg,0,MAX_OAMMSG_LEN);

    wMsgLen = strlen(pucXml) + 2 + 1;
    pucXmlBuf = (CHAR *)XOS_GetUB(wMsgLen);
    if (NULL == pucXmlBuf)
    {
        printf("XOS_GetUB Error!\n");
        return;
    }
    memset(pucXmlBuf,0,wMsgLen);
    memcpy(pucXmlBuf+2,pucXml,strlen(pucXml));
    OSS_SendFromTask(EV_XML_OMMCFG_REQ,(BYTE *)pucXmlBuf, (WORD16)wMsgLen,NULL,&g_ptOamIntVar->jidSelf);			
    XOS_RetUB(pucXmlBuf);
    return;
}
VOID Oam_TestOmmDampening(VOID)
{
    CHAR *pucXmlBuf = NULL;
     CHAR *pucXml = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?><CONF><T><VER>0</VER><NTP>3GPL</NTP><NID>0</NID><TRM>3288860</TRM><TID>65535</TID><SID>0</SID><MID>0</MID></T><D><OT>S</OT><OR>OK</OR><DC>1</DC><EOF>Y</EOF><MD><CMD id=\" 0x04010100\" optype=\"S\"></CMD></MD></D></CONF>";
    WORD16 wMsgLen = 0;
    gt_XmlLinkState[0].dwLinkChannel = 3288860;
    gt_XmlLinkState[0].bUsed = XML_LINKSTATE_USED;
    /*分配最近使用OAM结构体空间*/
    if (NULL != gt_XmlLinkState[0].ptLastSendMsg)
    {
        OAM_RETUB(gt_XmlLinkState[0].ptLastSendMsg);
    }
    gt_XmlLinkState[0].ptLastSendMsg = (MSG_COMM_OAM *)XOS_GetUB(MAX_OAMMSG_LEN);
    memset(gt_XmlLinkState[0].ptLastSendMsg,0,MAX_OAMMSG_LEN);

    wMsgLen = strlen(pucXml) + 2 + 1;
    pucXmlBuf = (CHAR *)XOS_GetUB(wMsgLen);
    if (NULL == pucXmlBuf)
    {
        printf("XOS_GetUB Error!\n");
        return;
    }
    memset(pucXmlBuf,0,wMsgLen);
    memcpy(pucXmlBuf+2,pucXml,strlen(pucXml));
    OSS_SendFromTask(EV_XML_OMMCFG_REQ,(BYTE *)pucXmlBuf, (WORD16)wMsgLen,NULL,&g_ptOamIntVar->jidSelf);			
    XOS_RetUB(pucXmlBuf);
    return;
}

VOID Oam_DbgLinkCfgHandShake(VOID)
{
    BYTE aucComponentId[2] = {0};
    CHAR aucXmlBuf[] = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?><LINKCFGREQ><TYPE>HandShake</TYPE></LINKCFGREQ>";
    INT iRet = 0;
    INT iOptions = XML_STAT_PARSE_NOBLANKS;
    XmlNode tRootNode;
    XmlDocPtr ptrDoc = NULL;
    WORD32 dwXmlLen = strlen(aucXmlBuf) + 1;

    memset(&tRootNode,0,sizeof(tRootNode));
    /*解析XML文档并返回根节点*/
    iRet = XML_ParaseDoc(aucXmlBuf,dwXmlLen,iOptions,&ptrDoc,&tRootNode);
    if (RETURN_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_ParaseDoc Error,Error code %d! ....................",__func__,__LINE__,iRet);        
	 return ;
    }
    LinkCfg_HandShakeProcess(aucComponentId,&tRootNode);
    XML_FreeXml(ptrDoc); 
    return;
}

/**********************************************************************
* 函数名称：XML_AGENT_STAT LinkCfg_HandShakeProcess
* 功能描述：链路配置-握手
* 访问的表：无
* 修改的表：无
* 输入参数：strComponentId-组件号
                             ptCurNode-当前节点
* 输出参数：无
* 返 回 值：   XML_AGENT_STAT
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2009-11-25             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT LinkCfg_HandShakeProcess(BYTE *pucComponentId,LPXmlNode ptNodeIn)
{
    INT iRet = RETURN_OK;                                   /*返回值*/
    CHAR *pOMMMsgBuf = NULL;                          /*OMM应答消息体*/
    WORD16 wLoop = 0;
    WORD16 wMsgLen = 0;
    WORD32 dwLoop = 0;
    JID tDtJid;
    LPXmlNode ptNodeTmp = NULL;    
    XmlNode tChildNode;                             /*子节点*/
    XmlNode tCurNode;                               /*当前节点*/
    XmlDocPtr ptrDoc = NULL;                               /*xml文档指针*/
    TYPE_XMLMODEL *ptXmlModel = NULL;
    
    /*入参检查*/
    if ((NULL == pucComponentId) || (NULL == ptNodeIn))
    {
        return NULL_PTR;
    }
    ptNodeTmp = ptNodeIn;    
    memset(&tChildNode,0,sizeof(tChildNode));
    memset(&tCurNode,0,sizeof(tCurNode));
    memset(&tDtJid,0,sizeof(tDtJid));
    /*杀掉握手定时器*/
    Xml_KillHandShakeTimer();
    /*启动握手定时器*/
    Xml_SetHandShakeTimer();
    iRet = XOS_Sender(&tDtJid);
    /*获取发送端JID失败*/
    if (XOS_SUCCESS != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                               "....................FUNC:%s LINE:%d XOS_Sender Error,Error code %d! ....................",__func__,__LINE__,iRet);
	 return RETURN_ERROR;
    }
    /*生成XML文本*/
    iRet = XML_NewDoc(NAME_ROOT_NODE_ACK,&ptrDoc);
    if (XML_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_NewDoc Error,Error code %d! ....................",__func__,__LINE__,iRet);
	 return iRet;
    }
    /*获取根节点*/
    iRet = XML_GetRootNode(ptrDoc,&tCurNode);
    if (XML_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_GetRootNode Error,Error code %d! ....................",__func__,__LINE__,iRet);
        XML_FreeXml(ptrDoc);
	 return iRet;
    }
    /*插入<Type>节点*/    
    iRet = XML_InsertChild(&tCurNode,0,NAME_TYPE_NODE,&tChildNode);
    if (XML_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_InsertChild Error,Error code %d! ....................",__func__,__LINE__,iRet);
	 XML_FreeXml(ptrDoc);
	 return iRet;
    }
    /*设置<Type>节点文本*/
    iRet = XML_SetChildText(&tChildNode,LINKCFG_NAME_HANDSHAKE);
    if (XML_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_InsertChild Error,Error code %d! ....................",__func__,__LINE__,iRet);
	 XML_FreeXml(ptrDoc);
	 return iRet;
    }
    /*插入<Trm>节点*/
    iRet = XML_InsertChild(&tCurNode,1,NAME_TRM_NODE,&tChildNode);
    if (XML_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_InsertChild Error,Error code %d! ....................",__func__,__LINE__,iRet);
	 XML_FreeXml(ptrDoc);
	 return iRet;
    }
    /*遍历链路信息获取注册的TRM ID*/
    for (wLoop = 0;wLoop < MAX_SIZE_LINKSTATE;wLoop++)
    {
        CHAR aucNodeText[MAX_NODETEXT_BUF_SIZE] = {0};/*节点文本*/
        XmlNode tIdNode;
        memset(&tIdNode,0,sizeof(tIdNode));
        if (XML_LINKSTATE_USED == gt_XmlLinkState[wLoop].bUsed)
        {
            /*插入<id>节点*/
            iRet = XML_InsertChild(&tChildNode,dwLoop,NAME_ID_NODE,&tIdNode);
	     if (XML_OK != iRet)
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR,
                                   "....................FUNC:%s LINE:%d XML_InsertChild Error,Error code %d! ....................",__func__,__LINE__,iRet);
	         continue;
            }
            memset(aucNodeText,0,sizeof(aucNodeText));
            snprintf(aucNodeText,sizeof(aucNodeText),"%d",gt_XmlLinkState[wLoop].dwLinkChannel);
	     /*设置<ID>节点文本*/
	     iRet = XML_SetChildText(&tIdNode,aucNodeText);
	     if(XML_OK != iRet)
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR,
                                   "....................FUNC:%s LINE:%d XML_SetChildText Error,Error code %d! ....................",__func__,__LINE__,iRet);
	         continue;		 
            }
        }
    }    
    pOMMMsgBuf = XML_GetTextForUTF(ptrDoc, (const CHAR *)pucComponentId);
    /*获取文本失败*/
    if (NULL == pOMMMsgBuf)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XOS_GetUB Error! ....................",__func__,__LINE__);
        XML_FreeXml(ptrDoc);
        return NULL_PTR;
    }
    ptXmlModel = (TYPE_XMLMODEL *)pOMMMsgBuf;
    wMsgLen = sizeof(ptXmlModel->wComponentId) + strlen(ptXmlModel->ucXML) + 1;
    /*消息打印*/
    if (bDebugPrint)
    {
        PrintMsgXml(ptXmlModel, "MSG_XMLAGNET_TO_OMM");
    }
    /*向网管返回链路注册应答*/
    iRet = XOS_SendAsynMsg(EV_XML_LINKCFG_ACK,
                            (BYTE *)pOMMMsgBuf,
                            wMsgLen, 
                            0, 
                            0, 
                            &tDtJid);
    if (XOS_SUCCESS != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                               "....................FUNC:%s LINE:%d XOS_SendAsynMsg Error,Error code %d! ....................",__func__,__LINE__,iRet);	 
    }
    /*释放资源*/
    OAM_RETUB(pOMMMsgBuf);
    XML_FreeXml(ptrDoc);
    return iRet;
}

/**********************************************************************
* 函数名称：XML_AGENT_STAT LoginTrmId
* 功能描述：注册终端号
* 访问的表：无
* 修改的表：无
* 输入参数：dwTrmId:终端号                             
* 输出参数：无
* 返 回 值：    TRUE/FALSE
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2009-11-25             齐龙兆      创建
************************************************************************/
static BOOLEAN LoginTrmId(WORD32 dwTrmId)
{
    WORD16 wLoop = 0;    

    /*检查终端号是否已经注册*/
    for(wLoop = 0;wLoop < MAX_SIZE_LINKSTATE;wLoop++)
    {
        if ((dwTrmId == gt_XmlLinkState[wLoop].dwLinkChannel)&&
	     (XML_LINKSTATE_USED == gt_XmlLinkState[wLoop].bUsed))
        {
            break;
        }
    }
    if (wLoop < MAX_SIZE_LINKSTATE)/*已经注册过*/
    {
        return TRUE;
    }
    /*分配链路信息*/
    for (wLoop = 0;wLoop < MAX_SIZE_LINKSTATE;wLoop++)
    {
        if (XML_LINKSTATE_UNUSED == gt_XmlLinkState[wLoop].bUsed)
	 {
	     break;
	 }
    }
    if (wLoop >= MAX_SIZE_LINKSTATE)/*未分配到*/
    {
        return FALSE;
    }
    memset(&gt_XmlLinkState[wLoop],0,sizeof(TYPE_XMLLINKSTATE));    
    /*释放最近使用OAM结构体空间*/
    if (NULL != gt_XmlLinkState[wLoop].ptLastSendMsg)
    {
        OAM_RETUB(gt_XmlLinkState[wLoop].ptLastSendMsg);
    }
    /*申请最近使用OAM结构体空间*/
    gt_XmlLinkState[wLoop].ptLastSendMsg = (MSG_COMM_OAM *)XOS_GetUB(MAX_OAMMSG_LEN);
    if (NULL == gt_XmlLinkState[wLoop].ptLastSendMsg)
    {                
        return FALSE;
    }
    memset(gt_XmlLinkState[wLoop].ptLastSendMsg,0,MAX_OAMMSG_LEN);
    /*设置使用标志*/
    gt_XmlLinkState[wLoop].bUsed = XML_LINKSTATE_USED;     
    /*设置链路号*/
    gt_XmlLinkState[wLoop].dwLinkChannel = dwTrmId;
    return TRUE;
}

/**********************************************************************
* 函数名称：XML_AGENT_STAT RegisterTrmId
* 功能描述：注册终端号
* 访问的表：无
* 修改的表：无
* 输入参数：WORD32 dwTrmId:终端号                             
* 输出参数：无
* 返 回 值：    TYPE_XMLLINKSTATE *ptLinkState:链路状态指针
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT RegisterTrmId(WORD16 wLinkChannel)
{    
    TYPE_XMLLINKSTATE *ptLinkState = NULL;

    /*检查终端号是否已经注册*/
    ptLinkState = Xml_GetLinkStateByLinkChannel(wLinkChannel);
    if (NULL != ptLinkState)
    {        
        return RETURN_ERROR;
    }
    ptLinkState = XML_GetUsableLinkState();

    if (NULL == ptLinkState)
    {
        return RETURN_ERROR;
    }
    /*设置使用标志*/
    ptLinkState->bUsed = XML_LINKSTATE_USED;     
    /*设置链路号*/
    ptLinkState->dwLinkChannel = wLinkChannel;    
    /*分配最近使用OAM结构体空间*/
    if (NULL != ptLinkState->ptLastSendMsg)
    {
        OAM_RETUB(ptLinkState->ptLastSendMsg);
    }
    ptLinkState->ptLastSendMsg = (MSG_COMM_OAM *)XOS_GetUB(MAX_OAMMSG_LEN);
    if (NULL == ptLinkState->ptLastSendMsg)
    {
        return RETURN_ERROR;
    }
    memset(ptLinkState->ptLastSendMsg,0,MAX_OAMMSG_LEN);
    return RETURN_OK;
}

/**********************************************************************
* 函数名称：XML_AGENT_STAT LogOutTrmId
* 功能描述：注销终端号
* 访问的表：无
* 修改的表：无
* 输入参数：dwTrmId:终端号                             
* 输出参数：无
* 返 回 值：    TRUE/FALSE
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 20011-11-25             齐龙兆      创建
************************************************************************/
static BOOLEAN LogOutTrmId(WORD32 dwTrmId)
{
    WORD16 wLoop = 0;
    /*检查终端号是否已经注册*/
    for(wLoop = 0;wLoop < MAX_SIZE_LINKSTATE;wLoop++)
    {
        if ((dwTrmId == gt_XmlLinkState[wLoop].dwLinkChannel)&&
	     (XML_LINKSTATE_USED == gt_XmlLinkState[wLoop].bUsed))
        {
            break;
        }
    }
    if (wLoop >= MAX_SIZE_LINKSTATE)/*未注册过*/
    {
        return TRUE;
    }
    /*终止当前链路命令*/
    Omm_CfgStopExcutingCmd(&(gt_XmlLinkState[wLoop]));
    /*杀掉当前链路应答定时器*/
    Xml_KillExeTimer(&(gt_XmlLinkState[wLoop]));
    /*释放当前链路中最近使用的结构体*/    
    OAM_RETUB(gt_XmlLinkState[wLoop].ptLastSendMsg);
    memset(&(gt_XmlLinkState[wLoop]),0,sizeof(TYPE_XMLLINKSTATE));    
    return TRUE;
}

/**********************************************************************
* 函数名称：BYTE GetNodeText
* 功能描述：获取节点文本
* 访问的表：无
* 修改的表：无
* 输入参数：ptrNodeIn :当前节点
                             strNodeName:节点名称
                             dwNodeIndex:节点索引
                             wNodeTextLen:节点文本长度                             
* 输出参数：strNodeText:节点文本
* 返 回 值：   XML_AGENT_STAT
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2009-11-25             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT GetNodeText(LPXmlNode ptrNodeIn,CHAR *strNodeName,WORD32 dwNodeIndex,WORD32 wNodeTextLen,CHAR *strNodeText)
{    
    INT iRet = RETURN_OK;                                      /*返回值*/    
    XmlNode tNodeOut;                                  /*目标节点*/
    LPXmlNode ptrNodeTmp = NULL;                         /*节点指针临时变量*/    
	
    /*入参检查*/
    if ((NULL == ptrNodeIn) || (NULL == strNodeName) ||(NULL == strNodeText))
    {
        return NULL_PTR;
    }
    ptrNodeTmp = ptrNodeIn;
    memset(&tNodeOut,0,sizeof(tNodeOut));
    /*获取指点节点*/
    iRet = GetChlidByNode(ptrNodeTmp,strNodeName,0,&tNodeOut);
    if (RETURN_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d GetChlidByNode Error,Code=%d!....................",__func__,__LINE__,iRet);        
        return iRet;
    }
    /*获取节点文本*/
    iRet = GetChildTextByNode(&tNodeOut,wNodeTextLen,strNodeText);
    if (RETURN_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d GetChildTextByNode Error,Code=%d!....................",__func__,__LINE__,iRet);        
        return iRet;
    }
    return iRet;
}

/**********************************************************************
* 函数名称：XML_AGENT_STAT UnRegisterTrmId
* 功能描述：注销终端号
* 访问的表：无
* 修改的表：无
* 输入参数：WORD32 dwTrmId:终端号                             
* 输出参数：无
* 返 回 值：    TYPE_XMLLINKSTATE *ptLinkState:链路状态指针
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT UnRegisterTrmId(WORD16 wLinkChannel)
{    
    TYPE_XMLLINKSTATE *ptLinkState = NULL;  

    ptLinkState = Xml_GetLinkStateByLinkChannel(wLinkChannel);
    if (NULL == ptLinkState)
    {
        return RETURN_ERROR;
    }
    /*终止当前链路命令*/
    Omm_CfgStopExcutingCmd(ptLinkState);
    /*杀掉当前链路应答定时器*/
    Xml_KillExeTimer(ptLinkState);
    /*释放当前链路中最近使用的结构体*/    
    OAM_RETUB(ptLinkState->ptLastSendMsg);
    memset(ptLinkState,0,sizeof(TYPE_XMLLINKSTATE));    
    return RETURN_OK;
}

/**********************************************************************
* 函数名称：BYTE SetXmlLinkState
* 功能描述：设置XML链路状态
* 访问的表：无
* 修改的表：无
* 输入参数：CHAR *ptrXmlMsgBuf:XML字符串
                              LPXmlNode ptrNode:当前节点指针                     
* 输出参数：TYPE_XMLLINKSTATE **pptLinkState:链路状态
* 返 回 值：RETURN_ERROR-失败 RETURN_OK-成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT SetXmlLinkState(TYPE_XMLMODEL *ptXmlModel,LPXmlNode ptrNode,TYPE_XMLLINKSTATE **pptLinkState)
{    
    TYPE_XMLMODEL *ptXmlModelTmp = NULL;/*XML模型临时指针*/	 
    LPXmlNode ptrNodeTmp = NULL;                /*节点指针临时变量*/    
    TYPE_XMLLINKSTATE *ptLinkState = NULL; /*链路状态指针*/    
    WORD32 dwLinkChannel = 0;/*终端连接号*/ 
    INT iRet = 0;

    if ((NULL == ptXmlModel) || (NULL == ptrNode) )
    {
        return NULL_PTR;
    }

    ptXmlModelTmp = ptXmlModel;
    ptrNodeTmp = ptrNode;     
	
    /*设置终端连接号*/
    if (RETURN_OK != SetTRMNodeText(ptrNodeTmp, &dwLinkChannel))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................SetTRMNodeText Error FUNC: %s,Line:%d!....................",__func__,__LINE__);
        return SET_NODE_TEXT_ERR;
    }
    /*根据终端链路号分配链路状态*/
    ptLinkState = Xml_AssignLinkState(dwLinkChannel);
    if (NULL == ptLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC: %s,Line:%d Xml_AssignLinkState Error!....................",__func__,__LINE__);
        return NoRegXmlLinkState;
    }    
    if(NULL == ptLinkState->ptLastSendMsg)
    {
        XOS_SysLog(OAM_CFG_LOG_NOTICE,
                       "....................FUNC: %s,Line:%d NULL PTR!....................",__func__,__LINE__);
        return NULL_PTR;
    }
    /*当前链路正在执行命令*/
    if (pec_MsgHandle_EXECUTE == XML_LinkGetCurRunState(ptLinkState))
    {        
        return CmdExecutMutex;
    }
    /*检查其他链路是否执行存盘命令*/
    if (CheckLinkIsExecutingSaveCmd())
    {        
        return SaveMutexTip;
    }
    /*填充发送端JID*/
    /*使用调试函数调试网管命令需要注销下面的检查*/
    if (XOS_SUCCESS != XOS_Sender(&(ptLinkState->tDtJid)))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC: %s,Line:%d XOS_Sender Error!....................",__func__,__LINE__);
        return RETURN_ERROR;
    }
    /*填充组件号*/   
    if (IsNeedInvertOrder())
    {
        ptXmlModelTmp->wComponentId = XOS_InvertWord16(ptXmlModelTmp->wComponentId);
    }
    memcpy(ptLinkState->ucComponentId,(BYTE *)&(ptXmlModelTmp->wComponentId),sizeof(ptLinkState->ucComponentId));    
    /*填充发送序号*/
    ptLinkState->dwSeqNo = g_dwSeqNo;    
    /*填充XML声明部分*/
    memset(ptLinkState->xmlDeclText,0,sizeof(ptLinkState->xmlDeclText));
    if(RETURN_OK != GetDeclTextByName(ptXmlModelTmp->ucXML,ptLinkState->xmlDeclText))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................GetDeclTextByName Error FUNC: %s,Line:%d!....................",__func__,__LINE__);
        return GET_NODE_TEXT_ERR;
    }
    /*填充XML字符串中T段内容*/
    memset(ptLinkState->xmlTSecText,0,sizeof(ptLinkState->xmlTSecText));
    if (RETURN_OK != GetTSectionTextByName(ptXmlModelTmp->ucXML,ptLinkState->xmlTSecText))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................GetTSectionTextByName Error FUNC: %s,Line:%d!....................",__func__,__LINE__);
        return GET_NODE_TEXT_ERR;
    }    
    /*当前进程状态为S_UpdateDAT则不处理*/
    if (S_UpdateDAT == XOS_GetCurState())
    {
        *pptLinkState = ptLinkState;
        return DatUpdateMutex;
    }    
    /*设置命令节点ID属性*/
    if(RETURN_OK != SetCMDNodeAttr_ID(ptrNodeTmp,&(ptLinkState->dwCmdId)))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................SetCMDNodeAttr_ID Error FUNC: %s,Line:%d!....................",__func__,__LINE__);
        return SET_NODE_ATTR_ERR;
    }  
    /*填充是否NO命令*/
    if(RETURN_OK != SetOTNodeText(ptrNodeTmp,&(ptLinkState->tCurCMDInfo.bIsNoCmd)))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................SetOTNodeText Error FUNC: %s,Line:%d!....................",__func__,__LINE__);
        return GET_NODE_TEXT_ERR;
    }    
    /*填充当前命令信息*/
    iRet = SetCurCMDInfo(ptLinkState);     
    if (g_dwSeqNo >= 65535)
    {
        g_dwSeqNo = 1;
    }
    g_dwSeqNo++;
    *pptLinkState = ptLinkState;    
    return iRet;        
}

/**************************************************************************
* 函数名称：VOID XML_ParaseDoc
* 功能描述：解析文档并返回根节点
* 访问的表：无
* 修改的表：无
* 输入参数：CHAR *ptrXml:XML字符串
                              WORD32 dwLen:XML字符串长度
                              INT iOptions:解析参数
* 输出参数：XmlDocPtr *ppXmlDocPtr:文档指针的指针
                              LPXmlNode ptRootNode:根节点指针
* 返 回 值：RETURN_ERROR-失败 RETURN_OK-成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
**************************************************************************/
static XML_AGENT_STAT XML_ParaseDoc(CHAR *ptrXml,WORD32 dwLen,INT iOptions,XmlDocPtr *ppXmlDocPtr,LPXmlNode ptRootNode)
{
    XmlDocPtr ptrDoc = NULL;/*XML文档指针*/

    /*参数检查*/
    if ((NULL == ptrXml) || (0 == dwLen) || (NULL == ppXmlDocPtr) || (NULL == ptRootNode))
    {
        return NULL_PTR;
    }
    /*生成XML文档*/
    if (XML_OK != XML_LoadXml(ptrXml,dwLen,iOptions,&ptrDoc))
    {        
	 return LOAD_XML_ERR;/*加载XML文档失败*/
    }    
    /*获取根节点指针*/    
    if (XML_OK != XML_GetRootNode(ptrDoc,ptRootNode))
    {    	
        XML_FreeXml(ptrDoc);
        return GET_NODE_ERR;/*获取XML根节点失败*/
    }
    *ppXmlDocPtr = ptrDoc;
    return RETURN_OK;
}

/**********************************************************************
* 函数名称：BYTE GetChlidByNode
* 功能描述：根据指定的节点找到对应的子节点
* 访问的表：无
* 修改的表：无
* 输入参数：LPXmlNode ptrNodeIn:当前子节点指针                              
                              CHAR *ptrNodeName:子节点名
                              WORD32 dwNodeIndex:子节点序号 0-不存在重复名称的节点 非0表示第几个节点
* 输出参数：LPXmlNode ptrNodeOut:子节点指针            
* 返 回 值：RETURN_ERROR-失败 RETURN_OK-成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT GetChlidByNode(LPXmlNode ptrNodeIn,CHAR *ptrNodeName,WORD32 dwNodeIndex,LPXmlNode ptrNodeOut)
{
    LPXmlNode ptrNodeTmp = NULL;/*节点指针临时变量*/
    CHAR *ptrNodeNameTmp = NULL;/*节点名称临时指针*/   
    WORD32 dwCnt = 0;
    WORD32 dwChildCnt = 0;
    WORD32 i = 0;
	
    if ((ptrNodeIn == NULL) || (ptrNodeName == NULL)  || (ptrNodeOut == NULL))
    {
        return NULL_PTR;
    }
    ptrNodeTmp = ptrNodeIn;	
    ptrNodeNameTmp = ptrNodeName;  
    /*查找子节点个数*/
    dwChildCnt = XML_GetChildCount(ptrNodeTmp);        
    if (dwNodeIndex == 0)/*表示不存在重名的节点*/
    {
        for (i = 0;i < dwChildCnt;i++)
        {
            if (XML_OK != XML_GetChild(ptrNodeTmp,i,ptrNodeOut))
            {
                return GET_NODE_ERR;
            }
            if (0 == strcmp(ptrNodeNameTmp,(CHAR *)ptrNodeOut->name))
	     {
	         break;
	     }
        }
	 if (i >= dwChildCnt)
	 {
	     return GET_NODE_ERR;
	 }       
    }
    else if(dwNodeIndex > 0)/*表示有重名的节点存在,需要指定是当前下的第几个节点*/
    {
        for (i = 0;i < dwChildCnt;i++)
        {            
            if (XML_OK != XML_GetChild(ptrNodeTmp,i,ptrNodeOut))
            {
                return GET_NODE_ERR;
            }
	     if ((strcmp(ptrNodeNameTmp,(CHAR *)ptrNodeOut->name) == 0) && (dwCnt == dwNodeIndex))
	     {
	         break;
	     }
	     dwCnt = dwCnt + 1;
        }
	 if (i >= dwChildCnt)
	 {
	     return GET_NODE_ERR;
	 }        
    }
    else
    {
        return GET_NODE_ERR;
    }
    return RETURN_OK;
}

/**********************************************************************
* 函数名称：BYTE GetChildByName
* 功能描述：根据指定的节点名找到对应的子节点
* 访问的表：无
* 修改的表：无
* 输入参数：LPXmlNode ptrNodeIn:当前子节点指针
            CHAR *ptrSecName:段名
            CHAR *ptrNodeName:子节点名
            WORD32 dwNodeIndex:子节点序号 0-不存在重复名称的节点 非0表示第几个节点
* 输出参数：LPXmlNode ptrNodeOut:子节点指针            
* 返 回 值：RETURN_ERROR-失败 RETURN_OK-成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT GetChildByName(LPXmlNode ptrNodeIn,CHAR *ptrSecName,CHAR *ptrNodeName,WORD32 dwNodeIndex,LPXmlNode ptrNodeOut)
{
    LPXmlNode ptrNodeInTmp = NULL;/*节点指针临时变量*/
    LPXmlNode ptrSecNodeTmp = NULL;  /*临时指针变量*/ 
    XmlNode tSecNodeOut;/*段节点结构*/
    CHAR *ptrSecNameTmp   = NULL;/*段名临时变量*/
    CHAR *ptrNodeNameTmp  = NULL;/*节点名临时变量*/    
	  	  
    if ((ptrNodeIn == NULL) || (ptrSecName == NULL) || (ptrNodeName == NULL) || (ptrNodeOut == NULL))
    {
        return NULL_PTR;
    }
    
    ptrNodeInTmp     = ptrNodeIn;
    ptrSecNameTmp  = ptrSecName;
    ptrNodeNameTmp = ptrNodeName;    
    memset(&tSecNodeOut,0,sizeof(XmlNode));
    ptrSecNodeTmp = &tSecNodeOut;
    /*根据段名查找段节点*/
    if (RETURN_OK != GetChlidByNode(ptrNodeInTmp,ptrSecNameTmp,0, ptrSecNodeTmp))
    {
        return GET_NODE_ERR;
    }
    /*根据节点名获取节点*/
    if (RETURN_OK != GetChlidByNode(ptrSecNodeTmp,ptrNodeNameTmp,dwNodeIndex,ptrNodeOut))
    {
        return GET_NODE_ERR;
    }   
    return RETURN_OK;
}

/**********************************************************************
* 函数名称：BYTE FindChildAttrByName
* 功能描述：根据属性名称查找属性
* 访问的表：无
* 修改的表：无
* 输入参数：LPXmlNode ptrNode:属性子节点
            CHAR *ptrAttrName:属性名称
            WORD16 wLen:属性缓冲区大小            
* 输出参数：CHAR * ptrAttrBuf:属性缓冲区指针
* 返 回 值：RETURN_ERROR-失败 RETURN_OK-成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT GetNodeAttrValueByName(LPXmlNode ptrNode,CHAR *ptrAttrName,WORD16 wLen,CHAR * ptrAttrBuf)
{
    LPXmlNode ptrNodeTmp = NULL;/*节点临时指针*/
    CHAR *ptrAttrNameTmp = NULL;/*属性名称临时指针*/
    LPXmlAttr ptXmlAttr = NULL;/*属性节点指针*/
    XmlAttr tXmlAttr;/*属性节点结构*/    
    WORD32 dwAttrCnt = 0;
    WORD32 i = 0;
	
    if ((ptrNode == NULL) || (ptrAttrName == NULL) || (ptrAttrBuf == NULL))
    {
        return NULL_PTR;
    }
    memset(ptrAttrBuf,0,wLen);
    memset(&tXmlAttr,0,sizeof(XmlAttr));
    ptrNodeTmp = ptrNode;
    ptrAttrNameTmp = ptrAttrName;
    ptXmlAttr = &tXmlAttr;
    /*获取指定节点属性个数*/
    dwAttrCnt = XML_GetAttributeCount(ptrNodeTmp);
    /*根据属性名获取属性结构*/
    for(i = 0;i < dwAttrCnt;i++)
    {
        if (XML_OK!= XML_GetAttribute(ptrNodeTmp,i,ptXmlAttr))
        {
            return GET_NODE_ATTR_ERR;
        }
	 if (strcmp(ptrAttrNameTmp,(CHAR *)ptXmlAttr->name) == 0)
	 {
	     break;
	 }
    }
    if (i >= dwAttrCnt)
    {
        return GET_NODE_ATTR_ERR;
    }
    memcpy(ptrAttrBuf,(CHAR *)ptXmlAttr->value,strlen((CHAR *)ptXmlAttr->value));    
    return RETURN_OK;
}

/**********************************************************************
* 函数名称：BYTE GetChildTextByNode
* 功能描述：根据指定的节点获取对应的文本
* 访问的表：无
* 修改的表：无
* 输入参数：LPXmlNode ptrNode:节点指针  
            WORD16 wLen:缓冲区大小         
* 输出参数：CHAR * ptrNodeText_Buf:文本缓冲区指针
* 返 回 值：RETURN_ERROR-失败 RETURN_OK-成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT GetChildTextByNode(LPXmlNode ptrNode,WORD16 wLen,CHAR *ptrNodeText_Buf)
{
    WORD16 wActualLen = 0;/*实际写入字符个数*/
    LPXmlNode ptrNodeTmp = NULL;/*节点指针临时变量*/
	  
    if ((ptrNode == NULL) || (ptrNodeText_Buf == NULL))
    {
        return NULL_PTR;
    }
    ptrNodeTmp = ptrNode;    
    if (XML_OK != XML_GetChildText(ptrNodeTmp,ptrNodeText_Buf,wLen,&wActualLen))
    {
        return GET_NODE_TEXT_ERR;
    }
    return RETURN_OK;
}

/**********************************************************************
* 函数名称：BYTE GetChildCountByName
* 功能描述：查找指定的节点下指定名称的子节点个数
* 访问的表：无
* 修改的表：无
* 输入参数：LPXmlNode ptrNode:节点指针  
            CHAR *ptrNodeName:指定节点名称      
* 输出参数：BYTE *bParaCnt:子节点个数
* 返 回 值：RETURN_ERROR-失败 RETURN_OK-成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT GetChildCountByName(LPXmlNode ptrNode,CHAR *ptrNodeName,WORD32 *dwNumber)
{
    LPXmlNode ptrNodeTmp = NULL;/*节点临时指针*/
    CHAR *ptrNodeNameTmp = NULL;/*节点名称临时指针*/
    LPXmlNode ptXmlNode = NULL;/*节点指针*/
    XmlNode tXmlNode;/*临时节点变量*/
    WORD32 dwChildCnt = 0;/*子节点个数*/
    WORD32 dwFindCnt = 0;/*已找到子节点个数*/
    WORD32 i = 0;
    
    if ((ptrNode == NULL) || (ptrNodeName == NULL) || (dwNumber == NULL))
    {
        return NULL_PTR;
    }
    
    ptrNodeTmp = ptrNode;
    ptrNodeNameTmp = ptrNodeName;
    ptXmlNode = &tXmlNode;
    memset(ptXmlNode,0,sizeof(XmlNode));
	
    /*获取指定节点下子节点个数*/
    dwChildCnt = XML_GetChildCount(ptrNodeTmp);
    /*获取指定节点下指定子节点名称的子节点个数*/
    for(i = 0;i < dwChildCnt;i++)
    {
        if (XML_OK != XML_GetChild(ptrNodeTmp,i,ptXmlNode))
        {
            return GET_NODE_ERR;
        }
	 if (strcmp(ptrNodeNameTmp,(CHAR *)ptXmlNode->name) == 0)
	 {
	     dwFindCnt = dwFindCnt + 1;
	 }
    }   
    *dwNumber = dwFindCnt; 
    return RETURN_OK;
}

/**********************************************************************
* 函数名称：BYTE GetTextByName
* 功能描述：根据头尾名称获取文本
* 访问的表：无
* 修改的表：无
* 输入参数：CHAR *ptrHeadName:头名称
                              CHAR *ptrlEndName:尾名称
                              WORD32 dwLength:输出缓冲区的最大长度
                              CHAR *ptrInBuf:输入缓冲区指针
* 输出参数：CHAR *ptrOutBuf:输出缓冲区指针
* 返 回 值：RETURN_ERROR-失败 RETURN_OK-成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT GetTextByName(CHAR *ptrHeadName,CHAR *ptrlEndName,WORD32 dwLength,CHAR *ptrInBuf,CHAR *ptrOutBuf)
{
    CHAR *pStrTmpHead = NULL;/*头名称位置*/
    CHAR *pStrTmpEnd = NULL;    /*尾名称位置*/
    WORD32 dwLenTmp = 0;	/*取出文本大小*/

    /*入参判断*/
    if ((NULL == ptrHeadName) || 
	 (NULL == ptrlEndName) ||
	 (0 == dwLength) ||
	 (NULL == ptrInBuf) ||
	 (NULL == ptrOutBuf))
    {
        return NULL_PTR;
    }

    pStrTmpHead = strstr(ptrInBuf,ptrHeadName);   
    pStrTmpEnd   = strstr(ptrInBuf,ptrlEndName);   
    if ((NULL == pStrTmpHead) || (NULL == pStrTmpEnd))
    {
        return NULL_PTR;
    }
    dwLenTmp =   pStrTmpEnd - pStrTmpHead + strlen(ptrlEndName);
    if (dwLenTmp >= dwLength)
    {
        return RETURN_ERROR;
    }
    memcpy(ptrOutBuf,pStrTmpHead,dwLenTmp);  
    return RETURN_OK;
}

/**********************************************************************
* 函数名称：BYTE GetDeclTextByName
* 功能描述：获取XML声明部分
* 访问的表：无
* 修改的表：无
* 输入参数：CHAR *ptrDeclHeadName:声明头名称
                              CHAR *ptrDeclEndName:声明尾名称
                              CHAR *ptrXmlBuf:XML字符串
* 输出参数：CHAR *ptrDeclBuf:XML声明文本
* 返 回 值：RETURN_ERROR-失败 RETURN_OK-成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT GetDeclTextByName(CHAR *ptrXmlBuf,CHAR *ptrDeclBuf)
{
    CHAR *ptrDeclHead = "<?xml";/*XML声明头*/
    CHAR *ptrDeclEnd = "?>";        /*XML声明尾*/
    CHAR *ptrInBuf = NULL;
    CHAR *ptrOutBuf = NULL;
    WORD32 dwLength = MAX_SIZE_DECL_TEXT;

    /*入参判断*/
    if((NULL == ptrXmlBuf) || (NULL == ptrDeclBuf))
    {
        return NULL_PTR;
    }

    ptrInBuf = ptrXmlBuf;
    ptrOutBuf = ptrDeclBuf;

    if (RETURN_OK != GetTextByName(ptrDeclHead,ptrDeclEnd,dwLength,ptrInBuf,ptrOutBuf))
    {
        return RETURN_ERROR;
    }
    return RETURN_OK;    
}

/**********************************************************************
* 函数名称：BYTE GetTSectionTextByName
* 功能描述：获取段节点下所有文本 包括节点名称
* 访问的表：无
* 修改的表：无
* 输入参数：CHAR *ptrSecName:节点名
                              LPXmlNode ptrNode:节点指针
* 输出参数：CHAR *ptrTSecBuf:T段内容
* 返 回 值：RETURN_ERROR-失败 RETURN_OK-成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT GetTSectionTextByName(CHAR *ptrXmlBuf,CHAR *ptrSecBuf)
{  
    CHAR *ptrSecHeadName = "<T>";
    CHAR *ptrSecEndName = "</T>";
    CHAR *ptrInBuf = NULL;
    CHAR *ptrOutBuf = NULL;
    WORD32 dwLen = MAX_SIZE_TSECTION_TEXT;    
	  
    /*入参检查*/
    if ((ptrXmlBuf == NULL) || (ptrSecBuf == NULL))
    {
        return NULL_PTR;
    }

    ptrInBuf = ptrXmlBuf;
    ptrOutBuf = ptrSecBuf;

    if (RETURN_OK != GetTextByName(ptrSecHeadName,ptrSecEndName,dwLen,ptrInBuf,ptrOutBuf))
    {
        return RETURN_ERROR;
    }
    return RETURN_OK;
}

/**********************************************************************
* 函数名称：BYTE GetCMDNode
* 功能描述：获取命令(CMD)节点
* 访问的表：无
* 修改的表：无
* 输入参数：LPXmlNode ptrNodeIn :当前节点指针
* 输出参数：LPXmlNode ptrNodeOut:返回节点指针
* 返 回 值：RETURN_ERROR-失败 RETURN_OK-成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT GetCMDNode(LPXmlNode ptrNodeIn,LPXmlNode ptrNodeOut)
{
    LPXmlNode ptrNodeTmp = NULL;/*节点指针临时变量*/
    LPXmlNode ptrDtNode = NULL;/*目标节点指针*/
    XmlNode tDtNode;/*目标节点临时变量*/    
    CHAR *ptrSecName = "D";/*D段*/
    CHAR *ptrMDNodeName = "MD";/*节点名称-MD*/
    CHAR *ptrNodeName = "CMD";/*节点名称-CMD*/
    WORD32 dwNodeIndex = 0;/*子节点序号 0-不存在重复名称的节点 非0表示第几个节点*/

    if ((ptrNodeIn == NULL) || (ptrNodeOut == NULL))
    {
         return NULL_PTR;
    }
    ptrNodeTmp = ptrNodeIn;
    ptrDtNode = &tDtNode;
    memset(ptrDtNode,0,sizeof(XmlNode));
    if (RETURN_OK != GetChildByName(ptrNodeTmp,ptrSecName,ptrMDNodeName,dwNodeIndex,ptrDtNode))
    {
        return GET_NODE_ERR;
    }
    if(RETURN_OK != GetChlidByNode(ptrDtNode,ptrNodeName,dwNodeIndex,ptrNodeOut))
    {
        return GET_NODE_ERR;
    }
    return RETURN_OK;
}
/**********************************************************************
* 函数名称：BYTE GetNIDNode
* 功能描述：获取网元编号(NID)节点
* 访问的表：无
* 修改的表：无
* 输入参数：LPXmlNode ptrNodeIn :当前节点指针
* 输出参数：LPXmlNode ptrNodeOut:返回节点指针
* 返 回 值：RETURN_ERROR-失败 RETURN_OK-成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT GetNIDNode(LPXmlNode ptrNodeIn,LPXmlNode ptrNodeOut)
{
    LPXmlNode ptrNodeTmp = NULL;/*节点指针临时变量*/
    CHAR *ptrSecName = "T";/*T段*/
    CHAR *ptrNodeName = "NID";/*节点名称-NTP*/
    WORD32 dwNodeIndex = 0;/*子节点序号 0-不存在重复名称的节点 非0表示第几个节点*/
    if (ptrNodeIn == NULL)
    {
        return NULL_PTR;
    }
    ptrNodeTmp = ptrNodeIn;
    if (RETURN_OK != GetChildByName(ptrNodeTmp,ptrSecName,ptrNodeName,dwNodeIndex,ptrNodeOut))
    {
        return GET_NODE_ERR;
    }
	
    return RETURN_OK;
}
/**********************************************************************
* 函数名称：BYTE GetOTNode
* 功能描述：获取操作类型(OT)节点
* 访问的表：无
* 修改的表：无
* 输入参数：LPXmlNode ptrNodeIn :当前节点指针
* 输出参数：LPXmlNode ptrNodeOut:返回节点指针
* 返 回 值：RETURN_ERROR-失败 RETURN_OK-成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT GetOTNode(LPXmlNode ptrNodeIn,LPXmlNode ptrNodeOut)
{
    LPXmlNode ptrNodeTmp = NULL;/*节点指针临时变量*/
    CHAR *ptrSecName = "D";/*D段*/
    CHAR *ptrNodeName = "OT";/*节点名称-CMD*/
    WORD32 dwNodeIndex = 0;/*子节点序号 0-不存在重复名称的节点 非0表示第几个节点*/
    if ((ptrNodeIn == NULL) || (ptrNodeOut == NULL))
    {
        return NULL_PTR;
    }
    ptrNodeTmp = ptrNodeIn;
    if (RETURN_OK != GetChildByName(ptrNodeTmp,ptrSecName,ptrNodeName,dwNodeIndex,ptrNodeOut))
    {
        return GET_NODE_ERR;
    }
    return RETURN_OK;
}

/**********************************************************************
* 函数名称：BYTE GetNTPNode
* 功能描述：获取网元类型(NTP)节点
* 访问的表：无
* 修改的表：无
* 输入参数：LPXmlNode ptrNodeIn :当前节点指针
* 输出参数：LPXmlNode ptrNodeOut:返回节点指针
* 返 回 值：RETURN_ERROR-失败 RETURN_OK-成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT GetNTPNode(LPXmlNode ptrNodeIn,LPXmlNode ptrNodeOut)
{
    LPXmlNode ptrNodeTmp = NULL;/*节点指针临时变量*/
    CHAR *ptrSecName = "T";/*T段*/
    CHAR *ptrNodeName = "NTP";/*节点名称-NTP*/
    WORD32 dwNodeIndex = 0;/*子节点序号 0-不存在重复名称的节点 非0表示第几个节点*/
    if (ptrNodeIn == NULL)
    {
        return NULL_PTR;
    }
    ptrNodeTmp = ptrNodeIn;
    if (RETURN_OK != GetChildByName(ptrNodeTmp,ptrSecName,ptrNodeName,dwNodeIndex,ptrNodeOut))
    {
        return GET_NODE_ERR;
    }
	
    return RETURN_OK;
}

/**********************************************************************
* 函数名称：BYTE GetTRMNode
* 功能描述：获取网元类型(TRM)节点
* 访问的表：无
* 修改的表：无
* 输入参数：LPXmlNode ptrNodeIn :当前节点指针
* 输出参数：LPXmlNode ptrNodeOut:返回节点指针
* 返 回 值：RETURN_ERROR-失败 RETURN_OK-成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT GetTRMNode(LPXmlNode ptrNodeIn,LPXmlNode ptrNodeOut)
{
    LPXmlNode ptrNodeTmp = NULL;/*节点指针临时变量*/
    CHAR *ptrSecName = "T";/*T段*/
    CHAR *ptrNodeName = "TRM";/*节点名称-TRM*/
    WORD32 dwNodeIndex = 0;/*子节点序号 0-不存在重复名称的节点 非0表示第几个节点*/
	  
    if (ptrNodeIn == NULL)
    {
        return NULL_PTR;
    }
    ptrNodeTmp = ptrNodeIn;
    if (RETURN_OK != GetChildByName(ptrNodeTmp,ptrSecName,ptrNodeName,dwNodeIndex,ptrNodeOut))
    {
        return GET_NODE_ERR;
    }
    return RETURN_OK;
}

/**********************************************************************
* 函数名称：BYTE GetPARANode
* 功能描述：获取命令节点下参数(P)节点
* 访问的表：无
* 修改的表：无
* 输入参数：LPXmlNode ptrNodeIn :当前节点指针
* 输出参数：LPXmlNode ptrNodeOut:返回节点指针
* 返 回 值：RETURN_ERROR-失败 RETURN_OK-成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT GetPARANode(LPXmlNode ptrNodeIn,WORD32 dwNodeIndex,LPXmlNode ptrNodeOut)
{
    LPXmlNode ptrNodeTmp = NULL;/*节点指针临时变量*/    
    CHAR *ptrNodeName = "P";/*节点名称-P*/
	  
    if (ptrNodeIn == NULL)
    {
        return NULL_PTR;
    }
    ptrNodeTmp = ptrNodeIn;
    if (RETURN_OK != GetChlidByNode(ptrNodeTmp,ptrNodeName,dwNodeIndex,ptrNodeOut))
    {
        return GET_NODE_ERR;
    }
    return RETURN_OK;
}

/**********************************************************************
* 函数名称：BYTE GetCMDNodeAttr_ID
* 功能描述：获取命令(CMD)节点ID属性
* 访问的表：无
* 修改的表：无
* 输入参数：LPXmlNode ptrNode :当前节点指针
                              WORD16 wLen:缓冲区长度
* 输出参数：CHAR *ptrNodeAttr_Buf:返回节点文本指针
* 返 回 值：RETURN_ERROR-失败 RETURN_OK-成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT GetCMDNodeAttr_ID(LPXmlNode ptrNode,WORD16 wLen,CHAR *ptrNodeAttr_Buf)
{
    LPXmlNode ptrNodeTmp = NULL;/*节点指针临时变量*/
    LPXmlNode ptrDtNode = NULL;/*目标节点指针*/
    XmlNode tDtNode;/*目标节点结构*/
    CHAR *ptrAttrName = "id";
	
    if ((ptrNode == NULL) || (wLen == 0) || (ptrNodeAttr_Buf == NULL))
    {
        return NULL_PTR;
    }
    ptrNodeTmp = ptrNode;
    ptrDtNode = &tDtNode;
    memset(ptrDtNode,0,sizeof(XmlNode));
    /*获取命令(CMD)节点*/
    if (RETURN_OK != GetCMDNode(ptrNodeTmp,ptrDtNode))
    {
    	  return GET_NODE_ERR;
    }
    /*填充CMD节点ID属性*/	
    if (RETURN_OK != GetNodeAttrValueByName(ptrDtNode,ptrAttrName,MAX_NODEATTR_BUF_SIZE,ptrNodeAttr_Buf))
    {
        return GET_NODE_ATTR_ERR;	    
    }
    return RETURN_OK;
}

/**********************************************************************
* 函数名称：BYTE SetCMDNodeAttr_ID
* 功能描述：获取命令(CMD)节点ID属性
* 访问的表：无
* 修改的表：无
* 输入参数：LPXmlNode ptrNodeIn :当前节点指针
* 输出参数：LPXmlNode ptrNodeOut:返回节点指针
* 返 回 值：RETURN_ERROR-失败 RETURN_OK-成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT SetCMDNodeAttr_ID(LPXmlNode ptrNode,DWORD *dwCmdId)
{
    LPXmlNode ptrNodeTmp = NULL;/*节点指针临时变量*/    
    CHAR ptrNodeAttr_Buf[MAX_NODEATTR_BUF_SIZE];/*节点属性缓冲区*/
    CHAR pStopStr[MAX_NODEATTR_BUF_SIZE];
    CHAR *pTmp = NULL;
        
    if ((ptrNode == NULL) || (dwCmdId == NULL))
    {
    	  return NULL_PTR;
    }
    ptrNodeTmp = ptrNode;   
    pTmp = pStopStr;
    memset(ptrNodeAttr_Buf,0,MAX_NODEATTR_BUF_SIZE);
    memset(pStopStr,0,MAX_NODEATTR_BUF_SIZE);
    /*获取CMD节点ID属性*/	
    if (RETURN_OK != GetCMDNodeAttr_ID(ptrNodeTmp,MAX_NODEATTR_BUF_SIZE,ptrNodeAttr_Buf))
    {
        return GET_NODE_ATTR_ERR;	    
    }
    *dwCmdId = strtoul(ptrNodeAttr_Buf,&pTmp,16);
    return RETURN_OK;
}

/**********************************************************************
* 函数名称：BYTE GetParaNodeAttr_ID
* 功能描述：获取参数(P)节点ID属性
* 访问的表：无
* 修改的表：无
* 输入参数：LPXmlNode ptrNode:当前节点指针
                              WORD32 dwNumber:节点序号
                              WORD16 wLen:缓冲区大小
* 输出参数：CHAR *ptrNodeAttr_Buf:节点文本缓冲区
* 返 回 值：RETURN_ERROR-失败 RETURN_OK-成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT GetParaNodeAttr_ID(LPXmlNode ptrNode,WORD32 dwNumber,WORD16 wLen,CHAR *ptrNodeAttr_Buf)
{
    LPXmlNode ptrNodeTmp = NULL;/*节点指针临时变量*/  
    LPXmlNode ptrDtNode = NULL;/*目标节点指针*/
    XmlNode tDtNode;/*目标节点结构变量*/    
    CHAR *ptrAttrName = "id";    

    if ((ptrNode == NULL) || (wLen == 0) || (ptrNodeAttr_Buf == NULL))
    {
        return NULL_PTR;
    }

    ptrNodeTmp = ptrNode;
    ptrDtNode = &tDtNode;
    memset(ptrDtNode,0,sizeof(XmlNode));
    /*获取参数节点属性*/
    if (RETURN_OK != GetPARANode(ptrNodeTmp,dwNumber,ptrDtNode))
    {
        return GET_NODE_ERR;
    }
    /*获取参数节点属性-ID*/    
    if (RETURN_OK != GetNodeAttrValueByName(ptrDtNode,ptrAttrName,wLen,ptrNodeAttr_Buf))
    {
        return GET_NODE_ATTR_ERR;	    
    }

    return RETURN_OK;
}

/**********************************************************************
* 函数名称：BYTE SetParaNodeAttr_ID
* 功能描述：获取参数(P)节点ID属性
* 访问的表：无
* 修改的表：无
* 输入参数：LPXmlNode ptrNode:当前节点指针
                              BYTE bNumber:节点序
* 输出参数：BYTE *bParaNo:命令序号
* 返 回 值：RETURN_ERROR-失败 RETURN_OK-成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT SetParaNodeAttr_ID(LPXmlNode ptrNode,BYTE bNumber,BYTE *bParaNo)
{
    LPXmlNode ptrNodeTmp = NULL;/*节点指针临时变量*/      
    CHAR ptrNodeAttr_Buf[MAX_NODEATTR_BUF_SIZE];/*节点属性缓冲区*/	  
    WORD32 dwNumber = 0;
    WORD32 dwParaNo = 0;
    
    if ((ptrNode == NULL) || (bParaNo == NULL))
    {
        return NULL_PTR;
    }
    ptrNodeTmp = ptrNode;   
    memset(ptrNodeAttr_Buf,0,MAX_NODEATTR_BUF_SIZE);
    dwNumber = (WORD32)bNumber;
    /*获取参数节点属性-ID*/    
    if (RETURN_OK != GetParaNodeAttr_ID(ptrNodeTmp,dwNumber,MAX_NODEATTR_BUF_SIZE,ptrNodeAttr_Buf))
    {
        return GET_NODE_ATTR_ERR;	    
    }
    if (0 != Convert_atoi(ptrNodeAttr_Buf,&dwParaNo))
    {
        return RETURN_ERROR;
    }
    *bParaNo = (BYTE)dwParaNo;
    /*网管参数ID从0开始而平台命令从1开始*/
    return RETURN_OK;
}

/**********************************************************************
* 函数名称：BYTE GetTRMNodeText
* 功能描述：获取终端号(TRM)节点文本
* 访问的表：无
* 修改的表：无
* 输入参数：LPXmlNode ptrNodeIn :当前节点指针
                              WORD16 wLen:节点缓冲区大小
* 输出参数：CHAR *ptrNodeText_Buf:节点文本缓冲区
* 返 回 值：RETURN_ERROR-失败 RETURN_OK-成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT GetTRMNodeText(LPXmlNode ptrNode,WORD16 wLen,CHAR *ptrNodeText_Buf)
{
    LPXmlNode ptrNodeTmp = NULL;/*节点指针临时变量*/
    LPXmlNode ptrDtNode = NULL;/*目标节点指针*/
    XmlNode tDtNode;/*目标节点结构*/    

    if((ptrNode == NULL) || (wLen == 0)  || (ptrNodeText_Buf == NULL))
    {
        return NULL_PTR;
    }
    ptrNodeTmp = ptrNode;
    ptrDtNode = &tDtNode;
    memset(ptrDtNode,0,sizeof(XmlNode));
    /*获取TRM节点*/
    if (RETURN_OK != GetTRMNode(ptrNodeTmp,ptrDtNode))
    {
        return GET_NODE_ERR;
    }
    /*获取TRM节点文本*/
    if (RETURN_OK != GetChildTextByNode(ptrDtNode,MAX_NODETEXT_BUF_SIZE,ptrNodeText_Buf))
    {
        return GET_NODE_TEXT_ERR;
    }
	
    return RETURN_OK;
}

/**********************************************************************
* 函数名称：BYTE SetTRMNodeText
* 功能描述：设置终端号(TRM)节点文本
* 访问的表：无
* 修改的表：无
* 输入参数：LPXmlNode ptrNodeIn :当前节点指针            
* 输出参数：WORD16 *pwLinkChannel:终端号
* 返 回 值：RETURN_ERROR-失败 RETURN_OK-成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT SetTRMNodeText(LPXmlNode ptrNode,WORD32 *pdwLinkChannel)
{
    LPXmlNode ptrNodeTmp = NULL;/*节点指针临时变量*/    
    CHAR ptrNodeText_Buf[MAX_NODETEXT_BUF_SIZE];/*节点文本缓冲区*/    
    WORD32 dwLinkChannel = 0;
    if ((ptrNode == NULL) || (pdwLinkChannel== NULL))
    {
        return NULL_PTR;
    }
    ptrNodeTmp = ptrNode;    
    memset(ptrNodeText_Buf,0,MAX_NODETEXT_BUF_SIZE);
    /*获取TRM节点文本*/
    if (RETURN_OK != GetTRMNodeText(ptrNodeTmp,MAX_NODETEXT_BUF_SIZE,ptrNodeText_Buf))
    {
        return GET_NODE_TEXT_ERR;
    }
    if (0 != Convert_atoi(ptrNodeText_Buf,&dwLinkChannel))
    {
        return RETURN_ERROR;
    } 
    *pdwLinkChannel = dwLinkChannel;
    return RETURN_OK;
}

/**********************************************************************
* 函数名称：BYTE GetNIDNodeText
* 功能描述：获取网元ID(NID)节点文本
* 访问的表：无
* 修改的表：无
* 输入参数：LPXmlNode ptrNode :当前节点指针
                              WORD16 wLen:缓冲区大小
* 输出参数：CHAR *ptrNodeText_Buf:节点文本缓冲区
* 返 回 值：RETURN_ERROR-失败 RETURN_OK-成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT GetNIDNodeText(LPXmlNode ptrNode,WORD16 wLen,CHAR *ptrNodeText_Buf)
{
    LPXmlNode ptrNodeTmp = NULL;/*节点指针临时变量*/
    LPXmlNode ptrDtNode = NULL;/*目标节点指针*/
    XmlNode tDtNode;/*目标节点结构*/
	
    if ((ptrNode == NULL) || (wLen == 0) || (ptrNodeText_Buf == NULL))
    {
        return NULL_PTR;
    }

    ptrNodeTmp = ptrNode;
    ptrDtNode = &tDtNode;
    memset(ptrDtNode,0,sizeof(XmlNode));
    /*获取NID节点*/
    if (RETURN_OK != GetNIDNode(ptrNodeTmp,ptrDtNode))
    {
        return GET_NODE_ERR;
    }
    /*获取NID节点文本*/
    if (RETURN_OK != GetChildTextByNode(ptrDtNode,MAX_NODETEXT_BUF_SIZE,ptrNodeText_Buf))
    {
        return GET_NODE_TEXT_ERR;
    }
	
    return RETURN_OK;	
}

/**********************************************************************
* 函数名称：BYTE SetNIDNodeText
* 功能描述：设置网元ID(NID)节点文本
* 访问的表：无
* 修改的表：无
* 输入参数：LPXmlNode ptrNodeIn :当前节点指针            
* 输出参数：WORD32 *pdwNetId:网元标识
* 返 回 值：RETURN_ERROR-失败 RETURN_OK-成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT SetNIDNodeText(LPXmlNode ptrNodeIn,WORD32 *pdwNetId)
{
    LPXmlNode ptrNodeTmp = NULL;/*节点指针临时变量*/    
    CHAR ptrNodeText_Buf[MAX_NODETEXT_BUF_SIZE];/*节点文本缓冲区*/    
    WORD32 dwNetId = 0;
    if ((ptrNodeIn == NULL) || (pdwNetId== NULL))
    {
        return NULL_PTR;
    }
    ptrNodeTmp = ptrNodeIn;    
    memset(ptrNodeText_Buf,0,MAX_NODETEXT_BUF_SIZE);
    /*获取TRM节点文本*/
    if (RETURN_OK != GetNIDNodeText(ptrNodeTmp,MAX_NODETEXT_BUF_SIZE,ptrNodeText_Buf))
    {
        return GET_NODE_TEXT_ERR;
    }
    if (0 !=  Convert_atoi(ptrNodeText_Buf, &dwNetId))
    {
        return RETURN_ERROR;
    }
    *pdwNetId = dwNetId;    
    return RETURN_OK;
}

/**********************************************************************
* 函数名称：BYTE GetOTNodeText
* 功能描述：获取命令操作类型(OT)节点文本
* 访问的表：无
* 修改的表：无
* 输入参数：LPXmlNode ptrNode :当前节点指针
                              WORD16 wLen:缓冲区大小
* 输出参数：CHAR *ptrNodeText_Buf:节点文本缓冲区
* 返 回 值：RETURN_ERROR-失败 RETURN_OK-成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT GetOTNodeText(LPXmlNode ptrNode,WORD16 wLen,CHAR *ptrNodeText_Buf)
{
    LPXmlNode ptrNodeTmp = NULL;/*节点指针临时变量*/
    LPXmlNode ptrDtNode = NULL;/*目标节点指针*/
    XmlNode tDtNode;/*目标节点结构*/
	
    if ((ptrNode == NULL) || (wLen == 0) || (ptrNodeText_Buf == NULL))
    {
        return NULL_PTR;
    }

    ptrNodeTmp = ptrNode;
    ptrDtNode = &tDtNode;
    memset(ptrDtNode,0,sizeof(XmlNode));
    /*获取OT节点*/
    if (RETURN_OK != GetOTNode(ptrNodeTmp,ptrDtNode))
    {
        return GET_NODE_ERR;
    }
    /*获取OT节点文本*/
    if (RETURN_OK != GetChildTextByNode(ptrDtNode,MAX_NODETEXT_BUF_SIZE,ptrNodeText_Buf))
    {
        return GET_NODE_TEXT_ERR;
    }
	
    return RETURN_OK;	
}

/**********************************************************************
* 函数名称：BYTE SetOTNodeText
* 功能描述：设置命令操作类型(OT)节点文本
* 访问的表：无
* 修改的表：无
* 输入参数：LPXmlNode ptrNodeIn :当前节点指针  
* 输出参数：BYTE *pucIfNo:命令操作类型
* 返 回 值：RETURN_ERROR-失败 RETURN_OK-成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT SetOTNodeText(LPXmlNode ptrNode,BYTE *pucIfNo)
{
    LPXmlNode ptrNodeTmp = NULL;/*节点指针临时变量*/    
    CHAR ptrNodeText_Buf[MAX_NODETEXT_BUF_SIZE];/*节点文本缓冲区*/    
    CHAR *ptrDelCmdName = "D";/*命令名称-删除*/    
    BYTE bIfNoTmp;    
    
    if ((ptrNode == NULL) || (pucIfNo == NULL))
    {
        return NULL_PTR;
    }
	
    ptrNodeTmp = ptrNode;    
    /*获取OT节点文本*/
    if (RETURN_OK != GetOTNodeText(ptrNodeTmp,MAX_NODETEXT_BUF_SIZE,ptrNodeText_Buf))
    {
        return GET_NODE_TEXT_ERR;
    }    
    if (strcmp(ptrNodeText_Buf,ptrDelCmdName) == 0)
    {
        bIfNoTmp = TRUE;/*删除命令*/
    }	
    else
    {
  	 bIfNoTmp = FALSE;/*配置命令*/
    }
    *pucIfNo  = bIfNoTmp;   
	
    return RETURN_OK;
}


/**********************************************************************
* 函数名称：BYTE GetCMDParaNodeCount
* 功能描述：获取命令节点下参数个数
* 访问的表：无
* 修改的表：无
* 输入参数：LPXmlNode ptrNode:节点指针                      
* 输出参数：BYTE *bNumber:参数个数
* 返 回 值：RETURN_ERROR-失败 RETURN_OK-成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT GetCMDParaNodeCount(LPXmlNode ptrNode,BYTE *pucNumber)
{
    LPXmlNode ptrNodeTmp = NULL;/*节点指针临时变量*/
    LPXmlNode ptrDtNode = NULL;/*目标节点指针*/
    XmlNode tDtNode;/*目标节点结构*/
    CHAR *ptrNodeName = "P";
    WORD32 dwNumber = 0;
    
    if ((ptrNode == NULL) || (pucNumber == NULL))
    {
        return NULL_PTR;
    }
    ptrNodeTmp = ptrNode;
    ptrDtNode = &tDtNode;
    memset(ptrDtNode,0,sizeof(XmlNode));
    /*获取命令节点指针*/
    if (RETURN_OK != GetCMDNode(ptrNodeTmp,ptrDtNode))
    {
        return GET_NODE_ERR;
    }
    /*获取命令节点参数*/
    if (RETURN_OK != (GetChildCountByName(ptrDtNode,ptrNodeName,&dwNumber)))
    {
        return GET_NODE_CNT_ERR;
    }
    *pucNumber = (BYTE)dwNumber;
    return RETURN_OK;
}

/**********************************************************************
* 函数名称：XML_AGENT_STAT ParaseValidPrammer
* 功能描述：解析有效性语法
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_CUR_CMDINFO *ptCurCMDInfo
                             WORD32 dwParaNo:
* 输出参数：BOOLEAN *bIsValidParmar
                             TYPE_CMD_PARA *ptTypeCmdPara
* 返 回 值： RETURN_OK-成功 其他-失败
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2009-04-13             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT ParaseValidPrammer(TYPE_CMD_PARA *ptCmdParaInfo,TYPE_CMD_ATTR *ptCmdAttr,CHAR *ptrNodeText,BOOLEAN *pbIsValidParmar,OPR_DATA * ptrData)
{
    BOOLEAN bIsValidParmarTmp = FALSE;
    WORD16 wValue = 0;
    CMDPARA tCmdPara = {0};/*命令参数*/
    assert(ptCmdParaInfo && ptCmdAttr && pbIsValidParmar && ptrData);
    if ((NULL == ptCmdParaInfo) ||
	 (NULL == ptCmdAttr) ||
	 (NULL == pbIsValidParmar) || 
	 (NULL == ptrData))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__); 
        return NULL_PTR;
    }
    if(ptCmdParaInfo->bStructType == STRUCTTYPE_PRAMMAR &&
       ((ptCmdParaInfo->uContent.tPrammar.bPrammarType == PRAMMAR_BRACKET_SQUARE
    && (*(ptCmdParaInfo->uContent.tPrammar.sSymbol)) != MARK_STRINGEND)
    || ptCmdParaInfo->uContent.tPrammar.bPrammarType == PRAMMAR_WHOLE_SYMBOL))
    {
        if(ptCmdAttr->bExeModeID != EXEC_PROTOCOL)/*非协议占命令*/
        {
            bIsValidParmarTmp = TRUE;
            ConvertValidPrammerToCmdPara(ptCmdParaInfo,DATA_TYPE_PARA,wValue,&tCmdPara);
        }
        else
        {
            if(ptCmdParaInfo->uContent.tPrammar.bItemCount == 0 &&
               ptCmdParaInfo->uContent.tPrammar.bPrammarType != PRAMMAR_WHOLE_SYMBOL)
            {
                bIsValidParmarTmp = TRUE;
                ConvertValidPrammerToCmdPara(ptCmdParaInfo,DATA_TYPE_CONST,wValue,&tCmdPara);
            }
        }
    }
    else if(ptCmdParaInfo->bStructType == STRUCTTYPE_PARA)
    {
        if(ptCmdParaInfo->uContent.tPara.bParaType == DATA_TYPE_CONST)
        {
            bIsValidParmarTmp = TRUE;
            ConvertValidPrammerToCmdPara(ptCmdParaInfo,DATA_TYPE_CONST,wValue,&tCmdPara);
        }
        else if(ptCmdParaInfo->uContent.tPara.bParaType == DATA_TYPE_MAP)    /* data_type_map */
        {  
            WORD32 dwMapValue = 0;
            bIsValidParmarTmp = TRUE;
	     if (0 != Convert_atoi(ptrNodeText, &dwMapValue))
	     {
	         return RETURN_ERROR;
	     }
	     wValue = dwMapValue;
	     ConvertValidPrammerToCmdPara(ptCmdParaInfo,DATA_TYPE_MAP,wValue ,&tCmdPara);	     
        }
    }
    if (bIsValidParmarTmp)
    {
        ptrData->Type = tCmdPara.Type;
        ptrData->Len = tCmdPara.Len;    
        memcpy(ptrData->Data,tCmdPara.pValue,tCmdPara.Len);
    }
    *pbIsValidParmar = bIsValidParmarTmp;
    return RETURN_OK;    
}

/**********************************************************************
* 函数名称：VOID ConvertValidPrammerToCmdPara
* 功能描述：将有效性语法转化成命令参数
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_CMD_PARA *ptTypeCmdPara
                             BYTE bParaType
* 输出参数：CMDPARA *ptCmdPara
* 返 回 值： 无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2009-04-13             齐龙兆      创建
************************************************************************/
static VOID ConvertValidPrammerToCmdPara(TYPE_CMD_PARA *ptCmdParaInfo,BYTE bParaType,WORD16 wValue,CMDPARA *ptCmdPara)
{
    BYTE ucLinkID = MAX_CLI_VTY_NUMBER - MIN_CLI_VTY_NUMBER + 1;
    ptCmdPara->ParaNo        = ptCmdPara->ParaNo;				
    memset(ptCmdPara->sName, 0, sizeof(ptCmdPara->sName));
    strncpy(ptCmdPara->sName, ptCmdParaInfo->uContent.tPara.sName , sizeof(ptCmdPara->sName));
    ptCmdPara->Type          = bParaType;
    if (DATA_TYPE_MAP == bParaType)
    {
        ptCmdPara->Len    = sizeof(WORD16);
        ptCmdPara->pValue = (BYTE *)Oam_InptGetBuf(sizeof(WORD16),ucLinkID);
        *((WORD16 *)ptCmdPara->pValue) = wValue;
    }
    else
    {
        ptCmdPara->Len           = sizeof(BYTE);
        ptCmdPara->pValue       = (BYTE *)Oam_InptGetBuf(sizeof(BYTE),ucLinkID);
        *((BYTE *)ptCmdPara->pValue) = TRUE;
    }    
    return;
}

/**********************************************************************
* 函数名称：BOOLEAN CheckIsValidPrammar
* 功能描述：检查是否有效性语法
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_CMD_PARA *ptTypeCmdPara:
                              TYPE_CMD_ATTR *ptCmdAttr:                             
* 输出参数：无
* 返 回 值： TRUE/FALSE
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2009-04-13             齐龙兆      创建
************************************************************************/
static BOOLEAN CheckIsValidPrammar(TYPE_CMD_PARA *ptTypeCmdPara,TYPE_CMD_ATTR *ptCmdAttr)
{
    if((ptTypeCmdPara->bStructType == STRUCTTYPE_PRAMMAR) &&
        ((ptTypeCmdPara->uContent.tPrammar.bPrammarType == PRAMMAR_BRACKET_SQUARE && 
         (*(ptTypeCmdPara->uContent.tPrammar.sSymbol)) != MARK_STRINGEND) ||
         (ptTypeCmdPara->uContent.tPrammar.bPrammarType == PRAMMAR_WHOLE_SYMBOL)))
        {
            if(ptCmdAttr->bExeModeID != EXEC_PROTOCOL)/*EXEC_PROTOCOL*/
            {
                return TRUE;
            }
            else
            {
                if(ptTypeCmdPara->uContent.tPrammar.bItemCount == 0 &&
                   ptTypeCmdPara->uContent.tPrammar.bPrammarType != PRAMMAR_WHOLE_SYMBOL)
                {
                    return TRUE;
                }
		}
    	}
	return FALSE;
}

/**********************************************************************
* 函数名称：XML_AGENT_STAT GetCurTypeCmdPara
* 功能描述：获取当前命令参数类型
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_CUR_CMDINFO *ptCurCMDInfo:
                             DWORD dwParaNo:                             
* 输出参数：TYPE_CMD_PARA *ptTypeCmdPara
* 返 回 值： RETURN_OK-成功 其他-失败
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2009-04-13             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT GetCurTypeCmdPara(TYPE_CUR_CMDINFO *ptCurCMDInfo,DWORD dwParaNo,TYPE_CMD_PARA *ptTypeCmdPara)
{
    TYPE_CUR_CMDINFO *ptCurCMDInfoTmp = NULL;/*命令脚本临时指针*/     
    WORD16 wCmdAttrPos = 0;/*命令属性索引*/
    DWORD dwParaPos = 0;  /*参数列表位置*/
    DWORD dwNoParaPos = 0;/*No命令参数起始位置*/
    DWORD dwEndParaPos = 0;/*命令参数结束位置*/
    DWORD dwParaSum = 0;     /*参数个数*/
    T_OAM_Cfg_Dat * ptcfgDat = NULL; /*DAT脚本指针*/

    /*参数检查*/
    if ((NULL == ptCurCMDInfo) || 
        (NULL == ptCurCMDInfo->cfgDat) || 
        (NULL == ptCurCMDInfo->cfgDat->tCmdAttr) ||
        (NULL == ptTypeCmdPara))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__); 
        return NULL_PTR;
    }
    /*变量初始化*/
    ptCurCMDInfoTmp = ptCurCMDInfo;    
    ptcfgDat = ptCurCMDInfoTmp->cfgDat;
    wCmdAttrPos = ptCurCMDInfoTmp->wCmdAttrPos;
    /*数组越界检查*/
    if (wCmdAttrPos >= ptCurCMDInfo->cfgDat->nCmdAttrSum)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d Array Over Flow!%d-%d ....................\n",__func__,__LINE__,wCmdAttrPos,ptCurCMDInfo->cfgDat->nCmdAttrSum); 
        return RETURN_ERROR;
    }
    /*获取命令参数的起始/结束位置、参数个数*/
    dwParaPos = ptcfgDat->tCmdAttr[wCmdAttrPos].dwParaPos;
    dwNoParaPos = dwParaPos + ptcfgDat->tCmdAttr[wCmdAttrPos].wCmdParaCount;
    dwEndParaPos = dwParaPos + ptcfgDat->tCmdAttr[wCmdAttrPos].wAllParaCount;    
    if (ptCurCMDInfoTmp->bIsNoCmd)/*NO命令*/
    {
        dwParaPos = dwNoParaPos;
	 dwParaSum = ptcfgDat->tCmdAttr[wCmdAttrPos].wNoParaCount;
    }
    else
    {
        dwParaSum = ptcfgDat->tCmdAttr[wCmdAttrPos].wCmdParaCount;
    }
    for(;dwParaSum > 0;dwParaSum--,dwParaPos++)
    {
        /*参数编号相等*/
	 if (dwParaNo == ptcfgDat->tCmdPara[dwParaPos].ParaNo)
	 {
	     TYPE_CMD_PARA *ptCmdPara = &(ptcfgDat->tCmdPara[dwParaPos]);
	     /*参数编号为0 且为语法*/
	     if ((0 == dwParaNo) &&
		  (STRUCTTYPE_PRAMMAR == ptCmdPara->bStructType))
	     {                
                TYPE_CMD_ATTR * ptCmdAttr = &(ptcfgDat->tCmdAttr[wCmdAttrPos]);
		   /*语法有效则退出,否则跳过当前语法*/
                if (CheckIsValidPrammar(ptCmdPara, ptCmdAttr))
	         {
	             break;
	         }	         	  
	     }
	     else
	     {
	         break;
	     }
	 }
    }
    if (0 == dwParaSum)/*没有找到*/
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d Can not find command parameter! ....................",__func__,__LINE__);
        return RETURN_ERROR;
    }
    if (dwParaPos > dwEndParaPos)/*命令参数位置超出*/
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d Command parameter position overflow! ....................",__func__,__LINE__);
        return RETURN_ERROR;
    }
    memcpy(ptTypeCmdPara,&(ptCurCMDInfoTmp->cfgDat->tCmdPara[dwParaPos]),sizeof(TYPE_CMD_PARA));           
    return RETURN_OK;
}

/**********************************************************************
* 函数名称：BYTE ConvertXmlStrToCmdPara
* 功能描述：将字符串转换成命令参数值
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_CMD_DATINFO *tDatInfo:命令脚本指针
            CHAR * ptrNodeText:参数内容
            BYTE bParaNo:参数序号
* 输出参数：OPR_DATA * pData:参数指针
* 返 回 值：0: success  1: convert error  2: overflow
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static INT ConvertXmlStrToCmdPara(TYPE_CUR_CMDINFO *ptCurCMDInfo,CHAR * ptrNodeText,WORD32 dwParaNo,OPR_DATA * ptrData)
{	 	  
    TYPE_CUR_CMDINFO *ptCurCMDInfoTmp = NULL;/*命令脚本临时指针*/
    CHAR * ptrNodeTextTmp = NULL;/*参数内容临时指针*/    
    TYPE_PARA *ptParaInfo = NULL;/*参数信息*/
    TYPE_CMD_PARA tTypeCmdPara;/*命令类型参数*/
    CMDPARA tCmdPara;/*命令参数*/
    TYPE_CMD_ATTR * ptCmdAttr;/*命令属性*/
    BOOLEAN bIsValidParmar = FALSE;    
    int nReturnVal = PARA_SUCCESS;
    BOOL bSupportBlank = TRUE;
    BYTE ucLinkID = MAX_CLI_VTY_NUMBER - MIN_CLI_VTY_NUMBER + 1;
	  
    if ((ptCurCMDInfo == NULL) ||
        (NULL == ptCurCMDInfo->cfgDat) || 
        (NULL == ptCurCMDInfo->cfgDat->tCmdAttr) || 
        (ptrNodeText == NULL) || 
        (ptrData == NULL))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC: %s,Line:%d NULL PTR!....................",__func__,__LINE__);
        return PARAERR_CONVERT;
    }
    
    ptCurCMDInfoTmp = ptCurCMDInfo;
    ptrNodeTextTmp = ptrNodeText;     
    memset(&tTypeCmdPara,0,sizeof(TYPE_CMD_PARA));
    memset(&tCmdPara,0,sizeof(CMDPARA));
    /*数组越界检查*/
    if (ptCurCMDInfoTmp->wCmdAttrPos >= ptCurCMDInfoTmp->cfgDat->nCmdAttrSum)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC: %s,Line:%d Array Over Flow!%d-%d....................\n",__func__,__LINE__,ptCurCMDInfoTmp->wCmdAttrPos,ptCurCMDInfoTmp->cfgDat->nCmdAttrSum);
        return PARAERR_CONVERT;
    }
    ptCmdAttr = &(ptCurCMDInfoTmp->cfgDat->tCmdAttr[ptCurCMDInfoTmp->wCmdAttrPos]);
	
    /*获取命令参数信息*/
    if (RETURN_OK != GetCurTypeCmdPara(ptCurCMDInfoTmp,dwParaNo,&tTypeCmdPara))    
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC: %s,Line:%d GetCurTypeCmdPara Error!....................",__func__,__LINE__);
        return PARAERR_CONVERT;
    }
    /*本地内存指针复位*/
    Oam_InptFreeBuf(ucLinkID);
    /*有效语法转换为命令参数*/
    if (RETURN_OK != ParaseValidPrammer(&tTypeCmdPara, ptCmdAttr,ptrNodeTextTmp, &bIsValidParmar, ptrData))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC: %s,Line:%d ParaseValidPrammer Error!....................",__func__,__LINE__);
        return PARAERR_CONVERT;
    }
    if (bIsValidParmar)
    {        
        return nReturnVal;
    }
    ptParaInfo = &(tTypeCmdPara.uContent.tPara);    
    
    /* 保存全局变量的值 */
    bSupportBlank = g_ptOamIntVar->bSupportStringWithBlank;
    /* 对OMC后台如果输入有空格的标识符，则提示输入错误 */
    g_ptOamIntVar->bSupportStringWithBlank = FALSE;
    /*根据命令参数类型转换参数内容*/
    nReturnVal = ConvertStrToCmdParaByParaType(ucLinkID,ptrNodeTextTmp,ptCurCMDInfo->cfgDat, ptParaInfo, ptrData->ParaNo, &tCmdPara,DM_NORMAL);
    /* 再将全局变量的值恢复原先的值 */
    g_ptOamIntVar->bSupportStringWithBlank = bSupportBlank;
    if (PARA_SUCCESS != nReturnVal)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                   "....................ConvertStrToCmdParaByParaType Error FUNC: %s,Line:%d!....................",__func__,__LINE__);            
        return PARAERR_CONVERT;
    }
    ptrData->Type = tCmdPara.Type;
    ptrData->Len = tCmdPara.Len;    
    memcpy(ptrData->Data,tCmdPara.pValue,tCmdPara.Len);
    return nReturnVal;
}

/**********************************************************************
* 函数名称：BYTE GetParaNodeText
* 功能描述：获取参数(P)节点文本
* 访问的表：无
* 修改的表：无
* 输入参数：LPXmlNode ptrNode:节点指针            
                              BYTE bParaNo:命令序号
                              WORD16 wLen:缓冲区长度
* 输出参数：CHAR *ptrNodeText_Buf:节点文本缓冲区
* 返 回 值：RETURN_ERROR-失败 RETURN_OK-成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT GetParaNodeText(LPXmlNode ptrNode,WORD32 dwParaNo,WORD16 wLen,CHAR *ptrNodeText_Buf)
{
    LPXmlNode ptrNodeTmp = NULL;/*节点指针临时变量*/  
    XmlNode tDtNode;/*目标节点结构变量*/ 
    LPXmlNode ptrDtNode = NULL;/*目标节点指针*/ 

    if ((ptrNode == NULL) || (wLen == 0) || (ptrNodeText_Buf == NULL))
    {
        return NULL_PTR;
    }
    ptrNodeTmp = ptrNode;
    ptrDtNode = &tDtNode;
    memset(ptrDtNode,0,sizeof(XmlNode));
    /*获取参数节点指针*/
    if (RETURN_OK != GetPARANode(ptrNodeTmp,dwParaNo,ptrDtNode))
    {
        return GET_NODE_ERR;
    }
    /*获取参数节点文本*/    
    if (RETURN_OK != GetChildTextByNode(ptrDtNode,wLen,ptrNodeText_Buf))
    {
        return GET_NODE_TEXT_ERR;
    }
    return RETURN_OK;
}

/**********************************************************************
* 函数名称：XML_AGENT_STAT SetParaNodeAttr
* 功能描述：获取参数(P)节点属性
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_CMD_DATINFO *tDatInfo:命令脚本指针
                              LPXmlNode ptrNode:节点指针
                              BYTE bParaNo:命令序号
* 输出参数：OPR_DATA * pData:参数指针
* 返 回 值：RETURN_ERROR-失败 RETURN_OK-成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT SetParaNodeAttr(TYPE_CUR_CMDINFO *ptCurCMDInfo,LPXmlNode ptrNode,BYTE bParaNo,OPR_DATA * pData)
{
    LPXmlNode ptrNodeTmp = NULL;/*节点指针临时变量*/     
    CHAR ptrNodeText_Buf[MAX_NODETEXT_BUF_SIZE];/*节点文本缓冲区*/
    WORD32 dwParaNo = 0;
    
    if ((ptrNode == NULL) || (ptCurCMDInfo == NULL) || (pData == NULL))
    {
        return NULL_PTR;
    }
    ptrNodeTmp = ptrNode;    
    memset(ptrNodeText_Buf,0,MAX_NODETEXT_BUF_SIZE);
    dwParaNo = (WORD32)bParaNo;
    /*获取参数节点文本*/    
    if (RETURN_OK != GetParaNodeText(ptrNodeTmp,dwParaNo,MAX_NODETEXT_BUF_SIZE,ptrNodeText_Buf))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................GetParaNodeText Error FUNC: %s,Line:%d!....................",__func__,__LINE__);
        return GET_NODE_TEXT_ERR;
    }
    /*参数内容类型转换，并填写参数长度和参数名*/
    if (PARA_SUCCESS != ConvertXmlStrToCmdPara(ptCurCMDInfo,ptrNodeText_Buf,pData->ParaNo,pData))
    {
        return RETURN_ERROR;
    }
    return RETURN_OK;
}

/**********************************************************************
* 函数名称：BYTE CompareChildTextByName
* 功能描述：比较节点名称同指定的字符串的大小
* 访问的表：无
* 修改的表：无
* 输入参数：LPXmlNode ptrNode:指定节点指针
            CHAR *ptrCompText :待比较文本
* 输出参数：无
* 返 回 值：RETURN_ERROR-失败 RETURN_OK-成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT CompareChildTextByName(LPXmlNode ptrNode,CHAR *ptrCompText,BOOLEAN *pbIs3GPL)
{
    LPXmlNode ptrNodeTmp = NULL;/*节点指针临时变量*/
    CHAR *ptrCompTextTmp = NULL;/*待比较文本临时变量*/
    CHAR ptrNodeText_Buf[MAX_NODETEXT_BUF_SIZE];/*节点文本缓冲区*/
	  
    if ((NULL == ptrNode) || (NULL == ptrCompText) || (NULL == pbIs3GPL))
    {
        return NULL_PTR;
    }
    ptrNodeTmp  = ptrNode;
    ptrCompTextTmp = ptrCompText;	  
    memset(ptrNodeText_Buf,0,MAX_NODETEXT_BUF_SIZE);
    if (RETURN_OK != GetChildTextByNode(ptrNodeTmp,MAX_NODETEXT_BUF_SIZE,ptrNodeText_Buf))
    {
        return GET_NODE_TEXT_ERR;
    }    
    if (strcmp(ptrCompTextTmp,ptrNodeText_Buf) != 0)/*两字符不等*/
    {
        *pbIs3GPL = FALSE;
    }
    return RETURN_OK;
}

/**********************************************************************
* 函数名称：BOOLEAN CheckNtpNodeIs3GPL
* 功能描述：判断NTP节点下文本是否为3GPL
* 访问的表：无
* 修改的表：无
* 输入参数：LPXmlNode ptrNodeIn:指定节点指针            
* 输出参数：BOOLEAN *pbIs3GPL:网元类型是否3GPL
* 返 回 值   ：XML_AGENT_STAT
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT CheckNtpNodeTextIs3GPL(LPXmlNode ptrNodeIn,BOOLEAN *pbIs3GPL)
{
    LPXmlNode ptrNodeTmp = NULL;/*节点指针临时变量*/
    LPXmlNode ptrDtNode = NULL;   /*目标节点指针*/
    XmlNode tXmlDtNode;               /*目标节点结构体*/
    CHAR *ptrNtpText = "3GPL";     /*待处理的节点文本*/
    if ((NULL == ptrNodeIn) || (NULL == pbIs3GPL))
    {
        return NULL_PTR;
    }
    ptrNodeTmp = ptrNodeIn;
    ptrDtNode = &tXmlDtNode;
    memset(ptrDtNode,0,sizeof(XmlNode));
    if (RETURN_OK != GetNTPNode(ptrNodeTmp,ptrDtNode))
    {
        return GET_NODE_ERR;
    }    
    if (RETURN_OK != CompareChildTextByName(ptrDtNode,ptrNtpText,pbIs3GPL))
    {
        return RETURN_ERROR;
    }
    return RETURN_OK;
}

/**********************************************************************
* 函数名称：BYTE ReseachTree
* 功能描述：递归遍历命令树查找对应的JID
* 访问的表：无
* 修改的表：无
* 输入参数：WORD16 wNodePos:命令树节点下标
                              DWORD dwCmdId:命令ID
* 输出参数：WORD16 **ppOutNodePos:命令树节点下标
* 返 回 值：0-失败 1-成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT ReseachTree(WORD16 wNodePos,DWORD dwCmdId,WORD16 **ppOutNodePos)
{
    WORD16 wCmdAttrPos = 0;/*命令属性索引*/
    WORD16 wNodePosTmp = 0;/*节点下标*/
    T_TreeNode *ptTreeNodeTmp = NULL;
    T_TreeNode *ptNextNodeTmp = NULL;
	
    if ((NULL == ppOutNodePos) ||
        (NULL == g_ptOamIntVar))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__); 
        return NULL_PTR;
    }
    wNodePosTmp = wNodePos;
    /*数组越界检查*/
    if (wNodePosTmp >= g_ptOamIntVar->dwTreeNodeSum)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d Array Over Flow!%d-%d ....................\n",__func__,__LINE__,wNodePosTmp,g_ptOamIntVar->dwTreeNodeSum); 
        return RETURN_ERROR;
    }
    ptTreeNodeTmp = &gtTreeNode[wNodePosTmp];    
    /*空指针检查*/
    if ((NULL == ptTreeNodeTmp->cfgDat) ||
        (NULL == ptTreeNodeTmp->cfgDat->tCmdAttr))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__); 
        return NULL_PTR;
    }
    wCmdAttrPos = ptTreeNodeTmp->treenode.wCmdAttrPos;
    /*数组越界检查*/
    if (wCmdAttrPos >= ptTreeNodeTmp->cfgDat->nCmdAttrSum)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d Array Over Flow!%d-%d ....................\n",__func__,__LINE__,wCmdAttrPos,ptTreeNodeTmp->cfgDat->nCmdAttrSum); 
        return RETURN_ERROR;
    }
    if (dwCmdId == ptTreeNodeTmp->cfgDat->tCmdAttr[wCmdAttrPos].dExeID)
    {
        **ppOutNodePos = wNodePosTmp;	 
        return RETURN_OK;
    }
    /*命令树的子节点不为空*/
    wNodePosTmp = ptTreeNodeTmp->treenode.wNextSonPos;    
    /*数组越界检查*/
    if (wNodePosTmp >= g_ptOamIntVar->dwTreeNodeSum)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d Array Over Flow!%d-%d ....................\n",__func__,__LINE__,wNodePosTmp,g_ptOamIntVar->dwTreeNodeSum); 
        return RETURN_ERROR;
    }
    ptNextNodeTmp = &gtTreeNode[wNodePosTmp];
    if (!(ptNextNodeTmp->treenode.wCmdAttrPos == 0 &&
            ptNextNodeTmp->treenode.wNextNodePos == 0 &&
            ptNextNodeTmp->treenode.wNextSonPos == 0))
    {
        if (RETURN_OK ==ReseachTree(wNodePosTmp,dwCmdId,ppOutNodePos))
        {
            return RETURN_OK;
        }
    }
    /*命令树的兄弟节点不为空*/
    wNodePosTmp = ptTreeNodeTmp->treenode.wNextNodePos;
    /*数组越界检查*/
    if (wNodePosTmp >= g_ptOamIntVar->dwTreeNodeSum)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d Array Over Flow!%d-%d ....................\n",__func__,__LINE__,wNodePosTmp,g_ptOamIntVar->dwTreeNodeSum); 
        return RETURN_ERROR;
    }
    ptNextNodeTmp = &gtTreeNode[wNodePosTmp];	 
    if (!(ptNextNodeTmp->treenode.wCmdAttrPos == 0 &&
            ptNextNodeTmp->treenode.wNextNodePos == 0 &&
            ptNextNodeTmp->treenode.wNextSonPos == 0))
    {
    	  if (RETURN_OK == ReseachTree(wNodePosTmp,dwCmdId,ppOutNodePos))
    	  {
    	      return RETURN_OK;
    	  }
    }
    return RETURN_ERROR;
}

/**********************************************************************
* 函数名称：BYTE GetTreeNode
* 功能描述：根据模块号和命令号获取命令树节点
* 访问的表：无
* 修改的表：无
* 输入参数：BYTE ucModeId:模式ID
            DWORD dwCmdId:命令ID
* 输出参数：WORD16 *pOutNodePos:命令树节点下标
* 返 回 值：0-失败 1-成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT GetTreeNode(BYTE bModeId,DWORD dwCmdId,WORD16 *pOutNodePos)
{
    DWORD dwIndex = 0;
    T_TreeNode tTreeNode;
    WORD16 wOutNodePosTmp = 0;
    WORD16 *pOutNodePosTmp = &wOutNodePosTmp; 
    WORD16 wNodePosTmp = 0;

    if (NULL == pOutNodePos)
    {
        return NULL_PTR;
    }
    pOutNodePosTmp = pOutNodePos;
    memset(&tTreeNode,0,sizeof(T_TreeNode));
    /*根据模式ID查找对应模式的命令树根位置*/
    for(dwIndex= 0;dwIndex <= g_dwModeCount;dwIndex++)
    {
        if(gtMode[dwIndex].bModeID == bModeId)
        {
        	  tTreeNode = gtTreeNode[gtMode[dwIndex].wTreeRootPos];
            break;
        }
    }
    if (dwIndex > g_dwModeCount)
    {
        return GET_NODE_ERR;
    }
    wNodePosTmp = tTreeNode.treenode.wNextSonPos;  
    
    /*递归遍历命令树,得到命令节点*/
    if (RETURN_OK != ReseachTree(wNodePosTmp,dwCmdId,&pOutNodePosTmp))
    {
        return GET_NODE_ERR;
    }
    return RETURN_OK;
}

/**********************************************************************
* 函数名称：BYTE SetCurCMDInfo
* 功能描述：获取当前命令信息
* 输入参数：LPXmlNode ptrNode:节点指针
* 输出参数：TYPE_CUR_CMDINFO *ptCurCMDInfo:当前命令信息
* 返 回 值： 0-失败 RETURN_OK-成功
* 其它说明：无
************************************************************************/
static XML_AGENT_STAT SetCurCMDInfo(TYPE_XMLLINKSTATE *ptLinkState)
{    
    BYTE  bModeId = 0;/*模式ID号*/        
    DWORD dwCmdId = 0;/*命令ID*/
    WORD16 wTreeNodePos = 0;
    WORD16 wCmdAttrPos = 0;   

    if (NULL == ptLinkState)
    {
        return NULL_PTR;
    }    
    
    /*针对查询模式栈信息特殊处理*/    
    dwCmdId = ptLinkState->dwCmdId;
    if (CMD_LDB_GET_MODESTATCK == dwCmdId)
    {
        bModeId = CMD_MODE_PRIV;/*特权模式*/
    }
    else if (IsSaveCmd(dwCmdId))
    {
        bModeId = CMD_MODE_MAINTAINANCE;/*维护模式*/
    }
    else
    {
        bModeId = ptLinkState->bModeID[ptLinkState->bModeStackTop -1];    
    }
    /*根据模式ID和命令ID查找命令树节点*/
    if (RETURN_OK != GetTreeNode(bModeId,dwCmdId,&wTreeNodePos))
    {        
        return SHOW_MODESTACK_ERR;
    }
    /*数组越界检查*/
    if (wTreeNodePos >= g_ptOamIntVar->dwTreeNodeSum)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d Array Over Flow!%d-%d ....................\n",__func__,__LINE__,wTreeNodePos,g_ptOamIntVar->dwTreeNodeSum); 
        return RETURN_ERROR;
    }
    
    /*空指针检查*/
    if ((NULL == gtTreeNode[wTreeNodePos].cfgDat) || 
        (NULL == gtTreeNode[wTreeNodePos].cfgDat->tCmdAttr))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__); 
        return NULL_PTR;
    }
    wCmdAttrPos = gtTreeNode[wTreeNodePos].treenode.wCmdAttrPos;
    /*数组越界检查*/
    if (wCmdAttrPos >= gtTreeNode[wTreeNodePos].cfgDat->nCmdAttrSum)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d Array Over Flow!%d-%d ....................\n",__func__,__LINE__,wCmdAttrPos,gtTreeNode[wTreeNodePos].cfgDat->nCmdAttrSum); 
        return RETURN_ERROR;
    }
    /*填充当前命令信息*/
    ptLinkState->tCurCMDInfo.wCmdAttrPos = wCmdAttrPos;
    ptLinkState->tCurCMDInfo.cfgDat = gtTreeNode[wTreeNodePos].cfgDat;
    ptLinkState->tCurCMDInfo.wParaBeginPos = (WORD16)(ptLinkState->tCurCMDInfo.cfgDat->tCmdAttr[wCmdAttrPos].dwParaPos);
    ptLinkState->tCurCMDInfo.wParaEndPos = (WORD16)(ptLinkState->tCurCMDInfo.cfgDat->tCmdAttr[wCmdAttrPos].dwParaPos + ptLinkState->tCurCMDInfo.cfgDat->tCmdAttr[wCmdAttrPos].wAllParaCount);
    return RETURN_OK;
    
}

/**********************************************************************
* 函数名称：BYTE SetOTNodeText
* 功能描述：设置OT节点名称及文本
* 访问的表：无
* 修改的表：无
* 输入参数：MSG_COMM_OAM *ptrMsgBuf：OAM消息
* 输出参数：CHAR *ptrXmlBuf:XML字符串
* 返 回 值：RETURN_ERROR-失败 RETURN_OK-成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT SetOTNode(CHAR *ptrXmlBuf,WORD16 wXmlBufLen)
{    
    CHAR *ptrXmlBufTmp = NULL;
    CHAR *pStrNodeNameHead = "<OT>";
    CHAR *pStrNodeText = "RP";/*固定为RP*/
    CHAR *pStrNodeNameEnd = "</OT>";    
    if (NULL == ptrXmlBuf)
    {
        return NULL_PTR;
    }   
    ptrXmlBufTmp = ptrXmlBuf;
    /*拷贝OT节点头*/
    strncat(ptrXmlBufTmp,pStrNodeNameHead,wXmlBufLen);
    /*拷贝OT节点文本*/
    strncat(ptrXmlBufTmp,pStrNodeText,wXmlBufLen);
    /*拷贝OT节点尾*/
    strncat(ptrXmlBufTmp,pStrNodeNameEnd,wXmlBufLen);
    
    return RETURN_OK;
}

/**********************************************************************
* 函数名称：BYTE SetORNode
* 功能描述：设置OR节点名称及文本
* 访问的表：无
* 修改的表：无
* 输入参数：MSG_COMM_OAM *ptrMsgBuf：OAM消息
* 输出参数：CHAR *ptrXmlBuf:XML字符串
* 返 回 值：RETURN_ERROR-失败 RETURN_OK-成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT SetORNode(MSG_COMM_OAM *ptrMsgBuf,CHAR *ptrXmlBuf,WORD16 wXmlBufLen)
{
    MSG_COMM_OAM *ptrMsgBufTmp = NULL;/*OAM通用结构体临时指针*/
    CHAR *ptrXmlBufTmp = NULL;
    CHAR *pStrNodeNameHead = "<OR>";
    CHAR *pStrNodeText = "";
    CHAR *pStrNodeNameEnd = "</OR>";    
	
    if ((ptrMsgBuf == NULL) || (ptrXmlBuf == NULL))
    {
        return NULL_PTR;
    }
    ptrMsgBufTmp = ptrMsgBuf;
    ptrXmlBufTmp = ptrXmlBuf;
    /*拷贝OR节点头*/
    strncat(ptrXmlBufTmp,pStrNodeNameHead,wXmlBufLen);
    /*拷贝OR节点文本*/
    switch(ptrMsgBufTmp->ReturnCode)		 	
    {
        case SUCC_AND_NOPARA:
	 case SUCC_AND_HAVEPARA:
	 case SUCC_CMD_NOT_FINISHED:
	 case SUCC_WAIT_AND_HAVEPARA:	 
	     pStrNodeText = (CHAR *)"OK";
	     break;
	 case ERR_AND_HAVEPARA:		  
	     pStrNodeText = (CHAR *)"NOK";
	     break;
	 default:
	     pStrNodeText = (CHAR *)"NS";
	     break;
    }    
    strncat(ptrXmlBufTmp,pStrNodeText,wXmlBufLen);        
    /*拷贝OR节点尾*/
    strncat(ptrXmlBufTmp,pStrNodeNameEnd,wXmlBufLen);   
	
    return RETURN_OK;
}

static XML_AGENT_STAT SetORNodeForPlan(CHAR *ptrXmlBuf)
{
    CHAR *pXmlStr = NULL;
    CHAR *pStrNodeNameHead = "<OR>";
    CHAR *pStrNodeText = "";
    CHAR *pStrNodeNameEnd = "</OR>";    
    WORD16 wXmlStrLen = MAX_SIZE_OMMMSG - sizeof(WORD16) -1;
    
    if (ptrXmlBuf == NULL)
    {
        return NULL_PTR;
    }
    pXmlStr = ptrXmlBuf;
    /*拷贝OR节点头*/
    strncat(pXmlStr,pStrNodeNameHead,wXmlStrLen);
    /*拷贝OR节点文本*/
    pStrNodeText = (CHAR *)"OK";
    strncat(pXmlStr,pStrNodeText,wXmlStrLen);        
    /*拷贝OR节点尾*/
    strncat(pXmlStr,pStrNodeNameEnd,wXmlStrLen);   
    
    return RETURN_OK;
}

/**********************************************************************
* 函数名称：BYTE SetDCNode
* 功能描述：设置DC节点名称及文本
* 访问的表：无
* 修改的表：无
* 输入参数：MSG_COMM_OAM *ptrMsgBuf：OAM消息
                              WORD16 wIndex:连接状态数组下标
* 输出参数：CHAR *ptrXmlBuf:XML字符串
* 返 回 值：RETURN_ERROR-失败 RETURN_OK-成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT SetDCNode(MSG_COMM_OAM *ptrMsgBuf,CHAR *ptrXmlBuf,WORD16 wXmlBufLen)
{
    MSG_COMM_OAM *ptrMsgBufTmp = NULL;/*OAM通用结构体临时指针*/
    CHAR *ptrXmlBufTmp = NULL;
    CHAR *pStrNodeNameHead = "<DC>";
    CHAR *pStrNodeText= "0";
    CHAR *pStrNodeNameEnd = "</DC>";    
	
    if ((ptrMsgBuf == NULL) || (ptrXmlBuf == NULL))
    {
        return NULL_PTR;
    }
    ptrMsgBufTmp = ptrMsgBuf;
    ptrXmlBufTmp = ptrXmlBuf;
    /*拷贝DC节点头*/
    strncat(ptrXmlBufTmp,pStrNodeNameHead,wXmlBufLen);    
    /*拷贝DC节点文本*/
    strncat(ptrXmlBufTmp,pStrNodeText,wXmlBufLen);
    /*拷贝DC节点尾*/
    strncat(ptrXmlBufTmp,pStrNodeNameEnd,wXmlBufLen);
	
    return RETURN_OK;
}

XML_AGENT_STAT SetDCNodeForPlan(CHAR *ptrXmlBuf)
{
    CHAR *pXmlStr = NULL;
    CHAR *pStrNodeNameHead = "<DC>";
    CHAR *pStrNodeText= "0";
    CHAR *pStrNodeNameEnd = "</DC>";    
    WORD16 wXmlStrLen = MAX_SIZE_OMMMSG - sizeof(WORD16) -1;
    
    if (ptrXmlBuf == NULL)
    {
        return NULL_PTR;
    }
    pXmlStr = ptrXmlBuf;
    /*拷贝DC节点头*/
    strncat(pXmlStr,pStrNodeNameHead,wXmlStrLen);    
    /*拷贝DC节点文本*/
    strncat(pXmlStr,pStrNodeText,wXmlStrLen);    
    /*拷贝DC节点尾*/
    strncat(pXmlStr,pStrNodeNameEnd,wXmlStrLen);
    
    return RETURN_OK;
}

/**********************************************************************
* 函数名称：BYTE SetEOFNode
* 功能描述：设置EOF节点名称及文本
* 访问的表：无
* 修改的表：无
* 输入参数：MSG_COMM_OAM *ptrMsgBuf：OAM消息
* 输出参数：CHAR *ptrXmlBuf:XML字符串
* 返 回 值：RETURN_ERROR-失败 RETURN_OK-成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT SetEOFNode(MSG_COMM_OAM *ptrMsgBuf,CHAR *ptrXmlBuf,WORD16 wXmlBufLen)
{
    MSG_COMM_OAM *ptrMsgBufTmp = NULL;/*OAM通用结构体临时指针*/
    CHAR *ptrXmlBufTmp = NULL;
    CHAR *pStrNodeNameHead = "<EOF>";
    CHAR *pStrNodeNameEnd = "</EOF>";   
	
    if ((ptrMsgBuf == NULL) || (ptrXmlBuf == NULL))
    {
        return NULL_PTR;
    }
    ptrMsgBufTmp = ptrMsgBuf;
    ptrXmlBufTmp = ptrXmlBuf;
    /*拷贝EOF节点头*/
    strncat(ptrXmlBufTmp,pStrNodeNameHead,wXmlBufLen);    
    /*拷贝EOF节点文本*/
    switch(ptrMsgBufTmp->ReturnCode)
    {
        case SUCC_WAIT_AND_HAVEPARA:
	 case SUCC_CMD_NOT_FINISHED:
	     strncat(ptrXmlBufTmp,"N",wXmlBufLen); 
	     break;
        case SUCC_AND_NOPARA:
        case SUCC_AND_HAVEPARA:
	     strncat(ptrXmlBufTmp,"Y",wXmlBufLen); 
	     break;
	 default:
	     strncat(ptrXmlBufTmp,"Y",wXmlBufLen);
	     break;		  
    }    
    /*拷贝EOF节点尾*/
    strncat(ptrXmlBufTmp,pStrNodeNameEnd,wXmlBufLen);
	
    return RETURN_OK;
}

static XML_AGENT_STAT SetEOFNodeForPlan(CHAR *ptrXmlBuf)
{
    CHAR *pXmlStr = NULL;
    CHAR *pStrNodeNameHead = "<EOF>";
    CHAR *pStrNodeNameEnd = "</EOF>";   
    WORD16 wXmlStrLen = MAX_SIZE_OMMMSG - sizeof(WORD16) -1;
    
    if (ptrXmlBuf == NULL)
    {
        return NULL_PTR;
    }
    pXmlStr = ptrXmlBuf;
    /*拷贝EOF节点头*/
    strncat(pXmlStr,pStrNodeNameHead,wXmlStrLen);    
    /*拷贝EOF节点文本*/
    strncat(pXmlStr,"N",wXmlStrLen);
    /*拷贝EOF节点尾*/
    strncat(pXmlStr,pStrNodeNameEnd,wXmlStrLen);    
    
    return RETURN_OK;
}

/**********************************************************************
* 函数名称：BYTE SetMDNode
* 功能描述：设置MD节点名称及文本
* 访问的表：无
* 修改的表：无
* 输入参数：MSG_COMM_OAM *ptrMsgBuf：OAM消息
                              WORD16 wIndex:连接状态数组下标
* 输出参数：CHAR *ptrXmlBuf:XML字符串
* 返 回 值：RETURN_ERROR-失败 RETURN_OK-成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT SetMDNode(CHAR *ptrXmlBuf,WORD16 wXmlBufLen)
{
    CHAR *ptrXmlBufTmp = NULL;
    CHAR *pStrNodeNameHead = "<MD>";
    CHAR *pStrNodeNameEnd = "</MD>";
    CHAR *pStrResultHead = "<RSTR>";
    CHAR *pStrResultEnd = "</RSTR>";    
    
    if (NULL == ptrXmlBuf)
    {
        return NULL_PTR;
    }    
    ptrXmlBufTmp = ptrXmlBuf;
    
    /*拷贝MD节点头*/
    strncat(ptrXmlBufTmp,pStrNodeNameHead,wXmlBufLen);
    /*拷贝RSTR节点头*/    
    strncat(ptrXmlBufTmp,pStrResultHead,wXmlBufLen);
    /*拷贝RSTR节点尾*/
    strncat(ptrXmlBufTmp,pStrResultEnd,wXmlBufLen);
    /*拷贝MD节点尾*/
    strncat(ptrXmlBufTmp,pStrNodeNameEnd,wXmlBufLen);
    return RETURN_OK;
}

static XML_AGENT_STAT SetMDNodeForPlan(T_Cmd_Plan_Cfg *ptrMsgBuf,TYPE_XMLLINKSTATE *ptLinkState,CHAR *ptrXmlBuf)
{
    T_Cmd_Plan_Cfg *pPlanInfo = NULL;/*OAM通用结构体临时指针*/
    CHAR *pXmlStr = NULL;
    CHAR *pStrNodeNameHead = "<MD>";
    CHAR *pStrNodeText = "";
    CHAR *pStrNodeNameEnd = "</MD>";
    CHAR *pStrResultHead = "<RSTR>";
    CHAR *pStrResultEnd = "</RSTR>";    
    WORD16 wXmlStrLen = MAX_SIZE_OMMMSG - sizeof(WORD16) -1;
    
    if ((NULL == ptrMsgBuf) || (NULL == ptLinkState) || (NULL == ptrXmlBuf))
    {
        return NULL_PTR;
    }    
    pPlanInfo = ptrMsgBuf;
    pXmlStr = ptrXmlBuf;
    
    /*拷贝MD节点头*/
    strncat(pXmlStr,pStrNodeNameHead,wXmlStrLen);
    /*拷贝RSTR节点头*/    
    strncat(pXmlStr,pStrResultHead,wXmlStrLen);    
    /*拷贝RSTR节点文本*/
    strncat(pXmlStr,pStrNodeText,wXmlStrLen);  
    
    /*拷贝RSTR节点尾*/
    strncat(pXmlStr,pStrResultEnd,wXmlStrLen);     
    /*拷贝MD节点尾*/
    strncat(pXmlStr,pStrNodeNameEnd,wXmlStrLen);        
    
    return RETURN_OK;
}

static XML_AGENT_STAT SetPGNodeForPlan(T_Cmd_Plan_Cfg *ptrMsgBuf,TYPE_XMLLINKSTATE *ptLinkState,CHAR *ptrXmlBuf)
{
    T_Cmd_Plan_Cfg *pPlanInfo = NULL;/*OAM通用结构体临时指针*/
    CHAR *pXmlStr = NULL;
    CHAR *pStrNodeNameHead = "<PG>";
    CHAR *pStrNodeText = "";
    CHAR *pStrNodeNameEnd = "</PG>";
    CHAR szPlanValue[4] = {0};
    CHAR *pStrPlanValue = szPlanValue;
    WORD16 wXmlStrLen = MAX_SIZE_OMMMSG - sizeof(WORD16) -1;
    
    if ((NULL == ptrMsgBuf) || (NULL == ptLinkState) || (NULL == ptrXmlBuf))
    {
        return NULL_PTR;
    }    
    pPlanInfo = ptrMsgBuf;
    pXmlStr = ptrXmlBuf;
    
    /*拷贝PG节点头*/
    strncat(pXmlStr,pStrNodeNameHead,wXmlStrLen);

    /*拷贝PG节点文本*/
    snprintf(pStrPlanValue, 4, "%d", pPlanInfo->ucPlanValue);
    pStrNodeText = pStrPlanValue;
    strncat(pXmlStr,pStrNodeText,wXmlStrLen);  
    
    /*拷贝PG节点尾*/
    strncat(pXmlStr,pStrNodeNameEnd,wXmlStrLen);        
    
    return RETURN_OK;
}

/**********************************************************************
* 函数名称：BYTE GetOpJid
* 功能描述：获取业务JOB ID
* 访问的表：无
* 修改的表：无
* 输入参数：LPXmlNode ptrNodeIn:节点指针                              
* 输出参数：JID *ptJid:JOD ID指针
* 返 回 值：RETURN_ERROR-失败 RETURN_OK-成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT GetOpJid(LPXmlNode ptrNodeIn,JID *ptJid)
{
    LPXmlNode ptrNodeTmp = NULL;	          /*节点指针*/
    T_CliAppDataEx *ptCliReg = NULL;/*非平台注册表指针*/
    WORD32 dwNetId = 0;                        /*网元标识*/ 
    WORD16 wLoop = 0;                                /*注册表下标*/
    BYTE ucRet = RETURN_ERROR;

    /*入参检查*/
    if ((ptrNodeIn == NULL) || (ptJid == NULL))
    {
        return NULL_PTR;
    }
    
    ptrNodeTmp = ptrNodeIn;        
    /*获取网元标识*/
    if (RETURN_OK != SetNIDNodeText(ptrNodeTmp,&dwNetId))
    {
        return SET_NODE_TEXT_ERR;
    }
    if (dwNetId == 0)
    {
        return RETURN_ERROR;
    }    

    ucRet = RETURN_ERROR;
    /*访问全局数据区*/
    for(wLoop = 0; wLoop < g_ptOamIntVar->tLompCliAppRegInfo.dwRegCount; wLoop++)
    {
        if(wLoop >= OAM_CLI_APP_SHM_NUM)
            break;
	 ptCliReg = &(g_ptOamIntVar->tLompCliAppRegInfo.tCliReg[wLoop]);
        if((dwNetId== ptCliReg->dwNetId)&&
	    (0 != ptCliReg->tAppJid.dwJno))
        {            
            ucRet = RETURN_OK;        
            memcpy(ptJid,&(ptCliReg->tAppJid),sizeof(JID));
            break;
        }
    }  
    
    /*如果数据区注册表中没有，检查共享内存*/
    if (RETURN_ERROR == ucRet)
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
                    ucRet = RETURN_ERROR;
                    break;
                }

                if ((dwNetId == ptOamCliReg->tCliReg[wLoop].dwNetId) &&
                     (0 != ptOamCliReg->tCliReg[wLoop].tAppJid.dwJno))
                {
                    ucRet = RETURN_OK; 
                    memcpy(ptJid, &(ptOamCliReg->tCliReg[wLoop].tAppJid), sizeof(JID));
                    break;
                }
            }

            Oam_UnLockRegExTable(ptOamCliReg);
        }
    }

    return ucRet;
}

/**********************************************************************
* 函数名称：BYTE GetOMMJid
* 功能描述：设置业务JOB ID
* 访问的表：无
* 修改的表：无
* 输入参数：LPXmlNode ptrNodeIn:节点指针                              
* 输出参数：无
* 返 回 值：RETURN_ERROR-失败 RETURN_OK-成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT GetOMMJid(LPXmlNode ptrNodeIn)
{
    LPXmlNode ptrNodeTmp = NULL;	          /*节点指针*/
    WORD32 dwLinkChannel = 0;                  /*链路号*/
    /*入参检查*/
    if(NULL == ptrNodeIn)
    {
        return NULL_PTR;
    }

    ptrNodeTmp = ptrNodeIn;
    if (RETURN_OK != SetTRMNodeText(ptrNodeTmp,&dwLinkChannel))
    {
        return SET_NODE_TEXT_ERR;
    }
    /*数组下标超出*/
    if (dwLinkChannel >= MAX_SIZE_OPJID)
    {
        return RETURN_ERROR;
    }
    /*填充自身JID*/
    if (XOS_SUCCESS != XOS_Sender(&gt_OpJid[dwLinkChannel]))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,"XOS_Sender ERROR!\n");
        return RETURN_ERROR;
    }
    return RETURN_OK;
}

/**********************************************************************
* 函数名称：BYTE SetDSecText
* 功能描述：从OAM通用消息体中设置D段文本
* 访问的表：无
* 修改的表：无
* 输入参数：MSG_COMM_OAM *ptrMsgBuf：OAM消息                              
* 输出参数：CHAR *ptrXmlBuf:XML字符串
* 返 回 值：RETURN_ERROR-失败 RETURN_OK-成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT SetDSecText(MSG_COMM_OAM *ptrMsgBuf,CHAR *ptrXmlBuf,WORD16 wXmlBufLen)
{
    MSG_COMM_OAM *ptrMsgBufTmp = NULL;/*OAM通用结构体临时指针*/
    CHAR *ptrXmlBufTmp = NULL;
    CHAR *pStrDSecHead = "<D>";
    CHAR *pStrDSecEnd = "</D>";
	
    if ((NULL == ptrMsgBuf) ||(NULL == ptrXmlBuf))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return NULL_PTR;
    }    
    ptrMsgBufTmp = ptrMsgBuf;
    ptrXmlBufTmp = ptrXmlBuf;
     /*拷贝D段头*/
    strncat(ptrXmlBufTmp,pStrDSecHead,wXmlBufLen);
    /*拷贝OT节点名称及文本*/
    if (RETURN_OK != SetOTNode(ptrXmlBufTmp,wXmlBufLen))
    {
        return SET_NODE_ERR;
    }
    /*拷贝OR节点名称及文本*/
    if (RETURN_OK != SetORNode(ptrMsgBufTmp,ptrXmlBufTmp,wXmlBufLen))
    {
        return SET_NODE_ERR; 
    }
    /*拷贝DC节点名称及文本*/
    if (RETURN_OK != SetDCNode(ptrMsgBufTmp,ptrXmlBufTmp,wXmlBufLen))
    {
        return SET_NODE_ERR; 
    }
    /*拷贝EOF节点名称及文本*/
    if (RETURN_OK != SetEOFNode(ptrMsgBufTmp,ptrXmlBufTmp,wXmlBufLen))
    {
        return SET_NODE_ERR; 
    }
    /*拷贝MD节点名称及文本*/
    if (RETURN_OK != SetMDNode(ptrXmlBufTmp,wXmlBufLen))
    {
        return SET_NODE_ERR; 
    }
    /*拷贝D段尾*/
    strncat(ptrXmlBufTmp,pStrDSecEnd,wXmlBufLen);
    
    return RETURN_OK;
}

XML_AGENT_STAT SetDSecTextForPlan(T_Cmd_Plan_Cfg *ptrMsgBuf,TYPE_XMLLINKSTATE *ptLinkState,CHAR *ptrXmlBuf)
{
    T_Cmd_Plan_Cfg *pPlanInfo = NULL;/*OAM通用结构体临时指针*/
    CHAR *pXmlStr = NULL;
    CHAR *pStrDSecHead = "<D>";
    CHAR *pStrDSecEnd = "</D>";
    WORD16 wXmlStrLen = 0;
    
    if ((NULL == ptrMsgBuf) || (NULL == ptLinkState) ||(NULL == ptrXmlBuf))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return NULL_PTR;
    }
    
    pPlanInfo = ptrMsgBuf;
    pXmlStr = ptrXmlBuf;
    wXmlStrLen = MAX_SIZE_OMMMSG - sizeof(WORD16) -1;

    /*拷贝D段头*/
    strncat(pXmlStr,pStrDSecHead,wXmlStrLen);

    /*拷贝OT节点名称及文本*/
    if (RETURN_OK != SetOTNode(pXmlStr,wXmlStrLen))
    {
        return SET_NODE_ERR;
    }

    /*拷贝OR节点名称及文本*/
    if (RETURN_OK != SetORNodeForPlan(pXmlStr))
    {
        return SET_NODE_ERR; 
    }

    /*拷贝DC节点名称及文本*/
    if (RETURN_OK != SetDCNodeForPlan(pXmlStr))
    {
        return SET_NODE_ERR; 
    }

    /*拷贝EOF节点名称及文本*/
    if (RETURN_OK != SetEOFNodeForPlan(pXmlStr))
    {
        return SET_NODE_ERR; 
    }

    /*拷贝PG节点名称及文本*/
    if (RETURN_OK != SetPGNodeForPlan(pPlanInfo, ptLinkState, pXmlStr))
    {
        return SET_NODE_ERR; 
    }

    /*拷贝MD节点名称及文本*/
    if (RETURN_OK != SetMDNodeForPlan(pPlanInfo,ptLinkState,pXmlStr))
    {
        return SET_NODE_ERR; 
    }

    /*拷贝D段尾*/
    strncat(pXmlStr,pStrDSecEnd,wXmlStrLen);
    
    return RETURN_OK;
}

/**********************************************************************
* 函数名称：XML_AGENT_STAT SetRSTRNodeText
* 功能描述：设置RSTR结点文本
* 访问的表：无
* 修改的表：无
* 输入参数：*ptRootNode:根结点
                               *ptrRSTRNodeText:操作结果
* 输出参数：无
* 返 回 值：XML_AGENT_STAT
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2011-7-29             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT SetRSTRNodeText(LPXmlNode ptRootNode,CHAR *ptrRSTRNodeText)
{
    XML_AGENT_STAT dwRet = RETURN_OK;
    XmlNode tCurNode;                                                /*当前节点结构变量*/
    XmlNode tDtNode;                                                /*目标节点结构变量*/
    if((NULL == ptRootNode) || (NULL == ptrRSTRNodeText))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                        "....................FUNC:%s LINE:%d NULL PTR! ....................",
                        __func__,
                        __LINE__);
        return NULL_PTR;
    }
    memset(&tCurNode,0,sizeof(tCurNode));
    memset(&tDtNode,0,sizeof(tDtNode));
    memcpy(&tCurNode,ptRootNode,sizeof(tCurNode));
    /*获取<D>段<MD>结点*/
    dwRet = GetChildByName(&tCurNode, "D", "MD", 0, &tDtNode);
    if (RETURN_OK != dwRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                            "....................FUNC:%s LINE:%d GetChildByName Error,%d! ....................",
                            __func__,
                            __LINE__,
                            dwRet);
        return GET_NODE_ERR;
    }
    /*获取<RSTR>结点*/
    memcpy(&tCurNode,&tDtNode,sizeof(tDtNode));
    dwRet = GetChlidByNode(&tCurNode, "RSTR", 0, &tDtNode);
    if (RETURN_OK != dwRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                            "....................FUNC:%s LINE:%d GetChlidByNode Error,%d! ....................",
                            __func__,
                            __LINE__,
                            dwRet);
        return GET_NODE_ERR;
    }
    /*设置<RSTR>结点文本*/
    dwRet = XML_SetChildText(&tDtNode, ptrRSTRNodeText);
    if (XML_OK != dwRet)
    {
        dwRet = SET_NODE_TEXT_ERR;
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                            "....................FUNC:%s LINE:%d XML_SetChildText Error,%d! ....................",
                            __func__,
                            __LINE__,
                            dwRet);
        return SET_NODE_TEXT_ERR;
    }
    return RETURN_OK;
}

/**********************************************************************
* 函数名称：BYTE XmlMgt2OamMgt
* 功能描述：XML消息转换为OAM消息
* 访问的表：无
* 修改的表：无
* 输入参数：LPXmlNode ptrNode:XML节点指针
                              TYPE_XMLLINKSTATE *ptLinkState:链路状态
* 输出参数：MSG_COMM_OAM *ptrMsgBuf:OAM消息体
* 返 回 值：RETURN_ERROR-失败 RETURN_OK-成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT XmlMgt2OamMgt(LPXmlNode ptrNode,TYPE_XMLLINKSTATE *ptLinkState,MSG_COMM_OAM *ptrMsgBuf)
{	
    LPXmlNode ptrNodeTmp = NULL;/*节点指针临时变量*/
    XmlNode tXmlDtNode;
    LPXmlNode ptrDtNode = NULL;/*目标节点指针*/    
    DWORD dwPacketSize = sizeof(MSG_COMM_OAM);
    OPR_DATA *pData;
    BYTE *pCurPos  = NULL;
    BYTE bLoop = 0;  
    DWORD dwCmdId = 0;/*命令ID*/
    BYTE bModeId = 0;/*模式ID*/
    INT iLinkChannel = -1;
	
    if ((ptrNode == NULL) || (NULL == ptLinkState) ||(ptrMsgBuf == NULL))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return NULL_PTR;
    }		
    ptrNodeTmp = ptrNode;	
    ptrDtNode = &tXmlDtNode;
    bModeId = ptLinkState->bModeID[ptLinkState->bModeStackTop-1];
    dwCmdId = ptrMsgBuf->CmdID;
    memset(ptrDtNode,0,sizeof(XmlNode));
	
    /*填充消息返回值*/
    ptrMsgBuf->ReturnCode  = RETURNCODE_INVALID;
	
    /*填充标识终端编号--将链路状态数组下标作链路号*/   
    iLinkChannel = Xml_GetIndexOfLinkState(ptLinkState->dwLinkChannel);
    if (-1 == iLinkChannel)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC: %s,Line:%d GetIndex_XmlLinkState Error!....................",__func__,__LINE__);
        return RETURN_ERROR;
    }
    ptrMsgBuf->LinkChannel  = 0X00000200|iLinkChannel;
    /*填充CMD节点ID属性*/	
    ptrMsgBuf->CmdID = ptLinkState->dwCmdId;	    
	
    /*消息中包含的参数个数*/
    if (RETURN_OK != GetCMDParaNodeCount(ptrNodeTmp,&ptrMsgBuf->Number))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................GetCMDParaNodeCount Error FUNC: %s,Line:%d!....................",__func__,__LINE__);
        return GET_NODE_CNT_ERR;
    }
	
    /*填充命令参数类型*/
    ptrMsgBuf->bIfNo = ptLinkState->tCurCMDInfo.bIsNoCmd; 
    /*填充输出模式*/
    ptrMsgBuf->OutputMode = OUTPUT_MODE_NORMAL;
  
    /*填充重启标志*/
    ptrMsgBuf->CmdRestartFlag = TRUE;
  
    /*填充命令参数*/
    if (ptrMsgBuf->Number > 0)
    { 
	 /*获取命令节点*/
        if (RETURN_OK != GetCMDNode(ptrNodeTmp,ptrDtNode))
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................GetCMDNode Error FUNC: %s,Line:%d!....................",__func__,__LINE__);
            return GET_NODE_ERR;
        }        
        dwPacketSize -= sizeof(OPR_DATA);
        pCurPos = (BYTE *) ptrMsgBuf->Data;
        for (bLoop = 0; bLoop < ptrMsgBuf->Number; bLoop++)
        {  
            pData = (OPR_DATA *) pCurPos;
            /*设置命令参数编号*/
            if (RETURN_OK != SetParaNodeAttr_ID(ptrDtNode,bLoop,&pData->ParaNo))
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................SetParaNodeAttr_ID Error FUNC: %s,Line:%d!....................",__func__,__LINE__);
                return SET_NODE_ATTR_ERR;
            }            
            /*设置参数内容并解析*/
            if (RETURN_OK != SetParaNodeAttr(&(ptLinkState->tCurCMDInfo),ptrDtNode,bLoop,pData))
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................SetParaNodeAttr Error FUNC: %s,Line:%d!....................",__func__,__LINE__);
                return SET_NODE_TEXT_ERR;
            } 
            /* set packet size */
            dwPacketSize += sizeof(OPR_DATA) - sizeof(BYTE) + pData->Len;
            pCurPos = pCurPos + sizeof(OPR_DATA) - sizeof(BYTE) + pData->Len;
        }
    }
  
    /*填充消息长度*/
    ptrMsgBuf->DataLen  = dwPacketSize;  

    return RETURN_OK;
}

/**********************************************************************
* 函数名称：BYTE OamMgt2XmlMgt
* 功能描述：OAM消息转换为XML消息
* 访问的表：无
* 修改的表：无
* 输入参数：MSG_COMM_OAM *ptrMsgBuf：OAM消息
* 输出参数：CHAR *ptrXmlBuf:XML字符串
* 返 回 值：RETURN_ERROR-失败 RETURN_OK-成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT OamMgt2XmlMgt(MSG_COMM_OAM *ptrMsgBuf,TYPE_XMLLINKSTATE *ptLinkState,WORD16 wOMMMsgBufLen,WORD16 *pwMsgLen,CHAR *ptrOMMMsgBuf)
{
    XML_AGENT_STAT dwRet = RETURN_OK;
    MSG_COMM_OAM *ptrMsgBufTmp = NULL;/*OAM通用结构体临时指针*/ 
    CHAR *ptrOMMMsgBufTmp = NULL;/*XML 字符串*/
    CHAR *pRSTRNodeText = NULL;
    TYPE_XMLLINKSTATE *ptLinkStateTmp = NULL;               /*连接状态*/    
    CHAR *pStrConfHead = "<CONF>";
    CHAR *pStrConfEnd = "</CONF>";    
    WORD16 wActLen = 0;
    XmlDocPtr ptrDoc = NULL;                                         /*xml文档指针*/
    XmlNode tRootNode;                                                /*当前节点结构变量*/
    static CHAR aucXmlBuf[MAX_SIZE_OMMACKMSG_FRTBUFFER] = {0};
    
    if((ptrMsgBuf == NULL) || (NULL == pwMsgLen) || (NULL == ptLinkState) ||(ptrOMMMsgBuf == NULL))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return NULL_PTR;
    }    
    
    ptrMsgBufTmp = ptrMsgBuf;
    ptrOMMMsgBufTmp = ptrOMMMsgBuf;    
    ptLinkStateTmp = ptLinkState;        
    memset(&tRootNode,0,sizeof(tRootNode));    
    memset(aucXmlBuf,0,sizeof(aucXmlBuf));
    
    /*拷贝XML声明部分*/
    strncat(aucXmlBuf,ptLinkStateTmp->xmlDeclText,sizeof(aucXmlBuf) - 1);    
    /*拷贝XML根节点头*/
    strncat(aucXmlBuf,pStrConfHead,sizeof(aucXmlBuf) - 1);    
    /*拷贝T段文本*/
    strncat(aucXmlBuf,ptLinkStateTmp->xmlTSecText,sizeof(aucXmlBuf) - 1);    
    /*拷贝D段文本*/
    if (RETURN_OK != SetDSecText(ptrMsgBufTmp,aucXmlBuf,sizeof(aucXmlBuf) - 1))
    {
        return SET_NODE_TEXT_ERR;
    }    
    /*拷贝XML根节点尾*/
    strncat(aucXmlBuf,pStrConfEnd,sizeof(aucXmlBuf) - 1);
    /*解析回显串*/
    switch(ptrMsgBufTmp->ReturnCode)
    {
        case SUCC_AND_NOPARA:
        {
            pRSTRNodeText = "Success";
            break;
        }
        case ERR_AND_HAVEPARA:
        case SUCC_AND_HAVEPARA:
        case SUCC_CMD_NOT_FINISHED:
        case SUCC_WAIT_AND_HAVEPARA:
        {
            ptLinkStateTmp->tDispGlobalPara.pDisplayBuffer = g_ptOamIntVar->sOutputResultBuf;
            if (DISPLAY_SUCCESS != XmlProcess_Return_Data(ptLinkStateTmp,ptrMsgBufTmp))
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR,
                                    "....................FUNC:%s LINE:%d XmlProcess_Return_Data Error! ....................",
                                    __func__,
                                    __LINE__);
                return RETURN_ERROR;
            }
            pRSTRNodeText = ptLinkStateTmp->tDispGlobalPara.pDisplayBuffer;
            break;
        }
        default:
        {
            XOS_SysLog(OAM_CFG_LOG_DEBUG,
                            "....................FUNC:%s LINE:%d ReturnCode Error!%d ....................",
                            __func__,
                            __LINE__,
                            ptrMsgBufTmp->ReturnCode);
            break;
        }
    }
    /*解析XML文档*/
    dwRet = XML_ParaseDoc(aucXmlBuf,strlen(aucXmlBuf),XML_STAT_PARSE_NOBLANKS,&ptrDoc,&tRootNode);
    if (RETURN_OK != dwRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                            "....................FUNC:%s LINE:%d XML_ParaseDoc Error,%d! ....................",
                            __func__,
                            __LINE__,
                            dwRet);
        return LOAD_XML_ERR;
    }
    /*设置RSTR结点文本*/
    dwRet = SetRSTRNodeText(&tRootNode, pRSTRNodeText);
    if (RETURN_OK != dwRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                            "....................FUNC:%s LINE:%d SetRSTRNodeText Error,%d! ....................",
                            __func__,
                            __LINE__,
                            dwRet);
        XML_FreeXml(ptrDoc);
        return SET_NODE_TEXT_ERR;
    }
    /*拷贝组件号*/
    memcpy(ptrOMMMsgBufTmp,ptLinkStateTmp->ucComponentId,sizeof(ptLinkStateTmp->ucComponentId));
    ptrOMMMsgBufTmp = ptrOMMMsgBufTmp + sizeof(ptLinkStateTmp->ucComponentId);
    /*获取文本*/
    dwRet = XML_GetText(ptrDoc,ptrOMMMsgBufTmp,wOMMMsgBufLen - sizeof(ptLinkStateTmp->ucComponentId) - 1,XML_STAT_PARSE_NOBLANKS,&wActLen);
    if (XML_OK != dwRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                            "....................FUNC:%s LINE:%d XML_GetText Error!%lu....................",
                            __func__,
                            __LINE__,
                            dwRet);
        XML_FreeXml(ptrDoc);
        return GET_DOCTEXTERR;
    }
    *pwMsgLen = wActLen + sizeof(ptLinkStateTmp->ucComponentId);
    XML_FreeXml(ptrDoc);
    return RETURN_OK;
}

XML_AGENT_STAT PlanMgt2XmlMgt(T_Cmd_Plan_Cfg *ptrMsgBuf,TYPE_XMLLINKSTATE *ptLinkState,WORD16 *pwMsgLen,CHAR *ptrOMMMsgBuf)
{
    T_Cmd_Plan_Cfg *pPlanInfo = NULL;/*进度条信息结构体临时指针*/
    CHAR *pXmlStr = NULL;/*XML 字符串*/
    CHAR *pStrConfHead = "<CONF>";
    CHAR *pStrConfEnd = "</CONF>";
    WORD16 wXmlStrLen = 0;
    
    if((ptrMsgBuf == NULL) || (NULL == pwMsgLen) || (NULL == ptLinkState) ||(ptrOMMMsgBuf == NULL))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return NULL_PTR;
    }
    pPlanInfo = ptrMsgBuf;
    pXmlStr = ptrOMMMsgBuf;
    
    /*拷贝组建号*/
    memcpy(pXmlStr,&ptLinkState->ucComponentId[0],sizeof(ptLinkState->ucComponentId));    
    pXmlStr = pXmlStr + sizeof(ptLinkState->ucComponentId);
    wXmlStrLen = MAX_SIZE_OMMMSG - sizeof(WORD16) -1;
    
    /*拷贝XML声明部分*/
    strncat(pXmlStr,ptLinkState->xmlDeclText,wXmlStrLen);
    
    /*拷贝XML根节点头*/
    strncat(pXmlStr,pStrConfHead,wXmlStrLen);
    
    /*拷贝T段文本*/
    strncat(pXmlStr,ptLinkState->xmlTSecText,wXmlStrLen);
    
    /*拷贝D段文本*/
    if (RETURN_OK != SetDSecTextForPlan(pPlanInfo,ptLinkState,pXmlStr))
    {
        return SET_NODE_TEXT_ERR;
    }
    
    /*拷贝XML根节点尾*/
    strncat(pXmlStr,pStrConfEnd,wXmlStrLen);      
    *pwMsgLen = sizeof(ptLinkState->ucComponentId) + strlen(pXmlStr);
    
    return RETURN_OK;
}

/**********************************************************************
* 函数名称：XML_AGENT_STAT SendOMMReqToOP
* 功能描述：发送网管请求到业务
* 访问的表：无
* 修改的表：无
* 输入参数：CHAR *ptrOMMReq:OMM配置请求消息
                              LPXmlNode ptrNode:根节点
* 输出参数：无
* 返 回 值：   RETURN_ERROR-失败 RETURN_OK-成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static VOID SendOMMReqToOP(CHAR *ptrOMMReq,WORD16 wMsgLen,LPXmlNode ptrNodeNode)
{
    CHAR *ptrOMMReqTmp = NULL;         /*OMM配置请求消息指针*/
    LPXmlNode ptrNodeNodeTmp = NULL;/*XML文档根指针*/
    JID tDtJid;                                         /*接收JOB ID*/

    /*参数检查*/
    if((NULL == ptrOMMReq) || (0 == wMsgLen) || (NULL == ptrNodeNode))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return;
    }

    /*初始化变量*/
    ptrOMMReqTmp = ptrOMMReq;
    ptrNodeNodeTmp = ptrNodeNode;
    memset(&tDtJid,0,sizeof(JID));
    
    if (RETURN_OK != GetOpJid(ptrNodeNodeTmp,&tDtJid))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC: %s,Line:%d GetOpJid Error!....................",__func__,__LINE__);
	 return;
    }
    if(RETURN_OK != GetOMMJid(ptrNodeNodeTmp))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC: %s,Line:%d GetOMMJid Error!....................",__func__,__LINE__);
	 return;
    }
    /*向业务发送消息*/
    XOS_SendAsynMsg(EV_XML_OMMCFG_REQ, (BYTE *)ptrOMMReqTmp,wMsgLen, 0, 0,&tDtJid);
    XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d SendOMMReqToOP Successfully! ....................",__func__,__LINE__);
    return;
}

/**********************************************************************
* 函数名称：VOID SendErrStringToOmm
* 功能描述：向网管发送错误XML字符串
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_XMLLINKSTATE *ptLinkState:XML链路信息
                             CHAR *sOutputResult:返回结果
* 输出参数：无
* 返 回 值：   无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
VOID SendErrStringToOmm(TYPE_XMLLINKSTATE *ptLinkState,CHAR *ptrOutputResult)
{
    BOOLEAN bOR = FALSE;
    BOOLEAN bEof = TRUE;
    /*参数检查*/
    if ((NULL == ptLinkState) || (NULL == ptrOutputResult))
    {
        XOS_SysLog(OAM_CFG_LOG_NOTICE,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return;
    }
    SendStringToOmm(ptLinkState, ptrOutputResult, bOR,bEof);
    return;
}

/**********************************************************************
* 函数名称：XML_AGENT_STAT CreateTSecNode
* 功能描述：创建T段信息函数
* 访问的表：无
* 修改的表：无
* 输入参数：LPXmlNode ptOriRtNode:原根节点指针                             
* 输出参数：LPXmlNode ptDstRtNode:目标根节点指针
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT CreateTSecNode(const LPXmlNode ptOriRtNode,const LPXmlNode ptDstRtNode)
{
    INT iRet = 0;
    WORD16 wActualLen = 0;
    WORD32 dwNodeCount = 0;
    WORD32 dwNodeLoop = 0;
    WORD32 dwAttrCount = 0;
    WORD32 dwAttrLoop = 0;
    CHAR aucNodeText[MAX_NODETEXT_BUF_SIZE] = {0};/*节点文本*/
    XmlNode tOriCurNode;/*原文档当前节点*/
    XmlNode tOriSubNode;/*原文档子节点*/
    XmlAttr tOriAttrNode;/*原文档属性结点*/
    XmlNode tDstCurNode;/*目标文档当前节点*/
    XmlNode tDstSubNode;/*目标文档子节点*/
    XmlAttr tDstAttrNode;/*目标文档属性结点*/
    /*参数检查*/
    if ((NULL == ptOriRtNode)  || (NULL == ptDstRtNode))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................\n",__func__,__LINE__);
        return NULL_PTR;
    }
    memset(&tOriCurNode,0,sizeof(tOriCurNode));    
    memset(&tDstCurNode,0,sizeof(tDstCurNode));
    iRet = XML_GetChild(ptOriRtNode, 0, &tOriCurNode);
    /*获取T结点失败*/
    if (XML_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_GetChild Error!%d ....................\n",__func__,__LINE__,iRet);
        return GET_NODE_ERR;
    }
    /*T 节点不存在*/
    if (0 != strcmp((CHAR *)(tOriCurNode.name),(CHAR *)NAME_T_NODE))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d Node is not exsit! ....................\n",__func__,__LINE__);
        return RETURN_ERROR;
    }
    iRet = XML_InsertChild(ptDstRtNode, 0, (CHAR *)(tOriCurNode.name), &tDstCurNode);
    /*生成T结点失败*/
    if (XML_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_InsertChild Error!%d ....................\n",__func__,__LINE__,iRet);
        return SET_NODE_ERR;
    }
    /*获取T结点下子节点个数*/
    dwNodeCount = XML_GetChildCount(&tOriCurNode);
    for (dwNodeLoop = 0;dwNodeLoop < dwNodeCount;dwNodeLoop++)
    {
        memset(aucNodeText,0,sizeof(aucNodeText));
        memset(&tOriSubNode,0,sizeof(tOriSubNode));
        memset(&tDstSubNode,0,sizeof(tDstSubNode));
        iRet = XML_GetChild(&tOriCurNode, dwNodeLoop, &tOriSubNode);
        /*获取结点失败*/
        if (XML_OK != iRet)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_GetChild Error!%d ....................\n",__func__,__LINE__,iRet);
            return GET_NODE_ERR;
        }
        iRet = XML_GetChildText(&tOriSubNode, aucNodeText, sizeof(aucNodeText), &wActualLen);
        /*获取子节点文本失败*/
        if (XML_OK != iRet)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_GetChildText Error!%d ....................\n",__func__,__LINE__,iRet);
            return GET_NODE_TEXT_ERR;
        }
        iRet = XML_InsertChild(&tDstCurNode, dwNodeLoop, (CHAR *)(tOriSubNode.name), &tDstSubNode);
        /*生成结点失败*/
        if (XML_OK != iRet)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_InsertChild Error!%d ....................\n",__func__,__LINE__,iRet);
            return SET_NODE_ERR;
        }
        iRet = XML_SetChildText(&tDstSubNode, aucNodeText);
        /*设置结点文本失败*/
        if (XML_OK != iRet)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_SetChildText Error!%d ....................\n",__func__,__LINE__,iRet);
            return SET_NODE_TEXT_ERR;
        }
        /*获取子结点属性个数*/
        dwAttrCount = XML_GetAttributeCount(&tOriSubNode);
        for(dwAttrLoop = 0;dwAttrLoop < dwAttrCount;dwAttrLoop++)
        {
            memset(&tOriAttrNode,0,sizeof(tOriAttrNode));
            memset(&tDstAttrNode,0,sizeof(tDstAttrNode));
            iRet = XML_GetAttribute(&tOriSubNode, dwAttrLoop, &tOriAttrNode);
            /*获取结点属性失败*/
            if (XML_OK != iRet)
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR,
                                   "....................FUNC:%s LINE:%d XML_GetAttribute Error!%d ....................\n",__func__,__LINE__,iRet);
                return GET_NODE_ATTR_ERR;
            }
            iRet = XML_SetAttribute(&tDstSubNode, (CHAR *)(tOriAttrNode.name), (CHAR *)(tOriAttrNode.value));
            /*设置结点属性失败*/
            if (XML_OK != iRet)
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR,
                                   "....................FUNC:%s LINE:%d XML_SetAttribute Error!%d ....................\n",__func__,__LINE__,iRet);
                return SET_NODE_ATTR_ERR;
            }
        }
    }
    return RETURN_OK;
}

/**********************************************************************
* 函数名称：XML_AGENT_STAT CreateRSTRNode
* 功能描述：创建<RSTR>结点函数
* 访问的表：无
* 修改的表：无
* 输入参数：const CHAR *pucRSTRText:<RSTR>结点文本
                             const XmlNodePtr ptCurNode:当前节点指针
* 输出参数：无
* 返 回 值：   无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT CreateRSTRNode(const CHAR *pucRSTRText,const LPXmlNode ptCurNode)
{
    WORD16 wUTF8BufLen = 0;
    WORD32 iRet = 0;
    XmlNode tDstCurNode;/*目标文档当前结点*/
    CHAR *pucUTF8Buf = NULL;
	
    /*参数检查*/
    if ((NULL == ptCurNode)  || 
        (NULL == pucRSTRText))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................\n",__func__,__LINE__);
        return NULL_PTR;
    }
    
    memset(&tDstCurNode,0,sizeof(tDstCurNode));
    iRet = XML_InsertChild((LPXmlNode)ptCurNode, 0, (CHAR *)NAME_RSTR_NODE, &tDstCurNode);
    /*插入<RSTR>失败*/
    if (XML_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                            "....................FUNC:%s LINE:%d XML_InsertChild Error!%d ....................\n",__func__,__LINE__,iRet);
        return SET_NODE_ERR;
    }
    wUTF8BufLen = strlen(pucRSTRText);
    wUTF8BufLen *= 2;
    pucUTF8Buf = (CHAR *)XOS_GetUB(wUTF8BufLen);
    /*申请动态内存失败*/
    if (NULL == pucUTF8Buf)
        {
            XOS_SysLog(OAM_CFG_LOG_NOTICE,
                       "....................FUNC:%s LINE:%d XOS_GetUB Error! ....................",__func__,__LINE__);
        return RETURN_ERROR;
        }
    memset(pucUTF8Buf,0,wUTF8BufLen);
            /*编码转换*/
    if (!Gb2312ToUtf8(pucRSTRText, wUTF8BufLen,pucUTF8Buf))
            {
                XOS_SysLog(OAM_CFG_LOG_NOTICE,
                       "....................FUNC:%s LINE:%d Gb2312ToUtf8 Error! ....................",__func__,__LINE__);            
        OAM_RETUB(pucUTF8Buf);
        return RETURN_ERROR;
    }
    iRet = XML_SetChildText(&tDstCurNode, pucUTF8Buf);
    /*设置结点文本失败*/
    if (XML_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_SetChildText Error!%d ....................\n",__func__,__LINE__,iRet);
        OAM_RETUB(pucUTF8Buf);
        return SET_NODE_TEXT_ERR;
    }
    OAM_RETUB(pucUTF8Buf);
    return RETURN_OK;
}

/**********************************************************************
* 函数名称：XML_AGENT_STAT CreateDSecNode
* 功能描述：创建D段信息函数
* 访问的表：无
* 修改的表：无
* 输入参数：const CHAR *pucORText:OR结点文本
                             const CHAR *pucEOFText:EOF结点文本
                             const CHAR *pucRSTRText:RSTR结点文本
                             const LPXmlNode ptOriRtNode:原文档根节点指针
* 输出参数：LPXmlNode ptDstRtNode:目标文档根节点指针
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT CreateDSecNode(const CHAR *pucORText,
	                                                             const CHAR *pucEOFText,
	                                                             const CHAR *pucRSTRText,
	                                                             const LPXmlNode ptOriRtNode,
	                                                             const LPXmlNode ptDstRtNode)
{
    INT iRet = 0;
    WORD32 dwNodeCount = 0;
    WORD32 dwNodeLoop = 0;
    XmlNode tOriCurNode;/*原文档当前节点*/
    XmlNode tOriSubNode;/*原文档子节点*/
    XmlNode tDstCurNode;/*目标文档当前节点*/
    XmlNode tDstSubNode;/*目标文档子节点*/

    /*参数检查*/
    if ((NULL == pucORText)  || 
        (NULL == pucEOFText)  || 
	 (NULL == pucRSTRText)  || 
        (NULL == ptOriRtNode)  || 
        (NULL == ptDstRtNode))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................\n",__func__,__LINE__);
        return NULL_PTR;
    }
    memset(&tOriCurNode,0,sizeof(tOriCurNode));
    memset(&tDstCurNode,0,sizeof(tDstCurNode));
    iRet = XML_GetChild(ptOriRtNode, 1, &tOriCurNode);
    /*获取D结点失败*/
    if (XML_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_GetChild Error!%d ....................\n",__func__,__LINE__,iRet);
        return GET_NODE_ERR;
    }
    /*D 节点不存在*/
    if (0 != strcmp((CHAR *)(tOriCurNode.name),NAME_D_NODE))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d Node is not exsit! ....................\n",__func__,__LINE__);
        return GET_NODE_TEXT_ERR;
    }
    iRet = XML_InsertChild(ptDstRtNode, 1, (CHAR *)(tOriCurNode.name), &tDstCurNode);
    /*生成D结点失败*/
    if (XML_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_InsertChild Error!%d ....................\n",__func__,__LINE__,iRet);
        return SET_NODE_ERR;
    }
    /*获取D结点下子节点个数*/
    dwNodeCount = XML_GetChildCount(&tOriCurNode);
    for (dwNodeLoop = 0;dwNodeLoop < dwNodeCount;dwNodeLoop++)
    {
        memset(&tOriSubNode,0,sizeof(tOriSubNode));
        memset(&tDstSubNode,0,sizeof(tDstSubNode));
        iRet = XML_GetChild(&tOriCurNode, dwNodeLoop, &tOriSubNode);
        /*获取结点失败*/
        if (XML_OK != iRet)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_GetChild Error!%d ....................\n",__func__,__LINE__,iRet);
            return GET_NODE_ERR;
        }
        iRet = XML_InsertChild(&tDstCurNode, dwNodeLoop, (CHAR *)(tOriSubNode.name), &tDstSubNode);
        /*生成结点失败*/
        if (XML_OK != iRet)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_InsertChild Error!%d ....................\n",__func__,__LINE__,iRet);
            return SET_NODE_ERR;
        }
        /*设置结点文本*/
        if (0 == strcmp((CHAR *)(tOriSubNode.name),NAME_OT_NODE))/*<OT>*/
        {
            iRet = XML_SetChildText(&tDstSubNode, (CHAR *)TEXT_OT_NODE);
        }
        else if (0 == strcmp((CHAR *)(tOriSubNode.name),NAME_OR_NODE))/*<OR>*/
        {
            iRet = XML_SetChildText(&tDstSubNode, (CHAR *)pucORText);
        }
        else if (0 == strcmp((CHAR *)(tOriSubNode.name),NAME_DC_NODE))/*<DC>*/
        {
            iRet = XML_SetChildText(&tDstSubNode, (CHAR *)TEXT_DC_NODE);
        }
        else if (0 == strcmp((CHAR *)(tOriSubNode.name),NAME_EOF_NODE))/*<EOF>*/
        {
            iRet = XML_SetChildText(&tDstSubNode, (CHAR *)pucEOFText);
        }
        else if (0 == strcmp((CHAR *)(tOriSubNode.name),NAME_MD_NODE))/*<MD>*/
        {
            iRet = CreateRSTRNode(pucRSTRText,(const LPXmlNode)&tDstSubNode);
            }
            else
            {
            ;
        }
        /*设置结点文本失败*/
        if (RETURN_OK != iRet)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_SetChildText %s Error!%d ....................\n",__func__,__LINE__,tOriSubNode.name,iRet);
            return SET_NODE_TEXT_ERR;
        }
    }
    memset(&tDstSubNode,0,sizeof(tDstSubNode));
    iRet = XML_InsertChild(&tDstCurNode, 4, "PG", &tDstSubNode);
    /*生成结点失败*/
    if (XML_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                           "....................FUNC:%s LINE:%d XML_InsertChild Error!%d ....................\n",__func__,__LINE__,iRet);
        return SET_NODE_ERR;
    }
    iRet = XML_SetChildText(&tDstSubNode, "0");
    /*设置结点文本失败*/
    if (XML_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                           "....................FUNC:%s LINE:%d XML_SetChildText Error!%d ....................\n",__func__,__LINE__,iRet);
        return SET_NODE_ERR;
    }
    return RETURN_OK;
}

/**********************************************************************
* 函数名称：XML_AGENT_STAT CreateDstXmlDoc
* 功能描述：创建目标文档
* 访问的表：无
* 修改的表：无
* 输入参数：const CHAR *pucORText:OR结点文本
                             const CHAR *pucEOFText:EOF结点文本
                             const CHAR *pucRSTRText:RSTR结点文本
                             const LPXmlNode pOriRtNode:原文档根节点
* 输出参数：XmlDocPtr *pptDstXmlDoc:目标文档节点
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT CreateDstXmlDoc(const CHAR *pucORText,
                                                                    const CHAR *pucEOFText,
                                                                    const CHAR *pucRSTRText,
                                                                    const LPXmlNode pOriRtNode,
                                                                    XmlDocPtr *pptDstXmlDoc)
{
    WORD32 dwRet = 0;
    XmlNode tDstRtNode;
    XmlDocPtr ptDstXmlDoc = NULL;/*目标文档指针*/

    /*参数检查*/
    if ((NULL == pucORText)  || 
        (NULL == pucEOFText)  || 
	 (NULL == pucRSTRText)  || 
        (NULL == pOriRtNode)  || 
        (NULL == pptDstXmlDoc))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................\n",__func__,__LINE__);
        return NULL_PTR;
    }
    
    memset(&tDstRtNode,0,sizeof(tDstRtNode));
    dwRet = XML_NewDoc((CHAR *)(pOriRtNode->name), &ptDstXmlDoc);
    /*生成目标文档失败*/
    if (XML_OK != dwRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_NewDoc Error!%d ....................\n",__func__,__LINE__,dwRet);
        return NEW_XML_ERR;
    }
    dwRet = XML_GetRootNode(ptDstXmlDoc, &tDstRtNode);
    /*获取目标根节点失败*/
    if (XML_OK != dwRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_GetRootNode Error!%d ....................\n",__func__,__LINE__,dwRet);
        XML_FreeXml(ptDstXmlDoc);
        return GET_NODE_ERR;
            }
    dwRet = CreateTSecNode(pOriRtNode,&tDstRtNode);
    /*创建T段信息失败*/
    if (RETURN_OK != dwRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d CreateTSecNode Error!%d ....................\n",__func__,__LINE__,dwRet);
        XML_FreeXml(ptDstXmlDoc);
        return SET_NODE_ERR;
        }
    dwRet = CreateDSecNode(pucORText,pucEOFText,pucRSTRText,pOriRtNode,&tDstRtNode);
    /*创建D段信息失败*/
    if (RETURN_OK != dwRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d CreateDSecNode Error!%d ....................\n",__func__,__LINE__,dwRet);
        XML_FreeXml(ptDstXmlDoc);
        return SET_NODE_ERR;
    }
    *pptDstXmlDoc = ptDstXmlDoc;
    return RETURN_OK;
}

/**********************************************************************
* 函数名称：XML_AGENT_STAT SendAckMsg2OMM
* 功能描述：发送应答消息到网管
* 访问的表：无
* 修改的表：无
* 输入参数：const CHAR *pucMsg:消息体
            WORD32 dwMsgId:消息号
            JID *ptOmmJid:网管端JID
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT SendAckMsg2OMM(const CHAR *pucMsg,
                                    WORD32 dwMsgId,
                                    JID *ptOmmJid)
{
    WORD16 wMsgLen = 0;
    WORD32 dwRet = 0;
    TYPE_XMLMODEL *ptXmlModel = NULL;
    
    /*参数检查*/
    if ((NULL == pucMsg) ||
        (NULL == ptOmmJid))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................\n",__func__,__LINE__);
        return NULL_PTR;
    }
    ptXmlModel = (TYPE_XMLMODEL *)pucMsg;
    wMsgLen = sizeof(ptXmlModel->wComponentId) + strlen(ptXmlModel->ucXML) + 1;
    /*打印XML字符流*/
    if (bDebugPrint)
    {
        PrintMsgXml(ptXmlModel, "MSG_XMLAGNET_TO_OMM");
    }
    dwRet = XOS_SendAsynMsg(dwMsgId,
                            (BYTE *)pucMsg,
                            wMsgLen,
                            0,
                            0,
                            ptOmmJid);
    /*向网管返回命令终止应答失败*/
    if (XOS_SUCCESS != dwRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                               "....................FUNC:%s LINE:%d XOS_SendAsynMsg Error,Error code %d! ....................",__func__,__LINE__,dwRet);
        return RETURN_ERROR;
    }
    return RETURN_OK;
}

/**********************************************************************
* 函数名称：XML_AGENT_STAT ConstructAckMsg2OMM
* 功能描述：构造应答消息到网管
* 访问的表：无
* 修改的表：无
* 输入参数：const CHAR *pucORText:OR结点文本
                             const CHAR *pucEOFText:EOF结点文本
                             const CHAR *pucRSTRText:RSTR结点文本
                             const CHAR *pucComponentId:组件号
                             const LPXmlNode pOriRtNode:源文档根节点
                             WORD32 dwMsgId:消息号
                             JID *ptOmmJid:网管端JID
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT ConstructAckMsg2OMM(const CHAR *pucORText,
                                                                                const CHAR *pucEOFText,
                                                                                const CHAR *pucRSTRText,
                                                                                const CHAR *pucComponentId,
                                                                                const LPXmlNode pOriRtNode,
                                                                                WORD32 dwMsgId,
                                                                                JID *ptOmmJid)
{
    WORD32 dwRet = RETURN_OK;
    XmlDocPtr ptDstDoc = NULL;
    CHAR *pucDstDocTextBuf = NULL;
    /*参数检查*/
    if ((NULL == pucORText)  || 
        (NULL == pucEOFText)  || 
	 (NULL == pucRSTRText)  || 
        (NULL == pucComponentId)  || 
        (NULL == pOriRtNode) ||
        (NULL == ptOmmJid))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................\n",__func__,__LINE__);
        return NULL_PTR;
    }
    dwRet = CreateDstXmlDoc(pucORText, pucEOFText, pucRSTRText, pOriRtNode, &ptDstDoc);
    /*创建文档失败*/
    if (RETURN_OK != dwRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d CreateDstXmlDoc %d Error! ....................\n",__func__,__LINE__,dwRet);
        return NEW_XML_ERR;
    }
    pucDstDocTextBuf = XML_GetTextForUTF(ptDstDoc, pucComponentId);
    /*获取目标文档文本失败*/
    if (NULL == pucDstDocTextBuf)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                            "....................FUNC:%s LINE:%d XML_GetTextForUTF Error ....................\n",__func__,__LINE__);
        /*释放资源*/
        XML_FreeXml(ptDstDoc);
        return RETURN_ERROR;
    }
    dwRet = SendAckMsg2OMM(pucDstDocTextBuf, dwMsgId, ptOmmJid);
    /*向网管发送应答消息失败*/
    if (RETURN_OK != dwRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d SendAckMsg2OMM %d Error! ....................\n",__func__,__LINE__,dwRet);
        /*释放资源*/
        XML_FreeXml(ptDstDoc);
        OAM_RETUB(pucDstDocTextBuf);
        return RETURN_ERROR;
    }
    /*释放资源*/
    XML_FreeXml(ptDstDoc);
    OAM_RETUB(pucDstDocTextBuf);
    return dwRet;
}

/**********************************************************************
* 函数名称：CHAR *GetRSTRText
* 功能描述：获取操作结果字符串
* 访问的表：无
* 修改的表：无
* 输入参数：WORD32 dwCode:操作码
* 输出参数：无
* 返 回 值：CHAR *
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static CHAR *GetRSTRText(WORD32 dwCode)
{    
    CHAR *pucCeaseCmdRslt = NULL;
    
    /*OAM 全局变量为空指针*/
    if (NULL == g_ptOamIntVar)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                   "....................FUNC:%s LINE:%d g_ptOamIntVar is null ptr! ....................",__func__,__LINE__);
        return NULL;
    }
    /*获取操作结果字符串*/
    switch(dwCode)
    {      
        case RETURN_OK:/*请求发送成功*/
        {
            pucCeaseCmdRslt = ReadMapValueOfOamDat(MAP_TYPE_XMLADAPTER,MAP_KEY_SUCCESS_SENDMSG);
            break;
        }
        case NO_EXECUTE_CMD_ON_LINKCHANNEL:/*当前链路没有命令执行*/
        {
            pucCeaseCmdRslt = ReadMapValueOfOamDat(MAP_TYPE_XMLADAPTER,MAP_KEY_NO_CMDEXECUTING);
            break;
        }
        default :
        {
            pucCeaseCmdRslt = ReadMapValueOfOamDat(MAP_TYPE_XMLADAPTER,MAP_KEY_ERR_EXECUTEOMMCMD);
            break;            
        }
    }
    return pucCeaseCmdRslt;
}

/**********************************************************************
* 函数名称：VOID ProcErrPlatCfgMsg
* 功能描述：处理错误平台配置消息
* 访问的表：无
* 修改的表：无
* 输入参数：const CHAR *ptrComponentId:网管组件号
                             const LPXmlNode ptOriRtNode:源文档根节点
                             const CHAR *ptrResult:返回结果
                             JID *ptDtJid:网管JID
* 输出参数：无
* 返 回 值：   无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static VOID ProcErrPlatCfgMsg(const CHAR *ptrComponentId,const LPXmlNode ptOriRtNode,const CHAR *ptrOutputResult,JID *ptDtJid)
{
    WORD32 dwRet = 0;
    CHAR *pucORText = "";
    CHAR *pucEOFText = "";
    

    /*参数检查*/
    if ((NULL == ptrComponentId) ||
        (NULL == ptOriRtNode) ||
        (NULL == ptrOutputResult) ||
        (NULL == ptDtJid))
    {
         XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
         return;
    }
    /*<OR>文本*/
    pucORText = TEXT_OR_NODE_NOK;
    /*<EOF>文本*/
    pucEOFText = TEXT_EOF_NODE_Y;

    dwRet = ConstructAckMsg2OMM(pucORText, 
                                                    pucEOFText, 
                                                    ptrOutputResult,
                                                    (const CHAR *)ptrComponentId,
                                                    (const LPXmlNode)ptOriRtNode, 
                                                    EV_XML_OMMCFG_ACK, 
                                                    ptDtJid);
    /*构造应答消息失败*/
    if (RETURN_OK != dwRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d ConstructAckMsg2OMM %d Error!....................",__func__,__LINE__,dwRet);
        return;
    }
    return;
}

/**********************************************************************
* 函数名称：VOID SendStringToOmm
* 功能描述：向网管发送XML字符串
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_XMLLINKSTATE *ptLinkState:XML链路信息
                             CHAR *sOutputResult:返回结果
                             BOOLEAN bOR:操作结果
                             BOOLEAN bEof:是否结束包
* 输出参数：无
* 返 回 值：   无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
VOID SendStringToOmm(TYPE_XMLLINKSTATE *ptLinkState,CHAR *ptrOutputResult,BOOLEAN bOR,BOOLEAN bEof)
{
    CHAR *ptrOmmMsgBuf = pRecvOMMMsgBuf;
    CHAR *ptrXmlMsgBuf = NULL;
    CHAR ptrConfHead[] = "<CONF>";
    CHAR ptrConfEnd[] = "</CONF>"; 
    CHAR ptrDSecHead[] = "<D>";
    CHAR ptrDSecEnd[] = "</D>";
    CHAR ptrOTNodeText[] = "<OT>RP</OT>";
    CHAR ptrORHeadNodeText[] = "<OR>";
    CHAR ptrOREndNodeText[]= "</OR>";    
    CHAR ptrDCNodeText[] = "<DC>0</DC>";
    CHAR ptrEOFHeadNodeText[] = "<EOF>";
    CHAR ptrEOFEndNodeText[] = "</EOF>";
    CHAR ptrMDNodeHeadText[] = "<MD><RSTR>";
    CHAR ptrMDNodeEndText[] = "</RSTR></MD>";
    CHAR *ptrUTF8Buf = NULL;
    INT iBufLen = 0;
    WORD16 wMsgLen = 0;                    /*错误消息长度*/   

    /*参数检查*/
    if ((NULL == ptLinkState) || (NULL == ptrOutputResult))
    {
        XOS_SysLog(OAM_CFG_LOG_NOTICE,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return;
    }    
    memset(ptrOmmMsgBuf,0,MAX_SIZE_OMMMSG);
    /*拷贝组建号*/
    memcpy(ptrOmmMsgBuf,(CHAR *)&ptLinkState->ucComponentId[0],sizeof(ptLinkState->ucComponentId));
    wMsgLen += sizeof(ptLinkState->ucComponentId);   
    ptrXmlMsgBuf = ptrOmmMsgBuf + wMsgLen;
    /*拷贝XML声明部分*/
    wMsgLen += strlen(ptLinkState->xmlDeclText); 
    strncat(ptrXmlMsgBuf,&ptLinkState->xmlDeclText[0],MAX_SIZE_OMMMSG-2);       
    /*拷贝XML根节点头*/
    wMsgLen += strlen(ptrConfHead);
    strncat(ptrXmlMsgBuf,ptrConfHead,MAX_SIZE_OMMMSG-2);     
    /*拷贝T段文本*/
    wMsgLen += strlen(ptLinkState->xmlTSecText);    
    strncat(ptrXmlMsgBuf,&ptLinkState->xmlTSecText[0],MAX_SIZE_OMMMSG-2);    
    /*拷贝D段头*/
    wMsgLen += strlen(ptrDSecHead);    
    strncat(ptrXmlMsgBuf,ptrDSecHead,MAX_SIZE_OMMMSG-2);        
    /*拷贝OT节点文本*/
    wMsgLen += strlen(ptrOTNodeText);   
    strncat(ptrXmlMsgBuf,&ptrOTNodeText[0],MAX_SIZE_OMMMSG-2);         
    /*拷贝OR节点文本*/
    wMsgLen += strlen(ptrORHeadNodeText);   
    strncat(ptrXmlMsgBuf,&ptrORHeadNodeText[0],MAX_SIZE_OMMMSG-2);
    if(bOR)
    {
        CHAR ptrORText[] = "OK";
        wMsgLen += strlen(ptrORText);
        strncat(ptrXmlMsgBuf,&ptrORText[0],MAX_SIZE_OMMMSG-2);
    }
    else
    {
        CHAR ptrORText[] = "NS";
        wMsgLen += strlen(ptrORText);
        strncat(ptrXmlMsgBuf,&ptrORText[0],MAX_SIZE_OMMMSG-2);
    }    
    wMsgLen += strlen(ptrOREndNodeText);
    strncat(ptrXmlMsgBuf,&ptrOREndNodeText[0],MAX_SIZE_OMMMSG-2);
    /*拷贝DC节点文本*/
    wMsgLen += strlen(ptrDCNodeText);   
    strncat(ptrXmlMsgBuf,&ptrDCNodeText[0],MAX_SIZE_OMMMSG-2);             
    /*拷贝EOF节点文本*/
    wMsgLen += strlen(ptrEOFHeadNodeText);   
    strncat(ptrXmlMsgBuf,&ptrEOFHeadNodeText[0],MAX_SIZE_OMMMSG-2);
    if(bEof)
    {
        CHAR ptrEOFText[] = "Y";
        wMsgLen += strlen(ptrEOFText);
        strncat(ptrXmlMsgBuf,&ptrEOFText[0],MAX_SIZE_OMMMSG-2);
    }
    else
    {
        CHAR ptrEOFText[] = "N";
        wMsgLen += strlen(ptrEOFText);
        strncat(ptrXmlMsgBuf,&ptrEOFText[0],MAX_SIZE_OMMMSG-2);
    }    
    wMsgLen += strlen(ptrEOFEndNodeText);
    strncat(ptrXmlMsgBuf,&ptrEOFEndNodeText[0],MAX_SIZE_OMMMSG-2);
    
    /*拷贝MD节点头文本*/
    wMsgLen += strlen(ptrMDNodeHeadText);
    strncat(ptrXmlMsgBuf,&ptrMDNodeHeadText[0],MAX_SIZE_OMMMSG-2);    
   
    /*将获取的回显串转换为UTF-8格式*/    
    if ('\0' != ptrOutputResult[0])
    {
        iBufLen = strlen(ptrOutputResult);
        iBufLen *= 2;
        ptrUTF8Buf = (CHAR *)XOS_GetUB(iBufLen);
        if (NULL == ptrUTF8Buf)
        {
            XOS_SysLog(OAM_CFG_LOG_NOTICE,
                       "....................FUNC:%s LINE:%d XOS_GetUB Error! ....................",__func__,__LINE__);
        }
        else 
        {
            memset(ptrUTF8Buf,0,iBufLen);
            /*编码转换*/
            if (!Gb2312ToUtf8(ptrOutputResult, iBufLen,ptrUTF8Buf))
            {
                XOS_SysLog(OAM_CFG_LOG_NOTICE,
                       "....................FUNC:%s LINE:%d Gb2312ToUtf8 Error! ....................",__func__,__LINE__);            
            }
            else
            {
                /*拷贝返回结果*/
                wMsgLen += strlen(ptrUTF8Buf);
                strncat(ptrXmlMsgBuf,ptrUTF8Buf,MAX_SIZE_OMMMSG-2);                
            }
            OAM_RETUB(ptrUTF8Buf);
        }
    }
   /*拷贝MD节点尾文本*/
   wMsgLen += strlen(ptrMDNodeEndText);
   strncat(ptrXmlMsgBuf,&ptrMDNodeEndText[0],MAX_SIZE_OMMMSG-2);
    /*拷贝D段尾*/
    wMsgLen += strlen(ptrDSecEnd);   
    strncat(ptrXmlMsgBuf,ptrDSecEnd,MAX_SIZE_OMMMSG-2);    
    /*拷贝XML根尾点头*/
    wMsgLen += strlen(ptrConfEnd);       
    strncat(ptrXmlMsgBuf,ptrConfEnd,MAX_SIZE_OMMMSG-2);    
    if (bDebugPrint)
    {
        PrintMsgXml((TYPE_XMLMODEL *)ptrOmmMsgBuf, "MSG_XMLAGNET_TO_OMM");
    }
    /*向OMM发送消息*/
    XOS_SendAsynMsg(EV_XML_OMMCFG_ACK,(BYTE *)ptrOmmMsgBuf,wMsgLen, 0, 0, &(ptLinkState->tDtJid));
    XOS_SysLog(OAM_CFG_LOG_NOTICE,
                       "....................FUNC:%s LINE:%d SendOMMAckToOmm Successfully! ....................",__func__,__LINE__);
    return;
}

/**********************************************************************
* 函数名称：VOID Oam_InitSaveLink
* 功能描述：初始化存盘链路
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：   无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2009-4-7             齐龙兆      创建
************************************************************************/
static VOID Oam_InitSaveLink(VOID)
{
    TYPE_LINK_STATE *pLinkState = &gt_SaveLinkState;
    memset(&gt_SaveLinkState,0,sizeof(gt_SaveLinkState));
    Oam_LinkInitializeForNewConn(pLinkState);
    /* 初始化为用户模式 */
    pLinkState->dwExeSeqNo = 0;
    Oam_LinkModeStack_Init(pLinkState);
    Oam_LinkModeStack_Push(pLinkState, CMD_MODE_USER);
    Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_TELNET);
    Oam_LinkSetLinkId(pLinkState, 1);
    pLinkState->ucSavedRight = DEFAULT_RITHT;
    pLinkState->ucUserRight = DEFAULT_ENABLE_RITHT;
    Oam_LinkSetInputState(pLinkState, INMODE_NORMAL);
    Oam_LinkModeStack_Push(pLinkState,CMD_MODE_PRIV);
    Oam_LinkModeStack_Push(pLinkState,CMD_MODE_MAINTAINANCE);
    return;
}


/**********************************************************************
* 函数名称：VOID Xml_ConstructTelnetSaveCmd
* 功能描述：构造Telnet存盘命令
* 访问的表：无
* 修改的表：无
* 输入参数：MSG_COMM_OAM *ptSendMsg
* 输出参数：无
* 返 回 值：   无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2009-04-08             齐龙兆      创建
************************************************************************/
static BOOLEAN Xml_ConstructTelnetSaveCmd(MSG_COMM_OAM *ptSendMsg,TYPE_TELNET_MSGHEAD  *pTelMsg)
{
    int i = 0;
    WORD16 wModule = 0;
    WORD16 wValue = 0;
    CHAR strMSaveType[] = "MASTER"; 
    CHAR strSSaveType[] = "SLAVE";
    CHAR strCmdLine [80] = "MSSAVE";
    CHAR strModuleNo[8] = {0};
    OPR_DATA *pOprData = NULL;
    /*参数检查*/
    if ((NULL == ptSendMsg) || (NULL == pTelMsg))
    {
        XOS_SysLog(OAM_CFG_LOG_NOTICE,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return FALSE;
    }
    pOprData =  (OPR_DATA *)ptSendMsg->Data;
    for (i = 0; i < ptSendMsg->Number; i++)
    {
        SWITCH(pOprData->ParaNo)
        {
            CASE(1)
	     /*lint -e420*/			
            memcpy(&wModule, pOprData->Data, sizeof(WORD16));
            BREAK
            CASE(1)
            memcpy(&wValue, pOprData->Data, sizeof(WORD16));
	     /*lint +e420*/			
            BREAK
            DEFAULT
            BREAK
        }
        END
        pOprData = GetNextOprData(pOprData);
    }
    snprintf(strModuleNo,sizeof(strModuleNo)," %d ",wModule);
    strncat(strCmdLine,strModuleNo,sizeof(strCmdLine));
    if (0 == wValue)/*主板存盘*/
    {
        strncat(strCmdLine,strMSaveType,sizeof(strCmdLine));
    }
    else if (1 == wValue)/*备板存盘*/
    {
        strncat(strCmdLine,strSSaveType,sizeof(strCmdLine));
    }
    else
    {
        return FALSE;
    }    
    pTelMsg->bMsgID = MSGID_SENDSTRING;
    pTelMsg->wSeqNo = 1;
    pTelMsg->bVtyNum = gt_SaveLinkState.ucLinkID;
    pTelMsg->bLastPacket = 1;
    pTelMsg->bDeterminer = 0;
    pTelMsg->wDataLen = strlen(strCmdLine);
    memcpy(&pTelMsg->pData, strCmdLine, strlen(strCmdLine));
    return TRUE;
}

/**********************************************************************
* 函数名称：VOID Xml_ExecSaveCmd
* 功能描述：存盘命令
* 访问的表：无
* 修改的表：无
* 输入参数：MSG_COMM_OAM *ptSendMsg:发送的OAM消息体
* 输出参数：无
* 返 回 值：   无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static VOID Xml_ExecSaveCmd(TYPE_XMLLINKSTATE *ptLinkState)
{
    CHAR *ptrOutputResult = NULL;
    static BYTE buffer[MAX_SEND_STRING_LEN + MSG_HDR_SIZE_OUT] = {0};
    TYPE_TELNET_MSGHEAD  *pTelMsg = NULL;    
    JID tSelfJid = {0};    
    WORD wDataLen = 0;
    
    /*参数检查*/
    if ((NULL == ptLinkState) || (NULL == ptLinkState->ptLastSendMsg))
    {
        XOS_SysLog(OAM_CFG_LOG_NOTICE,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return;
    }    
    /*检查是否有链路在执行命令操作*/
    if (CheckLinkIsExecutingCmd())
    {
        ptrOutputResult = ReadMapValueOfOamDat(MAP_TYPE_XMLADAPTER,MAP_KEY_CONFLICT_EXECUTECMD);
        SendErrStringToOmm(ptLinkState, ptrOutputResult);
        XOS_SysLog(OAM_CFG_LOG_NOTICE,
                       "....................FUNC:%s LINE:%d The other link is executing command! ....................",__func__,__LINE__);
        return;
    }
    /*初始化savelinkstate信息*/
    Oam_InitSaveLink();
    /*构造Telnet存盘命令信息*/
    pTelMsg = (TYPE_TELNET_MSGHEAD *)(buffer);
    if (!Xml_ConstructTelnetSaveCmd(ptLinkState->ptLastSendMsg,pTelMsg))
    {
        /*向网管发送构造存盘命令失败*/
	 ptrOutputResult = ReadMapValueOfOamDat(MAP_TYPE_XMLADAPTER,MAP_KEY_ERR_CONSTRUCTSAVECMD);
        SendErrStringToOmm(ptLinkState, ptrOutputResult);
        XOS_SysLog(OAM_CFG_LOG_NOTICE,
                       "....................FUNC:%s LINE:%d Xml_ConstructTelnetSaveCmd Error! ....................",__func__,__LINE__);
        return;
    }    
    wDataLen = sizeof(TYPE_TELNET_MSGHEAD) + pTelMsg->wDataLen - sizeof(BYTE);
    XOS_GetSelfJID(&tSelfJid);
    /*启动XML执行定时器*/
    Xml_SetExeTimer(ptLinkState);
    /*设置链路状态为运行状态*/
    XML_LinkSetCurRunState(ptLinkState, pec_MsgHandle_EXECUTE);
    XOS_SendAsynMsg(OAM_MSG_XMLAGNET_TO_INTERPRET, (BYTE *)buffer, (WORD16)wDataLen,
                    0, 0, &tSelfJid);
    return;
}

/**********************************************************************
* 函数名称：XML_AGENT_STAT SendOMMReqToPlat
* 功能描述：发送网管请求到平台
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_XMLMODEL *ptXmlModel:XML模型
                              LPXmlNode ptrNode:根节点
* 输出参数：无
* 返 回 值：   RETURN_ERROR-失败 RETURN_OK-成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static VOID SendOMMReqToPlat(TYPE_XMLLINKSTATE *ptLinkState,LPXmlNode ptrNodeNode)
{
    CHAR *ptrOutputResult = NULL;
    MSG_COMM_OAM *ptOAMMsgBuf = NULL;              /*OAM通用消息结构体*/    
    LPXmlNode ptrNodeNodeTmp = NULL;                   /*XML文档根节点*/    
    TYPE_XMLLINKSTATE *ptLinkStateTmp = NULL;     /*XML链路信息*/
    JID *ptDtJid = NULL;                                             /*接收JOB ID*/
    BYTE bExePos = 0;                                               /*命令执行位置*/    
    INT iRet = 0;                                                        /*操作状态*/
    /*参数检查*/
    if ((NULL == ptLinkState) || 
	 (NULL == ptLinkState->ptLastSendMsg) || 
	 (NULL == ptLinkState->tCurCMDInfo.cfgDat) || 
	 (NULL == ptLinkState->tCurCMDInfo.cfgDat->tCmdAttr) ||
	 (NULL == ptrNodeNode))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return;
    }

    /*初始化变量*/    
    ptrNodeNodeTmp = ptrNodeNode;   
    ptLinkStateTmp = ptLinkState;

    /*数组越界检查*/
    if (ptLinkStateTmp->tCurCMDInfo.wCmdAttrPos >= ptLinkStateTmp->tCurCMDInfo.cfgDat->nCmdAttrSum)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d Array Over Flow!%d-%d ....................\n",__func__,__LINE__,ptLinkStateTmp->tCurCMDInfo.wCmdAttrPos,ptLinkStateTmp->tCurCMDInfo.cfgDat->nCmdAttrSum);
        return;
    }
    bExePos = ptLinkStateTmp->tCurCMDInfo.cfgDat->tCmdAttr[ptLinkStateTmp->tCurCMDInfo.wCmdAttrPos].bExePos;
        
    /*申请内存*/
    ptOAMMsgBuf = (MSG_COMM_OAM *)XOS_GetUB(MAX_OAMMSG_LEN);
    if (NULL == ptOAMMsgBuf)
    {
        /*向网管发送申请内存失败*/
	 ptrOutputResult = ReadMapValueOfOamDat(MAP_TYPE_XMLADAPTER,MAP_KEY_ERR_GETUB);
        SendErrStringToOmm(ptLinkStateTmp, ptrOutputResult);        
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XOS_GetUB Error! ....................",__func__,__LINE__);
        return;
    }
    memset(ptOAMMsgBuf,0,MAX_OAMMSG_LEN);
    /*XML字符串转换为OAM通用结构体*/
    iRet = XmlMgt2OamMgt(ptrNodeNodeTmp,ptLinkStateTmp,ptOAMMsgBuf);    
    if (RETURN_OK != iRet)
    {
        /*向网管发送消息解析失败*/
	 ptrOutputResult = ReadMapValueOfOamDat(MAP_TYPE_XMLADAPTER,MAP_KEY_ERR_XMLMSG2OAM);
        SendErrStringToOmm(ptLinkStateTmp, ptrOutputResult);        
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XmlMgt2OamMgt Error,Error code %d! ....................",__func__,__LINE__,iRet);
        OAM_RETUB(ptOAMMsgBuf);
        return;
    }
    if (bDebugPrint)
    {
        PrintMSGCOMMOAMStruct(ptOAMMsgBuf, "MSG_XMLAGNET_TO_APP");
    }
    /*获取接收JOB ID*/
    ptDtJid = &(ptLinkStateTmp->tCurCMDInfo.cfgDat->tJid);
    
    if (NULL == ptDtJid)
    {
        /*向网管发送获取目标JID失败*/
	 ptrOutputResult = ReadMapValueOfOamDat(MAP_TYPE_XMLADAPTER,MAP_KEY_ERR_GETAPPJID);
        SendErrStringToOmm(ptLinkStateTmp, ptrOutputResult);	 
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        OAM_RETUB(ptOAMMsgBuf);
        return;
    }    
    /*解释端执行*/
    if((ptDtJid->dwJno == g_ptOamIntVar->jidSelf.dwJno) || (EXEPOS_INTERPRETATION == bExePos))
    {
        if(IsSaveCmd(ptOAMMsgBuf->CmdID))
        {
             /*存盘消息保存本次发送消息结构*/
            memset(ptLinkStateTmp->ptLastSendMsg,0,MAX_OAMMSG_LEN);
    	     memcpy(ptLinkStateTmp->ptLastSendMsg,ptOAMMsgBuf,ptOAMMsgBuf->DataLen);
            memcpy(ptOAMMsgBuf->LastData,ptLinkStateTmp->LastData,sizeof(ptOAMMsgBuf->LastData));
            memcpy(ptOAMMsgBuf->BUF,ptLinkStateTmp->BUF,sizeof(ptOAMMsgBuf->BUF));                
            Xml_ExecSaveCmd(ptLinkStateTmp);                
        }
	 else
        {
            /*设置链路状态为运行状态*/
            XML_LinkSetCurRunState(ptLinkState, pec_MsgHandle_EXECUTE);        
            DisposeInnerCmdForXmlAgent(ptOAMMsgBuf);
        }    
    }
    else
    {
        /*保存本次发送消息结构*/
        memset(ptLinkStateTmp->ptLastSendMsg,0,MAX_OAMMSG_LEN);
	 memcpy(ptLinkStateTmp->ptLastSendMsg,ptOAMMsgBuf,ptOAMMsgBuf->DataLen);
        memcpy(ptOAMMsgBuf->LastData,ptLinkStateTmp->LastData,sizeof(ptOAMMsgBuf->LastData));
        memcpy(ptOAMMsgBuf->BUF,ptLinkStateTmp->BUF,sizeof(ptOAMMsgBuf->BUF));                
            /*启动XML执行定时器*/
            Xml_SetExeTimer(ptLinkState);

            /* 命令发送，设置进度信息上报等待定时器 */
            XML_KillCmdPlanTimer(ptLinkState);
            XML_SetCmdPlanTimer(ptLinkState);

            /*设置链路状态为运行状态*/
            XML_LinkSetCurRunState(ptLinkState, pec_MsgHandle_EXECUTE);
	     /*向3G Plat发送消息*/
	      XOS_SendAsynMsg(EV_OAM_CFG_CFG, (BYTE *)ptOAMMsgBuf, ptOAMMsgBuf->DataLen, 0, 0,ptDtJid);
	      XOS_SysLog(OAM_CFG_LOG_NOTICE,
                       "....................FUNC:%s LINE:%d SendOMMReqToPlat Successfully! ....................",__func__,__LINE__);
	  } 
    OAM_RETUB(ptOAMMsgBuf);
    return;
}


/**********************************************************************
* 函数名称：XML_AGENT_STAT SendOPAckToOMM
* 功能描述：发送业务应答到网管
* 访问的表：无
* 修改的表：无
* 输入参数：CHAR *ptrOPAck:业务应答消息体
                              WORD16 wMsgLen:业务应答消息长度
                              LPXmlNode ptRootNode:根节点指针
* 输出参数：无
* 返 回 值：   RETURN_ERROR-失败 RETURN_OK-成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static VOID SendOPAckToOMM(CHAR *ptrOPAck,WORD16 wMsgLen,LPXmlNode ptRootNode)
{
    LPXmlNode ptRootNodeTmp = NULL; /*根节点结构变量*/    
    CHAR *ptrOPAckTmp = NULL;          /*业务应答消息体*/
    WORD32 dwLinkChannel = 0;            /*链路号*/    

    /*参数检查*/
    if ((NULL == ptrOPAck) || (0 == wMsgLen) || (NULL == ptRootNode))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return;
    }

    /*初始化变量*/
    ptrOPAckTmp = ptrOPAck;
    ptRootNodeTmp = ptRootNode;
	
    /*获取TRM节点值*/
    if(RETURN_OK != SetTRMNodeText(ptRootNodeTmp,&dwLinkChannel))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d SetTRMNodeText Error! ....................",__func__,__LINE__);
        return;
    }
    /*判断数组下表是否溢出*/
    if (dwLinkChannel >= MAX_SIZE_OPJID)
    {
    	XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d LinkChannel Error! ....................",__func__,__LINE__);
       return;
    }
    /*向OMM发送应答消息*/
    XOS_SendAsynMsg(EV_XML_OMMCFG_ACK, (BYTE *)ptrOPAckTmp,wMsgLen, 0, 0, &gt_OpJid[dwLinkChannel]);
    XOS_SysLog(OAM_CFG_LOG_NOTICE,
                     "....................FUNC:%s LINE:%d SendOPAckToOMM Successfully! ....................",__func__,__LINE__);    
    return;
    
}

/**********************************************************************
* 函数名称：XML_AGENT_STAT SendPLATAckToOMM
* 功能描述：发送平台应答到网管
* 访问的表：无
* 修改的表：无
* 输入参数：MSG_COMM_OAM *ptOamMsgBuf:OAM通用消息体 
* 输出参数：TYPE_XMLLINKSTATE **pptLinkState:XML链路状态
* 返 回 值：   RETURN_ERROR-失败 RETURN_OK-成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
VOID SendPLATAckToOMM(MSG_COMM_OAM *ptOamMsgBuf)
{
    CHAR *ptrOutputResult = NULL;
    MSG_COMM_OAM *ptOamMsgBufTmp = NULL;/*OAM通用消息体 */
    TYPE_XMLLINKSTATE *ptLinkState = NULL;     /*XML链路状态*/    
    CHAR *pOMMMsgBuf = NULL;   /*OMM应答消息体*/
    WORD16 wLinkStateIndex = 0;                     /*链路号*/
    WORD16 wMsgLen = 0;                                 /*OMM应答消息体长度*/    
    INT iRet = 0;                                                /*操作状态*/

    /*参数检查*/
    if (NULL == ptOamMsgBuf)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__); 
        return;
    }

    /*初始化变量*/
    ptOamMsgBufTmp = ptOamMsgBuf;       
    
    /*根据链路号获取链路状态*/
    wLinkStateIndex = (WORD16)(ptOamMsgBufTmp->LinkChannel&0X000001FF);    
    ptLinkState = Xml_GetLinkStateByIndex(wLinkStateIndex); 
    if (NULL == ptLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d Xml_GetLinkStateByIndex Error! ....................",__func__,__LINE__);  
        return;
    }
    /*杀掉定时器*/
    Xml_KillExeTimer(ptLinkState);

    /* 收到应用的返回结果之后，杀掉进度上报等待定时器 */
    XML_KillCmdPlanTimer(ptLinkState);
    
    /*当前链路空闲*/
    if (pec_MsgHandle_IDLE == ptLinkState->ucCurRunState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML link state is idle! ....................",__func__,__LINE__);  
        return;
    }
    if (NULL == ptLinkState->ptLastSendMsg)
    {  
        /*设置链路为空闲状态*/
        XML_LinkSetCurRunState(ptLinkState, pec_MsgHandle_IDLE);
        XOS_SysLog(OAM_CFG_LOG_NOTICE,
              "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);  
        return;
    }
    /*申请UB*/
    pOMMMsgBuf = (CHAR *)XOS_GetUB(MAX_SIZE_OMMACKMSG);
    if(NULL == pOMMMsgBuf)
    {
        /*向网管回复申请UB失败*/
        ptrOutputResult = ReadMapValueOfOamDat(MAP_TYPE_XMLADAPTER,MAP_KEY_ERR_GETUB);
        SendErrStringToOmm(ptLinkState, ptrOutputResult);
        /*设置链路为空闲状态*/
        XML_LinkSetCurRunState(ptLinkState, pec_MsgHandle_IDLE);
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XOS_GetUB Error! ....................",__func__,__LINE__);
        return;
    }
    memset(pOMMMsgBuf,0,MAX_SIZE_OMMACKMSG);    
    /*OAM消息转换为XML字符串*/
    iRet = OamMgt2XmlMgt(ptOamMsgBufTmp,ptLinkState,MAX_SIZE_OMMACKMSG,&wMsgLen,pOMMMsgBuf);
    if (RETURN_OK != iRet)
    {
        /*向网管回复OAM消息解析失败*/
	 ptrOutputResult = ReadMapValueOfOamDat(MAP_TYPE_XMLADAPTER,MAP_KEY_ERR_OAMMSG2XML);
        SendErrStringToOmm(ptLinkState, ptrOutputResult);
        /*设置链路为空闲状态*/
        XML_LinkSetCurRunState(ptLinkState, pec_MsgHandle_IDLE);
        OAM_RETUB(pOMMMsgBuf);
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d OamMgt2XmlMgt Error,code %d! ....................",__func__,__LINE__,iRet);         
        return;
    }
    /*保存应用返回数据,查询模式栈命令不保存,
    以免将上次执行的数据覆盖*/
    if((CMD_LDB_GET_MODESTATCK != ptOamMsgBufTmp->CmdID) && (!(IsSaveCmd(ptOamMsgBufTmp->CmdID))))
    {
        memcpy(ptLinkState->LastData,ptOamMsgBufTmp->LastData,sizeof(ptOamMsgBufTmp->LastData));
        memcpy(ptLinkState->BUF,ptOamMsgBufTmp->BUF,sizeof(ptOamMsgBufTmp->BUF));
    }    
    /*多包发送*/
    if (SUCC_CMD_NOT_FINISHED == ptOamMsgBufTmp->ReturnCode)
    {
        memcpy(ptLinkState->ptLastSendMsg->LastData,ptLinkState->LastData, sizeof(ptLinkState->LastData));	 
        memcpy(ptLinkState->ptLastSendMsg->BUF,ptLinkState->BUF, sizeof(ptLinkState->BUF));
	 ptLinkState->ptLastSendMsg->CmdRestartFlag = FALSE;
	 /*重启定时器*/
	 Xml_SetExeTimer(ptLinkState);
	 /*向OAM发送消息*/
	 XOS_SendAsynMsg(EV_OAM_CFG_CFG, (BYTE *)ptLinkState->ptLastSendMsg, ptLinkState->ptLastSendMsg->DataLen, 0, 0, &(ptLinkState->tCurCMDInfo.cfgDat->tJid));	 
    }
    else if (SUCC_WAIT_AND_HAVEPARA == ptOamMsgBufTmp->ReturnCode)
    {
	 /*重启定时器*/
	 Xml_SetExeTimer(ptLinkState);
    }
    else
    {
        /*设置链路为空闲状态*/
        XML_LinkSetCurRunState(ptLinkState, pec_MsgHandle_IDLE); 
    }
    if (bDebugPrint)
    {
        PrintMsgXml((TYPE_XMLMODEL *)pOMMMsgBuf, "MSG_XMLAGNET_TO_OMM");
    }
    if (!IsSaveCmd(ptOamMsgBufTmp->CmdID))
    {
        /*修改当前模式ID*/
        ChangeXmlLinkStateMode(ptOamMsgBufTmp->ReturnCode,ptLinkState);
    }
    /*向OMM发送消息*/
    XOS_SendAsynMsg(EV_XML_OMMCFG_ACK,(BYTE *)pOMMMsgBuf,wMsgLen, 0, 0, &(ptLinkState->tDtJid));
    /*释放内存*/   
    OAM_RETUB(pOMMMsgBuf);     
    return;		
}

/**********************************************************************
* 函数名称：VOID CeaseCmdReqProc
* 功能描述：发送命令终止信息到应用
* 访问的表：无
* 修改的表：无
* 输入参数：WORD32 dwTrmId:链路号
* 输出参数：无
* 返 回 值：XML_AGENT_STAT
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2009-12-4             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT CeaseCmdReqProc(WORD32 dwTrmId)
{
    CHAR *ptrOutputResult = NULL;
    INT iRet = 0;                                                /*操作结果*/
    TYPE_XMLLINKSTATE *pXmlLinkState = NULL;  /*链路信息指针*/
    T_OAM_Cfg_Dat * pCfgDat = NULL;                 /* 命令所在dat结构指针*/
    pXmlLinkState = Xml_GetLinkStateByLinkChannel(dwTrmId);
    /*未找到链路信息*/
    if (NULL == pXmlLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d Can not find xml link information! ....................",__func__,__LINE__);
        return NULL_PTR;
    }
    /*链路无命令执行*/
    if (pec_MsgHandle_IDLE == pXmlLinkState->ucCurRunState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d Link have not command! ....................",__func__,__LINE__);
        return NO_EXECUTE_CMD_ON_LINKCHANNEL;
    }
    pCfgDat = pXmlLinkState->tCurCMDInfo.cfgDat;
    /*命令脚本指针为空*/
    if((NULL == pCfgDat) ||
        (NULL == pCfgDat->tCmdAttr))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d pCfgDat is null! ....................",__func__,__LINE__);
        return NULL_PTR;
    }
    /*数组越界检查*/
    if (pXmlLinkState->tCurCMDInfo.wCmdAttrPos >= pCfgDat->nCmdAttrSum)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d Array Over Flow!%d-%d ....................\n",__func__,__LINE__,pXmlLinkState->tCurCMDInfo.wCmdAttrPos,pCfgDat->nCmdAttrSum);
        return RETURN_ERROR;
    }

    /*命令可终止且有应用回显*/
    if (CMD_ATTR_CANCEASE(&pCfgDat->tCmdAttr[pXmlLinkState->tCurCMDInfo.wCmdAttrPos]) &&
        CMD_ATTR_CANCEASE_NEEDACK(&pCfgDat->tCmdAttr[pXmlLinkState->tCurCMDInfo.wCmdAttrPos]))
    {        
        if (NULL == pXmlLinkState->ptLastSendMsg)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d Last message is null! ....................",__func__,__LINE__);
            return NULL_PTR;
        }
        /*重置定时器*/
        Xml_KillExeTimer(pXmlLinkState);
        Xml_SetExeTimer(pXmlLinkState);
        /*向应用发送命令终止请求*/
        iRet = XOS_SendAsynMsg(EV_OAM_CEASE_TO_APP, (BYTE *)pXmlLinkState->ptLastSendMsg, pXmlLinkState->ptLastSendMsg->DataLen, 0, 0, &(pCfgDat->tJid));
        if (XOS_SUCCESS != iRet)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d Send cease command to application error,error code = %d! ....................",__func__,__LINE__,iRet);
            return RETURN_ERROR;
        }
    }
    /*命令可终止且无应用回显*/
    else if(CMD_ATTR_CANCEASE(&pCfgDat->tCmdAttr[pXmlLinkState->tCurCMDInfo.wCmdAttrPos]))
    {
        SendStringToOmm(pXmlLinkState, Hdr_szCeasedTheCmdTip,TRUE,TRUE);
        /*杀掉XML链路定时器*/
        Xml_KillExeTimer(pXmlLinkState);
	 /*恢复状态为空闲*/
        XML_LinkSetCurRunState(pXmlLinkState,pec_MsgHandle_IDLE);
    }
    else/*命令不可终止*/
    {
        /*向客户端返回命令不可终止*/
	 ptrOutputResult = ReadMapValueOfOamDat(MAP_TYPE_XMLADAPTER,MAP_KEY_ERR_EXECUTEOMMCMD);
        SendStringToOmm(pXmlLinkState, ptrOutputResult,FALSE,FALSE);
    }
    return RETURN_OK;
}

/**********************************************************************
* 函数名称：CHAR* XML_GetTextForUTF
* 功能描述：获取UTF-8文本
* 访问的表：无
* 修改的表：无
* 输入参数：pXmlDocPtr:文档指针
                             pucComponentId:组建号
* 输出参数：无
* 返 回 值：CHAR *
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static CHAR* XML_GetTextForUTF(XmlDocPtr pXmlDocPtr,const CHAR *pucComponentId)
{
    XML_STAT dwRet = 0;
    CHAR *pOMMMsgBuf = NULL;
    CHAR *pXMLMsgBuf = NULL;
    CHAR *pXMLChar = NULL;
    CHAR *pEncoding = " encoding=\"UTF-8\"";
    WORD16 wLen = 0;
    WORD16 wLoop = 0;
    WORD16 wEncodingLen = strlen(pEncoding);
    int options = XML_STAT_PARSE_NOBLANKS;
    WORD16 wActualLen = 0;
    /*参数检查*/
    if((NULL == pXmlDocPtr) || (NULL == pucComponentId))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                               "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return NULL;
    }
    pOMMMsgBuf = (CHAR *)XOS_GetUB(MAX_SIZE_OMMACKMSG);
    /*申请内存失败*/
    if (NULL == pOMMMsgBuf)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XOS_GetUB Error! ....................",__func__,__LINE__);
        return NULL;
    }
    /*拷贝头文件*/
    memset(pOMMMsgBuf,0,MAX_SIZE_OMMACKMSG);
    memcpy(pOMMMsgBuf,pucComponentId,2);
    pXMLMsgBuf = pOMMMsgBuf + 2;
    wLen = MAX_SIZE_OMMACKMSG - 2;
    do
    {
        dwRet = XML_GetText(pXmlDocPtr, pXMLMsgBuf, wLen, options,&wActualLen);
        /*获取文本失败*/
        if (XML_OK != dwRet)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR,
                                "....................FUNC:%s LINE:%d XML_GetText Error,Error code %d! ....................",__func__,__LINE__,dwRet);
            break;
        }
        pXMLChar = pXMLMsgBuf;
        /*查找"?>"*/
        while(('\0' != *pXMLChar) && (wLoop < wActualLen))
        {
            if (('?' == *pXMLChar) && ('>' == *(pXMLChar+1)))
            {
                break;
            }
            pXMLChar++;
            wLoop++;
        }
        /*未找到*/
        if ((wLoop >= wActualLen) || ('\0' == *pXMLChar))
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR,
                                "....................FUNC:%s LINE:%d XML Char Error,%d,%d! ....................",__func__,__LINE__,wLoop,wActualLen);
            break;
        }
        /*空间不足*/
        if ((wLen - wActualLen) <=  wEncodingLen)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR,
                                "....................FUNC:%s LINE:%d MEM Space Error,%d,%d! ....................",__func__,__LINE__,wLen - wActualLen,wEncodingLen);
            break;
        }
        /*往后偏移" encoding=\"UTF-8\" "长度*/
        memmove(pXMLChar+wEncodingLen,pXMLChar,wActualLen - (pXMLChar - pXMLMsgBuf));
        /*插入编码*/
        memcpy(pXMLChar,pEncoding,wEncodingLen);
        return pOMMMsgBuf;
    }while(0);
    /*释放缓冲区*/
    OAM_RETUB(pOMMMsgBuf);
    return NULL;
}

/**********************************************************************
* 函数名称：VOID ProcCeaseCmd
* 功能描述：命令终止处理函数
* 访问的表：无
* 修改的表：无
* 输入参数：BYTE *pucComponentId:组建号
                             WORD16 wIdLen:组建号长度
                             LPXmlNode ptRootNode:根节点指针
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
static XML_AGENT_STAT ProcCeaseCmd(const LPXmlNode ptOriRtNode)
{
    WORD32 dwRet = 0;
    WORD32 dwTrmId = 0;
    /*参数检查*/
    if (NULL == ptOriRtNode)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................\n",__func__,__LINE__);
        return NULL_PTR;
    }
    dwRet = SetTRMNodeText(ptOriRtNode,&dwTrmId);
    /*获取原<TRM>文本失败*/
    if (XML_OK != dwRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d GetNodeText Error!%d ....................\n",__func__,__LINE__,dwRet);
        return GET_NODE_TEXT_ERR;
    }
    /*发送命令终止请求到应用*/
    dwRet = CeaseCmdReqProc(dwTrmId);    
    return dwRet;
}

/**********************************************************************
* 函数名称：void ReceiveFromOmm
* 功能描述：OMM发送消息到OAM
* 访问的表：无
* 修改的表：无
* 输入参数：LPVOID ptrMsgBuf:OMM消息体
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
VOID ReceiveFromOmm(LPVOID ptrMsgBuf)
{ 
    CHAR *ptrOutputResult = NULL;
    TYPE_XMLMODEL *ptXmlModel = NULL;                      /*XML模型指针*/    
    TYPE_XMLLINKSTATE *ptLinkState = NULL;                 /*链路状态指针*/
    XmlDocPtr ptrDoc = NULL;                                         /*xml文档指针*/
    XmlNode tRootNode;                                                /*根节点结构变量*/    
    WORD16 wMsgLen = 0;                                            /*OMM消息体长度*/
    WORD32  dwXmlLen = 0;                                         /*OMM消息体XML数据长度*/
    BOOLEAN bIs3GPL = TRUE;                                      /*网元类型是否为3GPL*/
    INT iRet = 0;                                                        /*操作状态*/
    INT iOptions = XML_STAT_PARSE_NOBLANKS;           /*解析参数*/
   
    /*入参判断*/
    if (NULL == ptrMsgBuf)
    {
         XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
         return;
    }

    /*初始化变量*/  
    memset(pRecvOMMMsgBuf,0,MAX_SIZE_OMMMSG);    
    memset(&tRootNode,0,sizeof(XmlNode)); 

    /*获取消息长度*/
    XOS_GetMsgDataLen(&wMsgLen);
    if((wMsgLen >= MAX_SIZE_OMMMSG) || (0 == wMsgLen))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XOS_GetMsgDataLen %d Error!....................",__func__,__LINE__,wMsgLen);        
        return;
    }
    memcpy(pRecvOMMMsgBuf,ptrMsgBuf,wMsgLen);    
    ptXmlModel = (TYPE_XMLMODEL *)pRecvOMMMsgBuf;
    dwXmlLen = strlen(ptXmlModel->ucXML);
    if (bDebugPrint)
    {
        PrintMsgXml(ptXmlModel, "MSG_OMM_TO_XMLAGNET");
    }    
    /*解析XML文档并返回根节点*/
    iRet = XML_ParaseDoc(ptXmlModel->ucXML,dwXmlLen,iOptions,&ptrDoc,&tRootNode);
    if (RETURN_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_ParaseDoc Error,Error code %d! ....................",__func__,__LINE__,iRet);    
	 return ;
    }
    /*检查网元类型是否为3GPL*/
    iRet = CheckNtpNodeTextIs3GPL(&tRootNode,&bIs3GPL);
    if (RETURN_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d CheckNtpNodeTextIs3GPL Error,Error code %d! ....................",__func__,__LINE__,iRet);    
	 return ;
    }
    /*网元类型为3GPL--平台内部处理*/    
    if (bIs3GPL)
    {
        CHAR ptrCommId[2] = {0};/*组件号*/
        JID tOmmJid = {0};/*网管端JID*/	 
	 /*获取网管端JID*/
	 XOS_Sender(&tOmmJid);
	 /*获取网管组建号*/
	 memcpy(ptrCommId,pRecvOMMMsgBuf,sizeof(ptrCommId));
	 /*保存链路信息*/
	 iRet = SetXmlLinkState(ptXmlModel,&tRootNode,&ptLinkState);
	 switch(iRet)
	 {
            case RETURN_OK:
                /*向平台发送配置请求*/
                SendOMMReqToPlat(ptLinkState,&tRootNode);	    
                break;
            case SHOW_MODESTACK_ERR:/*查询模式栈信息错误*/
	     {
	         if (XML_LINKSTATE_VERCHANGED == ptLinkState->bVerChanged)
	         {
	             /*向网管发送DAT脚本已切换信息*/	
	             ptrOutputResult = ReadMapValueOfOamDat(MAP_TYPE_XMLADAPTER,MAP_KEY_CHANGEMODEFORDAT);
	         }
                else
                {
                    /*向网管发送模式栈错误信息*/	
                    ptrOutputResult = ReadMapValueOfOamDat(MAP_TYPE_XMLADAPTER,MAP_KEY_NO_FINDCMDINMODE);
                }
	         ptLinkState->bVerChanged = XML_LINKSTATE_VERUNCHANGED;
                ProcErrPlatCfgMsg(ptrCommId,&tRootNode,ptrOutputResult,&tOmmJid);
		  break;
	     }
            case DatUpdateMutex:/*当前正在切换版本,不能执行命令*/
	         ProcErrPlatCfgMsg(ptrCommId, &tRootNode,Hdr_szDatUpdateMutexTip,&tOmmJid);
                break;
            case SaveMutexTip:/*有链路正在存盘,不能执行命令*/            
                ProcErrPlatCfgMsg(ptrCommId,&tRootNode ,Hdr_szSaveMutexTip,&tOmmJid);
                break;
	     case CmdExecutMutex:/*当前链路正在执行命令*/
                ptrOutputResult = ReadMapValueOfOamDat(MAP_TYPE_XMLADAPTER,MAP_KEY_EXECUTING_CMD);
                ProcErrPlatCfgMsg(ptrCommId,&tRootNode , ptrOutputResult,&tOmmJid);		   
                break;
            case NoRegXmlLinkState:/*未注册XML链路*/
                ptrOutputResult = ReadMapValueOfOamDat(MAP_TYPE_XMLADAPTER,MAP_KEY_NO_REGISTEREDTRM);
                ProcErrPlatCfgMsg(ptrCommId, &tRootNode ,ptrOutputResult,&tOmmJid);		   
                break;            
            default:
            {
                ptrOutputResult = ReadMapValueOfOamDat(MAP_TYPE_XMLADAPTER,MAP_KEY_ERR_XMLLINKINFO);
                ProcErrPlatCfgMsg(ptrCommId, &tRootNode ,ptrOutputResult,&tOmmJid);	         
                XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d SetXmlLinkState Error,Error code %d! ....................",__func__,__LINE__,iRet);
                break;
            }		   
	 }	 
    } 
    else
    {
        /*透传给业务*/        
	 SendOMMReqToOP(pRecvOMMMsgBuf,wMsgLen,&tRootNode);	 
    }
    /*释放文档指针*/
    XML_FreeXml(ptrDoc);    
    return;
}

/**********************************************************************
* 函数名称：void RecvFromProtocolToOMM
* 功能描述：OAM发送消息到OMM
* 访问的表：无
* 修改的表：无
* 输入参数：LPVOID ptrMsgBuf：OAM消息体            
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
VOID RecvFromProtocolToOMM(LPVOID ptrMsgBuf)
{   
    MSG_COMM_OAM *ptOamMsgBuf = NULL;                       /*OAM消息体临时指针*/
    WORD16 wMsgLen = 0;                                                  /*消息体长度*/
    	
    /*入参判断*/
    if(ptrMsgBuf == NULL)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__); 
        return;
    }
    memset(pRecvPlatMsgBuf,0,sizeof(pRecvPlatMsgBuf)); 
    /*获取消息长度*/
    XOS_GetMsgDataLen(&wMsgLen);
    if ((wMsgLen > sizeof(pRecvPlatMsgBuf))||(0 == wMsgLen))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XOS_GetMsgDataLen %d Error! ....................",__func__,__LINE__,wMsgLen);         
        return;
    }
    memcpy(pRecvPlatMsgBuf,ptrMsgBuf,wMsgLen);
    ptOamMsgBuf = (MSG_COMM_OAM *)pRecvPlatMsgBuf;
    if (bDebugPrint)
    {
        PrintMSGCOMMOAMStruct(ptOamMsgBuf, "MSG_APP_TO_XMLAGNET");
    }
    /*?? brs的返回码大于10时候要转换为错误码map类型*/
    if (ptOamMsgBuf->ReturnCode >= 10 &&
        ptOamMsgBuf->ReturnCode != SUCC_WAIT_AND_HAVEPARA) 
    {
        ConstructBrsMapPara(ptOamMsgBuf);
    }
    /*发送平台应答到网管*/
    SendPLATAckToOMM(ptOamMsgBuf);        
    return;
}

/**********************************************************************
* 函数名称：void RecvFromOP
* 功能描述：OAM发送消息到OMM
* 访问的表：无
* 修改的表：无
* 输入参数：LPVOID ptrMsgBuf：OAM消息体            
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-9-2             齐龙兆      创建
************************************************************************/
VOID RecvFromOP(LPVOID ptrMsgBuf)
{    
    TYPE_XMLMODEL *ptXmlModel = NULL;                 /*XML模型指针*/
    XmlDocPtr ptrDoc = NULL;                                     /*xml文档指针*/
    XmlNode tRootNode;                                            /*根节点结构变量*/    
    WORD16 wMsgLen = 0;                                        /*OMM消息体长度*/    
    WORD32  dwXmlLen = 0;                                      /*OMM消息体XML数据长度*/    
    INT iOptions = XML_STAT_PARSE_NOBLANKS;       /*解析参数*/
        
    /*入参判断*/
    if(ptrMsgBuf == NULL)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__); 
        return;
    }

    memset(pRecvOMMMsgBuf,0,MAX_SIZE_OMMMSG);
    memset(&tRootNode,0,sizeof(XmlDocPtr));
    /*获取消息长度*/
    XOS_GetMsgDataLen(&wMsgLen);
    if ((wMsgLen >= MAX_SIZE_OMMMSG) || (0 == wMsgLen))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XOS_GetMsgDataLen %d Error! ....................",__func__,__LINE__,wMsgLen);
        return;
    }
    memcpy(pRecvOMMMsgBuf,ptrMsgBuf,wMsgLen);
    ptXmlModel = (TYPE_XMLMODEL *)pRecvOMMMsgBuf;
    dwXmlLen = strlen(ptXmlModel->ucXML);
    if (bDebugPrint)
    {
        PrintMsgXml(ptXmlModel, "MSG_OP_TO_XMLAGNET");
    }
    /*解析XML文档并返回根节点*/
    if (RETURN_OK != XML_ParaseDoc(ptXmlModel->ucXML,dwXmlLen,iOptions,&ptrDoc,&tRootNode))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_ParaseDoc Error! ....................",__func__,__LINE__);
	 return ;
    }
    /*向网管发送业务应答消息*/
    SendOPAckToOMM(pRecvOMMMsgBuf,wMsgLen,&tRootNode);    
    /*释放文档指针*/
    XML_FreeXml(ptrDoc);     
    return;
}

/**********************************************************************
* 函数名称：VOID ReceiveRegisterMsgFromOmm
* 功能描述：接收网管端注册请求
* 访问的表：无
* 修改的表：无
* 输入参数：LPVOID ptrMsgBuf：注册请求消息体            
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-12-17             齐龙兆      创建
************************************************************************/
VOID ReceiveRegisterMsgFromOmm(LPVOID ptrMsgBuf)
{
    CHAR pRecvMsgBuf[MAX_SIZE_OMMREGISTERREQ_BUFFER];/*接收消息缓冲区*/
    WORD16 wMsgLen = 0; /*消息长度*/    
    WORD16 wLoop = 0;
    JID tDtJid;/*发送端JID*/    
    OMMRegisterReq *ptOMMRegisterReq = NULL;/*请求消息指针*/
    OMMRegisterAck tOMMRegisterAck = {0};/*应答消息指针*/		
    
    /*入参判断*/
    if (NULL == ptrMsgBuf)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return;
    }

    memset(pRecvMsgBuf,0,MAX_SIZE_OMMREGISTERREQ_BUFFER);
    memset(&tDtJid,0,sizeof(JID));    
    
    if (XOS_SUCCESS != XOS_Sender(&tDtJid))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XOS_Sender Error! ....................",__func__,__LINE__);
	 return;
    }
    /*获取消息长度*/
    XOS_GetMsgDataLen(&wMsgLen);
    if ((wMsgLen > MAX_SIZE_OMMREGISTERREQ_BUFFER) || (0 == wMsgLen))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XOS_GetMsgDataLen %d Error! ....................",__func__,__LINE__,wMsgLen);
        return;
    } 
    memcpy(pRecvMsgBuf,ptrMsgBuf,wMsgLen);
    ptOMMRegisterReq = (OMMRegisterReq *)pRecvMsgBuf;    
    tOMMRegisterAck.wComponentId = ptOMMRegisterReq->wComponentId;
    tOMMRegisterAck.wTRMNum = ptOMMRegisterReq->wTRMNum;    
    for (wLoop = 0;wLoop < ptOMMRegisterReq->wTRMNum;wLoop++)
    {
	 /*注册终端号*/
        if (RETURN_OK != RegisterTrmId(ptOMMRegisterReq->awTRMId[wLoop]))
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d RegisterTrmId %d Error! ....................",__func__,__LINE__,ptOMMRegisterReq->awTRMId[wLoop]);
            tOMMRegisterAck.awOR[wLoop] = XML_AGENT_ERR;
        }
	 else
	 {
	     tOMMRegisterAck.awOR[wLoop] = XML_AGENT_OK;
	 }
	 tOMMRegisterAck.awTRMId[wLoop] = ptOMMRegisterReq->awTRMId[wLoop];
    }
    /*统计应答消息体长度*/
    wMsgLen = sizeof(tOMMRegisterAck.wComponentId) + sizeof(tOMMRegisterAck.wTRMNum) + tOMMRegisterAck.wTRMNum * (sizeof(tOMMRegisterAck.awTRMId)/MAX_COUNT_TRM + sizeof(tOMMRegisterAck.awOR)/MAX_COUNT_TRM);
    /*打印消息*/
    XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d ComponentId:%d,TRMNum:%d! ....................",__func__,__LINE__,tOMMRegisterAck.wComponentId,tOMMRegisterAck.wTRMNum); 
    for (wLoop = 0;wLoop < tOMMRegisterAck.wTRMNum;wLoop++)
    {
        XOS_SysLog(OAM_CFG_LOG_NOTICE,
                       "....................FUNC:%s LINE:%d TRMId:%d,OR:%d! ....................",__func__,__LINE__,tOMMRegisterAck.awTRMId[wLoop],tOMMRegisterAck.awOR[wLoop]);
    }
    /*向OMM发送注册应答*/    
    XOS_SendAsynMsg(EV_XML_REGISTER_ACK, (BYTE *)&tOMMRegisterAck,wMsgLen, 0, 0, &tDtJid);    
    XOS_SysLog(OAM_CFG_LOG_NOTICE,
                       "....................FUNC:%s LINE:%d Send RegisterAck To OMM Successfully! ....................",__func__,__LINE__);    
    return;   
}

/**********************************************************************
* 函数名称：VOID ReceiveUnRegisterMsgFromOmm
* 功能描述：接收网管端注销请求
* 访问的表：无
* 修改的表：无
* 输入参数：LPVOID ptrMsgBuf：注册请求消息体            
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-12-17             齐龙兆      创建
************************************************************************/
VOID ReceiveUnRegisterMsgFromOmm(LPVOID ptrMsgBuf)
{
    CHAR pRecvMsgBuf[MAX_SIZE_OMMUNREGISTERREQ_BUFFER];/*接收消息缓冲区*/
    WORD16 wLoop = 0;
    WORD16 wMsgLen = 0;/*消息长度*/    
    JID tDtJid;/*发送端JID*/  
    OMMUnRegisterReq *ptOMMUnRegisterReq = NULL;/*请求消息指针*/
    OMMUnRegisterAck tOMMUnRegisterAck = {0};/*应答消息指针*/      
		
    /*入参判断*/
    if (NULL == ptrMsgBuf)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return;
    }

    memset(pRecvMsgBuf,0,MAX_SIZE_OMMUNREGISTERREQ_BUFFER);
    memset(&tDtJid,0,sizeof(JID));

    if (XOS_SUCCESS != XOS_Sender(&tDtJid))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XOS_Sender Error! ....................",__func__,__LINE__);
	 return;
    }
    /*获取消息长度*/
    XOS_GetMsgDataLen(&wMsgLen);
    if ((wMsgLen > MAX_SIZE_OMMUNREGISTERREQ_BUFFER) || (0 == wMsgLen))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XOS_GetMsgDataLen Error! ....................",__func__,__LINE__);
        return;
    }
    memcpy(pRecvMsgBuf,ptrMsgBuf,wMsgLen); 
    ptOMMUnRegisterReq = (OMMUnRegisterReq *)pRecvMsgBuf;    
    tOMMUnRegisterAck.wComponentId = ptOMMUnRegisterReq->wComponentId;
    tOMMUnRegisterAck.wTRMNum = ptOMMUnRegisterReq->wTRMNum;    
    for (wLoop = 0;wLoop < ptOMMUnRegisterReq->wTRMNum;wLoop++)
    {
	 /*注销终端号*/
        if (RETURN_OK != UnRegisterTrmId(ptOMMUnRegisterReq->awTRMId[wLoop]))
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d UnRegisterTrmId %d Error! ....................",__func__,__LINE__,ptOMMUnRegisterReq->awTRMId[wLoop]);
	     tOMMUnRegisterAck.awOR[wLoop]= XML_AGENT_ERR;
        }
	 else
	 {
	     tOMMUnRegisterAck.awOR[wLoop]= XML_AGENT_OK;
	 }	 
	 tOMMUnRegisterAck.awTRMId[wLoop] = ptOMMUnRegisterReq->awTRMId[wLoop];
    }    
    /*统计应答消息体长度*/
    wMsgLen = sizeof(tOMMUnRegisterAck.wComponentId) + sizeof(tOMMUnRegisterAck.wTRMNum) + tOMMUnRegisterAck.wTRMNum * (sizeof(tOMMUnRegisterAck.awTRMId)/MAX_COUNT_TRM + sizeof(tOMMUnRegisterAck.awOR)/MAX_COUNT_TRM);    
    /*打印消息*/
    XOS_SysLog(OAM_CFG_LOG_NOTICE,
                       "....................FUNC:%s LINE:%d ComponentId %d,TRMNum %d! ....................",__func__,__LINE__,tOMMUnRegisterAck.wComponentId,tOMMUnRegisterAck.wTRMNum);
    for (wLoop = 0;wLoop < tOMMUnRegisterAck.wTRMNum;wLoop++)
    {
        XOS_SysLog(OAM_CFG_LOG_NOTICE,
                       "....................FUNC:%s LINE:%d TRMId %d,OR %d! ....................",__func__,__LINE__,tOMMUnRegisterAck.awTRMId[wLoop],tOMMUnRegisterAck.awOR[wLoop]);        
    }
    /*向OMM发送注销应答*/
    XOS_SendAsynMsg(EV_XML_UNREGISTER_ACK, (BYTE *)&tOMMUnRegisterAck,wMsgLen, 0, 0, &tDtJid);    
    XOS_SysLog(OAM_CFG_LOG_NOTICE,
                       "....................FUNC:%s LINE:%d Send UnRegister Ack To OMM Successfully! ....................",__func__,__LINE__);    
    return;
}

/**********************************************************************
* 函数名称：VOID ReceiveUnRegisterMsgFromOmm
* 功能描述：接收网管端握手请求
* 访问的表：无
* 修改的表：无
* 输入参数：LPVOID ptrMsgBuf：握手请求消息体            
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-12-17             齐龙兆      创建
************************************************************************/
VOID ReceiveHandShakeMsgFromOmm(LPVOID ptrMsgBuf)
{
    CHAR pRecvMsgBuf[MAX_SIZE_OMMHANDSHAKEREQ_BUFFER];/*接收消息缓冲区*/
    JID tDtJid;
    WORD16 wMsgLen = 0;
    WORD16 wLoop = 0;
    OMMHandShankReq *ptOMMHandShankReq = NULL;/*请求消息体*/
    OMMHandShankAck tOMMHandShankAck = {0};/*应答消息体*/
	    
    if(NULL == ptrMsgBuf)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);        
	 return;
    }
    memset(pRecvMsgBuf,0,MAX_SIZE_OMMHANDSHAKEREQ_BUFFER);
    memset(&tDtJid,0,sizeof(JID));

    if (XOS_SUCCESS != XOS_Sender(&tDtJid))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XOS_Sender Error! ....................",__func__,__LINE__);
	 return;
    }
    /*获取消息长度*/
    XOS_GetMsgDataLen(&wMsgLen);
    if ((wMsgLen > MAX_SIZE_OMMHANDSHAKEREQ_BUFFER) || (0 == wMsgLen))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XOS_GetMsgDataLen Error! ....................",__func__,__LINE__);
        return;
    } 
    memcpy(pRecvMsgBuf,ptrMsgBuf,wMsgLen); 
    ptOMMHandShankReq = (OMMHandShankReq *)pRecvMsgBuf;
    
    /*设置握手应答消息*/
    if (RETURN_OK != SetOMMHandShakeAck(ptOMMHandShankReq,&tOMMHandShankAck))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d SetOMMHandShakeAck Error! ....................",__func__,__LINE__);	 
        return;
    }
    /*杀掉握手定时器*/
    Xml_KillHandShakeTimer();
    /*启动握手定时器*/
    Xml_SetHandShakeTimer();    
    wMsgLen = sizeof(tOMMHandShankAck.wComponentId) + sizeof(tOMMHandShankAck.wTRMNum) + tOMMHandShankAck.wTRMNum *(sizeof(tOMMHandShankAck.aucTRMId)/MAX_COUNT_TRM);
    /*打印消息*/
    XOS_SysLog(OAM_CFG_LOG_NOTICE,
                       "....................FUNC:%s LINE:%d ComponentId %d,TRMNum %d! ....................",__func__,__LINE__,tOMMHandShankAck.wComponentId,tOMMHandShankAck.wTRMNum);   
    for (wLoop = 0;wLoop < tOMMHandShankAck.wTRMNum;wLoop++)
    {
        XOS_SysLog(OAM_CFG_LOG_NOTICE,
                       "....................FUNC:%s LINE:%d TRMId %d ....................",__func__,__LINE__,tOMMHandShankAck.aucTRMId[wLoop]);
    }
    /*向OMM发送握手应答*/
    XOS_SendAsynMsg(EV_XML_HANDSHAKE_ACK, (BYTE *)&tOMMHandShankAck,wMsgLen, 0, 0, &tDtJid);
    XOS_SysLog(OAM_CFG_LOG_NOTICE,
                       "....................FUNC:%s LINE:%d Send HandShakeAck To OMM Successfully! ....................",__func__,__LINE__);
    return;
}

/**********************************************************************
* 函数名称：VOID ReceiveLinkCfgMsgFromOmm
* 功能描述：接收网管端链路配置请求
* 访问的表：无
* 修改的表：无
* 输入参数：LPVOID ptrMsgBuf：链路配置请求
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-12-17             齐龙兆      创建
************************************************************************/
VOID ReceiveLinkCfgMsgFromOmm(LPVOID ptrMsgBuf)
{
    CHAR *paucRecvOMMMsgBuf = NULL;                       /*接收消息缓冲区*/
    CHAR aucNodeText[MAX_NODETEXT_BUF_SIZE] = {0};/*节点文本*/
    BYTE aucComponentId[2] = {0};
    INT iRet = 0;                                                           /*操作状态*/
    INT iOptions = XML_STAT_PARSE_NOBLANKS;             /*解析参数*/
    WORD16 wMsgLen = 0;                                           /*OMM消息体长度*/
    WORD32  dwXmlLen = 0;                                         /*OMM消息体XML数据长度*/
    XmlDocPtr ptrDoc = NULL;                                        /*xml文档指针*/
    XmlNode tRootNode;                                               /*根节点结构变量*/
    TYPE_XMLMODEL *ptXmlModel = NULL;                     /*XML模型指针*/

    /*入参判断*/
    if (NULL == ptrMsgBuf)
    {
         XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
         return;
    }
    memset(&tRootNode,0,sizeof(tRootNode));
    /*获取消息长度*/
    XOS_GetMsgDataLen(&wMsgLen);
    if(0 == wMsgLen)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XOS_GetMsgDataLen %d Error!....................",__func__,__LINE__,wMsgLen);        
        return;
    }
    /*申请接收缓冲区内存*/
    paucRecvOMMMsgBuf = (CHAR *)XOS_GetUB(wMsgLen + 1);
    /*申请失败*/
    if (NULL == paucRecvOMMMsgBuf)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XOS_GetUB Error!....................",__func__,__LINE__);        
        return;
    }
    /*保存消息*/
    memset(paucRecvOMMMsgBuf,0,wMsgLen + 1);
    memcpy(paucRecvOMMMsgBuf,ptrMsgBuf,wMsgLen);
    /*保存组件号*/
    memcpy(aucComponentId,paucRecvOMMMsgBuf,2);
    ptXmlModel = (TYPE_XMLMODEL *)paucRecvOMMMsgBuf;
    dwXmlLen = strlen(ptXmlModel->ucXML);
    /*打印接收消息*/
    if (bDebugPrint)
    {
        PrintMsgXml(ptXmlModel, "MSG_OMM_TO_XMLAGNET");
    }
    /*解析XML文档并返回根节点*/
    iRet = XML_ParaseDoc(ptXmlModel->ucXML,dwXmlLen,iOptions,&ptrDoc,&tRootNode);
    if (RETURN_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_ParaseDoc Error,Error code %d! ....................",__func__,__LINE__,iRet);
        OAM_RETUB(paucRecvOMMMsgBuf);
	 return ;
    }
    /*获取节点<Type>的文本*/
    iRet = GetNodeText(&tRootNode, NAME_TYPE_NODE,0,sizeof(aucNodeText) - 1, aucNodeText);
    if (RETURN_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d GetNodeText Error,Error code %d! ....................",__func__,__LINE__,iRet);
        /*释放资源*/
        OAM_RETUB(paucRecvOMMMsgBuf);
        XML_FreeXml(ptrDoc); 
	 return ;
    }
    /*注册链路*/
    if(0 == strncmp(aucNodeText,LINKCFG_NAME_LOGIN,strlen(LINKCFG_NAME_LOGIN)))
    {
        iRet = LinkCfg_LoginProcess(aucComponentId,&tRootNode);
        XOS_SysLog(OAM_CFG_LOG_DEBUG,
                       "....................FUNC:%s LINE:%d LinkCfg_LoginProcess ,code %d! ....................",__func__,__LINE__,iRet);
    }
    /*注销链路*/
    else if(0 == strncmp(aucNodeText,LINKCFG_NAME_LOGOUT,strlen(LINKCFG_NAME_LOGOUT)))
    {
        iRet = LinkCfg_LogoutProcess(aucComponentId,&tRootNode);
        XOS_SysLog(OAM_CFG_LOG_DEBUG,
                       "....................FUNC:%s LINE:%d LinkCfg_LogoutProcess ,code %d! ....................",__func__,__LINE__,iRet);
    }
    /*握手请求*/
    else if(0 == strncmp(aucNodeText,LINKCFG_NAME_HANDSHAKE,strlen(LINKCFG_NAME_HANDSHAKE)))
    {
        iRet = LinkCfg_HandShakeProcess(aucComponentId,&tRootNode);
        XOS_SysLog(OAM_CFG_LOG_DEBUG,
                       "....................FUNC:%s LINE:%d LinkCfg_HandShakeProcess ,code %d! ....................",__func__,__LINE__,iRet);
    }
    else
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d Node Type Text=%s Error! ....................",__func__,__LINE__,aucNodeText);
    }
    /*释放资源*/
    OAM_RETUB(paucRecvOMMMsgBuf);
    XML_FreeXml(ptrDoc); 
    return;
}

/**********************************************************************
* 函数名称：VOID ReceiveCeaseCmdMsgFromOmm
* 功能描述：接收网管端命令终止请求
* 访问的表：无
* 修改的表：无
* 输入参数：LPVOID ptrMsgBuf：命令终止请求
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-12-17             齐龙兆      创建
************************************************************************/
VOID ReceiveCeaseCmdMsgFromOmm(LPVOID ptrMsgBuf)
{
    CHAR aucComponentId[MAX_SIZE_COMPONENTID] = {0};
    CHAR *pucRecvOMMMsgBuf = NULL;                        /*接收网管消息缓冲区*/
    WORD32 dwRet = 0;                                               /*操作状态*/
    INT iOptions = XML_STAT_PARSE_NOBLANKS;             /*解析参数*/
    WORD16 wMsgLen = 0;                                           /*OMM消息体长度*/
    WORD32  dwXmlLen = 0;                                         /*OMM消息体XML数据长度*/
    XmlDocPtr ptOriDoc = NULL;                                     /*原xml文档指针*/
    XmlNode tOriRtNode;                                              /*原文档根节点结构变量*/
    TYPE_XMLMODEL *ptXmlModel = NULL;                     /*XML模型指针*/    
    CHAR *pucORText = "";
    CHAR *pucEOFText = "";
    CHAR *pucRSTRText = "";
    JID tSenderJid = {0};

    /*入参判断*/
    if (NULL == ptrMsgBuf)
    {
         XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
         return;
    }
    /*获取发送端JID*/
    dwRet = XOS_Sender(&tSenderJid);
    if (XOS_SUCCESS != dwRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XOS_Sender Error!%d ....................",__func__,__LINE__,dwRet);
         return;
    }
    memset(&tOriRtNode,0,sizeof(tOriRtNode));
    dwRet = XOS_GetMsgDataLen(&wMsgLen);
    /*获取消息长度失败*/
    if (XOS_SUCCESS != dwRet)
    {
         XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XOS_GetMsgDataLen Error!%d ....................",__func__,__LINE__,dwRet);
         return;
    }
    /*空消息*/
    if(0 == wMsgLen)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XOS_GetMsgDataLen %d Error!....................",__func__,__LINE__,wMsgLen);        
        return;
    }
    pucRecvOMMMsgBuf = (CHAR *)XOS_GetUB(wMsgLen + 1);
    /*申请内存失败*/
    if (NULL == pucRecvOMMMsgBuf)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XOS_GetUB Error!....................",__func__,__LINE__);        
        return;
    }
    /*保存消息*/
    memset(pucRecvOMMMsgBuf,0,wMsgLen + 1);
    memcpy(pucRecvOMMMsgBuf,ptrMsgBuf,wMsgLen);
    memcpy(aucComponentId,pucRecvOMMMsgBuf,MAX_SIZE_COMPONENTID);
    /*获取XML字符流*/
    ptXmlModel = (TYPE_XMLMODEL *)pucRecvOMMMsgBuf;
    dwXmlLen = strlen(ptXmlModel->ucXML);
    /*打印接收消息XML字符流*/
    if (bDebugPrint)
    {        
        PrintMsgXml(ptXmlModel, "MSG_OMM_TO_XMLAGNET");
    }
    dwRet = XML_ParaseDoc(ptXmlModel->ucXML,dwXmlLen,iOptions,&ptOriDoc,&tOriRtNode);
    /*解析XML文档并返回根节点失败*/
    if (RETURN_OK != dwRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_ParaseDoc Error,Error code %d! ....................",__func__,__LINE__,dwRet);
        OAM_RETUB(pucRecvOMMMsgBuf);
	 return ;
    }
    /*处理命令终止*/
    dwRet = ProcCeaseCmd(&tOriRtNode);
    /*<OR>文本*/
    if (RETURN_OK == dwRet)
    {
        pucORText = TEXT_OR_NODE_OK;
    }
    else
    {
        pucORText = TEXT_OR_NODE_NOK;
    }
    /*<EOF>文本*/
    pucEOFText = TEXT_EOF_NODE_Y;
    /*<RSLT>文本*/
    pucRSTRText = GetRSTRText(dwRet);
    if (NULL == pucRSTRText)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                            "....................FUNC:%s LINE:%d GetRSTRText Error! ....................\n",__func__,__LINE__);
        /*释放资源*/
        OAM_RETUB(pucRecvOMMMsgBuf);
        XML_FreeXml(ptOriDoc);
	 return ;
    }
    
    dwRet = ConstructAckMsg2OMM(pucORText, 
                                                    pucEOFText, 
                                                    pucRSTRText,
                                                    (const CHAR *)aucComponentId,
                                                    (const LPXmlNode)&tOriRtNode, 
                                                    EV_XML_CEASECMD_ACK, 
                                                    &tSenderJid);
    /*构造应答消息失败*/
    if (RETURN_OK != dwRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d ConstructAckMsg2OMM %d Error!....................",__func__,__LINE__,dwRet);
	 /*释放资源*/
        OAM_RETUB(pucRecvOMMMsgBuf);
        XML_FreeXml(ptOriDoc);
        return ;
    }
    /*释放资源*/
    OAM_RETUB(pucRecvOMMMsgBuf);
    XML_FreeXml(ptOriDoc);
    return;
}

VOID XML_SetCmdPlanTimer(TYPE_XMLLINKSTATE *ptLinkState)
{
    ptLinkState->wCmdPlanTimer = XOS_SetRelativeTimer(TIMER_CMD_PLAN_XML, DURATION_TIMER_CMD_PLAN_XML, ptLinkState->dwLinkChannel);
    return;
}

VOID XML_KillCmdPlanTimer(TYPE_XMLLINKSTATE *ptLinkState)
{
    if (INVALID_TIMER_ID != ptLinkState->wCmdPlanTimer)
    {
        XOS_KillTimerByTimerId(ptLinkState->wCmdPlanTimer);
        ptLinkState->wCmdPlanTimer = INVALID_TIMER_ID;
    }
    return;
}

VOID XML_RecvPlanInfoFromApp(LPVOID pMsgPara, BOOLEAN bIsSameEndian)
{
    T_Cmd_Plan_Cfg *pPlanInfo =  NULL;
    TYPE_XMLLINKSTATE *ptLinkState = NULL;
    TYPE_XMLLINKSTATE *ptXmlLinkState = NULL;
    WORD16 wLinkStateIndex = 0;
    INT iRet = 0;
    WORD16 wMsgLen = 0;
    CHAR *pOMMMsgBuf = NULL;   /*OMM应答消息体*/
    WORD16 wLoop = 0;
    WORD16 wOmmLinkNum = sizeof(gt_XmlLinkState)/sizeof(TYPE_XMLLINKSTATE);

    pPlanInfo = (T_Cmd_Plan_Cfg *)pMsgPara;
    if (!bIsSameEndian)
    {
        pPlanInfo->wPlanNo = XOS_InvertWord16(pPlanInfo->wPlanNo);
        pPlanInfo->dwCmdID = XOS_InvertWord32(pPlanInfo->dwCmdID);
        pPlanInfo->dwLinkChannel = XOS_InvertWord32(pPlanInfo->dwLinkChannel);
    }

    if (S_CfgSave == XOS_GetCurState())
    {
        /*查找正在存盘的网管端*/
        for(wLoop = 0;wLoop < wOmmLinkNum;wLoop++)
        {
            ptXmlLinkState = &(gt_XmlLinkState[wLoop]);
            if (CMD_DEF_SET_SAVE == ptXmlLinkState->dwCmdId)
            {            
                break;
            }
        }

        if (wLoop < wOmmLinkNum)
            ptLinkState = ptXmlLinkState;
        else
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d No Xml Link To Save! ....................",__func__,__LINE__); 
            return;
        }
    }
    else
    {
        /*根据链路号获取链路状态*/
        wLinkStateIndex = (WORD16)(pPlanInfo->dwLinkChannel&0X000001FF);    
        ptLinkState = Xml_GetLinkStateByIndex(wLinkStateIndex); 
        if (NULL == ptLinkState)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR,
                           "....................FUNC:%s LINE:%d Xml_GetLinkStateByIndex Error! ....................",__func__,__LINE__);  
            return;
        }
    }
    
    XML_KillCmdPlanTimer(ptLinkState);
    XML_SetCmdPlanTimer(ptLinkState);

    Xml_KillExeTimer(ptLinkState);
    Xml_SetExeTimer(ptLinkState);

    if ((pPlanInfo->ucPlanValue == 0) || (pPlanInfo->ucPlanValue >= 100))
        return;
    
    if ((ptLinkState->ucPlanValuePrev > 0) && (ptLinkState->ucPlanValuePrev == pPlanInfo->ucPlanValue))
        return;

    ptLinkState->ucPlanValuePrev = pPlanInfo->ucPlanValue;

    /*申请UB*/
    pOMMMsgBuf = (CHAR *)XOS_GetUB(MAX_SIZE_OMMACKMSG_PLAN);
    if(NULL == pOMMMsgBuf)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XOS_GetUB Error! ....................",__func__,__LINE__);
        return;
    }
    memset(pOMMMsgBuf,0,MAX_SIZE_OMMACKMSG_PLAN);

    iRet = PlanMgt2XmlMgt(pPlanInfo,ptLinkState,&wMsgLen,pOMMMsgBuf);
    if (RETURN_OK != iRet)
    {
        OAM_RETUB(pOMMMsgBuf);
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d OamMgt2XmlMgt Error,code %d! ....................",__func__,__LINE__,iRet);         
        return;
    }
    /*向OMM发送消息*/
    XOS_SendAsynMsg(EV_XML_OMMCFG_ACK,(BYTE *)pOMMMsgBuf,wMsgLen, 0, 0, &(ptLinkState->tDtJid));
    /*释放内存*/   
    OAM_RETUB(pOMMMsgBuf);     
}

VOID XML_DealPlanTimer(TYPE_XMLLINKSTATE *ptLinkState)
{
    INT iRet = 0;
    WORD16 wMsgLen = 0;
    CHAR *pOMMMsgBuf = NULL;   /*OMM应答消息体*/
    T_Cmd_Plan_Cfg CmdPlanInfo = {0};

    XML_KillCmdPlanTimer(ptLinkState);
    XML_SetCmdPlanTimer(ptLinkState);

    CmdPlanInfo.ucPlanValue = ptLinkState->ucPlanValuePrev;
    
    /*申请UB*/
    pOMMMsgBuf = (CHAR *)XOS_GetUB(MAX_SIZE_OMMACKMSG_PLAN);
    if(NULL == pOMMMsgBuf)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XOS_GetUB Error! ....................",__func__,__LINE__);
        return;
    }
    memset(pOMMMsgBuf,0,MAX_SIZE_OMMACKMSG_PLAN);

    iRet = PlanMgt2XmlMgt(&CmdPlanInfo,ptLinkState,&wMsgLen,pOMMMsgBuf);
    if (RETURN_OK != iRet)
    {
        OAM_RETUB(pOMMMsgBuf);
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d OamMgt2XmlMgt Error,code %d! ....................",__func__,__LINE__,iRet);         
        return;
    }
    /*向OMM发送消息*/
    XOS_SendAsynMsg(EV_XML_OMMCFG_ACK,(BYTE *)pOMMMsgBuf,wMsgLen, 0, 0, &(ptLinkState->tDtJid));
    /*释放内存*/   
    OAM_RETUB(pOMMMsgBuf);     
}

/*-----------------------调试函数----------------------------*/
VOID Oam_DbgTestCfgCmd(VOID)
{
    CHAR *pucXmlBuf = NULL;
    CHAR *pucXml = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?><CONF><T><VER>0</VER><NTP>3GPL</NTP><NID>0</NID><TRM>3288860</TRM><TID>65535</TID><SID>0</SID><MID>0</MID></T><D><OT>S</OT><OR>OK</OR><DC>1</DC><EOF>Y</EOF><MD><CMD id=\"0x50020\" optype=\"S\"></CMD></MD></D></CONF>";
    WORD16 wMsgLen = 0;
    gt_XmlLinkState[0].dwLinkChannel = 3288860;
    gt_XmlLinkState[0].bUsed = XML_LINKSTATE_USED;
    /*分配最近使用OAM结构体空间*/
    if (NULL != gt_XmlLinkState[0].ptLastSendMsg)
    {
        OAM_RETUB(gt_XmlLinkState[0].ptLastSendMsg);
    }
    gt_XmlLinkState[0].ptLastSendMsg = (MSG_COMM_OAM *)XOS_GetUB(MAX_OAMMSG_LEN);
    memset(gt_XmlLinkState[0].ptLastSendMsg,0,MAX_OAMMSG_LEN);

    wMsgLen = strlen(pucXml) + 2 + 1;
    pucXmlBuf = (CHAR *)XOS_GetUB(wMsgLen);
    if (NULL == pucXmlBuf)
    {
        printf("XOS_GetUB Error!\n");
    return;
}
    memset(pucXmlBuf,0,wMsgLen);
    memcpy(pucXmlBuf+2,pucXml,strlen(pucXml));

    OSS_SendFromTask(EV_XML_CEASECMD_REQ,
                                    (BYTE *)pucXmlBuf,
                                    wMsgLen,
                                    NULL,
                                    &g_ptOamIntVar->jidSelf);
    XOS_RetUB(pucXmlBuf);
}

VOID Test_XML_RemoveNode()
{
    XML_STAT dwRet = XML_OK;
    XmlDocPtr pDoc = NULL;
    XmlNode   tRootNode;
    XmlNode   tTmpParentNode;
    XmlNode    tChildNodeTmp;
    WORD16 wTmpLen;
	
    CHAR aucTmpResult[400] = {0};
    CHAR aucResult[400] = {0};
	
    CHAR *pcXml = "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
"<nodes>"
"<Modes>"
"<node name=\"Mode1\" multinode=\"1\"/>"
"<node name=\"Mode2\" multinode=\"1\"/>"
"<node name=\"Mode3\" multinode=\"1\"/>"
"</Modes>"
"<Types>"
"<node name=\"Type1\" multinode=\"1\"/>"
"<node name=\"Type2\" multinode=\"1\"/>"
"<node name=\"Type3\" multinode=\"1\"/>"
"</Types>"
"</nodes>";

    /*读取上面的xml*/
    dwRet = XML_LoadXml(pcXml,strlen(pcXml)+1,XML_STAT_PARSE_NOBLANKS, &pDoc);
    if (XML_OK != dwRet)
    {
        printf("Test_Xml_RemoveChildNode::XML_LoadXml function error!%d ########\n",dwRet);
        return;
    }

    /*获取根节点*/
    dwRet = XML_GetRootNode(pDoc, &tRootNode);
    if (XML_OK != dwRet)
    {
        printf("Test_Xml_RemoveChildNode::XML_GetRootNode function error!%d ########\n",dwRet);
        XML_FreeXml(pDoc);
        return;
    }

    /*获取Modes子节点*/
    dwRet = XML_GetChild(&tRootNode, 0, &tTmpParentNode);
    if (XML_OK != dwRet)
    {
        printf("Test_Xml_RemoveChildNode::XML_GetChild function error!%d ########\n",dwRet);
        XML_FreeXml(pDoc);
        return;
    }

    /*获取Modes节点的一个子节点*/
    dwRet = XML_GetChild(&tTmpParentNode, 1, &tChildNodeTmp);
    if (XML_OK != dwRet)
    {
        printf("Test_Xml_RemoveChildNode::XML_GetChild function error!%d ########\n",dwRet);
        XML_FreeXml(pDoc);
        return;
    }
    
    /*删除Modes的中间一个子节点*/
    dwRet =  XML_RemoveNode(&tChildNodeTmp);
    if (XML_OK != dwRet)
    {
        printf("Test_Xml_RemoveChildNode::Xml_RemoveChildNode function error!%d ########\n",dwRet);
        XML_FreeXml(pDoc);
        assert(0);
        return;
    }

    /*获取xml 文本*/
    memset(aucResult, 0, sizeof(aucResult));
    dwRet = XML_GetText(pDoc, aucResult, sizeof(aucResult), XML_STAT_PARSE_NOBLANKS, &wTmpLen);
    if (XML_OK != dwRet)
    {
        printf("Test_Xml_RemoveChildNode::XML_GetChild function error!%d ########\n",dwRet);
        XML_FreeXml(pDoc);
        return;
    }

    strcpy(aucTmpResult, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<nodes>"
"<Modes>"
"<node name=\"Mode1\" multinode=\"1\"/>"
"<node name=\"Mode3\" multinode=\"1\"/>"
"</Modes>"
"<Types>"
"<node name=\"Type1\" multinode=\"1\"/>"
"<node name=\"Type2\" multinode=\"1\"/>"
"<node name=\"Type3\" multinode=\"1\"/>"
"</Types>"
"</nodes>\n");

    assert(strcmp(aucResult, aucTmpResult) == 0);
    
    /*删除Modes的第一个子节点*/
    /*获取Modes节点的一个子节点*/
    dwRet = XML_GetChild(&tTmpParentNode, 0, &tChildNodeTmp);
    if (XML_OK != dwRet)
    {
        printf("Test_Xml_RemoveChildNode::XML_GetChild function error!%d ########\n",dwRet);
        XML_FreeXml(pDoc);
        return;
    }
    dwRet =  XML_RemoveNode(&tChildNodeTmp);
    if (XML_OK != dwRet)
    {
        printf("Test_Xml_RemoveChildNode::Xml_RemoveChildNode function error!%d ########\n",dwRet);
        XML_FreeXml(pDoc);
        assert(0);
        return;
    }

    /*获取xml 文本*/
    memset(aucResult, 0, sizeof(aucResult));
    dwRet = XML_GetText(pDoc, aucResult, sizeof(aucResult), XML_STAT_PARSE_NOBLANKS, &wTmpLen);
    if (XML_OK != dwRet)
    {
        printf("Test_Xml_RemoveChildNode::XML_GetChild function error!%d ########\n",dwRet);
        XML_FreeXml(pDoc);
        return;
    }

    strcpy(aucTmpResult, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<nodes>"
"<Modes>"
"<node name=\"Mode3\" multinode=\"1\"/>"
"</Modes>"
"<Types>"
"<node name=\"Type1\" multinode=\"1\"/>"
"<node name=\"Type2\" multinode=\"1\"/>"
"<node name=\"Type3\" multinode=\"1\"/>"
"</Types>"
"</nodes>\n");

    assert(strcmp(aucResult, aucTmpResult) == 0);


    /*获取邋Types子节点*/
    dwRet = XML_GetChild(&tRootNode, 1, &tTmpParentNode);
    if (XML_OK != dwRet)
    {
        printf("Test_Xml_RemoveChildNode::XML_GetChild function error!%d ########\n",dwRet);
        XML_FreeXml(pDoc);
        return;
    }
	
    /*删除Types的第一个子节点*/
    /*获取Types节点的一个子节点*/
    dwRet = XML_GetChild(&tTmpParentNode, 2, &tChildNodeTmp);
    if (XML_OK != dwRet)
    {
        printf("Test_Xml_RemoveChildNode::XML_GetChild function error!%d ########\n",dwRet);
        XML_FreeXml(pDoc);
        return;
    }
    dwRet =  XML_RemoveNode(&tChildNodeTmp);
    if (XML_OK != dwRet)
    {
        printf("Test_Xml_RemoveChildNode::Xml_RemoveChildNode function error!%d ########\n",dwRet);
        XML_FreeXml(pDoc);
        assert(0);
        return;
    }

    /*获取xml 文本*/
    memset(aucResult, 0, sizeof(aucResult));
    dwRet = XML_GetText(pDoc, aucResult, sizeof(aucResult), XML_STAT_PARSE_NOBLANKS, &wTmpLen);
    if (XML_OK != dwRet)
    {
        printf("Test_Xml_RemoveChildNode::XML_GetChild function error!%d ########\n",dwRet);
        XML_FreeXml(pDoc);
        return;
    }

    strcpy(aucTmpResult, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<nodes>"
"<Modes>"
"<node name=\"Mode3\" multinode=\"1\"/>"
"</Modes>"
"<Types>"
"<node name=\"Type1\" multinode=\"1\"/>"
"<node name=\"Type2\" multinode=\"1\"/>"
"</Types>"
"</nodes>\n");

    assert(strcmp(aucResult, aucTmpResult) == 0);
		
    /*删除Types节点及其子节点*/
    /*获取Types节点*/
    dwRet = XML_GetChild(&tRootNode, 1, &tChildNodeTmp);
    if (XML_OK != dwRet)
    {
        printf("Test_Xml_RemoveChildNode::XML_GetChild function error!%d ########\n",dwRet);
        XML_FreeXml(pDoc);
        return;
    }
    dwRet = XML_RemoveNode(&tChildNodeTmp);
    if (XML_OK != dwRet)
    {
        printf("Test_Xml_RemoveChildNode::Xml_RemoveChildNode function error!%d ########\n",dwRet);
        XML_FreeXml(pDoc);
        assert(0);
        return;
    }

    /*获取xml 文本*/
    memset(aucResult, 0, sizeof(aucResult));
    dwRet = XML_GetText(pDoc, aucResult, sizeof(aucResult), XML_STAT_PARSE_NOBLANKS, &wTmpLen);
    if (XML_OK != dwRet)
    {
        printf("Test_Xml_RemoveChildNode::XML_GetChild function error!%d ########\n",dwRet);
        XML_FreeXml(pDoc);
        return;
    }

    strcpy(aucTmpResult, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<nodes>"
"<Modes>"
"<node name=\"Mode3\" multinode=\"1\"/>"
"</Modes>"
"</nodes>\n");

    assert(strcmp(aucResult, aucTmpResult) == 0);
	
    dwRet= XML_FreeXml(pDoc);
    if (XML_OK != dwRet)
    {
        printf("Test_Xml_RemoveChildNode::XML_FreeXml function error!%d########\n",dwRet);
    }
}

VOID Test_XML_RemoveAttribute()
{
    XML_STAT dwRet = XML_OK;
    XmlDocPtr pDoc = NULL;
    XmlNode   tRootNode;
    XmlNode   tTmpParentNode;
    XmlNode    tChildNodeTmp;
    WORD16 wTmpLen;
    XmlAttr     tAttrTmp;

    CHAR aucTmpResult[400] = {0};
    CHAR aucResult[400] = {0};
	
    CHAR *pcXml = "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
"<nodes>"
"<Modes>"
"<node name=\"Mode1\" multinode=\"1\"/>"
"<node name=\"Mode2\" multinode=\"1\"/>"
"<node name=\"Mode3\" multinode=\"1\"/>"
"</Modes>"
"<Types>"
"<node name=\"Type1\" multinode=\"1\"/>"
"<node name=\"Type2\" multinode=\"1\"/>"
"<node name=\"Type3\" multinode=\"1\"/>"
"</Types>"
"</nodes>";
	
    /*读取上面的xml*/
    dwRet = XML_LoadXml(pcXml,strlen(pcXml)+1,XML_STAT_PARSE_NOBLANKS, &pDoc);
    if (XML_OK != dwRet)
    {
        printf("Test_Xml_RemoveChildNode::XML_LoadXml function error!%d ########\n",dwRet);
        return;
    }

    /*获取根节点*/
    dwRet = XML_GetRootNode(pDoc, &tRootNode);
    if (XML_OK != dwRet)
    {
        printf("Test_Xml_RemoveChildNode::XML_GetRootNode function error!%d ########\n",dwRet);
        XML_FreeXml(pDoc);
        return;
    }

    /*获取Modes子节点*/
    dwRet = XML_GetChild(&tRootNode, 0, &tTmpParentNode);
    if (XML_OK != dwRet)
    {
        printf("Test_Xml_RemoveChildNode::XML_GetChild function error!%d ########\n",dwRet);
        XML_FreeXml(pDoc);
        return;
    }

    /*获取Modes节点的一个子节点*/
    dwRet = XML_GetChild(&tTmpParentNode, 0, &tChildNodeTmp);
    if (XML_OK != dwRet)
    {
        printf("Test_Xml_RemoveChildNode::XML_GetChild function error!%d ########\n",dwRet);
        XML_FreeXml(pDoc);
        return;
    }

    /*删除modes第一个子节点的name属性*/
    dwRet = XML_GetAttributeByName(&tChildNodeTmp, (BYTE *)"name", &tAttrTmp);
    assert(XML_OK == dwRet);
    XML_RemoveAttribute(&tAttrTmp);
    dwRet = XML_GetAttributeByName(&tChildNodeTmp, (BYTE *)"name", &tAttrTmp);
    assert(XML_ELEMENT_NOT_EXIST == dwRet);

     /*获取xml 文本*/
    memset(aucResult, 0, sizeof(aucResult));
    dwRet = XML_GetText(pDoc, aucResult, sizeof(aucResult), XML_STAT_PARSE_NOBLANKS, &wTmpLen);
    if (XML_OK != dwRet)
    {
        printf("Test_Xml_RemoveChildNode::XML_GetChild function error!%d ########\n",dwRet);
        XML_FreeXml(pDoc);
        return;
    }

    strcpy(aucTmpResult, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<nodes>"
"<Modes>"
"<node multinode=\"1\"/>"
"<node name=\"Mode2\" multinode=\"1\"/>"
"<node name=\"Mode3\" multinode=\"1\"/>"
"</Modes>"
"<Types>"
"<node name=\"Type1\" multinode=\"1\"/>"
"<node name=\"Type2\" multinode=\"1\"/>"
"<node name=\"Type3\" multinode=\"1\"/>"
"</Types>"
"</nodes>\n");

    assert(strcmp(aucResult, aucTmpResult) == 0);
	
    /*删除modes第一个个子节点的multinode属性*/
    dwRet = XML_GetAttributeByName(&tChildNodeTmp, (BYTE *)"multinode", &tAttrTmp);
    assert(XML_OK == dwRet);
    XML_RemoveAttribute(&tAttrTmp);
    dwRet = XML_GetAttributeByName(&tChildNodeTmp, (BYTE *)"multinode", &tAttrTmp);
    assert(XML_ELEMENT_NOT_EXIST == dwRet);

 /*获取xml 文本*/
    memset(aucResult, 0, sizeof(aucResult));
    dwRet = XML_GetText(pDoc, aucResult, sizeof(aucResult), XML_STAT_PARSE_NOBLANKS, &wTmpLen);
    if (XML_OK != dwRet)
    {
        printf("Test_Xml_RemoveChildNode::XML_GetChild function error!%d ########\n",dwRet);
        XML_FreeXml(pDoc);
        return;
    }

    strcpy(aucTmpResult, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<nodes>"
"<Modes>"
"<node/>"
"<node name=\"Mode2\" multinode=\"1\"/>"
"<node name=\"Mode3\" multinode=\"1\"/>"
"</Modes>"
"<Types>"
"<node name=\"Type1\" multinode=\"1\"/>"
"<node name=\"Type2\" multinode=\"1\"/>"
"<node name=\"Type3\" multinode=\"1\"/>"
"</Types>"
"</nodes>\n");

    assert(strcmp(aucResult, aucTmpResult) == 0);
	
    /*获取Modes节点的二个子节点*/
    dwRet = XML_GetChild(&tTmpParentNode, 1, &tChildNodeTmp);
    if (XML_OK != dwRet)
    {
        printf("Test_Xml_RemoveChildNode::XML_GetChild function error!%d ########\n",dwRet);
        XML_FreeXml(pDoc);
        return;
    }
    /*删除modes第二个个子节点的multinode属性*/
    dwRet = XML_GetAttributeByName(&tChildNodeTmp, (BYTE *)"multinode", &tAttrTmp);
    assert(XML_OK == dwRet);
    XML_RemoveAttribute(&tAttrTmp);
    dwRet = XML_GetAttributeByName(&tChildNodeTmp, (BYTE *)"multinode", &tAttrTmp);
    assert(XML_ELEMENT_NOT_EXIST == dwRet);
		
     /*获取xml 文本*/
    memset(aucResult, 0, sizeof(aucResult));
    dwRet = XML_GetText(pDoc, aucResult, sizeof(aucResult), XML_STAT_PARSE_NOBLANKS, &wTmpLen);
    if (XML_OK != dwRet)
    {
        printf("Test_Xml_RemoveChildNode::XML_GetChild function error!%d ########\n",dwRet);
        XML_FreeXml(pDoc);
        return;
    }

    strcpy(aucTmpResult, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<nodes>"
"<Modes>"
"<node/>"
"<node name=\"Mode2\"/>"
"<node name=\"Mode3\" multinode=\"1\"/>"
"</Modes>"
"<Types>"
"<node name=\"Type1\" multinode=\"1\"/>"
"<node name=\"Type2\" multinode=\"1\"/>"
"<node name=\"Type3\" multinode=\"1\"/>"
"</Types>"
"</nodes>\n");

    assert(strcmp(aucResult, aucTmpResult) == 0);
	
    dwRet= XML_FreeXml(pDoc);
    if (XML_OK != dwRet)
    {
        printf("Test_Xml_RemoveChildNode::XML_FreeXml function error!%d########\n",dwRet);
    }
}

VOID Test_XML_XPath()
{
    XML_STAT dwRet = XML_OK;
    XmlDocPtr pDoc = NULL;
    XmlXPathObjectPtr xmlXPathobj;	
    WORD32 dwCount=0;
    XmlNode tnode;
    XmlAttr tAttrNode;		
    CHAR *pcXml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
    <root>\
    <Node2 attribute=\"no\">ddd</Node2>\
    <Node2 attribute=\"yes\">eee</Node2>\
    <Node2 attribute=\"zte\">fff</Node2>\
    <Node3>content changed</Node3>\
    <Node4 >three</Node4>\
    <Node5 />\
    </root>";
    /* CHAR * pXPath ="/root/Node2[@attribute=\"yes\"]"; */
    CHAR * pXPath ="/root/Node2"; 	
    int i=0;	
    CHAR aucNodeName[20] = "Node2";	
    CHAR aucAttributeValue[3][20] = {"no","yes","zte"};		
    CHAR aucAttributename[20] = "attribute";		
    /*读取上面的xml*/
    dwRet = XML_LoadXml(pcXml,strlen(pcXml)+1,XML_STAT_PARSE_NOBLANKS, &pDoc);
    if (XML_OK != dwRet)
    {
        printf("Test_XML_XPath::XML_LoadXml function error!%d ########\n",dwRet);
        return;
    }

    /*获取XPATH节点集*/	
    dwRet = XML_GetXpathNodeSet(pDoc,pXPath,&xmlXPathobj);
    if (XML_OK != dwRet)
    {
        printf("Test_XML_XPath::XML_GetXpathNodeSet function error!%d ########\n",dwRet);
        XML_FreeXml(pDoc);
        return;
    }
    /*结点集 元素的个数*/
    if(XML_GetNodeSetNumOfXPathObject(xmlXPathobj,&dwCount) !=XML_OK)
    {
        printf("XML_GetNodeSetNumOfXPathObject func err\n");
    }
    assert(3 == dwCount);	
    for(i=0;i<dwCount;i++)
    {
        memset(&tnode,0,sizeof(tnode));
        memset(&tAttrNode,0,sizeof(tAttrNode));		
        if(XML_GetXmlNodeFromXPathObject(xmlXPathobj,i,&tnode) !=XML_OK)
        {
            printf("Get Node error\n");
	     break;
        }	
        assert(strcmp(aucNodeName, (CHAR *)tnode.name) == 0);		
        dwRet = XML_GetAttribute(&tnode,0,&tAttrNode);
        if (XML_OK != dwRet)
        {
            printf("########Test_XML_XPath::XML_GetAttribute function error!%d########\n",dwRet);
            break;
        } 
        assert(strcmp(aucAttributename, (CHAR *)tAttrNode.name) == 0);	
        assert(strcmp(aucAttributeValue[i], (CHAR *)tAttrNode.value) == 0);							
    }
    dwRet=XML_FreeXml(pDoc);	
    if (XML_OK != dwRet)
    {
        printf("########tTest_XML_XPath::XML_FreeXml function error!%d########\n",dwRet);
        return;
    }	
    dwRet=XML_FreeXpathObject(xmlXPathobj);	
    if (XML_OK != dwRet)
    {
        printf("########tTest_XML_XPath::XML_FreeXpathObject function error!%d########\n",dwRet);
        return;
    }	
}
VOID Oam_DbgTestNewXml(VOID)
{
    CHAR achTest[]="<hbl><begtime>2010-01-30&amp; 16:&lt;12:14</begtime></hbl>\n";
    CHAR achVal[1024];
    XmlDocPtr  ptDoc;
    XmlNode    tRoot,tTemp;
    WORD16 len;

    XML_LoadXml(achTest,sizeof(achTest),XML_STAT_PARSE_NOBLANKS,&ptDoc);
    XML_GetRootNode(ptDoc,&tRoot);
    XML_GetChild(&tRoot, 0, &tTemp);
    XML_GetChildText(&tTemp, achVal,1024, &len);
    printf("achVal1=%s\n",achVal);
    /* achVal 解析后输出：2010-01-30& 16:<12:14 把&amp;解析为&符号 */

    memset(achVal,0,sizeof(achVal));
    XML_NewDoc("testxml", &ptDoc);
    XML_GetRootNode(ptDoc, &tRoot);
    XML_InsertChild(&tRoot,0,"t2",&tTemp);
    XML_SetChildText(&tTemp,"this <is >a\" &test node!");
    XML_GetText(ptDoc, achVal, 1024, XML_STAT_SAVE_NO_EMPTY, &len);
    printf("achVal2=%s\n",achVal);
    /* achVal 编码后输出：<?xml version="1.0"?>
    <testxml><t2>this &lt;is &gt;a </t2></testxml>    左右括号转义正确，&及其后面的字符被丢弃掉了 */
    XML_FreeXml(ptDoc);	
    return;
}
VOID Oam_DbgTestXmlInterface(VOID)
{
    XML_STAT dwRet = XML_OK;
    XmlDocPtr pDoc = NULL;
    XmlNode tRootNode,tChildNode;
    LPXmlNode ptCurNode = NULL;
    XmlAttr tAttrNode;
    CHAR aucTextBuf[200] = {0};
    CHAR aucValue[200] = {0};
    CHAR *pucDocBuf = NULL;
    WORD16 wActualLen = 0;
    WORD32 dwDocBufLen = 200;
    WORD32 dwCount = 0;
    WORD32 dwIndex = 0;
    WORD32 dwValue = 0;
    WORD16 wValue = 0;
    WORD32 dwTestIndex = 0;
    BYTE ucValue = 0;

    Test_XML_RemoveNode();
    Test_XML_RemoveAttribute();
    Test_XML_XPath();

    printf("\n\n\n");
    printf("###############################################\n");
    printf("#########Welcome to test xml interface#########\n");
    printf("###############################################\n");

    while(1)
    {
        memset(&tRootNode,0,sizeof(tRootNode));
        memset(&tChildNode,0,sizeof(tChildNode));
        memset(&tAttrNode,0,sizeof(tAttrNode));
        memset(aucTextBuf,0,sizeof(aucTextBuf));
        memset(aucValue,0,sizeof(aucValue));
        
        printf("########pls input your test index########\n");
        printf("help:1 exit:0\n");
        XOS_ReadLine(aucTextBuf,sizeof(aucTextBuf));
        Convert_atoi(aucTextBuf,&dwTestIndex);
        memset(aucTextBuf,0,sizeof(aucTextBuf));
        printf("\n\n\n");
        if (0 == dwTestIndex)
        {
            break;
        }
        printf("test index:%d\n",dwTestIndex);
        switch(dwTestIndex)
        {
            case 1:
            {
                printf(" 2--XML_LoadXml\n");
                printf(" 3--XML_GetText\n");
                printf(" 4--XML_GetRootNode\n");
                printf(" 5--XML_GetChildCount\n");
                printf(" 6--XML_GetChild\n");
                printf(" 7--XML_InsertChild\n");
                printf(" 8--XML_GetAttributeCount\n");
                printf(" 9--XML_GetAttribute\n");
                printf("10--XML_SetAttribute\n");
                printf("11--XML_GetChildText\n");
                printf("12--XML_SetChildText\n");
                printf("13--XML_SaveXML\n");
                printf("14--XML_NewDoc\n");
                printf("15--XML_GetAttribute_AsWORD32\n");
                printf("16--XML_GetAttribute_AsWORD16\n");
                printf("17--XML_GetAttribute_AsBYTE\n");
                printf("18--XML_GetAttributeValueByName\n");
                printf("19--XML_FreeXml\n");
                printf("20--Get Current Node Name\n");
                printf("21--XML_ReadFile\n");
                break;
            }
            case 2:/*XML_LoadXml*/
            {
                dwDocBufLen = 200;
                printf("########test XML_LoadXml function now!########\n");
                pucDocBuf = (CHAR *)XOS_GetUB(dwDocBufLen);
                if (NULL == pucDocBuf)
                {
                    printf("########XOS_GetUB error!########\n");
                    break;
                }
                memset(pucDocBuf,0,dwDocBufLen);
                printf("pls input xml doc text:\n");
                XOS_ReadLine(pucDocBuf,dwDocBufLen);
                dwRet = XML_LoadXml(pucDocBuf,strlen(pucDocBuf)+1,XML_STAT_PARSE_NOBLANKS,&pDoc);
                if (XML_OK != dwRet)
                {
                    printf("########test XML_LoadXml function error!%d ########\n",dwRet);
                    XOS_RetUB(pucDocBuf);
                    break;
                }
                XOS_RetUB(pucDocBuf);
                printf("########test XML_LoadXml function ok!########\n");
                break;
            }
            case 3:/*XML_GetText*/
            {
                dwDocBufLen = 10 * 1024;
                printf("########test XML_GetText function now!########\n");
                pucDocBuf = (CHAR *)XOS_GetUB(dwDocBufLen);
                if (NULL == pucDocBuf)
                {
                    printf("########XOS_GetUB error!########\n");
                    break;
                }
                memset(pucDocBuf,0,dwDocBufLen);
                dwRet = XML_GetText(pDoc,pucDocBuf,dwDocBufLen,XML_STAT_PARSE_NOBLANKS,&wActualLen);
                if (XML_OK != dwRet)
                {
                    printf("########test XML_GetText function error!%d########\n",dwRet);
                    XOS_RetUB(pucDocBuf);
                    break;
                }
                printf("%s\n",pucDocBuf);
                XOS_RetUB(pucDocBuf);
                printf("########test XML_LoadXml function ok!########\n");
                break;
            }
            case 4:/*XML_GetRootNode*/
            {
                printf("########test XML_GetRootNode function now!########\n");
                dwRet = XML_GetRootNode(pDoc,&tRootNode);
                if (XML_OK != dwRet)
                {
                    printf("########test XML_GetRootNode function error!%d########\n",dwRet);
                    break;
                }
                printf("%s\n",tRootNode.name);
                ptCurNode = &tRootNode;
                printf("########test XML_GetRootNode function ok!########\n");
                break;
            }
            case 5:/*XML_GetChildCount*/
            {
                printf("########test XML_GetChildCount function now!########\n");
                dwCount = XML_GetChildCount(ptCurNode);
                printf("%d\n",dwCount);
                printf("########test XML_GetChildCount function ok!########\n");
                break;
            }
            case 6:/*XML_GetChild*/
            {
                printf("########test XML_GetChild function now!########\n");
                printf("pls input child index(WORD32):\n");
                XOS_ReadLine(aucTextBuf,sizeof(aucTextBuf)); 
                Convert_atoi(aucTextBuf,&dwIndex);
                dwRet = XML_GetChild(ptCurNode,dwIndex,&tChildNode);
                if (XML_OK != dwRet)
                {
                    printf("########test XML_GetChild function error!%d########\n",dwRet);
                    break;
                }
                printf("%s\n",tChildNode.name);
                ptCurNode = &tChildNode;
                printf("########test XML_GetChild function ok!########\n");
                break;
            }
            case 7:/*XML_InsertChild*/
            {
                printf("########test XML_InsertChild function now!########\n");
                printf("pls input child node index(WORD32):\n");
                XOS_ReadLine(aucTextBuf,sizeof(aucTextBuf)); 
                Convert_atoi(aucTextBuf,&dwIndex);
                printf("pls input child node name:\n");
                memset(aucTextBuf,0,sizeof(aucTextBuf));
                XOS_ReadLine(aucTextBuf,sizeof(aucTextBuf)); 
                dwRet = XML_InsertChild(ptCurNode,dwIndex,aucTextBuf,&tChildNode);
                if (XML_OK != dwRet)
                {
                    printf("########test XML_InsertChild function error!%d########\n",dwRet);
                    break;
                }
                printf("%s\n",tChildNode.name);
                printf("########test XML_InsertChild function ok!########\n");
                break;
            }
            case 8:/*XML_GetAttributeCount*/
            {
                printf("########test XML_GetAttributeCount function now!########\n");
                dwCount= XML_GetAttributeCount(ptCurNode);
                printf("%d\n",dwCount);
                printf("########test XML_GetAttributeCount function ok!########\n");
                break;
            }
            case 9:/*XML_GetAttribute*/
            {
                printf("########test XML_GetAttribute function now!########\n");
                printf("pls input attribute node index(WORD32):\n");
                XOS_ReadLine(aucTextBuf,sizeof(aucTextBuf)); 
                Convert_atoi(aucTextBuf,&dwIndex);
                dwRet = XML_GetAttribute(ptCurNode,dwIndex,&tAttrNode);
                if (XML_OK != dwRet)
                {
                    printf("########test XML_GetAttribute function error!%d########\n",dwRet);
                    break;
                }
                printf("name:%s value:%s\n",tAttrNode.name,tAttrNode.value);
                printf("########test XML_GetAttribute function ok!########\n");
                break;
            }
            case 10:/*XML_SetAttribute*/
            {
                printf("########test XML_SetAttribute function now!########\n");
                printf("pls input attribute node name:\n");
                XOS_ReadLine(aucTextBuf,sizeof(aucTextBuf)); 
                printf("pls input attribute node value:\n");
                XOS_ReadLine(aucValue,sizeof(aucValue));
                dwRet = XML_SetAttribute(ptCurNode,aucTextBuf,aucValue);
                if (XML_OK != dwRet)
                {
                    printf("########test XML_SetAttribute function error!%d########\n",dwRet);
                    break;
                }
                printf("########test XML_SetAttribute function ok!########\n");
                break;
            }
            case 11:/*XML_GetChildText*/
            {
                printf("########test XML_GetChildText function now!########\n");
                dwRet = XML_GetChildText(ptCurNode,aucTextBuf,sizeof(aucTextBuf),&wActualLen);
                if (XML_OK != dwRet)
                {
                    printf("########test XML_GetChildText function error!%d########\n",dwRet);
                    break;
                }
                printf("%s\n",aucTextBuf);
                printf("########test XML_GetChildText function ok!########\n");
                break;
            }
            case 12:/*XML_SetChildText*/
            {
                printf("########test XML_SetChildText function now!########\n");
                printf("pls input child node text:\n");
                XOS_ReadLine(aucTextBuf,sizeof(aucTextBuf)); 
                dwRet = XML_SetChildText(ptCurNode,aucTextBuf);
                if (XML_OK != dwRet)
                {
                    printf("########test XML_SetChildText function error!%d########\n",dwRet);
                    break;
                }
                printf("%s\n",aucTextBuf);
                printf("########test XML_SetChildText function ok!########\n");
                break;
            }
            case 13:/*XML_SaveXML*/
            {
                printf("########test XML_SaveXML function now!########\n");
                printf("pls input file name(path+filename):\n");
                XOS_ReadLine(aucTextBuf,sizeof(aucTextBuf)); 
                dwRet = XML_SaveXML(pDoc,aucTextBuf);
                if (XML_OK != dwRet)
                {
                    printf("########test XML_SaveXML function error!%d########\n",dwRet);
                    break;
                }
                printf("########test XML_SaveXML function ok!########\n");
                break;
            }
            case 14:/*XML_NewDoc*/
            {
                printf("########test XML_NewDoc function now!########\n");
                printf("pls input root node name:\n");
                XOS_ReadLine(aucTextBuf,sizeof(aucTextBuf)); 
                dwRet = XML_NewDoc(aucTextBuf,&pDoc);
                if (XML_OK != dwRet)
                {
                    printf("########test XML_NewDoc function error!%d########\n",dwRet);
                    break;
                }
                printf("########test XML_NewDoc function ok!########\n");
                break;
            }
            case 15:/*XML_GetAttribute_AsWORD32*/
            {
                printf("########test XML_GetAttribute_AsWORD32 function now!########\n");
                printf("pls input attribute name:\n");
                XOS_ReadLine(aucTextBuf,sizeof(aucTextBuf)); 
                dwRet = XML_GetAttribute_AsWORD32(ptCurNode,aucTextBuf,&dwValue);
                if (XML_OK != dwRet)
                {
                    printf("########test XML_GetAttribute_AsWORD32 function error!%d########\n",dwRet);
                    break;
                }
                printf("%lu\n",dwValue);
                printf("########test XML_GetAttribute_AsWORD32 function ok!########\n");
                break;
            }
            case 16:/*XML_GetAttribute_AsWORD16*/
            {
                printf("########test XML_GetAttribute_AsWORD16 function now!########\n");
                printf("pls input attribute name:\n");
                XOS_ReadLine(aucTextBuf,sizeof(aucTextBuf)); 
                dwRet= XML_GetAttribute_AsWORD16(ptCurNode,aucTextBuf,&wValue);
                if (XML_OK != dwRet)
                {
                    printf("########test XML_GetAttribute_AsWORD16 function error!%d########\n",dwRet);
                    break;
                }
                printf("%d\n",wValue);
                printf("########test XML_GetAttribute_AsWORD16 function ok!########\n");
                break;
            }
            case 17:/*XML_GetAttribute_AsBYTE*/
            {
                printf("########test XML_GetAttribute_AsBYTE function now!########\n");
                printf("pls input attribute name:\n");
                XOS_ReadLine(aucTextBuf,sizeof(aucTextBuf)); 
                dwRet= XML_GetAttribute_AsBYTE(ptCurNode,aucTextBuf,&ucValue);
                if (XML_OK != dwRet)
                {
                    printf("########test XML_GetAttribute_AsBYTE function error!%d########\n",dwRet);
                    break;
                }
                printf("%d\n",ucValue);
                printf("########test XML_GetAttribute_AsBYTE function ok!########\n");
                break;
            }
            case 18:/*XML_GetAttributeValueByName*/
            {
                printf("########test XML_GetAttributeValueByName function now!########\n");
                printf("pls input attribute name:\n");
                XOS_ReadLine(aucTextBuf,sizeof(aucTextBuf)); 
                dwRet= XML_GetAttributeValueByName(ptCurNode,aucTextBuf,sizeof(aucValue),aucValue,&wActualLen);
                if (XML_OK != dwRet)
                {
                    printf("########test XML_GetAttributeValueByName function error!%d########\n",dwRet);
                    break;
                }
                printf("%d\n",ucValue);
                printf("########test XML_GetAttributeValueByName function ok!########\n");
                break;
            }
            case 19:/*XML_FreeXml*/
            {
                printf("########test XML_FreeXml function now!########\n");
                dwRet= XML_FreeXml(pDoc);
                if (XML_OK != dwRet)
                {
                    printf("########test XML_FreeXml function error!%d########\n",dwRet);
                    break;
                }
                printf("########test XML_FreeXml function ok!########\n");
                break;
            }
            case 20:/*Get Current Node Name*/
            {
                printf("Current Node Name:%s\n",ptCurNode->name);
                break;
            }
            case 21:/*XML_ReadFile*/
            {
                printf("########test XML_ReadFile function now!########\n");
                printf("pls input file name:\n");
                XOS_ReadLine(aucTextBuf,sizeof(aucTextBuf)); 
                dwRet = XML_ReadFile(aucTextBuf,XML_STAT_PARSE_NOBLANKS,&pDoc);
                if (XML_OK != dwRet)
                {
                    printf("########test XML_ReadFile function error!%d########\n",dwRet);
                    break;
                }
                printf("########test XML_ReadFile function ok!########\n");
                break;
            }
            default:
            {
                printf("########test index :%d error,pls input again!########\n",dwTestIndex);
                break;
            }
        }
    }
}
#endif

