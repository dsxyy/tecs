/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：tc.cpp
* 文件标识：
* 内容摘要：tc进程的main函数实现文件
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
#include "alarm_agent.h"
#include "log_agent.h"
#include "storage_cli.h"
#include "mid.h"

using namespace zte_tecs;

namespace zte_tecs
{
int sa_main()
{
    STATUS ret = ERROR;
    AddKeyMu(LOG_AGENT);


    //启动系统日志
    ret = LogInit();
    if (ret != SUCCESS)
    {
        SkyAssert(0);
        SkyExit(-1, "LogInit failed!");
    }

    StorageAdaptorInitiator *pcli = StorageAdaptorInitiator::GetInstance();
    if (!pcli)
    {
        SkyAssert(0);
        SkyExit(-1, "sa_main: call StorageAdaptorInitiator::GetInstance failed.");
    }
    ret = pcli->Init();
    if (SUCCESS != ret)
    {
        while(1)
        {            
            OutPut(SYS_NOTICE,"StorageAdaptorInitiator Init failed, pls check sa config file and restart sa\n");
            sleep(5);
        }
        
    }
    return 0;
}
}

int main(int argc, char** argv)
{
    STATUS ret;
    TecsConfig *config = TecsConfig::Get();
    config->EnableSky();

    // config->EnableMs(MS_SERVER_TC);
    // 使能日志打印级别配置参数
    EnableLogOptions();
    EnableSaStoreOptions();

    ret = config->Parse(argc, argv);
    //解析命令行参数以及配置文件
    if (ERROR == ret)
    {
        if (!config->IsHelp())
        {
            SkyAssert(0);
            exit(-1);
        }
    }
    else if (ERROR_OPTION_NOT_EXIST == ret)
    {
        exit(-1);
    }

    //设置进程名称
    config->SetProcess(PROC_SA);

    //启动tecs进程
    ret = tecs_init();
    if (ret != SUCCESS)
    {
        printf("tecs init failed! ret = %d\n",ret);
        SkyExit(-1, "main: call tecs_init(tc_main) failed.");
    }


    // 启动调试shell
    Shell("sa_shell-> ");

    // 开始本进程的主体流程
    if (0 != sa_main())
    {
        printf("sa_main failed!\n");
        SkyExit(-1, "main: call sa_main failed.");
    }

    //等待SIGINT、SIGTERM信号到来后退出
    wait_signal_exit();
    return 0;
}

