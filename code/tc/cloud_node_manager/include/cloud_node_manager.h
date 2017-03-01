/******************************************************************************
* Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：cloud_node_manager.h
* 文件标识：
* 内容摘要：cloud节点数据管理的头文件
* 当前版本：1.0
* 作    者：zhangyb
* 完成日期：2012年8月23日
*
* 修改记录1：
*     修改日期：2012年8月23日
*     版 本 号：V1.0
*     修 改 人：zhangyb
*     修改内容：创建
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
        
        //启动消息单元工作线程
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

