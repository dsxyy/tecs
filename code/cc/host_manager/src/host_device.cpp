/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：host_device.cpp
* 文件标识：
* 内容摘要：host_device类的定义文件
* 当前版本：1.0
* 作    者：姚远
* 完成日期：2012年11月1日
*
* 修改记录1：
*     修改日期：2012/11/1
*     版 本 号：V1.0
*     修 改 人：姚远
*     修改内容：创建
*******************************************************************************/
#include "host_device.h"


namespace zte_tecs {

/******************************************************************************/
const char * HostDevicePool::_table_name = 
        "host_device";  //虚拟机 上下文
        
const char * HostDevicePool::_col_names = 
        "hid,"
        "type,"
        "number,"
        "description";
       

HostDevicePool *HostDevicePool::_instance = NULL;

/******************************************************************************/
int HostDevicePool::SelectCallback(void *nil, SqlColumn * columns)
{
    if ( NULL == columns || LIMIT != columns->get_column_num())
    {
        return -1;
    }
    
    PciPthDevInfo device;
    columns->GetValue(TYPE,    (int&)device._type);
    columns->GetValue(NUMBER,   (int&)device._number);
    columns->GetValue(DESCRIPTION,   device._description);

    vector<PciPthDevInfo>  *p  = static_cast<vector<PciPthDevInfo> *>(nil);
    p->push_back(device);
    
    return 0;
}

/******************************************************************************/
int HostDevicePool::GetHostDevices(vector<PciPthDevInfo> &devices, int64 hid)
{
    ostringstream   oss;
    int             rc;

    oss << "SELECT " << _col_names 
        << " FROM " << _table_name 
        << " WHERE hid = " << hid << " order by type desc ";

    SetCallback(static_cast<Callbackable::Callback>(&HostDevicePool::SelectCallback),&devices);
    rc = db->Execute(oss, this);
    UnsetCallback();
    if ( 0 != rc && SQLDB_RESULT_EMPTY != rc) 
    {
        SkyAssert(false);
    }
    return rc;
}

/******************************************************************************/
int HostDevicePool::InsertHostDevices(const vector<PciPthDevInfo> &devices, int64 hid)
{
    int rc = 0;

    if (devices.empty())
    {
        return 0;
    }

    SqlCommand    sql(db, SqlCommand::WITH_TRANSACTION);
    sql.SetTable(HostDevicePool::_table_name);
    
    vector<PciPthDevInfo>::const_iterator  it;
    for (it  = devices.begin(); 
         it != devices.end();
         it++ )
    {
        sql.Clear();
        sql.Add("hid",          hid);
        sql.Add("type",         it->_type);
        sql.Add("number",       it->_number);
        sql.Add("description",  it->_description);

        rc = sql.Insert();
        if(rc != 0)
        {
            SkyAssert(false);
            break;
        }
    }

    return rc;
}

/******************************************************************************/
int HostDevicePool::DropHostDevices(const vector<PciPthDevInfo> &devices, int64 hid)
{
    int rc = 0;

    if (devices.empty())
    {
        return 0;
    }
    
    SqlCommand    sql(db, SqlCommand::WITH_TRANSACTION);
    sql.SetTable(HostDevicePool::_table_name);
    
    vector<PciPthDevInfo>::const_iterator  it;
    for (it  = devices.begin(); 
         it != devices.end();
         it++ )
    {
        rc = DropHostDevice(*it, hid);
        if(rc != 0)
        {
            SkyAssert(false);
            break;
        }
    }

    return rc;
}

/******************************************************************************/
int HostDevicePool::DropHostDevice(const PciPthDevInfo &device, int64 hid)
{
    int rc = 0;
    ostringstream oss;

    oss << "DELETE FROM " << _table_name << " WHERE hid = " << hid 
        << "and type = " << device._type;

    if(Transaction::On())
    {
        rc = Transaction::Append(oss.str());
    }
    else
    {
        rc = db->Execute(oss);
    }

    SkyAssert(0 == rc);

    return rc;
}


} // namespace zte_tecs

