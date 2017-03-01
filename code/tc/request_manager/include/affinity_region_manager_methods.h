/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：affinity_region_manager_method.h
* 文件标识：
* 内容摘要：affinity_region的XML-RPC注册方法类
* 当前版本：1.0
* 作    者：
* 完成日期：2013年8月27日
*
* 修改记录1：
*    修改日期：2013年8月27日
*    版 本 号：V1.0
*    修 改 人：
*    修改内容：创建
*******************************************************************************/

#ifndef AFFINITY_REGION_MANAGER_METHODS_H        
#define AFFINITY_REGION_MANAGER_METHODS_H

#include "request_manager.h"
#include "tecs_rpc_method.h"
#include "user_pool.h"
#include "api_error.h"
#include "tecs_errcode.h"
#include "affinity_region_pool.h"
#include "affinity_region_operate.h"
#include "db_config.h"


// 禁用拷贝构造宏定义
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)
#endif

using namespace std;

namespace zte_tecs
{

/**
@brief 功能描述: 映像管理的实体类\n
@li @b 其它说明: 无
*/
class AffinityRegionAllocateMethod :public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:

    AffinityRegionAllocateMethod (UserPool *upool=0)
    {
        _method_name = "AffinityRegionAllocateMethod";
        _user_id = -1;
        _upool = upool;
    };

    ~AffinityRegionAllocateMethod ()
    {};

    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERR_AFFINITY_REGION_AUTH_FAIL     ,TECS_ERR_AUTHORIZE_FAILED)
        ERR_MAP(ERR_AFFINITY_REGION_ALLOC_FAIL    ,TECS_ERR_OBJECT_ALLOCATE_FAILED)
        ERR_MAP(ERR_AFFINITY_REGION_PARA_INVALID    ,TECS_ERR_PARA_INVALID)
        ERR_MAP_END
    }

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                                 xmlrpc_c::value  *const retval);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(AffinityRegionAllocateMethod); 

};

/**
@brief 功能描述: 映像管理的实体类\n
@li @b 其它说明: 无
*/
class AffinityRegionDeleteMethod :public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:

    AffinityRegionDeleteMethod (UserPool *upool=0)
    {
        _method_name = "AffinityRegionDeleteMethod";
        _user_id = -1;
        _upool = upool;
    };

    ~AffinityRegionDeleteMethod ()
    {};

    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERROR                             ,TECS_ERROR)
        ERR_MAP(ERR_AFFINITY_REGION_AUTH_FAIL     ,TECS_ERR_AUTHORIZE_FAILED)
        ERR_MAP(ERR_AFFINITY_REGION_NOT_EXIST     ,TECS_ERR_OBJECT_NOT_EXIST)
        ERR_MAP(ERR_AFFINITY_REGION_QUERY_FAIL,TECS_ERR_DB_QUERY_FAILED)
        ERR_MAP(ERR_AFFINITY_REGION_DELETE_DB_FAIL,TECS_ERR_OBJECT_DROP_FAILED)
        ERR_MAP(ERR_AFFINITY_REGION_HAVE_VM       ,TECS_ERR_OBJECT_FORBID_OP)
        ERR_MAP_END
    }

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                         xmlrpc_c::value  *const retval);
 

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(AffinityRegionDeleteMethod); 

};

/**
@brief 功能描述: 映像管理的实体类\n
@li @b 其它说明: 无
*/
class AffinityRegionModifyMethod :public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:

    AffinityRegionModifyMethod (UserPool *upool=0)
    {
        _method_name = "AffinityRegionModifyMethod";
        _user_id = -1;
        _upool = upool;
    };
    ~AffinityRegionModifyMethod ()
    {};

    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERROR                             ,TECS_ERROR)
        ERR_MAP(ERR_AFFINITY_REGION_AUTH_FAIL     ,TECS_ERR_AUTHORIZE_FAILED)
        ERR_MAP(ERR_AFFINITY_REGION_NOT_EXIST     ,TECS_ERR_OBJECT_NOT_EXIST)
        ERR_MAP(ERR_AFFINITY_REGION_UPDATA_FAIL   ,TECS_ERR_OBJECT_UPDATE_FAILED)
        ERR_MAP(ERR_AFFINITY_REGION_HAVE_REPELVM  ,TECS_ERR_OBJECT_FORBID_OP)
        ERR_MAP(ERR_AFFINITY_REGION_PARA_INVALID    ,TECS_ERR_PARA_INVALID)
        ERR_MAP_END
    }

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                         xmlrpc_c::value  *const retval);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(AffinityRegionModifyMethod); 

};

/**
@brief 功能描述: 映像管理的实体类\n
@li @b 其它说明: 无
*/
class AffinityRegionQueryMethod :public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:

    AffinityRegionQueryMethod (UserPool *upool=0)
    {
        _method_name = "AffinityRegionQueryMethod";
        _user_id = -1;
        _upool = upool;
    };
    ~AffinityRegionQueryMethod ()
    {};

    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERR_AFFINITY_REGION_AUTH_FAIL     ,TECS_ERR_AUTHORIZE_FAILED)
        ERR_MAP(ERR_AFFINITY_REGION_NOT_EXIST     ,TECS_ERR_OBJECT_NOT_EXIST)
        ERR_MAP(ERR_AFFINITY_REGION_QUERY_TYPE_ERR,TECS_ERR_PARA_INVALID)
        ERR_MAP(ERR_AFFINITY_REGION_QUERY_FAIL    ,TECS_ERR_DB_QUERY_FAILED)
        ERR_MAP(ERR_AFFINITY_REGION_PARA_INVALID    ,TECS_ERR_PARA_INVALID)
        ERR_MAP_END
    }

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                         xmlrpc_c::value  *const retval);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(AffinityRegionQueryMethod); 

};

#if 0
/**
@brief 功能描述: 映像管理的实体类\n
@li @b 其它说明: 无
*/
class ProjectShowVmMethod :public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:

    ProjectShowVmMethod (UserPool *upool=0)
    {
        _method_name = "ProjectShowVmMethod";
        _user_id = -1;
        _upool = upool;
    };
    ~ProjectShowVmMethod ()
    {};

    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERR_PROJECT_AUTH_FAIL     ,TECS_ERR_AUTHORIZE_FAILED)
        ERR_MAP(ERR_PROJECT_NOT_EXIST     ,TECS_ERR_OBJECT_NOT_EXIST)
        ERR_MAP(ERR_PROJECT_QUERY_TYPE_ERR,TECS_ERR_PROJECT_INVALID_QUERY_TYPE)
        ERR_MAP(ERR_PROJECT_QUERY_FAIL    ,TECS_ERR_DB_QUERY_FAILED)
        ERR_MAP_END
    }

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                         xmlrpc_c::value  *const retval);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(ProjectShowVmMethod); 

};

/**
@brief 功能描述: 工程保存的实体类\n
@li @b 其它说明: 无
*/
class ProjectSaveAsImageMethod :public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:

    ProjectSaveAsImageMethod (UserPool *upool=0)
    {
        _method_name = "ProjectSaveAsImageMethod";
        _user_id = -1;
        _upool = upool;
    };
    ~ProjectSaveAsImageMethod ()
    {};

    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERR_PROJECT_AUTH_FAIL     ,TECS_ERR_AUTHORIZE_FAILED)
        ERR_MAP(ERR_PROJECT_NOT_EXIST     ,TECS_ERR_OBJECT_NOT_EXIST)
        ERR_MAP(ERR_PROJECT_UPDATA_FAIL   ,TECS_ERR_OBJECT_UPDATE_FAILED)
        ERR_MAP_END
    }

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                         xmlrpc_c::value  *const retval);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(ProjectSaveAsImageMethod); 

};

/**
@brief 功能描述: 工程导入的实体类\n
@li @b 其它说明: 无
*/
class ProjectCreateByImageMethod :public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:

    ProjectCreateByImageMethod (UserPool *upool=0)
    {
        _method_name = "ProjectCreateByImageMethod";
        _user_id = -1;
        _upool = upool;
    };
    ~ProjectCreateByImageMethod ()
    {};

    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERR_PROJECT_AUTH_FAIL     ,TECS_ERR_AUTHORIZE_FAILED)
        ERR_MAP(ERR_PROJECT_NOT_EXIST     ,TECS_ERR_OBJECT_NOT_EXIST)
        ERR_MAP(ERR_PROJECT_UPDATA_FAIL   ,TECS_ERR_OBJECT_UPDATE_FAILED)
        ERR_MAP_END
    }

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                         xmlrpc_c::value  *const retval);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(ProjectCreateByImageMethod); 

};

#endif
}
#endif /* PROJECT_MANAGER_METHODS_H */

