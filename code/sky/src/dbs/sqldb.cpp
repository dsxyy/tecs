/*********************************************************************
* ��Ȩ���� (C)2011, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ� sqldb.cpp
* �ļ���ʶ��
* ����ժҪ�� SqlDB��ʵ��
* ����˵����
* ��ǰ�汾�� 1.0
* ��    �ߣ� ���Ľ�
* ������ڣ� 2012-06-19
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
            //��һ��begin�������������б�
            sql.clear();
        }

        //����֧�ֶ��begin���ü���������¼
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
            //�߳������Ļ�û������������
            Debug("Transaction not begin in proc 0x%lx!\n",(long)getpid());
            return 0;
        }
        else if(begin_count > 0)
        {
            //�������߳������ı�����ظ�beginʱ��ÿ��commitֻ�����һ��begin������ʵ�ʶ���
            begin_count--;
            if(begin_count > 0)
            {
                //����һ��֮���Ǵ���0��˵����Ȼ���ظ���beginû�ж�Ӧ��
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
    int begin_count; //���񱻶��begin����Ƕ��ʱ����count�������
    SqlDB *pDbCon;
    vector<string> sql;
    map<string,string> keyvalue;
};

/**********************************************************************
* �������ƣ�Transaction::Enable
* �����������ڵ�ǰ�߳��������������ݿ�����
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����     �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2011/12/01   V1.0    ���Ľ�      ����
************************************************************************/
STATUS Transaction::Enable(SqlDB *pDbConnection)
{
    SkyAssert(NULL != pDbConnection);
    TransactionContext *pDbTrans = static_cast<TransactionContext *>(pthread_getspecific(s_ThreadTransactionKey));
    if(pDbTrans)
    {
       return SUCCESS;  //�Ѵ��� ��ֱ�ӷ���
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
* �������ƣ�Transaction::On
* �����������жϵ�ǰ�߳����������ݿ������Ƿ������
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����     �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2011/12/01   V1.0    ���Ľ�      ����
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
* �������ƣ�Transaction::Begin
* �����������ڵ�ǰ�߳��������д�һ���µ����ݿ�����
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����     �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2011/12/01   V1.0    ���Ľ�      ����
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
* �������ƣ�Transaction::Append
* �����������ڵ�ǰ�߳��������е����ݿ�������׷��һ�����ݿ����
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����     �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2011/12/01   V1.0    ���Ľ�      ����
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
* �������ƣ�Transaction::Commit
* �����������ύ��ǰ�߳��������е����ݿ�����
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����     �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2011/12/01   V1.0    ���Ľ�      ����
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
* �������ƣ�Transaction::Cancel
* ����������������ǰ�߳��������е����ݿ������൱�ڲ����κ�ʵ���Զ�����Commit
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����     �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2012/06/25   V1.0    ���Ľ�      ����
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
* �������ƣ�CreateTransactionKey
* �����������������ݿ����������߳�˽��������key
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����     �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2011/12/01   V1.0    ���Ľ�      ����
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
* �������ƣ�FreeTransactionKey
* �����������ͷ����ݿ����������߳�˽��������key
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����     �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2011/12/01   V1.0    ���Ľ�      ����
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
* �������ƣ�SqlCommand::Add
* ������������sql����������һ����������ֶ����ƣ�������select
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����     �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2012/06/04   V1.0    ���Ľ�      ����
************************************************************************/
void SqlCommand::Add(const string& field)
{
    DuplicationCheck(field);
    _fields.push_back(field);
    _values.push_back(" NULL ");
}

/**********************************************************************
* �������ƣ�SqlCommand::Add
* ������������sql����������һ����������ֶ����ƺ��ֶε�ֵ
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ����
* ����˵���������˶����������ͣ��Ժ��������ӣ����ٲ�������Add����
* �޸�����     �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2012/06/04   V1.0    ���Ľ�      ����
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
* �������ƣ�SqlCommand::SelectSql
* ����������������װ�õ�select��䣬�û����Լ����Լ���where����
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����     �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2012/06/04   V1.0    ���Ľ�      ����
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
* �������ƣ�SqlCommand::InsertSql
* ����������������װ�õ�insert���
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����     �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2012/06/04   V1.0    ���Ľ�      ����
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
* �������ƣ�SqlCommand::UpdateSql
* ����������������װ�õ�update��䣬�û����Լ����Լ���where����
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����     �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2012/06/04   V1.0    ���Ľ�      ����
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
* �������ƣ�SqlCommand::Insert
* ����������ֱ��ִ��ƴװ�õ�insert���
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����     �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2012/06/04   V1.0    ���Ľ�      ����
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
* �������ƣ�SqlCommand::Update
* ����������ֱ��ִ��ƴװ�õ�Update���
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����     �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2012/06/04   V1.0    ���Ľ�      ����
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
* �������ƣ�SqlCommand::Delete
* ����������ֱ��ִ��ƴװ�õ�Delete���
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����     �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2012/06/25   V1.0    ���Ľ�      ����
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
* �������ƣ�SqlCommand::Vacuum
* �����������Ա��������
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����     �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2013/06/09   V1.0    ���Ľ�      ����
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
* �������ƣ�SqlCommand::Execute
* ����������ֱ��ִ��ƴװ�õ�����Sql���
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����     �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2013/03/29   V1.0    ���Ľ�      ����
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
* �������ƣ�SqlCommand::DuplicationCheck
* ��������������ֶ��Ƿ��ظ�
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����     �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2012/06/27   V1.0    ���Ľ�      ����
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

