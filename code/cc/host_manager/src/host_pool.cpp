/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：host_pool.cpp
* 文件标识：
* 内容摘要：host_pool类的定义文件
* 当前版本：1.0
* 作    者：袁进坤
* 完成日期：2011年7月25日
*
* 修改记录1：
*     修改日期：2011/8/25
*     版 本 号：V1.0
*     修 改 人：袁进坤
*     修改内容：创建
*******************************************************************************/
#include "host_pool.h"
#include "host_cpuinfo.h"
#include "vm_manager.h"
#include "host.h"

namespace zte_tecs {

const char *HostPool::_table_host_in_use =
    "host_pool_in_use";
    
const char *HostPool::_table_host_in_use_with_cpuinfo = 
    "host_pool_in_use_with_cpuinfo";

const char *HostPool::_host_in_use_stat =
    "host_in_use_stat";


HostPool *HostPool::_instance = NULL;

/******************************************************************************/
void HostPool::BootStrap(SqlDB *db)
{
    Host::BootStrap(db);
}

/******************************************************************************/  
int32 HostPool::InitCallback(void *nil, SqlColumn *columns)
{
    if ((NULL == columns) || (2 != columns->get_column_num()))
    {
        return -1;
    }
    
    int64 oid  = columns->GetValueInt64(0); 
    string name = columns->GetValueText(1); 
    MutexLock lock(_mutex);
    _host_names.insert(make_pair(name, oid));    
    return 0;
}

/******************************************************************************/  
HostPool::HostPool(SqlDB *db):PoolSQL(db, Host::_table), _host_names()
{
    _mutex.SetName("_host_pool_mutex");
    _mutex.Init();

    ostringstream   sql;
    
    SetCallback(static_cast<Callbackable::Callback>(&HostPool::InitCallback));
    
    sql << "SELECT oid, name FROM " << Host::_table;
    
    db->Execute(sql, this);
    
    UnsetCallback();
}

/******************************************************************************/  
int64 HostPool::Allocate(int64 *oid, const string &host_name, int64 cpu_info_id,
                         string &error_str)
{
    Host *host = new Host(-1, host_name);
    host->_cpu_info_id = cpu_info_id;
    *oid = PoolSQL::Allocate(host, error_str);

    if (*oid != -1)
    {
        MutexLock lock(_mutex);
        _host_names.insert(make_pair(host_name, *oid));
    }        
   
    return *oid;
}

/******************************************************************************/  
Host* HostPool::Get(int64 oid, bool lock)
{
    return static_cast<Host*>(PoolSQL::Get(oid, lock));
}

/******************************************************************************/  
Host* HostPool::Get(const string &host_name, bool lock)
{
    map<string, int64>::iterator it;
    _mutex.Lock();
    it = _host_names.find(host_name);
    _mutex.Unlock();
    if (it != _host_names.end())
    {
        return static_cast<Host*>(PoolSQL::Get(it->second, lock));
    }

    return NULL;
}

/******************************************************************************/  
int32 HostPool::Update(Host *host)
{
    return host->Update(_db);
}

/******************************************************************************/  
int32 HostPool::Drop(Host *host)
{
    int32 rc = PoolSQL::Drop(host);

    if (0 == rc)
    {
        MutexLock lock(_mutex);    
        _host_names.erase(host->_host_name);
    }

    return rc;
}

/******************************************************************************/  
int32 HostPool::Count()
{
    int64 host_count = 0;
    SqlCallbackable sqlcb(_db);
    sqlcb.SelectColumn(Host::_table,"COUNT(*)"," oid != -1",host_count);
    return host_count;
}
/******************************************************************************/
int32 HostPool::Match(vector<int64> &oids, const string &where)
{
    ostringstream new_where;
    new_where << " run_state = " << Host::ON_LINE
        << " AND enabled = " << 0 
        << " AND hardware_state = " << 0 
        << " AND " << where << " ";        
    return PoolSQL::Search(oids, _table_host_in_use_with_cpuinfo, new_where.str());
}

/******************************************************************************/
int32 HostPool::MatchSDevice(vector<int64> &oids, const vector<VmDeviceConfig> &devices)
{
    ostringstream oss;
    vector<VmDeviceConfig>::const_iterator it;

    //oss << "select hid from host_device_free";//这一句也可以保证如果devices为空，那给出所有的hid
    oss << "1=1";
    for (it = devices.begin(); it != devices.end(); it ++)
    {
        oss << " INTERSECT SELECT hid FROM host_device_free WHERE "
            << " type = " << it->_type << " and free >= " << it->_count;
    }

    return SelectColumn("host_device_free","hid", oss.str(),oids);
}

/******************************************************************************/
int32 HostPool::SortHost(vector<int64> &oids, const string &orderby)
{
    //2个入参都不允许为空
    SkyAssert(oids.size() && orderby.size());
    ostringstream oss;

    oss << "oid in (";

    vector<int64>::iterator it;
    for (it = oids.begin(); it != oids.end(); it ++)
    {
        oss << *it;
        if ((it+1) != oids.end())
        {
            oss << ", ";
        }
    }
    oss << ") " << orderby;

    oids.clear();
    /* select oid, mem_free from host_pool_in_use_with_cpuinfo where oid in (12,11,3) */
    return SelectColumn("host_pool_in_use_with_cpuinfo","oid", oss.str(),oids);
}

/******************************************************************************/
int32 HostPool::List(vector<int64> &oids, 
                     int32 duration)
{
    map<string, int64>::iterator it;
    time_t cur_time = time(0);

    for (it = _host_names.begin(); it != _host_names.end(); ++it)
    {
        Host *host = Get(it->second, false);
        if ((NULL != host) && 
            (cur_time - host->_last_moni_time > duration))
        {
            oids.push_back(it->second);
        }        
    }
    return 0;
}

/******************************************************************************/
int32 HostPool::SelectFreeCallback(void *nil, SqlColumn *columns)
{
    if ((NULL == columns) || (7 != columns->get_column_num()))
    {
        return -1;
    }

    HostFree *host_free = static_cast<HostFree *>(nil);
    host_free->_running_vms = columns->GetValueInt(0);
    host_free->_processor_num = columns->GetValueInt(1);
    host_free->_tcu_num = columns->GetValueInt(2);
    host_free->_all_tcu = columns->GetValueInt(3);
    host_free->_free_tcu = columns->GetValueInt(4);
    host_free->_mem_free = columns->GetValueInt64(5);
    host_free->_disk_free = columns->GetValueInt64(6);    

    return 0;
}

/******************************************************************************/ 
int32 HostPool::GetHostFree(int64 oid, HostFree &host_free)
{
    ostringstream   oss;
    int             rc;

    SetCallback(static_cast<Callbackable::Callback>
        (&HostPool::SelectFreeCallback), static_cast<void *>(&host_free));

    // 避免使用SELECT *，应该明确要查询的列，这样在表结构升级时才不会有问题
    oss << "SELECT " 
        << "running_vms, cpus, tcu_num, all_tcu, all_free_tcu, mem_free, disk_free" 
        << " FROM " << _table_host_in_use << " WHERE oid = " << oid;

    rc = _db->Execute(oss, this);

    UnsetCallback();

    return rc;
}

/******************************************************************************/
int32 HostPool::GetHostRunningVms(Host *host)
{
    HostFree host_free;
    GetHostFree(host->GetOID(), host_free);
    return host_free._running_vms;
}

/******************************************************************************/  
const string HostPool::GetHostNameById(int64 oid)
{
    string host_name;

    Host *host = Get(oid, true);
    if (NULL == host)
    {
        return host_name;
    }
    host_name = host->get_host_name();
    host->UnLock();
    return host_name;
}


const string HostPool::GetIscsinameById(int64 oid)
{
    string iscsiname;

    Host *host = Get(oid, true);
    if (NULL == host)
    {
        return iscsiname;
    }
    iscsiname = host->get_iscsiname();
    host->UnLock();
    return iscsiname;
}


/******************************************************************************/  
int64 HostPool::GetHostIdByName(const string &host_name)
{
    int64 host_id;

    Host *host = Get(host_name, true);
    if (NULL == host)
    {
        return -1;
    }
    host_id = host->GetOID();
    host->UnLock();
    return host_id;
}

/******************************************************************************/
string HostPool::GetHostSshkeyById(int64 oid)
{
    string ssh_key;
    Host *host = Get(oid, true);
    if (NULL == host)
    {
        return "";
    }
    ssh_key = host->get_ssh_key();
    host->UnLock();
    return ssh_key;
}

/******************************************************************************/
void HostPool::GetAllHostId(vector<int64> &oids)
{
    map<string, int64>::iterator it;
    for (it = _host_names.begin(); it != _host_names.end(); ++it)
    {
        oids.push_back(it->second);
    }

    return;
}

/******************************************************************************/ 
const string HostPool::GetHostIpById(int64 oid)
{
    string ip;

    Host *host = Get(oid, true);
    if (NULL == host)
    {
        return ip;
    }
    ip = host->get_ip_address();
    host->UnLock();
    return ip;
}

/******************************************************************************/ 
const string HostPool::GetMediaAddressById(int64 oid)
{
    string media_address;

    Host *host = Get(oid, true);
    if (NULL == host)
    {
        return media_address;
    }
    media_address = host->get_media_address();
    host->UnLock();
    return media_address;
}

int32 HostPool::GetReportClusterInfo(int64 &value,const string &columnname,const string &tablename)
{
    int  rc = -1;
    string  column(columnname);
    ostringstream   where;
    where << "  enabled = "<< false;
    int64   num = 0;
    value = 0;
    SqlCallbackable sqlcb(_db);
    rc = sqlcb.SelectColumn(tablename,
                            column,
                            where.str(),
                            num);
    if((SQLDB_OK != rc)
       &&(SQLDB_RESULT_EMPTY != rc))
    { 
        return ERROR_DB_SELECT_FAIL;
    }

    value = num;
    return SUCCESS;
}
int32 HostPool::GetReportClusterInfo(int32 &value,const string &columnname,const string &tablename)
{
    int  rc = -1;
    string  column(columnname);
    ostringstream   where;
    where << " enabled = "<< false;
    int64   num = 0;
    value = 0;
    SqlCallbackable sqlcb(_db);
    rc = sqlcb.SelectColumn(tablename,
                            column,
                            where.str(),
                            num);
    if((SQLDB_OK != rc)
       &&(SQLDB_RESULT_EMPTY != rc))
    { 
        return ERROR_DB_SELECT_FAIL;
    }

    value = num;
    return SUCCESS;
}

const string HostPool::GetHostLocationById(int64 oid)
{
    string location;

    Host *host = Get(oid, false);
    if (NULL == host)
    {
        return location;
    }
    location = host->get_location();
    host->UnLock();
    return location;
}

bool HostPool::IsHostOffline(int64 oid)
{
    ostringstream where;
    where << " oid = " << oid << endl;

    Host::RunState hc_state;
    SelectColumn(_table_host_in_use, "run_state", where.str(), (int&)hc_state);    

    return hc_state != Host::ON_LINE;
}

bool HostPool::CheckPositionIsRepeat(BoardPosition board_position)
{
    int32 num = 0;
    map<string, int64>::iterator it;
    for (it = _host_names.begin(); it != _host_names.end(); ++it)
    {
        Host *host = Get(it->second, false);
        if (NULL == host)
        {
            continue;
        }

        BoardPosition position;
        host->get_board_position(position);
        if(board_position.bureau == position.bureau &&
           board_position.rack == position.rack &&
           board_position.shelf == position.shelf &&
           board_position.slot == position.slot )
        {
            num++;
            if(num > 1)
            {
                return true;
            }
        }
    }

    return false;   
}
/******************************************************************************/  
void HostPool::Print()
{
    map<string, int64>::iterator it;

    for (it = _host_names.begin(); it != _host_names.end(); ++it)
    {        
        Host *host = Get(it->second, true);
        if (NULL != host)
        {
            HostFree host_free;
            GetHostFree(host->GetOID(), host_free);
            host->Print(host_free);            
            host->UnLock();
        }
    }
}

/******************************************************************************/
void DbgPrintAllHost()
{
    HostPool *host_pool = HostPool::GetInstance();

    if (NULL == host_pool)
    {
        cout << "HostPool is not created." << endl;
        return;
    }

    host_pool->Print();
}
DEFINE_DEBUG_CMD(allhost, DbgPrintAllHost);

/******************************************************************************/
void DbgPrintHost(const char *host_name)
{
    if (NULL == host_name)
    {
        cout << "No input hostname." << endl;
        return;
    }
    
    HostPool *host_pool = HostPool::GetInstance();
    if (NULL == host_pool)
    {
        cout << "HostPool is not created." << endl;
        return;
    }

    Host *host = host_pool->Get(host_name, true);
    if (NULL == host)
    {
        cout << "Host=" << host_name << " is not found." << endl;
        return;
    }

    HostFree host_free;
    host_pool->GetHostFree(host->GetOID(), host_free);
    host->Print(host_free);
    host->UnLock();
}
DEFINE_DEBUG_CMD(host, DbgPrintHost);


/******************************************************************************/
void DbgPrintHostPort(const char *host_name)
{
    if (NULL == host_name)
    {
        cout << "No input hostname." << endl;
        return;
    }

    HostPool *host_pool = HostPool::GetInstance();
    if (NULL == host_pool)
    {
        cout << "HostPool is not created." << endl;
        return;
    }

    Host *host = host_pool->Get(host_name, true);
    if (NULL == host)
    {
        cout << "Host=" << host_name << " is not found." << endl;
        return;
    }

    host->UnLock();
}
DEFINE_DEBUG_FUNC(DbgPrintHostPort);

/******************************************************************************/  
void HostPool::PrintTrunk()
{
    map<string, int64>::iterator it;

    for (it = _host_names.begin(); it != _host_names.end(); ++it)
    {        
        Host *host = Get(it->second, true);
        if (NULL != host)
        {
            host->UnLock();
        }
    }
}

/******************************************************************************/
void DbgPrintAllHostTrunk()
{
    HostPool *host_pool = HostPool::GetInstance();

    if (NULL == host_pool)
    {
        cout << "HostPool is not created." << endl;
        return;
    }

    host_pool->PrintTrunk();
}
DEFINE_DEBUG_FUNC(DbgPrintAllHostTrunk);

/******************************************************************************/
void DbgPrintHostTrunk(const char *host_name)
{
    if (NULL == host_name)
    {
        cout << "No input hostname." << endl;
        return;
    }

    HostPool *host_pool = HostPool::GetInstance();
    if (NULL == host_pool)
    {
        cout << "HostPool is not created." << endl;
        return;
    }

    Host *host = host_pool->Get(host_name, true);
    if (NULL == host)
    {
        cout << "Host=" << host_name << " is not found." << endl;
        return;
    }

    host->UnLock();
}
DEFINE_DEBUG_FUNC(DbgPrintHostTrunk);


/******************************************************************************/  
void HostPool::PrintAlarm()
{
    map<string, int64>::iterator it;

    for (it = _host_names.begin(); it != _host_names.end(); ++it)
    {        
        Host *host = Get(it->second, true);
        if (NULL != host)
        {
            host->PrintAlarm();
            host->UnLock();
        }
    }
}

/******************************************************************************/
void DbgPrintAllHostAlarm()
{
    HostPool *host_pool = HostPool::GetInstance();

    if (NULL == host_pool)
    {
        cout << "HostPool is not created." << endl;
        return;
    }

    host_pool->PrintAlarm();
}
DEFINE_DEBUG_FUNC(DbgPrintAllHostAlarm);
/******************************************************************************/
void DbgPrintHostAlarm(const char *host_name)
{
    if (NULL == host_name)
    {
        cout << "No input hostname." << endl;
        return;
    }

    HostPool *host_pool = HostPool::GetInstance();
    if (NULL == host_pool)
    {
        cout << "HostPool is not created." << endl;
        return;
    }

    Host *host = host_pool->Get(host_name, true);
    if (NULL == host)
    {
        cout << "Host=" << host_name << " is not found." << endl;
        return;
    }

    host->PrintAlarm();
    host->UnLock();
}
DEFINE_DEBUG_FUNC(DbgPrintHostAlarm);

} // namespace zte_tecs

