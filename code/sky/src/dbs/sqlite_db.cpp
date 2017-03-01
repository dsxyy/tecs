/*********************************************************************
* 版权所有 (C)2011, 深圳市中兴通讯股份有限公司。
*
* 文件名称： sqlite_db.cpp
* 文件标识：
* 内容摘要： SqliteDB类实现文件
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
#ifdef TECS_USE_SQLITE

#include <iostream>

#include "sqlite_db.h"
#include "sqlite_sql_column.h"

using namespace std;


/* -------------------------------------------------------------------------- */

extern "C" int SqliteCallback (
        void *                  _obj,
        int                     num,
        char **                 values,
        char **                 names)
{
    Callbackable *obj;
    SqliteSQLColumn columns;

    obj = static_cast<Callbackable *>(_obj);

    if (obj == 0)
    {
        return -1;
    }

    columns.SetResult(num,values,names);
    return obj->DoCallback(&columns);
};

/* ************************************************************************** */
/* PostgreSqlDB constructor/destructor                                             */
/* ************************************************************************** */

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

SqliteDB::SqliteDB(string& db_name)
{
    _db_name = db_name;
    
    pthread_mutex_init(&_mutex,0);
    _bopen = false;
}

/* -------------------------------------------------------------------------- */

SqliteDB::~SqliteDB()
{
    pthread_mutex_destroy(&_mutex);

    if ( _bopen ) 
    {
        sqlite3_close(_db);
    }
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
int SqliteDB::Open()
{
    int rc;
    
    Lock();
    
    if ( _bopen )
    {
        UnLock();
        
        return 0;
    }
    
    rc = sqlite3_open(_db_name.c_str(), &_db);

    if ( rc != SQLITE_OK )
    {
        UnLock();
        return -1;
    }    
    _bopen = true;
    UnLock();
    return 0;
}

/* -------------------------------------------------------------------------- */
void SqliteDB::Close()
{
    Lock();
    if ( _bopen )
    {
        sqlite3_close(_db);
        _db = NULL;
        _bopen = false;
    }
    UnLock();
}


/* -------------------------------------------------------------------------- */

int SqliteDB::Execute(ostringstream& cmd, Callbackable* obj)
{
    int          rc;

    const char * c_str;
    string       str;

    int          counter = 0;
    char *       err_msg = 0;

    int   (*callback)(void*,int,char**,char**);
    void * arg;

    str   = cmd.str();
    c_str = str.c_str();

    callback = 0;
    arg      = 0;

    if ((obj != 0)&&(obj->IsCallBackSet()))
    {
        callback = SqliteCallback;
        arg      = static_cast<void *>(obj);
    }

    Lock();
    
    //如果数据库没有连接直接返回失败
    if ( !_bopen )
    {
        UnLock();
        return -1;
    }

    do
    {
        counter++;

        rc = sqlite3_exec(_db, c_str, callback, arg, &err_msg);

        if (rc == SQLITE_BUSY || rc == SQLITE_IOERR)
        {
            struct timeval timeout;
            fd_set zero;

            FD_ZERO(&zero);
            timeout.tv_sec  = 0;
            timeout.tv_usec = 250000;

            select(0, &zero, &zero, &zero, &timeout);
        }
    }while( (rc == SQLITE_BUSY || rc == SQLITE_IOERR) &&
            (counter < 10));

    UnLock();

    if (rc != SQLITE_OK)
    {
        if (err_msg != 0)
        {
            ostringstream oss;

            oss << "SQL command was: " << c_str << ", error: " << err_msg;
            /* cout << "ONE" << oss.str() << endl; */

            sqlite3_free(err_msg);
        }

        return -1;
    }

    return 0;
}

/* -------------------------------------------------------------------------- */

char * SqliteDB::EscapeString(const string& str)
{
    return sqlite3_mprintf("%q",str.c_str());
}



void SqliteDB::FreeString(char * str)
{
    sqlite3_free(str);
}
#endif /* TECS_USE_SQLITE */

