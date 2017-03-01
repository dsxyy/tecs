/*********************************************************************
* 版权所有 (C)2008, 深圳市中兴通讯股份有限公司。
*
* 文件名称： oam_pub_type.h
* 文件标识：
* 内容摘要：
* 其它说明： 无
* 当前版本： V1.0
* 作    者： 刘华振——3G平台
* 完成日期：
*
* 修改记录1：
*    修改日期：2008年08月xx日
*    版 本 号：V1.0
*    修 改 人：刘华振
*    修改内容：创建
**********************************************************************/
#ifndef _OAM_PUB_TYPE_H_
#define _OAM_PUB_TYPE_H_

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

/***********************************************************
 *                    其它条件编译选项                     *
***********************************************************/

/***********************************************************
 *                   标准、非标准头文件                    *
***********************************************************/
/*#include "tulip.h"
#include "tulip_appcommon.h"
#include "oam_jobtype.h"
#include "db.h"*/
#include "sky.h"
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <stdlib.h>
//-----------------------------------------------------------------------------

/***********************************************************
 *                        常量定义                         *
***********************************************************/
extern char *g_pszJName;
extern BOOLEAN g_bLogByPrintf;

#define OAM_MAX_LOG_LEN (WORD16)250
extern char g_strlog[OAM_MAX_LOG_LEN];
 
/***********************************************************
 *                       全局宏                            *
***********************************************************/
#undef	MAX
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))

#undef	MIN
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))

#ifndef INVALID_SUBSYS
    #define INVALID_SUBSYS          0xFF   /* 无效子系统号 */
#endif

#define FAILED     0 //失败
#define SUCCESSED  1 //成功
#define EXIST      2 //存在

/* 双向链表插入类型*/
#define INSERT_HEAD    (BYTE)1    /* 链表头插入 */
#define INSERT_TAIL      (BYTE)2  /* 链表尾插入 */

/* 双向链表移除类型*/
#define REMOVE_HEAD    (BYTE)1 /* 链表头移除 */
#define REMOVE_TAIL    (BYTE)2  /* 链表尾移除 */


#define OAM_OSS_SUCCESS                             XOS_SUCCESS       /* 正确返回 */
#ifndef XOS_VOS_SHM_EXIST
#define  XOS_VOS_SHM_EXIST (XOS_VOS_ERR_BEGIN +12)
#endif
#define OAM_OSS_CREAT XOS_CREAT
#define OAM_OSS_RDWR XOS_RDWR

/* 比较函数返回值 */
#define CMP_RET_EQ  (int)0 /* 等于 */
#define CMP_RET_LT   (int)-1 /* 小于 */
#define CMP_RET_GT  (int)1 /* 大于 */

/* 遍历回调函数返回值 */
#define OAM_TRAVERSE_STOP (int)1    /* 希望跳出遍历 */
#define OAM_TRAVERSE_NSTOP (int)0 /* 继续遍历 */

/**< 方便在函数每个返回前加上打印等调试信息 */
#define OAM_RETURN return
#define OAM_RETURN_VAL(VAL)    return (VAL)
#define OAM_BREAK break

#if (defined(OAM_LINT))
#ifndef __func__
#define __func__ "OamCommFunc"
#endif
#endif

/**< 内存操作宏 */
#define OAM_MEMSET(DEST, DATA, SIZE)\
    memset((BYTE *)(DEST), (DATA), (SIZE))
#define OAM_MEMMOVE(DEST, SRC, SIZE)\
        memmove((void *)(DEST), (void *)(SRC), (SIZE))
#define OAM_MEMCOPY(DEST, SRC, SIZE)\
    memcpy((BYTE *)(DEST), (BYTE *)(SRC), (SIZE))
#define OAM_MEMCMP(DEST, SRC, SIZE)\
    memcmp((void *)(DEST), (void *)(SRC), (SIZE))
#define OAM_SIZEOF(DEST)\
            sizeof(DEST)
            
char *OamGetLogFlag();
void OamFormatLog(char *PcFmt, ...);
#define OAM_FIXSYSLOG(L, S)\
        do\
        {\
            g_pszJName = OamGetLogFlag();\
            if (NULL != g_pszJName) \
            {\
                if (!g_bLogByPrintf)\
                {\
                    syslog(L, "Line= %u, JName = %s, Func = %s : %s", __LINE__, g_pszJName, __func__, (char *)g_strlog);\
                }\
                else\
                {\
                    printf("Line= %u, JName = %s, Func = %s : %s", __LINE__, g_pszJName, __func__, (char *)g_strlog);\
                }\
            }\
            OAM_MEMSET(g_strlog, 0, sizeof(char) * OAM_MAX_LOG_LEN);\
        }while(0)
 

#define OAM_SYSLOG(L, S, args...) \
    do\
    {\
        OamFormatLog(S, ##args);\
        if (NULL != g_pszJName) \
        {\
            if (!g_bLogByPrintf)\
            {\
                syslog(L, "Line= %u, JName = %s, Func = %s : %s", __LINE__, g_pszJName, __func__, (char *)g_strlog);\
            }\
            else\
            {\
                printf("Line= %u, JName = %s, Func = %s : %s", __LINE__, g_pszJName, __func__, (char *)g_strlog);\
            }\
        }\
        OAM_MEMSET(g_strlog, 0, sizeof(char) * OAM_MAX_LOG_LEN);\
    }while(0)


/* 有些编译器不支持变参宏，统一改为定参宏，但是又需要打印出
行数和函数名称，所以区分定义*/
#define OAM_NASSERT_CONTINUE_IF_FAIL(EXPR)\
        if ((EXPR))\
        {\
            OAM_MEMSET(g_strlog, 0, sizeof(char) * OAM_MAX_LOG_LEN);\
        }\
        else\
        { \
            OAM_FIXSYSLOG(LOG_ERR, g_strlog);\
            continue;\
        }        

/**< 如果异常，直接跳出循环 */
#define OAM_NASSERT_BREAK_IF_FAIL(EXPR)\
        if ((EXPR))\
        {\
            OAM_MEMSET(g_strlog, 0, sizeof(char) * OAM_MAX_LOG_LEN);\
        }\
        else\
        {\
            OAM_FIXSYSLOG(LOG_ERR, g_strlog);\
            break;\
        }
        

/**< 如果异常，直接返回，用于无返回类型的函数类判断 */
#define OAM_NASSERT_RETURN_IF_FAIL(EXPR)\
    if ((EXPR))\
    {\
        OAM_MEMSET(g_strlog, 0, sizeof(char) * OAM_MAX_LOG_LEN);\
    }\
    else\
    {\
        OAM_FIXSYSLOG(LOG_ERR, g_strlog);\
        return;\
    }

/**< 如果异常，直接返回指定的值，用于有返回类型的函数类判断 */
#define OAM_NASSERT_RETURN_VAL_IF_FAIL(EXPR, VAL)\
    if ((EXPR))\
    {\
        OAM_MEMSET(g_strlog, 0, sizeof(char) * OAM_MAX_LOG_LEN);\
    }\
    else\
    {\
        OAM_FIXSYSLOG(LOG_ERR, g_strlog);\
        return (VAL);\
    }
    /**< 如果为真，直接跳出循环 */
#define OAM_NASSERT_BREAK_IF_SUCCESS(EXPR)\
        if ((EXPR))\
        {\
            OAM_FIXSYSLOG(LOG_ERR, g_strlog);\
            break;\
        }\
        else\
        {\
            OAM_MEMSET(g_strlog, 0, sizeof(char) * OAM_MAX_LOG_LEN);\
        }
    
    
    /**< 如果为真，直接返回，用于无返回类型的函数类判断 */
#define OAM_NASSERT_RETURN_IF_SUCCESS(EXPR)\
        if ((EXPR))\
        {\
            OAM_FIXSYSLOG(LOG_ERR, g_strlog);\
            return;\
        }\
        else\
        {\
            OAM_MEMSET(g_strlog, 0, sizeof(char) * OAM_MAX_LOG_LEN);\
        }
    
    /**< 如果成功，直接返回指定的值，用于有返回类型的函数类判断 */
#define OAM_NASSERT_RETURN_VAL_IF_SUCCESS(EXPR, VAL)\
        if ((EXPR))\
        {\
            OAM_FIXSYSLOG(LOG_ERR, g_strlog);\
            return (VAL);\
        }\
        else\
        {\
            OAM_MEMSET(g_strlog, 0, sizeof(char) * OAM_MAX_LOG_LEN);\
        }

#define OAM_CONTINUE_IF_FAIL(EXPR)\
        SkyAssert((EXPR));\
        if ((EXPR))\
        {\
            OAM_MEMSET(g_strlog, 0, sizeof(char) * OAM_MAX_LOG_LEN);\
        }\
        else\
        {\
            OAM_FIXSYSLOG(LOG_ERR, g_strlog);\
            continue;\
        }
/**< 如果异常，直接跳出循环 */
#define OAM_BREAK_IF_FAIL(EXPR)\
    SkyAssert((EXPR));\
    if ((EXPR))\
    {\
        OAM_MEMSET(g_strlog, 0, sizeof(char) * OAM_MAX_LOG_LEN);\
    }\
    else\
    {\
        OAM_FIXSYSLOG(LOG_ERR, g_strlog);\
        break;\
    }

/**< 如果异常，直接返回，用于无返回类型的函数类判断 */
#define OAM_RETURN_IF_FAIL(EXPR)\
    SkyAssert((EXPR));\
    if ((EXPR))\
    {\
        OAM_MEMSET(g_strlog, 0, sizeof(char) * OAM_MAX_LOG_LEN);\
    }\
    else\
    {\
        OAM_FIXSYSLOG(LOG_ERR, g_strlog);\
        return;\
    }

/**< 如果异常，直接返回指定的值，用于有返回类型的函数类判断 */
#define OAM_RETURN_VAL_IF_FAIL(EXPR, VAL)\
    SkyAssert(EXPR);\
    if ((EXPR))\
    {\
        OAM_MEMSET(g_strlog, 0, sizeof(char) * OAM_MAX_LOG_LEN);\
    }\
    else\
    {\
        OAM_FIXSYSLOG(LOG_ERR, g_strlog);\
        return (VAL);\
    }

/**< 如果为真，直接跳出循环 */
#define OAM_BREAK_IF_SUCCESS(EXPR)\
    SkyAssert(!(EXPR));\
    if ((EXPR))\
    {\
        OAM_FIXSYSLOG(LOG_ERR, g_strlog);\
        break;\
    }\
    else\
    {\
        OAM_MEMSET(g_strlog, 0, sizeof(char) * OAM_MAX_LOG_LEN);\
    }


/**< 如果为真，直接返回，用于无返回类型的函数类判断 */
#define OAM_RETURN_IF_SUCCESS(EXPR)\
    SkyAssert(!(EXPR));\
    if ((EXPR))\
    {\
        OAM_FIXSYSLOG(LOG_ERR, g_strlog);\
        return;\
    }\
    else\
    {\
        OAM_MEMSET(g_strlog, 0, sizeof(char) * OAM_MAX_LOG_LEN);\
    }

/**< 如果成功，直接返回指定的值，用于有返回类型的函数类判断 */
#define OAM_RETURN_VAL_IF_SUCCESS(EXPR, VAL)\
    SkyAssert(!(EXPR));\
    if ((EXPR))\
    {\
        OAM_FIXSYSLOG(LOG_ERR, g_strlog);\
        return (VAL);\
    }\
    else\
    {\
        OAM_MEMSET(g_strlog, 0, sizeof(char) * OAM_MAX_LOG_LEN);\
    }


/**< 用于无返回值函数类的指针强制转换 */    
#define OAM_PTRTYPE_CAST_RETURN(PTR, TYPE, PV)\
    do\
    {\
        (PTR) = (TYPE *)(PV);\
        OAM_RETURN_IF_FAIL((NULL != PTR));\
    }\
    while(0)

/**< 用于有返回值函数类的指针强制转换 */    
#define OAM_PTRTYPE_CAST_RETURN_VAL(PTR, TYPE, PV, VAL)\
        do\
        {\
            (PTR) = (TYPE *)(PV);\
            OAM_RETURN_VAL_IF_FAIL((NULL != (PTR)), VAL);\
        }\
        while(0)
/**< 参数判断宏 */
#define PARAM_JUDGE(E, P)\
    E *ptEntry = NULL;\
    P *ptPriData = NULL;\
    OAM_PTRTYPE_CAST_RETURN(ptEntry, E, pucEntry);\
    OAM_PTRTYPE_CAST_RETURN(ptPriData, P, ptEntry->pPriData)

/**< 字节序转换宏 */
#define OAM_INVERTWORD16(W)  (W) = XOS_InvertWord16((W))
#define OAM_INVERTWORD32(DW)  (DW) = XOS_InvertWord32((DW))
#define OAM_INVERTWORD64(FW)  (FW) = XOS_InvertWord64((FW))

            
/***********************************************************
 *                     全局数据类型                        *
 ***********************************************************/ 
typedef unsigned char    T_PAD;

typedef char   gchar;
typedef short  gshort;
typedef long   glong;
typedef int    gint;
typedef gint   gboolean;

typedef unsigned char   guchar;
typedef unsigned short  gushort;
typedef unsigned long   gulong;
typedef unsigned int    guint;

typedef signed char gint8;
typedef unsigned char guint8;
typedef signed short gint16;
typedef unsigned short guint16;

typedef void* gpointer;
typedef const void *gconstpointer;

typedef signed int gssize;
typedef unsigned int gsize;

typedef enum
{
  G_IN_ORDER,
  G_PRE_ORDER,
  G_POST_ORDER,
  G_LEVEL_ORDER
} GTraverseType;

#ifdef  __cplusplus
# define G_BEGIN_DECLS  extern "C" {
# define G_END_DECLS    }
#else
# define G_BEGIN_DECLS
# define G_END_DECLS
#endif

typedef gint            (*GCompareFunc)         (gconstpointer  a,
                                                 gconstpointer  b);
typedef void            (*GDestroyNotify)       (gpointer       data, 
gpointer user_data);
typedef int  (* T_OamCompareFunc) (const void *a, const void *b);
 /***********************************************************
 *                     全局函数原型                        *
***********************************************************/

#ifdef WIN32
    #pragma pack()
#endif

#endif

