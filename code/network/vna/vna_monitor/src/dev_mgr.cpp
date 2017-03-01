/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�dev_mgr.cpp
* �ļ���ʶ��
* ����ժҪ��dev�����Ķ����ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�
* ������ڣ�2013��1��16��
*
* �޸ļ�¼1��
*     �޸����ڣ�2013/1/16
*     �� �� �ţ�V1.0
*     �� �� �ˣ�lverchun
*     �޸����ݣ�����
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

