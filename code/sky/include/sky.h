#ifndef SKY_H
#define SKY_H
#include "pub.h"
#include "skytype.h"
#include "message.h"
#include "timer.h"
#include "clock.h"
#include "datetime.h"
#include "message_queue.h"
#include "message_unit.h"
#include "uds.h"
#include "sys.h"
#include "shell.h"
#include "exception.h"
#include "ini_file.h"
#include "option_parser.h"
#include "misc.h"

struct ConnectConfig
{
    string broker_url;
    int broker_port;
    string broker_option;
    bool enable_broker_require_ack;
};

struct MessagingConfig
{
    string application;
    string process;
    vector<ConnectConfig> connects;
};

class SkyConfig
{
public:
    MessagingConfig messaging;
    ExceptionConfig exccfg;
    TimerConfig     timecfg;
    friend ostream & operator << (ostream& os, const SkyConfig& conf)
    {
        os << "application = " << conf.messaging.application << endl;
        os << "process = " << conf.messaging.process << endl;
        for (vector<ConnectConfig>::const_iterator it= conf.messaging.connects.begin();
             it != conf.messaging.connects.end(); ++it)
        {
            os << "message broker url = " << it->broker_url << endl;
            os << "message broker port = " << it->broker_port << endl;      
            os << "message broker option = " << it->broker_option << endl;
            os << "message broker require_ack = " << it->enable_broker_require_ack << endl;
        }
        os << "debug mode = " << conf.exccfg.debug << endl;
        os << "lastwords file = " << conf.exccfg.lastwords_file << endl;
        
        os << "time scan msecond = " << conf.timecfg.tv_msec << endl;
        os << "time scan second = " << conf.timecfg.tv_msec/1000 << endl;
        os << "time scan nsecond = " << (conf.timecfg.tv_msec%1000)*1000*1000 << endl;
        
        return os;
    };
};

template<typename T>
class MessageHandlerTpl: public MessageHandler
{
public:
    MessageHandlerTpl()
    {
        m = NULL;
    };

    ~MessageHandlerTpl()
    {
        if(m)
            delete m;
    };

    STATUS Start(const string& name)
    {
        if(m)
        {
            return ERROR_DUPLICATED_OP;
        }
        
        //创建并启动消息单元，消息单元的名称最好用宏统一定义
        m = new T(name);
        m->SetHandler(this);
        STATUS ret = m->Run();
        if(SUCCESS != ret)
        {
            delete m;
            m = NULL;
            return ret;
        }
        
        ret = m->Register();
        if(SUCCESS != ret)
        {
            delete m;
            m = NULL;
            return ret;
        }
        
        m->Print();
        return SUCCESS;
    };

    STATUS Receive(const MessageFrame& message)
    {
        if(!m)
            return ERROR_NOT_READY;
        else
            return m->Receive(message);
    };
    
protected:
    T *m;
private:
    DISALLOW_COPY_AND_ASSIGN(MessageHandlerTpl);
};

STATUS SkyInit(const SkyConfig&);

STATUS ThreadDllMonitorInit(bool enable_check,uint32 check_time);
    
STATUS StartThreadDllMonitor(THREAD_ID workthread_id,string name);
    
STATUS EndThreadDllMonitor(THREAD_ID workthread_id);

STATUS DeleteThreadDllMonitor(THREAD_ID workthread_id);

#endif

