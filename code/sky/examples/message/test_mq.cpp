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
        
        //创建一个MessageQueue实例
        mq = new MessageQueue(name);
        //将MyHandler注册为该MessageQueue的消息处理器
        mq->SetHandler(this);
        //运行MessageQueue
        return mq->Run();
    }
    
    //这个函数是父类的虚函数，用户必须实现
    STATUS Receive(const MessageFrame& message)
    {
        return mq->Receive(message);
    };
    
    //用户也可以再实现其它样式的接收函数，方便使用   
    STATUS Receive(int id,const Serializable& data)
    {
        //将参数组织为消息之后放入自身的队列中
        MessageFrame message(data,id);
        return mq->Receive(message);
    };

    void DemoRelTimer()
    {
        //演示相对定时器的使用
        TIMER_ID mytid = mq->CreateTimer();
        TimeOut timeout;
        timeout.duration = 1000;
        timeout.msgid = EV_TIMER_1;
        timeout.arg = 45678;
        mq->SetTimer(mytid,timeout);
    }

    void DemoLoopTimer()
    {
        //演示循环定时器的使用
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
                    //从收到的消息字符串中反向序列化，得到所需的消息体
                    //基本数据类型反序列化比较简单，基本不会出错，所以能够使用构造函数直接生成
                    SkyPtr sp(message.data);

                    //使用消息体，这里得到的是一个内存地址
                    Info *p = (Info *)(sp.value);
                    cout << "MyHandler gets struct Info" << endl;
                    p->Print();
                }
                break;
                               
            case EV_MESSAGE_INT_NUMBER:
                {
                    //收到一个整数消息
                    SkyInt si(message.data);
                    cout << "MyHandler gets frame.data = " << message.data << endl;
                    cout << "MyHandler gets SkyInt = " << si << endl;
                    cout << "MyHandler gets int = " << si.value << endl;
                }
                break;

            case EV_MESSAGE_DEMO:
                {
                    //收到一个自定义消息，需要显式的序列化
                    DemoMessage dm;
                    if(true == dm.deserialize(message.data))
                    {
                        //序列化过程是可能会出错的，建议判断一下返回值再使用
                        cout << "MyHandler gets DemoMessage" << endl;
                        dm.Print();
                    }
                }
                break;
                
            case EV_TIMER_1:
                {
                    //收到定时器超时消息
                    cout << "MyHandler gets EV_TIMER_1!" << endl;
                    //得到当前超时的定时器控制块id
                    cout << "EV_TIMER_1 tid = " << mq->GetTimerId() << endl;
                    //得到定时器参数，就是当初设置定时器时传入的值
                    cout << "EV_TIMER_1 arg = " << mq->GetTimerArg() << endl;
                }
                break;
                
            case EV_TIMER_LOOP:
                {
                    //收到循环定时器超时消息
                    cout << "MyHandler gets EV_TIMER_LOOP!" << endl;
                    //得到当前超时的定时器控制块id
                    cout << "EV_TIMER_LOOP tid = " << mq->GetTimerId() << endl;
                    //得到定时器参数，就是当初设置定时器时传入的值
                    cout << "EV_TIMER_LOOP arg = " << mq->GetTimerArg() << endl;
                }
                break;
            default:
                cout << "unknown message!" << endl;
                break;
        }
    };
private:
    //私有的消息队列成员，负责消息收发
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
    //初始化sky库
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
    //给SimpleManager发个整数消息先
    MessageFrame simple_message(SkyInt(12345),EV_MESSAGE_INT_NUMBER);
    psim->Receive(simple_message);
    
    //实例化一个MyHandler
    MyHandler *h = new MyHandler();
    if(SUCCESS != h->Start("myhandler"))
    {
        printf("MyHandler start failed!\n");
        return -1;
    }

    //向MyHandler发送一个int变量
    SkyInt z = 123;
    h->Receive(EV_MESSAGE_INT_NUMBER, z);

    //向MyHandler发送一个结构体x的内存地址
    Info x;
    x.a = 1;
    x.b = 2;
    x.c = 3;
    SkyPtr p1(&x);
    h->Receive(EV_MESSAGE_INFO_ADDR, p1);

    //向MyHandler发送一个结构体y的内存地址
    Info y;
    y.a = 4;
    y.b = 5;
    y.c = 6;   
    SkyPtr p2(&y);
    h->Receive(EV_MESSAGE_INFO_ADDR, p2);

    //向MyHandler发送一个自定义的消息，这个消息对象会被拷贝发送
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

