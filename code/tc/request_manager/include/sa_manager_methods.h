/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：tecs_sa_manager_methods.h
* 文件标识：见配置管理计划书
* 内容摘要：物理机管理模块涉及的XML-RPC method类定义文件
* 当前版本：1.0
* 作    者：冯骏
* 完成日期：2012年11月20日
*
* 修改记录1：
*     修改日期：2012/11/20
*     版 本 号：V1.0
*     修 改 人：冯骏
*     修改内容：创建
*******************************************************************************/
#ifndef RM_TECS_SA_MANAGER_METHODS_H
#define RM_TECS_SA_MANAGER_METHODS_H

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

 
class SaDisableMethod : public TecsRpcMethod
{
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP(SUCCESS,                    TECS_SUCCESS)
        ERR_MAP(ERR_SC_AUTHORIZE_FAILED,    TECS_ERR_AUTHORIZE_FAILED)
        ERR_MAP(ERR_SC_PARA_INVALID,        TECS_ERR_PARA_INVALID)
        ERR_MAP(ERR_SC_TIMEOUT,             TECS_ERR_SC_TIMEOUT)       
        ERR_MAP(ERROR_OBJECT_NOT_EXIST,     TECS_ERR_SA_NOT_EXIST)  
        ERR_MAP(ERROR_DB_UPDATE_FAIL,       TECS_ERR_SC_DB_UPDATE_FAIL)          
        ERR_MAP_END
    }
    SaDisableMethod(UserPool * upool = 0)
    {
        _help = "Disable a sa of the cluster";
        _method_name = "tecs.storage.disable";
        _user_id = -1;
        _upool = upool;
    }
    SaDisableMethod() {}
    void MethodEntry(const xmlrpc_c::paramList& paramList,
                     xmlrpc_c::value* const retval);    
private:
    DISALLOW_COPY_AND_ASSIGN(SaDisableMethod);
};

class SaEnableMethod : public TecsRpcMethod
{
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP(SUCCESS,                    TECS_SUCCESS)
        ERR_MAP(ERR_SC_AUTHORIZE_FAILED,    TECS_ERR_AUTHORIZE_FAILED)
        ERR_MAP(ERR_SC_PARA_INVALID,        TECS_ERR_PARA_INVALID)
        ERR_MAP(ERR_SC_TIMEOUT,             TECS_ERR_SC_TIMEOUT)
        ERR_MAP(ERROR_OBJECT_NOT_EXIST,     TECS_ERR_SA_NOT_EXIST)  
        ERR_MAP(ERROR_DB_UPDATE_FAIL,       TECS_ERR_SC_DB_UPDATE_FAIL)   
        ERR_MAP_END
    }
    SaEnableMethod(UserPool * upool = 0)
    {
        _help = "Enable a sa of the sc";
        _method_name = "tecs.storage.enable";
        _user_id = -1;
        _upool = upool;
    }
    SaEnableMethod() {}
    void MethodEntry(const xmlrpc_c::paramList& paramList,
                     xmlrpc_c::value* const retval);    
private:
    DISALLOW_COPY_AND_ASSIGN(SaEnableMethod);
};


class SaForgetIdMethod : public TecsRpcMethod
{
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP(SUCCESS,                    TECS_SUCCESS)
        ERR_MAP(ERR_SC_AUTHORIZE_FAILED,    TECS_ERR_AUTHORIZE_FAILED)
        ERR_MAP(ERR_SC_PARA_INVALID,        TECS_ERR_PARA_INVALID)
        ERR_MAP(ERR_SC_TIMEOUT,             TECS_ERR_SC_TIMEOUT)   
        ERR_MAP(ERROR_OBJECT_NOT_EXIST,     TECS_ERR_SA_NOT_EXIST)  
        ERR_MAP(ERROR_DB_DELETE_FAIL,       TECS_ERR_SC_DB_DELETE_FAIL) 
        ERR_MAP(ERROR_PERMISSION_DENIED,    TECS_ERR_SA_BUSY) 
        ERR_MAP_END
    }
    SaForgetIdMethod(UserPool * upool = 0)
    {
        _help = "Forget a forbidden or unegistered sa of the sc";
        _method_name = "storage.adaptor.forget.sid";
        _user_id = -1;
        _upool = upool;
    }
    SaForgetIdMethod() {}
    void MethodEntry(const xmlrpc_c::paramList& paramList,
                     xmlrpc_c::value* const retval);    
private:
    DISALLOW_COPY_AND_ASSIGN(SaForgetIdMethod);
};

class SaMapClusterMethod : public TecsRpcMethod
{
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP(SUCCESS,                    TECS_SUCCESS)
        ERR_MAP(ERR_SC_AUTHORIZE_FAILED,    TECS_ERR_AUTHORIZE_FAILED)
        ERR_MAP(ERR_SC_PARA_INVALID,        TECS_ERR_PARA_INVALID)
        ERR_MAP(ERR_SC_TIMEOUT,             TECS_ERR_SC_TIMEOUT)   
        ERR_MAP(ERROR_DB_INSERT_FAIL,       TECS_ERR_SC_DB_INSERT_FAIL)  
        ERR_MAP(ERROR_DB_DELETE_FAIL,       TECS_ERR_SC_DB_DELETE_FAIL)        
        ERR_MAP(ERR_EXIST,                  TECS_ERR_SC_MAP_EXIST)     
        ERR_MAP(ERR_OBJECT_NOT_EXIST,       TECS_ERR_SC_MAP_NOT_EXIST)     
        ERR_MAP_END
    }
    SaMapClusterMethod(UserPool * upool = 0)
    {
        _help = "set the map between sa and cluster";
        _method_name = "tecs.storage.cluster.map";
        _user_id = -1;
        _upool = upool;
    }
    SaMapClusterMethod() {}
    void MethodEntry(const xmlrpc_c::paramList& paramList,
                     xmlrpc_c::value* const retval);    
private:
    DISALLOW_COPY_AND_ASSIGN(SaMapClusterMethod);
};

}

#endif

