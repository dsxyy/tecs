/*
case MODE_GET:
case MODE_GETNEXT:
case MODE_GETBULK:
case MODE_SET_RESERVE1: 
    netsnmp_cache_check_and_reload();
    break;


ret = netsnmp_call_next_handler(handler, reginfo, reqinfo, requests);

case MODE_SET_FREE:
    _cache_free(cache);            
    break;
case MODE_SET_COMMIT:
   ...
   _cache_free(cache);
   break;

*/
#include "snmp_oam_mibhandler.h"
#include "snmp_ftypes.h"
#include "snmp_scalar_handler.h"
#include "snmp_table_handler.h"
#include "oam_pub_type.h"
#include "log_agent.h"
using namespace zte_tecs;

extern T_SNMPProc_GlobVal *g_ptSnmpdPri;

/* 非bsp 调用*/
int SnmpProtocolSetValue(netsnmp_variable_list *requestvb, T_Oid_Property *pOidProp, BYTE *pDataBuf)
{    
    OAM_RETURN_VAL_IF_FAIL(pOidProp, SNMP_ERR_INCONSISTENTVALUE);
    OAM_RETURN_VAL_IF_FAIL(pDataBuf, SNMP_ERR_INCONSISTENTVALUE);
    if (!SnmpSet_snmptype_Value(pOidProp->ucParaType, pDataBuf, pOidProp->wArrayLen, requestvb->val.string, requestvb->val_len))
    {
        memcpy(pDataBuf, requestvb->val.string, requestvb->val_len);
    }
    return SNMP_ERR_NOERROR;
}/* SnmpProtocolSetValue */

T_Oid_Property *SnmpProtocolGetPropertyOffSet(oid *name,WORD name_len, 
    T_Oid_Description *ptOidDesc, BYTE *ptEntry, BYTE **ptDataBuf)
{
    WORD32 property_offset = 0;
    T_Oid_Property *pOidProp = NULL;
    oid   newname[MAX_OID_NUMBER]= {0};
    oid   oidlen = 0;

    pOidProp = ptOidDesc->pOidHead;
    while (pOidProp != NULL)
    {
        memcpy(newname, pOidProp->OidName, pOidProp->wOidLen*sizeof(oid));
        oidlen = pOidProp->wOidLen;
        if (ptOidDesc->ucOidType == OID_PROPERTY_SCALAR)
        {
            newname[oidlen++] = 0; /* 对于标量，name 中的是1.2.3.4.0; OidName中的是1.2.3.4 */
        }      
        if (0 == snmp_oid_compare(name, oidlen, (const oid*)newname, oidlen))
        {
            if (ptEntry != NULL)
            {
                (*ptDataBuf) = ptEntry + property_offset;
            }
            return pOidProp;
        }
        property_offset += SnmpGet_ftype_value_Len(pOidProp->ucParaType, pOidProp->wArrayLen);
        pOidProp = (T_Oid_Property *)pOidProp->pNext;
    }
    SNMP_PRINT_OID("SnmpProtocolGetPropertyOffSet ", name, name_len);
    return NULL;
}/* SnmpProtocolGetPropertyOffSet */

/************************************************************
* 函数名称： SnmpProtocolUnRegister()
* 功能描述： SNMP的OID节点注销函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： T_Module_Snmp_Description *pModuleDescription: 模块SNMP描述信息
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2010/07         V1.0       曹亮         创建
***************************************************************/ 
void SnmpProtocolUnRegister(T_Module_Snmp_Description *pModuleDescription)
{
    WORD16  wLoop = 0;
    T_Oid_Description *ptOidDesc = NULL;
    int  iRet = 0;
    
    OAM_RETURN_IF_FAIL(pModuleDescription);
    
    for (wLoop=0;wLoop<pModuleDescription->wOidNum;wLoop++)
    {
        ptOidDesc = &pModuleDescription->tOidDesCrip[wLoop]; 
        if ((ptOidDesc->ucOidType == OID_PROPERTY_SCALAR)
            && (ptOidDesc->ucAckType != OID_PROPERTY_RESPONSE))
        {
            continue;
        }
        SNMP_PRINT_OID("Unregister ", ptOidDesc->BaseOidName, ptOidDesc->wBaseOidLen);
        if ((iRet = unregister_mib(ptOidDesc->BaseOidName, ptOidDesc->wBaseOidLen)) != MIB_UNREGISTERED_OK)
        {
            OutPut(SYS_DEBUG,"unregister_mib scalar error! iRet = %u\n", iRet);
        }
    }
    OutPut(SYS_DEBUG, "Unregister Success!\n");
}

/************************************************************
* 函数名称： SnmpProtocolRegister()
* 功能描述： SNMP的OID节点注册函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： T_Module_Snmp_Description *pModuleDescription: 模块SNMP描述信息
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2009/08         V1.0       曹亮         创建
***************************************************************/ 
void SnmpProtocolRegister(T_Module_Snmp_Description *pModuleDescription)
{
    WORD16  wLoop = 0;
    T_Oid_Description *ptOitDesc = NULL;

    OamFormatLog((char *)"pModuleDescription can't be NULL!\n");
    OAM_RETURN_IF_FAIL(pModuleDescription);

    for (wLoop=0; wLoop<pModuleDescription->wOidNum; wLoop++)
    {
        OamFormatLog((char *)"exceed max oid num(wLoop = %u, max = %u)!\n", wLoop, MAX_OID_NUMBER);
        OAM_NASSERT_BREAK_IF_FAIL(wLoop < MAX_OID_NUMBER);

        ptOitDesc = &pModuleDescription->tOidDesCrip[wLoop];
        if (ptOitDesc->ucOidType == OID_PROPERTY_SCALAR)
        {
            if (ptOitDesc->ucAckType == OID_PROPERTY_RESPONSE)
            {
                SnmpProtocolScalarRegister(ptOitDesc);
            }
        }
        else
        {
            SnmpProtocolTableRegister(ptOitDesc);
        }
    }
}

