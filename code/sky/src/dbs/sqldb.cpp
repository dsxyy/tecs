/*********************************************************************
* 版权所有 (C)2011, 深圳市中兴通讯股份有限公司。
*
* 文件名称： sqldb.cpp
* 文件标识：
* 内容摘要： SqlDB类实现
* 其它说明：
* 当前版本： 1.0
* 作    者： 张文剑
* 完成日期： 2012-06-19
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
#include "sqldb.h"
#include "shell.h"
#include "misc.h"

static int sqldb_print_on = 0;
DEFINE_DEBUG_VAR(sqldb_print_on);
#define Debug(fmt,arg...) \
        do \
        { \
            if(sqldb_print_on) \
            { \
                printf("[%s:%d]"fmt,__func__,__LINE__, ##arg); \
            } \
        }while(0)
        
static pthread_key_t s_ThreadTransactionKey;
static void FreeTransactionKey(void *pvKey);

class TransactionContext
{
public:  
    TransactionContext(SqlDB *pDbConnection)
    {
        SkyAssert(NULL != pDbConnection);
        pDbCon = pDbConnection;
        begin_count = 0;
    }

    ~TransactionContext() {};
    
    void Begin()
    {
        if(begin_count == 0)
        {
            //第一次begin，清空事务语句列表
            sql.clear();
        }

        //事务支持多次begin，用计数器来记录
        begin_count++;
        Debug("Transaction begin in proc 0x%lx, begin_count = %d\n",(long)getpid(),begin_count);
        return;
    }
    
    void Append(const string& cmd)
    {
        sql.push_back(cmd);
        Debug("Transaction append in proc 0x%lx:\n%s\n",(long)getpid(),cmd.c_str());
    }
    
    int Commit()
    {
        if(begin_count == 0)
        {
            //线程上下文还没有启动过事务
            Debug("Transaction not begin in proc 0x%lx!\n",(long)getpid());
            return 0;
        }
        else if(begin_count > 0)
        {
            //事务在线程上下文被多次重复begin时，每次commit只会抵消一次begin，不做实际动作
            begin_count--;
            if(begin_count > 0)
            {
                //抵消一次之后还是大于0，说明仍然有重复的begin没有对应上
                Debug("Transaction commit ignored in proc 0x%lx, begin_count = %d\n",(long)getpid(),begin_count);
                return 0;
            }
        }

        SkyAssert(begin_count == 0);
        if(sql.empty())
        {
            Debug("No transaction statements in proc 0x%lx!\n",(long)getpid());
            return 0;
        }
        
        ostringstream oss;
        vector<string>::iterator it;
        for(it=sql.begin();it!=sql.end();it++)
        {
            oss << *it << ";" << endl;
        }
        
        int ret = pDbCon->ExecuteTransaction(oss);
        if(ret != SQLDB_OK)
        {
            cerr << "In proc " << hex << (long)getpid() << endl;
            cerr << "Transaction failed!" << endl;
            cerr << oss.str();
        }
        else
        {
            Debug("Transaction commited in proc 0x%lx:\n%s\n",
                (long)getpid(),oss.str().c_str());
        }

        return ret;
    }
      
    void Cancel()
    {
        if(begin_count == 0)
        {
            return;
        }
        else if(begin_count > 0)
        {
            begin_count--;
            if(begin_count == 0)
            {
                sql.clear();
            }
        }
    }
    
    void Print() const
    {
        vector<string>::const_iterator it;
        for(it=sql.begin();it!=sql.end();it++)
        {
            cout << *it << endl;
        }
    }
    
    bool IsOn() const
    {
        if(begin_count > 0)
            return true;
        else
            return false;
    }

    map<string,string>& KeyValue()
    {
        return keyvalue;
    }
    
private:
    DISALLOW_COPY_AND_ASSIGN(TransactionContext);
    TransactionContext() {};
    int begin_count; //事务被多次begin导致嵌套时，用count保存计数
    SqlDB *pDbCon;
    vector<string> sql;
    map<string,string> keyvalue;
};

/**********************************************************************
* 函数名称：Transaction::Enable
* 功能描述：在当前线程上下文启用数据库事务
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期     版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2011/12/01   V1.0    张文剑      创建
************************************************************************/
STATUS Transaction::Enable(SqlDB *pDbConnection)
{
    SkyAssert(NULL != pDbConnection);
    TransactionContext *pDbTrans = static_cast<TransactionContext *>(pthread_getspecific(s_ThreadTransactionKey));
    if(pDbTrans)
    {
       return SUCCESS;  //已创建 则直接返回
    }
    
    pDbTrans = new TransactionContext(pDbConnection);
    if(0 != pthread_setspecific(s_ThreadTransactionKey, (const void *)pDbTrans))
    {
        cerr << "EnableTransaction call pthread_setspecific failed! errno = " << errno << endl;
        SkyAssert(false);
        return ERROR;
    }

    printf("Thread 0x%lx has enabled db transaction!\n",(long)pthread_self());
    return SUCCESS;
}

/**********************************************************************
* 函数名称：Transaction::On
* 功能描述：判断当前线程上下文数据库事务是否进行中
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期     版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2011/12/01   V1.0    张文剑      创建
************************************************************************/
bool Transaction::On()
{
    TransactionContext *pDbTrans = static_cast<TransactionContext *>(pthread_getspecific(s_ThreadTransactionKey));
    if(!pDbTrans)
    {
        return false;
    }
    return pDbTrans->IsOn();
}

/**********************************************************************
* 函数名称：Transaction::Begin
* 功能描述：在当前线程上下文中打开一个新的数据库事务
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期     版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2011/12/01   V1.0    张文剑      创建
************************************************************************/
STATUS Transaction::Begin()
{
    TransactionContext *pDbTrans = static_cast<TransactionContext *>(pthread_getspecific(s_ThreadTransactionKey));
    if(!pDbTrans)
    {
        Debug("proc 0x%lx failed to get transaction context!\n",(long)getpid());
        return ERROR_OBJECT_NOT_EXIST;
    }
    
    pDbTrans->Begin();
    return SUCCESS;
}

/**********************************************************************
* 函数名称：Transaction::Append
* 功能描述：在当前线程上下文中的数据库事务中追加一个数据库操作
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期     版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2011/12/01   V1.0    张文剑      创建
************************************************************************/
STATUS Transaction::Append(const string& cmd)
{
    TransactionContext *pDbTrans = static_cast<TransactionContext *>(pthread_getspecific(s_ThreadTransactionKey));
    if(!pDbTrans)
    {
        Debug("proc 0x%lx failed to get transaction context!\n",(long)getpid());
        return ERROR_OBJECT_NOT_EXIST;
    }
    pDbTrans->Append(cmd);
    return SUCCESS;
}

/**********************************************************************
* 函数名称：Transaction::Commit
* 功能描述：提交当前线程上下文中的数据库事务
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期     版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2011/12/01   V1.0    张文剑      创建
************************************************************************/
STATUS Transaction::Commit()
{
    TransactionContext *pDbTrans = static_cast<TransactionContext *>(pthread_getspecific(s_ThreadTransactionKey));
    if(!pDbTrans)
    {
        Debug("proc 0x%lx failed to get transaction context!\n",(long)getpid());
        return ERROR_OBJECT_NOT_EXIST;
    }
    
    if(0 != pDbTrans->Commit())
    {
        return ERROR;
    }
    return SUCCESS;
}

/**********************************************************************
* 函数名称：Transaction::Cancel
* 功能描述：撤销当前线程上下文中的数据库事务，相当于不做任何实质性动作的Commit
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期     版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2012/06/25   V1.0    张文剑      创建
************************************************************************/
STATUS Transaction::Cancel()
{
    TransactionContext *pDbTrans = static_cast<TransactionContext *>(pthread_getspecific(s_ThreadTransactionKey));
    if(!pDbTrans)
    {
        Debug("proc 0x%lx failed to get transaction context!\n",(long)getpid());
        return ERROR_OBJECT_NOT_EXIST;
    }
    
    pDbTrans->Cancel();
    return SUCCESS;
}

map<string,string>& Transaction::KeyValue()
{
    TransactionContext *pDbTrans = static_cast<TransactionContext *>(pthread_getspecific(s_ThreadTransactionKey));
    if(!pDbTrans)
    {
        SkyAssert(false);
    }

    return pDbTrans->KeyValue();
}

/**********************************************************************
* 函数名称：CreateTransactionKey
* 功能描述：创建数据库事务对象的线程私有数据区key
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期     版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2011/12/01   V1.0    张文剑      创建
************************************************************************/
STATUS CreateTransactionKey()
{
    if(0 != pthread_key_create(&s_ThreadTransactionKey, FreeTransactionKey))
    {
        SkyAssert(false);
        return ERROR;
    }
    return SUCCESS;
}

/**********************************************************************
* 函数名称：FreeTransactionKey
* 功能描述：释放数据库事务对象的线程私有数据区key
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期     版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2011/12/01   V1.0    张文剑      创建
************************************************************************/
void FreeTransactionKey(void *pvKey)
{
    if(!pvKey)
        return;
    TransactionContext *pDbTrans = static_cast<TransactionContext *>(pvKey);
    delete pDbTrans;
    return;
}

/**********************************************************************
* 函数名称：SqlCommand::Add
* 功能描述：向sql命令中增加一个待处理的字段名称，仅用于select
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期     版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2012/06/04   V1.0    张文剑      创建
************************************************************************/
void SqlCommand::Add(const string& field)
{
    DuplicationCheck(field);
    _fields.push_back(field);
    _values.push_back(" NULL ");
}

/**********************************************************************
* 函数名称：SqlCommand::Add
* 功能描述：向sql命令中增加一个待处理的字段名称和字段的值
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 其它说明：重载了多种数据类型，以后如需增加，请再补充更多的Add函数
* 修改日期     版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2012/06/04   V1.0    张文剑      创建
************************************************************************/
void SqlCommand::Add(const string& field,int value)
{
    DuplicationCheck(field);
    _fields.push_back(field);
    _values.push_back(to_string<int>(value,dec));
}

void SqlCommand::Add(const string& field,unsigned int value)
{
    DuplicationCheck(field);
    _fields.push_back(field);
    _values.push_back(to_string<unsigned int>(value,dec));
}

void SqlCommand::Add(const string& field,time_t value)
{
    DuplicationCheck(field);
    _fields.push_back(field);
    _values.push_back(to_string<int>(value,dec));
}

void SqlCommand::Add(const string& field,int64 value)
{
    DuplicationCheck(field);
    _fields.push_back(field);
    _values.push_back(to_string<int64>(value,dec));
}

void SqlCommand::Add(const string& field,double value)
{
    DuplicationCheck(field);
    _fields.push_back(field);
    _values.push_back(to_string<double>(value,dec));
}

void SqlCommand::Add(const string& field,const Serializable& value)
{
    DuplicationCheck(field);
    char *sql_value = _db->EscapeString(value.serialize().c_str());
    if(!sql_value)
    {
        SkyAssert(false);
        return;
    }

    _fields.push_back(field);
    _values.push_back("'" + string(sql_value) + "'");
    _db->FreeString(sql_value);
}

void SqlCommand::Add(const string& field,const string& value)
{
    DuplicationCheck(field);
    char *sql_value = _db->EscapeString(value.c_str());
    if(!sql_value)
    {
        SkyAssert(false);
        return;
    }
    
    _fields.push_back(field);
    _values.push_back("'" + string(sql_value) + "'");
    _db->FreeString(sql_value);
}

void SqlCommand::AddSqlFunc(const string& field,const string& func)
{
    DuplicationCheck(field);
    _fields.push_back(field);
    _values.push_back(func);
}

/**********************************************************************
* 函数名称：SqlCommand::SelectSql
* 功能描述：返回组装好的select语句，用户可以加入自己的where条件
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期     版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2012/06/04   V1.0    张文剑      创建
************************************************************************/
string SqlCommand::SelectSql(const string& where)
{
    if(_fields.empty())
    {
        SkyAssert(false);
        return "";
    }
        
    string command("SELECT " + JoinStrings(_fields,",") + " FROM " + _table);
    if(!where.empty())
    {
        command.append(" " + where);
    }
    return command;
}

/**********************************************************************
* 函数名称：SqlCommand::InsertSql
* 功能描述：返回组装好的insert语句
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期     版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2012/06/04   V1.0    张文剑      创建
************************************************************************/
string SqlCommand::InsertSql()
{
    if(_fields.empty() || _values.empty() || _fields.size() != _values.size())
    {
        SkyAssert(false);
        return "";
    }
    string command("INSERT INTO " + _table + " ");
    command.append(" (" + JoinStrings(_fields,",") + ") ");
    command.append(" VALUES ");
    command.append(" (" + JoinStrings(_values,",") + ") ");
    return command;
}

/**********************************************************************
* 函数名称：SqlCommand::UpdateSql
* 功能描述：返回组装好的update语句，用户可以加入自己的where条件
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期     版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2012/06/04   V1.0    张文剑      创建
************************************************************************/
string SqlCommand::UpdateSql(const string& where)
{
    if(_fields.empty() || _values.empty() || _fields.size() != _values.size())
    {
        SkyAssert(false);
        return "";
    }
        
    string command("UPDATE " + _table + " SET ");
    int total = _fields.size();
    for(int i = 0; i < total; i++)
    {
        command.append(" " + _fields[i] + " = " + _values[i]);
        if(i != total - 1)
        {
            command.append(",");
        }
    }

    if(!where.empty())
    {
        command.append(" " + where);
    }
    return command;
}

/**********************************************************************
* 函数名称：SqlCommand::Insert
* 功能描述：直接执行拼装好的insert语句
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期     版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2012/06/04   V1.0    张文剑      创建
************************************************************************/
int SqlCommand::Insert()
{
    ostringstream oss(InsertSql());

    int ret = -1;
    if(_option == WITH_TRANSACTION && Transaction::On())
    {
        ret = Transaction::Append(oss.str());
    }
    else
    {
        ret = _db->Execute(oss);
    }

    if(0 != ret)
    {
        Debug("SqlCommand::Insert failed in proc 0x%lx! ret = %d\n",(long)getpid(),ret);
    }
    return ret;
}

/**********************************************************************
* 函数名称：SqlCommand::Update
* 功能描述：直接执行拼装好的Update语句
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期     版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2012/06/04   V1.0    张文剑      创建
************************************************************************/
int SqlCommand::Update(const string& where)
{
    ostringstream oss(UpdateSql(where));
    int ret = -1;
    if(_option == WITH_TRANSACTION && Transaction::On())
    {
        ret = Transaction::Append(oss.str());
    }
    else
    {
        ret = _db->Execute(oss);
    }

    if(0 != ret)
    {
        Debug("SqlCommand::Update failed in proc 0x%lx! ret = %d\n",(long)getpid(),ret);
        cerr << oss.str() << endl;
    }
    return ret;
}

/**********************************************************************
* 函数名称：SqlCommand::Delete
* 功能描述：直接执行拼装好的Delete语句
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期     版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2012/06/25   V1.0    张文剑      创建
************************************************************************/
int SqlCommand::Delete(const string& where)
{
    ostringstream oss;
    int ret = -1;
    oss << "DELETE FROM " << _table;
    if(!where.empty())
    {
        oss << " " << where;
    }
        
    if(_option == WITH_TRANSACTION && Transaction::On())
    {
        ret = Transaction::Append(oss.str());
    }
    else
    {
        ret = _db->Execute(oss);
    }

    if(0 != ret && SQLDB_RESULT_EMPTY != ret)
    {
        Debug("SqlCommand::Delete failed in proc 0x%lx! ret = %d\n",(long)getpid(),ret);
        cerr << oss.str() << endl;
    }
    return ret;
}

/**********************************************************************
* 函数名称：SqlCommand::Vacuum
* 功能描述：对表进行清理
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期     版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2013/06/09   V1.0    张文剑      创建
************************************************************************/
int SqlCommand::Vacuum(VacuumOption option)
{
    ostringstream oss;
    int ret = -1;
    oss << "VACUUM ";
    if(option == SqlCommand::VACUUM_FULL)
    {
        oss << "FULL ";
    }
    oss << _table;
    
    ret = _db->Execute(oss);
    if(0 != ret)
    {
        Debug("SqlCommand::Delete Vacuum in proc 0x%lx! ret = %d\n",(long)getpid(),ret);
        cerr << oss.str() << endl;
    }
    return ret;
}

/**********************************************************************
* 函数名称：SqlCommand::Execute
* 功能描述：直接执行拼装好的任意Sql语句
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期     版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2013/03/29   V1.0    张文剑      创建
************************************************************************/
int SqlCommand::Execute(const string& sql)
{
    ostringstream oss(sql);
    int ret = -1;
    if(_option == WITH_TRANSACTION && Transaction::On())
    {
        ret = Transaction::Append(oss.str());
    }
    else
    {
        ret = _db->Execute(oss);
    }

    if(0 != ret && SQLDB_RESULT_EMPTY != ret)
    {
        Debug("SqlCommand::Execute failed in proc 0x%lx! ret = %d\n",(long)getpid(),ret);
        cerr << oss.str() << endl;
    }
    return ret;
}

/**********************************************************************
* 函数名称：SqlCommand::DuplicationCheck
* 功能描述：检查字段是否重复
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期     版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2012/06/27   V1.0    张文剑      创建
************************************************************************/
bool SqlCommand::DuplicationCheck(const string& field)
{
    if(find(_fields.begin(),_fields.end(),field) != _fields.end())
    {
        Debug("Warning! SqlCommand add duplicate field: %s!\n",field.c_str());
        SkyAssert(false);
        return false;
    }
    return true;
}

string SqlCommand::JoinStrings(const vector<string>& strings,const string& separator)
{
    string result;
    vector<string>::const_iterator it;
    for(it = strings.begin(); it != strings.end(); it++)
    {
        result.append(*it);
        if(it != strings.end() - 1)
        {
            result.append(separator);
        }
    }
    return result;
}

