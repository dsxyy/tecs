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
#ifndef COMM_POWER_MANAGER_WITH_API_METHOD_H
#define COMM_POWER_MANAGER_WITH_API_METHOD_H

#include "tecs_errcode.h"

using namespace std;
namespace zte_tecs {

// 主机节点外部控制命令
class MessageHostExternCtrlCmdReq : public Serializable
{
public:
    enum extern_cmd{
        POWER_OFF = 1,
        POWER_ON  = 2,
        RESET     = 3
    };
    string _host_name;
    int    _cmd;      // 命令 由 枚举 extern_cmd 定义
    string _arg_info; // 可变的定义,目前不使用，后面可以容纳xml格式的参数

    string cmd_string()
    {
        switch (_cmd)
        {
            case 0:
                return "Unkown";
            case 1:
                return "Power_Off";
            case 2:
                return "Power_On";
            case 3:
                return "Reset";
        }
        ostringstream oss;
        oss << "Unkown_" << _cmd;
        return oss.str();
    }

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageHostExternCtrlCmdReq);
        WRITE_STRING(_host_name);
        WRITE_DIGIT(_cmd);
        WRITE_STRING(_arg_info);
        SERIALIZE_END(); 
    };
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageHostExternCtrlCmdReq);
        READ_STRING(_host_name);
        READ_DIGIT(_cmd);
        READ_STRING(_arg_info);
        DESERIALIZE_END();
    };
};

class MessageHostExternCtrlCmdAck : public Serializable
{
public:

    string  _host_name;         // 主机名
    int     _cmd;               // 命令, 与MessageHostExternCtrlReq中的定义相同
    int     _ret_code;          // 执行结果

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageHostExternCtrlCmdAck);
        WRITE_STRING(_host_name);
        WRITE_DIGIT(_cmd);
        WRITE_DIGIT(_ret_code);
        SERIALIZE_END(); 
    };
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageHostExternCtrlCmdAck);
        READ_STRING(_host_name);
        READ_DIGIT(_cmd);
        READ_DIGIT(_ret_code);
        DESERIALIZE_END();
    };
};

// 查询单板的状态(ATCA标准的Mx)命令
class MessageHostExternCtrlQueryReq : public Serializable
{
public:

    string  _host_name;  
    string  _arg_info;

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageHostExternCtrlQueryReq);
        WRITE_STRING(_host_name);
        WRITE_STRING(_arg_info);
        
        SERIALIZE_END(); 
    };
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageHostExternCtrlQueryReq);
        READ_STRING(_host_name);
        READ_STRING(_arg_info);
        DESERIALIZE_END();
    };
};

class MessageHostExternCtrlQueryAck : public Serializable
{
public:   
    enum Cmm_Host_State
    {
        M0_NotInstall=0,             // 未安装
        M1_Inactive=1,               // 不活动, 就是没有加电
        M2_ActivationRequest = 2,    // 激活请求, 已经收到上电请求了
        M3_ActivationInProcess = 3,  // 激活中
        M4_Active = 4,               // 活动， 这是正常的工作状态
        M5_DeactivationRequest = 5,  // 去激活请求，关闭电源
        M6_DeactivationInProcess = 6,// 去激活中
        M7_CommunicationLost = 7,    // 通信丢失，一般是单板被拔出或
        State_Unknown = 8            // 这是异常情况，例如：CMM不在位或IP地址错误等问题导致的状态无法查询
    };
    string  _host_name;         // 主机名
    int     _ret_code;          // 执行结果
    int     _state;             // 状态 由 Cmm_Host_State 定义

    string state_string()
    {
        switch (_state)
        {
            case 0:
                return "M0_NotInstall";
            case 1:
                return "M1_Inactive";
            case 2:
                return "M2_ActivationRequest";
            case 3:
                return "M3_ActivationInProcess";
            case 4:
                return "M4_Active";
            case 5:
                return "M5_DeactivationRequest";
            case 6:
                return "M6_DeactivationInProcess";
            case 7:
                return "M7_CommunicationLost";
            case 8:
                return "Unknown";
             
        }
        ostringstream oss;
        oss << "Unkown_" << _state;
        return oss.str();
    }

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageHostExternCtrlQueryAck);
        WRITE_STRING(_host_name);
        WRITE_DIGIT(_ret_code);
        WRITE_DIGIT(_state);
        SERIALIZE_END(); 
    };
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageHostExternCtrlQueryAck);
        READ_STRING(_host_name);
        READ_DIGIT(_ret_code);
        READ_DIGIT(_state);
        DESERIALIZE_END();
    };
};

// 配置
class MessageHostExternCtrlCfgSetReq : public Serializable
{
public:
   
    string  _host_name;  
    string _node_type;
    string _node_manager;
    string _node_address;

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageHostExternCtrlQueryReq);
        WRITE_STRING(_host_name);
        WRITE_STRING(_node_type);
        WRITE_STRING(_node_manager);
        WRITE_STRING(_node_address);        
        SERIALIZE_END(); 
    };
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageHostExternCtrlQueryReq);
        READ_STRING(_host_name);
        READ_STRING(_node_type);
        READ_STRING(_node_manager);
        READ_STRING(_node_address);
        DESERIALIZE_END();
    };
};

class MessageHostExternCtrlCfgSetAck : public Serializable
{
public:   
    string  _host_name;         // 主机名
    int     _ret_code;          // 执行结果

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageHostExternCtrlQueryAck);
        WRITE_STRING(_host_name);
        WRITE_DIGIT(_ret_code);
        SERIALIZE_END(); 
    };
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageHostExternCtrlQueryAck);
        READ_STRING(_host_name);
        READ_DIGIT(_ret_code);
        DESERIALIZE_END();
    };
};

class MessageHostExternCtrlCfgGetReq : public Serializable
{
public:
   
    string  _host_name;  

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageHostExternCtrlQueryReq);
        WRITE_STRING(_host_name);
        SERIALIZE_END(); 
    };
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageHostExternCtrlQueryReq);
        READ_STRING(_host_name);
        DESERIALIZE_END();
    };
};

class MessageHostExternCtrlCfgGetAck : public Serializable
{
public:   
    string  _host_name;         // 主机名
    int     _ret_code;          // 执行结果
    string _node_type;
    string _node_manager;
    string _node_address;

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageHostExternCtrlQueryAck);
        WRITE_STRING(_host_name);
        WRITE_DIGIT(_ret_code);
        WRITE_STRING(_node_type);
        WRITE_STRING(_node_manager);
        WRITE_STRING(_node_address);        
        SERIALIZE_END(); 
    };
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageHostExternCtrlQueryAck);
        READ_STRING(_host_name);
        READ_DIGIT(_ret_code);
        READ_STRING(_node_type);
        READ_STRING(_node_manager);
        READ_STRING(_node_address);        
        DESERIALIZE_END();
    };
};

class MessageFanRPMCtrlCmdReq : public Serializable
{
public:
    string _cluster_name;
    int    _cmm_bureau;
    int    _cmm_rack;
    int    _cmm_shelf;
    int    _fantray_index; // ATCA 14U 机框有 2 个风扇
    int    _rpm_level; // 风扇转速等级, 1-13, 13 为最快

    MessageFanRPMCtrlCmdReq()
    {
        _cmm_bureau = 0;
        _cmm_rack = 0;
        _cmm_shelf = 0;
        _fantray_index = 0;
        _rpm_level = 0;
    };

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageFanRPMCtrlCmdReq);
        WRITE_VALUE(_cluster_name);
        WRITE_VALUE(_cmm_bureau);
        WRITE_VALUE(_cmm_rack);
        WRITE_VALUE(_cmm_shelf);
        WRITE_VALUE(_fantray_index);
        WRITE_VALUE(_rpm_level);
        SERIALIZE_END(); 
    };
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageFanRPMCtrlCmdReq);
        READ_VALUE(_cluster_name);
        READ_VALUE(_cmm_bureau);
        READ_VALUE(_cmm_rack);
        READ_VALUE(_cmm_shelf);
        READ_VALUE(_fantray_index);
        READ_VALUE(_rpm_level);
        DESERIALIZE_END();
    };
};

class MessageFanRPMCtrlCmdAck : public Serializable
{
public:

    int     _ret_code;          // 执行结果

    MessageFanRPMCtrlCmdAck()
    {
        _ret_code = 0;
    };

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageFanRPMCtrlCmdAck);
        WRITE_VALUE(_ret_code);
        SERIALIZE_END(); 
    };
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageFanRPMCtrlCmdAck);
        READ_VALUE(_ret_code);
        DESERIALIZE_END();
    };
};

}  /* end namespace zte_tecs */

#endif /* end COMM_HOST_MANAGER_WITH_API_METHOD_H */

