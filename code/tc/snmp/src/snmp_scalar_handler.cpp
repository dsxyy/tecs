/*********************************************************************
* ��Ȩ���� (C)2011, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ� snmp_scalar_handler.c
* �ļ���ʶ��
* ����ժҪ��
* ����˵���� SNMP ��������/��ȡ�Ż�����
* ��ǰ�汾�� V1.0
* ��    �ߣ� �����񡪡�CSP ƽ̨
* ������ڣ�
*
* �޸ļ�¼1��
*    �޸����ڣ�2011��xx��xx��
*    �� �� �ţ�V1.0
*    �� �� �ˣ�������
*    �޸����ݣ�����
**********************************************************************/
#include "snmp_scalar_handler.h"
#include "snmp_oam_mibhandler.h"
#include "snmp_ftypes.h"
#include "oam_pub_type.h"
#include "log_agent.h"
#include "snmp_interface.h"
#include "snmp_info_method.h"

extern char    g_sSession[MAX_SESSION_LEN];

using namespace std;
using namespace xmlrpc_c;
/*
scalar �Ż�:
1��ipmi scalar ��setǰ��ȥ��ȡ���ݣ�ԭ����setǰҲȥ��ȡ���ݣ���Ϊ����ipmi scalar��һ��
       funcid ��ֻ��һ���ڵ㣬����set û�б�Ҫ;
2����ipmi scalar���Ż�
       2.1��������ָ�funcid��ע��Ľڵ����������Ľڵ������ͬ��
                 ����Set ֮ǰ������Get ��������setǰҲ��ȥ��ȡ����;
       2.2��funcid�²�ֹһ���ӿڵ�����������������ã�������������ͬһ��funcid �µ�
                 get�����ڿ�ʼǰ��ȡһ�Σ���;����ȡ(ԭ��ÿ���ڵ㶼���ȡһ��)��
                 ����set���󣬿�ʼǰ��ȡһ�Σ���������һ��(ԭ��ÿ���ڵ��ֱ��ȡ
                 ������һ��)��
       2.3�����ṹ������������Ƿ���Ҫ�Ż�?(����һ��funcid ����8���ڵ㣬setʱ������
                 8��������ʵ����Ҳ�ǲ���Ҫ�Ȼ�ȡ���ݵ�)�ݲ�����
*/
/*
ipmi:
    scalar:pOidDescrip->pSendStructHead��ŵ���ʵ�ʵ�ֵ;
    table:pOidDescrip->pSendStructHead��ŵ���������
*/
/* 5 ����֮�಻����Ӧ������������Ϣ����walk ʱ���� */
#define ZTE_SCALAR_STATUS_CACHE_TIMEOUT 5
#define MAX_ACKBUF_LEN          (WORD32)4000       /**<  Ӧ����Ϣ������󳤶� */

#define SNMP_RPC_USER_NAME   "snmpuser"

BYTE        acDDMAckBuf[MAX_ACKBUF_LEN] = {0};

extern T_SNMPProc_GlobVal *g_ptSnmpdPri;  /* SNMP�߳�ָ��Snmp_Protocol��˽����������ȫ�ֱ���ָ�� */ 
static int SnmpProtocolCacheScalarValue(T_Oid_Description *pOidDescrip, vector<xmlrpc_c::value> array_struct, int array_num);
static int SnmpProtocolScalarLoad(netsnmp_cache *cache, void *vmagic);
static void SnmpProtocolScalarFree(netsnmp_cache *cache, void *vmagic);
static int SnmpProtocolScalarCommit(netsnmp_cache *cache, void *vmagic);
static int SnmpProtocolScalarLoadFromDdm(T_Oid_Description *pOidDescrip, int mode, netsnmp_request_info *requests);
static int SnmpProtocolScalarGetValue(WORD32 dwFuncID,netsnmp_variable_list *requestvb, T_Oid_Property *pOidProp, BYTE *pDataBuf, WORD ipmi_var_len);
static int SnmpProtocolScalarCheckValue(netsnmp_variable_list *requestvb, T_Oid_Property *pOidProp);
static int count_regcounts(T_Oid_Property * reg_ptr);
static int count_reqcounts(netsnmp_request_info * req_ptr);
static int SnmpProtocolScalar_handler(netsnmp_mib_handler *handler, netsnmp_handler_registration *reginfo,
    netsnmp_agent_request_info   *reqinfo, netsnmp_request_info *requests);
static int SnmpProtocolScalarCommitToDdm(netsnmp_variable_list *requestvb, T_Oid_Description *pOidDescrip, T_Send_Struct  *pSendStruct);

static int SnmpProtocolCacheScalarValue(T_Oid_Description *pOidDescrip, vector<xmlrpc_c::value> array_struct, int array_num)
{
    T_Oid_Property *pOidProp = NULL;
    BYTE *pucData = NULL;

    OamFormatLog((char *)"pOidDescrip can't be NULL!\n");
    OAM_NASSERT_RETURN_VAL_IF_FAIL(pOidDescrip != NULL, SNMP_NOSUCHINSTANCE);
    OamFormatLog((char *)"pOidDescrip->dwStructLength == 0!\n");
    OAM_NASSERT_RETURN_VAL_IF_FAIL(pOidDescrip->dwStructLength > 0, SNMP_NOSUCHINSTANCE);
    OamFormatLog((char *)"array_num == 0!\n");
    OAM_NASSERT_RETURN_VAL_IF_FAIL(array_num > 0, SNMP_NOSUCHINSTANCE);
    
    pOidDescrip->pSendStructHead = (T_Send_Struct *)malloc(sizeof(T_Send_Struct));
    pOidDescrip->pSendStructTail = pOidDescrip->pSendStructHead;
    if (pOidDescrip->pSendStructHead != NULL)
    {
        memset(pOidDescrip->pSendStructHead, 0, sizeof(T_Send_Struct));
        pOidDescrip->pSendStructHead->pStruct = (BYTE *)malloc(pOidDescrip->dwStructLength);
        if (pOidDescrip->pSendStructHead->pStruct != NULL)
        {
           memset(pOidDescrip->pSendStructHead->pStruct, 0, pOidDescrip->dwStructLength);
        }
        else
        {
            OutPut(SYS_ERROR, "Get UB for pOidDescrip->pSendStructHead->pStruct fail!\n");
            SNMP_SAFE_RETUB(pOidDescrip->pSendStructHead);
            return SNMP_NOSUCHINSTANCE;
        }
    }
    else
    {
        OutPut(SYS_ERROR, "Get UB for pOidDescrip->pSendStructHead fail!\n");
        return SNMP_NOSUCHINSTANCE;
    }
    
    pucData = pOidDescrip->pSendStructHead->pStruct;
    vector<xmlrpc_c::value>::iterator it_struct = array_struct.begin();
    
    it_struct = array_struct.begin();
    for (; it_struct != array_struct.end(); ++it_struct)
    {
        map<string, xmlrpc_c::value> p = value_struct(*it_struct); 
        map<string, xmlrpc_c::value>::iterator it;
        
        pOidProp = pOidDescrip->pOidHead;
        while (pOidProp != NULL)
        {
            if(p.end() != (it = p.find(pOidProp->oidName)))
            {
                switch(pOidProp->ucParaXmlRpcType)
                {
                    case XMLRPC_TYPE_INT:
                    {
                        unsigned int value = xmlrpc_c::value_int(it->second);
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
            pOidProp = (T_Oid_Property *)pOidProp->pNext;
        }
    }
      
    return SNMP_ERR_NOERROR;
}/* SnmpProtocolCacheScalarValue */


static int count_regcounts(T_Oid_Property * reg_ptr)
{
    int count = 0;
    
    for (; reg_ptr != NULL; reg_ptr = reg_ptr->pNext)
    {
        count++;
    }

    return count;
}/* count_regcounts */

static int count_reqcounts(netsnmp_request_info * req_ptr)
{
    int count = 0;
    
    for (; req_ptr != NULL; req_ptr = req_ptr->next)
    {
        count++;
    }

    return count;
}/* count_reqcounts */

static int SnmpProtocolScalarLoadFromDdm(T_Oid_Description *pOidDescrip, int mode, netsnmp_request_info *requests)
{
    int vbCount = 0;
    int regCount = 0;
    ostringstream       oss;
    vector<xmlrpc_c::value> array_struct;
    int array_num = 0;
    int next_index = 0;

    OutPut(SYS_DEBUG, "Load Data From XML-RPC!\n");
    
    OamFormatLog((char *)"pOidDescrip can't be NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(pOidDescrip != NULL, SNMP_NOSUCHINSTANCE);
   
    if (MODE_IS_SET(mode))
    {
        /* ���ڷ�ipmi �����ڵ㣬һ��funcid ��������ж���ӽڵ㣬
        �൱��һ���ṹ���ж���ֶΣ���snmp ÿ��ֻ�ܸı�һ���ֶΣ�����Ҫ
        �Ȼ�ȡ�����ṹ��ֵ��Ȼ��ı�����ĳһ���ṹ��ֵ��Ȼ������ȥ*/
        vbCount = count_reqcounts(requests);
        regCount = count_regcounts(pOidDescrip->pOidHead);
        if (vbCount == regCount)
        {
            /* ��������൱��Ҫ���õı��������ͽṹ�ĸ�����ȣ�
                �������ṹ�������������֮ǰ���Բ���Get */
            return SnmpProtocolCacheScalarValue(pOidDescrip, array_struct, 1);
        }
    }
    oss << "tecs." << pOidDescrip->oidName << ".query";
    if(SnmpGetXmlRpcValue((char *)oss.str().c_str(),array_struct,&next_index,&array_num) < 0)
    {
        OutPut(SYS_NOTICE, "Did not get any value from XML-RPC\n");
        return SNMP_NOSUCHINSTANCE;
    }
    return SnmpProtocolCacheScalarValue(pOidDescrip, array_struct, array_num);
}/* SnmpProtocolScalarLoadFromDdm */


/* �������ݵ����� */
static int SnmpProtocolScalarLoad(netsnmp_cache *cache, void *vmagic)
{
    netsnmp_request_info *requests = NULL;
    netsnmp_agent_request_info *reqinfo = NULL;
    T_Oid_Description *pOidDescrip = NULL;

    OamFormatLog((char *)"cache can't be NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(cache != NULL, SNMP_CACHE_SUCCESS);
    OamFormatLog((char *)"cache->cache_hint can't be NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(cache->cache_hint != NULL, SNMP_CACHE_SUCCESS);
    OamFormatLog((char *)"cache->cache_hint->reginfo can't be NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(cache->cache_hint->reginfo != NULL, SNMP_CACHE_SUCCESS);
    pOidDescrip = (T_Oid_Description *)cache->cache_hint->reginfo->my_reg_void;     
    OamFormatLog((char *)"pOidDescrip can't be NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(pOidDescrip != NULL, SNMP_CACHE_SUCCESS);

    reqinfo = cache->cache_hint->reqinfo;
    requests = cache->cache_hint->requests;
    OamFormatLog((char *)"reqinfo can't be NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(reqinfo != NULL, SNMP_CACHE_SUCCESS);
    OamFormatLog((char *)"requests can't be NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(requests != NULL, SNMP_CACHE_SUCCESS);
    
    OutPut(SYS_DEBUG, "Load pOidDescrip->oidName = %s\n", pOidDescrip->oidName);

    SnmpProtocolScalarLoadFromDdm(pOidDescrip, reqinfo->mode, requests);
    return SNMP_CACHE_SUCCESS;
}/* SnmpProtocolScalarLoad */

/* SNMP_ERR_GENERR */
static void SnmpProtocolScalarFree(netsnmp_cache *cache, void *vmagic)
{
    T_Oid_Description *pOidDescrip = NULL;

    OamFormatLog((char *)"cache can't be NULL!\n");
    OAM_RETURN_IF_FAIL(cache != NULL);
    OamFormatLog((char *)"cache->cache_hint can't be NULL!\n");
    OAM_RETURN_IF_FAIL(cache->cache_hint != NULL);
    OamFormatLog((char *)"cache->cache_hint->reginfo can't be NULL!\n");
    OAM_RETURN_IF_FAIL(cache->cache_hint->reginfo != NULL);
    pOidDescrip = (T_Oid_Description *)cache->cache_hint->reginfo->my_reg_void;     
    OamFormatLog((char *)"pOidDescrip can't be NULL!\n");
    OAM_RETURN_IF_FAIL(pOidDescrip != NULL);
    
    OutPut(SYS_DEBUG, "free pOidDescrip->oidName = %s\n", pOidDescrip->oidName);

     /* ����ֻ���ͷ��ڴ� �����Ǳ�����ֱ��free���� */
    if (pOidDescrip->pSendStructHead != NULL)
    {
        SNMP_SAFE_RETUB(pOidDescrip->pSendStructHead->pStruct);
        SNMP_SAFE_RETUB(pOidDescrip->pSendStructHead);
    }
    return;
}/* SnmpProtocolScalarFree */

static int SnmpProtocolScalarCommitToDdm(netsnmp_variable_list *requestvb, T_Oid_Description *pOidDescrip, 
    T_Send_Struct  *pSendStruct)
{
    T_Oid_Property *pOidProp = NULL;
    BYTE *pucData = NULL;
    int retcode = 0;
    char temp[MAX_VALUE_LENGTH] = {0};
    ostringstream       oss;
    ostringstream       oss_ip;

    OutPut(SYS_DEBUG, "Send To XMLRPC Server!\n");
    
    OamFormatLog((char *)"requestvb can't be NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(requestvb != NULL, SNMP_NOSUCHINSTANCE);
    OamFormatLog((char *)"pOidDescrip can't be NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(pOidDescrip != NULL, SNMP_NOSUCHINSTANCE);
    /* MODE_SET_FREE ���ͷ� */
    OamFormatLog((char *)"pOidDescrip->pSendStructHead can't be NULL!\n");
    OAM_NASSERT_RETURN_VAL_IF_FAIL(pOidDescrip->pSendStructHead != NULL, SNMP_NOSUCHINSTANCE);
    OamFormatLog((char *)"pOidDescrip->pSendStructHead->pStruct can't be NULL!\n");
    OAM_NASSERT_RETURN_VAL_IF_FAIL(pOidDescrip->pSendStructHead->pStruct != NULL, SNMP_NOSUCHINSTANCE);
    
    if(pOidDescrip->pSendStructHead->ucCheckFlag == ENTRY_DATA_CHECK_WRONG)
    {
        OutPut(SYS_NOTICE,"wrong type or wrong value\n");
        return SNMP_ERR_NOERROR;
    }
    
    
    xmlrpc_c::paramList paramList;
    
    /*�����û���������*/
    paramList.add(xmlrpc_c::value_string(g_sSession));
    
    pucData =  pOidDescrip->pSendStructHead->pStruct;
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
    
    if(memcmp(pOidDescrip->oidName,SNMP_RPC_USER_NAME,strlen(pOidDescrip->oidName)) == 0)
    {
    	xmlrpc_c::value result;
        SnmpXmlRpcUserPoolSetMethod user_method(NULL);
        user_method.MethodEntry(paramList, &result);
        xmlrpc_c::value_array const arrayValue(result);
        vector<xmlrpc_c::value> rs = arrayValue.vectorValueValue();
        retcode = xmlrpc_c::value_int(rs[0]);
    }
    else
    {
        oss << "tecs." << pOidDescrip->oidName << ".set";
        SnmpSetXmlRpcValue((char *)oss.str().c_str(),paramList,&retcode);
    }
    if (retcode != 0)
    {
        OutPut(SYS_NOTICE,"Set oid failed!(Errcode=%d)\n", retcode);
        return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}/* SnmpProtocolScalarCommitToDdm */

static int SnmpProtocolScalarCommit(netsnmp_cache *cache, void *vmagic)
{
    netsnmp_agent_request_info *reqinfo = NULL;
    T_Oid_Description *pOidDescrip = NULL;
    netsnmp_request_info *requests = NULL;

    OamFormatLog((char *)"cache can't be NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(cache != NULL, SNMP_CACHE_SUCCESS);
    OamFormatLog((char *)"cache->cache_hint can't be NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(cache->cache_hint != NULL, SNMP_CACHE_SUCCESS);
    OamFormatLog((char *)"cache->cache_hint->reginfo can't be NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(cache->cache_hint->reginfo != NULL, SNMP_CACHE_SUCCESS);
    pOidDescrip = (T_Oid_Description *)cache->cache_hint->reginfo->my_reg_void;     
    OamFormatLog((char *)"pOidDescrip can't be NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(pOidDescrip != NULL, SNMP_CACHE_ERROR);
    reqinfo = cache->cache_hint->reqinfo;
    requests = cache->cache_hint->requests;
    OamFormatLog((char *)"reqinfo can't be NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(reqinfo != NULL, SNMP_CACHE_ERROR);
    OamFormatLog((char *)"requests can't be NULL!\n");
    OAM_RETURN_VAL_IF_FAIL(requests != NULL, SNMP_CACHE_ERROR);

    OutPut(SYS_DEBUG, "commit pOidDescrip->oidName = %s\n", pOidDescrip->oidName);
    
    if (MODE_IS_GET(reqinfo->mode))
    {
        return SNMP_CACHE_SUCCESS;
    }
    if(IS_BSP_OPT(pOidDescrip->dwFuncID))
    {
        return SNMP_CACHE_SUCCESS;
    }
    if (SNMP_ERR_NOERROR != SnmpProtocolScalarCommitToDdm(requests->requestvb, pOidDescrip, pOidDescrip->pSendStructHead))
    {
        return SNMP_CACHE_ERROR;
    }
    return SNMP_CACHE_SUCCESS;
}/* SnmpProtocolScalarCommit */

static int SnmpProtocolScalarGetValue(WORD32 dwFuncID, netsnmp_variable_list *requestvb, T_Oid_Property *pOidProp, BYTE *pDataBuf, WORD ipmi_var_len)
{
    BYTE type = 0;
    size_t var_len = 0;
    BYTE *ret_vaule = NULL;
    /*�жϽڵ�ɶ���*/
    OamFormatLog((char *)"No authority to get oid!");
    OAM_NASSERT_RETURN_VAL_IF_FAIL(pOidProp->ucRwProp != OID_PROPERTY_WRITE, SNMP_ERR_BADVALUE);
    ret_vaule = SnmpGet_snmptype_Value(pOidProp->ucParaType, pDataBuf, pOidProp->wArrayLen, &var_len);
    if (ret_vaule != NULL)
    {
        type = SnmpGet_snmptype_by_ftype(pOidProp->ucParaType, pOidProp->wArrayLen);
        if(IS_BSP_OPT(dwFuncID))
            return snmp_set_var_typed_value(requestvb, type, (BYTE *)ret_vaule, ipmi_var_len);
        else
            return snmp_set_var_typed_value(requestvb, type, (BYTE *)ret_vaule, var_len);
    }
    else
    {
        return SNMP_ERR_BADVALUE;
    }
}/* SnmpProtocolScalarGetValue */

static int SnmpProtocolScalarCheckValue(netsnmp_variable_list *requestvb, T_Oid_Property *pOidProp)
{    
    OAM_RETURN_VAL_IF_FAIL(requestvb, SNMP_ERR_INCONSISTENTVALUE);
    OAM_RETURN_VAL_IF_FAIL(pOidProp, SNMP_ERR_INCONSISTENTVALUE);
    return SnmpCheck_snmptype_value(requestvb->val.string, requestvb->type,  requestvb->val_len, pOidProp);
}/* SnmpProtocolScalarCheckValue */

static int SnmpProtocolScalar_handler(netsnmp_mib_handler *handler, netsnmp_handler_registration *reginfo,
    netsnmp_agent_request_info   *reqinfo, netsnmp_request_info *requests)
{
    T_Oid_Description *pOidDescrip = NULL;
    netsnmp_variable_list *requestvb = NULL;
    BYTE *pDataBuf = NULL;
    T_Oid_Property *pOidProp = NULL;
    WORD32 dwRet = 0;

    if (!netsnmp_cache_is_valid(reqinfo, reginfo->handlerName)) 
    {
        OutPut(SYS_ERROR, " cache is invalid!\n");
        return SNMP_ERR_NOERROR;
    }
    pOidDescrip = (T_Oid_Description *)reginfo->my_reg_void;
    if ((pOidDescrip == NULL) ||(pOidDescrip->pSendStructHead == NULL) || (pOidDescrip->pSendStructHead->pStruct == NULL))
    {
        netsnmp_set_request_error(reqinfo, requests, SNMP_NOSUCHINSTANCE);
        OutPut(SYS_ERROR, "pOidDescrip->pSendStructHead can't be NULL \n");
        return SNMP_ERR_NOERROR;
    }
    requestvb = requests->requestvb;
    /* set ״̬�ı仯���̲μ� handle_set 
        ��������Ĺ��̲μ�netsnmp_serialize_helper_handler ,
        ��������ύ�μ� handle_var_requests */
    switch (reqinfo->mode)
    {
        case MODE_GET:
            if ((pOidProp = SnmpProtocolGetPropertyOffSet(requestvb->name, requestvb->name_length, 
                pOidDescrip, pOidDescrip->pSendStructHead->pStruct, &pDataBuf)) == NULL)
            {
                netsnmp_set_request_error(reqinfo, requests, SNMP_NOSUCHINSTANCE);
                return SNMP_ERR_NOERROR;
            }
            if (SnmpProtocolScalarGetValue(pOidDescrip->dwFuncID, requestvb, pOidProp, pDataBuf, pOidDescrip->pSendStructHead->wIndexLength) != SNMP_ERR_NOERROR)
            {
                netsnmp_set_request_error(reqinfo, requests, SNMP_NOSUCHINSTANCE);
                return SNMP_ERR_NOERROR;
            }
            break;
        case MODE_GETNEXT:
        case MODE_GETBULK:
            break;
        case MODE_SET_RESERVE1:/* Check values for acceptability , next mode is MODE_SET_RESERVE2 or MODE_SET_FREE */
            if ((pOidProp = SnmpProtocolGetPropertyOffSet(requestvb->name, requestvb->name_length, 
                pOidDescrip, pOidDescrip->pSendStructHead->pStruct, &pDataBuf)) == NULL)
            {
                netsnmp_set_request_error(reqinfo, requests, SNMP_NOSUCHINSTANCE);
                return SNMP_ERR_NOERROR;
            }
            dwRet = SnmpProtocolScalarCheckValue(requestvb, pOidProp);
            if (dwRet != SNMP_ERR_NOERROR)
            {
                pOidDescrip->pSendStructHead->ucCheckFlag = ENTRY_DATA_CHECK_WRONG;
                netsnmp_set_request_error(reqinfo, requests, dwRet);
                return SNMP_ERR_NOERROR;
            }
            pOidDescrip->pSendStructHead->ucCheckFlag = ENTRY_DATA_CHECK_RIGHT;
            break;
        case MODE_SET_RESERVE2:/* Allocate memory and similar resources , next mode is MODE_SET_ACTION or MODE_SET_FREE */
            /*
             * memory reseveration, final preparation... 
             */
            break;
        case MODE_SET_ACTION:/* Perform the SET action (if reversible) , next mode is MODE_SET_COMMIT or MODE_SET_UNDO */  
            /*
             * The variable has been stored in string for
             * you to use, and you have just been asked to do something with
             * it.  Note that anything done here must be reversable in the UNDO case 
             */
            if(IS_BSP_OPT(pOidDescrip->dwFuncID))
            {
                return SNMP_ERR_NOERROR;
            }
            if ((pOidProp = SnmpProtocolGetPropertyOffSet(requestvb->name, requestvb->name_length, 
                    pOidDescrip, pOidDescrip->pSendStructHead->pStruct, &pDataBuf)) == NULL)
            {
                netsnmp_set_request_error(reqinfo, requests, SNMP_NOSUCHINSTANCE);
                return SNMP_ERR_NOERROR;
            }
            if (SnmpProtocolSetValue(requestvb, pOidProp, pDataBuf) != SNMP_ERR_NOERROR)
            {
                netsnmp_set_request_error(reqinfo, requests, SNMP_NOSUCHINSTANCE);
                return SNMP_ERR_NOERROR;
            }
            break;
        case MODE_SET_UNDO:/* Reverse the SET action, next mode is FINISHED_FAILURE */
            /*
             * Back out any changes made in the ACTION case 
             */
             /* UNDO �����������ύΪ SNMP_ERR_UNDOFAILED */
            break;
        case MODE_SET_FREE: /* next mode is FINISHED_FAILURE */
            /*
             * Release any resources that have been allocated 
             */
             /* ��netsnmp_cache_helper_handler ���ͷ��ڴ� */
            break;            
        case MODE_SET_COMMIT: /* next mode is FINISHED_SUCCESS or FINISHED_FAILURE */
            /*
             * Things are working well, so it's now safe to make the change
             * permanently.  Make sure that anything done here can't fail! 
             */
            if(!IS_BSP_OPT(pOidDescrip->dwFuncID))
            {
                return SNMP_ERR_NOERROR;
            }
            if ((pOidProp = SnmpProtocolGetPropertyOffSet(requestvb->name, requestvb->name_length, 
                    pOidDescrip, pOidDescrip->pSendStructHead->pStruct, &pDataBuf)) == NULL)
            {
                netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_COMMITFAILED);
                return SNMP_ERR_NOERROR;
            }
             /* commit ��Ĵ���������ύΪ SNMP_ERR_COMMITFAILED */
            break;                                    
        default:
            OutPut(SYS_DEBUG, "Unrecognised mode (%d)\n", reqinfo->mode);
            break;
    }
    return SNMP_ERR_NOERROR;
}/* SnmpProtocolScalar_handler */

void SnmpProtocolScalarRegister(T_Oid_Description    *pOidDesc)
{
    int modes = HANDLER_CAN_RWRITE;
    char szFuncId[30] = {0};
    netsnmp_handler_registration *reginfo = NULL;
    OamFormatLog((char *)"pOidDesc can't be NULL!\n");
    OAM_RETURN_IF_FAIL(pOidDesc != NULL);

    sprintf(szFuncId, "zte-table-%s", pOidDesc->oidName);
    SNMP_PRINT_OID("Snmp Register scalar ", pOidDesc->BaseOidName, pOidDesc->wBaseOidLen);
    OutPut(SYS_DEBUG, "Initialising %s group\n", szFuncId);
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
    reginfo = netsnmp_create_handler_registration(szFuncId, SnmpProtocolScalar_handler,
            pOidDesc->BaseOidName, pOidDesc->wBaseOidLen, modes);
    reginfo->my_reg_void = pOidDesc;
    netsnmp_register_scalar_group(reginfo, pOidDesc->pOidHead->ucPosNo, pOidDesc->pOidTail->ucPosNo);
    netsnmp_inject_handler(reginfo, netsnmp_get_cache_handler(ZTE_SCALAR_STATUS_CACHE_TIMEOUT,
        SnmpProtocolScalarLoad, SnmpProtocolScalarFree, SnmpProtocolScalarCommit, pOidDesc->BaseOidName, pOidDesc->wBaseOidLen));
}/* SnmpProtocolScalarRegister */

