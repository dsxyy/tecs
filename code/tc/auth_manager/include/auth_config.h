/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�auth_config.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��������Ϣ�ͺ�ֵ�Ķ���
* ��ǰ�汾��1.0
* ��    �ߣ� �����
* ������ڣ�
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/7/25
*     �� �� �ţ�V1.0
*     �� �� �ˣ������
*     �޸����ݣ�����
*******************************************************************************/
#ifndef AUTH_CONFIG_H
#define AUTH_CONFIG_H
#include "event.h"
#include <xmlrpc-c/server_abyss.hpp>
#include <xmlrpc-c/registry.hpp>

// ���ÿ�������궨��
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)
#endif

namespace zte_tecs
{

/**
@brief ��������: ��Ȩ������Ϣ��authmanager to drivermanager \n
@li @b ����˵��: ��
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
        SERIALIZE_BEGIN(AuthenticateMessage);  //���л���ʼ
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
@brief ��������: ��Ȩ������Ϣ��authmanager to drivermanager \n
@li @b ����˵��: ��
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
        SERIALIZE_BEGIN(AuthorizeMessage);  //���л���ʼ
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
@brief ��������: �����ظ���Ϣ��drivermanager to authmanager \n
@li @b ����˵��: ��
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
        SERIALIZE_BEGIN(DriverMessage);  //���л���ʼ
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
@brief ��������: �Ự��Ϣ��authrequest to authmanager \n
@li @b ����˵��: ��
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
        SERIALIZE_BEGIN(SessionMessage);  //���л���ʼ
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

