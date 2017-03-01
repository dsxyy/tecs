/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：msg_power_agent_with_high_available.h
* 文件标识：见配置管理计划书
* 内容摘要：power_agent 与 high_available.h之间的消息体结构定义
* 当前版本：1.0
* 作    者：liuyi
* 完成日期：2013年6月18日
*
* 修改记录1：
*     修改日期：2013/6/18
*     版 本 号：V1.0
*     修 改 人：liuyi
*     修改内容：创建
*******************************************************************************/
#ifndef CC_POWER_AGENT_WITH_HIGH_AVAILABLE_H
#define CC_POWER_AGENT_WITH_HIGH_AVAILABLE_H

using namespace std;
namespace zte_tecs {

class MessageHostResetReq : public WorkReq
{
public:
    int64 _hid; 

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageHostResetReq);
        WRITE_DIGIT(_hid);
        SERIALIZE_PARENT_END(WorkReq); 
    };
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageHostResetReq);
        READ_DIGIT(_hid);
        DESERIALIZE_PARENT_END(WorkReq);
    };
};
    
class MessageHostResetAck : public WorkAck
{
public:

    int64 _hid; 

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageHostResetAck);
        WRITE_DIGIT(_hid);
        SERIALIZE_PARENT_END(WorkAck); 
    };
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageHostResetAck);
        READ_DIGIT(_hid);
        DESERIALIZE_PARENT_END(WorkAck);
    };
};
    
}  /* end namespace zte_tecs */

#endif /* end CC_HOST_MANAGER_WITH_HIGH_AVAILABLE_H */

