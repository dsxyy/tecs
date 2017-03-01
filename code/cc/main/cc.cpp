/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：cc.cpp
* 文件标识：
* 内容摘要：cc进程的main函数实现文件
* 当前版本：1.0
* 作    者：袁进坤
* 完成日期：2011年8月19日
*
* 修改记录1：
*     修改日期：2011/8/19
*     版 本 号：V1.0
*     修 改 人：袁进坤
*     修改内容：创建
* * 修改记录2：
*     修改日期：2013/8/12
*     版 本 号：V1.1
*     修 改 人：xiett
*     修改内容：增加 HC/VM 资源使用统计相关表创建和初始化
******************************************************************************/
#include "tecs_config.h"
#include "workflow.h"
#include "host_manager.h"
#include "host_ha.h"
#include "vstorage_manager.h"
#include "scheduler.h"
#include "vm_manager.h"
#include "lifecycle_manager.h"
#include "proc_admin.h"
#include "cluster_agent.h"
#include "exc_log_agent.h"
#include "alarm_agent.h"
#include "volume.h"
#include "vm_image.h"
#include "dbschema_ver.h"
#include "vm_disk_share.h"
#include "vnet_libnet.h"
#include "power_manager.h"
#include "power_agent.h"
#include "vm_ha.h"
#include "config_vm_statistics.h"
#include "volume.h"

using namespace zte_tecs;
namespace zte_tecs
{

static void SetClusterCfg2Table(void)
{   
    ConfigCluster *cluster_config = ConfigCluster::GetInstance();
    if (NULL != cluster_config)
    {
        ImageStoreOption option;
        GetVstorageOption(option);
        cluster_config->Set(CFG_IMG_USAGE, option._img_usage);
        cluster_config->Set(CFG_IMG_SRC_TYPE, option._img_srctype);
        cluster_config->Set(CFG_IMG_SPC_EXP, to_string<int>(option._img_spcexp,dec));
        cluster_config->Set(CFG_SHARE_IMG_USAGE, option._share_img_usage);
    }
}

/* 资源统计历史记录预留时间配置
 * 单位: hour
 */
int32 statistics_reserve = 0;
STATUS EnableStatisticsOptions()
{
    TecsConfig *config = TecsConfig::Get();
    config->EnableCustom("statistics_reserve", statistics_reserve,
                         "reserve time of statistics history records, default = 0 hours.");
    return SUCCESS;
}
DEFINE_DEBUG_VAR(statistics_reserve);

#define InitMu(mu) \
    do \
    { \
        mu *p = mu::GetInstance(); \
        if(!p) \
        { \
            SkyAssert(0); \
            SkyExit(-1,#mu"::GetInstance failed!"); \
        } \
        STATUS ret = p->Init(); \
        if(SUCCESS != ret) \
        { \
            printf(#mu" Init failed! ret=%d\n",ret); \
            SkyAssert(0); \
            SkyExit(-1,#mu" Init failed!"); \
        } \
    } while (0)

#define ConfigInit(config) \
    do \
    { \
        config *p = config::CreateInstance(GetDB()); \
        if(!p) \
        { \
            SkyAssert(0); \
            SkyExit(-1,#config"::GetInstance failed!"); \
        } \
    } while (0)


int cc_main()
{
    STATUS ret;

    AddKeyMu(LOG_AGENT);
    AddKeyMu(EXC_LOG_AGENT);
    AddKeyMu(ALARM_AGENT);
    AddKeyMu(MU_PROC_ADMIN);
    AddKeyMu(MU_VM_SCHEDULER);
    AddKeyMu(MU_VM_LIFECYCLE_MANAGER);
    AddKeyMu(MU_VM_MANAGER);
    AddKeyMu(MU_HOST_MANAGER);
    AddKeyMu(MU_CLUSTER_AGENT);
    AddKeyMu(MU_HOST_HIGHAVAILABLE);
  //  AddKeyMu(MU_MACIP_SYN_MGR);
    AddKeyMu(MU_DISK_AGENT);
    AddKeyMu(MU_POWER_AGENT);
    AddKeyMu(MU_POWER_MANAGER);
    AddKeyMu(MU_VM_HA);
    //AddKeyMu(VNA_MODULE_ROLE_CC);
    
    //日志模块初始化
    ret = LogInit();    
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"LogInit Init failed!");
    }
    
    //异常日志代理初始化
    ExcLogAgent *pea = ExcLogAgent::GetInstance();
    if(!pea)
    {
        SkyAssert(0);
        SkyExit(-1,"ExcLogAgent::GetInstance failed!");
    }
    
    ret = pea->Init();
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"ExcLogAgent Init failed!");
    }
    
    //工作流引擎启动
    ret = StartWorkflowEngine(GetPrivateDB(),3);
    if(ret != SUCCESS)
    {
        SkyAssert(0);
        SkyExit(-1, "StartWorkflowEngine failed!");
    }
    
    // -------------------------------------------------------------------------
    // AlarmAgent启动
    AlarmAgent *paa = AlarmAgent::GetInstance();
    if (!paa)
    {
        SkyAssert(0);
        SkyExit(-1,"Create AlarmAgent failed!");
    }
    ret = paa->Init();
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"AlarmAgent Init failed!");
    }
    
    // 数据表的初始化
    config_coefficient *pccp = config_coefficient::CreateInstance(GetDB());
    if(!pccp)
    {
        SkyAssert(0);
        SkyExit(-1,"config_coefficient::CreateInstance failed!");
    }
    
    ConfigCluster *ccp = ConfigCluster::CreateInstance(GetDB());
    if(!ccp)
    {
        SkyAssert(0);
        SkyExit(-1,"ConfigCluster::CreateInstance failed!");
    }
    
    SetClusterCfg2Table();
    
    HostCpuInfo *hcp = HostCpuInfo::CreateInstance(GetDB());
    if(!hcp)
    {
        SkyAssert(0);
        SkyExit(-1,"HostCpuInfo::CreateInstance failed!");
    }

    
    ConfigTcu *ctp = ConfigTcu::CreateInstance(GetDB());
    if(!ctp)
    {
        SkyAssert(0);
        SkyExit(-1,"ConfigTcu::CreateInstance failed!");
    }
    BlockAdaptorDataPlane *pvmblockadaptor = BlockAdaptorDataPlane::GetInstance();
    if(!pvmblockadaptor)
    {
        SkyAssert(0);
        SkyExit(-1,"BlockAdaptorDataPlane::GetInstance failed!\n");
    }
    pvmblockadaptor->Init();
    VmDiskSharePool *vdsp = VmDiskSharePool::GetInstance(GetDB());
    if(!vdsp)
    {
        SkyAssert(0);
        SkyExit(-1,"VmDiskSharePool::GetInstance failed!");
    }

    ConfigCmmPool *cmm_pool = ConfigCmmPool::CreateInstance(GetDB());
    if(!cmm_pool)
    {
        SkyAssert(0);
        SkyExit(-1,"ConfigCmmPool::CreateInstance failed!");
    }
    
    // 其中用到了VirtualMachinePool中的view，需要放在VirtualMachinePool后面    
    HostPool::BootStrap(GetDB());
    HostPool *hpp = HostPool::CreateInstance(GetDB());
    if(!hpp)
    {
        SkyAssert(0);
        SkyExit(-1,"HostPool::CreateInstance failed!");
    }

    HostPool *php = HostPool::CreateInstance(GetDB());
    if(!php)
    {
        SkyAssert(0);
        SkyExit(-1,"HostPool::CreateInstance failed!");
    }

    //创建资源池对象
    VirtualMachinePool *pvmp = VirtualMachinePool::CreateInstance(GetDB());
    if (!pvmp)
    {
        SkyAssert(0);
        SkyExit(-1,"VirtualMachinePool::CreateInstance failed!");
    }
    ret = pvmp->Init();
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"VirtualMachinePool Init failed!");
    }


    if (NULL == ConfigVmStatisticsNics::CreateInstance(GetDB()))
    {
        SkyAssert(0);
        SkyExit(-1, "cc_main: ConfigVmStatisticsNics::CreateInstance(GetDB()) failed.");
    }

    if (NULL == VMImagePool::CreateInstance(GetDB()))
    {
        SkyAssert(0);
        SkyExit(-1, "cc_main: VMImagePool::CreateInstance(GetDB()) failed.");
    }

    if (NULL == VmDevicePool::GetInstance(GetDB()))
    {
        SkyAssert(0);
        SkyExit(-1, "cc_main: VmDevicePool::GetInstance(GetDB()) failed.");
    }

    if (NULL == VmUsbPool::GetInstance(GetDB()))
    {
        SkyAssert(0);
        SkyExit(-1, "cc_main: VmUsbPool::GetInstance(GetDB()) failed.");
    }

    //节能降耗功能新增host_atca表初始化
    if (NULL == HostAtca::CreateInstance(GetDB()))
    {
        SkyAssert(0);
        SkyExit(-1, "cc_main: ConfigHostAtca::GetInstance(GetDB()) failed.");
    }

    // 创建消息单元
    ProcAdmin *ppa = ProcAdmin::GetInstance();
    if(!ppa)
    {
        SkyAssert(0);
        SkyExit(-1,"ProcAdmin::GetInstance failed!");
    }
    ret = ppa->Init();
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"ProcAdmin Init failed!");
    }

    HostManager *phm = HostManager::GetInstance();
    if (!phm)
    {
        SkyAssert(0);
        SkyExit(-1, "cc_main:HostManager::GetInstance() failed.");
    }
    ret = phm->Init();
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"HostManager Init failed!");
    }  

    ClusterAgent *pca = ClusterAgent::GetInstance();
    if (!pca)
    {
        SkyAssert(0);
        SkyExit(-1, "cc_main:ClusterAgent::GetInstance() failed.");
    }
    ret = pca->Init();
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"ClusterAgent Init failed!");
    }  

    CVNetLibNet *pnet =CVNetLibNet::CreateInstance();
    if (!pnet)
    {
        SkyAssert(0);
        SkyExit(-1,"CVNetLibNet CreateInstance failed!");
    }

    string app=ApplicationName();
  
    ret = pnet->Init(app, VNA_MODULE_ROLE_CC);
    if(ret != SUCCESS)
    {
        SkyAssert(0);
        SkyExit(-1,"CVNetLibNet Init failed!");
    }
    
    HostHA *phh = HostHA::GetInstance();
    if (!phh)
    {
        SkyAssert(0);
        SkyExit(-1, "cc_main: HostHA::GetInstance() failed.");
    }
    ret = phh->Init();
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"HostHA Init failed!");
    }  
    
    ret = VStorageInit();
    if(SUCCESS != ret)
    {
        printf("VStorageInit start failed! ret=%d\n",ret);
        SkyAssert(0);
        SkyExit(-1, "cc_main: call VStorageInit() failed.");
    }  

   
    //虚拟机调度器Scheduler模块初始化
    InitMu(Scheduler);
    
    //虚拟机生命周期管理器LifecycleManager初始化
    InitMu(LifecycleManager);

    //虚拟机管理器VMManager模块初始化
    InitMu(VMManager);

    //节能降耗功能新增PowerAgent和PowerManager模块初始化    
    PowerAgent *power_agent = PowerAgent::GetInstance();
    if (NULL == power_agent)
    {
        SkyAssert(0);
        SkyExit(-1, "cc_main: PowerAgentr::GetInstance failed!");
    }
    ret = power_agent->Init();

    PowerManager *power_manager = PowerManager::GetInstance();
    if (NULL == power_manager)
    {
        SkyAssert(0);
        SkyExit(-1, "cc_main: PowerManager::GetInstance failed!");
    }
    ret = power_manager->Init();

    InitMu(VMHA);
    ConfigInit(VmDynamicData);	
    return 0;
}
}

int main(int argc, char** argv)
{
    STATUS ret;
    TecsConfig *config = TecsConfig::Get();
    config->EnableSky();
    config->EnableDB(DBSCHEMA_UPDATE_VERSION,DBSCHEMA_BUGFIX_VERSION);
    config->EnableMs(MS_SERVER_CC);    

    // 使能日志打印级别配置参数
    EnableLogOptions();
    EnableSchedulerOptions();
    //EnableDhcpOptions();
    //EnableNatOptions();
    EnableVstorageOptions();
    EnableLifeCycleOptions();
    EnableStatisticsOptions();

    ret = config->Parse(argc, argv);
	
    if(SUCCESS == ret)
    {
        if (config->IsHelp())
        {
            return 0;
        }        
    }

    //初始化配置
    if(ERROR == ret)
    {
        if (!config->IsHelp())
        {
            SkyAssert(0);		  		
            exit(-1);
        }
    }
    else if(ERROR_OPTION_NOT_EXIST == ret)
    {		
        exit(-1);
    }
    
    //设置进程名称
    config->SetProcess(PROC_CC);
    //启动tecs进程
    ret = tecs_init(cc_main);
    if(ret != SUCCESS)
    {
        printf("tecs init failed! ret = %d\n",ret);
        SkyExit(-1, "main: call tecs_init(cc_main) failed!");
    }
    
    // 启动调试shell
    Shell("cc_shell-> ");
    //等待SIGINT、SIGTERM信号到来后退出
    wait_signal_exit(); 
    return 0;
}

