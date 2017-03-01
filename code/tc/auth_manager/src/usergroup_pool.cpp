/******************************************************************************
* Copyright (c) 2011，深圳市中兴通讯股份有限公司
* All rights reserved.
* 
* 文件名称：usergrouppool.cpp
* 文件标识：见配置管理计划书
* 内容摘要：用户组对象信息持久化类
* 当前版本：1.0
* 作    者：王明辉
* 完成日期：2011年7月21日
*
* 修改记录1：
*    修改日期：2011/7/21
*    版 本 号：V1.0
*    修 改 人：王明辉
*    修改内容：创建
******************************************************************************/
#include "usergroup_pool.h"

namespace zte_tecs
{
UsergroupPool* UsergroupPool::instance = NULL;

/* ************************************************************************** */
/* UsergroupPool :: Constructor                                               */
/* ************************************************************************** */

UsergroupPool::UsergroupPool(SqlDB* db)
    : SqlCallbackable(db)
{
    _lastOid = -1;
}

STATUS UsergroupPool::Init()
{
    int     rc      = -1;
    string  column  = " MAX(oid) ";
    string  where   = " 1=1 ";
    int64   tmp_oid = 0;

    // 获取最大的oid
    SqlCallbackable sqlcb(_db);
    rc = sqlcb.SelectColumn(k_table, column, where, tmp_oid);
    
    if((SQLDB_OK != rc) &&(SQLDB_RESULT_EMPTY != rc))
    { 
        return rc;
    }

    _lastOid = tmp_oid;    

    if (_lastOid == 0)
    {
        int64  first_uid = -1;
        string first_group = "Default";
        string error_str;

        Allocate(&first_uid, first_group, 1, true, error_str);

        if(first_uid == -1)
        {
            cout << error_str << endl;
        }
    }

    return SUCCESS;
}


/* ************************************************************************** */
/* UsergroupPool :: Database Access Functions                                 */
/* ************************************************************************** */

const char* UsergroupPool::k_table = "usergroup_pool";

const char* UsergroupPool::k_db_names = "oid, name, description, create_time, type, enabled";


/* -------------------------------------------------------------------------- */

int64 UsergroupPool::Allocate(int64* oid, string groupname, int grouptype, bool enable, string& err_str)
{
    Datetime now;
    now.refresh();

    *oid = ++_lastOid;

    SqlCommand sql(_db, k_table);
    sql.Add("oid", *oid);
    sql.Add("name", groupname);
    sql.Add("description", "");
    sql.Add("create_time", now);    
    sql.Add("type", grouptype);
    sql.Add("enabled", enable);       

    int rc = sql.Insert();

    if (rc != 0)
    {
        --_lastOid;
        *oid = -1;
    }

    return *oid;
}

/* -------------------------------------------------------------------------- */

int UsergroupPool::Get(int64 oid, UserGroup* ug)
{
    ostringstream   oss;

    SetCallback(static_cast<Callbackable::Callback>(&UsergroupPool::SelectCallback), static_cast<void *>(ug));

    oss << "SELECT " << k_db_names << " FROM " << k_table << " WHERE oid = " << oid;

    int rc = _db->Execute(oss, this);

    UnsetCallback();

    return rc;
}

/* -------------------------------------------------------------------------- */

int UsergroupPool::Get(string groupname, UserGroup* ug)
{
    ostringstream   oss;

    SetCallback(static_cast<Callbackable::Callback>(&UsergroupPool::SelectCallback), static_cast<void *>(ug));

    char *sql_groupname = _db->EscapeString(groupname);

    if(!sql_groupname)
    {
        SkyAssert(false);
        return -1;
    }
    
    oss << "SELECT " << k_db_names << " FROM " << k_table << " WHERE name = '" << sql_groupname << "'";

    _db->FreeString(sql_groupname);

    int rc = _db->Execute(oss, this);

    UnsetCallback();

    return rc;
}

/* -------------------------------------------------------------------------- */

int64 UsergroupPool::GetGroupSize()
{
    int     rc      = -1;
    string  column  = " COUNT(oid) ";
    string  where   = " 1=1 ";
    int64   count   = -1;

    // 获取最大的oid
    SqlCallbackable sqlcb(_db);
    rc = sqlcb.SelectColumn(k_table, column, where, count);
    
    if((SQLDB_OK != rc) &&(SQLDB_RESULT_EMPTY != rc))
    { 
        count = -1;
    }     
    
    return count;
}

/* -------------------------------------------------------------------------- */

int UsergroupPool::SelectCallback(void * nil, SqlColumn * columns)
{
    if (columns == NULL || columns->get_column_num() != LIMIT)
    {
        return -1;
    }

    UserGroup* ug = static_cast<UserGroup*>(nil);

    if( ug == NULL)
    {
        return -1;
    }

    columns->GetValue(OID,          ug->_oid);
    columns->GetValue(GROUPNAME,    ug->_groupname);
    columns->GetValue(GROUPTYPE,    ug->_grouptype);
    columns->GetValue(ENABLED,      ug->_enabled); 
    columns->GetValue(DESCRIPTION,  ug->_description);
    columns->GetValue(CREATE_TIME,  ug->_create_time);

    return 0;
}


/* -------------------------------------------------------------------------- */

int UsergroupPool::Update(UserGroup* ug)
{
    SqlCommand sql(_db, k_table);
    sql.Add("oid",         ug->_oid);
    sql.Add("name",        ug->_groupname);
    sql.Add("description", ug->_description);
    sql.Add("create_time", ug->_create_time);    
    sql.Add("type",        ug->_grouptype);
    sql.Add("enabled",     ug->_enabled);    
    
    return sql.Update(" WHERE oid = " + to_string(ug->_oid, dec));
}

/* -------------------------------------------------------------------------- */

int UsergroupPool::Drop(UserGroup* ug)
{
    SqlCommand sql(_db, k_table);

    return sql.Delete(" WHERE oid = " + to_string(ug->_oid, dec));
}

/* -------------------------------------------------------------------------- */

int UsergroupPool::Dump(ostringstream& oss, const string& where)
{
    int             rc;
    ostringstream   cmd;

    _dump_groups.clear();

    oss << "<USERGROUP_POOL>";

    SetCallback(static_cast<Callbackable::Callback>(&UsergroupPool::DumpCallback),
                 static_cast<void *>(&oss));

    cmd << "SELECT " << k_db_names << " FROM " << k_table;

    if ( !where.empty() )
    {
        cmd << " WHERE " << where;
    }

    rc = _db->Execute(cmd, this);

    UnsetCallback();

    oss << "</USERGROUP_POOL>";

    return rc;
}


/* -------------------------------------------------------------------------- */

int UsergroupPool::DumpCallback(void * oss, SqlColumn * columns)
{
    if (NULL == columns || LIMIT != columns->get_column_num() )    
    {        
        return -1;    
    }   


    int oid;
    UsergroupInfo info;

    columns->GetValue(OID,          oid);
    columns->GetValue(GROUPNAME,    info.groupname);
    columns->GetValue(GROUPTYPE,    info.grouptype);
    columns->GetValue(ENABLED,      info.enabled  ); 
    columns->GetValue(DESCRIPTION,  info.description);
    columns->GetValue(CREATE_TIME,  info.create_time);

    _dump_groups.insert(make_pair(oid, info));

    return 0;
}

/* -------------------------------------------------------------------------- */
}

