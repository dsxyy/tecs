/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�wdg.cpp
* �ļ���ʶ��
* ����ժҪ��wdg���̵�main����ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�liuhx
* ������ڣ�2012��12��14��
******************************************************************************/
#include "tecs_config.h"
#include "registered_system.h"
#include "proc_admin.h"
#include "vnet_comm.h"
#include "wdg_common.h"
#include "vnet_monitor.h"
#include "wdg_pub.h"
#include "vnet_wdg.h"
#include "vnet_libnet.h"

using namespace zte_tecs;
namespace zte_tecs
{

extern bool enable_evb; 
extern string g_strHostName;
string g_strWdgApplication = "";
#if 1
int wdg_main()
{
    STATUS ret;

    AddKeyMu(LOG_AGENT);
    AddKeyMu(MU_PROC_ADMIN);
    //AddKeyMu(MU_VNA_CONTROLLER);
    //AddKeyMu(MU_VNA_HEARTBEAT);
    AddKeyMu(MU_VNET_WDG);
        
    //ִ��Log ��־��¼��ʼ��,1��ʾ����ΪHC,��������HC�Ľ��̶������ṩ�������
    ret = LogInit(0);
    if(ret != SUCCESS)
    {
        SkyAssert(0);
        SkyExit(-1, "wdg_main: call LogInit failed.");
    }

    // -------------------------------------------------------------------------
    // ������Ϣ��Ԫ
    ProcAdmin *pProcAdmin = ProcAdmin::GetInstance();
    if(!pProcAdmin)
    {
        SkyAssert(0);
        SkyExit(-1,"ProcAdmin::GetInstance failed!");
    }
    ret = pProcAdmin->Init();
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"ProcAdmin Init failed!");
    }

    //�ȴ�VNM��Ϣ�ѻ�ȡ
    //WaitRegisteredSystem();
    
    //��ȡ������CC֮������Ҫ���Լ��ϴ�����ʱ�����Է��ͳ�ȥ
    LogLastwords();
    
     // vnet ��ʼ��
#if 0
    CVNAController *pCVNAController = CVNAController::GetInstance();
    if(!pCVNAController)
    {
        SkyAssert(0);
        SkyExit(-1,"CVNAController::GetInstance failed!\n");
    }
    ret = pCVNAController->Init();
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"CVNAController Init failed!\n");
    }
#endif

    CVNetLibNet *pvnetlib = CVNetLibNet::CreateInstance();
    if(pvnetlib == NULL)
    {
        SkyAssert(0);
        SkyExit(-1,"CVNetLibNet::CreateInstance!");
    }
    
    if(SUCCESS != pvnetlib->Init(g_strWdgApplication, VNA_MODULE_ROLE_WATCHDOG))
    {
        SkyAssert(0);
        SkyExit(-1,"pvnetlib Init failed!");
    }

    CVNetWatchDog *pWdgHandle = CVNetWatchDog::GetInstance();
    if(!pWdgHandle)
    {
        SkyAssert(0);
        SkyExit(-1,"CVNetWatchDog::GetInstance failed!\n");
    }

    ret = pWdgHandle->Init();
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"CVNetWatchDog Init failed!\n");
    }

    //ע��event
    MID mid(g_strWdgApplication, PROC_WDG, MU_VNET_WDG);

    //pvnetlib->RegisterEvent(EV_VNETLIB_NOTIFY_VM_RR, mid);
    pvnetlib->RegisterEvent(EV_VNETLIB_NOTIFY_VM_RR_ACK, mid);
    pvnetlib->RegisterEvent(EV_VNETLIB_WDG_OPER, mid);
    //pvnetlib->RegisterEvent(EV_VNETLIB_WDG_OPER_ACK, mid);
    pvnetlib->RegisterEvent(EV_VNETLIB_WDG_TIMER_OPER, mid);
    //pvnetlib->RegisterEvent(EV_VNETLIB_WDG_TIMER_OPER_ACK, mid);

    return 0;
}
#endif
} //end namespace

int main(int argc, char** argv)
{
#if 1
    STATUS ret;
    TecsConfig *config = TecsConfig::Get();
    config->EnableSky();
    //HC��sky��ʱ������Ҫ��ϸߣ���Ҫ�ﵽ20����
    config->SetTimerScanValue(20);

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
    config->SetProcess(PROC_WDG);

    string  strWdgUUID;
    if (!GetWDGUUID(strWdgUUID))
    {
        string strWdgApp = "wdg_";
        strWdgApp += strWdgUUID;        
        config->SetApplication(strWdgApp);

        g_strWdgApplication = strWdgApp;
		
        printf("wdg.cpp: Set WDG Application(%s)\n", strWdgApp.c_str());
    }
    else
    {
        printf("wdg.cpp: GetWDGUUID failed\n");
        SkyExit(-1,"main(wdg.cpp): call GetWDGUUID failed.");
    }

#if 0   
    char host_name[256] = {0};
    if (0 == gethostname(host_name, 256))
    {
        g_strHostName = host_name;
    }
    else
    {

        printf("wdg.cpp: Get host name(%s)\n", g_strHostName.c_str());
    }
#endif

    //��������
    ret = tecs_init();
    if(ret != SUCCESS)
    {
        printf("wdg.cpp: tecs init failed! ret = %d\n",ret);
        SkyExit(-1,"main(wdg.cpp): call tecs_init failed.");
    }

    // ��������shell
    Shell("wdg_shell-> ");

    // ��ʼ�����̵���������
    if(0 != wdg_main())
    {
        printf("wdg_main failed!\n");
        SkyExit(-1, "main: call wdg_main failed.");
    }

    //�ȴ�SIGINT��SIGTERM�źŵ������˳�
    wait_signal_exit();

#endif
    return 0;
}

