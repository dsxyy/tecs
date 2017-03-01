/*********************************************************************
* 版权所有 (C)2002, 深圳市中兴通讯股份有限公司。
*
* 文件名称：CliFunc.h
* 文件标识：
* 内容摘要：命令拆分匹配相关函数
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
#ifndef __CLI_FUNC_H__
#define __CLI_FUNC_H__

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

/************************* the states of parsing the command line *************************/
/* begin state */
#define PARSESTATE_BEGINPARSE       0   /* begin parsing the command line */
/* parse state */
#define PARSESTATE_WORD_PARSING     10  /* parsing the words */
#define PARSESTATE_WORD_PARSED      20  /* finish parsing the words */
#define PARSESTATE_PARA_PARSING     30  /* parsing the parameters */
#define PARSESTATE_PARA_PARSED      40  /* finish parsing the parameters*/
/* finish parsing */
#define PARSESTATE_FINISH_PARSE     100 /* finish parsing the command line */
/* deal states */
#define PARSESTATE_WORD_DETERMINER  110 /* deal the words determiner */
#define PARSESTATE_WORD_EXCEPTION   120 /* deal the words exception */
#define PARSESTATE_PARA_DETERMINER  130 /* deal the para determiner */
#define PARSESTATE_PARA_EXCEPTION   140 /* deal the para exception */
#define PARSESTATE_ASK_TELNET       150 /* need to ask telnet*/

/* finish deal */
#define PARSESTATE_FINISH_DEAL      200 /* finish dealing the determiner or the exceptions */

/* 执行程序在CPU上的位置 */
#define EXEPOS_MP                   1
#define EXEPOS_XMLRPC                        250
#define EXEPOS_INTERPRETATION       251

#define MAX_LINEWIDTH               80

#define MORE_FLAG_ONE(a) ((((a) & 0x01) > 0) ? TRUE : FALSE)
#define MORE_FLAG_SEQ(a) ((((a) & 0x02) > 0) ? TRUE : FALSE)

/* 以下是为保持日志文件而定义的函数*/
#ifdef WIN32
#define OAM_CFG_PRTCL_CFG_LOG_DIR    "U:/OAM"
#else
#define OAM_CFG_PRTCL_CFG_LOG_DIR    "/DOC0/OAM"
#endif

#ifdef WIN32
    #pragma pack()
#endif

/* 对外暴露接口 */
int CalculateMatchPosition(TYPE_LINK_STATE *pLinkState);
void ParseCmdLine(TYPE_LINK_STATE   * pLinkState, CHAR *pInString, BYTE bDeterminer, BOOL bIsRestoreProcess);

#endif  /* __CLI_FUNC_H__ */
