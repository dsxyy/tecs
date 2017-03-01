/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：affinity_region_api.h
* 文件标识：
* 内容摘要：亲和域模块对外接口
* 当前版本：1.0
* 作    者：
* 完成日期：2013年8月26日
*
* 修改记录1：
*    修改日期：2013/8/26
*    版 本 号：V1.0
*    修 改 人：
*    修改内容：创建
*******************************************************************************/

#ifndef TECS_AFFINITY_REGION_API_H
#define TECS_AFFINITY_REGION_API_H
#include "rpcable.h"
#include "api_error.h"
#include "api_pub.h"
#include "api_const.h"


/**
@brief 功能描述: 亲和域创建接口的参数定义 \n
@li @b 其它说明: 无
*/
class ApiAffinityRegionAllocateData : public Rpcable
{
public:
    ApiAffinityRegionAllocateData()
    {
        is_forced = false;
    };
    ~ApiAffinityRegionAllocateData(){};

     bool Validate(string &err_str)
    {
        ostringstream    oss;

        if (StringCheck::CheckNormalName(name,1, STR_LEN_32) != true)
        {
            oss << "Error, invalid parameter with  " << name;
            err_str = oss.str();
            return false;
        }

        if (StringCheck::CheckSize(description, 0, STR_LEN_512) != true)
        {
            oss << "Error, invalid parameter with " << description;
            err_str = oss.str();
            return false;
        }

        if((AFFINITY_REGION_SHELF   != level)
           &&(AFFINITY_REGION_RACK != level)
           &&(AFFINITY_REGION_BOARD != level))
        {
            err_str = "error level type: " + level;
            return false;
        }

        return true;

    }

    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_VALUE(name);
        TO_VALUE(description);
        TO_VALUE(level);
        TO_VALUE(is_forced);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_VALUE(name);
        FROM_VALUE(description);
        FROM_VALUE(level);
        FROM_VALUE(is_forced);
        FROM_RPC_END();
    };

    string            name;
    string            description;
    string            level;
    bool              is_forced;
};

/**
@brief 功能描述: 亲和域创建接口的参数定义 \n
@li @b 其它说明: 无
*/
class ApiAffinityRegionDelete : public Rpcable
{
public:
    ApiAffinityRegionDelete(){};
    ~ApiAffinityRegionDelete(){};

    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_I8(affinity_region_id);

        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_I8(affinity_region_id);

        FROM_RPC_END();
    };

    long long         affinity_region_id;
};

/**
@brief 功能描述: 亲和域修改接口的参数定义 \n
@li @b 其它说明: 无
*/
class ApiAffinityRegionModifyData : public Rpcable
{
public:
    ApiAffinityRegionModifyData(){};
    ~ApiAffinityRegionModifyData(){};

     bool Validate(string &err_str)
    {
        ostringstream    oss;

        if (StringCheck::CheckNormalName(name,1, STR_LEN_32) != true)
        {
            oss << "Error, invalid parameter with  " << name;
            err_str = oss.str();
            return false;
        }

        if (StringCheck::CheckSize(new_des, 0, STR_LEN_512) != true)
        {
            oss << "Error, invalid parameter with  " << new_des;
            err_str = oss.str();
            return false;
        }

        if((AFFINITY_REGION_SHELF   != level)
           &&(AFFINITY_REGION_RACK != level)
           &&(AFFINITY_REGION_BOARD != level))
        {
            err_str = "error level type: " + level;
            return false;
        }

        return true;

    }

    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_I8(id);
        TO_VALUE(name);
        TO_VALUE(new_des);
        TO_VALUE(level);
        TO_VALUE(is_forced);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_I8(id);
        FROM_VALUE(name);
        FROM_VALUE(new_des);
        FROM_VALUE(level);
        FROM_VALUE(is_forced);
        FROM_RPC_END();
    };

    long long         id;
    string            name;
    string            new_des;
    string            level;
    bool              is_forced;
};


class ApiAffinityRegionQuery:public Rpcable
{
public:

    ApiAffinityRegionQuery(){};
    ~ApiAffinityRegionQuery(){};

    bool Validate(string &err_str)
    {
        ostringstream    oss;

        if(((unsigned long long)count > 200 )
            ||((unsigned long long)count == 0) )
        {
            err_str = "query count error, vaild range is [1,200]";
            return false;
        }

        if ((RPC_QUERY_ALL_SELF != type)
             &&(RPC_QUERY_ALL_APPOINTED != type)
             &&(RPC_QUERY_ALL != type)
             &&(RPC_QUERY_SELF_APPOINTED != type))
        {
            oss << "Incorrect query type:" << type;
            err_str = oss.str();
            return false;
        }

        if ((RPC_QUERY_ALL_APPOINTED == type)
             &&(appointed_user.empty()))
        {
            oss << "query type is :" << type<< " and appointed_user is empty.";
            err_str = oss.str();
            return false;
        }

        return true;
    };

    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_I8(start_index);
        TO_I8(count);
        TO_VALUE(type);
        TO_I8(appointed_id);
        TO_VALUE(appointed_user);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_I8(start_index);
        FROM_I8(count);
        FROM_VALUE(type);
        FROM_I8(appointed_id);
        FROM_VALUE(appointed_user);
        FROM_RPC_END();
    };

    long long   start_index;
    long long   count;
    string      type;        //查询的类型
    long long   appointed_id;//指定查询的域名
    string      appointed_user;   //指定查询的用户名
};

/**
@brief 功能描述: 亲和域查询接口的参数定义 \n
@li @b 其它说明: 无
*/
class ApiAffinityRegionInfo : public Rpcable
{
public:
    ApiAffinityRegionInfo(){};
    ~ApiAffinityRegionInfo(){};

  TO_RPC
    {
        TO_RPC_BEGIN();
        TO_I8(id);
        TO_VALUE(user_name);
        TO_VALUE(name);
        TO_VALUE(description);
        TO_VALUE(level);
        TO_VALUE(is_forced);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_I8(id);
        FROM_VALUE(user_name);
        FROM_VALUE(name);
        FROM_VALUE(description);
        FROM_VALUE(level);
        FROM_VALUE(is_forced);
        FROM_RPC_END();
    };

    long long         id;     //亲和域ID
    string            user_name;  //用户名称
    string            name;
    string            description;
    string            level;
    bool              is_forced;
};

#endif

