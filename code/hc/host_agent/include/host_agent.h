/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�host_agent.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��HostAgent�ඨ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�Ԭ����
* ������ڣ�2011��8��9��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/8/9
*     �� �� �ţ�V1.0
*     �� �� �ˣ�Ԭ����
*     �޸����ݣ�����
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

//��Ƭ��忨��Ϣ
//ֻ�ڴ���host.xmlʱ�õ������Է���������
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
    string _cpu_cores; // ÿ������ CPU �ж��� cores
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
@brief ��������: ����������ʵ���࣬ʵ��״̬���ģ��Ĺ���\n
@li @b ����˵��: ��
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
    virtual void MessageEntry(const MessageFrame &message); //��Ϣ������ں���

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
    MessageUnit         *_mu;               // ��Ϣ��Ԫ
    string              _cluster_name;      // �����������ļ�Ⱥ����
    TIMER_ID            _timer_id;          // �������ֺ���Ϣ�ϱ����õĶ�ʱ��
    vector<string>      _ssh_keys;
    int32               _cpu_cores;         //��������cpu�������
    int32               _max_cpus;          //�������ڲ����������cpu��Ŀ
    int32               _hypervisor_type;
    int32               _report_count;
    ResourceStatistics  _pre_host_statistics;
    vector<DiskInfo>    _vec_disk_info; // ���ʱ���ڵĻ���
    int32               _board_type;
    time_t              _pre_statistics_time;
    string              _hc_ip_address;
    string              _cc_ip_address;

    static HostAgent    *_instance;

    HostAgent();
    STATUS StartMu(const string& name);
    
    // ��Ϣ������
    void DoStartUp();
    STATUS DoJoinFriendKey(const string &key);  
    void DoDiscover();
    void DoReport();
    void DoRegister(const MessageFrame &message);
    void DoUnregister(const MessageFrame &message);
    void DoRegisteredSystem();
    void DoRegisteredSystem(const MessageFrame &message);

    // ��������
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
    // ���ÿ�������
    DISALLOW_COPY_AND_ASSIGN(HostAgent);   
};

} // namespace zte_tecs
#endif // #ifndef HA_HOST_AGENT_H

