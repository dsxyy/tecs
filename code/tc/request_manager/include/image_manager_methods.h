/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：image_manager_methods.h
* 文件标识：
* 内容摘要：image的XML-RPC注册方法类
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

#ifndef IMAGE_MANAGER_METHODS_H        
#define IMAGE_MANAGER_METHODS_H

#include "request_manager.h"
#include "tecs_rpc_method.h"
#include "user_pool.h"
#include "api_const.h"
#include "tecs_errcode.h"

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
class ImageRegisterMethod :public TecsRpcMethod
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
        ERR_MAP(ERR_IMAGE_FILE_NOT_EXIST,  TECS_ERR_IMAGE_FILE_NOT_EXIST)
        ERR_MAP(ERR_IMAGE_HAVE_REGISTERED, TECS_ERR_IMAGE_HAVE_REGISTERED)
        ERR_MAP(ERR_OBJECT_ALLOCATE_FAILED,TECS_ERR_OBJECT_ALLOCATE_FAILED)
        ERR_MAP_END
    }

    ImageRegisterMethod (UserPool *upool=0)
    {
        _method_name = "ImageRegisterMethod";
        _user_id = -1;
        _upool = upool;
    };
	
    ~ImageRegisterMethod (){};

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                                 xmlrpc_c::value  *const retval);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(ImageRegisterMethod); 

};

/**
@brief 功能描述: 映像管理的实体类\n
@li @b 其它说明: 无
*/
class ImageDeregisterMethod :public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:

    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERROR_TIME_OUT,         TECS_ERR_TIMEOUT) 
        ERR_MAP(ERR_AUTHORIZE_FAILED,   TECS_ERR_AUTHORIZE_FAILED) 
        ERR_MAP(ERR_OBJECT_NOT_EXIST,   TECS_ERR_OBJECT_NOT_EXIST)
        ERR_MAP(ERR_OBJECT_DROP_FAILED, TECS_ERR_OBJECT_DROP_FAILED)
        ERR_MAP(ERR_IMAGE_IS_USING,  TECS_ERR_IMAGE_IS_USING)
        ERR_MAP_END
    }
    
    ImageDeregisterMethod (UserPool *upool=0)
    {
        _method_name = "ImageDeRegisterMethod";
        _user_id = -1;
        _upool = upool;
    };
	
    ~ImageDeregisterMethod (){};

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                         xmlrpc_c::value  *const retval);
 

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(ImageDeregisterMethod); 

};


/**
@brief 功能描述: 映像管理的实体类\n
@li @b 其它说明: 无
*/
class ImageDeleteMethod :public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:

    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERROR_TIME_OUT,         TECS_ERR_TIMEOUT) 
        ERR_MAP(ERR_AUTHORIZE_FAILED,   TECS_ERR_AUTHORIZE_FAILED) 
        ERR_MAP(ERR_OBJECT_NOT_EXIST,   TECS_ERR_OBJECT_NOT_EXIST)
        ERR_MAP(ERR_OBJECT_DROP_FAILED, TECS_ERR_OBJECT_DROP_FAILED)
        ERR_MAP(ERR_IMAGE_IS_USING,  TECS_ERR_IMAGE_IS_USING)
        ERR_MAP(ERR_IMAGE_IS_DEPLOYED,  TECS_ERR_IMAGE_IS_DEPLOYED)
        ERR_MAP(ERROR_DB_SELECT_FAIL,   TECS_ERR_DB_QUERY_FAILED)
        ERR_MAP_END
    }
    
    ImageDeleteMethod (UserPool *upool=0)
    {
        _method_name = "ImageDeleteMethod";
        _user_id = -1;
        _upool = upool;
    };
	
    ~ImageDeleteMethod(){};

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                         xmlrpc_c::value  *const retval);
 

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:
   DISALLOW_COPY_AND_ASSIGN(ImageDeleteMethod); 
};


/**
@brief 功能描述: 映像管理的实体类\n
@li @b 其它说明: 无
*/
class ImageModifyMethod :public TecsRpcMethod
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
        ERR_MAP(ERR_OBJECT_NOT_EXIST,      TECS_ERR_OBJECT_NOT_EXIST )
        ERR_MAP(ERR_OBJECT_UPDATE_FAILED,  TECS_ERR_OBJECT_UPDATE_FAILED)
        ERR_MAP(ERR_IMAGE_IS_USING,  TECS_ERR_IMAGE_IS_USING)
        ERR_MAP_END
    }
    ImageModifyMethod (UserPool *upool=0)
    {
        _method_name = "ImageModifyMethod";
        _user_id = -1;
        _upool = upool;
    };
    ~ImageModifyMethod (){};

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                         xmlrpc_c::value  *const retval);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(ImageModifyMethod); 

};

/**
@brief 功能描述: 映像管理的实体类\n
@li @b 其它说明: 无
*/
class ImageQueryMethod :public TecsRpcMethod
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
		ERR_MAP(ERR_IMAGE_INVALID_QUERY_TYPE,  TECS_ERR_IMAGE_INVALID_QUERY_TYPE)
		ERR_MAP(ERROR_DB_SELECT_FAIL,          TECS_ERR_DB_QUERY_FAILED)
        ERR_MAP_END
    }
    ImageQueryMethod (UserPool *upool=0)
    {
        _method_name = "ImageQueryMethod";
        _user_id = -1;
        _upool = upool;
    };
    ~ImageQueryMethod (){};

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                         xmlrpc_c::value  *const retval);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(ImageQueryMethod); 

};


class ImageFtpSvrInfoGetMethod :public TecsRpcMethod
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
        ERR_MAP(ERR_IMAGE_INVALID_QUERY_TYPE,  TECS_ERR_IMAGE_INVALID_QUERY_TYPE)
	ERR_MAP(ERROR_DB_SELECT_FAIL,          TECS_ERR_DB_QUERY_FAILED)
        ERR_MAP_END
    }
    ImageFtpSvrInfoGetMethod (UserPool *upool=0)
    {
        _method_name = "ImageFtpSvrInfoGetMethod";
        _user_id = -1;
        _upool = upool;
    };
    ~ImageFtpSvrInfoGetMethod (){};

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                         xmlrpc_c::value  *const retval);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(ImageFtpSvrInfoGetMethod); 

};


/**
@brief 功能描述: 映像管理的实体类\n
@li @b 其它说明: 无
*/
class ImageEnableMethod :public TecsRpcMethod
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
		ERR_MAP(ERR_IMAGE_IS_USING,  TECS_ERR_IMAGE_IS_USING)
        ERR_MAP_END
    }
    ImageEnableMethod (UserPool *upool=0)
    {
        _method_name = "ImageEnableMethod";
        _user_id = -1;
        _upool = upool;
    };
    ~ImageEnableMethod (){};

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                         xmlrpc_c::value  *const retval);
 
/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(ImageEnableMethod); 

};


/**
@brief 功能描述: 映像管理的实体类\n
@li @b 其它说明: 无
*/
class ImagePublishMethod :public TecsRpcMethod
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
        ERR_MAP(ERR_IMAGE_IS_USING,  TECS_ERR_IMAGE_IS_USING)
        ERR_MAP_END
    }
    ImagePublishMethod (UserPool *upool=0)
    {
        _method_name = "ImagePublishMethod";
        _user_id = -1;
        _upool = upool;
    };
    ~ImagePublishMethod (){};

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                         xmlrpc_c::value  *const retval);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(ImagePublishMethod); 

};

/**
@brief 功能描述: 映像管理的实体类\n
@li @b 其它说明: 无
*/
class ImageDeployMethod :public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERROR_TIME_OUT,              TECS_ERR_TIMEOUT) 
        ERR_MAP(ERR_AUTHORIZE_FAILED,        TECS_ERR_AUTHORIZE_FAILED) 
        ERR_MAP(ERROR_OBJECT_NOT_EXIST,      TECS_ERR_IMAGE_FILE_NOT_EXIST )
        ERR_MAP(ERROR_ACTION_RUNNING,        TECS_ERR_IMAGE_OP_RUNNING)
        ERR_MAP(ERROR_MESSAGE_FAIL,          TECS_ERR_UNKNOWN)
        ERR_MAP_END
    }
    ImageDeployMethod (UserPool *upool=0)
    {
        _method_name = "ImageDeployMethod";
        _user_id = -1;
        _upool = upool;
    };
    ~ImageDeployMethod (){};

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                         xmlrpc_c::value  *const retval);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(ImageDeployMethod); 

};

/**
@brief 功能描述: 映像管理的实体类\n
@li @b 其它说明: 无
*/
class ImageCancelMethod :public TecsRpcMethod
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
        ERR_MAP(ERROR_OBJECT_NOT_EXIST,    TECS_ERR_IMAGE_FILE_NOT_EXIST )
        ERR_MAP(ERROR_ACTION_RUNNING,      TECS_ERR_IMAGE_OP_RUNNING)
        ERR_MAP(ERROR_MESSAGE_FAIL,        TECS_ERR_UNKNOWN)
        ERR_MAP_END
    }
    ImageCancelMethod (UserPool *upool=0)
    {
        _method_name = "ImageCancelMethod";
        _user_id = -1;
        _upool = upool;
    };
    ~ImageCancelMethod (){};

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                         xmlrpc_c::value  *const retval);
/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(ImageCancelMethod); 

};


/**
@brief 功能描述:镜像准备的实体类\n
@li @b 其它说明: 无
*/
class ImagePrepareMethod :public TecsRpcMethod
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
        ERR_MAP(ERROR_OBJECT_NOT_EXIST,    TECS_ERR_IMAGE_FILE_NOT_EXIST )
        ERR_MAP(ERROR_INVALID_ARGUMENT,    TECS_ERR_PARA_INVALID)
        ERR_MAP(ERROR_ACTION_RUNNING,      TECS_ERR_IMAGE_OP_RUNNING)
        ERR_MAP(ERROR_MESSAGE_FAIL,        TECS_ERR_UNKNOWN)
        ERR_MAP_END
    }
    ImagePrepareMethod (UserPool *upool=0)
    {
        _method_name = "ImagePrepareMethod";
        _user_id = -1;
        _upool = upool;
    };
    ~ImagePrepareMethod (){};

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                         xmlrpc_c::value  *const retval);


/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(ImagePrepareMethod); 

};

/**
@brief 功能描述:镜像服务器空间查询类\n
@li @b 其它说明: 无
*/
class ImageGetSpaceMethod :public TecsRpcMethod
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
        ERR_MAP(ERROR_OBJECT_NOT_EXIST,    TECS_ERR_IMAGE_FILE_NOT_EXIST )
        ERR_MAP(ERROR_MESSAGE_FAIL,        TECS_ERR_UNKNOWN)
        ERR_MAP_END
    }
    ImageGetSpaceMethod (UserPool *upool=0)
    {
        _method_name = "ImageGetSpaceMethod";
        _user_id = -1;
        _upool = upool;
    };
    ~ImageGetSpaceMethod (){};

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                         xmlrpc_c::value  *const retval);


/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(ImageGetSpaceMethod); 

};


}
#endif /* IMAGE_MANAGER_METHODS_H */

