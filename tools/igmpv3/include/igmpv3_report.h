/*********************************************************************
* 版权所有 (C)2012, 深圳市中兴通讯股份有限公司。
*
* 文件名称： igmpv3_report.h
* 文件标识： 
* 其它说明： 
* 当前版本： V1.0
* 作    者： liuxuefeng	
* 完成日期： 
*
* 修改记录1：
*    修改日期：2012年03月28日
*    版 本 号：V1.0
*    修 改 人：liuxuefeng
*    修改内容：创建
**********************************************************************/
#ifndef  _IGMPV3_REPORT_H_
#define  _IGMPV3_REPORT_H_
/**************************************************************************
*                            头文件                                        *
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
*                            宏                                          *
**************************************************************************/
/* 定义支撑一些基础数据结构 */
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

#define VOID             void /* g++  bug规避 */

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

typedef WORD32  XOS_STATUS;            /**< 支撑操作结果状态类型,示例：XOS_SUCCESS */
typedef unsigned long       OSS_STATUS;/**< OSS操作结果状态类型,示例： OSS_SUCCESS */

/**************************************************************************
*                            常量                                        *
**************************************************************************/
    /* 定义支撑一些通用宏 */
#define TULIP_ERROR_BEGIN     (WORD32)0                                           /**< tulip错误码基准码 */
#define TULIP_ERROR_END        (WORD32)(TULIP_ERROR_BEGIN+0x80000)   /**< tulip错误码结束基准码 */  
#define XOS_ERR_BEGIN              TULIP_ERROR_BEGIN   /**< XOS错误码基准码 */
#define XOS_SUCCESS                  XOS_ERR_BEGIN         /**< 成功 */
#define XOS_ERR_UNKNOWN        XOS_ERR_BEGIN+1     /**< XOS未知错误 */
#define XOS_ERROR_UNKNOWN    XOS_ERR_UNKNOWN  /**< XOS未知错误 */

#define OSS_ERR_BEGIN          XOS_ERR_BEGIN                          /**< OSS 错误码基准码 */
#define OSS_SUCCESS            XOS_SUCCESS                               /**<  成功 */
#define OSS_ERR_UNKNOWN        XOS_ERR_UNKNOWN                /**< OSS 未知错误 */
#define OSS_ERROR_UNKNOWN      XOS_ERROR_UNKNOWN         /**< OSS 未知错误 */
#define OSS_ERROR_SYSCALL         XOS_ERR_BEGIN+2         /**< OSS 未知错误 */

    /* IGMP_V3相关宏 */
#define ETH_HW_ADDR_LEN                 ((BYTE)6)        /* MAC地址字节数 */
#define IP_ADDR_LEN                           ((BYTE)4)        /* IPV4地址字节数 */
#define DEF_ISS_IGMP_DSTIP              inet_addr("224.0.0.22") /* IGMP目的ip地址 */
#define DEF_ISS_IGMP_DSTMAC             "\x1\x0\x5e\x0\x0\x16"  /* IGMP目的mac地址 */
#define ISS_IGMP_EXIT_REC_TYPE          ((BYTE)0x3)     /* IGMP V3组播组记录类型 退出组播组 */
#define ISS_IGMP_JOIN_REC_TYPE          ((BYTE)0x4)     /* IGMP V3组播组记录类型 加入组播组 */
#define ISS_FRAME_LEN                   ((WORD16)14)    /* 以太网帧长 */
#define ISS_IP_LEN                      ((WORD16)20)    /* IP头长,不含以太网帧 */
#define ISS_IGMP_HEAD_LEN               ((WORD16)8)     /* IGMP 头长*/
#define ISS_FRAME_TYPE                  ((WORD16)0x800) /* 以太网帧类型 */
#define ISS_IP_VER                      ((BYTE)0x45)    /* IP版本 */
#define ISS_IP_IGMP_TTL                 ((BYTE)0x1)     /* IP TTL字段 */
#define ISS_IP_IGMP_TYPE                ((BYTE)0x2)     /* IP协议类型(IGMPV3) */
#define ISS_IGMPV3_VER                  ((BYTE)0x22)    /* IGMP V3类型协议 */
#define ISS_MAX_MGROUP_INFO             ((BYTE)10)      /* 组播组记录 */
#define ISS_MAX_EXIT_SNDS               ((BYTE)3)       /* 退出组播组IGMP report最大次数 */
#define ISS_INVALID_GROUP_NO            ((WORD16)0xFFFF)/* 无效组播组号 */    
#define ISS_BASE_IF_NAME                     "eth6"           /* BASE面网口暂时固定为eth6 */

/*from pub_oss.h*/
#define ISS_MUTICAST_BASE_ADDR     "228.228.228.228"     /**< 组播地址的起始值 */
#define ISS_MAX_MGROUP_NO          ((WORD16)(60000))    /**< ISS组播支持的最大组播号 */
#define ISS_MSG_UDP_PORT           ((WORD16)23456)       /**< ISS套接字绑定端口 */
#define ISS_MSG_MAX_LEN            ((WORD16)(8*1024))    /**< ISS消息最大长度 */
#define ISS_MSG_HEAD_VER0          ((BYTE)0)             /**< ISS消息消息头版本号 */
    
#define ISS_MSG_TYPE_SINGLE        ((BYTE)1) /**< 单播消息*/
#define ISS_MSG_TYPE_MULTI         ((BYTE)2) /**< 多(组)播消息*/
#define ISS_MSG_TYPE_BROAD         ((BYTE)3) /**< 广播消息 暂不支持*/
/* OSS 组播范围：[0X0401, 0X0500] */
#define MG_PLAT_OSS_BEGIN  0X0401
#define MG_PLAT_OSS_IGMP_V3              (MG_PLAT_OSS_BEGIN+0x0fe)  /**< IGMP_V3消息组播组*/
#define MG_PLAT_OSS_END    0X0500
extern BOOLEAN              g_bIssIgmpPrnEn;                          /*ISS消息打印开关*/
#define IGMP_Print(format, arg...)  \
do {               \
         if (TRUE == g_bIssIgmpPrnEn)  \
         {                                               \
             printf(format, ##arg);           \
         }                                               \
}while(0)

/**************************************************************************
*                             变量                                      *
**************************************************************************/

/**************************************************************************
*                          数据类型                                       *
**************************************************************************/
typedef struct tagJID
{
    WORD32  dwJno;          /**< JOB号, 高16位为JOB 类型, 低16位为JOB 实例号 */
    WORD32  dwDevId;        /**< 设备ID, 包含本局的局号信息, 高8位为局号，不支持传递其他信息*/
    WORD16  wModule;        /**< MP编号 */
    WORD16  wUnit;          /**< 单板号 */
    BYTE    ucSUnit;        /**< 单板上的处理器号 */
    BYTE    ucSubSystem;    /**< 子系统号 */
    BYTE    ucRouteType;    /**< 路由类型，区别业务/管理消息，左右板位，主备通信*/
    BYTE    ucExtendFlag;   /**< 扩展标志 */
}JID;

/* IP报文头 */
typedef struct 
{
   BYTE   ucVerAndHLen;            /* IP版本和头长 = 0x45 */
   BYTE   ucTOS;                   /* 服务类型 = 0 */
   WORD16 wTotalLen;               /* 数据报长度 */
   WORD16 wID;                     /* 数据报ID */
   WORD16 wFrag;                   /* 分段与偏移 */
   BYTE   ucTTL;                   /* 生存期 */
   BYTE   ucType;                  /* 协议 */
   WORD16 wCheckSum;               /* 校验和 = 0 */
   WORD32 dwSourceAddr;            /* 源地址 */
   WORD32 dwDestAddr;              /* 目的地址 */
}T_ISS_IP_Head;

typedef struct 
{
    BYTE    ucRecordType;       /**< 组记录类型 = 1 */
    BYTE    ucAuxDataLen;       /**< 辅助数据长度 = 0 */
    WORD16  wSourcesNum;        /**< 源IP地址数 = 0 */
    WORD32  dwMultiCastAddress; /**< 组播IP地址 */
}T_ISS_Group_Record;

typedef struct 
{
    BYTE     ucType;                                 /**< 协议类型 = 0x22 */
    BYTE     ucReserved;                             /**< 保留 =0 */
    WORD16   wChecksum;                              /**< 校验和，暂时不校验 */
    BYTE     aucReserved[2];                         /**< 保留 =0 */
    WORD16   wGroupRecordNum;                        /**< 组记录数量 */
    T_ISS_Group_Record  atGroupRecord[ISS_MAX_MGROUP_INFO]; /**< 保存所有记录 */
}T_ISS_IGMPV3_Report;
/* IPMPV3通告报文 */
typedef struct
{
    CHAR                  acDestMac[ETH_HW_ADDR_LEN];        /* 目的MAC */
    CHAR                  acSourceMac[ETH_HW_ADDR_LEN];      /* 源MAC */
    WORD16                wType;                             /* 类型 = 0x800 */
    T_ISS_IP_Head         tIPHead;                           /* IP头 */ 
    T_ISS_IGMPV3_Report   tIGMPV3;                           /* IGMP V3报文数据 */
}_PACKED_1_ T_ISS_IGMPV3_PKT;


/*ISS 组播组成员记录*/
typedef struct tagT_IssMgRegRed
{
    BYTE   ucType;                       /* 类型 1-job,2-thread */
    BYTE   ucRegType;                    /* 注册类型 2-add,3-exit */
    WORD16 wGroupNo;                     /* 组播组号 */
    WORD32 dwStatus;                     /* 注册成功或者失败(失败原因) */
    time_t    tTime;                         /* 时间 */
}T_IssMgRegRed;

typedef struct 
{
    WORD32 dwSndJointSuccesss; /*发送加入igmp成功次数*/
    WORD32 dwSndExitSuccesss;  /*发送退出igmp次数*/
    WORD32 dwSndJointFails;    /*发送加入igmp失败次数*/
    WORD32 dwSndExitFails;     /*发送退出igmp失败次数*/
    WORD32 dwRcvBytes;         /*接收字节数*/
    WORD32 dwSndBytes;         /*发送字节数*/
    WORD32 dwRcvMsgs;          /*接收消息数*/
    WORD32 dwSndMsgs;          /*发送消息数*/
    WORD32 dwRcvUniCasts;      /*接收单播消息数*/
    WORD32 dwSndUniCasts;      /*发送单播消息数*/
    WORD32 dwRcvMultiCasts;    /*接收组播消息数*/
    WORD32 dwSndMultiCasts;    /*发送组播消息数*/
    WORD32 dwRcvBroadCasts;    /*接收广播消息数*/
    WORD32 dwSndBroadcasts;    /*发送广播消息数*/
}T_ISS_MG_STS;

typedef struct 
{
    BOOLEAN bIsJoin;   /*是否加入组播组*/
    BYTE    ucExitSnds;/*退出组播组IGMP报文发送次数 递减为0不再发送*/
    WORD16  wGroupNo;  /*组播组号*/
    WORD32  dwRefCount;/*加入该组播组的应用数*/
}T_ISS_MG_INFO;

typedef struct
{
    WORD32 dwLock;       /*互斥锁*/
    SOCKET iSndSocket;   /*发送socket*/
    SOCKET iRcvSocket;   /*接收socket*/
    WORD32 dwMgBaseIp;   /*组播base IP*/
    WORD32 dwLocalCtlIp; /*控制面IP*/
    INT    iPortId;      /*控制面口索引*/
    WORD32 wJoinCount;   /*加入组播组计数*/
    WORD32 wExitCount;   /*退出组播组计数*/
    BYTE   *pucRcvBuf;   /*接收缓冲区*/
    T_ISS_IGMPV3_PKT tIssIgmpV3Pkt; /*IGMP包*/
    T_ISS_MG_STS  tIssMgSts;        /*统计*/
    T_ISS_MG_INFO atIssMgInfo[ISS_MAX_MGROUP_INFO]; /*组播组信息*/
}T_ISS_MG_CTRL;

/**************************************************************************
*                            函数                                      *
**************************************************************************/
extern OSS_STATUS R_AddToUdpMultiGroup(WORD16 wGroupNo,JID *ptJid);
extern   OSS_STATUS IssSendIgmpReport(VOID);
extern OSS_STATUS InterFaceGetMac(const char  *ifname, CHAR *ucMac);
extern BOOLEAN InitIgmpv3(VOID);

#endif
