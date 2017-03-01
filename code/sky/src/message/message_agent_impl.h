#ifndef MSG_AGENT_QPID_IMPL
#define MSG_AGENT_QPID_IMPL
#include <qpid/messaging/Connection.h>
#include <qpid/messaging/Message.h>
#include <qpid/messaging/Receiver.h>
#include <qpid/messaging/Sender.h>
#include <qpid/messaging/Session.h>
#include <qpid/types/Variant.h>
#include "message_unit.h"
#include "sysinfo.h"

using namespace qpid::messaging;
using namespace qpid::types;
typedef Message QpidMessage;
typedef Connection QpidConnection;
typedef Session QpidSession;
typedef Sender QpidSender;
typedef Receiver QpidReceiver;
typedef Address QpidAddress;

enum AMQPImpl
{
    Qpid,
    RabbitMQ
};

class MessageAgentImpl
{
public:
    virtual ~MessageAgentImpl() {};
    virtual STATUS SetCommId(const string&,const string&) = 0;
    virtual STATUS Connect(const string& broker,const string& option,bool ackRequired) = 0;
    virtual const string& ApplicationName() = 0;
    virtual const string& ProcessName() = 0;
    virtual STATUS RegisterUnit(MessageUnit *) = 0;
    virtual STATUS DeregisterUnit(MessageUnit *) = 0;
    virtual MessageUnit *FindUnit(const string&) = 0;
    virtual MessageUnit *CurrentUnit() = 0;
    virtual STATUS JoinMcGroup(const string &group,const string & unit) = 0;
    virtual STATUS ExitMcGroup(const string & group,const string & unit) = 0;
    virtual STATUS ShowUnitTable() = 0;
    virtual STATUS ShowMcTable() = 0;
    virtual STATUS ShowRouteTable() = 0;
    virtual STATUS Summary() = 0;
    virtual STATUS Start(int priority) = 0;
    virtual STATUS Stop() = 0;
    virtual STATUS SendMessage(const string& data,const MessageOption& option) = 0;
    virtual STATUS SendDebug() = 0;
    virtual int ReceiveDebug() = 0;
    virtual STATUS InsertTraceUnit(const string &unit,int message_id) = 0;   
    virtual STATUS DelTraceUnit(const string& unit) = 0;
    virtual STATUS ShowTraceUnit() = 0;
    virtual STATUS AddKeyUnit(const string& unit) = 0;
    virtual STATUS DelKeyUnit(const string& unit) = 0;
    virtual STATUS ShowKeyUnit() = 0;
};

//维护多对多的组-成员关系
typedef list<string> NameList;
class MultiRelations
{
public:
    MultiRelations()
    {
    };

    ~MultiRelations() {};
    void AddMemberToGroup(const string & member,const string & group);
    void DelMemberFromGroup(const string & member,const string & group);
    void ShowMembers();
    void ShowGroups();
    void DeleteMember(const string & member);
    void DeleteGroup(const string & group);
    bool HasGroup(const string & group);
    const NameList* GetMemberList(const string & group);
    const NameList* GetGroupList(const string & member);

private:
    map<string,NameList> members; //某个组中有哪些成员
    map<string,NameList> groups; //某个成员属于哪些组
};

class MessageFrameHandler
{
public:
    virtual ~MessageFrameHandler() {};    
    virtual STATUS Receive(const MessageFrame&,const string&) = 0;
};

class ChannelAgentQpid
{
public:
    ChannelAgentQpid()
    {
        _messages_from_broker = 0;
        _messages_to_broker = 0;
        _connection = NULL;
        _handler = NULL;
        _workthread_id = INVALID_THREAD_ID;
        _is_ack_required = false;
        _sessions_mutex.SetName("_sessions_mutex");
        //_sessions_mutex.SetDebug(true);
        _sessions_mutex.Init();
        
        _mc_receivers_mutex.SetName("_mc_receivers_mutex");
        //_mc_receivers_mutex.SetDebug(true);
        _mc_receivers_mutex.Init();
    };

    ChannelAgentQpid(const string& broker,                    
                    const string& option,
                    bool ackRequired,
                    const string& subject):
    _broker(broker),    
    _option(option),
    _is_ack_required(ackRequired),
    _subject(subject)
    {
        _messages_from_broker = 0;
        _messages_to_broker = 0;
        _connection = NULL;
        _handler = NULL;
        _workthread_id = INVALID_THREAD_ID;
        
        _sessions_mutex.SetName("_sessions_mutex");
        //_sessions_mutex.SetDebug(true);
        _sessions_mutex.Init();
        
        _mc_receivers_mutex.SetName("_mc_receivers_mutex");
        //_mc_receivers_mutex.SetDebug(true);
        _mc_receivers_mutex.Init();
    };

    ~ChannelAgentQpid()
    {
        //cout << "~ConnectAgentQpid destructor called!" << endl;
        if(INVALID_THREAD_ID != _workthread_id)
            KillThread(_workthread_id);

        if(_connection)
        {
            _channel_session.acknowledge(false);
            _channel_session.close();
            _connection->close();
        }
    };

    STATUS Connect();
    STATUS CreateSender(const string & name);
    STATUS DeleteSender(const string & name);
    STATUS CreateMcReceiver(const string & group);
    STATUS DeleteMcReceiver(const string & group);
    STATUS RegisterHandler(MessageFrameHandler* phandler);
    STATUS SendMessage(const string& data,const MessageOption& option);
    STATUS SendMcMessage(const string& data,const MessageOption& option);    
    STATUS DirectSend(const string& data,const MessageOption& option);
    void WorkLoop();
    STATUS Start(int priority);
    STATUS Stop(void);
    int SendDebug();
    int ReceiveDebug();
    STATUS Summary();
    STATUS ShowMcTable();

private:
    STATUS QpidPack(const string& data, const MessageOption& option,Variant::Map &content);
    STATUS QpidUnPack(string& data,MessageOption& option,Variant::Map &content);
    STATUS QpidSend(QpidSender& sender,
                    const string subject,
                    const string& data,
                    const MessageOption& option);
    string& TopicAddressOption();
    string& FanoutAddressOption(const string& group);

    //本Channel的通信broker地址和连接选项
    string _broker;    
    string _option;
    bool _is_ack_required;
    string _subject;

    //服务器消息接收任务tid
    THREAD_ID _workthread_id;

    //qpid地址选项
    static string topic_address_option;
    static string fanout_address_option;
    //消息代理Channel与消息服务器之间的AMQP连接对象
    QpidConnection *_connection;
    //消息代理Channel工作线程与消息服务器之间的会话对象
    QpidSession _channel_session;
    
    string _channel_sender;
    string _channel_receiver;

    //保护会话表的信号量
    Mutex _sessions_mutex;
    
    //各个消息单元工作线程与消息服务器之间的会话对象
    map<string,QpidSession> _sessions;
    
    //各个消息单元用于发送普通单播消息的qpid sender对象
    map<string,string> _senders;
    
    //各个消息单元用于发送组播消息的qpid sender对象
    map<string,string> _mc_senders;
    
    //保护组播receiver表的信号量
    Mutex _mc_receivers_mutex;
    
    //多连接本进程加入的各个组播组的qpid receiver
    map<string,string> _mc_receivers;

    //消息数量统计
    uint64 _messages_from_broker;
    uint64 _messages_to_broker;

    MessageFrameHandler *_handler;
};

class MessageAgentQpidImpl:public MessageAgentImpl,MessageFrameHandler
{
public:
    MessageAgentQpidImpl()
    {       
        _connect_agents_mutex.SetName("_connect_agents_mutex");
        //_connect_agents_mutex.SetDebug(true);
        _connect_agents_mutex.Init();

        _units_mutex.SetName("_units_mutex");
        //_units_mutex.SetDebug(true);
        _units_mutex.Init();

        _mc_relation_mutex.SetName("_mc_relation_mutex");
        //_mc_relation_mutex.SetDebug(true);
        _mc_relation_mutex.Init();

        _routes_mutex.SetName("_routes_mutex");
        //_routes_mutex.SetDebug(true);
        _routes_mutex.Init();
    };

    MessageAgentQpidImpl(const string& application,const string& process):
        _application(application),_process(process)
    {
        _connect_agents_mutex.SetName("_connect_agents_mutex");
        //_connect_agents_mutex.SetDebug(true);
        _connect_agents_mutex.Init();

        _units_mutex.SetName("_units_mutex");
        //_units_mutex.SetDebug(true);
        _units_mutex.Init();

        _key_units_mutex.Init();
        
        _mc_relation_mutex.SetName("_mc_relation_mutex");
        //_mc_relation_mutex.SetDebug(true);
        _mc_relation_mutex.Init();

        _routes_mutex.SetName("_routes_mutex");
        //_routes_mutex.SetDebug(true);
        _routes_mutex.Init();
    };

    ~MessageAgentQpidImpl()
    {

    };

    STATUS SetCommId(const string& application,const string& process);
    const string& ApplicationName(){return _application;};
    const string& ProcessName(){return _process;};
    STATUS Connect(const string& broker,const string& option,bool ackRequired);
    STATUS RegisterUnit(MessageUnit *pmu);
    STATUS DeregisterUnit(MessageUnit *pmu);
    MessageUnit *CurrentUnit();
    STATUS JoinMcGroup(const string &group,const string & unit);
    STATUS ExitMcGroup(const string & group,const string & unit);
    MessageUnit *FindUnit(const string& unit);
    STATUS ShowUnitTable();
    STATUS ShowMcTable();
    STATUS ShowRouteTable();    
    STATUS Start(int priority);
    STATUS Stop();
    STATUS SendMessage(const string& data,const MessageOption& option);
    STATUS GetRunningInfo(ProcRuntimeInfo& info);
    STATUS DealSysMessage(const MessageFrame& message);
    STATUS Receive(const MessageFrame& message,const string& broker);
    STATUS Summary();
    STATUS AddKeyUnit(const string& unit);
    STATUS DelKeyUnit(const string& unit);
    bool IsKeyUnit(const string& unit);
    STATUS ShowKeyUnit();
    int SendDebug();
    int ReceiveDebug();
    STATUS InsertTraceUnit(const string &unit,int message_id);    
    STATUS DelTraceUnit(const string& unit);
    STATUS ShowTraceUnit();

private:
    STATUS DestroyAllUnits();
    STATUS DirectSend(const string& data,MessageOption& option);
    STATUS InternalSend(const string& data,const MessageOption& option);
    STATUS RouteSend(const string& route,const string& data,const MessageOption& option);
    STATUS MutliSend(const string& data,const MessageOption& option);
    STATUS InsertRoute(const string& route,const string& broker);
    bool FilterRedundantMc(const MessageFrame& message,const string& broker);
    void TracePrint(const string& data,const MessageOption& option);
    bool NeedTraceMessage(const string& unit,uint32& id);
    

    //本进程的通信application
    string _application;
    //本进程的通信process
    string _process;
    /* 下面锁目前的顺序为 :
	_connect_agents,_units_mutex
			_units_mutex,_mc_relation_mutex
			_units_mutex,_key_units_mutex
	_connect_agents,_routes_mutex
	以后如果要增加请注意顺序
    */
    Mutex _connect_agents_mutex;
    map<string,ChannelAgentQpid*> _connect_agents;

    //保护消息单元注册表的信号量
    Mutex _units_mutex;
    //消息单元注册表
    map<string,MessageUnit*> _units;
    
    map<string,uint32>  _trace_units; 

    //保护关键消息单元列表的信号量
    Mutex _key_units_mutex;
    //关键消息单元列表，包含消息单元名称以及加入本列表的时间
    map<string,int64> _key_units;
    
    //组播成员关系数据
    Mutex _mc_relation_mutex;
    MultiRelations _mc_relations;

    //发包的路由
    Mutex _routes_mutex;
    map<string,string> _routes;
};

class MessageAgentImplFactory
{
public:
    MessageAgentImplFactory(){};
    ~MessageAgentImplFactory(){};

    static MessageAgentImpl* GetImpl(AMQPImpl Impl)
    {
        MessageAgentImpl* impl = NULL;
        switch (Impl)
        {
            case Qpid:
                impl = new MessageAgentQpidImpl();
                break;
            default:
                break;
        }
        return impl;
    };

    static MessageAgentImpl* GetImpl(AMQPImpl Impl,const string& application,const string& process)
    {
        MessageAgentImpl* impl = NULL;
        switch (Impl)
        {
            case Qpid:
                impl = new MessageAgentQpidImpl(application,process);
                break;
            default:
                break;
        }
        return impl;
    };
};

#endif


