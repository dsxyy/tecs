/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vacuum.cpp
* 文件标识：见配置管理计划书
* 内容摘要：数据库例行清理模块
* 当前版本：1.0
* 作    者：张文剑
* 完成日期：2013年06月09日
*
* 修改记录1：
*     修改日期：2013/06/09
*     版 本 号：V1.0
*     修 改 人：张文剑
*     修改内容：创建
* 在PostgreSQL中，使用delete和update语句删除或更新的数据行并没有被实际删除，
* 而只是在旧版本数据行的物理地址上将该行的状态置为已删除或已过期。因此当数
* 据表中的数据变化极为频繁时，一段时间之后该表所占用的空间将会变得很大，然
* 而数据量却可能变化不大。要解决该问题，需要定期对数据变化频繁的数据表执行
* VACUUM操作。由于以下几个原因，必须周期性运行 PostgreSQL 的 VACUUM 命令∶
* 1. 恢复那些由已更新的或已删除的行占据的磁盘空间。
* 2. 更新 PostgreSQL 查询规划器使用的数据统计信息。
* 3. 避免因为事务 ID 重叠造成的老旧数据的丢失。
*******************************************************************************/
#include "sky.h"
#include "db_config.h"
#include "pool_sql.h"
#include "mid.h"

static int vacuum_print_on = 0;
DEFINE_DEBUG_VAR(vacuum_print_on);
#define Debug(fmt,arg...) \
        do \
        { \
            if(vacuum_print_on) \
            { \
                printf("[%s:%d]"fmt,__func__,__LINE__, ##arg); \
            } \
        }while(0)

#define EV_VACUUM_POWER_ON      EV_TIMER_1 //上电启动
#define EV_VACUUM_TIMER         EV_TIMER_2 //周期运转定时器
#define EV_VACUUM_REQ           EV_TIMER_3 //指定表执行操作

class VacuumReq: public Serializable
{
public:
    VacuumReq()
    {
        option = SqlCommand::VACUUM_NORMAL;
    };

    SERIALIZE
    {
        SERIALIZE_BEGIN(VacuumReq);
        WRITE_VALUE(table);
        WRITE_VALUE(option);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VacuumReq);
        READ_VALUE(table);
        READ_VALUE(option);
        DESERIALIZE_END();
    };

    string table;
    int option;
};

class VacuumManager:public MessageHandlerTpl<MessageUnit>
{
public:
    static VacuumManager* GetInstance()
    {
        if(NULL == instance)
        {
            instance = new VacuumManager(); 
        }
        return instance;
    };
    
    ~VacuumManager()
    {
    };

    STATUS Init();
    STATUS RegisterTable(const string& table, int interval);
    STATUS ReceiveVacuumReq(const VacuumReq& req);
    void MessageEntry(const MessageFrame&);

private:
    VacuumManager()
    {
        db = NULL;
    }
    
    DISALLOW_COPY_AND_ASSIGN(VacuumManager);
    static VacuumManager *instance;
    SqlDB *db;
    list<TIMER_ID> vacuum_timers;
    Mutex timers_mutex;
};

VacuumManager* VacuumManager::instance = NULL;

STATUS VacuumManager::Init()
{
    STATUS ret = ERROR;
    SkyAssert(NULL == db);
    db = GetPrivateDB();
    if(!db)
    {
        return ERROR;
    }

    ret = timers_mutex.Init();
    if(SUCCESS != ret)
    {
        return ret;
    }
    
    //创建、注册、启动消息单元
    ret = Start(MU_VACUUM_MANAGER);
    if(SUCCESS != ret)
    {
        return ret;
    }

    //消息单元没有更多的初始化流程，起来就可以干活了
    m->set_state(S_Work);
    return SUCCESS;
}

STATUS VacuumManager::RegisterTable(const string& table, int interval)
{
    //创建设置循环定时器
    TIMER_ID timer = m->CreateTimer();
    SkyAssert(timer != INVALID_TIMER_ID);
    TimeOut timeout;
    timeout.duration = 1000 * interval;
    timeout.msgid = EV_VACUUM_TIMER;
    timeout.type = LOOP_TIMER;
    //在定时器的字符串参数中记录要清理的表名称
    timeout.str_arg = table;

    timers_mutex.Lock();
    vacuum_timers.push_back(timer);
    timers_mutex.Unlock();
    return m->SetTimer(timer,timeout);
}

STATUS VacuumManager::ReceiveVacuumReq(const VacuumReq& req)
{
    MessageFrame message(req, EV_VACUUM_REQ);
    return m->Receive(message);
}

void VacuumManager::MessageEntry(const MessageFrame& message)
{
    switch (message.option.id())
    {
        case EV_VACUUM_TIMER:
        {
            string table = m->GetTimerStrArg();
            SqlCommand sql(db,table,SqlCommand::WITHOUT_TRANSACTION);
            Debug("EV_VACUUM_TIMER: vacuum table %s ...\n",
                table.c_str());
            int ret = sql.Vacuum(SqlCommand::VACUUM_NORMAL);
            if (SQLDB_OK != ret)
            {
                Debug("failed to vacuum table %s! ret = %d!\n",
                    table.c_str(),ret);
            }
            else
            {
                Debug("vacuum table %s success!",table.c_str());
            }   
            break;
        }

        case EV_VACUUM_REQ:
        {
            VacuumReq req;
            SkyAssert(req.deserialize(message.data));
            SqlCommand sql(db,req.table,SqlCommand::WITHOUT_TRANSACTION);
            Debug("EV_VACUUM_REQ: vacuum table %s with option %d ...\n",
                req.table.c_str(),req.option);
            int ret = sql.Vacuum((SqlCommand::VacuumOption)req.option);
            if (SQLDB_OK != ret)
            {
                Debug("failed to vacuum table %s with option %d! ret = %d!\n",
                    req.table.c_str(),req.option,ret);
            }
            else
            {
                Debug("vacuum table %s success!",req.table.c_str());
            } 
            break;
        }
        
        default:
        {
            break;
        }
    }
}

STATUS InitDbVacuum()
{
    VacuumManager *p = VacuumManager::GetInstance();
    SkyAssert(NULL != p);
    AddKeyMu(MU_VACUUM_MANAGER);
    return p->Init();
}

STATUS RegisterDbVacuum(const string& table,int interval)
{
    SkyAssert(interval > 0);
    VacuumManager *p = VacuumManager::GetInstance();
    SkyAssert(NULL != p);
    return p->RegisterTable(table,interval);
}

STATUS VacuumTable(const string& table,SqlCommand::VacuumOption option)
{
    VacuumReq req;
    req.table = table;
    req.option = option;
    VacuumManager *p = VacuumManager::GetInstance();
    SkyAssert(NULL != p);
    return p->ReceiveVacuumReq(req);
}

void DbgRegisterDbVacuum(const char* table,int interval)
{
    STATUS ret = RegisterDbVacuum(table,interval);
    cout << "RegisterDbVacuum return " << ret << endl;
}
DEFINE_DEBUG_FUNC(DbgRegisterDbVacuum);

void DbgVacuumTable(const char* table,int option)
{
    STATUS ret = VacuumTable(table,(SqlCommand::VacuumOption)option);
    cout << "VacuumTable return " << ret << endl;
}
DEFINE_DEBUG_FUNC(DbgVacuumTable);


