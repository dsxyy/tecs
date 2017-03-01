/*********************************************************************
* 版权所有 (C)2011, 深圳市中兴通讯股份有限公司。
*
* 文件名称： pool_sql.cpp
* 文件标识：
* 内容摘要： PoolSQL类定义实现
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
#include <climits>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <errno.h>
#include "pool_sql.h"

int SqlCallbackable::SelectCommon(SqlDB *db,const string& tablename,const string& columnname,const string& where,Callback cb, void *arg)
{
    if(!db || tablename.empty() || columnname.empty())
    {
        SkyAssert(false);
        return ERROR_INVALID_ARGUMENT;
    }
    ostringstream sql;
    sql << "SELECT " << columnname <<" FROM "  << tablename;
    if(!where.empty())
    {
        sql << " WHERE " << where;
    }
    SetCallback(cb,arg);
    int ret = db->Execute(sql, this);
    UnsetCallback();
    return ret;
}

int SqlCallbackable::SelectStringCallback(void *nil, SqlColumn *columns)
{
    if (!nil || !columns)
    {
        return -1;
    }
    string *pstr = static_cast<string*>(nil);
    columns->GetValue(0,*pstr);
    return 0;
}

int SqlCallbackable::SelectIntCallback(void *nil, SqlColumn *columns)
{
    if (!nil || !columns)
    {
        return -1;
    }
    int *p = static_cast<int*>(nil);
    columns->GetValue(0,*p);
    return 0;
}

int SqlCallbackable::SelectInt64Callback(void *nil, SqlColumn *columns)
{
    if (!nil || !columns)
    {
        return -1;
    }
    int64 *p = static_cast<int64*>(nil);
    columns->GetValue(0,*p);
    return 0;
}

int SqlCallbackable::SelectStringVecCallback(void *nil, SqlColumn *columns)
{
    if (!nil || !columns)
    {
        return -1;
    }
    string value;
    vector<string> *pstrs = static_cast<vector<string>*>(nil);
    columns->GetValue(0,value);
    pstrs->push_back(value);
    return 0;
}

int SqlCallbackable::SelectIntVecCallback(void *nil, SqlColumn *columns)
{
    if (!nil || !columns)
    {
        return -1;
    }
    int value = -1;
    columns->GetValue(0,value);
    vector<int> *p = static_cast<vector<int>*>(nil);
    p->push_back(value);
    return 0;
}

int SqlCallbackable::SelectInt64VecCallback(void *nil, SqlColumn *columns)
{
    if (!nil || !columns)
    {
        return -1;
    }
    
    int64 value = -1;
    columns->GetValue(0,value);
    vector<int64> *p = static_cast<vector<int64>*>(nil);
    p->push_back(value);
    return 0;
}

/* ************************************************************************** */
/* PoolSQL constructor/destructor                                             */
/* ************************************************************************** */

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

const unsigned int PoolSQL::_max_pool_size = 15000;

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

int PoolSQL::InitCallback(void *nil, SqlColumn *pColumns)
{
    _lastOID = pColumns->GetValueInt64(0);

    return 0;
}

/* -------------------------------------------------------------------------- */

PoolSQL::PoolSQL(SqlDB *db, const char * table): SqlCallbackable(db), _lastOID(-1)
{
    ostringstream   oss;
    pthread_mutexattr_t _attr;
    pthread_mutexattr_init(&_attr);

    pthread_mutexattr_settype(&_attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&_mutex, &_attr);

    SetCallback(static_cast<Callbackable::Callback>(&PoolSQL::InitCallback));

    oss << "SELECT MAX(oid) FROM " << table;

    _db->Execute(oss,this);

    UnsetCallback();
};

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

PoolSQL::~PoolSQL()
{
    map<int64,PoolObjectSQL *>::iterator  it;

    pthread_mutex_lock(&_mutex);

    for ( it = _pool.begin(); it != _pool.end(); it++)
    {
        it->second->Lock();

        delete it->second;
    }

    pthread_mutex_unlock(&_mutex);

    pthread_mutex_destroy(&_mutex);
}

/* ************************************************************************** */
/* PoolSQL public interface                                                   */
/* ************************************************************************** */

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

int64 PoolSQL::Allocate(
    PoolObjectSQL   *objsql,
    string&         error_str)
{
    int64 rc;
    
    if ( NULL == objsql )
    {
        return -1;
    }

    Lock();

    if (_lastOID == LLONG_MAX)
    {
        _lastOID = -1;
    }

    objsql->Lock();

    objsql->_oid = ++_lastOID;

    rc = objsql->Insert(_db,error_str);

    if ( rc != 0 )
    {
        _lastOID--;
        rc = -1;
    }
    else
    {
        rc = _lastOID;
    }

    objsql->UnLock();

    delete objsql;

    UnLock();

    return rc;
}


/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

int64 PoolSQL::Allocate(
    PoolObjectSQL   &objsql,
    string&         error_str)
{
    int64 rc;


    Lock();

    if (_lastOID == LLONG_MAX)
    {
        _lastOID = -1;
    }

    objsql.Lock();

    objsql._oid = ++_lastOID;

    rc = objsql.Insert(_db,error_str);

    if ( rc != 0 )
    {
        _lastOID--;
        rc = -1;
    }
    else
    {
        rc = _lastOID;
    }

    objsql.UnLock();

    UnLock();

    return rc;
}
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

PoolObjectSQL * PoolSQL::Get(
    int64     oid,
    bool    olock)
{
    map<int64,PoolObjectSQL *>::iterator  index;
    PoolObjectSQL *                     objectsql;
    int                                 rc;

    Lock();

    index = _pool.find(oid);

    if ( index != _pool.end() )
    {
        if ( index->second->IsValid() == false )
        {
            objectsql = 0;
        }
        else
        {
            objectsql = index->second;

            if ( olock == true )
            {
                objectsql->Lock();
            }
        }

        UnLock();

        return objectsql;
    }
    else
    {
        objectsql = Create();
        if ( NULL == objectsql )
        {
            UnLock();
            return NULL;            
        }
        objectsql->_oid = oid;

        rc = objectsql->Select(_db);

        if ( rc != 0 )
        {
            delete objectsql;

            UnLock();

            return NULL;
        }

        _pool.insert(make_pair(objectsql->_oid,objectsql));

        if ( olock == true )
        {
            objectsql->Lock();
        }

        _oid_queue.push(objectsql->_oid);

        if ( _pool.size() > _max_pool_size )
        {
            Replace();
        }

        UnLock();

        return objectsql;
    }
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
void PoolSQL::Put( PoolObjectSQL *objsql, bool lock)
{
    //如果获取对象时上锁，释放锁，对象不会被删除，在资源池队列满时才会被删除
    if ( lock )
    {
        objsql->UnLock();
    }
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

void PoolSQL::Replace()
{
    bool removed = false;
    int64  oid;
    int  rc;

    map<int64,PoolObjectSQL *>::iterator  index;

    while (!removed)
    {
        oid   = _oid_queue.front();
        index = _pool.find(oid);

        if ( index == _pool.end())
        {
            _oid_queue.pop();
            break;
        }

        rc = pthread_mutex_trylock(&(index->second->_mutex));

        if ( rc == EBUSY ) // In use by other thread, move to back
        {
            _oid_queue.pop();
            _oid_queue.push(oid);
        }
        else
        {
            PoolObjectSQL * tmp_ptr;

            tmp_ptr = index->second;
            _pool.erase(index);

            delete tmp_ptr;

            _oid_queue.pop();
            removed = true;
        }
    }
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

void PoolSQL::Clean()
{
    map<int64,PoolObjectSQL *>::iterator  it;

    Lock();

    for ( it = _pool.begin(); it != _pool.end(); it++)
    {
        it->second->Lock();

        delete it->second;
    }

    _pool.clear();

    UnLock();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
int PoolSQL:: SearchCallback(void * oids, SqlColumn *pColumns)
{
    vector<int64> *  oids_vector;

    oids_vector = static_cast<vector<int64> *>(oids);

    if ( pColumns->get_column_num() == 0 )
    {
        return -1;
    }

    oids_vector->push_back(pColumns->GetValueInt64(0));

    return 0;
}

/* -------------------------------------------------------------------------- */

int PoolSQL::Search(
    vector<int64>&    oids,
    const char *    table,
    const string&   where)
{
    ostringstream   sql;
    int             rc;

    SetCallback(static_cast<Callbackable::Callback>(&PoolSQL::SearchCallback),
                 static_cast<void *>(&oids));

    sql  << "SELECT oid FROM " <<  table << " WHERE " << where;

    rc = _db->Execute(sql, this);

    UnsetCallback();

    return rc;
}

