/******************************************************************************
* Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�license_manager.cpp
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��֤�������ģ��
* ��ǰ�汾��1.0
* ��    �ߣ����Ľ�
* ������ڣ�2012��7��25��
*
* �޸ļ�¼1��
*     �޸����ڣ�2012/7/25
*     �� �� �ţ�V1.0
*     �� �� �ˣ����Ľ�
*     �޸����ݣ�����
******************************************************************************/
#include "license_manager.h"
#include "license_common.h"
#include "log_agent.h"
#include "db_config.h"
#include "pool_sql.h"
#include "vm_messages.h"
#include "mid.h"
#include "event.h"

static int license_manager_debug_print = 0;  
DEFINE_DEBUG_VAR(license_manager_debug_print);
#define Debug(fmt,arg...) \
        do \
        { \
            if(license_manager_debug_print) \
            { \
                printf("[%s:%d]"fmt,__func__,__LINE__, ##arg); \
            } \
        }while(0)

#if 0
void RunningStatCheck()
{
    RunningStat stat;
    ostringstream error;
    if(GetRunningStat(stat) != SUCCESS)
    {
        OutPut(SYS_EMERGENCY,"failed to get running stat!\n");
        SkyExit(-1,"failed to get running stat!\n");
    }

    Debug("RunningStat created_vms = %d\n",stat.created_vms);
    Debug("RunningStat deployed_vms = %d\n",stat.deployed_vms);
    Debug("RunningStat users = %d\n",stat.users);
    Debug("RunningStat clusters = %d\n",stat.clusters);
    Debug("RunningStat images = %d\n",stat.images);
    Debug("RunningStat hosts = %d\n",stat.hosts);
    
    if(!license_valid(stat,error))
    {
        StopAllVM();
        OutPut(SYS_EMERGENCY,error.str().c_str());
        SkyExit(-1,error.str().c_str());
    }
}
#endif

extern "C" void *LicenseTaskEntry(void *arg)
{
    ostringstream error;
    while(1)
    {
        //ÿ���Ӽ��һ��license�Ƿ����˻�Ӳ���仯�ˣ���Ч���˳�
        int ret = license_valid(error);
        if(LICENSE_EXPIRED == ret || LICENSE_UNMATCHED == ret)
        {
            StopAllVM();
            OutPut(SYS_EMERGENCY,error.str().c_str());
            //SkyExit(-1,error.str().c_str());
        }

        //RunningStatCheck();
        sleep(60);
    }
    return NULL;
}

STATUS LicenseManagerInit()
{
    ostringstream error;
    int ret = license_init(NULL,NULL,error);
    if(LICENSE_OK != ret)
    {
        OutPut(SYS_EMERGENCY,"license_init failed!\n");
        SkyExit(-1,error.str().c_str());
        return ERROR;
    }
    
    THREAD_ID tid = StartThread("LicenseManager",LicenseTaskEntry,NULL);
    if(INVALID_THREAD_ID == tid)
    {
        OutPut(SYS_EMERGENCY,"failed to start LicenseManager task!\n");
        return ERROR;
    }
    return SUCCESS;
}


