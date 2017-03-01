/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�hc.cpp
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
******************************************************************************/
#include "tecs_config.h"
#include "host_agent.h"
#include "host_handler.h"
#include "storage.h"
#include "registered_system.h"
#include "log_agent.h"
#include "exc_log_agent.h"
#include "proc_admin.h"
#include "host_heartbeat.h"
#include "volume.h"
#include "host_storage.h"
#include "vm_handler.h"
#include "instance_manager.h"
#include "vnet_libnet.h"
#include "volume.h"


using namespace zte_tecs;
namespace zte_tecs
{
    extern bool enable_evb; 

int hc_main()
{
    STATUS ret;

    AddKeyMu(LOG_AGENT);
    AddKeyMu(MU_PROC_ADMIN);
    AddKeyMu(MU_HOST_AGENT);
    AddKeyMu(MU_HOST_HANDLER);
    AddKeyMu(MU_HOST_HEARTBEAT);
    AddKeyMu(MU_VM_HANDLER);
    AddKeyMu(INSTANCE_MANAGER);
    
    //ִ��Log ��־��¼��ʼ��,1��ʾ����ΪHC,��������HC�Ľ��̶������ṩ�������
    ret = LogInit(1);
    if(ret != SUCCESS)
    {
        SkyAssert(0);
        SkyExit(-1, "hc_main: call LogInit failed.");
    }

    // -------------------------------------------------------------------------
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

    StorageOperation *pInst=StorageOperation::GetInstance();
    if (!pInst)
    {
        SkyAssert(0);
        SkyExit(-1,"StorageOperation::GetInstance failed!");
    }
    if (SUCCESS != pInst->init())
    {
        SkyAssert(0);
        SkyExit(-1,"StorageOperation::init failed!");
    }
    
    HostAgent *pha = HostAgent::GetInstance();
    if(!pha)
    {
        SkyAssert(0);
        SkyExit(-1,"HostAgent::GetInstance failed!\n");
    }
    ret = pha->Init();
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"HostAgent Init failed!");
    }
        
    // HC�����н��̵�main�����о���Ҫ������һ�еȴ�������Ⱥ�Ĵ���
    WaitRegisteredSystem();
    
    //��ȡ������CC֮������Ҫ���Լ��ϴ�����ʱ�����Է��ͳ�ȥ
	LogLastwords();
    
    HostHandler *phh = HostHandler::GetInstance();
    if(!phh)
    {
        SkyAssert(0);
        SkyExit(-1,"HostHandler::GetInstance failed!\n");
    }
    ret = phh->Init();
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"HostHandler Init failed!\n");
    }

    HostHeartbeat *phhb = HostHeartbeat::GetInstance();
    if(!phhb)
    {
        SkyAssert(0);
        SkyExit(-1,"HostHeartbeat::GetInstance failed!\n");
    }
    ret = phhb->Init();
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"HostHeartbeat Init failed!\n");
    }
    BlockAdaptorDataPlane *pvmblockadaptor = BlockAdaptorDataPlane::GetInstance();
    if(!pvmblockadaptor)
    {
        SkyAssert(0);
        SkyExit(-1,"BlockAdaptorDataPlane::GetInstance failed!\n");
    }
    pvmblockadaptor->Init();
    VmHandler *pvmhandler = VmHandler::GetInstance();
    if(!pvmhandler)
    {
        SkyAssert(0);
        SkyExit(-1,"VmHandler::GetInstance failed!\n");
    }
    ret = pvmhandler->Init();
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"VmHandler Init failed!\n");
    } 

    InstanceManager *pins_manager = InstanceManager::GetInstance();
    if(!pins_manager)
    {
        SkyAssert(0);
        SkyExit(-1,"InstanceManager::GetInstance failed!\n");
    }
    ret = pins_manager->Init();
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"InstanceManager Init failed!\n");
    } 
    
    CVNetLibNet *pnet =CVNetLibNet::CreateInstance();
    if (!pnet)
    {
        SkyAssert(0);
        SkyExit(-1,"CVNetLibNet GetInstance failed!");
    }

    string app=ApplicationName();
  
    ret = pnet->Init(app,VNA_MODULE_ROLE_HC);
    if(ret != SUCCESS)
    {
        SkyAssert(0);
        SkyExit(-1,"CVNetLibNet Init failed!");
    }
    
     //ע�ῴ�Ź���Ϣ�Ľ�����
     MID mid(app,PROC_HC, MU_VM_HANDLER);
     pnet->RegisterEvent(EV_VM_OP ,  mid);
     pnet->RegisterEvent(EV_VNETLIB_WATCHDOG_OPER_ACK, mid);
    
    return 0;
}
} //end namespace

extern int32 statistics_interval;
extern string location;
extern string shelf_type;
extern string media_address;
extern STATUS EnableHostAgentOptions();

int main(int argc, char** argv)
{
    STATUS ret;
    TecsConfig *config = TecsConfig::Get();
    config->EnableSky();
    //HC��sky��ʱ������Ҫ��ϸߣ���Ҫ�ﵽ20����
    config->SetTimerScanValue(20);

    EnableLogOptions();
    EnableHostAgentOptions();

    ret = config->Parse(argc, argv);
    //���������в����Լ������ļ�
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
    config->SetProcess(PROC_HC);
    //��������
    //ret = config->Start();
    ret = tecs_init();
    if(ret != SUCCESS)
    {
        printf("tecs init failed! ret = %d\n",ret);
        SkyExit(-1,"main(hc.cpp): call tecs_init failed.");
    }

    // ��������shell
    Shell("hc_shell-> ");

    // ��ʼ�����̵���������
    if(0 != hc_main())
    {
        printf("hc_main failed!\n");
        SkyExit(-1, "main: call hc_main failed.");
    }

    //�ȴ�SIGINT��SIGTERM�źŵ������˳�
    wait_signal_exit();
    return 0;
}

