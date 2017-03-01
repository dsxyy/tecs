#ifndef TECS_CC_VM_INTERFACE_H
#define TECS_CC_VM_INTERFACE_H

#include "vm_pool.h"
#include "vm_image.h"
#include "host_pool.h"
#include "event.h"

/* 综合vm_pool和vm_cancel_pool，提供统一接口 */

namespace zte_tecs
{

enum FeedBackFunc
{
    NONE = 0,
    FEEDBACK_RESTART_ACTION,
    FEEDBACK_ROLLBACK_WORKFLOW,
    FEEDBACK_END_VMOP,
    FEEDBACK_UPDATE_VMLOCALIMAGE,
    FEEDBACK_UPDATE_VMSHAREIMAGE,
    FEEDBACK_UPDATE_VMSHAREBLOCK,
    FEEDBACK_MIGRATE_ADD_VMSHAREBLOCK,
    FEEDBACK_COLDMIGRATE_ADD_VMSHAREBLOCK,
    FEEDBACK_PLUGINUSB_FAILED,
    FEEDBACK_PLUGOUTUSB_SUCCESSED,
    FEEDBACK_PLUGIN_PDISK,
    FEEDBACK_PLUGOUT_PDISK,
    FEEDBACK_MODIYF_PDISKREQ,
    FEEDBACK_MIGRATE_SUCCESS,
    FEEDBACK_MODIFY_CREATESNAPSHOT,
    FEEDBACK_MIGRATE_ADD_VMSHAREIMAGE,
    FEEDBACK_COLDMIGRATE_ADD_VMSHAREIMAGE,
    FEEDBACK_CREATE_BACKUPIMG,
    FEEDBACK_DELETE_WORKFLOW,
    FEEDBACK_DELETE_BACKUPIMG,
    FEEDBACK_DELETE_VMSHAREIMAGE,
    FEEDBACK_DELETE_VMSHAREBLOCK,
    FEEDBACK_RESCH_MIGRATE_FAILED,
    FEEDBACK_RESCH_MIGRATE_SUCCESS,
    FEEDBACK_ACKVMOP_AND_DELETEWORKFLOW,
    FEEDBACK_ACKVMOP_AND_FINISHACTION,
    FEEDBACK_DELETE_MACHINE,
    FEEDBACK_INSERT_MACHINE,

    PREDECESSOR_SET_REQ_FOR_REMOVE_VM,
    PREDECESSOR_SET_REQ_FOR_CREATE_VM,
    PREDECESSOR_SET_REQ_FOR_PREPARE_VM,

    FEEDBACK_NULL,//空操作，增加这个的目的是为了不让engine自动将action结束
};

class VmOpHandle;
class VmWorkFlow : public Workflow
{
public:
    VmWorkFlow() : _vid(labels.label_int64_1), _operation(labels.label_int64_2), _hid(labels.label_int64_3)
    {
        category = VM_WF_CATEGORY;
    }

    VmWorkFlow(const VmOpHandle &vmop);

    int64   &_vid;

    int64   &_operation;    //给TC应答的时候，需要带上这个，加了这个字段后省事，不需要从"name"转换

    int64   &_hid;
};

class VmDeployWorkFlow : public VmWorkFlow
{
public:
    VmDeployWorkFlow() : VmWorkFlow(),
                                _tcu(labels.label_int64_3),
                                _vcpu(labels.label_int64_4),
                                _memory(labels.label_int64_5),
                                _local_disk(labels.label_int64_6)
    {
    }
    
    int64   &_tcu;
    int64   &_vcpu;
    int64   &_memory;
    int64   &_local_disk;
};

class VmAction : public Action
{
public:
#define INITVMACTION \
    _vid(labels.label_int64_1), _hid(labels.label_int64_2), _hid_next(labels.label_int64_1)
    
    VmAction():Action(), INITVMACTION
    {
        Init();
    }
    
    VmAction(const string& _category,const string& _name,int _message_id,const MID& _sender,const MID& _receiver)
                        :Action(_category, _name, _message_id, _sender, _receiver), INITVMACTION
    {
        Init();
    }

    VmAction(int64 vid, int64 hid, VmOperation op);

    void SetFeedback(int success = 0, int failure = 0, int timeout = 0)
    {
        success_feedback = success;
        failure_feedback = failure;
        timeout_feedback = timeout;
    }

    void Init()
    {
        success_feedback = false;
        failure_feedback = false;
        timeout_feedback = false;
    }

    int64   &_vid;
    int64   &_hid;
    int64   &_hid_next;
};

/* 有可能对数据库有写操作的，可以考虑放在这个类里 */
class VmInterface
{
public:
    VmInterface(MessageUnit *mu) : _boss(mu),
        _vm_pool(VirtualMachinePool::GetInstance()),
        _vm_image_pool(VMImagePool::GetInstance()),
        _host_pool(HostPool::GetInstance()),
        _vm_device_pool(VmDevicePool::GetInstance()),
        _vm_usb_pool(VmUsbPool::GetInstance())
    {
        SkyAssert(_boss && _vm_pool && _host_pool
                && _vm_image_pool);
    }
    //int operator()(const VmOperationReq &req);
    int HandleOpReq(const VmOperationReq &req);
    int SetValue(int64 vid, const string &key, int64 value);
    int InsertVmUsbInfo(const VmUsbOp &info);
    int DeleteVmUsbInfo(const VmUsbOp &info);
    int Update(const VmInfo &vminfo);
    int ConstructCreateReq(int64 vid, VmCreateReq &createreq);
    int UpdateVMLocalImage(int64 vid, const ImageUrlPrepareAck &ack);
    int UpdateVMShareImage(const string &iscsi_name,int64 vid, const AuthBlockAck &ack,bool add);
    int ConstructRecoverReq(int64 vid, VmRecoverImageReq &recoverreq);
    int UpdateVMShareDisk(const string &iscsi_name,int64 vid, const AuthBlockAck &ack,bool add);
    int LiveMigrateSuccess(const VmAction &act);
    int DeleteVMWorkflow(int64 vid);
    int SendOperateToHC(int64 vid, int operation, int64 hid);
    int SetTimer(TIMER_ID &timer, uint32 cycle, uint32 msg_id, uint32 type = LOOP_TIMER);
    bool CheckReqDoing(const WorkReq &req);

private:
    int SendOperateToHC(const VirtualMachine *pvm);
    int SendOperateToHC(const VmOperationReq &operation, int hid);
    int LiveMigrateFailed(VmAction &act);

    MessageUnit                 *_boss;

    VirtualMachinePool          *_vm_pool;
    VMImagePool                 *_vm_image_pool;
    HostPool                    *_host_pool;
    VmDevicePool                *_vm_device_pool;
    VmUsbPool                   *_vm_usb_pool;

};

class TransactionOp
{
public:
    TransactionOp( ) : _lock(0)
    {
        Begin();
    }

    ~TransactionOp( )
    {
        if (_lock)
        {
            Transaction::Cancel( );
        }
    }

    int Commit( )
    {
        int ret = 0;

        if (_lock)
        {
            ret = Transaction::Commit( );
            if (SUCCESS == ret)
            {
                _lock = 0;
            }
        }

        return ret;
    }

private:
    void Begin( )
    {
        Transaction::Begin( );
        _lock ++;
    }

    int _lock;
};

#define UpdateActionMsgReq(wf_id, ac_name, msg_type, member_name, new_value) \
    do \
    { \
        vector<string> actions; \
        vector<string>::iterator it; \
        STATUS ret = GetActionsByName(wf_id, ac_name, actions); \
        SkyAssert(ret == SUCCESS); \
         \
        for (it = actions.begin(); it != actions.end(); it++) \
        { \
            string message; \
            ret = GetActionMessageReq(*it, message); \
            msg_type req; \
            bool result = req.deserialize(message); \
            SkyAssert(result);\
            req.member_name = new_value; \
            ret = UpdateActionReq(*it, req); \
            SkyAssert(ret == SUCCESS); \
        } \
    }while(0)

class FuncResponseFeedback
{
public:
    int ResponseWorkAck(const WorkAck &ack);
    int CommonResponseFeedback(const int func, const WorkAck &ack);
    virtual int ResponseFeedback(const int func, const WorkAck &ack)
    {
        return 0;
    };
};

int SendMsgToHC(MessageUnit *mu, const string &msg, int hid, int msg_id = EV_VM_OP,
                const string& process = PROC_HC, const string& unit = MU_VM_HANDLER);

bool CheckWorkReq(const MessageFrame &message);
bool CheckWorkReq(WorkReq &req);
int RecordAction(const string &action_id, int status = ERROR_ACTION_RUNNING);

int SendVmOpAck(MessageUnit *mu, const VmAction &action);
int GetVMOpWFName(int op, string &wf_name, string &action_name);
void AuthBlockAck2FileUrl(FileUrl& url,const string &iscsi_name,const AuthBlockAck& ack,bool add);
STATUS UpdateActionReq(const string &wf_id, const string &action_name, WorkReq &req);
STATUS UpdateActionReceiver(const string &wf_id, const string &action_name, const MID &receiver);
bool IsNeedVNet(const VMConfig &config);
int ResponseFeedback(const WorkAck &ack);
int ResponseFeedback(const int func, const WorkAck &ack);
int AcktoUpstream(MessageUnit *mu, const WorkAck &ack);
void SendVmAlarm(const VirtualMachine &vm, uint32 alarm_code, int flag);

class VmWfLock
{
public:
    VmWfLock(const string &id) : _id(id)
    {
        _get_locked = (LockWorkflow(id) == SUCCESS);
    }

    ~VmWfLock()
    {
        if (_get_locked)
        {
            UnlockWorkflow(_id);
            _get_locked = false;
        }
    }

    bool IsLocked()
    {
        return _get_locked;
    }

private:

    bool            _get_locked;
    const string    _id;
};

}

#endif

