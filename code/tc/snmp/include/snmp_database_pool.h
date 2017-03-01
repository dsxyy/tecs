/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�current_alarm_pool.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��CurrentAlarmPoll�ඨ���ļ�
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


#ifndef SNMP_DATABASE_ALARM_POOL_H
#define SNMP_DATABASE_ALARM_POOL_H

#include "sky.h"
#include "sqldb.h"
#include "pool_sql.h"
#include "snmp_database.h"

//#include <time.h>
//#include <sstream>

//#include <iostream>

//#include <vector>

// ���ÿ�������궨��
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)
#endif


using namespace std;


namespace zte_tecs
{

class SnmpConfigurationPool : public PoolSQL
{

public:
    ~SnmpConfigurationPool(){};         // Ĭ����������

    int64 Allocate(int64* oid, const T_SnmpConfiguration& snmp_conf, string& error_str);

    SnmpConfiguration * Get(int64 oid, bool lock)
    {
        SnmpConfiguration * snmp = static_cast<SnmpConfiguration *>(PoolSQL::Get(oid,lock));

        return snmp;
    };
/*
    SnmpConfiguration * Get(string& alarmkey, bool lock)
    {
        map<string, int64>::iterator index;

        index = _known_alarms.find(alarmkey);

        if (_known_alarms.end() != index)
        {
            return Get((int64)index->second,lock);
        }

        return 0;
    };
*/
    int Update(SnmpConfiguration* snmp_conf)
    {
        return snmp_conf->Update(_db);
    };
/*
    int Drop(SnmpConfiguration* snmp_conf)
    {
        int rc = PoolSQL::Drop(snmp_conf);

        if (0 == rc)
        {
            _known_alarms.erase(cur_alarm->get_alarmkey());
        }

        return rc;
    };
*/    

    static void Bootstrap(SqlDB* db)
    {
        SnmpConfiguration::Bootstrap(db);
    };


    int Dump(ostringstream& oss, const string& where);
    
    
    int Dump(vector<xmlrpc_c::value>& arraySnmpConf, const string& where);
 
    static SnmpConfigurationPool *GetInstance()
    {
        return instance;
    };

    
    static SnmpConfigurationPool* CreateInstance(SqlDB *db)
    {
        if(NULL == instance)
        {
            instance = new SnmpConfigurationPool(db); 
        }
        
        return instance;
    };

private:

/*    map<string, int64> _known_alarms;  //�澯�ؼ���-���ݿ��¼IDӳ��*/
    SnmpConfigurationPool(SqlDB* db);   // Ĭ�Ϲ��캯��
    DISALLOW_COPY_AND_ASSIGN(SnmpConfigurationPool);
    static SnmpConfigurationPool *instance;

    PoolObjectSQL * Create()
    {
        return  new SnmpConfiguration();
    };

    int DumpCallback(void* oss, SqlColumn* columns);
    
    int DumpStructCallback(void *arraySnmpConf, SqlColumn* columns);

    int InitCallback(void* nil, SqlColumn* columns);

};

class TrapAddressPool : public PoolSQL
{

public:
    ~TrapAddressPool(){};         // Ĭ����������

    int64 Allocate(int64* oid, const T_TrapAddress& trap_address, string& error_str);

    TrapAddress * Get(int64 oid, bool lock)
    {
        TrapAddress * trap = static_cast<TrapAddress *>(PoolSQL::Get(oid,lock));

        return trap;
    };

    TrapAddress * Get(string& key, bool lock)
    {
        map<string, int64>::iterator index;

        index = _known_trap_address.find(key);

        if (_known_trap_address.end() != index)
        {
            return Get((int64)index->second,lock);
        }

        return 0;
    };

    int Update(TrapAddress* trap_address)
    {
        return trap_address->Update(_db);
    };

    int Drop(TrapAddress* trap_address)
    {
        int rc = PoolSQL::Drop(trap_address);
        string key;

        if (0 == rc)
        {
            _known_trap_address.erase(trap_address->get_key(key));
        }

        return rc;
    };
    

    static void Bootstrap(SqlDB* db)
    {
        TrapAddress::Bootstrap(db);
    };


    int Dump(ostringstream& oss, const string& where);
    
    
    int Dump(vector<xmlrpc_c::value>& arrayTrapAddress, const string& where);
 
    static TrapAddressPool *GetInstance()
    {
        return instance;
    };

    
    static TrapAddressPool* CreateInstance(SqlDB *db)
    {
        if(NULL == instance)
        {
            instance = new TrapAddressPool(db); 
        }
        
        return instance;
    };

private:

    map<string, int64> _known_trap_address;  //�澯�ؼ���-���ݿ��¼IDӳ��
    TrapAddressPool(SqlDB* db);   // Ĭ�Ϲ��캯��
    DISALLOW_COPY_AND_ASSIGN(TrapAddressPool);
    static TrapAddressPool *instance;

    PoolObjectSQL * Create()
    {
        return  new TrapAddress();
    };

    int DumpCallback(void* oss, SqlColumn* columns);
    
    int DumpStructCallback(void *arrayTrapAddress, SqlColumn* columns);

    int InitCallback(void* nil, SqlColumn* columns);

};

class SnmpXmlRpcUserPool : public Callbackable
{

public:
    ~SnmpXmlRpcUserPool(){};         // Ĭ����������

    int SelectCallback(void *nil, SqlColumn * columns);
    int DumpCallback(void* arraySnmpUser, SqlColumn * columns);
    int Dump(vector<xmlrpc_c::value>& arraySnmpUser, const string& where);
    int SetUser(SnmpXmlRpcUser* user);
    int GetUser(SnmpXmlRpcUser* user);
    int DeleteTable();
    
    int GetPasswordByUser(SnmpXmlRpcUser user,string& user_name);
    int SetPasswordByUser(string& user_name,const SnmpXmlRpcUser user);

    static int Bootstrap(SqlDB* db)
    {
        ostringstream oss(SnmpXmlRpcUserPool::db_bootstrap);
        return db->Execute(oss);
    };

 
    static SnmpXmlRpcUserPool *GetInstance()
    {
        return instance;
    };

    
    static SnmpXmlRpcUserPool* CreateInstance(SqlDB *db)
    {
        if(NULL == instance)
        {
            instance = new SnmpXmlRpcUserPool(db); 
        }
        
        return instance;
    };

private:

/*    map<string, int64> _known_alarms;  //�澯�ؼ���-���ݿ��¼IDӳ��*/
    // ���ݿ��ֶζ���
    enum ColNames
    {
        SNMPUSER  = 0,   // �û���
        LIMIT     = 1
    };
    
    SnmpXmlRpcUserPool(SqlDB* pDb)
    {
        db = pDb;
    };
    DISALLOW_COPY_AND_ASSIGN(SnmpXmlRpcUserPool);
    static SnmpXmlRpcUserPool *instance;
    SqlDB *db;
    static const char *table;
    static const char *columns;
    static const char *db_bootstrap;

};

}  // namespace zte_tecs
#endif // #ifndef SNMP_DATABASE_ALARM_POOL_H
