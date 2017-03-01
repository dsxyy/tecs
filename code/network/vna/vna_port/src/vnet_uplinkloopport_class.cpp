/*******************************************************************************
 * Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
 *
 * 文件名称：vnet_uplinkloopport_class.cpp
 * 文件标识：
 * 内容摘要：上行还回端口类实现文件
 * 当前版本：V1.0
 * 作    者：liuhx
 * 完成日期：2013年3月8日
 *******************************************************************************/
#include <linux/sockios.h>
#include <linux/ethtool.h>
#include "vnet_comm.h"
#include "vnet_uplinkloopport_class.h"
#include "vnet_function.h"
#include "phy_port_pub.h"

namespace zte_tecs
{

CUplinkLoopPortClass::~CUplinkLoopPortClass()
{
}

//获取每个物理端口的参数
void CUplinkLoopPortClass:: GetPortWorkPara(int fd)
{
    int ret = 0;
    struct ifreq ifr;
    struct ethtool_cmd;

    //先填写nic name
    if(0 == m_strName.length())
    {
        //VNET_ASSERT(0);
        return;
    }

    strcpy(ifr.ifr_name, m_strName.c_str());
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
        VNET_LOG(VNET_LOG_ERROR, "SIOCGIFFLAGS:[%s] GetPortWorkPara failed.\n", fd);
    }
}

int32 CUplinkLoopPortClass::speedFromSys(__u16 sys)
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

int32 CUplinkLoopPortClass::duplexFromSys(__u8 sys)
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

int32 CUplinkLoopPortClass::autonegFromSys(__u8 sys)
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

string CUplinkLoopPortClass::strOfSpeed(int32 speed)
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

string CUplinkLoopPortClass::strOfDuplex(int32 duplex)
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

string CUplinkLoopPortClass::strOfMaster(int32 master)
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

string CUplinkLoopPortClass::strOfStatus(int32 status)
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

string CUplinkLoopPortClass::strOfLinked(int32 linked)
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

string CUplinkLoopPortClass::strOfNegotiate(int32 negotiate)
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

string CUplinkLoopPortClass::strOfUsed(int32 used)
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

void CUplinkLoopPortClass::DbgShow()
{
    cout << "port_name: " << m_strName << endl;
    cout << "port_status: " << m_nState << endl;
    cout << "port_promiscuous: " << m_nPromiscuous << endl;
}
} // namespace zte_tecs

