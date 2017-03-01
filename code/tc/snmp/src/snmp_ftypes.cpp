/*********************************************************************
* ��Ȩ���� (C)2010, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ� snmp_ftypes.c
* �ļ���ʶ��
* ����ժҪ�� snmp ֧�ֵĸ����������Ͷ�Ӧ�Ĵ�����
* ����˵���� 
* ��ǰ�汾�� V1.0
* ��    �ߣ� �����񡪡�3Gƽ̨
* ������ڣ�
*
* �޸ļ�¼1��
*    �޸����ڣ�20010��xx��xx��
*    �� �� �ţ�V1.0
*    �� �� �ˣ�������
*    �޸����ݣ�����
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
        long_ret = *((T*)ptBuf); /* ��Ҫ���Ǵ�Сβ��������ptBuf ��long ��ת����BYTE��ָ��*/\
        return (unsigned char *) &long_ret;\
    }\
    else\
    {\
        /* ������Ϊ�ַ������� */\
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
        tmp = *((long *)pucVal);/* ��Ҫ���Ǵ�Сβ��������ptBuf ��long ��ת����BYTE��ָ��*/\
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
/* ������ͨ�ģ����ܰ��ַ���������Ϊ�����������ڴ���
    ��ʱû�취֪���ַ�������OID ����ʵ���� */
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
       BYTE        ftype; /* �Զ������ͣ���BYTE, WORD, WORD32, T_LogicalAddr �� */
       BYTE        xml_rpc_ftype; /* �Զ������ͣ���BYTE, WORD, WORD32, T_LogicalAddr �� */
       BYTE        ftypesize; /* �����Զ���������ռ�ڴ��С����sizeof(BYTE), sizeof(T_LogicalAddr) */
       BYTE        snmptype; /* �Զ������Ͷ�Ӧ����snmpЭ���е����� */
       BYTE        index_oidname_len;
	char		       *ftypename; /* �Զ����������� */
       Get_ftype_oid_value_Func get_ftype_oid_value; /* ���Զ�������ȡֵ */
       Set_ftype_oid_value_Func set_ftype_oid_value;
       Set_ftype_value_Func set_ftype_value; /* ���Զ������͸�ֵ */
       Get_snmptype_value_Func       get_snmptype_value;	/* ��SNMP֧�ֵ����ʹ��ڴ���ȡֵ������������飬����ռsizeof(long)���ֽ�*/
       Set_snmptype_value_Func   set_snmptype_value; /* ��SNMP ֧�ֵ����Ͷ��ڴ渳ֵ������������飬����ռsizeof(long)���ֽ�*/
       Check_snmpftype_value_Func check_snmptype_value; /* ��һ��������ֵ�ǰ�snmp֧�ֵ����ʹ洢�� */
       Format_String_Func format_string;
}_PACKED_1_ SNMP_FTYPE;

/* ftype Ϊ��ɢ��ֵ */

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

/* ftype Ϊ������ֵ����Ϊ�������е�����������µ�����ʱҪע�⣬
    �������м���룬����������ģ�׷��������������ɢ�ģ�
    ����g_tSnmpNSerialFtype�У���������Ϊ�˼ӿ��ȡ�ٶȣ�ֱ�Ӱ�ftype
    ��Ϊ�����±��ȡ */
SNMP_FTYPE g_tSnmpSerialFtype[] =
{
    /* ����              ����          ��Ӧ��snmp����, ���� ����ȡֵ           ��ȡȡֵ       */
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
* �������ƣ�SnmpConvert_atoi
* ����������atoiת������
* ���ʵı���
* �޸ĵı���
* ���������pString -- �ַ���
* ���������pDword -- ת�����WORD32��ֵ
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/08/28    V1.0    ����        ����
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
* �������ƣ�SnmpSet_ftype_CHAR_Value
* ��������������CHAR �ַ�������
* ���ʵı���
* �޸ĵı���
* ���������cStr -- �����ݵ��ַ�����ʾ
* ���������ptValue -- ������ú������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2010/06/08    V1.0    ������        ����
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
* �������ƣ�SnmpSet_ftype_BYTE_Value
* ���������������ֽ�����
* ���ʵı���
* �޸ĵı���
* ���������cStr -- �����ݵ��ַ�����ʾ
* ���������ptValue -- ������ú������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2010/06/08    V1.0    ������        ����
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
* �������ƣ�SnmpSet_ftype_WORD16_Value
* ��������������WORD16 ����
* ���ʵı���
* �޸ĵı���
* ���������cStr -- �����ݵ��ַ�����ʾ
* ���������ptValue -- ������ú������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2010/06/08    V1.0    ������        ����
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
* �������ƣ�SnmpSet_ftype_SWORD16_Value
* ��������������SWORD16  ����
* ���ʵı���
* �޸ĵı���
* ���������cStr -- �����ݵ��ַ�����ʾ
* ���������ptValue -- ������ú������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2010/06/08    V1.0    ������        ����
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
* �������ƣ�SnmpSet_ftype_WORD32_Value
* ��������������WORD32  ����
* ���ʵı���
* �޸ĵı���
* ���������cStr -- �����ݵ��ַ�����ʾ
* ���������ptValue -- ������ú������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2010/06/08    V1.0    ������        ����
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
* �������ƣ�SnmpSet_ftype_UINT32_Value
* ��������������WORD32  ����
* ���ʵı���
* �޸ĵı���
* ���������cStr -- �����ݵ��ַ�����ʾ
* ���������ptValue -- ������ú������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2010/06/08    V1.0    ������        ����
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
* �������ƣ�SnmpSet_ftype_Comm_Value
* ��������������ͨ������
* ���ʵı���
* �޸ĵı���
* ���������cStr -- �����ݵ��ַ�����ʾ
* ���������ptValue -- ������ú������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2010/11/08    V1.0    ������        ����
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

    /* ����IPMI strlen(ptBuf) Ӧ�õ���*indexLen */
    /* ������ͨ�ģ����͵Ľṹ���ַ����Ƕ����ģ�
    strlen(ptBuf)С�����*indexLen, ��ptBuf���涼�ǲ�0����ġ�*/
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
* �������ƣ�SnmpGet_snmptype_BYTE_Value
* ������������ȡBYTE  ����
* ���ʵı���
* �޸ĵı���
* ���������ptBuf -- ������ݵ�Buf
                              ftypesize -- ��������
                              wArrNum -- �������
* ���������var_len -- �����ݵ��ַ�����ʾ
* �� �� ֵ������ָ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2010/06/08    V1.0    ������        ����
**************************************************************************/
static BYTE *SnmpGet_snmptype_BYTE_Value(const BYTE *ptBuf, WORD32 ftypesize, WORD16 wArrNum, size_t *var_len)
{
    SNMP_GET_SNMPFTYPE_T_VALUE(BYTE);
}/* SnmpGet_snmptype_BYTE_Value */

/**************************************************************************
* �������ƣ�SnmpGet_snmptype_CHAR_Value
* ������������ȡchar  ����
* ���ʵı���
* �޸ĵı���
* ���������ptBuf -- ������ݵ�Buf
                              ftypesize -- ��������
                              wArrNum -- �������
* ���������var_len -- �����ݵ��ַ�����ʾ
* �� �� ֵ������ָ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2010/08/08    V1.0    ������        ����
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
* �������ƣ�SnmpGet_snmptype_WORD16_Value
* ������������ȡWORD16 ����
* ���ʵı���
* �޸ĵı���
* ���������ptBuf -- ������ݵ�Buf
                              ftypesize -- ��������
                              wArrNum -- �������
* ���������var_len -- �����ݵ��ַ�����ʾ
* �� �� ֵ������ָ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2010/06/08    V1.0    ������        ����
**************************************************************************/
static BYTE *SnmpGet_snmptype_WORD16_Value(const BYTE *ptBuf, WORD32 ftypesize, WORD16 wArrNum, size_t *var_len)
{
    SNMP_GET_SNMPFTYPE_T_VALUE(WORD16);
}/* SnmpGet_snmptype_WORD16_Value */

/**************************************************************************
* �������ƣ�SnmpGet_snmptype_SWORD16_Value
* ������������ȡSWORD16 ����
* ���ʵı���
* �޸ĵı���
* ���������ptBuf -- ������ݵ�Buf
                              ftypesize -- ��������
                              wArrNum -- �������
* ���������var_len -- �����ݵ��ַ�����ʾ
* �� �� ֵ������ָ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2010/09/15    V1.0    ������        ����
**************************************************************************/
static BYTE *SnmpGet_snmptype_SWORD16_Value(const BYTE *ptBuf, WORD32 ftypesize, WORD16 wArrNum, size_t *var_len)
{
    SNMP_GET_SNMPFTYPE_T_VALUE(SWORD16);
}/* SnmpGet_snmptype_SWORD16_Value */


/**************************************************************************
* �������ƣ�SnmpGet_snmptype_IPADDR_Value
* ������������ȡIP ����
* ���ʵı���
* �޸ĵı���
* ���������ptBuf -- ������ݵ�Buf
                              ftypesize -- ��������
                              wArrNum -- �������
* ���������var_len -- �����ݵ��ַ�����ʾ
* �� �� ֵ������ָ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2010/10/29    V1.0    ������        ����
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
* �������ƣ�SnmpGet_snmptype_WORD32_Value
* ������������ȡWORD32 ����
* ���ʵı���
* �޸ĵı���
* ���������ptBuf -- ������ݵ�Buf
                              ftypesize -- ��������
                              wArrNum -- �������
* ���������var_len -- �����ݵ��ַ�����ʾ
* �� �� ֵ������ָ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2010/06/08    V1.0    ������        ����
**************************************************************************/
static BYTE *SnmpGet_snmptype_WORD32_Value(const BYTE *ptBuf, WORD32 ftypesize, WORD16 wArrNum, size_t *var_len)
{
    SNMP_GET_SNMPFTYPE_T_VALUE(WORD32);
}/* SnmpGet_snmptype_WORD32_Value */

/**************************************************************************
* �������ƣ�SnmpGet_snmptype_UINT32_Value
* ������������ȡWORD32 ����
* ���ʵı���
* �޸ĵı���
* ���������ptBuf -- ������ݵ�Buf
                              ftypesize -- ��������
                              wArrNum -- �������
* ���������var_len -- �����ݵ��ַ�����ʾ
* �� �� ֵ������ָ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2010/06/08    V1.0    ������        ����
**************************************************************************/
static BYTE *SnmpGet_snmptype_UINT32_Value(const BYTE *ptBuf, WORD32 ftypesize, WORD16 wArrNum, size_t *var_len)
{
    SNMP_GET_SNMPFTYPE_T_VALUE(uint32);
}/* SnmpGet_snmptype_WORD32_Value */

/**************************************************************************
* �������ƣ�SnmpGet_snmptype_Comm_Value
* ������������ȡͨ������
* ���ʵı���
* �޸ĵı���
* ���������ptBuf -- ������ݵ�Buf
                              ftypesize -- ��������
                              wArrNum -- �������
* ���������var_len -- �����ݵ��ַ�����ʾ
* �� �� ֵ������ָ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2010/06/08    V1.0    ������        ����
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
* �������ƣ�SnmpSet_snmptype_BYTE_Value
* ��������������BYTE  ����
* ���ʵı���
* �޸ĵı���
* ���������ptBuf -- ������ݵ�Buf
                              ftypesize -- ��������
                              wArrNum -- �������
                              pucVal--����
* ���������
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2010/06/08    V1.0    ������        ����
**************************************************************************/
static void SnmpSet_snmptype_BYTE_Value(BYTE *ptBuf, WORD32 ftypesize, WORD16 wArrNum, const BYTE * pucVal, size_t var_val_len)
{
    SNMP_SET_SNMPTYPE_T_VALUE(BYTE);
}/* SnmpSet_snmptype_BYTE_Value */

/**************************************************************************
* �������ƣ�SnmpSet_snmptype_WORD16_Value
* ��������������WORD16 ����
* ���ʵı���
* �޸ĵı���
* ���������ptBuf -- ������ݵ�Buf
                              dwBaseSize -- ��������
                              wArrNum -- �������
                              pucVal--����
* ���������
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2010/06/08    V1.0    ������        ����
**************************************************************************/
static void SnmpSet_snmptype_WORD16_Value(BYTE *ptBuf, WORD32 ftypesize, WORD16 wArrNum, const BYTE * pucVal, size_t var_val_len)
{
    SNMP_SET_SNMPTYPE_T_VALUE(WORD16);
}/* SnmpSet_snmptype_WORD16_Value */

/**************************************************************************
* �������ƣ�SnmpSet_snmptype_SWORD16_Value
* ��������������SWORD16 ����
* ���ʵı���
* �޸ĵı���
* ���������ptBuf -- ������ݵ�Buf
                              dwBaseSize -- ��������
                              wArrNum -- �������
                              pucVal--����
* ���������
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2010/09/15   V1.0    ������        ����
**************************************************************************/
static void SnmpSet_snmptype_SWORD16_Value(BYTE *ptBuf, WORD32 ftypesize, WORD16 wArrNum, const BYTE * pucVal, size_t var_val_len)
{
    SNMP_SET_SNMPTYPE_T_VALUE(SWORD16);
}/* SnmpSet_snmptype_SWORD16_Value */


/**************************************************************************
* �������ƣ�SnmpSet_snmptype_IPADDR_Value
* ��������������IP����
* ���ʵı���
* �޸ĵı���
* ���������ptBuf -- ������ݵ�Buf
                              dwBaseSize -- ��������
                              wArrNum -- �������
                              pucVal--����
* ���������
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2010/10/28    V1.0    ������        ����
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
* �������ƣ�SnmpSet_snmptype_WORD32_Value
* ��������������WORD32����
* ���ʵı���
* �޸ĵı���
* ���������ptBuf -- ������ݵ�Buf
                              dwBaseSize -- ��������
                              wArrNum -- �������
                              pucVal--����
* ���������
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2010/06/08    V1.0    ������        ����
**************************************************************************/
static void SnmpSet_snmptype_WORD32_Value(BYTE *ptBuf, WORD32 ftypesize, WORD16 wArrNum, const BYTE * pucVal, size_t var_val_len)
{
    SNMP_SET_SNMPTYPE_T_VALUE(WORD32);
}/* SnmpSet_snmptype_WORD32_Value */

/**************************************************************************
* �������ƣ�SnmpSet_snmptype_UINT32_Value
* ��������������uint32����
* ���ʵı���
* �޸ĵı���
* ���������ptBuf -- ������ݵ�Buf
                              dwBaseSize -- ��������
                              wArrNum -- �������
                              pucVal--����
* ���������
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2010/06/08    V1.0    ������        ����
**************************************************************************/
static void SnmpSet_snmptype_UINT32_Value(BYTE *ptBuf, WORD32 ftypesize, WORD16 wArrNum, const BYTE * pucVal, size_t var_val_len)
{
    SNMP_SET_SNMPTYPE_T_VALUE(uint32);
}/* SnmpSet_snmptype_WORD32_Value */

/**************************************************************************
* �������ƣ�SnmpSet_snmptype_CHAR_Value
* ��������������char ����
* ���ʵı���
* �޸ĵı���
* ���������ptBuf -- ������ݵ�Buf
                              dwBaseSize -- ��������
                              wArrNum -- �������
                              pucVal--����
* ���������
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2010/06/08    V1.0    ������        ����
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
* �������ƣ�SnmpSet_snmptype_Comm_Value
* ��������������ͨ������
* ���ʵı���
* �޸ĵı���
* ���������ptBuf -- ������ݵ�Buf
                              dwBaseSize -- ��������
                              wArrNum -- �������
                              pucVal--����
* ���������
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2010/06/08    V1.0    ������        ����
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
* �������ƣ�SnmpCheck_snmptype_BYTE_Value
* ����������Set ����ʱУ��BYTE ֵ�����Ƿ���Ч
* ���ʵı���
* �޸ĵı���
* ���������var_val-- ��У���ֵ
                              var_val_type --ֵ(snmp)����
                              var_val_len -- ֵ����;
                              snmptype -- snmp type;
                              pOidProp -- oid�ڵ�������
* �����������
* �� �� ֵ���ɹ�������·���SNMP_ERR_NOERROR;
* ����˵����var_val �е�ֵ��СΪsizeof(long);
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2010/08/27    V1.0    ������        ����
**************************************************************************/
static int SnmpCheck_snmptype_BYTE_Value(u_char *var_val,u_char var_val_type,size_t var_val_len, BYTE snmptype, T_Oid_Property *pOidProp)
{    
    SNMP_VALUE_CHECK(ucByte, BYTE);
    return SNMP_ERR_NOERROR;
}/* SnmpCheck_snmptype_BYTE_Value */

/**************************************************************************
* �������ƣ�SnmpCheck_snmptype_CHAR_Value
* ����������Set ����ʱУ��CHAR ֵ�����Ƿ���Ч
* ���ʵı���
* �޸ĵı���
* ���������var_val-- ��У���ֵ
                              var_val_type --ֵ(snmp)����
                              var_val_len -- ֵ����;
                              snmptype -- snmp type;
                              pOidProp -- oid�ڵ�������
* �����������
* �� �� ֵ���ɹ�������·���SNMP_ERR_NOERROR;
* ����˵����var_val �е�ֵ��СΪsizeof(long);
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2010/09/15    V1.0    ������        ����
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
* �������ƣ�SnmpCheck_snmptype_WORD16_Value
* ����������Set ����ʱУ��WORD16 ֵ�����Ƿ���Ч
* ���ʵı���
* �޸ĵı���
* ���������var_val-- ��У���ֵ
                              var_val_type --ֵ(snmp)����
                              var_val_len -- ֵ����;
                              snmptype -- snmp type;
                              pOidProp -- oid�ڵ�������
* �����������
* �� �� ֵ���ɹ�������·���SNMP_ERR_NOERROR;
* ����˵����var_val �е�ֵ��СΪsizeof(long);
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2010/08/27    V1.0    ������        ����
**************************************************************************/
static int SnmpCheck_snmptype_WORD16_Value(u_char *var_val,u_char var_val_type,size_t var_val_len, BYTE snmptype,  T_Oid_Property *pOidProp)
{
    SNMP_VALUE_CHECK(wWord, WORD16);
    return SNMP_ERR_NOERROR;
}/* SnmpCheck_snmptype_WORD16_Value */

/**************************************************************************
* �������ƣ�SnmpCheck_snmptype_SWORD16_Value
* ����������Set ����ʱУ��SWORD16 ֵ�����Ƿ���Ч
* ���ʵı���
* �޸ĵı���
* ���������var_val-- ��У���ֵ
                              var_val_type --ֵ(snmp)����
                              var_val_len -- ֵ����;
                              snmptype -- snmp type;
                              pOidProp -- oid�ڵ�������
* �����������
* �� �� ֵ���ɹ�������·���SNMP_ERR_NOERROR;
* ����˵����var_val �е�ֵ��СΪsizeof(long);
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2010/09/15    V1.0    ������        ����
**************************************************************************/
static int SnmpCheck_snmptype_SWORD16_Value(u_char *var_val,u_char var_val_type,size_t var_val_len, BYTE snmptype,  T_Oid_Property *pOidProp)
{
    SNMP_VALUE_CHECK(sWord, SWORD16);
    return SNMP_ERR_NOERROR;
}/* SnmpCheck_snmptype_SWORD16_Value */


/**************************************************************************
* �������ƣ�SnmpCheck_snmptype_WORD32_Value
* ����������Set ����ʱУ��WORD32 ֵ�����Ƿ���Ч
* ���ʵı���
* �޸ĵı���
* ���������var_val-- ��У���ֵ
                              var_val_type --ֵ(snmp)����
                              var_val_len -- ֵ����;
                              snmptype -- snmp type;
                              pOidProp -- oid�ڵ�������
* �����������
* �� �� ֵ���ɹ�������·���SNMP_ERR_NOERROR;
* ����˵����var_val �е�ֵ��СΪsizeof(long);
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2010/08/27    V1.0    ������        ����
**************************************************************************/
static int SnmpCheck_snmptype_WORD32_Value(u_char *var_val,u_char var_val_type,size_t var_val_len, BYTE snmptype, T_Oid_Property *pOidProp)
{
    SNMP_VALUE_CHECK(dwWord, WORD32);
    return SNMP_ERR_NOERROR;
}/* SnmpCheck_snmptype_WORD32_Value */

/**************************************************************************
* �������ƣ�SnmpCheck_snmptype_Common_Value
* ����������Set ����ʱУ�� ͨ�� ֵ�����Ƿ���Ч
* ���ʵı���
* �޸ĵı���
* ���������var_val-- ��У���ֵ
                              var_val_type --ֵ(snmp)����
                              var_val_len -- ֵ����;
                              snmptype -- snmp type;
                              pOidProp -- oid�ڵ�������
* �����������
* �� �� ֵ���ɹ�������·���SNMP_ERR_NOERROR;
* ����˵����var_val �е�ֵ��СΪsizeof(long);
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2010/08/27    V1.0    ������        ����
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
* �������ƣ�SnmpGetTypeHandler
* ������������ȡĳһ�������͵���Ϣ
* ���ʵı���
* �޸ĵı���
* ���������dwParaType-- Ҫ��ȡ��Ϣ������
* �����������
* �� �� ֵ�������͵Ĵ�������NULL
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2010/06/08    V1.0    ������        ����
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
* �������ƣ�SnmpBufMove
* ����������ʹ����ƫ��
* ���ʵı���
* �޸ĵı���
* ���������dwParaType-- Ҫ��ȡ��Ϣ������
                              ptBuf -- ������ʼ��ַ;
                              wArrNum -- ���鳤��
* �����������
* �� �� ֵ���ɹ�������·���ƫ�ƺ�ĵ�ַ������
                          ����ԭ���ĵ�ַ;
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2010/06/08    V1.0    ������        ����
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
* �������ƣ�SnmpGet_snmptype_Value
* ������������ȡ ptBuf �е�ֵ
* ���ʵı���
* �޸ĵı���
* ���������ftype-- Ҫ��ȡ��Ϣ������(���BYTE, WORD16��)
                              ptBuf -- ������ʼ��ַ;
                              wArrNum -- ���鳤��;
* ���������var_len -- ���ݳ���;
* �� �� ֵ���ɹ�������·��ػ�ȡ�������ݣ����򷵻�NULL��
* ����˵������Ϊ��ʵ�ʵı�����õ�������ΪBYTE, WORD16�ȷֱ�ռ1,2���ֽڡ�
                              SNMP����С�ĳ���Ϊsizeof(long)�����Դ���BYTE,WORD16�ȵ�long ��ת�����⣬
                              ���ڴ���ʵ�ʳ���С��sizeof(long)ʱ�����ܼ򵥵ط����ڴ��ַ��
                              ���ﻹҪ�����ֽ��� 
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2010/06/08    V1.0    ������        ����
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
* �������ƣ�SnmpCheck_snmptype_value
* ����������Set ����ʱУ������Ƿ���Ч
* ���ʵı���
* �޸ĵı���
* ���������var_val-- ��У���ֵ
                              var_val_type --ֵ(snmp)����
                              var_val_len -- ֵ����;
                              pOidProp -- oid�ڵ�������
* �����������
* �� �� ֵ���ɹ�������·���SNMP_ERR_NOERROR;
* ����˵����var_val �е�ֵ��СΪsizeof(long);
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2010/08/27    V1.0    ������        ����
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
* �������ƣ�SnmpSet_snmptype_Value
* ������������pucVal �е�ֵ���õ�ptBuf�С�
* ���ʵı���
* �޸ĵı���
* ���������ftype-- ҪҪ���õ���Ϣ������(��BYTE, WORD16��)
                              ptBuf -- ptBuf �Ǵ������ڴ����ʼλ��;
                              wArrNum -- ���鳤��;
                              pucVal--ʵ�ʵ�ֵ(��SNMPЭ���е����ʹ洢����BYTE, WORD16 �Ⱦ�ռsizeof(LONG) ���ֽ�);
* ���������
* �� �� ֵ����
* ����˵������Ϊ��ʵ�ʵı�����õ�������ΪBYTE, WORD16�ȷֱ�ռ1,2���ֽڡ�
                              SNMP����С�ĳ���Ϊsizeof(long)�����Դ���long ��BYTE,WORD16�ȵ�ת�����⣬
                              ����Ҫ�����ֽ��� 
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2010/06/08    V1.0    ������        ����
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
* �������ƣ�SnmpGet_ftype_value_Len
* ������������ȡ�ڴ��еĳ���
* ���ʵı���
* �޸ĵı���
* ���������ftype-- Ҫ��ȡ��Ϣ������
                              wArrNum -- ���鳤��;
* ���������
* �� �� ֵ���ɹ�:ʵ�ʳ���;
                          ʧ��: 0;
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2010/06/08    V1.0    ������        ����
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
* �������ƣ�SnmpSet_ftype_Value
* ��������������ȡֵ
* ���ʵı���
* �޸ĵı���
* ���������ftype-- Ҫ������Ϣ������
                              wArrNum -- ���鳤��;
                              cStr -- ��Ӧ��ֵ��
* �����������
* �� �� ֵ���ɹ�:TRUE;
                          ʧ��:FALSE;
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2010/06/08    V1.0    ������        ����
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
* �������ƣ�SnmpGet_ftype_by_name
* ������������ȡĳһ�����ƶ�Ӧ������
* ���ʵı���
* �޸ĵı���
* ���������cstr - - ����
* �����������
* �� �� ֵ���ɹ�:ʵ�ʵ�ȡֵ;
                          ʧ��:255;
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2010/06/08    V1.0    ������        ����
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
* �������ƣ�SnmpGet_xmlrpcftype_by_name
* ������������ȡĳһ�����ƶ�Ӧ������
* ���ʵı���
* �޸ĵı���
* ���������cstr - - ����
* �����������
* �� �� ֵ���ɹ�:ʵ�ʵ�ȡֵ;
                          ʧ��:255;
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2010/06/08    V1.0    ������        ����
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
* �������ƣ�SnmpGet_name_by_ftyper
* ������������ȡĳһ�����Ͷ�Ӧ���ַ�������
* ���ʵı���
* �޸ĵı���
* ���������ftype - - ����
* �����������
* �� �� ֵ���ɹ�:ʵ�ʵ�����;
                          ʧ��:"NULL"
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2010/06/08    V1.0    ������        ����
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
