#ifndef _CLITIMER_TMP_H_
#define _CLITIMER_TMP_H_
#include <sys/time.h>
#include "oam_typedef.h"

#define  SYN_TIMER              1      /**< 同步定时器 */
#define  ABS_TIMER              2      /**< 绝对定时器 */
#define  RELATIVE_TIMER         3      /**< 普通相对定时器 */
#define  LOOP_TIMER             4      /**< 循环定时器 */
#define  MONO_TIMER             5      /**< mono定时器 */

#define JOB_TYPE_INVALID                   (WORD16)0

extern XOS_TIMER_ID SetTimer(WORD32 dwTimerNo, WORD16 wJno, WORD16 wTimerType, SWORD32 sdwDuration, WORD32 dwParam);
extern void KillTimerByTimerId(WORD32 dwTimerId);
extern void KillTimer(WORD16 wJno, WORD32 dwTimerNo,WORD32 dwParam);
extern void TimerInitial();
extern void timer_task();

#endif

