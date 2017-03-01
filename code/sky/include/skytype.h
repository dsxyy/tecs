#ifndef SKY_TYPE_H
#define SKY_TYPE_H
#include "pub.h"
#include "serialization.h"

//todo: 如果以后内建数据类型使用比较频繁，可以考虑为其增加各种运算符的重载
class SkyBool:public Serializable
{
public:
    friend ostream & operator << (ostream& os, const SkyBool& si)
    {
        os << si.value;
        return os;
    };

    SkyBool():value(false) {};
    SkyBool(bool val):value(val) {};
    SkyBool(const string& s):value(0) { deserialize(s); };
    SERIALIZE
    {
        return to_string<bool>(value,dec);
    };
    
    DESERIALIZE
    {
        if(!from_string<bool>(value,input,dec))
            return false;
        else
            return true;
    };

    bool value;
};

class SkyInt:public Serializable
{
public:
    friend ostream & operator << (ostream& os, const SkyInt& si)
    {
        os << si.value;
        return os;
    };

    SkyInt():value(0) {};
    SkyInt(int val):value(val) {};
    SkyInt(const string& s):value(0) { deserialize(s); };
    SERIALIZE
    {
        return to_string<int>(value,dec);
    };
    
    DESERIALIZE
    {
        if(!from_string<int>(value,input,dec))
            return false;
        else
            return true;
    };

    int value;
};
   
class SkyInt32:public Serializable
{
public:
    friend ostream & operator << (ostream& os, const SkyInt32& si)
    {
        os << si.value;
        return os;
    };
    
    SkyInt32():value(0) {};
    SkyInt32(int32 val):value(val) {};
    SkyInt32(const string& s):value(0) { deserialize(s); };
    SERIALIZE
    {
        return to_string<int32>(value,dec);
    };
    
    DESERIALIZE
    {
        if(!from_string<int32>(value,input,dec))
            return false;
        else
            return true;
    };
    int32 value;
};

class SkyInt64:public Serializable
{
public:
    friend ostream & operator << (ostream& os, const SkyInt64& si)
    {
        os << si.value;
        return os;
    };
    
    SkyInt64():value(0) {};
    SkyInt64(int64 val):value(val) {};
    SkyInt64(const string& s):value(0) { deserialize(s); };
    SERIALIZE
    {
        return to_string<int64>(value,dec);
    };
    
    DESERIALIZE
    {
        if(!from_string<int64>(value,input,dec))
            return false;
        else
            return true;
    };
    int64 value;
};

class SkyPtr:public Serializable
{
public:
    friend ostream & operator << (ostream& os, const SkyPtr& sp)
    {
        os << sp.value;
        return os;
    };
    
    SkyPtr():value(0) {};
    SkyPtr(uintptr val):value(val) {};
    SkyPtr(void *val):value((uintptr)val) {};
    SkyPtr(const string& s):value(0) { deserialize(s); };
    SERIALIZE
    {
        return "0x" + to_string<uintptr>(value,hex);
    };
    
    DESERIALIZE
    {
        if(!from_string<uintptr>(value,input,hex))
            return false;
        else
            return true;
    };
    uintptr value;
};    
#endif

