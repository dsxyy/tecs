/*********************************************************************
* 版权所有 (C)2002, 深圳市中兴通讯股份有限公司。
*
* 文件名称：OamScript.h
* 文件标识：
* 内容摘要：OAM脚本结构体
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
* 修改记录2：
**********************************************************************/
#ifndef __OAM_SCRIPT_H__
#define __OAM_SCRIPT_H__

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

/* 命令脚本部分 */
#define MARK_BLANK        ' '
#define MARK_STRINGEND    '\0'
#define MARK_VIRGULE      '/'
#define MARK_ZERO         '0'
#define MARK_NINE         '9'
#define MARK_POINT        '.'
#define MARK_HYPHEN       '-'
#define MARK_COLON        ':'
#define MARK_RETURN       '\n'
#define MARK_POS          '^'
#define MARK_TYPE_STRING  '"'           /*字符串类型中有空格时候，字符串要用引号*/
#define MARK_STRING_TRANS '\\'   /*字符串类型中的转义符*/
#define MARK_CHAR_QUESTION '?'      /*由于问号也是特殊的，需要转义*/

#define STR_CMD_NO        "no"

#define READ_CMD_SCRIPT_ERR   "\n\r%The DAT file which contains CLI map type haven`t registed, can not use OAM config shell now!\n"

#define PATH_INVALID      0   /* 无效值 */
#define PATH_COMMAND      10  /* 命令字 */
#define PATH_PARAMETER    20  /* 参数 */
#define PATH_TITLE        30  /* MAP, {}或者{[][][]}的抬头 */

#define INVALID_MODENO    0
#define INVALID_NODE      0
#define INVALID_CMDINDEX  0
#define INVALID_PARAINDEX 0
#define INVALID_SHOWINDEX 0
#define INVALID_EXEID     0

/* bPrammarType == 1, 2 */
#define PRAMMAR_BRACKET_SQUARE  1 /* [] */
#define PRAMMAR_BRACKET_ANGLE   2 /* {} */
#define PRAMMAR_WHOLE_SYMBOL    3 /* WHOLE: 连续的若干参数做一个整体 */
#define PRAMMAR_WHOLE_NOSYM     4 /* WHOLE: 无SYMBOL */
#define PRAMMAR_RECYCLE         5
#define PRAMMAR_MORE_ANGLE      6   /* {[][][]} */

#define FILE_FORMAT         0xFFFFFFFF
#define FILE_VERSION        0x00020000 /*0x00020000*/
#define FILE_VERSION_OLD    0x00010000

#define MAX_NODENAME        30 
#define MAX_NODECOMMENT     100 /*50*/ 
#define MAX_PARASYMBOL      MAX_PARANAME
#define MAX_PARACOMMENT     100 /*50 */

#define MAX_MODEHELP        80    /* 包括\n\0的长度 */

#define ATTR_FLAG_HASNO     0x01
#define ATTR_FLAG_CANCEASE  0x02
#define ATTR_FLAG_NEEDACK   0x04
#define ATTR_FLAG_ONMENU    0x08
#define ATTR_FLAG_CANCEASE_NEEDACK    0x10

/*中止属性*/
#define CEASE_ATTR_CANCEASE_NEEDACK    0
#define CEASE_ATTR_CANCEASE_NOACK      1
#define CEASE_ATTR_CANNOTCEASE         2

#define CMD_ATTR_HASNO(_P_)    (((_P_)->bAttrFlags & ATTR_FLAG_HASNO) != 0)
#define CMD_ATTR_CANCEASE(_P_) (((_P_)->bAttrFlags & ATTR_FLAG_CANCEASE) != 0)
#define CMD_ATTR_NEEDACK(_P_)  (((_P_)->bAttrFlags & ATTR_FLAG_NEEDACK) !=0)
#define CMD_ATTR_ONMENU(_P_)   (((_P_)->bAttrFlags & ATTR_FLAG_ONMENU) !=0)
#define CMD_ATTR_CANCEASE_NEEDACK(_P_) (((_P_)->bAttrFlags & ATTR_FLAG_CANCEASE_NEEDACK) != 0)

#define OAM_CLI_LANGTYPE_OLDFMT     "unknown"
#define OAM_CLI_LANGTYPE_ENGLISH    "english"
#define OAM_CLI_LANGTYPE_CHINESE    "chinese"
/*#define OAM_CLI_LANGTYPE_LEN            12*/

/* file head */
typedef struct tagTYPE_FILE_HEAD
{
    DWORD    dFormatDef;       /* FormatDef == 0xFFFFFFFF,否则非法 */
    DWORD    dVersion;         /* 命令脚本文件版本   */
    DWORD    dData;            /* 命令脚本文件制作日期 */
    DWORD    dTime;            /* 命令脚本文件制作日期 */
    DWORD    dPhyBoardType;    /* 命令脚本文件物理板类型 */
    DWORD    dLogicBoardType;  /* 命令脚本文件逻辑板类型 */
    /*DWORD    dPad[6];       */   /* 命令脚本文件头预留扩展 */
    DWORD    dwCurLangDatSize; /*当前语言类型的DAT文件大小*/
    BYTE        aucLanguageVer[OAM_CLI_LANGTYPE_LEN]; /* 语言类型字符串,中文：Chinese;英文：English */
    DWORD    adwPad2[2];       /* 保留字段2 */
    DWORD    dModeBegin;       /* 模式部分偏移量 */
    DWORD    dCmdTreeBegin;    /* 命令树部分偏移量 */
    DWORD    dCmdAttrBegin;    /* 命令属性部分偏移量 */
    DWORD    dCmdParaBegin;    /* 参数属性部分偏移量 */
    DWORD    dFormatBegin;     /* 显示格式部分偏移量 */
    DWORD    dMapTypeBegin;    /* 转换表类型部分偏移量 */
    DWORD    dMapItemBegin;    /* 转换表项部分偏移量 */
    DWORD    dMapStrBegin;     /* 转换值字符串偏移量 */
}_PACKED_1_ TYPE_FILE_HEAD;

/* mode */
typedef struct tagTYPE_MODE
{
    BYTE     bModeID;          /* 模式ID     */
    WORD16     wModeClass;       /* 该模式的级别   */
    WORD16     wTreeRootPos;     /* 命令树根位置   */
    BYTE     bHasNo;           /* 该模式命令是否有NO命令 */
    BYTE     bReserved;        /* 保留字节     */
    CHAR     sOddPrompt[MAX_PROMPT_LEN];   /* 附加的提示符   */
    CHAR     sModeHelp[MAX_MODEHELP];  /* 该模式的帮助信息 */
}_PACKED_1_ TYPE_MODE;

/* tree node*/
typedef struct tagTYPE_TREE_NODE
{
    WORD16     wNextNodePos;         /* 下一个节点位置 */
    WORD16     wNextSonPos;          /* 下一层节点位置 */
    WORD16     wCmdAttrPos;          /* 命令属性索引   */
    CHAR     sName[MAX_NODENAME];       /* 节点名称     */
    CHAR     sComment[MAX_NODECOMMENT]; /* 节点注释     */
}_PACKED_1_ TYPE_TREE_NODE;

/* command attribution */
typedef struct tagTYPE_CMD_ATTR
{
    DWORD dExeID;           /* 命令执行ID   */
    BYTE  bExeModeID;       /* 命令执行模式   */
    BYTE  bNextModeID;      /* 下一个执行模式ID */
    BYTE  bNextInputModeID; /* 下一个输入模式ID */
    BYTE  bExePos;          /* 命令执行位置   */
    BYTE  bAttrFlags;       /* 是否有否命令:0x01, 是否可中止:0x02, 是否需要返回信息:0x04, 是否可以放入菜单:0x08 */
    WORD16  wAllParaCount;    /* 参数总数(CMD + NO)*/
    DWORD dwParaPos;        /* 参数列表的位置 */
    WORD16  wCmdParaCount;    /* 命令参数个数   */
    WORD16  wNoParaCount;     /* NO命令参数个数 */
    WORD16  wToShowLen;       /* 显示信息格式长度 */
    DWORD dwToShowPos;      /* 显示信息的位置 */
}_PACKED_1_ TYPE_CMD_ATTR;

/* command parameter */
typedef struct tagTYPE_PARA
{
    BYTE    bParaType;    /* 参数类型   */
    WORD16    wLen;         /* 参数长度   */
    union
    {
        WORD64   qdwMinVal; /* WORD64参数值的下限 */
        float    fMinVal; /* float参数值的下限 */
    }Min;
    union 
    {
        WORD64   qdwMaxVal; /* WORD64参数值的上限 */
        float    fMaxVal; /* float参数值的上限 */
    }Max;
    CHAR    sName[MAX_PARANAME];       /* 参数名称 */
    CHAR    sComment[MAX_PARACOMMENT]; /* 节点注释 */
}_PACKED_1_ TYPE_PARA;

typedef struct tagTYPE_PARA_OLD
{
    BYTE    bParaType;    /* 参数类型   */
    WORD16    wLen;         /* 参数长度   */
    union
    {
        WORD32   dwMinVal; /* WORD64参数值的下限 */
        float    fMinVal; /* float参数值的下限 */
    }Min;
    union 
    {
        WORD32   dwMaxVal; /* WORD64参数值的上限 */
        float    fMaxVal; /* float参数值的上限 */
    }Max;
    CHAR    sName[MAX_PARANAME];       /* 参数名称 */
    CHAR    sComment[MAX_PARACOMMENT]; /* 节点注释 */
}_PACKED_1_ TYPE_PARA_OLD;

typedef struct tagTYPE_PRAMMAR
{
    BYTE     bPrammarType;   /* 语法类型   */
    BYTE     bItemCount;     /* 元素个数   */
    BYTE     bMoreFlags;     /* PRAMMAR_MORE_ANGLE语法选项标志 */
    CHAR     sSymbol[MAX_PARASYMBOL];     /* 缺省标识符 */
    CHAR     sComment[MAX_PARACOMMENT]; /* 注释信息   */
}_PACKED_1_ TYPE_PRAMMAR;

#define STRUCTTYPE_PARA      1      /* para */
#define STRUCTTYPE_PRAMMAR   2      /* prammar */

typedef struct tagTYPE_CMD_PARA
{
    BYTE     bStructType;    /* 结构类型   */
    BYTE     bIsNoCmdPara;   /* NO命令参数 */
    WORD16     ParaNo;         /* 参数编号   */
    union
    {
        TYPE_PRAMMAR  tPrammar;
        TYPE_PARA     tPara;
    }uContent;
}_PACKED_1_ TYPE_CMD_PARA;

typedef struct tagTYPE_CMD_PARA_OLD
{
    BYTE     bStructType;    /* 结构类型   */
    BYTE     bIsNoCmdPara;   /* NO命令参数 */
    WORD16     ParaNo;         /* 参数编号   */
    union
    {
        TYPE_PRAMMAR  tPrammar;
        TYPE_PARA_OLD tPara;
    }uContent;
}_PACKED_1_ TYPE_CMD_PARA_OLD;

/* map */
/* type */
typedef struct tagTYPE_MAP_TYPE
{
    WORD16     wType;          /* 参数类型   */
    WORD16     wCount;         /* 匹配项个数 */
    DWORD    dwItemPos;      /* 匹配项第一项的位置 */
}_PACKED_1_ TYPE_MAP_TYPE;

/* item */
typedef struct tagTYPE_MAP_ITEM
{ 
    WORD16     wValue;        /* 参数数值   */
    DWORD    dwKeyPos;      /* 字符串位置 */
    DWORD    dwCommentPos;  /* 字符串位置 */
}_PACKED_1_ TYPE_MAP_ITEM;


/* 配置脚本部分 */
#define CFG_FILE_FORMAT         0x5A5A5A5A
#define CFG_FILE_VERSION        0x00010000

#define CFG_FILE_COMMENT_MARK               "#"
#define CFG_FILE_EQUAL_MARK                    " = "
#define CFG_FILE_COMMON_MARK                 "COMMON"
#define CFG_FILE_BANNER_NAME                   "BANNER"
#define CFG_FILE_PROMPT_NAME                   "PROMPT"
#define CFG_FILE_ENABLEPWD_NAME            "ENABLE_PASSWORD"
#define CFG_FILE_CONSOLE_ABS_TIMEOUT    "CONSOLE_ABS_TIMEOUT"
#define CFG_FILE_CONSOLE_IDLE_TIMEOUT   "CONSOLE_IDLE_TIMEOUT"
#define CFG_FILE_TELNET_ABS_TIMEOUT       "TELNET_ABS_TIMEOUT"
#define CFG_FILE_TELNET_IDLE_TIMEOUT     "TELNET_IDLE_TIMEOUT"

#define CFG_FILE_USER_MARK                       "USER"
#define CFG_FILE_USERNAME_NAME              "USERNAME"
#define CFG_FILE_PASSWORD_NAME             "PASSWORD"
#define CFG_FILE_PRIVILEGE_NAME              "PRIVILEGE"

/* file head */
typedef struct tagTYPE_CFG_FILE_HEAD
{
    DWORD    dFormatDef;     /* FormatDef == 0x5A5A5A5A,否则非法 */
    DWORD    dVersion;       /* 配置脚本文件版本   */
    DWORD    dCommonBegin;   /* 公共部分偏移量   */
    DWORD    dUserBegin;     /* 用户部分偏移量   */
}_PACKED_1_ TYPE_CFG_FILE_HEAD;

typedef struct tagCommon_Info
{
    CHAR sBanner[MAX_BANNER_LEN];
    CHAR sPrompt[MAX_PROMPT_LEN];
    CHAR sEnablePassword[MAX_PASSWORD_LEN];
    WORD16 wConsoleAbsTimeout; 
    WORD16 wConsoleIdleTimeout; 
    WORD16 wTelnetAbsTimeout; 
    WORD16 wTelnetIdleTimeout; 
}_PACKED_1_ COMMON_INFO;

typedef struct tagUser_Info
{
    CHAR    sUsername[MAX_USERNAME_LEN];
    CHAR    sPassword[MAX_PASSWORD_LEN];
    BYTE    ucRight;
}_PACKED_1_ USER_INFO;

typedef struct tagUser_Node
{
    USER_INFO tUserInfo;
    struct tagUser_Node *pNext;
}_PACKED_1_ USER_NODE;

#ifdef WIN32
    #pragma pack()
#endif

#endif /* __OAM_SCRIPT_H__ */
