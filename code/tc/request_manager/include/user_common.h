/******************************************************************************
* Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：user_common.h
* 文件标识：
* 内容摘要：user模块的rpc methods共用代码
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
#ifndef TECS_USER_METHODS_COMMON_H
#define TECS_USER_METHODS_COMMON_H
#include "sky.h"
#include "vm_messages.h"
#include "pool_sql.h"
#include "user_api.h"

using namespace zte_tecs;

class UserFullInfo: public Serializable
{
public:
    int64 user_id;
    string user_name;
    //string password;
    int user_enabled;
    int role;
    string email;
    string phone;
    string location;
    int64 group_id;
    string group_name;
    int group_type;
    int group_enabled;
    string group_description;
    int64 project_count;
    int64 vmcfg_count;
    int64 vmtpl_count;
    int64 image_count;

    void ToApi(ApiUserFullInfo& apiuser)
    {
        apiuser.user_id = user_id;
        apiuser.user_name = user_name;
        //apiuser.password = password;
        apiuser.user_enabled = user_enabled;
        apiuser.role = role;
        apiuser.email = email;
        apiuser.phone = phone;
        apiuser.location = location;
        apiuser.group_id = group_id;
        apiuser.group_name = group_name;
        apiuser.group_type = group_type;
        apiuser.group_enabled = group_enabled;
        apiuser.group_description = group_description;
        apiuser.project_count = project_count;
        apiuser.vmcfg_count = vmcfg_count;
        apiuser.vmtpl_count = vmtpl_count;
        apiuser.image_count = image_count;
    };

    SERIALIZE
    {
        SERIALIZE_BEGIN(UserFullInfo)
        WRITE_VALUE(user_id);
        WRITE_VALUE(user_name);
        //WRITE_VALUE(password);
        WRITE_VALUE(user_enabled);
        WRITE_VALUE(role);
        WRITE_VALUE(email);
        WRITE_VALUE(phone);
        WRITE_VALUE(location);
        WRITE_VALUE(group_id);
        WRITE_VALUE(group_name);
        WRITE_VALUE(group_type);
        WRITE_VALUE(group_enabled);
        WRITE_VALUE(group_description);
        WRITE_VALUE(project_count);
        WRITE_VALUE(vmcfg_count);
        WRITE_VALUE(vmtpl_count);
        WRITE_VALUE(image_count);
        SERIALIZE_END()
    }

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(UserFullInfo)
        READ_VALUE(user_id);
        READ_VALUE(user_name);
        //READ_VALUE(password);
        READ_VALUE(user_enabled);
        READ_VALUE(role);
        READ_VALUE(email);
        READ_VALUE(phone);
        READ_VALUE(location);
        READ_VALUE(group_id);
        READ_VALUE(group_name);
        READ_VALUE(group_type);
        READ_VALUE(group_enabled);
        READ_VALUE(group_description);
        READ_VALUE(project_count);
        READ_VALUE(vmcfg_count);
        READ_VALUE(vmtpl_count);
        READ_VALUE(image_count);
        DESERIALIZE_END()
    }
};

class UserFullQuery: public Callbackable
{
public:
    UserFullQuery(SqlDB *pDb):_db(pDb)
    {

    };

    ~UserFullQuery()
    {

    };

    int GetUserList(vector<int64>& vids,const string& where);
    int GetUserFullInfo(vector<UserFullInfo>& users,const string& where);
    int64 GetUserCount(const string& where);
    static STATUS Init();
    
private:   
    DISALLOW_COPY_AND_ASSIGN(UserFullQuery);

    static const char *_user_full_info_view; 
    static vector<string> _columns;
    static string _columns_joined_string;
    #define COLUMN_ORDER(field) (find(_columns.begin(),_columns.end(),field) - _columns.begin())

    int GetUserFullInfoCallback(void *nil, SqlColumn *columns);
    int SelectInt64Callback(void *nil, SqlColumn *columns);
    int SelectInt64VecCallback(void *nil, SqlColumn *columns);
    SqlDB *_db;
};
#endif

