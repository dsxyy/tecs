#include "sky.h"

class OperationRecord: public Serializable
{
public:
    int64 oid;      //��������id
    string content; //��������
    Datetime time;  //����ʱ��(Datetime�ǿ����л��Ķ����ܹ�ֱ�����ڽṹ����)
    vector<int> numbers;

    SERIALIZE
    {
        SERIALIZE_BEGIN(OperationRecord);
        WRITE_VALUE(oid);
        WRITE_VALUE(content);
        WRITE_OBJECT(time); //Datetime�̳���Serializable������ֱ�����л�
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
    //��������ӡ���: 2011-8-17 (������ʱ����ʵ��ʱ��Ϊ׼)
    //������֮��ķָ���������б��/��Ҳ�������л���-(ֻҪ�ǵ����ַ�����)
    //ע: Ϊ������ۣ���ʽͳһ��TECS�й涨�����л���
    //��������Щû��ʹ���л��ߵĵط���������ʾ�����Ǳ�׼

    //�����κβ�������һ������ʱ�䣬���Զ�����ϵͳ�ӿ�ȥ��ȡ����ʱ��
    Datetime dt; 
    cout << dt.tostr() << endl;
    //��������ӡ���: 2011-08-17 17:36:22 (������ʱ����ʵ��ʱ��Ϊ׼)
    
    //����һ���ַ���ȥ��������ʱ�䣬���Զ������������ֶ�
    Datetime dt1("2011-08-15 19:00:01");  
    cout << dt1.tostr() << endl;
    //��������ӡ���: 2011-08-15 19:00:01

    //0000
    dt1.setDate("1221"); //�������޸�Ϊ12��21��
    cout << dt1.tostr() << endl;
    //��������ӡ���: 2011-12-21 19:00:01

    //00/00
    dt1.setDate("12/21"); //�������޸�Ϊ12��21��
    cout << dt1.strftime("%Y/%m/%d %X") << endl;
    //��������ӡ���: 2011/12/21 19:00:01

    //000000
    dt1.setDate("121221"); //���������޸�Ϊ2012��12��21��
    cout << dt1.strftime("%Y-%m-%d %X") << endl;
    //��������ӡ���: 2011-12-21 19:00:01

    
    //00000000
    dt1.setDate("20121221"); //���������޸�Ϊ2012��12��21��
    cout << dt1.strftime("%Y-%m-%d %X") << endl;
    //��������ӡ���: 2011-12-21 19:00:01

    
    //00/00/00
    dt1.setDate("12/12/21"); //���������޸�Ϊ2012��12��21��
    cout << dt1.strftime("%Y-%m-%d %X") << endl;
    //��������ӡ���: 2011-12-21 19:00:01

    
    //0000/00/00
    dt1.setDate("1999/08/01"); //���������޸�Ϊ1999��8��1��
    cout << dt1.strftime("%Y-%m-%d %X") << endl;
    //��������ӡ���: 1999-08-01 19:00:01

    //00:00
    dt1.setTime("19:25"); //��ʱ���޸�Ϊ19:25
    cout << dt1.strftime("%Y-%m-%d %X") << endl;
    //��������ӡ���: 1999-08-01 19:25:00

    //00:00:00
    dt1.setTime("19:25:41");//��ʱ���޸�Ϊ19:25:41
    cout << dt1.strftime("%Y-%m-%d %X") << endl;
    //��������ӡ���: 1999-08-01 19:25:41

    // 0000/00/00 00:00:00
    dt1.setDatetime("2012/12/21 01:20:59");//��ʱ���޸�Ϊ2012��12��21��01:20:59
    cout << dt1.strftime("%Y-%m-%d %X") << endl;
    //��������ӡ���: 2012-12-21 01:20:59
}
DEFINE_DEBUG_FUNC(test1);

//��ʾDatetime��������л��ͷ����л�
void test2()
{
    OperationRecord rec1;
    rec1.oid = 123;
    rec1.content = "Start";
    rec1.time.deserialize("2012-12-21 00:00:00");
    rec1.numbers.push_back(123);
    rec1.numbers.push_back(456);
    //Datetime�����Ĭ�Ϲ��캯������ֱ�ӻ�õ�ǰ�ı���ʱ�䣬��˲���Ҫר��ȥ��ʼ��
    
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
        dt.refresh(); //��Datetime����ˢ��Ϊ��ǰϵͳʱ��
        cout << dt.tostr(utc) << endl;
        sleep(interval);
    }
}
DEFINE_DEBUG_FUNC(test3);

void test4()
{
    //2012��12��21�գ�TECS����->����ĩ��?!
    Datetime dt1("2012/12/21 01:20:59");    //��ʱ���ʼ��Ϊ2012��12��21��01:20:59
    Datetime dt2("2012/12/21 02:20:59");    //��ʱ���ʼ��Ϊ2012��12��21��02:20:59
    cout << "dt1 = "<< dt1.serialize() << endl;
    cout << "dt2 = "<< dt2.serialize() << endl;

    dt1 = dt2; //����ֱ�ӻ��ำֵ
    cout << "After dt1 = dt2:" << endl;
    cout << "dt1 = "<< dt1.serialize() << endl;
    cout << "dt2 = "<< dt2.serialize() << endl;

    //����ֵ��������
    dt2.setDatetime("2013/11/21 02:20:59");
    cout << "---------------------" << endl;
    cout << "dt1 = "<< dt1.serialize() << endl;
    cout << "dt2 = "<< dt2.serialize() << endl;
    
    //����!����ֱ����������Ƚ���ľ��!
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
    //���� >,<,==,��ʵ����!=,<=,>=��������Ŷ����!
}
DEFINE_DEBUG_FUNC(test4);

void test5()
{
    //��ȡ��������ʱ����1970-01-01 00:00:00����������
    time_t now = time(NULL);
    cout << "time_t now = " << now << endl;

    //����������ת��Ϊ��������ʱ���ַ�����ʽ
    Datetime dt1(now);
    string s1 = dt1.tostr();
    cout << "now string = " << s1 << endl;

    //ֱ�ӻ�ȡ��������ʱ���ַ�����ʽ
    Datetime dt2;
    string s2 = dt2.tostr();
    cout << "dt2 string(utc) = " << s2 << endl;

    //תΪ����ʱ���ַ���
    s2 = dt2.tostr(false);
    cout << "dt2 string(local) = " << s2 << endl;

    //���ַ�����������Datetime
    string s3 = "2011-08-17 17:36:22";
    Datetime dt3(s3.c_str());  //��dt3.fromstr(s3.c_str());

    //��Datetime�õ���������ʱ����1970-01-01 00:00:00����������
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


