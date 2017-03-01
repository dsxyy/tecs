/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：TecsRpcMethod.h
* 文件标识：见配置管理计划书
* 内容摘要：TecsRpcMethod类的定义文件
* 当前版本：1.0
* 作    者：马兆勉
* 完成日期：2011年7月22日
*
* 修改记录1：
*     修改日期：2011/7/22
*     版 本 号：V1.0
*     修 改 人：马兆勉
*     修改内容：创建
*******************************************************************************/
#ifndef RM_TECSRPCMETHOD_H
#define RM_TECSRPCMETHOD_H
#include "user_pool.h"
#include "log_agent.h"
#include "operator_log.h"
#include "api_error.h"
#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/registry.hpp>
#include <xmlrpc-c/server_abyss.hpp>

// 禁用拷贝构造宏定义
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)
#endif
using namespace xmlrpc_c;

namespace zte_tecs
{
#define RPC_ERROR_MAP int RpcError(int inner_error)
#define ERR_MAP_BEGIN switch(inner_error) {
#define ERR_MAP_PUB case SUCCESS: return TECS_SUCCESS; break;
#define ERR_MAP(inner,rpc) case inner: return rpc; break;
#define ERR_MAP_END default: return TECS_ERR_UNKNOWN; break;}
#define METHOD_SET            0
#define METHOD_READONLY       1

#define xINT(x)   (xmlrpc_c::value_int(x))
#define xSTR(x)   (xmlrpc_c::value_string(x))
#define xI8(x)    (xmlrpc_c::value_i8(x))
#define xARRAY(x) (xmlrpc_c::value_array(x))

#define xRET1(a) \
{\
    *retval = a;\
}

#define xRET2(a,b) \
{\
    vector<value> array_data;\
    array_data.push_back(a);\
    array_data.push_back(b);\
    value_array *array_result = new xmlrpc_c::value_array(array_data);\
    *retval = *array_result;\
    delete array_result;\
}

#define xRET3(a,b,c) \
{\
    vector<value> array_data;\
    array_data.push_back(a);\
    array_data.push_back(b);\
    array_data.push_back(c);\
    value_array *array_result = new xmlrpc_c::value_array(array_data);\
    *retval = *array_result;\
    delete array_result;\
}

#define xRET4(a,b,c,d) \
{\
    vector<value> array_data;\
    array_data.push_back(a);\
    array_data.push_back(b);\
    array_data.push_back(c);\
    array_data.push_back(d);\
    value_array *array_result = new xmlrpc_c::value_array(array_data);\
    *retval = *array_result;\
    delete array_result;\
}

#define xRET5(a,b,c,d,e) \
{\
    vector<value> array_data;\
    array_data.push_back(a);\
    array_data.push_back(b);\
    array_data.push_back(c);\
    array_data.push_back(d);\
    array_data.push_back(e);\
    value_array *array_result = new xmlrpc_c::value_array(array_data);\
    *retval = *array_result;\
    delete array_result;\
}

class TecsRpcMethod : public xmlrpc_c::method2
{
public:
    TecsRpcMethod() { _method_type = METHOD_SET; };             //默认构造函数
    virtual ~TecsRpcMethod(){};    //默认析构函数
    virtual RPC_ERROR_MAP { return TECS_ERR_UNKNOWN;};     //返回码转换函数
    
    /**************************************************************************/
    /**
    @brief 功能描述: xmlrpc_c::method命令执行入口

    @li @b 入参列表：
    @verbatim
        paramList  XMLRPC参数列表
        callInfoP  XMLRPC调用上下文
    @endverbatim

    @li @b 出参列表：
    @verbatim
        retval     XMLRPC返回参数指针
    @endverbatim

    @li @b 返回值列表：
    @verbatim
        无
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    /**************************************************************************/
    void execute(const xmlrpc_c::paramList& paramList,
                 const xmlrpc_c::callInfo * const  callInfoP,
                 xmlrpc_c::value* const retval);

    /**************************************************************************/
    /**
    @brief 功能描述: TecsRpcMethod命令执行虚拟入口

    @li @b 入参列表：
    @verbatim
        paramList  XMLRPC参数列表
    @endverbatim

    @li @b 出参列表：
    @verbatim
        retval     XMLRPC返回参数指针
    @endverbatim

    @li @b 返回值列表：
    @verbatim
        无
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    /**************************************************************************/
    virtual void MethodEntry(const xmlrpc_c::paramList& paramList,
                             xmlrpc_c::value* const retval) = 0;

    /**************************************************************************/
    /**
    @brief 功能描述: 获取注册方法名称

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
        注册方法名称
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    /**************************************************************************/
    string get_method_name() const { return _method_name; };
    /**************************************************************************/
    /**
    @brief 功能描述: 获取注册方法类型

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
        注册方法类型
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    /**************************************************************************/
    int get_method_type() const { return _method_type; };
    /**************************************************************************/
    /**
    @brief 功能描述: 设置注册方法名称

    @li @b 入参列表：
    @verbatim
        method_name  注册方法名称
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
    /**************************************************************************/
    void set_method_name(const string& method_name) 
    { 
        _method_name = method_name; 
        if (string::npos != _method_name.find(".query", 0))
        {
            _method_type = METHOD_READONLY;
        }
        else
            _method_type = METHOD_SET;
    };

    /**************************************************************************/
    /**
    @brief 功能描述: 设置注册方法帮助信息

    @li @b 入参列表：
    @verbatim
        method_help  注册方法帮助信息
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
    /**************************************************************************/
    void set_method_help(const string& method_help) 
    {
        if (method_help.empty())
            return;
        _help = method_help; 
    };

    
    /**************************************************************************/
    /**
    @brief 功能描述: 获取方法调用者ID

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
        方法调用者ID
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    /**************************************************************************/
    int64 get_userid() const { return _user_id; };

    /**************************************************************************/
    /**
    @brief 功能描述: 获取方法调用者名称

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
        方法调用者名称
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    /**************************************************************************/
    string get_username() const { return _user_name; };
    
protected:

    UserPool * _upool;        //用户资源池指针
    string     _method_name;  //方法注册名称
    int64      _user_id;      //方法调用者ID
    string     _user_name;    //方法调用者名称
    int        _method_type;   //方法类型
    /**************************************************************************/
    /**
    @brief 功能描述: 构造鉴权处理接口错误格式化输出

    @li @b 入参列表：
    @verbatim
        method    注册方法名称
    @endverbatim

    @li @b 出参列表：
    @verbatim
          无
    @endverbatim

    @li @b 返回值列表：
    @verbatim
        鉴权失败错误格式化字符串
    @endverbatim

    @li @b 接口限制：静态成员函数
    @li @b 其它说明：无
    */
    /**************************************************************************/
    static string HandleError(const string& method);

    /**************************************************************************/
    /**
    @brief 功能描述: 构造鉴权失败错误格式化输出

    @li @b 入参列表：
    @verbatim
        method    注册方法名称
    @endverbatim

    @li @b 出参列表：
    @verbatim
          无
    @endverbatim

    @li @b 返回值列表：
    @verbatim
        鉴权失败错误格式化字符串
    @endverbatim

    @li @b 接口限制：静态成员函数
    @li @b 其它说明：无
    */
    /**************************************************************************/
    static string AuthenticateError(const string& method);

    /**************************************************************************/
    /**
    @brief 功能描述: 构造授权失败错误格式化输出

    @li @b 入参列表：
    @verbatim
        method     注册方法名称
        object     对象名称
        action     动作名称
        user_id    用户标识
        object_id  对象标识
    @endverbatim

    @li @b 出参列表：
    @verbatim
          无
    @endverbatim

    @li @b 返回值列表：
    @verbatim
        授权失败错误格式化字符串
    @endverbatim

    @li @b 接口限制：静态成员函数
    @li @b 其它说明：无
    */
    /**************************************************************************/
    static string AuthorizeError(const string& method,
                                 const string& object,
                                 const string& action,
                                 int64 user_id,
                                 int64 object_id);

    /**************************************************************************/
    /**
    @brief 功能描述: 构造获取对象失败错误格式化输出

    @li @b 入参列表：
    @verbatim
        method     注册方法名称
        object     对象名称
        object_id  对象标识
    @endverbatim

    @li @b 出参列表：
    @verbatim
          无
    @endverbatim

    @li @b 返回值列表：
    @verbatim
        获取对象失败错误格式化字符串
    @endverbatim

    @li @b 接口限制：静态成员函数
    @li @b 其它说明：无
    */
    /**************************************************************************/
    static string GetObjectError(const string& method,
                                 const string& object,
                                 int64 object_id);

    /**************************************************************************/
    /**
    @brief 功能描述: 构造动作失败错误格式化输出

    @li @b 入参列表：
    @verbatim
        method     注册方法名称
        object     对象名称
        action     动作名称
        object_id  对象标识
        error_code 错误码
    @endverbatim

    @li @b 出参列表：
    @verbatim
          无
    @endverbatim

    @li @b 返回值列表：
    @verbatim
        动作失败错误格式化字符串
    @endverbatim

    @li @b 接口限制：静态成员函数
    @li @b 其它说明：无
    */
    /**************************************************************************/
    static string ActionError(const string& method,
                              const string& object,
                              const string& action,
                              int64 object_id,
                              int64 error_code);

    /**************************************************************************/
    /**
    @brief 功能描述: 构造会话检查错误格式化输出

    @li @b 入参列表：
    @verbatim
        method     注册方法名称
    @endverbatim

    @li @b 出参列表：
    @verbatim
          无
    @endverbatim

    @li @b 返回值列表：
    @verbatim
        会话检查失败错误格式化字符串
    @endverbatim

    @li @b 接口限制：静态成员函数
    @li @b 其它说明：无
    */
    /**************************************************************************/
    static string SessionError(const string& method);
    
    /**************************************************************************/
    /**
    @brief 功能描述: 打印参数列表到XML字符串

    @li @b 入参列表：
    @verbatim
        paramList     参数列表
    @endverbatim

    @li @b 出参列表：
    @verbatim
          无
    @endverbatim

    @li @b 返回值列表：
    @verbatim
        参数列表到XML字符串
    @endverbatim

    @li @b 接口限制：静态成员函数
    @li @b 其它说明：无
    */
    /**************************************************************************/
    static string ParamListToXml(const xmlrpc_c::paramList& paramList);
    
    /**************************************************************************/
    /**
    @brief 功能描述: 打印XMLRPC参数到XML字符串

    @li @b 入参列表：
    @verbatim
        xmlrpc_value     XMLRPC参数
    @endverbatim

    @li @b 出参列表：
    @verbatim
          无
    @endverbatim

    @li @b 返回值列表：
    @verbatim
        参数XML格式化字符串
    @endverbatim

    @li @b 接口限制：静态成员函数
    @li @b 其它说明：无
    */
    /**************************************************************************/
    static string ParamValue(const xmlrpc_c::value xmlrpc_value);

private:
    
    
    /**************************************************************************/
    /**
    @brief 功能描述: 设置方法调用者ID

    @li @b 入参列表：
    @verbatim
        user_id   方法调用者ID
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
    /**************************************************************************/
    void set_userid(int64 user_id) { _user_id = user_id; };
    
   /**************************************************************************/
    /**
    @brief 功能描述: 设置方法调用者名称

    @li @b 入参列表：
    @verbatim
        user_name   方法调用者名称
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
    /**************************************************************************/
    void set_username(const string& user_name) { _user_name = user_name; };
    
    // 禁用拷贝构造
    DISALLOW_COPY_AND_ASSIGN(TecsRpcMethod);

};

}  //namespace zte_tecs
#endif // #ifndef RM_TECSRPCMETHOD_H

