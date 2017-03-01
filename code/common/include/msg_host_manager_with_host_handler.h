/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�msg_host_manager_with_host_handler.h
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
#ifndef COMM_MSG_HOST_MANAGER_WITH_HOST_HANDLER_H
#define COMM_MSG_HOST_MANAGER_WITH_HOST_HANDLER_H

using namespace std;
namespace zte_tecs {

// EV_HOST_RAW_HANDLE_REQ
class MessageHostRawHandleReq : public Serializable
{
public:
    string _command;
    string _append;

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageHostRawHandleReq);
        WRITE_STRING(_command);
        WRITE_STRING(_append);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageHostRawHandleReq);
        READ_STRING(_command);
        READ_STRING(_append);
        DESERIALIZE_END();
    };
};

// EV_HOST_RAW_HANDLE_ACK
class MessageHostRawHandleAck : public Serializable
{
public:
    string _command;
    string _append;
    int32  _ret_code;
    string _ret_info;

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageHostRawHandleAck);
        WRITE_STRING(_command);
        WRITE_STRING(_append);
        WRITE_DIGIT(_ret_code);
        WRITE_STRING(_ret_info);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageHostRawHandleAck);
        READ_STRING(_command);
        READ_STRING(_append);
        READ_DIGIT(_ret_code);
        READ_STRING(_ret_info);
        DESERIALIZE_END();
    };
};

// Ԥ��������
#define DEF_HANDLE_CMD_REBOOT           "reboot"        // ����
#define DEF_HANDLE_CMD_SHUTDOWN         "shutdown"      // �ػ�

// EV_HOST_DEF_HANDLE_REQ
class MessageHostDefHandleReq : public Serializable
{
public:
    string _command;
    string _argument;
    string _append;

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageHostDefHandleReq);
        WRITE_STRING(_command);
        WRITE_STRING(_argument);
        WRITE_STRING(_append);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageHostDefHandleReq);
        READ_STRING(_command);
        READ_STRING(_argument);
        READ_STRING(_append);
        DESERIALIZE_END();
    };
};

// EV_HOST_DEF_HANDLE_ACK
class MessageHostDefHandleAck : public Serializable
{
public:
    string _command;
    string _argument;
    string _append;
    int32  _ret_code;
    string _ret_info;

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageHostDefHandleAck);
        WRITE_STRING(_command);
        WRITE_STRING(_argument);        
        WRITE_STRING(_append);
        WRITE_DIGIT(_ret_code);
        WRITE_STRING(_ret_info);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageHostDefHandleAck);
        READ_STRING(_command);
        READ_STRING(_argument);
        READ_STRING(_append);
        READ_DIGIT(_ret_code);
        READ_STRING(_ret_info);
        DESERIALIZE_END();
    };
};

} //namespace zte_tecs

#endif // COMM_MSG_HOST_MANAGER_WITH_HOST_HANDLER_H

