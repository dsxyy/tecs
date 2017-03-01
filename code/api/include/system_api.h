/*******************************************************************************
* Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：system_api.h
* 文件标识：
* 内容摘要：系统模块对外接口
* 当前版本：1.0
* 作    者：张文剑
* 完成日期：2012年07月07日
*
* 修改记录1：
*    修改日期：2012/07/07
*    版 本 号：V1.0
*    修 改 人：张文剑
*    修改内容：创建
*
* 修改记录2：
*    修改日期：2012/07/10
*    版 本 号：V1.1
*    修 改 人：张文剑
*    修改内容：将set_runtime_state函数的实现移动到method实现文件中，
*              因为那里可以看到一些内部的宏定义，如S_Startup/S_Work
*******************************************************************************/
#ifndef TECS_SYSTEM_API_H
#define TECS_SYSTEM_API_H
#include "rpcable.h"
#include "api_error.h"
#include "api_const.h"

#ifndef MAX
#define	MAX(a,b) (((a)>(b))?(a):(b))
#endif

class MuRuntimeInfo;
class ApiMuRuntimeInfo: public Rpcable
{
public:
    ApiMuRuntimeInfo()
    {
        exist = false;
        age = 0;
        runtime_state = RUNTIME_STATE_NORMAL;
        state = 0;
        pid = 0;
        push_count = 0;
        pop_count = 0;
        queued_count = 0;
        timer_count = 0;
        last_message = 0;
    };
    
    ApiMuRuntimeInfo(const string& _name):name(_name)
    {
        exist = false;
        age = 0;
        runtime_state = RUNTIME_STATE_NORMAL;
        state = 0;
        pid = 0;
        push_count = 0;
        pop_count = 0;
        queued_count = 0;
        timer_count = 0;
        last_message = 0;
    };

    ApiMuRuntimeInfo(const MuRuntimeInfo& info);
    void set_runtime_state();

    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_VALUE(name);
        TO_VALUE(exist);
        TO_I8(age);
        TO_VALUE(state);
        TO_VALUE(runtime_state);
        TO_VALUE(pid);
        TO_I8(push_count);
        TO_I8(pop_count);
        TO_VALUE(queued_count);
        TO_VALUE(timer_count);
        TO_VALUE(last_message);
        TO_RPC_END();
    }

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_VALUE(name);
        FROM_VALUE(exist);
        FROM_I8(age);
        FROM_VALUE(state);
        FROM_VALUE(runtime_state);
        FROM_VALUE(pid);
        FROM_I8(push_count);
        FROM_I8(pop_count);
        FROM_VALUE(queued_count);
        FROM_VALUE(timer_count);
        FROM_VALUE(last_message);
        FROM_RPC_END();
    }
    
    string name;   //mu名称
    bool exist;  //消息单元是否存在，即信息是否成功获取到
    int state;     //mu状态，上电，启动，退出等等。。。。
    long long age; //注册成为关键消息单元已经多长时间了
    int runtime_state;
    int pid;     //mu处理线程的pid
    unsigned long long push_count; //mu接收的消息数量
    unsigned long long pop_count;  //mu处理的消息数量
    int queued_count; //mu等待队列中的消息数量
    int timer_count; //mu申请的定时器资源数量
    int last_message;  //mu最后一条处理的消息id
};

class ProcRuntimeInfo;
class ApiProcRuntimeInfo: public Rpcable
{
public:    
    ApiProcRuntimeInfo()
    {
        exist = false;
        runtime_state = RUNTIME_STATE_NORMAL;
        pid = 0;
        running_seconds = 0;
        exceptions = 0;
    };

    ApiProcRuntimeInfo(const string& _name):name(_name)
    {
        exist = false;
        runtime_state = RUNTIME_STATE_NORMAL;
        pid = 0;
        running_seconds = 0;
        exceptions = 0;
    };

    ApiProcRuntimeInfo(const ProcRuntimeInfo& info);
    void set_runtime_state();

    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_VALUE(name);
        TO_VALUE(exist);
        TO_VALUE(pid);
        TO_VALUE(runtime_state);
        TO_VALUE(running_seconds);
        TO_VALUE(exceptions);
        TO_VALUE(asserts);
        TO_STRUCT_ARRAY(muinfo);
        TO_RPC_END();
    }

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_VALUE(name);
        FROM_VALUE(exist);
        FROM_VALUE(runtime_state);
        FROM_VALUE(pid);
        FROM_VALUE(running_seconds);
        FROM_VALUE(exceptions);
        FROM_VALUE(asserts);
        FROM_STRUCT_ARRAY(muinfo);
        FROM_RPC_END();
    }

    string name;   //process名称
    bool exist;  //进程是否存在，即信息是否成功获取到
    int runtime_state;
    int pid;     //本进程pid
    int running_seconds;  //本进程已经运行了多少秒？
    int exceptions; //本进程发生过过少次异常
    vector<string> asserts; //本进程发生过的断言记录
    vector<ApiMuRuntimeInfo> muinfo;  //本进程中的消息单元信息数组
};

class AppRuntimeInfo;
class ApiAppRuntimeInfo: public Rpcable
{
public:
    ApiAppRuntimeInfo()
    {
        runtime_state = RUNTIME_STATE_NORMAL;
    };
    
    ApiAppRuntimeInfo(const string& _name):name(_name)
    {
        runtime_state = RUNTIME_STATE_NORMAL;
    };

    ApiAppRuntimeInfo(const AppRuntimeInfo& info);
    void set_runtime_state();
    
    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_VALUE(target);
        TO_VALUE(name);
        TO_DATETIME(collect_time);
        TO_VALUE(runtime_state);
        TO_STRUCT_ARRAY(procinfo);
        TO_RPC_END();
    }

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_VALUE(target);
        FROM_VALUE(name);
        FROM_DATETIME(collect_time);
        FROM_VALUE(runtime_state);
        FROM_STRUCT_ARRAY(procinfo);
        FROM_RPC_END();
    }

    string target;                              //本次查询的入参target
    string name;                                //本次查询的application名称
    time_t collect_time;                        //本次信息采集的时间
    int runtime_state;
    vector<ApiProcRuntimeInfo> procinfo;  //本application的所有进程信息数组
};

class ApiLicenseInfo
{
public:
    ApiLicenseInfo()
    {
        max_deployed_vms = 0;
        max_created_vms = 0;
        max_images = 0;
        max_users = 0;
        max_clusters = 0;
        max_hosts = 0;
        effective_days = 0;
        expired_date = 0;
    };
    
    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_VALUE(customer_name);
        TO_VALUE(customer_company);
        TO_VALUE(customer_address);
        TO_VALUE(customer_phone);
        TO_VALUE(customer_email);
        TO_VALUE(customer_description);
        TO_I8(max_created_vms);
        TO_I8(max_deployed_vms);
        TO_I8(max_images);
        TO_I8(max_users);
        TO_I8(max_clusters);
        TO_I8(max_hosts);
        TO_I8(current_created_vms);
        TO_I8(current_deployed_vms);
        TO_I8(current_images);
        TO_I8(current_users);
        TO_I8(current_clusters);    
        TO_VALUE(effective_days);
        TO_DATETIME(expired_date);
        TO_VALUE(remaining_days);
        TO_VALUE(hardware_fingerprints);
        //TO_VALUE(special_cpus);
        TO_RPC_END();
    }

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_VALUE(customer_name);
        FROM_VALUE(customer_company);
        FROM_VALUE(customer_address);
        FROM_VALUE(customer_phone);
        FROM_VALUE(customer_email);
        FROM_VALUE(customer_description);
        FROM_I8(max_created_vms);
        FROM_I8(max_deployed_vms);
        FROM_I8(max_images);
        FROM_I8(max_users);
        FROM_I8(max_clusters);
        FROM_I8(max_hosts);
        FROM_I8(current_created_vms);
        FROM_I8(current_deployed_vms);
        FROM_I8(current_images);
        FROM_I8(current_users);
        FROM_I8(current_clusters);    
        FROM_VALUE(effective_days);
        FROM_DATETIME(expired_date);
        FROM_VALUE(remaining_days);
        FROM_VALUE(hardware_fingerprints);
        //FROM_VALUE(special_cpus);
        FROM_RPC_END();
    }
    
    string customer_name;
    string customer_company;
    string customer_address;
    string customer_phone;
    string customer_email;
    string customer_description;
    int64 max_created_vms;
    int64 max_deployed_vms;
    int64 max_images;
    int64 max_users;
    int64 max_clusters;
    int64 max_hosts;
    int64 current_created_vms;
    int64 current_deployed_vms;
    int64 current_images;
    int64 current_users;
    int64 current_clusters;
    int effective_days;
    time_t expired_date;
    int remaining_days;
    vector<string> hardware_fingerprints;
    //vector<string> special_cpus;
};
#endif

