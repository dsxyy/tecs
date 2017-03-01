/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�clock.cpp
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��ʱ����ؽӿ�ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ����Ľ�
* ������ڣ�2011��8��16��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/07/01
*     �� �� �ţ�V1.0
*     �� �� �ˣ����Ľ�
*     �޸����ݣ�����
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
    
    struct tm *ptm = localtime(&timep); /*ȡ�õ���ʱ��*/
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


