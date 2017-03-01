#include "sky.h"

//��ʾ�߳���ں���
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

//��һ�ִ����̵߳ķ���:StartThread
//StartThread�Ƕ�pthread_create�ļ򵥷�װ������һ����ʵʱ���ȼ��̣߳�
//���û������ԭ�򣬽���ʹ������ӿ��������̣߳�ʹ��KillThreadɱ���߳�
//��ջ��С�ݲ�����֧���û��Լ����ã���Ϊ�ڷ�Ƕ��ʽ���ϣ�ָ����ջ��С���岻��
void test1(int x)
{
    int *count = new int(x);
    //����һ�����߳�
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
    //ɱ���½����߳�
    KillThread(tid);
}
DEFINE_DEBUG_FUNC(test1);

//�ڶ��ִ����̵߳ķ���:����һ���Լ����߳��࣬��Thread����̳�
//�����������ķ�ʽ��һ��������ò��ţ�ʹ��StartThread/KillThread����������������������
#define EV_TIMER_LOOP 1000
#define EV_POWER_ON   1001
class MyThread:public Thread
{
public:
    //����һ���߳�ʵ���ĺ���
    static MyThread* CreateMyThread(const string& name,void *arg)
    {
        MyThread *t = new MyThread(name);
        t->Start(arg);
        return t;
    };

    //ʵ��һ��Setup����(��ѡ),�ú��������߳���ں���֮ǰ�����̵߳������ı�����
    void Setup()
    {
        cout << "A new thread! id=" << Id() <<endl;
    };

    //�û��̵߳�������������Ҫ�û�����д
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

//��ʾ���������̴߳�����ʽ��ʽ
void test2(int x)
{
    int test = x;
    //����������һ���û��߳�ʵ������һ�������ĵ�ַ���ݸ����߳�
    MyThread *t = MyThread::CreateMyThread("test2",&test);
    sleep(6);
    delete t;
}
DEFINE_DEBUG_FUNC(test2);

//��ʾ����Ϣ��Ԫ��������δ����͹����µ��߳�
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
        //���Լ���һ���ϵ�֪ͨ��Ϣ
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
                //����һ��ѭ����ʱ���������Եش�����ɾ���߳�
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
                    //ɱ��һ�������е��߳�
                    if (SUCCESS != KillThread(demotid))
                    {
                        printf("failed to kill thread!\n");
                    }
                }

                //����һ�����̣߳������߳���ڴ��ݲ���
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

//����һ���̹߳�����Ϣ��Ԫ
void test3()
{
    ThreadManager *p = new ThreadManager();
    p->PowerOn();
}
DEFINE_DEBUG_FUNC(test3);

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



