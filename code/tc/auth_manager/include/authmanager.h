/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：authmanager.h
* 文件标识：见配置管理计划书
* 内容摘要：authmanager类的定义文件
* 当前版本：1.0
* 作    者： 李振红
* 完成日期：
*
* 修改记录1：
*     修改日期：2011/7/25
*     版 本 号：V1.0
*     修 改 人：李振红
*     修改内容：创建
*******************************************************************************/
#ifndef AM_AUTHMANAGER_H
#define AM_AUTHMANAGER_H
#include "mid.h"
#include "message.h"
#include "message_unit.h"
#include "authrequest.h"
#include "drivermanager.h"
#include "auth_config.h"
#include "user_pool.h"
#include "usergroup_pool.h"
#include <xmlrpc-c/server_abyss.hpp>
#include <xmlrpc-c/registry.hpp>


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
@brief 功能描述: 鉴权和授权管理控制类，主要负责鉴权和授权请求的处理\n
@li @b 其它说明: 无
*/
class AuthManager
{
public:
    static AuthManager* GetInstance()
    {
        if(!_instance)
        {
            _instance = new AuthManager();
        }
        return _instance;
    }

    UserPool* get_upool() 
    { 
        return _upool;
    }

    UsergroupPool* get_gpool()
    {
        return _gpool; 
    }
    
    /**************************************************************************/
    /**
    @brief 功能描述: 判断是否本地鉴权
    */
    bool is_local() 
    { 
        if(_dm == NULL) 
        {
            return false; 
        }
        else 
        {
            return true;
        }
    }    

    /**************************************************************************/
    /**
    @brief 功能描述:加载本地鉴权驱动
    @li @b 入参列表：
    @verbatim
        无
    @endverbatim
    
    @li @b 出参列表：
    @verbatim
        无 
    @endverbatim

    @li @b 返回值列表：
    @verbatim
        无
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    STATUS LoadMads(UserPool* upool);
    
    /**************************************************************************/
    /**
    @brief 功能描述:处理鉴权请求
    @li @b 入参列表：
    @verbatim
        ar 鉴权请求对象
    @endverbatim
    
    @li @b 出参列表：
    @verbatim
        无 
    @endverbatim

    @li @b 返回值列表：
    @verbatim
        无
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    bool Authenticate(AuthRequest * ar);
    
    /**************************************************************************/
    /**
    @brief 功能描述:处理授权请求
    @li @b 入参列表：
    @verbatim
        ar 授权请求对象
    @endverbatim
    
    @li @b 出参列表：
    @verbatim
        无 
    @endverbatim

    @li @b 返回值列表：
    @verbatim
        无
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    bool Authorize(AuthRequest * ar);
   
private:
    static AuthManager *_instance;
    AuthManager();    
    ~AuthManager();
        
    DISALLOW_COPY_AND_ASSIGN(AuthManager);

    /**
    *   加锁函数
    */
    void Lock();
    
    /**
    *   功能描述:解锁函数
    */
    void Unlock();     


    /**
     *  鉴权驱动  
     */
    DriverManager*   _dm; 
    
    /**
     *  消息单元
     */
    MessageUnit*     _mu;

    /**
     *  用户池
     */
    UserPool*        _upool;

    /**
     *  用户组池
     */
    UsergroupPool*   _gpool;

    /**
     *  Mutex to access the auth_requests
     */
    pthread_mutex_t      _mutex; 
  
};

}
#endif 

