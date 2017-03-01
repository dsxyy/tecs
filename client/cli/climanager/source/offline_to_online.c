/**************************************************************************
* ��Ȩ���� (C)2002, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ�offline_to_online.c
* �ļ���ʶ��
* ����ժҪ������brs����ת���ߵ�������������(V05.03.80.2_3GPF_TDRNC_003)
* ����˵����
            
* ��ǰ�汾��
* ��    ��       �����
* ������ڣ�20090116
*
* �޸ļ�¼1 ��// �޸���ʷ��¼�������޸����ڡ��޸��߼��޸�����
*    �޸����ڣ�
*    �� �� ��    ��
*    �� �� ��    ��
*    �޸����� :
* �޸ļ�¼2��
**************************************************************************/
/**************************************************************************
*                           ͷ�ļ�                                        *
**************************************************************************/
#include "includes.h"
#include "offline_to_online.h"
#include "saveprtclcfg.h"
#include "hrtelmsg.h"
#include "oam_execute.h"

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
/*��¼�Ƿ���һ��ת�������ĵ�һ������*/
static BOOLEAN g_bIsFirstCmd = TRUE;
static JID g_tProtocolPid;
static BYTE g_aucLastData[MAX_STORE_LEN]; 
TYPE_LINK_STATE g_tLinkStateForOfflineToOnline;

/**************************************************************************
*                          ���ر���                                       *
**************************************************************************/

/**************************************************************************
*                          ȫ�ֺ���ԭ��                                   *
**************************************************************************/
extern BOOLEAN Oam_CfgGlobalPointersIsOk(void);

/**************************************************************************
*                          �ֲ�����ԭ��                                   *
**************************************************************************/
static BOOLEAN FirstCharIsSpecialCmd(CHAR *strCmdLine);
static BOOLEAN GetCmdLine(CHAR *pcCmdLineOut, WORD16 wMaxCmdLen, LPVOID pMsgPara);
static void ExecuteSpecialCmd(TYPE_LINK_STATE *pLinkState, CHAR *strCmdLine);
static void ExecuteNormalCmd(TYPE_LINK_STATE *pLinkState, CHAR *strCmdLine);
static void ReturnParseErrToBrs(TYPE_LINK_STATE *pLinkState);

/**************************************************************************
*                         ��������                                               *
**************************************************************************/

/**************************************************************************
* �������ƣ�Oam_CfgHandleMSGOffline2Online
* ��������������brs����������ת��������
* ���ʵı���
* �޸ĵı���
* ���������TYPE_LINK_STATE *pLinkState --ȫ�ֱ���
                              LPVOID pMsgPara -- brs����������
* ���������
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/02/01   V1.0    ���      ����
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

    /*��ȡר��linkstate*/
    pLinkState = OamCfgGetLinkState(MSG_OFFLINE_CFG_TO_INTERPRET, pMsgPara);
    if (!pLinkState)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                   "Oam_cfgHandleMSGOffline2Online: Warnning GetLinkState Fail!\n");
        return;
    }

    /*CRDCM00238058 ����Ϣ���Ƚ��м�飬��ֹ���ڲ����ݰ汾��������*/
    XOS_GetMsgDataLen(PROCTYPE_OAM_CLIMANAGER, &wMsgLenTmp);
    if (wMsgLenTmp != sizeof(T_OfflineCfgCmd))
    {
        CHAR *pcMsg = "Oam_cfgHandleMSGOffline2Online: MsgLen error, please make sure version of RPU and OAMMANAGER is compatible!\n";
        XOS_SysLog(OAM_CFG_LOG_ERROR, "%s\n", pcMsg);
        ReturnParseErrToBrs(pLinkState);
        return;        	
    }
    
    /*��������ת���ߵ�brspid*/
    XOS_Sender(&g_tProtocolPid);
    
    /*������Ϣ�е�lastdata,����ִ���귢��MSG_OFFLINE_CFG_TO_INTERPRETʱ�����brs*/
    memset(g_aucLastData, 0, sizeof(g_aucLastData));
    memcpy(g_aucLastData, ptMsgData->LastData, sizeof(g_aucLastData));

    /*���dat�ļ��Ƿ����*/
    if (!Oam_CfgGlobalPointersIsOk())
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                   "Oam_cfgHandleMSGOffline2Online: Some global pointer is NULL! can`t execute message [%d]\n"
                   "check global pointers listed in function GlobalPtrIsOk()", MSG_OFFLINE_CFG_TO_INTERPRET);
        ReturnParseErrToBrs(pLinkState);

        return;
    }

    /*�����һ�������еĵ�һ�����л���Э������ģʽ*/
    if (g_bIsFirstCmd)
    {
        Oam_LinkInitializeForNewConn(pLinkState);

        pLinkState->ucLinkID =  MAX_CLI_VTY_NUMBER + 1;
        
        /*�л�ģʽ��Э��ջ����ģʽ*/
        Oam_LinkModeStack_Init(pLinkState);
        Oam_LinkModeStack_Push(pLinkState, CMD_MODE_PROTOCOL);

        g_bIsFirstCmd = FALSE;

        g_ptOamIntVar->bSupportStringWithBlank = TRUE;
    }

    /*��ȡ�����ַ���*/
    if (!GetCmdLine(aucCmdLineBuf, sizeof(aucCmdLineBuf), pMsgPara))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                   "Oam_cfgHandleMSGOffline2Online: Warnning GetCmdLine Fail!\n");
        /*??�����ȡ����brs�����������ַ�����֪ͨbrsִ����һ��*/
        Oam_CfgGetNextCmdFromBrs();
        return;
    }
    
    XOS_SysLog(OAM_CFG_LOG_NOTICE,
               "Oam_cfgHandleMSGOffline2Online: Execute cmd [%s]!\n", aucCmdLineBuf);

    pcCmdlineTmp = aucCmdLineBuf;
    
    /*ִ������*/
    /*
    �����ַ���������:
    1. ��ͷ��һ���������������������ϣ�����"!\ninterface 0 1 2 1 1"
    2. ������һ����ͨ������ "interface 0 1 5 1 1 "
    */
    if (FirstCharIsSpecialCmd(aucCmdLineBuf))
    {/*����ĸ����������*/
        ExecuteSpecialCmd(pLinkState, aucCmdLineBuf);

        /*����ǰ���һ������������ַ������������ⲿ��ȡ��һ��*/
        while(*pcCmdlineTmp == '\n'  || *pcCmdlineTmp == '\r' || FirstCharIsSpecialCmd(pcCmdlineTmp))
        {
            pcCmdlineTmp++;
            if (*pcCmdlineTmp == 0)
            {
                break;
            }
        }
    }

    /*����������ִ���Ժ󣬿��Ƿ�����ͨ����*/
    if (strlen(pcCmdlineTmp) > 0)
    {/*��ͨ����,Ҫ��brs����ִ�н����������һ������*/
        ExecuteNormalCmd(pLinkState, pcCmdlineTmp);
    }
    else
    {
        /* ??���ֻ��һ���������� ֪ͨbrsִ����һ��*/
        Oam_CfgGetNextCmdFromBrs();
    }
}

/**************************************************************************
* �������ƣ�Oam_CfgHandleMSGOffline2OnlineFinish
* ��������������brs����������ת���ߴ��������Ϣ
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��null- ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/02/01   V1.0    ���      ����
**************************************************************************/
void Oam_CfgHandleMSGOffline2OnlineFinish(void)
{
    g_bIsFirstCmd = TRUE;
}

/**************************************************************************
* �������ƣ�FirstCharIsSpecialCmd
* �����������ж����������һ���ַ��Ƿ�����������
* ���ʵı���
* �޸ĵı���
* ���������CHAR *strCmdLine -- �������ַ���
* ���������
* �� �� ֵ��   TRUE- ��
                              FALSE- ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/02/01   V1.0    ���      ����
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
* �������ƣ�GetCmdLine
* ������������brs��Ϣ�л�ȡ�������ַ�������trim����
* ���ʵı���
* �޸ĵı���
* ���������WORD16 wMaxCmdLen --����ַ�����󳤶�
                              LPVOID pMsgPara -- BRS��������Ϣ��
* ���������CHAR *pcCmdLineOut -- �����ַ���
* �� �� ֵ��   TRUE- ��ȡ�ɹ�
                              FALSE- ��ȡʧ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/02/01   V1.0    ���      ����
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
* �������ƣ�ExecuteSpecialCmd
* ����������ִ�в���Ҫ���͵���������
* ���ʵı���
* �޸ĵı���
* ���������TYPE_LINK_STATE *pLinkState -- ȫ�ֱ���
                              CHAR *strCmdLine -- �������ַ���
* �����������
* �� �� ֵ��   ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/02/01   V1.0    ���      ����
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
        /*�����ļ�����!���ص�Э������ģʽ*/
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
 /*�ӻָ�����û��$ ���ŵĴ���*/
       if((pLinkState->bModeStackTop > 1)&&((pLinkState->bModeID[pLinkState->bModeStackTop - 1]==30)||(pLinkState->bModeID[pLinkState->bModeStackTop - 1]==31)))
        {
            if((pLinkState->bModeStackTop>2)&&((pLinkState->bModeID[pLinkState->bModeStackTop - 2])==CMD_MODE_BGP))
            {
                /*���ݻָ���д�ģ���֪�������*/
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
* �������ƣ�ExecuteNormalCmd
* ����������ִ����Ҫ���͵���ͨ����
* ���ʵı���
* �޸ĵı���
* ���������TYPE_LINK_STATE *pLinkState -- ȫ�ֱ���
                              CHAR *strCmdLine -- �������ַ���
* �����������
* �� �� ֵ��   ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/02/01   V1.0    ���      ����
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

    /*?? �������������ʧ�ܣ���ӡһ�£������澯*/
    if (pMatchResult->wParseState != PARSESTATE_FINISH_PARSE)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                   "ExecuteNormalCmd: %s \n[Parse error, check whether this command match with dat file!]\n",
                strCmdLine);
        /*?? ��Ҫ��brs��һ������ʧ��֪ͨ*/
        ReturnParseErrToBrs(pLinkState);
        return;
    }

}

/**************************************************************************
* �������ƣ�Oam_CfgGetNextCmdFromBrs()
* ����������֪ͨbrsִ����һ������
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ��   ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/02/01   V1.0    ���      ����
**************************************************************************/
void Oam_CfgGetNextCmdFromBrs(void)
{
    MSG_COMM_OAM tRetMsg;

    /*������ṹ����lastdata��brs*/
    memset(&tRetMsg, 0, sizeof(tRetMsg));
    memcpy(tRetMsg.LastData, g_aucLastData, sizeof(tRetMsg.LastData));
    
//    OSS_SendAsynMsg(MSG_OFFLINE_CFG_TO_INTERPRET, (BYTE *)&tRetMsg, sizeof(tRetMsg), COMM_RELIABLE,&g_tProtocolPid);
}

/**************************************************************************
* �������ƣ�Oam_CfgCurLinkIsOffline2Online
* ������������ǰ�����Ƿ��Ǵ�������ת����
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ��   TRUE - ��
                              FALSE - ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/02/01   V1.0    ���      ����
**************************************************************************/
BOOLEAN Oam_CfgCurLinkIsOffline2Online(TYPE_LINK_STATE *pLinkState)
{
    return (pLinkState == &g_tLinkStateForOfflineToOnline);
}

/**************************************************************************
* �������ƣ�Oam_CfgGetBrsPidForOffline2Online
* ������������ȡ����ת���ߵ�brs pid
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ��   TRUE - ��
                              FALSE - ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/02/01   V1.0    ���      ����
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
* �������ƣ�Oam_CfgSetLastDataForOffline2Online
* ������������������ת������Ϣ��lastdata
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ��   TRUE - ��
                              FALSE - ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/02/01   V1.0    ���      ����
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
* �������ƣ�Oam_CfgCurCmdIsOffline2Online
* ������������ǰ�����Ƿ��Ǵ�������ת����
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ��   TRUE - ��
                              FALSE - ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/02/01   V1.0    ���      ����
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
* �������ƣ�ReturnParseErrToBrs
* �����������ؽ���ʧ��֪ͨ��brs
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ��   TRUE - ��
                              FALSE - ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/02/01   V1.0    ���      ����
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
    
    /* �������ʧ�ܵ���Ϣ*/
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

    /*����Execute�˶�ʱ��*/
    Oam_KillExeTimer(pLinkState);
    Oam_SetExeTimer(pLinkState);

    Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_EXECUTE);

    /*�������ת���ߵ�ר��lastdata*/
    Oam_CfgSetLastDataForOffline2Online(ptSendMsg);
    
    Oam_CfgGetBrsPidForOffline2Online(&tJidBrs);
    
    SendPacketToExecute(pLinkState, ptSendMsg, tJidBrs);
}


