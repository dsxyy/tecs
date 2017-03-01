/*********************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾
* All rights reserved.
* 
* �ļ����ƣ�nat_anegnt.c
* �ļ���ʶ��
* ����ժҪ��nat������� ��nat�豸���н��պ����ù���
* ��ǰ�汾��1.0
* ��    �ߣ���־ΰ
* ������ڣ�2012��3��9��
*
* �޸ļ�¼1��
*    �޸����ڣ�2011��3��9��
*    �� �� �ţ�V1.0
*    �� �� �ˣ���־ΰ
*    �޸����ݣ�����
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
    
    //����ϵͳ��־
    ret = LogInit();
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"LogInit Init failed!");
    }
    
    // AlarmAgent����
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

    // nat_agent_iptables����
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
    // sky��ʼ��
    STATUS ret;
    TecsConfig *config = TecsConfig::Get();
    config->EnableSky();

    // ʹ����־��ӡ�������ò���
    EnableLogOptions();
    EnableNatAgentOptions();
    
    ret = config->Parse(argc, argv);
    //���������в����Լ������ļ�
    if(SUCCESS == ret)
    {
        if (config->IsHelp())
        {
            return 0;
        }        
    }	
	
    //��ʼ������
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

   //���ý�������
    config->SetProcess(PROC_NAT);
   //����application����,��ʱ���������ļ��л�ȡ
    //config->SetApplication("nat_agent");
  
  //����tecs����
  ret = tecs_init(nat_main);
  if (ret != SUCCESS)
  {
    printf("tecs init failed! ret = %d\n",ret);
    SkyExit(-1, "main: call tecs_init(nat_main) failed.");
  }

  // ��������shell
  Shell("nat_agent_shell-> ");
  //�ȴ�SIGINT��SIGTERM�źŵ������˳�
  wait_signal_exit();
  return 0;
}


