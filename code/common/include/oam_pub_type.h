/*********************************************************************
* ��Ȩ���� (C)2008, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ� oam_pub_type.h
* �ļ���ʶ��
* ����ժҪ��
* ����˵���� ��
* ��ǰ�汾�� V1.0
* ��    �ߣ� �����񡪡�3Gƽ̨
* ������ڣ�
*
* �޸ļ�¼1��
*    �޸����ڣ�2008��08��xx��
*    �� �� �ţ�V1.0
*    �� �� �ˣ�������
*    �޸����ݣ�����
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
 *                    ������������ѡ��                     *
***********************************************************/

/***********************************************************
 *                   ��׼���Ǳ�׼ͷ�ļ�                    *
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
 *                        ��������                         *
***********************************************************/
extern char *g_pszJName;
extern BOOLEAN g_bLogByPrintf;

#define OAM_MAX_LOG_LEN (WORD16)250
extern char g_strlog[OAM_MAX_LOG_LEN];
 
/***********************************************************
 *                       ȫ�ֺ�                            *
***********************************************************/
#undef	MAX
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))

#undef	MIN
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))

#ifndef INVALID_SUBSYS
    #define INVALID_SUBSYS          0xFF   /* ��Ч��ϵͳ�� */
#endif

#define FAILED     0 //ʧ��
#define SUCCESSED  1 //�ɹ�
#define EXIST      2 //����

/* ˫�������������*/
#define INSERT_HEAD    (BYTE)1    /* ����ͷ���� */
#define INSERT_TAIL      (BYTE)2  /* ����β���� */

/* ˫�������Ƴ�����*/
#define REMOVE_HEAD    (BYTE)1 /* ����ͷ�Ƴ� */
#define REMOVE_TAIL    (BYTE)2  /* ����β�Ƴ� */


#define OAM_OSS_SUCCESS                             XOS_SUCCESS       /* ��ȷ���� */
#ifndef XOS_VOS_SHM_EXIST
#define  XOS_VOS_SHM_EXIST (XOS_VOS_ERR_BEGIN +12)
#endif
#define OAM_OSS_CREAT XOS_CREAT
#define OAM_OSS_RDWR XOS_RDWR

/* �ȽϺ�������ֵ */
#define CMP_RET_EQ  (int)0 /* ���� */
#define CMP_RET_LT   (int)-1 /* С�� */
#define CMP_RET_GT  (int)1 /* ���� */

/* �����ص���������ֵ */
#define OAM_TRAVERSE_STOP (int)1    /* ϣ���������� */
#define OAM_TRAVERSE_NSTOP (int)0 /* �������� */

/**< �����ں���ÿ������ǰ���ϴ�ӡ�ȵ�����Ϣ */
#define OAM_RETURN return
#define OAM_RETURN_VAL(VAL)    return (VAL)
#define OAM_BREAK break

#if (defined(OAM_LINT))
#ifndef __func__
#define __func__ "OamCommFunc"
#endif
#endif

/**< �ڴ������ */
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


/* ��Щ��������֧�ֱ�κ꣬ͳһ��Ϊ���κ꣬��������Ҫ��ӡ��
�����ͺ������ƣ��������ֶ���*/
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

/**< ����쳣��ֱ������ѭ�� */
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
        

/**< ����쳣��ֱ�ӷ��أ������޷������͵ĺ������ж� */
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

/**< ����쳣��ֱ�ӷ���ָ����ֵ�������з������͵ĺ������ж� */
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
    /**< ���Ϊ�棬ֱ������ѭ�� */
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
    
    
    /**< ���Ϊ�棬ֱ�ӷ��أ������޷������͵ĺ������ж� */
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
    
    /**< ����ɹ���ֱ�ӷ���ָ����ֵ�������з������͵ĺ������ж� */
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
/**< ����쳣��ֱ������ѭ�� */
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

/**< ����쳣��ֱ�ӷ��أ������޷������͵ĺ������ж� */
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

/**< ����쳣��ֱ�ӷ���ָ����ֵ�������з������͵ĺ������ж� */
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

/**< ���Ϊ�棬ֱ������ѭ�� */
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


/**< ���Ϊ�棬ֱ�ӷ��أ������޷������͵ĺ������ж� */
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

/**< ����ɹ���ֱ�ӷ���ָ����ֵ�������з������͵ĺ������ж� */
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


/**< �����޷���ֵ�������ָ��ǿ��ת�� */    
#define OAM_PTRTYPE_CAST_RETURN(PTR, TYPE, PV)\
    do\
    {\
        (PTR) = (TYPE *)(PV);\
        OAM_RETURN_IF_FAIL((NULL != PTR));\
    }\
    while(0)

/**< �����з���ֵ�������ָ��ǿ��ת�� */    
#define OAM_PTRTYPE_CAST_RETURN_VAL(PTR, TYPE, PV, VAL)\
        do\
        {\
            (PTR) = (TYPE *)(PV);\
            OAM_RETURN_VAL_IF_FAIL((NULL != (PTR)), VAL);\
        }\
        while(0)
/**< �����жϺ� */
#define PARAM_JUDGE(E, P)\
    E *ptEntry = NULL;\
    P *ptPriData = NULL;\
    OAM_PTRTYPE_CAST_RETURN(ptEntry, E, pucEntry);\
    OAM_PTRTYPE_CAST_RETURN(ptPriData, P, ptEntry->pPriData)

/**< �ֽ���ת���� */
#define OAM_INVERTWORD16(W)  (W) = XOS_InvertWord16((W))
#define OAM_INVERTWORD32(DW)  (DW) = XOS_InvertWord32((DW))
#define OAM_INVERTWORD64(FW)  (FW) = XOS_InvertWord64((FW))

            
/***********************************************************
 *                     ȫ����������                        *
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
 *                     ȫ�ֺ���ԭ��                        *
***********************************************************/

#ifdef WIN32
    #pragma pack()
#endif

#endif

