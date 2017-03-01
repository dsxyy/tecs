/*********************************************************************
* 版权所有 (C)2011, 深圳市中兴通讯股份有限公司。
*
* 文件名称：student.cpp
* 文件标识：
* 内容摘要： Student 类实现文件
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
**********************************************************************/
#include "student.h"
namespace zte_tecs
{
const char *BookPool::table = "book";
const char *BookPool::columns = "owner,name,description";
const char *BookPool::db_bootstrap = "CREATE TABLE IF NOT EXISTS book("
  "owner numeric(19,0) NOT NULL,"
  "name varchar(256) NOT NULL,"
  "description varchar(4096),"
  "CONSTRAINT pk1 PRIMARY KEY (owner, name),"
  "CONSTRAINT fk1 FOREIGN KEY (owner) REFERENCES student (oid) MATCH SIMPLE "
  "ON UPDATE NO ACTION ON DELETE CASCADE)";

BookPool *BookPool::instance = NULL;

/* -------------------------------------------------------------------------- */
int BookPool::SelectCallback(void *nil, SqlColumn * columns)
{
    if ( NULL == columns || LIMIT != columns->get_column_num())
    {
        return -1;
    }
    
    Book book;
    columns->GetValue(BookPool::OWNER,book._owner);
    columns->GetValue(BookPool::NAME,book._name);
    columns->GetValue(BookPool::DESCRIPTION,book._description);
    vector<Book>* pbooks = static_cast<vector<Book> *>(nil);
    pbooks->push_back(book);
    return 0;
}

int BookPool::GetBooksByOwner(vector<Book>& books,int64 owner)
{
    ostringstream   oss;
    int rc = -1;
    oss << "SELECT " << columns << " FROM " << table << " WHERE owner = " << owner;
    SetCallback(static_cast<Callbackable::Callback>(&BookPool::SelectCallback),&books);
    rc = db->Execute(oss, this);
    UnsetCallback();
    if ( 0 != rc && SQLDB_RESULT_EMPTY != rc) 
    {
        SkyAssert(false);
    }
    return rc;
}

int BookPool::SetBooksByOwner(int64 owner,const vector<Book>& books)
{
    SqlCommand sql(db,BookPool::table,SqlCommand::WITH_TRANSACTION);
    int rc = -1;
    rc = sql.Delete(" WHERE owner = " + to_string(owner,dec));
    if (0 != rc && SQLDB_RESULT_EMPTY != rc) 
    {
        SkyAssert(false);
    return rc;
}

    vector<Book>::const_iterator it;
    for(it = books.begin(); it != books.end(); it++)
    {
        sql.Clear();
        sql.Add("owner",owner);
        sql.Add("name",it->_name);
        sql.Add("description",it->_description);
        rc = sql.Insert();
        if(rc != 0)
        {
            SkyAssert(false);
            break;
        }
    }
    return rc;
}

const char * Student::table = "student";
const char * Student::view = "student_fullinfo";
const char * Student::columns = "oid,person_id,department,major";
const char * Student::db_bootstrap = "CREATE TABLE IF NOT EXISTS student ("
    "oid numeric(19) primary key, "
    "person_id numeric(19),"
    "department varchar(1024),"
    "major varchar(1024),"
    "UNIQUE(person_id))";

const char * Student::db_view_bootstrap = "CREATE VIEW student_fullinfo AS "
    "SELECT student.oid, student.person_id, student.department, student.major, "
    "person.name, person.age, person.gender, person.family "
    "FROM person, student "
    "WHERE student.person_id = person.id ";

Student::Student(int64 person_id, const string& department, const string& major):
_department(department),_major(major)
{
    _person._id = person_id;
}

Student::~Student()
{
}

/* -------------------------------------------------------------------------- */
int Student::Select(SqlDB * db)
{
    ostringstream   oss;
    int rc = -1;
    int64           boid;
    //避免使用SELECT *，应该明确要查询的列，这样在表结构升级时才不会有问题
    oss << "SELECT " << Student::columns << " FROM " << Student::table << " WHERE oid = " << _oid;
    boid = _oid;
    _oid  = -1;
    SetCallback(static_cast<Callbackable::Callback>(&Student::SelectCallback));
    rc = db->Execute(oss, this);
    UnsetCallback();
    if ( 0 != rc && SQLDB_RESULT_EMPTY != rc) 
    {
        //数据库系统出错
        SkyAssert(false);
        return rc;
    }
    
    if (_oid != boid || SQLDB_RESULT_EMPTY == rc)
    {
        //数据库系统没问题，但是没找到记录
        return -1;
    }

    PersonPool *ppp = PersonPool::GetInstance(db);
    _person._id = _person_id; 
    //上面这一步很重要!!!有了唯一关键字，才能查找附属表中的内容
    rc = ppp->Select(_person);
    if ( 0 != rc && SQLDB_RESULT_EMPTY != rc) 
    {
        SkyAssert(false);
        return rc;
    }
        
    //从附属关系表中查找对应成员记录
    BookPool *pbp = BookPool::GetInstance(db);
    rc = pbp->GetBooksByOwner(_books,_oid);
    if ( 0 != rc && SQLDB_RESULT_EMPTY != rc) 
    {
        SkyAssert(false);
        return rc;
    }
    return 0;
}

/* -------------------------------------------------------------------------- */
int Student::Insert(SqlDB * db, string& error_str)
{
    int rc = -1;
    Transaction::Begin();
    SqlCommand sql(db,table,SqlCommand::WITH_TRANSACTION);
    sql.Add("oid",_oid);
    sql.Add("person_id",_person._id);
    sql.Add("department",_department);
    sql.Add("major",_major);    
    rc = sql.Insert();
    if(0 != rc)
    {
        Transaction::Cancel();
        SkyAssert(false);
        return rc;
    }

    PersonPool *ppp = PersonPool::GetInstance(db);
    rc = ppp->Insert(_person);
    if(0 != rc)
    {
        Transaction::Cancel();
        SkyAssert(false);
        return rc;
    }
    
    BookPool *pbp = BookPool::GetInstance(db);
    rc = pbp->SetBooksByOwner(_oid,_books);
    if(0 != rc)
    {
        Transaction::Cancel();
        SkyAssert(false);
        return rc;
    }

    return Transaction::Commit();
}

/* -------------------------------------------------------------------------- */
int Student::Update(SqlDB * db)
{
    int rc = -1;
    Transaction::Begin();
    SqlCommand sql(db,table,SqlCommand::WITH_TRANSACTION);
    //sql.Add("oid",_oid);
    //sql.Add("person_id",_person._id);
    sql.Add("department",_department);
    sql.Add("major",_major);

    //存在一对多的附属关系表时，必须使用事务模式
    rc = sql.Update("WHERE oid = " + to_string(_oid,dec));
    if(0 != rc)
    {
        SkyAssert(false);
        return rc;
    }

    //更新一对一的附属关系表
    PersonPool *ppp = PersonPool::GetInstance(db);
    rc = ppp->Update(_person);
    if(0 != rc)
    {
        SkyAssert(false);
        return rc;
    }

    //更新一对多的附属关系表
    BookPool *pbp = BookPool::GetInstance(db);
    rc = pbp->SetBooksByOwner(_oid,_books);
    if(0 != rc)
    {
        SkyAssert(false);
        return rc;
    }
    return Transaction::Commit();
}

/* -------------------------------------------------------------------------- */
int Student::Drop(SqlDB * db)        
{
    ostringstream   oss;
    oss << "DELETE FROM " << table << " WHERE oid = " << _oid ;
    int rc = db->Execute(oss);
    if ( 0 == rc )
    {
        set_valid(false);
    }
    return rc;    
}

/* -------------------------------------------------------------------------- */
int Student::SelectCallback(void *nil, SqlColumn * columns)
{
    if ( NULL == columns || LIMIT != columns->get_column_num())
    {
        return -1;
    }
    
    columns->GetValue(OID,_oid);
    columns->GetValue(PERSON_ID,_person_id);
    columns->GetValue(DEPARTMENT,_department);
    columns->GetValue(MAJOR,_major);
    return 0;
}
}  //namespace zte_tecs

