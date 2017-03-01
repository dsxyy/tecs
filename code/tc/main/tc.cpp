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
#include "authmanager.h"
#include "log_agent.h"
#include "image_pool.h"
#include "mid.h"
#include "license_common.h"
#include "volume.h" 
#include "dbschema_ver.h"
#include "cluster_manager.h"
#include "cloud_node_manager.h"
#include "vmcfg_manager.h"
#include "vnet_libnet.h"
#include "project_manager.h"

using namespace zte_tecs;
namespace zte_tecs
{
int tc_main()
{
    STATUS ret = ERROR;
    ostringstream error;

    AddKeyMu(LOG_AGENT);
    AddKeyMu(ALARM_AGENT);
    AddKeyMu(MU_CLUSTER_MANAGER);
    AddKeyMu(MU_CLOUD_NODE_MANAGER);
    AddKeyMu(MU_VMCFG_MANAGER);
    AddKeyMu(MU_PROJECT_MANAGER);

    //启动系统日志
    ret = LogInit();
    if(ret != SUCCESS)
    {
        SkyAssert(0);
        SkyExit(-1, "LogInit failed!");
    }
    
    if(0 != license_init(NULL,NULL,error))
    {
        OutPut(SYS_EMERGENCY,"license_init failed!\n");
        SkyExit(-1,error.str().c_str());
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
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"AlarmAgent Init failed!");
    }
    
    // 创建消息单元
    //ClusterPool::BootStrap(GetDB());
    //集群管理
    ClusterPool *pcp = ClusterPool::CreateInstance(GetDB());
    if(!pcp)
    {
        SkyAssert(0);
        SkyExit(-1,"ClusterPool CreateInstance failed!");
    }
    ret = pcp->Init();
    if(ret != SUCCESS)
    {
        SkyAssert(0);
        SkyExit(-1,"ClusterPool Init failed!");
    }
    
    ClusterManager *pcm = ClusterManager::GetInstance();
    if(!pcm)
    {
        SkyAssert(0);
        SkyExit(-1,"ClusterManager GetInstance failed!");
    }
    ret = pcm->Init();
    if(ret != SUCCESS)
    {
        SkyAssert(0);
        SkyExit(-1,"ClusterManager Init failed!");
    }

    CloudNodePool *pnp = CloudNodePool::CreateInstance(GetDB());
    if(!pnp)
    {
        SkyAssert(0);
        SkyExit(-1,"ClusterPool CreateInstance failed!");
    }

    CloudNodeManager *pnm = CloudNodeManager::GetInstance();
    if(!pnm)
    {
        SkyAssert(0);
        SkyExit(-1,"CloudNodeManager GetInstance failed!");
    }
    ret = pnm->Init();
    if(ret != SUCCESS)
    {
        SkyAssert(0);
        SkyExit(-1,"CloudNodeManager Init failed!");
    }

    // 虚拟机配置管理
    VmCfgManager *pvc = VmCfgManager::GetInstance();
    if (!pvc)
    {
        SkyAssert(0);
        SkyExit(-1,"VmCfgManager GetInstance failed!");
    }
    ret = pvc->Init( GetDB());
    if(ret != SUCCESS)
    {
        SkyAssert(0);
        SkyExit(-1,"VmCfgManager Init failed!");
    }

    // AffinityRegionPool 必须放在 RequestManager实例创建之前
    AffinityRegionPool *parp = AffinityRegionPool::CreateInstance(GetDB());
    if (!parp)
    {
        SkyAssert(0);
        SkyExit(-1,"AffinityRegionPool CreateInstance failed!");
    }

    ret = parp->Init(false);
    if(ret != SUCCESS)
    {
        SkyAssert(0);
        SkyExit(-1,"AffinityRegionPool Init failed!");
    }

    // 虚拟机配置管理
    ProjectManager *ppm = ProjectManager::GetInstance();
    if (!ppm)
    {
        SkyAssert(0);
        SkyExit(-1,"ProjectManager GetInstance failed!");
    }
    ret = ppm->Init( GetDB());
    if(ret != SUCCESS)
    {
        SkyAssert(0);
        SkyExit(-1,"ProjectManager Init failed!");
    }
    
    // 其他消息单元在此增加
    CVNetLibNet *pnet =CVNetLibNet::CreateInstance();
    if (!pnet)
    {
        SkyAssert(0);
        SkyExit(-1,"CVNetLibNet CreateInstance failed!");
    }

    string app=ApplicationName();
  
    ret = pnet->Init(app,VNA_MODULE_ROLE_TC);
    if(ret != SUCCESS)
    {
        SkyAssert(0);
        SkyExit(-1,"CVNetLibNet Init failed!");
    }
    return 0;
}
}

int main(int argc, char** argv)
{
    STATUS ret;
    TecsConfig *config = TecsConfig::Get();
    config->EnableSky();
    config->EnableDB(DBSCHEMA_UPDATE_VERSION,DBSCHEMA_BUGFIX_VERSION);
    config->EnableMs(MS_SERVER_TC); 
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
    config->SetProcess(PROC_TC);

    //启动tecs进程
    ret = tecs_init(tc_main);
    if (ret != SUCCESS)
    {
        printf("tecs init failed! ret = %d\n",ret);
        SkyExit(-1, "main: call tecs_init(tc_main) failed.");
    }

    // 启动调试shell
    Shell("tc_shell-> ");
    //等待SIGINT、SIGTERM信号到来后退出
    wait_signal_exit();
    return 0;
}

