/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：user_manager_method.h
* 文件标识：
* 内容摘要：user manager的XML-RPC注册方法类
* 当前版本：1.0
* 作    者：张文剑
* 完成日期：2012年04月27日
*
* 修改记录1：
*    修改日期：2012年04月27日
*    版 本 号：V1.0
*    修 改 人：张文剑
*    修改内容：创建
*******************************************************************************/
#ifndef USER_MANAGER_METHODS_H        
#define USER_MANAGER_METHODS_H
#include "request_manager.h"
#include "tecs_rpc_method.h"
#include "user_common.h"
#include "api_error.h"
#include "tecs_errcode.h"

namespace zte_tecs
{
/**
@brief 功能描述: 用户完整信息查询\n
@li @b 其它说明: 一次传入所有参数
*/
class UserFullQueryMethod :public TecsRpcMethod
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
        ERR_MAP_END
    }

    enum UserCfgFullQueryType
    {
        ALL_USER        = -1, /** < 所有用户的, 仅admin用户可以 */
        CURRENT_USER    = 0, /** < 当前用户的，仅用户自己可以  */
        APPOINTED_USER  = 1, /** < 指定用户的, 仅admin用户可以 */
    };
    
    UserFullQueryMethod (UserPool *upool=0)
    {
        _method_name = "UserFullQueryMethod";
        _user_id = -1;
        _upool = upool;
    };
	
    ~UserFullQueryMethod(){};

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                                 xmlrpc_c::value  *const retval);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:
   DISALLOW_COPY_AND_ASSIGN(UserFullQueryMethod); 
};

}
#endif

