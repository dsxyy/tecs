/**************************************************************************
* 版权所有 (C)2008, 深圳市中兴通讯股份有限公司。
*
* 文件名称：climanager.c
* 文件标识：
* 内容摘要：
* 其它说明：
            
* 当前版本：
* 作    者：
* 完成日期：
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
#include "oam_execute.h"
#include "clirecvreg.h"
#include "pub_oam_cfg_event.h"
#include "xmlagent.h"
#include "offline_to_online.h"
#include "oam_cfg_shmlog.h"
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
/*连接状态检查次数，切换版本时候用*/
static BYTE g_ucLinkStateCheckTimes = 0;
#define OAM_CFG_MAX_LINKSTATE_CHECKTIMES    12

/*当前所有dat版本功能类型，和pub_vmm.h中定义一致*/
/*
vmm分配给cli的dat功能类型最多可以有50个,暂时分配配置30个，ddm20个
向vmm注册激活通知和请求版本时候使用
*/
#define DATFILE_FUNCTYPE_CFG_BEGIN     FUNC_SCRIPT_CLIS_OAM
#define DATFILE_FUNCTYPE_CFG_NUM       50
#define DATFILE_FUNCTYPE_DDM_BEGIN     FUNC_SCRIPT_DDM_OAM
#define DATFILE_FUNCTYPE_DDM_NUM       50
#define DATFILE_FUNCTYPE_SPECIAL_NUM   50
#define DATFILE_FUNCTYPE_TOTAL         150

/**************************************************************************
*                          本地变量                                       *
**************************************************************************/
/**************************************************************************
*                          局部函数原型                                   *
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
*                          全局函数声明                                   *
**************************************************************************/

extern void RegDatToVmm(void);
extern void ReqAllCfgVerInfo(void);
extern void Oam_CfgStopAllLinkExcutingCmd(void);

/*extern VOID OamCfgRemoveAllDat(VOID);*/
/**************************************************************************
*                          主入口函数                                     *
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
            /*上电消息*/
        case EV_MASTER_POWER_ON:
        case EV_SLAVE_POWER_ON:
            {
            /* 初始化 */
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
                    /*如果是pc机版本*/
                    /* 上电完毕，回应答 */
                    if (bInitSuccess)
                    {
                        Oam_CfgPowerOnOverAndToWorkState();
                    }
                }
            }

            break;

            /*SCS返回的文件信息，请求消息是在Initialize里面发送的*/
        case EV_OAMS_VERINFO_ACK:
            RecvDatVerInfoAck(pMsgPara);
            break;

            /*获取dat版本信息超时消息*/
        case EV_TIMER_GET_CFGVERINFO:
            RecvDatVerInfoGetTimeOut();
            break;

            /*注册dat切换回应*/
        case EV_VER_OAMS_NOTIFYREG_ACK:
            RecvRegOamsNtfAck(pMsgPara);
            break;

            /*注册dat切换回应超时消息*/
        case EV_TIMER_REGOAMS_NTF_ACK:
            RecvRegOamsNtfTimeOut();
            break;

            /*用户信息主备同步消息*/
        case OAM_MSG_MS_CFGFILE_SYN:
            /*agent用户信息cfg文件主备同步*/
            if (BOARD_SLAVE ==  XOS_GetBoardMSState())
            {/*备板接收*/
                Oam_RcvCfgFileFromMaster(pMsgPara);
                ReadCfgScript();
            }
            else
            {/*主板发送*/
                Oam_SendCfgFileToSlave();
            }
            break;
        case OAM_MSG_TELNET_TO_INTERPRET:/*telnetserver 消息*/
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

            /* 接收来自Telnet的消息 */
            RecvFromTelnet(pLinkState,pMsgPara);
            break;
        /* 收到订阅服务消息 */
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
            /*dat版本切换消息*/
        case EV_OAMS_VER_UPDATE_NOTIFY:
            if (BOARD_MASTER ==  XOS_GetBoardMSState())
            {
                XOS_SysLog(OAM_CFG_LOG_NOTICE, "reveive ver update notify msg, change to UpdateDAT state for the updating of dat ver ...");
               
                /*离线转在线操作复位，等版本切换完以后再处理，如果有的话*/
                Oam_CfgHandleMSGOffline2OnlineFinish();
                
                /*设置定时器，先等待，以免有连续更新的dat文件*/
                Oam_SetDATUpdateWaitTimer();

                g_ptOamIntVar->ucCfgVerInfoGetTimes = 0;
                g_ucLinkStateCheckTimes = 0;
                XOS_SetNextState(PROCTYPE_OAM_CLIMANAGER,S_UpdateDAT);
            }
            break;

        case OAM_MSG_XMLAGNET_TO_INTERPRET:/*来自XML适配器消息*/
        case OAM_MSG_TELNET_TO_INTERPRET:/*telnetserver 消息*/
            /*OAM_MSG_XMLAGNET_TO_INTERPRET 是存盘请求，
            不需要检查消息长度，
            OAM_MSG_TELNET_TO_INTERPRET要检查消息长度*/
            if (OAM_MSG_TELNET_TO_INTERPRET == dwMsgId)
            {
                /*CRDCM00322153 检查收到的消息长度是否错误*/         
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

            /* 接收来自Telnet的消息 */
            RecvFromTelnet(pLinkState,pMsgPara);
            break;

            /*业务的配置回应消息*/
        case EV_OAM_CFG_CFG:
            HandleMsgEV_OAM_CFG_CFG(dwMsgId, pMsgPara, bIsSameEndian);
            break;
#if 0
        case EV_XML_OMMCFG_ACK:/*将业务应答消息发往到OMM*/
            {
                RecvFromOP(pMsgPara);
                break;
            }
        case EV_XML_OMMCFG_REQ:/*将OMM的消息发往到OAM*/
            {
                ReceiveFromOmm(pMsgPara);
                break;
            }
        case EV_XML_CEASECMD_REQ:/*将OMM的命令终止请求发送到OMM*/
            {
                ReceiveCeaseCmdMsgFromOmm(pMsgPara);
                break;
            }
        case EV_XML_REGISTER_REQ:/*网管端XML注册请求*/
            {
                ReceiveRegisterMsgFromOmm(pMsgPara);
                break;
            }
        case EV_XML_UNREGISTER_REQ:/*网管端XML注销请求*/
            {
                ReceiveUnRegisterMsgFromOmm(pMsgPara);
                break;
            }
        case EV_XML_HANDSHAKE_REQ:/*网管端XML握手请求*/
            {
                ReceiveHandShakeMsgFromOmm(pMsgPara);
                break;
            }
        case EV_TIMER_XML_OMM_HANDSHAKE:/*与网管端握手定时器*/
            {
                RecvFromXmlAdapterTimer(NULL, dwMsgId);
                break;
            }
        case EV_TIMER_EXECUTE_XMLADAPTER:/*XML适配器与前台应答定时器*/
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
        case EV_XML_LINKCFG_REQ:/*XML适配器与网管端链路配置请求*/
            {
                ReceiveLinkCfgMsgFromOmm(pMsgPara);
                break;
            }
#endif
            /*业务的txt存盘数据恢复请求消息*/
        case EV_TXTCFG_REQ:
            RecvTxtCfgReq();
            break;
            /* Manager直接收到平台进程注册函数消息 */
        case EV_OAMCLI_APP_REG:
            RecvAgtReg(ptVar, pMsgPara, bIsSameEndian);
            break;
            /* Manager直接收到SBC等业务注册函数消息 */
        case EV_OAMCLI_SPECIAL_REG:
            RecvAgtRegSpecial(ptVar, pMsgPara, bIsSameEndian);
            break;
            /* VMM返回的SBC等业务dat的信息 */
        case EV_OAMS_VERINFO_ACK:
            RecvDatVerInfoAck(pMsgPara);
            break;
            /* Manager直接收到非平台进程注册函数消息 */
        case EV_OAMCLI_APP_REGEX:
            RecvAgtRegEx(ptVar, pMsgPara, bIsSameEndian);
            break;

            /* Manager直接收到平台进程注销函数消息 */
        case EV_OAMCLI_APP_UNREG:
            RecvAgtUnreg(ptVar, pMsgPara, bIsSameEndian);
            break;

            /* Manager收到Agent同步消息处理 */
        case EV_OAMCLI_APP_REG_A2M:
            Oam_CfgRecvRegA2M(ptVar, pMsgPara, bIsSameEndian);
            break;

        case EV_OAMCLI_APP_REGEX_A2M:
            RecvRegExA2M(ptVar, pMsgPara, bIsSameEndian);
            break;
            /* 定时扫描OMP的共享内存信息 */
        case EV_TIMER_SCAN_REGSHM:
            if (BOARD_MASTER ==  XOS_GetBoardMSState())
            {
                Oam_CfgScanRegTable();
            }
            break;
            /*每个连接相关的定时器消息*/
        case EV_TIMER_IDLE_TIMEOUT:
        /* 614000678048 (去掉24小时绝对超时) */
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
            /* 主备链路恢复 消息*/
        case EV_MATE_COMM_OK_OAM:
            if (BOARD_MASTER ==  XOS_GetBoardMSState())
            {
                Oam_SendCfgFileToSlave();
            }
            break;

            /*主备链路断消息*/
        case EV_MATE_COMM_ABORT_OAM:
            break;

            /* 单板与OMP链路断消息 */
        case EV_COMM_ABORT:
            /*case OAM_OMP_SLAVE_TO_MASTER:*/
            XOS_SysLog(OAM_CFG_LOG_NOTICE, "EV_COMM_ABORT msg was received!");
            Oam_CfgCloseAllLink(NULL);
            break;

            /* 单板与OMP链路恢复消息 */
        case EV_COMM_OK:
            break;
            /*用户信息主备同步消息*/
        case OAM_MSG_MS_CFGFILE_SYN:
            /*agent用户信息cfg文件主备同步*/
            if (BOARD_SLAVE ==  XOS_GetBoardMSState())
            {/*备板接收*/
                Oam_RcvCfgFileFromMaster(pMsgPara);
                ReadCfgScript();
            }
            else
            {/*主板发送*/
                Oam_SendCfgFileToSlave();
            }
            break;

            /* 主转备消息*/
        case EV_MASTER_TO_SLAVE:
            HandleMasterToSlave();
            break;

            /* 备转主消息*/
        case EV_SLAVE_TO_MASTER:
            HandleSlaveToMaster();
            break;

            /*注册dat切换回应*/
        case EV_VER_OAMS_NOTIFYREG_ACK:
            RecvRegOamsNtfAck(pMsgPara);
            break;

            /*注册dat切换回应超时消息*/
        case EV_TIMER_REGOAMS_NTF_ACK:
            RecvRegOamsNtfTimeOut();
            break;
        case MSG_OFFLINE_CFG_TO_INTERPRET:
            Oam_CfgHandleMSGOffline2Online(pLinkState, pMsgPara);
            break;
        case OAM_CFG_FINISH_ACK:
            Oam_CfgHandleMSGOffline2OnlineFinish();
            break;
        /* 主板接收到备板发来的用户文件同步请求消息 */
        case EV_CFG_USERFILE_REQ:
            if (BOARD_MASTER ==  XOS_GetBoardMSState())
            {
                if(!OamSendUserfileToSlave())
                {
                    XOS_SysLog(OAM_CFG_LOG_ERROR, "[MASTER] OamSendUserfileToSlave exec failed!\n");
                }
            }
            break;
        /* 备板接收到主板的用户文件请求应答 */
        case EV_CFG_USERFILE_ACK:
            if (BOARD_SLAVE ==  XOS_GetBoardMSState())
            {
                if(!OamRecvUserFileFromMaster(pMsgPara))
                {
                    XOS_SysLog(OAM_CFG_LOG_ERROR, "[SLAVE] OamRecvUserFileFromMaster exec failed!\n");
                }
            }
            break;
        /* 收到应用返回的进度信息 */
        case EV_OAM_CFG_PLAN:
            HandleMsgEV_OAM_CFG_PLAN(dwMsgId, pMsgPara, bIsSameEndian);
            break;
        /* 等待应用返回进度信息，5s超时 */
        case EV_TIMER_CMD_PLAN:
        /*611001634189 VMM模式下升级IPMC，升级进度界面显示混乱*/
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
        /* 等待应用返回进度信息，5s超时，XML通道使用 */
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
        /* 收到订阅服务消息 */
        case EV_SUBSCR_EVENT_TOAPP:
            OamRecvMateSubScr(pMsgPara);
            break;
        case EV_TIMER_REQ_USER_FILE:
            if (BOARD_SLAVE ==  XOS_GetBoardMSState())
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR, "###### Request user file from master timeout!\n");

                /* 继续向主板请求用户配置文件 */
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
    
            /* 收到存盘结果之后，杀掉等待进度信息的定时器 */
            Oam_KillCmdPlanTimer(pLinkState);
            Oam_KillExeTimer(pLinkState);
            /*字节序装换要实现*/
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
			
        /*如果应用的进度信息不是0, 说明应用前一步是进度信息，
        且没有返回100, 补充一个100%并换行*/
        if (pLinkState->ucPlanValuePrev > 0)
        {        
            CHAR szPlanStr[MAX_STRING_LEN] = {0};
            CHAR *pPlanStr = szPlanStr;
            BYTE ucPlan = 100;
	
            /* 将进度信息发送到telnet显示 */
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
                pOamMsgBufTmp->LinkChannel = wLoop | 0x200;
	     }
            Oam_RecvFromCliApp(dwMsgId,(LPVOID)ptRtnMsg);	
	     break;										 	
         }			
        /* CLI脚本语言类型发生了变化*/
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
            /* dat版本切换消息*/
        case EV_OAMS_VER_UPDATE_NOTIFY:
            XOS_SysLog(OAM_CFG_LOG_NOTICE, "reveive new ver update notify msg, reset update wait timer");
            /*设置定时器，先等待，以免有连续更新的dat文件*/
            Oam_KillDATUpdateWaitTimer();
            Oam_SetDATUpdateWaitTimer();
            break;

            /* 更新版本定时器消息(收到切换消息之后开始等待)*/
        case EV_TIMER_DAT_UPDATE_WAIT:
            ExecDatUpdateWaitTimerProc();
            break;

            /* dat版本信息消息*/
        case EV_OAMS_VERINFO_ACK:
            RecvDatVerInfoAck(pMsgPara);
            break;
        case EV_TIMER_GET_CFGVERINFO:
            RecvDatVerInfoGetTimeOut();
            break;

        case OAM_MSG_TELNET_TO_INTERPRET:/* telnet消息*/
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
                /* 接收来自Telnet的消息 */
                RecvFromTelnet(pLinkState,pMsgPara);
            }
            else
            {/* 接收来自Telnet的消息 */
                TYPE_TELNET_MSGHEAD *ptMsg = (TYPE_TELNET_MSGHEAD *)pMsgPara;
                /*如果是多包数据接收情况下请求数据或者接收到退出消息的，可以处理*/
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

            /*注册dat切换回应*/
        case EV_VER_OAMS_NOTIFYREG_ACK:
            RecvRegOamsNtfAck(pMsgPara);
            break;

            /*注册dat切换回应超时消息*/
        case EV_TIMER_REGOAMS_NTF_ACK:
            RecvRegOamsNtfTimeOut();
            break;
            
        /*业务的配置回应消息*/
        case EV_OAM_CFG_CFG:
            HandleMsgEV_OAM_CFG_CFG(dwMsgId, pMsgPara, bIsSameEndian);
            break;
#if 0
        case EV_XML_OMMCFG_ACK:/*将业务应答消息发往到OMM*/
            {
                RecvFromOP(pMsgPara);
                break;
            }
        case EV_XML_OMMCFG_REQ:/*将OMM的消息发往到OAM*/
            {
                ReceiveFromOmm(pMsgPara);
                break;
            }
        case EV_XML_CEASECMD_REQ:/*将OMM的命令终止请求发送到OMM*/
            {
                ReceiveCeaseCmdMsgFromOmm(pMsgPara);
                break;
            }
        case EV_XML_REGISTER_REQ:/*网管端XML注册请求*/
            {
                ReceiveRegisterMsgFromOmm(pMsgPara);
                break;
            }
        case EV_XML_UNREGISTER_REQ:/*网管端XML注销请求*/
            {
                ReceiveUnRegisterMsgFromOmm(pMsgPara);
                break;
            }
        case EV_XML_HANDSHAKE_REQ:/*网管端XML握手请求*/
            {
                ReceiveHandShakeMsgFromOmm(pMsgPara);
                break;
            }
        case EV_XML_LINKCFG_REQ:/*XML适配器与网管端链路配置请求*/
            {
                ReceiveLinkCfgMsgFromOmm(pMsgPara);
                break;
            }
        case EV_TIMER_XML_OMM_HANDSHAKE:/*与网管端握手定时器*/
            {
                RecvFromXmlAdapterTimer(NULL, dwMsgId);
                break;
            }
        case EV_TIMER_EXECUTE_XMLADAPTER:/*XML适配器与前台应答定时器*/
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
        /* 收到订阅服务消息 */
        case EV_SUBSCR_EVENT_TOAPP:
            OamRecvMateSubScr(pMsgPara);
            break;
        /* 主转备消息*/
        case EV_MASTER_TO_SLAVE:
            HandleMasterToSlave();
            break;
        default :
            break;
        }
        break;
    #if 0 /*去除存盘状态，存盘命令从work状态完成*/
    case S_CfgSave:
        switch(dwMsgId)
        {
        /* 存盘开始消息 */
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

            /*发送linkstate里面保存的存盘消息*/
            Oam_ExecSaveCmd(pLinkState);
            break;
            /* telnet消息消息*/
        case OAM_MSG_TELNET_TO_INTERPRET:
            pLinkState = OamCfgGetLinkState(dwMsgId,pMsgPara);
            if (!pLinkState)
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR,
                           "[Oam_Interpret] Failled to get linkstate while processing telnet msg");
                /*XOS_SetNextState(S_Work);*/
                break;
            }
            /*检查是否有链路正在存盘*/
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
            /* 接收来自DBS的消息 */
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

                /* 收到存盘结果之后，杀掉等待进度信息的定时器 */
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
        /* 收到应用返回的进度信息 */
        case EV_OAM_CFG_PLAN:
            HandleMsgEV_OAM_CFG_PLAN(dwMsgId, pMsgPara, bIsSameEndian);
            break;
        /* 等待应用返回进度信息，5s超时 */
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
            /* 业务恢复的消息*/
        case EV_OAM_CFG_CFG:
            /*该函数会调用Oam_SaveCfgProcess，存盘成功或者中间失败都会转到S_Work状态*/
            HandleMsgEV_OAM_CFG_CFG(dwMsgId, pMsgPara, bIsSameEndian);
            break;
        case EV_XML_OMMCFG_ACK:/*将业务应答消息发往到OMM*/
            {
                RecvFromOP(pMsgPara);
                break;
            }
        case EV_XML_OMMCFG_REQ:/*将OMM的消息发往到OAM*/
            {
                ReceiveFromOmm(pMsgPara);
                break;
            }
        case EV_XML_CEASECMD_REQ:/*将OMM的命令终止请求发送到OMM*/
            {
                ReceiveCeaseCmdMsgFromOmm(pMsgPara);
                break;
            }
        case EV_XML_REGISTER_REQ:/*网管端XML注册请求*/
            {
                ReceiveRegisterMsgFromOmm(pMsgPara);
                break;
            }
        case EV_XML_UNREGISTER_REQ:/*网管端XML注销请求*/
            {
                ReceiveUnRegisterMsgFromOmm(pMsgPara);
                break;
            }
        case EV_XML_HANDSHAKE_REQ:/*网管端XML握手请求*/
            {
                ReceiveHandShakeMsgFromOmm(pMsgPara);
                break;
            }        
        case EV_TIMER_XML_OMM_HANDSHAKE:/*与网管端握手定时器*/
            {
                RecvFromXmlAdapterTimer(NULL, dwMsgId);
                break;
            }
        case EV_TIMER_EXECUTE_XMLADAPTER:/*XML适配器与前台应答定时器*/
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
        case EV_XML_LINKCFG_REQ:/*XML适配器与网管端链路配置请求*/
            {
                ReceiveLinkCfgMsgFromOmm(pMsgPara);
                break;
            }
            /*执行定时器消息消息*/
        case EV_TIMER_EXECUTE_NORMAL:
            pLinkState = OamCfgGetLinkState(dwMsgId,pMsgPara);
            if (!pLinkState)
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR,
                           "failled to get linkstate while processing application msg, vtyno in msg is %d",
                           ((MSG_COMM_OAM *)pMsgPara)->LinkChannel);
                break;
            }

            /* 接收来自Timer的消息 */
            RecvFromTimer(pLinkState,dwMsgId);
            XOS_SetNextState(S_Work);
            break;
        case MSG_OFFLINE_CFG_TO_INTERPRET:
            Oam_CfgHandleMSGOffline2Online(pLinkState, pMsgPara);
            break;
        case OAM_CFG_FINISH_ACK:
            Oam_CfgHandleMSGOffline2OnlineFinish();
            break;

        /* 主转备消息*/
        case EV_MASTER_TO_SLAVE:
            HandleMasterToSlave();
            break;
            
        /* 备转主消息*/
        case EV_SLAVE_TO_MASTER:
            HandleSlaveToMaster();
            break;
        /* 收到订阅服务消息 */
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
        case EV_TIMER_EXECUTE_NORMAL:  /* added by fls on 2009-02-27 (CRDCR00481329 rpu重启之后,经常出现在线配置进入不了) */
            RecvFromRestore(dwMsgId, pMsgPara, bIsSameEndian);
            break;

            /*注册dat切换回应*/
        case EV_VER_OAMS_NOTIFYREG_ACK:
            RecvRegOamsNtfAck(pMsgPara);
            break;

            /*注册dat切换回应超时消息*/
        case EV_TIMER_REGOAMS_NTF_ACK:
            RecvRegOamsNtfTimeOut();
            break;

        /* 主转备消息*/
        case EV_MASTER_TO_SLAVE:
            HandleMasterToSlave();
            break;
            
        /* 备转主消息*/
        case EV_SLAVE_TO_MASTER:
            HandleSlaveToMaster();
            break;
        /* 收到订阅服务消息 */
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
*                          局部函数实现                                   *
**************************************************************************/

/**************************************************************************
* 函数名称：VOID ReqAllCfgVerInfo
* 功能描述：向scs请求全部dat版本文件信息
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
void ReqAllCfgVerInfo(void)
{
#if 0
    T_OamsVerInfoReq tCfgVerInfoReq;
    JID tJidVmmMgt;
    BYTE ucLoopCnt = 0;
    WORD16 wFuncTypeMaxNum = sizeof(tCfgVerInfoReq.wFuncType)/sizeof(tCfgVerInfoReq.wFuncType[0]);
    T_CliAppReg   *ptOamCliReg = NULL;
    WORD16        wLoop = 0;

    /* 构造vmm进程的JID*/
    memset(&tJidVmmMgt, 0, sizeof(tJidVmmMgt));
    XOS_GetSelfJID(&tJidVmmMgt);    
    tJidVmmMgt.dwJno = XOS_ConstructJNO(SCS_VM_MGT, 1);

    memset(&tCfgVerInfoReq, 0, sizeof(tCfgVerInfoReq));
    tCfgVerInfoReq.wVerType = VER_TYPE_SCRIPT;
    tCfgVerInfoReq.ucNewOrOld =  OAMS_VER_NEW;
    
    tCfgVerInfoReq.ucVerNum = OAM_MIN(wFuncTypeMaxNum, DATFILE_FUNCTYPE_TOTAL);

    XOS_SysLog(OAM_CFG_LOG_ERROR, 
        "######[ReqAllCfgVerInfo]: g_ptOamIntVar->ucVerInfoReqCnt = %d\n", g_ptOamIntVar->ucVerInfoReqCnt);

    /* 第一次请求50个平台非DDM dat */
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
    /* 第二次请求50个平台DDM dat */
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
    /* 第三次请求50个SBC等业务的dat */
    else
    {
        /******** 填充SBC等业务dat的功能类型 ********/
        /*** 1、先扫描共享内存 ***/
        ptOamCliReg = Oam_GetRegTableAndLock();
        if (!ptOamCliReg)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, 
                "######[ReqAllCfgVerInfo]: failed to create shm which contains rigister table and set lock on it!\r");

            /* 异常发生时，将请求次数清0 */
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

        /*** 2、再查看全局数据区 ***/
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
               
            /* 最后一次如果无请求，则将请求次数清0 */
            g_ptOamIntVar->ucVerInfoReqCnt = 0;
            return;
        }

    }

    /*向scs请求全部dat版本文件信息*/
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
    
    /*如果目录不存在则创建目录*/
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

    /* 将cli目录设为NFS共享 
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
* 函数名称：VOID Initialize
* 功能描述：检查函数指针型入参
* 访问的表：无
* 修改的表：无
* 输入参数：T_OAM_INTERPRET_VAR *ptVar
            BOOL isMaster
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
static BOOL Initialize(T_OAM_INTERPRET_VAR *ptVar, BOOL isMaster)
{
    INT32 iReadCfgResult = READERR_SUCCESS;
    WORD16 vty = 0;
    WORD16 i = 0;

//    XOS_TestVarSpace(sizeof(T_OAM_INTERPRET_VAR));

    g_ptOamIntVar = ptVar;
    memset(g_ptOamIntVar, 0, sizeof(T_OAM_INTERPRET_VAR));
    
    /* 构造自身 JID */
    XOS_GetSelfJID(&(g_ptOamIntVar->jidSelf));
    g_ptOamIntVar->jidSelf.dwJno = PROCTYPE_OAM_CLIMANAGER;

    /* 构造Telnet JID */
    XOS_GetSelfJID(&(g_ptOamIntVar->jidTelnet));
    g_ptOamIntVar->jidTelnet.dwJno = PROCTYPE_OAM_TELNETSERVER;
  //  g_ptOamIntVar->jidTelnet.dwJno = XOS_ConstructJNO(PROCTYPE_OAM_TELNETSERVER, 1);

    /*?? 下面是恢复时候用的，v4应该去掉这样的全局变量方式*/
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
    
    /*?? 初始化所有全局linkstate成员的输出空间，原来在登录时候初始化，
    但是不支持在线配置的网元登录处理前就需要输出*/
    memset(gtLinkState, 0, sizeof(gtLinkState));
    for (vty = MIN_CLI_VTY_NUMBER; vty <= sizeof(gtLinkState)/sizeof(TYPE_LINK_STATE); vty++)
    {
        gtLinkState[vty-MIN_CLI_VTY_NUMBER].sOutputResult = g_ptOamIntVar->sOutputResultBuf;
        gtLinkState[vty-MIN_CLI_VTY_NUMBER].sCmdLine = g_ptOamIntVar->sCmdLineBuf;
        /* 614000678048 (去掉24小时绝对超时) */
        /*gtLinkState[vty-MIN_CLI_VTY_NUMBER].wAbsTimer = INVALID_TIMER_ID;*/
        gtLinkState[vty-MIN_CLI_VTY_NUMBER].wExeTimer = INVALID_TIMER_ID;
        gtLinkState[vty-MIN_CLI_VTY_NUMBER].wIdleTimer = INVALID_TIMER_ID;
        gtLinkState[vty-MIN_CLI_VTY_NUMBER].bOutputMode = OUTPUT_MODE_NORMAL;
        gtLinkState[vty-MIN_CLI_VTY_NUMBER].ucCurRunState = pec_MsgHandle_IDLE;
    }
    for (vty = 0;vty < sizeof(g_ptOamIntVar->pMemBufOri)/sizeof(g_ptOamIntVar->pMemBufOri[0]);vty++)
    {
        /*为每条链路分配内存空间*/
        g_ptOamIntVar->pMemBufOri[vty] = (CHAR *)XOS_GetUB(MAX_MEM_BUF);
        /*申请空间失败*/
        if (NULL == g_ptOamIntVar->pMemBufOri[vty])
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR,
					"....................FUNC: %s,Line:%d XOS_GetUB Error!....................",__func__,__LINE__);
            return FALSE;
        }
        /*初始化内存*/
        memset(g_ptOamIntVar->pMemBufOri[vty],0,sizeof(MAX_MEM_BUF));
    }
    /*初始化离线转在线专用linkstate*/
    memset(&g_tLinkStateForOfflineToOnline, 0, sizeof(g_tLinkStateForOfflineToOnline));
    g_tLinkStateForOfflineToOnline.sOutputResult = g_ptOamIntVar->sOutputResultBuf;
    g_tLinkStateForOfflineToOnline.sCmdLine  = g_ptOamIntVar->sCmdLineBuf;

    /*初始化xml通道存盘专用linkstate*/
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

    /*XML适配器处理*/
    Xml_InitLinkState();    

    /*初始化omp上注册用的共享内存*/
    if (OamCliMgtInitShm() == OAM_CLI_FAIL)
    {
       XOS_SysLog(OAM_CFG_LOG_ERROR, "there are some errors in the processing of OamCliMgtInitShm");
    }

    /* 初始化OAM目录 */
    CreateOamDir();

    /* 加载用户信息 */
    if (!OamLoadUserFile())
    {
        /* 如果加载用户文件失败，则启用默认用户和默认密码，以防止不能登录的情况 */
        UseDefaultUserInfo();
    }

    /*读取用户信息配置脚本文件*/
    iReadCfgResult = ReadCfgScript();
    if (iReadCfgResult != READERR_SUCCESS)
    {
        /*使用默认信息*/
        ClearCfgScript();
        XOS_SysLog(OAM_CFG_LOG_NOTICE, "No user config info was got, use default user info");
    }

    if (!InitUserLogFile())
    {
        XOS_SysLog(OAM_CFG_LOG_NOTICE, "Read user log file failed!\n");
    }

    /* 上电的时候读取命令行日志文件，计算出具体的行数，以免在写日志时计算影响性能 */
    g_ptOamIntVar->wLinesOfCmdLogFile = 0;
    if(!GetCmdLogFileLines())
        XOS_SysLog(OAM_CFG_LOG_ERROR, "Get lines of log file fail!\n");

    /* 订阅备板上下电消息 */
    if(isMaster)
    {
        OamMateRequestSubScr();
    }

    g_ptOamIntVar->ucRegOamsNtfTimes = 0;

    /*
        根据文件初始化内存:
        pc机版本dat文件信息在配置文件中，可以直接读取使用
        刀片版本要先从vmm获取dat版本信息
    */
    if (Oam_CfgIfCurVersionIsPC())
    {
        if (GetScriptFileList(g_aucScriptPath) == FALSE)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, "Failed to create script configure file");
            return FALSE;
        }
		
        /*从配置文件读取dat版本信息到内存*/
        Oam_CfgGetDatVerInfoFromIniFile();
        /*如果是pc机版本，直接根据配置文件初始化dat内存*/
        if (!OAM_CFGInitGlobalDatMem())
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, "Failed to initialize global dat memory! all jobs can't register");
            return FALSE;
        }
    }
    else
    {
        RegDatToVmm();
        /*刀片版本，主板的话，向vmm请求版本信息*/
        if(isMaster)
        {
            /*如果是从vmm获取dat信息，向scs请求所有dat文件信息*/
            g_ptOamIntVar->ucCfgVerInfoGetTimes = 0;
            XOS_SysLog(OAM_CFG_LOG_NOTICE, "Begin to get all dat ver info from vmm ...");
            ReqAllCfgVerInfo();
        }
    }

    /*注册解释自用的dat
    if(OamCliRegFunc(g_ptOamIntVar->jidSelf, OAM_CFG_DAT_FUNCTYPE_OAM) != OAM_CLI_SUCCESS)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "Dat file of OAM registe failed! can not use OAM config shell");
    }
*/
    OAM_CliReRegisterAllRegistedJobs();
    
    /* 设置定时扫描DAT注册信息的共享内存 */
    XOS_SetLoopTimer(PROCTYPE_OAM_CLIMANAGER, TIMER_SCAN_REGSHM, DURATION_SCAN_REGSHM, PARAM_NULL);

    /*如果是备板,请求用户配置文件主备同步*/
    if (!isMaster)
    {
        /*~ 增加版本号和优先级 */
        XOS_Msend(OAM_MSG_MS_CFGFILE_SYN, NULL, 0, 0, 0, &g_ptOamIntVar->jidSelf);
        Oam_CfgPowerOnOverAndToWorkState();
    }
    
    /*向oammanager注册调试函数帮助*/
    //OamRegDbgFunc("CliMHelp()","Show all debug functions of CliManager.");
	
    return TRUE;
}


/**********************************************************************
* 函数名称：BYTE  OamCliMgtInitShm
* 功能描述：Manager上电时初使化共享内存，只能主板调用
*                          备转主也同时需要调用
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：OAM_CLI_SUCCESS(0)：成功
*           OAM_CLI_FAIL(1)：   失败
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* 2008-7-28             吴树高
* ---------------------------------------------------------------------
* 2008-7-28             吴树高      创建
************************************************************************/
BYTE  OamCliMgtInitShm(void)
{
#if 0
    WORD16        wLoop = 0;
    T_CliAppReg   *ptOamCliReg = NULL;

    /*必需在使用后调用Oam_UnLockRegTable释放锁*/
    ptOamCliReg = Oam_GetRegTableAndLock();
    if (!ptOamCliReg)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "failed to create shm which contains rigister table and set lock on it!\r");
        return OAM_CLI_FAIL;
    }

    /* 先清除读取的flag标记 */
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
@brief 显示CLIMANAGER上应用的注册信息，用于定位用户命令找不到的情况
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
\nOMP: RegCount = 6
DatFuncType = 1, Jno = 136052737, DAT readflag = 1
DatFuncType = 2, Jno = 202113025, DAT readflag = 1
DatFuncType = 6, Jno = 143458305, DAT readflag = 1
DatFuncType = 421, Jno = 144769025, DAT readflag = 1
DatFuncType = 403, Jno = 144769025, DAT readflag = 1
DatFuncType = 402, Jno = 144769025, DAT readflag = 1

Other MP: RegCount = 0

@li @b 修改记录：
-#  ： 
*/
/*****************************************************************************/
void OAM_DbgShowCliMgrRegInfo(void)
{
#if 0
    WORD16          wLoop = 0;
    T_CliAppReg     *tOamCliReg = NULL;
    T_CliAppAgtRegEx *ptOamCliExReg = NULL;

    /* 同步平台应用注册的信息 */
    /*必需在使用后调用Oam_UnLockRegTable释放锁*/
    tOamCliReg = Oam_GetRegTableAndLock();
    if (!tOamCliReg)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "failed to create shm which contains rigister table and set lock on it!\r");
        return;
    }


    /* 向Manager发送注册过的信息 */
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

    /*打印非平台注册信息*/
    /*必需在使用后调用Oam_UnLockRegExTable释放锁*/
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
@brief  打印CLI相关的几个模块调试信息打印基本设置函数名，用于查看系统日志打印
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
\n1、打印出来的是tulip的调试函数，需要把打印结果拷贝在ushell执行
2、目前打印的有climanager、telnetserver、cmmagent几个job的打印设置函数
@li @b 输出示例：
\nXOS_DbgSetSlJobLevel(0x81c,7)
XOS_DbgSetSlJobLevel(0x81d,7)
XOS_DbgSetSlJobLevel(0x81f,7)
XOS_DbgSetSlSysLevel(7)
XOS_DbgSetSlDirLevel(3,7)
@li @b 修改记录：
-#  ： 
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
* 函数名称：VOID UpdateDatFilesAndReg
* 功能描述：版本切换后，根据注册表重新注册所有job
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-8-18             殷浩      创建
************************************************************************/
void OAM_CliReRegisterAllRegistedJobs(void)
{
//    WORD32          dwRegNum = 0;
 //   T_CliAppReg     *ptPriData = NULL;
    WORD16          wLoop = 0;

    /*重新分配初始化全部dat内存*/
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
    /*设置共享内存中所有job都没有注册*/
    OamCliMgtInitShm();

    /*设置数据区中保存的注册job 为没有注册*/
    dwRegNum  = g_ptOamIntVar->tPlatMpCliAppInfo.dwRegCount;
    ptPriData = &g_ptOamIntVar->tPlatMpCliAppInfo;
    for(wLoop=0; wLoop<dwRegNum; wLoop++)
    {
        if(wLoop >= OAM_CLI_DAT_SHM_MAX_NUM)
        {
            break;
        }

        /*标记注册标志为0*/
        ptPriData->tCliReg[wLoop].ucReadFlag = OAM_CLI_REG_NOT_READED;
    }

    /*扫描注册表，对未注册的进行注册*/
    Oam_CfgScanRegTable();    
#endif
}

void SetSaveTypeAsTxt(const JID jid)
{
#if 0
    WORD16          wLoop = 0;
    T_CliAppReg     *tOamCliReg = NULL;

    /* 同步平台应用注册的信息 */
    /*必需在使用后调用Oam_UnLockRegTable释放锁*/
    tOamCliReg = Oam_GetRegTableAndLock();
    if (!tOamCliReg)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "failed to create shm which contains rigister table and set lock on it!\r");
        return;
    }

    /* 向Manager发送注册过的信息 */
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

    /*全局数据区*/
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
* 函数名称：VOID RecvDatVerInfoAck
* 功能描述：处理vmm返回的dat版本信息
* 访问的表：无
* 修改的表：无
* 输入参数：pMsgPara -- vmm返回的数据
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2008/10/15    V1.0    殷浩      创建
**************************************************************************/
void RecvDatVerInfoAck(LPVOID pMsgPara)
{
#if 0
    T_OamsVerInfoAck *pVerAck =  NULL;

    /*CRDCM00322153 检查收到的消息长度是否错误*/
    if (!Oam_CfgCheckRecvMsgMinLen(sizeof(T_OamsVerInfoAck)))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, 
            "######[RecvDatVerInfoAck]: The length of message received is less than the length of interface struct! ");

        /* 异常情况时，需要将请求次数清0 */
        g_ptOamIntVar->ucVerInfoReqCnt = 0;
        return;
    }
    
    pVerAck = (T_OamsVerInfoAck *)pMsgPara;
    
    /* 杀掉定时器 */
    Oam_KillGetCfgVerInfoTimer();
    g_ptOamIntVar->ucCfgVerInfoGetTimes = 0;

    /* 每次请求之后，在接收到应答的地方将请求次数加1 */    
    g_ptOamIntVar->ucVerInfoReqCnt++;

    XOS_SysLog(OAM_CFG_LOG_ERROR, 
        "######[RecvDatVerInfoAck]: receive dat ver info , %d  records in total, g_ptOamIntVar->ucVerInfoReqCnt = %d\n", 
        pVerAck->wRecordsNum,
        g_ptOamIntVar->ucVerInfoReqCnt);

    /* 如果第二次返回应答之后，总的dat个数为0，继续请求 */
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

    /* 第一次请求之后的返回，校验返回数据中版本功能类型的范围 */
    if ((1 == g_ptOamIntVar->ucVerInfoReqCnt) && (pVerAck->wRecordsNum > 0))
    {
        /* 第一次请求的是非DDM脚本，功能类型范围应该在1-50，其他的丢弃 */
        if ((pVerAck->tOamsVerInfo[0].wFuncType < DATFILE_FUNCTYPE_CFG_BEGIN) || 
            (pVerAck->tOamsVerInfo[0].wFuncType > (DATFILE_FUNCTYPE_CFG_BEGIN + DATFILE_FUNCTYPE_CFG_NUM - 1)))
         {   
         	  g_ptOamIntVar->ucVerInfoReqCnt--;
            return;
         }
    }

    /* 第二次请求之后的返回，校验返回数据中版本功能类型的范围 */
    if ((2 == g_ptOamIntVar->ucVerInfoReqCnt) && (pVerAck->wRecordsNum > 0))
    {
        /* 第二次请求的是DDM脚本，功能类型范围应该在401-450，其他的丢弃 */
        if ((pVerAck->tOamsVerInfo[0].wFuncType < DATFILE_FUNCTYPE_DDM_BEGIN) || 
            (pVerAck->tOamsVerInfo[0].wFuncType > (DATFILE_FUNCTYPE_DDM_BEGIN + DATFILE_FUNCTYPE_DDM_NUM - 1)))
        {   
         	  g_ptOamIntVar->ucVerInfoReqCnt--;
            return;
        }
    }

    /* 第三次请求之后的返回，校验返回数据中版本功能类型的范围 */
    if ((3 == g_ptOamIntVar->ucVerInfoReqCnt) && (pVerAck->wRecordsNum > 0))
    {
        /* 第三次请求的是SBC脚本，功能类型范围不定，但不在DDM和非DDM范围内，其他的丢弃 */
        if (((pVerAck->tOamsVerInfo[0].wFuncType >= DATFILE_FUNCTYPE_CFG_BEGIN) && 
              (pVerAck->tOamsVerInfo[0].wFuncType <= (DATFILE_FUNCTYPE_CFG_BEGIN + DATFILE_FUNCTYPE_CFG_NUM - 1))) || 
            ((pVerAck->tOamsVerInfo[0].wFuncType >= DATFILE_FUNCTYPE_DDM_BEGIN) && 
              (pVerAck->tOamsVerInfo[0].wFuncType <= (DATFILE_FUNCTYPE_DDM_BEGIN + DATFILE_FUNCTYPE_DDM_NUM - 1))))
         {   
         	  g_ptOamIntVar->ucVerInfoReqCnt--;
            return;
         }
    }

    /*请求新的dat信息*/
    if (pVerAck->ucNewOrOld == OAMS_VER_NEW)
    {
        /* 第一次应答，需要将全局变量清空 */
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

            /* 总数不能超过或等于最大值 */
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
            /* 总数不能超过或等于最大值 */
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
            /* 两种情况:1、第二次请求之后的应答，且没有SBC等业务的dat请求了; 2、有SBC等业务的请求 */
            if (((g_ptOamIntVar->ucVerInfoReqCnt == 2) && (!SpecialRegistered())) || 
                (g_ptOamIntVar->ucVerInfoReqCnt == 3))
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR, "######[RecvDatVerInfoAck]: initial memory for dat files and register oam dat file ...");
                if (!OAM_CFGInitGlobalDatMem())
                {
                    /*?? 初始化dat相关全局空间失败，需要添加告警*/
                    XOS_SysLog(OAM_CFG_LOG_ERROR, "######[RecvDatVerInfoAck]: Call OAM_CFGInitGlobalDatMem failed!dat files can't register");
                }

                /* 将请求次数置回0 */
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

                /* 将请求次数置回0 */
                g_ptOamIntVar->ucVerInfoReqCnt = 0;
    
                OAM_CliReRegisterAllRegistedJobs();
    
                XOS_SysLog(OAM_CFG_LOG_ERROR, "######[RecvDatVerInfoAck]: new dat update finished, change to work state");
                /*切换所有连接到特权模式*/
                /*611002035007 易用性改进六组（3）：cli>后需要输入en才能进入cli#*/
                OAM_CliChangeAllLinkToStartMode(map_szScriptReloaded);
                XOS_SetNextState(S_Work);
            }
        }
    }
#if 0
    /*请求BRS旧的dat信息*/
    else
    {
        memcpy(&g_ptOamIntVar->tBrsDatVerInfo, pVerAck->tOamsVerInfo, sizeof(T_OamsVerInfo));
        XOS_SysLog(OAM_CFG_LOG_NOTICE, "all dat ver info was got, change to work state");
        Oam_CfgPowerOnOverAndToWorkState();
    }
#endif
    /* 由于VMM每次最多支持50个dat请求，所以需要分多次请求 */
    ReqAllCfgVerInfo();
#endif
}

/**************************************************************************
* 函数名称：VOID RecvRegOamsNtfAck
* 功能描述：处理vmm返回的dat notify reg回应
* 访问的表：无
* 修改的表：无
* 输入参数：pMsgPara -- vmm返回的数据
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2008/10/15    V1.0    殷浩      创建
**************************************************************************/
void RecvRegOamsNtfAck(LPVOID pMsgPara)
{
#if 0
    T_OamsVerNotifyRegAck *pVerAck =  NULL;
    
    /*CRDCM00322153 检查收到的消息长度是否错误*/
    if (!Oam_CfgCheckRecvMsgMinLen(sizeof(T_OamsVerNotifyRegAck)))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, 
            "######[RecvRegOamsNtfAck]: The length of message received is less than the length of interface struct! ");

        /* 异常情况时，需要将请求次数清0 */
        g_ptOamIntVar->ucVerNotifyRegReqCnt = 0;
        return;
    }
    
    pVerAck = (T_OamsVerNotifyRegAck *)pMsgPara;
    
    Oam_KillRegOamsNotifyTimer();
    g_ptOamIntVar->ucRegOamsNtfTimes = 0;

    /* 每次请求之后，在接收到应答的地方将请求次数加1 */
    g_ptOamIntVar->ucVerNotifyRegReqCnt++;

    if (SCS_OK == pVerAck->dwRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "######[RecvRegOamsNtfAck]: receive EV_VER_OAMS_NOTIFYREG_ACK, register success!");
    }
    else
    {
        /*?? 版本切换通知注册失败，需要添加告警，将无法收到切换消息*/
        XOS_SysLog(OAM_CFG_LOG_ERROR, "######[RecvRegOamsNtfAck]: receive EV_VER_OAMS_NOTIFYREG_ACK, register failed!");
    }

    if (g_ptOamIntVar->ucVerNotifyRegReqCnt >= 3)
    {
        g_ptOamIntVar->ucVerNotifyRegReqCnt = 0;
        return;
    }

    /* 由于VMM每次最多支持50个dat请求，所以需要分多次请求 */
    RegDatToVmm();
#endif
}

/**************************************************************************
* 函数名称：VOID RecvDatVerInfoGetTimeOut
* 功能描述：处理vmm返回的dat版本信息
* 访问的表：无
* 修改的表：无
* 输入参数：pMsgPara -- vmm返回的数据
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2008/10/15    V1.0    殷浩      创建
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
* 函数名称：VOID RecvRegOamsNtfTimeOut
* 功能描述：处理注册dat切换回应等待超时
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2008/10/15    V1.0    殷浩      创建
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
        /*?? dat版本切换通知注册失败，需要添加告警，版本切换时候不能更新dat*/
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

    /*清除所有注册的dat内存*/
    for (wCnt = 0; wCnt < g_ptOamIntVar->wDatSum; wCnt++)
    {
        if (g_tDatLink)
            OAM_CFGClearCmdScript(&(g_tDatLink[wCnt].datIndex));
    }

    /*清除几个全局内存空间*/
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
* 函数名称：VOID RecvTxtCfgReq
* 功能描述：处理txt存盘数据恢复请求
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2008/10/15    V1.0    殷浩      创建
**************************************************************************/
void RecvTxtCfgReq(void)
{
    JID tAppJid;
    WORD32 dwRestoreErr = 0;
    T_Time tStart, tEnd;
    BYTE bRet = 0;

    /*保存发送TXT恢复请求的JID*/
    memset(&tAppJid, 0, sizeof(JID));
    XOS_Sender(&tAppJid);
    memcpy(&g_ptOamIntVar->jidProtocol, &tAppJid, sizeof(JID));
    /*设置存盘类型:TXT还是ZDB*/
    SetSaveTypeAsTxt(tAppJid);

    XOS_GetCurrentTime(&tStart);
    dwRestoreErr = Oam_RestorePrtclCfgPrepare();
    XOS_GetCurrentTime(&tEnd);
    g_ptOamIntVar->dwMillSecOnPrepare += Oam_Duration(tStart, tEnd);

    if(OAM_RESTORE_PREPARE_OK == dwRestoreErr)
    {
        /* 加载恢复用的dat到内存:
            如果是刀片版本:需要把前面获取的dat(new or old)加载到内存
            如果是pc机版本:目前只支持用新dat进行恢复            
        */
        if (Oam_CfgIfCurVersionIsPC())
        {
            /*如果是pc机环境，为了简化处理，直接注册一下brs dat文件*/
            bRet = OAM_CFGDatRegister(tAppJid, FUNC_SCRIPT_CLIS_BRS);
            if (OAM_CLI_SUCCESS != bRet)
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR, "!!!Dat file of OAM registe failed! can not use OAM config shell!\n");
            }
        }
        else
        {
            /*如果是刀片环境，先清除已加载的配置dat
            再加载恢复专用的dat文件到内存*/
            OAM_CFGRemoveAllDat();

            bRet = OAM_CFGDatRegisterForRestore(tAppJid, FUNC_SCRIPT_CLIS_BRS);
            if (OAM_CLI_SUCCESS != bRet)
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR, "!!!Dat file of OAM registe failed! can not use OAM config shell!\n");
                
                /*如果是刀片版本，需要重新加载配置用的dat文件*/
                if (!Oam_CfgIfCurVersionIsPC())
                {
                    OAM_CFGClearCmdScript(&(g_ptOamIntVar->tCfgdat));
                    OAM_CFGRemoveAllDat();

                    OAM_CliReRegisterAllRegistedJobs();
                }
                
                /*注册失败也算dat不存在*/
                EndRestoreByReasonOf(OAM_TXT_NO_DAT);
                return;
            }
        }

        XOS_SysLog(OAM_CFG_LOG_NOTICE,
                   "Handle EV_TXTCFG_REQ Msg Success! Restore Repare OK!");
        /* 给自己发送恢复准备工作完成消息 */
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
* 函数名称：EndRestoreByReasonOf
* 功能描述：结束恢复操作，并回结果给brs
* 访问的表：无
* 修改的表：无
* 输入参数：dwReason -- 恢复结束原因
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2008/10/15    V1.0    殷浩      创建
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

    /*CRDCM00090651 要给本板的vmm进程注册*/
    /* 构造vmm进程的JID*/
    memset(&tJidVmmMgt, 0, sizeof(tJidVmmMgt));
    XOS_GetSelfJID(&tJidVmmMgt);
    tJidVmmMgt.dwJno = XOS_ConstructJNO(SCS_VM_MGT, 1);

    memset(&tDatRegReq, 0, sizeof(tDatRegReq));
    XOS_GetSelfJID(&(tDatRegReq.tJid));
    
    tDatRegReq.wVerType = VER_TYPE_SCRIPT;
    
    tDatRegReq.ucVerNum = OAM_MIN(wFuncTypeMaxNum, DATFILE_FUNCTYPE_TOTAL);

    XOS_SysLog(OAM_CFG_LOG_ERROR, 
        "######[RegDatToVmm]: g_ptOamIntVar->ucVerNotifyRegReqCnt = %d\n", g_ptOamIntVar->ucVerNotifyRegReqCnt);

    /* 第一次请求50个平台非DDM dat */
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
    /* 第二次请求50个平台DDM dat */
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
    /* 第三次请求50个SBC等业务的dat */
    else
    {
        /*** 1、先扫描共享内存 ***/
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
    	
        /*** 2、再查看全局数据区 ***/
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
            
            /* 最后一次如果没有请求，将此全局变量置回0 */
            g_ptOamIntVar->ucVerNotifyRegReqCnt = 0;

            return;
        }
        
    }

    XOS_SysLog(OAM_CFG_LOG_NOTICE, "######[RegDatToVmm]: send EV_VER_OAMS_NOTIFYREG_REQ to vmm ...");

    XOS_SendAsynMsg(EV_VER_OAMS_NOTIFYREG_REQ, (BYTE *)&tDatRegReq, sizeof(tDatRegReq), 0, 0, &tJidVmmMgt);

    /*设置定时器等待回应*/
    Oam_SetRegOamsNotifyTimer();

    return;
#endif
}

/**************************************************************************
* 函数名称：VOID ExecDatUpdateWaitTimerProc
* 功能描述：处理版本更新定时器超时
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/04/12    V1.0    齐龙兆      创建
**************************************************************************/
static void ExecDatUpdateWaitTimerProc(void)
{
    Oam_SetDATUpdateWaitTimerInvalid();
    /*检查是否所有终端都已经没有命令执行了*/
    if (Oam_CfgIfNoLinkIsExecutingCmd())
    {
        /*清除内存中现有的dat信息*/
        OAM_CFGRemoveAllDat();

        /*如果是刀片环境，清除版本 信息，从vmm重新获取*/
        if (!Oam_CfgIfCurVersionIsPC())
        {
            g_ptOamIntVar->wDatSum = 0;
        }
    
        /*向scs请求所有dat文件信息*/
        g_ptOamIntVar->ucCfgVerInfoGetTimes = 0;
        XOS_SysLog(OAM_CFG_LOG_NOTICE, "begin to request new dat ver info ...");
        ReqAllCfgVerInfo();
    }
    else
    {
        g_ucLinkStateCheckTimes++;
        if(Oam_IfExecutingDatUpdateCmd())/*版本切换命令正在执行*/
        {
            g_ucLinkStateCheckTimes = 0;
            /*设定时器，直到版本切换命令执行完毕*/
            Oam_SetDATUpdateWaitTimer();
        }
        else if (g_ucLinkStateCheckTimes < OAM_CFG_MAX_LINKSTATE_CHECKTIMES)
        {
            /*设定时器，直到所有终端命令执行完毕*/
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
            /*为了使切换处理代码都在一起，通过定时器下一轮触发切换*/
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
* 函数名称：VOID HandleMsgEV_OAM_CFG_CFG
* 功能描述：处理命令执行后应用返回的消息
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/02/12    V1.0    殷浩      创建
**************************************************************************/
static void HandleMsgEV_OAM_CFG_CFG(WORD32 dwMsgId, LPVOID pMsgPara, BOOLEAN bIsSameEndian)
{
    int iLinkStateType = 0;
    TYPE_LINK_STATE * pLinkState = NULL;    
    BYTE ucIdx = 0;
    
    if (!Oam_CfgCheckCommOamMsgLength())
        return;
	
    /* added by fls on 2009-06-29 (CRDCM00197905 SSC2单板联调，OAM发送字节序固定为小尾字节序，没有按照OMP的字节序发送) */
    /*if (Oam_CfgIsNeedInvertOrder())*/
    if (!bIsSameEndian)
    {
        CfgMsgTransByteOder((MSG_COMM_OAM *)pMsgPara);
    }
	
    /* end of add */    
    /* 处理Debug消息 */
    {
        MSG_COMM_OAM *ptRecvMsg = (MSG_COMM_OAM *)pMsgPara;
        CHAR ucaBuf[MAX_OAMMSG_LEN] = {0};

        /* 如果是无效的终端号，则给所有终端发送 */
        if ((0 == ptRecvMsg->SeqNo) && (OAM_INVALID_LINK_CHANNEL == ptRecvMsg->LinkChannel))
        {
            /*debug消息在新行显示*/
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
    
    /*判断LinkState类型*/
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
        /* 收到应用的返回结果之后，杀掉进度上报等待定时器 */
        Oam_KillCmdPlanTimer(pLinkState);

        /*如果应用的进度信息不是0, 说明应用前一步是进度信息，
        且没有返回100, 补充一个100%并换行*/
        if (pLinkState->ucPlanValuePrev > 0)
        {        
            CHAR szPlanStr[MAX_STRING_LEN] = {0};
            CHAR *pPlanStr = szPlanStr;
            BYTE ucPlan = 100;
	
            /* 将进度信息发送到telnet显示 */
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
* 函数名称：VOID Oam_IfExecutingDatUpdateCmd
* 功能描述：检查当前是否正在执行版本切换
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/04/12    V1.0    齐龙兆      创建
**************************************************************************/
static BOOLEAN Oam_IfExecutingDatUpdateCmd(void)
{
    WORD16 wLoop = 0;
    WORD16 wXmlLinkNum = sizeof(gt_XmlLinkState)/sizeof(TYPE_XMLLINKSTATE);
    WORD16 wTelLinkNum = sizeof(gtLinkState)/sizeof(TYPE_LINK_STATE);
    TYPE_XMLLINKSTATE *ptXmlLinkState = NULL;
    TYPE_LINK_STATE *ptTelLinkState = NULL;
    /*判断Telnet端是否在执行版本切换命令*/
    for(wLoop = 0;wLoop < wTelLinkNum;wLoop++)
    {     
        ptTelLinkState = &gtLinkState[wLoop];     
        if ((pec_MsgHandle_EXECUTE == Oam_LinkGetCurRunState(ptTelLinkState)) &&
            ((CMD_INSTALL_ACTIVE == Oam_LinkGetCurCmdId(ptTelLinkState)) || (CMD_INSTALL_ACTIVE_APTD == Oam_LinkGetCurCmdId(ptTelLinkState))))
        {
            return TRUE;
        }
    }
    /*判断OMM端是否在执行版本切换命令*/
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
* 函数名称：VOID Oam_CfgStopAllXmlLinkExcutingCmd
* 功能描述：停止网管连接在执行命令
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/04/18   V1.0    齐龙兆      创建
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
* 函数名称：VOID Oam_CfgStopAllTelLinkExcutingCmd
* 功能描述：停止所有Telnet连接在执行命令
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/04/18   V1.0    齐龙兆      创建
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
    /*停止Telnet模拟链路上存盘命令*/
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
* 函数名称： Oam_CfgStopAllLinkExcutingCmd
* 功能描述：停止连接在执行命令
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/02/12    V1.0    殷浩      创建
**************************************************************************/
void Oam_CfgStopAllLinkExcutingCmd(void)
{
    /*终止XML链路上的执行命令*/
    Oam_CfgStopAllXmlLinkExcutingCmd();
    /*终止Telnet链路上的执行命令*/
    Oam_CfgStopAllTelLinkExcutingCmd();
    return;
}

/*上电成功结束，切换到work状态*/
void Oam_CfgPowerOnOverAndToWorkState(void)
{
    /* CRDCM00025960 OMP主备多次倒换触发OAM_Interpret异常，并且打印函数调用链失败 */
    if (BOARD_MASTER ==  XOS_GetBoardMSState())
    {
        /*
        切换状态到work之前扫描一下注册表
        防止初始状态有进程注册没有被处理，
        等下一次扫描时间会太久
        */
        Oam_CfgScanRegTable();            
    }    
    else if (BOARD_SLAVE ==  XOS_GetBoardMSState())
    {
        /* 向主板请求用户配置文件 */
        XOS_Msend(EV_CFG_USERFILE_REQ, NULL, 0, 0, 0, &g_ptOamIntVar->jidSelf);
        /* 设置定时扫描DAT注册信息的共享内存 */
        XOS_SetLoopTimer(PROCTYPE_OAM_CLIMANAGER, TIMER_REQ_USER_FILE, DURATION_TIMER_REQ_USER_FILE, PARAM_NULL);
    }
    else
    {
    }
        
    XOS_SetNextState(PROCTYPE_OAM_CLIMANAGER, S_Work);
}

/**************************************************************************
* 函数名称： HandleMasterToSlave
* 功能描述： 主转备处理
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/02/12    V1.0    殷浩      创建
**************************************************************************/
static void HandleMasterToSlave(void)
{
    /*释放dat所占内存*/ 
    Oam_CfgCloseAllLink(Hdr_szMSChangeoverTip);
    OAM_CFGRemoveAllDat();
    /*如果是刀片环境，清除版本 信息，从vmm重新获取*/
    if (!Oam_CfgIfCurVersionIsPC())
    {
        g_ptOamIntVar->wDatSum = 0;
    }
    /*设置循环定时器，用来定时发送用户信息文件*/
    XOS_SetLoopTimer(PROCTYPE_OAM_CLIMANAGER, TIMER_REQ_USER_FILE, DURATION_TIMER_REQ_USER_FILE, PARAM_NULL);
    XOS_SetNextState(PROCTYPE_OAM_CLIMANAGER, S_Work);
}

/**************************************************************************
* 函数名称： HandleSlaveToMaster
* 功能描述： 备转主处理
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/02/12    V1.0    殷浩      创建
**************************************************************************/
static void HandleSlaveToMaster(void)
{   
    WORD16 i = 0;
    
    Oam_CfgCloseAllLink(Hdr_szMSChangeoverTip);

    /* CRDCM00025960 OMP主备多次倒换触发OAM_Interpret异常，并且打印函数调用链失败 */
    OAM_CFGRemoveAllDat();
    /*如果是刀片环境，清除版本 信息，从vmm重新获取*/
    if (!Oam_CfgIfCurVersionIsPC())
    {
        g_ptOamIntVar->wDatSum = 0;
    }

    /*离线转在线操作复位，防止上次是异常结束*/
    Oam_CfgHandleMSGOffline2OnlineFinish();

    /* 611001186690 重新加载用户之前，先清掉内存 */
    memset(gtUserInfo, 0, sizeof(gtUserInfo));
    for (i = 0; i < sizeof(gtUserInfo)/sizeof(TYPE_USER_INFO); i++)
    {
        gtUserInfo[i].ucRole = 0;
        gtUserInfo[i].bLocked = FALSE;
        memset(gtUserInfo[i].szUserName, 0, MAX_OAM_USER_NAME_LEN);
        memset(gtUserInfo[i].szPassWord, 0, MAX_OAM_USER_PWD_ENCRYPT_LEN);
    }
    gnUserNum = 0;

    /* 重新加载用户信息 */
    if (!OamLoadUserFile())
    {
        /* 如果加载用户文件失败，则启用默认用户和默认密码，以防止不能登录的情况 */
        UseDefaultUserInfo();
    }

    /* 重新初始化用户日志文件 */
    if (!InitUserLogFile())
    {
        XOS_SysLog(OAM_CFG_LOG_NOTICE, "Read user log file failed!\n");
    }

   /* 重新读取命令行日志文件，计算出具体的行数，以免在写日志时计算影响性能 */
    g_ptOamIntVar->wLinesOfCmdLogFile = 0;
    if(!GetCmdLogFileLines())
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[HandleSlaveToMaster] Get lines of log file fail!\n");

    if (Oam_CfgIfCurVersionIsPC())
    {
        /*如果是pc机版本，直接重新注册*/
        OAM_CliReRegisterAllRegistedJobs();
    }
    else
    {
        /*如果是刀片版本，备转主时候，走与切换dat版本一样的流程*/
        Oam_SetDATUpdateWaitTimer();
        g_ptOamIntVar->ucCfgVerInfoGetTimes = 0;
        XOS_SetNextState(PROCTYPE_OAM_CLIMANAGER, S_UpdateDAT);
    }

    /* 订阅对板上下电消息 */
    OamMateRequestSubScr();
    /*杀掉循环定时器 */
    XOS_KillTimer(PROCTYPE_OAM_CLIMANAGER, TIMER_REQ_USER_FILE, PARAM_NULL);
}

static void HandleMsgEV_OAM_CFG_PLAN(WORD32 dwMsgId, LPVOID pMsgPara, BOOLEAN bIsSameEndian)
{
    /*判断LinkState类型*/
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

    /* 判断是否在存盘 */
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
        if (Xml_CheckIsExecSaving())/*OMM正在执行存盘命令*/
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
    
    /*检查是否有Telnet端在执行存盘操作*/
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
    
    /*** 1、先扫描共享内存 ***/
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
    
    /*** 2、再查看全局数据区 ***/
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
* 函数名称：VOID Oam_Cli_ChangeScriptLanguage
* 功能描述：改变命令行脚本语言类型
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2011/01/27    V1.0    殷浩      创建
**************************************************************************/
void Oam_Cli_ChangeScriptLanguage(LPVOID pMsgPara)
{	
    /*停止所有连接正在执行的命令*/
    Oam_CfgStopAllLinkExcutingCmd(); 
    /*延时2秒*/
    XOS_Delay(2*1000);
	
    if (Oam_CfgIfNoLinkIsExecutingCmd())
    {
        /*清除内存中现有的dat信息*/ 
        OAM_CFGRemoveAllDat();
        /*立即生效*/			
       OAM_CliReRegisterAllRegistedJobs();
       /*切换所有连接到用户模式*/
       OAM_CliChangeAllLinkToStartMode(map_szScriptReloaded);
    }
    else
    {
        /*设置定时器定时重复切换语言类型*/
        XOS_SetRelativeTimer(PROCTYPE_OAM_CLIMANAGER, TIMER_CLI_LOAD_SCRIPT_NEW_LANG, DURATION_CLI_LOAD_SCRIPT_NEW_LANG, 1);
    }
}

/*打印climanager调试函数信息*/
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


