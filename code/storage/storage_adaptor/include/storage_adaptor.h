#ifndef TECS_STORAGE_ADAPTOR_H
#define TECS_STORAGE_ADAPTOR_H

#include "sky.h"
#include "storage_adaptor_impl.h"
#include "storage_messages.h"

namespace zte_tecs 
{

class Storage_Adaptor: public MessageHandlerTpl<MessageUnit>
{
public:
    Storage_Adaptor(sa_cfg &cfg);
    ~Storage_Adaptor(){};

    STATUS Init(void);
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
    STATUS GetDataIp(vector<string> &dataip);
    STATUS GetVgInfo(vector<Vginfo> &vginfo);
    void   MessageEntry(const MessageFrame& message);
    void   GetScname(string &scname);
    STATUS ReBuildTarget(sa_target_info &target);
    bool bSaRegister(void);
        
private:    
    Storage_Adaptor();   
   
    //上报函数
    void DoSaInfoReport();
    void DoStartUp();
    void DoSaRegister(const MessageFrame &message);
    void DoSaUnregister(const MessageFrame &message);

    string               _type;
    sa_cfg               _cfg;
    StorageAdaptorImpl  *_impl;  
    bool                _have_startup;      // 是否已启动的标志，避免重新初始化
    string              _sc_name;
    string              _sa_ip;
    string              _sc_ip;
};

}

#endif

