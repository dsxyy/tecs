#ifndef TECS_CC_VMLIFECYCLE_MANAGER_H
#define TECS_CC_VMLIFECYCLE_MANAGER_H
#include "sky.h"
#include "event.h"
#include "mid.h"
//#include "vm_messages.h"
#include "vm_pool.h"
#include "msg_host_manager_with_host_agent.h"
#include "msg_host_manager_with_high_available.h"
#include "vm_interface.h"
#include "vm_disk_url.h"
#include "config_vm_statistics.h"
#include "msg_project_manager_with_api_method.h"
namespace zte_tecs
{
#define InitConfig(config, table) \
    table = config::CreateInstance(GetDB()); \
    if (!table) \
    { \
        SkyAssert(0); \
        OutPut(SYS_EMERGENCY, #config"::CreateInstance failed!\n"); \
        return ERROR_NOT_READY; \
    }

bool IsTcuShare();
    
class LifecycleManager:public MessageHandlerTpl<MessageUnit>
{
public:
    enum VmOpStatus
    {
        LAUNCHING,
        FAILED,
        SUCCSSED,
        TIMEOUT
    };

    static LifecycleManager* GetInstance()
    {
        if(NULL == instance)
        {
            instance = new LifecycleManager(); 
        }
        return instance;
    };
    
    ~LifecycleManager()
    {
    };

    STATUS Init();
    void MessageEntry(const MessageFrame&); 
    static LcmState Transition(LcmState old_state,VmOperation op,VmOpStatus status);
    void ShowVmOpTimer(int64 vid);
    int SendVmOperateToHC(const VmOperationReq &operation, int hid);

private:
    LifecycleManager() : VmHandleAck(VmOperate)
    {
        cancel_timer = INVALID_TIMER_ID;
    }
    
    DISALLOW_COPY_AND_ASSIGN(LifecycleManager);
    static LifecycleManager *instance;
    STATUS HandleOpReq(const MessageFrame& message);
    STATUS HandleOpReq(const VmOperationReq &req);
    STATUS HandleOpAck(const MessageFrame& message);
    STATUS HandleOpAck(const VmOperationAck &ack, const string &hostname);
    STATUS HandleOpTimeout(int64 vid,ostringstream&);
    STATUS HandleMoniResult(const MessageFrame& message);
    STATUS CheckVmState(int64 vid);    
    int HandleCfgReq(const MessageFrame& message);
    void PowerOn( );
    int ResponseFeedback(const VmAction &action, const WorkAck &ack);
    int ResponseFeedback(const int func, const VmAction &action, const WorkAck &ack);
    int ModifyCreateSnapshot(const VmAction &action);
    int UpdateVMLocalImage(const VmAction &action);
    int UpdateVMShareImage(const VmAction &action);
    int UpdateVMShareBlock(const VmAction &action);
    int AddMigrateVMShareImage(const VmAction &action);
    int AddColdMigrateVMShareImage(const VmAction &action);
    int AddMigrateVMShareBlock(const VmAction &action);
    int AddColdMigrateVMShareBlock(const VmAction &action);
    int DeleteVMShareImage(const VmAction &action);
    int DeleteVMShareBlock(const VmAction &action);
    int FinishVMCancel(VmAction &action, const WorkReq &req);
    int EndVmOp(const VmAction &action);
    int PluginUSBFailed(const VmAction &action);
    int PlugoutUSBSuccessed(const VmAction &action);
    int ModifyCreateVMAction(const VmAction &action, const WorkReq &req);
    STATUS RecordVMRunInfo(const VmInfo &vminfo, int64 hid);
    int ModifyPlugPdiskAction(const VmAction &action);
    int PlugInPDiskSuccess(const VmAction &action);
    int PlugOutPDiskSuccess(const VmAction &action);
    int MigrateSuccess(const VmAction &action);
    int MigrateFailed(VmAction &action, const WorkReq &req);
    int BackupImgCreateSuccess(const VmAction &action);
    int BackupImgDeleteSuccess(const VmAction &action);    
    void DoScanForDrop();
    void SetStatisticsInfoToDB(const ResourceStatistics& vm_statistics,
                               int64 vid);
    int FinishVMFreeze(VmAction &action, const WorkReq &req);
    int ModifyNextAction(const VmAction &action, const WorkReq &req);
    STATUS DoProjectDynamicDataReq(const MessageFrame& message);   
    int32 GetProjectDynamicData(const string &where, vector<ProjectDynamicData> &project_data);
    int32  ProjectDataCallback(void *nil, SqlColumn *columns);
    int32 CountProjectDynamicData(void);
    int UnbindVmfromHost(const VmAction &action);
    int AnswerUpstream(const VmAction &action);
    void CheckVmAlarm();
    void SendVmOfflineAlarm(const VirtualMachine &vm);
    void SendVmStartFailedAlarm(const VirtualMachine &vm);
    void SendVmStateConflictAlarm(const VirtualMachine &vm);

    inline WorkReq GetWorkReqfromMsg(const MessageFrame& message)
    {
        WorkReq req;
        STATUS ret = req.deserialize(message.data);
        SkyAssert(ret == true);

        return req;
    }

    inline WorkAck GetWorkAckfromMsg(const MessageFrame& message)
    {
        WorkAck ack;
        STATUS ret = ack.deserialize(message.data);
        SkyAssert(ret == true);

        return ack;
    }
    
    template <class T>
    int UpdateActionVMConfigReq(const string &action_id)
    {
        string message;
        STATUS ret = GetActionMessageReq(action_id, message);
        SkyAssert(ret == SUCCESS);
    
        T vmop_req;
        bool result = vmop_req.deserialize(message);
        SkyAssert(result);
    
        VirtualMachine *pvm = vm_pool->Get(vmop_req._vid, false);
        SkyAssert(pvm);
        vmop_req._config = pvm->_config;
        ret = UpdateActionReq(action_id, vmop_req);
        SkyAssert(ret == SUCCESS);

        return ret;
    }

    HostPool                *host_pool;
    VirtualMachinePool      *vm_pool;
    map<OBJECT_ID, TIMER_ID> vmop_timers;
    TIMER_ID cancel_timer;

    VmInterface             *VmOperate;
    VmInterface             *&VmHandleAck;

    ConfigVmStatistics     *_vm_statistics;
    ConfigVmStatisticsNics *_vm_statistics_nics;

    static const char *_view_project_dynamic_data;
};
STATUS EnableLifeCycleOptions();
}
#endif

