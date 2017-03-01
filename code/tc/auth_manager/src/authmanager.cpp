/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�authmanager.cpp
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��authmanager���ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ������
* ������ڣ�
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/7/25
*     �� �� �ţ�V1.0
*     �� �� �ˣ������
*     �޸����ݣ�����
*******************************************************************************/
#include "authmanager.h"
#include "db_config.h"

namespace zte_tecs
{

AuthManager* AuthManager::_instance = NULL;

/* -------------------------------------------------------------------------- */

 AuthManager::AuthManager()
{
    pthread_mutex_init(&_mutex,0);

    _dm = NULL;
    _upool = NULL;
    _gpool = NULL;
    _mu = NULL;
}

/* -------------------------------------------------------------------------- */

AuthManager::~AuthManager()
{
    pthread_mutex_destroy(&_mutex);

    if(_dm) 
    {
        delete _dm;
    }
    
    _dm = NULL;
}

/* -------------------------------------------------------------------------- */

STATUS AuthManager::LoadMads(UserPool* upool)
{
    _upool = upool;
    _gpool = UsergroupPool::GetInstance();

    _dm = DriverManager::GetInstance();
    if(!_dm)
    {
        return ERROR_NO_MEMORY;
    }

    return _dm->Init();
}

/* -------------------------------------------------------------------------- */

bool AuthManager::Authenticate(AuthRequest * ar)
{
    AuthenticateMessage msg(ar->_uid, ar->_username, ar->_password,ar->_secret);
    MID target(PROC_APISVR, DRIVERMANAGER);
    MessageOption option(target, EV_AUTHENTICATE, 0, 0);
    ar->Send(msg, option);

    DriverMessage dm;
    ar->Wait(dm);
    ar->set_result(dm.get_result());
    ar->set_message(dm.get_message());
    ar->Notify();

    return ar->get_result();
}

/* -------------------------------------------------------------------------- */

bool AuthManager::Authorize(AuthRequest * ar)
{
    MID target(PROC_APISVR, DRIVERMANAGER);
    MessageOption option(target, EV_AUTHORIZE, 0, 0);
    ar->Send(ar->get_auth_req(),option);

    DriverMessage dm;
    ar->Wait(dm);
    ar->set_result(dm.get_result());
    ar->set_message(dm.get_message());
    ar->Notify();

    return ar->get_result();
}

/* -------------------------------------------------------------------------- */

void AuthManager::Lock()
{
    pthread_mutex_lock(&_mutex);
}

/* -------------------------------------------------------------------------- */

void AuthManager::Unlock()
{
    pthread_mutex_unlock(&_mutex);
}

/* -------------------------------------------------------------------------- */

}
