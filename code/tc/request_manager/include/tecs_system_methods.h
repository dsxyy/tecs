/*******************************************************************************
* Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�tecs_system_methods.h
* �ļ���ʶ��
* ����ժҪ��tecs system��XML-RPCע�᷽����
* ��ǰ�汾��1.0
* ��    �ߣ����Ľ�
* ������ڣ�2012��7��5��
*
* �޸ļ�¼1��
*    �޸����ڣ�2012��7��5��
*    �� �� �ţ�V1.0
*    �� �� �ˣ����Ľ�
*    �޸����ݣ�����
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
@brief ��������: ϵͳ����ʱ��Ϣ��ѯ
@li @b ����˵��: ��
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
@brief ��������: ϵͳ���֤��Ϣ��ѯ
@li @b ����˵��: ��
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
@brief ��������: ϵͳ���֤��Ϣ��ѯ
@li @b ����˵��: ��
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

