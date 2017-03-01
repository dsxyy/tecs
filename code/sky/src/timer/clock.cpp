/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：clock.cpp
* 文件标识：见配置管理计划书
* 内容摘要：时钟相关接口实现文件
* 当前版本：1.0
* 作    者：张文剑
* 完成日期：2011年8月16日
*
* 修改记录1：
*     修改日期：2011/07/01
*     版 本 号：V1.0
*     修 改 人：张文剑
*     修改内容：创建
*******************************************************************************/
#include "pub.h"
#include "clock.h"

void Delay(uint32 miliseconds)
{
    struct timespec delay;
	struct timespec rem;
	int n = -1;
    if (miliseconds > 0)
    {
        delay.tv_sec = miliseconds/1000;
        delay.tv_nsec = (miliseconds - delay.tv_sec*1000)*1000*1000;
    }
    else
    {
        delay.tv_sec = 0;
        delay.tv_nsec  = 1;
    }
    
    do
    {
        n = nanosleep(&delay,&rem);
		delay = rem;
    }
    while (n < 0 && errno == EINTR);    
}

STATUS GetLocalClock(T_Clock *pclock)
{
    time_t timep;
    time(&timep);
    
    struct tm *ptm = localtime(&timep); /*取得当地时间*/
    memset(pclock,0,sizeof(*pclock));
    pclock->year = 1900 + ptm->tm_year;
    pclock->month = 1 + ptm->tm_mon;
    pclock->day = ptm->tm_mday;
    pclock->week = 1 + ptm->tm_wday;

    pclock->hour = ptm->tm_hour;
    pclock->minute = ptm->tm_min;
    pclock->second = ptm->tm_sec;

    return SUCCESS;
}

string ClockToString(const T_Clock& clock)
{
    stringstream oss;
    oss << clock.year;
    oss << '/';
    oss << clock.month;
    oss << '/';
    oss << clock.day;
    oss << ' ';
    oss << clock.hour;
    oss << ':';
    oss << clock.minute;
    oss << ':';
    oss << clock.second;
    return oss.str();
}


