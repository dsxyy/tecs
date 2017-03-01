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
#ifndef COMM_HOST_MANAGER_WITH_API_METHOD_H
#define COMM_HOST_MANAGER_WITH_API_METHOD_H

#include "tecs_errcode.h"

using namespace std;
namespace zte_tecs {

// tc.api -> cc.HostManager: �޸�������������Ϣ����
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

// tc.api <- cc.HostManager: �޸�������������ϢӦ��
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

// tc.api -> cc.HostManager: ��Forbidden��������ϵͳ��"����"��������
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

// tc.api <- cc.HostManager: ��Forbidden��������ϵͳ��"����"����Ӧ��
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

// tc.api -> cc.HostManager: ������������ά��ģʽ������
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

// tc.api <- cc.HostManager: ������������ά��ģʽ��Ӧ��
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

// tc.api -> cc.HostManager: ���������˳�ά��ģʽ������
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

// tc.api <- cc.HostManager: ���������˳�ά��ģʽ��Ӧ��
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

// tc.api -> cc.HostManager: ��������������������
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

// tc.api <- cc.HostManager: ����������������Ӧ��
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

// tc.api -> cc.HostManager: ���������йػ�������
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

// tc.api <- cc.HostManager: ���������йػ���Ӧ��
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

// tc.api -> cc.HostManager: ��ѯ������Ϣ������
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
    string          _host_name;             // ��������
    string          _ip_address;            // ������IP��ַ    
    int32           _run_state;             // ����������״̬
    bool            _is_disabled;           // �����Ƿ����ά��ģʽ
    time_t          _create_time;           // ��������Ĵ���ʱ�� 
    int64           _running_time;          // �������������ʱ��     
    int64           _cpu_info_id;           // ������CPU������Ϣ
    string          _vmm_info;              // ������Hypervisor��Ϣ
    string          _os_info;               // �����Ĳ���ϵͳ��Ϣ
    int32           _running_vms;           // ���������е����������
    int32           _tcu_max;               // ������tcu�������
    int32           _tcu_free;               // ������tcuʣ������
    int64           _mem_total;             // �������ڴ��ܸ��ռ䣬��λB
    int64           _mem_max;               // �������ڴ������ÿ��пռ䣬��λB
    int64           _mem_free;              // �������ڴ浱ǰ���ÿ��пռ䣬��λB
    int64           _disk_total;            // �������ش洢�ܿռ䣬��λB
    int64           _disk_max;              // �������ش洢�����ÿռ䣬��λB
    int64           _disk_free;             // �������ش洢��ǰ���ÿռ䣬��λB
    int32           _cpu_usage_1m;          // ���һ��ͳ�Ƶ�1��������CPUʹ����
    int32           _cpu_usage_5m;          // ���һ��ͳ�Ƶ�5��������CPUʹ����
    int32           _cpu_usage_30m;         // ���һ��ͳ�Ƶ�30��������CPUʹ����
    int64           _hardware_state;        // ����Ӳ���Ƿ���ϣ�0�޹���
    string          _description;           // ������������Ϣ
    string          _location;              // ����λ��
    bool            _is_support_hvm;        // �����Ƿ�֧�����⻯

    int32           _power_state;           //ͨ��cmm��ѯ���ĵ���״̬
    int32           _power_operate;         //��Ҫ���е����µ����
    int32           _rate_power;            //�����ļ�ʱ����(W), �� power_agent ��ȡ���
    time_t          _poweroff_time;         // ����������µ�ʱ��
    time_t          _total_poweroff_time;   // ����������ۼ�ʱ��
    int32           _cpus;                  // dom0�󶨵����vcpu��Ŀ

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

// tc.api <- cc.HostManager: ��ѯ������Ϣ��Ӧ��
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

