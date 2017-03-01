/*********************************************************************
* 版权所有 (C)2011, 深圳市中兴通讯股份有限公司。
*
* 文件名称： snmp_table_handler.c
* 文件标识：
* 内容摘要：
* 其它说明： SNMP 批量设置/获取优化功能
* 当前版本： V1.0
* 作    者： 刘华振——CSP 平台
* 完成日期：
*
* 修改记录1：
*    修改日期：2011年xx月xx日
*    版 本 号：V1.0
*    修 改 人：刘华振
*    修改内容：创建
**********************************************************************/
#include "snmp_scalar_handler.h"
#include "snmp_table_handler.h"
#include "snmp_oam_mibhandler.h"
#include "snmp_ftypes.h"
#include "oam_pub_type.h"
#include "log_agent.h"
#include "snmp_interface.h"
using namespace zte_tecs;

/* table 优化:
1、ipmi 无法优化:
2、其他:

处错时要调用netsnmp_set_request_error 设置明确的错误码。
get:
handle_pdu-->handle_var_requests-->netsnmp_call_handlers-->netsnmp_call_handler-->
cache_handler-->bulk_to_next(flags设置了MIB_HANDLER_AUTO_NEXT)-->table-->table_iterator-->serialize-->SnmpProtocolTable_handler
set:
handle_pdu-->handle_set_loop(asp->mode不等于成功或失败时不停循环)-
    -->handle_set(不断改变asp->mode)
    --handle_var_requests(根据asp->treecache_num for循环调用netsnmp_call_handlers)
    -->netsnmp_call_handler(根据next_handler->next 是否为空for循环调用
                                         flags 没有MIB_HANDLER_AUTO_NEXT 标志则跳出循环；
                                         有MIB_HANDLER_AUTO_NEXT_OVERRIDE_ONCE 也跳出循环)
    -->cache_handler-->bulk_to_next(flags设置了MIB_HANDLER_AUTO_NEXT)-->table-->table_iterator-->serialize-->SnmpProtocolTable_handler

关于MIB_HANDLER_AUTO_NEXT 和MIB_HANDLER_AUTO_NEXT_OVERRIDE_ONCE标志:
    如果没有设置MIB_HANDLER_AUTO_NEXT，则需要自己手动调用netsnmp_call_next_handler;
    如果设置了MIB_HANDLER_AUTO_NEXT，则不需要手动调用netsnmp_call_next_handler，如果手动
    调用了netsnmp_call_next_handler，则需要把flag置为MIB_HANDLER_AUTO_NEXT_OVERRIDE_ONCE，见bulk_to_next;
*/
/*
ipmi:
    scalar:pOidDescrip->pSendStructHead存放的是实际的值;
    table:pOidDescrip->pSendStructHead存放的是索引；
*/
using namespace std;
using namespace xmlrpc_c;

#define ZTE_TABLE_STATUS_CACHE_TIMEOUT	5

extern char    g_sSession[MAX_SESSION_LEN];

#define OID_TRAP_SET(T) ((T) == OID_PROPERTY_TRAP)

#define LST_ADD_NODE(OIDDESC, STRUCTNODE)    \
    if (OIDDESC->pSendStructHead == NULL)\
    {\
        OIDDESC->pSendStructHead = STRUCTNODE;\
        OIDDESC->pSendStructTail = STRUCTNODE;\
    }\
    else\
    {\
        OIDDESC->pSendStructTail->pNext = STRUCTNODE;\
        OIDDESC->pSendStructTail = STRUCTNODE;\
    }

extern T_SNMPProc_GlobVal *g_ptSnmpdPri;  /* SNMP线程指向Snmp_Protocol的私有数据区的全局变量指针 */ 
static netsnmp_variable_list * SnmpProtocolTable_first_entry(void **loop_context,
                     void **data_context,
                     netsnmp_variable_list *index,
                     netsnmp_iterator_info *data);
netsnmp_variable_list *SnmpProtocolTable_next_entry( void **loop_context,
                     void **data_context,
                     netsnmp_variable_list *index,
                     netsnmp_iterator_info *data);
static int SnmpProtocolTable_handler(netsnmp_mib_handler *handler, netsnmp_handler_registration *reginfo,
    netsnmp_agent_request_info   *reqinfo, netsnmp_request_info *requests);
static int SnmpProtocolTableLoad(netsnmp_cache *cache, void *vmagic);
static int SnmpProtocolTableLoadFromDdm(T_Oid_Description *pOidDescription, int mode);
static void SnmpProtocolTableFree(netsnmp_cache *cache, void *vmagic);
static int SnmpProtocolTableCommit(netsnmp_cache *cache, void *vmagic);
static int SnmpProtocolTableCommitToDdm(netsnmp_request_info *requests, T_Oid_Description *pOidDescrip);
static int SnmpProtocolTableCheckValue(netsnmp_request_info *requests, T_Oid_Property *pOidProp, T_Send_Struct *table_entry);
static int SnmpProtocolTableGetValue(WORD32 dwFuncID, netsnmp_request_info *requests, T_Oid_Property *pOidProp, BYTE *pDataBuf, size_t *ipmi_var_len);
static int SnmpProtocolTableCreate(netsnmp_request_info *requests, T_Oid_Description *pOidDescrip);
static int SnmpProtocolTableCommitRespToDdm(netsnmp_variable_list *requestvb, T_Oid_Description *pOidDescrip, 
    T_Send_Struct *table_entry);
static int SnmpProtocolTableCommitTrapToDdm(T_Oid_Description *pOidDescrip, BYTE *pDataBuf);

static netsnmp_variable_list * SnmpProtocolTable_first_entry(void **loop_context,
                     void **data_context,
                     netsnmp_variable_list *index,
                     netsnmp_iterator_info *data)
{
    T_Oid_Description    *pOidDesc = (T_Oid_Description *)data->myvoid;
    *loop_context = pOidDesc->pSendStructHead;
    return SnmpProtocolTable_next_entry(loop_context, data_context, index, data);
}/* SnmpProtocolTable_first_entry */

netsnmp_variable_list *SnmpProtocolTable_next_entry( void **loop_context,
                     void **data_context,
                     netsnmp_variable_list *index,
                     netsnmp_iterator_info *data)
{
    WORD32 property_offset = 0;
    T_Oid_Property *pOidProp = NULL;
    T_Oid_Description    *pOidDesc = (T_Oid_Description *)data->myvoid;
    T_Send_Struct *entry =(T_Send_Struct *) *loop_context;
    BYTE *ptDataBuf = NULL;
    netsnmp_variable_list *vptr = index;

    if (entry != NULL) 
    {
        ptDataBuf = entry->pStruct;
        pOidProp = pOidDesc->pOidHead;
            
        while ((pOidProp != NULL) && (vptr != NULL))
        {
            property_offset = SnmpGet_ftype_value_Len(pOidProp->ucParaType, pOidProp->wArrayLen);
            if (pOidProp->ucIndexProp == OID_PROPERTY_INDEX)
            {
                snmp_set_var_value(vptr, ptDataBuf, property_offset);
                ptDataBuf += property_offset;
                pOidProp = (T_Oid_Property *)pOidProp->pNext;
                vptr = vptr->next_variable;

            }
            
        }
        *data_context = (void *) entry;
        *loop_context = (void *) entry->pNext;
        return index;
    } 
    return NULL;
}/* SnmpProtocolTable_next_entry */

static int SnmpProtocolTableCreate(netsnmp_request_info *requests, T_Oid_Description *pOidDescrip)
{
    netsnmp_variable_list *requestvb = requests->requestvb;
    BYTE ucLoop = 0;
    WORD32 dwIndexDeserveLen = 0;
    T_Send_Struct *pTempSendStruct = NULL;
    T_Oid_Property    *pOidProp = NULL;
    BYTE *pSendStruct = NULL;
    WORD32 index = 0;
    WORD16 wIndexLen = 0;
    oid *index_name_start = NULL;
    
    pTempSendStruct = (T_Send_Struct *)malloc(sizeof(T_Send_Struct));
    if (pTempSendStruct == NULL)
    {
        return SNMP_ERR_GENERR;
    }
    memset(pTempSendStruct,0,sizeof(T_Send_Struct));
    pTempSendStruct->pStruct = (BYTE *)malloc(pOidDescrip->dwStructLength);
    if (pTempSendStruct->pStruct == NULL)
    {
        SNMP_SAFE_RETUB(pTempSendStruct);
        snmp_log(LOG_ERR,"XOS_GetUB error!");
        return SNMP_ERR_GENERR;
    }
    memset(pTempSendStruct->pStruct,0,pOidDescrip->dwStructLength);
    pSendStruct = pTempSendStruct->pStruct;

    wIndexLen = pOidDescrip->wIndexLen;
    pTempSendStruct->wIndexLength = wIndexLen;
    SNMP_PRINT_OID((char *)"requestvb->name ", requestvb->name, (int)requestvb->name_length);
    index_name_start = requestvb->name + requestvb->name_length - pOidDescrip->wIndex_name_len;
    SNMP_PRINT_OID((char *)"index_name_start ", index_name_start, pOidDescrip->wIndex_name_len);
    OAM_SYSLOG(LOG_DEBUG, (char *)"wIndexLen:%u;wIndex_name_len:%u\n", wIndexLen, pOidDescrip->wIndex_name_len);

    OAM_SYSLOG(LOG_DEBUG, (char *)"pOidDescrip->ucIndexNum:%d\n", pOidDescrip->ucIndexNum);
    pOidProp = pOidDescrip->pOidHead;
    for(ucLoop = 0, index=0;ucLoop<pOidDescrip->ucIndexNum;ucLoop++)
    {    
        if (pSendStruct != NULL)
        {
            pSendStruct = SnmpSet_Oid_Value(pOidProp->ucParaType, pSendStruct, index_name_start, &index, &wIndexLen, &dwIndexDeserveLen);
        }
        pOidProp = (T_Oid_Property *)pOidProp->pNext;
    }
    OAM_SYSLOG(LOG_DEBUG, (char *)"after dwIndexDeserveLen:%u\n", dwIndexDeserveLen);
    /* if (dwIndexDeserveLen != requestvb->name_length-(pOidDescrip->wBaseOidLen+2))
    {
        SNMP_SAFE_RETUB(pTempSendStruct->pStruct);
        SNMP_SAFE_RETUB(pTempSendStruct);
        return SNMP_ERR_GENERR;
    } */
    pTempSendStruct->pNext = NULL;
    pTempSendStruct->ucNeedCommit = ENTRY_DATA_NO_NEED_COMMIT;
    pTempSendStruct->ucCommit = ENTRY_DATA_NO_COMMIT;
    LST_ADD_NODE(pOidDescrip, pTempSendStruct);
    netsnmp_request_add_list_data(requests, netsnmp_create_data_list(TABLE_ITERATOR_NAME, pTempSendStruct, NULL));
    return SNMP_ERR_NOERROR;
}    

static int SnmpProtocolTableCheckValue(netsnmp_request_info *requests, T_Oid_Property *pOidProp, T_Send_Struct *table_entry)
{    
    netsnmp_variable_list *requestvb = requests->requestvb;
    long value = 0;
    OAM_RETURN_VAL_IF_FAIL(requestvb, SNMP_ERR_INCONSISTENTVALUE);
    OAM_RETURN_VAL_IF_FAIL(pOidProp, SNMP_ERR_INCONSISTENTVALUE);

    if (pOidProp->ucRowStateProp != OID_PROPERTY_ROWSTATUS)
    {
        return SnmpCheck_snmptype_value(requestvb->val.string, requestvb->type,  requestvb->val_len, pOidProp);
    }
    else
    {
        value = *((long *) requestvb->val.string);
        if (table_entry == NULL) /* 只能是创建列*/
        {
            if ((SNMP_ROW_CREATEANDGO != value) && (SNMP_ROW_CREATEANDWAIT != value))
            {
                return SNMP_ERR_INCONSISTENTVALUE;
            }
        }
        else /* 已存在的列只能取active 和destory! */
        {
            if ((SNMP_ROW_ACTIVE != value) && (SNMP_ROW_DESTROY != value))
            {
                return SNMP_ERR_INCONSISTENTVALUE;
            }
        }
    }
    return SNMP_ERR_NOERROR;
}/* SnmpProtocolTableCheckValue */

static int SnmpProtocolTableGetValue(WORD32 dwFuncID, netsnmp_request_info *requests, T_Oid_Property *pOidProp, BYTE *pDataBuf, size_t *ipmi_var_len)
{
    BYTE type = 0;
    size_t var_len = 0;
    BYTE *ret_vaule = NULL;

    /*判断节点可读性*/
    OamFormatLog((char *)"No authority to get oid!");
    OAM_NASSERT_RETURN_VAL_IF_FAIL(pOidProp->ucRwProp != OID_PROPERTY_WRITE, SNMP_ERR_BADVALUE);
    ret_vaule = SnmpGet_snmptype_Value(pOidProp->ucParaType, pDataBuf, pOidProp->wArrayLen, &var_len);
    if (ret_vaule != NULL)
    {
        type = SnmpGet_snmptype_by_ftype(pOidProp->ucParaType, pOidProp->wArrayLen);
        return snmp_set_var_typed_value(requests->requestvb, type, (BYTE *)ret_vaule, var_len);
    }
    else
    {
        return SNMP_ERR_BADVALUE;
    }
}/* SnmpProtocolScalarGetValue */

/* 除了requests->processed 处外出错时要设置明确的错误码，不能仅仅返回成功或失败 */
static int SnmpProtocolTable_handler(netsnmp_mib_handler *handler, netsnmp_handler_registration *reginfo,
    netsnmp_agent_request_info   *reqinfo, netsnmp_request_info *requests)
{
    long value = 0;    
    T_Oid_Property *pOidProp = NULL;
    T_Oid_Description *pOidDescrip = NULL;
    T_Send_Struct *table_entry = NULL;
    netsnmp_table_request_info *table_info = NULL;
    netsnmp_variable_list *requestvb = NULL;
    BYTE *pDataBuf = NULL;
    size_t var_len = 0;
    WORD32 dwRet = 0; 

    if (requests->processed) 
    {
        OAM_SYSLOG(LOG_DEBUG, (char *)"verbose:table already processed(%d)\n",requests->processed);
        return SNMP_ERR_NOERROR;
    }   
    pOidDescrip = (T_Oid_Description *)reginfo->my_reg_void;
    if (pOidDescrip == NULL)
    {
        netsnmp_set_request_error(reqinfo, requests, SNMP_NOSUCHINSTANCE);
        OAM_SYSLOG(LOG_ERR, (char *)"pOidDescrip->pSendStructHead can't be NULL\n");
        return SNMP_ERR_NOERROR;
    }
    /* 如果table_entry != NULL 说明有这一行 */
    table_entry = (T_Send_Struct *)netsnmp_extract_iterator_context(requests);
    table_info = netsnmp_extract_table_info(requests);
    requestvb = requests->requestvb;
    switch (reqinfo->mode)
    {
    case MODE_GET:
        if (table_entry == NULL)
        {
            netsnmp_set_request_error(reqinfo, requests, SNMP_NOSUCHINSTANCE);
            OAM_SYSLOG(LOG_DEBUG, (char *)"no table_entry\n");
            return SNMP_ERR_NOERROR;
        }
        pOidProp = SnmpProtocolGetPropertyOffSet(requestvb->name, requestvb->name_length, pOidDescrip, table_entry->pStruct, &pDataBuf);
        if ((pOidProp==NULL) || (pOidProp->ucIndexProp==OID_PROPERTY_INDEX))
        {
            netsnmp_set_request_error(reqinfo, requests, SNMP_NOSUCHINSTANCE);
            OAM_SYSLOG(LOG_DEBUG, (char *)"(pOidProp==NULL) || (pOidProp->ucIndexProp==OID_PROPERTY_INDEX)\n");
            return SNMP_ERR_NOERROR;
        }
        if (SNMP_ERR_NOERROR != SnmpProtocolTableGetValue(pOidDescrip->dwFuncID,requests, pOidProp, pDataBuf,&var_len))
        {
            netsnmp_set_request_error(reqinfo, requests, SNMP_NOSUCHINSTANCE);
            OAM_SYSLOG(LOG_DEBUG, (char *)"SNMP_ERR_NOERROR != SnmpProtocolTableGetValue(requests, pOidProp, pDataBuf)\n");
        }
        break;
    case MODE_GETNEXT:
    case MODE_GETBULK:
        break;
    case MODE_SET_RESERVE1:/* Check values for acceptability , next mode is MODE_SET_RESERVE2 or MODE_SET_FREE */
        if (table_entry == NULL)
        {
            pOidProp = SnmpProtocolGetPropertyOffSet(requestvb->name, requestvb->name_length, pOidDescrip, NULL, &pDataBuf);
        }
        else
        {
            pOidProp = SnmpProtocolGetPropertyOffSet(requestvb->name, requestvb->name_length, pOidDescrip, table_entry->pStruct, &pDataBuf);
        }
        if ((pOidProp==NULL) || (pOidProp->ucIndexProp==OID_PROPERTY_INDEX))
        {
            netsnmp_set_request_error(reqinfo, requests, SNMP_NOSUCHINSTANCE);
            OAM_SYSLOG(LOG_DEBUG, (char *)"(pOidProp==NULL) || (pOidProp->ucIndexProp==OID_PROPERTY_INDEX)\n");
            return SNMP_ERR_NOERROR;
        }
        dwRet = SnmpProtocolTableCheckValue(requests, pOidProp, table_entry);
        if (dwRet != SNMP_ERR_NOERROR)
        {
            netsnmp_set_request_error(reqinfo, requests, dwRet);
            if(table_entry != NULL)
                table_entry->ucCheckFlag = ENTRY_DATA_CHECK_WRONG;
            OAM_SYSLOG(LOG_DEBUG, (char *)"SnmpProtocolTableCheckValue(requests, pOidProp, table_entry) != SNMP_ERR_NOERROR\n");
            return SNMP_ERR_NOERROR;
        }
        else
        {
            if(table_entry != NULL)
                table_entry->ucCheckFlag = ENTRY_DATA_CHECK_RIGHT;
        }
        break;
    case MODE_SET_RESERVE2:/* Allocate memory and similar resources , next mode is MODE_SET_ACTION or MODE_SET_FREE */
        /*
         * memory reseveration, final preparation... 
         */
        if(table_entry != NULL)
        {
            /* ipmi 不支持创建*/
            OAM_SYSLOG(LOG_DEBUG, (char *)"table_entry != NULL\n");
            return SNMP_ERR_NOERROR;
        }
        pOidProp = SnmpProtocolGetPropertyOffSet(requestvb->name, requestvb->name_length, pOidDescrip, NULL, &pDataBuf);
        if ((pOidProp==NULL) || (pOidProp->ucIndexProp==OID_PROPERTY_INDEX))
        {
            netsnmp_set_request_error(reqinfo, requests, SNMP_NOSUCHINSTANCE);
            OAM_SYSLOG(LOG_DEBUG, (char *)"(pOidProp==NULL) || (pOidProp->ucIndexProp==OID_PROPERTY_INDEX)\n");
            return SNMP_ERR_NOERROR;
        }
        if (pOidProp->ucRowStateProp != OID_PROPERTY_ROWSTATUS)
        {
            OAM_SYSLOG(LOG_DEBUG, (char *)"pOidProp->ucRowStateProp != OID_PROPERTY_ROWSTATUS\n");
            return SNMP_ERR_NOERROR;
        }
        if (SnmpProtocolTableCreate(requests, pOidDescrip) != SNMP_ERR_NOERROR)
        {
            netsnmp_set_request_error(reqinfo, requests, SNMP_NOSUCHINSTANCE);
            OAM_SYSLOG(LOG_DEBUG, (char *)"SnmpProtocolTableCreate(requests, pOidDescrip) != SNMP_ERR_NOERROR\n");
            return SNMP_ERR_NOERROR;
        }
        break;
    case MODE_SET_ACTION:/* Perform the SET action (if reversible) , next mode is MODE_SET_COMMIT or MODE_SET_UNDO */  
        /*
         * The variable has been stored in string for
         * you to use, and you have just been asked to do something with
         * it.  Note that anything done here must be reversable in the UNDO case 
         */
        if ((table_entry = (T_Send_Struct *)netsnmp_extract_iterator_context(requests)) == NULL)
        {
            netsnmp_set_request_error(reqinfo, requests, SNMP_NOSUCHINSTANCE);
            OAM_SYSLOG(LOG_DEBUG, (char *)"(table_entry = (T_Send_Struct *)netsnmp_extract_iterator_context(requests)) == NULL\n");
            return SNMP_ERR_NOERROR;
        }
        pOidProp = SnmpProtocolGetPropertyOffSet(requestvb->name, requestvb->name_length, pOidDescrip, table_entry->pStruct, &pDataBuf);
        if ((pOidProp==NULL) || (pOidProp->ucIndexProp==OID_PROPERTY_INDEX))
        {
            netsnmp_set_request_error(reqinfo, requests, SNMP_NOSUCHINSTANCE);
            OAM_SYSLOG(LOG_DEBUG, (char *)"(pOidProp==NULL) || (pOidProp->ucIndexProp==OID_PROPERTY_INDEX)\n");
            return SNMP_ERR_NOERROR;
        }
        table_entry->ucCommit = ENTRY_DATA_NO_COMMIT;
        if (pOidProp->ucRowStateProp == OID_PROPERTY_ROWSTATUS)
        {
            value = *((long *) requestvb->val.string);
            if ((value==SNMP_ROW_CREATEANDWAIT) || (value==SNMP_ROW_CREATEANDGO))
            {
                table_entry->ucNeedCommit = ENTRY_DATA_NO_NEED_COMMIT;
                value = SNMP_ROW_NOTINSERVICE;
                memcpy(requestvb->val.string, &value, sizeof(long));
            }
            else if (value == SNMP_ROW_DESTROY)
            {
                if (OID_TRAP_SET(pOidDescrip->ucAckType))
                {
                    table_entry->ucNeedCommit = ENTRY_DATA_NO_NEED_COMMIT;
                    table_entry->ucCommit = ENTRY_DATA_COMMIT;
                }
                else
                {
                    table_entry->ucNeedCommit = ENTRY_DATA_NEED_DELETE;
                }
                break;
            }
            else if (value == SNMP_ROW_ACTIVE)
            {
                if (OID_TRAP_SET(pOidDescrip->ucAckType))
                {
                     table_entry->ucNeedCommit = ENTRY_DATA_NO_NEED_COMMIT;
                     table_entry->ucCommit = ENTRY_DATA_NO_COMMIT;
                    SnmpProtocolTableCommitTrapToDdm(pOidDescrip, table_entry->pStruct);
                }
                else
                {
                    table_entry->ucNeedCommit = ENTRY_DATA_NEED_COMMIT;
                }
            }
        }
        if (SNMP_ERR_NOERROR != SnmpProtocolSetValue(requestvb, pOidProp, pDataBuf))
        { 
            netsnmp_set_request_error(reqinfo, requests, SNMP_NOSUCHINSTANCE);
            OAM_SYSLOG(LOG_DEBUG, (char *)"SNMP_ERR_NOERROR != SnmpProtocolSetValue(requestvb, pOidProp, pDataBuf)\n");
            return SNMP_ERR_NOERROR;
        }
        break;
    case MODE_SET_UNDO:/* Reverse the SET action, next mode is FINISHED_FAILURE */
        /*
         * Back out any changes made in the ACTION case 
         */
         /* UNDO 里错误码必须提交为 SNMP_ERR_UNDOFAILED */
        break;
    case MODE_SET_FREE: /* next mode is FINISHED_FAILURE */
        /*
         * Release any resources that have been allocated 
         */
         /* 在netsnmp_cache_helper_handler 里释放内存 */
        if (table_entry != NULL)
        {
            table_entry->ucCommit = ENTRY_DATA_COMMIT;
        }
        break;            
    case MODE_SET_COMMIT: /* next mode is FINISHED_SUCCESS or FINISHED_FAILURE */
        /*
         * Things are working well, so it's now safe to make the change
         * permanently.  Make sure that anything done here can't fail! 
         */
#if 0
        if(!IS_BSP_OPT(pOidDescrip->dwFuncID))
        {
            OAM_SYSLOG(LOG_DEBUG, "!IS_BSP_OPT(pOidDescrip->dwFuncID)\n");
            return SNMP_ERR_NOERROR;
        }
        if ((table_entry = (T_Send_Struct *)netsnmp_extract_iterator_context(requests)) == NULL)
        {
            netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_COMMITFAILED);
            OAM_SYSLOG(LOG_DEBUG, "(table_entry = (T_Send_Struct *)netsnmp_extract_iterator_context(requests)) == NULL\n");
            return SNMP_ERR_NOERROR;
        }
        pOidProp = SnmpProtocolGetPropertyOffSet(requestvb->name, requestvb->name_length, pOidDescrip, table_entry->pStruct, &pDataBuf);
        if ((pOidProp==NULL) || (pOidProp->ucIndexProp==OID_PROPERTY_INDEX))
        {
            netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_COMMITFAILED);
            OAM_SYSLOG(LOG_DEBUG, "(pOidProp==NULL) || (pOidProp->ucIndexProp==OID_PROPERTY_INDEX)\n");
            return SNMP_ERR_NOERROR;
        }
        if (SNMP_ERR_NOERROR != SnmpProtocolScalarCommitToIpmi(requestvb, pOidDescrip, table_entry, pOidProp))
        {
            netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_COMMITFAILED);
            OAM_SYSLOG(LOG_DEBUG, "SNMP_ERR_NOERROR != SnmpProtocolScalarCommitToIpmi(requestvb, pOidDescrip, table_entry)\n");
            return SNMP_ERR_NOERROR;
        }
#endif
        break;                                    
    default:
        OAM_SYSLOG(LOG_DEBUG, (char *)"Unrecognised mode (%d)\n", reqinfo->mode);
        break;
    }
    return SNMP_ERR_NOERROR;
}/* SnmpProtocolTable_handler */

static int SnmpProtocolTableLoadFromDdm(T_Oid_Description *pOidDescription, int mode)
{
    WORD32          dwRowStatus = SNMP_ROW_ACTIVE;
    T_Send_Struct  *pTempStruct = NULL;
    T_Oid_Property *pOidProp = NULL;
    BYTE *pucData = NULL;
    ostringstream       oss;
    int array_num = 0; 
    int next_index = 0;   

    OAM_SYSLOG(LOG_DEBUG, (char *)"Load Table Data From XML-RPC(mode:%d)!\n", mode);
    
    oss << "tecs." << pOidDescription->oidName << ".query";
    
    while(next_index >= 0)
    {
        vector<xmlrpc_c::value> array_struct;
        if(SnmpGetXmlRpcValue((char *)oss.str().c_str(),array_struct,&next_index,&array_num) < 0)
        {
            OutPut(SYS_NOTICE, "Did not get any value from XML-RPC\n");
            return SNMP_NOSUCHINSTANCE;
        }
        
        if (array_num == 0)
        {
            OutPut(SYS_NOTICE, "array_num = 0\n");
            return SNMP_NOSUCHINSTANCE;
        } 
        
        vector<xmlrpc_c::value>::iterator it_struct = array_struct.begin();
        
        it_struct = array_struct.begin();
        for (; it_struct != array_struct.end(); ++it_struct)
        {
        	pTempStruct = (T_Send_Struct *)malloc(sizeof(T_Send_Struct));
            if (pTempStruct == NULL)
            {
                OAM_SYSLOG(LOG_NOTICE, (char *)"pTempStruct == NULL");
                return SNMP_NOSUCHINSTANCE;
            }
            memset(pTempStruct, 0, sizeof(T_Send_Struct));
            pTempStruct->pStruct = (BYTE *)malloc(pOidDescription->dwStructLength);
            if (pTempStruct->pStruct == NULL)
            {
                SNMP_SAFE_RETUB(pTempStruct);
                return SNMP_NOSUCHINSTANCE;
            }
            memset(pTempStruct->pStruct, 0, pOidDescription->dwStructLength);
            
            pucData = pTempStruct->pStruct;
            
            map<string, xmlrpc_c::value> p = value_struct(*it_struct); 
            map<string, xmlrpc_c::value>::iterator it;
            
            pOidProp = pOidDescription->pOidHead;
            while (pOidProp != NULL)
            {
                if(pOidProp->ucRowStateProp == OID_PROPERTY_ROWSTATUS)
                    memcpy(pucData, &dwRowStatus, sizeof(WORD32));
                else
                {
                    if(p.end() != (it = p.find(pOidProp->oidName)))
                    {
                        switch(pOidProp->ucParaXmlRpcType)
                        {
                            case XMLRPC_TYPE_INT:
                            {
                                uint32 value = xmlrpc_c::value_int(it->second);
                                memcpy(pucData,&value,pOidProp->wParaLen);
                                break;
                            }
                            case XMLRPC_TYPE_STRING:
                            {
                                string str = xmlrpc_c::value_string(it->second);
                                memcpy(pucData,str.c_str(),str.length());
                                break;
                            }
                            case XMLRPC_TYPE_IPV4:
                            {
                                string str = xmlrpc_c::value_string(it->second); 
                                DWORD   dwIP = 0;  
                                dwIP = inet_network(str.c_str()); 
                                memcpy(pucData,&dwIP,sizeof(DWORD));
                                break;
                            }
                            default:
                                break;
                        }
                        pucData += pOidProp->wParaLen;
                    }
                }
                pOidProp = (T_Oid_Property *)pOidProp->pNext;
            }
            
            pTempStruct->ucNeedCommit = ENTRY_DATA_NEED_COMMIT;
            pTempStruct->ucCommit = ENTRY_DATA_COMMIT;
            /*加入链表中*/
            pTempStruct->wIndexLength = pOidDescription->wIndexLen;
            LST_ADD_NODE(pOidDescription, pTempStruct);        
        }
    }
    return SNMP_ERR_NOERROR;
}/* SnmpProtocolTableLoadFromDdm */


/* 不能调用 netsnmp_request_set_error_all 和netsnmp_set_request_error */
static int SnmpProtocolTableLoad(netsnmp_cache *cache, void *vmagic)
{
    netsnmp_request_info *requests = NULL;
    netsnmp_agent_request_info *reqinfo = NULL;
    T_Oid_Description *pOidDescrip = NULL;

    OamFormatLog((char *)"cache can't be NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(cache != NULL, SNMP_ERR_NOERROR);
    OamFormatLog((char *)"cache->cache_hint can't be NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(cache->cache_hint != NULL, SNMP_ERR_NOERROR);
    OamFormatLog((char *)"cache->cache_hint->reginfo can't be NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(cache->cache_hint->reginfo != NULL, SNMP_ERR_NOERROR);
    pOidDescrip = (T_Oid_Description *)cache->cache_hint->reginfo->my_reg_void;     
    OamFormatLog((char *)"pOidDescrip can't be NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(pOidDescrip != NULL, SNMP_ERR_NOERROR);

    reqinfo = cache->cache_hint->reqinfo;
    requests = cache->cache_hint->requests;
    OamFormatLog((char *)"reqinfo can't be NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(reqinfo != NULL, SNMP_ERR_NOERROR);
    OamFormatLog((char *)"requests can't be NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(requests != NULL, SNMP_ERR_NOERROR);

    OutPut(SYS_DEBUG, "Load pOidDescrip->oidName = %s\n", pOidDescrip->oidName);

    /* acktype 为trap 的情况下所有的节点都是后台设置的，不需要从app 获取 */
    if (OID_TRAP_SET(pOidDescrip->ucAckType))
    {
        return SNMP_ERR_NOERROR;
    }
    
    SnmpProtocolTableLoadFromDdm(pOidDescrip, reqinfo->mode);
    return 0;
}/* SnmpProtocolTableLoad */

static int SnmpProtocolTableCommit(netsnmp_cache *cache, void *vmagic)
{
    netsnmp_request_info *requests = NULL;
    T_Oid_Description *pOidDescrip = NULL;

    OamFormatLog((char *)"cache can't be NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(cache != NULL, SNMP_ERR_GENERR);
    OamFormatLog((char *)"cache->cache_hint can't be NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(cache->cache_hint != NULL, SNMP_ERR_GENERR);
    OamFormatLog((char *)"cache->cache_hint->reginfo can't be NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(cache->cache_hint->reginfo != NULL, SNMP_ERR_GENERR);
    pOidDescrip = (T_Oid_Description *)cache->cache_hint->reginfo->my_reg_void;     
    OamFormatLog((char *)"pOidDescrip can't be NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(pOidDescrip != NULL, SNMP_ERR_GENERR);
    requests = cache->cache_hint->requests;
    OamFormatLog((char *)"requests can't be NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(requests != NULL, SNMP_ERR_GENERR);

    OutPut(SYS_DEBUG, "Commit pOidDescrip->oidName = %s\n", pOidDescrip->oidName);

    if (OID_TRAP_SET(pOidDescrip->ucAckType))
    {
        return SNMP_ERR_NOERROR;
    }
    
    if (IS_BSP_OPT(pOidDescrip->dwFuncID))
    {
        return SNMP_CACHE_SUCCESS;
    }
    if (SNMP_ERR_NOERROR != SnmpProtocolTableCommitToDdm(requests, pOidDescrip))
    {
        return SNMP_CACHE_ERROR;
    }
    return SNMP_CACHE_SUCCESS;
}

static int SnmpProtocolTableCommitToDdm(netsnmp_request_info *requests, T_Oid_Description *pOidDescrip)
{
    netsnmp_request_info *request = NULL;    
    T_Send_Struct *table_entry = NULL;
    
    for (request=requests; request!=NULL; request=request->next)
    {
        table_entry = (T_Send_Struct *)netsnmp_extract_iterator_context(request);
        OamFormatLog((char *)"table_entry is NULL!\n");
        OAM_NASSERT_CONTINUE_IF_FAIL(table_entry != NULL);
        OamFormatLog((char *)"table_entry->pStruct is NULL!\n");
        OAM_NASSERT_CONTINUE_IF_FAIL(table_entry->pStruct != NULL);
        OAM_SYSLOG(LOG_DEBUG, (char *)"ucNeedCommit:%d, ucCommit:%d\n", table_entry->ucNeedCommit, table_entry->ucCommit);
        if ((table_entry->ucCommit == ENTRY_DATA_COMMIT) || 
            (table_entry->ucCheckFlag == ENTRY_DATA_CHECK_WRONG))
        {
            OAM_SYSLOG(LOG_DEBUG, (char *)"this entry is not committed!\n");
            continue;
        }
        if ((table_entry->ucNeedCommit == ENTRY_DATA_NEED_COMMIT) || 
            (table_entry->ucNeedCommit == ENTRY_DATA_NEED_DELETE) )
        {
            /* 发送*/
            table_entry->ucCommit = ENTRY_DATA_COMMIT;
            if(SnmpProtocolTableCommitRespToDdm(requests->requestvb, pOidDescrip, table_entry) == SNMP_ERR_NOERROR)
            {
                OAM_SYSLOG(LOG_DEBUG, (char *)"this entry is committed!\n");
                continue;
            }
            else
            {
                OAM_SYSLOG(LOG_DEBUG, (char *)"commit error!\n");
                return SNMP_ERR_GENERR;
            }
        }
    }
    return SNMP_ERR_NOERROR;
}

static void SnmpProtocolTableFree(netsnmp_cache *cache, void *vmagic)
{
    T_Oid_Description *pOidDescrip = NULL;
    T_Send_Struct *pCurrStruct = NULL;
    T_Send_Struct *pNextStruct = NULL; 
    T_Send_Struct *pPreStruct = NULL; 

    OamFormatLog((char *)"cache can't be NULL!\n");
    OAM_RETURN_IF_FAIL(cache != NULL);
    OamFormatLog((char *)"cache->cache_hint can't be NULL!\n");
    OAM_RETURN_IF_FAIL(cache->cache_hint != NULL);
    OamFormatLog((char *)"cache->cache_hint->reginfo can't be NULL!\n");
    OAM_RETURN_IF_FAIL(cache->cache_hint->reginfo != NULL);
    pOidDescrip = (T_Oid_Description *)cache->cache_hint->reginfo->my_reg_void;     
    OamFormatLog((char *)"pOidDescrip can't be NULL!\n");
    OAM_RETURN_IF_FAIL(pOidDescrip != NULL);
    
    OutPut(SYS_DEBUG, "Free pOidDescrip->oidName = %s\n", pOidDescrip->oidName);

    pCurrStruct = pOidDescrip->pSendStructHead;
    while(pCurrStruct != NULL)
    {
        /* 已经提交的行可以删除*/
        if (pCurrStruct->ucCommit == ENTRY_DATA_COMMIT)
        {
            pNextStruct = pCurrStruct->pNext;            
            if (pPreStruct != NULL)
            {
                pPreStruct->pNext = pNextStruct;
            }
            /* 头节点*/
            if (pOidDescrip->pSendStructHead == pCurrStruct)
            {
                pOidDescrip->pSendStructHead = pNextStruct;
            }
            /* 尾节点*/
            if (pOidDescrip->pSendStructTail == pCurrStruct)
            {
                pOidDescrip->pSendStructTail = pPreStruct;
            }
            SNMP_SAFE_RETUB(pCurrStruct->pStruct);
            SNMP_SAFE_RETUB(pCurrStruct);
            pCurrStruct = pNextStruct;         
        }
        else
        {
            pPreStruct = pCurrStruct;
            pCurrStruct = pCurrStruct->pNext;
        }
    }   
}/* SnmpProtocolTableFree */

static int SnmpProtocolTableCommitRespToDdm(netsnmp_variable_list *requestvb, T_Oid_Description *pOidDescrip, 
    T_Send_Struct *table_entry)
{
    T_Oid_Property *pOidProp = NULL;
    BYTE *pucData = NULL;
    int retcode = 0;
    char temp[MAX_VALUE_LENGTH] = {0};
    ostringstream       oss;
    ostringstream       oss_ip;
   
    OamFormatLog((char *)"requestvb can't be NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(requestvb != NULL, SNMP_NOSUCHINSTANCE);
    OamFormatLog((char *)"pOidDescrip can't be NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(pOidDescrip != NULL, SNMP_NOSUCHINSTANCE);
    OamFormatLog((char *)"table_entry can't be NULL!\n");
    OAM_NASSERT_RETURN_VAL_IF_FAIL(table_entry != NULL, SNMP_NOSUCHINSTANCE);
    OamFormatLog((char *)"table_entry->pStruct can't be NULL!\n");
    OAM_NASSERT_RETURN_VAL_IF_FAIL(table_entry->pStruct != NULL, SNMP_NOSUCHINSTANCE);
    
    xmlrpc_c::paramList paramList;
    
    /*增加用户名和密码*/
    paramList.add(xmlrpc_c::value_string(g_sSession));

    pucData =  table_entry->pStruct;
    pOidProp = pOidDescrip->pOidHead;
    while (pOidProp != NULL)
    {
        switch(pOidProp->ucParaXmlRpcType)
        {
            case XMLRPC_TYPE_INT:
            {
                unsigned int value = 0;
                memcpy(&value,pucData,pOidProp->wParaLen);
                paramList.add(xmlrpc_c::value_int(value));
                break;
            }
            case XMLRPC_TYPE_STRING:
            {
                if(pOidProp->wParaLen > MAX_VALUE_LENGTH)
                {
                    OutPut(SYS_ERROR,"wrong string length\n");
                    return SNMP_CACHE_ERROR;
                }
                memcpy(temp,pucData,pOidProp->wParaLen);
                paramList.add(xmlrpc_c::value_string(temp));
                break;
            }
            case XMLRPC_TYPE_IPV4:
            {
                DWORD   dwIP = 0;  
                memcpy(&dwIP,pucData,pOidProp->wParaLen);
                oss_ip << ((dwIP & 0xff000000) >> 24) << "."
                       << ((dwIP & 0x00ff0000) >> 16) << "."
                       << ((dwIP & 0x0000ff00) >> 8) << "."
	               << (dwIP & 0x000000ff);
	        paramList.add(xmlrpc_c::value_string(oss_ip.str()));    
                break;
            }
            default:
                break;
        }
        pucData += pOidProp->wParaLen;
        pOidProp = (T_Oid_Property *)pOidProp->pNext;
    }
    
    if(table_entry->ucNeedCommit == ENTRY_DATA_NEED_COMMIT)
    {
        oss << "tecs." << pOidDescrip->oidName << ".set";
        SnmpSetXmlRpcValue((char *)oss.str().c_str(),paramList,&retcode);
        if (retcode != 0)
        {
            OutPut(SYS_NOTICE,"Set oid failed!(Errcode=%d)\n", retcode);
            return SNMP_ERR_GENERR;
        }
    }
    else if(table_entry->ucNeedCommit == ENTRY_DATA_NEED_DELETE)
    {
    	oss << "tecs." << pOidDescrip->oidName << ".delete";
        SnmpDeleteXmlRpcValue((char *)oss.str().c_str(),paramList,&retcode);
        if (retcode != 0)
        {
            OutPut(SYS_NOTICE,"Delete oid failed!(Errcode=%d)\n", retcode);
            return SNMP_ERR_GENERR;
        }
    }
    return SNMP_ERR_NOERROR;
}/* SnmpProtocolScalarCommitToDdm */

static int SnmpProtocolTableCommitTrapToDdm(T_Oid_Description *pOidDescrip, BYTE *pDataBuf)
{
//    XOS_STATUS      dwStatus = 0;
    
    OamFormatLog((char *)"pOidDescrip can't be NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(pOidDescrip != NULL, SNMP_NOSUCHINSTANCE);
    OamFormatLog((char *)"pDataBuf can't be NULL!\n");
    OAM_NASSERT_RETURN_VAL_IF_FAIL(pDataBuf != NULL, SNMP_NOSUCHINSTANCE);
#if 0
    memset(&g_tSnmpDdmReq,0,sizeof(T_SNMP_DDM_Req));
    g_tSnmpDdmReq.tDdmReq.tMsgHead.ucChnlType = SNMP_CHANNELS;
    g_tSnmpDdmReq.tDdmReq.wInvertFlag = 0x1234;
    g_tSnmpDdmReq.tDdmReq.dwFuncId = pOidDescrip->dwFuncID;
    g_tSnmpDdmReq.tDdmReq.wValidLen = pOidDescrip->dwStructLength -sizeof(WORD32);
    g_tSnmpDdmReq.tDdmReq.ucOperationFlag = DDM_SET;
    memcpy(&g_tSnmpDdmReq.jobID, &pOidDescrip->ptOwerModuleDesc->tJid, sizeof(JID));
    memcpy(g_tSnmpDdmReq.tDdmReq.acDDMReqBuf, pDataBuf, g_tSnmpDdmReq.tDdmReq.wValidLen);

    g_tSnmpDdmReq.tDdmReq.tMsgHead.uMsgHead.tSnmpMsgHead.ucWriteFlag = OID_PROPERTY_TRAP;
    dwStatus = OamSendMsg(EV_SNMP_DDM_FUNC_REQ,(BYTE *)&g_tSnmpDdmReq, sizeof(T_SNMP_DDM_Req),
                               EV_SNMP_DDM_FUNC_REQ_VER, XOS_MSG_HIGH,&g_ptSnmpdPri->tInterfaceJid);
    OamFormatLog("Send EV_SNMP_DDM_FUNC_REQ Fail(dwStatus=%u)!\n", dwStatus);
    OAM_NASSERT_RETURN_VAL_IF_FAIL(dwStatus == XOS_SUCCESS, SNMP_ERR_GENERR);
#endif
     return SNMP_ERR_NOERROR;
}/* SnmpProtocolTableCommitTrapToDdm */

void SnmpProtocolTableRegister(T_Oid_Description    *pOidDesc)
{
    BYTE ucType = 0;
    T_Oid_Property *pOidProperty = NULL;
    int modes = HANDLER_CAN_RWRITE;
    char szFuncId[30] = {0};
    netsnmp_table_registration_info *table_info = NULL;   
    netsnmp_iterator_info           *iinfo = NULL;
    netsnmp_handler_registration    *reginfo = NULL;

    sprintf(szFuncId, "zte-table-%s", pOidDesc->oidName);
    OAM_SYSLOG(LOG_DEBUG, (char *)"Initialising %s\n", szFuncId);
    SNMP_PRINT_OID((char *)"Snmp Register table ", pOidDesc->BaseOidName, pOidDesc->wBaseOidLen);
    OutPut(SYS_DEBUG, "Initialising %s table\n", szFuncId);
    if (pOidDesc->ucRwProp == OID_PROPERTY_RW)
    {
        modes = HANDLER_CAN_RWRITE;
    }
    else if (pOidDesc->ucRwProp == OID_PROPERTY_WRITE)
    {
        modes = HANDLER_CAN_SET_ONLY;
    }
    else
    {
        modes = HANDLER_CAN_RONLY;
    }
    /* Create the table data structure, and define the indexing....     */
    table_info = SNMP_MALLOC_TYPEDEF(netsnmp_table_registration_info);
    if (!table_info)
    {
        OAM_SYSLOG(LOG_ERR, (char *)"netsnmp_table_registration_info fail!\n");
        return;
    }
    /* .... and iteration information ....     */
    pOidProperty = pOidDesc->pOidHead;
    if(pOidProperty)
    {
        while (pOidProperty != NULL)
        {
            if (pOidProperty->ucIndexProp == OID_PROPERTY_INDEX)
            {
                ucType = SnmpGet_snmptype_by_ftype(pOidProperty->ucParaType, 1);
                netsnmp_table_helper_add_index(table_info, ucType);
            }
            pOidProperty = pOidProperty->pNext;
        }
        table_info->min_column = pOidDesc->pOidHead->ucPosNo;
        table_info->max_column = pOidDesc->pOidTail->ucPosNo;
        iinfo = SNMP_MALLOC_TYPEDEF(netsnmp_iterator_info);
        if (!iinfo) 
        {
            free(table_info);
            OAM_SYSLOG(LOG_ERR, (char *)"netsnmp_iterator_info fail!\n");
            return;
        }
        /* .... and register the table with the agent.     */
        iinfo->get_first_data_point = SnmpProtocolTable_first_entry;
        iinfo->get_next_data_point  = SnmpProtocolTable_next_entry;
        iinfo->table_reginfo        = table_info;
        iinfo->myvoid = pOidDesc;
        reginfo = netsnmp_create_handler_registration(szFuncId,
                SnmpProtocolTable_handler,
                pOidDesc->BaseOidName, pOidDesc->wBaseOidLen, modes),
        reginfo->my_reg_void = pOidDesc;      
        netsnmp_inject_handler(reginfo, netsnmp_get_serialize_handler());
        netsnmp_register_table_iterator(reginfo, iinfo);
        /* .... with a local cache     */
        netsnmp_inject_handler( reginfo,
		        netsnmp_get_cache_handler(ZTE_TABLE_STATUS_CACHE_TIMEOUT,
			   		    SnmpProtocolTableLoad, SnmpProtocolTableFree,
			   		    SnmpProtocolTableCommit,
					    pOidDesc->BaseOidName, pOidDesc->wBaseOidLen));
    }
    else
    {
        OAM_SYSLOG(LOG_ERR, (char *)"table has no colomn!\n");
        return;
    }
}/* SnmpProtocolTableRegister */

