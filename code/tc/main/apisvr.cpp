/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：tc.cpp
* 文件标识：
* 内容摘要：tc进程的main函数实现文件
* 当前版本：1.0
* 作    者：袁进坤
* 完成日期：2011年8月19日
*
* 修改记录1：
*     修改日期：2011/8/19
*     版 本 号：V1.0
*     修 改 人：袁进坤
*     修改内容：创建
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
// XMLRPC SERVER相关配置参数
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
    
    //启动系统日志
    ret = LogInit();
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"LogInit Init failed!");
    }

    //启动操作日志
    ret = OpLogInit(GetDB());
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"OpLogInit Init failed!");
    }

    //启动工作流引擎
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
    // 数据表的初始化
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

    // vmcfg_pool 必须放在 RequestManager实例创建之前
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

    // project_pool 必须放在 RequestManager实例创建之前
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
    
    // AffinityRegionPool 必须放在 RequestManager实例创建之前
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

    // vmtemplate_pool 必须放在 RequestManager实例创建之前
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
    // AuthManager加载鉴权驱动
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
    // RequestManager启动
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

    // AlarmAgent启动
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

    // AlarmManager启动
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

    // XMLRPC Server配置参数
    config->EnableCustom("xmlrpc_port", XmlrpcPort,
                         "XMLRPC Server port number, default port is 8080.");
    config->EnableCustom("xmlrpc_log", XmlrpcLog,
                         "XMLRPC Server log filename, leave empty means close log output.");

    // snmp xml file位置配置参数
    config->EnableCustom("snmp_xml_file", snmp_xml_file,
                         "Snmp_xml_file file path, default is /opt/tecs/tc/etc/snmp.xml.");

    // 使能日志打印级别配置参数
    EnableLogOptions();

    ret = config->Parse(argc, argv);
    //解析命令行参数以及配置文件
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

    //设置进程名称
    config->SetProcess(PROC_APISVR);

    //启动tecs进程
    ret = tecs_init(apisvr_main);
    if (ret != SUCCESS)
    {
        printf("tecs api service init failed! ret = %d\n",ret);
        SkyExit(-1, "main: call tecs_init(apisvr_main) failed!");
    }

    // 启动调试shell
    Shell("apisvr_shell-> ");
    //等待SIGINT、SIGTERM信号到来后退出
    wait_signal_exit();
    return 0;
}

