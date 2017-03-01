#ifndef HH_HOST_HA_H
#define HH_HOST_HA_H
#include "sky.h"
#include "mid.h"

namespace zte_tecs 
{

//ÏµÍ³²ßÂÔ
#define POLICY_MIGRATE    "migrate"
#define POLICY_STOP       "stop"
#define POLICY_REBOOT     "reboot"
#define POLICY_PAUSE      "pause"

class HostHaRec
{
public:
    string _host_name;
    int64  _host_id;
    string _port_name;
    string _vm_name;
    int64  _vm_id;
    int32  _type;
};

class HostHA : public MessageHandler
{
public:
    static HostHA *GetInstance()
    {
        if (NULL == _instance)
        {
            _instance = new HostHA();
        }
        return _instance;

    };

    ~ HostHA()
    {
        if(_mu)
        {
            delete _mu;
            _mu=NULL;
        }
       _instance = NULL;
     
    };   

    STATUS Init()
    {
        return StartMu(MU_HOST_HIGHAVAILABLE);
    }

    STATUS Receive(const MessageFrame& message)
    {
       return _mu->Receive(message);
    };

    void PrintAllFaultVm();    
    void RecordFaultVm (HostHaRec *pRec);
    virtual void MessageEntry(const MessageFrame& message);    

private:
    HostHA();	
    STATUS StartMu(const string& name);
    void HostFaultProcess(const MessageFrame &message);
    void HostEnetFaultProcess(const MessageFrame &message);
    void HostStorageFaultProcess(const MessageFrame &message);
    void HostVmFaultProcess(const MessageFrame &message);
    void HostCpuBusyProcess(const MessageFrame &message);
    void PolicyToOperation (const string &policy,int &operation);
    STATUS SendVmOp(int64 vid,int operation);
    static HostHA  *_instance;   
    MessageUnit *_mu;     
    list<HostHaRec*> _vm_process_list;			
};   

}// namespace zte_tecs
#endif // #ifndef HH_HOST_HA_H
