#ifndef OPERATOR_LOG_H
#define OPERATOR_LOG_H
#include "sky.h"
#include "tecs_pub.h"
#include "postgresql_db.h"
using namespace zte_tecs;
namespace zte_tecs
{

struct OpLogInfo
{
public:
    int64   _uid;
    string  _user_name;    
    string  _client_ip; 
    string  _user_agent; 
    string  _rpc_name;
    string  _parameters;
    string  _returns;
    Datetime  _begin_time;
    Datetime  _end_time;
    int     _visible;

    friend ostream & operator << (ostream& os, const OpLogInfo& log)
    {
        Datetime dt;
        os << "=================xmlrpc call at " << dt.serialize() << "=================" <<endl;
        os << "user id: " << log._uid << endl;
        os << "user name: " << log._user_name << endl;      
        os << "from ip: " << log._client_ip << endl;
        os << "rpc name: " << log._rpc_name << endl;
        os << "parameters: " << log._parameters << endl;
        os << "returns: " << log._returns << endl;
        os << "====================================================================" <<endl;
        return os;
    };
};

STATUS OpLogInit(SqlDB *pDb);
int RecOperatorLog(struct OpLogInfo *pctmpOpLogInfo);



}
#endif



