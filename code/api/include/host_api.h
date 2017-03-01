/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：host_api.h
* 文件标识：
* 内容摘要：物理机管理模块对外接口
* 当前版本：1.0
* 作    者：袁进坤
* 完成日期：2011年11月24日
*
* 修改记录1：
*    修改日期：2011/11/24
*    版 本 号：V1.0
*    修 改 人：袁进坤
*    修改内容：创建
*******************************************************************************/
#ifndef TECS_HOST_API_H
#define TECS_HOST_API_H

#include "rpcable.h"
#include "api_error.h"

class ApiHostInfo : public Rpcable
{
public:
    // 主机的运行状态, run_state
    enum RunState
    {
        INIT            = 0,    // 初始状态
        OFF_LINE        = 1,    // 离线
        ON_LINE         = 2     // 在线
    };

    // 主机的硬件状态，hardware_state
    enum HardWareState
    {
        NORMAL          = 0,    // 无故障
        ERR_MEMORY      = 1,    // 内存故障
        ERR_OSCILLATOR  = 2,    // 晶振故障
        ERR_MEM_OSC     = 3     // 内存和晶振都故障
    };

    long long       oid;                    // 主机对象的ID
    string          host_name;              // 主机名称
    string          ip_address;             // 主机的IP地址    
    int             run_state;              // 主机的运行状态
    bool            is_disabled;            // 主机是否进入维护模式
    time_t          create_time;            // 主机对象的创建时间 
    int64           running_time;           // 主机对象的在线时间 
    long long       cpu_info_id;            // 主机的CPU物理信息
    string          vmm_info;               // 主机的Hypervisor信息
    string          os_info;                // 主机的操作系统信息
    int             running_vms;            // 主机上运行的虚拟机数量
    int             tcu_max;                // 主机的tcu最大数量
    int             tcu_free;               // 主机的tcu剩余数量
    long long       mem_total;              // 主机的内存总个空间，单位B
    long long       mem_max;                // 主机的内存最大可用空闲空间，单位B
    long long       mem_free;               // 主机的内存当前可用空闲空间，单位B
    long long       disk_total;             // 主机本地存储总空间，单位B
    long long       disk_max;               // 主机本地存储最大可用空间，单位B
    long long       disk_free;              // 主机本地存储当前可用空间，单位B
    int             cpu_usage_1m;           // 最近一次统计的1分钟粒度CPU使用率
    int             cpu_usage_5m;           // 最近一次统计的5分钟粒度CPU使用率
    int             cpu_usage_30m;          // 最近一次统计的30分钟粒度CPU使用率
    long long       hardware_state;         // 主机硬件是否故障，0无故障
    string          description;            // 主机的描述信息
    string          location;                // 主机的位置信息
    bool            is_support_hvm;          // 主机是否支持全虚拟化
    int             rate_power;            // 主机的即时功率, 单位W

    ApiHostInfo() : 
    oid(-1), host_name(), ip_address(), run_state(INIT),
    is_disabled(false), create_time(0),running_time(0),cpu_info_id(-1), vmm_info(), os_info(),
    running_vms(0), tcu_max(0), tcu_free(0), mem_total(0), mem_max(0),
    mem_free(0), disk_total(0), disk_max(0), disk_free(0), cpu_usage_1m(0),
    cpu_usage_5m(0), cpu_usage_30m(0), hardware_state(NORMAL), description(),
    location(),is_support_hvm(), rate_power(0) {}
    ~ApiHostInfo() {}    

    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_I8(oid);
        TO_VALUE(host_name);
        TO_VALUE(ip_address);
        TO_VALUE(run_state);
        TO_VALUE(is_disabled);
        TO_DATETIME(create_time);
        TO_I8(running_time);
        TO_I8(cpu_info_id);
        TO_VALUE(vmm_info);
        TO_VALUE(os_info);
        TO_VALUE(running_vms);
        TO_VALUE(tcu_max);
        TO_VALUE(tcu_free);
        TO_I8(mem_total);
        TO_I8(mem_max);
        TO_I8(mem_free);
        TO_I8(disk_total);
        TO_I8(disk_max);
        TO_I8(disk_free);
        TO_VALUE(cpu_usage_1m);
        TO_VALUE(cpu_usage_5m);
        TO_VALUE(cpu_usage_30m);
        TO_I8(hardware_state);
        TO_VALUE(description);
        TO_VALUE(location);         
        TO_VALUE(is_support_hvm);
        TO_VALUE(rate_power);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_I8(oid);
        FROM_VALUE(host_name);
        FROM_VALUE(ip_address);
        FROM_VALUE(run_state);
        FROM_VALUE(is_disabled);
        FROM_DATETIME(create_time);
        FROM_I8(running_time);		
        FROM_I8(cpu_info_id);
        FROM_VALUE(vmm_info);
        FROM_VALUE(os_info);
        FROM_VALUE(running_vms);
        FROM_VALUE(tcu_max);
        FROM_VALUE(tcu_free);
        FROM_I8(mem_total);
        FROM_I8(mem_max);
        FROM_I8(mem_free);
        FROM_I8(disk_total);
        FROM_I8(disk_max);
        FROM_I8(disk_free);
        FROM_VALUE(cpu_usage_1m);
        FROM_VALUE(cpu_usage_5m);
        FROM_VALUE(cpu_usage_30m);
        FROM_I8(hardware_state);
        FROM_VALUE(description);
        FROM_VALUE(location);                
        FROM_VALUE(is_support_hvm);
        FROM_VALUE(rate_power);
        FROM_RPC_END();
    };
};

class ApiPortSet : public Rpcable
{
public:
    // 端口的启用状态, used
    enum Used
    {
        USED_UNKNOWN    = 0,    // 未知
        USED_YES        = 1,    // 启用
        USED_NO         = 2     // 停用
    };

    // 端口的协商模式, negotiate
    enum Negotiate
    {
        NEGOTIATE_AUTO  = 0,    // 自协商
        NEGOTIATE_FORCE = 1     // 强制
    };

    // 端口的速率模式, speed
    enum Speed
    {
        SPEED_UNKNOWN   = 0,    // 未知
        SPEED_10M       = 1,    // 十兆
        SPEED_100M      = 2,    // 百兆
        SPEED_1000M     = 3,    // 千兆
        SPEED_10G       = 4     // 万兆
    };

    // 端口的双工模式, duplex
    enum Duplex
    {
        DUPLEX_UNKNOWN  = 0,    // 未知
        DUPLEX_FULL     = 1,    // 全双工
        DUPLEX_HALF     = 2     // 半双工
    };

    // 端口的主从模式, master
    enum Master
    {
        MASTER_UNKNOWN  = 0,    // 未知
        MASTER_AUTO     = 1,    // 自适应
        MASTER_MASTER   = 2,    // 主模式
        MASTER_SLAVE    = 3     // 从模式
    };

    int     used;                   // 是否启用
    int     negotiate;              // 协商模式
    int     speed;                  // 配置速率
    int     duplex;                 // 配置工作模式
    int     master;                 // 主从模式
    int     delay_up_time;          // 延时UP时长    
    string  netplane;               // 延时UP时长    
    ApiPortSet() : 
    used(USED_UNKNOWN), negotiate(NEGOTIATE_AUTO), speed(SPEED_UNKNOWN),
    duplex(DUPLEX_UNKNOWN), master(MASTER_UNKNOWN), delay_up_time(0) {}
    ~ApiPortSet() {}

    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_VALUE(used);
        TO_VALUE(negotiate);
        TO_VALUE(speed);
        TO_VALUE(duplex);
        TO_VALUE(master);
        TO_VALUE(delay_up_time);
        TO_VALUE(netplane);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_VALUE(used);
        FROM_VALUE(negotiate);
        FROM_VALUE(speed);
        FROM_VALUE(duplex);
        FROM_VALUE(master);
        FROM_VALUE(delay_up_time); 
        FROM_VALUE(netplane); 
        FROM_RPC_END();
    };
};

class ApiPortInfo : public Rpcable
{
public:
    // 端口的启用状态, used
    enum Used
    {
        USED_UNKNOWN    = 0,    // 未知
        USED_YES        = 1,    // 启用
        USED_NO         = 2     // 停用
    };

    // 端口的协商模式, negotiate
    enum Negotiate
    {
        NEGOTIATE_AUTO  = 0,    // 自协商
        NEGOTIATE_FORCE = 1     // 强制
    };

    // 端口的速率模式, speed/work_speed
    enum Speed
    {
        SPEED_UNKNOWN   = 0,    // 未知
        SPEED_10M       = 1,    // 十兆
        SPEED_100M      = 2,    // 百兆
        SPEED_1000M     = 3,    // 千兆
        SPEED_10G       = 4     // 万兆
    };

    // 端口的双工模式, duplex/work_duplex
    enum Duplex
    {
        DUPLEX_UNKNOWN  = 0,    // 未知
        DUPLEX_FULL     = 1,    // 全双工
        DUPLEX_HALF     = 2     // 半双工
    };

    // 端口的主从模式, master
    enum Master
    {
        MASTER_UNKNOWN  = 0,    // 未知
        MASTER_AUTO     = 1,    // 自适应
        MASTER_MASTER   = 2,    // 主模式
        MASTER_SLAVE    = 3     // 从模式
    };

    // 端口的状态, state
    enum State
    {
        STATE_DOWN      = 0,    // DOWN
        STATE_UP        = 1     // UP
    };
    enum Linked
    {
        LINKED_UNKNOWN  = 0,
        LINKED_YES      = 1,
        LINKED_NO       = 2
    };
    enum Loopback_mode
    {
        LOOPBACK_NO  = 0,
        LOOPBACK_YES      = 1,
    };

    ApiPortInfo() : 
    name(), used(USED_UNKNOWN), negotiate(NEGOTIATE_AUTO), speed(SPEED_UNKNOWN),
    duplex(DUPLEX_UNKNOWN), master(MASTER_UNKNOWN),
    state(STATE_UP), linked(LINKED_UNKNOWN),loopback_mode(LOOPBACK_NO),netplane() {}
    ~ApiPortInfo() {}

    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_VALUE(name);
        TO_VALUE(used);
        TO_VALUE(negotiate);
        TO_VALUE(speed);
        TO_VALUE(duplex);
        TO_VALUE(master);
        TO_VALUE(state);
        TO_VALUE(linked);
        TO_VALUE(sriov_num);
        TO_VALUE(loopback_mode);
        TO_VALUE(netplane);        
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_VALUE(name);
        FROM_VALUE(used);
        FROM_VALUE(negotiate);
        FROM_VALUE(speed);
        FROM_VALUE(duplex);
        FROM_VALUE(master);
        FROM_VALUE(state);
        FROM_VALUE(linked);
        FROM_VALUE(sriov_num); 
        FROM_VALUE(loopback_mode);  
        FROM_VALUE(netplane);              
        FROM_RPC_END();
    };
    string  name;                   // 端口名称
    int     used;                   // 是否启用
    int     negotiate;              // 协商模式
    int     speed;                  // 配置速率
    int     duplex;                 // 配置工作模式
    int     master;                 // 主从模式
    int     state;                  // 端口IF状态
    int     linked;                 // 连接状态 TODO add by xuemzh
    int     sriov_num;              // sriov数目
    int     loopback_mode;          // 自环模式    
    string  netplane;               // 网络平面名称    
};

class ApiTrunkInfo : public Rpcable
{
public:
    // trunk口的类型模式, trunk_type
    enum TrunkType
    {
        TYPE_SHARE      = 1,    // 负荷分担模式
        TYPE_MS         = 2     // 主备模式
    };

    // trunk口的状态, trunk_state
    enum TrunkState
    {
        STATE_DOWN      = 0,    // DOWN
        STATE_UP        = 1     // UP
    };
    
    string          trunk_name;     // trunk口名称
    int             trunk_type;     // trunk口类型
    int             trunk_state;    // trunk口状态
    string          netplane;       // 网络平面名称
    vector<string>  trunk_ports;    // turnk口的成员口名称

    ApiTrunkInfo() : 
    trunk_name(), trunk_type(0), trunk_state(STATE_UP), trunk_ports() {}
    ~ApiTrunkInfo() {}

    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_VALUE(trunk_name);
        TO_VALUE(trunk_type);
        TO_VALUE(trunk_state);
        TO_VALUE(netplane);
        TO_VALUE(trunk_ports);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_VALUE(trunk_name);
        FROM_VALUE(trunk_type);
        FROM_VALUE(trunk_state);
        FROM_VALUE(netplane);
        FROM_VALUE(trunk_ports);     
        FROM_RPC_END();
    };
};

class ApiClusterConfigInfo : public Rpcable
{
public:
    string  config_name;        // 配置项名称
    string  config_value;       // 配置项的值

    ApiClusterConfigInfo() : config_name(), config_value() {}
    ~ApiClusterConfigInfo() {}

    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_VALUE(config_name);
        TO_VALUE(config_value);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_VALUE(config_name);
        FROM_VALUE(config_value);
        FROM_RPC_END();
    };
};

class ApiTcuInfo : public Rpcable
{
public:
    long long   cpu_info_id;    // 物理CPU型号的ID
    int         tcu_num;        // 物理CPU型号所支持的TCU数量
    int         commend_tcu_num; // 系统推荐物理CPU型号所支持的TCU数量
    string      cpu_info;       // 物理CPU型号详细信息
    string      description;    // 物理CPU型号的描述
   
    ApiTcuInfo() : cpu_info_id(-1), tcu_num(0), cpu_info(), description() {}
    ~ApiTcuInfo() {}

    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_I8(cpu_info_id);
        TO_VALUE(tcu_num);
        TO_VALUE(commend_tcu_num);
        TO_VALUE(cpu_info);
        TO_VALUE(description);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_I8(cpu_info_id);
        FROM_VALUE(tcu_num);
        FROM_VALUE(commend_tcu_num);
        FROM_VALUE(cpu_info);
        FROM_VALUE(description);
        FROM_RPC_END();
    };
};


class ApiNetplaneDefaultInfo : public Rpcable
{
public:
    
    string          if_name;   
    string          netplane;   

    ApiNetplaneDefaultInfo() : if_name(), netplane() {}
    ~ApiNetplaneDefaultInfo() {}

    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_VALUE(if_name);
        TO_VALUE(netplane);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_VALUE(if_name);
        FROM_VALUE(netplane);
        FROM_RPC_END();
    };
};

class ApiNetplaneInfo : public Rpcable
{
public:

    ApiNetplaneInfo() : netplane_name(), description(){}
    ~ApiNetplaneInfo() {}

    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_VALUE(netplane_name);
        TO_VALUE(description);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_VALUE(netplane_name);
        FROM_VALUE(description);
        FROM_RPC_END();
    };
    
    
    string          netplane_name;   
    string          description;   
};

class CmmConfigInfo : public Rpcable
{
public: 
    int32  bureau;
    int32  rack;
    int32  shelf;
    string type;
    string ip_address;
    string description;

    CmmConfigInfo() : bureau(0), rack(0), shelf(0) {}
    ~CmmConfigInfo() {}
    
    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_VALUE(bureau);
        TO_VALUE(rack);
        TO_VALUE(shelf);
        TO_VALUE(type);        
        TO_VALUE(ip_address);
        TO_VALUE(description);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_VALUE(bureau);
        FROM_VALUE(rack);
        FROM_VALUE(shelf);
        FROM_VALUE(type);        
        FROM_VALUE(ip_address);
        FROM_VALUE(description);
        FROM_RPC_END();
    };
};


#endif // TECS_HOST_API_H

