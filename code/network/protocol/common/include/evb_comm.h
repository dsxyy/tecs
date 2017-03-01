#if !defined(EVB_COMMON_INCLUDE_H__)
#define EVB_COMMON_INCLUDE_H__

#ifdef __cplusplus
extern "C"{
#endif 

/************************************************************************/
/* EVB macro define                                                     */
/************************************************************************/
#ifdef _WIN32
#include <time.h>
#include <crtdbg.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>


typedef unsigned char       u8;
typedef unsigned short      u16;
typedef unsigned int        u32;
typedef unsigned __int64    u64;
typedef unsigned int        socklen_t;
typedef int                 int32;

#ifndef __u8
#define __u8 u8
#define __u16 u16
#define __u32 u32
#define __u64 u64
#endif

#define true                1
#define false               0
#define bool                BOOL
#define snprintf            _snprintf
#ifndef assert
#define assert(X)
#endif
#define __func__            ""
#define GLUE_ALLOC          malloc
#define GLUE_FREE           free
#define MEMSET              memset
#define MEMCPY              memcpy
#define MEMCMP              memcmp
#define STRNCMP             strncmp

/*
 * List declarations.
 */
#define    LIST_HEAD(name, type)                        \
struct name {                                \
    struct type *lh_first;    /* first element */            \
}

#define    LIST_HEAD_INITIALIZER(head)                    \
    { NULL }

#define    LIST_ENTRY(type)                        \
struct {                                \
    struct type *le_next;    /* next element */            \
    struct type **le_prev;    /* address of previous next element */    \
}

/*
 * List functions.
 */

#define    LIST_EMPTY(head)    ((head)->lh_first == NULL)

#define    LIST_FIRST(head)    ((head)->lh_first)

#define    LIST_FOREACH(var, head, field)                    \
    for ((var) = LIST_FIRST((head));                \
        (var);                            \
        (var) = LIST_NEXT((var), field))

#define    LIST_FOREACH_SAFE(var, head, field, tvar)            \
    for ((var) = LIST_FIRST((head));                \
        (var) && ((tvar) = LIST_NEXT((var), field), 1);        \
        (var) = (tvar))

#define    LIST_INIT(head) do {                        \
    LIST_FIRST((head)) = NULL;                    \
} while (0)

#define    LIST_INSERT_AFTER(listelm, elm, field) do {            \
    if ((LIST_NEXT((elm), field) = LIST_NEXT((listelm), field)) != NULL)\
        LIST_NEXT((listelm), field)->field.le_prev =        \
            &LIST_NEXT((elm), field);                \
    LIST_NEXT((listelm), field) = (elm);                \
    (elm)->field.le_prev = &LIST_NEXT((listelm), field);        \
} while (0)

#define    LIST_INSERT_BEFORE(listelm, elm, field) do {            \
    (elm)->field.le_prev = (listelm)->field.le_prev;        \
    LIST_NEXT((elm), field) = (listelm);                \
    *(listelm)->field.le_prev = (elm);                \
    (listelm)->field.le_prev = &LIST_NEXT((elm), field);        \
} while (0)

#define    LIST_INSERT_HEAD(head, elm, field) do {                \
    if ((LIST_NEXT((elm), field) = LIST_FIRST((head))) != NULL)    \
        LIST_FIRST((head))->field.le_prev = &LIST_NEXT((elm), field);\
    LIST_FIRST((head)) = (elm);                    \
    (elm)->field.le_prev = &LIST_FIRST((head));            \
} while (0)

#define    LIST_NEXT(elm, field)    ((elm)->field.le_next)

#define    LIST_REMOVE(elm, field) do {                    \
    if (LIST_NEXT((elm), field) != NULL)                \
        LIST_NEXT((elm), field)->field.le_prev =         \
            (elm)->field.le_prev;                \
    *(elm)->field.le_prev = LIST_NEXT((elm), field);        \
} while (0)

typedef unsigned long       T_PORT;
typedef unsigned char       BYTE;
typedef unsigned short      UINT16;
typedef unsigned int        UINT32;
typedef unsigned __int64    UINT64;
typedef unsigned int        WORD32;
typedef unsigned short      WORD;


#define PRINTF printf

#define EVB_LOG(nLvl, fmt, ...)\
    do\
    {\
        PRINTF(fmt, __VA_ARGS__);\
    }while(0)

#endif 

/* EVB_TECS */
#ifdef _EVB_TECS
#include <unistd.h>
#include <getopt.h>
#include <mqueue.h>
#include <mqueue.h>
#include <sys/resource.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <assert.h>
#include <sys/queue.h>
#include <pthread.h>
#include <sys/syscall.h>


typedef unsigned char       u8;
typedef unsigned short      u16;
typedef unsigned int        u32;
typedef unsigned long long  u64;
typedef unsigned int        socklen_t;
typedef int                 int32;

typedef unsigned long       T_PORT;
typedef unsigned char       BYTE;
typedef unsigned short      UINT16;
typedef unsigned int        UINT32;
typedef unsigned long int   UINT64;
typedef unsigned int        WORD32;
typedef unsigned short      WORD;
typedef int                 INT32;

typedef unsigned int        DWORD32;
typedef int                 INT;
typedef char CHAR;

#ifndef FALSE
#define FALSE               0
#endif

#ifndef TRUE
#define TRUE                1
#endif

#define __func__            ""
#define GLUE_ALLOC          malloc
#define GLUE_FREE           free
#define MEMSET              memset
#define MEMCPY              memcpy
#define MEMCMP              memcmp
#define STRNCMP             strncmp

#define PRINTF printf
#define EVB_LOG(nLvl, fmt, arg...)\
    do\
    {\
    PRINTF(fmt, ##arg);\
    }while(0)

#endif

typedef enum tagEm8021abBridgeType
{
    EM_NEAREST_BRIDGE = 0,
    EM_NEAREST_NON_TPMR_BRIDGE,
    EM_NEAREST_CUSTOMER_BRIDGE,
    EM_8021AB_BR_TYPE_ALL
}EM_8021AB_BR_TYPE;

typedef enum tagEvbMode
{
    EN_NORMAL_MODE = 0,
    EN_VEB_MODE, 
    EN_VEPA_MODE, 
}EN_EVB_MODE;


#define EVB_DIGEST_COUNT                17
#define EVB_IPV4_ADDR_STRING_LEN sizeof("255.255.255.255")
#define EVB_IPV4_ADDR_LEN           4
#define EVB_MAX_IP_ADDR_LEN         EVB_IPV4_ADDR_LEN
#define EVB_MAX_DIGITS_LENGTH  sizeof("ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255")
#define EVB_MAX_PUNCT_LENGTH   sizeof("::::::...")
#define EVB_IP_TO_STRING_SIZE       4

#define EVB_PORT_NAME_MAX_LEN   (32) /* 端口名字符长度; */
#define EVB_PORT_MAC_LEN        (6)

#define EVB_LOG_ERROR           (0)
#define EVB_LOG_WARN            (1)
#define EVB_LOG_INFO            (2)
#define EVB_LOG_DEBUG           (3)

#define EVB_ASSERT(x)           assert(x)

/*字节序*/
#define EVB_BYTEORDER_INIT          0
#define EVB_BYTEORDER_BIGENDIAN     1
#define EVB_BYTEORDER_LITTLEENDIAN  2

/* To use debug function in TECS shell */
#ifdef DEFINE_DEBUG_FUNC
#define EVB_DEBUG_FUNC_REG(func) DEFINE_DEBUG_FUNC(func)
#else
#define EVB_DEBUG_FUNC_REG(func)
#endif

#ifdef DEFINE_DEBUG_VAR
#define EVB_DEBUG_FUNC_VAR_REG(variable) DEFINE_DEBUG_VAR(variable)
#else
#define EVB_DEBUG_FUNC_VAR_REG(variable)
#endif

#ifdef DEFINE_DEBUG_FUNC_ALIAS
#define EVB_DEBUG_FUNC_ALIAS_REG(alias,func) DEFINE_DEBUG_FUNC_ALIAS(alias,func)
#else
#define EVB_DEBUG_FUNC_ALIAS_REG(alias,func)
#endif

#define EVB_PKT_COUNT_INC(x)    do{if((x) != 0xFFFFFFFFL) (x)++;}while(0)
#define EVB_PKT_COUNT_DEC(x)    do{if((x) != 0) (x)--;}while(0)

#define EVB_NTOHS evb_ntohs
#define EVB_NTOHL evb_ntohl
#define EVB_HTONS evb_htons 
#define EVB_HTONL evb_htonl

/* bit操作; */
#define         EVB_BIT_SET(F, B)           ((F) |= (B))
#define         EVB_BIT_RESET(F, B)         ((F) &= ~(B))
#define         EVB_BIT_TEST(F, B)          ((F) & (B))

/*mac add del */
#define EVB_ISS_MAC_ADD   (1)
#define EVB_ISS_DRV_GET   (2)
#define EVB_ISS_MAC_DEL   (3) 

/* EVB TLV 协商的结果，端口粒度; */
#define EVB_TLV_STATUS_INIT (0)
#define EVB_TLV_STATUS_SUCC (1)

typedef struct tagEvbSysTime
{ 
    u16 wYear; 
    u16 wMonth; 
    u16 wDayOfWeek; 
    u16 wDay; 
    u16 wHour; 
    u16 wMinute; 
    u16 wSecond; 
    u16 wMilliseconds; 
}TEvbSysTime; 


/************************************************************************/
/* EVB common function declare                                          */
/************************************************************************/
extern int evb_random();
void EvbPrintPktStt(int32 lev, u64 num, const char *str);
void EvbDelay(u32 uiMillisecond);
int32 EvbGetNicMac(const char * cstrNicName, u8 aucMac[], u8 arSz);
int32 EvbGetLocTime(TEvbSysTime *pSysTm);

int32 evb_local_is_bigendian();
int32 evb_ntohl(int32 x);
int32 evb_htonl(int32 x);
unsigned short evb_ntohs(unsigned short x);
unsigned short evb_htons(unsigned short x);

u32 evbcalc_2_exponent(u32 exp);

char * evb_ipv4_to_string(u32 addr);
u32 evb_string_to_ipv4(char *str);

// evb iss mac interface
u32 evb_SetMacVlan(u32 port,BYTE mac[EVB_PORT_MAC_LEN],u16  vlan,u32 opt);

/* 交换单板上关闭端口mac自学习*/
int evb_set_cml(u32 dwPort, u32 dwCml);
/* 交换单板上获取端口mac学习参数*/
int evb_get_cml(u32 dwPort);

/* 调试函数：显示内存中数据; */
void DbgMemShow(void *pMem, WORD32 dwSz);
void EvbTrace(char   *fmt,   ...);

int32 PrintLocalTimer();


#ifdef __cplusplus
}
#endif 

#endif
