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
#include "workflow.h"
#include "alarm_agent.h"
#include "log_agent.h"
#include "mid.h"
#include "storage_control.h"
#include "sa_manager.h"
#include "dbschema_ver.h"

using namespace zte_tecs;
namespace zte_tecs
{
int sc_main()
{
    STATUS ret = ERROR;
    AddKeyMu(LOG_AGENT);
    AddKeyMu(ALARM_AGENT);    
    AddKeyMu(MU_SA_MANAGER);
    AddKeyMu(MU_SC_CONTROL);

    //����ϵͳ��־
    ret = LogInit();
    if (ret != SUCCESS)
    {
        SkyAssert(0);
        SkyExit(-1, "LogInit failed!");
    }

    //��������������
    ret = StartWorkflowEngine(GetPrivateDB(),3);
    if(ret != SUCCESS)
    {
        SkyAssert(0);
        SkyExit(-1, "StartWorkflowEngine failed!");
    }
    
    // AlarmAgent����
    AlarmAgent *paa = AlarmAgent::GetInstance();
    if (!paa)
    {
        SkyAssert(0);
        SkyExit(-1, "tc_main: call AlarmAgent::GetInstance failed.");
    }
    ret = paa->Init();
    if (SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"AlarmAgent Init failed!");
    }
#if 1
    SaManager *pSa_manager=SaManager::GetInstance();
    if (!pSa_manager)
    {
        SkyAssert(0);
        SkyExit(-1, "tc_main: call SaManager::GetInstance failed.");
    }
    ret = pSa_manager->Init();
    if (SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"SaManager Init failed!");
    }
#endif
    StorageController *pSc=StorageController::GetInstance();
    if (!pSc)
    {
        SkyAssert(0);
        SkyExit(-1, "tc_main: call StorageController::GetInstance failed.");
    }
    ret = pSc->Init();
    if (SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"StorageController Init failed!");
    }
    // ������Ϣ��Ԫ�ڴ�����

    return 0;
}
}

int main(int argc, char** argv)
{
    STATUS ret;
    TecsConfig *config = TecsConfig::Get();
    config->EnableSky();
    config->EnableDB(DBSCHEMA_UPDATE_VERSION,DBSCHEMA_BUGFIX_VERSION);
    config->EnableMs(MS_SERVER_SC);
    // ʹ����־��ӡ�������ò���
    EnableLogOptions();

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
    config->SetProcess(PROC_SC);

    //����tecs����
    ret = tecs_init(sc_main);
    if (ret != SUCCESS)
    {
        printf("tecs init failed! ret = %d\n",ret);
        SkyExit(-1, "main: call tecs_init(tc_main) failed.");
    }

    // ��������shell
    Shell("sc_shell-> ");
    //�ȴ�SIGINT��SIGTERM�źŵ������˳�
    wait_signal_exit();
    return 0;
}

