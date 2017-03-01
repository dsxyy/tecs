/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：authmanager.cpp
* 文件标识：见配置管理计划书
* 内容摘要：authmanager类的实现文件
* 当前版本：1.0
* 作    者：李振红
* 完成日期：
*
* 修改记录1：
*     修改日期：2011/7/25
*     版 本 号：V1.0
*     修 改 人：李振红
*     修改内容：创建
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
