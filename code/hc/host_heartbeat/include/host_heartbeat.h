/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：host_agent.h
* 文件标识：见配置管理计划书
* 内容摘要：HostAgent类定义文件
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
#ifndef HOST_HEARTBEAT_H
#define HOST_HEARTBEAT_H
#include "sky.h"
#include "mid.h"

namespace zte_tecs 
{

/**
@brief 功能描述: 物理机代理的实体类，实现状态监控模块的功能\n
@li @b 其它说明: 无
*/
class HostHeartbeat : public MessageHandler
{
public:
    static HostHeartbeat *GetInstance()
    {
        if (NULL == _instance)
        {
            _instance = new HostHeartbeat();
        }
        return _instance;
    };

    virtual ~HostHeartbeat() 
    {

    };
    
    STATUS Init()
    {
        return StartMu(MU_HOST_HEARTBEAT);
    }
    
    virtual void MessageEntry(const MessageFrame &message); //消息处理入口函数
private:
    MessageUnit *_mu;               // 消息单元
    string      _cluster_name;      // 本主机归属的集群名称
    TIMER_ID    _timer_id;          // 本模块使用的定时器
    int32       _timer_duration;    // 本模块使用的定时器时长

    static HostHeartbeat *_instance;  
    HostHeartbeat();
    STATUS StartMu(const string& name);
    void DoStartUp();
    void DoHeartbeatReport();

    // 禁用拷贝构造
    DISALLOW_COPY_AND_ASSIGN(HostHeartbeat);   
};

} // namespace zte_tecs
#endif // #ifndef HA_HOST_AGENT_H

