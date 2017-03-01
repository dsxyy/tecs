#ifndef SKY_MESSAGE_H
#define SKY_MESSAGE_H
#include "serialization.h"

#define MESSAGE_TYPE_DEFAULT            1    //默认的消息类型，所有上层用户消息都使用该类型
#define SKY_MESSAGE_TYPE_END            999 //1    //  [1, 999] 的消息类型都由sky保留
#define SKY_MESSAGE_ID_END              999 //1// 任何消息类型的1~999的消息id由sky保留
#define SKY_WORKFLOW_ID_BEGIN           100   //工作流的消息号从100开始定义
#define SKY_USER_MSG_TYPE_BEGIN         (SKY_MESSAGE_TYPE_END+1) //上层用户如果要自定义消息类型，请从1000开始
#define SKY_USER_MSG_ID_BEGIN           (SKY_MESSAGE_ID_END+1) //上层用户如果要自定义消息id，请从 1000开始
#define MESSAGE_AGENT                   "*"  //消息代理自身也可以接收和发送消息，此时它的unit字段是个专用的特殊名称
/* ==============sky内部使用的消息type，最大值为SKY_MESSAGE_TYPE_END============== */
#define MESSAGE_TIMER                   (MESSAGE_TYPE_DEFAULT+1)   //定时器消息
//#define MESSAGE_SYSTEM                (MESSAGE_TYPE_DEFAULT+2)   //系统管理消息

/* =====sky内部使用的消息id，类型为MESSAGE_TYPE_DEFAULT，最大值为SKY_MESSAGE_ID_END===== */
//注意: message id不能等于0，0作为无效的id值，在trace功能中用到了
#define EV_RUNTIME_INFO_REQ             (SKY_WORKFLOW_ID_BEGIN+1)    //消息单元运行时信息查询请求
#define EV_RUNTIME_INFO_ACK             (SKY_WORKFLOW_ID_BEGIN+2)    //消息单元运行时信息查询应答
#define EV_ACTION_TIMEOUT               (SKY_WORKFLOW_ID_BEGIN+3)    //工作流超时
#define EV_RESUME_WORKFLOW              (SKY_WORKFLOW_ID_BEGIN+4)    //工作流恢复请求
#define EV_RESUME_WORKFLOW_ACK          (SKY_WORKFLOW_ID_BEGIN+5)    //工作流恢复应答
#define EV_PAUSE_WORKFLOW               (SKY_WORKFLOW_ID_BEGIN+6)    //工作流暂停请求
#define EV_PAUSE_WORKFLOW_ACK           (SKY_WORKFLOW_ID_BEGIN+7)    //工作流暂停应答
#define EV_DELETE_WORKFLOW              (SKY_WORKFLOW_ID_BEGIN+8)    //工作流删除请求
#define EV_DELETE_WORKFLOW_ACK          (SKY_WORKFLOW_ID_BEGIN+9)    //工作流删除应答
#define EV_ROLLBACK_WORKFLOW            (SKY_WORKFLOW_ID_BEGIN+10)    //工作流回滚请求
#define EV_ROLLBACK_WORKFLOW_ACK        (SKY_WORKFLOW_ID_BEGIN+11)    //工作流回滚应答
#define EV_QUERY_WORKFLOW               (SKY_WORKFLOW_ID_BEGIN+12)   //工作流回滚请求
#define EV_QUERY_WORKFLOW_ACK           (SKY_WORKFLOW_ID_BEGIN+13)   //工作流回滚应答

/* ==============sky内部使用的定时器消息id，类型为MESSAGE_TIMER============== */
//注意: message id不能等于0，0作为无效的id值，在trace功能中用到了
#define EV_SKY_SYNC_TIMER               1
#define EV_TIMER_BEGIN                  10
#define EV_TIMER_1                      (EV_TIMER_BEGIN+1)
#define EV_TIMER_2                      (EV_TIMER_BEGIN+2)
#define EV_TIMER_3                      (EV_TIMER_BEGIN+3)
#define EV_TIMER_4                      (EV_TIMER_BEGIN+4)
#define EV_TIMER_5                      (EV_TIMER_BEGIN+5)
#define EV_TIMER_6                      (EV_TIMER_BEGIN+6)
#define EV_TIMER_7                      (EV_TIMER_BEGIN+7)
#define EV_TIMER_8                      (EV_TIMER_BEGIN+8)
#define EV_TIMER_9                      (EV_TIMER_BEGIN+9)
#define EV_TIMER_10                      (EV_TIMER_BEGIN+10)
#define EV_TIMER_11                      (EV_TIMER_BEGIN+11)
#define EV_TIMER_12                      (EV_TIMER_BEGIN+12)
#define EV_TIMER_13                      (EV_TIMER_BEGIN+13)
#define EV_TIMER_14                      (EV_TIMER_BEGIN+14)
#define EV_TIMER_15                      (EV_TIMER_BEGIN+15)
#define EV_TIMER_16                      (EV_TIMER_BEGIN+16)
#define EV_TIMER_17                      (EV_TIMER_BEGIN+17)
#define EV_TIMER_18                      (EV_TIMER_BEGIN+18)
#define EV_TIMER_19                      (EV_TIMER_BEGIN+19)
#define EV_TIMER_20                      (EV_TIMER_BEGIN+20)
#define EV_TIMER_21                      (EV_TIMER_BEGIN+21)
#define EV_TIMER_22                      (EV_TIMER_BEGIN+22)
#define EV_TIMER_23                      (EV_TIMER_BEGIN+23)
#define EV_TIMER_24                      (EV_TIMER_BEGIN+24)
#define EV_TIMER_25                      (EV_TIMER_BEGIN+25)
#define EV_TIMER_26                      (EV_TIMER_BEGIN+26)
#define EV_TIMER_27                      (EV_TIMER_BEGIN+27)
#define EV_TIMER_28                      (EV_TIMER_BEGIN+28)
#define EV_TIMER_29                      (EV_TIMER_BEGIN+29)
#define EV_TIMER_30                      (EV_TIMER_BEGIN+30)
#define EV_TIMER_31                      (EV_TIMER_BEGIN+31)
#define EV_TIMER_32                      (EV_TIMER_BEGIN+32)
#define EV_TIMER_33                      (EV_TIMER_BEGIN+33)


/*================================================================================*/
class MID: public Serializable
{
public:
    MID() {};
    MID(const MID &mid)
    {
        _application = mid._application;
        _process = mid._process; //process = "group" 时表示组播mid，此时unit为组播组名称
        _unit = mid._unit;
    };
    
    MID(const string& unit):
    _unit(unit) {};
    
    MID(const string& process,const string& unit):
    _process(process),_unit(unit) {};
    
    MID(const string& application,const string& process,const string& unit):
    _application(application),_process(process),_unit(unit){};
    
    friend ostream & operator << (ostream& os, const MID& mid)
    {
        os << mid._application + "." + mid._process + "." + mid._unit;
        return os;
    };
    
    MID& operator=(const MID& mid)
    {
        _application = mid._application;
        _process = mid._process;
        _unit = mid._unit;
        return *this;
    };

    int operator == (const MID& mid)
    {
        return (_application == mid._application &&
                _process == mid._process &&
                _unit == mid._unit);
    };
    
    int operator != (const MID& mid)
    {
        return (_application != mid._application ||
                _process != mid._process ||
                _unit != mid._unit);
    };

    string serialize() const 
    {
        return _application + "." + _process + "." + _unit;
    };
    
    bool deserialize(const string &input) 
    {
        stringstream iss(input);    
        getline(iss, _application, '.');
        getline(iss, _process, '.');
        getline(iss, _unit, '.');
        return true;
    };
    
    ~MID() {};
    string _application;
    string _process;
    string _unit;
};

class MessageOption
{
public:
    MessageOption()
    {
        _header_version = 0;
        _body_version = 0;
        _id = 0;
        _type = 0;
        _priority = 0;
    };
    
    MessageOption(MID receiver,uint32 id,uint32 priority,uint32 version)
    {
        Init(receiver,id,priority,version);
    };
    
    MessageOption(string receiver,uint32 id,uint32 priority,uint32 version)
    {
        Init(receiver,id,priority,version);
    };

    void Init(MID receiver,uint32 id,uint32 priority,uint32 version)
    {
        _receiver = receiver;
        _id = id;
        _body_version = version;
        _priority = priority;
        _type = MESSAGE_TYPE_DEFAULT;
        _header_version = 0;
    };

    void Init(string receiver,uint32 id,uint32 priority,uint32 version)
    {
        _receiver = MID(receiver);
        _id = id;
        _body_version = version;
        _priority = priority;
        _type = MESSAGE_TYPE_DEFAULT;
        _header_version = 0;
    };
    
    inline bool isLocal() const
    {
        return (_sender._application == _receiver._application && 
            _sender._process == _receiver._process);
    };
    
    inline void setId(uint32 id)
    {
        _id = id;
    };
    
    inline uint32 id() const
    {
        return _id;
    };

    inline void setType(uint32 type)
    {
        _type = type;
    };
    
    inline uint32 type() const
    {
        return _type;
    };
    
    inline void setVersion(uint32 version)
    {
        _header_version = version;
    };
   
    inline uint32 version() const
    {
        return _body_version;
    };

    inline void setPriority(uint32 priority)
    {
        _priority = priority;
    };
    
    inline uint32 priority() const
    {
        return _priority;
    };
    
    inline void setSender(const MID &mid)
    {
        _sender = mid;
    };

    inline const MID& sender() const
    {
        return _sender;
    };
    
    inline void setReceiver(const MID &mid)
    {
        _receiver = mid;
    };

    inline const MID& receiver() const
    {
        return _receiver;
    };

private:
    uint32 _header_version;
    uint32 _body_version;
    MID    _sender;
    MID    _receiver;
    uint32 _id;
    uint32 _type;
    uint32 _priority;
};

class MessageFrame
{
public:
    MessageFrame() {};

    MessageFrame(const Serializable& object,const MessageOption& opt)
    {
        option = opt;
        data = object.serialize();
    };
    
    MessageFrame(const string& s,const MessageOption& opt)
    {
        option = opt;
        data = s;
    };

    MessageFrame(const Serializable& object,uint32 id)
    {
        option.setType(MESSAGE_TYPE_DEFAULT);
        option.setId(id);
        data = object.serialize();
    };
    
    MessageFrame(const string& s,uint32 id)
    {
        option.setId(id);
        data = s;
    };
    
    string           data;
    MessageOption    option;
};

class MessageHandler
{
public:
    virtual void MessageEntry(const MessageFrame&) = 0;
    virtual STATUS Init() { return ERROR; };
    virtual STATUS Start(const string& name) { return ERROR; };
    virtual STATUS Receive(const MessageFrame&) { return ERROR; };
    virtual ~MessageHandler() {};
};

inline STATUS Send(MessageHandler *m,const Serializable& message,const MessageOption option)
{
    if(!m)
        return ERROR_INVALID_ARGUMENT;
    MessageFrame frame(message,option);
    return m->Receive(frame);
}

inline STATUS Send(MessageHandler *m,const string& data,const MessageOption option)
{
    if(!m)
        return ERROR_INVALID_ARGUMENT;
    MessageFrame frame(data,option);
    return m->Receive(frame);
}

template <class MSG>
MSG GetMsg(const MessageFrame& message)
{
    MSG msg;
    bool result = msg.deserialize(message.data);
    SkyAssert(result);
    return msg;
}

//本结构体仅作示例使用，无实际应用意义
class DemoMessage:public Serializable
{
public:
    DemoMessage() {};
    DemoMessage(const DemoMessage& m) {demo = m.demo;};
    DemoMessage(const string& s):demo(s) {};
    ~DemoMessage() {};
    
    string serialize() const 
    {
        return demo;
    };
    
    bool deserialize(const string &input) 
    {
        demo = input; 
        return true;
    };

    void Print()
    {
        cout << "Demo message: " << demo << endl;
    }
private:
    string demo;
};
#endif

