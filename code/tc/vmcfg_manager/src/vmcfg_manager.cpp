/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vmcfg_manager.cpp
* 文件标识：见配置管理计划书
* 内容摘要：VmCfgManager类的实现文件
* 当前版本：1.0
* 作    者：Bob
* 完成日期：2011年7月28日
*
* 修改记录1：
*    修改日期：2011/7/28
*    版 本 号：V1.0
*    修 改 人：Bob
*    修改内容：创建
*******************************************************************************/
#include "vmcfg_manager.h"
#include "sky.h"
#include "authrequest.h"
#include "authmanager.h"
#include "tecs_errcode.h"
#include "db_config.h"
#include "cluster_manager_with_vm_config.h"
#include "vmcfg_manager_with_file_manager.h"
#include "vmcfg_pool.h"
#include "workflow.h"
#include "vnet_libnet.h"
#include "volume.h"
#include "affinity_region_pool.h"
#include "image_db.h"

static int vcmm_print = 0;
DEFINE_DEBUG_VAR(vcmm_print);
#define Dbg_Prn(fmt,arg...) \
        do \
        { \
            if(vcmm_print) \
            { \
                printf("[%s:%d]"fmt,__func__,__LINE__, ##arg); \
            } \
        }while(0)


namespace zte_tecs
{
//#define MAX_CONFIG_VERSION  (int)(9999) //最大配置版本号

#define EV_VM_OPERATE_TIMEOUT EV_TIMER_1

#define EV_VM_QUERY_TIMEOUT   EV_TIMER_2



//const int k_deploy_wait_duration = VM_DEPLOY_DURATION_TC*1000; // 2小时 + 4 秒

const int k_deploy_wait_duration = VM_OTHER_OP_DURATION_TC*1000; //14 秒

const int k_cancel_wait_duration = VM_CANCEL_DURATION_TC*1000; // 3分钟 + 4 秒

const int k_action_wait_duration = VM_OTHER_OP_DURATION_TC*1000; //14 秒

const int k_query_wait_duration  = VM_QUERY_DURATION_TC*1000; // 3 秒


// 所有调度完成是 0 - 5
#define WEIGHT_SCHEDULE_TOTAL   5

// 计算
#define WEIGHT_FILTER_COMPUTE   1
// 选集群
#define WEIGHT_CHOOSE_CLUSTER   (WEIGHT_SCHEDULE_TOTAL - WEIGHT_FILTER_COMPUTE)


// 申请资源在 6 - 19
// 部署image
#define WEIGHT_IMAGE_DEPLOY     10
// coredump_url
#define WEIGHT_GET_COREDUMP_URL 2
// 网络
#define WEIGHT_ALLOCATE_NETWORK 2

//准备部署消息 19-20
#define WEIGHT_PREPARE_DEPLOY   1

// CC上的进度  21 -100
#define WEIGHT_DEPLOY_2_CLUSTER 80

// 从CC上撤销
#define WEIGHT_CANCEL_FROM_CLUSTER (WEIGHT_DEPLOY_2_CLUSTER)


VmCfgManager  *VmCfgManager::instance = NULL;


/******************************************************************************/
/* VmCfgManager :: Constructor/Destructor                                   */
/******************************************************************************/
VmCfgManager::VmCfgManager()
{
    _mu = NULL;
};

/************************************************************
* 函数名称： StartMu
* 功能描述： 启动mu的函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2012/5         V1.0       lixiaocheng         创建
***************************************************************/
STATUS VmCfgManager::StartMu(const string& name)
{
    if(_mu)
    {
        SkyAssert(false);
        return ERROR_DUPLICATED_OP;
    }

    // 消息单元的创建和初始化
    _mu = new MessageUnit(name);
    if (!_mu)
    {
        OutPut(SYS_EMERGENCY, "Create mu %s failed!\n",name.c_str());
        return ERROR_NO_MEMORY;
    }

    STATUS ret = _mu->SetHandler(this);
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "SetHandler mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }

    ret = _mu->Run();
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "Run mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }

    ret = _mu->Register();
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "Register mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }

    //给自己发送上电消息，促使消息单元状态切换到工作态
    MessageFrame message(SkyInt32(0), EV_POWER_ON);
    ret = _mu->Receive(message);
    if (SUCCESS != ret)
    {
        SkyAssert(0);
        return ret;
    }
    _mu->Print();
    return SUCCESS;
}

/******************************************************************************/
/* int VmCfgManager:: Init(SqlDB *database)
{

    // 创建 vmcfg_pool表
    VmCfgPool::Bootstrap(database);

    // 创建 VmCfgPool对象
    if (NULL == _vcpool)
    {
        _vcpool = new  VmCfgPool(database);
    }

    if (NULL == _vcpool)
    {
        return -1;
    }

    return 0;

} */

/******************************************************************************/
void VmCfgManager::MessageEntry(const MessageFrame &message)
{
    STATUS  ret = ERROR;

    switch (_mu->get_state())
    {
    case S_Startup:
    {
        switch (message.option.id())
        {
        case EV_POWER_ON:
        {
            OutPut(SYS_DEBUG,"VmCfgManager power on!\n");
            ret = Transaction::Enable(GetDB());
            if (SUCCESS != ret)
            {
                OutPut(SYS_ERROR,"vmcfg_manager enable transaction fail! ret = %d",ret);
                SkyExit(-1, "VmCfgManager::MessageEntry: call Transaction::Enable(GetDB()) failed.");
            }
            _mu->set_state(S_Work);
            OutPut(SYS_NOTICE, "%s power on!\n",_mu->name().c_str());
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
        case EV_VM_OP:
        {// 接收Method方法发过来的虚拟机部署/撤销请求
            VmOperationReq op_req;
            op_req.deserialize(message.data);

            DealVmOp(message.option.sender(), op_req, message);
            break;
        }
        case EV_VM_ALLOCATE_REQ:
        {// 接收Method方法发过来的虚拟机创建请求
            VmAllocateReq op_req;
            op_req.deserialize(message.data);
            DealAllocate(message.option.sender(), op_req);
            break;
        }

        // 2. 收到ClusterManager的应答
        case EV_FILTER_CLUSTER_BY_COMPUTE_ACK:
        {
            FilterClusterByComputeAck ack;
            ack.deserialize(message.data);

            DealFilterComputeAck( ack);
            break;
        }

        // 3. 收到SC的调度应答
        case EV_STORAGE_GET_CLUSTERS_ACK:
        {
            FilterClusterByShareStorageAck ack;
            ack.deserialize(message.data);
            DealFliterShareStorageAck (ack);
            break;
        }
        // 4. 收到VNM的调度应答
        case EV_VNETLIB_SELECT_CC_ACK:
        {
            CVNetSelectCCListMsgAck ack;
            ack.deserialize(message.data);
            DealFilterNetworkAck(ack);
            break;
        }

        // 5. 从候选者中挑选一个
        case EV_SELECT_CLUSTER_FROM_CANDIDATES_REQ:
        {
            VmOperationReq req;
            req.deserialize(message.data);
            DealSelectCluster(req);
            break;
        }

        //部署image的反馈处理
        case EV_IMAGE_PREPARE_ACK:
        {
            ImagePrepareAckMsg ack;
            ack.deserialize(message.data);

            DealImagePrepareAck( ack);
            break;
        }

        //获取coredump_url的反馈处理
        case EV_FILE_GET_URL_ACK:
        {
            UploadUrl ack;
            ack.deserialize(message.data);

            DealCoreDumpUrlAck( ack);
            break;
        }

        // 6. 收到VNM的分配应答
        case EV_VNETLIB_GETRESOURCE_CC_ACK:
        {
            CVNetGetResourceCCMsgAck ack;
            ack.deserialize(message.data);
            DealAllocateNetworkAck(ack);
            break;
        }
        case EV_VNETLIB_FREERESOURCE_CC_ACK:
        {
            CVNetFreeResourceCCMsgAck ack;
            ack.deserialize(message.data);
            DealFreeNetworkAck(ack);
            break;
        }

        case EV_VMCFG_PREPARE_DEPLOY_MSG_REQ:
        {//准备部署虚拟机的处理
            PrepareDeployMsg req_msg;
            req_msg.deserialize(message.data);

            DealPrepareDeployMsgReq(req_msg);
            break;
        }

        case EV_VM_OP_ACK:
        {
            DealAck(GetMsg<VmOperationAck>(message));
            break;
        }

        case EV_VM_ALLOCATE_AFFREG_REQ:
        {//准备部署虚拟机的处理
            AffinityRegion req_msg;
            req_msg.deserialize(message.data);
            DealAffregAllocateReq(req_msg);
            break;
        }

        case EV_VMCFG_MODIFY_REQ:
        {
            BatchModifyVmCfg(message);
            break;
        }

        default:
            OutPut(SYS_DEBUG, "VmCfgManager rcv a unexpected message %d !\n",\
                            message.option.id());
            break;
        }
        break;
    }

    default:
        break;
    }
}

/******************************************************************************/
bool VmCfgManager::CheckVmIsAllowDeploy(int64 vid,ostringstream &err_oss)
{
    STATUS tRet = _vcpool->CheckImageIdByVid(vid);
    if (SUCCESS != tRet)
    {
        err_oss<<"Image file of VM ("<< vid <<") not exist.";
        return false;
    }

    if (_vcpool->IsVmModifying(vid))
    {
        err_oss << "VM(" << vid << ") is modifying" << endl;
        return false;
    }

    return true;
}

/******************************************************************************/
void VmCfgManager::DealVmOp(MID receiver, VmOperationReq &req, const MessageFrame &message)
{
    VmOperationAck ack;

    ack._vid = req._vid;
    ack.action_id = req.action_id;

    VmCfg  vmcfg;
    STATUS tRet = _vcpool->GetVmById(vmcfg, req._vid);
    if (SUCCESS != tRet)
    {
        OutPut(SYS_DEBUG, "*** VM (%d) not exist in TC.***\n", req._vid);
        ack.state = ERR_OBJECT_NOT_EXIST;
        Ack2Api(receiver, ack);
    }

    req._user = vmcfg._user_name;

    if (DEPLOY == req._operation)
    {
        ostringstream error;
        if(true ==CheckVmIsAllowDeploy(req._vid,error))
        {            
            DealDeploy(receiver,req, vmcfg, ack);
        }
        else
        {
            ack.state  = ERROR;
            ack.detail = error.str();
        }
        
    }
    else if(CANCEL == req._operation)
    {
        DealCancel(receiver,req,vmcfg, ack);
    }
    else if (MODIFY_VMCFG == req._operation)
    {
        DealModify(message);
        return;
    }
    else
    {
        OutPut(SYS_DEBUG, "DealVmOp: recv unexpect op(%d) of VM (%d)***\n", \
                          req._operation, req._vid);

        ack.state  = ERROR;
        ack.detail = "unexpected op!";
    }

    if (MODIFY_VMCFG != req._operation)
    {
        GetWorkflowEngine(ack._workflow_id,ack._workflow_engine);
        Ack2Api(receiver, ack);
    }
    return;

}
/******************************************************************************/
void VmCfgManager::DealAllocate(MID receiver, const VmAllocateReq &req)
{
    ostringstream  oss_vid;
    ostringstream  oss;
    VmCfg vmcfg;
    int   vm_num;
    int   ret_code   = ERROR;
    VmAllocateAck ack_msg;

    vmcfg=req.base_info;
    vm_num = req.vm_num;

    for (int index =1; index <= vm_num; index ++)
    {
        ret_code= _vcpool->Allocate(vmcfg);

        if (SUCCESS != ret_code)
        {
            oss <<"Create VM "<<index << " failed !" << endl;
            oss <<"Success create VM ID are:" <<endl;
            oss << oss_vid.str() ;

            OutPut(SYS_DEBUG, "DealSetup Allocate faild!\n");
            SkyAssert(false);
            ret_code =ERROR_DB_INSERT_FAIL;
            goto error_exit;
        }

        oss_vid <<vmcfg._oid<<" ";
        if(0 == (index % 5))
        {
           oss_vid <<endl;
        }
        ack_msg.vids.push_back(vmcfg._oid);
    }

error_exit:

    ack_msg.detail = oss.str();
    ack_msg.state = ret_code;

    MessageOption ack_option(receiver, EV_VM_ALLOCATE_ACK, 0, 0);

    if (SUCCESS == ret_code)
    {
        OutPut(SYS_DEBUG,"Create vms Success,state:%d\n",
                         ack_msg.state);
    }
    else
    {
        OutPut(SYS_ERROR,"Create vms Failed,state:%d,detail:%s\n",
                         ack_msg.state,
                         ack_msg.detail.c_str());
    }
    _mu->Send(ack_msg, ack_option);

    return ;
}
/******************************************************************************/
void VmCfgManager::DealDeploy(MID receiver, const VmOperationReq &req, VmCfg &vmcfg, VmOperationAck &ack)
{
    STATUS ret = ERROR;
    vector<string> deploy_workflows;
    vector<string> rollback_workflows;
    vector<string> cancel_workflows;
    vector<string> actions;

    bool is_deploying = FindVmCfgWorkFlow(deploy_workflows,
                                             VM_WF_DEPLOY_TO_CLUSTER,
                                             req._vid,
                                             false);//(有正向流)

    bool has_rollback = FindVmCfgWorkFlow(rollback_workflows,
                                             VM_WF_CANCEL_FROM_CLUSTER,
                                             req._vid,
                                             true);//(有回滚流)

    bool has_cancel = FindVmCfgWorkFlow(cancel_workflows,
                                        VM_WF_CANCEL_FROM_CLUSTER,
                                        req._vid,
                                        false);//(有反向流)

    if(is_deploying)  //(有正向流)
    {
        // 正在部署中
        OutPut(SYS_DEBUG, "*** VM (%d) is deploying now.***\n", req._vid);
        ack.state    = ERROR_ACTION_RUNNING;
        ack.progress = GetWorkflowProgress(*deploy_workflows.begin());

    }
    else if(has_rollback)   //(有回滚流)
    {
         OutPut(SYS_DEBUG, "*** VM (%d) is roalling back now.***\n", req._vid);
         ack.state    = ERROR_ACTION_RUNNING;
         ack.progress = GetWorkflowProgress(*rollback_workflows.begin());
    }
    else if( has_cancel)  //(有反向流)
    {
         OutPut(SYS_DEBUG, "*** VM (%d) is canceling now.***\n", req._vid);
         ack.state    = ERROR;
         ack.progress = 0;
    }
    else if (vmcfg.isDeployed())  //(有表 )
    {
            //部署完成
            OutPut(SYS_DEBUG, "*** VM (%d) already deployed.***\n", req._vid);
            ack.state = SUCCESS;
            ack.progress = 100;
    }
    else //(无表)
    {
            //创建部署工作流
            OutPut(SYS_DEBUG, "*** VM (%d) create deploy workflow.***\n", req._vid);
            ret = CreateDeployWF(receiver,vmcfg, req, ack._workflow_id);
            if(SUCCESS == ret)
            {
                ack.progress = 0;
                ack.state = ERROR_ACTION_RUNNING;
            }
            else
            {
                ack.state = ERROR;
            }
    }

    return ;
}

STATUS VmCfgManager::CheckVmIsAllowCancel(int64 vid, ostringstream &err_oss)
{
    if (_vcpool->IsVmModifying(vid))
    {
        err_oss << "VM(" << vid << ") is modifying" << endl;
        return ERR_VMCFG_STATE_FORBIDDEN;
    }

    return SUCCESS;
}

/******************************************************************************/
void VmCfgManager::DealCancel(MID receiver, const VmOperationReq &req, VmCfg &vmcfg, VmOperationAck &ack)
{
    STATUS ret = ERROR;
    vector<string> deploy_workflows;
    vector<string> rollback_workflows;
    vector<string> cancel_workflows;
    vector<string> actions;

    ostringstream       err_oss;
    ret = CheckVmIsAllowCancel(req._vid, err_oss);
    if (ret != SUCCESS)
    {
        ack.state = ret;
        ack.detail = err_oss.str();
        return;
    }

    bool is_deploying = FindVmCfgWorkFlow(deploy_workflows,
                                             VM_WF_DEPLOY_TO_CLUSTER,
                                             req._vid,
                                             false);//(有正向流)

    bool has_rollback = FindVmCfgWorkFlow(rollback_workflows,
                                             VM_WF_CANCEL_FROM_CLUSTER,
                                             req._vid,
                                             true);//(有回滚流)

    bool has_cancel = FindVmCfgWorkFlow(cancel_workflows,
                                        VM_WF_CANCEL_FROM_CLUSTER,
                                        req._vid,
                                        false);//(有反向流)

        if (is_deploying)       //(有正向流)
        {

            //先获取回滚流
            string rollback_id;
            ret = GetRollbackWorkflow(*deploy_workflows.begin(), rollback_id);
            if (SUCCESS != ret)
            {
                OutPut(SYS_DEBUG, "Get rollback work flow of %s failed ! \n", (*deploy_workflows.begin()).c_str());
                ack.state = ERROR;
            }
            else
            {
                //启动回滚
                ret = RollbackWorkflow(*deploy_workflows.begin());
                if(SUCCESS == ret)
                {
                    //去掉回滚属性
                    ret = UnsetRollbackAttr(rollback_id);
                    if(SUCCESS == ret)
                    {
                        OutPut(SYS_DEBUG, "*** VM (%d) is deploying now.***\n", req._vid);
                        ack.state = ERROR_ACTION_RUNNING;
                        ack.progress = GetWorkflowProgress(rollback_id);
                        ack._workflow_id = rollback_id;
                    }
                    else
                    {
                        ack.state = ERROR;
                    }
                }
                else
                {
                    ack.state = ERROR;
                }
            }
        }
        else if(has_rollback)   //(有回滚流)
        {
            //去掉回滚属性
            ret = UnsetRollbackAttr(*rollback_workflows.begin());
            if(SUCCESS == ret)
            {
                OutPut(SYS_DEBUG, "*** VM (%d) is rolling back now.***\n", req._vid);
                ack.state = ERROR_ACTION_RUNNING;
                ack.progress = GetWorkflowProgress(*rollback_workflows.begin());
                ack._workflow_id = *rollback_workflows.begin();
            }
            else
            {
                ack.state = ERROR;
                ack.progress = 0;
            }
        }
        else if(has_cancel)   //(有反向流)
        {
             ack.state    = ERROR_ACTION_RUNNING;
             ack.progress = GetWorkflowProgress(*cancel_workflows.begin());
             ack._workflow_id = *cancel_workflows.begin();
             OutPut(SYS_DEBUG, "*** VM (%d) is canceling now.***\n", req._vid);
        }
        else if(!vmcfg.isDeployed())    //(无表)
        {
            OutPut(SYS_DEBUG, "*** VM (%d) is canceled.***\n", req._vid);
            ack.progress = 100;
            ack.state = SUCCESS;
        }
        else   //(有表 )
        {
            //启动反向工作流
            OutPut(SYS_DEBUG, "*** VM (%d) create cancel workflow.***\n", req._vid);
            ret = CreateCancelWF(receiver, vmcfg, req, ack._workflow_id);
            if(SUCCESS == ret)
            {
                ack.progress = 0;
                ack.state = ERROR_ACTION_RUNNING;
            }
            else
            {
                ack.progress = 0;
                ack.state = ERROR;
            }
        }

        return;
}
/******************************************************************************/
void VmCfgManager::DealAffregAllocateReq(AffinityRegion& req)
{

    STATUS rc = ERROR;
    string              err_str;
    ostringstream       err_oss;

    MID sender = _mu->CurrentMessageFrame()->option.sender();
    MessageOption ack_option(sender, EV_VM_ALLOCATE_AFFREG_ACK, 0, 0);
    VmAffregAllocateAck ack_msg;

      /* 授权 */
    if (_arpool->Authorize(req._uid, NULL, AuthRequest::CREATE) == false)
    {
        err_oss<<"Authorize Fail !";
        rc =ERR_AFFINITY_REGION_AUTH_FAIL;
        goto error_exit;
    }

    rc = _arpool->Allocate(req);
    if (SUCCESS != rc)
    {
        err_oss<<"Allocate Fail !";
        rc =ERR_AFFINITY_REGION_ALLOC_FAIL;
        goto error_exit;
    }

error_exit:
    ack_msg.detail = err_oss.str();
    ack_msg.state = rc;
    ack_msg.arid= req._oid;

    if (SUCCESS == rc)
    {
        OutPut(SYS_DEBUG,"Create affreg(%s) Success,state:%d,arid:%d\n",
                         req._name.c_str(),
                         ack_msg.state,
                         ack_msg.arid);
    }
    else
    {
        OutPut(SYS_ERROR,"Create Project(%s) Failed,state:%d,detail:%s\n",
                         req._name.c_str(),
                         ack_msg.state,
                         ack_msg.detail.c_str());
    }
    rc = _mu->Send(ack_msg, ack_option);
    return ;
}
/******************************************************************************/
//创建虚拟机部署工作流
STATUS VmCfgManager::CreateDeployWF(MID receiver, VmCfg &vm, const VmOperationReq &req, string &workflow_id)
{
    vector<VmBaseNic> nics;
    bool   ium_flag          = false;
    int32  cpu_num           = vm.get_vcpu();
    int32  tcu               = vm.get_tcu();
    int64  mem_size          = vm.get_memory();
    int64  local_disk_size   = vm.get_local_disk_size( );
    int64  share_disk_size   = vm.get_share_disk_size( );
    string appointed_cluster = vm.get_appointed_cluster();

    Workflow     deploy(VM_WF_CATEGORY, VM_WF_DEPLOY_TO_CLUSTER);
    Workflow     deploy_rollback(VM_WF_CATEGORY, VM_WF_CANCEL_FROM_CLUSTER);
    VmCfgDeployWorkflowLabels wf_labels;
    wf_labels.vid = req._vid;
    deploy.labels = wf_labels;
    deploy.originator = vm._user_name;
    deploy.upstream_sender=receiver;
    deploy.upstream_action_id=req.action_id;
    
    deploy_rollback.labels = wf_labels;
    deploy_rollback.originator = vm._user_name;
    deploy_rollback.upstream_sender=receiver;
    deploy_rollback.upstream_action_id=req.action_id;
    

    SelectClusterLabels         select_culster_labels;

    OutPut(SYS_DEBUG, "Vm deploy workflow!\n");

    if(appointed_cluster.empty())
    {/*从所在的亲和域查询已部署的虚拟机所在的cc作为当前虚拟机所要部署的cc*/
        string cluster;

        VmCfgPool  *vcpool = VmCfgPool::GetInstance();
        if(NULL == vcpool)
        {
            OutPut(SYS_ERROR, "VmCfgManager failed to get VM \n");
            return ERROR;
        }

        if(SUCCESS == vcpool->GetClusterWithARByVid(cluster, vm.get_vid()))
        {
            appointed_cluster = cluster;
        }

    }

    /* 1. 计算的调度请求 */
    Action action_compute(VM_ACT_CATEGORY,
                          VM_ACT_FILTER_COMPUTE,
                          EV_FILTER_CLUSTER_BY_COMPUTE_REQ,
                          MID(MU_VMCFG_MANAGER),
                          MID(MU_CLUSTER_MANAGER));
    FilterClusterByComputeReq  compute_msg(action_compute.get_id(),
                                          cpu_num,
                                          tcu,
                                          mem_size,
                                          local_disk_size,
                                          appointed_cluster);
    action_compute.message_req = compute_msg.serialize();
    action_compute.success_feedback = true;
    action_compute.failure_feedback = true;

    /* 2. 存储的调度请求 */
    Action action_storage(VM_ACT_CATEGORY,
                          VM_ACT_FILTER_STORAGE,
                          EV_STORAGE_GET_CLUSTERS_REQ,
                          MID(MU_VMCFG_MANAGER),
                          MID(PROC_SC, MU_SC_CONTROL));
    if(0 != share_disk_size)
    {
        select_culster_labels._is_need_share_storage = true;
        FilterClusterByShareStorageReq  share_storage_msg(action_storage.get_id(),
                                                          share_disk_size,
                                                          appointed_cluster);

        action_storage.message_req = share_storage_msg.serialize();
        action_storage.success_feedback = true;
        action_storage.failure_feedback = true;
    }

    /* 3. 网络的调度请求 */
    Action action_network(VM_ACT_CATEGORY,
                          VM_ACT_FILTER_NETWORK,
                          EV_VNETLIB_SELECT_CC,
                          MID(MU_VMCFG_MANAGER),
                          MID(MU_VNET_LIBNET));
    vector<VmBaseNic>        base_nics;
    vector<CVNetVnicConfig>  vnet_nics;
    vm.get_nics(base_nics);
    CWatchDogInfo watchdog_info(vm.is_enable_wdg(),
                                vm.get_wdg_period());
    if(vm.is_need_network())
    {
        select_culster_labels._is_need_network = true;
        vm.get_vnet_nics(vnet_nics);

        CVNetSelectCCListMsg  network_msg(action_network.get_id(),
                                           vm.get_vid(),
                                           vm.get_project_id(),
                                           appointed_cluster,
                                           vnet_nics,
                                           watchdog_info);
        action_network.message_req = network_msg.serialize();
        action_network.success_feedback = true;
        action_network.failure_feedback = true;
    }


    /* 4. 挑选集群请求 */
    Action action_choose_cluster(VM_ACT_CATEGORY,
                                 VM_ACT_SELECT_CLUSTER,
                                 EV_SELECT_CLUSTER_FROM_CANDIDATES_REQ,
                                 MID(MU_VMCFG_MANAGER),
                                 MID(MU_VMCFG_MANAGER));
    VmOperationReq  choose_cluster_msg(action_choose_cluster.get_id(),
	                                   req._vid,
                                           (int)(SCHEDULE),
                                           req._user);
    action_choose_cluster.message_req = choose_cluster_msg.serialize();
    action_choose_cluster.labels      = select_culster_labels;
    action_choose_cluster.progress_weight = WEIGHT_CHOOSE_CLUSTER;
    if(0 != share_disk_size)
    {
        action_choose_cluster.progress_weight--;
    }
    if(vm.is_need_network())
    {
        action_choose_cluster.progress_weight--;
    }


    // 5. 获取coredump_url
    Action action_get_coredump_url(VM_ACT_CATEGORY,
                                       VM_ACT_GET_COREDUMP_URL,
                                       EV_FILE_GET_URL_REQ,
                                       MID(MU_VMCFG_MANAGER),
                                       MID(PROC_FILE_MANAGER,MU_FILE_MANAGER));
    if (vm.is_enable_coredump())
    {
        WorkReq     req_info(action_get_coredump_url.get_id());
        action_get_coredump_url.message_req = req_info.serialize();
        action_get_coredump_url.success_feedback = true;
        action_get_coredump_url.progress_weight  = WEIGHT_GET_COREDUMP_URL;
        ium_flag = true;
    }

     /* 6. 分配网络资源 */
    Action action_network_allocate(VM_ACT_CATEGORY,
                                     VM_ACT_ALLOCATE_NETWORK,
                                     EV_VNETLIB_GETRESOURCE_CC,
                                     MID(MU_VMCFG_MANAGER),
                                     MID(MU_VNET_LIBNET));
        // 释放网络资源
    Action action_network_release(VM_ACT_CATEGORY,
                                     VM_ACT_RELEASE_NETWORK,
                                     EV_VNETLIB_FREERESOURCE_CC,
                                     MID(MU_VMCFG_MANAGER),
                                     MID(MU_VNET_LIBNET));
    if(vm.is_need_network())
    {
    	CVNetGetResourceCCMsg  allocate_network_msg(action_network_allocate.get_id(),
    	                                            "",
                                                 	vnet_nics,
                                                 	watchdog_info,
                                                 	vm.get_vid(),
                                                 	vm.get_project_id());
        action_network_allocate.message_req = allocate_network_msg.serialize();

        action_network_allocate.success_feedback = true;
        action_network_allocate.failure_feedback = true;
        action_network_allocate.progress_weight  = WEIGHT_ALLOCATE_NETWORK;
        AllocateNetworkLabels   allocate_network_labels;
        action_network_allocate.labels = allocate_network_labels;

        /*  反操作 */
        CVNetFreeResourceCCMsg  release_network_msg(action_network_release.get_id(),
                                                    vm.get_project_id(),
                                                    vm.get_vid(),
                                                    "",
                                                    vnet_nics,
                                                    watchdog_info);
        action_network_release.message_req = release_network_msg.serialize();
        action_network_release.skipped     = true;

        action_network_allocate.rollback_action_id = action_network_release.get_id();
    }


    // 8. 生成部署消息
    Action action_prepare_deploy(VM_ACT_CATEGORY,
                                 VM_ACT_GENERATE_DEPLOY,
                                 EV_VMCFG_PREPARE_DEPLOY_MSG_REQ,
                                 MID(MU_VMCFG_MANAGER),
                                 MID(MU_VMCFG_MANAGER));
    PrepareDeployMsg  prepare_deploy_msg(action_prepare_deploy.get_id(), req._vid);
    action_prepare_deploy.message_req = prepare_deploy_msg.serialize();
    action_prepare_deploy.progress_weight = WEIGHT_PREPARE_DEPLOY;

    // 生成部署消息,总体进度达到 30%
    if (!vm.is_enable_coredump())
    {
        action_prepare_deploy.progress_weight += WEIGHT_GET_COREDUMP_URL;
    }
    if(0 == share_disk_size)
    {
        action_prepare_deploy.progress_weight += WEIGHT_IMAGE_DEPLOY;
    }
    if(vm.is_need_network())
    {
        action_prepare_deploy.progress_weight += WEIGHT_ALLOCATE_NETWORK;
    }

    // 7. 部署Image到共享存储
    vector<VmBaseImage>::const_iterator  it_img;
    vector<ImagePrepareMsg>              vec_msg;
    int32 share_img_num = vm.share_image_num();
    for(it_img  = vm._images.begin();
        it_img != vm._images.end();
        it_img++)
    {
        if(VM_STORAGE_TO_SHARE ==  it_img->_position )
        {
            ium_flag = true;
            Action action_deploy_image(VM_ACT_CATEGORY,
                                      VM_ACT_DEPLOY_IMAGE,
                                      EV_IMAGE_PREPARE_REQ,
                                      MID(MU_VMCFG_MANAGER),
                                      MID(PROC_IMAGE_MANAGER,MU_IMAGE_MANAGER));
            ImagePrepareMsg prepare_img_msg(action_deploy_image.get_id(),
                                            vm.get_uid(),
                                            it_img->_image_id,
                                            "");
            action_deploy_image.message_req = prepare_img_msg.serialize();
            action_deploy_image.success_feedback = true;
            action_deploy_image.failure_feedback = true;
            action_deploy_image.progress_weight = WEIGHT_IMAGE_DEPLOY/share_img_num;

            deploy.Add(action_choose_cluster, action_deploy_image);
            deploy.Add(action_deploy_image, action_prepare_deploy);
        }
    }


    // 9. 部署到CC
    Action action_deploy_2_cluster(VM_ACT_CATEGORY,
                                  VM_ACT_SEND_DEPLOY,
                                  EV_VM_DEPLOY,
                                  MID(MU_VMCFG_MANAGER),
                                  MID(MU_VM_MANAGER));

    WorkReq     deploy_msg(action_deploy_2_cluster.get_id());
    action_deploy_2_cluster.message_req = deploy_msg.serialize();

    action_deploy_2_cluster.success_feedback = true;
    action_deploy_2_cluster.failure_feedback = true;
    action_deploy_2_cluster.progress_weight = WEIGHT_DEPLOY_2_CLUSTER;

        //从CC撤销
    Action action_cancel_from_cluster(VM_ACT_CATEGORY,
                                      VM_ACT_SEND_CANCEL,
                                      EV_VM_OP,
                                      MID(MU_VMCFG_MANAGER),
                                      MID(MU_VM_MANAGER));

    action_cancel_from_cluster.progress_weight = action_deploy_2_cluster.progress_weight;
    VmOperationReq msg_cancel(action_cancel_from_cluster.get_id(),
                              req._vid,
                              (int)(CANCEL),
                              req._user);
    action_cancel_from_cluster.message_req = msg_cancel.serialize();
    action_cancel_from_cluster.skipped     = true;
    action_deploy_2_cluster.rollback_action_id = action_cancel_from_cluster.get_id();


    // 计算
    deploy.Add(action_compute, action_choose_cluster);
    // 存储
    if(0 != share_disk_size)
    {
        deploy.Add(action_storage, action_choose_cluster);
    }

    // 网络
    if(vm.is_need_network())
    {
        deploy.Add(action_network, action_choose_cluster);
        deploy.Add(action_choose_cluster, action_network_allocate);
        deploy.Add(action_network_allocate, action_prepare_deploy);

        deploy_rollback.Add(action_cancel_from_cluster,action_network_release);
    }
    else
    {
        deploy_rollback.Add(action_cancel_from_cluster);
    }

    // 获取coredump_url
    if (vm.is_enable_coredump())
    {
        deploy.Add(action_choose_cluster, action_get_coredump_url);
        deploy.Add(action_get_coredump_url, action_prepare_deploy);
    }

    if(!ium_flag)
    {
        deploy.Add(action_choose_cluster, action_prepare_deploy);
    }
    // 部署到CC
    deploy.Add(action_prepare_deploy, action_deploy_2_cluster);


    // 10. 创建工作流
    STATUS ret = CreateWorkflow(deploy, deploy_rollback);
    if(SUCCESS != ret)
    {
        OutPut(SYS_ERROR, "VmCfgManager failed to create VM deploy and rollback workflow: %s, %s.\n",\
                       deploy.get_id().c_str(), deploy_rollback.get_id().c_str());
        return ERROR;
    }
    OutPut(SYS_NOTICE, "VmCfgManager created a VM deploy and rollback workflow: %s, %s.\n",\
                       deploy.get_id().c_str(), deploy_rollback.get_id().c_str());

    workflow_id = deploy.get_id();
    return SUCCESS;
}

/******************************************************************************/
//创建虚拟机撤销工作流
STATUS VmCfgManager::CreateCancelWF(MID receiver, VmCfg &vm,
                                           const VmOperationReq &req, string &workflow_id)
{
    OutPut(SYS_DEBUG, "Vm cancel workflow!\n");

    //创建工作流
    Workflow cancel_wf(VM_WF_CATEGORY, VM_WF_CANCEL_FROM_CLUSTER);
    VmCfgDeployWorkflowLabels wf_labels;
    wf_labels.vid = req._vid;
    cancel_wf.labels = wf_labels;
    cancel_wf.originator = vm._user_name;
    cancel_wf.upstream_sender=receiver;
    cancel_wf.upstream_action_id=req.action_id;

    //从CC撤销
    string cluster_name = vm.get_cluster_name();
    Action action_cancel_from_cluster(VM_ACT_CATEGORY,
                                     VM_WF_CANCEL_FROM_CLUSTER,
                                     EV_VM_OP,
                                     MID(MU_VMCFG_MANAGER),
                                     MID(cluster_name,PROC_CC, MU_VM_MANAGER));
    action_cancel_from_cluster.progress_weight = WEIGHT_CANCEL_FROM_CLUSTER;
    VmOperationReq req_msg1(action_cancel_from_cluster.get_id(),
                             req._vid,
                             req._operation,
                             req._user);

    action_cancel_from_cluster.message_req = req_msg1.serialize();

    vector<CVNetVnicConfig>  vnet_nics;
    vm.get_vnet_nics(vnet_nics);
    if(vm.is_need_network())
    {
      // 释放网络资源
        Action action_network_release(VM_ACT_CATEGORY,
                                         VM_ACT_RELEASE_NETWORK,
                                         EV_VNETLIB_FREERESOURCE_CC,
                                         MID(MU_VMCFG_MANAGER),
                                         MID(MU_VNET_LIBNET));


        CWatchDogInfo watchdog_info(vm.is_enable_wdg(),
                                    vm.get_wdg_period());
        CVNetFreeResourceCCMsg  release_network_msg(action_network_release.get_id(),
                                                    vm.get_project_id(),
                                                    vm.get_vid(),
                                                    "",
                                                    vnet_nics,
                                                    watchdog_info);
        action_network_release.message_req = release_network_msg.serialize();

        cancel_wf.Add(action_cancel_from_cluster, action_network_release);
     }
     else
     {
        cancel_wf.Add(action_cancel_from_cluster);
     }

    STATUS ret = CreateWorkflow(cancel_wf);
    if(SUCCESS != ret)
    {
        OutPut(SYS_ERROR, "VmCfgManager failed to cancel VM deploy workflow: %s!!!\n",\
                         cancel_wf.get_id().c_str());
        return ERROR;
    }
    OutPut(SYS_NOTICE, "VmCfgManager created a VM cancel workflow: %s\n", \
                         cancel_wf.get_id().c_str());

    workflow_id = cancel_wf.get_id();
    return SUCCESS;
}


/******************************************************************************/
STATUS VmCfgManager::DealFilterComputeAck(const FilterClusterByComputeAck &ack)
{
    //先过滤一下
    if(true == IsActionFinished(ack.action_id))
    {
        OutPut(SYS_INFORMATIONAL, "DealFilterComputeAck: rcv finished action:\n %s \n", ack.serialize().c_str());
        return ERROR;
    }

    VmOperationAck ack_api(ack.action_id, ERROR, 0, "", INVALID_VID, 0);

    //看操作结果
    if ((SUCCESS == ack.state)
        && (0 != ack._cluster_list.size()))
    {
        SelectClusterLabels  cluster_labels;
        STATUS ret = GetActionLabels(ack.action_id,VM_ACT_SELECT_CLUSTER, cluster_labels);
        if(SUCCESS != ret)
        {
            OutPut(SYS_DEBUG, "DealFilterComputeAck:GetActionLabels failed for action_id:%s \n", ack.action_id.c_str());
        }
        CandidateClusters  candidates(ack._cluster_list);
        cluster_labels._cluster_by_compute= candidates.serialize();

        SetActionLabels(ack.action_id, VM_ACT_SELECT_CLUSTER,cluster_labels);
        FinishAction(ack.action_id);
        return SUCCESS;
    }
    OutPut(SYS_INFORMATIONAL, "DealFilterComputeAck: will rollback, for rcv action ack:\n %s \n",\
           ack.serialize().c_str());
    ack_api.state  = ERR_VMCFG_LACK_COMPUTE;
    ack_api.detail = "ack._cluster_list.size is 0!";
    Ack2Api(ack.action_id, ack_api);
    RollbackWorkflow(ack.action_id);
    return ERROR;

}

/******************************************************************************/
STATUS VmCfgManager::DealFliterShareStorageAck(const FilterClusterByShareStorageAck &ack)
{
    //先过滤一下
    if(true == IsActionFinished(ack.action_id))
    {
        OutPut(SYS_INFORMATIONAL, "DealFliterShareStorageAck: rcv finished action:\n %s \n", ack.serialize().c_str());
        return ERROR;
    }

    VmOperationAck ack_api(ack.action_id, ERROR, 0, "", INVALID_VID, 0);
    //看操作结果
    if ((SUCCESS == ack.state)
        && (0 !=  ack._cluster_list.size()))
    {
        SelectClusterLabels  cluster_labels;
        STATUS ret = GetActionLabels(ack.action_id, VM_ACT_SELECT_CLUSTER, cluster_labels);
    if(SUCCESS != ret)
    {
            OutPut(SYS_DEBUG, "DealFliterShareStorageAck: GetActionLabels failed for action_id :%s !\n", \
                    ack.action_id.c_str());
        }
        CandidateClusters  candidates(ack._cluster_list);
        cluster_labels._cluster_by_share_storage= candidates.serialize();

        SetActionLabels(ack.action_id, VM_ACT_SELECT_CLUSTER,cluster_labels);
        FinishAction(ack.action_id);
        return SUCCESS;
    }

    OutPut(SYS_INFORMATIONAL, "DealFliterShareStorageAck: will rollback, for rcv action ack:\n %s \n",\
           ack.serialize().c_str());
    ack_api.state = ERR_VMCFG_LACK_SHARE_STORAGE;
    Ack2Api(ack.action_id, ack_api);
    RollbackWorkflow(ack.action_id);
    return ERROR;
}

/******************************************************************************/
STATUS VmCfgManager::DealFilterNetworkAck(const CVNetSelectCCListMsgAck &ack)
{
    //先过滤一下
    if(true == IsActionFinished(ack.action_id))
    {
        OutPut(SYS_INFORMATIONAL, "DealFilterNetworkAck: rcv finished action:\n %s \n", ack.serialize().c_str());
        return ERROR;
    }

    VmOperationAck ack_api(ack.action_id, ERROR, 0, "", INVALID_VID, 0);

    //看操作结果
    if ((SUCCESS == ack.state)
        && (0 != ack.m_select_cc_list.size()))
    {
        SelectClusterLabels  cluster_labels;
        STATUS ret = GetActionLabels(ack.action_id, VM_ACT_SELECT_CLUSTER, cluster_labels);
        if(SUCCESS != ret)
        {
            OutPut(SYS_DEBUG, "DealFilterNetworkAck:GetActionLabels failed for action_id:%s !\n",\
                   ack.action_id.c_str());
        }
        CandidateClusters  candidates(ack.m_select_cc_list);
        cluster_labels._cluster_by_network= candidates.serialize();

        SetActionLabels(ack.action_id, VM_ACT_SELECT_CLUSTER,cluster_labels);
        FinishAction(ack.action_id);
        return SUCCESS;
    }

    OutPut(SYS_INFORMATIONAL, "DealFilterNetworkAck: will rollback, for rcv action ack:\n %s \n",\
           ack.serialize().c_str());
    ack_api.state = ERR_VMCFG_LACK_NETWORK;
    Ack2Api(ack.action_id, ack_api);
    RollbackWorkflow(ack.action_id);
    return ERROR;
}

/******************************************************************************/
STATUS VmCfgManager::DealSelectCluster(const VmOperationReq &req)
{
    // 1. 先校验一下
    if(true == IsActionFinished(req.action_id))
    {
        OutPut(SYS_INFORMATIONAL, "DealSelectCluster: rcv finished action:\n %s \n", req.serialize().c_str());
        return ERROR;
    }

    VmOperationAck ack_api(req.action_id, ERROR, 0, "", INVALID_VID, 0);

    SelectClusterLabels  cluster_labels;
    STATUS ret = GetActionLabels(req.action_id, cluster_labels);
    if(SUCCESS != ret)
    {
        OutPut(SYS_DEBUG, "DealSelectCluster: GetActionLabels failed, action_id: %s\n",\
                req.action_id.c_str());      
        ack_api.state  = ERROR;
        ack_api.detail = "get actionlabel failed";
        Ack2Api(req.action_id, ack_api);
        RollbackWorkflow(req.action_id);
        return ERROR;
    }

    // 2. 过滤出同时满足计算、存储和网络的候选者
    CandidateClusters  compute;
    CandidateClusters  storage;
    CandidateClusters  network;
    vector<string>  vec_tmp;
    vector<string>  vec_result;

    bool is_success = compute.deserialize(cluster_labels._cluster_by_compute);
    SkyAssert(is_success);
    sort(compute._clusters.begin(), compute._clusters.end());
    vec_result = compute._clusters;

    if(cluster_labels._is_need_share_storage)
    {
        storage.deserialize(cluster_labels._cluster_by_share_storage);
        sort(storage._clusters.begin(), storage._clusters.end());

        set_intersection(vec_result.begin(), vec_result.end(),
                          storage._clusters.begin(), storage._clusters.end(),
                          inserter(vec_tmp, vec_tmp.begin()));
        vec_result = vec_tmp;
    }

    if(cluster_labels._is_need_network)
    {
        network.deserialize(cluster_labels._cluster_by_network);
        sort(network._clusters.begin(), network._clusters.end());
        vec_tmp.clear();
        set_intersection(vec_result.begin(), vec_result.end(),
                          network._clusters.begin(), network._clusters.end(),
                          inserter(vec_tmp,vec_tmp.begin()));
        vec_result = vec_tmp;
    }

    if (0 == vec_result.size())
    {
        ack_api.state  = ERR_VMCFG_NO_SUITABLE_CLUSTER;
        ack_api.detail = "no cluster is suitable for the vm";
        Ack2Api(req.action_id, ack_api);
        RollbackWorkflow(req.action_id);
        return ERROR;
    }

    // 3. 最终选举出一个，并入库
    string  des_cluster;
    SelectPolicy(vec_result, des_cluster);

    cluster_labels._des_cluster = des_cluster;
    SetActionLabels(req.action_id, cluster_labels);

    // 4. 更新image部署
    vector<string> deploy_img_actions;
    vector<string>::const_iterator iter;
    ret = GetActionsByName(req.action_id, VM_ACT_DEPLOY_IMAGE, deploy_img_actions);
    if(ret != SUCCESS)
    {
        OutPut(SYS_DEBUG, "s% GetActionsByName failed!\n", req.action_id.c_str());
        ack_api.state  = ERROR;
        ack_api.detail = "GetActionsByName failed for deploy image!";
        Ack2Api(req.action_id, ack_api);
        VmRollbackWorkflow(req.action_id);
        return ERROR;
    }

    for(iter = deploy_img_actions.begin(); iter != deploy_img_actions.end(); iter++)
    {
        string image_req;
        ret = GetActionMessageReq(*iter,  image_req);
        if(ret != SUCCESS || image_req.empty())
        {
            OutPut(SYS_DEBUG, "s% GetActionMessageReq failed!\n", (*iter).c_str());
            ack_api.state  = ERROR;
            ack_api.detail = "GetActionMessageReq failed!";
            Ack2Api(req.action_id, ack_api);
            VmRollbackWorkflow(req.action_id);
            return ERROR;
        }

        ImagePrepareMsg  prepare_image;
        prepare_image.deserialize(image_req);
        prepare_image._cluster_name = cluster_labels._des_cluster;
        UpdateActionReq(*iter, prepare_image);
    }

    // 5. 更新网络申请
    if(cluster_labels._is_need_network)
    {
        vector<string>     network_allocate_actions;
        ret = GetActionsByName(req.action_id,
                                VM_ACT_ALLOCATE_NETWORK,
                                network_allocate_actions);
        if(ret != SUCCESS)
        {
            OutPut(SYS_DEBUG, "s% GetActionsByName failed!\n", req.action_id.c_str());
            ack_api.state  = ERROR;
            ack_api.detail = "GetActionsByName failed for allocate network!";
            Ack2Api(req.action_id, ack_api);            
            VmRollbackWorkflow(req.action_id);
            return ERROR;
        }
        SkyAssert(0 != network_allocate_actions.size());

        string str_msg;
        GetActionMessageReq(*network_allocate_actions.begin() , str_msg);

        CVNetGetResourceCCMsg  allocate_network;
        allocate_network.deserialize(str_msg);
        allocate_network.m_cc_application   = des_cluster;
        UpdateActionReq(*network_allocate_actions.begin(), allocate_network);
    }

    // 6. 更新cancel_from_cluster
    string rollback_workflow_id;
    vector<string>  cancel_actions;
    ret = GetRollbackWorkflow(req.action_id, rollback_workflow_id);
    SkyAssert(SUCCESS == ret);

    ret = GetActionsByName(rollback_workflow_id,VM_ACT_SEND_CANCEL ,  cancel_actions);
    SkyAssert(SUCCESS == ret);

    MID  receiver(des_cluster, PROC_CC, MU_VM_MANAGER);
    ret = UpdateActionReceiver(*cancel_actions.begin(), receiver);
    SkyAssert(SUCCESS == ret);

    FinishAction(req.action_id);

    return SUCCESS;
}

/******************************************************************************/
STATUS VmCfgManager::SelectPolicy(const vector<string> &cluster_in, string &cluster_out)
{
    time_t          temp;
    int32          findnum = cluster_in.size();
    int32          postion;

    if (findnum == 0)
    {
        return ERROR;
    }

    /* 获取一个当前的系统运行秒数 */
    time(&temp);

    /* 根据随机数取当前位置 */
    postion = temp%(findnum);

    cluster_out = cluster_in.at(postion);
    return SUCCESS;
}

/******************************************************************************/
STATUS VmCfgManager::DealAllocateNetworkAck(const CVNetGetResourceCCMsgAck &ack)
{
    // 1. 先校验
    if(true == IsActionFinished(ack.action_id))
    {
        OutPut(SYS_INFORMATIONAL, "DealAllocateNetworkAck: rcv finished action:\n %s \n", ack.serialize().c_str());
        return ERROR;
    }

    VmOperationAck ack_api(ack.action_id, ERROR, 0, "", INVALID_VID, 0);
    if (SUCCESS != ack.state) //失败了,需回滚
    {
        ack_api.state = ERR_VMCFG_ALLOCATE_NETWORK_TC;
        Ack2Api(ack.action_id, ack_api);
        RollbackWorkflow(ack.action_id);
        return ERROR;
    }

    // 2. 将分配的网卡信息存入ActionLabels
    AllocateNetworkLabels  allocate_network_labels;
    STATUS ret = GetActionLabels(ack.action_id, allocate_network_labels);
    if(SUCCESS != ret)
    {
        OutPut(SYS_DEBUG, "DealAllocateNetworkAck: get AllocateNetworkLabels failed, action_id: %s\n",\
                ack.action_id.c_str());

        ack_api.state = ERROR;
        Ack2Api(ack.action_id, ack_api);
        RollbackWorkflow(ack.action_id);
        return ERROR;
    }
    AllocatedNetworkRes  res(ack.m_VmNicCfgList);
    allocate_network_labels._allocated_nics = res.serialize();
    SetActionLabels(ack.action_id, VM_ACT_ALLOCATE_NETWORK, allocate_network_labels);

    FinishAction(ack.action_id);

    return SUCCESS;
}

/******************************************************************************/
void VmCfgManager::DealImagePrepareAck(const  ImagePrepareAckMsg &ack)
{

    if(true == IsActionFinished(ack.action_id))
    {
        OutPut(SYS_INFORMATIONAL, "DealImageOperateAck: rcv finished action:\n %s \n",\
                 ack.serialize().c_str());
        return ;
    }

    if(SUCCESS != ack.state)
    {
        VmOperationAck ack_api(ack.action_id,
                              ERR_VMCFG_DEPLOY_IMAGE_FAILED,
                              0,
                              "deploy image to shared disk failed",
                              INVALID_VID,
                              DEPLOY);

            Ack2Api(ack.action_id, ack_api);
            VmRollbackWorkflow(ack.action_id);
            DeleteWorkflow(ack.action_id);
            return ;
    }

    FinishAction(ack.action_id);

    return;
}

/******************************************************************************/
void VmCfgManager::DealCoreDumpUrlAck(const  UploadUrl &ack)
{
    STATUS ret = ERROR;
    VmOperationAck ack_api(ack.action_id, ERROR, 0, "", INVALID_VID, DEPLOY);

    if(true == IsActionFinished(ack.action_id))
    {
        OutPut(SYS_INFORMATIONAL, "DealCoreDumpUrlAck: rcv finished action:\n %s \n",\
                 ack.serialize().c_str());
        return ;
    }

    if(SUCCESS != ack.state)
    {
        ack_api.state  = ERR_VMCFG_GET_COREDUMP_URL;
        Ack2Api(ack.action_id, ack_api);
        VmRollbackWorkflow(ack.action_id);
        return ;
    }

    //更新动作标签GetCoreDumpUrlLabels中的str_url字段
        GetCoreDumpUrlLabels ac_labels;
        ret = GetActionLabels(ack.action_id, ac_labels);
        if(SUCCESS != ret)
        {
            OutPut(SYS_DEBUG, "s% GetActionLabels failed!\n", ack.action_id.c_str());
            ack_api.state  = ERROR;
            ack_api.detail = "GetActionLabels failed!";
            Ack2Api(ack.action_id, ack_api);
            VmRollbackWorkflow(ack.action_id);
            return;
        }

    ac_labels.str_url = ack.serialize();
    SetActionLabels(ack.action_id, ac_labels);
    FinishAction(ack.action_id);

    return;
}

/******************************************************************************/
STATUS VmCfgManager::DealFreeNetworkAck(const CVNetFreeResourceCCMsgAck &ack)
{
    //先校验一下
    if(true == IsActionFinished(ack.action_id))
    {
        OutPut(SYS_INFORMATIONAL, "DealFreeNetworkAck: rcv finished action:\n %s \n", ack.serialize().c_str());
        return ERROR;
    }

    if (SUCCESS != ack.state) //失败了,需回滚
    {
        RestartAction(ack.action_id);
        OutPut(SYS_INFORMATIONAL, "DealFreeNetworkAck: rollback action ack fail,restart this action:\n %s \n", \
               ack.serialize().c_str());
        return ERROR;
    }

    FinishAction(ack.action_id);

    return SUCCESS;
}

/******************************************************************************/
void VmCfgManager::DealPrepareDeployMsgReq(const  PrepareDeployMsg &req)
{
    MID receiver;
    STATUS ret = ERROR;
    STATUS tRet =ERROR;
    if(true == IsActionFinished(req.action_id))
    {
        OutPut(SYS_INFORMATIONAL, "DealPrepareDeployMsgReq: rcv finished action:\n %s \n", \
              req.serialize().c_str());
        return ;
    }

    VmOperationAck ack_api(req.action_id, ERROR, 0, "", INVALID_VID, DEPLOY);
    VMDeployInfo deploy;

    if(SUCCESS != GenerateDeployMsg(req, deploy))
    {
        ack_api.state  = ERR_VMCFG_PREPARE_DEPLOY;
        ack_api.detail = "generate deploy msg failed !";
        Ack2Api(req.action_id, ack_api);
        VmRollbackWorkflow(req.action_id);
        return ;
    }

    VmCfg vmcfg;
    tRet = _vcpool->GetVmById(vmcfg, req._vid);
    if( SUCCESS != tRet)
    {
         ack_api.state  = ERROR_OBJECT_NOT_EXIST;
         ack_api.detail = "VM not exist!";
         Ack2Api(req.action_id, ack_api);
         VmRollbackWorkflow(req.action_id);
         return;
    }
        // 1. 获取将要部署的cluster信息
        SelectClusterLabels   ac_labels_cluster;
        ret = GetActionLabels(req.action_id, VM_ACT_SELECT_CLUSTER, ac_labels_cluster);
        if(SUCCESS != ret)
        {
            OutPut(SYS_DEBUG, "s% GetActionLabels failed!\n", req.action_id.c_str());
            SkyAssert(false);
        }
        string cluster_name = ac_labels_cluster._des_cluster;

        vector<string> actions;
        ret = GetNextActions(req.action_id, actions);
        if(SUCCESS != ret || (actions.empty()) )
        {
            VmRollbackWorkflow(req.action_id);
            WorkAck ack(req.action_id);
            ack.state  = ERROR;
            ack.detail = "GetNextActions failed, workflow is rolling back.";
            UpdateActionAck(req.action_id, ack.serialize());
            return;
        }

        MID dest_mid(cluster_name, PROC_CC, MU_VM_MANAGER);
        UpdateActionReceiver(*actions.begin(), dest_mid);

        // 更新部署到CC的消息
        deploy.action_id = *actions.begin();
        UpdateActionReq(*actions.begin(), deploy);
        FinishAction(req.action_id);

        return;

}

void VmCfgManager::DealAck(const VmOperationAck &ack)
{    
    if ((DEPLOY == ack._operation) || (CANCEL == ack._operation))
    {
        DealCCAck(ack);
    }

    ResponseWorkAck(ack);

    return;
}

/******************************************************************************/
void VmCfgManager::DealCCAck( const  VmOperationAck &ack)
{
    VmOperationAck ack_api(ack.action_id,
                           ack.state,
                           ack.progress,
                           "",
                           ack._vid,
                           ack._operation);

    if(true == IsActionFinished(ack.action_id))
    {
        OutPut(SYS_INFORMATIONAL, "DealCCAck: rcv finished action:\n %s \n", \
              ack.serialize().c_str());
        return ;
    }

    VmCfg  vmcfg;
    STATUS ret = _vcpool->GetVmById(vmcfg, ack._vid);
    if (SUCCESS != ret)
    {
        OutPut(SYS_DEBUG,"*** VM (%d) not exist in TC, _result:%d, _error:%s***\n",
                   ack._vid, ack.state, ack.detail.c_str());
        ack_api.state    = ERROR_OBJECT_NOT_EXIST;
        ack_api.progress = 0;
        goto FINISH_ACTION;
    }

    if (DEPLOY == ack._operation)
    {
        if (SUCCESS != ack.state)
        {
           VmRollbackWorkflow(ack.action_id);
           return ;
        }

         //获取cluster信息
        SelectClusterLabels ac_labels_cluster;
        ret = GetActionLabels(ack.action_id, VM_ACT_SELECT_CLUSTER, ac_labels_cluster);
        if(SUCCESS != ret)
        {
            OutPut(SYS_DEBUG, "s% GetActionLabels failed!\n", ack.action_id.c_str());
            ack_api.state  = ERROR;
            ack_api.detail = "GetActionLabels failed!";
            Ack2Api(ack.action_id, ack_api);
            VmRollbackWorkflow(ack.action_id);
            return;
        }
        SkyAssert(!ac_labels_cluster._des_cluster.empty());

        //更新网卡
        if (true == ac_labels_cluster._is_need_network)
        {
            AllocateNetworkLabels ac_labels;
            ret = GetActionLabels(ack.action_id, VM_ACT_ALLOCATE_NETWORK, ac_labels);
            if(SUCCESS != ret)
            {
                OutPut(SYS_DEBUG, "s% GetActionLabels failed!\n", ack.action_id.c_str());
                ack_api.state     = ERROR;
                ack_api.progress  = 0;
                goto FINISH_ACTION;
            }

            AllocatedNetworkRes  action_nics;
            action_nics.deserialize(ac_labels._allocated_nics);
            vmcfg.update_nics(action_nics._nics);
        }
        //更新集群名
        vmcfg.set_cluster_name(ac_labels_cluster._des_cluster);
        ret = _vcpool->Update(vmcfg);
        if (SUCCESS != ret)
        {
            OutPut(SYS_WARNING, "%s: update VM(%d) deployed cluster(%s) failed \n",
                                Enum2Str::VmOpStr(ack._operation),
                                ack._vid,
                                ac_labels_cluster._des_cluster.c_str());
            ack_api.state     = ERROR;
            ack_api.progress  = 0;
            goto FINISH_ACTION;
        }

     }
     else if(CANCEL == ack._operation)
     {
        if (SUCCESS != ack.state)
        {
           RestartAction(ack.action_id);
           return ;
        }
        //更新deployed_cluster和网卡中的vsi字段为空
        vmcfg.clear_vsi();
        vmcfg.set_cluster_name("");
        ret = _vcpool->Update(vmcfg);
        if (SUCCESS != ret)
        {
           OutPut(SYS_WARNING, "%s: erase VM(%d) deployed cluster failed \n",
                           Enum2Str::VmOpStr(ack._operation), ack._vid);
            ack_api.state     = ERROR;
            ack_api.progress  = 0;
            goto FINISH_ACTION;
        }
     }
     else if(MODIFY_VMCFG == ack._operation)
     {
        //DealModifyAck(ack);
     }

FINISH_ACTION:
    //处理完了，结束这个动作环节
    Ack2Api(ack.action_id, ack_api);
    FinishAction(ack.action_id);
    return;
}

/******************************************************************************/
STATUS VmCfgManager::GenerateDeployMsg(const  PrepareDeployMsg &req, VMDeployInfo &req_msg)
{
    VmCfg vmcfg;

    STATUS tRet = _vcpool->GetVmById(vmcfg, req._vid);
    if( SUCCESS!= tRet)
    {
        OutPut(SYS_ERROR, "VM (%ld) not exist!\n", req._vid);
        return ERR_OBJECT_NOT_EXIST;
    }

    // 1. 基本信息
   STATUS rc = _vcpool->ToVMDeployInfo( vmcfg, req_msg);
    if (SUCCESS != rc  )
    {
       OutPut(SYS_ERROR, "Failed to construct deploy msg of VM(%ld).\n", req._vid);
       return ERROR;
    }

    // 2.  获取互斥关系
    ostringstream  where_string;
    where_string << "vid1 = " << req._vid
                 << " OR vid2 = "<< req._vid <<endl;

    vector<ApiVmCfgMap> map_vm;
    vector<ApiVmCfgMap>::const_iterator iter;
    rc = _vcpool->SearchRepelTab(map_vm, where_string.str());
    if (rc != SUCCESS)
    {
        OutPut(SYS_ERROR, "Query vm relation failed !\n");
    }
    for(iter = map_vm.begin(); iter != map_vm.end(); iter ++)
    {
        if (iter->src == req._vid)
        {
           req_msg._requirement._repel_vms.push_back(iter->des);
        }
        else
        {
           req_msg._requirement._repel_vms.push_back(iter->src);
        }
    }


    AffinityRegion ar;
    rc = _vcpool->GetARByVid(req._vid,ar);
    if ((rc != SUCCESS)&&(rc != ERROR_OBJECT_NOT_EXIST))
    {
        OutPut(SYS_ERROR, "Query affinity region by vid failed!\n");
        return ERROR;
    }
    else if(rc == SUCCESS)
    {
        req_msg._requirement._affinity_region_id= ar._oid;
        req_msg._requirement._affinity_region_level = ar._level;
        req_msg._requirement._affinity_region_forced= ar._is_forced;
    }

    // 3. 获取coredump_url信息
    if (vmcfg.is_enable_coredump())
    {
         //更新动作标签GetCoreDumpUrlLabels中的str_url字段
         GetCoreDumpUrlLabels ac_labels_url;
         STATUS ret = GetActionLabels(req.action_id, VM_ACT_GET_COREDUMP_URL, ac_labels_url);
         if(SUCCESS != ret)
         {
            OutPut(SYS_DEBUG, "s% GetActionLabels failed!\n", req.action_id.c_str());
             SkyAssert(false);
        }
        req_msg._config._core_dump_url.deserialize(ac_labels_url.str_url);
     }
    return  SUCCESS;
}

STATUS VmCfgManager::BatchModifyVmCfg(const MessageFrame &message)
{
    VmCfgBatchModify modify_req;
    SkyAssert(modify_req.deserialize(message.data));

    VmOperationAck ack;

    if (modify_req._modifications.empty())
    {
        ack.state = SUCCESS;
        ack.progress = 100;
    }
    else
    {
        BatchModifyVmCfg(modify_req, ack);
    }

    Ack2Api(message.option.sender(), ack);
    return 0;
}

STATUS VmCfgManager::BatchModifyVmCfg(const VmCfgBatchModify &modify_req, VmOperationAck &result)
{
    string user_name;
    _vcpool->SelectColumn("user_pool", "name", "oid = " + to_string<int64>(modify_req._modifications[0]._uid, dec), user_name);

    VmCfgModifyWorkflow batch_modify_wf(VM_WF_BATCH_MODIFY_VMCFG);
    batch_modify_wf.originator = user_name;

    vector<VmCfgModifyAction> batch_modify_actions;
    batch_modify_actions.assign(modify_req._modifications.begin(), modify_req._modifications.end());

    batch_modify_wf.AddModifyActions(batch_modify_actions);

    STATUS ret = CreateWorkflow(batch_modify_wf);

    result.state = ret;
    if(SUCCESS != ret)
    {
        OutPut(SYS_WARNING, "BatchModifyVmCfg failed to created workflow: !!!\n" );
        SkyAssert(false);
        result.detail = "BatchModifyVmCfg failed to created workflow: !!!\n";
    }
    else
    {
        result.state = ERROR_ACTION_RUNNING;
        result._workflow_id = batch_modify_wf.get_id();
    }

    return ret;
}

STATUS VmCfgManager::CheckModifyValid(const VmCfgBatchModify &modify_req, ostringstream &err_oss)
{
    #if 0
    //检查虚拟机是否存在
    VmCfg  vmcfg;
    STATUS tRet = _vcpool->GetVmById(vmcfg, modify_req._vid);
    if (SUCCESS != tRet)
    {
        err_oss << "*** VM (" << modify_req._vid<< ") not exist in TC.***\n";
        return ERR_OBJECT_NOT_EXIST;
    }

    bool is_deploying = _vcpool->IsVmDeploying(modify_req._vid);
    bool is_canceling = _vcpool->IsVmCanceling(modify_req._vid);

    if(true == is_deploying)
    {
         err_oss<<"VM is deploying";
         return ERR_VMCFG_STATE_FORBIDDEN;
    }

    if(true == is_canceling)
    {
         err_oss<<"VM is canceling";
         return ERR_VMCFG_STATE_FORBIDDEN;
    }
#endif
    return SUCCESS;
}

class ModifyVmCfgElementMethods
{
public:
    ModifyVmCfgElementMethods(){};
    ModifyVmCfgElementMethods(VmCfgElement element, const string &table_name, const string &column_name,
                                        const string &key) 
                        :  _table_name(table_name), _column_name(column_name), _keyword_in_table(key)
    {
        AddNewModify(element, this);
    }

    virtual ~ModifyVmCfgElementMethods(){};
    
    static map<VmCfgElement, ModifyVmCfgElementMethods*> modify_funs;

    static STATUS AddNewModify(VmCfgElement element, ModifyVmCfgElementMethods *modify)
    {
        modify_funs[element] = modify;
        return 0;
    }

    static ModifyVmCfgElementMethods *GetElementMethod(VmCfgElement element)
    {
        return modify_funs[element];
    }

    virtual STATUS ImplandAck(const VmModifyReq &req, MID &sender, MessageUnit *mu)
    {
        ostringstream err;
        STATUS ret = Implement(req, sender, err);

        VmOperationAck result;
        GenResult(req, ret, err, result);

        AcktoSend(mu, sender, result);

        return ret;
    }

    template< typename T>
    STATUS TransParamfromApitoVmOp(const VmCfgModifyIndividualItem &modification, T &config)
    {
        from_string<T>(config, modification._args.begin()->second, dec);
        return 0;
    }

    virtual STATUS Implement(const VmModifyReq &req, const MID &sender, ostringstream &err)
    {
        if (IsDoing(req))
        {
            return ERROR_ACTION_RUNNING;
        }

        STATUS ret = Check(req._modification, err);
        if (ret != SUCCESS)
        {
            return ret;
        }

        return EffectModification(req, sender);
    }

    bool IsDoing(const VmModifyReq &req)
    {
        return !FindVmOpWorkflow(req).empty();
    }

    string FindVmOpWorkflow(const VmModifyReq &req)
    {
        ostringstream label_where;
        label_where << "upstream_action_id = '" << req.action_id << "'";

        return FindWorkflow(label_where.str());
    }

    string FindModifyWorkflow(int64 vid)
    {
        ostringstream label_where;
        label_where << "label_int64_1 = " << vid << endl;

        return FindWorkflow(VM_WF_CATEGORY, VM_WF_MODIFY_INDIVIDUAL_VMCFG, label_where.str());
    }

    virtual STATUS Check(const VmCfgModifyIndividualItem &modification, ostringstream &err)
    {
        VmCfgPool *vc_pool = VmCfgPool::GetInstance();

        VmCfg  vmcfg;
        STATUS ret = vc_pool->GetVmById(vmcfg, modification._vid);
        if (SUCCESS != ret)    //GetVmById接口内部返回2套系统的返回值，以后可能会有问题
        {
            err << "vm(" << modification._vid << ") has not allocated!" <<endl;
            return ERROR_OBJECT_NOT_EXIST;
        }

        if (vc_pool->IsVmDeploying(modification._vid))
        {
            err<<"VM ("<< modification._vid <<") is deploying, can not modify";
            return ERR_VMCFG_MODIFY_FORBIDDEN ;
        }

        if (vc_pool->IsVmCanceling(modification._vid))
        {
            err<<"VM ("<< modification._vid <<") is canceling, can not modify";
            return ERR_VMCFG_MODIFY_FORBIDDEN ;
        }

        if (ret != SUCCESS)
        {
            return ret;
        }
        return SUCCESS;
    }

    virtual STATUS EffectModification(const VmModifyReq &req, const MID &sender)
    {
        return EffectThisLayer(req._modification);
    }

    void GenResult(const VmModifyReq &req, int state, const ostringstream &err, 
                        VmOperationAck &result)
    {
        result._vid         = req._vid;
        result._operation   = MODIFY_VMCFG;
        result.state        = state;
        result.action_id    = req.action_id;

        if (state == SUCCESS)
        {
            result.progress = 100;
        }
        else if (state == ERROR_ACTION_RUNNING)
        {
            result._workflow_id = FindModifyWorkflow(result._vid);
            result.progress = GetWorkflowProgress(result._workflow_id);
        }
        else
        {
            result.progress = 0;
            result.detail = err.str();
        }
    }

    int AcktoSend(MessageUnit *mu, const MID &sender, VmOperationAck &ack)
    {
        if (ack.Validate())
        {
            MessageOption option(sender, EV_VM_OP_ACK, 0, 0);
            return mu->Send(ack, option);
        }

        return ERROR;
    }

    STATUS EffectThisLayer(const VmCfgModifyIndividualItem &modification)
    {
        return RecordtoDatabase(modification);
    }

    STATUS EffectLowerLayer(const VmModifyReq &req, const MID &sender)
    {
        if (IsVmNotDeployed(req._vid))
        {
            return EffectThisLayer(req._modification);
        }

        STATUS ret = SendtoLowerLayerbyWorkflow(req, sender);
        if ( ret == SUCCESS)
        {
            ret = ERROR_ACTION_RUNNING;
        }

        return ret;
    }

    virtual STATUS RecordtoDatabase(const VmCfgModifyIndividualItem &modification)
    {
        ostringstream cmd;
        cmd << "update " << _table_name << " set " << _column_name << " = " << modification._args.begin()->second
            << " where " << _keyword_in_table << " = " << modification._vid;

        VmCfgPool *vc_pool = VmCfgPool::GetInstance();
        int ret = vc_pool->Execute(cmd);

        if (ret == SQLDB_OK)
        {
            return SUCCESS;
        }
        else
        {
            return ERROR;
        }

        //todo: 改为调用VmCfgPool::Update接口
    }

    bool IsVmNotDeployed(int64 vid)
    {
        return GetLowerLayer(vid).empty();
    }

    virtual STATUS SendtoLowerLayerbyWorkflow(const VmModifyReq &req, const MID &sender)
    {
        VmCfgModifyWorkflow modify_wf(VM_WF_CATEGORY, VM_WF_MODIFY_INDIVIDUAL_VMCFG);
        modify_wf._vid                  = req._vid;
        modify_wf.upstream_sender       = sender;
        modify_wf.upstream_action_id    = req.action_id;

        VmCfgModifyAction modify_actions(req._modification);        
        GenSendtoLowerLayerAction(req._modification, modify_actions);

        modify_wf.Add(modify_actions);
        
        return CreateWorkflow(modify_wf);
    }

    void GenSendtoLowerLayerAction(const VmCfgModifyIndividualItem &modification, VmCfgModifyAction &action)
    {
        action.receiver = MID(GetLowerLayer(modification._vid), PROC_CC, MU_VM_MANAGER);

        action.success_feedback = FEEDBACK_MODIFY_SUCCESS;
        action.failure_feedback = FEEDBACK_MODIFY_FAILED;
    }

    virtual string GetLowerLayer(int64 vid)
    {
        VmCfg  vmcfg;
        VmCfgPool *vc_pool = VmCfgPool::GetInstance();
        STATUS tRet = vc_pool->GetVmById(vmcfg, vid);
        if (tRet == SUCCESS)
        {
            return vmcfg.get_cluster_name();
        }

        return "";
    }

    string      _table_name;
    string      _column_name;
    string      _keyword_in_table;
};
map<VmCfgElement, ModifyVmCfgElementMethods*> ModifyVmCfgElementMethods::modify_funs;

ModifyVmCfgElementMethods modify_vm_name(VM_CFG_NAME, "vmcfg_pool", "name", "oid");

class ModifyVmCfgtoCC : public ModifyVmCfgElementMethods
{
public:
    ModifyVmCfgtoCC(VmCfgElement element, const string &table_name, const string &column_name,
                                        const string &key) : ModifyVmCfgElementMethods(element, table_name, column_name, key)
    {
    }

    ~ModifyVmCfgtoCC(){};

    virtual STATUS EffectModification(const VmModifyReq &req, const MID &sender)
    {
        return EffectLowerLayer(req, sender);
    }
};
ModifyVmCfgtoCC modify_vm_memory(VM_CFG_MEMORY, "vmcfg_pool", "memory", "oid");
ModifyVmCfgtoCC modify_vm_watch_dog(VM_CFG_WATCH_DOG, "vmcfg_pool", "watchdog", "oid");

class ModifyVmCfgCPU : public ModifyVmCfgElementMethods
{
public:
    ModifyVmCfgCPU() : ModifyVmCfgElementMethods(VM_CFG_CPU, "vmcfg_pool", "vcpu", "oid")
    {
    }

    ~ModifyVmCfgCPU(){};
    virtual STATUS EffectModification(const VmModifyReq &req, const MID &sender)
    {
        return EffectLowerLayer(req, sender);
    }

    virtual STATUS RecordtoDatabase(const VmCfgModifyIndividualItem &modification)
    {
        ostringstream cmd;
        const string tcu = modification._args.find("tcu")->second;
        const string vcpu = modification._args.find("vcpu")->second;
        
        cmd << "update " << _table_name << " set tcu = " << tcu
                                        << ", vcpu = " << vcpu
            << " where oid = " << modification._vid;

        VmCfgPool *vc_pool = VmCfgPool::GetInstance();
        int ret = vc_pool->Execute(cmd);

        if (ret == SQLDB_OK)
        {
            return SUCCESS;
        }
        else
        {
            return ERROR;
        }
    }
};
ModifyVmCfgCPU modify_vm_cpu;

class ModifyVmCfgMachine : public ModifyVmCfgElementMethods
{
public:
    ModifyVmCfgMachine() : ModifyVmCfgElementMethods(VM_CFG_MACHINE, "vmcfg_image", "", "vid")
    {
    }

    ~ModifyVmCfgMachine(){};
    virtual STATUS EffectModification(const VmModifyReq &req, const MID &sender)
    {
        return EffectLowerLayer(req, sender);
    }

    virtual STATUS RecordtoDatabase(const VmCfgModifyIndividualItem &modification)
    {
        vector<VmBaseImage> images;
        VmBaseImage image;
        AnalysisArgsto(modification, image);
        images.push_back(image);

        STATUS ret;
        VmBaseImagePool *p_image = VmBaseImagePool::GetInstance();
        if (modification._operation == INSERT_VMCFG)
        {
            ret = p_image->InsertVmBaseImages(images, modification._vid, true);
        }
        else if (modification._operation == DELETE_VMCFG)
        {
            ret = p_image->DropVmBaseImages(image);
        }
        
        if (ret == SQLDB_OK)
        {
            return SUCCESS;
        }
        else
        {
            return ERROR;
        }
    }

    virtual STATUS SendtoLowerLayerbyWorkflow(const VmModifyReq &req, const MID &sender)
    {
        VmBaseImage image;
        AnalysisArgsto(req._modification, image);

        VmCfgModifyWorkflow modify_wf(VM_WF_CATEGORY, VM_WF_MODIFY_INDIVIDUAL_VMCFG);
        modify_wf._vid                  = req._vid;
        modify_wf.upstream_sender       = sender;
        modify_wf.upstream_action_id    = req.action_id;

        VmCfgModifyAction prepare_image_action(VM_ACT_CATEGORY,
                                      VM_ACT_DEPLOY_IMAGE,
                                      EV_IMAGE_PREPARE_REQ,
                                      MID(MU_VMCFG_MANAGER),
                                      MID(PROC_IMAGE_MANAGER,MU_IMAGE_MANAGER));        
        prepare_image_action.GenPrepareImageAction(image, GetLowerLayer(req._vid));
        if (req._modification._operation != INSERT_VMCFG)
        {
            prepare_image_action.skipped = true;
        }
        modify_wf.Add(prepare_image_action);

        VmCfgModifyIndividualItem modification = req._modification;

        int64 image_id;
        from_string<int64>(image_id, modification._args["id"], dec);
        ImageMetadata image_info;
        STATUS ret = GetImageMetadata(image_id, modification._uid, image_info);
        if (ret != SUCCESS)
        {
            return ret;
        }

        modification._args["disk_size"] = to_string<int64>(image_info._disk_size, dec);
        modification._args["url"] = image_info._url.serialize();
        modification._args["disk_format"] = image_info._disk_format;
        modification._args["os_arch"] = image_info._arch;

        VmCfgModifyAction modify_actions(modification);        
        GenSendtoLowerLayerAction(req._modification, modify_actions);

        modify_wf.Add(prepare_image_action, modify_actions);

        return CreateWorkflow(modify_wf);        
    }

    virtual STATUS Check(const VmCfgModifyIndividualItem &modification, ostringstream &err)
    {
        STATUS ret = ModifyVmCfgElementMethods::Check(modification, err);
        if (ret != SUCCESS)
        {
            return ret;
        }

        int64 vm_machine_id = GetVmMachineId(modification._vid);

        int64 modify_machine_id;
        map<string, string> args(modification._args);
        
        from_string<int64>(modify_machine_id, args["id"], dec);

        if (modification._operation == INSERT_VMCFG)
        {
            if (vm_machine_id != INVALID_FILEID)
            {
                err << "Vm(" << modification._vid << ") has machine, id = " << vm_machine_id << endl;
                return ERR_VMCFG_MODIFY_FORBIDDEN;
            }
        }
        else if (modification._operation == DELETE_VMCFG)
        {
            if (vm_machine_id != modify_machine_id)
            {
                err << "Vm(" << modification._vid << ") machine id = " << vm_machine_id << ", not " << modify_machine_id << endl;
                return ERR_VMCFG_MODIFY_FORBIDDEN;
            }
        }
        else
        {
            err << "Only INSERT_VMCFG and DELETE_VMCFG is allowed!" << endl;
            return ERR_VMCFG_MODIFY_FORBIDDEN;
        }

        return SUCCESS;
    }

    bool AnalysisArgsto(const VmCfgModifyIndividualItem &modification, VmBaseImage &base_image)
    {
        map<string, string> args(modification._args);

        base_image._id = modification._vid;
        base_image._target = args["target"];
        from_string<int32>(base_image._position, args["position"], dec);
        from_string<int64>(base_image._image_id, args["id"], dec);
        from_string<int32>(base_image._reserved_backup, args["reserved_backup"], dec);

        return true;
    }

    int64 GetVmMachineId(int64 vid)
    {
        VmBaseImagePool  *p_image = VmBaseImagePool::GetInstance();
        vector<VmBaseImageInfo>            image_info;
        STATUS rc = p_image->GetImageInfoByView(image_info, vid, true);
        if (SQLDB_OK != rc)
        {
            return INVALID_FILEID;
        }

        vector<VmBaseImageInfo>::iterator it;
        for (it = image_info.begin(); it != image_info.end(); it++)
        {
            if (DISK_TYPE_MACHINE == it->_type)
            {
                return it->_image_id;
            }
        }

        return INVALID_FILEID;
    }
};
ModifyVmCfgMachine modify_vm_machine;

class ModifyVmCfgMutex : public ModifyVmCfgElementMethods
{
public:
    ModifyVmCfgMutex() : ModifyVmCfgElementMethods(VM_CFG_MUTEX, "vmcfg_repel", "vid2", "vid1")
    {
    }

    ~ModifyVmCfgMutex(){};
    virtual STATUS EffectModification(const VmModifyReq &req, const MID &sender)
    {
        ApiVmCfgMap vm_mutex_map;
        AnalysisArgsto(req._modification, vm_mutex_map);
        
        if (IsVmNotDeployed(vm_mutex_map.src) && IsVmNotDeployed(vm_mutex_map.des))
        {
            return EffectThisLayer(req._modification);
        }

        STATUS ret = SendtoLowerLayerbyWorkflow(req, sender);
        if ( ret == SUCCESS)
        {
            ret = ERROR_ACTION_RUNNING;
        }

        return ret;
    }

    virtual STATUS RecordtoDatabase(const VmCfgModifyIndividualItem &modification)
    {
        ApiVmCfgMap vm_mutex_map;
        AnalysisArgsto(modification, vm_mutex_map);

        int64   vid_small = 0;
        int64   vid_big   = 0;

        if(vm_mutex_map.src < vm_mutex_map.des)
        {
            vid_small = vm_mutex_map.src;
            vid_big   = vm_mutex_map.des;
        }
        else
        {
            vid_small = vm_mutex_map.des;
            vid_big   = vm_mutex_map.src ;
        }

        VmCfgPool *vc_pool = VmCfgPool::GetInstance();
        STATUS ret;

        if (modification._operation == INSERT_VMCFG)
        {
            ret = vc_pool->Insert2Repel(vid_small, vid_big);
        }
        else if(modification._operation == DELETE_VMCFG)
        {
            ret = vc_pool->DropRepel(vid_small, vid_big);
        }
        else
        {
            SkyAssert(false);
        }
        
        if (ret == SQLDB_OK)
        {
            return SUCCESS;
        }
        else
        {
            OutPut(SYS_DEBUG, "Operate Database failed\n");
            return ERR_OBJECT_UPDATE_FAILED;
        }
    }

    virtual STATUS SendtoLowerLayerbyWorkflow(const VmModifyReq &req, const MID &sender)
    {
        VmCfgModifyWorkflow modify_wf(VM_WF_CATEGORY, VM_WF_MODIFY_INDIVIDUAL_VMCFG);
        modify_wf._vid                  = req._vid;
        modify_wf.upstream_sender       = sender;
        modify_wf.upstream_action_id    = req.action_id;

        ApiVmCfgMap vm_mutex_map;
        AnalysisArgsto(req._modification, vm_mutex_map);
        string src_cc = GetLowerLayer(vm_mutex_map.src);
        string des_cc = GetLowerLayer(vm_mutex_map.des);

        VmCfgModifyAction modify_src_action(req._modification);        
        GenSendtoLowerLayerAction(req._modification, modify_src_action);

        VmCfgModifyIndividualItem modification = req._modification;
        modification._vid = vm_mutex_map.des;
        VmCfgModifyAction modify_des_action(modification);        
        GenSendtoLowerLayerAction(modification, modify_des_action);

        if (src_cc.empty())
        {
            modify_wf.Add(modify_des_action);
        }
        else
        {
            modify_wf.Add(modify_src_action);

            if ((des_cc != src_cc) && !des_cc.empty())
            {
                modify_des_action.success_feedback = 0;
                modify_wf.Add(modify_des_action, modify_src_action);
            }
        }

        return CreateWorkflow(modify_wf);
    }

    virtual STATUS Check(const VmCfgModifyIndividualItem &modification, ostringstream &err)
    {
        STATUS ret = ModifyVmCfgElementMethods::Check(modification, err);
        if (ret != SUCCESS)
        {
            return ret;
        }

        VmCfgModifyIndividualItem modification1 = modification;        
        ApiVmCfgMap vm_mutex_map;
        AnalysisArgsto(modification, vm_mutex_map);
        modification1._vid = vm_mutex_map.des;
        ret = ModifyVmCfgElementMethods::Check(modification1, err);
        if (ret != SUCCESS)
        {
            return ret;
        }

        return SUCCESS;
    }

    bool AnalysisArgsto(const VmCfgModifyIndividualItem &modification, ApiVmCfgMap &vm_mutex)
    {
        map<string, string> args(modification._args);

        from_string<int64>(vm_mutex.src, args["src"], dec);
        from_string<int64>(vm_mutex.des, args["des"], dec);

        return true;
    }
};
ModifyVmCfgMutex modify_vmcfg_mutex;

void VmCfgManager::DealModify(const MessageFrame &message)
{
    VmModifyReq modify_req;
    modify_req.deserialize(message.data);

    ModifyVmCfgElementMethods *methods =  ModifyVmCfgElementMethods::GetElementMethod(VmCfgElement(modify_req._modification._element));
    if (methods != NULL)
    {
        MID sender = message.option.sender();
        methods->ImplandAck(modify_req, sender, _mu);
    }
}

int FuncResponseFeedback::ResponseWorkAck(const WorkAck &ack)
{
    SkyAssert(!ack.action_id.empty());

    if (IsActionFinished(ack.action_id))
    {
        return 0;
    }

    Action::ActionFeedbackTypes type;
    if (ack.state == SUCCESS)
    {
        type = Action::ACT_SUCCESS;
    }
    else if (ack.state == ERROR_ACTION_TIMEOUT)
    {
        type = Action::ACT_TIMEOUT;
    }
    else
    {
        type = Action::ACT_FAILURE;
    }

    int feedback_func;
    int ret = GetActionFeedback(ack.action_id, type, feedback_func);
    SkyAssert(ret == SUCCESS);

    return CommonResponseFeedback(feedback_func, ack);
}

int FuncResponseFeedback::CommonResponseFeedback(const int func, const WorkAck &ack)
{
    switch (func)
    {
        case FEEDBACK_RESTART_ACTION:
        {
            return RestartAction(ack.action_id);
        }
        case FEEDBACK_ROLLBACK_WORKFLOW:
        {
            return RollbackWorkflow(ack.action_id);
        }
        case FEEDBACK_DELETE_WORKFLOW:
        {
            return DeleteWorkflow(ack.action_id);
        }
        default:
        {
            return ResponseFeedback(func, ack);            
        }
    }

}

int VmCfgManager::ResponseFeedback(const int func, const WorkAck &ack)
{
    switch (func)
    {
        case FEEDBACK_MODIFY_FAILED:
        {
            AcktoWorkflowUpstreamSender(ack);
            return DeleteWorkflow(ack.action_id);
        }
        case FEEDBACK_MODIFY_SUCCESS:
        {
            EffectModificationThisLayer(ack);
            AcktoWorkflowUpstreamSender(ack);
            return FinishAction(ack.action_id);
        }

        default:
        {
            OutPut(SYS_NOTICE, "In VmCfgManager::ResponseFeed, unknown func code = %d.", func);
            return 0;            
        }
    }
}

STATUS VmCfgManager::AcktoWorkflowUpstreamSender(const WorkAck &ack)
{
    string upstream_action_id;
    STATUS ret = GetUpstreamActionId(ack.action_id, upstream_action_id);
    SkyAssert(ret == SUCCESS);

    VmOperationAck ack_to_upstream;
    ack_to_upstream = static_cast<const VmOperationAck&>(ack);
    ack_to_upstream.action_id = upstream_action_id;

    MID upstream_sender;
    ret = GetUpstreamSender(ack.action_id, upstream_sender);
    SkyAssert(ret == SUCCESS);
    MessageOption option(upstream_sender, EV_VM_OP_ACK, 0, 0);

    return _mu->Send(ack_to_upstream, option); 

    return 0;
}

STATUS VmCfgManager::EffectModificationThisLayer(const WorkAck &ack)
{
    string req_message;
    GetActionMessageReq(ack.action_id, req_message);
    if (req_message.empty())
    {
        return SUCCESS;
    }

    VmModifyReq modify_req;
    modify_req.deserialize(req_message);

    ModifyVmCfgElementMethods *methods =  ModifyVmCfgElementMethods::GetElementMethod(VmCfgElement(modify_req._modification._element));
    if (methods != NULL)
    {
        methods->EffectThisLayer(modify_req._modification);
    }

    return SUCCESS;
}

/******************************************************************************/
bool VmCfgManager::FindVmCfgWorkFlow(vector<string>& workflows,
                                              const string& workflow_name,
                                              int64    vid,
                                              bool     is_rollback)
{
    ostringstream where;
    where << " label_int64_1 = " << vid ;

    STATUS ret = FindWorkflow(workflows,
                                    VM_WF_CATEGORY,
                                    workflow_name,
                                    where.str(),
                                    is_rollback);//(有回滚流)
    if((SUCCESS == ret)
        &&(0 != workflows.size()))
    {
        return true;
    }

    return false;

}

/******************************************************************************/
void VmCfgManager::VmRollbackWorkflow(string action_id)
{
    STATUS ret = ERROR;

    if(IsSupportRollback(action_id))
    {
        //既然超时了，如果当初设置为能回滚的话，可以回滚一下
        OutPut(SYS_DEBUG, "VmCfg manager will rollback workflow of action: %s!\n", action_id.c_str());
        ret = RollbackWorkflow(action_id);
        if(SUCCESS != ret)
        {
            OutPut(SYS_DEBUG, "VmCfg manager failed to rollback workflow of action: %s!\n", action_id.c_str());
        }
        else
        {
            //调用了RollbackWorkflow之后就不需要再调用FinishAction了
            OutPut(SYS_DEBUG, "VmCfg manager rollback workflow of action: %s successfully!\n", action_id.c_str());
        }
    }
    else
    {
        OutPut(SYS_DEBUG, "Action %s is unrollbackable.!\n", action_id.c_str());
    }
}

/******************************************************************************/
void VmCfgManager::Ack2Api(MID receiver, VmOperationAck &ack)
{
    //无论成功or失败都直接给Method方法回应
    MessageOption option(receiver, EV_VMCFG_OPERATE_ACK, 0, 0);
    /*VmOperationAck tc_ack;

    tc_ack.action_id = ack.action_id;
    tc_ack._vid = ack._vid;
    tc_ack.state = ack.state;
    tc_ack.progress = ack.progress;
    tc_ack._operation = ack._operation;
    tc_ack.detail = ack.detail;
    tc_ack._vm_state = ack._vm_state;
    tc_ack._workflow_id = ack._workflow_id;*/

    _mu->Send(ack, option);

    return;
}

/******************************************************************************/
void VmCfgManager::Ack2Api(const string &action_id, VmOperationAck &ack)
{
    // 给Method方法回应
    MID  api_mid;
    GetUpstreamSender(action_id, api_mid);
    GetUpstreamActionId(action_id,ack.action_id);
    MessageOption option(api_mid, EV_VMCFG_OPERATE_ACK, 0, 0);    

    _mu->Send(ack, option);

    return;
}

/******************************************************************************/
bool VmCfgManager::Authorize(int64 opt_uid, VmCfg *buf, int oprate)
{
    /* 获取授权 */
    AuthRequest ar(opt_uid);
    int64 oid;
    int64 self_uid;

    if (buf == NULL)
    {
        oid = 0;
        self_uid = 0;
    }
    else
    {
        oid = buf->get_vid();
        self_uid = buf->get_uid();
        if (INVALID_UID == self_uid)
        {
           _vcpool->Drop(*buf);
           return false;
        }
    }


    ar.AddAuthorize(
                    AuthRequest::VMCONFIG,           //授权对象类型,用user做例子
                    oid,                             //对象的ID,oid
                    (AuthRequest::Operation)oprate,  //操作类型
                    self_uid,                        //对象拥有者的用户ID
                    false                            //此对象是否为公有
                    );

    if (-1 == UserPool::GetInstance()->Authorize(ar))
    {   /* 授权失败 */

        return false;
    }

    return true;

}




}

