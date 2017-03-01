#include "test_pub.h"

class TestServer: public MessageHandler
{
public:
    TestServer()
    {
        //������������Ϣ��Ԫ����Ϣ��Ԫ����������ú�ͳһ����
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
    
    //�������Ϣ��Ԫ�����������ʵҲ���Բ�ʵ�֣����˿��Ը�����Ϣ��Ԫ������Send��Ϣ����
    //�������������������Ļ�������Ҳ�����ڷ���Ϣ��Ԫ�����ĸ��ҷ�����Ϣ��
    STATUS Receive(const MessageFrame& message)
    {
        return mu->Receive(message);
    };
    
    //TestClient��һ���ϵ�֪ͨ�ӿ�
    void PowerOn()
    {
        //���Լ���һ���ϵ�֪ͨ��Ϣ
        DemoMessage msg("Power on!");
        MessageOption option(mu->name(),EV_POWER_ON,0,0);
        mu->Send(msg,option);
    }

    void JoinGroup()
    {
        //����һ���鲥��
        mu->JoinMcGroup(TEST_GROUP);
    }

    void ExitGroup()
    {
        //�˳�һ���鲥��
        mu->ExitMcGroup(TEST_GROUP);
    }
    
    void BroadcastInfo(const Serializable& message)
    {
        //���鲥�鷢���鲥��Ϣ
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
                        //�յ��ϵ���Ϣ
                        DemoMessage dm;
                        dm.deserialize(message.data);
                        dm.Print();

                        //�����鲥��
                        JoinGroup();
                                    
                        //����һ��״̬
                        mu->set_state(S_Work);
                        break;
                    }
                }
            }
            break;

            case S_Work:
            {
                //�����յ�����Ϣid���д���
                switch(message.option.id())
                {
                    case EV_INFO_REQ:
                    {
                        cout << "Server receives EV_INFO_REQ!" << endl;
                        cout << "EV_INFO_REQ = " << message.data << endl;
                        //���յ�����Ϣ�����л�
                        Information inforeq;
                        inforeq.deserialize(message.data);

                        //����Ӧ����Ϣ
                        Information infoack;
                        infoack.content = "This is an information ack!";
                        infoack.sequence = inforeq.sequence + 1;
                        
                        //����Ϣ���ͷ��ظ���Ϣ��ͨ��optionֱ�ӻ�÷��ͷ�MID
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
                        //����Ϣ���ͷ��ظ���Ϣ��ͨ��optionֱ�ӻ�÷��ͷ�MID
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
    //֪ͨ�ϵ�!
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

