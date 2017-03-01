#include "test_pub.h"

class TestClient: public MessageHandlerTpl<MessageUnit>
{
public:
    //TestClient��һ���ϵ�֪ͨ�ӿ�
    void PowerOn()
    {
        //���Լ���һ���ϵ�֪ͨ��Ϣ
        DemoMessage msg("Power on!");
        MessageOption option(m->name(),EV_POWER_ON,0,0);
        m->Send(msg,option);
    }

    void SendString(const char* data)
    {
        //������,test_server�����ǲ���ͬһ��process�������ͬһ��application��
        //��˹���test_serverd��MIDʱ��ֻҪ��д�Է���process���ƺ�unit���Ƽ���
        MID receiver("testcloud",SERVER_PROCESS,TEST_SERVER);
        //������Ϣ����ѡ��ʱ����Ҫ��Ҫ��д���շ�MID�Լ���Ϣid
        //�汾�ź���Ϣ���ȼ������ֶ���δ��ʽ֧�֣���0����
        MessageOption option(receiver,EV_INFO_STRING,0,0);
        m->Send(data,option);
    }
    
    void RequestInfo(int seq = 0)
    {
        Information inforeq;
        inforeq.content = "This is an information request!";
        inforeq.sequence = seq;
        //������,test_server�����ǲ���ͬһ��process�������ͬһ��application��
        //��˹���test_serverd��MIDʱ��ֻҪ��д�Է���process���ƺ�unit���Ƽ���
        MID receiver("testcloud",SERVER_PROCESS,TEST_SERVER);
        //������Ϣ����ѡ��ʱ����Ҫ��Ҫ��д���շ�MID�Լ���Ϣid
        //�汾�ź���Ϣ���ȼ������ֶ���δ��ʽ֧�֣���0����
        MessageOption option(receiver,EV_INFO_REQ,0,0);
        m->Send(inforeq,option);
    }

    void JoinGroup()
    {
        //����һ���鲥��
        m->JoinMcGroup(TEST_GROUP);
    }

    void ExitGroup()
    {
        //�˳�һ���鲥��
        m->ExitMcGroup(TEST_GROUP);
    }
    
    void BroadcastInfo(const Serializable& message)
    {
        //���鲥�鷢���鲥��Ϣ
        //�����鲥MIDʱ��process�������group����unit�����鲥������
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
                        //�յ��ϵ���Ϣ
                        DemoMessage dm;
                        dm.deserialize(message.data);
                        dm.Print();

                        //�����鲥��
                        JoinGroup();
                        
                        //��TestServer��Ϣ��Ԫ������Ϣ
                        RequestInfo();
                        
                        //����һ��״̬
                        m->set_state(S_Work);
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
    //֪ͨ�ϵ�!
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

