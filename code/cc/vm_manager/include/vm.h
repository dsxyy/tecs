#ifndef TECS_CC_VM_H
#define TECS_CC_VM_H
#include "sky.h"
#include "pool_object_sql.h"
#include "vm_messages.h"
namespace zte_tecs
{
/**
 @brief VirtualMachine ��
 @li @b ����˵������
 */ 
class VirtualMachine: public PoolObjectSQL,public Serializable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    VirtualMachine(int64 vid = INVALID_VID) :_vcpu(_config._vcpu),
                                                _tcu(_config._tcu),
                                                _cpu_percent(_config._cpu_percent),
                                                _memory(_config._memory),
                                                _vnc_port(_config._vnc_port),
                                                _serial_port(_config._serial_port),
                                                _enable_serial(_config._enable_serial),
                                                _config_version(_config._config_version),
                                                _region_id(_requirement._affinity_region_id)
    { 
        _vid = vid;
        //_ignore_moni_times = 0;
        Init();
    };
    virtual ~VirtualMachine() {};
    
    //������Ҫ��֯��ѯ���
    //��Դ��dump�ص������е���
    static int Dump(ostringstream& oss, SqlColumn *columns);

    //�����ļ���vm operation������ַ�����ʽ�����ڵ��Բ鿴
    #define VAL2STR(value) case(value): return #value;
    static const char* Errcode2Str(STATUS err)
    {
        switch(err)
        {
            VAL2STR(ERROR_OP_RUNNING);
            VAL2STR(SUCCESS);
            VAL2STR(ERROR);
            VAL2STR(ERROR_TIME_OUT);
            default:return "ERROR";
        }
    };
    
    friend ostream & operator << (ostream& os, const VirtualMachine& vm)
    {
        os << "vid: " << vm._vid << endl;
        os << "host id: " << vm._hid << endl;
        os << "next host id: " << vm._hid_next << endl;
        //os << "user id: " << vm._uid << endl;
        os << "project id: " << vm._project_id << endl;
        os << "vcpu: " << vm._vcpu << endl;
        os << "tcu: " << vm._tcu << endl;
        os << "cpu_percent: " << vm._cpu_percent << endl;

        os.setf(ios::fixed); 
        os << setprecision(0) << 
            "memory: " << BytesToMB(vm._memory) <<" M" << endl;
        //os << setprecision(2) << 
        //   "local disk: " << BytesToGB(vm._localdisk) <<" G" << endl;
        os.unsetf(ios::fixed); 
        
        os << "domain state: " << Enum2Str::InstStateStr(vm._state) << endl;
        os << "lifecycle state: " << Enum2Str::LcmStateStr(vm._lcm_state) << endl;
        os << "last monitored at: " << vm._lastmoniat.serialize() << endl;
        os << "last operation: " << Enum2Str::VmOpStr(vm._lastop) << endl;
        os << "last operated at: " << vm._lastopat.serialize() << endl;
        os << "last operation time out: " << vm._lastoptimeout << endl;
        os << "last operation status: " << Errcode2Str(vm._lastopresult) << endl;
        return os;
    };
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(VirtualMachine);
        WRITE_VALUE(_vid);
        WRITE_VALUE(_hid);
        WRITE_VALUE(_hid_next);
        //WRITE_VALUE(_uid);
        WRITE_VALUE(_project_id);
        WRITE_VALUE(_vcpu);
        WRITE_VALUE(_tcu);
        WRITE_VALUE(_cpu_percent);
        WRITE_VALUE(_memory);
        //WRITE_VALUE(_localdisk);
        WRITE_VALUE(_state);
        WRITE_VALUE(_lcm_state);
        WRITE_VALUE(_lastop);
        WRITE_OBJECT(_lastopat);
        WRITE_VALUE(_lastopresult);
        WRITE_VALUE(_lastopstamp);
        WRITE_VALUE(_dnat_out_ip);
        WRITE_VALUE(_dnat_out_port);
        WRITE_VALUE(_vnc_port);
        WRITE_VALUE(_serial_port);
        WRITE_VALUE(_enable_serial);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VirtualMachine);
        READ_VALUE(_vid);
        READ_VALUE(_hid);
        READ_VALUE(_hid_next);
        //READ_VALUE(_uid);
        READ_VALUE(_project_id);
        READ_VALUE(_vcpu);
        READ_VALUE(_tcu);
        READ_VALUE(_cpu_percent);
        READ_VALUE(_memory);
        //READ_VALUE(_localdisk);
        READ_VALUE(_state);
        READ_VALUE(_lcm_state);
        READ_VALUE(_lastop);
        READ_OBJECT(_lastopat);
        READ_VALUE(_lastopresult);
        READ_VALUE(_lastopstamp);
        READ_VALUE(_dnat_out_ip);
        READ_VALUE(_dnat_out_port);
        READ_VALUE(_vnc_port);
        READ_VALUE(_serial_port);
        READ_VALUE(_enable_serial);
        DESERIALIZE_END();
    };
    
protected:
    virtual int Select(SqlDB * db);
    virtual int Insert(SqlDB * db, string& error_str);
    virtual int Update(SqlDB * db);

    int InsertReplace(SqlDB *db, bool replace);
    virtual int Drop(SqlDB * db);

public:
    enum ColNames
    {
        OID = 0,
        VID = 1,
        HID = 2,
        HID_NEXT = 3,
        PROJECT_ID = 4,
        CONFIG = 5,
        VCPU = 6,
        TCU  = 7,
        CPU_PERCENT = 8,
        MEMORY = 9,
        CPU_RATE = 10,
        MEMORY_RATE = 11,
        NET_TX = 12,
        NET_RX = 13,
        STATE = 14,
        LCM_STATE = 15,
        LAST_MONI_AT = 16,
        LAST_OP = 17,
        LAST_OP_AT = 18,
        LAST_OP_RESULT = 19,
        LAST_OP_TIMEOUT = 20,
        LAST_OP_STAMP = 21,
        DNAT_OUT_IP = 22,
        DNAT_OUT_PORT = 23,
        VNC_PORT = 24,
        LAST_OP_PROCESS = 25,
        LAST_OP_DETAIL = 26,
        CONFIG_VERSION = 27,
        RUN_VERSION = 28,
        SERIAL_PORT = 29,
        ENABLE_SERIAL = 30,
        EXPECTED_STATE = 31,
        REQUIREMENT = 32,
        APPOINT_HOST = 33,
        REGION_LEVEL = 34,
        LOG_STATE = 35,
        LIMIT = 36,
    };

    friend class VirtualMachinePool;
    friend class LifecycleManager;
    friend class Scheduler;
    friend class VMManager;
    friend class VMCancel;
    friend class VMNicPool;
    friend class VmInterface;
    friend class VMHA;
    friend string ConstructWFName(const string &op, const VirtualMachine &vm);
    friend class VmOpHandle;
    friend class ModifyVmCPU;
    friend class ModifyVmMemory;
    friend class ModifyVmMachine;
    friend class ModifyVmMutex;
    friend class ModifyVmWatchdog;
    friend STATUS DeleteVmMachine(MessageUnit *mu, int64 vid, const string &action_id);

    DISALLOW_COPY_AND_ASSIGN(VirtualMachine);
    int SelectCallback(void *nil, SqlColumn *columns);//��ѯ�ص�����
    void Init();
    static const char *table;//����   
    static const char *col_names;//��ѯ����
    static const char *db_bootstrap;//�������SQL
    
    //��ĸ����ֶ�
    int64 _vid;   
    //string _name;
    int64 _hid;
    int64 _hid_next;
    //int64 _uid;
    int64 _project_id;
    VMConfig _config;
    uint32 &_vcpu;
    uint32 &_tcu;
    uint32 &_cpu_percent;    
    int64  &_memory;
    //int64  _localdisk;
    uint32 _cpu_rate;
    uint32 _memory_rate;
    uint32 _net_tx;
    uint32 _net_rx;
    int _state;
    int _log_state; //�߼�״̬��Ŀǰֻ�ж��ᡢ�ⶳ
    int _expected_state;
    int _lcm_state;
    Datetime _lastmoniat;
    int _lastop;
    Datetime _lastopat;
    STATUS _lastopresult;
    int32   _last_op_progress;         // �������ǰ�����Ľ���
    string  _last_op_detail;
    uint32 _lastoptimeout;
    string _lastopstamp;
    string  _dnat_out_ip;
    uint16  _dnat_out_port;
    uint16  &_vnc_port;    /* ����ֱ�Ӳ�ѯ,��vnc portҲ���������������. ����ֶα���� _config._vnc_port ����һ�£��ʶ���Ϊ���� */   
    uint16  &_serial_port;    
    bool    _enable_serial;    
    
    int32   &_config_version; 
    int32   _run_version; 

    VmRequirement   _requirement; /* ����ֶ�û��û��ʹ�õ����ʲ���� */
    string          _appoint_host;

    int64   &_region_id;
    /* �����浽���ݿ��� */
    uint32 _ignore_moni_times;
    T_TimeValue _lastmoni;   /* HC���һ���ϱ�vm״̬��ʱ�䣬ʹ��GetTimeFromPowerOn�ֶθ��� */
    UploadUrl _uploadurl;   // ����Ǳ���������ϴ�ӳ��ʹ�õ�url��ַ�������
    string _save_vm_img_name;    // ����Ǳ��������ʹ�õ����ļ�����, �����

    bool    _is_image_prepared;
    bool    _is_disk_prepared;
public:
    bool CheckOperateAck(const VmOperationAck& ack);
    int RecordOperate(const VmOperationReq &operation);
    int SetValue(const string &key, int64 value);

};

}
#endif

