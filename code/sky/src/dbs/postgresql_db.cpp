/*********************************************************************
* ��Ȩ���� (C)2011, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ� postgresql_db.cpp
* �ļ���ʶ��
* ����ժҪ�� PostgreSqlDB�ඨ��ʵ���ļ�
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
#include <iostream>
#include <stdexcept>
#include <string.h>
#include <stdlib.h>
#include "pg_type.h"

#include "postgresql_db.h"
#include "postgresql_column.h"

using namespace std;


/* ************************************************************************** */
/* PostgreSqlDB constructor/destructor                                             */
/* ************************************************************************** */

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
PostgreSqlDB::PostgreSqlDB(
        const string& server,
        int           port,
        const string& user,
        const string& password,
        const string& database)
{
    //�������ݿ���Ϣ
    _server = server;
    _port   = port;
    _user   = user;
    _password = password;
    _database = database;

    if ( _server.empty() ) 
    {
        _server = "localhost";
    }
    
    //��ʼ�����ݿ���
    _db = NULL;
    _bopen = false;
    
    //��ʼ���ź���
    pthread_mutex_init(&_mutex,0);
}

/* -------------------------------------------------------------------------- */

PostgreSqlDB::~PostgreSqlDB()
{
    PQfinish(_db);

    pthread_mutex_destroy(&_mutex);
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

void PostgreSqlDB::Close()
{
    Lock();
    if ( _bopen ) 
    {
        PQfinish(_db);
        _db = NULL;
        _bopen = false;
    }
    UnLock();
}

/* -------------------------------------------------------------------------- */
int PostgreSqlDB::CreateDB()
{
    PGconn         *template1_db;
    PGresult       *res;
    ExecStatusType  status;
    string          sql;
  
    if ( 0 == ConnectDB(true, "template1",&template1_db) && IsMaster(template1_db) )
    {
        goto createdb;
    }
    //�ر���������
    PQfinish(template1_db);
    template1_db = NULL;
        
    /* ���������Ӷ�ʧ�� */
    if ( 0 != ConnectDB(false, "template1",&template1_db) || !IsMaster(template1_db) )
    {
        PQfinish(template1_db);
        template1_db = NULL;
        return -1;
    }
createdb:	          
    sql = "CREATE DATABASE \"";
    sql += _database;
    sql += "\"";
    res = PQexec(template1_db, sql.c_str());  /* PQexec���ؽ�����Ƕ����Ƶ� */
    
    status = PQresultStatus(res);
    
    if ( status != PGRES_COMMAND_OK )
    {
        std::string::size_type idx;
        string          errmsg;
        
        errmsg = PQresultErrorMessage(res);
        idx = errmsg.find( "already exists" );
        
        //�����Ǳ���
        if ( idx == std::string::npos ) 
        {
            idx = errmsg.find("read-only transaction");
        }
        
        //���ܷ�������
        if ( idx == std::string::npos )
        {
            idx = errmsg.find( "�Ѿ�����" );
        }
        //���ݿⴴ��ʧ�ܵ�ԭ�����Ѿ�����
        if (idx == std::string::npos)
        {
            /* cout << "error:" << (int)status << " msg:" << PQresultErrorMessage(res) << endl; */
            //������ʧ��
            PQclear(res);
            PQfinish(template1_db);
            return -1;
        }
    }        
    
    //���ݿⴴ���ɹ������Ѿ�����
    PQclear(res);
    PQfinish(template1_db);    
    
    return 0;
}

/* -------------------------------------------------------------------------- */
int PostgreSqlDB::Open()
{
    int    rc;
    
    Lock();
    
    if ( _bopen )
    {
        UnLock();
        return 0;
    }
    
    if ( CreateDB() != 0 )
    {
        UnLock();
        return -1;
    }
    
    rc = RetryConnectMasterDB();
    
    UnLock();
    
    return rc;
}

/* -------------------------------------------------------------------------- */

int PostgreSqlDB::Execute(ostringstream& cmd, Callbackable* obj)
{
    const char * c_str;
    string       str;
    PGresult   *res;
    ExecStatusType  status;

    str   = cmd.str();
    c_str = str.c_str();
    
    if ( IsDDLcommand(cmd) )
    {
        return ExecDDLcommand(cmd);
    }
    
    Lock();

    if ( !_bopen && 0 != RetryConnectMasterDB())
    {
        UnLock();
        return -1;
    }
    /* res = PQexec(_db, c_str); */ /* PQexec���ؽ�����Ƕ����Ƶ� */
    res = PQexecParams(_db, c_str, 0, NULL,	NULL, NULL,	NULL, 0); /* ���һ������ 0--�����ַ��������, 1--���ض����ƽ���� */    

    status = PQresultStatus(res);
    
    /* TODO���ִ��ʧ�ܣ���Ҫ�ж����ݿ������Ƿ����� */
    if ( PGRES_FATAL_ERROR == status )
    {
        if( (IsReadonly(res) || PQstatus(_db) != CONNECTION_OK) && 
            ( 0 == RetryConnectMasterDB() ) 
          )
        {
            PQclear(res);

            res = PQexecParams(_db, c_str, 0, NULL,	NULL, NULL,	NULL, 0);    /* ���һ������ 0--�����ַ��������, 1--���ض����ƽ���� */    

            status = PQresultStatus(res);
        }
    }
    // �жϱ��Ƿ���ڿ���ʹ��SELECT datname FROM pg_database;
    
    /*
     * PGRES_COMMAND_OK �ɹ����һ�����������ݵ�����
     * PGRES_TUPLES_OK  �ɹ�ִ��һ���������ݵĲ�ѯ��ѯ������ SELECT ���� SHOW����
     */
    if ( status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK )
    {
        ostringstream   oss;
        const char *    err_msg = PQresultErrorMessage(res);

        oss << "SQL command was: " << c_str;
        oss << ", error " << status << " : " << err_msg;

        /*NebulaLog::log("ONE",Log::ERROR,oss);*/
        /* cout << "error:" <<(int)PQstatus(_db) << oss.str().c_str() << endl; */
        
        PQclear(res);

        UnLock();

        return -1;
    }

    /* ��ѯ */ 
    if ( status == PGRES_TUPLES_OK && (obj != 0) && (obj->IsCallBackSet()) )
    {
        unsigned int        num_fields;
        PostgreSQLColumn    columns;  
        int tuples;      

        num_fields  = PQnfields(res);

        // Fetch each row, and call-back the object waiting for them
        tuples = PQntuples(res);
        for ( int i = 0; i < tuples; i++)
        {
            columns.SetPGresult(_db, res, i, num_fields);
            obj->DoCallback(&columns);
        }
        //û�в�ѯ����¼
        if( 0 == tuples )
        {
            PQclear(res);
            UnLock();
            return SQLDB_RESULT_EMPTY;
        }
    }
    
    // Free the result object
    PQclear(res);
    
    UnLock();

    return 0;
}

/* -------------------------------------------------------------------------- */

int PostgreSqlDB::ExecuteTransaction(ostringstream& cmd)
{
    const char * c_str;
    string       str;
    PGresult   *res;
    ExecStatusType  status;

    str   = cmd.str();
    c_str = str.c_str();

    Lock();

    if ( !_bopen && 0 != RetryConnectMasterDB())
    {
        UnLock();
        return -1;
    }
    
    /* PQexecParams��֧�ֶ������ */
    /* ���ȿ������� */
    res = PQexec(_db, "BEGIN;");
    
    status = PQresultStatus(res);
    
    /* TODO���ִ��ʧ�ܣ���Ҫ�ж����ݿ������Ƿ����� */
    if ( PGRES_FATAL_ERROR == status )
    {
        if( ( IsReadonly(res) || PQstatus(_db) != CONNECTION_OK ) && 
            ( 0 == RetryConnectMasterDB() )
          )
        {
            PQclear(res);

            res = PQexec(_db, "BEGIN;");

            status = PQresultStatus(res);
        }
    }
    /*
     * PGRES_COMMAND_OK �ɹ����һ�����������ݵ�����
     * PGRES_TUPLES_OK  �ɹ�ִ��һ���������ݵĲ�ѯ��ѯ������ SELECT ���� SHOW����
     */
    /* ÿ��ʹ����res��,����Ҫ�ͷ�res,��������ڴ�й¶,�˴����begin�Ľ���� */
    PQclear(res);     
    if ( status != PGRES_COMMAND_OK )
    {
        UnLock();
        
        return SQLDB_BEGIN_FAILED;
    }
    
    /* ִ��SQL��� */
    res = PQexec(_db, c_str); /* PQexec���ؽ�����Ƕ����Ƶ� */
    
    status = PQresultStatus(res);
    /* ÿ��ʹ����res��,����Ҫ�ͷ�res,��������ڴ�й¶,�˴����SQL���Ľ���� */
    PQclear(res);    
    if ( status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK )
    {        
        res = PQexec(_db, "ROLLBACK;");
        
        PQclear(res);

        UnLock();

        return SQLDB_EXEC_SQL_FAILED;
    }

    res = PQexec(_db, "COMMIT;");
    
    status = PQresultStatus(res);
    
    // Free the result object
    PQclear(res);
    
    UnLock();

    return ( PGRES_COMMAND_OK == status ) ? 0 : SQLDB_COMMIT_FAILED;
}

void PostgreSqlDB::UpperString( string & str )
{
    std::string::size_type pos = 0;
    
    while ( pos < str.length() )
    {
        str[pos] = toupper(str[pos]);
        pos++;
    }
}

bool PostgreSqlDB::IsDDLcommand(ostringstream& cmd)
{
    const char * c_str;
    string       str;

    str = cmd.str();
    c_str = str.c_str();

    if ( strncasecmp(c_str, "create", 6) == 0 )
    {
        return true;
    }
    
    return false;
}

/* -------------------------------------------------------------------------- */
int PostgreSqlDB::ExecDDLcommand(ostringstream& cmd)
{
    string sql = cmd.str();
    string sql_exec = cmd.str();
    
    UpperString(sql);
    
    std::string::size_type pos_if;
    std::string::size_type pos_not;
    std::string::size_type pos_exists;
    std::string::size_type pos_space;
    
    pos_if = sql.find("IF", 0);
    pos_not = sql.find("NOT", pos_if);
    pos_exists = sql.find("EXISTS", pos_not);
    pos_space = sql.find(" ", pos_exists);
    
    /* IF NOT EXISTS �ؼ����Ƿ�������������Ҫɾ�� */
    if ( string::npos != pos_if &&
         string::npos != pos_not &&
         string::npos != pos_exists &&
         string::npos != pos_space
       )
    {
       /*  cout << sql_exec.c_str() << endl; */
        sql_exec.erase(pos_if, pos_space - pos_if);
        /* cout << sql_exec.c_str() << endl; */
       
    }

    const char * c_str;
    PGresult   *res;
    ExecStatusType  status;
    
    c_str = sql_exec.c_str();
    
    Lock();

    if ( !_bopen && 0 != RetryConnectMasterDB())
    {
        UnLock();
        return -1;
    }
    
    res = PQexecParams(_db, c_str, 0, NULL,	NULL, NULL,	NULL, 0);    /* ���һ������ 0--�����ַ��������, 1--���ض����ƽ���� */    

    status = PQresultStatus(res);
    
    /* TODO���ִ��ʧ�ܣ���Ҫ�ж����ݿ������Ƿ����� */
    if ( PGRES_FATAL_ERROR == status )
    {
        if( ( IsReadonly(res) || PQstatus(_db) != CONNECTION_OK) && 
            ( 0 == RetryConnectMasterDB() ) 
          )
        {   
            PQclear(res);

            res = PQexecParams(_db, c_str, 0, NULL,	NULL, NULL,	NULL, 0); 

            status = PQresultStatus(res);
        }
    }
    
    //PGRES_COMMAND_OK �ɹ����һ�����������ݵ�����
    if ( status != PGRES_COMMAND_OK )
    {        
        std::string::size_type idx;
        string          errmsg;
        
        errmsg = PQresultErrorMessage(res);
        idx = errmsg.find( "already exists" );
        /* cout << "error:" << (int)status << " msg:" << errmsg.c_str() << endl; */
        //���ݿⴴ��ʧ�ܵ�ԭ�����Ѿ�����
        if (idx == std::string::npos)
        {
            //�����Ķ���ʧ��
            PQclear(res);
            
            UnLock();
            
            return -1;
        }
    }
    
    // Free the result object
    PQclear(res);
    
    UnLock();

    return 0;
}

/* -------------------------------------------------------------------------- */

char * PostgreSqlDB::EscapeString(const string& str)
{
    char * result = new char[str.size()*2+1];
    
    if ( NULL != result )
    {
        PQescapeString(result, str.c_str(), str.size());
    }

    return result;
}

/* -------------------------------------------------------------------------- */

void PostgreSqlDB::FreeString(char * str)
{
    if ( NULL != str )
    {
        delete[] str;
    }
}


/* -------------------------------------------------------------------------- */

bool  PostgreSqlDB::IsMaster(PGconn *db)
{
    PGresult   *res;
    ExecStatusType  status;
    
    //���������޸�����,���������޸�����.
    res = PQexecParams(db, "create or replace view check_db_master_view as select 1;", 0, NULL,	NULL, NULL,	NULL, 0);
    
    status = PQresultStatus(res);
    
    PQclear(res);
    
    //ֻ��ִ�н���ɹ�����Ϊ������
    return (status == PGRES_COMMAND_OK) ? true : false;
}

/* -------------------------------------------------------------------------- */
void PostgreSqlDB::SetStandbyServer(const string& server, int port)
{
    _standby_server =  server;
    if ( _standby_server.empty() )
        _standby_server = "localhost";
    _standby_port     = port;
}        

/* -------------------------------------------------------------------------- */
int PostgreSqlDB::RetryConnectMasterDB()
{   
    if ( _bopen )
    {
        /* ������� */
        PQfinish(_db);
        _db = NULL;
        _bopen = false;
    }
    
    /* ������������ */
    if ( 0 == ConnectDB(true, _database.c_str(),&_db) && IsMaster(_db) )
    {
        _bopen          = true;
        _bConnectMaster = true;
        return 0;
    }
    /* ������� */
    PQfinish(_db);
    _db = NULL;    

    /* ���Ա��� */
    if ( 0 == ConnectDB(false, _database.c_str(), &_db) && IsMaster(_db) )
    {
        _bopen          = true;
        _bConnectMaster = false;
        return 0;            
    }
    
    PQfinish(_db);
    _db = NULL;
    
    return -1;    
}

/* -------------------------------------------------------------------------- */
int PostgreSqlDB::ConnectDB( bool isMaster, const char *dbname,PGconn **db)
{
    ostringstream   oss;
    string          connect_info;
    const char     *c_str;
    ConnStatusType  connect_status;
        
    if ( isMaster )
    {
        oss << "host = ";
        oss << _server;

        if ( _port > 0 ) {
           oss << " port = " << _port;
        }
        if ( !_user.empty() ) {
            oss << " user = " << _user;
        }
        if ( !_password.empty()) {
            oss <<  " password = " << _password; 
        }
        oss <<  " dbname = " << dbname;
    }
    else if(  !_standby_server.empty() )
    {
        oss << "host = ";
        oss << _standby_server;
        
        if ( _standby_port > 0 ) {
           oss << " port = " << _standby_port;
        }        
        if ( !_user.empty() ) {
            oss << " user = " << _user;
        }
        
        if ( !_password.empty()) {
            oss <<  " password = " << _password; 
        }
        oss <<  " dbname = " << dbname;
    }
    else
    {
        *db = NULL;
        return -1;
    }
    
    connect_info   = oss.str();
    c_str          = connect_info.c_str();
    *db            = PQconnectdb(c_str);
    connect_status = PQstatus(*db);

    if ( connect_status != CONNECTION_OK )
    {
        PQfinish(*db);
        *db = NULL;
        return -1;
    }
	  
    return 0;
}

/* -------------------------------------------------------------------------- */
bool PostgreSqlDB::IsReadonly(PGresult *res)
{
    std::string::size_type idx;
    string          errmsg;
    
    errmsg = PQresultErrorMessage(res);
    
    idx = errmsg.find( "read-only transaction" );
    
    if (idx == std::string::npos)
    {
        return false;
    }
    
    return true;
}


/* ģ��ADO�ӿ� */
CADODatabase::CADODatabase(
        const string& server,
        int           port,
        const string& user,
        const string& password,
        const char *  database)
{
    //�������ݿ���Ϣ
    _server = server;
    _port   = port;
    _user   = user;
    _password = password;
    _database = database;
    
    //��ʼ�����ݿ���
    _db = NULL;
    _bopen = false;
    
    //��ʼ���ź���
    pthread_mutex_init(&_mutex,0);
}

/* -------------------------------------------------------------------------- */

CADODatabase::~CADODatabase()
{
    PQfinish(_db);

    pthread_mutex_destroy(&_mutex);
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

void CADODatabase::Close()
{
    if ( _bopen ) 
    {
        PQfinish(_db);
        _db = NULL;
        _bopen = false;
    }
}

/* -------------------------------------------------------------------------- */
bool CADODatabase::Open()
{
    const char     *c_str;
    ConnStatusType  connect_status;
    ostringstream   oss;
    string          connect_info;
    
    
    if ( _bopen )
    {
        return true;
    }
    
    oss << "host = ";
    if ( !_server.empty() ) {
        oss << _server;
    }
    else {
        oss << "localhost";
    }
    
    if ( _port > 0 ) {
       oss << " port = " << _port;
    }
    
    if ( !_user.empty() ) {
        oss << " user = " << _user;
    }
    
    if ( !_password.empty()) {
        oss <<  " password = " << _password;
    }
    
    oss <<  " dbname = " << _database;
    connect_info = oss.str();
    
    c_str = connect_info.c_str();
    
    _db = PQconnectdb(c_str);
    
    connect_status = PQstatus(_db);
    if ( connect_status != CONNECTION_OK)
	{
        PQfinish(_db);
        _db = NULL;
        
        return  false;
    }
    
    _bopen = true;
        
    return true;
}



CADORecordset::CADORecordset(CADODatabase* pAdoDatabase)
{
    _pAdoDB   = pAdoDatabase;
    _res      = NULL;
    _nTuples  = -1;
    _nFields  = -1;
    _row      = -1;
    _bExecute = false;
}

CADORecordset::~CADORecordset()
{
    if ( NULL != _res )
    {
        PQclear(_res);
        _res = NULL;
    }
}

int CADORecordset::GetRecordCount()
{
    return _nTuples;
}

int CADORecordset::GetFieldCount()
{
    return _nFields;
}

int CADORecordset::GetFieldIndex(const char *szFieldName )
{
    int nLoop;
    char *szFieldBuf;
    
    for ( nLoop = 0;  nLoop < _nFields; nLoop++)
    {
        szFieldBuf =  PQfname(_res, nLoop );
        if (szFieldBuf != NULL && strcmp(szFieldBuf,szFieldName) == 0 )
        {
            return nLoop;
        }
    }
    
    return -1;
}

bool CADORecordset::GetFieldValue( int nIndex, int &nValue)
{
    Oid type;
    char *vptr;
    
    int column = nIndex;

    if ( IsEof() )
    {
        nValue = 0;
        return false;
    }
    /* ����ֶ�ֵΪNULL */
    if ( PQgetisnull(_res, _row, column) )
    {
        nValue = 0;
        return true;
    }
    if ( (vptr = PQgetvalue(_res, _row, column) ) == NULL )
    {
        nValue = 0;
        return true;
    }
    
    type = PQftype(_res, column );
    
	  switch (type)
	  {
	  	case BOOLOID:
              if( 't' == vptr[0]  ||  'T' == vptr[0] )
                  nValue = 1;
              else
                  nValue = 0;;              
              break;
	  	case INT2OID:
	  	case INT4OID:
              nValue = strtol(vptr, NULL, 10);              
              break;
	  	case INT8OID:
              nValue = strtoll(vptr, NULL, 10);
              break;
	  	case FLOAT4OID:
	  	case FLOAT8OID:
	  	case NUMERICOID:     
              nValue = (int)strtold(vptr, NULL);        
              break;
	  	case TEXTOID:            
	  	case BPCHAROID:
          case VARCHAROID:
              nValue = (int)strtoll(vptr, NULL, 10);
              break;      
          /* ������������ͺͶ��������Ͳ���ֱ��ת��Ϊ���� */            
	  	case DATEOID:
	  	case TIMEOID:
	  	case TIMESTAMPOID:
      case BYTEAOID:
      case INTERVALOID:
	  	default:
              nValue = 0;
	  		break;
	  }    
    return true;    
}

bool CADORecordset::GetFieldValue( int nIndex, int64 &nValue)
{
    Oid type;
    char *vptr;
    
    int column = nIndex;
    
    if ( IsEof() )
    {
        nValue = 0;
        return false;
    }
        
    /* ����ֶ�ֵΪNULL */
    if ( PQgetisnull(_res, _row, column) )
    {
        nValue = 0;
        return true;
    }
    if ( (vptr = PQgetvalue(_res, _row, column) ) == NULL )
    {
        nValue = 0;
        return true;
    }
    
    type = PQftype(_res, column );
    
	  switch (type)
	  {
	  	case BOOLOID:
              if( 't' == vptr[0]  ||  'T' == vptr[0] )
                  nValue =  1;
              else
              nValue =  0;
              break;
	  	case INT2OID:
	  	case INT4OID:
              nValue =  strtol(vptr, NULL, 10);
              break;
	  	case INT8OID:
              nValue =  strtoll(vptr, NULL, 10);
              break;
	  	case FLOAT4OID:
	  	case FLOAT8OID:
	  	case NUMERICOID:     
              nValue =  (long long)strtold(vptr, NULL);        
              break;
	  	case TEXTOID:            
	  	case BPCHAROID:
          case VARCHAROID:
              nValue =  strtoll(vptr, NULL, 10);
              break;      
          /* ������������ͺͶ��������Ͳ���ֱ��ת��Ϊ���� */            
	  	case DATEOID:
	  	case TIMEOID:
	  	case TIMESTAMPOID:
      case BYTEAOID:
      case INTERVALOID:
	  	default:
              nValue =  0;
	  		break;
	  }    
    return true;    
}
bool CADORecordset::GetFieldValue( int nIndex, double &dValue)
{
    Oid type;
    char *vptr;
    
    int column = nIndex;

    if ( IsEof() )
    {
        dValue = 0.0;
        return false;
    }
        
    /* ����ֶ�ֵΪNULL */
    if ( PQgetisnull(_res, _row, column) )
    {
        dValue = 0.0;
        return true;
    }
    if ( (vptr = PQgetvalue(_res, _row, column) ) == NULL )
    {
        dValue = 0.0;
        return true;
    }
    
    type = PQftype(_res, column );
    
	switch (type)
	{
		case BOOLOID:
            if( 't' == vptr[0]  ||  'T' == vptr[0] )
                dValue =  1.0;
            else
            dValue =  0.0;
            break;
		case INT2OID:
		case INT4OID:
            dValue =  (double)strtol(vptr, NULL, 10);
            break;
		case INT8OID:
            dValue =  (double)strtoll(vptr, NULL, 10);
            break;
		case FLOAT4OID:
		case FLOAT8OID:
		case NUMERICOID:     
            dValue =  strtold(vptr, NULL);        
            break;
		case TEXTOID:            
		case BPCHAROID:
        case VARCHAROID:
            dValue =  strtold(vptr, NULL);
            break;      
        /* ������������ͺͶ��������Ͳ���ֱ��ת��Ϊ���� */            
		case DATEOID:
		case TIMEOID:
		case TIMESTAMPOID:
        case BYTEAOID:
        case INTERVALOID:
		default:
            dValue =  0.0;
			break;
	}    
    return true;    
}

char * CADORecordset::GetFieldValue( int nIndex)
{
    Oid type;
    char *vptr;
    
    int column = nIndex;

    if ( IsEof() )
    { 
        return (char*)"";
    }
        
    /* ����ֶ�ֵΪNULL */
    if ( PQgetisnull(_res, _row, column) )
    {
        return (char*)"";
    }
    if ( (vptr = PQgetvalue(_res, _row, column) ) == NULL )
    {
        return (char*)"";
    }
    
    type = PQftype(_res, column );
    
	  switch (type)
	  {
	  	case BOOLOID:
              if( 't' == vptr[0]  ||  'T' == vptr[0] )
                  return (char*)"true";
              return (char*)"false";
              break;
	  	default:
              return vptr;
	  		break;
	  }    
    return (char*)"";    
}

int CADORecordset::GetFieldLen(int nIndex)
{
    if ( IsEof() )
    {
        return 0;
    }
    return PQgetlength(_res,_row,nIndex);
}

bool CADORecordset::IsFieldNull(int nIndex)
{
    if ( IsEof() )
    {
        return true;
    }
    return PQgetisnull(_res, _row, nIndex);
}

bool CADORecordset::IsEof()
{
    return (_row < 0 || _row >= _nTuples);
}
bool CADORecordset::IsEOF()
{
    return (_row < 0 || _row >= _nTuples);
}    
void CADORecordset::MoveFirst()
{
    if ( _nTuples > 0 )
    _row = 0;
}    
void CADORecordset::MoveNext()
{
    if ( _nTuples > 0 && _row < _nTuples)
    _row++;
}    
void CADORecordset::MovePrevious()
{
    if ( _nTuples > 0 && _row >= 0 )
    _row--;
}
void CADORecordset::MoveLast()
{
    if ( _nTuples > 0 )
        _row =_nTuples - 1;
}

int CADORecordset::ExecuteCursor(CADOCommand* pCommand)
{
    string sql;
    string fetch_sql;
    const char * c_str;
    ExecStatusType  status;
    char *cursor_name;
    PGresult         *begin_res = NULL;
    PGresult         *fetch_res = NULL;
    PGresult         *commit_res = NULL;
    bool              bBegin     = false;
    bool              bNull      = false;
    
    if ( !pCommand->GetCompleteSQL(sql) )
    {
        return SQLDB_ERROR;
    }
    /* �α�����Ҫ�������� */
    begin_res = PQexecParams(_pAdoDB->GetConnectDB(), "BEGIN;", 0, NULL,	NULL, NULL,	NULL, 0);     
    status = PQresultStatus(begin_res);
    /* ���ִ��ʧ�ܣ���Ҫ�ж����ݿ������Ƿ����� */
    if ( PGRES_FATAL_ERROR == status )
    {
        if( PQstatus(_pAdoDB->GetConnectDB()) != CONNECTION_OK )
        {
           //�����������ݿ⣬��ִ��һ��
            PQreset(_pAdoDB->GetConnectDB());
            PQclear(begin_res);

            begin_res = PQexecParams(_pAdoDB->GetConnectDB(), "BEGIN;", 0, NULL,	NULL, NULL,	NULL, 0);
            status = PQresultStatus(begin_res);
        }
    }
        
    if ( status != PGRES_COMMAND_OK )
    {        
        goto err;
    }  

    bBegin = true;

    c_str = sql.c_str();
    fetch_res = PQexecParams(_pAdoDB->GetConnectDB(), c_str, 0, NULL,	NULL, NULL,	NULL, 0); /* ���һ������ 0--�����ַ��������, 1--���ض����ƽ���� */    
    
    status = PQresultStatus(fetch_res);
    if ( status != PGRES_TUPLES_OK )
    {        
        goto err;
    }  

    /* ����ֶ�ֵΪNULL */
    if ( PQgetisnull(fetch_res, 0, 0) )
    {
        bNull = true;
        goto err;
    }
    cursor_name = PQgetvalue(fetch_res, 0, 0);
    if ( (NULL == cursor_name) || (strlen(cursor_name) <= 0) )
    {
        bNull = true;
        goto err;
    }    
    
    fetch_sql.append("FETCH ALL IN ");
    fetch_sql.append("\"");
    fetch_sql.append(cursor_name);
    fetch_sql.append("\"");
    c_str = fetch_sql.c_str();
    _res = PQexecParams(_pAdoDB->GetConnectDB(), c_str, 0, NULL,	NULL, NULL,	NULL, 0); /* ���һ������ 0--�����ַ��������, 1--���ض����ƽ���� */    
    status = PQresultStatus(_res);
    if ( status != PGRES_TUPLES_OK )
    {        
        goto err;
    }      
    
    _nFields = PQnfields(_res);
    _nTuples = PQntuples(_res);            
    if ( _nTuples > 0 )
    {
        _row = 0;
    }  
    
    pCommand->SetRecordSet(this);
    _bExecute = true;
    
    commit_res = PQexecParams(_pAdoDB->GetConnectDB(), "COMMIT;", 0, NULL,	NULL, NULL,	NULL, 0);  

    if ( NULL != begin_res )
        PQclear(begin_res);
    if ( NULL != fetch_res )
        PQclear(fetch_res);
    if ( NULL != commit_res )
        PQclear(commit_res);      
    return SQLDB_OK;  
err:
    /* �������������Ҫ�ύ */
    if ( bBegin )
        commit_res = PQexecParams(_pAdoDB->GetConnectDB(), "COMMIT;", 0, NULL,	NULL, NULL,	NULL, 0);
    
    if ( NULL != begin_res )
        PQclear(begin_res);
    if ( NULL != fetch_res )
        PQclear(fetch_res);
    if ( NULL != commit_res )
        PQclear(commit_res);    
    if ( NULL != _res)
    {
        PQclear(_res);            
        _res = NULL;
    }
        
    if ( bNull )
        return SQLDB_RESULT_EMPTY;
    else            
        return SQLDB_ERROR;  
}

int CADORecordset::Execute(CADOCommand* pCommand)
{
    string sql;
    const char * c_str;
    ExecStatusType  status;
    
    /* ���ݿ�û������ */
    if ( !_pAdoDB->IsOpen() )
    {
        return SQLDB_ERROR;
    }

    if ( pCommand->GetCommandType() == CADOCommand::typeCmdStoredProcCursor )
    {
        return ExecuteCursor(pCommand);
    }    
    
    if ( !pCommand->GetCompleteSQL(sql) )
    {
        return SQLDB_ERROR;
    }

    c_str = sql.c_str();
    /* res = PQexec(_db, c_str); */ /* PQexec���ؽ�����Ƕ����Ƶ� */
    _res = PQexecParams(_pAdoDB->GetConnectDB(), c_str, 0, NULL,	NULL, NULL,	NULL, 0); /* ���һ������ 0--�����ַ��������, 1--���ض����ƽ���� */    
    
    status = PQresultStatus(_res);
    
    /* ���ִ��ʧ�ܣ���Ҫ�ж����ݿ������Ƿ����� */
    if ( PGRES_FATAL_ERROR == status )
    {
        if( PQstatus(_pAdoDB->GetConnectDB()) != CONNECTION_OK )
        {
           //�����������ݿ⣬��ִ��һ��
            PQreset(_pAdoDB->GetConnectDB());
            PQclear(_res);
            _res = NULL;
            _res = PQexecParams(_pAdoDB->GetConnectDB(), c_str, 0, NULL,	NULL, NULL,	NULL, 0);    /* ���һ������ 0--�����ַ��������, 1--���ض����ƽ���� */    

            status = PQresultStatus(_res);
        }
    }
        

    if ( status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK )
    {        
        PQclear(_res);
        _res = NULL;
        return SQLDB_EXEC_SQL_FAILED;
    }  
    
    if ( status == PGRES_TUPLES_OK )
    {    
        _nFields  = PQnfields(_res);
        _nTuples = PQntuples(_res);    
        
        if ( _nTuples > 0 )
        {
            _row = 0;
        }  
    }
    pCommand->SetRecordSet(this);
    _bExecute = true;
      
    return SQLDB_OK;
}    

bool CADOCommand::GetCompleteSQL(string &sql)
{
    string proc;        
    int retParamCount; 
    vector<CADOParameter *>::iterator it;     
    string szField;
        
    if ( _strCommandText.empty() )
    {
        return false;
    }        
    
    if( typeCmdText == _nCommandType )
    {
        sql = _strCommandText;
        return true;
    }
    
    if ( 0 == _paramIn.size() )
    {
        proc = _strCommandText;
        proc.append("()");
    }
    else
    {

        proc = _strCommandText;
        proc.append("(");
        for(it = _paramIn.begin(); it != _paramIn.end(); it++)
        {
            if ( (*it)->GetType() == CADOParameter::paramText )
            {
                szField = (*it)->GetInputParamValue();
                char *sql_value = _pAdoDB->EscapeString(szField);
                if(!sql_value)
                {
                    SkyAssert(false);
                    return false;
                }
                
                proc.append("'" + string(sql_value) + "'");
                _pAdoDB->FreeString(sql_value);
            }
            else
            {                    
                proc.append((*it)->GetInputParamValue());                
            }
            
            if( it != _paramIn.end() - 1)
            {
                proc.append(",");
            }
        }      
        proc.append(")");      
    }
    retParamCount = _paramRet.size();
    sql = "select * from ";
    sql.append(proc + ";");
    
    return true;
}

void CADOCommand::SetRecordSet(CADORecordset *pRecordSet)
{
    vector<CADOParameter *>::iterator it; 
        
    _recordet = pRecordSet;
    
    for(it = _paramRet.begin(); it != _paramRet.end(); it++)
    {
        (*it)->SetRecordSet(pRecordSet);
    }
}
/**/
