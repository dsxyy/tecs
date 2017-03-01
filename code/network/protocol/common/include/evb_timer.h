#if !defined(EVB_TIMER_INCLUDE_H__)
#define EVB_TIMER_INCLUDE_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagEvbTimer{
  struct tagEvbTimer *link;             /* list link */
  struct tagEvbTimer *prev;             /* 前向链表; */
  u32 expire;                           /* relative expiration time in ms */
  void (*handler) (struct tagEvbTimer *, void *); /* expiration handler */
  void *cookie;                         /* handler cookie */
}TEvbTimer;

/* EVB协议栈基准定时器定时间隔 */
#define EVB_BASE_TIMER_INTERVAL    20

#define EVB_DEFAULT_EXPIRE      0xFFFFFFFFL
#define EVB_TIMER_IN_USE(tm)      ((tm)->expire != EVB_DEFAULT_EXPIRE)

/* 定时器时间点按20ms对齐 */                        
#define EVB_TIME_ROUND(t)   ((t) + EVB_BASE_TIMER_INTERVAL - 1) / EVB_BASE_TIMER_INTERVAL * EVB_BASE_TIMER_INTERVAL

#define EVB_TM_EVAL(f, p)       {NULL, NULL, EVB_DEFAULT_EXPIRE, (f), (p)}

/* 定时器初始化;
   struct tagEvbTimer t_tm;
   BRS_TM_INIT(&t_tm, func, cookie);
 */
#define EVB_TM_INIT(t, f, p) \
        do { \
            (t)->handler = (f); \
            (t)->cookie = (p); \
            tm_init(t); \
        } while(0)

void InitEvbTimerModule(void);                  /* 模块初始化接口 */
void Evb20msTimerHandler(void);                 /* EVB协议栈20ms定时器处理函数(外部接口); */
void EvbTmInit (struct tagEvbTimer *);          /* 初始化基于20ms基准时间的定时器，外部接口; */
void EvbTmSet (struct tagEvbTimer *, u32);      /* 设置基于20ms基准时间的定时器，外部接口; */
void EvbTmCancel (struct tagEvbTimer *);        /* 取消基于20ms基准时间的定时器，外部接口; */
/* 调试函数; */
void DbgShowEvbTimer(void);

#ifdef __cplusplus
}
#endif

#endif
