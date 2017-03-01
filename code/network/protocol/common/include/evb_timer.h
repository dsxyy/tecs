#if !defined(EVB_TIMER_INCLUDE_H__)
#define EVB_TIMER_INCLUDE_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagEvbTimer{
  struct tagEvbTimer *link;             /* list link */
  struct tagEvbTimer *prev;             /* ǰ������; */
  u32 expire;                           /* relative expiration time in ms */
  void (*handler) (struct tagEvbTimer *, void *); /* expiration handler */
  void *cookie;                         /* handler cookie */
}TEvbTimer;

/* EVBЭ��ջ��׼��ʱ����ʱ��� */
#define EVB_BASE_TIMER_INTERVAL    20

#define EVB_DEFAULT_EXPIRE      0xFFFFFFFFL
#define EVB_TIMER_IN_USE(tm)      ((tm)->expire != EVB_DEFAULT_EXPIRE)

/* ��ʱ��ʱ��㰴20ms���� */                        
#define EVB_TIME_ROUND(t)   ((t) + EVB_BASE_TIMER_INTERVAL - 1) / EVB_BASE_TIMER_INTERVAL * EVB_BASE_TIMER_INTERVAL

#define EVB_TM_EVAL(f, p)       {NULL, NULL, EVB_DEFAULT_EXPIRE, (f), (p)}

/* ��ʱ����ʼ��;
   struct tagEvbTimer t_tm;
   BRS_TM_INIT(&t_tm, func, cookie);
 */
#define EVB_TM_INIT(t, f, p) \
        do { \
            (t)->handler = (f); \
            (t)->cookie = (p); \
            tm_init(t); \
        } while(0)

void InitEvbTimerModule(void);                  /* ģ���ʼ���ӿ� */
void Evb20msTimerHandler(void);                 /* EVBЭ��ջ20ms��ʱ��������(�ⲿ�ӿ�); */
void EvbTmInit (struct tagEvbTimer *);          /* ��ʼ������20ms��׼ʱ��Ķ�ʱ�����ⲿ�ӿ�; */
void EvbTmSet (struct tagEvbTimer *, u32);      /* ���û���20ms��׼ʱ��Ķ�ʱ�����ⲿ�ӿ�; */
void EvbTmCancel (struct tagEvbTimer *);        /* ȡ������20ms��׼ʱ��Ķ�ʱ�����ⲿ�ӿ�; */
/* ���Ժ���; */
void DbgShowEvbTimer(void);

#ifdef __cplusplus
}
#endif

#endif
