/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�image_manager_methods.h
* �ļ���ʶ��
* ����ժҪ��image��XML-RPCע�᷽����
* ��ǰ�汾��1.0
* ��    �ߣ�Bob
* ������ڣ�2011��9��7��
*
* �޸ļ�¼1��
*    �޸����ڣ�2011��9��7��
*    �� �� �ţ�V1.0
*    �� �� �ˣ�Bob
*    �޸����ݣ�����
*******************************************************************************/

#ifndef VMTEMPLATE_MANAGER_METHODS_H        
#define VMTEMPLATE_MANAGER_METHODS_H

#include "request_manager.h"
#include "tecs_rpc_method.h"
#include "user_pool.h"
#include "api_error.h"
#include "tecs_errcode.h"
#include "vmtemplate_pool.h"
#include "vmtemplate_operate.h"
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
@brief ��������: ģ������ʵ����\n
@li @b ����˵��: ��
*/
class VmTemplateAllocateMethod :public TecsRpcMethod
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
        ERR_MAP(ERR_OBJECT_ALLOCATE_FAILED,  TECS_ERR_OBJECT_ALLOCATE_FAILED)
        ERR_MAP(ERROR_INVALID_ARGUMENT,       TECS_ERR_PARA_INVALID) 
        ERR_MAP(ERR_VMTEMPLATE_ACQUIRE_IMAGE_FAILED,  TECS_ERR_VMTEMPLATE_ACQUIRE_IMAGE_FAILED)
        ERR_MAP(ERR_VMTEMPLATE_NAME_CONFLICT,         TECS_ERR_VMTEMPLATE_NAME_CONFLICT)
        ERR_MAP(ERR_VMTEMPLATE_IMAGE_NOT_PUBLIC,  TECS_ERR_VMTEMPLATE_IMAGE_NOT_PUBLIC)
        ERR_MAP_END
    }
    VmTemplateAllocateMethod (UserPool *upool=0)
    {
        _method_name = "VmTemplateAllocateMethod";
        _user_id = -1;
        _upool = upool;
    };
    
    ~VmTemplateAllocateMethod (){};

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                                 xmlrpc_c::value  *const retval);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(VmTemplateAllocateMethod); 

};

/**
@brief ��������: ӳ������ʵ����\n
@li @b ����˵��: ��
*/
class VmTemplateDeleteMethod :public TecsRpcMethod
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
        ERR_MAP(ERR_OBJECT_NOT_EXIST,  TECS_ERR_OBJECT_NOT_EXIST)
        ERR_MAP(ERR_OBJECT_DROP_FAILED,  TECS_ERR_OBJECT_DROP_FAILED)
        ERR_MAP_END
    }
    VmTemplateDeleteMethod (UserPool *upool=0)
    {
        _method_name = "VmTemplateDeleteMethod";
        _user_id = -1;
        _upool = upool;
    };
    
    ~VmTemplateDeleteMethod (){};

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                         xmlrpc_c::value  *const retval);
 

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(VmTemplateDeleteMethod); 

};

/**
@brief ��������: ӳ������ʵ����\n
@li @b ����˵��: ��
*/
class VmTemplateModifyMethod :public TecsRpcMethod
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
        ERR_MAP(ERR_OBJECT_NOT_EXIST,      TECS_ERR_OBJECT_NOT_EXIST)
        ERR_MAP(ERR_OBJECT_UPDATE_FAILED,  TECS_ERR_OBJECT_UPDATE_FAILED)
        ERR_MAP(ERR_VMTEMPLATE_ACQUIRE_IMAGE_FAILED,  TECS_ERR_VMTEMPLATE_ACQUIRE_IMAGE_FAILED)
        ERR_MAP(ERR_VMTEMPLATE_IMAGE_NOT_PUBLIC,  TECS_ERR_VMTEMPLATE_IMAGE_NOT_PUBLIC)
        ERR_MAP_END
    }
    VmTemplateModifyMethod (UserPool *upool=0)
    {
        _method_name = "VmTemplateModifyMethod";
        _user_id = -1;
        _upool = upool;
    };
    ~VmTemplateModifyMethod (){};

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                         xmlrpc_c::value  *const retval);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(VmTemplateModifyMethod); 

};


/**
@brief ��������: ӳ������ʵ����\n
@li @b ����˵��: ��
*/
class VmTemplateQueryMethod :public TecsRpcMethod
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
        ERR_MAP(ERR_VMTEMPLATE_INVALID_QUERY_TYPE,  TECS_ERR_VMTEMPLATE_INVALID_QUERY_TYPE)
        ERR_MAP(ERROR_DB_SELECT_FAIL,  TECS_ERR_DB_QUERY_FAILED)
        ERR_MAP_END
    }
    VmTemplateQueryMethod (UserPool *upool=0)
    {
        _method_name = "VmTemplateQueryMethod";
        _user_id = -1;
        _upool = upool;
    };
    ~VmTemplateQueryMethod (){};

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                         xmlrpc_c::value  *const retval);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(VmTemplateQueryMethod); 

};


/**
@brief ��������: ӳ������ʵ����\n
@li @b ����˵��: ��
*/
class VmTemplatePublishMethod :public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERROR_TIME_OUT,        TECS_ERR_TIMEOUT) 
        ERR_MAP(ERR_AUTHORIZE_FAILED,  TECS_ERR_AUTHORIZE_FAILED) 
        ERR_MAP(ERR_OBJECT_NOT_EXIST,  TECS_ERR_OBJECT_NOT_EXIST)
        ERR_MAP(ERR_VMTEMPLATE_IMAGE_NOT_PUBLIC,  TECS_ERR_VMTEMPLATE_IMAGE_NOT_PUBLIC)
        ERR_MAP_END
    }
    VmTemplatePublishMethod (UserPool *upool=0)
    {
        _method_name = "VmTemplatePulishMethod";
        _user_id = -1;
        _upool = upool;
    };
    ~VmTemplatePublishMethod (){};

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                         xmlrpc_c::value  *const retval);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(VmTemplatePublishMethod); 

};


/**
@brief ��������: ӳ������ʵ����\n
@li @b ����˵��: ��
*/
class VmTemplateSetDiskMethod :public TecsRpcMethod
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
        ERR_MAP_END
    };

    VmTemplateSetDiskMethod (UserPool *upool=0)
    {
        _method_name = "VmTemplateSetDiskMethod";
        _user_id = -1;
        _upool = upool;
    };
    ~VmTemplateSetDiskMethod(){};

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                         xmlrpc_c::value  *const retval);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(VmTemplateSetDiskMethod); 

};


/**
@brief ��������: ӳ������ʵ����\n
@li @b ����˵��: ��
*/
class VmTemplateSetImageDiskMethod :public TecsRpcMethod
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
        ERR_MAP_END
    };
    VmTemplateSetImageDiskMethod (UserPool *upool=0)
    {
        _method_name = "VmTemplateSetImageDiskMethod";
        _user_id = -1;
        _upool = upool;
    };
    ~VmTemplateSetImageDiskMethod(){};

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                         xmlrpc_c::value  *const retval);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(VmTemplateSetImageDiskMethod); 

};


/**
@brief ��������: ӳ������ʵ����\n
@li @b ����˵��: ��
*/
class VmTemplateSetNicMethod :public TecsRpcMethod
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
        ERR_MAP_END
    };
    VmTemplateSetNicMethod (UserPool *upool=0)
    {
        _method_name = "VmTemplateSetNicMethod";
        _user_id = -1;
        _upool = upool;
    };
    ~VmTemplateSetNicMethod (){};

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                         xmlrpc_c::value  *const retval);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(VmTemplateSetNicMethod); 

};


}
#endif /* VMTEMPLATE_MANAGER_METHODS_H */

