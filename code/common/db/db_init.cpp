/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�db_init.cpp
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ�����ݿ��ʼ��ģ��
* ��ǰ�汾��1.0
* ��    �ߣ����Ľ�
* ������ڣ�2011��08��16��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/12/01
*     �� �� �ţ�V1.0
*     �� �� �ˣ����Ľ�
*     �޸����ݣ�����
*******************************************************************************/
#include "sky.h"
#include "db_config.h"
#include "pool_sql.h"
#include <pthread.h>

static int db_print_on = 0;
DEFINE_DEBUG_VAR(db_print_on);
#define Debug(fmt,arg...) \
        do \
        { \
            if(db_print_on) \
            { \
                printf("[%s:%d]"fmt,__func__,__LINE__, ##arg); \
            } \
        }while(0)
        
//ADO VNM ģ�� DB ���ӳ�
map<pthread_t, CADODatabase *> s_mapAdoDbThreadUsed;
vector<CADODatabase *> s_vecAdoDbFree;
#define CONNECT_ADO_DB_INSTANCE_MAX_NUM   15   /* ���ADO DB������   */
static CADODatabase *s_pAdoDbConnects[CONNECT_ADO_DB_INSTANCE_MAX_NUM] = {NULL};

// static CADODatabase *s_pAdoDbConnection = NULL;        
static SqlDB *s_pDbConnection = NULL;
static DBConfig s_RunningConfig;
static STATUS CheckDbSchema(const string& update_ver,const string& bugfix_ver);


/**********************************************************************
* �������ƣ�DBInit
* ������������ʼ�����ݿ�ģ��
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
STATUS DBInitNormal(const DBConfig& config)
{
#define CONNECT_DB_WAIT_TIME          10    /* �����������ݿ��ʱ���� */
#define CONNECT_DB_TRY_MAX_COUNT      12    /* ����Դ���,���12*10=2����   */
    int count = 0;
    STATUS ret = ERROR;
    
    // ���ݿ��ʼ��
    printf("connecting to database server=%s,port=%d,database=%s,user=%s,password=%s\n",
            config.server_url.c_str(),
            config.server_port,
            config.database.c_str(),
            config.user.c_str(),
            config.passwd.c_str());
    s_RunningConfig = config;
    s_pDbConnection = new PostgreSqlDB(config.server_url,
                                        config.server_port,
                                        config.user,
                                        config.passwd,
                                        config.database);
    if (NULL == s_pDbConnection)
    {
        printf("failed to connect to db server %s!\n",config.server_url.c_str());
        SkyAssert(0);
        return ERROR;
    }
    
    count = 0;
    while (0 != s_pDbConnection->Open() && count < CONNECT_DB_TRY_MAX_COUNT)
    {
        ++count;        
        printf("retry to open database %s,retry count:%d,PostgreSQL Usage:service postgresql {start|stop|restart|reload|status}\n",config.database.c_str(),count);
        sleep(CONNECT_DB_WAIT_TIME);
    }
    
    if ( count >= CONNECT_DB_TRY_MAX_COUNT )
    {
        printf("failed to open database %s,retry count:%d!\n",config.database.c_str(),count);
        SkyAssert(false);
        return ERROR;    
    }
    
    if(!config.ignore_dbschema_ver && !config.schema_update_version.empty() && !config.schema_bugfix_version.empty())
    {
        ret = CheckDbSchema(config.schema_update_version,config.schema_bugfix_version);
        if(SUCCESS != ret)
        {
            printf("failed to CheckDbSchema!\n");
            SkyAssert(false);
            return ret;
        }
    }
    
    ret = InitDbVacuum();
    if(SUCCESS != ret)
    {
        printf("failed to InitDbVacuum!\n");
        SkyAssert(false);
        return ret;
    }
    return CreateTransactionKey();
}


/**********************************************************************
* �������ƣ�DBInit
* ������������ʼ�����ݿ�ģ�� ADO
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
STATUS DBInitAdo(const DBConfig& config)
{
#define CONNECT_ADO_DB_WAIT_TIME          5    /* �����������ݿ��ʱ���� */
#define CONNECT_ADO_DB_TRY_MAX_COUNT      3    /* ����Դ���,���12*10=2����   */
//#define CONNECT_ADO_DB_INSTANCE_MAX_NUM   15   /* ���ADO DB������   */
    int count = 0;
    
    // ���ݿ��ʼ��
    printf("connecting to ado database server=%s,port=%d,database=%s,user=%s,password=%s\n",
            config.server_url.c_str(),
            config.server_port,
            config.database.c_str(),
            config.user.c_str(),
            config.passwd.c_str());
    s_RunningConfig = config;

    //s_mapAdoDbThreadUsed.clear();
    //s_vecAdoDbFree.clean();
    // �������ӳ� 
    for(int i = 0; i < CONNECT_ADO_DB_INSTANCE_MAX_NUM; i++)
    {
        count = 0;

        CADODatabase * tmp = new CADODatabase(config.server_url.c_str(),config.server_port,config.user.c_str(),config.passwd.c_str(),config.database.c_str());
    // ado database
    if( NULL == tmp)
    {
        printf("failed to connect to ado db server %s!\n",config.server_url.c_str());
        SkyAssert(0);
        return ERROR;            
    }

    // ado open 
    count = 0;
     while (!tmp->Open() && count < CONNECT_ADO_DB_TRY_MAX_COUNT)
    {
        ++count;        
        printf("retry to open ado database %s,retry count:%d,PostgreSQL Usage:service postgresql {start|stop|restart|reload|status}\n",config.database.c_str(),count);
        sleep(CONNECT_ADO_DB_WAIT_TIME);
    }
    if ( count >= CONNECT_ADO_DB_TRY_MAX_COUNT )
    {
        printf("failed to open ado database %s,retry count:%d!\n",config.database.c_str(),count);
        //SkyAssert(0);
        return ERROR;    
    }

        s_pAdoDbConnects[i] = tmp;
        s_vecAdoDbFree.push_back(tmp);
    }

    
#if 0    
    if(!config.ignore_dbschema_ver && !config.schema_update_version.empty() && !config.schema_bugfix_version.empty())
    {
        STATUS ret = CheckDbSchema(config.schema_update_version,config.schema_bugfix_version);
        if(SUCCESS != ret)
        {
            return ret;
        }
    }
#endif     
    return SUCCESS;
}


/**********************************************************************
* �������ƣ�DBInit
* ������������ʼ�����ݿ�ģ��
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
STATUS DBInit(const DBConfig& config)
{
     return DBInitNormal(config);
}

/**********************************************************************
* �������ƣ�GetDB
* ������������ȡ���õ����ݿ����Ӷ���ָ��
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
SqlDB *GetDB()
{
    return s_pDbConnection;
}

/**********************************************************************
* �������ƣ�GetDB
* ������������ȡ���õ����ݿ�ADO���Ӷ���ָ��
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

CADODatabase *GetAdoDB()
{
    pthread_t t = pthread_self();
    // ������
    if( s_mapAdoDbThreadUsed.find(t) != s_mapAdoDbThreadUsed.end() )
    {
        return s_mapAdoDbThreadUsed[t];
    }

    // ��ȡһ�����е�
    vector<CADODatabase *>::iterator it = s_vecAdoDbFree.begin();
    if( it == s_vecAdoDbFree.end() )
    {
        return NULL;
    }
    CADODatabase *tmp = (*it);
    s_mapAdoDbThreadUsed[t] = tmp;

    s_vecAdoDbFree.erase(it);
    return tmp;    
}

void FreeAdoDB()
{    
    pthread_t t = pthread_self();
    if( s_mapAdoDbThreadUsed.find(t) == s_mapAdoDbThreadUsed.end() )
    {
        return;
    }

    CADODatabase * tmp = NULL;
    tmp = s_mapAdoDbThreadUsed[t];
    s_vecAdoDbFree.push_back(tmp);

    s_mapAdoDbThreadUsed.erase(t);
    return ;
}

/**********************************************************************
* �������ƣ�GetPrivateDB
* ������������ȡ˽�е����ݿ����Ӷ���ָ��
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
SqlDB *GetPrivateDB()
{
    if(!s_pDbConnection)
        return NULL;

    SqlDB *pPrivateDb = new PostgreSqlDB(s_RunningConfig.server_url,
                                            s_RunningConfig.server_port,
                                            s_RunningConfig.user,
                                            s_RunningConfig.passwd,
                                            s_RunningConfig.database);
    if (NULL == pPrivateDb)
    {
        printf("failed to connect to db server %s!\n",s_RunningConfig.server_url.c_str());
        SkyAssert(0);
        return NULL;
    }
    
    if (0 != pPrivateDb->Open())
    {
        printf("failed to open database %s!\n",s_RunningConfig.database.c_str());
        SkyAssert(0);
        return NULL;
    }
    return pPrivateDb;
}

static STATUS CheckDbSchema(const string& update_ver,const string& bugfix_ver)
{
    SqlCallbackable sqlcb(GetDB());
    string value;
    STATUS ret = sqlcb.SelectColumn("_schema_log","MAX(version_number)","version_number NOT LIKE 'bugfix.%'",value);
    if(SQLDB_ERROR == ret)
    {
        cerr << "CheckDbSchema failed to select!" << endl;
        return ERROR_DB_DELETE_FAIL;
    }
    else if(SQLDB_RESULT_EMPTY == ret)
    {
        cerr << "CheckDbSchema result is empty!" << endl;
        return ERROR_DB_DELETE_FAIL;
    }

    cout << "expected db update version = " << update_ver << endl;
    cout << "current db update version = " << value << endl;
    if(update_ver != value)
    {
        cerr << "database schema compatibility check failed!" << endl;
        cerr << "Forget to upgrade database? Please use: service xxx upgrade_db!" << endl;
        return ERROR;
    }

    //null��ʾû�д���κβ����������������ݿ����shell�ű���һ�µģ�������Ϲ�ϵ
    value = "null";
    ret = sqlcb.SelectColumn("_schema_log","MAX(version_number)","version_number LIKE 'bugfix.%'",value);
    if(SQLDB_ERROR == ret)
    {
        cerr << "CheckDbSchema failed to select!" << endl;
        return ERROR_DB_DELETE_FAIL;
    }
    else if(SQLDB_RESULT_EMPTY == ret)
    {
        cerr << "CheckDbSchema result is empty!" << endl;
        return ERROR_DB_DELETE_FAIL;
    }

    cout << "expected db bugfix version = " << bugfix_ver << endl;
    cout << "current db bugfix version = " << value << endl;
    if(bugfix_ver != value)
    {
        cerr << "database schema compatibility check failed!" << endl;
        cerr << "Forget to upgrade database? Please use: service xxx upgrade_db!" << endl;
        return ERROR;
    }
    return SUCCESS;
}

void DbgSelectColumnStr(const char* table, const char* column, const char* where)
{
    SqlCallbackable sqlcb(GetDB());
    string value;
    STATUS ret = sqlcb.SelectColumn(table,column,where,value);
    if(SQLDB_ERROR == ret)
    {
        cerr << "failed to select!" << endl;
        return;
    }
    else if(SQLDB_RESULT_EMPTY == ret)
    {
        cerr << "result is empty!" << endl;
        return;
    }

    cout << "result value:" << value << endl;
}
DEFINE_DEBUG_FUNC(DbgSelectColumnStr);

void DbgSelectColumnInt(const char* table, const char* column, const char* where)
{
    SqlCallbackable sqlcb(GetDB());
    int value = 0;
    STATUS ret = sqlcb.SelectColumn(table,column,where,value);
    if(SQLDB_ERROR == ret)
    {
        cerr << "failed to select!" << endl;
        return;
    }
    else if(SQLDB_RESULT_EMPTY == ret)
    {
        cerr << "result is empty!" << endl;
        return;
    }

    cout << "result value:" << value << endl;
}
DEFINE_DEBUG_FUNC(DbgSelectColumnInt);

void DbgSelectColumnInt64(const char* table, const char* column, const char* where)
{
    SqlCallbackable sqlcb(GetDB());
    int64 value = 0;
    STATUS ret = sqlcb.SelectColumn(table,column,where,value);
    if(SQLDB_ERROR == ret)
    {
        cerr << "failed to select!" << endl;
        return;
    }
    else if(SQLDB_RESULT_EMPTY == ret)
    {
        cerr << "result is empty!" << endl;
        return;
    }

    cout << "result value:" << value << endl;
}
DEFINE_DEBUG_FUNC(DbgSelectColumnInt64);

void DbgShowAdoDbPool()
{

    cout << "sky ado thread used list--------------------" << endl;
    map<pthread_t, CADODatabase *>::iterator it = s_mapAdoDbThreadUsed.begin();
    for(;  it != s_mapAdoDbThreadUsed.end(); ++it)
    {
        cout << "thread id : " << (*it).first << ", ado db : " << (*it).second << endl;
    }

    cout << "sky ado free list--------------------" << endl;
    vector<CADODatabase *>::iterator it_v = s_vecAdoDbFree.begin();
    for(;  it_v != s_vecAdoDbFree.end(); ++it_v)
    {
        cout << (*it_v) << endl;
    } 
}
DEFINE_DEBUG_FUNC(DbgShowAdoDbPool);

