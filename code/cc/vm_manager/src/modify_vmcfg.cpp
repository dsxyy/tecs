#include "modify_vmcfg.h"
#include "vstorage_manager.h"
#include "volume.h"
#include "vm_manager.h"

namespace zte_tecs
{

const MID LCM(MU_VM_LIFECYCLE_MANAGER);
const MID SCH(MU_VM_SCHEDULER);
#define IA MID(ApplicationName(), PROC_IMAGE_AGENT, MU_IMAGE_AGENT)
#define SC GetSC_MID()

VmWorkFlow::VmWorkFlow(const VmOpHandle &vmop) : _vid(labels.label_int64_1), _operation(labels.label_int64_2), _hid(labels.label_int64_3)
{
    category = VM_WF_CATEGORY;
    _vid = vmop._req._vid;
    _operation = vmop._req._operation;
    string action_name;
    GetVMOpWFName(vmop._req._operation, name, action_name);
    upstream_sender = vmop._upstream_sender;
    upstream_action_id = vmop._req.action_id;
    if(!vmop._req._user.empty())
    {
        originator = vmop._req._user;
    }
}

VmAction::VmAction(int64 vid, int64 hid, VmOperation op) : INITVMACTION
{
    HostPool *host_pool = HostPool::GetInstance();
    string hostname = host_pool->GetHostNameById(hid);
    SkyAssert(false == hostname.empty());
    MID HC(hostname, PROC_HC, MU_VM_HANDLER);

    string wf_name, ac_name;
    GetVMOpWFName(op, wf_name, ac_name);

    new (this)VmAction(VM_WF_CATEGORY, ac_name, EV_VM_OP, LCM, HC);

    VmOperationReq req(vid, op);
    SetMessageReq(req);

    _vid = vid;
    _hid = hid;
}

template<typename T>
class VmOpScReq : public T
{
public:
    VmOpScReq(const VmDiskConfig &disk) : T()
    {
        T::_request_id  = disk._request_id;
    }

    VmOpScReq(int64 vid, int64 hid, const VmDiskConfig &disk)
    {
        T::_vid             = vid;
        T::_dist_id         = -1;
        T::_request_id      = disk._request_id;
        T::_cluster_name    = ApplicationName();
        T::_usage           = to_string<int64>(vid,dec)+":"+disk._target;

        if (hid != INVALID_HID)
        {
            HostPool *host_pool = HostPool::GetInstance();
            SkyAssert(host_pool);
            T::_userip = host_pool->GetHostIpById(hid);
            T::_iscsiname = host_pool->GetIscsinameById(hid);
        }
    }
};

template<>
class VmOpScReq<ImageUrlPrepareReq> : public ImageUrlPrepareReq
{
public:
    VmOpScReq(const VmDiskConfig &disk)
    {
        _image_id = disk._id;
        _position = disk._position;
        _request_id = disk._request_id;
        _cluster_name = ApplicationName();
        _url = disk._url;
        GetVstorageOption(_option);
    }
};

template<typename T>
class VmOpScAction : public VmAction
{
public:
    VmOpScAction(int64 vid, const VmDiskConfig &disk, bool retry = false, const MID &sender = LCM, const MID &receiver = SC) 
                    : VmAction(STORAGE_CATEGORY, action_name, ev_msg_id, sender, receiver)
    {
        VmOpScReq<T> req(disk);
        req.retry = retry;
        SetMessageReq(req);

        _vid = vid;
    }

    VmOpScAction(int64 vid, int64 hid, const VmDiskConfig &disk, bool retry = false) : VmAction(STORAGE_CATEGORY,action_name,ev_msg_id,LCM,SC)
    {
        VmOpScReq<T> req(vid, hid, disk);
        req.retry = retry;
        SetMessageReq(req);

        if (hid == INVALID_HID)
        {
            skipped = true;
        }

        _vid = vid;
    }

    static const int    ev_msg_id;
    static const string action_name;
};

template<> const int VmOpScAction<DeAuthBlockReq>::ev_msg_id = EV_STORAGE_DEAUTH_BLOCK_REQ;
template<> const string VmOpScAction<DeAuthBlockReq>::action_name = VM_ACT_DEAUTH_BLOCK;

template<> const int VmOpScAction<DeleteSnapshotReq>::ev_msg_id = EV_STORAGE_DELETE_SNAPSHOT_REQ;
template<> const string VmOpScAction<DeleteSnapshotReq>::action_name = DELETE_SNAPSHOT_BLOCK;

template<> const int VmOpScAction<DeleteBlockReq>::ev_msg_id = EV_STORAGE_DELETE_BLOCK_REQ;
template<> const string VmOpScAction<DeleteBlockReq>::action_name = DELETE_BLOCK;

template<> const int VmOpScAction<ImageUrlPrepareReq>::ev_msg_id = EV_IMAGE_URL_PREPARE_REQ;
template<> const string VmOpScAction<ImageUrlPrepareReq>::action_name = VM_ACT_PREPARE_IMAGE;

template<> const int VmOpScAction<CreateSnapshotReq>::ev_msg_id = EV_STORAGE_CREATE_SNAPSHOT_REQ;
template<> const string VmOpScAction<CreateSnapshotReq>::action_name = CREATE_SNAPSHOT_BLOCK;

template<> const int VmOpScAction<CreateBlockReq>::ev_msg_id = EV_STORAGE_CREATE_BLOCK_REQ;
template<> const string VmOpScAction<CreateBlockReq>::action_name = CREATE_BLOCK;

template<> const int VmOpScAction<AuthBlockReq>::ev_msg_id = EV_STORAGE_AUTH_BLOCK_REQ;
template<> const string VmOpScAction<AuthBlockReq>::action_name = VM_ACT_AUTH_BLOCK;

extern MsgFrmHandlCreaterBoundle<int> vmm_msgfrmhandl_creaters;

static MsgFrmHandlCreaterBoundle<VmOperation, VmOpHandle> vmop_creaters;
RegisterBaseCreater(vmm_msgfrmhandl_creaters, EV_VM_OP, vmop_creaters);

static MsgFrmHandlCreaterBoundle<VmCfgElement, ModifyVmcfg> vmmodify_creaters;
RegisterBaseCreater(vmop_creaters, MODIFY_VMCFG, vmmodify_creaters);

class ModifyVmCPU : public ModifyVmcfg
{
public:
    ModifyVmCPU(const MessageFrame &message, MessageUnit *receiver) :
            ModifyVmcfg(message, receiver)
    {
    }
    virtual ~ModifyVmCPU(){}

    virtual STATUS AnalysisCommonModifyStruct()
    {
        from_string<uint32>(_new_tcu, _modification._args["tcu"], dec);
        from_string<uint32>(_new_vcpu, _modification._args["vcpu"], dec);

        return SUCCESS;
    }

    virtual bool IsVmcfgChanged()
    {
        VirtualMachine *pvm = Get(false);
        SkyAssert(pvm);

        if ((pvm->_vcpu != _new_vcpu) || (pvm->_tcu!= _new_tcu))
        {
            return true;
        }

        return false;
    }

    virtual STATUS CheckHostSupport()
    {
        VirtualMachine *pvm = Get(false);
        SkyAssert(pvm);
        SkyAssert(pvm->_hid != INVALID_HID);

        ostringstream where;
        where << " cpus >= " << _new_vcpu;
        where << " AND tcu_num >= " << _new_tcu;
        where << " AND all_free_tcu >= " << int32(_new_tcu*_new_vcpu) - int32(pvm->_config._vcpu*pvm->_config._tcu);
        where << " AND oid = " << pvm->_hid;

        if (IsHostSupportModification(where))
        {
            return SUCCESS;
        }
        else
        {
            _op_info << "Host can not support modify CPU" << endl;
            return ERROR_NOT_SUPPORT;
        }
    }

    virtual STATUS EffectModification()
    {
        SetValue(_modification._vid, "vcpu", _new_vcpu);
        return SetValue(_modification._vid, "tcu", _new_tcu);
    }

private:
    uint32 _new_vcpu;
    uint32 _new_tcu;
};
RegisterDerivedCreater(vmmodify_creaters, VM_CFG_CPU, ModifyVmCPU);

class ModifyVmMemory : public ModifyVmcfg
{
public:
    ModifyVmMemory(const MessageFrame &message, MessageUnit *receiver) :
            ModifyVmcfg(message, receiver)
    {
    }
    virtual ~ModifyVmMemory(){}

    virtual STATUS AnalysisCommonModifyStruct()
    {
        from_string<int64>(_new_memory, _modification._args.begin()->second, dec);

        return SUCCESS;
    }

    virtual bool IsVmcfgChanged()
    {
        VirtualMachine *pvm = Get(false);
        SkyAssert(pvm);

        if (pvm->_memory != _new_memory)
        {
            return true;
        }

        return false;
    }

    virtual STATUS CheckHostSupport()
    {
        VirtualMachine *pvm = Get(false);
        SkyAssert(pvm);
        SkyAssert(pvm->_hid != INVALID_HID);

        ostringstream where;
        where << " mem_free >= " << _new_memory - pvm->_config._memory;
        where << " AND oid = " << pvm->_hid;

        if (IsHostSupportModification(where))
        {
            return SUCCESS;
        }
        else
        {
            _op_info << "Host can not support modify Memeory" << endl;
            return ERROR_NOT_SUPPORT;
        }
    }

    virtual STATUS EffectModification()
    {
        return SetValue(_modification._vid, "memory", _new_memory);
    }

private:
    int64 _new_memory;
};
RegisterDerivedCreater(vmmodify_creaters, VM_CFG_MEMORY, ModifyVmMemory);

class ModifyVmMachine : public ModifyVmcfg
{
public:
    ModifyVmMachine(const MessageFrame &message, MessageUnit *receiver) :
            ModifyVmcfg(message, receiver)
    {
    }
    virtual ~ModifyVmMachine(){}

    virtual STATUS AnalysisCommonModifyStruct()
    {
        _machine._bus           = _modification._args["bus"];
        _machine._type          = _modification._args["type"];
        _machine._target        = _modification._args["target"];
        _machine._os_arch       = _modification._args["os_arch"];
        _machine._disk_format   = _modification._args["disk_format"];

        from_string<int64>(_machine._id, _modification._args["id"], dec);
        from_string<int64>(_machine._size, _modification._args["size"], dec);
        from_string<int>(_machine._position, _modification._args["position"], dec);
        from_string<int64>(_machine._disk_size, _modification._args["disk_size"], dec);
        from_string<int32>(_machine._reserved_backup, _modification._args["reserved_backup"], dec);

        SkyAssert(_machine._url.deserialize(_modification._args["url"]));

        return SUCCESS;
    }

    virtual bool IsVmcfgChanged()
    {
        VirtualMachine *pvm = Get(false);
        SkyAssert(pvm);

        if ((_modification._operation == INSERT_VMCFG) && (pvm->_config._machine._id != _machine._id))
        {
            return true;
        }

        if ((_modification._operation == DELETE_VMCFG) && (pvm->_config._machine._id == _machine._id))
        {
            return true;
        }

        return false;
    }

    virtual STATUS CheckHostSupport()
    {
        VirtualMachine *pvm = Get(false);
        SkyAssert(pvm);

        if (_modification._operation == UPDATE_VMCFG)
        {
            _op_info << "Only INSERT_VMCFG and DELETE_VMCFG is allowed!" << endl;
            return ERR_VMCFG_MODIFY_FORBIDDEN;
        }
        else if (_modification._operation == DELETE_VMCFG)
        {//todo
            VMImagePool *vmimage_pool = VMImagePool::GetInstance();
            SkyAssert(vmimage_pool);
            bool ret = vmimage_pool->IsImageBackupExist(pvm->_config._machine._request_id);

            if (ret)
            {
                _op_info << "Vm machine id is in vm_image_backup, Maybe u need to delete this backup first!" << endl;
                return ERR_VMCFG_MODIFY_FORBIDDEN;
            }

            return SUCCESS;
        }
        else
        {
            if (_machine._position == VM_STORAGE_TO_SHARE)
            {
                return SUCCESS;
            }
        }

        SkyAssert(pvm->_hid != INVALID_HID);

        ostringstream where;
        where << " disk_free >= " << GetVmMachineNeedSizeInHost(_machine, pvm->_hid);
        where << " AND oid = " << pvm->_hid;

        if (IsHostSupportModification(where))
        {
            return SUCCESS;
        }
        else
        {
            _op_info << "Host can not support modify machine" << endl;
            return ERROR_NOT_SUPPORT;
        }
    }

    virtual STATUS EffectModification()
    {
        if (_modification._operation == DELETE_VMCFG)
        {
            return DeleteMachine();
        }
        else if (_modification._operation == INSERT_VMCFG)
        {
            return InsertMachine();
        }
        else 
        {
            _op_info << "Only INSERT_VMCFG and DELETE_VMCFG is allowed!" << endl;
            return ERR_VMCFG_MODIFY_FORBIDDEN;
        }
    }

    STATUS DeleteMachine()
    {
        VirtualMachine *pvm = Get(false);
        SkyAssert(pvm);

        STATUS ret;        
        if (pvm->_config._machine._position == VM_STORAGE_TO_SHARE)
        {
            ret = DeleteShareMachinebyWorkflow();
            if (ret == SUCCESS)
            {
                ret = ERROR_ACTION_RUNNING;
            }
        }
        else
        {
            ret = DeleteLocalMachine();
        }

        return ret;
    }

    STATUS InsertMachine()
    {
        _machine._request_id = GenerateUUID();

        STATUS ret;        
        if (_machine._position == VM_STORAGE_TO_SHARE)
        {
            ret = InsertShareMachinebyWorkflow();
        }
        else
        {
            ret = InsertLocalMachinebyWorkflow();
        }

        if (ret == SUCCESS)
        {
            ret = ERROR_ACTION_RUNNING;
        }

        return ret;
    }

    STATUS DeleteShareMachinebyWorkflow()
    {        
        VirtualMachine *pvm = Get(false);
        SkyAssert(pvm);

        VmWorkFlow wf(*this);

        VmOpScAction<DeAuthBlockReq> deauth_block_action(pvm->_vid, pvm->_hid, pvm->_config._machine);
        deauth_block_action.SetFeedback(0, FEEDBACK_ACKVMOP_AND_DELETEWORKFLOW, 0);

        VmOpScAction<DeleteSnapshotReq> delete_snapshot_action(pvm->_vid, pvm->_config._machine);
        delete_snapshot_action.SetFeedback(FEEDBACK_DELETE_MACHINE, FEEDBACK_ACKVMOP_AND_DELETEWORKFLOW, 0);
        delete_snapshot_action.skipped = !IsUseSnapshot();

        VmOpScAction<DeleteBlockReq> delete_block_action(pvm->_vid, pvm->_config._machine);
        delete_block_action.SetFeedback(FEEDBACK_DELETE_MACHINE, FEEDBACK_ACKVMOP_AND_DELETEWORKFLOW, 0);
        delete_block_action.skipped = IsUseSnapshot();

        wf.Add(deauth_block_action, delete_snapshot_action);
        wf.Add(deauth_block_action, delete_block_action);

        STATUS ret = CreateWorkflow(wf);
        if(SUCCESS != ret)
        {
            OutPut(SYS_WARNING, "DeleteShareMachinebyWorkflow failed to created workflow: %s!!!\n", wf.get_id().c_str());
            _op_info << "DeleteShareMachinebyWorkflow failed to created workflow: " << wf.get_id() << endl;
            SkyAssert(false);
        }

        return ret;
    }

    STATUS DeleteLocalMachine()
    {
        VirtualMachine *pvm = Get(false);
        SkyAssert(pvm);

        STATUS ret = DeleteVmMachineinDatabase(_modification._vid, _op_info);

        if (CouldUnbindVmfromHost())
        {
            ret = UnbindVmfromHostbyWorkflow(pvm->_config, pvm->_hid);

            if (ret == SUCCESS)
            {
                ret = ERROR_ACTION_RUNNING;
            }
        }

        return ret;
    }

    STATUS InsertShareMachinebyWorkflow()
    {        
        VirtualMachine *pvm = Get(false);
        SkyAssert(pvm);

        VmWorkFlow rollback_wf(*this);

        VmOpScAction<DeAuthBlockReq> deauth_block_action(pvm->_vid, pvm->_hid, _machine, true);
        deauth_block_action.SetFeedback(0, 0, 0);

        VmOpScAction<DeleteSnapshotReq> delete_snapshot_action(pvm->_vid, _machine, true);
        delete_snapshot_action.SetFeedback(FEEDBACK_ACKVMOP_AND_FINISHACTION, 0, 0);
        delete_snapshot_action.skipped = !IsUseSnapshot();

        VmOpScAction<DeleteBlockReq> delete_block_action(pvm->_vid, _machine, true);
        delete_block_action.SetFeedback(FEEDBACK_ACKVMOP_AND_FINISHACTION, 0, 0);
        delete_block_action.skipped = IsUseSnapshot();

        rollback_wf.Add(deauth_block_action, delete_snapshot_action);
        rollback_wf.Add(deauth_block_action, delete_block_action);

        VmWorkFlow wf(*this);

        VmOpScAction<ImageUrlPrepareReq> prepare_image_action(pvm->_vid, _machine, false, LCM, IA);
        prepare_image_action.labels.label_string_1 = _machine.serialize();

        VmOpScAction<CreateSnapshotReq> create_snapshot_action(pvm->_vid, _machine);
        create_snapshot_action.rollback_action_id = delete_snapshot_action.get_id();

        VmOpScAction<CreateBlockReq> create_block_action(pvm->_vid, _machine);
        create_block_action.rollback_action_id = delete_block_action.get_id();

        if (pvm->_hid == INVALID_HID)
        {
            create_snapshot_action.SetFeedback(FEEDBACK_INSERT_MACHINE, FEEDBACK_ROLLBACK_WORKFLOW, 0);
            create_block_action.SetFeedback(FEEDBACK_INSERT_MACHINE, FEEDBACK_ROLLBACK_WORKFLOW, 0);
        }
        else
        {
            create_snapshot_action.SetFeedback(0, FEEDBACK_ROLLBACK_WORKFLOW, 0);
            create_block_action.SetFeedback(0, FEEDBACK_ROLLBACK_WORKFLOW, 0);
        }

        VmOpScAction<AuthBlockReq> auth_block_action(pvm->_vid, pvm->_hid, _machine);
        auth_block_action.SetFeedback(FEEDBACK_INSERT_MACHINE, FEEDBACK_ROLLBACK_WORKFLOW, 0);
        auth_block_action.rollback_action_id = deauth_block_action.get_id();

        if (IsUseSnapshot())
        {
            prepare_image_action.SetFeedback(FEEDBACK_MODIFY_CREATESNAPSHOT, FEEDBACK_ACKVMOP_AND_DELETEWORKFLOW, 0);

            wf.Add(prepare_image_action, create_snapshot_action);
            wf.Add(create_snapshot_action, auth_block_action);
        }
        else
        {
            prepare_image_action.SetFeedback(0, FEEDBACK_ACKVMOP_AND_DELETEWORKFLOW, 0);

            wf.Add(prepare_image_action, create_block_action);
            wf.Add(create_block_action, auth_block_action);
        }

        STATUS ret = CreateWorkflow(wf, rollback_wf);
        if(SUCCESS != ret)
        {
            OutPut(SYS_WARNING, "InsertShareMachinebyWorkflow failed to created workflow: %s!!!\n", wf.get_id().c_str());
            _op_info << "InsertShareMachinebyWorkflow failed to created workflow: " << wf.get_id() << endl;
            SkyAssert(false);
        }
        
        return ret;
    }

    STATUS InsertLocalMachinebyWorkflow()
    {
        VmWorkFlow wf(*this);

        VmOpScAction<ImageUrlPrepareReq> prepare_image_action(_modification._vid, _machine, false, LCM, IA);
        prepare_image_action.SetFeedback(FEEDBACK_INSERT_MACHINE, FEEDBACK_ACKVMOP_AND_DELETEWORKFLOW, 0);
        prepare_image_action.labels.label_string_1 = _machine.serialize();

        wf.Add(prepare_image_action);

        STATUS ret = CreateWorkflow(wf);
        if(SUCCESS != ret)
        {
            OutPut(SYS_WARNING, "InsertLocalMachinebyWorkflow failed to created workflow: %s!!!\n", wf.get_id().c_str());
            _op_info << "InsertLocalMachinebyWorkflow failed to created workflow: " << wf.get_id() << endl;
            SkyAssert(false);
        }

        return ret;
    }

    static STATUS DeleteVmMachineinDatabase(int64 vid, ostringstream &err)
    {
        VirtualMachinePool *vm_pool = VirtualMachinePool::GetInstance();
        SkyAssert(vm_pool);
        
        VirtualMachine *pvm = vm_pool->Get(vid, true);
        SkyAssert(pvm);
        
        TransactionOp tsop; //启用事务操作
        
        VMImagePool *vmimage_pool = VMImagePool::GetInstance();
        SkyAssert(vmimage_pool);
        int ret = vmimage_pool->DeleteVmImage(pvm->_vid, pvm->_config._machine);
        SkyAssert(ret == 0);

        VmDiskConfig invalid_disk;
        pvm->_config._machine = invalid_disk;        

        vm_pool->Update(pvm);

        vm_pool->Put(pvm,true);
        
        ret = tsop.Commit();
        if (SUCCESS != ret)
        {
            SkyAssert(false);
            err << "tsop.Commit failed, ret = " << ret << endl;
            ret = ERROR_DB_UPDATE_FAIL;
        }

        return ret;
    }

    static STATUS InsertVmMachineinDatabase(int64 vid, const string &action_id, ostringstream &err)
    {
        ActionLabels labels;
        STATUS ret = GetActionLabels(action_id, VM_ACT_PREPARE_IMAGE, labels);
        SkyAssert(ret == SUCCESS);
        VmDiskConfig image;
        SkyAssert(image.deserialize(labels.label_string_1));

        string prepare_image_ack_msg;
        ret = GetActionMessageAck(action_id, VM_ACT_PREPARE_IMAGE, prepare_image_ack_msg);
        SkyAssert(ret == SUCCESS);
        ImageUrlPrepareAck prepare_ack;
        SkyAssert(prepare_ack.deserialize(prepare_image_ack_msg));

        SkyAssert(image._request_id == prepare_ack._request_id);

        image._url = prepare_ack._url;

        vector<string> actions;
        ret = GetActionsByName(action_id, VM_ACT_AUTH_BLOCK, actions);
        if ((ret == SUCCESS) && (!actions.empty()))
        {
            if (!IsActionSkipped(actions[0]))
            {
                string auth_req_msg;
                ret = GetActionMessageReq(action_id, VM_ACT_AUTH_BLOCK, auth_req_msg);
                AuthBlockReq auth_req;
                SkyAssert(auth_req.deserialize(auth_req_msg));

                string auth_ack_msg;
                ret = GetActionMessageAck(action_id, VM_ACT_AUTH_BLOCK, auth_ack_msg);
                AuthBlockAck auth_ack;
                SkyAssert(auth_ack.deserialize(auth_ack_msg));

                SkyAssert(ret == SUCCESS);
                
                AuthBlockAck2FileUrl(image._share_url, auth_req._iscsiname, auth_ack, true);
                image._fstype = IMAGE_DEFAULT_FSTYPE;
            }
        }

        VirtualMachinePool *vm_pool = VirtualMachinePool::GetInstance();
        SkyAssert(vm_pool);

        VirtualMachine *pvm = vm_pool->Get(vid, true);
        SkyAssert(pvm);

        pvm->_config._machine = image;

        TransactionOp tsop; //启用事务操作
        
        vm_pool->Update(pvm);
        
        VMImagePool *vmimage_pool = VMImagePool::GetInstance();
        SkyAssert(vmimage_pool);
        ret = vmimage_pool->InsertVmImage(pvm->_vid, pvm->_config._machine);
        SkyAssert(ret == 0);
        
        vm_pool->Put(pvm,true);
        
        ret = tsop.Commit();
        if (SUCCESS != ret)
        {
            SkyAssert(false);
            err << "tsop.Commit failed, ret = " << ret << endl;
            ret = ERROR_DB_UPDATE_FAIL;
        }

        return ret;
    }

    int64 GetVmMachineNeedSizeInHost(const VmDiskConfig &machine, int64 hid)
    {
        if (machine._position == VM_STORAGE_TO_SHARE)
        {
            return 0;
        }

        int64 local_size = GetVmMachineUsedSize(machine);

        if (IsUseSnapshot())
        {
            if (!IsImageDeployedtoHost(machine._id, hid))
            {
                local_size *= 2;
            }
        }

        return local_size;
    }

    int64 GetVmMachineUsedSize(const VmDiskConfig &machine)
    {
        return (VM_STORAGE_TO_LOCAL == ((machine)._position) ? CalcDiskRealSize(machine) : 0);
    }

    bool IsImageDeployedtoHost(int64 image_id, int64 hid)
    {
        vector<int64> hids;
        ostringstream where;
        where << "hid = " << hid << " and image_id = " << image_id;
        VirtualMachinePool *vm_pool = VirtualMachinePool::GetInstance();
        int ret = vm_pool->SelectColumn("host_using_image", "hid", where.str(), hids);

        if (ret == SQLDB_OK)
        {
            return true;
        }
        else if (ret == SQLDB_RESULT_EMPTY)
        {
            return false;
        }
        else 
        {
            SkyAssert(false);
            return false;
        }        
    }

    bool CouldUnbindVmfromHost()
    {
        if (GetVmLogicState(_req._vid) != VM_FROZEN)
        {
            return false;
        }

        VirtualMachine *pvm = Get(false);
        SkyAssert(pvm);
        if (pvm->_hid == INVALID_HID)
        {
            return false;
        }

        VMManager *vmm = VMManager::GetInstance();
        return vmm->IsVMSupportMigrate(_modification._vid);
    }

    STATUS UnbindVmfromHostbyWorkflow(const VMConfig &vm, int64 hid)
    {
        VmWorkFlow wf(*this);

        VmAction remove_action(_req._vid, hid, CANCEL);
        remove_action.SetFeedback(0, FEEDBACK_ACKVMOP_AND_DELETEWORKFLOW, 0);

        VmAction unbind_action(VM_WF_CATEGORY, VM_ACT_UNBIND_VM_FROM_HOST, EV_UNBIND_VM_FROM_HOST, LCM, LCM);
        unbind_action.SetMessageReq(_req);
        unbind_action._vid = _req._vid;

        //todo delete
        VmAction answer_action(VM_WF_CATEGORY, VM_ACT_ANSWER_UPSTREAM, EV_ANSWER_UPSTREAM, LCM, LCM);
        answer_action.SetMessageReq(_req);
        VmOperationAck ack(_req.action_id, SUCCESS, 100, "success", _req._vid, _req._operation);
        answer_action.message_ack = ack.serialize();
        answer_action._vid = _req._vid;

        wf.Add(remove_action, unbind_action);
        wf.Add(unbind_action, answer_action);

        STATUS ret = CreateWorkflow(wf);
        if(SUCCESS != ret)
        {
            _op_info << "UnbindVmfromHostbyWorkflow failed to created workflow: " << wf.get_id() << endl;
            OutPut(SYS_WARNING, _op_info.str());
            SkyAssert(false);
        }

        return ret;
    }

private:
    VmDiskConfig _machine;
};
RegisterDerivedCreater(vmmodify_creaters, VM_CFG_MACHINE, ModifyVmMachine);

STATUS DeleteVmMachine(MessageUnit *mu, int64 vid, const string &action_id)
{
    ostringstream err;
    STATUS ret = ModifyVmMachine::DeleteVmMachineinDatabase(vid, err);

    WorkAck ack(action_id, ret, 0, err.str());
    if (ret == SUCCESS)
    {
        ack.progress = 100;
    }
    else
    {
        ack.progress = 0;
    }
    AcktoUpstream(mu, ack);

    return FinishAction(action_id);
}

STATUS InsertVmMachine(MessageUnit *mu, int64 vid, const string &action_id)
{
    ostringstream err;
    STATUS ret = ModifyVmMachine::InsertVmMachineinDatabase(vid, action_id, err);

    WorkAck ack(action_id, ret, 0, err.str());
    if (ret == SUCCESS)
    {
        ack.progress = 100;
    }
    else
    {
        ack.progress = 0;
    }
    AcktoUpstream(mu, ack);

    return FinishAction(action_id);
}

class ModifyVmMutex : public ModifyVmcfg
{
public:
    ModifyVmMutex(const MessageFrame &message, MessageUnit *receiver) :
            ModifyVmcfg(message, receiver)
    {
    }
    virtual ~ModifyVmMutex(){}

    virtual STATUS Implement()
    {
        if (!IsVmExist())
        {
            _op_info << "Vm(" << _modification._vid << ") is not existed!" << endl;
            return ERR_OBJECT_NOT_EXIST;
        }

        if (IsDoing())
        {
            _op_info << "Vm(" << _modification._vid << ") is modifying!" << endl;
            return ERROR_ACTION_RUNNING;
        }

        string doing = GetVmDoing();
        if (!doing.empty())
        {
            _op_info << "Vm(" << _modification._vid << ") is doing " << doing << endl;
            return ERROR_PERMISSION_DENIED;
        }

        STATUS ret = AnalysisCommonModifyStruct();
        if (ret != SUCCESS)
        {
            return ret;
        }

        if (!IsVmcfgChanged())
        {
            return SUCCESS;
        }

        ret = CheckModificationSupport();
        if (ret != SUCCESS)
        {
            return ret;
        }

        ret = EffectModification();

        return ret;
    }

    virtual STATUS AnalysisCommonModifyStruct()
    {
        from_string<int64>(_src_vid, _modification._args["src"], dec);
        from_string<int64>(_des_vid, _modification._args["des"], dec);

        if (_modification._vid == _des_vid)
        {
            _des_vid = _src_vid;
            _src_vid = _modification._vid;
        }

        return SUCCESS;
    }

    virtual bool IsVmcfgChanged()
    {
        VirtualMachine *pvm = Get(false);
        SkyAssert(pvm);

        vector<int64>::iterator it = find(pvm->_requirement._repel_vms.begin(), pvm->_requirement._repel_vms.end(), _des_vid);

        if (_modification._operation == INSERT_VMCFG)
        {
            return (it ==  pvm->_requirement._repel_vms.end());
        }
        else if (_modification._operation == DELETE_VMCFG)
        {
            return (it !=  pvm->_requirement._repel_vms.end());
        }
        else
        {
            return false;
        }

        return false;
    }

    virtual STATUS CheckHostSupport()
    {
        if (_modification._operation == DELETE_VMCFG)
        {
            return SUCCESS;
        }
        else if (_modification._operation == UPDATE_VMCFG)
        {
            _op_info << "Only INSERT_VMCFG and DELETE_VMCFG is allowed!" << endl;
            return ERR_VMCFG_MODIFY_FORBIDDEN;
        }

        VirtualMachine *pvm = Get(false);
        SkyAssert(pvm);
        SkyAssert(pvm->_hid != INVALID_HID);

        ostringstream where;
        where << "hid = " << pvm->_hid << " and vid = " << _des_vid;
        VirtualMachinePool *vm_pool = VirtualMachinePool::GetInstance();
        int64 vid;
        int ret = vm_pool->SelectColumn("vm_pool", "vid", where.str(), vid);

        if (ret == SQLDB_RESULT_EMPTY)
        {
            return SUCCESS;
        }
        else
        {
            _op_info << "vm(" << _src_vid << ") and vm(" << _des_vid << ") is all in host(" << pvm->_hid << ")";
            return ERR_OBJECT_UPDATE_FAILED;
        }
    }

    virtual STATUS EffectModification()
    {
        VirtualMachinePool *vm_pool = VirtualMachinePool::GetInstance();
        SkyAssert(vm_pool);

        VirtualMachine *pvm1 = vm_pool->Get(_src_vid, true);
        SkyAssert(pvm1);
        ModifyMutex(pvm1->_requirement._repel_vms, (VmCfgOperation)_modification._operation, _des_vid);

        TransactionOp tsop;

        VirtualMachine *pvm2 = vm_pool->Get(_des_vid, true);
        if (pvm2)
        {
            ModifyMutex(pvm2->_requirement._repel_vms, (VmCfgOperation)_modification._operation, _src_vid);
            vm_pool->Update(pvm2);
            vm_pool->Put(pvm2,true);
        }

        vm_pool->Update(pvm1);
        vm_pool->Put(pvm1,true);

        STATUS ret = tsop.Commit();
        if (SUCCESS != ret)
        {
            SkyAssert(false);
            _op_info << "tsop.Commit failed, ret = " << ret << endl;
            ret = ERROR_DB_UPDATE_FAIL;
        }

        return ret;
    }

    void ModifyMutex(vector<int64> &repel_vms, VmCfgOperation op, int64 des_vid)
    {
        if (op == INSERT_VMCFG)
        {
            repel_vms.push_back(des_vid);
        }
        else if (op == DELETE_VMCFG)
        {
            repel_vms.erase(remove(repel_vms.begin(), repel_vms.end(), des_vid), repel_vms.end());
        }
    }

private:
    int64 _src_vid;
    int64 _des_vid;
};
RegisterDerivedCreater(vmmodify_creaters, VM_CFG_MUTEX, ModifyVmMutex);

class ModifyVmWatchdog : public ModifyVmcfg
{
public:
    ModifyVmWatchdog(const MessageFrame &message, MessageUnit *receiver) :
            ModifyVmcfg(message, receiver)
    {
    }
    virtual ~ModifyVmWatchdog(){}

    virtual STATUS AnalysisCommonModifyStruct()
    {
        from_string<int64>(_new_wdtime, _modification._args.begin()->second, dec);

        return SUCCESS;
    }

    virtual bool IsVmcfgChanged()
    {
        VirtualMachine *pvm = Get(false);
        SkyAssert(pvm);

        if (pvm->_config._vm_wdtime != _new_wdtime)
        {
            return true;
        }

        return false;
    }

    virtual STATUS CheckHostSupport()
    {
        return SUCCESS;
    }

    virtual STATUS EffectModification()
    {
        VirtualMachinePool *vm_pool = VirtualMachinePool::GetInstance();
        SkyAssert(vm_pool);

        VirtualMachine *pvm = vm_pool->Get(_req._vid, true);
        SkyAssert(pvm);
        pvm->_config._vm_wdtime = _new_wdtime;

        vm_pool->Update(pvm);
        vm_pool->Put(pvm,true);

        return SUCCESS;
    }

private:
    int64 _new_wdtime;
};
RegisterDerivedCreater(vmmodify_creaters, VM_CFG_WATCH_DOG, ModifyVmWatchdog);

}

