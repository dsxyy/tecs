/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�kvm_driver.cpp
* �ļ���ʶ��
* ����ժҪ��KvmDriver���ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�lixch
* ������ڣ�2013��3��21��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/7/25
*     �� �� �ţ�V1.0
*     �� �� �ˣ�lxch
*     �޸����ݣ�����
*******************************************************************************/
#if 0  /* �ϲ���libvirt_driver */
#include "kvm_driver.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <iostream>
#include <stdio.h>
#include "host_agent.h"
#include "vm_messages.h"
#include <libvirt/libvirt-qemu.h>

void kvm_error_handler (void *userData,virErrorPtr error);
/*****************************************************************************
 *                               ȫ�ֱ���                                    *
 ****************************************************************************/
#define PCPU_WEIGHT_TOTAL 10000


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
* 2011/08/12    V1.0    lxch         ����
****************************************************************************************/
int KvmDriver::Init()
{
    _uri     = "remote:///";
    _connect = NULL;
    int times = 0;

    // 2S����һ�Σ����Ƿ��ܹ����ӳɹ������ɹ���������
    while(_connect == NULL)
    {
        //��һ�β���Ҫ��¼�쳣
        if(times > 0)
        {
            OutPut(SYS_ERROR, "KvmDriver::virConnectOpen return null, retry! uri:%s\n", _uri.c_str());
            
            virErrorPtr error = virGetLastError();
            OutPut(SYS_ERROR, "virConnectOpen failed, error->code:%d, error->domain:%d, error->message:%s.\n", 
                    error->code, error->domain, error->message);
            Delay(2000);
        }

        if(times >= 60)
        {
            OutPut(SYS_ERROR, "KvmDriver::init failed.\n");
            return ERROR;
        }
        
        _connect = virConnectOpen(_uri.c_str());
        times++;
    }

    virSetErrorFunc (NULL, kvm_error_handler);
    return SUCCESS;
}

/****************************************************************************************
* �������ƣ�CheckHypervisorConnect
* �����������������
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lxch         ����
****************************************************************************************/
virConnectPtr KvmDriver::CheckHypervisorConnect()
{
    if(_connect == NULL)
    {
        _connect = virConnectOpen (_uri.c_str());
        if (_connect == NULL)
        {
            SkyAssert(0);
            OutPut(SYS_ALERT, "KvmDriver::CheckHypervisorConnect fail! virConnectOpen return null! uri:%s\n", _uri.c_str());
            return NULL;
        }
    }

    return _connect;
}

virConnectPtr KvmDriver::GetConn(const char* ip)
{
    virConnectPtr conn = NULL;
    string hypervisorURL;
    if(ip == NULL) //local connection
    {
        hypervisorURL.assign("remote:///");
    }
    else  //remote connection
    {
        hypervisorURL.assign("remote+ssh://");
        hypervisorURL.append(ip);
    }

    /* check that libvirt can query the hypervisor */
    conn = virConnectOpen (hypervisorURL.c_str());
    /* NULL means local hypervisor */
    if (conn == NULL)
    {
        cout<<"GetConn error: failed to connect to hypervisor, hypervisorURL:"<<hypervisorURL<<endl;
    }

    return conn;
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
* 2011/08/12    V1.0    lxch         ����
****************************************************************************************/
bool KvmDriver::Create(const string& filename, int *domid)
{
    bool rc = false;
    ifstream fin;
    string   xml;

    fin.open(filename.c_str());
    char ch;
    while(fin.get(ch))
    {
        xml += ch;
    }
    fin.close();

    virConnectPtr connect = CheckHypervisorConnect();

    cout << xml.c_str() << endl;

    Lock();
    virDomainPtr dom = virDomainCreateXML (connect, xml.c_str(), 0);
    if(dom != NULL)
    {
        *domid = virDomainGetID(dom);
        rc = true;
        virDomainFree(dom);
    }
    Unlock();

    return rc;
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
* 2011/08/12    V1.0    lxch         ����
****************************************************************************************/
int KvmDriver::Reboot(const string& instance_name)
{
    int err = -1;
    virConnectPtr connect = CheckHypervisorConnect();

    Lock();
    virDomainPtr dom = virDomainLookupByName(connect, instance_name.c_str());
    if (dom != NULL)
    {
        err=virDomainReboot(dom, 0);
        virDomainFree(dom);
    }
    Unlock();

    return err;
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
* 2011/08/12    V1.0    lxch         ����
****************************************************************************************/
int KvmDriver::Suspend(const string& instance_name)
{
    int err = -1;

    virConnectPtr connect = CheckHypervisorConnect();
    Lock();
    virDomainPtr dom = virDomainLookupByName(connect, instance_name.c_str());
    if (dom != NULL)
    {
        err=virDomainSuspend(dom);
        virDomainFree(dom);
    }
    Unlock();

    return err;
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
* 2011/08/12    V1.0    lxch         ����
****************************************************************************************/
int KvmDriver::Resume(const string& instance_name)
{
    int err = -1;

    virConnectPtr connect = CheckHypervisorConnect();
    Lock();
    virDomainPtr dom = virDomainLookupByName(connect, instance_name.c_str());
    if (dom != NULL)
    {
        err=virDomainResume(dom);
        virDomainFree(dom);
    }
    Unlock();

    return err;
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
* 2011/08/12    V1.0    lxch         ����
****************************************************************************************/
int KvmDriver::ShutDown(const string& instance_name)
{
    int err = 0;

    virConnectPtr connect = CheckHypervisorConnect();
    Lock();
    virDomainPtr dom = virDomainLookupByName(connect, instance_name.c_str());
    if (dom != NULL)
    {
        err=virDomainShutdown(dom);
        virDomainFree(dom);
    }
    Unlock();

    return err;
}

/****************************************************************************************
* �������ƣ�AddUsb
* ���������������������һ��USB�豸
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lxch         ����
* virDomainQemuMonitorCommand *
****************************************************************************************/
int KvmDriver::AddUsb(const string& instance_name, int vendor_id, int product_id)
{
    ostringstream cmd;

    cmd <<"virsh qemu-monitor-command  domid --hmp";
    cmd << "device_add  usb-host"<<",vendorid="<<to_string(vendor_id, hex)<<", productid="<<to_string(product_id, hex);
    return RunCmd(cmd.str());
}

/****************************************************************************************
* �������ƣ�DelUsb
* �������������������ɾ��һ��USB�豸
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lxch         ����
****************************************************************************************/
int KvmDriver::DelUsb(const string& instance_name, int vendor_id, int product_id)
{
    ostringstream cmd;

    cmd <<"virsh qemu-monitor-command  domid --hmp";
    cmd << "device_del  usb-host"<<",vendorid="<<to_string(vendor_id, hex)<<", productid="<<to_string(product_id, hex);
    return  RunCmd(cmd.str());
}

/****************************************************************************************
* �������ƣ�DelAndAddUsb
* �����������β�USB�豸
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lxch         ����
****************************************************************************************/
int KvmDriver::DelAndAddUsb(const string& instance_name, int vendor_id, int product_id)
{
    DelUsb(instance_name, vendor_id, product_id);
    return AddUsb(instance_name, vendor_id, product_id);
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
int KvmDriver::Reset(const string& instance_name, const string& cfg_file_name)
{
    int err = -1;
    virConnectPtr connect = CheckHypervisorConnect();

    Lock();
    virDomainPtr dom = virDomainLookupByName(connect, instance_name.c_str());
    if (dom != NULL)
    {
        char* xml_desc = NULL;
        xml_desc = virDomainGetXMLDesc(dom, 0);
        err=virDomainShutdown(dom);/*  ??????*/
        virDomainFree(dom);
        if (!err && !xml_desc)
        {
            dom = virDomainCreateXML(connect, xml_desc, 0);
            virDomainFree(dom);
        }

    }
    Unlock();

    return err;
}

/****************************************************************************************
* �������ƣ�VcpuPinSet
* ��������������CPU��bind�ӿ�
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
int KvmDriver::VcpuAffinitySet(const string &dom_name, unsigned int VcpuNum)
{
    HostAgent *host_agent = HostAgent::GetInstance();

    if(NULL == host_agent)
    {
        OutPut(SYS_ALERT, "HostAgent return null instance!\n");
        return -1;
    }

    // ������ǵ�һ�λ�ȡ����CPU�������򲻻�ȡ��ʹ�ú�ȥ����ֵ
    if (_pcpu_num == 0)
    {
        _pcpu_num = host_agent->GetPcpuNum();
    }

    if (_pcpu_num == 0)
    {
        OutPut(SYS_ALERT, "KvmDriver::virDomainPinVcpu get pcpu fail!\n");
        return -1;
    }

    if (_pcpu_num > 128)
    {
        OutPut(SYS_ALERT, "KvmDriver::virDomainPinVcpu get pcpu > 128!\n");
        _pcpu_num = 128;
    }

    virDomainPtr ptr = GetDomainByInstanceName(dom_name);
    if (ptr == NULL)
    {
        return -1;
    }

    unsigned char    cpumap[32];   /* cpumap�����֧��128������CPU */
    unsigned int     i;
    memset(cpumap, 0, sizeof(cpumap));
    for (i = 0; i < (unsigned int)_pcpu_num; i++)
    {
        VIR_USE_CPU(cpumap,i);
    }

    for (i = 0; i < VcpuNum; i++)
    {
        if (0 != virDomainPinVcpu(ptr, i, cpumap, VIR_CPU_MAPLEN(_pcpu_num)))
        {
            OutPut(SYS_ALERT, "KvmDriver::%s virDomainPinVcpu fail!vcpu is %d.\n",
                   dom_name.c_str(), i);
        }
    }

    FreeDomain(ptr);

    return 0;
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
int KvmDriver::Sched_credit_weight_set(int id, unsigned int percent)
{
    string name = GetDomainNameByDomid(id);

    return Sched_credit_weight_set(name, percent);
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
#define     DEFAULT_VCPU_QUOTA   ((unsigned long long)100000)
#define     DEFAULT_VCPU_SHARES  ((unsigned long long)1024)   /* head */
int KvmDriver::Sched_credit_weight_set(const string &dom_name, unsigned int percent)
{
    virSchedParameter  sche_para;
    virDomainPtr ptr = GetDomainByInstanceName(dom_name);
    if (ptr == NULL)
    {
        return -1;
    }

    if (percent > 100)
    {
        percent = 100;
    }

    memset(&sche_para, 0, sizeof(sche_para));

    sprintf(sche_para.field, "%s", "cpu_shares");
    sche_para.type = VIR_TYPED_PARAM_ULLONG;

    sche_para.value.ui = (DEFAULT_VCPU_SHARES/100)*percent;

    OutPut(SYS_INFORMATIONAL, "KvmDriver::Sched_credit_weight_set percent = %d!\n", percent);

    if (0 != virDomainSetSchedulerParameters (ptr, &sche_para, 1))
    {
        OutPut(SYS_ALERT, "LibvirtDriver::Sched_credit_weight_set Fail percent = %d!\n", percent);
    }

    FreeDomain(ptr);

    return 0;
}


/****************************************************************************************
* �������ƣ�cap_set_with_syscmd
* ����������ʹ��ϵͳ��������
* ���ʵı���
* �޸ĵı���
* ���������domid  �����id    CoreNum  ����  percent Ȩ�ذٷֱ�
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/02/12    V1.0    lxc         ����
* kvm has no cap param   *
****************************************************************************************/
bool KvmDriver::cap_set_zero_with_syscmd(const string &dom_name)
{
    return true;
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
int KvmDriver::Sched_credit_cap_set(const string &dom_name, int CoreNum, unsigned int percent)
{
    virSchedParameter  sche_para;
    virDomainPtr ptr = GetDomainByInstanceName(dom_name);
    if (ptr == NULL)
    {
        return -1;
    }

    if (0 == percent)
    {
        return -1;
    }

    if (percent > 100)
    {
        percent = 100;
    }

    memset(&sche_para, 0, sizeof(sche_para));

    sprintf(sche_para.field, "%s", "vcpu_quota");
    sche_para.type = VIR_TYPED_PARAM_LLONG;

    sche_para.value.ui = (DEFAULT_VCPU_QUOTA/100)*percent;

    OutPut(SYS_INFORMATIONAL, "KvmDriver::Sched_credit_weight_set percent = %d!\n", percent);

    if (0 != virDomainSetSchedulerParameters (ptr, &sche_para, 1))
    {
        OutPut(SYS_ALERT, "LibvirtDriver::Sched_credit_weight_set Fail percent = %d!\n", percent);
    }

    FreeDomain(ptr);

    return 0;
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
* 2011/12/31    V1.0    lxch         ����
****************************************************************************************/
int KvmDriver::Migrate(const string& instance_name, const string& host_ip)
{
    virDomainPtr dom = NULL;
    virDomainPtr dstdom = NULL;
    virConnectPtr dstconn = NULL;
    virConnectPtr connect = CheckHypervisorConnect();
    if((instance_name.empty())||(host_ip.empty()))
    {
        cout<<"In Migrate, input error!"<<endl;
        return -1;
    }

    Lock();
    dom = virDomainLookupByName(connect, instance_name.c_str());
    if (dom == NULL)
    {
        cout<<"In Migrate, virDomainLookupByName return NULL!"<<endl;
        Unlock();
        return -1;
    }

    dstconn = GetConn(host_ip.c_str());
    if(dstconn == NULL)
    {
        cout<<"In Migrate, GetConn return NULL!"<<endl;
        virDomainFree(dom);
        Unlock();
        return -1;
    }

    dstdom = virDomainMigrate(dom, dstconn, VIR_MIGRATE_LIVE, NULL, host_ip.c_str(), 0);
    virDomainFree(dom);
    if(NULL != dstdom)
    {
        virDomainFree(dstdom);
    }
    virConnectClose(dstconn);

    Unlock();
    return 0;
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
* 2011/08/12    V1.0    lxch         ����
****************************************************************************************/
int KvmDriver::GetDomainInfo(const string& dom_name, VmDomainInfo &Info)
{
    virDomainPtr dom = GetDomainByInstanceName(dom_name);
    virDomainInfo DomInfo;

    int err = GetDomainInfo(dom, &DomInfo);
    if ((dom != NULL))
    {
        virDomainFree(dom);
    }

    Info.state = InvertToVmState(DomInfo.state);
    Info.maxMem = DomInfo.maxMem;
    Info.memory = DomInfo.memory;
    Info.nrVirtCpu = DomInfo.nrVirtCpu;
    Info.cpuTime = DomInfo.cpuTime;

    return err;
}


/****************************************************************************************
* �������ƣ�GetDomainInfo
* ��������������ʵ�������������ʵ���������Ϣ
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lxch         ����
****************************************************************************************/
int KvmDriver::GetDomainInfo(virDomainPtr dom, virDomainInfo *pDomInfo)
{
    int err = -1;

    Lock();
    if ((dom != NULL)&&(pDomInfo != NULL))
    {
        err=virDomainGetInfo(dom, pDomInfo);
    }
    Unlock();

    return err;
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
* 2011/08/12    V1.0    lxch         ����
****************************************************************************************/
int KvmDriver::ListDomains(int* ids)
{
    int num_doms = 0;
    virConnectPtr connect = CheckHypervisorConnect();

    if(NULL != ids)
    {
        num_doms = virConnectListDomains(connect, ids, MAX_DOM_NUM);
    }

    return num_doms;
}

/****************************************************************************************
* �������ƣ�GetDomainByID
* ��������������domid���ʵ�����
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lxch         ����
****************************************************************************************/
virDomainPtr KvmDriver::GetDomainByInstanceName(const string &name)
{
    virDomainPtr dom = NULL;
    virConnectPtr connect = CheckHypervisorConnect();

    Lock();
    dom = virDomainLookupByName(connect, name.c_str());
    Unlock();

    return dom;
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
string KvmDriver::GetDomainNameByDomid(int id)
{
    string domname;
    virDomainPtr dom = NULL;
    virConnectPtr connect = CheckHypervisorConnect();

    Lock();
    dom = virDomainLookupByID(connect, id);
    if(NULL != dom)
    {
        domname = virDomainGetName(dom);
        virDomainFree(dom);
    }
    Unlock();

    return domname;
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
* modify by wangtl for kvm   *
****************************************************************************************/
int KvmDriver::GetDomainNetInfoById(int domid, uint32 nic_no,
                                    VmDomainInterfaceStatsStruct& info)
{
    int            err = -1;
    virDomainPtr   dom = NULL;
    virConnectPtr  connect = CheckHypervisorConnect();
    virDomainInterfaceStatsStruct nic_info;
    string         nic_path = "vnet" + to_string<int>(nic_no,dec);

    Lock();
    dom = virDomainLookupByID(connect, domid);
    if (NULL != dom)
    {
        err = virDomainInterfaceStats(dom, nic_path.c_str(), &nic_info, sizeof(nic_info));
        virDomainFree(dom);
    }
    Unlock();
    if (err < 0)
    {
        return err;
    }

    info.rx_bytes = nic_info.rx_bytes;
    info.rx_packets = nic_info.rx_packets;
    info.rx_errs = nic_info.rx_errs;
    info.rx_drop = nic_info.rx_drop;
    info.tx_bytes = nic_info.tx_bytes;
    info.tx_packets = nic_info.tx_packets;
    info.tx_errs = nic_info.tx_errs;
    info.tx_drop = nic_info.tx_drop;

    return 0;
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
VmInstanceState KvmDriver::GetDomainState(const string& name)
{
    int            err = -1;
    virDomainPtr   dom = NULL;
    virDomainInfo  domInfo;
    virConnectPtr  connect = CheckHypervisorConnect();

    Lock();
    dom   = virDomainLookupByName(connect, name.c_str());
    if (NULL != dom)
    {
        err = virDomainGetInfo(dom, &domInfo);
        virDomainFree(dom);
    }
    Unlock();
    if (err < 0)
    {
        return InvertToVmState(err);
    }

    return InvertToVmState(domInfo.state);
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
* 2011/08/12    V1.0    lxch         ����
****************************************************************************************/
int KvmDriver::GetDomainIDByName(const string& domname)
{
    int domid = -1;
    virDomainPtr dom = NULL;
    virConnectPtr connect = CheckHypervisorConnect();

    Lock();
    dom   = virDomainLookupByName(connect, domname.c_str());
    if (NULL != dom)
    {
        domid = virDomainGetID(dom);
        virDomainFree(dom);
    }
    Unlock();

    return domid;
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
string KvmDriver::GetDomainDescByName(const string& domname)
{
    string desc;

    XmlParser xml;
    char   *pxml = NULL;
    virDomainPtr dom = NULL;
    virConnectPtr connect = CheckHypervisorConnect();

    Lock();
    dom   = virDomainLookupByName(connect, domname.c_str());
    if (NULL != dom)
    {
        pxml = virDomainGetXMLDesc(dom, VIR_DOMAIN_XML_SECURE);
        virDomainFree(dom);
        if (pxml != NULL)
        {
            //cout << pxml <<endl;
            xml.Locate(pxml, "domain");
            xml.Read("description", desc);
            free(pxml);
        }
    }
    Unlock();

    return desc;
}

/****************************************************************************************
* �������ƣ�GetDomainDescByName
* ��������������ʵ������ø�ʵ����������Ϣ
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������ע�ͣ���Ҫ������libvirt9.1.2�󣬽Ӵ���ֵ
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/08/27    V1.0    lixch        ����
****************************************************************************************/
int KvmDriver::SetDomainDescByName(const string& domname, const string& desc)
{
    int rc = -1;
#if 0
    virDomainPtr dom = NULL;
    virConnectPtr connect = CheckHypervisorConnect();

    Lock();
    dom   = virDomainLookupByName(connect, domname.c_str());
    if (NULL != dom)
    {
        rc = virDomainSetMetadata(dom,
                                  VIR_DOMAIN_METADATA_DESCRIPTION,
                                  desc.c_str(),
                                  NULL,
                                  NULL,
                                  VIR_DOMAIN_AFFECT_CURRENT);
        virDomainFree(dom);
    }
    Unlock();
#endif
    return rc;
}



/****************************************************************************************
* �������ƣ�FreeDomain
* ��������������ʵ������ͷ���Ӧ��ʵ���ṹ
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lxch         ����
****************************************************************************************/
void KvmDriver::FreeDomain(virDomainPtr dom)
{
    if(NULL == dom)
    {
        return;
    }

    Lock();
    virDomainFree(dom);
    Unlock();
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
* 2011/08/12    V1.0    lxch         ����
****************************************************************************************/
int KvmDriver::DestroyDomain(const string &dom_name)
{
    int err  = -1;
    virDomainPtr dom = GetDomainByInstanceName(dom_name);
    Lock();
    if (NULL != dom)
    {
        err = virDomainDestroy(dom);
        virDomainFree(dom);
    }
    Unlock();

    return err;
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
VmInstanceState KvmDriver::InvertToVmState(int state_in)
{
    switch(state_in)
    {
    case LIBV_NO_STATE:
    case LIBV_RUNNING:
    case LIBV_BLOCKED:
    {
        return VM_RUNNING;
        break;
    }
    case LIBV_PAUSED:
    {
        return VM_PAUSED;
        break;
    }
    case LIBV_SHUTDOWN:
    case LIBV_SHUTOFF:
    {
        return VM_SHUTOFF;
        break;
    }
    case LIBV_CRASHED:
    {
        return VM_CRASHED;
        break;
    }
    default:
        // �п�������һ��-1����ʾ�ⲿִ�в�ѯlibvirtʧ��
        return VM_SHUTOFF;
    }
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
string KvmDriver::CreateConfig(const string vm_ins_name, VMConfig &vm_cfg)
{
    XmlParser xml;
    xml.Create("domain");
    xml.SetAttr("type","kvm");

    xml.Write("name",vm_ins_name);
    xml.Write("description", to_string<uint32>(vm_cfg._config_version,dec));


    CreateOsXml(xml, vm_cfg);
    CreateFeaturesXml(xml, vm_cfg);
    xml.Write("memory",to_string<int64>(vm_cfg._memory/1024,dec));
    xml.Write("vcpu",to_string<uint32>(vm_cfg._vcpu,dec));
    xml.Write("on_poweroff", "destroy");
    xml.Write("on_reboot", "restart");

    // ���coredump����ʹ�ܵĻ������޸Ķ�Ӧ��crash ����ʽ
    if (vm_cfg._core_dump_url.ip != "")
    {
        xml.Write("on_crash", "coredump-destroy");
    }
    else
    {
        xml.Write("on_crash", "restart");
    }

    // tsc_mode = 2 ��ʾ����ģ��tsc������ֱ�Ӷ�ȡtsc
    xml.Write("tsc_mode", "2");

    xml.Write("devices","");   // ����device����
    xml.Enter();

    CreateemulatorXml(xml, vm_cfg);

    CreateMachineXml(xml, vm_cfg);

    CreateAllDiskXml(xml, vm_cfg);

    CreateNicXml(xml, vm_cfg);

    CreateWdgXml(xml, vm_cfg);

    CreateSerialXml(xml, vm_cfg);

    CreateVncXml(xml, vm_cfg);

    CreatePciDeviceXml(xml, vm_cfg);

    CreateMouseXml(xml, vm_cfg);

    xml.Exit();     // �˳�device����

    return xml.DocToString();
}

/************************************************************************/
void KvmDriver::CreateOsXml(XmlParser &xml, VMConfig &vm_cfg)
{
    xml.Write("os","");

    xml.Enter();
    xml.Write("type","hvm");
    xml.SetAttr("arch", "x86_64");
    xml.Exit();
}

/************************************************************************/
void KvmDriver::CreateFeaturesXml(XmlParser &xml, VMConfig &vm_cfg)
{
    xml.Write("features","");
    xml.Enter();
    xml.Write("apic","");
    xml.Write("acpi","");
    xml.Write("pae","");
    xml.Write("hap", "1");
    xml.Exit();
}

/************************************************************************/
void KvmDriver::CreateemulatorXml(XmlParser &xml, VMConfig &vm_cfg)
{
	if (0 != access("/usr/libexec/qemu-kvm", X_OK))
	{
		xml.Write("emulator", "/usr/bin/qemu-system-x86_64");
	}
	else
	{
		xml.Write("emulator", "/usr/libexec/qemu-kvm");
	} 
}

/************************************************************************/
void KvmDriver::CreateMachineXml(XmlParser &xml, VMConfig &vm_cfg)
{
    CreateDiskXml(xml, vm_cfg._machine, vm_cfg);
}

/************************************************************************/
void KvmDriver::CreateAllDiskXml(XmlParser &xml, VMConfig &vm_cfg)
{
    vector<VmDiskConfig>::iterator itdisk;
    for(itdisk = vm_cfg._disks.begin(); itdisk != vm_cfg._disks.end(); itdisk++)
    {
        CreateDiskXml(xml, *itdisk, vm_cfg);
    }
}

/* modify by wangtl: readonly device must by cdrom in kvm  */
/* cdrom is in ide bus   */
/* other than ide, on the virtio bus   */
/* hda vda ????? sda */
/************************************************************************/
void KvmDriver::CreateDiskXml(XmlParser &xml, const VmDiskConfig &disk, VMConfig &vm_cfg)
{
    int is_cdrom = 0;
    int is_ide      = 0;
    xml.Write("disk","");
	
    xml.SetAttr("type",disk._source_type);
    if(((disk._target == "hdc")  &&(disk._id != INVALID_OID)) ||
            (disk._type  == "ro_disk"))
    {
        xml.SetAttr("device","cdrom");
        is_cdrom = 1;
    }
    else
    {
        xml.SetAttr("device","disk");
    }

    if (disk._bus == "ide" || disk._target.at(0) == 'h')
    {
        is_ide = 1;
    }

    xml.Enter();
    if(STORAGE_SOURCE_TYPE_FILE == disk._source_type)
    {
        xml.Write("driver","");
        xml.SetAttr("name","qemu");
	 
        if(IMAGE_FORMAT_QCOW2 == disk._disk_format)
        {
            xml.SetAttr("type","qcow2");
        }
        else
        {
            xml.SetAttr("type","raw");
        }
    }
    xml.Write("source","");
    if(STORAGE_SOURCE_TYPE_BLOCK == disk._source_type)
    {
        xml.SetAttr("dev",disk._source);
    }
    else
    {
        xml.SetAttr("file",disk._source);
    }

    xml.Write("target","");
    xml.SetAttr("dev",disk._target);
    if (!is_cdrom && !is_ide)
    {
        xml.SetAttr("bus", "virtio");
    }
    else
    {
        xml.SetAttr("bus",disk._bus);
    }

    if (is_cdrom)
    {
        xml.Write("readonly","");
    }
    xml.Exit();
}

/*<graphics type='vnc' port='-1' autoport='yes'/> */
/* the kvm format is above  */
/************************************************************************/
void KvmDriver::CreateVncXml(XmlParser &xml, VMConfig &vm_cfg)
{
    xml.Write("graphics","");
    xml.SetAttr("type","vnc");
    xml.SetAttr("port",to_string<int>(vm_cfg._vnc_port,dec));
    xml.SetAttr("passwd",vm_cfg._vnc_passwd);

    xml.Enter();
    xml.Write("listen","");
    xml.SetAttr("type","address");
    xml.SetAttr("address","0.0.0.0");
    xml.Exit();
}

/************************************************************************/
//���vnc�ͻ����������˫�������
void KvmDriver::CreateMouseXml(XmlParser &xml, VMConfig &vm_cfg)
{
    /** only enable one of the following, `mouse' for PS/2 protocol relative mouse,
      * `tablet' for absolute mouse
    */
    xml.Write("input","");
    xml.SetAttr("type","tablet");
    xml.SetAttr("bus","usb");
}

/************************************************************************/
void KvmDriver::CreatePciDeviceXml(XmlParser &xml, VMConfig &vm_cfg)
{
    vector<PciBus>::iterator it_bus;
    for (it_bus = vm_cfg._pci_devices.begin(); it_bus != vm_cfg._pci_devices.end(); it_bus ++)
    {
        WritePciDeviceXml(xml, it_bus->_domain, it_bus->_bus, it_bus->_slot, it_bus->_function);
    }
}

/************************************************************************/
void KvmDriver::WritePciDeviceXml(XmlParser &xml, string domain, string bus, string slot, string function)
{
    xml.Write("hostdev","");
    xml.SetAttr("mode","subsystem");
    xml.SetAttr("type","pci");
    xml.SetAttr("managed","no");
    xml.Enter();
    xml.Write("source","");
    xml.Enter();
    xml.Write("address","");

    xml.SetAttr("domain",domain);
    xml.SetAttr("bus",bus);
    xml.SetAttr("slot",slot);
    xml.SetAttr("function",function);
    xml.Exit();
    xml.Exit();
}

/************************************************************************/
void KvmDriver::CreateSerialXml(XmlParser &xml, VMConfig &vm_cfg)
{

    if(vm_cfg._enable_serial)
    {
        xml.Write("serial","");
        xml.SetAttr("type","tcp");
        xml.Enter();
        xml.Write("source","");
        xml.SetAttr("mode","bind");
        xml.SetAttr("host","0.0.0.0");
        xml.SetAttr("service",to_string<int>(vm_cfg._serial_port,dec));
        xml.Write("protocol","");
        xml.SetAttr("type","telnet");
        xml.Write("target","");
        xml.SetAttr("port","0");
        xml.Exit();

        xml.Write("console","");
        xml.SetAttr("type","tcp");
        xml.Enter();
        xml.Write("source","");
        xml.SetAttr("mode","bind");
        xml.SetAttr("host","0.0.0.0");
        xml.SetAttr("service",to_string<int>(vm_cfg._serial_port,dec));
        xml.Write("protocol","");
        xml.SetAttr("type","telnet");
        xml.Write("target","");
        xml.SetAttr("port","0");
        xml.Exit();
    }
    else
    {
        xml.Write("serial","");
        xml.SetAttr("type","pty");
        xml.Enter();
        //libvirtԴ��ע�ͽ���live xmlʱ�������host interface����������²�ʹ�á�
        xml.Write("source","");
        xml.SetAttr("path","/dev/pts/3");
        //guest interface
        xml.Write("target","");
        xml.SetAttr("port","0");
        xml.Exit();
    }
}

/************************************************************************/
void KvmDriver::CreateWdgXml(XmlParser &xml, VMConfig &vm_cfg)
{
    bool have_qemu_nic = false;
    vector<VmNicConfig>::const_iterator itnic;
    for(itnic = vm_cfg._nics.begin(); itnic != vm_cfg._nics.end(); itnic++)
    {
        if(true == itnic->_sriov)
        {
            continue;
        }

        if ((!vm_cfg._virt_type.compare(VIRT_TYPE_HVM))  /* HVM ,PVM����д*/
                &&(itnic->_model != "netfront"))
        {
            have_qemu_nic = true;
        }
    }

    // ���Ź�������д�������ļ���
    if(!vm_cfg._wdg_nics._mac.empty())
    {
        vm_cfg._wdg_nics._model = have_qemu_nic ? "" : "netfront";
        CreateNomalNicXml(xml, vm_cfg._wdg_nics, vm_cfg);
    }
}


/************************************************************************/
void KvmDriver::CreateNicXml(XmlParser &xml, VMConfig &vm_cfg)
{
    vector<VmNicConfig>::iterator itnic;
    for(itnic = vm_cfg._nics.begin(); itnic != vm_cfg._nics.end(); itnic++)
    {
        if(true == itnic->_sriov)
        {
            CreateSriovNicXml(xml, *itnic, vm_cfg);
        }
        else
        {
            CreateNomalNicXml(xml, *itnic, vm_cfg);
        }
    }
}

/************************************************************************/
void KvmDriver::CreateSriovNicXml(XmlParser &xml, const VmNicConfig &nic, VMConfig &vm_cfg)
{
    string domain,bus,slot,function;
    if(true == nic._sriov)
    {
        ParsePciNo(nic._pci_order, domain, bus, slot, function);
        WritePciDeviceXml(xml, domain, bus, slot, function);
    }
}

/************************************************************************/
/* modify by wangtl for kvm  */
/* in kvm bridge is virtio  */
void KvmDriver::CreateNomalNicXml(XmlParser &xml, const VmNicConfig &nic, VMConfig &vm_cfg)
{
    if(true == nic._sriov)
    {
        return;
    }

    xml.Write("interface","");
    xml.SetAttr("type","bridge");
    xml.Enter();
    xml.Write("source","");
    xml.SetAttr("bridge",nic._bridge_name);

    if ((!vm_cfg._virt_type.compare(VIRT_TYPE_HVM))  /* HVM ,PVM����д*/
            &&(nic._model.empty()!= true))
    {
        xml.Write("model","");

        if (nic._model == "netfront")
        {
            xml.SetAttr("type", "virtio");
        }
        else
        {
            xml.SetAttr("type",nic._model);
        }
    }

    if(nic._mac.empty()!= 1)
    {
        xml.Write("mac","");
        xml.SetAttr("address",nic._mac);
    }
    if(nic._ip.empty()!= 1)
    {
        xml.Write("ip","");
        xml.SetAttr("address",nic._ip);
    }

    xml.Write("virtualport","");
    xml.SetAttr("type","openvswitch");
    xml.Enter();
    xml.Write("parameters","");
    xml.SetAttr("interfaceid",nic._vsi_profile_id);
    xml.Exit();


    xml.Exit();
}

int KvmDriver::AddRemovableDisk(const string& instance_name,  const VmDiskConfig &disk)
{
    XmlParser xml;
    int err = 0;
    VMConfig vm_cfg;
    CreateDiskXml(xml, disk,vm_cfg);
    virConnectPtr connect = CheckHypervisorConnect();
    Lock();
    virDomainPtr dom = virDomainLookupByName(connect, instance_name.c_str());
    if (dom != NULL)
    {
        err=virDomainAttachDevice(dom, xml.DocToString().c_str());
        virDomainFree(dom);
    }
    Unlock();

    return err;
}

int KvmDriver::DelRemovableDisk(const string& instance_name,  const VmDiskConfig &disk)
{
    XmlParser xml;
    int err = 0;
    VMConfig vm_cfg;
    CreateDiskXml(xml, disk,vm_cfg);
    virConnectPtr connect = CheckHypervisorConnect();
    Lock();
    virDomainPtr dom = virDomainLookupByName(connect, instance_name.c_str());
    if (dom != NULL)
    {
        err=virDomainDetachDevice(dom, xml.DocToString().c_str());
        virDomainFree(dom);
    }
    Unlock();
    return err;
}


/****************************************************************************************
* �������ƣ�libvirt_error_handler
* ����������libvirt ��������
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2011/09/07    V1.0    jixp         ����
****************************************************************************************/
void kvm_error_handler (void *userData,virErrorPtr error)
{
    if (NULL == error)
    {
        OutPut(SYS_ALERT,"libvirt error handler was given a NULL pointer \n");
        return;
    }

    switch(error->code)
    {
        case VIR_ERR_NO_DOMAIN:
        case VIR_ERR_INVALID_DOMAIN:
        {
            OutPut(SYS_ALERT, "libvirt: %s (code=%d)\n", error->message, error->code);
            break;
        }
        default:
        {
            OutPut(SYS_ALERT, "libvirt: %s (code=%d) (dom=%d\n", error->message, error->code, error->domain);
            break;
        }
    }
}

#endif
