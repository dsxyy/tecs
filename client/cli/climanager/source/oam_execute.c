/**************************************************************************
* ��Ȩ���� (C)2002, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ�oam_execute.c
* �ļ���ʶ��
* ����ժҪ��OAMִ�н���
* ����˵����
            
* ��ǰ�汾��
* ��    �ߣ�����
* ������ڣ�2007.6.5
*
* �޸ļ�¼1��// �޸���ʷ��¼�������޸����ڡ��޸��߼��޸�����
*    �޸����ڣ�
*    �� �� �ţ�
*    �� �� �ˣ�
*    �޸����ݣ�
* �޸ļ�¼2����
**************************************************************************/
/**************************************************************************
*                           ͷ�ļ�                                        *
**************************************************************************/
#include <ctype.h>

#include "includes.h"

#include "oam_execute.h"
#include "xmlagent.h"
#include "offline_to_online.h"

/* added by fls on 2009-06-29 (CRDCM00197912 SSC2��������������ZDBʧ�ܣ�����λ����ΪOAM��DBS����Ľṹ�岻һ�µ���) */
//#include "db.h"
/* end of add */

/**************************************************************************
*                          ����                                           *
**************************************************************************/
/**************************************************************************
*                          ��                                             *
**************************************************************************/
/**************************************************************************
*                          ��������                                       *
**************************************************************************/
/**************************************************************************
*                          ȫ�ֱ���                                       *
**************************************************************************/
extern COMMON_INFO gtCommonInfo;
BOOL bDebugPrint = FALSE/*FALSE*/;
/**************************************************************************
*                          ���ر���                                       *
**************************************************************************/
extern T_OAM_INTERPRET_VAR *g_ptOamIntVar;

/**************************************************************************
*                          �ֲ�����ԭ��                                   *
**************************************************************************/

static void PrintDataType(BYTE bType);
static BYTE *PrintTable(BYTE *pData, BYTE bRecord, BYTE bTableType);

/*DBģʽ*/
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

/*Protocolģʽ*/
static void ExecSaveCmdProc(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *ptRcvMsg);
static void SaveZDBProc(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *ptMsg);

/*�����Ϣͷ�Ƿ�Ϸ�*/
static BOOL CheckMsgHead(MSG_COMM_OAM *ptMsg);


/*����������*/
static void ProcessAsycMsg(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *pMsg);
static void Oam_CmdReturnMsgHandler(TYPE_LINK_STATE *pLinkState,MSG_COMM_OAM *ptMsg);
static INT32 CheckExeMsgHead(TYPE_LINK_STATE *pLinkState,MSG_COMM_OAM * ptMsg);
static void ParseReturnData(TYPE_LINK_STATE *pLinkState,MSG_COMM_OAM * ptMsg);


/*static VOID CfgMsgTransAddrByteOder(MSG_COMM_OAM *ptr);*/

/**************************************************************************
*                          ȫ�ֺ�������                                   *
**************************************************************************/
extern BOOL AddUserToLink(USER_NODE *pNode);
extern BOOL DelUserFromLink(CHAR *pUserName);

extern void GetTerminalCfgData(MSG_COMM_OAM **pRtnMsg);
extern void GetUserNameData(MSG_COMM_OAM **pRtnMsg);
/* added by fls on 2009-06-29 (CRDCM00197905 SSC2����������OAM�����ֽ���̶�ΪСβ�ֽ���û�а���OMP���ֽ�����) */
/*extern BOOL Oam_CfgIsNeedInvertOrder(VOID);*/
/* end of add */

/**************************************************************************
*                          ȫ�ֺ���ʵ��                                   *
**************************************************************************/

/**************************************************************************
* �������ƣ�VOID Oam_RecvFromApp
* ������������������Protocol����Ϣ
* ���ʵı���
* �޸ĵı���
* ���������
  LPVOID pMsgPara
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
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
 
    /*��ӡ�շ���Ϣ*/
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
    if (dwCmdGrp == OAM_CLI_DDM_CMDGRP_SET)        /* DDM����SET��*/
        ucCmdType = IS_SET;
    else if ((dwCmdGrp == OAM_CLI_DDM_CMDGRP_GET) ||
                (dwCmdGrp == OAM_CLI_DDM_CMDGRP_GETTABLE))   /* DDM����GET��*/
        ucCmdType = IS_GET;
    else                                           // ��ͨCLI����
        ucCmdType = CheckGetOrSet(ptRcvMsg->CmdID);
    
    if ((ptRcvMsg->ucOamTag == OAMCFG_TAG_NORMAL) && 
        (ucCmdType == IS_SET) &&
        (OAM_MSG_IS_LAST_PACKET(ptRcvMsg)) &&
        (ptRcvMsg->ReturnCode != CLI_RET_KEEP_ALIVE_NOPARA))
    {
        /* д��������־ */
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

    /*������������Ԥ����*/
	
    /*debug���첽��Ϣseq����Ϊ0��
       �����첽��Ϣ�����ͬ����Ϣ��ͬ*/
    pLinkState->bRecvAsynMsg = (0 == ptRcvMsg->SeqNo) ? TRUE : FALSE;
    if (Oam_LinkIfRecvAsyncMsg(pLinkState))
    {
        ProcessAsycMsg(pLinkState, ptRcvMsg);
        return;
    }
    
	
    switch(ptRcvMsg->CmdID)
    {
    case CMD_LPTL_SET_TEMINAL_DISPLAY:
        /*�����terminaldisplay�����Ҫ�򿪻�ر���ʾ����*/
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

    /*?? brs�ķ��������10ʱ��Ҫת��Ϊ������map����*/
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
* �������ƣ�VOID ConstructBrsMapPara
* ��������������BRSӦ����Ϣ��MAP���Ͳ���
* ���ʵı���
* �޸ĵı���
* ���������MSG_COMM_OAM *ptMsg
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/04/02    V1.0    ������      ����
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
* �������ƣ�VOID CmdReturn_Msg_Handler
* ����������������������ִ�з�����Ϣ
* ���ʵı���
* �޸ĵı���
* ���������TYPE_LINK_STATE *pLinkState,MSG_COMM_OAM *ptMsg
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
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

    /* �û���¼�׶εȷ�ִ�н׶Σ�����Ҫ���� */
    if (Oam_LinkGetCurRunState(pLinkState) != pec_MsgHandle_EXECUTE)
    {
        XOS_SysLog(OAM_CFG_LOG_NOTICE,
                   "Oam_CmdReturnMsgHandler: Receive MSG_COMM_OAM msg, but current linkstate is not pec_MsgHandle_EXECUTE, discarded!");
        return;
    }

    /* 2.�����Ϣͷ */
    iErrCode = CheckExeMsgHead(pLinkState, ptRcvMsg);
    if (iErrCode != MSG_SUCCESS)
    {
        /*������յ�����Ϣ�е�SeqNoС�ڵ�ǰִ�е�SeqNo����������Ϣ(CRDCM00287137)*/
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

    /*********************** �����׶� *******************************/
    /*���ݷ���ֵ���� ��ʱ������ */
    switch(ptRcvMsg->ReturnCode)
    {
    case SUCC_CMD_DISPLAY_NO_KILLTIMER:
        /* ����Execute�˶�ʱ������ */
        Oam_KillExeTimer(pLinkState);
        if (Oam_LinkGetCurRunState(pLinkState) == pec_MsgHandle_EXECUTE)
            Oam_SetExeTimer(pLinkState);
        break;

    case SUCC_WAIT_AND_HAVEPARA:
        /* ÿ��ping��Ӧ��Ҫ����Execute�˶�ʱ�� 
        if (CMD_IS_PING_OR_TRACE(ptMsg->CmdID))
        {
            Oam_KillExeTimer(pLinkState);
            if (Oam_LinkGetCurRunState(pLinkState) == pec_MsgHandle_EXECUTE)
                Oam_SetExeTimer(pLinkState);
        }*/        
        /*����Execute�˶�ʱ��*/
        Oam_KillExeTimer(pLinkState);
        Oam_SetExeTimer(pLinkState);
        break;
    /*���ģʽ�£��յ�һ����ɱ����ʱ��������һ�η��������ʱ�����趨ʱ��*/
    case SUCC_CMD_NOT_FINISHED:
        Oam_KillExeTimer(pLinkState);
        break;
    case CLI_RET_KEEP_ALIVE_NOPARA:    
        /*����Execute�˶�ʱ����ֱ�ӷ���*/
        Oam_KillExeTimer(pLinkState);
        Oam_SetExeTimer(pLinkState);
        /*611000748643 ��ӱ�����Ϣ*/
        Oam_Cfg_SendKeepAliveMsgToTelnet(pLinkState);
        return;
    default:
        /* ������ɱ��Execute�˶�ʱ�� */
        if (OAM_MSG_IS_LAST_PACKET(ptRcvMsg) &&  (ptRcvMsg->SeqNo != 0))
        {
            Oam_KillExeTimer(pLinkState);
        }
        break;
    }

    /* ������Ϣ�������� */
    pLinkState->wAppMsgReturnCode = ptRcvMsg->ReturnCode;
    
    if (!Oam_LinkIfRecvAsyncMsg(pLinkState))
    {
        memcpy(&pLinkState->LastData, &ptRcvMsg->LastData, sizeof(ptRcvMsg->LastData));
        memcpy(&pLinkState->BUF, &ptRcvMsg->BUF, sizeof(ptRcvMsg->BUF));
    }

    /* ���ݷ���������ͬ�Ĵ��� */
    switch(ptRcvMsg->ReturnCode)
    {
    case SUCC_TERMINATE_INTERPT_TIMER:
        break;

    case SUCC_AND_NOPARA:
        pLinkState->bOutputResult    = FALSE;
        *(pLinkState->sOutputResult) = MARK_STRINGEND;

        OAM_ChangeMode(pLinkState, NEXTMODE_NORMAL);

        /*���������ת���߻��ǻָ�������Ҫtelnet��ʾ*/
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

        /* �ı䵱ǰ״̬ */
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
        /*���������ת���߻��ǻָ�������Ҫtelnet��ʾ*/
        if (Oam_CfgCurLinkIsOffline2Online(pLinkState))
        { 
            XOS_SysLog(OAM_CFG_LOG_NOTICE,
                   "OFFLINE TO ONLINE [brs return] cmdid = 0x%x, returncode = %u\n", (unsigned)ptRcvMsg->CmdID, (unsigned)ptRcvMsg->ReturnCode);
            Oam_CfgGetNextCmdFromBrs();
        }
        else if(S_Restore != XOS_GetCurState(PROCTYPE_OAM_CLIMANAGER))
        {
            /* ���������� */
            ParseReturnData(pLinkState, ptRcvMsg);
        }

        /* �ı䵱ǰ״̬ */
        if (Oam_LinkIfRecvLastPktOfApp(pLinkState))
        {
            /* �����ǰ�Ǵ���״̬����BRS�������һ��֮�󣬴������û����ȫ����(���滹��ͬ��txt������Ȳ���)
               ��ʱ�����ܽ���·״̬��ΪTelnet��Ӧ�ñ���Execute״̬*/
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
* �������ƣ�INT32 CheckExeMsgHead
* �������������Execute��Ϣͷ
* ���ʵı���
* �޸ĵı���
* ���������TYPE_LINK_STATE *pLinkState,MSG_COMM_OAM * ptMsg
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
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
            /*ͬһ����ID*/
            if (ptMsg->CmdID == Oam_LinkGetCurCmdId(pLinkState))
            {
                return MSGERR_SEQNO;
            }
            /*��ͬ����-��Ϣ��������*/
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

        /* 1.�Ƿ�����ID */
        if (ptMsg->CmdID == 0xFFFFFFFF)
            return MSGERR_CMDID;
        /* 2.��ͨ����Ļ�Ӧ */
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
* �������ƣ�VOID ParseReturnData
* �������������ݻ��Խű�������������
* ���ʵı���
* �޸ĵı���
* ���������TYPE_LINK_STATE *pLinkState,MSG_COMM_OAM * ptMsg
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
static void ParseReturnData(TYPE_LINK_STATE *pLinkState,MSG_COMM_OAM * ptMsg)
{
    CHAR *pFormatStr = NULL;
    pLinkState->bOutputMode = ptMsg->OutputMode;

    /* ��ȡ���Ը�ʽ�� */
    pFormatStr = GetFormatByCmdAttrPos(pLinkState->wCmdAttrPos, pLinkState->tMatchResult.cfgDat);
    if (!pFormatStr)
    {
        SendErrStringToClient(pLinkState,Hdr_szExeNoDisplay);
        return;
    }

    /* ���ݻ��Ը�ʽ�����ɻ����ַ��� */
    if (DISPLAY_SUCCESS == OamCfgintpSendToBuffer(pLinkState, ptMsg, pFormatStr,&pLinkState->tDispGlobalPara))
    {
        pLinkState->bOutputResult = TRUE;
    }
    else
    {
        /*���������������Ϊ�е��������Ϊ���Ըı�outputmode = OUTPUT_MODE_REQUEST��Ӱ���������ִ��*/
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
* �������ƣ�VOID PrintDataType
* ������������ӡ��������
* ���ʵı���
* �޸ĵı���
* ���������
  BYTE bType
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
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
* �������ƣ�BYTE *PrintTable
* ������������ӡ��
* ���ʵı���
* �޸ĵı���
* ���������
  BYTE *pData, BYTE bRecord
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
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
* �������ƣ�VOID PrintMSGCOMMOAMStruct
* ������������ӡMSG_COMM_OAM�ṹ
* ���ʵı���
* �޸ĵı���
* ���������
  MSG_COMM_OAM *ptMsg, CHAR* pDesc
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
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

        /*table����*/
        if ((DATA_TYPE_TABLE == pOprData->Type) || (DATA_TYPE_LIST == pOprData->Type))
        {
            pOprData = (OPR_DATA *)PrintTable(pOprData->Data, pOprData->Len, pOprData->Type);
        }
        /*��������*/
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
@brief ��CLI ��Ӧ��֮���շ���Ϣ�Ĵ�ӡ���أ�����CLI�����������ȶ�λ
@li @b ����б�
@verbatim
��
@endverbatim

@li @b ����ֵ�б�
@verbatim
��
@endverbatim

@li @b ���÷�Χ��
\n������OMP/CMM2���壬��oammanager������

@li @b ������ʾ��
\n��
 
@li @b ����˵����
\n1��ֻ��ʾCLI��Ӧ��ֱ�ӵ�EV_OAM_CFG_CFG��Ϣ��Ҳ����MSG_COMM_OAM�ṹ����
2��CLI����Ӧ�õĻ���Ӧ�÷���CLI�ĸ�����Ϣ��ӡʱ��Ŀ�ͷ�ͽ�β<>��������������
3���ú���ֻ�Ǵ򿪿��أ������ӡ��Ӧ��ִ������ʱ�����
@li @b ���ʾ����
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
@li @b �޸ļ�¼��
-#  �� 
*/
/*****************************************************************************/
void OAM_DbgOpenCliMsgPrint(void)
{
    bDebugPrint = TRUE;
}

/*****************************************************************************/
/**
@brief �ر�OAM_DbgShowCliMsg�����򿪵�CLI ��Ӧ��֮���շ���Ϣ�Ĵ�ӡ����
@verbatim
��
@endverbatim

@li @b ����ֵ�б�
@verbatim
��
@endverbatim

@li @b ���÷�Χ��
\n������OMP/CMM2���壬��oammanager������

@li @b ������ʾ��
\n��
 
@li @b ����˵����
\n��
@li @b ���ʾ����
\n��
@li @b �޸ļ�¼��
-#  �� 
*/
/*****************************************************************************/
void OAM_DbgCloseCliMsgPrint(void)
{
    bDebugPrint = FALSE;
}

/**************************************************************************
* �������ƣ�VOID ShowOAMDebugMode
* ������������ʾOAMDebugMode����
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
void ShowOAMDebugMode(void)
{
    if (bDebugPrint)
        printf("bDebugPrint = TRUE\n");
    else
        printf("bDebugPrint = FALSE\n");
}

/**************************************************************************
* �������ƣ�VOID ExecAppProc
* ����������Ӧ��ҵ������
* ���ʵı���
* �޸ĵı���
* ���������
    MSG_COMM_OAM *ptRcvMsg, 
    JID tJid
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2008/08/08    V1.0    ������      ����
**************************************************************************/
void ExecAppProc(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *ptRcvMsg, JID tJid)
{
/*���������ת���ߵ������Ҫ���⴦��һ��
       ʹ��brs����ʱ���jid*/
    if (Oam_CfgCurCmdIsOffline2Online(ptRcvMsg))
    {
        /*��ȡbrs pid*/
        Oam_CfgGetBrsPidForOffline2Online(&tJid);
    }
        
    {
#if 0
/*611000761985 �ڲ���ת��ʱ����λ���㱣֤����ֽ��򣬲����жϱ����ֽ�����ת��*/
        /*���ڵ�ַ����������͵Ĳ��������ڽ��������ڲ���д���Ĺ̶����ֽ��������ڷ���֮ǰ��Ҫת��Ϊ
        �뵱ǰģ��һ�µ��ֽ���*/
        if (Oam_CfgIsNeedInvertOrder())
        {
            CfgMsgTransAddrByteOder(ptRcvMsg);
        }
#endif
        /* ����ͣ����ý�����Ϣ�ϱ��ȴ���ʱ�� */
        Oam_KillCmdPlanTimer(pLinkState);
        Oam_SetCmdPlanTimer(pLinkState);

        XOS_SendAsynMsg(EV_OAM_CFG_CFG, (BYTE *)ptRcvMsg, ptRcvMsg->DataLen, XOS_MSG_VER0, XOS_MSG_MEDIUM, &tJid);
    }

    return;
}

/**************************************************************************
* �������ƣ�VOID ExecCLIProc
* �������������ݿ�ִ��ģʽ������
* ���ʵı���
* �޸ĵı���
* ���������
    MSG_COMM_OAM *ptRcvMsg, 
    MSG_COMM_OAM *ptRtnMsg
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
void ExecCLIProc(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *ptRcvMsg, MSG_COMM_OAM *ptRtnMsg)
{
    BYTE ucRet = CheckGetOrSet(ptRcvMsg->CmdID);

    if (ucRet == IS_GET)                     /* Get���� */
        ExecCLIGet(pLinkState, ptRcvMsg);
    else if(ucRet == IS_SET)                 /* Set���� */
        ExecCLISet(pLinkState, ptRcvMsg, ptRtnMsg);

    return;
}

/**************************************************************************
* �������ƣ�VOID GetTerminalCfg
* ������������ȡ��ǰ�û��ն���Ϣ
* ���ʵı���
* �޸ĵı���
* ���������
    MSG_COMM_OAM *ptRcvMsg, 
    MSG_COMM_OAM *ptRtnMsg
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
void GetTerminalCfg(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *ptRtnMsg)
{
    GetTerminalCfgData(&ptRtnMsg);
    Oam_CmdReturnMsgHandler(pLinkState, ptRtnMsg);

    return;
}

/**************************************************************************
* �������ƣ�VOID ExecCLIGet
* �������������ݿ�ִ��ģʽGET����
* ���ʵı���
* �޸ĵı���
* ���������
    MSG_COMM_OAM *ptRcvMsg, 
    MSG_COMM_OAM *ptRtnMsg
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
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
* �������ƣ�VOID ExecCLISet
* �������������ݿ�ִ��ģʽSET����
* ���ʵı���
* �޸ĵı���
* ���������
    MSG_COMM_OAM *ptRcvMsg, 
    MSG_COMM_OAM *ptRtnMsg
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
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
* �������ƣ�VOID GetOnlineUsers
* ������������ȡ�����û��б�
* ���ʵı���
* �޸ĵı���
* ���������
    MSG_COMM_OAM *ptRcvMsg, 
    MSG_COMM_OAM *ptRtnMsg
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
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
        
        /*���Ƿ���Ҫ�ְ�����*/
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
* �������ƣ�VOID GetUserName
* ������������ȡ�û��б�
* ���ʵı���
* �޸ĵı���
* ���������
    MSG_COMM_OAM *ptRcvMsg, 
    MSG_COMM_OAM *ptRtnMsg
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
void GetUserName(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *ptRtnMsg)
{

    GetUserNameData(&ptRtnMsg);
    Oam_CmdReturnMsgHandler(pLinkState, ptRtnMsg);

    return;
}

/**************************************************************************
* �������ƣ�VOID SetUserName
* ��������������/ɾ��/�޸��û�
* ���ʵı���
* �޸ĵı���
* ���������
    MSG_COMM_OAM *ptRcvMsg, 
    MSG_COMM_OAM *ptRtnMsg
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
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
* �������ƣ�VOID SetEnablePassword
* ����������ɾ��/�޸���Ȩ����
* ���ʵı���
* �޸ĵı���
* ���������
    MSG_COMM_OAM *ptRcvMsg, 
    MSG_COMM_OAM *ptRtnMsg
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
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
* �������ƣ�VOID SetConsoleIdleTimeout
* ����������ɾ��/�޸�wConsoleIdleTimeout
* ���ʵı���
* �޸ĵı���
* ���������
    MSG_COMM_OAM *ptRcvMsg, 
    MSG_COMM_OAM *ptRtnMsg
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
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
* �������ƣ�VOID SetConsoleAbsTimeout
* ����������ɾ��/�޸�wConsoleAbsTimeout
* ���ʵı���
* �޸ĵı���
* ���������
    MSG_COMM_OAM *ptRcvMsg, 
    MSG_COMM_OAM *ptRtnMsg
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
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
* �������ƣ�VOID SetTelnetIdleTimeout
* ����������ɾ��/�޸�wTelnetIdleTimeout
* ���ʵı���
* �޸ĵı���
* ���������
    MSG_COMM_OAM *ptRcvMsg, 
    MSG_COMM_OAM *ptRtnMsg
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
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
* �������ƣ�VOID SetTelnetAbsTimeout
* ����������ɾ��/�޸�wTelnetAbsTimeout
* ���ʵı���
* �޸ĵı���
* ���������
    MSG_COMM_OAM *ptRcvMsg, 
    MSG_COMM_OAM *ptRtnMsg
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
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
    
    /*����DBS��JID*/
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

    /* ����ͣ����ý�����Ϣ�ϱ��ȴ���ʱ�� */
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
    
    /* RPU����ֻ���������� */
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

        /*����Execute�˶�ʱ��*/
        Oam_KillExeTimer(pLinkState);
        Oam_SetExeTimer(pLinkState);

        Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_EXECUTE);
    }
    return;
}

/**************************************************************************
* �������ƣ�VOID ExecSaveCmdProc
* ���������������������
* ���ʵı���
* �޸ĵı���
* ���������
    MSG_COMM_OAM *ptRcvMsg, 
    JID tJid
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2008/08/08    V1.0    ������      ����
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

    /*�ж��Ƿ���OMP*/
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

        /*�ж��Ƿ���LOMP*/
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

    /*�ȴ�ZDB���ȵ�DBS���ؽ��֮�����ж��Ƿ���Ҫ��TXT*/
    SaveZDBProc(pLinkState, ptRcvMsg);
    
    return;
}

/**************************************************************************
* �������ƣ�IfModuleIsLomp
* ��������������ģ��ż��һ��ģ���Ƿ���LOMP
* ���ʵı���
* �޸ĵı���
* ���������
    MSG_COMM_OAM *ptRcvMsg, 
    JID tJid
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/05/27    V1.0    ���      ����
**************************************************************************/
BOOLEAN IfModuleIsLomp(WORD16 wModule)
{
    WORD32 wLoop = 0;
    BOOLEAN bRet = FALSE;

    if (g_ptOamIntVar)
    {        
        /*�����������ע����Ϣ*/
        for(wLoop=0; wLoop< g_ptOamIntVar->tLompCliAppRegInfo.dwRegCount; wLoop++)
        {
            if(wLoop >= OAM_CLI_APP_SHM_NUM)
            {
                bRet = FALSE;
                break;
            }

            /* �Ѿ�ע����� */
            if(wModule == g_ptOamIntVar->tLompCliAppRegInfo.tCliReg[wLoop].tAppJid.wModule)
            {
                bRet = TRUE;
                break;
            }
        }
    }

    /*���������ע�����û�У���鹲���ڴ�*/
    if(!bRet)
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
* �������ƣ�BOOL CheckMsgHead
* ���������������Ϣͷ�Ƿ�Ϸ�
* ���ʵı���
* �޸ĵı���
* ���������
    MSG_COMM_OAM *ptMsg
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
BOOL CheckMsgHead(MSG_COMM_OAM *ptMsg)
{
    WORD16 wMsgLenTmp = 0;
    
    XOS_GetMsgDataLen(PROCTYPE_OAM_CLIMANAGER, &wMsgLenTmp);
    
    if (ptMsg->CmdID   <= 0 ||                    /*����ID����Ϊ����*/
            ptMsg->DataLen < sizeof(MSG_COMM_OAM) ||  /*���ݳ��Ȳ���С�ڽṹ����*/
            ptMsg->DataLen > MAX_APPMSG_LEN||        /*���ݳ��Ȳ��ܳ����涨����*/
            ptMsg->DataLen != wMsgLenTmp)             /*���ݳ���Ҫ������Ϣ����*/
        return FALSE;
    else
        return TRUE;
}

/**************************************************************************
* �������ƣ�BYTE CheckGetOrSet
* ���������������Get����Set����: 0 ������Get����  1 ������Set����
                                  2 TRAP 3 ADD 4 DEC 5 PROCESS 6 INVALID-MODE        
* ���ʵı���
* �޸ĵı���
* ���������
    DWORD CmdID
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
BYTE CheckGetOrSet(DWORD CmdID)
{
    CMDID *pGetGSFlag;

    pGetGSFlag = (CMDID *)(&CmdID);

    return (BYTE)pGetGSFlag->OPM;
}

/**************************************************************************
* �������ƣ�BOOL IsSaveCmd
* ��������������Ƿ��������
                                  0 GET 1 SET 2 SAVE 3 DEBUG 4 PROCESS        
* ���ʵı���
* �޸ĵı���
* ���������
    DWORD CmdID
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2008/08/07    V1.0    ������      ����
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
* �������ƣ�DWORD GetMsgStructSize
* ����������
* ���ʵı���
* �޸ĵı���
* ���������
    MSG_COMM_OAM *ptMsg
    OPR_DATA *pOprData
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
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
* �������ƣ�VOID SetOprData
* ����������
* ���ʵı���
* �޸ĵı���
* ���������
    MSG_COMM_OAM *ptMsg,
    OPR_DATA *pOprData,
    BYTE bType,
    BYTE *value
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
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
* �������ƣ�VOID SetMapOprData
* ����������
* ���ʵı���
* �޸ĵı���
* ���������
    MSG_COMM_OAM *ptMsg,
    OPR_DATA *pOprData,
    WORD16 v1, WORD16 v2
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
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
* �������ƣ�VOID SetStringOprData
* ����������
* ���ʵı���
* �޸ĵı���
* ���������
    MSG_COMM_OAM *ptMsg,
    OPR_DATA *pOprData,
    BYTE *szValue,
    WORD16 wMaxLen
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
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

    /*�ֽ���ת��*/
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
            /** @brief ����Ŀ¼���̶�ʧ��*/
            if (pSaveResult->tDatabaseSaveResult[wDbNum].wReason == _DB_E_SAVE_BOTHPATH)
                strncat(errStr, Hdr_szSaveZDBErrSaveBothPath, dwBufSize);
            /** @brief ��Ŀ¼����ʧ��*/
            else if (pSaveResult->tDatabaseSaveResult[wDbNum].wReason == _DB_E_SAVE_MAINPATH)
                strncat(errStr, Hdr_szSaveZDBErrMainPath, dwBufSize);
            /** @brief��Ŀ¼����ʧ��*/
            else if (pSaveResult->tDatabaseSaveResult[wDbNum].wReason == _DB_E_SAVE_BACKUP)
                strncat(errStr, Hdr_szSaveZDBErrBackup, dwBufSize);
            /** @briefû�б���Ҫ����*/
            else if (pSaveResult->tDatabaseSaveResult[wDbNum].wReason == _DB_NOT_TABLE_SAVE)
                strncat(errStr, Hdr_szSaveZDBErrNoTblSave, dwBufSize);
            /** @brief��ȡ�ļ����ȴ���*/
            else if (pSaveResult->tDatabaseSaveResult[wDbNum].wReason == _DB_READ_FILE_ERROR)
                strncat(errStr, Hdr_szSaveZDBErrReadFileErr, dwBufSize);
            /** @brief����Ҫ����*/
            else if (pSaveResult->tDatabaseSaveResult[wDbNum].wReason == _DB_TABLE_NOT_NEED_SAVE)
                strncat(errStr, Hdr_szSaveZDBErrTblNotNeedSave, dwBufSize);
            /** @brief �����ڽ��б������� */
            else if (pSaveResult->tDatabaseSaveResult[wDbNum].wReason == _DB_E_LOCAL_SAVING)
                strncat(errStr, Hdr_szSaveZDBErrLocalSaving, dwBufSize);
            /** @brief �����ڽ����������� */
            else if (pSaveResult->tDatabaseSaveResult[wDbNum].wReason == _DB_E_MS_SAVING)
                strncat(errStr, Hdr_szSaveZDBErrMSSaving, dwBufSize);
            /** @brief �����ڽ����ϵ���� */
            else if (pSaveResult->tDatabaseSaveResult[wDbNum].wReason == _DB_E_POWERON_LOADING)
                strncat(errStr, Hdr_szSaveZDBErrPowerOnLoading, dwBufSize);
            /** @brief �����ڽ���RX FTP���� */
            else if (pSaveResult->tDatabaseSaveResult[wDbNum].wReason == _DB_E_FTP_LOADING)
                strncat(errStr, Hdr_szSaveZDBErrFtpLoading, dwBufSize);
            /** @brief �����ڽ���APP FTP���� */
            else if (pSaveResult->tDatabaseSaveResult[wDbNum].wReason == _DB_E_APPFTP_LOADING)
                strncat(errStr, Hdr_szSaveZDBErrAppFtpLoading, dwBufSize);
            /** @brief IO����æ */
            else if (pSaveResult->tDatabaseSaveResult[wDbNum].wReason == _DB_E_IOBUSY)
                strncat(errStr, Hdr_szSaveZDBErrIOBusy, dwBufSize);
            /** @brief �Ƿ���� */
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
* �������ƣ�VOID SendPacketToExecute
* ����������������Ϣ��Execute
* ���ʵı���
* �޸ĵı���
* ���������MSG_COMM_OAM *ptMsg
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
void SendPacketToExecute(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *ptMsg, JID tJid)
{
    WORD16 wMsgHeadLen = (WORD16)((BYTE *)ptMsg->Data - (BYTE *)ptMsg);
    /*���������ptMsg�Ѿ�����aucSendBuf�������������ͻ���Ϣ��aucRecvBuf*/
    MSG_COMM_OAM *ptRtnMsg = (MSG_COMM_OAM *)g_ptOamIntVar->aucRecvBuf;

    TYPE_MATCH_RESULT *pMatchResult = &(pLinkState->tMatchResult);
    WORD16 wCmdAttrPos = pMatchResult->wCmdAttrPos;
    T_OAM_Cfg_Dat* cfgDat = pMatchResult->cfgDat;

    memcpy(ptRtnMsg, ptMsg, wMsgHeadLen);

    /*  ptRtnMsg->PacketNo   = INIT_PACKET_NO;       ���Ŵ�1��ʼ */
    ptRtnMsg->OutputMode = OUTPUT_MODE_REQUEST; /*OUTPUT_MODE_NORMAL;     ��ʼʱTelnet���ģʽΪrequest */
    ptRtnMsg->Number     = 0;
/* ���ͺ�ִ���Ѿ���һ�����Ͳ���Ҫ���
    if (!CheckMsgHead(ptMsg))
    {
        ptRtnMsg->ReturnCode = ERR_MSG_FROM_INTPR;
        ptRtnMsg->DataLen    = sizeof(MSG_COMM_OAM);
        Oam_CmdReturnMsgHandler(pLinkState,ptRtnMsg);
        return;
    }
*/
    /*����Ǳ����������ȷ�����Ϣ�����忴�����Ƿ���Ӧ��*/
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

/*�����첽��Ϣ��debug��*/
static void ProcessAsycMsg(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *pMsg)
{ 
    /*611001435262 debug��Ϣ�ӿڸ�Ϊ���Է���n���ַ�������*/
    OPR_DATA *pRtnOprData = NULL;
    WORD16 i = 0;
    WORD16 iBufSize = MAX_OAMMSG_LEN - sizeof(TYPE_INTERPRET_MSGHEAD) -3;
    static CHAR ucaBuf[MAX_OAMMSG_LEN];
    memset(ucaBuf, 0, sizeof(ucaBuf));
	
    /*���Ŀǰtelsvr���뷽ʽ�����첽�Ͳ���ʾdebug��Ϣ*/ 
    if (TRUE== Oam_LinkIfTelSvrInputModeIsSyn(pLinkState))
    {
        pLinkState->bRecvAsynMsg = FALSE;
        XOS_SysLog(OAM_CFG_LOG_NOTICE, "receive async msg, but terminal-display not opened, discarded!");
	 return;	
    }
	
    /*debug��Ϣ��������ʾ*/
    ucaBuf[0] = '\n';
    /*ƴ���յ����ַ������Ͳ���*/
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

    /* ��������Ϣ���͵�telnet��ʾ */
    XOS_snprintf(pPlanStr, MAX_STRING_LEN, map_szExePlanFmtStr,pPlanInfo->ucPlanValue);
    /*pLinkState->bOutputCmdLine = FALSE;*/
    pLinkState->bOutputMode = OUTPUT_MODE_ROLL;
    pLinkState->wAppMsgReturnCode = SUCC_WAIT_AND_HAVEPARA;
    /*���������100������һ�����з�������λΪ0*/
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

    /*�ж�BIT31��־λ*/
    iType = ((pLinkState->ucLinkID)&0X00000200)>>9; /*BIT9 1-��ʾXMLӦ��0-CLIӦ��*/
    if (iType == LINKSTATE_TYPE_CLI)
    {
        XOS_SysLog(OAM_CFG_LOG_DEBUG, "Oam_DealPlanTimer: LINKSTATE_TYPE_CLI == iLinkStateType!");

        Oam_KillCmdPlanTimer(pLinkState);
        Oam_SetCmdPlanTimer(pLinkState);

        /* ��������Ϣ���͵�telnet��ʾ */
        XOS_snprintf(pPlanStr, MAX_STRING_LEN, map_szExePlanFmtStr,pLinkState->ucPlanValuePrev);
        /* ��¼��ԭ�ȵ�ֵ���ڷ��ͽ���֮ǰ���丳ֵΪFALSE��������֮���ٻָ�ԭֵ */
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
    
    /* ���ļ�������ļ��������򴴽����ļ� */
    dwRtn = XOS_OpenFile(acCmdLogFile, XOS_CREAT, (XOS_FD *)&dwFileFD);
    if (XOS_SUCCESS != dwRtn)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[WriteLogOfCmdLine]Open file fail! dwRtn = %d\n", dwRtn);
        return FALSE;
    }

    /* ��λ���ļ�β�� */
    dwRtn = XOS_SeekFile(dwFileFD, XOS_SEEK_END, 0);
    if (XOS_SUCCESS != dwRtn)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[WriteLogOfCmdLine]Seek file fail! dwRtn = %d\n", dwRtn);
        XOS_CloseFile(dwFileFD);
        return FALSE;
    }

    /* ��֯LOG�ַ��� */
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

    /* д��֮�󣬹ر��ļ�FD */
            XOS_CloseFile(dwFileFD);

    /* д��ɹ�֮�󣬽���־�ϱ�SYSLOG������ */
    XOS_SysLog(OAM_CFG_LOG_NOTICE, pLogBuf);

    /* д��һ����־�ɹ�֮�󣬽���־������1 */
    g_ptOamIntVar->wLinesOfCmdLogFile++;

    /* ��������ļ�����������򽫵�ǰ�ļ�����Ϊ�����ļ���������µ���������־д���µ��ļ��� */
    if (g_ptOamIntVar->wLinesOfCmdLogFile >= OAM_MAX_CMD_LOG_LINES)
            {
        /*ɾ�������ļ�*/
        dwRtn = XOS_DeleteFile(acCmdLogFileBak);
        if (XOS_SUCCESS != dwRtn)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, 
                "[WriteLogOfCmdLine]Delete file <%s> fail! dwRtn = %d\n", acCmdLogFileBak, dwRtn);
        }

        /* ����Ϊ�����ļ� */
        dwRtn = XOS_RenameFile(acCmdLogFile, acCmdLogFileBak);
        if (XOS_SUCCESS != dwRtn)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, 
                "[WriteLogOfCmdLine]Rename file fail! dwRtn = %d\n", dwRtn);
            return FALSE;
        }

        /* ��ȫ�ּ�¼����Ϊ0 */
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

    /* ���ļ���ֻ�� */
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
* �������ƣ�VOID Oam_RecvFromCliApp
* �����������������Խ����ڲ�����Ϣ
* ���ʵı���
* �޸ĵı���
* ���������
  LPVOID pMsgPara
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
void Oam_RecvFromCliApp(WORD32 dwMsgId,LPVOID pMsgPara)
{
    int iLinkStateType = 0;
    /*�ж�LinkState����*/
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
        /* �յ�Ӧ�õķ��ؽ��֮��ɱ�������ϱ��ȴ���ʱ�� */
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
    /*��ӡ�շ���Ϣ*/
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
    if (dwCmdGrp == OAM_CLI_DDM_CMDGRP_SET)        // DDM����SET��
        ucCmdType = IS_SET;
    else if (dwCmdGrp == OAM_CLI_DDM_CMDGRP_GET)   // DDM����GET��
        ucCmdType = IS_GET;
    else                                           // ��ͨCLI����
        ucCmdType = CheckGetOrSet(ptRcvMsg->CmdID);
    
    if ((ptRcvMsg->ucOamTag == OAMCFG_TAG_NORMAL) && 
        ((ucCmdType == IS_SET) || (ucCmdType == IS_SAVE))&&
        (OAM_MSG_IS_LAST_PACKET(ptRcvMsg)) &&
        (ptRcvMsg->ReturnCode != CLI_RET_KEEP_ALIVE_NOPARA))
    {
        /* д��������־ */
        if (!WriteLogOfCmdLine(&pLinkState->tCmdLog, bCmdExecRslt))
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, "[Oam_RecvFromApp] WriteLogOfCmdLine exec fail! line = %d\n", __LINE__);
        }
    }

    /*�����Ϣͷ�Ƿ�Ϸ�*/
    if (ptRcvMsg->CmdID   <= 0 || ptRcvMsg->DataLen < sizeof(MSG_COMM_OAM) ||ptRcvMsg->DataLen > MAX_APPMSG_LEN) 
    {
         ptRcvMsg->ReturnCode = ERR_MSG_FROM_INTPR;
         ptRcvMsg->DataLen    = sizeof(MSG_COMM_OAM);
         Oam_CmdReturnMsgHandler(pLinkState,ptRcvMsg);
         return;   
    }
	
    /*?? brs�ķ��������10ʱ��Ҫת��Ϊ������map����*/
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
    MSG_COMM_OAM *ptOamMsgBuf = NULL;                       /*OAM��Ϣ����ʱָ��*/  	
    /*����ж�*/
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
    /*?? brs�ķ��������10ʱ��Ҫת��Ϊ������map����*/
    if (ptOamMsgBuf->ReturnCode >= 10 &&
        ptOamMsgBuf->ReturnCode != SUCC_WAIT_AND_HAVEPARA) 
    {
        ConstructBrsMapPara(ptOamMsgBuf);
    }
    /*����ƽ̨Ӧ������*/
//    SendPLATAckToOMM(ptOamMsgBuf);        
    return;
}

/**************************************************************************
* �������ƣ�Oam_SaveCmdLompErr
* �������������̹������ж�lomp��omp�������ڲ����������
                              ��������Ϣ���أ�ִ��״̬�޸�Ϊtelnet
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ��   ��
* ����˵�������	�����ܹ�����Ϣ���ɽ�������ƥ����ʽ����
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/06/08   V1.0     ���     ����
**************************************************************************/           
void Oam_InnerSaveCmdLompErr(LPVOID ptOamMsg,WORD32 dwMsgId, CHAR *pStrOutResult)
{
    BOOLEAN bOR = FALSE;
    TYPE_LINK_STATE *pLinkState = NULL;
    
    /*����ж�*/
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
    /* ɱ����ʱ�� */
    Oam_KillExeTimer(pLinkState);
    SendTxtSaveAckToClient(pLinkState,pStrOutResult,bOR);
    Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_TELNET);
    return;
}
void  Oam_InnerKillCmdPlanTimer(LPVOID ptOamMsg,WORD32 dwMsgId)
{
    TYPE_LINK_STATE *pLinkState = NULL;
    /*����ж�*/
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
    /*����ж�*/
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
����:�Ƚ������ַ����������ִ�Сд
����: s1��s2 -- ����Ƚϵ��ַ�����
             n --����Ƚϵ��ַ�����
����ֵ: 0 -- �����ַ������
                  ���� 0 -- s1 > s2
                  С�� 0 -- s1 < s2
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


