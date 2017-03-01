#include "sky.h"
#include "operator_log.h"
#include "postgresql_db.h"
#include "sqlite_db.h"
#include "object_sql.h"
#include "pool_object_sql.h"
#include "log_agent.h"
#include "event.h"
#include "mid.h"
#include "db_config.h"

#define EV_OPERATOR_LOG     uint32(3)
#define LOCAL_TIME_LENTH    uint32(256)
#define KEEP_DURATION       time_t(30*24*3600)   //保存30天以内的操作日志，以秒为单位

using namespace zte_tecs;
namespace zte_tecs
{

//操作日志消息类定义开始
class OpLogMessage:public Serializable
{
public:
    OpLogMessage()
    {
        _uid     = 0;
        _visible = 0;
    };

    OpLogMessage( int64  uid,
                const string &user_name,
                const string &client_ip,
                const string &user_agent,
                const string &rpc_name,
                const string &parameters,
                const string &returns,
                Datetime &log_time,
                Datetime &begin_time,
                Datetime &end_time,
                int visible)
    {
        _uid        = uid;
        _user_name  = user_name;    
        _client_ip  = client_ip;
        _user_agent = user_agent;
        _rpc_name   = rpc_name;
        _parameters = parameters;
        _returns    = returns;
        _log_time   = log_time;   
        _begin_time = begin_time;
        _end_time   = end_time;
        _visible    = visible;
    };

    ~OpLogMessage()
    {};

    SERIALIZE
    {
        SERIALIZE_BEGIN(OpLogMessage);      //序列化开始，括号中填写类名
        WRITE_DIGIT(_uid);
        WRITE_STRING(_user_name);           //C++中的string容器必须用WRITE_STRING宏来序列化
        WRITE_STRING(_client_ip);
        WRITE_STRING(_user_agent);
        WRITE_STRING(_rpc_name);
        WRITE_STRING(_parameters);
        WRITE_STRING(_returns);
        WRITE_OBJECT(_log_time);           
        WRITE_OBJECT(_begin_time);
        WRITE_OBJECT(_end_time);
        WRITE_DIGIT(_visible);
        SERIALIZE_END();                    //序列化结束
    }

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(OpLogMessage);    //反向序列化开始，括号中填写类名
        READ_DIGIT(_uid);
        READ_STRING(_user_name);            //C++中的string容器必须用WRITE_STRING宏来序列化       
        READ_STRING(_client_ip);
        READ_STRING(_user_agent);
        READ_STRING(_rpc_name);
        READ_STRING(_parameters);
        READ_STRING(_returns);
        READ_OBJECT(_log_time);
        READ_OBJECT(_begin_time);
        READ_OBJECT(_end_time);
        READ_DIGIT(_visible);
        DESERIALIZE_END();                  //反向序列化结束
    }

    //OpLogInfo tOpLogInfo;
    int64   _uid;
    string  _user_name;    
    string  _client_ip;
    string  _user_agent;
    string  _rpc_name;
    string  _parameters;
    string  _returns;
    Datetime  _log_time;    
    Datetime  _begin_time;
    Datetime  _end_time;
    int     _visible;

};
//操作日志消息类定义结束


//操作日志类定义开始
class  OpLog:public PoolObjectSQL
{
    /*******************************************************************************
    * 1. public section
    *******************************************************************************/
public:
    OpLog();
    OpLog(int64 oid, 
        int64 uid,
        const string &_user_name, 
        const string &_client_ip, 
        const string &_user_agent, 
        const string &_rpc_name,
        const string &_parameters, 
        const string &_returns, 
        const Datetime &log_time,
        const Datetime &begin_time,
        const Datetime &end_time, 
        int visible);

    ~OpLog();


    //此处还应该定义一堆的属性的set get函数
    void setUid(int64 uid)
    {
        _uid = uid;
    }

    void setUsrName(const string &user_name)
    {
        _user_name = user_name;
        return;
    }

    void setClientIp(const string &client_ip)
    {
        _client_ip = client_ip;
    }

    void setUsrAgent(const string &user_agent)
    {
        _user_agent = user_agent;
    }

    void setRpcName(const string &rpc_name)
    {
        _rpc_name = rpc_name;
    }

    void setParameter(const string &parameters)
    {
        _parameters = parameters;
    }

    void setReturn(const string &returns)
    {
        _returns = returns;
    }
    void setLogTime(const Datetime & log_time)
    {
        _log_time = log_time;
        return;
    }

    void setBeginTime(const Datetime & begin_time)
    {
        _begin_time = begin_time;
    }

    void setEndTime(const Datetime & end_time)
    {
        _end_time = end_time;
    }

    void setVisible(int visible)
    {
        _visible = visible;
    }

    void setLogId(int64 oid)
    {
        _oid = oid;
    }

    virtual int Select(SqlDB * db);

    virtual int Insert(SqlDB * db, string& error_str);

    virtual int Update(SqlDB * db);

    virtual int Drop(SqlDB * db);

    //创建表
    static int BootStrap(SqlDB *db)
    {
        ostringstream oss(OpLog::db_bootstrap);

        return db->Execute(oss);
    }
    /*******************************************************************************
    * 3. private section
    *******************************************************************************/
private:
    friend class OperatorLogAgent;
    enum ColNames
    {
        OID                 = 0,
        USRNAME             = 1,
        INSERTCALENDAR      = 2,
        INSERTUTC           = 3,
        USRID               = 4,
        CLIENTIP            = 5,
        USRAGENT            = 6,
        RPCNAME             = 7,
        PARAMETER           = 8,
        RETURN              = 9,
        BEGINCALENDAR       = 10,
        ENDCALENDAR         = 11,
        BEGINUTC            = 12,
        ENDUTC              = 13,
        VISIBLE             = 14,
        LIMIT               = 15
    };

    DISALLOW_COPY_AND_ASSIGN(OpLog);

    //查询回调函数
    int SelectCallback(void *nil, SqlColumn * columns);

    //表名
    static const char * table;

    //查询的列
    static const char * db_names;

    //创建表的SQL
    static const char * db_bootstrap;

    //int64 _oid;  不需要定义在PoolObjectSQL中统一定义
    int64  _oid;

    //各个字段
    int64   _uid;
    string  _user_name;
    string  _client_ip;
    string  _user_agent;
    string  _rpc_name;
    string  _parameters;
    string  _returns;
    Datetime  _log_time;
    Datetime  _begin_time;
    Datetime  _end_time;
    int     _visible;
};

const char * OpLog::table       =   "operation_log";
const char * OpLog::db_names    =   "oid,"
                                    "uid,"
                                    "user_name,"
                                    "client_ip,"
                                    "user_agent,"
                                    "rpc_name,"
                                    "parameters,"
                                    "returns,"
                                    "log_calendar_time,"
                                    "begin_calendar_time,"
                                    "end_calendar_time,"
                                    "log_utc_time,"                              
                                    "begin_utc_time,"
                                    "end_utc_time,"
                                    "visible";

const char * OpLog::db_bootstrap=   "create table if not exists operatorlog "
                                    "("
                                    "oid NUMERIC(19) primary key,"
                                    "uid NUMERIC(19),"
                                    "user_name VARCHAR(32),"
                                    "client_ip VARCHAR(64),"
                                    "user_agent VARCHAR(512),"
                                    "rpc_name VARCHAR(128),"
                                    "parameters text,"
                                    "returns text,"
                                    "log_calendar_time NUMERIC(19),"
                                    "begin_calendar_time NUMERIC(19),"
                                    "end_calendar_time NUMERIC(19),"
                                    "log_utc_time VARCHAR(32),"
                                    "begin_utc_time VARCHAR(32),"
                                    "end_utc_time VARCHAR(32),"
                                    "visible int"
                                    ")";


OpLog::OpLog()
{}

OpLog::OpLog(int64 oid, 
            int64 uid,
            const string &user_name, 
            const string &client_ip, 
            const string &user_agent, 
            const string &rpc_name, 
            const string &parameters,
            const string &returns, 
            const Datetime & log_time,     
            const Datetime &begin_time, 
            const Datetime & end_time, 
            int visible)
{
    _oid        = oid,
    _uid        = uid;
    _user_name  = user_name;    
    _client_ip  = client_ip;
    _user_agent = user_agent;
    _rpc_name   = rpc_name;
    _parameters = parameters;
    _returns    = returns;
    _log_time   = log_time;
    _begin_time = begin_time;
    _end_time   = end_time;
    _visible    = visible;
}

OpLog::~OpLog()
{}



int OpLog::Select(SqlDB * db)
{
    ostringstream   oss;
    int             rc;
//    int64             boid;

    SetCallback(static_cast<Callbackable::Callback>(&OpLog::SelectCallback));

    //避免使用SELECT *，应该明确要查询的列，这样在表结构升级时才不会有问题
    oss << "SELECT " << db_names << " FROM " << table;

//    boid = _oid;
//    _oid  = -1;

    rc = db->Execute(oss, this);

    UnsetCallback();

    if (0 != rc)
    {
        return -1;
    }
    return 0;
}


int OpLog::SelectCallback(void *nil, SqlColumn * columns)
{
    if ( NULL == columns ||
            LIMIT != columns->get_column_num()
       )
    {
        cout << "ERROR!!!!!! \n";
        return -1;
    }

    cout << " Oid: " << columns->GetValueInt64(OID);
    cout << " UsrId:" << columns->GetValueInt64(USRID);
    cout << " UsrName:" << columns->GetValueText(USRNAME);
    cout << " ClientIp:" << columns->GetValueText(CLIENTIP);
    cout << " UsrAgent:" << columns->GetValueText(USRAGENT);
    cout << " RpcName:" << columns->GetValueText(RPCNAME);
    cout << " Parameter:" << columns->GetValueText(PARAMETER);
    cout << " Return:" << columns->GetValueText(RETURN);
    cout << " LogCalendar:" << columns->GetValueInt(INSERTCALENDAR);
    cout << " BeginCalendar:" << columns->GetValueInt(BEGINCALENDAR);
    cout << " EndCalendar:" << columns->GetValueInt(ENDCALENDAR);
    cout << " LogUTC:" << columns->GetValueText(INSERTUTC);
    cout << " BeginUTC:" << columns->GetValueText(BEGINUTC);
    cout << " EndUTC:" << columns->GetValueText(ENDUTC);
    cout << " Visible:" << columns->GetValueInt(VISIBLE);
    cout << endl;

    return 0;
}

int OpLog::Insert(SqlDB * db, string& error_str)
{
    ostringstream   oss;

    SqlCommand sql(db,table);
    sql.Add("oid", _oid);
    sql.Add("uid", _uid);
    sql.Add("user_name", _user_name);    
    sql.Add("client_ip", _client_ip);
    sql.Add("user_agent", _user_agent);
    sql.Add("rpc_name", _rpc_name);
    sql.Add("parameters",  _parameters);
    sql.Add("returns", _returns); 
    sql.Add("log_calendar_time", (int64)_log_time.tointeger());
    sql.Add("begin_calendar_time", (int64)_begin_time.tointeger());
    sql.Add("end_calendar_time", (int64)_end_time.tointeger());
    sql.Add("log_utc_time", _log_time.tostr(1));
    sql.Add("begin_utc_time", _begin_time.tostr(1));
    sql.Add("end_utc_time", _end_time.tostr(1));
    sql.Add("visible", _visible);
    
    return sql.Insert();
}

int OpLog::Update(SqlDB * db)
{
    return 0;
}

int OpLog::Drop(SqlDB * db)
{
    ostringstream   oss;
    oss << "delete from " 
        << table 
        << " where insert_to_db_calendar_time < " 
        << _log_time.tointeger() - KEEP_DURATION ;
    int rc = db->Execute(oss);
    if ( 0 == rc )
    {
        set_valid(false);
    }
    return rc;
}
//操作日志类定义结束


//操作日志代理类定义开始

class OperatorLogAgent: public MessageHandler,Callbackable
{
public:
    static OperatorLogAgent *GetInstance()
    {
        if (NULL == instance)
        {
            instance = new OperatorLogAgent();
        }

        return instance;
    };

    ~OperatorLogAgent()
    {
        delete _mu;
        instance = NULL;
    };
                

    int RecOpLog(int64 uid,
                const string &user_name, 
                const string &client_ip, 
                const string &user_agent, 
                const string &rpc_name, 
                const string &parameters,
                const string &returns,     
                Datetime &begin_time, 
                Datetime &end_time, 
                int visible);

    STATUS Receive(const MessageFrame& message);
    void MessageEntry(const MessageFrame& message);
    STATUS Init(SqlDB *pDb);
private:
    OperatorLogAgent()
    {
        _mu = NULL;
    };
    STATUS StartMu(const string& name);

    int InitCallback(void *nil, SqlColumn *pColumns)
    {
        _lastOID = pColumns->GetValueInt64(0);
        return 0;
    }

    int64 _lastOID;
    MessageUnit  *_mu;
    SqlDB  *pOpLogDB;
    static OperatorLogAgent* instance;
};


OperatorLogAgent *OperatorLogAgent::instance = NULL;

STATUS OperatorLogAgent::StartMu(const string& name)
{
    // 消息单元的创建和初始化
    _mu = new MessageUnit(name);
    if (!_mu)
    {
        OutPut(SYS_EMERGENCY, "Create mu %s failed!\n",name.c_str());
        return ERROR_NO_MEMORY;
    }

    STATUS ret = _mu->SetHandler(this);
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "SetHandler mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }

    ret = _mu->Run();
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "Run mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }

    ret = _mu->Register();
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "Register mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }

    //给自己发送上电消息，促使消息单元状态切换到工作态
    MessageFrame message(SkyInt32(0), EV_POWER_ON);
    ret = _mu->Receive(message);
    if (SUCCESS != ret)
    {
        SkyAssert(0);
        return ret;
    }
    _mu->Print();
    return SUCCESS;
}

STATUS OperatorLogAgent::Receive(const MessageFrame& message)
{
    return _mu->Receive(message);
}

STATUS OperatorLogAgent::Init(SqlDB *pDb)
{
    pOpLogDB = pDb;
    SetCallback(static_cast<Callbackable::Callback>(&OperatorLogAgent::InitCallback));

    ostringstream oss;
    oss << "SELECT MAX(oid) FROM " << OpLog::table;
    int ret = pOpLogDB->Execute(oss,this);
    UnsetCallback();
    if (0 != ret)
    {
        return ERROR_DB_SELECT_FAIL;
    }
    return StartMu(OP_LOG_AGENT);
}

int OperatorLogAgent::RecOpLog(int64 uid,
                            const string &user_name, 
                            const string &client_ip, 
                            const string &user_agent, 
                            const string &rpc_name, 
                            const string &parameters,
                            const string &returns,     
                            Datetime &begin_time, 
                            Datetime &end_time, 
                            int visible)
{ 
    Datetime log_time(time(0));//from 1970
    OpLogMessage OpLogMsg(uid, 
                        user_name,
                        client_ip, 
                        user_agent,
                        rpc_name,
                        parameters, 
                        returns, 
                        log_time, 
                        begin_time, 
                        end_time,
                        visible);

    //发往操作日志代理

    //让日志代理收到日志消息
    MessageOption option(OP_LOG_AGENT,EV_OPERATOR_LOG,0,0);
    //构造日志消息
    MessageFrame frame(OpLogMsg,option);
    Receive(frame);
    return SUCCESS;
}

void OperatorLogAgent::MessageEntry(const MessageFrame& message)
{
    switch (_mu->get_state())
    {
    case S_Startup:
    {
        switch (message.option.id())
        {
        case EV_POWER_ON:
        {
            _mu->set_state(S_Work);
            OutPut(SYS_NOTICE, "%s power on!\n",_mu->name().c_str());
            break;
        }
        default:
            OutPut(SYS_NOTICE, "Ignore any message except power on!\n");
            break;
        }
        break;
    }

    case S_Work:
    {
        //根据收到的消息id进行处理
        switch (message.option.id())
        {
        case EV_OPERATOR_LOG:
        {
            OpLogMessage log_msg;
            log_msg.deserialize(message.data);
            string errmsg;
            //构造操作日志
            OpLog op_log(++_lastOID,
                log_msg._uid,
                log_msg._user_name,
                log_msg._client_ip,
                log_msg._user_agent,
                log_msg._rpc_name,
                log_msg._parameters,
                log_msg._returns,
                log_msg._log_time,
                log_msg._begin_time,
                log_msg._end_time,
                log_msg._visible);

            //写入数据库
            //cout << "begin insert !!" << endl;
            op_log.Insert(pOpLogDB,errmsg);
            //tmpOpLog.Select(pOpLogDB);
            break;
        }
        default:
            return;
        }
        break;
    }

    default:
        break;
    }
    return;
}
//操作日志代理类定义结束

//对外提供的函数定义开始
int RecOperatorLog(struct OpLogInfo *operator_log)
{
    if ( NULL == operator_log)
    {
        cout<<"RecOperatorLog input para is NULL!!\n";
        return ERROR_INVALID_ARGUMENT;
    }

    OperatorLogAgent *pOpLog = OperatorLogAgent::GetInstance();

    return pOpLog->RecOpLog(operator_log->_uid,
                        operator_log->_user_name, 
                        operator_log->_client_ip, 
                        operator_log->_user_agent,
                        operator_log->_rpc_name, 
                        operator_log->_parameters, 
                        operator_log->_returns, 
                        operator_log->_begin_time,
                        operator_log->_end_time, 
                        operator_log->_visible);
}

STATUS OpLogInit(SqlDB *pDb)
{
    OperatorLogAgent *pOpLog = OperatorLogAgent::GetInstance();
    return pOpLog->Init(pDb);
}
}

