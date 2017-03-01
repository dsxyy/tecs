/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：project_api.h
* 文件标识：
* 内容摘要：工程模块对外接口
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

#ifndef TECS_PROJECT_API_H
#define TECS_PROJECT_API_H
#include "rpcable.h"
#include "api_error.h"
#include "api_pub.h"


/**
@brief 功能描述: 工程创建接口的参数定义 \n
@li @b 其它说明: 无
*/
class ApiProjectAllocateData : public Rpcable
{
public:
    ApiProjectAllocateData(){};
    ~ApiProjectAllocateData(){};

     bool Validate(string &err_str)
    {
        ostringstream    oss;

        if (StringCheck::CheckNormalName(name,1, STR_LEN_32) != true)
        {
            oss << "Error, invalid parameter with  " << name;
            err_str = oss.str();
            return false;
        }

        if (StringCheck::CheckSize(des, 0, STR_LEN_512) != true)
        {
            oss << "Error, invalid parameter with  " << des;
            err_str = oss.str();
            return false;
        }
        return true;

    }

    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_VALUE(name);
        TO_VALUE(des);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_VALUE(name);
        FROM_VALUE(des);
        FROM_RPC_END();
    };

    string            name;
    string            des;
};

/**
@brief 功能描述: 工程克隆接口的参数定义 \n
@li @b 其它说明: 无
*/
class ApiProjectCloneData : public Rpcable
{
public:
    ApiProjectCloneData(){};
    ~ApiProjectCloneData(){};

     bool Validate(string &err_str)
    {
        ostringstream    oss;

        if (StringCheck::CheckNormalName(old_name,1, STR_LEN_32) != true)
        {
            oss << "Error, invalid parameter with  " << old_name;
            err_str = oss.str();
            return false;
        }

        if (StringCheck::CheckNormalName(new_name,1, STR_LEN_32) != true)
        {
            oss << "Error, invalid parameter with  " << new_name;
            err_str = oss.str();
            return false;
        }

        if (StringCheck::CheckSize(des, 0, STR_LEN_512) != true)
        {
            oss << "Error, invalid parameter with  " << des;
            err_str = oss.str();
            return false;
        }
        return true;
    }

    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_VALUE(old_name);
        TO_VALUE(new_name);
        TO_VALUE(des);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_VALUE(old_name);
        FROM_VALUE(new_name);
        FROM_VALUE(des);
        FROM_RPC_END();
    };

    string            old_name;
    string            new_name;
    string            des;
};


/**
@brief 功能描述: 工程修改接口的参数定义 \n
@li @b 其它说明: 无
*/
class ApiProjectModifyData : public Rpcable
{
public:
    ApiProjectModifyData(){};
    ~ApiProjectModifyData(){};

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
        return true;

    }

    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_VALUE(name);
        TO_VALUE(new_des);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_VALUE(name);
        FROM_VALUE(new_des);
        FROM_RPC_END();
    };

    string            name;
    string            new_des;
};

enum ApiProjectQueryType
{
    PROJECT_USER_CREATE     = -5, /** < 当前用户的  */
    PROJECT_USER_APPOINTED  = -4, /** < 指定用户的, 仅admin用户可以 */
    PROJECT_USER_ALL        = -3, /** < 所有用户的, 仅admin用户可以 */
    PROJECT_APPOINTED       = 0   /** < 指定工程查询 */
};


class ApiProjectQuery:public Rpcable
{
public:

    ApiProjectQuery(long long tmp_start=0,
                         long long tmp_count =0,
                         long long tmp_type=0,
                         const string  &prj = "",
                         const string  &user = "")
    {
        start_index = tmp_start;
        count = tmp_count;
        type  = tmp_type;
        appointed_project = prj;
        appointed_user    = user;
    };

    bool Validate(string &err_str)
    {
        ostringstream    oss;

        if(((unsigned long long)count > 200 )
            ||((unsigned long long)count == 0) )
        {
            err_str = "query count error, vaild range is [1,200]";
            return false;
        }

        ApiProjectQueryType  tmp_type = static_cast <ApiProjectQueryType>(type);
        if ((PROJECT_USER_CREATE != tmp_type)
             &&(PROJECT_USER_APPOINTED != tmp_type)
             &&(PROJECT_USER_ALL != tmp_type)
             &&(PROJECT_APPOINTED != tmp_type))
        {
            oss << "Incorrect query type:" << type<< " .";
            err_str = oss.str();
            return false;
        }

        if ((PROJECT_USER_APPOINTED == tmp_type)
             &&(appointed_user.empty()))
        {
            oss << "query type is :" << type<< " and appointed_user is empty.";
            err_str = oss.str();
            return false;
        }

        if ((PROJECT_APPOINTED == tmp_type)
             &&(appointed_project.empty()))
        {
            oss << "query type is :" << type<< " and appointed_project is empty.";
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
        TO_I8(type);
        TO_VALUE(appointed_project);
        TO_VALUE(appointed_user);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_I8(start_index);
        FROM_I8(count);
        FROM_I8(type);
        FROM_VALUE(appointed_project);
        FROM_VALUE(appointed_user);
        FROM_RPC_END();
    };

    long long   start_index;
    long long   count;
    long long   type;        //查询的类型
    string      appointed_project;//指定查询的工程名
    string      appointed_user;   //指定查询的用户名
};

enum ProjectOperation
{
    //TC部分
    PROJECT_DEPLOY    = 1,
    PROJECT_CENCEL    = 2,
    PROJECT_REBOOT    = 3,
    PROJECT_RESET     = 4
};

/**
@brief 功能描述: 工程操作接口的参数定义 \n
@li @b 其它说明: 无
*/
class ApiProjectActionData : public Rpcable
{
public:
    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_VALUE(project_name);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_VALUE(project_name);
        FROM_RPC_END();
    };

    string            project_name;
};

/**
@brief 功能描述: 工程查询接口的参数定义 \n
@li @b 其它说明: 无
*/
class ApiProjectInfo : public Rpcable
{
public:
    ApiProjectInfo(){};
    ~ApiProjectInfo(){};

  TO_RPC
    {
        TO_RPC_BEGIN();
        TO_I8(id);
        TO_VALUE(uname);
        TO_VALUE(name);
        TO_VALUE(des);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_I8(id);
        FROM_VALUE(uname);
        FROM_VALUE(name);
        FROM_VALUE(des);
        FROM_RPC_END();
    };

    long long         id;  //工程ID
    string            uname;  //用户名称
    string            name;
    string            des;
};


class ApiProjectVmQuery :public Rpcable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:

    ApiProjectVmQuery() {};

    ~ApiProjectVmQuery() {};

    bool Validate(string &err_str)
    {
        ostringstream    oss;

        if(((unsigned long long)count > 50 )
            ||((unsigned long long)count == 0) )
        {
            err_str = "query count error, vaild range is [1,200]";
            return false;
        }

        return true;
    };

     TO_RPC
    {
        TO_RPC_BEGIN();
        TO_I8(start_index);
        TO_I8(count);
        TO_VALUE(appointed_project);
        TO_VALUE(appointed_user);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_I8(start_index);
        FROM_I8(count);
        FROM_VALUE(appointed_project);
        FROM_VALUE(appointed_user);
        FROM_RPC_END();
    };


    int64   start_index;
    int64   count;
    string  appointed_project;
    string  appointed_user;

};

class ApiProjectDataQuery :public Rpcable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:

    ApiProjectDataQuery() 
    {
        start_index = -1;
        count = 0;
    };

    ~ApiProjectDataQuery() {};

    bool Validate(string &err_str)
    {
        ostringstream    oss;

        if(((unsigned long long)count > 200 )
            ||((unsigned long long)count == 0) )
        {
            err_str = "query count error, vaild range is [1,200]";
            return false;
        }

        return true;
    };
    
     TO_RPC
    {
        TO_RPC_BEGIN();
        TO_I8(start_index);
        TO_I8(count);
        TO_VALUE(appointed_project);
        TO_VALUE(appointed_user);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_I8(start_index);
        FROM_I8(count);
        FROM_VALUE(appointed_project);
        FROM_VALUE(appointed_user);
        FROM_RPC_END();
    };


    int64   start_index;
    uint64  count;
    string  appointed_project;
    string  appointed_user;

};

class ApiProjectDynamicData :public Rpcable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:

    ApiProjectDynamicData() 
    {
        vm_id = -1;
        nic_used_rate = 0;
        cpu_used_rate = 0;
        mem_used_rate = 0;
    };

    ~ApiProjectDynamicData() {};

    
     TO_RPC
    {
        TO_RPC_BEGIN();
        TO_I8(vm_id);
        TO_I8(nic_used_rate);
        TO_I8(cpu_used_rate);
        TO_I8(mem_used_rate);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_I8(vm_id);
        FROM_I8(nic_used_rate);
        FROM_I8(cpu_used_rate);
        FROM_I8(mem_used_rate);	
        FROM_RPC_END();
    };

    long long  vm_id;   //虚拟机id
    long long  nic_used_rate;//网卡利用率
    long long  cpu_used_rate;// cpu利用率
    long long  mem_used_rate;//内存利用率

};


class ApiProjectVmInfo : public Rpcable
{
public:
    ApiProjectVmInfo(){};
    ~ApiProjectVmInfo(){};

  TO_RPC
    {
        TO_RPC_BEGIN();
        TO_VALUE(vm_name);
        TO_I8(vm_id);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_VALUE(vm_name);
        FROM_I8(vm_id);
        FROM_RPC_END();
    };


    string     vm_name; //虚拟机独有的字段
    long long  vm_id;   //虚拟机id
};

class ApiProjectSaveAsImageData : public Rpcable
{
public:
    ApiProjectSaveAsImageData()
    {
        is_deployed_only = false;
        is_image_save = true;
    };
    ~ApiProjectSaveAsImageData(){};

  TO_RPC
    {
        TO_RPC_BEGIN();
        TO_VALUE(project_name);
        TO_VALUE(is_deployed_only);
        TO_VALUE(is_image_save);
        TO_VALUE(description);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_VALUE(project_name);
        FROM_VALUE(is_deployed_only);
        FROM_VALUE(is_image_save);
        FROM_VALUE(description);
        FROM_RPC_END();
    };

    string project_name;    //工程名
    bool  is_deployed_only; //是否只包含已经部署的虚拟机
    bool  is_image_save;    //是否包含镜像
    string description;     //工程镜像描述信息
};

class ApiProjectCreateByImageData : public Rpcable
{
public:
    ApiProjectCreateByImageData()
    {
        image_id = -1;
    };
    ~ApiProjectCreateByImageData(){};

  TO_RPC
    {
        TO_RPC_BEGIN();
        TO_I8(image_id);
        TO_VALUE(project_name);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_I8(image_id);
        FROM_VALUE(project_name);
        FROM_RPC_END();
    };

    long long image_id;    //镜像ID
    string project_name;
};

/**
@brief 功能描述: 工程统计静态数据接口的参数定义 \n
@li @b 其它说明: 无
*/
class ApiProjectStatisticsStaticData : public Rpcable
{
public:
    ApiProjectStatisticsStaticData()
    {
        id = 0;
        vm_sum = 0;
        tcu_sum = 0;
        memory_sum = 0;
        nic_sum = 0;
        storage_share = 0;
        storage_local = 0;
    };
    ~ApiProjectStatisticsStaticData(){};

    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_I8(id);
        TO_VALUE(name);
        TO_I8(vm_sum);
        TO_I8(tcu_sum);
        TO_I8(memory_sum);
        TO_I8(nic_sum);
        TO_I8(storage_share);
        TO_I8(storage_local);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_I8(id);
        FROM_VALUE(name);  
        FROM_I8(vm_sum);
        FROM_I8(tcu_sum);
        FROM_I8(memory_sum);
        FROM_I8(nic_sum);
        FROM_I8(storage_share);
        FROM_I8(storage_local);
        FROM_RPC_END();
    };
    
    long long         id;  //工程ID
    string            name;  //工程名称
    long long         vm_sum; //vm总数
    long long         tcu_sum; // tcu总数
    long long         memory_sum; //内存总量
    long long         nic_sum; //网卡总数
    long long         storage_share; //共享存储总量
    long long         storage_local; //本地存储总量
};

#endif

