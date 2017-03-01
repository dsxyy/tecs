#include "test_pub.h"

class TestClient: public MessageHandlerTpl<MessageUnit>
{
public:
    //TestClient有一个上电通知接口
    void PowerOn()
    {
        //给自己发一个上电通知消息
        DemoMessage msg("Power on!");
        MessageOption option(m->name(),EV_POWER_ON,0,0);
        m->Send(msg,option);
    }

    void SendString(const char* data)
    {
        //本例中,test_server和我们不在同一个process里，但是在同一个application中
        //因此构造test_serverd的MID时，只要填写对方的process名称和unit名称即可
        MID receiver("testcloud",SERVER_PROCESS,TEST_SERVER);
        //构造消息发送选项时，主要是要填写接收方MID以及消息id
        //版本号和消息优先级两个字段暂未正式支持，填0即可
        MessageOption option(receiver,EV_INFO_STRING,0,0);
        m->Send(data,option);
    }
    
    void RequestInfo(int seq = 0)
    {
        Information inforeq;
        inforeq.content = "This is an information request!";
        inforeq.sequence = seq;
        //本例中,test_server和我们不在同一个process里，但是在同一个application中
        //因此构造test_serverd的MID时，只要填写对方的process名称和unit名称即可
        MID receiver("testcloud",SERVER_PROCESS,TEST_SERVER);
        //构造消息发送选项时，主要是要填写接收方MID以及消息id
        //版本号和消息优先级两个字段暂未正式支持，填0即可
        MessageOption option(receiver,EV_INFO_REQ,0,0);
        m->Send(inforeq,option);
    }

    void JoinGroup()
    {
        //加入一个组播组
        m->JoinMcGroup(TEST_GROUP);
    }

    void ExitGroup()
    {
        //退出一个组播组
        m->ExitMcGroup(TEST_GROUP);
    }
    
    void BroadcastInfo(const Serializable& message)
    {
        //向组播组发送组播消息
        //构造组播MID时，process必须等于group，而unit则是组播组名称
        MID target("group",TEST_GROUP);
        MessageOption option(target,EV_BROADCAST_INFO,0,0);
        m->Send(message, option);
    }
        
    void MessageEntry(const MessageFrame& message)
    {       
        switch(m->get_state())
        {
            case S_Startup:
            {
                switch(message.option.id())
                {
                    case EV_POWER_ON:
                    {
                        //收到上电消息
                        DemoMessage dm;
                        dm.deserialize(message.data);
                        dm.Print();

                        //加入组播组
                        JoinGroup();
                        
                        //给TestServer消息单元发送消息
                        RequestInfo();
                        
                        //重设一下状态
                        m->set_state(S_Work);
                        break;
                    }
                }
            }
            break;

            case S_Work:
            {
                //根据收到的消息id进行处理
                switch(message.option.id())
                {
                    case EV_INFO_ACK:
                    {
                        cout << "Client receives EV_INFO_ACK!" << endl;
                        cout << "EV_INFO_ACK = " << message.data << endl;
                    }
                    break;
                    
                    case EV_BROADCAST_INFO:
                    {
                        cout << "Client receives EV_BROADCAST_INFO!" << endl;
                        cout << "EV_BROADCAST_INFO = " << message.data << endl;
                    }
                    break;
                    
                    case EV_INFO_STRING:
                    {
                        cout << "Client receives EV_INFO_STRING!" << endl;
                        cout << "EV_INFO_STRING = " << message.data << endl;
                    }
                    break;
                    
                    default:
                    //cout << "Client receives a message!" << endl;
                    break;
                }
            }
            break;

            default:
                break;

        }
    
    }
private:
    //MessageUnit *mu;
};

TestClient *pclient = NULL;
int main()
{ 
    SkyConfig config;
    config.messaging.application = "testcloud";
    config.messaging.process = "client";
    //config.messaging.broker_url = "localhost";
    //config.messaging.broker_port = 5672;
    //config.messaging.broker_option = "";
    ConnectConfig connect;
    connect.broker_url = "localhost";
    connect.broker_port = 5672;
    connect.broker_option = "{reconnect:True}";
    config.messaging.connects.push_back(connect);
    if(SUCCESS != SkyInit(config))
    {
        printf("sky init failed!\n");
        return -1;
    }

    pclient = new TestClient();
    pclient->Start(TEST_CLIENT);
    //通知上电!
    pclient->PowerOn();

    Shell("test_mu_client-> ");
    while(1)
    {
        sleep(1);
    }
    return 0;
}

void DbgSendInfoReq(int seq = 0)
{
    pclient->RequestInfo(seq);
}
DEFINE_DEBUG_FUNC(DbgSendInfoReq);

void DbgBcMessage()
{
    DemoMessage dm("This is a demo multicast message!");
    pclient->BroadcastInfo(dm);
}
DEFINE_DEBUG_FUNC(DbgBcMessage);

void DbgSendString(const char* data)
{
    if(!data)
        pclient->SendString("nothing");
    else
        pclient->SendString(data);
}
DEFINE_DEBUG_FUNC(DbgSendString);

