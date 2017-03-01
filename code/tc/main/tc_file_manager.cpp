/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：tc_file_server.cpp
* 文件标识：tecs 文件管理服务
* 内容摘要：镜像服务器主进程
* 当前版本：1.0
* 作    者：李孝成
* 完成日期：2013年2月19日
*******************************************************************************/

/****************************************************************************************
*                头文件                                                                *
****************************************************************************************/
#include "mid.h"
#include "tecs_config.h"
#include "log_agent.h"
#include "alarm_agent.h"
#include "file_manager.h"
#include "dbschema_ver.h"

using namespace zte_tecs;
namespace zte_tecs
{
/**********************************************************************
* 函数名称：tc_file_main
* 功能描述：
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期     版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2012/02/28   V1.0    张文剑      创建
************************************************************************/
int tc_file_main()
{
    STATUS ret = ERROR;
    ostringstream error;
    
    AddKeyMu(LOG_AGENT);
    AddKeyMu(ALARM_AGENT);
    AddKeyMu(MU_FILE_MANAGER);
    
    //启动系统日志
    ret = LogInit();
    if(ret != SUCCESS)
    {
        SkyAssert(0);
        SkyExit(-1, "tc_file_main: call LogInit failed.");
    }

    // AlarmAgent启动
    AlarmAgent *paa = AlarmAgent::GetInstance();
    if (!paa)
    {
        SkyAssert(0);
        SkyExit(-1, "tc_file_main: call AlarmAgent::GetInstance failed.");
    }
    ret = paa->Init();
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"AlarmAgent Init failed!");
    }
        
    FileManager *fim = FileManager::GetInstance();
    if (!fim)
    {
        SkyAssert(0);
        SkyExit(-1,"FileManager CreateInstance failed!");
    }
    ret = fim->Init();
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"FileManager Init failed!");
    }
    return 0;
}
}

/**********************************************************************
* 函数名称：main函数
* 功能描述：
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期     版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2012/02/28   V1.0    张文剑      创建
************************************************************************/
int main(int argc, char** argv)
{ 
    STATUS ret;
    TecsConfig *config = TecsConfig::Get();
    config->EnableSky();
    config->EnableDB(DBSCHEMA_UPDATE_VERSION,DBSCHEMA_BUGFIX_VERSION);
    config->EnableMs(MS_SERVER_FILE_MANAGER);
    
    //使能日志打印级别配置参数
    EnableLogOptions();
    //使能File配置参数
    EnableCoredumpFileOptions();
    
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
    config->SetProcess(PROC_FILE_MANAGER);

    //启动tecs进程
    ret = tecs_init(tc_file_main);
    if(ret != SUCCESS)
    {
        printf("tecs init failed! ret = %d\n",ret);
        SkyExit(-1, "main: call tecs_init(tc_file_main) failed.");
    }

    // 启动调试shell
    Shell("file_manager-> ");
    //等待SIGINT、SIGTERM信号到来后退出
    wait_signal_exit(); 
    return 0;
}

