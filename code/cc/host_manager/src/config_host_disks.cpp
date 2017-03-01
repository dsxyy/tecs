/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�config_host_disks.cpp
* �ļ���ʶ��
* ����ժҪ��ConfigHostDisks��Ķ����ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�xiett
* ������ڣ�2013��7��25��
*
* �޸ļ�¼1��
*     �޸����ڣ�2013/7/25
*     �� �� �ţ�V1.0
*     �� �� �ˣ�xiett
*     �޸����ݣ�����
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
