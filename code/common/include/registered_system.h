/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�registered_system.h
* �ļ���ʶ��
* ����ժҪ��GetRegisteredSystem�ӿڵ������ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�Ԭ����
* ������ڣ�2011��9��22��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/9/22
*     �� �� �ţ�V1.0
*     �� �� �ˣ�Ԭ����
*     �޸����ݣ�����
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

