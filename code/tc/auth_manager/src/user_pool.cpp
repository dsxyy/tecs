/******************************************************************************
* Copyright (c) 2011������������ͨѶ�ɷ����޹�˾
* All rights reserved.
* 
* �ļ����ƣ�userpool.cpp
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ���û��������Ϣ�־û���
* ��ǰ�汾��1.0
* ��    �ߣ�������
* ������ڣ�2011��7��21��
*
* �޸ļ�¼1��
*    �޸����ڣ�2011/7/21
*    �� �� �ţ�V1.0
*    �� �� �ˣ�������
*    �޸����ݣ�����
******************************************************************************/
#include "user_pool.h"
#include "authmanager.h"
#include <pwd.h>

namespace zte_tecs
{
UserPool *UserPool::instance = NULL;
string UserPool::default_user = "admin";

/* -------------------------------------------------------------------------- */

int UserPool::InitCallback(void *nil, SqlColumn * columns)
{
    if ( NULL == columns  || 
         2    != columns->get_column_num()  )    
    {        
        return -1;    
    }   

    int64 oid;
    string name;   

    oid  = columns->GetValueInt64(0);     
    name = columns->GetValueText(1);     

    _known_users.insert(make_pair(name, oid));

    return 0;
}

/* -------------------------------------------------------------------------- */

UserPool::UserPool(SqlDB * db)
   : PoolSQL(db,User::k_table)
{
}

/* -------------------------------------------------------------------------- */
STATUS UserPool::Init()
{
    ostringstream   sql;

    SetCallback(static_cast<Callbackable::Callback>(&UserPool::InitCallback));

    sql  << "SELECT oid,name FROM " <<  User::k_table;

    _db->Execute(sql, this);

    UnsetCallback();

    if ((int) _known_users.size() == 0)
    {
        // User oneadmin needs to be added in the bootstrap
        int64    one_uid  = -1;
        string   one_name = default_user;
        string   sha1_pass = default_user; 
        string   error_str;
        
        Allocate(&one_uid, one_name, sha1_pass, true, 1, 1, error_str);

        if (one_uid == -1)
        {
            OutPut(SYS_ERROR, error_str.c_str());
        }
    }

    return SUCCESS;
}

/* -------------------------------------------------------------------------- */

int64 UserPool::Allocate (
    int64 *   oid,
    string  username,
    string  password,
    bool    enabled,
    int     role,
    int     groupid,    
    string& error_str)
{
    User *        user;

    // Build a new User object
    user = new User(-1,
        username,
        Sha1Digest(username, password),
        enabled);

    user->set_role(role);
    user->set_group(groupid);

    // Insert the Object in the pool
    *oid = PoolSQL::Allocate(user, error_str);

    if (*oid != -1)
    {
        // Add the user to the map of known_users
        _known_users.insert(make_pair(username,*oid));
    }

    return *oid;
}

/* -------------------------------------------------------------------------- */
int64 UserPool::ExternAuthenticate(const string& session)
{
    /* ��������Ҫ��apisvr������������̵��ã���Щ�������봴��UserPool���ʵ�� */
    string username,secret;
    int64 uid = -1;
    if (0 != User::SplitSecret(session, username, secret))
    {
        return -1;
    }

    AuthRequest ar(uid);
    ar.AddAuthenticate(username,"-", secret);

    AuthManager *authm = AuthManager::GetInstance();
    //��֪û�м�������������������Ϣȥ�ⲿ����ִ����֤
    authm->Authenticate(&ar);

    if (ar.get_result() == false)
    {
        OutPut(SYS_DEBUG, "Extern Auth Error: %s\n", ar.get_message().c_str());
        return -1;
    }
    
    stringstream uid_str;
    uid_str << ar.get_message();
    uid_str >> uid;
    if(uid == -1)
    {
        //��֤�Ѿ��ɹ��ˣ������Ӧ����-1
        OutPut(SYS_ERROR, "Extern Auth Error: %s\n", ar.get_message().c_str());
        SkyAssert(false);
    }
    return uid;
}

/* -------------------------------------------------------------------------- */

string UserPool::ExternGetSecretBySession(const string& session)
{
    string secret = "";
    
    MessageUnit temp_mu(TempUnitName(AUTHMANAGER));
    
    temp_mu.Register();  
    
    MID target(PROC_APISVR, DRIVERMANAGER);
    MessageOption option(target, EV_SESSION, 0, 0);  

    SessionMessage req(session, "");
    temp_mu.Send(req ,option);

    MessageFrame frame;
    STATUS ret = temp_mu.Wait(&frame, 3000);

    SessionMessage ack;

    if(ret == SUCCESS)
    {
        ack.deserialize(frame.data);
        secret = ack.get_secret();
    }

    return secret;   
}

/* -------------------------------------------------------------------------- */
int64 UserPool::Authenticate(const string& session)
{
    map<string, int64>::iterator index;

    User * user = NULL;
    string username;
    string secret, u_pass;
    int    uid;

    int64 user_id = -1;
    int rc;

    rc = User::SplitSecret(session, username, secret);

    if ( rc != 0 )
    {
        return -1;
    }

    index = _known_users.find(username);

    if ( index != _known_users.end() ) //User known
    {
        user = Get((int)index->second, true);

        if ( user != NULL )
        {
            u_pass = user->get_password();
            uid    = user->get_uid();
            Put(user, true);            
        }
    }
    else //External User
    {
        u_pass = "-";
        uid    = -1;
    }

    AuthRequest ar(uid);
    ar.AddAuthenticate(username, u_pass, secret);

    AuthManager *authm = AuthManager::GetInstance();
    if (authm->is_local()) //plain auth
    {
        //��������̼������û���֤��������ʹ�ñ�����֤��ʽ
        //������֤ģʽ�ܼ򵥣�ֻҪ����ƥ�䲢���˻�ʹ�ܾ�ͨ��
        if (user != NULL && ar.PlainAuthenticate()) //no plain for external users
        {
            if(user->is_enabled())
            {
                user_id = uid;
            }
        }
    }
    else //use the driver
    {
        //���û�м�������������Ҫ������Ϣȥ����ִ����֤
        authm->Authenticate(&ar);

        if (ar.get_result() == true)
        {
            stringstream uid_str;
            uid_str << ar.get_message();
            uid_str >> user_id;
        }
        else
        {
            user_id = -1;
        }

        if (user_id == -1)
        {
            OutPut(SYS_DEBUG, "Auth Error: %s\n", ar.get_message().c_str());
        }
    }

    return user_id;
}

/* -------------------------------------------------------------------------- */

int UserPool::Authorize(AuthRequest& ar)
{
    AuthManager *authm = AuthManager::GetInstance();
    int           rc    = -1;

    if (authm->is_local())
    {
        if (ar.PlainAuthorize())
        {
            rc = 0;
        }
    }
    else
    {
        authm->Authorize(&ar);

        if (ar.get_result() == true)
        {
            rc = 0;
        }
        else
        {
            OutPut(SYS_DEBUG, "Auth Error: %s\n", ar.get_message().c_str());
        }
    }

    return rc;
}

/* -------------------------------------------------------------------------- */

int UserPool::DumpCallback(void * oss, SqlColumn * columns)
{
    ostringstream * os;

    os = static_cast<ostringstream *>(oss);

    int64 oid;
    UserInfo info;

    oid  = columns->GetValueInt64(User::OID);
    info.username = columns->GetValueText(User::USERNAME);
    info.password = columns->GetValueText(User::PASSWORD);
    info.enabled = (columns->GetValueInt(User::ENABLED)==0)?false:true;
    info.role = columns->GetValueInt(User::ROLE);
    info.group_id = columns->GetValueInt64(User::GROUPID);
    info.email = columns->GetValueText(User::EMAIL);
    info.phone = columns->GetValueText(User::PHONE);
    info.location = columns->GetValueText(User::LOCATION);
    info.description = columns->GetValueText(User::DESCRIPTION);
    info.create_time = columns->GetValueText(User::CREATE_TIME);
    info.max_session = columns->GetValueInt(User::MAX_SESSION);

    _dump_users.insert(make_pair(oid, info));

    return User::Dump(*os, columns);
}

/* -------------------------------------------------------------------------- */

int UserPool::Dump(ostringstream& oss, const string& where)
{
    int             rc;
    ostringstream   cmd;

    _dump_users.clear();

    oss << "<USER_POOL>";

    SetCallback(static_cast<Callbackable::Callback>(&UserPool::DumpCallback),
                 static_cast<void *>(&oss));

    cmd << "SELECT " << User::k_db_names << " FROM " << User::k_table;

    if ( !where.empty() )
    {
        cmd << " WHERE " << where;
    }

    rc = _db->Execute(cmd, this);

    UnsetCallback();

    oss << "</USER_POOL>";

    return rc;
}

/* -------------------------------------------------------------------------- */

}
