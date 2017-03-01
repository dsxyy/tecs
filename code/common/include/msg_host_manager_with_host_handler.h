/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：msg_host_manager_with_host_handler.h
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

// 预定义命令
#define DEF_HANDLE_CMD_REBOOT           "reboot"        // 重启
#define DEF_HANDLE_CMD_SHUTDOWN         "shutdown"      // 关机

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

