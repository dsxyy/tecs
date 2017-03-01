/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：registered_system.h
* 文件标识：
* 内容摘要：GetRegisteredSystem接口的申明文件
* 当前版本：1.0
* 作    者：袁进坤
* 完成日期：2011年9月22日
*
* 修改记录1：
*     修改日期：2011/9/22
*     版 本 号：V1.0
*     修 改 人：袁进坤
*     修改内容：创建
*******************************************************************************/
#ifndef HC_COMMON_REGISTERED_SYSTEM_H
#define HC_COMMON_REGISTERED_SYSTEM_H

#include "sky.h"

class MessageRegisteredSystem : public Serializable
{
public:
    MessageRegisteredSystem(const string &system_name)
        { _system_name = system_name; }
    MessageRegisteredSystem() {}
    ~MessageRegisteredSystem() {}

    string  _system_name;

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageRegisteredSystem);
        WRITE_VALUE(_system_name);
        SERIALIZE_END();
    };
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageRegisteredSystem);
        READ_VALUE(_system_name);
        DESERIALIZE_END();
    };
};

extern STATUS GetRegisteredSystem(string &reg_system);
extern void SetRegisteredSystem(const string &reg_system);
extern void WaitRegisteredSystem();

#endif /* end HC_COMMON_REGISTERED_SYSTEM_H */

