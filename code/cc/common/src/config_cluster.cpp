/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：config_cluster.cpp
* 文件标识：
* 内容摘要：ConfigCluster类的定义文件
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
#include "config_cluster.h"

namespace zte_tecs {

/* 主机使用的表名称 */
const char *ConfigCluster::_table = "config_cluster";

/* 主机中的表字段 */
const char *ConfigCluster::_db_names = 
    "config_name, config_value";

SqlDB *ConfigCluster::_db = NULL;

ConfigCluster *ConfigCluster::_instance = NULL;

/******************************************************************************/  
int ConfigCluster::InitCallback(void *nil, SqlColumn *columns)
{
    if ((NULL == columns) || (LIMIT != columns->get_column_num()))
    {
        return -1;
    }

    _configs.insert(make_pair(columns->GetValueText(CONFIG_NAME),
                              columns->GetValueText(CONFIG_VALUE)));
    return 0;
}

/******************************************************************************/  
ConfigCluster::ConfigCluster():_configs()
{
}

/******************************************************************************/
ConfigCluster::ConfigCluster(SqlDB *pDb):_configs()
{
    _db = pDb;

    ostringstream   oss;

    SetCallback(static_cast<Callbackable::Callback>(
                &ConfigCluster::InitCallback));

    oss << "SELECT " << _db_names << " FROM " << _table;

    _db->Execute(oss, this);

    UnsetCallback();
    
};

/******************************************************************************/ 

int ConfigCluster::SelectStringCallback(void *nil, SqlColumn *columns)
{
    if (!nil || !columns)
    {
        return -1;
    }
    string *pstr = static_cast<string*>(nil);
    columns->GetValue(0,*pstr);
    return 0;
}

/******************************************************************************/ 
int ConfigCluster::GetConfigValueFromConfigCluster(const string & config_name,string &config_value)
{
    ostringstream where;
    int rc;
    where<<"select config_value from config_cluster where  config_name = '"<<config_name<<"'";

    SetCallback(static_cast<Callbackable::Callback>(&ConfigCluster::SelectStringCallback),
                &config_value);
    rc = _db->Execute(where, this);
    UnsetCallback();
    if ( 0 != rc && SQLDB_RESULT_EMPTY != rc) 
    {
        SkyAssert(false);
    }
    return rc;
}

/******************************************************************************/ 
const string ConfigCluster::Get(const string &name)
{
    SkyAssert(!name.empty());

    string  value = "";
    
    map<string, string>::iterator it = _configs.find(name);    
    /*  找到记录 */   
    if (it != _configs.end())
    {
        /* 如果内存中记录的值非空，直接传给返回值 */
        if(!it->second.empty())
        {
            value = it->second;
        }
        /* 如果内存中记录值为空，查表 */
        else
        {
            GetConfigValueFromConfigCluster(name,value);
            /* 查到有效值，将结果更新到内存 */
            if(!value.empty())
            {
                it->second = value;  
            }
        }
    }
    /* 没有找到记录,进行查表 */
    else
    {
        GetConfigValueFromConfigCluster(name,value);
        /* 查到有效值后，将结果记录到内存 */
        if(!value.empty())
        {
            _configs.insert(make_pair(name, value));
        }
    }

    return value;
}

/******************************************************************************/ 
int32 ConfigCluster::Set(const string &name, const string &value)
{
    SkyAssert(!name.empty());

    int32 rc;
    
    map<string, string>::iterator it = _configs.find(name);
    if (it != _configs.end())
    {
        if (-1 != (rc = InsertReplace(name, value, true)))
        {
            it->second = value;
        }
    }
    else
    {
        if (-1 != (rc = InsertReplace(name, value, false)))
        {
            _configs.insert(make_pair(name, value));
        }    
    }
    return rc;
}

/******************************************************************************/
int32 ConfigCluster::InsertReplace(const string &name, 
                                   const string &value,
                                   bool replace)
{
    ostringstream   oss;
 
    SqlCommand sql(_db, _table);
    sql.Add("config_name",  name );
    sql.Add("config_value", value);
    
    if (replace)
    {
       return sql.Update("WHERE config_name = '" +  name +"'");
    }
    else
    {
       return sql.Insert();
    }
    
    return -1;

}

/******************************************************************************/
int32 ConfigCluster::Drop(const string &name)
{
    char *sql_config_name;
    if (NULL == (sql_config_name = _db->EscapeString(name.c_str())))
    {
        return -1;
    }
    
    ostringstream oss;
    oss << "DELETE FROM " << _table << " WHERE config_name = '"
        << sql_config_name << "'";
    
    int32 rc = _db->Execute(oss);
    
    if (0 == rc)
    {
        _configs.erase(name);
    }

    _db->FreeString(sql_config_name);
    
    return rc;
}

/******************************************************************************/
void ConfigCluster::Print()
{
    cout << 
        "---- CLUSTER CONFIG----------------------------------------" << endl;

    map<string, string>::iterator it;
    for (it = _configs.begin(); it != _configs.end(); ++it)
    {
        cout << it->first << " = " << it->second << endl;
    }
}

/******************************************************************************/
void DbgPrintClusterConfig()
{
    ConfigCluster *config_cluster = ConfigCluster::GetInstance();
    if (NULL == config_cluster)
    {
        cout << "ConfigCluster is not initialed." << endl;
        return;
    }

    config_cluster->Print();
}
DEFINE_DEBUG_FUNC(DbgPrintClusterConfig);

} // namespace zte_tecs

