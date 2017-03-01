/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�request_manager.cpp
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��RequestManager���ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�������
* ������ڣ�2011��7��22��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/7/22
*     �� �� �ţ�V1.0
*     �� �� �ˣ�������
*     �޸����ݣ�����
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
    //��¼����־
    OutPut(SYS_INFORMATIONAL, "Starting RequestManager...\n");

    //�����û���Դ�ض���ָ��
    _upool = UserPool::GetInstance();

    //����Socket
    int rc = SetupSocket();
    if (0 != rc)
    {
        return ERROR_CREATE_SOCKET;
    }

    UserFullQuery::Init();

    //ע��XMLRPC����
    RegisterXmlrpcMethods();

    _rm_xmlrpc_server_thread = StartThread("XmlrpcServer",RmXmlrpcServerLoop,(void *)this);
    if (INVALID_THREAD_ID == _rm_xmlrpc_server_thread)
    {
        OutPut(SYS_EMERGENCY, "Failed to create XmlrpcServer thread!\n");
        return ERROR_CREATE_THREAD;
    }

    //��������MessageQueue�߳�
    _mq = new MessageQueue("RequestManager");
    ret = _mq->SetHandler(this);
    if (SUCCESS != ret)
    {
        return ret;
    }
    
    ret = _mq->Run();
    if (SUCCESS != ret)
    {
        //��¼����־
        OutPut(SYS_EMERGENCY, "Failed to call MessageQueue::Run()!\n");
        return ret;
    }

    //���Լ������ϵ���Ϣ����ʹ��Ϣ��Ԫ״̬�л�������̬
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
            //��¼����־
            OutPut(SYS_INFORMATIONAL, "Stopping Request Manager...\n");

            //pthread_cancel(_rm_xmlrpc_server_thread);
            //pthread_join(_rm_xmlrpc_server_thread,0);
            KillThread(_rm_xmlrpc_server_thread);

            //��¼����־
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
            //��¼����־
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
        //��¼����־
        OutPut(SYS_EMERGENCY, "Can not open server socket: %s.\n",strerror(errno));
        return -1;
    }

    rc = setsockopt(_socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

    if (-1 == rc)
    {
        //��¼����־
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
        //��¼����־
        OutPut(SYS_EMERGENCY,"Can not bind to port %d: %s\n", _port, strerror(errno));
        close(_socket_fd);
        return -1;
    }

    return 0;
}


/******************************************************************************/
//void RequestManager::RegisterXmlrpcMethods()����register_xmlrpc_methods.cpp
//�뵽register_xmlrpc_methods.cppע��XMLRPC����


} //namespace zte_tecs
/******************************************************************************/

