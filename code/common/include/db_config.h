/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�db_config.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ�����ݿ��ʼ��ģ��ͷ�ļ�
* ��ǰ�汾��1.0
* ��    �ߣ����Ľ�
* ������ڣ�2011��08��16��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/12/01
*     �� �� �ţ�V1.0
*     �� �� �ˣ����Ľ�
*     �޸����ݣ�����
*******************************************************************************/
#ifndef TECS_COMMON_DB_CONFIG_H
#define TECS_COMMON_DB_CONFIG_H
#include "sky.h"
#include "postgresql_db.h"
#include <pthread.h>

struct DBConfig
{
public:
    DBConfig()
    {
        server_port = 0;
        ignore_dbschema_ver = false;
    }

    void ShowSchemaVer()
    {
        //��һ�е�������ݺ͸�ʽ����Ҫ���Ǹ�service�ű�������
        //���Ҫ�Ķ�����ͬ���޸ķ���ű�
        cout << "dbschema_update_version : " << schema_update_version << endl;
        cout << "schema_bugfix_version : " << schema_bugfix_version << endl;
    }
    
    string  server_url;    //���ݿ��������ַ
    int     server_port;   //���ݿ�������˿ں�
    string  user;          //���ݿ�������û���
    string  passwd;        //���ݿ����������
    string  database;      //���ݿ�����
    string  schema_update_version; //���ݿ�schema update�汾��
    string  schema_bugfix_version; //���ݿ�schema bugfix�汾��
    bool    ignore_dbschema_ver;   //�Ƿ����������ݿ�ʱУ��schema�汾�Ƿ����

    friend ostream & operator << (ostream& os, const DBConfig& conf)
    {
        os << "database server url = " << conf.server_url << endl;
        os << "database server port = " << conf.server_port << endl;
        os << "database name = " << conf.database << endl;
        os << "database user = " << conf.user << endl;
        os << "database password = " << conf.passwd << endl;
        os << "database schema update version = " << conf.schema_update_version << endl;
        os << "database schema bugfix version = " << conf.schema_bugfix_version << endl;
        return os;
    };
};

STATUS DBInit(const DBConfig&) __attribute__ ((weak));
STATUS DBInitAdo(const DBConfig&)   __attribute__ ((weak));
CADODatabase *GetAdoDB()  __attribute__ ((weak));
void FreeAdoDB()  __attribute__ ((weak));

SqlDB *GetDB() __attribute__ ((weak));
SqlDB *GetPrivateDB() __attribute__ ((weak));

STATUS InitDbVacuum() __attribute__ ((weak));
//ע��һ�����ݿ�����Ĭ����5����һ�Σ�ע�����interval�ĵ�λ����!
STATUS RegisterDbVacuum(const string& table,int interval = 300)  __attribute__ ((weak)); 
STATUS VacuumTable(const string& table,SqlCommand::VacuumOption option)  __attribute__ ((weak)); 
#endif

