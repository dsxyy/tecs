/*******************************************************************************
* Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：tecs_system_methods.h
* 文件标识：
* 内容摘要：tecs system的XML-RPC注册方法类
* 当前版本：1.0
* 作    者：张文剑
* 完成日期：2012年7月5日
*
* 修改记录1：
*    修改日期：2012年7月5日
*    版 本 号：V1.0
*    修 改 人：张文剑
*    修改内容：创建
*******************************************************************************/
#ifndef TECS_SYSTEM_METHODS_H        
#define TECS_SYSTEM_METHODS_H
#include "request_manager.h"
#include "tecs_rpc_method.h"
#include "user_pool.h"
#include "api_error.h"
#include "tecs_errcode.h"
#include "sysinfo.h"
#include "system_api.h"

namespace zte_tecs
{
/**
@brief 功能描述: 系统运行时信息查询
@li @b 其它说明: 无
*/
class SysRuntimeInfoQueryMethod :public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    SysRuntimeInfoQueryMethod (UserPool *upool = 0)
    {
        _method_name = "SysRuntimeInfoQueryMethod";
        _user_id = -1;
        _upool = upool;
    };
	
    ~SysRuntimeInfoQueryMethod(){};

    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERROR_TIME_OUT,                 TECS_ERR_TIMEOUT) 
        ERR_MAP(ERR_AUTHORIZE_FAILED,           TECS_ERR_AUTHORIZE_FAILED) 
        ERR_MAP(ERROR_INVALID_ARGUMENT,         TECS_ERR_PARA_INVALID) 
        ERR_MAP_END
    }

    virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,xmlrpc_c::value  *const retval);
	
    static STATUS Query(const string &appname,const string &hopname,const string &hoppro,MessageUnit &mu,AppRuntimeInfoReq &req,AppRuntimeInfo& app_running_info);	
	
    static STATUS GetCalcNodeInfo(MessageUnit &mu,const string& target,AppRuntimeInfo& app_running_info);	
	
    static STATUS GetStorageNodeInfo(MessageUnit &mu,const string& target,AppRuntimeInfo& app_running_info);	
	
    static STATUS GetNetWorkNodeInfo(MessageUnit &mu,const string& target,AppRuntimeInfo& app_running_info);
    static STATUS GetSysInfo(const string& type,const string& target,AppRuntimeInfo& app_running_info);
	
private:
    DISALLOW_COPY_AND_ASSIGN(SysRuntimeInfoQueryMethod); 
};

/**
@brief 功能描述: 系统许可证信息查询
@li @b 其它说明: 无
*/
class LicenseQueryMethod :public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    LicenseQueryMethod (UserPool *upool = 0)
    {
        _method_name = "LicenseQueryMethod";
        _user_id = -1;
        _upool = upool;
    };
	
    ~LicenseQueryMethod(){};

    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERROR_TIME_OUT,                 TECS_ERR_TIMEOUT) 
        ERR_MAP_END
    }

    virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,xmlrpc_c::value  *const retval);
private:
    DISALLOW_COPY_AND_ASSIGN(LicenseQueryMethod); 
};

/**
@brief 功能描述: 系统许可证信息查询
@li @b 其它说明: 无
*/
class HardwareFingerprintQueryMethod :public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    HardwareFingerprintQueryMethod (UserPool *upool = 0)
    {
        _method_name = "HardwareFingerprintQueryMethod";
        _user_id = -1;
        _upool = upool;
    };
    
    ~HardwareFingerprintQueryMethod(){};

    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERROR_NOT_PERMITTED,                 TECS_ERR_AUTHORIZE_FAILED) 
        ERR_MAP_END
    }

    virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,xmlrpc_c::value  *const retval);
private:
    DISALLOW_COPY_AND_ASSIGN(HardwareFingerprintQueryMethod); 
};
}
#endif /* TECS_SYSTEM_METHODS_H */

