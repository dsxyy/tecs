/*******************************************************************************
 * Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
 *
 * 文件名称：vnet_phyport_class.cpp
 * 文件标识：
 * 内容摘要：物理端口类实现文件
 * 当前版本：V1.0
 * 作    者：liuhx
 * 完成日期：2013年2月5日
 *******************************************************************************/
#include <linux/sockios.h>
#include <linux/ethtool.h>
#include "vnet_comm.h"
#include "dev_monitor.h"
#include "dev_mgr.h"
#include "vnet_phyport_class.h"
#include "vnet_function.h"
#include "phy_port_pub.h"

namespace zte_tecs
{

CPhyPortClass::~CPhyPortClass()
{
    map<string,string>::iterator it = m_mapVirtfnPci.begin();
    for (;it != m_mapVirtfnPci.end();)
    {
        m_mapVirtfnPci.erase(it++);
    }
}

//获取每个物理端口的参数
void CPhyPortClass::GetPortWorkPara(int32 fd)
{
    int ret = 0;
    struct ifreq ifr;
    struct ethtool_cmd eth_cmd;

    //先填写nic name
    if (GetDevName().length() == 0)
    {
        //VNET_ASSERT(0);
        return;
    }

    strcpy(ifr.ifr_name, GetDevName().c_str());
    ret = ioctl(fd, SIOCGIFFLAGS, &ifr);
    if (0 == ret)
    {
        if (ifr.ifr_flags & IFF_UP)
        {
            m_nState  = PORT_STATUS_UP;//strOfStatus(PORT_STATUS_UP);
        }
        else
        {
            m_nState = PORT_STATUS_DOWN;//strOfStatus(PORT_STATUS_DOWN);
        }

        if (ifr.ifr_flags & IFF_PROMISC)
        {
            m_nPromiscuous = (PORT_PROMISC_YES);
        }
        else
        {
            m_nPromiscuous = (PORT_PROMISC_NO);
        }
    }
    else
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "SIOCGIFFLAGS:[%d] GetPortWorkPara failed.\n", ret);
    }

    ethtool_value eth_cmd_value;
    eth_cmd_value.cmd = ETHTOOL_GLINK;
    ifr.ifr_data = (caddr_t) &eth_cmd_value;
    ret = ioctl(fd, SIOCETHTOOL, &ifr);
    if (0 == ret)
    {
        m_nIsLinked = eth_cmd_value.data ? 1 : 0;
    }
    else
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "SIOCETHTOOL:[%d] GetPortWorkPara failed.\n", ret);
    }

    eth_cmd.cmd = ETHTOOL_GSET;
    ifr.ifr_data = (caddr_t) &eth_cmd;
    ret = ioctl(fd, SIOCETHTOOL, &ifr);
    if (0 == ret)
    {
        m_strSpeed = strOfSpeed(speedFromSys(eth_cmd.speed));
        m_strDuplex = strOfDuplex(duplexFromSys(eth_cmd.duplex));
        m_strAutoNegotiate = strOfNegotiate(autonegFromSys(eth_cmd.autoneg));
    }
    else
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "SIOCETHTOOL:[%d] GetPortWorkPara failed.\n", ret);
    }

    //先清除sriov设置的属性，以防先前设置了没设置回来
    m_nIsSriov = 0;
    m_nIsLoopReport = 0;

    /* sriov 环回模式设置*/
    if (m_nTotalVfNum > 0)
    {
        int32 dwPortLoopbackMode;

        m_nIsSriov = 1;
        //m_nPortType = 2;

        /* 调用函数接口获取VF PCI资源 */
        if (SUCCESS != VNetGetSriovPci(GetDevName(), m_mapVirtfnPci))
        {
            //VNET_ASSERT(0);
            VNET_LOG(VNET_LOG_ERROR, "GetPortWorkPara GetSriovPci failed.\n");
        }

        if (SUCCESS == VNetGetPortLoopbackMode(GetDevName(),dwPortLoopbackMode))
        {
            m_nIsLoopReport = dwPortLoopbackMode;
        }
    }

    //获取物理端口mtu
    uint32 dwMtu;
    if (!(VNetGetMtu(GetDevName().c_str(), dwMtu)))
    {
        m_nMtu = dwMtu;
    }
}

//获取物理端口下的vf和pci映射关系
int32 CPhyPortClass:: GetSriovVfPci(vector <CSriovPortReport> &vSriovInfo)
{
    vSriovInfo.clear();
    CSriovPortReport info;

    map<string, string>::const_iterator it = m_mapVirtfnPci.begin();
    for (; it!= m_mapVirtfnPci.end(); ++it)
    {
        info._vf = it->first;
        info._pci = it->second;

        stringstream strStream;
        int retvar;
        strStream << ((info._vf).substr(6));
        strStream >>retvar;
        VNetGetVFPortVlan(GetNicName(), retvar, info._vlan_num);

        vSriovInfo.push_back(info);
    }

    return 0;
}

int32 CPhyPortClass::IsPciExit(const string &strPci, string &strVf)
{
    map<string, string>::const_iterator it = m_mapVirtfnPci.begin();
    for (; it!= m_mapVirtfnPci.end(); ++it)
    {
        if (strPci == it->second)
        {
            strVf = it->first;
            return 0;
        }
    }

    return -1;
}

int32 CPhyPortClass::speedFromSys(__u16 sys)
{
    switch (sys)
    {
    case SPEED_10:
        return PORT_SPEED_10M;
    case SPEED_100:
        return PORT_SPEED_100M;
    case SPEED_1000:
        return PORT_SPEED_1000M;
    case SPEED_10000:
        return PORT_SPEED_10G;
    default:
        return PORT_SPEED_UNKNOWN;
    }
}

int32 CPhyPortClass::duplexFromSys(__u8 sys)
{
    switch (sys)
    {
    case DUPLEX_HALF:
        return PORT_DUPLEX_HALF;
    case DUPLEX_FULL:
        return PORT_DUPLEX_FULL;
    default:
        return PORT_DUPLEX_UNKNOWN;
    }
}

int32 CPhyPortClass::autonegFromSys(__u8 sys)
{
    switch (sys)
    {
    case AUTONEG_DISABLE:
        return PORT_NEGOTIATE_FORCE;
    case AUTONEG_ENABLE:
        return PORT_NEGOTIATE_AUTO;
    default:
        return PORT_NEGOTIATE_AUTO;
    }
}

string CPhyPortClass::strOfSpeed(int32 speed)
{
    switch (speed)
    {
    case PORT_SPEED_10M:
        return "10M";
    case PORT_SPEED_100M:
        return "100M";
    case PORT_SPEED_1000M:
        return "1000M";
    case PORT_SPEED_10G:
        return "10G";
    case PORT_SPEED_UNKNOWN:
        return "UNKNOWN";
    }
    return "Invalid";
}

string CPhyPortClass::strOfDuplex(int32 duplex)
{
    switch (duplex)
    {
    case PORT_DUPLEX_UNKNOWN:
        return "UNKNOWN";
    case PORT_DUPLEX_FULL:
        return "FULL";
    case PORT_DUPLEX_HALF:
        return "HALF";
    }
    return "Invalid";
}

string CPhyPortClass::strOfMaster(int32 master)
{
    switch (master)
    {
    case PORT_MASTER_UNKNOWN:
        return "UNKNOWN";
    case PORT_MASTER_AUTO:
        return "AUTO";
    case PORT_MASTER_MASTER:
        return "MASTER";
    case PORT_MASTER_SLAVE:
        return "SLAVE";
    }
    return "Invalid";
}

string CPhyPortClass::strOfStatus(int32 status)
{
    switch (status)
    {
    case PORT_STATUS_DOWN:
        return "DOWN";
    case PORT_STATUS_UP:
        return "UP";
    }
    return "Invalid";
}

string CPhyPortClass::strOfLinked(int32 linked)
{
    switch (linked)
    {
    case PORT_LINKED_UNKNOWN:
        return "UNKNOWN";
    case PORT_LINKED_YES:
        return "YES";
    case PORT_LINKED_NO:
        return "NO";
    }
    return "Invalid";
}

string CPhyPortClass::strOfNegotiate(int32 negotiate)
{
    switch (negotiate)
    {
    case PORT_NEGOTIATE_AUTO:
        return "AUTO";
    case PORT_NEGOTIATE_FORCE:
        return "FORCE";
    }
    return "Invalid";
}

string CPhyPortClass::strOfUsed(int32 used)
{
    switch (used)
    {
    case PORT_USED_YES:
        return "YES";
    case PORT_USED_NO:
        return "NO";
    }
    return "Invalid";
}

void CPhyPortClass::DbgShow()
{
    cout << "port_name: " << m_strNicName << endl;
    cout << "port_type: " << m_nPortType << endl;
    cout << "port_status: " << m_nState<< endl;
    cout << "port_linked: "<< m_nIsLinked<< endl;
    cout << "port_speed: " << m_strSpeed << endl;
    cout << "port_duplex: " << m_strDuplex << endl;
    cout << "port_negotiate: " << m_strAutoNegotiate << endl;
    cout << "port_vfnum: " << m_nTotalVfNum << endl;
    cout << "port_loop: " << m_nIsLoopReport << endl;
    cout << "port_promiscuous: " << m_nPromiscuous << endl;
    cout << "port_sriov: " << m_nIsSriov << endl;
    cout << "port_mtu: " << m_nMtu << endl;

    map<string,string>::const_iterator it = m_mapVirtfnPci.begin();
    for (;it != m_mapVirtfnPci.end();it++)
    {
        cout << "VF: " << (it->first) << endl;
        cout << "PCI: " << (it->second) << endl;
    }

    cout << "-------------end of -------" << endl
    << endl;
}
} // namespace zte_tecs

