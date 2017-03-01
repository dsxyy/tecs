/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：storage_control_methods.h
* 文件标识：见配置管理计划书
* 内容摘要：SC模块涉及的XML-RPC method类定义文件
* 当前版本：1.0
* 作    者：
* 完成日期：2013年5月3日
*
* 修改记录1：
*     修改日期：2013/5/3
*     版 本 号：V1.0
*     修 改 人：
*     修改内容：创建
*******************************************************************************/
#ifndef RM_TECS_STORAGE_CONTROL_METHODS_H
#define RM_TECS_STORAGE_CONTROL_METHODS_H

#include "tecs_rpc_method.h"
#include "tecs_errcode.h"
#include "api_error.h"

// 禁用拷贝构造宏定义
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)
#endif

using namespace std;
namespace zte_tecs {


class PortableDiskCreateMethod : public TecsRpcMethod
{
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP(SUCCESS,                    TECS_SUCCESS)
        ERR_MAP(ERROR,                      TECS_ERROR)
        ERR_MAP(ERR_SC_PARA_INVALID,        TECS_ERR_PARA_INVALID)
        ERR_MAP(ERR_SC_TIMEOUT,             TECS_ERR_SC_TIMEOUT)
        ERR_MAP(ERROR_ACTION_RUNNING,       TECS_ERR_OPRUNNING)
        ERR_MAP_END
    }
    PortableDiskCreateMethod(UserPool * upool = 0)
    {
        _help = "Create a portable disk";
        _method_name = "PortableDiskCreateMethod";
        _user_id = -1;
        _upool = upool;
    }
    ~PortableDiskCreateMethod() {};
    virtual void MethodEntry(const xmlrpc_c::paramList& paramList,
                     xmlrpc_c::value* const retval);
private:
    DISALLOW_COPY_AND_ASSIGN(PortableDiskCreateMethod);
};

class PortableDiskDeleteMethod : public TecsRpcMethod
{
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP(SUCCESS,                    TECS_SUCCESS)
        ERR_MAP(ERROR,                      TECS_ERROR)
        ERR_MAP(ERR_SC_PARA_INVALID,        TECS_ERR_PARA_INVALID)
        ERR_MAP(ERR_SC_TIMEOUT,             TECS_ERR_SC_TIMEOUT)
        ERR_MAP(ERROR_ACTION_RUNNING,       TECS_ERR_OPRUNNING)
        ERR_MAP_END
    }
    PortableDiskDeleteMethod(UserPool * upool = 0)
    {
        _help = "Delete a portable disk";
        _method_name = "PortableDiskDeleteMethod";
        _user_id = -1;
        _upool = upool;
    }
    ~PortableDiskDeleteMethod() {};
    void MethodEntry(const xmlrpc_c::paramList& paramList,
                     xmlrpc_c::value* const retval);
private:
    DISALLOW_COPY_AND_ASSIGN(PortableDiskDeleteMethod);
};


class PortableDiskAttachHostMethod : public TecsRpcMethod
{
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP(SUCCESS,                            TECS_SUCCESS)
        ERR_MAP(ERROR,                                TECS_ERROR)
        ERR_MAP(ERR_SC_TIMEOUT,                 TECS_ERR_TIMEOUT)
        ERR_MAP(ERROR_INVALID_ARGUMENT,    TECS_ERR_PARA_INVALID)
        ERR_MAP(ERROR_ACTION_RUNNING,       TECS_ERR_OPRUNNING)
        ERR_MAP(ERROR_OBJECT_NOT_EXIST,    TECS_ERR_OBJECT_NOT_EXIST)
        ERR_MAP(ERROR_DB_SELECT_FAIL,        TECS_ERR_DB_QUERY_FAILED)
        ERR_MAP(ERROR_PERMISSION_DENIED,   TECS_ERR_DENIED_OP)

        ERR_MAP_END
    }
    PortableDiskAttachHostMethod(UserPool * upool = 0)
    {
        _help = "Attach a portable disk to host";
        _method_name = "PortableDiskAttachHostMethod";
        _user_id = -1;
        _upool = upool;
    }
    ~PortableDiskAttachHostMethod() {};
    virtual void MethodEntry(const xmlrpc_c::paramList& paramList,
                     xmlrpc_c::value* const retval);
private:
    DISALLOW_COPY_AND_ASSIGN(PortableDiskAttachHostMethod);
};

class PortableDiskDetachHostMethod : public TecsRpcMethod
{
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP(SUCCESS,                              TECS_SUCCESS)
        ERR_MAP(ERROR,                                  TECS_ERROR)
        ERR_MAP(ERR_SC_TIMEOUT,                  TECS_ERR_TIMEOUT)
        ERR_MAP(ERROR_INVALID_ARGUMENT,     TECS_ERR_PARA_INVALID)
        ERR_MAP(ERROR_ACTION_RUNNING,        TECS_ERR_OPRUNNING)
        ERR_MAP(ERROR_OBJECT_NOT_EXIST,    TECS_ERR_OBJECT_NOT_EXIST)
        ERR_MAP(ERROR_DB_SELECT_FAIL,        TECS_ERR_DB_QUERY_FAILED)
        ERR_MAP(ERROR_PERMISSION_DENIED,   TECS_ERR_DENIED_OP)
        ERR_MAP_END
    }
    PortableDiskDetachHostMethod(UserPool * upool = 0)
    {
        _help = "Detach a portable disk for host";
        _method_name = "PortableDiskDetachHostMethod";
        _user_id = -1;
        _upool = upool;
    }
    ~PortableDiskDetachHostMethod() {};
    void MethodEntry(const xmlrpc_c::paramList& paramList,
                     xmlrpc_c::value* const retval);
private:
    DISALLOW_COPY_AND_ASSIGN(PortableDiskDetachHostMethod);
};


class PortableDiskBlockInfoGetMethod : public TecsRpcMethod
{
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP(SUCCESS,                           TECS_SUCCESS)
        ERR_MAP(ERROR,                               TECS_ERROR)
        ERR_MAP(ERR_SC_TIMEOUT,                TECS_ERR_TIMEOUT)
        ERR_MAP(ERROR_INVALID_ARGUMENT,   TECS_ERR_PARA_INVALID)
        ERR_MAP(ERROR_ACTION_RUNNING,      TECS_ERR_OPRUNNING)
        ERR_MAP(ERROR_OBJECT_NOT_EXIST,   TECS_ERR_OBJECT_NOT_EXIST)
        ERR_MAP(ERROR_DB_SELECT_FAIL,        TECS_ERR_DB_QUERY_FAILED)
        ERR_MAP(ERROR_PERMISSION_DENIED,   TECS_ERR_DENIED_OP)
        ERR_MAP_END
    }
    PortableDiskBlockInfoGetMethod(UserPool * upool = 0)
    {
        _help = "Get block info";
        _method_name = "PortableDiskBlockInfoGetMethod";
        _user_id = -1;
        _upool = upool;
    }
    ~PortableDiskBlockInfoGetMethod() {};
    void MethodEntry(const xmlrpc_c::paramList& paramList,
                     xmlrpc_c::value* const retval);
private:
    DISALLOW_COPY_AND_ASSIGN(PortableDiskBlockInfoGetMethod);
};




}

#endif

