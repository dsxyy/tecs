#ifndef MESSAGE_AGENT_H
#define MESSAGE_AGENT_H
#include "message_unit.h"
#include "message_agent_impl.h"

class MessageAgent
{
public:   
    static bool CreateInstance()
    {
        if(NULL == instance)
        {
            try
            {
                instance = new MessageAgent(); 
            }
            catch(...)
            {
                printf("exception happens when constructing MessageAgent!\n");
                instance = NULL;
                return false;
            }
        }

        return true;
    };
   
    static MessageAgent *GetInstance()
    {
        return instance;
    };
   
    ~MessageAgent() 
    {
        instance = NULL;
        delete impl;
        impl = NULL;
    };

    STATUS SetCommId(const string& application,const string& process)
    {
        return impl->SetCommId(application,process);
    }
   
    STATUS Connect(const string& broker,const string& option,bool ackRequired)
    {
        return impl->Connect(broker,option,ackRequired);
    };
    
    STATUS Start(int priority = 1)
    {
        return impl->Start(priority);
    };
    
    STATUS Stop(void)
    {
        return impl->Stop();
    };
    
    STATUS RegisterUnit(MessageUnit *pmu)
    {
        if(!instance)
            return ERROR_NOT_READY;
        return impl->RegisterUnit(pmu);
    };
    
    STATUS DeregisterUnit(MessageUnit *pmu)
    {
        if(!instance)
            return ERROR_NOT_READY;
        return impl->DeregisterUnit(pmu);
    };
    
    MessageUnit *FindUnit(const string& unit_name) const
    {
        if(!instance)
            return NULL;
        return impl->FindUnit(unit_name);
    };

    MessageUnit *CurrentUnit()
    {
        if(!instance)
            return NULL;
        return impl->CurrentUnit();
    };

    STATUS JoinMcGroup(string group,string unit)
    {
        if(!instance)
            return ERROR_NOT_READY;
        return impl->JoinMcGroup(group, unit);
    };

    STATUS ExitMcGroup(string group,string unit)
    {
        if(!instance)
            return ERROR_NOT_READY;
        return impl->ExitMcGroup(group, unit);
    };
    
    STATUS ShowUnitTable() const
    {
        if(!instance)
            return ERROR_NOT_READY;
        return impl->ShowUnitTable();
    };

    STATUS ShowMcTable() const
    {
        if(!instance)
            return ERROR_NOT_READY;
        return impl->ShowMcTable();
    };

    STATUS ShowRouteTable() const
    {
        if(!instance)
            return ERROR_NOT_READY;
        return impl->ShowRouteTable();
    };

    STATUS ShowTraceUnit() const
    {
        if(!instance)
            return ERROR_NOT_READY;
        return impl->ShowTraceUnit();
    };

    STATUS SendMessage(const string& data,const MessageOption& option)
    {
        if(!instance)
            return ERROR_NOT_READY;
        return impl->SendMessage(data,option);
    };

    const string& application() const
    {
        return impl->ApplicationName();
    };
    
    const string& process() const
    {
        return impl->ProcessName();
    };
    
    STATUS Summary() const
    {
        return impl->Summary();
    };
    
    STATUS TraceUnit(const string& unit_name, uint32 message_id)
    {
        if (0 == message_id)
        {
            return impl->DelTraceUnit(unit_name);
        }

        return impl->InsertTraceUnit(unit_name,message_id);
    };
    
    STATUS AddKeyUnit(const string& unit)
    {
        return impl->AddKeyUnit(unit);
    };
    
    STATUS DelKeyUnit(const string& unit)
    {
        return impl->DelKeyUnit(unit);
    };
    
    STATUS ShowKeyUnit()
    {
        return impl->ShowKeyUnit();
    };
    
    int SendDebug();
    int ReceiveDebug();
    
private:
    MessageAgent();
    DISALLOW_COPY_AND_ASSIGN(MessageAgent);
    static MessageAgent *instance;
    MessageAgentImpl *impl;
};

void DestroyMessageAgent();
#endif

