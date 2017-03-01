/******************************************************************************
* Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�cc_image.cpp
* �ļ���ʶ��
* ����ժҪ��image������̵�main����ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ����Ľ�
* ������ڣ�2012��3��12��
*
* �޸ļ�¼1��
*     �޸����ڣ�2012/3/12
*     �� �� �ţ�V1.0
*     �� �� �ˣ����Ľ�
*     �޸����ݣ�����
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
    
    //��������������
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
    
    // AlarmAgent����
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
        
    // ������Ϣ��Ԫ
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

    //���������������������VMManager
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
    //ʹ����־��ӡ�������ò���
    EnableLogOptions();
    //ʹ��ӳ�����ģ�����ò���
    EnableImageAgentOptions();
    
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
    config->SetProcess(PROC_IMAGE_AGENT);
    //����tecs����
    ret = tecs_init(cc_image_main);
    if(ret != SUCCESS)
    {
        printf("tecs init failed! ret = %d\n",ret);
        SkyExit(-1, "main: call tecs_init(cc_image_main) failed!");
    }
    
    // ��������shell
    Shell("image_agent-> ");
    //�ȴ�SIGINT��SIGTERM�źŵ������˳�
    wait_signal_exit(); 
    return 0;
}


