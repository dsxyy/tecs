/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：vm_driver.h
* 文件标识：
* 内容摘要：VmDriver类的定义文件
* 当前版本：1.0
* 作    者：lixch
* 完成日期：2011年7月25日
*
*******************************************************************************/

#ifndef HC_VMDRIVER_H
#define HC_VMDRIVER_H

#include "vm_pub.h"
#include "vm_driver_base.h"


using namespace std;

/************************************************************************************
 *                    虚拟化驱动                                                    *
 ***********************************************************************************/

class VmDriver
{
public:
    static VmDriver *GetInstance()
    {
        if(NULL == instance)
        {
            instance = new VmDriver(); 
        }
        return instance;
    };
   
    ~VmDriver() 
    {
        instance = NULL;
        delete impl;
        impl = NULL;
    };
    
    void Init();
    
    // 创建配置文件
    string CreateConfig(const string vm_ins_name, VMConfig &cfg)
    {
        return impl->CreateConfig(vm_ins_name, cfg);
    }

    // 创建配置文件
    bool Create(const string& xml, int *domid)
    {
        return impl->Create(xml, domid);
    }
    
    // 设置虚拟机权重值上限接口
    int Sched_credit_cap_set(const string &instance_name, int CoreNum, unsigned int percent)
    {
        return impl->Sched_credit_cap_set(instance_name, CoreNum, percent);
    }

    // 设置调度权重值接口
    int Sched_credit_weight_set(const string &instance_name, unsigned int percent)
    {
        return impl->Sched_credit_weight_set(instance_name, percent);
    }

    // 设置调度权重值接口
    int Sched_credit_weight_set(int id, unsigned int percent)
    {
        return impl->Sched_credit_weight_set(id, percent);
    }

    // 设置虚拟机全局不bind功能
    int VcpuAffinitySet(const string &instance_name, unsigned int VcpuNum)
    {
        return impl->VcpuAffinitySet(instance_name, VcpuNum);
    }

    // 关机
    int ShutDown(const string& instance_name)
    {
        return impl->ShutDown(instance_name);
    }

    // 挂起
    int Suspend(const string& instance_name)
    {
        return impl->Suspend(instance_name);
    }

    // 恢复
    int Resume(const string& instance_name)
    {
        return impl->Resume(instance_name);
    }

    // 重启
    int Reboot(const string& instance_name)
    {
        return impl->Reboot(instance_name);
    }

    // 强制重启
    int Reset(const string& instance_name, const string& cfg_file_name)
    {
        return impl->Reset(instance_name, cfg_file_name);
    }
    
    // 迁移虚拟机
    int Migrate(const string& instance_name, const string& host_ip)
    {
        return impl->Migrate(instance_name, host_ip);
    }

    // 获取domin的信息，这个是强相关信息，需要定义成自己的结构进行传递
    int GetDomainInfo(const string& instance_name, VmDomainInfo &DomInfo)
    {
        return impl->GetDomainInfo(instance_name, DomInfo);
    }

    // 获取虚拟机状态
    VmInstanceState GetDomainState(const string& name)
    {
        return impl->GetDomainState(name);
    }

    // 列出所有虚拟机
    int ListDomains(int* ids)
    {
        return impl->ListDomains(ids);
    }

    // 通过虚拟机的的ID获取虚拟机名称
    string GetDomainNameByDomid(int id)
    {
        return impl->GetDomainNameByDomid(id);
    }

    // 通过虚拟机ID，获取虚拟机网络信息
    int GetDomainNetInfoById(int domid, uint32 nic_no, 
                             VmDomainInterfaceStatsStruct& info)
    {
        return impl->GetDomainNetInfoById(domid, nic_no, info);
    }

    // 通过虚拟机名称获取domid
    int GetDomainIDByName(const string& instance_name)
    {
        return impl->GetDomainIDByName(instance_name);
    }

    // 通过虚拟机名称获取描述信息
    string GetDomainDescByName(const string& instance_name)
    {
        return impl->GetDomainDescByName(instance_name);
    }
    
    // 通过虚拟机名称设置虚拟机描述信息
    int SetDomainDescByName(const string& instance_name, const string& desc)
    {
        return impl->SetDomainDescByName(instance_name, desc);
    }

    // 强制关闭虚拟机
    int DestroyDomain(const string &instance_name)
    {
        return impl->DestroyDomain(instance_name);
    }

    // 向虚拟机插入USB盘
    int AddUsb(const string &instance_name, int vendor_id, int product_id)
    {
        return impl->AddUsb(instance_name, vendor_id, product_id);
    };
   
    // 从虚拟机拔出USB盘
    int DelUsb(const string &instance_name, int vendor_id, int product_id)
    {
        return impl->DelUsb(instance_name, vendor_id, product_id);
    };
    
    // 从虚拟机拔出USB盘
    int DelAndAddUsb(const string &instance_name, int vendor_id, int product_id)
    {
        return impl->AddUsb(instance_name, vendor_id, product_id);
    };
    //enum DriverType GetDriverType(void);    /*  modify by wangtl for driver type  */
    int AddRemovableDisk(const string& instance_name, const VmDiskConfig &disk)
    {
        return impl->AddRemovableDisk(instance_name, disk);
    };

    int DelRemovableDisk(const string& instance_name, const VmDiskConfig &disk)
    {
        return impl->DelRemovableDisk(instance_name, disk);
    };

    /*   0 : not exist, 1 exist, -1 unkown     */
    int IsDomExist(const string& instance_name, int domain_id)
    {
        return impl->IsDomExist(instance_name, domain_id);
    };
    
private:
    VmDriver()
    {
        impl = NULL;
        _is_init = false;
    }
    DISALLOW_COPY_AND_ASSIGN(VmDriver);
    static VmDriver *instance;
    VmDriverBase    *impl;
    bool _is_init;
    
};



#endif // #ifndef HC_VMDRIVER_H


