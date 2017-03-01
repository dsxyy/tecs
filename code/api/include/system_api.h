/*******************************************************************************
* Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�system_api.h
* �ļ���ʶ��
* ����ժҪ��ϵͳģ�����ӿ�
* ��ǰ�汾��1.0
* ��    �ߣ����Ľ�
* ������ڣ�2012��07��07��
*
* �޸ļ�¼1��
*    �޸����ڣ�2012/07/07
*    �� �� �ţ�V1.0
*    �� �� �ˣ����Ľ�
*    �޸����ݣ�����
*
* �޸ļ�¼2��
*    �޸����ڣ�2012/07/10
*    �� �� �ţ�V1.1
*    �� �� �ˣ����Ľ�
*    �޸����ݣ���set_runtime_state������ʵ���ƶ���methodʵ���ļ��У�
*              ��Ϊ������Կ���һЩ�ڲ��ĺ궨�壬��S_Startup/S_Work
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
    
    string name;   //mu����
    bool exist;  //��Ϣ��Ԫ�Ƿ���ڣ�����Ϣ�Ƿ�ɹ���ȡ��
    int state;     //mu״̬���ϵ磬�������˳��ȵȡ�������
    long long age; //ע���Ϊ�ؼ���Ϣ��Ԫ�Ѿ��೤ʱ����
    int runtime_state;
    int pid;     //mu�����̵߳�pid
    unsigned long long push_count; //mu���յ���Ϣ����
    unsigned long long pop_count;  //mu�������Ϣ����
    int queued_count; //mu�ȴ������е���Ϣ����
    int timer_count; //mu����Ķ�ʱ����Դ����
    int last_message;  //mu���һ���������Ϣid
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

    string name;   //process����
    bool exist;  //�����Ƿ���ڣ�����Ϣ�Ƿ�ɹ���ȡ��
    int runtime_state;
    int pid;     //������pid
    int running_seconds;  //�������Ѿ������˶����룿
    int exceptions; //�����̷��������ٴ��쳣
    vector<string> asserts; //�����̷������Ķ��Լ�¼
    vector<ApiMuRuntimeInfo> muinfo;  //�������е���Ϣ��Ԫ��Ϣ����
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

    string target;                              //���β�ѯ�����target
    string name;                                //���β�ѯ��application����
    time_t collect_time;                        //������Ϣ�ɼ���ʱ��
    int runtime_state;
    vector<ApiProcRuntimeInfo> procinfo;  //��application�����н�����Ϣ����
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

