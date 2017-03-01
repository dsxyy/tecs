/**************************************************************************
* ��Ȩ���� (C)2008, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ�climanager.c
* �ļ���ʶ��
* ����ժҪ��
* ����˵����
            
* ��ǰ�汾��
* ��    �ߣ�
* ������ڣ�
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
#include "oam_execute.h"
#include "clirecvreg.h"
#include "pub_oam_cfg_event.h"
#include "xmlagent.h"
#include "offline_to_online.h"
#include "oam_cfg_shmlog.h"
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
/*����״̬���������л��汾ʱ����*/
static BYTE g_ucLinkStateCheckTimes = 0;
#define OAM_CFG_MAX_LINKSTATE_CHECKTIMES    12

/*��ǰ����dat�汾�������ͣ���pub_vmm.h�ж���һ��*/
/*
vmm�����cli��dat����������������50��,��ʱ��������30����ddm20��
��vmmע�ἤ��֪ͨ������汾ʱ��ʹ��
*/
#define DATFILE_FUNCTYPE_CFG_BEGIN     FUNC_SCRIPT_CLIS_OAM
#define DATFILE_FUNCTYPE_CFG_NUM       50
#define DATFILE_FUNCTYPE_DDM_BEGIN     FUNC_SCRIPT_DDM_OAM
#define DATFILE_FUNCTYPE_DDM_NUM       50
#define DATFILE_FUNCTYPE_SPECIAL_NUM   50
#define DATFILE_FUNCTYPE_TOTAL         150

/**************************************************************************
*                          ���ر���                                       *
**************************************************************************/
/**************************************************************************
*                          �ֲ�����ԭ��                                   *
**************************************************************************/
static BOOL Initialize(T_OAM_INTERPRET_VAR *ptVar, BOOL isMaster);
static void SetSaveTypeAsTxt(const JID jid);
static BYTE OamCliMgtInitShm(void);

static void RecvDatVerInfoAck(LPVOID pMsgPara);
static void RecvDatVerInfoGetTimeOut(void);

static void RecvRegOamsNtfAck(LPVOID pMsgPara);
static void RecvRegOamsNtfTimeOut(void);

static void RecvTxtCfgReq(void);

static void ExecDatUpdateWaitTimerProc(void);
static void HandleMsgEV_OAM_CFG_CFG(WORD32 dwMsgId, LPVOID pMsgPara, BOOLEAN bIsSameEndian);
static void HandleMsgEV_OAM_CFG_PLAN(WORD32 dwMsgId, LPVOID pMsgPara, BOOLEAN bIsSameEndian);
static BOOLEAN Oam_IfExecutingDatUpdateCmd(void);
static void Oam_CfgStopAllXmlLinkExcutingCmd(void);
static void Oam_CfgStopAllTelLinkExcutingCmd(void);
static void EndRestoreByReasonOf(WORD32 dwReason);
/*static BOOLEAN OtherLinkIsSaving(TYPE_LINK_STATE *pLinkState);*/

static void Oam_CfgPowerOnOverAndToWorkState(void);

static void HandleMasterToSlave(void);
static void HandleSlaveToMaster(void);

//static BOOLEAN SpecialRegistered(VOID);

static void Oam_Cli_ChangeScriptLanguage(LPVOID pMsgPara);
/**************************************************************************
*                          ȫ�ֺ�������                                   *
**************************************************************************/

extern void RegDatToVmm(void);
extern void ReqAllCfgVerInfo(void);
extern void Oam_CfgStopAllLinkExcutingCmd(void);

/*extern VOID OamCfgRemoveAllDat(VOID);*/
/**************************************************************************
*                          ����ں���                                     *
**************************************************************************/
void Oam_Interpret(WORD16 wState, WORD32 dwMsgId, LPVOID pMsgPara, LPVOID pVar, BOOLEAN bIsSameEndian/*~*/)
{
    TYPE_LINK_STATE *pLinkState = NULL;
    T_OAM_INTERPRET_VAR *ptVar  = (T_OAM_INTERPRET_VAR*)pVar ;

    OamInterpretCheckPtr((BYTE*)pMsgPara, __LINE__);
    OamInterpretCheckPtr((BYTE*)ptVar, __LINE__);

    XOS_SysLog(OAM_CFG_LOG_DEBUG, "=======s/m = %d oam_interpret wstate=%d msgid=%d==========\n", XOS_GetBoardMSState(),wState,dwMsgId);
 //   Oam_CfgWriteShmLog(wState, dwMsgId);

    switch (wState)
    {
    case S_Init:
        switch(dwMsgId)
        {
        case EV_STARTUP:
            break;
            /*�ϵ���Ϣ*/
        case EV_MASTER_POWER_ON:
        case EV_SLAVE_POWER_ON:
            {
            /* ��ʼ�� */
                BOOL isMaster = TRUE;
                BOOL bInitSuccess = TRUE;
                if ( EV_SLAVE_POWER_ON == dwMsgId )
                {
                    isMaster = FALSE;
                }
                XOS_SysLog(OAM_CFG_LOG_NOTICE, "Receive power on msg, begin to initialize climanager...");
                bInitSuccess = Initialize(ptVar, isMaster);
                if (!bInitSuccess)
                {
                    XOS_SysLog(OAM_CFG_LOG_ERROR, "climanager Initialize has errors!");
                }
                XOS_SysLog(OAM_CFG_LOG_DEBUG, "climanager initialize finished");

                if (Oam_CfgIfCurVersionIsPC())
                {
                    /*�����pc���汾*/
                    /* �ϵ���ϣ���Ӧ�� */
                    if (bInitSuccess)
                    {
                        Oam_CfgPowerOnOverAndToWorkState();
                    }
                }
            }

            break;

            /*SCS���ص��ļ���Ϣ��������Ϣ����Initialize���淢�͵�*/
        case EV_OAMS_VERINFO_ACK:
            RecvDatVerInfoAck(pMsgPara);
            break;

            /*��ȡdat�汾��Ϣ��ʱ��Ϣ*/
        case EV_TIMER_GET_CFGVERINFO:
            RecvDatVerInfoGetTimeOut();
            break;

            /*ע��dat�л���Ӧ*/
        case EV_VER_OAMS_NOTIFYREG_ACK:
            RecvRegOamsNtfAck(pMsgPara);
            break;

            /*ע��dat�л���Ӧ��ʱ��Ϣ*/
        case EV_TIMER_REGOAMS_NTF_ACK:
            RecvRegOamsNtfTimeOut();
            break;

            /*�û���Ϣ����ͬ����Ϣ*/
        case OAM_MSG_MS_CFGFILE_SYN:
            /*agent�û���Ϣcfg�ļ�����ͬ��*/
            if (BOARD_SLAVE ==  XOS_GetBoardMSState())
            {/*�������*/
                Oam_RcvCfgFileFromMaster(pMsgPara);
                ReadCfgScript();
            }
            else
            {/*���巢��*/
                Oam_SendCfgFileToSlave();
            }
            break;
        case OAM_MSG_TELNET_TO_INTERPRET:/*telnetserver ��Ϣ*/
            if (!Oam_CfgCheckRecvMsgMinLen(sizeof(TYPE_TELNET_MSGHEAD)))
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR, "OAM_MSG_TELNET_TO_INTERPRET:The length of message received is less than the length of interface struct! ");
                return;
            }       

            pLinkState = OamCfgGetLinkState(dwMsgId,pMsgPara);

            if (!pLinkState)
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR,
                           "failled to get linkstate while processing telnetserver msg, vtyno in msg is %d",
                           ((TYPE_TELNET_MSGHEAD *)pMsgPara)->bVtyNum);
                break;
            }

            /* ��������Telnet����Ϣ */
            RecvFromTelnet(pLinkState,pMsgPara);
            break;
        /* �յ����ķ�����Ϣ */
        case EV_SUBSCR_EVENT_TOAPP:
            OamRecvMateSubScr(pMsgPara);
            break;
        default:
            break;
        }
        break;

    case S_Work:
        switch(dwMsgId)
        {
            /*dat�汾�л���Ϣ*/
        case EV_OAMS_VER_UPDATE_NOTIFY:
            if (BOARD_MASTER ==  XOS_GetBoardMSState())
            {
                XOS_SysLog(OAM_CFG_LOG_NOTICE, "reveive ver update notify msg, change to UpdateDAT state for the updating of dat ver ...");
               
                /*����ת���߲�����λ���Ȱ汾�л����Ժ��ٴ�������еĻ�*/
                Oam_CfgHandleMSGOffline2OnlineFinish();
                
                /*���ö�ʱ�����ȵȴ����������������µ�dat�ļ�*/
                Oam_SetDATUpdateWaitTimer();

                g_ptOamIntVar->ucCfgVerInfoGetTimes = 0;
                g_ucLinkStateCheckTimes = 0;
                XOS_SetNextState(PROCTYPE_OAM_CLIMANAGER,S_UpdateDAT);
            }
            break;

        case OAM_MSG_XMLAGNET_TO_INTERPRET:/*����XML��������Ϣ*/
        case OAM_MSG_TELNET_TO_INTERPRET:/*telnetserver ��Ϣ*/
            /*OAM_MSG_XMLAGNET_TO_INTERPRET �Ǵ�������
            ����Ҫ�����Ϣ���ȣ�
            OAM_MSG_TELNET_TO_INTERPRETҪ�����Ϣ����*/
            if (OAM_MSG_TELNET_TO_INTERPRET == dwMsgId)
            {
                /*CRDCM00322153 ����յ�����Ϣ�����Ƿ����*/         
                if (!Oam_CfgCheckRecvMsgMinLen(sizeof(TYPE_TELNET_MSGHEAD)))
                {
                    XOS_SysLog(OAM_CFG_LOG_ERROR, "OAM_MSG_TELNET_TO_INTERPRET:The length of message received is less than the length of interface struct! ");
                    return;
                }       
            }

            pLinkState = OamCfgGetLinkState(dwMsgId,pMsgPara);
            if (!pLinkState)
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR,
                           "failled to get linkstate while processing telnetserver msg, vtyno in msg is %d",
                           ((TYPE_TELNET_MSGHEAD *)pMsgPara)->bVtyNum);
                break;
            }

            /* ��������Telnet����Ϣ */
            RecvFromTelnet(pLinkState,pMsgPara);
            break;

            /*ҵ������û�Ӧ��Ϣ*/
        case EV_OAM_CFG_CFG:
            HandleMsgEV_OAM_CFG_CFG(dwMsgId, pMsgPara, bIsSameEndian);
            break;
#if 0
        case EV_XML_OMMCFG_ACK:/*��ҵ��Ӧ����Ϣ������OMM*/
            {
                RecvFromOP(pMsgPara);
                break;
            }
        case EV_XML_OMMCFG_REQ:/*��OMM����Ϣ������OAM*/
            {
                ReceiveFromOmm(pMsgPara);
                break;
            }
        case EV_XML_CEASECMD_REQ:/*��OMM��������ֹ�����͵�OMM*/
            {
                ReceiveCeaseCmdMsgFromOmm(pMsgPara);
                break;
            }
        case EV_XML_REGISTER_REQ:/*���ܶ�XMLע������*/
            {
                ReceiveRegisterMsgFromOmm(pMsgPara);
                break;
            }
        case EV_XML_UNREGISTER_REQ:/*���ܶ�XMLע������*/
            {
                ReceiveUnRegisterMsgFromOmm(pMsgPara);
                break;
            }
        case EV_XML_HANDSHAKE_REQ:/*���ܶ�XML��������*/
            {
                ReceiveHandShakeMsgFromOmm(pMsgPara);
                break;
            }
        case EV_TIMER_XML_OMM_HANDSHAKE:/*�����ܶ����ֶ�ʱ��*/
            {
                RecvFromXmlAdapterTimer(NULL, dwMsgId);
                break;
            }
        case EV_TIMER_EXECUTE_XMLADAPTER:/*XML��������ǰ̨Ӧ��ʱ��*/
            {
                TYPE_XMLLINKSTATE *pXmlLinkState = NULL; 
                pXmlLinkState = XmlCfgGetLinkState(dwMsgId,pMsgPara);
                if (!pXmlLinkState)
                {
                    XOS_SysLog(OAM_CFG_LOG_ERROR,
                           "failled to get linkstate while processing timer msg, vtyno in msg is %d",
                           XOS_GetConstParaOfCurTimer());
                    break;
                }
                RecvFromXmlAdapterTimer(pXmlLinkState, dwMsgId);
                break;
            }
        case EV_XML_LINKCFG_REQ:/*XML�����������ܶ���·��������*/
            {
                ReceiveLinkCfgMsgFromOmm(pMsgPara);
                break;
            }
#endif
            /*ҵ���txt�������ݻָ�������Ϣ*/
        case EV_TXTCFG_REQ:
            RecvTxtCfgReq();
            break;
            /* Managerֱ���յ�ƽ̨����ע�ắ����Ϣ */
        case EV_OAMCLI_APP_REG:
            RecvAgtReg(ptVar, pMsgPara, bIsSameEndian);
            break;
            /* Managerֱ���յ�SBC��ҵ��ע�ắ����Ϣ */
        case EV_OAMCLI_SPECIAL_REG:
            RecvAgtRegSpecial(ptVar, pMsgPara, bIsSameEndian);
            break;
            /* VMM���ص�SBC��ҵ��dat����Ϣ */
        case EV_OAMS_VERINFO_ACK:
            RecvDatVerInfoAck(pMsgPara);
            break;
            /* Managerֱ���յ���ƽ̨����ע�ắ����Ϣ */
        case EV_OAMCLI_APP_REGEX:
            RecvAgtRegEx(ptVar, pMsgPara, bIsSameEndian);
            break;

            /* Managerֱ���յ�ƽ̨����ע��������Ϣ */
        case EV_OAMCLI_APP_UNREG:
            RecvAgtUnreg(ptVar, pMsgPara, bIsSameEndian);
            break;

            /* Manager�յ�Agentͬ����Ϣ���� */
        case EV_OAMCLI_APP_REG_A2M:
            Oam_CfgRecvRegA2M(ptVar, pMsgPara, bIsSameEndian);
            break;

        case EV_OAMCLI_APP_REGEX_A2M:
            RecvRegExA2M(ptVar, pMsgPara, bIsSameEndian);
            break;
            /* ��ʱɨ��OMP�Ĺ����ڴ���Ϣ */
        case EV_TIMER_SCAN_REGSHM:
            if (BOARD_MASTER ==  XOS_GetBoardMSState())
            {
                Oam_CfgScanRegTable();
            }
            break;
            /*ÿ��������صĶ�ʱ����Ϣ*/
        case EV_TIMER_IDLE_TIMEOUT:
        /* 614000678048 (ȥ��24Сʱ���Գ�ʱ) */
        /*case EV_TIMER_ABS_TIMEOUT:*/
        case EV_TIMER_EXECUTE_NORMAL:
            pLinkState = OamCfgGetLinkState(dwMsgId,pMsgPara);
            if (!pLinkState)
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR,
                           "failled to get linkstate while processing timer msg, vtyno in msg is %d",
                           XOS_GetConstParaOfCurTimer(pMsgPara));
                break;
            }

            RecvFromTimer(pLinkState,dwMsgId);
            break;
            /* ������·�ָ� ��Ϣ*/
        case EV_MATE_COMM_OK_OAM:
            if (BOARD_MASTER ==  XOS_GetBoardMSState())
            {
                Oam_SendCfgFileToSlave();
            }
            break;

            /*������·����Ϣ*/
        case EV_MATE_COMM_ABORT_OAM:
            break;

            /* ������OMP��·����Ϣ */
        case EV_COMM_ABORT:
            /*case OAM_OMP_SLAVE_TO_MASTER:*/
            XOS_SysLog(OAM_CFG_LOG_NOTICE, "EV_COMM_ABORT msg was received!");
            Oam_CfgCloseAllLink(NULL);
            break;

            /* ������OMP��·�ָ���Ϣ */
        case EV_COMM_OK:
            break;
            /*�û���Ϣ����ͬ����Ϣ*/
        case OAM_MSG_MS_CFGFILE_SYN:
            /*agent�û���Ϣcfg�ļ�����ͬ��*/
            if (BOARD_SLAVE ==  XOS_GetBoardMSState())
            {/*�������*/
                Oam_RcvCfgFileFromMaster(pMsgPara);
                ReadCfgScript();
            }
            else
            {/*���巢��*/
                Oam_SendCfgFileToSlave();
            }
            break;

            /* ��ת����Ϣ*/
        case EV_MASTER_TO_SLAVE:
            HandleMasterToSlave();
            break;

            /* ��ת����Ϣ*/
        case EV_SLAVE_TO_MASTER:
            HandleSlaveToMaster();
            break;

            /*ע��dat�л���Ӧ*/
        case EV_VER_OAMS_NOTIFYREG_ACK:
            RecvRegOamsNtfAck(pMsgPara);
            break;

            /*ע��dat�л���Ӧ��ʱ��Ϣ*/
        case EV_TIMER_REGOAMS_NTF_ACK:
            RecvRegOamsNtfTimeOut();
            break;
        case MSG_OFFLINE_CFG_TO_INTERPRET:
            Oam_CfgHandleMSGOffline2Online(pLinkState, pMsgPara);
            break;
        case OAM_CFG_FINISH_ACK:
            Oam_CfgHandleMSGOffline2OnlineFinish();
            break;
        /* ������յ����巢�����û��ļ�ͬ��������Ϣ */
        case EV_CFG_USERFILE_REQ:
            if (BOARD_MASTER ==  XOS_GetBoardMSState())
            {
                if(!OamSendUserfileToSlave())
                {
                    XOS_SysLog(OAM_CFG_LOG_ERROR, "[MASTER] OamSendUserfileToSlave exec failed!\n");
                }
            }
            break;
        /* ������յ�������û��ļ�����Ӧ�� */
        case EV_CFG_USERFILE_ACK:
            if (BOARD_SLAVE ==  XOS_GetBoardMSState())
            {
                if(!OamRecvUserFileFromMaster(pMsgPara))
                {
                    XOS_SysLog(OAM_CFG_LOG_ERROR, "[SLAVE] OamRecvUserFileFromMaster exec failed!\n");
                }
            }
            break;
        /* �յ�Ӧ�÷��صĽ�����Ϣ */
        case EV_OAM_CFG_PLAN:
            HandleMsgEV_OAM_CFG_PLAN(dwMsgId, pMsgPara, bIsSameEndian);
            break;
        /* �ȴ�Ӧ�÷��ؽ�����Ϣ��5s��ʱ */
        case EV_TIMER_CMD_PLAN:
        /*611001634189 VMMģʽ������IPMC���������Ƚ�����ʾ����*/
        /*
            pLinkState = OamCfgGetLinkState(dwMsgId,pMsgPara);
            if (!pLinkState)
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR, "Failled to get linkstate when receive EV_TIMER_CMD_PLAN msg! \n");
                break;
            }
            Oam_DealPlanTimer(pLinkState);
        */
            break;
#if 0
        /* �ȴ�Ӧ�÷��ؽ�����Ϣ��5s��ʱ��XMLͨ��ʹ�� */
        case EV_TIMER_CMD_PLAN_XML:
            {
                TYPE_XMLLINKSTATE *pXmlLinkState = NULL; 
                pXmlLinkState = XmlCfgGetLinkState(dwMsgId,pMsgPara);
                if (!pXmlLinkState)
                {
                    XOS_SysLog(OAM_CFG_LOG_ERROR,
                        "failled to get linkstate while processing timer msg, vtyno in msg is %d",
                        XOS_GetConstParaOfCurTimer());
                    break;
                }
                XML_DealPlanTimer(pXmlLinkState);
                break;
            }
            break;
#endif
        /* �յ����ķ�����Ϣ */
        case EV_SUBSCR_EVENT_TOAPP:
            OamRecvMateSubScr(pMsgPara);
            break;
        case EV_TIMER_REQ_USER_FILE:
            if (BOARD_SLAVE ==  XOS_GetBoardMSState())
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR, "###### Request user file from master timeout!\n");

                /* ���������������û������ļ� */
                XOS_Msend(EV_CFG_USERFILE_REQ, NULL, 0, 0, 0, &g_ptOamIntVar->jidSelf);
            }
            break;
        case  EV_SAVEOK:
        case  EV_SAVEFAIL:
	 {
            _db_t_saveresult *pSaveResult  = NULL;
            WORD16 wDbNum = 0;   	 	
            MSG_COMM_OAM *ptRtnMsg = (MSG_COMM_OAM *)g_ptOamIntVar->aucRecvBuf;			
            WORD16 wOmmLinkNum = sizeof(gt_XmlLinkState)/sizeof(TYPE_XMLLINKSTATE);
            WORD16 wLoop = 0;
            if (!Oam_CfgCheckRecvMsgMinLen(sizeof(_db_t_saveresult)))
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR, "_db_t_saveresult:The length of message received is less than the length of interface struct! ");
                XOS_SetNextState(PROCTYPE_OAM_CLIMANAGER, S_Work);
                return;
            }             
            pLinkState = OamCfgGetLinkState(dwMsgId,pMsgPara);
            if (!pLinkState)
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR,
                           "failled to get linkstate while processing application msg");
                XOS_SetNextState(PROCTYPE_OAM_CLIMANAGER, S_Work);
                break;
            }
    
            /* �յ����̽��֮��ɱ���ȴ�������Ϣ�Ķ�ʱ�� */
            Oam_KillCmdPlanTimer(pLinkState);
            Oam_KillExeTimer(pLinkState);
            /*�ֽ���װ��Ҫʵ��*/
            pSaveResult = (_db_t_saveresult  *)pMsgPara;  			
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
			
        /*���Ӧ�õĽ�����Ϣ����0, ˵��Ӧ��ǰһ���ǽ�����Ϣ��
        ��û�з���100, ����һ��100%������*/
        if (pLinkState->ucPlanValuePrev > 0)
        {        
            CHAR szPlanStr[MAX_STRING_LEN] = {0};
            CHAR *pPlanStr = szPlanStr;
            BYTE ucPlan = 100;
	
            /* ��������Ϣ���͵�telnet��ʾ */
            XOS_snprintf(pPlanStr, MAX_STRING_LEN, map_szExePlanFmtStr, ucPlan);
            strncat(pPlanStr, "\n\r", MAX_STRING_LEN);
            /*pLinkState->bOutputCmdLine = FALSE;*/
            pLinkState->bOutputMode = OUTPUT_MODE_ROLL;
            pLinkState->wAppMsgReturnCode = SUCC_WAIT_AND_HAVEPARA;
            SendStringToTelnet(pLinkState, pPlanStr);

            pLinkState->bOutputMode = OUTPUT_MODE_NORMAL;
        }
		
            Oam_InnerApp((LPVOID)pSaveResult,dwMsgId,ptRtnMsg);
	     if(pLinkState == &gt_SaveLinkState)
	     {    
                MSG_COMM_OAM *pOamMsgBufTmp = (MSG_COMM_OAM *)ptRtnMsg;
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
                pOamMsgBufTmp->LinkChannel = wLoop | 0x200;
	     }
            Oam_RecvFromCliApp(dwMsgId,(LPVOID)ptRtnMsg);	
	     break;										 	
         }			
        /* CLI�ű��������ͷ����˱仯*/
        case EV_OAM_CLI_SCRIPT_LANGUAGE_CHANGED:
        case EV_TIMER_CLI_LOAD_SCRIPT_NEW_LANG:
            Oam_Cli_ChangeScriptLanguage(pMsgPara);
            break;		
        default:
            break;
        }
        break;

    case S_UpdateDAT:
        switch(dwMsgId)
        {
            /* dat�汾�л���Ϣ*/
        case EV_OAMS_VER_UPDATE_NOTIFY:
            XOS_SysLog(OAM_CFG_LOG_NOTICE, "reveive new ver update notify msg, reset update wait timer");
            /*���ö�ʱ�����ȵȴ����������������µ�dat�ļ�*/
            Oam_KillDATUpdateWaitTimer();
            Oam_SetDATUpdateWaitTimer();
            break;

            /* ���°汾��ʱ����Ϣ(�յ��л���Ϣ֮��ʼ�ȴ�)*/
        case EV_TIMER_DAT_UPDATE_WAIT:
            ExecDatUpdateWaitTimerProc();
            break;

            /* dat�汾��Ϣ��Ϣ*/
        case EV_OAMS_VERINFO_ACK:
            RecvDatVerInfoAck(pMsgPara);
            break;
        case EV_TIMER_GET_CFGVERINFO:
            RecvDatVerInfoGetTimeOut();
            break;

        case OAM_MSG_TELNET_TO_INTERPRET:/* telnet��Ϣ*/
            if (!Oam_CfgCheckRecvMsgMinLen(sizeof(TYPE_TELNET_MSGHEAD)))
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR, "OAM_MSG_TELNET_TO_INTERPRET:The length of message received is less than the length of interface struct! ");
                return;
            }
            
            pLinkState = OamCfgGetLinkState(dwMsgId,pMsgPara);
            if (!pLinkState)
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR,
                           "failled to get linkstate while processing telnetserver msg, vtyno in msg is %d",
                           ((TYPE_TELNET_MSGHEAD *)pMsgPara)->bVtyNum);
                break;
            }
            if (pec_MsgHandle_EXECUTE == Oam_LinkGetCurRunState(pLinkState))
            {
                /* ��������Telnet����Ϣ */
                RecvFromTelnet(pLinkState,pMsgPara);
            }
            else
            {/* ��������Telnet����Ϣ */
                TYPE_TELNET_MSGHEAD *ptMsg = (TYPE_TELNET_MSGHEAD *)pMsgPara;
                /*����Ƕ�����ݽ���������������ݻ��߽��յ��˳���Ϣ�ģ����Դ���*/
                if ((ptMsg->bMsgID == MSGID_REQUESTDATA) ||
                    (ptMsg->bMsgID == MSGID_TERMINATE))
                {
                    RecvFromTelnet(pLinkState,pMsgPara);
                }
                else
                {
                    if (ptMsg->bMsgID == MSGID_NEWCONNECT)
                    {
                        pLinkState->bSSHConnRslt = FALSE;
                        Oam_LinkSetLinkId(pLinkState, ptMsg->bVtyNum);
                    }
                    
                    SendStringToTelnet(pLinkState, Hdr_szDatUpdateMutexTip);
                }
                
                if (pec_MsgHandle_IDLE == Oam_LinkGetCurRunState(pLinkState))
                {
                    Oam_InnerCmdLogout(pLinkState);
                }
            }
            break;

            /*ע��dat�л���Ӧ*/
        case EV_VER_OAMS_NOTIFYREG_ACK:
            RecvRegOamsNtfAck(pMsgPara);
            break;

            /*ע��dat�л���Ӧ��ʱ��Ϣ*/
        case EV_TIMER_REGOAMS_NTF_ACK:
            RecvRegOamsNtfTimeOut();
            break;
            
        /*ҵ������û�Ӧ��Ϣ*/
        case EV_OAM_CFG_CFG:
            HandleMsgEV_OAM_CFG_CFG(dwMsgId, pMsgPara, bIsSameEndian);
            break;
#if 0
        case EV_XML_OMMCFG_ACK:/*��ҵ��Ӧ����Ϣ������OMM*/
            {
                RecvFromOP(pMsgPara);
                break;
            }
        case EV_XML_OMMCFG_REQ:/*��OMM����Ϣ������OAM*/
            {
                ReceiveFromOmm(pMsgPara);
                break;
            }
        case EV_XML_CEASECMD_REQ:/*��OMM��������ֹ�����͵�OMM*/
            {
                ReceiveCeaseCmdMsgFromOmm(pMsgPara);
                break;
            }
        case EV_XML_REGISTER_REQ:/*���ܶ�XMLע������*/
            {
                ReceiveRegisterMsgFromOmm(pMsgPara);
                break;
            }
        case EV_XML_UNREGISTER_REQ:/*���ܶ�XMLע������*/
            {
                ReceiveUnRegisterMsgFromOmm(pMsgPara);
                break;
            }
        case EV_XML_HANDSHAKE_REQ:/*���ܶ�XML��������*/
            {
                ReceiveHandShakeMsgFromOmm(pMsgPara);
                break;
            }
        case EV_XML_LINKCFG_REQ:/*XML�����������ܶ���·��������*/
            {
                ReceiveLinkCfgMsgFromOmm(pMsgPara);
                break;
            }
        case EV_TIMER_XML_OMM_HANDSHAKE:/*�����ܶ����ֶ�ʱ��*/
            {
                RecvFromXmlAdapterTimer(NULL, dwMsgId);
                break;
            }
        case EV_TIMER_EXECUTE_XMLADAPTER:/*XML��������ǰ̨Ӧ��ʱ��*/
            {
                TYPE_XMLLINKSTATE *pXmlLinkState = NULL; 
                pXmlLinkState = XmlCfgGetLinkState(dwMsgId,pMsgPara);
                if (!pXmlLinkState)
                {
                    XOS_SysLog(OAM_CFG_LOG_ERROR,
                           "failled to get linkstate while processing timer msg, vtyno in msg is %d",
                           XOS_GetConstParaOfCurTimer());
                    break;
                }
                RecvFromXmlAdapterTimer(pXmlLinkState, dwMsgId);
                break;
            }
#endif
        /* �յ����ķ�����Ϣ */
        case EV_SUBSCR_EVENT_TOAPP:
            OamRecvMateSubScr(pMsgPara);
            break;
        /* ��ת����Ϣ*/
        case EV_MASTER_TO_SLAVE:
            HandleMasterToSlave();
            break;
        default :
            break;
        }
        break;
    #if 0 /*ȥ������״̬�����������work״̬���*/
    case S_CfgSave:
        switch(dwMsgId)
        {
        /* ���̿�ʼ��Ϣ */
        case OAM_MSG_SAVE_BEGIN:
            if (!Oam_CfgCheckRecvMsgMinLen(sizeof(BYTE)))
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR, "OAM_MSG_SAVE_BEGIN:The length of message received is less than the length of interface struct! ");
                return;
            }
            
            pLinkState = OamCfgGetLinkState(dwMsgId,pMsgPara);
            if (!pLinkState)
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR,
                           "[SAVE_ERR] Failled to get linkstate while processing application msg");
                XOS_SetNextState(S_Work);
                break;
            }

            /*����linkstate���汣��Ĵ�����Ϣ*/
            Oam_ExecSaveCmd(pLinkState);
            break;
            /* telnet��Ϣ��Ϣ*/
        case OAM_MSG_TELNET_TO_INTERPRET:
            pLinkState = OamCfgGetLinkState(dwMsgId,pMsgPara);
            if (!pLinkState)
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR,
                           "[Oam_Interpret] Failled to get linkstate while processing telnet msg");
                /*XOS_SetNextState(S_Work);*/
                break;
            }
            /*����Ƿ�����·���ڴ���*/
            if (OtherLinkIsSaving(pLinkState))
            {
                TYPE_TELNET_MSGHEAD *ptMsg = (TYPE_TELNET_MSGHEAD *)pMsgPara;
                
                if (ptMsg->bMsgID == MSGID_NEWCONNECT)
                {
                    memset(pLinkState, 0, sizeof(TYPE_LINK_STATE));
                    Oam_LinkInitializeForNewConn(pLinkState);
                    Oam_LinkSetLinkId(pLinkState, ptMsg->bVtyNum);
                    pLinkState->bOutputCmdLine = FALSE;
                    SendLogoutToTelnet(pLinkState,Hdr_szSaveMutexTip);
                }
                else
                {
                    pLinkState->wAppMsgReturnCode = ERR_AND_HAVEPARA;
                    pLinkState->bOutputCmdLine = TRUE;
                    SendStringToTelnet(pLinkState, Hdr_szSaveMutexTip);
                }
                return;
            }
            else
            {
                if((CMD_DEF_SET_SAVE == pLinkState->tMatchResult.dExeID) && 
                    (pec_MsgHandle_EXECUTE == pLinkState->ucCurRunState))
                    XOS_SysLog(OAM_CFG_LOG_DEBUG, "[Oam_Interpret] Now is executing save command!\n");
                else
                    XOS_SysLog(OAM_CFG_LOG_DEBUG, "[Oam_Interpret] No other link is executing save command!\n");
            }
            
            break;
            /* ��������DBS����Ϣ */
        case  EV_SAVEOK:
        case  EV_SAVEFAIL:
            {
                CHAR *pErrString = NULL;
                BOOLEAN isNeedTxtSave = FALSE;
                WORD32 dwErrStrSize = 0;

                if (!Oam_CfgCheckRecvMsgMinLen(sizeof(_db_t_saveresult)))
                {
                    XOS_SysLog(OAM_CFG_LOG_ERROR, "_db_t_saveresult:The length of message received is less than the length of interface struct! ");
                    XOS_SetNextState(S_Work);
                    return;
                } 
                
                pLinkState = OamCfgGetLinkState(dwMsgId,pMsgPara);
                if (!pLinkState)
                {
                    XOS_SysLog(OAM_CFG_LOG_ERROR,
                               "failled to get linkstate while processing application msg");
                    XOS_SetNextState(S_Work);
                    break;
                }

                /* �յ����̽��֮��ɱ���ȴ�������Ϣ�Ķ�ʱ�� */
                Oam_KillCmdPlanTimer(pLinkState);

                Oam_KillExeTimer(pLinkState);
                    
                if (EV_SAVEFAIL == dwMsgId)
                    dwErrStrSize = ((_db_t_saveresult *)pMsgPara)->wDbNum * MAX_STRING_LEN;
                else
                    dwErrStrSize = MAX_STRING_LEN;

                pErrString = (CHAR *)XOS_GetUB(dwErrStrSize);
                if (NULL == pErrString)
                {
                    XOS_SysLog(OAM_CFG_LOG_ERROR, "failled to GetUB for ErrString\n");
                    XOS_SetNextState(S_Work);
                    break;
                }
                memset(pErrString,0,dwErrStrSize);
                RecvFromDBS(dwMsgId, pMsgPara,pErrString, dwErrStrSize, bIsSameEndian);

                isNeedTxtSave = FALSE;
                pLinkState->wAppMsgReturnCode = SUCC_AND_NOPARA;
                SendZDBSaveAckToClient(pLinkState,dwMsgId, pErrString, isNeedTxtSave);
                OAM_RETUB(pErrString);
                Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_TELNET);
                XOS_SetNextState(S_Work);
            }
            break;
        /* �յ�Ӧ�÷��صĽ�����Ϣ */
        case EV_OAM_CFG_PLAN:
            HandleMsgEV_OAM_CFG_PLAN(dwMsgId, pMsgPara, bIsSameEndian);
            break;
        /* �ȴ�Ӧ�÷��ؽ�����Ϣ��5s��ʱ */
        case EV_TIMER_CMD_PLAN:
            pLinkState = OamCfgGetLinkState(dwMsgId,pMsgPara);
            if (!pLinkState)
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "Failled to get linkstate when receive EV_TIMER_CMD_PLAN msg! \n");
                XOS_SetNextState(S_Work);
                break;
            }
            Oam_DealPlanTimer(pLinkState);
            break;
        case EV_TIMER_CMD_PLAN_XML:
            {
                TYPE_XMLLINKSTATE *pXmlLinkState = NULL; 
                pXmlLinkState = XmlCfgGetLinkState(dwMsgId,pMsgPara);
                if (!pXmlLinkState)
                {
                    XOS_SysLog(OAM_CFG_LOG_ERROR,
                        "failled to get linkstate while processing timer msg, vtyno in msg is %d",
                        XOS_GetConstParaOfCurTimer());
                    XOS_SetNextState(S_Work);
                    break;
                }
                XML_DealPlanTimer(pXmlLinkState);
                break;
            }
            break;
            /* ҵ��ָ�����Ϣ*/
        case EV_OAM_CFG_CFG:
            /*�ú��������Oam_SaveCfgProcess�����̳ɹ������м�ʧ�ܶ���ת��S_Work״̬*/
            HandleMsgEV_OAM_CFG_CFG(dwMsgId, pMsgPara, bIsSameEndian);
            break;
        case EV_XML_OMMCFG_ACK:/*��ҵ��Ӧ����Ϣ������OMM*/
            {
                RecvFromOP(pMsgPara);
                break;
            }
        case EV_XML_OMMCFG_REQ:/*��OMM����Ϣ������OAM*/
            {
                ReceiveFromOmm(pMsgPara);
                break;
            }
        case EV_XML_CEASECMD_REQ:/*��OMM��������ֹ�����͵�OMM*/
            {
                ReceiveCeaseCmdMsgFromOmm(pMsgPara);
                break;
            }
        case EV_XML_REGISTER_REQ:/*���ܶ�XMLע������*/
            {
                ReceiveRegisterMsgFromOmm(pMsgPara);
                break;
            }
        case EV_XML_UNREGISTER_REQ:/*���ܶ�XMLע������*/
            {
                ReceiveUnRegisterMsgFromOmm(pMsgPara);
                break;
            }
        case EV_XML_HANDSHAKE_REQ:/*���ܶ�XML��������*/
            {
                ReceiveHandShakeMsgFromOmm(pMsgPara);
                break;
            }        
        case EV_TIMER_XML_OMM_HANDSHAKE:/*�����ܶ����ֶ�ʱ��*/
            {
                RecvFromXmlAdapterTimer(NULL, dwMsgId);
                break;
            }
        case EV_TIMER_EXECUTE_XMLADAPTER:/*XML��������ǰ̨Ӧ��ʱ��*/
            {
                TYPE_XMLLINKSTATE *pXmlLinkState = NULL; 
                pXmlLinkState = XmlCfgGetLinkState(dwMsgId,pMsgPara);
                if (!pXmlLinkState)
                {
                    XOS_SysLog(OAM_CFG_LOG_ERROR,
                           "failled to get linkstate while processing timer msg, vtyno in msg is %d",
                           XOS_GetConstParaOfCurTimer());
                    break;
                }
                RecvFromXmlAdapterTimer(pXmlLinkState, dwMsgId);
                break;
            }
        case EV_XML_LINKCFG_REQ:/*XML�����������ܶ���·��������*/
            {
                ReceiveLinkCfgMsgFromOmm(pMsgPara);
                break;
            }
            /*ִ�ж�ʱ����Ϣ��Ϣ*/
        case EV_TIMER_EXECUTE_NORMAL:
            pLinkState = OamCfgGetLinkState(dwMsgId,pMsgPara);
            if (!pLinkState)
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR,
                           "failled to get linkstate while processing application msg, vtyno in msg is %d",
                           ((MSG_COMM_OAM *)pMsgPara)->LinkChannel);
                break;
            }

            /* ��������Timer����Ϣ */
            RecvFromTimer(pLinkState,dwMsgId);
            XOS_SetNextState(S_Work);
            break;
        case MSG_OFFLINE_CFG_TO_INTERPRET:
            Oam_CfgHandleMSGOffline2Online(pLinkState, pMsgPara);
            break;
        case OAM_CFG_FINISH_ACK:
            Oam_CfgHandleMSGOffline2OnlineFinish();
            break;

        /* ��ת����Ϣ*/
        case EV_MASTER_TO_SLAVE:
            HandleMasterToSlave();
            break;
            
        /* ��ת����Ϣ*/
        case EV_SLAVE_TO_MASTER:
            HandleSlaveToMaster();
            break;
        /* �յ����ķ�����Ϣ */
        case EV_SUBSCR_EVENT_TOAPP:
            OamRecvMateSubScr(pMsgPara);
            break;

        default:
            break;
        }
        break;
       #endif 
    case S_Restore:
        switch(dwMsgId)
        {
        case OAM_MSG_RESTORE_PREPARE_OK:
        case OAM_MSG_RESTORE_CONTINUE:
        case EV_OAM_CFG_CFG:
        case EV_TIMER_EXECUTE_NORMAL:  /* added by fls on 2009-02-27 (CRDCR00481329 rpu����֮��,���������������ý��벻��) */
            RecvFromRestore(dwMsgId, pMsgPara, bIsSameEndian);
            break;

            /*ע��dat�л���Ӧ*/
        case EV_VER_OAMS_NOTIFYREG_ACK:
            RecvRegOamsNtfAck(pMsgPara);
            break;

            /*ע��dat�л���Ӧ��ʱ��Ϣ*/
        case EV_TIMER_REGOAMS_NTF_ACK:
            RecvRegOamsNtfTimeOut();
            break;

        /* ��ת����Ϣ*/
        case EV_MASTER_TO_SLAVE:
            HandleMasterToSlave();
            break;
            
        /* ��ת����Ϣ*/
        case EV_SLAVE_TO_MASTER:
            HandleSlaveToMaster();
            break;
        /* �յ����ķ�����Ϣ */
        case EV_SUBSCR_EVENT_TOAPP:
            OamRecvMateSubScr(pMsgPara);
            break;

        default:
            break;
        }
        break;
    default:
        break;
    }
    XOS_SetDefaultNextState();
}

/**************************************************************************
*                          �ֲ�����ʵ��                                   *
**************************************************************************/

/**************************************************************************
* �������ƣ�VOID ReqAllCfgVerInfo
* ������������scs����ȫ��dat�汾�ļ���Ϣ
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
void ReqAllCfgVerInfo(void)
{
#if 0
    T_OamsVerInfoReq tCfgVerInfoReq;
    JID tJidVmmMgt;
    BYTE ucLoopCnt = 0;
    WORD16 wFuncTypeMaxNum = sizeof(tCfgVerInfoReq.wFuncType)/sizeof(tCfgVerInfoReq.wFuncType[0]);
    T_CliAppReg   *ptOamCliReg = NULL;
    WORD16        wLoop = 0;

    /* ����vmm���̵�JID*/
    memset(&tJidVmmMgt, 0, sizeof(tJidVmmMgt));
    XOS_GetSelfJID(&tJidVmmMgt);    
    tJidVmmMgt.dwJno = XOS_ConstructJNO(SCS_VM_MGT, 1);

    memset(&tCfgVerInfoReq, 0, sizeof(tCfgVerInfoReq));
    tCfgVerInfoReq.wVerType = VER_TYPE_SCRIPT;
    tCfgVerInfoReq.ucNewOrOld =  OAMS_VER_NEW;
    
    tCfgVerInfoReq.ucVerNum = OAM_MIN(wFuncTypeMaxNum, DATFILE_FUNCTYPE_TOTAL);

    XOS_SysLog(OAM_CFG_LOG_ERROR, 
        "######[ReqAllCfgVerInfo]: g_ptOamIntVar->ucVerInfoReqCnt = %d\n", g_ptOamIntVar->ucVerInfoReqCnt);

    /* ��һ������50��ƽ̨��DDM dat */
    if (g_ptOamIntVar->ucVerInfoReqCnt == 0)
    {
        for (ucLoopCnt = 0; ucLoopCnt < tCfgVerInfoReq.ucVerNum; ucLoopCnt++)
        {
            assert(ucLoopCnt < tCfgVerInfoReq.ucVerNum);
            if (ucLoopCnt >= tCfgVerInfoReq.ucVerNum)
            {
                    XOS_SysLog(OAM_CFG_LOG_ERROR, 
                        "######[ReqAllCfgVerInfo]: tCfgVerInfoReq.ucVerNum is too small, request top %u function types\n", ucLoopCnt);
                break;
            }
            tCfgVerInfoReq.wFuncType[ucLoopCnt] = DATFILE_FUNCTYPE_CFG_BEGIN + ucLoopCnt;
        }
    }
    /* �ڶ�������50��ƽ̨DDM dat */
    else if (g_ptOamIntVar->ucVerInfoReqCnt == 1)
    {
        for (ucLoopCnt = 0; ucLoopCnt < tCfgVerInfoReq.ucVerNum; ucLoopCnt++)
        {
            assert(ucLoopCnt < tCfgVerInfoReq.ucVerNum);
            if (ucLoopCnt >= tCfgVerInfoReq.ucVerNum)
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR, 
                    "######[ReqAllCfgVerInfo]: tCfgVerInfoReq.ucVerNum is too small, request top %u function types\n", ucLoopCnt);
                break;	
            }
            tCfgVerInfoReq.wFuncType[ucLoopCnt] = DATFILE_FUNCTYPE_DDM_BEGIN + ucLoopCnt;
        }
    }
    /* ����������50��SBC��ҵ���dat */
    else
    {
        /******** ���SBC��ҵ��dat�Ĺ������� ********/
        /*** 1����ɨ�蹲���ڴ� ***/
        ptOamCliReg = Oam_GetRegTableAndLock();
        if (!ptOamCliReg)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, 
                "######[ReqAllCfgVerInfo]: failed to create shm which contains rigister table and set lock on it!\r");

            /* �쳣����ʱ�������������0 */
            g_ptOamIntVar->ucVerInfoReqCnt = 0;
            return;
        }
        
        for(ucLoopCnt = 0; ucLoopCnt < OAM_CLI_DAT_SHM_MAX_NUM; ucLoopCnt++)
        {
            if (wLoop >= DATFILE_FUNCTYPE_SPECIAL_NUM)
                break;

            if (ucLoopCnt >= OAM_CLI_DAT_SHM_MAX_NUM)
                break;

            if(OAM_CLI_REG_TYPE_SPECIAL == ptOamCliReg->tCliReg[ucLoopCnt].ucRegType)
            {
                tCfgVerInfoReq.wFuncType[wLoop] = ptOamCliReg->tCliReg[ucLoopCnt].dwDatFuncType;
                wLoop++;
            }
        }

        if (!Oam_UnLockRegTable(ptOamCliReg))
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, 
                "######[ReqAllCfgVerInfo]: failed to unlock shm which contains register table ");
        }

        /*** 2���ٲ鿴ȫ�������� ***/
        for(ucLoopCnt = 0; ucLoopCnt < g_ptOamIntVar->tPlatMpCliAppInfo.dwRegCount; ucLoopCnt++)
        {
            if (wLoop >= DATFILE_FUNCTYPE_SPECIAL_NUM)
                break;

            if (ucLoopCnt >= OAM_CLI_DAT_SHM_MAX_NUM)
                break;

            if(OAM_CLI_REG_TYPE_SPECIAL == g_ptOamIntVar->tPlatMpCliAppInfo.tCliReg[ucLoopCnt].ucRegType)
            {
                tCfgVerInfoReq.wFuncType[wLoop] = g_ptOamIntVar->tPlatMpCliAppInfo.tCliReg[ucLoopCnt].dwDatFuncType;
                wLoop++;
            }
        }

        tCfgVerInfoReq.ucVerNum = wLoop;

        if (tCfgVerInfoReq.ucVerNum == 0)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, "######ReqAllCfgVerInfo: No Secpical dat need to request...");
               
            /* ���һ����������������������0 */
            g_ptOamIntVar->ucVerInfoReqCnt = 0;
            return;
        }

    }

    /*��scs����ȫ��dat�汾�ļ���Ϣ*/
    XOS_SendAsynMsg(EV_OAMS_VERINFO_REQ, (BYTE *)&tCfgVerInfoReq, sizeof(tCfgVerInfoReq), 0, 0, &tJidVmmMgt);

    Oam_SetGetCfgVerInfoTimer();
    return;
#endif
}

static void CreateOamDir(void)
{
    XOS_STATUS dwRtn = XOS_SUCCESS;
    CHAR  acOamDir[FILE_PATH_LEN] = {'\0'};
    CHAR  acMateDir[FILE_PATH_LEN] = {'\0'}; 

    XOS_CreateDirectoryEx(OAM_CLI_PATH);
    if (!OamGetPath(acOamDir, acMateDir))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[CreateOamDir]OamGetPath fail!\n");
        return;
    }
    XOS_SysLog(OAM_CFG_LOG_DEBUG, "[CreateOamDir]acOamDir = [%s]\n", acOamDir);
    XOS_SysLog(OAM_CFG_LOG_DEBUG, "[CreateOamDir]acMateDir = [%s]\n", acMateDir);
    
    /*���Ŀ¼�������򴴽�Ŀ¼*/
    dwRtn = XOS_CreateDirectoryEx(acOamDir);
    if (XOS_SUCCESS != dwRtn && XOS_FILE_ERR_DIR_EXIST != dwRtn)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[CreateOamDir]Create Oam Cfg Dir fail! dwRtn = %d\n", dwRtn);
    }
    else if (XOS_FILE_ERR_DIR_EXIST == dwRtn)
    {
        XOS_SysLog(OAM_CFG_LOG_DEBUG, "[CreateOamDir]Oam Cfg Dir is exists!\n");
    }
    else
    {
        XOS_SysLog(OAM_CFG_LOG_DEBUG, "[CreateOamDir]Create Oam Cfg Dir Success!\n");
    }

    /* ��cliĿ¼��ΪNFS���� 
    dwRtn = XOS_ExportDir(acOamDir);
    if (XOS_SUCCESS != dwRtn)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[CreateOamDir]XOS_ExportDir fail! dwRtn = %d\n", dwRtn);
    }
    else
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[CreateOamDir]XOS_ExportDir success!\n");
    }
*/
    dwRtn = XOS_CreateDirectoryEx(acMateDir);
    if (XOS_SUCCESS != dwRtn && XOS_FILE_ERR_DIR_EXIST != dwRtn)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[CreateOamDir]Create Oam Mate Dir fail! dwRtn = %d\n", dwRtn);
    }
    else if (XOS_FILE_ERR_DIR_EXIST == dwRtn)
    {
        XOS_SysLog(OAM_CFG_LOG_DEBUG, "[CreateOamDir]Oam Mate Dir is exists!\n");
    }
    else
    {
        XOS_SysLog(OAM_CFG_LOG_DEBUG, "[CreateOamDir]Create Oam Mate Dir Success!\n");
    }

    return;
}

/**************************************************************************
* �������ƣ�VOID Initialize
* ������������麯��ָ�������
* ���ʵı���
* �޸ĵı���
* ���������T_OAM_INTERPRET_VAR *ptVar
            BOOL isMaster
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
static BOOL Initialize(T_OAM_INTERPRET_VAR *ptVar, BOOL isMaster)
{
    INT32 iReadCfgResult = READERR_SUCCESS;
    WORD16 vty = 0;
    WORD16 i = 0;

//    XOS_TestVarSpace(sizeof(T_OAM_INTERPRET_VAR));

    g_ptOamIntVar = ptVar;
    memset(g_ptOamIntVar, 0, sizeof(T_OAM_INTERPRET_VAR));
    
    /* �������� JID */
    XOS_GetSelfJID(&(g_ptOamIntVar->jidSelf));
    g_ptOamIntVar->jidSelf.dwJno = PROCTYPE_OAM_CLIMANAGER;

    /* ����Telnet JID */
    XOS_GetSelfJID(&(g_ptOamIntVar->jidTelnet));
    g_ptOamIntVar->jidTelnet.dwJno = PROCTYPE_OAM_TELNETSERVER;
  //  g_ptOamIntVar->jidTelnet.dwJno = XOS_ConstructJNO(PROCTYPE_OAM_TELNETSERVER, 1);

    /*?? �����ǻָ�ʱ���õģ�v4Ӧ��ȥ��������ȫ�ֱ�����ʽ*/
    g_ptOamIntVar->dwTxtFileFd = OAM_INVALID_FD;
    g_ptOamIntVar->pcTxtFilePos = g_ptOamIntVar->pcTxtFileBuf;
    g_ptOamIntVar->bRestoreFirstPrint = TRUE;

    g_ptOamIntVar->wGetCfgVerInfoTimer = INVALID_TIMER_ID;
    g_ptOamIntVar->wDatUpdateWaitTimer = INVALID_TIMER_ID;
    g_ptOamIntVar->wRegOamsNotifyTimer = INVALID_TIMER_ID;

    g_ptOamIntVar->bSupportStringWithBlank = TRUE;

    g_ptOamIntVar->ucVerNotifyRegReqCnt = 0;
    g_ptOamIntVar->ucVerInfoReqCnt = 0;

    memset(g_ptOamIntVar->sOutputResultBuf, 0, MAX_OUTPUT_RESULT);
    memset(g_ptOamIntVar->sCmdLineBuf, 0, MAX_CMDLINE_LEN);
    
    /*?? ��ʼ������ȫ��linkstate��Ա������ռ䣬ԭ���ڵ�¼ʱ���ʼ����
    ���ǲ�֧���������õ���Ԫ��¼����ǰ����Ҫ���*/
    memset(gtLinkState, 0, sizeof(gtLinkState));
    for (vty = MIN_CLI_VTY_NUMBER; vty <= sizeof(gtLinkState)/sizeof(TYPE_LINK_STATE); vty++)
    {
        gtLinkState[vty-MIN_CLI_VTY_NUMBER].sOutputResult = g_ptOamIntVar->sOutputResultBuf;
        gtLinkState[vty-MIN_CLI_VTY_NUMBER].sCmdLine = g_ptOamIntVar->sCmdLineBuf;
        /* 614000678048 (ȥ��24Сʱ���Գ�ʱ) */
        /*gtLinkState[vty-MIN_CLI_VTY_NUMBER].wAbsTimer = INVALID_TIMER_ID;*/
        gtLinkState[vty-MIN_CLI_VTY_NUMBER].wExeTimer = INVALID_TIMER_ID;
        gtLinkState[vty-MIN_CLI_VTY_NUMBER].wIdleTimer = INVALID_TIMER_ID;
        gtLinkState[vty-MIN_CLI_VTY_NUMBER].bOutputMode = OUTPUT_MODE_NORMAL;
        gtLinkState[vty-MIN_CLI_VTY_NUMBER].ucCurRunState = pec_MsgHandle_IDLE;
    }
    for (vty = 0;vty < sizeof(g_ptOamIntVar->pMemBufOri)/sizeof(g_ptOamIntVar->pMemBufOri[0]);vty++)
    {
        /*Ϊÿ����·�����ڴ�ռ�*/
        g_ptOamIntVar->pMemBufOri[vty] = (CHAR *)XOS_GetUB(MAX_MEM_BUF);
        /*����ռ�ʧ��*/
        if (NULL == g_ptOamIntVar->pMemBufOri[vty])
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR,
					"....................FUNC: %s,Line:%d XOS_GetUB Error!....................",__func__,__LINE__);
            return FALSE;
        }
        /*��ʼ���ڴ�*/
        memset(g_ptOamIntVar->pMemBufOri[vty],0,sizeof(MAX_MEM_BUF));
    }
    /*��ʼ������ת����ר��linkstate*/
    memset(&g_tLinkStateForOfflineToOnline, 0, sizeof(g_tLinkStateForOfflineToOnline));
    g_tLinkStateForOfflineToOnline.sOutputResult = g_ptOamIntVar->sOutputResultBuf;
    g_tLinkStateForOfflineToOnline.sCmdLine  = g_ptOamIntVar->sCmdLineBuf;

    /*��ʼ��xmlͨ������ר��linkstate*/
    memset(&gt_SaveLinkState, 0, sizeof(gt_SaveLinkState));
    gt_SaveLinkState.sOutputResult = g_ptOamIntVar->sOutputResultBuf;
    gt_SaveLinkState.sCmdLine  = g_ptOamIntVar->sCmdLineBuf;

    memset(gtUserInfo, 0, sizeof(gtUserInfo));
    for (i = 0; i < sizeof(gtUserInfo)/sizeof(TYPE_USER_INFO); i++)
    {
        gtUserInfo[i].ucRole = 0;
        gtUserInfo[i].bLocked = FALSE;
        memset(gtUserInfo[i].szUserName, 0, MAX_OAM_USER_NAME_LEN);
        memset(gtUserInfo[i].szPassWord, 0, MAX_OAM_USER_PWD_ENCRYPT_LEN);
    }

    /*XML����������*/
    Xml_InitLinkState();    

    /*��ʼ��omp��ע���õĹ����ڴ�*/
    if (OamCliMgtInitShm() == OAM_CLI_FAIL)
    {
       XOS_SysLog(OAM_CFG_LOG_ERROR, "there are some errors in the processing of OamCliMgtInitShm");
    }

    /* ��ʼ��OAMĿ¼ */
    CreateOamDir();

    /* �����û���Ϣ */
    if (!OamLoadUserFile())
    {
        /* ��������û��ļ�ʧ�ܣ�������Ĭ���û���Ĭ�����룬�Է�ֹ���ܵ�¼����� */
        UseDefaultUserInfo();
    }

    /*��ȡ�û���Ϣ���ýű��ļ�*/
    iReadCfgResult = ReadCfgScript();
    if (iReadCfgResult != READERR_SUCCESS)
    {
        /*ʹ��Ĭ����Ϣ*/
        ClearCfgScript();
        XOS_SysLog(OAM_CFG_LOG_NOTICE, "No user config info was got, use default user info");
    }

    if (!InitUserLogFile())
    {
        XOS_SysLog(OAM_CFG_LOG_NOTICE, "Read user log file failed!\n");
    }

    /* �ϵ��ʱ���ȡ��������־�ļ�������������������������д��־ʱ����Ӱ������ */
    g_ptOamIntVar->wLinesOfCmdLogFile = 0;
    if(!GetCmdLogFileLines())
        XOS_SysLog(OAM_CFG_LOG_ERROR, "Get lines of log file fail!\n");

    /* ���ı������µ���Ϣ */
    if(isMaster)
    {
        OamMateRequestSubScr();
    }

    g_ptOamIntVar->ucRegOamsNtfTimes = 0;

    /*
        �����ļ���ʼ���ڴ�:
        pc���汾dat�ļ���Ϣ�������ļ��У�����ֱ�Ӷ�ȡʹ��
        ��Ƭ�汾Ҫ�ȴ�vmm��ȡdat�汾��Ϣ
    */
    if (Oam_CfgIfCurVersionIsPC())
    {
        if (GetScriptFileList(g_aucScriptPath) == FALSE)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, "Failed to create script configure file");
            return FALSE;
        }
		
        /*�������ļ���ȡdat�汾��Ϣ���ڴ�*/
        Oam_CfgGetDatVerInfoFromIniFile();
        /*�����pc���汾��ֱ�Ӹ��������ļ���ʼ��dat�ڴ�*/
        if (!OAM_CFGInitGlobalDatMem())
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, "Failed to initialize global dat memory! all jobs can't register");
            return FALSE;
        }
    }
    else
    {
        RegDatToVmm();
        /*��Ƭ�汾������Ļ�����vmm����汾��Ϣ*/
        if(isMaster)
        {
            /*����Ǵ�vmm��ȡdat��Ϣ����scs��������dat�ļ���Ϣ*/
            g_ptOamIntVar->ucCfgVerInfoGetTimes = 0;
            XOS_SysLog(OAM_CFG_LOG_NOTICE, "Begin to get all dat ver info from vmm ...");
            ReqAllCfgVerInfo();
        }
    }

    /*ע��������õ�dat
    if(OamCliRegFunc(g_ptOamIntVar->jidSelf, OAM_CFG_DAT_FUNCTYPE_OAM) != OAM_CLI_SUCCESS)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "Dat file of OAM registe failed! can not use OAM config shell");
    }
*/
    OAM_CliReRegisterAllRegistedJobs();
    
    /* ���ö�ʱɨ��DATע����Ϣ�Ĺ����ڴ� */
    XOS_SetLoopTimer(PROCTYPE_OAM_CLIMANAGER, TIMER_SCAN_REGSHM, DURATION_SCAN_REGSHM, PARAM_NULL);

    /*����Ǳ���,�����û������ļ�����ͬ��*/
    if (!isMaster)
    {
        /*~ ���Ӱ汾�ź����ȼ� */
        XOS_Msend(OAM_MSG_MS_CFGFILE_SYN, NULL, 0, 0, 0, &g_ptOamIntVar->jidSelf);
        Oam_CfgPowerOnOverAndToWorkState();
    }
    
    /*��oammanagerע����Ժ�������*/
    //OamRegDbgFunc("CliMHelp()","Show all debug functions of CliManager.");
	
    return TRUE;
}


/**********************************************************************
* �������ƣ�BYTE  OamCliMgtInitShm
* ����������Manager�ϵ�ʱ��ʹ�������ڴ棬ֻ���������
*                          ��ת��Ҳͬʱ��Ҫ����
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ��OAM_CLI_SUCCESS(0)���ɹ�
*           OAM_CLI_FAIL(1)��   ʧ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* 2008-7-28             ������
* ---------------------------------------------------------------------
* 2008-7-28             ������      ����
************************************************************************/
BYTE  OamCliMgtInitShm(void)
{
#if 0
    WORD16        wLoop = 0;
    T_CliAppReg   *ptOamCliReg = NULL;

    /*������ʹ�ú����Oam_UnLockRegTable�ͷ���*/
    ptOamCliReg = Oam_GetRegTableAndLock();
    if (!ptOamCliReg)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "failed to create shm which contains rigister table and set lock on it!\r");
        return OAM_CLI_FAIL;
    }

    /* �������ȡ��flag��� */
    for(wLoop=0; wLoop<OAM_CLI_DAT_SHM_MAX_NUM; wLoop++)
    {
        ptOamCliReg->tCliReg[wLoop].ucReadFlag = OAM_CLI_REG_NOT_READED;
    }

    if (!Oam_UnLockRegTable(ptOamCliReg))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "failed to unlock shm which contains register table ");
    }
#endif
    return OAM_CLI_SUCCESS;
}


/*****************************************************************************/
/**
@brief ��ʾCLIMANAGER��Ӧ�õ�ע����Ϣ�����ڶ�λ�û������Ҳ��������
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
\nOMP: RegCount = 6
DatFuncType = 1, Jno = 136052737, DAT readflag = 1
DatFuncType = 2, Jno = 202113025, DAT readflag = 1
DatFuncType = 6, Jno = 143458305, DAT readflag = 1
DatFuncType = 421, Jno = 144769025, DAT readflag = 1
DatFuncType = 403, Jno = 144769025, DAT readflag = 1
DatFuncType = 402, Jno = 144769025, DAT readflag = 1

Other MP: RegCount = 0

@li @b �޸ļ�¼��
-#  �� 
*/
/*****************************************************************************/
void OAM_DbgShowCliMgrRegInfo(void)
{
#if 0
    WORD16          wLoop = 0;
    T_CliAppReg     *tOamCliReg = NULL;
    T_CliAppAgtRegEx *ptOamCliExReg = NULL;

    /* ͬ��ƽ̨Ӧ��ע�����Ϣ */
    /*������ʹ�ú����Oam_UnLockRegTable�ͷ���*/
    tOamCliReg = Oam_GetRegTableAndLock();
    if (!tOamCliReg)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "failed to create shm which contains rigister table and set lock on it!\r");
        return;
    }


    /* ��Manager����ע�������Ϣ */
    printf("\nRegistered jobs count on OMP: %d\n", tOamCliReg->dwRegCount);
    printf( "        [Cli register table on OMP]\n");
    printf( "DatFuncType,       Jno,  DAT readflag \n");
    printf( "==========================================\n");
    for(wLoop = 0; wLoop < OAM_CLI_DAT_SHM_MAX_NUM; wLoop++)
    {
        if(0 == tOamCliReg->tCliReg[wLoop].dwDatFuncType)
            break;

        printf( "%11d,%10d,%14d\n",
                tOamCliReg->tCliReg[wLoop].dwDatFuncType,
                tOamCliReg->tCliReg[wLoop].tAppJid.dwJno,
                tOamCliReg->tCliReg[wLoop].ucReadFlag);
    }
    printf( "==========================================\n");

    if (!Oam_UnLockRegTable(tOamCliReg))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "failed to unlock shm which contains register table ");
    }

    if (!g_ptOamIntVar)
    {
        return;
    }
    tOamCliReg = &g_ptOamIntVar->tPlatMpCliAppInfo;

    printf("\nRegistered jobs count on other MP: %d\n", tOamCliReg->dwRegCount);
    if (tOamCliReg->dwRegCount > 0)
    {
        printf( "        [Cli register table on other MP]\n");
        printf( "DatFuncType,       Jno,  DAT readflag \n");
        printf( "==========================================\n");
    for(wLoop = 0; wLoop < OAM_CLI_DAT_SHM_MAX_NUM; wLoop++)
    {
        if(0 == tOamCliReg->tCliReg[wLoop].dwDatFuncType)
            break;

            printf( "%11d,%10d,%14d\n",
                tOamCliReg->tCliReg[wLoop].dwDatFuncType,
                tOamCliReg->tCliReg[wLoop].tAppJid.dwJno,
                tOamCliReg->tCliReg[wLoop].ucReadFlag);
    }
        printf( "==========================================\n");
    }

    /*��ӡ��ƽ̨ע����Ϣ*/
    /*������ʹ�ú����Oam_UnLockRegExTable�ͷ���*/
    ptOamCliExReg = Oam_GetRegExTableAndLock();
    if (ptOamCliExReg)
    {
        printf("\nExRegister count in share memory:%d\n", ptOamCliExReg->dwRegCount);
        for(wLoop = 0; wLoop < ptOamCliExReg->dwRegCount; wLoop++)
        {
            if(wLoop >= OAM_CLI_APP_SHM_NUM)
            {
                break;
            }
            printf("NetId=%d,Jno=%d,DevId=%d,Module=%d,Unit=%d,SUnit=%d,SubSystem=%d,RouteType=%d\n",
                    ptOamCliExReg->tCliReg[wLoop].dwNetId,
                    ptOamCliExReg->tCliReg[wLoop].tAppJid.dwJno,
                    ptOamCliExReg->tCliReg[wLoop].tAppJid.dwDevId,
                    ptOamCliExReg->tCliReg[wLoop].tAppJid.wModule,
                    ptOamCliExReg->tCliReg[wLoop].tAppJid.wUnit,
                    ptOamCliExReg->tCliReg[wLoop].tAppJid.ucSUnit,
                    ptOamCliExReg->tCliReg[wLoop].tAppJid.ucSubSystem,
                    ptOamCliExReg->tCliReg[wLoop].tAppJid.ucRouteType);
        }
        Oam_UnLockRegExTable(ptOamCliExReg);
    }
    if (!g_ptOamIntVar)
    {
        return;
    }
    ptOamCliExReg = &(g_ptOamIntVar->tLompCliAppRegInfo);
    printf("\nExRegister count in global-data-area: %d\n", ptOamCliExReg->dwRegCount);
    for(wLoop=0; wLoop < ptOamCliExReg->dwRegCount; wLoop++)
    {
        if(wLoop >= OAM_CLI_APP_SHM_NUM)
        {    
            break;
        }
        printf("NetId=%d,Jno=%d,DevId=%d,Module=%d,Unit=%d,SUnit=%d,SubSystem=%d,RouteType=%d\n",
             ptOamCliExReg->tCliReg[wLoop].dwNetId,
             ptOamCliExReg->tCliReg[wLoop].tAppJid.dwJno,
             ptOamCliExReg->tCliReg[wLoop].tAppJid.dwDevId,
             ptOamCliExReg->tCliReg[wLoop].tAppJid.wModule,
             ptOamCliExReg->tCliReg[wLoop].tAppJid.wUnit,
             ptOamCliExReg->tCliReg[wLoop].tAppJid.ucSUnit,
             ptOamCliExReg->tCliReg[wLoop].tAppJid.ucSubSystem,
             ptOamCliExReg->tCliReg[wLoop].tAppJid.ucRouteType);
    }
#endif
    return;
}
/*lint -e661*/

/*****************************************************************************/
/**
@brief  ��ӡCLI��صļ���ģ�������Ϣ��ӡ�������ú����������ڲ鿴ϵͳ��־��ӡ
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
\n1����ӡ��������tulip�ĵ��Ժ�������Ҫ�Ѵ�ӡ���������ushellִ��
2��Ŀǰ��ӡ����climanager��telnetserver��cmmagent����job�Ĵ�ӡ���ú���
@li @b ���ʾ����
\nXOS_DbgSetSlJobLevel(0x81c,7)
XOS_DbgSetSlJobLevel(0x81d,7)
XOS_DbgSetSlJobLevel(0x81f,7)
XOS_DbgSetSlSysLevel(7)
XOS_DbgSetSlDirLevel(3,7)
@li @b �޸ļ�¼��
-#  �� 
*/
/*****************************************************************************/
void OAM_DbgShowCliLogFuncs(void)
{
    printf("XOS_DbgSetSlJobLevel(0x%x,7)\n", PROCTYPE_OAM_CLIMANAGER);
    printf("XOS_DbgSetSlJobLevel(0x%x,7)\n", PROCTYPE_OAM_TELNETSERVER);
//    printf("XOS_DbgSetSlJobLevel(0x%x,7)\n", PROCTYPE_OAM_CMMAGENT);
    printf("XOS_DbgSetSlSysLevel(7)\n");
    printf("XOS_DbgSetSlDirLevel(3,7)\n");
}

/***********************************************************************
* �������ƣ�VOID UpdateDatFilesAndReg
* �����������汾�л��󣬸���ע�������ע������job
* ���ʵı���
* �޸ĵı���
* ���������
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-8-18             ���      ����
************************************************************************/
void OAM_CliReRegisterAllRegistedJobs(void)
{
//    WORD32          dwRegNum = 0;
 //   T_CliAppReg     *ptPriData = NULL;
    WORD16          wLoop = 0;

    /*���·����ʼ��ȫ��dat�ڴ�*/
    if (!OAM_CFGInitGlobalDatMem())
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "######[OAM_CliReRegisterAllRegistedJobs]: OAM_CFGInitGlobalDatMem failed...");
        return ;
    }

    for (wLoop = 0; wLoop < g_ptOamIntVar->wDatSum; wLoop++)
    {
        if(OAM_CLI_SUCCESS != OAM_CFGDatRegister(g_ptOamIntVar->jidSelf, g_ptOamIntVar->tDatVerInfo[wLoop].tFileInfo.wFuncType))
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, "[OAM_CliReRegisterAllRegistedJobs] Job register fail,  functype = %d", g_ptOamIntVar->tDatVerInfo[wLoop].tFileInfo.wFuncType);
            continue;
        }
    }
#if 0
    /*���ù����ڴ�������job��û��ע��*/
    OamCliMgtInitShm();

    /*�����������б����ע��job Ϊû��ע��*/
    dwRegNum  = g_ptOamIntVar->tPlatMpCliAppInfo.dwRegCount;
    ptPriData = &g_ptOamIntVar->tPlatMpCliAppInfo;
    for(wLoop=0; wLoop<dwRegNum; wLoop++)
    {
        if(wLoop >= OAM_CLI_DAT_SHM_MAX_NUM)
        {
            break;
        }

        /*���ע���־Ϊ0*/
        ptPriData->tCliReg[wLoop].ucReadFlag = OAM_CLI_REG_NOT_READED;
    }

    /*ɨ��ע�����δע��Ľ���ע��*/
    Oam_CfgScanRegTable();    
#endif
}

void SetSaveTypeAsTxt(const JID jid)
{
#if 0
    WORD16          wLoop = 0;
    T_CliAppReg     *tOamCliReg = NULL;

    /* ͬ��ƽ̨Ӧ��ע�����Ϣ */
    /*������ʹ�ú����Oam_UnLockRegTable�ͷ���*/
    tOamCliReg = Oam_GetRegTableAndLock();
    if (!tOamCliReg)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "failed to create shm which contains rigister table and set lock on it!\r");
        return;
    }

    /* ��Manager����ע�������Ϣ */
    for(wLoop = 0; wLoop < OAM_CLI_DAT_SHM_MAX_NUM; wLoop++)
    {
        if(0 == tOamCliReg->tCliReg[wLoop].dwDatFuncType)
            break;

        if(EqualJID(&tOamCliReg->tCliReg[wLoop].tAppJid, &jid))
        {
            tOamCliReg->tCliReg[wLoop].ucSaveType = SAVE_TYPE_AS_TXT;
            break;
        }
    }

    /*ȫ��������*/
    for(wLoop = 0; wLoop < g_ptOamIntVar->tPlatMpCliAppInfo.dwRegCount; wLoop++)
    {
        if (0 == g_ptOamIntVar->tPlatMpCliAppInfo.tCliReg[wLoop].dwDatFuncType)
            break;

        if (EqualJID(&g_ptOamIntVar->tPlatMpCliAppInfo.tCliReg[wLoop].tAppJid, &jid))
        {
            g_ptOamIntVar->tPlatMpCliAppInfo.tCliReg[wLoop].ucSaveType = SAVE_TYPE_AS_TXT;
            break;
        }
    }

    if (!Oam_UnLockRegTable(tOamCliReg))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "failed to unlock shm which contains register table ");
    }
#endif
    return;
}

/**************************************************************************
* �������ƣ�VOID RecvDatVerInfoAck
* ��������������vmm���ص�dat�汾��Ϣ
* ���ʵı���
* �޸ĵı���
* ���������pMsgPara -- vmm���ص�����
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2008/10/15    V1.0    ���      ����
**************************************************************************/
void RecvDatVerInfoAck(LPVOID pMsgPara)
{
#if 0
    T_OamsVerInfoAck *pVerAck =  NULL;

    /*CRDCM00322153 ����յ�����Ϣ�����Ƿ����*/
    if (!Oam_CfgCheckRecvMsgMinLen(sizeof(T_OamsVerInfoAck)))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, 
            "######[RecvDatVerInfoAck]: The length of message received is less than the length of interface struct! ");

        /* �쳣���ʱ����Ҫ�����������0 */
        g_ptOamIntVar->ucVerInfoReqCnt = 0;
        return;
    }
    
    pVerAck = (T_OamsVerInfoAck *)pMsgPara;
    
    /* ɱ����ʱ�� */
    Oam_KillGetCfgVerInfoTimer();
    g_ptOamIntVar->ucCfgVerInfoGetTimes = 0;

    /* ÿ������֮���ڽ��յ�Ӧ��ĵط������������1 */    
    g_ptOamIntVar->ucVerInfoReqCnt++;

    XOS_SysLog(OAM_CFG_LOG_ERROR, 
        "######[RecvDatVerInfoAck]: receive dat ver info , %d  records in total, g_ptOamIntVar->ucVerInfoReqCnt = %d\n", 
        pVerAck->wRecordsNum,
        g_ptOamIntVar->ucVerInfoReqCnt);

    /* ����ڶ��η���Ӧ��֮���ܵ�dat����Ϊ0���������� */
    if (2 == g_ptOamIntVar->ucVerInfoReqCnt)
    {
        if (0 == (pVerAck->wRecordsNum + g_ptOamIntVar->wDatSum))
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, "######[RecvDatVerInfoAck]: receive 0 dat after two times req, go on requeset...\n");
            g_ptOamIntVar->ucVerInfoReqCnt = 0;
            ReqAllCfgVerInfo();
            return;
        }
    }

    /* ��һ������֮��ķ��أ�У�鷵�������а汾�������͵ķ�Χ */
    if ((1 == g_ptOamIntVar->ucVerInfoReqCnt) && (pVerAck->wRecordsNum > 0))
    {
        /* ��һ��������Ƿ�DDM�ű����������ͷ�ΧӦ����1-50�������Ķ��� */
        if ((pVerAck->tOamsVerInfo[0].wFuncType < DATFILE_FUNCTYPE_CFG_BEGIN) || 
            (pVerAck->tOamsVerInfo[0].wFuncType > (DATFILE_FUNCTYPE_CFG_BEGIN + DATFILE_FUNCTYPE_CFG_NUM - 1)))
         {   
         	  g_ptOamIntVar->ucVerInfoReqCnt--;
            return;
         }
    }

    /* �ڶ�������֮��ķ��أ�У�鷵�������а汾�������͵ķ�Χ */
    if ((2 == g_ptOamIntVar->ucVerInfoReqCnt) && (pVerAck->wRecordsNum > 0))
    {
        /* �ڶ����������DDM�ű����������ͷ�ΧӦ����401-450�������Ķ��� */
        if ((pVerAck->tOamsVerInfo[0].wFuncType < DATFILE_FUNCTYPE_DDM_BEGIN) || 
            (pVerAck->tOamsVerInfo[0].wFuncType > (DATFILE_FUNCTYPE_DDM_BEGIN + DATFILE_FUNCTYPE_DDM_NUM - 1)))
        {   
         	  g_ptOamIntVar->ucVerInfoReqCnt--;
            return;
        }
    }

    /* ����������֮��ķ��أ�У�鷵�������а汾�������͵ķ�Χ */
    if ((3 == g_ptOamIntVar->ucVerInfoReqCnt) && (pVerAck->wRecordsNum > 0))
    {
        /* �������������SBC�ű����������ͷ�Χ������������DDM�ͷ�DDM��Χ�ڣ������Ķ��� */
        if (((pVerAck->tOamsVerInfo[0].wFuncType >= DATFILE_FUNCTYPE_CFG_BEGIN) && 
              (pVerAck->tOamsVerInfo[0].wFuncType <= (DATFILE_FUNCTYPE_CFG_BEGIN + DATFILE_FUNCTYPE_CFG_NUM - 1))) || 
            ((pVerAck->tOamsVerInfo[0].wFuncType >= DATFILE_FUNCTYPE_DDM_BEGIN) && 
              (pVerAck->tOamsVerInfo[0].wFuncType <= (DATFILE_FUNCTYPE_DDM_BEGIN + DATFILE_FUNCTYPE_DDM_NUM - 1))))
         {   
         	  g_ptOamIntVar->ucVerInfoReqCnt--;
            return;
         }
    }

    /*�����µ�dat��Ϣ*/
    if (pVerAck->ucNewOrOld == OAMS_VER_NEW)
    {
        /* ��һ��Ӧ����Ҫ��ȫ�ֱ������ */
        if (g_ptOamIntVar->ucVerInfoReqCnt == 1)
        {
            g_ptOamIntVar->wDatSum = 0;
            memset(&g_ptOamIntVar->tDatVerInfo, 0, sizeof(g_ptOamIntVar->tDatVerInfo));
        }

        XOS_SysLog(OAM_CFG_LOG_ERROR, "######[RecvDatVerInfoAck]: g_ptOamIntVar->wDatSum = %d\n", g_ptOamIntVar->wDatSum);

        if (pVerAck->wRecordsNum > OAMS_VER_MAX)
        {
            WORD16 iFor;
            XOS_SysLog(OAM_CFG_LOG_ERROR, 
                "######[RecvDatVerInfoAck]: the number of dat ver info is more than max(%d) ", OAMS_VER_MAX);

            /* �������ܳ�����������ֵ */
            assert((g_ptOamIntVar->wDatSum + OAMS_VER_MAX) < MAX_VER_INFO_NUM);
            if ((g_ptOamIntVar->wDatSum + OAMS_VER_MAX) >= MAX_VER_INFO_NUM)
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR, 
                    "######[RecvDatVerInfoAck]: g_ptOamIntVar->wDatSum + OAMS_VER_MAX is more than max(%d)\n", MAX_VER_INFO_NUM);
                g_ptOamIntVar->ucVerInfoReqCnt = 0;
                return;
            }
           /*
            memcpy(&g_ptOamIntVar->tDatVerInfo[g_ptOamIntVar->wDatSum], 
                   pVerAck->tOamsVerInfo, 
                   sizeof(T_OamsVerInfo)*OAMS_VER_MAX);
           */
            for (iFor = 0; iFor < OAMS_VER_MAX; iFor++)
            {
                memcpy(&g_ptOamIntVar->tDatVerInfo[g_ptOamIntVar->wDatSum+iFor].tFileInfo, (BYTE *)&pVerAck->tOamsVerInfo[iFor], sizeof(T_OamsVerInfo));
            }

            g_ptOamIntVar->wDatSum = g_ptOamIntVar->wDatSum + OAMS_VER_MAX;
        }
        else
        {
            /* �������ܳ�����������ֵ */
            assert((g_ptOamIntVar->wDatSum + pVerAck->wRecordsNum) < MAX_VER_INFO_NUM);
            if ((g_ptOamIntVar->wDatSum + pVerAck->wRecordsNum) >= MAX_VER_INFO_NUM)
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR, 
                    "######[RecvDatVerInfoAck]: g_ptOamIntVar->wDatSum + pVerAck->wRecordsNum is more than max(%d)\n", MAX_VER_INFO_NUM);
                g_ptOamIntVar->ucVerInfoReqCnt = 0;
                return;
            }

            if (pVerAck->wRecordsNum > 0)
            {
                WORD16 iFor;
                /*
                memcpy(&g_ptOamIntVar->tDatVerInfo[g_ptOamIntVar->wDatSum], 
                       pVerAck->tOamsVerInfo, 
                       sizeof(T_OamsVerInfo)*pVerAck->wRecordsNum);
                */	
                for (iFor = 0; iFor < pVerAck->wRecordsNum; iFor++)
                {
                    memcpy(&g_ptOamIntVar->tDatVerInfo[g_ptOamIntVar->wDatSum+iFor].tFileInfo, (BYTE *)&pVerAck->tOamsVerInfo[iFor], sizeof(T_OamsVerInfo));
                }
                g_ptOamIntVar->wDatSum = g_ptOamIntVar->wDatSum + pVerAck->wRecordsNum;
            }
        }

        Oam_ShowAllVerInfo();

        if (S_Init == XOS_GetCurState())
        {
            /* �������:1���ڶ�������֮���Ӧ����û��SBC��ҵ���dat������; 2����SBC��ҵ������� */
            if (((g_ptOamIntVar->ucVerInfoReqCnt == 2) && (!SpecialRegistered())) || 
                (g_ptOamIntVar->ucVerInfoReqCnt == 3))
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR, "######[RecvDatVerInfoAck]: initial memory for dat files and register oam dat file ...");
                if (!OAM_CFGInitGlobalDatMem())
                {
                    /*?? ��ʼ��dat���ȫ�ֿռ�ʧ�ܣ���Ҫ��Ӹ澯*/
                    XOS_SysLog(OAM_CFG_LOG_ERROR, "######[RecvDatVerInfoAck]: Call OAM_CFGInitGlobalDatMem failed!dat files can't register");
                }

                /* ����������û�0 */
                g_ptOamIntVar->ucVerInfoReqCnt = 0;

                Oam_CfgPowerOnOverAndToWorkState();
            }
        }
        else
        {
            if (((g_ptOamIntVar->ucVerInfoReqCnt == 2) && (!SpecialRegistered())) || 
                (g_ptOamIntVar->ucVerInfoReqCnt == 3))
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR, "######[RecvDatVerInfoAck]: ReRegister all jobs...");

                /* ����������û�0 */
                g_ptOamIntVar->ucVerInfoReqCnt = 0;
    
                OAM_CliReRegisterAllRegistedJobs();
    
                XOS_SysLog(OAM_CFG_LOG_ERROR, "######[RecvDatVerInfoAck]: new dat update finished, change to work state");
                /*�л��������ӵ���Ȩģʽ*/
                /*611002035007 �����ԸĽ����飨3����cli>����Ҫ����en���ܽ���cli#*/
                OAM_CliChangeAllLinkToStartMode(map_szScriptReloaded);
                XOS_SetNextState(S_Work);
            }
        }
    }
#if 0
    /*����BRS�ɵ�dat��Ϣ*/
    else
    {
        memcpy(&g_ptOamIntVar->tBrsDatVerInfo, pVerAck->tOamsVerInfo, sizeof(T_OamsVerInfo));
        XOS_SysLog(OAM_CFG_LOG_NOTICE, "all dat ver info was got, change to work state");
        Oam_CfgPowerOnOverAndToWorkState();
    }
#endif
    /* ����VMMÿ�����֧��50��dat����������Ҫ�ֶ������ */
    ReqAllCfgVerInfo();
#endif
}

/**************************************************************************
* �������ƣ�VOID RecvRegOamsNtfAck
* ��������������vmm���ص�dat notify reg��Ӧ
* ���ʵı���
* �޸ĵı���
* ���������pMsgPara -- vmm���ص�����
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2008/10/15    V1.0    ���      ����
**************************************************************************/
void RecvRegOamsNtfAck(LPVOID pMsgPara)
{
#if 0
    T_OamsVerNotifyRegAck *pVerAck =  NULL;
    
    /*CRDCM00322153 ����յ�����Ϣ�����Ƿ����*/
    if (!Oam_CfgCheckRecvMsgMinLen(sizeof(T_OamsVerNotifyRegAck)))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, 
            "######[RecvRegOamsNtfAck]: The length of message received is less than the length of interface struct! ");

        /* �쳣���ʱ����Ҫ�����������0 */
        g_ptOamIntVar->ucVerNotifyRegReqCnt = 0;
        return;
    }
    
    pVerAck = (T_OamsVerNotifyRegAck *)pMsgPara;
    
    Oam_KillRegOamsNotifyTimer();
    g_ptOamIntVar->ucRegOamsNtfTimes = 0;

    /* ÿ������֮���ڽ��յ�Ӧ��ĵط������������1 */
    g_ptOamIntVar->ucVerNotifyRegReqCnt++;

    if (SCS_OK == pVerAck->dwRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "######[RecvRegOamsNtfAck]: receive EV_VER_OAMS_NOTIFYREG_ACK, register success!");
    }
    else
    {
        /*?? �汾�л�֪ͨע��ʧ�ܣ���Ҫ��Ӹ澯�����޷��յ��л���Ϣ*/
        XOS_SysLog(OAM_CFG_LOG_ERROR, "######[RecvRegOamsNtfAck]: receive EV_VER_OAMS_NOTIFYREG_ACK, register failed!");
    }

    if (g_ptOamIntVar->ucVerNotifyRegReqCnt >= 3)
    {
        g_ptOamIntVar->ucVerNotifyRegReqCnt = 0;
        return;
    }

    /* ����VMMÿ�����֧��50��dat����������Ҫ�ֶ������ */
    RegDatToVmm();
#endif
}

/**************************************************************************
* �������ƣ�VOID RecvDatVerInfoGetTimeOut
* ��������������vmm���ص�dat�汾��Ϣ
* ���ʵı���
* �޸ĵı���
* ���������pMsgPara -- vmm���ص�����
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2008/10/15    V1.0    ���      ����
**************************************************************************/
void RecvDatVerInfoGetTimeOut(void)
{
    Oam_SetGetCfgVerInfoTimerInvalid();
    g_ptOamIntVar->ucCfgVerInfoGetTimes++;
    XOS_SysLog(OAM_CFG_LOG_ERROR, 
        "######[RecvDatVerInfoGetTimeOut]: timeout to got dat ver info from vmm %d times, g_ptOamIntVar->ucVerInfoReqCnt = %d\n",
        g_ptOamIntVar->ucCfgVerInfoGetTimes,
        g_ptOamIntVar->ucVerInfoReqCnt);

    ReqAllCfgVerInfo();
}

/**************************************************************************
* �������ƣ�VOID RecvRegOamsNtfTimeOut
* ��������������ע��dat�л���Ӧ�ȴ���ʱ
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2008/10/15    V1.0    ���      ����
**************************************************************************/
void RecvRegOamsNtfTimeOut(void)
{
    Oam_SetRegOamsNotifyTimerInvalid();
    g_ptOamIntVar->ucRegOamsNtfTimes++;
    XOS_SysLog(OAM_CFG_LOG_ERROR, 
        "######[RecvRegOamsNtfTimeOut]: timeout to reg oams notify to vmm %d times, g_ptOamIntVar->ucVerNotifyRegReqCnt = %d\n",
        g_ptOamIntVar->ucRegOamsNtfTimes,
        g_ptOamIntVar->ucVerNotifyRegReqCnt);

    if (g_ptOamIntVar->ucRegOamsNtfTimes < OAM_CFG_REGOAMS_NTF_RETRY_TIMES)
    {
        RegDatToVmm();
    }
    else
    {
        /*?? dat�汾�л�֪ͨע��ʧ�ܣ���Ҫ��Ӹ澯���汾�л�ʱ���ܸ���dat*/
        XOS_SysLog(OAM_CFG_LOG_ERROR, 
            "######[RecvRegOamsNtfTimeOut]: failed to reg oams notify to vmm! dat update notify msg will not received ...");
        g_ptOamIntVar->ucVerNotifyRegReqCnt = 0;
        g_ptOamIntVar->ucRegOamsNtfTimes = 0;
    }
}
#if 0
VOID OamCfgRemoveAllDat(VOID)
{
    WORD32 wCnt = 0;

    /*�������ע���dat�ڴ�*/
    for (wCnt = 0; wCnt < g_ptOamIntVar->wDatSum; wCnt++)
    {
        if (g_tDatLink)
            OAM_CFGClearCmdScript(&(g_tDatLink[wCnt].datIndex));
    }

    /*�������ȫ���ڴ�ռ�*/
    OAM_RETUB(gtMode);
    g_dwModeCount = 0;

    OAM_RETUB(gtTreeNode);
    g_wFreeNode = 1;

    OAM_RETUB(g_tDatLink);
    g_wRegDatCount = 0;

    g_ptOamIntVar->dwModeSum = 0;
    g_ptOamIntVar->dwTreeNodeSum = 0;

    XOS_SysLog(OAM_CFG_LOG_NOTICE, "All memory allocated to dat process were freed");
}
#endif
/**************************************************************************
* �������ƣ�VOID RecvTxtCfgReq
* ��������������txt�������ݻָ�����
* ���ʵı���
* �޸ĵı���
* ���������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2008/10/15    V1.0    ���      ����
**************************************************************************/
void RecvTxtCfgReq(void)
{
    JID tAppJid;
    WORD32 dwRestoreErr = 0;
    T_Time tStart, tEnd;
    BYTE bRet = 0;

    /*���淢��TXT�ָ������JID*/
    memset(&tAppJid, 0, sizeof(JID));
    XOS_Sender(&tAppJid);
    memcpy(&g_ptOamIntVar->jidProtocol, &tAppJid, sizeof(JID));
    /*���ô�������:TXT����ZDB*/
    SetSaveTypeAsTxt(tAppJid);

    XOS_GetCurrentTime(&tStart);
    dwRestoreErr = Oam_RestorePrtclCfgPrepare();
    XOS_GetCurrentTime(&tEnd);
    g_ptOamIntVar->dwMillSecOnPrepare += Oam_Duration(tStart, tEnd);

    if(OAM_RESTORE_PREPARE_OK == dwRestoreErr)
    {
        /* ���ػָ��õ�dat���ڴ�:
            ����ǵ�Ƭ�汾:��Ҫ��ǰ���ȡ��dat(new or old)���ص��ڴ�
            �����pc���汾:Ŀǰֻ֧������dat���лָ�            
        */
        if (Oam_CfgIfCurVersionIsPC())
        {
            /*�����pc��������Ϊ�˼򻯴���ֱ��ע��һ��brs dat�ļ�*/
            bRet = OAM_CFGDatRegister(tAppJid, FUNC_SCRIPT_CLIS_BRS);
            if (OAM_CLI_SUCCESS != bRet)
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR, "!!!Dat file of OAM registe failed! can not use OAM config shell!\n");
            }
        }
        else
        {
            /*����ǵ�Ƭ������������Ѽ��ص�����dat
            �ټ��ػָ�ר�õ�dat�ļ����ڴ�*/
            OAM_CFGRemoveAllDat();

            bRet = OAM_CFGDatRegisterForRestore(tAppJid, FUNC_SCRIPT_CLIS_BRS);
            if (OAM_CLI_SUCCESS != bRet)
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR, "!!!Dat file of OAM registe failed! can not use OAM config shell!\n");
                
                /*����ǵ�Ƭ�汾����Ҫ���¼��������õ�dat�ļ�*/
                if (!Oam_CfgIfCurVersionIsPC())
                {
                    OAM_CFGClearCmdScript(&(g_ptOamIntVar->tCfgdat));
                    OAM_CFGRemoveAllDat();

                    OAM_CliReRegisterAllRegistedJobs();
                }
                
                /*ע��ʧ��Ҳ��dat������*/
                EndRestoreByReasonOf(OAM_TXT_NO_DAT);
                return;
            }
        }

        XOS_SysLog(OAM_CFG_LOG_NOTICE,
                   "Handle EV_TXTCFG_REQ Msg Success! Restore Repare OK!");
        /* ���Լ����ͻָ�׼�����������Ϣ */
        XOS_SendAsynMsg(OAM_MSG_RESTORE_PREPARE_OK, NULL, 0, 0, 0, &g_ptOamIntVar->jidSelf);
        XOS_SetNextState(PROCTYPE_OAM_CLIMANAGER, S_Restore);
    }
    else if (OAM_RESTORE_PREPARE_FILE_NOT_EXIST == dwRestoreErr)
    {
        Oam_PrintRstrLog("Oam_RestorePrtclCfgPrepare error, no save file exist, restore success!\n");
        EndRestoreByReasonOf(OAM_TXT_SUCCESS);
    }
    else if (OAM_RESTORE_PREPARE_NO_DAT == dwRestoreErr)
    {
        Oam_PrintRstrLog("Oam_RestorePrtclCfgPrepare error, brs.dat file not exist, restore success!\n");
        EndRestoreByReasonOf(OAM_TXT_NO_DAT); 
    }
    else
    {
        Oam_PrintRstrLog("Oam_RestorePrtclCfgPrepare error, restore failed!\n");
        EndRestoreByReasonOf(dwRestoreErr);       
    }
}

/**************************************************************************
* �������ƣ�EndRestoreByReasonOf
* ���������������ָ����������ؽ����brs
* ���ʵı���
* �޸ĵı���
* ���������dwReason -- �ָ�����ԭ��
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2008/10/15    V1.0    ���      ����
**************************************************************************/
void EndRestoreByReasonOf(WORD32 dwReason)
{    
    if (g_ptOamIntVar->dwTxtFileFd != NULL)
    {
        XOS_CloseFile(g_ptOamIntVar->dwTxtFileFd);
        g_ptOamIntVar->dwTxtFileFd = OAM_INVALID_FD;
    }

    Oam_CloseRestoreLogFile();
    XOS_SendAsynMsg(EV_TXTCFG_ACK, (BYTE *)&dwReason, sizeof(dwReason), 0, 0, &g_ptOamIntVar->jidProtocol);
}

void RegDatToVmm(void)
{
#if 0
    T_OamsVerNotifyRegReq tDatRegReq;
    JID tJidVmmMgt;
    BYTE ucLoopCnt = 0;
    WORD16 wFuncTypeMaxNum = sizeof(tDatRegReq.wFuncType)/sizeof(tDatRegReq.wFuncType[0]);
    T_CliAppReg   *ptOamCliReg = NULL;
    WORD16        wLoop = 0;

    /*CRDCM00090651 Ҫ�������vmm����ע��*/
    /* ����vmm���̵�JID*/
    memset(&tJidVmmMgt, 0, sizeof(tJidVmmMgt));
    XOS_GetSelfJID(&tJidVmmMgt);
    tJidVmmMgt.dwJno = XOS_ConstructJNO(SCS_VM_MGT, 1);

    memset(&tDatRegReq, 0, sizeof(tDatRegReq));
    XOS_GetSelfJID(&(tDatRegReq.tJid));
    
    tDatRegReq.wVerType = VER_TYPE_SCRIPT;
    
    tDatRegReq.ucVerNum = OAM_MIN(wFuncTypeMaxNum, DATFILE_FUNCTYPE_TOTAL);

    XOS_SysLog(OAM_CFG_LOG_ERROR, 
        "######[RegDatToVmm]: g_ptOamIntVar->ucVerNotifyRegReqCnt = %d\n", g_ptOamIntVar->ucVerNotifyRegReqCnt);

    /* ��һ������50��ƽ̨��DDM dat */
    if (g_ptOamIntVar->ucVerNotifyRegReqCnt == 0)
    {
        for (ucLoopCnt = 0; ucLoopCnt < tDatRegReq.ucVerNum; ucLoopCnt++)
        {
            assert(ucLoopCnt < tDatRegReq.ucVerNum);
            if (ucLoopCnt >= tDatRegReq.ucVerNum)
            { 
                XOS_SysLog(OAM_CFG_LOG_ERROR, 
                    "######[RegDatToVmm]: tDatRegReq.ucVerNum is too small, registe top %u function types\n", ucLoopCnt);
                break;    
            }
            
            tDatRegReq.wFuncType[ucLoopCnt] = DATFILE_FUNCTYPE_CFG_BEGIN + ucLoopCnt;
        }
    }
#if 0    
    /* �ڶ�������50��ƽ̨DDM dat */
    else if (g_ptOamIntVar->ucVerNotifyRegReqCnt == 1)
    {
        for (ucLoopCnt = 0; ucLoopCnt < tDatRegReq.ucVerNum; ucLoopCnt++)
        {
            assert(ucLoopCnt < tDatRegReq.ucVerNum);
            if (ucLoopCnt >= tDatRegReq.ucVerNum)
            { 
                XOS_SysLog(OAM_CFG_LOG_ERROR, 
                    "######[RegDatToVmm]: tDatRegReq.ucVerNum is too small, registe top %u function types\n", ucLoopCnt);
                break;    
            }
            tDatRegReq.wFuncType[ucLoopCnt] = DATFILE_FUNCTYPE_DDM_BEGIN + ucLoopCnt;
        }
    }
#endif    
    /* ����������50��SBC��ҵ���dat */
    else
    {
        /*** 1����ɨ�蹲���ڴ� ***/
        ptOamCliReg = Oam_GetRegTableAndLock();
        if (!ptOamCliReg)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, 
                "######[RegDatToVmm]: failed to create shm which contains rigister table and set lock on it!\r");
            g_ptOamIntVar->ucVerNotifyRegReqCnt = 0;
            return;
        }

        for(ucLoopCnt = 0; ucLoopCnt < OAM_CLI_DAT_SHM_MAX_NUM; ucLoopCnt++)
        {
            if (wLoop >= DATFILE_FUNCTYPE_SPECIAL_NUM)
                break;

            if (ucLoopCnt >= OAM_CLI_DAT_SHM_MAX_NUM)
                break;

            if(OAM_CLI_REG_TYPE_SPECIAL == ptOamCliReg->tCliReg[ucLoopCnt].ucRegType)
            {
                tDatRegReq.wFuncType[wLoop] = ptOamCliReg->tCliReg[ucLoopCnt].dwDatFuncType;
                wLoop++;
            }
        }

        if (!Oam_UnLockRegTable(ptOamCliReg))
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, "######[RegDatToVmm]: failed to unlock shm which contains register table ");
        }
    	
        /*** 2���ٲ鿴ȫ�������� ***/
        for(ucLoopCnt = 0; ucLoopCnt < g_ptOamIntVar->tPlatMpCliAppInfo.dwRegCount; ucLoopCnt++)
        {
            if (wLoop >= DATFILE_FUNCTYPE_SPECIAL_NUM)
                break;

            if (ucLoopCnt >= OAM_CLI_DAT_SHM_MAX_NUM)
                break;

            if(OAM_CLI_REG_TYPE_SPECIAL == g_ptOamIntVar->tPlatMpCliAppInfo.tCliReg[ucLoopCnt].ucRegType)
            {
                tDatRegReq.wFuncType[wLoop] = g_ptOamIntVar->tPlatMpCliAppInfo.tCliReg[ucLoopCnt].dwDatFuncType;
                wLoop++;
            }
        }

        tDatRegReq.ucVerNum = wLoop;
        
        if (tDatRegReq.ucVerNum == 0)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, "######[RegDatToVmm]: No Secpical dat need to register...");
            
            /* ���һ�����û�����󣬽���ȫ�ֱ����û�0 */
            g_ptOamIntVar->ucVerNotifyRegReqCnt = 0;

            return;
        }
        
    }

    XOS_SysLog(OAM_CFG_LOG_NOTICE, "######[RegDatToVmm]: send EV_VER_OAMS_NOTIFYREG_REQ to vmm ...");

    XOS_SendAsynMsg(EV_VER_OAMS_NOTIFYREG_REQ, (BYTE *)&tDatRegReq, sizeof(tDatRegReq), 0, 0, &tJidVmmMgt);

    /*���ö�ʱ���ȴ���Ӧ*/
    Oam_SetRegOamsNotifyTimer();

    return;
#endif
}

/**************************************************************************
* �������ƣ�VOID ExecDatUpdateWaitTimerProc
* ��������������汾���¶�ʱ����ʱ
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/04/12    V1.0    ������      ����
**************************************************************************/
static void ExecDatUpdateWaitTimerProc(void)
{
    Oam_SetDATUpdateWaitTimerInvalid();
    /*����Ƿ������ն˶��Ѿ�û������ִ����*/
    if (Oam_CfgIfNoLinkIsExecutingCmd())
    {
        /*����ڴ������е�dat��Ϣ*/
        OAM_CFGRemoveAllDat();

        /*����ǵ�Ƭ����������汾 ��Ϣ����vmm���»�ȡ*/
        if (!Oam_CfgIfCurVersionIsPC())
        {
            g_ptOamIntVar->wDatSum = 0;
        }
    
        /*��scs��������dat�ļ���Ϣ*/
        g_ptOamIntVar->ucCfgVerInfoGetTimes = 0;
        XOS_SysLog(OAM_CFG_LOG_NOTICE, "begin to request new dat ver info ...");
        ReqAllCfgVerInfo();
    }
    else
    {
        g_ucLinkStateCheckTimes++;
        if(Oam_IfExecutingDatUpdateCmd())/*�汾�л���������ִ��*/
        {
            g_ucLinkStateCheckTimes = 0;
            /*�趨ʱ����ֱ���汾�л�����ִ�����*/
            Oam_SetDATUpdateWaitTimer();
        }
        else if (g_ucLinkStateCheckTimes < OAM_CFG_MAX_LINKSTATE_CHECKTIMES)
        {
            /*�趨ʱ����ֱ�������ն�����ִ�����*/
            Oam_SetDATUpdateWaitTimer();
            XOS_SysLog(OAM_CFG_LOG_NOTICE, 
                               "Check times : %d Some link is executing cmd ...", g_ucLinkStateCheckTimes);
        }
        else
        {
            g_ucLinkStateCheckTimes = 0;
            XOS_SysLog(OAM_CFG_LOG_NOTICE, 
                               "Stop all link executing cmd for updating of dat file");
            Oam_CfgStopAllLinkExcutingCmd();         
            /*Ϊ��ʹ�л�������붼��һ��ͨ����ʱ����һ�ִ����л�*/
            Oam_SetDATUpdateWaitTimer();
        }
    }
}

BOOLEAN Oam_CfgCheckCommOamMsgLength(void)
{
    WORD16 wMsgLen = 0;
    XOS_STATUS dwRtn = XOS_SUCCESS;

    dwRtn = XOS_GetMsgDataLen(PROCTYPE_OAM_CLIMANAGER, &wMsgLen);
    if (XOS_SUCCESS != dwRtn)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, 
            "[Oam_CfgCheckCommOamMsgLength] Execute XOS_GetMsgDataLen func fail! dwRtn = %d\n",dwRtn);
        return FALSE;
    }
    else
    {
        if (wMsgLen == 0)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, 
                "[Oam_CfgCheckCommOamMsgLength] Message length is equal 0!\n");
            return FALSE;
        }
    }

    if (wMsgLen < sizeof(MSG_COMM_OAM) || wMsgLen > MAX_OAMMSG_LEN)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, 
            "[Oam_CfgCheckCommOamMsgLength] Message length is error!");
        return FALSE;
    }
    return TRUE;
}

/**************************************************************************
* �������ƣ�VOID HandleMsgEV_OAM_CFG_CFG
* ������������������ִ�к�Ӧ�÷��ص���Ϣ
* ���ʵı���
* �޸ĵı���
* ���������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/02/12    V1.0    ���      ����
**************************************************************************/
static void HandleMsgEV_OAM_CFG_CFG(WORD32 dwMsgId, LPVOID pMsgPara, BOOLEAN bIsSameEndian)
{
    int iLinkStateType = 0;
    TYPE_LINK_STATE * pLinkState = NULL;    
    BYTE ucIdx = 0;
    
    if (!Oam_CfgCheckCommOamMsgLength())
        return;
	
    /* added by fls on 2009-06-29 (CRDCM00197905 SSC2����������OAM�����ֽ���̶�ΪСβ�ֽ���û�а���OMP���ֽ�����) */
    /*if (Oam_CfgIsNeedInvertOrder())*/
    if (!bIsSameEndian)
    {
        CfgMsgTransByteOder((MSG_COMM_OAM *)pMsgPara);
    }
	
    /* end of add */    
    /* ����Debug��Ϣ */
    {
        MSG_COMM_OAM *ptRecvMsg = (MSG_COMM_OAM *)pMsgPara;
        CHAR ucaBuf[MAX_OAMMSG_LEN] = {0};

        /* �������Ч���ն˺ţ���������ն˷��� */
        if ((0 == ptRecvMsg->SeqNo) && (OAM_INVALID_LINK_CHANNEL == ptRecvMsg->LinkChannel))
        {
            /*debug��Ϣ��������ʾ*/
            ucaBuf[0] = '\n';
            memcpy(ucaBuf+1, ptRecvMsg->Data->Data, ptRecvMsg->Data->Len);

            for (ucIdx = 0; ucIdx < MAX_CLI_VTY_NUMBER; ucIdx++)
            {
                if (gtLinkState[ucIdx].ucCurRunState != pec_MsgHandle_IDLE)
                {
                    gtLinkState[ucIdx].bRecvAsynMsg = TRUE;
                    SendStringToTelnet(&gtLinkState[ucIdx],ucaBuf);
                }
            }
            return;
        }
    }
    
    /*�ж�LinkState����*/
    iLinkStateType = OamCfgGetLinkStateType(pMsgPara);
    pLinkState = NULL;
    if (LINKSTATE_TYPE_ERR == iLinkStateType)
    {
        return;
    }
    else if (LINKSTATE_TYPE_CLI == iLinkStateType)
    {
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

        /*���Ӧ�õĽ�����Ϣ����0, ˵��Ӧ��ǰһ���ǽ�����Ϣ��
        ��û�з���100, ����һ��100%������*/
        if (pLinkState->ucPlanValuePrev > 0)
        {        
            CHAR szPlanStr[MAX_STRING_LEN] = {0};
            CHAR *pPlanStr = szPlanStr;
            BYTE ucPlan = 100;
	
            /* ��������Ϣ���͵�telnet��ʾ */
            XOS_snprintf(pPlanStr, MAX_STRING_LEN, map_szExePlanFmtStr, ucPlan);
            strncat(pPlanStr, "\n\r", MAX_STRING_LEN);
            /*pLinkState->bOutputCmdLine = FALSE;*/
            pLinkState->bOutputMode = OUTPUT_MODE_ROLL;
            pLinkState->wAppMsgReturnCode = SUCC_WAIT_AND_HAVEPARA;
            SendStringToTelnet(pLinkState, pPlanStr);

            pLinkState->bOutputMode = OUTPUT_MODE_NORMAL;
        }
		
        pLinkState->ucPlanValuePrev = 0;

        Oam_RecvFromApp(pLinkState,pMsgPara);
    }
    else if (LINKSTATE_TYPE_XML == iLinkStateType)
    {
  //      RecvFromProtocolToOMM(pMsgPara);
    }
}

/**************************************************************************
* �������ƣ�VOID Oam_IfExecutingDatUpdateCmd
* ������������鵱ǰ�Ƿ�����ִ�а汾�л�
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/04/12    V1.0    ������      ����
**************************************************************************/
static BOOLEAN Oam_IfExecutingDatUpdateCmd(void)
{
    WORD16 wLoop = 0;
    WORD16 wXmlLinkNum = sizeof(gt_XmlLinkState)/sizeof(TYPE_XMLLINKSTATE);
    WORD16 wTelLinkNum = sizeof(gtLinkState)/sizeof(TYPE_LINK_STATE);
    TYPE_XMLLINKSTATE *ptXmlLinkState = NULL;
    TYPE_LINK_STATE *ptTelLinkState = NULL;
    /*�ж�Telnet���Ƿ���ִ�а汾�л�����*/
    for(wLoop = 0;wLoop < wTelLinkNum;wLoop++)
    {     
        ptTelLinkState = &gtLinkState[wLoop];     
        if ((pec_MsgHandle_EXECUTE == Oam_LinkGetCurRunState(ptTelLinkState)) &&
            ((CMD_INSTALL_ACTIVE == Oam_LinkGetCurCmdId(ptTelLinkState)) || (CMD_INSTALL_ACTIVE_APTD == Oam_LinkGetCurCmdId(ptTelLinkState))))
        {
            return TRUE;
        }
    }
    /*�ж�OMM���Ƿ���ִ�а汾�л�����*/
    for(wLoop = 0;wLoop < wXmlLinkNum;wLoop++)
    {
        ptXmlLinkState = &gt_XmlLinkState[wLoop];
        if ((pec_MsgHandle_EXECUTE == XML_LinkGetCurRunState(ptXmlLinkState))&&
            ((CMD_INSTALL_ACTIVE == ptXmlLinkState->dwCmdId) || (CMD_INSTALL_ACTIVE_APTD == ptXmlLinkState->dwCmdId)))
        {
            return TRUE;
        }
    }

    return FALSE;
}

/**************************************************************************
* �������ƣ�VOID Oam_CfgStopAllXmlLinkExcutingCmd
* ����������ֹͣ����������ִ������
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/04/18   V1.0    ������      ����
**************************************************************************/
static void Oam_CfgStopAllXmlLinkExcutingCmd(void)
{
    WORD16 wLoop = 0;
    WORD16 wXmlLinkNum = sizeof(gt_XmlLinkState)/sizeof(TYPE_XMLLINKSTATE);
    for (wLoop = 0;wLoop < wXmlLinkNum;wLoop++)
    {
        Omm_CfgStopExcutingCmd(&gt_XmlLinkState[wLoop]);
    }
}

/**************************************************************************
* �������ƣ�VOID Oam_CfgStopAllTelLinkExcutingCmd
* ����������ֹͣ����Telnet������ִ������
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/04/18   V1.0    ������      ����
**************************************************************************/
static void Oam_CfgStopAllTelLinkExcutingCmd(void)
{
    CHAR *ptrOutputResult = NULL;
    WORD16 wLoop = 0;
    WORD16 wLinkNum = sizeof(gtLinkState)/sizeof(TYPE_LINK_STATE);    
    TYPE_LINK_STATE *pLinkState = NULL;
    for (wLoop = 0; wLoop < wLinkNum; wLoop++)
    {
        pLinkState = &gtLinkState[wLoop];
        if ((Oam_LinkGetCurRunState(pLinkState)  != pec_MsgHandle_TELNET) &&
            (Oam_LinkGetCurRunState(pLinkState)  != pec_MsgHandle_IDLE))
        {
            ptrOutputResult = ReadMapValueOfOamDat(MAP_TYPE_XMLADAPTER,MAP_KEY_STOP_CMDFORUPDATEDAT);
            Oam_CfgStopExcutingCmd(pLinkState, ptrOutputResult);
    /*        Oam_InnerCmdLogout(pLinkState);*/
        }
    }
    /*ֹͣTelnetģ����·�ϴ�������*/
    pLinkState = &gt_SaveLinkState;
    if ((Oam_LinkGetCurRunState(pLinkState)  != pec_MsgHandle_TELNET) &&
            (Oam_LinkGetCurRunState(pLinkState)  != pec_MsgHandle_IDLE))
    {
        Oam_CfgStopExcutingCmd(pLinkState, NULL);
 /*       Oam_InnerCmdLogout(pLinkState);*/
    }
    return;
}

/**************************************************************************
* �������ƣ� Oam_CfgStopAllLinkExcutingCmd
* ����������ֹͣ������ִ������
* ���ʵı���
* �޸ĵı���
* ���������
* ���������
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/02/12    V1.0    ���      ����
**************************************************************************/
void Oam_CfgStopAllLinkExcutingCmd(void)
{
    /*��ֹXML��·�ϵ�ִ������*/
    Oam_CfgStopAllXmlLinkExcutingCmd();
    /*��ֹTelnet��·�ϵ�ִ������*/
    Oam_CfgStopAllTelLinkExcutingCmd();
    return;
}

/*�ϵ�ɹ��������л���work״̬*/
void Oam_CfgPowerOnOverAndToWorkState(void)
{
    /* CRDCM00025960 OMP������ε�������OAM_Interpret�쳣�����Ҵ�ӡ����������ʧ�� */
    if (BOARD_MASTER ==  XOS_GetBoardMSState())
    {
        /*
        �л�״̬��work֮ǰɨ��һ��ע���
        ��ֹ��ʼ״̬�н���ע��û�б�����
        ����һ��ɨ��ʱ���̫��
        */
        Oam_CfgScanRegTable();            
    }    
    else if (BOARD_SLAVE ==  XOS_GetBoardMSState())
    {
        /* �����������û������ļ� */
        XOS_Msend(EV_CFG_USERFILE_REQ, NULL, 0, 0, 0, &g_ptOamIntVar->jidSelf);
        /* ���ö�ʱɨ��DATע����Ϣ�Ĺ����ڴ� */
        XOS_SetLoopTimer(PROCTYPE_OAM_CLIMANAGER, TIMER_REQ_USER_FILE, DURATION_TIMER_REQ_USER_FILE, PARAM_NULL);
    }
    else
    {
    }
        
    XOS_SetNextState(PROCTYPE_OAM_CLIMANAGER, S_Work);
}

/**************************************************************************
* �������ƣ� HandleMasterToSlave
* ���������� ��ת������
* ���ʵı���
* �޸ĵı���
* ���������
* ���������
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/02/12    V1.0    ���      ����
**************************************************************************/
static void HandleMasterToSlave(void)
{
    /*�ͷ�dat��ռ�ڴ�*/ 
    Oam_CfgCloseAllLink(Hdr_szMSChangeoverTip);
    OAM_CFGRemoveAllDat();
    /*����ǵ�Ƭ����������汾 ��Ϣ����vmm���»�ȡ*/
    if (!Oam_CfgIfCurVersionIsPC())
    {
        g_ptOamIntVar->wDatSum = 0;
    }
    /*����ѭ����ʱ����������ʱ�����û���Ϣ�ļ�*/
    XOS_SetLoopTimer(PROCTYPE_OAM_CLIMANAGER, TIMER_REQ_USER_FILE, DURATION_TIMER_REQ_USER_FILE, PARAM_NULL);
    XOS_SetNextState(PROCTYPE_OAM_CLIMANAGER, S_Work);
}

/**************************************************************************
* �������ƣ� HandleSlaveToMaster
* ���������� ��ת������
* ���ʵı���
* �޸ĵı���
* ���������
* ���������
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/02/12    V1.0    ���      ����
**************************************************************************/
static void HandleSlaveToMaster(void)
{   
    WORD16 i = 0;
    
    Oam_CfgCloseAllLink(Hdr_szMSChangeoverTip);

    /* CRDCM00025960 OMP������ε�������OAM_Interpret�쳣�����Ҵ�ӡ����������ʧ�� */
    OAM_CFGRemoveAllDat();
    /*����ǵ�Ƭ����������汾 ��Ϣ����vmm���»�ȡ*/
    if (!Oam_CfgIfCurVersionIsPC())
    {
        g_ptOamIntVar->wDatSum = 0;
    }

    /*����ת���߲�����λ����ֹ�ϴ����쳣����*/
    Oam_CfgHandleMSGOffline2OnlineFinish();

    /* 611001186690 ���¼����û�֮ǰ��������ڴ� */
    memset(gtUserInfo, 0, sizeof(gtUserInfo));
    for (i = 0; i < sizeof(gtUserInfo)/sizeof(TYPE_USER_INFO); i++)
    {
        gtUserInfo[i].ucRole = 0;
        gtUserInfo[i].bLocked = FALSE;
        memset(gtUserInfo[i].szUserName, 0, MAX_OAM_USER_NAME_LEN);
        memset(gtUserInfo[i].szPassWord, 0, MAX_OAM_USER_PWD_ENCRYPT_LEN);
    }
    gnUserNum = 0;

    /* ���¼����û���Ϣ */
    if (!OamLoadUserFile())
    {
        /* ��������û��ļ�ʧ�ܣ�������Ĭ���û���Ĭ�����룬�Է�ֹ���ܵ�¼����� */
        UseDefaultUserInfo();
    }

    /* ���³�ʼ���û���־�ļ� */
    if (!InitUserLogFile())
    {
        XOS_SysLog(OAM_CFG_LOG_NOTICE, "Read user log file failed!\n");
    }

   /* ���¶�ȡ��������־�ļ�������������������������д��־ʱ����Ӱ������ */
    g_ptOamIntVar->wLinesOfCmdLogFile = 0;
    if(!GetCmdLogFileLines())
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[HandleSlaveToMaster] Get lines of log file fail!\n");

    if (Oam_CfgIfCurVersionIsPC())
    {
        /*�����pc���汾��ֱ������ע��*/
        OAM_CliReRegisterAllRegistedJobs();
    }
    else
    {
        /*����ǵ�Ƭ�汾����ת��ʱ�������л�dat�汾һ��������*/
        Oam_SetDATUpdateWaitTimer();
        g_ptOamIntVar->ucCfgVerInfoGetTimes = 0;
        XOS_SetNextState(PROCTYPE_OAM_CLIMANAGER, S_UpdateDAT);
    }

    /* ���Ķ԰����µ���Ϣ */
    OamMateRequestSubScr();
    /*ɱ��ѭ����ʱ�� */
    XOS_KillTimer(PROCTYPE_OAM_CLIMANAGER, TIMER_REQ_USER_FILE, PARAM_NULL);
}

static void HandleMsgEV_OAM_CFG_PLAN(WORD32 dwMsgId, LPVOID pMsgPara, BOOLEAN bIsSameEndian)
{
    /*�ж�LinkState����*/
    int iLinkStateType = LINKSTATE_TYPE_ERR;
    TYPE_LINK_STATE * pLinkState = NULL;
    T_Cmd_Plan_Cfg *pPlanInfo =  NULL;
    BOOL bIsSaveCmd = FALSE;
    WORD16 wRecvMsgLen = 0;

    if( XOS_GetMsgDataLen(PROCTYPE_OAM_CLIMANAGER, &wRecvMsgLen) != XOS_SUCCESS)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "HandleMsgEV_OAM_CFG_PLAN:call XOS_GetMsgDataLen failed! ");
        return;
    }
    else if (wRecvMsgLen < sizeof(T_Cmd_Plan_Cfg))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "HandleMsgEV_OAM_CFG_PLAN:The length of message received is less than the length of interface struct! ");
        return;
    }

    /* �ж��Ƿ��ڴ��� */
    pPlanInfo = (T_Cmd_Plan_Cfg *)pMsgPara;
    if (!bIsSameEndian)
    {
        pPlanInfo->wPlanNo = XOS_InvertWord16(pPlanInfo->wPlanNo);
        pPlanInfo->dwCmdID = XOS_InvertWord32(pPlanInfo->dwCmdID);
        pPlanInfo->dwLinkChannel = XOS_InvertWord32(pPlanInfo->dwLinkChannel);
    }
    if (pPlanInfo->dwCmdID == CMDID_SAVE_PROTOCOL_CFG)
        bIsSaveCmd = TRUE;

    if (!bIsSaveCmd)
    {
        iLinkStateType = OamGetLinkStateTypeForPlan(pMsgPara);
        if (LINKSTATE_TYPE_ERR == iLinkStateType)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, "Error LinkState Type!!!\n");
            return;
        }
    }
    else
    {
        if (Xml_CheckIsExecSaving())/*OMM����ִ�д�������*/
            iLinkStateType = LINKSTATE_TYPE_XML;
        else
            iLinkStateType = LINKSTATE_TYPE_CLI;
    }
    
    if (LINKSTATE_TYPE_CLI == iLinkStateType)
    {
        pLinkState = OamCfgGetLinkState(dwMsgId,pMsgPara);
        if (!pLinkState)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR,
                   "[HandleMsgEV_OAM_CFG_PLAN] Failled to get linkstate! \n");
            return;
        }

        XOS_SysLog(OAM_CFG_LOG_NOTICE, "HandleMsgEV_OAM_CFG_PLAN: Current run state is %d!", Oam_LinkGetCurRunState(pLinkState));

        if (Oam_LinkGetCurRunState(pLinkState) != pec_MsgHandle_EXECUTE)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, "HandleMsgEV_OAM_CFG_PLAN: Current run state isn't Execute! ");
            return;
        }
                
        Oam_RecvPlanInfoFromApp(pLinkState, pMsgPara, bIsSameEndian);
    }
    else
    {
  //      XML_RecvPlanInfoFromApp(pMsgPara, bIsSameEndian);
    }
}

BOOLEAN OtherLinkIsSaving(TYPE_LINK_STATE *pLinkState)
{
    WORD16 wLoop = 0;
    WORD16 wTelLinkNum = 0;
    TYPE_LINK_STATE *ptTelLinkState = NULL;    
    
    wTelLinkNum = sizeof(gtLinkState)/sizeof(TYPE_LINK_STATE);
    
    /*����Ƿ���Telnet����ִ�д��̲���*/
    for(wLoop = 0;wLoop < wTelLinkNum;wLoop++)
    {
        ptTelLinkState = &(gtLinkState[wLoop]); 
        if((pec_MsgHandle_EXECUTE == ptTelLinkState->ucCurRunState) &&
            (CMD_DEF_SET_SAVE == ptTelLinkState->tMatchResult.dExeID) &&
            (pLinkState->ucLinkID != ptTelLinkState->ucLinkID))
        {
            break;
        }
    }
    if (wLoop < wTelLinkNum)
    {        
        XOS_SysLog(OAM_CFG_LOG_DEBUG, "[OtherLinkIsSaving] Other link is executing save command. \n");  
        return TRUE;
    }
    
    return FALSE;
}

void Oam_DbgDelFile(CHAR *pFileName)
{
    XOS_STATUS dwRtn = XOS_SUCCESS;

    dwRtn = XOS_DeleteFile(pFileName);
    if (XOS_SUCCESS != dwRtn)
    {
        printf("@@@ Delete file failed! dwRtn = %d\n", dwRtn);
    }
    else
    {
        printf("@@@ Delete file success!\n");
    }
}

/*lint +e661*/
/*lint +e662*/
#if 0
BOOLEAN SpecialRegistered(VOID)
{
    T_CliAppReg   *ptOamCliReg = NULL;
    BYTE ucLoopCnt = 0;
    BOOLEAN bRet = FALSE;
    
    /*** 1����ɨ�蹲���ڴ� ***/
    ptOamCliReg = Oam_GetRegTableAndLock();
    if (!ptOamCliReg)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "failed to create shm which contains rigister table and set lock on it!\r");
        return FALSE;
    }

    for(ucLoopCnt = 0; ucLoopCnt < OAM_CLI_DAT_SHM_MAX_NUM; ucLoopCnt++)
    {
        if(OAM_CLI_REG_TYPE_SPECIAL == ptOamCliReg->tCliReg[ucLoopCnt].ucRegType)
        {
            bRet = TRUE;
            break;
        }
    }

    if (!Oam_UnLockRegTable(ptOamCliReg))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "failed to unlock shm which contains register table ");
    }

    if (bRet)
        return bRet;
    
    /*** 2���ٲ鿴ȫ�������� ***/
    for(ucLoopCnt = 0; ucLoopCnt < g_ptOamIntVar->tPlatMpCliAppInfo.dwRegCount; ucLoopCnt++)
    {
        if(OAM_CLI_REG_TYPE_SPECIAL == g_ptOamIntVar->tPlatMpCliAppInfo.tCliReg[ucLoopCnt].ucRegType)
        {
            bRet = TRUE;
            break;
        }
    }

    return bRet;
}
#endif
/**************************************************************************
* �������ƣ�VOID Oam_Cli_ChangeScriptLanguage
* �����������ı������нű���������
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2011/01/27    V1.0    ���      ����
**************************************************************************/
void Oam_Cli_ChangeScriptLanguage(LPVOID pMsgPara)
{	
    /*ֹͣ������������ִ�е�����*/
    Oam_CfgStopAllLinkExcutingCmd(); 
    /*��ʱ2��*/
    XOS_Delay(2*1000);
	
    if (Oam_CfgIfNoLinkIsExecutingCmd())
    {
        /*����ڴ������е�dat��Ϣ*/ 
        OAM_CFGRemoveAllDat();
        /*������Ч*/			
       OAM_CliReRegisterAllRegistedJobs();
       /*�л��������ӵ��û�ģʽ*/
       OAM_CliChangeAllLinkToStartMode(map_szScriptReloaded);
    }
    else
    {
        /*���ö�ʱ����ʱ�ظ��л���������*/
        XOS_SetRelativeTimer(PROCTYPE_OAM_CLIMANAGER, TIMER_CLI_LOAD_SCRIPT_NEW_LANG, DURATION_CLI_LOAD_SCRIPT_NEW_LANG, 1);
    }
}

/*��ӡclimanager���Ժ�����Ϣ*/
void CliMHelp(void)
{
    printf("                [Cli manager debug functions table]\n");
    printf("Index,  Function Name,                       Description        \n");
    printf("===================================================================\n");
    printf("1,      OAM_DbgShowCliMgrRegInfo(VOID),      Show all register records on climanager\n");  
    printf("2,      OAM_DbgShowCliLogFuncs(VOID),        Show the functions to set system log print-level of climanager \n");  
    printf("3,      OAM_DbgShowCliAllLink(VOID),         Show all link-infomation on climanager\n");  
    printf("4,      OAM_DbgSendCliMsg(WORD16 dwMsgId),   Send debug message to climanager\n");  
    printf("5,      OAM_DbgShowCliDatVerInfo(VOID),      Show all datfile-information on climanager\n");  
    printf("6,      OAM_DbgShowCliCmdLog(CHAR *pIPAddr), Show all information of executed commands \n");  
    printf("7,      OAM_DbgShowCliSpecIP(VOID),          Show the ip address which specifed for user-unlocking\n");  
    printf("8,      OAM_DbgShowCliCmdTree(VOID),         Show the command tree in memory\n");  
    printf("9,      OAM_DbgOpenCliMsgPrint(VOID),        Open the print-flag of cli messages\n");  
    printf("10,     OAM_DbgCloseCliMsgPrint(VOID),       Close the print-flag of cli messages\n");  
    printf("11,     OAM_DbgShowCliLog(WORD16 wCnt),      Show the logs which record received message nearly\n");  
    printf("12,     OAM_DbgShowCliAllVtyInfo(VOID),      Show all link-information on clivty\n"); 
    printf("===================================================================\n");
}


