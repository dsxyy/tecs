#if 0
/*********************************************************************
* ��Ȩ���� (C)2007, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ� xmlagent.c
* �ļ���ʶ�� 
* ����˵���� XML������ģ�� 
* ��ǰ�汾�� V1.0
* ��    �ߣ� 
* ������ڣ� 
*
* �޸ļ�¼1��
*    �޸����ڣ�2008��9��10��
*    �� �� �ţ�V1.0
*    �� �� �ˣ�
*    �޸����ݣ�����
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
*                            ����                                        *
**************************************************************************/


/**************************************************************************
*                            ��                                          *
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
#define XML_AGENT_OK                                          (WORD16)0  /*�����ɹ�*/
#define XML_AGENT_ERR                                        (WORD16)1 /*����ʧ��*/
#define LINKCFG_OK                                               (WORD16)1 /*��·���óɹ�*/
#define LINKCFG_ERR                                               (WORD16)0 /*��·����ʧ��*/
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
*                          ��������                                       *
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
    RETURN_OK = 0, /*�����ɹ�*/
    NULL_PTR,/*����п�ָ��*/ 
    LOAD_XML_ERR,/*����XMLʧ��*/
    NEW_XML_ERR,/*����XMLʧ��*/
    GET_NODE_ERR,/*��ýڵ�ʧ��*/ 
    GET_NODE_TEXT_ERR,/*��ȡ�ڵ��ı�ʧ��*/
    GET_NODE_CNT_ERR,/*��ȡ�ڵ����*/
    GET_NODE_ATTR_ERR,/*��ȡ���Խڵ�ʧ��*/
    GET_NODE_ARR_VALUE_ERR,/*��ȡ���Խڵ�ֵʧ��*/
    SET_NODE_ERR,/*���ýڵ�ʧ��*/
    SET_NODE_TEXT_ERR,/*���ýڵ��ı�ʧ��*/
    SET_NODE_ATTR_ERR,/*�������Խڵ�ʧ��*/
    SHOW_MODESTACK_ERR,/*��ѯģʽջ��Ϣ����*/
    SaveMutexTip,/*����·���ڴ���,����ִ������*/
    DatUpdateMutex,/*��ǰ�����л��汾,����ִ������*/
    CmdExecutMutex,/*ִ�������*/
    NoRegXmlLinkState,/*δע��XML��·*/
    NO_EXECUTE_CMD_ON_LINKCHANNEL,/*��·��������ִ��*/
    GET_DOCTEXTERR,/*��ȡDOC�ĵ��ı�����*/
    RETURN_ERROR/*��������*/
}XML_AGENT_STAT;
/*lint -e749*/

/**************************************************************************
*                           ȫ�ֱ���                                      *
**************************************************************************/
extern BOOL bDebugPrint;
static JID gt_OpJid[MAX_SIZE_OPJID] = {{0}};
static CHAR pRecvOMMMsgBuf[MAX_SIZE_OMMMSG];/*OMM����������*/
static CHAR pRecvPlatMsgBuf[MAX_APPMSG_LEN];/*����ƽ̨Ӧ������*/
static DWORD  g_dwSeqNo = 1;/*�������*/
static XOS_TIMER_ID  wXML_OMM_HANDSHAKE;

/**************************************************************************
*                           ���ر���                                      *
**************************************************************************/
/**************************************************************************
*                           �ⲿ����ԭ��                                  *
**************************************************************************/
/**************************************************************************
*                           �ֲ�����ԭ��                                  *
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
* �������ƣ�VOID Convert_atoi
* ��������������ת��String->WORD32
* ���ʵı���
* �޸ĵı���
* ���������pString
* ���������pDword
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/03/05    V1.0    ������      ����
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
* �������ƣ�VOID PrintMsgXml
* ������������ӡXML��Ϣ
* ���ʵı���
* �޸ĵı���
* ���������CHAR *pXmlBuf
                             CHAR *pDesc
* �����������
* �� �� ֵ��    ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/03/05    V1.0    ������      ����
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
* �������ƣ�VOID ChangeXmlLinkStateMode
* �����������ı���·��Ϣģʽջ
* ���ʵı���
* �޸ĵı���
* ���������WORD16 wRtnCode
* ���������TYPE_XMLLINKSTATE *ptLinkState
* �� �� ֵ��    ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/03/05    V1.0    ������      ����
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
    if (SUCC_AND_NOPARA == wRtnCode)/*ֻ�з��ؽ����ȷ�Ÿı�ģʽջ*/
    {
        XML_ChangeMode(ptLinkState);
    }
    return;
}

/**************************************************************************
* �������ƣ�VOID Xml_SetHandShakeTimer
* ��������������ƽ̨���������ֶ�ʱ��
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ��    ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/03/05    V1.0    ������      ����
**************************************************************************/
static VOID Xml_SetHandShakeTimer(VOID)
{
    wXML_OMM_HANDSHAKE = XOS_SetRelativeTimer(TIMER_XML_OMM_HANDSHAKE, DURATION_XML_OMM_HANDSHAKE,1);
    return;
}

/**************************************************************************
* �������ƣ�VOID Xml_KillHandShakeTimer
* ����������ɱ��ƽ̨���������ֶ�ʱ��
* ���ʵı���
* �޸ĵı���
* ���������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/03/05    V1.0    ������      ����
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
* �������ƣ�BOOL IsNeedInvertOrder
* �����������ж��Ƿ���Ҫ�ֽ���ת��
* ���ʵı���
* �޸ĵı���
* ���������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
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
* �������ƣ�XML_AGENT_STAT  SetOMMHandShakeAck
* ����������������·����Ӧ����Ϣ
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ��   RETURN_ERROR-ʧ�� RETURN_OK-�ɹ�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static XML_AGENT_STAT SetOMMHandShakeAck(OMMHandShankReq *ptOMMHandShankReq,OMMHandShankAck *ptOMMHandShankAck)
{
    OMMHandShankReq *ptOMMHandShankReqTmp = NULL;
    TYPE_XMLLINKSTATE *ptXmlLinkState = NULL;
    WORD16 wLoop = 0;
    WORD16 wTRMNum = 0;
    WORD16 wTRMIndex = 0;
    /*�������*/
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
* �������ƣ�XML_AGENT_STAT LinkCfg_LoginProcess
* ������������·����-ע��
* ���ʵı���
* �޸ĵı���
* ���������strComponentId-�����
                             ptCurNode-��ǰ�ڵ�
* �����������
* �� �� ֵ��   XML_AGENT_STAT
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2009-11-25             ������      ����
************************************************************************/
static XML_AGENT_STAT LinkCfg_LoginProcess(BYTE *pucComponentId,LPXmlNode ptNodeIn)
{
    INT iRet = RETURN_OK;                                   /*����ֵ*/
    CHAR *pOMMMsgBuf = NULL;                          /*OMMӦ����Ϣ��*/
    WORD16 wMsgLen = 0;
    WORD32 dwLoop = 0;
    WORD32 dwTrmCnt = 0;                                 /*��·����*/
    WORD32 dwTrmId = 0;
    JID tDtJid;
    LPXmlNode ptNodeTmp = NULL;
    XmlNode tTrmNode;                                       /*Trm�ڵ�*/
    XmlNode tChildNode;                                      /*�ӽڵ�*/
    XmlNode tCurNode;                                        /*��ǰ�ڵ�*/
    XmlDocPtr ptrDoc = NULL;                               /*xml�ĵ�ָ��*/
    TYPE_XMLMODEL *ptXmlModel = NULL;
    
    /*��μ��*/
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
    /*��ȡ���Ͷ�JID*/
    if (XOS_SUCCESS != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                               "....................FUNC:%s LINE:%d XOS_Sender Error,Error code %d! ....................",__func__,__LINE__,iRet);
	 return RETURN_ERROR;
    }
    /*����XML�ı�*/
    iRet = XML_NewDoc(NAME_ROOT_NODE_ACK,&ptrDoc);
    if (XML_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_NewDoc Error,Error code %d! ....................",__func__,__LINE__,iRet);
	 return iRet;
    }
    /*��ȡ���ڵ�*/
    iRet = XML_GetRootNode(ptrDoc,&tCurNode);
    if (XML_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_GetRootNode Error,Error code %d! ....................",__func__,__LINE__,iRet);
        XML_FreeXml(ptrDoc);
	 return iRet;
    }
    /*����<Type>�ڵ�*/
    iRet = XML_InsertChild(&tCurNode,0,NAME_TYPE_NODE,&tChildNode);
    if (XML_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_InsertChild Error,Error code %d! ....................",__func__,__LINE__,iRet);
	 XML_FreeXml(ptrDoc);
	 return iRet;
    }
    /*����<Type>�ڵ��ı�*/
    iRet = XML_SetChildText(&tChildNode,LINKCFG_NAME_LOGIN);
    if (XML_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_InsertChild Error,Error code %d! ....................",__func__,__LINE__,iRet);
	 XML_FreeXml(ptrDoc);
	 return iRet;
    }
    /*����<Trm>�ڵ�*/
    iRet = XML_InsertChild(&tCurNode,1,NAME_TRM_NODE,&tChildNode);
    if (XML_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_InsertChild Error,Error code %d! ....................",__func__,__LINE__,iRet);
	 XML_FreeXml(ptrDoc);
	 return iRet;
    }
    /*��ȡ<Trm>�ڵ�*/
    iRet = GetChlidByNode(ptNodeTmp,NAME_TRM_NODE,0,&tTrmNode);
    if(RETURN_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d GetChlidByNode Error,Error code %d! ....................",__func__,__LINE__,iRet);
        XML_FreeXml(ptrDoc);
        return iRet;
    }
    /*��ȡ<Trm>�ӽڵ����*/
    iRet = GetChildCountByName(&tTrmNode,NAME_ID_NODE,&dwTrmCnt);
    if(RETURN_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d GetChildCountByName Error,Error code %d! ....................",__func__,__LINE__,iRet);
        XML_FreeXml(ptrDoc);
        return iRet;
    }
    /*����ע��TRM ID*/
    for(dwLoop = 0;dwLoop < dwTrmCnt;dwLoop++)
    {
        CHAR aucNodeText[MAX_NODETEXT_BUF_SIZE] = {0};/*�ڵ��ı�*/
        XmlNode tIdNode;
        memset(&tIdNode,0,sizeof(tIdNode));
        /*��ȡ<id>�ڵ��ı�*/
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
        /*����<id>�ڵ�*/
        iRet = XML_InsertChild(&tChildNode,dwLoop,NAME_ID_NODE,&tIdNode);
	 if (XML_OK != iRet)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR,
                               "....................FUNC:%s LINE:%d XML_InsertChild Error,Error code %d! ....................",__func__,__LINE__,iRet);
	     continue;
        }
	 /*����<ID>�ڵ��ı�*/
	 iRet = XML_SetChildText(&tIdNode,aucNodeText);
	 if(XML_OK != iRet)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR,
                               "....................FUNC:%s LINE:%d XML_SetChildText Error,Error code %d! ....................",__func__,__LINE__,iRet);
	     continue;
        }
	 memset(aucNodeText,0,sizeof(aucNodeText));
	 /*ע��TRM ID*/
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
    /*��ȡ�ı�ʧ��*/
    if (NULL == pOMMMsgBuf)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XOS_GetUB Error! ....................",__func__,__LINE__);
        XML_FreeXml(ptrDoc);
        return NULL_PTR;
    }
    ptXmlModel = (TYPE_XMLMODEL *)pOMMMsgBuf;
    wMsgLen = sizeof(ptXmlModel->wComponentId) + strlen(ptXmlModel->ucXML) + 1;
    /*��Ϣ��ӡ*/
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
    /*�����ܷ�����·ע��Ӧ��ʧ��*/
    if (XOS_SUCCESS != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                               "....................FUNC:%s LINE:%d XOS_SendAsynMsg Error,Error code %d! ....................",__func__,__LINE__,iRet);	 
    }
    /*�ͷ���Դ*/
    OAM_RETUB(pOMMMsgBuf);
    XML_FreeXml(ptrDoc);
    return iRet;
}

/**********************************************************************
* �������ƣ�XML_AGENT_STAT LinkCfg_LoginProcess
* ������������·����-ע��
* ���ʵı���
* �޸ĵı���
* ���������strComponentId-�����
                             ptCurNode-��ǰ�ڵ�
* �����������
* �� �� ֵ��   XML_AGENT_STAT
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2009-11-25             ������      ����
************************************************************************/
static XML_AGENT_STAT LinkCfg_LogoutProcess(BYTE *pucComponentId,LPXmlNode ptNodeIn)
{
    INT iRet = RETURN_OK;                                   /*����ֵ*/
    CHAR *pOMMMsgBuf = NULL;                          /*OMMӦ����Ϣ��*/
    WORD16 wMsgLen = 0;
    WORD32 dwLoop = 0;
    WORD32 dwTrmCnt = 0;                                 /*��·����*/
    WORD32 dwTrmId = 0;
    JID tDtJid;
    LPXmlNode ptNodeTmp = NULL;
    XmlNode tTrmNode;                              /*Trm�ڵ�*/
    XmlNode tChildNode;                             /*�ӽڵ�*/
    XmlNode tCurNode;                               /*��ǰ�ڵ�*/
    XmlDocPtr ptrDoc = NULL;                               /*xml�ĵ�ָ��*/
    TYPE_XMLMODEL *ptXmlModel = NULL;
    
    /*��μ��*/
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
    /*��ȡ���Ͷ�JIDʧ��*/
    if (XOS_SUCCESS != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                               "....................FUNC:%s LINE:%d XOS_Sender Error,Error code %d! ....................",__func__,__LINE__,iRet);
	 return iRet;
    }
    /*����XML�ı�*/
    iRet = XML_NewDoc(NAME_ROOT_NODE_ACK,&ptrDoc);
    if (XML_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_NewDoc Error,Error code %d! ....................",__func__,__LINE__,iRet);
	 return iRet;
    }
    /*��ȡ���ڵ�*/
    iRet = XML_GetRootNode(ptrDoc,&tCurNode);
    if (XML_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_GetRootNode Error,Error code %d! ....................",__func__,__LINE__,iRet);
        XML_FreeXml(ptrDoc);
	 return iRet;
    }
    /*����<Type>�ڵ�*/
    iRet = XML_InsertChild(&tCurNode,0,NAME_TYPE_NODE,&tChildNode);
    if (XML_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_InsertChild Error,Error code %d! ....................",__func__,__LINE__,iRet);
	 XML_FreeXml(ptrDoc);
	 return iRet;
    }
    /*����<Type>�ڵ��ı�*/
    iRet = XML_SetChildText(&tChildNode,LINKCFG_NAME_LOGOUT);
    if (XML_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_InsertChild Error,Error code %d! ....................",__func__,__LINE__,iRet);
	 XML_FreeXml(ptrDoc);
	 return iRet;
    }
    /*����<Trm>�ڵ�*/
    iRet = XML_InsertChild(&tCurNode,1,NAME_TRM_NODE,&tChildNode);
    if (XML_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_InsertChild Error,Error code %d! ....................",__func__,__LINE__,iRet);
	 XML_FreeXml(ptrDoc);
	 return iRet;
    }
    /*��ȡ<Trm>�ڵ�*/
    iRet = GetChlidByNode(ptNodeTmp,NAME_TRM_NODE,0,&tTrmNode);
    if(RETURN_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d GetChlidByNode Error,Error code %d! ....................",__func__,__LINE__,iRet);
	 XML_FreeXml(ptrDoc);
        return iRet;
    }
    /*��ȡ<Trm>�ӽڵ����*/
    iRet = GetChildCountByName(&tTrmNode,NAME_ID_NODE,&dwTrmCnt);
    if(RETURN_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d GetChildCountByName Error,Error code %d! ....................",__func__,__LINE__,iRet);
	 XML_FreeXml(ptrDoc);
        return iRet;
    }
    /*����ע��TRM ID*/
    for(dwLoop = 0;dwLoop < dwTrmCnt;dwLoop++)
    {
        CHAR aucNodeText[MAX_NODETEXT_BUF_SIZE] = {0};/*�ڵ��ı�*/
        XmlNode tIdNode;
        memset(&tIdNode,0,sizeof(tIdNode));
        /*��ȡ<id>�ڵ��ı�*/
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
        /*����<id>�ڵ�*/
        iRet = XML_InsertChild(&tChildNode,dwLoop,NAME_ID_NODE,&tIdNode);
	 if (XML_OK != iRet)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR,
                               "....................FUNC:%s LINE:%d XML_InsertChild Error,Error code %d! ....................",__func__,__LINE__,iRet);
	     continue;
        }
	 /*����<ID>�ڵ��ı�*/
	 iRet = XML_SetChildText(&tIdNode,aucNodeText);
	 if(XML_OK != iRet)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR,
                               "....................FUNC:%s LINE:%d XML_SetChildText Error,Error code %d! ....................",__func__,__LINE__,iRet);
	     continue;
        }
	 memset(aucNodeText,0,sizeof(aucNodeText));
	 /*ע��TRM ID*/
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
    /*��ȡ�ı�ʧ��*/
    if (NULL == pOMMMsgBuf)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XOS_GetUB Error! ....................",__func__,__LINE__);
        XML_FreeXml(ptrDoc);
        return NULL_PTR;
    }
    ptXmlModel = (TYPE_XMLMODEL *)pOMMMsgBuf;
    wMsgLen = sizeof(ptXmlModel->wComponentId) + strlen(ptXmlModel->ucXML) + 1;
    /*��Ϣ��ӡ*/
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
    /*�����ܷ�����·ע��Ӧ��ʧ��*/
    if (XOS_SUCCESS != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                               "....................FUNC:%s LINE:%d XOS_SendAsynMsg Error,Error code %d! ....................",__func__,__LINE__,iRet);	 
    }
    /*�ͷ���Դ*/
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
    /*����XML�ĵ������ظ��ڵ�*/
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
    /*�������ʹ��OAM�ṹ��ռ�*/
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
    /*�������ʹ��OAM�ṹ��ռ�*/
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
    /*�������ʹ��OAM�ṹ��ռ�*/
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
    /*�������ʹ��OAM�ṹ��ռ�*/
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
    /*�������ʹ��OAM�ṹ��ռ�*/
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
    /*�������ʹ��OAM�ṹ��ռ�*/
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
    /*�������ʹ��OAM�ṹ��ռ�*/
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
    /*�������ʹ��OAM�ṹ��ռ�*/
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
    /*����XML�ĵ������ظ��ڵ�*/
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
* �������ƣ�XML_AGENT_STAT LinkCfg_HandShakeProcess
* ������������·����-����
* ���ʵı���
* �޸ĵı���
* ���������strComponentId-�����
                             ptCurNode-��ǰ�ڵ�
* �����������
* �� �� ֵ��   XML_AGENT_STAT
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2009-11-25             ������      ����
************************************************************************/
static XML_AGENT_STAT LinkCfg_HandShakeProcess(BYTE *pucComponentId,LPXmlNode ptNodeIn)
{
    INT iRet = RETURN_OK;                                   /*����ֵ*/
    CHAR *pOMMMsgBuf = NULL;                          /*OMMӦ����Ϣ��*/
    WORD16 wLoop = 0;
    WORD16 wMsgLen = 0;
    WORD32 dwLoop = 0;
    JID tDtJid;
    LPXmlNode ptNodeTmp = NULL;    
    XmlNode tChildNode;                             /*�ӽڵ�*/
    XmlNode tCurNode;                               /*��ǰ�ڵ�*/
    XmlDocPtr ptrDoc = NULL;                               /*xml�ĵ�ָ��*/
    TYPE_XMLMODEL *ptXmlModel = NULL;
    
    /*��μ��*/
    if ((NULL == pucComponentId) || (NULL == ptNodeIn))
    {
        return NULL_PTR;
    }
    ptNodeTmp = ptNodeIn;    
    memset(&tChildNode,0,sizeof(tChildNode));
    memset(&tCurNode,0,sizeof(tCurNode));
    memset(&tDtJid,0,sizeof(tDtJid));
    /*ɱ�����ֶ�ʱ��*/
    Xml_KillHandShakeTimer();
    /*�������ֶ�ʱ��*/
    Xml_SetHandShakeTimer();
    iRet = XOS_Sender(&tDtJid);
    /*��ȡ���Ͷ�JIDʧ��*/
    if (XOS_SUCCESS != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                               "....................FUNC:%s LINE:%d XOS_Sender Error,Error code %d! ....................",__func__,__LINE__,iRet);
	 return RETURN_ERROR;
    }
    /*����XML�ı�*/
    iRet = XML_NewDoc(NAME_ROOT_NODE_ACK,&ptrDoc);
    if (XML_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_NewDoc Error,Error code %d! ....................",__func__,__LINE__,iRet);
	 return iRet;
    }
    /*��ȡ���ڵ�*/
    iRet = XML_GetRootNode(ptrDoc,&tCurNode);
    if (XML_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_GetRootNode Error,Error code %d! ....................",__func__,__LINE__,iRet);
        XML_FreeXml(ptrDoc);
	 return iRet;
    }
    /*����<Type>�ڵ�*/    
    iRet = XML_InsertChild(&tCurNode,0,NAME_TYPE_NODE,&tChildNode);
    if (XML_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_InsertChild Error,Error code %d! ....................",__func__,__LINE__,iRet);
	 XML_FreeXml(ptrDoc);
	 return iRet;
    }
    /*����<Type>�ڵ��ı�*/
    iRet = XML_SetChildText(&tChildNode,LINKCFG_NAME_HANDSHAKE);
    if (XML_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_InsertChild Error,Error code %d! ....................",__func__,__LINE__,iRet);
	 XML_FreeXml(ptrDoc);
	 return iRet;
    }
    /*����<Trm>�ڵ�*/
    iRet = XML_InsertChild(&tCurNode,1,NAME_TRM_NODE,&tChildNode);
    if (XML_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_InsertChild Error,Error code %d! ....................",__func__,__LINE__,iRet);
	 XML_FreeXml(ptrDoc);
	 return iRet;
    }
    /*������·��Ϣ��ȡע���TRM ID*/
    for (wLoop = 0;wLoop < MAX_SIZE_LINKSTATE;wLoop++)
    {
        CHAR aucNodeText[MAX_NODETEXT_BUF_SIZE] = {0};/*�ڵ��ı�*/
        XmlNode tIdNode;
        memset(&tIdNode,0,sizeof(tIdNode));
        if (XML_LINKSTATE_USED == gt_XmlLinkState[wLoop].bUsed)
        {
            /*����<id>�ڵ�*/
            iRet = XML_InsertChild(&tChildNode,dwLoop,NAME_ID_NODE,&tIdNode);
	     if (XML_OK != iRet)
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR,
                                   "....................FUNC:%s LINE:%d XML_InsertChild Error,Error code %d! ....................",__func__,__LINE__,iRet);
	         continue;
            }
            memset(aucNodeText,0,sizeof(aucNodeText));
            snprintf(aucNodeText,sizeof(aucNodeText),"%d",gt_XmlLinkState[wLoop].dwLinkChannel);
	     /*����<ID>�ڵ��ı�*/
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
    /*��ȡ�ı�ʧ��*/
    if (NULL == pOMMMsgBuf)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XOS_GetUB Error! ....................",__func__,__LINE__);
        XML_FreeXml(ptrDoc);
        return NULL_PTR;
    }
    ptXmlModel = (TYPE_XMLMODEL *)pOMMMsgBuf;
    wMsgLen = sizeof(ptXmlModel->wComponentId) + strlen(ptXmlModel->ucXML) + 1;
    /*��Ϣ��ӡ*/
    if (bDebugPrint)
    {
        PrintMsgXml(ptXmlModel, "MSG_XMLAGNET_TO_OMM");
    }
    /*�����ܷ�����·ע��Ӧ��*/
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
    /*�ͷ���Դ*/
    OAM_RETUB(pOMMMsgBuf);
    XML_FreeXml(ptrDoc);
    return iRet;
}

/**********************************************************************
* �������ƣ�XML_AGENT_STAT LoginTrmId
* ����������ע���ն˺�
* ���ʵı���
* �޸ĵı���
* ���������dwTrmId:�ն˺�                             
* �����������
* �� �� ֵ��    TRUE/FALSE
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2009-11-25             ������      ����
************************************************************************/
static BOOLEAN LoginTrmId(WORD32 dwTrmId)
{
    WORD16 wLoop = 0;    

    /*����ն˺��Ƿ��Ѿ�ע��*/
    for(wLoop = 0;wLoop < MAX_SIZE_LINKSTATE;wLoop++)
    {
        if ((dwTrmId == gt_XmlLinkState[wLoop].dwLinkChannel)&&
	     (XML_LINKSTATE_USED == gt_XmlLinkState[wLoop].bUsed))
        {
            break;
        }
    }
    if (wLoop < MAX_SIZE_LINKSTATE)/*�Ѿ�ע���*/
    {
        return TRUE;
    }
    /*������·��Ϣ*/
    for (wLoop = 0;wLoop < MAX_SIZE_LINKSTATE;wLoop++)
    {
        if (XML_LINKSTATE_UNUSED == gt_XmlLinkState[wLoop].bUsed)
	 {
	     break;
	 }
    }
    if (wLoop >= MAX_SIZE_LINKSTATE)/*δ���䵽*/
    {
        return FALSE;
    }
    memset(&gt_XmlLinkState[wLoop],0,sizeof(TYPE_XMLLINKSTATE));    
    /*�ͷ����ʹ��OAM�ṹ��ռ�*/
    if (NULL != gt_XmlLinkState[wLoop].ptLastSendMsg)
    {
        OAM_RETUB(gt_XmlLinkState[wLoop].ptLastSendMsg);
    }
    /*�������ʹ��OAM�ṹ��ռ�*/
    gt_XmlLinkState[wLoop].ptLastSendMsg = (MSG_COMM_OAM *)XOS_GetUB(MAX_OAMMSG_LEN);
    if (NULL == gt_XmlLinkState[wLoop].ptLastSendMsg)
    {                
        return FALSE;
    }
    memset(gt_XmlLinkState[wLoop].ptLastSendMsg,0,MAX_OAMMSG_LEN);
    /*����ʹ�ñ�־*/
    gt_XmlLinkState[wLoop].bUsed = XML_LINKSTATE_USED;     
    /*������·��*/
    gt_XmlLinkState[wLoop].dwLinkChannel = dwTrmId;
    return TRUE;
}

/**********************************************************************
* �������ƣ�XML_AGENT_STAT RegisterTrmId
* ����������ע���ն˺�
* ���ʵı���
* �޸ĵı���
* ���������WORD32 dwTrmId:�ն˺�                             
* �����������
* �� �� ֵ��    TYPE_XMLLINKSTATE *ptLinkState:��·״ָ̬��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static XML_AGENT_STAT RegisterTrmId(WORD16 wLinkChannel)
{    
    TYPE_XMLLINKSTATE *ptLinkState = NULL;

    /*����ն˺��Ƿ��Ѿ�ע��*/
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
    /*����ʹ�ñ�־*/
    ptLinkState->bUsed = XML_LINKSTATE_USED;     
    /*������·��*/
    ptLinkState->dwLinkChannel = wLinkChannel;    
    /*�������ʹ��OAM�ṹ��ռ�*/
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
* �������ƣ�XML_AGENT_STAT LogOutTrmId
* ����������ע���ն˺�
* ���ʵı���
* �޸ĵı���
* ���������dwTrmId:�ն˺�                             
* �����������
* �� �� ֵ��    TRUE/FALSE
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 20011-11-25             ������      ����
************************************************************************/
static BOOLEAN LogOutTrmId(WORD32 dwTrmId)
{
    WORD16 wLoop = 0;
    /*����ն˺��Ƿ��Ѿ�ע��*/
    for(wLoop = 0;wLoop < MAX_SIZE_LINKSTATE;wLoop++)
    {
        if ((dwTrmId == gt_XmlLinkState[wLoop].dwLinkChannel)&&
	     (XML_LINKSTATE_USED == gt_XmlLinkState[wLoop].bUsed))
        {
            break;
        }
    }
    if (wLoop >= MAX_SIZE_LINKSTATE)/*δע���*/
    {
        return TRUE;
    }
    /*��ֹ��ǰ��·����*/
    Omm_CfgStopExcutingCmd(&(gt_XmlLinkState[wLoop]));
    /*ɱ����ǰ��·Ӧ��ʱ��*/
    Xml_KillExeTimer(&(gt_XmlLinkState[wLoop]));
    /*�ͷŵ�ǰ��·�����ʹ�õĽṹ��*/    
    OAM_RETUB(gt_XmlLinkState[wLoop].ptLastSendMsg);
    memset(&(gt_XmlLinkState[wLoop]),0,sizeof(TYPE_XMLLINKSTATE));    
    return TRUE;
}

/**********************************************************************
* �������ƣ�BYTE GetNodeText
* ������������ȡ�ڵ��ı�
* ���ʵı���
* �޸ĵı���
* ���������ptrNodeIn :��ǰ�ڵ�
                             strNodeName:�ڵ�����
                             dwNodeIndex:�ڵ�����
                             wNodeTextLen:�ڵ��ı�����                             
* ���������strNodeText:�ڵ��ı�
* �� �� ֵ��   XML_AGENT_STAT
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2009-11-25             ������      ����
************************************************************************/
static XML_AGENT_STAT GetNodeText(LPXmlNode ptrNodeIn,CHAR *strNodeName,WORD32 dwNodeIndex,WORD32 wNodeTextLen,CHAR *strNodeText)
{    
    INT iRet = RETURN_OK;                                      /*����ֵ*/    
    XmlNode tNodeOut;                                  /*Ŀ��ڵ�*/
    LPXmlNode ptrNodeTmp = NULL;                         /*�ڵ�ָ����ʱ����*/    
	
    /*��μ��*/
    if ((NULL == ptrNodeIn) || (NULL == strNodeName) ||(NULL == strNodeText))
    {
        return NULL_PTR;
    }
    ptrNodeTmp = ptrNodeIn;
    memset(&tNodeOut,0,sizeof(tNodeOut));
    /*��ȡָ��ڵ�*/
    iRet = GetChlidByNode(ptrNodeTmp,strNodeName,0,&tNodeOut);
    if (RETURN_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d GetChlidByNode Error,Code=%d!....................",__func__,__LINE__,iRet);        
        return iRet;
    }
    /*��ȡ�ڵ��ı�*/
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
* �������ƣ�XML_AGENT_STAT UnRegisterTrmId
* ����������ע���ն˺�
* ���ʵı���
* �޸ĵı���
* ���������WORD32 dwTrmId:�ն˺�                             
* �����������
* �� �� ֵ��    TYPE_XMLLINKSTATE *ptLinkState:��·״ָ̬��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static XML_AGENT_STAT UnRegisterTrmId(WORD16 wLinkChannel)
{    
    TYPE_XMLLINKSTATE *ptLinkState = NULL;  

    ptLinkState = Xml_GetLinkStateByLinkChannel(wLinkChannel);
    if (NULL == ptLinkState)
    {
        return RETURN_ERROR;
    }
    /*��ֹ��ǰ��·����*/
    Omm_CfgStopExcutingCmd(ptLinkState);
    /*ɱ����ǰ��·Ӧ��ʱ��*/
    Xml_KillExeTimer(ptLinkState);
    /*�ͷŵ�ǰ��·�����ʹ�õĽṹ��*/    
    OAM_RETUB(ptLinkState->ptLastSendMsg);
    memset(ptLinkState,0,sizeof(TYPE_XMLLINKSTATE));    
    return RETURN_OK;
}

/**********************************************************************
* �������ƣ�BYTE SetXmlLinkState
* ��������������XML��·״̬
* ���ʵı���
* �޸ĵı���
* ���������CHAR *ptrXmlMsgBuf:XML�ַ���
                              LPXmlNode ptrNode:��ǰ�ڵ�ָ��                     
* ���������TYPE_XMLLINKSTATE **pptLinkState:��·״̬
* �� �� ֵ��RETURN_ERROR-ʧ�� RETURN_OK-�ɹ�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static XML_AGENT_STAT SetXmlLinkState(TYPE_XMLMODEL *ptXmlModel,LPXmlNode ptrNode,TYPE_XMLLINKSTATE **pptLinkState)
{    
    TYPE_XMLMODEL *ptXmlModelTmp = NULL;/*XMLģ����ʱָ��*/	 
    LPXmlNode ptrNodeTmp = NULL;                /*�ڵ�ָ����ʱ����*/    
    TYPE_XMLLINKSTATE *ptLinkState = NULL; /*��·״ָ̬��*/    
    WORD32 dwLinkChannel = 0;/*�ն����Ӻ�*/ 
    INT iRet = 0;

    if ((NULL == ptXmlModel) || (NULL == ptrNode) )
    {
        return NULL_PTR;
    }

    ptXmlModelTmp = ptXmlModel;
    ptrNodeTmp = ptrNode;     
	
    /*�����ն����Ӻ�*/
    if (RETURN_OK != SetTRMNodeText(ptrNodeTmp, &dwLinkChannel))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................SetTRMNodeText Error FUNC: %s,Line:%d!....................",__func__,__LINE__);
        return SET_NODE_TEXT_ERR;
    }
    /*�����ն���·�ŷ�����·״̬*/
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
    /*��ǰ��·����ִ������*/
    if (pec_MsgHandle_EXECUTE == XML_LinkGetCurRunState(ptLinkState))
    {        
        return CmdExecutMutex;
    }
    /*���������·�Ƿ�ִ�д�������*/
    if (CheckLinkIsExecutingSaveCmd())
    {        
        return SaveMutexTip;
    }
    /*��䷢�Ͷ�JID*/
    /*ʹ�õ��Ժ�����������������Ҫע������ļ��*/
    if (XOS_SUCCESS != XOS_Sender(&(ptLinkState->tDtJid)))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC: %s,Line:%d XOS_Sender Error!....................",__func__,__LINE__);
        return RETURN_ERROR;
    }
    /*��������*/   
    if (IsNeedInvertOrder())
    {
        ptXmlModelTmp->wComponentId = XOS_InvertWord16(ptXmlModelTmp->wComponentId);
    }
    memcpy(ptLinkState->ucComponentId,(BYTE *)&(ptXmlModelTmp->wComponentId),sizeof(ptLinkState->ucComponentId));    
    /*��䷢�����*/
    ptLinkState->dwSeqNo = g_dwSeqNo;    
    /*���XML��������*/
    memset(ptLinkState->xmlDeclText,0,sizeof(ptLinkState->xmlDeclText));
    if(RETURN_OK != GetDeclTextByName(ptXmlModelTmp->ucXML,ptLinkState->xmlDeclText))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................GetDeclTextByName Error FUNC: %s,Line:%d!....................",__func__,__LINE__);
        return GET_NODE_TEXT_ERR;
    }
    /*���XML�ַ�����T������*/
    memset(ptLinkState->xmlTSecText,0,sizeof(ptLinkState->xmlTSecText));
    if (RETURN_OK != GetTSectionTextByName(ptXmlModelTmp->ucXML,ptLinkState->xmlTSecText))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................GetTSectionTextByName Error FUNC: %s,Line:%d!....................",__func__,__LINE__);
        return GET_NODE_TEXT_ERR;
    }    
    /*��ǰ����״̬ΪS_UpdateDAT�򲻴���*/
    if (S_UpdateDAT == XOS_GetCurState())
    {
        *pptLinkState = ptLinkState;
        return DatUpdateMutex;
    }    
    /*��������ڵ�ID����*/
    if(RETURN_OK != SetCMDNodeAttr_ID(ptrNodeTmp,&(ptLinkState->dwCmdId)))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................SetCMDNodeAttr_ID Error FUNC: %s,Line:%d!....................",__func__,__LINE__);
        return SET_NODE_ATTR_ERR;
    }  
    /*����Ƿ�NO����*/
    if(RETURN_OK != SetOTNodeText(ptrNodeTmp,&(ptLinkState->tCurCMDInfo.bIsNoCmd)))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................SetOTNodeText Error FUNC: %s,Line:%d!....................",__func__,__LINE__);
        return GET_NODE_TEXT_ERR;
    }    
    /*��䵱ǰ������Ϣ*/
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
* �������ƣ�VOID XML_ParaseDoc
* ���������������ĵ������ظ��ڵ�
* ���ʵı���
* �޸ĵı���
* ���������CHAR *ptrXml:XML�ַ���
                              WORD32 dwLen:XML�ַ�������
                              INT iOptions:��������
* ���������XmlDocPtr *ppXmlDocPtr:�ĵ�ָ���ָ��
                              LPXmlNode ptRootNode:���ڵ�ָ��
* �� �� ֵ��RETURN_ERROR-ʧ�� RETURN_OK-�ɹ�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2008-9-2             ������      ����
**************************************************************************/
static XML_AGENT_STAT XML_ParaseDoc(CHAR *ptrXml,WORD32 dwLen,INT iOptions,XmlDocPtr *ppXmlDocPtr,LPXmlNode ptRootNode)
{
    XmlDocPtr ptrDoc = NULL;/*XML�ĵ�ָ��*/

    /*�������*/
    if ((NULL == ptrXml) || (0 == dwLen) || (NULL == ppXmlDocPtr) || (NULL == ptRootNode))
    {
        return NULL_PTR;
    }
    /*����XML�ĵ�*/
    if (XML_OK != XML_LoadXml(ptrXml,dwLen,iOptions,&ptrDoc))
    {        
	 return LOAD_XML_ERR;/*����XML�ĵ�ʧ��*/
    }    
    /*��ȡ���ڵ�ָ��*/    
    if (XML_OK != XML_GetRootNode(ptrDoc,ptRootNode))
    {    	
        XML_FreeXml(ptrDoc);
        return GET_NODE_ERR;/*��ȡXML���ڵ�ʧ��*/
    }
    *ppXmlDocPtr = ptrDoc;
    return RETURN_OK;
}

/**********************************************************************
* �������ƣ�BYTE GetChlidByNode
* ��������������ָ���Ľڵ��ҵ���Ӧ���ӽڵ�
* ���ʵı���
* �޸ĵı���
* ���������LPXmlNode ptrNodeIn:��ǰ�ӽڵ�ָ��                              
                              CHAR *ptrNodeName:�ӽڵ���
                              WORD32 dwNodeIndex:�ӽڵ���� 0-�������ظ����ƵĽڵ� ��0��ʾ�ڼ����ڵ�
* ���������LPXmlNode ptrNodeOut:�ӽڵ�ָ��            
* �� �� ֵ��RETURN_ERROR-ʧ�� RETURN_OK-�ɹ�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static XML_AGENT_STAT GetChlidByNode(LPXmlNode ptrNodeIn,CHAR *ptrNodeName,WORD32 dwNodeIndex,LPXmlNode ptrNodeOut)
{
    LPXmlNode ptrNodeTmp = NULL;/*�ڵ�ָ����ʱ����*/
    CHAR *ptrNodeNameTmp = NULL;/*�ڵ�������ʱָ��*/   
    WORD32 dwCnt = 0;
    WORD32 dwChildCnt = 0;
    WORD32 i = 0;
	
    if ((ptrNodeIn == NULL) || (ptrNodeName == NULL)  || (ptrNodeOut == NULL))
    {
        return NULL_PTR;
    }
    ptrNodeTmp = ptrNodeIn;	
    ptrNodeNameTmp = ptrNodeName;  
    /*�����ӽڵ����*/
    dwChildCnt = XML_GetChildCount(ptrNodeTmp);        
    if (dwNodeIndex == 0)/*��ʾ�����������Ľڵ�*/
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
    else if(dwNodeIndex > 0)/*��ʾ�������Ľڵ����,��Ҫָ���ǵ�ǰ�µĵڼ����ڵ�*/
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
* �������ƣ�BYTE GetChildByName
* ��������������ָ���Ľڵ����ҵ���Ӧ���ӽڵ�
* ���ʵı���
* �޸ĵı���
* ���������LPXmlNode ptrNodeIn:��ǰ�ӽڵ�ָ��
            CHAR *ptrSecName:����
            CHAR *ptrNodeName:�ӽڵ���
            WORD32 dwNodeIndex:�ӽڵ���� 0-�������ظ����ƵĽڵ� ��0��ʾ�ڼ����ڵ�
* ���������LPXmlNode ptrNodeOut:�ӽڵ�ָ��            
* �� �� ֵ��RETURN_ERROR-ʧ�� RETURN_OK-�ɹ�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static XML_AGENT_STAT GetChildByName(LPXmlNode ptrNodeIn,CHAR *ptrSecName,CHAR *ptrNodeName,WORD32 dwNodeIndex,LPXmlNode ptrNodeOut)
{
    LPXmlNode ptrNodeInTmp = NULL;/*�ڵ�ָ����ʱ����*/
    LPXmlNode ptrSecNodeTmp = NULL;  /*��ʱָ�����*/ 
    XmlNode tSecNodeOut;/*�νڵ�ṹ*/
    CHAR *ptrSecNameTmp   = NULL;/*������ʱ����*/
    CHAR *ptrNodeNameTmp  = NULL;/*�ڵ�����ʱ����*/    
	  	  
    if ((ptrNodeIn == NULL) || (ptrSecName == NULL) || (ptrNodeName == NULL) || (ptrNodeOut == NULL))
    {
        return NULL_PTR;
    }
    
    ptrNodeInTmp     = ptrNodeIn;
    ptrSecNameTmp  = ptrSecName;
    ptrNodeNameTmp = ptrNodeName;    
    memset(&tSecNodeOut,0,sizeof(XmlNode));
    ptrSecNodeTmp = &tSecNodeOut;
    /*���ݶ������Ҷνڵ�*/
    if (RETURN_OK != GetChlidByNode(ptrNodeInTmp,ptrSecNameTmp,0, ptrSecNodeTmp))
    {
        return GET_NODE_ERR;
    }
    /*���ݽڵ�����ȡ�ڵ�*/
    if (RETURN_OK != GetChlidByNode(ptrSecNodeTmp,ptrNodeNameTmp,dwNodeIndex,ptrNodeOut))
    {
        return GET_NODE_ERR;
    }   
    return RETURN_OK;
}

/**********************************************************************
* �������ƣ�BYTE FindChildAttrByName
* ���������������������Ʋ�������
* ���ʵı���
* �޸ĵı���
* ���������LPXmlNode ptrNode:�����ӽڵ�
            CHAR *ptrAttrName:��������
            WORD16 wLen:���Ի�������С            
* ���������CHAR * ptrAttrBuf:���Ի�����ָ��
* �� �� ֵ��RETURN_ERROR-ʧ�� RETURN_OK-�ɹ�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static XML_AGENT_STAT GetNodeAttrValueByName(LPXmlNode ptrNode,CHAR *ptrAttrName,WORD16 wLen,CHAR * ptrAttrBuf)
{
    LPXmlNode ptrNodeTmp = NULL;/*�ڵ���ʱָ��*/
    CHAR *ptrAttrNameTmp = NULL;/*����������ʱָ��*/
    LPXmlAttr ptXmlAttr = NULL;/*���Խڵ�ָ��*/
    XmlAttr tXmlAttr;/*���Խڵ�ṹ*/    
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
    /*��ȡָ���ڵ����Ը���*/
    dwAttrCnt = XML_GetAttributeCount(ptrNodeTmp);
    /*������������ȡ���Խṹ*/
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
* �������ƣ�BYTE GetChildTextByNode
* ��������������ָ���Ľڵ��ȡ��Ӧ���ı�
* ���ʵı���
* �޸ĵı���
* ���������LPXmlNode ptrNode:�ڵ�ָ��  
            WORD16 wLen:��������С         
* ���������CHAR * ptrNodeText_Buf:�ı�������ָ��
* �� �� ֵ��RETURN_ERROR-ʧ�� RETURN_OK-�ɹ�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static XML_AGENT_STAT GetChildTextByNode(LPXmlNode ptrNode,WORD16 wLen,CHAR *ptrNodeText_Buf)
{
    WORD16 wActualLen = 0;/*ʵ��д���ַ�����*/
    LPXmlNode ptrNodeTmp = NULL;/*�ڵ�ָ����ʱ����*/
	  
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
* �������ƣ�BYTE GetChildCountByName
* ��������������ָ���Ľڵ���ָ�����Ƶ��ӽڵ����
* ���ʵı���
* �޸ĵı���
* ���������LPXmlNode ptrNode:�ڵ�ָ��  
            CHAR *ptrNodeName:ָ���ڵ�����      
* ���������BYTE *bParaCnt:�ӽڵ����
* �� �� ֵ��RETURN_ERROR-ʧ�� RETURN_OK-�ɹ�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static XML_AGENT_STAT GetChildCountByName(LPXmlNode ptrNode,CHAR *ptrNodeName,WORD32 *dwNumber)
{
    LPXmlNode ptrNodeTmp = NULL;/*�ڵ���ʱָ��*/
    CHAR *ptrNodeNameTmp = NULL;/*�ڵ�������ʱָ��*/
    LPXmlNode ptXmlNode = NULL;/*�ڵ�ָ��*/
    XmlNode tXmlNode;/*��ʱ�ڵ����*/
    WORD32 dwChildCnt = 0;/*�ӽڵ����*/
    WORD32 dwFindCnt = 0;/*���ҵ��ӽڵ����*/
    WORD32 i = 0;
    
    if ((ptrNode == NULL) || (ptrNodeName == NULL) || (dwNumber == NULL))
    {
        return NULL_PTR;
    }
    
    ptrNodeTmp = ptrNode;
    ptrNodeNameTmp = ptrNodeName;
    ptXmlNode = &tXmlNode;
    memset(ptXmlNode,0,sizeof(XmlNode));
	
    /*��ȡָ���ڵ����ӽڵ����*/
    dwChildCnt = XML_GetChildCount(ptrNodeTmp);
    /*��ȡָ���ڵ���ָ���ӽڵ����Ƶ��ӽڵ����*/
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
* �������ƣ�BYTE GetTextByName
* ��������������ͷβ���ƻ�ȡ�ı�
* ���ʵı���
* �޸ĵı���
* ���������CHAR *ptrHeadName:ͷ����
                              CHAR *ptrlEndName:β����
                              WORD32 dwLength:�������������󳤶�
                              CHAR *ptrInBuf:���뻺����ָ��
* ���������CHAR *ptrOutBuf:���������ָ��
* �� �� ֵ��RETURN_ERROR-ʧ�� RETURN_OK-�ɹ�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static XML_AGENT_STAT GetTextByName(CHAR *ptrHeadName,CHAR *ptrlEndName,WORD32 dwLength,CHAR *ptrInBuf,CHAR *ptrOutBuf)
{
    CHAR *pStrTmpHead = NULL;/*ͷ����λ��*/
    CHAR *pStrTmpEnd = NULL;    /*β����λ��*/
    WORD32 dwLenTmp = 0;	/*ȡ���ı���С*/

    /*����ж�*/
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
* �������ƣ�BYTE GetDeclTextByName
* ������������ȡXML��������
* ���ʵı���
* �޸ĵı���
* ���������CHAR *ptrDeclHeadName:����ͷ����
                              CHAR *ptrDeclEndName:����β����
                              CHAR *ptrXmlBuf:XML�ַ���
* ���������CHAR *ptrDeclBuf:XML�����ı�
* �� �� ֵ��RETURN_ERROR-ʧ�� RETURN_OK-�ɹ�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static XML_AGENT_STAT GetDeclTextByName(CHAR *ptrXmlBuf,CHAR *ptrDeclBuf)
{
    CHAR *ptrDeclHead = "<?xml";/*XML����ͷ*/
    CHAR *ptrDeclEnd = "?>";        /*XML����β*/
    CHAR *ptrInBuf = NULL;
    CHAR *ptrOutBuf = NULL;
    WORD32 dwLength = MAX_SIZE_DECL_TEXT;

    /*����ж�*/
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
* �������ƣ�BYTE GetTSectionTextByName
* ������������ȡ�νڵ��������ı� �����ڵ�����
* ���ʵı���
* �޸ĵı���
* ���������CHAR *ptrSecName:�ڵ���
                              LPXmlNode ptrNode:�ڵ�ָ��
* ���������CHAR *ptrTSecBuf:T������
* �� �� ֵ��RETURN_ERROR-ʧ�� RETURN_OK-�ɹ�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static XML_AGENT_STAT GetTSectionTextByName(CHAR *ptrXmlBuf,CHAR *ptrSecBuf)
{  
    CHAR *ptrSecHeadName = "<T>";
    CHAR *ptrSecEndName = "</T>";
    CHAR *ptrInBuf = NULL;
    CHAR *ptrOutBuf = NULL;
    WORD32 dwLen = MAX_SIZE_TSECTION_TEXT;    
	  
    /*��μ��*/
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
* �������ƣ�BYTE GetCMDNode
* ������������ȡ����(CMD)�ڵ�
* ���ʵı���
* �޸ĵı���
* ���������LPXmlNode ptrNodeIn :��ǰ�ڵ�ָ��
* ���������LPXmlNode ptrNodeOut:���ؽڵ�ָ��
* �� �� ֵ��RETURN_ERROR-ʧ�� RETURN_OK-�ɹ�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static XML_AGENT_STAT GetCMDNode(LPXmlNode ptrNodeIn,LPXmlNode ptrNodeOut)
{
    LPXmlNode ptrNodeTmp = NULL;/*�ڵ�ָ����ʱ����*/
    LPXmlNode ptrDtNode = NULL;/*Ŀ��ڵ�ָ��*/
    XmlNode tDtNode;/*Ŀ��ڵ���ʱ����*/    
    CHAR *ptrSecName = "D";/*D��*/
    CHAR *ptrMDNodeName = "MD";/*�ڵ�����-MD*/
    CHAR *ptrNodeName = "CMD";/*�ڵ�����-CMD*/
    WORD32 dwNodeIndex = 0;/*�ӽڵ���� 0-�������ظ����ƵĽڵ� ��0��ʾ�ڼ����ڵ�*/

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
* �������ƣ�BYTE GetNIDNode
* ������������ȡ��Ԫ���(NID)�ڵ�
* ���ʵı���
* �޸ĵı���
* ���������LPXmlNode ptrNodeIn :��ǰ�ڵ�ָ��
* ���������LPXmlNode ptrNodeOut:���ؽڵ�ָ��
* �� �� ֵ��RETURN_ERROR-ʧ�� RETURN_OK-�ɹ�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static XML_AGENT_STAT GetNIDNode(LPXmlNode ptrNodeIn,LPXmlNode ptrNodeOut)
{
    LPXmlNode ptrNodeTmp = NULL;/*�ڵ�ָ����ʱ����*/
    CHAR *ptrSecName = "T";/*T��*/
    CHAR *ptrNodeName = "NID";/*�ڵ�����-NTP*/
    WORD32 dwNodeIndex = 0;/*�ӽڵ���� 0-�������ظ����ƵĽڵ� ��0��ʾ�ڼ����ڵ�*/
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
* �������ƣ�BYTE GetOTNode
* ������������ȡ��������(OT)�ڵ�
* ���ʵı���
* �޸ĵı���
* ���������LPXmlNode ptrNodeIn :��ǰ�ڵ�ָ��
* ���������LPXmlNode ptrNodeOut:���ؽڵ�ָ��
* �� �� ֵ��RETURN_ERROR-ʧ�� RETURN_OK-�ɹ�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static XML_AGENT_STAT GetOTNode(LPXmlNode ptrNodeIn,LPXmlNode ptrNodeOut)
{
    LPXmlNode ptrNodeTmp = NULL;/*�ڵ�ָ����ʱ����*/
    CHAR *ptrSecName = "D";/*D��*/
    CHAR *ptrNodeName = "OT";/*�ڵ�����-CMD*/
    WORD32 dwNodeIndex = 0;/*�ӽڵ���� 0-�������ظ����ƵĽڵ� ��0��ʾ�ڼ����ڵ�*/
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
* �������ƣ�BYTE GetNTPNode
* ������������ȡ��Ԫ����(NTP)�ڵ�
* ���ʵı���
* �޸ĵı���
* ���������LPXmlNode ptrNodeIn :��ǰ�ڵ�ָ��
* ���������LPXmlNode ptrNodeOut:���ؽڵ�ָ��
* �� �� ֵ��RETURN_ERROR-ʧ�� RETURN_OK-�ɹ�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static XML_AGENT_STAT GetNTPNode(LPXmlNode ptrNodeIn,LPXmlNode ptrNodeOut)
{
    LPXmlNode ptrNodeTmp = NULL;/*�ڵ�ָ����ʱ����*/
    CHAR *ptrSecName = "T";/*T��*/
    CHAR *ptrNodeName = "NTP";/*�ڵ�����-NTP*/
    WORD32 dwNodeIndex = 0;/*�ӽڵ���� 0-�������ظ����ƵĽڵ� ��0��ʾ�ڼ����ڵ�*/
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
* �������ƣ�BYTE GetTRMNode
* ������������ȡ��Ԫ����(TRM)�ڵ�
* ���ʵı���
* �޸ĵı���
* ���������LPXmlNode ptrNodeIn :��ǰ�ڵ�ָ��
* ���������LPXmlNode ptrNodeOut:���ؽڵ�ָ��
* �� �� ֵ��RETURN_ERROR-ʧ�� RETURN_OK-�ɹ�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static XML_AGENT_STAT GetTRMNode(LPXmlNode ptrNodeIn,LPXmlNode ptrNodeOut)
{
    LPXmlNode ptrNodeTmp = NULL;/*�ڵ�ָ����ʱ����*/
    CHAR *ptrSecName = "T";/*T��*/
    CHAR *ptrNodeName = "TRM";/*�ڵ�����-TRM*/
    WORD32 dwNodeIndex = 0;/*�ӽڵ���� 0-�������ظ����ƵĽڵ� ��0��ʾ�ڼ����ڵ�*/
	  
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
* �������ƣ�BYTE GetPARANode
* ������������ȡ����ڵ��²���(P)�ڵ�
* ���ʵı���
* �޸ĵı���
* ���������LPXmlNode ptrNodeIn :��ǰ�ڵ�ָ��
* ���������LPXmlNode ptrNodeOut:���ؽڵ�ָ��
* �� �� ֵ��RETURN_ERROR-ʧ�� RETURN_OK-�ɹ�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static XML_AGENT_STAT GetPARANode(LPXmlNode ptrNodeIn,WORD32 dwNodeIndex,LPXmlNode ptrNodeOut)
{
    LPXmlNode ptrNodeTmp = NULL;/*�ڵ�ָ����ʱ����*/    
    CHAR *ptrNodeName = "P";/*�ڵ�����-P*/
	  
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
* �������ƣ�BYTE GetCMDNodeAttr_ID
* ������������ȡ����(CMD)�ڵ�ID����
* ���ʵı���
* �޸ĵı���
* ���������LPXmlNode ptrNode :��ǰ�ڵ�ָ��
                              WORD16 wLen:����������
* ���������CHAR *ptrNodeAttr_Buf:���ؽڵ��ı�ָ��
* �� �� ֵ��RETURN_ERROR-ʧ�� RETURN_OK-�ɹ�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static XML_AGENT_STAT GetCMDNodeAttr_ID(LPXmlNode ptrNode,WORD16 wLen,CHAR *ptrNodeAttr_Buf)
{
    LPXmlNode ptrNodeTmp = NULL;/*�ڵ�ָ����ʱ����*/
    LPXmlNode ptrDtNode = NULL;/*Ŀ��ڵ�ָ��*/
    XmlNode tDtNode;/*Ŀ��ڵ�ṹ*/
    CHAR *ptrAttrName = "id";
	
    if ((ptrNode == NULL) || (wLen == 0) || (ptrNodeAttr_Buf == NULL))
    {
        return NULL_PTR;
    }
    ptrNodeTmp = ptrNode;
    ptrDtNode = &tDtNode;
    memset(ptrDtNode,0,sizeof(XmlNode));
    /*��ȡ����(CMD)�ڵ�*/
    if (RETURN_OK != GetCMDNode(ptrNodeTmp,ptrDtNode))
    {
    	  return GET_NODE_ERR;
    }
    /*���CMD�ڵ�ID����*/	
    if (RETURN_OK != GetNodeAttrValueByName(ptrDtNode,ptrAttrName,MAX_NODEATTR_BUF_SIZE,ptrNodeAttr_Buf))
    {
        return GET_NODE_ATTR_ERR;	    
    }
    return RETURN_OK;
}

/**********************************************************************
* �������ƣ�BYTE SetCMDNodeAttr_ID
* ������������ȡ����(CMD)�ڵ�ID����
* ���ʵı���
* �޸ĵı���
* ���������LPXmlNode ptrNodeIn :��ǰ�ڵ�ָ��
* ���������LPXmlNode ptrNodeOut:���ؽڵ�ָ��
* �� �� ֵ��RETURN_ERROR-ʧ�� RETURN_OK-�ɹ�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static XML_AGENT_STAT SetCMDNodeAttr_ID(LPXmlNode ptrNode,DWORD *dwCmdId)
{
    LPXmlNode ptrNodeTmp = NULL;/*�ڵ�ָ����ʱ����*/    
    CHAR ptrNodeAttr_Buf[MAX_NODEATTR_BUF_SIZE];/*�ڵ����Ի�����*/
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
    /*��ȡCMD�ڵ�ID����*/	
    if (RETURN_OK != GetCMDNodeAttr_ID(ptrNodeTmp,MAX_NODEATTR_BUF_SIZE,ptrNodeAttr_Buf))
    {
        return GET_NODE_ATTR_ERR;	    
    }
    *dwCmdId = strtoul(ptrNodeAttr_Buf,&pTmp,16);
    return RETURN_OK;
}

/**********************************************************************
* �������ƣ�BYTE GetParaNodeAttr_ID
* ������������ȡ����(P)�ڵ�ID����
* ���ʵı���
* �޸ĵı���
* ���������LPXmlNode ptrNode:��ǰ�ڵ�ָ��
                              WORD32 dwNumber:�ڵ����
                              WORD16 wLen:��������С
* ���������CHAR *ptrNodeAttr_Buf:�ڵ��ı�������
* �� �� ֵ��RETURN_ERROR-ʧ�� RETURN_OK-�ɹ�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static XML_AGENT_STAT GetParaNodeAttr_ID(LPXmlNode ptrNode,WORD32 dwNumber,WORD16 wLen,CHAR *ptrNodeAttr_Buf)
{
    LPXmlNode ptrNodeTmp = NULL;/*�ڵ�ָ����ʱ����*/  
    LPXmlNode ptrDtNode = NULL;/*Ŀ��ڵ�ָ��*/
    XmlNode tDtNode;/*Ŀ��ڵ�ṹ����*/    
    CHAR *ptrAttrName = "id";    

    if ((ptrNode == NULL) || (wLen == 0) || (ptrNodeAttr_Buf == NULL))
    {
        return NULL_PTR;
    }

    ptrNodeTmp = ptrNode;
    ptrDtNode = &tDtNode;
    memset(ptrDtNode,0,sizeof(XmlNode));
    /*��ȡ�����ڵ�����*/
    if (RETURN_OK != GetPARANode(ptrNodeTmp,dwNumber,ptrDtNode))
    {
        return GET_NODE_ERR;
    }
    /*��ȡ�����ڵ�����-ID*/    
    if (RETURN_OK != GetNodeAttrValueByName(ptrDtNode,ptrAttrName,wLen,ptrNodeAttr_Buf))
    {
        return GET_NODE_ATTR_ERR;	    
    }

    return RETURN_OK;
}

/**********************************************************************
* �������ƣ�BYTE SetParaNodeAttr_ID
* ������������ȡ����(P)�ڵ�ID����
* ���ʵı���
* �޸ĵı���
* ���������LPXmlNode ptrNode:��ǰ�ڵ�ָ��
                              BYTE bNumber:�ڵ���
* ���������BYTE *bParaNo:�������
* �� �� ֵ��RETURN_ERROR-ʧ�� RETURN_OK-�ɹ�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static XML_AGENT_STAT SetParaNodeAttr_ID(LPXmlNode ptrNode,BYTE bNumber,BYTE *bParaNo)
{
    LPXmlNode ptrNodeTmp = NULL;/*�ڵ�ָ����ʱ����*/      
    CHAR ptrNodeAttr_Buf[MAX_NODEATTR_BUF_SIZE];/*�ڵ����Ի�����*/	  
    WORD32 dwNumber = 0;
    WORD32 dwParaNo = 0;
    
    if ((ptrNode == NULL) || (bParaNo == NULL))
    {
        return NULL_PTR;
    }
    ptrNodeTmp = ptrNode;   
    memset(ptrNodeAttr_Buf,0,MAX_NODEATTR_BUF_SIZE);
    dwNumber = (WORD32)bNumber;
    /*��ȡ�����ڵ�����-ID*/    
    if (RETURN_OK != GetParaNodeAttr_ID(ptrNodeTmp,dwNumber,MAX_NODEATTR_BUF_SIZE,ptrNodeAttr_Buf))
    {
        return GET_NODE_ATTR_ERR;	    
    }
    if (0 != Convert_atoi(ptrNodeAttr_Buf,&dwParaNo))
    {
        return RETURN_ERROR;
    }
    *bParaNo = (BYTE)dwParaNo;
    /*���ܲ���ID��0��ʼ��ƽ̨�����1��ʼ*/
    return RETURN_OK;
}

/**********************************************************************
* �������ƣ�BYTE GetTRMNodeText
* ������������ȡ�ն˺�(TRM)�ڵ��ı�
* ���ʵı���
* �޸ĵı���
* ���������LPXmlNode ptrNodeIn :��ǰ�ڵ�ָ��
                              WORD16 wLen:�ڵ㻺������С
* ���������CHAR *ptrNodeText_Buf:�ڵ��ı�������
* �� �� ֵ��RETURN_ERROR-ʧ�� RETURN_OK-�ɹ�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static XML_AGENT_STAT GetTRMNodeText(LPXmlNode ptrNode,WORD16 wLen,CHAR *ptrNodeText_Buf)
{
    LPXmlNode ptrNodeTmp = NULL;/*�ڵ�ָ����ʱ����*/
    LPXmlNode ptrDtNode = NULL;/*Ŀ��ڵ�ָ��*/
    XmlNode tDtNode;/*Ŀ��ڵ�ṹ*/    

    if((ptrNode == NULL) || (wLen == 0)  || (ptrNodeText_Buf == NULL))
    {
        return NULL_PTR;
    }
    ptrNodeTmp = ptrNode;
    ptrDtNode = &tDtNode;
    memset(ptrDtNode,0,sizeof(XmlNode));
    /*��ȡTRM�ڵ�*/
    if (RETURN_OK != GetTRMNode(ptrNodeTmp,ptrDtNode))
    {
        return GET_NODE_ERR;
    }
    /*��ȡTRM�ڵ��ı�*/
    if (RETURN_OK != GetChildTextByNode(ptrDtNode,MAX_NODETEXT_BUF_SIZE,ptrNodeText_Buf))
    {
        return GET_NODE_TEXT_ERR;
    }
	
    return RETURN_OK;
}

/**********************************************************************
* �������ƣ�BYTE SetTRMNodeText
* ���������������ն˺�(TRM)�ڵ��ı�
* ���ʵı���
* �޸ĵı���
* ���������LPXmlNode ptrNodeIn :��ǰ�ڵ�ָ��            
* ���������WORD16 *pwLinkChannel:�ն˺�
* �� �� ֵ��RETURN_ERROR-ʧ�� RETURN_OK-�ɹ�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static XML_AGENT_STAT SetTRMNodeText(LPXmlNode ptrNode,WORD32 *pdwLinkChannel)
{
    LPXmlNode ptrNodeTmp = NULL;/*�ڵ�ָ����ʱ����*/    
    CHAR ptrNodeText_Buf[MAX_NODETEXT_BUF_SIZE];/*�ڵ��ı�������*/    
    WORD32 dwLinkChannel = 0;
    if ((ptrNode == NULL) || (pdwLinkChannel== NULL))
    {
        return NULL_PTR;
    }
    ptrNodeTmp = ptrNode;    
    memset(ptrNodeText_Buf,0,MAX_NODETEXT_BUF_SIZE);
    /*��ȡTRM�ڵ��ı�*/
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
* �������ƣ�BYTE GetNIDNodeText
* ������������ȡ��ԪID(NID)�ڵ��ı�
* ���ʵı���
* �޸ĵı���
* ���������LPXmlNode ptrNode :��ǰ�ڵ�ָ��
                              WORD16 wLen:��������С
* ���������CHAR *ptrNodeText_Buf:�ڵ��ı�������
* �� �� ֵ��RETURN_ERROR-ʧ�� RETURN_OK-�ɹ�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static XML_AGENT_STAT GetNIDNodeText(LPXmlNode ptrNode,WORD16 wLen,CHAR *ptrNodeText_Buf)
{
    LPXmlNode ptrNodeTmp = NULL;/*�ڵ�ָ����ʱ����*/
    LPXmlNode ptrDtNode = NULL;/*Ŀ��ڵ�ָ��*/
    XmlNode tDtNode;/*Ŀ��ڵ�ṹ*/
	
    if ((ptrNode == NULL) || (wLen == 0) || (ptrNodeText_Buf == NULL))
    {
        return NULL_PTR;
    }

    ptrNodeTmp = ptrNode;
    ptrDtNode = &tDtNode;
    memset(ptrDtNode,0,sizeof(XmlNode));
    /*��ȡNID�ڵ�*/
    if (RETURN_OK != GetNIDNode(ptrNodeTmp,ptrDtNode))
    {
        return GET_NODE_ERR;
    }
    /*��ȡNID�ڵ��ı�*/
    if (RETURN_OK != GetChildTextByNode(ptrDtNode,MAX_NODETEXT_BUF_SIZE,ptrNodeText_Buf))
    {
        return GET_NODE_TEXT_ERR;
    }
	
    return RETURN_OK;	
}

/**********************************************************************
* �������ƣ�BYTE SetNIDNodeText
* ����������������ԪID(NID)�ڵ��ı�
* ���ʵı���
* �޸ĵı���
* ���������LPXmlNode ptrNodeIn :��ǰ�ڵ�ָ��            
* ���������WORD32 *pdwNetId:��Ԫ��ʶ
* �� �� ֵ��RETURN_ERROR-ʧ�� RETURN_OK-�ɹ�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static XML_AGENT_STAT SetNIDNodeText(LPXmlNode ptrNodeIn,WORD32 *pdwNetId)
{
    LPXmlNode ptrNodeTmp = NULL;/*�ڵ�ָ����ʱ����*/    
    CHAR ptrNodeText_Buf[MAX_NODETEXT_BUF_SIZE];/*�ڵ��ı�������*/    
    WORD32 dwNetId = 0;
    if ((ptrNodeIn == NULL) || (pdwNetId== NULL))
    {
        return NULL_PTR;
    }
    ptrNodeTmp = ptrNodeIn;    
    memset(ptrNodeText_Buf,0,MAX_NODETEXT_BUF_SIZE);
    /*��ȡTRM�ڵ��ı�*/
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
* �������ƣ�BYTE GetOTNodeText
* ������������ȡ�����������(OT)�ڵ��ı�
* ���ʵı���
* �޸ĵı���
* ���������LPXmlNode ptrNode :��ǰ�ڵ�ָ��
                              WORD16 wLen:��������С
* ���������CHAR *ptrNodeText_Buf:�ڵ��ı�������
* �� �� ֵ��RETURN_ERROR-ʧ�� RETURN_OK-�ɹ�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static XML_AGENT_STAT GetOTNodeText(LPXmlNode ptrNode,WORD16 wLen,CHAR *ptrNodeText_Buf)
{
    LPXmlNode ptrNodeTmp = NULL;/*�ڵ�ָ����ʱ����*/
    LPXmlNode ptrDtNode = NULL;/*Ŀ��ڵ�ָ��*/
    XmlNode tDtNode;/*Ŀ��ڵ�ṹ*/
	
    if ((ptrNode == NULL) || (wLen == 0) || (ptrNodeText_Buf == NULL))
    {
        return NULL_PTR;
    }

    ptrNodeTmp = ptrNode;
    ptrDtNode = &tDtNode;
    memset(ptrDtNode,0,sizeof(XmlNode));
    /*��ȡOT�ڵ�*/
    if (RETURN_OK != GetOTNode(ptrNodeTmp,ptrDtNode))
    {
        return GET_NODE_ERR;
    }
    /*��ȡOT�ڵ��ı�*/
    if (RETURN_OK != GetChildTextByNode(ptrDtNode,MAX_NODETEXT_BUF_SIZE,ptrNodeText_Buf))
    {
        return GET_NODE_TEXT_ERR;
    }
	
    return RETURN_OK;	
}

/**********************************************************************
* �������ƣ�BYTE SetOTNodeText
* �������������������������(OT)�ڵ��ı�
* ���ʵı���
* �޸ĵı���
* ���������LPXmlNode ptrNodeIn :��ǰ�ڵ�ָ��  
* ���������BYTE *pucIfNo:�����������
* �� �� ֵ��RETURN_ERROR-ʧ�� RETURN_OK-�ɹ�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static XML_AGENT_STAT SetOTNodeText(LPXmlNode ptrNode,BYTE *pucIfNo)
{
    LPXmlNode ptrNodeTmp = NULL;/*�ڵ�ָ����ʱ����*/    
    CHAR ptrNodeText_Buf[MAX_NODETEXT_BUF_SIZE];/*�ڵ��ı�������*/    
    CHAR *ptrDelCmdName = "D";/*��������-ɾ��*/    
    BYTE bIfNoTmp;    
    
    if ((ptrNode == NULL) || (pucIfNo == NULL))
    {
        return NULL_PTR;
    }
	
    ptrNodeTmp = ptrNode;    
    /*��ȡOT�ڵ��ı�*/
    if (RETURN_OK != GetOTNodeText(ptrNodeTmp,MAX_NODETEXT_BUF_SIZE,ptrNodeText_Buf))
    {
        return GET_NODE_TEXT_ERR;
    }    
    if (strcmp(ptrNodeText_Buf,ptrDelCmdName) == 0)
    {
        bIfNoTmp = TRUE;/*ɾ������*/
    }	
    else
    {
  	 bIfNoTmp = FALSE;/*��������*/
    }
    *pucIfNo  = bIfNoTmp;   
	
    return RETURN_OK;
}


/**********************************************************************
* �������ƣ�BYTE GetCMDParaNodeCount
* ������������ȡ����ڵ��²�������
* ���ʵı���
* �޸ĵı���
* ���������LPXmlNode ptrNode:�ڵ�ָ��                      
* ���������BYTE *bNumber:��������
* �� �� ֵ��RETURN_ERROR-ʧ�� RETURN_OK-�ɹ�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static XML_AGENT_STAT GetCMDParaNodeCount(LPXmlNode ptrNode,BYTE *pucNumber)
{
    LPXmlNode ptrNodeTmp = NULL;/*�ڵ�ָ����ʱ����*/
    LPXmlNode ptrDtNode = NULL;/*Ŀ��ڵ�ָ��*/
    XmlNode tDtNode;/*Ŀ��ڵ�ṹ*/
    CHAR *ptrNodeName = "P";
    WORD32 dwNumber = 0;
    
    if ((ptrNode == NULL) || (pucNumber == NULL))
    {
        return NULL_PTR;
    }
    ptrNodeTmp = ptrNode;
    ptrDtNode = &tDtNode;
    memset(ptrDtNode,0,sizeof(XmlNode));
    /*��ȡ����ڵ�ָ��*/
    if (RETURN_OK != GetCMDNode(ptrNodeTmp,ptrDtNode))
    {
        return GET_NODE_ERR;
    }
    /*��ȡ����ڵ����*/
    if (RETURN_OK != (GetChildCountByName(ptrDtNode,ptrNodeName,&dwNumber)))
    {
        return GET_NODE_CNT_ERR;
    }
    *pucNumber = (BYTE)dwNumber;
    return RETURN_OK;
}

/**********************************************************************
* �������ƣ�XML_AGENT_STAT ParaseValidPrammer
* ����������������Ч���﷨
* ���ʵı���
* �޸ĵı���
* ���������TYPE_CUR_CMDINFO *ptCurCMDInfo
                             WORD32 dwParaNo:
* ���������BOOLEAN *bIsValidParmar
                             TYPE_CMD_PARA *ptTypeCmdPara
* �� �� ֵ�� RETURN_OK-�ɹ� ����-ʧ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2009-04-13             ������      ����
************************************************************************/
static XML_AGENT_STAT ParaseValidPrammer(TYPE_CMD_PARA *ptCmdParaInfo,TYPE_CMD_ATTR *ptCmdAttr,CHAR *ptrNodeText,BOOLEAN *pbIsValidParmar,OPR_DATA * ptrData)
{
    BOOLEAN bIsValidParmarTmp = FALSE;
    WORD16 wValue = 0;
    CMDPARA tCmdPara = {0};/*�������*/
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
        if(ptCmdAttr->bExeModeID != EXEC_PROTOCOL)/*��Э��ռ����*/
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
* �������ƣ�VOID ConvertValidPrammerToCmdPara
* ��������������Ч���﷨ת�����������
* ���ʵı���
* �޸ĵı���
* ���������TYPE_CMD_PARA *ptTypeCmdPara
                             BYTE bParaType
* ���������CMDPARA *ptCmdPara
* �� �� ֵ�� ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2009-04-13             ������      ����
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
* �������ƣ�BOOLEAN CheckIsValidPrammar
* ��������������Ƿ���Ч���﷨
* ���ʵı���
* �޸ĵı���
* ���������TYPE_CMD_PARA *ptTypeCmdPara:
                              TYPE_CMD_ATTR *ptCmdAttr:                             
* �����������
* �� �� ֵ�� TRUE/FALSE
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2009-04-13             ������      ����
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
* �������ƣ�XML_AGENT_STAT GetCurTypeCmdPara
* ������������ȡ��ǰ�����������
* ���ʵı���
* �޸ĵı���
* ���������TYPE_CUR_CMDINFO *ptCurCMDInfo:
                             DWORD dwParaNo:                             
* ���������TYPE_CMD_PARA *ptTypeCmdPara
* �� �� ֵ�� RETURN_OK-�ɹ� ����-ʧ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2009-04-13             ������      ����
************************************************************************/
static XML_AGENT_STAT GetCurTypeCmdPara(TYPE_CUR_CMDINFO *ptCurCMDInfo,DWORD dwParaNo,TYPE_CMD_PARA *ptTypeCmdPara)
{
    TYPE_CUR_CMDINFO *ptCurCMDInfoTmp = NULL;/*����ű���ʱָ��*/     
    WORD16 wCmdAttrPos = 0;/*������������*/
    DWORD dwParaPos = 0;  /*�����б�λ��*/
    DWORD dwNoParaPos = 0;/*No���������ʼλ��*/
    DWORD dwEndParaPos = 0;/*�����������λ��*/
    DWORD dwParaSum = 0;     /*��������*/
    T_OAM_Cfg_Dat * ptcfgDat = NULL; /*DAT�ű�ָ��*/

    /*�������*/
    if ((NULL == ptCurCMDInfo) || 
        (NULL == ptCurCMDInfo->cfgDat) || 
        (NULL == ptCurCMDInfo->cfgDat->tCmdAttr) ||
        (NULL == ptTypeCmdPara))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__); 
        return NULL_PTR;
    }
    /*������ʼ��*/
    ptCurCMDInfoTmp = ptCurCMDInfo;    
    ptcfgDat = ptCurCMDInfoTmp->cfgDat;
    wCmdAttrPos = ptCurCMDInfoTmp->wCmdAttrPos;
    /*����Խ����*/
    if (wCmdAttrPos >= ptCurCMDInfo->cfgDat->nCmdAttrSum)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d Array Over Flow!%d-%d ....................\n",__func__,__LINE__,wCmdAttrPos,ptCurCMDInfo->cfgDat->nCmdAttrSum); 
        return RETURN_ERROR;
    }
    /*��ȡ�����������ʼ/����λ�á���������*/
    dwParaPos = ptcfgDat->tCmdAttr[wCmdAttrPos].dwParaPos;
    dwNoParaPos = dwParaPos + ptcfgDat->tCmdAttr[wCmdAttrPos].wCmdParaCount;
    dwEndParaPos = dwParaPos + ptcfgDat->tCmdAttr[wCmdAttrPos].wAllParaCount;    
    if (ptCurCMDInfoTmp->bIsNoCmd)/*NO����*/
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
        /*����������*/
	 if (dwParaNo == ptcfgDat->tCmdPara[dwParaPos].ParaNo)
	 {
	     TYPE_CMD_PARA *ptCmdPara = &(ptcfgDat->tCmdPara[dwParaPos]);
	     /*�������Ϊ0 ��Ϊ�﷨*/
	     if ((0 == dwParaNo) &&
		  (STRUCTTYPE_PRAMMAR == ptCmdPara->bStructType))
	     {                
                TYPE_CMD_ATTR * ptCmdAttr = &(ptcfgDat->tCmdAttr[wCmdAttrPos]);
		   /*�﷨��Ч���˳�,����������ǰ�﷨*/
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
    if (0 == dwParaSum)/*û���ҵ�*/
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d Can not find command parameter! ....................",__func__,__LINE__);
        return RETURN_ERROR;
    }
    if (dwParaPos > dwEndParaPos)/*�������λ�ó���*/
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d Command parameter position overflow! ....................",__func__,__LINE__);
        return RETURN_ERROR;
    }
    memcpy(ptTypeCmdPara,&(ptCurCMDInfoTmp->cfgDat->tCmdPara[dwParaPos]),sizeof(TYPE_CMD_PARA));           
    return RETURN_OK;
}

/**********************************************************************
* �������ƣ�BYTE ConvertXmlStrToCmdPara
* �������������ַ���ת�����������ֵ
* ���ʵı���
* �޸ĵı���
* ���������TYPE_CMD_DATINFO *tDatInfo:����ű�ָ��
            CHAR * ptrNodeText:��������
            BYTE bParaNo:�������
* ���������OPR_DATA * pData:����ָ��
* �� �� ֵ��0: success  1: convert error  2: overflow
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static INT ConvertXmlStrToCmdPara(TYPE_CUR_CMDINFO *ptCurCMDInfo,CHAR * ptrNodeText,WORD32 dwParaNo,OPR_DATA * ptrData)
{	 	  
    TYPE_CUR_CMDINFO *ptCurCMDInfoTmp = NULL;/*����ű���ʱָ��*/
    CHAR * ptrNodeTextTmp = NULL;/*����������ʱָ��*/    
    TYPE_PARA *ptParaInfo = NULL;/*������Ϣ*/
    TYPE_CMD_PARA tTypeCmdPara;/*�������Ͳ���*/
    CMDPARA tCmdPara;/*�������*/
    TYPE_CMD_ATTR * ptCmdAttr;/*��������*/
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
    /*����Խ����*/
    if (ptCurCMDInfoTmp->wCmdAttrPos >= ptCurCMDInfoTmp->cfgDat->nCmdAttrSum)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC: %s,Line:%d Array Over Flow!%d-%d....................\n",__func__,__LINE__,ptCurCMDInfoTmp->wCmdAttrPos,ptCurCMDInfoTmp->cfgDat->nCmdAttrSum);
        return PARAERR_CONVERT;
    }
    ptCmdAttr = &(ptCurCMDInfoTmp->cfgDat->tCmdAttr[ptCurCMDInfoTmp->wCmdAttrPos]);
	
    /*��ȡ���������Ϣ*/
    if (RETURN_OK != GetCurTypeCmdPara(ptCurCMDInfoTmp,dwParaNo,&tTypeCmdPara))    
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC: %s,Line:%d GetCurTypeCmdPara Error!....................",__func__,__LINE__);
        return PARAERR_CONVERT;
    }
    /*�����ڴ�ָ�븴λ*/
    Oam_InptFreeBuf(ucLinkID);
    /*��Ч�﷨ת��Ϊ�������*/
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
    
    /* ����ȫ�ֱ�����ֵ */
    bSupportBlank = g_ptOamIntVar->bSupportStringWithBlank;
    /* ��OMC��̨��������пո�ı�ʶ��������ʾ������� */
    g_ptOamIntVar->bSupportStringWithBlank = FALSE;
    /*���������������ת����������*/
    nReturnVal = ConvertStrToCmdParaByParaType(ucLinkID,ptrNodeTextTmp,ptCurCMDInfo->cfgDat, ptParaInfo, ptrData->ParaNo, &tCmdPara,DM_NORMAL);
    /* �ٽ�ȫ�ֱ�����ֵ�ָ�ԭ�ȵ�ֵ */
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
* �������ƣ�BYTE GetParaNodeText
* ������������ȡ����(P)�ڵ��ı�
* ���ʵı���
* �޸ĵı���
* ���������LPXmlNode ptrNode:�ڵ�ָ��            
                              BYTE bParaNo:�������
                              WORD16 wLen:����������
* ���������CHAR *ptrNodeText_Buf:�ڵ��ı�������
* �� �� ֵ��RETURN_ERROR-ʧ�� RETURN_OK-�ɹ�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static XML_AGENT_STAT GetParaNodeText(LPXmlNode ptrNode,WORD32 dwParaNo,WORD16 wLen,CHAR *ptrNodeText_Buf)
{
    LPXmlNode ptrNodeTmp = NULL;/*�ڵ�ָ����ʱ����*/  
    XmlNode tDtNode;/*Ŀ��ڵ�ṹ����*/ 
    LPXmlNode ptrDtNode = NULL;/*Ŀ��ڵ�ָ��*/ 

    if ((ptrNode == NULL) || (wLen == 0) || (ptrNodeText_Buf == NULL))
    {
        return NULL_PTR;
    }
    ptrNodeTmp = ptrNode;
    ptrDtNode = &tDtNode;
    memset(ptrDtNode,0,sizeof(XmlNode));
    /*��ȡ�����ڵ�ָ��*/
    if (RETURN_OK != GetPARANode(ptrNodeTmp,dwParaNo,ptrDtNode))
    {
        return GET_NODE_ERR;
    }
    /*��ȡ�����ڵ��ı�*/    
    if (RETURN_OK != GetChildTextByNode(ptrDtNode,wLen,ptrNodeText_Buf))
    {
        return GET_NODE_TEXT_ERR;
    }
    return RETURN_OK;
}

/**********************************************************************
* �������ƣ�XML_AGENT_STAT SetParaNodeAttr
* ������������ȡ����(P)�ڵ�����
* ���ʵı���
* �޸ĵı���
* ���������TYPE_CMD_DATINFO *tDatInfo:����ű�ָ��
                              LPXmlNode ptrNode:�ڵ�ָ��
                              BYTE bParaNo:�������
* ���������OPR_DATA * pData:����ָ��
* �� �� ֵ��RETURN_ERROR-ʧ�� RETURN_OK-�ɹ�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static XML_AGENT_STAT SetParaNodeAttr(TYPE_CUR_CMDINFO *ptCurCMDInfo,LPXmlNode ptrNode,BYTE bParaNo,OPR_DATA * pData)
{
    LPXmlNode ptrNodeTmp = NULL;/*�ڵ�ָ����ʱ����*/     
    CHAR ptrNodeText_Buf[MAX_NODETEXT_BUF_SIZE];/*�ڵ��ı�������*/
    WORD32 dwParaNo = 0;
    
    if ((ptrNode == NULL) || (ptCurCMDInfo == NULL) || (pData == NULL))
    {
        return NULL_PTR;
    }
    ptrNodeTmp = ptrNode;    
    memset(ptrNodeText_Buf,0,MAX_NODETEXT_BUF_SIZE);
    dwParaNo = (WORD32)bParaNo;
    /*��ȡ�����ڵ��ı�*/    
    if (RETURN_OK != GetParaNodeText(ptrNodeTmp,dwParaNo,MAX_NODETEXT_BUF_SIZE,ptrNodeText_Buf))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................GetParaNodeText Error FUNC: %s,Line:%d!....................",__func__,__LINE__);
        return GET_NODE_TEXT_ERR;
    }
    /*������������ת��������д�������ȺͲ�����*/
    if (PARA_SUCCESS != ConvertXmlStrToCmdPara(ptCurCMDInfo,ptrNodeText_Buf,pData->ParaNo,pData))
    {
        return RETURN_ERROR;
    }
    return RETURN_OK;
}

/**********************************************************************
* �������ƣ�BYTE CompareChildTextByName
* �����������ȽϽڵ�����ָͬ�����ַ����Ĵ�С
* ���ʵı���
* �޸ĵı���
* ���������LPXmlNode ptrNode:ָ���ڵ�ָ��
            CHAR *ptrCompText :���Ƚ��ı�
* �����������
* �� �� ֵ��RETURN_ERROR-ʧ�� RETURN_OK-�ɹ�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static XML_AGENT_STAT CompareChildTextByName(LPXmlNode ptrNode,CHAR *ptrCompText,BOOLEAN *pbIs3GPL)
{
    LPXmlNode ptrNodeTmp = NULL;/*�ڵ�ָ����ʱ����*/
    CHAR *ptrCompTextTmp = NULL;/*���Ƚ��ı���ʱ����*/
    CHAR ptrNodeText_Buf[MAX_NODETEXT_BUF_SIZE];/*�ڵ��ı�������*/
	  
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
    if (strcmp(ptrCompTextTmp,ptrNodeText_Buf) != 0)/*���ַ�����*/
    {
        *pbIs3GPL = FALSE;
    }
    return RETURN_OK;
}

/**********************************************************************
* �������ƣ�BOOLEAN CheckNtpNodeIs3GPL
* �����������ж�NTP�ڵ����ı��Ƿ�Ϊ3GPL
* ���ʵı���
* �޸ĵı���
* ���������LPXmlNode ptrNodeIn:ָ���ڵ�ָ��            
* ���������BOOLEAN *pbIs3GPL:��Ԫ�����Ƿ�3GPL
* �� �� ֵ   ��XML_AGENT_STAT
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static XML_AGENT_STAT CheckNtpNodeTextIs3GPL(LPXmlNode ptrNodeIn,BOOLEAN *pbIs3GPL)
{
    LPXmlNode ptrNodeTmp = NULL;/*�ڵ�ָ����ʱ����*/
    LPXmlNode ptrDtNode = NULL;   /*Ŀ��ڵ�ָ��*/
    XmlNode tXmlDtNode;               /*Ŀ��ڵ�ṹ��*/
    CHAR *ptrNtpText = "3GPL";     /*������Ľڵ��ı�*/
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
* �������ƣ�BYTE ReseachTree
* �����������ݹ�������������Ҷ�Ӧ��JID
* ���ʵı���
* �޸ĵı���
* ���������WORD16 wNodePos:�������ڵ��±�
                              DWORD dwCmdId:����ID
* ���������WORD16 **ppOutNodePos:�������ڵ��±�
* �� �� ֵ��0-ʧ�� 1-�ɹ�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static XML_AGENT_STAT ReseachTree(WORD16 wNodePos,DWORD dwCmdId,WORD16 **ppOutNodePos)
{
    WORD16 wCmdAttrPos = 0;/*������������*/
    WORD16 wNodePosTmp = 0;/*�ڵ��±�*/
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
    /*����Խ����*/
    if (wNodePosTmp >= g_ptOamIntVar->dwTreeNodeSum)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d Array Over Flow!%d-%d ....................\n",__func__,__LINE__,wNodePosTmp,g_ptOamIntVar->dwTreeNodeSum); 
        return RETURN_ERROR;
    }
    ptTreeNodeTmp = &gtTreeNode[wNodePosTmp];    
    /*��ָ����*/
    if ((NULL == ptTreeNodeTmp->cfgDat) ||
        (NULL == ptTreeNodeTmp->cfgDat->tCmdAttr))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__); 
        return NULL_PTR;
    }
    wCmdAttrPos = ptTreeNodeTmp->treenode.wCmdAttrPos;
    /*����Խ����*/
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
    /*���������ӽڵ㲻Ϊ��*/
    wNodePosTmp = ptTreeNodeTmp->treenode.wNextSonPos;    
    /*����Խ����*/
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
    /*���������ֵܽڵ㲻Ϊ��*/
    wNodePosTmp = ptTreeNodeTmp->treenode.wNextNodePos;
    /*����Խ����*/
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
* �������ƣ�BYTE GetTreeNode
* ��������������ģ��ź�����Ż�ȡ�������ڵ�
* ���ʵı���
* �޸ĵı���
* ���������BYTE ucModeId:ģʽID
            DWORD dwCmdId:����ID
* ���������WORD16 *pOutNodePos:�������ڵ��±�
* �� �� ֵ��0-ʧ�� 1-�ɹ�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
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
    /*����ģʽID���Ҷ�Ӧģʽ����������λ��*/
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
    
    /*�ݹ����������,�õ�����ڵ�*/
    if (RETURN_OK != ReseachTree(wNodePosTmp,dwCmdId,&pOutNodePosTmp))
    {
        return GET_NODE_ERR;
    }
    return RETURN_OK;
}

/**********************************************************************
* �������ƣ�BYTE SetCurCMDInfo
* ������������ȡ��ǰ������Ϣ
* ���������LPXmlNode ptrNode:�ڵ�ָ��
* ���������TYPE_CUR_CMDINFO *ptCurCMDInfo:��ǰ������Ϣ
* �� �� ֵ�� 0-ʧ�� RETURN_OK-�ɹ�
* ����˵������
************************************************************************/
static XML_AGENT_STAT SetCurCMDInfo(TYPE_XMLLINKSTATE *ptLinkState)
{    
    BYTE  bModeId = 0;/*ģʽID��*/        
    DWORD dwCmdId = 0;/*����ID*/
    WORD16 wTreeNodePos = 0;
    WORD16 wCmdAttrPos = 0;   

    if (NULL == ptLinkState)
    {
        return NULL_PTR;
    }    
    
    /*��Բ�ѯģʽջ��Ϣ���⴦��*/    
    dwCmdId = ptLinkState->dwCmdId;
    if (CMD_LDB_GET_MODESTATCK == dwCmdId)
    {
        bModeId = CMD_MODE_PRIV;/*��Ȩģʽ*/
    }
    else if (IsSaveCmd(dwCmdId))
    {
        bModeId = CMD_MODE_MAINTAINANCE;/*ά��ģʽ*/
    }
    else
    {
        bModeId = ptLinkState->bModeID[ptLinkState->bModeStackTop -1];    
    }
    /*����ģʽID������ID�����������ڵ�*/
    if (RETURN_OK != GetTreeNode(bModeId,dwCmdId,&wTreeNodePos))
    {        
        return SHOW_MODESTACK_ERR;
    }
    /*����Խ����*/
    if (wTreeNodePos >= g_ptOamIntVar->dwTreeNodeSum)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d Array Over Flow!%d-%d ....................\n",__func__,__LINE__,wTreeNodePos,g_ptOamIntVar->dwTreeNodeSum); 
        return RETURN_ERROR;
    }
    
    /*��ָ����*/
    if ((NULL == gtTreeNode[wTreeNodePos].cfgDat) || 
        (NULL == gtTreeNode[wTreeNodePos].cfgDat->tCmdAttr))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__); 
        return NULL_PTR;
    }
    wCmdAttrPos = gtTreeNode[wTreeNodePos].treenode.wCmdAttrPos;
    /*����Խ����*/
    if (wCmdAttrPos >= gtTreeNode[wTreeNodePos].cfgDat->nCmdAttrSum)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d Array Over Flow!%d-%d ....................\n",__func__,__LINE__,wCmdAttrPos,gtTreeNode[wTreeNodePos].cfgDat->nCmdAttrSum); 
        return RETURN_ERROR;
    }
    /*��䵱ǰ������Ϣ*/
    ptLinkState->tCurCMDInfo.wCmdAttrPos = wCmdAttrPos;
    ptLinkState->tCurCMDInfo.cfgDat = gtTreeNode[wTreeNodePos].cfgDat;
    ptLinkState->tCurCMDInfo.wParaBeginPos = (WORD16)(ptLinkState->tCurCMDInfo.cfgDat->tCmdAttr[wCmdAttrPos].dwParaPos);
    ptLinkState->tCurCMDInfo.wParaEndPos = (WORD16)(ptLinkState->tCurCMDInfo.cfgDat->tCmdAttr[wCmdAttrPos].dwParaPos + ptLinkState->tCurCMDInfo.cfgDat->tCmdAttr[wCmdAttrPos].wAllParaCount);
    return RETURN_OK;
    
}

/**********************************************************************
* �������ƣ�BYTE SetOTNodeText
* ��������������OT�ڵ����Ƽ��ı�
* ���ʵı���
* �޸ĵı���
* ���������MSG_COMM_OAM *ptrMsgBuf��OAM��Ϣ
* ���������CHAR *ptrXmlBuf:XML�ַ���
* �� �� ֵ��RETURN_ERROR-ʧ�� RETURN_OK-�ɹ�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static XML_AGENT_STAT SetOTNode(CHAR *ptrXmlBuf,WORD16 wXmlBufLen)
{    
    CHAR *ptrXmlBufTmp = NULL;
    CHAR *pStrNodeNameHead = "<OT>";
    CHAR *pStrNodeText = "RP";/*�̶�ΪRP*/
    CHAR *pStrNodeNameEnd = "</OT>";    
    if (NULL == ptrXmlBuf)
    {
        return NULL_PTR;
    }   
    ptrXmlBufTmp = ptrXmlBuf;
    /*����OT�ڵ�ͷ*/
    strncat(ptrXmlBufTmp,pStrNodeNameHead,wXmlBufLen);
    /*����OT�ڵ��ı�*/
    strncat(ptrXmlBufTmp,pStrNodeText,wXmlBufLen);
    /*����OT�ڵ�β*/
    strncat(ptrXmlBufTmp,pStrNodeNameEnd,wXmlBufLen);
    
    return RETURN_OK;
}

/**********************************************************************
* �������ƣ�BYTE SetORNode
* ��������������OR�ڵ����Ƽ��ı�
* ���ʵı���
* �޸ĵı���
* ���������MSG_COMM_OAM *ptrMsgBuf��OAM��Ϣ
* ���������CHAR *ptrXmlBuf:XML�ַ���
* �� �� ֵ��RETURN_ERROR-ʧ�� RETURN_OK-�ɹ�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static XML_AGENT_STAT SetORNode(MSG_COMM_OAM *ptrMsgBuf,CHAR *ptrXmlBuf,WORD16 wXmlBufLen)
{
    MSG_COMM_OAM *ptrMsgBufTmp = NULL;/*OAMͨ�ýṹ����ʱָ��*/
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
    /*����OR�ڵ�ͷ*/
    strncat(ptrXmlBufTmp,pStrNodeNameHead,wXmlBufLen);
    /*����OR�ڵ��ı�*/
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
    /*����OR�ڵ�β*/
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
    /*����OR�ڵ�ͷ*/
    strncat(pXmlStr,pStrNodeNameHead,wXmlStrLen);
    /*����OR�ڵ��ı�*/
    pStrNodeText = (CHAR *)"OK";
    strncat(pXmlStr,pStrNodeText,wXmlStrLen);        
    /*����OR�ڵ�β*/
    strncat(pXmlStr,pStrNodeNameEnd,wXmlStrLen);   
    
    return RETURN_OK;
}

/**********************************************************************
* �������ƣ�BYTE SetDCNode
* ��������������DC�ڵ����Ƽ��ı�
* ���ʵı���
* �޸ĵı���
* ���������MSG_COMM_OAM *ptrMsgBuf��OAM��Ϣ
                              WORD16 wIndex:����״̬�����±�
* ���������CHAR *ptrXmlBuf:XML�ַ���
* �� �� ֵ��RETURN_ERROR-ʧ�� RETURN_OK-�ɹ�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static XML_AGENT_STAT SetDCNode(MSG_COMM_OAM *ptrMsgBuf,CHAR *ptrXmlBuf,WORD16 wXmlBufLen)
{
    MSG_COMM_OAM *ptrMsgBufTmp = NULL;/*OAMͨ�ýṹ����ʱָ��*/
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
    /*����DC�ڵ�ͷ*/
    strncat(ptrXmlBufTmp,pStrNodeNameHead,wXmlBufLen);    
    /*����DC�ڵ��ı�*/
    strncat(ptrXmlBufTmp,pStrNodeText,wXmlBufLen);
    /*����DC�ڵ�β*/
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
    /*����DC�ڵ�ͷ*/
    strncat(pXmlStr,pStrNodeNameHead,wXmlStrLen);    
    /*����DC�ڵ��ı�*/
    strncat(pXmlStr,pStrNodeText,wXmlStrLen);    
    /*����DC�ڵ�β*/
    strncat(pXmlStr,pStrNodeNameEnd,wXmlStrLen);
    
    return RETURN_OK;
}

/**********************************************************************
* �������ƣ�BYTE SetEOFNode
* ��������������EOF�ڵ����Ƽ��ı�
* ���ʵı���
* �޸ĵı���
* ���������MSG_COMM_OAM *ptrMsgBuf��OAM��Ϣ
* ���������CHAR *ptrXmlBuf:XML�ַ���
* �� �� ֵ��RETURN_ERROR-ʧ�� RETURN_OK-�ɹ�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static XML_AGENT_STAT SetEOFNode(MSG_COMM_OAM *ptrMsgBuf,CHAR *ptrXmlBuf,WORD16 wXmlBufLen)
{
    MSG_COMM_OAM *ptrMsgBufTmp = NULL;/*OAMͨ�ýṹ����ʱָ��*/
    CHAR *ptrXmlBufTmp = NULL;
    CHAR *pStrNodeNameHead = "<EOF>";
    CHAR *pStrNodeNameEnd = "</EOF>";   
	
    if ((ptrMsgBuf == NULL) || (ptrXmlBuf == NULL))
    {
        return NULL_PTR;
    }
    ptrMsgBufTmp = ptrMsgBuf;
    ptrXmlBufTmp = ptrXmlBuf;
    /*����EOF�ڵ�ͷ*/
    strncat(ptrXmlBufTmp,pStrNodeNameHead,wXmlBufLen);    
    /*����EOF�ڵ��ı�*/
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
    /*����EOF�ڵ�β*/
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
    /*����EOF�ڵ�ͷ*/
    strncat(pXmlStr,pStrNodeNameHead,wXmlStrLen);    
    /*����EOF�ڵ��ı�*/
    strncat(pXmlStr,"N",wXmlStrLen);
    /*����EOF�ڵ�β*/
    strncat(pXmlStr,pStrNodeNameEnd,wXmlStrLen);    
    
    return RETURN_OK;
}

/**********************************************************************
* �������ƣ�BYTE SetMDNode
* ��������������MD�ڵ����Ƽ��ı�
* ���ʵı���
* �޸ĵı���
* ���������MSG_COMM_OAM *ptrMsgBuf��OAM��Ϣ
                              WORD16 wIndex:����״̬�����±�
* ���������CHAR *ptrXmlBuf:XML�ַ���
* �� �� ֵ��RETURN_ERROR-ʧ�� RETURN_OK-�ɹ�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
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
    
    /*����MD�ڵ�ͷ*/
    strncat(ptrXmlBufTmp,pStrNodeNameHead,wXmlBufLen);
    /*����RSTR�ڵ�ͷ*/    
    strncat(ptrXmlBufTmp,pStrResultHead,wXmlBufLen);
    /*����RSTR�ڵ�β*/
    strncat(ptrXmlBufTmp,pStrResultEnd,wXmlBufLen);
    /*����MD�ڵ�β*/
    strncat(ptrXmlBufTmp,pStrNodeNameEnd,wXmlBufLen);
    return RETURN_OK;
}

static XML_AGENT_STAT SetMDNodeForPlan(T_Cmd_Plan_Cfg *ptrMsgBuf,TYPE_XMLLINKSTATE *ptLinkState,CHAR *ptrXmlBuf)
{
    T_Cmd_Plan_Cfg *pPlanInfo = NULL;/*OAMͨ�ýṹ����ʱָ��*/
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
    
    /*����MD�ڵ�ͷ*/
    strncat(pXmlStr,pStrNodeNameHead,wXmlStrLen);
    /*����RSTR�ڵ�ͷ*/    
    strncat(pXmlStr,pStrResultHead,wXmlStrLen);    
    /*����RSTR�ڵ��ı�*/
    strncat(pXmlStr,pStrNodeText,wXmlStrLen);  
    
    /*����RSTR�ڵ�β*/
    strncat(pXmlStr,pStrResultEnd,wXmlStrLen);     
    /*����MD�ڵ�β*/
    strncat(pXmlStr,pStrNodeNameEnd,wXmlStrLen);        
    
    return RETURN_OK;
}

static XML_AGENT_STAT SetPGNodeForPlan(T_Cmd_Plan_Cfg *ptrMsgBuf,TYPE_XMLLINKSTATE *ptLinkState,CHAR *ptrXmlBuf)
{
    T_Cmd_Plan_Cfg *pPlanInfo = NULL;/*OAMͨ�ýṹ����ʱָ��*/
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
    
    /*����PG�ڵ�ͷ*/
    strncat(pXmlStr,pStrNodeNameHead,wXmlStrLen);

    /*����PG�ڵ��ı�*/
    snprintf(pStrPlanValue, 4, "%d", pPlanInfo->ucPlanValue);
    pStrNodeText = pStrPlanValue;
    strncat(pXmlStr,pStrNodeText,wXmlStrLen);  
    
    /*����PG�ڵ�β*/
    strncat(pXmlStr,pStrNodeNameEnd,wXmlStrLen);        
    
    return RETURN_OK;
}

/**********************************************************************
* �������ƣ�BYTE GetOpJid
* ������������ȡҵ��JOB ID
* ���ʵı���
* �޸ĵı���
* ���������LPXmlNode ptrNodeIn:�ڵ�ָ��                              
* ���������JID *ptJid:JOD IDָ��
* �� �� ֵ��RETURN_ERROR-ʧ�� RETURN_OK-�ɹ�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static XML_AGENT_STAT GetOpJid(LPXmlNode ptrNodeIn,JID *ptJid)
{
    LPXmlNode ptrNodeTmp = NULL;	          /*�ڵ�ָ��*/
    T_CliAppDataEx *ptCliReg = NULL;/*��ƽ̨ע���ָ��*/
    WORD32 dwNetId = 0;                        /*��Ԫ��ʶ*/ 
    WORD16 wLoop = 0;                                /*ע����±�*/
    BYTE ucRet = RETURN_ERROR;

    /*��μ��*/
    if ((ptrNodeIn == NULL) || (ptJid == NULL))
    {
        return NULL_PTR;
    }
    
    ptrNodeTmp = ptrNodeIn;        
    /*��ȡ��Ԫ��ʶ*/
    if (RETURN_OK != SetNIDNodeText(ptrNodeTmp,&dwNetId))
    {
        return SET_NODE_TEXT_ERR;
    }
    if (dwNetId == 0)
    {
        return RETURN_ERROR;
    }    

    ucRet = RETURN_ERROR;
    /*����ȫ��������*/
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
    
    /*���������ע�����û�У���鹲���ڴ�*/
    if (RETURN_ERROR == ucRet)
    {
        T_CliAppAgtRegEx *ptOamCliReg = NULL;
        /*������ʹ�ú����Oam_UnLockRegTable�ͷ���*/
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
* �������ƣ�BYTE GetOMMJid
* ��������������ҵ��JOB ID
* ���ʵı���
* �޸ĵı���
* ���������LPXmlNode ptrNodeIn:�ڵ�ָ��                              
* �����������
* �� �� ֵ��RETURN_ERROR-ʧ�� RETURN_OK-�ɹ�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static XML_AGENT_STAT GetOMMJid(LPXmlNode ptrNodeIn)
{
    LPXmlNode ptrNodeTmp = NULL;	          /*�ڵ�ָ��*/
    WORD32 dwLinkChannel = 0;                  /*��·��*/
    /*��μ��*/
    if(NULL == ptrNodeIn)
    {
        return NULL_PTR;
    }

    ptrNodeTmp = ptrNodeIn;
    if (RETURN_OK != SetTRMNodeText(ptrNodeTmp,&dwLinkChannel))
    {
        return SET_NODE_TEXT_ERR;
    }
    /*�����±곬��*/
    if (dwLinkChannel >= MAX_SIZE_OPJID)
    {
        return RETURN_ERROR;
    }
    /*�������JID*/
    if (XOS_SUCCESS != XOS_Sender(&gt_OpJid[dwLinkChannel]))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,"XOS_Sender ERROR!\n");
        return RETURN_ERROR;
    }
    return RETURN_OK;
}

/**********************************************************************
* �������ƣ�BYTE SetDSecText
* ������������OAMͨ����Ϣ��������D���ı�
* ���ʵı���
* �޸ĵı���
* ���������MSG_COMM_OAM *ptrMsgBuf��OAM��Ϣ                              
* ���������CHAR *ptrXmlBuf:XML�ַ���
* �� �� ֵ��RETURN_ERROR-ʧ�� RETURN_OK-�ɹ�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static XML_AGENT_STAT SetDSecText(MSG_COMM_OAM *ptrMsgBuf,CHAR *ptrXmlBuf,WORD16 wXmlBufLen)
{
    MSG_COMM_OAM *ptrMsgBufTmp = NULL;/*OAMͨ�ýṹ����ʱָ��*/
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
     /*����D��ͷ*/
    strncat(ptrXmlBufTmp,pStrDSecHead,wXmlBufLen);
    /*����OT�ڵ����Ƽ��ı�*/
    if (RETURN_OK != SetOTNode(ptrXmlBufTmp,wXmlBufLen))
    {
        return SET_NODE_ERR;
    }
    /*����OR�ڵ����Ƽ��ı�*/
    if (RETURN_OK != SetORNode(ptrMsgBufTmp,ptrXmlBufTmp,wXmlBufLen))
    {
        return SET_NODE_ERR; 
    }
    /*����DC�ڵ����Ƽ��ı�*/
    if (RETURN_OK != SetDCNode(ptrMsgBufTmp,ptrXmlBufTmp,wXmlBufLen))
    {
        return SET_NODE_ERR; 
    }
    /*����EOF�ڵ����Ƽ��ı�*/
    if (RETURN_OK != SetEOFNode(ptrMsgBufTmp,ptrXmlBufTmp,wXmlBufLen))
    {
        return SET_NODE_ERR; 
    }
    /*����MD�ڵ����Ƽ��ı�*/
    if (RETURN_OK != SetMDNode(ptrXmlBufTmp,wXmlBufLen))
    {
        return SET_NODE_ERR; 
    }
    /*����D��β*/
    strncat(ptrXmlBufTmp,pStrDSecEnd,wXmlBufLen);
    
    return RETURN_OK;
}

XML_AGENT_STAT SetDSecTextForPlan(T_Cmd_Plan_Cfg *ptrMsgBuf,TYPE_XMLLINKSTATE *ptLinkState,CHAR *ptrXmlBuf)
{
    T_Cmd_Plan_Cfg *pPlanInfo = NULL;/*OAMͨ�ýṹ����ʱָ��*/
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

    /*����D��ͷ*/
    strncat(pXmlStr,pStrDSecHead,wXmlStrLen);

    /*����OT�ڵ����Ƽ��ı�*/
    if (RETURN_OK != SetOTNode(pXmlStr,wXmlStrLen))
    {
        return SET_NODE_ERR;
    }

    /*����OR�ڵ����Ƽ��ı�*/
    if (RETURN_OK != SetORNodeForPlan(pXmlStr))
    {
        return SET_NODE_ERR; 
    }

    /*����DC�ڵ����Ƽ��ı�*/
    if (RETURN_OK != SetDCNodeForPlan(pXmlStr))
    {
        return SET_NODE_ERR; 
    }

    /*����EOF�ڵ����Ƽ��ı�*/
    if (RETURN_OK != SetEOFNodeForPlan(pXmlStr))
    {
        return SET_NODE_ERR; 
    }

    /*����PG�ڵ����Ƽ��ı�*/
    if (RETURN_OK != SetPGNodeForPlan(pPlanInfo, ptLinkState, pXmlStr))
    {
        return SET_NODE_ERR; 
    }

    /*����MD�ڵ����Ƽ��ı�*/
    if (RETURN_OK != SetMDNodeForPlan(pPlanInfo,ptLinkState,pXmlStr))
    {
        return SET_NODE_ERR; 
    }

    /*����D��β*/
    strncat(pXmlStr,pStrDSecEnd,wXmlStrLen);
    
    return RETURN_OK;
}

/**********************************************************************
* �������ƣ�XML_AGENT_STAT SetRSTRNodeText
* ��������������RSTR����ı�
* ���ʵı���
* �޸ĵı���
* ���������*ptRootNode:�����
                               *ptrRSTRNodeText:�������
* �����������
* �� �� ֵ��XML_AGENT_STAT
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2011-7-29             ������      ����
************************************************************************/
static XML_AGENT_STAT SetRSTRNodeText(LPXmlNode ptRootNode,CHAR *ptrRSTRNodeText)
{
    XML_AGENT_STAT dwRet = RETURN_OK;
    XmlNode tCurNode;                                                /*��ǰ�ڵ�ṹ����*/
    XmlNode tDtNode;                                                /*Ŀ��ڵ�ṹ����*/
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
    /*��ȡ<D>��<MD>���*/
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
    /*��ȡ<RSTR>���*/
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
    /*����<RSTR>����ı�*/
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
* �������ƣ�BYTE XmlMgt2OamMgt
* ����������XML��Ϣת��ΪOAM��Ϣ
* ���ʵı���
* �޸ĵı���
* ���������LPXmlNode ptrNode:XML�ڵ�ָ��
                              TYPE_XMLLINKSTATE *ptLinkState:��·״̬
* ���������MSG_COMM_OAM *ptrMsgBuf:OAM��Ϣ��
* �� �� ֵ��RETURN_ERROR-ʧ�� RETURN_OK-�ɹ�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static XML_AGENT_STAT XmlMgt2OamMgt(LPXmlNode ptrNode,TYPE_XMLLINKSTATE *ptLinkState,MSG_COMM_OAM *ptrMsgBuf)
{	
    LPXmlNode ptrNodeTmp = NULL;/*�ڵ�ָ����ʱ����*/
    XmlNode tXmlDtNode;
    LPXmlNode ptrDtNode = NULL;/*Ŀ��ڵ�ָ��*/    
    DWORD dwPacketSize = sizeof(MSG_COMM_OAM);
    OPR_DATA *pData;
    BYTE *pCurPos  = NULL;
    BYTE bLoop = 0;  
    DWORD dwCmdId = 0;/*����ID*/
    BYTE bModeId = 0;/*ģʽID*/
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
	
    /*�����Ϣ����ֵ*/
    ptrMsgBuf->ReturnCode  = RETURNCODE_INVALID;
	
    /*����ʶ�ն˱��--����·״̬�����±�����·��*/   
    iLinkChannel = Xml_GetIndexOfLinkState(ptLinkState->dwLinkChannel);
    if (-1 == iLinkChannel)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC: %s,Line:%d GetIndex_XmlLinkState Error!....................",__func__,__LINE__);
        return RETURN_ERROR;
    }
    ptrMsgBuf->LinkChannel  = 0X00000200|iLinkChannel;
    /*���CMD�ڵ�ID����*/	
    ptrMsgBuf->CmdID = ptLinkState->dwCmdId;	    
	
    /*��Ϣ�а����Ĳ�������*/
    if (RETURN_OK != GetCMDParaNodeCount(ptrNodeTmp,&ptrMsgBuf->Number))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................GetCMDParaNodeCount Error FUNC: %s,Line:%d!....................",__func__,__LINE__);
        return GET_NODE_CNT_ERR;
    }
	
    /*��������������*/
    ptrMsgBuf->bIfNo = ptLinkState->tCurCMDInfo.bIsNoCmd; 
    /*������ģʽ*/
    ptrMsgBuf->OutputMode = OUTPUT_MODE_NORMAL;
  
    /*���������־*/
    ptrMsgBuf->CmdRestartFlag = TRUE;
  
    /*����������*/
    if (ptrMsgBuf->Number > 0)
    { 
	 /*��ȡ����ڵ�*/
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
            /*��������������*/
            if (RETURN_OK != SetParaNodeAttr_ID(ptrDtNode,bLoop,&pData->ParaNo))
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................SetParaNodeAttr_ID Error FUNC: %s,Line:%d!....................",__func__,__LINE__);
                return SET_NODE_ATTR_ERR;
            }            
            /*���ò������ݲ�����*/
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
  
    /*�����Ϣ����*/
    ptrMsgBuf->DataLen  = dwPacketSize;  

    return RETURN_OK;
}

/**********************************************************************
* �������ƣ�BYTE OamMgt2XmlMgt
* ����������OAM��Ϣת��ΪXML��Ϣ
* ���ʵı���
* �޸ĵı���
* ���������MSG_COMM_OAM *ptrMsgBuf��OAM��Ϣ
* ���������CHAR *ptrXmlBuf:XML�ַ���
* �� �� ֵ��RETURN_ERROR-ʧ�� RETURN_OK-�ɹ�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static XML_AGENT_STAT OamMgt2XmlMgt(MSG_COMM_OAM *ptrMsgBuf,TYPE_XMLLINKSTATE *ptLinkState,WORD16 wOMMMsgBufLen,WORD16 *pwMsgLen,CHAR *ptrOMMMsgBuf)
{
    XML_AGENT_STAT dwRet = RETURN_OK;
    MSG_COMM_OAM *ptrMsgBufTmp = NULL;/*OAMͨ�ýṹ����ʱָ��*/ 
    CHAR *ptrOMMMsgBufTmp = NULL;/*XML �ַ���*/
    CHAR *pRSTRNodeText = NULL;
    TYPE_XMLLINKSTATE *ptLinkStateTmp = NULL;               /*����״̬*/    
    CHAR *pStrConfHead = "<CONF>";
    CHAR *pStrConfEnd = "</CONF>";    
    WORD16 wActLen = 0;
    XmlDocPtr ptrDoc = NULL;                                         /*xml�ĵ�ָ��*/
    XmlNode tRootNode;                                                /*��ǰ�ڵ�ṹ����*/
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
    
    /*����XML��������*/
    strncat(aucXmlBuf,ptLinkStateTmp->xmlDeclText,sizeof(aucXmlBuf) - 1);    
    /*����XML���ڵ�ͷ*/
    strncat(aucXmlBuf,pStrConfHead,sizeof(aucXmlBuf) - 1);    
    /*����T���ı�*/
    strncat(aucXmlBuf,ptLinkStateTmp->xmlTSecText,sizeof(aucXmlBuf) - 1);    
    /*����D���ı�*/
    if (RETURN_OK != SetDSecText(ptrMsgBufTmp,aucXmlBuf,sizeof(aucXmlBuf) - 1))
    {
        return SET_NODE_TEXT_ERR;
    }    
    /*����XML���ڵ�β*/
    strncat(aucXmlBuf,pStrConfEnd,sizeof(aucXmlBuf) - 1);
    /*�������Դ�*/
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
    /*����XML�ĵ�*/
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
    /*����RSTR����ı�*/
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
    /*���������*/
    memcpy(ptrOMMMsgBufTmp,ptLinkStateTmp->ucComponentId,sizeof(ptLinkStateTmp->ucComponentId));
    ptrOMMMsgBufTmp = ptrOMMMsgBufTmp + sizeof(ptLinkStateTmp->ucComponentId);
    /*��ȡ�ı�*/
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
    T_Cmd_Plan_Cfg *pPlanInfo = NULL;/*��������Ϣ�ṹ����ʱָ��*/
    CHAR *pXmlStr = NULL;/*XML �ַ���*/
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
    
    /*�����齨��*/
    memcpy(pXmlStr,&ptLinkState->ucComponentId[0],sizeof(ptLinkState->ucComponentId));    
    pXmlStr = pXmlStr + sizeof(ptLinkState->ucComponentId);
    wXmlStrLen = MAX_SIZE_OMMMSG - sizeof(WORD16) -1;
    
    /*����XML��������*/
    strncat(pXmlStr,ptLinkState->xmlDeclText,wXmlStrLen);
    
    /*����XML���ڵ�ͷ*/
    strncat(pXmlStr,pStrConfHead,wXmlStrLen);
    
    /*����T���ı�*/
    strncat(pXmlStr,ptLinkState->xmlTSecText,wXmlStrLen);
    
    /*����D���ı�*/
    if (RETURN_OK != SetDSecTextForPlan(pPlanInfo,ptLinkState,pXmlStr))
    {
        return SET_NODE_TEXT_ERR;
    }
    
    /*����XML���ڵ�β*/
    strncat(pXmlStr,pStrConfEnd,wXmlStrLen);      
    *pwMsgLen = sizeof(ptLinkState->ucComponentId) + strlen(pXmlStr);
    
    return RETURN_OK;
}

/**********************************************************************
* �������ƣ�XML_AGENT_STAT SendOMMReqToOP
* ����������������������ҵ��
* ���ʵı���
* �޸ĵı���
* ���������CHAR *ptrOMMReq:OMM����������Ϣ
                              LPXmlNode ptrNode:���ڵ�
* �����������
* �� �� ֵ��   RETURN_ERROR-ʧ�� RETURN_OK-�ɹ�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static VOID SendOMMReqToOP(CHAR *ptrOMMReq,WORD16 wMsgLen,LPXmlNode ptrNodeNode)
{
    CHAR *ptrOMMReqTmp = NULL;         /*OMM����������Ϣָ��*/
    LPXmlNode ptrNodeNodeTmp = NULL;/*XML�ĵ���ָ��*/
    JID tDtJid;                                         /*����JOB ID*/

    /*�������*/
    if((NULL == ptrOMMReq) || (0 == wMsgLen) || (NULL == ptrNodeNode))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return;
    }

    /*��ʼ������*/
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
    /*��ҵ������Ϣ*/
    XOS_SendAsynMsg(EV_XML_OMMCFG_REQ, (BYTE *)ptrOMMReqTmp,wMsgLen, 0, 0,&tDtJid);
    XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d SendOMMReqToOP Successfully! ....................",__func__,__LINE__);
    return;
}

/**********************************************************************
* �������ƣ�VOID SendErrStringToOmm
* ���������������ܷ��ʹ���XML�ַ���
* ���ʵı���
* �޸ĵı���
* ���������TYPE_XMLLINKSTATE *ptLinkState:XML��·��Ϣ
                             CHAR *sOutputResult:���ؽ��
* �����������
* �� �� ֵ��   ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
VOID SendErrStringToOmm(TYPE_XMLLINKSTATE *ptLinkState,CHAR *ptrOutputResult)
{
    BOOLEAN bOR = FALSE;
    BOOLEAN bEof = TRUE;
    /*�������*/
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
* �������ƣ�XML_AGENT_STAT CreateTSecNode
* ��������������T����Ϣ����
* ���ʵı���
* �޸ĵı���
* ���������LPXmlNode ptOriRtNode:ԭ���ڵ�ָ��                             
* ���������LPXmlNode ptDstRtNode:Ŀ����ڵ�ָ��
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static XML_AGENT_STAT CreateTSecNode(const LPXmlNode ptOriRtNode,const LPXmlNode ptDstRtNode)
{
    INT iRet = 0;
    WORD16 wActualLen = 0;
    WORD32 dwNodeCount = 0;
    WORD32 dwNodeLoop = 0;
    WORD32 dwAttrCount = 0;
    WORD32 dwAttrLoop = 0;
    CHAR aucNodeText[MAX_NODETEXT_BUF_SIZE] = {0};/*�ڵ��ı�*/
    XmlNode tOriCurNode;/*ԭ�ĵ���ǰ�ڵ�*/
    XmlNode tOriSubNode;/*ԭ�ĵ��ӽڵ�*/
    XmlAttr tOriAttrNode;/*ԭ�ĵ����Խ��*/
    XmlNode tDstCurNode;/*Ŀ���ĵ���ǰ�ڵ�*/
    XmlNode tDstSubNode;/*Ŀ���ĵ��ӽڵ�*/
    XmlAttr tDstAttrNode;/*Ŀ���ĵ����Խ��*/
    /*�������*/
    if ((NULL == ptOriRtNode)  || (NULL == ptDstRtNode))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................\n",__func__,__LINE__);
        return NULL_PTR;
    }
    memset(&tOriCurNode,0,sizeof(tOriCurNode));    
    memset(&tDstCurNode,0,sizeof(tDstCurNode));
    iRet = XML_GetChild(ptOriRtNode, 0, &tOriCurNode);
    /*��ȡT���ʧ��*/
    if (XML_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_GetChild Error!%d ....................\n",__func__,__LINE__,iRet);
        return GET_NODE_ERR;
    }
    /*T �ڵ㲻����*/
    if (0 != strcmp((CHAR *)(tOriCurNode.name),(CHAR *)NAME_T_NODE))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d Node is not exsit! ....................\n",__func__,__LINE__);
        return RETURN_ERROR;
    }
    iRet = XML_InsertChild(ptDstRtNode, 0, (CHAR *)(tOriCurNode.name), &tDstCurNode);
    /*����T���ʧ��*/
    if (XML_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_InsertChild Error!%d ....................\n",__func__,__LINE__,iRet);
        return SET_NODE_ERR;
    }
    /*��ȡT������ӽڵ����*/
    dwNodeCount = XML_GetChildCount(&tOriCurNode);
    for (dwNodeLoop = 0;dwNodeLoop < dwNodeCount;dwNodeLoop++)
    {
        memset(aucNodeText,0,sizeof(aucNodeText));
        memset(&tOriSubNode,0,sizeof(tOriSubNode));
        memset(&tDstSubNode,0,sizeof(tDstSubNode));
        iRet = XML_GetChild(&tOriCurNode, dwNodeLoop, &tOriSubNode);
        /*��ȡ���ʧ��*/
        if (XML_OK != iRet)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_GetChild Error!%d ....................\n",__func__,__LINE__,iRet);
            return GET_NODE_ERR;
        }
        iRet = XML_GetChildText(&tOriSubNode, aucNodeText, sizeof(aucNodeText), &wActualLen);
        /*��ȡ�ӽڵ��ı�ʧ��*/
        if (XML_OK != iRet)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_GetChildText Error!%d ....................\n",__func__,__LINE__,iRet);
            return GET_NODE_TEXT_ERR;
        }
        iRet = XML_InsertChild(&tDstCurNode, dwNodeLoop, (CHAR *)(tOriSubNode.name), &tDstSubNode);
        /*���ɽ��ʧ��*/
        if (XML_OK != iRet)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_InsertChild Error!%d ....................\n",__func__,__LINE__,iRet);
            return SET_NODE_ERR;
        }
        iRet = XML_SetChildText(&tDstSubNode, aucNodeText);
        /*���ý���ı�ʧ��*/
        if (XML_OK != iRet)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_SetChildText Error!%d ....................\n",__func__,__LINE__,iRet);
            return SET_NODE_TEXT_ERR;
        }
        /*��ȡ�ӽ�����Ը���*/
        dwAttrCount = XML_GetAttributeCount(&tOriSubNode);
        for(dwAttrLoop = 0;dwAttrLoop < dwAttrCount;dwAttrLoop++)
        {
            memset(&tOriAttrNode,0,sizeof(tOriAttrNode));
            memset(&tDstAttrNode,0,sizeof(tDstAttrNode));
            iRet = XML_GetAttribute(&tOriSubNode, dwAttrLoop, &tOriAttrNode);
            /*��ȡ�������ʧ��*/
            if (XML_OK != iRet)
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR,
                                   "....................FUNC:%s LINE:%d XML_GetAttribute Error!%d ....................\n",__func__,__LINE__,iRet);
                return GET_NODE_ATTR_ERR;
            }
            iRet = XML_SetAttribute(&tDstSubNode, (CHAR *)(tOriAttrNode.name), (CHAR *)(tOriAttrNode.value));
            /*���ý������ʧ��*/
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
* �������ƣ�XML_AGENT_STAT CreateRSTRNode
* ��������������<RSTR>��㺯��
* ���ʵı���
* �޸ĵı���
* ���������const CHAR *pucRSTRText:<RSTR>����ı�
                             const XmlNodePtr ptCurNode:��ǰ�ڵ�ָ��
* �����������
* �� �� ֵ��   ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static XML_AGENT_STAT CreateRSTRNode(const CHAR *pucRSTRText,const LPXmlNode ptCurNode)
{
    WORD16 wUTF8BufLen = 0;
    WORD32 iRet = 0;
    XmlNode tDstCurNode;/*Ŀ���ĵ���ǰ���*/
    CHAR *pucUTF8Buf = NULL;
	
    /*�������*/
    if ((NULL == ptCurNode)  || 
        (NULL == pucRSTRText))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................\n",__func__,__LINE__);
        return NULL_PTR;
    }
    
    memset(&tDstCurNode,0,sizeof(tDstCurNode));
    iRet = XML_InsertChild((LPXmlNode)ptCurNode, 0, (CHAR *)NAME_RSTR_NODE, &tDstCurNode);
    /*����<RSTR>ʧ��*/
    if (XML_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                            "....................FUNC:%s LINE:%d XML_InsertChild Error!%d ....................\n",__func__,__LINE__,iRet);
        return SET_NODE_ERR;
    }
    wUTF8BufLen = strlen(pucRSTRText);
    wUTF8BufLen *= 2;
    pucUTF8Buf = (CHAR *)XOS_GetUB(wUTF8BufLen);
    /*���붯̬�ڴ�ʧ��*/
    if (NULL == pucUTF8Buf)
        {
            XOS_SysLog(OAM_CFG_LOG_NOTICE,
                       "....................FUNC:%s LINE:%d XOS_GetUB Error! ....................",__func__,__LINE__);
        return RETURN_ERROR;
        }
    memset(pucUTF8Buf,0,wUTF8BufLen);
            /*����ת��*/
    if (!Gb2312ToUtf8(pucRSTRText, wUTF8BufLen,pucUTF8Buf))
            {
                XOS_SysLog(OAM_CFG_LOG_NOTICE,
                       "....................FUNC:%s LINE:%d Gb2312ToUtf8 Error! ....................",__func__,__LINE__);            
        OAM_RETUB(pucUTF8Buf);
        return RETURN_ERROR;
    }
    iRet = XML_SetChildText(&tDstCurNode, pucUTF8Buf);
    /*���ý���ı�ʧ��*/
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
* �������ƣ�XML_AGENT_STAT CreateDSecNode
* ��������������D����Ϣ����
* ���ʵı���
* �޸ĵı���
* ���������const CHAR *pucORText:OR����ı�
                             const CHAR *pucEOFText:EOF����ı�
                             const CHAR *pucRSTRText:RSTR����ı�
                             const LPXmlNode ptOriRtNode:ԭ�ĵ����ڵ�ָ��
* ���������LPXmlNode ptDstRtNode:Ŀ���ĵ����ڵ�ָ��
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
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
    XmlNode tOriCurNode;/*ԭ�ĵ���ǰ�ڵ�*/
    XmlNode tOriSubNode;/*ԭ�ĵ��ӽڵ�*/
    XmlNode tDstCurNode;/*Ŀ���ĵ���ǰ�ڵ�*/
    XmlNode tDstSubNode;/*Ŀ���ĵ��ӽڵ�*/

    /*�������*/
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
    /*��ȡD���ʧ��*/
    if (XML_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_GetChild Error!%d ....................\n",__func__,__LINE__,iRet);
        return GET_NODE_ERR;
    }
    /*D �ڵ㲻����*/
    if (0 != strcmp((CHAR *)(tOriCurNode.name),NAME_D_NODE))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d Node is not exsit! ....................\n",__func__,__LINE__);
        return GET_NODE_TEXT_ERR;
    }
    iRet = XML_InsertChild(ptDstRtNode, 1, (CHAR *)(tOriCurNode.name), &tDstCurNode);
    /*����D���ʧ��*/
    if (XML_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_InsertChild Error!%d ....................\n",__func__,__LINE__,iRet);
        return SET_NODE_ERR;
    }
    /*��ȡD������ӽڵ����*/
    dwNodeCount = XML_GetChildCount(&tOriCurNode);
    for (dwNodeLoop = 0;dwNodeLoop < dwNodeCount;dwNodeLoop++)
    {
        memset(&tOriSubNode,0,sizeof(tOriSubNode));
        memset(&tDstSubNode,0,sizeof(tDstSubNode));
        iRet = XML_GetChild(&tOriCurNode, dwNodeLoop, &tOriSubNode);
        /*��ȡ���ʧ��*/
        if (XML_OK != iRet)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_GetChild Error!%d ....................\n",__func__,__LINE__,iRet);
            return GET_NODE_ERR;
        }
        iRet = XML_InsertChild(&tDstCurNode, dwNodeLoop, (CHAR *)(tOriSubNode.name), &tDstSubNode);
        /*���ɽ��ʧ��*/
        if (XML_OK != iRet)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_InsertChild Error!%d ....................\n",__func__,__LINE__,iRet);
            return SET_NODE_ERR;
        }
        /*���ý���ı�*/
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
        /*���ý���ı�ʧ��*/
        if (RETURN_OK != iRet)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_SetChildText %s Error!%d ....................\n",__func__,__LINE__,tOriSubNode.name,iRet);
            return SET_NODE_TEXT_ERR;
        }
    }
    memset(&tDstSubNode,0,sizeof(tDstSubNode));
    iRet = XML_InsertChild(&tDstCurNode, 4, "PG", &tDstSubNode);
    /*���ɽ��ʧ��*/
    if (XML_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                           "....................FUNC:%s LINE:%d XML_InsertChild Error!%d ....................\n",__func__,__LINE__,iRet);
        return SET_NODE_ERR;
    }
    iRet = XML_SetChildText(&tDstSubNode, "0");
    /*���ý���ı�ʧ��*/
    if (XML_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                           "....................FUNC:%s LINE:%d XML_SetChildText Error!%d ....................\n",__func__,__LINE__,iRet);
        return SET_NODE_ERR;
    }
    return RETURN_OK;
}

/**********************************************************************
* �������ƣ�XML_AGENT_STAT CreateDstXmlDoc
* ��������������Ŀ���ĵ�
* ���ʵı���
* �޸ĵı���
* ���������const CHAR *pucORText:OR����ı�
                             const CHAR *pucEOFText:EOF����ı�
                             const CHAR *pucRSTRText:RSTR����ı�
                             const LPXmlNode pOriRtNode:ԭ�ĵ����ڵ�
* ���������XmlDocPtr *pptDstXmlDoc:Ŀ���ĵ��ڵ�
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static XML_AGENT_STAT CreateDstXmlDoc(const CHAR *pucORText,
                                                                    const CHAR *pucEOFText,
                                                                    const CHAR *pucRSTRText,
                                                                    const LPXmlNode pOriRtNode,
                                                                    XmlDocPtr *pptDstXmlDoc)
{
    WORD32 dwRet = 0;
    XmlNode tDstRtNode;
    XmlDocPtr ptDstXmlDoc = NULL;/*Ŀ���ĵ�ָ��*/

    /*�������*/
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
    /*����Ŀ���ĵ�ʧ��*/
    if (XML_OK != dwRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_NewDoc Error!%d ....................\n",__func__,__LINE__,dwRet);
        return NEW_XML_ERR;
    }
    dwRet = XML_GetRootNode(ptDstXmlDoc, &tDstRtNode);
    /*��ȡĿ����ڵ�ʧ��*/
    if (XML_OK != dwRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_GetRootNode Error!%d ....................\n",__func__,__LINE__,dwRet);
        XML_FreeXml(ptDstXmlDoc);
        return GET_NODE_ERR;
            }
    dwRet = CreateTSecNode(pOriRtNode,&tDstRtNode);
    /*����T����Ϣʧ��*/
    if (RETURN_OK != dwRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d CreateTSecNode Error!%d ....................\n",__func__,__LINE__,dwRet);
        XML_FreeXml(ptDstXmlDoc);
        return SET_NODE_ERR;
        }
    dwRet = CreateDSecNode(pucORText,pucEOFText,pucRSTRText,pOriRtNode,&tDstRtNode);
    /*����D����Ϣʧ��*/
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
* �������ƣ�XML_AGENT_STAT SendAckMsg2OMM
* ��������������Ӧ����Ϣ������
* ���ʵı���
* �޸ĵı���
* ���������const CHAR *pucMsg:��Ϣ��
            WORD32 dwMsgId:��Ϣ��
            JID *ptOmmJid:���ܶ�JID
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static XML_AGENT_STAT SendAckMsg2OMM(const CHAR *pucMsg,
                                    WORD32 dwMsgId,
                                    JID *ptOmmJid)
{
    WORD16 wMsgLen = 0;
    WORD32 dwRet = 0;
    TYPE_XMLMODEL *ptXmlModel = NULL;
    
    /*�������*/
    if ((NULL == pucMsg) ||
        (NULL == ptOmmJid))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................\n",__func__,__LINE__);
        return NULL_PTR;
    }
    ptXmlModel = (TYPE_XMLMODEL *)pucMsg;
    wMsgLen = sizeof(ptXmlModel->wComponentId) + strlen(ptXmlModel->ucXML) + 1;
    /*��ӡXML�ַ���*/
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
    /*�����ܷ���������ֹӦ��ʧ��*/
    if (XOS_SUCCESS != dwRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                               "....................FUNC:%s LINE:%d XOS_SendAsynMsg Error,Error code %d! ....................",__func__,__LINE__,dwRet);
        return RETURN_ERROR;
    }
    return RETURN_OK;
}

/**********************************************************************
* �������ƣ�XML_AGENT_STAT ConstructAckMsg2OMM
* ��������������Ӧ����Ϣ������
* ���ʵı���
* �޸ĵı���
* ���������const CHAR *pucORText:OR����ı�
                             const CHAR *pucEOFText:EOF����ı�
                             const CHAR *pucRSTRText:RSTR����ı�
                             const CHAR *pucComponentId:�����
                             const LPXmlNode pOriRtNode:Դ�ĵ����ڵ�
                             WORD32 dwMsgId:��Ϣ��
                             JID *ptOmmJid:���ܶ�JID
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
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
    /*�������*/
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
    /*�����ĵ�ʧ��*/
    if (RETURN_OK != dwRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d CreateDstXmlDoc %d Error! ....................\n",__func__,__LINE__,dwRet);
        return NEW_XML_ERR;
    }
    pucDstDocTextBuf = XML_GetTextForUTF(ptDstDoc, pucComponentId);
    /*��ȡĿ���ĵ��ı�ʧ��*/
    if (NULL == pucDstDocTextBuf)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                            "....................FUNC:%s LINE:%d XML_GetTextForUTF Error ....................\n",__func__,__LINE__);
        /*�ͷ���Դ*/
        XML_FreeXml(ptDstDoc);
        return RETURN_ERROR;
    }
    dwRet = SendAckMsg2OMM(pucDstDocTextBuf, dwMsgId, ptOmmJid);
    /*�����ܷ���Ӧ����Ϣʧ��*/
    if (RETURN_OK != dwRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d SendAckMsg2OMM %d Error! ....................\n",__func__,__LINE__,dwRet);
        /*�ͷ���Դ*/
        XML_FreeXml(ptDstDoc);
        OAM_RETUB(pucDstDocTextBuf);
        return RETURN_ERROR;
    }
    /*�ͷ���Դ*/
    XML_FreeXml(ptDstDoc);
    OAM_RETUB(pucDstDocTextBuf);
    return dwRet;
}

/**********************************************************************
* �������ƣ�CHAR *GetRSTRText
* ������������ȡ��������ַ���
* ���ʵı���
* �޸ĵı���
* ���������WORD32 dwCode:������
* �����������
* �� �� ֵ��CHAR *
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static CHAR *GetRSTRText(WORD32 dwCode)
{    
    CHAR *pucCeaseCmdRslt = NULL;
    
    /*OAM ȫ�ֱ���Ϊ��ָ��*/
    if (NULL == g_ptOamIntVar)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                   "....................FUNC:%s LINE:%d g_ptOamIntVar is null ptr! ....................",__func__,__LINE__);
        return NULL;
    }
    /*��ȡ��������ַ���*/
    switch(dwCode)
    {      
        case RETURN_OK:/*�����ͳɹ�*/
        {
            pucCeaseCmdRslt = ReadMapValueOfOamDat(MAP_TYPE_XMLADAPTER,MAP_KEY_SUCCESS_SENDMSG);
            break;
        }
        case NO_EXECUTE_CMD_ON_LINKCHANNEL:/*��ǰ��·û������ִ��*/
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
* �������ƣ�VOID ProcErrPlatCfgMsg
* �����������������ƽ̨������Ϣ
* ���ʵı���
* �޸ĵı���
* ���������const CHAR *ptrComponentId:���������
                             const LPXmlNode ptOriRtNode:Դ�ĵ����ڵ�
                             const CHAR *ptrResult:���ؽ��
                             JID *ptDtJid:����JID
* �����������
* �� �� ֵ��   ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static VOID ProcErrPlatCfgMsg(const CHAR *ptrComponentId,const LPXmlNode ptOriRtNode,const CHAR *ptrOutputResult,JID *ptDtJid)
{
    WORD32 dwRet = 0;
    CHAR *pucORText = "";
    CHAR *pucEOFText = "";
    

    /*�������*/
    if ((NULL == ptrComponentId) ||
        (NULL == ptOriRtNode) ||
        (NULL == ptrOutputResult) ||
        (NULL == ptDtJid))
    {
         XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
         return;
    }
    /*<OR>�ı�*/
    pucORText = TEXT_OR_NODE_NOK;
    /*<EOF>�ı�*/
    pucEOFText = TEXT_EOF_NODE_Y;

    dwRet = ConstructAckMsg2OMM(pucORText, 
                                                    pucEOFText, 
                                                    ptrOutputResult,
                                                    (const CHAR *)ptrComponentId,
                                                    (const LPXmlNode)ptOriRtNode, 
                                                    EV_XML_OMMCFG_ACK, 
                                                    ptDtJid);
    /*����Ӧ����Ϣʧ��*/
    if (RETURN_OK != dwRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d ConstructAckMsg2OMM %d Error!....................",__func__,__LINE__,dwRet);
        return;
    }
    return;
}

/**********************************************************************
* �������ƣ�VOID SendStringToOmm
* ���������������ܷ���XML�ַ���
* ���ʵı���
* �޸ĵı���
* ���������TYPE_XMLLINKSTATE *ptLinkState:XML��·��Ϣ
                             CHAR *sOutputResult:���ؽ��
                             BOOLEAN bOR:�������
                             BOOLEAN bEof:�Ƿ������
* �����������
* �� �� ֵ��   ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
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
    WORD16 wMsgLen = 0;                    /*������Ϣ����*/   

    /*�������*/
    if ((NULL == ptLinkState) || (NULL == ptrOutputResult))
    {
        XOS_SysLog(OAM_CFG_LOG_NOTICE,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return;
    }    
    memset(ptrOmmMsgBuf,0,MAX_SIZE_OMMMSG);
    /*�����齨��*/
    memcpy(ptrOmmMsgBuf,(CHAR *)&ptLinkState->ucComponentId[0],sizeof(ptLinkState->ucComponentId));
    wMsgLen += sizeof(ptLinkState->ucComponentId);   
    ptrXmlMsgBuf = ptrOmmMsgBuf + wMsgLen;
    /*����XML��������*/
    wMsgLen += strlen(ptLinkState->xmlDeclText); 
    strncat(ptrXmlMsgBuf,&ptLinkState->xmlDeclText[0],MAX_SIZE_OMMMSG-2);       
    /*����XML���ڵ�ͷ*/
    wMsgLen += strlen(ptrConfHead);
    strncat(ptrXmlMsgBuf,ptrConfHead,MAX_SIZE_OMMMSG-2);     
    /*����T���ı�*/
    wMsgLen += strlen(ptLinkState->xmlTSecText);    
    strncat(ptrXmlMsgBuf,&ptLinkState->xmlTSecText[0],MAX_SIZE_OMMMSG-2);    
    /*����D��ͷ*/
    wMsgLen += strlen(ptrDSecHead);    
    strncat(ptrXmlMsgBuf,ptrDSecHead,MAX_SIZE_OMMMSG-2);        
    /*����OT�ڵ��ı�*/
    wMsgLen += strlen(ptrOTNodeText);   
    strncat(ptrXmlMsgBuf,&ptrOTNodeText[0],MAX_SIZE_OMMMSG-2);         
    /*����OR�ڵ��ı�*/
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
    /*����DC�ڵ��ı�*/
    wMsgLen += strlen(ptrDCNodeText);   
    strncat(ptrXmlMsgBuf,&ptrDCNodeText[0],MAX_SIZE_OMMMSG-2);             
    /*����EOF�ڵ��ı�*/
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
    
    /*����MD�ڵ�ͷ�ı�*/
    wMsgLen += strlen(ptrMDNodeHeadText);
    strncat(ptrXmlMsgBuf,&ptrMDNodeHeadText[0],MAX_SIZE_OMMMSG-2);    
   
    /*����ȡ�Ļ��Դ�ת��ΪUTF-8��ʽ*/    
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
            /*����ת��*/
            if (!Gb2312ToUtf8(ptrOutputResult, iBufLen,ptrUTF8Buf))
            {
                XOS_SysLog(OAM_CFG_LOG_NOTICE,
                       "....................FUNC:%s LINE:%d Gb2312ToUtf8 Error! ....................",__func__,__LINE__);            
            }
            else
            {
                /*�������ؽ��*/
                wMsgLen += strlen(ptrUTF8Buf);
                strncat(ptrXmlMsgBuf,ptrUTF8Buf,MAX_SIZE_OMMMSG-2);                
            }
            OAM_RETUB(ptrUTF8Buf);
        }
    }
   /*����MD�ڵ�β�ı�*/
   wMsgLen += strlen(ptrMDNodeEndText);
   strncat(ptrXmlMsgBuf,&ptrMDNodeEndText[0],MAX_SIZE_OMMMSG-2);
    /*����D��β*/
    wMsgLen += strlen(ptrDSecEnd);   
    strncat(ptrXmlMsgBuf,ptrDSecEnd,MAX_SIZE_OMMMSG-2);    
    /*����XML��β��ͷ*/
    wMsgLen += strlen(ptrConfEnd);       
    strncat(ptrXmlMsgBuf,ptrConfEnd,MAX_SIZE_OMMMSG-2);    
    if (bDebugPrint)
    {
        PrintMsgXml((TYPE_XMLMODEL *)ptrOmmMsgBuf, "MSG_XMLAGNET_TO_OMM");
    }
    /*��OMM������Ϣ*/
    XOS_SendAsynMsg(EV_XML_OMMCFG_ACK,(BYTE *)ptrOmmMsgBuf,wMsgLen, 0, 0, &(ptLinkState->tDtJid));
    XOS_SysLog(OAM_CFG_LOG_NOTICE,
                       "....................FUNC:%s LINE:%d SendOMMAckToOmm Successfully! ....................",__func__,__LINE__);
    return;
}

/**********************************************************************
* �������ƣ�VOID Oam_InitSaveLink
* ������������ʼ��������·
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ��   ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2009-4-7             ������      ����
************************************************************************/
static VOID Oam_InitSaveLink(VOID)
{
    TYPE_LINK_STATE *pLinkState = &gt_SaveLinkState;
    memset(&gt_SaveLinkState,0,sizeof(gt_SaveLinkState));
    Oam_LinkInitializeForNewConn(pLinkState);
    /* ��ʼ��Ϊ�û�ģʽ */
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
* �������ƣ�VOID Xml_ConstructTelnetSaveCmd
* ��������������Telnet��������
* ���ʵı���
* �޸ĵı���
* ���������MSG_COMM_OAM *ptSendMsg
* �����������
* �� �� ֵ��   ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2009-04-08             ������      ����
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
    /*�������*/
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
    if (0 == wValue)/*�������*/
    {
        strncat(strCmdLine,strMSaveType,sizeof(strCmdLine));
    }
    else if (1 == wValue)/*�������*/
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
* �������ƣ�VOID Xml_ExecSaveCmd
* ������������������
* ���ʵı���
* �޸ĵı���
* ���������MSG_COMM_OAM *ptSendMsg:���͵�OAM��Ϣ��
* �����������
* �� �� ֵ��   ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static VOID Xml_ExecSaveCmd(TYPE_XMLLINKSTATE *ptLinkState)
{
    CHAR *ptrOutputResult = NULL;
    static BYTE buffer[MAX_SEND_STRING_LEN + MSG_HDR_SIZE_OUT] = {0};
    TYPE_TELNET_MSGHEAD  *pTelMsg = NULL;    
    JID tSelfJid = {0};    
    WORD wDataLen = 0;
    
    /*�������*/
    if ((NULL == ptLinkState) || (NULL == ptLinkState->ptLastSendMsg))
    {
        XOS_SysLog(OAM_CFG_LOG_NOTICE,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return;
    }    
    /*����Ƿ�����·��ִ���������*/
    if (CheckLinkIsExecutingCmd())
    {
        ptrOutputResult = ReadMapValueOfOamDat(MAP_TYPE_XMLADAPTER,MAP_KEY_CONFLICT_EXECUTECMD);
        SendErrStringToOmm(ptLinkState, ptrOutputResult);
        XOS_SysLog(OAM_CFG_LOG_NOTICE,
                       "....................FUNC:%s LINE:%d The other link is executing command! ....................",__func__,__LINE__);
        return;
    }
    /*��ʼ��savelinkstate��Ϣ*/
    Oam_InitSaveLink();
    /*����Telnet����������Ϣ*/
    pTelMsg = (TYPE_TELNET_MSGHEAD *)(buffer);
    if (!Xml_ConstructTelnetSaveCmd(ptLinkState->ptLastSendMsg,pTelMsg))
    {
        /*�����ܷ��͹����������ʧ��*/
	 ptrOutputResult = ReadMapValueOfOamDat(MAP_TYPE_XMLADAPTER,MAP_KEY_ERR_CONSTRUCTSAVECMD);
        SendErrStringToOmm(ptLinkState, ptrOutputResult);
        XOS_SysLog(OAM_CFG_LOG_NOTICE,
                       "....................FUNC:%s LINE:%d Xml_ConstructTelnetSaveCmd Error! ....................",__func__,__LINE__);
        return;
    }    
    wDataLen = sizeof(TYPE_TELNET_MSGHEAD) + pTelMsg->wDataLen - sizeof(BYTE);
    XOS_GetSelfJID(&tSelfJid);
    /*����XMLִ�ж�ʱ��*/
    Xml_SetExeTimer(ptLinkState);
    /*������·״̬Ϊ����״̬*/
    XML_LinkSetCurRunState(ptLinkState, pec_MsgHandle_EXECUTE);
    XOS_SendAsynMsg(OAM_MSG_XMLAGNET_TO_INTERPRET, (BYTE *)buffer, (WORD16)wDataLen,
                    0, 0, &tSelfJid);
    return;
}

/**********************************************************************
* �������ƣ�XML_AGENT_STAT SendOMMReqToPlat
* ����������������������ƽ̨
* ���ʵı���
* �޸ĵı���
* ���������TYPE_XMLMODEL *ptXmlModel:XMLģ��
                              LPXmlNode ptrNode:���ڵ�
* �����������
* �� �� ֵ��   RETURN_ERROR-ʧ�� RETURN_OK-�ɹ�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static VOID SendOMMReqToPlat(TYPE_XMLLINKSTATE *ptLinkState,LPXmlNode ptrNodeNode)
{
    CHAR *ptrOutputResult = NULL;
    MSG_COMM_OAM *ptOAMMsgBuf = NULL;              /*OAMͨ����Ϣ�ṹ��*/    
    LPXmlNode ptrNodeNodeTmp = NULL;                   /*XML�ĵ����ڵ�*/    
    TYPE_XMLLINKSTATE *ptLinkStateTmp = NULL;     /*XML��·��Ϣ*/
    JID *ptDtJid = NULL;                                             /*����JOB ID*/
    BYTE bExePos = 0;                                               /*����ִ��λ��*/    
    INT iRet = 0;                                                        /*����״̬*/
    /*�������*/
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

    /*��ʼ������*/    
    ptrNodeNodeTmp = ptrNodeNode;   
    ptLinkStateTmp = ptLinkState;

    /*����Խ����*/
    if (ptLinkStateTmp->tCurCMDInfo.wCmdAttrPos >= ptLinkStateTmp->tCurCMDInfo.cfgDat->nCmdAttrSum)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d Array Over Flow!%d-%d ....................\n",__func__,__LINE__,ptLinkStateTmp->tCurCMDInfo.wCmdAttrPos,ptLinkStateTmp->tCurCMDInfo.cfgDat->nCmdAttrSum);
        return;
    }
    bExePos = ptLinkStateTmp->tCurCMDInfo.cfgDat->tCmdAttr[ptLinkStateTmp->tCurCMDInfo.wCmdAttrPos].bExePos;
        
    /*�����ڴ�*/
    ptOAMMsgBuf = (MSG_COMM_OAM *)XOS_GetUB(MAX_OAMMSG_LEN);
    if (NULL == ptOAMMsgBuf)
    {
        /*�����ܷ��������ڴ�ʧ��*/
	 ptrOutputResult = ReadMapValueOfOamDat(MAP_TYPE_XMLADAPTER,MAP_KEY_ERR_GETUB);
        SendErrStringToOmm(ptLinkStateTmp, ptrOutputResult);        
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XOS_GetUB Error! ....................",__func__,__LINE__);
        return;
    }
    memset(ptOAMMsgBuf,0,MAX_OAMMSG_LEN);
    /*XML�ַ���ת��ΪOAMͨ�ýṹ��*/
    iRet = XmlMgt2OamMgt(ptrNodeNodeTmp,ptLinkStateTmp,ptOAMMsgBuf);    
    if (RETURN_OK != iRet)
    {
        /*�����ܷ�����Ϣ����ʧ��*/
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
    /*��ȡ����JOB ID*/
    ptDtJid = &(ptLinkStateTmp->tCurCMDInfo.cfgDat->tJid);
    
    if (NULL == ptDtJid)
    {
        /*�����ܷ��ͻ�ȡĿ��JIDʧ��*/
	 ptrOutputResult = ReadMapValueOfOamDat(MAP_TYPE_XMLADAPTER,MAP_KEY_ERR_GETAPPJID);
        SendErrStringToOmm(ptLinkStateTmp, ptrOutputResult);	 
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        OAM_RETUB(ptOAMMsgBuf);
        return;
    }    
    /*���Ͷ�ִ��*/
    if((ptDtJid->dwJno == g_ptOamIntVar->jidSelf.dwJno) || (EXEPOS_INTERPRETATION == bExePos))
    {
        if(IsSaveCmd(ptOAMMsgBuf->CmdID))
        {
             /*������Ϣ���汾�η�����Ϣ�ṹ*/
            memset(ptLinkStateTmp->ptLastSendMsg,0,MAX_OAMMSG_LEN);
    	     memcpy(ptLinkStateTmp->ptLastSendMsg,ptOAMMsgBuf,ptOAMMsgBuf->DataLen);
            memcpy(ptOAMMsgBuf->LastData,ptLinkStateTmp->LastData,sizeof(ptOAMMsgBuf->LastData));
            memcpy(ptOAMMsgBuf->BUF,ptLinkStateTmp->BUF,sizeof(ptOAMMsgBuf->BUF));                
            Xml_ExecSaveCmd(ptLinkStateTmp);                
        }
	 else
        {
            /*������·״̬Ϊ����״̬*/
            XML_LinkSetCurRunState(ptLinkState, pec_MsgHandle_EXECUTE);        
            DisposeInnerCmdForXmlAgent(ptOAMMsgBuf);
        }    
    }
    else
    {
        /*���汾�η�����Ϣ�ṹ*/
        memset(ptLinkStateTmp->ptLastSendMsg,0,MAX_OAMMSG_LEN);
	 memcpy(ptLinkStateTmp->ptLastSendMsg,ptOAMMsgBuf,ptOAMMsgBuf->DataLen);
        memcpy(ptOAMMsgBuf->LastData,ptLinkStateTmp->LastData,sizeof(ptOAMMsgBuf->LastData));
        memcpy(ptOAMMsgBuf->BUF,ptLinkStateTmp->BUF,sizeof(ptOAMMsgBuf->BUF));                
            /*����XMLִ�ж�ʱ��*/
            Xml_SetExeTimer(ptLinkState);

            /* ����ͣ����ý�����Ϣ�ϱ��ȴ���ʱ�� */
            XML_KillCmdPlanTimer(ptLinkState);
            XML_SetCmdPlanTimer(ptLinkState);

            /*������·״̬Ϊ����״̬*/
            XML_LinkSetCurRunState(ptLinkState, pec_MsgHandle_EXECUTE);
	     /*��3G Plat������Ϣ*/
	      XOS_SendAsynMsg(EV_OAM_CFG_CFG, (BYTE *)ptOAMMsgBuf, ptOAMMsgBuf->DataLen, 0, 0,ptDtJid);
	      XOS_SysLog(OAM_CFG_LOG_NOTICE,
                       "....................FUNC:%s LINE:%d SendOMMReqToPlat Successfully! ....................",__func__,__LINE__);
	  } 
    OAM_RETUB(ptOAMMsgBuf);
    return;
}


/**********************************************************************
* �������ƣ�XML_AGENT_STAT SendOPAckToOMM
* ��������������ҵ��Ӧ������
* ���ʵı���
* �޸ĵı���
* ���������CHAR *ptrOPAck:ҵ��Ӧ����Ϣ��
                              WORD16 wMsgLen:ҵ��Ӧ����Ϣ����
                              LPXmlNode ptRootNode:���ڵ�ָ��
* �����������
* �� �� ֵ��   RETURN_ERROR-ʧ�� RETURN_OK-�ɹ�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static VOID SendOPAckToOMM(CHAR *ptrOPAck,WORD16 wMsgLen,LPXmlNode ptRootNode)
{
    LPXmlNode ptRootNodeTmp = NULL; /*���ڵ�ṹ����*/    
    CHAR *ptrOPAckTmp = NULL;          /*ҵ��Ӧ����Ϣ��*/
    WORD32 dwLinkChannel = 0;            /*��·��*/    

    /*�������*/
    if ((NULL == ptrOPAck) || (0 == wMsgLen) || (NULL == ptRootNode))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return;
    }

    /*��ʼ������*/
    ptrOPAckTmp = ptrOPAck;
    ptRootNodeTmp = ptRootNode;
	
    /*��ȡTRM�ڵ�ֵ*/
    if(RETURN_OK != SetTRMNodeText(ptRootNodeTmp,&dwLinkChannel))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d SetTRMNodeText Error! ....................",__func__,__LINE__);
        return;
    }
    /*�ж������±��Ƿ����*/
    if (dwLinkChannel >= MAX_SIZE_OPJID)
    {
    	XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d LinkChannel Error! ....................",__func__,__LINE__);
       return;
    }
    /*��OMM����Ӧ����Ϣ*/
    XOS_SendAsynMsg(EV_XML_OMMCFG_ACK, (BYTE *)ptrOPAckTmp,wMsgLen, 0, 0, &gt_OpJid[dwLinkChannel]);
    XOS_SysLog(OAM_CFG_LOG_NOTICE,
                     "....................FUNC:%s LINE:%d SendOPAckToOMM Successfully! ....................",__func__,__LINE__);    
    return;
    
}

/**********************************************************************
* �������ƣ�XML_AGENT_STAT SendPLATAckToOMM
* ��������������ƽ̨Ӧ������
* ���ʵı���
* �޸ĵı���
* ���������MSG_COMM_OAM *ptOamMsgBuf:OAMͨ����Ϣ�� 
* ���������TYPE_XMLLINKSTATE **pptLinkState:XML��·״̬
* �� �� ֵ��   RETURN_ERROR-ʧ�� RETURN_OK-�ɹ�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
VOID SendPLATAckToOMM(MSG_COMM_OAM *ptOamMsgBuf)
{
    CHAR *ptrOutputResult = NULL;
    MSG_COMM_OAM *ptOamMsgBufTmp = NULL;/*OAMͨ����Ϣ�� */
    TYPE_XMLLINKSTATE *ptLinkState = NULL;     /*XML��·״̬*/    
    CHAR *pOMMMsgBuf = NULL;   /*OMMӦ����Ϣ��*/
    WORD16 wLinkStateIndex = 0;                     /*��·��*/
    WORD16 wMsgLen = 0;                                 /*OMMӦ����Ϣ�峤��*/    
    INT iRet = 0;                                                /*����״̬*/

    /*�������*/
    if (NULL == ptOamMsgBuf)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__); 
        return;
    }

    /*��ʼ������*/
    ptOamMsgBufTmp = ptOamMsgBuf;       
    
    /*������·�Ż�ȡ��·״̬*/
    wLinkStateIndex = (WORD16)(ptOamMsgBufTmp->LinkChannel&0X000001FF);    
    ptLinkState = Xml_GetLinkStateByIndex(wLinkStateIndex); 
    if (NULL == ptLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d Xml_GetLinkStateByIndex Error! ....................",__func__,__LINE__);  
        return;
    }
    /*ɱ����ʱ��*/
    Xml_KillExeTimer(ptLinkState);

    /* �յ�Ӧ�õķ��ؽ��֮��ɱ�������ϱ��ȴ���ʱ�� */
    XML_KillCmdPlanTimer(ptLinkState);
    
    /*��ǰ��·����*/
    if (pec_MsgHandle_IDLE == ptLinkState->ucCurRunState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML link state is idle! ....................",__func__,__LINE__);  
        return;
    }
    if (NULL == ptLinkState->ptLastSendMsg)
    {  
        /*������·Ϊ����״̬*/
        XML_LinkSetCurRunState(ptLinkState, pec_MsgHandle_IDLE);
        XOS_SysLog(OAM_CFG_LOG_NOTICE,
              "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);  
        return;
    }
    /*����UB*/
    pOMMMsgBuf = (CHAR *)XOS_GetUB(MAX_SIZE_OMMACKMSG);
    if(NULL == pOMMMsgBuf)
    {
        /*�����ܻظ�����UBʧ��*/
        ptrOutputResult = ReadMapValueOfOamDat(MAP_TYPE_XMLADAPTER,MAP_KEY_ERR_GETUB);
        SendErrStringToOmm(ptLinkState, ptrOutputResult);
        /*������·Ϊ����״̬*/
        XML_LinkSetCurRunState(ptLinkState, pec_MsgHandle_IDLE);
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XOS_GetUB Error! ....................",__func__,__LINE__);
        return;
    }
    memset(pOMMMsgBuf,0,MAX_SIZE_OMMACKMSG);    
    /*OAM��Ϣת��ΪXML�ַ���*/
    iRet = OamMgt2XmlMgt(ptOamMsgBufTmp,ptLinkState,MAX_SIZE_OMMACKMSG,&wMsgLen,pOMMMsgBuf);
    if (RETURN_OK != iRet)
    {
        /*�����ܻظ�OAM��Ϣ����ʧ��*/
	 ptrOutputResult = ReadMapValueOfOamDat(MAP_TYPE_XMLADAPTER,MAP_KEY_ERR_OAMMSG2XML);
        SendErrStringToOmm(ptLinkState, ptrOutputResult);
        /*������·Ϊ����״̬*/
        XML_LinkSetCurRunState(ptLinkState, pec_MsgHandle_IDLE);
        OAM_RETUB(pOMMMsgBuf);
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d OamMgt2XmlMgt Error,code %d! ....................",__func__,__LINE__,iRet);         
        return;
    }
    /*����Ӧ�÷�������,��ѯģʽջ�������,
    ���⽫�ϴ�ִ�е����ݸ���*/
    if((CMD_LDB_GET_MODESTATCK != ptOamMsgBufTmp->CmdID) && (!(IsSaveCmd(ptOamMsgBufTmp->CmdID))))
    {
        memcpy(ptLinkState->LastData,ptOamMsgBufTmp->LastData,sizeof(ptOamMsgBufTmp->LastData));
        memcpy(ptLinkState->BUF,ptOamMsgBufTmp->BUF,sizeof(ptOamMsgBufTmp->BUF));
    }    
    /*�������*/
    if (SUCC_CMD_NOT_FINISHED == ptOamMsgBufTmp->ReturnCode)
    {
        memcpy(ptLinkState->ptLastSendMsg->LastData,ptLinkState->LastData, sizeof(ptLinkState->LastData));	 
        memcpy(ptLinkState->ptLastSendMsg->BUF,ptLinkState->BUF, sizeof(ptLinkState->BUF));
	 ptLinkState->ptLastSendMsg->CmdRestartFlag = FALSE;
	 /*������ʱ��*/
	 Xml_SetExeTimer(ptLinkState);
	 /*��OAM������Ϣ*/
	 XOS_SendAsynMsg(EV_OAM_CFG_CFG, (BYTE *)ptLinkState->ptLastSendMsg, ptLinkState->ptLastSendMsg->DataLen, 0, 0, &(ptLinkState->tCurCMDInfo.cfgDat->tJid));	 
    }
    else if (SUCC_WAIT_AND_HAVEPARA == ptOamMsgBufTmp->ReturnCode)
    {
	 /*������ʱ��*/
	 Xml_SetExeTimer(ptLinkState);
    }
    else
    {
        /*������·Ϊ����״̬*/
        XML_LinkSetCurRunState(ptLinkState, pec_MsgHandle_IDLE); 
    }
    if (bDebugPrint)
    {
        PrintMsgXml((TYPE_XMLMODEL *)pOMMMsgBuf, "MSG_XMLAGNET_TO_OMM");
    }
    if (!IsSaveCmd(ptOamMsgBufTmp->CmdID))
    {
        /*�޸ĵ�ǰģʽID*/
        ChangeXmlLinkStateMode(ptOamMsgBufTmp->ReturnCode,ptLinkState);
    }
    /*��OMM������Ϣ*/
    XOS_SendAsynMsg(EV_XML_OMMCFG_ACK,(BYTE *)pOMMMsgBuf,wMsgLen, 0, 0, &(ptLinkState->tDtJid));
    /*�ͷ��ڴ�*/   
    OAM_RETUB(pOMMMsgBuf);     
    return;		
}

/**********************************************************************
* �������ƣ�VOID CeaseCmdReqProc
* ��������������������ֹ��Ϣ��Ӧ��
* ���ʵı���
* �޸ĵı���
* ���������WORD32 dwTrmId:��·��
* �����������
* �� �� ֵ��XML_AGENT_STAT
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2009-12-4             ������      ����
************************************************************************/
static XML_AGENT_STAT CeaseCmdReqProc(WORD32 dwTrmId)
{
    CHAR *ptrOutputResult = NULL;
    INT iRet = 0;                                                /*�������*/
    TYPE_XMLLINKSTATE *pXmlLinkState = NULL;  /*��·��Ϣָ��*/
    T_OAM_Cfg_Dat * pCfgDat = NULL;                 /* ��������dat�ṹָ��*/
    pXmlLinkState = Xml_GetLinkStateByLinkChannel(dwTrmId);
    /*δ�ҵ���·��Ϣ*/
    if (NULL == pXmlLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d Can not find xml link information! ....................",__func__,__LINE__);
        return NULL_PTR;
    }
    /*��·������ִ��*/
    if (pec_MsgHandle_IDLE == pXmlLinkState->ucCurRunState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d Link have not command! ....................",__func__,__LINE__);
        return NO_EXECUTE_CMD_ON_LINKCHANNEL;
    }
    pCfgDat = pXmlLinkState->tCurCMDInfo.cfgDat;
    /*����ű�ָ��Ϊ��*/
    if((NULL == pCfgDat) ||
        (NULL == pCfgDat->tCmdAttr))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d pCfgDat is null! ....................",__func__,__LINE__);
        return NULL_PTR;
    }
    /*����Խ����*/
    if (pXmlLinkState->tCurCMDInfo.wCmdAttrPos >= pCfgDat->nCmdAttrSum)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d Array Over Flow!%d-%d ....................\n",__func__,__LINE__,pXmlLinkState->tCurCMDInfo.wCmdAttrPos,pCfgDat->nCmdAttrSum);
        return RETURN_ERROR;
    }

    /*�������ֹ����Ӧ�û���*/
    if (CMD_ATTR_CANCEASE(&pCfgDat->tCmdAttr[pXmlLinkState->tCurCMDInfo.wCmdAttrPos]) &&
        CMD_ATTR_CANCEASE_NEEDACK(&pCfgDat->tCmdAttr[pXmlLinkState->tCurCMDInfo.wCmdAttrPos]))
    {        
        if (NULL == pXmlLinkState->ptLastSendMsg)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d Last message is null! ....................",__func__,__LINE__);
            return NULL_PTR;
        }
        /*���ö�ʱ��*/
        Xml_KillExeTimer(pXmlLinkState);
        Xml_SetExeTimer(pXmlLinkState);
        /*��Ӧ�÷���������ֹ����*/
        iRet = XOS_SendAsynMsg(EV_OAM_CEASE_TO_APP, (BYTE *)pXmlLinkState->ptLastSendMsg, pXmlLinkState->ptLastSendMsg->DataLen, 0, 0, &(pCfgDat->tJid));
        if (XOS_SUCCESS != iRet)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d Send cease command to application error,error code = %d! ....................",__func__,__LINE__,iRet);
            return RETURN_ERROR;
        }
    }
    /*�������ֹ����Ӧ�û���*/
    else if(CMD_ATTR_CANCEASE(&pCfgDat->tCmdAttr[pXmlLinkState->tCurCMDInfo.wCmdAttrPos]))
    {
        SendStringToOmm(pXmlLinkState, Hdr_szCeasedTheCmdTip,TRUE,TRUE);
        /*ɱ��XML��·��ʱ��*/
        Xml_KillExeTimer(pXmlLinkState);
	 /*�ָ�״̬Ϊ����*/
        XML_LinkSetCurRunState(pXmlLinkState,pec_MsgHandle_IDLE);
    }
    else/*�������ֹ*/
    {
        /*��ͻ��˷����������ֹ*/
	 ptrOutputResult = ReadMapValueOfOamDat(MAP_TYPE_XMLADAPTER,MAP_KEY_ERR_EXECUTEOMMCMD);
        SendStringToOmm(pXmlLinkState, ptrOutputResult,FALSE,FALSE);
    }
    return RETURN_OK;
}

/**********************************************************************
* �������ƣ�CHAR* XML_GetTextForUTF
* ������������ȡUTF-8�ı�
* ���ʵı���
* �޸ĵı���
* ���������pXmlDocPtr:�ĵ�ָ��
                             pucComponentId:�齨��
* �����������
* �� �� ֵ��CHAR *
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
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
    /*�������*/
    if((NULL == pXmlDocPtr) || (NULL == pucComponentId))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                               "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return NULL;
    }
    pOMMMsgBuf = (CHAR *)XOS_GetUB(MAX_SIZE_OMMACKMSG);
    /*�����ڴ�ʧ��*/
    if (NULL == pOMMMsgBuf)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XOS_GetUB Error! ....................",__func__,__LINE__);
        return NULL;
    }
    /*����ͷ�ļ�*/
    memset(pOMMMsgBuf,0,MAX_SIZE_OMMACKMSG);
    memcpy(pOMMMsgBuf,pucComponentId,2);
    pXMLMsgBuf = pOMMMsgBuf + 2;
    wLen = MAX_SIZE_OMMACKMSG - 2;
    do
    {
        dwRet = XML_GetText(pXmlDocPtr, pXMLMsgBuf, wLen, options,&wActualLen);
        /*��ȡ�ı�ʧ��*/
        if (XML_OK != dwRet)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR,
                                "....................FUNC:%s LINE:%d XML_GetText Error,Error code %d! ....................",__func__,__LINE__,dwRet);
            break;
        }
        pXMLChar = pXMLMsgBuf;
        /*����"?>"*/
        while(('\0' != *pXMLChar) && (wLoop < wActualLen))
        {
            if (('?' == *pXMLChar) && ('>' == *(pXMLChar+1)))
            {
                break;
            }
            pXMLChar++;
            wLoop++;
        }
        /*δ�ҵ�*/
        if ((wLoop >= wActualLen) || ('\0' == *pXMLChar))
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR,
                                "....................FUNC:%s LINE:%d XML Char Error,%d,%d! ....................",__func__,__LINE__,wLoop,wActualLen);
            break;
        }
        /*�ռ䲻��*/
        if ((wLen - wActualLen) <=  wEncodingLen)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR,
                                "....................FUNC:%s LINE:%d MEM Space Error,%d,%d! ....................",__func__,__LINE__,wLen - wActualLen,wEncodingLen);
            break;
        }
        /*����ƫ��" encoding=\"UTF-8\" "����*/
        memmove(pXMLChar+wEncodingLen,pXMLChar,wActualLen - (pXMLChar - pXMLMsgBuf));
        /*�������*/
        memcpy(pXMLChar,pEncoding,wEncodingLen);
        return pOMMMsgBuf;
    }while(0);
    /*�ͷŻ�����*/
    OAM_RETUB(pOMMMsgBuf);
    return NULL;
}

/**********************************************************************
* �������ƣ�VOID ProcCeaseCmd
* ����������������ֹ������
* ���ʵı���
* �޸ĵı���
* ���������BYTE *pucComponentId:�齨��
                             WORD16 wIdLen:�齨�ų���
                             LPXmlNode ptRootNode:���ڵ�ָ��
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
static XML_AGENT_STAT ProcCeaseCmd(const LPXmlNode ptOriRtNode)
{
    WORD32 dwRet = 0;
    WORD32 dwTrmId = 0;
    /*�������*/
    if (NULL == ptOriRtNode)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................\n",__func__,__LINE__);
        return NULL_PTR;
    }
    dwRet = SetTRMNodeText(ptOriRtNode,&dwTrmId);
    /*��ȡԭ<TRM>�ı�ʧ��*/
    if (XML_OK != dwRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d GetNodeText Error!%d ....................\n",__func__,__LINE__,dwRet);
        return GET_NODE_TEXT_ERR;
    }
    /*����������ֹ����Ӧ��*/
    dwRet = CeaseCmdReqProc(dwTrmId);    
    return dwRet;
}

/**********************************************************************
* �������ƣ�void ReceiveFromOmm
* ����������OMM������Ϣ��OAM
* ���ʵı���
* �޸ĵı���
* ���������LPVOID ptrMsgBuf:OMM��Ϣ��
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
VOID ReceiveFromOmm(LPVOID ptrMsgBuf)
{ 
    CHAR *ptrOutputResult = NULL;
    TYPE_XMLMODEL *ptXmlModel = NULL;                      /*XMLģ��ָ��*/    
    TYPE_XMLLINKSTATE *ptLinkState = NULL;                 /*��·״ָ̬��*/
    XmlDocPtr ptrDoc = NULL;                                         /*xml�ĵ�ָ��*/
    XmlNode tRootNode;                                                /*���ڵ�ṹ����*/    
    WORD16 wMsgLen = 0;                                            /*OMM��Ϣ�峤��*/
    WORD32  dwXmlLen = 0;                                         /*OMM��Ϣ��XML���ݳ���*/
    BOOLEAN bIs3GPL = TRUE;                                      /*��Ԫ�����Ƿ�Ϊ3GPL*/
    INT iRet = 0;                                                        /*����״̬*/
    INT iOptions = XML_STAT_PARSE_NOBLANKS;           /*��������*/
   
    /*����ж�*/
    if (NULL == ptrMsgBuf)
    {
         XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
         return;
    }

    /*��ʼ������*/  
    memset(pRecvOMMMsgBuf,0,MAX_SIZE_OMMMSG);    
    memset(&tRootNode,0,sizeof(XmlNode)); 

    /*��ȡ��Ϣ����*/
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
    /*����XML�ĵ������ظ��ڵ�*/
    iRet = XML_ParaseDoc(ptXmlModel->ucXML,dwXmlLen,iOptions,&ptrDoc,&tRootNode);
    if (RETURN_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_ParaseDoc Error,Error code %d! ....................",__func__,__LINE__,iRet);    
	 return ;
    }
    /*�����Ԫ�����Ƿ�Ϊ3GPL*/
    iRet = CheckNtpNodeTextIs3GPL(&tRootNode,&bIs3GPL);
    if (RETURN_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d CheckNtpNodeTextIs3GPL Error,Error code %d! ....................",__func__,__LINE__,iRet);    
	 return ;
    }
    /*��Ԫ����Ϊ3GPL--ƽ̨�ڲ�����*/    
    if (bIs3GPL)
    {
        CHAR ptrCommId[2] = {0};/*�����*/
        JID tOmmJid = {0};/*���ܶ�JID*/	 
	 /*��ȡ���ܶ�JID*/
	 XOS_Sender(&tOmmJid);
	 /*��ȡ�����齨��*/
	 memcpy(ptrCommId,pRecvOMMMsgBuf,sizeof(ptrCommId));
	 /*������·��Ϣ*/
	 iRet = SetXmlLinkState(ptXmlModel,&tRootNode,&ptLinkState);
	 switch(iRet)
	 {
            case RETURN_OK:
                /*��ƽ̨������������*/
                SendOMMReqToPlat(ptLinkState,&tRootNode);	    
                break;
            case SHOW_MODESTACK_ERR:/*��ѯģʽջ��Ϣ����*/
	     {
	         if (XML_LINKSTATE_VERCHANGED == ptLinkState->bVerChanged)
	         {
	             /*�����ܷ���DAT�ű����л���Ϣ*/	
	             ptrOutputResult = ReadMapValueOfOamDat(MAP_TYPE_XMLADAPTER,MAP_KEY_CHANGEMODEFORDAT);
	         }
                else
                {
                    /*�����ܷ���ģʽջ������Ϣ*/	
                    ptrOutputResult = ReadMapValueOfOamDat(MAP_TYPE_XMLADAPTER,MAP_KEY_NO_FINDCMDINMODE);
                }
	         ptLinkState->bVerChanged = XML_LINKSTATE_VERUNCHANGED;
                ProcErrPlatCfgMsg(ptrCommId,&tRootNode,ptrOutputResult,&tOmmJid);
		  break;
	     }
            case DatUpdateMutex:/*��ǰ�����л��汾,����ִ������*/
	         ProcErrPlatCfgMsg(ptrCommId, &tRootNode,Hdr_szDatUpdateMutexTip,&tOmmJid);
                break;
            case SaveMutexTip:/*����·���ڴ���,����ִ������*/            
                ProcErrPlatCfgMsg(ptrCommId,&tRootNode ,Hdr_szSaveMutexTip,&tOmmJid);
                break;
	     case CmdExecutMutex:/*��ǰ��·����ִ������*/
                ptrOutputResult = ReadMapValueOfOamDat(MAP_TYPE_XMLADAPTER,MAP_KEY_EXECUTING_CMD);
                ProcErrPlatCfgMsg(ptrCommId,&tRootNode , ptrOutputResult,&tOmmJid);		   
                break;
            case NoRegXmlLinkState:/*δע��XML��·*/
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
        /*͸����ҵ��*/        
	 SendOMMReqToOP(pRecvOMMMsgBuf,wMsgLen,&tRootNode);	 
    }
    /*�ͷ��ĵ�ָ��*/
    XML_FreeXml(ptrDoc);    
    return;
}

/**********************************************************************
* �������ƣ�void RecvFromProtocolToOMM
* ����������OAM������Ϣ��OMM
* ���ʵı���
* �޸ĵı���
* ���������LPVOID ptrMsgBuf��OAM��Ϣ��            
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
VOID RecvFromProtocolToOMM(LPVOID ptrMsgBuf)
{   
    MSG_COMM_OAM *ptOamMsgBuf = NULL;                       /*OAM��Ϣ����ʱָ��*/
    WORD16 wMsgLen = 0;                                                  /*��Ϣ�峤��*/
    	
    /*����ж�*/
    if(ptrMsgBuf == NULL)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__); 
        return;
    }
    memset(pRecvPlatMsgBuf,0,sizeof(pRecvPlatMsgBuf)); 
    /*��ȡ��Ϣ����*/
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
    /*?? brs�ķ��������10ʱ��Ҫת��Ϊ������map����*/
    if (ptOamMsgBuf->ReturnCode >= 10 &&
        ptOamMsgBuf->ReturnCode != SUCC_WAIT_AND_HAVEPARA) 
    {
        ConstructBrsMapPara(ptOamMsgBuf);
    }
    /*����ƽ̨Ӧ������*/
    SendPLATAckToOMM(ptOamMsgBuf);        
    return;
}

/**********************************************************************
* �������ƣ�void RecvFromOP
* ����������OAM������Ϣ��OMM
* ���ʵı���
* �޸ĵı���
* ���������LPVOID ptrMsgBuf��OAM��Ϣ��            
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-9-2             ������      ����
************************************************************************/
VOID RecvFromOP(LPVOID ptrMsgBuf)
{    
    TYPE_XMLMODEL *ptXmlModel = NULL;                 /*XMLģ��ָ��*/
    XmlDocPtr ptrDoc = NULL;                                     /*xml�ĵ�ָ��*/
    XmlNode tRootNode;                                            /*���ڵ�ṹ����*/    
    WORD16 wMsgLen = 0;                                        /*OMM��Ϣ�峤��*/    
    WORD32  dwXmlLen = 0;                                      /*OMM��Ϣ��XML���ݳ���*/    
    INT iOptions = XML_STAT_PARSE_NOBLANKS;       /*��������*/
        
    /*����ж�*/
    if(ptrMsgBuf == NULL)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__); 
        return;
    }

    memset(pRecvOMMMsgBuf,0,MAX_SIZE_OMMMSG);
    memset(&tRootNode,0,sizeof(XmlDocPtr));
    /*��ȡ��Ϣ����*/
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
    /*����XML�ĵ������ظ��ڵ�*/
    if (RETURN_OK != XML_ParaseDoc(ptXmlModel->ucXML,dwXmlLen,iOptions,&ptrDoc,&tRootNode))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_ParaseDoc Error! ....................",__func__,__LINE__);
	 return ;
    }
    /*�����ܷ���ҵ��Ӧ����Ϣ*/
    SendOPAckToOMM(pRecvOMMMsgBuf,wMsgLen,&tRootNode);    
    /*�ͷ��ĵ�ָ��*/
    XML_FreeXml(ptrDoc);     
    return;
}

/**********************************************************************
* �������ƣ�VOID ReceiveRegisterMsgFromOmm
* �����������������ܶ�ע������
* ���ʵı���
* �޸ĵı���
* ���������LPVOID ptrMsgBuf��ע��������Ϣ��            
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-12-17             ������      ����
************************************************************************/
VOID ReceiveRegisterMsgFromOmm(LPVOID ptrMsgBuf)
{
    CHAR pRecvMsgBuf[MAX_SIZE_OMMREGISTERREQ_BUFFER];/*������Ϣ������*/
    WORD16 wMsgLen = 0; /*��Ϣ����*/    
    WORD16 wLoop = 0;
    JID tDtJid;/*���Ͷ�JID*/    
    OMMRegisterReq *ptOMMRegisterReq = NULL;/*������Ϣָ��*/
    OMMRegisterAck tOMMRegisterAck = {0};/*Ӧ����Ϣָ��*/		
    
    /*����ж�*/
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
    /*��ȡ��Ϣ����*/
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
	 /*ע���ն˺�*/
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
    /*ͳ��Ӧ����Ϣ�峤��*/
    wMsgLen = sizeof(tOMMRegisterAck.wComponentId) + sizeof(tOMMRegisterAck.wTRMNum) + tOMMRegisterAck.wTRMNum * (sizeof(tOMMRegisterAck.awTRMId)/MAX_COUNT_TRM + sizeof(tOMMRegisterAck.awOR)/MAX_COUNT_TRM);
    /*��ӡ��Ϣ*/
    XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d ComponentId:%d,TRMNum:%d! ....................",__func__,__LINE__,tOMMRegisterAck.wComponentId,tOMMRegisterAck.wTRMNum); 
    for (wLoop = 0;wLoop < tOMMRegisterAck.wTRMNum;wLoop++)
    {
        XOS_SysLog(OAM_CFG_LOG_NOTICE,
                       "....................FUNC:%s LINE:%d TRMId:%d,OR:%d! ....................",__func__,__LINE__,tOMMRegisterAck.awTRMId[wLoop],tOMMRegisterAck.awOR[wLoop]);
    }
    /*��OMM����ע��Ӧ��*/    
    XOS_SendAsynMsg(EV_XML_REGISTER_ACK, (BYTE *)&tOMMRegisterAck,wMsgLen, 0, 0, &tDtJid);    
    XOS_SysLog(OAM_CFG_LOG_NOTICE,
                       "....................FUNC:%s LINE:%d Send RegisterAck To OMM Successfully! ....................",__func__,__LINE__);    
    return;   
}

/**********************************************************************
* �������ƣ�VOID ReceiveUnRegisterMsgFromOmm
* �����������������ܶ�ע������
* ���ʵı���
* �޸ĵı���
* ���������LPVOID ptrMsgBuf��ע��������Ϣ��            
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-12-17             ������      ����
************************************************************************/
VOID ReceiveUnRegisterMsgFromOmm(LPVOID ptrMsgBuf)
{
    CHAR pRecvMsgBuf[MAX_SIZE_OMMUNREGISTERREQ_BUFFER];/*������Ϣ������*/
    WORD16 wLoop = 0;
    WORD16 wMsgLen = 0;/*��Ϣ����*/    
    JID tDtJid;/*���Ͷ�JID*/  
    OMMUnRegisterReq *ptOMMUnRegisterReq = NULL;/*������Ϣָ��*/
    OMMUnRegisterAck tOMMUnRegisterAck = {0};/*Ӧ����Ϣָ��*/      
		
    /*����ж�*/
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
    /*��ȡ��Ϣ����*/
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
	 /*ע���ն˺�*/
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
    /*ͳ��Ӧ����Ϣ�峤��*/
    wMsgLen = sizeof(tOMMUnRegisterAck.wComponentId) + sizeof(tOMMUnRegisterAck.wTRMNum) + tOMMUnRegisterAck.wTRMNum * (sizeof(tOMMUnRegisterAck.awTRMId)/MAX_COUNT_TRM + sizeof(tOMMUnRegisterAck.awOR)/MAX_COUNT_TRM);    
    /*��ӡ��Ϣ*/
    XOS_SysLog(OAM_CFG_LOG_NOTICE,
                       "....................FUNC:%s LINE:%d ComponentId %d,TRMNum %d! ....................",__func__,__LINE__,tOMMUnRegisterAck.wComponentId,tOMMUnRegisterAck.wTRMNum);
    for (wLoop = 0;wLoop < tOMMUnRegisterAck.wTRMNum;wLoop++)
    {
        XOS_SysLog(OAM_CFG_LOG_NOTICE,
                       "....................FUNC:%s LINE:%d TRMId %d,OR %d! ....................",__func__,__LINE__,tOMMUnRegisterAck.awTRMId[wLoop],tOMMUnRegisterAck.awOR[wLoop]);        
    }
    /*��OMM����ע��Ӧ��*/
    XOS_SendAsynMsg(EV_XML_UNREGISTER_ACK, (BYTE *)&tOMMUnRegisterAck,wMsgLen, 0, 0, &tDtJid);    
    XOS_SysLog(OAM_CFG_LOG_NOTICE,
                       "....................FUNC:%s LINE:%d Send UnRegister Ack To OMM Successfully! ....................",__func__,__LINE__);    
    return;
}

/**********************************************************************
* �������ƣ�VOID ReceiveUnRegisterMsgFromOmm
* �����������������ܶ���������
* ���ʵı���
* �޸ĵı���
* ���������LPVOID ptrMsgBuf������������Ϣ��            
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-12-17             ������      ����
************************************************************************/
VOID ReceiveHandShakeMsgFromOmm(LPVOID ptrMsgBuf)
{
    CHAR pRecvMsgBuf[MAX_SIZE_OMMHANDSHAKEREQ_BUFFER];/*������Ϣ������*/
    JID tDtJid;
    WORD16 wMsgLen = 0;
    WORD16 wLoop = 0;
    OMMHandShankReq *ptOMMHandShankReq = NULL;/*������Ϣ��*/
    OMMHandShankAck tOMMHandShankAck = {0};/*Ӧ����Ϣ��*/
	    
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
    /*��ȡ��Ϣ����*/
    XOS_GetMsgDataLen(&wMsgLen);
    if ((wMsgLen > MAX_SIZE_OMMHANDSHAKEREQ_BUFFER) || (0 == wMsgLen))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XOS_GetMsgDataLen Error! ....................",__func__,__LINE__);
        return;
    } 
    memcpy(pRecvMsgBuf,ptrMsgBuf,wMsgLen); 
    ptOMMHandShankReq = (OMMHandShankReq *)pRecvMsgBuf;
    
    /*��������Ӧ����Ϣ*/
    if (RETURN_OK != SetOMMHandShakeAck(ptOMMHandShankReq,&tOMMHandShankAck))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d SetOMMHandShakeAck Error! ....................",__func__,__LINE__);	 
        return;
    }
    /*ɱ�����ֶ�ʱ��*/
    Xml_KillHandShakeTimer();
    /*�������ֶ�ʱ��*/
    Xml_SetHandShakeTimer();    
    wMsgLen = sizeof(tOMMHandShankAck.wComponentId) + sizeof(tOMMHandShankAck.wTRMNum) + tOMMHandShankAck.wTRMNum *(sizeof(tOMMHandShankAck.aucTRMId)/MAX_COUNT_TRM);
    /*��ӡ��Ϣ*/
    XOS_SysLog(OAM_CFG_LOG_NOTICE,
                       "....................FUNC:%s LINE:%d ComponentId %d,TRMNum %d! ....................",__func__,__LINE__,tOMMHandShankAck.wComponentId,tOMMHandShankAck.wTRMNum);   
    for (wLoop = 0;wLoop < tOMMHandShankAck.wTRMNum;wLoop++)
    {
        XOS_SysLog(OAM_CFG_LOG_NOTICE,
                       "....................FUNC:%s LINE:%d TRMId %d ....................",__func__,__LINE__,tOMMHandShankAck.aucTRMId[wLoop]);
    }
    /*��OMM��������Ӧ��*/
    XOS_SendAsynMsg(EV_XML_HANDSHAKE_ACK, (BYTE *)&tOMMHandShankAck,wMsgLen, 0, 0, &tDtJid);
    XOS_SysLog(OAM_CFG_LOG_NOTICE,
                       "....................FUNC:%s LINE:%d Send HandShakeAck To OMM Successfully! ....................",__func__,__LINE__);
    return;
}

/**********************************************************************
* �������ƣ�VOID ReceiveLinkCfgMsgFromOmm
* �����������������ܶ���·��������
* ���ʵı���
* �޸ĵı���
* ���������LPVOID ptrMsgBuf����·��������
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-12-17             ������      ����
************************************************************************/
VOID ReceiveLinkCfgMsgFromOmm(LPVOID ptrMsgBuf)
{
    CHAR *paucRecvOMMMsgBuf = NULL;                       /*������Ϣ������*/
    CHAR aucNodeText[MAX_NODETEXT_BUF_SIZE] = {0};/*�ڵ��ı�*/
    BYTE aucComponentId[2] = {0};
    INT iRet = 0;                                                           /*����״̬*/
    INT iOptions = XML_STAT_PARSE_NOBLANKS;             /*��������*/
    WORD16 wMsgLen = 0;                                           /*OMM��Ϣ�峤��*/
    WORD32  dwXmlLen = 0;                                         /*OMM��Ϣ��XML���ݳ���*/
    XmlDocPtr ptrDoc = NULL;                                        /*xml�ĵ�ָ��*/
    XmlNode tRootNode;                                               /*���ڵ�ṹ����*/
    TYPE_XMLMODEL *ptXmlModel = NULL;                     /*XMLģ��ָ��*/

    /*����ж�*/
    if (NULL == ptrMsgBuf)
    {
         XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
         return;
    }
    memset(&tRootNode,0,sizeof(tRootNode));
    /*��ȡ��Ϣ����*/
    XOS_GetMsgDataLen(&wMsgLen);
    if(0 == wMsgLen)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XOS_GetMsgDataLen %d Error!....................",__func__,__LINE__,wMsgLen);        
        return;
    }
    /*������ջ������ڴ�*/
    paucRecvOMMMsgBuf = (CHAR *)XOS_GetUB(wMsgLen + 1);
    /*����ʧ��*/
    if (NULL == paucRecvOMMMsgBuf)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XOS_GetUB Error!....................",__func__,__LINE__);        
        return;
    }
    /*������Ϣ*/
    memset(paucRecvOMMMsgBuf,0,wMsgLen + 1);
    memcpy(paucRecvOMMMsgBuf,ptrMsgBuf,wMsgLen);
    /*���������*/
    memcpy(aucComponentId,paucRecvOMMMsgBuf,2);
    ptXmlModel = (TYPE_XMLMODEL *)paucRecvOMMMsgBuf;
    dwXmlLen = strlen(ptXmlModel->ucXML);
    /*��ӡ������Ϣ*/
    if (bDebugPrint)
    {
        PrintMsgXml(ptXmlModel, "MSG_OMM_TO_XMLAGNET");
    }
    /*����XML�ĵ������ظ��ڵ�*/
    iRet = XML_ParaseDoc(ptXmlModel->ucXML,dwXmlLen,iOptions,&ptrDoc,&tRootNode);
    if (RETURN_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_ParaseDoc Error,Error code %d! ....................",__func__,__LINE__,iRet);
        OAM_RETUB(paucRecvOMMMsgBuf);
	 return ;
    }
    /*��ȡ�ڵ�<Type>���ı�*/
    iRet = GetNodeText(&tRootNode, NAME_TYPE_NODE,0,sizeof(aucNodeText) - 1, aucNodeText);
    if (RETURN_OK != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d GetNodeText Error,Error code %d! ....................",__func__,__LINE__,iRet);
        /*�ͷ���Դ*/
        OAM_RETUB(paucRecvOMMMsgBuf);
        XML_FreeXml(ptrDoc); 
	 return ;
    }
    /*ע����·*/
    if(0 == strncmp(aucNodeText,LINKCFG_NAME_LOGIN,strlen(LINKCFG_NAME_LOGIN)))
    {
        iRet = LinkCfg_LoginProcess(aucComponentId,&tRootNode);
        XOS_SysLog(OAM_CFG_LOG_DEBUG,
                       "....................FUNC:%s LINE:%d LinkCfg_LoginProcess ,code %d! ....................",__func__,__LINE__,iRet);
    }
    /*ע����·*/
    else if(0 == strncmp(aucNodeText,LINKCFG_NAME_LOGOUT,strlen(LINKCFG_NAME_LOGOUT)))
    {
        iRet = LinkCfg_LogoutProcess(aucComponentId,&tRootNode);
        XOS_SysLog(OAM_CFG_LOG_DEBUG,
                       "....................FUNC:%s LINE:%d LinkCfg_LogoutProcess ,code %d! ....................",__func__,__LINE__,iRet);
    }
    /*��������*/
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
    /*�ͷ���Դ*/
    OAM_RETUB(paucRecvOMMMsgBuf);
    XML_FreeXml(ptrDoc); 
    return;
}

/**********************************************************************
* �������ƣ�VOID ReceiveCeaseCmdMsgFromOmm
* �����������������ܶ�������ֹ����
* ���ʵı���
* �޸ĵı���
* ���������LPVOID ptrMsgBuf��������ֹ����
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-12-17             ������      ����
************************************************************************/
VOID ReceiveCeaseCmdMsgFromOmm(LPVOID ptrMsgBuf)
{
    CHAR aucComponentId[MAX_SIZE_COMPONENTID] = {0};
    CHAR *pucRecvOMMMsgBuf = NULL;                        /*����������Ϣ������*/
    WORD32 dwRet = 0;                                               /*����״̬*/
    INT iOptions = XML_STAT_PARSE_NOBLANKS;             /*��������*/
    WORD16 wMsgLen = 0;                                           /*OMM��Ϣ�峤��*/
    WORD32  dwXmlLen = 0;                                         /*OMM��Ϣ��XML���ݳ���*/
    XmlDocPtr ptOriDoc = NULL;                                     /*ԭxml�ĵ�ָ��*/
    XmlNode tOriRtNode;                                              /*ԭ�ĵ����ڵ�ṹ����*/
    TYPE_XMLMODEL *ptXmlModel = NULL;                     /*XMLģ��ָ��*/    
    CHAR *pucORText = "";
    CHAR *pucEOFText = "";
    CHAR *pucRSTRText = "";
    JID tSenderJid = {0};

    /*����ж�*/
    if (NULL == ptrMsgBuf)
    {
         XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
         return;
    }
    /*��ȡ���Ͷ�JID*/
    dwRet = XOS_Sender(&tSenderJid);
    if (XOS_SUCCESS != dwRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XOS_Sender Error!%d ....................",__func__,__LINE__,dwRet);
         return;
    }
    memset(&tOriRtNode,0,sizeof(tOriRtNode));
    dwRet = XOS_GetMsgDataLen(&wMsgLen);
    /*��ȡ��Ϣ����ʧ��*/
    if (XOS_SUCCESS != dwRet)
    {
         XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XOS_GetMsgDataLen Error!%d ....................",__func__,__LINE__,dwRet);
         return;
    }
    /*����Ϣ*/
    if(0 == wMsgLen)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XOS_GetMsgDataLen %d Error!....................",__func__,__LINE__,wMsgLen);        
        return;
    }
    pucRecvOMMMsgBuf = (CHAR *)XOS_GetUB(wMsgLen + 1);
    /*�����ڴ�ʧ��*/
    if (NULL == pucRecvOMMMsgBuf)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XOS_GetUB Error!....................",__func__,__LINE__);        
        return;
    }
    /*������Ϣ*/
    memset(pucRecvOMMMsgBuf,0,wMsgLen + 1);
    memcpy(pucRecvOMMMsgBuf,ptrMsgBuf,wMsgLen);
    memcpy(aucComponentId,pucRecvOMMMsgBuf,MAX_SIZE_COMPONENTID);
    /*��ȡXML�ַ���*/
    ptXmlModel = (TYPE_XMLMODEL *)pucRecvOMMMsgBuf;
    dwXmlLen = strlen(ptXmlModel->ucXML);
    /*��ӡ������ϢXML�ַ���*/
    if (bDebugPrint)
    {        
        PrintMsgXml(ptXmlModel, "MSG_OMM_TO_XMLAGNET");
    }
    dwRet = XML_ParaseDoc(ptXmlModel->ucXML,dwXmlLen,iOptions,&ptOriDoc,&tOriRtNode);
    /*����XML�ĵ������ظ��ڵ�ʧ��*/
    if (RETURN_OK != dwRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XML_ParaseDoc Error,Error code %d! ....................",__func__,__LINE__,dwRet);
        OAM_RETUB(pucRecvOMMMsgBuf);
	 return ;
    }
    /*����������ֹ*/
    dwRet = ProcCeaseCmd(&tOriRtNode);
    /*<OR>�ı�*/
    if (RETURN_OK == dwRet)
    {
        pucORText = TEXT_OR_NODE_OK;
    }
    else
    {
        pucORText = TEXT_OR_NODE_NOK;
    }
    /*<EOF>�ı�*/
    pucEOFText = TEXT_EOF_NODE_Y;
    /*<RSLT>�ı�*/
    pucRSTRText = GetRSTRText(dwRet);
    if (NULL == pucRSTRText)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                            "....................FUNC:%s LINE:%d GetRSTRText Error! ....................\n",__func__,__LINE__);
        /*�ͷ���Դ*/
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
    /*����Ӧ����Ϣʧ��*/
    if (RETURN_OK != dwRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d ConstructAckMsg2OMM %d Error!....................",__func__,__LINE__,dwRet);
	 /*�ͷ���Դ*/
        OAM_RETUB(pucRecvOMMMsgBuf);
        XML_FreeXml(ptOriDoc);
        return ;
    }
    /*�ͷ���Դ*/
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
    CHAR *pOMMMsgBuf = NULL;   /*OMMӦ����Ϣ��*/
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
        /*�������ڴ��̵����ܶ�*/
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
        /*������·�Ż�ȡ��·״̬*/
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

    /*����UB*/
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
    /*��OMM������Ϣ*/
    XOS_SendAsynMsg(EV_XML_OMMCFG_ACK,(BYTE *)pOMMMsgBuf,wMsgLen, 0, 0, &(ptLinkState->tDtJid));
    /*�ͷ��ڴ�*/   
    OAM_RETUB(pOMMMsgBuf);     
}

VOID XML_DealPlanTimer(TYPE_XMLLINKSTATE *ptLinkState)
{
    INT iRet = 0;
    WORD16 wMsgLen = 0;
    CHAR *pOMMMsgBuf = NULL;   /*OMMӦ����Ϣ��*/
    T_Cmd_Plan_Cfg CmdPlanInfo = {0};

    XML_KillCmdPlanTimer(ptLinkState);
    XML_SetCmdPlanTimer(ptLinkState);

    CmdPlanInfo.ucPlanValue = ptLinkState->ucPlanValuePrev;
    
    /*����UB*/
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
    /*��OMM������Ϣ*/
    XOS_SendAsynMsg(EV_XML_OMMCFG_ACK,(BYTE *)pOMMMsgBuf,wMsgLen, 0, 0, &(ptLinkState->tDtJid));
    /*�ͷ��ڴ�*/   
    OAM_RETUB(pOMMMsgBuf);     
}

/*-----------------------���Ժ���----------------------------*/
VOID Oam_DbgTestCfgCmd(VOID)
{
    CHAR *pucXmlBuf = NULL;
    CHAR *pucXml = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?><CONF><T><VER>0</VER><NTP>3GPL</NTP><NID>0</NID><TRM>3288860</TRM><TID>65535</TID><SID>0</SID><MID>0</MID></T><D><OT>S</OT><OR>OK</OR><DC>1</DC><EOF>Y</EOF><MD><CMD id=\"0x50020\" optype=\"S\"></CMD></MD></D></CONF>";
    WORD16 wMsgLen = 0;
    gt_XmlLinkState[0].dwLinkChannel = 3288860;
    gt_XmlLinkState[0].bUsed = XML_LINKSTATE_USED;
    /*�������ʹ��OAM�ṹ��ռ�*/
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

    /*��ȡ�����xml*/
    dwRet = XML_LoadXml(pcXml,strlen(pcXml)+1,XML_STAT_PARSE_NOBLANKS, &pDoc);
    if (XML_OK != dwRet)
    {
        printf("Test_Xml_RemoveChildNode::XML_LoadXml function error!%d ########\n",dwRet);
        return;
    }

    /*��ȡ���ڵ�*/
    dwRet = XML_GetRootNode(pDoc, &tRootNode);
    if (XML_OK != dwRet)
    {
        printf("Test_Xml_RemoveChildNode::XML_GetRootNode function error!%d ########\n",dwRet);
        XML_FreeXml(pDoc);
        return;
    }

    /*��ȡModes�ӽڵ�*/
    dwRet = XML_GetChild(&tRootNode, 0, &tTmpParentNode);
    if (XML_OK != dwRet)
    {
        printf("Test_Xml_RemoveChildNode::XML_GetChild function error!%d ########\n",dwRet);
        XML_FreeXml(pDoc);
        return;
    }

    /*��ȡModes�ڵ��һ���ӽڵ�*/
    dwRet = XML_GetChild(&tTmpParentNode, 1, &tChildNodeTmp);
    if (XML_OK != dwRet)
    {
        printf("Test_Xml_RemoveChildNode::XML_GetChild function error!%d ########\n",dwRet);
        XML_FreeXml(pDoc);
        return;
    }
    
    /*ɾ��Modes���м�һ���ӽڵ�*/
    dwRet =  XML_RemoveNode(&tChildNodeTmp);
    if (XML_OK != dwRet)
    {
        printf("Test_Xml_RemoveChildNode::Xml_RemoveChildNode function error!%d ########\n",dwRet);
        XML_FreeXml(pDoc);
        assert(0);
        return;
    }

    /*��ȡxml �ı�*/
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
    
    /*ɾ��Modes�ĵ�һ���ӽڵ�*/
    /*��ȡModes�ڵ��һ���ӽڵ�*/
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

    /*��ȡxml �ı�*/
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


    /*��ȡ��Types�ӽڵ�*/
    dwRet = XML_GetChild(&tRootNode, 1, &tTmpParentNode);
    if (XML_OK != dwRet)
    {
        printf("Test_Xml_RemoveChildNode::XML_GetChild function error!%d ########\n",dwRet);
        XML_FreeXml(pDoc);
        return;
    }
	
    /*ɾ��Types�ĵ�һ���ӽڵ�*/
    /*��ȡTypes�ڵ��һ���ӽڵ�*/
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

    /*��ȡxml �ı�*/
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
		
    /*ɾ��Types�ڵ㼰���ӽڵ�*/
    /*��ȡTypes�ڵ�*/
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

    /*��ȡxml �ı�*/
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
	
    /*��ȡ�����xml*/
    dwRet = XML_LoadXml(pcXml,strlen(pcXml)+1,XML_STAT_PARSE_NOBLANKS, &pDoc);
    if (XML_OK != dwRet)
    {
        printf("Test_Xml_RemoveChildNode::XML_LoadXml function error!%d ########\n",dwRet);
        return;
    }

    /*��ȡ���ڵ�*/
    dwRet = XML_GetRootNode(pDoc, &tRootNode);
    if (XML_OK != dwRet)
    {
        printf("Test_Xml_RemoveChildNode::XML_GetRootNode function error!%d ########\n",dwRet);
        XML_FreeXml(pDoc);
        return;
    }

    /*��ȡModes�ӽڵ�*/
    dwRet = XML_GetChild(&tRootNode, 0, &tTmpParentNode);
    if (XML_OK != dwRet)
    {
        printf("Test_Xml_RemoveChildNode::XML_GetChild function error!%d ########\n",dwRet);
        XML_FreeXml(pDoc);
        return;
    }

    /*��ȡModes�ڵ��һ���ӽڵ�*/
    dwRet = XML_GetChild(&tTmpParentNode, 0, &tChildNodeTmp);
    if (XML_OK != dwRet)
    {
        printf("Test_Xml_RemoveChildNode::XML_GetChild function error!%d ########\n",dwRet);
        XML_FreeXml(pDoc);
        return;
    }

    /*ɾ��modes��һ���ӽڵ��name����*/
    dwRet = XML_GetAttributeByName(&tChildNodeTmp, (BYTE *)"name", &tAttrTmp);
    assert(XML_OK == dwRet);
    XML_RemoveAttribute(&tAttrTmp);
    dwRet = XML_GetAttributeByName(&tChildNodeTmp, (BYTE *)"name", &tAttrTmp);
    assert(XML_ELEMENT_NOT_EXIST == dwRet);

     /*��ȡxml �ı�*/
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
	
    /*ɾ��modes��һ�����ӽڵ��multinode����*/
    dwRet = XML_GetAttributeByName(&tChildNodeTmp, (BYTE *)"multinode", &tAttrTmp);
    assert(XML_OK == dwRet);
    XML_RemoveAttribute(&tAttrTmp);
    dwRet = XML_GetAttributeByName(&tChildNodeTmp, (BYTE *)"multinode", &tAttrTmp);
    assert(XML_ELEMENT_NOT_EXIST == dwRet);

 /*��ȡxml �ı�*/
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
	
    /*��ȡModes�ڵ�Ķ����ӽڵ�*/
    dwRet = XML_GetChild(&tTmpParentNode, 1, &tChildNodeTmp);
    if (XML_OK != dwRet)
    {
        printf("Test_Xml_RemoveChildNode::XML_GetChild function error!%d ########\n",dwRet);
        XML_FreeXml(pDoc);
        return;
    }
    /*ɾ��modes�ڶ������ӽڵ��multinode����*/
    dwRet = XML_GetAttributeByName(&tChildNodeTmp, (BYTE *)"multinode", &tAttrTmp);
    assert(XML_OK == dwRet);
    XML_RemoveAttribute(&tAttrTmp);
    dwRet = XML_GetAttributeByName(&tChildNodeTmp, (BYTE *)"multinode", &tAttrTmp);
    assert(XML_ELEMENT_NOT_EXIST == dwRet);
		
     /*��ȡxml �ı�*/
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
    /*��ȡ�����xml*/
    dwRet = XML_LoadXml(pcXml,strlen(pcXml)+1,XML_STAT_PARSE_NOBLANKS, &pDoc);
    if (XML_OK != dwRet)
    {
        printf("Test_XML_XPath::XML_LoadXml function error!%d ########\n",dwRet);
        return;
    }

    /*��ȡXPATH�ڵ㼯*/	
    dwRet = XML_GetXpathNodeSet(pDoc,pXPath,&xmlXPathobj);
    if (XML_OK != dwRet)
    {
        printf("Test_XML_XPath::XML_GetXpathNodeSet function error!%d ########\n",dwRet);
        XML_FreeXml(pDoc);
        return;
    }
    /*��㼯 Ԫ�صĸ���*/
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
    /* achVal �����������2010-01-30& 16:<12:14 ��&amp;����Ϊ&���� */

    memset(achVal,0,sizeof(achVal));
    XML_NewDoc("testxml", &ptDoc);
    XML_GetRootNode(ptDoc, &tRoot);
    XML_InsertChild(&tRoot,0,"t2",&tTemp);
    XML_SetChildText(&tTemp,"this <is >a\" &test node!");
    XML_GetText(ptDoc, achVal, 1024, XML_STAT_SAVE_NO_EMPTY, &len);
    printf("achVal2=%s\n",achVal);
    /* achVal ����������<?xml version="1.0"?>
    <testxml><t2>this &lt;is &gt;a </t2></testxml>    ��������ת����ȷ��&���������ַ����������� */
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

