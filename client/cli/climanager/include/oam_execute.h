/*********************************************************************
* 版权所有 (C)2002, 深圳市中兴通讯股份有限公司。
*
* 文件名称：oam_execute.h
* 文件标识：
* 内容摘要：OAM执行进程
* 其它说明：
            
* 当前版本：
* 作    者：马俊辉
* 完成日期：2007.6.5
*
* 修改记录1：// 修改历史记录，包括修改日期、修改者及修改内容
*    修改日期：
*    版 本 号：
*    修 改 人：
*    修改内容：
* 修改记录2：…
**********************************************************************/
#ifndef __OAM_EXECUTE_H__
#define __OAM_EXECUTE_H__

/***********************************************************
 *                     常量定义                            *
***********************************************************/
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

/*switch替换宏,case子句必须连续
替换前:
switch(pRevOprData->ParaNo)
{
case 0:
    printf("0");
    reak;
case 1:
    rintf("1");
    break;
case 2:
case 3:
case 4:
    rintf("2/3/4");
    break;
default:
    printf("unknown");
    break;
}
替换后:
SWITCH(pRevOprData->ParaNo)
{
CASE(1)
    printf("0");
    BREAK
CASE(1)
    printf("1");
    BREAK
CASE(3)
    printf("2/3/4");
    BREAK
DEFAULT
    printf("unknown");
    BREAK
}
END
*/

/* 替换原来的switch子句 */
#define SWITCH(x) \
{\
WORD16 __MY_INPARA__=(x); \
WORD16 __MY_PARANO__=0; \
WORD16 __MY_OFFSET__=0; \
do \
{ \
/* 替换原来的case子句, 表示连续n个case子句做相同的处理(n>0) */
#define CASE(n) \
for(__MY_OFFSET__=0;__MY_OFFSET__<(n);__MY_OFFSET__++) \
{ \
    if (__MY_INPARA__==(__MY_PARANO__+__MY_OFFSET__)) \
		break; \
} \
if (__MY_OFFSET__>=(n)) \
    __MY_PARANO__+=(n); \
else \
{ 
/* 替换原来的default子句 */
#define DEFAULT {
/* 替换原来的break子句 */
#define BREAK continue;}
/* 增加结束符(必须增加) */
#define END }while(1==2);}

#define S_Init                    (WORD16)0
#define S_Work                    (WORD16)1

/* 定时器 */
#define TIMER_READ_CMD_SCRIPT     (TIMER_NO_1)
#define EV_TIMER_READ_CMD_SCRIPT  (EV_TIMER_1)
#define DURATION_READ_CMD_SCRIPT  (WORD16)(30*1000)

#define PRO_CMD_BASE              (WORD16)10
#define INIT_PACKET_NO            (WORD16)1

#define ERR_EXE_INNER_CFG_SUCC    (WORD16)1
#define ERR_EXE_INNER_CFG_FAIL    (WORD16)2
#define ERR_EXE_INNER_REC_TOOMANY (WORD16)3

#define ERR_DBS_MASTER_SAVE_FAIL  (WORD16)200
#define ERR_DBS_SLAVE_SAVE_FAIL   (WORD16)201
#define ERR_DBS_MP_IS_BUSY        (WORD16)101
#define ERR_DBS_UNKNOWN_EVEN      (WORD16)255

#define SAVEINFO_LEN_MAX          150  /*存盘时携带的信息*/

#if (_CPU_BYTE_ORDER == _LITTLE_ENDIAN)
    typedef struct tagCMDID
    {
        WORD16    OID;   /* objectID */
        BYTE    OPM;   /* 操作模式 */
        BYTE    GRP;   /* 分类组 */
    }_PACKED_1_ CMDID;
#else
    typedef struct tagCMDID
    {
        BYTE    GRP;   /* 分类组 */
        BYTE    OPM;   /* 操作模式 */
        WORD16    OID;   /* objectID */
    }_PACKED_1_ CMDID;
#endif

typedef union tagT_CMDID
{
  DWORD   dwID;
  CMDID   tUid;
}_PACKED_1_ T_CMDID;

typedef struct tagT_PCMDMatch
{
  BYTE    ucBID;
  BYTE    ucPID;
}_PACKED_1_ T_PCMDMatch;

typedef struct tagT_OMCToBTSAttachInfo 
{ 
    JID      SourceJID;
}_PACKED_1_ T_OMCToBTSAttachInfo;

typedef	T_OMCToBTSAttachInfo T_PRODB_REQ;

#if 0
#define DB_NAME_LEN    (WORD16)32
#define DB_INSTANCE_NUM_MAX    (WORD16)64

typedef struct tagT_SaveDBItem
{
    BOOL         ucNeedSaveAllFlg; /* 本数据库是否全部表存盘标记：1：表示全部表存盘，0：表示增量表存盘*/
    BYTE         ucPad[3];       /*填充字节*/
    CHAR         dbName[DB_NAME_LEN];
}_PACKED_1_ T_SaveDB_Item;

/*CTRCR00113922 add by yinhao 增加增量存盘功能*/

typedef struct tagT_DBSMssaveReq
{
    BYTE     ucNeedSaveAllFlg; /* 需要全部表存盘标记*/
    JID      BackMgtJid;       /* 后台的JID*/
}_PACKED_1_ T_DbsBack_SaveReq;

typedef struct tagT_DBSMssaveReq
{
    JID                   BackMgtJid;         /* 后台的JID*/ 
    BOOL                   isAllDbSave; /*是否本CPU的所有库都存盘，1：本CPU的所有数据库都存盘;0：表示ucDatabaseNum和tDatabaseSave代表的数据库存盘*/
    BYTE                   ucPad;       /*填充字节*/
    WORD16               wDbNum;       /*本次存盘的数据库实例数目,目前一个CPU上最多64个数据库*/
    T_SaveDB_Item     tDatabaseSave[DB_INSTANCE_NUM_MAX];       /*本次存盘的数据库实例数目,目前一个CPU上最多64个数据库*/
}_PACKED_1_ T_DbsBack_SaveReq;
#endif

#define DM_GETBULOADINFO     	(WORD16) 5230	/*查询前台数据库备份局的当前激活局信息.暂定为5230，以后可能变化*/

typedef struct
{
    WORD16    wTransId;          /* 传输标识号 GES单板新增 */
    BYTE    ucNeedSaveAllFlg;  /* 需要全部表存盘标记*/
    JID     BackMgtJid;        /* 后台的JID*/
}_PACKED_1_ TDbsBack_SaveReq;

#define BUBAKLOAD_FAIL_TAB_NUM    (WORD16)5  /*用于加载失败表统计的数组定义，cFailTabName中使用，目前只提供前5个失败的表名*/    
#define BUBAKLOAD_DBNAMELEN       (WORD16)13 /* 表名最大长度12 */
typedef struct
{
    BYTE     ucResult;    /* 加载结果： 0加载成功，其它加载失败 */
    BYTE     ucReLoad;    /* 当加载失败时，触发源是否可以重新请求加载：  1 前台忙，可以重新加载，2不能  */
    WORD16     wFailTabNum; /*加载失败的表个数*/                        
    CHAR     strFailTabName[BUBAKLOAD_FAIL_TAB_NUM][BUBAKLOAD_DBNAMELEN];/*加载失败的表名*/
}_PACKED_1_ FtpSyncFailReason, *LPFtpSyncFailReason;

typedef struct
{
    WORD16     wRetCODE;      /* 返回码 */
    BYTE     ucBureauno;    /* 激活局号 */
    BYTE     ucPad;         /* 填充字段 */
    WORD16     wDbVersion;    /* 大版本号 */
    WORD16     wDbVersionRef; /* 小版本号 */
}_PACKED_1_ GETBULOADINFO_ACK, *LPGETBULOADINFO_ACK;

typedef struct
{
    WORD16    wTransId;         /* 传输标识号 */
    BYTE    ucMorS;           /* 1:Master, 2:Slave */
}_PACKED_1_ TSAVESTATE_REQ;

typedef struct
{
    BYTE     ucBureauno;     /* 预备倒换的局号 */
    BOOL     bMasterToSlave; /* 是否触发主备同步*/
    BOOL     bSave;          /* 是否触发存盘操作*/
    BOOL     bOmpToMp;       /* 网元内数据分发标志位 */
    DWORD    dwReserved[2];  /* 预留2个DWORD，待扩充，置0 */
}_PACKED_1_ TDbsFtpLoadBuInfo,  *LPTDbsFtpLoadBuInfo;

typedef struct
{
    BYTE     ucMsgType;  /* 消息类型,填为34，表示异步消息 */
    BYTE     aucPad[3];  /* 填充字段 */
}_PACKED_1_ GETBULOADINFO_REQ, *LPGETBULOADINFO_REQ;

typedef struct
{
    WORD16    wTransId;                      /*传输标识号*/
    CHAR    chSaveInfo[SAVEINFO_LEN_MAX];  /*附加存盘信息*/
    WORD16    wSaveStat;                     /*目前的存盘状态*/
    DWORD   dwDevId;                       /*设备ID，里面包含区号，局号信息，高8位为局号，次高16位为区号*/
    WORD16    wModule;                       /*模块号*/
    WORD16    wTblTotal;                     /*要存盘的总表数*/
    WORD16    wTblMDone;                     /*主目录上已存的的表数*/
    WORD16    wTblSDone;                     /*备目录上已存的的表数*/
}_PACKED_1_ TSAVESTATE_ACK;

/*执行相关的头文件*/
 /*void LoopReadCmdScript();*/
 /*extern void RecvFromInterpret(LPVOID pMsgPara, JID);*/
extern void ParseBrsTxtSaveData(TYPE_XMLLINKSTATE *pLinkState,LPVOID pMsgPara);
extern void ConstructBrsMapPara(MSG_COMM_OAM *ptMsgBuf);
extern void PrintMSGCOMMOAMStruct(MSG_COMM_OAM *ptMsg, CHAR *pDesc);
extern void Oam_RecvFromApp(TYPE_LINK_STATE *pLinkState,LPVOID pMsgPara);
extern void RecvFromDBS(WORD16 dwMsgId, LPVOID pMsgPara, CHAR *errStr, WORD32 dwBufSize, BOOLEAN bIsSameEndian);
extern void SendPacketToExecute(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *ptMsg, JID tJid);
extern void Oam_ExecSaveCmd(TYPE_LINK_STATE *pLinkState);

extern BOOL GetCmdLogFileLines(void);
extern BOOL WriteLogOfCmdLine(T_CliCmdLogRecord *pCmdLog, BOOL bCmdExecRslt);

extern void Oam_SetCmdPlanTimer(TYPE_LINK_STATE *pLinkState);
extern void Oam_KillCmdPlanTimer(TYPE_LINK_STATE *pLinkState);
extern void Oam_RecvPlanInfoFromApp(TYPE_LINK_STATE *pLinkState, LPVOID pMsgPara, BOOLEAN bIsSameEndian);
extern void Oam_DealPlanTimer(TYPE_LINK_STATE *pLinkState);
extern void Oam_InnerSaveCmdLompErr(LPVOID ptOamMsg,WORD32 dwMsgId, CHAR *pStrOutResult);
extern void Oam_InnerKillCmdPlanTimer(LPVOID ptOamMsg,WORD32 dwMsgId);
extern void Oam_InnerSetCmdPlanTimer(LPVOID ptOamMsg,WORD32 dwMsgId);

extern void SetMapOprData(MSG_COMM_OAM *ptMsg, OPR_DATA *pOprData, WORD16 v1, WORD16 v2);
extern DWORD GetMsgStructSize(MSG_COMM_OAM *ptMsg, OPR_DATA *pOprData);

extern void Oam_RecvFromCliApp(WORD32 dwMsgId,LPVOID pMsgPara);
extern void Oam_ProRecMsgFromCliApp(TYPE_LINK_STATE *pLinkState,LPVOID pMsgPara);
extern void Oam_ProRecMsgToOmm(LPVOID pMsgPara);
extern BOOLEAN IfModuleIsLomp(WORD16 wModule);
extern INT Oam_String_Compare_Nocase(const CHAR *s1, const CHAR *s2,INT n);
extern void CfgMsgTransByteOder(MSG_COMM_OAM *ptr);

extern BOOL IsSaveCmd(DWORD CmdID);

/*检查命令ID*/
extern BYTE CheckGetOrSet(DWORD CmdID);
#ifdef WIN32
    #pragma pack()
#endif

#endif /*__OAM_EXECUTE_H__*/

