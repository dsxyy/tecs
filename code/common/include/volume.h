#ifndef BLOCK_ADAPTOR_H
#define BLOCK_ADAPTOR_H
#include "tecs_pub.h"
#include "iscsi.h"
#include "workflow.h"

namespace zte_tecs
{


#define FILE_OPE_SUCCESS                 0
#define FILE_STAT_ERR                    1
#define FILE_NOT_EXIST                   2
#define FILE_ERR_DEST_EXIST              3
#define FILE_SYS_ERR                     4
#define FILE_ERR_NOT_FILE                5

#define  VOS_ISDIR(mode)      (S_ISDIR(mode))
#define  VOS_ISREG(mode)      (S_ISREG(mode))

#define   STORAGE_DIR      "/dev/tecs_storage/"
enum StorageAdaptorOp
{
    OP_CREATE_VOL = 0,
    OP_CREATE_SVOL=1,
    OP_DEL_VOL=2,
    OP_DEL_SVOL=3,
    OP_CREATE_TARGET=4,
    OP_ADD_HOST2TARGET =5,
    OP_ADD_VOL2TARGET=6,
    OP_DEL_VOLFROMTARGET=7,
    OP_DEL_HOSTFROMTARGET=8,
    OP_DEL_TARGET=9
};

enum StorageScOp
{
    SC_CREATE_VOL = 0,
    SC_AUTH_BLOCK=1,
    SC_DEAUTH_BLOCK=2,
    SC_DEL_VOL=3,
    SC_GET_VOLINFO=4,
    SC_ADD_AUTH_HOST = 5,
    SC_DEL_AUTH_HOST =6
};


/*****************************************************************************/
/* VmCfgManager 向SC 发送资源查找请求 */
/*该消息结构的消息ID为: EV_FILTER_CLUSTER_BY_SHARE_STORAGE_REQ */
/*****************************************************************************/
class  FilterClusterByShareStorageReq:  public  WorkReq
{     
public:
    FilterClusterByShareStorageReq(const string &id_of_action,
                                             int64  share_disksize,
                                             const string &appointed_cluster):WorkReq(id_of_action)
    {
        _share_disksize    = share_disksize;
        _appointed_cluster = appointed_cluster;
    };

    FilterClusterByShareStorageReq(){};

    SERIALIZE
    {
        SERIALIZE_BEGIN(FilterClusterByShareStorageReq);
        WRITE_VALUE(_share_disksize);       
        WRITE_VALUE(_appointed_cluster);        
        SERIALIZE_PARENT_END(WorkReq);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(FilterClusterByShareStorageReq);
        READ_VALUE(_share_disksize);      
        READ_VALUE(_appointed_cluster);        
        DESERIALIZE_PARENT_END(WorkReq);
    };

    int64    _share_disksize;     // 请求的共享硬盘大小  
    string   _appointed_cluster;   //指定集群部署
};

/*****************************************************************************/
/* SC 给VmCfgManager 回应的资源查找应答 */
/*该消息结构的消息ID为: EV_FILTER_CLUSTER_BY_SHARE_STORAGE_ACK */
/*****************************************************************************/
class FilterClusterByShareStorageAck:  public  WorkAck
{     
public:
    FilterClusterByShareStorageAck(const string & id):WorkAck(id)
    {
    };

    FilterClusterByShareStorageAck(){};

    SERIALIZE
    {
        SERIALIZE_BEGIN(FilterClusterByShareStorageAck);
        WRITE_VALUE(_cluster_list);            
        SERIALIZE_PARENT_END(WorkAck);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(FilterClusterByShareStorageAck);
        READ_VALUE(_cluster_list);           
        DESERIALIZE_PARENT_END(WorkAck);
    };

    vector <string>   _cluster_list;    //查询到的CC列表
};

class CreateBlockReq:public WorkReq
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    CreateBlockReq() 
    {
        _uid = INVALID_UID;
        _volsize = 0;
        _user = "";
    };

    ~CreateBlockReq() {};

    int64  _uid;
    int64  _volsize;
    string _cluster_name;
    string _request_id;
    string _description;
    string _user;

    SERIALIZE
    {
        SERIALIZE_BEGIN(CreateBlockReq);
        WRITE_DIGIT(_uid);
        WRITE_DIGIT(_volsize);
        WRITE_STRING(_cluster_name);
        WRITE_STRING(_request_id);
        WRITE_STRING(_description);
        WRITE_STRING(_user);
        SERIALIZE_PARENT_END(WorkReq);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CreateBlockReq);
        READ_DIGIT(_uid);
        READ_DIGIT(_volsize);
        READ_STRING(_cluster_name);
        READ_STRING(_request_id);
        READ_STRING(_description);
        READ_STRING(_user);
        DESERIALIZE_PARENT_END(WorkReq);
    };
};

class CreateBlockAck:public WorkAck
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    CreateBlockAck() {};
    CreateBlockAck(const string &id,int state,int progress,const string & request_id)
        :WorkAck(id,state,progress),_request_id(request_id)
    {
    };
    CreateBlockAck(const string &id,int state,int progress,const string & request_id,int64 uid)
        :WorkAck(id,state,progress),_request_id(request_id),_uid(uid)
    {
    };
    CreateBlockAck(const string &id,int state,int progress,const string & request_id,int64 uid,string & workflow_id)
        :WorkAck(id,state,progress),_request_id(request_id),_uid(uid),_workflow_id(workflow_id)
    {
    };
    CreateBlockAck(const string &id,int state,int progress,const string & request_id,int64 uid,string & workflow_id,const string & detail_info)
        :WorkAck(id,state,progress),_request_id(request_id),_uid(uid),_workflow_id(workflow_id)
    {
        detail = detail_info;
    };
    ~CreateBlockAck() {};

    string _request_id;
    int64 _uid;
    string _workflow_id;

    SERIALIZE
    {
        SERIALIZE_BEGIN(CreateBlockAck);
        WRITE_STRING(_request_id);
        WRITE_DIGIT(_uid);
        WRITE_STRING(_workflow_id);
        SERIALIZE_PARENT_END(WorkAck);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CreateBlockAck);
        READ_STRING(_request_id);
        READ_DIGIT(_uid);
        READ_STRING(_workflow_id);
        DESERIALIZE_PARENT_END(WorkAck);
    };
};

class DeleteBlockReq:public WorkReq
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    DeleteBlockReq() 
    {
        _uid = INVALID_UID;
        _user = "";
    };

    ~DeleteBlockReq() {};

    int64  _uid;
    string _request_id;
    string _user;

    SERIALIZE
    {
        SERIALIZE_BEGIN(DeleteBlockReq);
        WRITE_DIGIT(_uid);
        WRITE_STRING(_request_id);
        WRITE_STRING(_user);
        SERIALIZE_PARENT_END(WorkReq);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(DeleteBlockReq);
        READ_DIGIT(_uid);
        READ_STRING(_request_id);
        READ_STRING(_user);
        DESERIALIZE_PARENT_END(WorkReq);
    };
};

class DeleteBlockAck:public WorkAck
{
public:
    DeleteBlockAck() {};
    DeleteBlockAck(const string &id,int state,int progress,const string & request_id)
        :WorkAck(id,state,progress),_request_id(request_id)
    {
    };
    DeleteBlockAck(const string &id,int state,int progress,const string & request_id,int64 uid)
        :WorkAck(id,state,progress),_request_id(request_id),_uid(uid)
    {
    };
    DeleteBlockAck(const string &id,int state,int progress,const string & request_id,int64 uid,string & workflow_id)
        :WorkAck(id,state,progress),_request_id(request_id),_uid(uid),_workflow_id(workflow_id)
    {
    };
    ~DeleteBlockAck() {};

    string _request_id;
    int64  _uid;
    string _workflow_id;

    SERIALIZE
    {
        SERIALIZE_BEGIN(DeleteBlockAck);
        WRITE_STRING(_request_id);
        WRITE_DIGIT(_uid);
        WRITE_STRING(_workflow_id);
        SERIALIZE_PARENT_END(WorkAck);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(DeleteBlockAck);
        READ_STRING(_request_id);
        READ_DIGIT(_uid);
        READ_STRING(_workflow_id);
        DESERIALIZE_PARENT_END(WorkAck);
    };

};


class CreateSnapshotReq:public WorkReq
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    CreateSnapshotReq() 
    {
        _volsize = 0;
    };

    ~CreateSnapshotReq() {};


    int64           _volsize;
    vector<string>  _vec_baseid;
    string          _request_id;

    SERIALIZE
    {
        SERIALIZE_BEGIN(CreateSnapshotReq);
        WRITE_DIGIT(_volsize);
        WRITE_VALUE(_vec_baseid);
        WRITE_STRING(_request_id);
        SERIALIZE_PARENT_END(WorkReq);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CreateSnapshotReq);
        READ_DIGIT(_volsize);
        READ_VALUE(_vec_baseid);
        READ_STRING(_request_id);
        DESERIALIZE_PARENT_END(WorkReq);
    };
};

class CreateSnapshotAck:public WorkAck
{
public:
    CreateSnapshotAck() {};
    CreateSnapshotAck(const string &id,int state,int progress,const string & request_id)
        :WorkAck(id,state,progress),_request_id(request_id)
    {
    };
    CreateSnapshotAck(const string &id,int state,int progress,const string & request_id,int64 uid)
        :WorkAck(id,state,progress),_request_id(request_id),_uid(uid)
    {
    };
    CreateSnapshotAck(const string &id,int state,int progress,const string & request_id,int64 uid,string & workflow_id)
        :WorkAck(id,state,progress),_request_id(request_id),_uid(uid),_workflow_id(workflow_id)
    {
    };
    CreateSnapshotAck(const string &id,int state,int progress,const string & request_id,int64 uid,string & workflow_id,const string & detail_info)
        :WorkAck(id,state,progress),_request_id(request_id),_uid(uid),_workflow_id(workflow_id)
    {
        detail = detail_info;
    };
    ~CreateSnapshotAck() {};

    string _request_id;
    int64 _uid;
    string _workflow_id;

    SERIALIZE
    {
        SERIALIZE_BEGIN(CreateSnapshotAck);
        WRITE_STRING(_request_id);
        WRITE_DIGIT(_uid);
        WRITE_STRING(_workflow_id);
        SERIALIZE_PARENT_END(WorkAck);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CreateSnapshotAck);
        READ_STRING(_request_id);
        READ_DIGIT(_uid);
        READ_STRING(_workflow_id);
        DESERIALIZE_PARENT_END(WorkAck);
    };
};

class DeleteSnapshotReq:public WorkReq
{
public:
    DeleteSnapshotReq() 
    {
        _uid = INVALID_UID;
        _user = "";
    };

    ~DeleteSnapshotReq() {};

    int64  _uid;
    string _request_id;
    string _user;

    SERIALIZE
    {
        SERIALIZE_BEGIN(DeleteSnapshotReq);
        WRITE_DIGIT(_uid);
        WRITE_STRING(_request_id);
        WRITE_STRING(_user);
        SERIALIZE_PARENT_END(WorkReq);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(DeleteSnapshotReq);
        READ_DIGIT(_uid);
        READ_STRING(_request_id);
        READ_STRING(_user);
        DESERIALIZE_PARENT_END(WorkReq);
    };
};

class DeleteSnapshotAck:public WorkAck
{
public:
    DeleteSnapshotAck() {};
    DeleteSnapshotAck(const string &id,int state,int progress,const string & request_id)
        :WorkAck(id,state,progress),_request_id(request_id)
    {
    };
    DeleteSnapshotAck(const string &id,int state,int progress,const string & request_id,int64 uid)
        :WorkAck(id,state,progress),_request_id(request_id),_uid(uid)
    {
    };
    DeleteSnapshotAck(const string &id,int state,int progress,const string & request_id,int64 uid,string & workflow_id)
        :WorkAck(id,state,progress),_request_id(request_id),_uid(uid),_workflow_id(workflow_id)
    {
    };
    ~DeleteSnapshotAck() {};

    string _request_id;
    int64 _uid;
    string _workflow_id;

    SERIALIZE
    {
        SERIALIZE_BEGIN(DeleteSnapshotAck);
        WRITE_STRING(_request_id);
        WRITE_DIGIT(_uid);
        WRITE_STRING(_workflow_id);
        SERIALIZE_PARENT_END(WorkAck);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(DeleteSnapshotAck);
        READ_STRING(_request_id);
        READ_DIGIT(_uid);
        READ_STRING(_workflow_id);
        DESERIALIZE_PARENT_END(WorkAck);
    };
};


class AuthBlockReq:public WorkReq
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    AuthBlockReq() {};

    ~AuthBlockReq() {};

    string _request_id;
    string _iscsiname;
    string _userip;
    int64  _vid;
    int64  _dist_id; //识别号，当vid为-1时起作用
    string _cluster_name;
    string _usage;

    SERIALIZE
    {
        SERIALIZE_BEGIN(AuthBlockReq);
        WRITE_VALUE(_request_id);
        WRITE_VALUE(_iscsiname);
        WRITE_VALUE(_userip);
        WRITE_VALUE (_vid);
        WRITE_VALUE (_dist_id);
        WRITE_VALUE (_cluster_name);
        WRITE_VALUE (_usage);
        SERIALIZE_PARENT_END(WorkReq);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(AuthBlockReq);
        READ_VALUE(_request_id);
        READ_VALUE(_iscsiname);
        READ_VALUE(_userip);
        READ_VALUE (_vid);
        READ_VALUE (_dist_id);
        READ_VALUE (_cluster_name);
        READ_VALUE (_usage);
        DESERIALIZE_PARENT_END(WorkReq);
    };
};

class AuthBlockAck:public WorkAck
{
public:

    string  _request_id;
    string  _type;
    string  _target_name;
    vector<string>  _dataip_vec;
    int32   _lunid;

    AuthBlockAck(){};

    AuthBlockAck(const string &id,int state,int progress,const string & request_id)
        :WorkAck(id,state,progress),_request_id(request_id)
    {
    };
    SERIALIZE
    {
        SERIALIZE_BEGIN(AuthBlockAck);
        WRITE_VALUE(_request_id);
        WRITE_VALUE(_type);
        WRITE_VALUE(_target_name);
        WRITE_VALUE(_dataip_vec);
        WRITE_VALUE(_lunid);
        SERIALIZE_PARENT_END(WorkAck);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(AuthBlockAck);
        READ_VALUE(_request_id);
        READ_VALUE(_type);
        READ_VALUE(_target_name);
        READ_VALUE(_dataip_vec);
        READ_VALUE(_lunid);
        DESERIALIZE_PARENT_END(WorkAck);
    };
};



class DeAuthBlockReq:public AuthBlockReq
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    DeAuthBlockReq(){};
};

class DeAuthBlockAck:public WorkAck
{
public:
    DeAuthBlockAck() {};
    DeAuthBlockAck(const string &id,int state,int progress,const string & request_id)
        :WorkAck(id,state,progress),_request_id(request_id)
    {
    };
    DeAuthBlockAck(const string &id,int state,int progress,const string & request_id,int64 uid)
        :WorkAck(id,state,progress),_request_id(request_id),_uid(uid)
    {
    };
    DeAuthBlockAck(const string &id,int state,int progress,const string & request_id,int64 uid,string & workflow_id)
        :WorkAck(id,state,progress),_request_id(request_id),_uid(uid),_workflow_id(workflow_id)
    {
    };
    ~DeAuthBlockAck() {};

    string _request_id;
    int64 _uid;
    string _workflow_id;

    SERIALIZE
    {
        SERIALIZE_BEGIN(DeAuthBlockAck);
        WRITE_STRING(_request_id);
        WRITE_DIGIT(_uid);
        WRITE_STRING(_workflow_id);
        SERIALIZE_PARENT_END(WorkAck);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(DeAuthBlockAck);
        READ_STRING(_request_id);
        READ_DIGIT(_uid);
        READ_STRING(_workflow_id);
        DESERIALIZE_PARENT_END(WorkAck);
    };
};

class GetBlockInfoReq:public WorkReq
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    GetBlockInfoReq() {};

    ~GetBlockInfoReq() {};
    string _request_id;

    SERIALIZE
    {
        SERIALIZE_BEGIN(GetBlockInfoReq);
        WRITE_VALUE(_request_id);   
        SERIALIZE_PARENT_END(WorkReq);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(GetBlockInfoReq);
        READ_VALUE(_request_id);      
        DESERIALIZE_PARENT_END(WorkReq);
    };
};

class GetBlockInfoAck:public WorkAck
{
public:

    string  _request_id;
    string  _type;
    string  _target_name;
    vector<string>  _dataip_vec;
    int32   _lunid;
    string  _iscsi_name;
    string  _ip_address;

    GetBlockInfoAck(){};

    GetBlockInfoAck(const string &id,int state,int progress,const string & request_id)
        :WorkAck(id,state,progress),_request_id(request_id)
    {
    };
    SERIALIZE
    {
        SERIALIZE_BEGIN(GetBlockInfoAck);
        WRITE_VALUE(_request_id);
        WRITE_VALUE(_type);
        WRITE_VALUE(_target_name);
        WRITE_VALUE(_dataip_vec);
        WRITE_VALUE(_lunid);
        WRITE_VALUE(_iscsi_name);
        WRITE_VALUE(_ip_address);
        SERIALIZE_PARENT_END(WorkAck);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(GetBlockInfoAck);
        READ_VALUE(_request_id);
        READ_VALUE(_type);
        READ_VALUE(_target_name);
        READ_VALUE(_dataip_vec);
        READ_VALUE(_lunid);
        READ_VALUE(_iscsi_name);
        READ_VALUE(_ip_address);
        DESERIALIZE_PARENT_END(WorkAck);
    };
};




class auth_info
{
public:
    auth_info()
    {
        vid = INVALID_VID;
        dist_id = INVALID_OID;
        lunid=0;
    };

    ~auth_info(){};
    string  request_id;
    int64   vid;
    int64   dist_id;
    string  cluster_name;
    string  userip ;
    string  iscsiname;
    int32   lunid;
    string  target_name;

};

typedef  struct tagBlockLun
{
    int64  size;
    int32  lunid;
}T_BLOCK_LUN;

typedef  struct tagLoadBlock
{
    int32  lunid;
    string sTarget;
    string sDeviceName;

}T_LOAD_BLOCK_INFO;

typedef  struct tagUnLoadBlock
{
    int32  lunid;
    string sTarget;

}T_UNLOAD_BLOCK_INFO;

typedef struct tagFileStat
{
    uint64  st_dev;      /**< 文件所在设备的标识*/
    uint32  st_ino;      /**< 与该文件关联的inode*/
    uint32  st_mode;     /**< 文件权限和文件类型信息*/
    uint32  st_nlink;    /**< 该文件上硬连接的个数*/
    uint32  st_uid;      /**< 文件属主的UID号*/
    uint32  st_gid;      /**< 文件属主的GID号*/
    uint64  st_rdev;     /**< 文件所表示的特殊设备文件的设备标识*/
    int64   st_size;     /**< 文件大小，字节为单位*/
    int32   dwfs_atime;  /**< 文件上一次被访问的时间*/
    int32   dwfs_mtime;  /**< 文件上一次被修改的时间*/
    int32   dwfs_ctime;  /**< 文件的权限、属主、组或内容上一次被修改的时间*/
}T_FileStat;

class LunAddr
{
public:
    LunAddr(const string &target,int32 lunid)
        :_target(target),_lunid(lunid)
          
    {
    };
    LunAddr()
    {
        _lunid=-1;
    }
 
    string _target;
    int32  _lunid;
};

class iScsiAuthTable: public Serializable
{
public:
    void AddAuth(const string &iscsiname,LunAddr &addr)
    {
        string addrstr = addr._target + ":" + to_string<int>(addr._lunid,dec);
        map<string,string>::iterator iter = _table.find(iscsiname);
        if( iter == _table.end() ) 
        {                 
            
            _table.insert(make_pair(iscsiname,addrstr));
        }
        else
        {
            _table[iscsiname]=addrstr;
        }
           
    };
    void DelAuth(const string &iscsiname)
    {
        map<string,string>::iterator iter = _table.find(iscsiname);
        if( iter != _table.end() ) 
        {
            _table.erase(iter);
        }
    };
    SERIALIZE
    {
        SERIALIZE_BEGIN(iScsiAuthTable);       
        WRITE_VALUE(_table);       
        SERIALIZE_END();
    }

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(iScsiAuthTable);      
        READ_VALUE(_table);       
        DESERIALIZE_END();
    }
  
    map<string,string> _table;  
};


//iscsi访问选项，download客户端在有了文件uri之后，需要利用此选项来下载
class iSCSIAccessOption: public Serializable
{
public:   
    string          _type;
    string          _request_id;
    vector<string>  _dataip_array;
    iScsiAuthTable  _auth_tbl;
    iSCSIAccessOption()
    {

    };


    SERIALIZE
    {
        SERIALIZE_BEGIN(iSCSIAccessOption);       
        WRITE_VALUE(_type);
        WRITE_VALUE(_request_id);
        WRITE_STRING_VECTOR(_dataip_array);
        WRITE_OBJECT(_auth_tbl);
        SERIALIZE_END();
    }

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(iSCSIAccessOption);      
        READ_VALUE(_type);
        READ_VALUE(_request_id);
        READ_STRING_VECTOR(_dataip_array);       
        READ_OBJECT(_auth_tbl);
        DESERIALIZE_END();
    }
};

class Sc_AckInfo
{
public:
    string request_id;
    int    last_op;
    int    ret;
    iSCSIAccessOption option;
};

class BlockAdaptorDataPlane
{
public:    
    static BlockAdaptorDataPlane *GetInstance()
    {
        if (NULL == _instance)
        {
            _instance = new BlockAdaptorDataPlane();
        }
        return _instance;

    }
    
    STATUS Init();
    BlockAdaptorDataPlane()
    {
        _init=false;
    };    
    ~BlockAdaptorDataPlane(){};

    STATUS LoadBlocks(const string &type, vector<string>& iplist,int64 vmid, string &targetname,vector<T_LOAD_BLOCK_INFO> & loadinfo,int &iLoadNum);
    STATUS RefreshLoadBlocks(const string &type, vector<string>& iplist, string &targetname,vector<T_LOAD_BLOCK_INFO> & loadinfo);
    STATUS UnloadBlocks(const string &type,vector<string>& iplist,int64 vmid,string &targetname, vector<T_UNLOAD_BLOCK_INFO> & unloadinfo);
    void   ReclaimDev();
    
private:
    static BlockAdaptorDataPlane *_instance;
    Mutex _login_mutex;
    bool  _init;
    
};

}




#endif

