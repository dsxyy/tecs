/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：hc.cpp
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
    
    //执行Log 日志记录初始化,1表示本机为HC,凡是属于HC的进程都必须提供这个参数
    ret = LogInit(1);
    if(ret != SUCCESS)
    {
        SkyAssert(0);
        SkyExit(-1, "hc_main: call LogInit failed.");
    }

    // -------------------------------------------------------------------------
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
        
    // HC上所有进程的main函数中均需要增加这一行等待归属集群的代码
    WaitRegisteredSystem();
    
    //获取到所属CC之后，首先要把自己上次运行时的遗言发送出去
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
    
     //注册看门狗消息的接收者
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
    //HC对sky定时器精度要求较高，需要达到20毫秒
    config->SetTimerScanValue(20);

    EnableLogOptions();
    EnableHostAgentOptions();

    ret = config->Parse(argc, argv);
    //解析命令行参数以及配置文件
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
    config->SetProcess(PROC_HC);
    //启动配置
    //ret = config->Start();
    ret = tecs_init();
    if(ret != SUCCESS)
    {
        printf("tecs init failed! ret = %d\n",ret);
        SkyExit(-1,"main(hc.cpp): call tecs_init failed.");
    }

    // 启动调试shell
    Shell("hc_shell-> ");

    // 开始本进程的主体流程
    if(0 != hc_main())
    {
        printf("hc_main failed!\n");
        SkyExit(-1, "main: call hc_main failed.");
    }

    //等待SIGINT、SIGTERM信号到来后退出
    wait_signal_exit();
    return 0;
}

