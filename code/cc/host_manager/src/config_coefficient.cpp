/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：config_coefficient.cpp
* 文件标识：
* 内容摘要：config_coefficient类的定义文件
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
#include "config_coefficient.h"
#include "host_cpuinfo.h"
#include "sky.h"

namespace zte_tecs {

/* coefficient配置使用的表名称 */
const char *config_coefficient::_table = "config_coefficient";

/* coefficient配置中的表字段 */
const char *config_coefficient::_db_names = 
    "tecs_bench, description";

SqlDB *config_coefficient::_db = NULL;

config_coefficient *config_coefficient::_instance = NULL;

/******************************************************************************/ 
config_coefficient::config_coefficient()
{
}

/******************************************************************************/ 
config_coefficient::config_coefficient(SqlDB *pDb)
{
    _db = pDb;
};

/******************************************************************************/ 
int32 config_coefficient::Set(const string &coefficient_type, 
                               int32        coefficient_value, 
                       const string &description)
{
    InsertReplace(coefficient_type, coefficient_value, description, false);
    return InsertReplace(coefficient_type, coefficient_value, description, true);
}

/******************************************************************************/
int32 config_coefficient::InsertReplace(const string &coefficient_type, 
                                            int32 coefficient_value, 
                                            const string &description, 
                                            bool replace)
{
    char *sql_description;
    char *sql_coefficient_type;
    
    if (NULL == (sql_description = _db->EscapeString(description.c_str())))
    {
        return -1;
    }
    if (NULL == (sql_coefficient_type = _db->EscapeString(coefficient_type.c_str())))
    {
        return -1;
    }
    
    ostringstream   oss;

    if (replace)
    {
        oss << "UPDATE " << _table << " SET "
            << "tecs_bench = " << coefficient_value << ", "
            << "description = '" << sql_description << "'";
    }
    else
    {
        oss << "INSERT INTO " << _table << " ("<< _db_names <<") VALUES (" 
            << "tecs_bench=" << "," << coefficient_value << ", '" 
            << sql_description << "')";
    }
    
    _db->FreeString(sql_description);
    _db->FreeString(sql_coefficient_type);
    
    return _db->Execute(oss);
}

} // namespace zte_tecs

