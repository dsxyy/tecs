/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：auth_config.h
* 文件标识：见配置管理计划书
* 内容摘要：公共消息和宏值的定义
* 当前版本：1.0
* 作    者： 李振红
* 完成日期：
*
* 修改记录1：
*     修改日期：2011/7/25
*     版 本 号：V1.0
*     修 改 人：李振红
*     修改内容：创建
*******************************************************************************/
#ifndef AUTH_CONFIG_H
#define AUTH_CONFIG_H
#include "event.h"
#include <xmlrpc-c/server_abyss.hpp>
#include <xmlrpc-c/registry.hpp>

// 禁用拷贝构造宏定义
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)
#endif

namespace zte_tecs
{

/**
@brief 功能描述: 鉴权请求消息，authmanager to drivermanager \n
@li @b 其它说明: 无
*/  
class AuthenticateMessage : public Serializable
{
public:
    AuthenticateMessage()
    {
    }
    
    AuthenticateMessage(int64 uid, string username, string password, string secret)
    {
        _uid = uid;
        _username = username;
        _password = password,
        _secret =  secret;  
    }   
    
    ~AuthenticateMessage()
    {
    }
    
    int64 get_uid()
    {
        return _uid;
    }       
    string get_username() const
    {
        return _username;
    }   
    
    string get_password() const
    {
        return _password;
    }   
    
    string get_secret() const
    {
        return _secret;
    }   
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(AuthenticateMessage);  //序列化开始
        WRITE_DIGIT(_uid);
        WRITE_STRING(_username);
        WRITE_STRING(_password);        
        WRITE_STRING(_secret);              
        SERIALIZE_END();
    }
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(AuthenticateMessage);
        READ_DIGIT(_uid);
        READ_STRING(_username);
        READ_STRING(_password);     
        READ_STRING(_secret);           
        DESERIALIZE_END();
    }
    
private:
    int64  _uid;
    string _username;
    string _password;
    string _secret;
};

/**
@brief 功能描述: 授权请求消息，authmanager to drivermanager \n
@li @b 其它说明: 无
*/
class AuthorizeMessage : public Serializable
{
public:
    AuthorizeMessage()
    {
    }
    
    AuthorizeMessage(int64 uid, int ob, string ob_id, int op, int64 owner, bool pub)
    {
        _uid = uid;
        _ob  = ob;
        _ob_id = ob_id;
        _op = op;
        _owner = owner;
        _pub = pub;
    }   
    
    ~AuthorizeMessage()
    {
    }
    
    int64 get_uid()
    {
        return _uid;
    }   

    int get_ob()
    {
        return _ob;
    }
    
    string get_ob_id() const
    {
        return _ob_id;
    }

    int get_op()
    {
        return _op;
    }

    int64 get_owner()
    {
        return _owner;
    }

    bool get_pub()
    {
        return _pub;
    }
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(AuthorizeMessage);  //序列化开始
        WRITE_DIGIT(_uid);
        WRITE_DIGIT(_ob);
        WRITE_STRING(_ob_id);       
        WRITE_DIGIT(_op);
        WRITE_DIGIT(_owner);
        WRITE_DIGIT(_pub);      
        SERIALIZE_END();
    }
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(AuthorizeMessage);
        READ_DIGIT(_uid);
        READ_DIGIT(_ob);
        READ_STRING(_ob_id);        
        READ_DIGIT(_op);
        READ_DIGIT(_owner);
        READ_DIGIT(_pub);
        DESERIALIZE_END();
    }
    
private:
    int64    _uid;
    int      _ob;
    string   _ob_id;
    int      _op;
    int64    _owner;    
    bool     _pub;
};

/**
@brief 功能描述: 驱动回复消息，drivermanager to authmanager \n
@li @b 其它说明: 无
*/
class DriverMessage : public Serializable
{
public:
    DriverMessage()
    {
    }
    
    DriverMessage(bool result, const string& message)
    {
        _result = result;
        _message = message;
    }
    
    ~DriverMessage()
    {
    }
    
    bool get_result()
    {
        return _result;
    }
    
    string get_message() const
    {
        return _message;
    }

    void set_result(bool result)
    {
        _result = result;
    }

    void set_message(const string& msg)
    {
        _message = msg;
    }
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(DriverMessage);  //序列化开始
        WRITE_DIGIT(_result);
        WRITE_STRING(_message);         
        SERIALIZE_END();
    }
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(DriverMessage);
        READ_DIGIT(_result);
        READ_STRING(_message);      
        DESERIALIZE_END();
    }
    
private:
    bool     _result;
    string   _message;
    
};

/**
@brief 功能描述: 会话消息，authrequest to authmanager \n
@li @b 其它说明: 无
*/
class SessionMessage : public Serializable
{
public:
    SessionMessage()
    {
    }
    
    SessionMessage(const string& session, const string& secret)
    {
        _session = session;
        _secret  = secret;
    }
    
    ~SessionMessage()
    {
    }       
    
    const string& get_session() const
    {
        return _session;
    } 

    const string& get_secret() const
    {
        return _secret;
    }     
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(SessionMessage);  //序列化开始
        WRITE_STRING(_session); 
        WRITE_STRING(_secret);        
        SERIALIZE_END();
    }
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(SessionMessage);
        READ_STRING(_session);  
        READ_STRING(_secret);           
        DESERIALIZE_END();
    }

private:
    string      _session;
    string      _secret;
};

}
#endif

