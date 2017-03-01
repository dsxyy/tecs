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
@brief ��������: ����������ʵ����\n
@li @b ����˵��: ��
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
        // ��ȡ�����ض���ָ��
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
    MessageUnit         *_mu;               // ��Ϣ��Ԫ
    bool                _have_startup;      // �Ƿ��������ı�־���������³�ʼ��
    string              _tecs_name;         // ����Ⱥ������tecsϵͳ����
    string              _cluster_verify_id; // ��Ⱥ����TC����֤�룬��������������Ⱥ
    ConfigCluster       *_cluster_config;   // ����Ⱥ��������
    ConfigTcu           *_tcu_config;       // ����Ⱥtcu������
    HostPool            *_host_pool;        // ����Ⱥ�������������Դ��
    ConfigCmmPool       *_cmm_config;       // ����Ⱥ��cmm������
    string              _cc_ip_address;
    string              _tc_ip_address;

    static ClusterAgent *_instance;      

    ClusterAgent();
    virtual void MessageEntry(const MessageFrame &message); //��Ϣ������ں���
    STATUS StartMu(const string& name);

    void ExitTecsGroup();
    void JoinTecsGroup();
    void SetTecsName(const string &tecs_name);
    
    void DoRegisteredSystem();
    void DoRegisteredSystem(const MessageFrame &message);

    // ��Ϣ������
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


    // ���ÿ�������
    DISALLOW_COPY_AND_ASSIGN(ClusterAgent);   
};

} // namespace zte_tecs
#endif // #ifndef HM_HOST_MANAGER_H

