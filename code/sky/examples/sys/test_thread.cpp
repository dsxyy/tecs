#include "sky.h"

//演示线程入口函数
void *Hello(void *arg)
{
    int *p = static_cast<int *>(arg);
    int count = *p;
    printf("Hello thread 0x%lx is running!\n",pthread_self());
    for (int i=0;i< count;i++)
    {
        printf("hello, world! i = %d\n",i);
        sleep(2);
    }
    return NULL;
}

//第一种创建线程的方法:StartThread
//StartThread是对pthread_create的简单封装，创建一个非实时优先级线程，
//如果没有特殊原因，建议使用这个接口来创建线程，使用KillThread杀死线程
//堆栈大小暂不考虑支持用户自己设置，因为在非嵌入式场合，指定堆栈大小意义不大
void test1(int x)
{
    int *count = new int(x);
    //创建一个新线程
    THREAD_ID tid = StartThread("hello",(ThreadEntry)Hello,(void *)count);
    if (INVALID_THREAD_ID != tid)
    {
        printf("A new thread created! tid = 0x%lx\n",tid);
    }
    else
    {
        printf("Failed to create new thread!\n");
    }

    sleep(5);
    //杀死新建的线程
    KillThread(tid);
}
DEFINE_DEBUG_FUNC(test1);

//第二种创建线程的方法:定义一个自己的线程类，从Thread基类继承
//这是面向对象的方式，一般情况下用不着，使用StartThread/KillThread基本可以满足绝大多数需求
#define EV_TIMER_LOOP 1000
#define EV_POWER_ON   1001
class MyThread:public Thread
{
public:
    //创建一个线程实例的函数
    static MyThread* CreateMyThread(const string& name,void *arg)
    {
        MyThread *t = new MyThread(name);
        t->Start(arg);
        return t;
    };

    //实现一个Setup函数(可选),该函数会在线程入口函数之前在新线程的上下文被调用
    void Setup()
    {
        cout << "A new thread! id=" << Id() <<endl;
    };

    //用户线程的主处理函数，需要用户来编写
    void Execute(void* arg)
    {
        int *p = static_cast<int *>(arg);
        printf("The content of arg is %d\n",*p);
        while (1)
        {
            //cout << "This is my thread! id=" << Id() <<endl;
            sleep(1);
        }
    };
    
private:
    MyThread() {};
    MyThread(const string& name):Thread(name) {};
    DISALLOW_COPY_AND_ASSIGN(MyThread);
};

//演示面向对象的线程创建方式方式
void test2(int x)
{
    int test = x;
    //创建并启动一个用户线程实例，将一个变量的地址传递给该线程
    MyThread *t = MyThread::CreateMyThread("test2",&test);
    sleep(6);
    delete t;
}
DEFINE_DEBUG_FUNC(test2);

//演示在消息单元上下文如何创建和管理新的线程
class ThreadManager:public MessageHandler
{
public:
    ThreadManager()
    {
        demotid = INVALID_THREAD_ID;
        mu = new MessageUnit("thread_handler");
        mu->SetHandler(this);
        mu->Register();
        mu->Run();
    };

    ~ThreadManager()
    {
        if (mu)
            delete mu;
    };

    void PowerOn()
    {
        //给自己发一个上电通知消息
        DemoMessage demo("Power on!");
        MessageOption option(mu->name(),EV_POWER_ON,0,0);
        MessageFrame message(demo,option);
        mu->Receive(message);
    };

    void MessageEntry(const MessageFrame& message)
    {
        switch (message.option.id())
        {
            case EV_POWER_ON:
            {
                //设置一个循环定时器，周期性地创建和删除线程
                TimeOut timeout;
                mytid = mu->CreateTimer();
                timeout.type = LOOP_TIMER;
                timeout.duration = 2000;
                timeout.msgid = EV_TIMER_LOOP;
                timeout.arg = 123456;
                mu->SetTimer(mytid,timeout);
                printf("ThreadManager power on!\n");
                break;
            }
            
            case EV_TIMER_LOOP:
            {
                printf("----------EV_TIMER_LOOP----------\n");
                if (demotid != INVALID_THREAD_ID)
                {
                    //杀死一个运行中的线程
                    if (SUCCESS != KillThread(demotid))
                    {
                        printf("failed to kill thread!\n");
                    }
                }

                //创建一个新线程，并给线程入口传递参数
                int *arg = new int(12345);
                demotid = StartThreadEx("demotask",3,0,(ThreadEntry)Hello,(void *)arg);
                if (INVALID_THREAD_ID == demotid)
                {
                    printf("failed to start thread!\n");
                }
                break;
            }
            
            default:
                cout << "unknown message!" << endl;
                break;
        }
    };

private:
    TIMER_ID        mytid;
    THREAD_ID       demotid;
    MessageUnit     *mu;
};

//创建一个线程管理消息单元
void test3()
{
    ThreadManager *p = new ThreadManager();
    p->PowerOn();
}
DEFINE_DEBUG_FUNC(test3);

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
    connect.broker_option = "{reconnect:True}";
    
    config.messaging.connects.push_back(connect);
    if(SUCCESS != SkyInit(config))
    {
        printf("sky init failed!\n");
        return -1;
    }
    
    Shell("test_thread-> ");
    while (1)
    {
        sleep(1);
    }
    return 0;
}



