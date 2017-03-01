/******************************************************************************
* Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�cloud_node_manager.cpp
* �ļ���ʶ��
* ����ժҪ��cloud�ڵ����
* ��ǰ�汾��1.0
* ��    �ߣ�zhangyb
* ������ڣ�2012��8��23��
*
* �޸ļ�¼1��
*     �޸����ڣ�2012��8��23��
*     �� �� �ţ�V1.0
*     �� �� �ˣ�zhangyb
*     �޸����ݣ�����
******************************************************************************/

#include "cloud_node_manager.h"
#include "tecs_pub.h"
#include "sky.h"
#include "cloud_node.h"
#include "message.h"
#include "event.h"
#include "mid.h"
#include "tecs_errcode.h"
#include "log_agent.h"
#include "db_config.h"
//#include "ms_corelib.h"
#include "ms_config.h"




namespace zte_tecs
{
#define TIMER_QUERY_CLOUD_NODE      EV_TIMER_1  // ���ò�ѯ�ڵ㶨ʱ��

CloudNodeManager  *CloudNodeManager::_instance = NULL;

STATUS CloudNodeManager::StartMu(const string& name)
{
    if(_mu)
    {
        SkyAssert(false);
        return ERROR_DUPLICATED_OP;
    }
    
    // ��Ϣ��Ԫ�Ĵ����ͳ�ʼ��
    _mu = new MessageUnit(name);
    if (!_mu)
    {
        OutPut(SYS_EMERGENCY, "Create mu %s failed!\n",name.c_str());
        return ERROR_NO_MEMORY;
    }

    STATUS ret = _mu->SetHandler(this);
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "SetHandler mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }

    ret = _mu->Run();
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "Run mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }

    ret = _mu->Register();
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "Register mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }

    //���Լ������ϵ���Ϣ����ʹ��Ϣ��Ԫ״̬�л�������̬
    MessageFrame message(SkyInt32(0), EV_POWER_ON);
    ret = _mu->Receive(message);
    if (SUCCESS != ret)
    {
        SkyAssert(0);
        return ret;
    }
    _mu->Print();
    return SUCCESS;
}

void CloudNodeManager::MessageEntry(const MessageFrame& message)
{
    switch (_mu->get_state())
    {
        case S_Startup:
        {
            switch (message.option.id())
            {
                case EV_POWER_ON:
                {
                    DoStartUp();
                    _mu->set_state(S_Work);
                    OutPut(SYS_NOTICE, "%s power on!\n",_mu->name().c_str());
                    break;
                }
                default:
                    break;
            }
            break;
        }

        case S_Work:
        {
            switch (message.option.id())
            {
            case TIMER_QUERY_CLOUD_NODE:
            {
                QueryNode();
                break;
            }

            default:
            {
                OutPut(SYS_DEBUG, "Tc node mu receives a unkown message!");
                break;
            }
            }
            break;
        }

        default:
            break;
    }
}

STATUS CloudNodeManager::SetNode(const MessageFrame& message)
{
    vector<CloudNode> nodes;
    MessageNodeInfoReport msg;
    MID sender;

    msg.deserialize(message.data);

    sender = message.option.sender();

    OutPut(SYS_DEBUG, "recv from %s\r\n", sender._application.c_str());

    map<string,int>::iterator node_iter;
    for (node_iter = msg._node.begin();
         node_iter != msg._node.end();
         node_iter++ )
    {
        CloudNode node(node_iter->first, node_iter->second);
        nodes.push_back(node);
    }

    /* д�����ݿ� */
    _npool->UpdateNodes(nodes);
    return SUCCESS;
}

struct T_MS_Node
{
    string                     ip;      /* �ڵ��IP��ַ                      */
    uint32                     state;   /* �ڵ������״̬ */               
};

uint32  ms_query_node(const string &name,vector<T_MS_Node> &nodes)
{
    T_MS_Node node;
    node.ip="127.0.0.1";
    node.state=1;
    nodes.push_back(node);

    return 0;
}
void CloudNodeManager::SetQueryNodeTimer()
{ 
    //����ʱ5��Ķ�ʱ��ѯ
    STATUS      ret = 0;
    TIMER_ID    timer;
    // ���ö�ʱ��
    if (INVALID_TIMER_ID == (timer = _mu->CreateTimer()))
    {
        OutPut(SYS_EMERGENCY, "Cloud node manager create timer failed!\n");
        SkyAssert(INVALID_TIMER_ID != timer);
        SkyExit(-1, "Cloud node manager  creat timer failed.");
    }

    TimeOut timeout;
    timeout.type = LOOP_TIMER;
    timeout.duration = 5000;
    timeout.msgid = TIMER_QUERY_CLOUD_NODE;
    if (SUCCESS != (ret = _mu->SetTimer(timer,timeout)))
    {
        OutPut(SYS_EMERGENCY, "Cloud node manager set query node timer fail:%u!\n",ret);
        SkyAssert(0);
        SkyExit(-1, "Cloud node manager set query node timer fail!");
    }

    return;
}

void CloudNodeManager::DoStartUp()
{
    SetQueryNodeTimer();
    QueryNode();
}

STATUS CloudNodeManager::QueryNode()
{
    vector<CloudNode> nodes;
    MID sender;

     vector<T_MS_Node>ms_nodes;
     ms_query_node(ApplicationName(),ms_nodes);
     
     vector<T_MS_Node>::const_iterator it;
     for(it = ms_nodes.begin();it != ms_nodes.end();it++)
     {
         CloudNode node(it->ip,it->state);

         nodes.push_back(node);
     }

    /* д�����ݿ� */
    _npool->UpdateNodes(nodes);
    return SUCCESS;
}

void CloudNodeDbgReport(char *name,char* type,char *cmd)
{
    MessageUnit temp_mu(TempUnitName("NodeTest"));
    temp_mu.Register();
    MID mid(PROC_TC, MU_CLOUD_NODE_MANAGER);
    MessageOption option(mid, TIMER_QUERY_CLOUD_NODE,0,0);

    MessageNodeInfoReport msg;

    msg._node.insert(make_pair("127.0.0.1",1));
    msg._node.insert(make_pair("10.43.181.229",1));

    temp_mu.Send(msg, option);
}

DEFINE_DEBUG_FUNC(CloudNodeDbgReport);

} //end namespace zte_tecs

