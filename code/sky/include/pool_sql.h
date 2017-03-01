/*********************************************************************
* 版权所有 (C)2011, 深圳市中兴通讯股份有限公司。
*
* 文件名称： pool_sql.h
* 文件标识：
* 内容摘要： PoolSQL类定义文件
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
#ifndef DBS_POOL_SQL_H_
#define DBS_POOL_SQL_H_

#include <stdint.h>
#include <map>
#include <string>
#include <queue>

#include "sqldb.h"
#include "pool_object_sql.h"
#include "sql_column.h"

using namespace std;

// 禁用拷贝构造函数
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)
#endif

class SqlCallbackable: public Callbackable
{
public:
    SqlCallbackable() 
    {
    };

    SqlCallbackable(SqlDB *db)
    {
        _db = db;
    };

    int SelectColumn(const string& tablename,const string& columnname,const string& where, string& value)
    {
        return SelectCommon(_db,
                            tablename,
                            columnname,
                            where,
                            static_cast<Callbackable::Callback>(&SqlCallbackable::SelectStringCallback),
                            (void *)&value);
    }
    
    int SelectColumn(const string& tablename,const string& columnname,const string& where, int& value)
    {
        return SelectCommon(_db,
                            tablename,
                            columnname,
                            where,
                            static_cast<Callbackable::Callback>(&SqlCallbackable::SelectIntCallback),
                            (void *)&value);
    }
    
    int SelectColumn(const string& tablename,const string& columnname,const string& where, int64& value)
    {
        return SelectCommon(_db,
                            tablename,
                            columnname,
                            where,
                            static_cast<Callbackable::Callback>(&SqlCallbackable::SelectInt64Callback),
                            (void *)&value);
    }

    int SelectColumn(const string& tablename,const string& columnname,const string& where, vector<string>& value)
    {
        return SelectCommon(_db,
                            tablename,
                            columnname,
                            where,
                            static_cast<Callbackable::Callback>(&SqlCallbackable::SelectStringVecCallback),
                            (void *)&value);
    }

    int SelectColumn(const string& tablename,const string& columnname,const string& where, vector<int>& value)
    {
        return SelectCommon(_db,
                            tablename,
                            columnname,
                            where,
                            static_cast<Callbackable::Callback>(&SqlCallbackable::SelectIntVecCallback),
                            (void *)&value);
    }

    int SelectColumn(const string& tablename,const string& columnname,const string& where, vector<int64>& value)
    {
        return SelectCommon(_db,
                            tablename,
                            columnname,
                            where,
                            static_cast<Callbackable::Callback>(&SqlCallbackable::SelectInt64VecCallback),
                            (void *)&value);
    }

protected:
    SqlDB *_db;
    
private:
    int SelectCommon(SqlDB *db,
                    const string& tablename,
                    const string& columnname,
                    const string& where,
                    Callback cb, 
                    void *arg = 0);
    int SelectStringCallback(void *nil, SqlColumn *columns);
    int SelectIntCallback(void *nil, SqlColumn *columns);
    int SelectInt64Callback(void *nil, SqlColumn *columns);
    int SelectStringVecCallback(void *nil, SqlColumn *columns);
    int SelectIntVecCallback(void *nil, SqlColumn *columns);
    int SelectInt64VecCallback(void *nil, SqlColumn *columns);
};

/**
 @brief PoolSQL类。持久化对象资源池类，提供多线程并发访问保护
 @li @b 其它说明：无
 */
 
class PoolSQL: public SqlCallbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    /**************************************************************************/
    /**
    @brief 功能描述:初始化对象标识计数器，从相应的数据库表中获取最大的对象标识。

    @li @b 入参列表：
    @verbatim
       db       数据库对象
       table    持久化对象对应的表
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
    PoolSQL(SqlDB * db, const char * table);

    virtual ~PoolSQL();

    /**************************************************************************/
    /**
    @brief 功能描述:分配一个新的对象，并插入到数据库中，但是缓存中没有。

    @li @b 入参列表：
    @verbatim
       objsql       已经初始化的持久化对象
    @endverbatim

    @li @b 出参列表：
    @verbatim
       error_str    详细的错误信息
    @endverbatim

    @li @b 返回值列表：
    @verbatim
       -1       失败
       其他     成功分配一个新的对象
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    /**************************************************************************/     
    virtual int64 Allocate(
        PoolObjectSQL   *objsql,
        string&          error_str);

    /**************************************************************************/
    /**
    @brief 功能描述:分配一个新的对象，并插入到数据库中，但是缓存中没有。

    @li @b 入参列表：
    @verbatim
       objsql       已经初始化的持久化对象
    @endverbatim

    @li @b 出参列表：
    @verbatim
       error_str    详细的错误信息
    @endverbatim

    @li @b 返回值列表：
    @verbatim
       -1       失败
       其他     成功分配一个新的对象
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    /**************************************************************************/     
    virtual int64 Allocate(
        PoolObjectSQL   &objsql,
        string&          error_str ); 
                
    /**************************************************************************/
    /**
    @brief 功能描述:从资源池获取指定的对象，资源池不存在从数据库获取。

    @li @b 入参列表：
    @verbatim
       oid       持久化对象标识
       lock      是否对对象上锁
    @endverbatim

    @li @b 出参列表：
    @verbatim
       无
    @endverbatim

    @li @b 返回值列表：
    @verbatim
       NULL       失败
       其他       成功
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    /**************************************************************************/
    virtual PoolObjectSQL * Get(
        int64     oid,
        bool    lock);

    /**************************************************************************/
    /**
    @brief 功能描述:如果获取对象时上锁，释放锁，对象不会被删除，在资源池队列满时才会被删除。

    @li @b 入参列表：
    @verbatim
       objsql    持久化对象
       lock      是否对对象上锁
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
    virtual void Put(
        PoolObjectSQL *objsql,
        bool    lock);	
	
    /**************************************************************************/
    /**
    @brief 功能描述:查询满足条件的一组对象。

    @li @b 入参列表：
    @verbatim
       table       持久化对象对应的关系表
       where       SQL语句的where查询条件
    @endverbatim

    @li @b 出参列表：
    @verbatim
       oids        持久化对象标识数组
    @endverbatim

    @li @b 返回值列表：
    @verbatim
       0          失败
       其他       成功
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    /**************************************************************************/     
    virtual int Search(
        vector<int64>&    oids,
        const char *    table,
        const string&   where);

    /**************************************************************************/
    /**
    @brief 功能描述:更新数据库中持久化对象对应的记录

    @li @b 入参列表：
    @verbatim
       objsql       持久化对象
    @endverbatim

    @li @b 出参列表：
    @verbatim
       无
    @endverbatim

    @li @b 返回值列表：
    @verbatim
       0          失败
       其他       成功
    @endverbatim

    @li @b 接口限制：对象必须已经上锁
    @li @b 其它说明：无
    */
    /**************************************************************************/      
    virtual int Update(
        PoolObjectSQL * objsql)
    {
        int rc;

        rc = objsql->Update(_db);

        return rc;
    };

    /**************************************************************************/
    /**
    @brief 功能描述:删除数据库中持久化对象对应的记录

    @li @b 入参列表：
    @verbatim
       objsql       持久化对象
    @endverbatim

    @li @b 出参列表：
    @verbatim
       无
    @endverbatim

    @li @b 返回值列表：
    @verbatim
       0          失败
       其他       成功
    @endverbatim

    @li @b 接口限制：对象必须已经上锁
    @li @b 其它说明：无
    */
    /**************************************************************************/     
    virtual int Drop(
        PoolObjectSQL * objsql)
    {
       return objsql->Drop(_db);
    };

    /**************************************************************************/
    /**
    @brief 功能描述:清除资源池中所有的持久化对象

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
    void Clean();

    /**************************************************************************/
    /**
    @brief 功能描述:将资源池中的持久化对象以XML格式输出

    @li @b 入参列表：
    @verbatim
       oss     输出流
       where   SQL语句where查询条件
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
    virtual int Dump(ostringstream& oss, const string& where) = 0;

    /**
    @brief 功能描述:获取最大对象标识
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
      int64 最大对象标识
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    /**************************************************************************/      
    int64 get_lastOID()
    {
        return _lastOID;
    };

    /**
    @brief 功能描述:设置最大对象标识
    @li @b 入参列表：
    @verbatim
       int64  new_oid 要设置的最大对象标识
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
    void set_lastOID(int64  new_oid)
    {
        _lastOID = new_oid;
    };
    
/*******************************************************************************
* 3. private section
*******************************************************************************/
private:
    //锁住资源池
    void Lock()
    {
        pthread_mutex_lock(&_mutex);
    };

    //解锁资源池
    void UnLock()
    {
        pthread_mutex_unlock(&_mutex);
    };

    //抽象对象工厂方法，创建一个持久化对象
    virtual PoolObjectSQL * Create() = 0;
    
    DISALLOW_COPY_AND_ASSIGN(PoolSQL);
    
    /**************************************************************************/
    /**
    @brief 功能描述:使用类似FIFO的策略。在删除对象之前会查看对象是否被锁住，没有锁住会被删除，否则插入到队列尾

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

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    /**************************************************************************/     
    void Replace();

    //设置持久化对象标识的回调汉书
    int  InitCallback(void *nil, SqlColumn * pColumns);

    //查询满足条件的一组对象的回调函数
    int  SearchCallback(void *oids, SqlColumn * pColumns);
    
    //控制并发的互斥信号量
    pthread_mutex_t             _mutex;

    //资源池缓冲区的大小，如果资源池中对象总数大于它，会调用replace删除队列头的对象
    static const unsigned int   _max_pool_size;

    //最大对象标识
    int64                         _lastOID;

    //对象标识/对象指针的map对象
    map<int64,PoolObjectSQL *>    _pool;

    //OID队列，用来实现资源池缓存的FIFO替换策略
    queue<int64>                  _oid_queue;
};
#endif /*DBS_POOL_SQL_H_*/

