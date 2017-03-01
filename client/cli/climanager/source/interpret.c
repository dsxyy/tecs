/**************************************************************************
* ��Ȩ���� (C)2002, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ�Interpret.c
* �ļ���ʶ��
* ����ժҪ��OAM���ͽ������
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
#include "saveprtclcfg.h"
#include "oam_execute.h"
#include "clirecvreg.h"
#include "offline_to_online.h"

/**************************************************************************
*                          ����                                           *
**************************************************************************/
/**************************************************************************
*                          ��                                             *
**************************************************************************/
#define OAM_TO_4BYTE_EDGE(X) (((int)(X) & ~3) + 4)  /*���뵽���ֽڱ�Ե*/
/**************************************************************************
*                          ��������                                       *
**************************************************************************/
/**************************************************************************
*                          ȫ�ֱ���                                       *
**************************************************************************/
T_DAT_LINK *g_tDatLink = NULL;
WORD16    g_wRegDatCount;

/*������Ϣ*/
TYPE_LINK_STATE gtLinkState[MAX_CLI_VTY_NUMBER - MIN_CLI_VTY_NUMBER + 1] = {{0}};
TYPE_LINK_STATE gt_SaveLinkState = {0};/*����������·��Ϣ*/
TYPE_XMLLINKSTATE  gt_XmlLinkState[MAX_SIZE_LINKSTATE] = {{0}};

/*���ýű�*/
COMMON_INFO gtCommonInfo = {{0}};
USER_NODE *pUserLinkHead = NULL;
USER_NODE *pUserLinkTail = NULL;

T_OAM_INTERPRET_VAR *g_ptOamIntVar = NULL;

CHAR g_szSpecifyIP[LEN_IP_ADDR_STR] = {0};
BYTE   gnPasswordIntensity;     /*�Ƿ�Ϊǿ����*/
BYTE   gnNorPasslesslen;          /*�������������С�̶�*/
CHAR  g_aucScriptLanguage[OAM_CLI_LANGTYPE_LEN] = {0}; /* ���������ַ���*/
DWORD gnUserNum = 0;
TYPE_USER_INFO gtUserInfo[MAX_USER_NUM_ROLE_ADMIN + MAX_USER_NUM_ROLE_CONFIG + MAX_USER_NUM_ROLE_VIEW] = {{0}};

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
*                          ȫ�ֺ�������                                   *
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
* �������ƣ�BOOL CheckIsXMLAdapterLinkState()
* ������������鵱ǰ��·�Ƿ�XML������ר����·
* ���ʵı���
* �޸ĵı���
* ���������*ptLinkState:��·��Ϣ
* ���������
* �� �� ֵ��BOOL
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/04/26    V1.0    ������    ����
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
* �������ƣ�VOID XmlShowLinkState()
* �������������Ժ���:�쿴��ǰ��OMM���������
* ���ʵı���
* �޸ĵı���
* ���������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/04/26    V1.0    ������    ����
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
        case pec_MsgHandle_IDLE: /* ��״̬ */
            strncpy(aucState, "Idle", sizeof(aucState));
            break;        
        case pec_MsgHandle_EXECUTE: /* Execute��ִ��״̬ */
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
@brief ��ʾCLIMANAGER�����ӵ�������Ϣ�����ڶ�λ�����쳣���
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
@li @b �޸ļ�¼��
-#  �� 
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
        case pec_MsgHandle_IDLE: /* ��״̬ */
            strncpy(aucState, "Idle", sizeof(aucState));
            break;
        case pec_Init_USERNAME:  /* �û������뼰��֤״̬ */
        case pec_Init_PASSWORD: /* �������뼰��Ȩ״̬ */
            strncpy(aucState, "Logging", sizeof(aucState));
            break;
        case pec_MsgHandle_INIT:
            strncpy(aucState, "Initialize", sizeof(aucState));
            break;
        case pec_MsgHandle_TELNET: /* Telnet��ִ��״̬ */
        case pec_MsgHandle_EXECUTE: /* Execute��ִ��״̬ */
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
* �������ƣ�VOID OamInterpretCheckPtr
* ������������麯��ָ�������
* ���ʵı���
* �޸ĵı���
* ���������
            BYTE *InputPtr
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
void OamInterpretCheckPtr(BYTE *InputPtr, WORD32 dwLine)
{
    assert(InputPtr != NULL);
    if(NULL == InputPtr)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "OamInterpret:OamInterpretCheckPtr(): NULL ptr; line=%d!", dwLine);

        /* ������Ϊ�գ�ֱ���˳� */
        XOS_SetDefaultNextState();
    }
}

/**************************************************************************
* �������ƣ�Xml_CfgCurLinkIsExcutingSave
* ������������ǰ�����Ƿ��ڴ���
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ��   TRUE - ��
                              FALSE - ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/05/05   V1.0    ������      ����
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
    /*�ж�BIT31��־λ*/
    iType = ((pOamMsgBufTmp->LinkChannel)&0X00000200)>>9; /*BIT9 1-��ʾXMLӦ��0-CLIӦ��*/   
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
    /*�ж�BIT31��־λ*/
    iType = ((pPlanMsgBuf->dwLinkChannel)&0X00000200)>>9; /*BIT9 1-��ʾXMLӦ��0-CLIӦ��*/   
    if (iType == LINKSTATE_TYPE_CLI)
    {
        return LINKSTATE_TYPE_CLI;
    }    
    return LINKSTATE_TYPE_XML;    
}

/**************************************************************************
* �������ƣ�TYPE_XMLLINKSTATE *XmlCfgGetLinkState
* ������������ȡXML����
* ���ʵı���
* �޸ĵı���
* �����������
            WORD16 dwMsgId, LPVOID pMsgPara
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/05/05    V1.0    ������      ����
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
* �������ƣ�static TYPE_LINK_STATE *OamCfgGetLinkState
* ������������ȡ����
* ���ʵı���
* �޸ĵı���
* ���������
            WORD16 dwMsgId, LPVOID pMsgPara
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
TYPE_LINK_STATE *OamCfgGetLinkState(WORD32 dwMsgId, LPVOID pMsgPara)
{
    BYTE vty = MIN_CLI_VTY_NUMBER;
    TYPE_LINK_STATE *pSaveLinkState = NULL;/*ģ��Telnet������·*/

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
        if (Xml_CheckIsExecSaving())/*OMM����ִ�д�������*/	 
        {
            pSaveLinkState = &gt_SaveLinkState;            
        }
        else if (Oam_CfgCurCmdIsOffline2Online((MSG_COMM_OAM *)pMsgPara))
        {
            /* ר�õ�linkstate ��������ת���ߴ���*/
            return &g_tLinkStateForOfflineToOnline;
        }
        else
        {
            vty = ((MSG_COMM_OAM *)pMsgPara)->LinkChannel;
	 }
        break;
    case EV_OAM_CFG_PLAN:
        if (Xml_CheckIsExecSaving())/*OMM����ִ�д�������*/     
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
        /*dbs�ӿ���û��vtyid*/
        {
            WORD32 dwCmdId = 0;
            if (Xml_CheckIsExecSaving())/*OMM����ִ�д�������*/	 
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
    /* 614000678048 (ȥ��24Сʱ���Գ�ʱ) */
    /*case EV_TIMER_ABS_TIMEOUT:*/
    case EV_TIMER_CMD_PLAN:
        vty = XOS_GetConstParaOfCurTimer(pMsgPara);
        break;
    case EV_TIMER_EXECUTE_NORMAL:
        if (Xml_CheckIsExecSaving())/*OMM����ִ�д�������*/	 
        {
            pSaveLinkState = &gt_SaveLinkState;            
        }
        else
        {
            vty = XOS_GetConstParaOfCurTimer(pMsgPara);
	 }
        break;
    case OAM_MSG_SAVE_BEGIN:
	 if (Xml_CheckIsExecSaving())/*OMM����ִ�д�������*/	 
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
        /* ר�õ�linkstate ��������ת���ߴ���*/
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
*                          ȫ�ֺ���ʵ��                                   *
**************************************************************************/

/**************************************************************************
* �������ƣ�OPR_DATA *GetNextOprData
* ������������ȡ��һ������
* ���ʵı���
* �޸ĵı���
* ���������
  OPR_DATA *pOprData
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
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
* �������ƣ�VOID Oam_InptGetBuf
* ���������������ڴ����
* ���ʵı���
* �޸ĵı���
* ���������bufSize:�ڴ��С
            ucLinkID:��·��
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
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
* �������ƣ�VOID Oam_InptFreeBuf
* ���������������ڴ�ָ�븴λ
* ���ʵı���
* �޸ĵı���
* ���������ucLinkID:���ӱ�ʶ
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
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
* �������ƣ�VOID Oam_SetDATUpdateWaitTimer
* �����������������Զ�ʱ������Ϣ
* ���ʵı���
* �޸ĵı���
* ���������
  TYPE_LINK_STATE *pLinkState
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
void Oam_SetDATUpdateWaitTimer(void)
{
    Oam_KillDATUpdateWaitTimer();
    g_ptOamIntVar->wDatUpdateWaitTimer = XOS_SetRelativeTimer(PROCTYPE_OAM_CLIMANAGER, TIMER_DAT_UPDATE_WAIT, DURATION_DAT_UPDATE_WAIT, 1);
    return;
}

/**************************************************************************
* �������ƣ�VOID Oam_SetDATUpdateWaitTimerInvalid
* ��������������dat update ��ʱ��ʱ����Ч
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2011/02/11    V1.0    ������      ����
**************************************************************************/
void Oam_SetDATUpdateWaitTimerInvalid(void)
{
    g_ptOamIntVar->wDatUpdateWaitTimer = INVALID_TIMER_ID;
    return;
}

/**************************************************************************
* �������ƣ�VOID Oam_KillDATUpdateWaitTimer
* ����������ɱ��dat update ��ʱ��ʱ��
* ���ʵı���
* �޸ĵı���
* ���������
  TYPE_LINK_STATE *pLinkState
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
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
* �������ƣ�VOID Oam_SetGetCfgVerInfoTimerInvalid
* ��������������get cfg verinfo ��ʱ��ʱ����Ч
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2011/02/11    V1.0    ������      ����
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
* �������ƣ�VOID Oam_SetRegOamsNotifyTimerInvalid
* ��������������reg oams notify ��ʱ��ʱ����Ч
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2011/02/11    V1.0    ������      ����
**************************************************************************/
void Oam_SetRegOamsNotifyTimerInvalid(void)
{
    g_ptOamIntVar->wRegOamsNotifyTimer = INVALID_TIMER_ID;
    return;
}

/**************************************************************************
* �������ƣ�BOOL FindUserFromLink
* ������������ȡ�û�
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
* �������ƣ�BOOL AddUserToLink
* ���������������û�
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
BOOL AddUserToLink(USER_NODE *pNode)
{
    USER_NODE *ppNodeTmp = NULL;
    USER_NODE *pNodeTmp = NULL;

    if (!pNode)
        return FALSE;

    if (FindUserFromLink(pNode->tUserInfo.sUsername,&ppNodeTmp,&pNodeTmp))
    {
        /* �޸��û� */
        strncpy(pNodeTmp->tUserInfo.sPassword, pNode->tUserInfo.sPassword, sizeof(pNodeTmp->tUserInfo.sPassword));
        if (pNode->tUserInfo.ucRight != 0)
            pNodeTmp->tUserInfo.ucRight = pNode->tUserInfo.ucRight;

        /* �����������Ȩ��, ����ԭ����Ȩ�� */
        return TRUE;
    }
    else
    {
        /* �����û� */
        /* �����������Ȩ��, ��Ĭ��Ϊ1 */
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
* �������ƣ�BOOL DelUserFromLink
* ����������ɾ���û�
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
BOOL DelUserFromLink(CHAR *pUserName)
{
    USER_NODE *ppNode = NULL;
    USER_NODE *pNode = NULL;
    BYTE i;

    if (!pUserName)
        return FALSE;

    /*�û�������*/
    if (!FindUserFromLink(pUserName,&ppNode,&pNode))
        return FALSE;

    /*Ĭ���û�����ɾ��*/
    if (strcmp(pUserName, DEFAULT_USERNAME) == 0)
        return FALSE;

    /*�����û�����ɾ��*/
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
* �������ƣ�VOID GetTerminalCfgData
* ������������ȡ��ǰ�û��ն���Ϣ
* ���ʵı���
* �޸ĵı���
* ���������MSG_COMM_OAM **pRtnMsg
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
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
* �������ƣ�VOID GetUserNameData
* ������������ȡ�û��б�
* ���ʵı���
* �޸ĵı���
* ���������MSG_COMM_OAM **pRtnMsg
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
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
* ���Ժ�����ʼ����
**************************************************************************/
/**************************************************************************
* �������ƣ�VOID OAM_DbgShowCfgAgtPriData
* ������������ȡ�û��б�
* ���ʵı���
* �޸ĵı���
* ���������MSG_COMM_OAM **pRtnMsg
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
void OAM_DbgShowCfgAgtPriData(void)
{


    return;
}

/*****************************************************************************/
/**
@brief  CLIMANAGER���Լ�����������ϵͳ����Ϣ�����ڶ�λ��������ϵͳ����������
@verbatim
dwMsgId: WORD16 ���ͣ�Ҫ���͵���Ϣ�ڵ��Ժ����еı��
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
\n1��ֻ�ܷ��Ͱ�����Ϣ�д�ӡ��������Щ��Ϣ
@li @b ���ʾ����
\n��

@li @b �޸ļ�¼��
-#  �� 
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
@brief ��ʾCLI���յ���VMM������ȫ��DAT�汾��Ϣ�����ڶ�λCLI�������ȫ�����
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
\nRecord 0: function type = 1, filename = /IDE0/setup/omp.pkg/210c0000//oam-NLS.dat, Using language type = chinese
Record 1: function type = 2, filename = /IDE0/setup/omp.pkg/210c0000//phy-NLS.dat, Using language type = chinese
Record 2: function type = 4, filename = /IDE0/setup/omp.pkg/210c0000//sig-NLS.dat, Using language type = chinese
Record 3: function type = 5, filename = /IDE0/setup/omp.pkg/210c0000//brs-NLS.dat, Using language type = chinese
@li @b �޸ļ�¼��
-#  �� 
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
* �������ƣ�Oam_CfgIfCurVersionIsPC
* �����������жϵ�ǰ���а汾��pc���汾���ǵ�Ƭ�汾
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ��    TRUE  -- ��PC���汾
                                 FALSE -- ����pc���汾
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/06/08   V1.0     ���     ����
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
* �������ƣ�Oam_CfgCheckRecvMsgMinLen
* �����������жϵ�ǰ�յ�����Ϣ�����Ƿ�Ϸ�
* ���ʵı���
* �޸ĵı���
* ���������wMinLen -- ��������Ϣ��С�Ϸ�����
* �����������
* �� �� ֵ��    TRUE  -- ���ȺϷ�
                                 FALSE -- ���Ȳ��Ϸ�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/06/08   V1.0     ���     ����
**************************************************************************/
BOOLEAN Oam_CfgCheckRecvMsgMinLen(WORD16 wMinLen)
{
    WORD16 wRecvLen = 0;
    BOOLEAN bRet = FALSE;

    if (XOS_GetMsgDataLen(PROCTYPE_OAM_CLIMANAGER, &wRecvLen) != XOS_SUCCESS)
    {
        /*�����ȡ�յ�����Ϣ����ʧ�ܣ�����false*/
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
* �������ƣ�CheckBlfOnline
* �����������ж��û��Ƿ����ߣ����߷���true�������߷���false
* ���ʵı���
* �޸ĵı���
* ���������sUserName --�û���
* �����������
* �� �� ֵ��    TRUE  -- ����
                                 FALSE --������
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/06/08   V1.0     ���     ����
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
        
        /* �����û� */
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

/* ��һ������־д���ļ� */
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

    /* �����ж���־�ļ��Ƿ���� */
    bIsFileExists = OamUserFileExists(acUserFile);
    /* ����ļ������ڣ��򴴽����ļ� */
    if (!bIsFileExists)
    {
        ulRet = XOS_OpenFile(acUserFile, XOS_CREAT, &fd);
        if (ulRet != XOS_SUCCESS)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, "[InsertOneUseLog] XOS_OpenFile exec failed! ulRet = %d, line = %d\n", ulRet, __LINE__);
            return FALSE;
        }

        /* д�ļ�ͷ */
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

    /* ��ȡ�ļ����� */
    ulRet = XOS_GetFileLength(acUserFile, &sdwFileLen);
    if (ulRet != XOS_SUCCESS)
    {
        return FALSE;
    }

    if ((sdwFileLen + sizeof(TYPE_USER_LOG)) > MAX_USER_LOG_FILE_SIZE)
        bFileTooBig = TRUE;

    /* ��ԭ��־�ļ� */
    ulRet = XOS_OpenFile(acUserFile, XOS_RDONLY, &fd);
    if (ulRet != XOS_SUCCESS)
    {
        return FALSE;
    }
    /* ��ȡ�ļ�ͷ */
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

    /* �½���ʱ�ļ� */
    ulRet = XOS_OpenFile(acUserFileTmp, XOS_CREAT, &fdTemp);
    if (ulRet != XOS_SUCCESS)
    {
        XOS_CloseFile(fd);
        return FALSE;
    }

    /* д��ʱ�ļ��ļ�ͷ */
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

    /* ����ļ������������������Ҫ��������·�еļ�¼���кż�1����Ϊ�����ѭ���лὫ����ԭ��¼���кż�1 */
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
        /* ��ԭ�ļ��ж�ȡÿ����¼ */
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

        /* ����ļ���С�������ֵ����Ҫ�������м�¼���к� */
        if (bFileTooBig)
            tUserLog.wNo--;

        /* д����ʱ�ļ� */
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

        /* ��������ļ����������ʵ���Ǵӵڶ�����¼��ʼ��ȡ�ģ�����Ӧ���ټ���һ��i */
        if (bFileTooBig)
        {
            if (i >= (tFileHead.dwLogNum - 2))
                break;
        }

    }

    /* ��ʼ����ǰ��¼д���ļ�ĩβ */
    if (strlen(pLinkState->tUserLog.sUserName) == 0)
    {
        XOS_CloseFile(fdTemp);
        XOS_CloseFile(fd);
        /* ɾ����ʱ�ļ� */
        XOS_DeleteFile(acUserFileTmp);
        return FALSE;
    }
    
    if (XOS_SeekFile(fdTemp, XOS_SEEK_SET, dwOffset) != XOS_SUCCESS)
    {
        XOS_CloseFile(fdTemp);
        XOS_CloseFile(fd);
        /* ɾ����ʱ�ļ� */
        XOS_DeleteFile(acUserFileTmp);
        return FALSE;
    }

    ulRet = XOS_WriteFile(fdTemp, (CHAR *)&pLinkState->tUserLog,sizeof(TYPE_USER_LOG), &sdwByteCount);
    if (ulRet != XOS_SUCCESS)
    {
        XOS_CloseFile(fdTemp);
        XOS_CloseFile(fd);
        /* ɾ����ʱ�ļ� */
        XOS_DeleteFile(acUserFileTmp);
        return FALSE;
    }
    if (sdwByteCount != sizeof(TYPE_USER_LOG))
    {
        XOS_CloseFile(fdTemp);
        XOS_CloseFile(fd);
        /* ɾ����ʱ�ļ� */
        XOS_DeleteFile(acUserFileTmp);
        return FALSE;
    }
        
    XOS_CloseFile(fdTemp);
    XOS_CloseFile(fd);

    /* ɾ��ԭ�ļ�������ʱ�ļ�����Ϊ��ʽ�ļ� */
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

    /* ��ԭ�ļ� */
    dwRet = XOS_OpenFile(acUserFile, XOS_RDONLY, &fd);
    if (dwRet != XOS_SUCCESS)
    {
        return FALSE;
    }
    /* ��ȡ�ļ�ͷ */
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

    /* �½���ʱ�ļ� */
    dwRet = XOS_OpenFile(acUserFileTmp, XOS_CREAT, &fdTemp);
    if (dwRet != XOS_SUCCESS)
    {
        XOS_CloseFile(fd);
        return FALSE;
    }

    /* д��ʱ�ļ��ļ�ͷ */
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

    /* ��ԭ�ļ��ж�ȡ���ݣ�д����ʱ�ļ� */
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
        /* ��ԭ�ļ��ж�ȡÿ����¼ */
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

        /* д����ʱ�ļ� */
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

    /* ��ȡ�԰�JID */
    dwRtn = XOS_GetSelfJID(&tMateJID);
    if(XOS_SUCCESS != dwRtn)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[OamMateRequestSubScr]XOS_GetSelfJID fail! dwRtn = %d\n", dwRtn);
        return;
    }

    /* �ж���廹���Ұ� */
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

    /* ���Ķ԰��ϵ���Ϣ
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

    /* ����Manager��Jno */
    dwJno = XOS_ConstructJNO(PROCTYPE_OAM_CLIMANAGER, 1);
    if(dwJno == ptMsg->tSubscrptJID.dwJno)
    {
        /* JOB�˳� */
        if(TIPC_WITHDRAWN == ptMsg->event)
        {
        }
        /* JOB���� */
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
@brief  ��ӡ�Ѿ��趨�������û���������IP��ַ
@verbatim
\n��
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
\n
@li @b ���ʾ����
\nThe specified IP address: 129.0.0.1
@li @b �޸ļ�¼��
-#  �� 
*/
/*****************************************************************************/
void OAM_DbgShowCliSpecIP(void)
{
    printf("The specified IP address: %s\n", g_szSpecifyIP);
}

/* ���Ժ���:�趨ָ��IP */
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
@brief ��ʾCLIMANAGER��������־�м�¼������ִ����־
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
\n��
@li @b ���ʾ����
\n2010-12-22 15:57:47, SSH, root, 192.168.1.200, install_remove sev.pkg 285278208, FAILED
2010-12-22 15:58:0, SSH, root, 192.168.1.200, install_deactive_dft sev.pkg 285278208, SUCCESS
2010-12-22 15:58:15, SSH, root, 192.168.1.200, install_remove /IDE0/oam/cli/CliCmdLog.txtsev.pkg 285278208, SUCCESS
2010-12-22 15:59:8, SSH, root, 192.168.1.200, install_deactive_dft SBCV4.00.10_PC_D_0x0B0213.pkg 721427, FAILED
2010-12-22 15:59:20, SSH, root, 192.168.1.200, install_remove SBCV4.00.10_PC_D_0x0B0213.pkg 721427, SUCCESS

@li @b �޸ļ�¼��
-#  �� 
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

    /* ��ԭ�ļ� */
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
    /*����ж�*/
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
    /*����ж�*/
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
    /*����ж�*/
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
* �������ƣ�VOID OAM_CliChangeAllLinkToStartMode
* �������������������ӵ�����ģʽ���л�����ʼ���û�ģʽ
* ���ʵı���
* �޸ĵı���
* ���������pcReason:�л�ԭ��
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/02/12    V1.0    ���      ����
**************************************************************************/
void OAM_CliChangeAllLinkToStartMode(CHAR *pcReason)
{    
    WORD16 i = 0;
    WORD16 wLinkNum = sizeof(gtLinkState)/sizeof(TYPE_LINK_STATE);
    BYTE bCurModeId = 0;
    TYPE_LINK_STATE * pLinkState = NULL;
    TYPE_XMLLINKSTATE *pXmlLinkState = NULL;

    /*����XML��·״̬*/
    for (i = 0;i < MAX_SIZE_LINKSTATE;i++)
    {
        pXmlLinkState = Xml_GetLinkStateByIndex(i);
        if (NULL != pXmlLinkState)
        {
            if (XML_LINKSTATE_USED == pXmlLinkState->bUsed)
            {
                /*��ȡ��ǰģʽID*/
                bCurModeId = XML_GetCurModeId(pXmlLinkState);
                while(bCurModeId > CMD_MODE_INVALID)
                {
                    /*������Ȩģʽ���˳�*/
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
                /*611002035007 �����ԸĽ����飨3����cli>����Ҫ����en���ܽ���cli#*/
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
* �������ƣ� Oam_CfgIfNoLinkIsExecutingCmd
* ��������������Ƿ�û��������ִ������
* ���ʵı���
* �޸ĵı���
* ���������
* ���������
* �� �� ֵ��TRUE-û��������ִ������ 
                           FALSE - ��������ִ������
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/02/12    V1.0    ���      ����
**************************************************************************/
BOOLEAN Oam_CfgIfNoLinkIsExecutingCmd(void)
{
    WORD16 i = 0;    
    WORD16 wTelLinkNum = sizeof(gtLinkState)/sizeof(TYPE_LINK_STATE);
    WORD16 wXmlLinkNum = sizeof(gt_XmlLinkState)/sizeof(TYPE_XMLLINKSTATE);
    
    /*����Ŀǰ�ǵ��û�����������ping�ȳ�ʱ��ִ�е�����
    ����datһֱ�����л������������xmlҪ����*/
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
    /*����ж�*/
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
    /*����ж�*/
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
    /*����ж�*/
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
    /*����ж�*/
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
    /*����ж�*/
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
    /*����ж�*/
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
    /*����ж�*/
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
    /*����ж�*/
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
���û��ļ�������cli�ű���������
*/
BOOLEAN OAM_CliSetLanguageType(CHAR *pStrLangType)
{
    /*����ж�*/
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
��ȡcli�ű���������
*/
BOOLEAN OAM_CliGetLanguageType(T_Cli_LangType *ptLangType)
{
    /*����ж�*/
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
* �������ƣ�CHAR *OAM_RTrim(CHAR *pStr)
* ����������ȥ���ַ����ұߵĿո�tab�س������ַ�
* ���ʵı���
* �޸ĵı���
* ���������CHAR *pStr
* ���������
* �� �� ֵ��null- ����
            ����-ȥ������´�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2008/03/17   V1.0    ���      ����
**************************************************************************/
CHAR *OAM_RTrim(CHAR *pStr)
{
    CHAR *pRet = pStr;
    WORD16 wLen = 0;
    SWORD16 i = 0;
    if (NULL == pStr)/*����Ŀմ���ֱ�ӷ���*/
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
* �������ƣ�CHAR *OAM_LTrim(CHAR *pStr )
* ����������ȥ���ַ�����ߵĿո�tab�س������ַ�
* ���ʵı���
* �޸ĵı���
* ���������CHAR *pStr
* ���������
* �� �� ֵ��null- ����
            ����-ȥ������´�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2008/03/17   V1.0    ���      ����
**************************************************************************/
CHAR *OAM_LTrim(CHAR *pStr )
{
    CHAR *pRet = pStr;
    WORD16 wLen = 0;
    SWORD16 i = 0;
    if (NULL == pStr)/*����Ŀմ���ֱ�ӷ���*/
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


