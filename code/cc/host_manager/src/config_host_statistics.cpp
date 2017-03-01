/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：config_host_statistics.cpp
* 文件标识：
* 内容摘要：ConfigHostStatistics 和 ConfigHostStatisticsNics 类的定义文件
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
#include "config_host_statistics.h"

namespace zte_tecs{

/* host_statistics 配置使用的表名称 */
const char *ConfigHostStatistics::_table = "host_statistics";
const char *ConfigHostStatisticsNics::_table = "host_statistics_nics";

/* host_statistics 配置中的表字段 */
const char *ConfigHostStatistics::_db_names =
    "hid, statistics_time, " // 主键, hid + HC 监控的时间
    "save_time, cpu_used_rate, mem_used_rate ";

/* host_statistics_nics 配置中的表字段 */
const char *ConfigHostStatisticsNics::_db_names =
    "hid, name, statistics_time, " // 主键, hid + HC 监控的时间 + name
    "save_time, name, used_rate"; // CC 入库的时间

SqlDB *ConfigHostStatistics::_db = NULL;
SqlDB *ConfigHostStatisticsNics::_db = NULL;

ConfigHostStatistics *ConfigHostStatistics::_instance = NULL;
ConfigHostStatisticsNics *ConfigHostStatisticsNics::_instance = NULL;

/******************************************************************************/
ConfigHostStatistics::ConfigHostStatistics(SqlDB *pDb)
{
    _db = pDb;
}

/******************************************************************************/ 
int32 ConfigHostStatistics::SelectCallback(void *nil, SqlColumn * columns)
{
    if (NULL == columns)
    {
        return -1;
    }
    
    ResourceStatistics host_statistics;
    columns->GetValue(STATISTICS_TIME, host_statistics._statistics_time); 
    vector<ResourceStatistics> *vec_host_statistics
                    = static_cast<vector<ResourceStatistics> *>(nil);
    vec_host_statistics->push_back(host_statistics);
    return 0;
}

/******************************************************************************/ 
int32 ConfigHostStatistics::Search(vector<ResourceStatistics>& vec_host_statistics,
                                   const string where)
{
    ostringstream   oss;
    int             rc;

    oss << "SELECT " << _db_names << " FROM " << _table << where;

    SetCallback(static_cast<Callbackable::Callback>(&ConfigHostStatistics::SelectCallback),
                &vec_host_statistics);
    rc = _db->Execute(oss, this);
    UnsetCallback();

    if (0 == rc || SQLDB_RESULT_EMPTY == rc)
    {
        return 0;
    }
    return -1;
}

/******************************************************************************/
int32 ConfigHostStatistics::Insert(const ResourceStatistics& host_statistics,
                                   int64 hid,
                                   const string& save_time)
{
    SqlCommand sql(_db,  _table);
    sql.Add("hid", hid); // hid
    sql.Add("statistics_time", host_statistics._statistics_time);
    sql.Add("save_time",       save_time);
    sql.Add("cpu_used_rate",   host_statistics._cpu_used_rate);
    sql.Add("mem_used_rate",   host_statistics._mem_used_rate);
    return sql.Insert();
}


/******************************************************************************/
int32 ConfigHostStatistics::Delete(const string& statistics_time)
{
    SqlCommand sql(_db,  _table);

    ostringstream where ;
    where << " WHERE statistics_time <= '" << statistics_time << "'";
    return sql.Delete(where.str());
}

/******************************************************************************/
ConfigHostStatisticsNics::ConfigHostStatisticsNics(SqlDB *pDb)
{
    _db = pDb;
}

/******************************************************************************/ 
int32 ConfigHostStatisticsNics::SelectCallback(void *nil, SqlColumn *columns)
{
    if (NULL == columns)
    {
        return -1;
    }
    
    NicInfo nic_info;
    columns->GetValue(USED_RATE, nic_info._used_rate); 
    vector<NicInfo> *vec_nic_info = static_cast<vector<NicInfo> *>(nil);
    vec_nic_info->push_back(nic_info);
    return 0;
}

/******************************************************************************/ 
int32 ConfigHostStatisticsNics::Search(vector<NicInfo>& vec_nic_info,
                                       const string where)
{
    ostringstream   oss;
    int             rc;

    oss << "SELECT " << _db_names << " FROM " << _table << where;

    SetCallback(static_cast<Callbackable::Callback>(&ConfigHostStatisticsNics::SelectCallback),
                &vec_nic_info);
    rc = _db->Execute(oss, this);
    UnsetCallback();

    if (0 == rc || SQLDB_RESULT_EMPTY == rc)
    {
        return 0;
    }
    return -1;
}

/******************************************************************************/
int32 ConfigHostStatisticsNics::Insert(const NicInfo& nic_info, 
                                       int64 hid,
                                       const string& statistics_time,
                                       const string& save_time)
{
    SqlCommand sql(_db,  _table);
    sql.Add("hid",             hid); // hid
    sql.Add("name",            nic_info._name);
    sql.Add("statistics_time", statistics_time);
    sql.Add("save_time",       save_time);
    sql.Add("used_rate",       nic_info._used_rate);
    return sql.Insert();
}

/******************************************************************************/
int32 ConfigHostStatisticsNics::Delete(const string& statistics_time)
{
    SqlCommand sql(_db,  _table);

    ostringstream where ;
    where << " WHERE statistics_time <= '" << statistics_time << "'";
    return sql.Delete(where.str());
}

/******************************************************************************/
// 调试函数的地盘

}
