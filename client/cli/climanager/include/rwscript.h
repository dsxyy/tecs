/*********************************************************************
* 版权所有 (C)2002, 深圳市中兴通讯股份有限公司。
*
* 文件名称：RWScript.h
* 文件标识：
* 内容摘要：读写脚本
* 其它说明：
            
* 当前版本：
* 作    者：wushg
* 完成日期：2007.6.5
*
* 修改记录1：// 修改历史记录，包括修改日期、修改者及修改内容
*    修改日期：
*    版 本 号：
*    修 改 人：
*    修改内容：
* 修改记录2：…
**********************************************************************/
#ifndef __RW_SCRIPT_H__
#define __RW_SCRIPT_H__

#include "oamscript.h"
#include "datreg.h"

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

/* script file */
#ifdef WIN32
#define OAM_FILE_DIR        "U:/OAM/"
/*#define OAM_CFG_SCRIPT_FILE "U:/OAM/zx3gplatcfg.dat"
#define OAM_CFG_SCRIPT_TEMP "U:/OAM/zx3gplatcfg.tmp"*/
#define OAM_CMD_SCRIPT_NEW_FILE  "U:/OAM/zx3gplatcmdnew.dat"
#define OAM_CMD_SCRIPT_OLD_FILE  "U:/OAM/zx3gplatcmdold.dat"
#else
#define OAM_FILE_DIR        "/home/oam"
/*#define OAM_CFG_SCRIPT_FILE OAM_FILE_DIR"/zx3gplatcfg.dat"
#define OAM_CFG_SCRIPT_TEMP OAM_FILE_DIR"/zx3gplatcfg.tmp"*/
#endif

#define OAM_CFG_DAT_CONFIG_FILE  "~oamdatcfg.ini"
#define OAM_CFG_METHOD_CONFIG_FILE  "client.ini"
#define MAX_LINE_STR_LEN 200

/*611000135193 pc机改为也从vmm获取版本*/
#if 0
/*pc机从配置文件获取dat版本信息，刀片从vmm获取*/
#ifdef OS_LINUX
    #define OAM_CFG_DAT_GET_FROM_VMM  /*dat文件信息从vmm获取*/
#else
    #undef OAM_CFG_DAT_GET_FROM_VMM  /*dat文件信息从配置文件获取*/
#endif
#endif
#undef OAM_CFG_DAT_GET_FROM_VMM  /*dat文件信息从vmm获取*/

/*pc机环境系统信息在oam.dat中，刀片在 vmm.dat中*/
#ifdef OAM_CFG_DAT_GET_FROM_VMM
    #define OAM_CFG_GETCOMMONINFO_FUNC_TYPE  FUNC_SCRIPT_DDMDAT_VMM
#else
    #define OAM_CFG_GETCOMMONINFO_FUNC_TYPE  FUNC_SCRIPT_CLIS_OAM
#endif

/*pc机环境dat文件有256字节vmm版本头，刀片环境没有*/
#ifdef  OAM_CFG_DAT_GET_FROM_VMM
    #define  OAM_VMM_FILE_HEAD_SIZE     0
#else
#define  OAM_VMM_FILE_HEAD_SIZE     0
#endif

/*cfg文件主备同步用的结构体*/
#define OAM_CFG_MAX_MSG_LEN        1024
typedef struct
{
	WORD16    wDataLen;
	BYTE      aucData[OAM_CFG_MAX_MSG_LEN];
}_PACKED_1_ MSG_MS_CFGFILE_SYN;

/*文件主备同步用的结构体*/
typedef struct
{
	WORD16    wDataLen;
	BOOLEAN   bLastPacket;      
	BYTE      aucData[1];
}_PACKED_1_ MSG_MS_FILE_SYN;

/* error code */
#define READERR_NONE        -1
#define READERR_SUCCESS     0
#define READERR_OPENFILE    1
#define READERR_READHEAD    2
#define READERR_FORMAT      3
#define READERR_VERSION     4
#define READERR_PHYBOARD    5
#define READERR_LOGICBOARD  6
#define READERR_OFFSET      7
#define READERR_FILESEEK    8
#define READERR_FREAD       9
#define READERR_MEM         10
#define READERR_CLOSEFILE   11

/* error code */
#define WRITEERR_SUCCESS    0
#define WRITEERR_CREATEFILE 1
#define WRITEERR_OPENFILE   2
#define WRITEERR_WRITEHEAD  3
#define WRITEERR_FILESEEK   4
#define WRITEERR_FWRITE     5
#define WRITEERR_COPYFILE   6
#define WRITEERR_CLOSEFILE  7
/*XML适配器错误码*/
#define MAP_TYPE_XMLADAPTER                     (WORD16)5
#define MAP_KEY_NO_FINDCMDINMODE           (WORD16)1 /*%Can not find command in the mode!*/
#define MAP_KEY_CHANGEMODEFORDAT          (WORD16)2 /*%Change to user mode, because DAT was changed!*/
#define MAP_KEY_CONFLICT_EXECUTECMD      (WORD16)3 /*%The other link is executing command, pls try later!*/
#define MAP_KEY_ERR_CONSTRUCTSAVECMD  (WORD16)4 /*%Xml Adapter:Construct telnet save command error!*/
#define MAP_KEY_STOP_CMDFORUPDATEDAT  (WORD16)5 /*%Command stoped for updating of dat version!*/
#define MAP_KEY_ERR_GETUB                         (WORD16)6 /*%Xml Adapter:Get Ub error!*/
#define MAP_KEY_ERR_XMLMSG2OAM              (WORD16)7 /*%Xml Adapter:Convert Xml message to Oam message error!*/
#define MAP_KEY_ERR_OAMMSG2XML              (WORD16)8 /*%Xml Adapter:Convert Oam message to Xml message error!*/
#define MAP_KEY_ERR_GETAPPJID                  (WORD16)9 /*%Xml Adapter:Get 3G Plat Application Jid error!*/
#define MAP_KEY_EXECUTING_CMD                 (WORD16)10 /*%The Xml link is excuting command,pls try later.*/
#define MAP_KEY_NO_REGISTEREDTRM           (WORD16)11 /*%The OMM have not registered trm id!*/
#define MAP_KEY_ERR_XMLLINKINFO              (WORD16)12 /*%The Xml Link State Error!*/
#define MAP_KEY_SUCCESS_SENDMSG             (WORD16)13 /*Send Message Success!*/
#define MAP_KEY_NO_CMDEXECUTING             (WORD16)14 /*No Any Command Executing!*/
#define MAP_KEY_ERR_EXECUTEOMMCMD        (WORD16)15 /*Execute Command Fail!*/
#define STRING_NOTFINDMAPKEY                    "%Can not find map value!"

#define MAX_PATH_LEN        100
extern CHAR g_aucScriptPath[MAX_PATH_LEN];

/* CLI命令提示 */
extern CHAR *CLI_szCLIHelp0;
extern CHAR *CLI_szCLIHelp1;
extern CHAR *CLI_szCLIHelp2;
extern CHAR *CLI_szCLIHelp3;
extern CHAR *CLI_szCLIHelp4;
extern CHAR *CLI_szCLIHelp5;
extern CHAR *CLI_szCLIHelp6;
extern CHAR *CLI_szCLIHelp7;
extern CHAR *CLI_szCLIHelp8;
extern CHAR *CLI_szCLIHelp9;
extern CHAR *CLI_szCLIHelp10;
extern CHAR *CLI_szCLIHelp11;
extern CHAR *CLI_szCLIHelp12;
extern CHAR *CLI_szCLIHelp13;
extern CHAR *CLI_szCLIHelp14;
extern CHAR *CLI_szCLIHelp15;
extern CHAR *CLI_szCLIHelp16;
extern CHAR *CLI_szCLIHelp17;
extern CHAR *CLI_szCLIHelp18;
extern CHAR *CLI_szCLIHelp19;
extern CHAR *CLI_szCLIHelp20;
extern CHAR *CLI_szCLIHelp21;
extern CHAR *CLI_szCLIHelp22;
extern CHAR *CLI_szCLIHelp23;
extern CHAR *CLI_szCLIHelp24;
extern CHAR *CLI_szCLIHelp25;

/* 字符串常量 */
extern CHAR *Hdr_szEnterString;
extern CHAR *Hdr_szUserName;
extern CHAR *Hdr_szPassword;
extern CHAR *Hdr_szCmdNoCmdTip;
extern CHAR *Hdr_szTelInputText;
extern CHAR *Hdr_szOsBadUserName;
extern CHAR *Hdr_szOsBadPassword;
extern CHAR *Hdr_szCanNotEnterConfig;
extern CHAR *Hdr_szEnterConfigTip;
extern CHAR *Hdr_szEnterConfigWarning;
extern CHAR *Hdr_szCanNotLogin;
extern CHAR *Hdr_szExeNoReturn;
extern CHAR *Hdr_szExeNoDisplay;
extern CHAR *Hdr_szDisplayScriptErr;
extern CHAR *Hdr_szExeTimeout;
extern CHAR *Hdr_szSlaveAckTimeout;
extern CHAR *Hdr_szTelRequestOutMode;
extern CHAR *Hdr_szCanNotChangMode;
extern CHAR *Hdr_szCeasedTheCmdTip;
extern CHAR *Hdr_szUpdateDatTip;
extern CHAR *Hdr_szNeNotSupportOnlineCfg;
extern CHAR *Hdr_szSavePrtclCfgOK;
extern CHAR *Hdr_szSavePrtclCfgFail;
extern CHAR *Hdr_szOpenSaveFileErr;
extern CHAR *Hdr_szInsertCrcErr;
extern CHAR *Hdr_szMSChangeoverTip;
extern CHAR *Hdr_szCommandMutexTip;
extern CHAR *Hdr_szSaveMutexTip;
extern CHAR *Hdr_szDatUpdateMutexTip;
extern CHAR *Hdr_szSlaveSaveOk;
extern CHAR *Hdr_szSlaveSaveFail;
extern CHAR *Hdr_szSaveMasterOnSlaveErr;
extern CHAR *Hdr_szCannotSaveOnSlave;
extern CHAR *Hdr_szUnknownSaveType;
extern CHAR *Hdr_szSaveZDBOK;
extern CHAR *Hdr_szSaveZDBFail;
extern CHAR *Hdr_szSaveZDBErrSaveBothPath;
extern CHAR *Hdr_szSaveZDBErrMainPath;
extern CHAR *Hdr_szSaveZDBErrBackup;
extern CHAR *Hdr_szSaveZDBErrLocalSaving;
extern CHAR *Hdr_szSaveZDBErrMSSaving;
extern CHAR *Hdr_szSaveZDBErrOther;
extern CHAR *Hdr_szSaveZDBErrNoTblSave;
extern CHAR *Hdr_szSaveZDBErrReadFileErr;
extern CHAR *Hdr_szSaveZDBErrTblNotNeedSave;
extern CHAR *Hdr_szSaveZDBErrPowerOnLoading;
extern CHAR *Hdr_szSaveZDBErrFtpLoading;
extern CHAR *Hdr_szSaveZDBErrAppFtpLoading;
extern CHAR *Hdr_szSaveZDBErrIOBusy;
extern CHAR *Hdr_szSaveZDBErrInvalidHandle;

extern CHAR *Hdr_szErrInvalidModule;

extern CHAR *Hdr_szNotAdmin;
extern CHAR *Hdr_szCannotDelAdmin;
extern CHAR *Hdr_szUserNotExists;
extern CHAR *Hdr_szUserBeenDeleted;
extern CHAR *Hdr_szCmdExecSucc;
extern CHAR *Hdr_szCmdExecFailed;
extern CHAR *Hdr_szCannotCreateAdmin;
extern CHAR *Hdr_szUserDoesExists;
extern CHAR *Hdr_szPasswordRule;
extern CHAR *Hdr_szPwdNotMeetRule;
extern CHAR *Hdr_szTwoInputPwdNotSame;
extern CHAR *Hdr_szConfUserFull;
extern CHAR *Hdr_szViewUserFull;
extern CHAR *Hdr_szUserLocked;
extern CHAR *Hdr_szCannotSetAdminRole;
extern CHAR *Hdr_szUserOnline;
extern CHAR *Hdr_szNeedNotSetRole;
extern CHAR *Hdr_szCannotSetOtherUserPwd;
extern CHAR *Hdr_szUserNotLocked;
extern CHAR *Hdr_szNoPermission;

/* 错误信息 */
extern CHAR *Err_szExeMsgHead;
extern CHAR *Err_szReturnCode;
extern CHAR *Err_szLinkChannel;
extern CHAR *Err_szSeqNo;
extern CHAR *Err_szLastPacket;
extern CHAR *Err_szOutputMode;
extern CHAR *Err_szCmdID;
extern CHAR *Err_szNumber;
extern CHAR *Err_szExecModeID;
extern CHAR *Err_szIfNo;
extern CHAR *Err_szFromIntpr;
extern CHAR *Err_szNoNeedTheCmd;
extern CHAR *Err_szTelMsgHead;
extern CHAR *Err_szMsgId;
extern CHAR *Err_szVtyNum;
extern CHAR *Err_szDeterminer;
extern CHAR *Err_szUndefined;

extern CHAR *map_szAppMsgDatalengthErr ;
extern CHAR *map_szUserBeenLocked ;
extern CHAR *map_szUserOrPwdErr ;
extern CHAR *map_szGotoSupperModeErr ;
extern CHAR *map_szExePlanFmtStr ;
extern CHAR *map_szScriptReloaded ;
extern CHAR *map_szCeaseCmdFailed ;
extern CHAR *map_szCmdSendAgainFailed ;
extern CHAR *map_szDisposeCmdFailed ;
extern CHAR *map_szExeCmdFailed ;
extern CHAR *map_szPrtclRestoreNotify ;

/* 对外暴露接口 */
/*extern int ReadCmdScript();*/
extern void OAM_CFGClearCmdScript(T_OAM_Cfg_Dat *ptCfgDat);
extern BOOL OAM_CFGInitGlobalDatMem(void);
extern BOOL OAM_CFGInitGlobalDatMemForRestore(WORD32 dwModeSum, WORD32 dwTreeNodeSum);

extern int ReadCfgScript();
extern void ClearCfgScript();
extern int SaveOamCfgInfo();
/*extern SWORD32 OAM_ReadCmdDatScript(BYTE  ucFileType);*/
extern BOOL Oam_RcvCfgFileFromMaster(LPVOID pMsgPara);
extern BOOL Oam_SendCfgFileToSlave(void);

extern void OAM_CFGRemoveAllDat(void);
extern WORD32 OAM_CFGGetTreeNodeCntInAllDat(void);
extern WORD32 OAM_CFGGetModeCntInAllDat(void);

extern INT32 ReadCmdScript(CHAR *pcFileName, T_OAM_Cfg_Dat *ptCfgDat, T_Cli_LangType *ptOutLangType);

extern BOOLEAN Oam_CfgGetDatVerInfoFromIniFile(void);
extern CHAR *ReadMapValueOfOamDat(WORD16 wMapType,WORD16 wMapKey);
extern CHAR *ReadMapValueOfCmmDat(WORD16 wMapType,WORD16 wMapKey);

extern BOOL InitUserLogFile(void);
extern BOOLEAN OamLoadUserFile(void);
extern void UseDefaultUserInfo(void);
extern BOOL LockUser(CHAR *pName);
extern BOOL UnLockUser(CHAR *pName);
extern BOOL WriteUserScriptFile(void);

extern BOOL OamSendUserfileToSlave(void);
extern BOOL OamRecvUserFileFromMaster(LPVOID pMsgPara);

extern BOOL OamGetPath(CHAR *pPathName, CHAR *pMatePath);
BOOLEAN OamUserFileExists(CHAR *pcFileName);

extern CHAR * GetMapString(WORD16 wType, WORD16 wValue);
extern BOOLEAN GetScriptFileList(char *CfgDir);

#ifdef WIN32
    #pragma pack()
#endif

#endif /* __RW_SCRIPT_H__ */


 
