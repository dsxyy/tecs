#ifndef SKY_SERIALIZATION_H
#define SKY_SERIALIZATION_H
#include "pub.h"
#include "exception.h"

class Serializable
{
public:
    Serializable()  {};
    virtual ~Serializable() {};
    virtual string serialize() const = 0;
    virtual bool deserialize(const string&) = 0;
};

class XmlParserImpl;
class XmlParser
{
public:
    XmlParser();
    ~XmlParser();
    
    bool Create(const char *title);
    bool SetAttr(const char *name,const string& value);
    bool WriteString(const char *name,const string& s);
    bool WriteObject(const char *name,const Serializable& object);

    template<typename T> 
    bool Write(const char *name,const T& value)
    {
        string s = to_string<T>(value,dec);
        return WriteString(name,s);
    }

    template<typename T> 
    bool Write(const char *name,const vector<T>& values)
    {
        stringstream ss;
        string s;
        WriteString(name,"");
        if(true == values.empty())
            return true;
        
        Enter();

        typename vector<T>::const_iterator it;
        for (it=values.begin();it!=values.end();it++)
        {
            s = to_string<T>(*it,dec);
            WriteString("item",s);
        }
        Exit();
        return true;
    }

    template<typename T> 
    bool Write(const char *name,const list<T>& values)
    {
        stringstream ss;
        string s;
        WriteString(name,"");
        if(true == values.empty())
            return true;
            
        Enter();
        typename list<T>::const_iterator it;
        for (it=values.begin();it!=values.end();it++)
        {
            s = to_string<T>(*it,dec);
            WriteString("item",s);
        }
        Exit();
        return true;
    }

    template<typename T1,typename T2> 
    bool Write(const char *name,const map<T1,T2>& dict)
    {
        WriteString(name,"");
        if(true == dict.empty())
            return true;
        
        Enter();                                       
        typename map<T1,T2>::const_iterator it;
        for ( it=dict.begin() ; it != dict.end(); it++)
        {                                                  
            WriteString((to_string<T1>(it->first,dec)).c_str(),(to_string<T2>(it->second,dec)).c_str());
        }                                                  
        Exit();
        return true;
    }
    
    template<typename T>
    bool WriteObjArray(const char *name,const vector<T>& objects)
    {
        WriteString(name,"");                             
        if(true == objects.empty())
            return true;
            
        Enter();
        typename vector<T>::const_iterator it;                     
        for (it=objects.begin(); it != objects.end(); it++)  
        {
            WriteObject("item",*it);                         
        }
        Exit();
        return true;
    }
    
    template<typename T>
    bool WriteObjArray(const char *name,const list<T>& objects)
    {
        WriteString(name,"");                             
        if(true == objects.empty())
            return true;
            
        Enter();                                       
        typename list<T>::const_iterator it;                     
        for (it=objects.begin(); it != objects.end(); it++)  
        {                                                  
            WriteObject("item",*it);                         
        }                                                  
        Exit();
        return true;
    }
    
    bool Locate(const string& xmlstring,const char*name);
    bool Locate(const char*name);
    bool ReadString(const char *name,string& s);
    bool ReadObject(const char *name,Serializable& object);

    template<typename T> 
    bool Read(const char *name,T& value)
    {
        string s;
        if(false == ReadString(name, s))
            return false;
        return from_string(value,s,dec);
    }

    template<typename T> 
    bool Read(const char *name,vector<T>& values)
    {
        values.clear();
        if(false == Locate(name))
            return false;

        if(true == FirstItem())                     
        {                                               
            while(1)                                    
            {                                           
                string s = GetValue();          
                T t;
                from_string(t,s,dec);
                values.push_back(t);
                if(false == NextItem())             
                    break;                              
            }                                           
        }                                               
        Exit();   
        return true;
    }

    template<typename T> 
    bool Read(const char *name,list<T>& values)
    {
        values.clear();
        if(false == Locate(name))
            return false;

        if(true == FirstItem())                     
        {                                               
            while(1)                                    
            {                                           
                string s = GetValue();          
                T t;
                from_string(t,s,dec);
                values.push_back(t);
                if(false == NextItem())             
                    break;                              
            }                                           
        }                                               
        Exit();   
        return true;
    }
    
    template<typename T1,typename T2> 
    bool Read(const char *name,map<T1,T2>& dict)
    {
        dict.clear();
        if(false == Locate(name))
            return false;

        if(GotoFirst(NULL))                     
        {                                               
            while(1)                                    
            {                                           
                T1 t1;
                T2 t2;
                from_string<T1>(t1,GetName(),dec);
                from_string<T2>(t2,GetValue(),dec);
                dict.insert(make_pair(t1,t2));
                if(!GotoNext(NULL))             
                    break;                              
            }                                           
        }                                               
        Exit();   
        return true;
    }

    template<typename T> 
    bool ReadObjArray(const char *name,vector<T>& objects)
    {
        objects.clear();
        if(false == Locate(name))
            return false;
            
        if(true == FirstItem())                     
        {                                               
            while(1)                                    
            {                                           
                string s = ChildToString(NULL);          
                T object;                            
                object.deserialize(s);                  
                objects.push_back(object);               
                if(false == NextItem())             
                    break;                              
            }                                           
        }                                               
        Exit();   
        return true;
    }

    template<typename T> 
    bool ReadObjArray(const char *name,list<T>& objects)
    {
        objects.clear();
        if(false == Locate(name))
            return false;
            
        if(true == FirstItem())                     
        {                                               
            while(1)                                    
            {                                           
                string s = ChildToString(NULL);          
                T object;                            
                object.deserialize(s);                  
                objects.push_back(object);               
                if(false == NextItem())             
                    break;                              
            }                                           
        }                                               
        Exit();   
        return true;
    }
    
    void Enter();
    void Exit();
    bool FirstItem();
    bool NextItem();
    string ChildToString(const char *name);
    string DocToString();
    bool GotoFirst(const char* name);
    bool GotoNext(const char* name);
    string GetName();
    string GetValue();
    string GetAttr(const char *name);
private:
    XmlParserImpl *impl;
};

#define SERIALIZE     string serialize() const    
#define DESERIALIZE   bool deserialize(const string& input) 

#define SERIALIZE_BEGIN(classname) \
    XmlParser xmlw; \
    xmlw.Create(#classname);

#define WRITE_VALUE(member)                 xmlw.Write(#member,member);
#define WRITE_OBJECT(member)                xmlw.WriteObject(#member,member);
#define WRITE_OBJECT_ARRAY(member)          xmlw.WriteObjArray(#member,member);
#define SERIALIZE_END()                     return xmlw.DocToString();
#define SERIALIZE_PARENT_END(parent)        return xmlw.DocToString() + parent::serialize();

#define DESERIALIZE_BEGIN(classname)    \
    XmlParser xmlr;                      \
    if(!xmlr.Locate(input,#classname))   \
    {                                   \
        return false;                   \
    }

#define READ_VALUE(member)                  xmlr.Read(#member,member);
#define READ_OBJECT(member)                 xmlr.ReadObject(#member,member);
#define READ_OBJECT_ARRAY(member)           xmlr.ReadObjArray(#member,member);
#define DESERIALIZE_END()                   return true;
#define DESERIALIZE_PARENT_END(parent)      return parent::deserialize(input);  

/* 以下DIGIT/STRING相关宏定义是对旧有代码的兼容而设，
新增代码请直接使用后面定义体中的WRITE_VALU和READ_VALUE */
#define WRITE_DIGIT                         WRITE_VALUE
#define WRITE_DIGIT_VECTOR                  WRITE_VALUE
#define WRITE_DIGIT_LIST                    WRITE_VALUE
#define WRITE_DIGIT_MAP                     WRITE_VALUE
#define WRITE_STRING                        WRITE_VALUE
#define WRITE_STRING_VECTOR                 WRITE_VALUE
#define WRITE_STRING_LIST                   WRITE_VALUE
#define WRITE_STRING_MAP                    WRITE_VALUE
#define WRITE_OBJECT_VECTOR                 WRITE_OBJECT_ARRAY
#define WRITE_OBJECT_LIST                   WRITE_OBJECT_ARRAY
#define READ_DIGIT                          READ_VALUE
#define READ_DIGIT_VECTOR                   READ_VALUE
#define READ_DIGIT_LIST                     READ_VALUE
#define READ_DIGIT_MAP                      READ_VALUE
#define READ_STRING                         READ_VALUE
#define READ_STRING_VECTOR                  READ_VALUE
#define READ_STRING_LIST                    READ_VALUE
#define READ_STRING_MAP                     READ_VALUE
#define READ_OBJECT_VECTOR                  READ_OBJECT_ARRAY
#define READ_OBJECT_LIST                    READ_OBJECT_ARRAY
#endif



