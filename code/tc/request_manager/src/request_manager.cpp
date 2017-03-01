/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：request_manager.cpp
* 文件标识：见配置管理计划书
* 内容摘要：RequestManager类的实现文件
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
#include "sky.h"
#include "event.h"
#include "request_manager.h"
#include "user_common.h"
#include "db_config.h"

namespace zte_tecs
{
RequestManager *RequestManager::_instance = NULL;

/******************************************************************************/
STATUS RequestManager::Init()
{
    STATUS ret;
    //记录到日志
    OutPut(SYS_INFORMATIONAL, "Starting RequestManager...\n");

    //设置用户资源池对象指针
    _upool = UserPool::GetInstance();

    //设置Socket
    int rc = SetupSocket();
    if (0 != rc)
    {
        return ERROR_CREATE_SOCKET;
    }

    UserFullQuery::Init();

    //注册XMLRPC方法
    RegisterXmlrpcMethods();

    _rm_xmlrpc_server_thread = StartThread("XmlrpcServer",RmXmlrpcServerLoop,(void *)this);
    if (INVALID_THREAD_ID == _rm_xmlrpc_server_thread)
    {
        OutPut(SYS_EMERGENCY, "Failed to create XmlrpcServer thread!\n");
        return ERROR_CREATE_THREAD;
    }

    //创建启动MessageQueue线程
    _mq = new MessageQueue("RequestManager");
    ret = _mq->SetHandler(this);
    if (SUCCESS != ret)
    {
        return ret;
    }
    
    ret = _mq->Run();
    if (SUCCESS != ret)
    {
        //记录到日志
        OutPut(SYS_EMERGENCY, "Failed to call MessageQueue::Run()!\n");
        return ret;
    }

    //给自己发送上电消息，促使消息单元状态切换到工作态
    MessageFrame message(SkyInt32(0), EV_POWER_ON);
    return _mq->Receive(message);
}

/******************************************************************************/
void RequestManager::MessageEntry(const MessageFrame& message)
{
    switch (_mq->get_state())
    {
    case S_Startup:
    {
        switch (message.option.id())
        {
        case EV_POWER_ON:
        {
            _mq->set_state(S_Work);
            OutPut(SYS_NOTICE, "%s power on!\n",_mq->name().c_str());
            break;
        }
        default:
            OutPut(SYS_NOTICE, "Ignore any message except power on!\n");
            break;
        }
        break;
    }

    case S_Work:
    {
        switch (message.option.id())
        {
        case EV_FINALIZE:
        {
            //记录到日志
            OutPut(SYS_INFORMATIONAL, "Stopping Request Manager...\n");

            //pthread_cancel(_rm_xmlrpc_server_thread);
            //pthread_join(_rm_xmlrpc_server_thread,0);
            KillThread(_rm_xmlrpc_server_thread);

            //记录到日志
            OutPut(SYS_INFORMATIONAL, "XML-RPC server stopped.\n");

            delete _rm_xmlrpc_server;

            if (-1 == _socket_fd)
            {
                close(_socket_fd);
            }
        }
        break;
        default:
        {
            //记录到日志
            OutPut(SYS_ALERT, "Unknown message action: %d.\n",
                   message.option.id());
        }
        break;
        }
        break;
    }

    default:
        break;
    }
}

/******************************************************************************/
extern "C" void * RmXmlrpcServerLoop(void *arg)
{
    if (!arg)
    {
        return 0;
    }

    RequestManager *rm = static_cast<RequestManager *>(arg);

    // Set cancel state for the thread

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,0);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,0);

    //Start the server
    if (rm->_xml_log_file.length() > 0)
    {
        rm->_rm_xmlrpc_server = new xmlrpc_c::serverAbyss(
                                    xmlrpc_c::serverAbyss::constrOpt()
                                    .registryP(&rm->_rm_registry)
                                    .logFileName(rm->_xml_log_file)
                                    .socketFd(rm->_socket_fd));
    }
    else
    {
        rm->_rm_xmlrpc_server = new xmlrpc_c::serverAbyss(
                                    xmlrpc_c::serverAbyss::constrOpt()
                                    .registryP(&rm->_rm_registry)
                                    .socketFd(rm->_socket_fd));
    }
    rm->_rm_xmlrpc_server->run();

    return 0;
}

/******************************************************************************/
int RequestManager::SetupSocket()
{
    int                 rc;
    int                 yes = 1;
    struct sockaddr_in  rm_addr;

    _socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (-1 == _socket_fd)
    {
        //记录到日志
        OutPut(SYS_EMERGENCY, "Can not open server socket: %s.\n",strerror(errno));
        return -1;
    }

    rc = setsockopt(_socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

    if (-1 == rc)
    {
        //记录到日志
        OutPut(SYS_EMERGENCY, "Can not set socket options: %s.\n", strerror(errno));
        close(_socket_fd);
        return -1;
    }

    fcntl(_socket_fd,F_SETFD,FD_CLOEXEC); // Close socket in MADs

    rm_addr.sin_family      = AF_INET;
    rm_addr.sin_port        = htons(_port);
    rm_addr.sin_addr.s_addr = INADDR_ANY;

    rc = bind(_socket_fd, (struct sockaddr *) &(rm_addr), sizeof(struct sockaddr));

    if (-1 == rc)
    {
        //记录到日志
        OutPut(SYS_EMERGENCY,"Can not bind to port %d: %s\n", _port, strerror(errno));
        close(_socket_fd);
        return -1;
    }

    return 0;
}


/******************************************************************************/
//void RequestManager::RegisterXmlrpcMethods()移至register_xmlrpc_methods.cpp
//请到register_xmlrpc_methods.cpp注册XMLRPC方法


} //namespace zte_tecs
/******************************************************************************/

