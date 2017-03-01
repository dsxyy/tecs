#ifndef TECS_VM_DISK_URL_H
#define TECS_VM_DISK_URL_H
#include "sky.h"
#include "vm_messages.h"

enum VmDiskShareOp
{
    VDSO_CREATE = 0,
    VDSO_AUTH = 1,
    VDSO_DELETE = 2,
    VDSO_DEAUTH = 3,
    VDSO_UNKNOWN = 4
};

inline const char* VmDiskShareOpStr(int state)
{
#define ENUM2STR(value) case(value): return #value;
    switch((VmDiskShareOp)state)
    {
        ENUM2STR(VDSO_CREATE);
        ENUM2STR(VDSO_AUTH);
        ENUM2STR(VDSO_DELETE);
        ENUM2STR(VDSO_DEAUTH);
        ENUM2STR(VDSO_UNKNOWN);
        default:return "VDSO_UNKNOWN";
    }
};

enum VmDiskShareState
{
    VDSS_CREATEING = 0,
    VDSS_AUTHING = 1,
    VDSS_DEAUTHING = 2,
    VDSS_DELETEING = 3,
    VDSS_OK = 4,
    VDSS_FAILED = 5,
    VDSS_UNKNOWN = 6
};
inline const char* VmDiskShareStateStr(int state)
{
#define ENUM2STR(value) case(value): return #value;
    switch((VmDiskShareState)state)
    {
        ENUM2STR(VDSS_CREATEING);
        ENUM2STR(VDSS_AUTHING);
        ENUM2STR(VDSS_DELETEING);
        ENUM2STR(VDSS_DEAUTHING);
        ENUM2STR(VDSS_OK);
        ENUM2STR(VDSS_FAILED);
        ENUM2STR(VDSS_UNKNOWN);
        default:return "VDSS_UNKNOWN";
    }
};

//vm disk url请求消息
class VmDiskUrlReq : public Serializable
{
public:
    int64 _hid;
    int64 _vid;
    string _host_ip;
    string _iscsi_initiator;
    vector<VmDiskConfig> _disks;
    
    VmDiskUrlReq() 
    {
        _hid = INVALID_HID;
        _vid = INVALID_VID;
    };

    SERIALIZE
    {
        SERIALIZE_BEGIN(VmDiskUrlReq);
        WRITE_VALUE(_hid);
        WRITE_VALUE(_vid);
        WRITE_VALUE(_host_ip);
        WRITE_VALUE(_iscsi_initiator);
        WRITE_OBJECT_ARRAY(_disks);
        SERIALIZE_END();
    }
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VmDiskUrlReq);
        READ_VALUE(_vid);
        READ_VALUE(_hid);
        READ_VALUE(_host_ip);
        READ_VALUE(_iscsi_initiator);
        READ_OBJECT_ARRAY(_disks);
        DESERIALIZE_END();
    }
};

//vm disk url应答消息
class VmDiskUrlAck:public Serializable
{
public:
    int64 _hid;
    int64 _vid;
    vector<VmDiskConfig> _disks;
    STATUS _result;
    string _error;
    
    VmDiskUrlAck() 
    {
        _hid = INVALID_HID;
        _vid = INVALID_VID;
        _result = ERROR;
    };

    VmDiskUrlAck(const VmDiskUrlReq& req):
    _hid(req._hid),_vid(req._vid),_disks(req._disks)
    {
        _result = ERROR;
    };
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(VmDiskUrlAck);
        WRITE_VALUE(_hid);
        WRITE_VALUE(_vid);
        WRITE_OBJECT_ARRAY(_disks);
        WRITE_VALUE(_result);
        WRITE_VALUE(_error);
        SERIALIZE_END();
    }
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VmDiskUrlAck);
        READ_VALUE(_hid);
        READ_VALUE(_vid);
        READ_OBJECT_ARRAY(_disks);
        READ_VALUE(_result);
        READ_VALUE(_error);
        DESERIALIZE_END();
    }
};

#endif


