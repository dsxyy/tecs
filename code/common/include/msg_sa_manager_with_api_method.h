/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：msg_sa_manager_with_api_method.h
* 文件标识：见配置管理计划书
* 内容摘要：sa_manager 与 api_method 之间的消息体结构定义
* 当前版本：1.0
* 作    者：冯骏
* 完成日期：2012年11月20日
*
* 修改记录1：
*     修改日期：2012/11/20
*     版 本 号：V1.0
*     修 改 人：冯骏
*     修改内容：创建
*******************************************************************************/
#ifndef COMM_SA_MANAGER_WITH_API_METHOD_H
#define COMM_SA_MANAGER_WITH_API_METHOD_H

using namespace std;
namespace zte_tecs {

class MessageSaEnableReq : public Serializable
{
public:
    int64   _sid;
    bool    _enable;

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageSaEnableReq);
        WRITE_VALUE(_sid);
        WRITE_VALUE(_enable);
        SERIALIZE_END();
    };
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageSaEnableReq);
        READ_VALUE(_sid);
        READ_VALUE(_enable);
        DESERIALIZE_END();
    };
};

class MessageSaCommonAck : public Serializable
{
public:    
    int32   _ret_code;
    string  _result;

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageSaCommonAck);        
        WRITE_VALUE(_ret_code);
        WRITE_VALUE(_result);
        SERIALIZE_END();
    };
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageSaCommonAck);       
        READ_VALUE(_ret_code);
        READ_VALUE(_result);
        DESERIALIZE_END();
    };
};

class MessageSaForgetIdReq : public Serializable
{
public:
    int64   _sid;   

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageSaForgetIdReq);
        WRITE_VALUE(_sid);        
        SERIALIZE_END();
    };
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageSaForgetIdReq);
        READ_VALUE(_sid);       
        DESERIALIZE_END();
    };
};

class MessageSaForgetNameReq : public Serializable
{
public:
    string   _sa_name;   

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageSaForgetNameReq);
        WRITE_VALUE(_sa_name);        
        SERIALIZE_END();
    };
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageSaForgetNameReq);
        READ_VALUE(_sa_name);       
        DESERIALIZE_END();
    };
};

class MessageSaClusterMapReq : public Serializable
{
public:
    int64    _sid;
    string   _cluster_name;
    bool     _is_add;

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageSaClusterMapReq);
        WRITE_VALUE(_sid);    
        WRITE_VALUE(_cluster_name);       
        WRITE_VALUE(_is_add);  
        SERIALIZE_END();
    };
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageSaClusterMapReq);
        READ_VALUE(_sid);  
        READ_VALUE(_cluster_name); 
        READ_VALUE(_is_add);
        DESERIALIZE_END();
    };
};

}

#endif
