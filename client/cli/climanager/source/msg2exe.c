/**************************************************************************
* ��Ȩ���� (C)2002, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ�HrExeMsg.c
* �ļ���ʶ��
* ����ժҪ��������Ϣ��Execute
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
* �޸ļ�¼2��
**************************************************************************/
/**************************************************************************
*                           ͷ�ļ�                                        *
**************************************************************************/
#include "includes.h"
#include "offline_to_online.h"
#include "oam_execute.h"
#include "cliapp.h"

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
/**************************************************************************
*                          ���ر���                                       *
**************************************************************************/
/**************************************************************************
*                          �ֲ�����ԭ��                                   *
**************************************************************************/
/**************************************************************************
*                          �ֲ�����ʵ��                                   *
**************************************************************************/
/**************************************************************************
*                          ȫ�ֺ���ʵ��                                   *
**************************************************************************/
/*extern  void RecvFromInterpret(LPVOID pMsgPara, JID);*/
extern BOOL bDebugPrint;
extern void SendPacketToExecute(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *ptMsg, JID tJid);
/**************************************************************************
* �������ƣ�MSG_COMM_OAM *OamCfgConstructMsgData
* ����������������Ϣ��
* ���ʵı���
* �޸ĵı���
* ���������TYPE_LINK_STATE *pLinkState
* ���������
* �� �� ֵ��    NULL - ��������
                                ��NULL - �����ɹ�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
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

    /*���ܴ�����·*/
    if(pLinkState == &gt_SaveLinkState)
    {
        TYPE_XMLLINKSTATE *ptXmlLinkState = NULL;
        /*�������ڴ��̵����ܶ�*/
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
*                          ȫ�ֺ���ʵ��                                   *
**************************************************************************/

/**************************************************************************
* �������ƣ�VOID ExecCeaseOmmCfgCmd
* ����������ȡ��������������
* ���ʵı���
* �޸ĵı���
* ���������TYPE_XMLLINKSTATE *pLinkState
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/04/18    V1.0    ������      ����
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
    /*���ö�ʱ��*/
    Xml_KillExeTimer(pXmlLinkState);
    Xml_SetExeTimer(pXmlLinkState);
    /*��ƽ̨Ӧ�÷�����ֹ����*/
    XOS_SendAsynMsg(EV_OAM_CFG_CFG, (BYTE *)ptSendMsg, ptSendMsg->DataLen, XOS_MSG_VER0, XOS_MSG_MEDIUM,ptDtJid);
    return;
}
/**************************************************************************
* �������ƣ�VOID ExecCeaseProtocolCmd
* ����������ȡ��Э��ջ��������
* ���ʵı���
* �޸ĵı���
* ���������TYPE_LINK_STATE *pLinkState
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
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
* �������ƣ�VOID ExecSendCmdAgain
* �����������ط�����
* ���ʵı���
* �޸ĵı���
* ���������TYPE_LINK_STATE *pLinkState
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
void ExecSendCmdAgain(TYPE_LINK_STATE *pLinkState)
{
    MSG_COMM_OAM *ptRtnMsg = (MSG_COMM_OAM *)g_ptOamIntVar->aucRecvBuf;/*ģʽջ��ѯ���*/	
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
    
    /*����Execute�˶�ʱ��*/
    Oam_KillExeTimer(pLinkState);
    Oam_SetExeTimer(pLinkState);

    Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_EXECUTE);
    
    return;
}

/**************************************************************************
* �������ƣ�VOID DisposeMPCmd
* ��������������MP����
* ���ʵı���
* �޸ĵı���
* ���������TYPE_LINK_STATE *pLinkState
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
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
    
    /*����ͻ��˲���ͬʱִ��ǰ̨����*/
    if (CheckMpExecuteMutex(pLinkState, ptSendMsg))
    {
        if (!Xml_CfgCurLinkIsExcutingSave(pLinkState))/*��ǰ���������ڴ���*/
        {
            /*����Ƿ�����·���ڴ���*/
            if (CheckLinkIsExecutingSaveCmd() && (S_CfgSave == XOS_GetCurState(PROCTYPE_OAM_CLIMANAGER)))
            {
                SendStringToTelnet(pLinkState, Hdr_szSaveMutexTip);
                return;
            }
            /*����Ǵ���������������·�Ƿ���ִ������*/
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
	 
        /*����Execute�˶�ʱ��*/
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
        /*��ʾ�û��������û�����ִ������Ժ�����*/
        SendStringToTelnet(pLinkState, Hdr_szCommandMutexTip);
        return;
    }

    return; 
}

/**************************************************************************
* �������ƣ�BOOL CheckMpExecuteMutex
* ��������������ͻ��˼�������������ִ�л������
* ���ʵı���
* �޸ĵı���
* ���������TYPE_LINK_STATE *pLinkState
* ���������TRUE - û�������ͻ�����ִ��
*                             FALSE - �������ͻ�������ִ��
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2008/03/12    V1.0    ���      ����
**************************************************************************/
BOOL CheckMpExecuteMutex(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *ptMsg)
{
    WORD32 dwCmdType = ptMsg->CmdID & 0x00FF0000;
    WORD32 dwTmpCmdType = 0;
    int i = 0;

    /*DDM ���������*/
    WORD32 dwCmdGrp = ptMsg->CmdID & 0xFF000000;
    if (dwCmdGrp == OAM_CLI_DDM_CMDGRP_SET ||
        dwCmdGrp == OAM_CLI_DDM_CMDGRP_GET ||
        dwCmdGrp == OAM_CLI_CMM_CMDGRP ||
        dwCmdGrp == OAM_CLI_DDM_CMDGRP_GETTABLE)
    {
        return TRUE;
    }
	
    /*ͬһ�����͵���������ֻ����һ���û���ִ��*/
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


