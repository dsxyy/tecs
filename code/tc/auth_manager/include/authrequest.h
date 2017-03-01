/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：authrequest.h
* 文件标识：见配置管理计划书
* 内容摘要：authrequest类的定义文件
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
#ifndef AM_AUTHREQUEST_H
#define AM_AUTHREQUEST_H
#include "sky.h"
#include "auth_config.h"
#include "log_agent.h"
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
@brief 功能描述: 鉴权请求类，用于定义鉴权或授权请求的上下文信息\n
@li @b 其它说明: 无
*/  
class AuthRequest
{
    friend class AuthManager;
public:

    /**
     *  授权操作类型
     */
    enum Operation
    {
        CREATE,  /** Authorization to create an object (host, vm, net, image)*/
        DELETE,  /** Authorization to delete an object */
        USE,     /** Authorization to use an object */
        MANAGE,  /** Authorization to manage an object */
        INFO     /** Authorization to view an object */
    };

    /**
     *  授权对象类型
     */
    enum Object
    {
        VM,
        HOST,
        NET,
        IMAGE,
        USER,
        USERGROUP,
        CLUSTER,
        PROJECT,
        VMTEMPLATE,
        VMCONFIG,
        VERSION,
        SA,
        FILE,
        AFFINITY_REGION
    };
    
    /**************************************************************************/
    /**
    @brief 功能描述: 构造函数，初始化变量

    @li @b 入参列表：
    @verbatim
        uid 用户id
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
    explicit AuthRequest(int64 uid);
    
    /**************************************************************************/
    /**
    @brief 功能描述: 析构函数，释放相关内存

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
    ~AuthRequest();
    
    /**************************************************************************/
    /**
    @brief 功能描述: 增加鉴权
    @li @b 入参列表：
    @verbatim
        username 用户名
        password  密码
        session     会话信息
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
    void AddAuthenticate(const string &username,
                          const string &password,
                          const string &secret);
                          
    /**************************************************************************/
    /**
    @brief 功能描述: 增加授权
    @li @b 入参列表：
    @verbatim
        ob        操作对象
        ob_id    操作对象id
        op        操作类型
        owner   对象所有者的uid    
        pub      是否公开
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
    void AddAuthorize(Object        ob,
                       const string& ob_id,
                       Operation     op,
                       int64         owner,
                       bool          pub);

    void AddAuthorize(Object        ob,
                       int64        ob_id,
                       Operation     op,
                       int64         owner,
                       bool          pub);
                       
    /**************************************************************************/
    /**
     *
     */
    
    void Send(const Serializable& msg, MessageOption& option)
    {
        _mu->Send(msg, option);
    }
    
    /**************************************************************************/
    /**
    @brief 功能描述: 等待请求处理完成
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
    void Wait(DriverMessage& msg);
    
    /**************************************************************************/
    /**
    @brief 功能描述: 请求处理完成通知
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
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    void Notify();
    
    /**************************************************************************/
    /**
    @brief 功能描述:平授权操作
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
      1-成功，0失败
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    bool PlainAuthorize();
    
    /**************************************************************************/
    /**
    @brief 功能描述:平鉴权操作
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
        1-成功，0失败
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    bool PlainAuthenticate();
    
    /**************************************************************************/
    /**
    @brief 功能描述: 获取请求队列
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
        得到请求队列字符串
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    const AuthorizeMessage& get_auth_req();
 
    /**************************************************************************/
    /**
    @brief 功能描述:远程鉴权结果
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
    bool set_result(const bool result);
    
    /**************************************************************************/
    /**
    @brief 功能描述:得到远程鉴权结果
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
    bool get_result();      
    
    /**************************************************************************/
    /**
    @brief 功能描述:设置鉴权结果字符串
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
    bool set_message(const string& message);
    
    /**************************************************************************/
    /**
    @brief 功能描述:得到鉴权结果字符串
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
    string get_message();

    
private:

    DISALLOW_COPY_AND_ASSIGN(AuthRequest);
    
    /**
    *  对字符串进行base64编码
    */
    static string * Base64Encode(const string& in);
    
    /**
     *  用户id
     */
    int64 _uid;
    
    /**
     * 鉴权用户的用户名
     */
    string _username;

    /**
     *  鉴权用户的密码
     */
    string _password;

    /**
     *  鉴权用户的session信息
     */
    string _secret;
    
    /**
     * 鉴权请求队列
     */
    AuthorizeMessage _auth_req;

    /**
     *  是否为平鉴权
     */
    bool _self_authorize;
    
    /**
     *  消息单元
     */ 
    MessageUnit *_mu;
   
    /**
     *  鉴权结果
     */
    bool  _result;

    /**
     *  远程鉴权的消息
     */
    string _message;

}; 

}
#endif 
