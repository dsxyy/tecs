/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：tecs_rpc_test_method.cpp
* 文件标识：见配置管理计划书
* 内容摘要：RequestManager类的实现文件
* 当前版本：1.0
* 作    者：马兆勉
* 完成日期：2011年7月22日
*
* 修改记录1：
*     修改日期：2011/7/22
*     版 本 号：V1.0
*     修 改 人：马兆勉
*     修改内容：创建
*******************************************************************************/

#include "tecs_rpc_test_method.h"

void TecsRpcTestMethod::MethodEntry(const xmlrpc_c::paramList& paramList,
                                    xmlrpc_c::value* const retval)
{
    int                 inParam;

    /*   -- RPC specific vars --  */
    vector<xmlrpc_c::value> arrayData;
    xmlrpc_c::value_array * arrayresult;

    //NebulaLog::log("ReM",Log::DEBUG,"UserAllocate method invoked");

    // Get the parameters
    // session      = xmlrpc_c::value_string(paramList.getString(0));

    inParam     = xmlrpc_c::value_int(paramList.getInt(1));
   #if 0 
    
    string  name = xmlrpc_c::value_string(paramList.getStruct(2)["name"]);
    int     age  = xmlrpc_c::value_int(paramList.getStruct(2)["age"]);
    //cout << "Get struct from RPC: Name=" << name <<" Age=" << age << endl;
    
    xmlrpc_c::value_array  arrayTest(paramList.getArray(3));
    vector<xmlrpc_c::value> rs = arrayTest.vectorValueValue();
    int    a = xmlrpc_c::value_int(rs[0]);
    double b = xmlrpc_c::value_double(rs[1]);
    string c = xmlrpc_c::value_string(rs[2]);
    
    string  user_name;
    int     user_age;
    map <string,xmlrpc_c::value> member = xmlrpc_c::value_struct(rs[3]);
    map <string,xmlrpc_c::value>::iterator iter = member.find("name");
    if( iter != member.end()) {
        user_name = xmlrpc_c::value_string(iter->second);
    }
    iter = member.find("age");
    if( iter != member.end() ) {
        user_age = xmlrpc_c::value_int(iter->second);
    }
    //cout << "Get array from RPC: a="<< a << " b=" << b << " c=" << c 
    //     << " username:" << user_name << " age:" << user_age <<endl; 
   #endif      
    // All nice, return the new uid to client
    arrayData.push_back(xmlrpc_c::value_int(0)); // SUCCESS
    arrayData.push_back(xmlrpc_c::value_int(inParam + 1));

    // Copy arrayresult into retval mem space
    arrayresult = new xmlrpc_c::value_array(arrayData);
    *retval = *arrayresult;
    
    delete arrayresult; // and get rid of the original

    return;
}
/******************************************************************************/

