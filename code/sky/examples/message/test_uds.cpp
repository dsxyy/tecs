#include "sky.h"

#define LIGHT_MANAGER   "/tmp/light_manager"
#define LIGHT_GROUP     "/tmp/light"
#define LIGHT_PROCESS_1 "/tmp/light/process1"
#define LIGHT_PROCESS_2 "/tmp/light/process2"

#define EV_LIGHT_POWERON           1000
#define EV_LIGHT_INFO_REQ          1001
#define EV_LIGHT_INFO_ACK          1002
#define EV_TEST_INFO_REQ           1003
#define EV_TEST_MSG_ID             1004

typedef struct tagLightInfo
{
    int a;
    int b;
    char buf[20*1024];
}LightInfo;

class TestInfo: public Serializable
{
public:
    SERIALIZE
    {
        SERIALIZE_BEGIN(TestInfo);
        WRITE_VALUE(a);
        WRITE_VALUE(b);
        WRITE_VALUE(c);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(TestInfo);
        READ_VALUE(a);
        READ_VALUE(b);
        READ_VALUE(c);
        DESERIALIZE_END();
    };

    void Print()
    {
        cout << "a:" << a << endl;
        cout << "b:" << b << endl;
        cout << "c:" << c << endl;
    };
    
    string a;
    string b;
    int c;
};

class LightAgent: public UdsMessenger
{
public:   
    LightAgent():UdsMessenger("LightAgent")
    {
    
    }
    
    STATUS LightInfoAck()
    {
        printf("Get light req!\n");
        sleep(2);
        LightInfo info;
        info.a = 123;
        info.b = 456;
        printf("light ack to sender: %s!\n",GetPeerPath());

        UdsParam param(EV_LIGHT_INFO_ACK, &info, sizeof(LightInfo), 0);
        return Send(GetPeerPath(),param);
    };
        
    STATUS TestInfoAck(const string& message)
    {
        cout << message << endl;
        TestInfo info;
        bool ret = info.deserialize(message);
        SkyAssert(ret);
        info.Print();
        return SUCCESS;
    };
    
    //以二进制数据块方式接收数据
    void MessageEntry(uint32 id,void *data,uint32 length,uint32 version)
    {
        string message((char *)data);
        printf("LightAgent(%s) receive a message id = %u!\n",GetSelfPath(),id);
        switch(id)
        {
            case EV_LIGHT_INFO_REQ:
                LightInfoAck();
                break;
                
            case EV_TEST_INFO_REQ:
                TestInfoAck(message);
                break;

            default:
                break;
        }
    };

    //以c++ string方式接收数据
    void MessageEntry(const string& data,const UdsMessageOption& option)
    {
        printf("LightAgent(%s) receive a message id = %u!\n",GetSelfPath(),option.msgid);
        switch(option.msgid)
        {
            case EV_TEST_INFO_REQ:
                TestInfoAck(data);
                break;

            case EV_TEST_MSG_ID:
                cout << "receive test message:" << endl << data << endl;
                //给sender回一个应答
                Send("OK!, I received your message: " + data,EV_TEST_MSG_ID,option.sender);
                break;
                
            default:
                break;
        }
    };
};

class LightManager: public UdsMessenger
{
public:           
    LightManager():UdsMessenger("LightManager")
    {
    
    }
    
    STATUS PowerOn()
    {
        UdsParam param(EV_LIGHT_POWERON, 0, 0, 0);
        return Send(GetSelfPath(), param);
    };
    
    STATUS LightInfoReq()
    {
        UdsParam param(EV_LIGHT_INFO_REQ, 0, 0, 0);
        return Send(LIGHT_GROUP, param);
    };
    
    STATUS TestInfoReq()
    {
        TestInfo info;
        info.a = "hello";
        info.b = "world";
        info.c = 12345;
        UdsMessageOption option(LIGHT_GROUP,EV_TEST_INFO_REQ);
        return Send(info,option);
    };

    void MessageEntry(uint32 id,void *data,uint32 length,uint32 version)
    {
        printf("LightManager receive a message id = %u!\n",id);
        switch(id)
        {
            case EV_LIGHT_POWERON:
                {
                    printf("LightManager PowerOn!\n");    
                    TIMER_ID mytid = CreateTimer();
                    TimeOut timeout;
                    timeout.duration = 1000;
                    timeout.msgid = EV_TIMER_1;
                    timeout.arg = 45678;
                    timeout.type = LOOP_TIMER;
                    SetTimer(mytid,timeout);
                }
                break;

            case EV_TIMER_1:
                {
                    printf("Get EV_TIMER_1! tid=%u,arg=%lu\n",GetTimerId(),GetTimerArg());
                    LightInfoReq();
                    TestInfoReq();
                }
                break;
                
            case EV_LIGHT_INFO_ACK:
                {
                    LightInfo *pinfo = (LightInfo *)data;
                    printf("Get light ack! a=%d,b=%d!\n",pinfo->a,pinfo->b);
                }
                break;
                
            default:
                break;
        }
    };
};

static LightManager *s_pm = NULL;
static LightAgent *s_pa1 = NULL;
static LightAgent *s_pa2 = NULL;

int main()
{
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
    
    LightAgent *pa1 = new LightAgent();
    if(SUCCESS != pa1->Init(LIGHT_PROCESS_1))
    {
        printf("init process1 failed!");
        return -1;
    }
    pa1->SetMessageMode(UdsMessenger::MESSAGE_STRING_MODE);
    pa1->Run();
    s_pa1 = pa1;

    LightAgent *pa2 = new LightAgent();
    if(SUCCESS != pa2->Init(LIGHT_PROCESS_2))
    {
        printf("init process2 failed!");
        return -1;
    }
    pa2->Run();
    s_pa2 = pa2;

#if 1
    LightManager *pm = new LightManager();
    s_pm = pm;

    pm->Init(LIGHT_MANAGER);
    pm->Run();
    pm->PowerOn();
    #endif
    
#if 0
    char buf[32 * 1024];
    memset(buf,0,sizeof(buf));
    UdsMessenger *ptest = new UdsMessenger();
    if(SUCCESS != ptest->Init("/tmp/ptest.sock"))
    {
        printf("init ptest.sock failed!");
        return -1;
    }

    UdsParam param(EV_LIGHT_INFO_REQ,0,0,0);
    param.timeout = 3000;
    
    UdsResult result;
    result.buf = buf;
    result.bufsize = sizeof(buf);

    if(SUCCESS != ptest->Call("/tmp/light/process2",param,result))
    {
        printf("ptest->Call failed!\n");
        delete ptest;
    }
    else
    {
        LightInfo *pinfo = (LightInfo *)(result.buf);
        printf("ptest Get light ack! a=%d,b=%d!\n",pinfo->a,pinfo->b);
    }
#endif
    Shell("test_uds-> ");
    while(1)
    {
        sleep(1);
    }
    return 0;
}

#define TEMP_UDS "/tmp/tempuds"
void DbgTempSend(const char* data)
{
    STATUS ret = ERROR;
    
    //创建一个UdsMessenger，给个名字吧
    //如果和消息单元一起使用，建议作为消息单元的成员变量保存，不要每次使用就临时建一个，还是有代价的
    UdsMessenger uds("tempuds");
    
    //初始化时给个socket文件路径，如果文件不存在，会临时建一个，但是目录不会自动建
    ret = uds.Init(TEMP_UDS);
    if(SUCCESS != ret)
    {
        cerr << "failed to init UdsMessenger!" << endl;
        return;
    }

    //给LIGHT_PROCESS_1发送一条消息
    ret = uds.Send(data, EV_TEST_MSG_ID, LIGHT_PROCESS_1);
    if(SUCCESS != ret)
    {
        cerr << "failed to send uds message!" << endl;
        return;
    }

    //开始等待应答
    string ack;
    UdsMessageOption option;
    ret = uds.Wait(ack,option,WAIT_FOREVER);
    if(SUCCESS != ret)
    {
        cerr << "failed to wait ack!" << endl;
        return;
    }

    cout << "ack:" << endl << ack << endl;
}
DEFINE_DEBUG_FUNC(DbgTempSend);


