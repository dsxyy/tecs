/*********************************************************************
* ��Ȩ���� (C)2011, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ� person_pool.cpp
* �ļ���ʶ��
* ����ժҪ�� PersonPool ��ʵ���ļ�
* ����˵����
* ��ǰ�汾�� 1.0
* ��    �ߣ� ���Ľ�
* ������ڣ� 2012-06-20
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

//������
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

