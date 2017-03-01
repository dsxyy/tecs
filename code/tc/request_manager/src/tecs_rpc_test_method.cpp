/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�tecs_rpc_test_method.cpp
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��RequestManager���ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�������
* ������ڣ�2011��7��22��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/7/22
*     �� �� �ţ�V1.0
*     �� �� �ˣ�������
*     �޸����ݣ�����
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

