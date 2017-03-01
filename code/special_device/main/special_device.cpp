/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�special_device.cpp
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ�������豸������
* ��ǰ�汾��1.0
* ��    �ߣ���ΰ
* ������ڣ�2012��9��7��
*
* �޸ļ�¼1��
*     �޸����ڣ�2012/09/07
*     �� �� �ţ�V1.0
*     �� �� �ˣ���ΰ
*     �޸����ݣ�����
*******************************************************************************/
#include "tecs_config.h"
#include "mid.h"
#include "registered_system.h"
#include "light_manager.h"

using namespace zte_tecs;
namespace zte_tecs
{
int special_device_main()
{
    // -------------------------------------------------------------------------
    LightManager *plm = LightManager::GetInstance();
    if(!plm)
    {
        SkyAssert(0);
        SkyExit(-1,"LightManager::new LightManager failed!");
    }
    //����socket�ļ�
    string cmd="mkdir -p ";
    cmd.append(LIGHT_SOCKET_PATH);
    RunCmd(cmd);
    cmd = "touch ";
    cmd.append(LIGHT_MANAGER_SOCKET);
    RunCmd(cmd);
    if (SUCCESS != plm->Init(LIGHT_MANAGER_SOCKET))
    {
        SkyAssert(0);
        SkyExit(-1,"LightManager::Init failed!");
    }
    plm->Run();
    plm->PowerOn();

    return 0;
}
void start_without_mu(TecsConfig *config)
{
    config->ClearConnects();
}
} //end namespace

int main(int argc, char** argv)
{
    STATUS ret;
    TecsConfig *config = TecsConfig::Get();
    config->EnableSky();
    config->SetTimerScanValue(20);
    
    ret = config->Parse(argc, argv);
    //���������в����Լ������ļ�
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
    config->SetProcess(PROC_SPECIAL_DEVICE);
    //��������
    start_without_mu(config);
    //ret = config->Start();
    ret = tecs_init();
    if(ret != SUCCESS)
    {
        printf("tecs init failed! ret = %d\n",ret);
        SkyExit(-1,"main(special_device.cpp): call tecs_init failed.");
    }

    // ��������shell
    Shell("special_device-> ");

    // ��ʼ�����̵���������
    if(0 != special_device_main())
    {
        printf("special_device_main failed!\n");
        SkyExit(-1, "main: call special_device_main failed.");
    }

    //�ȴ�SIGINT��SIGTERM�źŵ������˳�
    wait_signal_exit();
    return 0;
}

