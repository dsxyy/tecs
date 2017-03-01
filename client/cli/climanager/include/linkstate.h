/*********************************************************************
* 版权所有 (C)2002, 深圳市中兴通讯股份有限公司。
*
* 文件名称：linkstate.h
* 文件标识：
* 内容摘要：OAM解释进程
* 其它说明：
            
* 当前版本：
* 作    者：殷浩
* 完成日期：2008.10.30
*
* 修改记录1：// 修改历史记录，包括修改日期、修改者及修改内容
*    修改日期：
*    版 本 号：
*    修 改 人：
*    修改内容：
* 修改记录2：…
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
#define MAX_MORE_LAYER        200 /* 对于MoreAngle语法嵌套可由有100/2层,因为层数索引每次递增为2 */

#define MAX_VERIFYFAILURE     3

#define MAX_CMDLINE_LEN       OAM_STY_LINE_SIZE /*和OAM_STY_LINE_SIZE 一致*/
#define MAX_OUTPUT_RESULT     1024 * 10 
#define MAX_MEM_BUF           1024 * 10 
#define MAX_VTY_NUMBER        7
/*XML适配器专用*/
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
    BYTE    bDeterminer;            /* 匹配限定词 */
    BYTE    bHasNo;                 /* 有NO修饰符 */
    WORD16    wWordSum;               /* 子串个数 */
    BYTE         ucPad[2];
    WORD16    wBlankSum[MAX_LAYER];   /* 空格计数 */
    CHAR    sCmdWord[MAX_LAYER][MAX_STRING_LEN];/* 命令字&参数 */
}_PACKED_1_ TYPE_CMD_LINE;

/*命令脚本*/
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
    /* 进程号 */
    JID  tJid;
}_PACKED_1_ T_OAM_Cfg_Dat;

typedef struct
{
    WORD16    wPosition;     /* 命令树节点/参数的位置*/
    WORD16    wType;         /* 类型 */
    BYTE    bMatched;      /* 已经被匹配上 */
    BYTE    ucPad[3];
    CHAR    *pName;        /* 名字 */
    CHAR    *pComment;     /* 注释 */
    T_OAM_Cfg_Dat * cfgDat;
}_PACKED_1_ TYPE_HINT;     /* 提示信息 */

typedef struct
{
    WORD16    wPosition;     /* 匹配位置 */
    WORD16    wType;         /* 类型 */
    T_OAM_Cfg_Dat * cfgDat;
}_PACKED_1_ TYPE_PATH;     /* 路径信息 */

typedef struct
{
    WORD16    ParaNo;        /* Prarmeter Code Number */
    CHAR   sName[MAX_PARANAME];       /* 参数名称 */
    BYTE    Type;          /* Prarmeter Type */
    BYTE    Len;           /* Prarmeter Length, If Type equal to DATA_TYPE_TABLE, it represent the number of the records */
    BYTE    *pValue;
}_PACKED_1_ CMDPARA;

typedef struct
{
    /* for command */
    BYTE    bHasNo;           /*  有NO修饰符 */
    WORD16    wSum;                 /* 匹配数目 */
    WORD16    wDepth;               /* 匹配深度 */
    WORD16    wLength;              /* 匹配长度 */
    WORD16    wFirst;               /* 匹配最终位置 */
    WORD16    wCmdAttrPos;          /* 命令属性索引 */
    /* for parameter */
    WORD16    wParaSum;             /* 命令参数个数, 匹配上的数目，完全匹配上时(dExeID != 0),是参数总数 */
    BYTE    bParaNum;             /* 命令参数编号 */
    WORD16    wCurParaPos;          /* 参数匹配位置 */
    CMDPARA tPara[MAX_PARASUM];   /* 命令参数属性 */
    WORD16    wMaxDepth;            /* 保存旧的值 */
    WORD16    wMaxLength;           /* 保存旧的值 */
    WORD16    wMaxFirst;            /* 保存旧的值 */
    WORD16    wMaxCurParaPos;       /* 保存旧的值 */
    /* for hint */
    TYPE_HINT tHint[MAX_HINTSUM]; /* 匹配/提示信息栈  */
    BOOLEAN    bCanExecute;          /* 是否可以执行(包含<CR>)*/
    WORD16    wHintStackTop;        /* 信息栈顶指针 */
    /* for para pos */
    WORD16    wParaBeginPos;
    WORD16    wParaEndPos;
    /*  for {[][][]} and recycle*/
    BYTE    bMoreLayerTop;                   /* MoreAngle语法嵌套栈顶 */
    WORD16    wMoreBeginPos[MAX_MORE_LAYER];   /* 语法起始位置 */
    WORD16    wMoreEndPos[MAX_MORE_LAYER];     /* 语法结束位置 */
    BYTE    bType[MAX_MORE_LAYER];           /* 语法类型5(6) */
    /*  for {[][][]} */
    BYTE    bMoreFlags[MAX_MORE_LAYER];      /* 针对当前语法的选项设置 */
    BOOLEAN    bContinueSearch[MAX_MORE_LAYER]; /* 是否往后搜索 */
    TYPE_PATH tMatchedPath[MAX_PARASUM];     /* 以匹配过的信息 */
    WORD16    wMatchedPathTop;                 /* 信息栈顶指针 */
    /* for recycle */
    BOOLEAN    bPrammarRecycle;
    /* result */
    WORD16    wParseState;          /* 命令行语法分析状态 */
    TYPE_PATH tPath[MAX_LAYER];   /* 匹配路径 */
    WORD16    wPathStackTop;        /* 匹配路径栈顶指针 */
    WORD16    wCliMatchLen;         /* CLI匹配长度 */
    DWORD   dExeID;               /* 命令执行ID */
    T_OAM_Cfg_Dat * cfgDat;
}_PACKED_1_ TYPE_MATCH_RESULT;

typedef struct tag_TYPE_LINK_STATE
{
    BYTE    ucLinkID;                                /* 连接标识 */
    BYTE    ucCurRunState;                       /* 当前运行状态 */
	
    CHAR    tnIpAddr[LEN_IP_ADDR_STR];              /* 保存Telnet终端IP地址 */
    TYPE_TEXT_INPUT tInputControl;            /*用户配置欢迎信息，输入换行数据专用*/

    CHAR    sUserName[MAX_USERNAME_LEN];       /* 用户名 */
    CHAR    sPassword[MAX_OAM_USER_PWD_ENCRYPT_LEN];       /* 用户密码 */
    CHAR    sPasswordOrg[MAX_PASSWORD_LEN];       /* 未加密的密码*/
    CHAR    sSession[MAX_SESSION_LEN];
    CHAR    sSavedUser[MAX_USERNAME_LEN];      /* 切换用户时保存用户名 */
    BYTE    ucUserRight;                            /* 用户权限 */
    BYTE    ucSavedRight;                          /* 切换用户时保存用户权限 */
    BYTE    ucInputErrTimes;                    /* 密码错误次数 */

    BOOLEAN    bTelnetQuit;                       /* 用户从Telnet终端退出  */
    BOOLEAN    bRecvAsynMsg;                   /*正在处理的是异步消息*/
	
    XOS_TIMER_ID  wExeTimer;                         /* 等待Execute端超时定时器 */
    /* 614000678048 (去掉24小时绝对超时) */
    /*XOS_TIMER_ID  wAbsTimer;*/                         /* Telnet端绝对超时定时器 */
    XOS_TIMER_ID  wIdleTimer;                        /* Telnet端空闲超时定时器 */
    XOS_TIMER_ID  wCmdPlanTimer;                         /* 命令执行进度条超时定时器 */
    WORD16            wAbsTimeout;                     /* Telnet端绝对超时秒数 */
    WORD16            wIdleTimeout;                    /* Telnet端空闲超时秒数 */

    DWORD     dwExeSeqNo;                        /* 和Execute交互的消息流水号 */
    WORD16    wTelSeqNo;                           /* 和Telnet交互的消息流水号 */

    BYTE    LastData[MAX_STORE_LEN];           /* 保存业务使用的存储区*/
    BYTE    BUF[MAX_STORE_LEN];                   /* 保存业务使用的存储区*/

    BOOLEAN bChangeMode;                     /* 需要更改模式 */
    BOOLEAN bTelInputModeIsSyn;          /*控制telnetsvr输入模式是异步还是同步，debug用*/
    BOOLEAN bTelInputModeChanged;     /*telnetsvr输入模式是否发生变化*/
    BYTE        ucInputState;                         /* 输入状态 ,输入用户名、密码等状态*/
    BOOLEAN bInputStateChanged;          /* 输入状态是否发生了变化*/
    CHAR       sPrompt[MAX_PROMPT_LEN];   /* 提示符 */
	
    BYTE    bModeID[MAX_MODESTACK];     /* 模式ID堆栈 */
    BYTE    bModeStackTop;                       /* 堆栈顶指针 */

    WORD16   wAppMsgReturnCode;         /*app的消息返回码*/
    BOOLEAN  bOutputResult;                    /* 需要输出执行结果 */
    BYTE        bOutputMode;                      /* 输出模式 */
    CHAR       *sOutputResult;                   /* 输出结果字符串 */
    WORD16   wSendPos;                          /* 输出的位置 */

    BOOLEAN bOutputCmdLine;                /* 需要回显命令行 */
    CHAR      *sCmdLine;                          /* 回显的命令行 */

    TYPE_CMD_LINE         tCmdLine;                /* 命令行信息 */
    TYPE_MATCH_RESULT tMatchResult;            /* 匹配结果 */
    WORD16                    wCmdAttrPos;                      /* 命令属性索引 */

    TYPE_DISP_GLOBAL_PARA  tDispGlobalPara;    /* 回显部分的全局变量及静态变量 */

    BOOLEAN bLogFileOK;                             /*该链接日志文件是否可用*/
    BOOLEAN bIsSupperUsrMode;                 /*当前用户是否是超级用户模式下*/

    T_CliCmdLogRecord       tCmdLog;          /* 命令行操作日志 */

    BYTE    ucPlanValuePrev;                    /* 保存当前进度值，当5s超时之后需要用上 */

    BOOLEAN    bSSHConnRslt;                      /* SSH连接结果 */
    BOOLEAN    bSSHAuthMsg;                       /* 是否为SSH登录消息 */
    BOOLEAN    bSSHAuthRslt;                      /* SSH登录结果 */

    BOOLEAN bRestartReqSended;
    TYPE_USER_LOG    tUserLog;                 /* 用户登录登出日志 */
    
    BOOLEAN    CmdRestartFlagForXmlRpc;
}_PACKED_1_ TYPE_LINK_STATE;

/*XML适配器专用*/
typedef struct tagT_TYPE_CUR_CMDINFO
{
    WORD16 wCmdAttrPos;  /* 命令属性索引 */
    WORD16  wParaBeginPos;    /* 命令属性索引起始位置*/    
    WORD16  wParaEndPos;       /* 命令属性索引结束位置*/
    BYTE bIsNoCmd;                   /*是否NO命令*/
    BYTE ucPad[1];
    T_OAM_Cfg_Dat * cfgDat;/* 命令所在dat结构指针*/
}_PACKED_1_ TYPE_CUR_CMDINFO;/*当前命令信息*/

typedef struct tagT_TYPE_XMLLINKSTATE
{
    XOS_TIMER_ID  wExeTimer;                         /* 等待Execute端超时定时器 */
    XOS_TIMER_ID  wCmdPlanTimer;                         /* 命令执行进度条超时定时器 */
    DWORD  dwSeqNo;                                              /*同业务交互的发送序号*/
    DWORD dwCmdId;                                               /*命令ID*/
    WORD32  dwLinkChannel;                                     /*链路号*/
    BYTE aucPad[2];
    BYTE ucComponentId[2];                                    /*组件号*/	   
    TYPE_CUR_CMDINFO tCurCMDInfo;                      /*当前命令信息*/ 
    CHAR xmlTSecText[MAX_SIZE_TSECTION_TEXT];  /*XML字符串T段*/        
    CHAR xmlDeclText[MAX_SIZE_DECL_TEXT];         /*XML声明部分*/
    BYTE    LastData[MAX_STORE_LEN];           /* 保存业务使用的存储区*/
    BYTE    BUF[MAX_STORE_LEN];                   /* 保存业务使用的存储区*/
    JID tDtJid;                                                            /*当前返回目标JID*/    
    TYPE_DISP_GLOBAL_PARA  tDispGlobalPara;       /* 回显部分的全局变量及静态变量 */
    MSG_COMM_OAM *ptLastSendMsg;                     /*上一次发送的OAM消息体指针*/
    BYTE    bModeID[MAX_MODESTACK];     /* 模式ID堆栈 */
    BYTE    bModeStackTop;                       /* 堆栈顶指针 */    
    BYTE bUsed;                                                        /*是否使用 0-未使用 1-使用*/   
    BYTE    ucCurRunState;                       /* 当前运行状态 */
    BYTE    bVerChanged;                         /*版本切换标志*/
    BYTE    ucPlanValuePrev;                    /* 保存当前进度值，当5s超时之后需要用上 */
}_PACKED_1_ TYPE_XMLLINKSTATE;/*XML连接状态*/

/* 当前运行状态*/
#define pec_MsgHandle_IDLE    0    /* 空状态 */
#define pec_Init_USERNAME     10   /* 用户名输入及验证状态 */
#define pec_Init_PASSWORD     20   /* 密码输入及鉴权状态 */

#define pec_MsgHandle_INIT        100  
#define pec_MsgHandle_TELNET   110  /* Telnet端执行状态 */
#define pec_MsgHandle_EXECUTE 120  /* Execute端执行状态 */

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
/* 614000678048 (去掉24小时绝对超时) */
#if 0
extern void Oam_SetAbsTimer(TYPE_LINK_STATE *pLinkState);
#endif
extern void Oam_SetExeTimer(TYPE_LINK_STATE *pLinkState);
extern void Xml_SetExeTimer(TYPE_XMLLINKSTATE *pLinkState);
extern void Xml_KillExeTimer(TYPE_XMLLINKSTATE *pLinkState); 
extern void Oam_KillIdleTimer(TYPE_LINK_STATE *pLinkState);
/* 614000678048 (去掉24小时绝对超时) */
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
/*XML适配器专用*/
extern BOOLEAN XML_CheckCurCmdIsNo(TYPE_XMLLINKSTATE *ptLinkState);
extern void XML_ChangeMode(TYPE_XMLLINKSTATE *pLinkState);/*改变模式*/
extern BYTE XML_GetCurModeId(TYPE_XMLLINKSTATE *ptLinkState);/*获取当前模式ID */
extern void XML_LinkModeStack_Push(TYPE_XMLLINKSTATE *pLinkState,  BYTE ucModeId);/*模式ID压栈*/
extern void XML_LinkModeStack_Pop(TYPE_XMLLINKSTATE *pLinkState);/*模式ID出栈*/
extern BOOLEAN XML_LinkModeStatck_IsEmpty(TYPE_XMLLINKSTATE *pLinkState);/*判断模式ID是否为空*/
extern TYPE_XMLLINKSTATE *Xml_GetLinkStateByLinkChannel(WORD32 dwLinkChannel);/*获取XML链路信息*/
extern TYPE_XMLLINKSTATE *Xml_GetCurUsedLinkState(void);
extern void Xml_InitLinkState(void);/*初始化链路状态*/
extern INT Xml_GetIndexOfLinkState(WORD32 dwLinkChannel);/*获取XML链路状态下标*/
extern TYPE_XMLLINKSTATE *Xml_GetLinkStateByIndex(WORD16 wLinkStateIndex);/*根据XML链路下标获取链路状态*/
extern TYPE_XMLLINKSTATE *XML_GetUsableLinkState(void);/*获取可用XML链路信息*/
extern TYPE_XMLLINKSTATE *Xml_AssignLinkState(WORD32 dwLinkChannel);/*申请XML链路状态*/
extern BOOLEAN XML_LinkGetModeID(WORD16 wLinkStateIndex,BYTE *pModeID);
extern BOOLEAN XML_LinkSetModeID(WORD16 wLinkStateIndex,BYTE *pModeID);
extern BOOLEAN XML_LinkGetModeStackTop(WORD16 wLinkStateIndex,BYTE *pModeStackTop);
extern BOOLEAN XML_LinkSetModeStackTop(WORD16 wLinkStateIndex,BYTE *pModeStackTop);
#if 0
/* 定时器 */
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

