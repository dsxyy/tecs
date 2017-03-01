/*********************************************************************
* ��Ȩ���� (C)2011, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ� student.h
* �ļ���ʶ��
* ����ժҪ�� Student �ඨ���ļ�
* ����˵����
* ��ǰ�汾�� 1.0
* ��    �ߣ� KIMI
* ������ڣ� 2011-06-21
*
*    �޸ļ�¼1��// �޸���ʷ��¼�������޸����ڡ��޸��߼��޸�����
*    �޸����ڣ�
*    �� �� �ţ�
*    �� �� �ˣ�
*    �޸����ݣ�
* �޸ļ�¼2����
* �� �� �ˣ�
* �޸����ڣ�
* �޸����ݣ�
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

    //������
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
 @brief Student ��. ��Դ�ض��������
 @li @b ����˵������
 */ 
class Student: public PoolObjectSQL
{
public:
    Student() { _oid = -1;};
    Student(int64 person_id, const string &department, const string& major);

    virtual ~Student();
    
    //������Ҫ��֯��ѯ���
    //��Դ��dump�ص������е���
    static int Dump(ostringstream& oss, SqlColumn * columns);
    
    //�˴���Ӧ�ö���һ�ѵ����Ե�set get����
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
    
    //������
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
    //��Դ������������Ԫ��
    friend class StudentPool;
    
    enum ColNames {
        OID  = 0,
        PERSON_ID = 1,
        DEPARTMENT  = 2,
        MAJOR  = 3,
        LIMIT    = 4    
    };

    DISALLOW_COPY_AND_ASSIGN(Student);
    
    //��ѯ�ص�����
    int SelectCallback(void *nil, SqlColumn * columns);

    static const char * table;
    static const char * view;
    static const char * columns;
    static const char * db_bootstrap;
    static const char * db_view_bootstrap;
    //�����ֶ�
    int64 _person_id;
    Person _person;
    string _department;
    string _major;
    vector<Book> _books;
};
}  //namespace zte_tecs

#endif /*DBS_STUDENT_SQL_H_*/

