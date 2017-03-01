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


#include "snmp_database_pool.h"

namespace zte_tecs
{
	
const char * SnmpXmlRpcUserPool::table = "snmp_user";

const char * SnmpXmlRpcUserPool::columns = "user_name ";

const char * SnmpXmlRpcUserPool::db_bootstrap = "CREATE TABLE IF NOT EXISTS "
    "snmp_user (user_name VARCHAR(32) PRIMARY KEY);";

SnmpConfigurationPool *SnmpConfigurationPool::instance = NULL;
TrapAddressPool *TrapAddressPool::instance = NULL;
SnmpXmlRpcUserPool *SnmpXmlRpcUserPool::instance = NULL;
/******************************************************************************/
SnmpConfigurationPool::SnmpConfigurationPool(SqlDB* db) :
    PoolSQL(db,SnmpConfiguration::_table)
{
    ostringstream   sql;

    SetCallback(static_cast<Callbackable::Callback>(
         &SnmpConfigurationPool::InitCallback));

    sql  << "SELECT oid FROM " <<  SnmpConfiguration::_table;

    db->Execute(sql, this);

    UnsetCallback();
}

/******************************************************************************/

int64 SnmpConfigurationPool::Allocate(int64 * oid,
                                 const T_SnmpConfiguration& snmp_conf,
                                 string& error_str)
{
    SnmpConfiguration * snmp;

    // Build a new CurrentAlarm object
    
    snmp = new SnmpConfiguration(-1, snmp_conf);
    
    
    // Insert the Object in the pool

    *oid = PoolSQL::Allocate(snmp, error_str);

    if (-1 != *oid)
    {
        // Add the alarm to the map of known_alarms
        return *oid;
    }

    return *oid;
}

/******************************************************************************/

int SnmpConfigurationPool::Dump(ostringstream& oss, const string& where)
{
    int             rc;
    ostringstream   cmd;

    oss << "<SNMP_CONFIGURATION_POOL>";

    SetCallback(static_cast<Callbackable::Callback>(
        &SnmpConfigurationPool::DumpCallback), static_cast<void *>(&oss));

    cmd << "SELECT " << SnmpConfiguration::_db_names << " FROM "
        << SnmpConfiguration::_table;

    if (!where.empty())
    {
        cmd << " WHERE " << where;
    }

    rc = _db->Execute(cmd, this);

    UnsetCallback();

    oss << "</SNMP_CONFIGURATION_POOL>";

    return rc;
}

/******************************************************************************/

int SnmpConfigurationPool::Dump(vector<xmlrpc_c::value>& arraySnmpConf, const string& where)
{
    int             rc;
    ostringstream   cmd;

    SetCallback(static_cast<Callbackable::Callback>(
        &SnmpConfigurationPool::DumpStructCallback), static_cast<void *>(&arraySnmpConf));

    cmd << "SELECT " << SnmpConfiguration::_db_names << " FROM "
        << SnmpConfiguration::_table;

    if ( !where.empty() )
    {
        cmd << " WHERE " << where;
    }

    rc = _db->Execute(cmd, this);

    UnsetCallback();


    return rc;
}

/******************************************************************************/

int SnmpConfigurationPool::DumpCallback(void* oss, SqlColumn* columns)
{
    ostringstream * oss_t;

    oss_t = static_cast<ostringstream *>(oss);

    return SnmpConfiguration::Dump(*oss_t, columns);
}

/******************************************************************************/

int SnmpConfigurationPool::DumpStructCallback(void *arraySnmpConf, SqlColumn* columns)
{
    vector<xmlrpc_c::value>* arraySnmpConf_t;

    arraySnmpConf_t = static_cast<vector<xmlrpc_c::value> *>(arraySnmpConf);

    return SnmpConfiguration::Dump(*arraySnmpConf_t, columns);
}


/******************************************************************************/

int SnmpConfigurationPool::InitCallback(void* nil, SqlColumn* columns)
{
    if (NULL == columns || 2 != columns->get_column_num())
    {
        return -1;
    }

    int64 oid;
    oid = columns->GetValueInt64(0);

    return 0;
}

/******************************************************************************/

/******************************************************************************/
TrapAddressPool::TrapAddressPool(SqlDB* db) :
    PoolSQL(db,TrapAddress::_table)
{
    ostringstream   sql;

    SetCallback(static_cast<Callbackable::Callback>(
         &TrapAddressPool::InitCallback));

    sql  << "SELECT oid, ip_address, port FROM " <<  TrapAddress::_table;

    db->Execute(sql, this);

    UnsetCallback();
}

/******************************************************************************/

int64 TrapAddressPool::Allocate(int64 * oid,
                                 const T_TrapAddress& trap_address,
                                 string& error_str)
{
    TrapAddress * trap;
    
    // Build a new CurrentAlarm object
    
    trap = new TrapAddress(-1, trap_address);
    
    string trap_key;
    
    string key = trap->get_key(trap_key);
    
    // Insert the Object in the pool

    *oid = PoolSQL::Allocate(trap, error_str);

    if (-1 != *oid)
    {
        // Add the alarm to the map of known_alarms
        _known_trap_address.insert(make_pair(key,*oid));
    }

    return *oid;
}

/******************************************************************************/

int TrapAddressPool::Dump(ostringstream& oss, const string& where)
{
    int             rc;
    ostringstream   cmd;

    oss << "<TRAP_ADDRESS_POOL>";

    SetCallback(static_cast<Callbackable::Callback>(
        &TrapAddressPool::DumpCallback), static_cast<void *>(&oss));

    cmd << "SELECT " << TrapAddress::_db_names << " FROM "
        << TrapAddress::_table;

    if (!where.empty())
    {
        cmd << " WHERE " << where;
    }

    rc = _db->Execute(cmd, this);

    UnsetCallback();

    oss << "</TRAP_ADDRESS_POOL>";

    return rc;
}

/******************************************************************************/

int TrapAddressPool::Dump(vector<xmlrpc_c::value>& arrayTrapAddress, const string& where)
{
    int             rc;
    ostringstream   cmd;

    SetCallback(static_cast<Callbackable::Callback>(
        &TrapAddressPool::DumpStructCallback), static_cast<void *>(&arrayTrapAddress));

    cmd << "SELECT " << TrapAddress::_db_names << " FROM "
        << TrapAddress::_table;

    if ( !where.empty() )
    {
        cmd << " WHERE " << where;
    }

    rc = _db->Execute(cmd, this);

    UnsetCallback();


    return rc;
}

/******************************************************************************/

int TrapAddressPool::DumpCallback(void* oss, SqlColumn* columns)
{
    ostringstream * oss_t;

    oss_t = static_cast<ostringstream *>(oss);

    return TrapAddress::Dump(*oss_t, columns);
}

/******************************************************************************/

int TrapAddressPool::DumpStructCallback(void *arrayTrapAddress, SqlColumn* columns)
{
    vector<xmlrpc_c::value>* arrayTrapAddress_t;

    arrayTrapAddress_t = static_cast<vector<xmlrpc_c::value> *>(arrayTrapAddress);

    return TrapAddress::Dump(*arrayTrapAddress_t, columns);
}


/******************************************************************************/

int TrapAddressPool::InitCallback(void* nil, SqlColumn* columns)
{
    if (NULL == columns || 3 != columns->get_column_num())
    {
        return -1;
    }
    
    ostringstream   oss;
    string key;

    int64 oid = columns->GetValueInt64(0);
    string address = columns->GetValueText(1);
    int64 port = columns->GetValueInt64(2);
    
    oss << address << ":" << port;
    cout << "oss" << oss.str() << endl;
    key = oss.str();
    
    _known_trap_address.insert(make_pair(key,oid));

    return 0;
}

/******************************************************************************/

/******************************************************************************/
int SnmpXmlRpcUserPool::SelectCallback(void *nil, SqlColumn * columns)
{
    if ( NULL == columns || LIMIT != columns->get_column_num())
    {
        return -1;
    }
    SnmpXmlRpcUser* user = static_cast<SnmpXmlRpcUser *>(nil);
    columns->GetValue(SnmpXmlRpcUserPool::SNMPUSER,user->_snmp_user.User);
    return 0;
}

int SnmpXmlRpcUserPool::DumpCallback(void* arraySnmpUser, SqlColumn * columns)
{
    vector<xmlrpc_c::value>* arraySnmpUser_t;

    arraySnmpUser_t = static_cast<vector<xmlrpc_c::value> *>(arraySnmpUser);
    
    if ( NULL == columns || LIMIT != columns->get_column_num())
    {
        return -1;
    }
    
    map<string, xmlrpc_c::value> oneSnmpUser;
    oneSnmpUser.insert(make_pair("user_name", xmlrpc_c::value_string(columns->GetValueText(SNMPUSER))));
    arraySnmpUser_t->push_back(xmlrpc_c::value_struct(oneSnmpUser));
    return 0;
}

int SnmpXmlRpcUserPool::Dump(vector<xmlrpc_c::value>& arraySnmpUser, const string& where)
{
    ostringstream   oss;
    int rc = -1;
    oss << "SELECT " << columns << " FROM " << table << " where " << where;
    SetCallback(static_cast<Callbackable::Callback>(&SnmpXmlRpcUserPool::DumpCallback), static_cast<void *>(&arraySnmpUser));
    rc = db->Execute(oss, this);
    UnsetCallback();

    return rc;
}

int SnmpXmlRpcUserPool::SetUser(SnmpXmlRpcUser* user)
{
    SqlCommand sql(db,SnmpXmlRpcUserPool::table,SqlCommand::WITH_TRANSACTION);
    int rc = -1;
    
    sql.Clear();
    sql.Add("user_name",user->_snmp_user.User);
    rc = sql.Insert();

    return rc;
}

int SnmpXmlRpcUserPool::GetUser(SnmpXmlRpcUser* user)
{
    ostringstream   oss;
    int rc = -1;
    oss << "SELECT " << SnmpXmlRpcUserPool::columns << "FROM " << SnmpXmlRpcUserPool::table;
    SetCallback(static_cast<Callbackable::Callback>(&SnmpXmlRpcUserPool::SelectCallback),user);
    rc = db->Execute(oss, this);
    UnsetCallback();
    if ( 0 != rc && SQLDB_RESULT_EMPTY != rc) 
    {
        SkyAssert(false);
    }
    return rc;
}

int SnmpXmlRpcUserPool::DeleteTable()
{
    ostringstream oss;
    ostringstream oss1;
    int rc;

    oss << "truncate " << table;

    rc = db->Execute(oss);

/*    if ( rc == 0 )
    {
        set_valid(false);
    }
*/    

    return rc;
}



int SnmpXmlRpcUserPool::GetPasswordByUser(SnmpXmlRpcUser user,string& user_name)
{
    ostringstream   oss;
    int rc = -1;
    oss << "SELECT " << columns << " FROM " << table << " WHERE user_name = " << user_name;
    SetCallback(static_cast<Callbackable::Callback>(&SnmpXmlRpcUserPool::SelectCallback),&user);
    rc = db->Execute(oss, this);
    UnsetCallback();
    if ( 0 != rc && SQLDB_RESULT_EMPTY != rc) 
    {
        SkyAssert(false);
    }
    return rc;
}

int SnmpXmlRpcUserPool::SetPasswordByUser(string& user_name,SnmpXmlRpcUser user)
{
    SqlCommand sql(db,SnmpXmlRpcUserPool::table,SqlCommand::WITH_TRANSACTION);
    int rc = -1;
    rc = sql.Delete(" WHERE user_name = " + user._snmp_user.User);
    if (0 != rc && SQLDB_RESULT_EMPTY != rc) 
    {
        SkyAssert(false);
        return rc;
    }

    sql.Clear();
    sql.Add("user_name",user._snmp_user.User);
    rc = sql.Insert();
    if(rc != 0)
    {
        SkyAssert(false);
    }
    return rc;
}

/******************************************************************************/

} // namespace zte_tecs


