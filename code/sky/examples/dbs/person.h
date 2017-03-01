/*********************************************************************
* 版权所有 (C)2012, 深圳市中兴通讯股份有限公司。
*
* 文件名称： person.h
* 文件标识：
* 内容摘要： Person 类定义文件
* 其它说明：
* 当前版本： 1.0
* 作    者： 张文剑
* 完成日期： 2012-06-20
*
*    修改记录1：// 修改历史记录，包括修改日期、修改者及修改内容
*    修改日期：
*    版 本 号：
*    修 改 人：
*    修改内容：
* 修改记录2：…
* 修 改 人：
* 修改日期：
* 修改内容：
*********************************************************************/
#ifndef DBS_PERSON_SQL_H_
#define DBS_PERSON_SQL_H_
#include "sky.h"
#include "pool_object_sql.h"

namespace zte_tecs
{
class Family: public Serializable
{
public:
    Family():address("nanjing 68"),postcode("210012")
    {
        persons = 4;
    }
    
    string address;
    string postcode;
    int    persons;

    SERIALIZE
    {
        SERIALIZE_BEGIN(Family);
        WRITE_VALUE(address);
        WRITE_VALUE(postcode);
        WRITE_VALUE(persons);
        SERIALIZE_END();
    }

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(Family);
        READ_VALUE(address);
        READ_VALUE(postcode);
        READ_VALUE(persons);
        DESERIALIZE_END();
    }
};

class Person: public Serializable
{
public:
    Person():_name("noname"),_gender("man")
    {
        _id = -1;
        _age = 0;
    };
    
    Person(int64 id,const string& name):
    _id(id),_name(name)
    {

    };

    Person(int64 id,const string& name,const string& gender):
    _id(id),_name(name),_gender(gender)
    {

    };
    
    int64 _id;
    string _name;
    string _gender;
    int _age;
    Family _family;

    SERIALIZE
    {
        SERIALIZE_BEGIN(Person);
        WRITE_VALUE(_name);
        WRITE_VALUE(_gender);
        WRITE_VALUE(_age);
        WRITE_OBJECT(_family);
        SERIALIZE_END();
    }

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(Person);
        READ_VALUE(_name);
        READ_VALUE(_gender);
        READ_VALUE(_age);
        READ_OBJECT(_family);
        DESERIALIZE_END();
    }
};

class PersonPool: public Callbackable
{
public:
    static PersonPool* GetInstance(SqlDB *db = NULL)
    {
        if(NULL == instance)
        {
            SkyAssert(NULL != db);
            instance = new PersonPool(db); 
        }
        
        return instance;
    };
    
    int Select(Person& person);
    int Insert(const Person& person);
    int Update(const Person& person);
    static int BootStrap(SqlDB *db);

private:
    enum ColNames {
        ID  = 0,
        NAME = 1,
        AGE  = 2,
        GENDER  = 3,
        FAMILY = 4,
        LIMIT    = 5    
    };

    PersonPool(SqlDB *pDb)
    {
        db = pDb;
    };

    int SelectCallback(void *nil, SqlColumn * columns);
    DISALLOW_COPY_AND_ASSIGN(PersonPool);
    static PersonPool *instance;
    SqlDB *db;
    static const char *table;
    static const char *columns;
    static const char *db_bootstrap;
};
}
#endif /*DBS_STUDENT_SQL_H_*/

