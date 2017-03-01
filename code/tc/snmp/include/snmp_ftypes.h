#ifndef _SNMP_FTYPES_H_
#define _SNMP_FTYPES_H_
#include <net-snmp/net-snmp-config.h>
#include "types.h"
#include "snmpd.h"

typedef  T_TimeValue T_TIME ;

typedef struct
{
    BYTE     aucMacAddr[6]; /**<MAC地址 6-5-4-3-2-1  aucMacAddr[0] = 1 */ 
}_PACKED_1_ T_MACADDR;
/**
  @struct	T_LogicalAddrM
  @brief	模块逻辑地址
*/
typedef struct
{
    WORD16  wModule;        /**< MP编号 */
    BYTE   ucPad[2];       /**< 填充字段  */  
}_PACKED_1_ T_LogicalAddrM;
/**
  @struct	T_LogicalAddrU
  @brief	单元逻辑地址
*/
typedef struct
{
    WORD16  wUnit;          /**< 单元号   */
    BYTE    ucPad[2];       /**< 填充字段 */
    BYTE    ucSubSystem;    /**< 子系统号 */
    BYTE    ucSUnit;        /**< 单板上的处理器和业务处理芯片编号 */
    BYTE    ucPad1[2];      /**< 填充字段 */
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
    WORD16  wModule;        /* MP编号 */
    WORD16  wUnit;          /* 单板号 */
    WORD16  wIndex;        /* 索引号，在子单元号内部编号 */
    BYTE    ucSubSystem;    /* 子系统号 */
    BYTE    ucSUnit;        /* 单板上的处理器和业务处理芯片编号 */
    BYTE    ucPad[2];      /* 填充字节，拼凑成DWORD*/
}T_LogicalAddr;

typedef struct tagSYS_CLOCK
{
    WORD16  wSysYear;       /* 年 */
    BYTE    ucSysMon;       /* 月 */
    BYTE    ucSysDay;       /* 日 */
    
    BYTE    ucSysHour;      /* 时 */
    BYTE    ucPading0;       /* 填充字*/
    BYTE    ucSysMin;       /* 分 */
    BYTE    ucSysSec;       /* 秒 */
    
    WORD16  wMilliSec;      /* 毫秒 */
    BYTE    ucSysWeek;      /* 周 */
    BYTE    ucPading1;       /* 填充字*/
}T_SysSoftClock; /* 系统软时钟结构，表示绝对时间 */

/* 设置默认值*/
BOOLEAN SnmpSet_ftype_Value(BYTE ftype, T_SNMP_NodeValue *ptValue, const char *cStr);
BYTE *SnmpGet_Oid_Value(BYTE ftype, BYTE *ptBuf, oid *name, WORD32 *oidLen, WORD16 *indexLen);
BYTE *SnmpSet_Oid_Value(BYTE ftype, BYTE *ptBuf, oid *name, WORD32 *oidLen, WORD16 *indexLen, WORD32 *dwIndexDeserveLen);
/* 给定类型名称获取对应的类型*/
BYTE SnmpGet_ftype_by_name( const char *cStr);

BYTE SnmpGet_xmlrpcftype_by_name(const char *cStr);
/* 根据类型名称获取在snmp 中对应的类型 */
BYTE SnmpGet_snmptype_by_ftype(BYTE ftype, WORD16 wArrNum);
/* 给定类型获取对应的名称 */
char *SnmpGet_name_by_ftype(BYTE ftype);
/* 获取内存中数据的实际长度*/
WORD16 SnmpGet_ftype_value_Len(BYTE ftype, WORD16 wArrNum);
/* 获取内存中的实际数据及该数据长度*/
BYTE *SnmpGet_snmptype_Value(BYTE ftype, const BYTE *ptBuf, WORD16 wArrNum, size_t *var_len);
/* 设置内存中的实际数据*/
BOOLEAN SnmpSet_snmptype_Value(BYTE ftype, BYTE *ptBuf, WORD16 wArrNum, const BYTE * pucVal, size_t var_val_len);
/* 对内存中的数据进行偏移操作*/
BYTE *SnmpBufMove(BYTE *ptBuf, BYTE ftype, WORD16 wArrNum);
/* Set 时校验数据是否有效 */
int SnmpCheck_snmptype_value(u_char *var_val,u_char var_val_type,size_t var_val_len,T_Oid_Property *pOidProp);
BYTE *Snmp_ftype_addvariable(BYTE ftype, BYTE *pDataBuf, T_Oid_Property  *pOidProp, netsnmp_variable_list *var_list);
WORD16 SnmpGet_Index_name_len(BYTE ftype);
void SnmpFormatString(BYTE ftype, T_SNMP_NodeValue *pNodeValue, char *pszBuffer, WORD32 dwMaxCount);
#endif

