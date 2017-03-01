/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�vm_driver.h
* �ļ���ʶ��
* ����ժҪ��VmDriver��Ķ����ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�lixch
* ������ڣ�2011��7��25��
*
*******************************************************************************/

#ifndef HC_VMDRIVER_H
#define HC_VMDRIVER_H

#include "vm_pub.h"
#include "vm_driver_base.h"


using namespace std;

/************************************************************************************
 *                    ���⻯����                                                    *
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
    
    // ���������ļ�
    string CreateConfig(const string vm_ins_name, VMConfig &cfg)
    {
        return impl->CreateConfig(vm_ins_name, cfg);
    }

    // ���������ļ�
    bool Create(const string& xml, int *domid)
    {
        return impl->Create(xml, domid);
    }
    
    // ���������Ȩ��ֵ���޽ӿ�
    int Sched_credit_cap_set(const string &instance_name, int CoreNum, unsigned int percent)
    {
        return impl->Sched_credit_cap_set(instance_name, CoreNum, percent);
    }

    // ���õ���Ȩ��ֵ�ӿ�
    int Sched_credit_weight_set(const string &instance_name, unsigned int percent)
    {
        return impl->Sched_credit_weight_set(instance_name, percent);
    }

    // ���õ���Ȩ��ֵ�ӿ�
    int Sched_credit_weight_set(int id, unsigned int percent)
    {
        return impl->Sched_credit_weight_set(id, percent);
    }

    // ���������ȫ�ֲ�bind����
    int VcpuAffinitySet(const string &instance_name, unsigned int VcpuNum)
    {
        return impl->VcpuAffinitySet(instance_name, VcpuNum);
    }

    // �ػ�
    int ShutDown(const string& instance_name)
    {
        return impl->ShutDown(instance_name);
    }

    // ����
    int Suspend(const string& instance_name)
    {
        return impl->Suspend(instance_name);
    }

    // �ָ�
    int Resume(const string& instance_name)
    {
        return impl->Resume(instance_name);
    }

    // ����
    int Reboot(const string& instance_name)
    {
        return impl->Reboot(instance_name);
    }

    // ǿ������
    int Reset(const string& instance_name, const string& cfg_file_name)
    {
        return impl->Reset(instance_name, cfg_file_name);
    }
    
    // Ǩ�������
    int Migrate(const string& instance_name, const string& host_ip)
    {
        return impl->Migrate(instance_name, host_ip);
    }

    // ��ȡdomin����Ϣ�������ǿ�����Ϣ����Ҫ������Լ��Ľṹ���д���
    int GetDomainInfo(const string& instance_name, VmDomainInfo &DomInfo)
    {
        return impl->GetDomainInfo(instance_name, DomInfo);
    }

    // ��ȡ�����״̬
    VmInstanceState GetDomainState(const string& name)
    {
        return impl->GetDomainState(name);
    }

    // �г����������
    int ListDomains(int* ids)
    {
        return impl->ListDomains(ids);
    }

    // ͨ��������ĵ�ID��ȡ���������
    string GetDomainNameByDomid(int id)
    {
        return impl->GetDomainNameByDomid(id);
    }

    // ͨ�������ID����ȡ�����������Ϣ
    int GetDomainNetInfoById(int domid, uint32 nic_no, 
                             VmDomainInterfaceStatsStruct& info)
    {
        return impl->GetDomainNetInfoById(domid, nic_no, info);
    }

    // ͨ����������ƻ�ȡdomid
    int GetDomainIDByName(const string& instance_name)
    {
        return impl->GetDomainIDByName(instance_name);
    }

    // ͨ����������ƻ�ȡ������Ϣ
    string GetDomainDescByName(const string& instance_name)
    {
        return impl->GetDomainDescByName(instance_name);
    }
    
    // ͨ��������������������������Ϣ
    int SetDomainDescByName(const string& instance_name, const string& desc)
    {
        return impl->SetDomainDescByName(instance_name, desc);
    }

    // ǿ�ƹر������
    int DestroyDomain(const string &instance_name)
    {
        return impl->DestroyDomain(instance_name);
    }

    // �����������USB��
    int AddUsb(const string &instance_name, int vendor_id, int product_id)
    {
        return impl->AddUsb(instance_name, vendor_id, product_id);
    };
   
    // ��������γ�USB��
    int DelUsb(const string &instance_name, int vendor_id, int product_id)
    {
        return impl->DelUsb(instance_name, vendor_id, product_id);
    };
    
    // ��������γ�USB��
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


