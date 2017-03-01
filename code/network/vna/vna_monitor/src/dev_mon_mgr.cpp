/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：dev_mon_mgr.cpp
* 文件标识：
* 内容摘要：监控管理类的定义文件
* 当前版本：1.0
* 作    者：
* 完成日期：2013年1月16日
*
* 修改记录1：
*     修改日期：2013/1/16
*     版 本 号：V1.0
*     修 改 人：lverchun
*     修改内容：创建
******************************************************************************/
#include <typeinfo>
#include "vna_common.h"
#include "dev_monitor.h"
#include "dev_mgr.h"
#include "dev_mon_reg_func.h"
#include "dev_mon_mgr.h"
#include "vnet_portagent.h"
#include "vna_switch_module.h"
#include "vna_switch_agent.h"
#include "vna_agent.h"

namespace zte_tecs
{
extern string g_strVNMApplication;


/************************************************************
* 函数名称： CDevMonTask
* 功能描述： CDevMonTask构造函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       lvech         创建
***************************************************************/
CDevMonTask::CDevMonTask(CNetDevMgr *pDevMgr, int32 nMonInterval)
{
    m_pDevMgr = pDevMgr;
    m_nMonitorInterval = nMonInterval;
    time_t cur_time = time(0);
    m_nLastMonTime = cur_time - nMonInterval;
}

/************************************************************
* 函数名称： ~CDevMonTask
* 功能描述： CDevMonTask析构函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       lvech         创建
***************************************************************/
CDevMonTask::~CDevMonTask()
{
    m_pDevMgr = NULL;
    m_nMonitorInterval = 0;
    m_nMonitorInterval= 0;
}

/************************************************************
* 函数名称： DbgShowData
* 功能描述： 打印调试信息
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       lvech         创建
***************************************************************/
void CDevMonTask::DbgShowData()
{
    if (NULL != m_pDevMgr)
    {
        cout << "Monitor Task Info:"<<endl;
        cout << "    Mgr Info:" << m_pDevMgr->GetMgrInfo() << endl;
        cout << "    Monitor Interval:" <<m_nMonitorInterval << endl;
        cout << "    Last Monitor Time:" <<m_nLastMonTime << endl;
    }
};

/************************************************************
* 函数名称： CDevMonMgr
* 功能描述： CDevMonMgr构造函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       lvech         创建
***************************************************************/
CDevMonMgr::CDevMonMgr()
{
    m_pMU = NULL;
    m_nTimerID = INVALID_TIMER_ID;
    m_bOpenDbgInf = VNET_FALSE;
}

/************************************************************
* 函数名称： ~CDevMonMgr
* 功能描述： CDevMonMgr析构函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       lvech         创建
***************************************************************/
CDevMonMgr::~CDevMonMgr()
{
    map<CDevMonTask*, CNetDevVisitor*>::iterator net_it = m_vecDevMonTask.begin();
    for (; net_it !=m_vecDevMonTask.end(); net_it++)
    {
        delete net_it->first;
    }

    map<CDevMonTask*, CBridgeVisitor*>::iterator bridge_it = m_vecBridgeMonTask.begin();
    for (; bridge_it !=m_vecBridgeMonTask.end(); bridge_it++)
    {
        delete net_it->first;
    }

    map<CDevMonTask *, CPortVisitor*>::iterator port_it = m_vecPortMonTask.begin();
    for (; port_it !=m_vecPortMonTask.end(); port_it++)
    {
        delete net_it->first;
    }
}

/************************************************************
* 函数名称： Init
* 功能描述： 模块初始化函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       lvech         创建
***************************************************************/
STATUS CDevMonMgr::Init(MessageUnit  *mu)
{
    m_pMU = mu;

    //创建监控定时器
    m_nTimerID = m_pMU->CreateTimer();
    if (INVALID_TIMER_ID == m_nTimerID)
    {
        VNET_LOG(VNET_LOG_ERROR, 
               "CDevMonMgr::Init() Create timer failed for CDevMonMgr!\n");
        SkyAssert(0);
    }
    
    TimeOut timeout;
    timeout.type = LOOP_TIMER;
    timeout.duration = 1 * 1000;
    timeout.msgid = TIMER_VNA_DEV_MON;
    m_pMU->SetTimer(m_nTimerID, timeout);    

    return SUCCESS;
};
    
/************************************************************
* 函数名称： MessageEntry
* 功能描述： 消息主入口函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       lvech         创建
***************************************************************/
void CDevMonMgr::MessageEntry(const MessageFrame& message)
{
    switch(message.option.id())
    {
        case TIMER_VNA_DEV_MON:
        {
            DoMonitorTask();
            break;
        }
        
        case EV_VNET_VNA_REPORT_INFO:
        {
            SendVNAReport(message);
            break;
        }

        default:
            break;
    }
};

/************************************************************
* 函数名称： AddMonitorTask
* 功能描述： 添加监控任务函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       lvech         创建
***************************************************************/
void CDevMonMgr::AddMonitorTask(CNetDevMgr * pDevMgr, CNetDevVisitor *cDevVisitor, int32 nMonInterval)
{
    CDevMonTask *pDevMonTask = new CDevMonTask(pDevMgr, nMonInterval);
    if (NULL == pDevMonTask)
    {
        SkyAssert(0);
        return;
    }
    
    m_vecDevMonTask.insert(pair<CDevMonTask * , CNetDevVisitor*> (pDevMonTask, cDevVisitor));
};

/************************************************************
* 函数名称： AddMonitorTask
* 功能描述： 添加监控任务函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       lvech         创建
***************************************************************/
void CDevMonMgr::AddMonitorTask(CNetDevMgr * pDevMgr, CBridgeVisitor *cBridgeVisitor, int32 nMonInterval)
{
    CDevMonTask *pDevMonTask = new CDevMonTask(pDevMgr, nMonInterval);
    if (NULL == pDevMonTask)
    {
        SkyAssert(0);
        return;
    }
    
    m_vecBridgeMonTask.insert(pair<CDevMonTask * , CBridgeVisitor*> (pDevMonTask, cBridgeVisitor));
};

/************************************************************
* 函数名称： AddMonitorTask
* 功能描述： 添加监控任务函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       lvech         创建
***************************************************************/
void CDevMonMgr::AddMonitorTask(CNetDevMgr * pDevMgr, CPortVisitor *cPortVisitor, int32 nMonInterval)
{
    CDevMonTask *pDevMonTask = new CDevMonTask(pDevMgr, nMonInterval);
    if (NULL == pDevMonTask)
    {
        SkyAssert(0);
        return;
    }
    
    m_vecPortMonTask.insert(pair<CDevMonTask * , CPortVisitor*> (pDevMonTask, cPortVisitor));
};

/************************************************************
* 函数名称： AddMonitorTask
* 功能描述： 添加监控任务函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       lvech         创建
***************************************************************/
void CDevMonMgr::AddMonitorTask(CNetDevMgr * pDevMgr, CVnicVisitor *cVnicVisitor, int32 nMonInterval)
{
    CDevMonTask *pDevMonTask = new CDevMonTask(pDevMgr, nMonInterval);
    if (NULL == pDevMonTask)
    {
        SkyAssert(0);
        return;
    }
    
    m_vecVnictMonTask.insert(pair<CDevMonTask * , CVnicVisitor*> (pDevMonTask, cVnicVisitor));
};

/************************************************************
* 函数名称： DoMonitorTask
* 功能描述： 执行监控任务函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       lvech         创建
***************************************************************/
void CDevMonMgr::DoMonitorTask()
{
    time_t cur_time = time(0);
    map<CDevMonTask*, CNetDevVisitor *>::iterator net_it = m_vecDevMonTask.begin();
    for (; net_it !=m_vecDevMonTask.end(); net_it++)
    {        
        if ((cur_time -net_it->first->GetLastMon())  >  net_it->first->GetMonInterval())
        {
            net_it->first->GetDevMgr()->DoMonitor( net_it->second);
            net_it->first->SetLastMon(cur_time);
        }
    }

    map<CDevMonTask*, CBridgeVisitor *>::iterator bridge_it = m_vecBridgeMonTask.begin();
    for (; bridge_it !=m_vecBridgeMonTask.end(); bridge_it++)
    {
        if ((cur_time -bridge_it->first->GetLastMon())  >  bridge_it->first->GetMonInterval())
        {
            bridge_it->first->GetDevMgr()->DoMonitor( bridge_it->second);
            bridge_it->first->SetLastMon(cur_time);
        }
    }

    map<CDevMonTask *, CPortVisitor *>::iterator port_it = m_vecPortMonTask.begin();
    for (; port_it !=m_vecPortMonTask.end(); port_it++)
    {
        if ((cur_time -port_it->first->GetLastMon())  >  port_it->first->GetMonInterval())
        {
            port_it->first->GetDevMgr()->DoMonitor( port_it->second);
            port_it->first->SetLastMon(cur_time);
        }
    }

    map<CDevMonTask *, CVnicVisitor *>::iterator vnic_it = m_vecVnictMonTask.begin();
    for (; vnic_it !=m_vecVnictMonTask.end(); vnic_it++)
    {
        if ((cur_time -vnic_it->first->GetLastMon())  >  vnic_it->first->GetMonInterval())
        {
            vnic_it->first->GetDevMgr()->DoMonitor( vnic_it->second);
            vnic_it->first->SetLastMon(cur_time);
        }
    }
};

/************************************************************
* 函数名称： SendVNAReport
* 功能描述： 向VNM上报信息
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1         V1.0       lvech         创建
***************************************************************/
void CDevMonMgr::SendVNAReport(const MessageFrame &message)
{
    CVNAReportMsg msg;
    msg.deserialize(message.data);

    // 补充物理端口, TRUNK口信息
    CPortAgent *pPortHandle = CPortAgent::GetInstance();
    if (pPortHandle)
    {
        pPortHandle->GetPortReportInfo(msg.m_cPortInfo);
    }

    // 补充网桥信息
    CSwitchAgent *pSwitchHandle = CSwitchAgent::GetInstance();
    if (pSwitchHandle)
    {
        pSwitchHandle->DoSwitchReport(msg.m_vecSwitchInfo);
    }
    
    // 发送消息
    MID receiver;
    receiver._application = g_strVNMApplication;
    receiver._process = PROC_VNM;
    receiver._unit = MU_VNM;
    MessageOption option(receiver, EV_VNA_INFO_REPORT, 0, 0);
    m_pMU->Send(msg, option);

    if (m_bOpenDbgInf)
    {
        cout << "CDevMonMgr::Send CVNAReportMsg message:\n" << endl;  
        msg.Print();
    }
}

/************************************************************
* 函数名称： DbgShowData
* 功能描述： 调试信息打印
* 访问的表： 无
* 修改的表： 无
* 输入参数： 
* 输出参数： 
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1         V1.0       lvech         创建
***************************************************************/
void CDevMonMgr::DbgShowData()
{
    cout <<endl;    
    cout << "++++++++++++++++++++++VNA Monitor Information BEGIN ++++++++++++++++++++++++" << endl;

    map<CDevMonTask *, CNetDevVisitor *> m_vecDevMonTask;
    map<CDevMonTask *, CBridgeVisitor *> m_vecBridgeMonTask;
    map<CDevMonTask *, CPortVisitor *> m_vecPortMonTask;

    cout << "Network Dev Monitor:" << endl;
    map<CDevMonTask *, CNetDevVisitor *>::iterator iter = m_vecDevMonTask.begin();
    for ( ; iter != m_vecDevMonTask.end(); ++iter)
    {
        iter->first->DbgShowData();
        cout << "   Visitor Info" << iter->second->GetVisitorInfo() <<endl;
    }

    cout << "Bridge Dev Monitor:" << endl;

    map<CDevMonTask *, CBridgeVisitor *>::iterator iterBr = m_vecBridgeMonTask.begin();
    for ( ; iterBr != m_vecBridgeMonTask.end(); ++iterBr)
    {
        iterBr->first->DbgShowData();
        cout << "   Visitor Info" << iterBr->second->GetVisitorInfo() <<endl;
    }

    cout << "Port Dev Monitor:" << endl;

    map<CDevMonTask *, CPortVisitor *>::iterator iterPort = m_vecPortMonTask.begin();
    for ( ; iterPort != m_vecPortMonTask.end(); ++iterPort)
    {
        iterPort->first->DbgShowData();
        cout << "   Visitor Info" << iterPort->second->GetVisitorInfo() <<endl;
    }

    cout << "Vnic Dev Monitor:" << endl;

    map<CDevMonTask *, CVnicVisitor *>::iterator iterVnic = m_vecVnictMonTask.begin();
    for ( ; iterVnic != m_vecVnictMonTask.end(); ++iterVnic)
    {
        iterVnic->first->DbgShowData();
        cout << "   Visitor Info" << iterVnic->second->GetVisitorInfo() <<endl;
    }
    
    cout << "++++++++++++++++++++++VNA Monitor Information END ++++++++++++++++++++++++" << endl;
};

/************************************************************
* 函数名称： SetDbgFlag
* 功能描述： 调试信息开关
* 访问的表： 无
* 修改的表： 无
* 输入参数： 
* 输出参数： 
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1         V1.0       lvech         创建
***************************************************************/
void CDevMonMgr::SetDbgFlag(int bDbgFlag)
{

    m_bOpenDbgInf = bDbgFlag;
};

int VNET_DBG_SHOW_VNA_MONITOR_INFO(void)
{
    CDevMonMgr *pVNetHandle = CDevMonMgr::GetInstance();
    if (pVNetHandle)
    {
        pVNetHandle->DbgShowData();
        cout << endl;
    }
    
    return 0;
}
DEFINE_DEBUG_FUNC(VNET_DBG_SHOW_VNA_MONITOR_INFO);
}


