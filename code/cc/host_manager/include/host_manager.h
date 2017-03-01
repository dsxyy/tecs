/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�host_manager.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��HostManager�ඨ���ļ�
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
@brief ��������: ����������ʵ����\n
@li @b ����˵��: ��
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
        // ��ȡ�����ض���ָ��
        if (NULL == (_host_pool = HostPool::GetInstance()))
        {
            SkyAssert(0);
            OutPut(SYS_EMERGENCY, "Get host pool failed!\n");
            return ERROR_NOT_READY;
        }    

        // ��ȡCPU����ϵ��Ȩ������ָ��
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

        //ÿ5���Ӷ�host_pool����һ�����ݿ�������
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
    MessageUnit              *_mu;               // ��Ϣ��Ԫ
    bool                     _have_startup;      // �Ƿ��������ı�־���������³�ʼ��
    string                   _tecs_name;         // ����Ⱥ������tecsϵͳ����
    HostPool                 *_host_pool;        // ����Ⱥ�������������Դ��
    config_coefficient       *_coefficient_config;  // ����Ⱥ��cpu����ϵ��Ȩ������
    HostAtca                 *_host_atca;        // ����Ⱥ��֧��atca��ر�����
    ConfigHostStatistics     *_host_statistics;
    ConfigHostStatisticsNics *_host_statistics_nics;
    ConfigHostDisks          *_host_disks;
    ConfigHostHwmon          *_host_hwmon;
    HostCpuInfo              *_host_cpuinfo;
    string                   _ip_connect_lowstream;
    map<string, string>      _ifs_info;

    static HostManager *_instance;      

    HostManager();
    virtual void MessageEntry(const MessageFrame &message); //��Ϣ������ں���
    STATUS StartMu(const string& name);

    // ��Ϣ������
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

    // ����������
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

    // ���ÿ�������
    DISALLOW_COPY_AND_ASSIGN(HostManager);   
};

} // namespace zte_tecs
#endif // #ifndef HM_HOST_MANAGER_H

