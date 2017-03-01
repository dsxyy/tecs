/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�tecs_version.cpp
* �ļ���ʶ��
* ����ժҪ���汾����main����ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�licanwei
* ������ڣ�2011��8��29��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/8/29
*     �� �� �ţ�V1.0
*     �� �� �ˣ�licanwei
*     �޸����ݣ�����
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

    // ʹ����־��ӡ�������ò���
    EnableLogOptions();

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
    config->SetProcess(PROC_VERSION_MANAGER);
    //��������
    ret = tecs_init(NULL);
    if(ret != SUCCESS)
    {
        printf("tecs init failed! ret = %d\n",ret);
        SkyExit(-1, "main(tecs_version.cpp): call tecs_init() failed.");
    }

    //��ʼ��ϵͳ��־�쳣��־��¼
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
    //�ȴ�SIGINT��SIGTERM�źŵ������˳�
    wait_signal_exit(); 
    return 0;
}



