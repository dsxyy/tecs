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
#include "alarm_agent.h"
#include "snmp_job.h"
#include "request_manager.h"
#include "alarm_manager.h"
#include "authmanager.h"
#include "log_agent.h"
#include "operator_log.h"
#include "license_manager.h"
#include "sessionmanager.h"
#include "dbschema_ver.h"
#include "current_alarm_pool.h"
#include "history_alarm_pool.h"
#include "snmp_database_pool.h"
#include "vmcfg_pool.h"
#include "vmcfg_operate.h"
#include "project_pool.h"
#include "project_operate.h"
#include "vmtemplate_pool.h"
#include "vmtemplate_operate.h"
#include "workflow.h"
#include "vnet_libnet.h"
#include "affinity_region_pool.h"
#include "affinity_region_operate.h"


using namespace zte_tecs;
namespace zte_tecs
{
// XMLRPC SERVER������ò���
static int XmlrpcPort = 8080 ;
static string XmlrpcLog = "" ;
extern string snmp_xml_file;

int apisvr_main()
{
    STATUS ret = ERROR;

    AddKeyMu(LOG_AGENT);
    AddKeyMu(ALARM_AGENT);
    AddKeyMu(ALARM_MANAGER);
    AddKeyMu(OP_LOG_AGENT);
    AddKeyMu(DRIVERMANAGER);
    AddKeyMu(SNMP_AGENT);
    
    //����ϵͳ��־
    ret = LogInit();
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"LogInit Init failed!");
    }

    //����������־
    ret = OpLogInit(GetDB());
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"OpLogInit Init failed!");
    }

    //��������������
    ret = StartWorkflowEngine(GetPrivateDB(),3);
    if(ret != SUCCESS)
    {
        SkyAssert(0);
        SkyExit(-1, "StartWorkflowEngine failed!");
    }
    
    ret = LicenseManagerInit();
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"LicenseManagerInit Init failed!");
    }

    // -------------------------------------------------------------------------
    // ���ݱ�ĳ�ʼ��
    CurrentAlarmPool *pcap = CurrentAlarmPool::CreateInstance(GetDB());
    if(!pcap)
    {
        SkyAssert(0);
        SkyExit(-1,"CurrentAlarmPool::CreateInstance failed!");
    }
    ret = pcap->Init(GetDB());
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"CurrentAlarmPool Init failed!");
    }

    HistoryAlarmPool *phap = HistoryAlarmPool::CreateInstance(GetDB());
    if(!phap)
    {
        SkyAssert(0);
        SkyExit(-1,"HistoryAlarmPool::CreateInstance failed!");
    }
    ret = phap->Init();
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"HistoryAlarmPool Init failed!");
    }

    //SnmpConfigurationPool::Bootstrap(GetDB());
    SnmpConfigurationPool *pscp = SnmpConfigurationPool::CreateInstance(GetDB());
    if(!pscp)
    {
        SkyAssert(0);
        SkyExit(-1,"SnmpConfigurationPool::CreateInstance failed!");
    }

    //TrapAddressPool::Bootstrap(GetDB());
    TrapAddressPool *ptap = TrapAddressPool::CreateInstance(GetDB());
    if(!ptap)
    {
        SkyAssert(0);
        SkyExit(-1,"TrapAddressPool::CreateInstance failed!");
    }

    //SnmpXmlRpcUserPool::Bootstrap(GetDB());
    SnmpXmlRpcUserPool *psxrp = SnmpXmlRpcUserPool::CreateInstance(GetDB());
    if(!psxrp)
    {
        SkyAssert(0);
        SkyExit(-1,"SnmpXmlRpcUserPool::CreateInstance failed!");
    }

    UsergroupPool *pUsergroupPool = UsergroupPool::CreateInstance(GetDB());
    if(!pUsergroupPool)
    {
        SkyAssert(0);
        SkyExit(-1,"UsergroupPool::CreateInstance failed!");        
    }
    ret = pUsergroupPool->Init();
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"UsergroupPool Init failed!");
    }

    UserPool *pUserPool = UserPool::CreateInstance(GetDB());
    if(!pUserPool)
    {
        SkyAssert(0);
        SkyExit(-1,"UserPool::CreateInstance failed!");        
    }    
    ret = pUserPool->Init();
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"UserPool Init failed!");
    }
    
    SessionManager* pSess = SessionManager::CreateInstance();
    if(!pSess)
    {
        SkyAssert(0);
        SkyExit(-1,"SessionManager::CreateInstance failed!");        
    }

    // vmcfg_pool ������� RequestManagerʵ������֮ǰ
    VmCfgPool *pvp = VmCfgPool::CreateInstance(GetDB());
    if (!pvp)
    {
        SkyAssert(0);
        SkyExit(-1,"VmCfgPool CreateInstance failed!");
    }
    
    ret = pvp->Init(true);
    if(ret != SUCCESS)
    {
        SkyAssert(0);
        SkyExit(-1,"VmCfgPool Init failed!");
    }

    VmCfgOperate *vc_op_inst = VmCfgOperate::CreateInstance();
    if (!vc_op_inst)
    {
        SkyAssert(0);
        SkyExit(-1,"VmCfgOperate CreateInstance failed!");
    }    

    // project_pool ������� RequestManagerʵ������֮ǰ
    ProjectPool *ppp = ProjectPool::CreateInstance(GetDB());
    if (!ppp)
    {
        SkyAssert(0);
        SkyExit(-1,"ProjectPool CreateInstance failed!");
    }
    
    ret = ppp->Init(true);
    if(ret != SUCCESS)
    {
        SkyAssert(0);
        SkyExit(-1,"ProjectPool Init failed!");
    }

    ProjectOperate *prj_op_inst = ProjectOperate::CreateInstance();
    if (!prj_op_inst)
    {
        SkyAssert(0);
        SkyExit(-1,"ProjectOperate CreateInstance failed!");
    }
    
    // AffinityRegionPool ������� RequestManagerʵ������֮ǰ
    AffinityRegionPool *parp = AffinityRegionPool::CreateInstance(GetDB());
    if (!parp)
    {
        SkyAssert(0);
        SkyExit(-1,"AffinityRegionPool CreateInstance failed!");
    }

    ret = parp->Init(true);
    if(ret != SUCCESS)
    {
        SkyAssert(0);
        SkyExit(-1,"AffinityRegionPool Init failed!");
    }

    AffinityRegionOperate *ar_op_inst = AffinityRegionOperate::CreateInstance();
    if (!ar_op_inst)
    {
        SkyAssert(0);
        SkyExit(-1,"AffinityRegionOperate CreateInstance failed!");
    }

    // vmtemplate_pool ������� RequestManagerʵ������֮ǰ
    VmTemplatePool *pvtp = VmTemplatePool::CreateInstance(GetDB());
    if (!pvp)
    {
        SkyAssert(0);
        SkyExit(-1,"VmTemplatePool CreateInstance failed!");
    }
    
    ret = pvtp->Init();
    if(ret != SUCCESS)
    {
        SkyAssert(0);
        SkyExit(-1,"VmTemplatePool Init failed!");
    }
    
    VmTemplateOperate *vt_op_inst = VmTemplateOperate::CreateInstance();
    if (!vt_op_inst)
    {
        SkyAssert(0);
        SkyExit(-1,"VmTemplateOperate CreateInstance failed!");
    }
    
    // -------------------------------------------------------------------------
    // AuthManager���ؼ�Ȩ����
    AuthManager *pam = AuthManager::GetInstance();
    if(!pam)
    {
        SkyAssert(0);
        SkyExit(-1,"AuthManager::GetInstance() failed!");        
    }
    ret = pam->LoadMads(pUserPool);
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"AuthManager LoadMads failed!");
    }

    // -------------------------------------------------------------------------
    // RequestManager����
    RequestManager *rm = RequestManager::CreateInstance(XmlrpcPort, XmlrpcLog);
    if(!rm)
    {
        SkyAssert(0);
        SkyExit(-1,"RequestManager::CreateInstance failed!");
    }
    ret = rm->Init();
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"RequestManager Start failed!");
    }

    // AlarmAgent����
    AlarmAgent *paa = AlarmAgent::GetInstance();
    if (!paa)
    {
        SkyAssert(0);
        SkyExit(-1,"Create AlarmAgent failed!");
    }
    ret = paa->Init();
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"AlarmAgent Init failed!");
    }

    // AlarmManager����
    AlarmManager *am = AlarmManager::GetInstance();
    if (NULL == am)
    {
        SkyAssert(0);
        SkyExit(-1,"AlarmManager start failed!");
    }
    ret = am->Init();
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"AlarmManager Init failed!");
    }

    SnmpJob *psj = SnmpJob::GetInstance();
    if (NULL == psj)
    {
        SkyAssert(0);
        SkyExit(-1,"SnmpJob GetInstance failed!");
    }
    ret = psj->Init();
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"SnmpJob Init failed!");
    }

    CVNetLibNet *pvnetlib = CVNetLibNet::CreateInstance();
    if(pvnetlib == NULL)
    {
        SkyAssert(0);
        SkyExit(-1,"CVNetLibNet::CreateInstance!");
    }

    string app=ApplicationName();
    if(SUCCESS != pvnetlib->Init(app,VNA_MODULE_ROLE_APISVR))
    {
        SkyAssert(0);
        SkyExit(-1,"pvnetlib Init failed!");
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
    config->EnableMs(MS_SERVER_API);

    // XMLRPC Server���ò���
    config->EnableCustom("xmlrpc_port", XmlrpcPort,
                         "XMLRPC Server port number, default port is 8080.");
    config->EnableCustom("xmlrpc_log", XmlrpcLog,
                         "XMLRPC Server log filename, leave empty means close log output.");

    // snmp xml fileλ�����ò���
    config->EnableCustom("snmp_xml_file", snmp_xml_file,
                         "Snmp_xml_file file path, default is /opt/tecs/tc/etc/snmp.xml.");

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
    config->SetProcess(PROC_APISVR);

    //����tecs����
    ret = tecs_init(apisvr_main);
    if (ret != SUCCESS)
    {
        printf("tecs api service init failed! ret = %d\n",ret);
        SkyExit(-1, "main: call tecs_init(apisvr_main) failed!");
    }

    // ��������shell
    Shell("apisvr_shell-> ");
    //�ȴ�SIGINT��SIGTERM�źŵ������˳�
    wait_signal_exit();
    return 0;
}

