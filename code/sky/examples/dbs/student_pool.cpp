/*********************************************************************
* 版权所有 (C)2011, 深圳市中兴通讯股份有限公司。
*
* 文件名称： student_pool.cpp
* 文件标识：
* 内容摘要： StudentPool 类实现文件
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

