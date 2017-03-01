#ifndef EXC_LOG_AGENT_H
#define EXC_LOG_AGENT_H

#include "sky.h"
#include "mid.h"
#include "log_common.h"
using  namespace zte_tecs;
namespace zte_tecs
{

#define EXC_LOG_PATH_NAME_LENTH     (1024)
#define EXC_LOG_FILE_SIZE           (1024*1024)
class ExcLogAgent: public MessageHandler
{
public:
    static ExcLogAgent *GetInstance()
    {
        if( NULL == _instance)
        {
            _instance = new ExcLogAgent(); 
        }

        return _instance;
    };

    STATUS Init()
    {
        _exc_log_path = HC_LOG_PATH;
        if (0 != access(_exc_log_path.c_str(), 0))
        {
            STATUS ret;
            string cmd;
            cmd = "mkdir -p "+ _exc_log_path;
            ret = RunCmd(cmd);
            if (SUCCESS != ret)
            {
                printf("exe log init fail\n");
                return ret;
        }
        }
        return StartMu(EXC_LOG_AGENT);
    }
    
    ~ExcLogAgent();
    
    STATUS Receive(const MessageFrame& message);
    void MessageEntry(const MessageFrame& message);
    
private:
    static ExcLogAgent *_instance;
    ExcLogAgent();
    STATUS StartMu(const string& name);
    DISALLOW_COPY_AND_ASSIGN(ExcLogAgent);
    MessageUnit *_mu;
    string _exc_log_path;
};
}
#endif

