#ifdef __cplusplus
extern "C"{
#endif 

#include "evb_comm.h"
#include "evb_dbg_func.h"
#include "evb_timer.h"

#define ACCURACY_TIMER_FREQUENCY    (160)
    static TEvbTimer s_tAccuracyTimer;

    void TestAccuracyTimerOut(TEvbTimer *tm, void *cookie)
    {
        TEvbSysTime tSysTm = {0};
        if(NULL == tm)
        {
            EVB_ASSERT(0);
            return ;
        }
        EvbGetLocTime(&tSysTm);
        PRINTF("Now Time: %02d:%02d:%02d:%3d.\n", 
            tSysTm.wHour, tSysTm.wMinute, tSysTm.wSecond, tSysTm.wMilliseconds);

        EvbTmSet(tm, ACCURACY_TIMER_FREQUENCY);
        return ;
    }

   void DbgAccTimerTestStart(void)
    {
        EvbTmInit(&s_tAccuracyTimer);
        s_tAccuracyTimer.handler = TestAccuracyTimerOut;
        s_tAccuracyTimer.cookie  = NULL;
        EvbTmSet(&s_tAccuracyTimer, ACCURACY_TIMER_FREQUENCY);

        return ;
    }

   void DbgAccTimerTestStop(void)
   {
       EvbTmCancel(&s_tAccuracyTimer);

       return ;
   }


#ifdef __cplusplus
}
#endif 
