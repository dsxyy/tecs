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
#ifndef COMM_POWER_MANAGER_WITH_API_METHOD_H
#define COMM_POWER_MANAGER_WITH_API_METHOD_H

#include "tecs_errcode.h"

using namespace std;
namespace zte_tecs {

// �����ڵ��ⲿ��������
class MessageHostExternCtrlCmdReq : public Serializable
{
public:
    enum extern_cmd{
        POWER_OFF = 1,
        POWER_ON  = 2,
        RESET     = 3
    };
    string _host_name;
    int    _cmd;      // ���� �� ö�� extern_cmd ����
    string _arg_info; // �ɱ�Ķ���,Ŀǰ��ʹ�ã������������xml��ʽ�Ĳ���

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

    string  _host_name;         // ������
    int     _cmd;               // ����, ��MessageHostExternCtrlReq�еĶ�����ͬ
    int     _ret_code;          // ִ�н��

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

// ��ѯ�����״̬(ATCA��׼��Mx)����
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
        M0_NotInstall=0,             // δ��װ
        M1_Inactive=1,               // ���, ����û�мӵ�
        M2_ActivationRequest = 2,    // ��������, �Ѿ��յ��ϵ�������
        M3_ActivationInProcess = 3,  // ������
        M4_Active = 4,               // ��� ���������Ĺ���״̬
        M5_DeactivationRequest = 5,  // ȥ�������󣬹رյ�Դ
        M6_DeactivationInProcess = 6,// ȥ������
        M7_CommunicationLost = 7,    // ͨ�Ŷ�ʧ��һ���ǵ��屻�γ���
        State_Unknown = 8            // �����쳣��������磺CMM����λ��IP��ַ��������⵼�µ�״̬�޷���ѯ
    };
    string  _host_name;         // ������
    int     _ret_code;          // ִ�н��
    int     _state;             // ״̬ �� Cmm_Host_State ����

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

// ����
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
    string  _host_name;         // ������
    int     _ret_code;          // ִ�н��

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
    string  _host_name;         // ������
    int     _ret_code;          // ִ�н��
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
    int    _fantray_index; // ATCA 14U ������ 2 ������
    int    _rpm_level; // ����ת�ٵȼ�, 1-13, 13 Ϊ���

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

    int     _ret_code;          // ִ�н��

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

