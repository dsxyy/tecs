/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：kvm_driver.h
* 文件标识：
* 内容摘要：KvmDriver类的定义文件
* 当前版本：1.0
* 作    者       ：wangtl
* 完成日期：2013年5月15日
*
* 修改记录1：
*     修改日期：2013/5/15
*     版 本 号：V1.0
*     修 改 人：wangtl
*     修改内容：创建
*******************************************************************************/
#if 0 /* 合并到libvirt_driver */
#ifndef HC_KVMDRIVER_H
#define HC_KVMDRIVER_H

#include "vm_driver_base.h"

#include <libvirt/libvirt.h>
#include <libvirt/virterror.h>

using namespace std;

/************************************************************************************
 *                    虚拟化驱动                                                    *
 ***********************************************************************************/
class KvmDriver:public VmDriverBase
{

public:
    //构造函数
    KvmDriver()
    {
        _connect  = NULL;
        _pcpu_num = 0;
    };

    //析构函数
    ~KvmDriver()
    {
    };
    
    // libvirt状态
    enum LibVirtVmState
    {
        /* libvirt state */
        LIBV_NO_STATE = 0, 
        LIBV_RUNNING  = 1,
        LIBV_BLOCKED  = 2,
        LIBV_PAUSED   = 3,
        LIBV_SHUTDOWN = 4,
        LIBV_SHUTOFF  = 5,
        LIBV_CRASHED  = 6,
    };
    
    // 下面函数都是虚函数，所有实现，均可以被子类替换
    // 类初始化
    int Init();

    // 创建配置文件
    string CreateConfig(const string vm_ins_name, VMConfig &cfg);
    
    // 创建虚拟机接口
    bool Create(const string& filename, int *domid);

    // 设置虚拟机权重值上限接口
    int Sched_credit_cap_set(const string &dom_name, int CoreNum, unsigned int percent);

    // 设置调度权重值接口
    int Sched_credit_weight_set(const string &dom_name, unsigned int percent);

    // 设置调度权重值接口
    int Sched_credit_weight_set(int id, unsigned int percent);

    // 设置虚拟机全局不bind功能
    int VcpuAffinitySet(const string &dom_name, unsigned int VcpuNum);

    // 关机
    int ShutDown(const string& instance_name);

    // 挂起
    int Suspend(const string& instance_name);

    // 恢复
    int Resume(const string& instance_name);

    // 重启
    int Reboot(const string& instance_name);

    // 强制重启
    int Reset(const string& instance_name, const string& cfg_file_name);

    // 迁移虚拟机
    int Migrate(const string& instance_name, const string& host_ip);

    // 获取domin的信息，这个是强相关信息，需要定义成自己的结构进行传递
    int GetDomainInfo(const string& dom_name, VmDomainInfo &DomInfo);

    // 获取虚拟机状态
    VmInstanceState GetDomainState(const string& name);

    // 列出所有虚拟机
    int ListDomains(int* ids);

    // 通过虚拟机的的ID获取虚拟机名称
    string GetDomainNameByDomid(int id);

    // 通过虚拟机ID，获取虚拟机网络信息
    int GetDomainNetInfoById(int domid, uint32 nic_no, 
                             VmDomainInterfaceStatsStruct& info);

    // 通过虚拟机名称获取domid
    int GetDomainIDByName(const string& domname);

    // 通过虚拟机名称获取描述信息
    string GetDomainDescByName(const string& domname);
    
    // 通过虚拟机名称设置虚拟机描述信息
    int SetDomainDescByName(const string& domname, const string& desc);

    // 强制关闭虚拟机
    int DestroyDomain(const string &dom_name);

    // 向虚拟机插入USB盘
    int AddUsb(const string &instance_name, int vendor_id, int product_id);
   
    // 从虚拟机拔出USB盘
    int DelUsb(const string &instance_name, int vendor_id, int product_id);
    
    // 从虚拟机拔出USB盘
    int DelAndAddUsb(const string &instance_name, int vendor_id, int product_id);

    int AddRemovableDisk(const string& instance_name,  const VmDiskConfig &disk);

    int DelRemovableDisk(const string& instance_name,  const VmDiskConfig &disk);
    
private:
    string             _uri;                 /* libvirt访问hypervisor的URI */
    virConnectPtr      _connect;             /* libvirt访问hypervisor的连接句柄 */
    uint32             _pcpu_num;
    
    VmInstanceState InvertToVmState(int state_in);
    virConnectPtr      CheckHypervisorConnect();
    
    // 获取虚拟机的链接指针
    virConnectPtr GetConn(const char* ip);
        
    // 获取domin的信息
    int GetDomainInfo(virDomainPtr dom, virDomainInfo *pDomInfo);
    
    // 通过虚拟机名称获取虚拟机控制指针
    virDomainPtr GetDomainByInstanceName(const string &name);

    // 释放虚拟机控制指针
    void FreeDomain(virDomainPtr dom);
    
    bool cap_set_zero_with_syscmd(const string &dom_name);
    
    // 内部使用的函数
    void CreateNomalNicXml(XmlParser &xml, const VmNicConfig &nic, VMConfig &vm_cfg);
    void CreateSriovNicXml(XmlParser &xml, const VmNicConfig &nic, VMConfig &vm_cfg);
    void CreateNicXml(XmlParser &xml, VMConfig &vm_cfg);
    void CreateWdgXml(XmlParser & xml, VMConfig &vm_cfg);
    void CreateSerialXml(XmlParser & xml, VMConfig &vm_cfg);
    void CreateVncXml(XmlParser &xml, VMConfig &vm_cfg);
    void CreatePciDeviceXml(XmlParser &xml, VMConfig &vm_cfg);
    void CreateDiskXml(XmlParser & xml, const VmDiskConfig & disk, VMConfig &vm_cfg);
    void CreateAllDiskXml(XmlParser & xml, VMConfig &vm_cfg);
    void CreateMachineXml(XmlParser & xml, VMConfig &vm_cfg);
    void CreateemulatorXml(XmlParser & xml, VMConfig &vm_cfg);
    void CreateFeaturesXml(XmlParser & xml, VMConfig &vm_cfg);
    void CreateOsXml(XmlParser & xml, VMConfig &vm_cfg);
    void CreateMouseXml(XmlParser & xml, VMConfig &vm_cfg);
    void WritePciDeviceXml(XmlParser &xml, string domain, string bus, string slot, string function);
};



#endif // #ifndef HC_VMDRIVER_H

#endif
