/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：cluster_config.h
* 文件标识：见配置管理计划书
* 内容摘要：cluster管理XMl配置接口申明
* 当前版本：1.0
* 作    者：李孝成
* 完成日期：2011年7月25日
*
* 修改记录1：
*     修改日期：2011/7/25
*     版 本 号：V1.0
*     修 改 人：李孝成
*     修改内容：创建
******************************************************************************/
#ifndef TC_CLUSTER_MANAGER_METHODS_H
#define TC_CLUSTER_MANAGER_METHODS_H

#include "sky.h"
#include "cluster_pool.h"
#include "request_manager.h"
#include "tecs_rpc_method.h"
#include "user_pool.h"
#include "mid.h"
#include "event.h"
#include "api_error.h"
#include "tecs_errcode.h"

namespace zte_tecs
{

class ClusterSet:public TecsRpcMethod {
public:
    
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERR_CM_CREATE_FAIL,TECS_ERR_CM_CREATE_FAIL)
        ERR_MAP(ERR_CM_CLUSTER_REGISTERD,TECS_ERR_CM_CLUSTER_REGISTERD)
        ERR_MAP(ERR_CM_CLUSTER_NOT_EXIST,TECS_ERR_CM_CLUSTER_NOT_EXIST)
        ERR_MAP(ERR_CM_CLUSTER_NOT_FREE,TECS_ERR_CM_CLUSTER_NOT_FREE)
        ERR_MAP_END
    }

    ClusterSet(UserPool *upool=0)
    {
        _method_name = "ClusterSetMethod";
        _user_id = -1;
        _upool = upool;
    };

    ~ClusterSet(){};

/*****************************************************************************/
/**
@brief 功能描述: 集群注销XML方法接口

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
                      xmlrpc_c::value *   const  retval);
        
private:

};


class ClusterForget:public TecsRpcMethod {
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERR_CM_CREATE_FAIL,TECS_ERR_CM_CREATE_FAIL)
        ERR_MAP(ERR_CM_CLUSTER_REGISTERD,TECS_ERR_CM_CLUSTER_REGISTERD)
        ERR_MAP(ERR_CM_CLUSTER_NOT_EXIST,TECS_ERR_CM_CLUSTER_NOT_EXIST)
        ERR_MAP(ERR_CM_CLUSTER_NOT_FREE,TECS_ERR_CM_CLUSTER_NOT_FREE)
        ERR_MAP_END
    }
    
    ClusterForget(UserPool *upool=0)
    {
        _method_name = "ClusterForgetMethod";
        _user_id = -1;
        _upool = upool;
    };

    ~ClusterForget (){};

/*****************************************************************************/
/**
@brief 功能描述: 集群注册XML方法接口

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
                      xmlrpc_c::value *   const  retval);
        
private:
 
};

class ClusterQuery : public TecsRpcMethod 
{
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERR_CM_CREATE_FAIL,TECS_ERR_CM_CREATE_FAIL)
        ERR_MAP(ERR_CM_CLUSTER_REGISTERD,TECS_ERR_CM_CLUSTER_REGISTERD)
        ERR_MAP(ERR_CM_CLUSTER_NOT_EXIST,TECS_ERR_CM_CLUSTER_NOT_EXIST)
        ERR_MAP(ERR_CM_CLUSTER_NOT_FREE,TECS_ERR_CM_CLUSTER_NOT_FREE)
        ERR_MAP_END
    }

    
    ClusterQuery (UserPool *upool=0)
    {
        _method_name = "ClusterQueryMethod";
        _user_id = -1;
        _upool = upool;
    };

    ~ClusterQuery (){};

/*****************************************************************************/
/**
@brief 功能描述: 集群注册XML方法接口

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


class ClusterQueryCoreAndTcu : public TecsRpcMethod 
{
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP_END
    }

    
    ClusterQueryCoreAndTcu (UserPool *upool=0)
    {
        _method_name = "ClusterQueryCoreAndTcuMethod";
        _user_id = -1;
        _upool = upool;
    };

    ~ClusterQueryCoreAndTcu (){};

/*****************************************************************************/
/**
@brief 功能描述: 集群注册XML方法接口

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

class ClusterQueryNetPlane : public TecsRpcMethod 
{
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP_END
    }

    
    ClusterQueryNetPlane (UserPool *upool=0)
    {
        _method_name = "ClusterQueryNetPlaneMethod";
        _user_id = -1;
        _upool = upool;
    };

    ~ClusterQueryNetPlane (){};

/*****************************************************************************/
/**
@brief 功能描述: 集群注册XML方法接口

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



class ClusterPreserveStart : public TecsRpcMethod 
{
public:

    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERR_CM_CREATE_FAIL,TECS_ERR_CM_CREATE_FAIL)
        ERR_MAP(ERR_CM_CLUSTER_REGISTERD,TECS_ERR_CM_CLUSTER_REGISTERD)
        ERR_MAP(ERR_CM_CLUSTER_NOT_EXIST,TECS_ERR_CM_CLUSTER_NOT_EXIST)
        ERR_MAP(ERR_CM_CLUSTER_NOT_FREE,TECS_ERR_CM_CLUSTER_NOT_FREE)
        ERR_MAP_END
    }
    
    ClusterPreserveStart (UserPool *upool=0)
    {
        _method_name = "ClusterPreserveStart";
        _user_id = -1;
        _upool = upool;
    };

    ~ClusterPreserveStart (){};

/*****************************************************************************/
/**
@brief 功能描述: 集群开始维护XML方法接口

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

class ClusterPreserveStop : public TecsRpcMethod 
{
public:

    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERR_CM_CREATE_FAIL,TECS_ERR_CM_CREATE_FAIL)
        ERR_MAP(ERR_CM_CLUSTER_REGISTERD,TECS_ERR_CM_CLUSTER_REGISTERD)
        ERR_MAP(ERR_CM_CLUSTER_NOT_EXIST,TECS_ERR_CM_CLUSTER_NOT_EXIST)
        ERR_MAP(ERR_CM_CLUSTER_NOT_FREE,TECS_ERR_CM_CLUSTER_NOT_FREE)
        ERR_MAP_END
    }
    
    ClusterPreserveStop (UserPool *upool=0)
    {
        _method_name = "ClusterPreserveStop";
        _user_id = -1;
        _upool = upool;
    };

    ~ClusterPreserveStop (){};

/*****************************************************************************/
/**
@brief 功能描述: 集群开始维护XML方法接口

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




#endif //end TC_CLUSTER_MANAGER_METHODS_H
