/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�vacuum.cpp
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ�����ݿ���������ģ��
* ��ǰ�汾��1.0
* ��    �ߣ����Ľ�
* ������ڣ�2013��06��09��
*
* �޸ļ�¼1��
*     �޸����ڣ�2013/06/09
*     �� �� �ţ�V1.0
*     �� �� �ˣ����Ľ�
*     �޸����ݣ�����
* ��PostgreSQL�У�ʹ��delete��update���ɾ������µ������в�û�б�ʵ��ɾ����
* ��ֻ���ھɰ汾�����е������ַ�Ͻ����е�״̬��Ϊ��ɾ�����ѹ��ڡ���˵���
* �ݱ��е����ݱ仯��ΪƵ��ʱ��һ��ʱ��֮��ñ���ռ�õĿռ佫���úܴ�Ȼ
* ��������ȴ���ܱ仯����Ҫ��������⣬��Ҫ���ڶ����ݱ仯Ƶ�������ݱ�ִ��
* VACUUM�������������¼���ԭ�򣬱������������� PostgreSQL �� VACUUM �����
* 1. �ָ���Щ���Ѹ��µĻ���ɾ������ռ�ݵĴ��̿ռ䡣
* 2. ���� PostgreSQL ��ѯ�滮��ʹ�õ�����ͳ����Ϣ��
* 3. ������Ϊ���� ID �ص���ɵ��Ͼ����ݵĶ�ʧ��
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

#define EV_VACUUM_POWER_ON      EV_TIMER_1 //�ϵ�����
#define EV_VACUUM_TIMER         EV_TIMER_2 //������ת��ʱ��
#define EV_VACUUM_REQ           EV_TIMER_3 //ָ����ִ�в���

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
    
    //������ע�ᡢ������Ϣ��Ԫ
    ret = Start(MU_VACUUM_MANAGER);
    if(SUCCESS != ret)
    {
        return ret;
    }

    //��Ϣ��Ԫû�и���ĳ�ʼ�����̣������Ϳ��Ըɻ���
    m->set_state(S_Work);
    return SUCCESS;
}

STATUS VacuumManager::RegisterTable(const string& table, int interval)
{
    //��������ѭ����ʱ��
    TIMER_ID timer = m->CreateTimer();
    SkyAssert(timer != INVALID_TIMER_ID);
    TimeOut timeout;
    timeout.duration = 1000 * interval;
    timeout.msgid = EV_VACUUM_TIMER;
    timeout.type = LOOP_TIMER;
    //�ڶ�ʱ�����ַ��������м�¼Ҫ����ı�����
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


