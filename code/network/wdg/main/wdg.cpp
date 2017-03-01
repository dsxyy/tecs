/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：wdg.cpp
* 文件标识：
* 内容摘要：wdg进程的main函数实现文件
* 当前版本：1.0
* 作    者：liuhx
* 完成日期：2012年12月14日
******************************************************************************/
#include "tecs_config.h"
#include "registered_system.h"
#include "proc_admin.h"
#include "vnet_comm.h"
#include "wdg_common.h"
#include "vnet_monitor.h"
#include "wdg_pub.h"
#include "vnet_wdg.h"
#include "vnet_libnet.h"

using namespace zte_tecs;
namespace zte_tecs
{

extern bool enable_evb; 
extern string g_strHostName;
string g_strWdgApplication = "";
#if 1
int wdg_main()
{
    STATUS ret;

    AddKeyMu(LOG_AGENT);
    AddKeyMu(MU_PROC_ADMIN);
    //AddKeyMu(MU_VNA_CONTROLLER);
    //AddKeyMu(MU_VNA_HEARTBEAT);
    AddKeyMu(MU_VNET_WDG);
        
    //执行Log 日志记录初始化,1表示本机为HC,凡是属于HC的进程都必须提供这个参数
    ret = LogInit(0);
    if(ret != SUCCESS)
    {
        SkyAssert(0);
        SkyExit(-1, "wdg_main: call LogInit failed.");
    }

    // -------------------------------------------------------------------------
    // 创建消息单元
    ProcAdmin *pProcAdmin = ProcAdmin::GetInstance();
    if(!pProcAdmin)
    {
        SkyAssert(0);
        SkyExit(-1,"ProcAdmin::GetInstance failed!");
    }
    ret = pProcAdmin->Init();
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"ProcAdmin Init failed!");
    }

    //等待VNM信息已获取
    //WaitRegisteredSystem();
    
    //获取到所属CC之后，首先要把自己上次运行时的遗言发送出去
    LogLastwords();
    
     // vnet 初始化
#if 0
    CVNAController *pCVNAController = CVNAController::GetInstance();
    if(!pCVNAController)
    {
        SkyAssert(0);
        SkyExit(-1,"CVNAController::GetInstance failed!\n");
    }
    ret = pCVNAController->Init();
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"CVNAController Init failed!\n");
    }
#endif

    CVNetLibNet *pvnetlib = CVNetLibNet::CreateInstance();
    if(pvnetlib == NULL)
    {
        SkyAssert(0);
        SkyExit(-1,"CVNetLibNet::CreateInstance!");
    }
    
    if(SUCCESS != pvnetlib->Init(g_strWdgApplication, VNA_MODULE_ROLE_WATCHDOG))
    {
        SkyAssert(0);
        SkyExit(-1,"pvnetlib Init failed!");
    }

    CVNetWatchDog *pWdgHandle = CVNetWatchDog::GetInstance();
    if(!pWdgHandle)
    {
        SkyAssert(0);
        SkyExit(-1,"CVNetWatchDog::GetInstance failed!\n");
    }

    ret = pWdgHandle->Init();
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"CVNetWatchDog Init failed!\n");
    }

    //注册event
    MID mid(g_strWdgApplication, PROC_WDG, MU_VNET_WDG);

    //pvnetlib->RegisterEvent(EV_VNETLIB_NOTIFY_VM_RR, mid);
    pvnetlib->RegisterEvent(EV_VNETLIB_NOTIFY_VM_RR_ACK, mid);
    pvnetlib->RegisterEvent(EV_VNETLIB_WDG_OPER, mid);
    //pvnetlib->RegisterEvent(EV_VNETLIB_WDG_OPER_ACK, mid);
    pvnetlib->RegisterEvent(EV_VNETLIB_WDG_TIMER_OPER, mid);
    //pvnetlib->RegisterEvent(EV_VNETLIB_WDG_TIMER_OPER_ACK, mid);

    return 0;
}
#endif
} //end namespace

int main(int argc, char** argv)
{
#if 1
    STATUS ret;
    TecsConfig *config = TecsConfig::Get();
    config->EnableSky();
    //HC对sky定时器精度要求较高，需要达到20毫秒
    config->SetTimerScanValue(20);

    // 使能日志打印级别配置参数
    EnableLogOptions();

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
    config->SetProcess(PROC_WDG);

    string  strWdgUUID;
    if (!GetWDGUUID(strWdgUUID))
    {
        string strWdgApp = "wdg_";
        strWdgApp += strWdgUUID;        
        config->SetApplication(strWdgApp);

        g_strWdgApplication = strWdgApp;
		
        printf("wdg.cpp: Set WDG Application(%s)\n", strWdgApp.c_str());
    }
    else
    {
        printf("wdg.cpp: GetWDGUUID failed\n");
        SkyExit(-1,"main(wdg.cpp): call GetWDGUUID failed.");
    }

#if 0   
    char host_name[256] = {0};
    if (0 == gethostname(host_name, 256))
    {
        g_strHostName = host_name;
    }
    else
    {

        printf("wdg.cpp: Get host name(%s)\n", g_strHostName.c_str());
    }
#endif

    //启动配置
    ret = tecs_init();
    if(ret != SUCCESS)
    {
        printf("wdg.cpp: tecs init failed! ret = %d\n",ret);
        SkyExit(-1,"main(wdg.cpp): call tecs_init failed.");
    }

    // 启动调试shell
    Shell("wdg_shell-> ");

    // 开始本进程的主体流程
    if(0 != wdg_main())
    {
        printf("wdg_main failed!\n");
        SkyExit(-1, "main: call wdg_main failed.");
    }

    //等待SIGINT、SIGTERM信号到来后退出
    wait_signal_exit();

#endif
    return 0;
}

