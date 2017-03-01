
#ifndef TECS_STORAGE_CONTROL_H
#define TECS_STORAGE_CONTROL_H

#include "sky.h"
#include "storage_messages.h"
#include "volume.h"
#include "workflow.h"



namespace zte_tecs
{

const string SC_CATEROTY ="storage" ;


class  target_info
{
public:
    string  _targetname;
    int32   _lunid;
    string  _ip;
    string  _iscsiname;
    MID     _sa;
};

class req_info
{
public:   
    req_info(string & _request_id):request_id(_request_id)
    {
    };
    req_info()
    {
    };
    string request_id;
    string ip;
    string iscsi_name;  
};

class Authinfo
{
public:
    string target_name;
    int64 target_id;
    int64  lunid;
    bool    create_target_skiped;
    bool    add_host_skiped;
    bool    add_vol_skiped;
    bool    del_target;
    bool    del_host;
    bool    del_vol;
    MID     sender;
};


class StorageController: public MessageHandler
{
public:
    static StorageController *GetInstance()
    {
        if (NULL == _instance)
        {
            _instance = new StorageController();
        }
        return _instance;

    }
    StorageController()
    {
        m=NULL;
    };
    ~StorageController(){};

    STATUS Init();
    STATUS StartMu();
    void MessageEntry(const MessageFrame& frame);

private:
    ScDbOperationSet *_db_set;
    MID _sender;
    MessageUnit *m;
    enum
        {IPSAN_VG_MAX_VOLUME = 32};
    STATUS GenerateTargetName(const string  &type ,const AuthBlockReq &req, string &targetname);
    STATUS GetSnapBaseVolume(vector<string> &vec_baseid,StorageVolume &sv);

    void   ScAckSend(uint32 req_msgid,MID &sender,req_info &rinfo,int64 &uid,WorkAck & w_ack,string &workflow_id);
    void   ScAckSend(uint32 req_msgid,MID &sender,req_info &rinfo,int64 &uid,WorkAck & w_ack);

    STATUS CreateVol(StorageVolumeWithAdaptor &si,CreateBlockReq &req,StAllocVolReq & sa_req,int64 volume_id,MID &sender, string & workflow_id);
    STATUS DeleteVol(StorageVolumeWithAdaptor &si,int req_msgid,DeleteBlockReq &req,StDelVolReq & sa_req,StorageVolume &sv,MID &sender, string & workflow_id);
    STATUS CreateSnapshot(StorageVolumeWithAdaptor &si,CreateSnapshotReq &req,StAllocSnapshotReq & sa_req,int64 volume_id,string &base_uuid,MID &sender);
    STATUS AuthVol(StorageVolumeWithAdaptor &si,AuthBlockReq &req, Authinfo &info);
    STATUS DeAuthVol(StorageVolumeWithAdaptor &si,DeAuthBlockReq &req, Authinfo &info);
    STATUS ScheduletoAdaptor(const string &clustername, int64 volsize, StorageVolumeWithAdaptor &si);
    STATUS HandleSaAck(const MessageFrame &message);

    STATUS HandleCreateVolAck(const StStorageSvrAck &sa_ack,MID &sender,string &action_id,req_info & rinfo,int64 &uid);
    STATUS HandleDelVolAck(const StStorageSvrAck &sa_ack,MID &sender,string &action_id,req_info & rinfo,int64 &uid,bool &isRollBack);
    STATUS HandleAuthAck(const StStorageSvrAck &sa_ack,bool &finish,MID& sender,string &action_id,req_info & rinfo);
    STATUS HandleDeauthAck(const StStorageSvrAck &sa_ack,bool &finish,MID &sender,string &action_id,req_info & rinfo,bool &isRollBack);
    static StorageController *_instance;
    void GetCreateVolWorkFlowNum(int64 sid,const string &vgname,int32 &workflownum);
};


class CreateVolWorkflowLabels: public WorkflowLabels
{
public:
    CreateVolWorkflowLabels():
        request_id(label_string_1),
        cluster_name(label_string_2),
        base_id(label_string_3),
        vol_size(label_int64_1),
        uid(label_int64_2)
    {
        //这个类存在的意义是用引用的方式给label取一个业务自己的名字
        //便于编码时识别和使用
    };
    string get_label_where() const
    {
        ostringstream where;
        where << "label_string_1 = '" << request_id<<"'";
        return where.str();
    }
    string& request_id;
    string& cluster_name;
    string& base_id;
    int64 & vol_size;
    int64 & uid;
};

class CreateVolLabels: public ActionLabels
{
public:
    CreateVolLabels():
        sid(label_int64_1),
        size(label_int64_2),
        volume_id(label_int64_3),
        uid(label_int64_4),
        vgname(label_string_1),
        name(label_string_2),
        request_id(label_string_3),
        description(label_string_4)
    {
        //这个类存在的意义是用引用的方式给label取一个业务自己的名字
        //便于编码时识别和使用
    };
    string get_label_where() const
    {
        ostringstream where;
        where << "label_string_1 = '" << vgname<<"'";
        return where.str();
    }
    int64  &sid;
    int64  &size;
    int64  &volume_id;
    int64  &uid;
    string &vgname;
    string &name;
    string &request_id;
    string &description;

};

class CreateSnapWorkflowLabels: public WorkflowLabels
{
public:
    CreateSnapWorkflowLabels():
        base_id(label_string_1),
        request_id(label_string_2),
        vol_size(label_int64_1)
    {
        //这个类存在的意义是用引用的方式给label取一个业务自己的名字
        //便于编码时识别和使用
    };

    string &base_id;
    string &request_id;
    int64  &vol_size;
};


class DelVolWorkflowLabels: public WorkflowLabels
{
public:
    DelVolWorkflowLabels():
        request_id(label_string_1),
        uid(label_int64_1)
    {
        //这个类存在的意义是用引用的方式给label取一个业务自己的名字
        //便于编码时识别和使用
    };
    string get_label_where() const
    {
        ostringstream where;
        where << "label_string_1 ='" << request_id <<"'";
        return where.str();
    }
    string& request_id;
    int64 & uid;
};


class DelVolLabels: public ActionLabels
{
public:
     DelVolLabels():
        sid(label_int64_1),
        size(label_int64_2),
        volume_id(label_int64_3),
        uid(label_int64_4),
        vgname(label_string_1),
        name(label_string_2),
        request_id(label_string_3),
        description(label_string_4)
    {
        //这个类存在的意义是用引用的方式给label取一个业务自己的名字
        //便于编码时识别和使用
    };
    
    int64  &sid;
    int64  &size;
    int64  &volume_id;
    int64  &uid;
    string &vgname;
    string &name;
    string &request_id;
    string &description;



};

class AuthWorkflowLabels: public WorkflowLabels
{
public:
    AuthWorkflowLabels():
        request_id(label_string_1),
        ip_address(label_string_2),
        iscsi_initiator(label_string_3),
        target_name(label_string_4),
        usage(label_string_5)
    {
        //这个类存在的意义是用引用的方式给label取一个业务自己的名字
        //便于编码时识别和使用
    };
    string get_label_where() const
    {
        ostringstream where;
        where << "label_string_1 = '" << request_id               
               <<"' AND label_string_3 = '"<<iscsi_initiator
               <<"' AND label_string_4 = '"<<target_name<<"'";
               
        return where.str();
    }
    string get_target_where() const
    {
        ostringstream where;
        where << "label_string_4 = '" << target_name <<"'";

        return where.str();
    }
    string &request_id;
    string &ip_address;
    string &iscsi_initiator;
    string &target_name;
    string &usage;
};

class CreateTargetLabels: public ActionLabels
{
public:
    CreateTargetLabels():
        target_name(label_string_1),
        sid(label_int64_1),
        target_id(label_int64_2)
    {
        //这个类存在的意义是用引用的方式给label取一个业务自己的名字
        //便于编码时识别和使用
    };

    string &target_name;
    int64  &sid;
    int64  &target_id;


};

class AddHost2TargetLabels: public ActionLabels
{
public:
    AddHost2TargetLabels():
        target_name(label_string_1),
        ip_address(label_string_2),
        iscsi_initiator(label_string_3),
        sid(label_int64_1)
    {
        //这个类存在的意义是用引用的方式给label取一个业务自己的名字
        //便于编码时识别和使用
    };

    string &target_name;
    string &ip_address;
    string &iscsi_initiator;
    int64  &sid;

};

class AddVol2TargetLabels: public ActionLabels
{
public:
    AddVol2TargetLabels():
        target_name(label_string_1),
        volume_uuid(label_string_2),
        sid(label_int64_1),
        lunid(label_int64_2)
    {
        //这个类存在的意义是用引用的方式给label取一个业务自己的名字
        //便于编码时识别和使用
    };

    string &target_name;
    string &volume_uuid;
    int64  &sid;
    int64  &lunid;

};


class DeAuthWorkflowLabels: public WorkflowLabels
{
public:
     DeAuthWorkflowLabels():
        request_id(label_string_1),
        ip_address(label_string_2),
        iscsi_initiator(label_string_3),
        target_name(label_string_4),
        usage(label_string_5)
    {
        //这个类存在的意义是用引用的方式给label取一个业务自己的名字
        //便于编码时识别和使用
    };

    string get_label_where() const
    {
        ostringstream where;
        where << "label_string_1 = '" << request_id             
               <<"' AND label_string_3 = '"<<iscsi_initiator
               <<"' AND label_string_4 = '"<<target_name<<"'";
               
        return where.str();
    }
    string get_target_where() const
    {
        ostringstream where;
        where << "label_string_4 = '" << target_name <<"'";

        return where.str();
    }
    string &request_id;
    string &ip_address;
    string &iscsi_initiator;
    string &target_name;
    string &usage;
    
};

class DelVol4TargetLabels: public ActionLabels
{
public:
    DelVol4TargetLabels():
        target_name(label_string_1),
        volume_uuid(label_string_2),
        sid(label_int64_1),
        lunid(label_int64_2)
    {
        //这个类存在的意义是用引用的方式给label取一个业务自己的名字
        //便于编码时识别和使用
    };

    string &target_name;
    string &volume_uuid;
    int64  &sid;
    int64  &lunid;


};

class DelHost4TargetLabels: public ActionLabels
{
public:
    DelHost4TargetLabels():
        target_name(label_string_1),
        ip_address(label_string_2),
        iscsi_initiator(label_string_3),
        sid(label_int64_1)
    {
        //这个类存在的意义是用引用的方式给label取一个业务自己的名字
        //便于编码时识别和使用
    };

    string &target_name;
    string &ip_address;
    string &iscsi_initiator;
    int64  &sid;


};

class DelTargetLabels:  public ActionLabels
{
public:
    DelTargetLabels():
        target_name(label_string_1),
        sid(label_int64_1),
        target_id(label_int64_2)
    {
        //这个类存在的意义是用引用的方式给label取一个业务自己的名字
        //便于编码时识别和使用
    };

    string &target_name;
    int64  &sid;
    int64  &target_id;
    


};



}

#endif

