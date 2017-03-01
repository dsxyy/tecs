/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：image_server.cpp
* 文件标识：见配置管理计划书
* 内容摘要：镜像服务器主进程
* 当前版本：1.0
* 作    者：张文剑
* 完成日期：2012年2月28日
*
* 修改记录1：
*     修改日期：2012/02/28
*     版 本 号：V1.0
*     修 改 人：张文剑
*     修改内容：创建
*******************************************************************************/

/****************************************************************************************
*                头文件                                                                *
****************************************************************************************/
#include "mid.h"
#include "tecs_config.h"
#include "log_agent.h"
#include "alarm_agent.h"
#include "image_store.h"
#include "image_pool.h"
#include "base_image.h"
#include "image_manager.h"
#include "authmanager.h"
#include "ftpd.h"
#include "license_common.h"
#include "dbschema_ver.h"
#include "project_image.h"

using namespace zte_tecs;
namespace zte_tecs
{
/**********************************************************************
* 函数名称：tc_image_main
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
int tc_image_main()
{
    STATUS ret = ERROR;
    ostringstream error;
    
    AddKeyMu(LOG_AGENT);
    AddKeyMu(ALARM_AGENT);
    AddKeyMu(MU_IMAGE_MANAGER);
    AddKeyMu(MU_PROJECT_IMAGE_AGENT);
    
    //启动系统日志
    ret = LogInit();
    if(ret != SUCCESS)
    {
        SkyAssert(0);
        SkyExit(-1, "tc_image_main: call LogInit failed.");
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
        SkyExit(-1, "tc_image_main: call AlarmAgent::GetInstance failed.");
    }
    ret = paa->Init();
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"AlarmAgent Init failed!");
    }

    // 映像管理
    ImagePool *pip = ImagePool::CreateInstance(GetDB());
    if (!pip)
    {
        SkyAssert(0);
        SkyExit(-1,"ImagePool CreateInstance failed!");
    }
    ret = pip->Init();
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"ImagePool Init failed!");
    }
        
    BaseImageOperationSet *pBioSet = BaseImageOperationSet::GetInstance(GetDB());
    if (!pBioSet)
    {
        SkyAssert(0);
        SkyExit(-1,"BaseImageOperationSet GetInstance failed!");
    }
    
    ret = pBioSet->Init(GetDB());
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"BaseImageOperationSet Init failed!");
    }
    
    ImageManager *pim = ImageManager::GetInstance();
    if (!pim)
    {
        SkyAssert(0);
        SkyExit(-1,"ImageManager GetInstance failed!");
    }
    ret = pim->Init();
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"ImageManager Init failed!");
    }
    
    ret = ImageStoreInit();
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"ImageStoreInit failed!");
    }
    
    ProjectImageAgent *ppia = ProjectImageAgent::GetInstance();
    if (!ppia)
    {
        SkyAssert(0);
        SkyExit(-1,"ProjectImageAgent GetInstance failed!");
    }
    ret = ppia->Init();
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"ProjectImageAgent Init failed!");
    }
    
    ret = FtpdInit();
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"FtpdInit failed!");
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
    config->EnableMs(MS_SERVER_IMAGE_MANAGER);
    
    //使能日志打印级别配置参数
    EnableLogOptions();
    //使能store配置参数
    EnableImageStoreOptions();
    //使能ftpd配置参数
    EnableFtpdOptions();
    
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
    config->SetProcess(PROC_IMAGE_MANAGER);

    //启动tecs进程
    ret = tecs_init(tc_image_main);
    if(ret != SUCCESS)
    {
        printf("tecs init failed! ret = %d\n",ret);
        SkyExit(-1, "main: call tecs_init(tc_image_main) failed.");
    }

    // 启动调试shell
    Shell("image_manager-> ");
    //等待SIGINT、SIGTERM信号到来后退出
    wait_signal_exit(); 
    return 0;
}

