/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�host_manager_with_high_available.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��host_manager �� high_available.h֮�����Ϣ��ṹ����
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
#ifndef CC_HOST_MANAGER_WITH_HIGH_AVAILABLE_H
#define CC_HOST_MANAGER_WITH_HIGH_AVAILABLE_H

using namespace std;
namespace zte_tecs {

// cc.HostManager -> cc.HighAvailable: �������ϻ�����
// EV_HOST_FAULT
class MessageHostFault : public Serializable
{
public:
    string _host_name;
    int64  _host_id;

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageHostFault);
        WRITE_STRING(_host_name);
        WRITE_DIGIT(_host_id);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageHostFault);
        READ_STRING(_host_name);
        READ_DIGIT(_host_id);
        DESERIALIZE_END();
    };
};
    
// cc.HostManager -> cc.HighAvailable: �����������ڹ���
// EV_HOST_ENET_ERROR
class MessageHostEnetError : public Serializable
{
public:
    string _host_name;
    int64  _host_id;
    string _port_name;

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageHostEnetError);
        WRITE_STRING(_host_name);
        WRITE_DIGIT(_host_id);
        WRITE_STRING(_port_name);        
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageHostEnetError);
        READ_STRING(_host_name);
        READ_DIGIT(_host_id);
        READ_STRING(_port_name);        
        DESERIALIZE_END();
    };
};
    
// cc.HostManager -> cc.HighAvailable: �������ش洢����
// EV_HOST_STORAGE_ERROR
class MessageHostStorageFault : public Serializable
{
public:
    string _host_name;
    int64  _host_id;

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageHostStorageFault);
        WRITE_STRING(_host_name);
        WRITE_DIGIT(_host_id);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageHostStorageFault);
        READ_STRING(_host_name);
        READ_DIGIT(_host_id);
        DESERIALIZE_END();
    };
};
    
// cc.HostManager -> cc.HighAvailable: ���������е����������
// EV_HOST_VM_FAULT
class MessageVmFault : public Serializable
{
public:
    string _host_name;
    int64  _host_id;
    string _vm_name;
    int64  _vm_id;
    bool   _exist; //������Ƿ񻹴�����hc��
    int    _state; //�������ǰ��״̬

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageVmFault);
        WRITE_STRING(_host_name);
        WRITE_DIGIT(_host_id);
        WRITE_STRING(_vm_name);
        WRITE_DIGIT(_vm_id);
        WRITE_DIGIT(_exist);
        WRITE_DIGIT(_state);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageVmFault);
        READ_STRING(_host_name);
        READ_DIGIT(_host_id);
        READ_STRING(_vm_name);
        READ_DIGIT(_vm_id);        
        READ_DIGIT(_exist);
        READ_DIGIT(_state);
        DESERIALIZE_END();
    };
};

class MessageHostCpuBusy : public Serializable
{
public:
    string _host_name;
    int64  _host_id;

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageHostCpuBusy);
        WRITE_STRING(_host_name);
        WRITE_DIGIT(_host_id);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageHostCpuBusy);
        READ_STRING(_host_name);
        READ_DIGIT(_host_id);
        DESERIALIZE_END();
    };
};
    
}  /* end namespace zte_tecs */

#endif /* end CC_HOST_MANAGER_WITH_HIGH_AVAILABLE_H */

