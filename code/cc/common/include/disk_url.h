#ifndef TECS_DISK_URL_H
#define TECS_DISK_URL_H
#include "sky.h"
#include "vm_messages.h"

//disk url请求消息
class DiskUrlReq : public Serializable
{
public:
    int64 _hid;
    int64 _vid;
    string _host_ip;
    string _iscsi_initiator;
    vector<VmDiskConfig> _disks;
    
    DiskUrlReq() 
    {
        _hid = INVALID_HID;
        _vid = INVALID_VID;
    };

    SERIALIZE
    {
        SERIALIZE_BEGIN(DiskUrlReq);
        WRITE_VALUE(_hid);
        WRITE_VALUE(_vid);
        WRITE_VALUE(_host_ip);
        WRITE_VALUE(_iscsi_initiator);
        WRITE_OBJECT_ARRAY(_disks);
        SERIALIZE_END();
    }
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(DiskUrlReq);
        READ_VALUE(_vid);
        READ_VALUE(_hid);
        READ_VALUE(_host_ip);
        READ_VALUE(_iscsi_initiator);
        READ_OBJECT_ARRAY(_disks);
        DESERIALIZE_END();
    }
};

//disk url应答消息
class DiskUrlAck:public Serializable
{
public:
    int64 _hid;
    int64 _vid;
    vector<VmDiskConfig> _disks;
    STATUS _result;
    string _error;
    
    DiskUrlAck() 
    {
        _hid = INVALID_HID;
        _vid = INVALID_VID;
        _result = ERROR;
    };

    DiskUrlAck(const DiskUrlReq& req):
    _hid(req._hid),_vid(req._vid),_disks(req._disks)
    {
        _result = ERROR;
    };
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(DiskUrlAck);
        WRITE_VALUE(_hid);
        WRITE_VALUE(_vid);
        WRITE_OBJECT_ARRAY(_disks);
        WRITE_VALUE(_result);
        WRITE_VALUE(_error);
        SERIALIZE_END();
    }
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(DiskUrlAck);
        READ_VALUE(_hid);
        READ_VALUE(_vid);
        READ_OBJECT_ARRAY(_disks);
        READ_VALUE(_result);
        READ_VALUE(_error);
        DESERIALIZE_END();
    }
};

#endif


