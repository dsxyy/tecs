/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：power_manager_methods.h
* 文件标识：见配置管理计划书
* 内容摘要：物理机管理模块涉及的XML-RPC method类定义文件
* 当前版本：1.0
* 作    者：liuyi
* 完成日期：2013年4月30日
*
* 修改记录1：
*     修改日期：2013/4/30
*     版 本 号：V1.0
*     修 改 人：liuyi
*     修改内容：创建
*******************************************************************************/
#ifndef RM_TECS_POWER_MANAGER_METHODS_H
#define RM_TECS_POWER_MANAGER_METHODS_H

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

class HostExternCtrlCmdMethod : public TecsRpcMethod
{
public:
    // 错误的定义: 使用REBOOT的返回码，增加不支持
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP(ERR_HM_AUTHORIZE_FAILED,    TECS_ERR_AUTHORIZE_FAILED)
        ERR_MAP(ERR_HM_PARA_INVALID,        TECS_ERR_PARA_INVALID)
        ERR_MAP(ERR_CLUSTER_TIMEOUT,        TECS_ERR_CLUSTER_TIMEOUT)
        ERR_MAP(ERR_CLUSTER_NOT_REGISTER,   TECS_ERR_CLUSTER_NOT_REGISTER)
        ERR_MAP(ERR_HOST_NOT_EXIST,         TECS_ERR_HOST_NOT_EXIST)
        ERR_MAP(ERR_HOST_VMS_RUNNING,       TECS_ERR_HOST_VMS_RUNNING) 
        ERR_MAP(ERR_HANDLE_OK,              TECS_SUCCESS)
        ERR_MAP(ERR_HOST_OK,                TECS_SUCCESS)
        ERR_MAP(ERR_HANDLE_EMPTY,           TECS_ERR_HANDLE_EMPTY)
        ERR_MAP(ERR_HANDLE_FAILED,          TECS_ERR_HANDLE_FAILED) 
        ERR_MAP(ERR_HOST_NOT_SUPPORT,       TECS_ERR_HOST_NOT_SUPPORT)
        ERR_MAP(ERR_HOST_THREAD_POOL_FAIL,  TECS_ERR_HANDLE_FAILED)
        ERR_MAP(ERR_HOST_DB_FAIL         ,  TECS_ERR_HANDLE_FAILED)    
        ERR_MAP(ERR_HOST_CTRL_HANDLE_FAIL,  TECS_ERR_HANDLE_FAILED)   
        ERR_MAP_END
    }
    HostExternCtrlCmdMethod(UserPool * upool = 0)
    {
        _help = "control host power";
        _method_name = "tecs.host.extern_ctrl_cmd";
        _user_id = -1;
        _upool = upool;
    }
    HostExternCtrlCmdMethod() {}
    void MethodEntry(const xmlrpc_c::paramList& paramList,
                     xmlrpc_c::value* const retval);    
private:
    DISALLOW_COPY_AND_ASSIGN(HostExternCtrlCmdMethod);
};


class HostExternCtrlQueryMethod : public TecsRpcMethod
{
public:
    // 错误的定义: 使用REBOOT的返回码，增加不支持
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP(ERR_HM_AUTHORIZE_FAILED,    TECS_ERR_AUTHORIZE_FAILED)
        ERR_MAP(ERR_HM_PARA_INVALID,        TECS_ERR_PARA_INVALID)
        ERR_MAP(ERR_CLUSTER_TIMEOUT,        TECS_ERR_CLUSTER_TIMEOUT)
        ERR_MAP(ERR_CLUSTER_NOT_REGISTER,   TECS_ERR_CLUSTER_NOT_REGISTER)
        ERR_MAP(ERR_HOST_NOT_EXIST,         TECS_ERR_HOST_NOT_EXIST)
        ERR_MAP(ERR_HOST_VMS_RUNNING,       TECS_ERR_HOST_VMS_RUNNING) 
        ERR_MAP(ERR_HANDLE_OK,              TECS_SUCCESS)
        ERR_MAP(ERR_HOST_OK,                TECS_SUCCESS)
        ERR_MAP(ERR_CMM_IP_NULL,            TECS_ERR_CMM_IP_NULL)
        ERR_MAP(ERR_HANDLE_FAILED,          TECS_ERR_HANDLE_FAILED) 
        ERR_MAP(ERR_OPERATE_NOT_SUPPORT,    TECS_ERR_HOST_NOT_SUPPORT)
        ERR_MAP(ERR_HOST_THREAD_POOL_FAIL,  TECS_ERR_HANDLE_FAILED)
        ERR_MAP(ERR_HOST_DB_FAIL         ,  TECS_ERR_HANDLE_FAILED)    
        ERR_MAP(ERR_HOST_CTRL_HANDLE_FAIL,  TECS_ERR_HANDLE_FAILED)  
        ERR_MAP_END
    }
    HostExternCtrlQueryMethod(UserPool * upool = 0)
    {
        _help = "query host power state";
        _method_name = "tecs.host.extern_ctrl.query";
        _user_id = -1;
        _upool = upool;
    }
    HostExternCtrlQueryMethod() {}
    void MethodEntry(const xmlrpc_c::paramList& paramList,
                     xmlrpc_c::value* const retval);    
private:
    DISALLOW_COPY_AND_ASSIGN(HostExternCtrlQueryMethod);
};

class FantrayRPMCtlMethod : public TecsRpcMethod
{
public:
    // 错误的定义:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP(ERR_HM_AUTHORIZE_FAILED,    TECS_ERR_AUTHORIZE_FAILED)
        ERR_MAP(ERR_HM_PARA_INVALID,        TECS_ERR_PARA_INVALID)
        ERR_MAP(ERR_CLUSTER_TIMEOUT,        TECS_ERR_CLUSTER_TIMEOUT)
        ERR_MAP(ERR_CMM_IP_NULL,            TECS_ERR_CMM_IP_NULL)
        ERR_MAP(ERR_CMM_CFG_INCORRECT,      TECS_ERR_CMM_CFG_INCORRECT)
        ERR_MAP(ERR_POWER_AGENT_SUCCESS,    TECS_SUCCESS)
        ERR_MAP_END
    }
    FantrayRPMCtlMethod(UserPool * upool = 0)
    {
        _help = "control fantray RPM";
        _method_name = "tecs.cmm.fantray_rpmctl";
        _user_id = -1;
        _upool = upool;
    }
    FantrayRPMCtlMethod() {}
    void MethodEntry(const xmlrpc_c::paramList& paramList,
                     xmlrpc_c::value* const retval);    
private:
    DISALLOW_COPY_AND_ASSIGN(FantrayRPMCtlMethod);
};


} // namespace zte_tecs

#endif  //#ifndef RM_TECS_POWER_MANAGER_METHODS_H

