#ifndef SKY_MESSAGE_UNIT_H
#define SKY_MESSAGE_UNIT_H
#include "message_queue.h"

class MessageUnit:public MessageQueue
{
public:
    MessageUnit(const string& name):MessageQueue(name) {};
    virtual ~MessageUnit();
    STATUS Register();
    STATUS Send(const Serializable& data,MessageOption& option);
    STATUS Send(const string& data,MessageOption& option);
    STATUS JoinMcGroup(const string& group);
    STATUS ExitMcGroup(const string& group);
    STATUS GetSelfMID(MID &self);
    string GetSelfMID( );
    
    void Print();
    static  STATUS  SendFromTmp(const Serializable& data,MessageOption& option);
    static  STATUS  SendFromTmp(const string& data, MessageOption& option);
private:
    STATUS CompleteOption(MessageOption& option);
    MessageUnit():MessageQueue("error") {}; //不允许构造无名的消息单元
};

string TempUnitName(const string& prefix = "unit");
const string& ApplicationName();
const string& ProcessName();
STATUS AddKeyMu(const string& name);
STATUS DelKeyMu(const string& name);
#endif


