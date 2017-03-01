/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：dev_mgr.cpp
* 文件标识：
* 内容摘要：dev相关类的定义文件
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
#include "vna_common.h"
#include "dev_monitor.h"
#include "dev_mgr.h"

namespace zte_tecs
{


void CNetDevMgr::DoMonitor(CNetDevVisitor *dev_visit)
{
};

void CNetDevMgr::DoMonitor(CPortVisitor *dev_visit)
{
};

void CNetDevMgr::DoMonitor(CBridgeVisitor *dev_visit)
{
};

void CNetDevMgr::DoMonitor(CVnicVisitor *dev_visit)
{
};

void CPortManager::GetReportInfo()
{
};

void CPortManager::DoMonitor(CNetDevVisitor *dev_visit)
{
    map<string, CPortNetDev*>::iterator port_dev = m_mapPortDev.begin();
    
    for(; port_dev!=m_mapPortDev.end(); port_dev++)
    {
        port_dev->second->Accept(dev_visit);
    }
};

void CPortManager::DoMonitor(CPortVisitor *dev_visit)
{
    map<string, CPortNetDev*>::iterator port_dev = m_mapPortDev.begin();

    for(; port_dev!=m_mapPortDev.end(); port_dev++)
    {
        port_dev->second->Accept(dev_visit);
    }
};

#if 0
void CBridgeManager::GetReportInfo()
{
};

void CBridgeManager::DoMonitor(CNetDevVisitor *dev_visit)
{
    map<string, CDVSNetDev*>::iterator bridge_dev = m_mapBridgeDev.begin();

    for(; bridge_dev!=m_mapBridgeDev.end(); bridge_dev++)
    {
        bridge_dev->second->Accept(dev_visit);
    }
};

void CBridgeManager::DoMonitor(CBridgeVisitor* dev_visit)
{    
    map<string, CDVSNetDev*>::iterator bridge_dev = m_mapBridgeDev.begin();

    for(; bridge_dev!=m_mapBridgeDev.end(); bridge_dev++)
    {
        bridge_dev->second->Accept(dev_visit);
    }
};

#endif


}

