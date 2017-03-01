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
#include "workflow.h"
#include "alarm_agent.h"
#include "log_agent.h"
#include "mid.h"
#include "storage_control.h"
#include "sa_manager.h"
#include "dbschema_ver.h"

using namespace zte_tecs;
namespace zte_tecs
{
int sc_main()
{
    STATUS ret = ERROR;
    AddKeyMu(LOG_AGENT);
    AddKeyMu(ALARM_AGENT);    
    AddKeyMu(MU_SA_MANAGER);
    AddKeyMu(MU_SC_CONTROL);

    //启动系统日志
    ret = LogInit();
    if (ret != SUCCESS)
    {
        SkyAssert(0);
        SkyExit(-1, "LogInit failed!");
    }

    //工作流引擎启动
    ret = StartWorkflowEngine(GetPrivateDB(),3);
    if(ret != SUCCESS)
    {
        SkyAssert(0);
        SkyExit(-1, "StartWorkflowEngine failed!");
    }
    
    // AlarmAgent启动
    AlarmAgent *paa = AlarmAgent::GetInstance();
    if (!paa)
    {
        SkyAssert(0);
        SkyExit(-1, "tc_main: call AlarmAgent::GetInstance failed.");
    }
    ret = paa->Init();
    if (SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"AlarmAgent Init failed!");
    }
#if 1
    SaManager *pSa_manager=SaManager::GetInstance();
    if (!pSa_manager)
    {
        SkyAssert(0);
        SkyExit(-1, "tc_main: call SaManager::GetInstance failed.");
    }
    ret = pSa_manager->Init();
    if (SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"SaManager Init failed!");
    }
#endif
    StorageController *pSc=StorageController::GetInstance();
    if (!pSc)
    {
        SkyAssert(0);
        SkyExit(-1, "tc_main: call StorageController::GetInstance failed.");
    }
    ret = pSc->Init();
    if (SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"StorageController Init failed!");
    }
    // 其他消息单元在此增加

    return 0;
}
}

int main(int argc, char** argv)
{
    STATUS ret;
    TecsConfig *config = TecsConfig::Get();
    config->EnableSky();
    config->EnableDB(DBSCHEMA_UPDATE_VERSION,DBSCHEMA_BUGFIX_VERSION);
    config->EnableMs(MS_SERVER_SC);
    // 使能日志打印级别配置参数
    EnableLogOptions();

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
    config->SetProcess(PROC_SC);

    //启动tecs进程
    ret = tecs_init(sc_main);
    if (ret != SUCCESS)
    {
        printf("tecs init failed! ret = %d\n",ret);
        SkyExit(-1, "main: call tecs_init(tc_main) failed.");
    }

    // 启动调试shell
    Shell("sc_shell-> ");
    //等待SIGINT、SIGTERM信号到来后退出
    wait_signal_exit();
    return 0;
}

