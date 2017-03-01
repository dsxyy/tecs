/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：vnm_main.cpp
* 文件标识：
* 内容摘要：vnm进程的main函数实现文件
* 当前版本：1.0
* 作    者：Wang.Lule
* 完成日期：2012年12月17日
*
* 修改记录1：
*     修改日期：2012/12/17
*     版 本 号：V1.0
*     修 改 人：Wang.Lule
*     修改内容：创建
******************************************************************************/
#include "tecs_config.h"
#include "vnet_comm.h"
#include "vnm_pub.h"
#include "vnet_manager.h"
#include "exc_log_agent.h"
#include "alarm_agent.h"

#include "vnet_db_mgr.h"
#include "dbschema_ver.h"

using namespace zte_tecs;

namespace zte_tecs
{
    int32 vnm_main(void)
    {
        STATUS ret;

        AddKeyMu(LOG_AGENT);
        AddKeyMu(EXC_LOG_AGENT);
        AddKeyMu(ALARM_AGENT);
        //AddKeyMu(MU_VNET_MANAGER);
        //AddKeyMu(MU_NETADAPTER);
        //AddKeyMu(MU_MACIP_SYN_MGR);
        AddKeyMu(MU_VNM);
        AddKeyMu(MU_VNM_QUERY);
        AddKeyMu(MU_VNM_SCHEDULE);
        AddKeyMu(MU_VNM_VNA_REPORT);
        
        //日志模块初始化
        ret = LogInit();    
        if(SUCCESS != ret)
        {
            SkyAssert(0);
            SkyExit(-1,"vnm_main: LogInit Init failed!");
        }
        
        //异常日志代理初始化
        ExcLogAgent *pea = ExcLogAgent::GetInstance();
        if(!pea)
        {
            SkyAssert(0);
            SkyExit(-1,"vnm_main: ExcLogAgent::GetInstance failed!");
        }
        ret = pea->Init();
        if(SUCCESS != ret)
        {
            SkyAssert(0);
            SkyExit(-1,"vnm_main: ExcLogAgent Init failed!");
        }
        
        // AlarmAgent启动
        AlarmAgent *paa = AlarmAgent::GetInstance();
        if (!paa)
        {
            SkyAssert(0);
            SkyExit(-1,"vnm_main: Create AlarmAgent failed!");
        }
        ret = paa->Init();
        if(SUCCESS != ret)
        {
            SkyAssert(0);
            SkyExit(-1,"vnm_main: AlarmAgent Init failed!");
        }  
        
        // vnet db 
        CVNetDbMgr * pNet = CVNetDbMgr::GetInstance();
        if( NULL == pNet)
        {
            SkyAssert(0);
            SkyExit(-1,"CVNetDbMgr::GetInstance failed!");
        }
        ret = pNet->Init();
        if(0 != ret )
        {
            cout << "pNet->Init failed. ret : " << ret << endl;
            SkyAssert(0);
            SkyExit(-1,"CVNetDbMgr::init failed!");    
        }

        // CVNetManager 实例化
        CVNetManager *pcvnm = CVNetManager::GetInstance();
        if (!pcvnm)
        {
            SkyAssert(0);
            SkyExit(-1, "vnm_main: CVNetManager::GetInstance() failed.");
        }
        ret = pcvnm->Init();
        if(SUCCESS != ret)
        {
            SkyAssert(0);
            SkyExit(-1,"vnm_main: CVNetManager Init failed!");
        }  
    
        return 0;
    }
}

int main(int argc, char** argv)
{
    STATUS ret;
    TecsConfig *config = TecsConfig::Get();
    config->EnableSky(); 
    //config->EnableDB(DBSCHEMA_VERSION_SUPPORT,"",true);
    config->EnableDB(DBSCHEMA_UPDATE_VERSION,DBSCHEMA_BUGFIX_VERSION,true);
    // 使能日志打印级别配置参数
    EnableLogOptions();
    
    ret = config->Parse(argc, argv);
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
    config->SetProcess(PROC_VNM);
    //启动tecs进程
    ret = tecs_init(vnm_main);
    if(ret != SUCCESS)
    {
        printf("tecs init(vnm) failed! ret = %d\n",ret);
        SkyExit(-1, "main: call tecs_init(vnm_main) failed!");
    }
    
    // 启动调试shell
    Shell("vnm_shell-> ");
    //等待SIGINT、SIGTERM信号到来后退出
    wait_signal_exit(); 
    return 0;
}


