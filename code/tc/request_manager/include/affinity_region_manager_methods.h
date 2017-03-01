/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�affinity_region_manager_method.h
* �ļ���ʶ��
* ����ժҪ��affinity_region��XML-RPCע�᷽����
* ��ǰ�汾��1.0
* ��    �ߣ�
* ������ڣ�2013��8��27��
*
* �޸ļ�¼1��
*    �޸����ڣ�2013��8��27��
*    �� �� �ţ�V1.0
*    �� �� �ˣ�
*    �޸����ݣ�����
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


// ���ÿ�������궨��
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)
#endif

using namespace std;

namespace zte_tecs
{

/**
@brief ��������: ӳ������ʵ����\n
@li @b ����˵��: ��
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
@brief ��������: ӳ������ʵ����\n
@li @b ����˵��: ��
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
@brief ��������: ӳ������ʵ����\n
@li @b ����˵��: ��
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
@brief ��������: ӳ������ʵ����\n
@li @b ����˵��: ��
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
@brief ��������: ӳ������ʵ����\n
@li @b ����˵��: ��
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
@brief ��������: ���̱����ʵ����\n
@li @b ����˵��: ��
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
@brief ��������: ���̵����ʵ����\n
@li @b ����˵��: ��
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

