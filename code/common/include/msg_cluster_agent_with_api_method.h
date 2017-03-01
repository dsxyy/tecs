/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：msg_cluster_agent_with_api_method.h
* 文件标识：见配置管理计划书
* 内容摘要：cluster_agent 与 api_method 之间的消息体结构定义
* 当前版本：1.0
* 作    者：liuyi
* 完成日期：2012年12月18日
*
* 修改记录1：
*     修改日期：2012/12/18
*     版 本 号：V1.0
*     修 改 人：liuyi
*     修改内容：创建
*******************************************************************************/
#ifndef COMM_CLUSTER_AGENT_WITH_API_METHOD_H
#define COMM_CLUSTER_AGENT_WITH_API_METHOD_H

#include "tecs_errcode.h"

using namespace std;
namespace zte_tecs {

// tc.api -> cc.HostManager: 修改主机的描述信息请求
// EV_HOST_SET_REQ
// tc.ClusterManager -> cc.HostManager: CC收到TC对集群参数进行配置的请求
// EV_CLUSTER_CONFIG_REQ           (TECS_HOST_MANAGER_BEGIN + 23)
class MessageClusterCfgReq : public Serializable
{
public:
    string _command;   
    string _config_name;
    string _config_value;
   
    SERIALIZE 
    {
        SERIALIZE_BEGIN(MessageClusterCfgReq);
        WRITE_STRING(_command);
        WRITE_STRING(_config_name);
        WRITE_STRING(_config_value);
        SERIALIZE_END();
    };
    
    DESERIALIZE  
    {
        DESERIALIZE_BEGIN(MessageClusterCfgReq);
        READ_STRING(_command);
        READ_STRING(_config_name);
        READ_STRING(_config_value);
        DESERIALIZE_END();
    };
};

// tc.ClusterManager -> cc.HostManager: CC收到TC对集群参数进行配置的应答
// EV_CLUSTER_CONFIG_ACK
class MessageClusterCfgAck : public Serializable
{
public:
    int32  _ret_code;

    SERIALIZE 
    {
        SERIALIZE_BEGIN(MessageClusterCfgAck);
        WRITE_DIGIT(_ret_code);
        SERIALIZE_END();
    };
    
    DESERIALIZE  
    {
        DESERIALIZE_BEGIN(MessageClusterCfgAck);
        READ_DIGIT(_ret_code);
        DESERIALIZE_END();
    };
};

class MessageClusterCfgQueryReq : public Serializable
{
public:
    string _config_name;
   
    SERIALIZE 
    {
        SERIALIZE_BEGIN(MessageClusterCfgQueryReq);
        WRITE_STRING(_config_name);
        SERIALIZE_END();
    };
    
    DESERIALIZE  
    {
        DESERIALIZE_BEGIN(MessageClusterCfgQueryReq);
        READ_STRING(_config_name);
        DESERIALIZE_END();
    };
};

class MessageClusterCfgQueryAck : public Serializable
{
public:
    int32               _ret_code;
    map<string, string> _config_infos;

    SERIALIZE 
    {
        SERIALIZE_BEGIN(MessageClusterCfgQueryAck);
        WRITE_DIGIT(_ret_code);
        WRITE_STRING_MAP(_config_infos);
        SERIALIZE_END();
    };
    
    DESERIALIZE  
    {
        DESERIALIZE_BEGIN(MessageClusterCfgQueryAck);
        READ_DIGIT(_ret_code);
        READ_STRING_MAP(_config_infos);
        DESERIALIZE_END();
    };
};

// tc.ClusterManager -> cc.HostManager: CC收到TC对集群TCU进行配置的请求
// EV_CLUSTER_TCU_CONFIG_REQ
class MessageClusterTcuCfgReq : public Serializable
{
public:
    string  _command;
    int64   _cpu_info_id;
    int32   _tcu_num;
    string  _description;
    
    SERIALIZE 
    {
        SERIALIZE_BEGIN(MessageClusterTcuCfgReq);
        WRITE_STRING(_command);
        WRITE_DIGIT(_cpu_info_id);
        WRITE_DIGIT(_tcu_num);
        WRITE_STRING(_description);
        SERIALIZE_END();
    };
    
    DESERIALIZE  
    {
        DESERIALIZE_BEGIN(MessageClusterTcuCfgReq);
        READ_STRING(_command);
        READ_DIGIT(_cpu_info_id);
        READ_DIGIT(_tcu_num);
        READ_STRING(_description);
        DESERIALIZE_END();
    };   
};

// tc.ClusterManager -> cc.HostManager: CC收到TC对集群TCU进行配置的应答
// EV_CLUSTER_TCU_CONFIG_ACK
class MessageClusterTcuCfgAck : public Serializable
{
public:
    int32   _ret_code;
    
    SERIALIZE 
    {
        SERIALIZE_BEGIN(MessageClusterTcuCfgAck);
        WRITE_DIGIT(_ret_code);
        SERIALIZE_END();
    };
    
    DESERIALIZE  
    {
        DESERIALIZE_BEGIN(MessageClusterTcuCfgAck);
        READ_DIGIT(_ret_code);
        DESERIALIZE_END();
    };
};

class MessageClusterTcuQueryReq : public Serializable
{
public:
    int64   _start_index;
    uint64  _query_count;
    string  _cpu_info;
    int64   _cpu_info_id;
    
    SERIALIZE 
    {
        SERIALIZE_BEGIN(MessageClusterTcuQueryReq);
        WRITE_DIGIT(_start_index);
        WRITE_DIGIT(_query_count);
        WRITE_STRING(_cpu_info);        
        WRITE_DIGIT(_cpu_info_id);
        SERIALIZE_END();
    };
    
    DESERIALIZE  
    {
        DESERIALIZE_BEGIN(MessageClusterTcuQueryReq);
        READ_DIGIT(_start_index);
        READ_DIGIT(_query_count);
        READ_STRING(_cpu_info);        
        READ_DIGIT(_cpu_info_id);
        DESERIALIZE_END();
    };   
};

class TcuInfoType : public Serializable
{
public: 
    int64  _cpu_info_id;
    int32  _tcu_num;
    int32  _commend_tcu_num;
    string _cpu_info;
    string _description;

    
    TcuInfoType() {_tcu_num = 0;}
    SERIALIZE 
    {
        SERIALIZE_BEGIN(TcuInfoType);
        WRITE_DIGIT(_cpu_info_id);
        WRITE_DIGIT(_tcu_num);
        WRITE_DIGIT(_commend_tcu_num);
        WRITE_STRING(_cpu_info);
        WRITE_STRING(_description);
        SERIALIZE_END();
    };
    
    DESERIALIZE  
    {
        DESERIALIZE_BEGIN(TcuInfoType);
        READ_DIGIT(_cpu_info_id);
        READ_DIGIT(_tcu_num);
        READ_DIGIT(_commend_tcu_num);
        READ_STRING(_cpu_info);
        READ_STRING(_description);
        DESERIALIZE_END();
    };
};

class MessageClusterTcuQueryAck : public Serializable
{
public:
    int32                   _ret_code;
    int64                   _next_index;
    int64                   _max_count;
    vector<TcuInfoType>   _tcu_infos;    
    
    SERIALIZE 
    {
        SERIALIZE_BEGIN(MessageClusterTcuQueryAck);
        WRITE_DIGIT(_ret_code);
        WRITE_DIGIT(_next_index);
        WRITE_DIGIT(_max_count);
        WRITE_OBJECT_VECTOR(_tcu_infos);
        SERIALIZE_END();
    };
    
    DESERIALIZE  
    {
        DESERIALIZE_BEGIN(MessageClusterTcuQueryAck);
        READ_DIGIT(_ret_code);
        READ_DIGIT(_next_index);
        READ_DIGIT(_max_count);
        READ_OBJECT_VECTOR(_tcu_infos);
        DESERIALIZE_END();
    };
};


class CmmConfig : public Serializable
{
public: 
    int32  _bureau;
    int32  _rack;
    int32  _shelf;
    string _type;
    string  _ip_address;
    string _description;

    
    CmmConfig() 
    {_bureau = 0;_rack=0;_shelf=0;}
    SERIALIZE 
    {
        SERIALIZE_BEGIN(CmmConfig);
        WRITE_DIGIT(_bureau);
        WRITE_DIGIT(_rack);
        WRITE_DIGIT(_shelf);
        WRITE_STRING(_type);
        WRITE_STRING(_ip_address);
        WRITE_STRING(_description);
        SERIALIZE_END();
    };
    
    DESERIALIZE  
    {
        DESERIALIZE_BEGIN(CmmConfig);
        READ_DIGIT(_bureau);
        READ_DIGIT(_rack);
        READ_DIGIT(_shelf);
        READ_STRING(_type);
        READ_STRING(_ip_address);
        READ_STRING(_description);
        DESERIALIZE_END();
    };
};

class MessageCmmConfigReq : public Serializable
{
public:
    int32  _bureau;
    int32  _rack;
    int32  _shelf;
    string _type;
    string  _ip_address;
    string _description;
   
    SERIALIZE 
    {
        SERIALIZE_BEGIN(MessageCmmConfigReq);
        WRITE_DIGIT(_bureau);
        WRITE_DIGIT(_rack);
        WRITE_DIGIT(_shelf);
        WRITE_STRING(_type);
        WRITE_STRING(_ip_address);
        WRITE_STRING(_description);
        SERIALIZE_END();
    };
    
    DESERIALIZE  
    {
        DESERIALIZE_BEGIN(MessageCmmConfigReq);
        READ_DIGIT(_bureau);
        READ_DIGIT(_rack);
        READ_DIGIT(_shelf);
        READ_STRING(_type);
        READ_STRING(_ip_address);
        READ_STRING(_description);
        DESERIALIZE_END();
    };
};
class MessageCmmConfigCfgAck : public Serializable
{
public:
    int32   _ret_code;
    
    SERIALIZE 
    {
        SERIALIZE_BEGIN(MessageCmmConfigCfgAck);
        WRITE_DIGIT(_ret_code);
        SERIALIZE_END();
    };
    
    DESERIALIZE  
    {
        DESERIALIZE_BEGIN(MessageCmmConfigCfgAck);
        READ_DIGIT(_ret_code);
        DESERIALIZE_END();
    };
};

class MessageCmmQueryReq : public Serializable
{
public:
    int64   _start_index;
    uint64  _query_count;
    
    SERIALIZE 
    {
        SERIALIZE_BEGIN(MessageClusterTcuQueryReq);
        WRITE_DIGIT(_start_index);
        WRITE_DIGIT(_query_count);
        SERIALIZE_END();
    };
    
    DESERIALIZE  
    {
        DESERIALIZE_BEGIN(MessageClusterTcuQueryReq);
        READ_DIGIT(_start_index);
        READ_DIGIT(_query_count);
        DESERIALIZE_END();
    };   
};


class MessageCmmQueryAck : public Serializable
{
public:
    int32                   _ret_code;
    int64                   _next_index;
    int64                   _max_count;
    vector<CmmConfig> _cmm_infos;
    
    SERIALIZE 
    {
        SERIALIZE_BEGIN(MessageClusterTcuQueryReq);
        WRITE_DIGIT(_ret_code);
        WRITE_DIGIT(_next_index);
        WRITE_DIGIT(_max_count);
        WRITE_OBJECT_VECTOR(_cmm_infos);
        SERIALIZE_END();
    };
    
    DESERIALIZE  
    {
        DESERIALIZE_BEGIN(MessageClusterTcuQueryReq);
        READ_DIGIT(_ret_code);
        READ_DIGIT(_next_index);
        READ_DIGIT(_max_count);
        READ_OBJECT_VECTOR(_cmm_infos);
        DESERIALIZE_END();
    };   
};
class MessageCmmDeleteReq : public Serializable
{
public:
    int32  _bureau;
    int32  _rack;
    int32  _shelf;
   
    SERIALIZE 
    {
        SERIALIZE_BEGIN(MessageCmmDeleteReq);
        WRITE_DIGIT(_bureau);
        WRITE_DIGIT(_rack);
        WRITE_DIGIT(_shelf);
        SERIALIZE_END();
    };
    
    DESERIALIZE  
    {
        DESERIALIZE_BEGIN(MessageCmmDeleteReq);
        READ_DIGIT(_bureau);
        READ_DIGIT(_rack);
        READ_DIGIT(_shelf);
        DESERIALIZE_END();
    };
};
class MessageCmmDeleteAck : public Serializable
{
public:
    int32   _ret_code;
    
    SERIALIZE 
    {
        SERIALIZE_BEGIN(MessageCmmDeleteAck);
        WRITE_DIGIT(_ret_code);
        SERIALIZE_END();
    };
    
    DESERIALIZE  
    {
        DESERIALIZE_BEGIN(MessageCmmDeleteAck);
        READ_DIGIT(_ret_code);
        DESERIALIZE_END();
    };
};

class HostAtcaConfig : public Serializable
{
public: 
    int64  _hid;
    int32  _bureau;
    int32  _rack;
    int32  _shelf;
    int32  _slot;
    string _board_type;

    
    HostAtcaConfig() 
    {_hid = 0;_bureau = 0;_rack=0;_shelf=0;_slot=0;}
    SERIALIZE 
    {
        SERIALIZE_BEGIN(HostAtcaConfig);
        WRITE_DIGIT(_hid);
        WRITE_DIGIT(_bureau);
        WRITE_DIGIT(_rack);
        WRITE_DIGIT(_shelf);
        WRITE_DIGIT(_slot);
        WRITE_STRING(_board_type);
        SERIALIZE_END();
    };
    
    DESERIALIZE  
    {
        DESERIALIZE_BEGIN(HostAtcaConfig);
        READ_DIGIT(_hid);
        READ_DIGIT(_bureau);
        READ_DIGIT(_rack);
        READ_DIGIT(_shelf);
        READ_DIGIT(_slot);
        READ_STRING(_board_type);
        DESERIALIZE_END();
    };
};

class MessageClusterSaveEnergyParaCfgReq : public Serializable
{
public:
    string  _command; 
    bool    _save_energy_enable;   
    int32   _max_host_num;
    int32   _min_host_num;
    int32   _host_percent;
    int32   _save_energy_interval;

    STATUS Validate()
    {
        int32 retcode = SUCCESS;

        if(_min_host_num >  _max_host_num)
        {
            retcode = ERR_CLUSTER_CFG_PARA_ERR;
        }

        return retcode;
    }

    
    SERIALIZE 
    {
        SERIALIZE_BEGIN(MessageClusterSaveEnergyParaCfgReq);
        WRITE_STRING(_command);   
        WRITE_DIGIT(_save_energy_enable);        
        WRITE_DIGIT(_max_host_num);
        WRITE_DIGIT(_min_host_num);
        WRITE_DIGIT(_host_percent);
        WRITE_VALUE(_save_energy_interval);
        SERIALIZE_END();
    };
    
    DESERIALIZE  
    {
        DESERIALIZE_BEGIN(MessageClusterSaveEnergyParaCfgReq);
        READ_STRING(_command);  
        READ_DIGIT(_save_energy_enable);  
        READ_DIGIT(_max_host_num);
        READ_DIGIT(_min_host_num);
        READ_DIGIT(_host_percent);
        READ_VALUE(_save_energy_interval);
        DESERIALIZE_END();
    };   
};

class MessageClusterSaveEnergyParaQueryReq : public Serializable
{
public:
    string _query_type;
   
    SERIALIZE 
    {
        SERIALIZE_BEGIN(MessageClusterSaveEnergyParaQueryReq);
        WRITE_STRING(_query_type);
        SERIALIZE_END();
    };
    
    DESERIALIZE  
    {
        DESERIALIZE_BEGIN(MessageClusterSaveEnergyParaQueryReq);
        READ_STRING(_query_type);
        DESERIALIZE_END();
    };
};

class MessageClusterSaveEnergyParaQueryAck : public Serializable
{
public:
    bool    _save_energy_enable;   
    int32   _max_host_num;
    int32   _min_host_num;
    int32   _host_percent;
    int32   _save_energy_interval;
    int32   _ret_code;
    
   SERIALIZE 
   {
       SERIALIZE_BEGIN(MessageClusterSaveEnergyParaQueryAck);
       WRITE_DIGIT(_save_energy_enable);        
       WRITE_DIGIT(_max_host_num);
       WRITE_DIGIT(_min_host_num);
       WRITE_DIGIT(_host_percent);
       WRITE_VALUE(_save_energy_interval);
       WRITE_DIGIT(_ret_code);
       SERIALIZE_END();
   };
   
   DESERIALIZE  
   {
       DESERIALIZE_BEGIN(MessageClusterSaveEnergyParaQueryAck);
       READ_DIGIT(_save_energy_enable);  
       READ_DIGIT(_max_host_num);
       READ_DIGIT(_min_host_num);
       READ_DIGIT(_host_percent);
       READ_VALUE(_save_energy_interval);
       READ_DIGIT(_ret_code);       
       DESERIALIZE_END();
   };      
};

class MessageSchedulerPolicySetReq : public Serializable
{
public:
    string      _policy;            //调度策略
    int32       _cycle_time;        //运营调度的周期
    int32       _influence_vms;     //单次运营调度影响的虚拟机最大数量
    
   SERIALIZE 
   {
       SERIALIZE_BEGIN(MessageSchedulerPolicySetReq);
       WRITE_STRING(_policy);        
       WRITE_DIGIT(_cycle_time);
       WRITE_DIGIT(_influence_vms);
       SERIALIZE_END();
   };
   
   DESERIALIZE  
   {
       DESERIALIZE_BEGIN(MessageSchedulerPolicySetReq);
       READ_STRING(_policy);  
       READ_DIGIT(_cycle_time);
       READ_DIGIT(_influence_vms);   
       DESERIALIZE_END();
   };      
};

class MessageSchedulerPolicyQueryAck : public Serializable
{
public:
    int32       _ret_code;
    string      _policy;            //调度策略
    int32       _cycle_time;        //运营调度的周期
    int32       _influence_vms;     //单次运营调度影响的虚拟机最大数量

   SERIALIZE 
   {
        SERIALIZE_BEGIN(MessageSchedulerPolicyQueryAck);
        WRITE_DIGIT(_ret_code);
        WRITE_STRING(_policy);        
        WRITE_DIGIT(_cycle_time);
        WRITE_DIGIT(_influence_vms);
        SERIALIZE_END();
   };
   
   DESERIALIZE  
   {
        DESERIALIZE_BEGIN(MessageSchedulerPolicyQueryAck);
        READ_DIGIT(_ret_code);
        READ_STRING(_policy);  
        READ_DIGIT(_cycle_time);
        READ_DIGIT(_influence_vms);   
        DESERIALIZE_END();
   };      
};

class MessageVMHAPolicyQueryAck : public Serializable
{
public:
    int32       _ret_code;
    string      vm_crash;
    string      host_down;
    string      host_fault;
    string      host_eth_down;
    string      host_storage_fault;    
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(VmHAPolicy);
        WRITE_DIGIT(_ret_code);
        WRITE_STRING(vm_crash);
        WRITE_STRING(host_down);
        WRITE_STRING(host_fault);
        WRITE_STRING(host_eth_down);
        WRITE_STRING(host_storage_fault);
        SERIALIZE_END();
    }

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VmHAPolicy);
        READ_DIGIT(_ret_code);
        READ_STRING(vm_crash);
        READ_STRING(host_down);
        READ_STRING(host_fault);
        READ_STRING(host_eth_down);
        READ_STRING(host_storage_fault);
        DESERIALIZE_END();
    }
};

}  /* end namespace zte_tecs */

#endif /* end COMM_HOST_MANAGER_WITH_API_METHOD_H */

