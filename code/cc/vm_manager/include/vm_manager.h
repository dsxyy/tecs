#ifndef TECS_CC_VMMANAGER_H
#define TECS_CC_VMMANAGER_H
#include "sky.h"
#include "event.h"
#include "mid.h"
//#include "vm_messages.h"
#include "vm_pool.h"
#include "registered_system.h"
//#include "nat_manager_iptables.h"
#include "host_pool.h"
#include "vm_interface.h"
#include "config_vm_statistics.h"

namespace zte_tecs
{
#define SCHED_STAMP      MU_VM_SCHEDULER
#define LCM_STAMP        MU_VM_LIFECYCLE_MANAGER
#define VMM_STAMP        MU_VM_MANAGER

class MessageFrameHandling;

class VMManager:public MessageHandlerTpl<MessageUnit>
{
public:
    static VMManager* GetInstance()
    {
        if(NULL == instance)
        {
            instance = new VMManager();
        }

        return instance;
    };

    ~VMManager()
    {
    };

    STATUS Init();
    void MessageEntry(const MessageFrame&);
    STATUS OperateStartVM(const VmOperationReq& req, const MID sender, ostringstream& error);
    bool IsVMSupportMigrate(int64 vid);
    bool IsVMSupportLiveMigrate(int64 vid);

    inline static STATUS SendVmOperationAck(MessageUnit *mu,const VmOperationAck& ack, const MID &receiver)
    {
        MessageOption option(receiver,EV_VM_OP_ACK,0,0);
        return mu->Send(ack, option);
    }

private:
    VMManager()
    {
        _usb_query_timer_id = INVALID_TIMER_ID;
    };
    DISALLOW_COPY_AND_ASSIGN(VMManager);
    static VMManager *instance;
    static string tc;
    STATUS DeployVM(VMDeployInfo&, const MID&, ostringstream&);
    STATUS DeployVM(const MessageFrame& message, ostringstream& error);
    STATUS OperateVM(const VmOperationReq& req,ostringstream&);
    STATUS MigrateVM( const MessageFrame& message, ostringstream& error );
    STATUS MigrateVM(const VmOperationReq &req, const string &req_info, const MID &sender, ostringstream& error );
    STATUS QueryVM(const VmQueryReq& req,ostringstream&);
    bool LegalOperation(VmOperation op,VmInstanceState inst_state,LcmState lcm_state);
    bool LegalOperation(VmOperation op, VmOperation last_op, STATUS last_op_result);
    bool LegalOperationforLogState(VmOperation op, VmInstanceState log_state);

    int SyncVmCfg(const MessageFrame &message);
    int HandleSelectHostAck(const MessageFrame &message);
    int PrepareDeployVMConfig(VMConfig &onfig);
    STATUS DealVmRestoreReq(const MessageFrame &message);
    STATUS DealVmRestoreAck(const MessageFrame &message);

    STATUS OperateVM(const MessageFrame& message, ostringstream& error);
    STATUS OperateVM(const VmOperationReq& req, MID sender, const string &req_info, int &progress, ostringstream& error);
    STATUS DoOperation(const VmOperationReq& req, MID sender, const string &req_info, int &progress, ostringstream& error);
    STATUS OperateCancelVM(const VmOperationReq& req, const MID sender, int &progress, ostringstream& error);
    STATUS AddCancelWorkFLow(const VmOperationReq& req, const MID sender, ostringstream& error);
    STATUS OperatePluginUSB(const VmOperationReq& req, MID sender, const string &req_info, ostringstream& error);
    STATUS OperatePlugoutUSB(const VmOperationReq& req, MID sender, const string &req_info, ostringstream& error);
    STATUS OperateRecoverImage(const VmOperationReq& req, const MID sender, ostringstream& error);
    STATUS AddCommonWorkflow(VmOperationReq &req, MID sender, const int success_func = FEEDBACK_END_VMOP, const int failure_func = FEEDBACK_DELETE_WORKFLOW);
    STATUS DoCommonOp(VmOperationReq &req, MID sender, const int success_func = FEEDBACK_END_VMOP, const int failure_func = FEEDBACK_DELETE_WORKFLOW);
    STATUS SetExpectedState(const VmOperationReq& req);
    STATUS OperateAttachPdisk(const VmOperationReq& req, const MID& sender, const string &req_info, int& progress, ostringstream& error);
    STATUS AttachPdisk(VmPortableDiskOp& req, const MID& sender, ostringstream& error);
    STATUS OperateDetachPdisk(const VmOperationReq& req, const MID& sender, const string &req_info, int& progress, ostringstream& error);
    STATUS DetachPdisk(VmPortableDiskOp& req, const MID& sender, ostringstream& error);
    STATUS OperateModifyImageBackup(const string &req, ostringstream& error);
    STATUS ColdMigrate(const VmOperationReq &req, const string &req_info, const MID &sender, ostringstream& error );
    STATUS OperateRestoreVM(const VmOperationReq& req, const string &req_info,const MID sender, int &progress,ostringstream& error);
    STATUS OperateBackupImg(const VmOperationReq& req, const MID& sender, const string &req_info, int& progress, ostringstream& error);
    STATUS BackupImgCreate(VmImageBackupOp& req, const MID& sender, ostringstream& error);
    STATUS OperateDeleteBackupImg(const VmOperationReq& req, const MID& sender, const string &req_info, int& progress, ostringstream& error);
    STATUS BackupImgDelete(VmImageBackupOp& req, const MID& sender, ostringstream& error);
    int GetWorkflowProgress(const string& action_id);
    STATUS Freeze(const VmOperationReq& req, const MID sender, ostringstream& error);
    STATUS UnFreeze(const VmOperationReq& req, const MID sender, ostringstream& error);
    STATUS ModifyVmCfg(const MessageFrame& message);
    MessageFrameHandling* CreateModifyVmcfgHandling(const MessageFrame &message, MessageUnit *receiver);

    //MessageUnit *m;
    VirtualMachinePool  *vm_pool;
    class HostPool      *host_pool;
    VmUsbPool           *usb_pool;
    ConfigVmStatisticsNics  *vm_statistics_nics_pool;
    TIMER_ID              _usb_query_timer_id;

    class VmInterface   *VmAssistant;

    int     DealVmUsbInfoQueryTimeOut();
    int     DealVmUsbInfoQueryFromApi(const MessageFrame &message);
    int     DealVmUsbInfoQueryFromHc(const MessageFrame &message);
};
}
#endif

