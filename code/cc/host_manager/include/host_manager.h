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
#ifndef HM_HOST_MANAGER_H
#define HM_HOST_MANAGER_H
#include "sky.h"
#include "mid.h"
#include "db_config.h"
#include "log_agent.h"
#include "host_pool.h" 
#include "config_cluster.h"
#include "config_tcu.h"
#include "config_coefficient.h"
#include "host_cpuinfo.h"
#include "config_cmm_pool.h"
#include "config_host_atca.h"
#include "config_host_hwmon.h"
#include "config_host_statistics.h"
#include "config_host_disks.h"
#include "smart_api.h"

namespace zte_tecs 
{
#define InitConfig(config, table) \
    table = config::CreateInstance(GetDB()); \
    if (!table) \
    { \
        SkyAssert(0); \
        OutPut(SYS_EMERGENCY, #config"::CreateInstance failed!\n"); \
        return ERROR_NOT_READY; \
    }

/**
@brief 功能描述: 物理机管理的实体类\n
@li @b 其它说明: 无
*/
class HostManager : public MessageHandler
{
public:
    static HostManager *GetInstance()
    {
        if (NULL == _instance)
        {
            _instance = new HostManager();
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

        // 获取CPU能力系数权重配置指针
        if (NULL == (_coefficient_config = config_coefficient::GetInstance()))
        {
            SkyAssert(0);
            OutPut(SYS_EMERGENCY, "Get config_coefficient failed!\n");
            return ERROR_NOT_READY;
        }

        if (NULL == (_host_atca = HostAtca::GetInstance()))
        {
            SkyAssert(0);
            OutPut(SYS_EMERGENCY, "Get HostAtca failed!\n");
            return ERROR_NOT_READY;
        }

        InitConfig(ConfigHostStatistics, _host_statistics);
        InitConfig(ConfigHostStatisticsNics, _host_statistics_nics);
        InitConfig(ConfigHostDisks, _host_disks);
        InitConfig(ConfigHostHwmon, _host_hwmon);
        InitConfig(HostCpuInfo, _host_cpuinfo);

        //每5分钟对host_pool表做一次数据库清理动作
        STATUS ret = RegisterDbVacuum(Host::_table,300);
        if(SUCCESS != ret)
        {
            return ret;
        }

        return StartMu(MU_HOST_MANAGER);
    }
    
    virtual ~HostManager() 
    { 
        _instance = NULL; 
    }
    
private:
    MessageUnit              *_mu;               // 消息单元
    bool                     _have_startup;      // 是否已启动的标志，避免重新初始化
    string                   _tecs_name;         // 本集群归属的tecs系统名称
    HostPool                 *_host_pool;        // 本集群所管理的主机资源池
    config_coefficient       *_coefficient_config;  // 本集群中cpu能力系数权重配置
    HostAtca                 *_host_atca;        // 本集群中支持atca相关表配置
    ConfigHostStatistics     *_host_statistics;
    ConfigHostStatisticsNics *_host_statistics_nics;
    ConfigHostDisks          *_host_disks;
    ConfigHostHwmon          *_host_hwmon;
    HostCpuInfo              *_host_cpuinfo;
    string                   _ip_connect_lowstream;
    map<string, string>      _ifs_info;

    static HostManager *_instance;      

    HostManager();
    virtual void MessageEntry(const MessageFrame &message); //消息处理入口函数
    STATUS StartMu(const string& name);

    // 消息处理函数
    void DoStartUp();
    void DoHostSet(const MessageFrame &message);
    void DoHostForget(const MessageFrame &message);
    void DoHostDisable(const MessageFrame &message);
    void DoHostEnable(const MessageFrame &message);
    void DoHostRebootReq(const MessageFrame &message);
    void DoHostShutdownReq(const MessageFrame &message);
    void DoHostDefHandleAck(const MessageFrame &message);
    
    void DoHostExternCtrlReq(const MessageFrame &message);
    void DoHostExternQueryReq(const MessageFrame &message);
    void DoHostExternCfgSetReq(const MessageFrame &message);
    void DoHostExternCfgGetReq(const MessageFrame &message);

    void DoHostQueryReq(const MessageFrame &message);
    void DoHostDiscover(const MessageFrame &message);
    void DoHostInfoReport(const MessageFrame &message);
    void DoHostRunStateCheck();
    void DoHostCpuUsageCalc();
    void DoHostHaCheck();
    void DoHostAlarmCheck();
    void DoScanForDrop();

    // 辅助处理函数
    void SendHostRegisterReq(Host *host);
    void SendHighAvailable(Host *host, int32 msg_id, void *data = NULL);
    void SendAlarm(Host *host, uint32 code, byte flag, const string &info = "");
    void SendAlarm(Host *host);
    void SendAlarmResume(Host *host);
    
    void SetHostRunState(Host *host, Host::RunState state, 
                         const string &info = "");
    void SetHostIsDisabled(Host *host, bool is_disabled);    
    void SetHostMemState(Host *host, bool state);
    void SetHostOsciliatorState(Host *host, bool state);
    void SetHostDiskState(Host *host, const vector<DiskInfo>& vec_disk_info);

    void SetTecsName(const string &tecs_name);
    
    void DoRegisteredSystem();
    void DoRegisteredSystem(const MessageFrame &message);
    void DoHeartbeatReport(const MessageFrame &message);
    void SetClusterName(const MessageFrame &message);

    void SetStatisticsInfoToDB(const ResourceStatistics& host_statistics,
                               int64 hid);

    // 禁用拷贝构造
    DISALLOW_COPY_AND_ASSIGN(HostManager);   
};

} // namespace zte_tecs
#endif // #ifndef HM_HOST_MANAGER_H

