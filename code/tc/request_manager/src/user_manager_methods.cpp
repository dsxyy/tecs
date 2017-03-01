/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�user_manager_methods.cpp
* �ļ���ʶ��
* ����ժҪ��user managerģ��XML-RPCע�᷽�����ʵ���ļ�
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
#include "mid.h"
#include "event.h"
#include "sky.h"
#include "user_common.h"
#include "db_config.h"
#include "usergroup.h"
#include "user_manager_methods.h"

static int user_manager_methods_print = 0;  
DEFINE_DEBUG_VAR(user_manager_methods_print);
#define Debug(fmt,arg...) \
        do \
        { \
            if(user_manager_methods_print) \
            { \
                printf("[%s:%d]"fmt,__func__,__LINE__, ##arg); \
            } \
        }while(0)

namespace zte_tecs
{
void UserFullQueryMethod::MethodEntry(const paramList& paramList,value* const retval)
{
    ostringstream oss;
    int64 start_index =0;
    int64 query_count = 0;
    UserCfgFullQueryType query_type;
    
    /* 1. ��ȡ���� */
    int64 current_uid =  get_userid();
    int role = _upool->Get(current_uid,false)->get_role();

    start_index = xI8(paramList.getI8(1));
    query_count = xI8(paramList.getI8(2));
    int tmp = xINT(paramList.getInt(3));
    query_type = static_cast<UserFullQueryMethod::UserCfgFullQueryType>(tmp);
    string query_user = xSTR(paramList.getString(4));
  
    /* 2. У������Ƿ�Ϸ� */      
    /* ����������ֵ��100����¼ */
    if ((uint64)query_count > 100)
    {
        oss << "query count too big, legal range is [1,100].";
        Debug("%s\n",oss.str().c_str());
        xRET2(xINT(TECS_ERR_PARA_INVALID),xSTR(oss.str()));
        return;
    }
    
    if(query_type != UserFullQueryMethod::ALL_USER &&
        query_type != UserFullQueryMethod::APPOINTED_USER &&
        query_type != UserFullQueryMethod::CURRENT_USER)
    {
        oss<<"incorrect query type " << query_type;
        Debug("%s\n",oss.str().c_str());
        xRET2(xINT(TECS_ERR_PARA_INVALID),xSTR(oss.str()));
        return;
    }

    if(query_type == UserFullQueryMethod::APPOINTED_USER)
    {
        if(query_user.empty())
        {
            oss<<"no appointed user!";
            Debug("%s\n",oss.str().c_str());
            xRET2(xINT(TECS_ERR_PARA_INVALID),xSTR(oss.str()));
            return;
        }     
    }

    if(query_type == UserFullQueryMethod::APPOINTED_USER || 
        query_type == UserFullQueryMethod::ALL_USER)
    {
        //ֻ�й���Ա����Ȩ��ѯ�����û���ָ���û�
        if (role != UserGroup::CLOUD_ADMINISTRATOR)
        {
            oss << "Error, only administrators can query appoined user or all users!";
            Debug("%s\n",oss.str().c_str());
            xRET2(xINT(TECS_ERR_AUTHORIZE_FAILED),xSTR(oss.str()));
            return;
        }
    }
    
    ostringstream where_string;
    ostringstream where_string_count;
    UserFullQuery query(GetDB());

    switch (query_type)
    {
        /* ��ѯ��ǰ�û�����Ϣ */
        case UserFullQueryMethod::CURRENT_USER:
        {
            where_string << " user_id = " << current_uid;
            where_string_count<< " user_id = " << current_uid;
            break;
        }

        /* ��ѯָ���û�����Ϣ */
        case UserFullQueryMethod::APPOINTED_USER:
        {
            where_string << " user_name = '" << query_user << "'";
            where_string_count<< " user_name = '" << query_user << "'";
            break;
        }

        /* ��ѯ�����û�����Ϣ */
        case UserFullQueryMethod::ALL_USER:
        {                   
            where_string << " 1 = 1 " << 
                 " order by user_id limit " << query_count <<
                 " offset " << start_index;
            where_string_count<< " 1=1 ";
            break;
        }

        default:
        {
            SkyAssert(false);
            break;
        }
    }
       
    vector<UserFullInfo> users;
    vector<value> array_users;
    int64 max_count = query.GetUserCount(where_string_count.str());
    if(max_count < 0)
    {
        oss << "failed to get user max count! where string = " << where_string_count.str();
        Debug("%s\n",oss.str().c_str());
        xRET2(xINT(TECS_ERR_DB_FAIL),xSTR(oss.str()));
        return;
    }

    if(max_count == 0)
    {
        //���Բ����ٲ���
        xRET4(xINT(TECS_SUCCESS),xI8(-1),xI8(max_count),xARRAY(array_users));
        return;
    }
    
    int ret = query.GetUserFullInfo(users,where_string.str());
    if(ret < 0)
    {
        oss << "failed to get user full info! where string = " << where_string.str();
        Debug("%s\n",oss.str().c_str());
        xRET2(xINT(TECS_ERR_DB_FAIL),xSTR(oss.str()));
        return;
    }

    int64 next_index = -1;
    if (UserFullQueryMethod::ALL_USER && (int64)users.size() >= query_count )
    {
        next_index = start_index + query_count;
    }

    vector<UserFullInfo>::iterator it;
    for(it = users.begin(); it != users.end(); it++)
    {
        ApiUserFullInfo apiuser;
        it->ToApi(apiuser);
        array_users.push_back(apiuser.to_rpc());
    }
    xRET4(xINT(TECS_SUCCESS),xI8(next_index),xI8(max_count),xARRAY(array_users));
    return;
}

}

