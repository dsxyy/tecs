/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�msg_power_agent_with_high_available.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��power_agent �� high_available.h֮�����Ϣ��ṹ����
* ��ǰ�汾��1.0
* ��    �ߣ�liuyi
* ������ڣ�2013��6��18��
*
* �޸ļ�¼1��
*     �޸����ڣ�2013/6/18
*     �� �� �ţ�V1.0
*     �� �� �ˣ�liuyi
*     �޸����ݣ�����
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

