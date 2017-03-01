/*********************************************************************
* 版权所有 (C)2010, 深圳市中兴通讯股份有限公司。
*
* 文件名称： snmp_ftypes.c
* 文件标识：
* 内容摘要： snmp 支持的各种数据类型对应的处理函数
* 其它说明： 
* 当前版本： V1.0
* 作    者： 刘华振——3G平台
* 完成日期：
*
* 修改记录1：
*    修改日期：20010年xx月xx日
*    版 本 号：V1.0
*    修 改 人：刘华振
*    修改内容：创建
typedef union 
{
   long           *integer;
   u_char       *string;
   oid             *objid;
   u_char       *bitstring;
   struct counter64 *counter64;
} netsnmp_vardata;

int snmp_set_var_value(netsnmp_variable_list * vars, const u_char * value, size_t len);
header_generic();
**********************************************************************/

#include "snmp_ftypes.h"
#if HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#include "oam_pub_type.h"
#include "log_agent.h"

using namespace zte_tecs;
#define GET_OID_NAME_FROM_BUF(T)\
    name[*oidLen] = *((T *)ptBuf);\
    ptBuf += sizeof(T);\
    if ((*indexLen) >= sizeof(T))\
    {\
        (*indexLen) -= sizeof(T);\
    }\
    (*oidLen)++

#define SET_OID_NAME_TO_BUF(T)\
    tmp = name[*oidLen];\
    memcpy(ptBuf, &tmp, sizeof(T));\
    ptBuf += sizeof(T);\
    if ((*indexLen) >= sizeof(T))\
    {\
        (*indexLen) -= sizeof(T);\
    }\
    (*oidLen)++
        

#define SNMP_GET_FTYPE_OID_T_VALUE(T)\
    OamFormatLog((char *)"ptBuf is NULL!\n");\
    OAM_RETURN_VAL_IF_FAIL(ptBuf != NULL, ptBuf);\
    OamFormatLog((char *)"name is NULL!\n");\
    OAM_RETURN_VAL_IF_FAIL(name != NULL, ptBuf);\
    OamFormatLog((char *)"oidLen is NULL!\n");\
    OAM_RETURN_VAL_IF_FAIL(oidLen != NULL, ptBuf);\
    OamFormatLog((char *)"indexLen is NULL!\n");\
    OAM_RETURN_VAL_IF_FAIL(indexLen != NULL, ptBuf);\
    \
        GET_OID_NAME_FROM_BUF(T);\
        return ptBuf;

#define SNMP_SET_FTYPE_OID_T_VALUE(T)\
    T tmp = 0;\
    \
    OamFormatLog((char *)"ptBuf is NULL!\n");\
    OAM_RETURN_VAL_IF_FAIL(ptBuf != NULL, ptBuf);\
    OamFormatLog((char *)"name is NULL!\n");\
    OAM_RETURN_VAL_IF_FAIL(name != NULL, ptBuf);\
    OamFormatLog((char *)"oidLen is NULL!\n");\
    OAM_RETURN_VAL_IF_FAIL(oidLen != NULL, ptBuf);\
    OamFormatLog((char *)"indexLen is NULL!\n");\
    OAM_RETURN_VAL_IF_FAIL(indexLen != NULL, ptBuf);\
    \
    SET_OID_NAME_TO_BUF(T);\
    return ptBuf;

#define SNMP_GET_SNMPFTYPE_T_VALUE(T)    \
    static long long_ret = 0;\
\
    OamFormatLog((char *)"ptBuf is NULL!\n");\
    OAM_RETURN_VAL_IF_FAIL(ptBuf != NULL, NULL);\
    OamFormatLog((char *)"var_len is NULL!\n");\
    OAM_RETURN_VAL_IF_FAIL(var_len != NULL, NULL);\
    OamFormatLog((char *)"wArrNum = %u!\n", wArrNum);\
    OAM_NASSERT_RETURN_VAL_IF_FAIL(wArrNum > 0, NULL);\
    \
    if (wArrNum == 1)\
    {\
        *var_len = sizeof(long);\
        long_ret = *((T*)ptBuf); /* 主要考虑大小尾传进来的ptBuf 是long 型转换成BYTE的指针*/\
        return (unsigned char *) &long_ret;\
    }\
    else\
    {\
        /* 数组作为字符串处理 */\
        *var_len = wArrNum * ftypesize;\
        return (unsigned char *)ptBuf;\
    }

#define SNMP_SET_SNMPTYPE_T_VALUE(T)\
    WORD16 wMinLen = 0;\
    T tmp = 0;\
\
    OamFormatLog((char *)"ptBuf is NULL!\n");\
    OAM_RETURN_IF_FAIL(ptBuf != NULL);\
    OamFormatLog((char *)"pucVal is NULL!\n");\
    OAM_RETURN_IF_FAIL(pucVal != NULL);\
    OamFormatLog((char *)"wArrNum = %u!\n", wArrNum);\
    OAM_NASSERT_RETURN_IF_FAIL(wArrNum > 0);\
    \
    if (wArrNum == 1)\
    {\
        tmp = *((long *)pucVal);/* 主要考虑大小尾传进来的ptBuf 是long 型转换成BYTE的指针*/\
        memcpy(ptBuf,&tmp, ftypesize);\
    }\
    else\
    {\
        wMinLen = MIN(wArrNum * ftypesize, var_val_len);\
        memcpy(ptBuf, pucVal, wMinLen);\
    }

#define SNMP_VALUE_CHECK(MEMBER, VALUE_TYPE)\
    WORD16 wIndex = 0;\
    VALUE_TYPE tmpData = 0;\
    OamFormatLog((char *)"var_val is NULL!\n");\
    OAM_RETURN_VAL_IF_FAIL(var_val != NULL, SNMP_ERR_GENERR);\
    OamFormatLog((char *)"pOidProp is NULL!\n");\
    OAM_RETURN_VAL_IF_FAIL(pOidProp != NULL, SNMP_ERR_GENERR);\
    if (pOidProp->wArrayLen == 1)\
    {\
        OAM_NASSERT_RETURN_VAL_IF_FAIL(var_val_type == snmptype, SNMP_ERR_WRONGTYPE);\
        tmpData = *((long *)var_val);\
        if(pOidProp->pMaxValue != NULL)\
        {\
            OAM_NASSERT_RETURN_VAL_IF_FAIL(*((long *)var_val) <= (long)pOidProp->pMaxValue->MEMBER, SNMP_ERR_WRONGVALUE);\
        }\
        if(pOidProp->pMinValue != NULL)\
        {\
            OAM_NASSERT_RETURN_VAL_IF_FAIL(*((long *)var_val) >= (long)pOidProp->pMinValue->MEMBER, SNMP_ERR_WRONGVALUE);\
        }\
    }\
    else\
    {\
        OAM_NASSERT_RETURN_VAL_IF_FAIL(var_val_type == ASN_OCTET_STR, SNMP_ERR_WRONGTYPE);\
        OAM_NASSERT_RETURN_VAL_IF_FAIL(var_val_len <= pOidProp->wParaLen, SNMP_ERR_WRONGLENGTH);\
        for (wIndex = 0; wIndex < pOidProp->wArrayLen; wIndex++)\
        {\
            tmpData = *var_val;\
            if(pOidProp->pMaxValue != NULL)\
            {\
                OAM_NASSERT_RETURN_VAL_IF_FAIL(tmpData <= pOidProp->pMaxValue->MEMBER, SNMP_ERR_WRONGVALUE);\
            }\
            if(pOidProp->pMinValue)\
            {\
                OAM_NASSERT_RETURN_VAL_IF_FAIL(tmpData >= pOidProp->pMinValue->MEMBER, SNMP_ERR_WRONGVALUE);\
            }\
            var_val += sizeof(VALUE_TYPE);\
        }\
    }


#define SNMP_OID_INDEX_LEN_NONE (BYTE)0
#define SNMP_OID_INDEX_LEN_COMMON (BYTE)1
#define SNMP_OID_INDEX_LEN_IPADDR  (BYTE)4
#define SNMP_OID_INDEX_LEN_PHYADDR  (BYTE)4
/* #define SNMP_OID_INDEX_LEN_CHAR      (BYTE)255 */

static int SnmpConvert_atoi(char * pString, WORD32 * pDword);

static void SnmpSet_ftype_CHAR_Value(T_SNMP_NodeValue *ptValue, const char *cStr);
static void SnmpSet_ftype_BYTE_Value(T_SNMP_NodeValue *ptValue, const char *cStr);
static void SnmpSet_ftype_WORD16_Value(T_SNMP_NodeValue *ptValue, const char *cStr);
static void SnmpSet_ftype_SWORD16_Value(T_SNMP_NodeValue *ptValue, const char *cStr);
static void SnmpSet_ftype_WORD32_Value(T_SNMP_NodeValue *ptValue, const char *cStr);
static void SnmpSet_ftype_UINT32_Value(T_SNMP_NodeValue *ptValue, const char *cStr);
static void SnmpSet_ftype_Comm_Value(T_SNMP_NodeValue *ptValue, const char *cStr);


static BYTE *SnmpGet_ftype_OID_CHAR_Value(BYTE *ptBuf, oid *name, WORD32 *oidLen, WORD16 *indexLen);
static BYTE *SnmpGet_ftype_OID_BYTE_Value(BYTE *ptBuf, oid *name, WORD32 *oidLen, WORD16 *indexLen);
static BYTE *SnmpGet_ftype_OID_WORD16_Value(BYTE *ptBuf, oid *name, WORD32 *oidLen, WORD16 *indexLen);
static BYTE *SnmpGet_ftype_OID_WORD32_Value(BYTE *ptBuf, oid *name, WORD32 *oidLen, WORD16 *indexLen);
static BYTE *SnmpGet_ftype_OID_UINT32_Value(BYTE *ptBuf, oid *name, WORD32 *oidLen, WORD16 *indexLen);
static BYTE *SnmpGet_ftype_OID_IPADDR_Value(BYTE *ptBuf, oid *name, WORD32 *oidLen, WORD16 *indexLen);
static BYTE *SnmpGet_ftype_OID_PHYADDR_Value(BYTE *ptBuf, oid *name, WORD32 *oidLen, WORD16 *indexLen);

#if 0
/* 对于普通的，不能把字符串类型作为索引，否则在创建
    表时没办法知道字符串索引OID 的真实长度 */
static BYTE *SnmpSet_ftype_OID_CHAR_Value(BYTE *ptBuf, const oid *name, WORD32 *oidLen, WORD16 *indexLen);
#endif
static BYTE *SnmpSet_ftype_OID_BYTE_Value(BYTE *ptBuf, const oid *name, WORD32 *oidLen, WORD16 *indexLen, WORD32 *dwIndexDeserveLen);
static BYTE *SnmpSet_ftype_OID_WORD16_Value(BYTE *ptBuf, const oid *name, WORD32 *oidLen, WORD16 *indexLen, WORD32 *dwIndexDeserveLen);
static BYTE *SnmpSet_ftype_OID_WORD32_Value(BYTE *ptBuf, const oid *name, WORD32 *oidLen, WORD16 *indexLen, WORD32 *dwIndexDeserveLen);
static BYTE *SnmpSet_ftype_OID_UINT32_Value(BYTE *ptBuf, const oid *name, WORD32 *oidLen, WORD16 *indexLen, WORD32 *dwIndexDeserveLen);
static BYTE *SnmpSet_ftype_OID_IPADDR_Value(BYTE *ptBuf, const oid *name, WORD32 *oidLen, WORD16 *indexLen, WORD32 *dwIndexDeserveLen);
static BYTE *SnmpSet_ftype_OID_PHYADDR_Value(BYTE *ptBuf, const oid *name, WORD32 *oidLen, WORD16 *indexLen, WORD32 *dwIndexDeserveLen);


static BYTE *SnmpGet_snmptype_BYTE_Value(const BYTE *ptBuf, WORD32 ftypesize, WORD16 wArrNum, size_t *var_len);
static BYTE *SnmpGet_snmptype_CHAR_Value(const BYTE *ptBuf, WORD32 ftypesize, WORD16 wArrNum, size_t *var_len);
static BYTE *SnmpGet_snmptype_WORD16_Value(const BYTE *ptBuf, WORD32 ftypesize, WORD16 wArrNum, size_t *var_len);
static BYTE *SnmpGet_snmptype_SWORD16_Value(const BYTE *ptBuf, WORD32 ftypesize, WORD16 wArrNum, size_t *var_len);

static BYTE *SnmpGet_snmptype_WORD32_Value(const BYTE *ptBuf, WORD32 ftypesize, WORD16 wArrNum, size_t *var_len);
static BYTE *SnmpGet_snmptype_UINT32_Value(const BYTE *ptBuf, WORD32 ftypesize, WORD16 wArrNum, size_t *var_len);
static BYTE *SnmpGet_snmptype_IPADDR_Value(const BYTE *ptBuf, WORD32 ftypesize, WORD16 wArrNum, size_t *var_len);
static BYTE *SnmpGet_snmptype_Comm_Value(const BYTE *ptBuf, WORD32 ftypesize, WORD16 wArrNum, size_t *var_len);

static void SnmpSet_snmptype_BYTE_Value(BYTE *ptBuf, WORD32 ftypesize, WORD16 wArrNum, const BYTE * pucVal, size_t var_val_len);
static void SnmpSet_snmptype_CHAR_Value(BYTE *ptBuf, WORD32 ftypesize, WORD16 wArrNum, const BYTE * pucVal, size_t var_val_len);
static void SnmpSet_snmptype_WORD16_Value(BYTE *ptBuf, WORD32 ftypesize, WORD16 wArrNum, const BYTE * pucVal, size_t var_val_len);
static void SnmpSet_snmptype_SWORD16_Value(BYTE *ptBuf, WORD32 ftypesize, WORD16 wArrNum, const BYTE * pucVal, size_t var_val_len);

static void SnmpSet_snmptype_WORD32_Value(BYTE *ptBuf, WORD32 ftypesize, WORD16 wArrNum, const BYTE * pucVal, size_t var_val_len);
static void SnmpSet_snmptype_UINT32_Value(BYTE *ptBuf, WORD32 ftypesize, WORD16 wArrNum, const BYTE * pucVal, size_t var_val_len);
static void SnmpSet_snmptype_IPADDR_Value(BYTE *ptBuf, WORD32 ftypesize, WORD16 wArrNum, const BYTE * pucVal, size_t var_val_len);
static void SnmpSet_snmptype_Comm_Value(BYTE *ptBuf, WORD32 ftypesize, WORD16 wArrNum, const BYTE * pucVal, size_t var_val_len);

static int SnmpCheck_snmptype_CHAR_Value(u_char *var_val,u_char var_val_type,size_t var_val_len, BYTE snmptype, T_Oid_Property *pOidProp);
static int SnmpCheck_snmptype_BYTE_Value(u_char *var_val,u_char var_val_type,size_t var_val_len, BYTE snmptype, T_Oid_Property *pOidProp);
static int SnmpCheck_snmptype_WORD16_Value(u_char *var_val,u_char var_val_type,size_t var_val_len, BYTE snmptype, T_Oid_Property *pOidProp);
static int SnmpCheck_snmptype_SWORD16_Value(u_char *var_val,u_char var_val_type,size_t var_val_len, BYTE snmptype, T_Oid_Property *pOidProp);

static int SnmpCheck_snmptype_WORD32_Value(u_char *var_val,u_char var_val_type,size_t var_val_len, BYTE snmptype, T_Oid_Property *pOidProp);
static int SnmpCheck_snmptype_Common_Value(u_char *var_val,u_char var_val_type,size_t var_val_len, BYTE snmptype, T_Oid_Property *pOidProp);

static void Snmp_snprintf_CHAR(T_SNMP_NodeValue *pNodeValue, char *pszBuffer, WORD32 dwMaxCount);
static void Snmp_snprintf_BYTE(T_SNMP_NodeValue *pNodeValue, char *pszBuffer, WORD32 dwMaxCount);
static void Snmp_snprintf_WORD16(T_SNMP_NodeValue *pNodeValue, char *pszBuffer, WORD32 dwMaxCount);
static void Snmp_snprintf_SWORD16(T_SNMP_NodeValue *pNodeValue, char *pszBuffer, WORD32 dwMaxCount);
static void Snmp_snprintf_WORD32(T_SNMP_NodeValue *pNodeValue, char *pszBuffer, WORD32 dwMaxCount);
static void Snmp_snprintf_WORD64(T_SNMP_NodeValue *pNodeValue, char *pszBuffer, WORD32 dwMaxCount);
static void Snmp_snprintf_IPADDR(T_SNMP_NodeValue *pNodeValue, char *pszBuffer, WORD32 dwMaxCount);


typedef void (*Set_ftype_value_Func)(T_SNMP_NodeValue *ptValue, const char *cStr);
typedef BYTE* (*Get_snmptype_value_Func)(const BYTE *ptBuf, WORD32 ftypesize, WORD16 wArrNum, size_t *var_len);
typedef void (*Set_snmptype_value_Func)(BYTE *ptBuf, WORD32 ftypesize, WORD16 wArrNum, const BYTE * pucVal, size_t var_val_len);
typedef int (*Check_snmpftype_value_Func)(u_char *var_val,u_char var_val_type,size_t var_val_len, BYTE snmptype, T_Oid_Property *pOidProp);
typedef BYTE* (*Set_ftype_oid_value_Func)(BYTE *ptBuf, const oid *name, WORD32 *oidLen, WORD16 *indexLen, WORD32 *dwIndexDeserveLen);
typedef BYTE* (*Get_ftype_oid_value_Func)(BYTE *ptBuf, oid *name, WORD32 *oidLen, WORD16 *indexLen);
typedef void (*Format_String_Func)(T_SNMP_NodeValue *pNodeValue, char *pszBuffer, WORD32 dwMaxCount);

typedef struct _SNMP_ftype_t 
{
       BYTE        ftype; /* 自定义类型，如BYTE, WORD, WORD32, T_LogicalAddr 等 */
       BYTE        xml_rpc_ftype; /* 自定义类型，如BYTE, WORD, WORD32, T_LogicalAddr 等 */
       BYTE        ftypesize; /* 单个自定义类型所占内存大小，如sizeof(BYTE), sizeof(T_LogicalAddr) */
       BYTE        snmptype; /* 自定义类型对应的在snmp协议中的类型 */
       BYTE        index_oidname_len;
	char		       *ftypename; /* 自定义类型名称 */
       Get_ftype_oid_value_Func get_ftype_oid_value; /* 按自定义类型取值 */
       Set_ftype_oid_value_Func set_ftype_oid_value;
       Set_ftype_value_Func set_ftype_value; /* 按自定义类型赋值 */
       Get_snmptype_value_Func       get_snmptype_value;	/* 按SNMP支持的类型从内存中取值，如果不是数组，最少占sizeof(long)个字节*/
       Set_snmptype_value_Func   set_snmptype_value; /* 按SNMP 支持的类型对内存赋值，如果不是数组，最少占sizeof(long)个字节*/
       Check_snmpftype_value_Func check_snmptype_value; /* 第一个参数的值是按snmp支持的类型存储的 */
       Format_String_Func format_string;
}_PACKED_1_ SNMP_FTYPE;

/* ftype 为离散的值 */

SNMP_FTYPE g_tSnmpNSerialFtype[] =
{
/* 100 */{DATA_TYPE_PAD, XMLRPC_TYPE_STRING, sizeof(BYTE), ASN_UNSIGNED, SNMP_OID_INDEX_LEN_NONE, (char *)"T_PAD", 
                                NULL, NULL,
                                SnmpSet_ftype_BYTE_Value, 
                                SnmpGet_snmptype_BYTE_Value, SnmpSet_snmptype_BYTE_Value, 
                                NULL, Snmp_snprintf_BYTE},
/* 101 {DATA_TYPE_JID, sizeof(JID), ASN_OCTET_STR, SNMP_OID_INDEX_LEN_NONE, "JID", 
                                NULL, NULL, 
                                NULL,
                                SnmpGet_snmptype_Comm_Value, SnmpSet_snmptype_Comm_Value, 
                                SnmpCheck_snmptype_Common_Value, NULL},  */
/* 102 */{DATA_TYPE_ITEM_LIST , XMLRPC_TYPE_STRING, 0, ASN_OCTET_STR, SNMP_OID_INDEX_LEN_NONE,  
                                (char *)"ITEM_LIST", NULL, NULL, NULL, NULL, NULL, NULL, NULL},
/* 103 */{DATA_TYPE_ITEM_COMMAND , XMLRPC_TYPE_STRING, 0, ASN_OCTET_STR, SNMP_OID_INDEX_LEN_NONE, 
                                (char *)"ITEM_COMMAND", NULL, NULL, NULL, NULL, NULL, NULL, NULL},
/* 254 */{DATA_TYPE_PARA, XMLRPC_TYPE_STRING, 0, ASN_OCTET_STR, SNMP_OID_INDEX_LEN_NONE, 
                                (char *)"PARA", NULL, NULL, NULL, NULL, NULL, NULL, NULL},
/* 255 */{DATA_TYPE_UNDEFINED, XMLRPC_TYPE_STRING, 0, ASN_OCTET_STR, SNMP_OID_INDEX_LEN_NONE, 
                                NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}
};

#define DATA_TYPE_BEGIN 0

/* ftype 为连续的值，即为在数组中的索引，填加新的类型时要注意，
    不能在中间插入，如果是连续的，追加在最后，如果是离散的，
    加入g_tSnmpNSerialFtype中，这样做是为了加快存取速度，直接把ftype
    作为数组下标存取 */
SNMP_FTYPE g_tSnmpSerialFtype[] =
{
    /* 类型              长度          对应的snmp类型, 名称 设置取值           获取取值       */
/* 00 */{DATA_TYPE_BEGIN, XMLRPC_TYPE_STRING, 0, 0, SNMP_OID_INDEX_LEN_NONE, (char *)"", NULL, NULL, NULL, NULL, NULL, NULL, NULL},
/* 01 */{DATA_TYPE_BYTE, XMLRPC_TYPE_INT, sizeof(BYTE), ASN_UNSIGNED, SNMP_OID_INDEX_LEN_COMMON, (char *)"BYTE", 
                                  SnmpGet_ftype_OID_BYTE_Value, SnmpSet_ftype_OID_BYTE_Value, 
                                  SnmpSet_ftype_BYTE_Value,  
                                  SnmpGet_snmptype_BYTE_Value, SnmpSet_snmptype_BYTE_Value, 
                                  SnmpCheck_snmptype_BYTE_Value, Snmp_snprintf_BYTE},
/* 02 */{DATA_TYPE_WORD, XMLRPC_TYPE_INT, sizeof(WORD16), ASN_UNSIGNED, SNMP_OID_INDEX_LEN_COMMON, (char *)"WORD16", 
                                  SnmpGet_ftype_OID_WORD16_Value, SnmpSet_ftype_OID_WORD16_Value,
                                  SnmpSet_ftype_WORD16_Value, 
                                  SnmpGet_snmptype_WORD16_Value, SnmpSet_snmptype_WORD16_Value, 
                                  SnmpCheck_snmptype_WORD16_Value, Snmp_snprintf_WORD16},
/* 03 */{DATA_TYPE_DWORD, XMLRPC_TYPE_INT, sizeof(WORD32), ASN_UNSIGNED, SNMP_OID_INDEX_LEN_COMMON, (char *)"WORD32", 
                                  SnmpGet_ftype_OID_WORD32_Value, SnmpSet_ftype_OID_WORD32_Value,
                                  SnmpSet_ftype_WORD32_Value, 
                                  SnmpGet_snmptype_WORD32_Value, SnmpSet_snmptype_WORD32_Value, 
                                  SnmpCheck_snmptype_WORD32_Value, Snmp_snprintf_WORD32},
/* 04 */{DATA_TYPE_INT, XMLRPC_TYPE_INT, sizeof(int), ASN_INTEGER, SNMP_OID_INDEX_LEN_COMMON, (char *)"int", 
                                  SnmpGet_ftype_OID_WORD32_Value, SnmpSet_ftype_OID_WORD32_Value, 
                                  SnmpSet_ftype_WORD32_Value, 
                                  SnmpGet_snmptype_Comm_Value, SnmpSet_snmptype_Comm_Value, 
                                  SnmpCheck_snmptype_WORD32_Value, Snmp_snprintf_WORD32}, 
/* 05 */{DATA_TYPE_CHAR, XMLRPC_TYPE_STRING, sizeof(char), ASN_OCTET_STR, SNMP_OID_INDEX_LEN_NONE, (char *)"char", 
                                  SnmpGet_ftype_OID_CHAR_Value, NULL, 
                                  SnmpSet_ftype_CHAR_Value, 
                                  SnmpGet_snmptype_CHAR_Value, SnmpSet_snmptype_CHAR_Value, 
                                  SnmpCheck_snmptype_CHAR_Value, Snmp_snprintf_CHAR},
/* 06 */{DATA_TYPE_STRING, XMLRPC_TYPE_STRING, 0, ASN_OCTET_STR, SNMP_OID_INDEX_LEN_NONE,  
                                  (char *)"STRING", NULL, NULL, NULL, NULL, NULL, NULL, NULL},
/* 07 */{DATA_TYPE_TEXT, XMLRPC_TYPE_STRING, 0, ASN_OCTET_STR, SNMP_OID_INDEX_LEN_NONE, 
                                  (char *)"TEXT", NULL, NULL, NULL, NULL, NULL, NULL, NULL},
/* 08 */{DATA_TYPE_TABLE, XMLRPC_TYPE_STRING, 0, ASN_OCTET_STR, SNMP_OID_INDEX_LEN_NONE, 
                                  (char *)"TABLE", NULL, NULL, NULL, NULL, NULL, NULL, NULL},
/* 09 */{DATA_TYPE_IPADDR, XMLRPC_TYPE_IPV4, sizeof(uint32), ASN_IPADDRESS, SNMP_OID_INDEX_LEN_IPADDR, (char *)"T_IPADDR", 
                                  SnmpGet_ftype_OID_IPADDR_Value, SnmpSet_ftype_OID_IPADDR_Value,
                                  SnmpSet_ftype_UINT32_Value,
                                  SnmpGet_snmptype_IPADDR_Value, SnmpSet_snmptype_IPADDR_Value, 
                                  SnmpCheck_snmptype_WORD32_Value, Snmp_snprintf_IPADDR},
/* 10 */{DATA_TYPE_DATE, XMLRPC_TYPE_STRING, 0, ASN_OCTET_STR, SNMP_OID_INDEX_LEN_NONE,  
                                  (char *)"DATE",  NULL, NULL, NULL, NULL, NULL, NULL, NULL},
/* 11 */{DATA_TYPE_TIME, XMLRPC_TYPE_STRING, sizeof(WORD32), ASN_OCTET_STR, SNMP_OID_INDEX_LEN_NONE, (char *)"Time", 
                                   NULL, NULL,
                                  SnmpSet_ftype_WORD32_Value, 
                                  SnmpGet_snmptype_WORD32_Value, SnmpSet_snmptype_WORD32_Value, 
                                  SnmpCheck_snmptype_WORD32_Value, Snmp_snprintf_WORD32},
/* 12 */{DATA_TYPE_MAP, XMLRPC_TYPE_INT, sizeof(WORD32), ASN_UNSIGNED, SNMP_OID_INDEX_LEN_COMMON, (char *)"MAP", 
                                  SnmpGet_ftype_OID_WORD32_Value, SnmpSet_ftype_OID_WORD32_Value, 
                                  SnmpSet_ftype_WORD32_Value, 
                                  SnmpGet_snmptype_WORD32_Value, SnmpSet_snmptype_WORD32_Value, 
                                  SnmpCheck_snmptype_WORD32_Value, Snmp_snprintf_WORD32},
/* 13 */{DATA_TYPE_LIST, XMLRPC_TYPE_STRING, 0, ASN_OCTET_STR, SNMP_OID_INDEX_LEN_NONE, 
                                  (char *)"LIST", NULL, NULL, NULL, NULL, NULL, NULL, NULL},
/* 14 */{DATA_TYPE_CONST, XMLRPC_TYPE_STRING, 0, ASN_OCTET_STR, SNMP_OID_INDEX_LEN_NONE, 
                                  (char *)"CONST", NULL, NULL, NULL, NULL, NULL, NULL, NULL},
/* 15 */{DATA_TYPE_IFPORT, XMLRPC_TYPE_STRING, 0, ASN_OCTET_STR, SNMP_OID_INDEX_LEN_NONE, 
                                  (char *)"IFPORT", NULL, NULL, NULL, NULL, NULL, NULL, NULL},
/* 16 */{DATA_TYPE_HEX, XMLRPC_TYPE_STRING, 0, ASN_OCTET_STR, SNMP_OID_INDEX_LEN_NONE, 
                                  (char *)"HEX", NULL, NULL, NULL, NULL, NULL, NULL, NULL},
/* 17 */{DATA_TYPE_MASK, XMLRPC_TYPE_STRING, 0, ASN_OCTET_STR, SNMP_OID_INDEX_LEN_NONE, 
                                  (char *)"MASK", NULL, NULL, NULL, NULL, NULL, NULL, NULL},
/* 18 */{DATA_TYPE_IMASK, XMLRPC_TYPE_STRING, 0, ASN_OCTET_STR, SNMP_OID_INDEX_LEN_NONE, 
                                  (char *)"IMASK", NULL, NULL, NULL, NULL, NULL, NULL, NULL},
/* 19 */{DATA_TYPE_MACADDR, XMLRPC_TYPE_STRING, sizeof(T_MACADDR), ASN_OCTET_STR, SNMP_OID_INDEX_LEN_NONE, (char *)"T_MACADDR", 
                                  NULL, NULL, 
                                  NULL,
                                  SnmpGet_snmptype_Comm_Value, SnmpSet_snmptype_Comm_Value, 
                                  NULL, NULL},
/* 20 */{DATA_TYPE_FLOAT, XMLRPC_TYPE_STRING, 0,  ASN_OCTET_STR, SNMP_OID_INDEX_LEN_NONE, 
                                  (char *)"FLOAT", NULL, NULL, NULL, NULL, NULL, NULL, NULL},
/* 21 */{DATA_TYPE_BOARDNAME, XMLRPC_TYPE_STRING, 0, ASN_OCTET_STR, SNMP_OID_INDEX_LEN_NONE, 
                                  (char *)"BOARDNAME", NULL, NULL, NULL, NULL, NULL, NULL, NULL},
/* 22 */{DATA_TYPE_IPV6PREFIX, XMLRPC_TYPE_STRING, 0, ASN_OCTET_STR, SNMP_OID_INDEX_LEN_NONE, 
                                  (char *)"IPV6PREFIX", NULL, NULL, NULL, NULL, NULL, NULL, NULL},
/* 23 */{DATA_TYPE_IPV6ADDR, XMLRPC_TYPE_STRING, 0, ASN_OCTET_STR, SNMP_OID_INDEX_LEN_NONE, 
                                  (char *)"IPV6ADDR", NULL, NULL, NULL, NULL, NULL, NULL, NULL},
/* 24 */{DATA_TYPE_VPN_ASN, XMLRPC_TYPE_STRING, 0, ASN_OCTET_STR, SNMP_OID_INDEX_LEN_NONE, 
                                  (char *)"VPN_ASN", NULL, NULL, NULL, NULL, NULL, NULL, NULL}, 
/* 25 */{DATA_TYPE_VPN_IP, XMLRPC_TYPE_STRING, 0, ASN_OCTET_STR, SNMP_OID_INDEX_LEN_NONE, 
                                  (char *)"VPN_IP", NULL, NULL, NULL, NULL, NULL, NULL, NULL},                                   
/* 26 */{DATA_TYPE_PHYADDRESS, XMLRPC_TYPE_STRING, sizeof(T_PhysAddress), ASN_OCTET_STR, SNMP_OID_INDEX_LEN_PHYADDR, (char *)"T_PhysAddress", 
                                  SnmpGet_ftype_OID_PHYADDR_Value, SnmpSet_ftype_OID_PHYADDR_Value, 
                                  SnmpSet_ftype_Comm_Value,
                                  SnmpGet_snmptype_Comm_Value, SnmpSet_snmptype_Comm_Value, 
                                  SnmpCheck_snmptype_Common_Value, NULL},
/* 27 */{DATA_TYPE_LOGICADDRESS, XMLRPC_TYPE_STRING, sizeof(T_LogicalAddr), ASN_OCTET_STR, SNMP_OID_INDEX_LEN_NONE, (char *)"T_LogicalAddr", 
                                  NULL, NULL, 
                                  NULL,
                                  SnmpGet_snmptype_Comm_Value, SnmpSet_snmptype_Comm_Value, 
                                  SnmpCheck_snmptype_Common_Value, NULL}, 
/* 28 */{DATA_TYPE_WORD64, XMLRPC_TYPE_INT, sizeof(WORD64), ASN_OCTET_STR, SNMP_OID_INDEX_LEN_NONE, (char *)"WORD64", 
                                  NULL, NULL, 
                                  NULL,
                                  SnmpGet_snmptype_Comm_Value, SnmpSet_snmptype_Comm_Value, 
                                  SnmpCheck_snmptype_Common_Value, Snmp_snprintf_WORD64},
/* 29 */{DATA_TYPE_SWORD, XMLRPC_TYPE_INT, sizeof(SWORD16), ASN_INTEGER, SNMP_OID_INDEX_LEN_NONE, (char *)"SWORD", 
                                  NULL, NULL,
                                  SnmpSet_ftype_SWORD16_Value,
                                  SnmpGet_snmptype_SWORD16_Value, SnmpSet_snmptype_SWORD16_Value, 
                                  SnmpCheck_snmptype_SWORD16_Value, Snmp_snprintf_SWORD16},
/* 30 */{DATA_TYPE_LOGIC_ADDR_M, XMLRPC_TYPE_STRING, sizeof(T_LogicalAddrM), ASN_OCTET_STR, SNMP_OID_INDEX_LEN_NONE, (char *)"T_LogicalAddrM", 
                                  NULL, NULL, 
                                  NULL,
                                  SnmpGet_snmptype_Comm_Value, SnmpSet_snmptype_Comm_Value, 
                                  NULL, NULL},
/* 31 */{DATA_TYPE_LOGIC_ADDR_U, XMLRPC_TYPE_STRING, sizeof(T_LogicalAddrU), ASN_OCTET_STR, SNMP_OID_INDEX_LEN_NONE, (char *)"T_LogicalAddrU", 
                                  NULL, NULL, 
                                  NULL,
                                  SnmpGet_snmptype_Comm_Value, SnmpSet_snmptype_Comm_Value, 
                                  NULL, NULL},
/* 32 */{DATA_TYPE_IPVXADDR , XMLRPC_TYPE_STRING, 0, ASN_OCTET_STR, SNMP_OID_INDEX_LEN_NONE, (char *)"T_IPVXADDR", 
                                  NULL, NULL, NULL, NULL, NULL, NULL, NULL},
/* 33 */{DATA_TYPE_T_TIME, XMLRPC_TYPE_STRING, sizeof(T_TIME), ASN_OCTET_STR, SNMP_OID_INDEX_LEN_NONE, (char *)"T_Time", 
                                  NULL, NULL, 
                                  NULL,
                                  SnmpGet_snmptype_Comm_Value, SnmpSet_snmptype_Comm_Value, 
                                  NULL, NULL},
/* 34 */{DATA_TYPE_SYSCLOCK , XMLRPC_TYPE_STRING, sizeof(T_SysSoftClock), ASN_OCTET_STR, SNMP_OID_INDEX_LEN_NONE, (char *)"T_SysSoftClock", 
                                  NULL, NULL, 
                                  NULL,
                                  SnmpGet_snmptype_Comm_Value, SnmpSet_snmptype_Comm_Value, 
                                  SnmpCheck_snmptype_Common_Value, NULL},
/* 35 */{DATA_TYPE_VMMDATETIME , XMLRPC_TYPE_INT, sizeof(WORD32), ASN_UNSIGNED, SNMP_OID_INDEX_LEN_COMMON, (char *)"VMMDATETIME", 
                                  SnmpGet_ftype_OID_WORD32_Value, SnmpSet_ftype_OID_WORD32_Value, 
                                  SnmpSet_ftype_WORD32_Value, 
                                  SnmpGet_snmptype_WORD32_Value, SnmpSet_snmptype_WORD32_Value, 
                                  SnmpCheck_snmptype_WORD32_Value, Snmp_snprintf_WORD32},
/* 36 */{DATA_TYPE_VMMDATE , XMLRPC_TYPE_INT, sizeof(WORD32), ASN_UNSIGNED, SNMP_OID_INDEX_LEN_COMMON, (char *)"VMMDATE", 
                                  SnmpGet_ftype_OID_WORD32_Value, SnmpSet_ftype_OID_WORD32_Value, 
                                  SnmpSet_ftype_WORD32_Value, 
                                  SnmpGet_snmptype_WORD32_Value, SnmpSet_snmptype_WORD32_Value, 
                                  SnmpCheck_snmptype_WORD32_Value, Snmp_snprintf_WORD32},
/* 37 */{DATA_TYPE_DWORD, XMLRPC_TYPE_INT, sizeof(WORD32), ASN_UNSIGNED, SNMP_OID_INDEX_LEN_COMMON, (char *)"T_WORD32_HEX", 
                                  SnmpGet_ftype_OID_WORD32_Value, SnmpSet_ftype_OID_WORD32_Value,
                                  SnmpSet_ftype_WORD32_Value, 
                                  SnmpGet_snmptype_WORD32_Value, SnmpSet_snmptype_WORD32_Value, 
                                  SnmpCheck_snmptype_WORD32_Value, Snmp_snprintf_WORD32},
/* 38 */{DATA_TYPE_PHYADDRESS, XMLRPC_TYPE_STRING, sizeof(T_PhysAddress), ASN_OCTET_STR, SNMP_OID_INDEX_LEN_PHYADDR, (char *)"T_PhysAddressEx", 
                                  SnmpGet_ftype_OID_PHYADDR_Value, SnmpSet_ftype_OID_PHYADDR_Value, 
                                  SnmpSet_ftype_Comm_Value,
                                  SnmpGet_snmptype_Comm_Value, SnmpSet_snmptype_Comm_Value, 
                                  SnmpCheck_snmptype_Common_Value, NULL},
/* 39 */{DATA_TYPE_BYTE, XMLRPC_TYPE_INT, sizeof(BYTE), ASN_UNSIGNED, SNMP_OID_INDEX_LEN_COMMON, (char *)"T_PLAN", 
                                  SnmpGet_ftype_OID_BYTE_Value, SnmpSet_ftype_OID_BYTE_Value, 
                                  SnmpSet_ftype_BYTE_Value,  
                                  SnmpGet_snmptype_BYTE_Value, SnmpSet_snmptype_BYTE_Value, 
                                  SnmpCheck_snmptype_BYTE_Value, Snmp_snprintf_BYTE},
/* 40 */{DATA_TYPE_UINT32, XMLRPC_TYPE_INT, sizeof(uint32), ASN_UNSIGNED, SNMP_OID_INDEX_LEN_COMMON, (char *)"uint32", 
                                  SnmpGet_ftype_OID_UINT32_Value, SnmpSet_ftype_OID_UINT32_Value,
                                  SnmpSet_ftype_UINT32_Value, 
                                  SnmpGet_snmptype_UINT32_Value, SnmpSet_snmptype_UINT32_Value, 
                                  SnmpCheck_snmptype_WORD32_Value, Snmp_snprintf_WORD32},
};

#define DATA_TYPE_END (sizeof(g_tSnmpSerialFtype)/sizeof(g_tSnmpSerialFtype[0]))
#define SNMP_NSERIALDATATYPE_NUM (sizeof(g_tSnmpNSerialFtype)/sizeof(g_tSnmpNSerialFtype[0]))

static SNMP_FTYPE *SnmpGetTypeHandler(BYTE ftype);

/**************************************************************************
* 函数名称：SnmpConvert_atoi
* 功能描述：atoi转换函数
* 访问的表：无
* 修改的表：无
* 输入参数：pString -- 字符串
* 输出参数：pDword -- 转换后的WORD32的值
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/08/28    V1.0    曹亮        创建
**************************************************************************/
static int SnmpConvert_atoi(char * pString, WORD32 * pDword)
{
    char *pChar = pString;
    WORD32 dVal = 0;
    int nDigit = 0;
    int nLength = 0;

    *pDword = 0;
    nLength = strlen(pString);
    if (nLength == 0)
        OAM_RETURN -1;

    nLength = 0;

    while (pChar != NULL && *pChar != '\0')
    {
        /* not a digit */
        if (*pChar < '0' || *pChar > '9')
            OAM_RETURN -1;

        nDigit = *pChar - '0';

        /* overflow */
        if (dVal > (0xffffffff - nDigit) / 10)
            OAM_RETURN -1;
        dVal = dVal * 10 + nDigit;
        if (dVal > 0)
            nLength++;

        /* too many digits */
        if (nLength > 10)
            OAM_RETURN -1;
        pChar++;
    }

    *pDword = dVal;
    OAM_RETURN 0;
}

/**************************************************************************
* 函数名称：SnmpSet_ftype_CHAR_Value
* 功能描述：设置CHAR 字符串数据
* 访问的表：无
* 修改的表：无
* 输入参数：cStr -- 该数据的字符串表示
* 输出参数：ptValue -- 存放设置后的数据
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2010/06/08    V1.0    刘华振        创建
**************************************************************************/
static void SnmpSet_ftype_CHAR_Value(T_SNMP_NodeValue *ptValue, const char *cStr)
{
    OamFormatLog((char *)"ptValue is NULL!\n");
    OAM_RETURN_IF_FAIL(ptValue != NULL);
    OamFormatLog((char *)"cStr is NULL!\n");
    OAM_RETURN_IF_FAIL(cStr != NULL);
    OamFormatLog((char *)"cStr len is 0!\n");
    OAM_NASSERT_RETURN_IF_FAIL(strlen(cStr) != 0);
    memcpy(ptValue->cStr, cStr, STR_VALUE_MAX);
}/* SnmpSet_ftype_CHAR_Value */

/**************************************************************************
* 函数名称：SnmpSet_ftype_BYTE_Value
* 功能描述：设置字节数据
* 访问的表：无
* 修改的表：无
* 输入参数：cStr -- 该数据的字符串表示
* 输出参数：ptValue -- 存放设置后的数据
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2010/06/08    V1.0    刘华振        创建
**************************************************************************/
static void SnmpSet_ftype_BYTE_Value(T_SNMP_NodeValue *ptValue, const char *cStr)
{
    OamFormatLog((char *)"ptValue is NULL!\n");
    OAM_RETURN_IF_FAIL(ptValue != NULL);
    OamFormatLog((char *)"cStr is NULL!\n");
    OAM_RETURN_IF_FAIL(cStr != NULL);
    
    memset(ptValue,0,sizeof(T_SNMP_NodeValue));
    ptValue->ucByte = (BYTE)atoi((char *)cStr);
}/* SnmpSet_ftype_BYTE_Value */

/**************************************************************************
* 函数名称：SnmpSet_ftype_WORD16_Value
* 功能描述：设置WORD16 数据
* 访问的表：无
* 修改的表：无
* 输入参数：cStr -- 该数据的字符串表示
* 输出参数：ptValue -- 存放设置后的数据
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2010/06/08    V1.0    刘华振        创建
**************************************************************************/
static void SnmpSet_ftype_WORD16_Value(T_SNMP_NodeValue *ptValue, const char *cStr)
{
    OamFormatLog((char *)"ptValue is NULL!\n");
    OAM_RETURN_IF_FAIL(ptValue != NULL);
    OamFormatLog((char *)"cStr is NULL!\n");
    OAM_RETURN_IF_FAIL(cStr != NULL);
    
    memset(ptValue,0,sizeof(T_SNMP_NodeValue));
    ptValue->wWord = (WORD16)atoi((char *)cStr);
}/* SnmpSet_ftype_WORD16_Value */

/**************************************************************************
* 函数名称：SnmpSet_ftype_SWORD16_Value
* 功能描述：设置SWORD16  数据
* 访问的表：无
* 修改的表：无
* 输入参数：cStr -- 该数据的字符串表示
* 输出参数：ptValue -- 存放设置后的数据
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2010/06/08    V1.0    刘华振        创建
**************************************************************************/
static void SnmpSet_ftype_SWORD16_Value(T_SNMP_NodeValue *ptValue, const char *cStr)
{
    OamFormatLog((char *)"ptValue is NULL!\n");
    OAM_RETURN_IF_FAIL(ptValue != NULL);
    OamFormatLog((char *)"cStr is NULL!\n");
    OAM_RETURN_IF_FAIL(cStr != NULL);
    
    memset(ptValue,0,sizeof(T_SNMP_NodeValue));
    ptValue->sWord = (SWORD16)atoi((char *)cStr);
}/* SnmpSet_ftype_SWORD16_Value */


/**************************************************************************
* 函数名称：SnmpSet_ftype_WORD32_Value
* 功能描述：设置WORD32  数据
* 访问的表：无
* 修改的表：无
* 输入参数：cStr -- 该数据的字符串表示
* 输出参数：ptValue -- 存放设置后的数据
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2010/06/08    V1.0    刘华振        创建
**************************************************************************/
static void SnmpSet_ftype_WORD32_Value(T_SNMP_NodeValue *ptValue, const char *cStr)
{
    OamFormatLog((char *)"ptValue is NULL!\n");
    OAM_RETURN_IF_FAIL(ptValue != NULL);
    OamFormatLog((char *)"cStr is NULL!\n");
    OAM_RETURN_IF_FAIL(cStr != NULL);
    
    memset(ptValue,0,sizeof(T_SNMP_NodeValue));
    SnmpConvert_atoi((char *)cStr, &ptValue->dwWord);
}/* SnmpSet_ftype_WORD32_Value */

/**************************************************************************
* 函数名称：SnmpSet_ftype_UINT32_Value
* 功能描述：设置WORD32  数据
* 访问的表：无
* 修改的表：无
* 输入参数：cStr -- 该数据的字符串表示
* 输出参数：ptValue -- 存放设置后的数据
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2010/06/08    V1.0    刘华振        创建
**************************************************************************/
static void SnmpSet_ftype_UINT32_Value(T_SNMP_NodeValue *ptValue, const char *cStr)
{
    OamFormatLog((char *)"ptValue is NULL!\n");
    OAM_RETURN_IF_FAIL(ptValue != NULL);
    OamFormatLog((char *)"cStr is NULL!\n");
    OAM_RETURN_IF_FAIL(cStr != NULL);
    
    memset(ptValue,0,sizeof(T_SNMP_NodeValue));
    SnmpConvert_atoi((char *)cStr, &ptValue->dwWord);
}/* SnmpSet_ftype_UINT32_Value */

/**************************************************************************
* 函数名称：SnmpSet_ftype_Comm_Value
* 功能描述：设置通用数据
* 访问的表：无
* 修改的表：无
* 输入参数：cStr -- 该数据的字符串表示
* 输出参数：ptValue -- 存放设置后的数据
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2010/11/08    V1.0    刘华振        创建
**************************************************************************/
static void SnmpSet_ftype_Comm_Value(T_SNMP_NodeValue *ptValue, const char *cStr)
{
    int len = 0;
    OamFormatLog((char *)"ptValue is NULL!\n");
    OAM_RETURN_IF_FAIL(ptValue != NULL);
    OamFormatLog((char *)"cStr is NULL!\n");
    OAM_RETURN_IF_FAIL(cStr != NULL);

    len = strlen(cStr);
    len = MIN(STR_VALUE_MAX, len);
    memset(ptValue,0,sizeof(T_SNMP_NodeValue));
    memcpy(ptValue->cStr, cStr, len);
}/* SnmpSet_ftype_Comm_Value */

static BYTE *SnmpGet_ftype_OID_CHAR_Value(BYTE *ptBuf, oid *name, WORD32 *oidLen, WORD16 *indexLen)
{
    WORD16 wIndexLen = 0;
    OamFormatLog((char *)"ptBuf is NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(ptBuf != NULL, ptBuf);
    OamFormatLog((char *)"name is NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(name != NULL, ptBuf);
    OamFormatLog((char *)"oidLen is NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(oidLen != NULL, ptBuf);
    OamFormatLog((char *)"indexLen is NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(indexLen != NULL, ptBuf);

    /* 对于IPMI strlen(ptBuf) 应该等于*indexLen */
    /* 对于普通的，发送的结构中字符串是定长的，
    strlen(ptBuf)小或等于*indexLen, 且ptBuf后面都是补0对齐的。*/
    wIndexLen = strlen((char *)ptBuf);
    while(wIndexLen > 0 && (*oidLen) < MAX_OID_LENGTH)
    {
        GET_OID_NAME_FROM_BUF(char);
        wIndexLen -= sizeof(BYTE);
    }
    ptBuf += (*indexLen);
    (*indexLen) = 0;
    return ptBuf;
}/* SnmpGet_ftype_OID_CHAR_Value */

static BYTE *SnmpGet_ftype_OID_BYTE_Value(BYTE *ptBuf, oid *name, WORD32 *oidLen, WORD16 *indexLen)
{
    SNMP_GET_FTYPE_OID_T_VALUE(BYTE);
}/* SnmpGet_ftype_OID_BYTE_Value */

static BYTE *SnmpGet_ftype_OID_WORD16_Value(BYTE *ptBuf, oid *name, WORD32 *oidLen, WORD16 *indexLen)
{
    SNMP_GET_FTYPE_OID_T_VALUE(WORD16);
}/* SnmpGet_ftype_OID_WORD16_Value */

static BYTE  *SnmpGet_ftype_OID_WORD32_Value(BYTE *ptBuf, oid *name, WORD32 *oidLen, WORD16 *indexLen)
{
    SNMP_GET_FTYPE_OID_T_VALUE(WORD32);
}/* SnmpGet_ftype_OID_WORD32_Value */

static BYTE  *SnmpGet_ftype_OID_UINT32_Value(BYTE *ptBuf, oid *name, WORD32 *oidLen, WORD16 *indexLen)
{
    SNMP_GET_FTYPE_OID_T_VALUE(uint32);
}/* SnmpGet_ftype_OID_WORD32_Value */

static BYTE *SnmpGet_ftype_OID_PHYADDR_Value(BYTE *ptBuf, oid *name, WORD32 *oidLen, WORD16 *indexLen)
{
    WORD16 wIndex = 0;
    OamFormatLog((char *)"ptBuf is NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(ptBuf != NULL, ptBuf);
    OamFormatLog((char *)"name is NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(name != NULL, ptBuf);
    OamFormatLog((char *)"oidLen is NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(oidLen != NULL, ptBuf);
    OamFormatLog((char *)"indexLen is NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(indexLen != NULL, ptBuf);

    for(wIndex=0;wIndex<4;wIndex++)
    {
        GET_OID_NAME_FROM_BUF(BYTE);
    }
    return ptBuf;
}/* SnmpGet_ftype_OID_PHYADDR_Value */

static BYTE *SnmpGet_ftype_OID_IPADDR_Value(BYTE *ptBuf, oid *name, WORD32 *oidLen, WORD16 *indexLen)
{
    WORD32 dwIPAddress = 0;
    OamFormatLog((char *)"ptBuf is NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(ptBuf != NULL, ptBuf);
    OamFormatLog((char *)"name is NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(name != NULL, ptBuf);
    OamFormatLog((char *)"oidLen is NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(oidLen != NULL, ptBuf);
    OamFormatLog((char *)"indexLen is NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(indexLen != NULL, ptBuf);

    dwIPAddress = (WORD32 )*ptBuf;
    name[*oidLen+0] = (dwIPAddress & 0xff000000) >> 24;
    name[*oidLen+1] = (dwIPAddress & 0x00ff0000) >> 16;
    name[*oidLen+2] = (dwIPAddress & 0x0000ff00) >> 8;
    name[*oidLen+3] = (dwIPAddress & 0x000000ff);
    *oidLen += 4;
    *indexLen -= 4;
    ptBuf += 4;
    return ptBuf;
}/* SnmpGet_ftype_OID_IPADDR_Value */

static BYTE *SnmpSet_ftype_OID_BYTE_Value(BYTE *ptBuf, const oid *name, WORD32 *oidLen, WORD16 *indexLen, WORD32 *dwIndexDeserveLen)
{
    (*dwIndexDeserveLen)++;
    SNMP_SET_FTYPE_OID_T_VALUE(BYTE);
}/* SnmpSet_ftype_OID_BYTE_Value */

static BYTE *SnmpSet_ftype_OID_WORD16_Value(BYTE *ptBuf, const oid *name, WORD32 *oidLen, WORD16 *indexLen, WORD32 *dwIndexDeserveLen)
{
    (*dwIndexDeserveLen)++;
    SNMP_SET_FTYPE_OID_T_VALUE(WORD16);
}/* SnmpSet_ftype_OID_WORD16_Value */

static BYTE *SnmpSet_ftype_OID_WORD32_Value(BYTE *ptBuf, const oid *name, WORD32 *oidLen, WORD16 *indexLen, WORD32 *dwIndexDeserveLen)
{
    (*dwIndexDeserveLen)++;
    SNMP_SET_FTYPE_OID_T_VALUE(WORD32);
    return ptBuf;
}/* SnmpSet_ftype_OID_WORD32_Value */

static BYTE *SnmpSet_ftype_OID_UINT32_Value(BYTE *ptBuf, const oid *name, WORD32 *oidLen, WORD16 *indexLen, WORD32 *dwIndexDeserveLen)
{
    (*dwIndexDeserveLen)++;
    SNMP_SET_FTYPE_OID_T_VALUE(uint32);
    return ptBuf;
}/* SnmpSet_ftype_OID_WORD32_Value */


static BYTE *SnmpSet_ftype_OID_PHYADDR_Value(BYTE *ptBuf, const oid *name, WORD32 *oidLen, WORD16 *indexLen, WORD32 *dwIndexDeserveLen)
{
    BYTE tmp = 0;
    WORD16 wIndex = 0;

    OamFormatLog((char *)"ptBuf is NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(ptBuf != NULL, ptBuf);
    OamFormatLog((char *)"name is NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(name != NULL, ptBuf);
    OamFormatLog((char *)"oidLen is NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(oidLen != NULL, ptBuf);
    OamFormatLog((char *)"indexLen is NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(indexLen != NULL, ptBuf);

    for(wIndex=0; wIndex<4; wIndex++)
    {
        SET_OID_NAME_TO_BUF(BYTE);
    }
    (*dwIndexDeserveLen) += 5;
    return ptBuf;
}/* SnmpSet_ftype_OID_PHYADDR_Value */

static BYTE *SnmpSet_ftype_OID_IPADDR_Value(BYTE *ptBuf, const oid *name, WORD32 *oidLen, WORD16 *indexLen,WORD32 *dwIndexDeserveLen)
{
    WORD32 dwIpAddress = 0;

    OamFormatLog((char *)"ptBuf is NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(ptBuf != NULL, ptBuf);
    OamFormatLog((char *)"name is NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(name != NULL, ptBuf);
    OamFormatLog((char *)"oidLen is NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(oidLen != NULL, ptBuf);
    OamFormatLog((char *)"indexLen is NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(indexLen != NULL, ptBuf);

    dwIpAddress |= name[*oidLen+0]<<24;
    dwIpAddress |= name[*oidLen+1]<<16;
    dwIpAddress |= name[*oidLen+2]<<8;
    dwIpAddress |= name[*oidLen+3];
    memcpy(ptBuf, &dwIpAddress, 4);
    ptBuf += 4;
    (*oidLen) += 4;
    if (*indexLen >= 4)
    {
        (*indexLen) -= 4;
    }
    (*dwIndexDeserveLen) += 4;
    return ptBuf;
}/* SnmpSet_ftype_OID_IPADDR_Value */

/**************************************************************************
* 函数名称：SnmpGet_snmptype_BYTE_Value
* 功能描述：获取BYTE  数据
* 访问的表：无
* 修改的表：无
* 输入参数：ptBuf -- 存放数据的Buf
                              ftypesize -- 基本长度
                              wArrNum -- 数组个数
* 输出参数：var_len -- 该数据的字符串表示
* 返 回 值：数据指针
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2010/06/08    V1.0    刘华振        创建
**************************************************************************/
static BYTE *SnmpGet_snmptype_BYTE_Value(const BYTE *ptBuf, WORD32 ftypesize, WORD16 wArrNum, size_t *var_len)
{
    SNMP_GET_SNMPFTYPE_T_VALUE(BYTE);
}/* SnmpGet_snmptype_BYTE_Value */

/**************************************************************************
* 函数名称：SnmpGet_snmptype_CHAR_Value
* 功能描述：获取char  数据
* 访问的表：无
* 修改的表：无
* 输入参数：ptBuf -- 存放数据的Buf
                              ftypesize -- 基本长度
                              wArrNum -- 数组个数
* 输出参数：var_len -- 该数据的字符串表示
* 返 回 值：数据指针
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2010/08/08    V1.0    刘华振        创建
**************************************************************************/
static BYTE *SnmpGet_snmptype_CHAR_Value(const BYTE *ptBuf, WORD32 ftypesize, WORD16 wArrNum, size_t *var_len)
{
    OamFormatLog((char *)"ptBuf is NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(ptBuf != NULL, NULL);
    OamFormatLog((char *)"var_len is NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(var_len != NULL, NULL);
    OamFormatLog((char *)"wArrNum = %u!\n", wArrNum);
    OAM_NASSERT_RETURN_VAL_IF_FAIL(wArrNum > 0, NULL);

    *var_len = strlen((char *)ptBuf);
     return (unsigned char *)ptBuf;
}/* SnmpGet_snmptype_CHAR_Value */

/**************************************************************************
* 函数名称：SnmpGet_snmptype_WORD16_Value
* 功能描述：获取WORD16 数据
* 访问的表：无
* 修改的表：无
* 输入参数：ptBuf -- 存放数据的Buf
                              ftypesize -- 基本长度
                              wArrNum -- 数组个数
* 输出参数：var_len -- 该数据的字符串表示
* 返 回 值：数据指针
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2010/06/08    V1.0    刘华振        创建
**************************************************************************/
static BYTE *SnmpGet_snmptype_WORD16_Value(const BYTE *ptBuf, WORD32 ftypesize, WORD16 wArrNum, size_t *var_len)
{
    SNMP_GET_SNMPFTYPE_T_VALUE(WORD16);
}/* SnmpGet_snmptype_WORD16_Value */

/**************************************************************************
* 函数名称：SnmpGet_snmptype_SWORD16_Value
* 功能描述：获取SWORD16 数据
* 访问的表：无
* 修改的表：无
* 输入参数：ptBuf -- 存放数据的Buf
                              ftypesize -- 基本长度
                              wArrNum -- 数组个数
* 输出参数：var_len -- 该数据的字符串表示
* 返 回 值：数据指针
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2010/09/15    V1.0    刘华振        创建
**************************************************************************/
static BYTE *SnmpGet_snmptype_SWORD16_Value(const BYTE *ptBuf, WORD32 ftypesize, WORD16 wArrNum, size_t *var_len)
{
    SNMP_GET_SNMPFTYPE_T_VALUE(SWORD16);
}/* SnmpGet_snmptype_SWORD16_Value */


/**************************************************************************
* 函数名称：SnmpGet_snmptype_IPADDR_Value
* 功能描述：获取IP 数据
* 访问的表：无
* 修改的表：无
* 输入参数：ptBuf -- 存放数据的Buf
                              ftypesize -- 基本长度
                              wArrNum -- 数组个数
* 输出参数：var_len -- 该数据的字符串表示
* 返 回 值：数据指针
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2010/10/29    V1.0    刘华振        创建
**************************************************************************/
static BYTE *SnmpGet_snmptype_IPADDR_Value(const BYTE *ptBuf, WORD32 ftypesize, WORD16 wArrNum, size_t *var_len)
{
    static long long_ret = 0;
    WORD32 dwIP = 0;
    char   szIP[20] = {0};
    OamFormatLog((char *)"ptBuf is NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(ptBuf != NULL, NULL);
    OamFormatLog((char *)"var_len is NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(var_len != NULL, NULL);
    OamFormatLog((char *)"wArrNum = %u!\n", wArrNum);
    OAM_NASSERT_RETURN_VAL_IF_FAIL(wArrNum > 0, NULL);

    if (wArrNum == 1)
    {
    	dwIP = *((WORD32*)ptBuf); 
        sprintf(szIP, "%d.%d.%d.%d",(dwIP & 0xff000000)>>24,
                                     (dwIP & 0x00ff0000)>>16,
                                     (dwIP & 0x0000ff00)>>8,
                                     (dwIP & 0x000000ff));
        long_ret = (WORD32)inet_network(szIP);
        *var_len = sizeof(WORD32);
        return (BYTE *) &long_ret;   
    }
    *var_len = wArrNum * sizeof(WORD32);
    return (BYTE *) ptBuf;
}/* SnmpGet_snmptype_IPADDR_Value */

/**************************************************************************
* 函数名称：SnmpGet_snmptype_WORD32_Value
* 功能描述：获取WORD32 数据
* 访问的表：无
* 修改的表：无
* 输入参数：ptBuf -- 存放数据的Buf
                              ftypesize -- 基本长度
                              wArrNum -- 数组个数
* 输出参数：var_len -- 该数据的字符串表示
* 返 回 值：数据指针
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2010/06/08    V1.0    刘华振        创建
**************************************************************************/
static BYTE *SnmpGet_snmptype_WORD32_Value(const BYTE *ptBuf, WORD32 ftypesize, WORD16 wArrNum, size_t *var_len)
{
    SNMP_GET_SNMPFTYPE_T_VALUE(WORD32);
}/* SnmpGet_snmptype_WORD32_Value */

/**************************************************************************
* 函数名称：SnmpGet_snmptype_UINT32_Value
* 功能描述：获取WORD32 数据
* 访问的表：无
* 修改的表：无
* 输入参数：ptBuf -- 存放数据的Buf
                              ftypesize -- 基本长度
                              wArrNum -- 数组个数
* 输出参数：var_len -- 该数据的字符串表示
* 返 回 值：数据指针
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2010/06/08    V1.0    刘华振        创建
**************************************************************************/
static BYTE *SnmpGet_snmptype_UINT32_Value(const BYTE *ptBuf, WORD32 ftypesize, WORD16 wArrNum, size_t *var_len)
{
    SNMP_GET_SNMPFTYPE_T_VALUE(uint32);
}/* SnmpGet_snmptype_WORD32_Value */

/**************************************************************************
* 函数名称：SnmpGet_snmptype_Comm_Value
* 功能描述：获取通用数据
* 访问的表：无
* 修改的表：无
* 输入参数：ptBuf -- 存放数据的Buf
                              ftypesize -- 基本长度
                              wArrNum -- 数组个数
* 输出参数：var_len -- 该数据的字符串表示
* 返 回 值：数据指针
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2010/06/08    V1.0    刘华振        创建
**************************************************************************/
static BYTE *SnmpGet_snmptype_Comm_Value(const BYTE *ptBuf, WORD32 ftypesize, WORD16 wArrNum, size_t *var_len)
{
    OamFormatLog((char *)"ptBuf is NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(ptBuf != NULL, NULL);
    OamFormatLog((char *)"var_len is NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(var_len != NULL, NULL);
    OamFormatLog((char *)"wArrNum = %u!\n", wArrNum);
    OAM_NASSERT_RETURN_VAL_IF_FAIL(wArrNum > 0, NULL);
    OamFormatLog((char *)"ftypesize = %u!\n", ftypesize);
    OAM_NASSERT_RETURN_VAL_IF_FAIL(ftypesize > 0, NULL);
    
    *var_len = wArrNum * ftypesize;
    return (unsigned char *)ptBuf;
}/* SnmpGet_snmptype_Comm_Value */

/**************************************************************************
* 函数名称：SnmpSet_snmptype_BYTE_Value
* 功能描述：设置BYTE  数据
* 访问的表：无
* 修改的表：无
* 输入参数：ptBuf -- 存放数据的Buf
                              ftypesize -- 基本长度
                              wArrNum -- 数组个数
                              pucVal--数据
* 输出参数：
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2010/06/08    V1.0    刘华振        创建
**************************************************************************/
static void SnmpSet_snmptype_BYTE_Value(BYTE *ptBuf, WORD32 ftypesize, WORD16 wArrNum, const BYTE * pucVal, size_t var_val_len)
{
    SNMP_SET_SNMPTYPE_T_VALUE(BYTE);
}/* SnmpSet_snmptype_BYTE_Value */

/**************************************************************************
* 函数名称：SnmpSet_snmptype_WORD16_Value
* 功能描述：设置WORD16 数据
* 访问的表：无
* 修改的表：无
* 输入参数：ptBuf -- 存放数据的Buf
                              dwBaseSize -- 基本长度
                              wArrNum -- 数组个数
                              pucVal--数据
* 输出参数：
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2010/06/08    V1.0    刘华振        创建
**************************************************************************/
static void SnmpSet_snmptype_WORD16_Value(BYTE *ptBuf, WORD32 ftypesize, WORD16 wArrNum, const BYTE * pucVal, size_t var_val_len)
{
    SNMP_SET_SNMPTYPE_T_VALUE(WORD16);
}/* SnmpSet_snmptype_WORD16_Value */

/**************************************************************************
* 函数名称：SnmpSet_snmptype_SWORD16_Value
* 功能描述：设置SWORD16 数据
* 访问的表：无
* 修改的表：无
* 输入参数：ptBuf -- 存放数据的Buf
                              dwBaseSize -- 基本长度
                              wArrNum -- 数组个数
                              pucVal--数据
* 输出参数：
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2010/09/15   V1.0    刘华振        创建
**************************************************************************/
static void SnmpSet_snmptype_SWORD16_Value(BYTE *ptBuf, WORD32 ftypesize, WORD16 wArrNum, const BYTE * pucVal, size_t var_val_len)
{
    SNMP_SET_SNMPTYPE_T_VALUE(SWORD16);
}/* SnmpSet_snmptype_SWORD16_Value */


/**************************************************************************
* 函数名称：SnmpSet_snmptype_IPADDR_Value
* 功能描述：设置IP数据
* 访问的表：无
* 修改的表：无
* 输入参数：ptBuf -- 存放数据的Buf
                              dwBaseSize -- 基本长度
                              wArrNum -- 数组个数
                              pucVal--数据
* 输出参数：
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2010/10/28    V1.0    刘华振        创建
**************************************************************************/
static void SnmpSet_snmptype_IPADDR_Value(BYTE *ptBuf, WORD32 ftypesize, WORD16 wArrNum, const BYTE * pucVal, size_t var_val_len)
{
    WORD16 wMinLen = 0;
    WORD32 dwIP = 0;
    OamFormatLog((char *)"ptBuf is NULL!\n");
    OAM_RETURN_IF_FAIL(ptBuf != NULL);
    OamFormatLog((char *)"pucVal is NULL!\n");
    OAM_RETURN_IF_FAIL(pucVal != NULL);
    OamFormatLog((char *)"wArrNum = %u!\n", wArrNum);
    OAM_NASSERT_RETURN_IF_FAIL(wArrNum > 0);

    if (wArrNum == 1)
    {
        dwIP = 0;
        dwIP |= pucVal[0]<<24;
        dwIP |= pucVal[1]<<16;
        dwIP |= pucVal[2]<<8;
        dwIP |= pucVal[3];
        memcpy(ptBuf, &dwIP, sizeof(WORD32));
    }
    else
    {
        wMinLen = MIN(wArrNum * ftypesize, var_val_len);
        memcpy(ptBuf, pucVal, wMinLen);
    }
    return;
}/* SnmpSet_snmptype_IPADDR_Value */

/**************************************************************************
* 函数名称：SnmpSet_snmptype_WORD32_Value
* 功能描述：设置WORD32数据
* 访问的表：无
* 修改的表：无
* 输入参数：ptBuf -- 存放数据的Buf
                              dwBaseSize -- 基本长度
                              wArrNum -- 数组个数
                              pucVal--数据
* 输出参数：
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2010/06/08    V1.0    刘华振        创建
**************************************************************************/
static void SnmpSet_snmptype_WORD32_Value(BYTE *ptBuf, WORD32 ftypesize, WORD16 wArrNum, const BYTE * pucVal, size_t var_val_len)
{
    SNMP_SET_SNMPTYPE_T_VALUE(WORD32);
}/* SnmpSet_snmptype_WORD32_Value */

/**************************************************************************
* 函数名称：SnmpSet_snmptype_UINT32_Value
* 功能描述：设置uint32数据
* 访问的表：无
* 修改的表：无
* 输入参数：ptBuf -- 存放数据的Buf
                              dwBaseSize -- 基本长度
                              wArrNum -- 数组个数
                              pucVal--数据
* 输出参数：
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2010/06/08    V1.0    刘华振        创建
**************************************************************************/
static void SnmpSet_snmptype_UINT32_Value(BYTE *ptBuf, WORD32 ftypesize, WORD16 wArrNum, const BYTE * pucVal, size_t var_val_len)
{
    SNMP_SET_SNMPTYPE_T_VALUE(uint32);
}/* SnmpSet_snmptype_WORD32_Value */

/**************************************************************************
* 函数名称：SnmpSet_snmptype_CHAR_Value
* 功能描述：设置char 数据
* 访问的表：无
* 修改的表：无
* 输入参数：ptBuf -- 存放数据的Buf
                              dwBaseSize -- 基本长度
                              wArrNum -- 数组个数
                              pucVal--数据
* 输出参数：
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2010/06/08    V1.0    刘华振        创建
**************************************************************************/
static void SnmpSet_snmptype_CHAR_Value(BYTE *ptBuf, WORD32 ftypesize, WORD16 wArrNum, const BYTE * pucVal, size_t var_val_len)
{
    WORD16 wMinLen = 0;
    OamFormatLog((char *)"ptBuf is NULL!\n");
    OAM_RETURN_IF_FAIL(ptBuf != NULL);
    OamFormatLog((char *)"pucVal is NULL!\n");
    OAM_RETURN_IF_FAIL(pucVal != NULL);
    OamFormatLog((char *)"wArrNum = %u!\n", wArrNum);
    OAM_NASSERT_RETURN_IF_FAIL(wArrNum > 0);

    wMinLen = MIN(wArrNum * ftypesize, var_val_len);
    memcpy(ptBuf, pucVal, wMinLen);
    ptBuf[wMinLen] = '\0';
    return;
}/* SnmpSet_snmptype_CHAR_Value */

/**************************************************************************
* 函数名称：SnmpSet_snmptype_Comm_Value
* 功能描述：设置通用数据
* 访问的表：无
* 修改的表：无
* 输入参数：ptBuf -- 存放数据的Buf
                              dwBaseSize -- 基本长度
                              wArrNum -- 数组个数
                              pucVal--数据
* 输出参数：
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2010/06/08    V1.0    刘华振        创建
**************************************************************************/
static void SnmpSet_snmptype_Comm_Value(BYTE *ptBuf, WORD32 ftypesize, WORD16 wArrNum, const BYTE * pucVal, size_t var_val_len)
{
    WORD16 wMinLen = 0;
    OamFormatLog((char *)"ptBuf is NULL!\n");
    OAM_RETURN_IF_FAIL(ptBuf != NULL);
    OamFormatLog((char *)"pucVal is NULL!\n");
    OAM_RETURN_IF_FAIL(pucVal != NULL);
    OamFormatLog((char *)"wArrNum = %u!\n", wArrNum);
    OAM_NASSERT_RETURN_IF_FAIL(wArrNum > 0);

    wMinLen = MIN(wArrNum * ftypesize, var_val_len);
    memcpy(ptBuf, pucVal, wMinLen);
    return;
}/* SnmpSet_snmptype_Comm_Value */

/**************************************************************************
* 函数名称：SnmpCheck_snmptype_BYTE_Value
* 功能描述：Set 命令时校验BYTE 值参数是否有效
* 访问的表：无
* 修改的表：无
* 输入参数：var_val-- 待校验的值
                              var_val_type --值(snmp)类型
                              var_val_len -- 值长度;
                              snmptype -- snmp type;
                              pOidProp -- oid节点描述；
* 输出参数：无
* 返 回 值：成功的情况下返回SNMP_ERR_NOERROR;
* 其它说明：var_val 中的值最小为sizeof(long);
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2010/08/27    V1.0    刘华振        创建
**************************************************************************/
static int SnmpCheck_snmptype_BYTE_Value(u_char *var_val,u_char var_val_type,size_t var_val_len, BYTE snmptype, T_Oid_Property *pOidProp)
{    
    SNMP_VALUE_CHECK(ucByte, BYTE);
    return SNMP_ERR_NOERROR;
}/* SnmpCheck_snmptype_BYTE_Value */

/**************************************************************************
* 函数名称：SnmpCheck_snmptype_CHAR_Value
* 功能描述：Set 命令时校验CHAR 值参数是否有效
* 访问的表：无
* 修改的表：无
* 输入参数：var_val-- 待校验的值
                              var_val_type --值(snmp)类型
                              var_val_len -- 值长度;
                              snmptype -- snmp type;
                              pOidProp -- oid节点描述；
* 输出参数：无
* 返 回 值：成功的情况下返回SNMP_ERR_NOERROR;
* 其它说明：var_val 中的值最小为sizeof(long);
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2010/09/15    V1.0    刘华振        创建
**************************************************************************/
static int SnmpCheck_snmptype_CHAR_Value(u_char *var_val,u_char var_val_type,size_t var_val_len, BYTE snmptype, T_Oid_Property *pOidProp)
{
    OamFormatLog((char *)"var_val is NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(var_val != NULL, SNMP_ERR_GENERR);
    OamFormatLog((char *)"pOidProp is NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(pOidProp != NULL, SNMP_ERR_GENERR);
    
    OAM_RETURN_VAL_IF_FAIL(var_val_type == ASN_OCTET_STR, SNMP_ERR_WRONGTYPE);
    
    if (var_val_len > (size_t)pOidProp->wParaLen-1)
    {
        return SNMP_ERR_WRONGLENGTH;
    }
    return SNMP_ERR_NOERROR;
}/* SnmpCheck_snmptype_CHAR_Value */

/**************************************************************************
* 函数名称：SnmpCheck_snmptype_WORD16_Value
* 功能描述：Set 命令时校验WORD16 值参数是否有效
* 访问的表：无
* 修改的表：无
* 输入参数：var_val-- 待校验的值
                              var_val_type --值(snmp)类型
                              var_val_len -- 值长度;
                              snmptype -- snmp type;
                              pOidProp -- oid节点描述；
* 输出参数：无
* 返 回 值：成功的情况下返回SNMP_ERR_NOERROR;
* 其它说明：var_val 中的值最小为sizeof(long);
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2010/08/27    V1.0    刘华振        创建
**************************************************************************/
static int SnmpCheck_snmptype_WORD16_Value(u_char *var_val,u_char var_val_type,size_t var_val_len, BYTE snmptype,  T_Oid_Property *pOidProp)
{
    SNMP_VALUE_CHECK(wWord, WORD16);
    return SNMP_ERR_NOERROR;
}/* SnmpCheck_snmptype_WORD16_Value */

/**************************************************************************
* 函数名称：SnmpCheck_snmptype_SWORD16_Value
* 功能描述：Set 命令时校验SWORD16 值参数是否有效
* 访问的表：无
* 修改的表：无
* 输入参数：var_val-- 待校验的值
                              var_val_type --值(snmp)类型
                              var_val_len -- 值长度;
                              snmptype -- snmp type;
                              pOidProp -- oid节点描述；
* 输出参数：无
* 返 回 值：成功的情况下返回SNMP_ERR_NOERROR;
* 其它说明：var_val 中的值最小为sizeof(long);
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2010/09/15    V1.0    刘华振        创建
**************************************************************************/
static int SnmpCheck_snmptype_SWORD16_Value(u_char *var_val,u_char var_val_type,size_t var_val_len, BYTE snmptype,  T_Oid_Property *pOidProp)
{
    SNMP_VALUE_CHECK(sWord, SWORD16);
    return SNMP_ERR_NOERROR;
}/* SnmpCheck_snmptype_SWORD16_Value */


/**************************************************************************
* 函数名称：SnmpCheck_snmptype_WORD32_Value
* 功能描述：Set 命令时校验WORD32 值参数是否有效
* 访问的表：无
* 修改的表：无
* 输入参数：var_val-- 待校验的值
                              var_val_type --值(snmp)类型
                              var_val_len -- 值长度;
                              snmptype -- snmp type;
                              pOidProp -- oid节点描述；
* 输出参数：无
* 返 回 值：成功的情况下返回SNMP_ERR_NOERROR;
* 其它说明：var_val 中的值最小为sizeof(long);
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2010/08/27    V1.0    刘华振        创建
**************************************************************************/
static int SnmpCheck_snmptype_WORD32_Value(u_char *var_val,u_char var_val_type,size_t var_val_len, BYTE snmptype, T_Oid_Property *pOidProp)
{
    SNMP_VALUE_CHECK(dwWord, WORD32);
    return SNMP_ERR_NOERROR;
}/* SnmpCheck_snmptype_WORD32_Value */

/**************************************************************************
* 函数名称：SnmpCheck_snmptype_Common_Value
* 功能描述：Set 命令时校验 通用 值参数是否有效
* 访问的表：无
* 修改的表：无
* 输入参数：var_val-- 待校验的值
                              var_val_type --值(snmp)类型
                              var_val_len -- 值长度;
                              snmptype -- snmp type;
                              pOidProp -- oid节点描述；
* 输出参数：无
* 返 回 值：成功的情况下返回SNMP_ERR_NOERROR;
* 其它说明：var_val 中的值最小为sizeof(long);
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2010/08/27    V1.0    刘华振        创建
**************************************************************************/
static int SnmpCheck_snmptype_Common_Value(u_char *var_val,u_char var_val_type,size_t var_val_len, BYTE snmptype, T_Oid_Property *pOidProp)
{
    OamFormatLog((char *)"var_val is NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(var_val != NULL, SNMP_ERR_GENERR);
    OamFormatLog((char *)"pOidProp is NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(pOidProp != NULL, SNMP_ERR_GENERR);
    
    OAM_NASSERT_RETURN_VAL_IF_FAIL(var_val_type == snmptype, SNMP_ERR_WRONGTYPE);
    OAM_NASSERT_RETURN_VAL_IF_FAIL(var_val_len <= pOidProp->wParaLen, SNMP_ERR_WRONGLENGTH);
    return SNMP_ERR_NOERROR;
}/* SnmpCheck_snmptype_Common_Value */

static void Snmp_snprintf_BYTE(T_SNMP_NodeValue *pNodeValue, char *pszBuffer, WORD32 dwMaxCount)
{
    OamFormatLog((char *)"pNodeValue is NULL!\n");
    OAM_RETURN_IF_FAIL(pNodeValue != NULL);
    OamFormatLog((char *)"pszBuffer is NULL!\n");
    OAM_RETURN_IF_FAIL(pszBuffer != NULL);
    
    snprintf(pszBuffer, dwMaxCount, "%-6d", pNodeValue->ucByte);
}/* Snmp_snprintf_BYTE */

static void Snmp_snprintf_WORD16(T_SNMP_NodeValue *pNodeValue, char *pszBuffer, WORD32 dwMaxCount)
{
    OamFormatLog((char *)"pNodeValue is NULL!\n");
    OAM_RETURN_IF_FAIL(pNodeValue != NULL);
    OamFormatLog((char *)"pszBuffer is NULL!\n");
    OAM_RETURN_IF_FAIL(pszBuffer != NULL);
    
    snprintf(pszBuffer, dwMaxCount, "%-6d", pNodeValue->wWord);
}/* Snmp_snprintf_WORD16 */

static void Snmp_snprintf_WORD32(T_SNMP_NodeValue *pNodeValue, char *pszBuffer, WORD32 dwMaxCount)
{
    OamFormatLog((char *)"pNodeValue is NULL!\n");
    OAM_RETURN_IF_FAIL(pNodeValue != NULL);
    OamFormatLog((char *)"pszBuffer is NULL!\n");
    OAM_RETURN_IF_FAIL(pszBuffer != NULL);
    
    snprintf(pszBuffer, dwMaxCount, "%-6u", pNodeValue->dwWord);
}/* Snmp_snprintf_WORD32 */

static void Snmp_snprintf_WORD64(T_SNMP_NodeValue *pNodeValue, char *pszBuffer, WORD32 dwMaxCount)
{
    OamFormatLog((char *)"pNodeValue is NULL!\n");
    OAM_RETURN_IF_FAIL(pNodeValue != NULL);
    OamFormatLog((char *)"pszBuffer is NULL!\n");
    OAM_RETURN_IF_FAIL(pszBuffer != NULL);
    
    snprintf(pszBuffer, dwMaxCount, "%-6llu", pNodeValue->ddwWord);
}

static void Snmp_snprintf_SWORD16(T_SNMP_NodeValue *pNodeValue, char *pszBuffer, WORD32 dwMaxCount)
{
    OamFormatLog((char *)"pNodeValue is NULL!\n");
    OAM_RETURN_IF_FAIL(pNodeValue != NULL);
    OamFormatLog((char *)"pszBuffer is NULL!\n");
    OAM_RETURN_IF_FAIL(pszBuffer != NULL);
    
    snprintf(pszBuffer, dwMaxCount, "%-6d", pNodeValue->sWord);
}/* Snmp_snprintf_SWORD16 */


static void Snmp_snprintf_IPADDR(T_SNMP_NodeValue *pNodeValue, char *pszBuffer, WORD32 dwMaxCount)
{
    OamFormatLog((char *)"pNodeValue is NULL!\n");
    OAM_RETURN_IF_FAIL(pNodeValue != NULL);
    OamFormatLog((char *)"pszBuffer is NULL!\n");
    OAM_RETURN_IF_FAIL(pszBuffer != NULL);

    snprintf (pszBuffer, dwMaxCount, "%u.%u.%u.%u", 
             (pNodeValue->dwWord & 0xff000000)>>24, 
             (pNodeValue->dwWord & 0x00ff0000)>>16, 
             (pNodeValue->dwWord & 0x0000ff00)>>8, 
             (pNodeValue->dwWord & 0x000000ff));
    
    snprintf(pszBuffer, dwMaxCount, "%-6u", pNodeValue->dwWord);
}/* Snmp_snprintf_IPADDR */

static void Snmp_snprintf_CHAR(T_SNMP_NodeValue *pNodeValue, char *pszBuffer, WORD32 dwMaxCount)
{
    OamFormatLog((char *)"pNodeValue is NULL!\n");
    OAM_RETURN_IF_FAIL(pNodeValue != NULL);
    OamFormatLog((char *)"pszBuffer is NULL!\n");
    OAM_RETURN_IF_FAIL(pszBuffer != NULL);
    
    snprintf(pszBuffer, dwMaxCount, "%-6s", pNodeValue->cStr);
}/* Snmp_snprintf_CHAR */    

/**************************************************************************
* 函数名称：SnmpGetTypeHandler
* 功能描述：获取某一数据类型的信息
* 访问的表：无
* 修改的表：无
* 输入参数：dwParaType-- 要获取信息的类型
* 输出参数：无
* 返 回 值：该类型的处理函数或NULL
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2010/06/08    V1.0    刘华振        创建
**************************************************************************/
static SNMP_FTYPE *SnmpGetTypeHandler(BYTE ftype)
{
    WORD16 wIndex = 0;
    if ((ftype < DATA_TYPE_END) && (ftype > DATA_TYPE_BEGIN))
    {
        OamFormatLog((char *)"ftype =  %d, g_tSnmpSerialFtype[ftype].ftype = %d.\n", ftype, g_tSnmpSerialFtype[ftype].ftype);
        OAM_NASSERT_RETURN_VAL_IF_FAIL(ftype == g_tSnmpSerialFtype[ftype].ftype, NULL);
        return &g_tSnmpSerialFtype[ftype];
    }

    for (wIndex = 0; wIndex < SNMP_NSERIALDATATYPE_NUM; wIndex++)
    {
        if (g_tSnmpNSerialFtype[wIndex].ftype == ftype)
        {
            return &g_tSnmpNSerialFtype[wIndex];
        }
    }

    OutPut(SYS_ERROR, " ftype(%u), is not registered!\n", ftype);
    return NULL;
}/* SnmpGetTypeHandler */

/**************************************************************************
* 函数名称：SnmpBufMove
* 功能描述：使数组偏移
* 访问的表：无
* 修改的表：无
* 输入参数：dwParaType-- 要获取信息的类型
                              ptBuf -- 数据起始地址;
                              wArrNum -- 数组长度
* 输出参数：无
* 返 回 值：成功的情况下返回偏移后的地址，否则
                          返回原来的地址;
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2010/06/08    V1.0    刘华振        创建
**************************************************************************/
BYTE *SnmpBufMove(BYTE *ptBuf, BYTE ftype, WORD16 wArrNum)
{
    SNMP_FTYPE * ptTypeHandler = NULL;
    BYTE *pTempData = ptBuf;

    OamFormatLog((char *)"ptBuf is NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(ptBuf != NULL, ptBuf);
    OamFormatLog((char *)"wArrNum = %u!\n", wArrNum);
    OAM_NASSERT_RETURN_VAL_IF_FAIL(wArrNum > 0, ptBuf);

    ptTypeHandler = SnmpGetTypeHandler(ftype);
    OamFormatLog((char *)"ftype(%u), is not registered!\n", ftype);
    OAM_NASSERT_RETURN_VAL_IF_FAIL(ptTypeHandler != NULL, ptBuf);
    pTempData = ptBuf + ptTypeHandler->ftypesize * wArrNum;
    return pTempData;
}/* SnmpBufMove */

/**************************************************************************
* 函数名称：SnmpGet_snmptype_Value
* 功能描述：获取 ptBuf 中的值
* 访问的表：无
* 修改的表：无
* 输入参数：ftype-- 要获取信息的类型(如果BYTE, WORD16等)
                              ptBuf -- 数据起始地址;
                              wArrNum -- 数组长度;
* 输出参数：var_len -- 数据长度;
* 返 回 值：成功的情况下返回获取到的数据，否则返回NULL。
* 其它说明：因为在实际的编程中用到的类型为BYTE, WORD16等分别占1,2个字节。
                              SNMP中最小的长度为sizeof(long)，所以存在BYTE,WORD16等到long 的转换问题，
                              当内存中实际长度小于sizeof(long)时，不能简单地返回内存地址，
                              这里还要考虑字节序。 
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2010/06/08    V1.0    刘华振        创建
**************************************************************************/
BYTE *SnmpGet_snmptype_Value(BYTE ftype, const BYTE *ptBuf, WORD16 wArrNum, size_t *var_len)
{
    SNMP_FTYPE * ptTypeHandler = NULL;

    OamFormatLog((char *)"ptBuf is NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(ptBuf != NULL, NULL);
    OamFormatLog((char *)"var_len is NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(var_len != NULL, NULL);
    OamFormatLog((char *)"wArrNum = %u!\n", wArrNum);
    OAM_NASSERT_RETURN_VAL_IF_FAIL(wArrNum > 0, NULL);

    ptTypeHandler = SnmpGetTypeHandler(ftype);
    OamFormatLog((char *)"ftype(%u), is not registered!\n", ftype);
    OAM_NASSERT_RETURN_VAL_IF_FAIL(ptTypeHandler != NULL, NULL);

    if (ptTypeHandler->get_snmptype_value != NULL)
    {
        return ptTypeHandler->get_snmptype_value(ptBuf, ptTypeHandler->ftypesize, wArrNum, var_len);
    }
    OutPut(SYS_ERROR, " ftype(%u), is not registered(get_bufvalue = NULL)!\n", ftype);
    return NULL;
}/* SnmpGet_snmptype_Value */

/**************************************************************************
* 函数名称：SnmpCheck_snmptype_value
* 功能描述：Set 命令时校验参数是否有效
* 访问的表：无
* 修改的表：无
* 输入参数：var_val-- 待校验的值
                              var_val_type --值(snmp)类型
                              var_val_len -- 值长度;
                              pOidProp -- oid节点描述；
* 输出参数：无
* 返 回 值：成功的情况下返回SNMP_ERR_NOERROR;
* 其它说明：var_val 中的值最小为sizeof(long);
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2010/08/27    V1.0    刘华振        创建
**************************************************************************/
int SnmpCheck_snmptype_value(u_char *var_val, u_char var_val_type, size_t var_val_len, T_Oid_Property *pOidProp)
{
    SNMP_FTYPE * ptTypeHandler = NULL;

    OamFormatLog((char *)"var_val can't be NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(var_val != NULL, SNMP_ERR_NOERROR);
    OamFormatLog((char *)"pOidProp can't be NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(pOidProp != NULL, SNMP_ERR_NOERROR);
    OamFormatLog((char *)"var_val_len = %u!\n", var_val_len);
    OAM_NASSERT_RETURN_VAL_IF_FAIL(var_val_len > 0, SNMP_ERR_WRONGLENGTH);
    OamFormatLog((char *)"pOidProp->ucRwProp is Read Only!\n");
    OAM_NASSERT_RETURN_VAL_IF_FAIL(pOidProp->ucRwProp != OID_PROPERTY_READ, SNMP_ERR_INCONSISTENTVALUE);
    
    ptTypeHandler = SnmpGetTypeHandler(pOidProp->ucParaType);
    OamFormatLog((char *)"ftype(%u), is not registered!\n", pOidProp->ucParaType);
    OAM_NASSERT_RETURN_VAL_IF_FAIL(ptTypeHandler != NULL, SNMP_ERR_NOERROR);

    if (ptTypeHandler->check_snmptype_value != NULL)
    {
        return ptTypeHandler->check_snmptype_value(var_val, var_val_type, var_val_len, ptTypeHandler->snmptype, pOidProp);
    }
    OutPut(SYS_ERROR, " ftype(%u), is not registered(get_bufvalue = NULL)!\n", pOidProp->ucParaType);
    return SNMP_ERR_NOERROR;
}/* SnmpCheck_snmptype_value */

/**************************************************************************
* 函数名称：SnmpSet_snmptype_Value
* 功能描述：把pucVal 中的值设置到ptBuf中。
* 访问的表：无
* 修改的表：无
* 输入参数：ftype-- 要要设置的信息的类型(如BYTE, WORD16等)
                              ptBuf -- ptBuf 是待设置内存的起始位置;
                              wArrNum -- 数组长度;
                              pucVal--实际的值(以SNMP协议中的类型存储，如BYTE, WORD16 等均占sizeof(LONG) 个字节);
* 输出参数：
* 返 回 值：无
* 其它说明：因为在实际的编程中用到的类型为BYTE, WORD16等分别占1,2个字节。
                              SNMP中最小的长度为sizeof(long)，所以存在long 到BYTE,WORD16等的转换问题，
                              这里要考虑字节序。 
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2010/06/08    V1.0    刘华振        创建
**************************************************************************/
BOOLEAN SnmpSet_snmptype_Value(BYTE ftype, BYTE *ptBuf, WORD16 wArrNum, const BYTE * pucVal, size_t var_val_len)
{
    SNMP_FTYPE * ptTypeHandler = NULL;

    OamFormatLog((char *)"ptBuf is NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(ptBuf != NULL, FALSE);
    OamFormatLog((char *)"pucVal is NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(pucVal != NULL, FALSE);
    OamFormatLog((char *)"wArrNum = %u!\n", wArrNum);
    OAM_NASSERT_RETURN_VAL_IF_FAIL(wArrNum > 0, FALSE);

    ptTypeHandler = SnmpGetTypeHandler(ftype);
    OamFormatLog((char *)"ftype(%u), is not registered!\n", ftype);
    OAM_NASSERT_RETURN_VAL_IF_FAIL(ptTypeHandler != NULL, FALSE);

    if (ptTypeHandler->set_snmptype_value != NULL)
    {
        ptTypeHandler->set_snmptype_value(ptBuf, ptTypeHandler->ftypesize, wArrNum, pucVal, var_val_len);
        return TRUE;
    }
    OutPut(SYS_ERROR, " ftype(%u), is not registered(get_bufvalue = NULL)!\n", ftype);
    return FALSE;
}/* SnmpSet_snmptype_Value */


/**************************************************************************
* 函数名称：SnmpGet_ftype_value_Len
* 功能描述：获取内存中的长度
* 访问的表：无
* 修改的表：无
* 输入参数：ftype-- 要获取信息的类型
                              wArrNum -- 数组长度;
* 输出参数：
* 返 回 值：成功:实际长度;
                          失败: 0;
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2010/06/08    V1.0    刘华振        创建
**************************************************************************/
WORD16 SnmpGet_ftype_value_Len(BYTE ftype, WORD16 wArrNum)
{
    SNMP_FTYPE * ptTypeHandler = NULL;

    OamFormatLog((char *)"wArrNum = %u!\n", wArrNum);
    OAM_NASSERT_RETURN_VAL_IF_FAIL(wArrNum > 0, 0);

    ptTypeHandler = SnmpGetTypeHandler(ftype);
    OamFormatLog((char *)"ftype(%u), is not registered!\n", ftype);
    OAM_NASSERT_RETURN_VAL_IF_FAIL(ptTypeHandler != NULL, 0);

    return (ptTypeHandler->ftypesize * wArrNum);
}/* SnmpGet_ftype_value_Len */

/**************************************************************************
* 函数名称：SnmpSet_ftype_Value
* 功能描述：设置取值
* 访问的表：无
* 修改的表：无
* 输入参数：ftype-- 要设置信息的类型
                              wArrNum -- 数组长度;
                              cStr -- 对应的值；
* 输出参数：无
* 返 回 值：成功:TRUE;
                          失败:FALSE;
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2010/06/08    V1.0    刘华振        创建
**************************************************************************/
BOOLEAN SnmpSet_ftype_Value(BYTE ftype, T_SNMP_NodeValue *ptValue, const char *cStr)
{
    SNMP_FTYPE * ptTypeHandler = NULL;

    OamFormatLog((char *)"ptValue is NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(ptValue != NULL, FALSE);
    OamFormatLog((char *)"cStr is NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(cStr != NULL, FALSE);

    ptTypeHandler = SnmpGetTypeHandler(ftype);
    OamFormatLog((char *)"ftype(%u), is not registered!\n", ftype);
    OAM_NASSERT_RETURN_VAL_IF_FAIL(ptTypeHandler != NULL, FALSE);

    if (ptTypeHandler->set_ftype_value != NULL)
    {
        ptTypeHandler->set_ftype_value(ptValue, cStr);
        return TRUE;
    }
    OutPut(SYS_ERROR, " ftype(%u), is not registered(set_value = NULL)!\n", ftype);
    return FALSE;
}/* SnmpSet_ftype_Value */

/**************************************************************************
* 函数名称：SnmpGet_ftype_by_name
* 功能描述：获取某一个名称对应的类型
* 访问的表：无
* 修改的表：无
* 输入参数：cstr - - 名称
* 输出参数：无
* 返 回 值：成功:实际的取值;
                          失败:255;
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2010/06/08    V1.0    刘华振        创建
**************************************************************************/
BYTE SnmpGet_ftype_by_name(const char *cStr)
{
    WORD16 wIndex = 0;

    OamFormatLog((char *)"cStr is NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(cStr != NULL, DATA_TYPE_UNDEFINED);
    
    for (wIndex = DATA_TYPE_BEGIN; wIndex < DATA_TYPE_END; wIndex++)
    {
        if ((g_tSnmpSerialFtype[wIndex].ftypename != NULL) &&
            (strcmp(g_tSnmpSerialFtype[wIndex].ftypename, cStr) == 0))
        {
            return (BYTE)g_tSnmpSerialFtype[wIndex].ftype;
        }
    }
    for (wIndex = 0; wIndex < SNMP_NSERIALDATATYPE_NUM; wIndex++)
    {
        if ((g_tSnmpNSerialFtype[wIndex].ftypename != NULL) &&
            (strcmp(g_tSnmpNSerialFtype[wIndex].ftypename, cStr) == 0))
        {
            return (BYTE)g_tSnmpNSerialFtype[wIndex].ftype;
        }
    }
    OutPut(SYS_ERROR, " cStr(%s) is not registered!\n", cStr);
    return DATA_TYPE_UNDEFINED;
}/* SnmpGet_ftype_by_name */

/**************************************************************************
* 函数名称：SnmpGet_xmlrpcftype_by_name
* 功能描述：获取某一个名称对应的类型
* 访问的表：无
* 修改的表：无
* 输入参数：cstr - - 名称
* 输出参数：无
* 返 回 值：成功:实际的取值;
                          失败:255;
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2010/06/08    V1.0    刘华振        创建
**************************************************************************/
BYTE SnmpGet_xmlrpcftype_by_name(const char *cStr)
{
    WORD16 wIndex = 0;

    OamFormatLog((char *)"cStr is NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(cStr != NULL, DATA_TYPE_UNDEFINED);
    
    for (wIndex = DATA_TYPE_BEGIN; wIndex < DATA_TYPE_END; wIndex++)
    {
        if ((g_tSnmpSerialFtype[wIndex].ftypename != NULL) &&
            (strcmp(g_tSnmpSerialFtype[wIndex].ftypename, cStr) == 0))
        {
            return (BYTE)g_tSnmpSerialFtype[wIndex].xml_rpc_ftype;
        }
    }
    for (wIndex = 0; wIndex < SNMP_NSERIALDATATYPE_NUM; wIndex++)
    {
        if ((g_tSnmpNSerialFtype[wIndex].ftypename != NULL) &&
            (strcmp(g_tSnmpNSerialFtype[wIndex].ftypename, cStr) == 0))
        {
            return (BYTE)g_tSnmpNSerialFtype[wIndex].xml_rpc_ftype;
        }
    }
    OutPut(SYS_ERROR, " cStr(%s) is not registered!\n", cStr);
    return DATA_TYPE_UNDEFINED;
}/* SnmpGet_ftype_by_name */

/**************************************************************************
* 函数名称：SnmpGet_name_by_ftyper
* 功能描述：获取某一个类型对应的字符串描述
* 访问的表：无
* 修改的表：无
* 输入参数：ftype - - 类型
* 输出参数：无
* 返 回 值：成功:实际的名称;
                          失败:"NULL"
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2010/06/08    V1.0    刘华振        创建
**************************************************************************/
char *SnmpGet_name_by_ftype(BYTE ftype)
{
    SNMP_FTYPE * ptTypeHandler = NULL;

    ptTypeHandler = SnmpGetTypeHandler(ftype);
    OamFormatLog((char *)"ftype(%d), is not registered!\n", ftype);
    OAM_NASSERT_RETURN_VAL_IF_FAIL(ptTypeHandler != NULL, NULL);

    return ptTypeHandler->ftypename;
}/* SnmpGet_name_by_ftyper */

BYTE *Snmp_ftype_addvariable(BYTE ftype, BYTE *pDataBuf, T_Oid_Property  *pOidProp, netsnmp_variable_list *var_list)
{
    SNMP_FTYPE * ptTypeHandler = NULL;

    OamFormatLog((char *)"pDataBuf is NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(pDataBuf != NULL, pDataBuf);
    OamFormatLog((char *)"pOidProp is NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(pOidProp != NULL, pDataBuf);
    OamFormatLog((char *)"var_list is NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(var_list != NULL, pDataBuf);
    
    ptTypeHandler = SnmpGetTypeHandler(ftype);
    if (ptTypeHandler == NULL)
    {
        snmp_varlist_add_variable (&var_list,
                                               pOidProp->OidName, pOidProp->wOidLen,
                                               ASN_OCTET_STR, pDataBuf, pOidProp->wArrayLen*pOidProp->wParaLen);
        pDataBuf += pOidProp->wArrayLen * pOidProp->wParaLen;
        return pDataBuf;
    }
    
    if (pOidProp->wArrayLen == 1)
    {
        if (ptTypeHandler->ftype == DATA_TYPE_UNDEFINED)
        {
            snmp_varlist_add_variable (&var_list,
                                       pOidProp->OidName, pOidProp->wOidLen,
                                       ASN_OCTET_STR, pDataBuf, pOidProp->wParaLen);
            pDataBuf += pOidProp->wParaLen;
        }
        else
        {
            snmp_varlist_add_variable (&var_list,
                                       pOidProp->OidName, pOidProp->wOidLen,
                                       ptTypeHandler->snmptype, pDataBuf, ptTypeHandler->ftypesize);
            pDataBuf += ptTypeHandler->ftypesize;
        }
        return pDataBuf;

    }
    
    if (ptTypeHandler->ftype == DATA_TYPE_UNDEFINED)
    {
        snmp_varlist_add_variable (&var_list,
                                   pOidProp->OidName, pOidProp->wOidLen,
                                   ASN_OCTET_STR, pDataBuf, pOidProp->wParaLen);
        pDataBuf += pOidProp->wParaLen;
    }
    else
    {
        snmp_varlist_add_variable (&var_list,
                                   pOidProp->OidName, pOidProp->wOidLen,
                                   ASN_OCTET_STR, pDataBuf, pOidProp->wArrayLen * ptTypeHandler->ftypesize);
        pDataBuf += pOidProp->wArrayLen * ptTypeHandler->ftypesize;
    }
    return pDataBuf;
}


BYTE SnmpGet_snmptype_by_ftype(BYTE ftype, WORD16 wArrNum)
{
    SNMP_FTYPE * ptTypeHandler = NULL;

    if (wArrNum > 1)
    {
        return ASN_OCTET_STR;
    }
    
    ptTypeHandler = SnmpGetTypeHandler(ftype);
    OamFormatLog((char *)"ftype(%d), is not registered!\n", ftype);
    OAM_NASSERT_RETURN_VAL_IF_FAIL(ptTypeHandler != NULL, ASN_OCTET_STR);
    return ptTypeHandler->snmptype;
}

BYTE *SnmpGet_Oid_Value(BYTE ftype, BYTE *ptBuf, oid *name, WORD32 *oidLen, WORD16 *indexLen)
{
    SNMP_FTYPE * ptTypeHandler = NULL;

    OamFormatLog((char *)"ptBuf is NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(ptBuf != NULL, ptBuf);
    OamFormatLog((char *)"name is NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(name != NULL, ptBuf);
    OamFormatLog((char *)"oidLen is NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(oidLen != NULL, ptBuf);
    OamFormatLog((char *)"indexLen is NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(indexLen != NULL, ptBuf);

    ptTypeHandler = SnmpGetTypeHandler(ftype);
    OamFormatLog((char *)"ftype(%u), is not registered!\n", ftype);
    OAM_NASSERT_RETURN_VAL_IF_FAIL(ptTypeHandler != NULL, ptBuf);

    if (ptTypeHandler->get_ftype_oid_value != NULL)
    {
        return ptTypeHandler->get_ftype_oid_value(ptBuf, name, oidLen, indexLen);
    }
    OutPut(SYS_ERROR, " ftype(%u), is not registered(set_value = NULL)!\n", ftype);
    return ptBuf;
}/* SnmpGet_Oid_Value */

BYTE *SnmpSet_Oid_Value(BYTE ftype, BYTE *ptBuf, oid *name, WORD32 *oidLen, WORD16 *indexLen, WORD32 *dwIndexDeserveLen)
{
    SNMP_FTYPE * ptTypeHandler = NULL;
    OamFormatLog((char *)"ptBuf is NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(ptBuf != NULL, ptBuf);
    OamFormatLog((char *)"name is NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(name != NULL, ptBuf);
    OamFormatLog((char *)"oidLen is NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(oidLen != NULL, ptBuf);
    OamFormatLog((char *)"indexLen is NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(indexLen != NULL, ptBuf);
    
    ptTypeHandler = SnmpGetTypeHandler(ftype);
    OamFormatLog((char *)"ftype(%d), is not registered!\n", ftype);
    OAM_NASSERT_RETURN_VAL_IF_FAIL(ptTypeHandler != NULL, ptBuf);
    if (ptTypeHandler->set_ftype_oid_value != NULL)
    {
        return ptTypeHandler->set_ftype_oid_value(ptBuf, name, oidLen, indexLen, dwIndexDeserveLen);
    }
    return ptBuf;
}/* SnmpSet_Oid_Value */

WORD16 SnmpGet_Index_name_len(BYTE ftype)
{
    SNMP_FTYPE * ptTypeHandler = NULL;
    
    ptTypeHandler = SnmpGetTypeHandler(ftype);
    OamFormatLog((char *)"ftype(%d), is not registered!\n", ftype);
    OAM_NASSERT_RETURN_VAL_IF_FAIL(ptTypeHandler != NULL, 0);
    return ptTypeHandler->index_oidname_len;
}

void SnmpFormatString(BYTE ftype, T_SNMP_NodeValue *pNodeValue, char *pszBuffer, WORD32 dwMaxCount)
{
    SNMP_FTYPE * ptTypeHandler = NULL;

    OamFormatLog((char *)"pszBuffer can't be NULL!\n");
    OAM_NASSERT_RETURN_IF_FAIL(pszBuffer != NULL);
    OamFormatLog((char *)"dwMaxCount = %u!\n", dwMaxCount);
    OAM_NASSERT_RETURN_IF_FAIL(dwMaxCount > 10);
    
    ptTypeHandler = SnmpGetTypeHandler(ftype);
    if ((pNodeValue == NULL) || (ptTypeHandler == NULL) || (ptTypeHandler->format_string == NULL))
    {
        snprintf(pszBuffer, dwMaxCount, "%s", "NULL");
        return;
    }
    ptTypeHandler->format_string(pNodeValue, pszBuffer, dwMaxCount);
}

void OAM_DbgShowSnmpFtypes()
{
    WORD16 wIndex = 0;
    SNMP_FTYPE *snmp_ftype = NULL;
    printf("ftype, ftypesize, snmptype, oidlen, ftypename\n");
    for (wIndex=0; wIndex<DATA_TYPE_END; wIndex++)
    {
        snmp_ftype = &g_tSnmpSerialFtype[wIndex];
        printf("%d, %d, %d, %d, %s\n", snmp_ftype->ftype, snmp_ftype->ftypesize, snmp_ftype->snmptype, snmp_ftype->index_oidname_len, 
        snmp_ftype->ftypename);
    }
    for (wIndex = 0; wIndex < SNMP_NSERIALDATATYPE_NUM; wIndex++)
    {            
        snmp_ftype = &g_tSnmpNSerialFtype[wIndex];
        printf("%d, %d, %d, %d, %s\n", snmp_ftype->ftype, snmp_ftype->ftypesize, snmp_ftype->snmptype, snmp_ftype->index_oidname_len, 
        snmp_ftype->ftypename);
    }

}
