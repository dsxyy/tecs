/*************************************************************
* 版权所有 (C)2010, 深圳市中兴通讯股份有限公司。
*
* 文件名称： snmp_interface.c
* 文件标识：
* 内容摘要： 
* 其它说明： 无
* 当前版本： V1.0
* 作    者： 曹亮——3G平台
* 完成日期：
*
* 修改记录1：
*    修改日期：2010年xx月xx日
*    版 本 号：V1.0
*    修 改 人：曹亮
*    修改内容：创建
*************************************************************/
/***********************************************************
 *                      头文件                             *
***********************************************************/
#include "snmp_interface.h"
#include "snmp_info_method.h"

/***********************************************************
 *                     常量定义                            *
***********************************************************/
#define XMLRPCAPP_QUERY_COUNT       5

/***********************************************************
*                 文件内部使用的宏                        *
***********************************************************/

/***********************************************************
*               文件内部使用的数据类型                    *
***********************************************************/
/***********************************************************
 *                     全局变量                            *
***********************************************************/

/***********************************************************
 *                     本地变量                            *
***********************************************************/

/***********************************************************
 *                     全局函数                            *
***********************************************************/

/***********************************************************
 *                     局部函数                            *
***********************************************************/
using namespace std;
using namespace xmlrpc_c;

string serverUrl = "http://localhost:8080/RPC2";

char    g_sSession[MAX_SESSION_LEN] = {0};


/************************************************************
* 函数名称： SnmpGetXmlRpcValue()
* 功能描述： SNMP获取XMLRPC数据
* 访问的表： 无
* 修改的表： 无
* 输入参数： 
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2012/04/18      V1.0       曹亮          创建
***************************************************************/
int SnmpGetXmlRpcValue(char *aucMethodName,vector<xmlrpc_c::value> &array_struct,int *next_index,int *array_num)
{
    OutPut(SYS_NOTICE, "%s is called!\n",aucMethodName);
    
    int iReturncode = 0;
        
    /*处理入参*/
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
* 函数名称： SnmpSetXmlRpcValue()
* 功能描述： SNMP设置XMLRPC数据
* 访问的表： 无
* 修改的表： 无
* 输入参数： 
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2012/04/18      V1.0       曹亮          创建
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