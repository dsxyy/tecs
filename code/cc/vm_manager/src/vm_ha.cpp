#include "vm_ha.h"
#include "vm_pool.h"
#include "host_pool.h"
#include "event.h"
#include "msg_host_manager_with_high_available.h"
#include "msg_power_agent_with_high_available.h"
#include "vm_interface.h"
#include "db_config.h"
#include "config_cluster.h"

static int vmha_print_on = 0;
DEFINE_DEBUG_VAR(vmha_print_on);
#define Debug(level,fmt,arg...) \
        do \
        { \
            OutPut(level,fmt,##arg);\
        }while(0)

namespace zte_tecs
{

VMHA* VMHA::instance = NULL;

STATUS VMHA::Init()
{
    vm_pool = VirtualMachinePool::GetInstance();
    if(!vm_pool)
    {
        return ERROR_NOT_READY;
    }

    host_pool = HostPool::GetInstance();
    if(!host_pool)
    {
        return ERROR_NOT_READY;
    }

    //创建、注册、启动消息单元
    STATUS ret = Start(MU_VM_HA);
    if(SUCCESS != ret)
    {
        return ret;
    }

    //给自己发一个上电通知消息
    MessageOption option(MU_VM_HA, EV_POWER_ON, 0, 0);
    MessageFrame frame(SkyInt(0),option);
    return m->Receive(frame);
}

void VMHA::MessageEntry(const MessageFrame& message)
{
    switch(m->get_state())
    {
        case S_Startup:
        {
            switch(message.option.id())
            {
                case EV_POWER_ON:
                {
                    PowerOn();
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
                case EV_HOST_OFFLINE:
                {
                    HostOfflineProcess(GetMsg<MessageHostFault>(message));
                    break;
                }

                case EV_HOST_FAULT:
                {
                    HostFaultProcess(GetMsg<MessageHostFault>(message));
                    break;
                }

                case EV_HOST_STORAGE_ERROR:
                {
                    HostStorageFaultProcess(GetMsg<MessageHostFault>(message));
                    break;
                }

                case EV_HOST_VM_FAULT:
                {
                    //HostFaultProcess(message);
                    break;
                }

                case EV_VM_DESTROY_ALL_VMS:
                {
                    DestroyAllVms(GetMsg<MessageHostResetReq>(message));
                    break;
                }

                case EV_ACTION_TIMEOUT:
                case EV_HOST_RESET_ACK:
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

void VMHA::PowerOn()
{
    STATUS ret = Transaction::Enable(GetDB());
    if(SUCCESS != ret)
    {
        Debug(SYS_ERROR,"VMHA enable transaction fail! ret = %d",ret);
        SkyExit(-1, "VMHA::PowerOn: call Transaction::Enable(GetDB()) failed.");
    }

    VmAssistant = new VmInterface(m);
    SkyAssert(VmAssistant);

    Debug(SYS_DEBUG,"vm_manager power on!\n");

    m->set_state(S_Work);
    OutPut(SYS_NOTICE, "%s power on!\n",m->name().c_str());
}

void VMHA::HostOfflineProcess(const MessageHostFault &fault)
{
    //离线物理机，其上的所有虚拟机，状态都是未知
    int64 hid = fault._host_id;

    const string policy = GetPolicy(CFG_VMHA_HOST_DOWN_POLICY);
    cout << "host down policy = " << policy << endl;
    if (policy != "migrate")
    {
        return;
    }

    vector<int64> vids;
    int ret = vm_pool->GetVmsByHostAndNext(vids, hid, 1000);
    if (ret == SQLDB_RESULT_EMPTY)
    {//hc上没有数据，那就不要执行
        return;
    }

    SkipOfflineActions(hid);

    ostringstream label_where;
    label_where << "label_int64_3 = " << hid << endl;
    string wf_now = FindWorkflow(VM_WF_CATEGORY, VM_WF_HA_MIGRATE, label_where.str());
    if (!wf_now.empty())
    {//当前已有工作流
        return;
    }

    VmWorkFlow migrate_vms;
    migrate_vms._hid = hid;
    migrate_vms.category = VM_WF_CATEGORY;
    migrate_vms.name = VM_WF_HA_MIGRATE;

    Action shutoff(VM_WF_CATEGORY, VM_ACT_DESTROY_VMS, EV_VM_DESTROY_ALL_VMS, MID(MU_VM_HA), MID(MU_VM_HA));
        MessageHostResetReq shutoff_req;
        shutoff_req._hid = hid;
        shutoff_req.action_id = shutoff.get_id();
    shutoff.message_req = shutoff_req.serialize();
    shutoff.success_feedback = 0;
    shutoff.failure_feedback = 0;
    migrate_vms.Add(shutoff);

    Action reboot_hc(VM_WF_CATEGORY, VM_ACT_REBOOT_HC, EV_HOST_RESET_REQ, MID(MU_VM_HA), MID(MU_POWER_AGENT));
        MessageHostResetReq reboot_hc_req;
        reboot_hc_req._hid = hid;
        reboot_hc_req.action_id = reboot_hc.get_id();
    reboot_hc.message_req = reboot_hc_req.serialize();
    reboot_hc.success_feedback = 0;
    reboot_hc.failure_feedback = FEEDBACK_DELETE_WORKFLOW;    //并没有定义回滚流，所以直接就结束了
    migrate_vms.Add(shutoff, reboot_hc);

    vector<Action> vec_migrate_vms;
    SkyAssert(vids.size() != 0);
    vector<int64>::iterator it;
    for (it = vids.begin(); it != vids.end(); it++)
    {
        //离线了，所有其上的虚拟机状态都是未知的了
        VmAssistant->SetValue(*it, "state", VM_UNKNOWN);
        
        Action migrate(VM_WF_CATEGORY, VM_ACT_COLD_MIGRATE, EV_VM_OP, MID(MU_VM_HA), MID(MU_VM_MANAGER));
            VmMigrateReq migrate_req(*it, INVALID_HID, true, COLD_MIGRATE, "stamp:HA migrate");
            migrate_req.action_id = migrate.get_id();
        migrate.message_req = migrate_req.serialize();
        migrate.success_feedback = 0;
        migrate.failure_feedback = 0;
        vec_migrate_vms.push_back(migrate);
    }
    migrate_vms.Add(reboot_hc, vec_migrate_vms);

    ret = CreateWorkflow(migrate_vms);
    if(SUCCESS != ret)
    {
        Debug(SYS_WARNING, "VMHA::MigrateVms failed to created workflow: %s!!!\n", migrate_vms.get_id().c_str());
        SkyAssert(false);
    }    
}

//HC在线故障，将所有的虚拟机都迁移走
//运行中的热迁移，关机的冷迁移
void VMHA::HostFaultProcess(const MessageHostFault &fault)
{
    int64 hid = fault._host_id;

    const string policy = GetPolicy(CFG_VMHA_HOST_FAULT_POLICY);
    cout << "host fault policy = " << policy << endl;
    if (policy != "migrate")
    {
        return;
    }

    MigrateVms(hid);
}

void VMHA::HostStorageFaultProcess(const MessageHostFault &fault)
{
    int64 hid = fault._host_id;

    const string policy = GetPolicy(CFG_VMHA_HOST_STORAGE_FAULT_POLICY);
    cout << "host storage fault policy = " << policy << endl;
    if (policy != "migrate")
    {
        return;
    }

    MigrateVms(hid);
}

//将指定HC上所有的虚拟机迁移走
void VMHA::MigrateVms(int64 hid)
{
    vector<int64> vids;
    vm_pool->GetVmsByHost(vids, hid, 1000);

    vector<int64>::iterator it;
    for (it = vids.begin(); it != vids.end(); it++)
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
            action_type = COLD_MIGRATE;            
        }

        VmMigrateReq migrate_req(*it, INVALID_HID, true, action_type, "stamp:HA migrate");        
        MessageOption option(MID(MU_VM_MANAGER),EV_VM_OP,0,0);
        m->Send(migrate_req, option);
    }

}

void VMHA::DestroyAllVms(const MessageHostResetReq &req)
{
    if (IsActionFinished(req.action_id))
    {
        return;
    }

    const string ip = host_pool->GetHostIpById(req._hid);
    SkyAssert(ip.size());
    
    const string killvms = "/opt/tecs/hc/scripts/killvm.sh";
    const string cmd = "ssh " + ip + " " + killvms;
    string result;
    int ret = system(cmd.c_str());
    cout << "DestroyAllVms ret = " << ret << endl; 

    int progress = (ret == SUCCESS) ? 100 : 0;
    WorkAck ack(req.action_id, ret, progress);
    UpdateActionAck(req.action_id, ack.serialize());

    if (ret == SUCCESS)
    {//杀死虚拟机成功，那就不必要重启HC了
        vector<string> actions;
        GetActionsByName(req.action_id, VM_ACT_REBOOT_HC, actions);
        SkyAssert(actions.size());
        SkipAction(actions[0]);

        //hc上的所有虚拟机，状态都应该是关机了
        vector<int64> vids;
        vm_pool->GetVmsByHost(vids, req._hid, 1000);
        vector<int64>::iterator it;
        for (it = vids.begin(); it != vids.end(); it++)
        {
            //离线了，所有其上的虚拟机状态都是未知的了
            VmAssistant->SetValue(*it, "state", VM_SHUTOFF);
        }        
    }

    FinishAction(req.action_id);
}

const string VMHA::GetPolicy(const string &fault)
{
    ConfigCluster *p = ConfigCluster::GetInstance();
    SkyAssert(p);

    return p->Get(fault);
}

void VMHA::SkipOfflineActions(int64 hid)
{
    const string hostname = host_pool->GetHostNameById(hid);

    ostringstream where;
    where << "receiver like '%" << hostname << "%' and receiver like '%" << PROC_HC 
        << "%' and receiver like '%" << MU_VM_HANDLER << "%' and category = '" << VM_ACT_CATEGORY 
        << "' and label_int64_1 != -1" << endl;
    vector<string> actions;
    vm_pool->SelectColumn("view_actions", "action_id", where.str(), actions);

    vector<string>::iterator it;
    for (it = actions.begin(); it != actions.end(); it ++)
    {
        SkipAction(*it);
    }
}

void DbgSetVMHA(const char* chFault, const char* chPolicy)
{
    VMHA *p = VMHA::GetInstance();
    if (!p)
    {
        cout << "VMHA::GetInstance failed!" << endl;
        return;
    }

    const string fault(chFault);
    const string policy(chPolicy);
    
    if (fault == "host_down")
    {
        //p->policy.host_down = policy;
    }
    else if (fault == "host_fault")
    {
        //p->policy.host_fault = policy;
    }
    else if (fault == "host_storage_fault")
    {
        //p->policy.host_storage_fault = policy;
    }    
}
DEFINE_DEBUG_FUNC(DbgSetVMHA);

enum VmHAFault
{
    VMHA_FAULT_UNKNOWN = 0,
    VMHA_HOST_DOWN,
    VMHA_HOST_FAULT,
    VMHA_HOST_STORAGE_FAULT,
};

enum VMHAPolicy
{
    VMHA_NONE = 0,
    VMHA_MIGRATE,
};

void SetVMHA(int fault, int policy)
{
    map <int, string> faults;
    faults[VMHA_HOST_DOWN]          = "host_down";
    faults[VMHA_HOST_FAULT]         = "host_fault";
    faults[VMHA_HOST_STORAGE_FAULT] = "host_storage_fault";

    map <int, string> policies;
    policies[VMHA_NONE] = "none";
    policies[VMHA_MIGRATE] = "migrate";
    
    if (faults.count(fault) == 0)
    {
        cout << "Input error for fault type!" << endl;
        return;
    }
    
    if (policies.count(policy) == 0)
    {
        cout << "Input error for fault policy!" << endl;
        return;
    }    
    
    DbgSetVMHA(faults[fault].c_str(), policies[policy].c_str());
}
DEFINE_DEBUG_FUNC_ALIAS(vha, SetVMHA);

void DbgSimulateFault(const char* chFault, int64 hid)
{
    VMHA *p = VMHA::GetInstance();
    if (!p)
    {
        cout << "VMHA::GetInstance failed!" << endl;
        return;
    }

    MessageHostFault msg;
    msg._host_id = hid;

    const string fault(chFault);
    int msg_id = 0;
    if (fault == "host_down")
    {
        msg_id = EV_HOST_OFFLINE;
    }
    else if (fault == "host_fault")
    {
        msg_id = EV_HOST_FAULT;
    }
    else if (fault == "host_storage_fault")
    {
        msg_id = EV_HOST_STORAGE_ERROR;
    }

    MID receiver(MU_VM_HA);
    MessageOption option(receiver, msg_id, 0, 0);

    p->m->Send(msg, option);
}
DEFINE_DEBUG_FUNC(DbgSimulateFault);

}

