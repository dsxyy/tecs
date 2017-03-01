/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vnet_phyport_manager.h
* 文件标识：
* 内容摘要：
* 当前版本：1.0
* 作    者：liuhx
* 完成日期：2013年2月20日
*******************************************************************************/
#ifndef VNET_PHYPORT_MANAGER_H
#define VNET_PHYPORT_MANAGER_H
#include "vnet_phyport_class.h"
#include "vnet_portinfo.h"

using namespace _GLIBCXX_STD;

namespace zte_tecs
{

class CPhyPortManager:public CNetDevMgr
{
    /*******************************************************************************
    * 1. public section
    *******************************************************************************/

public:
    static CPhyPortManager *GetInstance();

    virtual ~CPhyPortManager();

    int32 GetPhyPortInfo(vector <CPhyPortReport> &phyportInfo);

    void GetPortWorkPara();

    void DbgShow();

    void LookupPhyPort();

    int32 SetPortLoopback(const string &strPortName, const uint32 &dwFlag);

    int32 SetPortMtu(const string &strPortName, const uint32 &dwMtu);

    void DoMonitor(CNetDevVisitor *dev_visit);
    void DoMonitor(CPortVisitor *dev_visit);

    int32 GetKernelPortInfo(CKernelPortReport &kernelportinfo);
    int32 GetKernelPortCfgInfo(vector<CKernelPortCfgReport> &vKernelCfgInfo);

    int32 IsPciAssignable(const string &strPci);

    int32 IsPciExit(const string &strPci, string &strPhyName, string &strVf);

    int32 SetVFPortMac(const string &strPci, const string &strMac);
    int32 SetVFPortVlan(const string &strPci, const int32 &nVlan);
    int32 GetVFPortMac(const string &strPci, string &strMac);
    int32 GetVFPortVlan(const string &strPci, int32 &nVlan);

    int32 GetPortMapInfo(map<string, int32> &mapPortInfo);

    int32 GetPortIpMask(const string &strPortName, string &strIp, string &strMask);	

    int32 GetVirPortIpMask(const string &strPortName, string &strIp, string &strMask);
	
    int32 CreateVtepPort(const string &strVtepName, const string &strSwitchName,  
                             const string &strUplinkPort, const string &strIp, const string &strMask, int32 nVlanId);
    int32 DeleteVtepPort(const string &strSwitchName);
    int32 GetVtepPortCfgInfo(vector<CVtepPortCfgReport> &vVtepCfgInfo);


    /*******************************************************************************
    * 2. protected section
    *******************************************************************************/
protected:

    /*******************************************************************************
    * 3. private section
    *******************************************************************************/
private:

    map<string, CPhyPortClass*> m_mapPorts;
    static CPhyPortManager *m_instance;
    Mutex m_mutex;

    CPhyPortManager()
    {
        m_mutex.Init();
    }

    DISALLOW_COPY_AND_ASSIGN(CPhyPortManager);
};

} /* end namespace zte_tecs */

#endif // VNET_PHYPORT_MANAGER_H
