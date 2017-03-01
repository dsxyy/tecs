/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：current_alarm.cpp
* 文件标识：见配置管理计划书
* 内容摘要：CurrentAlarm类实现文件
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


#include "current_alarm.h"

#include <sstream>
#include "db_config.h"


namespace zte_tecs
{

/* ************************************************************************** */
/* CurrentAlarm :: Database Access Functions                                  */
/* ************************************************************************** */

const char * CurrentAlarm::_table = "current_alarm";

const char * CurrentAlarm::_db_names = "oid,alarm_id,alarm_code,alarm_time,alarm_addr,"
    "level1,level2,level3,location,addinfo,alarm_key,crccode0,crccode1,crccode2,crccode3,subsystem";

ostream& operator<<(ostream& os, CurrentAlarm& alarm)
{
    string alarm_str;

    os << alarm.to_xml(alarm_str);

    return os;
}


/******************************************************************************/

CurrentAlarm::CurrentAlarm(int64 id, const T_CurrentAlarm& current_alarm) :
    PoolObjectSQL(id), _current_alarm(current_alarm)
{

}


string& CurrentAlarm::to_str(string& str) const
{

    ostringstream   os;

    os <<
        "ID        = "  << _oid                      << endl <<
        "ALARMID   = "  << _current_alarm.AlarmID    << endl <<
        "ALARMCODE = "  << _current_alarm.AlarmCode  << endl <<
        "ALARMTIME = "  << _current_alarm.AlarmTime  << endl <<
        "ALARMADDR = "  << _current_alarm.AlarmAddr  << endl <<
        "SUBSYSTEM = "  << _current_alarm.SubSystem  << endl <<
        "LEVEL1    = "  << _current_alarm.Level1     << endl <<
        "LEVEL2    = "  << _current_alarm.Level2     << endl <<
        "LEVEL3    = "  << _current_alarm.Level3     << endl <<
        "LOCATION  = "  << _current_alarm.Location   << endl <<
        "ADDINFO   = "  << _current_alarm.AddInfo    << endl <<
        "ALARMKEY  = "  << _current_alarm.AlarmKey   << endl <<
        "CRCCODE0  = "  << _current_alarm.dwCRCCode0 << endl <<
        "CRCCODE1  = "  << _current_alarm.dwCRCCode1 << endl <<
        "CRCCODE2  = "  << _current_alarm.dwCRCCode2 << endl <<
        "CRCCODE3  = "  << _current_alarm.dwCRCCode3 << endl;

    str = os.str();

    return str;
}


/******************************************************************************/

string& CurrentAlarm::to_xml(string& xml) const
{

    ostringstream   oss;

    oss <<
    "<CURRENTALARM>"
         "<ID>"          << _oid                      <<"</ID>"         <<
         "<ALARMID>"     << _current_alarm.AlarmID    <<"</ALARMID>"    <<
         "<ALARMCODE>"   << _current_alarm.AlarmCode  <<"</ALARMCODE>"  <<
         "<ALARMTIME>"   << _current_alarm.AlarmTime  <<"</ALARMTIME>"  <<
         "<ALARMADDR>"   << _current_alarm.AlarmAddr  <<"</ALARMADDR>"  <<
         "<SUBSYSTEM>"   << _current_alarm.SubSystem  <<"</SUBSYSTEM>"  <<
         "<LEVEL1>"      << _current_alarm.Level1     <<"</LEVEL1>"     <<
         "<LEVEL2>"      << _current_alarm.Level2     <<"</LEVEL2>"     <<
         "<LEVEL3>"      << _current_alarm.Level3     <<"</LEVEL3>"     <<
         "<LOCATION>"    << _current_alarm.Location   <<"</LOCATION>"   <<
         "<ADDINFO>"     << _current_alarm.AddInfo    <<"</ADDINFO>"    <<
         "<ALARMKEY>"    << _current_alarm.AlarmKey   <<"</ALARMKEY>"   <<
         "<CRCCODE0>"    << _current_alarm.dwCRCCode0 <<"</CRCCODE0>"   <<
         "<CRCCODE1>"    << _current_alarm.dwCRCCode1 <<"</CRCCODE1>"   <<
         "<CRCCODE2>"    << _current_alarm.dwCRCCode2 <<"</CRCCODE2>"   <<
         "<CRCCODE3>"    << _current_alarm.dwCRCCode3 <<"</CRCCODE3>"   <<
    "</CURRENTALARM>";

    xml = oss.str();

    return xml;
}

/******************************************************************************/

int CurrentAlarm::Insert_Replace(SqlDB* db, bool replace)
{

    ostringstream   oss;

     SqlCommand sql(db,_table);
    sql.Add("oid",        _oid);
    sql.Add("alarm_id",   (int)_current_alarm.AlarmID);
    sql.Add("alarm_code", (int)_current_alarm.AlarmCode);
    sql.Add("alarm_time",  _current_alarm.AlarmTime);
    sql.Add("alarm_addr",  _current_alarm.AlarmAddr);
    sql.Add("subsystem",   _current_alarm.SubSystem);
    sql.Add("level1",  _current_alarm.Level1);
    sql.Add("level2",  _current_alarm.Level2);
    sql.Add("level3",  _current_alarm.Level3);
    sql.Add("location",  _current_alarm.Location);
    sql.Add("addinfo",  _current_alarm.AddInfo);
    sql.Add("alarm_key",  _current_alarm.AlarmKey);
    sql.Add("crccode0",  (int)_current_alarm.dwCRCCode0);
    sql.Add("crccode1",  (int)_current_alarm.dwCRCCode1);
    sql.Add("crccode2", (int) _current_alarm.dwCRCCode2);
    sql.Add("crccode3",  (int)_current_alarm.dwCRCCode3);

    if(replace)
    {
        return sql.Update("WHERE oid = " + to_string<int64>(_oid, dec));
    }
    else
    {
         return sql.Insert();
    }

}


/******************************************************************************/

int CurrentAlarm::SelectCallback(void* nil, SqlColumn * columns)
{
    if (NULL == columns || LIMIT != columns->get_column_num())
    {
        return -1;
    }

    _oid                      = columns->GetValueInt64(OID);
    _current_alarm.AlarmID    = columns->GetValueInt(ALARMID);
    _current_alarm.AlarmCode  = columns->GetValueInt(ALARMCODE);
    _current_alarm.AlarmTime  = columns->GetValueInt(ALARMTIME);
    _current_alarm.AlarmAddr  = columns->GetValueText(ALARMADDR);
    _current_alarm.SubSystem  = columns->GetValueText(SUBSYSTEM);
    _current_alarm.Level1     = columns->GetValueText(LEVEL1);
    _current_alarm.Level2     = columns->GetValueText(LEVEL2);
    _current_alarm.Level3     = columns->GetValueText(LEVEL3);
    _current_alarm.Location   = columns->GetValueText(LOCATION);
    _current_alarm.AddInfo    = columns->GetValueText(ADDINFO);
    _current_alarm.AlarmKey   = columns->GetValueText(ALARMKEY);
    _current_alarm.dwCRCCode0 = columns->GetValueInt64(CRCCODE0);
    _current_alarm.dwCRCCode1 = columns->GetValueInt64(CRCCODE1);
    _current_alarm.dwCRCCode2 = columns->GetValueInt64(CRCCODE2);
    _current_alarm.dwCRCCode3 = columns->GetValueInt64(CRCCODE3);
    return 0;

}


/******************************************************************************/

int CurrentAlarm::Select(SqlDB* db)
{
    ostringstream   oss;
    int             rc;
    int64           boid;

    SetCallback(static_cast<Callbackable::Callback>(
        &CurrentAlarm::SelectCallback));

    oss << "SELECT " << _db_names << " FROM " << _table << " WHERE oid = " <<_oid;

    boid = _oid;
    _oid  = -1;

    rc = db->Execute(oss, this);

    UnsetCallback();

    if ((0 != rc) || (_oid != boid))
    {
        return -1;
    }

    return 0;
}


/******************************************************************************/

int CurrentAlarm::Insert(SqlDB* db, string& error_str)
{
    int rc;

    rc = Insert_Replace(db, false);

    if (0 != rc)
    {
        error_str = "Error inserting CurrentAlarm in DB.";
        return rc;
    }

    return 0;
}


/******************************************************************************/

int CurrentAlarm::Update(SqlDB* db)
{
    int rc;

    rc = Insert_Replace(db, true);

    if (0 != rc)
    {
        return rc;
    }

    return 0;;
}


/******************************************************************************/

int CurrentAlarm::Drop(SqlDB* db)
{
    ostringstream oss;
    int rc;

    oss << "DELETE FROM " << _table << " WHERE oid=" << _oid;

    rc = db->Execute(oss);

    if (0 == rc)
    {
        set_valid(false);
    }

    return rc;
}

/******************************************************************************/

int CurrentAlarm::DeleteTable(SqlDB* db)
{
    ostringstream oss;
    int rc;

    oss << "truncate " << _table;

    rc = db->Execute(oss);

    if ( rc == 0 )
    {
        set_valid(false);
    }

    return rc;
}


/******************************************************************************/

int CurrentAlarm::Dump(ostringstream& oss, SqlColumn* columns)
{
    if (NULL == columns || LIMIT != columns->get_column_num())
    {
        return -1;
    }

    oss <<
        "<CURRENTALARM>" <<
            "<ID>"        << columns->GetValueInt64(OID)     <<"</ID>"        <<
            "<ALARMID>"   << columns->GetValueInt(ALARMID)   <<"</ALARMID>"   <<
            "<ALARMCODE>" << columns->GetValueInt(ALARMCODE) <<"</ALARMCODE>" <<
            "<ALARMTIME>" << columns->GetValueInt(ALARMTIME) <<"</ALARMTIME>" <<
            "<ALARMADDR>" << columns->GetValueText(ALARMADDR) <<"</ALARMADDR>"  <<
            "<SUBSYSTEM>" << columns->GetValueText(SUBSYSTEM) <<"</SUBSYSTEM>"    <<
            "<LEVEL1>"    << columns->GetValueText(LEVEL1)   <<"</LEVEL1>"    <<
            "<LEVEL2>"    << columns->GetValueText(LEVEL2)   <<"</LEVEL2>" <<
            "<LEVEL3>"    << columns->GetValueText(LEVEL3)    <<"</LEVEL3>" <<
            "<LOCATION>"  << columns->GetValueText(LOCATION) <<"</LOCATION>" <<
            "<ADDINFO>"   << columns->GetValueText(ADDINFO)  <<"</ADDINFO>"   <<
            "<ALARMKEY>"  << columns->GetValueText(ALARMKEY) <<"</ALARMKEY>"  <<
            "<CRCCODE0>"  << columns->GetValueInt64(CRCCODE0) <<"</CRCCODE0>" <<
            "<CRCCODE1>"  << columns->GetValueInt64(CRCCODE1) <<"</CRCCODE1>" <<
            "<CRCCODE2>"  << columns->GetValueInt64(CRCCODE2) <<"</CRCCODE2>" <<
            "<CRCCODE3>"  << columns->GetValueInt64(CRCCODE3) <<"</CRCCODE3>" <<
        "</CURRENTALARM>";

    return 0;

}

int CurrentAlarm::Dump(T_CurrentAlarm *pCurrentAlarm, SqlColumn* columns)
{
    if (NULL == columns || LIMIT != columns->get_column_num())
    {
        return -1;
    }

    pCurrentAlarm->AlarmID    = columns->GetValueInt(ALARMID);
    pCurrentAlarm->AlarmCode  = columns->GetValueInt(ALARMCODE);
    pCurrentAlarm->AlarmTime  = columns->GetValueInt(ALARMTIME);
    pCurrentAlarm->AlarmAddr  = columns->GetValueText(ALARMADDR);
    pCurrentAlarm->SubSystem  = columns->GetValueText(SUBSYSTEM);
    pCurrentAlarm->Level1     = columns->GetValueText(LEVEL1);
    pCurrentAlarm->Level2     = columns->GetValueText(LEVEL2);
    pCurrentAlarm->Level3     = columns->GetValueText(LEVEL3);
    pCurrentAlarm->Location= columns->GetValueText(LOCATION);
    pCurrentAlarm->AddInfo    = columns->GetValueText(ADDINFO);
    pCurrentAlarm->AlarmKey   = columns->GetValueText(ALARMKEY);
    pCurrentAlarm->dwCRCCode0 = columns->GetValueInt64(CRCCODE0);
    pCurrentAlarm->dwCRCCode1 = columns->GetValueInt64(CRCCODE1);
    pCurrentAlarm->dwCRCCode2 = columns->GetValueInt64(CRCCODE2);
    pCurrentAlarm->dwCRCCode3 = columns->GetValueInt64(CRCCODE3);

    return 0;

}

int CurrentAlarm::Dump(vector<xmlrpc_c::value>& arrayAlarm, SqlColumn* columns)
{
    if (NULL == columns || LIMIT != columns->get_column_num())
    {
        return -1;
    }
    
    map<string, xmlrpc_c::value> oneAlarm;
    oneAlarm.insert(make_pair("oid", xmlrpc_c::value_int(columns->GetValueInt64(OID))));
    oneAlarm.insert(make_pair("alarmid", xmlrpc_c::value_int(columns->GetValueInt(ALARMID))));
    oneAlarm.insert(make_pair("alarmcode", xmlrpc_c::value_int(columns->GetValueInt(ALARMCODE))));
    Datetime dt1(columns->GetValueInt(ALARMTIME));
    string s1 = dt1.tostr();
    oneAlarm.insert(make_pair("alarmtime", xmlrpc_c::value_string(s1)));
    oneAlarm.insert(make_pair("alarmaddr", xmlrpc_c::value_string(columns->GetValueText(ALARMADDR))));
    oneAlarm.insert(make_pair("subsystem", xmlrpc_c::value_string(columns->GetValueText(SUBSYSTEM))));
    oneAlarm.insert(make_pair("level1", xmlrpc_c::value_string(columns->GetValueText(LEVEL1))));
    oneAlarm.insert(make_pair("level2", xmlrpc_c::value_string(columns->GetValueText(LEVEL2))));
    oneAlarm.insert(make_pair("level3", xmlrpc_c::value_string(columns->GetValueText(LEVEL3))));
    oneAlarm.insert(make_pair("location", xmlrpc_c::value_string(columns->GetValueText(LOCATION))));
    oneAlarm.insert(make_pair("alarmaddinfo", xmlrpc_c::value_string(columns->GetValueText(ADDINFO))));
    oneAlarm.insert(make_pair("alarmkey", xmlrpc_c::value_string(columns->GetValueText(ALARMKEY))));
    oneAlarm.insert(make_pair("crccode0", xmlrpc_c::value_int(columns->GetValueInt64(CRCCODE0))));
    oneAlarm.insert(make_pair("crccode1", xmlrpc_c::value_int(columns->GetValueInt64(CRCCODE1))));
    oneAlarm.insert(make_pair("crccode2", xmlrpc_c::value_int(columns->GetValueInt64(CRCCODE2))));
    oneAlarm.insert(make_pair("crccode3", xmlrpc_c::value_int(columns->GetValueInt64(CRCCODE3))));
    arrayAlarm.push_back(xmlrpc_c::value_struct(oneAlarm));

    return 0;

}

/******************************************************************************/

}   // namespace zte_tecs

