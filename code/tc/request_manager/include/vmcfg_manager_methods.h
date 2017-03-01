/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：vmcfg_manager_method.h
* 文件标识：
* 内容摘要：vmcfg的XML-RPC注册方法类
* 当前版本：1.0
* 作    者：Bob
* 完成日期：2011年9月7日
*
* 修改记录1：
*    修改日期：2011年9月7日
*    版 本 号：V1.0
*    修 改 人：Bob
*    修改内容：创建
*******************************************************************************/
#ifndef VMCFG_MANAGER_METHODS_H        
#define VMCFG_MANAGER_METHODS_H
#include "request_manager.h"
#include "tecs_rpc_method.h"
#include "user_pool.h"
#include "api_error.h"
#include "tecs_errcode.h"
#include "vmcfg_pool.h"
#include "vmcfg_operate.h"
#include "db_config.h"
#include "cluster_manager_with_vm_config.h"
#include "workflow.h"
#include "vnet_libnet.h"

namespace zte_tecs
{

/**
@brief 功能描述: 虚拟机配置管理的实体类\n
@li @b 其它说明: 一次传入所有参数
*/
class VmCfgAllocateMethod :public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERROR_TIME_OUT,                 TECS_ERR_TIMEOUT) 
        ERR_MAP(ERR_AUTHORIZE_FAILED,           TECS_ERR_AUTHORIZE_FAILED) 
        ERR_MAP(ERROR_INVALID_ARGUMENT,         TECS_ERR_PARA_INVALID) 
        ERR_MAP(ERR_VMCFG_ACQUIRE_IMAGE_FAILED, TECS_ERR_VMCFG_ACQUIRE_IMAGE_FAILED)
        ERR_MAP(ERR_VMCFG_INVALID_PROJECT,      TECS_ERR_VMCFG_INVALID_PROJECT) 
        ERR_MAP(ERR_OBJECT_ALLOCATE_FAILED,     TECS_ERR_OBJECT_ALLOCATE_FAILED)
        ERR_MAP_END
    }
    
    VmCfgAllocateMethod (UserPool *upool=0)
    {
        _method_name = "VmCfgAllocateMethod";
        _user_id = -1;
        _upool = upool;
    };
    
    ~VmCfgAllocateMethod (){};

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                                 xmlrpc_c::value  *const retval);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(VmCfgAllocateMethod); 

};

class VmCfgAllocateByVtMethod :public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERROR_TIME_OUT,                 TECS_ERR_TIMEOUT)
        ERR_MAP(ERR_AUTHORIZE_FAILED,           TECS_ERR_AUTHORIZE_FAILED)
        ERR_MAP(ERROR_INVALID_ARGUMENT,         TECS_ERR_PARA_INVALID)
        ERR_MAP(ERR_VMCFG_ACQUIRE_IMAGE_FAILED, TECS_ERR_VMCFG_ACQUIRE_IMAGE_FAILED)
        ERR_MAP(ERR_VMCFG_INVALID_PROJECT,      TECS_ERR_VMCFG_INVALID_PROJECT)
        ERR_MAP(ERR_OBJECT_ALLOCATE_FAILED,     TECS_ERR_OBJECT_ALLOCATE_FAILED)
        ERR_MAP_END
    }

    VmCfgAllocateByVtMethod (UserPool *upool=0)
    {
        _method_name = "VmCfgAllocateByVtMethod";
        _user_id = -1;
        _upool = upool;
    };

    ~VmCfgAllocateByVtMethod (){};

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                                 xmlrpc_c::value  *const retval);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(VmCfgAllocateByVtMethod);

};

/**
@brief 功能描述: 映像管理的实体类\n
@li @b 其它说明: 无
*/
class VmCfgSetMethod :public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERROR_TIME_OUT,           TECS_ERR_TIMEOUT) 
        ERR_MAP(ERR_AUTHORIZE_FAILED,     TECS_ERR_AUTHORIZE_FAILED) 
        ERR_MAP(ERROR_INVALID_ARGUMENT,   TECS_ERR_PARA_INVALID) 
        ERR_MAP(ERR_OBJECT_NOT_EXIST,     TECS_ERR_OBJECT_NOT_EXIST)
        ERR_MAP(ERR_OBJECT_UPDATE_FAILED, TECS_ERR_OBJECT_UPDATE_FAILED)
        ERR_MAP(ERR_VMCFG_INVALID_PROJECT, TECS_ERR_VMCFG_INVALID_PROJECT)
        ERR_MAP(ERR_VMCFG_ACQUIRE_IMAGE_FAILED, TECS_ERR_VMCFG_ACQUIRE_IMAGE_FAILED)
        ERR_MAP(ERR_VMCFG_MODIFY_FORBIDDEN, TECS_ERR_VMCFG_MODIFY_FORBIDDEN)
        ERR_MAP_END
    }
    VmCfgSetMethod (UserPool *upool=0)
    {
        _method_name = "VmCfgSetMethod";
        _user_id = -1;
        _upool = upool;
    };
    ~VmCfgSetMethod (){};

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                         xmlrpc_c::value  *const retval);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(VmCfgSetMethod); 

};

class VmCfgModifyMethod :public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERROR_TIME_OUT,           TECS_ERR_TIMEOUT)
        ERR_MAP(ERR_AUTHORIZE_FAILED,     TECS_ERR_AUTHORIZE_FAILED)
        ERR_MAP(ERROR_INVALID_ARGUMENT,   TECS_ERR_PARA_INVALID)
        ERR_MAP(ERR_OBJECT_NOT_EXIST,     TECS_ERR_OBJECT_NOT_EXIST)
        ERR_MAP(ERR_OBJECT_UPDATE_FAILED, TECS_ERR_OBJECT_UPDATE_FAILED)
        ERR_MAP(ERR_VMCFG_INVALID_PROJECT, TECS_ERR_VMCFG_INVALID_PROJECT)
        ERR_MAP(ERR_VMCFG_ACQUIRE_IMAGE_FAILED, TECS_ERR_VMCFG_ACQUIRE_IMAGE_FAILED)
        ERR_MAP(ERR_VMCFG_MODIFY_FORBIDDEN, TECS_ERR_VMCFG_MODIFY_FORBIDDEN)
        ERR_MAP_END
    }
    VmCfgModifyMethod (UserPool *upool=0)
    {
        _method_name = "VmCfgModifyMethod";
        _user_id = -1;
        _upool = upool;
    };
    ~VmCfgModifyMethod(){};

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                         xmlrpc_c::value  *const retval);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(VmCfgModifyMethod);

};

/**
@brief 功能描述: 映像管理的实体类\n
@li @b 其它说明: 无
*/
class VmCfgQueryMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:

    VmCfgQueryMethod (){};
    ~VmCfgQueryMethod (){};

   void  SendQueryMsg(const QuerySession &query_session,
                                       MessageUnit &mu);
   STATUS WaitQueryAckMsg(QuerySession &query_session,
                                    MessageUnit &mu,
                                   ostringstream &err_oss);
   void DealStateQueryAck(VmQueryAck  &ack,
                                      QuerySession &query_session,
                                      MessageUnit &mu);

   void DealVmNicQueryAck(CQueryVmNicInfoMsgAck  &ack,
                                      QuerySession &query_session,
                                      MessageUnit &mu);


};

/**
@brief 功能描述: 映像管理的实体类\n
@li @b 其它说明: 无
*/
class VmStaticInfoQueryMethod :public VmCfgQueryMethod, public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERROR_TIME_OUT,            TECS_ERR_TIMEOUT) 
        ERR_MAP(ERR_AUTHORIZE_FAILED,      TECS_ERR_AUTHORIZE_FAILED) 
        ERR_MAP(ERROR_INVALID_ARGUMENT,   TECS_ERR_PARA_INVALID) 
        ERR_MAP(ERR_VMCFG_INVALID_QUERY_TYPE, TECS_ERR_VMCFG_INVALID_QUERY_TYPE )
        ERR_MAP(ERROR_DB_SELECT_FAIL,   TECS_ERR_DB_QUERY_FAILED)
        ERR_MAP_END
    }

    VmStaticInfoQueryMethod (UserPool *upool=0)
    {
        _method_name = "VmCfgStaticInfoQueryMethod";
        _user_id = -1;
        _upool = upool;
    };
    ~VmStaticInfoQueryMethod (){};

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                         xmlrpc_c::value  *const retval);
/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(VmStaticInfoQueryMethod);

};

/**
@brief 功能描述: 映像管理的实体类\n
@li @b 其它说明: 无
*/
class VmRunningInfoQueryMethod :public VmCfgQueryMethod, public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERROR_TIME_OUT,            TECS_ERR_TIMEOUT)
        ERR_MAP(ERR_AUTHORIZE_FAILED,      TECS_ERR_AUTHORIZE_FAILED)
        ERR_MAP(ERROR_INVALID_ARGUMENT,   TECS_ERR_PARA_INVALID)
        ERR_MAP(ERR_VMCFG_INVALID_QUERY_TYPE, TECS_ERR_VMCFG_INVALID_QUERY_TYPE )
        ERR_MAP(ERROR_DB_SELECT_FAIL,   TECS_ERR_DB_QUERY_FAILED)
        ERR_MAP_END
    }
    VmRunningInfoQueryMethod (UserPool *upool=0)
    {
        _method_name = "VmRunningInfoQueryMethod";
        _user_id = -1;
        _upool = upool;
    };
    ~VmRunningInfoQueryMethod(){};
                                   
    virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                         xmlrpc_c::value  *const retval);
                                      
/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

    DISALLOW_COPY_AND_ASSIGN(VmRunningInfoQueryMethod);

};

#if 0
/**
@brief 功能描述: 查询虚拟机完整信息\n
@li @b 其它说明: 无
*/
class VmCfgFullQueryMethod :public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERROR_TIME_OUT,            TECS_ERR_TIMEOUT) 
        ERR_MAP(ERR_AUTHORIZE_FAILED,      TECS_ERR_AUTHORIZE_FAILED) 
        ERR_MAP(ERROR_INVALID_ARGUMENT,   TECS_ERR_PARA_INVALID) 
        ERR_MAP(ERR_VMCFG_INVALID_QUERY_TYPE, TECS_ERR_VMCFG_INVALID_QUERY_TYPE )
        ERR_MAP(ERROR_DB_SELECT_FAIL,   TECS_ERR_DB_QUERY_FAILED)
        ERR_MAP_END
    }
    VmCfgFullQueryMethod (UserPool *upool=0)
    {
        _method_name = "VmCfgFullQueryMethod";
        _user_id = -1;
        _upool = upool;
    };
    ~VmCfgFullQueryMethod (){};

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                         xmlrpc_c::value  *const retval);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:
   DISALLOW_COPY_AND_ASSIGN(VmCfgFullQueryMethod); 
};
#endif

/**
@brief 功能描述: 根据虚拟机名称查询虚拟机id\n
@li @b 其它说明: 无
*/
class VmCfgGetIdByNameMethod :public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERROR_TIME_OUT,            TECS_ERR_TIMEOUT) 
        ERR_MAP(ERR_AUTHORIZE_FAILED,      TECS_ERR_AUTHORIZE_FAILED) 
        ERR_MAP(ERR_VMCFG_INVALID_QUERY_TYPE, TECS_ERR_VMCFG_INVALID_QUERY_TYPE )
        ERR_MAP(ERROR_DB_SELECT_FAIL,   TECS_ERR_DB_QUERY_FAILED)
        ERR_MAP_END
    }
    VmCfgGetIdByNameMethod (UserPool *upool=0)
    {
        _method_name = "VmCfgGetIdByNameMethod";
        _user_id = -1;
        _upool = upool;
    };
    ~VmCfgGetIdByNameMethod (){};

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                         xmlrpc_c::value  *const retval);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:
   DISALLOW_COPY_AND_ASSIGN(VmCfgGetIdByNameMethod); 
};

/**
@brief 功能描述: 映像管理的实体类\n
@li @b 其它说明: 无
*/
class VmCfgActionMethod :public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    RPC_ERROR_MAP
    { 
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERROR_TIME_OUT,            TECS_ERR_TIMEOUT) 
        ERR_MAP(ERR_AUTHORIZE_FAILED,      TECS_ERR_AUTHORIZE_FAILED) 
        ERR_MAP(ERR_OBJECT_NOT_EXIST,          TECS_ERR_OBJECT_NOT_EXIST)
        ERR_MAP(ERR_VMCFG_INVALID_ACTION_TYPE, TECS_ERR_VMCFG_INVALID_ACTION_TYPE)
        ERR_MAP(ERR_VMCFG_STATE_FORBIDDEN,     TECS_ERR_VMCFG_STATE_FORBIDDEN)
        ERR_MAP(ERR_VMCFG_NO_SUITABLE_CLUSTER, TECS_ERR_VMCFG_NO_SUITABLE_CLUSTER)
        ERR_MAP(ERR_VMCFG_ACTION_FAILED_IN_CC, TECS_ERR_VMCFG_ACTION_FAILED_IN_CC) 
        ERR_MAP(ERR_VMCFG_ACTION_FAILED_IN_HC, TECS_ERR_VMCFG_ACTION_FAILED_IN_HC)
        ERR_MAP(ERROR_DB_DELETE_FAIL,          TECS_ERR_DB_DELETE_FAILED)
        ERR_MAP(ERROR_NOT_READY,               TECS_ERR_VMCFG_STATE_FORBIDDEN)
        ERR_MAP(ERR_VMCFG_DEPLOY_IMAGE_FAILED, TECS_ERR_VMCFG_DEPLOY_IMAGE_FAILED)
        ERR_MAP(ERR_VMCFG_IMAGE_DEPLOYING,     TECS_ERR_VMCFG_IMAGE_DEPLOYING)
        ERR_MAP(ERR_VMCFG_LACK_COMPUTE,        TECS_ERR_VMCFG_LACK_COMPUTE )
        ERR_MAP(ERR_VMCFG_LACK_SHARE_STORAGE,  TECS_ERR_VMCFG_LACK_SHARE_STORAGE )
        ERR_MAP(ERR_VMCFG_LACK_NETWORK,        TECS_ERR_VMCFG_LACK_NETWORK )
        ERR_MAP(ERR_VMCFG_GET_COREDUMP_URL,    TECS_ERR_VMCFG_GET_COREDUMP_URL )
        ERR_MAP(ERR_VMCFG_PREPARE_DEPLOY,      TECS_ERR_VMCFG_PREPARE_DEPLOY )
        ERR_MAP(ERR_VMCFG_ALLOCATE_NETWORK_TC, TECS_ERR_VMCFG_ALLOCATE_NETWORK_TC )
        ERR_MAP(ERROR_DB_UNKNOWN_FAIL,         TECS_ERR_DB_UNKNOWN_FAILED)
        ERR_MAP(ERROR_ACTION_RUNNING,          TECS_SUCCESS)
        ERR_MAP_END
    }
    VmCfgActionMethod (UserPool *upool=0)
    {
        _method_name = "VmCfgActionMethod";
        _user_id = -1;
        _upool = upool;
    };
    ~VmCfgActionMethod (){};

    virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                         xmlrpc_c::value  *const retval);

    STATUS DealOpDeploy(int64         vid, string& workflow_id,
                             ostringstream   &err_oss,
                             string &workflow_engine);  

    STATUS DealOpCancel(int64         vid, string& workflow_id,
                             ostringstream   &err_oss,
                             string &workflow_engine);
    STATUS DeployImg2ShareStorage(const VmCfg    &vmcfg,
                                   const string   &des_cluster,
                                   int64          uid,
                                   ostringstream  &err_oss);         

    STATUS DealOpUpLoadImg(int64         vid,
                          const string  &session,
                          string& workflow_id,
                          ostringstream &err_oss,
                          string &workflow_engine); 

    STATUS DealOpSync(int64         vid, string& workflow_id,
                             ostringstream   &err_oss);  

    STATUS DealOpNormal(int64         vid,
                              VmOperation   op,  string& workflow_id,
                              ostringstream &err_oss,
                              string &workflow_engine);
                                     
    STATUS SendSync2Cluster(VmCfg    &vmcfg,
                                     const string  &cluster_name,
                                     string& workflow_id,
                                     ostringstream &err_oss);

    STATUS ScheduleCluster(VmCfg          &vmcfg, 
                                 string        &candidate_cluster,
                                 ostringstream &err_oss);
   
    STATUS SendUpLoadImg2Cluster(int64 vid,
                             const UploadUrl &img_url,
                             const string    &cluster,
                             ostringstream   &err_oss,
                             string &workflow_id,
                             string &workflow_engine);
                                     
    STATUS GetCoredumpUrl(
                                 UploadUrl  &core_dump_url,
                                 ostringstream &err_oss);

    STATUS GetUpLoadUrl(
                                 UploadUrl  &img_url,
                                 ostringstream &err_oss);

    STATUS DealClusterAck(VmOperationAck  &ack, 
                         const string    &cc_name,
                         string& workflow_id,
                         ostringstream   &err_oss);  
   
/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(VmCfgActionMethod); 

};


/**
@brief 功能描述: 映像管理的实体类\n
@li @b 其它说明: 无
*/
class VmCfgSetDiskMethod :public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERROR_TIME_OUT,           TECS_ERR_TIMEOUT) 
        ERR_MAP(ERR_AUTHORIZE_FAILED,     TECS_ERR_AUTHORIZE_FAILED) 
        ERR_MAP(ERROR_INVALID_ARGUMENT,   TECS_ERR_PARA_INVALID) 
        ERR_MAP(ERR_OBJECT_NOT_EXIST,     TECS_ERR_OBJECT_NOT_EXIST)
        ERR_MAP(ERR_OBJECT_UPDATE_FAILED, TECS_ERR_OBJECT_UPDATE_FAILED)
        ERR_MAP(ERR_VMCFG_MODIFY_FORBIDDEN, TECS_ERR_VMCFG_MODIFY_FORBIDDEN)
        ERR_MAP_END
    };

    VmCfgSetDiskMethod (UserPool *upool=0)
    {
        _method_name = "VmCfgSetDiskMethod";
        _user_id = -1;
        _upool = upool;
    };
    ~VmCfgSetDiskMethod(){};

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                         xmlrpc_c::value  *const retval);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(VmCfgSetDiskMethod); 

};


/**
@brief 功能描述: 映像管理的实体类\n
@li @b 其它说明: 无
*/
class VmCfgSetImageDiskMethod :public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERROR_TIME_OUT,           TECS_ERR_TIMEOUT) 
        ERR_MAP(ERR_AUTHORIZE_FAILED,     TECS_ERR_AUTHORIZE_FAILED) 
        ERR_MAP(ERROR_INVALID_ARGUMENT,   TECS_ERR_PARA_INVALID) 
        ERR_MAP(ERR_OBJECT_NOT_EXIST,     TECS_ERR_OBJECT_NOT_EXIST)
        ERR_MAP(ERR_OBJECT_UPDATE_FAILED, TECS_ERR_OBJECT_UPDATE_FAILED)
        ERR_MAP(ERR_VMCFG_MODIFY_FORBIDDEN, TECS_ERR_VMCFG_MODIFY_FORBIDDEN)
        ERR_MAP(ERR_VMCFG_ACQUIRE_IMAGE_FAILED, TECS_ERR_VMCFG_ACQUIRE_IMAGE_FAILED)
        ERR_MAP_END
    };
    VmCfgSetImageDiskMethod (UserPool *upool=0)
    {
        _method_name = "VmCfgSetImageDiskMethod";
        _user_id = -1;
        _upool = upool;
    };
    ~VmCfgSetImageDiskMethod(){};

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                         xmlrpc_c::value  *const retval);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(VmCfgSetImageDiskMethod); 

};

/**
@brief 功能描述: 映像管理的实体类\n
@li @b 其它说明: 无
*/
class VmCfgSetNicMethod :public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERROR_TIME_OUT,           TECS_ERR_TIMEOUT) 
        ERR_MAP(ERR_AUTHORIZE_FAILED,     TECS_ERR_AUTHORIZE_FAILED) 
        ERR_MAP(ERROR_INVALID_ARGUMENT,   TECS_ERR_PARA_INVALID) 
        ERR_MAP(ERR_OBJECT_NOT_EXIST,     TECS_ERR_OBJECT_NOT_EXIST)
        ERR_MAP(ERR_OBJECT_UPDATE_FAILED, TECS_ERR_OBJECT_UPDATE_FAILED)
        ERR_MAP(ERR_VMCFG_MODIFY_FORBIDDEN, TECS_ERR_VMCFG_MODIFY_FORBIDDEN)
        ERR_MAP_END
    };

    VmCfgSetNicMethod (UserPool *upool=0)
    {
        _method_name = "VmCfgSetNicMethod";
        _user_id = -1;
        _upool = upool;
    };
    ~VmCfgSetNicMethod (){};

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                         xmlrpc_c::value  *const retval);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(VmCfgSetNicMethod); 

};


/**
@brief 功能描述: 创建/删除互斥关系实体类\n
@li @b 其它说明: 无
*/
class VmCfgRelationMethod :public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERROR_TIME_OUT,           TECS_ERR_TIMEOUT) 
        ERR_MAP(ERR_AUTHORIZE_FAILED,     TECS_ERR_AUTHORIZE_FAILED) 
        ERR_MAP(ERROR_INVALID_ARGUMENT,   TECS_ERR_PARA_INVALID) 
        ERR_MAP(ERR_OBJECT_NOT_EXIST,     TECS_ERR_OBJECT_NOT_EXIST)
        ERR_MAP(ERR_OBJECT_UPDATE_FAILED, TECS_ERR_OBJECT_UPDATE_FAILED)
        ERR_MAP(ERR_VMCFG_MODIFY_FORBIDDEN, TECS_ERR_VMCFG_MODIFY_FORBIDDEN)
        ERR_MAP_END
    };

    VmCfgRelationMethod (UserPool *upool=0)
    {
        _method_name = "VmCfgRelationMethod";
        _user_id = -1;
        _upool = upool;
    };
    ~VmCfgRelationMethod (){};

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                         xmlrpc_c::value  *const retval);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(VmCfgRelationMethod); 

};


/**
@brief 功能描述: 查询虚拟机互斥关系\n
@li @b 其它说明: 无
*/
class VmCfgRelationQueryMethod :public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERROR_TIME_OUT,           TECS_ERR_TIMEOUT) 
        ERR_MAP(ERR_AUTHORIZE_FAILED,     TECS_ERR_AUTHORIZE_FAILED) 
        ERR_MAP(ERROR_INVALID_ARGUMENT,   TECS_ERR_PARA_INVALID) 
        ERR_MAP(ERR_OBJECT_NOT_EXIST,     TECS_ERR_OBJECT_NOT_EXIST)
        ERR_MAP(ERR_OBJECT_UPDATE_FAILED, TECS_ERR_OBJECT_UPDATE_FAILED)
        ERR_MAP(ERR_VMCFG_MODIFY_FORBIDDEN, TECS_ERR_VMCFG_MODIFY_FORBIDDEN)
        ERR_MAP_END
    };

    VmCfgRelationQueryMethod (UserPool *upool=0)
    {
        _method_name = "VmCfgRelationQueryMethod";
        _user_id = -1;
        _upool = upool;
    };
    ~VmCfgRelationQueryMethod (){};

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                         xmlrpc_c::value  *const retval);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(VmCfgRelationQueryMethod); 

};

/**
@brief 功能描述: 虚拟机加入亲和域实体类\n
@li @b 其它说明: 无
*/
class VmCfgAffinityRegionAddMethod :public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERROR,                    TECS_ERROR)
        ERR_MAP(ERR_AUTHORIZE_FAILED,     TECS_ERR_AUTHORIZE_FAILED)
        ERR_MAP(ERROR_INVALID_ARGUMENT,   TECS_ERR_PARA_INVALID)
        ERR_MAP(ERR_AFFINITY_REGION_NOT_EXIST,     TECS_ERR_OBJECT_NOT_EXIST)
        ERR_MAP(ERR_OBJECT_NOT_EXIST,TECS_ERR_OBJECT_NOT_EXIST)
        ERR_MAP(ERR_VMCFG_MODIFY_FORBIDDEN,TECS_ERR_OBJECT_FORBID_OP)
        ERR_MAP(ERROR_DB_SELECT_FAIL,TECS_ERR_DB_QUERY_FAILED)
        ERR_MAP(TECS_ERR_DB_INSERT_FAILED,TECS_ERR_DB_INSERT_FAILED)
        ERR_MAP(ERR_AFFINITY_REGION_HAVE_REPELVM,TECS_ERR_OBJECT_FORBID_OP)
        ERR_MAP_END
    };

    VmCfgAffinityRegionAddMethod (UserPool *upool=0)
    {
        _method_name = "VmCfgAffinityRegionAddMethod";
        _user_id = -1;
        _upool = upool;
    };
    ~VmCfgAffinityRegionAddMethod (){};

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                         xmlrpc_c::value  *const retval);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(VmCfgAffinityRegionAddMethod);

};

/**
@brief 功能描述: 虚拟机加入亲和域实体类\n
@li @b 其它说明: 无
*/
class VmCfgAffinityRegionDelMethod :public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERR_AUTHORIZE_FAILED,     TECS_ERR_AUTHORIZE_FAILED)
        ERR_MAP(ERROR_INVALID_ARGUMENT,   TECS_ERR_PARA_INVALID)
        ERR_MAP(ERR_OBJECT_NOT_EXIST,     TECS_ERR_OBJECT_NOT_EXIST)
        ERR_MAP(ERR_OBJECT_DROP_FAILED, TECS_ERR_OBJECT_DROP_FAILED)
        ERR_MAP(ERR_VMCFG_MODIFY_FORBIDDEN, TECS_ERR_OBJECT_FORBID_OP)
        ERR_MAP_END
    };

    VmCfgAffinityRegionDelMethod (UserPool *upool=0)
    {
        _method_name = "VmCfgAffinityRegionDelMethod";
        _user_id = -1;
        _upool = upool;
    };
    ~VmCfgAffinityRegionDelMethod (){};

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                         xmlrpc_c::value  *const retval);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(VmCfgAffinityRegionDelMethod);

};

/**
@brief 功能描述: 发起虚拟机热迁移\n
@li @b 其它说明: 无
*/
class VmCfgMigrateMethod :public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERROR,                      TECS_ERROR)  
        ERR_MAP(ERROR_TIME_OUT,             TECS_ERR_TIMEOUT) 
        ERR_MAP(ERR_AUTHORIZE_FAILED,       TECS_ERR_AUTHORIZE_FAILED) 
        ERR_MAP(ERROR_INVALID_ARGUMENT,     TECS_ERR_PARA_INVALID) 
        ERR_MAP(ERR_OBJECT_NOT_EXIST,       TECS_ERR_OBJECT_NOT_EXIST)
        ERR_MAP(ERROR_RESOURCE_UNAVAILABLE, TECS_ERR_VMCFG_ACTION_FAILED_IN_CC)
        ERR_MAP(ERROR_DEVICE_NOT_EXIST,     TECS_ERR_HOST_NOT_EXIST)
        ERR_MAP(ERROR_OBJECT_NOT_EXIST,     TECS_ERR_OBJECT_NOT_EXIST)
        ERR_MAP(ERR_VM_NOT_EXIST,           TECS_ERR_VMCFG_NO_VM)
        ERR_MAP(ERROR_ACTION_RUNNING,       TECS_SUCCESS)
        ERR_MAP_END
    };

    VmCfgMigrateMethod (UserPool *upool=0)
    {
        _method_name = "VmCfgMigrateMethod";
        _user_id = -1;
        _upool = upool;
    };
    VmCfgMigrateMethod (){};

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                         xmlrpc_c::value  *const retval);



/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(VmCfgMigrateMethod); 

};


/**
@brief 功能描述: 查询虚拟机能够使用的USB设备\n
@li @b 其它说明: 无
*/
class VmCfgUsbDeviceQueryMethod :public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERROR_TIME_OUT,           TECS_ERR_TIMEOUT) 
        ERR_MAP(ERROR_NOT_READY,     TECS_ERR_VMCFG_STATE_FORBIDDEN) 
        ERR_MAP(ERR_OBJECT_NOT_EXIST,     TECS_ERR_OBJECT_NOT_EXIST)
        ERR_MAP_END
    };

    VmCfgUsbDeviceQueryMethod (UserPool *upool=0)
    {
        _method_name = "VmCfgUsbDeviceQueryMethod";
        _user_id = -1;
        _upool = upool;
    };
    ~VmCfgUsbDeviceQueryMethod (){};

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                         xmlrpc_c::value  *const retval);

    
/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(VmCfgUsbDeviceQueryMethod); 

};


/**
@brief 功能描述: 操作虚拟机能够使用的USB设备\n
@li @b 其它说明: 无
*/
class VmCfgUsbDeviceOpMethod :public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERROR_TIME_OUT,           TECS_ERR_TIMEOUT) 
        ERR_MAP(ERROR_NOT_PERMITTED,           TECS_ERR_AUTHENTICATE_FAILED) 
        ERR_MAP(ERROR_NOT_READY,     TECS_ERR_VMCFG_STATE_FORBIDDEN) 
        ERR_MAP(ERR_OBJECT_NOT_EXIST,     TECS_ERR_OBJECT_NOT_EXIST)
        ERR_MAP(ERROR_INVALID_ARGUMENT,     TECS_ERR_PARA_INVALID)
        ERR_MAP(ERROR_ACTION_RUNNING,        TECS_SUCCESS)
        ERR_MAP_END
    };

    VmCfgUsbDeviceOpMethod (UserPool *upool=0)
    {
        _method_name = "VmCfgUsbDeviceOpMethod";
        _user_id = -1;
        _upool = upool;
    };
    ~VmCfgUsbDeviceOpMethod (){};

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                         xmlrpc_c::value  *const retval);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(VmCfgUsbDeviceOpMethod); 

};

/**
@brief 功能描述: attach 虚拟机移动盘\n
@li @b 其它说明: 无
*/
class VmCfgAttachPortableDiskMethod :public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERROR_TIME_OUT,           TECS_ERR_TIMEOUT) 
        ERR_MAP(ERROR_NOT_PERMITTED,      TECS_ERR_AUTHENTICATE_FAILED) 
        ERR_MAP(ERR_OBJECT_NOT_EXIST,     TECS_ERR_OBJECT_NOT_EXIST)
        ERR_MAP(ERROR_INVALID_ARGUMENT,   TECS_ERR_PARA_INVALID)
        ERR_MAP(ERR_VMCFG_INVALID_ACTION_TYPE, TECS_ERR_VMCFG_INVALID_ACTION_TYPE)
        ERR_MAP(ERR_VMCFG_STATE_FORBIDDEN,     TECS_ERR_VMCFG_STATE_FORBIDDEN)
        ERR_MAP(ERROR_ACTION_RUNNING,        TECS_SUCCESS)
        ERR_MAP_END
    };

    VmCfgAttachPortableDiskMethod (UserPool *upool=0)
    {
        _method_name = "VmCfgAttachPortableDiskMethod";
        _user_id = -1;
        _upool = upool;
    };
    ~VmCfgAttachPortableDiskMethod (){};

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                         xmlrpc_c::value  *const retval);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(VmCfgAttachPortableDiskMethod); 

};


/**
@brief 功能描述: detach 虚拟机移动盘\n
@li @b 其它说明: 无
*/
class VmCfgDetachPortableDiskMethod :public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERROR_TIME_OUT,           TECS_ERR_TIMEOUT) 
        ERR_MAP(ERROR_NOT_PERMITTED,      TECS_ERR_AUTHENTICATE_FAILED) 
        ERR_MAP(ERR_OBJECT_NOT_EXIST,     TECS_ERR_OBJECT_NOT_EXIST)
        ERR_MAP(ERROR_INVALID_ARGUMENT,   TECS_ERR_PARA_INVALID)
        ERR_MAP(ERR_VMCFG_INVALID_ACTION_TYPE, TECS_ERR_VMCFG_INVALID_ACTION_TYPE)
        ERR_MAP(ERR_VMCFG_STATE_FORBIDDEN,     TECS_ERR_VMCFG_STATE_FORBIDDEN)
        ERR_MAP(ERROR_ACTION_RUNNING,        TECS_SUCCESS)
        ERR_MAP_END
    };

    VmCfgDetachPortableDiskMethod (UserPool *upool=0)
    {
        _method_name = "VmCfgDetachPortableDiskMethod";
        _user_id = -1;
        _upool = upool;
    };
    ~VmCfgDetachPortableDiskMethod (){};

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                         xmlrpc_c::value  *const retval);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(VmCfgDetachPortableDiskMethod); 

};

/**
@brief 功能描述: 创建镜像备份盘\n
@li @b 其它说明: 无
*/
class VmCfgCreateImageBackupMethod :public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERROR_TIME_OUT,           TECS_ERR_TIMEOUT) 
        ERR_MAP(ERROR_NOT_PERMITTED,      TECS_ERR_AUTHENTICATE_FAILED) 
        ERR_MAP(ERR_OBJECT_NOT_EXIST,     TECS_ERR_OBJECT_NOT_EXIST)
        ERR_MAP(ERROR_INVALID_ARGUMENT,   TECS_ERR_PARA_INVALID)
        ERR_MAP(ERR_VMCFG_INVALID_ACTION_TYPE, TECS_ERR_VMCFG_INVALID_ACTION_TYPE)
        ERR_MAP(ERR_VMCFG_STATE_FORBIDDEN,     TECS_ERR_VMCFG_STATE_FORBIDDEN)
        ERR_MAP(ERROR_ACTION_RUNNING,        TECS_SUCCESS)
        ERR_MAP_END
    };

    VmCfgCreateImageBackupMethod (UserPool *upool=0)
    {
        _method_name = "VmCfgDetachPortableDiskMethod";
        _user_id = -1;
        _upool = upool;
    };
    ~VmCfgCreateImageBackupMethod (){};

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                         xmlrpc_c::value  *const retval);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(VmCfgCreateImageBackupMethod); 

};

/**
@brief 功能描述: 删除镜像备份盘\n
@li @b 其它说明: 无
*/
class VmCfgDeleteImageBackupMethod :public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERROR_TIME_OUT,           TECS_ERR_TIMEOUT) 
        ERR_MAP(ERROR_NOT_PERMITTED,      TECS_ERR_AUTHENTICATE_FAILED) 
        ERR_MAP(ERR_OBJECT_NOT_EXIST,     TECS_ERR_OBJECT_NOT_EXIST)
        ERR_MAP(ERROR_INVALID_ARGUMENT,   TECS_ERR_PARA_INVALID)
        ERR_MAP(ERR_VMCFG_INVALID_ACTION_TYPE, TECS_ERR_VMCFG_INVALID_ACTION_TYPE)
        ERR_MAP(ERR_VMCFG_STATE_FORBIDDEN,     TECS_ERR_VMCFG_STATE_FORBIDDEN)
        ERR_MAP(ERROR_IMG_BACKUP_ACTIVED, TECS_ERR_VMCFG_DEL_IMGBACKUP_ACTIVED)
        ERR_MAP(ERROR_ACTION_RUNNING,        TECS_SUCCESS)
        ERR_MAP_END
    };

    VmCfgDeleteImageBackupMethod (UserPool *upool=0)
    {
        _method_name = "VmCfgDeleteImageBackupMethod";
        _user_id = -1;
        _upool = upool;
    };
    ~VmCfgDeleteImageBackupMethod (){};

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                         xmlrpc_c::value  *const retval);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(VmCfgDeleteImageBackupMethod); 

};

/**
@brief 功能描述: 虚拟机镜像备份信息修改\n
@li @b 其它说明: 无
*/
class VmCfgImageBackupModifyMethod :public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERROR_TIME_OUT,           TECS_ERR_TIMEOUT) 
        ERR_MAP(ERROR_NOT_PERMITTED,      TECS_ERR_AUTHENTICATE_FAILED) 
        ERR_MAP(ERR_OBJECT_NOT_EXIST,     TECS_ERR_OBJECT_NOT_EXIST)
        ERR_MAP(ERROR_INVALID_ARGUMENT,   TECS_ERR_PARA_INVALID)
        ERR_MAP(ERR_VMCFG_INVALID_ACTION_TYPE, TECS_ERR_VMCFG_INVALID_ACTION_TYPE)
        ERR_MAP(ERR_VMCFG_STATE_FORBIDDEN,     TECS_ERR_VMCFG_STATE_FORBIDDEN)
        ERR_MAP(ERROR_ACTION_RUNNING,        TECS_SUCCESS)
        ERR_MAP_END
    };

    VmCfgImageBackupModifyMethod (UserPool *upool=0)
    {
        _method_name = "VmCfgImageBackupModifyMethod";
        _user_id = -1;
        _upool = upool;
    };
    ~VmCfgImageBackupModifyMethod (){};

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                         xmlrpc_c::value  *const retval);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(VmCfgImageBackupModifyMethod); 

};

/**
@brief 功能描述: 虚拟机镜像备份恢复\n
@li @b 其它说明: 无
*/
class VmCfgRestoreImageBackupMethod :public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERROR_TIME_OUT,           TECS_ERR_TIMEOUT)
        ERR_MAP(ERROR_NOT_PERMITTED,      TECS_ERR_AUTHENTICATE_FAILED)
        ERR_MAP(ERR_OBJECT_NOT_EXIST,     TECS_ERR_OBJECT_NOT_EXIST)
        ERR_MAP(ERROR_INVALID_ARGUMENT,   TECS_ERR_PARA_INVALID)
        ERR_MAP(ERR_VMCFG_INVALID_ACTION_TYPE, TECS_ERR_VMCFG_INVALID_ACTION_TYPE)
        ERR_MAP(ERR_VMCFG_STATE_FORBIDDEN,     TECS_ERR_VMCFG_STATE_FORBIDDEN)
        ERR_MAP(ERROR_ACTION_RUNNING,        TECS_SUCCESS)
        ERR_MAP_END
    };

    VmCfgRestoreImageBackupMethod (UserPool *upool=0)
    {
        _method_name = "VmCfgRestoreImageBackupMethod";
        _user_id = -1;
        _upool = upool;
    };
    ~VmCfgRestoreImageBackupMethod (){};

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                         xmlrpc_c::value  *const retval);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(VmCfgRestoreImageBackupMethod);

};

/**
@brief 功能描述: 虚拟机克隆的实体类\n
@li @b 其它说明: 一次传入所有参数
*/
class VmCfgCloneMethod :public VmCfgQueryMethod,public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERROR,                          TECS_ERROR)  
        ERR_MAP(ERROR_TIME_OUT,                 TECS_ERR_TIMEOUT) 
        ERR_MAP(ERR_AUTHORIZE_FAILED,           TECS_ERR_AUTHORIZE_FAILED) 
        ERR_MAP(ERROR_INVALID_ARGUMENT,         TECS_ERR_PARA_INVALID) 
        ERR_MAP(ERR_VMCFG_ACQUIRE_IMAGE_FAILED, TECS_ERR_VMCFG_ACQUIRE_IMAGE_FAILED)
        ERR_MAP(ERR_VMCFG_INVALID_PROJECT,      TECS_ERR_VMCFG_INVALID_PROJECT) 
        ERR_MAP(ERR_OBJECT_ALLOCATE_FAILED,     TECS_ERR_OBJECT_ALLOCATE_FAILED)
        ERR_MAP_END
    }
    
    VmCfgCloneMethod (UserPool *upool=0)
    {
        _method_name = "VmCfgCloneMethod";
        _user_id = -1;
        _upool = upool;
    };
    
    ~VmCfgCloneMethod (){};

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                                 xmlrpc_c::value  *const retval);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(VmCfgCloneMethod); 

};

}
#endif /* VMCFG_MANAGER_METHODS_H */

