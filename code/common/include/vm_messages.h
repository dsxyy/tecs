/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�vm_messages.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ������������Ϣ��ṹ����
* ��ǰ�汾��1.0
* ��    �ߣ�������
* ������ڣ�2011��9��5��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/9/5
*     �� �� �ţ�V1.0
*     �� �� �ˣ�������
*     �޸����ݣ�����
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
    EN_NAT_RULE_DEPLOY = 0, /* NAT�������� */
    EN_NAT_RULE_DELETE, /* NAT����ɾ��*/
}EN_NAT_RULE_OPTYPE;

typedef struct tagSNATRuleInfo
{
    string IpHC; /* HOST ��IP��HC��CCע���ʱ��ȷ��*/
    string IpNatin; /* NAT���ص�������ַ��VMM ��CC�������ļ��л��*/ 
    WORD32 Optype; /* ����EN_NAT_RULE_DEPLOY��ɾ��EN_NAT_RULE_DELETE*/
}T_SNATRuleInfo;
// added by vnet end


#define  DURATIONG_BETWEEN_CC_AND_HC  (2)//��λ S
#define  DURATIONG_BETWEEN_TC_AND_CC  (2)

// ��������ֲ����ȴ�ʱ��
//STAGING״̬��ʱʱ��
#define VM_STAGING_DURATION_HC    (2*60*60)  // 2Сʱ
//BOOTING״̬��ʱʱ��
#define  VM_BOOTING_DURATION_HC   (1*60)  // 1����
//�ͷų�ʱʱ��
#define  VM_TEARDOWN_DURATION_HC  (3*60)   // 3 ����
//��������
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


// ��������������Ƿ���Ҫִ��
#define OP_STAT_LAUNCH          0   /* ������װ���� */              //ִ������
#define OP_STAT_PROCESSING      1   /* ��������ִ�� */              //���ص�ǰ����
#define OP_STAT_CONFILICT       2   /* �����ͻ����ʱ����ִ�� */    //����������?

/* �����ʵ����״̬ */
/* �޸ı�enumʱ����ͬ���޸�vmcfg_api.h�е�ApiVmUserState״̬!!! */
enum VmInstanceState
{
    /* HC��ʼ״̬������CC�� */
    VM_INIT     = 0, 

    /* �����⼸��״̬��ֱ�ӳ��ָ��û��� */
    VM_RUNNING  = 2,
    VM_PAUSED   = 3,
    VM_SHUTOFF  = 4,
    VM_CRASHED  = 5,
    
    /* TC,CC�ϵ�״̬��HC�����ϱ����״̬,���Ҳ����ָ��û��� */
    VM_UNKNOWN  = 6,
    VM_FROZEN   = 8,

    /* TC�ϵ�״̬�����Ҳ����ָ��û��� */
    VM_CONFIGURATION = 7,
};


//�����cluster��������״̬
//ע��:�޸ı�enumʱ����ͬ���޸�Enum2Str::LcmState!!!
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

//��Щ״̬��Ӧ����λ��host�ϵ�?
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

//�������������
//ע��:�޸ı�enumʱ����ͬ���޸�Enum2Str::VmOp!!!
enum VmOperation
{
    //TC����
    SETUP   = 1,
    DELETE  = 2,
    DEPLOY  = 3,   
    CANCEL  = 4,   //���������

    //CC����
    QUERY    = 5,
    SCHEDULE = 6,
    RESCHEDULE = 7,
    APPLYRES = 8,

     //������HC�Ĳ���
    CREATE  = 9,
    START   = 10,   //����
    STOP    = 11,   //�ػ�
    REBOOT  = 12,
    PAUSE   = 13,
    RESUME  = 14,
    PREP_M  = 15,
    MIGRATE  = 16,
    RESET    = 17,   //ǿ������
    DESTROY  = 18,   //ǿ�ƹػ�
    POST_M   = 19,   // �����Ǩ�Ƴɹ���ִ�������������ʶǨ��ִ�гɹ������л�����Ķ���
    SYNC     = 20,
    RR       = 21,   // �����REBOOT֮�����RESET����ʱ�䣬��ʱʹ��
    UPLOAD_IMG     = 22,
    CANCEL_UPLOAD_IMG   = 23,
    PLUG_IN_USB = 24,
    PLUG_OUT_USB = 25,
    OUT_IN_USB = 26,
    PLUG_IN_PDISK = 27,
    PLUG_OUT_PDISK = 28,
    RECOVER_IMAGE = 29,
    MODIFY_IMAGE_BACKUP = 30,

    //CC����
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

//������ö��ֵ����Ϊ�ַ�����ʽ�����ڵ��Ժ�����ʹ��
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


// ����PCI�������͵��豸
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

    string          _domain;    //��
    string          _bus;       //����
    string          _slot;      //��λ
    string          _function;  //����
};


// �������ѯUSB��Ϣ
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

// ��ѯ������ܹ�������USB�豸Ӧ��
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

//�����������������TC->CC��CC->HC
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

//���������Ӧ������HC->CC��CC->TC
class VmOperationAck: public WorkAck
{
public:
    int64   _vid;
    int     _operation;
    int     _vm_state;        // �����״̬
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

    //���ack�Ļ����Ϸ���
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
    string          _name;      // ��������������
    int64           _speed;     // ��������������, ��λ�� B
    int64           _net_flow;  // ͳ�Ƽ��ʱ���ڵ�����, �շ���������, ��λ�� B
    int32           _used_rate; // ͳ�Ƽ��ʱ���ڵ�����ʹ����

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
    string _statistics_time; // HC ͳ�Ƶ�ʱ��
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


// vmagt��CC���͵�״̬��Ϣ
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

    // ��ID��_op_info._vid�ظ���֮�����ظ�������Ϊ���������û�й���ṹ
    // ɨ�������ID��ֻ�ܷ�����ط�
    int64           _vm_id;                 // �����ID,
    string          _vm_name;               // ���������
    int32           _vm_dom_id;             // �����DomId
    int32           _vm_vcpu_num;           // �������vcpu����
    int32           _vm_cpu_usage;          // �������CPUռ����
    int64           _vm_mem_max;            // ��������ڴ����ֵ
    int64           _vm_mem_free;           // ������Ŀ����ڴ��С
    int64           _vm_net_tx;             // ����������緢������
    int64           _vm_net_rx;             // ������������������
    int32           _run_version;           // ��������а汾
    int32           _vm_state;              // �����״̬
    ResourceStatistics _vm_statistics;      // �������Դͳ����Ϣ
   
    
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

// VmAgt��CC���͵������״̬��Ϣ
class MessageVmInfoReport : public Serializable
{
public:
    vector<VmInfo>      _vm_info;           // ���������Ϣ

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
    //���������Ҫ���ŵ�λ�ã��궨���ֵ�����api�е�Ҫ�����!!!
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
    @brief ��������: �����ļ�id�������INVALID_FILEID����ʾ��HC���ط���LVM�������;��������û����ж����ϴ��Ĵ���ӳ���ļ�
    @li @b �Ƿ��û�����: �����ж����ϴ���ӳ���ļ�����Ҫ�û���д�ϴ��������ļ����
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: INVALID_FILEID
    @li @b ����˵��: 
    */
    IMAGE_ID _id;    //���ļ��洢ϵͳ�е�id������ǿգ���ʾ��HC���ط���
    
    /**
    @brief ��������: �������ڵĴ洢����λ��
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: true
    @li @b ����˵��: ����0��ʾ��cc��ָ�������Ǳ��أ��������û�ָ��
    */
    int   _position;

    /**
    @brief ��������: �����ļ���С
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: 0, �����ΪDISK_TYPE_DISK������� >= 0
    @li @b ����˵��: 
    */    
    int64   _size;

    /**
    @brief ��������: �ô����ļ���dom0�е�·��
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��: 
    */
    string   _source;

    /**
    @brief ��������: HC�ϴ洢��ȡ��Դ����
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��: �����LVM����Ϊblock������Ϊfile��
    */    
    string   _source_type;   
    
    /**
    @brief ��������: �����ļ�����
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��: 
    */    
    string   _type;    //(floppy,disk,cdrom,swap ...)

    /**
    @brief ��������: domU�����Ĵ�����������
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ide
    @li @b ����˵��: ȡֵΪide����scsi��
    */    
    string   _bus;     //(ide,scsi)

    /**
    @brief ��������: domU�����Ĵ�����
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: hda
    @li @b ����˵��: ȡֵΪhda/hdb...,���û����ö��diskʱ����target��Ҫ�����ۼӣ�����hda��hdb�������ơ�ע��libvirt��hdcΪcdrom������
    */     
    string   _target;  //(ӳ�䵽�ĸ��豸)

    /**
    @brief ��������: ���������������
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��: Ŀǰ������vm��libvirt�����ļ�ʱ��δ�õ�
    */     
    string   _driver;  //(raw,qcow2,tap:aio,file ...)
    
    /**
    @brief ��������: ������Ҫ��ʽ����ʲô�ļ�ϵͳ
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��: Ŀǰֻ֧��ext2,ext3,ext4��Ϊ�ձ�ʾ����Ҫ��ʽ��
    */     
    string   _fstype;
    
    /**
    @brief ��������: ӳ���ļ�����λ��
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��: FileUrl��Ҫ����CC/HC��TC����ӳ�����أ�
    */
    FileUrl  _url;  //(nfs,http ...) 
    /**
    @brief ��������: ���������λ��
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��: FileUrl��Ҫ����CC/HC��TC����ӳ�����أ�
    */
    FileUrl  _share_url;  //(nfs,http ...) 

    string  _request_id;
    bool    _is_need_release;

    /**
    @brief ��������: �����ʽ
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��: Ŀǰ֧��raw��vhd
    */
    string  _disk_format;  

    /**
    @brief ��������: ��ž���������̴�С
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��: ��
    */
    int64  _disk_size;  

    /**
    @brief ��������: ��ҪΪ�þ���Ԥ�����ٸ����ݿռ�
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: 0
    @li @b ����˵��: ��
    */
    int32  _reserved_backup;  

    /**
    @brief ��������: ����ϵͳ�ܹ��������̱��ȡֵ����i386��x86_64
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��: ��
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


/* ������������Ϣ */
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
    @brief ��������:     �Ƿ���Ҫʹ��SR-IOV����������
    @li @b �Ƿ��û�����:  ��Ҫʹ��SR-IOVʱ�����ֶα�����д���������Ҫ��д 0 ��
    @li @b �Ƿ������:  ÿ����������ֻ������һ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: 0
    @li @b ����˵��:     ע�⣺������SR-IOV�����������Ͳ������������š�
    */
    bool     _sriov;

    // �Ƿ񻷻��豸
    bool    _loop;                        

    // �߼�����ID
    string  _logic_network_id;       

     /**
    @brief ��������:      �����ͺ�
    @li @b �Ƿ��û�����:  ������д
    @li @b �Ƿ������:  ÿ����������ֻ������һ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�:
    @li @b ����˵��:      
    @li @b ����˵��:      1. SR-IOV�����ݲ�֧�������ͺ�ѡ�� 
                          2. �����ΪPVMʱ����SR-IOV����������ѡ�������ͺţ�modelΪ�գ�
                          3. �����ΪHVMʱ����SR-IOV��������ѡ�������ͺ�
                             a) ����ҪôȫΪ"netfront"��
                             b) ҪôȫΪ��"netfront"����"e1000"��"rtl8139"��"ne2k_pci"��"pcnet"
                                �е�һ�ֻ���ϡ�
    */
    string  _model;         

    //VSIProfile UUID,������Ψһ��ʶ
    string   _vsi_profile_id;    

    /**
    @brief ��������:     ��������SR-IOVʱ�����������PCI����˳��ţ�
    @li @b �Ƿ��û�����:  ��Ҫʹ��SR-IOVʱ�����ֶα�����д���������Ҫ��д 0 ��
    @li @b �Ƿ������:  ÿ����������ֻ������һ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: 0
    @li @b ����˵��: 
    */
    string   _pci_order;   

    //�������ڵ�����
    string   _bridge_name;

    //�����IP
    string   _ip;

      /**
    @brief ��������:     ��������ָ����������
    @li @b �Ƿ��û�����:  ������д
    @li @b �Ƿ������:  ÿ��������������һ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: Ĭ��ֵΪ0
    @li @b ����˵��:      
    */
    string   _netmask ;

    //���ص�ַ
    string   _gateway;

    //mac��ַ
    string   _mac;

     /**
    @brief ��������:     ��������������VLAN
    @li @b �Ƿ��û�����:  ������д
    @li @b �Ƿ������:  ÿ����������ֻ������һ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: Ĭ��ֵΪ0
    @li @b ����˵��:      �����VLAN IDΪ�û��Լ����õ�VLAN��ʵ��ϵͳ�л����ת����
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
    @brief ��������: �����������������µĲ���
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��: Ŀǰ֧�ֲ����� POLICY_MIGRATE��POLICY_STOP��POLICY_REBOOT��POLICY_PAUSE
    */
    string vm_crash;

    /**
    @brief ��������: �����������������µĲ���
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��: Ŀǰ֧�ֲ����� POLICY_MIGRATE��POLICY_NONE
    */
    string host_down;

    /**
    @brief ��������: �����������������µĲ���
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��: Ŀǰ֧�ֲ����� POLICY_MIGRATE��POLICY_NONE
    */
    string host_fault;

    /**
    @brief ��������: ����������������ڹ�������µĲ���
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��: Ŀǰ֧�ֲ����� POLICY_MIGRATE��POLICY_NONE
    */
    string host_eth_down;

    /**
    @brief ��������: ������������ֱ��ش洢��������µĲ���
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��: Ŀǰ֧�ֲ����� POLICY_MIGRATE��POLICY_NONE
    */
    string host_storage_fault;
};

/*****************************************************************************/
/* �����ģ��/�������Ϣ����    */
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
    vector<VmDiskConfig> _disks;   //����������б�

    //Network Section
    vector<VmNicConfig>  _nics;     //���������ӿ��б�

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
    //��������е��ֶ�
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
    
    //ģ����е��ֶ�
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
    @brief ��������: �����Ψһid����TC������
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��: ��
    */
    int64 _vid;
    /**
    @brief ��������: �����Ψһid����TC������
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��: ���Ϊ�գ����ָ�ʽΪvid��ĳ���ض��ַ�������vm��instance�ȣ�ͳһȷ�����ɣ�ǰ׺�����
    */
    string _name;
    /**
    @brief ��������: ������ڴ��С
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��: 
    */
    int64 _memory;
    /**
    @brief ��������: Percentage of CPU
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: 100
    @li @b ����˵��: �ο�����opennebula�ĸ��50��ʾ50%
    */
    uint32 _cpu_percent;
    /**
    @brief ��������: �����vcpu����
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��: 
    */
    uint32 _vcpu;
    uint32 _tcu;
    
    int64  _local_disk_size;
    int64  _share_disk_size;

    //OS and Boot Options Section
    VmDiskConfig  _kernel;
    VmDiskConfig  _initrd;
    VmDiskConfig  _machine;     //���machine����Чֵ�������Ȳ���machine
    string _root_device;
    string _kernel_command;
    string _bootloader;

    //Disk Section
    vector<VmDiskConfig> _disks;   //����������б�

    //Network Section
    vector<VmNicConfig> _nics;     //���������ӿ��б�

    //���Ź�ר������HC������д,�����л�
    VmNicConfig              _wdg_nics; 

    //Context Section
    map<string,string> _context;   //�û��ṩ����������Ϣ��ͨ��iso����

    //RAW Section
    /**
    @brief ��������: ���⻯����
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��: ֻ����"pvm"����"hvm"
    */
    string _virt_type;   // (pvm,hvm)
    /**
    @brief ��������: ֱ�Ӵ��ݸ�hypervisor��ԭʼ������
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��: 
    */
    string _rawdata;     //

    /**
    @brief ��������: �û����õķ���vnc������
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��:
    */    
    string  _vnc_passwd;

    /**
    @brief ��������: �����û����õ������֧��coredump,��ŵ�URL��ַ
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��:
    */    
    UploadUrl  _core_dump_url;

    /**
    @brief ��������: �����û����õ������ʹ��USB����Ϣ
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��:
    */    
    vector<VmUsbInfo> _usb;

    uint16  _vnc_port;    

    /**
    @brief ��������: �û����õķ��ʴ��ں�
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��:
    */    

    uint16  _serial_port;    

    /**
    @brief ��������: ���Ź�ʱ��
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: 0, �����ÿ��Ź�����
    @li @b ����˵��:
    */    

    bool _enable_serial;
    uint32  _vm_wdtime;

    /**
    @brief ��������: �������ݰ汾��
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: 0
    @li @b ����˵��:tc�Զ�����
    */    
    int32   _config_version;

    //Device Section
    vector<VmDeviceConfig> _devices;   //����������豸��?

    // ͨ����������ת����PCI�����豸��Ϣ��HC
    // �������ģ�����ҪCC���ݣ�����Ҫ���⻯
    vector<PciBus>               _pci_devices;

    string                       _hypervisor;

    bool  _enable_livemigrate;   //�Ƿ�������Ǩ��

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
    @brief ��������: �����Ӧ������ı��ʽ����
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��: ��
    */
    vector<Expression> _qualifications;
    /**
    @brief ��������: ����Щ��������⣬�����ܹ�����ͬһhost
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��: ��
    */
    vector<int64> _repel_vms;
    /**
    @brief ��������: ����Щ����������������빲����ͬһhost
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��: ��
    */
    //vector<int64> _attract_vms;


    /**
    @brief ��������: ����������׺���id
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��: ��
    */
    int64  _affinity_region_id;

     /**
    @brief ��������: ����������׺���ļ���
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��: ֻ����"shelf","rack"��"board"��һ��
    */
    string  _affinity_region_level;

    /**
    @brief ��������: ���������ʱ�Ƿ�������������ָ�����׺���
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��:
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
    @brief ��������: ����������û�id
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��: ��TC������д
    */
    int64           _uid;
    /**
    @brief ��������: ���������project id
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��: ��TC������д
    */
    int64           _project_id;

    /**
    @brief ��������: ���������host����
    @li @b �Ƿ��û�����: ��ֻ�й���Ա��Ȩָ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ���ַ���
    @li @b ����˵��: ����û�ָ������������������ָ�������ļ�Ⱥ
    */
    string           _host;
    string           _user;
};

class VMOption:public Serializable
{
public:
    ImageStoreOption _image_store_option; 
    bool            _is_tcu_share;

    // Ĭ��TCU share��־��FALSE
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

//�����������������CC->HC
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

//�����һ���ָ���������CC->HC
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

//���������ӳ������,TC->cc,cc->hc
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


//REBOOT֮��RESET����Ϣ�ṹ
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



// �������������Ľ��
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


//�����Ǩ�ƴ�����������CC->HC
class VmMigrateData: public VmCreateReq
{
public:
    string  _src_ssh_public_key;    /* Դ�˹�Կ */

    string  _targe_ip;              /* Ŀ�Ķ˵�IP */
    
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

//�������ѯ��������TC->CC
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


//�����Ǩ����������tc->cc
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
    string   _vnc_out_ip;   /* �ⲿ����VNC��IP��Ҳ����NAT������IP */
    uint16   _vnc_port;
    uint16   _serial_port;
    int      _state;
    uint32   _cpu_rate;
    uint32   _memory_rate;
    uint32   _net_tx;
    uint32   _net_rx;
    uint32   _nic_rate;
    STATUS   _result; //SUCCESSʱ��ֱ��ʹ��_state, 
                      //������ʱ�������û���״̬ΪCONFIG
                      // ������� ����δ֪״̬
    vector<VmNicConfig> _nicinfo; //�������mac��ip��Ӧ��ϵ
                      
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

//�������ѯӦ������CC->TC
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
    STATUS        _result; //�˴����ɹ�����ȫ��ʧ��
    string        _error; 
    vector<VmRunningInfo> _running_info;     
};


//�������ѯӦ������CC->TC
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



