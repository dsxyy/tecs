/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：config_vm_statistics.cpp
* 文件标识：
* 内容摘要：ConfigVmStatistics 和 ConfigVmStatisticsNics 类的定义文件
* 当前版本：1.0
* 作    者：xiett
* 完成日期：2013年7月30日
*
* 修改记录1：
*     修改日期：2013/7/30
*     版 本 号：V1.0
*     修 改 人：xiett
*     修改内容：创建
*******************************************************************************/
#include "config_vm_statistics.h"

namespace zte_tecs{

/* vm_statistics 配置使用的表名称 */
const char *ConfigVmStatistics::_table = "vm_statistics";
/* vm_statistics_nics 配置使用的表名称 */
const char *ConfigVmStatisticsNics::_table = "vm_statistics_nics";

/* vm_statistics 配置中的表字段 */
const char *ConfigVmStatistics::_db_names =
    "vid, statistics_time, " // 主键: hid + HC 统计的时间
    "save_time, cpu_used_rate, mem_used_rate "; // CC 入库的时间
/* host_nic 配置中的表字段 */
const char *ConfigVmStatisticsNics::_db_names =
    "vid, name, statistics_time, " // 主键: hid+ HC 统计的时间 + name
    "save_time, used_rate "; // CC 入库的时间

const char *VmDynamicData::_table = "view_project_dynamic_data";
const char *VmDynamicData::_db_names =
    "vid, nic_used_rate, cpu_used_rate,mem_used_rate " ;

	
SqlDB *ConfigVmStatistics::_db = NULL;
SqlDB *ConfigVmStatisticsNics::_db = NULL;
SqlDB *VmDynamicData::_db = NULL;

ConfigVmStatistics *ConfigVmStatistics::_instance = NULL;
ConfigVmStatisticsNics *ConfigVmStatisticsNics::_instance = NULL;
VmDynamicData *VmDynamicData::_instance = NULL;

/******************************************************************************/
ConfigVmStatistics::ConfigVmStatistics(SqlDB *pDb)
{
    _db = pDb;
}

/******************************************************************************/ 
int32 ConfigVmStatistics::SelectCallback(void *nil, SqlColumn *columns)
{
    if (NULL == columns)
    {
        return -1;
    }
    
    ResourceStatistics vm_statistics;
    columns->GetValue(STATISTICS_TIME, vm_statistics._statistics_time); 
    vector<ResourceStatistics> *vec_vm_statistics
                    = static_cast<vector<ResourceStatistics> *>(nil);
    vec_vm_statistics->push_back(vm_statistics);
    return 0;
}

/******************************************************************************/ 
int32 ConfigVmStatistics::Search(vector<ResourceStatistics>& vec_vm_statistics,
                                 const string where)
{
    ostringstream   oss;
    int             rc;

    oss << "SELECT " << _db_names << " FROM " << _table << where;

    SetCallback(static_cast<Callbackable::Callback>(&ConfigVmStatistics::SelectCallback),
                &vec_vm_statistics);
    rc = _db->Execute(oss, this);
    UnsetCallback();

    if (0 == rc || SQLDB_RESULT_EMPTY == rc)
    {
        return 0;
    }
    return -1;
}

/******************************************************************************/
int32 ConfigVmStatistics::Insert(const ResourceStatistics& vm_statistics,
                                 int64 vid,
                                 const string& save_time)
{
    SqlCommand sql(_db,  _table);
    sql.Add("vid", vid); // vid
    sql.Add("statistics_time", vm_statistics._statistics_time);
    sql.Add("save_time",       save_time);
    sql.Add("cpu_used_rate",   vm_statistics._cpu_used_rate);
    sql.Add("mem_used_rate",   vm_statistics._mem_used_rate);
    return sql.Insert();
}

/******************************************************************************/
int32 ConfigVmStatistics::Delete(const string& statistics_time)
{
    SqlCommand sql(_db,  _table);

    ostringstream where ;
    where << " WHERE statistics_time <= '" << statistics_time << "'";
    return sql.Delete(where.str());
}

/******************************************************************************/
ConfigVmStatisticsNics::ConfigVmStatisticsNics(SqlDB *pDb)
{
    _db = pDb;
}

/******************************************************************************/ 
int32 ConfigVmStatisticsNics::SelectCallback(void *nil, SqlColumn *columns)
{
    VmStatisticsNics nic;
    if (NULL == columns ||LIMIT != columns->get_column_num())
    {
        return -1;
    }
    
    columns->GetValue(VID, nic._vid);
    columns->GetValue(NAME, nic._nic_name);
    columns->GetValue(STATISTICS_TIME, nic._statistics_time);
    columns->GetValue(SAVE_TIME, nic._save_time);
    columns->GetValue(USED_RATE, nic._used_rate);
    vector<VmStatisticsNics> *_nics = static_cast<vector<VmStatisticsNics> *>(nil);
    _nics->push_back(nic);
    return 0;
}
#if 0
/******************************************************************************/ 
int32 ConfigVmStatisticsNics::Search(vector<NicInfo>& vec_nic_info,
                                     const string where)
{
    ostringstream   oss;
    int             rc;

    oss << "SELECT " << _db_names << " FROM " << _table << where;

    SetCallback(static_cast<Callbackable::Callback>(&ConfigVmStatisticsNics::SelectCallback),
                &vec_nic_info);
    rc = _db->Execute(oss, this);
    UnsetCallback();

    if (0 == rc || SQLDB_RESULT_EMPTY == rc)
    {
        return 0;
    }
    return -1;
}
#endif
/******************************************************************************/
int ConfigVmStatisticsNics::GetVmStatisticsNics(vector<VmStatisticsNics>& vec_info,const string where)
{
    ostringstream   oss;
    int             rc;

    oss << "SELECT " << _db_names << " FROM " << _table << where;

    SetCallback(static_cast<Callbackable::Callback>(&ConfigVmStatisticsNics::SelectCallback),
                &vec_info);
    rc = _db->Execute(oss, this);
    UnsetCallback();

    if (0 == rc || SQLDB_RESULT_EMPTY == rc)
    {
        return 0;
    }
    return -1;
}
/******************************************************************************/
int32 ConfigVmStatisticsNics::Insert(const NicInfo& nic_info, 
                                     int64 vid,
                                     const string& statistics_time,
                                     const string& save_time)
{
    SqlCommand sql(_db,  _table);
    sql.Add("vid",             vid); // vid
    sql.Add("name",            nic_info._name);
    sql.Add("statistics_time", statistics_time);
    sql.Add("save_time",       save_time);
    sql.Add("used_rate",       nic_info._used_rate);
    return sql.Insert();
}

/******************************************************************************/
int32 ConfigVmStatisticsNics::Delete(const string& statistics_time)
{
    SqlCommand sql(_db,  _table);

    ostringstream where ;
    where << " WHERE statistics_time <= '" << statistics_time << "'";
    return sql.Delete(where.str());
}

VmDynamicData::VmDynamicData(SqlDB *pDb)
{
    _db = pDb;
}

/******************************************************************************/
int32 VmDynamicData::CountProjectDynamicData(void)
{
    int32 data_count = 0;
    SqlCallbackable sqlcb(_db);
    int ret = sqlcb.SelectColumn(_table,"COUNT(1)","",data_count);
    if (SQLDB_OK != ret)
    {
        SkyAssert(false);
        return -1;
    }
    return data_count;
}


/******************************************************************************/
int32 VmDynamicData::ProjectDataCallback(void *nil, SqlColumn *columns)
{
    ProjectDynamicData project_data;
    vector<ProjectDynamicData>* vec;
    if ((NULL == columns) || (LIMIT != columns->get_column_num()))
    {
        return -1;
    }
    vec = static_cast<vector<ProjectDynamicData> *>(nil);

    columns->GetValue(VID,project_data._vm_id);
    columns->GetValue(NIC_USED_RATE,project_data._nic_used_rate);
    columns->GetValue(CPU_USED_RATE,project_data._cpu_used_rate);
    columns->GetValue(MEM_USED_RATE,project_data._mem_used_rate);
 
    vec->push_back(project_data);
    return 0;
}

/******************************************************************************/ 
int32 VmDynamicData::GetProjectDynamicData(const string &where, vector<ProjectDynamicData> &project_data)
{
    ostringstream   oss;
    int             rc;

    SetCallback(static_cast<Callbackable::Callback>
        (&VmDynamicData::ProjectDataCallback), static_cast<void *>(&project_data));

    oss << "SELECT " 
        << "vid, nic_used_rate, cpu_used_rate, mem_used_rate" 
        << " FROM " << _table << " WHERE " << where;

    rc = _db->Execute(oss, this);

    UnsetCallback();
    if ((SQLDB_OK != rc)
         &&(SQLDB_RESULT_EMPTY != rc))
    {
        return -1;
    }
    return 0;
}

/******************************************************************************/
// 调试函数的地盘

}
