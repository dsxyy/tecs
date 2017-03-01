/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vm_instance.h
* 文件标识：
* 内容摘要：VmInstance类的定义文件
* 当前版本：1.0
* 作    者：mhb
* 完成日期：2011年7月26日
*
* 修改记录1：
*     修改日期：2011/7/26
*     版 本 号：V1.0
*     修 改 人：mhb
*     修改内容：创建
*******************************************************************************/
#ifndef HC_VMINSTANCE_H
#define HC_VMINSTANCE_H
#include "vm_pub.h"
#include "vm_messages.h"
#include "vm_driver.h"
#include "host_storage.h"
#include "image_url.h"
#include "vnet_libnet.h"
#include "nfs.h"

extern string tecs_instance_path;
extern string tecs_ins_chk_path;
#define CREATE_SUCC     0  // 文件创建成功 
#define CREATE_EMPTY    1  // 文件为空，无需创建
#define CREATE_ERROR   -1  // 文件创建失败 

// 定义系统进度细节描述
#define WAIT_EXEC_SUCC      "Wait Vm Cmd Exec Success"
#define START_EXEC_CREATE   "Start Exec Create Cmd"
#define START_EXEC_SHUTDOWN "Start Exec ShutDown Cmd"
#define START_EXEC_PAUSE    "Start Exec pause Cmd"
#define START_EXEC_RESUME   "Start Exec Resume Cmd"
#define START_EXEC_REBOOT   "Start Exec Reboot Cmd"
#define START_EXEC_RESET    "Start Exec Reset Cmd"
#define START_EXEC_DESTROY  "Start Exec Destroy Cmd"
#define START_MOUNT_COREDUMP_URL "Start Mount CoreDump url"
#define START_PREP_NET      "Start Prepare NetWork Resource"
#define START_PREP_STORAGE  "Start Prepare Storage Resource "
#define START_PREP_CONTEXT  "Start Prepare Context"
#define START_PREP_CONFIG   "Start Prepare Configfile"
#define START_REL_NET       "Start Release NetWork"
#define START_REL_STORAGE   "Start Release Storage"
#define START_DEL_DIR       "Start Del Dir"
#define START_EXEC_MIGRATE  "Start Exec Migrate Cmd"

// 网络的进度安排在0-10
#define VM_NET_PROCESS     10

// 存储的进度安排在11-95
#define VM_STORAGE_PROCESS 85

// 计算的进度安排在96-100
#define VM_COMPUTER_PROCESS 5

// 定义虚拟机状态检查定时器
#define EV_TIMER_OP_RESULT  EV_TIMER_1

// 定义虚拟机状态刷新定时器
#define EV_TIMER_REFRESH    EV_TIMER_2

#define EV_TIMER_LOAD       EV_TIMER_3
/************************************************************************
 *                 虚拟机实例类                                           *
 ***********************************************************************/
class VmInstance : public MessageHandler
{
public:
    VMConfig       vm_cfg;                         /* 虚拟机配置信息 */
    VMOption       vm_option;                      /* 虚拟机选项信息 */

    //构造函数
    VmInstance()
    {
        _instance_id        = 0;
        _instance_name      = "instance_" + to_string<int64>(0,dec);
        CommonInit();
    };

    VmInstance(int64 vm_id)
    {
        _instance_id        = vm_id;
        _instance_name      = "instance_" + to_string<int64>(vm_id,dec);
        CommonInit();
    };

    //析构函数
    ~VmInstance()
    {
        // 删除虚拟机实例的监控
        DelKeyMu(_instance_name);
        delete _mu;
    };

    // 初始化函数，包括启动MU，
    STATUS Init();

    // 收到上电消息的处理函数
    STATUS DoStartup();

    // 启动虚拟机查找监控定时器，当操作返回RUNNING的时候启动
    void StartOpCheckTimer();

    // 启动刷新定时器
    void StartRefreshTimer();

    // 根据执行的结果进行一定的应答处理函数
    void DoOpResultFunction(STATUS rs);

    // 获取自己的运行版本
    void GetRunVersion();

    // 虚拟机操作结果检查函数
    STATUS DoOpCheck();

    // 虚拟机操作执行函数
    void ExecVmOperation(const MessageFrame& message);

    // 虚拟机保存为映像的函数
    STATUS InstanceUpload(const string& message);

    // STATUS FtpToImgServer(UploadUrl url, string image_name, int32 &progress);
    STATUS InstanceOpUsb(VmOperation cmd, const string &message);

    bool IsUsbInvalid(int vendor_id, int product_id, const string &name);

    STATUS InstanceAddUsb();

    // 虚拟机取消保存映像的函数
    STATUS InstanceCancelUpload();

    // 刷新虚拟机的CPU使用率信息
    void RefrushInstanceCpuUseRateInfo(VmDomainInfo &DomInfo);

    // 刷新虚拟机的网口统计信息
    void RefreshInstanceNicInfo();

    // 刷新虚拟机的状态信息
    void RefreshInstanceInfo();

    // 刷新虚拟机实例的输出信息
    void RefreshInstanceReportInfo(VmInfo &info);

    // 刷新虚拟机的状态信息
    bool GetInstanceReportInfo(VmInfo &info);

    //  存储设置进度函数
    void SetStorageProgress(uint32 progress);

    //  网络设置进度函数
    void SetNetProgress(uint32 progress);

    //  网络设置进度函数
    void SetComputerProgress(uint32 progress);

    //  设置进度函数
    void SetProgress(uint32 progress);

    //  设置进度函数
    void SetProgressDesc(const string &descrition)
    {
        _op_process_desc = descrition;
    }

    // 刷新虚拟机的状态
    void RefreshVmState();

    // 创建虚拟机
    STATUS CreateDomain(int op_option);

    // 启动虚拟机
    STATUS StartDomain();

    // 关闭虚拟机
    STATUS StopDomain();

    // 暂停虚拟机
    STATUS PauseDomain();

    // 恢复虚拟机
    STATUS ResumeDomain();

    // 重启虚拟机
    STATUS RebootDomain(bool need_stop_wgd = true);

    // 强制重启虚拟机
    STATUS ResetDomain(bool need_stop_wgd = true);

    // 强制关闭虚拟机
    STATUS DestroyDomain();

    // 虚拟机一键恢复镜像
    STATUS RecoverDomain(const string& message);

    // 填充disk信息
    void SetDiskInfo(vector<VmDiskConfig> &disks);

    // 根据存储执行的结果获取disk的信息
    void GetDiskInfo(vector<VmDiskConfig> &disks);

    //存储预处理
    void PredealStorage(const vector<VmDiskConfig> &disks);

    // 存储环境准备，包括下载映像和申请虚拟存储
    // 如果之前已经就绪，则由存储接口负责屏蔽
    STATUS StartStorage();

    // 存储资源释放，包括下载删除和虚拟存储回收
    STATUS ReleaseStorage();

    // 虚拟机操作应答函数
    int CreateConfigFile();

    // 虚拟机操作应答函数
    int CreateFile(const string &filename, const string &filecontext);

    // 创建TECS的上下文
    int CreateTecsContext();

    // 创建用户上下文
    int CreateUserContext();

    // 创建上下文
    int CreateContext();

    // 创建上下文的ISO
    int CreateContextIso();

    // 获取上下文设备的磁盘名称
    string GetContextDiskTarget();

    // 修改vm 的coredump文件名称
    void ModifyCoredumpFileName();

    // 虚拟机操作应答函数,
    void SendVmOprAckMsg(const MID &receiver, string &action_id, MessageUnit *mu);

    void SendVmOprAckMsg(const MID &receiver,  uint32 result,
                         int32 op_cmd, uint32 progress,
                         string &action_id, MessageUnit *mu);

    void SendVmOpAck2Wdg(const MID &receiver, CVNetNotifyHCrestatMsgAck &ack,
                         int32 msg_id,      MessageUnit *mu);

    void SendDel2VmHandler();

    // 准备资源
    STATUS PrepareResource(bool IsMigration);

    // 退出实例消亡
    void ExitDestroy(const STATUS rs);

    // 取消虚拟机
    void CancelDomain();

    // 进入实例消亡流程
    void EnterDestroy();

    // 释放资源的函数
    STATUS ReleaseResource();

    // 虚拟机实例目的端执行迁移准备的函数
    STATUS PrepareVmMigration(const string& message);

    // 虚拟机实例源端执行迁移命名的函数
    void ExecVmMigration(const string& message);

    // 虚拟机实例目的端迁移成功的执行函数
    STATUS HandleVmMigrateSucc(const string& message);

    STATUS PlugInPdisk(const string& message);

    STATUS PlugOutPdisk(const string& message);

    STATUS ImageBackupCreate(const string& message);

    STATUS ImageBackupDelete(const string& message);

    // 获取虚拟机的实例ID
    int GetDomainId()
    {
        return _domain_id;
    }

    // 获取虚拟机的id
    int64 GetInstanceId()
    {
        return _instance_id;
    }

    // 获取虚拟机实例的名称
    const string& GetInstanceName()
    {
        return _instance_name;
    }

    // 获取创建时间
    time_t GetLaunchTime()
    {
        return _launch_time;
    }

    // 获取虚拟机配置的版本号
    int32 GetVmVersion()
    {
        return _config_version;
    }

    // 设置创建时间
    void   SetLaunchTime()
    {
        _launch_time = time(NULL);
    }

    // 设置上次启动时间，主要用来保证60S内不能出现再次REBOOT
    void   SetBootTime()
    {
        _boot_time = time(NULL);
    }

    // 判断是否允许重启
    bool IsReboot()
    {
        time_t now = time(NULL);
        return ((now - _boot_time) > 60);
    }

    // 设置操作命令
    void SetOpCmd(uint32 cmd)
    {
        _op_cmd = cmd;
    }

    // 设置操作命令
    uint32 GetOpCmd()
    {
        return _op_cmd;
    }
    
    bool GetOpAckEnd()
    {
        return _op_ack_end;
    }

    uint32 GetOpResult()
    {
        return _op_result;
    }


    // 获取当前的action_id
    bool IsSameActionId(string &action_id)
    {
        return (action_id == _op_action_id);
    }

    // 设置操作结果
    void SetOpResult(uint32 result)
    {
        _op_result = result;

        // 当设置RUNNING的时候，需要设置应答结束标志为false
        if (_op_result == VMOP_RUNNING)
        {
            _op_process_desc = "";
            _op_ack_end = false;
        }
    }

    // 判断操作流程是否结束
    bool IsOpRunning()
    {
        if (_op_ack_end)
        {
            return false;
        }
            
        //  如果结果是正在操作中，责返回正在运行
        if (_op_result == VMOP_RUNNING)
        {
            return true;
        }

        return false;
    }


    bool HasNetwork()
    {
        if((0 ==  vm_cfg._nics.size())
                &&(0 == vm_cfg._vm_wdtime))
        {
            return false ;
        }
        return true;
    }

    // 清除ACK应答结束标志
    void CleanAckEndFlag()
    {
        _op_ack_end = true;
    }

    //  设置实例是否处于待消亡状态
    bool InstanceIsDestroy()
    {
        return _is_instance_del;
    }

    //  清除实例消亡标识，用在CANCEL和MIGRATE失败的时候
    void VmClearDestroyFlag()
    {
        _is_instance_del = false;
    }

    //  设置实例消亡标志
    void VmSetDestroyFlag()
    {
        _is_instance_del = true;
    };

    // 获取当前实例收到的消息的发送者
    void GetOpSender(MID &sender)
    {
        sender = _op_sender;
    };

    // 判断实例是否取消保存映像
    bool InstanceIsUnSave()
    {
        // 如果被删除了
        if (InstanceIsDestroy())
        {
            return true;
        }

        return _unsave_flag;
    }

    // 设置虚拟机为UNSAVE
    void SetInstanceUnSave()
    {
        _unsave_flag = true;
    }

    // 清除虚拟机为UNSAVE
    void ClearInstanceUnSave()
    {
        _unsave_flag = false;
    }

    void ClearDoubleCmdFlag()
    {
        _double_op_cmd       = UNKNOWN_OP;
        _double_op_wait_time = 0;
    }

    void MessageEntry(const MessageFrame& frame);

    // 打印当前虚拟机的实例内容
    void PrintInstanceInfo(bool flag)
    {
        Datetime launchtime(_launch_time);
        Datetime boottime(_boot_time);

        cout << " Instance id: " << _instance_id << " Name: " << _instance_name<< endl;
        cout << " Domain id:" << _domain_id<<endl;
        cout << " Launch time: " << launchtime.tostr()<< endl;
        cout << " Boot time: " << boottime.tostr()<< endl;
        cout << " Run time: " << _domain_run_time << "ns"<< endl;
        cout << " Refresh time: " << _refresh_time << " ms"<< endl;
        cout << " instanc del Flag: " << _is_instance_del<< endl;
        cout << " Unsave Flag: " << _unsave_flag<< endl;
        cout << " Vm State: " <<  Enum2Str::InstStateStr(_state)<< endl;
        cout << " Vm run version: " << _run_version<< endl;
        cout << " config version: " << _config_version<< endl;

        cout << " op cmd: " << Enum2Str::VmOpStr(_op_cmd)<< endl;
        cout << " op progress: " << _op_progress<< endl;
        cout << " op result: " << Enum2Str::VmOpError(_op_result)<< endl;
        cout << " op progress detail: " << _op_process_desc<< endl;
        cout << " op domid: " << _op_domid<< endl;
        cout << " op action id: " << _op_action_id<< endl;
        cout << " op sender: " << _op_sender<< endl;
        cout << " op wait time: " <<_wait_time<< " s"<< endl;
        cout << " op ack end: " <<_op_ack_end<< endl;
        cout << " RR Or SS wait Time:" <<_double_op_wait_time<< " s"<< endl;
        cout << " double op cmd: " <<  Enum2Str::VmOpStr(_double_op_cmd)<< endl;

        // 正常打印不打印配置信息，这个可以看配置文件，或者指定打开，不然屏幕打印太多
        if (flag)
        {
            cout << " Vm Config: " << vm_cfg.serialize() << endl;
            cout << " Vm Option: " << vm_option.serialize() << endl;
        }
        cout << "**********************************************" << endl;
    }

    // 设置虚拟机配置
    void SetVmCfg(VMConfig cfg)
    {
        vm_cfg = cfg;
        _vcpu_num = cfg._vcpu;
        _config_version = cfg._config_version;
        _run_version = -1;
    }

    // 设置虚拟机选项
    void SetVmOption(VMOption option)
    {
        vm_option = option;
    }

    // 设置正在执行的actionid
    void SetActionID(string& actionid )
    {
        _op_action_id = actionid;
    }
    
    // 获取虚拟机操作进度
    const uint32 GetProgress()
    {
        return _op_progress;
    }

    
    // 获取虚拟机操作进度
    const string& GetActionID()
    {
        return _op_action_id;
    }

    const int32 GetVmState()
    {
        return _state;
    }

    void RefreshLoadBlocks();

    void DoVmStatistics(ResourceStatistics& vm_statistics,
                        int64& vid);

    // 辅助函数
    void GetNicUsedRate(vector<NicInfo>& vec_nic_info);
    int32 XenVmStatistics(ResourceStatistics& vm_statistics,
                          const int64& vid);
    int32 KvmVmStatistics(ResourceStatistics& vm_statistics,
                          const int64& vid);
    int32 GetVnicSpeed(string strallports, int64& ispeed);
    
private:
    int64          _instance_id;                   // 虚拟机调度下发的实例编号
    string         _instance_name;                 // 用instanceid构造的实例名
    int            _domain_id;                     // hypervisor维护的domain编号
    int32          _vcpu_num;                      // 虚拟机的CPU数量
    int32          _config_version;                // 虚拟机配置版本
    int32          _run_version;                   // 虚拟机正在运行的版本，这个只在刷新的时候更新

    time_t         _launch_time;                   // 收到创建请求的时间
    time_t         _boot_time;                     // 上次启动的时间，系统两次启动时间不能短于1分钟
    uint64         _domain_run_time;               // domain运行时间,单位ns
    int64          _refresh_time;                  // 刷新状态的时间 单位毫秒
    bool           _is_instance_del;               // 当前实例是否收到CANCEL消息
    Mutex         _refresh_lock;                  // 刷新锁
    bool           _unsave_flag;                   // 取消虚拟机保存的标准
    uint32         _report_count;
    time_t         _pre_statistics_time;

    ResourceStatistics   _vm_statistics;
    /* 前一次的物理机资源使用信息
     *   ps: 目前主要是网卡的使用率会用到
     */
    ResourceStatistics  _pre_vm_statistics;

    STATUS StartMu();                              // 启动MU

    #define DOM_EXIST             1
    #define DOM_NOT_EXIST    0
    #define DOM_UNKNOWN       -1
    void CommonInit()
    {
        _domain_id          = 0;
        _run_version        = -1;
        _config_version     = 0;

        _launch_time        = 0;
        _boot_time          = 0;
        _mu                 = NULL;
        _domain_run_time    = 0;
        _refresh_time       = 0;
        _vcpu_num           = 1;
        _is_instance_del    = false;
        _op_check_timer     = INVALID_TIMER_ID;
        _refresh_timer      = INVALID_TIMER_ID;
        _load_timer         = INVALID_TIMER_ID;
        _op_cmd             = UNKNOWN_OP;
        _op_progress        = 0;
        _op_result          = VMOP_SUCC;
        _refresh_lock.Init();
        _wait_time          = 0;
        _op_ack_end         = true;
        _double_op_wait_time= 0;
        _double_op_cmd      = UNKNOWN_OP;
        _unsave_flag             = false;

        _cpu_usage          = 0;
        _mem_max            = 0;
        _mem_free           = 0;
        _net_tx             = 0;
        _net_rx             = 0;
        _state              = VM_INIT;
        _report_count       = 0;
        _pre_statistics_time = time(0);
        // 增加虚拟机实例的监控
        AddKeyMu(_instance_name);        
    };
    int DomainIsExist() const;
    bool CreateCfgParaIsValid();
    bool  StartWDGTimer(const char *const op_fun);
    bool  StopWDGTimer(const char *const op_fun);
    STATUS StartOpCheckWDG();
    STATUS CheckOpTimeOut();
    bool NotifyNetDomIDChg(const char *const  fun_name);
    // 申请网络资源接口
    bool StartNetWork(const char *const  fun_name, BYTE option);
    // 释放网络资源接口
    bool StopNetWork(const char *const  fun_name, BYTE option);
    void GetCoredumpFilePath(string &path);
    void GetSdPciInfo(vector<PciBus> &pci_info);
    bool SendMsgToVnetWithTimeOut(const Serializable& message,
                                  uint32 msg_id,
                                  uint32 ack_id,
                                  uint32 time_out,
                                  const string &request_stamp,
                                  string &out_msg);
    bool OperateWatchDog(const char *const  fun_name, int32 op_id);


    // 引用外部的实例的指针
    MessageUnit       *_mu;                        // 虚拟机实例的消息单元
    VmDriver          *_vm_driver;                 // 操作虚拟机的接口指针
    StorageOperation  *_storage_instance;           // 存贮的实例指针

    // 操作信息
    int32          _op_cmd;                        // 当前操作的命令
    uint32         _op_progress;                    // 当前操作的进度
    uint32         _op_result;                     // 当前操作的结果
    string         _op_process_desc;               // 操作进度的详细描述
    uint32         _op_domid;                      // 操作时候的domid,主要用在检测某些操作是否完成
    string         _op_action_id;                 // 当前操作的工作流ID
    TIMER_ID       _op_check_timer;                // 操作检查定时器
    MID            _op_sender;                     // 当前消息执行的发送者
    uint32         _wait_time;                     // 操作等待时间
    bool           _op_ack_end;                    // 实例是否成功回完应答
    TIMER_ID       _refresh_timer;                 // 状态刷新定时器
    int32          _double_op_wait_time;           // REBOOT和RESET，以及SHUTOFF和destroy之间的时延
    int32          _double_op_cmd;                 // 上面双操作的子命令
    TIMER_ID       _load_timer;                   // 操作检查定时器

    // 状态信息
    int32           _cpu_usage;                    // 虚拟机的CPU占用率
    int64           _mem_max;                      // 虚拟机的内存最大值
    int64           _mem_free;                     // 虚拟机的空闲内存大小
    int64           _net_tx;                       // 虚拟机的网络发送流量
    int64           _net_rx;                       // 虚拟机的网络接收流量
    int32           _state;                        // 虚拟机的状态

    VmInfo          _out_info;                     // 给监控输出的信息。

};
#endif // #ifndef HC_VMINSTANCE_H



