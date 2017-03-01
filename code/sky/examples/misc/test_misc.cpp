#include "sky.h"

void test1()
{
    string pubkey;
    STATUS ret = SshGetPubkey(pubkey);
    if(SUCCESS != ret)
    {
        cerr << "SshGetPubkey failed! ret = " << ret << endl;
    }
    cout << "public key: " << endl << pubkey << endl;
}
DEFINE_DEBUG_CMD(test1,test1);

void test2(const char* envname)
{
    //证书加密字符串太长，没法放在命令行里面输入，
    //可以放在一个环境变量里面，这里的参数就是环境变量的名字
    string pubkey(getenv(envname));
    STATUS ret = SshTrust(pubkey);
    if(SUCCESS != ret)
    {
        cerr << "SshTrust failed! ret = " << ret << endl;
    }
}
DEFINE_DEBUG_FUNC(test2);

void test3(const char* envname)
{
    string pubkey(getenv(envname));
    STATUS ret = SshUntrust(pubkey);
    if(SUCCESS != ret)
    {
        cerr << "SshUntrust failed! ret = " << ret << endl;
    }
}
DEFINE_DEBUG_FUNC(test3);

void test4()
{
    Timespan interval;
    interval.Begin();
    string uuid = GenerateUUID();
    EXIT_POINT();
    interval.End();
    interval.ShowSpan();
    printf("UUID = %s\n", uuid.c_str());
}
DEFINE_DEBUG_FUNC(test4);

void test5()
{
    Timespan interval;
    interval.Begin();
    string uuid = GenerateUUID();
    EXIT_POINT();
    interval.End();
    interval.ShowSpan();
    printf("UUID = %s\n", uuid.c_str());
}
DEFINE_DEBUG_FUNC(test5);

void DbgGetStrMd5sum(const char* str)
{
    if(!str)
        return;
    
    string md5;
    Timespan clock;
    clock.Begin();
    md5 = GetStrMd5sum(str);
    clock.End();
    clock.ShowSpan();
    cout << "md5: " << md5 << endl;
}
DEFINE_DEBUG_FUNC(DbgGetStrMd5sum);

void DbgGetFileMd5sum(const char* file)
{
    if(!file)
        return;
    
    string md5;
    Timespan clock;
    clock.Begin();
    GetFileMd5sum(file,md5);
    clock.End();
    clock.ShowSpan();
    cout << "md5: " << md5 << endl;
}
DEFINE_DEBUG_FUNC(DbgGetFileMd5sum);

int main()
{
    ExceptionConfig config;
    config.debug = true;
    ExcInit(config);

    Shell("test_misc-> ");
    while(1)
    {
        sleep(1);
    }
    return 0;
}


