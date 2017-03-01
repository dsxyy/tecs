/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�user_api.h
* �ļ���ʶ��
* ����ժҪ���û�����ģ�����ӿ�
* ��ǰ�汾��1.0
* ��    �ߣ����Ľ�
* ������ڣ�2012��04��27��
*
* �޸ļ�¼1��
*    �޸����ڣ�2012/4/27
*    �� �� �ţ�V1.0
*    �� �� �ˣ����Ľ�
*    �޸����ݣ�����
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

