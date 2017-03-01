/******************************************************************************
* Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：cc_image.cpp
* 文件标识：
* 内容摘要：image代理进程的main函数实现文件
* 当前版本：1.0
* 作    者：张文剑
* 完成日期：2012年3月12日
*
* 修改记录1：
*     修改日期：2012/3/12
*     版 本 号：V1.0
*     修 改 人：张文剑
*     修改内容：创建
******************************************************************************/
#include "tecs_config.h"
#include "log_agent.h"
#include "alarm_agent.h"
#include "image_agent.h"
#include "dbschema_ver.h"
#include "config_cluster.h"
#include "proc_admin.h"
#include "vm_disk_share.h"
#include "image_cache.h"
#include "image_download.h"
#include "image_base.h"

using namespace zte_tecs;
namespace zte_tecs
{
int cc_image_main()
{
    STATUS ret;
    AddKeyMu(LOG_AGENT);
    AddKeyMu(ALARM_AGENT);
    AddKeyMu(MU_IMAGE_AGENT);
    AddKeyMu(MU_IMAGE_DOWNLOAD_AGENT);

    ret = LogInit();
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"LogInit Init failed!");
    }
    
    //工作流引擎启动
    ret = StartWorkflowEngine(GetPrivateDB(),3);
    if(ret != SUCCESS)
    {
        SkyAssert(0);
        SkyExit(-1, "StartWorkflowEngine failed!");
    }
    
    ConfigCluster *ccp = ConfigCluster::CreateInstance(GetDB());
    if(!ccp)
    {
        SkyAssert(0);
        SkyExit(-1,"ConfigCluster::CreateInstance failed!");
    }
    
    // AlarmAgent启动
    AlarmAgent *paa = AlarmAgent::GetInstance();
    if (!paa)
    {
        SkyAssert(0);
        SkyExit(-1,"AlarmAgent::GetInstance failed!");
    }
    ret = paa->Init();
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"AlarmAgent Init failed!");
    }
        
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
    
    ImageCachePool *pimgcp = ImageCachePool::CreateInstance(GetDB());
    if(!pimgcp)
    {
        SkyAssert(0);
        SkyExit(-1,"ImageCachePool::CreateInstance failed!");
    }   
    ret = pimgcp->Init();
    if(SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY,"ImageCachePool init failed!ret=%d\n",ret);
        SkyAssert(0);
        SkyExit(-1,"ImageCachePool init failed!");
    } 
    
    VmDiskSharePool *vdsp = VmDiskSharePool::GetInstance(GetDB());
    if(!vdsp)
    {
        SkyAssert(0);
        SkyExit(-1,"VmDiskSharePool::GetInstance failed!");
    }  
    
    ImageBaseOperationSet *pIboSet = ImageBaseOperationSet::GetInstance(GetDB());
    if (!pIboSet)
    {
        SkyAssert(0);
        SkyExit(-1,"ImageBaseOperationSet GetInstance failed!");
    }
    
    ret = pIboSet->Init(GetDB());
    if(SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY,"ImageBaseOperationSet Init failed! ret = %d\n",ret);
        SkyAssert(0);
        SkyExit(-1,"ImageBaseOperationSet Init failed!");
    }  
    
    ImageAgent *pia = ImageAgent::GetInstance();
    if(!pia)
    {
        SkyAssert(0);
        SkyExit(-1,"ImageAgent::GetInstance failed!");
    } 
    ret = pia->Init();
    if(SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY,"ImageAgent Init failed! ret = %d\n",ret);
        SkyAssert(0);
        SkyExit(-1,"ImageAgent Init failed!");
    }  

    //创建并启动虚拟机管理器VMManager
    ImageDownloadAgent *pida = ImageDownloadAgent::GetInstance();
    if(!pida)
    {
        SkyAssert(0);
        SkyExit(-1,"ImageDownloadAgent::GetInstance failed!");
    } 
    ret = pida->Init();
    if(SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY,"ImageDownloadAgent Init failed! ret = %d\n",ret);
        SkyAssert(0);
        SkyExit(-1,"ImageDownloadAgent Init failed!");
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
    config->EnableMs(MS_SERVER_IMAGE_PROXY);    
    //使能日志打印级别配置参数
    EnableLogOptions();
    //使能映像代理模块配置参数
    EnableImageAgentOptions();
    
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
    config->SetProcess(PROC_IMAGE_AGENT);
    //启动tecs进程
    ret = tecs_init(cc_image_main);
    if(ret != SUCCESS)
    {
        printf("tecs init failed! ret = %d\n",ret);
        SkyExit(-1, "main: call tecs_init(cc_image_main) failed!");
    }
    
    // 启动调试shell
    Shell("image_agent-> ");
    //等待SIGINT、SIGTERM信号到来后退出
    wait_signal_exit(); 
    return 0;
}


