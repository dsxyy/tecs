/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�request_manager.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��RequestManager�ඨ���ļ�
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

    //��������
    ~RequestManager()
    {
        Finalize();
        delete _mq;
    };

    /**************************************************************************/
    /**
    @brief ��������: ����XMLRPC Server��MessageQueue�߳�

    @li @b ����б�
    @verbatim
        ��
    @endverbatim

    @li @b �����б�
    @verbatim
        ��
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
        0    �ɹ�
        -1   ʧ��
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    /**************************************************************************/
    STATUS Init();

    /**************************************************************************/
    /**
    @brief ��������: XMLRPC Server�߳̽�����������

    @li @b ����б�
    @verbatim
        ��
    @endverbatim

    @li @b �����б�
    @verbatim
        ��
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
        ��
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
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
    @brief ��������: �߳���Ϣ���

    @li @b ����б�
    @verbatim
        frame     ��Ϣ֡
    @endverbatim

    @li @b �����б�
    @verbatim
        ��
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
        ��
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    /**************************************************************************/
    void MessageEntry(const MessageFrame& message);
    
    /**************************************************************************/
    /**
    @brief ��������: �����û���Դ�ض���ָ��

    @li @b ����б�
    @verbatim
        upool     �û���Դ�ض���ָ��
    @endverbatim

    @li @b �����б�
    @verbatim
        ��
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
        ��
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    /**************************************************************************/
    void set_user_pool(UserPool * upool)
    {
        _upool = upool;
    }
    
    /**************************************************************************/
    /**
    @brief ��������: TecsRpcMethodע���װ�ӿ�

    @li @b ����б�
    @verbatim
        rpc_method   TecsRpcMethod��������ָ��
    @endverbatim

    @li @b �����б�
    @verbatim
        ��
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
        ��
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
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
    //���캯��
    RequestManager(int port, const string& xml_log_file):
        _port(port), _socket_fd(-1), _xml_log_file(xml_log_file), _upool(0)
    {
        _mq = NULL;
    };
    static RequestManager *_instance;

    THREAD_ID _rm_xmlrpc_server_thread;         //Thread id for the XML Server

    int _port;              //Port number where the connection will be open

    int _socket_fd;         //FD for the XML server socket

    string _xml_log_file;                       //XMLRPC��������־����ļ�

    MessageQueue* _mq;                   //MessageQueue����ָ��

    xmlrpc_c::registry _rm_registry;            //XML-RPC����ע����

    xmlrpc_c::serverAbyss* _rm_xmlrpc_server;   //XML-RPC������
    
    UserPool * _upool;                          //�û���Դ�ض���ָ��

    /**************************************************************************/
    /**
    @brief ��������: MessageQueue�߳���Ϣ������

    @li @b ����б�
    @verbatim
        frame     ��Ϣ֡
    @endverbatim

    @li @b �����б�
    @verbatim
        ��
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
        ��
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    /**************************************************************************/
    STATUS Trigger(uint32 id,const Serializable& data)
    {
        MessageFrame message(data,id);
        return _mq->Receive(message);
    };

    /**************************************************************************/
    /**
    @brief ��������: �������XMLRPC�����߳�

    @li @b ����б�
    @verbatim
        arg     ���֮��
    @endverbatim

    @li @b �����б�
    @verbatim
        ��
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
        �߳̾��
    @endverbatim

    @li @b �ӿ����ƣ���Ԫ����
    @li @b ����˵������
    */
    /**************************************************************************/
    friend void * RmXmlrpcServerLoop(void* arg);

    /**************************************************************************/
    /**
    @brief ��������: ע��XMLRPC������XMLRPC Server

    @li @b ����б�
    @verbatim
        ��
    @endverbatim

    @li @b �����б�
    @verbatim
        ��
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
        ��
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    /**************************************************************************/
    void RegisterXmlrpcMethods();

    /**************************************************************************/
    /**
    @brief ��������: ����XMLRPC Server�׽���

    @li @b ����б�
    @verbatim
        ��
    @endverbatim

    @li @b �����б�
    @verbatim
        ��
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
        0    �ɹ�
        -1   ʧ��
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    /**************************************************************************/
    int SetupSocket();

    // ���ÿ�������
    DISALLOW_COPY_AND_ASSIGN(RequestManager);

};

}  //namespace zte_tecs
#endif // #ifndef RM_REQUESTMANAGER_H

