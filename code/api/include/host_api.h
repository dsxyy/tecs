/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�host_api.h
* �ļ���ʶ��
* ����ժҪ�����������ģ�����ӿ�
* ��ǰ�汾��1.0
* ��    �ߣ�Ԭ����
* ������ڣ�2011��11��24��
*
* �޸ļ�¼1��
*    �޸����ڣ�2011/11/24
*    �� �� �ţ�V1.0
*    �� �� �ˣ�Ԭ����
*    �޸����ݣ�����
*******************************************************************************/
#ifndef TECS_HOST_API_H
#define TECS_HOST_API_H

#include "rpcable.h"
#include "api_error.h"

class ApiHostInfo : public Rpcable
{
public:
    // ����������״̬, run_state
    enum RunState
    {
        INIT            = 0,    // ��ʼ״̬
        OFF_LINE        = 1,    // ����
        ON_LINE         = 2     // ����
    };

    // ������Ӳ��״̬��hardware_state
    enum HardWareState
    {
        NORMAL          = 0,    // �޹���
        ERR_MEMORY      = 1,    // �ڴ����
        ERR_OSCILLATOR  = 2,    // �������
        ERR_MEM_OSC     = 3     // �ڴ�;��񶼹���
    };

    long long       oid;                    // ���������ID
    string          host_name;              // ��������
    string          ip_address;             // ������IP��ַ    
    int             run_state;              // ����������״̬
    bool            is_disabled;            // �����Ƿ����ά��ģʽ
    time_t          create_time;            // ��������Ĵ���ʱ�� 
    int64           running_time;           // �������������ʱ�� 
    long long       cpu_info_id;            // ������CPU������Ϣ
    string          vmm_info;               // ������Hypervisor��Ϣ
    string          os_info;                // �����Ĳ���ϵͳ��Ϣ
    int             running_vms;            // ���������е����������
    int             tcu_max;                // ������tcu�������
    int             tcu_free;               // ������tcuʣ������
    long long       mem_total;              // �������ڴ��ܸ��ռ䣬��λB
    long long       mem_max;                // �������ڴ������ÿ��пռ䣬��λB
    long long       mem_free;               // �������ڴ浱ǰ���ÿ��пռ䣬��λB
    long long       disk_total;             // �������ش洢�ܿռ䣬��λB
    long long       disk_max;               // �������ش洢�����ÿռ䣬��λB
    long long       disk_free;              // �������ش洢��ǰ���ÿռ䣬��λB
    int             cpu_usage_1m;           // ���һ��ͳ�Ƶ�1��������CPUʹ����
    int             cpu_usage_5m;           // ���һ��ͳ�Ƶ�5��������CPUʹ����
    int             cpu_usage_30m;          // ���һ��ͳ�Ƶ�30��������CPUʹ����
    long long       hardware_state;         // ����Ӳ���Ƿ���ϣ�0�޹���
    string          description;            // ������������Ϣ
    string          location;                // ������λ����Ϣ
    bool            is_support_hvm;          // �����Ƿ�֧��ȫ���⻯
    int             rate_power;            // �����ļ�ʱ����, ��λW

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
    // �˿ڵ�����״̬, used
    enum Used
    {
        USED_UNKNOWN    = 0,    // δ֪
        USED_YES        = 1,    // ����
        USED_NO         = 2     // ͣ��
    };

    // �˿ڵ�Э��ģʽ, negotiate
    enum Negotiate
    {
        NEGOTIATE_AUTO  = 0,    // ��Э��
        NEGOTIATE_FORCE = 1     // ǿ��
    };

    // �˿ڵ�����ģʽ, speed
    enum Speed
    {
        SPEED_UNKNOWN   = 0,    // δ֪
        SPEED_10M       = 1,    // ʮ��
        SPEED_100M      = 2,    // ����
        SPEED_1000M     = 3,    // ǧ��
        SPEED_10G       = 4     // ����
    };

    // �˿ڵ�˫��ģʽ, duplex
    enum Duplex
    {
        DUPLEX_UNKNOWN  = 0,    // δ֪
        DUPLEX_FULL     = 1,    // ȫ˫��
        DUPLEX_HALF     = 2     // ��˫��
    };

    // �˿ڵ�����ģʽ, master
    enum Master
    {
        MASTER_UNKNOWN  = 0,    // δ֪
        MASTER_AUTO     = 1,    // ����Ӧ
        MASTER_MASTER   = 2,    // ��ģʽ
        MASTER_SLAVE    = 3     // ��ģʽ
    };

    int     used;                   // �Ƿ�����
    int     negotiate;              // Э��ģʽ
    int     speed;                  // ��������
    int     duplex;                 // ���ù���ģʽ
    int     master;                 // ����ģʽ
    int     delay_up_time;          // ��ʱUPʱ��    
    string  netplane;               // ��ʱUPʱ��    
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
    // �˿ڵ�����״̬, used
    enum Used
    {
        USED_UNKNOWN    = 0,    // δ֪
        USED_YES        = 1,    // ����
        USED_NO         = 2     // ͣ��
    };

    // �˿ڵ�Э��ģʽ, negotiate
    enum Negotiate
    {
        NEGOTIATE_AUTO  = 0,    // ��Э��
        NEGOTIATE_FORCE = 1     // ǿ��
    };

    // �˿ڵ�����ģʽ, speed/work_speed
    enum Speed
    {
        SPEED_UNKNOWN   = 0,    // δ֪
        SPEED_10M       = 1,    // ʮ��
        SPEED_100M      = 2,    // ����
        SPEED_1000M     = 3,    // ǧ��
        SPEED_10G       = 4     // ����
    };

    // �˿ڵ�˫��ģʽ, duplex/work_duplex
    enum Duplex
    {
        DUPLEX_UNKNOWN  = 0,    // δ֪
        DUPLEX_FULL     = 1,    // ȫ˫��
        DUPLEX_HALF     = 2     // ��˫��
    };

    // �˿ڵ�����ģʽ, master
    enum Master
    {
        MASTER_UNKNOWN  = 0,    // δ֪
        MASTER_AUTO     = 1,    // ����Ӧ
        MASTER_MASTER   = 2,    // ��ģʽ
        MASTER_SLAVE    = 3     // ��ģʽ
    };

    // �˿ڵ�״̬, state
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
    string  name;                   // �˿�����
    int     used;                   // �Ƿ�����
    int     negotiate;              // Э��ģʽ
    int     speed;                  // ��������
    int     duplex;                 // ���ù���ģʽ
    int     master;                 // ����ģʽ
    int     state;                  // �˿�IF״̬
    int     linked;                 // ����״̬ TODO add by xuemzh
    int     sriov_num;              // sriov��Ŀ
    int     loopback_mode;          // �Ի�ģʽ    
    string  netplane;               // ����ƽ������    
};

class ApiTrunkInfo : public Rpcable
{
public:
    // trunk�ڵ�����ģʽ, trunk_type
    enum TrunkType
    {
        TYPE_SHARE      = 1,    // ���ɷֵ�ģʽ
        TYPE_MS         = 2     // ����ģʽ
    };

    // trunk�ڵ�״̬, trunk_state
    enum TrunkState
    {
        STATE_DOWN      = 0,    // DOWN
        STATE_UP        = 1     // UP
    };
    
    string          trunk_name;     // trunk������
    int             trunk_type;     // trunk������
    int             trunk_state;    // trunk��״̬
    string          netplane;       // ����ƽ������
    vector<string>  trunk_ports;    // turnk�ڵĳ�Ա������

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
    string  config_name;        // ����������
    string  config_value;       // �������ֵ

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
    long long   cpu_info_id;    // ����CPU�ͺŵ�ID
    int         tcu_num;        // ����CPU�ͺ���֧�ֵ�TCU����
    int         commend_tcu_num; // ϵͳ�Ƽ�����CPU�ͺ���֧�ֵ�TCU����
    string      cpu_info;       // ����CPU�ͺ���ϸ��Ϣ
    string      description;    // ����CPU�ͺŵ�����
   
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

