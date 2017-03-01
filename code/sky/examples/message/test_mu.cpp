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

//ʹ��MessageHandlerģ�嶨��һ���Լ���Handler
//ʹ��ģ��ĺô��ǲ���Ҫ�Լ���ȥ���塢������������������Ϣ��Ԫ��
//�������ģ��ķ�ʽ�����ã�Ҳ�����Լ�дһ�����μ������StorageManager
//SimpleHandler���÷��μ�main����
class SimpleHandler: public MessageHandlerTpl<MessageUnit>
{
public:
    //ʹ����ģ��֮����Ҫ���������ֻʣһ��:��д��Ϣ������
    void MessageEntry(const MessageFrame& message)
    {
        printf("SimpleHandler receive a message, content:\n%s\n",message.data.c_str());
    };
};
    
//����ʹ��ģ�崴���Ĺ��ܸ����MessageHandler��ʾ
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
        
    //NetworkManager��һ���ϵ�֪ͨ�ӿ�
    void PowerOn()
    {
        //���Լ���һ���ϵ�֪ͨ��Ϣ
        DemoMessage msg("Power on!");
        MessageOption option(NETWORK_MANAGER,EV_POWER_ON,0,0);
        //�����ϵ�֪ͨ��Ϣ֡
        MessageFrame frame(msg,option);
        //��mu�����ϵ�֪ͨ��Ϣ֡
        m->Receive(frame);
    };

    void JoinNetworkGroup()
    {
        //����һ����Ϊnet1���鲥��
        m->JoinMcGroup("net1");
    };

    void BroadcastNetworkInfo(const Serializable& message)
    {
        //����Ϊnet1���鲥�鷢���鲥��Ϣ
        MID target("group","net1");
        MessageOption option(target,EV_BROADCAST_INFO,0,0);
        m->Send(message, option);
    };
        
    void ShowRemainedTime()
    {
        T_TimeValue remained;
        //��ȡһ����ʱ�����볬ʱʱ��ʣ���ʱ��
        if(SUCCESS != m->GetRemainedTime(demo_tid_1,&remained))
        {
            printf("GetRemainedTime failed\n");
            return;
        }
        printf("demo_tid_1 remained %d seconds, %d miliseconds\n",remained.second,remained.milisecond);
    }

    void MessageEntry(const MessageFrame& message)
    {       
        //�����յ�����Ϣid���д���
        switch(message.option.id())
        {
            case EV_POWER_ON:
            {
                //�յ��ϵ���Ϣ
                DemoMessage dm;
                dm.deserialize(message.data);
                dm.Print();

                //����һ��״̬
                m->set_state(1);

                //������һ����Ϣ��Ԫ������Ϣ
                DemoMessage msg("A message from NetworkManager to StorageManager!");
                //���ڽ��շ���������ͬһ�����������MessageOptionʱ������дapplication��process
                //����������ȥ����MID�ˣ�ֱ������Է���unit���Ƽ���
                MessageOption option(STORAGE_MANAGER,EV_STORAGE_NOTIFY,0,0);
                //���������е�Ч��MessageOption option(MID(STORAGE_MANAGER),EV_STORAGE_NOTIFY,0,0);
                m->Send(msg,option);

                //������ʾ��ʱ����ʹ��
                TimeOut timeout;

                //����һ����ͨ��Զ�ʱ��
                demo_tid_1 = m->CreateTimer();
                timeout.duration = 1000 * 3;
                timeout.msgid = EV_TIMER_1;
                timeout.arg = 12345;
                timeout.str_arg = "hello,timer";
                //type�ֶβ���дʱ��Ĭ�Ͼ�����ͨ��Զ�ʱ��
                //timeout.type = RELATIVE_TIMER;  
                m->SetTimer(demo_tid_1,timeout);
                //ע��:��ʱ�Ķ�ʱ�������Զ��ͷţ���Ҫ�û��Լ�ȥkill���tid!!!
                //����Ӧ�ð�tid�����������������˽�г�Ա������
                
                //����һ��ѭ����ʱ��
                demo_tid_2 = m->CreateTimer();
                timeout.duration = 1000;
                timeout.msgid = EV_TIMER_2;
                timeout.arg = 67890;
                //���뽫type�ֶ���дΪLOOP_TIMER!!!
                timeout.type = LOOP_TIMER;
                m->SetTimer(demo_tid_2,timeout);
                count = 0;
                //todo: ���Զ�ʱ��Ŀǰ��δ�������󣬴���ʽʵ�ֺ󲹳���ʾ����
                break;
            }

            //����ʱ����ʱ��Ϣ
            case EV_TIMER_1:
            {
                /*
                ��������ö�ʱ����������Լ���Set�������ҿ���Set������type�Ķ�ʱ����
                Ҳ����˵��ֻҪ��Create��һ����ʱ�����õ���tid��Դ���Ϳ������⽫��
                Set����ͨ��Զ�ʱ������ѭ����ʱ�����߾��Զ�ʱ����
                ��Ȼ�ˣ�ֻ�����һ��Set���ǻ�������Ч��!!!

                ���ֻ�����ö�ʱ��ֹͣ��ʱ�����ҽ�tid��Դ�������������պ��ظ�ʹ�ã�����
                �����ڼ�ʱ�еĶ�ʱ��Stop���Ժ�������ʱ������Set�Ϳ����ˣ�����Ҫ����ʱ��
                ��Դ�黹֮�������롣
                */
                //��GetTimerArg���Ի�ȡ��ʱ��Я���Ĳ���
                cout << "NetworkManager receives EV_TIMER_1!" << endl;
                cout << "timer arg = " << m->GetTimerArg() << endl;
                cout << "timer string arg = " << m->GetTimerStrArg() << endl;
                //�������ʹ�ö�ʱ��������Ϳ��Խ����ͷŵ�
                m->KillTimer(demo_tid_1);
                //һ����ʱ�����ͷţ��ͽ���¼��tid��λΪ��Чֵ�������պ���ȥ�����!!!
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
                    //��GetTimerId���Ի�ȡ��ǰ��ʱ�������ʱ����id
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
    //��ʱ����Դ����ʹ��ʱӦ��kill��
    int      count;
    TIMER_ID demo_tid_1;
    TIMER_ID demo_tid_2;
    static NetworkManager *instance;
};
NetworkManager* NetworkManager::instance = NULL;

//����һ����ʹ��MessageHandlerģ�������
//��Ҫ�Լ�����һ��MessageUnit *m��Ա�����ұ�дStart��������ʼ��
//Ȼ�󴴽���������ע����Ϣ��Ԫ��������������������Ϣ��Ԫ
class StorageManager: public MessageHandler
{
public:
    StorageManager()
    {
        m = NULL;  //�����ʼ�����費����!
    };
    
    ~StorageManager()
    {
        if(m) //����жϵĲ��費����!
            delete m;
    };
    
    STATUS Start(const string& name)
    {
        if(m)
        {
            return ERROR_DUPLICATED_OP;
        }
        
        //������������Ϣ��Ԫ����Ϣ��Ԫ����������ú�ͳһ����
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

    //�������Ϣ��Ԫ�����������ʵҲ���Բ�ʵ�֣����˿��Ը�����Ϣ��Ԫ������Send��Ϣ����
    //�������������������Ļ�������Ҳ�����ڷ���Ϣ��Ԫ�����ĸ��ҷ�����Ϣ��
    //�����������������MessageHandler�Ķ����з���һ����Ϣ����MessageHandlerȥ����
    //���仰˵����������MessageHandler����һ����Ϣ�����ﲢ����Ҫ�����κΡ����ͷ����ĸ��
    //���������������ΪReceive����������κεط����κ������ģ�ֻҪ�õ�MessageHandler�����ָ��
    //�Ϳ��������������Ѿ�����õ�һ����Ϣ
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
                //���յ�����Ϣ�������л�
                DemoMessage dm;
                dm.deserialize(message.data);
                dm.Print();
                m->set_state(1);
                break;
            }

            case EV_STORAGE_REQ:
            {
                //MID�ṹ��Ĵ�ӡ���������Ѿ����ع��ˣ�����ֱ����cout�����
                //�����ʽ��: application.process.unit
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

//��ʾ��ʱ��Ϣ��Ԫ��ʹ��
void TestTempMu()
{
    //��ջ�ϴ���һ����ʱ��Ϣ��Ԫ������ʹ��TempUnitName()���ɿ��Ա�֤����������ظ�
    //ע�⴫��TempUnitName�Ĳ���"test"ֻ��һ��ǰ׺������������ǰ׺+��ǰtid+����ʱ�����
    //����û�û���ṩǰ׺�ַ�������Ĭ����"unit"Ϊǰ׺
    MessageUnit tempmu(TempUnitName("test"));
    //����ʱ��Ϣ��Ԫע�ᵽ��Ϣ�����������ܽ��ս�������Ϣ
    tempmu.Register();

    //��STORAGE_MANAGER����һ����Ϣ��
    DemoMessage req("A EV_STORAGE_REQ message to StorageManager!");
    MessageOption option(STORAGE_MANAGER,EV_STORAGE_REQ,0,0);
    tempmu.Send(req,option);

    //Ȼ���ڵ�ǰ�߳������ĵȴ�STORAGE_MANAGER�����ǵĻظ�
    MessageFrame message;
    //�ȴ�ʱ�������ó�ʱ����ʱʱ���Ժ���Ϊ��λ
    STATUS ret = tempmu.Wait(&message,3000);
    if(SUCCESS == ret)
    {
        //�յ�Ӧ����
        printf("tempmu.Wait success!\n");
        DemoMessage dm;
        dm.deserialize(message.data);
        //�������Ӧ����Ϣ�ǲ����������ǰ�淢��ȥ����Ϣ��Ӧ��?
        //�����Ҫ�û��Լ�ȥ�ж��ˣ�sky��������Щ
        dm.Print();
    }
    else if(ERROR_TIME_OUT == ret)
    {
        //��Ҫ���ݲ�ͬ�Ĵ����������Ƿ�ʱ������������
        printf("tempmu.Wait time out!\n");
    }
    else
    {
        printf("tempmu.Wait failed! ret = %u\n",ret);
    }
}

//sky���ṩ��to_string��from_string����ģ�庯����
//��������ʵ���ڻ����������ͺ��ַ���֮�以��ת��
void TestStringConverter()
{
    int a = 123456;
    float b = 78.9; 
    uintptr c = (uintptr)&a;

    printf("a = %d \n", a);
    printf("b = %f \n", b);
    printf("c = 0x%lx \n", c);

    //��int aת�����ַ�������10���Ƶ���ʽ��ʾ
    string s1 = to_string<int>(a,dec);
    cout << "string a = " << s1 << endl;

    //��float bת�����ַ�������10���Ƶ���ʽ��ʾ
    string s2 = to_string<float>(b,dec);
    cout << "string b = " << s2 << endl;

    //��uintptr cת�����ַ�������16���Ƶ���ʽ��ʾ
    string s3 = to_string<uintptr>(c,hex);
    cout << "string c = " << s3 << endl;

    //���ַ���ת����int
    int a1;
    from_string<int>(a1,s1,dec);

    //���ַ���ת����float
    float b1;
    from_string<float>(b1,s2,dec);

    //���ַ���ת����uintptr
    uintptr c1;
    from_string<uintptr>(c1,s3,hex);

    printf("a1 = %d \n", a1);
    printf("b1 = %f \n", b1);
    printf("c1 = 0x%lx \n", c1);
}

//sky��Ѽ��ֳ����Ļ����������Ͷ���Ϊ�����л�����
//ʵ�����ǰ�������ַ����໥ת���Ĺ��̽�һ���򵥻���
void TestBuiltinType()
{
    //����һ��SkyInt32���������л�Ϊ�ַ���
    SkyInt32 i(123);
    string si = i.serialize();
    cout << "string si = " << si << endl;

    //��һ���ַ��������л��õ�SkyInt32
    string s1("456");
    i.deserialize(s1);
    printf("now i value = %d\n", i.value);

    //����һ��SkyInt64���������л�Ϊ�ַ���
    SkyInt64 i64(123456789);
    string si64 = i64.serialize();
    cout << "string si64 = " << si64 << endl;

    //��һ���ַ��������л��õ�SkyInt64
    string s2("987654321");
    i64.deserialize(s2);
    printf("now i64 value = %lld\n", i64.value);
    
    //����һ��SkyPtr���������л�Ϊ�ַ���
    SkyPtr j((uintptr)TestBuiltinType);
    string sj = j.serialize();
    printf("TestBuiltinType = %p\n",TestBuiltinType);
    cout << "string sj = " << sj << endl;

    //��һ���ַ��������л��õ�SkyPtr
    string s3("0x121121");
    j.deserialize(s3);
    printf("now j value = 0x%lx\n", j.value);
}
DEFINE_DEBUG_FUNC(TestBuiltinType);

//��ʾ����MID�����ַ�ʽ
void testMID()
{
    MID mid_temp;
    MID mid1("unit_1"); //ֻ�ṩһ��������sky��Ϊ����һ��unit����

    //MIDҲ�ǿ������л��ͷ����л���
    string s = mid1.serialize();
    cout << "mid1 = " << s << endl;
    mid_temp.deserialize(s);
    cout << "mid_temp = " << mid_temp.serialize() << endl;
    
    MID mid2("proc_1","unit_1"); //�ṩ����������sky��Ϊ����һ��process���Ƽ���һ��unit����
    s = mid2.serialize();
    cout << "mid2 = " << s << endl;
    mid_temp.deserialize(s);
    cout << "mid_temp = " << mid_temp.serialize() << endl;
    
    MID mid3("app_1","proc_1","unit_1"); //�ṩ���е������ֶ�
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
    //��ʼ��sky��Ĳ��֣�ÿ����������ֻҪͳһ����һ�ξͿ�����
    SkyConfig config;
    //application�ֶ���TC����cloud�����֣���CC����cluster�����֣���HC����host������
    config.messaging.application = "mycloud";
    //���������exe���ж������ģ�飬��������Ҫ�ۺ���Щģ����ص�����һ���и����Եĺ��ʵ�����
    config.messaging.process = "mutest1";
    //��Ϣ�������ĵ�ַ�Ͷ˿ں�
    //config.messaging.broker_url = "localhost";
    //config.messaging.broker_port = 5672;
    //��Ϣ������������ѡ��
    //config.messaging.broker_option = "";

    ConnectConfig connect;
    connect.broker_url = "localhost";
    connect.broker_port = 5672;
    connect.broker_option = "";
    config.messaging.connects.push_back(connect);
    
    //ֻ�������ʼ���ɹ��ˣ����ܴ�����������Ϣ��Ԫ
    if(SUCCESS != SkyInit(config))
    {
        printf("sky init failed!\n");
        return -1;
    }

    //��ȡ����ʾ�����̵�application��process
    cout << "application = " << ApplicationName() << endl;
    cout << "process = " << ProcessName() << endl;
    
    //����һ����ģ����Ƶ�MessageHandler
    SimpleHandler *psim = new SimpleHandler;
    //Start�Ĳ���������Ϣ��Ԫ������
    psim->Start("SimpleHandler");
    //��SimpleHandler����������Ϣ��
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
    //֪ͨ�ϵ�!
    pnm->PowerOn();

    psm = new StorageManager();
    if(SUCCESS != psm->Start(STORAGE_MANAGER))
    {
        printf("StorageManager start failed!\n");
        return -1;
    }
    //��STORAGE_MANAGER����������Ϣ��
    MessageFrame message(SkyInt32(123),EV_STORAGE_INT32);
    psm->Receive(message);

    //��STORAGE_MANAGER��������Ϣ��ֻ����Ϣid�����壬��Ϣ���в�Я���κ���Ϣ
    //�൱������Ϣ����Ϊ0��������ʽ�ϻ��ǵ���һ�¹��������������SkyInt(0)���������Ϣ
    //�Է��յ�����һ������="0"���ַ�������Ҫ���Ϳ���Ϣ��ĳ��Ͽ�����ô��
    //��Ȼ�ˣ����ֲ�һ����0��ԭ������Խ��Խ�ã������ǰ�ռ�˿ռ䣬û��ʵ�������
    MessageFrame notice(SkyInt(0),EV_SIMPLE_NOTICE);
    psm->Receive(notice);
    
    //������ʱ��Ϣ��Ԫ��ô��
    TestTempMu();

    //�����鲥��ô��
    pnm->JoinNetworkGroup();
    pnm->BroadcastNetworkInfo(SkyInt32(456));
    
    //����һ��shell��������shell��������ʾ�ַ���
    //����Ҳ���Բ����������κβ�����������ʾĬ����ʾ��"sky-> "
    Shell("test_mu-> ");
    while(1)
    {
        sleep(1);
    }
    return 0;
}

void DbgSendInt(int a)
{
    //��STORAGE_MANAGER����������Ϣ��
    MessageFrame message(SkyInt32(a),EV_STORAGE_INT32);
    psm->Receive(message);
}
DEFINE_DEBUG_FUNC(DbgSendInt);  //����궨����һ�����Ժ�����������shell����ʹ��

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
        //����һ����ģ����Ƶ�MessageHandler
        SimpleHandler *psim = new SimpleHandler;
        //Start�Ĳ���������Ϣ��Ԫ������
        psim->Start("SimpleHandler");
        //��SimpleHandler����������Ϣ��
        MessageFrame simple_message(SkyInt32(12345),EV_STORAGE_INT32);
        psim->Receive(simple_message);
        Delay(1000);
        delete psim;
    }
}
DEFINE_DEBUG_FUNC(DbgTestDestroyHandler);


