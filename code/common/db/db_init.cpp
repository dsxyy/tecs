/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：db_init.cpp
* 文件标识：见配置管理计划书
* 内容摘要：数据库初始化模块
* 当前版本：1.0
* 作    者：张文剑
* 完成日期：2011年08月16日
*
* 修改记录1：
*     修改日期：2011/12/01
*     版 本 号：V1.0
*     修 改 人：张文剑
*     修改内容：创建
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
        
//ADO VNM 模块 DB 连接池
map<pthread_t, CADODatabase *> s_mapAdoDbThreadUsed;
vector<CADODatabase *> s_vecAdoDbFree;
#define CONNECT_ADO_DB_INSTANCE_MAX_NUM   15   /* 最大ADO DB连接数   */
static CADODatabase *s_pAdoDbConnects[CONNECT_ADO_DB_INSTANCE_MAX_NUM] = {NULL};

// static CADODatabase *s_pAdoDbConnection = NULL;        
static SqlDB *s_pDbConnection = NULL;
static DBConfig s_RunningConfig;
static STATUS CheckDbSchema(const string& update_ver,const string& bugfix_ver);


/**********************************************************************
* 函数名称：DBInit
* 功能描述：初始化数据库模块
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
STATUS DBInitNormal(const DBConfig& config)
{
#define CONNECT_DB_WAIT_TIME          10    /* 尝试连接数据库的时间间隔 */
#define CONNECT_DB_TRY_MAX_COUNT      12    /* 最大尝试次数,最多12*10=2分钟   */
    int count = 0;
    STATUS ret = ERROR;
    
    // 数据库初始化
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
* 函数名称：DBInit
* 功能描述：初始化数据库模块 ADO
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
STATUS DBInitAdo(const DBConfig& config)
{
#define CONNECT_ADO_DB_WAIT_TIME          5    /* 尝试连接数据库的时间间隔 */
#define CONNECT_ADO_DB_TRY_MAX_COUNT      3    /* 最大尝试次数,最多12*10=2分钟   */
//#define CONNECT_ADO_DB_INSTANCE_MAX_NUM   15   /* 最大ADO DB连接数   */
    int count = 0;
    
    // 数据库初始化
    printf("connecting to ado database server=%s,port=%d,database=%s,user=%s,password=%s\n",
            config.server_url.c_str(),
            config.server_port,
            config.database.c_str(),
            config.user.c_str(),
            config.passwd.c_str());
    s_RunningConfig = config;

    //s_mapAdoDbThreadUsed.clear();
    //s_vecAdoDbFree.clean();
    // 申请连接池 
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
* 函数名称：DBInit
* 功能描述：初始化数据库模块
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
STATUS DBInit(const DBConfig& config)
{
     return DBInitNormal(config);
}

/**********************************************************************
* 函数名称：GetDB
* 功能描述：获取共用的数据库连接对象指针
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
SqlDB *GetDB()
{
    return s_pDbConnection;
}

/**********************************************************************
* 函数名称：GetDB
* 功能描述：获取共用的数据库ADO连接对象指针
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

CADODatabase *GetAdoDB()
{
    pthread_t t = pthread_self();
    // 可重入
    if( s_mapAdoDbThreadUsed.find(t) != s_mapAdoDbThreadUsed.end() )
    {
        return s_mapAdoDbThreadUsed[t];
    }

    // 获取一个空闲的
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
* 函数名称：GetPrivateDB
* 功能描述：获取私有的数据库连接对象指针
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

    //null表示没有打过任何补丁，这个定义和数据库相关shell脚本是一致的，存在耦合关系
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

