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
#ifndef COMM_MSG_HOST_MANAGER_WITH_HOST_AGENT_H
#define COMM_MSG_HOST_MANAGER_WITH_HOST_AGENT_H

#include "serialization.h"
#include "message.h"
#include "vm_messages.h"

using namespace std;
namespace zte_tecs {

// cc.HostManager -> hc.HostAgent: 将主机注册到集群的请求
// EV_HOST_REGISTER_REQ
class MessageHostRegisterReq : public Serializable
{
public:
    string  _host_name;
    map<string, string> _cc_ifs_info;

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageHostRegisterReq);
        WRITE_STRING(_host_name);
        WRITE_VALUE(_cc_ifs_info);
        SERIALIZE_END();
    };
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageHostRegisterReq);
        READ_STRING(_host_name);
        READ_VALUE(_cc_ifs_info);
        DESERIALIZE_END();
    };
};

// cc.HostManager -> hc.HostAgent: 将主机从集群中注销的请求
// EV_HOST_UNREGISTER_REQ
class MessageHostUnregisterReq : public Serializable
{
public:
    string  _host_name;

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageHostUnregisterReq);
        WRITE_STRING(_host_name);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageHostUnregisterReq);
        READ_STRING(_host_name);
        DESERIALIZE_END();
    };
};


class CpuInfo: public Serializable
{
public:
    map<string, string> _info;
    SERIALIZE
    {
        SERIALIZE_BEGIN(CpuInfo);
        WRITE_STRING_MAP(_info);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CpuInfo);
        READ_STRING_MAP(_info);       
        DESERIALIZE_END();
    };
};

class PciPthDevInfo: public Serializable
{
public:
    PciPthDevInfo(int32 type=0, int number=0, const string & des="") : _type(type), _number(number), _description(des) {}
    int32           _type;                   // 设备类型
    int32           _number;                 //设备数量
    string          _description;            // 设备描述，比如“D16E1_2 subcard”
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(PciPthDevInfo);
        WRITE_DIGIT(_type);
        WRITE_DIGIT(_number);
        WRITE_STRING(_description);
        SERIALIZE_END();
    };
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(PciPthDevInfo);
        READ_DIGIT(_type);
        READ_DIGIT(_number);
        READ_STRING(_description);
        DESERIALIZE_END();
    };
};

class BoardPosition : public Serializable
{
public:
    BoardPosition():bureau(0), rack(0), shelf(0), slot(0){}

    int32 bureau;    
    int32 rack;    
    int32 shelf;        
    int32 slot; 

    bool Validate()
    {
        if (bureau == 0 && rack == 0 && shelf == 0 && slot == 0)
        {
            return false;
        }

        return true;
    }

    SERIALIZE
    {
        SERIALIZE_BEGIN(BoardPosition);
        WRITE_DIGIT(bureau);
        WRITE_DIGIT(rack);
        WRITE_DIGIT(shelf); 
        WRITE_DIGIT(slot); 
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(BoardPosition);
        READ_DIGIT(bureau);
        READ_DIGIT(rack);
        READ_DIGIT(shelf);
        READ_DIGIT(slot); 
        DESERIALIZE_END();
    };
};

// cc.HostManager <- hc.HostAgent: 主机发现主动上报
// EV_HOST_DISCOVER
// cc.HostManager <- hc.HostAgent: 获取主机信息的应答
// EV_HOST_INFO_ACK
class MessageHostBaseInfo : public Serializable
{
public:
    string          _ip_address;            // 主机的IP地址        
    string          _media_address;         // 主机的媒体面地址
    CpuInfo         _cpu_info;              // 主机的CPU物理信息
    string          _vmm_info;              // 主机的Hypervisor信息
    string          _os_info;               // 主机的操作系统信息
    int64           _running_time;          // 主机的在线时间，单位s
    int64           _mem_total;             // 主机的内存总个空间，单位B
    int64           _mem_max;               // 主机的内存最大可用空闲空间，单位B
    int64           _disk_total;            // 主机本地存储总空间，单位B
    int64           _disk_max;              // 主机本地存储最大可用空间，单位B
    int32           _mem_state;             // 内存是否硬件故障    
    bool            _is_support_hvm;        // 内存是否支持全虚拟化        
    map<string,string> _location;
    string          _ssh_key;               // ssh密钥
    string          _shelf_type;            // 主机所在的机框类型
    string          _iscsiname;
    vector<PciPthDevInfo> _sdevice_info;    //特殊设备信息，例如后插卡
    BoardPosition   _phy_position;          //物理位置信息，包括局，架，框，槽
    int32           _board_type;            //物理位置信息，包括局，架，框，槽
    int32           _cpus;                  // 可用于部署虚拟机的cpu数目

    MessageHostBaseInfo()
    {
        _running_time = 0;
        _mem_total = 0;
        _mem_max = 0;
        _disk_total = 0;
        _disk_max = 0;
        _mem_state = 0;
        _is_support_hvm = 0;
        _board_type = 0;
        _cpus = 0;
    };    

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageHostBaseInfo);
        WRITE_STRING(_ip_address);
        WRITE_STRING(_media_address);
        WRITE_OBJECT(_cpu_info);
        WRITE_STRING(_vmm_info);
        WRITE_STRING(_os_info);  
        WRITE_DIGIT(_running_time);		
        WRITE_DIGIT(_mem_total);
        WRITE_DIGIT(_mem_max);
        WRITE_DIGIT(_disk_total);
        WRITE_DIGIT(_disk_max); 
        WRITE_DIGIT(_mem_state); 
        WRITE_DIGIT(_is_support_hvm);        
        WRITE_STRING_MAP(_location);   
        WRITE_STRING(_ssh_key);       
        WRITE_STRING(_shelf_type);
        WRITE_STRING(_iscsiname);
        WRITE_OBJECT_ARRAY(_sdevice_info);
        WRITE_OBJECT(_phy_position);
        WRITE_DIGIT(_board_type); 
        WRITE_DIGIT(_cpus); 
        
        SERIALIZE_END();
    };
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageHostBaseInfo);
        READ_STRING(_ip_address);
        READ_STRING(_media_address);
        READ_OBJECT(_cpu_info);
        READ_STRING(_vmm_info);
        READ_STRING(_os_info);  
        READ_DIGIT(_running_time);		
        READ_DIGIT(_mem_total);
        READ_DIGIT(_mem_max);
        READ_DIGIT(_disk_total);
        READ_DIGIT(_disk_max); 
        READ_DIGIT(_mem_state); 
        READ_DIGIT(_is_support_hvm);        
        READ_STRING_MAP(_location);  
        READ_STRING(_ssh_key);        
        READ_STRING(_shelf_type);
        READ_STRING(_iscsiname);
        READ_OBJECT_ARRAY(_sdevice_info);
        READ_OBJECT(_phy_position); 
        READ_DIGIT(_board_type); 
        READ_DIGIT(_cpus); 
        
        DESERIALIZE_END();
    };
};


class DiskInfo : public Serializable
{
public:
    DiskInfo()
    {
        _type = 0;
        _status = 0;
    };

    string          _name;                  // 硬盘名字, 如/dev/sda
    int32           _type;                  // 硬盘类型, 1 - ATA, 2 - SCSI
    int32           _status;                // 硬盘健康状态, 1 - normal, 2 - warn, 3 - fault
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(DiskInfo);
        WRITE_VALUE(_name);
        WRITE_VALUE(_type);
        WRITE_VALUE(_status);
        SERIALIZE_END();
    };
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(DiskInfo);
        READ_VALUE(_name);
        READ_VALUE(_type);
        READ_VALUE(_status);
        DESERIALIZE_END();
    };
};

// 主机监控的硬件信息
// 传感器类型
#define SENSOR_TYPE_UNKNOWN ((int32)0)
#define SENSOR_TYPE_TEMP    ((int32)1)
#define SENSOR_TYPE_CPUVID  ((int32)2)
#define SENSOR_TYPE_FANRPM  ((int32)3)
#define SENSOR_TYPE_IN      ((int32)4)
class HostHwmon : public Serializable
{
public:
    HostHwmon()
    {
        _type = SENSOR_TYPE_UNKNOWN;
    };

    string _name;
    int32  _type;
    string _value;

    SERIALIZE
    {
        SERIALIZE_BEGIN(HostHwmon);
        WRITE_VALUE(_name);
        WRITE_VALUE(_type);
        WRITE_VALUE(_value);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(HostHwmon);
        READ_VALUE(_name);
        READ_VALUE(_type);
        READ_VALUE(_value);
        DESERIALIZE_END();
    };
};

class MessageHostInfoReport : public Serializable
{
public:
    int32               _cpu_usage;         // 物理CPU使用率
    map<string,string>  _location;          // 本地的物理位置信息(架框槽)
    vector<DiskInfo>    _vec_disk_info;     // 本地硬盘的信息
    int64               _running_time;      // 主机的在线时间，单位s
    int64               _disk_total;        // 主机本地存储总空间，单位B
    int64               _disk_max;          // 主机本地存储最大可用空间，单位B    
    int32               _oscillator;        // 晶振是否停振，0正常，1故障
    BoardPosition       _phy_position;      //物理位置信息，包括局，架，框，槽
    int32               _board_type;        //物理位置信息，包括局，架，框，槽
    vector<HostHwmon>   _vec_host_hwmon;    // 主机监控的硬件信息
    ResourceStatistics  _host_statistics;   // 物理机资源使用监控与统计信息
    string              _ip_address;        // 物理机的IP地址 
    
    MessageHostInfoReport()
    {
        _cpu_usage = 0;
        _running_time = 0;
        _disk_total = 0;
        _disk_max = 0;
        _oscillator = 0;
        _board_type = 0;
    };
        
    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageHostInfoReport);
        WRITE_DIGIT(_cpu_usage);
        WRITE_STRING_MAP(_location);
        WRITE_OBJECT_ARRAY(_vec_disk_info);
        WRITE_DIGIT(_running_time);
        WRITE_DIGIT(_disk_total);
        WRITE_DIGIT(_disk_max);
        WRITE_DIGIT(_oscillator);
        WRITE_OBJECT(_phy_position);
        WRITE_DIGIT(_board_type);
        WRITE_OBJECT_ARRAY(_vec_host_hwmon);
        WRITE_OBJECT(_host_statistics);
        WRITE_STRING(_ip_address);        
        SERIALIZE_END();
    };
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageHostInfoReport);
        READ_DIGIT(_cpu_usage);
        READ_STRING_MAP(_location);
        READ_OBJECT_ARRAY(_vec_disk_info);
        READ_DIGIT(_running_time);
        READ_DIGIT(_disk_total);
        READ_DIGIT(_disk_max);
        READ_DIGIT(_oscillator);    
        READ_OBJECT(_phy_position);
        READ_DIGIT(_board_type);
        READ_OBJECT_ARRAY(_vec_host_hwmon);
        READ_OBJECT(_host_statistics);
        READ_STRING(_ip_address);         
        DESERIALIZE_END();
    };
};


class MessageConfigAckFromHC : public Serializable
{
public:
    int32   _ret_code;
    string  _ret_info;
    MID     _apisvr;         
    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageConfigAckFromHC);
        WRITE_DIGIT(_ret_code);
        WRITE_STRING(_ret_info);
        WRITE_OBJECT(_apisvr);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageConfigAckFromHC);
        READ_DIGIT(_ret_code);
        READ_STRING(_ret_info);
        READ_OBJECT(_apisvr);
        DESERIALIZE_END();
    };
};



} //namespace zte_tecs

#endif // COMM_MSG_HOST_MANAGER_WITH_HOST_AGENT_H
