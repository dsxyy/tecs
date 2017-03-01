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

#ifndef IMAGE_MANAGER_METHODS_H        
#define IMAGE_MANAGER_METHODS_H

#include "request_manager.h"
#include "tecs_rpc_method.h"
#include "user_pool.h"
#include "api_const.h"
#include "tecs_errcode.h"

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
@brief ��������: ӳ������ʵ����\n
@li @b ����˵��: ��
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
@brief ��������: ӳ������ʵ����\n
@li @b ����˵��: ��
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
@brief ��������: ӳ������ʵ����\n
@li @b ����˵��: ��
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
@brief ��������: ӳ������ʵ����\n
@li @b ����˵��: ��
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
@brief ��������: ӳ������ʵ����\n
@li @b ����˵��: ��
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
@brief ��������: ӳ������ʵ����\n
@li @b ����˵��: ��
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
@brief ��������: ӳ������ʵ����\n
@li @b ����˵��: ��
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
@brief ��������: ӳ������ʵ����\n
@li @b ����˵��: ��
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
@brief ��������:����׼����ʵ����\n
@li @b ����˵��: ��
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
@brief ��������:����������ռ��ѯ��\n
@li @b ����˵��: ��
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

