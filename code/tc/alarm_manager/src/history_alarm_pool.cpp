/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：history_alarm_pool.cpp
* 文件标识：见配置管理计划书
* 内容摘要：HistoryAlarmPoll类实现文件
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


#include "history_alarm.h"
#include "history_alarm_pool.h"
#include "alarm_api.h"
#include "log_agent.h"
#include "db_config.h"

namespace zte_tecs
{
HistoryAlarmPool *HistoryAlarmPool::instance = NULL;

const char* HistoryAlarmPool::_table = "history_alarm";

const char* HistoryAlarmPool::_db_names = "oid,alarm_id,alarm_code,alarm_time,"
    "alarm_addr,level1,level2,level3,location,addinfo,restore_time,restore_type,subsystem";

int64 HistoryAlarmPool::_lastOID = -1;

pthread_mutex_t HistoryAlarmPool::_mutex;

/******************************************************************************/
STATUS HistoryAlarmPool::Init()
{
    int64 oid = 0;
    /*初始化锁*/
    pthread_mutex_init(&_mutex,0);
    
    /*获取oid最大值，后续allocate函数会用到*/
    int ret = SelectColumn(_table,"MAX(oid)","", oid);
    
    if((SQLDB_OK != ret) && (SQLDB_RESULT_EMPTY != ret))
    {
        return ERROR_DB_SELECT_FAIL;
    }

    _lastOID = oid;

    return SUCCESS;
}

/******************************************************************************/

STATUS HistoryAlarmPool::Allocate(HistoryAlarm &alarm)
{
    int rc;

    if (_lastOID == LLONG_MAX)
    {
        _lastOID = -1;
    }

    Lock();
    alarm._oid = ++_lastOID;
    UnLock();

    SqlCommand sql(_db, _table);
    sql.Add("oid",        alarm._oid);
    sql.Add("alarm_id",   (int)alarm.AlarmID);
    sql.Add("alarm_code", (int)alarm.AlarmCode);
    sql.Add("alarm_time",  alarm.AlarmTime);
    sql.Add("alarm_addr",  alarm.AlarmAddr);
    sql.Add("subsystem", alarm.SubSystem);
    sql.Add("level1",  alarm.Level1);
    sql.Add("level2",  alarm.Level2);
    sql.Add("level3",  alarm.Level3);
    sql.Add("location",  alarm.Location);
    sql.Add("addinfo",  alarm.AddInfo);
    sql.Add("restore_time",  alarm.RestoreTime);
    sql.Add("restore_type",  (int)alarm.RestoreType);

    rc = sql.Insert();

    if ( rc != 0 )
    {
        return ERROR;
    }

    return SUCCESS;
}

int HistoryAlarmPool::GetHistoryAlarmCallback(void *nil, SqlColumn *columns)
{    
    if (NULL == columns ||  LIMIT != columns->get_column_num())
    {
        return -1;
    }

    if(!nil)
    {
        return 0;
    }

    HistoryAlarm * alarm;
    alarm = static_cast<HistoryAlarm *>(nil);

    columns->GetValue(OID, alarm->_oid);
    columns->GetValue(ALARMID, alarm->AlarmID);
    columns->GetValue(ALARMCODE, alarm->AlarmCode);
    columns->GetValue(ALARMTIME, alarm->AlarmTime);
    columns->GetValue(ALARMADDR, alarm->AlarmAddr); 
    columns->GetValue(SUBSYSTEM, alarm->SubSystem);
    columns->GetValue(LEVEL1, alarm->Level1);
    columns->GetValue(LEVEL2, alarm->Level2);   
    columns->GetValue(LEVEL3, alarm->Level3); 
    columns->GetValue(LOCATION, alarm->Location);
    columns->GetValue(ADDINFO, alarm->AddInfo);   
    columns->GetValue(RESTORETIME, alarm->RestoreTime);
    columns->GetValue(RESTORETYPE, alarm->RestoreType);
    
    return 0;
}


/******************************************************************************/

STATUS HistoryAlarmPool::Get(int64 oid, HistoryAlarm &alarm)
{
    STATUS rc;
    ostringstream oss;

    alarm._oid = oid;

    SetCallback(static_cast<Callbackable::Callback>(&HistoryAlarmPool::GetHistoryAlarmCallback), (void *)&alarm);

    oss << "SELECT " << _db_names << " FROM " << _table << " WHERE oid = " << oid;

    rc = _db->Execute(oss, this);

    UnsetCallback();

    if (0 != rc)
    {
        return SQLDB_ERROR;
    }

    return SUCCESS;
}

int HistoryAlarmPool::ShowHistoryAlarmCallback(void * arg, SqlColumn * columns)
{
    vector<ApiHistoryAlarmInfo> *ptHARecord;
    ApiHistoryAlarmInfo tInfo;

    if (NULL == arg)
    {
        return -1;
    }
    ptHARecord = static_cast<vector<ApiHistoryAlarmInfo> *>(arg);

    if ((NULL == columns) || (LIMIT != columns->get_column_num()))
    {
        return -1;
    }

    columns->GetValue(ALARMID, tInfo.AlarmID);
    columns->GetValue(ALARMCODE, tInfo.AlarmCode);
    columns->GetValue(ALARMTIME, tInfo.AlarmTime);
    columns->GetValue(ALARMADDR, tInfo.AlarmAddr); 
    columns->GetValue(SUBSYSTEM, tInfo.SubSystem);    
    columns->GetValue(LEVEL1, tInfo.Level1);
    columns->GetValue(LEVEL2, tInfo.Level2);   
    columns->GetValue(LEVEL3, tInfo.Level3); 
    columns->GetValue(LOCATION, tInfo.Location);
    columns->GetValue(ADDINFO, tInfo.AddInfo);   
    columns->GetValue(RESTORETIME, tInfo.RestoreTime);
    columns->GetValue(RESTORETYPE, tInfo.RestoreType);

    ptHARecord->push_back(tInfo);

    return 0;
}

STATUS HistoryAlarmPool::Show(vector<ApiHistoryAlarmInfo> &vec_api_alarm, const string& where)
{
    STATUS rc;
    ostringstream oss;

    SetCallback(static_cast<Callbackable::Callback>(&HistoryAlarmPool::ShowHistoryAlarmCallback),
                static_cast<void *>(&vec_api_alarm));

    oss << "SELECT " << _db_names << " FROM " << _table;

    if (!where.empty())
    {
        oss << " WHERE " << where;
    }

    rc = _db->Execute(oss, this);

    UnsetCallback();

    if ((SQLDB_OK != rc) && (SQLDB_RESULT_EMPTY != rc))
    {
        return SQLDB_ERROR;
    }

    return SUCCESS;
}

/******************************************************************************/

STATUS HistoryAlarmPool::Update(const HistoryAlarm &alarm)
{
    STATUS rc;

    SqlCommand sql(_db, _table);
    sql.Add("oid",        alarm._oid);
    sql.Add("alarm_id",   (int)alarm.AlarmID);
    sql.Add("alarm_code", (int)alarm.AlarmCode);
    sql.Add("alarm_time",  alarm.AlarmTime);
    sql.Add("alarm_addr",  alarm.AlarmAddr);
    sql.Add("subsystem",  alarm.SubSystem);
    sql.Add("level1",  alarm.Level1);
    sql.Add("level2",  alarm.Level2);
    sql.Add("level3",  alarm.Level3);
    sql.Add("location",  alarm.Location);
    sql.Add("addinfo",  alarm.AddInfo);
    sql.Add("restore_time",  alarm.RestoreTime);
    sql.Add("restore_type",  (int)alarm.RestoreType);

    rc = sql.Update("WHERE oid = " + to_string<int64>(alarm._oid, dec));
    
    return rc;
}

/******************************************************************************/

STATUS HistoryAlarmPool::Drop(const HistoryAlarm &alarm)
{
    ostringstream oss;
    STATUS rc;

    oss << "DELETE FROM " << _table << " WHERE oid=" << alarm._oid;

    rc = _db->Execute(oss);

    return rc;
}

/******************************************************************************/

} // namespace zte_tecs


