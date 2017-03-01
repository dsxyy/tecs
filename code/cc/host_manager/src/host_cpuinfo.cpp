/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：host_cpuinfo.cpp
* 文件标识：
* 内容摘要：host_cpuinfo类的定义文件
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
#include "host_cpuinfo.h"
#include "host_pool.h"
#include "config_tcu.h"

namespace zte_tecs {

/* 主机使用的表名称 */
const char *HostCpuInfo::_table = "host_cpuinfo";

/* 主机中的表字段 */
const char *HostCpuInfo::_db_names = 
    "cpu_info_id, processor_num, physical_id_num, cpu_cores, vendor_id, cpu_family, "
    "model, model_name, stepping, cache_size, fpu, fpu_exception, cpuid_level, "
    "wp, flags, cpu_bench";

HostCpuInfo *HostCpuInfo::_instance = NULL;

SqlDB *HostCpuInfo::_db = NULL;

/******************************************************************************/ 
int32 HostCpuInfo::InitCallback(void *nil, SqlColumn *columns)
{
    if ((NULL == columns) || (1 != columns->get_column_num()))
    {
        return -1;
    }

    _last_cpu_info_id = columns->GetValueInt64(0);

    return 0;
}

/******************************************************************************/
HostCpuInfo::HostCpuInfo(SqlDB *pDb): _last_cpu_info_id(INVALID_CPU_INFO_ID)
{
    _db = pDb;

    ostringstream oss;

    SetCallback(static_cast<Callbackable::Callback>(
                &HostCpuInfo::InitCallback));

    oss << "SELECT MAX(cpu_info_id) FROM " << _table;
    
    _db->Execute(oss,this);
    
    UnsetCallback();

    if (INVALID_CPU_INFO_ID == _last_cpu_info_id)
    {
        _last_cpu_info_id = 0;
    }
};

/******************************************************************************/ 
string HostCpuInfo::WhereByInfo(const map<string, string> &info)
{
    ostringstream oss;
    map<string, string>::const_iterator it;
    char *sql_string = NULL;

    if (info.end() != (it = info.find("processor_num")))
    {
        oss << " WHERE processor_num = " << it->second;
    }
    else
    {
        oss << " WHERE processor_num = " << 0;
    }
    if (info.end() != (it = info.find("physical_id_num")))
    {
        oss << " AND physical_id_num = " << it->second;
    }
    else
    {
        oss << " AND physical_id_num = " << 0;
    } 
    if (info.end() != (it = info.find("cpu_cores")))
    {
        oss << " AND cpu_cores = " << it->second;
    }
    else
    {
        oss << " AND cpu_cores = " << 0;
    }  
    if ((info.end() != (it = info.find("vendor_id"))) && \
        (NULL != (sql_string = _db->EscapeString(it->second.c_str()))))
    {
        oss << " AND vendor_id = '" << sql_string << "'";
        _db->FreeString(sql_string);
    }
    else
    {
        oss << " AND vendor_id = ''";
    }     
    if (info.end() != (it = info.find("cpu_family")))
    {
        oss << " AND cpu_family = " << it->second;
    }
    else
    {
        oss << " AND cpu_family = " << 0;
    }
    if (info.end() != (it = info.find("model")))
    {
        oss << " AND model = " << it->second;
    }
    else
    {
        oss << " AND model = " << 0;
    } 
    if ((info.end() != (it = info.find("model_name"))) && \
        (NULL != (sql_string = _db->EscapeString(it->second.c_str()))))
    {
        oss << " AND model_name = '" << sql_string << "'";
        _db->FreeString(sql_string);
    }
    else
    {
        oss << " AND model_name = ''";
    }  
    if (info.end() != (it = info.find("stepping")))
    {
        oss << " AND stepping = " << it->second;
    }
    else
    {
        oss << " AND stepping = " << 0;
    }
    if (info.end() != (it = info.find("cache_size")))
    {
        oss << " AND cache_size = " << it->second;
    }
    else
    {
        oss << " AND cache_size = " << 0;
    }   
    if ((info.end() != (it = info.find("fpu"))) && \
        (NULL != (sql_string = _db->EscapeString(it->second.c_str()))))

    {
        oss << " AND fpu = '" << sql_string << "'";
        _db->FreeString(sql_string);
    }
    else
    {
        oss << " AND fpu = ''";
    }       
    if ((info.end() != (it = info.find("fpu_exception"))) && \
        (NULL != (sql_string = _db->EscapeString(it->second.c_str()))))

    {
        oss << " AND fpu_exception = '" << sql_string << "'";
        _db->FreeString(sql_string);
    }
    else
    {
        oss << " AND fpu_exception = ''";
    } 
    if (info.end() != (it = info.find("cpuid_level")))
    {
        oss << " AND cpuid_level = " << it->second;
    }
    else
    {
        oss << " AND cpuid_level = " << 0;
    }
    if ((info.end() != (it = info.find("wp"))) && \
        (NULL != (sql_string = _db->EscapeString(it->second.c_str()))))

    {
        oss << " AND wp = '" << sql_string << "'";
        _db->FreeString(sql_string);
    }
    else
    {
        oss << " AND wp = ''";
    }    
    if ((info.end() != (it = info.find("flags"))) && \
        (NULL != (sql_string = _db->EscapeString(it->second.c_str()))))

    {
        oss << " AND flags = '" << sql_string << "'";
        _db->FreeString(sql_string);
    }
    else
    {
        oss << " AND flags = ''";
    }    

    return oss.str();
}

/******************************************************************************/
int32 HostCpuInfo::GetIdCallback(void *nil, SqlColumn *columns)
{
    if ((NULL == nil) || (NULL == columns) || (1 != columns->get_column_num()))
    {
        return -1;
    }
    
    vector<int64> *oids = static_cast<vector<int64> *>(nil);
    oids->push_back(columns->GetValueInt64(0));
    return 0;
}

/******************************************************************************/
int64 HostCpuInfo::GetIdByInfo(const map<string, string> &info)
{
   ostringstream sql;
   int rc;
   vector<int64> oids;

   SetCallback(static_cast<Callbackable::Callback>(&HostCpuInfo::GetIdCallback),
               static_cast<void *>(&oids));

    sql << "SELECT cpu_info_id FROM " << _table << WhereByInfo(info);

    rc = _db->Execute(sql, this);

    UnsetCallback();

    if (0 != rc)
    {
        return INVALID_CPU_INFO_ID;
    }

    if (1 != oids.size())
    {
        return INVALID_CPU_INFO_ID;
    }

    return *oids.begin();
}

/******************************************************************************/
int32 HostCpuInfo::GetInfoCallback(void *nil, SqlColumn *columns)
{
    if ((NULL == nil) || (NULL == columns) || \
        (LIMIT != columns->get_column_num()))
    {
        return -1;
    }
    
    map<string, string> *info = static_cast<map<string, string> *>(nil);
    info->insert(make_pair("processor_num", 
        to_string<int32>(columns->GetValueInt(PROCESSOR_NUM), dec)));
    info->insert(make_pair("physical_id_num", 
        to_string<int32>(columns->GetValueInt(PHYSICAL_ID_NUM), dec)));
    info->insert(make_pair("cpu_cores", 
        to_string<int32>(columns->GetValueInt(CPU_CORES), dec)));   
    info->insert(make_pair("vendor_id", columns->GetValueText(VENDOR_ID)));
    info->insert(make_pair("cpu_family",
        to_string<int32>(columns->GetValueInt(CPU_FAMILY), dec)));
    info->insert(make_pair("model", 
        to_string<int32>(columns->GetValueInt(MODEL), dec)));
    info->insert(make_pair("model_name", columns->GetValueText(MODEL_NAME)));
    info->insert(make_pair("stepping",
        to_string<int32>(columns->GetValueInt(STEPPING), dec)));
    info->insert(make_pair("cache_size",
        to_string<int32>(columns->GetValueInt(CACHE_SIZE), dec)));
    info->insert(make_pair("fpu", columns->GetValueText(FPU)));
    info->insert(make_pair("fpu_exception", 
        columns->GetValueText(FPU_EXCEPTION)));
    info->insert(make_pair("cpuid_level",
        to_string<int32>(columns->GetValueInt(CPUID_LEVEL), dec)));
    info->insert(make_pair("wp", columns->GetValueText(WP)));
    info->insert(make_pair("flags", columns->GetValueText(FLAGS)));
    info->insert(make_pair("cpu_bench", to_string<int32>(columns->GetValueInt(CPU_BENCH),dec)));

    return 0;
}

/******************************************************************************/
int32 HostCpuInfo::GetInfoById(int64 cpu_info_id, map<string, string> &info)
{
   ostringstream sql;
   int rc;

   info.clear();

   SetCallback(static_cast<Callbackable::Callback>(
               &HostCpuInfo::GetInfoCallback),
               static_cast<void *>(&info));

    sql << "SELECT " << _db_names << " FROM " << _table
        << " WHERE cpu_info_id = " << cpu_info_id;

    rc = _db->Execute(sql, this);

    UnsetCallback();

    return rc;
}
/******************************************************************************/
int64 HostCpuInfo::GetCpuBenchByCpuId(const int64 &cpu_info_id)
{
    int64 cpu_bench = 0;
    ostringstream   where;
    where << " cpu_info_id = "<< cpu_info_id ;
    
    SqlCallbackable sqlcb(_db);
    sqlcb.SelectColumn(_table,
                       "cpu_bench",
                       where.str(),
                       cpu_bench);
    return cpu_bench;
}

/******************************************************************************/
int64 HostCpuInfo::UpdateCpubenchByCpuId(const map<string, string> &info,const int64 &cpu_info_id)
{
    int64 cpu_bench = 0;
    
    map<string, string>::const_iterator it;
    if (info.end() != (it = info.find("cpu_bench")))
    {
        string tmp = it->second;
        cpu_bench = atoi(tmp.c_str());
    }

    SqlCommand sql(_db,  _table);
    sql.Add("cpu_bench",    cpu_bench);

    ostringstream where ;
    where << "where cpu_info_id = "<< cpu_info_id ;
    return sql.Update(where.str());

}

/******************************************************************************/
int64 HostCpuInfo::UpdateCpubenchByCpuId(const int64& cpu_bench,
                                         const int64& cpu_info_id)
{
    SqlCommand sql(_db,  _table);
    sql.Add("cpu_bench", cpu_bench);

    ostringstream where ;
    where << "where cpu_info_id = "<< cpu_info_id ;
    return sql.Update(where.str());
}

/******************************************************************************/
int64 HostCpuInfo::Insert(const map<string, string> &info)
{
    ostringstream oss;
    oss << "INSERT INTO " << _table << " ("<< _db_names <<") VALUES (" 
        << _last_cpu_info_id + 1;

    map<string, string>::const_iterator it;
    char *sql_string = NULL;   

    if (info.end() != (it = info.find("processor_num")))
    {
        oss << ", " << it->second;
    }
    else
    {
        oss << ", " << 0;
    }
    if (info.end() != (it = info.find("physical_id_num")))
    {
        oss << ", " << it->second;
    }
    else
    {
        oss << ", " << 0;
    } 
    if (info.end() != (it = info.find("cpu_cores")))
    {
        oss << ", " << it->second;
    }
    else
    {
        oss << ", " << 0;
    }  
    if ((info.end() != (it = info.find("vendor_id"))) && \
        (NULL != (sql_string = _db->EscapeString(it->second.c_str()))))
    {
        oss << ", '" << sql_string << "'";
        _db->FreeString(sql_string);
    }
    else
    {
        oss << ", ''";
    }     
    if (info.end() != (it = info.find("cpu_family")))
    {
        oss << ", " << it->second;
    }
    else
    {
        oss << ", " << 0;
    }
    if (info.end() != (it = info.find("model")))
    {
        oss << ", " << it->second;
    }
    else
    {
        oss << ", " << 0;
    } 
    if ((info.end() != (it = info.find("model_name"))) && \
        (NULL != (sql_string = _db->EscapeString(it->second.c_str()))))
    {
        oss << ", '" << sql_string << "'";
        _db->FreeString(sql_string);
    }
    else
    {
        oss << ", ''";
    }  
    if (info.end() != (it = info.find("stepping")))
    {
        oss << ", " << it->second;
    }
    else
    {
        oss << ", " << 0;
    }
    if (info.end() != (it = info.find("cache_size")))
    {
        oss << ", " << it->second;
    }
    else
    {
        oss << ", " << 0;
    }   
    if ((info.end() != (it = info.find("fpu"))) && \
        (NULL != (sql_string = _db->EscapeString(it->second.c_str()))))

    {
        oss << ", '" << sql_string << "'";
        _db->FreeString(sql_string);
    }
    else
    {
        oss << ", ''";
    }       
    if ((info.end() != (it = info.find("fpu_exception"))) && \
        (NULL != (sql_string = _db->EscapeString(it->second.c_str()))))

    {
        oss << ", '" << sql_string << "'";
        _db->FreeString(sql_string);
    }
    else
    {
        oss << ", ''";
    } 
    if (info.end() != (it = info.find("cpuid_level")))
    {
        oss << ", " << it->second;
    }
    else
    {
        oss << ", " << 0;
    }
    if ((info.end() != (it = info.find("wp"))) && \
        (NULL != (sql_string = _db->EscapeString(it->second.c_str()))))

    {
        oss << ", '" << sql_string << "'";
        _db->FreeString(sql_string);
    }
    else
    {
        oss << ", ''";
    }    
    if ((info.end() != (it = info.find("flags"))) && \
        (NULL != (sql_string = _db->EscapeString(it->second.c_str()))))

    {
        oss << ", '" << sql_string << "'";
        _db->FreeString(sql_string);
    }
    else
    {
        oss << ", ''";
    }    

    if (info.end() != (it = info.find("cpu_bench")))
    {
        oss << ", " << it->second;
    }
    else
    {
        oss << ", " << 0.0;
    }
    
    oss << ")";

    if (0 != _db->Execute(oss))
    {
        return INVALID_CPU_INFO_ID;
    }

    return ++_last_cpu_info_id;
}

/******************************************************************************/
int32 HostCpuInfo::Drop(const map<string, string> &info)
{
    return Drop(GetIdByInfo(info));
}

/******************************************************************************/
int32 HostCpuInfo::Drop(int64 cpu_info_id)
{
    if (INVALID_CPU_INFO_ID == cpu_info_id)
    {
        return -1;
    }

    ostringstream   oss;    
    oss << "DELETE FROM " << _table << " WHERE cpu_info_id = " << cpu_info_id;
    return _db->Execute(oss);    
}

/******************************************************************************/
int32 HostCpuInfo::SearchCallback(void *nil, SqlColumn *columns)
{
    vector<int64> *cpu_info_ids;

    cpu_info_ids = static_cast<vector<int64> *>(nil);

    if ((NULL == columns) || (1 != columns->get_column_num()))
    {
        return -1;
    }
    
    cpu_info_ids->push_back(columns->GetValueInt64(0));

    return 0;
}

/******************************************************************************/ 
int32 HostCpuInfo::Search(vector<int64> &cpu_info_ids, const string &where)
{
    ostringstream   sql;
    int             rc;

    SetCallback(static_cast<Callbackable::Callback>(&HostCpuInfo::SearchCallback),
                static_cast<void *>(&cpu_info_ids));

    sql << "SELECT cpu_info_id FROM " << HostCpuInfo::_table;
    if (!where.empty())
    {
        sql << " WHERE " << where;
    }

    rc = _db->Execute(sql, this);

    UnsetCallback();

    return rc;
}
/******************************************************************************/
bool  HostCpuInfo::IsExist(int64 cpu_id)
{
    vector<int64> cpu_info_ids;
    ostringstream oss;
    oss << " cpu_info_id = " << cpu_id;
    Search(cpu_info_ids, oss.str());
    if (cpu_info_ids.empty())
    {
        return false;
    }

    return true;
}

/******************************************************************************/
void HostCpuInfo::Dump(int64 cpu_info_id, string &info)
{
    ostringstream oss;

    map<string, string> cpu_info;
    GetInfoById(cpu_info_id, cpu_info);
    map<string, string>::const_iterator it;
    for (it = cpu_info.begin(); it != cpu_info.end(); ++it)
    {
        oss << "<" << it->first << ">" << it->second 
            << "</" << it->first << ">" << endl;
    }

    info = oss.str();    
}

/******************************************************************************/
void HostCpuInfo::Print(int64 cpu_info_id)
{
    cout << 
        "---- CPUINFO INFO -----------------------------------------" << endl <<
        "cpu_info_id = " << cpu_info_id << endl;

    string info;
    Dump(cpu_info_id, info);
    cout << info;
}

/******************************************************************************/
void DbgPrintCpuInfo(int64 cpu_info_id)
{
    HostCpuInfo *host_cpu_info = HostCpuInfo::GetInstance();
    if (NULL == host_cpu_info)
    {
        cout << "HostCpuInfo is not initialed." << endl;
        return;
    }

    host_cpu_info->Print(cpu_info_id);
}
DEFINE_DEBUG_FUNC(DbgPrintCpuInfo);

} // namespace zte_tecs

