/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：lifecycle_manager.cpp
* 文件标识：见配置管理计划书
* 内容摘要：虚拟机生命周期管理模块实现文件
* 当前版本：1.0
* 作    者：张文剑
* 完成日期：2011年8月16日
*
* 修改记录1：
*     修改日期：2011/07/01
*     版 本 号：V1.0
*     修 改 人：张文剑
*     修改内容：创建
*******************************************************************************/
#include "scheduler.h"
#include "vm_manager.h"
#include "lifecycle_manager.h"
#include "host_pool.h"
#include "tecs_config.h"
#include "log_agent.h"
#include "volume.h"
#include "vm_image.h"
#include "vstorage_manager.h"
#include "vnetlib_event.h"
#include "config_cluster.h"
#include "alarm_agent.h"

static int lcm_print_on = 0;
DEFINE_DEBUG_VAR(lcm_print_on);
#define Debug(level,fmt,arg...) \
        do \
        { \
            if(lcm_print_on) \
            { \
                printf("[%s:%d]"fmt,__func__,__LINE__, ##arg); \
            } \
            OutPut(level,fmt,##arg);\
        }while(0)

namespace zte_tecs
{
#define EV_VM_OP_TIMEOUT EV_TIMER_1
#define EV_CANCEL_TIMER  EV_TIMER_2
#define TIMER_VM_STATISTICS_SCAN EV_TIMER_3
#define CANCEL_FAILED_LIMIT 10
#define TIMER_CHECK_VM_ALARM EV_TIMER_4

extern int32 statistics_reserve;

static bool enable_tcu_share = false;

STATUS DeleteVmMachine(MessageUnit *mu, int64 vid, const string &action_id);
STATUS InsertVmMachine(MessageUnit *mu, int64 vid, const string &action_id);

STATUS EnableLifeCycleOptions()
{
    TecsConfig *config = TecsConfig::Get();
    config->EnableCustom("enable_tcu_share", enable_tcu_share,
                         "Vm tcu share flag, yes is share tcu with vm each other, default is no(bool switch).");
    return SUCCESS;
}

LifecycleManager* LifecycleManager::instance = NULL;

STATUS LifecycleManager::Init()
{
    vm_pool = VirtualMachinePool::GetInstance();
    host_pool = HostPool::GetInstance();
    if(!vm_pool || !host_pool)
    {
        return ERROR_NOT_READY;
    }

    InitConfig(ConfigVmStatistics, _vm_statistics);
    InitConfig(ConfigVmStatisticsNics, _vm_statistics_nics);

    //创建、注册、启动消息单元
    STATUS ret = Start(MU_VM_LIFECYCLE_MANAGER);
    if(SUCCESS != ret)
    {
        return ret;
    }

    //给自己发一个上电通知消息
    MessageOption option(MU_VM_LIFECYCLE_MANAGER,EV_POWER_ON,0,0);
    MessageFrame frame(SkyInt(0),option);
    return m->Receive(frame);
}

static bool check_vm = true;
#define GetActionInfo(action, style) \
    Work##style style; \
    STATUS ret = style.deserialize(message.data); \
    SkyAssert(ret == true); \
    if (style.action_id.empty()) \
    { \
        break; \
    } \
     \
    VmWfLock wf_lock(style.action_id); \
    if (!wf_lock.IsLocked()) \
    { \
        break; \
    } \
    \
    VmAction action; \
    if (GetActionById(style.action_id, action) != SUCCESS) \
    { \
        Debug(SYS_ERROR,"lifecycle_manager GetActionById fail! Action ID is %s.", style.action_id.c_str());\
        SkyAssert(false); \
        break; \
    } \
    \
    if (check_vm &&(vm_pool->IsVmExist(action._vid) != SQLDB_OK)) \
    { \
        Debug(SYS_NOTICE,"lifecycle_manager check vm(%d) is not exist.", action._vid); \
        SkyAssert(false); \
        break; \
    }

void LifecycleManager::MessageEntry(const MessageFrame& message)
{
    ostringstream oss;

    switch (m->get_state())
    {
    case S_Startup:
    {
        switch (message.option.id())
        {
        case EV_POWER_ON:
        {
            PowerOn( );
            break;
        }

        default:
            break;
        }
        break;
    }

    case S_Work:
    {
        switch (message.option.id())
        {
            //虚拟机操作请求
        case EV_VM_OP:
        case EV_VM_MIGRATE:
        {
            SkyAssert(false);
            //HandleOpReq(message);
            break;
        }

        //虚拟机监控状态上报
        case EV_VM_INFO_REPORT:
        {
            HandleMoniResult(message);
            break;
        }

        case EV_VMAGT_CFG_REQ:
        {
            HandleCfgReq(message);
            break;
        }

        case EV_VM_UNSELECT_HOST:
        {
            check_vm = false;   //执行这个动作的时候，vm_pool里有可能是没有记录的
            GetActionInfo(action, Req);
            FinishVMCancel(action, Req);
            check_vm = true;
            break;
        }

        case EV_VM_UNSELECT_HOST_NEXT:
        {
            GetActionInfo(action, Req);
            MigrateFailed(action, Req);
            break;
        }

        case EV_VM_MODIFY_CREATE_VM:
        {
            GetActionInfo(action, Req);
            ModifyCreateVMAction(action, Req);
            break;
        }

        case EV_VM_FINISH_FREEZE:
        {
            GetActionInfo(action, Req);
            FinishVMFreeze(action, Req);
            break;
        }

        case EV_VM_OP_ACK:
        case EV_ACTION_TIMEOUT:
        case EV_VM_IMAGE_URL_PREPARE_ACK:
        case EV_VM_IMAGE_URL_RELEASE_ACK:
        case EV_STORAGE_CREATE_BLOCK_ACK:
        case EV_STORAGE_CREATE_SNAPSHOT_ACK:
        case EV_STORAGE_AUTH_BLOCK_ACK:
        case EV_STORAGE_DEAUTH_BLOCK_ACK:
        case EV_STORAGE_DELETE_BLOCK_REQ:
        case EV_IMAGE_URL_PREPARE_ACK:
        case EV_VNETLIB_SELECT_HC_ACK:
        case EV_VNETLIB_GETRESOURCE_HC_ACK:
        case EV_VNETLIB_FREERESOURCE_HC_ACK:
        case EV_VNETLIB_VM_MIGRATE_BEGIN_ACK:
        case EV_VNETLIB_VM_MIGRATE_END_ACK:
        case EV_STORAGE_DELETE_SNAPSHOT_ACK:
        {
            GetActionInfo(action, Ack);
            ResponseFeedback(action, Ack);
            break;
        }

        case EV_VM_MODIFY_NEXT_ACTION:
        {
            GetActionInfo(action, Req);
            ModifyNextAction(action, Req);
            break;
        }

        case TIMER_VM_STATISTICS_SCAN:
        {
            DoScanForDrop();
            break;
        }

        case EV_PROJECT_DYNAMIC_DATA_REQ:
        {//资产统计动态信息
            DoProjectDynamicDataReq(message);
            break;
        }

        case EV_UNBIND_VM_FROM_HOST:
        {
            GetActionInfo(action, Req);
            UnbindVmfromHost(action);
            break;
        }

        case EV_ANSWER_UPSTREAM:
        {
            GetActionInfo(action, Req);
            AnswerUpstream(action);
            break;
        }

        case TIMER_CHECK_VM_ALARM:
        {
            CheckVmAlarm();
            break;
        }

        default:
            break;
        }
        break;

        default:
            break;
        }
    }
}

void LifecycleManager::PowerOn( )
{
    //todo: 数据一致性审核

    //将处于ready状态的operation重新设置定时器
    printf("lifecycle_manager power on!\n");
    int ret = Transaction::Enable(GetDB());
    if (SUCCESS != ret)
    {
        Debug(SYS_ERROR,"lifecycle_manager enable transaction fail! ret = %d",ret);
        SkyExit(-1, "LifecycleManager::PowerOn: Call Transaction::Enable(GetDB()) failed.");
    }

    if (cancel_timer == INVALID_TIMER_ID)
    {
        cancel_timer = m->CreateTimer();
        TimeOut to;
        to.duration = 1000 * 15;
        to.type = LOOP_TIMER;
        to.msgid = EV_CANCEL_TIMER;
        m->SetTimer(cancel_timer,to);
    }

    VmOperate = new VmInterface(m);
    SkyAssert(VmOperate);

    // 创建需要的定时器
    TIMER_ID    timer;
    TimeOut     timeout;
    // 设置定时器: 用于虚拟机扫描清理 vm_statistics 的记录
    if (INVALID_TIMER_ID == (timer = m->CreateTimer()))
    {
        OutPut(SYS_EMERGENCY,
               "Create timer for vm statistics scan failed!\n");
        SkyAssert(INVALID_TIMER_ID != timer);
        SkyExit(-1, "LifecycleManager::PowerOn: Create timer for vm statistics scan "
                "failed.");
    }
    timeout.type = LOOP_TIMER;
    timeout.duration = 60 * 60 * 1000; // 1h
    timeout.msgid = TIMER_VM_STATISTICS_SCAN;
    if (SUCCESS != (ret = m->SetTimer(timer, timeout)))
    {
        OutPut(SYS_EMERGENCY, "Set timer for vm statistics scan failed!\n");
        SkyAssert(SUCCESS == ret);
        SkyExit(-1, "LifecycleManager::PowerOn: Set timer for vm statistics scan "
                "failed.");
    }

    timer = INVALID_TIMER_ID;
    VmHandleAck->SetTimer(timer, 10*60*1000, TIMER_CHECK_VM_ALARM);

    m->set_state(S_Work);
    OutPut(SYS_NOTICE, "%s power on!\n",m->name().c_str());
}

STATUS LifecycleManager::HandleOpReq(const VmOperationReq &req)
{
    MID vmm(MU_VM_MANAGER);
    MessageOption option(vmm,EV_VM_OP,0,0);

    return m->Send(req, option);
}

/**********************************************************************
* 函数名称：LifecycleManager::HandleMoniResult
* 功能描述：处理监控上报信息
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期     版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2011/12/01   V1.0    张文剑      创建
* 2012/04/01   V1.0    GuoXiang    修改:通过hostname得到hostid
************************************************************************/
STATUS LifecycleManager::HandleMoniResult(const MessageFrame& message)
{
    int ret = -1;

    MessageVmInfoReport report;
    report.deserialize(message.data);

    //首先提取该物理机上应该有的合法虚拟机列表
    vector<int64> legal_vms;
    int64 host_id = host_pool->GetHostIdByName(message.option.sender()._application);
    ret = vm_pool->GetVmsByHost(legal_vms,host_id,1000);
    if ((ret != SQLDB_OK) && (ret != SQLDB_RESULT_EMPTY))
    {
        return ERROR_DB_SELECT_FAIL;
    }

    if (SQLDB_RESULT_EMPTY == ret && report._vm_info.empty())
    {
        //该物理机上没有虚拟机，不管它了
        return SUCCESS;
    }

    vector<VmInfo>::iterator vit;
    for (vit=report._vm_info.begin();vit!=report._vm_info.end();vit++)
    {
        RecordVMRunInfo(*vit, host_id);
        legal_vms.erase(remove(legal_vms.begin(), legal_vms.end(), vit->_vm_id), legal_vms.end());   /* 已上报的vm在合法列表里剪掉，那剩下的就是消失的vm，需要重新创建 */
    }

    //根据报上来的信息检查该host上的虚拟机数量和id是否正确

    //vms中还有内容，说明有虚拟机消失了
    //这些虚拟机需要重新进行HA处理
    vector<int64>::iterator it;
    for (it=legal_vms.begin();it!=legal_vms.end();it++)
    {
        /* 正在执行操作的时候，状态时不稳定的，不考虑状态 */
        string wf_id;
        if (vm_pool->GetVmWorkFlowId(*it, wf_id) != SQLDB_RESULT_EMPTY)
        {
            continue;
        }

        VmOperationReq req(*it,CREATE,LCM_STAMP);
        HandleOpReq(req);
        Debug(SYS_NOTICE,"Disappeared vm %lld will be recreated on fixed host!\n",*it);
    }

    return SUCCESS;
}

//记录虚拟机运行的数据
STATUS LifecycleManager::RecordVMRunInfo(const VmInfo &vminfo, int64 hid)
{
    VirtualMachine *pvm = vm_pool->Get(vminfo._vm_id, false);

    if (NULL == pvm)
    {
        Debug(SYS_ERROR,"unknown vm(%d) on hc(%d)! Maybe u need to manual destroy it.\n", vminfo._vm_id, hid);
        if (vminfo._vm_state == VM_RUNNING)
        {
            SkyAssert(false);
        }
        else
        {//以防万一，只有非运行态的虚拟机才发CANCEL
            VmOperate->SendOperateToHC(vminfo._vm_id, CANCEL, hid);
        }
        return ERROR;
    }
    else
    {
        if (pvm->_hid != hid)
        {
            /* 正在迁移的虚拟机，有可能也在目标hc上报信息里，直接忽略 */
            if (pvm->_hid_next != hid)
            {
                //SkyAssert(false);
                Debug(SYS_ERROR,"RecordVMRunInfo: vm(%d) must not appear in hc(%d)!\n", vminfo._vm_id, hid);
                if (vminfo._vm_state != VM_RUNNING)
                {//以防万一，只有非运行态的虚拟机才发CANCEL
                    VmOperate->SendOperateToHC(vminfo._vm_id, CANCEL, hid);
                }
            }
            return ERROR;
        }
        else
        {
            VmOperate->Update(vminfo);  //更新虚拟机的在数据库的状态
            CheckVmState(vminfo._vm_id);//检查状态是否合法

            // 如下更新 VM 资源使用相关的表
            SetStatisticsInfoToDB(vminfo._vm_statistics, vminfo._vm_id);
            return SUCCESS;
        }
    }

    return ERROR;
}

/**********************************************************************
* 函数名称：LifecycleManager::Transition
* 功能描述：定义在某个生命周期状态下执行某个操作之后，
            不同的操作结果导致生命周期状态发生怎样的变化
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期     版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2011/12/01   V1.0    张文剑      创建
************************************************************************/
LcmState LifecycleManager::Transition(LcmState old_state,VmOperation op,VmOpStatus status)
{
    if (status != SUCCSSED)
        return old_state;

    //生命周期状态的跃迁理论上应该和旧的状态，操作类型，操作结果都有密切联系
    //目前暂时只考虑操作类型带来的变化，其它因素待后续进一步利用
    LcmState new_state = UNKNOWN;
    switch (op)
    {
    case SCHEDULE:
    {
        new_state = ALLOCATION;
        break;
    }

    case RESCHEDULE:
    {
        new_state = SCHEDULING;
        break;
    }

    case APPLYRES:
    {
        new_state = LCM_INIT;
        break;
    }

    case CREATE:
    {
        new_state = PROLOG;
        break;
    }

    case CANCEL:
    {
        new_state = EPILOG;
        break;
    }

    case START:
    {
        new_state = PROLOG;
        break;
    }

    case STOP:
    {
        new_state = SAVE_STOP;
        break;
    }

    case REBOOT:
    {
        new_state = DOMAINING;
        break;
    }

    case PAUSE:
    {
        new_state = SAVE_SUSPEND;
        break;
    }

    case RESUME:
    {
        new_state = PROLOG_RESUME;
        break;
    }

    case PREP_M:
    {
        new_state = MIGRATING;
        break;
    }

    case MIGRATE:
    {
        new_state = DOMAINING;
        break;
    }

    case RESET:
    {
        new_state = DOMAINING;
        break;
    }

    case DESTROY:
    {
        new_state = SAVE_STOP;
        break;
    }
    case UPLOAD_IMG:
    {
        new_state = SAVE_STOP;
        break;
    }
    case CANCEL_UPLOAD_IMG:
    {
        new_state = SAVE_STOP;
        break;
    }
    case PLUG_IN_USB:
    case PLUG_OUT_USB:
    case OUT_IN_USB:
    {
        new_state = PROLOG;
        break;
    }

    default:
    {
        break;
    }
    }

    SkyAssert(new_state != UNKNOWN);
    return new_state;
}

STATUS LifecycleManager::CheckVmState(int64 vid)
{
    VirtualMachine *pvm = vm_pool->Get(vid,false);
    SkyAssert(pvm);

    /* 正在执行操作的时候，状态时不稳定的，不考虑状态 */
    string wf_id;
    if (vm_pool->GetVmWorkFlowId(vid, wf_id) != SQLDB_RESULT_EMPTY)
    {
        return SUCCESS;
    }

    if ((pvm->_expected_state == VM_RUNNING) && (pvm->_state == VM_SHUTOFF))
    {
        VmOperationReq req(vid, START, LCM_STAMP);
        return HandleOpReq(req);
    }

    return SUCCESS;
}

int LifecycleManager::HandleCfgReq(const MessageFrame& message)
{
    string hc_name(message.data);

    //首先提取该物理机上应该有的合法虚拟机列表
    HostPool *host_pool = HostPool::GetInstance();
    if (!host_pool)
    {
        Debug(SYS_ERROR,"HandleMoniResult failed to get host_pool.\n");
        return ERROR_NOT_READY;
    }

    vector<int64> vms;
    int64 host_id = host_pool->GetHostIdByName(hc_name);
    int ret = vm_pool->GetVmsByHost(vms,host_id,1000);
    if ((ret != SQLDB_OK) && (ret != SQLDB_RESULT_EMPTY))
    {
        return ERROR_DB_SELECT_FAIL;
    }

    VmHandlerCfgAckInfo vmscfg;
    if (SUCCESS == ret)
    {
        vector<int64>::iterator it;
        for (it = vms.begin(); it != vms.end(); it ++)
        {
            string wf_id;
            if (SQLDB_OK == vm_pool->GetVmWorkFlowId(*it, VM_WF_DEPLOY, wf_id))
            {//正在部署的虚拟机，配置有可能还是错误的，不下发
                continue;
            }

            VirtualMachine *pvm = vm_pool->Get(*it, false);
            SkyAssert(pvm);

            VmCreateReq createreq(pvm->_config);
            createreq._vid= *it;
            createreq._stamp=LCM_STAMP;
            GetVstorageOption(createreq._option._image_store_option);

            vmscfg._create_info.push_back(createreq);
        }
    }

    return SendMsgToHC(m, vmscfg.serialize(), host_id, EV_VMAGT_CFG_ACK, PROC_HC, MU_VM_HANDLER);
}

int LifecycleManager::ResponseFeedback(const VmAction &action, const WorkAck &ack)
{
    int feedback_func;
    if (ack.state == SUCCESS)
    {
        feedback_func = action.success_feedback;
    }
    else if (ack.state == ERROR_ACTION_TIMEOUT)
    {
        feedback_func = action.timeout_feedback;
    }
    else
    {
        feedback_func = action.failure_feedback;
    }

    SkyAssert(feedback_func);

    return ResponseFeedback(feedback_func, action, ack);
}

int LifecycleManager::ResponseFeedback(const int func, const VmAction &action, const WorkAck &ack)
{
    switch (func)
    {
        case FEEDBACK_RESTART_ACTION:
        {
            return RestartAction(action.get_id());
        }
        case FEEDBACK_ROLLBACK_WORKFLOW:
        {
            string w_id;
            GetWorkflowByActionId(action.get_id(),w_id);
            OutPut(SYS_ERROR,"lcm, workflow:%s,action %s failed! begin rollback\n",w_id.c_str(),action.get_id().c_str());
            UnlockWorkflow(action.get_id());
            while(RollbackWorkflow(action.get_id()) == ERROR_OP_RUNNING);
            return UpdateWorkflowHistory(w_id,ack.detail);
        }
        case FEEDBACK_DELETE_WORKFLOW:
        {
            SendVmOpAck(m, action);
            UnlockWorkflow(action.get_id());
            return DeleteWorkflow(action.get_id());
        }
        case FEEDBACK_END_VMOP:
        {
            return EndVmOp(action);
        }
        case FEEDBACK_UPDATE_VMLOCALIMAGE:
        {
            return UpdateVMLocalImage(action);
        }
        case FEEDBACK_UPDATE_VMSHAREIMAGE:
        {
            return UpdateVMShareImage(action);
        }
        case FEEDBACK_UPDATE_VMSHAREBLOCK:
        {
            return UpdateVMShareBlock(action);
        }
        case FEEDBACK_MIGRATE_ADD_VMSHAREIMAGE:
        {
            return AddMigrateVMShareImage(action);
        }
        case FEEDBACK_COLDMIGRATE_ADD_VMSHAREIMAGE:
        {
            return AddColdMigrateVMShareImage(action);
        }
        case FEEDBACK_MIGRATE_ADD_VMSHAREBLOCK:
        {
            return AddMigrateVMShareBlock(action);
        }
        case FEEDBACK_COLDMIGRATE_ADD_VMSHAREBLOCK:
        {
            return AddColdMigrateVMShareBlock(action);
        }
        case FEEDBACK_DELETE_VMSHAREIMAGE:
        {
            return DeleteVMShareImage(action);
        }
        case FEEDBACK_DELETE_VMSHAREBLOCK:
        {
            return DeleteVMShareBlock(action);
        }
        case FEEDBACK_PLUGINUSB_FAILED:
        {
            return PluginUSBFailed(action);
        }
        case FEEDBACK_PLUGOUTUSB_SUCCESSED:
        {
            return PlugoutUSBSuccessed(action);
        }

        case FEEDBACK_MODIYF_PDISKREQ:
        {
            return ModifyPlugPdiskAction(action);
        }

        case FEEDBACK_PLUGIN_PDISK:
        {
            return PlugInPDiskSuccess(action);
        }

        case FEEDBACK_PLUGOUT_PDISK:
        {
            return PlugOutPDiskSuccess(action);
        }

        case FEEDBACK_MODIFY_CREATESNAPSHOT:
        {
            return ModifyCreateSnapshot(action);
        }

        case FEEDBACK_MIGRATE_SUCCESS:
        {
            return MigrateSuccess(action);
        }

        case FEEDBACK_CREATE_BACKUPIMG:
        {
            return BackupImgCreateSuccess(action);
        }
        case FEEDBACK_DELETE_BACKUPIMG:
        {
            return BackupImgDeleteSuccess(action);
        }
        case FEEDBACK_ACKVMOP_AND_DELETEWORKFLOW:
        {
            AcktoUpstream(m, ack);            
            UnlockWorkflow(action.get_id());
            return DeleteWorkflow(action.get_id());
        }
        case FEEDBACK_ACKVMOP_AND_FINISHACTION:
        {
            AcktoUpstream(m, ack);            
            return FinishAction(action.get_id());
        }
        case FEEDBACK_DELETE_MACHINE:
        {
            return DeleteVmMachine(m, action._vid, ack.action_id);
        }
        case FEEDBACK_INSERT_MACHINE:
        {
            return InsertVmMachine(m, action._vid, ack.action_id);
        }
        case FEEDBACK_NULL:
        {
            return 0;
        }
        default:
        {
            SkyAssert(false);
            return 0;
        }
    }
}

//结束虚拟机的操作
int LifecycleManager::EndVmOp(const VmAction &action)
{
    SendVmOpAck(m, action);

    UnlockWorkflow(action.get_id());
    return FinishAction(action.get_id());
}

//拔出USB成功
//其实和插入USB失败做同样的处理: 删除USB_POOL
int LifecycleManager::PlugoutUSBSuccessed(const VmAction &action)
{
    return PluginUSBFailed(action);
}

//插入USB失败了，那就需要将USB_POOL删除，然后给TC应答
int LifecycleManager::PluginUSBFailed(const VmAction &action)
{
    VmUsbOp usb_req;
    bool result = usb_req.deserialize(action.message_req);
    SkyAssert(result);

    VmOperate->DeleteVmUsbInfo(usb_req);

    return EndVmOp(action);
}

//根据IA的应答，修改createsnapshotreq
int LifecycleManager::ModifyCreateSnapshot(const VmAction &action)
{
    ImageUrlPrepareAck ack;
    SkyAssert(true == ack.deserialize(action.message_ack));

    vector<string> actions;
    STATUS ret = GetNextActions(action.get_id(), actions);
    SkyAssert(ret == SUCCESS);
    SkyAssert(1 == actions.size());

    CreateSnapshotReq req;
    req.action_id = actions[0];
    req._request_id = ack._request_id;
    req._vec_baseid = ack._base_uuid_vec;
    req._volsize = ack._size;

    TransactionOp tsop; //启用事务操作
    UpdateActionReq(actions[0], req);

    FinishAction(action.get_id());

    ret = tsop.Commit();
    SkyAssert(ret == SUCCESS);
    return ret;
}

//根据IA的应答，修改vm_pool
int LifecycleManager::UpdateVMLocalImage(const VmAction &action)
{
    ImageUrlPrepareAck ack;
    SkyAssert(true == ack.deserialize(action.message_ack));

    VmOperate->UpdateVMLocalImage(action._vid, ack);

    return FinishAction(action.get_id());
}

//根据SC的应答，修改vm_pool和vm_image
int LifecycleManager::UpdateVMShareImage(const VmAction &action)
{
    AuthBlockAck ack;
    AuthBlockReq req;
    req.deserialize(action.message_req);
    SkyAssert(true == ack.deserialize(action.message_ack));

    VmOperate->UpdateVMShareImage(req._iscsiname,action._vid, ack,true);
    return FinishAction(action.get_id());
}

int LifecycleManager::UpdateVMShareBlock(const VmAction &action)
{
    AuthBlockAck ack;
    AuthBlockReq req;
    req.deserialize(action.message_req);
    SkyAssert(true == ack.deserialize(action.message_ack));

    VmOperate->UpdateVMShareDisk(req._iscsiname,action._vid, ack,true);
    return FinishAction(action.get_id());
}



//根据SC的应答，修改vm_pool和vm_image
int LifecycleManager::AddMigrateVMShareImage(const VmAction &action)
{
    AuthBlockAck ack;
    AuthBlockReq req;
    req.deserialize(action.message_req);
    SkyAssert(true == ack.deserialize(action.message_ack));

    VmOperate->UpdateVMShareImage(req._iscsiname,action._vid, ack,true);
    VirtualMachine *pvm = vm_pool->Get(action._vid, false);
    SkyAssert(pvm);

    string action_id=action.get_id();
    string rollback_id;
    int ret;
    ret = GetRollbackAction(action_id, rollback_id);
    SkyAssert(ret == SUCCESS);
    UpdateActionMsgReq(action_id, VM_ACT_PRAPARE_M, VmMigrateData, _config, pvm->_config);
    UpdateActionMsgReq(action_id, VM_ACT_MIGRATE, VmMigrateData, _config, pvm->_config);
    UpdateActionMsgReq(action_id, VM_ACT_POST_M, VmMigrateData, _config, pvm->_config);
    UpdateActionMsgReq(rollback_id, VM_ACT_REMOVE, VmMigrateData, _config, pvm->_config);
    UpdateActionMsgReq(rollback_id, VM_ACT_CREATE, VmMigrateData, _config, pvm->_config);
    return FinishAction(action.get_id());
}

//根据SC的应答，修改vm_pool和vm_image
int LifecycleManager::AddColdMigrateVMShareImage(const VmAction &action)
{
    AuthBlockAck ack;
    AuthBlockReq req;
    req.deserialize(action.message_req);
    SkyAssert(true == ack.deserialize(action.message_ack));

    VmOperate->UpdateVMShareImage(req._iscsiname,action._vid, ack,true);
    VirtualMachine *pvm = vm_pool->Get(action._vid, false);
    SkyAssert(pvm);

    string action_id=action.get_id();
    string rollback_id;
    int ret;
    ret = GetRollbackAction(action_id, rollback_id);
    SkyAssert(ret == SUCCESS);
    UpdateActionMsgReq(action_id,  VM_ACT_PRAPARE_M, VmMigrateData, _config, pvm->_config);
    UpdateActionMsgReq(rollback_id, VM_ACT_REMOVE, VmMigrateData, _config, pvm->_config);
    UpdateActionMsgReq(rollback_id, VM_ACT_PRAPARE_M, VmMigrateData, _config, pvm->_config);

    return FinishAction(action.get_id());
}


int LifecycleManager::AddMigrateVMShareBlock(const VmAction &action)
{
    AuthBlockAck ack;
    AuthBlockReq req;
    req.deserialize(action.message_req);
    SkyAssert(true == ack.deserialize(action.message_ack));

    VmOperate->UpdateVMShareDisk(req._iscsiname,action._vid, ack,true);
    VirtualMachine *pvm = vm_pool->Get(action._vid, false);
    SkyAssert(pvm);

    string action_id=action.get_id();
    string rollback_id;
    int ret;
    ret = GetRollbackAction(action_id, rollback_id);
    SkyAssert(ret == SUCCESS);
    UpdateActionMsgReq(action_id, VM_ACT_PRAPARE_M, VmMigrateData, _config, pvm->_config);
    UpdateActionMsgReq(action_id, VM_ACT_MIGRATE, VmMigrateData, _config, pvm->_config);
    UpdateActionMsgReq(action_id, VM_ACT_POST_M, VmMigrateData, _config, pvm->_config);
    UpdateActionMsgReq(rollback_id, VM_ACT_REMOVE, VmMigrateData, _config, pvm->_config);
    UpdateActionMsgReq(rollback_id, VM_ACT_CREATE, VmMigrateData, _config, pvm->_config);
    return FinishAction(action.get_id());
}

int LifecycleManager::AddColdMigrateVMShareBlock(const VmAction &action)
{
    AuthBlockAck ack;
    AuthBlockReq req;
    req.deserialize(action.message_req);
    SkyAssert(true == ack.deserialize(action.message_ack));

    VmOperate->UpdateVMShareDisk(req._iscsiname,action._vid, ack,true);
    VirtualMachine *pvm = vm_pool->Get(action._vid, false);
    SkyAssert(pvm);

    string action_id=action.get_id();
    string rollback_id;
    int ret;
    ret = GetRollbackAction(action_id, rollback_id);
    SkyAssert(ret == SUCCESS);
    UpdateActionMsgReq(action_id,  VM_ACT_PRAPARE_M, VmMigrateData, _config, pvm->_config);
    UpdateActionMsgReq(rollback_id, VM_ACT_REMOVE, VmMigrateData, _config, pvm->_config);
    UpdateActionMsgReq(rollback_id, VM_ACT_PRAPARE_M, VmMigrateData, _config, pvm->_config);
    return FinishAction(action.get_id());
}

//根据SC的应答，修改vm_pool和vm_image
int LifecycleManager::DeleteVMShareImage(const VmAction &action)
{
    DeAuthBlockAck deauthack;
    AuthBlockAck   authack;
    AuthBlockReq req;
    req.deserialize(action.message_req);
    SkyAssert(true == deauthack.deserialize(action.message_ack));
    authack._request_id=deauthack._request_id;
    VmOperate->UpdateVMShareImage(req._iscsiname,action._vid, authack,false);

    return FinishAction(action.get_id());
}

int LifecycleManager::DeleteVMShareBlock(const VmAction &action)
{
    DeAuthBlockAck deauthack;
    AuthBlockAck   authack;
    AuthBlockReq req;
    req.deserialize(action.message_req);
    SkyAssert(true == deauthack.deserialize(action.message_ack));
    authack._request_id=deauthack._request_id;
    VmOperate->UpdateVMShareDisk(req._iscsiname,action._vid, authack,false);

    return FinishAction(action.get_id());
}

//删除vm_pool并向tc应答
int LifecycleManager::FinishVMCancel(VmAction &action, const WorkReq &req)
{
    VmOperationAck ack_tc;
    ack_tc.action_id = action.get_id();
    ack_tc._vid = action._vid;
    ack_tc._operation = CANCEL;
    ack_tc.state = SUCCESS;
    ack_tc.progress = 100;
    action.message_ack = ack_tc.serialize();
    SendVmOpAck(m, action); //向TC应答, 应答需要在 FinishAction 之前，否则action已经结束，应答的数据就没有了

    WorkAck ack(req.action_id, SUCCESS, 100, "SUCCESS");
    UpdateActionAck(req.action_id, ack.serialize());

    TransactionOp tsop; //启用事务操作

    VMImagePool *vmimage_pool = VMImagePool::GetInstance();
    SkyAssert(vmimage_pool);
    STATUS ret1 = SUCCESS;
    STATUS ret2 = SUCCESS;

    VirtualMachine *pvm = vm_pool->Get(action._vid, false);
    if (pvm != NULL)
    {
        vm_pool->Drop(pvm);
        ret1 = vmimage_pool->DeleteImageBackupByVid(action._vid, pvm->_config._machine._target);
        if(SUCCESS != ret1)
        {
            Debug(SYS_ERROR,"FinishVMCancel: DeleteImageBackupByVid del fail.\n");
        }
    }

    UnlockWorkflow(action.get_id());
    FinishAction(action.get_id());

    ret2 = tsop.Commit();
    SkyAssert(ret2 == SUCCESS);
    SkyAssert(ret1 == SUCCESS);
    if (ret1 != SUCCESS)
    {
        return ret1;
    }

    return ret2;
}

int LifecycleManager::FinishVMFreeze(VmAction &action, const WorkReq &req)
{
    VirtualMachine *pvm = vm_pool->Get(action._vid, true);
    if (!pvm)
    {
        SkyAssert(false);
        return ERR_VM_NOT_EXIST;
    }

    VMManager *vmm = VMManager::GetInstance();
    if (vmm->IsVMSupportMigrate(action._vid))
    {//如虚拟机支持冷迁移，那vm冻结之后可以完全跟hc脱离关系了，所以，hid字段清空
        pvm->_hid = INVALID_HID;
    }

    if (IsRollbackWorkflow(action.get_id()))
    {
        pvm->_hid = INVALID_HID;
    }

    //虚拟机状态冻结
    pvm->_log_state = VM_FROZEN;

    vm_pool->Update(pvm);
    vm_pool->Put(pvm, true);

    VmOperationAck ack_tc;
    ack_tc.action_id = action.get_id();
    ack_tc._vid = action._vid;
    ack_tc._operation = FREEZE;
    ack_tc.state = SUCCESS;
    ack_tc.progress = 100;
    action.message_ack = ack_tc.serialize();
    SendVmOpAck(m, action); //向TC应答, 应答需要在 FinishAction 之前，否则action已经结束，应答的数据就没有了

    UnlockWorkflow(action.get_id());
    return FinishAction(action.get_id());
}

int LifecycleManager::ModifyCreateVMAction(const VmAction &action, const WorkReq &req)
{
    int vid = action._vid;

    VirtualMachine *pvm = vm_pool->Get(vid, false);
    SkyAssert(pvm);

    VmCreateReq createreq(vid, CREATE);
    VmOperate->ConstructCreateReq(vid, createreq);

    vector<string> actions;
    STATUS ret = GetNextActions(action.get_id(), actions);
    SkyAssert(ret == SUCCESS);
    SkyAssert(1 == actions.size());

    const string action_id = actions[0];

    TransactionOp tsop; //启用事务操作

    //修改正向
    createreq.action_id = action_id;
    UpdateActionReq(action_id, createreq);

    MID hc(host_pool->GetHostNameById(pvm->_hid), PROC_HC, MU_VM_HANDLER);
    UpdateActionReceiver(action_id, hc);

    //修改回滚
    string rollback_id;
    GetRollbackAction(action_id, rollback_id);

    createreq._operation = CANCEL;
    createreq.action_id = rollback_id;
    createreq.retry     = true;         //回滚流的删除，不接受失败
    UpdateActionReq(rollback_id, createreq);

    UpdateActionReceiver(rollback_id, hc);

    FinishAction(action.get_id());

    ret = tsop.Commit();
    SkyAssert(ret == SUCCESS);
    return ret;
}

int LifecycleManager::ModifyPlugPdiskAction(const VmAction &action)
{
    int vid = action._vid;

    AuthBlockAck ack;
    SkyAssert(true == ack.deserialize(action.message_ack));

    VmAttachReq attachreq;
    vector<string> actions;
    STATUS ret = GetNextActions(action.get_id(), actions);
    SkyAssert(ret == SUCCESS);
    SkyAssert(1 == actions.size());

    const string action_id = actions[0];

    TransactionOp tsop; //启用事务操作

    Action  action1;
    VmPortableDiskOp req1;
    GetActionById(action_id, action1);
    req1.deserialize(action1.message_req);
    attachreq.action_id = action_id;
    attachreq._vid=vid;
    attachreq._diskcfg._id=INVALID_OID;
    attachreq._diskcfg._position=VM_STORAGE_TO_SHARE;
    attachreq._diskcfg._type=DISK_TYPE_DISK;
    attachreq._diskcfg._bus=req1._bus;
    attachreq._diskcfg._target=req1._target;
    attachreq._diskcfg._size = 0;
    attachreq._diskcfg._request_id = req1._request_id;
    attachreq._operation = PLUG_IN_PDISK;
    attachreq._stamp = req1._target;

    iSCSIAccessOption option;
    AuthBlockReq req;
    req.deserialize(action.message_req);
    option._type = ack._type;
    option._dataip_array = ack._dataip_vec;
    LunAddr  addr(ack._target_name,ack._lunid);
    option._type=ack._type;
    vector<string>::iterator it;

    option._request_id=ack._request_id;
    option._auth_tbl.AddAuth(req._iscsiname, addr);
    attachreq._diskcfg._share_url.access_type = STORAGE_ACCESS_ISCSI;
    attachreq._diskcfg._share_url.access_option = option.serialize();
    attachreq._diskcfg._is_need_release = false;

    UpdateActionReq(action_id, attachreq);
    //修改回滚
    string rollback_id;
    GetRollbackAction(action_id, rollback_id);
    attachreq.action_id = rollback_id;
    UpdateActionReq(rollback_id, attachreq);

    FinishAction(action.get_id());
    ret = tsop.Commit();
    SkyAssert(ret == SUCCESS);
    return ret;
}

int LifecycleManager::PlugInPDiskSuccess(const VmAction &action)
{
    int ret = ERROR;
    int64 vid = action._vid;
    TransactionOp tsop; //启用事务操作
    VirtualMachine *pvm = vm_pool->Get(vid, true);
    SkyAssert(pvm);
    VmAttachReq req;
    req.deserialize(action.message_req);
    VMImagePool *image_pool = VMImagePool::GetInstance();
    SkyAssert(image_pool);
    image_pool->InsertDisk(vid, req._diskcfg);
    pvm->_config._disks.push_back(req._diskcfg);
    vm_pool->Update(pvm);
    FinishAction(action.get_id());
    vm_pool->Put(pvm,true);
    ret = tsop.Commit();
    SkyAssert(ret == SUCCESS);
    return ret;
}

int LifecycleManager::PlugOutPDiskSuccess(const VmAction &action)
{
    int ret = ERROR;
    int64 vid = action._vid;

    TransactionOp tsop; //启用事务操作
    VirtualMachine *pvm = vm_pool->Get(vid, true);
    SkyAssert(pvm);
    DeAuthBlockReq req;
    req.deserialize(action.message_req);

    VmDiskConfig disk;
    vector<VmDiskConfig>::iterator it = pvm->_config._disks.begin();
    for(; it != pvm->_config._disks.end(); it++)
    {
        if (it->_request_id == req._request_id)
        {
            VMImagePool *image_pool = VMImagePool::GetInstance();
            SkyAssert(image_pool);
            disk = *it;
            image_pool->DeleteDisk(vid, disk);
            pvm->_config._disks.erase(it);
            vm_pool->Update(pvm);
            break;
        }
    }
    FinishAction(action.get_id());
    vm_pool->Put(pvm,true);
    ret = tsop.Commit();
    SkyAssert(ret == SUCCESS);
    return ret;
}

int LifecycleManager::BackupImgCreateSuccess(const VmAction &action)
{
    int ret = ERROR;
    VMImagePool *vmimage_pool = VMImagePool::GetInstance();
    SkyAssert(vmimage_pool);
    VMImgBackup imgbackup;
    TransactionOp tsop; //启用事务操作
    string  request_id;

    VmImageBackupReq req;
    req.deserialize(action.message_req);
    if(vmimage_pool->IsImageBackupExist(req._request_id))
    {
        FinishAction(action.get_id());
        ret = tsop.Commit();
        return ret;
    }

    if(!req._disk._target.empty())
    {
        ret = vmimage_pool->GetActiveImageBackup(request_id,req._disk._target,req._vid);
        if(SUCCESS==ret&&(!request_id.empty()))
        {
            ret = vmimage_pool->UpdateImageBackup(request_id,IMG_BACKUP_BACKED );
            if(ret != SUCCESS)
            {
                Debug(SYS_ERROR,"BackupImgCreateSuccess update imgbackup fail.\n");
                SkyAssert(false);
                return ret;
            }
        }
        else if(ret != ERROR_OBJECT_NOT_EXIST)
        {
            Debug(SYS_ERROR,"BackupImgCreateSuccess get imgbackup fail.\n");
            SkyAssert(false);
            return ret;
        }
    }
    else
    {
        Debug(SYS_ERROR,"BackupImgCreateSuccess target null.\n");
        SkyAssert(false);
        return ret;
    }

    imgbackup._create_time.refresh();
    imgbackup._parent_request_id=req._disk._request_id;
    imgbackup._request_id=req._request_id;
    imgbackup._state=IMG_BACKUP_ACTIVE;
    imgbackup._target=req._disk._target;
    imgbackup._vid=req._vid;
    imgbackup._description=req._description;
    ret = vmimage_pool->InsertImageBackup(imgbackup);
    FinishAction(action.get_id());
    ret = tsop.Commit();
    SkyAssert(ret == SUCCESS);
    return ret;
}

int LifecycleManager::BackupImgDeleteSuccess(const VmAction &action)
{
    int ret = ERROR;
    VMImagePool *vmimage_pool = VMImagePool::GetInstance();
    SkyAssert(vmimage_pool);
    TransactionOp tsop; //启用事务操作
    string  request_id;

    VmImageBackupReq req;
    req.deserialize(action.message_req);
    if(!vmimage_pool->IsImageBackupExist(req._request_id))
    {
        FinishAction(action.get_id());
        ret = tsop.Commit();
        return ret;
    }

    if(!req._disk._target.empty())
    {
        ret = vmimage_pool->DeleteImageBackup(req._request_id);
        if(SUCCESS!=ret)
        {
            Debug(SYS_ERROR,"BackupImgDeleteSuccess: DeleteImageBackup del fail.\n");
            SkyAssert(false);
            return ret;
        }
    }
    else
    {
        Debug(SYS_ERROR,"BackupImgDeleteSuccess target null.\n");
        SkyAssert(false);
        return ret;
    }

    FinishAction(action.get_id());
    ret = tsop.Commit();
    SkyAssert(ret == SUCCESS);
    return ret;
}

int LifecycleManager::MigrateSuccess(const VmAction &action)
{
    VmOperate->LiveMigrateSuccess(action);

    return EndVmOp(action);
}

int LifecycleManager::MigrateFailed(VmAction &action, const WorkReq &req)
{
    VmOperate->SetValue(action._vid, "hid_next", INVALID_HID);

    VmOperationAck ack_tc;
    ack_tc.action_id = action.get_id();
    ack_tc._vid = action._vid;
    ack_tc._operation = MIGRATE;
    ack_tc.state = SUCCESS;
    ack_tc.progress = 100;
    action.message_ack = ack_tc.serialize();
    SendVmOpAck(m, action); //向TC应答, 应答需要在 FinishAction 之前，否则action已经结束，应答的数据就没有了

    UnlockWorkflow(action.get_id());
    return FinishAction(action.get_id());
}

/**********************************************************************
* 函数名称：LifecycleManager::DoProjectDynamicDataReq
* 功能描述：处理project动态数据请求信息
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期     版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2013/10/22   V1.0    zhoubin      创建
************************************************************************/
STATUS LifecycleManager::DoProjectDynamicDataReq(const MessageFrame& message)
{
    MessageProjectDataReq req;
    req.deserialize(message.data);

    MessageProjectDataAck ack;
    ack._project_name = req._project_name;
    ack._ret_code = 0;
    ack._max_count = 0;
    ack._next_index = -1;

    if (!req._project_name.empty())
    {
        VmDynamicData vm_dynamic_data;
        ack._max_count = vm_dynamic_data.CountProjectDynamicData();


        ostringstream oss;
        oss << " (vid != -1) " 
        << " ORDER BY vid LIMIT " << req._count 
        << " offset " << req._start_index;
        
        if(0 != vm_dynamic_data.GetProjectDynamicData(oss.str(),ack._project_data))
        {
            ack._ret_code = -1;
        }
        else
        {
            ack._ret_code = 0;
            if (static_cast<int64>(ack._project_data.size()) >= req._count )
            {
                ack._next_index = req._start_index + req._count;
            }
        }
    }


    MessageOption option(message.option.sender(), EV_PROJECT_DYNAMIC_DATA_ACK, 0, 0);
    return m->Send(ack, option);

}


int LifecycleManager::ModifyNextAction(const VmAction &action, const WorkReq &req)
{
    if (VmOperate->CheckReqDoing(req))
    {
        return 0;
    }

    vector<string> actions;
    STATUS ret = GetNextActions(action.get_id(), actions);
    SkyAssert(ret == SUCCESS);
    SkyAssert(1 == actions.size());

    const string action_id = actions[0];
    if (IsActionSkipped(action_id))
    {//下一个动作如果是skip，那就不需要修改了，比如回滚流的一些动作
        return 0;
    }

    int predecessor_func;
    ret = GetActionFeedback(action.get_id(), Action::ACT_SUCCESS, predecessor_func);
    SkyAssert(ret == SUCCESS);

    switch (predecessor_func)
    {
        case PREDECESSOR_SET_REQ_FOR_PREPARE_VM:
        {
            UpdateActionVMConfigReq<VmMigrateData>(action_id);
            break;
        }
        case PREDECESSOR_SET_REQ_FOR_CREATE_VM:
        case PREDECESSOR_SET_REQ_FOR_REMOVE_VM:
        {//都是修改发送给HC的虚拟机的配置
            UpdateActionVMConfigReq<VmCreateReq>(action_id);
            break;
        }

        default:
        {
            SkyAssert(false);
        }
    }

    return FinishAction(action.get_id());
}

int LifecycleManager::UnbindVmfromHost(const VmAction &action)
{
    VmOperationReq req;
    SkyAssert(req.deserialize(action.message_req));

    VmOperate->SetValue(req._vid, "hid", INVALID_HID);

    return FinishAction(action.get_id());
}

int LifecycleManager::AnswerUpstream(const VmAction &action)
{
    MID upstream;
    STATUS ret = GetUpstreamSender(action.get_id(), upstream);
    SkyAssert(ret == SUCCESS);
    MessageOption option(upstream,EV_VM_OP_ACK,0,0);

    m->Send(action.message_ack, option);

    return FinishAction(action.get_id());
}

/******************************************************************************/
void LifecycleManager::DoScanForDrop()
{
    time_t now = time(0);
    // 所有 vm 一视同仁
    time_t duration = statistics_reserve * 60 * 60;

    time_t need_drop = now - duration;

    Datetime drop(need_drop);

    _vm_statistics->Delete(drop.tostr());
    _vm_statistics_nics->Delete(drop.tostr());
}

/******************************************************************************/
void LifecycleManager::SetStatisticsInfoToDB(const ResourceStatistics& vm_statistics,
                                             int64 vid)
{
    if (0 == statistics_reserve)
    {// 默认配置为 0 不入库
        return;
    }
    Datetime now;
    string save_time = now.tostr();

    ostringstream where;
    where << " WHERE vid = " << vid
          << " AND statistics_time = '" << vm_statistics._statistics_time << "'";
    vector<ResourceStatistics> vec_vm_statistics;
    if ((0 == _vm_statistics->Search(vec_vm_statistics, where.str()))
        && (true == vec_vm_statistics.empty()))
    {
        /* 因 HC 统计时间可能大于上报时间, 会有重复, 为避免 INSERT 失败
         * 增加数据库的日志, 占用很多的硬盘空间, 故增加检查
         */
        _vm_statistics->Insert(vm_statistics, vid, save_time);
    }

    vector<VmStatisticsNics> vec_nic_info;
    vector<NicInfo>::const_iterator it;
    for (it  = vm_statistics._vec_nic_info.begin();
         it != vm_statistics._vec_nic_info.end();
         it++)
    {
        where.str("");
        vec_nic_info.clear();
        where << " WHERE vid = " << vid
              << " AND name = '" << it->_name << "'"
              << " AND statistics_time = '" << vm_statistics._statistics_time << "'";
        if ((0 == _vm_statistics_nics->GetVmStatisticsNics(vec_nic_info, where.str()))
            && (true == vec_nic_info.empty()))
        {
            _vm_statistics_nics->Insert(*it,
                                        vid,
                                        vm_statistics._statistics_time,
                                        save_time);
        }
    }
}
/* 判断TCU是否是共享的 */

void LifecycleManager::CheckVmAlarm()
{
    vector<int64> vms;
    vm_pool->GetAllVms(vms, 1000);

    vector<int64>::iterator it;
    for (it = vms.begin(); it != vms.end(); it++)
    {
        VirtualMachine *pvm = vm_pool->Get(*it, false);
        SkyAssert(pvm);

        SendVmOfflineAlarm(*pvm);
        SendVmStartFailedAlarm(*pvm);
        SendVmStateConflictAlarm(*pvm);
    }
}

void LifecycleManager::SendVmOfflineAlarm(const VirtualMachine &vm)
{
    ostringstream where;
    int state = VM_UNKNOWN;
    where << "vid = " << vm._vid;
    vm_pool->SelectColumn("web_view_vmstate", "state", where.str(), state);

    int flag = (state == VM_UNKNOWN) ? OAM_REPORT_ALARM : OAM_REPORT_RESTORE;
    SendVmAlarm(vm, ALM_TECS_VM_OFF_LINE, flag);
}

void LifecycleManager::SendVmStartFailedAlarm(const VirtualMachine &vm)
{
    if (!((vm._expected_state == VM_RUNNING) && (vm._state == VM_SHUTOFF)))
    {
        SendVmAlarm(vm, ALM_TECS_VM_START_FAILED, OAM_REPORT_RESTORE);
    }
}

void LifecycleManager::SendVmStateConflictAlarm(const VirtualMachine &vm)
{
    int flag;
    if ((vm._log_state == VM_FROZEN) && (vm._state != VM_SHUTOFF))
    {
        flag = OAM_REPORT_ALARM;
    }
    else
    {
        flag = OAM_REPORT_RESTORE;
    }

    SendVmAlarm(vm, ALM_TECS_VM_STATE_CONFLICT, flag);
}

bool IsTcuShare()
{
    return enable_tcu_share;
}

void DbgDeleteVm(int64 vid)
{
    VirtualMachinePool *vm_pool = VirtualMachinePool::GetInstance();
    if (!vm_pool)
    {
        printf("get vm_pool failed!\n");
        return;
    }

    VirtualMachine *pvm = vm_pool->Get(vid,true);
    if (!pvm)
    {
        printf("VM %lld not exist!\n",vid);
        return;
    }

    ostringstream error;
    vm_pool->Drop(pvm);
    vm_pool->Put(pvm,true);
    printf("VM %lld deleted!\n",vid);
}
DEFINE_DEBUG_FUNC(DbgDeleteVm);

void DbgShowVMCancel()
{
    return; //todo
}
DEFINE_DEBUG_CMD(VMCancels,DbgShowVMCancel);
}

