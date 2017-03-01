/*********************************************************************
* ��Ȩ���� (C)2008, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ� DDM_SNMP.h
* �ļ���ʶ��
* ����ժҪ��
* ����˵���� ��
* ��ǰ�汾�� V1.0
* ��    �ߣ�
* ������ڣ�
**********************************************************************/
#ifndef DDM_SNMP_H_INCLUDE_
#define DDM_SNMP_H_INCLUDE_

/***********************************************************
                   ������������ѡ��                     
***********************************************************/

/***********************************************************
                   ��׼���Ǳ�׼ͷ�ļ�                     
***********************************************************/
#define        MAX_STRING        20
#define        MAX_ACL_NUMBER    50


#define        SNMP_SUCCESS        0
#define        SNMP_FAIL               (WORD32)DDM_RETCODE_SNMP_BEGIN+0
#define        SNMP_ERR_UNKNOWN        (WORD32)DDM_RETCODE_SNMP_BEGIN+1
#define        SNMP_RECORD_NULL        (WORD32)DDM_RETCODE_SNMP_BEGIN+2
#define        SNMP_RECORD_FULL        (WORD32)DDM_RETCODE_SNMP_BEGIN+3
#define        SNMP_RECORD_EXIST       (WORD32)DDM_RETCODE_SNMP_BEGIN+4

#define        SNMP_RETCODE_SUCC_AND_NOPARA 16 /* �����ɹ������ݷ���*/
#define        SNMP_TRAP_ENABLE        0
#define        SNMP_TRAP_DISABLE        1
#define        SNMP_TRAP_VERSION_1        0
#define        SNMP_TRAP_VERSION_2        1

/* SNMP��ַ�����б� */
#define        FUNCID_OAM_SNMP_ACL_CONFIG        (WORD32)0x00070000
/* SNMP��ַ�����б����� */
typedef struct
{
    T_IPADDR        dwIPAddr;    /* ��ַ���ƶ� */
    CHAR        strCommunity[MAX_STRING];    /* �������ַ��� */
}_PACKED_1_ T_OAM_SNMP_ACL_CONFIG_Req;

/* SNMP��ַ�����б�Ӧ�� */
typedef struct
{
    T_IPADDR        dwIPAddr;    /* ��ַ���ƶ� */
    CHAR        strCommunity[MAX_STRING];    /* �������ַ��� */
}_PACKED_1_ T_OAM_SNMP_ACL_CONFIG_Ack;

/* SNMP TRAP���������� */
#define        FUNCID_OAM_SNMP_TRAP_ENABLE_CONFIG        (WORD32)0x00070001
/* SNMP TRAP�������������� */
typedef struct
{
    WORD32        dwTrapEnable;    /* TRAP�Ƿ���� */
}_PACKED_1_ T_OAM_SNMP_TRAP_ENABLE_CONFIG_Req;

/* SNMP TRAP����������Ӧ�� */
typedef struct
{
    WORD32        dwTrapEnable;    /* TRAP�Ƿ���� */
}_PACKED_1_ T_OAM_SNMP_TRAP_ENABLE_CONFIG_Ack;

/* SNMP TRAP���崮���� */
#define        FUNCID_OAM_SNMP_TRAP_COMMUNITY_CONFIG        (WORD32)0x00070002
/* SNMP TRAP���崮�������� */
typedef struct
{
    CHAR        strTrapCommunity[MAX_STRING];    /* TRAP�������ַ��� */
}_PACKED_1_ T_OAM_SNMP_TRAP_COMMUNITY_CONFIG_Req;

/* SNMP TRAP���崮����Ӧ�� */
typedef struct
{
    CHAR        strTrapCommunity[MAX_STRING];    /* TRAP�������ַ��� */
}_PACKED_1_ T_OAM_SNMP_TRAP_COMMUNITY_CONFIG_Ack;

/* SNMP TRAP�汾���� */
#define        FUNCID_OAM_SNMP_TRAP_VERSION_CONFIG        (WORD32)0x00070003
/* SNMP TRAP�汾�������� */
typedef struct
{
    WORD32        dwTrapVersion;    /* TRAP�汾 */
}_PACKED_1_ T_OAM_SNMP_TRAP_VERSION_CONFIG_Req;

/* SNMP TRAP�汾����Ӧ�� */
typedef struct
{
    WORD32        dwTrapVersion;    /* TRAP�汾 */
}_PACKED_1_ T_OAM_SNMP_TRAP_VERSION_CONFIG_Ack;

/* SNMP TRAP��ַ1���� */
#define        FUNCID_OAM_SNMP_TRAP_ADDRESS1_CONFIG        (WORD32)0x00070004
/* SNMP TRAP��ַ1�������� */
typedef struct
{
    T_IPADDR        dwTrapAddress;    /* TRAP����Ŀ�ĵ�ַ */
    WORD32        dwTrapPort;    /* TRAP����Ŀ�ĵ�ַ�˿� */
}_PACKED_1_ T_OAM_SNMP_TRAP_ADDRESS1_CONFIG_Req;

/* SNMP TRAP��ַ1����Ӧ�� */
typedef struct
{
    T_IPADDR        dwTrapAddress;    /* TRAP����Ŀ�ĵ�ַ */
    WORD32        dwTrapPort;    /* TRAP����Ŀ�ĵ�ַ�˿� */
}_PACKED_1_ T_OAM_SNMP_TRAP_ADDRESS1_CONFIG_Ack;

/* SNMP TRAP��ַ2���� */
#define        FUNCID_OAM_SNMP_TRAP_ADDRESS2_CONFIG        (WORD32)0x00070005
/* SNMP TRAP��ַ2�������� */
typedef struct
{
    T_IPADDR        dwTrapAddress;    /* TRAP����Ŀ�ĵ�ַ */
    WORD32        dwTrapPort;    /* TRAP����Ŀ�ĵ�ַ�˿� */
}_PACKED_1_ T_OAM_SNMP_TRAP_ADDRESS2_CONFIG_Req;

/* SNMP TRAP��ַ2����Ӧ�� */
typedef struct
{
    T_IPADDR        dwTrapAddress;    /* TRAP����Ŀ�ĵ�ַ */
    WORD32        dwTrapPort;    /* TRAP����Ŀ�ĵ�ַ�˿� */
}_PACKED_1_ T_OAM_SNMP_TRAP_ADDRESS2_CONFIG_Ack;

/* SNMP TRAP��ַ3���� */
#define        FUNCID_OAM_SNMP_TRAP_ADDRESS3_CONFIG        (WORD32)0x00070006
/* SNMP TRAP��ַ3�������� */
typedef struct
{
    T_IPADDR        dwTrapAddress;    /* TRAP����Ŀ�ĵ�ַ */
    WORD32        dwTrapPort;    /* TRAP����Ŀ�ĵ�ַ�˿� */
}_PACKED_1_ T_OAM_SNMP_TRAP_ADDRESS3_CONFIG_Req;

/* SNMP TRAP��ַ3����Ӧ�� */
typedef struct
{
    T_IPADDR        dwTrapAddress;    /* TRAP����Ŀ�ĵ�ַ */
    WORD32        dwTrapPort;    /* TRAP����Ŀ�ĵ�ַ�˿� */
}_PACKED_1_ T_OAM_SNMP_TRAP_ADDRESS3_CONFIG_Ack;

/* SNMP TRAP��ַ4���� */
#define        FUNCID_OAM_SNMP_TRAP_ADDRESS4_CONFIG        (WORD32)0x00070007
/* SNMP TRAP��ַ4�������� */
typedef struct
{
    T_IPADDR        dwTrapAddress;    /* TRAP����Ŀ�ĵ�ַ */
    WORD32        dwTrapPort;    /* TRAP����Ŀ�ĵ�ַ�˿� */
}_PACKED_1_ T_OAM_SNMP_TRAP_ADDRESS4_CONFIG_Req;

/* SNMP TRAP��ַ4����Ӧ�� */
typedef struct
{
    T_IPADDR        dwTrapAddress;    /* TRAP����Ŀ�ĵ�ַ */
    WORD32        dwTrapPort;    /* TRAP����Ŀ�ĵ�ַ�˿� */
}_PACKED_1_ T_OAM_SNMP_TRAP_ADDRESS4_CONFIG_Ack;

/* SNMP TRAP��ַ5���� */
#define        FUNCID_OAM_SNMP_TRAP_ADDRESS5_CONFIG        (WORD32)0x00070008
/* SNMP TRAP��ַ5�������� */
typedef struct
{
    T_IPADDR        dwTrapAddress;    /* TRAP����Ŀ�ĵ�ַ */
    WORD32        dwTrapPort;    /* TRAP����Ŀ�ĵ�ַ�˿� */
}_PACKED_1_ T_OAM_SNMP_TRAP_ADDRESS5_CONFIG_Req;

/* SNMP TRAP��ַ5����Ӧ�� */
typedef struct
{
    T_IPADDR        dwTrapAddress;    /* TRAP����Ŀ�ĵ�ַ */
    WORD32        dwTrapPort;    /* TRAP����Ŀ�ĵ�ַ�˿� */
}_PACKED_1_ T_OAM_SNMP_TRAP_ADDRESS5_CONFIG_Ack;


#endif

