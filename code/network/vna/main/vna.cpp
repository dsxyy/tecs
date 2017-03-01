/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vna.cpp
* 文件标识：
* 内容摘要：vna进程的main函数实现文件
* 当前版本：1.0
* 作    者：lvech
* 完成日期：2012年12月14日
*
* 修改记录1：
*     修改日期：2012/12/14
*     版 本 号：V1.0
*     修 改 人：lvech
*     修改内容：创建
******************************************************************************/
#include "tecs_config.h"
#include "registered_system.h"
#include "proc_admin.h"
#include "vnet_comm.h"
#include "vna_common.h"
#include "vna_pub.h"

using namespace zte_tecs;
namespace zte_tecs
{

extern bool enable_evb; 
extern string g_strHostName;
extern string g_strVNAApplication;
int32 g_nHypeVisorType = HYPE_VISOR_TYPE_XEN;

int vna_main()
{
    STATUS ret;

    AddKeyMu(LOG_AGENT);
    AddKeyMu(MU_PROC_ADMIN);
    AddKeyMu(MU_VNA_CONTROLLER);
    AddKeyMu(MU_VNA_AGENT);
    AddKeyMu(MU_VNA_HEARTBEAT);
    AddKeyMu(MU_VNET_MONITOR_MGR);
        
    //执行Log 日志记录初始化,1表示本机为HC,凡是属于HC的进程都必须提供这个参数
    ret = LogInit();
    if(ret != SUCCESS)
    {
        SkyAssert(0);
        SkyExit(-1, "vna_main: call LogInit failed.");
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

    CVNetHeartbeat *pCVNetHeart = CVNetHeartbeat::GetInstance();
    if(!pCVNetHeart)
    {
        SkyAssert(0);
        SkyExit(-1,"CVNetHeartbeat::GetInstance failed!\n");
    }
    ret = pCVNetHeart->Init();
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"CVNetHeartbeat Init failed!\n");
    }

    //初始化CVNetAgent主控类
    CVNetAgent *pVnetAgentInst =  CVNetAgent::GetInstance();
    if (NULL == pVnetAgentInst)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetAgent::GetInstance() failed.\n");
        return -1;
    }
    
    if (pVnetAgentInst->Init())
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetAgent Init()failed.\n");
        return -1;
    }

    return 0;
}
} //end namespace

int main(int argc, char** argv)
{
    STATUS ret;
    TecsConfig *config = TecsConfig::Get();
    config->EnableSky();
    //HC对sky定时器精度要求较高，需要达到20毫秒
    config->SetTimerScanValue(20);

    // 使能日志打印级别配置参数
    EnableLogOptions();
    config->EnableCustom("enable_evb", enable_evb, "The EVB(Edge Virtual Bridging) module configuration switch, default = no(bool switch). ");

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
    config->SetProcess(PROC_VNA);
    
    string  strUUID;
    if (!GetVNAUUIDByVNA(strUUID))
    {
        config->SetApplication(strUUID);
        g_strVNAApplication = strUUID;
        printf("vna.cpp: Set VNA Application(%s)\n", strUUID.c_str());
    }
    else
    {
        printf("vna.cpp: GetVNAUUIDByVNA failed\n");
        SkyExit(-1,"main(vna.cpp): call GetVNAUUIDByVNA failed.");
    }

    char host_name[256] = {0};
    if (0 == gethostname(host_name, 256))
    {
        g_strHostName = host_name;
    }
    else
    {

        printf("vna.cpp: Get host name(%s)\n", g_strHostName.c_str());
    }
        
    g_nHypeVisorType = GetHypeVisorType();
        
    //启动配置
    ret = tecs_init();
    if(ret != SUCCESS)
    {
        printf("vna.cpp: tecs init failed! ret = %d\n",ret);
        SkyExit(-1,"main(vna.cpp): call tecs_init failed.");
    }

    // 启动调试shell
    Shell("vna_shell-> ");

    // 开始本进程的主体流程
    if(0 != vna_main())
    {
        printf("vna_main failed!\n");
        SkyExit(-1, "main: call vna_main failed.");
    }

    //等待SIGINT、SIGTERM信号到来后退出
    wait_signal_exit();
    return 0;
}

