/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：userapi.h
* 文件标识：见配置管理计划书
* 内容摘要：用户相关管理类
* 当前版本：1.0
* 作    者：李振红
* 完成日期：2011年7月22日
*
* 修改记录1：
*     修改日期：2011/7/22
*     版 本 号：V1.0
*     修 改 人：李振红
*     修改内容：创建
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
@brief 功能描述: 创建用户接口\n
@li @b 其它说明: 无
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
@brief 功能描述: 删除用户接口\n
@li @b 其它说明: 无
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
@brief 功能描述: 更新用户接口\n
@li @b 其它说明: 无
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
@brief 功能描述: 得到用户信息\n
@li @b 其它说明: 无
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
@brief 功能描述: 创建用户组接口\n
@li @b 其它说明: 无
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
@brief 功能描述: 删除用户组接口\n
@li @b 其它说明: 无
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
@brief 功能描述: 更新用户组接口\n
@li @b 其它说明: 无
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
@brief 功能描述: 查看用户组信息\n
@li @b 其它说明: 无
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
@brief 功能描述: 登陆接口\n
@li @b 其它说明: 无
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
@brief 功能描述: 会话查询接口\n
@li @b 其它说明: 无
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
@brief 功能描述: 登出接口\n
@li @b 其它说明: 无
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

