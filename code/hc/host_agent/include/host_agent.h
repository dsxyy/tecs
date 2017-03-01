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
#ifndef HA_HOST_AGENT_H
#define HA_HOST_AGENT_H
#include "sky.h"
#include "mid.h"
#include "light_agent.h"
#include "msg_host_manager_with_host_agent.h"
#include "smart.h"
#include "smart_api.h"
#include <boost/algorithm/string.hpp>

namespace zte_tecs 
{

//刀片后插卡信息
//只在创建host.xml时用到，所以放在这里了
class BoardKCSInfo {
public:
    string      _slot;
    string      _id;
    string      _pcb;
    string      _bom;
};

class CommonCpuInfo
{
public:
    string _physical_id_num;
    string _cpu_cores; // 每个物理 CPU 有多少 cores
    string _vendor_id;
    string _cpu_family;
    string _model;
    string _model_name;
    string _stepping;
    string _cache_size;
    string _fpu;
    string _fpu_exception;
    string _cpuid_level;
    string _wp;
    string _flags;
    string _cpu_bench;
};

/**
@brief 功能描述: 物理机代理的实体类，实现状态监控模块的功能\n
@li @b 其它说明: 无
*/
class HostAgent : public MessageHandler
{
public:
    static HostAgent *GetInstance()
    {
        if (NULL == _instance)
        {
            _instance = new HostAgent();
        }
        return _instance;
    }

    STATUS Init()
    {
        string my_key;
        SshGetPubkey(my_key);
        SetHypervisorType();
        CreateHostResourceXml();
        GetBoardTypeFromKcs(_board_type);

        return StartMu(MU_HOST_AGENT);
    };

    virtual ~HostAgent() { _instance = NULL; }
    virtual void MessageEntry(const MessageFrame &message); //消息处理入口函数

    const int32 &GetPcpuNum() const
    {
        return _cpu_cores;
    };
    
    const int32 & GetCpusForVm() const
    {
        return _max_cpus;
    };

    int32 GetDom0Cpus() 
    {
        return ((_cpu_cores == _max_cpus) ? _cpu_cores : (_cpu_cores-_max_cpus));
    };

    int GetAllDiskInfo(vector<DiskInfo>& vec_disk_info)
    {
        vec_disk_info = _vec_disk_info;
        return SUCCESS; 
    };
    int SetDiskInfo(string disk_name, int status);

    int32 GetHypervisorType()
    {
        return _hypervisor_type;
    };

    bool GetCrystalStatus();
    void GetBoardKCSInfo(vector<BoardKCSInfo> &infos);
    int32 GetBoardTypeFromKcs(int32  &borad_type);
    int GetCpuCoreNumForXen(void);
    int GetCpuCoreNumForKvm(void);
    int GetDom0MaxVcpus(void);
    int GetMaxCpusForXenVm(void);
    int GetMaxCpusForKvmVm(void);
    string GetVmmInfoForXen(void);
    string GetVmmInfoForKvm(void);
    int64 GetTotalMemoryForXen(void);
    int64 GetTotalMemoryForKvm(void);
    int64 GetDom0Memory(void);
    int64 GetFreeMemoryForXen(void);
    int64 GetFreeMemoryForKvm(void);
    bool GetIsSupportHvmForXen(void);
    bool GetIsSupportHvmForKvm(void);
    string GetOsInfo(void);
    string GetSshKey(void);
    void GetCommonCpuInfo(CommonCpuInfo& common_cpuinfo);
    void GetHostHwMonInfo(vector<HostHwmon>& vec_host_hwmon);
    void GetHostStatistics(ResourceStatistics& host_statistics);
    void GetNicUsedRate(vector<NicInfo>& vec_nic_info);

private:
    MessageUnit         *_mu;               // 消息单元
    string              _cluster_name;      // 本主机归属的集群名称
    TIMER_ID            _timer_id;          // 主机发现和信息上报公用的定时器
    vector<string>      _ssh_keys;
    int32               _cpu_cores;         //主机所有cpu物理核数
    int32               _max_cpus;          //最大可用于部署虚拟机的cpu数目
    int32               _hypervisor_type;
    int32               _report_count;
    ResourceStatistics  _pre_host_statistics;
    vector<DiskInfo>    _vec_disk_info; // 间隔时间内的缓存
    int32               _board_type;
    time_t              _pre_statistics_time;
    string              _hc_ip_address;
    string              _cc_ip_address;

    static HostAgent    *_instance;

    HostAgent();
    STATUS StartMu(const string& name);
    
    // 消息处理函数
    void DoStartUp();
    STATUS DoJoinFriendKey(const string &key);  
    void DoDiscover();
    void DoReport();
    void DoRegister(const MessageFrame &message);
    void DoUnregister(const MessageFrame &message);
    void DoRegisteredSystem();
    void DoRegisteredSystem(const MessageFrame &message);

    // 辅助函数
    void SetHypervisorType();
    void RunCmdAndTrim(string& cmd, string& result);
    void GoToDiscover();
    void GoToReport();
    void SetCluster(const string &cluster_name);
    void ExitClusterGroup();
    void JoinClusterGroup();
    void LightHost(LightState light_state, bool is_err);
    void GetExtendLocationInfo(BoardPosition &phy_position);
    void GetBaseLocationInfo(map<string,string> &location);
    void GetDiskInfoForReport(vector<DiskInfo>& vec_disk_info);
    void GetDiskInfoForDiscover(int64 &disk_total,int64 &disk_max);
    void GetBaseInfoFromScript(MessageHostBaseInfo &message);
    void CreateHostResourceXml();
    void ConstSdeviceInfo(vector<PciPthDevInfo> &data);
    int32 XenHostStatistics(ResourceStatistics& host_statistics);
    int32 KvmHostStatistics(ResourceStatistics& host_statistics);
    int32 GetHostRunningTime(int64 &running_time);
    // 禁用拷贝构造
    DISALLOW_COPY_AND_ASSIGN(HostAgent);   
};

} // namespace zte_tecs
#endif // #ifndef HA_HOST_AGENT_H

