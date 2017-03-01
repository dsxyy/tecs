/******************************************************************************
* Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：user_common.cpp
* 文件标识：
* 内容摘要：在apisvr进程中直接查询tc上的user信息
* 当前版本：1.0
* 作    者：张文剑
* 完成日期：2012年4月27日
*
* 修改记录1：
*     修改日期：2012/4/27
*     版 本 号：V1.0
*     修 改 人：张文剑
*     修改内容：创建
******************************************************************************/
#include "user_common.h"
#include "log_agent.h"
#include "tecs_config.h"

/******************************************************************************/
vector<string> UserFullQuery::_columns;
string UserFullQuery::_columns_joined_string;
const char* UserFullQuery::_user_full_info_view = "user_full_info";

/******************************************************************************/
STATUS UserFullQuery::Init()
{
    if(!_columns.empty())
    {
        SkyAssert(false);
        return ERROR_DUPLICATED_OP;
    }
  
    _columns.push_back("user_id");
    _columns.push_back("user_name");
    //_columns.push_back("password");
    _columns.push_back("user_enabled");
    _columns.push_back("role");
    _columns.push_back("email");
    _columns.push_back("phone");
    _columns.push_back("location");
    _columns.push_back("group_id");
    _columns.push_back("group_name");
    _columns.push_back("group_type");
    _columns.push_back("group_enabled");
    _columns.push_back("group_description");
    _columns.push_back("project_count");
    _columns.push_back("vmcfg_count");
    _columns.push_back("vmtpl_count");
    _columns.push_back("image_count");
    _columns_joined_string = JoinStrings(_columns, ",");
    return SUCCESS;
}

/******************************************************************************/
int UserFullQuery::GetUserFullInfoCallback(void *nil, SqlColumn *columns)
{
    vector<UserFullInfo>* pusers = NULL;
    if(!nil)
    {
        SkyAssert(false);
        return 0;
    }
    
    pusers = static_cast<vector<UserFullInfo> *>(nil);
    if ( NULL == columns || (int)_columns.size() != columns->get_column_num())
    {
        SkyAssert(false);
        return -1;
    }
    
    UserFullInfo user;    
    columns->GetValue(COLUMN_ORDER("user_id"),user.user_id);
    columns->GetValue(COLUMN_ORDER("user_name"),user.user_name);
    //columns->GetValue(COLUMN_ORDER("password"),user.password);
    columns->GetValue(COLUMN_ORDER("user_enabled"),user.user_enabled);
    columns->GetValue(COLUMN_ORDER("role"),user.role);
    columns->GetValue(COLUMN_ORDER("email"),user.email);
    columns->GetValue(COLUMN_ORDER("phone"),user.phone);
    columns->GetValue(COLUMN_ORDER("location"),user.location);
    columns->GetValue(COLUMN_ORDER("group_id"),user.group_id);
    columns->GetValue(COLUMN_ORDER("group_name"),user.group_name);
    columns->GetValue(COLUMN_ORDER("group_type"),user.group_type);
    columns->GetValue(COLUMN_ORDER("group_enabled"),user.group_enabled);
    columns->GetValue(COLUMN_ORDER("group_description"),user.group_description);
    columns->GetValue(COLUMN_ORDER("project_count"),user.project_count);
    columns->GetValue(COLUMN_ORDER("vmcfg_count"),user.vmcfg_count);
    columns->GetValue(COLUMN_ORDER("vmtpl_count"),user.vmtpl_count);
    columns->GetValue(COLUMN_ORDER("image_count"),user.image_count);
    pusers->push_back(user);
    return 0;
}

/******************************************************************************/
int UserFullQuery::GetUserFullInfo(vector<UserFullInfo>& users,const string& where)
{
    ostringstream sql;
    sql << "SELECT " << _columns_joined_string
        << " FROM "  << UserFullQuery::_user_full_info_view
        << " WHERE " << where;
    OutPut(SYS_DEBUG,"GetUserFullInfo: %s\n",sql.str().c_str());

    SetCallback(static_cast<Callbackable::Callback>(&UserFullQuery::GetUserFullInfoCallback),(void *)&users);
    int ret = _db->Execute(sql, this);
    UnsetCallback();
    return ret;
}

/******************************************************************************/
int UserFullQuery::SelectInt64Callback(void *nil, SqlColumn *columns)
{
    if (!nil || !columns)
    {
        return -1;
    }
    int64 *p = static_cast<int64*>(nil);
    *p = columns->GetValueInt64(0);
    return 0;
}

/******************************************************************************/
int64 UserFullQuery::GetUserCount(const string& where)
{
    int64 count = -1;
    ostringstream sql;
    sql << "SELECT COUNT(*) FROM "  << UserFullQuery::_user_full_info_view
        << " WHERE " << where;
    OutPut(SYS_DEBUG,"GetUserCount: %s\n",sql.str().c_str());

    SetCallback(static_cast<Callbackable::Callback>(&UserFullQuery::SelectInt64Callback),(void *)&count);
    _db->Execute(sql, this);
    UnsetCallback();
    return count;
}

/******************************************************************************/
int UserFullQuery::SelectInt64VecCallback(void *nil, SqlColumn *columns)
{
    if (!nil || !columns)
    {
        return -1;
    }
    vector<int64> *p = static_cast<vector<int64> *>(nil);
    p->push_back(columns->GetValueInt64(0));
    return 0;
}

/******************************************************************************/
int UserFullQuery::GetUserList(vector<int64>& uids,const string& where)
{
    ostringstream sql;
    sql << "SELECT user_id FROM "  << UserFullQuery::_user_full_info_view
        << " WHERE " << where;
    OutPut(SYS_DEBUG,"GetUserList: %s\n",sql.str().c_str());

    SetCallback(static_cast<Callbackable::Callback>(&UserFullQuery::SelectInt64VecCallback),(void *)&uids);
    int ret = _db->Execute(sql, this);
    UnsetCallback();
    return ret;
}

/******************************************************************************/
void DbgShowAllUserFullInfo()
{
    vector<UserFullInfo> users;
    UserFullQuery query(GetDB());
    query.GetUserFullInfo(users,"1=1");
    vector<UserFullInfo>::iterator it;
    for(it=users.begin();it!=users.end();it++)
    {
        cout << "user:" << it->serialize() <<endl;
    }
}
DEFINE_DEBUG_FUNC(DbgShowAllUserFullInfo);
  

