/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：msg_host_manager_with_api_method.h
* 文件标识：见配置管理计划书
* 内容摘要：host_manager 与 api_method 之间的消息体结构定义
* 当前版本：1.0
* 作    者：袁进坤
* 完成日期：2011年8月18日
*
* 修改记录1：
*     修改日期：2011/8/18
*     版 本 号：V1.0
*     修 改 人：袁进坤
*     修改内容：创建
*******************************************************************************/
#ifndef COMM_HOST_MANAGER_WITH_API_METHOD_H
#define COMM_HOST_MANAGER_WITH_API_METHOD_H

#include "tecs_errcode.h"

using namespace std;
namespace zte_tecs {

// tc.api -> cc.HostManager: 修改主机的描述信息请求
// EV_HOST_SET_REQ
class MessageHostSetReq : public Serializable
{
public:
    string  _host_name;
    string  _description;

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageHostSetReq);
        WRITE_STRING(_host_name);
        WRITE_STRING(_description);
        SERIALIZE_END();
    };
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageHostSetReq);
        READ_STRING(_host_name);
        READ_STRING(_description);
        DESERIALIZE_END();
    };
};

// tc.api <- cc.HostManager: 修改主机的描述信息应答
// EV_HOST_SET_ACK
class MessageHostSetAck : public Serializable
{
public:
    string  _host_name;
    int32   _ret_code;

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageHostSetAck);
        WRITE_STRING(_host_name);
        WRITE_DIGIT(_ret_code);
        SERIALIZE_END();
    };
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageHostSetAck);
        READ_STRING(_host_name);
        READ_DIGIT(_ret_code);
        DESERIALIZE_END();
    };
};

// tc.api -> cc.HostManager: 将Forbidden的主机从系统中"遗忘"掉的请求
// EV_HOST_FORGET_REQ
class MessageHostForgetReq : public Serializable
{
public:
    string  _host_name;

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageHostForgetReq);
        WRITE_STRING(_host_name);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageHostForgetReq);
        READ_STRING(_host_name);
        DESERIALIZE_END();
    };
};

// tc.api <- cc.HostManager: 将Forbidden的主机从系统中"遗忘"掉的应答
// EV_HOST_FORGET_ACK
class MessageHostForgetAck : public Serializable
{
public:
    string  _host_name;
    int32   _ret_code;

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageHostForgetAck);
        WRITE_STRING(_host_name);
        WRITE_DIGIT(_ret_code);       
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageHostForgetAck);
        READ_STRING(_host_name);
        READ_DIGIT(_ret_code);
        DESERIALIZE_END();
    };
};

// tc.api -> cc.HostManager: 设置主机进入维护模式的请求
// EV_HOST_DISABLE_REQ
class MessageHostDisableReq : public Serializable
{
public:
    string  _host_name;
    bool    _is_forced;

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageHostDisableReq);
        WRITE_STRING(_host_name);
        WRITE_DIGIT(_is_forced);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageHostDisableReq);
        READ_STRING(_host_name);
        READ_DIGIT(_is_forced);
        DESERIALIZE_END();
    };
};

// tc.api <- cc.HostManager: 设置主机进入维护模式的应答
// EV_HOST_DISABLE_ACK
class MessageHostDisableAck : public Serializable
{
public:
    string  _host_name;
    int32   _ret_code;

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageHostDisableAck);
        WRITE_STRING(_host_name);
        WRITE_DIGIT(_ret_code);       
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageHostDisableAck);
        READ_STRING(_host_name);
        READ_DIGIT(_ret_code);
        DESERIALIZE_END();
    };
};

// tc.api -> cc.HostManager: 设置主机退出维护模式的请求
// EV_HOST_ENABLE_REQ
class MessageHostEnableReq : public Serializable
{
public:
    string  _host_name;

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageHostEnableReq);
        WRITE_STRING(_host_name);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageHostEnableReq);
        READ_STRING(_host_name);
        DESERIALIZE_END();
    };
};

// tc.api <- cc.HostManager: 设置主机退出维护模式的应答
// EV_HOST_ENABLE_ACK
class MessageHostEnableAck : public Serializable
{
public:
    string  _host_name;
    int32   _ret_code;

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageHostEnableAck);
        WRITE_STRING(_host_name);
        WRITE_DIGIT(_ret_code);       
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageHostEnableAck);
        READ_STRING(_host_name);
        READ_DIGIT(_ret_code);
        DESERIALIZE_END();
    };
};

// tc.api -> cc.HostManager: 将主机进行重启的请求
// EV_HOST_REBOOT_REQ
class MessageHostRebootReq : public Serializable
{
public:
    string  _host_name;
    bool    _is_forced;

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageHostRebootReq);
        WRITE_STRING(_host_name);
        WRITE_DIGIT(_is_forced);        
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageHostRebootReq);
        READ_STRING(_host_name);
        READ_DIGIT(_is_forced);        
        DESERIALIZE_END();
    };
};

// tc.api <- cc.HostManager: 将主机进行重启的应答
// EV_HOST_REBOOT_ACK
class MessageHostRebootAck : public Serializable
{
public:
    string  _host_name;
    int32   _ret_code;

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageHostRebootAck);
        WRITE_STRING(_host_name);
        WRITE_DIGIT(_ret_code);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageHostRebootAck);
        READ_STRING(_host_name);
        READ_DIGIT(_ret_code);
        DESERIALIZE_END();
    };
};

// tc.api -> cc.HostManager: 将主机进行关机的请求
// EV_HOST_SHUTDOWN_REQ
class MessageHostShutdownReq : public Serializable
{
public:
    string  _host_name;

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageHostShutdownReq);
        WRITE_STRING(_host_name);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageHostShutdownReq);
        READ_STRING(_host_name);
        DESERIALIZE_END();
    };
};

// tc.api <- cc.HostManager: 将主机进行关机的应答
// EV_HOST_SHUTDOWN_ACK
class MessageHostShutdownAck : public Serializable
{
public:
    string  _host_name;
    int32   _ret_code;

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageHostShutdownAck);
        WRITE_STRING(_host_name);
        WRITE_DIGIT(_ret_code);       
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageHostShutdownAck);
        READ_STRING(_host_name);
        READ_DIGIT(_ret_code);
        DESERIALIZE_END();
    };
};

// tc.api -> cc.HostManager: 查询主机信息的请求
// EV_HOST_QUERY_REQ
class MessageHostQueryReq : public Serializable
{
public:
    int64   _start_index;
    uint64  _query_count;
    string  _host_name;

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageHostQueryReq);
        WRITE_DIGIT(_start_index);
        WRITE_DIGIT(_query_count);
        WRITE_STRING(_host_name);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageHostQueryReq);
        READ_DIGIT(_start_index);
        READ_DIGIT(_query_count);
        READ_STRING(_host_name);
        DESERIALIZE_END();
    };
};

class HostInfoType : public Serializable
{
public:
    int64           _oid;
    string          _host_name;             // 主机名称
    string          _ip_address;            // 主机的IP地址    
    int32           _run_state;             // 主机的运行状态
    bool            _is_disabled;           // 主机是否进入维护模式
    time_t          _create_time;           // 主机对象的创建时间 
    int64           _running_time;          // 主机对象的在线时间     
    int64           _cpu_info_id;           // 主机的CPU物理信息
    string          _vmm_info;              // 主机的Hypervisor信息
    string          _os_info;               // 主机的操作系统信息
    int32           _running_vms;           // 主机上运行的虚拟机数量
    int32           _tcu_max;               // 主机的tcu最大数量
    int32           _tcu_free;               // 主机的tcu剩余数量
    int64           _mem_total;             // 主机的内存总个空间，单位B
    int64           _mem_max;               // 主机的内存最大可用空闲空间，单位B
    int64           _mem_free;              // 主机的内存当前可用空闲空间，单位B
    int64           _disk_total;            // 主机本地存储总空间，单位B
    int64           _disk_max;              // 主机本地存储最大可用空间，单位B
    int64           _disk_free;             // 主机本地存储当前可用空间，单位B
    int32           _cpu_usage_1m;          // 最近一次统计的1分钟粒度CPU使用率
    int32           _cpu_usage_5m;          // 最近一次统计的5分钟粒度CPU使用率
    int32           _cpu_usage_30m;         // 最近一次统计的30分钟粒度CPU使用率
    int64           _hardware_state;        // 主机硬件是否故障，0无故障
    string          _description;           // 主机的描述信息
    string          _location;              // 物理位置
    bool            _is_support_hvm;        // 主机是否支持虚拟化

    int32           _power_state;           //通过cmm查询到的单板状态
    int32           _power_operate;         //所要进行的上下电操作
    int32           _rate_power;            //主机的即时功率(W), 由 power_agent 获取入库
    time_t          _poweroff_time;         // 主机对象的下电时间
    time_t          _total_poweroff_time;   // 主机对象的累计时间
    int32           _cpus;                  // dom0绑定的最大vcpu数目

    SERIALIZE
    {
        SERIALIZE_BEGIN(HostInfoType);
        WRITE_DIGIT(_oid);
        WRITE_STRING(_host_name);
        WRITE_STRING(_ip_address);        
        WRITE_DIGIT(_run_state);
        WRITE_DIGIT(_is_disabled);
        WRITE_DIGIT(_create_time);
        WRITE_DIGIT(_running_time);
        WRITE_DIGIT(_cpu_info_id);
        WRITE_STRING(_vmm_info);
        WRITE_STRING(_os_info);
        WRITE_DIGIT(_running_vms);
        WRITE_DIGIT(_tcu_max);
        WRITE_DIGIT(_tcu_free);
        WRITE_DIGIT(_mem_total);
        WRITE_DIGIT(_mem_max);
        WRITE_DIGIT(_mem_free);
        WRITE_DIGIT(_disk_total);
        WRITE_DIGIT(_disk_max);
        WRITE_DIGIT(_disk_free);
        WRITE_DIGIT(_cpu_usage_1m);
        WRITE_DIGIT(_cpu_usage_5m);
        WRITE_DIGIT(_cpu_usage_30m);
        WRITE_DIGIT(_hardware_state);
        WRITE_STRING(_description);
        WRITE_STRING(_location);
        WRITE_DIGIT(_is_support_hvm);

        WRITE_DIGIT(_power_state);
        WRITE_DIGIT(_power_operate);
        WRITE_DIGIT(_rate_power);
        WRITE_DIGIT(_poweroff_time);
        WRITE_DIGIT(_total_poweroff_time);
        WRITE_DIGIT(_cpus);
        
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(HostInfoType);
        READ_DIGIT(_oid);
        READ_STRING(_host_name);
        READ_STRING(_ip_address);        
        READ_DIGIT(_run_state);
        READ_DIGIT(_is_disabled);
        READ_DIGIT(_create_time);
        READ_DIGIT(_running_time);
        READ_DIGIT(_cpu_info_id);
        READ_STRING(_vmm_info);
        READ_STRING(_os_info);
        READ_DIGIT(_running_vms);
        READ_DIGIT(_tcu_max);
        READ_DIGIT(_tcu_free);
        READ_DIGIT(_mem_total);
        READ_DIGIT(_mem_max);
        READ_DIGIT(_mem_free);
        READ_DIGIT(_disk_total);
        READ_DIGIT(_disk_max);
        READ_DIGIT(_disk_free);
        READ_DIGIT(_cpu_usage_1m);
        READ_DIGIT(_cpu_usage_5m);
        READ_DIGIT(_cpu_usage_30m);
        READ_DIGIT(_hardware_state);
        READ_STRING(_description);
        READ_STRING(_location);
        READ_DIGIT(_is_support_hvm);   
        READ_DIGIT(_power_state);
        READ_DIGIT(_power_operate);
        READ_DIGIT(_rate_power);
        READ_DIGIT(_poweroff_time);
        READ_DIGIT(_total_poweroff_time);
        READ_DIGIT(_cpus);
        
        DESERIALIZE_END();
    };
};

// tc.api <- cc.HostManager: 查询主机信息的应答
// EV_HOST_QUERY_ACK
class MessageHostQueryAck : public Serializable
{
public:
    string  _host_name;
    int32   _ret_code;
    int64   _next_index;
    int64   _max_count;
    vector<HostInfoType> _host_infos;

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageHostQueryAck);
        WRITE_STRING(_host_name);
        WRITE_DIGIT(_ret_code);
        WRITE_DIGIT(_next_index);
        WRITE_DIGIT(_max_count);
        WRITE_OBJECT_VECTOR(_host_infos);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageHostQueryAck);
        READ_STRING(_host_name);
        READ_DIGIT(_ret_code);
        READ_DIGIT(_next_index);
        READ_DIGIT(_max_count);
        READ_OBJECT_VECTOR(_host_infos);
        DESERIALIZE_END();
    };
};

class MessageConfigAck : public Serializable
{
public:
    int32   _ret_code;
    string  _ret_info;

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageConfigAck);
        WRITE_DIGIT(_ret_code);
        WRITE_STRING(_ret_info);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageConfigAck);
        READ_DIGIT(_ret_code);
        READ_STRING(_ret_info);
        DESERIALIZE_END();
    };
};


}  /* end namespace zte_tecs */

#endif /* end COMM_HOST_MANAGER_WITH_API_METHOD_H */

