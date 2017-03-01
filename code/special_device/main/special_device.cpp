/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：special_device.cpp
* 文件标识：见配置管理计划书
* 内容摘要：特殊设备主进程
* 当前版本：1.0
* 作    者：颜伟
* 完成日期：2012年9月7日
*
* 修改记录1：
*     修改日期：2012/09/07
*     版 本 号：V1.0
*     修 改 人：颜伟
*     修改内容：创建
*******************************************************************************/
#include "tecs_config.h"
#include "mid.h"
#include "registered_system.h"
#include "light_manager.h"

using namespace zte_tecs;
namespace zte_tecs
{
int special_device_main()
{
    // -------------------------------------------------------------------------
    LightManager *plm = LightManager::GetInstance();
    if(!plm)
    {
        SkyAssert(0);
        SkyExit(-1,"LightManager::new LightManager failed!");
    }
    //创建socket文件
    string cmd="mkdir -p ";
    cmd.append(LIGHT_SOCKET_PATH);
    RunCmd(cmd);
    cmd = "touch ";
    cmd.append(LIGHT_MANAGER_SOCKET);
    RunCmd(cmd);
    if (SUCCESS != plm->Init(LIGHT_MANAGER_SOCKET))
    {
        SkyAssert(0);
        SkyExit(-1,"LightManager::Init failed!");
    }
    plm->Run();
    plm->PowerOn();

    return 0;
}
void start_without_mu(TecsConfig *config)
{
    config->ClearConnects();
}
} //end namespace

int main(int argc, char** argv)
{
    STATUS ret;
    TecsConfig *config = TecsConfig::Get();
    config->EnableSky();
    config->SetTimerScanValue(20);
    
    ret = config->Parse(argc, argv);
    //解析命令行参数以及配置文件
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
    config->SetProcess(PROC_SPECIAL_DEVICE);
    //启动配置
    start_without_mu(config);
    //ret = config->Start();
    ret = tecs_init();
    if(ret != SUCCESS)
    {
        printf("tecs init failed! ret = %d\n",ret);
        SkyExit(-1,"main(special_device.cpp): call tecs_init failed.");
    }

    // 启动调试shell
    Shell("special_device-> ");

    // 开始本进程的主体流程
    if(0 != special_device_main())
    {
        printf("special_device_main failed!\n");
        SkyExit(-1, "main: call special_device_main failed.");
    }

    //等待SIGINT、SIGTERM信号到来后退出
    wait_signal_exit();
    return 0;
}

