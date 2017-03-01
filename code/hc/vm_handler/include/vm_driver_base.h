/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�vm_driver_base.h
* �ļ���ʶ��
* ����ժҪ��VmDriver��Ķ����ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�lixch
* ������ڣ�2011��7��25��
*
*******************************************************************************/

#ifndef HC_VMDRIVER_BASE_H
#define HC_VMDRIVER_BASE_H

#include "vm_messages.h"

#include "vm_pub.h"

/************************************************************************************
 *                    ���⻯���������ӿ��� ʹ�õ��Ĳ���                                      *
 ***********************************************************************************/
namespace zte_tecs
{
/* �����ʵ����״̬ */
/* �޸ı�enumʱ����ͬ���޸�vmcfg_api.h�е�ApiVmUserState״̬!!! */

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
 *                    ���⻯���������ӿ���                                                    *
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
    
    // ���溯�������麯��������ʵ�֣������Ա������滻
    // ���ʼ��,��������
    virtual int Init() = 0;

    // ���������ļ�
    virtual string CreateConfig(const string vm_ins_name, VMConfig &cfg) = 0;
    
    // ����������ӿ�
    virtual bool Create(const string& xml, int *domid) = 0;

    // ���������Ȩ��ֵ���޽ӿ�
    virtual int Sched_credit_cap_set(const string &dom_name, int CoreNum, 
                                                   unsigned int percent) = 0;

    // ���õ���Ȩ��ֵ�ӿ�
    virtual int Sched_credit_weight_set(const string &dom_name, unsigned int percent) = 0;

    // ���õ���Ȩ��ֵ�ӿ�
    virtual int Sched_credit_weight_set(int id, unsigned int percent) = 0;

    // ���������ȫ�ֲ�bind����
    virtual int VcpuAffinitySet(const string &dom_name, unsigned int VcpuNum) = 0;

    // �ػ�
    virtual int ShutDown(const string& instance_name) = 0;

    // ����
    virtual int Suspend(const string& instance_name) = 0;

    // �ָ�
    virtual int Resume(const string& instance_name) = 0;

    // ����
    virtual int Reboot(const string& instance_name) = 0;

    // ǿ������
    virtual int Reset(const string& instance_name, const string& cfg_file_name) = 0;

    // Ǩ�������
    virtual int Migrate(const string& instance_name, const string& host_ip) = 0;

    // ��ȡdomin����Ϣ�������ǿ�����Ϣ����Ҫ������Լ��Ľṹ���д���
    virtual int GetDomainInfo(const string& dom_name, VmDomainInfo &DomInfo) = 0;

    // ��ȡ�����״̬
    virtual VmInstanceState GetDomainState(const string& name) = 0;

    // �г����������
    virtual int ListDomains(int* ids) = 0;

    // ͨ��������ĵ�ID��ȡ���������
    virtual string GetDomainNameByDomid(int id) = 0;

    // ͨ�������ID����ȡ�����������Ϣ
    virtual int GetDomainNetInfoById(int domid, uint32 nic_no, 
                             VmDomainInterfaceStatsStruct& info) = 0;

    // ͨ����������ƻ�ȡdomid
    virtual int GetDomainIDByName(const string& domname) = 0;

    // ͨ����������ƻ�ȡ������Ϣ
    virtual string GetDomainDescByName(const string& domname) = 0;
    
    // ͨ��������������������������Ϣ
    virtual int SetDomainDescByName(const string& domname, const string& desc) = 0;

    // ǿ�ƹر������
    virtual int DestroyDomain(const string &dom_name) = 0;
   
    // �����������USB��
    virtual int AddUsb(const string &dom_name, int vendor_id, int product_id) = 0;
   
    // ��������γ�USB��
    virtual int DelUsb(const string &dom_name, int vendor_id, int product_id) = 0;
    
    // ��������γ�USB��
    virtual int DelAndAddUsb(const string &dom_name, int vendor_id, int product_id) = 0;

    virtual int AddRemovableDisk(const string& instance_name,  const VmDiskConfig &disk) = 0;

    virtual int DelRemovableDisk(const string& instance_name,  const VmDiskConfig &disk) = 0;

    /*  0 : ������  �� 1   ���ڣ� -1  unkown  */
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
    VmmType            _vmm_type;            /* ����������ͣ�xen or kvm,etc. */
    
    void Lock()
    {
        pthread_mutex_lock(&_driver_mutex);
    };

    void Unlock()
    {
        pthread_mutex_unlock(&_driver_mutex);
    };
        
    /****************************************************************************************
    * �������ƣ�ParsePciNo
    * ����������׼��PCI��
    * ���ʵı���
    * �޸ĵı���
    * �����������
    * ���������
    * �� �� ֵ��
    * ����˵����
    * �޸�����      �汾��  �޸���      �޸�����
    * ---------------------------------------------------------------------------------------
    * 2012/07/09    V1.0    lixch        ����
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
    * �������ƣ�ConstructPciNoString
    * ����������׼��PCI��
    * ���ʵı���
    * �޸ĵı���
    * �����������
    * ���������
    * �� �� ֵ��
    * ����˵����
    * �޸�����      �汾��  �޸���      �޸�����
    * ---------------------------------------------------------------------------------------
    * 2012/07/09    V1.0    lixch        ����
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
    pthread_mutex_t    _driver_mutex;        /* �������hypervisor */
    
};


}

#endif // #ifndef HC_VMDRIVER_BASE_H


