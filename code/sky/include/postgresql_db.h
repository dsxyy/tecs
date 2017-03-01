/*********************************************************************
* ��Ȩ���� (C)2011, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ� postgresql_db.h
* �ļ���ʶ��
* ����ժҪ�� PostgreSqlDB�ඨ���ļ�
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
#ifndef DBS_POSTGRESQLDB_H_
#define DBS_POSTGRESQLDB_H_

#include "libpq-fe.h"
#include "sqldb.h"

using namespace std;

// ���ÿ������캯��
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)
#endif

/**
 @brief PostgreSqlDB��. PostgreSQL���ݿ������
 @li @b ����˵������
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
    @brief ��������:���û�ָ����PostgreSQL���ݿ⡣

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
    virtual int Open();

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
    virtual void SetStandbyServer(const string& server, int port);
    
    /**************************************************************************/
    /**
    @brief ��������:�ر��û�ָ����PostgreSQL���ݿ⡣

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
    virtual void Close();    

    /**************************************************************************/
    /**
    @brief ��������:PostgreSQL���ݿ�ִ��SQL���

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
    int Execute(ostringstream& cmd, Callbackable* obj=0);
    
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
    virtual int ExecuteTransaction(ostringstream& cmd);  
    
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

    @li @b �ӿ����ƣ���Ҫ����free_str�ͷŶ�̬�����SQL�ַ���
    @li @b ����˵������
    */
    /**************************************************************************/    
    char * EscapeString(const string& str);

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

    //�Կ�����
    void Lock()
    {
        pthread_mutex_lock(&_mutex);
    };

    //�Կ����
    void UnLock()
    {
        pthread_mutex_unlock(&_mutex);
    };  
    
    //�������ݿ�
    int CreateDB();
        
    //ִ��DDL���ݶ��������Ϊpostgresql��֧�� if not exists
    //Ŀǰֻ֧��create����
    int ExecDDLcommand(ostringstream& cmd);
    
    //�ж��Ƿ���DDL���ݶ������Ŀǰֻ֧��create����
    bool IsDDLcommand(ostringstream& cmd);
    
    //�жϵ�ǰ�������Ƿ�������
    bool  IsMaster(PGconn *db);
    
    //�жϵ�ǰ�������Ƿ��Ǳ���
    bool IsReadonly(PGresult *res);

    //�������ݿ�
    int ConnectDB( bool isMaster, const char *dbname,PGconn **db);
    
    //���������ݿ������
    int RetryConnectMasterDB();
    
    //���ַ���ȫ��ת��Ϊ��д
    void UpperString( string & str );    
    
    //PostgreSQL���ݿ����Ӿ��
    PGconn *_db;
    
    //���������ź���
    pthread_mutex_t     _mutex;
    
    //���������ƻ��ַ
    string    _server;
    
    //���Ӷ˿�
    int       _port;
    
    //�û���
    string    _user;
    
    //����
    string    _password;
    
    //���ݿ���
    string    _database;
    
    //���÷��������ƻ��ַ
    string    _standby_server;
    
    //���÷��������Ӷ˿�
    int       _standby_port;
    
    //��ǰ�����ӷ�����
    bool    _bConnectMaster;
        
    //���ݿ��Ƿ��Ѿ���
    bool     _bopen;
};

/* ģ��ADO�ӿ� */
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
    //�Կ�����
    void Lock()
    {
        pthread_mutex_lock(&_mutex);
    };

    //�Կ����
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
      //PostgreSQL���ݿ����Ӿ��
      PGconn *_db;      
      
      //���������ź���
      pthread_mutex_t     _mutex;
      
      //���������ƻ��ַ
      string    _server;
      
      //���Ӷ˿�
      int       _port;
      
      //�û���
      string    _user;
      
      //����
      string    _password;
      
      //���ݿ���
      string    _database;
      
      //���ݿ��Ƿ��Ѿ���
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
    //PostgreSQL���ݿ����Ӿ��
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
            /* ����Postgresql��˵û�г��Σ����Ƿ���ֵ */
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
    
    vector<CADOParameter*> _paramRet;    /* ���ص����ֶεĴ洢���� */
    vector<CADOParameter*> _paramIn;     /* ������� */
    
    CADORecordset *_recordet;
};

#endif /*DBS_POSTGRESQLDB_H_*/

