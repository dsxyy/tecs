/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：request_manager.h
* 文件标识：见配置管理计划书
* 内容摘要：RequestManager类定义文件
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
#ifndef RM_REQUESTMANAGER_H
#define RM_REQUESTMANAGER_H
#include "sky.h"
#include "user_pool.h"
#include "log_agent.h"
#include "tecs_rpc_method.h"
#include <xmlrpc-c/server_abyss.hpp>
#include <xmlrpc-c/registry.hpp>
#define EV_FINALIZE 1

namespace zte_tecs
{
extern "C" void * RmXmlrpcServerLoop(void *arg);

class RequestManager : public MessageHandler
{
public:
    static RequestManager* GetInstance()
    {
        return _instance;
    };

    static RequestManager* CreateInstance(int port = 8080, const string& xml_log_file = "")
    {
        if (NULL == _instance)
        {
            _instance = new RequestManager(port,xml_log_file);
        }

        return _instance;
    };

    //析构函数
    ~RequestManager()
    {
        Finalize();
        delete _mq;
    };

    /**************************************************************************/
    /**
    @brief 功能描述: 启动XMLRPC Server和MessageQueue线程

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
        0    成功
        -1   失败
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    /**************************************************************************/
    STATUS Init();

    /**************************************************************************/
    /**
    @brief 功能描述: XMLRPC Server线程结束触发函数

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
    /**************************************************************************/
    void Finalize()
    {
        OutPut(SYS_INFORMATIONAL,"RequestManager is finalizing...\n");
        SkyInt data=0;
        Trigger(EV_FINALIZE,data);
    }

    /**************************************************************************/
    /**
    @brief 功能描述: 线程消息入口

    @li @b 入参列表：
    @verbatim
        frame     消息帧
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
    void MessageEntry(const MessageFrame& message);
    
    /**************************************************************************/
    /**
    @brief 功能描述: 设置用户资源池对象指针

    @li @b 入参列表：
    @verbatim
        upool     用户资源池对象指针
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
    void set_user_pool(UserPool * upool)
    {
        _upool = upool;
    }
    
    /**************************************************************************/
    /**
    @brief 功能描述: TecsRpcMethod注册封装接口

    @li @b 入参列表：
    @verbatim
        rpc_method   TecsRpcMethod方法对象指针
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
    void AddMethod(string method_name, TecsRpcMethod * rpc_method, 
                   string method_help = "")
    {
    #define METHODPTR(x) xmlrpc_c::methodPtr(dynamic_cast<xmlrpc_c::method*>(x))
        rpc_method->set_method_name(method_name);
        rpc_method->set_method_help(method_help);
        _rm_registry.addMethod(rpc_method->get_method_name(), 
                               METHODPTR(rpc_method));
    }
    

private:
    //构造函数
    RequestManager(int port, const string& xml_log_file):
        _port(port), _socket_fd(-1), _xml_log_file(xml_log_file), _upool(0)
    {
        _mq = NULL;
    };
    static RequestManager *_instance;

    THREAD_ID _rm_xmlrpc_server_thread;         //Thread id for the XML Server

    int _port;              //Port number where the connection will be open

    int _socket_fd;         //FD for the XML server socket

    string _xml_log_file;                       //XMLRPC服务器日志输出文件

    MessageQueue* _mq;                   //MessageQueue对象指针

    xmlrpc_c::registry _rm_registry;            //XML-RPC方法注册器

    xmlrpc_c::serverAbyss* _rm_xmlrpc_server;   //XML-RPC服务器
    
    UserPool * _upool;                          //用户资源池对象指针

    /**************************************************************************/
    /**
    @brief 功能描述: MessageQueue线程消息触发器

    @li @b 入参列表：
    @verbatim
        frame     消息帧
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
    STATUS Trigger(uint32 id,const Serializable& data)
    {
        MessageFrame message(data,id);
        return _mq->Receive(message);
    };

    /**************************************************************************/
    /**
    @brief 功能描述: 请求管理XMLRPC服务线程

    @li @b 入参列表：
    @verbatim
        arg     入参之间
    @endverbatim

    @li @b 出参列表：
    @verbatim
        无
    @endverbatim

    @li @b 返回值列表：
    @verbatim
        线程句柄
    @endverbatim

    @li @b 接口限制：友元函数
    @li @b 其它说明：无
    */
    /**************************************************************************/
    friend void * RmXmlrpcServerLoop(void* arg);

    /**************************************************************************/
    /**
    @brief 功能描述: 注册XMLRPC方法到XMLRPC Server

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
    /**************************************************************************/
    void RegisterXmlrpcMethods();

    /**************************************************************************/
    /**
    @brief 功能描述: 设置XMLRPC Server套接字

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
        0    成功
        -1   失败
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    /**************************************************************************/
    int SetupSocket();

    // 禁用拷贝构造
    DISALLOW_COPY_AND_ASSIGN(RequestManager);

};

}  //namespace zte_tecs
#endif // #ifndef RM_REQUESTMANAGER_H

