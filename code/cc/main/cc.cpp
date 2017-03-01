/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�cc.cpp
* �ļ���ʶ��
* ����ժҪ��cc���̵�main����ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�Ԭ����
* ������ڣ�2011��8��19��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/8/19
*     �� �� �ţ�V1.0
*     �� �� �ˣ�Ԭ����
*     �޸����ݣ�����
* * �޸ļ�¼2��
*     �޸����ڣ�2013/8/12
*     �� �� �ţ�V1.1
*     �� �� �ˣ�xiett
*     �޸����ݣ����� HC/VM ��Դʹ��ͳ����ر����ͳ�ʼ��
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

/* ��Դͳ����ʷ��¼Ԥ��ʱ������
 * ��λ: hour
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
    
    //��־ģ���ʼ��
    ret = LogInit();    
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"LogInit Init failed!");
    }
    
    //�쳣��־�����ʼ��
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
    
    //��������������
    ret = StartWorkflowEngine(GetPrivateDB(),3);
    if(ret != SUCCESS)
    {
        SkyAssert(0);
        SkyExit(-1, "StartWorkflowEngine failed!");
    }
    
    // -------------------------------------------------------------------------
    // AlarmAgent����
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
    
    // ���ݱ�ĳ�ʼ��
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
    
    // �����õ���VirtualMachinePool�е�view����Ҫ����VirtualMachinePool����    
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

    //������Դ�ض���
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

    //���ܽ��Ĺ�������host_atca���ʼ��
    if (NULL == HostAtca::CreateInstance(GetDB()))
    {
        SkyAssert(0);
        SkyExit(-1, "cc_main: ConfigHostAtca::GetInstance(GetDB()) failed.");
    }

    // ������Ϣ��Ԫ
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

   
    //�����������Schedulerģ���ʼ��
    InitMu(Scheduler);
    
    //������������ڹ�����LifecycleManager��ʼ��
    InitMu(LifecycleManager);

    //�����������VMManagerģ���ʼ��
    InitMu(VMManager);

    //���ܽ��Ĺ�������PowerAgent��PowerManagerģ���ʼ��    
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

    // ʹ����־��ӡ�������ò���
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

    //��ʼ������
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
    
    //���ý�������
    config->SetProcess(PROC_CC);
    //����tecs����
    ret = tecs_init(cc_main);
    if(ret != SUCCESS)
    {
        printf("tecs init failed! ret = %d\n",ret);
        SkyExit(-1, "main: call tecs_init(cc_main) failed!");
    }
    
    // ��������shell
    Shell("cc_shell-> ");
    //�ȴ�SIGINT��SIGTERM�źŵ������˳�
    wait_signal_exit(); 
    return 0;
}

