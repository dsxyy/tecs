/*********************************************************************
* ��Ȩ���� (C)2002, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ�linkstate.h
* �ļ���ʶ��
* ����ժҪ��OAM���ͽ���
* ����˵����
            
* ��ǰ�汾��
* ��    �ߣ����
* ������ڣ�2008.10.30
*
* �޸ļ�¼1��// �޸���ʷ��¼�������޸����ڡ��޸��߼��޸�����
*    �޸����ڣ�
*    �� �� �ţ�
*    �� �� �ˣ�
*    �޸����ݣ�
* �޸ļ�¼2����
**********************************************************************/
#ifndef __LINKSTATE_H__
#define __LINKSTATE_H__

#ifndef WIN32
    #ifndef _PACKED_1_
        #define _PACKED_1_ __attribute__ ((packed)) 
    #endif
#else
    #ifndef _PACKED_1_
        #define _PACKED_1_
    #endif
#endif

#ifdef WIN32
    #pragma pack(1)
#endif

#include "oam_cfg_common.h"
#include "oamscript.h"
#include "display.h"

#define MAX_MODESTACK         20
#define MAX_LAYER             260
#define MAX_PARASUM           255
#define MAX_HINTSUM           300
#define MAX_MORE_LAYER        200 /* ����MoreAngle�﷨Ƕ�׿�����100/2��,��Ϊ��������ÿ�ε���Ϊ2 */

#define MAX_VERIFYFAILURE     3

#define MAX_CMDLINE_LEN       OAM_STY_LINE_SIZE /*��OAM_STY_LINE_SIZE һ��*/
#define MAX_OUTPUT_RESULT     1024 * 10 
#define MAX_MEM_BUF           1024 * 10 
#define MAX_VTY_NUMBER        7
/*XML������ר��*/
#define MAX_SIZE_TSECTION_TEXT   (WORD16)256
#define MAX_SIZE_DECL_TEXT           (WORD16)80
#define XML_LINKSTATE_USED       (BYTE)1
#define XML_LINKSTATE_UNUSED   (BYTE)0
#define XML_LINKSTATE_VERCHANGED       (BYTE)1
#define XML_LINKSTATE_VERUNCHANGED   (BYTE)0

typedef struct
{
    CHAR cEndMark;
    CHAR sTextBuf[MAX_STRING_LEN];
}_PACKED_1_ TYPE_TEXT_INPUT;

typedef struct
{
    BYTE    bDeterminer;            /* ƥ���޶��� */
    BYTE    bHasNo;                 /* ��NO���η� */
    WORD16    wWordSum;               /* �Ӵ����� */
    BYTE         ucPad[2];
    WORD16    wBlankSum[MAX_LAYER];   /* �ո���� */
    CHAR    sCmdWord[MAX_LAYER][MAX_STRING_LEN];/* ������&���� */
}_PACKED_1_ TYPE_CMD_LINE;

/*����ű�*/
typedef struct tagT_OAM_Cfg_Dat
{
    WORD16  wCRC;
    BYTE bIsValid;
    BYTE    ucPad[1];
    TYPE_FILE_HEAD tFileHead;
    DWORD nModeSum;
    TYPE_MODE *tMode;
    DWORD nTreeNodeSum ;
    TYPE_TREE_NODE *tCmdTree;
    DWORD nCmdAttrSum;
    TYPE_CMD_ATTR *tCmdAttr;
    DWORD nCmdParaSum;
    TYPE_CMD_PARA *tCmdPara;
    DWORD nFormatSizeSum;
    CHAR *tFormat;
    DWORD nMapTypeSum;
    TYPE_MAP_TYPE *tMapType;
    DWORD nMapItemSum;
    TYPE_MAP_ITEM *tMapItem;
    DWORD nMapStringSum;
    CHAR *tMapString;
    /* ���̺� */
    JID  tJid;
}_PACKED_1_ T_OAM_Cfg_Dat;

typedef struct
{
    WORD16    wPosition;     /* �������ڵ�/������λ��*/
    WORD16    wType;         /* ���� */
    BYTE    bMatched;      /* �Ѿ���ƥ���� */
    BYTE    ucPad[3];
    CHAR    *pName;        /* ���� */
    CHAR    *pComment;     /* ע�� */
    T_OAM_Cfg_Dat * cfgDat;
}_PACKED_1_ TYPE_HINT;     /* ��ʾ��Ϣ */

typedef struct
{
    WORD16    wPosition;     /* ƥ��λ�� */
    WORD16    wType;         /* ���� */
    T_OAM_Cfg_Dat * cfgDat;
}_PACKED_1_ TYPE_PATH;     /* ·����Ϣ */

typedef struct
{
    WORD16    ParaNo;        /* Prarmeter Code Number */
    CHAR   sName[MAX_PARANAME];       /* �������� */
    BYTE    Type;          /* Prarmeter Type */
    BYTE    Len;           /* Prarmeter Length, If Type equal to DATA_TYPE_TABLE, it represent the number of the records */
    BYTE    *pValue;
}_PACKED_1_ CMDPARA;

typedef struct
{
    /* for command */
    BYTE    bHasNo;           /*  ��NO���η� */
    WORD16    wSum;                 /* ƥ����Ŀ */
    WORD16    wDepth;               /* ƥ����� */
    WORD16    wLength;              /* ƥ�䳤�� */
    WORD16    wFirst;               /* ƥ������λ�� */
    WORD16    wCmdAttrPos;          /* ������������ */
    /* for parameter */
    WORD16    wParaSum;             /* �����������, ƥ���ϵ���Ŀ����ȫƥ����ʱ(dExeID != 0),�ǲ������� */
    BYTE    bParaNum;             /* ���������� */
    WORD16    wCurParaPos;          /* ����ƥ��λ�� */
    CMDPARA tPara[MAX_PARASUM];   /* ����������� */
    WORD16    wMaxDepth;            /* ����ɵ�ֵ */
    WORD16    wMaxLength;           /* ����ɵ�ֵ */
    WORD16    wMaxFirst;            /* ����ɵ�ֵ */
    WORD16    wMaxCurParaPos;       /* ����ɵ�ֵ */
    /* for hint */
    TYPE_HINT tHint[MAX_HINTSUM]; /* ƥ��/��ʾ��Ϣջ  */
    BOOLEAN    bCanExecute;          /* �Ƿ����ִ��(����<CR>)*/
    WORD16    wHintStackTop;        /* ��Ϣջ��ָ�� */
    /* for para pos */
    WORD16    wParaBeginPos;
    WORD16    wParaEndPos;
    /*  for {[][][]} and recycle*/
    BYTE    bMoreLayerTop;                   /* MoreAngle�﷨Ƕ��ջ�� */
    WORD16    wMoreBeginPos[MAX_MORE_LAYER];   /* �﷨��ʼλ�� */
    WORD16    wMoreEndPos[MAX_MORE_LAYER];     /* �﷨����λ�� */
    BYTE    bType[MAX_MORE_LAYER];           /* �﷨����5(6) */
    /*  for {[][][]} */
    BYTE    bMoreFlags[MAX_MORE_LAYER];      /* ��Ե�ǰ�﷨��ѡ������ */
    BOOLEAN    bContinueSearch[MAX_MORE_LAYER]; /* �Ƿ��������� */
    TYPE_PATH tMatchedPath[MAX_PARASUM];     /* ��ƥ�������Ϣ */
    WORD16    wMatchedPathTop;                 /* ��Ϣջ��ָ�� */
    /* for recycle */
    BOOLEAN    bPrammarRecycle;
    /* result */
    WORD16    wParseState;          /* �������﷨����״̬ */
    TYPE_PATH tPath[MAX_LAYER];   /* ƥ��·�� */
    WORD16    wPathStackTop;        /* ƥ��·��ջ��ָ�� */
    WORD16    wCliMatchLen;         /* CLIƥ�䳤�� */
    DWORD   dExeID;               /* ����ִ��ID */
    T_OAM_Cfg_Dat * cfgDat;
}_PACKED_1_ TYPE_MATCH_RESULT;

typedef struct tag_TYPE_LINK_STATE
{
    BYTE    ucLinkID;                                /* ���ӱ�ʶ */
    BYTE    ucCurRunState;                       /* ��ǰ����״̬ */
	
    CHAR    tnIpAddr[LEN_IP_ADDR_STR];              /* ����Telnet�ն�IP��ַ */
    TYPE_TEXT_INPUT tInputControl;            /*�û����û�ӭ��Ϣ�����뻻������ר��*/

    CHAR    sUserName[MAX_USERNAME_LEN];       /* �û��� */
    CHAR    sPassword[MAX_OAM_USER_PWD_ENCRYPT_LEN];       /* �û����� */
    CHAR    sPasswordOrg[MAX_PASSWORD_LEN];       /* δ���ܵ�����*/
    CHAR    sSession[MAX_SESSION_LEN];
    CHAR    sSavedUser[MAX_USERNAME_LEN];      /* �л��û�ʱ�����û��� */
    BYTE    ucUserRight;                            /* �û�Ȩ�� */
    BYTE    ucSavedRight;                          /* �л��û�ʱ�����û�Ȩ�� */
    BYTE    ucInputErrTimes;                    /* ���������� */

    BOOLEAN    bTelnetQuit;                       /* �û���Telnet�ն��˳�  */
    BOOLEAN    bRecvAsynMsg;                   /*���ڴ�������첽��Ϣ*/
	
    XOS_TIMER_ID  wExeTimer;                         /* �ȴ�Execute�˳�ʱ��ʱ�� */
    /* 614000678048 (ȥ��24Сʱ���Գ�ʱ) */
    /*XOS_TIMER_ID  wAbsTimer;*/                         /* Telnet�˾��Գ�ʱ��ʱ�� */
    XOS_TIMER_ID  wIdleTimer;                        /* Telnet�˿��г�ʱ��ʱ�� */
    XOS_TIMER_ID  wCmdPlanTimer;                         /* ����ִ�н�������ʱ��ʱ�� */
    WORD16            wAbsTimeout;                     /* Telnet�˾��Գ�ʱ���� */
    WORD16            wIdleTimeout;                    /* Telnet�˿��г�ʱ���� */

    DWORD     dwExeSeqNo;                        /* ��Execute��������Ϣ��ˮ�� */
    WORD16    wTelSeqNo;                           /* ��Telnet��������Ϣ��ˮ�� */

    BYTE    LastData[MAX_STORE_LEN];           /* ����ҵ��ʹ�õĴ洢��*/
    BYTE    BUF[MAX_STORE_LEN];                   /* ����ҵ��ʹ�õĴ洢��*/

    BOOLEAN bChangeMode;                     /* ��Ҫ����ģʽ */
    BOOLEAN bTelInputModeIsSyn;          /*����telnetsvr����ģʽ���첽����ͬ����debug��*/
    BOOLEAN bTelInputModeChanged;     /*telnetsvr����ģʽ�Ƿ����仯*/
    BYTE        ucInputState;                         /* ����״̬ ,�����û����������״̬*/
    BOOLEAN bInputStateChanged;          /* ����״̬�Ƿ����˱仯*/
    CHAR       sPrompt[MAX_PROMPT_LEN];   /* ��ʾ�� */
	
    BYTE    bModeID[MAX_MODESTACK];     /* ģʽID��ջ */
    BYTE    bModeStackTop;                       /* ��ջ��ָ�� */

    WORD16   wAppMsgReturnCode;         /*app����Ϣ������*/
    BOOLEAN  bOutputResult;                    /* ��Ҫ���ִ�н�� */
    BYTE        bOutputMode;                      /* ���ģʽ */
    CHAR       *sOutputResult;                   /* �������ַ��� */
    WORD16   wSendPos;                          /* �����λ�� */

    BOOLEAN bOutputCmdLine;                /* ��Ҫ���������� */
    CHAR      *sCmdLine;                          /* ���Ե������� */

    TYPE_CMD_LINE         tCmdLine;                /* ��������Ϣ */
    TYPE_MATCH_RESULT tMatchResult;            /* ƥ���� */
    WORD16                    wCmdAttrPos;                      /* ������������ */

    TYPE_DISP_GLOBAL_PARA  tDispGlobalPara;    /* ���Բ��ֵ�ȫ�ֱ�������̬���� */

    BOOLEAN bLogFileOK;                             /*��������־�ļ��Ƿ����*/
    BOOLEAN bIsSupperUsrMode;                 /*��ǰ�û��Ƿ��ǳ����û�ģʽ��*/

    T_CliCmdLogRecord       tCmdLog;          /* �����в�����־ */

    BYTE    ucPlanValuePrev;                    /* ���浱ǰ����ֵ����5s��ʱ֮����Ҫ���� */

    BOOLEAN    bSSHConnRslt;                      /* SSH���ӽ�� */
    BOOLEAN    bSSHAuthMsg;                       /* �Ƿ�ΪSSH��¼��Ϣ */
    BOOLEAN    bSSHAuthRslt;                      /* SSH��¼��� */

    BOOLEAN bRestartReqSended;
    TYPE_USER_LOG    tUserLog;                 /* �û���¼�ǳ���־ */
    
    BOOLEAN    CmdRestartFlagForXmlRpc;
}_PACKED_1_ TYPE_LINK_STATE;

/*XML������ר��*/
typedef struct tagT_TYPE_CUR_CMDINFO
{
    WORD16 wCmdAttrPos;  /* ������������ */
    WORD16  wParaBeginPos;    /* ��������������ʼλ��*/    
    WORD16  wParaEndPos;       /* ����������������λ��*/
    BYTE bIsNoCmd;                   /*�Ƿ�NO����*/
    BYTE ucPad[1];
    T_OAM_Cfg_Dat * cfgDat;/* ��������dat�ṹָ��*/
}_PACKED_1_ TYPE_CUR_CMDINFO;/*��ǰ������Ϣ*/

typedef struct tagT_TYPE_XMLLINKSTATE
{
    XOS_TIMER_ID  wExeTimer;                         /* �ȴ�Execute�˳�ʱ��ʱ�� */
    XOS_TIMER_ID  wCmdPlanTimer;                         /* ����ִ�н�������ʱ��ʱ�� */
    DWORD  dwSeqNo;                                              /*ͬҵ�񽻻��ķ������*/
    DWORD dwCmdId;                                               /*����ID*/
    WORD32  dwLinkChannel;                                     /*��·��*/
    BYTE aucPad[2];
    BYTE ucComponentId[2];                                    /*�����*/	   
    TYPE_CUR_CMDINFO tCurCMDInfo;                      /*��ǰ������Ϣ*/ 
    CHAR xmlTSecText[MAX_SIZE_TSECTION_TEXT];  /*XML�ַ���T��*/        
    CHAR xmlDeclText[MAX_SIZE_DECL_TEXT];         /*XML��������*/
    BYTE    LastData[MAX_STORE_LEN];           /* ����ҵ��ʹ�õĴ洢��*/
    BYTE    BUF[MAX_STORE_LEN];                   /* ����ҵ��ʹ�õĴ洢��*/
    JID tDtJid;                                                            /*��ǰ����Ŀ��JID*/    
    TYPE_DISP_GLOBAL_PARA  tDispGlobalPara;       /* ���Բ��ֵ�ȫ�ֱ�������̬���� */
    MSG_COMM_OAM *ptLastSendMsg;                     /*��һ�η��͵�OAM��Ϣ��ָ��*/
    BYTE    bModeID[MAX_MODESTACK];     /* ģʽID��ջ */
    BYTE    bModeStackTop;                       /* ��ջ��ָ�� */    
    BYTE bUsed;                                                        /*�Ƿ�ʹ�� 0-δʹ�� 1-ʹ��*/   
    BYTE    ucCurRunState;                       /* ��ǰ����״̬ */
    BYTE    bVerChanged;                         /*�汾�л���־*/
    BYTE    ucPlanValuePrev;                    /* ���浱ǰ����ֵ����5s��ʱ֮����Ҫ���� */
}_PACKED_1_ TYPE_XMLLINKSTATE;/*XML����״̬*/

/* ��ǰ����״̬*/
#define pec_MsgHandle_IDLE    0    /* ��״̬ */
#define pec_Init_USERNAME     10   /* �û������뼰��֤״̬ */
#define pec_Init_PASSWORD     20   /* �������뼰��Ȩ״̬ */

#define pec_MsgHandle_INIT        100  
#define pec_MsgHandle_TELNET   110  /* Telnet��ִ��״̬ */
#define pec_MsgHandle_EXECUTE 120  /* Execute��ִ��״̬ */

extern void Oam_LinkInitializeForNewConn(TYPE_LINK_STATE *pLinkState);
extern void Oam_LinkSetCurRunState(TYPE_LINK_STATE *pLinkState, BYTE bCurRunState);
extern BOOLEAN CheckLinkIsExecutingSaveCmd(void);
extern BOOLEAN CheckLinkIsExecutingCmd(void);
extern WORD16 Xml_GetIndexOfSaveLinkState(void);
extern BOOLEAN Xml_CheckIsExecSaving(void);
extern BYTE XML_LinkGetCurRunState(TYPE_XMLLINKSTATE *pLinkState);
extern void XML_LinkSetCurRunState(TYPE_XMLLINKSTATE *pLinkState,BYTE ucState);
extern BYTE Oam_LinkGetCurRunState(TYPE_LINK_STATE *pLinkState);

extern BOOLEAN Oam_LinkGetUserRight(LPVOID ptOamMsg,WORD32 dwMsgId,BYTE *pUserRight);
extern BOOLEAN Oam_LinkSetUserRight(LPVOID ptOamMsg,WORD32 dwMsgId,BYTE *pUserRight);
extern BOOLEAN Oam_LinkGetCmdLine(LPVOID ptOamMsg,WORD32 dwMsgId,TYPE_CMD_LINE  *pCmdLine);
extern BOOLEAN Oam_LinkSetCmdLine(LPVOID ptOamMsg,WORD32 dwMsgId,TYPE_CMD_LINE  *pCmdLine);
extern BOOLEAN Oam_LinkGetCmdLog(LPVOID ptOamMsg,WORD32 dwMsgId,T_CliCmdLogRecord *pCmdLog);
extern BOOLEAN Oam_LinkSetCmdLog(LPVOID ptOamMsg,WORD32 dwMsgId,T_CliCmdLogRecord *pCmdLog);
extern BOOLEAN Oam_LinkGetUserName(LPVOID ptOamMsg,WORD32 dwMsgId,CHAR *pUserName);
extern BOOLEAN Oam_LinkSetUserName(LPVOID ptOamMsg,WORD32 dwMsgId,CHAR *pUserName);
extern BOOLEAN Oam_LinkGetModeID(LPVOID ptOamMsg,WORD32 dwMsgId,BYTE *pModeID);
extern BOOLEAN Oam_LinkSetModeID(LPVOID ptOamMsg,WORD32 dwMsgId,BYTE *pModeID);
extern BOOLEAN Oam_LinkGetModeStackTop(LPVOID ptOamMsg,WORD32 dwMsgId,BYTE *pModeStackTop);
extern BOOLEAN Oam_LinkSetModeStackTop(LPVOID ptOamMsg,WORD32 dwMsgId,BYTE *pModeStackTop);
extern BYTE Oam_LinkGetCurRunStateByIndex(BYTE index);
extern void Oam_LinkSetCurRunStateByIndex(BYTE index,BYTE ucState);
extern BOOLEAN Oam_LinkGetIdleTimeout(LPVOID ptOamMsg,WORD32 dwMsgId,WORD16 *wIdleTimeout);
extern BOOLEAN Oam_LinkSetIdleTimeout(LPVOID ptOamMsg,WORD32 dwMsgId,WORD16 *wIdleTimeout);
extern BOOLEAN Oam_LinkGetTnIpAddr(LPVOID ptOamMsg,WORD32 dwMsgId,CHAR *tnIpAddr);
extern BOOLEAN Oam_LinkSetTnIpAddr(LPVOID ptOamMsg,WORD32 dwMsgId,CHAR *tnIpAddr);

extern BOOLEAN Oam_LinkGetIdleTimeoutByIndex(BYTE index,WORD16 *wIdleTimeout);
extern BOOLEAN Oam_LinkGetTnIpAddrByIndex(BYTE index,CHAR *tnIpAddr);
extern BOOLEAN Oam_LinkGetUserNameByIndex(BYTE index,CHAR *pUserName);


extern void Oam_LinkSetLinkId(TYPE_LINK_STATE *pLinkState, BYTE bLinkId);
extern BYTE Oam_LinkGetLinkId(TYPE_LINK_STATE *pLinkState);
extern void Oam_SetIdleTimer(TYPE_LINK_STATE *pLinkState);
extern void Oam_InnerGetRtnMsgInfo(LPVOID ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg);
/* 614000678048 (ȥ��24Сʱ���Գ�ʱ) */
#if 0
extern void Oam_SetAbsTimer(TYPE_LINK_STATE *pLinkState);
#endif
extern void Oam_SetExeTimer(TYPE_LINK_STATE *pLinkState);
extern void Xml_SetExeTimer(TYPE_XMLLINKSTATE *pLinkState);
extern void Xml_KillExeTimer(TYPE_XMLLINKSTATE *pLinkState); 
extern void Oam_KillIdleTimer(TYPE_LINK_STATE *pLinkState);
/* 614000678048 (ȥ��24Сʱ���Գ�ʱ) */
#if 0
extern void Oam_KillAbsTimer(TYPE_LINK_STATE *pLinkState);
#endif
extern void Oam_KillExeTimer(TYPE_LINK_STATE *pLinkState);

extern void Oam_LinkSetTelsvrInputMode(TYPE_LINK_STATE *pLinkState, TELSVR_INPUT_MODE ucInputMode);
extern TELSVR_INPUT_MODE Oam_LinkGetTelsvrInputMode(TYPE_LINK_STATE *pLinkState);
extern BYTE Oam_GetCurModeId(TYPE_LINK_STATE *pLinkState);
extern TYPE_MODE* Oam_GetCurMode(TYPE_LINK_STATE *pLinkState);
extern void Oam_LinkSetInputState(TYPE_LINK_STATE *pLinkState, BYTE ucInputState);
extern BYTE Oam_LinkGetInputState(TYPE_LINK_STATE *pLinkState);
extern WORD32 Oam_LinkGetCurCmdId(TYPE_LINK_STATE *pLinkState);
extern BOOLEAN Oam_LinkIfRecvAsyncMsg(TYPE_LINK_STATE *pLinkState);
extern BOOLEAN Oam_LinkIfRecvLastPktOfApp(TYPE_LINK_STATE *pLinkState);
extern void Oam_LinkModeStack_Push(TYPE_LINK_STATE *pLinkState,  BYTE ucModeId);
extern void Oam_LinkModeStack_Pop(TYPE_LINK_STATE *pLinkState);
extern BOOLEAN Oam_LinkModeStatck_IsEmpty(TYPE_LINK_STATE *pLinkState);
extern void Oam_LinkModeStack_Init(TYPE_LINK_STATE *pLinkState);
extern BOOLEAN Oam_CheckIsHasMode(TYPE_LINK_STATE *pLinkState,BYTE bModeId);
extern void Oam_LinkMarkLogout(TYPE_LINK_STATE *pLinkState, BOOLEAN bLogOut);
extern BOOLEAN Oam_LinkNeedLogout(TYPE_LINK_STATE *pLinkState);
extern BOOLEAN Oam_LinkIfTelSvrInputModeIsSyn(TYPE_LINK_STATE *pLinkState);
extern void Oam_LinkSetTelSvrInputModeAsSyn(TYPE_LINK_STATE *pLinkState, BOOLEAN bOpen);
extern BOOLEAN Oam_LinkIfCurCmdIsNo(TYPE_LINK_STATE *pLinkState);
/*XML������ר��*/
extern BOOLEAN XML_CheckCurCmdIsNo(TYPE_XMLLINKSTATE *ptLinkState);
extern void XML_ChangeMode(TYPE_XMLLINKSTATE *pLinkState);/*�ı�ģʽ*/
extern BYTE XML_GetCurModeId(TYPE_XMLLINKSTATE *ptLinkState);/*��ȡ��ǰģʽID */
extern void XML_LinkModeStack_Push(TYPE_XMLLINKSTATE *pLinkState,  BYTE ucModeId);/*ģʽIDѹջ*/
extern void XML_LinkModeStack_Pop(TYPE_XMLLINKSTATE *pLinkState);/*ģʽID��ջ*/
extern BOOLEAN XML_LinkModeStatck_IsEmpty(TYPE_XMLLINKSTATE *pLinkState);/*�ж�ģʽID�Ƿ�Ϊ��*/
extern TYPE_XMLLINKSTATE *Xml_GetLinkStateByLinkChannel(WORD32 dwLinkChannel);/*��ȡXML��·��Ϣ*/
extern TYPE_XMLLINKSTATE *Xml_GetCurUsedLinkState(void);
extern void Xml_InitLinkState(void);/*��ʼ����·״̬*/
extern INT Xml_GetIndexOfLinkState(WORD32 dwLinkChannel);/*��ȡXML��·״̬�±�*/
extern TYPE_XMLLINKSTATE *Xml_GetLinkStateByIndex(WORD16 wLinkStateIndex);/*����XML��·�±��ȡ��·״̬*/
extern TYPE_XMLLINKSTATE *XML_GetUsableLinkState(void);/*��ȡ����XML��·��Ϣ*/
extern TYPE_XMLLINKSTATE *Xml_AssignLinkState(WORD32 dwLinkChannel);/*����XML��·״̬*/
extern BOOLEAN XML_LinkGetModeID(WORD16 wLinkStateIndex,BYTE *pModeID);
extern BOOLEAN XML_LinkSetModeID(WORD16 wLinkStateIndex,BYTE *pModeID);
extern BOOLEAN XML_LinkGetModeStackTop(WORD16 wLinkStateIndex,BYTE *pModeStackTop);
extern BOOLEAN XML_LinkSetModeStackTop(WORD16 wLinkStateIndex,BYTE *pModeStackTop);
#if 0
/* ��ʱ�� */
#define TIMER_IDLE_TIMEOUT              (TIMER_NO_1)
#define EV_TIMER_IDLE_TIMEOUT           (EV_TIMER_1)
#define DURATION_IDLE_TIMEOUT           (WORD16)(60*1000)

#define TIMER_ABS_TIMEOUT               (TIMER_NO_2)
#define EV_TIMER_ABS_TIMEOUT            (EV_TIMER_2)
#define DURATION_ABS_TIMEOUT            (WORD16)(60*1000)

#define TIMER_EXECUTE_NORMAL            (TIMER_NO_3)
#define EV_TIMER_EXECUTE_NORMAL         (EV_TIMER_3)
#define DURATION_EXECUTE_NORMAL         (WORD16)(30*1000)
#endif

#ifdef WIN32
    #pragma pack()
#endif

#endif /*__LINKSTATE_H__*/

