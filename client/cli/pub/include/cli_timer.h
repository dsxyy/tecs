#ifndef _CLITIMER_TMP_H_
#define _CLITIMER_TMP_H_
#include <sys/time.h>
#include "oam_typedef.h"

#define  SYN_TIMER              1      /**< ͬ����ʱ�� */
#define  ABS_TIMER              2      /**< ���Զ�ʱ�� */
#define  RELATIVE_TIMER         3      /**< ��ͨ��Զ�ʱ�� */
#define  LOOP_TIMER             4      /**< ѭ����ʱ�� */
#define  MONO_TIMER             5      /**< mono��ʱ�� */

#define JOB_TYPE_INVALID                   (WORD16)0

extern XOS_TIMER_ID SetTimer(WORD32 dwTimerNo, WORD16 wJno, WORD16 wTimerType, SWORD32 sdwDuration, WORD32 dwParam);
extern void KillTimerByTimerId(WORD32 dwTimerId);
extern void KillTimer(WORD16 wJno, WORD32 dwTimerNo,WORD32 dwParam);
extern void TimerInitial();
extern void timer_task();

#endif

