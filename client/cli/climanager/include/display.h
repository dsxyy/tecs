/*********************************************************************
* 版权所有 (C)2002, 深圳市中兴通讯股份有限公司。
*
* 文件名称：Display.h
* 文件标识：
* 内容摘要：回显解析相关函数
* 其它说明：
            
* 当前版本：
* 作    者：马俊辉 移植整理
* 完成日期：2007.6.5
*
* 修改记录1：// 修改历史记录，包括修改日期、修改者及修改内容
*    修改日期：
*    版 本 号：
*    修 改 人：
*    修改内容：
* 修改记录2：
**********************************************************************/
#ifndef __DISPLAY_H__
#define __DISPLAY_H__

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

#include "oam_cfg.h"
#include "pub_oam_cfg_event.h"

/* 成功 */
#define DISPLAY_SUCCESS               0
                                      
/* 回显部分返回值的定义 */            
#define ERROR_INDIS                   2
#define INVALID_OPRMODE               3
#define SUCCESS_END                   9
#define SUCCESS_ENDWITHSWITCHSTART    10  /* 遇到%hs */
#define SUCCESS_ENDWITHSWITCHCASE     11  /* 遇到%hc */
#define SUCCESS_ENDWITHSWITCHEND      12  /* 遇到%he */
#define SUCCESS_ENDWITHFORSTART       13  /* 遇到%rs */
#define SUCCESS_ENDWITHFOREND         14  /* 遇到%re */
#define SUCCESS_ENDWITHSWITCH         15  /* 遇到%r */
#define SUCCESS_ENDWITHFOR            16  /* 遇到%h */
#define SUCCESS_ENDWITHTABLE          17  /* 遇到%t */
#define SUCCESS_CONTINUETONEXTPACKET  20  /* 遇到未完参数，进到下一个指针 */

#define MAX_PARA_NUMBER               2001 /* 最多能够显示的参数数量 */
#define DISPLAY_BUFFER_SIZE          MAX_OUTPUT_RESULT /* 1025 * 50 *//* 50k size */
#define MAX_DISPLAY_STRLEN            1025      /* 1k size */
#define MAX_DISPLAY_WIDTH             80

#define MAX_STACK_DEPTH               10
#define TEXT_MARK                     34

/* 错误显示及记录 */
/* 回显部分error code及其定义 */
#define UNKNOW_ERROR_IN_DISPLAY       1   /*unknow error in display*/
#define INSUFFICIENT_MEMORY           2   /*insufficient memory*/
#define BUFFER_OVERFLOW               3   /*buffer overflow*/
#define INVALID_TYPE                  4   /*invalid type*/
#define INVALID_FIELD_NUMBERS         5   /*invalid field_numbers*/
#define INVALID_FOOTSCRIPT            6   /*invalid footscript*/
#define INVALID_STRING                7   /*invalid string*/
#define WRONG_RETURN_VALUE            8   /*wrong return value*/
#define FOOTSCRIPT_DATA_MISMATCH      9   /*footScript data mismatch*/
#define INVALID_POINTER               10  /*invalid pointer*/
#define INVALID_STATE                 11  /*invalid state*/
#define INVALID_DATALEN               12  /*invalid datalen*/
#define DATA_LENGTH_ERROR             15  /*data length error*/
#define CMD_ROUTER_BGP						0x0c01000b
#define MODE_PROTOCOL	(BYTE)2

/* 错误表定义 */
typedef struct
{
    CHAR sErrorString[50];
    BYTE bErrorLevel;
}_PACKED_1_ ERROR_INFO;

/* DBG_ERR_LOG用于检测程序正常运行时的错误 */
/*#define DBG_ERR_LOG(ErrorCode)\
    if (sErrorTable[ErrorCode].bErrorLevel > 0)\
    {\
        OSS_Printf(PRINT_OAM_PLATCFGAGT, PRN_NORMAL, PRN_LEVEL_HIGHEST,sErrorTable[ErrorCode].sErrorString);\
        OSS_Printf(PRINT_OAM_PLATCFGAGT, PRN_NORMAL, PRN_LEVEL_HIGHEST,"\nin %s, %d\n",__FILE__ , __LINE__);\
    }*/
#define DBG_ERR_LOG(ErrorCode)\
    if (sErrorTable[ErrorCode].bErrorLevel > 0)\
    {\
        XOS_SysLog(OAM_CFG_LOG_ERROR, "%s\n", sErrorTable[ErrorCode].sErrorString); \
        XOS_SysLog(OAM_CFG_LOG_ERROR, "\nin %s, %d\n",__FILE__ , __LINE__); \
    }
    
/* ASSERT用于检测本部份程序调试运行时可能出现的错误 */
/*#define DISPLAY_ASSERT(test)    \
if (!(test))\
{\
    OSS_Printf(PRINT_OAM_PLATCFGAGT, PRN_NORMAL, PRN_LEVEL_HIGHEST,"\n!!Assertion failed: file %s,line %d!!\n", __FILE__ , __LINE__);\
    return ERROR_INDIS;\
}*/
#define DISPLAY_ASSERT(test)    \
if (!(test))\
{\
    return ERROR_INDIS;\
}

typedef struct 
{
    CHAR    *pDisplayBuffer;    /* a pointer to the display buffer */
    CHAR    *pDispBuff;         /* 缓冲区中当前的位置 */
    CHAR    *pFootScript;       /* 使用的脚本指针 */
    
    /* 参数的起始位置表 */
    CHAR    *pParameterStartAddress[MAX_PARA_NUMBER];
    /* 参数位置的默认值 */
    int     giParameterLocation;
    DWORD   dwNumber;
    
    /* 保存当前包的最后一个paraNo值 */
    WORD16    ParaNo;
    
    /* 保存退出时脚本的位置,当出现LastPack时，应将此指针清空 */
    CHAR    *spFootScript;
    
    struct 
    {
        WORD16    wForCounter;        /* 循环的次数 */
        CHAR    *pFootScript;       /* 循环时，指向循环脚本开始的位置 */
    }_PACKED_1_ sForStack[MAX_STACK_DEPTH];    /* for语句的控制栈 */
    BYTE     bForStackTop;          /* for语句的控制栈顶 */
    
    struct
    {
        WORD16    wParaValue;
    }_PACKED_1_ sSwitchStack[MAX_STACK_DEPTH]; /* switch语句的控制栈 */
    BYTE    bSwitchStackTop;        /* switch语句的控制栈顶 */
    
}_PACKED_1_ TYPE_DISP_GLOBAL_PARA;

typedef struct 
{
    CHAR    *pDisplayBuffer;        /* a pointer to the display buffer */
    CHAR    *pDispBuff;             /* 缓冲区中当前的位置 */
    CHAR    *pFootScript;           /* 使用的脚本指针 */
    
    /* 参数的起始位置表 */
    CHAR    *pParameterStartAddress[MAX_PARA_NUMBER];
    
    /* 参数位置的默认值 */
    int     giParameterLocation;
    
    struct
    {
        WORD16    wParaValue;
    }_PACKED_1_ sSwitchStack[MAX_STACK_DEPTH]; /* switch语句的控制栈 */
    BYTE    bSwitchStackTop;        /* switch语句的控制栈顶 */
    
    BYTE    bTableType;             /* 表的类型：1 table 2 list */
    TABLE_DATA TableData;
}_PACKED_1_ TYPE_DISP_GLOBAL_PARA_ITM;


/* 对外暴露接口 */
struct tag_TYPE_LINK_STATE;
INT32 OamCfgintpSendToBuffer(struct tag_TYPE_LINK_STATE *pLinkState,
                            MSG_COMM_OAM *ptMsg,           /* 参数空间入口地址 */
                            CHAR *pFootScriptEx,           /* 脚本的入口位置 */
                            TYPE_DISP_GLOBAL_PARA *pGlobal /* 保存所有全局变量的指针 */
                            );

#ifdef WIN32
    #pragma pack()
#endif

#endif /* __DISPLAY_H__ */
