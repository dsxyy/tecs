/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：config_tcu.cpp
* 文件标识：
* 内容摘要：ConfigTcu类的定义文件
* 当前版本：1.0
* 作    者：袁进坤
* 完成日期：2011年8月21日
*
* 修改记录1：
*     修改日期：2011/8/21
*     版 本 号：V1.0
*     修 改 人：袁进坤
*     修改内容：创建
*******************************************************************************/
#include "config_tcu.h"
#include "host_cpuinfo.h"
#include "sky.h"
#include "db_config.h"
#include "pool_sql.h"
namespace zte_tecs {

/* tcu配置使用的表名称 */
const char *ConfigTcu::_table = "config_tcu";

/* tcu配置中的表字段 */
const char *ConfigTcu::_view_config_tcu_select_names = 
    "cpu_info_id, processor_num, system_tcu_num,config_tcu_num,description";

/* tcu配置中的表字段 */
const char *ConfigTcu::_db_names = 
    "cpu_info_id, tcu_num, description";

const char *ConfigTcu::_view = "config_tcu_select";

SqlDB *ConfigTcu::_db = NULL;

ConfigTcu *ConfigTcu::_instance = NULL;

/******************************************************************************/
ConfigTcu::ConfigTcu(SqlDB *pDb)
{
    _db = pDb;
}

/******************************************************************************/
int32 ConfigTcu::SelectCallback(void *nil, SqlColumn *columns)
{
    if ((NULL == nil) || (NULL == columns) )
    {
        return -1;
    }

    TcuInfoType *tcu_info = static_cast<TcuInfoType *>(nil);

    columns->GetValue(0, tcu_info->_cpu_info_id);
    columns->GetValue(2, tcu_info->_commend_tcu_num);
    columns->GetValue(3, tcu_info->_tcu_num);
    columns->GetValue(4, tcu_info->_description);

    cout<<"SelectCallback _commend_tcu_num = "<<tcu_info->_commend_tcu_num<<" !"<<endl;
    
    return 0;
}

/******************************************************************************/ 
int32 ConfigTcu::Get(int64 cpu_info_id, TcuInfoType &tcu_info)
{
    ostringstream sql;
    sql << "SELECT " << _view_config_tcu_select_names << " FROM " << _view
        << " WHERE cpu_info_id = " << cpu_info_id;

    SetCallback(static_cast<Callbackable::Callback>(
                &ConfigTcu::SelectCallback),
                static_cast<void *>(&tcu_info));
    int32 rc = _db->Execute(sql, this);

    UnsetCallback();

    if (0 != rc)
    {
        return rc;
    }

    HostCpuInfo *host_cpu_info = HostCpuInfo::GetInstance();
    if (NULL != host_cpu_info)
    {
        host_cpu_info->Dump(cpu_info_id, tcu_info._cpu_info);
    }
    return 0;
}

/******************************************************************************/ 
int32 ConfigTcu::Set(int64 cpu_info_id, int32 tcu_num, 
                       const string &description)
{
    InsertReplace(cpu_info_id, tcu_num, description, false);
    return InsertReplace(cpu_info_id, tcu_num, description, true);
}

/******************************************************************************/
int32 ConfigTcu::InsertReplace(int64 cpu_info_id, int32 tcu_num, 
                                 const string &description, bool replace)
{ 
    SqlCommand sql(_db,  _table);
    sql.Add("cpu_info_id",  cpu_info_id);
    sql.Add("tcu_num",     tcu_num);
    sql.Add("description", description);

    if (replace)
    {
        ostringstream where ;
        where << " WHERE cpu_info_id = " << cpu_info_id;        
        return sql.Update(where.str());
    }
    else
    {
        return sql.Insert();     
    }

    return -1;
}

/******************************************************************************/
int32 ConfigTcu::Drop(int64 cpu_info_id)
{
    ostringstream oss;
    oss << "DELETE FROM " << _table << " WHERE cpu_info_id = " << cpu_info_id;
    
    int32 rc = _db->Execute(oss);
    
    if (0 != rc)
    {
        return rc;
    }

    HostCpuInfo *host_cpu_info = HostCpuInfo::GetInstance();
    if (NULL != host_cpu_info)
    {
        host_cpu_info->Drop(cpu_info_id);
    }
    
    return 0;
}

/******************************************************************************/ 
int32 ConfigTcu::Get(const map<string, string> &cpu_info, 
                       TcuInfoType &tcu_info)
{
    if (cpu_info.empty())
    {
        return -1;
    }

    HostCpuInfo *host_cpu_info = HostCpuInfo::GetInstance();
    if (NULL == host_cpu_info)
    {
        return -1;
    }

    int64 cpu_info_id = host_cpu_info->GetIdByInfo(cpu_info);
    if (HostCpuInfo::INVALID_CPU_INFO_ID == cpu_info_id)
    {
        return -1;
    }
    return Get(cpu_info_id, tcu_info);
}

/******************************************************************************/ 
int32 ConfigTcu::Set(const map<string, string> &cpu_info, int32 tcu_num, 
                       const string &description)
{
    if (cpu_info.empty())
    {
        return -1;
    }

    HostCpuInfo *host_cpu_info = HostCpuInfo::GetInstance();
    if (NULL == host_cpu_info)
    {
        return -1;
    }

    int64 cpu_info_id = host_cpu_info->GetIdByInfo(cpu_info);
    if (HostCpuInfo::INVALID_CPU_INFO_ID == cpu_info_id)
    {
        cpu_info_id = host_cpu_info->Insert(cpu_info);
        if (HostCpuInfo::INVALID_CPU_INFO_ID == cpu_info_id)
        {
            return -1;
        }        
    }
   
    return Set(cpu_info_id, tcu_num, description);
}

/******************************************************************************/
int32 ConfigTcu::Drop(const map<string, string> &cpu_info)
{
    if (cpu_info.empty())
    {
        return -1;
    }

    HostCpuInfo *host_cpu_info = HostCpuInfo::GetInstance();
    if (NULL == host_cpu_info)
    {
        return -1;
    }

    int64 cpu_info_id = host_cpu_info->GetIdByInfo(cpu_info);
    if (HostCpuInfo::INVALID_CPU_INFO_ID == cpu_info_id)
    {
        return -1;
    }
    return Drop(cpu_info_id);
}

/******************************************************************************/
int32 ConfigTcu::SearchCallback(void *nil, SqlColumn *columns)
{
    if ((NULL == nil) || (NULL == columns))
    {
        return -1;
    }

    vector<TcuInfoType> *tcu_infos;
    tcu_infos = static_cast<vector<TcuInfoType> *>(nil);

    TcuInfoType tcu_info;
    columns->GetValue(0, tcu_info._cpu_info_id);
    columns->GetValue(2, tcu_info._commend_tcu_num);
    columns->GetValue(3, tcu_info._tcu_num);
    columns->GetValue(4, tcu_info._description);

    cout<<"SearchCallback _commend_tcu_num = "<<tcu_info._commend_tcu_num<<" !"<<endl;

    tcu_infos->push_back(tcu_info);    
    
    return 0;
}

/******************************************************************************/ 
int32 ConfigTcu::Search(vector<TcuInfoType> &tcu_infos, 
                          const string &where)
{
    ostringstream sql;
    sql << "SELECT " << _view_config_tcu_select_names << " FROM " << _view << " WHERE " << where;

    SetCallback(static_cast<Callbackable::Callback>(
                &ConfigTcu::SearchCallback), 
                static_cast<void *>(&tcu_infos));
    int32 rc = _db->Execute(sql, this);

    UnsetCallback();

    if ((0 != rc) && (SQLDB_RESULT_EMPTY != rc))
    {
        return rc;
    }    

    HostCpuInfo *host_cpu_info = HostCpuInfo::GetInstance();
    if (NULL != host_cpu_info)
    {
        vector<TcuInfoType>::iterator it;
        for (it = tcu_infos.begin(); it != tcu_infos.end(); ++it)
        {
            host_cpu_info->Dump(it->_cpu_info_id, it->_cpu_info);
        }
    }
    
    return 0;
}

/******************************************************************************/  
int32 ConfigTcu::CountCallback(void *nil, SqlColumn *columns)
{
    if ((NULL == nil) || (NULL == columns) || (1 != columns->get_column_num()))
    {
        return -1;
    }

    int64 *tcu_count = static_cast<int64*>(nil);
    *tcu_count = columns->GetValueInt64(0); 
    return 0;
}

/******************************************************************************/  
int64 ConfigTcu::Count()
{
    ostringstream   sql;

    int64 tcu_count = 0;
    SetCallback(static_cast<Callbackable::Callback>(
                &ConfigTcu::CountCallback),
                static_cast<void*>(&tcu_count));
    
    sql << "SELECT COUNT(*) FROM " << _view 
        << " WHERE cpu_info_id != " << HostCpuInfo::INVALID_CPU_INFO_ID;
    
    int32 rc = _db->Execute(sql, this);
    UnsetCallback();    

    if (rc != 0)
    {
        return 0;
    }

    return tcu_count;
}
int32 ConfigTcu::GetSystemTcuNumByCpuId(int64 cpuid)
{
    ostringstream oss;
    oss << " cpu_info_id = " << cpuid;

    string  column  = " system_tcu_num ";
    string   where  = oss.str();
    int   num = -1;

    SqlCallbackable sqlcb(_db);
    sqlcb.SelectColumn("config_tcu_select",
                            column,
                            where,
                            num);
    
    return num;
}

/******************************************************************************/
void ConfigTcu::Print()
{
    cout << 
        "---- TCU CONFIG--------------------------------------------" << endl;

    vector<TcuInfoType> tcu_infos;
    Search(tcu_infos, "cpu_info_id != -1");

    vector<TcuInfoType>::const_iterator it;
    for (it = tcu_infos.begin(); it != tcu_infos.end(); ++it)
    {
        cout << "cpu_info_id = " << it->_cpu_info_id    << endl 
             << "tcu_num   = "   << it->_tcu_num        << endl
             << "description = " << it->_description    << endl;
    }
}

/******************************************************************************/
void DbgPrintTcuConfig()
{
    ConfigTcu *config_tcu = ConfigTcu::GetInstance();
    if (NULL == config_tcu)
    {
        cout << "ConfigTcu is not initialed." << endl;
        return;
    }

    config_tcu->Print();
}
DEFINE_DEBUG_FUNC(DbgPrintTcuConfig);

} // namespace zte_tecs

