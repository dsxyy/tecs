/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* sessionmanager.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��sessionmanager���ͷ�ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�������
* ������ڣ�
*
* �޸ļ�¼1��
*     �޸����ڣ�2012/8/22
*     �� �� �ţ�V1.0
*     �� �� �ˣ�������
*     �޸����ݣ�����
*******************************************************************************/
#ifndef _SESSION_MANAGER_H
#define _SESSION_MANAGER_H

#include "sky.h"

using namespace std;

// ���ÿ�������궨��
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

    //����һ���»Ự
    string NewSession(const string& user, const string& secret);

    //ɾ��һ���Ự
    void DeleteSession(const string& session);

    //ͨ���Ự��ȡ��Ȩ��Ϣ
    string GetSecretBySession(const string& session);

    //��ȡ��ǰ�Ự����
    int GetCurrentSessionCount() { return _sessions.size(); }

    const map<string, SessionInfo>& GetSessionsMap() const { return _sessions; }

    //�ѵ�ǰ�ĻỰ��Ϣ��ӡ����
    void ShowAllSessions();

private:
    SessionManager();
    ~SessionManager();

    DISALLOW_COPY_AND_ASSIGN(SessionManager);

    map<string, SessionInfo> _sessions;

    static SessionManager* _instance;

    //����һ���ỰID
    string GenerateSessionId();

    //�����ʱ�Ự
    void CleanTimeoutSession();

    void CheckUserSession(const string& user);

    //��ʱ��
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