/*********************************************************************
* 版权所有 (C)2002, 深圳市中兴通讯股份有限公司。
*
* 文件名称：Interpret.h
* 文件标识：
* 内容摘要：OAM解释进程
* 其它说明：
            
* 当前版本：
* 作    者：马俊辉
* 完成日期：2007.6.12
*
* 修改记录1：// 修改历史记录，包括修改日期、修改者及修改内容
*    修改日期：
*    版 本 号：
*    修 改 人：
*    修改内容：
* 修改记录2：…
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

/*MSG_COMM_OAM 结构中ucOamTag定义*/
#define OAMCFG_TAG_NORMAL                     0
#define OAMCFG_TAG_OFFLINE_TO_ONLINE          1
#define OAMCFG_TAG_TXT_RESTORE                2

#define OAM_STY_LINE_SIZE          512

#define S_Init          (WORD16)0
#define S_Work          (WORD16)1
#define S_UpdateDAT     (WORD16)2
#define S_CfgSave       (WORD16)3
#define S_Restore       (WORD16)4

/* 无效的终端号 */
#define OAM_INVALID_LINK_CHANNEL    0

/* 定时器 */
#define TIMER_IDLE_TIMEOUT              (TIMER_NO_1)
#define EV_TIMER_IDLE_TIMEOUT           (EV_TIMER_1)
#define DURATION_IDLE_TIMEOUT           (SWORD32)(60*1000)

/* 614000678048 (去掉24小时绝对超时) */
#if 0
#define TIMER_ABS_TIMEOUT               (TIMER_NO_2)
#define EV_TIMER_ABS_TIMEOUT            (EV_TIMER_2)
#define DURATION_ABS_TIMEOUT            (SWORD32)(60*1000)
#endif

#define TIMER_EXECUTE_NORMAL            (TIMER_NO_3)
#define EV_TIMER_EXECUTE_NORMAL         (EV_TIMER_3)
#define DURATION_EXECUTE_NORMAL         (SWORD32)(30*1000)

/* 定时扫描共享内存信息 */
#define TIMER_SCAN_REGSHM               (TIMER_NO_4)
#define EV_TIMER_SCAN_REGSHM            (EV_TIMER_4)
#define DURATION_SCAN_REGSHM            (SWORD32)(120*1000)

/* 版本切换时候先等待一会，
在此期间收到新的切换消息，重设定时器
超时后才开始加载版本*/
#define TIMER_DAT_UPDATE_WAIT         (TIMER_NO_5)
#define EV_TIMER_DAT_UPDATE_WAIT   (EV_TIMER_5)
#define DURATION_DAT_UPDATE_WAIT  (SWORD32)(5*1000)

/*从vmm获取dat信息重试次数*/
#define OAM_CFG_GETDATINFO_RETRY_TIMES  100
/* 获取dat版本信息定时器 */
#define TIMER_GET_CFGVERINFO               (TIMER_NO_6)
#define EV_TIMER_GET_CFGVERINFO         (EV_TIMER_6)
#define DURATION_GET_CFGVERINFO        (SWORD32)(5*1000)

/*向vmm注册dat版本超时重试次数*/
#define OAM_CFG_REGOAMS_NTF_RETRY_TIMES  OAM_CFG_GETDATINFO_RETRY_TIMES
/*等待vmm dat版本注册应答定时器*/
#define TIMER_REGOAMS_NTF_ACK                    (TIMER_NO_8)
#define EV_TIMER_REGOAMS_NTF_ACK              (EV_TIMER_8)
#define DURATION_REGOAMS_NTF_ACK             DURATION_GET_CFGVERINFO

/*加载新语言类型脚本定时器，用于加载失败的情况*/
#define TIMER_CLI_LOAD_SCRIPT_NEW_LANG     (TIMER_NO_9)
#define EV_TIMER_CLI_LOAD_SCRIPT_NEW_LANG    (EV_TIMER_9)
#define DURATION_CLI_LOAD_SCRIPT_NEW_LANG   (SWORD32)(5*1000)

/*应用上报命令执行进度条信息超时*/
#define TIMER_CMD_PLAN                 (TIMER_NO_11)
#define EV_TIMER_CMD_PLAN              (EV_TIMER_11)
#define DURATION_TIMER_CMD_PLAN        (SWORD32)(5*1000)

/*应用上报命令执行进度条信息超时(XML用)*/
#define TIMER_CMD_PLAN_XML                 (TIMER_NO_12)
#define EV_TIMER_CMD_PLAN_XML              (EV_TIMER_12)
#define DURATION_TIMER_CMD_PLAN_XML        (SWORD32)(5*1000)

/*备板向主板请求用户配置文件定时器*/
#define TIMER_REQ_USER_FILE                 (TIMER_NO_13)
#define EV_TIMER_REQ_USER_FILE              (EV_TIMER_13)
#define DURATION_TIMER_REQ_USER_FILE        (SWORD32)(10*1000)

/*XML适配器与网管握手定时器*/
#define TIMER_XML_OMM_HANDSHAKE      (TIMER_NO_20)
#define EV_TIMER_XML_OMM_HANDSHAKE  (EV_TIMER_20)
#define DURATION_XML_OMM_HANDSHAKE  (WORD16)(15*1000)

/*XML适配器与前台应用应答定时器*/
#define TIMER_EXECUTE_XMLADAPTER               (TIMER_NO_21)
#define EV_TIMER_EXECUTE_XMLADAPTER         (EV_TIMER_21)
#define DURATION_EXECUTE_XMLADAPTER         (SWORD32)(35*1000)


#define NEXTMODE_NORMAL       0      /*命令下一模式是正常的指定下一模式*/

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

/*最大命令行长度，恢复数据时候用*/
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

/*根据消息returncode判断是否是最后一包*/
#define OAM_MSG_IS_LAST_PACKET(pMsg)\
            (((pMsg)->ReturnCode != SUCC_CMD_NOT_FINISHED) && \
              ((pMsg)->ReturnCode != SUCC_CMD_DISPLAY_AND_KILLTIMER) && \
              ((pMsg)->ReturnCode != SUCC_CMD_DISPLAY_NO_KILLTIMER) && \
              ((pMsg)->ReturnCode != SUCC_WAIT_AND_HAVEPARA) )
		
/* define the structures */



/* information about the router */
typedef struct
{
    CHAR    sRawPrompt[MAX_PROMPT_LEN];   /* 原始提示符 */
    CHAR    sBanner[MAX_BANNER_LEN];      /* 欢迎信息 */
}_PACKED_1_ TYPE_ROUTER_INFO;

typedef enum
{
    LINKSTATE_TYPE_ERR = -1,/*链路状态类型错误*/
    LINKSTATE_TYPE_CLI,      /*CLI*/
    LINKSTATE_TYPE_XML      /*XML*/
}LINKSTATE_TYPE;/*链路状态类型*/

typedef struct tagEXEVTY
{
    JID     jidSender;
    BYTE    aucRcvMsg[MAX_APPMSG_LEN]; /*接收的消息存储区*/
    BYTE    aucRtnMsg[MAX_OAMMSG_LEN]; /*返回的消息存储区*/
}_PACKED_1_ VTY;

/*
#define   FILE_NAME_LENGTH 80
*/

/*******************************************************************************/
typedef struct tag_DAT_Link
{
    /* 索引表 */
    T_OAM_Cfg_Dat datIndex;
    /* 链表“指针” */
    WORD16 next;
}_PACKED_1_ T_DAT_LINK;
#define MAX_DAT_LINK_COUNT  100
/*******************************************************************************/

 #define    MAX_VER_INFO_NUM    (OAMS_VER_MAX * 3)
 
 /*cli语言类型结构体*/
typedef struct
{
    CHAR aucLangType[OAM_CLI_LANGTYPE_LEN];
    SWORD32 swLangOffSet;
}_PACKED_1_ T_Cli_LangType;

/*Dat文件信息*/
typedef struct{
    T_OamsVerInfo tFileInfo;
    T_Cli_LangType tLang; /*当前加载的dat语言类型*/
}_PACKED_1_ T_DatFileInfo;

/* 数据区结构体 */
typedef struct tagT_OAM_INTERPRET_VAR
{
    JID     jidSelf;
    JID     jidTelnet;
    JID     jidProtocol;
    
    BYTE    aucRecvBuf[MAX_APPMSG_LEN];
    BYTE    aucSendBuf[MAX_OAMMSG_LEN];
    CHAR    sOutputResultBuf[MAX_OUTPUT_RESULT];  /* 输出结果字符串 */
    CHAR    sCmdLineBuf[MAX_CMDLINE_LEN];         /* 回显的命令行 */
    CHAR    *pMemBufOri[MAX_CLI_VTY_NUMBER - MIN_CLI_VTY_NUMBER + 2];/*内部使用的内存块,最后一条分给XML Adpater*/
    CHAR    *pMemBufCur[MAX_CLI_VTY_NUMBER - MIN_CLI_VTY_NUMBER + 2];/*内存块当前指针*/

    /*SWORD32 swCfgFileFd;*/
    XOS_FD dwTxtFileFd;
    CHAR   *pcTxtFileBuf;
    CHAR   *pcTxtFilePos;
    BOOLEAN   bRestoreFirstPrint;                            /*是否是恢复的第一条打印*/
    WORD32 dwMillSecOnBrs;                                /*恢复期间brs处理所花毫秒数*/
    WORD32 dwMillSecOnParse;                              /*恢复期间解析处理所花毫秒数*/
    WORD32 dwMillSecOnPrepare;                            /*恢复期间准备处理所花毫秒数*/
    WORD32 dwMillSecOnReadTxt;
    WORD32 dwMillSecOnExit;
/*    T_OAM_Cfg_Dat oamCfgDat;                              OAM自己的DAT脚本*/
    T_CliAppReg         tPlatMpCliAppInfo;         /*非OMP上的平台业务进程注册的DAT信息 */
    T_CliAppAgtRegEx   tLompCliAppRegInfo;        /* 保存在OMP上的Lomp非平台业务进程注册的DAT信息 */
    WORD16 wDatSum;                                       /*版本管理表中所有dat文件数*/
    WORD32 dwModeSum;                                     /*所有dat文件中模式总数*/
    WORD32 dwTreeNodeSum;                                 /*所有dat文件中节点总数*/
    BYTE       ucCfgVerInfoGetTimes;                 /*dat版本信息获取尝试次数*/
    BYTE       ucRegOamsNtfTimes;                      /*向vmm注册dat切换通知尝试次数*/

    WORD16 wLinesOfCmdLogFile;                /* 命令行日志的行数 */
    
    XOS_TIMER_ID wGetCfgVerInfoTimer;        /*获取所有dat版本信息定时器*/
    XOS_TIMER_ID  wDatUpdateWaitTimer;       /* dat update 使用的定时器 */
    XOS_TIMER_ID wRegOamsNotifyTimer;        /*注册dat版本切换通知定时器*/

    T_DatFileInfo tDatVerInfo[MAX_VER_INFO_NUM];
    T_OamsVerInfo tBrsDatVerInfo;
    T_OAM_Cfg_Dat tCfgdat;
    BOOL bSupportStringWithBlank;              /*是否支持带空格的string类型，true-支持*/

    BYTE ucVerNotifyRegReqCnt;                 /* 由于VMM每次最多支持50个请求，所以定义这个脚本版本注册请求次数 */
    BYTE ucVerInfoReqCnt;                      /* 由于VMM每次最多支持50个请求，所以定义这个脚本信息请求次数 */
}_PACKED_1_ T_OAM_INTERPRET_VAR;
extern T_DAT_LINK *g_tDatLink;
extern WORD16 g_wRegDatCount;

/*******************************************************************************/

/*******************************************************************************/
/* 临时方案，应该使用inline函数 */
BOOL EqualJID(const JID* jid1, const JID* jid2);
/*******************************************************************************/

extern TYPE_LINK_STATE gtLinkState[MAX_CLI_VTY_NUMBER - MIN_CLI_VTY_NUMBER + 1];
extern TYPE_LINK_STATE gt_SaveLinkState;
extern TYPE_XMLLINKSTATE  gt_XmlLinkState[MAX_SIZE_LINKSTATE];

extern CHAR g_szSpecifyIP[LEN_IP_ADDR_STR];
extern BYTE   gnPasswordIntensity;     /*是否为强密码*/
extern BYTE   gnNorPasslesslen;          /*设置弱密码的最小程度*/
extern DWORD gnUserNum;                /* 所有的用户个数 */
extern TYPE_USER_INFO gtUserInfo[MAX_USER_NUM_ROLE_ADMIN + MAX_USER_NUM_ROLE_CONFIG + MAX_USER_NUM_ROLE_VIEW]; /* 所有的用户信息 */
extern CHAR  g_aucScriptLanguage[OAM_CLI_LANGTYPE_LEN]; /* 语言类型字符串*/

/*****************************************************************************/
typedef struct
{
    TYPE_TREE_NODE treenode;
    T_OAM_Cfg_Dat* cfgDat;
}_PACKED_1_ T_TreeNode;

/* 变量声明 */
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


/* 对外暴露接口 */
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

extern BOOLEAN Xml_CfgCurLinkIsExcutingSave(TYPE_LINK_STATE *pLinkState);/*判断当前连接是否在执行存盘*/
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
