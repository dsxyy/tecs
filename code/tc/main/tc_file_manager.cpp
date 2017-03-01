/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�tc_file_server.cpp
* �ļ���ʶ��tecs �ļ��������
* ����ժҪ�����������������
* ��ǰ�汾��1.0
* ��    �ߣ���Т��
* ������ڣ�2013��2��19��
*******************************************************************************/

/****************************************************************************************
*                ͷ�ļ�                                                                *
****************************************************************************************/
#include "mid.h"
#include "tecs_config.h"
#include "log_agent.h"
#include "alarm_agent.h"
#include "file_manager.h"
#include "dbschema_ver.h"

using namespace zte_tecs;
namespace zte_tecs
{
/**********************************************************************
* �������ƣ�tc_file_main
* ����������
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����     �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2012/02/28   V1.0    ���Ľ�      ����
************************************************************************/
int tc_file_main()
{
    STATUS ret = ERROR;
    ostringstream error;
    
    AddKeyMu(LOG_AGENT);
    AddKeyMu(ALARM_AGENT);
    AddKeyMu(MU_FILE_MANAGER);
    
    //����ϵͳ��־
    ret = LogInit();
    if(ret != SUCCESS)
    {
        SkyAssert(0);
        SkyExit(-1, "tc_file_main: call LogInit failed.");
    }

    // AlarmAgent����
    AlarmAgent *paa = AlarmAgent::GetInstance();
    if (!paa)
    {
        SkyAssert(0);
        SkyExit(-1, "tc_file_main: call AlarmAgent::GetInstance failed.");
    }
    ret = paa->Init();
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"AlarmAgent Init failed!");
    }
        
    FileManager *fim = FileManager::GetInstance();
    if (!fim)
    {
        SkyAssert(0);
        SkyExit(-1,"FileManager CreateInstance failed!");
    }
    ret = fim->Init();
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"FileManager Init failed!");
    }
    return 0;
}
}

/**********************************************************************
* �������ƣ�main����
* ����������
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����     �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2012/02/28   V1.0    ���Ľ�      ����
************************************************************************/
int main(int argc, char** argv)
{ 
    STATUS ret;
    TecsConfig *config = TecsConfig::Get();
    config->EnableSky();
    config->EnableDB(DBSCHEMA_UPDATE_VERSION,DBSCHEMA_BUGFIX_VERSION);
    config->EnableMs(MS_SERVER_FILE_MANAGER);
    
    //ʹ����־��ӡ�������ò���
    EnableLogOptions();
    //ʹ��File���ò���
    EnableCoredumpFileOptions();
    
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
    config->SetProcess(PROC_FILE_MANAGER);

    //����tecs����
    ret = tecs_init(tc_file_main);
    if(ret != SUCCESS)
    {
        printf("tecs init failed! ret = %d\n",ret);
        SkyExit(-1, "main: call tecs_init(tc_file_main) failed.");
    }

    // ��������shell
    Shell("file_manager-> ");
    //�ȴ�SIGINT��SIGTERM�źŵ������˳�
    wait_signal_exit(); 
    return 0;
}

