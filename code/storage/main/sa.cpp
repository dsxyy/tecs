/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�tc.cpp
* �ļ���ʶ��
* ����ժҪ��tc���̵�main����ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�Ԭ����
* ������ڣ�2011��8��19��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/8/19
*     �� �� �ţ�V1.0
*     �� �� �ˣ�Ԭ����
*     �޸����ݣ�����
******************************************************************************/
#include "tecs_config.h"
#include "alarm_agent.h"
#include "log_agent.h"
#include "storage_cli.h"
#include "mid.h"

using namespace zte_tecs;

namespace zte_tecs
{
int sa_main()
{
    STATUS ret = ERROR;
    AddKeyMu(LOG_AGENT);


    //����ϵͳ��־
    ret = LogInit();
    if (ret != SUCCESS)
    {
        SkyAssert(0);
        SkyExit(-1, "LogInit failed!");
    }

    StorageAdaptorInitiator *pcli = StorageAdaptorInitiator::GetInstance();
    if (!pcli)
    {
        SkyAssert(0);
        SkyExit(-1, "sa_main: call StorageAdaptorInitiator::GetInstance failed.");
    }
    ret = pcli->Init();
    if (SUCCESS != ret)
    {
        while(1)
        {            
            OutPut(SYS_NOTICE,"StorageAdaptorInitiator Init failed, pls check sa config file and restart sa\n");
            sleep(5);
        }
        
    }
    return 0;
}
}

int main(int argc, char** argv)
{
    STATUS ret;
    TecsConfig *config = TecsConfig::Get();
    config->EnableSky();

    // config->EnableMs(MS_SERVER_TC);
    // ʹ����־��ӡ�������ò���
    EnableLogOptions();
    EnableSaStoreOptions();

    ret = config->Parse(argc, argv);
    //���������в����Լ������ļ�
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

    //���ý�������
    config->SetProcess(PROC_SA);

    //����tecs����
    ret = tecs_init();
    if (ret != SUCCESS)
    {
        printf("tecs init failed! ret = %d\n",ret);
        SkyExit(-1, "main: call tecs_init(tc_main) failed.");
    }


    // ��������shell
    Shell("sa_shell-> ");

    // ��ʼ�����̵���������
    if (0 != sa_main())
    {
        printf("sa_main failed!\n");
        SkyExit(-1, "main: call sa_main failed.");
    }

    //�ȴ�SIGINT��SIGTERM�źŵ������˳�
    wait_signal_exit();
    return 0;
}

