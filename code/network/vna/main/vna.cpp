/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�vna.cpp
* �ļ���ʶ��
* ����ժҪ��vna���̵�main����ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�lvech
* ������ڣ�2012��12��14��
*
* �޸ļ�¼1��
*     �޸����ڣ�2012/12/14
*     �� �� �ţ�V1.0
*     �� �� �ˣ�lvech
*     �޸����ݣ�����
******************************************************************************/
#include "tecs_config.h"
#include "registered_system.h"
#include "proc_admin.h"
#include "vnet_comm.h"
#include "vna_common.h"
#include "vna_pub.h"

using namespace zte_tecs;
namespace zte_tecs
{

extern bool enable_evb; 
extern string g_strHostName;
extern string g_strVNAApplication;
int32 g_nHypeVisorType = HYPE_VISOR_TYPE_XEN;

int vna_main()
{
    STATUS ret;

    AddKeyMu(LOG_AGENT);
    AddKeyMu(MU_PROC_ADMIN);
    AddKeyMu(MU_VNA_CONTROLLER);
    AddKeyMu(MU_VNA_AGENT);
    AddKeyMu(MU_VNA_HEARTBEAT);
    AddKeyMu(MU_VNET_MONITOR_MGR);
        
    //ִ��Log ��־��¼��ʼ��,1��ʾ����ΪHC,��������HC�Ľ��̶������ṩ�������
    ret = LogInit();
    if(ret != SUCCESS)
    {
        SkyAssert(0);
        SkyExit(-1, "vna_main: call LogInit failed.");
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

    CVNetHeartbeat *pCVNetHeart = CVNetHeartbeat::GetInstance();
    if(!pCVNetHeart)
    {
        SkyAssert(0);
        SkyExit(-1,"CVNetHeartbeat::GetInstance failed!\n");
    }
    ret = pCVNetHeart->Init();
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"CVNetHeartbeat Init failed!\n");
    }

    //��ʼ��CVNetAgent������
    CVNetAgent *pVnetAgentInst =  CVNetAgent::GetInstance();
    if (NULL == pVnetAgentInst)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetAgent::GetInstance() failed.\n");
        return -1;
    }
    
    if (pVnetAgentInst->Init())
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetAgent Init()failed.\n");
        return -1;
    }

    return 0;
}
} //end namespace

int main(int argc, char** argv)
{
    STATUS ret;
    TecsConfig *config = TecsConfig::Get();
    config->EnableSky();
    //HC��sky��ʱ������Ҫ��ϸߣ���Ҫ�ﵽ20����
    config->SetTimerScanValue(20);

    // ʹ����־��ӡ�������ò���
    EnableLogOptions();
    config->EnableCustom("enable_evb", enable_evb, "The EVB(Edge Virtual Bridging) module configuration switch, default = no(bool switch). ");

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
    config->SetProcess(PROC_VNA);
    
    string  strUUID;
    if (!GetVNAUUIDByVNA(strUUID))
    {
        config->SetApplication(strUUID);
        g_strVNAApplication = strUUID;
        printf("vna.cpp: Set VNA Application(%s)\n", strUUID.c_str());
    }
    else
    {
        printf("vna.cpp: GetVNAUUIDByVNA failed\n");
        SkyExit(-1,"main(vna.cpp): call GetVNAUUIDByVNA failed.");
    }

    char host_name[256] = {0};
    if (0 == gethostname(host_name, 256))
    {
        g_strHostName = host_name;
    }
    else
    {

        printf("vna.cpp: Get host name(%s)\n", g_strHostName.c_str());
    }
        
    g_nHypeVisorType = GetHypeVisorType();
        
    //��������
    ret = tecs_init();
    if(ret != SUCCESS)
    {
        printf("vna.cpp: tecs init failed! ret = %d\n",ret);
        SkyExit(-1,"main(vna.cpp): call tecs_init failed.");
    }

    // ��������shell
    Shell("vna_shell-> ");

    // ��ʼ�����̵���������
    if(0 != vna_main())
    {
        printf("vna_main failed!\n");
        SkyExit(-1, "main: call vna_main failed.");
    }

    //�ȴ�SIGINT��SIGTERM�źŵ������˳�
    wait_signal_exit();
    return 0;
}

