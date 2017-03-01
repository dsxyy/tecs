#include "vm_interface.h"
#include "vstorage_manager.h"
#include "lifecycle_manager.h"
#include "vm_manager.h"
#include "vm_disk_share.h"
#include "log_agent.h"
#include "storage.h"
#include "config_cluster.h"
#include "alarm_agent.h"

static int vm_inter_print_on = 0;
DEFINE_DEBUG_VAR(vm_inter_print_on);
#define Debug(level,fmt,arg...) \
        do \
        { \
            if(vm_inter_print_on) \
            { \
                printf("[%s:%d]"fmt,__func__,__LINE__, ##arg); \
            } \
            OutPut(level,fmt,##arg);\
        }while(0)

namespace zte_tecs
{

/* vm_pool�Ĳ������Ƚϸ��ӣ���Ҫ��ȡ�������޸�vmֵ��
��һ����ȡ���ˣ�����ڲ�����ʱ��ܳ����п������������ȡ���ĵط������ˣ�
��ֻ���޸�vmֵ��ʱ���Ȼ�ȡ����Ȼ��update��֮�������ͷ� */

/*_____________________________________________________________________________
vm_interface�Ķ�λ���Ǹ�VMM��SCH��LCM�ṩ���ַ�װ�ӿڣ���������ݿ�Ĳ������Լ�
����һЩ�Ĳ�����ĳһ���������̵Ĳ���; ���漰���̵ĺ��������ڴ��ļ���
_____________________________________________________________________________*/


int VmInterface::InsertVmUsbInfo(const VmUsbOp &info)
{
    VirtualMachine *pvm = _vm_pool->Get(info._vid, true);
    SkyAssert(pvm);


    pvm->_config._usb.push_back(info._usb_info);

    TransactionOp tsop; //�����������

    _vm_pool->Update(pvm);
    _vm_pool->Put(pvm, true);

    STATUS ret = _vm_usb_pool->InsertVmUsbInfo(info._vid, info._usb_info);
    SkyAssert(ret == SQLDB_OK);

    ret = tsop.Commit();
    SkyAssert(ret == SUCCESS);

    return ret;
}

int VmInterface::DeleteVmUsbInfo(const VmUsbOp &info)
{
    VirtualMachine *pvm = _vm_pool->Get(info._vid, true);
    SkyAssert(pvm);

    vector<VmUsbInfo> &usbs = pvm->_config._usb;
    vector<VmUsbInfo>::iterator it;
    for (it = usbs.begin(); it != usbs.end(); it++)
    {
        if ((it->_vendor_id == info._usb_info._vendor_id)
            && (it->_product_id == info._usb_info._product_id))
        {
            usbs.erase(it);
            break;
        }
    }

    TransactionOp tsop; //�����������

    _vm_pool->Update(pvm);
    _vm_pool->Put(pvm, true);

    STATUS ret = _vm_usb_pool->DeleteVmUsbInfo(info._vid, info._usb_info);
    SkyAssert(ret == SQLDB_OK);

    ret = tsop.Commit();
    SkyAssert(ret == SUCCESS);

    return ret;
}

int VmInterface::SetValue(int64 vid, const string &key, int64 value)
{
    VirtualMachine *pvm = _vm_pool->Get(vid, true);
    if (!pvm)
    {
        SkyAssert(false);
        return ERR_VM_NOT_EXIST;
    }

    pvm->SetValue(key, value);

    /* �����Ҫͬʱ�޸Ķ��ֵ������������϶����ʺ��ˣ���Ҫ���ⶨ��һ��֧�ֿɱ���εĺ��� */
    _vm_pool->Update(pvm);
    _vm_pool->Put(pvm, true);

    return 0;
}

int VmInterface::SendOperateToHC(const VirtualMachine *pvm)
{
    SkyAssert(pvm);

    VmOperationReq req(pvm->_vid, pvm->_lastop, pvm->_lastopstamp);
    return SendOperateToHC(req, pvm->_hid);
}

int VmInterface::SendOperateToHC(int64 vid, int operation, int64 hid)
{
    VmOperationReq req(vid, operation);
    return SendOperateToHC(req, hid);
}

/* VM��һ��������ǽ����������HC */
int VmInterface::SendOperateToHC(const VmOperationReq &operation, int hid)
{
    string hostname = _host_pool->GetHostNameById(hid);
    SkyAssert(false == hostname.empty());

    MID receiver(hostname, PROC_HC, MU_VM_HANDLER);
    MessageOption option(receiver,EV_VM_OP,0,0);

    Debug(SYS_DEBUG,"Send %s on vm(%d)to hc(%d) \n", Enum2Str::VmOpStr(operation._operation), operation._vid, hid);

    return _boss->Send(operation.serialize(), option);
}

int VmInterface::Update(const VmInfo &vminfo)
{
    VirtualMachine *pvm = _vm_pool->Get(vminfo._vm_id, true);
    SkyAssert(pvm);

    GetTimeFromPowerOn(&(pvm->_lastmoni));

    pvm->_state     = vminfo._vm_state;
    pvm->_net_tx    = vminfo._vm_net_tx;
    pvm->_net_rx    = vminfo._vm_net_rx;
    pvm->_cpu_rate  = vminfo._vm_cpu_usage;
    pvm->_run_version = vminfo._run_version;

    if (vminfo._vm_mem_max == 0)
    {
        pvm->_memory_rate = 0;
    }
    else
    {
        if (vminfo._vm_mem_max - vminfo._vm_mem_free < 0)
        {
            //printf("invalid memory rate info! max=%lld,free=%lld.\n",vit->_vm_mem_max,vit->_vm_mem_free);
            pvm->_memory_rate = 0;
        }
        else
        {
            pvm->_memory_rate = (vminfo._vm_mem_max - vminfo._vm_mem_free)*100/(vminfo._vm_mem_max);
            if (pvm->_memory_rate > 100)
            {
                pvm->_memory_rate = 100;
            }
        }
    }

    _vm_pool->Update(pvm);
    _vm_pool->Put(pvm, true);

    return 0;
}

int VmInterface::LiveMigrateSuccess(const VmAction& action)
{
    //�޸����ݿ�hid���ֶ�
    int64 vid = action._vid;
    VirtualMachine *pvm = _vm_pool->Get(vid, true);
    SkyAssert(pvm);
    if (pvm->_hid_next != INVALID_HID)
    {//hid�ֶ�Ҫ������
        pvm->_hid = pvm->_hid_next;
    }
    pvm->_hid_next = INVALID_HID;
    _vm_pool->Update(pvm);
    _vm_pool->Put(pvm, true);

    return 0;
}

/*��Ϊ��ʱ����Ҳ��Ҫ���ô˽ӿڣ�����ʱ��Ϣ����û��vid�ģ����ԣ���Ҫ�����ߵ���������vid����Ϣ*/
int VmInterface::LiveMigrateFailed(VmAction& action)
{
    return SetValue(action._vid, "hid_next", INVALID_HID);
}

int VmInterface::UpdateVMLocalImage(int64 vid, const ImageUrlPrepareAck &ack)
{
    VirtualMachine *pvm = _vm_pool->Get(vid, true);
    SkyAssert(pvm);

    VmDiskConfig disk;

    if (pvm->_config._machine._request_id == ack._request_id)
    {
        pvm->_config._machine._url = ack._url;
        disk = pvm->_config._machine;
    }
    else if (pvm->_config._initrd._request_id == ack._request_id)
    {
        pvm->_config._initrd._url = ack._url;
        disk = pvm->_config._initrd;
    }
    else if (pvm->_config._kernel._request_id == ack._request_id)
    {
        pvm->_config._kernel._url = ack._url;
        disk = pvm->_config._kernel;
    }
    else
    {
        vector<VmDiskConfig>::iterator it;
        for (it = pvm->_config._disks.begin(); it != pvm->_config._disks.end(); it++)
        {
            if (it->_request_id == (ack)._request_id)
            {
                it->_url = ack._url;
                break;
            }
        }
        SkyAssert(it != pvm->_config._disks.end());
        disk = *it;
    }

    _vm_pool->Update(pvm);
    _vm_pool->Put(pvm, true);

    return SUCCESS;
}

int VmInterface::UpdateVMShareImage(const string &iscsi_name, int64 vid, const AuthBlockAck &ack,bool add)
{
    VirtualMachine *pvm = _vm_pool->Get(vid, true);
    SkyAssert(pvm);
    VmDiskConfig disk;
    if (pvm->_config._machine._request_id == ack._request_id)
    {
        AuthBlockAck2FileUrl(pvm->_config._machine._share_url,iscsi_name,ack,add);
        pvm->_config._machine._fstype = IMAGE_DEFAULT_FSTYPE;
        disk = pvm->_config._machine;
    }
    else if (pvm->_config._initrd._request_id == ack._request_id)
    {
        AuthBlockAck2FileUrl(pvm->_config._initrd._share_url,iscsi_name,ack,add);
        pvm->_config._initrd._fstype = IMAGE_DEFAULT_FSTYPE;
        disk = pvm->_config._initrd;
    }
    else if (pvm->_config._kernel._request_id == ack._request_id)
    {
        AuthBlockAck2FileUrl(pvm->_config._kernel._share_url,iscsi_name,ack,add);
        pvm->_config._kernel._fstype = IMAGE_DEFAULT_FSTYPE;
        disk = pvm->_config._kernel;
    }
    else
    {
        vector<VmDiskConfig>::iterator it;
        for (it = pvm->_config._disks.begin(); it != pvm->_config._disks.end(); it++)
        {
            if (it->_request_id == (ack)._request_id)
            {
                AuthBlockAck2FileUrl(it->_share_url,iscsi_name,ack,add);
                it->_fstype = IMAGE_DEFAULT_FSTYPE;
                break;
            }
        }
        SkyAssert(it != pvm->_config._disks.end());
        disk = *it;
    }

    TransactionOp tsop; //�����������

    _vm_pool->Update(pvm);
    _vm_pool->Put(pvm, true);

    _vm_image_pool->UpdateImage(vid, disk);

    return tsop.Commit();
}

int VmInterface::UpdateVMShareDisk(const string &iscsi_name,int64 vid, const AuthBlockAck &ack,bool add)
{
    VirtualMachine *pvm = _vm_pool->Get(vid, true);
    SkyAssert(pvm);
    vector<VmDiskConfig>::iterator it;
    for (it = pvm->_config._disks.begin(); it != pvm->_config._disks.end(); it++)
    {
        if (it->_request_id == (ack)._request_id)
        {
            AuthBlockAck2FileUrl(it->_share_url,iscsi_name,ack,add);
            break;
        }
    }

    SkyAssert(it != pvm->_config._disks.end());

    TransactionOp tsop; //�����������

    _vm_pool->Update(pvm);
    _vm_pool->Put(pvm, true);

    _vm_image_pool->UpdateDisk(vid, *it);

    return tsop.Commit();
}

int VmInterface::ConstructCreateReq(int64 vid, VmCreateReq &createreq)
{
    string request_id;
    string target;
    VirtualMachine *pvm = _vm_pool->Get(vid, false);
    SkyAssert(pvm);
    VMImagePool *vmimage_pool = VMImagePool::GetInstance();
    SkyAssert(vmimage_pool);

    createreq._config = pvm->_config;
    createreq._vid = pvm->_vid;

    if(SUCCESS==vmimage_pool->GetActiveImageBackup(request_id,pvm->_config._machine._target,vid)&&(!request_id.empty()))
    {
        createreq._config._machine._request_id=request_id;
    }
    else if(SUCCESS==vmimage_pool->GetActiveImageBackup(request_id,pvm->_config._kernel._target,vid)&&(!request_id.empty()))
    {
        createreq._config._machine._request_id=request_id;
    }
    else if(SUCCESS==vmimage_pool->GetActiveImageBackup(request_id,pvm->_config._initrd._target,vid)&&(!request_id.empty()))
    {
        createreq._config._machine._request_id=request_id;
    }
    else if(pvm->_config._disks.size())
    {
        vector<VmDiskConfig>::iterator it;
        for (it = pvm->_config._disks.begin(); it != pvm->_config._disks.end(); it++)
        {
            if(SUCCESS==vmimage_pool->GetActiveImageBackup(request_id,it->_target,vid)&&(!request_id.empty()))
            {
                createreq._config._machine._request_id=request_id;
                break;
            }
        }
    }

    GetVstorageOption(createreq._option._image_store_option);

    // ���TCU�������ñ�־
    if (IsTcuShare())
    {
        createreq._option._is_tcu_share = true;
    }

    return 0;
}

int VmInterface::ConstructRecoverReq(int64 vid, VmRecoverImageReq &recoverreq)
{
    VirtualMachine *pvm = _vm_pool->Get(vid, false);
    SkyAssert(pvm);

    recoverreq._config = pvm->_config;
    recoverreq._vid = pvm->_vid;


    // ���TCU�������ñ�־
    if (IsTcuShare())
    {
        recoverreq._option._is_tcu_share = true;
    }

    return 0;
}

int VmInterface::DeleteVMWorkflow(int64 vid)
{
    string wf_id_now;
    STATUS ret = _vm_pool->GetVmWorkFlowId(vid, wf_id_now);

    if (ret == SQLDB_OK)
    {
        return DeleteWorkflow(wf_id_now);
    }

    return SUCCESS;
}

int VmInterface::SetTimer(TIMER_ID &timer, uint32 cycle, uint32 msg_id, uint32 type)
{
    //������cycle = 0
    SkyAssert(cycle > 0);

    if (timer != INVALID_TIMER_ID)
    {
        _boss->StopTimer(timer);
    }
    else
    {
        timer = _boss->CreateTimer();
        if (timer == INVALID_TIMER_ID)
        {
            return ERROR;
        }
    }

    TimeOut to;
    to.duration = cycle;
    to.type = type;
    to.msgid = msg_id;
    return _boss->SetTimer(timer,to);
}

/* ���actionִ������������ص������Ƿ���Ҫ���������action : fase--��Ҫ����; true--���账��
��������ԭ������ڵ�һ���յ�REQ�����ʱ�򣬸������������ؽ��(DOING, 50)��
�������յ�REQ��ʱ���ȼ��ִ�������������ɣ�����DOING��������ִ���ˡ�
*/
bool VmInterface::CheckReqDoing(const WorkReq &req)
{
    if (IsActionFinished(req.action_id))
    {
        return true;
    }

    int state;
    int ret = GetActionState(req.action_id, state);
    if (ret != SUCCESS)
    {//��ѯ���ݿⲻ�ɹ���Ҫô���ݿ��ǿգ�Ҫô�����ݿ�ʧ�ܣ���2������£�action������ִ����
        return true;
    }

    if (state != ERROR_ACTION_READY)
    {//�Ѿ���running,˵��ǰ���Ѿ��յ������action��req�������Ѿ���ʼִ����
        return true;
    }

    //������һ����˵���ǵ�һ���յ���req���Ǿ��ȸ�reqһ��Ӧ��
    WorkAck ack(req.action_id, ERROR_ACTION_RUNNING, 50, "Received a req, begin to running.");
    UpdateActionAck(req.action_id, ack.serialize());

    return false;
}

int SendMsgToHC(MessageUnit *mu, const string &msg, int hid, int msg_id,
                const string& process, const string& unit)
{
    string hostname;
    ostringstream error;

    if (INVALID_HID == hid)
    {
        SkyAssert(false);
        return ERROR;
    }
    else
    {
        HostPool *host_pool = HostPool::GetInstance();
        SkyAssert(host_pool);
        hostname = host_pool->GetHostNameById(hid);
        if (hostname.empty())
        {
            SkyAssert(false);
            error << "SendOperateToHC failed to get hostname of " << hid;
            return ERROR;
        }
    }

    MID receiver(hostname,process,unit);
    MessageOption option(receiver,msg_id,0,0);

    return mu->Send(msg, option);
}

int GetVMOpWFName(int op, string &wf_name, string &action_name)
{
#define CaseOp(op) \
    case op: \
    { \
        wf_name = VM_WF_##op; \
        action_name = VM_ACT_##op; \
        break; \
    }

    switch (op)
    {
        CaseOp(CREATE)
        CaseOp(START)
        CaseOp(STOP)
        CaseOp(REBOOT)
        CaseOp(PAUSE)
        CaseOp(RESUME)
        CaseOp(RESET)
        CaseOp(DESTROY)
        CaseOp(UPLOAD_IMG)
        CaseOp(CANCEL_UPLOAD_IMG)
        CaseOp(PLUG_IN_USB)
        CaseOp(PLUG_OUT_USB)
        CaseOp(OUT_IN_USB)
        CaseOp(RECOVER_IMAGE)
        CaseOp(COLD_MIGRATE)
        CaseOp(FREEZE)
        CaseOp(UNFREEZE)
        CaseOp(MODIFY_VMCFG)
        CaseOp(CANCEL)

        default:
        {
            SkyAssert(false);
        }
    }

    return 0;
}

//����action�Ľ����Ӧ��workflow�Ľ��
//һ���ǹ�������������TC�ϱ���ʱ�����
int SendVmOpAck(MessageUnit *mu, const VmAction &action)
{
    const string action_id = action.get_id();
    STATUS ret;

    VmOperationAck ack_tc;
    SkyAssert(true == ack_tc.deserialize(action.message_ack));

    VmWorkFlow wf;
    GetWorkflowLabels(action_id, wf.labels);
    ack_tc._operation = wf._operation;

    //����ǻع������ǻع��ɹ�����������ʧ��
    if (IsRollbackWorkflow(action_id))
    {
        if (wf._operation == CANCEL)
        {
            ack_tc._operation   = DEPLOY;
        }

        WorkflowHistory history;
        ret = GetForwardWorkflowHistory(action_id, history);
        SkyAssert(ret == SUCCESS);

        ack_tc.state        = history.state;
        ack_tc.detail       = history.detail;
        ack_tc.progress     = GetWorkflowProgress(action_id);
    }

    if (ack_tc._operation == START)
    {
        int flag;
        flag = (ack_tc.state == SUCCESS) ? OAM_REPORT_RESTORE : OAM_REPORT_ALARM;
        
        VirtualMachinePool *vm_pool = VirtualMachinePool::GetInstance();
        SkyAssert(vm_pool);

        SendVmAlarm(*(vm_pool->Get(action._vid, false)), ALM_TECS_VM_START_FAILED, flag);
    }

    string tc_action_id;
    ret = GetUpstreamActionId(action.get_id(), tc_action_id);
    SkyAssert(ret == SUCCESS);
    ack_tc.action_id = tc_action_id;

    MID tc;
    ret = GetUpstreamSender(action.get_id(), tc);
    SkyAssert(ret == SUCCESS);
    MessageOption option(tc,EV_VM_OP_ACK,0,0);

    return mu->Send(ack_tc, option);
}

int AcktoUpstream(MessageUnit *mu, const WorkAck &ack)
{
    const string action_id = ack.action_id;
    STATUS ret;

    VmOperationAck ack_tc;
    ack_tc.state        = ack.state;
    ack_tc.detail       = ack.detail;
    ack_tc.progress     = ack.progress;
    GetWorkflowByActionId(action_id, ack_tc._workflow_id);

    VmWorkFlow wf;
    GetWorkflowLabels(action_id, wf.labels);
    ack_tc._vid         = wf._vid;

    //����ǻع������ǻع��ɹ�����������ʧ��
    if (IsRollbackWorkflow(action_id))
    {
        if (wf._operation == CANCEL)
        {
            ack_tc._operation   = DEPLOY;
        }

        WorkflowHistory history;
        ret = GetForwardWorkflowHistory(action_id, history);
        SkyAssert(ret == SUCCESS);

        ack_tc.state        = history.state;
        ack_tc.detail       = history.detail;
        ack_tc.progress     = GetWorkflowProgress(action_id);
    }

    string tc_action_id;
    ret = GetUpstreamActionId(action_id, tc_action_id);
    SkyAssert(ret == SUCCESS);
    ack_tc.action_id = tc_action_id;

    MID tc;
    ret = GetUpstreamSender(action_id, tc);
    SkyAssert(ret == SUCCESS);
    MessageOption option(tc,EV_VM_OP_ACK,0,0);

    return mu->Send(ack_tc, option);
}

void AuthBlockAck2FileUrl(FileUrl& url,const string &iscsi_name,const AuthBlockAck& ack,bool add)
{
    iSCSIAccessOption option;

    option.deserialize(url.access_option);
    if(!ack._type.empty())
    {
        option._type = ack._type;
        option._dataip_array = ack._dataip_vec;
    }
    url.access_type = IMAGE_ACCESS_ISCSI;
    LunAddr  addr(ack._target_name,ack._lunid);
    if(add)
    {
        option._auth_tbl.AddAuth(iscsi_name, addr);
    }
    else
    {
        option._auth_tbl.DelAuth(iscsi_name);
    }
    url.access_option = option.serialize();
}

STATUS UpdateActionReq(const string &wf_id, const string &action_name, WorkReq &req)
{
    vector<string> actions;
    STATUS ret = GetActionsByName(wf_id, action_name, actions);
    SkyAssert(ret == SUCCESS);

    vector<string>::iterator it;
    for (it = actions.begin(); it != actions.end(); it++)
    {
        req.action_id = *it;
        ret = UpdateActionReq(*it, req);
        SkyAssert(ret == SUCCESS);
    }

    return 0;
}

STATUS UpdateActionReceiver(const string &wf_id, const string &action_name, const MID &receiver)
{
    vector<string> actions;
    STATUS ret = GetActionsByName(wf_id, action_name, actions);
    SkyAssert(ret == SUCCESS);

    vector<string>::iterator it;
    for (it = actions.begin(); it != actions.end(); it++)
    {
        ret = UpdateActionReceiver(*it, receiver);
        SkyAssert(ret == SUCCESS);
    }

    return 0;
}

bool IsNeedVNet(const VMConfig &config)
{
    if (config._nics.empty() && (config._vm_wdtime == 0))
    {
        return false;
    }

    return true;
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

void SendVmAlarm(const VirtualMachine &vm, uint32 alarm_code, int flag)
{
    ConfigCluster *p = ConfigCluster::GetInstance();
    SkyAssert(p);

    HostPool *host_pool = HostPool::GetInstance();
    SkyAssert(host_pool);

    // ����澯λ����Ϣ
    AlarmLocation location;
    location.strSubSystem   = TECS_ALARM_SYS_COMPUTING;
    location.strLevel1      = p->Get(CFG_TECS_NAME);
    location.strLevel2      = ApplicationName();
    location.strLevel3      = host_pool->GetHostNameById(vm._hid);
    location.strLocation    = host_pool->GetHostLocationById(vm._hid);

    // ���͸澯��澯�ָ���֪ͨ
    AlarmReport ar;
    ar.dwAlarmCode = alarm_code;
    ar.ucAlarmFlag = flag;
    ar.alarm_location = location;
    ar.aucMaxAddInfo = AlarmVm(vm._vid, vm._hid).serialize();
    AlarmAgent *aa = AlarmAgent::GetInstance();
    SkyAssert(NULL != aa);
    aa->Send(ar);
}

}

