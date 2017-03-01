/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：current_alarm.h
* 文件标识：见配置管理计划书
* 内容摘要：CurrentAlarm类定义文件
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


#ifndef SNMP_DATABASE_ALARM_H
#define SNMP_DATABASE_ALARM_H

#include "sky.h"
#include "sqldb.h"
#include "pool_object_sql.h"
#include "tecs_rpc_method.h"

//#include <sstream>
//#include <ctime>

// 禁用拷贝构造宏定义
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)
#endif

using namespace std;


namespace zte_tecs
{

// T_CurrentAlarm数据结构定义
struct T_SnmpConfiguration
{
    string  Community;
    string  TrapCommunity;
    int     TrapVersion;
    
    // 构造方法

    T_SnmpConfiguration (const T_SnmpConfiguration& sc) :
                    Community(sc.Community),
                    TrapCommunity(sc.TrapCommunity),
                    TrapVersion(sc.TrapVersion){};

    // 构造方法
    T_SnmpConfiguration (
                    const string&  aCommunity = "",
                    const string&  aTrapcommunity = "",
                    int   aTrapversion = 0) :
                    Community(aCommunity),
                    TrapCommunity(aTrapcommunity),
                    TrapVersion(aTrapversion){};

};

struct T_TrapAddress
{
    string  Address;
    int     Port;
    
    // 构造方法

    T_TrapAddress (const T_TrapAddress& ta) :
                    Address(ta.Address),
                    Port(ta.Port){};

    // 构造方法
    T_TrapAddress (
                    const string& aAddresss = "",
                    int   aPort = 0) :
                    Address(aAddresss),
                    Port(aPort){};

};

struct T_SnmpXmlRpcUser
{
    string  User;
    
    // 构造方法

    T_SnmpXmlRpcUser (const T_SnmpXmlRpcUser& su) :
                    User(su.User){};

    // 构造方法
    T_SnmpXmlRpcUser (
                    const string&  aUser = "") :
                    User(aUser){};

};

class SnmpConfiguration : public PoolObjectSQL
{

public:

    friend ostream& operator<<(ostream& os, SnmpConfiguration& snmpconf);

    string& to_str(string& str) const;

    string& to_xml(string& xml) const;

    int64 get_id() const
    {
        return _oid;
    };
    
    const T_SnmpConfiguration & get_current_conf() const
    {
        return _snmp_conf;
    }
    
    void set_current_conf(const T_SnmpConfiguration & current_conf)
    {
        _snmp_conf = current_conf;
    }


protected:

   // 默认构造函数
    SnmpConfiguration(int64 id = -1, const T_SnmpConfiguration& current_conf = (string)"");
    // 默认析构函数
    virtual ~SnmpConfiguration(){};

    // 数据库字段定义
    enum ColNames
    {
        OID            = 0,   // 对象标识
        COMMUNITY      = 1,   // 联合体串
        TRAPCOMMUNITY  = 2,   // TRAP联合体串
        TRAPVERSION    = 3,   // TRAP版本号
        LIMIT          = 4
    };

    static const char* _db_names;       // 数据库字段定义
    static const char* _db_bootstrap;   // 数据库创建脚本定义
    static const char* _table;          // 数据库表定义

    virtual int Select(SqlDB* db);

    virtual int Insert(SqlDB* db, string& error_str);

    virtual int Update(SqlDB* db);

    virtual int Drop(SqlDB* db);
    
    static int DeleteTable(SqlDB* db);

    static  int Dump(ostringstream& oss, SqlColumn * columns);
    
    static  int Dump(vector<xmlrpc_c::value>& arraySnmpConf, SqlColumn* columns);

private:

    friend class SnmpConfigurationPool;

    T_SnmpConfiguration _snmp_conf;     // 当前SNMP配置结构

    int Insert_Replace(SqlDB* db, bool replace);

    int SelectCallback(void* nil, SqlColumn * columns);

    static void Bootstrap(SqlDB* db)
    {
        ostringstream oss_snmp_conf(SnmpConfiguration::_db_bootstrap);

        db->Execute(oss_snmp_conf);

    };

    // 禁用拷贝构造
    DISALLOW_COPY_AND_ASSIGN(SnmpConfiguration);
};

class TrapAddress : public PoolObjectSQL
{

public:

    friend ostream& operator<<(ostream& os, TrapAddress& trap_address);

    string& to_str(string& str) const;

    string& to_xml(string& xml) const;
    
    string& get_key(string& key) const;

    int64 get_id() const
    {
        return _oid;
    };
    
    const T_TrapAddress & get_current_address() const
    {
        return _trap_address;
    }
    
    void set_current_address(const T_TrapAddress & trap_address)
    {
        _trap_address = trap_address;
    }
    

protected:

   // 默认构造函数
    TrapAddress(int64 id = -1, const T_TrapAddress& trap_address = (string)"");
    // 默认析构函数
    virtual ~TrapAddress(){};

    // 数据库字段定义
    enum ColNames
    {
        OID            = 0,   // 对象标识
        ADDRESS        = 1,   // 上报地址
        PORT           = 2,   // 端口号
        LIMIT          = 3
    };

    static const char* _db_names;       // 数据库字段定义
    static const char* _db_bootstrap;   // 数据库创建脚本定义
    static const char* _table;          // 数据库表定义

    virtual int Select(SqlDB* db);

    virtual int Insert(SqlDB* db, string& error_str);

    virtual int Update(SqlDB* db);

    virtual int Drop(SqlDB* db);
    
    static int DeleteTable(SqlDB* db);

    static  int Dump(ostringstream& oss, SqlColumn * columns);
    
    static  int Dump(vector<xmlrpc_c::value>& arrayTrapAddress, SqlColumn* columns);

private:

    friend class TrapAddressPool;

    T_TrapAddress _trap_address;

    int Insert_Replace(SqlDB* db, bool replace);

    int SelectCallback(void* nil, SqlColumn * columns);

    static void Bootstrap(SqlDB* db)
    {
        ostringstream oss_trap_address(TrapAddress::_db_bootstrap);

        db->Execute(oss_trap_address);

    };

    // 禁用拷贝构造
    DISALLOW_COPY_AND_ASSIGN(TrapAddress);
};

class SnmpXmlRpcUser
{

public:

    friend ostream& operator<<(ostream& os, SnmpXmlRpcUser& snmpuser);
    
    // 默认构造函数
    SnmpXmlRpcUser(const T_SnmpXmlRpcUser& snmp_user = (string)"");

    string& to_str(string& str) const;

    string& to_xml(string& xml) const;
    
    T_SnmpXmlRpcUser _snmp_user;     // 当前SNMP XMLRPC用户

    const T_SnmpXmlRpcUser & get_current_user() const
    {
        return _snmp_user;
    }
    
    void set_current_user(const T_SnmpXmlRpcUser & snmp_user)
    {
        _snmp_user = snmp_user;
    }
     
    // 默认析构函数
    virtual ~SnmpXmlRpcUser(){};



};

}  //namespace zte_tecs

#endif // #ifndefALARM_SNMP_DATABASE_H

