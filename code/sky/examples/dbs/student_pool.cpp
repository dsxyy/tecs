/*********************************************************************
* ��Ȩ���� (C)2011, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ� student_pool.cpp
* �ļ���ʶ��
* ����ժҪ�� StudentPool ��ʵ���ļ�
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
**********************************************************************/
#include "student_pool.h"

namespace zte_tecs
{
StudentPool *StudentPool::instance = NULL;

/* -------------------------------------------------------------------------- */
int StudentPool::InitCallback(void *nil, SqlColumn * columns)
{
    if (NULL == columns || 2 != columns->get_column_num())
    {
        return -1;
    }
    
    int64 oid;
    string name;

    columns->GetValue(0,oid); 
    columns->GetValue(1,name); 
    _known_users.insert(make_pair(name,oid));
    return 0;
}

StudentPool::StudentPool(SqlDB * db):PoolSQL(db,Student::table)
{
}

StudentPool::~StudentPool()
{
}

STATUS StudentPool::Init()
{
    ostringstream sql;
    sql  << "SELECT oid,name FROM " <<  Student::view;
    SetCallback(static_cast<Callbackable::Callback>(&StudentPool::InitCallback));
    int rc = PoolSQL::_db->Execute(sql, this);
    UnsetCallback();
    if(0 != rc && SQLDB_RESULT_EMPTY != rc)
    {
        SkyAssert(false);
        return ERROR_DB_SELECT_FAIL;
    }
    return SUCCESS;
}
    
int64 StudentPool::Allocate(int64 *oid,Student& stu,string& error_str)
{
    if(_known_users.find(stu.get_name()) != _known_users.end())
    {
        cerr << "name " << stu.get_name() << " already exists!" << endl;
        *oid = _known_users[stu.get_name()];
        return *oid;
    }
    
    // Insert the Object in the pool
    *oid = PoolSQL::Allocate(stu, error_str);
    if (*oid != -1)
    {
        // Add the user to the map of known_users
        _known_users.insert(make_pair(stu.get_name(),*oid));
    }
    return *oid;
}
}  //namespace zte_tecs

