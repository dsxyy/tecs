/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�current_alarm.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��CurrentAlarm�ඨ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�������
* ������ڣ�2011��7��27��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/7/27
*     �� �� �ţ�V1.0
*     �� �� �ˣ�������
*     �޸����ݣ�����
*******************************************************************************/


#ifndef SNMP_DATABASE_ALARM_H
#define SNMP_DATABASE_ALARM_H

#include "sky.h"
#include "sqldb.h"
#include "pool_object_sql.h"
#include "tecs_rpc_method.h"

//#include <sstream>
//#include <ctime>

// ���ÿ�������궨��
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)
#endif

using namespace std;


namespace zte_tecs
{

// T_CurrentAlarm���ݽṹ����
struct T_SnmpConfiguration
{
    string  Community;
    string  TrapCommunity;
    int     TrapVersion;
    
    // ���췽��

    T_SnmpConfiguration (const T_SnmpConfiguration& sc) :
                    Community(sc.Community),
                    TrapCommunity(sc.TrapCommunity),
                    TrapVersion(sc.TrapVersion){};

    // ���췽��
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
    
    // ���췽��

    T_TrapAddress (const T_TrapAddress& ta) :
                    Address(ta.Address),
                    Port(ta.Port){};

    // ���췽��
    T_TrapAddress (
                    const string& aAddresss = "",
                    int   aPort = 0) :
                    Address(aAddresss),
                    Port(aPort){};

};

struct T_SnmpXmlRpcUser
{
    string  User;
    
    // ���췽��

    T_SnmpXmlRpcUser (const T_SnmpXmlRpcUser& su) :
                    User(su.User){};

    // ���췽��
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

   // Ĭ�Ϲ��캯��
    SnmpConfiguration(int64 id = -1, const T_SnmpConfiguration& current_conf = (string)"");
    // Ĭ����������
    virtual ~SnmpConfiguration(){};

    // ���ݿ��ֶζ���
    enum ColNames
    {
        OID            = 0,   // �����ʶ
        COMMUNITY      = 1,   // �����崮
        TRAPCOMMUNITY  = 2,   // TRAP�����崮
        TRAPVERSION    = 3,   // TRAP�汾��
        LIMIT          = 4
    };

    static const char* _db_names;       // ���ݿ��ֶζ���
    static const char* _db_bootstrap;   // ���ݿⴴ���ű�����
    static const char* _table;          // ���ݿ����

    virtual int Select(SqlDB* db);

    virtual int Insert(SqlDB* db, string& error_str);

    virtual int Update(SqlDB* db);

    virtual int Drop(SqlDB* db);
    
    static int DeleteTable(SqlDB* db);

    static  int Dump(ostringstream& oss, SqlColumn * columns);
    
    static  int Dump(vector<xmlrpc_c::value>& arraySnmpConf, SqlColumn* columns);

private:

    friend class SnmpConfigurationPool;

    T_SnmpConfiguration _snmp_conf;     // ��ǰSNMP���ýṹ

    int Insert_Replace(SqlDB* db, bool replace);

    int SelectCallback(void* nil, SqlColumn * columns);

    static void Bootstrap(SqlDB* db)
    {
        ostringstream oss_snmp_conf(SnmpConfiguration::_db_bootstrap);

        db->Execute(oss_snmp_conf);

    };

    // ���ÿ�������
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

   // Ĭ�Ϲ��캯��
    TrapAddress(int64 id = -1, const T_TrapAddress& trap_address = (string)"");
    // Ĭ����������
    virtual ~TrapAddress(){};

    // ���ݿ��ֶζ���
    enum ColNames
    {
        OID            = 0,   // �����ʶ
        ADDRESS        = 1,   // �ϱ���ַ
        PORT           = 2,   // �˿ں�
        LIMIT          = 3
    };

    static const char* _db_names;       // ���ݿ��ֶζ���
    static const char* _db_bootstrap;   // ���ݿⴴ���ű�����
    static const char* _table;          // ���ݿ����

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

    // ���ÿ�������
    DISALLOW_COPY_AND_ASSIGN(TrapAddress);
};

class SnmpXmlRpcUser
{

public:

    friend ostream& operator<<(ostream& os, SnmpXmlRpcUser& snmpuser);
    
    // Ĭ�Ϲ��캯��
    SnmpXmlRpcUser(const T_SnmpXmlRpcUser& snmp_user = (string)"");

    string& to_str(string& str) const;

    string& to_xml(string& xml) const;
    
    T_SnmpXmlRpcUser _snmp_user;     // ��ǰSNMP XMLRPC�û�

    const T_SnmpXmlRpcUser & get_current_user() const
    {
        return _snmp_user;
    }
    
    void set_current_user(const T_SnmpXmlRpcUser & snmp_user)
    {
        _snmp_user = snmp_user;
    }
     
    // Ĭ����������
    virtual ~SnmpXmlRpcUser(){};



};

}  //namespace zte_tecs

#endif // #ifndefALARM_SNMP_DATABASE_H

