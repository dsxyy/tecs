/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�msg_host_manager_with_api_method.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��host_manager �� api_method ֮�����Ϣ��ṹ����
* ��ǰ�汾��1.0
* ��    �ߣ�Ԭ����
* ������ڣ�2011��8��18��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/8/18
*     �� �� �ţ�V1.0
*     �� �� �ˣ�Ԭ����
*     �޸����ݣ�����
*******************************************************************************/
#ifndef COMM_MSG_HOST_MANAGER_WITH_HOST_AGENT_H
#define COMM_MSG_HOST_MANAGER_WITH_HOST_AGENT_H

#include "serialization.h"
#include "message.h"
#include "vm_messages.h"

using namespace std;
namespace zte_tecs {

// cc.HostManager -> hc.HostAgent: ������ע�ᵽ��Ⱥ������
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

// cc.HostManager -> hc.HostAgent: �������Ӽ�Ⱥ��ע��������
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
    int32           _type;                   // �豸����
    int32           _number;                 //�豸����
    string          _description;            // �豸���������硰D16E1_2 subcard��
    
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

// cc.HostManager <- hc.HostAgent: �������������ϱ�
// EV_HOST_DISCOVER
// cc.HostManager <- hc.HostAgent: ��ȡ������Ϣ��Ӧ��
// EV_HOST_INFO_ACK
class MessageHostBaseInfo : public Serializable
{
public:
    string          _ip_address;            // ������IP��ַ        
    string          _media_address;         // ������ý�����ַ
    CpuInfo         _cpu_info;              // ������CPU������Ϣ
    string          _vmm_info;              // ������Hypervisor��Ϣ
    string          _os_info;               // �����Ĳ���ϵͳ��Ϣ
    int64           _running_time;          // ����������ʱ�䣬��λs
    int64           _mem_total;             // �������ڴ��ܸ��ռ䣬��λB
    int64           _mem_max;               // �������ڴ������ÿ��пռ䣬��λB
    int64           _disk_total;            // �������ش洢�ܿռ䣬��λB
    int64           _disk_max;              // �������ش洢�����ÿռ䣬��λB
    int32           _mem_state;             // �ڴ��Ƿ�Ӳ������    
    bool            _is_support_hvm;        // �ڴ��Ƿ�֧��ȫ���⻯        
    map<string,string> _location;
    string          _ssh_key;               // ssh��Կ
    string          _shelf_type;            // �������ڵĻ�������
    string          _iscsiname;
    vector<PciPthDevInfo> _sdevice_info;    //�����豸��Ϣ�������忨
    BoardPosition   _phy_position;          //����λ����Ϣ�������֣��ܣ��򣬲�
    int32           _board_type;            //����λ����Ϣ�������֣��ܣ��򣬲�
    int32           _cpus;                  // �����ڲ����������cpu��Ŀ

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

    string          _name;                  // Ӳ������, ��/dev/sda
    int32           _type;                  // Ӳ������, 1 - ATA, 2 - SCSI
    int32           _status;                // Ӳ�̽���״̬, 1 - normal, 2 - warn, 3 - fault
    
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

// ������ص�Ӳ����Ϣ
// ����������
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
    int32               _cpu_usage;         // ����CPUʹ����
    map<string,string>  _location;          // ���ص�����λ����Ϣ(�ܿ��)
    vector<DiskInfo>    _vec_disk_info;     // ����Ӳ�̵���Ϣ
    int64               _running_time;      // ����������ʱ�䣬��λs
    int64               _disk_total;        // �������ش洢�ܿռ䣬��λB
    int64               _disk_max;          // �������ش洢�����ÿռ䣬��λB    
    int32               _oscillator;        // �����Ƿ�ͣ��0������1����
    BoardPosition       _phy_position;      //����λ����Ϣ�������֣��ܣ��򣬲�
    int32               _board_type;        //����λ����Ϣ�������֣��ܣ��򣬲�
    vector<HostHwmon>   _vec_host_hwmon;    // ������ص�Ӳ����Ϣ
    ResourceStatistics  _host_statistics;   // �������Դʹ�ü����ͳ����Ϣ
    string              _ip_address;        // �������IP��ַ 
    
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
