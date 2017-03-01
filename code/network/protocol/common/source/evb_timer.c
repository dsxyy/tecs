#ifdef __cplusplus
extern "C" {
#endif

#include "evb_comm.h"
#include "evb_timer.h"

#define EVB_TM_LT(x, y)     (((x) - (y)) & 0x80000000L)
#define EVB_TM_GE(x, y)     (!EVB_TM_LT(x, y))
#define EVB_TM_LE(x, y)     (EVB_TM_GE(y, x))
#define EVB_TM_GT(x, y)     (!EVB_TM_LE(x, y))

#define EVB_TIMER_CALLOUT_DISABLED      (u8)0  
#define EVB_TIMER_CALLOUT_ENABLED       (u8)1

    int32 s_nEvbTimerCalloutStatus = EVB_TIMER_CALLOUT_DISABLED;
    u32 s_dwEvbTimerCalloutTick = 0;
    void (*pfEvbTimerCalloutFunc)(void);

    static u32 s_dwNiddTicks      = 0;
    static u32 s_dwNiddTickCount    = 0;
    static u32 s_dwNiddSecs       = 0;

    static TEvbTimer *s_ptEvbTimerList = NULL;
    static void timer_exp(void);
    static void EvbTimerUpdate(void);

    void InitEvbTimerModule(void)
    {
        s_nEvbTimerCalloutStatus = EVB_TIMER_CALLOUT_DISABLED;
        s_dwEvbTimerCalloutTick = 0;
        pfEvbTimerCalloutFunc = NULL;

        s_dwNiddTicks      = 0;
        s_dwNiddTickCount    = 0;
        s_dwNiddSecs       = 0;

        s_ptEvbTimerList = NULL;
        return ;
    }

    static u32 EvbGlueNow(void)
    {
        return(s_dwNiddTicks);
    }

/*
    static u32 EvbGlueNowSec(void)
    {
        return(s_dwNiddSecs);
    }
*/

    static void EvbGlueCallin(u32 tm_para, void (*function)(void))
    {
        s_nEvbTimerCalloutStatus   = EVB_TIMER_CALLOUT_ENABLED;
        s_dwEvbTimerCalloutTick     = EvbGlueNow() + tm_para;
        pfEvbTimerCalloutFunc = function;
    }

    void EvbTmInit(TEvbTimer *tm_para)
    {
        TEvbTimer *t;

        EVB_ASSERT(tm_para);

        /* 首先判断待初始化定时器是否已经在定时器队列中，如果存在，;
        * 从队列中删除该定时器。首先查找定时器队列，如果已经存在，;
        * 在函数etc_tm_cancel()也会查找一次，这样对有两次查找，但;
        * 可读性强，同时认为初始化不需要考虑效率问题;
        */
        for(t = s_ptEvbTimerList; t; t = t->link)
        {
            if(t == tm_para)
            {
                EvbTmCancel(tm_para);
                break;
            }
        }

        /* 将超时时间设置为全F，以标示该定时器未使用。
        * 虽然在etc_tm_cancel已经做了该操作，但这里
        * 再次操作是为了强调初始化的主要工作。;
        */
        tm_para->expire = EVB_DEFAULT_EXPIRE;
    }

    /*
    * Bring the expire times of all timers currently in the queue up to date.
    * ALL code that modifies the timer queue MUST call this function FIRST!
    */
    static void EvbTimerUpdate(void)
    {
        static u32 last_update = 0;
        u32 now = EvbGlueNow();
        u32 elapsed = now - last_update;
        TEvbTimer *t = s_ptEvbTimerList;

        while (t && elapsed > t->expire)
        {
            elapsed -= t->expire;
            t->expire = 0;
            t = t->link;
        }

        if (t && elapsed > 0)
        {
            t->expire -= elapsed;
        }

        last_update = now;
    }

    /*
    * This function is called when the OS timer fires.  This code presumes
    * that we will not be called until we should have been, and then,
    * precisely once.  [This may no longer be true, but let's be paranoid....]
    *
    * We have to be able to deal with the timer list changing as we run through
    * it, so we loop bringing the expire times up to date and processing the
    * first timer on the queue if we're reached its expire time.  If there's
    * anything left on the queue when we finish processing expired timers,
    * we schedule the OS timer to wake us up when it's time to resume processing.
    *
    * Processing a timer means dequeuing it and calling its handler.
    */
    static void timer_exp(void)
    {
        while (s_ptEvbTimerList && (EvbTimerUpdate(), s_ptEvbTimerList->expire == 0))
        {
            TEvbTimer *t = s_ptEvbTimerList;
            s_ptEvbTimerList = t->link;
            t->expire = EVB_DEFAULT_EXPIRE;
            (t->handler)(t, t->cookie);
        }

        if (s_ptEvbTimerList)
        {
            EvbGlueCallin(s_ptEvbTimerList->expire, timer_exp);
        }
    }

    /*
    * Set a timer to expire "when" milliseconds from now, canceling it first
    * if it was already set.  We won't queue a timer that has no handler.
    *
    * For fairness, if there are other timers already queued that will
    * expire at the same time as this one, we put this one at the end of
    * the group so the others will get to run first.
    *
    * If, when we're done with the insertion, this timer is the new head
    * of the queue, we reset the OS timer accordingly.
    */
    void EvbTmSet(TEvbTimer *tm_para, u32 when)
    {
        register TEvbTimer **tt;

        EVB_ASSERT(tm_para);

        if (!tm_para || !tm_para->handler)
        {
            return;
        }
        if (EVB_TIMER_IN_USE(tm_para))
        {
            EvbTmCancel(tm_para);
        }
        EvbTimerUpdate();

        /*
        * Skip down the queue looking for the right place to insert,
        * adjusting the relative expire time as we go.
        */
        tm_para->expire = when;
        for (tt = &s_ptEvbTimerList; *tt && (*tt)->expire <= tm_para->expire; tt = &(*tt)->link)
        {
            if (tm_para == *tt)
            {
                EVB_LOG(EVB_LOG_ERROR, 
                    "EvbTmSet: duplicate timer!, tm: %p, handler: %p, cookie: 0x%x",
                    tm_para, tm_para->handler, tm_para->cookie);
                EVB_ASSERT(tm_para != *tt);
                return;
            }

            tm_para->expire -= (*tt)->expire;
        }

        /*
        * Insert the timer, and adjust the relative expiration time of
        * the timer immediately following it, if there is one.
        */
        tm_para->link = *tt;
        *tt = tm_para;

        EVB_ASSERT(tm_para->link != *tt);

        if (tm_para->link)
        {
            tm_para->link->expire -= tm_para->expire;
        }

        /*
        * If we inserted at the front of the queue, set the OS timer.
        */
        if (tm_para == s_ptEvbTimerList)
        {
            EvbGlueCallin(s_ptEvbTimerList->expire, timer_exp);
        }
    }

    /*
    * Cancel a timer.  We have to adjust the relative expiration time
    * of the timer immediately following the one we're canceling.
    * If we're canceling the first timer and the adjusted expiration time
    * of the new head of the queue is still in the future, we reset the OS
    * timer.  We don't need to reset the OS timer if we're not canceling
    * the head of the queue.  We don't want to reset the OS timer if
    * the OS is already running behind schedule, because we expect the OS
    * timer we've already scheduled to expire Real Soon Now.
    */
    void EvbTmCancel(TEvbTimer *tm_para)
    {
        register TEvbTimer **tt = &s_ptEvbTimerList;
        int was_first = (tm_para == s_ptEvbTimerList);

        EVB_ASSERT(tm_para);

        if (!tm_para || !EVB_TIMER_IN_USE(tm_para))
        {
            return;
        }
        EvbTimerUpdate();

        if (tm_para->link)
        {
            tm_para->link->expire += tm_para->expire;
        }

        while (*tt && *tt != tm_para)
        {
            tt = &(*tt)->link;
        }
        if (*tt)
        {
            *tt = tm_para->link;
        }
        tm_para->expire = EVB_DEFAULT_EXPIRE;

        if (was_first && s_ptEvbTimerList && s_ptEvbTimerList->expire > 0)
        {
            EvbGlueCallin(s_ptEvbTimerList->expire, timer_exp);
        }
    }

    void DbgShowEvbTimer(void)
    {
        TEvbTimer *t;
        PRINTF("\n======Timer List:\n");
        for (t = s_ptEvbTimerList; t; t = t->link)
        {
            PRINTF("tm:%p handler:%p exp:(%u)\n", t, t->handler, t->expire);
        }
        PRINTF("\n");
    }

    void Evb20msTimerHandler(void)
    {
        s_dwNiddTicks += EVB_BASE_TIMER_INTERVAL; 
        s_dwNiddTickCount++;
        if(s_dwNiddTickCount >= 50)
        {
            s_dwNiddSecs++;
            s_dwNiddTickCount = 0;
        }

        if(s_nEvbTimerCalloutStatus == EVB_TIMER_CALLOUT_ENABLED &&
            EVB_TM_GE(EvbGlueNow(), s_dwEvbTimerCalloutTick))
        {
            s_nEvbTimerCalloutStatus = EVB_TIMER_CALLOUT_DISABLED;
            s_dwEvbTimerCalloutTick   = 0;
            (*pfEvbTimerCalloutFunc)();
        }
        return ;
    }

#ifdef __cplusplus
}
#endif
