/*********************************************************************
* ��Ȩ���� (C)2002, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ�Interpret.h
* �ļ���ʶ��
* ����ժҪ��OAM���ͽ���
* ����˵����
            
* ��ǰ�汾��
* ��    �ߣ�����
* ������ڣ�2007.6.12
*
* �޸ļ�¼1��// �޸���ʷ��¼�������޸����ڡ��޸��߼��޸�����
*    �޸����ڣ�
*    �� �� �ţ�
*    �� �� �ˣ�
*    �޸����ݣ�
* �޸ļ�¼2����
**********************************************************************/
#ifndef __INTERPRET_H__
#define __INTERPRET_H__

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

#include "display.h"
#include "linkstate.h"
#include "pub_tmp.h"

/*MSG_COMM_OAM �ṹ��ucOamTag����*/
#define OAMCFG_TAG_NORMAL                     0
#define OAMCFG_TAG_OFFLINE_TO_ONLINE          1
#define OAMCFG_TAG_TXT_RESTORE                2

#define OAM_STY_LINE_SIZE          512

#define S_Init          (WORD16)0
#define S_Work          (WORD16)1
#define S_UpdateDAT     (WORD16)2
#define S_CfgSave       (WORD16)3
#define S_Restore       (WORD16)4

/* ��Ч���ն˺� */
#define OAM_INVALID_LINK_CHANNEL    0

/* ��ʱ�� */
#define TIMER_IDLE_TIMEOUT              (TIMER_NO_1)
#define EV_TIMER_IDLE_TIMEOUT           (EV_TIMER_1)
#define DURATION_IDLE_TIMEOUT           (SWORD32)(60*1000)

/* 614000678048 (ȥ��24Сʱ���Գ�ʱ) */
#if 0
#define TIMER_ABS_TIMEOUT               (TIMER_NO_2)
#define EV_TIMER_ABS_TIMEOUT            (EV_TIMER_2)
#define DURATION_ABS_TIMEOUT            (SWORD32)(60*1000)
#endif

#define TIMER_EXECUTE_NORMAL            (TIMER_NO_3)
#define EV_TIMER_EXECUTE_NORMAL         (EV_TIMER_3)
#define DURATION_EXECUTE_NORMAL         (SWORD32)(30*1000)

/* ��ʱɨ�蹲���ڴ���Ϣ */
#define TIMER_SCAN_REGSHM               (TIMER_NO_4)
#define EV_TIMER_SCAN_REGSHM            (EV_TIMER_4)
#define DURATION_SCAN_REGSHM            (SWORD32)(120*1000)

/* �汾�л�ʱ���ȵȴ�һ�ᣬ
�ڴ��ڼ��յ��µ��л���Ϣ�����趨ʱ��
��ʱ��ſ�ʼ���ذ汾*/
#define TIMER_DAT_UPDATE_WAIT         (TIMER_NO_5)
#define EV_TIMER_DAT_UPDATE_WAIT   (EV_TIMER_5)
#define DURATION_DAT_UPDATE_WAIT  (SWORD32)(5*1000)

/*��vmm��ȡdat��Ϣ���Դ���*/
#define OAM_CFG_GETDATINFO_RETRY_TIMES  100
/* ��ȡdat�汾��Ϣ��ʱ�� */
#define TIMER_GET_CFGVERINFO               (TIMER_NO_6)
#define EV_TIMER_GET_CFGVERINFO         (EV_TIMER_6)
#define DURATION_GET_CFGVERINFO        (SWORD32)(5*1000)

/*��vmmע��dat�汾��ʱ���Դ���*/
#define OAM_CFG_REGOAMS_NTF_RETRY_TIMES  OAM_CFG_GETDATINFO_RETRY_TIMES
/*�ȴ�vmm dat�汾ע��Ӧ��ʱ��*/
#define TIMER_REGOAMS_NTF_ACK                    (TIMER_NO_8)
#define EV_TIMER_REGOAMS_NTF_ACK              (EV_TIMER_8)
#define DURATION_REGOAMS_NTF_ACK             DURATION_GET_CFGVERINFO

/*�������������ͽű���ʱ�������ڼ���ʧ�ܵ����*/
#define TIMER_CLI_LOAD_SCRIPT_NEW_LANG     (TIMER_NO_9)
#define EV_TIMER_CLI_LOAD_SCRIPT_NEW_LANG    (EV_TIMER_9)
#define DURATION_CLI_LOAD_SCRIPT_NEW_LANG   (SWORD32)(5*1000)

/*Ӧ���ϱ�����ִ�н�������Ϣ��ʱ*/
#define TIMER_CMD_PLAN                 (TIMER_NO_11)
#define EV_TIMER_CMD_PLAN              (EV_TIMER_11)
#define DURATION_TIMER_CMD_PLAN        (SWORD32)(5*1000)

/*Ӧ���ϱ�����ִ�н�������Ϣ��ʱ(XML��)*/
#define TIMER_CMD_PLAN_XML                 (TIMER_NO_12)
#define EV_TIMER_CMD_PLAN_XML              (EV_TIMER_12)
#define DURATION_TIMER_CMD_PLAN_XML        (SWORD32)(5*1000)

/*���������������û������ļ���ʱ��*/
#define TIMER_REQ_USER_FILE                 (TIMER_NO_13)
#define EV_TIMER_REQ_USER_FILE              (EV_TIMER_13)
#define DURATION_TIMER_REQ_USER_FILE        (SWORD32)(10*1000)

/*XML���������������ֶ�ʱ��*/
#define TIMER_XML_OMM_HANDSHAKE      (TIMER_NO_20)
#define EV_TIMER_XML_OMM_HANDSHAKE  (EV_TIMER_20)
#define DURATION_XML_OMM_HANDSHAKE  (WORD16)(15*1000)

/*XML��������ǰ̨Ӧ��Ӧ��ʱ��*/
#define TIMER_EXECUTE_XMLADAPTER               (TIMER_NO_21)
#define EV_TIMER_EXECUTE_XMLADAPTER         (EV_TIMER_21)
#define DURATION_EXECUTE_XMLADAPTER         (SWORD32)(35*1000)


#define NEXTMODE_NORMAL       0      /*������һģʽ��������ָ����һģʽ*/

#define CMD_MODE_INVALID      0
#define CMD_MODE_USER         1
#define CMD_MODE_PRIV         2
#define CMD_MODE_MAINTAINANCE 3
#define CMD_MODE_PROTOCOL     20
#define CMD_MODE_CONFIG       CMD_MODE_PROTOCOL /*3*/
#define CMD_MODE_SUBINTERFACE 24
#define CMD_MODE_TUNNEL       20
#define CMD_MODE_BGP          22
#define CMD_MODE_ADDRIPV4     36
#define CMD_MODE_ADDRVPNV4    37
#define CMD_MODE_SAME         251
#define CMD_MODE_FORWARD      252

/* input mode */
#define INMODE_INVALID        0
#define INMODE_NORMAL         1
#define INMODE_USERNAME       2
#define INMODE_PASSWORD       3
#define INMODE_PARAMETER      4
#define INMODE_COMMAND        5
#define INMODE_CMOS           6
#define INMODE_MUTUAL         7
#define INMODE_MENU           8
#define INMODE_TEXT           10

#define OAM_CFG_RESTORE_IO_BUF_LEN    1024*4

/* define the constant */
#define MSG_SUCCESS           0
/* error code of checking the message head of the telnet part */
#define MSGERR_MSGID          -101
#define MSGERR_VTYNUM         -102
#define MSGERR_DETERMINER     -103

/* error code of checking the message head of the execute part */
#define MSGERR_RETURNCODE     -201
#define MSGERR_LINKCHANNEL    -202
#define MSGERR_SEQNO          -203
#define MSGERR_LASTPACKET     -204
#define MSGERR_OUTPUTMODE     -205
#define MSGERR_CMDID          -206
#define MSGERR_EXECMODEID     -207
#define MSGERR_IFNO           -208
#define MSGERR_NUMBER         -209
#define MSGERR_FROMINTPR      -210
#define MSGERR_NONEEDTHECMD   -211
#define MSGERR_DISCARDMSG       -212

/*��������г��ȣ��ָ�����ʱ����*/
#define OAM_MAX_CMDLINE_LEN    512

/* SCS VMM */
#define  OAM_NEW_FILE                0
#define  OAM_OLD_FILE                1
#define  MAX_WAIT_SCS_DATFILE_TIME   3
#define  OAM_DAT_FILE_EXIT           1
#define  SCS_DAT_FILE_EXIT           0
#define  SCS_DAT_FILE_NO_EXIT        1

#define SAFE_FREE(p) \
	if(p != NULL) \
	{ \
	    XOS_Free((BYTE *)p); \
	    p = NULL; \
	}

/*������Ϣreturncode�ж��Ƿ������һ��*/
#define OAM_MSG_IS_LAST_PACKET(pMsg)\
            (((pMsg)->ReturnCode != SUCC_CMD_NOT_FINISHED) && \
              ((pMsg)->ReturnCode != SUCC_CMD_DISPLAY_AND_KILLTIMER) && \
              ((pMsg)->ReturnCode != SUCC_CMD_DISPLAY_NO_KILLTIMER) && \
              ((pMsg)->ReturnCode != SUCC_WAIT_AND_HAVEPARA) )
		
/* define the structures */



/* information about the router */
typedef struct
{
    CHAR    sRawPrompt[MAX_PROMPT_LEN];   /* ԭʼ��ʾ�� */
    CHAR    sBanner[MAX_BANNER_LEN];      /* ��ӭ��Ϣ */
}_PACKED_1_ TYPE_ROUTER_INFO;

typedef enum
{
    LINKSTATE_TYPE_ERR = -1,/*��·״̬���ʹ���*/
    LINKSTATE_TYPE_CLI,      /*CLI*/
    LINKSTATE_TYPE_XML      /*XML*/
}LINKSTATE_TYPE;/*��·״̬����*/

typedef struct tagEXEVTY
{
    JID     jidSender;
    BYTE    aucRcvMsg[MAX_APPMSG_LEN]; /*���յ���Ϣ�洢��*/
    BYTE    aucRtnMsg[MAX_OAMMSG_LEN]; /*���ص���Ϣ�洢��*/
}_PACKED_1_ VTY;

/*
#define   FILE_NAME_LENGTH 80
*/

/*******************************************************************************/
typedef struct tag_DAT_Link
{
    /* ������ */
    T_OAM_Cfg_Dat datIndex;
    /* ����ָ�롱 */
    WORD16 next;
}_PACKED_1_ T_DAT_LINK;
#define MAX_DAT_LINK_COUNT  100
/*******************************************************************************/

 #define    MAX_VER_INFO_NUM    (OAMS_VER_MAX * 3)
 
 /*cli�������ͽṹ��*/
typedef struct
{
    CHAR aucLangType[OAM_CLI_LANGTYPE_LEN];
    SWORD32 swLangOffSet;
}_PACKED_1_ T_Cli_LangType;

/*Dat�ļ���Ϣ*/
typedef struct{
    T_OamsVerInfo tFileInfo;
    T_Cli_LangType tLang; /*��ǰ���ص�dat��������*/
}_PACKED_1_ T_DatFileInfo;

/* �������ṹ�� */
typedef struct tagT_OAM_INTERPRET_VAR
{
    JID     jidSelf;
    JID     jidTelnet;
    JID     jidProtocol;
    
    BYTE    aucRecvBuf[MAX_APPMSG_LEN];
    BYTE    aucSendBuf[MAX_OAMMSG_LEN];
    CHAR    sOutputResultBuf[MAX_OUTPUT_RESULT];  /* �������ַ��� */
    CHAR    sCmdLineBuf[MAX_CMDLINE_LEN];         /* ���Ե������� */
    CHAR    *pMemBufOri[MAX_CLI_VTY_NUMBER - MIN_CLI_VTY_NUMBER + 2];/*�ڲ�ʹ�õ��ڴ��,���һ���ָ�XML Adpater*/
    CHAR    *pMemBufCur[MAX_CLI_VTY_NUMBER - MIN_CLI_VTY_NUMBER + 2];/*�ڴ�鵱ǰָ��*/

    /*SWORD32 swCfgFileFd;*/
    XOS_FD dwTxtFileFd;
    CHAR   *pcTxtFileBuf;
    CHAR   *pcTxtFilePos;
    BOOLEAN   bRestoreFirstPrint;                            /*�Ƿ��ǻָ��ĵ�һ����ӡ*/
    WORD32 dwMillSecOnBrs;                                /*�ָ��ڼ�brs��������������*/
    WORD32 dwMillSecOnParse;                              /*�ָ��ڼ������������������*/
    WORD32 dwMillSecOnPrepare;                            /*�ָ��ڼ�׼����������������*/
    WORD32 dwMillSecOnReadTxt;
    WORD32 dwMillSecOnExit;
/*    T_OAM_Cfg_Dat oamCfgDat;                              OAM�Լ���DAT�ű�*/
    T_CliAppReg         tPlatMpCliAppInfo;         /*��OMP�ϵ�ƽ̨ҵ�����ע���DAT��Ϣ */
    T_CliAppAgtRegEx   tLompCliAppRegInfo;        /* ������OMP�ϵ�Lomp��ƽ̨ҵ�����ע���DAT��Ϣ */
    WORD16 wDatSum;                                       /*�汾�����������dat�ļ���*/
    WORD32 dwModeSum;                                     /*����dat�ļ���ģʽ����*/
    WORD32 dwTreeNodeSum;                                 /*����dat�ļ��нڵ�����*/
    BYTE       ucCfgVerInfoGetTimes;                 /*dat�汾��Ϣ��ȡ���Դ���*/
    BYTE       ucRegOamsNtfTimes;                      /*��vmmע��dat�л�֪ͨ���Դ���*/

    WORD16 wLinesOfCmdLogFile;                /* ��������־������ */
    
    XOS_TIMER_ID wGetCfgVerInfoTimer;        /*��ȡ����dat�汾��Ϣ��ʱ��*/
    XOS_TIMER_ID  wDatUpdateWaitTimer;       /* dat update ʹ�õĶ�ʱ�� */
    XOS_TIMER_ID wRegOamsNotifyTimer;        /*ע��dat�汾�л�֪ͨ��ʱ��*/

    T_DatFileInfo tDatVerInfo[MAX_VER_INFO_NUM];
    T_OamsVerInfo tBrsDatVerInfo;
    T_OAM_Cfg_Dat tCfgdat;
    BOOL bSupportStringWithBlank;              /*�Ƿ�֧�ִ��ո��string���ͣ�true-֧��*/

    BYTE ucVerNotifyRegReqCnt;                 /* ����VMMÿ�����֧��50���������Զ�������ű��汾ע��������� */
    BYTE ucVerInfoReqCnt;                      /* ����VMMÿ�����֧��50���������Զ�������ű���Ϣ������� */
}_PACKED_1_ T_OAM_INTERPRET_VAR;
extern T_DAT_LINK *g_tDatLink;
extern WORD16 g_wRegDatCount;

/*******************************************************************************/

/*******************************************************************************/
/* ��ʱ������Ӧ��ʹ��inline���� */
BOOL EqualJID(const JID* jid1, const JID* jid2);
/*******************************************************************************/

extern TYPE_LINK_STATE gtLinkState[MAX_CLI_VTY_NUMBER - MIN_CLI_VTY_NUMBER + 1];
extern TYPE_LINK_STATE gt_SaveLinkState;
extern TYPE_XMLLINKSTATE  gt_XmlLinkState[MAX_SIZE_LINKSTATE];

extern CHAR g_szSpecifyIP[LEN_IP_ADDR_STR];
extern BYTE   gnPasswordIntensity;     /*�Ƿ�Ϊǿ����*/
extern BYTE   gnNorPasslesslen;          /*�������������С�̶�*/
extern DWORD gnUserNum;                /* ���е��û����� */
extern TYPE_USER_INFO gtUserInfo[MAX_USER_NUM_ROLE_ADMIN + MAX_USER_NUM_ROLE_CONFIG + MAX_USER_NUM_ROLE_VIEW]; /* ���е��û���Ϣ */
extern CHAR  g_aucScriptLanguage[OAM_CLI_LANGTYPE_LEN]; /* ���������ַ���*/

/*****************************************************************************/
typedef struct
{
    TYPE_TREE_NODE treenode;
    T_OAM_Cfg_Dat* cfgDat;
}_PACKED_1_ T_TreeNode;

/* �������� */
extern WORD16 g_wFreeNode;
extern T_TreeNode *gtTreeNode;
WORD16 AppendSiblingNode(WORD16 siblingnode, TYPE_TREE_NODE * treenode, T_OAM_Cfg_Dat* cfgDat);
WORD16 OAM_CFGAppendChildNode(WORD16 parentnode, TYPE_TREE_NODE * treenode, T_OAM_Cfg_Dat* cfgDat);
void DeleteChildNodes(T_OAM_Cfg_Dat* cfgDat);

extern TYPE_MODE *gtMode;
extern DWORD g_dwModeCount;

void InitMode();
WORD16 OAM_CFGAppendMode(TYPE_MODE * mode, BOOLEAN bRestore);
void OAM_CFGDeleteMode(BYTE modeID);
DWORD OAM_CFGSearchMode(BYTE modeID);

extern void OAM_CFGPrintgtMode(void);

extern void OAM_CFGInitTreeNode(void);
extern void OAM_CFGInitTreeNodeForRestore(WORD32 dwTreeNodeSum);

extern BOOLEAN Oam_CfgIfCurVersionIsPC(void);
/*****************************************************************************/
extern COMMON_INFO gtCommonInfo;
extern USER_NODE *pUserLinkHead;
extern USER_NODE *pUserLinkTail;

extern T_OAM_INTERPRET_VAR *g_ptOamIntVar;


/* ���Ⱪ¶�ӿ� */
extern OPR_DATA *GetNextOprData(OPR_DATA *pOprData);
extern void *Oam_InptGetBuf(DWORD bufSize,BYTE ucLinkID);
extern void Oam_InptFreeBuf(BYTE ucLinkID);

extern void Oam_SetDATUpdateWaitTimer(void);
extern void Oam_KillDATUpdateWaitTimer(void);
extern void Oam_SetDATUpdateWaitTimerInvalid(void);

extern void Oam_SetGetCfgVerInfoTimer(void);
extern void Oam_KillGetCfgVerInfoTimer(void);
extern void Oam_SetGetCfgVerInfoTimerInvalid(void);

extern void Oam_SetRegOamsNotifyTimer(void);
extern void Oam_KillRegOamsNotifyTimer(void);
extern void Oam_SetRegOamsNotifyTimerInvalid(void);

extern BOOL FindUserFromLink(CHAR *pUserName, USER_NODE **ppNode,USER_NODE **pNode);
extern BOOL AddUserToLink(USER_NODE *pNode);
extern BOOL DelUserFromLink(CHAR *pUserName);
extern BOOL CheckIsXMLAdapterLinkState(TYPE_LINK_STATE *ptLinkState);
extern void OamInterpretCheckPtr(BYTE *InputPtr, WORD32 dwLine);

extern BOOLEAN Xml_CfgCurLinkIsExcutingSave(TYPE_LINK_STATE *pLinkState);/*�жϵ�ǰ�����Ƿ���ִ�д���*/
extern INT OamCfgGetLinkStateType(LPVOID pMsgPara);
extern TYPE_LINK_STATE *OamCfgGetLinkState(WORD32 dwMsgId, LPVOID pMsgPara);
extern INT OamGetLinkStateTypeForPlan(LPVOID pMsgPara);
extern TYPE_XMLLINKSTATE *XmlCfgGetLinkState(WORD16 dwMsgId, LPVOID pMsgPara);
extern int XmlProcess_Return_Data(TYPE_XMLLINKSTATE *pLinkState,MSG_COMM_OAM * ptMsg);
extern void Oam_ShowAllVerInfo(void);
extern void Oam_ShowAllVerInfo(void);

extern BOOLEAN Oam_CfgCheckRecvMsgMinLen(WORD16 wMinLen);

extern BOOLEAN Oam_CfgCheckCommOamMsgLength(void);

extern BOOL AddOneUserToLink(TYPE_USER_INFO *pSSHUser);
extern BOOL UpdateOneUserToLink(TYPE_USER_INFO *pSSHUser);
extern BOOL FindUserInfoFromLink(CHAR *pUserName, TYPE_USER_INFO *pSSHUser);

extern BOOL InsertOneUseLog(TYPE_LINK_STATE *pLinkState);
extern BOOL UpdateOneUseLog(TYPE_LINK_STATE *pLinkState);

extern void OamMateRequestSubScr(void);
extern void OamRecvMateSubScr(LPVOID pMsgPara);
extern BOOL IsSameLogicAddr(JID *ptJid1, JID *ptJid2);
extern BOOL LogoutLinkStateToTelnet(CHAR *pUserName);
extern BOOL DeleteUserInfo(CHAR *pUserName);
extern BOOL GetUserConfViewNum(BYTE *pUserConfCnt,BYTE *pUserViewCnt);
extern BOOL CheckUsernameBlfOnline(CHAR *sUserName);
extern BOOL Oam_GetUserInfo(TYPE_USER_INFO *pUserInfo);
extern BOOL Oam_GetSpecifyIP(CHAR *pSpecifyIp);
extern BOOL Oam_SetSpecifyIp(CHAR *pSpecifyIp);
extern BOOL Oam_GetCommonInfo(COMMON_INFO *pCommonInfo);
extern BOOL Oam_SetCommonInfo(COMMON_INFO *pCommonInfo);
extern BOOL Oam_GetPasswordIntensity(BYTE *pPasswordIntensity);
extern BOOL Oam_SetPasswordIntensity(BYTE *pPasswordIntensity);
extern BOOL Oam_GetNorPasslesslen(BYTE *pNorPasslesslen);
extern BOOL Oam_SetNorPasslesslen(BYTE *pNorPasslesslen);
extern BOOL Oam_InnerCmdGetPtVarJidinfo(JID *jidSelf);
extern BOOL Oam_GetLangType(BYTE ucDatVerInfoNum,CHAR *aucLangType);
extern BOOL Oam_SetLangType(BYTE ucDatVerInfoNum,CHAR *aucLangType);
extern BOOL Oam_GetVerFileName(BYTE ucDatVerInfoNum,CHAR *aucVerFileName);
extern BOOL Oam_SetVerFileName(BYTE ucDatVerInfoNum,CHAR *aucVerFileName);
extern BOOL Oam_GetVerFilePath(BYTE ucDatVerInfoNum,CHAR *aucVerFilePath);
extern BOOL Oam_SetVerFilePath(BYTE ucDatVerInfoNum,CHAR *aucVerFilePath);
extern BOOL Oam_GetDatSum(WORD16 *wDatSum);
extern BOOL Oam_SetDatSum(WORD16 *wDatSum);
extern BOOLEAN OAM_CliSetLanguageType(CHAR *pStrLangType);
extern BOOLEAN OAM_CliGetLanguageType(T_Cli_LangType *ptLangType);

extern void OAM_CliChangeAllLinkToStartMode(CHAR *pcReason);
extern BOOLEAN Oam_CfgIfNoLinkIsExecutingCmd(void);

extern CHAR *OAM_RTrim(CHAR *pStr);
extern CHAR *OAM_LTrim(CHAR *pStr);

#ifdef WIN32
    #pragma pack()
#endif

#endif /* __INTERPRET_H__ */
