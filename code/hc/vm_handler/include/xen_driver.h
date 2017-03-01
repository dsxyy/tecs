/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�xen_driver.h
* �ļ���ʶ��
* ����ժҪ��XenDriver��Ķ����ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�lixch
* ������ڣ�2011��7��25��
*
* �޸ļ�¼1��
*     �޸����ڣ�2013/3/25
*     �� �� �ţ�V1.0
*     �� �� �ˣ�lixch
*     �޸����ݣ�����
*******************************************************************************/
#ifndef HC_XENDRIVER_H
#define HC_XENDRIVER_H

#include "vm_driver_base.h"

using namespace std;




/************************************************************************************
 *                    XEN���⻯����                                                    *
 ***********************************************************************************/
class XenDriver:public VmDriverBase
{
public:
    //���캯��
    XenDriver()
    {
    };

    //��������
    ~XenDriver()
    {
    };
    
    // ���溯�������麯��������ʵ�֣������Ա������滻
    // ���ʼ��
    int Init();

    // ���������ļ�
    string CreateConfig(const string vm_ins_name, VMConfig &cfg);
    
    // ����������ӿ�
    bool Create(const string& xml, int *domid);

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
    int AddUsb(const string &dom_name, int vendor_id, int product_id);
   
    // ��������γ�USB��
    int DelUsb(const string &dom_name, int vendor_id, int product_id);
    
    // ��������γ�USB��
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


