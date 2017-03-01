#include "sky.h"

//ע��:���������sky��ʱ��ģ����ڲ��ܹ����ԣ�sky���û�����Ҫ�˽�!!!
//����Ϣ��Ԫ/��Ϣ����/udsmessenger��ʹ�ö�ʱ���ķ�������μ�����ģ���Լ�������
class TestTimerClient: public TimerClient
{
public:
    TestTimerClient(const string& name):TimerClient(name)
    {

    };
    
    void DoTimer(const TimerMessage& m)
    {
        cout << "TestTimerClient Timer " << m.tid << " time out!" << endl;
    };

    uintptr GetTimerArg()
    {
        return 0;
    };

    TIMER_ID GetTimerId()
    {
        return INVALID_TIMER_ID;
    };
private:
    string client_name;
};

int main()
{
    SkyConfig config;
    if(SUCCESS != SkyInit(config))
    {
        printf("sky init failed!\n");
        return -1;
    }
    
    TestTimerClient client("test");
   
    TIMER_ID mytid = client.CreateTimer();
    TimeOut timeout;
    timeout.duration = 1000;
    timeout.msgid = 2;
    client.SetTimer(mytid,timeout);
    client.StopTimer(mytid);
    client.KillTimer(mytid);
    
    TIMER_ID mytid2 = client.CreateTimer();
    timeout.msgid = 1;
    client.SetTimer(mytid2,timeout);

    TIMER_ID mytid3 = client.CreateTimer();
    timeout.msgid = 1;
    client.SetTimer(mytid3,timeout);

    TIMER_ID mytid4 = client.CreateTimer();
    client.SetTimer(mytid4,timeout);

    TIMER_ID mytid5 = client.CreateTimer();
    client.SetTimer(mytid5,timeout);

    TIMER_ID mytid6 = client.CreateTimer();
    client.SetTimer(mytid6,timeout);
    client.SetTimer(mytid6,timeout);

    Shell("test_timer-> ");
    while(1)
    {
        sleep(1);
    }
}

