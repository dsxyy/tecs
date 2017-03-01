/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：vm_driver.cpp
* 文件标识：
* 内容摘要：XenDriver类的实现文件
* 当前版本：1.0
* 作    者：lixch
* 完成日期：2011年7月25日
*******************************************************************************/
#ifdef __XEN_DEFINE__

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string> 
#include <iostream>
#include <stdio.h>
#include "libxl_impl.h"       // 该头文件和xen_driver.h 顺序不能替换
#include "xen_driver.h"


/*****************************************************************************
 *                               全局变量                                    *
 ****************************************************************************/
#define PCPU_WEIGHT_TOTAL 10000
#define libxl_vm_desc_dir        "/var/run/libxl/"
#define STREQ(a,b) (strcmp(a,b) == 0)

/****************************************************************************************
* 函数名称：Init
* 功能描述：初始化虚拟化驱动
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         创建
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
* 函数名称：GetDiskConfig
* 功能描述：获取DISK的配置文件
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         创建
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
* 函数名称：GetNormalVifConfig
* 功能描述：获取普通网口的配置
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         创建
****************************************************************************************/
void XenDriver::GetNormalVifConfig(ostringstream &vif_cfg, const VmNicConfig &nic, VMConfig &cfg)
{
    vif_cfg << " \"";
    vif_cfg << "mac=" << nic._mac << ",";
    vif_cfg << "bridge="<<nic._bridge;
    if ((!cfg._virt_type.compare(VIRT_TYPE_HVM))  /* HVM ,PVM不填写*/
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
* 函数名称：GetAllDiskConfig
* 功能描述：获取所有磁盘的配置文件
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         创建
****************************************************************************************/
string XenDriver::GetAllDiskConfig(VMConfig &cfg)
{
    ostringstream disk_cfg;

    disk_cfg << "[";

    // 把映像盘加入到disk列表中
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
* 函数名称：GetAllSriovConfig
* 功能描述：获取所有SRIOV的配置
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         创建
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
* 函数名称：GetSRIOVConfig
* 功能描述：获取单个SRIOV的配置
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         创建
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
* 函数名称：GetAllPciCfg
* 功能描述：创建所有PCI设备配置
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         创建
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
* 函数名称：CreateConfig
* 功能描述：创建配置文件
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         创建
****************************************************************************************/
string XenDriver::CreateConfig(const string vm_ins_name, VMConfig &cfg)
{
    ostringstream cfg_value;

    // 写基本配置
    // pvm参数暂不实现，先实现HVM
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
    // vnc配置
    cfg_value << "vnc=" << 1<<"\n";
    cfg_value << "vncunused=" << 0<<"\n";
    cfg_value << "vnclisten=\"0.0.0.0\""<<"\n";
    cfg_value << "vncdisplay=" << to_string<int>((cfg._vnc_port-5900),dec) <<"\n";
    if (!cfg._vnc_passwd.empty())
    {
        cfg_value << "vncpasswd=\"" << cfg._vnc_passwd<<"\"\n"; 
    }
    // 磁盘配置
    cfg_value << "disk=" << GetAllDiskConfig(cfg)<<"\n"; 
    
    // 网口配置
    cfg_value << "vif =" << GetAllNormalNicCfg(cfg)<<"\n"; 
    

    // PCI设备配置包括SRIOV网口
    cfg_value << "pci =" << GetAllPciCfg(cfg)<<"\n";

    if(cfg._enable_serial)
    {
        // 串口配置
        cfg_value << "serial=\"telnet:0.0.0.0:" << to_string<int>(cfg._serial_port,dec)<<\
                           ",server,nowait\""<<"\n";
    }

    //鼠标配置
    cfg_value<<"usbdevice="<<"\"tablet\""<<"\n";

    
    return cfg_value.str();
}


/****************************************************************************************
* 函数名称：InvertToVmState
* 功能描述：转换libvirt的虚拟机状态到TECS标准状态
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         创建
****************************************************************************************/
VmInstanceState XenDriver::InvertToVmState(void *libxl_info)
{   // 私有函数，不对指针进行为NULL判断
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
* 函数名称：Create
* 功能描述：创建运行虚拟机实例
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         创建
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
    // 从配置文件解析文件明和描述
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
        // 虚拟机创建文件，不应该没有描述，不然问题很严重
        SkyExit(-1, "vm does not exist description");
    }
    SetDomainDescByName(instance_name, desc);
    return true;
}

/****************************************************************************************
* 函数名称：Reboot
* 功能描述：重启虚拟机实例
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         创建
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
* 函数名称：Suspend
* 功能描述：暂停虚拟机实例
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         创建
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
* 函数名称：Resume
* 功能描述：恢复运行虚拟机实例
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         创建
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
* 函数名称：Shutdown
* 功能描述：关闭运行虚拟机实例
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         创建
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
* 函数名称：Reset
* 功能描述：虚拟机强制重启命令
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/06/1    V1.0     lixiaocheng        创建
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
* 函数名称：VcpuAffinitySet
* 功能描述：虚拟CPU的bind接口,对虚拟机CPU进行全CPU不bind操作
* 访问的表：无
* 修改的表：无
* 输入参数：domid  虚拟机id    VcpuNum  虚拟CPU数量   
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/02/12    V1.0    lxc         创建
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
* 函数名称：Sched_credit_weight_set
* 功能描述：虚拟机权重设置
* 访问的表：无
* 修改的表：无
* 输入参数：domid  虚拟机id    percent 权重百分比
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/02/12    V1.0    lxc         创建
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
* 函数名称：Sched_credit_weight_set
* 功能描述：虚拟机权重设置
* 访问的表：无
* 修改的表：无
* 输入参数：domid  虚拟机id    percent 权重百分比
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/02/12    V1.0    lxc         创建
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
* 函数名称：credit_cap_set
* 功能描述：调度CAP设置
* 访问的表：无
* 修改的表：无
* 输入参数：domid  虚拟机id    CoreNum  核数  percent 权重百分比
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/02/12    V1.0    lxc         创建
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
* 函数名称：Sched_credit_cap_set
* 功能描述：虚拟机的cap设置
* 访问的表：无
* 修改的表：无
* 输入参数：domid  虚拟机id    CoreNum  核数  percent 权重百分比
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/02/12    V1.0    lxc         创建
****************************************************************************************/
int XenDriver::Sched_credit_cap_set(const string &dom_name, int CoreNum, unsigned int percent)
{
    if (percent > 100)
    {
        percent = 100;
    }

    // 如果CPU的权重百分比为100，那么为了定时器的准时性
    // 需要把虚拟机的权重设置为0，虚拟机CPU权重为0，在
    // 调度上限上和单个核100相似，但是在过负荷调度时
    // 会有较好的中断响应
    if (percent == 100)
    {
        return credit_cap_set(dom_name, 0);
    }

    return credit_cap_set (dom_name, CoreNum*percent);
}



/****************************************************************************************
* 函数名称：Migrate
* 功能描述：迁移虚拟机实例
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/12/31    V1.0    lixch         创建
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
* 函数名称：GetDomainInfo
* 功能描述：根据实例名获得虚拟机实例的相关信息
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         创建
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
* 函数名称：ListDomains
* 功能描述：列出当前运行的虚拟机实例
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         创建
****************************************************************************************/
int XenDriver::ListDomains(int* ids)
{
    Lock();
    int rc = list_vm(ids);
    Unlock();
    return rc;
}

/****************************************************************************************
* 函数名称：GetDomainNameByDomid
* 功能描述：根据实例句柄获得实例名
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         创建
****************************************************************************************/
string XenDriver::GetDomainNameByDomid(int id)
{
    Lock();
    string rc = main_domname(id);
    Unlock();
    return rc;
}

/****************************************************************************************
* 函数名称：GetDomainNetInfoById
* 功能描述：根据实例句柄获得实例名
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         创建
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
* 函数名称：GetDomainState
* 功能描述：根据实例句柄获得虚拟机实例状态
* 访问的表：无
* 修改的表：无
* 输入参数：id   domain ID
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    jixp         创建
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
* 函数名称：GetDomainIDByName
* 功能描述：根据实例名获得domid
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         创建
****************************************************************************************/
int XenDriver::GetDomainIDByName(const string& domname)
{
    Lock();
    int rc = main_domid(domname.c_str());
    Unlock();
    return rc;
}


/****************************************************************************************
* 函数名称：GetDomainDescByName
* 功能描述：根据实例名获得该实例的描述信息
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/08/27    V1.0    lixch        创建
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
* 函数名称：GetDomainDescByName
* 功能描述：根据实例名获得该实例的描述信息
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：该接口没有使用，暂不实现
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/08/27    V1.0    lixch        创建
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
* 函数名称：DestroyDomain
* 功能描述：根据domain ID 制终止运行相应的虚拟机
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         创建
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

// 向虚拟机插入USB盘
int XenDriver::AddUsb(const string &dom_name, int vendor_id, int product_id)
{
    ostringstream cmd;
    cmd <<  "xm usb-add  " << dom_name <<" host:" << to_string(vendor_id, hex) << ":" << to_string(product_id, hex);
    return RunCmd(cmd.str());
}

// 从虚拟机拔出USB盘
int XenDriver::DelUsb(const string &dom_name, int vendor_id, int product_id)
{
    ostringstream cmd;
    cmd <<  "xm usb-del  " << dom_name <<" host:" << to_string(vendor_id, hex) << ":" << to_string(product_id, hex);
    return RunCmd(cmd.str());
}

// 从虚拟机拔出USB盘
int XenDriver::DelAndAddUsb(const string &dom_name, int vendor_id, int product_id)
{
    DelUsb(dom_name, vendor_id, product_id);
    return AddUsb(dom_name, vendor_id, product_id);
}


#endif //__XEN_DEFINE__

