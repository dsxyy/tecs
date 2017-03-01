/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：Rpcable.h
* 文件标识：见配置管理计划书
* 内容摘要：Rpcable基类的定义文件以及各种xmlrpc转换宏
* 当前版本：1.0
* 作    者：张文剑
* 完成日期：2011年10月18日
*
* 修改记录1：
*     修改日期：2011/10/18
*     版 本 号：V1.0
*     修 改 人：张文剑
*     修改内容：创建
* 修改记录2：
*     修改日期：2011/10/19
*     版 本 号：V1.0
*     修 改 人：张文剑
*     修改内容：增加PrintXml打印函数
*******************************************************************************/
#ifndef TECS_RPCABLE_H
#define TECS_RPCABLE_H
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/util.h>
using namespace std;

#define TO_RPC                  xmlrpc_c::value_struct to_rpc() const    
#define TO_RPC_BEGIN()          map<string, xmlrpc_c::value> res;
#define TO_RPC_END()            return xmlrpc_c::value_struct(res);
template<typename T>
bool ToStructArray(vector<xmlrpc_c::value>& array_data,const char*name,const vector<T>& objects)
{
    typename vector<T>::const_iterator it; 
    for (it=objects.begin(); it != objects.end(); it++) 
    {
        array_data.push_back(it->to_rpc());                 
    }  
    return true;
}

template<typename T>
bool ToI8Array(vector<xmlrpc_c::value>& array_data,const char*name,const vector<T>& values)
{
    typename vector<T>::const_iterator it; 
    for (it=values.begin(); it != values.end(); it++) 
    {
        array_data.push_back(xmlrpc_c::value_i8(*it));  
    }  
    return true;
}
#define TO_VALUE(member)        res.insert(make_pair(#member,xmlrpc_c::toValue(member)));
#define TO_I8(member)           res.insert(make_pair(#member,xmlrpc_c::value_i8(member)));
#define TO_I8_ARRAY(member)                         \
    vector<xmlrpc_c::value> array_data_##member;    \
    ToI8Array(array_data_##member,#member,member);  \
    res.insert(make_pair(#member,xmlrpc_c::value_array(array_data_##member)));
#define TO_DATETIME(member)     res.insert(make_pair(#member,xmlrpc_c::value_datetime(member)));
#define TO_STRUCT(member)       res.insert(make_pair(#member,member.to_rpc()));
#define TO_STRUCT_ARRAY(member)                         \
    vector<xmlrpc_c::value> array_data_##member;        \
    ToStructArray(array_data_##member,#member,member);  \
    res.insert(make_pair(#member,xmlrpc_c::value_array(array_data_##member)));

#define FROM_RPC            bool from_rpc(const map<string, xmlrpc_c::value>& s)
#define FROM_RPC_BEGIN()    map<string, xmlrpc_c::value>::const_iterator it;
#define FROM_RPC_END()      return true;
template<typename T>
bool FromStructArray(vector<T>& objects,const vector<xmlrpc_c::value>& array_data)
{
    typename vector<xmlrpc_c::value>::const_iterator it; 
    for (it=array_data.begin(); it != array_data.end(); it++)  
    {                                     
        T obj;
        map<string, xmlrpc_c::value> s;
        xmlrpc_c::fromValue(s,*it);
        obj.from_rpc(s);
        objects.push_back(obj);                 
    }  
    return true;
}

template<typename T>
bool FromI8Array(vector<T>& values,const vector<xmlrpc_c::value>& array_data)
{
    typename vector<xmlrpc_c::value>::const_iterator it; 
    for (it=array_data.begin(); it != array_data.end(); it++)  
    {                                     
        T val = xmlrpc_c::value_i8(*it);
        values.push_back(val);                 
    }  
    return true;
}

#define FROM_VALUE(member)                      \
    if (s.end() != (it = s.find(#member)))      \
    {                                           \
        xmlrpc_c::fromValue(member,it->second); \
    }
 
#define FROM_I8(member)                             \
    if (s.end() != (it = s.find(#member)))          \
    {                                               \
        member = xmlrpc_c::value_i8(it->second);    \
    }
    
#define FROM_I8_ARRAY(member)                                           \
    if (s.end() != (it = s.find(#member)))                              \
    {                                                                   \
        FromI8Array(member,xmlrpc_c::value_array(it->second).cvalue()); \
    }

#define FROM_DATETIME(member)                           \
    if (s.end() != (it = s.find(#member)))              \
    {                                                   \
        member = xmlrpc_c::value_datetime(it->second);  \
    } 
    
#define FROM_STRUCT(member)                                     \
    if (s.end() != (it = s.find(#member)))                      \
    {                                                           \
        member.from_rpc(xmlrpc_c::value_struct(it->second));    \
    }

#define FROM_STRUCT_ARRAY(member)                                               \
    if (s.end() != (it = s.find(#member)))                                      \
    {                                                                           \
        FromStructArray(member,xmlrpc_c::value_array(it->second).cvalue());     \
    }
    
class Rpcable
{
public:
    virtual TO_RPC = 0;    
    virtual FROM_RPC = 0;
    virtual bool Validate(string &err_str) {return true;};
    virtual ~Rpcable() {};

    void PrintXml()
    {
        xmlrpc_env env;
        xmlrpc_mem_block out;
        xmlrpc_env_init(&env);
        XMLRPC_MEMBLOCK_INIT(char, &env, &out, 0);
        if(env.fault_occurred)
        {
            //XMLRPC_MEMBLOCK_INIT中malloc内存失败
            fprintf(stderr, "Rpcable::PrintXml ERROR: %s (%d)\n",env.fault_string, env.fault_code);
            return;
        }
        
        xmlrpc_serialize_value(&env, &out,to_rpc().cValue());
        printf("XML-RPC XML:\n");
        printf("%.*s\n",
               XMLRPC_MEMBLOCK_SIZE(char, &out),
               XMLRPC_MEMBLOCK_CONTENTS(char, &out));
        XMLRPC_MEMBLOCK_CLEAN(char, &out);
        xmlrpc_env_clean(&env);
    };

    string toString() const
    {
        xmlrpc_env        env;
        xmlrpc_mem_block  out;
        ostringstream    oss;
        
        xmlrpc_env_init(&env);
        XMLRPC_MEMBLOCK_INIT(char, &env, &out, 0);
        if(env.fault_occurred)
        {            
            return "";
        }
        
        xmlrpc_serialize_value(&env, &out,to_rpc().cValue());
              
        oss << XMLRPC_MEMBLOCK_CONTENTS(char, &out);
        
        XMLRPC_MEMBLOCK_CLEAN(char, &out);
        xmlrpc_env_clean(&env);

        return oss.str();
    };
    
};
#endif



