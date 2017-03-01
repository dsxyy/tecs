/*********************************************************************
* 版权所有 (C)2011, 深圳市中兴通讯股份有限公司。
*
* 文件名称： sqldb.h
* 文件标识：
* 内容摘要： SqlDB类定义文件
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
#ifndef DBS_SQL_DB_H_
#define DBS_SQL_DB_H_
#include <sstream>
#include "callbackable.h"

using namespace std;

// 禁用拷贝构造函数
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)
#endif

#define SQLDB_OK              0
#define SQLDB_ERROR          -1
#define SQLDB_RESULT_EMPTY    1
#define SQLDB_BEGIN_FAILED       -2
#define SQLDB_EXEC_SQL_FAILED    -3  
#define SQLDB_COMMIT_FAILED      -4
    

/**
 @brief SqlDB类。数据库访问抽象基类，不同的数据库访问实体类都必须从它派生，如：sqlite postgresql等
 @li @b 其它说明：无  stdint.h 
 */
class SqlDB
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:

    SqlDB(){};

    virtual ~SqlDB(){};
    
    /**************************************************************************/
    /**
    @brief 功能描述:打开用户指定的数据库。

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
    virtual int Open() = 0;

    /**************************************************************************/
    /**
    @brief 功能描述:设置数据库备用服务器地址和端口。

    @li @b 入参列表：
    @verbatim
       server 数据库备用服务器地址
       port   数据库备用服务器端口
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
    virtual void SetStandbyServer(const string& server, int port) = 0;
    
    /**************************************************************************/
    /**
    @brief 功能描述:关闭用户指定数据库。

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
    virtual void Close() = 0;    

    /**************************************************************************/
    /**
    @brief 功能描述:执行SQL命令。

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
    virtual int Execute(ostringstream& cmd, Callbackable* obj=0) = 0;

    /**************************************************************************/
    /**
    @brief 功能描述:以事务的方式执行多条SQL语句

    @li @b 入参列表：
    @verbatim
       cmd      SQL命令,可以是多条,不能有查询语句
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
    virtual int ExecuteTransaction(ostringstream& cmd) = 0;    

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

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    /**************************************************************************/       
    virtual char * EscapeString(const string& str) = 0;

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
    virtual void FreeString(char * str) = 0;
/*******************************************************************************
* 2. protected section
*******************************************************************************/
// protected:

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:
    DISALLOW_COPY_AND_ASSIGN(SqlDB);
};

class Transaction
{
public:
    static STATUS Enable(SqlDB *pDbConnection);
    static bool On();
    static STATUS Begin();
    static STATUS Append(const string& cmd);
    static STATUS Commit();
    static STATUS Cancel();
    static map<string,string>& KeyValue();
};

STATUS CreateTransactionKey();

class SqlCommand
{
public:
    enum SqlCmdOption
    {
        WITH_TRANSACTION = 1, //当前上下文如果已经启用事务，则自动使用事务模式
        WITHOUT_TRANSACTION = 2, //不检查当前上下文是否使用事务，直接执行sql命令
        UNKNOWN_OPTION = 3
    };
    
    enum VacuumOption
    {
        VACUUM_NORMAL = 1, 
        VACUUM_FULL = 2,
    };
	
    SqlCommand(SqlDB *db, const string& table):_db(db),_table(table)
    {
        _option = WITHOUT_TRANSACTION;
    }

    SqlCommand(SqlDB *db, const string& table, SqlCmdOption transaction):_db(db),_table(table)
    {
        SkyAssert(transaction < UNKNOWN_OPTION);
        _option = transaction;
    }

    SqlCommand(SqlDB *db, SqlCmdOption transaction):_db(db)
    {
        
        _option = transaction;
    }
    
    void Add(const string& field);
    void Add(const string& field,int value);
    void Add(const string& field,unsigned int value);
    void Add(const string& field,time_t value);
    void Add(const string& field,int64 value);
    void Add(const string& field,double value);
    void Add(const string& field,const string& value);
    void Add(const string& field,const Serializable& value);
    void AddSqlFunc(const string& field,const string& func);

    void SetTable(const string &table)
    {
       _table = table;
    };
    
    string SelectSql(const string& where = "");
    string InsertSql();
    string UpdateSql(const string& where = "");
    int Insert();
    int Update(const string& where = "");
    int Delete(const string& where = "");
    int Vacuum(VacuumOption option);
    int Execute(const string& sql);

    void Clear()
    {
        _fields.clear();
        _values.clear();
    };
    
private:
    bool DuplicationCheck(const string& field);
    string JoinStrings(const vector<string>& strings,const string& separator);

    SqlDB *_db;
    SqlCmdOption _option;
    string _table;
    vector<string> _fields;
    vector<string> _values;
};
#endif /*SQL_DB_H_*/


