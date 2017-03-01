#include "sky.h"

class OperationRecord: public Serializable
{
public:
    int64 oid;      //操作对象id
    string content; //操作内容
    Datetime time;  //操作时间(Datetime是可序列化的对象，能够直接用在结构体中)
    vector<int> numbers;

    SERIALIZE
    {
        SERIALIZE_BEGIN(OperationRecord);
        WRITE_VALUE(oid);
        WRITE_VALUE(content);
        WRITE_OBJECT(time); //Datetime继承自Serializable，可以直接序列化
        WRITE_VALUE(numbers);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(OperationRecord);
        READ_VALUE(oid);
        READ_VALUE(content);
        READ_OBJECT(time);
        READ_VALUE(numbers);
        DESERIALIZE_END();
    };
};

void test1()
{
    Date d;
    printf("%d-%d-%d\n",d.getYear(),d.getMonth(),d.getDay()); 
    //上述语句打印结果: 2011-8-17 (以运行时本地实际时间为准)
    //年月日之间的分隔符可以是斜杠/，也可以是中划线-(只要是单个字符即可)
    //注: 为表达美观，形式统一，TECS中规定都用中划线
    //本例中有些没有使用中划线的地方仅仅是演示，不是标准

    //不加任何参数构造一个日期时间，会自动调用系统接口去获取本地时间
    Datetime dt; 
    cout << dt.tostr() << endl;
    //上述语句打印结果: 2011-08-17 17:36:22 (以运行时本地实际时间为准)
    
    //利用一个字符串去构造日期时间，会自动解析出各个字段
    Datetime dt1("2011-08-15 19:00:01");  
    cout << dt1.tostr() << endl;
    //上述语句打印结果: 2011-08-15 19:00:01

    //0000
    dt1.setDate("1221"); //将月日修改为12月21日
    cout << dt1.tostr() << endl;
    //上述语句打印结果: 2011-12-21 19:00:01

    //00/00
    dt1.setDate("12/21"); //将月日修改为12月21日
    cout << dt1.strftime("%Y/%m/%d %X") << endl;
    //上述语句打印结果: 2011/12/21 19:00:01

    //000000
    dt1.setDate("121221"); //将年月日修改为2012年12月21日
    cout << dt1.strftime("%Y-%m-%d %X") << endl;
    //上述语句打印结果: 2011-12-21 19:00:01

    
    //00000000
    dt1.setDate("20121221"); //将年月日修改为2012年12月21日
    cout << dt1.strftime("%Y-%m-%d %X") << endl;
    //上述语句打印结果: 2011-12-21 19:00:01

    
    //00/00/00
    dt1.setDate("12/12/21"); //将年月日修改为2012年12月21日
    cout << dt1.strftime("%Y-%m-%d %X") << endl;
    //上述语句打印结果: 2011-12-21 19:00:01

    
    //0000/00/00
    dt1.setDate("1999/08/01"); //将年月日修改为1999年8月1日
    cout << dt1.strftime("%Y-%m-%d %X") << endl;
    //上述语句打印结果: 1999-08-01 19:00:01

    //00:00
    dt1.setTime("19:25"); //将时间修改为19:25
    cout << dt1.strftime("%Y-%m-%d %X") << endl;
    //上述语句打印结果: 1999-08-01 19:25:00

    //00:00:00
    dt1.setTime("19:25:41");//将时间修改为19:25:41
    cout << dt1.strftime("%Y-%m-%d %X") << endl;
    //上述语句打印结果: 1999-08-01 19:25:41

    // 0000/00/00 00:00:00
    dt1.setDatetime("2012/12/21 01:20:59");//将时间修改为2012年12月21日01:20:59
    cout << dt1.strftime("%Y-%m-%d %X") << endl;
    //上述语句打印结果: 2012-12-21 01:20:59
}
DEFINE_DEBUG_FUNC(test1);

//演示Datetime对象的序列化和反序列化
void test2()
{
    OperationRecord rec1;
    rec1.oid = 123;
    rec1.content = "Start";
    rec1.time.deserialize("2012-12-21 00:00:00");
    rec1.numbers.push_back(123);
    rec1.numbers.push_back(456);
    //Datetime对象的默认构造函数就是直接获得当前的本地时间，因此不需要专门去初始化
    
    string s = rec1.serialize();
    cout << "-------------rec1.serialize------------" << endl;
    cout << s << endl;
    
    OperationRecord rec2;
    rec2.deserialize(s);
    s = rec2.serialize();
    cout << "-------------rec2.serialize------------" << endl;
    cout << s << endl;
}
DEFINE_DEBUG_FUNC(test2);

void test3(int interval = 1,bool utc = true)
{
    Datetime dt;
    for(int i = 0;i < 3;i++)
    {
        dt.refresh(); //将Datetime对象刷新为当前系统时间
        cout << dt.tostr(utc) << endl;
        sleep(interval);
    }
}
DEFINE_DEBUG_FUNC(test3);

void test4()
{
    //2012年12月21日，TECS商用->世界末日?!
    Datetime dt1("2012/12/21 01:20:59");    //将时间初始化为2012年12月21日01:20:59
    Datetime dt2("2012/12/21 02:20:59");    //将时间初始化为2012年12月21日02:20:59
    cout << "dt1 = "<< dt1.serialize() << endl;
    cout << "dt2 = "<< dt2.serialize() << endl;

    dt1 = dt2; //可以直接互相赋值
    cout << "After dt1 = dt2:" << endl;
    cout << "dt1 = "<< dt1.serialize() << endl;
    cout << "dt2 = "<< dt2.serialize() << endl;

    //换个值，继续玩
    dt2.setDatetime("2013/11/21 02:20:59");
    cout << "---------------------" << endl;
    cout << "dt1 = "<< dt1.serialize() << endl;
    cout << "dt2 = "<< dt2.serialize() << endl;
    
    //尼玛!还能直接拿来互相比较有木有!
    if(dt1 > dt2)
    {
        cout << "dt1 > dt2" << endl;
    }
    else if(dt1 < dt2)
    {
        cout << "dt1 < dt2" << endl;
    }
    else if(dt1 == dt2)
    {
        cout << "dt1 = dt2" << endl;
    }
    //除了 >,<,==,其实还有!=,<=,>=都可以用哦，亲!
}
DEFINE_DEBUG_FUNC(test4);

void test5()
{
    //获取格林威治时间自1970-01-01 00:00:00以来的秒数
    time_t now = time(NULL);
    cout << "time_t now = " << now << endl;

    //将上述秒数转换为格林威治时间字符串格式
    Datetime dt1(now);
    string s1 = dt1.tostr();
    cout << "now string = " << s1 << endl;

    //直接获取格林威治时间字符串格式
    Datetime dt2;
    string s2 = dt2.tostr();
    cout << "dt2 string(utc) = " << s2 << endl;

    //转为本地时间字符串
    s2 = dt2.tostr(false);
    cout << "dt2 string(local) = " << s2 << endl;

    //从字符串解析构造Datetime
    string s3 = "2011-08-17 17:36:22";
    Datetime dt3(s3.c_str());  //或dt3.fromstr(s3.c_str());

    //从Datetime得到格林威治时间自1970-01-01 00:00:00以来的秒数
    time_t t3 = dt3.tointeger();
    cout << "time_t t3 = " << t3 << endl;
}
DEFINE_DEBUG_FUNC(test5);

int main()
{
    //test1();
    //test2();
    //test3();
    test5();
#if 0
    time_t theTime1;
    time( &theTime1 );   // get the calendar time
    tm *t1 = localtime( &theTime1 );  // convert to local
    cout << "The local time is: " << asctime(t1);

    time_t theTime2;
    time( &theTime2 );   // get the calendar time
    tm *t2 = gmtime( &theTime2 );  // convert to local
    cout << "The gmt time is: " << asctime(t2);
#endif
    Shell("test_datetime-> ");
    while(1)
    {
        sleep(1);
    }
    return 0;
}


