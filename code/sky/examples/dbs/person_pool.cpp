/*********************************************************************
* 版权所有 (C)2011, 深圳市中兴通讯股份有限公司。
*
* 文件名称： person_pool.cpp
* 文件标识：
* 内容摘要： PersonPool 类实现文件
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
**********************************************************************/
#include "person.h"

namespace zte_tecs
{
PersonPool *PersonPool::instance = NULL;
const char *PersonPool::table = "person";
const char *PersonPool::columns = "id,name,age,gender,family";
const char *PersonPool::db_bootstrap = "CREATE TABLE IF NOT EXISTS person("
  "id numeric(19,0) NOT NULL,"
  "name varchar(128) NOT NULL,"
  "age integer,"
  "gender varchar(16) NOT NULL,"
  "family varchar(4096),"
  "CONSTRAINT fk1 FOREIGN KEY (id) REFERENCES student (person_id) MATCH SIMPLE "
  "ON UPDATE NO ACTION ON DELETE CASCADE)";

//创建表
int PersonPool::BootStrap(SqlDB *db)
{
    ostringstream oss(PersonPool::db_bootstrap);
    int ret = db->Execute(oss);
    if(0 != ret)
    {
        return ret;
    }
    return ret;
}
    
/* -------------------------------------------------------------------------- */
int PersonPool::SelectCallback(void *nil, SqlColumn * columns)
{
    if ( NULL == columns || LIMIT != columns->get_column_num())
    {
        return -1;
    }
    
    Person* p = static_cast<Person *>(nil);
    columns->GetValue(PersonPool::ID,p->_id);
    columns->GetValue(PersonPool::NAME,p->_name);
    columns->GetValue(PersonPool::AGE,p->_age);
    columns->GetValue(PersonPool::GENDER,p->_gender);
    columns->GetValue(PersonPool::FAMILY,p->_family);
    return 0;
}

int PersonPool::Select(Person& person)
{
    ostringstream   oss;
    int             rc;
    oss << "SELECT " << PersonPool::columns << " FROM " << PersonPool::table << " WHERE id = " << person._id;
    SetCallback(static_cast<Callbackable::Callback>(&PersonPool::SelectCallback),&person);
    rc = db->Execute(oss, this);
    UnsetCallback();
    if ( 0 != rc && SQLDB_RESULT_EMPTY != rc) 
    {
        SkyAssert(false);
    }
    return rc;
}

int PersonPool::Insert(const Person& person)
{
    int rc = 0;
    SqlCommand sql(db,PersonPool::table,SqlCommand::WITH_TRANSACTION);
    sql.Add("id",person._id);
    sql.Add("name",person._name);
    sql.Add("age",person._age);
    sql.Add("gender",person._gender);
    sql.Add("family",person._family);
    rc = sql.Insert();
    if(rc != 0)
    {
        SkyAssert(false);
    }

    return rc;
}

int PersonPool::Update(const Person& person)
{
    int rc = 0;
    SqlCommand sql(db,PersonPool::table,SqlCommand::WITH_TRANSACTION);
    sql.Add("id",person._id);
    sql.Add("name",person._name);
    sql.Add("age",person._age);
    sql.Add("gender",person._gender);
    sql.Add("family",person._family);
    
    rc = sql.Update("WHERE id = " + to_string(person._id,dec));
    if(rc != 0)
    {
        SkyAssert(false);
    }
    return rc;
}
}  //namespace zte_tecs

