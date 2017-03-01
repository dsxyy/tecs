/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：vm_driver_base.h
* 文件标识：
* 内容摘要：VmDriver类的定义文件
* 当前版本：1.0
* 作    者：lixch
* 完成日期：2011年7月25日
*
*******************************************************************************/

#ifndef HC_VMDRIVER_BASE_H
#define HC_VMDRIVER_BASE_H

#include "vm_messages.h"

#include "vm_pub.h"

/************************************************************************************
 *                    虚拟化驱动基础接口类 使用到的参数                                      *
 ***********************************************************************************/
namespace zte_tecs
{
/* 虚拟机实例的状态 */
/* 修改本enum时必须同步修改vmcfg_api.h中的ApiVmUserState状态!!! */

enum VmmType
{
    XEN = 0,
    LIBVIRT = 1,
    KVM = 2,
    VMWARE = 3,
    SIMULATE = 4,
    UNKNOWN_DRIVER_TYPE
};

class VmDomainInfo
{
public:
    VmInstanceState state;        /* the running state, one of virDomainState */
    unsigned long maxMem;       /* the maximum memory in KBytes allowed */
    unsigned long memory;       /* the memory in KBytes used by the domain */
    unsigned short nrVirtCpu;   /* the number of virtual CPUs for the domain */
    unsigned long long cpuTime; /* the CPU time used in nanoseconds */
};

class VmDomainInterfaceStatsStruct
{
public:
  long long rx_bytes;
  long long rx_packets;
  long long rx_errs;
  long long rx_drop;
  long long tx_bytes;
  long long tx_packets;
  long long tx_errs;
  long long tx_drop;
};

/************************************************************************************
 *                    虚拟化驱动基础接口类                                                    *
 ***********************************************************************************/
class VmDriverBase
{
public:
    VmDriverBase()
    {
        pthread_mutex_init(&_driver_mutex,0);
    };
    
    ~VmDriverBase()
    {
        pthread_mutex_destroy(&_driver_mutex);
    };
    
    // 下面函数都是虚函数，所有实现，均可以被子类替换
    // 类初始化,包括链接
    virtual int Init() = 0;

    // 创建配置文件
    virtual string CreateConfig(const string vm_ins_name, VMConfig &cfg) = 0;
    
    // 创建虚拟机接口
    virtual bool Create(const string& xml, int *domid) = 0;

    // 设置虚拟机权重值上限接口
    virtual int Sched_credit_cap_set(const string &dom_name, int CoreNum, 
                                                   unsigned int percent) = 0;

    // 设置调度权重值接口
    virtual int Sched_credit_weight_set(const string &dom_name, unsigned int percent) = 0;

    // 设置调度权重值接口
    virtual int Sched_credit_weight_set(int id, unsigned int percent) = 0;

    // 设置虚拟机全局不bind功能
    virtual int VcpuAffinitySet(const string &dom_name, unsigned int VcpuNum) = 0;

    // 关机
    virtual int ShutDown(const string& instance_name) = 0;

    // 挂起
    virtual int Suspend(const string& instance_name) = 0;

    // 恢复
    virtual int Resume(const string& instance_name) = 0;

    // 重启
    virtual int Reboot(const string& instance_name) = 0;

    // 强制重启
    virtual int Reset(const string& instance_name, const string& cfg_file_name) = 0;

    // 迁移虚拟机
    virtual int Migrate(const string& instance_name, const string& host_ip) = 0;

    // 获取domin的信息，这个是强相关信息，需要定义成自己的结构进行传递
    virtual int GetDomainInfo(const string& dom_name, VmDomainInfo &DomInfo) = 0;

    // 获取虚拟机状态
    virtual VmInstanceState GetDomainState(const string& name) = 0;

    // 列出所有虚拟机
    virtual int ListDomains(int* ids) = 0;

    // 通过虚拟机的的ID获取虚拟机名称
    virtual string GetDomainNameByDomid(int id) = 0;

    // 通过虚拟机ID，获取虚拟机网络信息
    virtual int GetDomainNetInfoById(int domid, uint32 nic_no, 
                             VmDomainInterfaceStatsStruct& info) = 0;

    // 通过虚拟机名称获取domid
    virtual int GetDomainIDByName(const string& domname) = 0;

    // 通过虚拟机名称获取描述信息
    virtual string GetDomainDescByName(const string& domname) = 0;
    
    // 通过虚拟机名称设置虚拟机描述信息
    virtual int SetDomainDescByName(const string& domname, const string& desc) = 0;

    // 强制关闭虚拟机
    virtual int DestroyDomain(const string &dom_name) = 0;
   
    // 向虚拟机插入USB盘
    virtual int AddUsb(const string &dom_name, int vendor_id, int product_id) = 0;
   
    // 从虚拟机拔出USB盘
    virtual int DelUsb(const string &dom_name, int vendor_id, int product_id) = 0;
    
    // 从虚拟机拔出USB盘
    virtual int DelAndAddUsb(const string &dom_name, int vendor_id, int product_id) = 0;

    virtual int AddRemovableDisk(const string& instance_name,  const VmDiskConfig &disk) = 0;

    virtual int DelRemovableDisk(const string& instance_name,  const VmDiskConfig &disk) = 0;

    /*  0 : 不存在  ， 1   存在， -1  unkown  */
    virtual int IsDomExist(const string& instance_name, int domain_id)
    {
		return 0;
    };

    static VmmType GetDriverType(void)
    {
        string        sTypePath  =  "/sys/hypervisor/type";
        string        sHyType;
        string        sValue;

        /*  first detect if is it  simulate */
        /*if (SimulateDriver::SimulateIsOk())
        {
            return SIMULATE;
        }*/

        ifstream fin(sTypePath.c_str());
        if (!fin.is_open())    /* in kvm or other vmm, the file is not exist */
        {
            /* is kvm ? */
            string  scdev_kvm = "/dev/kvm";
            ifstream fin_kvm(scdev_kvm.c_str());
            if (fin_kvm.is_open())
            {
                return KVM;
            }
            else
            {
                return UNKNOWN_DRIVER_TYPE;
            }
        }

        getline(fin, sValue);

        sHyType = sValue;
        if (string::npos != sHyType.find("xen", 0))
        {
            return XEN;
        }

        if (string::npos != sHyType.find("kvm", 0))
        {
            return KVM;
        }

        return UNKNOWN_DRIVER_TYPE;
    }  

protected:
    VmmType            _vmm_type;            /* 管理进程类型，xen or kvm,etc. */
    
    void Lock()
    {
        pthread_mutex_lock(&_driver_mutex);
    };

    void Unlock()
    {
        pthread_mutex_unlock(&_driver_mutex);
    };
        
    /****************************************************************************************
    * 函数名称：ParsePciNo
    * 功能描述：准备PCI号
    * 访问的表：无
    * 修改的表：无
    * 输入参数：无
    * 输出参数：
    * 返 回 值：
    * 其它说明：
    * 修改日期      版本号  修改人      修改内容
    * ---------------------------------------------------------------------------------------
    * 2012/07/09    V1.0    lixch        创建
    ****************************************************************************************/ 
    int ParsePciNo(const string& pci, string&domain, string& bus, string&slot, string& function)
    {
        string tmppci = pci;
        string::size_type loc = tmppci.find(":", 0);

        loc = tmppci.find( ":", 0 );
        if(loc == string::npos)
        {
            OutPut(SYS_ALERT,"parsepci a return error!\n");
            return -1;
        }
        else
        {
            domain = "0x"+tmppci.substr(0, loc);
            tmppci.erase(0,loc+1);

            loc = tmppci.find( ":", 0 );
            if(loc != string::npos)
            {
                bus = "0x"+tmppci.substr(0, loc);
                tmppci.erase(0,loc+1);

                loc = tmppci.find(".",0);
                if(loc != string::npos)
                {
                   slot = "0x"+tmppci.substr(0,loc);
                   tmppci.erase(0,loc+1);

                   loc = tmppci.find(".",0);
                   if(loc == string::npos)
                   {
                       function = "0x"+tmppci;
                   }
                   else
                   {
                       OutPut(SYS_ALERT,"parsepci d return error!\n");
                       return -1;
                   }
                }
                else
                {
                    OutPut(SYS_ALERT,"parsepci c return error!\n");
                    return -1;
                }
            }
            else
            {
                OutPut(SYS_ALERT,"parsepci b return error!\n");
                return -1;
            }
        }
        return 0;
    };
    
    /****************************************************************************************
    * 函数名称：ConstructPciNoString
    * 功能描述：准备PCI号
    * 访问的表：无
    * 修改的表：无
    * 输入参数：无
    * 输出参数：
    * 返 回 值：
    * 其它说明：
    * 修改日期      版本号  修改人      修改内容
    * ---------------------------------------------------------------------------------------
    * 2012/07/09    V1.0    lixch        创建
    ****************************************************************************************/ 
    void ConstructPciNoString(const string&domain, const string& bus, const string&slot, const string& function, string& out_pci)
    {
        string temp;
        
        temp = domain;
        if (domain.find("0x", 0))
        {
            temp = domain.substr(2);
        }
        out_pci = temp + ":";
        
        temp = bus;
        if (bus.find("0x", 0))
        {
            temp = bus.substr(2);
        }
        out_pci = out_pci + temp + ".";
        
        temp = slot;
        if (slot.find("0x", 0))
        {
            temp = slot.substr(2);
        }
        out_pci = out_pci + temp + ".";
        
        temp = function;
        if (function.find("0x", 0))
        {
            temp = function.substr(2);
        }
        out_pci = out_pci + temp;
    };


    
private:
    pthread_mutex_t    _driver_mutex;        /* 互斥访问hypervisor */
    
};


}

#endif // #ifndef HC_VMDRIVER_BASE_H


