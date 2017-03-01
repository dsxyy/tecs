/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�vm_driver.cpp
* �ļ���ʶ��
* ����ժҪ��XenDriver���ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�lixch
* ������ڣ�2011��7��25��
*******************************************************************************/
#ifdef __XEN_DEFINE__

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string> 
#include <iostream>
#include <stdio.h>
#include "libxl_impl.h"       // ��ͷ�ļ���xen_driver.h ˳�����滻
#include "xen_driver.h"


/*****************************************************************************
 *                               ȫ�ֱ���                                    *
 ****************************************************************************/
#define PCPU_WEIGHT_TOTAL 10000
#define libxl_vm_desc_dir        "/var/run/libxl/"
#define STREQ(a,b) (strcmp(a,b) == 0)

/****************************************************************************************
* �������ƣ�Init
* ������������ʼ�����⻯����
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         ����
****************************************************************************************/
int XenDriver::Init()
{
    if (0 != libxl_init())
    {
        return -1;
    }
    
    if (mkdir(libxl_vm_desc_dir, 0777) < 0 && errno != EEXIST) {
        return -1;
    }
    return 0;
}

/****************************************************************************************
* �������ƣ�GetDiskConfig
* ������������ȡDISK�������ļ�
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         ����
****************************************************************************************/
void XenDriver::GetDiskConfig(ostringstream &disk_cfg, const VmDiskConfig &disk)
{
    bool is_readonly = false;
    disk_cfg << " \"";
    if(STORAGE_SOURCE_TYPE_BLOCK == disk._source_type)
    {
        disk_cfg << "phy:";
    }
    else
    {
        disk_cfg << "file:";
    }
    disk_cfg << disk._source << "," ;
    disk_cfg <<"ioemu:"<< disk._target ;
    
    if((disk._target == "hdc") 
       &&(disk._id != INVALID_OID))
    {
        disk_cfg <<":cdrom";
        is_readonly = true; 
    }

    if (disk._type  == "ro_disk")
    {
        is_readonly = true;
    }
    
    if (is_readonly)
    {
        disk_cfg << ",r\" " ;
    }
    else
    {
        disk_cfg << ",w\" " ;
    }
}

/****************************************************************************************
* �������ƣ�GetNormalVifConfig
* ������������ȡ��ͨ���ڵ�����
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         ����
****************************************************************************************/
void XenDriver::GetNormalVifConfig(ostringstream &vif_cfg, const VmNicConfig &nic, VMConfig &cfg)
{
    vif_cfg << " \"";
    vif_cfg << "mac=" << nic._mac << ",";
    vif_cfg << "bridge="<<nic._bridge;
    if ((!cfg._virt_type.compare(VIRT_TYPE_HVM))  /* HVM ,PVM����д*/
        &&(nic._model.empty()!= true)) 
    {
        vif_cfg << ",model="<<nic._model<<"\"";
    }
    else
    {
        vif_cfg << " \"";
    }
}

/************************************************************************/
string XenDriver::GetAllNormalNicCfg(VMConfig &vm_cfg)
{
    ostringstream nic_cfg;
    nic_cfg << "[";
    
    vector<VmNicConfig>::iterator itnic;
    uint32 nic_num = vm_cfg._nics.size();
    for(itnic = vm_cfg._nics.begin(); itnic != vm_cfg._nics.end(); itnic++)
    {
        nic_num--;
        if(true == itnic->_sriov)
        {
           continue;
        }
        else
        {
            GetNormalVifConfig(nic_cfg, *itnic, vm_cfg);
        }
        
        if (nic_num != 0)
        {
            nic_cfg << ", ";
        }
    }
    nic_cfg << "]";
    return nic_cfg.str();
}


/****************************************************************************************
* �������ƣ�GetAllDiskConfig
* ������������ȡ���д��̵������ļ�
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         ����
****************************************************************************************/
string XenDriver::GetAllDiskConfig(VMConfig &cfg)
{
    ostringstream disk_cfg;

    disk_cfg << "[";

    // ��ӳ���̼��뵽disk�б���
    GetDiskConfig(disk_cfg, cfg._machine);
    if (cfg._disks.size() != 0)
    {
        disk_cfg << ",  ";
    }
    
    vector<VmDiskConfig>::iterator itdisk;
    uint32 disk_num = cfg._disks.size();
    for(itdisk = cfg._disks.begin(); itdisk != cfg._disks.end(); itdisk++)
    {
        GetDiskConfig(disk_cfg, *itdisk);
        disk_num--;
        if (disk_num != 0)
        {
            disk_cfg << ",  ";
        }
    }
    disk_cfg << "]";
    return disk_cfg.str();
}

/****************************************************************************************
* �������ƣ�GetAllSriovConfig
* ������������ȡ����SRIOV������
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         ����
****************************************************************************************/
void XenDriver::GetAllSriovConfig(ostringstream &vif_cfg, VMConfig &cfg)
{
    vector<VmNicConfig>::iterator itnic;
    uint32 nic_num = cfg._nics.size();
    for(itnic = cfg._nics.begin(); itnic != cfg._nics.end(); itnic++)
    {
        nic_num--;
        if(true == itnic->_sriov)
        {
           GetSRIOVConfig(vif_cfg, *itnic);
        }
        else
        {
            continue;
        }
        
        if (nic_num != 0)
        {
            vif_cfg << ", ";
        }
    }
}

/****************************************************************************************
* �������ƣ�GetSRIOVConfig
* ������������ȡ����SRIOV������
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         ����
****************************************************************************************/
void XenDriver::GetSRIOVConfig(ostringstream &vif_cfg, const VmNicConfig &nic)
{
    if(true != nic._sriov)
    {
        return ;
    }
    
    vif_cfg << " \"" << nic._pci_order << " \"";
}


/****************************************************************************************
* �������ƣ�GetAllPciCfg
* ������������������PCI�豸����
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         ����
****************************************************************************************/
string XenDriver::GetAllPciCfg(VMConfig &cfg)
{
    ostringstream pci_cfg;
    string             pci_order;

    pci_cfg << "[";
    GetAllSriovConfig(pci_cfg, cfg);

    vector<PciBus>::iterator it_bus;
    uint32 pci_num = cfg._pci_devices.size();
    for (it_bus = cfg._pci_devices.begin(); it_bus != cfg._pci_devices.end(); it_bus ++)
    {
        ConstructPciNoString(it_bus->_domain, it_bus->_bus, it_bus->_slot, 
                                       it_bus->_function,pci_order);
        
        pci_cfg << " \"" << pci_order << " \"";
        pci_num--;
        if (pci_num  != 0)
        {
            pci_cfg << ", ";
        }
    }
    pci_cfg << "]";

    return pci_cfg.str();
}

/****************************************************************************************
* �������ƣ�CreateConfig
* �������������������ļ�
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         ����
****************************************************************************************/
string XenDriver::CreateConfig(const string vm_ins_name, VMConfig &cfg)
{
    ostringstream cfg_value;

    // д��������
    // pvm�����ݲ�ʵ�֣���ʵ��HVM
    cfg_value << "name=\"" << vm_ins_name<<"\"\n";
    cfg_value << "description=\"" << cfg._config_version <<"\"\n"; 
    cfg_value << "maxmem=" << to_string<int64>(cfg._memory/(1024*1024),dec)<<"\n";
    cfg_value << "memory=" << to_string<int64>(cfg._memory/(1024*1024),dec)<<"\n";
    cfg_value << "vcpus=" << to_string<uint32>(cfg._vcpu,dec)<<"\n";
    cfg_value << "builder=" << "\"hvm\"" <<"\n";
    cfg_value << "kernel=" << "\"/usr/lib/xen/boot/hvmloader\""<<"\n";
    cfg_value << "device_model_override=" << "\"/usr/lib/xen/bin/qemu-system-i386\""<<"\n";
    cfg_value << "device_model_version =\"qemu-xen\""<<"\n";
    cfg_value << "boot=" << "\"dca\""<<"\n";
    cfg_value << "pae=" << 1<<"\n";
    cfg_value << "acpi=" <<1<<"\n";
    cfg_value << "apic=" << 1<<"\n";
    cfg_value << "hap=" << 1<<"\n";
    cfg_value << "tsc_mode=" << 2<<"\n";
    cfg_value << "on_poweroff=" << "\"destroy\""<<"\n";
    cfg_value << "on_restart=" << "\"restart\""<<"\n";
    if (cfg._core_dump_url.ip != "")
    {
       cfg_value << "on_crash=" << "\"coredump-restart\""<<"\n";
    }
    else
    {
        cfg_value << "on_crash=" << "\"restart\""<<"\n";
    }

    if(!cfg._virt_type.compare("\"hvm\""))
    {
        if(true == Is64BitSystem())
        {
            cfg_value << "emulator=" << "\"/usr/lib64/xen/bin/qemu-dm\"" <<"\n";
        }
        else
        {
            cfg_value << "emulator=" << "\"/usr/lib/xen/bin/qemu-dm\"" <<"\n";
        }
    }
    
    cfg_value << "sdl=" << 0 <<"\n";
    // vnc����
    cfg_value << "vnc=" << 1<<"\n";
    cfg_value << "vncunused=" << 0<<"\n";
    cfg_value << "vnclisten=\"0.0.0.0\""<<"\n";
    cfg_value << "vncdisplay=" << to_string<int>((cfg._vnc_port-5900),dec) <<"\n";
    if (!cfg._vnc_passwd.empty())
    {
        cfg_value << "vncpasswd=\"" << cfg._vnc_passwd<<"\"\n"; 
    }
    // ��������
    cfg_value << "disk=" << GetAllDiskConfig(cfg)<<"\n"; 
    
    // ��������
    cfg_value << "vif =" << GetAllNormalNicCfg(cfg)<<"\n"; 
    

    // PCI�豸���ð���SRIOV����
    cfg_value << "pci =" << GetAllPciCfg(cfg)<<"\n";

    if(cfg._enable_serial)
    {
        // ��������
        cfg_value << "serial=\"telnet:0.0.0.0:" << to_string<int>(cfg._serial_port,dec)<<\
                           ",server,nowait\""<<"\n";
    }

    //�������
    cfg_value<<"usbdevice="<<"\"tablet\""<<"\n";

    
    return cfg_value.str();
}


/****************************************************************************************
* �������ƣ�InvertToVmState
* ����������ת��libvirt�������״̬��TECS��׼״̬
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         ����
****************************************************************************************/
VmInstanceState XenDriver::InvertToVmState(void *libxl_info)
{   // ˽�к���������ָ�����ΪNULL�ж�
    libxl_dominfo *state_info = (libxl_dominfo *)libxl_info;

#define SHUTDOWN_poweroff   0  /* Domain exited normally. Clean up and kill. */
#define SHUTDOWN_reboot     1  /* Clean up, kill, and then restart.          */
#define SHUTDOWN_suspend    2  /* Clean up, save suspend info, kill.         */
#define SHUTDOWN_crash      3  /* Tell controller we've crashed.             */
#define SHUTDOWN_watchdog   4  /* Restart because watchdog time expired.     */

    if (state_info->shutdown)
    {
        if (state_info->shutdown_reason == SHUTDOWN_crash)
        {
            return VM_CRASHED;
        }
        else if (state_info->shutdown_reason == SHUTDOWN_reboot)
        {
            return VM_RUNNING;
        }
        return VM_SHUTOFF;
    }
    else if (state_info->paused)
    {
        return VM_PAUSED;
    }
    
    return VM_RUNNING;
}

/****************************************************************************************
* �������ƣ�Create
* �����������������������ʵ��
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         ����
****************************************************************************************/
bool XenDriver::Create(const string& file_name, int *domid)
{
    Lock();
    *domid = main_create(file_name.c_str());
    Unlock();
    if (*domid == -1)
    {
        return false;
    }
    // �������ļ������ļ���������
    string instance_name;
    string desc;
    string value;
    ifstream fin(file_name.c_str());
    while (getline(fin, value))
    {
        string::size_type loc = value.find("name=");
        if (loc != string::npos)
        {
            instance_name = value.substr(loc+strlen("name="));
            loc = instance_name.find("\"");
            if (loc != string::npos)    instance_name.erase(loc,1); 
            loc = instance_name.find("\"");
            if (loc != string::npos)    instance_name.erase(loc,1); 
            continue;
        }
        loc = value.find("description=");
        if (loc != string::npos)
        {
            desc = value.substr(loc+strlen("description="));
            loc = desc.find("\"");
            if (loc != string::npos)    desc.erase(loc,1); 
            loc = desc.find("\"");
            if (loc != string::npos)    desc.erase(loc,1); 
            continue;
        }
    }
    fin.close();
    if (instance_name.empty())
    {
        // ����������ļ�����Ӧ��û����������Ȼ���������
        SkyExit(-1, "vm does not exist description");
    }
    SetDomainDescByName(instance_name, desc);
    return true;
}

/****************************************************************************************
* �������ƣ�Reboot
* �������������������ʵ��
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         ����
****************************************************************************************/
int XenDriver::Reboot(const string& instance_name)
{
    int domid = GetDomainIDByName(instance_name);
    if (domid == -1)
    {
        return -1;
    }
    Lock();
    int rc = reboot_domain(domid);
    Unlock();
    return rc;
}

/****************************************************************************************
* �������ƣ�Suspend
* ������������ͣ�����ʵ��
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         ����
****************************************************************************************/
int XenDriver::Suspend(const string& instance_name)
{
    int domid = GetDomainIDByName(instance_name);
    if (domid == -1)
    {
        return -1;
    }
    Lock();
    int rc = pause_domain(domid);
    Unlock();
    return rc;
}

/****************************************************************************************
* �������ƣ�Resume
* �����������ָ����������ʵ��
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         ����
****************************************************************************************/
int XenDriver::Resume(const string& instance_name)
{
    int domid = GetDomainIDByName(instance_name);
    if (domid == -1)
    {
        return -1;
    }
    Lock();
    int rc = unpause_domain(domid);
    Unlock();
    return rc;
}

/****************************************************************************************
* �������ƣ�Shutdown
* �����������ر����������ʵ��
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         ����
****************************************************************************************/
int XenDriver::ShutDown(const string& instance_name)
{
    int domid = GetDomainIDByName(instance_name);
    if (domid == -1)
    {
        return -1;
    }
    Lock();
    if(-1 != main_shutdown(domid))
    {
        Unlock();
        remove((libxl_vm_desc_dir+instance_name).c_str());
        return 0;
    }
    Unlock();
    return -1;
}

/****************************************************************************************
* �������ƣ�Reset
* ���������������ǿ����������
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/06/1    V1.0     lixiaocheng        ����
****************************************************************************************/
int XenDriver::Reset(const string& instance_name, const string& cfg_file_name)
{
    int domid = GetDomainIDByName(instance_name);
    if (domid == -1)
    {
        return -1;
    }
    Lock();
    if(0 == main_destroy(domid))
    {
        if(-1 != main_create(cfg_file_name.c_str()))
        {
            Unlock();
            return 0;
        }
    }
    Unlock();
    return -1;
}

/****************************************************************************************
* �������ƣ�VcpuAffinitySet
* ��������������CPU��bind�ӿ�,�������CPU����ȫCPU��bind����
* ���ʵı���
* �޸ĵı���
* ���������domid  �����id    VcpuNum  ����CPU����   
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/02/12    V1.0    lxc         ����
****************************************************************************************/
int XenDriver::VcpuAffinitySet(const string &dom_name, unsigned int VcpuNum)
{
    int domid = GetDomainIDByName(dom_name);
    if (domid == -1)
    {
        return -1;
    }
    char *pcpu = "all";
    Lock();
    int rc = vcpupin(domid, "all", pcpu);
    Unlock();
    return rc;
}

/****************************************************************************************
* �������ƣ�Sched_credit_weight_set
* ���������������Ȩ������
* ���ʵı���
* �޸ĵı���
* ���������domid  �����id    percent Ȩ�ذٷֱ�
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/02/12    V1.0    lxc         ����
****************************************************************************************/
int XenDriver::Sched_credit_weight_set(int id, unsigned int percent)
{
    if (percent > 100)
    {
        percent = 100;
    }
    
    Lock();
    int rc = sched_credit_weight_set(id, PCPU_WEIGHT_TOTAL*percent/100);
    Unlock();
    return rc;
}


/****************************************************************************************
* �������ƣ�Sched_credit_weight_set
* ���������������Ȩ������
* ���ʵı���
* �޸ĵı���
* ���������domid  �����id    percent Ȩ�ذٷֱ�
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/02/12    V1.0    lxc         ����
****************************************************************************************/
int XenDriver::Sched_credit_weight_set(const string &dom_name, unsigned int percent)
{
    int domid = GetDomainIDByName(dom_name);
    if (domid == -1)
    {
        return -1;
    }
    return Sched_credit_weight_set(domid, percent);
}


/****************************************************************************************
* �������ƣ�credit_cap_set
* ��������������CAP����
* ���ʵı���
* �޸ĵı���
* ���������domid  �����id    CoreNum  ����  percent Ȩ�ذٷֱ�
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/02/12    V1.0    lxc         ����
****************************************************************************************/
int XenDriver::credit_cap_set(const string &dom_name, int percent)
{
    int domid = GetDomainIDByName(dom_name);
    if (domid == -1)
    {
        return -1;
    }
    Lock();
    int rc = sched_credit_cap_set(domid, percent);
    Unlock();
    return rc;
}

/****************************************************************************************
* �������ƣ�Sched_credit_cap_set
* �����������������cap����
* ���ʵı���
* �޸ĵı���
* ���������domid  �����id    CoreNum  ����  percent Ȩ�ذٷֱ�
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/02/12    V1.0    lxc         ����
****************************************************************************************/
int XenDriver::Sched_credit_cap_set(const string &dom_name, int CoreNum, unsigned int percent)
{
    if (percent > 100)
    {
        percent = 100;
    }

    // ���CPU��Ȩ�ذٷֱ�Ϊ100����ôΪ�˶�ʱ����׼ʱ��
    // ��Ҫ���������Ȩ������Ϊ0�������CPUȨ��Ϊ0����
    // ���������Ϻ͵�����100���ƣ������ڹ����ɵ���ʱ
    // ���нϺõ��ж���Ӧ
    if (percent == 100)
    {
        return credit_cap_set(dom_name, 0);
    }

    return credit_cap_set (dom_name, CoreNum*percent);
}



/****************************************************************************************
* �������ƣ�Migrate
* ����������Ǩ�������ʵ��
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2011/12/31    V1.0    lixch         ����
****************************************************************************************/
int XenDriver::Migrate(const string& instance_name, const string& host_ip)
{
    int domid = GetDomainIDByName(instance_name);
    if (domid == -1)
    {
        return -1;
    }
    Lock();
    int rc = main_migrate(domid, host_ip.c_str());
    Unlock();
    return rc;
}

/****************************************************************************************
* �������ƣ�GetDomainInfo
* ��������������ʵ������������ʵ���������Ϣ
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         ����
****************************************************************************************/
int XenDriver::GetDomainInfo(const string& dom_name, VmDomainInfo &DomInfo)
{
    libxl_dominfo info;
    int domid = GetDomainIDByName(dom_name);
    if (domid == -1)
    {
        return -1;
    }

    Lock();
    if (0 != domain_info_show(domid, info))
    {  Unlock();
        return -1;
    }
    Unlock();
    DomInfo.cpuTime = info.cpu_time/1e9;
    DomInfo.maxMem = info.current_memkb;
    DomInfo.memory = info.current_memkb;
    DomInfo.nrVirtCpu = info.vcpu_online;
    DomInfo.state = InvertToVmState(&info);
    return 0;
}


/****************************************************************************************
* �������ƣ�ListDomains
* �����������г���ǰ���е������ʵ��
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         ����
****************************************************************************************/
int XenDriver::ListDomains(int* ids)
{
    Lock();
    int rc = list_vm(ids);
    Unlock();
    return rc;
}

/****************************************************************************************
* �������ƣ�GetDomainNameByDomid
* ��������������ʵ��������ʵ����
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         ����
****************************************************************************************/
string XenDriver::GetDomainNameByDomid(int id)
{
    Lock();
    string rc = main_domname(id);
    Unlock();
    return rc;
}

/****************************************************************************************
* �������ƣ�GetDomainNetInfoById
* ��������������ʵ��������ʵ����
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         ����
****************************************************************************************/
int XenDriver::GetDomainNetInfoById(int domid, uint32 nic_no, 
                                   VmDomainInterfaceStatsStruct& info)
{
    string         nic_path = "vif" + to_string<int64>(domid,dec) \
                              +"." + to_string<int>(nic_no,dec);

    int rc = linuxDomainInterfaceStats(nic_path.c_str(), info);
    return rc;

}


int XenDriver::linuxDomainInterfaceStats(const char *path,
                                      VmDomainInterfaceStatsStruct &stats)
{
    int path_len;
    FILE *fp;
    char line[256], *colon;

    fp = fopen ("/proc/net/dev", "r");
    if (!fp) {
        OutPut(SYS_ERROR, "Could not open /proc/net/dev erron %d\n", errno);
        return -1;
    }

    path_len = strlen (path);

    while (fgets (line, sizeof line, fp)) {
        long long dummy;
        long long rx_bytes;
        long long rx_packets;
        long long rx_errs;
        long long rx_drop;
        long long tx_bytes;
        long long tx_packets;
        long long tx_errs;
        long long tx_drop;

        /* The line looks like:
         *   "   eth0:..."
         * Split it at the colon.
         */
        colon = strchr (line, ':');
        if (!colon) continue;
        *colon = '\0';
        if (colon-path_len >= line &&
            STREQ (colon-path_len, path)) {
            /* IMPORTANT NOTE!
             * /proc/net/dev vif<domid>.nn sees the network from the point
             * of view of dom0 / hypervisor.  So bytes TRANSMITTED by dom0
             * are bytes RECEIVED by the domain.  That's why the TX/RX fields
             * appear to be swapped here.
             */
            if (sscanf (colon+1,
                        "%lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld",
                        &tx_bytes, &tx_packets, &tx_errs, &tx_drop,
                        &dummy, &dummy, &dummy, &dummy,
                        &rx_bytes, &rx_packets, &rx_errs, &rx_drop,
                        &dummy, &dummy, &dummy, &dummy) != 16)
                continue;

            stats.rx_bytes = rx_bytes;
            stats.rx_packets = rx_packets;
            stats.rx_errs = rx_errs;
            stats.rx_drop = rx_drop;
            stats.tx_bytes = tx_bytes;
            stats.tx_packets = tx_packets;
            stats.tx_errs = tx_errs;
            stats.tx_drop = tx_drop;
            fclose (fp);

            return 0;
        }
    }
    fclose(fp);

    OutPut(SYS_ERROR, "/proc/net/dev: Interface not found\n");
    return -1;
}

/****************************************************************************************
* �������ƣ�GetDomainState
* ��������������ʵ�������������ʵ��״̬
* ���ʵı���
* �޸ĵı���
* ���������id   domain ID
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    jixp         ����
****************************************************************************************/
VmInstanceState XenDriver::GetDomainState(const string& name)
{
    VmDomainInfo DomInfo;
    
    if (-1 == GetDomainInfo(name, DomInfo))
    {
        return VM_SHUTOFF;
    }
    return DomInfo.state;
;
}

/****************************************************************************************
* �������ƣ�GetDomainIDByName
* ��������������ʵ�������domid
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         ����
****************************************************************************************/
int XenDriver::GetDomainIDByName(const string& domname)
{
    Lock();
    int rc = main_domid(domname.c_str());
    Unlock();
    return rc;
}


/****************************************************************************************
* �������ƣ�GetDomainDescByName
* ��������������ʵ������ø�ʵ����������Ϣ
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/08/27    V1.0    lixch        ����
****************************************************************************************/
string XenDriver::GetDomainDescByName(const string& domname)
{
    ifstream fin((libxl_vm_desc_dir+domname).c_str());
    string value;

    getline(fin, value);
    fin.close();
    return value;
}

/****************************************************************************************
* �������ƣ�GetDomainDescByName
* ��������������ʵ������ø�ʵ����������Ϣ
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵�����ýӿ�û��ʹ�ã��ݲ�ʵ��
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/08/27    V1.0    lixch        ����
****************************************************************************************/
int XenDriver::SetDomainDescByName(const string& domname, const string& desc)
{
    ofstream file;
    string file_name=libxl_vm_desc_dir+domname;

    file.open(file_name.c_str(), ios::out);
    if (file.fail() == true)
    {
        OutPut(SYS_ERROR,"SetDomainDescByName: write libxl file file:%s\n", file_name.c_str());
        return -1;
    }
    file << desc << endl;
    file.close();

    return SUCCESS;   
}

/****************************************************************************************
* �������ƣ�DestroyDomain
* ��������������domain ID ����ֹ������Ӧ�������
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         ����
****************************************************************************************/
int XenDriver::DestroyDomain(const string &dom_name)
{  
    int domid = GetDomainIDByName(dom_name);
    if (domid == -1)
    {
        return -1;
    }
    Lock();
    if(-1 != main_destroy(domid))
    {   
        Unlock();
        string desc_file = libxl_vm_desc_dir+dom_name;
        remove(desc_file.c_str());
        return 0;
    }
    Unlock();
    return -1;
}

// �����������USB��
int XenDriver::AddUsb(const string &dom_name, int vendor_id, int product_id)
{
    ostringstream cmd;
    cmd <<  "xm usb-add  " << dom_name <<" host:" << to_string(vendor_id, hex) << ":" << to_string(product_id, hex);
    return RunCmd(cmd.str());
}

// ��������γ�USB��
int XenDriver::DelUsb(const string &dom_name, int vendor_id, int product_id)
{
    ostringstream cmd;
    cmd <<  "xm usb-del  " << dom_name <<" host:" << to_string(vendor_id, hex) << ":" << to_string(product_id, hex);
    return RunCmd(cmd.str());
}

// ��������γ�USB��
int XenDriver::DelAndAddUsb(const string &dom_name, int vendor_id, int product_id)
{
    DelUsb(dom_name, vendor_id, product_id);
    return AddUsb(dom_name, vendor_id, product_id);
}


#endif //__XEN_DEFINE__

