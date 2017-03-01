/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：file_manager_methods.h
* 文件标识：
* 内容摘要：file的XML-RPC 方法类
* 当前版本：1.0
* 作    者：lixch
* 完成日期：2013年3月4日
*
*******************************************************************************/

#ifndef FILE_MANAGER_METHODS_H        
#define FILE_MANAGER_METHODS_H

#include "request_manager.h"
#include "tecs_rpc_method.h"
#include "user_pool.h"
#include "api_const.h"
#include "tecs_errcode.h"
#include "image_url.h"


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
@brief 功能描述: 文件查询的实体类\n
@li @b 其它说明: 无
*/
class FileGetCoredumpUrlMethod :public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:

    FileGetCoredumpUrlMethod (UserPool *upool=0)
    {
        _method_name = "FileGetCoredumpUrlMethod";
        _user_id = -1;
        _upool = upool;
    };
    ~FileGetCoredumpUrlMethod (){};
    
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP_END
    }

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                         xmlrpc_c::value  *const retval);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(FileGetCoredumpUrlMethod); 

};

}
#endif /* IMAGE_MANAGER_METHODS_H */

