/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：host.cpp
* 文件标识：
* 内容摘要：host类的定义文件
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
#include "host.h"
#include "host_cpuinfo.h"
#include "config_tcu.h"
#include "db_config.h"
#include "host_pool.h" 
#include "host_device.h"

namespace zte_tecs {

/* 主机使用的表名称 */
const char *Host::_table = "host_pool";

/* 主机中的表字段 */
const char *Host::_db_names = 
    "oid, name, ip_address, run_state, enabled, "
    "create_time,running_time, cpu_info_id, vmm_info, os_info, mem_total, mem_max, "
    "disk_total, disk_max, cpu_usage_1m, cpu_usage_5m, cpu_usage_30m, "
    "hardware_state, description, location, is_support_hvm, ssh_key, shelf_type, "
    "iscsiname, media_address, cpus ";

SqlDB *Host::_db = NULL;

/******************************************************************************/ 
int32 Host::BootStrap(SqlDB *db)
{
    if (NULL == _db)
    {
        _db = db;
    }
   
    return 0;
}

/******************************************************************************/ 
Host::Host(int64 oid, const string host_name):
    PoolObjectSQL(oid), _host_name(host_name), _disks(), 
    _alarms()
{
    
    _run_state = INIT;
    _is_disabled = false;
    _create_time = time(0);
    _last_moni_time = time(0);
    _running_time = 0;
    _cpu_info_id = HostCpuInfo::INVALID_CPU_INFO_ID;
    _mem_total = 0;
    _mem_max = 0;
    _disk_total = 0;
    _disk_max = 0;
    memset(_cpu_usage_5s_rec, 0, sizeof(_cpu_usage_5s_rec));
    _cpu_usage_5s_num = 0;
    memset(_cpu_usage_1m_rec, 0, sizeof(_cpu_usage_1m_rec));
    _cpu_usage_1m_num = 0;
    _cpu_usage_1m = 0;
    _cpu_usage_5m = 0;
    _cpu_usage_30m = 0;
    _hardware_state = 0;
    _is_support_hvm = false;
    _shelf_type = "";
    _description = "";
    _location = "";
    _cpus = 0;
}

/******************************************************************************/ 
Host::~Host()
{
}

/******************************************************************************/
int32 Host::SelectCallback(void *nil, SqlColumn *columns)
{
    if ((NULL == columns) || (LIMIT != columns->get_column_num()))
    {
        return -1;
    }
    int    tmp_int =0 ;
    int64  tmp_int64 = 0;

    columns->GetValue(OID, tmp_int64);
    if (_oid != tmp_int64)
    {
        SkyAssert(_oid == columns->GetValueInt64(OID));
        return -1;
    }
 
    columns->GetValue(HOST_NAME, _host_name);
    columns->GetValue(IP_ADDRESS, _ip_address);    
    
    columns->GetValue(RUN_STATE, tmp_int);
    _run_state = static_cast<RunState>(tmp_int);
    
    columns->GetValue(IS_DISABLED, tmp_int);
    _is_disabled = static_cast<bool>(tmp_int);

    tmp_int64 = 0;
    columns->GetValue(CREATE_TIME, tmp_int64);
    _create_time    = static_cast<time_t>(tmp_int64);
    
    tmp_int64 = 0;
    columns->GetValue(RUNNING_TIME, tmp_int64);
    _running_time    = static_cast<time_t>(tmp_int64);
	
    columns->GetValue(CPU_INFO_ID, _cpu_info_id);
    columns->GetValue(VMM_INFO,   _vmm_info);
    columns->GetValue(OS_INFO,    _os_info);
    columns->GetValue(MEM_TOTAL,  _mem_total);
    columns->GetValue(MEM_MAX,    _mem_max);
    columns->GetValue(DISK_TOTAL, _disk_total);
    columns->GetValue(DISK_MAX,   _disk_max);
    columns->GetValue(CPU_USAGE_1M, _cpu_usage_1m);
    columns->GetValue(CPU_USAGE_5M, _cpu_usage_5m);
    columns->GetValue(CPU_USAGE_30M, _cpu_usage_30m);
    columns->GetValue(HARDWARE_STATE, _hardware_state);
    columns->GetValue(DESCRIPTION, _description);
    columns->GetValue(LOCATION,    _location);
    columns->GetValue(IS_SUUPORT_HVM, tmp_int);
    _is_support_hvm = static_cast<bool>(tmp_int);
    columns->GetValue(SSH_KEY, _ssh_key);
    columns->GetValue(SHELF_TYPE, _shelf_type);
    columns->GetValue(ISCSI_NAME, _iscsiname);    
    columns->GetValue(MEDIA_ADDRESS, _media_address);
    columns->GetValue(CPUS, _cpus);

    
    return 0;
}

/******************************************************************************/ 
int32 Host::Select(SqlDB *db)
{
    ostringstream   oss;
    int             rc;
    
    SetCallback(static_cast<Callbackable::Callback>(&Host::SelectCallback));
    
    // 避免使用SELECT *，应该明确要查询的列，这样在表结构升级时才不会有问题
    oss << "SELECT " << _db_names << " FROM " << _table << " WHERE oid = " 
        << _oid;
    
    rc = db->Execute(oss, this);
    
    UnsetCallback();
    
    if (rc != 0)
    {
        return rc;
    }

    return 0;
}

/******************************************************************************/
int32 Host::Insert(SqlDB *db, string &error_str)
{
    int32 rc;

    rc = InsertReplace(db, false);

    if ( rc != 0 )
    {
        error_str = "Error inserting Host in DB.";
        return rc;
    }

    return 0;
}

/******************************************************************************/
int32 Host::Update(SqlDB *db)
{
    int32 rc;

    rc = InsertReplace(db, true);

    if (rc != 0)
    {
        return rc;
    }

    return 0;;
}

/******************************************************************************/
int32 Host::InsertReplace(SqlDB *db, bool replace)
{
    ostringstream   oss;

    SqlCommand sql(db,  _table);
    sql.Add("oid" ,  _oid);
    sql.Add("name" ,  _host_name);
    sql.Add("ip_address" ,  _ip_address);
    sql.Add("run_state" ,  _run_state);
    sql.Add("enabled" ,  _is_disabled);
    sql.Add("create_time" ,  _create_time);
    sql.Add("running_time" ,  _running_time);	
    sql.Add("cpu_info_id" ,  _cpu_info_id);
    sql.Add("vmm_info" ,  _vmm_info);
    sql.Add("os_info" ,  _os_info);
    sql.Add("mem_total" ,  _mem_total);
    sql.Add("mem_max" , _mem_max );
    sql.Add("disk_total" , _disk_total );
    sql.Add("disk_max" ,  _disk_max);
    sql.Add("cpu_usage_1m" , _cpu_usage_1m );
    sql.Add("cpu_usage_5m" ,  _cpu_usage_5m);
    sql.Add("cpu_usage_30m" ,  _cpu_usage_30m);
    sql.Add("hardware_state" , _hardware_state );
    sql.Add("description" , _description);
    sql.Add("location" ,  _location);
    sql.Add("is_support_hvm" , _is_support_hvm );
    sql.Add("ssh_key" ,   _ssh_key);
    sql.Add("shelf_type" ,  _shelf_type);
    sql.Add("iscsiname" ,  _iscsiname);
    sql.Add("media_address" ,  _media_address);
    sql.Add("cpus" ,  _cpus);
    

    
    if (replace)
    {
        ostringstream where;
        where << " WHERE oid = " << _oid;
        return sql.Update(where.str());
    }
    else
    {
        return sql.Insert();     
    }
}

/******************************************************************************/
int32 Host::Drop(SqlDB *db)
{
    // 从数据库中删除主机对象
    ostringstream   oss;
    int32           rc;
    
    oss << "DELETE FROM " << _table << " WHERE oid = " << _oid ;
    
    rc = db->Execute(oss);
    
    if ( 0 == rc )
    {
        set_valid(false);
    }

    // 尝试删除cpu_info_id
    HostCpuInfo *host_cpu_info = HostCpuInfo::GetInstance();
    if (NULL != host_cpu_info)
    {
        host_cpu_info->Drop(_cpu_info_id);
    }
    
    return rc;
}

/******************************************************************************/
void Host::CleanCpuUsage()
{
    _cpu_usage_1m = 0;
    _cpu_usage_5m = 0;
    _cpu_usage_30m = 0;
    _cpu_usage_5s_num = 0;
    _cpu_usage_1m_num = 0;
    memset(_cpu_usage_5s_rec, 0, sizeof(_cpu_usage_5s_rec));
    memset(_cpu_usage_1m_rec, 0, sizeof(_cpu_usage_1m_rec));
}

/******************************************************************************/
void Host::RecCpuUsage(int32 cpu_usage)
{
    if (_cpu_usage_5s_num < 12)
    {
        SkyAssert(cpu_usage <= 1000);
        _cpu_usage_5s_rec[_cpu_usage_5s_num++] = cpu_usage;
    }
}

/******************************************************************************/
void Host::CalcCpuUsage()
{
    SkyAssert(_cpu_usage_5s_num <= 12);
    SkyAssert(_cpu_usage_1m_num <= 30);

    int32 total = 0;
    for (int32 i = 0; i < _cpu_usage_5s_num; ++i)
    {
        SkyAssert(_cpu_usage_5s_rec[i] <= 1000);
        total += _cpu_usage_5s_rec[i];        
    }

    if (_cpu_usage_5s_num != 0)
    {
        _cpu_usage_1m = total / _cpu_usage_5s_num;
    }
    else
    {
        _cpu_usage_1m = 0;
    }

    memset(_cpu_usage_5s_rec, 0, sizeof(_cpu_usage_5s_rec));
    _cpu_usage_5s_num = 0;

    if (_cpu_usage_1m_num < 30)
    {
        _cpu_usage_1m_rec[_cpu_usage_1m_num++] = _cpu_usage_1m;
    }

    if (_cpu_usage_1m_num % 5 == 0)
    {
        total = 0;
        for (int32 i= _cpu_usage_1m_num - 5; i < _cpu_usage_1m_num; ++i)
        {
            SkyAssert(_cpu_usage_1m_rec[i] <= 1000);
            total += _cpu_usage_1m_rec[i];   
        }
        _cpu_usage_5m = total / 5;
    }

    if (_cpu_usage_1m_num == 30)
    {
        total = 0;
        for (int32 i = 0; i < 30; ++i)
        {
            SkyAssert(_cpu_usage_1m_rec[i] <= 1000);
            total += _cpu_usage_1m_rec[i];        
        }
        _cpu_usage_30m = total / 30;
        memset(_cpu_usage_1m_rec, 0, sizeof(_cpu_usage_1m_rec));
        _cpu_usage_1m_num = 0;        
    }
}

/******************************************************************************/ 
void Host::SetCpuInfo(const map<string, string> &info)
{
    _cpu_info_id = HostCpuInfo::INVALID_CPU_INFO_ID;

    // 根据cpu_info字符串信息获取cpu_info_id
    HostCpuInfo *host_cpu_info = HostCpuInfo::GetInstance();
    if (NULL == host_cpu_info)
    {
        return;
    }

    int64 cpu_info_id = host_cpu_info->GetIdByInfo(info);
    if (HostCpuInfo::INVALID_CPU_INFO_ID == cpu_info_id)
    {
        // 如果没有相应的cpu_info，则插入到host_cpuinfo表中生成ID
        cpu_info_id = host_cpu_info->Insert(info);
        if (HostCpuInfo::INVALID_CPU_INFO_ID == cpu_info_id)
        {
            return;
        }
    }

    _cpu_info_id = cpu_info_id;
}

/******************************************************************************/  
void Host::get_board_position(BoardPosition &position) const
{
    string location = "<BoardPosition>\n"+_location+"</BoardPosition>";
    position.deserialize(location);
}

static bool comp(const PciPthDevInfo &device1, const PciPthDevInfo &device2)
{
    return device1._type < device2._type;
}

/******************************************************************************/  
int32 Host::UpdateDevices(const vector<PciPthDevInfo> &devices)
{
    vector<PciPthDevInfo> diff;
    vector<PciPthDevInfo> devices_in_db;

    HostDevicePool *host_device_pool = HostDevicePool::GetInstance(_db);
    SkyAssert(host_device_pool);

    host_device_pool->GetHostDevices(devices_in_db, _oid);

    set_difference(devices_in_db.begin(), devices_in_db.end(),
                        devices.begin(), devices.end(),
                        back_inserter(diff), comp);
    host_device_pool->DropHostDevices(diff, _oid); //删除已经没有的设备

    diff.clear();
    set_difference(devices.begin(), devices.end(),
                        devices_in_db.begin(), devices_in_db.end(),
                        back_inserter(diff), comp);
    host_device_pool->InsertHostDevices(diff, _oid); //增加新设备
    
    return 0;
}


/******************************************************************************/
void Host::Dump(HostInfoType &host_info, HostFree &host_free)
{
    host_info._oid = _oid;
    host_info._host_name = _host_name;
    host_info._ip_address = _ip_address;
    host_info._run_state = _run_state;
    host_info._is_disabled = _is_disabled;
    host_info._create_time = _create_time;
    host_info._running_time = _running_time;	
    host_info._cpu_info_id = _cpu_info_id;
    host_info._vmm_info = _vmm_info;
    host_info._os_info = _os_info;
    host_info._running_vms = host_free._running_vms;
    host_info._tcu_max = host_free._all_tcu;
    host_info._tcu_free = host_free._free_tcu;
    host_info._mem_total = _mem_total;
    host_info._mem_max = _mem_max;
    host_info._mem_free = host_free._mem_free;
    host_info._disk_total = _disk_total;
    host_info._disk_max = _disk_max;
    host_info._disk_free = host_free._disk_free;
    host_info._cpu_usage_1m = _cpu_usage_1m;
    host_info._cpu_usage_5m = _cpu_usage_5m;
    host_info._cpu_usage_30m = _cpu_usage_30m;
    host_info._hardware_state = _hardware_state;
    host_info._description = _description;
    host_info._location = _location;   
    host_info._is_support_hvm = _is_support_hvm;
    host_info._cpus = _cpus;

    
}

/******************************************************************************/
void Host::Print(HostFree &host_free)
{
#define RUN_STR(state) \
    ((state == INIT       ) ? "INIT"        : (\
    ((state == OFF_LINE   ) ? "OFF_LINE"    : (\
    ((state == ON_LINE    ) ? "ON_LINE"     : "UNKNOWN")))))

    Datetime dt_create_time(_create_time);
    Datetime dt_last_moni_time(_last_moni_time);

    cout << 
        "---- HOST INFO --------------------------------------------" << endl <<
        "oid               = " << GetOID()                  << endl << 
        "host_name         = " << _host_name                << endl <<
        "ip_address        = " << _ip_address               << endl <<
        "media_address     = " << _media_address            << endl <<
        "iscsiname         = " << _iscsiname                << endl <<
        "run_state         = " << RUN_STR(_run_state)       << endl <<
        "enabled           = " << _is_disabled              << endl <<
        "create_time       = " << dt_create_time.tostr()    << endl <<
        "last_moni_time    = " << dt_last_moni_time.tostr() << endl <<
        "running_time      = " << _running_time <<"s"       << endl <<		
        "cpu_info_id       = " << _cpu_info_id              << endl <<
        "vmm_info          = " << _vmm_info                 << endl <<
        "os_info           = " << _os_info                  << endl <<
        "running_vms       = " << host_free._running_vms    << endl <<
        "tcu_max         = " << host_free._all_tcu      << endl <<
        "tcu_free        = " << host_free._free_tcu     << endl;
    
    cout.setf(ios::fixed); 
    cout << setprecision(0) << 
        "mem_total         = " << BytesToMB(_mem_total)     <<" M" << endl <<
        "mem_max           = " << BytesToMB(_mem_max)       <<" M" << endl <<
        "mem_free          = " << BytesToMB(host_free._mem_free) <<" M" << endl;
    cout << setprecision(2) << 
        "disk_total        = " << BytesToGB(_disk_total)    <<" G" << endl <<
        "disk_max          = " << BytesToGB(_disk_max)      <<" G" << endl <<
        "disk_free         = " << BytesToGB(host_free._disk_free) <<" G" << endl;
    cout.unsetf(ios::fixed); 
    
    cout << 
        "cpu_usage_1m      = " << _cpu_usage_1m             << endl <<
        "cpu_usage_5m      = " << _cpu_usage_5m             << endl <<
        "cpu_usage_30m     = " << _cpu_usage_30m            << endl <<
        "cpu_usage_5s_num  = " << _cpu_usage_5s_num         << endl <<
        "cpu_usage_1m_num  = " << _cpu_usage_1m_num         << endl <<
        "hardware_state    = " << _hardware_state           << endl <<        
        "description       = " << _description              << endl <<        
        "location          = " << _location                 << endl <<                        
        "is_support_hvm    = " << _is_support_hvm           << endl << 
        "cpus              = " << _cpus                     << endl << 
        "ssh_key    = "        << _ssh_key                  << endl;
}
/******************************************************************************/
void Host::PrintAlarm()
{
    cout << 
        "---- HOST ALARM INFO --------------------------------------" << endl;        
    cout << "host_name         = " << _host_name << endl;

    map<uint32, list<string> >::iterator it_code = _alarms.begin();
    for (; it_code != _alarms.end(); ++it_code)
    {
        cout << "alarm code        = " << it_code->first << endl;
        list<string> &info_list = it_code->second;
        list<string>::iterator it_info = info_list.begin();
        int index = 1;
        for (; it_info != info_list.end(); ++it_info, ++index)
        {
            cout << index << ". " << *it_info << endl;
        }
        cout << endl;
    }
}

} // namespace zte_tecs

