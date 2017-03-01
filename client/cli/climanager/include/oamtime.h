/*********************************************************************
* 版权所有 (C)2002, 深圳市中兴通讯股份有限公司。
*
* 文件名称：OamTime.h
* 文件标识：
* 内容摘要：时间处理
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
* 修改记录2：…
**********************************************************************/
#ifndef __OAM_TIME_H__
#define __OAM_TIME_H__

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

#define Time_SUCCESS                0
#define Time_WRONG                  1
#define Time_WRONG_TIMESTRING       2
#define Time_WRONG_TIME             3

#define TM_DAY_SECONDS              86400

#define TM_START_YEAR               1970/*2000*/
#define TM_START_MONTH              1
#define TM_START_DAY                1

typedef struct tagOAM_Clock_STRUCT
{
    BYTE    sec;                /* seconds after the minute - [0,  59]      */
    BYTE    min;                /* minutes after the hour   - [0,  59]      */
    BYTE    hour;               /* hours after midnight     - [0,  23]      */
    BYTE    mday;               /* day of the month         - [1,  31]      */
    BYTE    mon;                /* months since January     - [1,  12]      */
    BYTE    week;               /* week,now,no use                          */
    WORD16    year;               /* years since 2000         - [2k, 2130]    */
}_PACKED_1_ OAM_Clock_STRUCT;


/* 对外暴露接口 */
extern WORD16 OamCfgTimeToString(WORD32 *dSecs, CHAR *pString, WORD32 dwBufSize);
extern WORD16 StringToTime(CHAR *pString, WORD32 *dSecond);

extern WORD16 DateToStringAmericanStyle(WORD32 *dSecs, CHAR *pString, WORD32 dwBufSize);
extern WORD16 StringToDateAmericanStyle(CHAR *pString, WORD32 *dSecs);

extern WORD16 DateToStringEuropeanStyle(WORD32 *dSecs, CHAR *pString, WORD32 dwBufSize);
extern WORD16 StringToDateEuropeanStyle(CHAR *pString, WORD32 *dSecs);

extern WORD16 DateToStringChineseStyle(WORD32 *dSecs, CHAR *pString, WORD32 dwBufSize);
extern WORD16 StringToDateChineseStyle(CHAR * pString, WORD32 *dSecond);

extern WORD16 DateTimeToStringAmericanStyle(WORD32 *dSecs, CHAR *pString, WORD32 dwBufSize);
extern WORD16 DateTimeToStringEuropeanStyle(WORD32 *dSecs, CHAR *pString, WORD32 dwBufSize);
extern WORD16 DateTimeToStringChineseStyle(WORD32 *dSecs, CHAR *pString, WORD32 dwBufSize);

extern WORD16 OAM_ClockToDword(OAM_Clock_STRUCT *clk, WORD32 *dSecs);
extern WORD16 OAM_DwordToClock(WORD32 *dSecs, OAM_Clock_STRUCT *clk);
extern WORD16 OAM_TimeToDword(OAM_Clock_STRUCT *clk, WORD32 *dSecs);
extern WORD16 OAM_DwordToTime(WORD32 *dSecs, OAM_Clock_STRUCT *clk);

#ifdef WIN32
    #pragma pack()
#endif

#endif /* __OAM_TIME_H__ */


