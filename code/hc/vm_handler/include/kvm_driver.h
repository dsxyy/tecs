/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�kvm_driver.h
* �ļ���ʶ��
* ����ժҪ��KvmDriver��Ķ����ļ�
* ��ǰ�汾��1.0
* ��    ��       ��wangtl
* ������ڣ�2013��5��15��
*
* �޸ļ�¼1��
*     �޸����ڣ�2013/5/15
*     �� �� �ţ�V1.0
*     �� �� �ˣ�wangtl
*     �޸����ݣ�����
*******************************************************************************/
#if 0 /* �ϲ���libvirt_driver */
#ifndef HC_KVMDRIVER_H
#define HC_KVMDRIVER_H

#include "vm_driver_base.h"

#include <libvirt/libvirt.h>
#include <libvirt/virterror.h>

using namespace std;

/************************************************************************************
 *                    ���⻯����                                                    *
 ***********************************************************************************/
class KvmDriver:public VmDriverBase
{

public:
    //���캯��
    KvmDriver()
    {
        _connect  = NULL;
        _pcpu_num = 0;
    };

    //��������
    ~KvmDriver()
    {
    };
    
    // libvirt״̬
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
    
    // ���溯�������麯��������ʵ�֣������Ա������滻
    // ���ʼ��
    int Init();

    // ���������ļ�
    string CreateConfig(const string vm_ins_name, VMConfig &cfg);
    
    // ����������ӿ�
    bool Create(const string& filename, int *domid);

    // ���������Ȩ��ֵ���޽ӿ�
    int Sched_credit_cap_set(const string &dom_name, int CoreNum, unsigned int percent);

    // ���õ���Ȩ��ֵ�ӿ�
    int Sched_credit_weight_set(const string &dom_name, unsigned int percent);

    // ���õ���Ȩ��ֵ�ӿ�
    int Sched_credit_weight_set(int id, unsigned int percent);

    // ���������ȫ�ֲ�bind����
    int VcpuAffinitySet(const string &dom_name, unsigned int VcpuNum);

    // �ػ�
    int ShutDown(const string& instance_name);

    // ����
    int Suspend(const string& instance_name);

    // �ָ�
    int Resume(const string& instance_name);

    // ����
    int Reboot(const string& instance_name);

    // ǿ������
    int Reset(const string& instance_name, const string& cfg_file_name);

    // Ǩ�������
    int Migrate(const string& instance_name, const string& host_ip);

    // ��ȡdomin����Ϣ�������ǿ�����Ϣ����Ҫ������Լ��Ľṹ���д���
    int GetDomainInfo(const string& dom_name, VmDomainInfo &DomInfo);

    // ��ȡ�����״̬
    VmInstanceState GetDomainState(const string& name);

    // �г����������
    int ListDomains(int* ids);

    // ͨ��������ĵ�ID��ȡ���������
    string GetDomainNameByDomid(int id);

    // ͨ�������ID����ȡ�����������Ϣ
    int GetDomainNetInfoById(int domid, uint32 nic_no, 
                             VmDomainInterfaceStatsStruct& info);

    // ͨ����������ƻ�ȡdomid
    int GetDomainIDByName(const string& domname);

    // ͨ����������ƻ�ȡ������Ϣ
    string GetDomainDescByName(const string& domname);
    
    // ͨ��������������������������Ϣ
    int SetDomainDescByName(const string& domname, const string& desc);

    // ǿ�ƹر������
    int DestroyDomain(const string &dom_name);

    // �����������USB��
    int AddUsb(const string &instance_name, int vendor_id, int product_id);
   
    // ��������γ�USB��
    int DelUsb(const string &instance_name, int vendor_id, int product_id);
    
    // ��������γ�USB��
    int DelAndAddUsb(const string &instance_name, int vendor_id, int product_id);

    int AddRemovableDisk(const string& instance_name,  const VmDiskConfig &disk);

    int DelRemovableDisk(const string& instance_name,  const VmDiskConfig &disk);
    
private:
    string             _uri;                 /* libvirt����hypervisor��URI */
    virConnectPtr      _connect;             /* libvirt����hypervisor�����Ӿ�� */
    uint32             _pcpu_num;
    
    VmInstanceState InvertToVmState(int state_in);
    virConnectPtr      CheckHypervisorConnect();
    
    // ��ȡ�����������ָ��
    virConnectPtr GetConn(const char* ip);
        
    // ��ȡdomin����Ϣ
    int GetDomainInfo(virDomainPtr dom, virDomainInfo *pDomInfo);
    
    // ͨ����������ƻ�ȡ���������ָ��
    virDomainPtr GetDomainByInstanceName(const string &name);

    // �ͷ����������ָ��
    void FreeDomain(virDomainPtr dom);
    
    bool cap_set_zero_with_syscmd(const string &dom_name);
    
    // �ڲ�ʹ�õĺ���
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
