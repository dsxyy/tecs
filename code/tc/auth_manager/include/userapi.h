/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�userapi.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ���û���ع�����
* ��ǰ�汾��1.0
* ��    �ߣ������
* ������ڣ�2011��7��22��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/7/22
*     �� �� �ţ�V1.0
*     �� �� �ˣ������
*     �޸����ݣ�����
*******************************************************************************/
#ifndef AM_USERAPI_H
#define AM_USERAPI_H

#include "tecs_rpc_method.h"
#include "user.h"
#include "user_pool.h"
#include "usergroup.h"
#include "usergroup_pool.h"

using namespace std;

namespace zte_tecs
{

/**************************************************************************/
/**
@brief ��������: �����û��ӿ�\n
@li @b ����˵��: ��
*/
class UserAllocateMethod : public TecsRpcMethod
{

public:
    UserAllocateMethod(UserPool *upool);
    ~UserAllocateMethod();
    void MethodEntry(const xmlrpc_c::paramList& paramList, xmlrpc_c::value* const retval);

};

/**************************************************************************/
/**
@brief ��������: ɾ���û��ӿ�\n
@li @b ����˵��: ��
*/
class UserDeleteMethod: public TecsRpcMethod
{
public:
    UserDeleteMethod(UserPool * _upool);
    ~UserDeleteMethod();
    void MethodEntry(const xmlrpc_c::paramList& paramList, xmlrpc_c::value* const retval);
};

/**************************************************************************/
/**
@brief ��������: �����û��ӿ�\n
@li @b ����˵��: ��
*/
class UserUpdateMethod : public TecsRpcMethod
{
public:
    UserUpdateMethod(UserPool *upool);
    ~UserUpdateMethod();
    void MethodEntry(const xmlrpc_c::paramList& paramList, xmlrpc_c::value* const retval);

};

/**************************************************************************/
/**
@brief ��������: �õ��û���Ϣ\n
@li @b ����˵��: ��
*/
class UserInfoMethod: public TecsRpcMethod
{
public:
    UserInfoMethod(UserPool * _upool);
    ~UserInfoMethod();
    void MethodEntry(const xmlrpc_c::paramList& paramList, xmlrpc_c::value* const retval);
};

/**************************************************************************/
/**
@brief ��������: �����û���ӿ�\n
@li @b ����˵��: ��
*/
class UserGroupAllocateMethod : public TecsRpcMethod
{
public:
    UserGroupAllocateMethod(UserPool *upool);
    ~UserGroupAllocateMethod();
    void MethodEntry(const xmlrpc_c::paramList& paramList, xmlrpc_c::value* const retval);
};

/**************************************************************************/
/**
@brief ��������: ɾ���û���ӿ�\n
@li @b ����˵��: ��
*/
class UserGroupDeleteMethod: public TecsRpcMethod
{
public:
    UserGroupDeleteMethod(UserPool *upool);
    ~UserGroupDeleteMethod();
    void MethodEntry(const xmlrpc_c::paramList& paramList, xmlrpc_c::value* const retval);
};

/**************************************************************************/
/**
@brief ��������: �����û���ӿ�\n
@li @b ����˵��: ��
*/
class UserGroupUpdateMethod: public TecsRpcMethod
{
public:
    UserGroupUpdateMethod(UserPool *upool);
    ~UserGroupUpdateMethod();
    void MethodEntry(const xmlrpc_c::paramList& paramList, xmlrpc_c::value* const retval);
};

/**************************************************************************/
/**
@brief ��������: �鿴�û�����Ϣ\n
@li @b ����˵��: ��
*/
class UserGroupInfoMethod: public TecsRpcMethod
{
public:
    UserGroupInfoMethod(UserPool *upool);
    ~UserGroupInfoMethod();
    void MethodEntry(const xmlrpc_c::paramList& paramList, xmlrpc_c::value* const retval);
};

/**************************************************************************/
/**
@brief ��������: ��½�ӿ�\n
@li @b ����˵��: ��
*/
class SessionLoginMethod: public TecsRpcMethod
{
public:
    SessionLoginMethod(UserPool *upool);
    ~SessionLoginMethod();
    void MethodEntry(const xmlrpc_c::paramList& paramList, xmlrpc_c::value* const retval);
};

/**************************************************************************/
/**
@brief ��������: �Ự��ѯ�ӿ�\n
@li @b ����˵��: ��
*/
class SessionQueryMethod: public TecsRpcMethod
{
public:
    SessionQueryMethod(UserPool *upool);
    ~SessionQueryMethod();
    void MethodEntry(const xmlrpc_c::paramList& paramList, xmlrpc_c::value* const retval);
};


/**************************************************************************/
/**
@brief ��������: �ǳ��ӿ�\n
@li @b ����˵��: ��
*/
class SessionLogoutMethod: public TecsRpcMethod
{
public:
    SessionLogoutMethod(UserPool *upool);
    ~SessionLogoutMethod();
    void MethodEntry(const xmlrpc_c::paramList& paramList, xmlrpc_c::value* const retval);
};



}
#endif

