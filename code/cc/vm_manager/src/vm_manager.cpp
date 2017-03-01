/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vm_manager.cpp
* 文件标识：见配置管理计划书
* 内容摘要：虚拟机管理器模块实现文件
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
#include "vm_manager.h"
#include "lifecycle_manager.h"
#include "scheduler.h"
#include "host_pool.h"
#include "log_agent.h"
#include "vstorage_manager.h"
#include "db_config.h"
#include "tecs_config.h"
#include "host_pool.h"
#include "vm_messages.h"
#include "workflow.h"
#include "vnetlib_event.h"
#include "vnetlib_msg.h"
#include "vnet_libnet.h"
#include "config_cluster.h"
#include "modify_vmcfg.h"

static int vmmgt_print_on = 0;
DEFINE_DEBUG_VAR(vmmgt_print_on);
#define Debug(level,fmt,arg...) \
        do \
        { \
            OutPut(level,fmt,##arg);\
        }while(0)

namespace zte_tecs
{

VMManager* VMManager::instance = NULL;
string VMManager::tc;

#define EV_USB_QUERY_TIMER  EV_TIMER_1

STATUS VMManager::Init()
{
    vm_pool = VirtualMachinePool::GetInstance();
    if(!vm_pool)
    {
        return ERROR_NOT_READY;
    }
    usb_pool = VmUsbPool::GetInstance();
    if(!usb_pool)
    {
        return ERROR_NOT_READY;
    }

    host_pool = HostPool::GetInstance();
    if(!host_pool)
    {
        return ERROR_NOT_READY;
    }

    vm_statistics_nics_pool = ConfigVmStatisticsNics::GetInstance();
    if(!vm_statistics_nics_pool)
    {
        return ERROR_NOT_READY;
    }

    //每5分钟对vm_pool表做一次数据库清理动作
    STATUS ret = RegisterDbVacuum(VirtualMachine::table,300);
    if(SUCCESS != ret)
    {
        return ret;
    }

    //创建、注册、启动消息单元
    ret = Start(MU_VM_MANAGER);
    if(SUCCESS != ret)
    {
        return ret;
    }

    //给自己发一个上电通知消息
    MessageOption option(MU_VM_MANAGER,EV_POWER_ON,0,0);
    MessageFrame frame(SkyInt(0),option);
    return m->Receive(frame);
}

/**********************************************************************
* 函数名称：VMManager::MessageEntry
* 功能描述：VMManager消息单元主处理循环
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
void VMManager::MessageEntry(const MessageFrame& message)
{
    STATUS ret;
    ostringstream oss;

    switch(m->get_state())
    {
        case S_Startup:
        {
            switch(message.option.id())
            {
                //vm_manager上电，需要支持异常重启
                case EV_POWER_ON:
                {
                    ret = Transaction::Enable(GetDB());
                    if(SUCCESS != ret)
                    {
                        Debug(SYS_ERROR,"vm manager enable transaction fail! ret = %d",ret);
                        SkyExit(-1, "VMManager::MessageEntry: call Transaction::Enable(GetDB()) failed.");
                    }

                    Debug(SYS_DEBUG,"vm_manager power on!\n");

                    VmAssistant = new VmInterface(m);
                    SkyAssert(VmAssistant);

                    // 创建定时器或者停止定时器
                    if (INVALID_TIMER_ID == _usb_query_timer_id)
                    {
                        if (INVALID_TIMER_ID == (_usb_query_timer_id = m->CreateTimer()))
                        {
                            OutPut(SYS_EMERGENCY, "VMManager: CreateTimer failed!\n");
                            SkyExit(-1, "VMManager::MessageEntry: CreateTimer failed!");
                            SkyAssert(0);
                        }
                    }
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
                //部署虚拟机
                case EV_VM_DEPLOY:
                {
                    DeployVM(message,oss);
                    break;
                }

                case EV_VM_OP:
                {
                    OperateVM(message,oss);
                    break;
                }

                case EV_VM_MIGRATE:
                {
                    OperateVM(message,oss);
                    break;
                }

                case EV_VM_QUERY:
                {
                    GetRegisteredSystem(tc);
                    if(message.option.sender()._application != tc)
                    {
                        Debug(SYS_WARNING,"vm_manager receive query request from unknown tc!\n");
                        break;
                    }
                    VmQueryReq req;
                    SkyAssert(true == req.deserialize(message.data));
                    ret = QueryVM(req,oss);
                    if(SUCCESS != ret)
                    {
                        VmQueryAck ack(ret,oss.str(),req._stamp);
                        MID receiver;
                        receiver.deserialize(req._stamp);
                        MessageOption option(receiver,EV_VM_QUERY_ACK,0,0);
                        m->Send(ack,option);
                    }
                    break;
                }

                case EV_VMCFG_SYNC_REQ:
                {
                    SyncVmCfg(message);
                    break;
                }
                case EV_VMCFG_USB_INFO_QUERY:
                {
                    DealVmUsbInfoQueryFromApi(message);
                    break;
                }
                case EV_VMCFG_USB_INFO_QUERY_ACK:
                {
                    DealVmUsbInfoQueryFromHc(message);
                    break;
                }

                case EV_VM_SCHEDULE_SINGLE:
                {//SCH的调度应答消息
                    HandleSelectHostAck(message);
                    break;
                }

                case EV_VM_RESTORE_REQ:
                {
                    DealVmRestoreReq(message);
                    break;
                }

                case EV_VM_RESTORE_ACK:
                {
                    DealVmRestoreAck(message);
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

STATUS VMManager::DeployVM(const MessageFrame& message, ostringstream& error)
{
    MID sender = message.option.sender();
    GetRegisteredSystem(tc);
    if(sender._application != tc)
    {
        Debug(SYS_WARNING,"vm_manager receive deploy info from unknown tc!\n");
        return ERROR;
    }

    VMDeployInfo deploy;
    if(false == deploy.deserialize(message.data))
    {
        Debug(SYS_ERROR,"vm_manager failed to deserialize deploy info\n");
        SkyAssert(false);
        return ERROR;
    }

    Debug(SYS_DEBUG,"Cluster will deploy vm %lld, stamp=%s.\n",
                deploy._config._vid,
                deploy._stamp.c_str());

    string wf_id;
    STATUS ret;
    int progress;

    if (SQLDB_OK == vm_pool->GetVmWorkFlowId(deploy._config._vid, VM_WF_DEPLOY, wf_id))
    {//是否有正向流
        progress = GetWorkflowProgress(wf_id);
        ret = (progress == 100) ? SUCCESS : ERROR_ACTION_RUNNING;
        error << "VM " << deploy._config._vid << " is deploying.";
    }
    else if (SQLDB_OK == vm_pool->GetVmWorkFlowId(deploy._config._vid, VM_WF_CANCEL, wf_id))
    {//反响或者回滚流
        if (IsRollbackWorkflow(wf_id))
        {//回滚流
            progress = GetWorkflowProgress(wf_id);
            ret = (progress == 100) ? SUCCESS : ERROR_ACTION_RUNNING;
            error << "The action for deploy VM " << deploy._config._vid << " is rollbacking.";
        }
        else
        {//反向流
            ret = ERROR_PERMISSION_DENIED;
            progress = GetWorkflowProgress(wf_id);
            error << "VM " << deploy._config._vid << " is canceling.";
        }
    }
    else if (SQLDB_OK == vm_pool->IsVmExist(deploy._config._vid))
    {//vm_pool里已有记录
        ret = SUCCESS;
        progress = 100;
        error << "vm " << deploy._config._vid << " is already deployed in cluster!";
    }
    else
    {
        if ((!deploy.action_id.empty()) && (IsUuidDeleted(deploy.action_id)))
        {
            progress = 0;
            return ERROR_DUPLICATED_OP;
        }

        ret = DeployVM(deploy, sender, error);
        if (ret == SUCCESS)
        {//工作流增加成功了，才给TC返回DOING，否则返回错误信息
            ret = ERROR_ACTION_RUNNING;
            progress = 0;
            error << "VM " << deploy._config._vid << " is deploying.";

            if ((!deploy.action_id.empty()) && (!IsUuidDeleted(deploy.action_id)))
            {
                DeleteUuid(deploy.action_id);
            }
        }
        else
        {
            progress = 100;
        }
    }

    Debug(SYS_DEBUG,"%s\n",error.str().c_str());

    VmOperationAck ack(deploy.action_id, ret, progress, error.str(), deploy._config._vid, DEPLOY);
    return SendVmOperationAck(m, ack, sender);
}

/**********************************************************************
* 函数名称：VMManager::DeployVM
* 功能描述：本函数负责对TC发来的虚拟机部署操作做初步的合法性检查过滤，
            然后将处理结果转发给虚拟机调度器模块执行
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
STATUS VMManager::DeployVM(VMDeployInfo& deploy, const MID &sender, ostringstream& error)
{
    int64 vid = deploy._config._vid;

    PrepareDeployVMConfig(deploy._config);

    //todo: 这里要区分批量部署和单个部署，采用不同的消息发送给调度器

    MID LCM = MID(MU_VM_LIFECYCLE_MANAGER);
    MID SCH = MID(MU_VM_SCHEDULER);
    const string cc_name = ApplicationName();
    MID IA(cc_name, PROC_IMAGE_AGENT, MU_IMAGE_AGENT);
    MID SC = GetSC_MID();

    VmWorkFlow  wf_deploy;
    wf_deploy.category = VM_WF_CATEGORY;
    wf_deploy._vid = vid;
    wf_deploy.name = VM_WF_DEPLOY;
    wf_deploy._operation = DEPLOY;
    wf_deploy.upstream_sender = sender;
    wf_deploy.upstream_action_id = deploy.action_id;
    if(!deploy._user.empty())
    {
        wf_deploy.originator = deploy._user;
    }

    VmWorkFlow  wf_rollback;
    wf_rollback.category = VM_WF_CATEGORY;
    wf_rollback._vid = vid;
    wf_rollback.name = VM_WF_CANCEL;
    wf_rollback._operation = CANCEL;
    wf_rollback.upstream_sender = sender;
    wf_rollback.upstream_action_id = deploy.action_id;
    if(!deploy._user.empty())
    {
        wf_rollback.originator = deploy._user;
    }

    //构造正向之前先构造回滚
    VmAction unselect_host(VM_WF_CATEGORY, VM_ACT_UNSELECT_HC, EV_VM_UNSELECT_HOST, LCM, LCM);
    unselect_host._vid = vid;
    VmOperationReq req;
    req._vid = vid;
    req.action_id = unselect_host.get_id();
    unselect_host.message_req = req.serialize();
    unselect_host.success_feedback  = FEEDBACK_NULL;
    unselect_host.failure_feedback  = FEEDBACK_NULL;
    unselect_host.skipped           = true;
    wf_rollback.Add(&unselect_host);

    //正向
    VmAction select_host(VM_WF_CATEGORY, VM_ACT_SELECT_HC, EV_VM_SCHEDULE_SINGLE, SCH, SCH);
    select_host._vid                = vid;
    select_host.rollback_action_id  = unselect_host.get_id();
    deploy.action_id                = select_host.get_id();
    select_host.message_req         = deploy.serialize();
    select_host.failure_feedback    = FEEDBACK_ROLLBACK_WORKFLOW;
    select_host.progress_weight     = 20;
    wf_deploy.Add(&select_host);

    //回滚
    VmAction remove_vm(VM_WF_CATEGORY, VM_ACT_REMOVE, EV_VM_OP, LCM, MID(MU_VM_HANDLER));
    remove_vm._vid = vid;
        deploy.action_id        = remove_vm.get_id();
        deploy.retry            = true;
    remove_vm.message_req       = deploy.serialize();
    remove_vm.skipped           = true;
    wf_rollback.Add(&remove_vm, &unselect_host);

    //创建一个消息，由LCM重新生成发给hc的、创建虚拟机的数据
    VmAction modify(VM_WF_CATEGORY, VM_ACT_GENERATE_CREATE_REQUEST, EV_VM_MODIFY_CREATE_VM, LCM, LCM);
    modify._vid                 = vid;
    deploy.action_id            = modify.get_id();
    modify.message_req          = deploy.serialize();
    modify.progress_weight      = 10;
    wf_deploy.Add(&select_host, &modify);

    //正向
    VmAction create_vm(VM_WF_CATEGORY, VM_ACT_CREATE, EV_VM_OP, LCM, MID(MU_VM_HANDLER));
    create_vm._vid                  = vid;
        deploy.action_id            = create_vm.get_id();
        deploy.retry                = false;
    create_vm.message_req       = deploy.serialize();
    create_vm.rollback_action_id    = remove_vm.get_id();
    create_vm.success_feedback = FEEDBACK_END_VMOP;
    create_vm.failure_feedback = FEEDBACK_ROLLBACK_WORKFLOW;
    create_vm.progress_weight  = 70;
    wf_deploy.Add(&modify, &create_vm);

    //_____________________________________________________
    //添加网络的工作流
    if (IsNeedVNet(deploy._config))
    {//虚拟机配置了网卡的时候，才需要增加下面的action
        //下面这个是调度ACTION，没有回滚
        VmAction select_host_by_nic(VM_WF_CATEGORY, VM_ACT_SELECT_HC_BY_NIC, EV_VNETLIB_SELECT_HC, LCM, MID(MU_VNET_LIBNET));
        select_host_by_nic._vid                 = vid;
        CVNetSelectHCListMsg    select_hc_req;
        select_hc_req.action_id         = select_host_by_nic.get_id();
        select_hc_req.m_vm_id           = vid;
        select_hc_req.m_project_id      = deploy._project_id;
        select_hc_req.m_cc_application  = cc_name;
        select_hc_req.m_WatchDogInfo = CWatchDogInfo(deploy._config._vm_wdtime != 0, deploy._config._vm_wdtime);
        select_hc_req.m_VmNicList.assign(deploy._config._nics.begin(), deploy._config._nics.end());
        select_host_by_nic.message_req = select_hc_req.serialize();
        wf_deploy.Add(&select_host_by_nic, &select_host);

        //回滚
        VmAction release_nic(VM_WF_CATEGORY, VM_ACT_RELEASE_NIC, EV_VNETLIB_FREERESOURCE_HC, LCM, MID(MU_VNET_LIBNET));
        release_nic._vid                 = vid;
        CVNetFreeResourceHCMsg    release_nic_req;
        release_nic_req.action_id         = release_nic.get_id();
        release_nic_req.m_vm_id           = vid;
        release_nic_req.m_project_id      = deploy._project_id;
        release_nic_req.m_cc_application  = cc_name;
        release_nic_req.m_WatchDogInfo = CWatchDogInfo(deploy._config._vm_wdtime != 0, deploy._config._vm_wdtime);
        release_nic_req.m_VmNicCfgList.assign(deploy._config._nics.begin(), deploy._config._nics.end());
        release_nic_req.retry            = true;
        release_nic.message_req = release_nic_req.serialize();
        release_nic.skipped          = true;
        wf_rollback.Add(&remove_vm, &release_nic);
        wf_rollback.Add(&release_nic, &unselect_host);

        //正向
        VmAction prepare_nic(VM_WF_CATEGORY, VM_ACT_PREPARE_NIC, EV_VNETLIB_GETRESOURCE_HC, LCM, MID(MU_VNET_LIBNET));
        prepare_nic._vid                 = vid;
        CVNetGetResourceHCMsg    prepare_nic_req;
        prepare_nic_req.action_id         = prepare_nic.get_id();
        prepare_nic_req.m_vm_id           = vid;
        prepare_nic_req.m_project_id      = deploy._project_id;
        prepare_nic_req.m_cc_application  = cc_name;
        prepare_nic_req.m_WatchDogInfo = CWatchDogInfo(deploy._config._vm_wdtime != 0, deploy._config._vm_wdtime);
        prepare_nic_req.m_VmNicCfgList.assign(deploy._config._nics.begin(), deploy._config._nics.end());
        prepare_nic.message_req = prepare_nic_req.serialize();
        prepare_nic.rollback_action_id = release_nic.get_id();
        prepare_nic.failure_feedback = FEEDBACK_ROLLBACK_WORKFLOW;
        wf_deploy.Add(&select_host, &prepare_nic);
        wf_deploy.Add(&prepare_nic, &create_vm);
    }

    //添加存储的工作流
    vector<VmDiskConfig> disks = deploy._config._disks;
    disks.push_back(deploy._config._machine);
    //disks.push_back(deploy._config._kernel);
    //disks.push_back(deploy._config._initrd);

    ImageStoreOption option;
    GetVstorageOption(option);
    vector<VmDiskConfig>::iterator it;
    for (it = disks.begin(); it != disks.end(); it++)
    {
        if (it->_id != INVALID_FILEID)
        {// image
            //image prepare action
            ImageUrlPrepareReq prepare_image_req;
            prepare_image_req._image_id = it->_id;
            prepare_image_req._position = it->_position;
            prepare_image_req._request_id = it->_request_id;
            prepare_image_req._cluster_name = cc_name;
            prepare_image_req._url = it->_url;
            prepare_image_req._option = option;

            VmAction prepare_image(IMAGE_CATEGORY, VM_ACT_PREPARE_IMAGE, EV_IMAGE_URL_PREPARE_REQ, LCM, IA);
            prepare_image._vid = vid;
            prepare_image_req.action_id = prepare_image.get_id();
            prepare_image.message_req = prepare_image_req.serialize();
            prepare_image.failure_feedback = FEEDBACK_ROLLBACK_WORKFLOW;

            if (IMAGE_POSITION_LOCAL == it->_position)
            {
                prepare_image.success_feedback = FEEDBACK_UPDATE_VMLOCALIMAGE;
                wf_rollback.Add(&remove_vm, &unselect_host);

                wf_deploy.Add(&select_host, &prepare_image);
                wf_deploy.Add(&prepare_image, &modify);
            }
            else
            {

                if (IMAGE_USE_SNAPSHOT == option._share_img_usage)
                {

                    prepare_image.success_feedback = FEEDBACK_MODIFY_CREATESNAPSHOT;
                    //image snapshot create
                    CreateSnapshotReq create_snapshot_req;
                    create_snapshot_req._request_id = it->_request_id;

                    DeleteSnapshotReq delete_snapshot_req;
                    delete_snapshot_req._uid = -1;
                    delete_snapshot_req._request_id = it->_request_id;

                    VmAction delete_snapshot(STORAGE_CATEGORY,DELETE_SNAPSHOT_BLOCK,EV_STORAGE_DELETE_SNAPSHOT_REQ,LCM,SC);
                    delete_snapshot._vid = vid;
                    delete_snapshot_req.retry = true;
                    delete_snapshot_req.action_id = delete_snapshot.get_id();
                    delete_snapshot.message_req = delete_snapshot_req.serialize();

                    VmAction create_snapshot(STORAGE_CATEGORY,CREATE_SNAPSHOT_BLOCK,EV_STORAGE_CREATE_SNAPSHOT_REQ,LCM,SC);
                    create_snapshot._vid = vid;
                    create_snapshot_req.action_id = create_snapshot.get_id();
                    create_snapshot.message_req = create_snapshot_req.serialize();
                    create_snapshot.failure_feedback = FEEDBACK_ROLLBACK_WORKFLOW;
                    create_snapshot.rollback_action_id = delete_snapshot.get_id();

                    //image snapshot auth
                    AuthBlockReq auth_snapshot_req;
                    DeAuthBlockReq deauth_snapshot_req;
                    auth_snapshot_req._vid           = deauth_snapshot_req._vid         = vid;
                    auth_snapshot_req._request_id    = deauth_snapshot_req._request_id  = it->_request_id;
                    auth_snapshot_req._cluster_name  = deauth_snapshot_req._cluster_name = cc_name;
                    auth_snapshot_req._usage  = deauth_snapshot_req._usage = to_string<int64>(vid,dec)+":"+it->_target;

                    VmAction deauth_snapshot(STORAGE_CATEGORY, VM_ACT_DEAUTH_BLOCK, EV_STORAGE_DEAUTH_BLOCK_REQ, LCM, SC);
                    deauth_snapshot._vid = vid;
                    deauth_snapshot_req.retry = true;
                    deauth_snapshot_req.action_id = deauth_snapshot.get_id();
                    deauth_snapshot.message_req = deauth_snapshot_req.serialize();

                    VmAction auth_snapshot(STORAGE_CATEGORY, VM_ACT_AUTH_BLOCK, EV_STORAGE_AUTH_BLOCK_REQ, LCM, SC);
                    auth_snapshot._vid = vid;
                    auth_snapshot_req.action_id = auth_snapshot.get_id();
                    auth_snapshot.message_req = auth_snapshot_req.serialize();
                    auth_snapshot.rollback_action_id = deauth_snapshot.get_id();
                    auth_snapshot.success_feedback = FEEDBACK_UPDATE_VMSHAREIMAGE;
                    auth_snapshot.failure_feedback = FEEDBACK_ROLLBACK_WORKFLOW;

                    wf_rollback.Add(&remove_vm, &deauth_snapshot);
                    wf_rollback.Add(&deauth_snapshot, &delete_snapshot);
                    wf_rollback.Add(&delete_snapshot, &unselect_host);

                    wf_deploy.Add(&select_host, &prepare_image);
                    wf_deploy.Add(&prepare_image, &create_snapshot);
                    wf_deploy.Add(&create_snapshot, &auth_snapshot);
                    wf_deploy.Add(&auth_snapshot, &modify);
                }
                else
                {

                    prepare_image.success_feedback = FEEDBACK_UPDATE_VMLOCALIMAGE;
                    //image block create
                    CreateBlockReq create_block_req;
                    create_block_req._request_id = it->_request_id;
                    create_block_req._cluster_name = cc_name;
                    create_block_req._volsize = CalcDiskRealSize(*it);

                    DeleteBlockReq delete_block_req;
                    delete_block_req._uid = -1;
                    delete_block_req._request_id = it->_request_id;

                    VmAction delete_block(STORAGE_CATEGORY,DELETE_BLOCK,EV_STORAGE_DELETE_BLOCK_REQ,LCM,SC);
                    delete_block._vid = vid;
                    delete_block_req.action_id = delete_block.get_id();
                    delete_block_req.retry = true;
                    delete_block.message_req = delete_block_req.serialize();

                    VmAction create_block(STORAGE_CATEGORY,CREATE_BLOCK,EV_STORAGE_CREATE_BLOCK_REQ,LCM,SC);
                    create_block._vid = vid;
                    create_block_req.action_id = create_block.get_id();
                    create_block.message_req = create_block_req.serialize();
                    create_block.failure_feedback = FEEDBACK_ROLLBACK_WORKFLOW;
                    create_block.rollback_action_id = delete_block.get_id();

                    //image block auth
                    AuthBlockReq auth_block_req;
                    DeAuthBlockReq deauth_block_req;
                    auth_block_req._vid           = deauth_block_req._vid         = vid;
                    auth_block_req._request_id    = deauth_block_req._request_id  = it->_request_id;
                    auth_block_req._cluster_name  = deauth_block_req._cluster_name = cc_name;
                    auth_block_req._usage  = deauth_block_req._usage = to_string<int64>(vid,dec)+":"+it->_target;

                    VmAction deauth_block(STORAGE_CATEGORY, VM_ACT_DEAUTH_BLOCK, EV_STORAGE_DEAUTH_BLOCK_REQ, LCM, SC);
                    deauth_block._vid = vid;
                    deauth_block_req.action_id = deauth_block.get_id();
                    deauth_block_req.retry = true;
                    deauth_block.message_req = deauth_block_req.serialize();

                    VmAction auth_block(STORAGE_CATEGORY, VM_ACT_AUTH_BLOCK, EV_STORAGE_AUTH_BLOCK_REQ, LCM, SC);
                    auth_block._vid = vid;
                    auth_block_req.action_id = auth_block.get_id();
                    auth_block.message_req = auth_block_req.serialize();
                    auth_block.rollback_action_id = deauth_block.get_id();
                    auth_block.success_feedback = FEEDBACK_UPDATE_VMSHAREIMAGE;
                    auth_block.failure_feedback = FEEDBACK_ROLLBACK_WORKFLOW;

                    wf_rollback.Add(&remove_vm, &deauth_block);
                    wf_rollback.Add(&deauth_block, &delete_block);
                    wf_rollback.Add(&delete_block, &unselect_host);

                    wf_deploy.Add(&select_host, &prepare_image);
                    wf_deploy.Add(&prepare_image, &create_block);
                    wf_deploy.Add(&create_block, &auth_block);
                    wf_deploy.Add(&auth_block, &modify);
                }
            }

        }
        else if (it->_position == VM_STORAGE_TO_SHARE)
        {//使用共享磁盘
            CreateBlockReq create_req;
            DeleteBlockReq delete_req;
            create_req._volsize     = it->_size;
            create_req._cluster_name   = cc_name;
            create_req._request_id  = delete_req._request_id = it->_request_id;
            delete_req._uid = -1;
            create_req._uid = -1;

            AuthBlockReq auth_req;
            DeAuthBlockReq deauth_req;
            auth_req._vid           = deauth_req._vid         = vid;
            auth_req._request_id    = deauth_req._request_id  = it->_request_id;
            auth_req._cluster_name  = deauth_req._cluster_name = cc_name;
            auth_req._usage  = deauth_req._usage = to_string<int64>(vid,dec)+":"+it->_target;

            VmAction deauth_share_block(STORAGE_CATEGORY, VM_ACT_DEAUTH_BLOCK, EV_STORAGE_DEAUTH_BLOCK_REQ, LCM, SC);
            deauth_share_block._vid = vid;
            deauth_req.action_id = deauth_share_block.get_id();
            deauth_req.retry = true;
            deauth_share_block.message_req = deauth_req.serialize();
            deauth_share_block.skipped = true;
            wf_rollback.Add(&remove_vm, &deauth_share_block);

            VmAction delete_share_block(STORAGE_CATEGORY, VM_ACT_DELETE_BLOCK, EV_STORAGE_DELETE_BLOCK_REQ, LCM, SC);
            delete_share_block._vid = vid;
            delete_req.action_id = delete_share_block.get_id();
            delete_req.retry = true;
            delete_share_block.message_req = delete_req.serialize();
            delete_share_block.skipped          = true;
            wf_rollback.Add(&deauth_share_block, &delete_share_block);
            wf_rollback.Add(&delete_share_block, &unselect_host);

            VmAction create_share_block(STORAGE_CATEGORY, VM_ACT_CREATE_BLOCK, EV_STORAGE_CREATE_BLOCK_REQ, LCM, SC);
            create_share_block._vid = vid;
            create_req.action_id = create_share_block.get_id();
            create_share_block.message_req = create_req.serialize();
            create_share_block.failure_feedback = FEEDBACK_ROLLBACK_WORKFLOW;
            create_share_block.rollback_action_id = delete_share_block.get_id();
            wf_deploy.Add(&select_host, &create_share_block);

            VmAction auth_share_block(STORAGE_CATEGORY, VM_ACT_AUTH_BLOCK, EV_STORAGE_AUTH_BLOCK_REQ, LCM, SC);
            auth_share_block._vid = vid;
            auth_req.action_id = auth_share_block.get_id();
            auth_share_block.message_req = auth_req.serialize();
            auth_share_block.rollback_action_id = deauth_share_block.get_id();
            auth_share_block.success_feedback = FEEDBACK_UPDATE_VMSHAREBLOCK;
            auth_share_block.failure_feedback = FEEDBACK_ROLLBACK_WORKFLOW;
            wf_deploy.Add(&create_share_block, &auth_share_block);
            wf_deploy.Add(&auth_share_block, &modify);
        }
    }

    STATUS ret = CreateWorkflow(wf_deploy, wf_rollback);
    if(SUCCESS != ret)
    {
        Debug(SYS_WARNING, "interface failed to created workflow: %s!!!\n", select_host.get_id().c_str());
        SkyAssert(false);
    }

    return ret;
}

STATUS VMManager::OperateVM(const MessageFrame& message, ostringstream& error)
{
    MID sender = message.option.sender();

    VmOperationReq req;
    if(false == req.deserialize(message.data))
    {
        Debug(SYS_ERROR,"vm_manager failed to deserialize VmOperationReq info\n");
        SkyAssert(false);
        return ERROR;
    }

    if (req._operation == MODIFY_VMCFG)
    {
        return ModifyVmCfg(message);
    }

    STATUS ret;
    int progress;
    ret = OperateVM(req, sender, message.data, progress, error);

    VmOperationAck ack(req.action_id, ret, progress, error.str(), req._vid, req._operation);
    if (ret == ERROR_ACTION_RUNNING)
    {//如果正在执行，那就返回正在执行的工作流的ID
        vm_pool->GetVmWorkFlowId(req._vid, ack._workflow_id);
        GetWorkflowEngine(ack._workflow_id,ack._workflow_engine);
        if ((!req.action_id.empty()) && (!IsUuidDeleted(req.action_id)))
        {
            DeleteUuid(req.action_id);
        }
    }
    return SendVmOperationAck(m, ack, sender);
}

/**********************************************************************
* 函数名称：VMManager::OperateVM
* 功能描述：本函数负责对TC发来的虚拟机操作做初步的合法性检查过滤，
            然后将处理结果转发给生命周期管理模块执行
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
STATUS VMManager::OperateVM(const VmOperationReq& req, MID sender, const string &req_info, int &progress, ostringstream& error)
{
    //虚拟机不存在时的处理
    if(vm_pool->IsVmExist(req._vid) == SQLDB_RESULT_EMPTY)
    {
        if(req._operation == CANCEL)
        {//虚拟机刚刚增加了部署流程，但还没插入vm_pool时，此时来了cancel命令
            return OperateCancelVM(req, sender, progress, error);
        }
        else
        {
            //其它操作都必须是针对已经存在的vm，否则返回失败
            error << "vm_manager find no vm(id=" <<req._vid<< ") in this cluster pool!";
            Debug(SYS_WARNING,"%s\n",error.str().c_str());
            progress = 0;
            return ERR_VM_NOT_EXIST;
        }
    }

    VirtualMachine *pvm = vm_pool->Get(req._vid, false);
    if(!pvm)
    {
        error << "vm_manager get vm(id=" <<req._vid<< ") object failed!";
        Debug(SYS_ERROR,"%s\n",error.str().c_str());
        progress = 0;
        return ERR_OBJECT_GET_FAILED;
    }

    string wf_id_now;
    VmWorkFlow wf_now;
    STATUS ret = vm_pool->GetVmWorkFlowId(req._vid, wf_id_now);
    if (ret == SUCCESS)
    {
        ret = GetWorkflowLabels(wf_id_now,wf_now.labels);
    }

    if (ret == SUCCESS)
    {//当前有虚拟机操作的工作流

        //对该虚拟机的上一个同样的操作正在进行中时，不允许重复发起该操作
        if(wf_now._operation == (VmOperation)req._operation)
        {
            error << "A same operation is already executing! no more operation "
                  << Enum2Str::VmOpStr(req._operation)
                  << " on vm "
                  <<req._vid << "!";
            Debug(SYS_NOTICE,"%s\n",error.str().c_str());
            progress = GetWorkflowProgress(wf_id_now);
            return ERROR_ACTION_RUNNING;
        }

        if (!LegalOperation((VmOperation)req._operation, (VmOperation)wf_now._operation, ERROR_ACTION_RUNNING))
        {
            error << "illegal operation " << Enum2Str::VmOpStr(req._operation)
                << " when last_op = " << Enum2Str::VmOpStr(wf_now._operation)
                << "is doing";
            Debug(SYS_DEBUG,"%s\n",error.str().c_str());
            progress = 0;
            return ERROR_PERMISSION_DENIED;
        }
    }

    //虚拟机存在，但是处于不同的状态时允许的操作也不同
    if (!LegalOperationforLogState((VmOperation)req._operation,(VmInstanceState)pvm->_log_state))
    {//冻结状态，只允许解冻，关机操作
        error << "illegal operation " << Enum2Str::VmOpStr(req._operation)
            << " when state = " << Enum2Str::InstStateStr(pvm->_log_state);
        Debug(SYS_DEBUG,"%s\n",error.str().c_str());
        progress = 0;
        return ERROR_PERMISSION_DENIED;
    }

    if(false == LegalOperation((VmOperation)req._operation,(VmInstanceState)pvm->_state,(LcmState)pvm->_lcm_state))
    {
        error << "illegal operation " << Enum2Str::VmOpStr(req._operation)
            << " when state = " << Enum2Str::InstStateStr(pvm->_state)
            << " lcm_state = " << Enum2Str::LcmStateStr(pvm->_lcm_state);
        Debug(SYS_DEBUG,"%s\n",error.str().c_str());
        progress = 0;
        return ERROR_PERMISSION_DENIED;
    }

    //上面的检测都通过后，下面就要创建对应的工作流了

    if ((!req.action_id.empty()) && (IsUuidDeleted(req.action_id)))
    {
        progress = 0;
        return ERROR_DUPLICATED_OP;
    }

    ret = DoOperation(req, sender, req_info, progress, error);
    return ret;
}

STATUS VMManager::DoOperation(const VmOperationReq& req, MID sender, const string &req_info, int &progress, ostringstream& error)
{
    SetExpectedState(req);

    progress = 0;

    switch (req._operation)
    {
        case CREATE:
        case START:
        {
            return OperateStartVM(req, sender, error);
            break;
        }

        case CANCEL:
        {
            return OperateCancelVM(req, sender, progress, error);
        }

        case LIVE_MIGRATE:
        {
            return MigrateVM(req, req_info, sender, error);
        }

        case PLUG_IN_USB:
        {
            return OperatePluginUSB(req, sender, req_info, error);
        }

        case PLUG_OUT_USB:
        case OUT_IN_USB:
        {
            return OperatePlugoutUSB(req, sender, req_info, error);
        }

        case PLUG_IN_PDISK:
        {
            return OperateAttachPdisk(req, sender, req_info, progress, error);
        }
        case PLUG_OUT_PDISK:
        {
            return OperateDetachPdisk(req, sender, req_info, progress, error);
        }
        case RECOVER_IMAGE:
        {
            return OperateRecoverImage(req, sender, error);
        }
        case MODIFY_IMAGE_BACKUP:
        {
            return OperateModifyImageBackup(req_info, error);
        }
        case UPLOAD_IMG:
        {
            VmSaveReq savereq;
            bool result = savereq.deserialize(req_info);
            SkyAssert(result);
            return DoCommonOp(savereq, sender,FEEDBACK_END_VMOP,FEEDBACK_DELETE_WORKFLOW);
        }

        case CREATE_IMAGE_BACKUP:
        {
             return OperateBackupImg(req, sender, req_info, progress, error);
        }

	 case DELETE_IMAGE_BACKUP:
        {
             return OperateDeleteBackupImg(req, sender, req_info, progress, error);
        }

        case RESTORE_IMAGE_BACKUP:
        {
            return OperateRestoreVM(req,req_info, sender, progress,error);
        }

#if 0
        case DETACH_PORTABLE_DISK:
        {
            return OperatePortableDisk(req, sender, req_info, error);
        }
#endif

        case COLD_MIGRATE:
        {
            return ColdMigrate(req, req_info, sender, error);
        }

        case FREEZE:
        {
            return Freeze(req, sender, error);
        }

        case UNFREEZE:
        {
            return UnFreeze(req, sender, error);
        }
        case REBOOT:
        {
            return DoCommonOp(*(VmOperationReq*)(&req), sender,FEEDBACK_END_VMOP, FEEDBACK_DELETE_WORKFLOW);
        }
        case STOP:
        case PAUSE:
        case RESUME:
        case RESET:
        case DESTROY:
        case CANCEL_UPLOAD_IMG:
        {
            return DoCommonOp(*(VmOperationReq*)(&req), sender, FEEDBACK_END_VMOP, FEEDBACK_DELETE_WORKFLOW);
        }

        default:
        {
            SkyAssert(false);
            return DoCommonOp(*(VmOperationReq*)(&req), sender);
        }
    }
}

STATUS VMManager::AttachPdisk(VmPortableDiskOp& req, const MID& sender, ostringstream& error)
{
    string wf_id;
    STATUS ret;
    MID LCM = MID(MU_VM_LIFECYCLE_MANAGER);
    MID SC = GetSC_MID();

    VmWorkFlow  wf;
    wf.category = VM_WF_CATEGORY;
    wf._vid = req._vid;
    wf.name = VM_WF_PLUG_IN_PDISK;
    wf._operation = PLUG_IN_PDISK;
    wf.upstream_sender = sender;
    wf.upstream_action_id = req.action_id;
    if(!req._user.empty())
    {
        wf.originator = req._user;
    }

    VmWorkFlow  wf_rollback;
    wf_rollback.category = VM_WF_CATEGORY;
    wf_rollback._vid = req._vid;
    wf_rollback.name = VM_WF_PLUG_OUT_PDISK;
    wf_rollback._operation = PLUG_OUT_PDISK;
    wf_rollback.upstream_sender = sender;
    wf_rollback.upstream_action_id = req.action_id;
    if(!req._user.empty())
    {
        wf_rollback.originator = req._user;
    }

    VirtualMachine *pvm = vm_pool->Get(req._vid, false);
    SkyAssert(pvm);
    string hostname = host_pool->GetHostNameById(pvm->_hid);
    SkyAssert(false == hostname.empty());
    string ip = host_pool->GetHostIpById(pvm->_hid);
    SkyAssert(false == ip.empty());
    string iscsi = host_pool->GetIscsinameById(pvm->_hid);
    SkyAssert(false == iscsi.empty());

    AuthBlockReq auth_req;
    DeAuthBlockReq deauth_req;
    auth_req._vid           = deauth_req._vid         = req._vid;
    auth_req._request_id    = deauth_req._request_id  = req._request_id;
    auth_req._cluster_name  = deauth_req._cluster_name = ApplicationName();
    auth_req._userip = deauth_req._userip = ip;
    auth_req._iscsiname = deauth_req._iscsiname = iscsi;
    auth_req._usage  = deauth_req._usage = to_string<int64>(req._vid,dec)+":"+req._target;

    VmAction deauth_share_block(STORAGE_CATEGORY, VM_ACT_DEAUTH_BLOCK, EV_STORAGE_DEAUTH_BLOCK_REQ, LCM, SC);
    deauth_req.action_id = deauth_share_block.get_id();
    deauth_req.retry = true;
    deauth_share_block._vid = req._vid;
    deauth_share_block.message_req = deauth_req.serialize();
    deauth_share_block.success_feedback=FEEDBACK_PLUGOUT_PDISK;

    VmAction auth_share_block(STORAGE_CATEGORY, VM_ACT_AUTH_BLOCK, EV_STORAGE_AUTH_BLOCK_REQ, LCM, SC);
    auth_req.action_id = auth_share_block.get_id();
    auth_share_block._vid = req._vid;
    auth_share_block.message_req = auth_req.serialize();
    auth_share_block.rollback_action_id = deauth_share_block.get_id();
    auth_share_block.success_feedback = FEEDBACK_MODIYF_PDISKREQ;
    auth_share_block.failure_feedback = FEEDBACK_ROLLBACK_WORKFLOW;

    MID HC(hostname, PROC_HC, MU_VM_HANDLER);
    VmAction deattach_pdisk(VM_WF_CATEGORY, VM_ACT_PLUG_OUT_PDISK, EV_VM_OP, LCM, HC);
    deattach_pdisk._vid = req._vid;
    req.action_id = deattach_pdisk.get_id();
    req.retry = true;
    deattach_pdisk._vid = req._vid;
    deattach_pdisk.message_req = req.serialize();
    deattach_pdisk.success_feedback = FEEDBACK_END_VMOP;

    VmAction attach_pdisk(VM_WF_CATEGORY, VM_ACT_PLUG_IN_PDISK, EV_VM_OP, LCM, HC);
    attach_pdisk._vid = req._vid;
    req.action_id = attach_pdisk.get_id();
    req.retry = false;
    attach_pdisk._vid = req._vid;
    attach_pdisk.message_req = req.serialize();
    attach_pdisk.rollback_action_id=deattach_pdisk.get_id();
    attach_pdisk.success_feedback = FEEDBACK_PLUGIN_PDISK;
    attach_pdisk.failure_feedback = FEEDBACK_ROLLBACK_WORKFLOW;

    wf.Add(&auth_share_block, &attach_pdisk);
    wf_rollback.Add(&deattach_pdisk,&deauth_share_block);

    ret = CreateWorkflow(wf, wf_rollback);
    if(SUCCESS != ret)
    {
        Debug(SYS_WARNING, "AttachPdisk failed to created workflow: !!!\n" );
        SkyAssert(false);
    }
    return ret;
}

STATUS VMManager::OperateAttachPdisk(const VmOperationReq& req, const MID& sender, const string &req_info, int& progress, ostringstream& error)
{
    VmPortableDiskOp pdisk_req;
    bool result = pdisk_req.deserialize(req_info);
    SkyAssert(result);
    string wf_id;
    STATUS ret = ERROR;
    VMImagePool *image_pool = VMImagePool::GetInstance();
    SkyAssert(image_pool);

    if (SQLDB_OK == vm_pool->GetVmWorkFlowId(pdisk_req._vid, VM_WF_PLUG_IN_PDISK, wf_id))
    {//是否有正向流
        ret = ERROR_ACTION_RUNNING;
        progress = GetWorkflowProgress(wf_id);
        error << "VM " << pdisk_req._vid << " is attaching pdisk.";
    }
    else if (SQLDB_OK == vm_pool->GetVmWorkFlowId(pdisk_req._vid, VM_WF_PLUG_OUT_PDISK, wf_id))
    {//反响或者回滚流
        if (IsRollbackWorkflow(wf_id))
        {//回滚流
            ret = ERROR_ACTION_RUNNING;
            progress = GetWorkflowProgress(wf_id);
            error << "The action for deploy VM " << pdisk_req._vid << " is rollbacking.";
        }
        else
        {//反向流
            ret = ERROR_PERMISSION_DENIED;
            progress = GetWorkflowProgress(wf_id);
            error << "VM " << pdisk_req._vid << " is deattach pdisk.";
        }
    }
    else if (image_pool->IsDiskExist(pdisk_req._request_id,pdisk_req._vid))
    {//vm_disk里已有记录
        ret = SUCCESS;
        progress = 100;
        error << "vm " << pdisk_req._vid << " is already attached pdisk";
    }
    else if (image_pool->IsDiskExist(pdisk_req._vid, pdisk_req._target))
    {//同一个vid+target下是否有已经有盘了
        ret = ERROR;
        progress = 0;
        error << "vm " << pdisk_req._vid << " has disk on "<<pdisk_req._target<<" already!";
    }
    else if (image_pool->IsImageExist(pdisk_req._vid, pdisk_req._target))
    {//同一个vid+target下是否有已经有镜像盘了
        ret = ERROR;
        progress = 0;
        error << "vm " << pdisk_req._vid << " has image on "<<pdisk_req._target<<" already!";
    }
    else
    {
        ret = AttachPdisk(pdisk_req, sender, error);
        if (ret == SUCCESS)
        {//工作流增加成功了，才给TC返回DOING，否则返回错误信息
            ret = ERROR_ACTION_RUNNING;
            progress = 0;
            error << "VM " << pdisk_req._vid << " is attaching pdisk.";
        }
        else
        {
            progress = 0;
        }
    }

    return ret;

}

STATUS VMManager::DetachPdisk(VmPortableDiskOp& req, const MID& sender, ostringstream& error)
{
    string wf_id;
    STATUS ret = ERROR;
    MID LCM = MID(MU_VM_LIFECYCLE_MANAGER);
    MID SC = GetSC_MID();
    VmWorkFlow  wf;
    wf.category = VM_WF_CATEGORY;
    wf._vid = req._vid;
    wf.name = VM_WF_PLUG_OUT_PDISK;
    wf._operation = PLUG_OUT_PDISK;
    wf.upstream_sender = sender;
    wf.upstream_action_id = req.action_id;
    if(!req._user.empty())
    {
        wf.originator = req._user;
    }

    VirtualMachine *pvm = vm_pool->Get(req._vid, false);
    SkyAssert(pvm);
    string hostname = host_pool->GetHostNameById(pvm->_hid);
    SkyAssert(false == hostname.empty());
    string ip = host_pool->GetHostIpById(pvm->_hid);
    SkyAssert(false == ip.empty());
    string iscsi = host_pool->GetIscsinameById(pvm->_hid);
    SkyAssert(false == iscsi.empty());

    DeAuthBlockReq deauth_req;
    deauth_req._vid         = req._vid;
    deauth_req._request_id  = req._request_id;
    deauth_req._userip = ip;
    deauth_req._iscsiname = iscsi;
    deauth_req._cluster_name = ApplicationName();
    deauth_req._usage = to_string<int64>(req._vid,dec)+":"+req._target;

    VmAction deauth_share_block(STORAGE_CATEGORY, VM_ACT_DEAUTH_BLOCK, EV_STORAGE_DEAUTH_BLOCK_REQ, LCM, SC);
    deauth_req.action_id = deauth_share_block.get_id();
    deauth_req.retry = true;
    deauth_share_block._vid = req._vid;
    deauth_share_block.message_req = deauth_req.serialize();
    deauth_share_block.success_feedback=FEEDBACK_PLUGOUT_PDISK;

    vector<VmDiskConfig>::iterator it = pvm->_config._disks.begin();
    for (it = pvm->_config._disks.begin(); it != pvm->_config._disks.end(); it++)
    {
        if (it->_request_id == req._request_id)
        {
            break;
        }
    }

    if (it == pvm->_config._disks.end())
    {
        return ERROR_OBJECT_NOT_EXIST;
    }

    VmAttachReq detachreq;
    detachreq._vid=req._vid;
    detachreq._diskcfg._id=INVALID_OID;
    detachreq._diskcfg._position=VM_STORAGE_TO_SHARE;
    detachreq._diskcfg._type=DISK_TYPE_DISK;
    detachreq._diskcfg._bus=it->_bus;
    detachreq._diskcfg._target=it->_target;
    detachreq._operation = PLUG_OUT_PDISK;
    detachreq._stamp = it->_target;
    detachreq._diskcfg._share_url = it->_share_url;
    detachreq._diskcfg._is_need_release = it->_is_need_release;

    MID HC(hostname, PROC_HC, MU_VM_HANDLER);
    VmAction deattach_pdisk(VM_WF_CATEGORY, VM_ACT_PLUG_OUT_PDISK, EV_VM_OP, LCM, HC);
    deattach_pdisk._vid = detachreq._vid;
    detachreq.action_id = deattach_pdisk.get_id();
    detachreq.retry = true;
    deattach_pdisk._vid = req._vid;
    deattach_pdisk.message_req = detachreq.serialize();
    deattach_pdisk.success_feedback = NONE;

    wf.Add(&deattach_pdisk, &deauth_share_block);
    ret = CreateWorkflow(wf);
    if(SUCCESS != ret)
    {
        Debug(SYS_WARNING, "DetachPdisk failed to created workflow: !!!\n" );
        SkyAssert(false);
    }
    return ret;
}

STATUS VMManager::OperateDetachPdisk(const VmOperationReq& req, const MID& sender, const string &req_info, int& progress, ostringstream& error)
{
    VmPortableDiskOp pdisk_req;
    bool result = pdisk_req.deserialize(req_info);
    SkyAssert(result);

    string wf_id;
    STATUS ret = ERROR;
    VMImagePool *image_pool = VMImagePool::GetInstance();
    SkyAssert(image_pool);

    if (SQLDB_OK == vm_pool->GetVmWorkFlowId(pdisk_req._vid, VM_WF_PLUG_IN_PDISK, wf_id))
    {//是否有正向流
        string rollback_id;
        GetRollbackWorkflow(wf_id, rollback_id);
        ret = RollbackWorkflow(wf_id);
        if (ret == SUCCESS)
        {
            UnsetRollbackAttr(rollback_id, sender, pdisk_req.action_id);
            ret = ERROR_ACTION_RUNNING;
            progress = GetWorkflowProgress(rollback_id);
            error << "VM " << pdisk_req._vid << " portable disk " << pdisk_req._request_id << " is detaching after rollback!";
        }
        else
        {
            progress = 0;
            error << "rollback workflow " << wf_id << " failed! Ret = " << ret;
        }
    }
    else if (SQLDB_OK == vm_pool->GetVmWorkFlowId(pdisk_req._vid, VM_WF_PLUG_OUT_PDISK, wf_id))
    {//反响或者回滚流
        if (IsRollbackWorkflow(wf_id))
        {//回滚流
            UnsetRollbackAttr(wf_id, sender, pdisk_req.action_id);

            ret = ERROR_ACTION_RUNNING;
            progress = GetWorkflowProgress(wf_id);
            error << "VM " << pdisk_req._vid << " portable disk " << pdisk_req._request_id << " is rollbacking!";
        }
        else
        {//反向流
            ret = ERROR_ACTION_RUNNING;
            progress = GetWorkflowProgress(wf_id);
            error << "VM " << pdisk_req._vid << " portable disk " << pdisk_req._request_id << " is detaching!";
        }
    }
    else if (image_pool->IsDiskExist(pdisk_req._request_id,pdisk_req._vid))
    {//vm_disk里存在
        progress = 0;
        error << "Begin to detach portdisk " << pdisk_req._request_id <<" from vm " << pdisk_req._vid;
        image_pool->GetDiskTarget(pdisk_req._request_id,pdisk_req._vid,pdisk_req._target);
        ret = DetachPdisk(pdisk_req, sender, error);
        if (ret == SUCCESS)
        {
            ret = ERROR_ACTION_RUNNING;
        }
    }
    else
    {//vm_disk里不存在
        ret = SUCCESS;
        progress = 100;
        error << "Portdisk " << pdisk_req._request_id<< " is already detached.";
    }

    return ret;
}

STATUS VMManager::BackupImgCreate(VmImageBackupOp& req, const MID& sender, ostringstream& error)
{
    string wf_id;
    STATUS ret;
    MID LCM = MID(MU_VM_LIFECYCLE_MANAGER);
    MID SC = GetSC_MID();
    string  request_id;
    /*容量计算*/
    int  rsvnum,usenum;
    VMImagePool *image_pool = VMImagePool::GetInstance();
    SkyAssert(image_pool);
    VirtualMachine *pvm = vm_pool->Get(req._vid, false);
    SkyAssert(pvm);

    image_pool->GetImageRsvNumByVid(req._vid,rsvnum );
    image_pool->GetImageBackupNumByVid(req._vid, usenum);
    HostFree  hfree;
    host_pool->GetHostFree(pvm->_hid, hfree);
    if(usenum>=rsvnum && hfree._disk_free < pvm->_config._machine._size)
    {
        error <<"free space not enough "<<req._target;
        return ERROR_NO_SPACE;
    }

    VmWorkFlow  wf;
    wf.category = VM_WF_CATEGORY;
    wf._vid = req._vid;
    wf.name = VM_WF_IMAGE_BACKUP_CREATE;
    wf._operation = CREATE_IMAGE_BACKUP;
    wf.upstream_sender = sender;
    wf.upstream_action_id = req.action_id;

    VmWorkFlow  wf_rollback;
    wf_rollback.category = VM_WF_CATEGORY;
    wf_rollback._vid = req._vid;
    wf_rollback.name = VM_WF_IMAGE_BACKUP_DELETE;
    wf_rollback._operation = DELETE_IMAGE_BACKUP;
    wf.upstream_sender = sender;
    wf.upstream_action_id = req.action_id;

    VmImageBackupReq  imgbackup_create,imgbackup_delete;

    string hostname = host_pool->GetHostNameById(pvm->_hid);
    SkyAssert(false == hostname.empty());

    if(pvm->_config._machine._target!=req._target)
    {
        vector<VmDiskConfig>::iterator it = pvm->_config._disks.begin();
        for (it = pvm->_config._disks.begin(); it != pvm->_config._disks.end(); it++)
        {
            if (it->_target == req._target)
            {
                break;
            }
        }

        if (it == pvm->_config._disks.end())
        {
            OutPut(SYS_ERROR, "not find backup target %s:\n",req._target.c_str());
            error <<"not find backup target "<<req._target;
            return ERROR_OBJECT_NOT_EXIST;
        }
        imgbackup_create._disk=imgbackup_delete._disk=*it;
    }
    else
    {
       imgbackup_create._disk=imgbackup_delete._disk= pvm->_config._machine;
    }
    imgbackup_create._request_id=imgbackup_delete._request_id=GenerateUUID();
    imgbackup_create._description=req._description;

    if(SUCCESS==image_pool->GetActiveImageBackup(request_id, req._target, req._vid))
    {
        imgbackup_create._disk._request_id=request_id;
    }

    MID HC(hostname, PROC_HC, MU_VM_HANDLER);
    VmAction backup_delete(VM_WF_CATEGORY, VM_ACT_BACKUP_IMAGE_DELETE, EV_VM_OP, LCM, HC);
    imgbackup_delete.action_id = backup_delete.get_id();
    imgbackup_delete._operation=DELETE_IMAGE_BACKUP;
    imgbackup_delete._vid = req._vid;
    imgbackup_delete.retry     = true;
    backup_delete._vid=req._vid;
    backup_delete.message_req = imgbackup_delete.serialize();
    backup_delete.success_feedback = FEEDBACK_END_VMOP;

    VmAction backup_create(VM_WF_CATEGORY, VM_ACT_BACKUP_IMAGE_CREATE, EV_VM_OP, LCM, HC);
    imgbackup_create.action_id = backup_create.get_id();
    imgbackup_create._operation=CREATE_IMAGE_BACKUP;
    imgbackup_create._vid = req._vid;
    backup_create._vid=req._vid;
    backup_create.message_req = imgbackup_create.serialize();
    backup_create.rollback_action_id=backup_delete.get_id();
    backup_create.success_feedback = FEEDBACK_CREATE_BACKUPIMG;
    backup_create.failure_feedback = FEEDBACK_ROLLBACK_WORKFLOW;

    wf.Add(&backup_create);
    wf_rollback.Add(&backup_delete);

    ret = CreateWorkflow(wf, wf_rollback);
    if(SUCCESS != ret)
    {
        Debug(SYS_WARNING, "BackupImgCreate failed to created workflow: !!!\n" );
        error <<"create backup image workflow fail with ret "<<ret;
        SkyAssert(false);
    }
    return ret;
}

STATUS VMManager::OperateBackupImg(const VmOperationReq& req, const MID& sender, const string &req_info, int& progress, ostringstream& error)
{
    VmImageBackupOp imgbackup_req;
    bool result = imgbackup_req.deserialize(req_info);
    SkyAssert(result);
    string wf_id;
    STATUS ret = ERROR;
    VMImagePool *image_pool = VMImagePool::GetInstance();
    SkyAssert(image_pool);

    if (SQLDB_OK == vm_pool->GetVmWorkFlowId(imgbackup_req._vid, VM_WF_IMAGE_BACKUP_CREATE, wf_id))
    {//是否有正向流
        ret = ERROR_ACTION_RUNNING;
        progress = GetWorkflowProgress(wf_id);
        error << "VM " << imgbackup_req._vid << " is creating backup image.";
    }
    else if (SQLDB_OK == vm_pool->GetVmWorkFlowId(imgbackup_req._vid, VM_WF_IMAGE_BACKUP_DELETE, wf_id))
    {//反响或者回滚流
        if (IsRollbackWorkflow(wf_id))
        {//回滚流
            ret = ERROR_ACTION_RUNNING;
            progress = GetWorkflowProgress(wf_id);
            error << "The action for deploy VM " << imgbackup_req._vid << " is rollbacking.";
        }
        else
        {//反向流
            ret = ERROR_PERMISSION_DENIED;
            progress = GetWorkflowProgress(wf_id);
            error << "VM " << imgbackup_req._vid << " is deleting image.";
        }
    }
    else if (false)//收到消息就创建一个备份
    {
        ret = SUCCESS;
        progress = 100;
        error << "vm " << imgbackup_req._vid << " is already ba pdisk";
    }
    else
    {
        ret = BackupImgCreate(imgbackup_req, sender, error);
        if (ret == SUCCESS)
        {//工作流增加成功了，才给TC返回DOING，否则返回错误信息
            ret = ERROR_ACTION_RUNNING;
            progress = 0;
            error << "VM " << imgbackup_req._vid << " is attaching pdisk.";
        }
        else
        {
            progress = 0;
        }
    }

    return ret;

}

STATUS VMManager::BackupImgDelete(VmImageBackupOp& req, const MID& sender, ostringstream& error)
{
    string wf_id;
    STATUS ret;
    MID LCM = MID(MU_VM_LIFECYCLE_MANAGER);
    MID SC = GetSC_MID();
    string  request_id,target;
    vector<string> son_request_id;

    VMImagePool *image_pool = VMImagePool::GetInstance();
    SkyAssert(image_pool);
    VirtualMachine *pvm = vm_pool->Get(req._vid, false);
    SkyAssert(pvm);

    /* 要删除的备份是激活态，直接返回失败 */
    ret = image_pool->GetImageBackupTarget(req._request_id, target);
    if(ERROR_DB_SELECT_FAIL == ret)
    {
         Debug(SYS_ERROR,"BackupImgDelete GetImageBackupTarget: db select fail!\n");
         error<<"get backup img target,db selete fail";
         return ERROR_DB_SELECT_FAIL;
    }
    else if(ERROR_OBJECT_NOT_EXIST == ret)
    {
         Debug(SYS_ERROR,"BackupImgDelete success! GetImageBackupTarget: request_id %s don't exist!\n", req._request_id.c_str());
         return SUCCESS;
    }

    ret = image_pool->GetActiveImageBackup(request_id, target, req._vid);
    if(SUCCESS == ret)
    {
         if(req._request_id == request_id)
         {
             Debug(SYS_ERROR,"BackupImgDelete GetActiveImageBackup: IMG_BACKUP_ACTIVE, can't delete it!\n");
             error<<"backup img active,can't delelte";
             return ERROR_IMG_BACKUP_ACTIVED;
         }
    }
    else if(ERROR_OBJECT_NOT_EXIST != ret)
    {
         Debug(SYS_ERROR,"BackupImgDelete GetActiveImageBackup:db select fail.\n");
         error<<"get active backup img,db selete fail";
         return ERROR_DB_SELECT_FAIL;
    }

    /* 查询待删除备份的子备份 */
    ret = image_pool->GetSonImageBackup(req._request_id, son_request_id);
    if((SUCCESS != ret)&&(ERROR_OBJECT_NOT_EXIST != ret))
    {
         Debug(SYS_ERROR,"BackupImgDelete GetSonImageBackup:db select fail.\n");
         error<<"get son backup img,db selete fail";
         return ERROR_DB_SELECT_FAIL;
    }

    VmWorkFlow  wf;
    wf.category = VM_WF_CATEGORY;
    wf._vid = req._vid;
    wf.name = VM_WF_IMAGE_BACKUP_DELETE;
    wf._operation = DELETE_IMAGE_BACKUP;
    wf.upstream_sender = sender;
    wf.upstream_action_id = req.action_id;

    VmImageBackupReq  imgbackup_delete;

    string hostname = host_pool->GetHostNameById(pvm->_hid);
    SkyAssert(false == hostname.empty());

    if(pvm->_config._machine._target!=target)
    {
        vector<VmDiskConfig>::iterator it = pvm->_config._disks.begin();
        for (it = pvm->_config._disks.begin(); it != pvm->_config._disks.end(); it++)
        {
            if (it->_target == target)
            {
                break;
            }
        }

        if (it == pvm->_config._disks.end())
        {
            OutPut(SYS_ERROR, "BackupImgDelete:not find backup target %s:\n",target.c_str());
            error<<"not find backup target "<<target;
            return ERROR_OBJECT_NOT_EXIST;
        }
        imgbackup_delete._disk=*it;
    }
    else
    {
        imgbackup_delete._disk= pvm->_config._machine;
    }
    imgbackup_delete._request_id=req._request_id;   
    imgbackup_delete._son_request_id.assign(son_request_id.begin(), son_request_id.end());

    MID HC(hostname, PROC_HC, MU_VM_HANDLER);
    VmAction backup_delete(VM_WF_CATEGORY, VM_ACT_BACKUP_IMAGE_DELETE, EV_VM_OP, LCM, HC);
    imgbackup_delete.action_id = backup_delete.get_id();
    imgbackup_delete._operation=DELETE_IMAGE_BACKUP;
    imgbackup_delete._vid = req._vid;
    imgbackup_delete.retry     = true;
    backup_delete._vid=req._vid;
    backup_delete.message_req = imgbackup_delete.serialize();
    backup_delete.success_feedback = FEEDBACK_DELETE_BACKUPIMG;
    backup_delete.failure_feedback = NONE;

    wf.Add(&backup_delete);

    ret = CreateWorkflow(wf);
    if(SUCCESS != ret)
    {
        Debug(SYS_WARNING, "BackupImgDelete failed to created workflow: !!!\n" );
        error<<"failed to created workflow";
        SkyAssert(false);
    }
    return ret;
}

STATUS VMManager::OperateDeleteBackupImg(const VmOperationReq& req, const MID& sender, const string &req_info, int& progress, ostringstream& error)
{
    VmImageBackupOp imgbackup_req;
    bool result = imgbackup_req.deserialize(req_info);
    SkyAssert(result);
    string wf_id;
    STATUS ret = ERROR;
    VMImagePool *image_pool = VMImagePool::GetInstance();
    SkyAssert(image_pool);

    if (SQLDB_OK == vm_pool->GetVmWorkFlowId(imgbackup_req._vid, VM_WF_IMAGE_BACKUP_DELETE, wf_id))
    {//是否有正向流
        ret = ERROR_ACTION_RUNNING;
        progress = GetWorkflowProgress(wf_id);
        error << "VM " << imgbackup_req._vid << " is deleting backup image.";
    }
    else if (SQLDB_OK == vm_pool->GetVmWorkFlowId(imgbackup_req._vid, VM_WF_IMAGE_BACKUP_CREATE, wf_id))
    {//反响流
        if (!IsRollbackWorkflow(wf_id))
        {//反向流
            ret = ERROR_PERMISSION_DENIED;
            progress = GetWorkflowProgress(wf_id);
            error << "VM " << imgbackup_req._vid << " is creating image.";
        }
    }
    else if (false)//收到消息就删除一个备份
    {
        ret = SUCCESS;
        progress = 100;
        error << "vm " << imgbackup_req._vid << " is already delete a backup image";
    }
    else
    {
        ret = BackupImgDelete(imgbackup_req, sender, error);
        if (ret == SUCCESS)
        {//工作流增加成功了，才给TC返回DOING，否则返回错误信息
            ret = ERROR_ACTION_RUNNING;
            progress = 0;
            error << "VM " << imgbackup_req._vid << " is deleting backup image.";
        }
        else
        {
            progress = 0;
        }
    }

    return ret;

}


STATUS VMManager::SetExpectedState(const VmOperationReq& req)
{
    int expected_state;
    switch (req._operation)
    {
        case CREATE:
        case START:
        case RESUME:
        {
            expected_state = VM_RUNNING;
            break;
        }

        case DESTROY:
        case STOP:
        case CANCEL:
        {
            expected_state = VM_SHUTOFF;
            break;
        }

        case PAUSE:
        {
            expected_state = VM_PAUSED;
        }

        default:
        {//期望状态没有改变，直接返回
            return SUCCESS;
        }
    }

    return VmAssistant->SetValue(req._vid, "expected_state", expected_state);
}

STATUS VMManager::OperateStartVM(const VmOperationReq& req, const MID sender, ostringstream& error)
{
    VmCreateReq createreq(req._vid, req._operation);
    createreq._user = req._user;
    VmAssistant->ConstructCreateReq(req._vid, createreq);
    return DoCommonOp(createreq, sender, FEEDBACK_END_VMOP, FEEDBACK_DELETE_WORKFLOW);
}

STATUS VMManager::OperateRecoverImage(const VmOperationReq& req, const MID sender, ostringstream& error)
{
    //一键恢复镜像,需要检查镜像使用方式是否为快照
    ConfigCluster *cluster_config = ConfigCluster::GetInstance();
    if (NULL == cluster_config)
    {
        error << "OperateRecoverImage: ConfigCluster getinstance error!\n";
        Debug(SYS_WARNING, error.str());
        return ERROR_PERMISSION_DENIED;
    }
    string img_usage = cluster_config->Get(CFG_IMG_USAGE);
    if (0 != img_usage.compare("snapshot"))
    {
        error << "OperateRecoverImage: img_usage is not snapshot!\n";
        Debug(SYS_WARNING, error.str());
        return ERROR_PERMISSION_DENIED;
    }

    VMImagePool *image_pool = VMImagePool::GetInstance();
    SkyAssert(image_pool);

    int back_img_num = 0;
    image_pool->GetImageBackupNumByVid(req._vid, back_img_num);

    if(0 != back_img_num)
    {
        error << "vm "<<req._vid<<" has backup image,not allow to recover image";
        Debug(SYS_WARNING, error.str());
        return ERROR_PERMISSION_DENIED;
    }

    VmRecoverImageReq recoverreq(req._vid, req._operation);
    recoverreq._user = req._user;
    VmAssistant->ConstructRecoverReq(req._vid, recoverreq);
    return DoCommonOp(recoverreq, sender, FEEDBACK_END_VMOP, FEEDBACK_DELETE_WORKFLOW);
}

STATUS VMManager::OperateModifyImageBackup(const string &req, ostringstream& error)
{
    VmImageBackupOp op;
    bool result = op.deserialize(req);
    SkyAssert(result);

    VMImagePool *image_pool = VMImagePool::GetInstance();
    SkyAssert(image_pool);
    STATUS ret = image_pool->UpdateImageBackup(op._request_id, op._description);
    if (SUCCESS != ret)
    {
        error << "update vm_image_backup failed" <<endl;
    }
    return ret;
}

STATUS VMManager::OperateRestoreVM(const VmOperationReq& req, const string &req_info,const MID sender, int &progress,ostringstream& error)
{
    VmImageBackupOp restore_info;
    bool result = restore_info.deserialize(req_info);
    SkyAssert(result);
    STATUS ret = ERROR;
    int state;
    bool bIsBase = false;

    VMImagePool *image_pool = VMImagePool::GetInstance();
    SkyAssert(image_pool);

    //检查恢复的镜像快照状态
    ret = image_pool->GetImageBackupState(restore_info._request_id,restore_info._target,restore_info._vid,state);
    if(ret != SUCCESS)
    {
        if(ret == ERROR_OBJECT_NOT_EXIST)
        {
            VirtualMachine *pvm = vm_pool->Get(restore_info._vid, false);
            SkyAssert(pvm);
            if((pvm->_config._machine._request_id == restore_info._request_id)||\
              (pvm->_config._kernel._request_id == restore_info._request_id) ||\
              (pvm->_config._initrd._request_id == restore_info._request_id))
            {
                bIsBase = true;
            }
            else if(pvm->_config._disks.size())
            {
                vector<VmDiskConfig>::iterator it;
                for (it = pvm->_config._disks.begin(); it != pvm->_config._disks.end(); it++)
                {
                    if(it->_request_id == restore_info._request_id)
                    {
                        bIsBase = true;
                        break;
                    }
                }
            }

            if(bIsBase)
            {
                //虽然符合上面规则，但是不符合表规则，则返回非base
                if(false == image_pool->IsBaseImageBackup(restore_info._request_id,restore_info._target,restore_info._vid))
                {
                    bIsBase= false;
                }
            }

            if(!bIsBase)
            {
                progress = 0;
                error << "image backup "<<restore_info._request_id<<" no exist ";
                return ERR_OBJECT_NOT_EXIST;
            }
        }
        else
        {
            progress = 0;
            error << "select image backup table fail ";
            return ERROR_DB_SELECT_FAIL;
        }
    }
    else if(state==IMG_BACKUP_ACTIVE)
    {
        progress = 100;
        error << "image backup is active ";
        return SUCCESS;
    }

    string wf_id;
    if (SQLDB_OK == vm_pool->GetVmWorkFlowId(restore_info._vid, VM_WF_RESTORE, wf_id))
    {//是否有正向流
        ret = ERROR_ACTION_RUNNING;
        progress = GetWorkflowProgress(wf_id);
        error << "VM " << restore_info._vid << " is restoring.";
    }
    else
    {
        MID VM = MID(MU_VM_MANAGER);

        VmWorkFlow  restore_wf;
        int64 vid = req._vid;

        restore_wf._vid = vid;
        restore_wf._operation = RESTORE_IMAGE_BACKUP;
        restore_wf.category = VM_WF_CATEGORY;
        restore_wf.name = VM_WF_RESTORE;
        restore_wf.upstream_sender = sender;
        restore_wf.upstream_action_id = req.action_id;

        VmAction restore_action(VM_WF_CATEGORY,VM_WF_RESTORE,EV_VM_RESTORE_REQ,VM,VM);
        //restore_action.failure_feedback = FEEDBACK_RESTART_ACTION;
        restore_info.retry = true;
        restore_action.success_feedback = true;
        restore_action._vid = vid;
        restore_info.action_id = restore_action.get_id();
        restore_action.message_req = restore_info.serialize();

        restore_wf.Add(restore_action);
        ret = CreateWorkflow(restore_wf);
        if(SUCCESS != ret)
        {
            Debug(SYS_WARNING, "VMM failed to created restore workflow: %s!!!\n", restore_wf.get_id().c_str());
            SkyAssert(false);
            error << "VM " << restore_info._vid << " restore fail.";
        }
        else
        {
            error << "VM " << restore_info._vid << " is restoring.";
            ret = ERROR_ACTION_RUNNING;
        }
        progress = 0;

    }

    return ret;
}

STATUS VMManager::OperateCancelVM(const VmOperationReq& req, const MID sender, int &progress, ostringstream& error)
{
    string wf_id;
    STATUS ret;

    if (SQLDB_OK == vm_pool->GetVmWorkFlowId(req._vid, VM_WF_DEPLOY, wf_id))
    {//是否有正向流
        string rollback_id;
        GetRollbackWorkflow(wf_id, rollback_id);
        ret = RollbackWorkflow(wf_id);
        if (ret == SUCCESS)
        {
            UnsetRollbackAttr(rollback_id, sender, req.action_id);
            ret = ERROR_ACTION_RUNNING;
            progress = GetWorkflowProgress(rollback_id);
            error << "VM " << req._vid << " is canceling!";
        }
        else
        {
            progress = 0;
            error << "rollback workflow " << wf_id << " failed! Ret = " << ret;
        }
    }
    else if (SQLDB_OK == vm_pool->GetVmWorkFlowId(req._vid, VM_WF_CANCEL, wf_id))
    {//反响或者回滚流
        if (IsRollbackWorkflow(wf_id))
        {//回滚流
            UnsetRollbackAttr(wf_id, sender, req.action_id);

            progress = GetWorkflowProgress(wf_id);
            ret = (progress == 100) ? SUCCESS : ERROR_ACTION_RUNNING;
            error << "The action for deploy VM " << req._vid << "is rollbacking, changed to cancel.";
        }
        else
        {//反向流
            progress = GetWorkflowProgress(wf_id);
            ret = (progress == 100) ? SUCCESS : ERROR_ACTION_RUNNING;
            error << "VM " << req._vid << " is canceling.";
        }
    }
    else if (SQLDB_OK == vm_pool->IsVmExist(req._vid))
    {//vm_pool里存在
        progress = 0;
        error << "Begin to cancel vm " << req._vid;
        ret = AddCancelWorkFLow(req, sender, error);
        if (ret == SUCCESS)
        {
            ret = ERROR_ACTION_RUNNING;
        }
    }
    else
    {
        ret = SUCCESS;
        progress = 100;
        error << "VM " << req._vid << " is already canceled.";
    }

    return ret;
}

STATUS VMManager::AddCancelWorkFLow(const VmOperationReq& req, const MID sender, ostringstream& error)
{
    //增加新工作流，那就要把当前的工作流删除
    STATUS ret = VmAssistant->DeleteVMWorkflow(req._vid);

    MID LCM = MID(MU_VM_LIFECYCLE_MANAGER);
    MID SCH = MID(MU_VM_SCHEDULER);
    int64 vid = req._vid;

    VmWorkFlow  cancel_wf;
    cancel_wf.category = VM_WF_CATEGORY;
    cancel_wf._vid = vid;
    cancel_wf.name = VM_WF_CANCEL;
    cancel_wf._operation = CANCEL;
    cancel_wf.upstream_sender = sender;
    cancel_wf.upstream_action_id = req.action_id;
    if(!req._user.empty())
    {
        cancel_wf.originator = req._user;
    }

    VirtualMachine *pvm = vm_pool->Get(req._vid, false);
    SkyAssert(pvm);

    MID SC = GetSC_MID();

    bool skip_hc = false;
    MID HC("", PROC_HC, MU_VM_HANDLER);
    if (pvm->_hid == -1)
    {
        skip_hc = true;
    }
    else
    {
        skip_hc = false;
        HC._application = host_pool->GetHostNameById(pvm->_hid);
    }

    string cc_name      = ApplicationName();
    string iscsiname    = host_pool->GetIscsinameById(pvm->_hid);
    string userip       = host_pool->GetHostIpById(pvm->_hid);
    string hc_name       = host_pool->GetHostNameById(pvm->_hid);
    MID IA(cc_name, PROC_IMAGE_AGENT, MU_IMAGE_AGENT);

    VmAction remove_vm(VM_WF_CATEGORY, VM_ACT_REMOVE, EV_VM_OP, LCM, HC);
    remove_vm._vid = vid;
    VmCreateReq cancelreq(pvm->_config);
    cancelreq.action_id = remove_vm.get_id();
    cancelreq._vid = vid;
    cancelreq._operation = CANCEL;
    GetVstorageOption(cancelreq._option._image_store_option);
    cancelreq.retry = true;
    remove_vm.message_req = cancelreq.serialize();
    remove_vm.skipped     = skip_hc;
    cancel_wf.Add(&remove_vm);

    VmAction unselect_host(VM_WF_CATEGORY, VM_ACT_UNSELECT_HC, EV_VM_UNSELECT_HOST, LCM, LCM);
    unselect_host._vid = vid;
    VmOperationReq unselect_req;
    unselect_req._vid = vid;
    unselect_req.action_id = unselect_host.get_id();
    unselect_host.message_req = unselect_req.serialize();
    unselect_host.success_feedback  = FEEDBACK_NULL;
    unselect_host.failure_feedback  = FEEDBACK_NULL;    
    cancel_wf.Add(&remove_vm, &unselect_host);

    //添加存储的工作流
    vector<VmDiskConfig> disks = pvm->_config._disks;
    disks.push_back(pvm->_config._machine);
    //disks.push_back(deploy._config._kernel);
    //disks.push_back(deploy._config._initrd);

    ImageStoreOption option;
    GetVstorageOption(option);
    vector<VmDiskConfig>::iterator it;
    for (it = disks.begin(); it != disks.end(); it++)
    {
        if (it->_id != INVALID_FILEID)
        {// image
            if (IMAGE_POSITION_LOCAL != it->_position)
            {
                if (IMAGE_USE_SNAPSHOT == option._share_img_usage)
                {
                    DeleteSnapshotReq delete_snapshot_req;
                    delete_snapshot_req._uid = -1;
                    delete_snapshot_req._request_id = it->_request_id;

                    VmAction delete_snapshot(STORAGE_CATEGORY,DELETE_SNAPSHOT_BLOCK,EV_STORAGE_DELETE_SNAPSHOT_REQ,LCM,SC);
                    delete_snapshot._vid = vid;
                    delete_snapshot_req.action_id = delete_snapshot.get_id();
                    delete_snapshot_req.retry     = true;
                    delete_snapshot.message_req = delete_snapshot_req.serialize();

                    DeAuthBlockReq deauth_snapshot_req;
                    deauth_snapshot_req._vid         = vid;
                    deauth_snapshot_req._request_id  = it->_request_id;
                    deauth_snapshot_req._cluster_name = cc_name;
                    deauth_snapshot_req._userip = userip;
                    deauth_snapshot_req._iscsiname = iscsiname;
                    deauth_snapshot_req._dist_id = -1;
                    deauth_snapshot_req._usage = to_string<int64>(vid,dec)+":"+it->_target;

                    VmAction deauth_snapshot(STORAGE_CATEGORY, VM_ACT_DEAUTH_BLOCK, EV_STORAGE_DEAUTH_BLOCK_REQ, LCM, SC);
                    deauth_snapshot._vid = vid;
                    deauth_snapshot_req.action_id = deauth_snapshot.get_id();
                    deauth_snapshot_req.retry     = true;
                    deauth_snapshot.message_req = deauth_snapshot_req.serialize();

                    cancel_wf.Add(&remove_vm, &deauth_snapshot);
                    cancel_wf.Add(&deauth_snapshot, &delete_snapshot);
                    cancel_wf.Add(&delete_snapshot, &unselect_host);
                }
                else
                {
                    DeleteBlockReq delete_block_req;
                    delete_block_req._uid = -1;
                    delete_block_req._request_id = it->_request_id;

                    VmAction delete_block(STORAGE_CATEGORY,DELETE_BLOCK,EV_STORAGE_DELETE_BLOCK_REQ,LCM,SC);
                    delete_block._vid = vid;
                    delete_block_req.action_id = delete_block.get_id();
                    delete_block_req.retry     = true;
                    delete_block.message_req = delete_block_req.serialize();

                    DeAuthBlockReq deauth_block_req;
                    deauth_block_req._vid         = vid;
                    deauth_block_req._request_id  = it->_request_id;
                    deauth_block_req._cluster_name = cc_name;
                    deauth_block_req._userip = userip;
                    deauth_block_req._iscsiname = iscsiname;
                    deauth_block_req._dist_id = -1;
                    deauth_block_req._usage = to_string<int64>(vid,dec)+":"+it->_target;

                    VmAction deauth_block(STORAGE_CATEGORY, VM_ACT_DEAUTH_BLOCK, EV_STORAGE_DEAUTH_BLOCK_REQ, LCM, SC);
                    deauth_block._vid = vid;
                    deauth_block_req.action_id = deauth_block.get_id();
                    deauth_block_req.retry     = true;
                    deauth_block.message_req = deauth_block_req.serialize();

                    cancel_wf.Add(&remove_vm, &deauth_block);
                    cancel_wf.Add(&deauth_block, &delete_block);
                    cancel_wf.Add(&delete_block, &unselect_host);
                }
            }
        }
    }

    //撤销共享disk
    vector<VmDiskConfig> share_disk;
    VMImagePool *image_pool = VMImagePool::GetInstance();
    SkyAssert(image_pool);
    ret = image_pool->GetDisk(vid, share_disk);
    for (it = disks.begin(); it != disks.end(); it++)
    {
        if (it->_position == VM_STORAGE_TO_SHARE)
        {//使用共享磁盘
            DeleteBlockReq del_block;
            del_block._request_id = it->_request_id;
            del_block._uid = -1;

            VmAction deauth_share_block(STORAGE_CATEGORY, VM_ACT_DEAUTH_BLOCK, EV_STORAGE_DEAUTH_BLOCK_REQ, LCM, SC);
            DeAuthBlockReq deauth_req;
            deauth_req.action_id    = deauth_share_block.get_id();
            deauth_req._request_id  = it->_request_id;
            deauth_req._iscsiname   = iscsiname;
            deauth_req._userip      = userip;
            deauth_req._vid         = vid;
            deauth_req._cluster_name = cc_name;
            deauth_req._usage =to_string<int64>(vid,dec)+":"+it->_target;
            deauth_req.retry        = true;
            deauth_share_block.message_req = deauth_req.serialize();

            cancel_wf.Add(&remove_vm, &deauth_share_block);

            VmAction delete_share_block(STORAGE_CATEGORY, VM_ACT_DELETE_BLOCK, EV_STORAGE_DELETE_BLOCK_REQ, LCM, SC);
            del_block.action_id = delete_share_block.get_id();
            del_block.retry     = true;
            delete_share_block.message_req = del_block.serialize();
            delete_share_block.skipped = !(it->_is_need_release);
            cancel_wf.Add(&deauth_share_block, &delete_share_block);
            cancel_wf.Add(&delete_share_block, &unselect_host);
        }
    }

    ret = CreateWorkflow(cancel_wf);
    if(SUCCESS != ret)
    {
        Debug(SYS_WARNING, "VMM failed to created workflow: %s!!!\n", cancel_wf.get_id().c_str());
        SkyAssert(false);
    }

    return ret;
}

STATUS VMManager::OperatePluginUSB(const VmOperationReq& req, MID sender, const string &req_info, ostringstream& error)
{
    TransactionOp tsop; //启用事务操作

    VmUsbOp usb_req;
    bool result = usb_req.deserialize(req_info);
    SkyAssert(result);

    int64 vid_using_usb;
    int ret = usb_pool->GetVidbyUsb(vid_using_usb, usb_req._usb_info._vendor_id,usb_req._usb_info._product_id);
    if (ret == SQLDB_OK)
    {//如找到，说明这个usb已经被使用了
        if (vid_using_usb == req._vid)
        {
            error << "usb already be used!";
            Debug(SYS_DEBUG,"%s\n",error.str().c_str());
            return SUCCESS;
        }
        else
        {
            error << "usb already be used in other vm " << vid_using_usb;
            Debug(SYS_DEBUG,"%s\n",error.str().c_str());
            return ERROR_PERMISSION_DENIED;
        }
    }

    VmAssistant->InsertVmUsbInfo(usb_req);

    ret = AddCommonWorkflow(usb_req, sender, FEEDBACK_END_VMOP, FEEDBACK_PLUGINUSB_FAILED);
    if (ret == SUCCESS)
    {
        ret = tsop.Commit();
    }
    else
    {
        SkyAssert(false);
    }

    if (ret == SUCCESS)
    {
        ret = ERROR_ACTION_RUNNING;
    }
    else
    {
        SkyAssert(false);
    }
    return ret;
}

STATUS VMManager::OperatePlugoutUSB(const VmOperationReq& req, MID sender, const string &req_info, ostringstream& error)
{
    //TransactionOp tsop; //启用事务操作

    VmUsbOp usb_req;
    bool result = usb_req.deserialize(req_info);
    SkyAssert(result);

    // 拔出和插拔都需要检查USB是否在表中
    if (!usb_pool->IsUsbExist(usb_req._vid, usb_req._usb_info._vendor_id,usb_req._usb_info._product_id))
    {
        error << "usb info does not exist";
        Debug(SYS_DEBUG,"%s\n",error.str().c_str());
        return ERROR_DEVICE_NOT_EXIST;
    }

    //usb重插拔也借用此接口,但成功后不需要再写表了
    int success_func;
    if (req._operation == PLUG_OUT_USB)
    {
        success_func = FEEDBACK_PLUGOUTUSB_SUCCESSED;
    }
    else
    {
        success_func = FEEDBACK_END_VMOP;
    }

    //成功拔出USB之后再删表
    return DoCommonOp(usb_req, sender, success_func);
}

STATUS VMManager::DoCommonOp(VmOperationReq &req, MID sender, const int success_func, const int failure_func)
{
    STATUS ret = AddCommonWorkflow(req, sender, success_func, failure_func);
    if(SUCCESS == ret)
    {
        ret = ERROR_ACTION_RUNNING;
    }

    return ret;
}

STATUS VMManager::AddCommonWorkflow(VmOperationReq &req, MID sender, const int success_func, const int failure_func)
{
    //增加新工作流，那就要把当前的工作流删除
    STATUS ret = VmAssistant->DeleteVMWorkflow(req._vid);

    VirtualMachine *pvm = vm_pool->Get(req._vid, false);
    SkyAssert(pvm);

    string wf_name, action_name;
    GetVMOpWFName(req._operation, wf_name, action_name);

    VmWorkFlow  wf;
    wf.category = VM_WF_CATEGORY;
    wf._vid = req._vid;
    wf.name = wf_name;
    wf._operation = req._operation;
    wf.upstream_sender = sender;
    wf.upstream_action_id = req.action_id;
    if(!req._user.empty())
    {
        wf.originator = req._user;
    }

    string hostname = host_pool->GetHostNameById(pvm->_hid);
    SkyAssert(false == hostname.empty());
    MID HC(hostname, PROC_HC, MU_VM_HANDLER);

    MID LCM = MID(MU_VM_LIFECYCLE_MANAGER);

    VmAction action(VM_WF_CATEGORY, action_name, EV_VM_OP, LCM, HC);
    action._vid = req._vid;
    req.action_id = action.get_id();
    if (failure_func == FEEDBACK_RESTART_ACTION)
    {
        req.retry = true;
    }
    action.message_req = req.serialize();
    action.success_feedback = success_func;
    if (failure_func != FEEDBACK_RESTART_ACTION)
    {
        action.failure_feedback = failure_func;
    }

    wf.Add(action);
    ret = CreateWorkflow(wf);
    if(SUCCESS != ret)
    {
        Debug(SYS_WARNING, "VMM failed to created workflow: %s!!!\n", wf.get_id().c_str());
        SkyAssert(false);
    }

    return ret;

}

/**********************************************************************
* 函数名称：VMManager::MigrateVM
* 功能描述：处理虚拟机热迁移请求
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期     版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2012/03/05   V1.0    姚远        创建
************************************************************************/
STATUS VMManager::MigrateVM(const VmOperationReq &req, const string &req_info, const MID &sender, ostringstream& error )
{
    if ( !IsVMSupportLiveMigrate( req._vid ) )
    {/* 如不支持迁移，则返回不支持 */
        error << "This vm(id = "<< req._vid << ") is not supported migrate.";
        Debug(SYS_WARNING,"%s\n",error.str().c_str());
        return ERROR_NOT_SUPPORT;
    }

    VmMigrateReq migrate_req;
    bool result = migrate_req.deserialize(req_info);
    SkyAssert(result);

    int64 vid = req._vid;

    VirtualMachine *pvm = vm_pool->Get(vid, false);
    SkyAssert(pvm);

    if (pvm->_state != VM_RUNNING)
    {
        error << "live migrate is forbidden when " << Enum2Str::InstStateStr(pvm->_state) <<" state.";
        Debug(SYS_WARNING,"%s\n",error.str().c_str());
        return ERROR_NOT_SUPPORT;
    }

    MID SC = GetSC_MID();
    MID LCM = MID(MU_VM_LIFECYCLE_MANAGER);
    MID SCH = MID(MU_VM_SCHEDULER);
    const string src_hc_name = host_pool->GetHostNameById(pvm->_hid);
    const string host_ip = host_pool->GetHostIpById(pvm->_hid);
    const string iscsiname = host_pool->GetIscsinameById(pvm->_hid);
    MID HC(src_hc_name, PROC_HC, MU_VM_HANDLER);  //此时源端已知，目的端未知
    const string cc_name = ApplicationName();
    const bool skip_nic = !IsNeedVNet(pvm->_config);   //没有虚拟网卡的时候，不需要向NET发消息
    const int timeout = 60*30;   //30分钟超时

    //构建热迁移的数据
    string src_ssh_public_key = host_pool->GetHostSshkeyById(pvm->_hid);
    VmMigrateData live_migrate(vid, MIGRATE, src_ssh_public_key);
    VmAssistant->ConstructCreateReq(vid, live_migrate);

    VmWorkFlow wf_migrate;
    wf_migrate.category             = VM_WF_CATEGORY;
    wf_migrate.name                 = VM_WF_MIGRATE;
    wf_migrate._vid                 = vid;
    wf_migrate._operation           = LIVE_MIGRATE;
    wf_migrate.upstream_sender      = sender;
    wf_migrate.upstream_action_id   = req.action_id;
    if(!req._user.empty())
    {
        wf_migrate.originator           = req._user;
    }    

    VmWorkFlow  wf_rollback;
    wf_rollback.category            = VM_WF_CATEGORY;
    wf_rollback.name                = VM_WF_MIGRATE;
    wf_rollback._vid                = vid;
    wf_rollback._operation          = LIVE_MIGRATE;
    wf_rollback.upstream_sender     = sender;
    wf_rollback.upstream_action_id  = req.action_id;
    if(!req._user.empty())
    {
        wf_rollback.originator          = req._user;
    }  

    //因为存储相关的action需要遍历disks，所以，先构造存储自身action的参数
    vector<Action> vec_auth_hc2, vec_deauth_hc1, vec_deauth_hc2;

    vector<VmDiskConfig> disks = pvm->_config._disks;
    disks.push_back(pvm->_config._machine);
    //disks.push_back(deploy._config._kernel);
    //disks.push_back(deploy._config._initrd);

    vector<VmDiskConfig>::iterator it;
    for (it = disks.begin(); it != disks.end(); it++)
    {
        //SkyAssert(it->_position == VM_STORAGE_TO_SHARE);
        if (it->_position != VM_STORAGE_TO_SHARE)
        {
            continue;
        }

        AuthBlockReq auth_req;
        DeAuthBlockReq deauth_req;
        auth_req._vid           = deauth_req._vid         = vid;
        auth_req._request_id    = deauth_req._request_id  = it->_request_id;
        auth_req._cluster_name  = deauth_req._cluster_name = cc_name;
        //auth_req._usage  = deauth_req._usage = to_string<int64>(vid,dec)+":"+it->_target;  usage don't set

        VmAction deauth_hc2(STORAGE_CATEGORY, VM_ACT_DEAUTH_BLOCK, EV_STORAGE_DEAUTH_BLOCK_REQ, LCM, SC);
        deauth_hc2._vid = vid;
        deauth_req.action_id = deauth_hc2.get_id();
        deauth_req.retry     = true;
        deauth_hc2.message_req = deauth_req.serialize();
        deauth_hc2.skipped = true;
        deauth_hc2.success_feedback=(it->_id != INVALID_FILEID)?FEEDBACK_DELETE_VMSHAREIMAGE:FEEDBACK_DELETE_VMSHAREBLOCK;
        vec_deauth_hc2.push_back(deauth_hc2);

        VmAction auth_hc2(STORAGE_CATEGORY, VM_ACT_AUTH_BLOCK, EV_STORAGE_AUTH_BLOCK_REQ, LCM, SC);
        auth_hc2._vid = vid;
        auth_req.action_id = auth_hc2.get_id();
        auth_hc2.message_req = auth_req.serialize();
        auth_hc2.rollback_action_id = deauth_hc2.get_id();
        auth_hc2.failure_feedback = FEEDBACK_ROLLBACK_WORKFLOW;
        auth_hc2.success_feedback=(it->_id != INVALID_FILEID)?FEEDBACK_MIGRATE_ADD_VMSHAREIMAGE:FEEDBACK_MIGRATE_ADD_VMSHAREBLOCK;
        vec_auth_hc2.push_back(auth_hc2);

        VmAction deauth_hc1(STORAGE_CATEGORY, VM_ACT_DEAUTH_BLOCK, EV_STORAGE_DEAUTH_BLOCK_REQ, LCM, SC);
        deauth_hc1._vid = vid;
            deauth_req._userip      = host_ip;
            deauth_req._iscsiname   = iscsiname;
            deauth_req.retry        = true;
        deauth_req.action_id    = deauth_hc1.get_id();
        deauth_hc1.message_req  = deauth_req.serialize();
        deauth_hc1.success_feedback=(it->_id != INVALID_FILEID)?FEEDBACK_DELETE_VMSHAREIMAGE:FEEDBACK_DELETE_VMSHAREBLOCK;
        vec_deauth_hc1.push_back(deauth_hc1);
    }

    //构造正向之前先构造回滚
    /***********************************回滚流***********************************/
    //回滚
    VmAction remove_vm(VM_WF_CATEGORY, VM_ACT_REMOVE, EV_VM_OP, LCM, HC);
    remove_vm._vid = vid;
    live_migrate._operation     = CANCEL;
    live_migrate.action_id      = remove_vm.get_id();
    live_migrate.retry          = true;
    remove_vm.message_req       = live_migrate.serialize();
    remove_vm.skipped           = true;
    wf_rollback.Add(remove_vm, vec_deauth_hc2);

    //回滚
    VmAction create_vm(VM_WF_CATEGORY, VM_ACT_CREATE, EV_VM_OP, LCM, HC);
    create_vm._vid = vid;
    live_migrate._operation     = CREATE;
    live_migrate.action_id      = create_vm.get_id();
    live_migrate.retry          = true;
    create_vm.message_req       = live_migrate.serialize();
    create_vm.skipped           = true;
    wf_rollback.Add(remove_vm, create_vm);

    live_migrate.retry           = false;

    //回滚
    VmAction failure_nic(VM_WF_CATEGORY, VM_ACT_END_M_NIC, EV_VNETLIB_VM_MIGRATE_END, LCM, MID(MU_VNET_LIBNET));
    failure_nic._vid                 = vid;
        CVNetVmMigrateEndMsg    failure_nic_req;
        failure_nic_req.m_vm_id           = vid;
        failure_nic_req.m_src_cc_application    = failure_nic_req.m_dst_cc_application = cc_name;
        failure_nic_req.m_src_hc_application    = src_hc_name;
        failure_nic_req.m_result                = 1;    //热迁移失败了
        failure_nic_req.retry                   = true;
    failure_nic_req.action_id         = failure_nic.get_id();
    failure_nic.message_req = failure_nic_req.serialize();
    failure_nic.skipped           = true;
    wf_rollback.Add(failure_nic);

    VmAction unselect_hid_next(VM_WF_CATEGORY, VM_ACT_UNSELECT_HC, EV_VM_UNSELECT_HOST_NEXT, LCM, LCM);
    unselect_hid_next._vid = vid;
    migrate_req.action_id = unselect_hid_next.get_id();
    unselect_hid_next.message_req   = migrate_req.serialize();
    unselect_hid_next.skipped       = true;
    wf_rollback.Add(vec_deauth_hc2, unselect_hid_next);
    wf_rollback.Add(create_vm, unselect_hid_next);
    wf_rollback.Add(failure_nic, unselect_hid_next);

    /***********************************正向流***********************************/
    //下面这个是调度ACTION，没有回滚
    VmAction select_host_by_nic(VM_WF_CATEGORY, VM_ACT_SELECT_HC_BY_NIC, EV_VNETLIB_SELECT_MIGRATE_HC, LCM, MID(MU_VNET_LIBNET));
    select_host_by_nic._vid                 = vid;
        CVNetSelectHCListMsg    select_hc_req;
        select_hc_req.action_id         = select_host_by_nic.get_id();
        select_hc_req.m_vm_id           = vid;
        select_hc_req.m_project_id      = pvm->_project_id;
        select_hc_req.m_cc_application  = cc_name;
        select_hc_req.m_WatchDogInfo    = CWatchDogInfo(pvm->_config._vm_wdtime != 0, pvm->_config._vm_wdtime);
        select_hc_req.m_VmNicList.assign(pvm->_config._nics.begin(), pvm->_config._nics.end());
    select_host_by_nic.message_req  = select_hc_req.serialize();
    select_host_by_nic.skipped      = skip_nic;
    select_host_by_nic.failure_feedback = FEEDBACK_ROLLBACK_WORKFLOW;
    wf_migrate.Add(select_host_by_nic);

    //由SCH选择新的HC
    VmAction select_hid_next(VM_WF_CATEGORY, VM_ACT_SELECT_HC, EV_VM_MIGRATE, LCM, SCH);
    select_hid_next._vid = vid;
    select_hid_next.rollback_action_id  = unselect_hid_next.get_id();
    migrate_req.action_id = select_hid_next.get_id();
    select_hid_next.message_req = migrate_req.serialize();
    select_hid_next.failure_feedback = FEEDBACK_ROLLBACK_WORKFLOW;
    wf_migrate.Add(select_host_by_nic, select_hid_next);

    wf_migrate.Add(select_hid_next, vec_auth_hc2);

    //正向
    VmAction prepare_nic(VM_WF_CATEGORY, VM_ACT_BEGIN_M_NIC, EV_VNETLIB_VM_MIGRATE_BEGIN, LCM, MID(MU_VNET_LIBNET));
    prepare_nic._vid                 = vid;
        CVNetVmMigrateBeginMsg    prepare_nic_req;
        prepare_nic_req.m_vm_id           = vid;
        prepare_nic_req.m_src_cc_application    = prepare_nic_req.m_dst_cc_application = cc_name;
        prepare_nic_req.m_src_hc_application    = src_hc_name;
    prepare_nic_req.action_id         = prepare_nic.get_id();
    prepare_nic.message_req = prepare_nic_req.serialize();
    prepare_nic.rollback_action_id = failure_nic.get_id();
    prepare_nic.skipped             = skip_nic;
    prepare_nic.failure_feedback    = FEEDBACK_ROLLBACK_WORKFLOW;
    wf_migrate.Add(select_hid_next, prepare_nic);

    //正向
    VmAction prepare_m(VM_WF_CATEGORY, VM_ACT_PRAPARE_M, EV_VM_OP, LCM, HC);
    prepare_m._vid              = vid;
    live_migrate._operation     = PREP_M;
    live_migrate.action_id      = prepare_m.get_id();
    prepare_m.message_req       = live_migrate.serialize();
    prepare_m.rollback_action_id= remove_vm.get_id();
    prepare_m.failure_feedback  = FEEDBACK_ROLLBACK_WORKFLOW;
    prepare_m.timeout           = timeout;
    prepare_m.timeout_feedback  = FEEDBACK_ROLLBACK_WORKFLOW;
    wf_migrate.Add(prepare_nic, prepare_m);
    wf_migrate.Add(vec_auth_hc2, prepare_m);

    //正向
    VmAction migrate(VM_WF_CATEGORY, VM_ACT_MIGRATE, EV_VM_OP, LCM, HC);
    migrate._vid              = vid;
    live_migrate._operation     = MIGRATE;
    live_migrate.action_id      = migrate.get_id();
    migrate.message_req       = live_migrate.serialize();
    migrate.rollback_action_id= create_vm.get_id();
    migrate.failure_feedback  = FEEDBACK_ROLLBACK_WORKFLOW;
    migrate.timeout           = timeout;
    migrate.timeout_feedback  = FEEDBACK_ROLLBACK_WORKFLOW;
    wf_migrate.Add(prepare_m, migrate);

    //正向
    VmAction post_m(VM_WF_CATEGORY, VM_ACT_POST_M, EV_VM_OP, LCM, HC);
    post_m._vid              = vid;
    live_migrate._operation     = POST_M;
    live_migrate.action_id      = post_m.get_id();
    post_m.message_req       = live_migrate.serialize();
    post_m.success_feedback  = FEEDBACK_MIGRATE_SUCCESS;
    post_m.failure_feedback  = FEEDBACK_ROLLBACK_WORKFLOW;
    post_m.timeout           = timeout;
    post_m.timeout_feedback  = FEEDBACK_ROLLBACK_WORKFLOW;
    wf_migrate.Add(migrate, post_m);

    //正向
    VmAction success_nic(VM_WF_CATEGORY, VM_ACT_END_M_NIC, EV_VNETLIB_VM_MIGRATE_END, LCM, MID(MU_VNET_LIBNET));
    success_nic._vid                 = vid;
        CVNetVmMigrateEndMsg    success_nic_req;
        success_nic_req.m_vm_id           = vid;
        success_nic_req.m_src_cc_application    = success_nic_req.m_dst_cc_application = cc_name;
        success_nic_req.m_src_hc_application    = src_hc_name;
        success_nic_req.m_result                = 0;
        success_nic_req.retry                   = true;
    success_nic_req.action_id         = success_nic.get_id();
    success_nic.message_req = success_nic_req.serialize();
    success_nic.skipped             = skip_nic;
    wf_migrate.Add(post_m, success_nic);

    wf_migrate.Add(post_m, vec_deauth_hc1);

    STATUS ret = CreateWorkflow(wf_migrate, wf_rollback);
    if(SUCCESS != ret)
    {
        Debug(SYS_WARNING, "interface failed to created workflow: %s!!!\n", wf_migrate.get_id().c_str());
        SkyAssert(false);
    }
    else
    {
        ret = ERROR_ACTION_RUNNING;
    }

    return ret;
}

//web上，只允许开机状态冷迁移
//HA时，未知状态也允许冷迁移
STATUS VMManager::ColdMigrate(const VmOperationReq &req, const string &req_info, const MID &sender, ostringstream& error )
{
    if ( !IsVMSupportMigrate( req._vid ) )
    {/* 如不支持迁移，则返回不支持 */
        error << "This vm(id = "<< req._vid << ") is not supported migrate.";
        Debug(SYS_WARNING,"%s\n",error.str().c_str());
        return ERROR_NOT_SUPPORT;
    }

    VmMigrateReq migrate_req;
    bool result = migrate_req.deserialize(req_info);
    SkyAssert(result);

    int64 vid = req._vid;

    VirtualMachine *pvm = vm_pool->Get(vid, false);
    SkyAssert(pvm);

    ostringstream where;
    int state = VM_UNKNOWN;
    where << "vid = " << vid;
    vm_pool->SelectColumn("web_view_vmstate", "state", where.str(), state);
    if ((state != VM_SHUTOFF) && (state != VM_UNKNOWN))
    {
        error << "Cold migrate is forbidden when " << Enum2Str::InstStateStr(state) <<" state.";
        Debug(SYS_WARNING,"%s\n",error.str().c_str());
        return ERROR_NOT_SUPPORT;
    }

    MID SC = GetSC_MID();
    MID LCM = MID(MU_VM_LIFECYCLE_MANAGER);
    MID SCH = MID(MU_VM_SCHEDULER);
    const string src_hc_name = host_pool->GetHostNameById(pvm->_hid);
    const string host_ip = host_pool->GetHostIpById(pvm->_hid);
    const string iscsiname = host_pool->GetIscsinameById(pvm->_hid);
    MID HC(src_hc_name, PROC_HC, MU_VM_HANDLER);  //此时源端已知，目的端未知
    const string cc_name = ApplicationName();
    const bool skip_nic = !IsNeedVNet(pvm->_config);    //没有虚拟网卡的时候，不需要向NET发消息
    const bool skip_hc1 = host_pool->IsHostOffline(pvm->_hid);
    const int timeout = 60*10;   //10分钟超时

    //构建热迁移的数据
    VmMigrateData createreq(vid, CREATE, "");
    VmAssistant->ConstructCreateReq(vid, createreq);

    VmWorkFlow wf_migrate;
    wf_migrate.category             = VM_WF_CATEGORY;
    wf_migrate.name                 = VM_WF_COLD_MIGRATE;
    wf_migrate._vid                 = vid;
    wf_migrate._operation           = COLD_MIGRATE;
    wf_migrate.upstream_sender      = sender;
    wf_migrate.upstream_action_id   = req.action_id;
    if(!req._user.empty())
    {
        wf_migrate.originator           = req._user;
    }

    VmWorkFlow  wf_rollback;
    wf_rollback.category            = VM_WF_CATEGORY;
    wf_rollback.name                = VM_WF_COLD_MIGRATE;
    wf_rollback._vid                = vid;
    wf_rollback._operation          = COLD_MIGRATE;
    wf_rollback.upstream_sender     = sender;
    wf_rollback.upstream_action_id  = req.action_id;
    if(!req._user.empty())
    {
        wf_rollback.originator          = req._user;
    }

    //因为存储相关的action需要遍历disks，所以，先构造存储自身action的参数
    vector<Action> vec_auth_hc1, vec_auth_hc2, vec_deauth_hc1, vec_deauth_hc2;

    vector<VmDiskConfig> disks = pvm->_config._disks;
    disks.push_back(pvm->_config._machine);
    //disks.push_back(deploy._config._kernel);
    //disks.push_back(deploy._config._initrd);

    vector<VmDiskConfig>::iterator it;
    for (it = disks.begin(); it != disks.end(); it++)
    {
        if (it->_position != VM_STORAGE_TO_SHARE)
        {
            continue;
        }

        AuthBlockReq auth_req;
        DeAuthBlockReq deauth_req;
        auth_req._vid           = deauth_req._vid           = vid;
        auth_req._request_id    = deauth_req._request_id    = it->_request_id;
        auth_req._cluster_name  = deauth_req._cluster_name  = cc_name;
        auth_req._userip        = deauth_req._userip        = host_ip;
        auth_req._iscsiname     = deauth_req._iscsiname     = iscsiname;
        //auth_req._usage  = deauth_req._usage = to_string<int64>(vid,dec)+":"+it->_target; usage don't set

        //回滚
        VmAction auth_hc1(STORAGE_CATEGORY, VM_ACT_AUTH_BLOCK, EV_STORAGE_AUTH_BLOCK_REQ, LCM, SC);
        auth_hc1._vid = vid;
            auth_req.retry          = true;
            auth_req.action_id      = auth_hc1.get_id();
        auth_hc1.message_req = auth_req.serialize();
        auth_hc1.skipped = true;
        auth_hc1.success_feedback=(it->_id != INVALID_FILEID)?FEEDBACK_COLDMIGRATE_ADD_VMSHAREIMAGE:FEEDBACK_COLDMIGRATE_ADD_VMSHAREBLOCK;
        vec_auth_hc1.push_back(auth_hc1);

        //回滚
        VmAction deauth_hc2(STORAGE_CATEGORY, VM_ACT_DEAUTH_BLOCK, EV_STORAGE_DEAUTH_BLOCK_REQ, LCM, SC);
        deauth_hc2._vid = vid;
            deauth_req.retry        = true;
            deauth_req.action_id    = deauth_hc2.get_id();
        deauth_hc2.message_req = deauth_req.serialize();
        deauth_hc2.skipped = true;
        deauth_hc2.success_feedback=(it->_id != INVALID_FILEID)?FEEDBACK_DELETE_VMSHAREIMAGE:FEEDBACK_DELETE_VMSHAREBLOCK;
        vec_deauth_hc2.push_back(deauth_hc2);

        //正向
        VmAction auth_hc2(STORAGE_CATEGORY, VM_ACT_AUTH_BLOCK, EV_STORAGE_AUTH_BLOCK_REQ, LCM, SC);
        auth_hc2._vid = vid;
            auth_req.retry          = false;
            auth_req.action_id = auth_hc2.get_id();
        auth_hc2.message_req = auth_req.serialize();
        auth_hc2.rollback_action_id = deauth_hc2.get_id();
        auth_hc2.failure_feedback = FEEDBACK_ROLLBACK_WORKFLOW;
        auth_hc2.success_feedback=(it->_id != INVALID_FILEID)?FEEDBACK_COLDMIGRATE_ADD_VMSHAREIMAGE:FEEDBACK_COLDMIGRATE_ADD_VMSHAREBLOCK;
        vec_auth_hc2.push_back(auth_hc2);

        //正向
        VmAction deauth_hc1(STORAGE_CATEGORY, VM_ACT_DEAUTH_BLOCK, EV_STORAGE_DEAUTH_BLOCK_REQ, LCM, SC);
        deauth_hc1._vid = vid;
            deauth_req.retry        = true;
            deauth_req.action_id    = deauth_hc1.get_id();
        deauth_hc1.message_req  = deauth_req.serialize();
        deauth_hc1.rollback_action_id = auth_hc1.get_id();
        deauth_hc1.success_feedback=(it->_id != INVALID_FILEID)?FEEDBACK_DELETE_VMSHAREIMAGE:FEEDBACK_DELETE_VMSHAREBLOCK;
        vec_deauth_hc1.push_back(deauth_hc1);
    }

    //构造正向之前先构造回滚
    /***********************************回滚流***********************************/
    CVNetGetResourceHCMsg       prepare_nic_req;
    CVNetFreeResourceHCMsg      release_nic_req;
    prepare_nic_req.m_vm_id             = release_nic_req.m_vm_id           = vid;
    prepare_nic_req.m_project_id        = release_nic_req.m_project_id      = pvm->_project_id;
    prepare_nic_req.m_cc_application    = release_nic_req.m_cc_application  = cc_name;
    prepare_nic_req.m_hc_application    = release_nic_req.m_hc_application  = src_hc_name;
    prepare_nic_req.m_WatchDogInfo      = release_nic_req.m_WatchDogInfo    = CWatchDogInfo(pvm->_config._vm_wdtime != 0, pvm->_config._vm_wdtime);
    prepare_nic_req.m_VmNicCfgList.assign(pvm->_config._nics.begin(), pvm->_config._nics.end());
    release_nic_req.m_VmNicCfgList      = prepare_nic_req.m_VmNicCfgList;

    //回滚
    VmAction remove_vm_hc2(VM_WF_CATEGORY, VM_ACT_REMOVE, EV_VM_OP, LCM, HC);
    remove_vm_hc2._vid = vid;
        createreq.retry         = true;
        createreq._operation    = CANCEL;
        createreq.action_id     = remove_vm_hc2.get_id();
    remove_vm_hc2.message_req       = createreq.serialize();
    remove_vm_hc2.skipped           = true;
    wf_rollback.Add(remove_vm_hc2, vec_auth_hc1);

    if (!(vec_auth_hc1.empty() || vec_deauth_hc2.empty()))
    {
        wf_rollback.Add(vec_auth_hc1, vec_deauth_hc2);
    }

    //回滚
    VmAction release_nic_hc2(VM_WF_CATEGORY, VM_ACT_RELEASE_NIC, EV_VNETLIB_FREERESOURCE_HC, LCM, MID(MU_VNET_LIBNET));
    release_nic_hc2._vid        = vid;
        release_nic_req.retry       = true;
        release_nic_req.action_id   = release_nic_hc2.get_id();
    release_nic_hc2.message_req = release_nic_req.serialize();
    release_nic_hc2.skipped     = true;
    wf_rollback.Add(remove_vm_hc2, release_nic_hc2);

    //回滚
    VmAction prepare_nic_hc1(VM_WF_CATEGORY, VM_ACT_PREPARE_NIC, EV_VNETLIB_GETRESOURCE_HC, LCM, MID(MU_VNET_LIBNET));
    prepare_nic_hc1._vid                 = vid;
        prepare_nic_req.retry       = true;
        prepare_nic_req.action_id   = prepare_nic_hc1.get_id();
    prepare_nic_hc1.message_req = prepare_nic_req.serialize();
    prepare_nic_hc1.skipped     = true;
    wf_rollback.Add(release_nic_hc2, prepare_nic_hc1);

    VmAction prepare_m_hc1(VM_WF_CATEGORY, VM_ACT_PRAPARE_M, EV_VM_OP, LCM, HC);
    prepare_m_hc1._vid              = vid;
        createreq.retry         = true;
        createreq._operation    = PREP_M;
        createreq.action_id     = prepare_m_hc1.get_id();
    prepare_m_hc1.message_req       = createreq.serialize();
    prepare_m_hc1.skipped           = true;
    wf_rollback.Add(prepare_nic_hc1, prepare_m_hc1);
    wf_rollback.Add(vec_deauth_hc2, prepare_m_hc1);

    VmAction unselect_hid_next(VM_WF_CATEGORY, VM_ACT_UNSELECT_HC, EV_VM_UNSELECT_HOST_NEXT, LCM, LCM);
    unselect_hid_next._vid = vid;
        migrate_req.action_id = unselect_hid_next.get_id();
    unselect_hid_next.message_req   = migrate_req.serialize();
    unselect_hid_next.skipped       = true;
    wf_rollback.Add(prepare_m_hc1, unselect_hid_next);

    /***********************************正向流***********************************/
    //下面这个是调度ACTION，没有回滚
    VmAction select_host_by_nic(VM_WF_CATEGORY, VM_ACT_SELECT_HC_BY_NIC, EV_VNETLIB_SELECT_HC, LCM, MID(MU_VNET_LIBNET));
    select_host_by_nic._vid                 = vid;
        CVNetSelectHCListMsg    select_hc_req;
        select_hc_req.action_id         = select_host_by_nic.get_id();
        select_hc_req.m_vm_id           = vid;
        select_hc_req.m_project_id      = pvm->_project_id;
        select_hc_req.m_cc_application  = cc_name;
        select_hc_req.m_WatchDogInfo    = CWatchDogInfo(pvm->_config._vm_wdtime != 0, pvm->_config._vm_wdtime);
        select_hc_req.m_VmNicList.assign(pvm->_config._nics.begin(), pvm->_config._nics.end());
    select_host_by_nic.message_req  = select_hc_req.serialize();
    select_host_by_nic.skipped      = skip_nic;
    wf_migrate.Add(select_host_by_nic);

    //由SCH选择新的HC
    VmAction select_hid_next(VM_WF_CATEGORY, VM_ACT_SELECT_HC, EV_VM_COLD_MIGRATE, LCM, SCH);
    select_hid_next._vid = vid;
        migrate_req.action_id = select_hid_next.get_id();
    select_hid_next.message_req = migrate_req.serialize();
    select_hid_next.rollback_action_id  = unselect_hid_next.get_id();
    select_hid_next.failure_feedback = FEEDBACK_ROLLBACK_WORKFLOW;
    wf_migrate.Add(select_host_by_nic, select_hid_next);

    VmAction remove_vm_hc1(VM_WF_CATEGORY, VM_ACT_REMOVE, EV_VM_OP, LCM, HC);
    remove_vm_hc1._vid = vid;
        createreq.retry         = true;
        createreq._operation    = CANCEL;
        createreq.action_id     = remove_vm_hc1.get_id();
    remove_vm_hc1.message_req       = createreq.serialize();
    remove_vm_hc1.rollback_action_id  = prepare_m_hc1.get_id();
    remove_vm_hc1.skipped           = skip_hc1;
    wf_migrate.Add(select_hid_next, remove_vm_hc1);

    VmAction release_nic_hc1(VM_WF_CATEGORY, VM_ACT_RELEASE_NIC, EV_VNETLIB_FREERESOURCE_HC, LCM, MID(MU_VNET_LIBNET));
    release_nic_hc1._vid        = vid;
        release_nic_req.retry       = true;
        release_nic_req.action_id   = release_nic_hc1.get_id();
    release_nic_hc1.message_req = release_nic_req.serialize();
    release_nic_hc1.rollback_action_id  = prepare_nic_hc1.get_id();
    release_nic_hc1.skipped     = skip_nic;
    wf_migrate.Add(remove_vm_hc1, release_nic_hc1);

    VmAction prepare_nic_hc2(VM_WF_CATEGORY, VM_ACT_PREPARE_NIC, EV_VNETLIB_GETRESOURCE_HC, LCM, MID(MU_VNET_LIBNET));
    prepare_nic_hc2._vid                 = vid;
        prepare_nic_req.retry       = false;
        prepare_nic_req.action_id   = prepare_nic_hc2.get_id();
    prepare_nic_hc2.message_req = prepare_nic_req.serialize();
    prepare_nic_hc2.failure_feedback = FEEDBACK_ROLLBACK_WORKFLOW;
    prepare_nic_hc2.rollback_action_id  = release_nic_hc2.get_id();
    prepare_nic_hc2.skipped     = skip_nic;
    wf_migrate.Add(release_nic_hc1, prepare_nic_hc2);

    wf_migrate.Add(remove_vm_hc1, vec_auth_hc2);

    if (!(vec_auth_hc2.empty() || vec_deauth_hc1.empty()))
    {
        wf_migrate.Add(vec_auth_hc2, vec_deauth_hc1);
    }

    VmAction prepare_m_hc2(VM_WF_CATEGORY, VM_ACT_PRAPARE_M, EV_VM_OP, LCM, HC);
    prepare_m_hc2._vid              = vid;
        createreq._operation    = PREP_M;
        createreq.retry         = false;
        createreq.action_id     = prepare_m_hc2.get_id();
    prepare_m_hc2.message_req       = createreq.serialize();
    prepare_m_hc2.success_feedback  = FEEDBACK_MIGRATE_SUCCESS;
    prepare_m_hc2.failure_feedback  = FEEDBACK_ROLLBACK_WORKFLOW;
    prepare_m_hc2.timeout           = timeout;
    prepare_m_hc2.timeout_feedback  = FEEDBACK_ROLLBACK_WORKFLOW;
    prepare_m_hc2.rollback_action_id= remove_vm_hc2.get_id();
    wf_migrate.Add(prepare_nic_hc2, prepare_m_hc2);
    wf_migrate.Add(vec_deauth_hc1, prepare_m_hc2);

    STATUS ret = CreateWorkflow(wf_migrate, wf_rollback);
    if(SUCCESS != ret)
    {
        Debug(SYS_WARNING, "interface failed to created workflow: %s!!!\n", wf_migrate.get_id().c_str());
        SkyAssert(false);
    }
    else
    {
        ret = ERROR_ACTION_RUNNING;
    }

    return ret;
}

STATUS VMManager::Freeze(const VmOperationReq& req, const MID sender, ostringstream& error)
{
    int64 vid = req._vid;

    VmWorkFlow wf_freeze;
    wf_freeze.category             = VM_WF_CATEGORY;
    wf_freeze.name                 = VM_WF_FREEZE;
    wf_freeze._vid                 = vid;
    wf_freeze._operation           = FREEZE;
    wf_freeze.upstream_sender      = sender;
    wf_freeze.upstream_action_id   = req.action_id;
    if(!req._user.empty())
    {
        wf_freeze.originator           = req._user;
    }

    bool is_support_cl = IsVMSupportMigrate(vid);   //是否支持冷迁移

    VirtualMachine *pvm = vm_pool->Get(vid, false);
    SkyAssert(pvm);

    const string cc_name = ApplicationName();
    const string host_ip = host_pool->GetHostIpById(pvm->_hid);
    const string hc_name = host_pool->GetHostNameById(pvm->_hid);
    const string iscsiname = host_pool->GetIscsinameById(pvm->_hid);

    MID SC = GetSC_MID();
    MID LCM = MID(MU_VM_LIFECYCLE_MANAGER);
    MID HC(hc_name, PROC_HC, MU_VM_HANDLER);

    //HC上删除虚拟机
    VmAction remove_vm(VM_WF_CATEGORY, VM_ACT_REMOVE, EV_VM_OP, LCM, HC);
    remove_vm._vid = vid;
        VmCreateReq cancelreq(vid, CANCEL);
        VmAssistant->ConstructCreateReq(vid,cancelreq);
        cancelreq.retry         = true;
        cancelreq.action_id     = remove_vm.get_id();
    remove_vm.message_req       = cancelreq.serialize();
    remove_vm.skipped           = !is_support_cl;
    wf_freeze.Add(remove_vm);

    //CC上释放网络
    VmAction release_nic(VM_WF_CATEGORY, VM_ACT_RELEASE_NIC, EV_VNETLIB_FREERESOURCE_HC, LCM, MID(MU_VNET_LIBNET));
    release_nic._vid        = vid;
        CVNetFreeResourceHCMsg      release_nic_req;
        release_nic_req.m_vm_id           = vid;
        release_nic_req.m_project_id      = pvm->_project_id;
        release_nic_req.m_cc_application  = cc_name;
        release_nic_req.m_hc_application  = hc_name;
        release_nic_req.m_WatchDogInfo    = CWatchDogInfo(pvm->_config._vm_wdtime != 0, pvm->_config._vm_wdtime);
        release_nic_req.m_VmNicCfgList.assign(pvm->_config._nics.begin(), pvm->_config._nics.end());
        release_nic_req.retry       = true;
        release_nic_req.action_id   = release_nic.get_id();
    release_nic.message_req = release_nic_req.serialize();
    release_nic.skipped     = !IsNeedVNet(pvm->_config);
    wf_freeze.Add(remove_vm, release_nic);

    //CC上deauth共享存储
    vector<Action> vec_deauth;

    vector<VmDiskConfig> disks = pvm->_config._disks;
    disks.push_back(pvm->_config._machine);
    //disks.push_back(deploy._config._kernel);
    //disks.push_back(deploy._config._initrd);

    vector<VmDiskConfig>::iterator it;
    for (it = disks.begin(); it != disks.end(); it++)
    {
        if (it->_position != VM_STORAGE_TO_SHARE)
        {
            continue;
        }

        DeAuthBlockReq deauth_req;
        deauth_req._vid           = vid;
        deauth_req._request_id    = it->_request_id;
        deauth_req._cluster_name  = cc_name;
        deauth_req._userip        = host_ip;
        deauth_req._iscsiname     = iscsiname;
        deauth_req._usage         = to_string<int64>(vid,dec)+":"+it->_target;

        VmAction deauth(STORAGE_CATEGORY, VM_ACT_DEAUTH_BLOCK, EV_STORAGE_DEAUTH_BLOCK_REQ, LCM, SC);
        deauth._vid = vid;
            deauth_req.retry        = true;
            deauth_req.action_id    = deauth.get_id();
        deauth.message_req  = deauth_req.serialize();
        deauth.skipped      = !is_support_cl;
        deauth.success_feedback=(it->_id != INVALID_FILEID)?FEEDBACK_DELETE_VMSHAREIMAGE : FEEDBACK_DELETE_VMSHAREBLOCK;
        vec_deauth.push_back(deauth);
    }
    wf_freeze.Add(remove_vm, vec_deauth);

    VmAction finish_freeze(VM_WF_CATEGORY, VM_ACT_FREEZE, EV_VM_FINISH_FREEZE, LCM, LCM);
    finish_freeze._vid = vid;
        VmOperationReq freeze_req(vid, FREEZE);
        freeze_req.action_id = finish_freeze.get_id();
    finish_freeze.message_req = freeze_req.serialize();
    wf_freeze.Add(release_nic, finish_freeze);
    wf_freeze.Add(vec_deauth, finish_freeze);

    STATUS ret = CreateWorkflow(wf_freeze);
    if(SUCCESS != ret)
    {
        Debug(SYS_WARNING, "interface failed to created workflow: %s!!!\n", wf_freeze.get_id().c_str());
        SkyAssert(false);
    }
    else
    {
        ret = ERROR_ACTION_RUNNING;
    }

    return ret;
}

#define ConstructModifyAction(_action, _func) \
    VmAction _action(VM_WF_CATEGORY, VM_ACT_MODIFY_NEXT, EV_VM_MODIFY_NEXT_ACTION, LCM, LCM); \
    _action._vid = vid; \
        WorkReq _action##_req(_action.get_id()); \
    _action.message_req         = _action##_req.serialize(); \
    _action.success_feedback    = _func;

STATUS VMManager::UnFreeze(const VmOperationReq& req, const MID sender, ostringstream& error)
{
    int64 vid = req._vid;

    VmWorkFlow wf_freeze;   //回滚流
    wf_freeze.category             = VM_WF_CATEGORY;
    wf_freeze.name                 = VM_WF_UNFREEZE;
    wf_freeze._vid                 = vid;
    wf_freeze._operation           = UNFREEZE;
    wf_freeze.upstream_sender      = sender;
    wf_freeze.upstream_action_id   = req.action_id;
    if(!req._user.empty())
    {
        wf_freeze.originator           = req._user;
    }

    VirtualMachine *pvm = vm_pool->Get(vid, false);
    SkyAssert(pvm);

    const string cc_name = ApplicationName();

    MID SC = GetSC_MID();
    MID SCH = MID(MU_VM_SCHEDULER);
    MID LCM = MID(MU_VM_LIFECYCLE_MANAGER);
    MID HC("", PROC_HC, MU_VM_HANDLER);

    const bool skip_nic = !IsNeedVNet(pvm->_config);    //没有虚拟网卡的时候，不需要向NET发消息

    //修改回滚流的 REMOVE_VM 的参数
    ConstructModifyAction(modify_remove_vm, PREDECESSOR_SET_REQ_FOR_REMOVE_VM);
    modify_remove_vm.skipped    = true;
    wf_freeze.Add(modify_remove_vm);

    //HC上删除虚拟机
    VmAction remove_vm(VM_WF_CATEGORY, VM_ACT_REMOVE, EV_VM_OP, LCM, HC);
    remove_vm._vid = vid;
        VmCreateReq cancelreq(vid, CANCEL);
        VmAssistant->ConstructCreateReq(vid,cancelreq);
        cancelreq.retry         = true;
        cancelreq.action_id     = remove_vm.get_id();
    remove_vm.message_req       = cancelreq.serialize();
    remove_vm.skipped           = true;
    wf_freeze.Add(modify_remove_vm, remove_vm);

    //CC上释放网络
    CVNetGetResourceHCMsg       prepare_nic_req;
    CVNetFreeResourceHCMsg      release_nic_req;
    prepare_nic_req.m_vm_id             = release_nic_req.m_vm_id           = vid;
    prepare_nic_req.m_project_id        = release_nic_req.m_project_id      = pvm->_project_id;
    prepare_nic_req.m_cc_application    = release_nic_req.m_cc_application  = cc_name;
    prepare_nic_req.m_WatchDogInfo      = release_nic_req.m_WatchDogInfo    = CWatchDogInfo(pvm->_config._vm_wdtime != 0, pvm->_config._vm_wdtime);
    prepare_nic_req.m_VmNicCfgList.assign(pvm->_config._nics.begin(), pvm->_config._nics.end());
    release_nic_req.m_VmNicCfgList      = prepare_nic_req.m_VmNicCfgList;

    VmAction release_nic(VM_WF_CATEGORY, VM_ACT_RELEASE_NIC, EV_VNETLIB_FREERESOURCE_HC, LCM, MID(MU_VNET_LIBNET));
    release_nic._vid        = vid;
        release_nic_req.retry       = true;
        release_nic_req.action_id   = release_nic.get_id();
    release_nic.message_req = release_nic_req.serialize();
    release_nic.skipped     = true;
    wf_freeze.Add(remove_vm, release_nic);

    //CC上deauth共享存储
    vector<Action> vec_auth_hc2, vec_deauth_hc2;

    vector<VmDiskConfig> disks = pvm->_config._disks;
    disks.push_back(pvm->_config._machine);
    //disks.push_back(deploy._config._kernel);
    //disks.push_back(deploy._config._initrd);

    vector<VmDiskConfig>::iterator it;
    for (it = disks.begin(); it != disks.end(); it++)
    {
        if (it->_position != VM_STORAGE_TO_SHARE)
        {
            continue;
        }

        AuthBlockReq auth_req;
        DeAuthBlockReq deauth_req;
        auth_req._vid           = deauth_req._vid           = vid;
        auth_req._request_id    = deauth_req._request_id    = it->_request_id;
        auth_req._cluster_name  = deauth_req._cluster_name  = cc_name;
        auth_req._usage  = deauth_req._usage = to_string<int64>(vid,dec)+":"+it->_target;

        //回滚
        VmAction deauth_hc2(STORAGE_CATEGORY, VM_ACT_DEAUTH_BLOCK, EV_STORAGE_DEAUTH_BLOCK_REQ, LCM, SC);
        deauth_hc2._vid = vid;
            deauth_req.retry        = true;
            deauth_req.action_id    = deauth_hc2.get_id();
        deauth_hc2.message_req  = deauth_req.serialize();
        deauth_hc2.skipped      = true;
        deauth_hc2.success_feedback=(it->_id != INVALID_FILEID)?FEEDBACK_DELETE_VMSHAREIMAGE : FEEDBACK_DELETE_VMSHAREBLOCK;
        vec_deauth_hc2.push_back(deauth_hc2);

        //正向
        VmAction auth_hc2(STORAGE_CATEGORY, VM_ACT_AUTH_BLOCK, EV_STORAGE_AUTH_BLOCK_REQ, LCM, SC);
        auth_hc2._vid = vid;
            auth_req.retry          = false;
            auth_req.action_id = auth_hc2.get_id();
        auth_hc2.message_req = auth_req.serialize();
        auth_hc2.rollback_action_id = deauth_hc2.get_id();
        auth_hc2.failure_feedback = FEEDBACK_ROLLBACK_WORKFLOW;
        auth_hc2.success_feedback=(it->_id != INVALID_FILEID)?FEEDBACK_UPDATE_VMSHAREIMAGE : FEEDBACK_UPDATE_VMSHAREBLOCK;
        vec_auth_hc2.push_back(auth_hc2);
    }
    wf_freeze.Add(remove_vm, vec_deauth_hc2);

    VmAction finish_freeze(VM_WF_CATEGORY, VM_ACT_FREEZE, EV_VM_FINISH_FREEZE, LCM, LCM);
    finish_freeze._vid = vid;
        VmOperationReq freeze_req(vid, FREEZE);
        freeze_req.action_id = finish_freeze.get_id();
    finish_freeze.message_req   = freeze_req.serialize();
    finish_freeze.skipped       = true;
    wf_freeze.Add(release_nic, finish_freeze);
    wf_freeze.Add(vec_deauth_hc2, finish_freeze);

    //正向流
    VmWorkFlow wf_unfreeze;   //回滚流
    wf_unfreeze.category             = VM_WF_CATEGORY;
    wf_unfreeze.name                 = VM_WF_UNFREEZE;
    wf_unfreeze._vid                 = vid;
    wf_unfreeze._operation           = UNFREEZE;
    wf_unfreeze.upstream_sender      = sender;
    wf_unfreeze.upstream_action_id   = req.action_id;
    if(!req._user.empty())
    {
        wf_unfreeze.originator           = req._user;
    }

    /***********************************正向流***********************************/
    //下面这个是调度ACTION，没有回滚
    VmAction select_host_by_nic(VM_WF_CATEGORY, VM_ACT_SELECT_HC_BY_NIC, EV_VNETLIB_SELECT_HC, LCM, MID(MU_VNET_LIBNET));
    select_host_by_nic._vid                 = vid;
        CVNetSelectHCListMsg    select_hc_req;
        select_hc_req.action_id         = select_host_by_nic.get_id();
        select_hc_req.m_vm_id           = vid;
        select_hc_req.m_project_id      = pvm->_project_id;
        select_hc_req.m_cc_application  = cc_name;
        select_hc_req.m_WatchDogInfo    = CWatchDogInfo(pvm->_config._vm_wdtime != 0, pvm->_config._vm_wdtime);
        select_hc_req.m_VmNicList.assign(pvm->_config._nics.begin(), pvm->_config._nics.end());
    select_host_by_nic.message_req  = select_hc_req.serialize();
    select_host_by_nic.skipped      = skip_nic;
    wf_unfreeze.Add(select_host_by_nic);

    //由SCH选择新的HC
    VmAction reselect_hid(VM_WF_CATEGORY, VM_ACT_SELECT_HC, EV_VM_RESCHEDULE_SINGLE, LCM, SCH);
    reselect_hid._vid = vid;
        VmOperationReq send_req(vid, UNFREEZE);
        send_req.action_id = reselect_hid.get_id();
    reselect_hid.message_req = send_req.serialize();
    reselect_hid.rollback_action_id  = finish_freeze.get_id();
    reselect_hid.failure_feedback = FEEDBACK_ROLLBACK_WORKFLOW;
    wf_unfreeze.Add(select_host_by_nic, reselect_hid);

    VmAction prepare_nic(VM_WF_CATEGORY, VM_ACT_PREPARE_NIC, EV_VNETLIB_GETRESOURCE_HC, LCM, MID(MU_VNET_LIBNET));
    prepare_nic._vid                 = vid;
        prepare_nic_req.retry       = false;
        prepare_nic_req.action_id   = prepare_nic.get_id();
    prepare_nic.message_req = prepare_nic_req.serialize();
    prepare_nic.failure_feedback = FEEDBACK_ROLLBACK_WORKFLOW;
    prepare_nic.rollback_action_id  = release_nic.get_id();
    prepare_nic.skipped     = skip_nic;
    wf_unfreeze.Add(reselect_hid, prepare_nic);

    wf_unfreeze.Add(reselect_hid, vec_auth_hc2);

    //修改正向流 PREPARE_M 的参数
    ConstructModifyAction(modify_prepare_vm, PREDECESSOR_SET_REQ_FOR_PREPARE_VM);
    wf_unfreeze.Add(prepare_nic, modify_prepare_vm);
    wf_unfreeze.Add(vec_auth_hc2, modify_prepare_vm);

    VmAction prepare_m_hc2(VM_WF_CATEGORY, VM_ACT_PRAPARE_M, EV_VM_OP, LCM, HC);
    prepare_m_hc2._vid              = vid;
        //构建热迁移的数据
        VmMigrateData createreq(vid, CREATE, "");
        VmAssistant->ConstructCreateReq(vid, createreq);
        createreq._operation    = PREP_M;
        createreq.retry         = false;
        createreq.action_id     = prepare_m_hc2.get_id();
    prepare_m_hc2.message_req       = createreq.serialize();
    prepare_m_hc2.success_feedback  = FEEDBACK_END_VMOP;
    prepare_m_hc2.failure_feedback  = FEEDBACK_ROLLBACK_WORKFLOW;
    prepare_m_hc2.timeout           = 60*10;   //10分钟超时
    prepare_m_hc2.timeout_feedback  = FEEDBACK_ROLLBACK_WORKFLOW;
    prepare_m_hc2.rollback_action_id= remove_vm.get_id();
    wf_unfreeze.Add(modify_prepare_vm, prepare_m_hc2);

    STATUS ret = CreateWorkflow(wf_unfreeze, wf_freeze);
    if(SUCCESS != ret)
    {
        Debug(SYS_WARNING, "interface failed to created workflow: %s!!!\n", wf_unfreeze.get_id().c_str());
        SkyAssert(false);
    }
    else
    {
        ret = ERROR_ACTION_RUNNING;
    }

    return ret;
}

MsgFrmHandlCreaterBoundle<> vmm_msgfrmhandl_creaters;

STATUS VMManager::ModifyVmCfg(const MessageFrame& message)
{
    MessageFrameHandling *handle = vmm_msgfrmhandl_creaters.CreateHandling(message, m);
    handle->Handling();
    delete handle;
    return SUCCESS;
}

/**********************************************************************
* 函数名称：VMManager::IsVMSupportLiveMigrate
* 功能描述：判断该虚拟机是否支持热迁移
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期     版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2012/03/05   V1.0    姚远        创建
************************************************************************/
bool VMManager::IsVMSupportLiveMigrate(int64 vid)
{
    VirtualMachine *pvm = vm_pool->Get(vid, false);
    if ( NULL == pvm )
    {
        return false;
    }

    if(!pvm->_config._enable_livemigrate)
    {//如设定不允许热迁移
        return false;
    }

    if (!pvm->_config._devices.empty())
    {//如果PCI设备非空，不允许热迁移
        return false;
    }

    return IsVMSupportMigrate(vid);
}

/**********************************************************************
* 函数名称：VMManager::IsVMSupportMigrate
* 功能描述：判断该虚拟机是否支持迁移
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期     版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2012/03/05   V1.0    姚远        创建
************************************************************************/
bool VMManager::IsVMSupportMigrate(int64 vid)
{
    VirtualMachine *pvm = vm_pool->Get(vid, false);
    if ( NULL == pvm )
    {
        return false;
    }

//todo pdh
    return IsVMStorageSupportMigrate(pvm->_config);
}

/**********************************************************************
* 函数名称：VMManager::QueryVM
* 功能描述：处理虚拟机查询请求
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
STATUS VMManager::QueryVM(const VmQueryReq& req,ostringstream& error)
{
    SkyAssert(req._stamp != LCM_STAMP && req._stamp != SCHED_STAMP);
    if(req._vids.empty())
    {
        return ERROR_INVALID_ARGUMENT;
    }
    ostringstream where;
    int ret = -1;
    int nic_rate = 0;
    VmQueryAck ack;
    VmRunningInfo info;
    vector<int64>::const_iterator it;
    for(it=req._vids.begin();it!=req._vids.end();it++)
    {
        info.Init();
        info._vid = *it;
        ret = vm_pool->IsVmExist(info._vid);
        if(ret == SQLDB_ERROR)
        {
            info._state = VM_UNKNOWN;
            info._result = ERROR_DB_SELECT_FAIL;
        }
        else if(ret == SQLDB_RESULT_EMPTY)
        {
            //虚拟机不存在时，用户看到的状态为配置状态，错误码为对象不存在
            info._state = VM_CONFIGURATION;
            info._result = ERROR_OBJECT_NOT_EXIST; //此错误码为TC所使用!!!
        }
        else
        {
            //如果确定虚拟机存在于vm_pool中，则从缓存中获取运行时信息
            ret = vm_pool->GetRunningInfo(*it,info);
            if(ret == 0)
            {
                //info._state = ToUserState((VmInstanceState)info._state);
                info._result = SUCCESS;
            }
            else
            {
                info._state = VM_UNKNOWN;
                info._result = ret;//ERR_OBJECT_GET_FAILED;
            }

            vector<VmStatisticsNics> nics_info;
            nics_info.clear();
            nic_rate = 0;
            where.str("");
            where << " WHERE vid = " << *it;
            ret = vm_statistics_nics_pool->GetVmStatisticsNics(nics_info, where.str());
            if(ret == 0)
            {
                vector<VmStatisticsNics>::const_iterator  nic_it;
                for (nic_it = nics_info.begin(); nic_it != nics_info.end();nic_it++ )
                {
                    nic_rate = nic_rate + nic_it->_used_rate;
                }

                if(nics_info.size()!=0)
                {
                    info._nic_rate = nic_rate/nics_info.size();
                }
            }
            else
            {
                Debug(SYS_WARNING, " failed to get statistics nics workflow: %d!!!\n", ret);
                info._result = ret;
            }
        }
        ack._running_info.push_back(info);
    }

    ack._result = SUCCESS;
    ack._stamp = req._stamp;
    //cout << ack;

    MID receiver;
    receiver.deserialize(req._stamp);
    MessageOption option(receiver,EV_VM_QUERY_ACK,0,0);
    return m->Send(ack,option);
}

/* CC接受TC过来的操作命令，如前一次操作未执行结束，本次操作不允许。以下情况特例：
Cancel可以抢占任何命令。
Reset可以抢占Reboot命令。
Destroy可以抢占Stop命令*/
bool VMManager::LegalOperation(VmOperation op, VmOperation last_op, STATUS last_op_result)
{
    if (last_op_result != ERROR_ACTION_RUNNING)
    {/* 如上一次操作已经结束 */
        return true;
    }

    switch (op)
    {
        case CANCEL:
        {
            return (MIGRATE != last_op) && (COLD_MIGRATE != last_op) && (LIVE_MIGRATE != last_op)   //撤销虚拟机，必须等热迁移有个结果
                    && (PLUG_IN_PDISK != last_op) && (MODIFY_VMCFG != last_op);
        }

        case RESET:
        {
            return (REBOOT == last_op);
        }

        case STOP:
        {
            return (PLUG_IN_PDISK == last_op) || (PLUG_OUT_PDISK == last_op);
        }

        case DESTROY:
        {
            return (STOP == last_op) || (PLUG_IN_PDISK == last_op) || (PLUG_OUT_PDISK == last_op);
        }

        case CANCEL_UPLOAD_IMG:
        {
                return (UPLOAD_IMG == last_op);
        }

        case RESTORE_IMAGE_BACKUP:
        {
            return (RECOVER_IMAGE !=last_op && UPLOAD_IMG != last_op && DELETE_IMAGE_BACKUP != last_op && CREATE_IMAGE_BACKUP != last_op
                    && MODIFY_VMCFG != last_op);
        }

        default:
        {
            return (last_op_result != ERROR_ACTION_RUNNING);
        }
    }
}

/**********************************************************************
* 函数名称：VMManager::LegalOperation
* 功能描述：检查虚拟机操作是否合法
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
bool VMManager::LegalOperation(VmOperation op,VmInstanceState inst_state,LcmState lcm_state)
{
    //除迁移期间之外,任何时候都可以撤销
    if(op == CANCEL)
        return true;

    switch(inst_state)
    {
        case VM_INIT:
            return (op == CREATE);

        case VM_CRASHED:
            return (op == DESTROY || op == RESET || op == PLUG_IN_USB ||
                       op == PLUG_OUT_USB || op == OUT_IN_USB || op == MODIFY_IMAGE_BACKUP);

        case VM_PAUSED:
            return (op == RESUME || op == DESTROY || op == RESET ||
                       op == PLUG_IN_USB || op == PLUG_OUT_USB || op == OUT_IN_USB || op == MODIFY_IMAGE_BACKUP);

        case VM_RUNNING:
            return (op == STOP || op == REBOOT || op == PAUSE || op == PREP_M || op == MIGRATE || op == POST_M || op == LIVE_MIGRATE
                    || op == RESET || op == DESTROY || op == PLUG_IN_USB || op == PLUG_OUT_USB || op == OUT_IN_USB
                    ||op == PLUG_IN_PDISK ||op == PLUG_OUT_PDISK || op == MODIFY_IMAGE_BACKUP);

        case VM_SHUTOFF:
            return (op == CREATE || op == START || op == UPLOAD_IMG || op == CANCEL_UPLOAD_IMG ||
                       op == PLUG_IN_USB || op == PLUG_OUT_USB || op == OUT_IN_USB ||op == PLUG_IN_PDISK ||
                       op == PLUG_OUT_PDISK||op == RECOVER_IMAGE || op == MODIFY_IMAGE_BACKUP
                       ||op == COLD_MIGRATE||op==CREATE_IMAGE_BACKUP ||op==DELETE_IMAGE_BACKUP ||op ==RESTORE_IMAGE_BACKUP
                       || op == FREEZE || op == UNFREEZE
                       || op == MODIFY_VMCFG);

        case VM_UNKNOWN:
            return (op != FREEZE) && (op != UNFREEZE);

        default:
            return false;
    }
}

//根据逻辑状态判断合法性
bool VMManager::LegalOperationforLogState(VmOperation op, VmInstanceState log_state)
{
    //除迁移期间之外,任何时候都可以撤销
    if(op == CANCEL)
        return true;

    switch(log_state)
    {
        case VM_INIT:
            return (op != UNFREEZE);

        case VM_FROZEN:
            return (op == UNFREEZE) || (op == MODIFY_VMCFG);

        default:
            return false;
    }
}


/* 处理从Api 来的VM的USB信息查询函数,超时调用暂时没有被调用，因为不支持多实例查询 */
int VMManager::DealVmUsbInfoQueryTimeOut()
{
    VmUsbListAck    usb_list_from_cc;
    vector <int64>   vids;

    int64 hid = m->GetTimerArg();

    // 获取对应CC的hostname
    usb_list_from_cc._ret = SUCCESS;
    vm_pool->GetVmsByHost(vids, hid, 1000);
    usb_pool->GetUsbInfoByVid(vids, usb_list_from_cc._info);

    // 如果超时，直接返回数据内容给API
    uint32 cc;
    for (cc = 0; cc < usb_list_from_cc._info.size(); cc++)
    {
        usb_list_from_cc._info.at(cc).online_states = USB_UNKOWEN;
    }

    // 回消息给发送者
    MID receiver;
    receiver.deserialize(m->GetTimerStrArg());
    MessageOption option(receiver, EV_VMCFG_USB_INFO_QUERY_ACK,0,0);

    STATUS ret = m->Send(usb_list_from_cc,option);
    Debug( SYS_DEBUG, "DealVmUsbInfoQueryTimeOut: send msg to Api rc=%d\n", ret );
    return SUCCESS;

}


/* 处理从Api 来的VM的USB信息查询函数 */
int VMManager::DealVmUsbInfoQueryFromHc(const MessageFrame &message)
{
    VmUsbListAck    usb_list_from_hc(TECS_ERR_OBJECT_NOT_EXIST);
    VmUsbListAck    usb_list_from_cc;
    vector <int64>   vids;

    usb_list_from_hc.deserialize(message.data);

    // 获取对应CC的hostname
    usb_list_from_cc._ret = SUCCESS;

    HostPool *host_pool = HostPool::GetInstance();
    int64 hid = host_pool->GetHostIdByName(message.option.sender()._application);
    vm_pool->GetVmsByHost(vids, hid, 1000);
    usb_pool->GetUsbInfoByVid(vids, usb_list_from_cc._info);

    // 先更新数据库中更新的数据
    uint32 cc,hc;
    uint32 cc_size = usb_list_from_cc._info.size();
    for (cc = 0; cc < cc_size; cc++)
    {
        for (hc = 0; hc < usb_list_from_hc._info.size(); hc++)
        {
            if ((usb_list_from_cc._info.at(cc).vendor_id == \
                usb_list_from_hc._info.at(hc).vendor_id) &&\
                (usb_list_from_cc._info.at(cc).product_id == \
                usb_list_from_hc._info.at(hc).product_id))
            {  // 默认在线都是false
                usb_list_from_cc._info.at(cc).online_states = USB_ON_LINE;
                usb_list_from_cc._info.at(cc).is_conflict = usb_list_from_hc._info.at(hc).is_conflict;
                usb_list_from_cc._info.at(cc).manufacturer = usb_list_from_hc._info.at(hc).manufacturer;
            }
        }
    }

    // 再增加HC上上报的数据到CC记录中，一起上报
    uint32 hc_size = usb_list_from_hc._info.size();
    for (hc = 0; hc < hc_size; hc++)
    {
        bool is_find = false;
        for (cc = 0; cc < usb_list_from_cc._info.size(); cc++)
        {
            if ((usb_list_from_cc._info.at(cc).vendor_id == \
                usb_list_from_hc._info.at(hc).vendor_id) &&\
                (usb_list_from_cc._info.at(cc).product_id == \
                usb_list_from_hc._info.at(hc).product_id))
            {
                is_find = true;
                break;
            }
        }
        if (!is_find)
        {
            usb_list_from_cc._info.push_back(usb_list_from_hc._info.at(hc));
        }
    }

    // 回消息给发送者
    MID receiver;
    receiver.deserialize(usb_list_from_hc._stamp);
    MessageOption option(receiver, EV_VMCFG_USB_INFO_QUERY_ACK,0,0);

    STATUS ret = m->Send(usb_list_from_cc,option);
    Debug( SYS_DEBUG, "DealVmUsbInfoQueryFromHc: send msg to Api rc=%d\n", ret );
    return SUCCESS;
}


/* 处理从Api 来的VM的USB信息查询函数 */
int VMManager::DealVmUsbInfoQueryFromApi(const MessageFrame &message)
{
    VmUsbListAck usb_list(TECS_ERR_OBJECT_NOT_EXIST);
    int64 vid = atol(message.data.c_str());

    if (vm_pool->IsVmExist(vid) == SQLDB_RESULT_EMPTY)
    {
        Debug( SYS_WARNING, "DealVmUsbInfoQueryFromApi: vm %d does not exist\n", vid );
        MessageOption option(message.option.sender(),EV_VMCFG_USB_INFO_QUERY_ACK,0,0);
        m->Send(usb_list, option);
        return ERROR;
    }

    // 获取对应HC的hostname
    int64 hid = vm_pool->GetHid(vid);

    HostPool *host_pool = HostPool::GetInstance();
    string hc_name = host_pool->GetHostNameById(hid);

    //发消息给HC
    MID receiver(hc_name, PROC_HC, MU_VM_HANDLER);
    MessageOption option(receiver, EV_VMCFG_USB_INFO_QUERY,0,0);

    usb_list._stamp = message.option.sender().serialize();
    STATUS ret = m->Send(usb_list,option);
    Debug( SYS_DEBUG, "DealVmUsbInfoQueryFromApi: send msg to Hc rc=%d\n", ret );

    return SUCCESS;
}

int VMManager::SyncVmCfg(const MessageFrame &message)
{
    int ret;
    ostringstream error;

    MID sender = message.option.sender();
    GetRegisteredSystem(tc);
    if(sender._application != tc)
    {
        Debug(SYS_WARNING,"vm_manager receive sync info from unknown tc!\n");
        return ERROR;
    }

    VMDeployInfo deploy;
    if(false == deploy.deserialize(message.data))
    {
        Debug(SYS_ERROR,"vm_manager failed to deserialize deploy info\n");
        return ERROR;
    }

    Debug(SYS_DEBUG,"Cluster will sync vm cfg %lld, stamp=%s.\n",
                deploy._config._vid,
                deploy._stamp.c_str());

    //虚拟机不存在时的处理
    if(vm_pool->IsVmExist(deploy._config._vid) == SQLDB_RESULT_EMPTY)
    {
        //error << "vm_manager find no vm(id=" <<req._vid<< ") in this cluster pool!";
        //Debug(SYS_WARNING,"%s\n",error.str().c_str());
        return ERR_VM_NOT_EXIST;
    }

    deploy._stamp = sender.serialize();
    MessageOption option(MU_VM_SCHEDULER,EV_VMCFG_SYNC_REQ,0,0);
    ret = m->Send(deploy,option);
    if(SUCCESS != ret)
    {
        error << "Scheduler failed to receive, ret = " << ret;
        Debug(SYS_ALERT,"%s\n",error.str().c_str());
    }
    return ret;
}

//schedule模块应答消息，目前只有部署和热迁移的时候会给schedule发消息
int VMManager::HandleSelectHostAck(const MessageFrame &message)
{
    SkyAssert(false);

    return 0;
}

int VMManager::PrepareDeployVMConfig(VMConfig &config)
{
    config._machine._request_id = GenerateUUID();
    config._initrd._request_id = GenerateUUID();
    config._kernel._request_id = GenerateUUID();

    vector<VmDiskConfig>::iterator it;
    for (it = config._disks.begin(); it != config._disks.end(); it++)
    {
        it->_request_id = GenerateUUID();
    }

    return 0;
}

STATUS VMManager::DealVmRestoreReq(const MessageFrame &message)
{
    VmImageBackupOp req_info;
    ostringstream oss;
    ostringstream error;
    STATUS ret;

    if(false == req_info.deserialize(message.data))
    {
        SkyAssert(false);
        return ERROR;
    }

    if (IsActionFinished(req_info.action_id))
    {
        return 0;
    }

    //操作表
    VMImagePool *image_pool = VMImagePool::GetInstance();
    SkyAssert(image_pool);

    if(false == image_pool->IsBaseImageBackup(req_info._request_id,req_info._target, req_info._vid))
    {
        ret = image_pool->UpdateImageBackup(req_info._request_id, req_info._target,req_info._vid,IMG_BACKUP_ACTIVE);
    }
    else
    {
        //对于base的，则直接设置所有的备份去激活
        ret = image_pool->SetImageBackupState(req_info._target,req_info._vid);
    }
    
    if(ret != SUCCESS)
    {
        oss <<"vid "<<req_info._vid<<" "<<req_info._request_id<< "restore fail";
        ret = ERROR;
    }
    else
    {
        oss << "success";
        ret = SUCCESS;
    }

    int progress = (ret == SUCCESS) ? 100 : 0;
    VmOperationAck ack(req_info.action_id, ret, progress, oss.str(), req_info._vid, RESTORE_IMAGE_BACKUP);

    MessageOption option(message.option.sender(),EV_VM_RESTORE_ACK,0,0);
    ret = m->Send(ack,option);
    if(SUCCESS != ret)
    {
        error << "Vm failed to send restore ack, ret = " << ret;
        Debug(SYS_ALERT,"%s\n",error.str().c_str());
    }
    return ret;
}

STATUS VMManager::DealVmRestoreAck(const MessageFrame &message)
{
    VmOperationAck ack;
    ostringstream error;
    MID sender;
    STATUS ret;

    if(false == ack.deserialize(message.data))
    {
        SkyAssert(false);
        return ERROR;
    }

    ret = GetUpstreamSender(ack.action_id, sender);
    if(SUCCESS != ret)
    {
        error << "Vm failed to get upstream sender, ret = " << ret;
        Debug(SYS_ALERT,"%s\n",error.str().c_str());
    }

    if(ack.state == SUCCESS)
    {
        if(SUCCESS != FinishAction(ack.action_id))
        {
            SkyAssert(false);
        }
    }
    return SendVmOperationAck(m, ack, sender);
}

int VMManager::GetWorkflowProgress(const string& action_id)
{
    int progress = ::GetWorkflowProgress(action_id);

    if (progress == 100)
    {
        progress = 99;
    }

    return progress;
}

void DbgShowAllVms()
{
    VirtualMachinePool *vm_pool = VirtualMachinePool::GetInstance();
    vector<int64> vids;
    if(0 != vm_pool->GetAllVms(vids,100))
    {
        printf("vm_pool->GetAllVms failed!\n");
        return;
    }

    printf("=============vm information begin=============\n");
    vector<int64>::iterator it;
    for(it=vids.begin();it!=vids.end();it++)
    {
        VirtualMachine *pvm = vm_pool->Get(*it,true);
        if(!pvm)
            continue;
        //cout << pvm->serialize() << endl;
        cout << *pvm << endl;
        vm_pool->Put(pvm,true);
    }
    printf("=============vm information end===============\n");
}
DEFINE_DEBUG_CMD(vms,DbgShowAllVms);

void DbgOperateVm(int64 vid,int op)
{
    VmOperationReq req(vid,op);
    MessageOption option(MU_VM_MANAGER,EV_VM_OP,0,0);
    MessageFrame message(req,option);
    VMManager *vmm = VMManager::GetInstance();
    vmm->Receive(message);
}
DEFINE_DEBUG_CMD(vmop,DbgOperateVm);

void DbgDeployVm(int64 vid,const char* hostname)
{
    if(!hostname)
    {
        printf("please give a host name!\n");
        return;
    }

    VMDeployInfo info;
    info._uid = 123;
    info._host = hostname;
    info._config._vid = vid;
    info._config._name = "myvm";
    info._config._vcpu = 2;
    info._config._memory = 128;

    VmDiskConfig disk1;
    disk1._id = 12345;
    info._config._disks.push_back(disk1);
    VmDiskConfig disk2;
    disk2._id = 67890;
    info._config._disks.push_back(disk2);

    VmNicConfig nic1;
    nic1._sriov= 0;
    info._config._nics.push_back(nic1);

#if 0
    info._config._behavior._on_crash = "reboot";
    info._config._behavior._on_poweroff = "alarm_only";
    info._config._behavior._on_reboot = "ignore";
    Expression exp1("HYPERVISOR","=","XEN");
    info._requirement._qualifications.push_back(exp1);
#endif

#if 0
    string s = info.serialize();
    cout << s << endl;
    cout << "------------------------------------------------------\n\n\n" << endl;
    VMDeployInfo _info;
    _info.deserialize(s);
    cout << _info.serialize() << endl;
#else
    VmOperationReq req(vid,DEPLOY,"zwj");
    MessageOption option(MU_VM_MANAGER,EV_VM_DEPLOY,0,0);
    MessageFrame message(info,option);
    VMManager *vmm = VMManager::GetInstance();
    vmm->Receive(message);
#endif
}
DEFINE_DEBUG_CMD(deploy,DbgDeployVm);

void DbgMigrateVm(int64 vid,int64 hid)
{
    VmMigrateReq req;
    req._vid = vid;
    req._operation = MIGRATE;
    req._stamp = "debug";
    req._hid = hid;

    MessageOption option(MU_VM_MANAGER,EV_VM_MIGRATE,0,0);
    string tc;
    GetRegisteredSystem(tc);
    MID sender(tc,"tc","vmcfg");
    option.setSender(sender);

    MessageFrame message(req,option);
    VMManager *vmm = VMManager::GetInstance();

    vmm->Receive(message);
}
DEFINE_DEBUG_CMD(migrate,DbgMigrateVm);

void DbgRestoreVm(int64 vid, const char* request_id,const char* target)
{
    if (NULL == request_id || 0 == vid ||NULL == target)
    {
        cout << "Invalid para!" <<endl;
        return;
    }

    VmImageBackupOp para;
    para._vid = vid;
    para._request_id = request_id;
    para._target = target;

    MessageUnit    temp_mu(TempUnitName("restore_vm_image"));
    temp_mu.Register();
    MessageOption  req_option(MU_VM_MANAGER, EV_VM_OP, 0, 0);

    VmImageBackupOp op_msg(para._vid, RESTORE_IMAGE_BACKUP, temp_mu.GetSelfMID(), para._request_id, para._target ,"");
    temp_mu.Send(op_msg, req_option);

    MessageFrame  msg;
    if (SUCCESS == temp_mu.Wait(&msg, 5000))
    {
        VmOperationAck  cc_ack;
        cc_ack.deserialize(msg.data);
        cout << " VmOperationAck: "
             << " vid: " << cc_ack._vid
             << " operate:" << cc_ack._operation
             << " vm_state: " << cc_ack._vm_state
             << "action_id: "<< cc_ack.action_id
             << "state : "<<cc_ack.state
             <<"progress:"<<cc_ack.progress
             <<"detail: "<<cc_ack.detail << endl;
    }
    else
    {
       cout<<"Wait ack timeout"<<endl;
    }
}
DEFINE_DEBUG_FUNC(DbgRestoreVm);

void DbgCreateImgBackup(int64 vid,  const char* target)
{
    if (INVALID_IID == vid)
    {
        cout << "Invalid vid " << vid <<endl;
        return;
    }

    if ( NULL == target )
    {
        cout << "Invalid para!" <<endl;
        return;
    }


    // 发送消息给CC

    MessageUnit    temp_mu(TempUnitName("imgbakcp_op_dbg"));
    temp_mu.Register();
    MessageOption  req_option(MU_VM_MANAGER, EV_VM_OP, 0, 0);

    VmImageBackupOp op_msg(vid, CREATE_IMAGE_BACKUP, "", "",target,"" );
    STATUS ret = temp_mu.Send(op_msg, req_option);
    if (SUCCESS != ret)
    {
        cerr << "Fail to send imgbackup request! ret = " << ret << endl;
    }
    else
    {
        cout << "backup img  request send success" << endl;
    }

    MessageFrame message;
    if(SUCCESS == temp_mu.Wait(&message,3000))
    {
        VmOperationAck  ack;
        ack.deserialize(message.data);
        cout << " VmOperationAck: "
             << " vid: " << ack._vid
             << " op:" << ack._operation
             << " state:" << ack._vm_state << endl;
    }
    else
    {
        cout<<"Wait ack timeout"<<endl;
    }
}
DEFINE_DEBUG_FUNC(DbgCreateImgBackup);

}

