/*********************************************************************
* ��Ȩ���� (C)2009, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ� snmpd.h
* �ļ���ʶ��
* ����ժҪ��
* ����˵���� ��
* ��ǰ�汾�� V1.0
* ��    �ߣ� ����3Gƽ̨
* ������ڣ�
*
* �޸ļ�¼1��
*    �޸����ڣ�2010��1��13��
*    �� �� �ţ�V1.0
*    �� �� �ˣ�����
*    �޸����ݣ�����
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
 *                    ������������ѡ��                     *
***********************************************************/
/***********************************************************
 *                   ��׼���Ǳ�׼ͷ�ļ�                    *
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
 *                        ��������                         *
***********************************************************/
#ifndef SWORD
typedef signed short        SWORD;
#endif

#ifndef WORD64
typedef unsigned long long WORD64;
#endif

/***********************************************************
 *                       ȫ�ֺ�                            *
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

/*��ʱɨ��ע����Ϣ��ʱ��*/
#define     TIMER_SNMP_READ_REGINFO         (TIMER_NO_1)
#define     DURATION_SNMP_READ_REGINFO      (WORD32)(5000)
#define     EV_TIMER_SNMP_READ_REGINFO      (EV_TIMER_1)

/*��ʱɨ��ű�Ӧ����Ϣ��ʱ��*/
#define     TIMER_SNMP_XML_ACK              (TIMER_NO_2)
#define     DURATION_SNMP_XML_ACK           (WORD32)(120000)
#define     EV_TIMER_SNMP_XML_ACK           (EV_TIMER_2)

/*�����ϱ���ʱ��*/
#define     TIMER_SNMP_HEART_BEAT           (TIMER_NO_3)
#define     DURATION_SNMP_HEART_BEAT        (WORD32)(15000)
#define     EV_TIMER_SNMP_HEART_BEAT        (EV_TIMER_3)

/*---------------------------------------------------
                     ������������
--------------------------------------------------*/
#define     DATA_TYPE_PAD          (BYTE)100
#define     DATA_TYPE_JID          (BYTE)101
#define     DATA_TYPE_ITEM_LIST    (BYTE)102
#define     DATA_TYPE_ITEM_COMMAND (BYTE)103
#define     DATA_TYPE_UNDEFINED    (BYTE)255  

/*---------------------------------------------------
                     ������������
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
#define DATA_TYPE_VMMDATETIME        (BYTE)35   /**< vmm����İ汾����ʱ������*/
#define DATA_TYPE_VMMDATE            (BYTE)36   /**< vmm����İ汾������������*/
#define DATA_TYPE_ONLINESTRING            (BYTE)37     /**< DATA_TYPE_ONLINESTRING */
#define DATA_TYPE_PHYADDRESSEX       (BYTE)38    /**< T_PhysAddressEx: rack-shelf-slot-cpu ��ο�ѡ��DDM���ĵ�ר��*/]
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
                     OID�ڵ�IN/OUT����
--------------------------------------------------*/
#define OID_PROPERTY_IN            (BYTE)0    /*OID����ΪIN*/
#define OID_PROPERTY_OUT           (BYTE)1    /*OID����ΪOUT*/
#define OID_PROPERTY_IN_AND_OUT    (BYTE)2    /*OID����ΪIN/OUT*/

/*---------------------------------------------------
                     OID�ڵ���������
--------------------------------------------------*/
#define OID_PROPERTY_NOT_INDEX     (BYTE)0   /*OID����Ϊ������*/
#define OID_PROPERTY_INDEX         (BYTE)1   /*OID����Ϊ����*/

/*---------------------------------------------------
                     OID�ڵ���Ч������
--------------------------------------------------*/
#define OID_PROPERTY_NOT_AVAIABLE  (BYTE)0   /*OID����Ϊ��Ч*/
#define OID_PROPERTY_AVAIABLE      (BYTE)1   /*OID����Ϊ��Ч*/

/*---------------------------------------------------
                     OID�ڵ�RowStaus����
--------------------------------------------------*/
#define OID_PROPERTY_NOT_ROWSTATUS (BYTE)0   /*OID����Ϊ��RowStatus*/
#define OID_PROPERTY_ROWSTATUS     (BYTE)1   /*OID����ΪRowStatus*/

/*---------------------------------------------------
                     OID�ڵ��д����
--------------------------------------------------*/
#define OID_PROPERTY_READ          (BYTE)0   /*OID����Ϊֻ��*/
#define OID_PROPERTY_WRITE         (BYTE)1   /*OID����Ϊֻд*/
#define OID_PROPERTY_RW            (BYTE)2   /*OID����Ϊ��д*/

/*---------------------------------------------------
                     MIB OID���ڵ����/��������
--------------------------------------------------*/
#define OID_PROPERTY_SCALAR        (BYTE)0   /*OID����Ϊ����*/
#define OID_PROPERTY_TABLE         (BYTE)1   /*OID����Ϊ����*/

/*---------------------------------------------------
                     OID�ṹ��غ궨��
--------------------------------------------------*/
#define MAX_OID_LENGTH             40
#define MAX_OID_NUMBER             100
#define MAX_MODULE_NUMBER          20

/*---------------------------------------------------
                     ע����Ϣ��غ궨��
--------------------------------------------------*/
#define XML_HAVE_NO_ACK           0
#define XML_HAVE_ACK              1

/*---------------------------------------------------
                     IPMI������غ궨��
--------------------------------------------------*/
#define IPMI_INDEX_INT           0
#define IPMI_INDEX_INT_STR       1
#define IPMI_INDEX_INT_INT       2
#define IPMI_INDEX_INT_INT_STR   3
#define IPMI_INDEX_NOT_DEFINE    0xff

/*---------------------------------------------------
                     IPMI������غ궨��
--------------------------------------------------*/
#define FUNCID_NOT_SUPPORT_MODIFY  0
#define FUNCID_SUPPORT_MODIFY      1

#define OID_PROPERTY_RESPONSE      (BYTE)0   /*OID����ΪRESPONSE��Ӧ��*/
#define OID_PROPERTY_TRAP          (BYTE)1   /*OID����ΪTRAP��Ӧ��*/

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
                     �澯ģ��ע����غ궨��
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
 *                     ȫ����������                        *
 ***********************************************************/

typedef union 
{
    BYTE           ucByte;        /*BYTE/T_PAD���͵�ֵ*/
    WORD         wWord;         /* WORD���͵�ֵ*/
    SWORD         sWord;         /* SWORD���͵�ֵ*/
    WORD32         dwWord;        /* WORD32/MAP/IPADDRESS���͵�ֵ*/
    WORD64         ddwWord;       /* WORD64���͵�ֵ*/
//    JID            tJid;          /* JID���͵�ֵ*/
//    T_PhysAddress  tPhyAddress;   /* �����ַ���͵�ֵ*/
//    T_LogicalAddr  tLogAddress;   /* �߼���ַ���͵�ֵ*/
//    T_LogicalAddrM  tLogAddressM;   /* �߼���ַM���͵�ֵ*/
//   T_LogicalAddrU  tLogAddressU;   /* �߼���ַU���͵�ֵ*/
//    T_Time        tTime;           /* ʱ�����͵�ֵ*/
//    T_SysSoftClock        tSysClock;/* ϵͳʱ�����͵�ֵ*/
    char   cStr[STR_VALUE_MAX];   /* CHAR���͵�ֵ*/
}_PACKED_1_ T_SNMP_NodeValue;

struct _T_Oid_Description;
typedef struct _T_Oid_Property
{
    BYTE    ucParaType;           /*�ڵ������*/
    BYTE    ucAvailProp;          /*�ڵ���Ч��*/
    WORD    wArrayLen;            /*�ڵ�����鳤��*/
    BYTE    ucInOutProp;          /*�ڵ�InOut����*/
    BYTE    ucIndexProp;          /*�ڵ���������*/
    BYTE    ucRowStateProp;       /*�ڵ�RowStatus����*/
    BYTE    ucPosNo;              /*�ڵ��λ�ú�*/
    BYTE    ucRwProp;             /*�ڵ�Ķ�д����*/
    WORD    wParaLen;             /*�ڵ㳤��*/
    BYTE    ucParaXmlRpcType;
    oid     OidName[MAX_OID_LENGTH];/*�ڵ��OID��Ϣ*/
    char    oidName[OID_NAME];
    WORD    wOidLen;              /*OID�ڵ㳤��*/
    BYTE    ucPad1[6];
    T_SNMP_NodeValue     *pMaxValue;    /*�ڵ����ֵ*/
    T_SNMP_NodeValue     *pMinValue;    /*�ڵ���Сֵ*/
    struct  _T_Oid_Property *pNext;/*��һ���ڵ�*/
    struct _T_Oid_Description *ptOwerOidDesc;/* ������module �ڵ�*/
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
    WORD    wIndexLength;         /*����������������*/
    BYTE      ucNeedCommit;
    BYTE      ucCommit; /* ���������Ƿ��Ѿ��ύ, 0δ�ύ��1�����ύ*/
    BYTE      ucCheckFlag; /* ��������У���Ƿ���ȷ, 0����ȷ��1������ȷ*/
    BYTE     *pStruct;              /*�ȴ����͵Ľṹ����*/
    struct  _T_Send_Struct *pNext;  /*�ȴ��������ݵ���һ���ڵ�*/
}_PACKED_1_ T_Send_Struct;

struct _T_Module_Snmp_Description;
typedef struct _T_Oid_Description
{
    BYTE    ucOidType;            /*�ڵ�ı���/��������*/
    BYTE    ucAckType;            /*�ڵ��Ӧ������*/
    BYTE    ucFuncType;           /*��������*/
    BYTE    ucIndexNum;           /*����Ǳ����ڵ�*/
    BYTE    ucModifyProp;         /*�ù���ID�Ƿ�֧����ɾ����*/
    BYTE    ucRwProp;    
    WORD    wIndex_name_len;
    WORD    wPad;
    WORD    wIndexLen;
    WORD32  dwFuncID;             /*����ID*/
    oid     BaseOidName[MAX_OID_LENGTH];/*�ڵ�OID*/
    WORD    wBaseOidLen;          /*OID�ڵ㳤��*/
    BYTE    ucPad1[2];
    char    oidName[OID_NAME];
    WORD32  dwStructLength;       /*�ȴ����͵����ݳ���*/
    T_Send_Struct  *pSendStructHead; /*�ȴ����͵Ľṹͷ�ڵ�*/
    T_Send_Struct  *pSendStructTail; /*�ȴ����͵Ľṹβ�ڵ�*/
    T_Oid_Property *pOidHead;     /*OID���׽ڵ�*/
    T_Oid_Property *pOidTail;     /*OID��β�ڵ�*/
    struct _T_Module_Snmp_Description *ptOwerModuleDesc; /* ������Module */
}_PACKED_1_ T_Oid_Description;

typedef struct _T_DDM_Ack_Struct
{
//    T_DDM_Ack tDdmAck;                   /* DDMӦ����Ϣ */
    struct    _T_DDM_Ack_Struct  *pNext;   /* ������һ���ڵ� */
}_PACKED_1_ T_DDM_Ack_Struct;

typedef struct _T_Module_Snmp_Description
{
    WORD  wOidNum;             /*��ģ����OID�ڵ����*/
    WORD  wFuncType;           /*ģ�鹦������*/
//    JID     tJid;                /*ע��ģ��JID*/
    BYTE    ucXMLAckFlag;        /*��VMM����XML��Ӧ����Ϣ*/
    BYTE    ucXMLReqTimes;       /*��VMM����XML��Ӧ����Ϣ*/
    BYTE    ucPad[2];
    WORD32  wRootOidLen; /* add by liuhzh SnmpProtocolSearchOid �мӿ�����ٶ�*/
    oid     rootOid[MAX_OID_LENGTH];/* add by liuhzh SnmpProtocolSearchOid �мӿ�����ٶ�*/
    T_Oid_Description tOidDesCrip[MAX_OID_NUMBER]; /*OID�����ṹ��Ϣ*/
}_PACKED_1_ T_Module_Snmp_Description;

typedef struct
{
	BYTE ucMSState;                       /* ��������״̬ */  
	BYTE ucPriJidNum;                     /* ����JID���� */
	WORD wProcState;                    /* ����״̬ */
//	JID  tInterfaceJid;                   /* INTERFACE JOB��JID��Ϣ*/
	BYTE ucModuleNum;                     /* ��ע���ģ�����*/
	BYTE ucAlarmRegFlag;                  /* �Ƿ���ALARMע���ʶ */
	BYTE ucPad[2];
	T_DDM_Ack_Struct *pDdmAckHead;        /* ����Ӧ����Ϣ����ͷ */
//	XOS_TASK_T tSnmpProtocolThreadID;     /* SNMPЭ���̵߳�Task ID*/
	T_Module_Snmp_Description tModuleDescrip[MAX_MODULE_NUMBER]; /*ģ��SNMP��XML������Ϣ*/
}_PACKED_1_ T_SNMPProc_GlobVal;


 /***********************************************************
 *                     ȫ�ֺ���ԭ��                        *
***********************************************************/
extern int      snmp_read_packet(int);
void            agentBoots_conf(char *, char *);
void SnmpProtocolRegisterAllModule();

#endif

