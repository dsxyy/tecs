/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�image_server.cpp
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ�����������������
* ��ǰ�汾��1.0
* ��    �ߣ����Ľ�
* ������ڣ�2012��2��28��
*
* �޸ļ�¼1��
*     �޸����ڣ�2012/02/28
*     �� �� �ţ�V1.0
*     �� �� �ˣ����Ľ�
*     �޸����ݣ�����
*******************************************************************************/

/****************************************************************************************
*                ͷ�ļ�                                                                *
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
* �������ƣ�tc_image_main
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
int tc_image_main()
{
    STATUS ret = ERROR;
    ostringstream error;
    
    AddKeyMu(LOG_AGENT);
    AddKeyMu(ALARM_AGENT);
    AddKeyMu(MU_IMAGE_MANAGER);
    AddKeyMu(MU_PROJECT_IMAGE_AGENT);
    
    //����ϵͳ��־
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
        SkyExit(-1, "tc_image_main: call AlarmAgent::GetInstance failed.");
    }
    ret = paa->Init();
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"AlarmAgent Init failed!");
    }

    // ӳ�����
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
    config->EnableMs(MS_SERVER_IMAGE_MANAGER);
    
    //ʹ����־��ӡ�������ò���
    EnableLogOptions();
    //ʹ��store���ò���
    EnableImageStoreOptions();
    //ʹ��ftpd���ò���
    EnableFtpdOptions();
    
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
    config->SetProcess(PROC_IMAGE_MANAGER);

    //����tecs����
    ret = tecs_init(tc_image_main);
    if(ret != SUCCESS)
    {
        printf("tecs init failed! ret = %d\n",ret);
        SkyExit(-1, "main: call tecs_init(tc_image_main) failed.");
    }

    // ��������shell
    Shell("image_manager-> ");
    //�ȴ�SIGINT��SIGTERM�źŵ������˳�
    wait_signal_exit(); 
    return 0;
}

