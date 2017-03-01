#ifndef TECS_CC_VMSCHEDULER_H
#define TECS_CC_VMSCHEDULER_H
#include "sky.h"
#include "event.h"
#include "mid.h"
//#include "vm_messages.h"
#include "vm_pool.h"
#include "vm_interface.h"

namespace zte_tecs
{
class SchedulingVM;

#define SCH_POLICY_PACKING      "packing"
#define SCH_POLICY_STRIPING     "striping"

class Scheduler:public MessageHandlerTpl<MessageUnit>, public FuncResponseFeedback
{
public:
    enum HostPolicyType
    {
        SIMPLE_POLICY = 0,
        PACKING,    //填满
        STRIPING,   //分散
    };

    static Scheduler* GetInstance()
    {
        if(NULL == instance)
        {
            instance = new Scheduler();
        }

        return instance;
    };

    ~Scheduler()
    {
    };

    STATUS Init();
    void MessageEntry(const MessageFrame&);
    bool VerifyVmRelations(const vector<int64>& existing,
                           const vector<int64>& repels) const;
    STATUS ScheduleNow();
    void SortbyImage(VMConfig const &vmcfg, vector<int64> &hosts);
    int ResponseFeedback(const int func, const WorkAck &ack);
    STATUS GetNewPortNo(const int rangemin,const int rangemax, vector<uint16> blacklist,uint16 &newportno);

private:
    Scheduler()
    {
        schedule_timer = INVALID_TIMER_ID;
        _running_sch_timer = INVALID_TIMER_ID;
    };

    DISALLOW_COPY_AND_ASSIGN(Scheduler);
    static Scheduler *instance;
    STATUS AcceptVM(const VMDeployInfo&);
    STATUS SelectHost(int64& hid, vector< PhyNic > &ports, const VMConfig&, const VmRequirement&);
    STATUS SelectHost(int64& hid, const VMDeployInfo &deploy);
    STATUS Schedule(const VMDeployInfo&,ostringstream&);
    STATUS Schedule(const VmOperationReq&,ostringstream&);
    STATUS VerifyHostShare(int64 hid);
    int64 ApplyHostPolicy(const SchedulingVM &schdl_vm,HostPolicyType policy);
    int64 CalcLocalDiskSize(const VMConfig &vmcfg, int isMax = 1);
    STATUS MigrateVM( const MessageFrame& message, ostringstream& error );
    STATUS MigrateVM( const VmMigrateReq &req, ostringstream& error );

    int SaveChngedCfg(const VMConfig &newcfg);
    bool IsChngCfgOK(const VMConfig &newcfg);
    int HandleChngCfgReq(const MessageFrame& message);
    int CalcVmCpuPercent(VirtualMachine &vm);
    STATUS FilterHostbyBaseInfo(SchedulingVM &schdl, string &err);
    STATUS FilterHostbyImage(SchedulingVM &schdl, string &err);
    STATUS FilterHostbyCancels(SchedulingVM &schdl, string &err);
    STATUS FilterHostbyNetwork(SchedulingVM &schdl, string &err);
    STATUS FilterHostbyRepel(SchedulingVM &schdl, string &err);
    STATUS FilterHostbySDevice(SchedulingVM &schdl, string &err);
    STATUS FilterHostbyRegion(SchedulingVM &schdl, string &err);
    STATUS FilterHost(SchedulingVM &schdl);
    STATUS SchSingle(const MessageFrame& message, ostringstream& oss);
    STATUS ModifyDeployWorkflow(const string action_id);
    STATUS ModifyMigrateWorkflow(const string action_id);
    STATUS ModifyColdMigrateWorkflow(const string action_id);
    STATUS ColdMigrateVM(const MessageFrame& message, ostringstream& oss);
    void SetRunningReSCH();
    void RunningReSCH();
    void RunningReSCH(const vector<int64> &vids, const string &sch_policy, int migrate_max);
    int ReSchMigrateFailed(const WorkAck &ack);
    int ReSchMigrateSuccess(const WorkAck &ack);
    STATUS ReSchSingle(const MessageFrame& message, ostringstream& oss);
    STATUS ModifyUnfreezeWorkflow(const string action_id);
    STATUS GetNewVNCPortNo(uint16 &newportno,vector<uint16> blacklist);
    STATUS GetNewSerialPortNo(uint16 &newportno,vector<uint16> blacklist);

    //MessageUnit *m;
    VirtualMachinePool *vm_pool;
    class HostPool *host_pool;

    class VmInterface   *VmAssistant;

    TIMER_ID schedule_timer;
    TIMER_ID _running_sch_timer;

};

class SchedulingVM {
public:
    SchedulingVM() : _config(_deploy._config), _require(_deploy._requirement)
    {
    }

    SchedulingVM(const VMDeployInfo &deploy) : _deploy(deploy), _config(_deploy._config), _require(_deploy._requirement)
    {
    }

    VMDeployInfo        _deploy;
    VMConfig            &_config;        /* 虚拟机的配置 */
    VmRequirement       &_require;       /* 对虚拟机的分配要求 */

    vector<int64>       _hids;          /* 过滤出来，暂时符合部分条件的虚拟机 */

    //map< int64, vector < PhyNic > > _hostports; /* 符合条件的HC的端口 */
};

typedef STATUS (Scheduler::*FileterHost)(SchedulingVM &schdl, string &err);

STATUS EnableSchedulerOptions();
int GetMaxScheduleHosts();
}
#endif

