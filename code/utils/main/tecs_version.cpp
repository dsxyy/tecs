/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：tecs_version.cpp
* 文件标识：
* 内容摘要：版本管理main函数实现文件
* 当前版本：1.0
* 作    者：licanwei
* 完成日期：2011年8月29日
*
* 修改记录1：
*     修改日期：2011/8/29
*     版 本 号：V1.0
*     修 改 人：licanwei
*     修改内容：创建
******************************************************************************/
#include "tecs_config.h"
#include "sky.h"
#include "event.h"
#include "mid.h"
#include "version_manager.h"
#include "log_agent.h"
#include "proc_admin.h"

using namespace zte_tecs;

/******************************************************************************/
VersionManager *tecsversion;

int main(int argc, char** argv)
{
    STATUS ret;
    TecsConfig *config = TecsConfig::Get();
    config->EnableSky();
    config->EnableDB();

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
    config->SetProcess(PROC_VERSION_MANAGER);
    //启动配置
    ret = tecs_init(NULL);
    if(ret != SUCCESS)
    {
        printf("tecs init failed! ret = %d\n",ret);
        SkyExit(-1, "main(tecs_version.cpp): call tecs_init() failed.");
    }

    //初始化系统日志异常日志记录
    ret = LogInit();
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"LogInit Init failed!");
    }
    AddKeyMu(MU_VERSION_MANAGER);
    Shell("tecs_version-> ");
    
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

    VersionManager *pvm = VersionManager::GetInstance();
	if(!pvm)
    {
        SkyAssert(0);
        SkyExit(-1,"VersionManager::GetInstance failed!");
    }
    ret = pvm->Init();
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"VersionManager Init failed!");
    }
    
    Shell("tecs_version-> ");
    //等待SIGINT、SIGTERM信号到来后退出
    wait_signal_exit(); 
    return 0;
}



