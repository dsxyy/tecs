/*********************************************************************
* ��Ȩ���� (C)2002, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ�OamTime.h
* �ļ���ʶ��
* ����ժҪ��ʱ�䴦��
* ����˵����
            
* ��ǰ�汾��
* ��    �ߣ����� ��ֲ����
* ������ڣ�2007.6.5
*
* �޸ļ�¼1��// �޸���ʷ��¼�������޸����ڡ��޸��߼��޸�����
*    �޸����ڣ�
*    �� �� �ţ�
*    �� �� �ˣ�
*    �޸����ݣ�
* �޸ļ�¼2����
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


/* ���Ⱪ¶�ӿ� */
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


