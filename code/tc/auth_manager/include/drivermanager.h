/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：drivermanager.h
* 文件标识：见配置管理计划书
* 内容摘要：drivermanager类的定义文件
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
#ifndef AM_DRIVERMANAGER_H
#define AM_DRIVERMANAGER_H
#include "message.h"
#include "message_unit.h"
#include "auth_config.h"
#include "user_pool.h"
#include "mid.h"
#include <xmlrpc-c/server_abyss.hpp>
#include <xmlrpc-c/registry.hpp>

namespace zte_tecs
{

/**
@brief 功能描述: 加载驱动类\n
@li @b 其它说明: 无
*/
class DriverManager: public MessageHandler
{
public:
    static DriverManager* GetInstance()
    {
        if(!_instance)
        {
            _instance = new DriverManager();
        }
        return _instance;
    }

    STATUS Init()
    {
        _upool = UserPool::GetInstance();
        return StartMu(DRIVERMANAGER);
    }
    
    /**************************************************************************/
    /**
    @brief 功能描述: 析构函数
    */
    virtual ~DriverManager();
   
    /**************************************************************************/
    /**
    @brief 功能描述: 设置userpool
    */  
    void set_userpool(UserPool* up)
    {
        _upool = up;
    }
    
    /**************************************************************************/
    /**
    @brief 功能描述: 获取userpool
    */  
    UserPool* get_userpool(void)
    {
        return _upool;
    }
    
    /**
     *  消息处理函数
     */    
    void MessageEntry(const MessageFrame& frame);
    
private:
    DriverManager();
    DISALLOW_COPY_AND_ASSIGN(DriverManager);
    
    STATUS StartMu(const string& name);
    void HandleAuthenticateEvent(const MessageFrame& frame);
    void HandleAuthorizeEvent(const MessageFrame& frame);
    void HandleSessionEvent(const MessageFrame& frame);    
    
    MessageUnit* _mu;
    UserPool* _upool;
    static DriverManager* _instance;
  
} ;

}
#endif 


