/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�msg_sa_manager_with_api_method.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��sa_manager �� api_method ֮�����Ϣ��ṹ����
* ��ǰ�汾��1.0
* ��    �ߣ��뿥
* ������ڣ�2012��11��20��
*
* �޸ļ�¼1��
*     �޸����ڣ�2012/11/20
*     �� �� �ţ�V1.0
*     �� �� �ˣ��뿥
*     �޸����ݣ�����
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
