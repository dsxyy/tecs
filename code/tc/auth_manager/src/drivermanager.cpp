/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：drivermanager.cpp
* 文件标识：见配置管理计划书
* 内容摘要：drivermanager类的实现文件
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
#include "mid.h"
#include "drivermanager.h"
#include "authrequest.h"
#include "sessionmanager.h"

namespace zte_tecs
{

DriverManager* DriverManager::_instance = NULL;

/************************************************************
* 函数名称： StartMu
* 功能描述： 启动mu的函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2012/5         V1.0       lixiaocheng         创建
***************************************************************/
STATUS DriverManager::StartMu(const string& name)
{
    if(_mu)
    {
        SkyAssert(false);
        return ERROR_DUPLICATED_OP;
    }
    
    // 消息单元的创建和初始化
    _mu = new MessageUnit(name);
    if (!_mu)
    {
        OutPut(SYS_EMERGENCY, "Create mu %s failed!\n",name.c_str());
        return ERROR_NO_MEMORY;
    }

    STATUS ret = _mu->SetHandler(this);
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "SetHandler mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }

    ret = _mu->Run();
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "Run mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }

    ret = _mu->Register();
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "Register mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }

    //给自己发送上电消息，促使消息单元状态切换到工作态
    MessageFrame message(SkyInt32(0), EV_POWER_ON);
    ret = _mu->Receive(message);
    if (SUCCESS != ret)
    {
        SkyAssert(0);
        return ret;
    }
    _mu->Print();
    return SUCCESS;
}


/* -------------------------------------------------------------------------- */

DriverManager::DriverManager()
{
    _mu = NULL;
}


/* -------------------------------------------------------------------------- */

DriverManager::~DriverManager()
{
    delete _mu;
    _mu = NULL;   
}

/* -------------------------------------------------------------------------- */

void DriverManager::MessageEntry(const MessageFrame& frame)
{
    switch(_mu->get_state())
    {
    case S_Startup:
    {
        switch(frame.option.id())
        {
            case EV_POWER_ON:
            {
                _mu->set_state(S_Work);
                OutPut(SYS_NOTICE, "%s power on!\n",_mu->name().c_str());
            }
        }
        break;
    }

    case S_Work:
    {
        switch(frame.option.id())
        {
            case EV_AUTHENTICATE:
            {  
                HandleAuthenticateEvent(frame);
                break;       
            }         
            
            case EV_AUTHORIZE:
            {  
                HandleAuthorizeEvent(frame);
                break;       
            }  

            case EV_SESSION:
            {
                HandleSessionEvent(frame);
                break;
            }
            
            default:
            {
                OutPut(SYS_ALERT, "AuthDriverManager receive Unknown message: %d.\n", frame.option.id());
                break;        
            } 
            
        }
        break;
    }

    default:
        break;
        
    }
}

void DriverManager::HandleAuthenticateEvent(const MessageFrame& frame)
{
    bool result = false;
    string message = "Unknown Error";

    OutPut(SYS_INFORMATIONAL, "AuthDriverManager receive EV_AUTHENTICATE msg.\n");
    AuthenticateMessage am;
    am.deserialize(frame.data);
    
    string username = am.get_username();
    string secret = am.get_secret();
    string session = username + ":" + secret;
    
    int64 rc = _upool->Authenticate(session);
    
    if(rc == -1)
    {
        stringstream err_info;
        err_info << username << " Authenticate Error." << endl;
        
        message = err_info.str(); 
        result = false;
    }
    else
    {
        stringstream uid_str;
        uid_str << rc;
        message = uid_str.str();
        result = true;
    }
    
    DriverMessage msg(result, message);
    MessageOption option(frame.option.sender(), EV_AUTHENTICATE, 0, 0);
    _mu->Send(msg,option);  
}

void DriverManager::HandleAuthorizeEvent(const MessageFrame& frame)
{
    bool result = false;
    string message = "Unknown Error";

    OutPut(SYS_INFORMATIONAL, "AuthDriverManager receive EV_AUTHORIZE msg.\n");
    
    AuthorizeMessage am;
    am.deserialize(frame.data);
    
    AuthRequest ar(am.get_uid());
    ar.AddAuthorize((AuthRequest::Object)am.get_ob(), am.get_ob_id(),
                    (AuthRequest::Operation)am.get_op(), am.get_owner(),
                    am.get_pub());
    
    int rc = UserPool::Authorize(ar);
    
    if(rc == -1)
    {
        stringstream err_info;
        err_info << "User[" << am.get_uid() << "] Authorize Error." << endl;
        
        message = err_info.str(); 
        result = false;                    
    }
    else
    {
        message = "Success";
        result = true;                    
    }
    
    DriverMessage msg(result, message);
    MessageOption option(frame.option.sender(), EV_AUTHORIZE, 0, 0);
    _mu->Send(msg,option);  
}

void DriverManager::HandleSessionEvent(const MessageFrame& frame)
{
    SessionMessage req;
    req.deserialize(frame.data);

    string session = req.get_session();
    string secret  = SessionManager::GetInstance()->GetSecretBySession(session);

    SessionMessage ack(session, secret);
    MessageOption option(frame.option.sender(), EV_SESSION, 0, 0);
    _mu->Send(ack, option);     
}

/* -------------------------------------------------------------------------- */

}

