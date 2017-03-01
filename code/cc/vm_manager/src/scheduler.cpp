/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�scheduler.cpp
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ�����������ģ��ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ����Ľ�
* ������ڣ�2011��8��16��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/07/01
*     �� �� �ţ�V1.0
*     �� �� �ˣ����Ľ�
*     �޸����ݣ�����
*******************************************************************************/
#include "scheduler.h"
#include "vm_manager.h"
#include "vstorage_manager.h"
#include "lifecycle_manager.h"
#include "host_pool.h"
#include "log_agent.h"
#include "tecs_config.h"
#include "vm_image.h"
#include "tecs_pub.h"
#include "vnetlib_msg.h"
#include "config_cluster.h"
#include "config_host_atca.h"

static int ignore_res_on = 0;
DEFINE_DEBUG_VAR(ignore_res_on);
static int sche_print_on = 0;
DEFINE_DEBUG_VAR(sche_print_on);
static int max_try_hosts = 1;
DEFINE_DEBUG_VAR(max_try_hosts);
#define Debug(level,fmt,arg...) \
        do \
        { \
            if(sche_print_on) \
            { \
                printf("[%s:%d]"fmt,__func__,__LINE__, ##arg); \
            } \
            OutPut(level,fmt,##arg);\
        }while(0)

namespace zte_tecs
{
#define EV_SCHEDULE_TIMER EV_TIMER_1
#define EV_RUNNING_SCH_TIMER EV_TIMER_2

#define VNC_PORT_MIN      (5900)
#define VNC_PORT_MAX      (65535)
#define SERIAL_PORT_MIN   (20000)
#define SERIAL_PORT_MAX   (65535)

//���Ȳ��Գ������
class SchedulerHostPolicy
{
public:
    virtual int64 Select(const SchedulingVM &schdl_vm) = 0;
    virtual ~SchedulerHostPolicy() {};

void SortHost(vector<int64> &hosts, const string &orderby)
{
    HostPool *host_pool = HostPool::GetInstance();
    SkyAssert(host_pool);

    host_pool->SortHost(hosts, orderby);
}

void SortbyImage(vector<int64> &hosts, VMConfig const &vmcfg)
{
    Scheduler *schdl = Scheduler::GetInstance();
    schdl->SortbyImage(vmcfg, hosts);
}

};

//��򵥵ļ�Ⱥ�������Ȳ���:�ӷ���������������ѡ���һ��
class SimplePolicy:SchedulerHostPolicy
{
public:
    
    virtual int64 Select(const SchedulingVM &schdl_vm)
    {
        if(schdl_vm._hids.empty())
            return INVALID_OID;

        vector<int64> hids = schdl_vm._hids;
        SortHost(hids, "order by mem_free desc");
        SortbyImage(hids, schdl_vm._config);
        return hids[0];
    };
    virtual ~SimplePolicy() {};
};

//�������Ȳ��ԣ����ǽ�������������е�HC��
class PackingPolicy:SchedulerHostPolicy
{
public:
    
    virtual int64 Select(const SchedulingVM &schdl_vm)
    {
        if(schdl_vm._hids.empty())
            return INVALID_OID;

        vector<int64> hids = schdl_vm._hids;
        SortHost(hids, "order by running_vms desc");	//��HC�ϵ����������������
        //SortbyImage(hids, schdl_vm._config);
        return hids[0];
    };
    virtual ~PackingPolicy() {};
};

//��ɢ���Ȳ��ԣ����ǽ������������ɢ��ÿ��HC
class StripingPolicy:SchedulerHostPolicy
{
public:
    
    virtual int64 Select(const SchedulingVM &schdl_vm)
    {
        if(schdl_vm._hids.empty())
            return INVALID_OID;

        vector<int64> hids = schdl_vm._hids;
        SortHost(hids, "order by running_vms");	//��HC�ϵ����������������
        //SortbyImage(hids, schdl_vm._config);
        return hids[0];
    };
    virtual ~StripingPolicy() {};
};

//todo: ʵ�ֶ��ַḻ�ļ�Ⱥ���Ȳ���

Scheduler *Scheduler::instance = NULL;

STATUS Scheduler::Init()
{
    vm_pool = VirtualMachinePool::GetInstance();
    host_pool = HostPool::GetInstance();
    if(!vm_pool || !host_pool)
    {
        return ERROR_NOT_READY;
    }

    //������ע�ᡢ������Ϣ��Ԫ
    STATUS ret = Start(MU_VM_SCHEDULER);
    if(SUCCESS != ret)
    {
        return ret;
    }

    //���Լ���һ���ϵ�֪ͨ��Ϣ
    MessageOption option(MU_VM_SCHEDULER,EV_POWER_ON,0,0);
    MessageFrame frame(SkyInt(0),option);
    return m->Receive(frame);
}

void Scheduler::MessageEntry(const MessageFrame& message)
{
    STATUS ret;
    ostringstream oss;
    switch(m->get_state())
    {
        case S_Startup:
        {
            switch(message.option.id())
            {
                case EV_POWER_ON:
                {
                    printf("scheduler power on!\n");
                    SkyAssert(max_try_hosts > 0);
                    ret = Transaction::Enable(GetDB());
                    if(SUCCESS != ret)
                    {
                        Debug(SYS_ERROR,"scheduler enable transaction fail! ret = %d",ret);
                        SkyExit(-1, "Scheduler::MessageEntry: call Transaction::Enable(GetDB()) failed.");
                    }

                    VmAssistant = new VmInterface(m);
                    SkyAssert(VmAssistant);

                    //�������µ��ȶ�ʱ���������Գ��Խ�û���������������½�������
                    if (schedule_timer == INVALID_TIMER_ID)
                    {
                        schedule_timer = m->CreateTimer();
                        TimeOut to;
                        to.duration = 1000 * 30;
                        to.type = LOOP_TIMER;
                        to.msgid = EV_SCHEDULE_TIMER;
                        m->SetTimer(schedule_timer,to);
                    }

                    SetRunningReSCH();
                    m->set_state(S_Work);
                    OutPut(SYS_NOTICE, "%s power on!\n",m->name().c_str());
                    break;
                }
                default:
                    break;
            }
            break;
        }

        case S_Work:
        {
            switch(message.option.id())
            {
                case EV_VM_SCHEDULE_SINGLE:
                {
                    SchSingle(message, oss);
                    break;
                }

                //todo:ʵ����������
                case EV_VM_SCHEDULE_BATCH:
                {
                    //�������SelectHost֮��������ɹ��ˣ����������Schedule
                    break;
                }

                case EV_VM_RESCHEDULE_SINGLE:
                {
                    ReSchSingle(message, oss);
                    break;
                }

                case EV_SCHEDULE_TIMER:
                {
                    //��ȡhid=-1������������½�������
                    vector<int64> vms;
                    vms.clear();
                    //���ڷ��ϳ���������ÿ��host��ȡ���������е�������б�
                    vm_pool->GetVmsByHost(vms,INVALID_HID,1000);
                    vector<int64>::iterator it;
                    for (it=vms.begin();it!=vms.end();it++)
                    {
                        Debug(SYS_DEBUG,"schedule timer will select host for vm %lld.\n",*it);
                        VmOperationReq req(*it,RESCHEDULE,SCHED_STAMP);
                        Schedule(req,oss);
                    }
                    break;
                }

                case EV_VM_MIGRATE:
                {
                    MigrateVM(message,oss);
                    break;
                }

                case EV_VMCFG_SYNC_REQ:
                {
                    HandleChngCfgReq(message);
                    break;
                }

                case EV_VM_COLD_MIGRATE:
                {
                    ColdMigrateVM(message,oss);
                    break;
                }

                case EV_VM_SCH_POLICY_SET_REQ:
                {
                    SetRunningReSCH();
                    break;
                }

                case EV_RUNNING_SCH_TIMER:
                {
                    RunningReSCH();
                    break;
                }

                case EV_VM_OP_ACK:
                {
                    ResponseWorkAck(GetMsg<WorkAck>(message));
                    break;
                }

                default:
                    break;
            }
            break;
        }

        default:
            break;
    }
}

bool Scheduler::VerifyVmRelations(const vector<int64>& existing,
                                    const vector<int64>& repels) const
{
    bool result = true;

    if(result && repels.size() > 0 && existing.size() > 0)
    {
        vector<int64>::const_iterator vit;
        //����host���Ƿ���ڻ���������
        for(vit=repels.begin();vit!=repels.end();vit++)
        {
            if(find(existing.begin(),existing.end(),*vit) != existing.end())
            {
                //����һ�������vm�����ڸ�host��ʱ����֤ʧ��
                result = false;
                break;
            }
        }
    }

    return result;
}

/* ����TCU,MEMORY����Ϣ���� */
STATUS Scheduler::FilterHostbyBaseInfo(SchedulingVM &schdl, string &err)
{
    vector<int64> hosts_checking;

    //-----------------------------------------------------------------------
    //�����ҳ���������cpu/memory/disk�Լ�qualifications�������
    Debug(SYS_DEBUG,"Checking computing capability for vm %lld ...\n",schdl._config._vid);
    ostringstream where;
    where << " cpus >= " << schdl._config._vcpu;
    where << " AND tcu_num >= " << schdl._config._tcu;
    where << " AND all_free_tcu >= " << schdl._config._tcu*schdl._config._vcpu;
    where << " AND mem_free >= " << schdl._config._memory + 9*1024*1024;

    //if (!IsUseSnapshot())
    {/* ������ÿ��գ���ֱ�Ӽ��� */
        int64 local_size = CalcLocalDiskSize(schdl._config, 0);
        where << " AND disk_free >= " << local_size;
    }

    vector<Expression>::const_iterator eit;
    for(eit=schdl._require._qualifications.begin();eit!=schdl._require._qualifications.end();eit++)
    {
        where << " AND " << eit->_key << " " << eit->_op << " " << eit->_value;
    }

    if(schdl._config._virt_type == VIRT_TYPE_HVM)
    {
        where << " AND is_support_hvm = 1 ";
    }

    if (schdl._config._hypervisor == "xen")
    {
        where << "and vmm_info LIKE 'xen%'";
    }
    else if (schdl._config._hypervisor == "kvm")
    {
        where << "and vmm_info LIKE 'kvm%'";
    }
    
    /* ��hid��С����, �����Ŀ����Ϊ�˺�����vector���� */
    where << " order by oid ";

    HostPool *host_pool = HostPool::GetInstance();
    Debug(SYS_DEBUG,"WHERE to select host for vm %lld: \n%s\n",schdl._config._vid,where.str().c_str());
    host_pool->Match(hosts_checking, where.str());

    if (schdl._hids.empty())
    {
        schdl._hids = hosts_checking;
    }
    else    //ֻ��hids�ǿյ�ʱ�򣬲���Ҫ�󽻼�
    {
        vector<int64> tmp;
        set_intersection(schdl._hids.begin(), schdl._hids.end(),
                            hosts_checking.begin(), hosts_checking.end(),
                            back_inserter(tmp));    //�󽻼�
        schdl._hids = tmp;
    }
    
    if(schdl._hids.empty())
    {
        Debug(SYS_NOTICE,"Computing capability match failed for vm %lld!\n",schdl._config._vid);
        return ERROR_RESOURCE_UNAVAILABLE;
    }

    return SUCCESS;
}

/* ����TCU,MEMORY����Ϣ���� */
STATUS Scheduler::FilterHostbyImage(SchedulingVM &schdl, string &err)
{
    if (!IsUseSnapshot())
    {
        return SUCCESS;
    }
    
    /* select oid from host_pool_in_use_with_cpuinfo where disk_free >= 25769803776 
and oid  in (select hid from host_using_image where image_id=20121029123300001) and oid in (1,2,3)
order by oid  */
    HostPool *host_pool = HostPool::GetInstance();

    ostringstream where;

    int64 min_local_size = CalcLocalDiskSize(schdl._config, 0);
    where << " disk_free >= " << min_local_size;
    where << " and oid  in (select hid from host_using_image where image_id = " << schdl._config._machine._id << " )";
    vector<int64> hids_with_image;
    host_pool->Match(hids_with_image, where.str());

    int64 max_local_size = CalcLocalDiskSize(schdl._config, 1);
    where.str("");
    where << " disk_free >= " << max_local_size;
    where << " and oid not in (select hid from host_using_image where image_id = " << schdl._config._machine._id << " )";
    vector<int64> hids_no_image;
    host_pool->Match(hids_no_image, where.str());

    /* �ϲ� */
    hids_no_image.insert(hids_no_image.end(), hids_with_image.begin(), hids_with_image.end());

    /* �󽻼� */
    vector<int64> tmp;
    sort(hids_no_image.begin(), hids_no_image.end());
    set_intersection(schdl._hids.begin(), schdl._hids.end(),
                        hids_no_image.begin(), hids_no_image.end(),
                        back_inserter(tmp));    //�󽻼�
    schdl._hids = tmp;

    if(schdl._hids.empty())
    {
        Debug(SYS_NOTICE,"Computing disk match failed for vm %lld!\n",schdl._config._vid);
        return ERROR_RESOURCE_UNAVAILABLE;
    }

    return SUCCESS;
}

STATUS Scheduler::FilterHostbyCancels(SchedulingVM &schdl, string &err)
{
    return SUCCESS;
}

STATUS Scheduler::FilterHostbyNetwork(SchedulingVM &schdl, string &err)
{
    if (schdl._config._nics.empty())
    {
        return SUCCESS;
    }
    else
    {
        string str_ack;
        STATUS ret = GetActionMessageAck(schdl._deploy.action_id, VM_ACT_SELECT_HC_BY_NIC, str_ack);
        SkyAssert(ret == SUCCESS);
        CVNetSelectHCListMsgAck select_hc_ack;
        bool result = select_hc_ack.deserialize(str_ack);
        SkyAssert(result);
        vector<int64> hid_by_nic;
        vector<string>::iterator it;
        for (it = select_hc_ack.m_select_hc_list.begin(); it != select_hc_ack.m_select_hc_list.end(); it++)
        {
            int hid = host_pool->GetHostIdByName(*it);
            SkyAssert(hid != INVALID_HID);
            hid_by_nic.push_back(hid);
        }

        sort(hid_by_nic.begin(), hid_by_nic.end());

        vector<int64> tmp;
        set_intersection(schdl._hids.begin(), schdl._hids.end(),
                            hid_by_nic.begin(), hid_by_nic.end(),
                            back_inserter(tmp));    //�󽻼�

        schdl._hids = tmp;
    }

    if(schdl._hids.empty())
    {
        //�������ɸѡ���Ϊ�㣬ֱ�ӷ��ش���
        Debug(SYS_NOTICE,"Network availability checking failed for vm %lld!\n",schdl._config._vid);
        return ERROR_RESOURCE_UNAVAILABLE;
    }

    return SUCCESS;
}

/* ��һ�����˳������������������������ϵҪ�������� */
STATUS Scheduler::FilterHostbyRepel(SchedulingVM &schdl, string &err)
{
    Debug(SYS_DEBUG,"Checking attract and repel for vm %lld ...\n",schdl._config._vid);
    //����û�ָ�������������������Ķ��󣬻�Ҫ�ڷ�����������������ٴμ��
    if(schdl._require._repel_vms.size() > 0)
    {
        vector<int64> tmp;
        vector<int64>::iterator oit;
        for(oit=schdl._hids.begin();oit!=schdl._hids.end();oit++)
        {
            vector<int64> existing_vms;
            existing_vms.clear();
            //���ڷ��ϳ���������ÿ��host��ȡ���������е�������б�
            vm_pool->GetVmsByHostAndNext(existing_vms, *oit, 1000);

            //��������֤ͨ����host����hosts_passed
            if(true == VerifyVmRelations(existing_vms,
                schdl._require._repel_vms))
            {
                tmp.push_back(*oit);
            }
        }

        schdl._hids = tmp;
    }
    else
    {
        //�û�û��ָ�����������������ϵ�����д������������ͨ�����ּ��
        return SUCCESS;
    }

    if(schdl._hids.empty())
    {
        //�������ɸѡ���Ϊ�㣬ֱ�ӷ��ش���
        Debug(SYS_NOTICE,"attract and repel checking failed for vm %lld.\n",schdl._config._vid);
        return ERROR_RESOURCE_UNAVAILABLE;
    }

    return SUCCESS;
}

STATUS Scheduler::FilterHostbySDevice(SchedulingVM &schdl, string &err)
{
    
    //��������豸
    if (schdl._config._devices.empty())
    {
         return SUCCESS;
    }
    else
    {
        vector<int64> hids;
        HostPool *host_pool = HostPool::GetInstance();
        SkyAssert(host_pool);
        host_pool->MatchSDevice(hids, schdl._config._devices);

        sort(hids.begin(), hids.end());

        vector<int64> tmp;
        set_intersection(schdl._hids.begin(), schdl._hids.end(),
                            hids.begin(), hids.end(),
                            back_inserter(tmp));    //�󽻼�
        schdl._hids = tmp;
    }

    if(schdl._hids.empty())
    {
        //�������ɸѡ���Ϊ�㣬ֱ�ӷ��ش���
        Debug(SYS_NOTICE,"Special devices check failed for vm %lld.\n",schdl._config._vid);
        return ERROR_RESOURCE_UNAVAILABLE;
    }

    return SUCCESS;    
}

STATUS Scheduler::FilterHostbyRegion(SchedulingVM &schdl, string &err)
{
    if (schdl._require._affinity_region_id == INVALID_OID)
    {
        return SUCCESS;
    }

    int64 region_hid = INVALID_HID;
    ostringstream where;
    where << "region_id = " << schdl._require._affinity_region_id << endl;
    vm_pool->SelectColumn("vm_pool", "hid", where.str(), region_hid);

    where.str("");
    vector<int64> region_hids;
    if (region_hid == INVALID_HID)
    {//˵��ĿǰCC�ϻ�û��ͬһ��Ȧ�ӵ������,���Ǹ�Ȧ�����һ������������
        if (schdl._require._affinity_region_level == "rack")
        {
            where << "rack != 0";
        }
        else if (schdl._require._affinity_region_level == "shelf")
        {
            where << "shelf != 0";
        }
        else if (schdl._require._affinity_region_level == "board")
        {//ͬһ��HC��ֻҪHIDһ�¾Ϳ����ˣ���һ�����ȵ����������Ҫ��host_atca_with_host_pool����
            return SUCCESS;
        }
        else
        {
            SkyAssert(false);
        }
    }
    else
    {
        HostAtca *p = HostAtca::GetInstance();
        SkyAssert(p);
        host_atca_table_info region_hid_info;
        p->SearchByHostId(region_hid_info, region_hid);

        where << "bureau = " << region_hid_info.host_position.bureau;
        if (schdl._require._affinity_region_level == "rack")
        {
            where << " and rack = " << region_hid_info.host_position.rack;
        }
        else if (schdl._require._affinity_region_level == "shelf")
        {
            where << " and rack = " << region_hid_info.host_position.rack 
                << " and shelf = " << region_hid_info.host_position.shelf;
        }
        else if (schdl._require._affinity_region_level == "board")
        {
            //where << " and hid = " << region_hid;
            region_hids.push_back(region_hid);
            goto intersection;
        }
        else
        {
            SkyAssert(false);
        }
    }

    where << "order by hid" << endl;

    vm_pool->SelectColumn("host_atca_with_host_pool", "hid", where.str(), region_hids);

intersection:

    /* �󽻼� */
    vector<int64> tmp;
    sort(region_hids.begin(), region_hids.end());
    set_intersection(schdl._hids.begin(), schdl._hids.end(),
                        region_hids.begin(), region_hids.end(),
                        back_inserter(tmp));    //�󽻼�

    if (tmp.empty() && !schdl._require._affinity_region_forced)
    {//������Ƚ��Ϊ�գ����׺�Ȧ�ӷ�ǿ�ƣ��ǾͲ���
    //���Ҫ֧��Ȧ�ӵ�ǿ��/��ǿ�����ԣ��Ǳ����������������й������������
        return SUCCESS;
    }

    schdl._hids = tmp;

    if(schdl._hids.empty())
    {
        Debug(SYS_NOTICE,"Computing region match failed for vm %lld!\n",schdl._config._vid);
        return ERROR_RESOURCE_UNAVAILABLE;
    }

    return SUCCESS;
}

STATUS Scheduler::FilterHost(SchedulingVM &schdl)
{
    if(max_try_hosts == 0)
    {
        schdl._hids.clear();    //���
        Debug(SYS_NOTICE,"max_try_hosts = 0, vm %lld will not be scheduled!!!\n",schdl._config._vid);
        return ERROR_PERMISSION_DENIED;
    }
    
    string          err;
    FileterHost     filters[] = {&Scheduler::FilterHostbyBaseInfo,
                                &Scheduler::FilterHostbyImage,
                                &Scheduler::FilterHostbyCancels,
                                &Scheduler::FilterHostbyNetwork,
                                &Scheduler::FilterHostbyRepel,
                                &Scheduler::FilterHostbySDevice,
                                &Scheduler::FilterHostbyRegion,};

    uint32 i = 0;
    for (i = 0; i < sizeof(filters)/sizeof(filters[0]); i++ )
    {
        STATUS ret = (this->*filters[i])(schdl, err);
        if (ret != SUCCESS)
        {
            return ret;
        }
    }

    return 0;
}

STATUS Scheduler::SelectHost(int64& hid, const VMDeployInfo &deploy)
{
    SchedulingVM schdl(deploy);
    if(hid != INVALID_OID)
    {
        schdl._hids.push_back(hid);
    }

    STATUS ret = FilterHost(schdl);
    if (ret != SUCCESS)
    {
        return ret;
    }

    //-----------------------------------------------------------------------
    //���һ�ּ�飬���ü�Ⱥ���Ȳ��ԣ�����ApplyHostPolicyѡ�����յ�host
    ConfigCluster *p = ConfigCluster::GetInstance();
    SkyAssert(p);
    HostPolicyType policytype;
    const string policy = p->Get(CFG_SCH_POLICY);
    if (policy == SCH_POLICY_PACKING)
    {
        policytype = PACKING;
    }
    else if (policy == SCH_POLICY_STRIPING)
    {
        policytype = STRIPING;
    }
    else
    {
        policytype = SIMPLE_POLICY;
    }

    hid = ApplyHostPolicy(schdl, policytype);
    Debug(SYS_NOTICE,"vm %lld is scheduled to host %lld.\n",deploy._config._vid,hid);
    return SUCCESS;
}

//�����µĵ��������
STATUS Scheduler::SchSingle(const MessageFrame& message, ostringstream& oss)
{
    VMDeployInfo deploy;
    if(false == deploy.deserialize(message.data))
    {
        SkyAssert(false);
        return ERROR;
    }

    if (VmAssistant->CheckReqDoing(deploy.action_id))
    {
        return 0;
    }

    VmWfLock wf_lock(deploy.action_id);
    if (!wf_lock.IsLocked())
    {
        return ERROR;
    }

    STATUS ret = Schedule(deploy,oss);
    Debug(SYS_DEBUG,"%s\n",oss.str().c_str());

    if (ret == SUCCESS)
    {
        ModifyDeployWorkflow(deploy.action_id);
    }

    int progress = (ret == SUCCESS) ? 100 : 0;
    VmOperationAck ack(deploy.action_id, ret, progress, oss.str(), deploy._config._vid, DEPLOY);

    MessageOption option(message.option.sender(),EV_VM_OP_ACK,0,0);
    return m->Send(ack, option);
	
}

//�����µĵ������������������ʱ������ñ��ӿ�
STATUS Scheduler::Schedule(const VMDeployInfo& deploy,ostringstream& error)
{
    STATUS ret;
    int64 hid = INVALID_HID;
    if(!deploy._host.empty())
    {
        HostPool *host_pool = HostPool::GetInstance();
        if (!host_pool)
        {
            Debug(SYS_ERROR,"Scheduler::Schedule failed to get host_pool.\n");
            error << "Scheduler::Schedule failed to get host_pool.\n";            
            return ERROR_NOT_READY;
        }
        hid = host_pool->GetHostIdByName(deploy._host);
        if(hid == INVALID_HID)
        {
            Debug(SYS_ERROR,"No host %s in this cluster!\n",deploy._host.c_str());
            error << "No host " << deploy._host.c_str() << " in this cluster!\n";            
            return ERROR_RESOURCE_UNAVAILABLE;
        }
    }

    vector< PhyNic > ports;
    ports.clear();
    ret = SelectHost(hid, deploy);
    if(SUCCESS != ret)
    {
        error << "SelectHost failed, ret = " << ret;
        return ret;
    }

    //��vm-host��Թ�ϵ��⣬���ڱ�������˵�����ﶼ�������������
    VirtualMachine vm(deploy._config._vid);
//    vm._uid = deploy._uid;
    vm._project_id = deploy._project_id;
    vm._requirement = deploy._requirement;
    vm._appoint_host = deploy._host;

    vm._hid = hid;
    vm._hid_next = INVALID_HID;
    vm._vcpu = deploy._config._vcpu;
    vm._tcu = deploy._config._tcu;

    vm._config = deploy._config;
    vm._run_version = deploy._config._config_version;

    /* ��ȡvnc�ʹ��ڶ˿ں� */
    vector < uint16 > blacklist;
    int PortRet = GetNewVNCPortNo(vm._vnc_port,blacklist);
    if ((0 != PortRet)||(0 == vm._vnc_port))
    {
        error << "Schedule failed to get VNC portno. Ret = " << PortRet 
              << " port = " << vm._vnc_port;
        return ERROR_PORTNO_GET;
    }	
    blacklist.push_back(vm._vnc_port);

    if(deploy._config._enable_serial)
    {
        PortRet = GetNewSerialPortNo(vm._serial_port,blacklist);
        if ((0 != PortRet)||(0 == vm._serial_port))
        {
            error << "Schedule failed to get Serial portno. Ret = " << PortRet 
                  << " port = " << vm._serial_port;
            return ERROR_PORTNO_GET;
        }
        blacklist.push_back(vm._serial_port);
    }

    CalcVmCpuPercent(vm);
    vm._enable_serial = deploy._config._enable_serial;
    vm._requirement = deploy._requirement;
    vm._memory = deploy._config._memory;
    //vm._localdisk = deploy._config._local_disk_size;
    vm._cpu_rate = 0;
    vm._memory_rate = 0;
    vm._net_tx = 0;
    vm._net_rx = 0;
    vm._state = VM_INIT;
    vm._lastop = SCHEDULE; //���һ�β����ǵ���
    vm._lastopresult = SUCCESS;
    vm._lastopstamp = deploy._stamp;
    vm._lastoptimeout = 0;
    vm._lastopat.refresh();
    //���ȳɹ�����������������Դ״̬
    vm._lcm_state = LifecycleManager::Transition((LcmState)vm._lcm_state,
                                                    SCHEDULE,
                                                    LifecycleManager::SUCCSSED);
    int64 oid = INVALID_OID;
    vm_pool->Allocate(&oid,vm);
    if (oid == INVALID_OID)
    {
        error << "Schedule failed to allocate vid = " << deploy._config._vid;
        return ERROR_DB_INSERT_FAIL;
    }

    VirtualMachine *pvm = vm_pool->Get(deploy._config._vid,true);
    if(!pvm)
    {
        error << "Schedule can not find vid = " << deploy._config._vid;
        return ERROR_DB_SELECT_FAIL;
    }

    Transaction::Begin();

    VMImagePool *vmimage_pool = VMImagePool::GetInstance();
    SkyAssert(vmimage_pool);
    ret = vmimage_pool->InsertVMImageConfig(pvm->_vid,pvm->_config);
    SkyAssert(ret == 0);

    VmDevicePool *vmdevice_pool = VmDevicePool::GetInstance();
    SkyAssert(vmdevice_pool);
    ret = vmdevice_pool->InsertVmDevices(pvm->_config._devices, pvm->_vid);
    SkyAssert(ret == 0);

    if(SUCCESS != Transaction::Commit())
    {
        vm_pool->Drop(pvm);
        vm_pool->Put(pvm,true);
        SkyAssert(false);
        error << "Transaction::Commit failed!" << endl;
        return ERROR_DB_UNKNOWN_FAIL;
    }

    vm_pool->Put(pvm,true);

    return ret;
}

//���µ������е����������������ʱ������ñ��ӿ�
STATUS Scheduler::Schedule(const VmOperationReq& req,ostringstream& error)
{
    VirtualMachine *pvm = vm_pool->Get(req._vid,false);
    if(!pvm)
    {
        //�����Ҳ���������ǿ��ܳ��ֵģ�����ϵͳ������HA����ʱ���û�����ȥcancel�������
        error << "Schedule can not find vid = " << req._vid;
        return ERROR_DB_SELECT_FAIL;
    }

    VMDeployInfo deploy("", pvm->_config, pvm->_requirement, 0, pvm->_project_id);
    deploy.action_id = req.action_id;
    deploy._host = pvm->_appoint_host;

    if (pvm->_hid != INVALID_HID)
    {//vm�Ѿ���hc����
        //deploy._host = host_pool->GetHostNameById(pvm->_hid);
        // deploy._host �п����ڶ���״̬���޸�Ϊ��һ�µ�HC���ʲ���ͨ���޸�����ֶ����ⶳ
        Expression hc_id_exp("oid", "=", to_string<int64>(pvm->_hid,dec));
        deploy._requirement._qualifications.push_back( hc_id_exp );

        if (req._operation == UNFREEZE)
        {//�ⶳ�Ļ����洢����Ҫ���������
            deploy._config._machine._disk_size = 0;
            deploy._config._kernel._disk_size = 0;
            deploy._config._initrd._disk_size = 0;
            deploy._config._disks.clear();
        }
    }

    STATUS ret;
    int64 hid = INVALID_HID;
    ret = SelectHost(hid, deploy);
    if ( ret != SUCCESS )
    {
        error << "Schedule failed to re-sch vid = " << req._vid << ", ret = " << ret;
        Debug(SYS_WARNING,"%s\n",error.str().c_str());
        return ret;
    }

    //��ѡ����д��vm_pool
    pvm = vm_pool->Get(req._vid,true);
    SkyAssert(pvm);
    pvm->_hid       = hid;
    pvm->_log_state = VM_INIT;  //�������ֶ�Ҫͬʱ�޸ģ�����������ռ��Դ
    vm_pool->Update(pvm);
    vm_pool->Put(pvm, true);

    return ret;
}

STATUS Scheduler::ScheduleNow()
{
    //������ʱ�����̳������µ���
    MessageOption option(MU_VM_SCHEDULER,EV_SCHEDULE_TIMER,0,0);
    MessageFrame frame("",option);
    return Receive(frame);
}

int64 Scheduler::ApplyHostPolicy(const SchedulingVM &schdl_vm,HostPolicyType policy)
{
    if(schdl_vm._hids.empty())
        return INVALID_OID;

    if(schdl_vm._hids.size() == 1)
        return schdl_vm._hids[0];

    switch(policy)
    {
        case SIMPLE_POLICY:
        {
            SimplePolicy sp;
            return sp.Select(schdl_vm);
            break;
        }

        case PACKING:
        {
            PackingPolicy sp;
            return sp.Select(schdl_vm);
            break;
        }

        case STRIPING:
        {
            StripingPolicy sp;
            return sp.Select(schdl_vm);
            break;
        }

        default:
            printf("unknown cluster policy!\n");
            break;
    }

    return INVALID_OID;
}

/* image id !=-1ʱ������hc�ϻ�����2���ļ���base�ļ���snapshot������Ҫ��2 */
/* isMax��ʾ�Ƿ�Ҫ����base�Ĵ�С */
int64 Scheduler::CalcLocalDiskSize(const VMConfig &vmcfg, int isMax)
{
#define LocalDiskSize(_disk) (VM_STORAGE_TO_LOCAL == ((_disk)._position) ? CalcDiskRealSize(_disk) : 0)
#define DiskMaxNeed(_disk) (INVALID_FILEID == ((_disk)._id) ? (LocalDiskSize(_disk)) : (LocalDiskSize(_disk))* (1 + (isMax) * IsUseSnapshot()))

    int64 total_size = 0;

    if (vmcfg._machine._id != INVALID_FILEID)
    {
        total_size += DiskMaxNeed(vmcfg._machine);
        if (0 != vmcfg._machine._reserved_backup)
        {//����Ԥ�����ݿռ�
            total_size += vmcfg._machine._disk_size * vmcfg._machine._reserved_backup * (100 + GetSpaceRatio()) / 100;
        }
    }
    else
    {
        total_size += DiskMaxNeed(vmcfg._kernel);
        total_size += DiskMaxNeed(vmcfg._initrd);
    }

    vector<VmDiskConfig>::const_iterator it_image;
    for (it_image = vmcfg._disks.begin(); it_image != vmcfg._disks.end(); it_image ++)
    {
        total_size += DiskMaxNeed(*it_image);
        if ((INVALID_FILEID != it_image->_id) && (0 != it_image->_reserved_backup))
        {//����Ԥ�����ݿռ�
            total_size += it_image->_disk_size * it_image->_reserved_backup * (100 + GetSpaceRatio()) / 100;
        }
    }

    return total_size;
}


/* ������������õ�image�Ƿ���hc�����У������ȼ�
Ҳ���ǰѸ�hc��������ͬid�ģ��ŵ���һ��λ�� */
void Scheduler::SortbyImage(VMConfig const &vmcfg, vector<int64> &hosts)
{
    if ( (0 == hosts.size()) || (INVALID_FILEID == vmcfg._machine._id))
    {
        return;
    }

    VMImagePool *vmimage_pool = VMImagePool::GetInstance();
    SkyAssert(vmimage_pool);
    vector<Host_using_Image> vec;
    int ret = vmimage_pool->GetHostUsingImage(vec);
    SkyAssert((ret == 0) || (ret == SQLDB_RESULT_EMPTY));
    if (0 == vec.size())
    {
        return;
    }

    vector<int64>::iterator it_host;
    Host_using_Image req_image={-1, vmcfg._machine._id};
    for (it_host = hosts.begin(); it_host != hosts.end(); it_host ++)
    {
        req_image._hid = *it_host;

        for (vector<Host_using_Image>::iterator it_image = vec.begin(); it_image != vec.end(); it_image ++)
        {
            if (req_image == (*it_image))
            {
                swap(*it_host, hosts[0]);
                return;
            }
        }
#if 0
        if (find(vec.begin(), vec.end(), req_image) != vec.end());
        {
            swap(*it_host, hosts[0]);
            break;
        }
#endif
    }
}

int Scheduler::HandleChngCfgReq(const MessageFrame& message)
{
    VMDeployInfo deploy;
    SkyAssert(deploy.deserialize(message.data));

    MID receiver;
    SkyAssert(receiver.deserialize(deploy._stamp));

    VirtualMachine *pvm = vm_pool->Get(deploy._config._vid, false);
    if (pvm->_config._config_version == deploy._config._config_version)
    {
        VmOperationAck ack(deploy.action_id, SUCCESS, 100, "SUCCESS", deploy._config._vid, SYNC);
        return VMManager::SendVmOperationAck(m, ack, receiver);
    }

    if (!IsChngCfgOK(deploy._config))
    {
        VmOperationAck ack(deploy.action_id, ERROR, 0, "insufficient resources ", deploy._config._vid, SYNC);
        return VMManager::SendVmOperationAck(m, ack, receiver);
    }

/* ��2���ӿ��Ƿ���Ҫ�ٴε��ã���ȷ�� */
#if 0
    VirtualMachine *pvm = vm_pool->Get(newcfg._vid,false);
    STATUS ret;
    ret = AllocateNetworkResource(pvm,error);
    if(SUCCESS != ret)
    {
        return ret;
    }

    ret=AllocateStorageResource(pvm,error);
    if(SUCCESS != ret)
    {
        return ret;
    }
#endif

    SaveChngedCfg(deploy._config);

    VmOperationAck ack(deploy.action_id, SUCCESS, 100, "SUCCESS", deploy._config._vid, SYNC);
    return VMManager::SendVmOperationAck(m, ack, receiver);
}

bool Scheduler::IsChngCfgOK(const VMConfig &newcfg)
{
    Debug(SYS_DEBUG,"Checking new configuration for vm %lld ...\n",newcfg._vid);

    VirtualMachine *pvm = vm_pool->Get(newcfg._vid,false);
    SkyAssert(pvm);

    if (pvm->_config._machine._position != newcfg._machine._position)
    {//�����λ�����Բ������޸�
        return false;
    }
    else if((newcfg._machine._position == IMAGE_POSITION_SHARE) 
        && (pvm->_config._machine._id != newcfg._machine._id))
    {//ʹ�ù����̵�ʱ�򣬲������������
        return false;
    }
    
    ostringstream where;
    where << " cpus >= " << newcfg._vcpu;  //���ע�⣬����Ҫ���
    where << " AND tcu_num >= " << newcfg._tcu;     //���ע�⣬����Ҫ���
    where << " AND all_free_tcu >= " << int32(newcfg._tcu*newcfg._vcpu) - int32(pvm->_config._vcpu*pvm->_config._tcu);
    where << " AND mem_free >= " << newcfg._memory - pvm->_config._memory;
    where << " AND disk_free >= " << CalcLocalDiskSize(newcfg) - CalcLocalDiskSize(pvm->_config);
    where << " AND oid = " << pvm->_hid;

    HostPool *host_pool = HostPool::GetInstance();
    Debug(SYS_DEBUG,"WHERE to check if host fit for new vm-cfg %lld: \n%s\n",newcfg._vid,where.str().c_str());

    vector<int64> hosts_checking;
    host_pool->Match(hosts_checking, where.str());

    return !hosts_checking.empty();
}

int Scheduler::SaveChngedCfg(const VMConfig &newcfg)
{
    VirtualMachine *pvm = vm_pool->Get(newcfg._vid,true);
    SkyAssert(pvm);

    /* VirtualMachine._config����Щ�ֶ�����CC���ӹ��ģ������������õĶ˿�,vnc_port�ȣ����Բ���ֱ�Ӹ��� */
    pvm->_config._name = newcfg._name;
    pvm->_config._memory = newcfg._memory;
    //pvm->_config._cpu_percent = newcfg._cpu_percent;
    pvm->_config._vcpu = newcfg._vcpu;
    pvm->_config._tcu = newcfg._tcu;
    pvm->_config._kernel = newcfg._kernel;
    pvm->_config._initrd = newcfg._initrd;
    if (newcfg._machine._position == IMAGE_POSITION_LOCAL)
    {
        pvm->_config._machine = newcfg._machine;
    }
    //pvm->_config._disks = newcfg._disks;    ---del by zhangh  611003579341��TECS������洢��������йػ��޸ľ����ļ��󣬿���ʧ��
    //pvm->_config._nics = newcfg._nics;
    pvm->_config._config_version = newcfg._config_version;

    CalcVmCpuPercent(*pvm);
    
    TransactionOp tsop; //�����������

    vm_pool->Update(pvm);

    VMImagePool *vmimage_pool = VMImagePool::GetInstance();
    SkyAssert(vmimage_pool);
    int ret = vmimage_pool->UpdateVMImageConfig(pvm->_vid,pvm->_config);
    SkyAssert(ret == 0);

    vm_pool->Put(pvm,true);

    if (SUCCESS != tsop.Commit( ))
    {
        SkyAssert(false);
        return ERROR_DB_UNKNOWN_FAIL;
    }

    return 0;
}

int Scheduler::CalcVmCpuPercent(VirtualMachine &vm)
{
    HostFree host_free;
    HostPool *host_pool = HostPool::GetInstance();
    host_pool->GetHostFree(vm._hid, host_free);

    vm._config._cpu_percent = (vm._tcu*100)/(host_free._tcu_num);

    /* ���TCU�ٷֱ�С��1%����ô��ȡ1% */
    if (vm._config._cpu_percent == 0)
    {
        vm._config._cpu_percent = 1;
    }
    
    return 0;
}

#define ModifyNicAction(id, ac_name, msg_type) \
    do \
    { \
        vector<string> actions; \
        STATUS ret = GetActionsByName(id, ac_name, actions); \
        SkyAssert(ret == SUCCESS); \
        if (!actions.empty()) \
        { \
            SkyAssert(actions.size() == 1); \
            VmAction action; \
            ret = GetActionById(actions[0], action); \
            SkyAssert(ret == SUCCESS); \
            msg_type req; \
            bool result = req.deserialize(action.message_req); \
            SkyAssert(result);\
            req.m_hc_application = host_name; \
            ret = UpdateActionReq(actions[0], req); \
            SkyAssert(ret == SUCCESS); \
            action._hid = pvm->_hid; \
            ret = SetActionLabels(actions[0], action.labels); \
            SkyAssert(ret == SUCCESS); \
        } \
    }while(0)
    
#define ModifyDiskActions(id, ac_name, msg_type) \
    do \
    { \
        vector<string> actions; \
        vector<string>::iterator it; \
        STATUS ret = GetActionsByName(id, ac_name, actions); \
        SkyAssert(ret == SUCCESS); \
         \
        for (it = actions.begin(); it != actions.end(); it++) \
        { \
            VmAction action; \
            ret = GetActionById(*it, action); \
            msg_type req; \
            bool result = req.deserialize(action.message_req); \
            SkyAssert(result);\
            req._userip = host_ip; \
            req._iscsiname = iscsiname; \
            ret = UpdateActionReq(action.get_id(), req); \
            SkyAssert(ret == SUCCESS); \
            action._hid = pvm->_hid; \
            ret = SetActionLabels(action.get_id(), action.labels); \
            SkyAssert(ret == SUCCESS); \
        } \
    }while(0)
    
STATUS Scheduler::ModifyDeployWorkflow(const string action_id)
{
    VmWorkFlow wf_deploy;
    STATUS ret = GetWorkflowLabels(action_id, wf_deploy.labels);
    SkyAssert(ret == SUCCESS);
    
    VirtualMachine *pvm = vm_pool->Get(wf_deploy._vid,false);
    SkyAssert(pvm);

    const string host_ip = host_pool->GetHostIpById(pvm->_hid);
    const string host_name = host_pool->GetHostNameById(pvm->_hid);
    const string iscsiname = host_pool->GetIscsinameById(pvm->_hid);
    
    TransactionOp tsop; //�����������

    //�ع�������
    string rollback_id;
    ret = GetRollbackAction(action_id, rollback_id);
    SkyAssert(ret == SUCCESS);

    ModifyNicAction(action_id, VM_ACT_PREPARE_NIC, CVNetGetResourceHCMsg);
    ModifyNicAction(rollback_id, VM_ACT_RELEASE_NIC, CVNetFreeResourceHCMsg);

    //ModifyDiskActions(action_id, VM_ACT_PREPARE_IMAGE, ImageUrlPrepareReq);
    ModifyDiskActions(action_id, VM_ACT_AUTH_BLOCK, AuthBlockReq);
    //ModifyDiskActions(rollback_id, VM_ACT_RELEASE_IMAGE, ImageUrlReleaseReq);
    ModifyDiskActions(rollback_id, VM_ACT_DEAUTH_BLOCK, DeAuthBlockReq);

    ret = tsop.Commit();
    SkyAssert(ret == SUCCESS);
    return ret;
}

#define ModifyNicMiarateAction(id, ac_name, msg_type) \
    do \
    { \
        vector<string> actions; \
        STATUS ret = GetActionsByName(id, ac_name, actions); \
        SkyAssert(ret == SUCCESS); \
        if (!actions.empty()) \
        { \
            SkyAssert(actions.size() == 1); \
            VmAction action; \
            ret = GetActionById(actions[0], action); \
            SkyAssert(ret == SUCCESS); \
            msg_type req; \
            bool result = req.deserialize(action.message_req); \
            SkyAssert(result);\
            req.m_dst_hc_application = host_name; \
            ret = UpdateActionReq(actions[0], req); \
            SkyAssert(ret == SUCCESS); \
        } \
    }while(0)

STATUS Scheduler::ModifyMigrateWorkflow(const string action_id)
{
    VmWorkFlow wf_migrate;
    STATUS ret = GetWorkflowLabels(action_id, wf_migrate.labels);
    SkyAssert(ret == SUCCESS);
    
    VirtualMachine *pvm = vm_pool->Get(wf_migrate._vid,false);
    SkyAssert(pvm);

    const string host_ip = host_pool->GetHostIpById(pvm->_hid_next);
    const string host_name = host_pool->GetHostNameById(pvm->_hid_next);
    const string iscsiname = host_pool->GetIscsinameById(pvm->_hid_next);
    
    TransactionOp tsop; //�����������

    //�ع�������
    string rollback_id;
    ret = GetRollbackAction(action_id, rollback_id);
    SkyAssert(ret == SUCCESS);

    ModifyNicMiarateAction(action_id, VM_ACT_BEGIN_M_NIC, CVNetVmMigrateBeginMsg);
    ModifyNicMiarateAction(action_id, VM_ACT_END_M_NIC, CVNetVmMigrateEndMsg);
    ModifyNicMiarateAction(rollback_id, VM_ACT_END_M_NIC, CVNetVmMigrateEndMsg);

    ModifyDiskActions(action_id, VM_ACT_AUTH_BLOCK, AuthBlockReq);
    ModifyDiskActions(rollback_id, VM_ACT_DEAUTH_BLOCK, DeAuthBlockReq);

    UpdateActionMsgReq(action_id, VM_ACT_PRAPARE_M, VmMigrateData, _targe_ip, host_ip);
    UpdateActionMsgReq(action_id, VM_ACT_MIGRATE, VmMigrateData, _targe_ip, host_ip);
    UpdateActionMsgReq(action_id, VM_ACT_POST_M, VmMigrateData, _targe_ip, host_ip);

    MID hc2(host_name, PROC_HC, MU_VM_HANDLER);
    UpdateActionReceiver(action_id, VM_ACT_PRAPARE_M, hc2);
    UpdateActionReceiver(action_id, VM_ACT_POST_M, hc2);
    UpdateActionReceiver(rollback_id, VM_ACT_REMOVE, hc2);

    ret = tsop.Commit();
    SkyAssert(ret == SUCCESS);
    return ret;
}

STATUS Scheduler::ModifyColdMigrateWorkflow(const string action_id)
{
    VmWorkFlow wf_migrate;
    STATUS ret = GetWorkflowLabels(action_id, wf_migrate.labels);
    SkyAssert(ret == SUCCESS);
    
    VirtualMachine *pvm = vm_pool->Get(wf_migrate._vid,false);
    SkyAssert(pvm);

    const string host_ip = host_pool->GetHostIpById(pvm->_hid_next);
    const string host_name = host_pool->GetHostNameById(pvm->_hid_next);
    const string iscsiname = host_pool->GetIscsinameById(pvm->_hid_next);
    
    TransactionOp tsop; //�����������

    //�ع�������
    string rollback_id;
    ret = GetRollbackAction(action_id, rollback_id);
    SkyAssert(ret == SUCCESS);

    ModifyNicAction(action_id, VM_ACT_PREPARE_NIC, CVNetGetResourceHCMsg);
    ModifyNicAction(rollback_id, VM_ACT_RELEASE_NIC, CVNetFreeResourceHCMsg);

    ModifyDiskActions(action_id, VM_ACT_AUTH_BLOCK, AuthBlockReq);
    ModifyDiskActions(rollback_id, VM_ACT_DEAUTH_BLOCK, DeAuthBlockReq);

    MID hc2(host_name, PROC_HC, MU_VM_HANDLER);
    UpdateActionReceiver(action_id, VM_ACT_PRAPARE_M, hc2);
    UpdateActionReceiver(rollback_id, VM_ACT_REMOVE, hc2);

    ret = tsop.Commit();
    SkyAssert(ret == SUCCESS);
    return ret;
}


STATUS EnableSchedulerOptions() 
{
    TecsConfig *config = TecsConfig::Get();
    // ������ģ�����ò������Ժ��������
    config->EnableCustom("max_try_hosts", max_try_hosts,
                         "The max count of hosts scheduler will try for a virtual machine, default is 1.");
    return SUCCESS;
}

int GetMaxScheduleHosts()
{
    return max_try_hosts;
}

//�����µĵ��������
STATUS Scheduler::MigrateVM(const MessageFrame& message, ostringstream& oss)
{
    VmMigrateReq req;
    STATUS ret = req.deserialize(message.data);
    SkyAssert(true == ret);
    
    if (VmAssistant->CheckReqDoing(req.action_id))
    {
        return 0;
    }

    ret = MigrateVM(req, oss);

    if (ret == SUCCESS)
    {
        ModifyMigrateWorkflow(req.action_id);
    }

    int progress = (ret == SUCCESS) ? 100 : 0;
    VmOperationAck ack(req.action_id, ret, progress, oss.str(), req._vid, req._operation);
    //UpdateActionAck(req.action_id, ack.serialize());
    MessageOption option(message.option.sender(),EV_VM_OP_ACK,0,0);
    return m->Send(ack, option);        
}

/**********************************************************************
* �������ƣ�VMManager::MigrateVM
* �������������������Ǩ������
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����     �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2012/03/05   V1.0    ҦԶ        ����
************************************************************************/
STATUS Scheduler::MigrateVM( const VmMigrateReq &req, ostringstream& error )
{
    //�����������ʱ�Ĵ���
    VirtualMachine *pvm = vm_pool->Get(req._vid,false);
    SkyAssert(pvm);
    SkyAssert(pvm->_hid != INVALID_HID);

    if (!pvm->_appoint_host.empty())
    {
        error << "VM " << req._vid <<" is apppint to " << pvm->_appoint_host << ", migrate is forbidden!" << endl;
        Debug(SYS_WARNING,"%s\n",error.str().c_str());
        return ERROR;
    }

    //�����������ʱ�Ĵ���
    string host_name;
    if ( req._hid != INVALID_HID )
    {/* ���ָ����host������HOST ID�Ƿ���ȷ */
        host_name = host_pool->GetHostNameById(req._hid);
        if (host_name.empty())
        {
            //����������Ѿ����ڵ�host�����򷵻�ʧ��
            error << "Schedule find no host(id=" <<req._hid<< ") in this cluster pool!";
            Debug(SYS_WARNING,"%s\n",error.str().c_str());
            return ERROR_OBJECT_NOT_EXIST;
        }
    }

    if (req._hid == pvm->_hid)
    {
        error << "This vm(id = "<< req._vid << ") is already in the host(id = " << req._hid << ").";
        Debug(SYS_WARNING,"%s\n",error.str().c_str());
        return ERROR_INVALID_ARGUMENT;
    }

    VMConfig        vmcfg       = pvm->_config;
    VmRequirement   vmreq       = pvm->_requirement;

    HostPool *host_pool = HostPool::GetInstance();
    Host    *runningHost = host_pool->Get( pvm->_hid, false );
    SkyAssert( runningHost != NULL );

    /* ����ѡ����HC�����벻�ǵ�ǰ���е�HC */
    ostringstream hid("");
    hid << runningHost->GetOID();
    Expression hc_id_exp("oid","!=",hid.str());
    vmreq._qualifications.push_back( hc_id_exp );

    vmreq._qualifications.insert(vmreq._qualifications.end(), req._qualifications.begin(), req._qualifications.end());

    if (req._operation == LIVE_MIGRATE)
    {
        //��Ǩ�ƣ����붼��XEN����KVM
        Expression vmm_info_exp("vmm_info","like","'" + runningHost->get_hyper_info() + "%'");
        vmreq._qualifications.push_back( vmm_info_exp );
    }
    
    if ( !req._bForced )
    {/* ��û��ǿ��ָ�����������ѡ��HCһЩ�������� */
        /* CPU���������vm����hcһ�� */
        ostringstream cpu_info("");
        cpu_info << runningHost->get_cpu_info_id();
        Expression cpu_info_exp( "cpu_info_id","=", cpu_info.str() );
        vmreq._qualifications.push_back( cpu_info_exp );

        /* OS���������vm����hcһ�� */
        Expression os_info_exp("os_info","=", "'" + runningHost->get_os_info() + "'");
        vmreq._qualifications.push_back( os_info_exp );

        /* hypervisor���������vm����hcһ�� */
        Expression vmm_info_exp("vmm_info","=","'" + runningHost->get_vmm_info() + "'");
        vmreq._qualifications.push_back( vmm_info_exp );
    }

    int64           targetHC    = req._hid;
    VMDeployInfo    deploy_for_migrate;
    deploy_for_migrate.action_id = req.action_id;
    deploy_for_migrate._config = vmcfg;
    deploy_for_migrate._requirement = vmreq;
    STATUS ret = SelectHost( targetHC,deploy_for_migrate );
    if ( ret != SUCCESS )
    {
        error << "Schedule determ there is not hosts fit the vm except the running host.";
        Debug(SYS_WARNING,"%s\n",error.str().c_str());
        return ret;
    }

    //��ѡ����д��vm_pool
    VmAssistant->SetValue(req._vid, "hid_next", targetHC);

    return ret;
}

//�����µĵ��������
STATUS Scheduler::ColdMigrateVM(const MessageFrame& message, ostringstream& oss)
{
    VmMigrateReq req;
    STATUS ret = req.deserialize(message.data);
    SkyAssert( true == ret );
    
    if (VmAssistant->CheckReqDoing(req.action_id))
    {
        return 0;
    }

    req._bForced = true;
    ret = MigrateVM(req, oss);

    if (ret == SUCCESS)
    {
        ModifyColdMigrateWorkflow(req.action_id);
    }
    
    int progress = (ret == SUCCESS) ? 100 : 0;
    VmOperationAck ack(req.action_id, ret, progress, oss.str(), req._vid, req._operation);
    //UpdateActionAck(req.action_id, ack.serialize());
    MessageOption option(message.option.sender(),EV_VM_OP_ACK,0,0);
    return m->Send(ack, option);
	
}

void Scheduler::SetRunningReSCH()
{
    ConfigCluster *p = ConfigCluster::GetInstance();
    SkyAssert(p);

    int cycle = atoi(p->Get(CFG_RUN_SCH_CYCLE).c_str());
    if (cycle == 0)
    {//ȱʡ��30������Ӫ����ִ��һ��
        cycle = 30 * 60;
    }
    cycle *= 1000;  //ת��Ϊ����

    VmAssistant->SetTimer(_running_sch_timer, cycle, EV_RUNNING_SCH_TIMER);
}

//��Ӫ����
void Scheduler::RunningReSCH()
{
    ConfigCluster *p = ConfigCluster::GetInstance();
    SkyAssert(p);

    const string policy = p->Get(CFG_SCH_POLICY);
    if (policy.empty())
    {
        return;
    }
    
    //ȡ��ÿ�ε��ȿ�Ǩ�Ƶ�����������
    int max_vms = atoi(p->Get(CFG_RUN_SCH_MAX_VMS).c_str());
    if (max_vms <= 0)
    {
        max_vms = 0xFFFF;
    }

    vector<int64> vms;
    vm_pool->GetAllVms(vms, 1000);

    RunningReSCH(vms, policy, max_vms);
}

//��Ӫ���ȣ���һ�������������
//��Ӫ���ȵ�ʱ��һ��Ǩ�Ƶ���������������ܳ������ֵ����Ǩ��ʧ�ܣ�����Ǩ������������Ǩ��һ��
//���ԣ������һ��һ��Ǩ�ƣ��ڹ������б���Ǩ�Ƴɹ����������������Ǩ�Ƴɹ�����������1��ʧ�ܲ��㡣
//�绹û�г������ֵ���������һ��Ǩ��
void Scheduler::RunningReSCH(const vector<int64> &vids, const string &sch_policy, int migrate_max)
{
    if (vids.empty())
    {
        return;
    }

    string wf_now = FindWorkflow(VM_WF_RESCH_CATEGORY, VM_WF_SCH_RUN_RESCH, "1 = 1");
    if (!wf_now.empty())
    {//��ǰ���й�����
        return;
    }

    Workflow wf_resch;
    wf_resch.category = VM_WF_RESCH_CATEGORY;
    wf_resch.name = VM_WF_SCH_RUN_RESCH;

    vector<Action> migrate_vms;
    vector<int64>::const_iterator it;
    for (it = vids.begin(); it != vids.end(); it ++)
    {
        VirtualMachine *pvm = vm_pool->Get(*it, false);
        SkyAssert(pvm);
        VmOperation       action_type;
        if (pvm->_state == VM_RUNNING)
        {
            action_type = LIVE_MIGRATE;
        }
        else if (pvm->_state == VM_SHUTOFF)
        {
            action_type = COLD_MIGRATE;
        }
        else
        {
            Debug(SYS_NOTICE,"When RunningReSCH, vm(%lld) state = %d", *it, pvm->_state);
            continue;
        }

        Expression qualification;
        qualification._key = "target_vms";

        char value[1024];
        sprintf(value, "(select target_vms from host_pool_in_use_with_cpuinfo where oid = (select hid from vm_pool where vid=%lld))", *it);
        qualification._value = value;

        if (sch_policy == SCH_POLICY_PACKING)
        {//���У�ѡ��ȵ�ǰHC�����������������
            qualification._op = ">=";
        }
        else if (sch_policy == SCH_POLICY_STRIPING)
        {//��ɢ��ѡ��ȵ�ǰHC���������2�����ϵ������
        //���磬һ��HC��3��VM,һ��HC��2��VM,�Ǿ�û��Ҫ������Ӫ������
            qualification._key = "target_vms + 1";
            qualification._op = "<";
        }
        else
        {
            return;
        }

        VmAction migrate(VM_ACT_RESCH_CATEGORY, VM_ACT_MIGRATE, EV_VM_OP, MID(MU_VM_SCHEDULER), MID(MU_VM_MANAGER));
            VmMigrateReq migrate_req(*it, INVALID_HID, false, action_type, "stamp:RunningReSCH migrate");
            migrate_req._qualifications.push_back(qualification);
            migrate_req.action_id = migrate.get_id();
        migrate.message_req = migrate_req.serialize();
        migrate.failure_feedback = FEEDBACK_RESCH_MIGRATE_FAILED;
        migrate.success_feedback = FEEDBACK_RESCH_MIGRATE_SUCCESS;

        if ((it - vids.begin()) >= migrate_max)     //����һ����Ӫ����ֻ����Ӱ��10�����������10��֮���actin��skip
        {
            migrate.receiver = MID(""); //û��skip�����Ը��ս��̷���Ϣ
        }
        
        //wf_resch.Add(migrate);
        migrate_vms.push_back(migrate);
    }

    if (!migrate_vms.empty())
    {
        wf_resch.Add(migrate_vms);
        STATUS ret = CreateWorkflow(wf_resch);
        if(SUCCESS != ret)
        {
            Debug(SYS_WARNING, "RunningReSCH failed to created workflow: %s!!!\n", wf_resch.get_id().c_str());
            SkyAssert(false);
        }
    }
}

int Scheduler::ResponseFeedback(const int func, const WorkAck &ack)
{
    switch (func)
    {
        case FEEDBACK_RESCH_MIGRATE_FAILED:
        {
            return ReSchMigrateFailed(ack);
        }

        case FEEDBACK_RESCH_MIGRATE_SUCCESS:
        {
            return ReSchMigrateSuccess(ack);
        }

        default:
        {
            Debug(SYS_NOTICE, "In Scheduler::ResponseFeed, unknown func code = %d.", func);
            return 0;            
        }
    }
}

//һ��Ǩ��ʧ���ˣ��ǾͰѺ����action ����
int Scheduler::ReSchMigrateFailed(const WorkAck &ack)
{
    string wf_now;
    GetWorkflowByActionId(ack.action_id, wf_now);
    
    string paused_action;
    vm_pool->SelectColumn("action_pool", "action_id", "receiver = '..' and workflow_id = '" + wf_now + "'", paused_action);

    if (!paused_action.empty())
    {
        STATUS ret = UpdateActionReceiver(paused_action, MID(MU_VM_MANAGER));
        SkyAssert(ret == SUCCESS);
    }

    return FinishAction(ack.action_id);
}

//����ɹ���������������������Խ�����������
int Scheduler::ReSchMigrateSuccess(const WorkAck &ack)
{
    ConfigCluster *p = ConfigCluster::GetInstance();
    SkyAssert(p);

    //ȡ��ÿ�ε��ȿ�Ǩ�Ƶ�����������
    int max_vms = atoi(p->Get(CFG_RUN_SCH_MAX_VMS).c_str());
    if (max_vms <= 0)
    {//û�д������ƣ��ǾͰ����е�action��ִ����
        return FinishAction(ack.action_id);
    }
    
    //select count(*) from action_pool where state=0 
    int count = 0;
    vm_pool->SelectColumn("action_pool", "COUNT(*)", "state=0",count);

    if (count >= max_vms)
    {//�����п�����skip������������������
        string wf_now;
        GetWorkflowByActionId(ack.action_id, wf_now);
        
        vector<string> paused_actions;
        vector<string>::iterator it;
        vm_pool->SelectColumn("action_pool", "action_id", "receiver = '..' and workflow_id = '" + wf_now + "'", paused_actions);
        for (it = paused_actions.begin(); it != paused_actions.end(); it ++)
        {
            SkipAction(*it);
        }
    }

    return FinishAction(ack.action_id);
}

STATUS Scheduler::ReSchSingle(const MessageFrame& message, ostringstream& oss)
{
    VmOperationReq req;
    if(false == req.deserialize(message.data))
    {
        SkyAssert(false);
        return ERROR;
    }

    if (VmAssistant->CheckReqDoing(req))
    {
        return 0;
    }

    STATUS ret = Schedule(req,oss);

    if (ret == SUCCESS)
    {
        ModifyUnfreezeWorkflow(req.action_id);
    }

    int progress = (ret == SUCCESS) ? 100 : 0;
    VmOperationAck ack(req.action_id, ret, progress, oss.str(), req._vid, req._operation);
    MessageOption option(message.option.sender(),EV_VM_OP_ACK,0,0);
    return m->Send(ack, option);        
}

//��������ȫ���� ModifyColdMigrateWorkflow �Ŀ��������˰�hid_next�ĳ� hid
STATUS Scheduler::ModifyUnfreezeWorkflow(const string action_id)
{
    VmWorkFlow wf_migrate;
    STATUS ret = GetWorkflowLabels(action_id, wf_migrate.labels);
    SkyAssert(ret == SUCCESS);
    
    VirtualMachine *pvm = vm_pool->Get(wf_migrate._vid,false);
    SkyAssert(pvm);

    const string host_ip = host_pool->GetHostIpById(pvm->_hid);
    const string host_name = host_pool->GetHostNameById(pvm->_hid);
    const string iscsiname = host_pool->GetIscsinameById(pvm->_hid);
    
    TransactionOp tsop; //�����������

    //�ع�������
    string rollback_id;
    ret = GetRollbackAction(action_id, rollback_id);
    SkyAssert(ret == SUCCESS);

    ModifyNicAction(action_id, VM_ACT_PREPARE_NIC, CVNetGetResourceHCMsg);
    ModifyNicAction(rollback_id, VM_ACT_RELEASE_NIC, CVNetFreeResourceHCMsg);

    ModifyDiskActions(action_id, VM_ACT_AUTH_BLOCK, AuthBlockReq);
    ModifyDiskActions(rollback_id, VM_ACT_DEAUTH_BLOCK, DeAuthBlockReq);

    MID hc2(host_name, PROC_HC, MU_VM_HANDLER);
    UpdateActionReceiver(action_id, VM_ACT_PRAPARE_M, hc2);
    UpdateActionReceiver(rollback_id, VM_ACT_REMOVE, hc2);

    ret = tsop.Commit();
    SkyAssert(ret == SUCCESS);
    return ret;
}

STATUS Scheduler::GetNewVNCPortNo(uint16 &newportno,vector<uint16> blacklist)
{    
    int ret = GetNewPortNo(VNC_PORT_MIN,VNC_PORT_MAX,blacklist,newportno);
    return ret;
}

STATUS Scheduler::GetNewSerialPortNo(uint16 &newportno,vector<uint16> blacklist)
{
	int ret = GetNewPortNo(SERIAL_PORT_MIN,SERIAL_PORT_MAX,blacklist,newportno);
    return ret;	
}

STATUS Scheduler::GetNewPortNo(const int rangemin,const int rangemax, vector<uint16> blacklist,uint16 &newportno)
{	
    int port = 0;
    ostringstream osstable;	
    ostringstream osswhere;
    ostringstream ossblacklist;

    if (rangemin == 0)
    {
        newportno = 0;
        return ERROR_INVALID_ARGUMENT;
    }
	
    int size = blacklist.size(); 		
    for ( int i = 0; i < size; i ++ )
    {
        ossblacklist << " union select " << blacklist[i];		
    }	
	
    osstable << "(select portno from port_number_used union select " << rangemin 
		     << ossblacklist.str() << " ) as temp";
    osswhere << "portno between " << rangemin << " and " << rangemax 
             << " and portno+1 not in (select portno from port_number_used " 
             << ossblacklist.str() << " ) and (portno+1 <= " << rangemax << " ) " ; 
	
    int ret = vm_pool->SelectColumn(osstable.str(),"min(portno)",osswhere.str(),port);

    if ((SQLDB_OK != ret) || (0 == port))
    {
        newportno = 0;
    }
    else
    {
        newportno = port + 1;
    }

    Debug(SYS_DEBUG, "In Scheduler::GetNewPortNo,ret=%d, mini=%d, max=%d, newportno=%d.\n",
          ret,rangemin,rangemax,newportno);	
    return ret;
}

void DbgVerifyVmRelations()
{
    Scheduler *sche = Scheduler::GetInstance();
    vector<int64> existing;
    vector<int64> attracts;
    vector<int64> repels;

    existing.push_back(1);
    existing.push_back(2);
    existing.push_back(3);

    //attracts.push_back(5);
    repels.push_back(2);
    bool ret = sche->VerifyVmRelations(existing,repels);
    cout << "VerifyVmRelations:" << ret << endl;
}
DEFINE_DEBUG_FUNC(DbgVerifyVmRelations);

void DbgSchedule()
{
    Scheduler *sche = Scheduler::GetInstance();
    sche->ScheduleNow();
}
DEFINE_DEBUG_FUNC(DbgSchedule);

void DbgGetNewPortno(uint16 mini,uint16 max,uint16 blackport)
{  
    Scheduler *sche = Scheduler::GetInstance();
    uint16 newportno = 0;
    int ret = 0;
	
    vector < uint16 > blacklist;
    blacklist.push_back( blackport );	
    ret = sche->GetNewPortNo(mini,max,blacklist,newportno);	
	
    cout << " Ret: " << ret << " mini: " << mini 
		 << " max: " << max << "  newportno: " << newportno << endl;
}
DEFINE_DEBUG_FUNC(DbgGetNewPortno);

}

