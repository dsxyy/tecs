/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：config_host_disks.cpp
* 文件标识：
* 内容摘要：ConfigHostDisks类的定义文件
* 当前版本：1.0
* 作    者：xiett
* 完成日期：2013年7月25日
*
* 修改记录1：
*     修改日期：2013/7/25
*     版 本 号：V1.0
*     修 改 人：xiett
*     修改内容：创建
*******************************************************************************/
#include "config_host_disks.h"

namespace zte_tecs{

const char *ConfigHostDisks::_table = "host_disks";

const char *ConfigHostDisks::_db_names =
    "hid, name, "
    "type, status ";

SqlDB *ConfigHostDisks::_db = NULL;

ConfigHostDisks *ConfigHostDisks::_instance = NULL;

/******************************************************************************/
ConfigHostDisks::ConfigHostDisks(SqlDB *pDb)
{
    _db = pDb;
}

/******************************************************************************/
int32 ConfigHostDisks::Insert(const DiskInfo& disk_info, int64 hid)
{
    SqlCommand sql(_db, _table);
    sql.Add("hid",    hid);
    sql.Add("name",   disk_info._name);
    sql.Add("type",   disk_info._type);
    sql.Add("status", disk_info._status);
    return sql.Insert();
}

/******************************************************************************/
int32 ConfigHostDisks::Delete(int64 hid)
{
    SqlCommand sql(_db,  _table);
    sql.Add("hid",  hid);

    ostringstream where ;
    where << " WHERE hid = " << hid;        
    return sql.Delete(where.str());
}

}
