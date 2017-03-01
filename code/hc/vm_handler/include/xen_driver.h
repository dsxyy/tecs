/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：xen_driver.h
* 文件标识：
* 内容摘要：XenDriver类的定义文件
* 当前版本：1.0
* 作    者：lixch
* 完成日期：2011年7月25日
*
* 修改记录1：
*     修改日期：2013/3/25
*     版 本 号：V1.0
*     修 改 人：lixch
*     修改内容：创建
*******************************************************************************/
#ifndef HC_XENDRIVER_H
#define HC_XENDRIVER_H

#include "vm_driver_base.h"

using namespace std;




/************************************************************************************
 *                    XEN虚拟化驱动                                                    *
 ***********************************************************************************/
class XenDriver:public VmDriverBase
{
public:
    //构造函数
    XenDriver()
    {
    };

    //析构函数
    ~XenDriver()
    {
    };
    
    // 下面函数都是虚函数，所有实现，均可以被子类替换
    // 类初始化
    int Init();

    // 创建配置文件
    string CreateConfig(const string vm_ins_name, VMConfig &cfg);
    
    // 创建虚拟机接口
    bool Create(const string& xml, int *domid);

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
    int AddUsb(const string &dom_name, int vendor_id, int product_id);
   
    // 从虚拟机拔出USB盘
    int DelUsb(const string &dom_name, int vendor_id, int product_id);
    
    // 从虚拟机拔出USB盘
    int DelAndAddUsb(const string &dom_name, int vendor_id, int product_id);

    int AddRemovableDisk(const string& instance_name,  const VmDiskConfig &disk)
    {
        return SUCCESS;
    };

    int DelRemovableDisk(const string& instance_name,  const VmDiskConfig &disk)
    {
        return SUCCESS;
    };
    
private:
    VmInstanceState InvertToVmState(void *libxl_info);
    int credit_cap_set(const string &dom_name, int percent);
    void GetDiskConfig(ostringstream &disk_cfg, const VmDiskConfig &disk);
    void GetNormalVifConfig(ostringstream &vif_cfg, const VmNicConfig &nic, VMConfig &cfg);
    string GetAllNormalNicCfg(VMConfig &vm_cfg);
    string GetAllDiskConfig(VMConfig &cfg);
    void GetAllSriovConfig(ostringstream &vif_cfg, VMConfig &cfg);
    void GetSRIOVConfig(ostringstream &vif_cfg, const VmNicConfig &nic);
    string GetAllPciCfg(VMConfig &cfg);
    int linuxDomainInterfaceStats(const char *path,
                                      VmDomainInterfaceStatsStruct &stats);
    
};



#endif // #ifndef HC_XENDRIVER_H


