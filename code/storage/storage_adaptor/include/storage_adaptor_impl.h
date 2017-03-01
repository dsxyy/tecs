#ifndef STORAGE_ADAPTOR_IMPL_H
#define STORAGE_ADAPTOR_IMPL_H

#include "storage_messages.h"

namespace zte_tecs 
{
#define  LOCALSAN    "LOCALSAN"
#define  IPSAN       "IPSAN"

class sa_cfg:public Serializable
{

public:
    sa_cfg() {};
    ~sa_cfg() {};
    
    SERIALIZE 
    {
        SERIALIZE_BEGIN(sa_cfg);       
        WRITE_VALUE(_type);
        WRITE_VALUE(_control_ip);
        WRITE_VALUE(_user);
        WRITE_VALUE(_password);
        WRITE_VALUE(_name);
        WRITE_VALUE(_description);
        WRITE_VALUE(_data_ip);
        SERIALIZE_END();    
    };
        
    DESERIALIZE 
    {
        DESERIALIZE_BEGIN(sa_cfg);       
        READ_VALUE(_type);
        READ_VALUE(_control_ip);
        READ_VALUE(_user);
        READ_VALUE(_password);
        READ_VALUE(_name);
        READ_VALUE(_description);   
        READ_VALUE(_data_ip);
        DESERIALIZE_END();       
    };    
        
    string          _type;            
    string          _control_ip; 
    vector<string>  _data_ip;
    string          _user;
    string          _password;
    string          _name;
    string          _description;
};

class sa_cfg_array:public Serializable
{
public:
    sa_cfg_array() {};
    ~sa_cfg_array() {};
    
    SERIALIZE 
    {
        SERIALIZE_BEGIN(sa_cfg_array);       
        WRITE_OBJECT_VECTOR(_cfg_array);
        SERIALIZE_END();    
    };
        
    DESERIALIZE 
    {
        DESERIALIZE_BEGIN(sa_cfg_array);       
        READ_OBJECT_VECTOR(_cfg_array);       
        DESERIALIZE_END();            
    };    
        
    vector<sa_cfg> _cfg_array;
};

class StorageAdaptorImpl
{
public:   
    virtual ~StorageAdaptorImpl() {};
    virtual STATUS Init(const sa_cfg & cfg) = 0;
    virtual STATUS AllocVol(const StAllocVolReq & req,string &vol_uuid)=0;
    virtual STATUS DeleteVol(const StDelVolReq & req)= 0;  
    virtual STATUS AllocSnapshot(const StAllocSnapshotReq & req,string &vol_uuid)=0;
    virtual STATUS DeleteSnapshot(const StDelVolReq & req)= 0;  
    virtual STATUS CreateTarget(const StCreateTargetReq & req)= 0;
    virtual STATUS DelTarget(const StDelTargetReq & req)= 0;
    virtual STATUS AddVol2Target(const StAddToTarReq& req)= 0; 
    virtual STATUS AddHost2Target(const StAddToTarReq& req)= 0; 
    virtual STATUS DelVolFromTarget(const StDelFromTarReq & req)=0; 
    virtual STATUS DelHostFromTarget(const StDelFromTarReq & req)=0; 
    virtual STATUS GetStorageInfo(capacity & info) =0;
    virtual STATUS KeepAlive(void) 
    {
        return SUCCESS;
    }
    virtual STATUS GetDataIp(vector<string> &dataip)=0;
    virtual STATUS GetVgInfo(vector<Vginfo> &vginfo)=0;
    virtual STATUS ReBuildTarget(sa_target_info &target)
    {
        return SUCCESS;
    }
    int64   Index2Id(int64 index)
    {
        return index;
    }
};

class IpsanAdaptor:public StorageAdaptorImpl
{
public:
    IpsanAdaptor(){};
    ~IpsanAdaptor() {};
    STATUS Init(const sa_cfg & cfg) ;
    STATUS AllocVol(const StAllocVolReq & req,string &vol_uuid);
    STATUS DeleteVol(const StDelVolReq & req);  
    STATUS AllocSnapshot(const StAllocSnapshotReq & req,string &vol_uuid);
    STATUS DeleteSnapshot(const StDelVolReq & req);   
    STATUS CreateTarget(const StCreateTargetReq & req);
    STATUS DelTarget(const StDelTargetReq & req);
    STATUS AddVol2Target(const StAddToTarReq& req); 
    STATUS AddHost2Target(const StAddToTarReq& req); 
    STATUS DelVolFromTarget(const StDelFromTarReq & req); 
    STATUS DelHostFromTarget(const StDelFromTarReq & req); 
    STATUS GetStorageInfo(capacity & info); 
    STATUS KeepAlive(void); 
    STATUS GetDataIp(vector<string>  &dataip);
    STATUS GetVgInfo(vector<Vginfo> &vginfo);
    
private:
    void ProcessCapiRet(int errcode);
    sa_cfg _cfg;
};

class LocalAdaptor:public StorageAdaptorImpl
{
public:
    LocalAdaptor(){};
    ~LocalAdaptor() {};
    STATUS Init(const sa_cfg & cfg) ;
    STATUS AllocVol(const StAllocVolReq & req,string &vol_uuid);
    STATUS DeleteVol(const StDelVolReq & req);  
    STATUS AllocSnapshot(const StAllocSnapshotReq & req,string &vol_uuid);
    STATUS DeleteSnapshot(const StDelVolReq & req);  
    STATUS CreateTarget(const StCreateTargetReq & req);
    STATUS DelTarget(const StDelTargetReq & req);
    STATUS AddVol2Target(const StAddToTarReq& req); 
    STATUS AddHost2Target(const StAddToTarReq& req); 
    STATUS DelVolFromTarget(const StDelFromTarReq & req); 
    STATUS DelHostFromTarget(const StDelFromTarReq & req); 
    STATUS GetStorageInfo(capacity & info);     
    STATUS GetDataIp(vector<string>  &dataip);
    STATUS GetVgInfo(vector<Vginfo> &vginfo);
    STATUS ReBuildTarget(sa_target_info &target);
    int64   Index2Id(int64 index)
    {
        return index+1;
    }
private:
    STATUS RunFind(const string &cmd,const string &sub);
    sa_cfg _cfg;
};

class StorageAdaptorFactory
{
public:
    StorageAdaptorFactory(){};
    ~StorageAdaptorFactory(){};
    
    static StorageAdaptorImpl* GetImpl(string kind)
    {
        StorageAdaptorImpl* impl = NULL;
        if(kind==IPSAN)
        {
            impl = new IpsanAdaptor();
        }
        else if (kind==LOCALSAN)
        {
            impl = new  LocalAdaptor();
        }        
        
        return impl;
    };    
    
};
}
#endif

