/*********************************************************************
* 版权所有 (C)2009, 深圳市中兴通讯股份有限公司。
*
* 文件名称： snmpd.h
* 文件标识：
* 内容摘要：
* 其它说明： 无
* 当前版本： V1.0
* 作    者： ——3G平台
* 完成日期：
*
* 修改记录1：
*    修改日期：2010年1月13日
*    版 本 号：V1.0
*    修 改 人：曹亮
*    修改内容：创建
**********************************************************************/
#ifndef _SNMPD_H_
#define _SNMPD_H_

#ifndef _PACKED_1_
 #ifndef VOS_WINNT
  #define _PACKED_1_  __attribute__((packed))
 #else
  #define _PACKED_1_
 #endif
#endif

#if defined(_ARCHITECTURE_)
    #pragma noalign     /* noalign for ic386 */
#elif defined(__BORLANDC__)
    #pragma option -a- /* noalign for Borland C */
#elif defined(_MSC_VER)
    #pragma pack(1)    /* noalign for Microsoft C */
#elif defined(__WATCOMC__)
    #pragma pack(1)    /* noalign for Watcom C */
#elif defined(__DIAB)
    #pragma pack(1)    /* noalign for psosystem C */
#endif

/***********************************************************
 *                    其它条件编译选项                     *
***********************************************************/
/***********************************************************
 *                   标准、非标准头文件                    *
***********************************************************/
//#include "tulip_scs.h"
//#include "pub_vmm.h"
//#include "xmlinterface.h"
/* #include "parser.h" */
//#include "oam_cfg.h"
//#include "clireglib.h"
//#include "oam_onlinecfg.h"
//#include "oam_snmp_lib.h"
/* #include "snmp_data_func.h" */
#include <cstdlib>
#include <string>
#include <iostream>
#include <xmlrpc-c/girerr.hpp>
#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/client_simple.hpp>
#include "sky.h"



/***********************************************************
 *                        常量定义                         *
***********************************************************/
#ifndef SWORD
typedef signed short        SWORD;
#endif

#ifndef WORD64
typedef unsigned long long WORD64;
#endif

/***********************************************************
 *                       全局宏                            *
***********************************************************/



#define MASTER_AGENT 0
#define SUB_AGENT    1

#define MAX_REGISTER_OID  50

#define STR_PRINT_MAX     (BYTE)20

#define FUNCID_BSP_BEGIN  (WORD32)0x00020000

#define MAX_XML_REQ_TIMES (BYTE)5

#define OID_NAME 48
#define MAX_VALUE_LENGTH 256

#define IS_BSP_OPT(FUNCID)\
        (((FUNCID) & 0x00FF0000) == FUNCID_BSP_BEGIN)

/*定时扫描注册信息定时器*/
#define     TIMER_SNMP_READ_REGINFO         (TIMER_NO_1)
#define     DURATION_SNMP_READ_REGINFO      (WORD32)(5000)
#define     EV_TIMER_SNMP_READ_REGINFO      (EV_TIMER_1)

/*定时扫描脚本应答信息定时器*/
#define     TIMER_SNMP_XML_ACK              (TIMER_NO_2)
#define     DURATION_SNMP_XML_ACK           (WORD32)(120000)
#define     EV_TIMER_SNMP_XML_ACK           (EV_TIMER_2)

/*心跳上报定时器*/
#define     TIMER_SNMP_HEART_BEAT           (TIMER_NO_3)
#define     DURATION_SNMP_HEART_BEAT        (WORD32)(15000)
#define     EV_TIMER_SNMP_HEART_BEAT        (EV_TIMER_3)

/*---------------------------------------------------
                     参数数据类型
--------------------------------------------------*/
#define     DATA_TYPE_PAD          (BYTE)100
#define     DATA_TYPE_JID          (BYTE)101
#define     DATA_TYPE_ITEM_LIST    (BYTE)102
#define     DATA_TYPE_ITEM_COMMAND (BYTE)103
#define     DATA_TYPE_UNDEFINED    (BYTE)255  

/*---------------------------------------------------
                     参数数据类型
--------------------------------------------------*/
#define DATA_TYPE_BYTE              (BYTE)1     /**< BYTE */
#define DATA_TYPE_WORD              (BYTE)2     /**< WORD16 */
#define DATA_TYPE_DWORD             (BYTE)3     /**< DWORD */
#define DATA_TYPE_INT               (BYTE)4     /**< INT */
#define DATA_TYPE_CHAR              (BYTE)5     /**< CHAR */
#define DATA_TYPE_STRING            (BYTE)6     /**< STRING */
#define DATA_TYPE_TEXT              (BYTE)7     /**< text : many a string  */
#define DATA_TYPE_TABLE             (BYTE)8     /**< represent it is a table varible */
#define DATA_TYPE_IPADDR            (BYTE)9     /**< ip address */
#define DATA_TYPE_DATE              (BYTE)10    /**< DATE = the number of the days from 1900/1/1 */
#define DATA_TYPE_TIME              (BYTE)11    /**< TIME = the number of the seconds from 2000/1/1/00:00:00*/
#define DATA_TYPE_MAP               (BYTE)12    /**< Mapping Variable */
#define DATA_TYPE_LIST              (BYTE)13    /**< represent it is a special Table varible */
#define DATA_TYPE_CONST             (BYTE)14    /**< represent constant variable */
#define DATA_TYPE_IFPORT            (BYTE)15    /**< represent interface port type */
#define DATA_TYPE_HEX               (BYTE)16    /**< 1234.2fcd.e34d */
#define DATA_TYPE_MASK              (BYTE)17    /**< IP MASK */
#define DATA_TYPE_IMASK             (BYTE)18    /**< IP INVERSE MASK */
#define DATA_TYPE_MACADDR           (BYTE)19    /**< Mac Address */
#define DATA_TYPE_FLOAT             (BYTE)20    /**< Float */
#define DATA_TYPE_BOARDNAME         (BYTE)21    /**< BOARDNAME */
#define DATA_TYPE_IPV6PREFIX        (BYTE)22    /**< IPV6 PREFIX */
#define DATA_TYPE_IPV6ADDR          (BYTE)23    /**< IPV6 ADDR */
#define DATA_TYPE_VPN_ASN           (BYTE)24    /**< ASN:nn */
#define DATA_TYPE_VPN_IP            (BYTE)25    /**< IP:nn */
#define DATA_TYPE_PHYADDRESS        (BYTE)26    /**< T_PhysAddress: rack-shelf-slot-cpu*/
#define DATA_TYPE_LOGICADDRESS    (BYTE)27      /**< T_LogicalAddr: system-subsystem-module-unit-sunit-index*/
#define DATA_TYPE_WORD64            (BYTE)28    /**< WORD64 */
#define DATA_TYPE_SWORD             (BYTE)29    /**< SWORD */
#define DATA_TYPE_LOGIC_ADDR_M      (BYTE)30    /**< LogicalAddrM */
#define DATA_TYPE_LOGIC_ADDR_U      (BYTE)31    /**< LogicalAddrU */
#define DATA_TYPE_IPVXADDR          (BYTE)32    /**< IPVXADDR */
#define DATA_TYPE_T_TIME            (BYTE)33    /**< T_TIME */
#define DATA_TYPE_SYSCLOCK          (BYTE)34    /**< SYSCLOCK */
#define DATA_TYPE_VMMDATETIME        (BYTE)35   /**< vmm定义的版本操作时间类型*/
#define DATA_TYPE_VMMDATE            (BYTE)36   /**< vmm定义的版本制作日期类型*/
#define DATA_TYPE_ONLINESTRING            (BYTE)37     /**< DATA_TYPE_ONLINESTRING */
#define DATA_TYPE_PHYADDRESSEX       (BYTE)38    /**< T_PhysAddressEx: rack-shelf-slot-cpu 入参可选，DDM基文档专用*/]
#define DATA_TYPE_PLAN               (BYTE)39
#define DATA_TYPE_UINT32             (BYTE)40
#define DATA_TYPE_PARA              (BYTE)254    /**< Add for [] and whole by wangmh */

#define    XMLRPC_TYPE_INT      0
#define    XMLRPC_TYPE_BOOL     1
#define    XMLRPC_TYPE_DOUBLE   2
#define    XMLRPC_TYPE_DATETIME 3
#define    XMLRPC_TYPE_STRING   4
#define    XMLRPC_TYPE_BASE64   5
#define    XMLRPC_TYPE_ARRAY    6
#define    XMLRPC_TYPE_STRUCT   7
#define    XMLRPC_TYPE_C_PTR    8
#define    XMLRPC_TYPE_NIL      9
#define    XMLRPC_TYPE_I8       10
#define    XMLRPC_TYPE_IPV4     254

/*---------------------------------------------------
                     OID节点IN/OUT属性
--------------------------------------------------*/
#define OID_PROPERTY_IN            (BYTE)0    /*OID属性为IN*/
#define OID_PROPERTY_OUT           (BYTE)1    /*OID属性为OUT*/
#define OID_PROPERTY_IN_AND_OUT    (BYTE)2    /*OID属性为IN/OUT*/

/*---------------------------------------------------
                     OID节点索引属性
--------------------------------------------------*/
#define OID_PROPERTY_NOT_INDEX     (BYTE)0   /*OID属性为非索引*/
#define OID_PROPERTY_INDEX         (BYTE)1   /*OID属性为索引*/

/*---------------------------------------------------
                     OID节点有效性属性
--------------------------------------------------*/
#define OID_PROPERTY_NOT_AVAIABLE  (BYTE)0   /*OID属性为无效*/
#define OID_PROPERTY_AVAIABLE      (BYTE)1   /*OID属性为有效*/

/*---------------------------------------------------
                     OID节点RowStaus属性
--------------------------------------------------*/
#define OID_PROPERTY_NOT_ROWSTATUS (BYTE)0   /*OID属性为非RowStatus*/
#define OID_PROPERTY_ROWSTATUS     (BYTE)1   /*OID属性为RowStatus*/

/*---------------------------------------------------
                     OID节点读写属性
--------------------------------------------------*/
#define OID_PROPERTY_READ          (BYTE)0   /*OID属性为只读*/
#define OID_PROPERTY_WRITE         (BYTE)1   /*OID属性为只写*/
#define OID_PROPERTY_RW            (BYTE)2   /*OID属性为读写*/

/*---------------------------------------------------
                     MIB OID根节点标量/表量属性
--------------------------------------------------*/
#define OID_PROPERTY_SCALAR        (BYTE)0   /*OID属性为标量*/
#define OID_PROPERTY_TABLE         (BYTE)1   /*OID属性为表量*/

/*---------------------------------------------------
                     OID结构相关宏定义
--------------------------------------------------*/
#define MAX_OID_LENGTH             40
#define MAX_OID_NUMBER             100
#define MAX_MODULE_NUMBER          20

/*---------------------------------------------------
                     注册信息相关宏定义
--------------------------------------------------*/
#define XML_HAVE_NO_ACK           0
#define XML_HAVE_ACK              1

/*---------------------------------------------------
                     IPMI索引相关宏定义
--------------------------------------------------*/
#define IPMI_INDEX_INT           0
#define IPMI_INDEX_INT_STR       1
#define IPMI_INDEX_INT_INT       2
#define IPMI_INDEX_INT_INT_STR   3
#define IPMI_INDEX_NOT_DEFINE    0xff

/*---------------------------------------------------
                     IPMI索引相关宏定义
--------------------------------------------------*/
#define FUNCID_NOT_SUPPORT_MODIFY  0
#define FUNCID_SUPPORT_MODIFY      1

#define OID_PROPERTY_RESPONSE      (BYTE)0   /*OID属性为RESPONSE做应答*/
#define OID_PROPERTY_TRAP          (BYTE)1   /*OID属性为TRAP做应答*/

#define SCAN_FROM_TABLE_HEAD  0x04000000

#define     STR_VALUE_MAX          (BYTE)20

#define SNMP_SAFE_RETUB(P)\
                if (P != NULL)\
                {\
                    free(P);    \
                    P = NULL;\
                }

#define SNMP_PRINT_OID(S, O, L)\
    do\
    {\
        WORD wLoop = 0;\
        g_pszJName = OamGetLogFlag();\
        if (g_pszJName == NULL)\
        {\
            break;\
        }\
        /* printf("--------%s begin--------\n", S); */\
        /* printf("oidlen = %u\n", (L)); */\
        for(wLoop=0; wLoop<(L); wLoop++)\
        {\
            if ((wLoop+1) == (L))\
            {\
                /* printf("%lu\n",O[wLoop]); */\
            }\
            else\
            {\
                /* printf("%lu.", O[wLoop]); */\
            }\
        }\
        /* printf("--------%s end--------\n", S); */\
     }while(0)

/*---------------------------------------------------
                     告警模块注册相关宏定义
--------------------------------------------------*/
#define ALARM_NOT_REG  0 
#define ALARM_REG      1

#define     STR_VALUE_MAX          (BYTE)20

extern int      agent_role;

extern int      snmp_dump_packet;
extern int      verbose;
extern int      (*sd_handlers[]) (int);
extern int      smux_listen_sd;

/***********************************************************
 *                     全局数据类型                        *
 ***********************************************************/

typedef union 
{
    BYTE           ucByte;        /*BYTE/T_PAD类型的值*/
    WORD         wWord;         /* WORD类型的值*/
    SWORD         sWord;         /* SWORD类型的值*/
    WORD32         dwWord;        /* WORD32/MAP/IPADDRESS类型的值*/
    WORD64         ddwWord;       /* WORD64类型的值*/
//    JID            tJid;          /* JID类型的值*/
//    T_PhysAddress  tPhyAddress;   /* 物理地址类型的值*/
//    T_LogicalAddr  tLogAddress;   /* 逻辑地址类型的值*/
//    T_LogicalAddrM  tLogAddressM;   /* 逻辑地址M类型的值*/
//   T_LogicalAddrU  tLogAddressU;   /* 逻辑地址U类型的值*/
//    T_Time        tTime;           /* 时间类型的值*/
//    T_SysSoftClock        tSysClock;/* 系统时间类型的值*/
    char   cStr[STR_VALUE_MAX];   /* CHAR类型的值*/
}_PACKED_1_ T_SNMP_NodeValue;

struct _T_Oid_Description;
typedef struct _T_Oid_Property
{
    BYTE    ucParaType;           /*节点的类型*/
    BYTE    ucAvailProp;          /*节点有效性*/
    WORD    wArrayLen;            /*节点的数组长度*/
    BYTE    ucInOutProp;          /*节点InOut属性*/
    BYTE    ucIndexProp;          /*节点索引属性*/
    BYTE    ucRowStateProp;       /*节点RowStatus属性*/
    BYTE    ucPosNo;              /*节点的位置号*/
    BYTE    ucRwProp;             /*节点的读写属性*/
    WORD    wParaLen;             /*节点长度*/
    BYTE    ucParaXmlRpcType;
    oid     OidName[MAX_OID_LENGTH];/*节点的OID信息*/
    char    oidName[OID_NAME];
    WORD    wOidLen;              /*OID节点长度*/
    BYTE    ucPad1[6];
    T_SNMP_NodeValue     *pMaxValue;    /*节点最大值*/
    T_SNMP_NodeValue     *pMinValue;    /*节点最小值*/
    struct  _T_Oid_Property *pNext;/*下一个节点*/
    struct _T_Oid_Description *ptOwerOidDesc;/* 归属的module 节点*/
}_PACKED_1_ T_Oid_Property;

#define ENTRY_DATA_NO_COMMIT 0
#define ENTRY_DATA_COMMIT 1

#define ENTRY_DATA_NO_NEED_COMMIT 0
#define ENTRY_DATA_NEED_COMMIT 1
#define ENTRY_DATA_NEED_DELETE 2

#define ENTRY_DATA_CHECK_RIGHT 0
#define ENTRY_DATA_CHECK_WRONG 1

typedef struct _T_Send_Struct
{
    WORD    wIndexLength;         /*该行数据索引长度*/
    BYTE      ucNeedCommit;
    BYTE      ucCommit; /* 该行数据是否已经提交, 0未提交，1、已提交*/
    BYTE      ucCheckFlag; /* 该行数据校验是否正确, 0、正确，1、不正确*/
    BYTE     *pStruct;              /*等待发送的结构内容*/
    struct  _T_Send_Struct *pNext;  /*等待发送内容的下一个节点*/
}_PACKED_1_ T_Send_Struct;

struct _T_Module_Snmp_Description;
typedef struct _T_Oid_Description
{
    BYTE    ucOidType;            /*节点的标量/表量属性*/
    BYTE    ucAckType;            /*节点的应答属性*/
    BYTE    ucFuncType;           /*功能属性*/
    BYTE    ucIndexNum;           /*如果是表量节点*/
    BYTE    ucModifyProp;         /*该功能ID是否支持增删命令*/
    BYTE    ucRwProp;    
    WORD    wIndex_name_len;
    WORD    wPad;
    WORD    wIndexLen;
    WORD32  dwFuncID;             /*功能ID*/
    oid     BaseOidName[MAX_OID_LENGTH];/*节点OID*/
    WORD    wBaseOidLen;          /*OID节点长度*/
    BYTE    ucPad1[2];
    char    oidName[OID_NAME];
    WORD32  dwStructLength;       /*等待发送的内容长度*/
    T_Send_Struct  *pSendStructHead; /*等待发送的结构头节点*/
    T_Send_Struct  *pSendStructTail; /*等待发送的结构尾节点*/
    T_Oid_Property *pOidHead;     /*OID的首节点*/
    T_Oid_Property *pOidTail;     /*OID的尾节点*/
    struct _T_Module_Snmp_Description *ptOwerModuleDesc; /* 归属的Module */
}_PACKED_1_ T_Oid_Description;

typedef struct _T_DDM_Ack_Struct
{
//    T_DDM_Ack tDdmAck;                   /* DDM应答消息 */
    struct    _T_DDM_Ack_Struct  *pNext;   /* 链表下一个节点 */
}_PACKED_1_ T_DDM_Ack_Struct;

typedef struct _T_Module_Snmp_Description
{
    WORD  wOidNum;             /*该模块中OID节点个数*/
    WORD  wFuncType;           /*模块功能类型*/
//    JID     tJid;                /*注册模块JID*/
    BYTE    ucXMLAckFlag;        /*对VMM请求XML的应答消息*/
    BYTE    ucXMLReqTimes;       /*对VMM请求XML的应答消息*/
    BYTE    ucPad[2];
    WORD32  wRootOidLen; /* add by liuhzh SnmpProtocolSearchOid 中加快检索速度*/
    oid     rootOid[MAX_OID_LENGTH];/* add by liuhzh SnmpProtocolSearchOid 中加快检索速度*/
    T_Oid_Description tOidDesCrip[MAX_OID_NUMBER]; /*OID描述结构信息*/
}_PACKED_1_ T_Module_Snmp_Description;

typedef struct
{
	BYTE ucMSState;                       /* 单板主备状态 */  
	BYTE ucPriJidNum;                     /* 网管JID个数 */
	WORD wProcState;                    /* 进程状态 */
//	JID  tInterfaceJid;                   /* INTERFACE JOB的JID信息*/
	BYTE ucModuleNum;                     /* 已注册的模块个数*/
	BYTE ucAlarmRegFlag;                  /* 是否向ALARM注册标识 */
	BYTE ucPad[2];
	T_DDM_Ack_Struct *pDdmAckHead;        /* 缓存应答消息链表头 */
//	XOS_TASK_T tSnmpProtocolThreadID;     /* SNMP协议线程的Task ID*/
	T_Module_Snmp_Description tModuleDescrip[MAX_MODULE_NUMBER]; /*模块SNMP的XML描述信息*/
}_PACKED_1_ T_SNMPProc_GlobVal;


 /***********************************************************
 *                     全局函数原型                        *
***********************************************************/
extern int      snmp_read_packet(int);
void            agentBoots_conf(char *, char *);
void SnmpProtocolRegisterAllModule();

#endif

