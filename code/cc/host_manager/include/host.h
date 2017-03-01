/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�host.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��host��������ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�Ԭ����
* ������ڣ�2011��7��25��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/7/25
*     �� �� �ţ�V1.0
*     �� �� �ˣ�Ԭ����
*     �޸����ݣ�����
*******************************************************************************/
#ifndef HM_HOST_H
#define HM_HOST_H

#include "sky.h"
#include "pool_object_sql.h"
#include "msg_host_manager_with_api_method.h"
#include "msg_host_manager_with_host_agent.h"
#include "log_agent.h"

// ���ÿ�������궨��
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)
#endif

using namespace std;
namespace zte_tecs {

class HostFree
{
public:
    HostFree():
    _running_vms(0), _processor_num(0), _tcu_num(0), _all_tcu(0), _free_tcu(0),
    _mem_free(0), _disk_free(0)
    {}

    int32 _running_vms;     // ������ĸ���
    int32 _processor_num;   // ���VCPU�ĸ�������cpu�ĺ���
    int32 _tcu_num;         // ÿ�˵�tcu��Ŀ
    int32 _all_tcu;         // cpu����tcu����Ŀ
    int32 _free_tcu;        // cpuʣ��tcu�ĸ���    
    int64 _mem_free;        // ʣ���ڴ���������λB
    int64 _disk_free;       // ʣ�౾�ش洢��������λB
};

/**
@brief ��������: �����ඨ��
@li @b ����˵������
*/


class Host: public PoolObjectSQL   
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    /* ����������״̬ */
    enum RunState
    {
        INIT            = 0,    // ��ʼ״̬
        OFF_LINE        = 1,    // ����
        ON_LINE         = 2     // ����
    };

    /* cmm��ѯ״̬ */
    enum Power_State
    {
        UNKOUWN          = 0,    // ��ʼ״̬
        BOARD_POWER_ON   = 1,    // ������λ�ϵ�
        BOARD_POWER_OFF  = 2,    // ������λ�µ�
        BOARD_ABSENT     = 3     // ���岻��λ
    };


    /* ������������״̬ */
    enum Power_Operate
    {
        OPERATE_POWER_ON         = 0,   // �ϵ����
        OPERATE_POWER_OFF        = 1    // �µ����
    };

    const string &get_host_name() const
    {
        return _host_name;
    }

    const string &get_ip_address() const
    {
        return _ip_address;
    }

    const string &get_media_address() const
    {
        return _media_address;
    }

    const string &get_iscsiname() const
    {
        return _iscsiname;
    }
    
    void set_ip_address(const string &ip_address)
    {
        _ip_address = ip_address;
    }

    void set_media_address(const string &media_address)
    {
        _media_address = media_address;
    }


    void set_iscsiname(const string &iscsiname)
    {
        _iscsiname = iscsiname;
    }

    RunState get_run_state() const
    {
        return _run_state;
    }

    void set_run_state(RunState run_state)
    {
        _run_state = run_state;
    }

    bool get_is_disabled() const
    {
        return _is_disabled;
    }

    void set_is_disabled(bool is_disabled)
    {
        _is_disabled = is_disabled;
    }

    void set_create_time(time_t create_time)
    {
        _create_time = create_time;
    }

    void set_last_moni_time(time_t last_moni_time)
    {
        _last_moni_time = last_moni_time;
    }

    int64 get_cpu_info_id() const
    {
        return _cpu_info_id;
    }

    const string get_vmm_info() const
    {
        return _vmm_info;
    }

    const string get_hyper_info() const
    {
        if (_vmm_info.find("xen") != string::npos)
        {
            return "xen";
        }
        if (_vmm_info.find("kvm") != string::npos)
        {
            return "kvm";
        }

        SkyAssert(false);
        return "";
    }
    
    void set_vmm_info(const string &vmm_info)
    {
        _vmm_info = vmm_info;
    }   

    const string get_os_info() const
    {
        return _os_info;
    }
    
    void set_os_info(const string &os_info)
    {
        _os_info = os_info;
    }    

    void set_mem_total(int64 mem_total)
    {
        _mem_total = mem_total;
    }

    int64 get_mem_max() const
    {
        return _mem_max;
    }

    void set_mem_max(int64 mem_max)
    {
        _mem_max = mem_max;
    }

    int64 get_disk_total() const
    {
        return _disk_total;
    }

    void set_disk_total(int64 disk_total)
    {
        _disk_total = disk_total;
    }
    
    int64 get_disk_max() const
    {
        return _disk_max;
    }

    void set_disk_max(int64 disk_max)
    {
        _disk_max = disk_max;
    }

    int32 get_cpu_usage_30m() const
    {
        return _cpu_usage_30m;
    }


    map<string, int32> &get_disks()
    {
        return _disks;
    }

    map<uint32, list<string> > &get_alarms()
    {
        return _alarms;
    }

    int64 get_hardware_state() const
    {
        return _hardware_state;
    }

    bool get_mem_state()
    {
        return _hardware_state & 1;
    }
    
    void set_mem_state(bool mem_err)
    {
        if (mem_err)
        {
            _hardware_state |= ((int64)1);
        }
        else
        {
            _hardware_state &= ~((int64)1);
        }
    }

    bool get_oscillator() const
    {
        return _hardware_state & 2;
    }    
    
    void set_oscillator(bool oscillator_err)
    {
        if (oscillator_err)
        {
            _hardware_state |= ((int64)2);
        }
        else
        {
            _hardware_state &= ~((int64)2);
        }
    }

    const string &get_description() const
    {
        return _description;
    }

    void set_description(const string &description)
    {
        _description = description;
    }

    void set_location(const string & location)
    {
        _location = location;
    }

    void set_shelf_type(const string & shelf_type)
    {
        _shelf_type = shelf_type;
    }

    const string &get_location() const
    {
        return _location;
    }

    void set_ssh_key(const string & ssh_key)
    {
        _ssh_key = ssh_key;
    }

    const string &get_ssh_key() const
    {
        return _ssh_key;
    }

    bool get_is_support_hvm() const
    {
        return _is_support_hvm;
    }

    void set_is_support_hvm(bool is_support_hvm)
    {
        _is_support_hvm = is_support_hvm;
    }

    const int32 &get_cpus() const 
    {
        return _cpus;
    }

    void set_cpus(const int32 &cpus)
    {
        _cpus = cpus;
    }
    
    void set_running_time(const int64 &running_time)
    {
        _running_time = running_time;
    }
	
    void   get_board_position(BoardPosition &position)const;

    // ����CPU��Ϣ����������ؽӿ�
    void        CleanCpuUsage();
    void        RecCpuUsage(int32 cpu_usage);
    void        CalcCpuUsage();
    void        SetCpuInfo(const map<string, string> &info);

    void        Dump(HostInfoType &host_info, HostFree &host_free);

    // �����ô�ӡ�ӿ�
    void        Print(HostFree &host_free);
    void        PrintAlarm();

    int32 UpdateDevices(const vector<PciPthDevInfo> &devices);

/*******************************************************************************
* 2. protected section
*******************************************************************************/
protected:    
    Host(int64 oid = -1, const string host_name = "");
    virtual ~Host();

    virtual int32 Select(SqlDB *db);
    virtual int32 Insert(SqlDB *db, string &error_str);
    virtual int32 Update(SqlDB *db);
    virtual int32 Drop(SqlDB *db);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:
    friend class HostPool;
    friend class HostManager;
    
    /* ����Ա�����ݿ���е���λ�� */
    enum ColNames
    {
        OID             = 0,
        HOST_NAME       = 1,
        IP_ADDRESS      = 2,
        RUN_STATE       = 3,
        IS_DISABLED     = 4,
        CREATE_TIME     = 5,
        RUNNING_TIME    = 6,
        CPU_INFO_ID     = 7,
        VMM_INFO        = 8,
        OS_INFO         = 9,
        MEM_TOTAL       = 10,
        MEM_MAX         = 11,
        DISK_TOTAL      = 12,
        DISK_MAX        = 13,
        CPU_USAGE_1M    = 14,
        CPU_USAGE_5M    = 15,
        CPU_USAGE_30M   = 16,
        HARDWARE_STATE  = 17,
        DESCRIPTION     = 18,
        LOCATION        = 19,
        IS_SUUPORT_HVM  = 20,
        SSH_KEY         = 21,        
        SHELF_TYPE      = 22,
        ISCSI_NAME      = 23,
        MEDIA_ADDRESS   = 24,
        CPUS            = 25,
        LIMIT           = 26
    };

    string          _host_name;             // ��������
    string          _ip_address;            // ������IP��ַ    
    string          _media_address;         // ������ý�����ַ
    string          _iscsiname;             // ����iscsiname
    RunState        _run_state;             // ����������״̬
    bool            _is_disabled;           // �����Ƿ����ά��ģʽ
    time_t          _create_time;           // ��������Ĵ���ʱ�� 
    time_t          _last_moni_time;        // �����ı���ʱ�䣬���ֶβ������ݿ�
    int64           _running_time;          // ��������ʱ��

    int64           _cpu_info_id;           // ������CPU������Ϣ
    string          _vmm_info;              // ������Hypervisor��Ϣ
    string          _os_info;               // �����Ĳ���ϵͳ��Ϣ

    int64           _mem_total;             // �������ڴ��ܸ��ռ䣬��λB
    int64           _mem_max;               // �������ڴ������ÿ��пռ䣬��λB
    int64           _disk_total;            // �������ش洢�ܿռ䣬��λB
    int64           _disk_max;              // �������ش洢�����ÿռ䣬��λB

    int32           _cpu_usage_1m;          // ���һ��ͳ�Ƶ�1��������CPUʹ����
    int32           _cpu_usage_5m;          // ���һ��ͳ�Ƶ�5��������CPUʹ����
    int32           _cpu_usage_30m;         // ���һ��ͳ�Ƶ�30��������CPUʹ����

    int64           _hardware_state;        // ����Ӳ���Ƿ���ϣ�0�޹���

    string          _description;           // ������������Ϣ
    string          _location;              // ������λ����Ϣ
    bool            _is_support_hvm;        // �����Ƿ�֧�����⻯
    string          _ssh_key;               // _ssh_key
    string          _shelf_type;            // �������ڵĻ�������

    // �����ֶβ���host_pool���ݱ�
    int32           _cpu_usage_5s_rec[12];  // ���1������12��5�����ȵ�CPUʹ����
    int32           _cpu_usage_5s_num;      // �Ѽ�¼�ڼ���5s���ȵ�CPU
    int32           _cpu_usage_1m_rec[30];  // ���30������30��1��������CPU
    int32           _cpu_usage_1m_num;      // �Ѽ�¼�ڼ���1m���ȵ�CPU    
    int32           _cpus;                  // �������ڲ��������cpu����

    map<string, int32>          _disks;     // ������Ӳ��״̬

    map<uint32, list<string> >  _alarms;    // �����ĸ澯��Ϣ

    // ���ݿ���ز��������ֶ�
    static const char *_table;              /* ���ݿ��д��Host����ı��� */
    static const char *_db_names;           /* ����Host��Դ��ʱ�õ����ֶ��� */
    static       SqlDB *_db;
    
    static int32 BootStrap(SqlDB *db);

    int32 InsertReplace(SqlDB *db, bool replace);
    int32 SelectCallback(void *nil, SqlColumn * columns);
    
    DISALLOW_COPY_AND_ASSIGN(Host);
};

}  /* end namespace zte_tecs */

#endif /* end HM_HOST_H */

