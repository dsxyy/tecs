/*********************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司
* All rights reserved.
* 
* 文件名称：nat_anegnt.c
* 文件标识：
* 内容摘要：nat代理进程 ，nat设备进行接收和设置规则
* 当前版本：1.0
* 作    者：陈志伟
* 完成日期：2012年3月9日
*
* 修改记录1：
*    修改日期：2011年3月9日
*    版 本 号：V1.0
*    修 改 人：陈志伟
*    修改内容：创建
**********************************************************************/
#include "tecs_config.h"
#include "alarm_agent.h"
#include "log_agent.h"
#include "nat_agent_iptables.h"

using namespace zte_tecs;
int nat_main()
{
    STATUS ret;

    AddKeyMu(LOG_AGENT);
    AddKeyMu(ALARM_AGENT);
    AddKeyMu(MU_NAT_RULE_MANAGER);
    
    //启动系统日志
    ret = LogInit();
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"LogInit Init failed!");
    }
    
    // AlarmAgent启动
    AlarmAgent *paa = AlarmAgent::GetInstance();
    if (!paa)
    {
        SkyAssert(0);
        SkyExit(-1, "nat_main:call AlarmAgent::GetInstance failed.");
    }
    ret = paa->Init();
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"AlarmAgent Init failed!");
    }

    // nat_agent_iptables启动
    NAT_Agent_iptables *pnai = NAT_Agent_iptables::GetInstance();
    if (!pnai)
    {
        SkyAssert(0);
        SkyExit(-1, "nat_main:call NAT_Agent_iptables::GetInstance failed.");
    }
    ret = pnai->Init();
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"NAT_Agent_iptables Init failed!");
    }
 
    return 0;
}
int main(int argc, char *argv[])
{
    // -------------------------------------------------------------------------
    // sky初始化
    STATUS ret;
    TecsConfig *config = TecsConfig::Get();
    config->EnableSky();

    // 使能日志打印级别配置参数
    EnableLogOptions();
    EnableNatAgentOptions();
    
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
    config->SetProcess(PROC_NAT);
   //设置application名称,暂时不从配置文件中获取
    //config->SetApplication("nat_agent");
  
  //启动tecs进程
  ret = tecs_init(nat_main);
  if (ret != SUCCESS)
  {
    printf("tecs init failed! ret = %d\n",ret);
    SkyExit(-1, "main: call tecs_init(nat_main) failed.");
  }

  // 启动调试shell
  Shell("nat_agent_shell-> ");
  //等待SIGINT、SIGTERM信号到来后退出
  wait_signal_exit();
  return 0;
}


