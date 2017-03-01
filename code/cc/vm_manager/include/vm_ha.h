#ifndef TECS_CC_VM_HA_H
#define TECS_CC_VM_HA_H

#include "vm_messages.h"
#include "vm_interface.h"

namespace zte_tecs
{

class VMHA : public MessageHandlerTpl<MessageUnit>, public FuncResponseFeedback
{
public:
    static VMHA* GetInstance()
    {
        if(NULL == instance)
        {
            instance = new VMHA(); 
        }
   
        return instance;
    };
    
    ~VMHA()
    {
    };

    STATUS Init();
    void MessageEntry(const MessageFrame&);

private:
    friend void DbgSetVMHA(const char* chFault, const char* chPolicy);
    friend void DbgSimulateFault(const char* chFault, int64 hid);
    
    VMHA()
    {
    };
    DISALLOW_COPY_AND_ASSIGN(VMHA);
    static VMHA *instance;

    void PowerOn();
    void HostOfflineProcess(const class MessageHostFault &fault);
    void HostFaultProcess(const class MessageHostFault &fault);
    void HostStorageFaultProcess(const class MessageHostFault &fault);
    void ExecutePolicy(const string &policy, const class MessageHostFault &fault);
    void MigrateVms(int64 hid);
    void DestroyAllVms(const class MessageHostResetReq &req);
    const string GetPolicy(const string &fault);
    void SkipOfflineActions(int64 hid);

    class HostPool              *host_pool;
    class VmInterface           *VmAssistant;
    class VirtualMachinePool    *vm_pool;

};

}

#endif

