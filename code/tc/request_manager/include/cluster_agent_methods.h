/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：tecs_cluster_agent_methods.h
* 文件标识：见配置管理计划书
* 内容摘要：物理机管理模块涉及的XML-RPC method类定义文件
* 当前版本：1.0
* 作    者：袁进坤
* 完成日期：2011年8月30日
*
* 修改记录1：
*     修改日期：2011/8/30
*     版 本 号：V1.0
*     修 改 人：袁进坤
*     修改内容：创建
*******************************************************************************/
#ifndef RM_TECS_CLUSTER_AGENT_METHODS_H
#define RM_TECS_CLUSTER_AGENT_METHODS_H

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

class ClusterConfigSetMethod : public TecsRpcMethod
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
        ERR_MAP(ERR_CLUSTER_CFG_NAME_INVALID,TECS_ERR_CLUSTER_CFG_NAME_INVALID)
        ERR_MAP(ERR_CLUSTER_CFG_SET_FAILD,  TECS_ERR_CLUSTER_CFG_SET_FAILD)
        ERR_MAP_END
    }
    ClusterConfigSetMethod(UserPool * upool = 0)
    {
        _help = "Set cluster config";
        _method_name = "tecs.clustercfg.set";
        _user_id = -1;
        _upool = upool;
    }
    ClusterConfigSetMethod() {}
    void MethodEntry(const xmlrpc_c::paramList& paramList,
                     xmlrpc_c::value* const retval);    
private:
    DISALLOW_COPY_AND_ASSIGN(ClusterConfigSetMethod);
};

class ClusterConfigDelMethod : public TecsRpcMethod
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
        ERR_MAP(ERR_CLUSTER_CFG_NAME_INVALID,TECS_ERR_CLUSTER_CFG_NAME_INVALID)
        ERR_MAP(ERR_CLUSTER_CFG_DEL_FAILD,  TECS_ERR_CLUSTER_CFG_DEL_FAILD)
        ERR_MAP_END
    }
    ClusterConfigDelMethod(UserPool * upool = 0)
    {
        _help = "Delete cluster config";
        _method_name = "tecs.clustercfg.delete";
        _user_id = -1;
        _upool = upool;
    }
    ClusterConfigDelMethod() {}
    void MethodEntry(const xmlrpc_c::paramList& paramList,
                     xmlrpc_c::value* const retval);    
private:
    DISALLOW_COPY_AND_ASSIGN(ClusterConfigDelMethod);
};

class ClusterConfigQueryMethod : public TecsRpcMethod
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
        ERR_MAP_END
    }
    ClusterConfigQueryMethod(UserPool * upool = 0)
    {
        _help = "Show cluster config";
        _method_name = "tecs.clustercfg.query";
        _user_id = -1;
        _upool = upool;
    }
    ClusterConfigQueryMethod() {}
    void MethodEntry(const xmlrpc_c::paramList& paramList,
                     xmlrpc_c::value* const retval);    
private:
    DISALLOW_COPY_AND_ASSIGN(ClusterConfigQueryMethod);
};

class TcuConfigSetMethod : public TecsRpcMethod
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
        ERR_MAP(ERR_TCU_CFG_INVALID,      TECS_ERR_TCU_CFG_INVALID)
        ERR_MAP(ERR_CPUINFO_ID_NOT_EXIST,   TECS_ERR_CPUINFO_ID_NOT_EXIST)
        ERR_MAP(ERR_TCU_SET_FAILD,        TECS_ERR_TCU_SET_FAILD) 
        ERR_MAP(ERR_TCU_OPERATE_FAILD,        TECS_ERR_TCU_OPERATE_FAILD)            
        
        ERR_MAP_END
    }
    TcuConfigSetMethod(UserPool * upool = 0)
    {
        _help = "Set tcu config";
        _method_name = "tecs.tcu.set";
        _user_id = -1;
        _upool = upool;
    }
    TcuConfigSetMethod() {}
    void MethodEntry(const xmlrpc_c::paramList& paramList,
                     xmlrpc_c::value* const retval);    
private:
    DISALLOW_COPY_AND_ASSIGN(TcuConfigSetMethod);
};

class TcuConfigDelMethod : public TecsRpcMethod
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
        ERR_MAP(ERR_TCU_CFG_INVALID,      TECS_ERR_TCU_CFG_INVALID)
        ERR_MAP(ERR_CPUINFO_ID_NOT_EXIST,   TECS_ERR_CPUINFO_ID_NOT_EXIST)
        ERR_MAP(ERR_TCU_DEL_FAILD,        TECS_ERR_TCU_DEL_FAILD)
        ERR_MAP_END
    }
    TcuConfigDelMethod(UserPool * upool = 0)
    {
        _help = "Delete tcu config";
        _method_name = "tecs.tcu.delete";
        _user_id = -1;
        _upool = upool;
    }
    TcuConfigDelMethod() {}
    void MethodEntry(const xmlrpc_c::paramList& paramList,
                     xmlrpc_c::value* const retval);    
private:
    DISALLOW_COPY_AND_ASSIGN(TcuConfigDelMethod);
};

class TcuConfigQueryMethod : public TecsRpcMethod
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
        ERR_MAP(ERR_TCU_CFG_INVALID,      TECS_ERR_TCU_CFG_INVALID)
        ERR_MAP(ERR_TCU_QUERY_FAILD,      TECS_ERR_DB_QUERY_FAILED)
        ERR_MAP(ERR_CPUINFO_ID_NOT_EXIST,   TECS_ERR_CPUINFO_ID_NOT_EXIST)
        ERR_MAP_END
    }
    TcuConfigQueryMethod(UserPool * upool = 0)
    {
        _help = "Show tcu config";
        _method_name = "tecs.tcu.query";
        _user_id = -1;
        _upool = upool;
    }
    TcuConfigQueryMethod() {}
    void MethodEntry(const xmlrpc_c::paramList& paramList,
                     xmlrpc_c::value* const retval);    
private:
    DISALLOW_COPY_AND_ASSIGN(TcuConfigQueryMethod);
};

class CmmConfigAddMethod : public TecsRpcMethod
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
            ERR_MAP(ERR_CMM_ADD_FAIL,           TECS_ERR_CMM_ADD_FAIL)
        ERR_MAP_END
    }
    CmmConfigAddMethod(UserPool * upool = 0)
    {
        _help = "Add cmm config";
        _method_name = "tecs.cmm.add";
        _user_id = -1;
        _upool = upool;
    }
    CmmConfigAddMethod() {}
    void MethodEntry(const xmlrpc_c::paramList& paramList,
                     xmlrpc_c::value* const retval);    
private:
    DISALLOW_COPY_AND_ASSIGN(CmmConfigAddMethod);
};


class CmmConfigSetMethod : public TecsRpcMethod
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
        ERR_MAP(ERR_CMM_SET_FAIL,           TECS_ERR_CMM_SET_FAIL)
        ERR_MAP_END
    }
    CmmConfigSetMethod(UserPool * upool = 0)
    {
        _help = "Set cmm config";
        _method_name = "tecs.cmm.set";
        _user_id = -1;
        _upool = upool;
    }
    CmmConfigSetMethod() {}
    void MethodEntry(const xmlrpc_c::paramList& paramList,
                     xmlrpc_c::value* const retval);    
private:
    DISALLOW_COPY_AND_ASSIGN(CmmConfigSetMethod);
};

class CmmConfigDeleteMethod : public TecsRpcMethod
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
        ERR_MAP(ERR_CMM_DEL_FAIL,           TECS_ERR_CMM_DEL_FAIL)
        ERR_MAP_END
    }
    CmmConfigDeleteMethod(UserPool * upool = 0)
    {
        _help = "Delete cmm config";
        _method_name = "tecs.cmm.delete";
        _user_id = -1;
        _upool = upool;
    }
    CmmConfigDeleteMethod() {}
    void MethodEntry(const xmlrpc_c::paramList& paramList,
                     xmlrpc_c::value* const retval);    
private:
    DISALLOW_COPY_AND_ASSIGN(CmmConfigDeleteMethod);
};


class CmmConfigQueryMethod : public TecsRpcMethod
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
        ERR_MAP(ERR_CMM_QUERY_FAIL,         TECS_ERR_CMM_QUERY_FAIL)
        ERR_MAP_END
    }
    CmmConfigQueryMethod(UserPool * upool = 0)
    {
        _help = "Query cmm info";
        _method_name = "tecs.cmm.query";
        _user_id = -1;
        _upool = upool;
    }
    CmmConfigQueryMethod() {}
    void MethodEntry(const xmlrpc_c::paramList& paramList,
                     xmlrpc_c::value* const retval);    
private:
    DISALLOW_COPY_AND_ASSIGN(CmmConfigQueryMethod);
};

class ClusterConfigSetSaveEnergyParaMethod : public TecsRpcMethod
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
        ERR_MAP(ERR_CLUSTER_CFG_NAME_INVALID,TECS_ERR_CLUSTER_CFG_NAME_INVALID)
        ERR_MAP(ERR_CLUSTER_CFG_SET_FAILD,  TECS_ERR_CLUSTER_CFG_SET_FAILD)
        ERR_MAP_END
    }
    ClusterConfigSetSaveEnergyParaMethod(UserPool * upool = 0)
    {
        _help = "Set host save energy info";
        _method_name = "tecs.save_energy.set";
        _user_id = -1;
        _upool = upool;
    }
    ClusterConfigSetSaveEnergyParaMethod() {}
    void MethodEntry(const xmlrpc_c::paramList& paramList,
                     xmlrpc_c::value* const retval);    
private:
    DISALLOW_COPY_AND_ASSIGN(ClusterConfigSetSaveEnergyParaMethod);
};


class ClusterConfigSaveEnergyParaQueryMethod : public TecsRpcMethod
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
        ERR_MAP_END
    }
    ClusterConfigSaveEnergyParaQueryMethod(UserPool * upool = 0)
    {
        _help = "Query host save energy info";
        _method_name = "tecs.save_energy.query";
        _user_id = -1;
        _upool = upool;
    }
    ClusterConfigSaveEnergyParaQueryMethod() {}
    void MethodEntry(const xmlrpc_c::paramList& paramList,
                     xmlrpc_c::value* const retval);    
private:
    DISALLOW_COPY_AND_ASSIGN(ClusterConfigSaveEnergyParaQueryMethod);
};

class SchedulerPolicySetMethod : public TecsRpcMethod
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
        ERR_MAP_END
    }
    SchedulerPolicySetMethod(UserPool * upool = 0)
    {
        _help = "Set scheduler policy for cluster";
        _method_name = "tecs.scheduler.set_policy";
        _user_id = -1;
        _upool = upool;
    }
    SchedulerPolicySetMethod() {}
    void MethodEntry(const xmlrpc_c::paramList& paramList,
                     xmlrpc_c::value* const retval);    
private:
    DISALLOW_COPY_AND_ASSIGN(SchedulerPolicySetMethod);
};

class SchedulerPolicyQueryMethod : public TecsRpcMethod
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
        ERR_MAP_END
    }
    SchedulerPolicyQueryMethod(UserPool * upool = 0)
    {
        _help = "Show scheduler policy for cluster";
        _method_name = "tecs.scheduler.query_policy";
        _user_id = -1;
        _upool = upool;
    }
    SchedulerPolicyQueryMethod() {}
    void MethodEntry(const xmlrpc_c::paramList& paramList,
                     xmlrpc_c::value* const retval);    
private:
    DISALLOW_COPY_AND_ASSIGN(SchedulerPolicyQueryMethod);
};

class VMHAPolicySetMethod : public TecsRpcMethod
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
        ERR_MAP_END
    }
    VMHAPolicySetMethod(UserPool * upool = 0)
    {
        _help = "Set VM high available policy for cluster";
        _method_name = "tecs.vm_ha.set_policy";
        _user_id = -1;
        _upool = upool;
    }
    VMHAPolicySetMethod() {}
    void MethodEntry(const xmlrpc_c::paramList& paramList,
                     xmlrpc_c::value* const retval);    
private:
    DISALLOW_COPY_AND_ASSIGN(VMHAPolicySetMethod);
};

class VMHAPolicyQueryMethod : public TecsRpcMethod
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
        ERR_MAP_END
    }
    VMHAPolicyQueryMethod(UserPool * upool = 0)
    {
        _help = "Show VM high available policy for cluster";
        _method_name = "tecs.vm_ha.query_policy";
        _user_id = -1;
        _upool = upool;
    }
    VMHAPolicyQueryMethod() {}
    void MethodEntry(const xmlrpc_c::paramList& paramList,
                     xmlrpc_c::value* const retval);    
private:
    DISALLOW_COPY_AND_ASSIGN(VMHAPolicyQueryMethod);
};

} // namespace zte_tecs

#endif  //#ifndef RM_TECS_HOST_MANAGER_METHODS_H

