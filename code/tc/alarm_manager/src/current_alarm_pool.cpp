/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：current_alarm_pool.cpp
* 文件标识：见配置管理计划书
* 内容摘要：CurrentAlarmPoll类实现文件
* 当前版本：1.0
* 作    者：马兆勉
* 完成日期：2011年7月27日
*
* 修改记录1：
*     修改日期：2011/7/27
*     版 本 号：V1.0
*     修 改 人：马兆勉
*     修改内容：创建
*******************************************************************************/


#include "current_alarm_pool.h"

namespace zte_tecs
{
CurrentAlarmPool *CurrentAlarmPool::instance = NULL;
/******************************************************************************/
CurrentAlarmPool::CurrentAlarmPool(SqlDB* db) :
    PoolSQL(db,CurrentAlarm::_table)
{
    ostringstream   sql;

    SetCallback(static_cast<Callbackable::Callback>(
         &CurrentAlarmPool::InitCallback));

    sql  << "SELECT oid, alarm_key FROM " <<  CurrentAlarm::_table;

    db->Execute(sql, this);

    UnsetCallback();
}

/******************************************************************************/

int64 CurrentAlarmPool::Allocate(int64 * oid,
                                 const T_CurrentAlarm& cur_alarm,
                                 string& error_str)
{
    CurrentAlarm * alarm;

    // Build a new CurrentAlarm object
    
    alarm = new CurrentAlarm(-1, cur_alarm);
    
    string alarmkey = cur_alarm.AlarmKey;
    
    // Insert the Object in the pool

    *oid = PoolSQL::Allocate(alarm, error_str);

    if (-1 != *oid)
    {
        // Add the alarm to the map of known_alarms
        _known_alarms.insert(make_pair(alarmkey,*oid));
    }

    return *oid;
}

/******************************************************************************/

int CurrentAlarmPool::Dump(ostringstream& oss, const string& where)
{
    int             rc;
    ostringstream   cmd;

    oss << "<CURRENT_ALARM_POOL>";

    SetCallback(static_cast<Callbackable::Callback>(
        &CurrentAlarmPool::DumpCallback), static_cast<void *>(&oss));

    cmd << "SELECT " << CurrentAlarm::_db_names << " FROM "
        << CurrentAlarm::_table;

    if (!where.empty())
    {
        cmd << " WHERE " << where;
    }

    rc = _db->Execute(cmd, this);

    UnsetCallback();

    oss << "</CURRENT_ALARM_POOL>";

    return rc;
}

/******************************************************************************/

int CurrentAlarmPool::Dump(vector<xmlrpc_c::value>& arrayAlarm, const string& where)
{
    int             rc;
    ostringstream   cmd;

    SetCallback(static_cast<Callbackable::Callback>(
        &CurrentAlarmPool::DumpStructCallback), static_cast<void *>(&arrayAlarm));

    cmd << "SELECT " << CurrentAlarm::_db_names << " FROM "
        << CurrentAlarm::_table;

    if ( !where.empty() )
    {
        cmd << " WHERE " << where;
    }

    rc = _db->Execute(cmd, this);

    UnsetCallback();


    return rc;
}

/******************************************************************************/

int CurrentAlarmPool::DumpCallback(void* oss, SqlColumn* columns)
{
    ostringstream * oss_t;

    oss_t = static_cast<ostringstream *>(oss);

    return CurrentAlarm::Dump(*oss_t, columns);
}

/******************************************************************************/

int CurrentAlarmPool::DumpStructCallback(void *arrayAlarm, SqlColumn* columns)
{
    vector<xmlrpc_c::value>* arrayAlarm_t;

    arrayAlarm_t = static_cast<vector<xmlrpc_c::value> *>(arrayAlarm);

    return CurrentAlarm::Dump(*arrayAlarm_t, columns);
}


/******************************************************************************/

int CurrentAlarmPool::InitCallback(void* nil, SqlColumn* columns)
{
    if (NULL == columns || 2 != columns->get_column_num())
    {
        return -1;
    }

    int64 oid = columns->GetValueInt64(0);
    string alarmkey = columns->GetValueText(1);

    _known_alarms.insert(make_pair(alarmkey,oid));

    return 0;
}

void CurrentAlarmPool::ShowMemoryAlarm(void)
{
    cout << "alarm cout = " << _known_alarms.size() << endl;
    map<string,int64>::iterator iter;
    for( iter = _known_alarms.begin(); iter != _known_alarms.end(); ++iter ) 
    {
        cout << "alarmkey: " << iter->first << ", oid: " << iter->second << endl;
    }
}

void OamDbgAlarmMShowMemoryAlarm(void)
{
    CurrentAlarmPool *pcap = CurrentAlarmPool::GetInstance();
    pcap->ShowMemoryAlarm();
    
}
DEFINE_DEBUG_FUNC(OamDbgAlarmMShowMemoryAlarm);

/******************************************************************************/

} // namespace zte_tecs


