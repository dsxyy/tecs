/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：project_manager_method.h
* 文件标识：
* 内容摘要：project的XML-RPC注册方法类
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

#ifndef PROJECT_MANAGER_METHODS_H        
#define PROJECT_MANAGER_METHODS_H

#include "request_manager.h"
#include "tecs_rpc_method.h"
#include "user_pool.h"
#include "api_error.h"
#include "tecs_errcode.h"
#include "project_pool.h"
#include "project_operate.h"
#include "db_config.h"
#include "msg_project_manager_with_api_method.h"

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
class ProjectAllocateMethod :public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:

    ProjectAllocateMethod (UserPool *upool=0)
    {
        _method_name = "ProjectCreateMethod";
        _user_id = -1;
        _upool = upool;
    };
	
    ~ProjectAllocateMethod ()
    {};

    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERR_PROJECT_EXIST         ,TECS_ERR_PROJECT_NAME_CONFLICT)
        ERR_MAP(ERR_PROJECT_AUTH_FAIL     ,TECS_ERR_AUTHORIZE_FAILED)
        ERR_MAP(ERR_PROJECT_ALLOC_FAIL    ,TECS_ERR_OBJECT_ALLOCATE_FAILED)
        ERR_MAP_END
    }

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                                 xmlrpc_c::value  *const retval);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(ProjectAllocateMethod); 

};
/**
@brief 功能描述: 映像管理的实体类\n
@li @b 其它说明: 无
*/
class ProjectCloneMethod :public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:

    ProjectCloneMethod (UserPool *upool=0)
    {
        _method_name = "ProjectCloneMethod";
        _user_id = -1;
        _upool = upool;
    };

    ProjectCloneMethod ()
    {};

    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERR_PROJECT_NOT_EXIST     ,TECS_ERR_OBJECT_NOT_EXIST)
        ERR_MAP(ERR_PROJECT_AUTH_FAIL     ,TECS_ERR_AUTHORIZE_FAILED)
        ERR_MAP(ERR_PROJECT_EXIST         ,TECS_ERR_PROJECT_NAME_CONFLICT)
        ERR_MAP_END
    }

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                                 xmlrpc_c::value  *const retval);
   STATUS DealClone(const ApiProjectCloneData  &req, 
                                 int64          uid,
                                 int64         &project_id,
                                 ostringstream &err_oss);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(ProjectCloneMethod); 

};
/**
@brief 功能描述: 映像管理的实体类\n
@li @b 其它说明: 无
*/
class ProjectDeleteMethod :public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:

    ProjectDeleteMethod (UserPool *upool=0)
    {
        _method_name = "ProjectDeleteMethod";
        _user_id = -1;
        _upool = upool;
    };
	
    ~ProjectDeleteMethod ()
    {};

    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERR_PROJECT_AUTH_FAIL     ,TECS_ERR_AUTHORIZE_FAILED)
        ERR_MAP(ERR_PROJECT_NOT_EXIST     ,TECS_ERR_OBJECT_NOT_EXIST)
        ERR_MAP(ERR_PROJECT_DELETE_DB_FAIL,TECS_ERR_OBJECT_DROP_FAILED)
        ERR_MAP(ERR_RPOJECT_HAVE_VM       ,TECS_ERR_PROJECT_VM_RUNNING)
        ERR_MAP_END
    }

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                         xmlrpc_c::value  *const retval);
 

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(ProjectDeleteMethod); 

};

/**
@brief 功能描述: 映像管理的实体类\n
@li @b 其它说明: 无
*/
class ProjectModifyMethod :public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:

    ProjectModifyMethod (UserPool *upool=0)
    {
        _method_name = "ProjectModifyMethod";
        _user_id = -1;
        _upool = upool;
    };
    ~ProjectModifyMethod ()
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

   DISALLOW_COPY_AND_ASSIGN(ProjectModifyMethod); 

};

/**
@brief 功能描述: 映像管理的实体类\n
@li @b 其它说明: 无
*/
class ProjectQueryMethod :public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:

    ProjectQueryMethod (UserPool *upool=0)
    {
        _method_name = "ProjectQueryMethod";
        _user_id = -1;
        _upool = upool;
    };
    ~ProjectQueryMethod ()
    {};

    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERR_PROJECT_AUTH_FAIL     ,TECS_ERR_AUTHORIZE_FAILED)
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

   DISALLOW_COPY_AND_ASSIGN(ProjectQueryMethod); 

};
/**
@brief 功能描述: 工程管理的实体类\n
@li @b 其它说明: 无
*/
class ProjectActionMethod :public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:

    ProjectActionMethod (UserPool *upool=0)
    {
        _method_name = "ProjectActionMethod";
        _user_id = -1;
        _upool = upool;
    };
    ProjectActionMethod ()
    {};

    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERR_PROJECT_NOT_EXIST     ,TECS_ERR_OBJECT_NOT_EXIST)
        ERR_MAP(ERR_PROJECT_AUTH_FAIL     ,TECS_ERR_AUTHORIZE_FAILED)
        ERR_MAP(ERR_RPOJECT_INVALID_ACTION_TYPE, TECS_ERR_PROJECT_INVALID_ACTION_TYPE)
        ERR_MAP_END
    }

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                         xmlrpc_c::value  *const retval);
   STATUS DealDeploy(const ApiProjectActionData  &req, 
                                 int64          uid,
                                 int64         &project_id,
                                 ostringstream &err_oss,
                                 string &workflow_engine);
   STATUS DealCancel(const ApiProjectActionData  &req, 
                                     int64          uid,
                                     int64         &project_id,
                                     ostringstream &err_oss,
                                     string &workflow_engine);
   STATUS DealOperate(const ApiProjectActionData  &req, 
                                     int64          uid,
                                     int64         &project_id,
                                     ProjectOperation op,
                                     ostringstream &err_oss,
                                     string &workflow_engine);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(ProjectActionMethod); 

};

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

/**
@brief 功能描述: 映像管理的实体类\n
@li @b 其它说明: 无
*/
class ProjectStatisticsStaticMethod :public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:

    ProjectStatisticsStaticMethod (UserPool *upool=0)
    {
        _method_name = "ProjectStatisticsStaticMethod";
        _user_id = -1;
        _upool = upool;
    };
    ~ProjectStatisticsStaticMethod ()
    {};
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERR_PROJECT_AUTH_FAIL     ,TECS_ERR_AUTHORIZE_FAILED)
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

   DISALLOW_COPY_AND_ASSIGN(ProjectStatisticsStaticMethod); 

};


/**
@brief 功能描述: 映像管理的实体类\n
@li @b 其它说明: 无
*/
class ProjectStatisticsDynamicMethod :public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:

    ProjectStatisticsDynamicMethod (UserPool *upool=0)
    {
        _method_name = "ProjectStatisticsDynamicMethod";
        _user_id = -1;
        _upool = upool;
    };
    ~ProjectStatisticsDynamicMethod ()
    {};

    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERR_PROJECT_AUTH_FAIL     ,TECS_ERR_AUTHORIZE_FAILED)
        ERR_MAP(ERR_PROJECT_QUERY_FAIL    ,TECS_ERR_DB_QUERY_FAILED)
        ERR_MAP_END
    }

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                         xmlrpc_c::value  *const retval);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(ProjectStatisticsDynamicMethod); 

};

}
#endif /* PROJECT_MANAGER_METHODS_H */

