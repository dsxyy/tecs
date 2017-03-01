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
        SERIALIZE_BEGIN(SimpleObject);  //序列化开始，括号中填写类名
        WRITE_VALUE(a);         //所有基本数据类型(bool/char/int/float/double,
                                //以及int32/int64/uintptr/c++ string都用WRITE_VALUE宏来序列化)
        WRITE_VALUE(b);
        SERIALIZE_END();        //序列化结束
    }

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(SimpleObject); //反向序列化开始，括号中填写类名
        READ_VALUE(a);          //所有基本数据类型(bool/char/int/float/double,
                                //以及int32/int64/uintptr/c++ string都用READ_VALUE宏来反序列化)
        READ_VALUE(b);
        DESERIALIZE_END();      //反向序列化结束
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
        WRITE_OBJECT(so);       //嵌套的结构体使用WRITE_OBJECT序列化
        SERIALIZE_END();             
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(EmbbedObj);  
        READ_VALUE(member);
        READ_OBJECT(so);        //嵌套的结构体使用READ_OBJECT反序列化
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
        SERIALIZE_BEGIN(Vectors);  //序列化开始
        WRITE_VALUE(intergers);     //所有基本数据类型以及c++ stirng组成的vector/list/map都用WRITE_VALUE序列化
        WRITE_VALUE(charactors);
        WRITE_VALUE(dict);
        WRITE_VALUE(str);
        SERIALIZE_END();             //序列化结束
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(Vectors);  
        READ_VALUE(intergers);     //所有基本数据类型以及c++ stirng组成的vector/list/map都用READ_VALUE反序列化
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

    //注意: 当map的第一个成员为string类型时，其实际运行时内容必须是符合c/c++语法规范要求的"标志符"字符串，
    //例如"name"，"age"，"cpu1"，"diskN"等可以作为变量名称的语法字符串，不能是带有空格，引号，斜杠之类的字符串
    //map的第二个成员为string时，内容格式不限
    
    //序列化函数
    SERIALIZE
    {
        SERIALIZE_BEGIN(MultiTypes);  //序列化开始
        WRITE_VALUE(ch);            //序列化一个char类型字段
        WRITE_VALUE(bl);            //序列化一个bool类型字段
        WRITE_VALUE(s);             //序列化一个string容器类型字段
        WRITE_VALUE(a);             //序列化一个整型字段
        WRITE_VALUE(b);             //序列化一个int16类型字段
        WRITE_VALUE(ub);            // ....
        WRITE_VALUE(c);
        WRITE_VALUE(uc);
        WRITE_VALUE(d);
        WRITE_VALUE(ud);
        WRITE_VALUE(f);
        WRITE_OBJECT(so);           //注意:当结构体中又嵌入结构体时，被嵌入的结构体必须也是可序列化的!!!
        WRITE_VALUE(chars);
        WRITE_VALUE(strs);
        WRITE_VALUE(li);
        WRITE_VALUE(vf);
        WRITE_VALUE(si);
        WRITE_VALUE(is);
        SERIALIZE_END();             //序列化结束
    }

    //反序列化函数
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MultiTypes);   //反序列化开始
        READ_VALUE(ch);
        READ_VALUE(bl);
        READ_VALUE(s);               //逐个解析成员
        READ_VALUE(a);
        READ_VALUE(b);
        READ_VALUE(ub);
        READ_VALUE(c);
        READ_VALUE(uc);
        READ_VALUE(d);
        READ_VALUE(ud);
        READ_VALUE(f);
        READ_OBJECT(so);             //结构体字段必须使用READ_OBJECT来反向序列化
        READ_VALUE(chars);
        READ_VALUE(strs);
        READ_VALUE(li);
        READ_VALUE(vf);
        READ_VALUE(si);
        READ_VALUE(is);
        DESERIALIZE_END();              //反序列化结束
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

    //序列化函数
    SERIALIZE
    {
        SERIALIZE_BEGIN(OneConfig);  //序列化开始
        WRITE_VALUE(cpu);           //逐个增加成员
        WRITE_VALUE(memory);
        WRITE_VALUE(name);
        WRITE_VALUE(disk);      //序列化map<string,string>
        WRITE_VALUE(context);
        SERIALIZE_END();             //序列化结束
    }

    //反序列化函数
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(OneConfig);   //反序列化开始
        READ_VALUE(cpu);               //逐个解析成员
        READ_VALUE(memory);
        READ_VALUE(disk);          //反向序列化map<string,string>
        READ_VALUE(name);
        READ_VALUE(context);
        DESERIALIZE_END();              //反序列化结束
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
        WRITE_OBJECT_ARRAY(m_networks);  //嵌套的结构体列表成员Network必须也是可序列化的
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

class XenVMConfig: public VMConfig    //从VMConfig继承，XenVMConfig也就成为可序列化对象
{
public:
    void Init()
    {
        domid = 123456;
        VMConfig::Init();
    };

    SERIALIZE
    {
        SERIALIZE_BEGIN(XenVMConfig);   //序列化开始
        WRITE_VALUE(domid);            //仅需序列化自己的成员
        SERIALIZE_PARENT_END(VMConfig); //以调用父类的序列化函数来结束
    }
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(XenVMConfig);  //反序列化开始
        READ_VALUE(domid);              //仅需反向序列化自己的成员
        DESERIALIZE_PARENT_END(VMConfig); //以调用父类的反序列化函数来结束，暂不考虑支持多重继承的情况      
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

//演示两个string组成的map如何序列化和反序列化
class cpu_info: public Serializable
{
public:
    map<string,string> info;
    map<int,string> info2;
    SERIALIZE
    {
        SERIALIZE_BEGIN(cpu_info);   //序列化开始
        WRITE_VALUE(info); 
        WRITE_VALUE(info2);
        SERIALIZE_END(); 
    }
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(cpu_info);  //反序列化开始
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

//演示两个基本数据类型组成的map如何序列化和反序列化
class ImageVerify
{
public:
    //这里的int64,int可以换成其它任意的DIGIT
    map<int64,int> images;
    SERIALIZE
    {
        SERIALIZE_BEGIN(ImageVerify);   //序列化开始
        WRITE_VALUE(images);        
        SERIALIZE_END(); 
    }
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(ImageVerify);  //反序列化开始
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


