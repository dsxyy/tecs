#ifndef TECS_STORAGE_MESSAGES_H
#define TECS_STORAGE_MESSAGES_H

#include "sky.h"
#include "lvm.h"
#include "sc_db.h"

namespace zte_tecs 
{

class Vginfo:public Serializable
{
public:
    Vginfo() 
    {
        _total_size=0;
        _max_size  =0;
    };
    ~Vginfo() {};
    
    SERIALIZE 
    {
        SERIALIZE_BEGIN(Vginfo);
        WRITE_VALUE(_vgname);
        WRITE_VALUE(_total_size);
        WRITE_VALUE(_max_size);
        SERIALIZE_END();    
    };
        
    DESERIALIZE 
    {
        DESERIALIZE_BEGIN(Vginfo);
        READ_VALUE(_vgname);
        READ_VALUE(_total_size);
        READ_VALUE(_max_size);            
        DESERIALIZE_END();       
            
    };
    
public:    
    string _vgname;    
    int64  _total_size;      
    int64  _max_size;            
};

class capacity 
{
public:
    capacity() 
    {
        iTotalCapacity=0;
        iAllocMaxCapacity=0;        
    };
    ~capacity() {};     
    
public:    
    int64   iTotalCapacity;  
    int64   iAllocMaxCapacity;      
};


class MessageSaInfoReport:public Serializable
{
public:
    MessageSaInfoReport() {};
    ~MessageSaInfoReport() {};
    
    SERIALIZE 
    {
        SERIALIZE_BEGIN(MessageSaInfoReport);
        WRITE_VALUE(_control_ip);
        WRITE_VALUE(_sa_ip);
        WRITE_VALUE(_type);
        WRITE_VALUE(_total_size);
        WRITE_VALUE(_max_size);    
        WRITE_VALUE(_dataip);
        WRITE_OBJECT_VECTOR(_vginfo);
        WRITE_VALUE(_description);       
        SERIALIZE_END();    
    };
    
    DESERIALIZE 
    {
        DESERIALIZE_BEGIN(MessageSaInfoReport);
        READ_VALUE(_control_ip);
        READ_VALUE(_sa_ip);
        READ_VALUE(_type);
        READ_VALUE(_total_size);
        READ_VALUE(_max_size);    
        READ_VALUE(_dataip);
        READ_OBJECT_VECTOR(_vginfo);
        READ_VALUE(_description);            
        DESERIALIZE_END();       
            
    };
    
    string _control_ip;
    string _sa_ip;
    string  _type;    
    int64 _total_size;      
    int64 _max_size;        
    vector<string>  _dataip;  
    vector<Vginfo>  _vginfo;
    string _description;    
    
};

class sa_target_info: public Serializable
{
public:   
    sa_target_info(){};
    ~sa_target_info() {};
    
    SERIALIZE 
    {
        SERIALIZE_BEGIN(sa_target_info);
        WRITE_OBJECT_ARRAY(_volume_with_lun);
        WRITE_OBJECT_ARRAY(_authorize_with_target_id);       
        SERIALIZE_END();
    };
    
    DESERIALIZE  
    {
        DESERIALIZE_BEGIN(sa_target_info);
        READ_OBJECT_ARRAY(_volume_with_lun);
        READ_OBJECT_ARRAY(_authorize_with_target_id);     
        DESERIALIZE_END();
    };

    vector<StorageVolumeWithLun> _volume_with_lun;
    vector<StorageAuthorizeWithTargetId> _authorize_with_target_id;
};

class MessageSaRegisterReq : public Serializable
{
public:
    MessageSaRegisterReq(const string& name, const string& appendinfo)
    {
        _name = name;
        _sa_append = appendinfo;
    }
    MessageSaRegisterReq(){};
    ~MessageSaRegisterReq() {};
    
    SERIALIZE 
    {
        SERIALIZE_BEGIN(MessageSaRegisterReq);
        WRITE_VALUE(_name);
        WRITE_VALUE(_sa_append);
        WRITE_VALUE(_appendinfo);
        WRITE_OBJECT(_sa_target);
        WRITE_VALUE(_sc_ifs_info);
        SERIALIZE_END();
    };
    
    DESERIALIZE  
    {
        DESERIALIZE_BEGIN(MessageSaRegisterReq);
        READ_VALUE(_name);
        READ_VALUE(_sa_append);
        READ_VALUE(_appendinfo);
        READ_OBJECT(_sa_target);
        READ_VALUE(_sc_ifs_info);
        DESERIALIZE_END();
    };

    string _name;            
    string _sa_append;   
    string _appendinfo;   
    sa_target_info _sa_target;
    map<string, string> _sc_ifs_info;
    
};

class MessageSaUnRegisterReq : public Serializable
{
public:
    MessageSaUnRegisterReq() {};
    MessageSaUnRegisterReq(const string& s)
    {
        _name = s;
    }
    ~MessageSaUnRegisterReq() {};
    
    SERIALIZE 
    {
        SERIALIZE_BEGIN(MessageSaUnRegisterReq);
        WRITE_STRING(_name);
        WRITE_STRING(_appendinfo);
        SERIALIZE_END();
    };
    
    DESERIALIZE  
    {
        DESERIALIZE_BEGIN(MessageSaUnRegisterReq);
        READ_STRING(_name);
        READ_STRING(_appendinfo);
        DESERIALIZE_END();
    };
    
    string _name;              // 要删除注册的集群名称
    string _appendinfo;       
};

class StStorageSvrAck:public WorkAck
{
public:
    StStorageSvrAck(){};
    
    ~StStorageSvrAck(){};   
  
    string  vol_uuid;
    int     last_op;
    
    StStorageSvrAck(const string &id):WorkAck(id)
    {
    };
    SERIALIZE
    {
        SERIALIZE_BEGIN(StStorageSvrAck);                   
        WRITE_VALUE(vol_uuid); 
        WRITE_VALUE(last_op);     
        SERIALIZE_PARENT_END(WorkAck);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(StStorageSvrAck);           
        READ_VALUE(vol_uuid);
        READ_VALUE(last_op);      
        DESERIALIZE_PARENT_END(WorkAck); 
    };
};


class StCreateTargetReq:public WorkReq
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    StCreateTargetReq() {};

    ~StCreateTargetReq() {};      
       
    string _targetname;
    int64  _targetindex;

    SERIALIZE
    {
        SERIALIZE_BEGIN(StCreateTargetReq);         
        WRITE_VALUE(_targetname);
        WRITE_VALUE(_targetindex);
        SERIALIZE_PARENT_END(WorkReq);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(StCreateTargetReq);
        READ_VALUE(_targetname);    
        READ_VALUE(_targetindex);    
        DESERIALIZE_PARENT_END(WorkReq);
    };
};

class StDelTargetReq:public WorkReq
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    StDelTargetReq() {};

    ~StDelTargetReq() {};    
     
    string _targetname;
    int64  _targetindex;
    int64  _sid;

    StDelTargetReq(const string & action_id):WorkReq(action_id)
    {
        ;
    }
    SERIALIZE
    {
        SERIALIZE_BEGIN(StDelTargetReq);  
        WRITE_VALUE(_targetname);
        WRITE_VALUE(_targetindex);
        WRITE_VALUE(_sid);
        SERIALIZE_PARENT_END(WorkReq);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(StDelTargetReq);         
        READ_VALUE(_targetname);    
        READ_VALUE(_targetindex);
        READ_VALUE(_sid);
        DESERIALIZE_PARENT_END(WorkReq);
    };
};

class StAllocVolReq:public WorkReq
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    StAllocVolReq() {};

    ~StAllocVolReq() {};    

    int64  _volsize; 
    string _volname;
    string _vgname;

    SERIALIZE
    {
        SERIALIZE_BEGIN(StAllocVolReq);
        WRITE_VALUE(_volsize);             
        WRITE_VALUE(_volname);
        WRITE_VALUE(_vgname);
        SERIALIZE_PARENT_END(WorkReq);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(StAllocVolReq);
        READ_VALUE(_volsize);     
        READ_VALUE(_volname);   
        READ_VALUE(_vgname);
        DESERIALIZE_PARENT_END(WorkReq);
    };
};

class StAllocSnapshotReq:public WorkReq
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    StAllocSnapshotReq() {};

    ~StAllocSnapshotReq() {};    
  

    int64  _volsize; 
    string _base_name;
    string _volname;
    string _vgname;

    SERIALIZE
    {
        SERIALIZE_BEGIN(StAllocVolReq);
        WRITE_VALUE(_volsize); 
        WRITE_VALUE(_base_name);
        WRITE_VALUE(_volname);
        WRITE_VALUE(_vgname);
        SERIALIZE_PARENT_END(WorkReq);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(StAllocVolReq);
        READ_VALUE(_volsize);   
        READ_VALUE(_base_name);
        READ_VALUE(_volname);   
        READ_VALUE(_vgname);
        DESERIALIZE_PARENT_END(WorkReq);
    };
};

class StDelVolReq:public WorkReq
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    StDelVolReq() {};

    ~StDelVolReq() {};    
  
    string _vgname;
    string _volname;

    SERIALIZE
    {
        SERIALIZE_BEGIN(StDelVolReq);    
        WRITE_VALUE(_vgname);   
        WRITE_VALUE(_volname);        
        SERIALIZE_PARENT_END(WorkReq);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(StDelVolReq);
        READ_VALUE(_vgname);  
        READ_VALUE(_volname);
        DESERIALIZE_PARENT_END(WorkReq);
    };
};

class StAddToTarReq:public WorkReq
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    StAddToTarReq() {};

    ~StAddToTarReq() {};    
  

    string  _targetname; 
    string  _vgname; 
    string  _volname;
    string  _ip;
    string  _iscsiname;
    int64   _targetindex;
    int     _lunid;
    
    StAddToTarReq(const string& action_id):WorkReq(action_id)
    {
        ;
    }
    SERIALIZE
    {
        SERIALIZE_BEGIN(StAddToTarReq);
        WRITE_VALUE(_targetname);  
        WRITE_VALUE(_vgname);  
        WRITE_VALUE(_volname);
        WRITE_VALUE(_ip);             
        WRITE_VALUE(_iscsiname);
        WRITE_VALUE(_targetindex);
        WRITE_VALUE(_lunid);
        SERIALIZE_PARENT_END(WorkReq);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(StAddToTarReq);
        READ_VALUE(_targetname);  
        READ_VALUE(_vgname);  
        READ_VALUE(_volname);  
        READ_VALUE(_ip);     
        READ_VALUE(_iscsiname); 
        READ_VALUE(_targetindex);
        READ_VALUE(_lunid);
        DESERIALIZE_PARENT_END(WorkReq);
    };
};


class StDelFromTarReq:public WorkReq
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    StDelFromTarReq() {};

    ~StDelFromTarReq() {};    
  
    string  _targetname; 
    int32   _lunid;    
    int64   _targetindex;  
    int64   _sid;      
    string  _ip;
    string  _iscsiname;
    
    StDelFromTarReq(const string& action_id):WorkReq(action_id)
    {
        
    };
    SERIALIZE
    {
        SERIALIZE_BEGIN(StAddToTarReq);
        WRITE_VALUE(_targetname);             
        WRITE_VALUE(_lunid);       
        WRITE_VALUE(_targetindex);
        WRITE_VALUE(_sid);        
        WRITE_VALUE(_ip);
        WRITE_VALUE(_iscsiname);
        SERIALIZE_PARENT_END(WorkReq);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(StAddToTarReq);
        READ_VALUE(_targetname);     
        READ_VALUE(_lunid);       
        READ_VALUE(_targetindex);
        READ_VALUE(_sid);       
        READ_VALUE(_ip);
        READ_VALUE(_iscsiname);
        DESERIALIZE_PARENT_END(WorkReq);
    };
};

class MessageSaInfoReq : public Serializable
{
public:
    MessageSaInfoReq() {};
    MessageSaInfoReq(int32 ReqMode)
    {
        _ReqMode = ReqMode;
        _StartIndex = 0;
        _Query_count = 0;
    };
    ~MessageSaInfoReq() {};
    
    SERIALIZE 
    {
        SERIALIZE_BEGIN(MessageSaInfoReq);
        WRITE_DIGIT(_ReqMode);
        WRITE_DIGIT(_StartIndex);
        WRITE_DIGIT(_Query_count);
        WRITE_STRING(_appendinfo);
        SERIALIZE_END();
    };
    
    DESERIALIZE  
    {
        DESERIALIZE_BEGIN(MessageSaInfoReq);
        READ_DIGIT(_ReqMode);
        READ_DIGIT(_StartIndex);
        READ_DIGIT(_Query_count);
        READ_STRING(_appendinfo);
        DESERIALIZE_END();
    };

    int32  _ReqMode;            // 请求模式，0表示异步请求，1表示同步请求
    int64  _StartIndex;         // 查询的起始位置
    int64  _Query_count;        // 查询数量
    string _appendinfo;         // 存放的是发送过来的消息单元，
};

class MessageSaRegisterAck : public Serializable
{
public:
    MessageSaRegisterAck() {};
    MessageSaRegisterAck(const string& s, const string& appendinfo)
    {
        _result = s;
        _appendinfo = appendinfo;
    }
    ~MessageSaRegisterAck() {};
    
    SERIALIZE 
    {
        SERIALIZE_BEGIN(MessageSaRegisterAck);
        WRITE_DIGIT(_err_code);
        WRITE_STRING(_result);
        WRITE_STRING(_appendinfo);
        SERIALIZE_END();
    };
    
    DESERIALIZE  
    {
        DESERIALIZE_BEGIN(MessageSaRegisterAck);
        READ_DIGIT(_err_code);
        READ_STRING(_result);
        READ_STRING(_appendinfo);
        DESERIALIZE_END();
    };
    
    int    _err_code;          // 错误码
    string _result;            // sa注册到sc系统的处理结果
    string _appendinfo;
};

}
#endif

