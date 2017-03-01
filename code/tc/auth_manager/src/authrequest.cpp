/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�authrequest.cpp
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��authrequest���ʵ���ļ�
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
#include "authrequest.h"
#include "authmanager.h"
#include "user_pool.h"
#include "usergroup.h"

#include <iostream>
#include <sstream>

namespace zte_tecs
{
    
/* -------------------------------------------------------------------------- */

AuthRequest::AuthRequest(int64 uid)
{
    _uid = uid;
    _self_authorize = true;

    set_result(false);
    set_message("default");
    
    _mu = new MessageUnit(TempUnitName(AUTHREQUEST));
    _mu->Register(); 
}

/* -------------------------------------------------------------------------- */

AuthRequest::~AuthRequest()
{
    if(_mu) delete _mu;
    _mu = NULL;
}

/* -------------------------------------------------------------------------- */

void AuthRequest::AddAuthenticate( const string &username,
                                   const string &password,
                                   const string &secret  )
{
    _username = username;
    _password = password;
    _secret  = secret;
}

/* -------------------------------------------------------------------------- */

void AuthRequest::AddAuthorize( Object        ob,
                                const string& ob_id,
                                Operation     op,
                                int64         owner,
                                bool          pub)
{
    bool          auth;

    //store the info as authmsg
    AuthorizeMessage amsg(_uid, ob, ob_id, op, owner, pub);
    _auth_req = amsg; 

    UserPool* upool = UserPool::GetInstance();

    if(upool == NULL)
    {
        return;
    }
    
    //get the owner
    User* u = upool->Get(_uid, false);
    User* o = upool->Get(owner, false);

    // -------------------------------------------------------------------------
    // Authorize the request for self authorization
    // -------------------------------------------------------------------------
    auth = false;

    if(_uid == 1 || u->get_role() == UserGroup::CLOUD_ADMINISTRATOR)
    {
        switch (op)
        {
            case CREATE:
                auth = true;
                break;

            case DELETE:
                auth = true;    
                break;

            case USE:
                if(ob == PROJECT || ob == VM || ob == VMTEMPLATE || ob == VMCONFIG)
                {
                    auth = (owner == _uid) || pub;
                }
                else
                {
                    auth = true;
                }
                break;

            case MANAGE:
                if(ob == PROJECT || ob == VM)
                {
                    auth = (owner == _uid) || pub;
                }
                else
                {
                    auth = true;
                }
                break;
                
            case INFO: 
                auth = true;
                break;
        }           
    }
    else
    {
        switch (op)
        {
        case CREATE:
            if(ob == VM || ob == NET || ob == IMAGE || ob == PROJECT || ob == VMCONFIG|| ob == AFFINITY_REGION)
            {
                auth = true;
            }
            else if(ob == USER)
            {
                if(u->get_role() == UserGroup::ACCOUNT_ADMINISTRATOR)
                {
                    auth = true;
                }
            }
            break;

        case DELETE:
            if(ob == USER)
            {
                if(   (owner == _uid) 
                   || (u->get_role() == UserGroup::ACCOUNT_ADMINISTRATOR && o != NULL && o->get_group() == u->get_group()))

                {
                    auth = true;
                }
            }
            else
            {
                auth = owner == _uid;
            }
            break;

        case USE:
            if(ob == NET || ob == IMAGE || ob == PROJECT || ob == VMTEMPLATE || ob == VMCONFIG)
            {
                auth = (owner == _uid) || pub;
            }
            else if (ob == HOST)
            {
                auth = true;
            }
            break;

        case MANAGE:
            if(ob == USER)
            {
                if(   (owner == _uid) 
                   || (u->get_role() == UserGroup::ACCOUNT_ADMINISTRATOR && o != NULL && o->get_group() == u->get_group()))
                {
                    auth = true;
                }
            }
            else
            {
                auth = owner == _uid;
            }
            break;
            
        case INFO: 
            if(ob == USER)
            {
                if(   (owner == _uid) 
                   || (u->get_role() == UserGroup::ACCOUNT_ADMINISTRATOR ))

                {
                    auth = true;
                }
            } 
            else
            {
                auth = owner == _uid; 
            }
            break;
        }
    }


    _self_authorize = _self_authorize && auth;
}

/* -------------------------------------------------------------------------- */

void AuthRequest::AddAuthorize( Object        ob,
                                int64         ob_id,
                                Operation     op,
                                int64         owner,
                                bool          pub)
{
    ostringstream oss;
    oss << ob_id;

    AddAuthorize(ob, oss.str(), op, owner, pub);
}

/* -------------------------------------------------------------------------- */

void AuthRequest::Wait(DriverMessage& msg)
{
    MessageFrame frame;
    
    STATUS ret = _mu->Wait(&frame, 3000);

    if(ret == SUCCESS)
    {
        msg.deserialize(frame.data);
    }
    else if(ret == ERROR_TIME_OUT)
    {
        msg.set_result(false);
        msg.set_message("Time Out");
    }
    else
    {
        msg.set_result(false);
        msg.set_message("Network Error");
    }
}

/* -------------------------------------------------------------------------- */

void AuthRequest::Notify()
{
    return;
}

/* -------------------------------------------------------------------------- */

bool AuthRequest::PlainAuthorize()
{
    return ( _uid == 1 || _self_authorize );
}

/* -------------------------------------------------------------------------- */

bool AuthRequest::PlainAuthenticate()
{
    return (_password == _secret);
}

/* -------------------------------------------------------------------------- */

string * AuthRequest::Base64Encode(const string& in)
{
    /*�ݲ�֧��
    BIO *     bio_mem;
    BIO *     bio_64;

    char *    encoded_c;
    long int  size;

    bio_64  = BIO_new(BIO_f_base64());
    bio_mem = BIO_new(BIO_s_mem());

    BIO_push(bio_64, bio_mem);

    BIO_set_flags(bio_64,BIO_FLAGS_BASE64_NO_NL);

    BIO_write(bio_64, in.c_str(), in.length());

    if (BIO_flush(bio_64) != 1)
    {
        return 0;
    }

    size = BIO_get_mem_data(bio_mem,&encoded_c);

    string * encoded = new string(encoded_c,size);

    BIO_free_all(bio_64);

    return encoded;
    */
    return NULL;
}

/* -------------------------------------------------------------------------- */

const AuthorizeMessage& AuthRequest::get_auth_req()
{
    return _auth_req;
}

/* -------------------------------------------------------------------------- */

bool AuthRequest::set_result(const bool result)
{
    _result = result;
    return true;
}

/* -------------------------------------------------------------------------- */

bool AuthRequest::get_result()
{
    return _result;
}

/* -------------------------------------------------------------------------- */

bool AuthRequest::set_message(const string& message)
{
    _message = message;
    return true;
}

/* -------------------------------------------------------------------------- */

string AuthRequest::get_message()
{
    return _message;
}

/* -------------------------------------------------------------------------- */ 

}
