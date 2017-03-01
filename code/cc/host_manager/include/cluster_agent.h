/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：host_manager.h
* 文件标识：见配置管理计划书
* 内容摘要：HostManager类定义文件
* 当前版本：1.0
* 作    者：袁进坤
* 完成日期：2011年8月9日
*
* 修改记录1：
*     修改日期：2011/8/9
*     版 本 号：V1.0
*     修 改 人：袁进坤
*     修改内容：创建
*******************************************************************************/
#ifndef HM_CLUSTER_AGENT_H
#define HM_CLUSTER_AGENT_H
#include "sky.h"
#include "mid.h"
#include "log_agent.h"
#include "host_pool.h" 
#include "config_cluster.h"
#include "config_tcu.h"
#include "config_coefficient.h"
#include "host_cpuinfo.h"
#include "config_cmm_pool.h"

namespace zte_tecs 
{

/**
@brief 功能描述: 物理机管理的实体类\n
@li @b 其它说明: 无
*/
class ClusterAgent : public MessageHandler
{
public:
    static ClusterAgent *GetInstance()
    {
        if (NULL == _instance)
        {
            _instance = new ClusterAgent();
        }
        return _instance;

    }

    STATUS Init()
    {
        // 获取主机池对象指针
        if (NULL == (_host_pool = HostPool::GetInstance()))
        {
            SkyAssert(0);
            OutPut(SYS_EMERGENCY, "Get host pool failed!\n");
            return ERROR_NOT_READY;
        }    

        if (NULL == (_tcu_config = ConfigTcu::GetInstance()))
        {
            SkyAssert(0);
            OutPut(SYS_EMERGENCY, "Get config tcu failed!\n");
            return ERROR_NOT_READY;
        }

        if (NULL == (_cluster_config = ConfigCluster::GetInstance()))
        {
            SkyAssert(0);
            OutPut(SYS_EMERGENCY, "Get ConfigCluster failed!\n");
            return ERROR_NOT_READY;
        }

        if (NULL == (_cmm_config = ConfigCmmPool::GetInstance()))
        {
            SkyAssert(0);
            OutPut(SYS_EMERGENCY, "Get ConfigCmmPool failed!\n");
            return ERROR_NOT_READY;
        }

        return StartMu(MU_CLUSTER_AGENT);
    }
    
    virtual ~ClusterAgent() 
    { 
        _instance = NULL; 
    }
    
private:
    MessageUnit         *_mu;               // 消息单元
    bool                _have_startup;      // 是否已启动的标志，避免重新初始化
    string              _tecs_name;         // 本集群归属的tecs系统名称
    string              _cluster_verify_id; // 集群接入TC的验证码，用于区分重名集群
    ConfigCluster       *_cluster_config;   // 本集群的配置项
    ConfigTcu           *_tcu_config;       // 本集群tcu的配置
    HostPool            *_host_pool;        // 本集群所管理的主机资源池
    ConfigCmmPool       *_cmm_config;       // 本集群的cmm配置项
    string              _cc_ip_address;
    string              _tc_ip_address;

    static ClusterAgent *_instance;      

    ClusterAgent();
    virtual void MessageEntry(const MessageFrame &message); //消息处理入口函数
    STATUS StartMu(const string& name);

    void ExitTecsGroup();
    void JoinTecsGroup();
    void SetTecsName(const string &tecs_name);
    
    void DoRegisteredSystem();
    void DoRegisteredSystem(const MessageFrame &message);

    // 消息处理函数
    void DoStartUp();
    void DoClusterRegister(const MessageFrame &message);
    void DoClusterUnregister(const MessageFrame &message);
    void DoClusterConfig(const MessageFrame &message);
    void DoClusterConfigQuery(const MessageFrame &message);
    void DoClusterTcuConfig(const MessageFrame &message);
    void DoClusterTcuQuery(const MessageFrame &message);
    void DoClusterInfoReport();
    void DoClusterCmmAdd(const MessageFrame &message);
    void DoClusterCmmSet(const MessageFrame &message);
    void DoClusterCmmDelete(const MessageFrame &message);
    void DoClusterCmmQuery(const MessageFrame &message);
    void DoClusterSaveEnergyQuery(const MessageFrame &message);
    void DoClusterSaveEnergyConfig(const MessageFrame &message);
    void SetSchPolicy(const MessageFrame &message);
    void QuerySchPolicy(const MessageFrame &message);
    void SetVMHAPolicy(const MessageFrame &message);
    void QueryVMHAPolicy(const MessageFrame &message);


    // 禁用拷贝构造
    DISALLOW_COPY_AND_ASSIGN(ClusterAgent);   
};

} // namespace zte_tecs
#endif // #ifndef HM_HOST_MANAGER_H

