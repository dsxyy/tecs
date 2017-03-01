/*********************************************************************
* 版权所有 (C)2011, 深圳市中兴通讯股份有限公司。
*
* 文件名称： postgresql_db.h
* 文件标识：
* 内容摘要： PostgreSqlDB类定以文件
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
#ifndef DBS_POSTGRESQLDB_H_
#define DBS_POSTGRESQLDB_H_

#include "libpq-fe.h"
#include "sqldb.h"

using namespace std;

// 禁用拷贝构造函数
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)
#endif

/**
 @brief PostgreSqlDB类. PostgreSQL数据库访问类
 @li @b 其它说明：无
 */ 
 
class PostgreSqlDB : public SqlDB
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    PostgreSqlDB(const string& server,
            int           port,
            const string& user,
            const string& password,
            const string& database);

    virtual ~PostgreSqlDB();
    
    /**************************************************************************/
    /**
    @brief 功能描述:打开用户指定的PostgreSQL数据库。

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
    virtual void SetStandbyServer(const string& server, int port);
    
    /**************************************************************************/
    /**
    @brief 功能描述:关闭用户指定的PostgreSQL数据库。

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
    @brief 功能描述:PostgreSQL数据库执行SQL命令。

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
    virtual int ExecuteTransaction(ostringstream& cmd);  
    
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

    DISALLOW_COPY_AND_ASSIGN(PostgreSqlDB);

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
    
    //创建数据库
    int CreateDB();
        
    //执行DDL数据定义命令，因为postgresql不支持 if not exists
    //目前只支持create命令
    int ExecDDLcommand(ostringstream& cmd);
    
    //判断是否是DDL数据定义命令，目前只支持create命令
    bool IsDDLcommand(ostringstream& cmd);
    
    //判断当前的连接是否是主机
    bool  IsMaster(PGconn *db);
    
    //判断当前的连接是否是备机
    bool IsReadonly(PGresult *res);

    //连接数据库
    int ConnectDB( bool isMaster, const char *dbname,PGconn **db);
    
    //连接主数据库服务器
    int RetryConnectMasterDB();
    
    //将字符串全部转换为大写
    void UpperString( string & str );    
    
    //PostgreSQL数据库连接句柄
    PGconn *_db;
    
    //并发控制信号量
    pthread_mutex_t     _mutex;
    
    //服务器名称或地址
    string    _server;
    
    //连接端口
    int       _port;
    
    //用户名
    string    _user;
    
    //密码
    string    _password;
    
    //数据库名
    string    _database;
    
    //备用服务器名称或地址
    string    _standby_server;
    
    //备用服务器连接端口
    int       _standby_port;
    
    //当前的连接服务器
    bool    _bConnectMaster;
        
    //数据库是否已经打开
    bool     _bopen;
};

/* 模拟ADO接口 */
class CADODatabase
{
public:  
    CADODatabase(const string& server,
      int           port,
      const string& user,
      const string& password,
      const char *  database);
    
    virtual ~CADODatabase();
    
    bool Open();
    void Close();
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
    
    PGconn * GetConnectDB() { return _db; };
    
    bool IsOpen() { return _bopen; };

    char * EscapeString(const string& str)
    {
        char * result = new char[str.size()*2+1];
        
        if ( NULL != result )
        {
            PQescapeString(result, str.c_str(), str.size());
        }
    
        return result;
    }
    
    /* -------------------------------------------------------------------------- */
    
    void FreeString(char * str)
    {
        if ( NULL != str )
        {
            delete[] str;
        }
    }    
protected:
      //PostgreSQL数据库连接句柄
      PGconn *_db;      
      
      //并发控制信号量
      pthread_mutex_t     _mutex;
      
      //服务器名称或地址
      string    _server;
      
      //连接端口
      int       _port;
      
      //用户名
      string    _user;
      
      //密码
      string    _password;
      
      //数据库名
      string    _database;
      
      //数据库是否已经打开
      bool     _bopen;
};  

class CADOCommand;

class CADORecordset
{   
public:      
    CADORecordset(CADODatabase* pAdoDatabase);
    
    virtual ~CADORecordset();
    int GetRecordCount();
    
    int GetFieldCount();
    int GetFieldIndex(const char *szFieldName );
    int GetFieldLen(int nIndex);
    bool GetFieldValue( int nIndex, int &nValue);
    bool GetFieldValue( int nIndex, int64 &nValue);
    bool GetFieldValue( int nIndex, double &dValue);
    char *GetFieldValue( int nIndex);
    bool IsFieldNull(int nIndex);
    
    bool IsEof();
    bool IsEOF();
    void MoveFirst();
    void MoveNext();
    void MovePrevious();
    void MoveLast();
        
    int Execute(CADOCommand* pCommand);
    int ExecuteCursor(CADOCommand* pCommand);
private:
    CADORecordset();
protected:
    //PostgreSQL数据库连接句柄
    CADODatabase     *_pAdoDB;
    PGresult         *_res;
    int               _nTuples;
    bool              _bExecute;
    int               _row;
    int               _nFields;
};

class CADOParameter
{
public:
    enum ParameterType
    {
        paramInt    = 0,
        paramInt64  = 1,
        paramDouble = 2,
        paramText   = 3,
        paramBlob   = 4
    };
    enum ParameterDirection
    {
        paramUnknown     = 0,
        paramInput       = 1,
        paramOutput      = 2,
        paramInputOutput = 3,
        paramReturnValue = 4 
    };

    CADOParameter(char *szName, int nType, int nDirection )
    {
        _type      = nType;
        _direction = nDirection;
        _name      = szName;
        _recordset = NULL;
    }
    CADOParameter(int nType, int nDirection, int value)
    {
        _type = nType;
        _direction = nDirection;
        _recordset = NULL;
        SetValue(value);
    };

    CADOParameter(int nType, int nDirection, int64 value)
    {
        _type = nType;
        _direction = nDirection;
        _recordset = NULL;
        SetValue(value);
    };    

    CADOParameter(int nType, int nDirection, double value)
    {
        _type = nType;
        _direction = nDirection;
        _recordset = NULL;
        SetValue(value);
    };

    CADOParameter(int nType, int nDirection, char * value)
    {
        _type = nType;
        _direction = nDirection;
        _recordset = NULL;
        SetValue(value);
    };
        
    virtual ~CADOParameter() { };

    void SetValue(int value)    { _value = to_string<int>(value,dec);     };
    void SetValue(int64 value)  { _value = to_string<int64>(value,dec);   };
    void SetValue(double value) { _value = to_string<double>(value,dec);  };
    void SetValue(char * value) { _value = value; };
    
    bool GetValue(int &value)     { 
        if ( paramInput != _direction && NULL != _recordset )
            return  _recordset->GetFieldValue(column,value);
        else
            return false;
    };
    
    bool GetValue( int64 &value)  { 
        if ( paramInput != _direction && NULL != _recordset  )
            return  _recordset->GetFieldValue(column,value); 
        else
            return false;
    };
    
    bool GetValue(double &value)  { 
        if ( paramInput != _direction && NULL != _recordset  )
            return  _recordset->GetFieldValue(column,value);
        else
            return false;
    };
    char *GetValue()              { 
        if ( paramInput != _direction && NULL != _recordset  )
            return  _recordset->GetFieldValue(column); 
        else
            return false;
    };
    int GetValueLen()             { 
        if ( paramInput != _direction && NULL != _recordset  )
            return  _recordset->GetFieldLen(column); 
         else
            return -1;
    };
    
    void SetRecordSet(CADORecordset *pRecordSet) {
        _recordset = pRecordSet; 
        column = pRecordSet->GetFieldIndex(_name.c_str());
    };
    
    string GetInputParamValue() { return _value; };
    
    void SetName(string szName)   { _name = szName; };
    string GetName()              { return _name; };
    int GetType()                 { return _type; };
    int GetDirction()             { return _direction; };
private:
    CADOParameter();    
private:
    int    _type;
    long   _size;
    int    _direction;
    string _name;     
    
    string _value;
    
    CADORecordset *_recordset;
    int column;
};

class CADOCommand
{
public:
    enum CommandType
    {
        typeCmdText        = 0,
        typeCmdTable       = 1,
        typeCmdTableDirect = 2,
        typeCmdStoredProc  = 3,
        typeCmdUnknown     = 4,
        typeCmdFile        = 5,
        typeCmdStoredProcCursor  = 6,
    };
    
    CADOCommand(CADODatabase* pAdoDatabase, string strCommandText, int nCommandType = typeCmdStoredProc)
    {
        _pAdoDB = pAdoDatabase;
        _strCommandText = strCommandText;
        _nCommandType = nCommandType;
        
    }
        
    virtual ~CADOCommand(){};

    void SetText(string strCommandText) { _strCommandText = strCommandText; };
    void SetType(int nCommandType)      { _nCommandType = nCommandType;     };
    int GetType()                       { return _nCommandType;             };
        
    bool AddParameter(CADOParameter* pAdoParameter)
    {
        switch (pAdoParameter->GetDirction())
        {
            case CADOParameter::paramInput:
                _paramIn.push_back(pAdoParameter);
                break;
            /* 对于Postgresql来说没有出参，都是返回值 */
            case CADOParameter::paramOutput:
                _paramRet.push_back(pAdoParameter);
            case CADOParameter::paramReturnValue:
                _paramRet.push_back(pAdoParameter);
                break;
            default:
                return false;
                break;
        }
        
        return true;
    };
    
    string GetText()  {return _strCommandText;};
    bool GetCompleteSQL(string &sql);
    void SetRecordSet(CADORecordset *pRecordSet);
    int GetCommandType() { return _nCommandType; };
private:
    CADODatabase     *_pAdoDB;;
    string            _strCommandText;
    int               _nCommandType;
    
    vector<CADOParameter*> _paramRet;    /* 返回单个字段的存储过程 */
    vector<CADOParameter*> _paramIn;     /* 输入参数 */
    
    CADORecordset *_recordet;
};

#endif /*DBS_POSTGRESQLDB_H_*/

