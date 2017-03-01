/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：host_manager_with_high_available.h
* 文件标识：见配置管理计划书
* 内容摘要：host_manager 与 high_available.h之间的消息体结构定义
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
#ifndef CC_HOST_MANAGER_WITH_HIGH_AVAILABLE_H
#define CC_HOST_MANAGER_WITH_HIGH_AVAILABLE_H

using namespace std;
namespace zte_tecs {

// cc.HostManager -> cc.HighAvailable: 主机故障或离线
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
    
// cc.HostManager -> cc.HighAvailable: 主机物理网口故障
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
    
// cc.HostManager -> cc.HighAvailable: 主机本地存储故障
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
    
// cc.HostManager -> cc.HighAvailable: 主机上运行的虚拟机故障
// EV_HOST_VM_FAULT
class MessageVmFault : public Serializable
{
public:
    string _host_name;
    int64  _host_id;
    string _vm_name;
    int64  _vm_id;
    bool   _exist; //虚拟机是否还存在于hc上
    int    _state; //虚拟机当前的状态

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

