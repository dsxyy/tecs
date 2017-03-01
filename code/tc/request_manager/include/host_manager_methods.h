/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�tecs_host_manager_methods.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ�����������ģ���漰��XML-RPC method�ඨ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�Ԭ����
* ������ڣ�2011��8��30��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/8/30
*     �� �� �ţ�V1.0
*     �� �� �ˣ�Ԭ����
*     �޸����ݣ�����
*******************************************************************************/
#ifndef RM_TECS_HOST_MANAGER_METHODS_H
#define RM_TECS_HOST_MANAGER_METHODS_H

#include "tecs_rpc_method.h"
#include "tecs_errcode.h"
#include "api_error.h"

// ���ÿ�������궨��
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)
#endif

using namespace std;
namespace zte_tecs {

class HostSetMethod : public TecsRpcMethod
{
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERR_HM_AUTHORIZE_FAILED,    TECS_ERR_AUTHORIZE_FAILED)
        ERR_MAP(ERR_HM_PARA_INVALID,        TECS_ERR_PARA_INVALID)
        ERR_MAP(ERR_CLUSTER_TIMEOUT,        TECS_ERR_CLUSTER_TIMEOUT)
        ERR_MAP(ERR_CLUSTER_NOT_REGISTER,   TECS_ERR_CLUSTER_NOT_REGISTER)
        ERR_MAP(ERR_HOST_NOT_EXIST,         TECS_ERR_HOST_NOT_EXIST)
        ERR_MAP_END
    }
    HostSetMethod(UserPool * upool = 0)
    {
        _help = "Set the description of a host";
        _method_name = "HostSetMethod";
        _user_id = -1;
        _upool = upool;
    }
    HostSetMethod() {}
    void MethodEntry(const xmlrpc_c::paramList& paramList,
                     xmlrpc_c::value* const retval);
private:
    DISALLOW_COPY_AND_ASSIGN(HostSetMethod);
};

class HostForgetMethod : public TecsRpcMethod
{
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERR_HM_AUTHORIZE_FAILED,    TECS_ERR_AUTHORIZE_FAILED)        
        ERR_MAP(ERR_HM_PARA_INVALID,        TECS_ERR_PARA_INVALID)
        ERR_MAP(ERR_CLUSTER_TIMEOUT,        TECS_ERR_CLUSTER_TIMEOUT)
        ERR_MAP(ERR_CLUSTER_NOT_REGISTER,   TECS_ERR_CLUSTER_NOT_REGISTER)
        ERR_MAP(ERR_HOST_VMS_RUNNING,       TECS_ERR_HOST_VMS_RUNNING)
        ERR_MAP(ERR_HOST_NOT_EXIST,         TECS_ERR_HOST_NOT_EXIST)
        ERR_MAP_END
    }
    HostForgetMethod(UserPool * upool = 0)
    {
        _help = "Forget a forbidden or unegistered host of a cluster";
        _method_name = "tecs.host.forget";
        _user_id = -1;
        _upool = upool;
    }
    HostForgetMethod() {}
    void MethodEntry(const xmlrpc_c::paramList& paramList,
                     xmlrpc_c::value* const retval);    
private:
    DISALLOW_COPY_AND_ASSIGN(HostForgetMethod);
};
 
class HostDisableMethod : public TecsRpcMethod
{
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERR_HM_AUTHORIZE_FAILED,    TECS_ERR_AUTHORIZE_FAILED)
        ERR_MAP(ERR_HM_PARA_INVALID,        TECS_ERR_PARA_INVALID)
        ERR_MAP(ERR_CLUSTER_TIMEOUT,        TECS_ERR_CLUSTER_TIMEOUT)
        ERR_MAP(ERR_CLUSTER_NOT_REGISTER,   TECS_ERR_CLUSTER_NOT_REGISTER)
        ERR_MAP(ERR_HOST_FORCED_DISABLE,    TECS_SUCCESS)
        ERR_MAP(ERR_HOST_NOT_EXIST,         TECS_ERR_HOST_NOT_EXIST)
        ERR_MAP(ERR_HOST_VMS_RUNNING,       TECS_ERR_HOST_VMS_RUNNING)        
        ERR_MAP_END
    }
    HostDisableMethod(UserPool * upool = 0)
    {
        _help = "Disable a host of a cluster";
        _method_name = "tecs.host.disable";
        _user_id = -1;
        _upool = upool;
    }
    HostDisableMethod() {}
    void MethodEntry(const xmlrpc_c::paramList& paramList,
                     xmlrpc_c::value* const retval);    
private:
    DISALLOW_COPY_AND_ASSIGN(HostDisableMethod);
};

class HostEnableMethod : public TecsRpcMethod
{
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERR_HM_AUTHORIZE_FAILED,    TECS_ERR_AUTHORIZE_FAILED)
        ERR_MAP(ERR_HM_PARA_INVALID,        TECS_ERR_PARA_INVALID)
        ERR_MAP(ERR_CLUSTER_TIMEOUT,        TECS_ERR_CLUSTER_TIMEOUT)
        ERR_MAP(ERR_CLUSTER_NOT_REGISTER,   TECS_ERR_CLUSTER_NOT_REGISTER)
        ERR_MAP(ERR_HOST_NOT_EXIST,         TECS_ERR_HOST_NOT_EXIST)
        ERR_MAP_END
    }
    HostEnableMethod(UserPool * upool = 0)
    {
        _help = "Enable a host of a cluster";
        _method_name = "tecs.host.enable";
        _user_id = -1;
        _upool = upool;
    }
    HostEnableMethod() {}
    void MethodEntry(const xmlrpc_c::paramList& paramList,
                     xmlrpc_c::value* const retval);    
private:
    DISALLOW_COPY_AND_ASSIGN(HostEnableMethod);
};

class HostRebootMethod : public TecsRpcMethod
{
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERR_HM_AUTHORIZE_FAILED,    TECS_ERR_AUTHORIZE_FAILED)
        ERR_MAP(ERR_HM_PARA_INVALID,        TECS_ERR_PARA_INVALID)
        ERR_MAP(ERR_CLUSTER_TIMEOUT,        TECS_ERR_CLUSTER_TIMEOUT)
        ERR_MAP(ERR_CLUSTER_NOT_REGISTER,   TECS_ERR_CLUSTER_NOT_REGISTER)
        ERR_MAP(ERR_HOST_NOT_EXIST,         TECS_ERR_HOST_NOT_EXIST)
        ERR_MAP(ERR_HOST_VMS_RUNNING,       TECS_ERR_HOST_VMS_RUNNING) 
        ERR_MAP(ERR_HANDLE_EMPTY,           TECS_ERR_HANDLE_EMPTY)
        ERR_MAP(ERR_HANDLE_FAILED,          TECS_ERR_HANDLE_FAILED)
        ERR_MAP_END
    }
    HostRebootMethod(UserPool * upool = 0)
    {
        _help = "Reboot a host of a cluster";
        _method_name = "tecs.host.reboot";
        _user_id = -1;
        _upool = upool;
    }
    HostRebootMethod() {}
    void MethodEntry(const xmlrpc_c::paramList& paramList,
                     xmlrpc_c::value* const retval);
private:
    DISALLOW_COPY_AND_ASSIGN(HostRebootMethod);
};

class HostShutdownMethod : public TecsRpcMethod
{
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERR_HM_AUTHORIZE_FAILED,    TECS_ERR_AUTHORIZE_FAILED)
        ERR_MAP(ERR_HM_PARA_INVALID,        TECS_ERR_PARA_INVALID)
        ERR_MAP(ERR_CLUSTER_TIMEOUT,        TECS_ERR_CLUSTER_TIMEOUT)
        ERR_MAP(ERR_CLUSTER_NOT_REGISTER,   TECS_ERR_CLUSTER_NOT_REGISTER)
        ERR_MAP(ERR_HOST_NOT_EXIST,         TECS_ERR_HOST_NOT_EXIST)
        ERR_MAP(ERR_HOST_VMS_RUNNING,       TECS_ERR_HOST_VMS_RUNNING) 
        ERR_MAP(ERR_HANDLE_EMPTY,           TECS_ERR_HANDLE_EMPTY)
        ERR_MAP(ERR_HANDLE_FAILED,          TECS_ERR_HANDLE_FAILED)
        ERR_MAP_END
    }
    HostShutdownMethod(UserPool * upool = 0)
    {
        _help = "Shutdown a host of a cluster";
        _method_name = "tecs.host.shutdown";
        _user_id = -1;
        _upool = upool;
    }
    HostShutdownMethod() {}
    void MethodEntry(const xmlrpc_c::paramList& paramList,
                     xmlrpc_c::value* const retval);    
private:
    DISALLOW_COPY_AND_ASSIGN(HostShutdownMethod);
};

class HostQueryMethod : public TecsRpcMethod
{
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERR_HM_AUTHORIZE_FAILED,    TECS_ERR_AUTHORIZE_FAILED)
        ERR_MAP(ERR_HM_PARA_INVALID,        TECS_ERR_PARA_INVALID)
        ERR_MAP(ERR_CLUSTER_TIMEOUT,        TECS_ERR_CLUSTER_TIMEOUT)
        ERR_MAP(ERR_HOST_NOT_EXIST,         TECS_ERR_HOST_NOT_EXIST)
        ERR_MAP_END
    }
    HostQueryMethod(UserPool * upool = 0)
    {
        _help = "Show a/all host info of a cluster";
        _method_name = "tecs.host.show";
        _user_id = -1;
        _upool = upool;
    }
    HostQueryMethod() {}
    void MethodEntry(const xmlrpc_c::paramList& paramList,
                     xmlrpc_c::value* const retval);    
private:
    DISALLOW_COPY_AND_ASSIGN(HostQueryMethod);
};

} // namespace zte_tecs

#endif  //#ifndef RM_TECS_HOST_MANAGER_METHODS_H

