/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�drivermanager.cpp
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��drivermanager���ʵ���ļ�
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
#include "mid.h"
#include "drivermanager.h"
#include "authrequest.h"
#include "sessionmanager.h"

namespace zte_tecs
{

DriverManager* DriverManager::_instance = NULL;

/************************************************************
* �������ƣ� StartMu
* ���������� ����mu�ĺ���
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2012/5         V1.0       lixiaocheng         ����
***************************************************************/
STATUS DriverManager::StartMu(const string& name)
{
    if(_mu)
    {
        SkyAssert(false);
        return ERROR_DUPLICATED_OP;
    }
    
    // ��Ϣ��Ԫ�Ĵ����ͳ�ʼ��
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

    //���Լ������ϵ���Ϣ����ʹ��Ϣ��Ԫ״̬�л�������̬
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

