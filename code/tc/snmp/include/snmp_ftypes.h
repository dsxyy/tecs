#ifndef _SNMP_FTYPES_H_
#define _SNMP_FTYPES_H_
#include <net-snmp/net-snmp-config.h>
#include "types.h"
#include "snmpd.h"

typedef  T_TimeValue T_TIME ;

typedef struct
{
    BYTE     aucMacAddr[6]; /**<MAC��ַ 6-5-4-3-2-1  aucMacAddr[0] = 1 */ 
}_PACKED_1_ T_MACADDR;
/**
  @struct	T_LogicalAddrM
  @brief	ģ���߼���ַ
*/
typedef struct
{
    WORD16  wModule;        /**< MP��� */
    BYTE   ucPad[2];       /**< ����ֶ�  */  
}_PACKED_1_ T_LogicalAddrM;
/**
  @struct	T_LogicalAddrU
  @brief	��Ԫ�߼���ַ
*/
typedef struct
{
    WORD16  wUnit;          /**< ��Ԫ��   */
    BYTE    ucPad[2];       /**< ����ֶ� */
    BYTE    ucSubSystem;    /**< ��ϵͳ�� */
    BYTE    ucSUnit;        /**< �����ϵĴ�������ҵ����оƬ��� */
    BYTE    ucPad1[2];      /**< ����ֶ� */
}_PACKED_1_ T_LogicalAddrU;

typedef struct tagT_PhysAddress
{
    BYTE    ucRackId;       /* rack no */
    BYTE    ucShelfId;      /* shelf no */
    BYTE    ucSlotId;       /* slot no */ 
    BYTE    ucCpuId;        /* cpu no */  
}T_PhysAddress;

typedef struct  tagLogicalAddr
{
    WORD16  wSystem;
    WORD16  wModule;        /* MP��� */
    WORD16  wUnit;          /* ����� */
    WORD16  wIndex;        /* �����ţ����ӵ�Ԫ���ڲ���� */
    BYTE    ucSubSystem;    /* ��ϵͳ�� */
    BYTE    ucSUnit;        /* �����ϵĴ�������ҵ����оƬ��� */
    BYTE    ucPad[2];      /* ����ֽڣ�ƴ�ճ�DWORD*/
}T_LogicalAddr;

typedef struct tagSYS_CLOCK
{
    WORD16  wSysYear;       /* �� */
    BYTE    ucSysMon;       /* �� */
    BYTE    ucSysDay;       /* �� */
    
    BYTE    ucSysHour;      /* ʱ */
    BYTE    ucPading0;       /* �����*/
    BYTE    ucSysMin;       /* �� */
    BYTE    ucSysSec;       /* �� */
    
    WORD16  wMilliSec;      /* ���� */
    BYTE    ucSysWeek;      /* �� */
    BYTE    ucPading1;       /* �����*/
}T_SysSoftClock; /* ϵͳ��ʱ�ӽṹ����ʾ����ʱ�� */

/* ����Ĭ��ֵ*/
BOOLEAN SnmpSet_ftype_Value(BYTE ftype, T_SNMP_NodeValue *ptValue, const char *cStr);
BYTE *SnmpGet_Oid_Value(BYTE ftype, BYTE *ptBuf, oid *name, WORD32 *oidLen, WORD16 *indexLen);
BYTE *SnmpSet_Oid_Value(BYTE ftype, BYTE *ptBuf, oid *name, WORD32 *oidLen, WORD16 *indexLen, WORD32 *dwIndexDeserveLen);
/* �����������ƻ�ȡ��Ӧ������*/
BYTE SnmpGet_ftype_by_name( const char *cStr);

BYTE SnmpGet_xmlrpcftype_by_name(const char *cStr);
/* �����������ƻ�ȡ��snmp �ж�Ӧ������ */
BYTE SnmpGet_snmptype_by_ftype(BYTE ftype, WORD16 wArrNum);
/* �������ͻ�ȡ��Ӧ������ */
char *SnmpGet_name_by_ftype(BYTE ftype);
/* ��ȡ�ڴ������ݵ�ʵ�ʳ���*/
WORD16 SnmpGet_ftype_value_Len(BYTE ftype, WORD16 wArrNum);
/* ��ȡ�ڴ��е�ʵ�����ݼ������ݳ���*/
BYTE *SnmpGet_snmptype_Value(BYTE ftype, const BYTE *ptBuf, WORD16 wArrNum, size_t *var_len);
/* �����ڴ��е�ʵ������*/
BOOLEAN SnmpSet_snmptype_Value(BYTE ftype, BYTE *ptBuf, WORD16 wArrNum, const BYTE * pucVal, size_t var_val_len);
/* ���ڴ��е����ݽ���ƫ�Ʋ���*/
BYTE *SnmpBufMove(BYTE *ptBuf, BYTE ftype, WORD16 wArrNum);
/* Set ʱУ�������Ƿ���Ч */
int SnmpCheck_snmptype_value(u_char *var_val,u_char var_val_type,size_t var_val_len,T_Oid_Property *pOidProp);
BYTE *Snmp_ftype_addvariable(BYTE ftype, BYTE *pDataBuf, T_Oid_Property  *pOidProp, netsnmp_variable_list *var_list);
WORD16 SnmpGet_Index_name_len(BYTE ftype);
void SnmpFormatString(BYTE ftype, T_SNMP_NodeValue *pNodeValue, char *pszBuffer, WORD32 dwMaxCount);
#endif

