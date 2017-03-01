/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：user_manager_methods.cpp
* 文件标识：
* 内容摘要：user manager模块XML-RPC注册方法类的实现文件
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
    
    /* 1. 获取参数 */
    int64 current_uid =  get_userid();
    int role = _upool->Get(current_uid,false)->get_role();

    start_index = xI8(paramList.getI8(1));
    query_count = xI8(paramList.getI8(2));
    int tmp = xINT(paramList.getInt(3));
    query_type = static_cast<UserFullQueryMethod::UserCfgFullQueryType>(tmp);
    string query_user = xSTR(paramList.getString(4));
  
    /* 2. 校验参数是否合法 */      
    /* 单包数量限值在100条记录 */
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
        //只有管理员才有权查询所有用户或指定用户
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
        /* 查询当前用户的信息 */
        case UserFullQueryMethod::CURRENT_USER:
        {
            where_string << " user_id = " << current_uid;
            where_string_count<< " user_id = " << current_uid;
            break;
        }

        /* 查询指定用户的信息 */
        case UserFullQueryMethod::APPOINTED_USER:
        {
            where_string << " user_name = '" << query_user << "'";
            where_string_count<< " user_name = '" << query_user << "'";
            break;
        }

        /* 查询所有用户的信息 */
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
        //可以不用再查了
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

