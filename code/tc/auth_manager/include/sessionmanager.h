/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* sessionmanager.h
* 文件标识：见配置管理计划书
* 内容摘要：sessionmanager类的头文件
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
#ifndef _SESSION_MANAGER_H
#define _SESSION_MANAGER_H

#include "sky.h"

using namespace std;

// 禁用拷贝构造宏定义
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)
#endif

namespace zte_tecs
{

struct SessionInfo
{
    string    user;
    string    secret;
    Datetime  login_time;
};

class SessionManager
{
public:
    static SessionManager* CreateInstance()
    {
        if(_instance == NULL)
        {
            _instance = new SessionManager();
        }
        return _instance;
    }

    static SessionManager* GetInstance()
    {
        return _instance;
    }

    //创建一个新会话
    string NewSession(const string& user, const string& secret);

    //删除一个会话
    void DeleteSession(const string& session);

    //通过会话获取授权信息
    string GetSecretBySession(const string& session);

    //获取当前会话数量
    int GetCurrentSessionCount() { return _sessions.size(); }

    const map<string, SessionInfo>& GetSessionsMap() const { return _sessions; }

    //把当前的会话信息打印出来
    void ShowAllSessions();

private:
    SessionManager();
    ~SessionManager();

    DISALLOW_COPY_AND_ASSIGN(SessionManager);

    map<string, SessionInfo> _sessions;

    static SessionManager* _instance;

    //生成一个会话ID
    string GenerateSessionId();

    //清楚超时会话
    void CleanTimeoutSession();

    void CheckUserSession(const string& user);

    //定时器
    class SessionTimer : public MessageHandlerTpl<MessageQueue>
    {
    public:
        void StartLoopTimer()
        {
            TimeOut timeout;
            timeout.type = LOOP_TIMER;
            timeout.duration = 60000;  // 1 minute

            TIMER_ID tid = m->CreateTimer();
            m->SetTimer(tid, timeout);
        }

        void MessageEntry(const MessageFrame& message)
        {
            SessionManager::GetInstance()->CleanTimeoutSession();
        }
    };

    SessionTimer* _timer;

    Mutex         _mutex;
};

}

#endif