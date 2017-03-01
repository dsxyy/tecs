/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vnet_phyport_manager.cpp
* 文件标识：
* 内容摘要：物理端口类实现文件
* 当前版本：V1.0
* 作    者：liuhx
* 完成日期：2013年2月17日
*******************************************************************************/
#include "vnet_comm.h"
#include "dev_monitor.h"
#include "dev_mgr.h"
#include "vnet_function.h"
#include "vnet_phyport_manager.h"
#include "vna_vxlan_agent.h"

using namespace std;
using namespace _GLIBCXX_STD;

namespace zte_tecs
{

//获取实例
CPhyPortManager *CPhyPortManager::m_instance = NULL;
CPhyPortManager *CPhyPortManager::GetInstance()
{
    if (m_instance == NULL)
    {
        m_instance = new CPhyPortManager();
    }
    return m_instance;
}

CPhyPortManager::~CPhyPortManager()
{
    m_instance = NULL;

    map<string,CPhyPortClass*>::iterator it = m_mapPorts.begin();
    for (;it != m_mapPorts.end();)
    {
        CPhyPortClass *port = static_cast<CPhyPortClass *>(it->second);
        if (NULL != port)
        {
            delete port;
        }

        m_mapPorts.erase(it++);
    }
}

//实际调用PortClass获取端口各个参数信息
void CPhyPortManager::GetPortWorkPara()
{
    int fd = 0;
    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPhyPortManager:[%d] GetPortWorkPara failed.\n", fd);
    }
    else
    {
        map<string, CPhyPortClass*>::iterator iter;
        for (iter = m_mapPorts.begin(); iter != m_mapPorts.end(); ++iter)
        {
            CPhyPortClass *ptemp;
            ptemp = iter->second;
            if (NULL == ptemp)
            {
                continue;
            }
            //实际执行PortClass获取端口各个参数信息
            ptemp->GetPortWorkPara(fd);
        }
        close(fd);
    }
}

//扫描所有物理端口
void CPhyPortManager::LookupPhyPort()
{
    vector<string> nics;
    vector<string> vResult_GRO;

    //先获取所有物理网卡名称
    if (SUCCESS != VNetGetAllNic(nics))
    {
        VNET_LOG(VNET_LOG_ERROR, "CPhyPortManager:LookupPhyPort GetAllNic failed.\n");
        return;
    }

    if (nics.empty())
    {
        map<string,CPhyPortClass*>::iterator itMap = m_mapPorts.begin();
        for (;itMap != m_mapPorts.end();)
        {
            CPhyPortClass *port = static_cast<CPhyPortClass *>(itMap->second);
            if (NULL != port)
            {
                delete port;
            }

            m_mapPorts.erase(itMap++);
        }

        return;
    }

    vector<string>::iterator it_nics = nics.begin();
    for (;it_nics!=nics.end();it_nics++)
    {
        //关闭gro
        string cmd_gro("ethtool -K ");
        cmd_gro += *it_nics;
        cmd_gro += " gro off";

        vResult_GRO.clear();
        if (SUCCESS != RunCmd(cmd_gro, vResult_GRO))
        {
            VNET_LOG(VNET_LOG_ERROR, "CPhyPortManager::[%s] disable GRO failed.\n", (*it_nics).c_str());
        }
	
        //和先前的比较，查找是否存在
        if (m_mapPorts.find(*it_nics) != m_mapPorts.end())
        {
            continue;
        }

        CPhyPortClass *port = NULL;
        if (NULL != (port = new CPhyPortClass()))
        {
            string nic = *it_nics;

            int32 sriov_num = 0;
            VNetGetSriovNum(nic, sriov_num);

            //可以考虑设置一些固有属性
            port->SetPortSriovNum(sriov_num);
            port->SetNicName(nic);
            port->SetDevName(nic);

            //插入新的物理端口
            m_mapPorts.insert(make_pair(nic, port));
        }
    }

    //删除不存在物理端口
    map<string,CPhyPortClass*>::iterator it = m_mapPorts.begin();
    for (;it != m_mapPorts.end();)
    {
        //上次扫描到的端口和新查到物理端口比较，查找是否存在
        vector<string>::iterator pos;
        pos = find(nics.begin(), nics.end(), (it->first));
        if (pos != nics.end())
        {
            it++;
            continue;
        }

        CPhyPortClass *port = static_cast<CPhyPortClass *>(it->second);
        if (NULL != port)
        {
            delete port;
        }

        m_mapPorts.erase((it++)->first);
    }

    //扫描后执行获取端口参数信息
    GetPortWorkPara();
}

//获取端口信息
int32 CPhyPortManager::GetPhyPortInfo(vector <CPhyPortReport> &phyportInfo)
{
    //目前设置和查询没考虑互斥
    //MutexLock lock(_mutex);

    //先清除
    phyportInfo.clear();

    //先扫描所有的物理端口号
    LookupPhyPort();

    if (m_mapPorts.empty()) return 0;

    map<string, CPhyPortClass*>::const_iterator it = m_mapPorts.begin();
    for (; it!= m_mapPorts.end(); ++it)
    {
        CPhyPortClass *ptemp = it->second;
        if (NULL == ptemp)
        {
            continue;
        }

        CPhyPortReport portinfo;
        //暂且获取这些基础值方便调试
        portinfo._basic_info._name    = ptemp->GetNicName();
        portinfo._basic_info._is_master = ptemp->GetPortMaster();
        portinfo._basic_info._state = ptemp->GetPortStatus();
        portinfo._basic_info._mtu = ptemp->GetPortMtu();
        portinfo._basic_info._port_type = ptemp->GetPortType();
        portinfo._basic_info._promiscuous = ptemp->GetPortPromiscuous();

        //物理口借用kernel接口获取ip/mask
        VNetGetKernelPortInfo(portinfo._basic_info._name, portinfo._basic_info._ip, portinfo._basic_info._mask);

        portinfo._is_linked = ptemp->GetPortLinked();
        portinfo._speed = ptemp->GetPortSpeed();
        portinfo._duplex = ptemp->GetPortDuplex();
        portinfo._auto_negotiate = ptemp->GetPortNegotiate();
        portinfo._total_vf_num= ptemp->GetPortSriovNum();
        portinfo._is_loop_report = ptemp->GetLoopbackMode();
        portinfo._is_sriov = ptemp->GetIsSriov();

        if (portinfo._is_sriov)
        {
            ptemp->GetSriovVfPci(portinfo._sriov_port);
        }

        phyportInfo.push_back(portinfo);
    }

    return 0;
}

//获取安装时产生的kernel配置信息
int32 CPhyPortManager::GetKernelPortInfo(CKernelPortReport &kernelportinfo)
{
    string cmd = "";

    vector<string> vName;
    vector<string>::iterator it;

    vector<string> vIp;
    vector<string> vMask;
    vector<string> vTap;

    //从配置文件中读
    string filePath("/opt/tecs/network/vna/etc/tecs.conf");
    if (0 != access(filePath.c_str(), 0))
    {
        return -1;
    }

    kernelportinfo._name = "";

    cmd = "cat /opt/tecs/network/vna/etc/tecs.conf | grep kernel_uplink_port | awk -F '=' '{print $2}' ";
    if (SUCCESS == VNetRunCmd(cmd,vName))
    {
        if (TRUE != vName.empty())
        {
            it = vName.begin();

            if ((*it).empty())
            {
                return -1;
            }

            kernelportinfo._type = 0;  //type标识是不是bond

            if ((*it) == "bond_kernel")
            {
                kernelportinfo._type  = 1;

                CBondInfo cInfo;
                string strMode = "";
                string active_nic = "";
                vector <string> other_nic;
                int32 nStatus = 0;
                if (VNET_PLUGIN_SUCCESS == VNetGetBondModeOVS((*it), strMode))
                {
                    if (strMode == "1"/*EN_BOND_MODE_BACKUP*/)
                    {
                        if (VNET_PLUGIN_SUCCESS == VNetBackupInfo((*it), active_nic, other_nic))
                        {
                            if (("None" != active_nic) && ("" != active_nic))
                            {
                                kernelportinfo._bond_map.push_back(active_nic);
                            }

                            vector<string>::iterator it_nics = other_nic.begin();
                            for (; it_nics != other_nic.end(); ++it_nics)
                            {
                                kernelportinfo._bond_map.push_back(*it_nics);
                            }

                            kernelportinfo._bond_mode = EN_BOND_MODE_BACKUP;
                        }
                    }
                    else if (strMode == "4"/*EN_BOND_MODE_802*/)
                    {
                        if (VNET_PLUGIN_SUCCESS == VNetLacpInfo((*it), nStatus/*cInfo.lacp.status*/, cInfo.lacp.aggregator_id, cInfo.lacp.partner_mac, \
                                                                cInfo.lacp.nic_suc, \
                                                                cInfo.lacp.nic_fail))
                        {

                            if (0 != nStatus)
                            {
                                vector<string>::iterator suc_nics = cInfo.lacp.nic_suc.begin();
                                for (; suc_nics != cInfo.lacp.nic_suc.end(); ++suc_nics)
                                {
                                    kernelportinfo._bond_map.push_back(*suc_nics);
                                }

                                vector<string>::iterator fail_nics = cInfo.lacp.nic_fail.begin();
                                for (; fail_nics != cInfo.lacp.nic_fail.end(); ++fail_nics)
                                {
                                    kernelportinfo._bond_map.push_back(*fail_nics);
                                }
                            }
                            else
                            {
                                cInfo.lacp.nic_suc.clear();
                                cInfo.lacp.nic_fail.clear();
                                //协商失败，lacp重新获取slave
                                if (0 == VNetGetBondSlavesOVS((*it), cInfo.lacp.nic_fail))
                                {
                                    vector<string>::iterator slave_nics = cInfo.lacp.nic_fail.begin();
                                    for (; slave_nics != cInfo.lacp.nic_fail.end(); ++slave_nics)
                                    {
                                        kernelportinfo._bond_map.push_back(*slave_nics);
                                    }
                                }
                            }

                            kernelportinfo._bond_mode = EN_BOND_MODE_802;
                        }
                    }
                    else
                    {
                        //0 模式不确定
                    }
                }
            }

            kernelportinfo._name = (*it);
        }
    }

    vTap.clear();
    cmd = "cat /opt/tecs/network/vna/etc/tecs.conf | grep kernel_tap | awk -F '=' '{print $2}' ";
    if (SUCCESS == VNetRunCmd(cmd,vTap))
    {
        if (TRUE != vTap.empty())
        {
            it = vTap.begin();
            kernelportinfo._krport_name = (*it);
        }
        else
        {
            kernelportinfo._name = "";
        }
    }

    vIp.clear();
    cmd = "cat /opt/tecs/network/vna/etc/tecs.conf | grep kernel_ip_uplink_port | awk -F '=' '{print $2}' ";
    if (SUCCESS == VNetRunCmd(cmd,vIp))
    {
        if (TRUE != vIp.empty())
        {
            it = vIp.begin();
            kernelportinfo._ip = (*it);
        }
    }

    vMask.clear();
    cmd = "cat /opt/tecs/network/vna/etc/tecs.conf | grep kernel_mask_uplink_port | awk -F '=' '{print $2}' ";
    if (SUCCESS == VNetRunCmd(cmd,vMask))
    {
        if (TRUE != vMask.empty())
        {
            it = vMask.begin();
            kernelportinfo._mask = (*it);
        }
    }

    kernelportinfo._switch_name = "sdvs_00";

    return 0;
}

int32 CPhyPortManager::GetKernelPortCfgInfo(vector<CKernelPortCfgReport> &vKernelCfgInfo)
{
    vKernelCfgInfo.clear();

    //获取所有的kernel口，以带kernel名字过滤
    vector<string> outVInfo;
    if (SUCCESS != VNetGetAllKernelPort(outVInfo))
    {
        return -1;
    }

    if (outVInfo.empty())
    {
        return 0;
    }

    vector<string>::iterator it = outVInfo.begin();
    for (; it != outVInfo.end(); ++it)
    {
        CKernelPortCfgReport  info;

        if (((*it).find("kernel",0) == 0) || ("dvs_kernel" == (*it)))
        {
            //获取该端口绑定的交换
            string bridge;
            if (SUCCESS == VNetGetBridgeOfIf(*it, bridge))
            {
                info._switch_name = bridge;
            }

            info._krport_name = (*it);

            //获取ip/mask
            VNetGetKernelPortInfo(info._krport_name, info._ip, info._mask);

            //获取kernel state
            //VNetGetNicStatus(info._krport_name, info._basic_info._state);
            //经讨论虚口使用ifconfig获取2013.06.24
            string cmd("ifconfig ");
            cmd += info._krport_name;
            cmd += " | grep 'UP' ";

            vector<string> vResult;
            if (SUCCESS == RunCmd(cmd, vResult))
            {
                if (vResult.empty())
                {
                    info._basic_info._state = 0;
                }
                else
                {
                    info._basic_info._state = 1;
                }
            }

            vKernelCfgInfo.push_back(info);
        }
    }

    return 0;
}

//获取vtep口信息上报
int32 CPhyPortManager::GetVtepPortCfgInfo(vector<CVtepPortCfgReport> &vVtepCfgInfo)
{
    vVtepCfgInfo.clear();

    //获取所有的虚拟口，以带vtep名字过滤
    vector<string> outVInfo;
    if (SUCCESS != VNetGetAllKernelPort(outVInfo))
    {
        return -1;
    }

    if (outVInfo.empty())
    {
        return 0;
    }

    vector<string>::iterator it = outVInfo.begin();
    for (; it != outVInfo.end(); ++it)
    {
        CVtepPortCfgReport  info;

        if (((*it).find("vtep",0) == 0))
        {
            //获取该端口绑定的交换
            string bridge;
            if (SUCCESS == VNetGetBridgeOfIf(*it, bridge))
            {
                info._switch_name = bridge;
            }

            info._vtep_name = (*it);

            //获取ip/mask
            VNetGetKernelPortInfo(info._vtep_name, info._ip, info._mask);

            //获取kernel state
            string cmd("ifconfig ");
            cmd += info._vtep_name;
            cmd += " | grep 'UP' ";

            vector<string> vResult;
            if (SUCCESS == RunCmd(cmd, vResult))
            {
                if (vResult.empty())
                {
                    info._basic_info._state = 0;
                }
                else
                {
                    info._basic_info._state = 1;
                }
            }

            vVtepCfgInfo.push_back(info);
        }
    }

    return 0;
}

//获取端口的ip/mask，分别采用socket和ifconfig命令
int32 CPhyPortManager::GetPortIpMask(const string &strPortName, string &strIp, string &strMask)
{
    int sockfd;
    int ret;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        //VNET_ASSERT(0);
        return -1;
    }

    struct ifreq ifr;
    struct ifreq ifr_mask;

    bzero(ifr.ifr_name, sizeof(ifr.ifr_name));
    strncpy(ifr.ifr_name, (strPortName).c_str(), sizeof(ifr.ifr_name)-1);
    ret = ioctl(sockfd, SIOCGIFADDR, ifr);
    if (ret >= 0)
    {
        strIp = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
    }

    bzero(ifr_mask.ifr_name, sizeof(ifr_mask.ifr_name));
    strncpy(ifr_mask.ifr_name, (strPortName).c_str(), sizeof(ifr_mask.ifr_name)-1);
    ret = ioctl(sockfd, SIOCGIFNETMASK, ifr_mask);
    if (ret >= 0)
    {
        strMask = inet_ntoa(((struct sockaddr_in *)&ifr_mask.ifr_netmask )->sin_addr);
    }

    close(sockfd);
    return 0;
}

int32 CPhyPortManager::GetVirPortIpMask(const string &strPortName, string &strIp, string &strMask)
{
    string cmd("ifconfig ");
    cmd += strPortName;
    cmd += " | grep 'inet addr' | awk '{print $2}'";

    vector<string> vResult;
    vector<string>::iterator it_vResult;
    if ( SUCCESS != RunCmd(cmd, vResult))
    {
        return ERROR;
    }
    if (vResult.empty())
    {
        return ERROR;
    }

    it_vResult = vResult.begin();

    string cmd1("echo ");
    cmd1 += *it_vResult;
    cmd1 += " | awk -F ':' '{print $2}'";

    vResult.clear();
    if ( SUCCESS != RunCmd(cmd1, vResult))
    {
        return ERROR;
    }
    if (vResult.empty())
    {
        return ERROR;
    }

    strIp = *(vResult.begin());

    string cmd2("ifconfig ");
    cmd2 += strPortName;
    cmd2 += " | grep 'Mask' | awk '{print $4}'";

    vector<string> vResult1;
    vector<string>::iterator it_vResult1;
    if ( SUCCESS != RunCmd(cmd2, vResult1))
    {
        return ERROR;
    }
    if (vResult1.empty())
    {
        return ERROR;
    }

    it_vResult1 = vResult1.begin();

    string cmd3("echo ");
    cmd3 += *it_vResult1;
    cmd3 += " | awk -F ':' '{print $2}'";

    vResult1.clear();
    if ( SUCCESS != RunCmd(cmd3, vResult1))
    {
        return ERROR;
    }
    if (vResult1.empty())
    {
        return ERROR;
    }

    strMask = *(vResult1.begin());

    return SUCCESS;

}

//设置端口环回模式[SRIOV]
int32 CPhyPortManager::SetPortLoopback(const string &strPortName, const uint32 &dwFlag)
{
    if (0 == VNetSetPorLoopback(strPortName,dwFlag))
    {
        return 0;
    }

    return -1;
}

//提供设置mtu接口
int32 CPhyPortManager::SetPortMtu(const string &strPortName, const uint32 &dwMtu)
{
    if (0 == VNetSetMtu(strPortName.c_str(), dwMtu))
    {
        return 0;
    }

    return -1;
}

int32 CPhyPortManager::IsPciAssignable(const string &strPci)
{
#if 0
    vector<string> vPci;
    vector<string>::iterator it_vPci;
    string cmd("xm pci-list-assignable ");

    if ( SUCCESS != VNetRunCmd(cmd,vPci))
    {
        return -1;
    }

    if (vPci.empty())
    {
        return 0;
    }

    it_vPci = vPci.begin();
    for (; it_vPci != vPci.end(); ++it_vPci)
    {
        if ((*it_vPci) == pci)
        {
            return 1;
        }
    }

    return 0;
#endif

    string strTmpVf;

    map<string,CPhyPortClass*>::iterator it = m_mapPorts.begin();
    for (;it != m_mapPorts.end();it++)
    {
        CPhyPortClass *port = static_cast<CPhyPortClass *>(it->second);
        if (NULL != port)
        {
            if (!(port->IsPciExit(strPci, strTmpVf)))
            {
                return 0;
            }
        }
    }

    VNET_LOG(VNET_LOG_ERROR, "CPhyPortManager:IsPciAssignable PCI:%s not exit.\n", strPci.c_str());
    return -1;

}

//检查PCI是否存在
int32 CPhyPortManager::IsPciExit(const string &strPci, string &strPhyName, string &strVf)
{
    string strTmpVf;

    map<string,CPhyPortClass*>::iterator it = m_mapPorts.begin();
    for (;it != m_mapPorts.end();it++)
    {
        CPhyPortClass *port = static_cast<CPhyPortClass *>(it->second);
        if (NULL != port)
        {
            if (!(port->IsPciExit(strPci, strTmpVf)))
            {
                strPhyName = it->first;
                strVf = strTmpVf;
                return 0;
            }
        }
    }

    VNET_LOG(VNET_LOG_ERROR, "CPhyPortManager:IsPciExit PCI:%s not exit.\n", strPci.c_str());
    return -1;
}

int32 CPhyPortManager::SetVFPortMac(const string &strPci, const string &strMac)
{
    int32 ret;
    string strTmpPortName;
    string strTmpVf;

    if ((strPci.empty()) || (strMac.empty()))
    {
        return -1;
    }

    ret = IsPciExit(strPci, strTmpPortName, strTmpVf);
    if (!ret)
    {
        stringstream strStream;
        int retvar;
        strStream << (strTmpVf.substr(6));
        strStream >>retvar;

        VNET_LOG(VNET_LOG_INFO, "CPhyPortManager:SetVFPortMac %s %s %s %d.\n",
                     strPci.c_str(),
                     strTmpPortName.c_str(),
                     strMac.c_str(),
                     retvar);

        if (0 != VNetSetVFPortMac(strTmpPortName, retvar, strMac))
        {
            VNET_LOG(VNET_LOG_ERROR, "CPhyPortManager:SetVFPortMac failed %s %s %s %d.\n",
                     strPci.c_str(),
                     strTmpPortName.c_str(),
                     strMac.c_str(),
                     retvar);
            return -1;
        }

        return 0;
    }

    return -1;
}

int32 CPhyPortManager::SetVFPortVlan(const string &strPci, const int32 &nVlan)
{
    int32 ret;
    string strTmpPortName;
    string strTmpVf;

    if (strPci.empty())
    {
        return -1;
    }

    ret = IsPciExit(strPci, strTmpPortName, strTmpVf);
    if (!ret)
    {
        stringstream strStream;
        int retvar;
        strStream << (strTmpVf.substr(6));
        strStream >>retvar;

       VNET_LOG(VNET_LOG_INFO, "CPhyPortManager:SetVFPortVlan %s %s %d %d.\n",
                     strPci.c_str(),
                     strTmpPortName.c_str(),
                     nVlan,
                     retvar);

        if (0 != VNetSetVFPortVlan(strTmpPortName, retvar, nVlan))
        {
            VNET_LOG(VNET_LOG_ERROR, "CPhyPortManager:SetVFPortVlan failed %s %s %d %d.\n",
                     strPci.c_str(),
                     strTmpPortName.c_str(),
                     nVlan,
                     retvar);
            return -1;
        }

        return 0;
    }

    return -1;
}

int32 CPhyPortManager::GetVFPortMac(const string &strPci, string &strMac)
{
    int32 ret;
    string strTmpPortName;
    string strTmpVf;

    if (strPci.empty())
    {
        return -1;
    }

    ret = IsPciExit(strPci, strTmpPortName, strTmpVf);
    if (!ret)
    {
        stringstream strStream;
        int retvar;
        strStream << (strTmpVf.substr(6));
        strStream >>retvar;

        /*VNET_LOG(VNET_LOG_INFO, "CPhyPortManager:GetVFPortMac %s %s %d.\n",
                     strPci.c_str(),
                     strTmpPortName.c_str(),
                     retvar);*/

        if (0 != VNetGetVFPortMac(strTmpPortName, retvar, strMac))
        {
            VNET_LOG(VNET_LOG_ERROR, "CPhyPortManager:GetVFPortMac failed %s %s %d.\n",
                     strPci.c_str(),
                     strTmpPortName.c_str(),
                     retvar);
            return -1;
        }

        return 0;
    }

    return -1;
}

int32 CPhyPortManager::GetVFPortVlan(const string &strPci, int32 &nVlan)
{
    int32 ret;
    string strTmpPortName;
    string strTmpVf;

    if (strPci.empty())
    {
        return -1;
    }

    ret = IsPciExit(strPci, strTmpPortName, strTmpVf);
    if (!ret)
    {
        stringstream strStream;
        int retvar;
        strStream << (strTmpVf.substr(6));
        strStream >>retvar;

       /*VNET_LOG(VNET_LOG_INFO, "CPhyPortManager:GetVFPortVlan %s %s %d.\n",
                     strPci.c_str(),
                     strTmpPortName.c_str(),
                     retvar);*/

        if (0 != VNetGetVFPortVlan(strTmpPortName, retvar, nVlan))
        {
            VNET_LOG(VNET_LOG_ERROR, "CPhyPortManager:GetVFPortVlan failed %s %s %d.\n",
                     strPci.c_str(),
                     strTmpPortName.c_str(),
                     retvar);
            return -1;
        }

        return 0;
    }

    return -1;
}

#if 0
int32 CPhyPortManager::GetPortMapInfo(map<string, int32> &mapPortInfo)
{
    string strName;
    int32  nStatus;

    map<string,CPhyPortClass*>::iterator it = m_mapPorts.begin();
    for (;it != m_mapPorts.end();it++)
    {
        CPhyPortClass *port = static_cast<CPhyPortClass *>(it->second);
        if (NULL != port)
        {
            strName = port->GetNicName();
            nStatus = port->GetPortLinked();
            mapPortInfo.insert(make_pair(strName, nStatus));
        }
    }

    return 0;
}
#endif

int32 CPhyPortManager::GetPortMapInfo(map<string, int32> &mapPortInfo)
{
    vector<string> nics;
    string strName;
    int32  nStatus;

    //先获取所有物理网卡名称
    if (SUCCESS != VNetGetAllNic(nics))
    {
        VNET_LOG(VNET_LOG_ERROR, "CPhyPortManager:GetPortMapInfo GetAllNic failed.\n");
        return -1;
    }

    vector<string>::iterator it = nics.begin();
    for (;it != nics.end();it++)
    {
        strName = (*it);
        nStatus = 0;
        mapPortInfo.insert(make_pair(strName, nStatus));
    }

    return 0;
}

//创建vtep接口
int32 CPhyPortManager::CreateVtepPort(const string &strVtepName, const string &strSwitchName, const string &strUplinkPort,
                                         const string &strIp, const string &strMask, int32 nVlanId)
{
    if((""== strVtepName) || ("" == strSwitchName) || ("" == strUplinkPort) || ("" == strIp) || ("" == strMask))
    {
        return -1;
    }

    //此处沿用kernel口创建
    if (0 != VNetCreateKernelPort(strVtepName, strSwitchName, strUplinkPort, strIp, strMask, 0, (uint32)nVlanId, 0))
    {
        VNET_LOG(VNET_LOG_ERROR, "CPhyPortManager:[%s %s %s %s]CreateVtepPort failed.\n", strVtepName.c_str(), strSwitchName.c_str(), strIp.c_str(), strMask.c_str());
        return -1;
    }

    return 0;
}

int32 CPhyPortManager::DeleteVtepPort(const string &strSwitchName)
{
    if (strSwitchName.empty())
    {
        VNET_LOG(VNET_LOG_ERROR, "CPhyPortManager:strSwitchName is empty.\n");
        return -1;
    }

    CVxlanAgent *pAgent = CVxlanAgent::GetInstance();
    if(!pAgent)
    {
        VNET_LOG(VNET_LOG_ERROR, "CPhyPortManager:CVxlanAgent::GetInstance() is NULL.\n");
        return -1;
    }

    string strVtepName = pAgent->GetVtepNameBySwitchName(strSwitchName);
    if (strVtepName.empty())
    {
        VNET_LOG(VNET_LOG_ERROR, "CPhyPortManager:[%s]GetVtepNameBySwitchName failed.\n", strSwitchName.c_str());
        return -1;
    }
    
    if (0 != VNetDeleteKernelPort(strSwitchName, strVtepName))
    {
        VNET_LOG(VNET_LOG_ERROR, "CPhyPortManager:[%s %s]DeleteVtepPort failed.\n", strVtepName.c_str(), strSwitchName.c_str());
        return -1;
    }
    
    return 0;
}

void CPhyPortManager::DoMonitor(CNetDevVisitor *dev_visit)
{
    map<string, CPhyPortClass*>::iterator port_dev = m_mapPorts.begin();

    for (; port_dev!=m_mapPorts.end(); port_dev++)
    {
        port_dev->second->Accept(dev_visit);
    }
};

void CPhyPortManager::DoMonitor(CPortVisitor *dev_visit)
{
    map<string, CPhyPortClass*>::iterator port_dev = m_mapPorts.begin();

    for (; port_dev!=m_mapPorts.end(); port_dev++)
    {
        port_dev->second->Accept(dev_visit);
    }
};

//调试和测试代码
/******************************************************************************/
void CPhyPortManager::DbgShow()
{
    //MutexLock lock(_mutex);
    CPhyPortClass *port;
    map<string, CPhyPortClass*>::iterator it_port;
    for (it_port = m_mapPorts.begin(); it_port != m_mapPorts.end(); ++it_port)
    {
        port = it_port->second;
        if (NULL != port)
        {
            port->DbgShow();
        }
    }
}

//test code
void TestPhyPortInfo()
{
    CMessageVNAPortInfoReport PortInfo;
    CPhyPortManager *pHandle = CPhyPortManager::GetInstance();
    if (pHandle)
    {
        pHandle->GetPhyPortInfo(PortInfo._phy_port);
        pHandle->DbgShow();
    }
    return;
}
DEFINE_DEBUG_FUNC(TestPhyPortInfo);

void TestSriov(char * pci)
{
    string strMac;
    int nVlan;
    string strPci(pci);

    CPhyPortManager *pHandle = CPhyPortManager::GetInstance();
    if (pHandle)
    {
        pHandle->GetVFPortMac(strPci, strMac);
        cout << "MAC:" << strMac << endl;
        pHandle->GetVFPortVlan(strPci, nVlan);
        cout << "VLAN:" << nVlan << endl;
    }
    return;
}
DEFINE_DEBUG_FUNC(TestSriov);

void VnetDbgShowPhyPortInfo()
{
    CPhyPortManager *pHandle = CPhyPortManager::GetInstance();
    if (pHandle)
    {
        pHandle->DbgShow();
    }
    return;
}
DEFINE_DEBUG_FUNC(VnetDbgShowPhyPortInfo);
} // namespace zte_tecs

