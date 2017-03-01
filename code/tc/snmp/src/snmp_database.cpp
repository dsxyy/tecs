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


#include "snmp_database.h"

#include <sstream>
#include "db_config.h"


namespace zte_tecs
{

/* ************************************************************************** */
/* SnmpConfiguration :: Database Access Functions                                  */
/* ************************************************************************** */

const char * SnmpConfiguration::_table = "snmp_configuration";

const char * SnmpConfiguration::_db_names = "oid,community,trap_community,trap_version";

const char * SnmpConfiguration::_db_bootstrap = "CREATE TABLE IF NOT EXISTS "
    "snmp_configuration (oid numeric(19) PRIMARY KEY, community VARCHAR(64), trap_community "
    "VARCHAR(64), trap_version INTEGER);";

const char * TrapAddress::_table = "trap_address";

const char * TrapAddress::_db_names = "oid,ip_address,port";

const char * TrapAddress::_db_bootstrap = "CREATE TABLE IF NOT EXISTS "
    "trap_address (oid numeric(19) PRIMARY KEY, ip_address VARCHAR(64), port "
    "INTEGER, UNIQUE(ip_address), UNIQUE(port));";


ostream& operator<<(ostream& os, SnmpConfiguration& snmpconf)
{
    string snmpconf_str;

    os << snmpconf.to_xml(snmpconf_str);

    return os;
}


/******************************************************************************/

SnmpConfiguration::SnmpConfiguration(int64 id, const T_SnmpConfiguration& snmp_conf) :
    PoolObjectSQL(id), _snmp_conf(snmp_conf)
{

}


string& SnmpConfiguration::to_str(string& str) const
{

    ostringstream   os;

    os <<
        "ID            = "  << _oid                      << endl <<
        "COMMNITY      = "  << _snmp_conf.Community      << endl <<
        "TRAPCOMMUNITY = "  << _snmp_conf.TrapCommunity  << endl <<
        "TRAPVERSION   = "  << _snmp_conf.TrapVersion    << endl;

    str = os.str();

    return str;
}


/******************************************************************************/

string& SnmpConfiguration::to_xml(string& xml) const
{

    ostringstream   oss;

    oss <<
    "<SNMPCONFIGURATION>"
         "<ID>"            << _oid                      <<"</ID>"             <<
         "<COMMNITY>"      << _snmp_conf.Community      <<"</COMMNITY"        <<
         "<TRAPCOMMUNITY>" << _snmp_conf.TrapCommunity  <<"</TRAPCOMMUNITY>"  <<
         "<TRAPVERSION>"   << _snmp_conf.TrapVersion    <<"</TRAPVERSION>"    <<
    "</SNMPCONFIGURATION>";

    xml = oss.str();

    return xml;
}

/******************************************************************************/

int SnmpConfiguration::Insert_Replace(SqlDB* db, bool replace)
{

    SqlCommand sql(db, _table);
    sql.Add("oid",  _oid);
    sql.Add("community", _snmp_conf.Community);
    sql.Add("trap_community", _snmp_conf.TrapCommunity);
    sql.Add("trap_version", _snmp_conf.TrapVersion);
    
    if(replace)
    {
       ostringstream where;
       where << "WHERE oid = " << _oid;
       
       return sql.Update(where.str());
    }
    else
    {
       return sql.Insert();
    }

    return 0 ;
}


/******************************************************************************/

int SnmpConfiguration::SelectCallback(void* nil, SqlColumn * columns)
{
    if (NULL == columns || LIMIT != columns->get_column_num())
    {
        return -1;
    }

    columns->GetValue(OID, _oid);
    columns->GetValue(COMMUNITY, _snmp_conf.Community);
    columns->GetValue(TRAPCOMMUNITY, _snmp_conf.TrapCommunity);
    columns->GetValue(TRAPVERSION, _snmp_conf.TrapVersion);
    
    return 0;

}


/******************************************************************************/

int SnmpConfiguration::Select(SqlDB* db)
{
    ostringstream   oss;
    int             rc;
    int64           boid;

    SetCallback(static_cast<Callbackable::Callback>(
        &SnmpConfiguration::SelectCallback));

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

int SnmpConfiguration::Insert(SqlDB* db, string& error_str)
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

int SnmpConfiguration::Update(SqlDB* db)
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

int SnmpConfiguration::Drop(SqlDB* db)
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

int SnmpConfiguration::DeleteTable(SqlDB* db)
{
    ostringstream oss;
    ostringstream oss1;
    int rc;

    oss << "truncate " << _table;

    rc = db->Execute(oss);

/*    if ( rc == 0 )
    {
        set_valid(false);
    }
*/    
    oss1 << "drop table " << _table;
	
    rc = db->Execute(oss1);
/* 
    if ( rc == 0 )
    {
        set_valid(false);
    }
*/ 	

    return rc;
}


/******************************************************************************/

int SnmpConfiguration::Dump(ostringstream& oss, SqlColumn* columns)
{
    if (NULL == columns || LIMIT != columns->get_column_num())
    {
        return -1;
    }

    oss <<
        "<SNMPCONFIGURATION>" <<
            "<ID>"            << columns->GetValueInt64(OID)          <<"</ID>"            <<
            "<COMMUNITY>"     << columns->GetValueText(COMMUNITY)     <<"</COMMUNITY>"     <<
            "<TRAPCOMMUNITY>" << columns->GetValueText(TRAPCOMMUNITY) <<"</TRAPCOMMUNITY>" <<
            "<TRAPVERSION>"   << columns->GetValueInt(TRAPVERSION)    <<"</TRAPVERSION>"   <<
        "</SNMPCONFIGURATION>";

    return 0;

}


int SnmpConfiguration::Dump(vector<xmlrpc_c::value>& arraySnmpConf, SqlColumn* columns)
{
    if (NULL == columns || LIMIT != columns->get_column_num())
    {
        return -1;
    }
    
    map<string, xmlrpc_c::value> oneSnmpConf;
    oneSnmpConf.insert(make_pair("community", xmlrpc_c::value_string(columns->GetValueText(COMMUNITY))));
    oneSnmpConf.insert(make_pair("trap_community", xmlrpc_c::value_string(columns->GetValueText(TRAPCOMMUNITY))));
    oneSnmpConf.insert(make_pair("trap_version", xmlrpc_c::value_int(columns->GetValueInt(TRAPVERSION))));
    arraySnmpConf.push_back(xmlrpc_c::value_struct(oneSnmpConf));

    return 0;

}

/******************************************************************************/


ostream& operator<<(ostream& os, TrapAddress& trapaddress)
{
    string trapaddress_str;

    os << trapaddress.to_xml(trapaddress_str);

    return os;
}


/******************************************************************************/

TrapAddress::TrapAddress(int64 id, const T_TrapAddress& trap_address) :
    PoolObjectSQL(id), _trap_address(trap_address)
{

}


string& TrapAddress::to_str(string& str) const
{

    ostringstream   os;

    os <<
        "ID            = "  << _oid                    << endl <<
        "IPADDRESS     = "  << _trap_address.Address   << endl <<
        "PORT          = "  << _trap_address.Port      << endl;

    str = os.str();

    return str;
}


/******************************************************************************/

string& TrapAddress::to_xml(string& xml) const
{

    ostringstream   oss;

    oss <<
    "<SNMPTRAPADDRESS>"
         "<ID>"        << _oid                  <<"</ID>"          <<
         "<IPADDRESS>" << _trap_address.Address <<"</IPADDRESS>"   <<
         "<PORT>"      << _trap_address.Port    <<"</PORT>"        <<
    "</SNMPTRAPADDRESS>";

    xml = oss.str();

    return xml;
}

/******************************************************************************/

int TrapAddress::Insert_Replace(SqlDB* db, bool replace)
{
    SqlCommand sql(db, _table);
    sql.Add("oid",  _oid);
    sql.Add("ip_address", _trap_address.Address);
    sql.Add("port",    _trap_address.Port );
    
    if(replace)
    {
       ostringstream where;
       where << "WHERE oid = " << _oid;
       
       return sql.Update(where.str());
    }
    else
    {
       return sql.Insert();
    }

    return 0 ;
    
}


/******************************************************************************/

int TrapAddress::SelectCallback(void* nil, SqlColumn * columns)
{
    if (NULL == columns || LIMIT != columns->get_column_num())
    {
        return -1;
    }

    columns->GetValue(OID, _oid);
    columns->GetValue(ADDRESS, _trap_address.Address);
    columns->GetValue(PORT, _trap_address.Port);
    
    return 0;

}


/******************************************************************************/

int TrapAddress::Select(SqlDB* db)
{
    ostringstream   oss;
    int             rc;
    int64           boid;

    SetCallback(static_cast<Callbackable::Callback>(
        &TrapAddress::SelectCallback));

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

int TrapAddress::Insert(SqlDB* db, string& error_str)
{
    int rc;

    rc = Insert_Replace(db, false);

    if (0 != rc)
    {
        error_str = "Error inserting TrapAddress in DB.";
        return rc;
    }

    return 0;
}


/******************************************************************************/

int TrapAddress::Update(SqlDB* db)
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

int TrapAddress::Drop(SqlDB* db)
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

int TrapAddress::DeleteTable(SqlDB* db)
{
    ostringstream oss;
    ostringstream oss1;
    int rc;

    oss << "truncate " << _table;

    rc = db->Execute(oss);

/*    if ( rc == 0 )
    {
        set_valid(false);
    }
*/    
    oss1 << "drop table " << _table;
	
    rc = db->Execute(oss1);
/* 
    if ( rc == 0 )
    {
        set_valid(false);
    }
*/ 	

    return rc;
}


/******************************************************************************/

int TrapAddress::Dump(ostringstream& oss, SqlColumn* columns)
{
    if (NULL == columns || LIMIT != columns->get_column_num())
    {
        return -1;
    }

    oss <<
        "<TRAPADDRESS>" <<
            "<ID>"      << columns->GetValueInt64(OID)    <<"</ID>"     <<
            "<ADDRESS>" << columns->GetValueInt(ADDRESS)  <<"</ADDRESS>"<<
            "<PORT>"    << columns->GetValueInt(PORT)     <<"</PORT>"   <<
        "</TRAPADDRESS>";

    return 0;

}


int TrapAddress::Dump(vector<xmlrpc_c::value>& arrayTrapAddress, SqlColumn* columns)
{
    if (NULL == columns || LIMIT != columns->get_column_num())
    {
        return -1;
    }
    
    map<string, xmlrpc_c::value> oneTrapAddress;
    oneTrapAddress.insert(make_pair("ip_address", xmlrpc_c::value_string(columns->GetValueText(ADDRESS))));
    oneTrapAddress.insert(make_pair("port", xmlrpc_c::value_int(columns->GetValueInt(PORT))));
    arrayTrapAddress.push_back(xmlrpc_c::value_struct(oneTrapAddress));

    return 0;

}

string& TrapAddress::get_key(string& key) const
{
    ostringstream   oss;
    
    oss << _trap_address.Address << ":" << _trap_address.Port ;
    key = oss.str();
    return key;
};

/******************************************************************************/

ostream& operator<<(ostream& os, SnmpXmlRpcUser& snmpuser)
{
    string snmpuser_str;

    os << snmpuser.to_xml(snmpuser_str);

    return os;
}


/******************************************************************************/

SnmpXmlRpcUser::SnmpXmlRpcUser(const T_SnmpXmlRpcUser& snmp_user) :
    _snmp_user(snmp_user)
{

}


string& SnmpXmlRpcUser::to_str(string& str) const
{

    ostringstream   os;

    os <<
        "SNMPUSER = "  << _snmp_user.User      << endl;

    str = os.str();

    return str;
}


/******************************************************************************/

string& SnmpXmlRpcUser::to_xml(string& xml) const
{

    ostringstream   oss;

    oss <<
    "<SNMPXMLRPCUSER>"
         "<SNMPUSER>" << _snmp_user.User      <<"</SNMPUSER>"  <<
    "</SNMPXMLRPCUSER>";

    xml = oss.str();

    return xml;
}
#if 0
/******************************************************************************/

int SnmpXmlRpcUser::Insert_Replace(SqlDB* db, bool replace)
{
    SqlCommand sql(db, _table);
    sql.Add("user_name",  _snmp_user.User);
    sql.Add("password",  _snmp_user.Password );
    
    if(replace)
    {
       ostringstream where;
       where << "WHERE user_name = " << _snmp_user.User;
       
       return sql.Update(where.str());
    }
    else
    {
       return sql.Insert();
    }

    return 0 ;
    
}


/******************************************************************************/

int SnmpXmlRpcUser::SelectCallback(void* nil, SqlColumn * columns)
{
    if (NULL == columns || LIMIT != columns->get_column_num())
    {
        return -1;
    }

    columns->GetValue(SNMPUSER,  _snmp_user.User);
    columns->GetValue(PASSWORD, _snmp_user.Password);      
    
    return 0;

}


/******************************************************************************/

int SnmpXmlRpcUser::Select(SqlDB* db)
{
    ostringstream   oss;
    int             rc;

    SetCallback(static_cast<Callbackable::Callback>(
        &SnmpXmlRpcUser::SelectCallback));

    oss << "SELECT " << _db_names << " FROM " << _table << " WHERE user_name = " <<_snmp_user.User;

    rc = db->Execute(oss, this);

    UnsetCallback();

    if (0 != rc)
    {
        return -1;
    }

    return 0;
}


/******************************************************************************/

int SnmpXmlRpcUser::Insert(SqlDB* db, string& error_str)
{
    int rc;

    rc = Insert_Replace(db, false);

    if (0 != rc)
    {
        error_str = "Error inserting SnmpXmlRpcUser in DB.";
        return rc;
    }

    return 0;
}


/******************************************************************************/

int SnmpXmlRpcUser::Update(SqlDB* db)
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

int SnmpXmlRpcUser::Drop(SqlDB* db)
{
    ostringstream oss;
    int rc;

    oss << "DELETE FROM " << _table << " WHERE user_name = " <<_snmp_user.User;;

    rc = db->Execute(oss);

    if (0 == rc)
    {
        set_valid(false);
    }

    return rc;
}

/******************************************************************************/

int SnmpXmlRpcUser::DeleteTable(SqlDB* db)
{
    ostringstream oss;
    ostringstream oss1;
    int rc;

    oss << "truncate " << _table;

    rc = db->Execute(oss);

/*    if ( rc == 0 )
    {
        set_valid(false);
    }
*/    
    oss1 << "drop table " << _table;
	
    rc = db->Execute(oss1);
/* 
    if ( rc == 0 )
    {
        set_valid(false);
    }
*/ 	

    return rc;
}


/******************************************************************************/

int SnmpXmlRpcUser::Dump(ostringstream& oss, SqlColumn* columns)
{
    if (NULL == columns || LIMIT != columns->get_column_num())
    {
        return -1;
    }

    oss <<
        "<SNMPXMLRPCUSER>" <<
            "<SNMPUSER>" << columns->GetValueText(SNMPUSER) <<"</SNMPUSER>" <<
            "<PASSWORD>" << columns->GetValueText(PASSWORD) <<"</PASSWORD>" <<
        "</SNMPXMLRPCUSER>";

    return 0;

}


int SnmpXmlRpcUser::Dump(vector<xmlrpc_c::value>& arraySnmpUser, SqlColumn* columns)
{
    if (NULL == columns || LIMIT != columns->get_column_num())
    {
        return -1;
    }
    
    map<string, xmlrpc_c::value> oneSnmpUser;
    oneSnmpUser.insert(make_pair("user_name", xmlrpc_c::value_string(columns->GetValueText(SNMPUSER))));
    oneSnmpUser.insert(make_pair("password", xmlrpc_c::value_string(columns->GetValueText(PASSWORD))));
    arraySnmpUser.push_back(xmlrpc_c::value_struct(oneSnmpUser));

    return 0;

}
#endif
}   // namespace zte_tecs

