/*********************************************************************
* ��Ȩ���� (C)2012, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ� igmpv3_report.h
* �ļ���ʶ�� 
* ����˵���� 
* ��ǰ�汾�� V1.0
* ��    �ߣ� liuxuefeng	
* ������ڣ� 
*
* �޸ļ�¼1��
*    �޸����ڣ�2012��03��28��
*    �� �� �ţ�V1.0
*    �� �� �ˣ�liuxuefeng
*    �޸����ݣ�����
**********************************************************************/
#ifndef  _IGMPV3_REPORT_H_
#define  _IGMPV3_REPORT_H_
/**************************************************************************
*                            ͷ�ļ�                                        *
**************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/select.h>
#include <pthread.h>
#include <string.h>
#include <sys/errno.h>
#include <net/if.h>
#include <sys/stat.h>
#include <sys/types.h>



/**************************************************************************
*                            ��                                          *
**************************************************************************/
/* ����֧��һЩ�������ݽṹ */
typedef unsigned int        WORD32;
typedef unsigned short      WORD16;
typedef int                 SWORD32;
typedef short               SWORD16;
typedef int                 INT;
typedef char                CHAR;
typedef unsigned char       BYTE;
typedef signed   char       SBYTE;
typedef unsigned char    BOOLEAN;
typedef long     int        SWORD64;
typedef unsigned long int   WORD64;
typedef unsigned char       T_PAD;
typedef unsigned long int   WORDPTR; 
typedef void             VOID;
typedef void*            LPVOID;

typedef int                SOCKET;
typedef fd_set           SOCKET_SET;

#define VOID             void /* g++  bug��� */

#if (!defined(TRUE) || (TRUE!=1))
#undef TRUE
#define TRUE    1
#endif

#if (!defined(FALSE) || (FALSE!=0))
#undef FALSE
#define FALSE   0
#endif

#if (!defined(YES) || (YES!=1))
#undef YES
#define YES     (BYTE)1
#endif

#if (!defined(NO) || (NO!=0))
#undef NO
#define NO      (BYTE)0
#endif

#ifndef WAIT_FOREVER
# define WAIT_FOREVER ((WORD32)0xFFFFFFFF)
#endif  /* WAIT_FOREVER */

#ifndef NO_WAIT
# define NO_WAIT ((WORD32)0)
#endif  /* NO_WAIT */


#ifndef NULL
#define NULL ((void *)0)
#endif  /* NULL */

#ifndef LEOF
#define LEOF ((WORD32)(0xFFFFFFFF))
#endif

#ifndef WEOF
#define WEOF ((WORD16)(0xFFFF))
#endif

#ifndef _PACKED_1_
#define _PACKED_1_  __attribute__((packed))
#endif

typedef WORD32  XOS_STATUS;            /**< ֧�Ų������״̬����,ʾ����XOS_SUCCESS */
typedef unsigned long       OSS_STATUS;/**< OSS�������״̬����,ʾ���� OSS_SUCCESS */

/**************************************************************************
*                            ����                                        *
**************************************************************************/
    /* ����֧��һЩͨ�ú� */
#define TULIP_ERROR_BEGIN     (WORD32)0                                           /**< tulip�������׼�� */
#define TULIP_ERROR_END        (WORD32)(TULIP_ERROR_BEGIN+0x80000)   /**< tulip�����������׼�� */  
#define XOS_ERR_BEGIN              TULIP_ERROR_BEGIN   /**< XOS�������׼�� */
#define XOS_SUCCESS                  XOS_ERR_BEGIN         /**< �ɹ� */
#define XOS_ERR_UNKNOWN        XOS_ERR_BEGIN+1     /**< XOSδ֪���� */
#define XOS_ERROR_UNKNOWN    XOS_ERR_UNKNOWN  /**< XOSδ֪���� */

#define OSS_ERR_BEGIN          XOS_ERR_BEGIN                          /**< OSS �������׼�� */
#define OSS_SUCCESS            XOS_SUCCESS                               /**<  �ɹ� */
#define OSS_ERR_UNKNOWN        XOS_ERR_UNKNOWN                /**< OSS δ֪���� */
#define OSS_ERROR_UNKNOWN      XOS_ERROR_UNKNOWN         /**< OSS δ֪���� */
#define OSS_ERROR_SYSCALL         XOS_ERR_BEGIN+2         /**< OSS δ֪���� */

    /* IGMP_V3��غ� */
#define ETH_HW_ADDR_LEN                 ((BYTE)6)        /* MAC��ַ�ֽ��� */
#define IP_ADDR_LEN                           ((BYTE)4)        /* IPV4��ַ�ֽ��� */
#define DEF_ISS_IGMP_DSTIP              inet_addr("224.0.0.22") /* IGMPĿ��ip��ַ */
#define DEF_ISS_IGMP_DSTMAC             "\x1\x0\x5e\x0\x0\x16"  /* IGMPĿ��mac��ַ */
#define ISS_IGMP_EXIT_REC_TYPE          ((BYTE)0x3)     /* IGMP V3�鲥���¼���� �˳��鲥�� */
#define ISS_IGMP_JOIN_REC_TYPE          ((BYTE)0x4)     /* IGMP V3�鲥���¼���� �����鲥�� */
#define ISS_FRAME_LEN                   ((WORD16)14)    /* ��̫��֡�� */
#define ISS_IP_LEN                      ((WORD16)20)    /* IPͷ��,������̫��֡ */
#define ISS_IGMP_HEAD_LEN               ((WORD16)8)     /* IGMP ͷ��*/
#define ISS_FRAME_TYPE                  ((WORD16)0x800) /* ��̫��֡���� */
#define ISS_IP_VER                      ((BYTE)0x45)    /* IP�汾 */
#define ISS_IP_IGMP_TTL                 ((BYTE)0x1)     /* IP TTL�ֶ� */
#define ISS_IP_IGMP_TYPE                ((BYTE)0x2)     /* IPЭ������(IGMPV3) */
#define ISS_IGMPV3_VER                  ((BYTE)0x22)    /* IGMP V3����Э�� */
#define ISS_MAX_MGROUP_INFO             ((BYTE)10)      /* �鲥���¼ */
#define ISS_MAX_EXIT_SNDS               ((BYTE)3)       /* �˳��鲥��IGMP report������ */
#define ISS_INVALID_GROUP_NO            ((WORD16)0xFFFF)/* ��Ч�鲥��� */    
#define ISS_BASE_IF_NAME                     "eth6"           /* BASE��������ʱ�̶�Ϊeth6 */

/*from pub_oss.h*/
#define ISS_MUTICAST_BASE_ADDR     "228.228.228.228"     /**< �鲥��ַ����ʼֵ */
#define ISS_MAX_MGROUP_NO          ((WORD16)(60000))    /**< ISS�鲥֧�ֵ�����鲥�� */
#define ISS_MSG_UDP_PORT           ((WORD16)23456)       /**< ISS�׽��ְ󶨶˿� */
#define ISS_MSG_MAX_LEN            ((WORD16)(8*1024))    /**< ISS��Ϣ��󳤶� */
#define ISS_MSG_HEAD_VER0          ((BYTE)0)             /**< ISS��Ϣ��Ϣͷ�汾�� */
    
#define ISS_MSG_TYPE_SINGLE        ((BYTE)1) /**< ������Ϣ*/
#define ISS_MSG_TYPE_MULTI         ((BYTE)2) /**< ��(��)����Ϣ*/
#define ISS_MSG_TYPE_BROAD         ((BYTE)3) /**< �㲥��Ϣ �ݲ�֧��*/
/* OSS �鲥��Χ��[0X0401, 0X0500] */
#define MG_PLAT_OSS_BEGIN  0X0401
#define MG_PLAT_OSS_IGMP_V3              (MG_PLAT_OSS_BEGIN+0x0fe)  /**< IGMP_V3��Ϣ�鲥��*/
#define MG_PLAT_OSS_END    0X0500
extern BOOLEAN              g_bIssIgmpPrnEn;                          /*ISS��Ϣ��ӡ����*/
#define IGMP_Print(format, arg...)  \
do {               \
         if (TRUE == g_bIssIgmpPrnEn)  \
         {                                               \
             printf(format, ##arg);           \
         }                                               \
}while(0)

/**************************************************************************
*                             ����                                      *
**************************************************************************/

/**************************************************************************
*                          ��������                                       *
**************************************************************************/
typedef struct tagJID
{
    WORD32  dwJno;          /**< JOB��, ��16λΪJOB ����, ��16λΪJOB ʵ���� */
    WORD32  dwDevId;        /**< �豸ID, �������ֵľֺ���Ϣ, ��8λΪ�ֺţ���֧�ִ���������Ϣ*/
    WORD16  wModule;        /**< MP��� */
    WORD16  wUnit;          /**< ����� */
    BYTE    ucSUnit;        /**< �����ϵĴ������� */
    BYTE    ucSubSystem;    /**< ��ϵͳ�� */
    BYTE    ucRouteType;    /**< ·�����ͣ�����ҵ��/������Ϣ�����Ұ�λ������ͨ��*/
    BYTE    ucExtendFlag;   /**< ��չ��־ */
}JID;

/* IP����ͷ */
typedef struct 
{
   BYTE   ucVerAndHLen;            /* IP�汾��ͷ�� = 0x45 */
   BYTE   ucTOS;                   /* �������� = 0 */
   WORD16 wTotalLen;               /* ���ݱ����� */
   WORD16 wID;                     /* ���ݱ�ID */
   WORD16 wFrag;                   /* �ֶ���ƫ�� */
   BYTE   ucTTL;                   /* ������ */
   BYTE   ucType;                  /* Э�� */
   WORD16 wCheckSum;               /* У��� = 0 */
   WORD32 dwSourceAddr;            /* Դ��ַ */
   WORD32 dwDestAddr;              /* Ŀ�ĵ�ַ */
}T_ISS_IP_Head;

typedef struct 
{
    BYTE    ucRecordType;       /**< ���¼���� = 1 */
    BYTE    ucAuxDataLen;       /**< �������ݳ��� = 0 */
    WORD16  wSourcesNum;        /**< ԴIP��ַ�� = 0 */
    WORD32  dwMultiCastAddress; /**< �鲥IP��ַ */
}T_ISS_Group_Record;

typedef struct 
{
    BYTE     ucType;                                 /**< Э������ = 0x22 */
    BYTE     ucReserved;                             /**< ���� =0 */
    WORD16   wChecksum;                              /**< У��ͣ���ʱ��У�� */
    BYTE     aucReserved[2];                         /**< ���� =0 */
    WORD16   wGroupRecordNum;                        /**< ���¼���� */
    T_ISS_Group_Record  atGroupRecord[ISS_MAX_MGROUP_INFO]; /**< �������м�¼ */
}T_ISS_IGMPV3_Report;
/* IPMPV3ͨ�汨�� */
typedef struct
{
    CHAR                  acDestMac[ETH_HW_ADDR_LEN];        /* Ŀ��MAC */
    CHAR                  acSourceMac[ETH_HW_ADDR_LEN];      /* ԴMAC */
    WORD16                wType;                             /* ���� = 0x800 */
    T_ISS_IP_Head         tIPHead;                           /* IPͷ */ 
    T_ISS_IGMPV3_Report   tIGMPV3;                           /* IGMP V3�������� */
}_PACKED_1_ T_ISS_IGMPV3_PKT;


/*ISS �鲥���Ա��¼*/
typedef struct tagT_IssMgRegRed
{
    BYTE   ucType;                       /* ���� 1-job,2-thread */
    BYTE   ucRegType;                    /* ע������ 2-add,3-exit */
    WORD16 wGroupNo;                     /* �鲥��� */
    WORD32 dwStatus;                     /* ע��ɹ�����ʧ��(ʧ��ԭ��) */
    time_t    tTime;                         /* ʱ�� */
}T_IssMgRegRed;

typedef struct 
{
    WORD32 dwSndJointSuccesss; /*���ͼ���igmp�ɹ�����*/
    WORD32 dwSndExitSuccesss;  /*�����˳�igmp����*/
    WORD32 dwSndJointFails;    /*���ͼ���igmpʧ�ܴ���*/
    WORD32 dwSndExitFails;     /*�����˳�igmpʧ�ܴ���*/
    WORD32 dwRcvBytes;         /*�����ֽ���*/
    WORD32 dwSndBytes;         /*�����ֽ���*/
    WORD32 dwRcvMsgs;          /*������Ϣ��*/
    WORD32 dwSndMsgs;          /*������Ϣ��*/
    WORD32 dwRcvUniCasts;      /*���յ�����Ϣ��*/
    WORD32 dwSndUniCasts;      /*���͵�����Ϣ��*/
    WORD32 dwRcvMultiCasts;    /*�����鲥��Ϣ��*/
    WORD32 dwSndMultiCasts;    /*�����鲥��Ϣ��*/
    WORD32 dwRcvBroadCasts;    /*���չ㲥��Ϣ��*/
    WORD32 dwSndBroadcasts;    /*���͹㲥��Ϣ��*/
}T_ISS_MG_STS;

typedef struct 
{
    BOOLEAN bIsJoin;   /*�Ƿ�����鲥��*/
    BYTE    ucExitSnds;/*�˳��鲥��IGMP���ķ��ʹ��� �ݼ�Ϊ0���ٷ���*/
    WORD16  wGroupNo;  /*�鲥���*/
    WORD32  dwRefCount;/*������鲥���Ӧ����*/
}T_ISS_MG_INFO;

typedef struct
{
    WORD32 dwLock;       /*������*/
    SOCKET iSndSocket;   /*����socket*/
    SOCKET iRcvSocket;   /*����socket*/
    WORD32 dwMgBaseIp;   /*�鲥base IP*/
    WORD32 dwLocalCtlIp; /*������IP*/
    INT    iPortId;      /*�����������*/
    WORD32 wJoinCount;   /*�����鲥�����*/
    WORD32 wExitCount;   /*�˳��鲥�����*/
    BYTE   *pucRcvBuf;   /*���ջ�����*/
    T_ISS_IGMPV3_PKT tIssIgmpV3Pkt; /*IGMP��*/
    T_ISS_MG_STS  tIssMgSts;        /*ͳ��*/
    T_ISS_MG_INFO atIssMgInfo[ISS_MAX_MGROUP_INFO]; /*�鲥����Ϣ*/
}T_ISS_MG_CTRL;

/**************************************************************************
*                            ����                                      *
**************************************************************************/
extern OSS_STATUS R_AddToUdpMultiGroup(WORD16 wGroupNo,JID *ptJid);
extern   OSS_STATUS IssSendIgmpReport(VOID);
extern OSS_STATUS InterFaceGetMac(const char  *ifname, CHAR *ucMac);
extern BOOLEAN InitIgmpv3(VOID);

#endif
