/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：cluster_manager.h
* 文件标识：见配置管理计划书
* 内容摘要：cluster_manager类的声明文件
* 当前版本：1.0
* 作    者：李孝成
* 完成日期：2011年7月25日
*
* 修改记录1：
*     修改日期：2011/7/25
*     版 本 号：V1.0
*     修 改 人：李孝成
*     修改内容：创建
******************************************************************************/
#ifndef TC_CLUSTER_MANAGER_H
#define TC_CLUSTER_MANAGER_H
#include "sky.h"
#include "cluster_pool.h"
#include "mid.h"
#include "event.h"

namespace zte_tecs
{
/* 定义集群的名称 */
#define CLUSTER_CC_CLUSTER_AGENT_UNIT_NAME   MU_CLUSTER_AGENT    // CC的主机管理消息单元名称
#define CLUSTER_PERIOD_TIME_LEN            10000              // 集群老化定时器时长，10s

#define CLUSTER_QUERY_SYN                  1                  // 同步查询
#define CLUSTER_QUERY_ASYN                 0                  // 异步查询

/* 外部同步查询集群保存会话的临时结构 */
class ClusterResourceSynReq 
{
    public:
        MID    _sender;
        int64  _start_index;
        int64  _query_count;
};


/**
@brief 功能描述: 集群资源管理模块的类，主要包含集群的注册管理，集群状态管理
@li @b 其它说明：无
*/
class ClusterManager: public MessageHandler
{
public:
    static ClusterManager *GetInstance()
    {
        if(NULL == instance)
        {
            instance = new ClusterManager();
        }
        
        return instance;
    };

    STATUS Init()
    {
        _cpool = ClusterPool::GetInstance();
        if(!_cpool)
        {
            return ERROR_NOT_READY;
        }

        _manager_period_timer = 0;
        _req_cluster_timer    = 0;
        _req_cluster_mid_tab.clear();
        _ClusterNameTab.clear();
        
        //启动消息单元工作线程
        return StartMu(MU_CLUSTER_MANAGER);
    }
   
    ~ClusterManager()
    {
        delete _mu;
    }
    
    void MessageEntry(const MessageFrame& message);

    void SetIp(string ip)
    {
        _ip_connect_lowstream = ip;
    }

    string GetIp()
    {
        return _ip_connect_lowstream;
    }

private:
    ClusterManager()
    {
        _cpool = NULL;
        _mu = NULL;
    }
    
    static ClusterManager               *instance;
    ClusterPool                         * _cpool;
    MessageUnit                         * _mu;
    TIMER_ID                            _manager_period_timer;
    vector<ClusterResourceSynReq>       _req_cluster_mid_tab;
    map<string,int>                     _ClusterNameTab;
    TIMER_ID                            _req_cluster_timer;
    string                              _ip_connect_lowstream;
    map<string, string>                 _ifs_info;

    STATUS StartMu(const string& name);

    void doClusterDiscover(const MessageFrame& message);

    void UpdateClusterInfo(const string &cluster_name, const MessageFrame& message);
    
    void doAgentRegCluster(const MessageFrame& message);//该函数保留测试用

    void doAgentSetCluster(const MessageFrame& message);
    
    void doClusterRegAck(const MessageFrame& message);

    void doAgentUnRegCluster(const MessageFrame& message);//该函数保留测试用
    
    void doAgentForgetCluster(const MessageFrame& message);    

    void doAgentQueryClusterInfo(const MessageFrame& message);

    void doClusterInfoReport(const MessageFrame& message);

    void doVmCfgFindCluster(const MessageFrame& message);

    void doAgentStopCluster(const MessageFrame& message);

    void doAgentStartCluster(const MessageFrame& message);

    void doAgentGetClusterCoreAndTcu(const MessageFrame& message);

    
/*****************************************************************************/
/**
@brief 功能描述: 集群资源查询应答发送函数
    
@li @b 入参列表：
@verbatim
sender    发送请求消息的请求者地址  
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
/*****************************************************************************/   
    void ClusterResourceAckSend(ClusterResourceSynReq &ReqInfo);
 
/*****************************************************************************/
/**
@brief 功能描述: 集群管理进程给所有CC发送集群资源上报请求
    
@li @b 入参列表：
@verbatim
sender    发送请求消息的请求者地址  
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
/*****************************************************************************/    
    void ResourceReqSendToAllCluster(ClusterResourceSynReq &ReqInfo);

/*****************************************************************************/
/**
@brief 功能描述: 为集群管理进程设置一个循环定时器，
    
@li @b 入参列表：
@verbatim
sender    发送请求消息的请求者地址  
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
@li @b 其它说明：该函数调用多次，也只会设置一次循环定时器
*/
/*****************************************************************************/      
    void SetClusterKeepTimer();
};


} /* end namespace zte_tecs */

#endif /* end TC_CLUSTER_MANAGER_H */

