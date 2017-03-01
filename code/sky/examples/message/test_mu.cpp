#include "sky.h"

#define NETWORK_MANAGER "NetworkManager"
#define STORAGE_MANAGER "StorageManager"

#define EV_POWER_ON          1001
#define EV_POWER_OFF         1002
#define EV_STORAGE_NOTIFY    1003
#define EV_STORAGE_REQ       1004
#define EV_STORAGE_ACK       1005
#define EV_STORAGE_INT32     1006
#define EV_BROADCAST_INFO    1007
#define EV_SIMPLE_NOTICE     1008
#define EV_PLEASE_SLEEP      1013

//使用MessageHandler模板定义一个自己的Handler
//使用模板的好处是不需要自己再去定义、创建、启动、销毁消息单元了
//如果觉得模板的方式不好用，也可以自己写一个，参见下面的StorageManager
//SimpleHandler的用法参见main函数
class SimpleHandler: public MessageHandlerTpl<MessageUnit>
{
public:
    //使用了模板之后，你要做的事情就只剩一个:编写消息处理函数
    void MessageEntry(const MessageFrame& message)
    {
        printf("SimpleHandler receive a message, content:\n%s\n",message.data.c_str());
    };
};
    
//这是使用模板创建的功能更多的MessageHandler演示
class NetworkManager: public MessageHandlerTpl<MessageUnit>
{
public:   
    static NetworkManager* GetInstance()
    {
        if(NULL == instance)
        {
            instance = new NetworkManager(); 
        }
        return instance;
    };
        
    //NetworkManager有一个上电通知接口
    void PowerOn()
    {
        //给自己发一个上电通知消息
        DemoMessage msg("Power on!");
        MessageOption option(NETWORK_MANAGER,EV_POWER_ON,0,0);
        //构造上电通知消息帧
        MessageFrame frame(msg,option);
        //让mu收下上电通知消息帧
        m->Receive(frame);
    };

    void JoinNetworkGroup()
    {
        //加入一个名为net1的组播组
        m->JoinMcGroup("net1");
    };

    void BroadcastNetworkInfo(const Serializable& message)
    {
        //向名为net1的组播组发送组播消息
        MID target("group","net1");
        MessageOption option(target,EV_BROADCAST_INFO,0,0);
        m->Send(message, option);
    };
        
    void ShowRemainedTime()
    {
        T_TimeValue remained;
        //获取一个定时器距离超时时刻剩余的时间
        if(SUCCESS != m->GetRemainedTime(demo_tid_1,&remained))
        {
            printf("GetRemainedTime failed\n");
            return;
        }
        printf("demo_tid_1 remained %d seconds, %d miliseconds\n",remained.second,remained.milisecond);
    }

    void MessageEntry(const MessageFrame& message)
    {       
        //根据收到的消息id进行处理
        switch(message.option.id())
        {
            case EV_POWER_ON:
            {
                //收到上电消息
                DemoMessage dm;
                dm.deserialize(message.data);
                dm.Print();

                //重设一下状态
                m->set_state(1);

                //给另外一个消息单元发送消息
                DemoMessage msg("A message from NetworkManager to StorageManager!");
                //由于接收方和我们在同一个进程里，构造MessageOption时无需填写application和process
                //甚至不必再去构造MID了，直接填入对方的unit名称即可
                MessageOption option(STORAGE_MANAGER,EV_STORAGE_NOTIFY,0,0);
                //上述代码行等效于MessageOption option(MID(STORAGE_MANAGER),EV_STORAGE_NOTIFY,0,0);
                m->Send(msg,option);

                //下面演示定时器的使用
                TimeOut timeout;

                //设置一个普通相对定时器
                demo_tid_1 = m->CreateTimer();
                timeout.duration = 1000 * 3;
                timeout.msgid = EV_TIMER_1;
                timeout.arg = 12345;
                timeout.str_arg = "hello,timer";
                //type字段不填写时，默认就是普通相对定时器
                //timeout.type = RELATIVE_TIMER;  
                m->SetTimer(demo_tid_1,timeout);
                //注意:超时的定时器不会自动释放，需要用户自己去kill这个tid!!!
                //所以应该把tid保存起来，建议放在私有成员变量中
                
                //设置一个循环定时器
                demo_tid_2 = m->CreateTimer();
                timeout.duration = 1000;
                timeout.msgid = EV_TIMER_2;
                timeout.arg = 67890;
                //必须将type字段填写为LOOP_TIMER!!!
                timeout.type = LOOP_TIMER;
                m->SetTimer(demo_tid_2,timeout);
                count = 0;
                //todo: 绝对定时器目前暂未发现需求，待正式实现后补充演示用例
                break;
            }

            //处理定时器超时消息
            case EV_TIMER_1:
            {
                /*
                如果还想用定时器，这里可以继续Set它，并且可以Set成其它type的定时器。
                也就是说，只要你Create了一个定时器，得到了tid资源，就可以任意将其
                Set成普通相对定时器或者循环定时器或者绝对定时器，
                当然了，只有最后一次Set才是会真正生效的!!!

                如果只是想让定时器停止计时，并且将tid资源保留下来留待日后重复使用，可以
                将正在计时中的定时器Stop，以后再想用时，调用Set就可以了，不需要将定时器
                资源归还之后再申请。
                */
                //用GetTimerArg可以获取定时器携带的参数
                cout << "NetworkManager receives EV_TIMER_1!" << endl;
                cout << "timer arg = " << m->GetTimerArg() << endl;
                cout << "timer string arg = " << m->GetTimerStrArg() << endl;
                //如果不再使用定时器，这里就可以将其释放掉
                m->KillTimer(demo_tid_1);
                //一旦定时器被释放，就将记录的tid复位为无效值，以免日后再去误操作!!!
                demo_tid_1 = INVALID_TIMER_ID; 
                MID mid;
                m->GetSelfMID(mid);
                cout << "whoami: " << mid << endl;
                if(mid != message.option.sender())
                {
                    cout << " sender is not myself!" << endl;
                }

                if(mid == message.option.receiver())
                {
                    cout << " receiver is myself!" << endl;
                }
                else
                {
                    cout << "receiver = " << message.option.receiver() << endl;
                }
                break;
            }

            case EV_TIMER_2:
            {
                cout << "NetworkManager receives loop EV_TIMER_2! timer arg = " << m->GetTimerArg() << endl;
                if(++ count == 3)
                {
                    //用GetTimerId可以获取当前超时的这个定时器的id
                    m->StopTimer(m->GetTimerId());
                    cout << "NetworkManager has stopped loop timer!" << endl;
                }
                break;
            }

            case EV_BROADCAST_INFO:
            {
                cout << "NetworkManager receives EV_BROADCAST_INFO!" << endl;
                cout << "EV_BROADCAST_INFO data = " << message.data << endl;
                break;
            }
            
            case EV_PLEASE_SLEEP:
            {
                SkyInt i(message.data);
                printf("NetworkManager will sleep %d seconds ...\n",i.value);
                sleep(i.value);
                printf("NetworkManager wake up!\n");
            }
            
            default:
                cout << "NetworkManager receives a message!" << endl;
                break;
        }
    };
    
private:
    //定时器资源，不使用时应该kill掉
    int      count;
    TIMER_ID demo_tid_1;
    TIMER_ID demo_tid_2;
    static NetworkManager *instance;
};
NetworkManager* NetworkManager::instance = NULL;

//这是一个不使用MessageHandler模板的例子
//需要自己定义一个MessageUnit *m成员，并且编写Start函数来初始化
//然后创建、启动、注册消息单元，在析构函数中销毁消息单元
class StorageManager: public MessageHandler
{
public:
    StorageManager()
    {
        m = NULL;  //这个初始化步骤不可少!
    };
    
    ~StorageManager()
    {
        if(m) //这个判断的步骤不可少!
            delete m;
    };
    
    STATUS Start(const string& name)
    {
        if(m)
        {
            return ERROR_DUPLICATED_OP;
        }
        
        //创建并启动消息单元，消息单元的名称最好用宏统一定义
        m = new MessageUnit(name);
        m->SetHandler(this);
        STATUS ret = m->Run();
        if(SUCCESS != ret)
        {
            delete m;
            m = NULL;
            return ret;
        }
        
        ret = m->Register();
        if(SUCCESS != ret)
        {
            delete m;
            m = NULL;
            return ret;
        }
        
        m->Print();
        return SUCCESS;
    };

    //如果是消息单元，这个函数其实也可以不实现，别人可以根据消息单元的名字Send消息过来
    //不过如果有了这个函数的话，别人也可以在非消息单元上下文给我发送消息了
    //这个函数的作用是向MessageHandler的队列中放入一条消息，让MessageHandler去处理
    //换句话说，是在命令MessageHandler收下一条消息，这里并不需要存在任何“发送方”的概念，
    //所以这个方法命名为Receive，你可以在任何地方，任何上下文，只要得到MessageHandler对象的指针
    //就可以让它收下你已经构造好的一条消息
    STATUS Receive(const MessageFrame& message)
    {
        return m->Receive(message);
    };
    
    void MessageEntry(const MessageFrame& message)
    {
        switch(message.option.id())
        {
            case EV_STORAGE_NOTIFY:
            {
                //将收到的消息反向序列化
                DemoMessage dm;
                dm.deserialize(message.data);
                dm.Print();
                m->set_state(1);
                break;
            }

            case EV_STORAGE_REQ:
            {
                //MID结构体的打印输出运算符已经重载过了，可以直接用cout输出，
                //输出格式是: application.process.unit
                cout << "A EV_STORAGE_REQ message from " << message.option.sender() << endl;
                DemoMessage dm;
                dm.deserialize(message.data);
                dm.Print();
                DemoMessage ack("This is a EV_STORAGE_ACK message!");
                MessageOption option(message.option.sender(),EV_STORAGE_ACK,0,0);
                //sleep(2);
                m->Send(ack,option);
                break;
            }

            case EV_STORAGE_INT32:
            {
                SkyInt32 i(message.data);
                cout << "StorageManager receives EV_STORAGE_INT32 = " << i << endl;
                m->set_state(1);
                break;
            }
            
            case EV_SIMPLE_NOTICE:
            {
                cout << "StorageManager receives a simple notice!" << endl;
                break;
            }
            
            default:
                cout << "StorageManager receives an unknown message!" << endl;
                break;
        }
    }
private:
    MessageUnit *m;
};

//演示临时消息单元的使用
void TestTempMu()
{
    //在栈上创建一个临时消息单元，名字使用TempUnitName()生成可以保证不会与别人重复
    //注意传给TempUnitName的参数"test"只是一个前缀，最终名称由前缀+当前tid+本地时间组成
    //如果用户没有提供前缀字符串，则默认以"unit"为前缀
    MessageUnit tempmu(TempUnitName("test"));
    //将临时消息单元注册到消息代理，这样才能接收进程外消息
    tempmu.Register();

    //向STORAGE_MANAGER发送一条消息先
    DemoMessage req("A EV_STORAGE_REQ message to StorageManager!");
    MessageOption option(STORAGE_MANAGER,EV_STORAGE_REQ,0,0);
    tempmu.Send(req,option);

    //然后在当前线程上下文等待STORAGE_MANAGER给我们的回复
    MessageFrame message;
    //等待时可以设置超时，超时时间以毫秒为单位
    STATUS ret = tempmu.Wait(&message,3000);
    if(SUCCESS == ret)
    {
        //收到应答了
        printf("tempmu.Wait success!\n");
        DemoMessage dm;
        dm.deserialize(message.data);
        //但是这个应答消息是不是针对我们前面发过去的消息的应答?
        //这个就要用户自己去判断了，sky不负责这些
        dm.Print();
    }
    else if(ERROR_TIME_OUT == ret)
    {
        //需要根据不同的错误码区分是否超时还是其他错误
        printf("tempmu.Wait time out!\n");
    }
    else
    {
        printf("tempmu.Wait failed! ret = %u\n",ret);
    }
}

//sky库提供了to_string和from_string两个模板函数，
//可以轻松实现在基本数据类型和字符串之间互相转换
void TestStringConverter()
{
    int a = 123456;
    float b = 78.9; 
    uintptr c = (uintptr)&a;

    printf("a = %d \n", a);
    printf("b = %f \n", b);
    printf("c = 0x%lx \n", c);

    //把int a转换成字符串，以10进制的形式表示
    string s1 = to_string<int>(a,dec);
    cout << "string a = " << s1 << endl;

    //把float b转换成字符串，以10进制的形式表示
    string s2 = to_string<float>(b,dec);
    cout << "string b = " << s2 << endl;

    //把uintptr c转换成字符串，以16进制的形式表示
    string s3 = to_string<uintptr>(c,hex);
    cout << "string c = " << s3 << endl;

    //从字符串转换出int
    int a1;
    from_string<int>(a1,s1,dec);

    //从字符串转换出float
    float b1;
    from_string<float>(b1,s2,dec);

    //从字符串转换出uintptr
    uintptr c1;
    from_string<uintptr>(c1,s3,hex);

    printf("a1 = %d \n", a1);
    printf("b1 = %f \n", b1);
    printf("c1 = 0x%lx \n", c1);
}

//sky库把几种常见的基本数据类型定义为可序列化对象
//实际上是把上面和字符串相互转换的过程进一步简单化了
void TestBuiltinType()
{
    //定义一个SkyInt32并将其序列化为字符串
    SkyInt32 i(123);
    string si = i.serialize();
    cout << "string si = " << si << endl;

    //从一个字符串反序列化得到SkyInt32
    string s1("456");
    i.deserialize(s1);
    printf("now i value = %d\n", i.value);

    //定义一个SkyInt64并将其序列化为字符串
    SkyInt64 i64(123456789);
    string si64 = i64.serialize();
    cout << "string si64 = " << si64 << endl;

    //从一个字符串反序列化得到SkyInt64
    string s2("987654321");
    i64.deserialize(s2);
    printf("now i64 value = %lld\n", i64.value);
    
    //定义一个SkyPtr并将其序列化为字符串
    SkyPtr j((uintptr)TestBuiltinType);
    string sj = j.serialize();
    printf("TestBuiltinType = %p\n",TestBuiltinType);
    cout << "string sj = " << sj << endl;

    //从一个字符串反序列化得到SkyPtr
    string s3("0x121121");
    j.deserialize(s3);
    printf("now j value = 0x%lx\n", j.value);
}
DEFINE_DEBUG_FUNC(TestBuiltinType);

//演示构造MID的三种方式
void testMID()
{
    MID mid_temp;
    MID mid1("unit_1"); //只提供一个参数，sky认为这是一个unit名称

    //MID也是可以序列化和反序列化的
    string s = mid1.serialize();
    cout << "mid1 = " << s << endl;
    mid_temp.deserialize(s);
    cout << "mid_temp = " << mid_temp.serialize() << endl;
    
    MID mid2("proc_1","unit_1"); //提供两个参数，sky认为这是一个process名称加上一个unit名称
    s = mid2.serialize();
    cout << "mid2 = " << s << endl;
    mid_temp.deserialize(s);
    cout << "mid_temp = " << mid_temp.serialize() << endl;
    
    MID mid3("app_1","proc_1","unit_1"); //提供所有的三个字段
    s = mid3.serialize();
    cout << "mid3 = " << s << endl;
    mid_temp.deserialize(s);
    cout << "mid_temp = " << mid_temp.serialize() << endl;
}
DEFINE_DEBUG_FUNC(testMID);

static NetworkManager *pnm = NULL;
static StorageManager *psm = NULL;
int main()
{ 
    //初始化sky库的部分，每个进程里面只要统一调用一次就可以了
    SkyConfig config;
    //application字段在TC上是cloud的名字，在CC上是cluster的名字，在HC上是host的名字
    config.messaging.application = "mycloud";
    //如果本进程exe中有多个功能模块，进程名称要综合这些模块的特点设置一个有概括性的合适的名字
    config.messaging.process = "mutest1";
    //消息服务器的地址和端口号
    //config.messaging.broker_url = "localhost";
    //config.messaging.broker_port = 5672;
    //消息服务器的连接选项
    //config.messaging.broker_option = "";

    ConnectConfig connect;
    connect.broker_url = "localhost";
    connect.broker_port = 5672;
    connect.broker_option = "";
    config.messaging.connects.push_back(connect);
    
    //只有这个初始化成功了，才能创建和启动消息单元
    if(SUCCESS != SkyInit(config))
    {
        printf("sky init failed!\n");
        return -1;
    }

    //获取并显示本进程的application和process
    cout << "application = " << ApplicationName() << endl;
    cout << "process = " << ProcessName() << endl;
    
    //创建一个用模板设计的MessageHandler
    SimpleHandler *psim = new SimpleHandler;
    //Start的参数就是消息单元的名称
    psim->Start("SimpleHandler");
    //给SimpleHandler发个整数消息先
    MessageFrame simple_message(SkyInt32(12345),EV_STORAGE_INT32);
    psim->Receive(simple_message);
    
    //TestStringConverter();
    //TestBuiltinType();
    pnm = NetworkManager::GetInstance(); 
    if(SUCCESS != pnm->Start(NETWORK_MANAGER))
    {
        printf("NetworkManager start failed!\n");
        return -1;
    }
    //通知上电!
    pnm->PowerOn();

    psm = new StorageManager();
    if(SUCCESS != psm->Start(STORAGE_MANAGER))
    {
        printf("StorageManager start failed!\n");
        return -1;
    }
    //给STORAGE_MANAGER发个整数消息先
    MessageFrame message(SkyInt32(123),EV_STORAGE_INT32);
    psm->Receive(message);

    //给STORAGE_MANAGER发个空消息，只有消息id有意义，消息体中不携带任何信息
    //相当于是消息长度为0，但是形式上还是得走一下过场，这里可以用SkyInt(0)来代替空消息
    //对方收到的是一个内容="0"的字符串，需要发送空消息体的场合可以这么做
    //当然了，数字不一定是0，原则上是越短越好，反正是白占了空间，没有实际意义的
    MessageFrame notice(SkyInt(0),EV_SIMPLE_NOTICE);
    psm->Receive(notice);
    
    //试试临时消息单元怎么用
    TestTempMu();

    //试试组播怎么用
    pnm->JoinNetworkGroup();
    pnm->BroadcastNetworkInfo(SkyInt32(456));
    
    //启动一个shell，参数是shell的输入提示字符串
    //参数也可以不填，如果不填任何参数，将会显示默认提示符"sky-> "
    Shell("test_mu-> ");
    while(1)
    {
        sleep(1);
    }
    return 0;
}

void DbgSendInt(int a)
{
    //给STORAGE_MANAGER发个整数消息先
    MessageFrame message(SkyInt32(a),EV_STORAGE_INT32);
    psm->Receive(message);
}
DEFINE_DEBUG_FUNC(DbgSendInt);  //这个宏定义了一个调试函数，可以在shell里面使用

void DbgBcInt(int a)
{
    pnm->BroadcastNetworkInfo(SkyInt32(a));
}
DEFINE_DEBUG_FUNC(DbgBcInt);

void DbgLetItSleep(int interval)
{
    MessageFrame message(SkyInt(interval),EV_PLEASE_SLEEP);
    pnm->Receive(message);
}

void DbgShowRemainedTime()
{
    pnm->ShowRemainedTime();
}
DEFINE_DEBUG_FUNC(DbgShowRemainedTime);

void DbgDeleteMu()
{
    if(pnm)
    {
        MessageFrame message(SkyInt(5),EV_PLEASE_SLEEP);
        //pnm->Receive(message);
        delete pnm;
    }
    
    if(psm)
        delete psm;    
}
DEFINE_DEBUG_FUNC(DbgDeleteMu);

void DbgTestDestroyHandler()
{
    while(1)
    {
        //创建一个用模板设计的MessageHandler
        SimpleHandler *psim = new SimpleHandler;
        //Start的参数就是消息单元的名称
        psim->Start("SimpleHandler");
        //给SimpleHandler发个整数消息先
        MessageFrame simple_message(SkyInt32(12345),EV_STORAGE_INT32);
        psim->Receive(simple_message);
        Delay(1000);
        delete psim;
    }
}
DEFINE_DEBUG_FUNC(DbgTestDestroyHandler);


