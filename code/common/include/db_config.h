/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：db_config.h
* 文件标识：见配置管理计划书
* 内容摘要：数据库初始化模块头文件
* 当前版本：1.0
* 作    者：张文剑
* 完成日期：2011年08月16日
*
* 修改记录1：
*     修改日期：2011/12/01
*     版 本 号：V1.0
*     修 改 人：张文剑
*     修改内容：创建
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
        //这一行的输出内容和格式很重要，是给service脚本解析的
        //如果要改动，请同步修改服务脚本
        cout << "dbschema_update_version : " << schema_update_version << endl;
        cout << "schema_bugfix_version : " << schema_bugfix_version << endl;
    }
    
    string  server_url;    //数据库服务器地址
    int     server_port;   //数据库服务器端口号
    string  user;          //数据库服务器用户名
    string  passwd;        //数据库服务器密码
    string  database;      //数据库名称
    string  schema_update_version; //数据库schema update版本号
    string  schema_bugfix_version; //数据库schema bugfix版本号
    bool    ignore_dbschema_ver;   //是否在连接数据库时校验schema版本是否兼容

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
//注册一个数据库清理，默认是5分钟一次，注意入参interval的单位是秒!
STATUS RegisterDbVacuum(const string& table,int interval = 300)  __attribute__ ((weak)); 
STATUS VacuumTable(const string& table,SqlCommand::VacuumOption option)  __attribute__ ((weak)); 
#endif

