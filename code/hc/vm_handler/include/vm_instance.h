/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�vm_instance.h
* �ļ���ʶ��
* ����ժҪ��VmInstance��Ķ����ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�mhb
* ������ڣ�2011��7��26��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/7/26
*     �� �� �ţ�V1.0
*     �� �� �ˣ�mhb
*     �޸����ݣ�����
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
#define CREATE_SUCC     0  // �ļ������ɹ� 
#define CREATE_EMPTY    1  // �ļ�Ϊ�գ����贴��
#define CREATE_ERROR   -1  // �ļ�����ʧ�� 

// ����ϵͳ����ϸ������
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

// ����Ľ��Ȱ�����0-10
#define VM_NET_PROCESS     10

// �洢�Ľ��Ȱ�����11-95
#define VM_STORAGE_PROCESS 85

// ����Ľ��Ȱ�����96-100
#define VM_COMPUTER_PROCESS 5

// ���������״̬��鶨ʱ��
#define EV_TIMER_OP_RESULT  EV_TIMER_1

// ���������״̬ˢ�¶�ʱ��
#define EV_TIMER_REFRESH    EV_TIMER_2

#define EV_TIMER_LOAD       EV_TIMER_3
/************************************************************************
 *                 �����ʵ����                                           *
 ***********************************************************************/
class VmInstance : public MessageHandler
{
public:
    VMConfig       vm_cfg;                         /* �����������Ϣ */
    VMOption       vm_option;                      /* �����ѡ����Ϣ */

    //���캯��
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

    //��������
    ~VmInstance()
    {
        // ɾ�������ʵ���ļ��
        DelKeyMu(_instance_name);
        delete _mu;
    };

    // ��ʼ����������������MU��
    STATUS Init();

    // �յ��ϵ���Ϣ�Ĵ�����
    STATUS DoStartup();

    // ������������Ҽ�ض�ʱ��������������RUNNING��ʱ������
    void StartOpCheckTimer();

    // ����ˢ�¶�ʱ��
    void StartRefreshTimer();

    // ����ִ�еĽ������һ����Ӧ������
    void DoOpResultFunction(STATUS rs);

    // ��ȡ�Լ������а汾
    void GetRunVersion();

    // ��������������麯��
    STATUS DoOpCheck();

    // ���������ִ�к���
    void ExecVmOperation(const MessageFrame& message);

    // ���������Ϊӳ��ĺ���
    STATUS InstanceUpload(const string& message);

    // STATUS FtpToImgServer(UploadUrl url, string image_name, int32 &progress);
    STATUS InstanceOpUsb(VmOperation cmd, const string &message);

    bool IsUsbInvalid(int vendor_id, int product_id, const string &name);

    STATUS InstanceAddUsb();

    // �����ȡ������ӳ��ĺ���
    STATUS InstanceCancelUpload();

    // ˢ���������CPUʹ������Ϣ
    void RefrushInstanceCpuUseRateInfo(VmDomainInfo &DomInfo);

    // ˢ�������������ͳ����Ϣ
    void RefreshInstanceNicInfo();

    // ˢ���������״̬��Ϣ
    void RefreshInstanceInfo();

    // ˢ�������ʵ���������Ϣ
    void RefreshInstanceReportInfo(VmInfo &info);

    // ˢ���������״̬��Ϣ
    bool GetInstanceReportInfo(VmInfo &info);

    //  �洢���ý��Ⱥ���
    void SetStorageProgress(uint32 progress);

    //  �������ý��Ⱥ���
    void SetNetProgress(uint32 progress);

    //  �������ý��Ⱥ���
    void SetComputerProgress(uint32 progress);

    //  ���ý��Ⱥ���
    void SetProgress(uint32 progress);

    //  ���ý��Ⱥ���
    void SetProgressDesc(const string &descrition)
    {
        _op_process_desc = descrition;
    }

    // ˢ���������״̬
    void RefreshVmState();

    // ���������
    STATUS CreateDomain(int op_option);

    // ���������
    STATUS StartDomain();

    // �ر������
    STATUS StopDomain();

    // ��ͣ�����
    STATUS PauseDomain();

    // �ָ������
    STATUS ResumeDomain();

    // ���������
    STATUS RebootDomain(bool need_stop_wgd = true);

    // ǿ�����������
    STATUS ResetDomain(bool need_stop_wgd = true);

    // ǿ�ƹر������
    STATUS DestroyDomain();

    // �����һ���ָ�����
    STATUS RecoverDomain(const string& message);

    // ���disk��Ϣ
    void SetDiskInfo(vector<VmDiskConfig> &disks);

    // ���ݴ洢ִ�еĽ����ȡdisk����Ϣ
    void GetDiskInfo(vector<VmDiskConfig> &disks);

    //�洢Ԥ����
    void PredealStorage(const vector<VmDiskConfig> &disks);

    // �洢����׼������������ӳ�����������洢
    // ���֮ǰ�Ѿ����������ɴ洢�ӿڸ�������
    STATUS StartStorage();

    // �洢��Դ�ͷţ���������ɾ��������洢����
    STATUS ReleaseStorage();

    // ���������Ӧ����
    int CreateConfigFile();

    // ���������Ӧ����
    int CreateFile(const string &filename, const string &filecontext);

    // ����TECS��������
    int CreateTecsContext();

    // �����û�������
    int CreateUserContext();

    // ����������
    int CreateContext();

    // ���������ĵ�ISO
    int CreateContextIso();

    // ��ȡ�������豸�Ĵ�������
    string GetContextDiskTarget();

    // �޸�vm ��coredump�ļ�����
    void ModifyCoredumpFileName();

    // ���������Ӧ����,
    void SendVmOprAckMsg(const MID &receiver, string &action_id, MessageUnit *mu);

    void SendVmOprAckMsg(const MID &receiver,  uint32 result,
                         int32 op_cmd, uint32 progress,
                         string &action_id, MessageUnit *mu);

    void SendVmOpAck2Wdg(const MID &receiver, CVNetNotifyHCrestatMsgAck &ack,
                         int32 msg_id,      MessageUnit *mu);

    void SendDel2VmHandler();

    // ׼����Դ
    STATUS PrepareResource(bool IsMigration);

    // �˳�ʵ������
    void ExitDestroy(const STATUS rs);

    // ȡ�������
    void CancelDomain();

    // ����ʵ����������
    void EnterDestroy();

    // �ͷ���Դ�ĺ���
    STATUS ReleaseResource();

    // �����ʵ��Ŀ�Ķ�ִ��Ǩ��׼���ĺ���
    STATUS PrepareVmMigration(const string& message);

    // �����ʵ��Դ��ִ��Ǩ�������ĺ���
    void ExecVmMigration(const string& message);

    // �����ʵ��Ŀ�Ķ�Ǩ�Ƴɹ���ִ�к���
    STATUS HandleVmMigrateSucc(const string& message);

    STATUS PlugInPdisk(const string& message);

    STATUS PlugOutPdisk(const string& message);

    STATUS ImageBackupCreate(const string& message);

    STATUS ImageBackupDelete(const string& message);

    // ��ȡ�������ʵ��ID
    int GetDomainId()
    {
        return _domain_id;
    }

    // ��ȡ�������id
    int64 GetInstanceId()
    {
        return _instance_id;
    }

    // ��ȡ�����ʵ��������
    const string& GetInstanceName()
    {
        return _instance_name;
    }

    // ��ȡ����ʱ��
    time_t GetLaunchTime()
    {
        return _launch_time;
    }

    // ��ȡ��������õİ汾��
    int32 GetVmVersion()
    {
        return _config_version;
    }

    // ���ô���ʱ��
    void   SetLaunchTime()
    {
        _launch_time = time(NULL);
    }

    // �����ϴ�����ʱ�䣬��Ҫ������֤60S�ڲ��ܳ����ٴ�REBOOT
    void   SetBootTime()
    {
        _boot_time = time(NULL);
    }

    // �ж��Ƿ���������
    bool IsReboot()
    {
        time_t now = time(NULL);
        return ((now - _boot_time) > 60);
    }

    // ���ò�������
    void SetOpCmd(uint32 cmd)
    {
        _op_cmd = cmd;
    }

    // ���ò�������
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


    // ��ȡ��ǰ��action_id
    bool IsSameActionId(string &action_id)
    {
        return (action_id == _op_action_id);
    }

    // ���ò������
    void SetOpResult(uint32 result)
    {
        _op_result = result;

        // ������RUNNING��ʱ����Ҫ����Ӧ�������־Ϊfalse
        if (_op_result == VMOP_RUNNING)
        {
            _op_process_desc = "";
            _op_ack_end = false;
        }
    }

    // �жϲ��������Ƿ����
    bool IsOpRunning()
    {
        if (_op_ack_end)
        {
            return false;
        }
            
        //  �����������ڲ����У��𷵻���������
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

    // ���ACKӦ�������־
    void CleanAckEndFlag()
    {
        _op_ack_end = true;
    }

    //  ����ʵ���Ƿ��ڴ�����״̬
    bool InstanceIsDestroy()
    {
        return _is_instance_del;
    }

    //  ���ʵ��������ʶ������CANCEL��MIGRATEʧ�ܵ�ʱ��
    void VmClearDestroyFlag()
    {
        _is_instance_del = false;
    }

    //  ����ʵ��������־
    void VmSetDestroyFlag()
    {
        _is_instance_del = true;
    };

    // ��ȡ��ǰʵ���յ�����Ϣ�ķ�����
    void GetOpSender(MID &sender)
    {
        sender = _op_sender;
    };

    // �ж�ʵ���Ƿ�ȡ������ӳ��
    bool InstanceIsUnSave()
    {
        // �����ɾ����
        if (InstanceIsDestroy())
        {
            return true;
        }

        return _unsave_flag;
    }

    // ���������ΪUNSAVE
    void SetInstanceUnSave()
    {
        _unsave_flag = true;
    }

    // ��������ΪUNSAVE
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

    // ��ӡ��ǰ�������ʵ������
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

        // ������ӡ����ӡ������Ϣ��������Կ������ļ�������ָ���򿪣���Ȼ��Ļ��ӡ̫��
        if (flag)
        {
            cout << " Vm Config: " << vm_cfg.serialize() << endl;
            cout << " Vm Option: " << vm_option.serialize() << endl;
        }
        cout << "**********************************************" << endl;
    }

    // �������������
    void SetVmCfg(VMConfig cfg)
    {
        vm_cfg = cfg;
        _vcpu_num = cfg._vcpu;
        _config_version = cfg._config_version;
        _run_version = -1;
    }

    // ���������ѡ��
    void SetVmOption(VMOption option)
    {
        vm_option = option;
    }

    // ��������ִ�е�actionid
    void SetActionID(string& actionid )
    {
        _op_action_id = actionid;
    }
    
    // ��ȡ�������������
    const uint32 GetProgress()
    {
        return _op_progress;
    }

    
    // ��ȡ�������������
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

    // ��������
    void GetNicUsedRate(vector<NicInfo>& vec_nic_info);
    int32 XenVmStatistics(ResourceStatistics& vm_statistics,
                          const int64& vid);
    int32 KvmVmStatistics(ResourceStatistics& vm_statistics,
                          const int64& vid);
    int32 GetVnicSpeed(string strallports, int64& ispeed);
    
private:
    int64          _instance_id;                   // ����������·���ʵ�����
    string         _instance_name;                 // ��instanceid�����ʵ����
    int            _domain_id;                     // hypervisorά����domain���
    int32          _vcpu_num;                      // �������CPU����
    int32          _config_version;                // ��������ð汾
    int32          _run_version;                   // ������������еİ汾�����ֻ��ˢ�µ�ʱ�����

    time_t         _launch_time;                   // �յ����������ʱ��
    time_t         _boot_time;                     // �ϴ�������ʱ�䣬ϵͳ��������ʱ�䲻�ܶ���1����
    uint64         _domain_run_time;               // domain����ʱ��,��λns
    int64          _refresh_time;                  // ˢ��״̬��ʱ�� ��λ����
    bool           _is_instance_del;               // ��ǰʵ���Ƿ��յ�CANCEL��Ϣ
    Mutex         _refresh_lock;                  // ˢ����
    bool           _unsave_flag;                   // ȡ�����������ı�׼
    uint32         _report_count;
    time_t         _pre_statistics_time;

    ResourceStatistics   _vm_statistics;
    /* ǰһ�ε��������Դʹ����Ϣ
     *   ps: Ŀǰ��Ҫ��������ʹ���ʻ��õ�
     */
    ResourceStatistics  _pre_vm_statistics;

    STATUS StartMu();                              // ����MU

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
        // ���������ʵ���ļ��
        AddKeyMu(_instance_name);        
    };
    int DomainIsExist() const;
    bool CreateCfgParaIsValid();
    bool  StartWDGTimer(const char *const op_fun);
    bool  StopWDGTimer(const char *const op_fun);
    STATUS StartOpCheckWDG();
    STATUS CheckOpTimeOut();
    bool NotifyNetDomIDChg(const char *const  fun_name);
    // ����������Դ�ӿ�
    bool StartNetWork(const char *const  fun_name, BYTE option);
    // �ͷ�������Դ�ӿ�
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


    // �����ⲿ��ʵ����ָ��
    MessageUnit       *_mu;                        // �����ʵ������Ϣ��Ԫ
    VmDriver          *_vm_driver;                 // ����������Ľӿ�ָ��
    StorageOperation  *_storage_instance;           // ������ʵ��ָ��

    // ������Ϣ
    int32          _op_cmd;                        // ��ǰ����������
    uint32         _op_progress;                    // ��ǰ�����Ľ���
    uint32         _op_result;                     // ��ǰ�����Ľ��
    string         _op_process_desc;               // �������ȵ���ϸ����
    uint32         _op_domid;                      // ����ʱ���domid,��Ҫ���ڼ��ĳЩ�����Ƿ����
    string         _op_action_id;                 // ��ǰ�����Ĺ�����ID
    TIMER_ID       _op_check_timer;                // ������鶨ʱ��
    MID            _op_sender;                     // ��ǰ��Ϣִ�еķ�����
    uint32         _wait_time;                     // �����ȴ�ʱ��
    bool           _op_ack_end;                    // ʵ���Ƿ�ɹ�����Ӧ��
    TIMER_ID       _refresh_timer;                 // ״̬ˢ�¶�ʱ��
    int32          _double_op_wait_time;           // REBOOT��RESET���Լ�SHUTOFF��destroy֮���ʱ��
    int32          _double_op_cmd;                 // ����˫������������
    TIMER_ID       _load_timer;                   // ������鶨ʱ��

    // ״̬��Ϣ
    int32           _cpu_usage;                    // �������CPUռ����
    int64           _mem_max;                      // ��������ڴ����ֵ
    int64           _mem_free;                     // ������Ŀ����ڴ��С
    int64           _net_tx;                       // ����������緢������
    int64           _net_rx;                       // ������������������
    int32           _state;                        // �������״̬

    VmInfo          _out_info;                     // ������������Ϣ��

};
#endif // #ifndef HC_VMINSTANCE_H



