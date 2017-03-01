#include "sky.h"

#define EV_MESSAGE_INT_NUMBER    1001
#define EV_MESSAGE_INFO_ADDR     1002
#define EV_MESSAGE_DEMO          1003
#define EV_TIMER_LOOP            2002

struct Info
{
    int a;
    int b;
    int c;
    void Print()
    {
        cout << "a = " << a << endl;
        cout << "b = " << b << endl;
        cout << "c = " << c << endl;
    }
};

class MyHandler:public MessageHandler
{
public:
    MyHandler()
    {
        mq = NULL;
    };

    ~MyHandler()
    {
        if(mq)
            delete mq;
    };
    
    STATUS Start(const string& name)
    {
        if(mq)
        {
            return ERROR_DUPLICATED_OP;
        }
        
        //����һ��MessageQueueʵ��
        mq = new MessageQueue(name);
        //��MyHandlerע��Ϊ��MessageQueue����Ϣ������
        mq->SetHandler(this);
        //����MessageQueue
        return mq->Run();
    }
    
    //��������Ǹ�����麯�����û�����ʵ��
    STATUS Receive(const MessageFrame& message)
    {
        return mq->Receive(message);
    };
    
    //�û�Ҳ������ʵ��������ʽ�Ľ��պ���������ʹ��   
    STATUS Receive(int id,const Serializable& data)
    {
        //��������֯Ϊ��Ϣ֮���������Ķ�����
        MessageFrame message(data,id);
        return mq->Receive(message);
    };

    void DemoRelTimer()
    {
        //��ʾ��Զ�ʱ����ʹ��
        TIMER_ID mytid = mq->CreateTimer();
        TimeOut timeout;
        timeout.duration = 1000;
        timeout.msgid = EV_TIMER_1;
        timeout.arg = 45678;
        mq->SetTimer(mytid,timeout);
    }

    void DemoLoopTimer()
    {
        //��ʾѭ����ʱ����ʹ��
        TimeOut timeout;
        TIMER_ID mytid2 = mq->CreateTimer();
        timeout.type = LOOP_TIMER;
        timeout.duration = 2000;
        timeout.msgid = EV_TIMER_LOOP;
        timeout.arg = 123456;
        mq->SetTimer(mytid2,timeout);
    }
    
    void MessageEntry(const MessageFrame& message)
    {
        //cout << "MyHandler gets message id = " << frame.option.id() << endl;
        switch(message.option.id())
        {
            case EV_MESSAGE_INFO_ADDR:
                {
                    //���յ�����Ϣ�ַ����з������л����õ��������Ϣ��
                    //�����������ͷ����л��Ƚϼ򵥣�����������������ܹ�ʹ�ù��캯��ֱ������
                    SkyPtr sp(message.data);

                    //ʹ����Ϣ�壬����õ�����һ���ڴ��ַ
                    Info *p = (Info *)(sp.value);
                    cout << "MyHandler gets struct Info" << endl;
                    p->Print();
                }
                break;
                               
            case EV_MESSAGE_INT_NUMBER:
                {
                    //�յ�һ��������Ϣ
                    SkyInt si(message.data);
                    cout << "MyHandler gets frame.data = " << message.data << endl;
                    cout << "MyHandler gets SkyInt = " << si << endl;
                    cout << "MyHandler gets int = " << si.value << endl;
                }
                break;

            case EV_MESSAGE_DEMO:
                {
                    //�յ�һ���Զ�����Ϣ����Ҫ��ʽ�����л�
                    DemoMessage dm;
                    if(true == dm.deserialize(message.data))
                    {
                        //���л������ǿ��ܻ����ģ������ж�һ�·���ֵ��ʹ��
                        cout << "MyHandler gets DemoMessage" << endl;
                        dm.Print();
                    }
                }
                break;
                
            case EV_TIMER_1:
                {
                    //�յ���ʱ����ʱ��Ϣ
                    cout << "MyHandler gets EV_TIMER_1!" << endl;
                    //�õ���ǰ��ʱ�Ķ�ʱ�����ƿ�id
                    cout << "EV_TIMER_1 tid = " << mq->GetTimerId() << endl;
                    //�õ���ʱ�����������ǵ������ö�ʱ��ʱ�����ֵ
                    cout << "EV_TIMER_1 arg = " << mq->GetTimerArg() << endl;
                }
                break;
                
            case EV_TIMER_LOOP:
                {
                    //�յ�ѭ����ʱ����ʱ��Ϣ
                    cout << "MyHandler gets EV_TIMER_LOOP!" << endl;
                    //�õ���ǰ��ʱ�Ķ�ʱ�����ƿ�id
                    cout << "EV_TIMER_LOOP tid = " << mq->GetTimerId() << endl;
                    //�õ���ʱ�����������ǵ������ö�ʱ��ʱ�����ֵ
                    cout << "EV_TIMER_LOOP arg = " << mq->GetTimerArg() << endl;
                }
                break;
            default:
                cout << "unknown message!" << endl;
                break;
        }
    };
private:
    //˽�е���Ϣ���г�Ա��������Ϣ�շ�
    MessageQueue *mq;
};
   
class SimpleHandler: public MessageHandlerTpl<MessageQueue>
{
    void MessageEntry(const MessageFrame& message)
    { 
        printf("SimpleHandler receive a message, content:\n%s\n",message.data.c_str());
    };
};

int main()
{
    //��ʼ��sky��
    SkyConfig config;
    config.messaging.application = "mycloud";
    config.messaging.process = "mutest1";
    //config.messaging.broker_url = "localhost";
    //config.messaging.broker_port = 5672;
    //config.messaging.broker_option = "";
    ConnectConfig connect;
    connect.broker_url = "localhost";
    connect.broker_port = 5672;
    connect.broker_option = "";
    config.messaging.connects.push_back(connect);
    if(SUCCESS != SkyInit(config))
    {
        printf("sky init failed!\n");
        return -1;
    }

    SimpleHandler *psim = new SimpleHandler;
    psim->Start("SimpleHandler");
    //��SimpleManager����������Ϣ��
    MessageFrame simple_message(SkyInt(12345),EV_MESSAGE_INT_NUMBER);
    psim->Receive(simple_message);
    
    //ʵ����һ��MyHandler
    MyHandler *h = new MyHandler();
    if(SUCCESS != h->Start("myhandler"))
    {
        printf("MyHandler start failed!\n");
        return -1;
    }

    //��MyHandler����һ��int����
    SkyInt z = 123;
    h->Receive(EV_MESSAGE_INT_NUMBER, z);

    //��MyHandler����һ���ṹ��x���ڴ��ַ
    Info x;
    x.a = 1;
    x.b = 2;
    x.c = 3;
    SkyPtr p1(&x);
    h->Receive(EV_MESSAGE_INFO_ADDR, p1);

    //��MyHandler����һ���ṹ��y���ڴ��ַ
    Info y;
    y.a = 4;
    y.b = 5;
    y.c = 6;   
    SkyPtr p2(&y);
    h->Receive(EV_MESSAGE_INFO_ADDR, p2);

    //��MyHandler����һ���Զ������Ϣ�������Ϣ����ᱻ��������
    DemoMessage msg("Hello");
    h->Receive(EV_MESSAGE_DEMO, msg);

    h->DemoRelTimer();

    h->DemoLoopTimer();

    Shell("test_mq-> ");
    while(1)
    {
        sleep(1);
    }
    return 0;
}

