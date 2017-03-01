/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vm_messages.h
* 文件标识：见配置管理计划书
* 内容摘要：虚拟机相关消息体结构定义
* 当前版本：1.0
* 作    者：陈文文
* 完成日期：2011年9月5日
*
* 修改记录1：
*     修改日期：2011/9/5
*     版 本 号：V1.0
*     修 改 人：陈文文
*     修改内容：创建
******************************************************************************/
#ifndef COMM_VM_MESSAGES_H
#define COMM_VM_MESSAGES_H
#include "api_const.h"
#include "tecs_pub.h"
#include "tecs_errcode.h"
#include "image_url.h"
#include "sky.h"
#include "api_vmbase.h"
#include "vmcfg_api.h"
#include "workflow.h"

using namespace std;
namespace zte_tecs
{
// added by vnet start
typedef enum tagRuleOpType
{
    EN_NAT_RULE_DEPLOY = 0, /* NAT规则设置 */
    EN_NAT_RULE_DELETE, /* NAT规则删除*/
}EN_NAT_RULE_OPTYPE;

typedef struct tagSNATRuleInfo
{
    string IpHC; /* HOST 的IP，HC向CC注册的时候确定*/
    string IpNatin; /* NAT网关的内网地址，VMM 从CC的配置文件中获得*/ 
    WORD32 Optype; /* 设置EN_NAT_RULE_DEPLOY，删除EN_NAT_RULE_DELETE*/
}T_SNATRuleInfo;
// added by vnet end


#define  DURATIONG_BETWEEN_CC_AND_HC  (2)//单位 S
#define  DURATIONG_BETWEEN_TC_AND_CC  (2)

// 虚拟机各种操作等待时间
//STAGING状态超时时长
#define VM_STAGING_DURATION_HC    (2*60*60)  // 2小时
//BOOTING状态超时时长
#define  VM_BOOTING_DURATION_HC   (1*60)  // 1分钟
//释放超时时长
#define  VM_TEARDOWN_DURATION_HC  (3*60)   // 3 分钟
//其他操作
#define VM_OTHER_OP_DURATION_HC    (10)

// CC 
#define VM_DEPLOY_DURATION_CC   (VM_STAGING_DURATION_HC + VM_BOOTING_DURATION_HC + DURATIONG_BETWEEN_CC_AND_HC) 
#define VM_CANCEL_DURATION_CC   (VM_TEARDOWN_DURATION_HC + DURATIONG_BETWEEN_CC_AND_HC)//182 S
#define VM_OTHER_OP_DURATION_CC  (VM_OTHER_OP_DURATION_HC + DURATIONG_BETWEEN_CC_AND_HC) //12 S

// TC 
#define VM_DEPLOY_DURATION_TC   (VM_DEPLOY_DURATION_CC + DURATIONG_BETWEEN_TC_AND_CC)
#define VM_CANCEL_DURATION_TC   (VM_CANCEL_DURATION_CC + DURATIONG_BETWEEN_TC_AND_CC) //184 S
#define VM_OTHER_OP_DURATION_TC  (VM_OTHER_OP_DURATION_CC + DURATIONG_BETWEEN_TC_AND_CC) // 14 S
#define VM_QUERY_DURATION_TC     (3)


// 虚拟机操作请求是否需要执行
#define OP_STAT_LAUNCH          0   /* 命令整装待发 */              //执行命令
#define OP_STAT_PROCESSING      1   /* 命令正在执行 */              //返回当前进度
#define OP_STAT_CONFILICT       2   /* 命令冲突，暂时不能执行 */    //工作流结束?

/* 虚拟机实例的状态 */
/* 修改本enum时必须同步修改vmcfg_api.h中的ApiVmUserState状态!!! */
enum VmInstanceState
{
    /* HC初始状态，报给CC的 */
    VM_INIT     = 0, 

    /* 下面这几个状态是直接呈现给用户的 */
    VM_RUNNING  = 2,
    VM_PAUSED   = 3,
    VM_SHUTOFF  = 4,
    VM_CRASHED  = 5,
    
    /* TC,CC上的状态，HC不会上报这个状态,这个也会呈现给用户的 */
    VM_UNKNOWN  = 6,
    VM_FROZEN   = 8,

    /* TC上的状态，这个也会呈现给用户的 */
    VM_CONFIGURATION = 7,
};


//虚拟机cluster生命周期状态
//注意:修改本enum时必须同步修改Enum2Str::LcmState!!!
enum LcmState
{
    LCM_NO_STATE   = 0,
    SCHEDULING     = 1,
    ALLOCATION     = 2,
    LCM_INIT       = 3,
    PROLOG         = 4,
    DOMAINING      = 5,
    MIGRATING      = 6,
    SAVE_STOP      = 7,
    SAVE_SUSPEND   = 8,
    SAVE_MIGRATE   = 9,
    PROLOG_MIGRATE = 10,
    PROLOG_RESUME  = 11,
    EPILOG_STOP    = 12,
    EPILOG         = 13,
    FAILURE        = 14,
    CLEANUP        = 15,
    UNKNOWN        = 16
};

//哪些状态下应该是位于host上的?
inline bool IsVmOnHost(LcmState state)
{
    if(state != LCM_NO_STATE &&
       state != SCHEDULING &&
       state != ALLOCATION &&
       /* state != LCM_INIT && */
       state != UNKNOWN &&
       state != CLEANUP &&
       state != EPILOG)
    {
        return true;
    }
    return false;
};

//虚拟机操作名称
//注意:修改本enum时必须同步修改Enum2Str::VmOp!!!
enum VmOperation
{
    //TC部分
    SETUP   = 1,
    DELETE  = 2,
    DEPLOY  = 3,   
    CANCEL  = 4,   //撤销虚拟机

    //CC部分
    QUERY    = 5,
    SCHEDULE = 6,
    RESCHEDULE = 7,
    APPLYRES = 8,

     //以下是HC的部分
    CREATE  = 9,
    START   = 10,   //开机
    STOP    = 11,   //关机
    REBOOT  = 12,
    PAUSE   = 13,
    RESUME  = 14,
    PREP_M  = 15,
    MIGRATE  = 16,
    RESET    = 17,   //强制重启
    DESTROY  = 18,   //强制关机
    POST_M   = 19,   // 这个是迁移成功的执行命令，是用来标识迁移执行成功真正切换网络的动作
    SYNC     = 20,
    RR       = 21,   // 这个是REBOOT之后接着RESET，带时间，临时使用
    UPLOAD_IMG     = 22,
    CANCEL_UPLOAD_IMG   = 23,
    PLUG_IN_USB = 24,
    PLUG_OUT_USB = 25,
    OUT_IN_USB = 26,
    PLUG_IN_PDISK = 27,
    PLUG_OUT_PDISK = 28,
    RECOVER_IMAGE = 29,
    MODIFY_IMAGE_BACKUP = 30,

    //CC部分
    LIVE_MIGRATE = 31,
    COLD_MIGRATE = 32,
    RESTORE_IMAGE_BACKUP = 33,
    CREATE_IMAGE_BACKUP = 34,
    DELETE_IMAGE_BACKUP = 35,
    FREEZE = 36,
    UNFREEZE = 37,
    MODIFY_VMCFG = 38,
    UNKNOWN_OP = 39
};

//将各个枚举值返回为字符串形式，便于调试函数中使用
class Enum2Str
{
public:
    #define ENUM2STR(value) case(value): return #value;
    static const char* VmOpStr(int op)
    {
        switch((VmOperation)op)
        {
            ENUM2STR(SETUP);
            ENUM2STR(DELETE);
            ENUM2STR(DEPLOY);
            ENUM2STR(CANCEL);
            ENUM2STR(QUERY);
            ENUM2STR(SCHEDULE);
            ENUM2STR(RESCHEDULE);
            ENUM2STR(APPLYRES);
            ENUM2STR(CREATE);
            ENUM2STR(START);
            ENUM2STR(STOP);
            ENUM2STR(REBOOT);
            ENUM2STR(PAUSE);
            ENUM2STR(RESUME);
            ENUM2STR(PREP_M);
            ENUM2STR(MIGRATE);
            ENUM2STR(RESET);
            ENUM2STR(DESTROY);
            ENUM2STR(POST_M);
            ENUM2STR(SYNC);
            ENUM2STR(RR);
            ENUM2STR(UPLOAD_IMG);
            ENUM2STR(CANCEL_UPLOAD_IMG);
            ENUM2STR(PLUG_IN_USB);
            ENUM2STR(PLUG_OUT_USB);
            ENUM2STR(OUT_IN_USB);
            ENUM2STR(PLUG_IN_PDISK);
            ENUM2STR(PLUG_OUT_PDISK);
            ENUM2STR(RECOVER_IMAGE);
            ENUM2STR(MODIFY_IMAGE_BACKUP);
            ENUM2STR(LIVE_MIGRATE);
            ENUM2STR(COLD_MIGRATE);
            ENUM2STR(RESTORE_IMAGE_BACKUP);
            ENUM2STR(CREATE_IMAGE_BACKUP);
            ENUM2STR(DELETE_IMAGE_BACKUP);
            ENUM2STR(FREEZE);
            ENUM2STR(UNFREEZE);
            ENUM2STR(MODIFY_VMCFG);
            ENUM2STR(UNKNOWN_OP);
            default:return "unsupported_op";
        }
    };

    static const char* InstStateStr(int state)
    {
        switch((VmInstanceState)state)
        {
            ENUM2STR(VM_INIT);
            ENUM2STR(VM_CRASHED);
            ENUM2STR(VM_PAUSED);
            ENUM2STR(VM_RUNNING);
            ENUM2STR(VM_SHUTOFF);
            ENUM2STR(VM_UNKNOWN);
            ENUM2STR(VM_CONFIGURATION);
            ENUM2STR(VM_FROZEN);
            default:return "unknown_inst_state";
        }
    };
    
    static const char* LcmStateStr(int state)
    {
        switch((LcmState)state)
        {
            ENUM2STR(LCM_NO_STATE);
            ENUM2STR(ALLOCATION);
            ENUM2STR(SCHEDULING);
            ENUM2STR(LCM_INIT);
            ENUM2STR(PROLOG);
            ENUM2STR(DOMAINING);
            ENUM2STR(MIGRATING);
            ENUM2STR(SAVE_STOP);
            ENUM2STR(SAVE_SUSPEND);
            ENUM2STR(SAVE_MIGRATE);
            ENUM2STR(PROLOG_MIGRATE);
            ENUM2STR(PROLOG_RESUME);
            ENUM2STR(EPILOG_STOP);
            ENUM2STR(EPILOG);
            ENUM2STR(FAILURE);
            ENUM2STR(CLEANUP);
            ENUM2STR(UNKNOWN);
            default:return "unknown_lcm_state";
        }
    };
    
    
    static const char* VmOpError(int code)
    {
        switch(code)
        {  
            ENUM2STR(VMOP_SUCC);
            ENUM2STR(CREATE_ERROR_EXISTINSTANCE);
            ENUM2STR(CREATE_ERROR_NEWINSTANCE);
            ENUM2STR(CREATE_ERROR_PTHREADCREATE);
            ENUM2STR(CREATE_ERROR_MKDIR);
            ENUM2STR(CREATE_ERROR_STARTNETWORK);
            ENUM2STR(CREATE_ERROR_STARTSTORAGE);
            ENUM2STR(CREATE_ERROR_CREATECONTEXT);
            ENUM2STR(CREATE_ERROR_CREATEISO);
            ENUM2STR(CREATE_ERROR_CREATEXML);
            ENUM2STR(CREATE_ERROR_CREATECP);
            ENUM2STR(CREATE_ERROR_CREATEDOMAIN);
            ENUM2STR(VMOP_ERROR_NOTEXIST);
            ENUM2STR(VMOP_ERROR_NETWORK);
            ENUM2STR(VMOP_ERROR_NETWORK_START);
            ENUM2STR(VMOP_ERROR_NETWORK_STOP);
            ENUM2STR(VMOP_ERROR_NETWORK_NOTIFY);
            ENUM2STR(VMOP_ERROR_STORAGE);
            ENUM2STR(VMOP_ERROR_DIR);
            ENUM2STR(VMOP_ERROR_UNKNOWN);
            ENUM2STR(VMOP_ERROR_REBOOT_FAST);
            ENUM2STR(VMOP_ERROR_CANCEL_ABORT); 
            ENUM2STR(VMOP_RUNNING);
            ENUM2STR(VMOP_VM_DO_FAIL);
            ENUM2STR(VMOP_VM_AGT_BUSY);
            ENUM2STR(VMOP_ERROR_WATCHDOG_START);
            ENUM2STR(VMOP_ERROR_WATCHDOG_STOP);
            ENUM2STR(VMOP_VM_DO_TIMEOUT);
            ENUM2STR(VMOP_ERROR_MOUNT_COREDUMP_URL);
            ENUM2STR(VMOP_ERROR_UMOUNT_COREDUMP_URL);
            ENUM2STR(VMOP_ERROR_STATE);
            ENUM2STR(VMOMP_UPLOAD_ERROR);
            ENUM2STR(VMOP_TIME_OUT);
            ENUM2STR(VMOP_ERROR_LOAD_SHARE_DISK);
            ENUM2STR(VMOP_ERROR_UNLOAD_SHARE_DISK);
            ENUM2STR(VMOP_ERROR_STORAGE_START);
            ENUM2STR(VMOP_ERROR_STORAGE_RELEASE);
            default:return "unknown_vmhandler_error";
        }
    };

    static const char* VmCfgElementStr(int element)
    {
        switch((VmCfgElement)element)
        {
            ENUM2STR(VM_CFG_NAME);
            ENUM2STR(VM_CFG_DESCRIPTION);
            ENUM2STR(VM_CFG_PROJECT);
            ENUM2STR(VM_CFG_VIRT_TYPE);
            ENUM2STR(VM_CFG_HYPERVISOR);
            ENUM2STR(VM_CFG_APPOINTED_CLUSTER);
            ENUM2STR(VM_CFG_APPOINTED_HOST);
            ENUM2STR(VM_CFG_SERIAL);
            ENUM2STR(VM_CFG_COREDUMP);
            ENUM2STR(VM_CFG_ENABLE_LIVEMIGRATE);
            ENUM2STR(VM_CFG_CPU);
            ENUM2STR(VM_CFG_MEMORY);
            ENUM2STR(VM_CFG_VNC_PASSWORD);
            ENUM2STR(VM_CFG_MACHINE);
            ENUM2STR(VM_CFG_WATCH_DOG);
            ENUM2STR(VM_CFG_MUTEX);
            default:return "unknown_vmcfg_element";
        }
    }    
};


// 定义PCI总线类型的设备
class PciBus : public Serializable {
public:
    SERIALIZE
    {
        SERIALIZE_BEGIN(PciBus);
        WRITE_VALUE(_domain);
        WRITE_VALUE(_bus);
        WRITE_VALUE(_slot);
        WRITE_VALUE(_function);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(PciBus);
        READ_VALUE(_domain);
        READ_VALUE(_bus);
        READ_VALUE(_slot);
        READ_VALUE(_function);
        DESERIALIZE_END();
    };

    string          _domain;    //域
    string          _bus;       //总线
    string          _slot;      //槽位
    string          _function;  //功能
};


// 虚拟机查询USB信息
class VmQueryUsbInfo: public Serializable
{
public:
    string name;
    string manufacturer;
    int     vendor_id;
    int     product_id;
    int64 use_vid;
    int    online_states;
    bool  is_conflict;

    #define USB_ON_LINE     0
    #define USB_OFF_LINE    1
    #define USB_UNKOWEN    2

    VmQueryUsbInfo()
    {
        vendor_id = 0;
        product_id = 0;
        use_vid = -1;
        online_states = USB_OFF_LINE;
        is_conflict = false;
        name = "unknown name";
        manufacturer = "unknown factory";
    }

    void clear()
    {
        vendor_id = 0;
        product_id = 0;
        name = "unkowen name";
        manufacturer = "unkowen name";
    }
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(VmQueryUsbInfo);  
        WRITE_VALUE(name);
        WRITE_VALUE(manufacturer);
        WRITE_VALUE(vendor_id);
        WRITE_VALUE(product_id);
        WRITE_VALUE(use_vid);
        WRITE_VALUE(online_states);
        WRITE_VALUE(is_conflict);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VmQueryUsbInfo);
        READ_VALUE(name);
        READ_VALUE(manufacturer);
        READ_VALUE(vendor_id);
        READ_VALUE(product_id);
        READ_VALUE(use_vid);
        READ_VALUE(online_states);
        READ_VALUE(is_conflict);
        DESERIALIZE_END();
    };
};

// 查询虚拟机能够看到的USB设备应答
class VmUsbListAck: public Serializable
{
public:
    vector<VmQueryUsbInfo>   _info;
    string                                _stamp;
    STATUS                             _ret;

    void GetApi(vector<ApiVmUsbInfo> &ApiInfo)
    {
        uint32 i;
        ApiVmUsbInfo temp;
        for (i = 0; i < _info.size(); i++)
        {
            temp.name = _info.at(i).name;
            temp.manufacturer = _info.at(i).manufacturer;
            temp.vendor_id = _info.at(i).vendor_id;
            temp.product_id = _info.at(i).product_id;
            temp.use_vid = _info.at(i).use_vid;
            temp.online_states = _info.at(i).online_states;
            temp.is_conflict = _info.at(i).is_conflict;
            ApiInfo.push_back(temp);
        }
    };

    VmUsbListAck(){};


    VmUsbListAck(STATUS ret)
    {
        _ret = ret;
    }
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(VmUsbListAck);  
        WRITE_OBJECT_ARRAY(_info);
        WRITE_VALUE(_stamp);
        WRITE_VALUE(_ret);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VmUsbListAck);
        READ_OBJECT_ARRAY(_info);
        READ_VALUE(_stamp);
        READ_VALUE(_ret);
        DESERIALIZE_END();
    };
};

//虚拟机操作请求，用于TC->CC，CC->HC
class VmOperationReq: public WorkReq
{
public:
    int64   _vid;
    int     _operation;
    string  _stamp;
    string  _user;

    VmOperationReq() 
    {
        _vid = -1;
        _operation = UNKNOWN_OP;
    };

    VmOperationReq(const string &id_of_action):WorkReq(id_of_action){};
    
    VmOperationReq(int64 vid,int operation):
    _vid(vid),
    _operation(operation) 
    {};

    VmOperationReq(const string id_of_action ,
                         int64 vid,
                         int operation,
                         string user):
    WorkReq(id_of_action),                     
    _vid(vid),
    _operation(operation),
    _user(user)
    {};

    VmOperationReq(int64 vid,int operation,const string& stamp):
    _vid(vid),
    _operation(operation),
    _stamp(stamp)
    {};
    
    VmOperationReq(int64 vid,int operation,const string& stamp, string user):
    _vid(vid),
    _operation(operation),
    _stamp(stamp),
    _user(user)
    {};

    SERIALIZE
    {
        SERIALIZE_BEGIN(VmOperationReq);  
        WRITE_DIGIT(_vid);
        WRITE_DIGIT(_operation);
        WRITE_STRING(_stamp);
        WRITE_STRING(_user);
        SERIALIZE_PARENT_END(WorkReq);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VmOperationReq);
        READ_DIGIT(_vid);
        READ_DIGIT(_operation);
        READ_STRING(_stamp);
        READ_STRING(_user);
        DESERIALIZE_PARENT_END(WorkReq);
    };
};


class VmUsbInfo: public Serializable
{
public:
    
    int _vendor_id;
    int _product_id;
    string _name;
    
    VmUsbInfo()
    {
        _vendor_id = 0;
        _product_id = 0;
    };

    VmUsbInfo(int vendor_id, int product_id, const string &name) : _name(name)
    {
        _vendor_id = vendor_id;
        _product_id = product_id;
    }

    SERIALIZE
    {
        SERIALIZE_BEGIN(VmUsbInfo);
        WRITE_VALUE(_vendor_id);
        WRITE_VALUE(_product_id);
        WRITE_VALUE(_name);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VmUsbInfo);
        READ_VALUE(_vendor_id);
        READ_VALUE(_product_id);
        READ_VALUE(_name);
        DESERIALIZE_END();
    };
};

class VmUsbOp:public VmOperationReq
{
public:
    VmUsbInfo _usb_info;

    VmUsbOp() : _usb_info()
    {
    }
    
    VmUsbOp( int64 vid, int op_cmd, 
                         int vendor_id, int product_id, 
                         string  &name, string stamp):
     VmOperationReq(vid, op_cmd, stamp),
        _usb_info(vendor_id, product_id, name)
    {

    };

    SERIALIZE
    {
        SERIALIZE_BEGIN(VmUsbOp);  
        WRITE_OBJECT(_usb_info);
        SERIALIZE_PARENT_END(VmOperationReq);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VmUsbOp);
        READ_OBJECT(_usb_info);
        DESERIALIZE_PARENT_END(VmOperationReq);
    };
};

class VmPortableDiskOp:public VmOperationReq
{
public:
    string _target;
    string _bus;
    string _request_id;

    VmPortableDiskOp()
    {
    };
    
    VmPortableDiskOp( int64 vid, int op_cmd, 
                         const string& target, const string& bus, 
                         const string& request_id,  const string& stamp, const string& user):
    VmOperationReq(vid, op_cmd, stamp, user), _target(target), _bus(bus), _request_id(request_id)
    {
    };

    SERIALIZE
    {
        SERIALIZE_BEGIN(VmPortableDiskOp);  
        WRITE_VALUE(_target);
        WRITE_VALUE(_bus);
        WRITE_VALUE(_request_id);        
        SERIALIZE_PARENT_END(VmOperationReq);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VmPortableDiskOp);
        READ_VALUE(_target);
        READ_VALUE(_bus);
        READ_VALUE(_request_id);       
        DESERIALIZE_PARENT_END(VmOperationReq);
    };
};

class VmImageBackupOp:public VmOperationReq
{
public:
    string _request_id;
    string _target;
    string _description;

    VmImageBackupOp ()
    {
    };
    
    VmImageBackupOp ( int64 vid, int op_cmd, const string& stamp ,
                             const string& request_id, const string& target, const string& description ):
    VmOperationReq(vid, op_cmd, stamp), _request_id(request_id), _target(target), _description(description)
    {
    };

    SERIALIZE
    {
        SERIALIZE_BEGIN(VmImageBackupOp);  
        WRITE_VALUE(_request_id);   
        WRITE_VALUE(_target);  
        WRITE_VALUE(_description);  
        SERIALIZE_PARENT_END(VmOperationReq);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VmImageBackupOp);
        READ_VALUE(_request_id);   
        READ_VALUE(_target);
        READ_VALUE (_description);  
        DESERIALIZE_PARENT_END(VmOperationReq);
    };
};

//虚拟机操作应答，用于HC->CC，CC->TC
class VmOperationAck: public WorkAck
{
public:
    int64   _vid;
    int     _operation;
    int     _vm_state;        // 虚拟机状态
    string  _workflow_id;
    string  _workflow_engine;

    VmOperationAck(const string &id_of_action):WorkAck(id_of_action){};
    
    VmOperationAck() 
    {
        _vid                  = -1;
        _operation        = UNKNOWN_OP;
        _vm_state        = VM_INIT;
        progress           = 0;
        state                 = VMOP_ERROR_UNKNOWN;
    };


    VmOperationAck(const string &_workreq, 
                         int            _state, 
                         int           _progress, 
                         const string &_detail,
                         int64        vid, 
                         int          operation)
                        : WorkAck(_workreq, _state, _progress, _detail) 
    {
        _vid = vid;
        _operation = operation;
        _vm_state = VM_INIT;        
    }

    VmOperationAck(const string &_workreq, 
                         int            _state, 
                         int           _progress, 
                         const string &_detail,
                         int64        vid, 
                         int          operation,
                         int          vm_state)
                        : WorkAck(_workreq, _state, _progress, _detail) 
    {
        _vid = vid;
        _operation = operation;
        _vm_state  = vm_state;
    }

    //检查ack的基本合法性
    bool Validate()
    {
        if(progress < 0 || progress > 100)
        {
            return false;
        }

        if((progress != 100 && state == SUCCESS) || (progress == 100 && state != SUCCESS))
        {
            return false;
        }

        return true;
    }

    SERIALIZE
    {
        SERIALIZE_BEGIN(VmOperationAck);  
        WRITE_VALUE(_vid);
        WRITE_VALUE(_operation);
        WRITE_VALUE(_vm_state);
        WRITE_VALUE(_workflow_id);
        WRITE_VALUE(_workflow_engine);
        SERIALIZE_PARENT_END(WorkAck);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VmOperationAck);
        READ_VALUE(_vid);
        READ_VALUE(_operation);
        READ_VALUE(_vm_state);
        READ_VALUE(_workflow_id);
        READ_VALUE(_workflow_engine);
        DESERIALIZE_PARENT_END(WorkAck);
    };
};

class NicInfo : public Serializable
{
public:
    string          _name;      // 物理网卡的名称
    int64           _speed;     // 物理网卡的速率, 单位是 B
    int64           _net_flow;  // 统计间隔时间内的流量, 收发包的总量, 单位是 B
    int32           _used_rate; // 统计间隔时间内的网卡使用率

    NicInfo():
        _speed(0),
        _net_flow(0),
        _used_rate(0)
    {
    };

    NicInfo(string name):
        _speed(0),
        _net_flow(0),
        _used_rate(0)
    {
        _name = name;
    };

    bool operator()(NicInfo& nic_info) const
    {
        return (_name == nic_info._name);
    };
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(NicInfo);
        WRITE_VALUE(_name);
        WRITE_VALUE(_speed);
        WRITE_VALUE(_net_flow);
        WRITE_VALUE(_used_rate);
        SERIALIZE_END();
    };
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(NicInfo);
        READ_VALUE(_name); 
        READ_VALUE(_speed); 
        READ_VALUE(_net_flow); 
        READ_VALUE(_used_rate); 
        DESERIALIZE_END();
    };
};

class VmStatisticsNics : public Serializable
{
public:
    int64           _vid;
    string          _nic_name;
    string          _statistics_time;
    string          _save_time;
    int            _used_rate;

    VmStatisticsNics()
    {
        _vid = INVALID_VID;
        _nic_name = "";
        _used_rate = 0;
    };

    ~VmStatisticsNics(){};

    SERIALIZE
    {
        SERIALIZE_BEGIN(VmStatisticsNics);
        WRITE_VALUE(_vid);
        WRITE_VALUE(_nic_name);
        WRITE_VALUE(_statistics_time);
        WRITE_VALUE(_save_time);
        WRITE_VALUE(_used_rate);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VmStatisticsNics);
        READ_VALUE(_vid);
        READ_VALUE(_nic_name);
        READ_VALUE(_statistics_time);
        READ_VALUE(_save_time);
        READ_VALUE(_used_rate);
        DESERIALIZE_END();
    };
};


class ResourceStatistics : public Serializable
{
public:
    string _statistics_time; // HC 统计的时间
    int32  _cpu_used_rate;
    int32  _mem_used_rate;
    vector<NicInfo> _vec_nic_info;

    ResourceStatistics():
        _cpu_used_rate(0),
        _mem_used_rate(0),
        _vec_nic_info()
    {
    };

    SERIALIZE
    {
        SERIALIZE_BEGIN(ResourceStatistics);
        WRITE_VALUE(_statistics_time);
        WRITE_VALUE(_cpu_used_rate);
        WRITE_VALUE(_mem_used_rate);
        WRITE_OBJECT_VECTOR(_vec_nic_info);
        SERIALIZE_END();
    };
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(ResourceStatistics);
        READ_VALUE(_statistics_time);
        READ_VALUE(_cpu_used_rate);
        READ_VALUE(_mem_used_rate);
        READ_OBJECT_VECTOR(_vec_nic_info);
        DESERIALIZE_END();
    };
};


// vmagt向CC发送的状态信息
// EV_VM_INFO_REPORT
class VmInfo : public Serializable
{
public:
    VmInfo():
        _vm_statistics()
    {
        _vm_id = INVALID_VID;
        _vm_dom_id = -1;
        _vm_vcpu_num = 1;
        _vm_cpu_usage = 0;
        _vm_mem_max = 0;
        _vm_mem_free = 0;
        _vm_net_tx = 0;
        _vm_net_rx = 0;
    };

    // 该ID和_op_info._vid重复，之所以重复，是因为存在虚拟机没有管理结构
    // 扫描出来的ID，只能放这个地方
    int64           _vm_id;                 // 虚拟机ID,
    string          _vm_name;               // 虚拟机名称
    int32           _vm_dom_id;             // 虚拟机DomId
    int32           _vm_vcpu_num;           // 虚拟机的vcpu个数
    int32           _vm_cpu_usage;          // 虚拟机的CPU占用率
    int64           _vm_mem_max;            // 虚拟机的内存最大值
    int64           _vm_mem_free;           // 虚拟机的空闲内存大小
    int64           _vm_net_tx;             // 虚拟机的网络发送流量
    int64           _vm_net_rx;             // 虚拟机的网络接收流量
    int32           _run_version;           // 虚拟机运行版本
    int32           _vm_state;              // 虚拟机状态
    ResourceStatistics _vm_statistics;      // 虚拟机资源统计信息
   
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(VmInfo);
        WRITE_VALUE(_vm_id);
        WRITE_VALUE(_vm_name);
        WRITE_VALUE(_vm_dom_id);
        WRITE_VALUE(_vm_vcpu_num);
        WRITE_VALUE(_vm_cpu_usage);
        WRITE_VALUE(_vm_mem_max);
        WRITE_VALUE(_vm_mem_free);
        WRITE_VALUE(_vm_net_tx);
        WRITE_VALUE(_vm_net_rx); 
        WRITE_VALUE(_run_version); 
        WRITE_VALUE(_vm_state);
        WRITE_OBJECT(_vm_statistics);
        SERIALIZE_END();
    };
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VmInfo);
        READ_VALUE(_vm_id);
        READ_VALUE(_vm_name);
        READ_VALUE(_vm_dom_id);
        READ_VALUE(_vm_vcpu_num);
        READ_VALUE(_vm_cpu_usage);
        READ_VALUE(_vm_mem_max);
        READ_VALUE(_vm_mem_free);
        READ_VALUE(_vm_net_tx);
        READ_VALUE(_vm_net_rx);
        READ_VALUE(_run_version); 
        READ_VALUE(_vm_state);
        READ_OBJECT(_vm_statistics);
        DESERIALIZE_END();
    };
};

// VmAgt向CC发送的虚拟机状态消息
class MessageVmInfoReport : public Serializable
{
public:
    vector<VmInfo>      _vm_info;           // 虚拟机的信息

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageVmInfoReport);
        WRITE_OBJECT_VECTOR(_vm_info);
        SERIALIZE_END();
    };
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageVmInfoReport);
        READ_OBJECT_VECTOR(_vm_info);     
        DESERIALIZE_END();
    };
};

class VmDiskConfig: public Serializable
{
public:
    //虚拟机磁盘要求存放的位置，宏定义的值必须和api中的要求符合!!!
    #define VM_STORAGE_TO_ANY     0
    #define VM_STORAGE_TO_LOCAL   1
    #define VM_STORAGE_TO_SHARE   2

    VmDiskConfig()
    {
        _id   = INVALID_FILEID;
        _position = VM_STORAGE_TO_ANY;
        _size = 0;
        _is_need_release = true;
        _disk_size = 0;
        _reserved_backup = 0;
    };
        
    VmDiskConfig(IMAGE_ID id,
                 int position,
                 int64  size,
                 const string& type,
                 const string& bus,
                 const string& target,
                 const string& source,
                 const string& driver,
                 const string& fstype):
                 _source(source),
                 _type(type),
                 _bus(bus),
                 _target(target),
                 _driver(driver),
                 _fstype(fstype)
    {
          _id    = id;
        _position = position;
         _size   = size;
         _is_need_release = true;
         _disk_size = size;
    };


    VmDiskConfig(IMAGE_ID id,
                 int position,
                 int64  size,
                 const string& type,
                 const string& bus,
                 const string& target,
                 const string& source,
                 const string& driver,
                 const string& fstype,
                 FileUrl     &url,
                 FileUrl     &share_url):
                 _source(source),
                 _type(type),
                 _bus(bus),
                 _target(target),
                 _driver(driver),
                 _fstype(fstype),
                 _url(url),
                 _share_url(share_url)
                 
    {
          _id    = id;
        _position = position;
         _size   = size;
         _is_need_release = true;
         _disk_size = size;
    };

    VmDiskConfig(IMAGE_ID id,
                 int position,
                 int64  size,
                 const string& type,
                 const string& bus,
                 const string& target,
                 FileUrl     &url,
                 FileUrl     &share_url,
                 const string& disk_format,
                 int64 disk_size):
                 _type(type),
                 _bus(bus),
                 _target(target),
                 _url(url),
                 _share_url(share_url),
                 _disk_format(disk_format),
                 _disk_size(disk_size)
                 
    {
        _id    = id;
        _position = position;
        _size   = size;
        _is_need_release = true;
    };


    ~VmDiskConfig() {};
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(VmDiskConfig);
        WRITE_DIGIT(_id);
        WRITE_VALUE(_position);
        WRITE_DIGIT(_size);
        WRITE_STRING(_source);
        WRITE_STRING(_source_type);
        WRITE_STRING(_type);
        WRITE_STRING(_bus);
        WRITE_STRING(_target);
        WRITE_STRING(_driver);
        WRITE_STRING(_fstype);
        WRITE_OBJECT(_url);
        WRITE_OBJECT(_share_url);
        WRITE_STRING(_request_id);
        WRITE_DIGIT(_is_need_release);
        WRITE_STRING(_disk_format);
        WRITE_DIGIT(_disk_size);
        WRITE_DIGIT(_reserved_backup);
        WRITE_DIGIT(_os_arch);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VmDiskConfig);
        READ_DIGIT(_id);
        READ_VALUE(_position);
        READ_DIGIT(_size);
        READ_STRING(_source);
        READ_STRING(_source_type);
        READ_STRING(_type);
        READ_STRING(_bus);
        READ_STRING(_target);
        READ_STRING(_driver);
        READ_STRING(_fstype);
        READ_OBJECT(_url);
        READ_OBJECT(_share_url);
        READ_STRING(_request_id);
        READ_DIGIT(_is_need_release);
         READ_STRING(_disk_format);
        READ_DIGIT(_disk_size);
        READ_DIGIT(_reserved_backup);
        READ_DIGIT(_os_arch);
        DESERIALIZE_END();
    };

    /**
    @brief 参数描述: 磁盘文件id，如果是INVALID_FILEID，表示由HC本地分配LVM虚拟磁盘;否则就是用户自行定制上传的磁盘映像文件
    @li @b 是否用户必填: 对自行定制上传的映像文件，需要用户填写上传后分配的文件编号
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: INVALID_FILEID
    @li @b 其它说明: 
    */
    IMAGE_ID _id;    //在文件存储系统中的id，如果是空，表示由HC本地分配
    
    /**
    @brief 参数描述: 磁盘所在的存储介质位置
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: true
    @li @b 其它说明: 等于0表示由cc来指定共享还是本地，否则由用户指定
    */
    int   _position;

    /**
    @brief 参数描述: 磁盘文件大小
    @li @b 是否用户必填: 是
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 0, 如果类为DISK_TYPE_DISK，则必须 >= 0
    @li @b 其它说明: 
    */    
    int64   _size;

    /**
    @brief 参数描述: 该磁盘文件在dom0中的路径
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明: 
    */
    string   _source;

    /**
    @brief 参数描述: HC上存储获取的源类型
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明: 如果是LVM，则为block，否则为file。
    */    
    string   _source_type;   
    
    /**
    @brief 参数描述: 磁盘文件类型
    @li @b 是否用户必填: 是
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明: 
    */    
    string   _type;    //(floppy,disk,cdrom,swap ...)

    /**
    @brief 参数描述: domU看到的磁盘总线类型
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: ide
    @li @b 其它说明: 取值为ide或者scsi。
    */    
    string   _bus;     //(ide,scsi)

    /**
    @brief 参数描述: domU看到的磁盘名
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: hda
    @li @b 其它说明: 取值为hda/hdb...,当用户配置多个disk时，该target需要依次累加，即从hda，hdb依次类推。注意libvirt中hdc为cdrom保留。
    */     
    string   _target;  //(映射到哪个设备)

    /**
    @brief 参数描述: 虚拟磁盘驱动类型
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明: 目前在生成vm的libvirt配置文件时暂未用到
    */     
    string   _driver;  //(raw,qcow2,tap:aio,file ...)
    
    /**
    @brief 参数描述: 磁盘需要格式化成什么文件系统
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明: 目前只支持ext2,ext3,ext4，为空表示不需要格式化
    */     
    string   _fstype;
    
    /**
    @brief 参数描述: 映像文件所在位置
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明: FileUrl主要用于CC/HC向TC请求映像下载，
    */
    FileUrl  _url;  //(nfs,http ...) 
    /**
    @brief 参数描述: 共享块所在位置
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明: FileUrl主要用于CC/HC向TC请求映像下载，
    */
    FileUrl  _share_url;  //(nfs,http ...) 

    string  _request_id;
    bool    _is_need_release;

    /**
    @brief 参数描述: 镜像格式
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明: 目前支持raw和vhd
    */
    string  _disk_format;  

    /**
    @brief 参数描述: 存放镜像所需磁盘大小
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明: 无
    */
    int64  _disk_size;  

    /**
    @brief 参数描述: 需要为该镜像预留多少个备份空间
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 0
    @li @b 其它说明: 无
    */
    int32  _reserved_backup;  

    /**
    @brief 参数描述: 镜像系统架构，镜像盘必填，取值形如i386或x86_64
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明: 无
    */
    string _os_arch;

};

class VmAttachReq: public VmOperationReq
{
public:
    VmDiskConfig _diskcfg;
    
    VmAttachReq() {};
    VmAttachReq(const VmDiskConfig& diskcfg):_diskcfg(diskcfg) {};
    VmAttachReq(int64 vid,int operation): VmOperationReq(vid, operation) {};
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(VmAttachReq);   
        WRITE_OBJECT(_diskcfg);                    
        SERIALIZE_PARENT_END(VmOperationReq); 
    }
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VmAttachReq);  
        READ_OBJECT(_diskcfg);                    
        DESERIALIZE_PARENT_END(VmOperationReq);
    }
};

class VmImageBackupReq: public VmOperationReq
{
public:
    string _request_id;
    string _description;
    vector<string> _son_request_id;
    VmDiskConfig _disk;
    
    VmImageBackupReq () {};
    VmImageBackupReq (const string& request_id,const VmDiskConfig& disk):
_request_id(request_id),_disk (disk) {};
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(VmImageBackupReq);  
        WRITE_VALUE(_request_id); 
        WRITE_VALUE(_description); 
        WRITE_VALUE(_son_request_id);
        WRITE_OBJECT(_disk);                    
        SERIALIZE_PARENT_END(VmOperationReq); 
    }
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VmImageBackupReq);  
        READ_VALUE(_request_id);
        READ_VALUE(_description);
        READ_VALUE(_son_request_id);
        READ_OBJECT(_disk);                    
        DESERIALIZE_PARENT_END(VmOperationReq);
    }
};


/* 物理网卡的信息 */
class PhyNic
{
public:

    string  _name;
    string  _netplane;
    int     _freenum;
};
typedef vector<PhyNic> PHYNIC_VEC;

class NicNetplaneInfo
{
public:
    NicNetplaneInfo()    
    {
         _sriov_num = 0;
    };

    string  _nic_name;
    string  _netplane;
    int32   _sriov_num;
};


class VmNicConfig: public Serializable
{
    /*******************************************************************************
    * 1. public section
    *******************************************************************************/
public:
    VmNicConfig()    
    {
        _nic_index = 0;
        _sriov    = false;
        _loop     = false;
        _c_vlan   = -1;
    };

    VmNicConfig(int32    nic_index,
                     bool     sriov,
                     bool     loop,
                     const string &logic_network_id,
                     const string &model,
                     const string &vsi_profile_id) 
    {
         _nic_index= nic_index;
         _sriov    = sriov;
         _loop    = loop;
        _logic_network_id = logic_network_id;
        _model     = model;
         _vsi_profile_id = vsi_profile_id;
    };

       VmNicConfig(int32    nic_index,
                     bool     sriov,
                     bool     loop,
                     const string &logic_network_id,
                     const string &model,
                     const string &vsi_profile_id,
                     const string &pci_order,
                     const string &bridge_name,
                     const string &ip,
                     const string &netmask,
                     const string &gateway,
                     const string &mac,
                     int32    c_vlan) 
    {
         _nic_index= nic_index;
         _sriov    = sriov;
         _loop    = loop;
        _logic_network_id = logic_network_id;
        _model     = model;
         _vsi_profile_id = vsi_profile_id;
         _pci_order = pci_order;
         _bridge_name   = bridge_name;
         _ip       = ip;
         _netmask  = netmask;
         _gateway  = gateway;
         _mac      = mac;
         _c_vlan = c_vlan;
         
    };
    
    ~VmNicConfig(){};

    SERIALIZE
    {
        SERIALIZE_BEGIN(VmNicConfig);
        WRITE_VALUE(_nic_index);
        WRITE_VALUE(_sriov);
        WRITE_VALUE(_loop);
        WRITE_VALUE(_logic_network_id);
        WRITE_VALUE(_model);
        WRITE_VALUE(_vsi_profile_id);
        WRITE_VALUE(_pci_order);
        WRITE_VALUE(_bridge_name);
        WRITE_VALUE(_ip);
        WRITE_VALUE(_netmask);
        WRITE_VALUE(_gateway);
        WRITE_VALUE(_mac);
        WRITE_VALUE(_netmask);
        WRITE_VALUE(_gateway);
        WRITE_VALUE(_mac);
        WRITE_VALUE(_c_vlan);        
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VmNicConfig);
        READ_VALUE(_nic_index);
        READ_VALUE(_sriov);
        READ_VALUE(_loop);
        READ_VALUE(_logic_network_id);
        READ_VALUE(_model);
        READ_VALUE(_vsi_profile_id);
        READ_VALUE(_pci_order);
        READ_VALUE(_bridge_name);
        READ_VALUE(_ip);
        READ_VALUE(_netmask);
        READ_VALUE(_gateway);
        READ_VALUE(_mac);
        READ_VALUE(_netmask);
        READ_VALUE(_gateway);
        READ_VALUE(_mac);
        READ_VALUE(_c_vlan);        
        DESERIALIZE_END();
    };

    int32   _nic_index;   
    
    /**
    @brief 参数描述:     是否需要使用SR-IOV虚拟网卡；
    @li @b 是否用户必填:  需要使用SR-IOV时，此字段必须填写；如果不需要，写 0 ；
    @li @b 是否可配多个:  每个虚拟网卡只能配置一个
    @li @b 默认值（租户可不填的情况下）: 0
    @li @b 其它说明:     注意：配置了SR-IOV虚拟网卡，就不能再配置网桥。
    */
    bool     _sriov;

    // 是否环回设备
    bool    _loop;                        

    // 逻辑网络ID
    string  _logic_network_id;       

     /**
    @brief 参数描述:      网卡型号
    @li @b 是否用户必填:  必须填写
    @li @b 是否可配多个:  每个虚拟网卡只能配置一个
    @li @b 默认值（租户可不填的情况下）:
    @li @b 其它说明:      
    @li @b 其它说明:      1. SR-IOV网卡暂不支持网卡型号选择； 
                          2. 虚拟机为PVM时，非SR-IOV网卡不允许选择网卡型号，model为空；
                          3. 虚拟机为HVM时，非SR-IOV网卡允许选择网卡型号
                             a) 网卡要么全为"netfront"；
                             b) 要么全为非"netfront"，即"e1000"、"rtl8139"、"ne2k_pci"、"pcnet"
                                中的一种或组合。
    */
    string  _model;         

    //VSIProfile UUID,网卡的唯一标识
    string   _vsi_profile_id;    

    /**
    @brief 参数描述:     网卡进行SR-IOV时，虚拟出来的PCI总线顺序号；
    @li @b 是否用户必填:  需要使用SR-IOV时，此字段必须填写；如果不需要，写 0 ；
    @li @b 是否可配多个:  每个虚拟网卡只能配置一个
    @li @b 默认值（租户可不填的情况下）: 0
    @li @b 其它说明: 
    */
    string   _pci_order;   

    //网卡所在的网桥
    string   _bridge_name;

    //虚拟机IP
    string   _ip;

      /**
    @brief 参数描述:     虚拟网卡指定子网掩码
    @li @b 是否用户必填:  可以填写
    @li @b 是否可配多个:  每个虚拟网卡配置一个
    @li @b 默认值（租户可不填的情况下）: 默认值为0
    @li @b 其它说明:      
    */
    string   _netmask ;

    //网关地址
    string   _gateway;

    //mac地址
    string   _mac;

     /**
    @brief 参数描述:     虚拟网卡所属的VLAN
    @li @b 是否用户必填:  必须填写
    @li @b 是否可配多个:  每个虚拟网卡只能配置一个
    @li @b 默认值（租户可不填的情况下）: 默认值为0
    @li @b 其它说明:      这里的VLAN ID为用户自己配置的VLAN，实际系统中会进行转换。
    */
    int     _c_vlan;   

};

class VmDeviceConfig: public Serializable
{
public:
    VmDeviceConfig()
    {
        _type = 0;
        _count = 0;
    };
    
    VmDeviceConfig(uint32 type, uint32 count)
    {
        _type = type;
        _count = count;
    }

    VmDeviceConfig(const ApiVmDevice &dev)
    {
        _type = dev.type;
        _count = dev.count;
    }    
    SERIALIZE
    {
        SERIALIZE_BEGIN(VmDeviceConfig);
        WRITE_DIGIT(_type);
        WRITE_DIGIT(_count);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VmDeviceConfig);
        READ_DIGIT(_type);
        READ_DIGIT(_count);
        DESERIALIZE_END();
    };

//private:    
    //int64           _id;
    uint32          _type;
    uint32          _count;
};

class Expression: public Serializable
{
public:
    Expression()
    {};
    Expression(const string& key,const string& op, const string& value):
    _key(key),_op(op),_value(value)
    {};
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(Expression);
        WRITE_STRING(_key);
        WRITE_STRING(_op);
        WRITE_STRING(_value);
        SERIALIZE_END();
    }

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(Expression);
        READ_STRING(_key);
        READ_STRING(_op);
        READ_STRING(_value);
        DESERIALIZE_END();
    }

    string _key;
    string _op;
    string _value;
};


class VmHAPolicy: public Serializable
{
public:    
    SERIALIZE
    {
        SERIALIZE_BEGIN(VmHAPolicy);
        WRITE_STRING(vm_crash);
        WRITE_STRING(host_down);
        WRITE_STRING(host_fault);
        WRITE_STRING(host_eth_down);
        WRITE_STRING(host_storage_fault);
        SERIALIZE_END();
    }

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VmHAPolicy);
        READ_STRING(vm_crash);
        READ_STRING(host_down);
        READ_STRING(host_fault);
        READ_STRING(host_eth_down);
        READ_STRING(host_storage_fault);
        DESERIALIZE_END();
    }
    /**
    @brief 参数描述: 描述虚拟机故障情况下的策略
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明: 目前支持策略有 POLICY_MIGRATE，POLICY_STOP，POLICY_REBOOT，POLICY_PAUSE
    */
    string vm_crash;

    /**
    @brief 参数描述: 描述物理机离线情况下的策略
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明: 目前支持策略有 POLICY_MIGRATE，POLICY_NONE
    */
    string host_down;

    /**
    @brief 参数描述: 描述物理机故障情况下的策略
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明: 目前支持策略有 POLICY_MIGRATE，POLICY_NONE
    */
    string host_fault;

    /**
    @brief 参数描述: 描述物理机出现网口故障情况下的策略
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明: 目前支持策略有 POLICY_MIGRATE，POLICY_NONE
    */
    string host_eth_down;

    /**
    @brief 参数描述: 描述物理机出现本地存储故障情况下的策略
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明: 目前支持策略有 POLICY_MIGRATE，POLICY_NONE
    */
    string host_storage_fault;
};

/*****************************************************************************/
/* 虚拟机模板/虚拟机信息基类    */
/*****************************************************************************/
class VmBaseInfo : public Serializable
{
public:
    VmBaseInfo() 
    {
        _id      = -1;
        _uid     = -1;
        _vcpu    = 0;
        _tcu          =0;
        _cpu_percent =0;
        _memory     = 0;
        _watchdog   = 0;
        _machine._type = DISK_TYPE_MACHINE ;
        _config_version = 0;
        _enable_coredump = false;
        _enable_serial = false;
    };
    
    ~VmBaseInfo() {};

    SERIALIZE
    {
        SERIALIZE_BEGIN(VmBaseInfo);
        WRITE_VALUE(_id);
        WRITE_VALUE(_description);
        WRITE_VALUE(_uid);
        WRITE_VALUE(_user_name);
        WRITE_VALUE(_memory);
        WRITE_VALUE(_vcpu);
        WRITE_VALUE(_tcu);
        WRITE_OBJECT(_machine);
        WRITE_OBJECT(_kernel);
        WRITE_OBJECT(_ramdisk);        
        WRITE_VALUE(_disk_size);
        WRITE_VALUE(_root_device);
        WRITE_VALUE(_kernel_command);
        WRITE_VALUE(_bootloader);
        WRITE_VALUE(_virt_type);
        WRITE_VALUE(_rawdata);
        WRITE_VALUE(_vnc_passwd);
        WRITE_VALUE(_enable_coredump);
        WRITE_VALUE(_enable_serial);
        WRITE_VALUE(_appointed_cluster);
        WRITE_VALUE(_appointed_host);
        WRITE_VALUE(_watchdog);
        WRITE_VALUE(_config_version);
        WRITE_VALUE(_context);
        WRITE_OBJECT_VECTOR(_qualifications);
        WRITE_OBJECT_VECTOR(_disks);
        WRITE_OBJECT_VECTOR(_nics);
        WRITE_OBJECT_VECTOR(_devices);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VmBaseInfo);
        READ_VALUE(_id);
        READ_VALUE(_description);
        READ_VALUE(_uid);
        READ_VALUE(_user_name);
        READ_VALUE(_vcpu);
        READ_VALUE(_tcu);
        READ_VALUE(_memory);
        READ_OBJECT(_machine);
        READ_OBJECT(_kernel);
        READ_OBJECT(_ramdisk);
        READ_VALUE(_disk_size);
        READ_VALUE(_root_device);
        READ_VALUE(_kernel_command);
        READ_VALUE(_bootloader);
        READ_VALUE(_virt_type);
        READ_VALUE(_rawdata);
        READ_VALUE(_vnc_passwd);
        READ_VALUE(_enable_coredump);
        READ_VALUE(_enable_serial);
        READ_VALUE(_appointed_cluster);
        READ_VALUE(_appointed_host);
        READ_VALUE(_watchdog);
        READ_VALUE(_config_version);
        READ_VALUE(_context);
        READ_OBJECT_ARRAY(_qualifications);
        READ_OBJECT_ARRAY(_disks);
        READ_OBJECT_ARRAY(_nics);
        READ_OBJECT_ARRAY(_devices);
        DESERIALIZE_END();
    };
    
    int64     _id;
    string    _description;
    int64     _uid;
    string    _user_name;
    int       _vcpu; 
    int       _tcu;
    int       _cpu_percent;
    int64     _memory;
       
    VmDiskConfig  _machine;
    VmDiskConfig  _kernel;
    VmDiskConfig  _ramdisk;    
    int64     _disk_size;
    string    _root_device;
    string    _kernel_command;
    string    _bootloader;
    string    _virt_type;
    string    _rawdata;
    string    _vnc_passwd;
    bool      _enable_coredump;
    bool      _enable_serial;
    string    _appointed_cluster;
    string    _appointed_host;
    uint32    _watchdog;
    int32     _config_version;
    
    map<string,string>   _context;
    vector<Expression>   _qualifications;

    //Disk Section
    vector<VmDiskConfig> _disks;   //虚拟机磁盘列表

    //Network Section
    vector<VmNicConfig>  _nics;     //虚拟机网络接口列表

    vector<VmDeviceConfig> _devices;

};

class VmCfgInfo : public Serializable
{
public:
    VmCfgInfo()
   {
        _project_id = INVALID_OID;
        _vnc_port = 0;
        _serial_port = 0;
        _state = 0;
    };
   
    ~VmCfgInfo()
    {
    };

    SERIALIZE
    {
        SERIALIZE_BEGIN(VmCfgInfo);
        WRITE_OBJECT(_base_info);
        WRITE_VALUE(_vm_name);
        WRITE_VALUE(_deployed_cluster);
        WRITE_VALUE(_deployed_hc);
        WRITE_VALUE(_project_name);
        WRITE_VALUE(_project_id);
        WRITE_STRING(_vnc_ip);
        WRITE_VALUE(_vnc_port);
        WRITE_VALUE(_serial_port);
        WRITE_VALUE(_create_time);
        WRITE_VALUE(_state);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VmCfgInfo);
        READ_OBJECT(_base_info);
        READ_VALUE(_vm_name);
        READ_VALUE(_deployed_cluster);
        READ_VALUE(_deployed_hc);
        READ_VALUE(_project_name);
        READ_VALUE(_project_id);
        READ_STRING(_vnc_ip);
        READ_VALUE(_vnc_port);
        READ_VALUE(_serial_port);
        READ_VALUE(_create_time);
        READ_VALUE(_state);
        DESERIALIZE_END();
    };

    VmBaseInfo _base_info;
    //虚拟机独有的字段
    string    _vm_name;
    string    _deployed_cluster;
    string    _deployed_hc;
    
    string    _project_name; 
    int64     _project_id;
    string    _create_time;
    int       _state;

    string    _vnc_ip;
    uint16    _vnc_port;    
    uint16    _serial_port;    
    
};


class VmTemplateInfo : public Serializable
{
public:
    VmTemplateInfo()
    {

    };
    
    ~VmTemplateInfo() {};
    SERIALIZE
    {
        SERIALIZE_BEGIN(VmTemplateInfo);
        WRITE_OBJECT(_base_info);
        WRITE_STRING(_vt_name);
        WRITE_STRING(_create_time);
        WRITE_DIGIT(_is_public);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VmTemplateInfo);
        READ_OBJECT(_base_info);
        READ_STRING(_vt_name);
        READ_STRING(_create_time);
        READ_DIGIT(_is_public);
        DESERIALIZE_END();
    };
    
    //模板独有的字段
    VmBaseInfo _base_info;
    string    _vt_name;
    string    _create_time;
    bool      _is_public;
};

class VMConfig: public Serializable
{
    /*******************************************************************************
    * 1. public section
    *******************************************************************************/
public:
    VMConfig()   
    {
        _vid = INVALID_OID;          
        _memory = 0;       
        _cpu_percent = 0;   
        _vcpu = 0;     
        _tcu = 0;         
        _local_disk_size = 0;
        _share_disk_size = 0;
        _kernel._id = INVALID_OID;
        _initrd._id = INVALID_OID;
        _machine._id = INVALID_OID;    
        _virt_type = VIRT_TYPE_HVM;
        _hypervisor = HYPERVISOR_XEN;
        _vnc_passwd = "";
        _vnc_port = 0;
        _serial_port = 0;
        _vm_wdtime = 0;
        _config_version = 0;
        _enable_serial = false;
        _hypervisor = "xen";
        _enable_livemigrate = true;
    };
    ~VMConfig()    {};
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(VMConfig);
        WRITE_DIGIT(_vid);
        WRITE_STRING(_name);
        WRITE_DIGIT(_memory);
        WRITE_DIGIT(_vcpu);
        WRITE_DIGIT(_tcu);   
        WRITE_DIGIT(_cpu_percent);        
        WRITE_DIGIT(_local_disk_size);
        WRITE_DIGIT(_share_disk_size);
        WRITE_OBJECT(_kernel);
        WRITE_OBJECT(_initrd);
        WRITE_OBJECT(_machine);
        WRITE_STRING(_root_device);
        WRITE_STRING(_kernel_command);
        WRITE_STRING(_bootloader);
        WRITE_OBJECT_VECTOR(_disks);
        WRITE_OBJECT_VECTOR(_nics);
        WRITE_STRING_MAP(_context);
        WRITE_STRING(_virt_type);
        WRITE_STRING(_rawdata);
        WRITE_STRING(_vnc_passwd);
        WRITE_DIGIT(_vnc_port); 
        WRITE_DIGIT(_serial_port);  
        WRITE_DIGIT(_enable_serial);
        WRITE_OBJECT(_core_dump_url);
        WRITE_OBJECT_VECTOR(_usb);
        WRITE_DIGIT(_vm_wdtime);
        WRITE_DIGIT(_config_version);
        WRITE_OBJECT_VECTOR(_devices);
        WRITE_VALUE(_hypervisor);
        WRITE_DIGIT(_enable_livemigrate);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VMConfig);
        READ_DIGIT(_vid);
        READ_STRING(_name);
        READ_DIGIT(_memory);
        READ_DIGIT(_vcpu);
        READ_DIGIT(_tcu);   
        READ_DIGIT(_cpu_percent);
        READ_DIGIT(_local_disk_size);
        READ_DIGIT(_share_disk_size);
        READ_OBJECT(_kernel);
        READ_OBJECT(_initrd);
        READ_OBJECT(_machine);
        READ_STRING(_root_device);
        READ_STRING(_kernel_command);
        READ_STRING(_bootloader);
        READ_OBJECT_VECTOR(_disks);
        READ_OBJECT_VECTOR(_nics);
        READ_STRING_MAP(_context);
        READ_STRING(_virt_type);
        READ_STRING(_rawdata);
        READ_STRING(_vnc_passwd);
        READ_DIGIT(_vnc_port);
        READ_DIGIT(_serial_port);
        READ_DIGIT(_enable_serial);
        READ_OBJECT(_core_dump_url);
        READ_OBJECT_VECTOR(_usb);
        READ_DIGIT(_vm_wdtime);
        READ_DIGIT(_config_version);
        READ_OBJECT_VECTOR(_devices);
        READ_VALUE(_hypervisor);
        READ_DIGIT(_enable_livemigrate);
        DESERIALIZE_END();
    };

    //Capacity Section
    /**
    @brief 参数描述: 虚拟机唯一id，在TC上生成
    @li @b 是否用户必填: 是
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明: 无
    */
    int64 _vid;
    /**
    @brief 参数描述: 虚拟机唯一id，在TC上生成
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明: 如果为空，名字格式为vid加某个特定字符串（如vm、instance等，统一确定即可）前缀的组合
    */
    string _name;
    /**
    @brief 参数描述: 虚拟机内存大小
    @li @b 是否用户必填: 是
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明: 
    */
    int64 _memory;
    /**
    @brief 参数描述: Percentage of CPU
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 100
    @li @b 其它说明: 参考来自opennebula的概念，50表示50%
    */
    uint32 _cpu_percent;
    /**
    @brief 参数描述: 虚拟机vcpu数量
    @li @b 是否用户必填: 是
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 否
    @li @b 其它说明: 
    */
    uint32 _vcpu;
    uint32 _tcu;
    
    int64  _local_disk_size;
    int64  _share_disk_size;

    //OS and Boot Options Section
    VmDiskConfig  _kernel;
    VmDiskConfig  _initrd;
    VmDiskConfig  _machine;     //如果machine是有效值，则优先采用machine
    string _root_device;
    string _kernel_command;
    string _bootloader;

    //Disk Section
    vector<VmDiskConfig> _disks;   //虚拟机磁盘列表

    //Network Section
    vector<VmNicConfig> _nics;     //虚拟机网络接口列表

    //看门狗专用网口HC用来填写,不序列化
    VmNicConfig              _wdg_nics; 

    //Context Section
    map<string,string> _context;   //用户提供的上下文信息，通过iso传递

    //RAW Section
    /**
    @brief 参数描述: 虚拟化类型
    @li @b 是否用户必填: 是
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明: 只能是"pvm"或者"hvm"
    */
    string _virt_type;   // (pvm,hvm)
    /**
    @brief 参数描述: 直接传递给hypervisor的原始配置项
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明: 
    */
    string _rawdata;     //

    /**
    @brief 参数描述: 用户配置的访问vnc的密码
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明:
    */    
    string  _vnc_passwd;

    /**
    @brief 参数描述: 描述用户配置的虚拟机支持coredump,存放的URL地址
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明:
    */    
    UploadUrl  _core_dump_url;

    /**
    @brief 参数描述: 描述用户配置的虚拟机使用USB的信息
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 是
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明:
    */    
    vector<VmUsbInfo> _usb;

    uint16  _vnc_port;    

    /**
    @brief 参数描述: 用户配置的访问串口号
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明:
    */    

    uint16  _serial_port;    

    /**
    @brief 参数描述: 看门狗时长
    @li @b 是否用户必填: 是
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 0, 不启用看门狗功能
    @li @b 其它说明:
    */    

    bool _enable_serial;
    uint32  _vm_wdtime;

    /**
    @brief 参数描述: 配置数据版本号
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 0
    @li @b 其它说明:tc自动生成
    */    
    int32   _config_version;

    //Device Section
    vector<VmDeviceConfig> _devices;   //虚拟机特殊设备列?

    // 通过特殊设置转来的PCI总线设备信息，HC
    // 解析来的，不需要CC传递，不需要虚拟化
    vector<PciBus>               _pci_devices;

    string                       _hypervisor;

    bool  _enable_livemigrate;   //是否允许热迁移

};

class VmRequirement: public Serializable
{
    /*******************************************************************************
    * 1. public section
    *******************************************************************************/
public:

    VmRequirement()
    {
        _affinity_region_id = INVALID_OID;
        _affinity_region_level = AFFINITY_REGION_SHELF;
        _affinity_region_forced = false;

    };
    ~VmRequirement()    {};

    SERIALIZE
    {
        SERIALIZE_BEGIN(VmRequirement);
        WRITE_OBJECT_VECTOR(_qualifications);
        WRITE_DIGIT_VECTOR(_repel_vms);
        WRITE_DIGIT(_affinity_region_id);
        WRITE_STRING(_affinity_region_level);
        WRITE_DIGIT(_affinity_region_forced);
        SERIALIZE_END();
    }

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VmRequirement);
        READ_OBJECT_VECTOR(_qualifications);
        READ_DIGIT_VECTOR(_repel_vms);
        READ_DIGIT(_affinity_region_id);
        READ_STRING(_affinity_region_level);
        READ_DIGIT(_affinity_region_forced);
        DESERIALIZE_END();
    }

    /**
    @brief 参数描述: 物理机应该满足的表达式条件
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 是
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明: 无
    */
    vector<Expression> _qualifications;
    /**
    @brief 参数描述: 与哪些虚拟机互斥，即不能共存于同一host
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 是
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明: 无
    */
    vector<int64> _repel_vms;
    /**
    @brief 参数描述: 与哪些虚拟机依赖，即必须共存于同一host
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 是
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明: 无
    */
    //vector<int64> _attract_vms;


    /**
    @brief 参数描述: 虚拟机归属亲和域id
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明: 无
    */
    int64  _affinity_region_id;

     /**
    @brief 参数描述: 虚拟机归属亲和域的级别
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明: 只能是"shelf","rack"或"board"中一种
    */
    string  _affinity_region_level;

    /**
    @brief 参数描述: 虚拟机部署时是否必须满足归属其指定的亲和域
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明:
    */
    bool  _affinity_region_forced;

};

class VMDeployInfo: public WorkReq
{
    /*******************************************************************************
    * 1. public section
    *******************************************************************************/
public:
    VMDeployInfo(const string &id_of_action):WorkReq(id_of_action){};
    VMDeployInfo()    
    {
        _uid = INVALID_OID;
        _project_id = INVALID_OID;
    };
    VMDeployInfo(const string& stamp,
                const VMConfig& config,
                const VmRequirement& requirement,
                int64 uid,
                int64 project_id):
            _stamp(stamp),
            _config(config),
            _requirement(requirement)
    {
        _uid = uid;
        _project_id = project_id;
    };

    ~VMDeployInfo()
    {};

    SERIALIZE
    {
        SERIALIZE_BEGIN(VMDeployInfo);
        WRITE_STRING(_stamp);
        WRITE_OBJECT(_config);
        WRITE_OBJECT(_requirement);
        WRITE_DIGIT(_uid);
        WRITE_DIGIT(_project_id);
        WRITE_VALUE(_host);
        WRITE_VALUE(_user);
        SERIALIZE_PARENT_END(WorkReq);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VMDeployInfo);
        READ_STRING(_stamp);
        READ_OBJECT(_config);
        READ_OBJECT(_requirement);
        READ_DIGIT(_uid);
        READ_DIGIT(_project_id);
        READ_VALUE(_host);
        READ_VALUE(_user);
        DESERIALIZE_PARENT_END(WorkReq);
    };


    string          _stamp;
    VMConfig        _config;
    VmRequirement   _requirement;
    /**
    @brief 参数描述: 虚拟机所属用户id
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明: 由TC负责填写
    */
    int64           _uid;
    /**
    @brief 参数描述: 虚拟机所属project id
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明: 由TC负责填写
    */
    int64           _project_id;

    /**
    @brief 参数描述: 虚拟机所属host名称
    @li @b 是否用户必填: 否，只有管理员有权指定
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 空字符串
    @li @b 其它说明: 如果用户指定了主机名，还必须指定所属的集群
    */
    string           _host;
    string           _user;
};

class VMOption:public Serializable
{
public:
    ImageStoreOption _image_store_option; 
    bool            _is_tcu_share;

    // 默认TCU share标志是FALSE
    VMOption()
    {
        _is_tcu_share = false;
    }
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(VMOption);   
        WRITE_OBJECT(_image_store_option); 
        WRITE_VALUE(_is_tcu_share);
        SERIALIZE_END(); 
    }
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VMOption);  
        READ_OBJECT(_image_store_option); 
        READ_VALUE(_is_tcu_share); 
        DESERIALIZE_END();
    }
};

//虚拟机创建请求，用于CC->HC
class VmCreateReq: public VmOperationReq
{
public:
    VMConfig _config;
    VMOption _option;
    VmCreateReq() {};
    VmCreateReq(const VMConfig& config):_config(config) 
    {
        _vid = config._vid;
        
    };
    VmCreateReq(int64 vid,int operation): VmOperationReq(vid, operation) {};
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(VmCreateReq);   
        WRITE_OBJECT(_config);            
        WRITE_OBJECT(_option);
        SERIALIZE_PARENT_END(VmOperationReq); 
    }
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VmCreateReq);  
        READ_OBJECT(_config);             
        READ_OBJECT(_option);
        DESERIALIZE_PARENT_END(VmOperationReq);
    }
};

//虚拟机一键恢复镜像，用于CC->HC
class VmRecoverImageReq: public VmOperationReq
{
public:
    VMConfig _config;
    VMOption _option;
    VmRecoverImageReq() {};
    VmRecoverImageReq(const VMConfig& config):_config(config) 
    {
        _vid = config._vid;
        
    };
    VmRecoverImageReq(int64 vid,int operation): VmOperationReq(vid, operation) {};
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(VmRecoverImageReq);   
        WRITE_OBJECT(_config);            
        WRITE_OBJECT(_option);
        SERIALIZE_PARENT_END(VmOperationReq); 
    }
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VmRecoverImageReq);  
        READ_OBJECT(_config);             
        READ_OBJECT(_option);
        DESERIALIZE_PARENT_END(VmOperationReq);
    }
};

//虚拟机保存映像请求,TC->cc,cc->hc
class VmSaveReq: public VmOperationReq
{
public:
    UploadUrl _url;
    string       _name;
    VmSaveReq() {};
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(VmSaveReq);   
        WRITE_OBJECT(_url);            
        WRITE_VALUE(_name);
        SERIALIZE_PARENT_END(VmOperationReq); 
    }
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VmSaveReq);  
        READ_OBJECT(_url);             
        READ_VALUE(_name);
        DESERIALIZE_PARENT_END(VmOperationReq);
    }
};


//REBOOT之后RESET的消息结构
class VmRRReq: public VmOperationReq
{
public:
    int32 _delay_time;
    VmRRReq() {};
    VmRRReq(int64 vid, int op, int delay) 
    {
        _vid        = vid;
        _operation  = op;
        _delay_time = delay;
    };
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(VmRRReq);   
        WRITE_VALUE(_delay_time);            
        SERIALIZE_PARENT_END(VmOperationReq); 
    }
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VmRRReq);  
        READ_VALUE(_delay_time);             
        DESERIALIZE_PARENT_END(VmOperationReq);
    }
};



// 虚拟机配置请求的结果
class VmHandlerCfgAckInfo: public Serializable
{
public:
    vector<VmCreateReq>    _create_info;

    SERIALIZE
    {
        SERIALIZE_BEGIN(VmHandlerCfgAckInfo);   
        WRITE_OBJECT_ARRAY(_create_info);            
        SERIALIZE_END(); 
    }
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VmHandlerCfgAckInfo);  
        READ_OBJECT_ARRAY(_create_info);             
        DESERIALIZE_END();
    }
    
};


//虚拟机迁移创建请求，用于CC->HC
class VmMigrateData: public VmCreateReq
{
public:
    string  _src_ssh_public_key;    /* 源端公钥 */

    string  _targe_ip;              /* 目的端的IP */
    
    VmMigrateData() {};
    VmMigrateData(const VMConfig& config, const string &key, const string &ip):VmCreateReq(config), _src_ssh_public_key(key), _targe_ip(ip){};
    VmMigrateData(const int64 vid, int operation, const string &key): VmCreateReq(vid, operation), _src_ssh_public_key(key) {}; 

    SERIALIZE
    {
        SERIALIZE_BEGIN(VmMigrateData);   
        WRITE_STRING(_src_ssh_public_key);            
        WRITE_STRING(_targe_ip);            
        SERIALIZE_PARENT_END(VmCreateReq); 
    }

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VmMigrateData);  
        READ_STRING(_src_ssh_public_key);             
        READ_STRING(_targe_ip);             
        DESERIALIZE_PARENT_END(VmCreateReq);
    }
};

//虚拟机查询请求，用于TC->CC
class VmQueryReq: public Serializable
{
public:
    vector<int64>  _vids;
    string _stamp;

    VmQueryReq()
    {
    };

    VmQueryReq(const string  &stamp, vector<int64>  &vids)
    {
        _stamp = stamp;
        _vids  = vids;
    };
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(VmQueryReq);  
        WRITE_VALUE(_vids);
        WRITE_VALUE(_stamp);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VmQueryReq);
        READ_VALUE(_vids);
        READ_VALUE(_stamp);
        DESERIALIZE_END();
    };
};


//虚拟机迁移请求，用于tc->cc
class VmMigrateReq: public VmOperationReq
{
public:
    //string _host_name;
    //string _host_ip;
    int64 _hid;
    bool    _bForced;
    vector<Expression> _qualifications;

    VmMigrateReq() 
    {
        _hid = -1;
        _bForced = false;
    };
    
    VmMigrateReq(int64 vid,int operation):VmOperationReq(vid,operation) 
    {
        _hid = -1;
        _bForced = false;
    };

    VmMigrateReq(int64 vid,
                       int64 hid,
                       bool  bForce,
                       int operation,
                       const string &stamp):VmOperationReq(vid,operation,stamp) 
    {
        _hid     = hid;
        _bForced = bForce;
    };
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(VmMigrateReq);   
        //WRITE_VALUE(_host_name); 
        //WRITE_VALUE(_host_ip); 
        WRITE_VALUE(_hid); 
        WRITE_VALUE(_bForced);
        WRITE_OBJECT_VECTOR(_qualifications);
        SERIALIZE_PARENT_END(VmOperationReq); 
    }
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VmMigrateReq);  
        //READ_VALUE(_host_name); 
        //READ_VALUE(_host_ip);     
        READ_VALUE(_hid);     
        READ_VALUE(_bForced);
        READ_OBJECT_VECTOR(_qualifications);
        DESERIALIZE_PARENT_END(VmOperationReq);
    }
};

class VmCfgModifyIndividualItem : public Serializable
{
public:
    VmCfgModifyIndividualItem(){};
    VmCfgModifyIndividualItem(const ApiVmCfgModifyIndividualItem &api_modify, int64 uid) : _args(api_modify.args)
    {
        _vid        = api_modify.vid;
        _element    = api_modify.element;
        _operation  = api_modify.operation;
        _uid        = uid;
    };
    ~VmCfgModifyIndividualItem(){};

    SERIALIZE
    {
        SERIALIZE_BEGIN(VmCfgModifyIndividualItem);
        WRITE_VALUE(_vid);
        WRITE_VALUE(_operation);
        WRITE_VALUE(_element);
        WRITE_VALUE(_uid);
        WRITE_VALUE(_args);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VmCfgModifyIndividualItem);
        READ_VALUE(_vid);
        READ_VALUE(_operation);
        READ_VALUE(_element);
        READ_VALUE(_uid);
        READ_VALUE(_args);
        DESERIALIZE_END();
    };

    string GetArgs()
    {
        boost::smatch mat;
        boost::regex reg( "\\s*<_args>[\\s\\S]*</_args>" );
        bool r=boost::regex_search(serialize() , mat, reg);
        if (r)
        {
            return mat[0];
        }
        else
        {
            return "";
        }
    }

    string AddUnderlinetoKeyword(const string &args)
    {
        return boost::regex_replace( args, (boost::regex)"(</?)(.+?)", "\\1_\\2");
    }

    string ReplaceArgwithClassName(const string &args, const string &class_name)
    {
        return boost::regex_replace( args, (boost::regex)"__args", class_name);
    }

    template<typename T>
    string GetClassName(const T &config)
    {
        boost::smatch mat;
        //boost::regex reg( "(?<=<).+?(?=>)" );
        boost::regex reg( "(\\w+)" );
        string tmp = config.serialize();
        bool r=boost::regex_search(tmp, mat, reg);
        if (r)
        {
            return mat[0];
        }
        else
        {
            SkyAssert(false);
            return "";
        }
    }

    template<typename T>
    bool AnalysisArgsto(T &config)
    {
        string args = GetArgs();

        args = AddUnderlinetoKeyword(args);

        string class_name = GetClassName(config);

        args = ReplaceArgwithClassName(args, class_name);

        bool r = config.deserialize(args);        
        SkyAssert(r);
        return r;
    }

    int64               _vid;
    int                 _operation;
    int                 _element;
    int64               _uid;
    map<string, string> _args;
};

class VmCfgBatchModify : public Serializable
{
public:
    VmCfgBatchModify(){};
    VmCfgBatchModify(const ApiVmCfgBatchModify &api_elements, int64 uid)
    {
        vector<ApiVmCfgModifyIndividualItem>::const_iterator it;
        for (it = api_elements.modifications.begin(); it != api_elements.modifications.end(); it ++)
        {
            _modifications.push_back(VmCfgModifyIndividualItem(*it, uid));
        }
    };

    ~VmCfgBatchModify(){};

    SERIALIZE
    {
        SERIALIZE_BEGIN(VmCfgBatchModify);
        WRITE_OBJECT_ARRAY(_modifications);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VmCfgBatchModify);
        READ_OBJECT_ARRAY(_modifications);
        DESERIALIZE_END();
    };

    vector<VmCfgModifyIndividualItem>  _modifications;
};

class VmModifyReq : public VmOperationReq
{
public :
    VmModifyReq() : VmOperationReq(){};
    VmModifyReq(const VmCfgModifyIndividualItem &modification) : VmOperationReq(modification._vid, MODIFY_VMCFG), _modification(modification)
    {
    }

    ~VmModifyReq(){};

    SERIALIZE
    {
        SERIALIZE_BEGIN(VmModifyReq);
        WRITE_OBJECT(_modification);
        SERIALIZE_PARENT_END(VmOperationReq);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VmModifyReq);
        READ_OBJECT(_modification);
        DESERIALIZE_PARENT_END(VmOperationReq);
    };

    VmCfgModifyIndividualItem         _modification;
};

class VmRunningInfo :public Serializable
{
public:
    int64    _vid;
    string   _hc_name;
    string   _vnc_out_ip;   /* 外部访问VNC的IP，也就是NAT的外网IP */
    uint16   _vnc_port;
    uint16   _serial_port;
    int      _state;
    uint32   _cpu_rate;
    uint32   _memory_rate;
    uint32   _net_tx;
    uint32   _net_rx;
    uint32   _nic_rate;
    STATUS   _result; //SUCCESS时，直接使用_state, 
                      //不存在时，返回用户的状态为CONFIG
                      // 其他情况 返回未知状态
    vector<VmNicConfig> _nicinfo; //虚拟机的mac和ip对应关系
                      
    VmRunningInfo()
    { 
        Init();
    };

    void Init()
    {
        _vid = INVALID_VID;
        _state = VM_UNKNOWN;
        _vnc_port = 0;
        _serial_port = 0;
        _cpu_rate = 0;
        _memory_rate = 0;
        _net_tx = 0;
        _net_rx = 0;
        _nic_rate = 0;
        _result = ERROR;
    }
       
    SERIALIZE
    {
        SERIALIZE_BEGIN(VmRunningInfo);  
        WRITE_VALUE(_vid);
        WRITE_VALUE(_hc_name);
        WRITE_VALUE(_vnc_out_ip);
        WRITE_VALUE(_vnc_port);
        WRITE_VALUE(_serial_port);
        WRITE_VALUE(_state);
        WRITE_VALUE(_cpu_rate);
        WRITE_VALUE(_memory_rate);
        WRITE_VALUE(_net_tx);
        WRITE_VALUE(_net_rx);
        WRITE_VALUE(_nic_rate);
        WRITE_VALUE(_result);
        WRITE_OBJECT_VECTOR(_nicinfo);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VmRunningInfo);
        READ_VALUE(_vid);
        READ_VALUE(_hc_name);
        READ_VALUE(_vnc_out_ip);
        READ_VALUE(_vnc_port);
        READ_VALUE(_serial_port);
        READ_VALUE(_state);
        READ_VALUE(_cpu_rate);
        READ_VALUE(_memory_rate);
        READ_VALUE(_net_tx);
        READ_VALUE(_net_rx);
        READ_VALUE(_nic_rate);
        READ_VALUE(_result);
        READ_OBJECT_VECTOR(_nicinfo);
        DESERIALIZE_END();
    };

    friend ostream & operator << (ostream& os, const VmRunningInfo& info)
    {
        cout << "vid = " << info._vid << endl;
        cout << "hc = " << info._hc_name << endl;
        cout << "VNC address = " << info._vnc_out_ip << ":" << info._vnc_port << endl;
        cout << "state = " << Enum2Str::InstStateStr(info._state) << endl;
        cout << "cpu_rate = " << info._cpu_rate << endl;
        cout << "memory_rate = " << info._memory_rate << endl;
        cout << "net_tx = " << info._net_tx << endl;
        cout << "net_rx = " << info._net_rx << endl;
        cout << "nic_rate = " << info._nic_rate << endl;
        cout << "result = " << info._result << endl;
        return os;
    }
};

//虚拟机查询应答，用于CC->TC
class VmQueryAck: public Serializable
{
public:    
    VmQueryAck() {};
    VmQueryAck(STATUS result,const string& error,const string& stamp)
    {
        _result = result;
        _error  = error;
        _stamp  = stamp;
    };
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(VmQueryAck);  
        WRITE_OBJECT_ARRAY(_running_info);
        WRITE_VALUE(_result);
        WRITE_VALUE(_error);
        WRITE_VALUE(_stamp);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VmQueryAck);
        READ_OBJECT_ARRAY(_running_info);
        READ_VALUE(_result);
        READ_VALUE(_error);
        READ_VALUE(_stamp);
        DESERIALIZE_END();
    };

    friend ostream & operator << (ostream& os, const VmQueryAck& ack)
    {
        cout << "ACK result = " << ack._result << endl;
        cout << "ACK error = " << ack._error << endl;
        cout << "ACK stamp = " << ack._stamp << endl;
        cout << "=======VM Running Information=======" << endl;
        vector<VmRunningInfo>::const_iterator it;
        for(it = ack._running_info.begin();it != ack._running_info.end(); it ++)
        {
            cout << *it << endl;
        }
        return os;
}

    string        _stamp;
    STATUS        _result; //此处不成功，则全部失败
    string        _error; 
    vector<VmRunningInfo> _running_info;     
};


//虚拟机查询应答，用于CC->TC
class VmSynQueryAck: public Serializable
{
public:    
    VmSynQueryAck() 
    {
        _vid = -1;
    };
    
    VmSynQueryAck(const string& stamp):_stamp(stamp)
    {
        _vid = -1;
    };
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(VmSynQueryAck);  
        WRITE_VALUE(_stamp);
        WRITE_VALUE(_vid);
        WRITE_OBJECT(_vm_running_info);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VmSynQueryAck);
        READ_VALUE(_stamp);
        READ_VALUE(_vid);
        READ_OBJECT(_vm_running_info);
        DESERIALIZE_END();
    };

    string        _stamp;
    int64         _vid;
    VmRunningInfo _vm_running_info;
         
};


class ClusterVmRunningInfo: public Serializable
{
public:
    bool                    _is_ack;
    vector<VmRunningInfo>   _vm_running_info;
    
    ClusterVmRunningInfo():_is_ack(false)
    {
    };

    ClusterVmRunningInfo(const ClusterVmRunningInfo &info):
           _is_ack(info._is_ack),
           _vm_running_info(info._vm_running_info)
    {
    };    
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(ClusterVmRunningInfo); 
        WRITE_VALUE(_is_ack);
        WRITE_OBJECT_VECTOR(_vm_running_info);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(ClusterVmRunningInfo);
        READ_VALUE(_is_ack);
        READ_OBJECT_VECTOR(_vm_running_info);
        DESERIALIZE_END();
    };
};
    

}
/* end namespace zte_tecs */

#endif /* end COMM_VMCFG_MANAGER_WITH_VM_MANAGER_H */



