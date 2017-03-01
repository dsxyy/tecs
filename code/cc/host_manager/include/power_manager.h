/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�power_manager.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��power_manager�ඨ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�liuyi
* ������ڣ�2013��4��19��
*
* �޸ļ�¼1��
*     �޸����ڣ�2013/4/19
*     �� �� �ţ�V1.0
*     �� �� �ˣ�liuyi
*     �޸����ݣ�����
*******************************************************************************/
#ifndef HM_POWER_MANAGER_H
#define HM_POWER_MANAGER_H
#include "sky.h"
#include "mid.h"
#include "host_pool.h" 
#include "config_cluster.h"
#include "config_host_atca.h"
#include "config_cmm_pool.h"
#include "event.h"
#include "msg_power_manager_with_api_method.h"

namespace zte_tecs 
{

/**
@brief ��������: ����������ʵ����\n
@li @b ����˵��: ��
*/
class PowerManager : public MessageHandler
{
public:


    static PowerManager *GetInstance()
    {
        if (NULL == _instance)
        {
            _instance = new PowerManager();
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


        if (NULL == (_cluster_config = ConfigCluster::GetInstance()))
        {
            SkyAssert(0);
            OutPut(SYS_EMERGENCY, "Get ConfigCluster failed!\n");
            return ERROR_NOT_READY;
        }

        if (NULL == (_host_atca = HostAtca::GetInstance()))
        {
            SkyAssert(0);
            OutPut(SYS_EMERGENCY, "Get HostAtca failed!\n");
            return ERROR_NOT_READY;
        }    

        if (NULL == (_cmm_config = ConfigCmmPool::GetInstance()))
        {
            SkyAssert(0);
            OutPut(SYS_EMERGENCY, "Get ConfigCmmPool failed!\n");
            return ERROR_NOT_READY;
        }

        return StartMu(MU_POWER_MANAGER);
    }
    
    virtual ~PowerManager() 
    { 
        _instance = NULL; 
    }

    // GET interfaces
    bool GetFlagOfSaveEnergy()
    {
        return _save_energy_enable;
    };

    uint32 GetSaveTimerDuration()
    {
        return _timer_duration;
    };
    
private:
    MessageUnit         *_mu;               // ��Ϣ��Ԫ
    bool                _have_startup;      // �Ƿ��������ı�־���������³�ʼ��
    ConfigCluster       *_cluster_config;   // ����Ⱥ��������
    HostPool            *_host_pool;        // ����Ⱥ�������������Դ��
    HostAtca            *_host_atca;
    ConfigCmmPool       *_cmm_config;       // ����Ⱥ��cmm������    
    bool                _save_energy_enable;
    TIMER_ID            _timer_id;          // ���ܼ�鶨ʱ��
    uint32              _timer_duration;    // ���ܼ�鶨ʱ��ʱ��
    static PowerManager *_instance;      

    PowerManager();
    virtual void MessageEntry(const MessageFrame &message); //��Ϣ������ں���
    STATUS StartMu(const string& name);
    int32 DoGetHostPowerOffNum();
    void  DoSaveEnergyProc();
    bool  DoGetSaveEnergyFlg();
    void  DoSaveEnergyTimer();
    void  DoAllHostPowerOn();
    void  DoHostExternCtrlReq(const MessageFrame & message);
    void  DoHostExternQueryReq(const MessageFrame &message);

    // ��Ϣ������
    void DoStartUp();
    void DoSetEnergyTimer();

    // ��������
    uint32 GetCheckDuration();

    // ���ÿ�������
    DISALLOW_COPY_AND_ASSIGN(PowerManager);   
};

} // namespace zte_tecs
#endif // #ifndef HM_HOST_MANAGER_H

