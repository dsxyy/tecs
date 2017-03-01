/*------------------------------------------------------------------------------
  author wangwei
  corporation: ZTE
  date: 2001/12/9
  version 01a
  history:
------------------------------------------------------------------------------*/
#include "includes.h"

/*lint -e123*/

static BYTE OAM_DayOfMonth[] = {0, 31, 28, 31, 30, 31, 30 , 31, 31, 30, 31, 30, 31};
static BYTE OAM_DeltaDays[12] = {0, 3, 3, 6, 8, 11, 13, 16, 19, 21, 24, 26};
/*------------------------------------------------------------------------------
    函 数 名:   OamCfgTimeToString
    功能描述:   调试用，合并时可去掉
    输    入:   long *dSecs     指向秒数的指针
                CHAR *pString       存放转换字符串的缓冲区
    输    出:   将秒数拆分为时分秒
    返    回:   
    全局变量:   
                
    注    释:   无错误返回，pString的地址空间由调用的程序分配
==============================================================================
    修改记录:
    修改日期        版本     修改人      修改原因及内容
==============================================================================
    2000\00\00  1.0             创建
------------------------------------------------------------------------------*/
WORD16 OamCfgTimeToString(WORD32 *dSecs, CHAR *pString, WORD32 dwBufSize)
{
    CHAR TempString[20];
    long dSecond;
    CHAR bHour, bMinute, bSecond;
    bHour   = 0;
    bMinute = 0;
    bSecond = 0;
    dSecond = *dSecs;
    /* check the dSecond and Seconds less than a day will be subtracted 
       from dSecond */
    bSecond = (BYTE)(dSecond % 60);                 /* 得到秒 */
    dSecond = (long)dSecond / 60;
    bMinute = (BYTE)(dSecond % 60);                 /* 得到分 */
    dSecond = (long)dSecond / 60;
    bHour   = (BYTE)(dSecond % 24);                 /* 得到一天内的小时 */
    if (bHour < 10)
        XOS_snprintf(TempString, sizeof(TempString),"0%d", bHour);
    else
        XOS_snprintf(TempString, sizeof(TempString), "%d", bHour);
    strncpy(pString, TempString, dwBufSize);
    strncat(pString, ":", dwBufSize);
    if (bMinute < 10)
        XOS_snprintf(TempString, sizeof(TempString), "0%d", bMinute);
    else
        XOS_snprintf(TempString, sizeof(TempString), "%d", bMinute);
    strncat(pString, TempString, dwBufSize);
    strncat(pString, ":", dwBufSize);
    if (bSecond < 10)
        XOS_snprintf(TempString, sizeof(TempString), "0%d", bSecond);
    else
        XOS_snprintf(TempString, sizeof(TempString), "%d", bSecond);
    strncat(pString, TempString, dwBufSize);
    return 0;
}

/* 时间格式的字符串转化为秒数 */
WORD16 StringToTime(CHAR *pString, WORD32 *dSecond)
{

    CHAR bHour, bMinute, bSecond;
    bHour   = 0;
    bMinute = 0;
    bSecond = 0;
    *dSecond = 0;
    /* hour:minute:second */
    /*check and get hour */
    while(*pString >= '0' && *pString <= '9')
    {
        bHour = 10 * bHour + *pString++ - '0';
    }
    if (bHour > 23)
        return Time_WRONG_TIMESTRING;

    /* check and get minute */
    if (*pString++ != MARK_COLON)
        return Time_WRONG_TIMESTRING;
    while(*pString >= '0' && *pString <= '9')
    {
        bMinute = 10 * bMinute + *pString++ - '0';
    }
    if (bMinute > 59)
        return Time_WRONG_TIMESTRING;

    /* check and get second */
    if (*pString++ != MARK_COLON)
        return Time_WRONG_TIMESTRING;
    while(*pString >= '0' && *pString <= '9')
    {
        bSecond = 10 * bSecond + *pString++ - '0';
    }
    if (bSecond > 59)
        return Time_WRONG_TIMESTRING;
    *dSecond = 3600 * bHour + 60 * bMinute + bSecond;
    return Time_SUCCESS;
}

WORD16 DateToStringAmericanStyle(WORD32 *dSecs, CHAR *pString, WORD32 dwBufSize)
{
    OAM_Clock_STRUCT clk;
    CHAR TempString[5];
    if (OAM_DwordToClock(dSecs, &clk) != Time_SUCCESS)
        return Time_WRONG_TIME;
    /* month */
    if (clk.mon < 10)
        XOS_snprintf(TempString, sizeof(TempString), "0%d", clk.mon);
    else
        XOS_snprintf(TempString, sizeof(TempString), "%d", clk.mon);
    strncpy(pString, TempString, dwBufSize);
    strncat(pString, "/", dwBufSize);
    /* day */
    if (clk.mday < 10)
        XOS_snprintf(TempString, sizeof(TempString), "0%d", clk.mday);
    else
        XOS_snprintf(TempString, sizeof(TempString), "%d", clk.mday);
    strncat(pString, TempString, dwBufSize);
    strncat(pString, "/", dwBufSize);
    /* year */
    XOS_snprintf(TempString, sizeof(TempString), "%d", clk.year);
    strncat(pString, TempString, dwBufSize);

    return Time_SUCCESS;
}
WORD16 StringToDateAmericanStyle(CHAR *pString, WORD32 *dSecs)
{
    long lYear = 0;
    BYTE cMonth = 0, cDay = 0;
    long lDay = 0;
    CHAR cInterChar;
    BYTE iTemp;
    /* the first section should be year */
    /* check and get the year number */
    while(*pString >= '0' && *pString <= '9')
    {
        cDay = 10 * cDay + *pString++ - '0';
    }
    if ( cDay <= 0 || cDay > OAM_DayOfMonth[cMonth])
        return Time_WRONG_TIMESTRING;
    cInterChar = *pString;
    if (*pString != '/' && *pString != '-')
        return Time_WRONG_TIMESTRING;
    pString++;
    while(*pString >= '0' && *pString <= '9')
    {
        cMonth = 10 * cMonth + *pString++ - '0';
    }
    if (cMonth <= 0 || cMonth > 12)
        return Time_WRONG_TIMESTRING;
    if (*pString != cInterChar)
        return Time_WRONG_TIMESTRING;
    pString++;
    while(*pString >= '0' && *pString <= '9')
    {
        lYear = 10 * lYear + *pString++ - '0';
    }
    if (lYear <= 1000 || lYear > 9999)
        return Time_WRONG_TIMESTRING;
    if (lYear > TM_START_YEAR)
        lDay =    (lYear - TM_START_YEAR) * 365
                + (lYear - TM_START_YEAR) / 4
                - (lYear - TM_START_YEAR) / 100
                + (lYear - TM_START_YEAR) / 400 + 1;
    else
        lDay = 0;
    if (lYear % 400 == 0
        || (lYear % 100 != 0 && lYear % 4 == 0)
        ) 
        OAM_DayOfMonth[2] = 29;
    else
        OAM_DayOfMonth[2] = 28;
    for (iTemp = 1; iTemp < cMonth; iTemp++)
        lDay += OAM_DayOfMonth[iTemp];
    lDay += cDay - 1;
    *dSecs = lDay * 24 * 3600;
    return Time_SUCCESS;
}

WORD16 DateToStringEuropeanStyle(WORD32 *dSecs, CHAR *pString, WORD32 dwBufSize)
{
    OAM_Clock_STRUCT clk;
    CHAR TempString[5];
    if (OAM_DwordToClock(dSecs, &clk) != Time_SUCCESS)
        return Time_WRONG_TIME;
    /* day */
    if (clk.mday < 10)
        XOS_snprintf(TempString, sizeof(TempString), "0%d", clk.mday);
    else
        XOS_snprintf(TempString, sizeof(TempString), "%d", clk.mday);
    strncpy(pString, TempString, dwBufSize);
    strncat(pString, "/", dwBufSize);
    /* month */
    if (clk.mon < 10)
        XOS_snprintf(TempString, sizeof(TempString), "0%d", clk.mon);
    else
        XOS_snprintf(TempString, sizeof(TempString), "%d", clk.mon);
    strncat(pString, TempString, dwBufSize);
    strncat(pString, "/", dwBufSize);
    /* year */
    XOS_snprintf(TempString, sizeof(TempString), "%d", clk.year);
    strncat(pString, TempString, dwBufSize);

    return Time_SUCCESS;
}
WORD16 StringToDateEuropeanStyle(CHAR *pString, WORD32 *dSecs)
{
    return Time_SUCCESS;
}

/* 秒数化为年月日 */
WORD16 DateToStringChineseStyle(WORD32 *dSecs, CHAR *pString, WORD32 dwBufSize)
{
    OAM_Clock_STRUCT clk;
    CHAR TempString[5];
    if (OAM_DwordToClock(dSecs, &clk) != Time_SUCCESS)
        return Time_WRONG_TIME;
    /* year */
    XOS_snprintf(TempString, sizeof(TempString), "%d", clk.year);
    strncpy(pString, TempString, dwBufSize);
    strncat(pString, "-", dwBufSize);
    /* month */
    if (clk.mon < 10)
        XOS_snprintf(TempString, sizeof(TempString), "0%d", clk.mon);
    else
        XOS_snprintf(TempString, sizeof(TempString), "%d", clk.mon);
    strncat(pString, TempString, dwBufSize);
    strncat(pString, "-", dwBufSize);
    /* day */
    if (clk.mday < 10)
        XOS_snprintf(TempString, sizeof(TempString), "0%d", clk.mday);
    else
        XOS_snprintf(TempString, sizeof(TempString), "%d", clk.mday);
    strncat(pString, TempString, dwBufSize);
    return Time_SUCCESS;
}

/* 年月日字符串化为秒数 */
WORD16 StringToDateChineseStyle(CHAR * pString, WORD32 *dSecond)
{
    long lYear = 0;
    BYTE cMonth = 0, cDay = 0;
    long lDay = 0;
    CHAR cInterChar;
    BYTE iTemp;
    /* the first section should be year */
    /* check and get the year number */
    while(*pString >= '0' && *pString <= '9')
    {
        lYear = 10 * lYear + *pString++ - '0';
    }
    if (lYear <= 1000 || lYear > 9999)
        return Time_WRONG_TIMESTRING;
    cInterChar = *pString;
    if (*pString != '/' && *pString != '-')
        return Time_WRONG_TIMESTRING;
    pString++;
    while(*pString >= '0' && *pString <= '9')
    {
        cMonth = 10 * cMonth + *pString++ - '0';
    }
    if (cMonth <= 0 || cMonth > 12)
        return Time_WRONG_TIMESTRING;
    if (*pString != cInterChar)
        return Time_WRONG_TIMESTRING;
    pString++;
    while(*pString >= '0' && *pString <= '9')
    {
        cDay = 10 * cDay + *pString++ - '0';
    }
    if ( cDay <= 0 || cDay > OAM_DayOfMonth[cMonth])
        return Time_WRONG_TIMESTRING;
    if (lYear > TM_START_YEAR)
        lDay =    (lYear - TM_START_YEAR) * 365
                + (lYear - TM_START_YEAR) / 4
                - (lYear - TM_START_YEAR) / 100
                + (lYear - TM_START_YEAR) / 400 + 1;
    else
        lDay = 0;
    if (lYear % 400 == 0
        || (lYear % 100 != 0 && lYear % 4 == 0)
        ) 
        OAM_DayOfMonth[2] = 29;
    else
        OAM_DayOfMonth[2] = 28;
    for (iTemp = 1; iTemp < cMonth; iTemp++)
        lDay += OAM_DayOfMonth[iTemp];
    lDay += cDay - 1;
    *dSecond = lDay * 24 * 3600;
    return Time_SUCCESS;    
}

WORD16 DateTimeToStringAmericanStyle(WORD32 *dSecs, CHAR *pString,WORD32 dwBufSize)
{
    OAM_Clock_STRUCT clk;
    CHAR TempString[5];
    if (OAM_DwordToClock(dSecs, &clk) != Time_SUCCESS)
        return Time_WRONG_TIME;
    /* year */
    XOS_snprintf(TempString, sizeof(TempString), "%d", clk.year);
    strncpy(pString, TempString, dwBufSize);
    strncat(pString, "-", dwBufSize);
    /* month */
    XOS_snprintf(TempString, sizeof(TempString), "%d", clk.mon);
    strncat(pString, TempString, dwBufSize);
    strncat(pString, "-", dwBufSize);
   /* day */
    XOS_snprintf(TempString, sizeof(TempString), "%d", clk.mday);
    strncat(pString, TempString, dwBufSize);

    /* time */
    strncat(pString, " ", dwBufSize);
    /* hour */
    XOS_snprintf(TempString, sizeof(TempString), "%d", clk.hour);
    strncat(pString, TempString, dwBufSize);
    strncat(pString, ":", dwBufSize);
    /* minute*/
    XOS_snprintf(TempString, sizeof(TempString), "%d", clk.min);
    strncat(pString, TempString, dwBufSize);
    strncat(pString, ":", dwBufSize);
    /* second */
    XOS_snprintf(TempString, sizeof(TempString), "%d", clk.sec);
    strncat(pString, TempString, dwBufSize);

    return Time_SUCCESS;
}
WORD16 DateTimeToStringEuropeanStyle(WORD32 *dSecs, CHAR *pString, WORD32 dwBufSize)
{
    OAM_Clock_STRUCT clk;
    CHAR TempString[5];
    if (OAM_DwordToClock(dSecs, &clk) != Time_SUCCESS)
        return Time_WRONG_TIME;
    /* day */
    if (clk.mday < 10)
        XOS_snprintf(TempString, sizeof(TempString), "0%d", clk.mday);
    else
        XOS_snprintf(TempString, sizeof(TempString), "%d", clk.mday);
    strncpy(pString, TempString, dwBufSize);
    strncat(pString, "/", dwBufSize);
    /* month */
    if (clk.mon < 10)
        XOS_snprintf(TempString, sizeof(TempString), "0%d", clk.mon);
    else
        XOS_snprintf(TempString, sizeof(TempString), "%d", clk.mon);
    strncat(pString, TempString, dwBufSize);
    strncat(pString, "/", dwBufSize);
    /* year */
    XOS_snprintf(TempString, sizeof(TempString), "%d", clk.year);
    strncat(pString, TempString, dwBufSize);

    /* time */
    strncat(pString, " ", dwBufSize);
    /* hour */
    if (clk.hour < 10)
        XOS_snprintf(TempString, sizeof(TempString), "0%d", clk.hour);
    else
        XOS_snprintf(TempString, sizeof(TempString), "%d", clk.hour);
    strncat(pString, TempString, dwBufSize);
    strncat(pString, ":", dwBufSize);
    /* minute */
    if (clk.min < 10)
        XOS_snprintf(TempString, sizeof(TempString), "0%d", clk.min);
    else
        XOS_snprintf(TempString, sizeof(TempString), "%d", clk.min);
    strncat(pString, TempString, dwBufSize);
    strncat(pString, ":", dwBufSize);
    /* second */
    if (clk.sec < 10)
        XOS_snprintf(TempString, sizeof(TempString), "0%d", clk.sec);
    else
        XOS_snprintf(TempString, sizeof(TempString), "%d", clk.sec);
    strncat(pString, TempString, dwBufSize);

    return Time_SUCCESS;
}
WORD16 DateTimeToStringChineseStyle(WORD32 *dSecs, CHAR *pString, WORD32 dwBufSize)
{
    OAM_Clock_STRUCT clk;
    CHAR TempString[5];
    if (OAM_DwordToClock(dSecs, &clk) != Time_SUCCESS)
        return Time_WRONG_TIME;
    /* year */
    XOS_snprintf(TempString, sizeof(TempString), "%d", clk.year);
    strncpy(pString, TempString, dwBufSize);
    strncat(pString, "-", dwBufSize);
    /* month */
    if (clk.mon < 10)
        XOS_snprintf(TempString, sizeof(TempString), "0%d", clk.mon);
    else
        XOS_snprintf(TempString, sizeof(TempString), "%d", clk.mon);
    strncat(pString, TempString, dwBufSize);
    strncat(pString, "-",dwBufSize);
    /* day */
    if (clk.mday < 10)
        XOS_snprintf(TempString, sizeof(TempString), "0%d", clk.mday);
    else
        XOS_snprintf(TempString, sizeof(TempString), "%d", clk.mday);
    strncat(pString, TempString, dwBufSize);

    /* time */
    strncat(pString, " ", dwBufSize);
    /* hour */
    if (clk.hour < 10)
        XOS_snprintf(TempString, sizeof(TempString), "0%d", clk.hour);
    else
        XOS_snprintf(TempString, sizeof(TempString), "%d", clk.hour);
    strncat(pString, TempString, dwBufSize);
    strncat(pString, ":", dwBufSize);
    /* minute */
    if (clk.min < 10)
        XOS_snprintf(TempString, sizeof(TempString), "0%d", clk.min);
    else
        XOS_snprintf(TempString, sizeof(TempString), "%d", clk.min);
    strncat(pString, TempString, dwBufSize);
    strncat(pString, ":", dwBufSize);
    /* second */
    if (clk.sec < 10)
        XOS_snprintf(TempString, sizeof(TempString), "0%d", clk.sec);
    else
        XOS_snprintf(TempString, sizeof(TempString), "%d", clk.sec);
    strncat(pString, TempString, dwBufSize);

    return Time_SUCCESS;
}

WORD16 OAM_ClockToDword(OAM_Clock_STRUCT *clk, WORD32 *dSecs)
{
    WORD16    DaysSince;
    WORD32   SecondsSince;
    INT32   nYearNumber;
    
    DaysSince  = clk->mday - 1;
    DaysSince += (clk->mon - 1) * 28;
    DaysSince += OAM_DeltaDays[clk->mon - 1];
    if (clk->mon > 2 && 
        ((clk->year % 400) == 0 || 
        (clk->year % 100 != 0 && clk->year % 4 == 0)))
        DaysSince ++;
    
    for (nYearNumber = TM_START_YEAR; nYearNumber < clk->year; nYearNumber++)
    {
        /* 判断是否为润年 */
        if (nYearNumber % 400 == 0
            || (nYearNumber % 100 != 0 && nYearNumber % 4 == 0)
            ) 
            DaysSince += 366;
        else 
            DaysSince += 365;
    }
    
    SecondsSince = DaysSince * TM_DAY_SECONDS + clk->hour * 3600
                      + clk->min * 60 + clk->sec;
    
    *dSecs = SecondsSince;
    
    return 0;
}

/* 秒数化为年月日 */
WORD16 OAM_DwordToClock(WORD32 *dSecs, OAM_Clock_STRUCT *clk)
{
    INT32 nDay, TempDay;
    INT32 nYearNumber;
    BYTE bMonthNumber;
    BYTE bDayNumber;
    WORD32 dwLeftSecs;


    if (*dSecs <= 0)
        return Time_WRONG_TIME;
    nDay = (*dSecs) / TM_DAY_SECONDS;         /* 求出天数 */
    dwLeftSecs = (*dSecs) % TM_DAY_SECONDS;
    /* get year */
    for (nYearNumber = TM_START_YEAR, TempDay = nDay; ; nYearNumber++)
    {
        /* 判断是否为润年 */
        if (nYearNumber % 400 == 0
            || (nYearNumber % 100 != 0 && nYearNumber % 4 == 0)
            ) 
            TempDay -= 366;
        else 
            TempDay -= 365;
        if (TempDay < 0)
            break;
        nDay = TempDay;
    }
    if (nYearNumber > 9999)
        return Time_WRONG_TIME;
    /* get month */
    if (nYearNumber % 400 == 0
        || (nYearNumber % 100 != 0 && nYearNumber % 4 == 0)
        ) 
        OAM_DayOfMonth[2] = 29;
    else
        OAM_DayOfMonth[2] = 28;

    for (bMonthNumber = 1, TempDay = nDay; ; bMonthNumber++)
    {
        TempDay -= OAM_DayOfMonth[bMonthNumber];
        if (TempDay < 0)
            break;
        nDay = TempDay;
    }
    bDayNumber = (CHAR)nDay + 1;

    clk->year = (WORD16)nYearNumber;
    clk->mon = bMonthNumber;
    clk->mday = bDayNumber;
    /* hour min sec */
    clk->hour = (BYTE)(dwLeftSecs / 3600);
    dwLeftSecs = dwLeftSecs % 3600;
    clk->min = (BYTE)(dwLeftSecs / 60);
    clk->sec = (BYTE)(dwLeftSecs % 60);

    return Time_SUCCESS;
}

WORD16 OAM_TimeToDword(OAM_Clock_STRUCT *clk, WORD32 *dSecs)
{
    *dSecs = clk->hour * 3600 + clk->min * 60 + clk->sec;

    return Time_SUCCESS;
}
WORD16 OAM_DwordToTime(WORD32 *dSecs, OAM_Clock_STRUCT *clk)
{
    WORD32 dwLeftSecs = *dSecs;

    clk->hour = (BYTE)(dwLeftSecs / 3600);
    dwLeftSecs = dwLeftSecs % 3600;
    clk->min = (BYTE)(dwLeftSecs / 60);
    clk->sec = (BYTE)(dwLeftSecs % 60);

    return Time_SUCCESS;
}
/*lint +e123*/
 
