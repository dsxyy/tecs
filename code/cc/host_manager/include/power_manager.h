/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：power_manager.h
* 文件标识：见配置管理计划书
* 内容摘要：power_manager类定义文件
* 当前版本：1.0
* 作    者：liuyi
* 完成日期：2013年4月19日
*
* 修改记录1：
*     修改日期：2013/4/19
*     版 本 号：V1.0
*     修 改 人：liuyi
*     修改内容：创建
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
@brief 功能描述: 物理机管理的实体类\n
@li @b 其它说明: 无
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
        // 获取主机池对象指针
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
    MessageUnit         *_mu;               // 消息单元
    bool                _have_startup;      // 是否已启动的标志，避免重新初始化
    ConfigCluster       *_cluster_config;   // 本集群的配置项
    HostPool            *_host_pool;        // 本集群所管理的主机资源池
    HostAtca            *_host_atca;
    ConfigCmmPool       *_cmm_config;       // 本集群的cmm配置项    
    bool                _save_energy_enable;
    TIMER_ID            _timer_id;          // 节能检查定时器
    uint32              _timer_duration;    // 节能检查定时器时长
    static PowerManager *_instance;      

    PowerManager();
    virtual void MessageEntry(const MessageFrame &message); //消息处理入口函数
    STATUS StartMu(const string& name);
    int32 DoGetHostPowerOffNum();
    void  DoSaveEnergyProc();
    bool  DoGetSaveEnergyFlg();
    void  DoSaveEnergyTimer();
    void  DoAllHostPowerOn();
    void  DoHostExternCtrlReq(const MessageFrame & message);
    void  DoHostExternQueryReq(const MessageFrame &message);

    // 消息处理函数
    void DoStartUp();
    void DoSetEnergyTimer();

    // 辅助函数
    uint32 GetCheckDuration();

    // 禁用拷贝构造
    DISALLOW_COPY_AND_ASSIGN(PowerManager);   
};

} // namespace zte_tecs
#endif // #ifndef HM_HOST_MANAGER_H

