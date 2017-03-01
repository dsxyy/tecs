/******************************************************************************
* Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�cloud_node_manager.h
* �ļ���ʶ��
* ����ժҪ��cloud�ڵ����ݹ����ͷ�ļ�
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

#ifndef TC_CLOUD_NODE_MANAGER_H
#define TC_CLOUD_NODE_MANAGER_H
#include "sky.h"
#include "cloud_node.h"
#include "mid.h"
#include "event.h"

namespace zte_tecs
{
class MessageNodeInfoReport : public Serializable
{
public:
    MessageNodeInfoReport() {};
    ~MessageNodeInfoReport() {};
    
    SERIALIZE 
    {
        SERIALIZE_BEGIN(MessageNodeInfoReport);
        WRITE_STRING_MAP(_node);
        SERIALIZE_END();    
    };
    
    DESERIALIZE 
    {
        DESERIALIZE_BEGIN(MessageNodeInfoReport);
        READ_STRING_MAP(_node);
        DESERIALIZE_END();    
    };
    
    map<string,int>_node;    
};

class CloudNodeManager: public MessageHandler
{
public:
    static CloudNodeManager *GetInstance()
    {
        if(NULL == _instance)
        {
            _instance = new CloudNodeManager();
        }
        
        return _instance;
    };

    STATUS Init()
    {
        _npool = CloudNodePool::GetInstance();
        if(!_npool)
        {
            return ERROR_NOT_READY;
        }
        
        //������Ϣ��Ԫ�����߳�
        return StartMu(MU_CLOUD_NODE_MANAGER);
    }
   
    ~CloudNodeManager()
    {
        delete _mu;
    }
    
    void MessageEntry(const MessageFrame& message);

private:
    CloudNodeManager()
    {
        _npool = NULL;
        _mu = NULL;
    }
    
    static CloudNodeManager *_instance;
    CloudNodePool           *_npool;
    MessageUnit             *_mu;
    void SetQueryNodeTimer();
    STATUS StartMu(const string& name);    
    STATUS QueryNode();
    STATUS SetNode(const MessageFrame& message);
    void DoStartUp();
    DISALLOW_COPY_AND_ASSIGN(CloudNodeManager);
};


} /* end namespace zte_tecs */

#endif /* end TC_CLOUD_NODE_MANAGER_H */

