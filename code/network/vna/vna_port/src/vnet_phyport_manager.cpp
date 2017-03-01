/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�vnet_phyport_manager.cpp
* �ļ���ʶ��
* ����ժҪ������˿���ʵ���ļ�
* ��ǰ�汾��V1.0
* ��    �ߣ�liuhx
* ������ڣ�2013��2��17��
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

//��ȡʵ��
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

//ʵ�ʵ���PortClass��ȡ�˿ڸ���������Ϣ
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
            //ʵ��ִ��PortClass��ȡ�˿ڸ���������Ϣ
            ptemp->GetPortWorkPara(fd);
        }
        close(fd);
    }
}

//ɨ����������˿�
void CPhyPortManager::LookupPhyPort()
{
    vector<string> nics;
    vector<string> vResult_GRO;

    //�Ȼ�ȡ����������������
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
        //�ر�gro
        string cmd_gro("ethtool -K ");
        cmd_gro += *it_nics;
        cmd_gro += " gro off";

        vResult_GRO.clear();
        if (SUCCESS != RunCmd(cmd_gro, vResult_GRO))
        {
            VNET_LOG(VNET_LOG_ERROR, "CPhyPortManager::[%s] disable GRO failed.\n", (*it_nics).c_str());
        }
	
        //����ǰ�ıȽϣ������Ƿ����
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

            //���Կ�������һЩ��������
            port->SetPortSriovNum(sriov_num);
            port->SetNicName(nic);
            port->SetDevName(nic);

            //�����µ�����˿�
            m_mapPorts.insert(make_pair(nic, port));
        }
    }

    //ɾ������������˿�
    map<string,CPhyPortClass*>::iterator it = m_mapPorts.begin();
    for (;it != m_mapPorts.end();)
    {
        //�ϴ�ɨ�赽�Ķ˿ں��²鵽����˿ڱȽϣ������Ƿ����
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

    //ɨ���ִ�л�ȡ�˿ڲ�����Ϣ
    GetPortWorkPara();
}

//��ȡ�˿���Ϣ
int32 CPhyPortManager::GetPhyPortInfo(vector <CPhyPortReport> &phyportInfo)
{
    //Ŀǰ���úͲ�ѯû���ǻ���
    //MutexLock lock(_mutex);

    //�����
    phyportInfo.clear();

    //��ɨ�����е�����˿ں�
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
        //���һ�ȡ��Щ����ֵ�������
        portinfo._basic_info._name    = ptemp->GetNicName();
        portinfo._basic_info._is_master = ptemp->GetPortMaster();
        portinfo._basic_info._state = ptemp->GetPortStatus();
        portinfo._basic_info._mtu = ptemp->GetPortMtu();
        portinfo._basic_info._port_type = ptemp->GetPortType();
        portinfo._basic_info._promiscuous = ptemp->GetPortPromiscuous();

        //����ڽ���kernel�ӿڻ�ȡip/mask
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

//��ȡ��װʱ������kernel������Ϣ
int32 CPhyPortManager::GetKernelPortInfo(CKernelPortReport &kernelportinfo)
{
    string cmd = "";

    vector<string> vName;
    vector<string>::iterator it;

    vector<string> vIp;
    vector<string> vMask;
    vector<string> vTap;

    //�������ļ��ж�
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

            kernelportinfo._type = 0;  //type��ʶ�ǲ���bond

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
                                //Э��ʧ�ܣ�lacp���»�ȡslave
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
                        //0 ģʽ��ȷ��
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

    //��ȡ���е�kernel�ڣ��Դ�kernel���ֹ���
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
            //��ȡ�ö˿ڰ󶨵Ľ���
            string bridge;
            if (SUCCESS == VNetGetBridgeOfIf(*it, bridge))
            {
                info._switch_name = bridge;
            }

            info._krport_name = (*it);

            //��ȡip/mask
            VNetGetKernelPortInfo(info._krport_name, info._ip, info._mask);

            //��ȡkernel state
            //VNetGetNicStatus(info._krport_name, info._basic_info._state);
            //���������ʹ��ifconfig��ȡ2013.06.24
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

//��ȡvtep����Ϣ�ϱ�
int32 CPhyPortManager::GetVtepPortCfgInfo(vector<CVtepPortCfgReport> &vVtepCfgInfo)
{
    vVtepCfgInfo.clear();

    //��ȡ���е�����ڣ��Դ�vtep���ֹ���
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
            //��ȡ�ö˿ڰ󶨵Ľ���
            string bridge;
            if (SUCCESS == VNetGetBridgeOfIf(*it, bridge))
            {
                info._switch_name = bridge;
            }

            info._vtep_name = (*it);

            //��ȡip/mask
            VNetGetKernelPortInfo(info._vtep_name, info._ip, info._mask);

            //��ȡkernel state
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

//��ȡ�˿ڵ�ip/mask���ֱ����socket��ifconfig����
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

//���ö˿ڻ���ģʽ[SRIOV]
int32 CPhyPortManager::SetPortLoopback(const string &strPortName, const uint32 &dwFlag)
{
    if (0 == VNetSetPorLoopback(strPortName,dwFlag))
    {
        return 0;
    }

    return -1;
}

//�ṩ����mtu�ӿ�
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

//���PCI�Ƿ����
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

    //�Ȼ�ȡ����������������
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

//����vtep�ӿ�
int32 CPhyPortManager::CreateVtepPort(const string &strVtepName, const string &strSwitchName, const string &strUplinkPort,
                                         const string &strIp, const string &strMask, int32 nVlanId)
{
    if((""== strVtepName) || ("" == strSwitchName) || ("" == strUplinkPort) || ("" == strIp) || ("" == strMask))
    {
        return -1;
    }

    //�˴�����kernel�ڴ���
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

//���ԺͲ��Դ���
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

