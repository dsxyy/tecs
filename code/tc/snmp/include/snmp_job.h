///////////////////////////////////////////////////////////
//  AlarmManager.h
//  Implementation of the Class AlarmManager
//  Created on:      15-ÆßÔÂ-2011 12:16:54
//  Original author: mazhaomian
///////////////////////////////////////////////////////////
#ifndef SNMP_JOB_H
#define SNMP_JOB_H
#include "event.h"
#include "mid.h"
#include "snmp_database.h"
#include "snmp_database_pool.h"
#include "alarm_agent_report.h"
#include "snmp_ftypes.h"
#include "pub_alarm.h"
#include "sky.h"

using namespace zte_tecs;
namespace zte_tecs
{
class SnmpJob : public MessageHandler
{
public:
    virtual ~SnmpJob();
    STATUS Receive(const MessageFrame& message);
    STATUS start();
    void read_snmpdataconf();
    static SnmpJob *GetInstance()
    {
        if(NULL == instance)
        {
            instance = new SnmpJob();
        }
        return instance;
    };
    
    STATUS Init()
    {
        STATUS ret = start();
        if(SUCCESS != ret)
        {
            return ret;
        }
        return StartMu(SNMP_AGENT);
    }
    
    T_SNMPProc_GlobVal tSnmpProcGlobal;
    bool ReadXml();
    void GetXmlRpcSession(void);
    void MessageEntry(const MessageFrame& message);

private:
    SnmpJob();
    static SnmpJob *instance;
    THREAD_ID _workthread_id;
    MessageUnit *mu;
    SnmpConfigurationPool *_scp;
    TrapAddressPool *_tap;
    SnmpXmlRpcUserPool *_sup;
    string _snmp_xml_file;
    STATUS StartMu(const string& name);
    void read_configdata_handler(char* achTblName);
    DISALLOW_COPY_AND_ASSIGN(SnmpJob);
};
}
void *SnmpCommunicationThread(void *ptr);

#endif

