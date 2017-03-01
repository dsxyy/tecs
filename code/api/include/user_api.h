/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：user_api.h
* 文件标识：
* 内容摘要：用户管理模块对外接口
* 当前版本：1.0
* 作    者：张文剑
* 完成日期：2012年04月27日
*
* 修改记录1：
*    修改日期：2012/4/27
*    版 本 号：V1.0
*    修 改 人：张文剑
*    修改内容：创建
*******************************************************************************/
#ifndef TECS_USER_API_H
#define TECS_USER_API_H
#include "rpcable.h"
#include "api_error.h"


class ApiUserFullInfo: public Rpcable
{
public:
    ApiUserFullInfo(){};
    ~ApiUserFullInfo(){};
    
    long long user_id;
    string user_name;
    //string password;
    int user_enabled;
    int role;
    string email;
    string phone;
    string location;
    long long group_id;
    string group_name;
    int group_type;
    int group_enabled;
    string group_description;
    long long project_count;
    long long vmcfg_count;
    long long vmtpl_count;
    long long image_count;

    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_I8(user_id);
        TO_VALUE(user_name);
        //TO_VALUE(password);
        TO_VALUE(user_enabled);
        TO_VALUE(role);
        TO_VALUE(email);
        TO_VALUE(phone);
        TO_VALUE(location);
        TO_I8(group_id);
        TO_VALUE(group_name);
        TO_VALUE(group_type);
        TO_VALUE(group_enabled);
        TO_VALUE(group_description);
        TO_I8(project_count);
        TO_I8(vmcfg_count);
        TO_I8(vmtpl_count);
        TO_I8(image_count);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_I8(user_id);
        FROM_VALUE(user_name);
        //FROM_VALUE(password);
        FROM_VALUE(user_enabled);
        FROM_VALUE(role);
        FROM_VALUE(email);
        FROM_VALUE(phone);
        FROM_VALUE(location);
        FROM_I8(group_id);
        FROM_VALUE(group_name);
        FROM_VALUE(group_type);
        FROM_VALUE(group_enabled);
        FROM_VALUE(group_description);
        FROM_I8(project_count);
        FROM_I8(vmcfg_count);
        FROM_I8(image_count);
        FROM_RPC_END();
    };
};
#endif

