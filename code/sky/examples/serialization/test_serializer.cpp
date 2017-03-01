#include "sky.h"

typedef map<string,string> Dict;

class SimpleObject: public Serializable
{
public:
    SimpleObject()
    {
        //a = 987654;
        //b = "This is a simple object";
    };
    
    int a;
    string b;

    SERIALIZE
    {
        SERIALIZE_BEGIN(SimpleObject);  //���л���ʼ����������д����
        WRITE_VALUE(a);         //���л�����������(bool/char/int/float/double,
                                //�Լ�int32/int64/uintptr/c++ string����WRITE_VALUE�������л�)
        WRITE_VALUE(b);
        SERIALIZE_END();        //���л�����
    }

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(SimpleObject); //�������л���ʼ����������д����
        READ_VALUE(a);          //���л�����������(bool/char/int/float/double,
                                //�Լ�int32/int64/uintptr/c++ string����READ_VALUE���������л�)
        READ_VALUE(b);
        DESERIALIZE_END();      //�������л�����
    }
};

class EmbbedObj:public Serializable
{
public:
    int member;
    SimpleObject so;
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(EmbbedObj);  
        WRITE_VALUE(member);
        WRITE_OBJECT(so);       //Ƕ�׵Ľṹ��ʹ��WRITE_OBJECT���л�
        SERIALIZE_END();             
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(EmbbedObj);  
        READ_VALUE(member);
        READ_OBJECT(so);        //Ƕ�׵Ľṹ��ʹ��READ_OBJECT�����л�
        DESERIALIZE_END();             
    };
};

class Vectors: public Serializable
{
public:
    vector<int32> intergers;
    list<char>  charactors;
    Dict dict;
    string str;
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(Vectors);  //���л���ʼ
        WRITE_VALUE(intergers);     //���л������������Լ�c++ stirng��ɵ�vector/list/map����WRITE_VALUE���л�
        WRITE_VALUE(charactors);
        WRITE_VALUE(dict);
        WRITE_VALUE(str);
        SERIALIZE_END();             //���л�����
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(Vectors);  
        READ_VALUE(intergers);     //���л������������Լ�c++ stirng��ɵ�vector/list/map����READ_VALUE�����л�
        READ_VALUE(charactors);
        READ_VALUE(dict);
        READ_VALUE(str);
        DESERIALIZE_END();             
    };
};

class MultiTypes: public Serializable
{
public:
    char   ch;
    bool   bl;
    string s;
    int    a;
    int16  b;
    uint16 ub;
    int32  c;
    uint32 uc;
    int64  d;
    uint64 ud;
    float  f;
    SimpleObject so;
    vector<char>   chars;
    vector<string> strs;
    list<int32>    li;
    vector<float>  vf;
    map<string,string> ss;
    map<int,string> is;
    map<string,int> si;

    //ע��: ��map�ĵ�һ����ԱΪstring����ʱ����ʵ������ʱ���ݱ����Ƿ���c/c++�﷨�淶Ҫ���"��־��"�ַ�����
    //����"name"��"age"��"cpu1"��"diskN"�ȿ�����Ϊ�������Ƶ��﷨�ַ����������Ǵ��пո����ţ�б��֮����ַ���
    //map�ĵڶ�����ԱΪstringʱ�����ݸ�ʽ����
    
    //���л�����
    SERIALIZE
    {
        SERIALIZE_BEGIN(MultiTypes);  //���л���ʼ
        WRITE_VALUE(ch);            //���л�һ��char�����ֶ�
        WRITE_VALUE(bl);            //���л�һ��bool�����ֶ�
        WRITE_VALUE(s);             //���л�һ��string���������ֶ�
        WRITE_VALUE(a);             //���л�һ�������ֶ�
        WRITE_VALUE(b);             //���л�һ��int16�����ֶ�
        WRITE_VALUE(ub);            // ....
        WRITE_VALUE(c);
        WRITE_VALUE(uc);
        WRITE_VALUE(d);
        WRITE_VALUE(ud);
        WRITE_VALUE(f);
        WRITE_OBJECT(so);           //ע��:���ṹ������Ƕ��ṹ��ʱ����Ƕ��Ľṹ�����Ҳ�ǿ����л���!!!
        WRITE_VALUE(chars);
        WRITE_VALUE(strs);
        WRITE_VALUE(li);
        WRITE_VALUE(vf);
        WRITE_VALUE(si);
        WRITE_VALUE(is);
        SERIALIZE_END();             //���л�����
    }

    //�����л�����
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MultiTypes);   //�����л���ʼ
        READ_VALUE(ch);
        READ_VALUE(bl);
        READ_VALUE(s);               //���������Ա
        READ_VALUE(a);
        READ_VALUE(b);
        READ_VALUE(ub);
        READ_VALUE(c);
        READ_VALUE(uc);
        READ_VALUE(d);
        READ_VALUE(ud);
        READ_VALUE(f);
        READ_OBJECT(so);             //�ṹ���ֶα���ʹ��READ_OBJECT���������л�
        READ_VALUE(chars);
        READ_VALUE(strs);
        READ_VALUE(li);
        READ_VALUE(vf);
        READ_VALUE(si);
        READ_VALUE(is);
        DESERIALIZE_END();              //�����л�����
    }
};

class OneConfig: public Serializable
{
public:
    string cpu;
    string memory;
    string name;
    Dict disk;
    Dict context;

    //���л�����
    SERIALIZE
    {
        SERIALIZE_BEGIN(OneConfig);  //���л���ʼ
        WRITE_VALUE(cpu);           //������ӳ�Ա
        WRITE_VALUE(memory);
        WRITE_VALUE(name);
        WRITE_VALUE(disk);      //���л�map<string,string>
        WRITE_VALUE(context);
        SERIALIZE_END();             //���л�����
    }

    //�����л�����
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(OneConfig);   //�����л���ʼ
        READ_VALUE(cpu);               //���������Ա
        READ_VALUE(memory);
        READ_VALUE(disk);          //�������л�map<string,string>
        READ_VALUE(name);
        READ_VALUE(context);
        DESERIALIZE_END();              //�����л�����
    }
};

class Network: public Serializable
{
public:
    Network()
    {
        m_name = "vif0";
        m_phy = "eth0";
        m_bridge = "xenbr0";
    }
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(Network);
        WRITE_VALUE(m_name);
        WRITE_VALUE(m_phy);
        WRITE_VALUE(m_bridge);
        SERIALIZE_END();
    }
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(Network);
        READ_VALUE(m_name);
        READ_VALUE(m_phy);
        READ_VALUE(m_bridge);
        DESERIALIZE_END();
    }
    
private:
    string m_name;
    string m_phy;
    string m_bridge;
};

class VMConfig: public Serializable
{
public:
    VMConfig()  {};

    virtual void Init()
    {
        m_vcpus = 2;
        m_kernel = "/usr/lib/xen/boot/hvmloader";
        m_disks.push_back("file:/home/data0/CentOS-HVM.img,hda,w");
        m_disks.push_back("hdc:cdrom,r");
        m_context["key1"] = "value1"; 
        m_context["key2"] = "value2"; 
        m_context["key3"] = "value3"; 

        Network nw1,nw2;
        m_networks.push_back(nw1);
        m_networks.push_back(nw2);
    }

    SERIALIZE
    {
        SERIALIZE_BEGIN(VMConfig);
        WRITE_VALUE(m_vcpus);
        WRITE_VALUE(m_kernel);
        WRITE_OBJECT_ARRAY(m_networks);  //Ƕ�׵Ľṹ���б��ԱNetwork����Ҳ�ǿ����л���
        WRITE_VALUE(m_disks);
        WRITE_VALUE(m_context);
        SERIALIZE_END();
    }
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VMConfig);
        READ_VALUE(m_vcpus);
        READ_VALUE(m_kernel);
        READ_OBJECT_ARRAY(m_networks);
        READ_VALUE(m_disks);
        READ_VALUE(m_context);
        DESERIALIZE_END();
    }
private:
    int m_vcpus;
    string m_kernel;
    list<Network> m_networks;
    list<string> m_disks;
    map<string,string> m_context; 
};

class XenVMConfig: public VMConfig    //��VMConfig�̳У�XenVMConfigҲ�ͳ�Ϊ�����л�����
{
public:
    void Init()
    {
        domid = 123456;
        VMConfig::Init();
    };

    SERIALIZE
    {
        SERIALIZE_BEGIN(XenVMConfig);   //���л���ʼ
        WRITE_VALUE(domid);            //�������л��Լ��ĳ�Ա
        SERIALIZE_PARENT_END(VMConfig); //�Ե��ø�������л�����������
    }
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(XenVMConfig);  //�����л���ʼ
        READ_VALUE(domid);              //���跴�����л��Լ��ĳ�Ա
        DESERIALIZE_PARENT_END(VMConfig); //�Ե��ø���ķ����л��������������ݲ�����֧�ֶ��ؼ̳е����      
    }
private:
    int domid;
};

void test1()
{
    VMConfig config;
    config.Init();
    string s = config.serialize();    
    cout << "----------------VMConfig serialize----------------\n" << s << endl;
}
DEFINE_DEBUG_FUNC(test1);

void test2()
{
    VMConfig config1;
    VMConfig config2;
    string s;

    Timespan clock;
    clock.Begin();
    config1.Init();
    s = config1.serialize();
    cout << "----------------config1 serialize----------------\n" << s << endl;

    config2.deserialize(s);
    s = config2.serialize();
    cout << "----------------config2 serialize----------------\n" << s << endl;
    clock.End();
    clock.ShowSpan();
    cout << "serialize string len = " << s.length() << endl;
}
DEFINE_DEBUG_FUNC(test2);

void test3()
{
    string s;
    OneConfig oneconfig1;
    oneconfig1.cpu = "0.1";
    oneconfig1.memory = "64";
    oneconfig1.name = "myttyserver";
    oneconfig1.disk["source"] = "/home/srv/cloud/images/ttylinux.0.4.img";
    oneconfig1.disk["target"] = "hda";
    oneconfig1.disk["readonly"] = "no";
    oneconfig1.context["hostname"] = "$NAME",   
    oneconfig1.context["files"] = "/home/oneadmin/one-templates/ttylinux/init.sh /home/oneadmin/one-templates/ttylinux/id_dsa.pub";  
    oneconfig1.context["target"] = "hdc";   
    oneconfig1.context["root_pubkey"] = "id_dsa.pub";
    oneconfig1.context["username"] = "oneadmin"; 
    oneconfig1.context["user_pubkey"] = "id_dsa.pub";
    
    Timespan clock;
    clock.Begin();
    s = oneconfig1.serialize();
    cout << "----------------oneconfig1 serialize----------------\n" << s << endl;
     
    OneConfig oneconfig2;
    oneconfig2.deserialize(s);
    s = oneconfig2.serialize();
    cout << "----------------oneconfig2 serialize----------------\n" << s << endl;
    clock.End();
    clock.ShowSpan();
    cout << "serialize string len = " << s.length() << endl;
}
DEFINE_DEBUG_FUNC(test3);

void test4()
{
    XenVMConfig xconfig1;
    XenVMConfig xconfig2;
    string s;

    Timespan clock;
    clock.Begin();
    xconfig1.Init();
    s = xconfig1.serialize();
    cout << "----------------xconfig1 serialize----------------\n" << s << endl;

    xconfig2.deserialize(s);
    s = xconfig2.serialize();
    cout << "----------------xconfig2 serialize----------------\n" << s << endl;
    clock.End();
    clock.ShowSpan();
    cout << "serialize string len = " << s.length() << endl;
}
DEFINE_DEBUG_FUNC(test4);

void test5()
{
    MultiTypes mt1;
    mt1.ch = 'x';
    mt1.bl = 1;
    mt1.s = "test only";
    mt1.a = -123;
    mt1.b = -456;
    mt1.ub = 789;
    mt1.c = -321;
    mt1.uc = 321;
    mt1.d = -654;
    mt1.ud = 654;
    mt1.f = 0.5;
    mt1.chars.push_back('o');
    mt1.chars.push_back('k');
    mt1.strs.push_back("hello");
    mt1.strs.push_back("world");
    mt1.li.push_back(41);
    mt1.li.push_back(42);
    mt1.li.push_back(43);
    
    mt1.vf.push_back(0.1);
    mt1.vf.push_back(0.2);
    mt1.vf.push_back(0.3);
    mt1.so.a = 123456;
    mt1.so.b = "this is a simple object!";
    mt1.si.insert(make_pair("first",123));
    mt1.si.insert(make_pair("second",321));
    mt1.is.insert(make_pair(123,"is first number"));
    mt1.is.insert(make_pair(321,"is second number"));
    string s1 = mt1.serialize();
    cout << "----------------mt1 serialize----------------\n" << s1 << endl;
    
    MultiTypes mt2;
    mt2.deserialize(s1);
    string s2 = mt2.serialize();
    cout << "----------------mt2 serialize----------------\n" << s2 << endl;
}
DEFINE_DEBUG_FUNC(test5);

void test6()
{
    Vectors v;
    v.intergers.push_back(123);
    v.intergers.push_back(456);
    v.intergers.push_back(789);

    v.charactors.push_back('x');
    v.charactors.push_back('y');
    v.charactors.push_back('z');

    //v.dict["name"] = "zwj";
    v.str = "hello,world";
    
    string s = v.serialize();
    cout << "----------------v serialize----------------\n" << s << endl;

    Vectors v2;
    v2.deserialize(s);
    cout << "v2.str = " << v2.str << endl;
    cout << "----------------v2 serialize----------------\n" << v2.serialize() << endl;
}
DEFINE_DEBUG_FUNC(test6);

void test7()
{
    EmbbedObj obj1;
    obj1.member = 5678;
    obj1.so.a = 1999;
    obj1.so.b = "this is test7!";
    
    string s1 = obj1.serialize();
    cout << "----------------obj1 serialize----------------\n" << s1 << endl;
    
    EmbbedObj obj2;
    obj2.deserialize(s1);
    string s2 = obj2.serialize();
    cout << "----------------obj2 serialize----------------\n" << s2 << endl;
}
DEFINE_DEBUG_FUNC(test7);

//��ʾ����string��ɵ�map������л��ͷ����л�
class cpu_info: public Serializable
{
public:
    map<string,string> info;
    map<int,string> info2;
    SERIALIZE
    {
        SERIALIZE_BEGIN(cpu_info);   //���л���ʼ
        WRITE_VALUE(info); 
        WRITE_VALUE(info2);
        SERIALIZE_END(); 
    }
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(cpu_info);  //�����л���ʼ
        READ_VALUE(info);      
        READ_VALUE(info2);           
        DESERIALIZE_END(); 
    }
};

void test8()
{
    cpu_info cpu1;
    cpu1.info["_model_name"] = "Intel(R) Core(TM)2 Duo CPU E8400 @ 3.00GHz";
    cpu1.info["_processor_num"] = "2";
    cpu1.info2[123] = "Intel(R) Core(TM)2 Duo CPU E8400 @ 3.00GHz";
    cpu1.info2[234] = "2";
    string s = cpu1.serialize();
    cout << "cpu1 = " << endl << s << endl;

    cpu_info cpu2;
    cpu2.deserialize(s);
    s = cpu2.serialize();
    cout << "cpu2 = " << endl << s << endl;
}
DEFINE_DEBUG_FUNC(test8);

//��ʾ������������������ɵ�map������л��ͷ����л�
class ImageVerify
{
public:
    //�����int64,int���Ի������������DIGIT
    map<int64,int> images;
    SERIALIZE
    {
        SERIALIZE_BEGIN(ImageVerify);   //���л���ʼ
        WRITE_VALUE(images);        
        SERIALIZE_END(); 
    }
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(ImageVerify);  //�����л���ʼ
        READ_VALUE(images);           
        DESERIALIZE_END(); 
    }
};

void test9()
{
    ImageVerify iv;
    iv.images[12] = 34;
    iv.images[56] = 78;    
    string s = iv.serialize();
    cout << "iv serialization" << endl << s << endl;

    ImageVerify iv2;
    iv2.deserialize(s);
    s = iv2.serialize();
    cout << "iv2 serialization" << endl << s << endl;
}
DEFINE_DEBUG_FUNC(test9);

int main()
{
    //test1();
    //test2();
    //test3();
    //test4();
    //test5();
    //test6();
    //test7();
    test8();
#if 1
    Shell("serializer-> ");
    while(1)
    {
        sleep(100);
    }
#endif
    return 0;
}


