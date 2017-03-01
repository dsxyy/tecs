/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�vnm_main.cpp
* �ļ���ʶ��
* ����ժҪ��vnm���̵�main����ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�Wang.Lule
* ������ڣ�2012��12��17��
*
* �޸ļ�¼1��
*     �޸����ڣ�2012/12/17
*     �� �� �ţ�V1.0
*     �� �� �ˣ�Wang.Lule
*     �޸����ݣ�����
******************************************************************************/
#include "tecs_config.h"
#include "vnet_comm.h"
#include "vnm_pub.h"
#include "vnet_manager.h"
#include "exc_log_agent.h"
#include "alarm_agent.h"

#include "vnet_db_mgr.h"
#include "dbschema_ver.h"

using namespace zte_tecs;

namespace zte_tecs
{
    int32 vnm_main(void)
    {
        STATUS ret;

        AddKeyMu(LOG_AGENT);
        AddKeyMu(EXC_LOG_AGENT);
        AddKeyMu(ALARM_AGENT);
        //AddKeyMu(MU_VNET_MANAGER);
        //AddKeyMu(MU_NETADAPTER);
        //AddKeyMu(MU_MACIP_SYN_MGR);
        AddKeyMu(MU_VNM);
        AddKeyMu(MU_VNM_QUERY);
        AddKeyMu(MU_VNM_SCHEDULE);
        AddKeyMu(MU_VNM_VNA_REPORT);
        
        //��־ģ���ʼ��
        ret = LogInit();    
        if(SUCCESS != ret)
        {
            SkyAssert(0);
            SkyExit(-1,"vnm_main: LogInit Init failed!");
        }
        
        //�쳣��־�����ʼ��
        ExcLogAgent *pea = ExcLogAgent::GetInstance();
        if(!pea)
        {
            SkyAssert(0);
            SkyExit(-1,"vnm_main: ExcLogAgent::GetInstance failed!");
        }
        ret = pea->Init();
        if(SUCCESS != ret)
        {
            SkyAssert(0);
            SkyExit(-1,"vnm_main: ExcLogAgent Init failed!");
        }
        
        // AlarmAgent����
        AlarmAgent *paa = AlarmAgent::GetInstance();
        if (!paa)
        {
            SkyAssert(0);
            SkyExit(-1,"vnm_main: Create AlarmAgent failed!");
        }
        ret = paa->Init();
        if(SUCCESS != ret)
        {
            SkyAssert(0);
            SkyExit(-1,"vnm_main: AlarmAgent Init failed!");
        }  
        
        // vnet db 
        CVNetDbMgr * pNet = CVNetDbMgr::GetInstance();
        if( NULL == pNet)
        {
            SkyAssert(0);
            SkyExit(-1,"CVNetDbMgr::GetInstance failed!");
        }
        ret = pNet->Init();
        if(0 != ret )
        {
            cout << "pNet->Init failed. ret : " << ret << endl;
            SkyAssert(0);
            SkyExit(-1,"CVNetDbMgr::init failed!");    
        }

        // CVNetManager ʵ����
        CVNetManager *pcvnm = CVNetManager::GetInstance();
        if (!pcvnm)
        {
            SkyAssert(0);
            SkyExit(-1, "vnm_main: CVNetManager::GetInstance() failed.");
        }
        ret = pcvnm->Init();
        if(SUCCESS != ret)
        {
            SkyAssert(0);
            SkyExit(-1,"vnm_main: CVNetManager Init failed!");
        }  
    
        return 0;
    }
}

int main(int argc, char** argv)
{
    STATUS ret;
    TecsConfig *config = TecsConfig::Get();
    config->EnableSky(); 
    //config->EnableDB(DBSCHEMA_VERSION_SUPPORT,"",true);
    config->EnableDB(DBSCHEMA_UPDATE_VERSION,DBSCHEMA_BUGFIX_VERSION,true);
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
    config->SetProcess(PROC_VNM);
    //����tecs����
    ret = tecs_init(vnm_main);
    if(ret != SUCCESS)
    {
        printf("tecs init(vnm) failed! ret = %d\n",ret);
        SkyExit(-1, "main: call tecs_init(vnm_main) failed!");
    }
    
    // ��������shell
    Shell("vnm_shell-> ");
    //�ȴ�SIGINT��SIGTERM�źŵ������˳�
    wait_signal_exit(); 
    return 0;
}


