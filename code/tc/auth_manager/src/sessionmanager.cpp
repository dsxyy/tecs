/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：sessionmanager.cpp
* 文件标识：见配置管理计划书
* 内容摘要：sessionmanager类的实现文件
* 当前版本：1.0
* 作    者：王明辉
* 完成日期：
*
* 修改记录1：
*     修改日期：2012/8/22
*     版 本 号：V1.0
*     修 改 人：王明辉
*     修改内容：创建
*******************************************************************************/

#include "sessionmanager.h"
#include "log_agent.h"
#include "uuid.h"
#include "user_pool.h"

namespace zte_tecs
{

#define MAX_RETRY_TIME   5
#define SESSION_LENGTH   24

SessionManager* SessionManager::_instance = NULL;

SessionManager::SessionManager()
{
    _timer = new SessionTimer();
    _timer->Start("SessionManagerHandler");
    _timer->StartLoopTimer();

    _mutex.Init();
}

SessionManager::~SessionManager()
{
    delete _timer;
}

string SessionManager::NewSession(const string& user, const string& secret)
{
    CheckUserSession(user);

    for(int i=0; i<MAX_RETRY_TIME; i++)
    {
        string session = GenerateSessionId();

        SessionInfo si;

        si.user   = user;
        si.secret = secret;
        si.login_time.refresh();

        map<string, SessionInfo>::iterator it = _sessions.find(session);

        if(it == _sessions.end())
        {
            _mutex.Lock();
            _sessions.insert(make_pair(session, si));
            _mutex.Unlock();  
            return session;
        }

        sleep(1);
    }

    OutPut(SYS_ALERT, "The unlucky guy five try have not been get a session!");

    return "";
}

void SessionManager::DeleteSession(const string& session)
{
    _mutex.Lock();
    _sessions.erase(session);
    _mutex.Unlock();     
}

string SessionManager::GetSecretBySession(const string& session)
{
    map<string, SessionInfo>::iterator it = _sessions.find(session);

    if(it != _sessions.end())
    {
        return it->second.secret;
    }

    return "";
}

string SessionManager::GenerateSessionId()
{
    /*ostringstream oss;
    srand(clock());
    int rand_num;

    for(int i=0; i<SESSION_LENGTH; i++)
    {
        rand_num = rand();
        srand(rand_num);
        char c = 'a' + rand_num%26;
        oss << c;
        usleep(1000);
    }

    return oss.str();*/

#if 0
    uuid_t uid;
    char   acUid[64]={0};
    string tmp_str;
    uuid_create(&uid);        
    sprintf(acUid,UUIDFORMAT_PRINTF,uid.time_low, 
                      uid.time_mid,                  uid.time_hi_and_version, 
                      uid.clock_seq_hi_and_reserved, uid.clock_seq_low,
                      uid.node[0],uid.node[1],uid.node[2], 
                      uid.node[3],uid.node[4],uid.node[5]);
    tmp_str = acUid;
    return tmp_str;    
#endif
    return GenerateUUID();
}

void SessionManager::CleanTimeoutSession()
{
    map<string, SessionInfo>::iterator it = _sessions.begin();

    Datetime now;
    now.refresh();
    
    int now_int = now.tointeger();

    _mutex.Lock();        
    for(;it!=_sessions.end();)
    {
        time_t timeout = it->second.login_time.tointeger() + 24*60*60;
        
        if(timeout < now_int)
        {
            _sessions.erase(it++);
        }
        else
        {
            it ++;
        }
    }
    _mutex.Unlock();    
}

void SessionManager::CheckUserSession(const string& user)
{
    string    firstLoginSession;
    Datetime  firstLoginTime;     
    int       currentSessionCount;

    firstLoginTime.refresh();    
    currentSessionCount = 0;
    
    //find user's session count and detect the first
    map<string, SessionInfo>::iterator it = _sessions.begin();
    for(;it!=_sessions.end(); it++)
    {
        if(it->second.user == user)
        {
            currentSessionCount++;

            if(it->second.login_time < firstLoginTime)
            {
                firstLoginTime = it->second.login_time;
                firstLoginSession = it->first;
            }            
        }
    }  

    //compare with user's max_session
    User* u = UserPool::GetInstance()->Get(user, false);
    if(u != NULL && currentSessionCount < u->get_max_session())
    {
        return;
    }

    //delete first if already equal max_session
    DeleteSession(firstLoginSession);
}

void SessionManager::ShowAllSessions()
{
    map<string, SessionInfo>::iterator it = _sessions.begin();
    cout << "---------------------------------------------------------" << endl;    
    cout << "Current sessions count: " << GetCurrentSessionCount() << endl;
    cout << "---------------------------------------------------------" << endl;
    
    for( ; it!=_sessions.end(); it++)
    {
        cout << (it->second.user.empty()? "<<snmp>>" : it->second.user)  
             << "(" << it->second.login_time.tostr()  << ")" 
             << " : " << it->first << endl;
    }

    cout << "---------------------------------------------------------" << endl;
}

void dbg_show_all_sessions()
{
    SessionManager::GetInstance()->ShowAllSessions();
}

DEFINE_DEBUG_FUNC(dbg_show_all_sessions);

} // namespace