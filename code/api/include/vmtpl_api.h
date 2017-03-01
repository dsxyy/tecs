/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：vmtpl_api.h
* 文件标识：
* 内容摘要：虚拟机模板模块对外接口
* 当前版本：1.0
* 作    者：Bob
* 完成日期：2011年10月19日
*
* 修改记录1：
*    修改日期：2011/7/26
*    版 本 号：V1.0
*    修 改 人：Bob
*    修改内容：创建
*******************************************************************************/

#ifndef TECS_VMTPL_API_H
#define TECS_VMTPL_API_H
#include "api_vmbase.h"

class ApiVtAllocateData : public Rpcable
{
public:
    ApiVtAllocateData(){};
    
    ~ApiVtAllocateData(){};
    
	bool Validate(string &err_str)
    {
        if (StringCheck::CheckNormalName(vt_name,1, STR_LEN_32) != true)
        {
            err_str = "Error, invalide parameter with  " + vt_name;
            return false;
        }
        
        return cfg_info.base_info.Validate(err_str);
    };
    
  TO_RPC
    {
        TO_RPC_BEGIN();
        TO_STRUCT(cfg_info);
        TO_VALUE(vt_name);
        TO_VALUE(is_public);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_STRUCT(cfg_info);
        FROM_VALUE(vt_name);
        FROM_VALUE(is_public);
        FROM_RPC_END();
    };
    
    //ApiVmBase  base_info;

    ApiVtCfgInfo cfg_info;

    //虚拟机独有的字段

    /**
    @brief 参数描述: 虚拟机模板名称
    @li @b 是否用户必填: 是
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明: 
    */
    string    vt_name;

    /**
    @brief 参数描述: 虚拟机模板是否公用
    @li @b 是否用户必填: 是
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明: 
    */
    bool      is_public;
};


class ApiVtModifyData : public Rpcable
{
public:
    ApiVtModifyData(){};
    ~ApiVtModifyData(){};
    
    bool Validate(string &err_str)
    {
        if (StringCheck::CheckNormalName(vt_name,1, STR_LEN_32) != true)
        {
            err_str = "Error, invalide parameter with  " + vt_name;
            return false;
        }
        return cfg_info.base_info.Validate(err_str);
    };
    
  TO_RPC
    {
        TO_RPC_BEGIN();
        TO_STRUCT(cfg_info);
        TO_VALUE(vt_name);
        TO_VALUE(is_public);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_STRUCT(cfg_info);
        FROM_VALUE(vt_name);
        FROM_VALUE(is_public);
        FROM_RPC_END();
    };

    
    //ApiVmBase  base_info;

    ApiVtCfgInfo cfg_info;

    //虚拟机独有的字段

    /**
    @brief 参数描述: 虚拟机模板名称
    @li @b 是否用户必填: 是
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明: 
    */
    string    vt_name;

    /**
    @brief 参数描述: 虚拟机模板是否公用
    @li @b 是否用户必填: 是
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明: 
    */
    bool      is_public;
};

enum ApiVtQueryType
{
   VT_USER_CREATE   = 1, /** < User's  VmTemplate   */
   VT_USER_VISIBLE  = 2, /** < User's  and public VmTemplates  */
   VT_APPOINTED     = 3   /** < VmTemplate ID, user's VmTemplate */
};


class ApiVtQuery:public Rpcable
{
public:

    ApiVtQuery(long long tmp_start=0, long long tmp_count =0,long long tmp_type=0)
    {
        start_index = tmp_start;
        count = tmp_count;
        type  = tmp_type;
    };

    bool Validate(string &err_str)
    {
        ostringstream    oss;

        if(((unsigned long long)count > 50 )
            ||((unsigned long long)count == 0) )
        {
            err_str = "query count too large,range [1,50]";
            return false;
        }

        ApiVtQueryType  tmp_type = static_cast <ApiVtQueryType>(type);
        if ((tmp_type < VT_USER_CREATE)
             &&(tmp_type > VT_APPOINTED))
        {
            oss << "Incorrect query type:" << type<< " .";
            err_str = oss.str();
            return false;
        }

        if(VT_APPOINTED ==  tmp_type)
        {
           if(StringCheck::CheckNormalName(vt_name,1, STR_LEN_32) != true)
           {
               oss << "Incorrect vmtemplate name :" << vt_name<< ".";
               err_str = oss.str();
               return false;
           }
        }
        return true;
    };

    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_I8(start_index);
        TO_I8(count);
        TO_VALUE(type);
        TO_VALUE(vt_name);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_I8(start_index);
        FROM_I8(count);
        FROM_VALUE(type);
        FROM_VALUE(vt_name);
        FROM_RPC_END();
    };

    long long   start_index;
    long long   count;
    int         type;
    string      vt_name;
};

class ApiVtInfo : public Rpcable
{
public:
    ApiVtInfo(){};
    ~ApiVtInfo(){};

  TO_RPC
    {
        TO_RPC_BEGIN();
        TO_STRUCT(cfg_info);
        TO_I8(vt_id);
        TO_VALUE(vt_name);
        TO_I8(uid);
        TO_VALUE(user_name);
        TO_VALUE(create_time);
        TO_VALUE(is_public);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_STRUCT(cfg_info);
        FROM_I8(vt_id);
        FROM_VALUE(vt_name);
        FROM_I8(uid);
        FROM_VALUE(user_name);
        FROM_VALUE(create_time);
        FROM_VALUE(is_public);
        FROM_RPC_END();
    };
    
     //虚拟机模板独有的字段
    int64     vt_id;
    string    vt_name;
    int64     uid;
    string    user_name;
    string    create_time;
    bool      is_public; 
    
    ApiVtCfgInfo cfg_info;
    //ApiVmBase  base_info;

};

#endif

