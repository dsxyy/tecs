#ifndef TECS_CC_VM_MODIFY_VMCFG_H
#define TECS_CC_VM_MODIFY_VMCFG_H

#include "event.h"
#include "host_pool.h"
#include "vm_pool.h"
#include "vm_interface.h"

namespace zte_tecs
{

class MessageFrameHandling
{
public:
    MessageFrameHandling(){}
    MessageFrameHandling(const MessageFrame &message, MessageUnit *receiver) :
                            _message(message)
    {
        _receiver = receiver;
    }
    virtual ~MessageFrameHandling(){};

    virtual void Handling()
    {
        cout << "Unknown message" << endl;
        cout << _message.data << endl;
    }

    static int GetDerivedKey(const MessageFrame &message)
    {
        return message.option.id();
    }

    const MessageFrame      _message;
    MessageUnit       *_receiver;
};

typedef   std::tr1::function<MessageFrameHandling* (const MessageFrame&, MessageUnit *)>   class_creator;

template<typename T>
MessageFrameHandling *CreaterTemplate(const MessageFrame &message, MessageUnit *receiver)
{
    return new T(message, receiver);
}

template<typename T1 = int, typename T2 = MessageFrameHandling>
class MsgFrmHandlCreaterBoundle
{
public:
    MessageFrameHandling *CreateHandling(const MessageFrame &message, MessageUnit *receiver)
    {
        typename map<T1, class_creator>::iterator it = _creater_boundle.find(T2::GetDerivedKey(message));;
        if (it != _creater_boundle.end())
        {
            return (it->second)(message, receiver);
        }
        else
        {
            return new T2(message, receiver);
        }
    }

    void Register(T1 key, class_creator creater)
    {
        _creater_boundle[key] = creater;
    }

private:
    map<T1, class_creator> _creater_boundle;
};

class AutoRegister
{
public:
    template<typename T1, typename T2>
    AutoRegister(MsgFrmHandlCreaterBoundle<T1, T2> &owner, T1 key, class_creator creater)
    {
        owner.Register(key, creater);
    }
};

#define RegisterDerivedCreater(owner, key, new_class) \
    static AutoRegister reg##owner##new_class(owner, key, CreaterTemplate<new_class>);

#define RegisterBaseCreater(owner, key, new_class) \
    inline MessageFrameHandling *create##new_class (const MessageFrame &message, MessageUnit *receiver) \
    { \
        return new_class.CreateHandling(message, receiver); \
    } \
    static AutoRegister reg##owner##new_class(owner, key, create##new_class);

class VmInterface;
class VmOpHandle : public MessageFrameHandling
{
public:
    VmOpHandle(const MessageFrame &message, MessageUnit *receiver) :
                MessageFrameHandling(message, receiver), _upstream_sender(message.option.sender()), VmAssistant(receiver)
    {
        _req.deserialize(message.data);
    }
    virtual ~VmOpHandle(){}

    static VmOperation GetDerivedKey(const MessageFrame &message)
    {
        VmOperationReq req;
        req.deserialize(message.data);

        return (VmOperation)req._operation;
    }

    virtual void Handling()
    {
        STATUS ret = Implement();

        VmOperationAck result = GenResult(ret);

        AcktoSender(result);
    }

    virtual STATUS Implement()
    {
        _op_info << "Unknown op" << endl;
        return ERROR;
    }

    VmOperationAck GenResult(STATUS state)
    {
        VmOperationAck ack(_req.action_id);
        ack._vid  = _req._vid;
        ack.state = state;

        if (state == SUCCESS)
        {
            ack.progress = 100;
        }
        else if (state == ERROR_ACTION_RUNNING)
        {
            //ack._workflow_id = FindModifyWorkflow(result._vid);
            //ack.progress = GetWorkflowProgress(result._workflow_id);
        }
        else
        {
            ack.progress = 0;
            ack.detail = _op_info.str();
        }

        return ack;
    }

    STATUS AcktoSender(const VmOperationAck &ack)
    {
        VmOperationAck send_ack = ack;
        send_ack.action_id = _req.action_id;

        if (send_ack.Validate())
        {
            MessageOption option(_upstream_sender, EV_VM_OP_ACK, 0, 0);
            return _receiver->Send(send_ack, option);
        }

        return ERROR;   //todo
    }

    bool IsDoing()
    {
        return !FindVmOpWorkflow(_req).empty();
    }

    string FindVmOpWorkflow(const VmOperationReq &req)
    {
        ostringstream label_where;
        label_where << "upstream_action_id = '" << req.action_id << "'";

        return FindWorkflow(label_where.str());
    }

    string GetVmDoing()
    {
        string wf_id;
        string wf_category;
        string wf_name;

        VirtualMachinePool *vm_pool = VirtualMachinePool::GetInstance();
        vm_pool->GetVmWorkFlowId(_req._vid, wf_id);
        GetWorkflowCategoryName(wf_id, wf_category, wf_name);

        return wf_name;
    }

    int GetVmState(int64 vid)
    {
        VirtualMachine *pvm = Get(false);
        SkyAssert(pvm);
        return pvm->_state;
    }

    int GetVmLogicState(int64 vid)
    {
        VirtualMachine *pvm = Get(false);
        SkyAssert(pvm);
        return pvm->_log_state;
    }

    bool IsVmExist()
    {
        VirtualMachinePool *vm_pool = VirtualMachinePool::GetInstance();
        return (SQLDB_OK == vm_pool->IsVmExist(_req._vid));
    }
    
    VirtualMachine *Get(bool lock)
    {
        VirtualMachinePool *vm_pool = VirtualMachinePool::GetInstance();
        SkyAssert(vm_pool);
        return vm_pool->Get(_req._vid, lock);
    }

    bool IsVmBindtoHost()
    {
        VirtualMachine *pvm = Get(false);
        if (pvm == NULL)
        {
            return false;
        }
        else
        {
            return (pvm->_hid != INVALID_HID);
        }
    }

    int SetValue(int64 vid, const string &key, int64 value)
    {
        return VmAssistant.SetValue(vid, key,value);
    }

protected:
    friend class VmWorkFlow;

    const MID   _upstream_sender;
    VmOperationReq _req;

    class VmInterface  VmAssistant;

    ostringstream   _op_info;
};

class ModifyVmcfg : public VmOpHandle
{
public:
    ModifyVmcfg(const MessageFrame &message, MessageUnit *receiver) :
            VmOpHandle(message, receiver)
    {
        VmModifyReq modify_req;
        modify_req.deserialize(message.data);

        _modification = modify_req._modification;
    }
    virtual ~ModifyVmcfg(){}

    static VmCfgElement GetDerivedKey(const MessageFrame &message)
    {
        VmModifyReq req;
        req.deserialize(message.data);
        
        return (VmCfgElement)req._modification._element;
    }

    virtual STATUS Implement()
    {
        if (!IsVmExist())
        {
            _op_info << "Vm(" << _modification._vid << ") is not existed!" << endl;
            return ERR_OBJECT_NOT_EXIST;
        }

        if (IsDoing())
        {
            _op_info << "Vm(" << _modification._vid << ") is modifying!" << endl;
            return ERROR_ACTION_RUNNING;
        }

        string doing = GetVmDoing();
        if (!doing.empty())
        {
            _op_info << "Vm(" << _modification._vid << ") is doing " << doing << endl;
            return ERROR_PERMISSION_DENIED;
        }

        if (GetVmState(_modification._vid) != VM_SHUTOFF)
        {
            _op_info << "Need shutoff VM and modify VM config!" << endl;
            return ERROR_NOT_SUPPORT;
        }

        STATUS ret = AnalysisCommonModifyStruct();
        if (ret != SUCCESS)
        {
            return ret;
        }

        if (!IsVmcfgChanged())
        {
            return SUCCESS;
        }

        ret = CheckModificationSupport();
        if (ret != SUCCESS)
        {
            return ret;
        }

        ret = EffectModification();

        if (ret == SUCCESS)
        {
            AttemptUnbindVmfromHost();
        }

        return ret;
    }

    virtual STATUS AnalysisCommonModifyStruct()
    {
        _op_info << "Modify " << Enum2Str::VmCfgElementStr(_modification._element) << " is not supportted!" << endl;
        return ERROR_NOT_SUPPORT;
    }

    virtual bool IsVmcfgChanged()
    {
        return true;
    }

    virtual STATUS CheckModificationSupport()
    {
        if (!IsVmBindtoHost())
        {
            return SUCCESS;
        }

        return CheckHostSupport();
    }

    virtual STATUS CheckHostSupport()
    {
        _op_info << "Modify " << Enum2Str::VmCfgElementStr(_modification._element) << " is not supportted!" << endl;
        return ERROR_NOT_SUPPORT;
    }

    virtual STATUS EffectModification()
    {
        _op_info << "Modify " << Enum2Str::VmCfgElementStr(_modification._element) << " is not supportted!" << endl;
        return ERROR_NOT_SUPPORT;
    }

    bool IsHostSupportModification(const ostringstream &where)
    {
        HostPool *host_pool = HostPool::GetInstance();
        SkyAssert(host_pool);

        vector<int64> hosts;
        host_pool->Match(hosts, where.str());

        return !hosts.empty();
    }

    void AttemptUnbindVmfromHost()
    {
    }

    VmCfgModifyIndividualItem   _modification;
};

}

#endif

