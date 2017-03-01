/*************************************************************
* ��Ȩ���� (C)2010, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ� snmp_interface.c
* �ļ���ʶ��
* ����ժҪ�� 
* ����˵���� ��
* ��ǰ�汾�� V1.0
* ��    �ߣ� ��������3Gƽ̨
* ������ڣ�
*
* �޸ļ�¼1��
*    �޸����ڣ�2010��xx��xx��
*    �� �� �ţ�V1.0
*    �� �� �ˣ�����
*    �޸����ݣ�����
*************************************************************/
/***********************************************************
 *                      ͷ�ļ�                             *
***********************************************************/
#include "snmp_interface.h"
#include "snmp_info_method.h"

/***********************************************************
 *                     ��������                            *
***********************************************************/
#define XMLRPCAPP_QUERY_COUNT       5

/***********************************************************
*                 �ļ��ڲ�ʹ�õĺ�                        *
***********************************************************/

/***********************************************************
*               �ļ��ڲ�ʹ�õ���������                    *
***********************************************************/
/***********************************************************
 *                     ȫ�ֱ���                            *
***********************************************************/

/***********************************************************
 *                     ���ر���                            *
***********************************************************/

/***********************************************************
 *                     ȫ�ֺ���                            *
***********************************************************/

/***********************************************************
 *                     �ֲ�����                            *
***********************************************************/
using namespace std;
using namespace xmlrpc_c;

string serverUrl = "http://localhost:8080/RPC2";

char    g_sSession[MAX_SESSION_LEN] = {0};


/************************************************************
* �������ƣ� SnmpGetXmlRpcValue()
* ���������� SNMP��ȡXMLRPC����
* ���ʵı� ��
* �޸ĵı� ��
* ��������� 
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2012/04/18      V1.0       ����          ����
***************************************************************/
int SnmpGetXmlRpcValue(char *aucMethodName,vector<xmlrpc_c::value> &array_struct,int *next_index,int *array_num)
{
    OutPut(SYS_NOTICE, "%s is called!\n",aucMethodName);
    
    int iReturncode = 0;
        
    /*�������*/
    xmlrpc_c::paramList paramList;
    
    paramList.add(xmlrpc_c::value_string(g_sSession));
    paramList.add(xmlrpc_c::value_i8(*next_index));
    paramList.add(xmlrpc_c::value_i8(XMLRPCAPP_QUERY_COUNT));        
    
    xmlrpc_c::clientSimple myClient;
    xmlrpc_c::value result;
    
    myClient.call(serverUrl, aucMethodName, paramList, &result);
    
    
    xmlrpc_c::value_array const arrayValue(result);
    vector<xmlrpc_c::value> rs = arrayValue.vectorValueValue();
    iReturncode = xmlrpc_c::value_int(rs[0]);
                
    if (iReturncode == 0)
    {
        *next_index = xmlrpc_c::value_i8(rs[1]);
        *array_num += xmlrpc_c::value_int(rs[3]);
        array_struct = xmlrpc_c::value_array(rs[4]).vectorValueValue();
    }
    else
    {
        *next_index = -1;
        return -1;
    }	

    return 0;
}

/************************************************************
* �������ƣ� SnmpSetXmlRpcValue()
* ���������� SNMP����XMLRPC����
* ���ʵı� ��
* �޸ĵı� ��
* ��������� 
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2012/04/18      V1.0       ����          ����
***************************************************************/
int SnmpSetXmlRpcValue(char *aucMethodName,xmlrpc_c::paramList &paramList,int *retcode)
{
    OutPut(SYS_NOTICE, "%s is called!\n",aucMethodName);
    
    xmlrpc_c::clientSimple myClient;
    xmlrpc_c::value result;
        
    myClient.call(serverUrl, aucMethodName, paramList, &result);
    xmlrpc_c::value_array const arrayValue(result);
    vector<xmlrpc_c::value> rs = arrayValue.vectorValueValue();
    *retcode = xmlrpc_c::value_int(rs[0]);
    
    return 0;
}

int SnmpDeleteXmlRpcValue(char *aucMethodName,xmlrpc_c::paramList &paramList,int *retcode)
{
    OutPut(SYS_NOTICE, "%s is called!\n",aucMethodName);
    
    xmlrpc_c::clientSimple myClient;
    xmlrpc_c::value result;
        
    myClient.call(serverUrl, aucMethodName, paramList, &result);
    xmlrpc_c::value_array const arrayValue(result);
    vector<xmlrpc_c::value> rs = arrayValue.vectorValueValue();
    *retcode = xmlrpc_c::value_int(rs[0]);

    return 0;
}


bool SnmpLoginAuth(const string &user)
{
    string str_session;
    str_session = GetSessionByName(user);
    memset(g_sSession, 0, MAX_SESSION_LEN);
    strncpy(g_sSession, str_session.c_str(), sizeof(g_sSession)); 
    
    return true;
}