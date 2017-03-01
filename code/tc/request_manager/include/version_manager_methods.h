/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：version_manager_methods.h
* 文件标识：
* 内容摘要：版本管理XMl配置接口申明
* 当前版本：1.0
* 作    者：zhangyb
* 完成日期：2012年10月13日
*
* 修改记录1：
*     修改日期：2012/10/13
*     版 本 号：V1.0
*     修 改 人：zhangyb
*     修改内容：创建
******************************************************************************/
#ifndef TC_VERSION_MANAGER_METHODS_H
#define TC_VERSION_MANAGER_METHODS_H

#include "sky.h"
#include "request_manager.h"
#include "tecs_rpc_method.h"
#include "user_pool.h"
#include "mid.h"
#include "event.h"
#include "tecs_errcode.h"
#include "msg_version_with_api_method.h"

namespace zte_tecs
{
class VersionGetRepoAddr : public TecsRpcMethod
{
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERROR,TECS_ERROR)
        ERR_MAP(ERROR_TIME_OUT,TECS_ERR_TIMEOUT)
        ERR_MAP(ERROR_INVALID_ARGUMENT,TECS_ERR_PARA_INVALID)
        ERR_MAP_END
    }

    VersionGetRepoAddr (UserPool *upool=0)
    {
        _method_name = "VmmGetRepoAddrMethod";
        _user_id = -1;
        _upool = upool;
    };

    ~VersionGetRepoAddr (){};

/*****************************************************************************/
/**
@brief 功能描述:结果查询XML方法接口

@li @b 入参列表：
@verbatim
paramList    这个是XML传入的请求内容
@endverbatim

@li @b 出参列表：
@verbatim
retval      这个是该请求内置执行的结果
@endverbatim

@li @b 返回值列表：
@verbatim
无
@endverbatim

@li @b 接口限制：无
@li @b 其它说明：无
*/
/*****************************************************************************/
    void MethodEntry( xmlrpc_c::paramList const& paramList,
                      xmlrpc_c::value    *const  retval);

private:

};

class VersionSetRepoAddr : public TecsRpcMethod
{
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERROR,TECS_ERROR)
        ERR_MAP(ERROR_TIME_OUT,TECS_ERR_TIMEOUT)
        ERR_MAP(ERROR_INVALID_ARGUMENT,TECS_ERR_PARA_INVALID)
        ERR_MAP_END
    }

    VersionSetRepoAddr (UserPool *upool=0)
    {
        _method_name = "VmmSetRepoAddrMethod";
        _user_id = -1;
        _upool = upool;
    };

    ~VersionSetRepoAddr (){};

/*****************************************************************************/
/**
@brief 功能描述:结果查询XML方法接口

@li @b 入参列表：
@verbatim
paramList    这个是XML传入的请求内容
@endverbatim

@li @b 出参列表：
@verbatim
retval      这个是该请求内置执行的结果
@endverbatim

@li @b 返回值列表：
@verbatim
无
@endverbatim

@li @b 接口限制：无
@li @b 其它说明：无
*/
/*****************************************************************************/
    void MethodEntry( xmlrpc_c::paramList const& paramList,
                      xmlrpc_c::value    *const  retval);

private:

};

class VersionQuery : public TecsRpcMethod
{
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERROR,TECS_ERROR)
        ERR_MAP(ERROR_TIME_OUT,TECS_ERR_TIMEOUT)
        ERR_MAP(ERROR_INVALID_ARGUMENT,TECS_ERR_PARA_INVALID)
        ERR_MAP(ERROR_PROCESS_WILL_EXIT,TECS_ERR_VERSION_CANCEL_OPERATION)
        ERR_MAP(ERROR_NO_UPDATE_PACKAGE,TECS_ERR_VERSION_NO_UPGRADE_PACKAGE)
        ERR_MAP(ERROR_NO_INSTALLED_PACKAGE,TECS_ERR_VERSION_NO_INSTALLED_PACKAGE)
        ERR_MAP(ERROR_POPEN_FAIL,TECS_ERR_VERSION_START_OPERATION_FAIL)
        ERR_MAP(ERROR_POPEN_SELECT_FAIL,TECS_ERR_VERSION_OPERATION_ABNORMAL_EXIT)
        ERR_MAP_END
    }

    VersionQuery (UserPool *upool=0)
    {
        _method_name = "VersionQueryMethod";
        _user_id = -1;
        _upool = upool;
    };

    ~VersionQuery (){};

/*****************************************************************************/
/**
@brief 功能描述:版本查询XML方法接口

@li @b 入参列表：
@verbatim
paramList    这个是XML传入的请求内容
@endverbatim

@li @b 出参列表：
@verbatim
retval      这个是该请求内置执行的结果
@endverbatim

@li @b 返回值列表：
@verbatim
无
@endverbatim

@li @b 接口限制：无
@li @b 其它说明：无
*/
/*****************************************************************************/
    void MethodEntry( xmlrpc_c::paramList const& paramList,
                      xmlrpc_c::value    *const  retval);

private:

};

class VersionUpdate : public TecsRpcMethod
{
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERROR_TIME_OUT,TECS_ERR_TIMEOUT)
        ERR_MAP(ERROR_INVALID_ARGUMENT,TECS_ERR_PARA_INVALID)
        ERR_MAP(ERROR_DUPLICATED_OP,TECS_ERR_DUPLICATED_OP)
        ERR_MAP_END
    }

    VersionUpdate (UserPool *upool=0)
    {
        _method_name = "VmmUpdateMethod";
        _user_id = -1;
        _upool = upool;
    };

    ~VersionUpdate (){};

/*****************************************************************************/
/**
@brief 功能描述:结果查询XML方法接口

@li @b 入参列表：
@verbatim
paramList    这个是XML传入的请求内容
@endverbatim

@li @b 出参列表：
@verbatim
retval      这个是该请求内置执行的结果
@endverbatim

@li @b 返回值列表：
@verbatim
无
@endverbatim

@li @b 接口限制：无
@li @b 其它说明：无
*/
/*****************************************************************************/
    void MethodEntry( xmlrpc_c::paramList const& paramList,
                      xmlrpc_c::value    *const  retval);

private:

};

class VersionCancel : public TecsRpcMethod
{
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERROR,TECS_ERROR)
        ERR_MAP(ERROR_TIME_OUT,TECS_ERR_TIMEOUT)
        ERR_MAP(ERROR_INVALID_ARGUMENT,TECS_ERR_PARA_INVALID)
        ERR_MAP(ERROR_OBJECT_NOT_EXIST,TECS_ERR_ACTION_FAILED)
        ERR_MAP_END
    }

    VersionCancel (UserPool *upool=0)
    {
        _method_name = "VmmUninstallMethod";
        _user_id = -1;
        _upool = upool;
    };

    ~VersionCancel (){};

/*****************************************************************************/
/**
@brief 功能描述:结果查询XML方法接口

@li @b 入参列表：
@verbatim
paramList    这个是XML传入的请求内容
@endverbatim

@li @b 出参列表：
@verbatim
retval      这个是该请求内置执行的结果
@endverbatim

@li @b 返回值列表：
@verbatim
无
@endverbatim

@li @b 接口限制：无
@li @b 其它说明：无
*/
/*****************************************************************************/
    void MethodEntry( xmlrpc_c::paramList const& paramList,
                      xmlrpc_c::value    *const  retval);

private:

};

} //end namespace zte_tecs

#endif //end TC_VERSION_MANAGER_METHODS_H
