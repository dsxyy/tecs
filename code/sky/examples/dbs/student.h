/*********************************************************************
* 版权所有 (C)2011, 深圳市中兴通讯股份有限公司。
*
* 文件名称： student.h
* 文件标识：
* 内容摘要： Student 类定义文件
* 其它说明：
* 当前版本： 1.0
* 作    者： KIMI
* 完成日期： 2011-06-21
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
#ifndef DBS_STUDENT_SQL_H_
#define DBS_STUDENT_SQL_H_
#include "sky.h"
#include "pool_object_sql.h"
#include "person.h"

namespace zte_tecs
{
class Book
{
public:
    Book()
    {
    };
    
    Book(int64 owner,const string& name):
    _name(name)
    {

    };

    Book(int64 owner,const string& name,const string& description):
    _name(name),_description(description)
    {

    };

    int64 _owner;
    string _name;
    string _description;
};

class BookPool: public Callbackable
{
public:
    static BookPool* GetInstance(SqlDB *db = NULL)
    {
        if(NULL == instance)
        {
            SkyAssert(NULL != db);
            instance = new BookPool(db); 
        }
        
        return instance;
    };
    
    int SelectCallback(void *nil, SqlColumn * columns);
    int GetBooksByOwner(vector<Book>& books,int64 owner);
    int SetBooksByOwner(int64 owner,const vector<Book>& books);

    //创建表
    static int BootStrap(SqlDB *db)
    {
        ostringstream oss(BookPool::db_bootstrap);
        return db->Execute(oss);
    }
    
private:
    enum ColNames {
        OWNER  = 0,
        NAME = 1,
        DESCRIPTION  = 2,
        LIMIT = 3
    };
    
    BookPool(SqlDB *pDb)
    {
        db = pDb;
    };
    
    DISALLOW_COPY_AND_ASSIGN(BookPool);
    static BookPool *instance;
    SqlDB *db;
    static const char *table;
    static const char *columns;
    static const char *db_bootstrap;
};

/**
 @brief Student 类. 资源池对象测试类
 @li @b 其它说明：无
 */ 
class Student: public PoolObjectSQL
{
public:
    Student() { _oid = -1;};
    Student(int64 person_id, const string &department, const string& major);

    virtual ~Student();
    
    //根据需要组织查询结果
    //资源池dump回调函数中调用
    static int Dump(ostringstream& oss, SqlColumn * columns);
    
    //此处还应该定义一堆的属性的set get函数
    const string& get_name()
    {
        return _person._name;
    };

    void set_name(const string& name)
    {
        _person._name = name;
    };
    
    const int get_person_id()
    {
        return _person._id;
    }

    int get_age()
    {
        return _person._age;
    }

    void set_age(int age)
    {
        _person._age = age;
    }

    const Family& get_family()
    {
        return _person._family;
    }
    
    void set_family(const Family& family)
    {
        _person._family = family;
    }

    void add_book(const string& name,const string& description)
    {
        Book book(_oid,name,description);
        _books.push_back(book);
    }
    
protected:
    virtual int Select(SqlDB *db);
    virtual int Insert(SqlDB *db, string& error_str);
    virtual int Update(SqlDB *db);
    virtual int Drop(SqlDB *db);
    
    //创建表
    static int BootStrap(SqlDB *db)
    {
        ostringstream oss(Student::db_bootstrap);
        int ret = db->Execute(oss);
        if(0 != ret)
        {
            SkyAssert(false);
            return ret;
        }

        ret = BookPool::BootStrap(db);
        if(0 != ret)
        {
            SkyAssert(false);
            return ret;
        }
        return ret;
    }

    static int BootStrap2(SqlDB *db)
    {
        ostringstream oss(Student::db_view_bootstrap);
        int ret = db->Execute(oss);
        if(0 != ret)
        {
            SkyAssert(false);
        }
        return ret;
    }
    
private:
    //资源池类是他的友元类
    friend class StudentPool;
    
    enum ColNames {
        OID  = 0,
        PERSON_ID = 1,
        DEPARTMENT  = 2,
        MAJOR  = 3,
        LIMIT    = 4    
    };

    DISALLOW_COPY_AND_ASSIGN(Student);
    
    //查询回调函数
    int SelectCallback(void *nil, SqlColumn * columns);

    static const char * table;
    static const char * view;
    static const char * columns;
    static const char * db_bootstrap;
    static const char * db_view_bootstrap;
    //各个字段
    int64 _person_id;
    Person _person;
    string _department;
    string _major;
    vector<Book> _books;
};
}  //namespace zte_tecs

#endif /*DBS_STUDENT_SQL_H_*/

