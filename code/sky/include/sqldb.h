/*********************************************************************
* ��Ȩ���� (C)2011, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ� sqldb.h
* �ļ���ʶ��
* ����ժҪ�� SqlDB�ඨ���ļ�
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
#ifndef DBS_SQL_DB_H_
#define DBS_SQL_DB_H_
#include <sstream>
#include "callbackable.h"

using namespace std;

// ���ÿ������캯��
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
 @brief SqlDB�ࡣ���ݿ���ʳ�����࣬��ͬ�����ݿ����ʵ���඼��������������磺sqlite postgresql��
 @li @b ����˵������  stdint.h 
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
    @brief ��������:���û�ָ�������ݿ⡣

    @li @b ����б�
    @verbatim
       ��
    @endverbatim

    @li @b �����б�
    @verbatim
       ��
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
       0       �ɹ�
       ����    ʧ��
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    /**************************************************************************/
    virtual int Open() = 0;

    /**************************************************************************/
    /**
    @brief ��������:�������ݿⱸ�÷�������ַ�Ͷ˿ڡ�

    @li @b ����б�
    @verbatim
       server ���ݿⱸ�÷�������ַ
       port   ���ݿⱸ�÷������˿�
    @endverbatim

    @li @b �����б�
    @verbatim
       ��
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
       ��
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    /**************************************************************************/    
    virtual void SetStandbyServer(const string& server, int port) = 0;
    
    /**************************************************************************/
    /**
    @brief ��������:�ر��û�ָ�����ݿ⡣

    @li @b ����б�
    @verbatim
       ��
    @endverbatim

    @li @b �����б�
    @verbatim
       ��
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
      ��
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    /**************************************************************************/
    virtual void Close() = 0;    

    /**************************************************************************/
    /**
    @brief ��������:ִ��SQL���

    @li @b ����б�
    @verbatim
       cmd      SQL����
       obj      �ص����� ��ѯʱʹ�ã�������е�ÿһ�����ݶ������
    @endverbatim

    @li @b �����б�
    @verbatim
       ��
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
       0       �ɹ�
       ����    ʧ��
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    /**************************************************************************/     
    virtual int Execute(ostringstream& cmd, Callbackable* obj=0) = 0;

    /**************************************************************************/
    /**
    @brief ��������:������ķ�ʽִ�ж���SQL���

    @li @b ����б�
    @verbatim
       cmd      SQL����,�����Ƕ���,�����в�ѯ���
    @endverbatim

    @li @b �����б�
    @verbatim
       ��
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
       0       �ɹ�
       ����    ʧ��
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    /**************************************************************************/     
    virtual int ExecuteTransaction(ostringstream& cmd) = 0;    

    /**************************************************************************/
    /**
    @brief ��������:��Ӧ�ô�����ַ���ת��Ϊ�Ϸ���SQL����ַ���

    @li @b ����б�
    @verbatim
       str      Ӧ�ô�����ַ���
    @endverbatim

    @li @b �����б�
    @verbatim
       ��
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
       ��NULL       ��Ч��SQL�ַ����������׵�ַ
       NULL         ʧ��
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    /**************************************************************************/       
    virtual char * EscapeString(const string& str) = 0;

    /**************************************************************************/
    /**
    @brief ��������:�ͷŶ�̬������ַ���

    @li @b ����б�
    @verbatim
       str      ��̬������ַ������׵�ַ
    @endverbatim

    @li @b �����б�
    @verbatim
       ��
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
       ��
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
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
        WITH_TRANSACTION = 1, //��ǰ����������Ѿ������������Զ�ʹ������ģʽ
        WITHOUT_TRANSACTION = 2, //����鵱ǰ�������Ƿ�ʹ������ֱ��ִ��sql����
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


