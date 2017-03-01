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
#include "authmanager.h"
#include "log_agent.h"
#include "image_pool.h"
#include "mid.h"
#include "license_common.h"
#include "volume.h" 
#include "dbschema_ver.h"
#include "cluster_manager.h"
#include "cloud_node_manager.h"
#include "vmcfg_manager.h"
#include "vnet_libnet.h"
#include "project_manager.h"

using namespace zte_tecs;
namespace zte_tecs
{
int tc_main()
{
    STATUS ret = ERROR;
    ostringstream error;

    AddKeyMu(LOG_AGENT);
    AddKeyMu(ALARM_AGENT);
    AddKeyMu(MU_CLUSTER_MANAGER);
    AddKeyMu(MU_CLOUD_NODE_MANAGER);
    AddKeyMu(MU_VMCFG_MANAGER);
    AddKeyMu(MU_PROJECT_MANAGER);

    //����ϵͳ��־
    ret = LogInit();
    if(ret != SUCCESS)
    {
        SkyAssert(0);
        SkyExit(-1, "LogInit failed!");
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
        SkyExit(-1, "tc_main: call AlarmAgent::GetInstance failed.");
    }
    ret = paa->Init();
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"AlarmAgent Init failed!");
    }
    
    // ������Ϣ��Ԫ
    //ClusterPool::BootStrap(GetDB());
    //��Ⱥ����
    ClusterPool *pcp = ClusterPool::CreateInstance(GetDB());
    if(!pcp)
    {
        SkyAssert(0);
        SkyExit(-1,"ClusterPool CreateInstance failed!");
    }
    ret = pcp->Init();
    if(ret != SUCCESS)
    {
        SkyAssert(0);
        SkyExit(-1,"ClusterPool Init failed!");
    }
    
    ClusterManager *pcm = ClusterManager::GetInstance();
    if(!pcm)
    {
        SkyAssert(0);
        SkyExit(-1,"ClusterManager GetInstance failed!");
    }
    ret = pcm->Init();
    if(ret != SUCCESS)
    {
        SkyAssert(0);
        SkyExit(-1,"ClusterManager Init failed!");
    }

    CloudNodePool *pnp = CloudNodePool::CreateInstance(GetDB());
    if(!pnp)
    {
        SkyAssert(0);
        SkyExit(-1,"ClusterPool CreateInstance failed!");
    }

    CloudNodeManager *pnm = CloudNodeManager::GetInstance();
    if(!pnm)
    {
        SkyAssert(0);
        SkyExit(-1,"CloudNodeManager GetInstance failed!");
    }
    ret = pnm->Init();
    if(ret != SUCCESS)
    {
        SkyAssert(0);
        SkyExit(-1,"CloudNodeManager Init failed!");
    }

    // ��������ù���
    VmCfgManager *pvc = VmCfgManager::GetInstance();
    if (!pvc)
    {
        SkyAssert(0);
        SkyExit(-1,"VmCfgManager GetInstance failed!");
    }
    ret = pvc->Init( GetDB());
    if(ret != SUCCESS)
    {
        SkyAssert(0);
        SkyExit(-1,"VmCfgManager Init failed!");
    }

    // AffinityRegionPool ������� RequestManagerʵ������֮ǰ
    AffinityRegionPool *parp = AffinityRegionPool::CreateInstance(GetDB());
    if (!parp)
    {
        SkyAssert(0);
        SkyExit(-1,"AffinityRegionPool CreateInstance failed!");
    }

    ret = parp->Init(false);
    if(ret != SUCCESS)
    {
        SkyAssert(0);
        SkyExit(-1,"AffinityRegionPool Init failed!");
    }

    // ��������ù���
    ProjectManager *ppm = ProjectManager::GetInstance();
    if (!ppm)
    {
        SkyAssert(0);
        SkyExit(-1,"ProjectManager GetInstance failed!");
    }
    ret = ppm->Init( GetDB());
    if(ret != SUCCESS)
    {
        SkyAssert(0);
        SkyExit(-1,"ProjectManager Init failed!");
    }
    
    // ������Ϣ��Ԫ�ڴ�����
    CVNetLibNet *pnet =CVNetLibNet::CreateInstance();
    if (!pnet)
    {
        SkyAssert(0);
        SkyExit(-1,"CVNetLibNet CreateInstance failed!");
    }

    string app=ApplicationName();
  
    ret = pnet->Init(app,VNA_MODULE_ROLE_TC);
    if(ret != SUCCESS)
    {
        SkyAssert(0);
        SkyExit(-1,"CVNetLibNet Init failed!");
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
    config->EnableMs(MS_SERVER_TC); 
    // ʹ����־��ӡ�������ò���
    EnableLogOptions();


    ret = config->Parse(argc, argv);
	
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
    config->SetProcess(PROC_TC);

    //����tecs����
    ret = tecs_init(tc_main);
    if (ret != SUCCESS)
    {
        printf("tecs init failed! ret = %d\n",ret);
        SkyExit(-1, "main: call tecs_init(tc_main) failed.");
    }

    // ��������shell
    Shell("tc_shell-> ");
    //�ȴ�SIGINT��SIGTERM�źŵ������˳�
    wait_signal_exit();
    return 0;
}

