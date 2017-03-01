#include "test_pub.h"

class TestServer: public MessageHandler
{
public:
    TestServer()
    {
        //创建并启动消息单元，消息单元的名称最好用宏统一定义
        mu = new MessageUnit(TEST_SERVER);
        mu->SetHandler(this);
        mu->Run();
        mu->Register();
        mu->Print();
    };

    ~TestServer()
    {
        if(mu)
            delete mu;
    };
    
    //如果是消息单元，这个函数其实也可以不实现，别人可以根据消息单元的名字Send消息过来
    //不过如果有了这个函数的话，别人也可以在非消息单元上下文给我发送消息了
    STATUS Receive(const MessageFrame& message)
    {
        return mu->Receive(message);
    };
    
    //TestClient有一个上电通知接口
    void PowerOn()
    {
        //给自己发一个上电通知消息
        DemoMessage msg("Power on!");
        MessageOption option(mu->name(),EV_POWER_ON,0,0);
        mu->Send(msg,option);
    }

    void JoinGroup()
    {
        //加入一个组播组
        mu->JoinMcGroup(TEST_GROUP);
    }

    void ExitGroup()
    {
        //退出一个组播组
        mu->ExitMcGroup(TEST_GROUP);
    }
    
    void BroadcastInfo(const Serializable& message)
    {
        //向组播组发送组播消息
        MID target("group",TEST_GROUP);
        MessageOption option(target,EV_BROADCAST_INFO,0,0);
        mu->Send(message, option);
    }
        
    void MessageEntry(const MessageFrame& message)
    {       
        switch(mu->get_state())
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
                                    
                        //重设一下状态
                        mu->set_state(S_Work);
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
                    case EV_INFO_REQ:
                    {
                        cout << "Server receives EV_INFO_REQ!" << endl;
                        cout << "EV_INFO_REQ = " << message.data << endl;
                        //将收到的消息反序列化
                        Information inforeq;
                        inforeq.deserialize(message.data);

                        //构造应答消息
                        Information infoack;
                        infoack.content = "This is an information ack!";
                        infoack.sequence = inforeq.sequence + 1;
                        
                        //给消息发送方回复消息，通过option直接获得发送方MID
                        MessageOption option(message.option.sender(),EV_INFO_ACK,0,0);
                        mu->Send(infoack,option);
                    }
                    break;
                    
                    case EV_BROADCAST_INFO:
                    {
                        cout << "Server receives EV_BROADCAST_INFO!" << endl;
                        cout << "EV_BROADCAST_INFO = " << message.data << endl;
                    }
                    break;
                    
                    case EV_INFO_STRING:
                    {
                        cout << "Server receives EV_INFO_STRING!" << endl;
                        cout << "EV_INFO_STRING = " << message.data << endl;
                        //给消息发送方回复消息，通过option直接获得发送方MID
                        MessageOption option(message.option.sender(),EV_INFO_STRING,0,0);
                        mu->Send(message.data,option);
                    }
                    break;
                    
                    default:
                    //cout << "Server receives a message!" << endl;
                    break;
                }
            }
            break;

            default:
                break;

        }
    
    }
private:
    MessageUnit *mu;
};

TestServer *pserver = NULL;
int main()
{ 
    SkyConfig config;
    config.messaging.application = "testcloud";
    config.messaging.process = "server";
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

    pserver = new TestServer();
    //通知上电!
    pserver->PowerOn();

    Shell("test_mu_server-> ");
    while(1)
    {
        sleep(1);
    }
    return 0;
}

void DbgJoinGroup()
{
    pserver->JoinGroup();
}
DEFINE_DEBUG_FUNC(DbgJoinGroup);

void DbgExitGroup()
{
    pserver->ExitGroup();
}
DEFINE_DEBUG_FUNC(DbgExitGroup);

