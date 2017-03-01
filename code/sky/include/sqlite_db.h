/*********************************************************************
* 版权所有 (C)2011, 深圳市中兴通讯股份有限公司。
*
* 文件名称：sqlite_db.h
* 文件标识：
* 内容摘要： SqliteDB类定义文件
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
#ifndef DBS_SQLITE_DB_H_
#define DBS_SQLITE_DB_H_
#ifdef TECS_USE_SQLITE
#include <string>
#include <sstream>
#include <stdexcept>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sqlite3.h>

#include "sqldb.h"
#include "object_sql.h"

using namespace std;

 // 禁用拷贝构造函数
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)
#endif

/**
 @brief SqliteDB类. SQLite数据库访问类
 @li @b 其它说明：无
 */
class SqliteDB : public SqlDB
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:

    SqliteDB(string& db_name);

    ~SqliteDB();

    /**************************************************************************/
    /**
    @brief 功能描述:打开用户指定的sqlite数据库。

    @li @b 入参列表：
    @verbatim
       无
    @endverbatim

    @li @b 出参列表：
    @verbatim
       无
    @endverbatim

    @li @b 返回值列表：
    @verbatim
       0       成功
       其他    失败
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    /**************************************************************************/
    virtual int Open();
    
    /**************************************************************************/
    /**
    @brief 功能描述:关闭用户指定的sqlite数据库。

    @li @b 入参列表：
    @verbatim
       无
    @endverbatim

    @li @b 出参列表：
    @verbatim
       无
    @endverbatim

    @li @b 返回值列表：
    @verbatim
      无
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    /**************************************************************************/
    virtual void Close();    

    /**************************************************************************/
    /**
    @brief 功能描述:SQLite数据库执行SQL命令。

    @li @b 入参列表：
    @verbatim
       cmd      SQL命令
       obj      回调函数 查询时使用，结果集中的每一行数据都会调用
    @endverbatim

    @li @b 出参列表：
    @verbatim
       无
    @endverbatim

    @li @b 返回值列表：
    @verbatim
       0       成功
       其他    失败
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    /**************************************************************************/
    int Execute(ostringstream& cmd, Callbackable* obj=0);

    /**************************************************************************/
    /**
    @brief 功能描述:将应用传入的字符串转换为合法的SQL语句字符串

    @li @b 入参列表：
    @verbatim
       str      应用传入的字符串
    @endverbatim

    @li @b 出参列表：
    @verbatim
       无
    @endverbatim

    @li @b 返回值列表：
    @verbatim
       非NULL       有效的SQL字符串缓冲区首地址
       NULL         失败
    @endverbatim

    @li @b 接口限制：需要调用free_str释放动态分配的SQL字符串
    @li @b 其它说明：无
    */
    /**************************************************************************/
    char * EscapeString(const string& str);

    /**************************************************************************/
    /**
    @brief 功能描述:释放动态分配的字符串

    @li @b 入参列表：
    @verbatim
       str      动态分配的字符串的首地址
    @endverbatim

    @li @b 出参列表：
    @verbatim
       无
    @endverbatim

    @li @b 返回值列表：
    @verbatim
       无
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    /**************************************************************************/
    void FreeString(char * str);
    
/*******************************************************************************
* 2. protected section
*******************************************************************************/
//protected:
/*******************************************************************************
* 3. private section
*******************************************************************************/ 
private:

    DISALLOW_COPY_AND_ASSIGN(SqliteDB);

    //对库上锁
    void Lock()
    {
        pthread_mutex_lock(&_mutex);
    };

    //对库解锁
    void UnLock()
    {
        pthread_mutex_unlock(&_mutex);
    };
    
    //并发控制信号量
    pthread_mutex_t     _mutex;

    //SQLite数据库连接句柄
    sqlite3             *_db;
    
    //数据库名称
    string               _db_name;
    
    //数据库是否连接
    bool                 _bopen;
};
#endif /* TECS_USE_SQLITE */
#endif /*DBS_SQLITE_DB_H_*/

