/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：host.h
* 文件标识：见配置管理计划书
* 内容摘要：host类的声明文件
* 当前版本：1.0
* 作    者：袁进坤
* 完成日期：2011年7月25日
*
* 修改记录1：
*     修改日期：2011/7/25
*     版 本 号：V1.0
*     修 改 人：袁进坤
*     修改内容：创建
*******************************************************************************/
#ifndef HM_HOST_H
#define HM_HOST_H

#include "sky.h"
#include "pool_object_sql.h"
#include "msg_host_manager_with_api_method.h"
#include "msg_host_manager_with_host_agent.h"
#include "log_agent.h"

// 禁用拷贝构造宏定义
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

    int32 _running_vms;     // 虚拟机的个数
    int32 _processor_num;   // 最大VCPU的个数就是cpu的核数
    int32 _tcu_num;         // 每核的tcu数目
    int32 _all_tcu;         // cpu所有tcu的数目
    int32 _free_tcu;        // cpu剩余tcu的个数    
    int64 _mem_free;        // 剩余内存数量，单位B
    int64 _disk_free;       // 剩余本地存储数量，单位B
};

/**
@brief 功能描述: 主机类定义
@li @b 其它说明：无
*/


class Host: public PoolObjectSQL   
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    /* 主机的运行状态 */
    enum RunState
    {
        INIT            = 0,    // 初始状态
        OFF_LINE        = 1,    // 离线
        ON_LINE         = 2     // 在线
    };

    /* cmm查询状态 */
    enum Power_State
    {
        UNKOUWN          = 0,    // 初始状态
        BOARD_POWER_ON   = 1,    // 单板在位上电
        BOARD_POWER_OFF  = 2,    // 单板在位下电
        BOARD_ABSENT     = 3     // 单板不在位
    };


    /* 主机操作期望状态 */
    enum Power_Operate
    {
        OPERATE_POWER_ON         = 0,   // 上电操作
        OPERATE_POWER_OFF        = 1    // 下电操作
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

    // 物理CPU信息及利用率相关接口
    void        CleanCpuUsage();
    void        RecCpuUsage(int32 cpu_usage);
    void        CalcCpuUsage();
    void        SetCpuInfo(const map<string, string> &info);

    void        Dump(HostInfoType &host_info, HostFree &host_free);

    // 调试用打印接口
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
    
    /* 各成员在数据库表中的列位置 */
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

    string          _host_name;             // 主机名称
    string          _ip_address;            // 主机的IP地址    
    string          _media_address;         // 主机的媒体面地址
    string          _iscsiname;             // 主机iscsiname
    RunState        _run_state;             // 主机的运行状态
    bool            _is_disabled;           // 主机是否进入维护模式
    time_t          _create_time;           // 主机对象的创建时间 
    time_t          _last_moni_time;        // 主机的保活时间，此字段不入数据库
    int64           _running_time;          // 主机在线时间

    int64           _cpu_info_id;           // 主机的CPU物理信息
    string          _vmm_info;              // 主机的Hypervisor信息
    string          _os_info;               // 主机的操作系统信息

    int64           _mem_total;             // 主机的内存总个空间，单位B
    int64           _mem_max;               // 主机的内存最大可用空闲空间，单位B
    int64           _disk_total;            // 主机本地存储总空间，单位B
    int64           _disk_max;              // 主机本地存储最大可用空间，单位B

    int32           _cpu_usage_1m;          // 最近一次统计的1分钟粒度CPU使用率
    int32           _cpu_usage_5m;          // 最近一次统计的5分钟粒度CPU使用率
    int32           _cpu_usage_30m;         // 最近一次统计的30分钟粒度CPU使用率

    int64           _hardware_state;        // 主机硬件是否故障，0无故障

    string          _description;           // 主机的描述信息
    string          _location;              // 主机的位置信息
    bool            _is_support_hvm;        // 主机是否支持虚拟化
    string          _ssh_key;               // _ssh_key
    string          _shelf_type;            // 主机所在的机框类型

    // 以下字段不入host_pool数据表
    int32           _cpu_usage_5s_rec[12];  // 存放1分钟内12次5秒粒度的CPU使用率
    int32           _cpu_usage_5s_num;      // 已记录第几个5s粒度的CPU
    int32           _cpu_usage_1m_rec[30];  // 存放30分钟内30次1分钟粒度CPU
    int32           _cpu_usage_1m_num;      // 已记录第几个1m粒度的CPU    
    int32           _cpus;                  // 最大可用于部署虚拟机cpu核数

    map<string, int32>          _disks;     // 主机的硬盘状态

    map<uint32, list<string> >  _alarms;    // 主机的告警信息

    // 数据库相关操作属性字段
    static const char *_table;              /* 数据库中存放Host对象的表名 */
    static const char *_db_names;           /* 查找Host资源表时用到的字段名 */
    static       SqlDB *_db;
    
    static int32 BootStrap(SqlDB *db);

    int32 InsertReplace(SqlDB *db, bool replace);
    int32 SelectCallback(void *nil, SqlColumn * columns);
    
    DISALLOW_COPY_AND_ASSIGN(Host);
};

}  /* end namespace zte_tecs */

#endif /* end HM_HOST_H */

