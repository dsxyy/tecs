#include "sky.h"

class Sleeper : public Worktodo
{
public:
    int sleep_seconds;
    
    STATUS virtual Execute()
    {
        cout << getName() << " is running! will sleep " << sleep_seconds << " seconds. "<< endl; 
        sleep(sleep_seconds);
        return SUCCESS;
    }
    
    Sleeper(const string& name,int sec) : Worktodo(name),sleep_seconds(sec)
    {
    }
    
    ~Sleeper()
    {
        //cout << "sleeper " << name << " is over!" << endl; 
    }
};

#define NUM_OF_SLEEPERS 3
ThreadPool *s_tpool = NULL;
void test1()
{
    s_tpool = new ThreadPool(NUM_OF_SLEEPERS);
    s_tpool->CreateThreads("SleepWorker");
    
    Sleeper *p1 = new Sleeper("sleeper_1",3);
    s_tpool->AssignWork(p1);

    Sleeper *p2 = new Sleeper("sleeper_2",4);
    s_tpool->AssignWork(p2);

    Sleeper *p3 = new Sleeper("sleeper_3",5);
    s_tpool->AssignWork(p3);

    Sleeper *p4 = new Sleeper("sleeper_4",6);
    s_tpool->AssignWork(p4);
}
DEFINE_DEBUG_FUNC(test1);

void test2(const char* name, int seconds)
{
    if(!s_tpool)
        return;

    Sleeper *p = new Sleeper(name,seconds);
    s_tpool->AssignWork(p);
}
DEFINE_DEBUG_FUNC(test2);

int main()
{
    //³õÊ¼»¯sky¿â
    SkyConfig config;
    config.messaging.application = "mycloud";
    config.messaging.process = "mutest1";
    ConnectConfig connect;
    connect.broker_url = "localhost";
    connect.broker_port = 5672;
    connect.broker_option = "{reconnect:True}";
    config.messaging.connects.push_back(connect);

    if (SUCCESS != SkyInit(config))
    {
        printf("sky init failed!\n");
        return -1;
    }

    Shell("test_threadpool-> ");
    while (1)
    {
        sleep(1);
    }
    return 0;
}



