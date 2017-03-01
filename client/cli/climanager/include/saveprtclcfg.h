/*********************************************************************
* 版权所有 (C)2002, 深圳市中兴通讯股份有限公司。
*
* 文件名称：SavePrtclCfg.h
* 文件标识：
* 内容摘要：在线配置存盘
* 其它说明：
            
* 当前版本：
* 作    者       ：殷浩
* 完成日期：20080307
*
* 修改记录1：// 修改历史记录，包括修改日期、修改者及修改内容
*    修改日期：
*    版 本 号：
*    修 改 人：
*    修改内容：
* 修改记录2：…
**********************************************************************/
#ifndef __SAVE_PROTOCOL_CONFIG_H__
#define __SAVE_PROTOCOL_CONFIG_H__

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

#define OAM_CFG_MIN(A,B) (A) > (B) ? (B) :(A)

/*存盘命令ID*/
#define CMDID_SAVE_PROTOCOL_CFG    (DWORD)CMD_DEF_SET_SAVE
/*在线配置存盘目录*/
#ifdef WIN32
#define OAM_CFG_PRTCL_SAVE_ORG_DIR    "U:/DATA1"
#define OAM_CFG_PRTCL_SAVE_BAK_DIR    "U:/DATA2"
#else
  #ifdef OS_LINUX_PC
#define OAM_CFG_PRTCL_SAVE_ORG_DIR    "/home/DATA1"
#define OAM_CFG_PRTCL_SAVE_BAK_DIR    "/home/DATA2"
  #else
  #define OAM_CFG_PRTCL_SAVE_ORG_DIR    "/IDE0/DATA0"
  #define OAM_CFG_PRTCL_SAVE_BAK_DIR    "/IDE0/DATA1"
  #endif
#endif
#define OAM_CFG_PRTCL_SAVE_FILE_ORG   OAM_CFG_PRTCL_SAVE_ORG_DIR"/prtclcfgsave.txt"
#define OAM_CFG_PRTCL_SAVE_FILE_BAK   OAM_CFG_PRTCL_SAVE_BAK_DIR"/prtclcfgsave.txt"

#define OAM_MAX_RESTORE_LOG_LEN  1024
#define MAX_CMDFILE_NAME_LEN     (BYTE)30

#define OAM_SAVE_ERR_BEGIN       (WORD32)0
#define OAM_SAVE_SUCESS          OAM_SAVE_ERR_BEGIN + 0
#define OAM_SAVE_ERR_SEEK_END    OAM_SAVE_ERR_BEGIN + 1
#define OAM_SAVE_CMDLINE_ERR     (WORD32)0

#define OAM_CRC_MARK    "[CRC_"
#define OAM_DATCRC_MARK_IN_SAVEFILE    "[CRC_DATFILE] = "
#define OAM_TXTCRC_MARK_IN_SAVEFILE    "[CRC_TXTFILE] = "
#define OAM_NEXLINE_MARK    "\n\r"
#define OAM_VERSION_IN_SAVEFILE    "TXT_VERSION = "

/*txt文件校验和头部长度
    标识字符串 + 两个回车换行 + 2字节txt的16位校验和 + 32位dat校验和*/
#define OAM_TXT_CRC_HEAD_LEN (strlen(OAM_DATCRC_MARK_IN_SAVEFILE) + strlen(OAM_TXTCRC_MARK_IN_SAVEFILE) + 2*(strlen(OAM_NEXLINE_MARK)) + OAM_SIZEOF_DATCRC + OAM_SIZEOF_TXTCRC)

/*TXT文件版本号*/
#define OAM_TXT_VERSION_0  0  /*0 - 默认，没有指定版本时候为0*/
#define OAM_TXT_VERSION_1  1  /*1-字符串类型参数支持带空格，用引号括起，解析方法不同于版本0*/

/*无效文件描述符*/
#define OAM_INVALID_FD           NULL

/*存盘操作提示信息*/
#define OAM_SAVE_SHOW_DELORGZDB_WARN    "%Warn: delete zdb file:"OAM_CFG_PRTCL_SAVE_ORG_DIR" error!"
#define OAM_SAVE_SHOW_DELBAKZDB_WARN    "%Warn: delete zdb file:"OAM_CFG_PRTCL_SAVE_BAK_DIR" error!"

#define OAM_RESTORE_READ_CMD_ERR        "%Read commands from back up file error!"
#define OAM_RESTORE_PARSE_CMD_ERR       "%Parse command line error!"
#define OAM_RESTORE_UNKNOWN_ERR         "%Unknown error in restore process!"
#define OAM_RESTORE_PRTCL_CMD_OK        "%Restore protocol config data from back up file success!"

/*存盘函数返回值Oam_SaveCfgCmdLine函数用*/
#define OAM_SAVE_ERR_BEGIN        (WORD32)0
#define OAM_SAVE_SUCESS           OAM_SAVE_ERR_BEGIN + 0
#define OAM_SAVE_ERR_SEEK_END     OAM_SAVE_ERR_BEGIN + 1
#define OAM_SAVE_ERR_NULL_FD      OAM_SAVE_ERR_BEGIN + 2
#define OAM_SAVE_CMDLINE_ERR      (WORD32)0
extern WORD32 Oam_WriteStrToFile(XOS_FD *pSaveFileFd, CHAR *strCmdLines);

/*数据恢复预处理函数返回值,返回值引用oamcfg.h中定义的返回给brs的值*/
#define OAM_RESTORE_PREPARE_OK       OAM_TXT_SUCCESS
#define OAM_RESTORE_PREPARE_CRC_ERR  OAM_TXT_CRC_ERROR
#define OAM_RESTORE_PREPARE_RD_ERR   OAM_TXT_READ_ERROR
#define OAM_RESTORE_PREPARE_NO_DAT   OAM_TXT_NO_DAT
#define OAM_RESTORE_PREPARE_FILE_NOT_EXIST 20
#define OAM_RESTORE_PREPARE_OTHERERR       21
extern WORD32 Oam_RestorePrtclCfgPrepare(void);

/*读取行函数返回值*/
#define OAM_GET_NEXT_LINE_OK               0
#define OAM_GET_NEXT_LINE_OK_EOF           1
#define OAM_GET_NEXT_LINE_FAIL            2

extern void ProtocolCmdIDToOamCmdID(MSG_COMM_OAM *ptMsg);

extern void Oam_InitOneLinkForTxtRestore(TYPE_LINK_STATE *pLinkState);
extern void Oam_PrintRstrLog(CHAR *pcLog);
extern void Oam_CloseRestoreLogFile(void);
extern BYTE Oam_GetCurModeId(TYPE_LINK_STATE *pLinkState);
extern void RecvFromRestore(WORD32 dwMsgId, LPVOID pMsgPara, BOOLEAN bIsSameEndian);

extern CHAR *OAM_RTrim(CHAR *pStr);
extern CHAR *OAM_LTrim(CHAR *pStr);

extern BOOL Oam_GetDateStrFromSysTime(T_Time *ptInTime, CHAR *pcOutSoftClock, WORD32 dwBufSize);
extern BOOL Oam_GetTimeStrFromSysTime(T_Time *ptInTime, CHAR *pcOutSoftClock, WORD32 dwBufSize);
extern WORD32 Oam_Duration(T_Time tBegin, T_Time tEnd);

extern BOOL Oam_CheckDatFileIsExist(void);

void Oam_InitRestore(TYPE_LINK_STATE *pLinkState);
void Oam_RecvAppOfRestore(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *ptRcvMsg, JID tJid, BOOLEAN bIsSameEndian);
void Oam_ParseDollarCharOfRestore(TYPE_LINK_STATE *pLinkState);
void Oam_ParseExcalmatoryPointOfRestore(TYPE_LINK_STATE *pLinkState);
void Oam_ParseCmdLineOfRestore(TYPE_LINK_STATE *pLinkState, CHAR *pCmdLine);

#ifdef WIN32
    #pragma pack()
#endif

#endif /* __SAVE_PROTOCOL_CONFIG_H__*/



