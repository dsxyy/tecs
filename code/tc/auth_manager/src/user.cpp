/******************************************************************************
* Copyright (c) 2011，深圳市中兴通讯股份有限公司
* All rights reserved.
* 
* 文件名称：user.cpp
* 文件标识：见配置管理计划书
* 内容摘要：用户组对象信息持久化类
* 当前版本：1.0
* 作    者：王明辉
* 完成日期：2011年7月21日
*
* 修改记录1：
*    修改日期：2011/7/21
*    版 本 号：V1.0
*    修 改 人：王明辉
*    修改内容：创建
******************************************************************************/
#include "user.h"
#include "authmanager.h"
#include "log_agent.h"
#include "db_config.h"

namespace zte_tecs
{

/* ************************************************************************** */
/* User :: Constructor/Destructor                                             */
/* ************************************************************************** */

User::User(
    int64   id,
    string  username,
    string  password,
    bool    enabled):
        PoolObjectSQL(id),
        _username     (username),
        _password     (password),
        _enabled      (enabled),
        _max_session  (1)
        {};


User::~User(){};

/* ************************************************************************** */
/* User :: Database Access Functions                                          */
/* ************************************************************************** */

const char * User::k_table = "user_pool";

const char * User::k_db_names = "oid, name, description, create_time, enabled, role, group_id,  password, email, phone, location, max_session";

const char * User::k_table_userdel = "deleted_user_pool";

/* -------------------------------------------------------------------------- */

int User::SelectCallback(void *nil, SqlColumn * columns)
{
    if ( columns->get_column_num() != LIMIT )
    {
        return -1;
    }

    int tmp = 0;
    columns->GetValue(OID, _oid);
    columns->GetValue(USERNAME, _username);
    columns->GetValue(DESCRIPTION, _description);
    columns->GetValue(CREATE_TIME, _create_time);
    columns->GetValue(PASSWORD, _password);
    columns->GetValue(ENABLED, tmp); _enabled  =  (tmp==0)?false:true;
    columns->GetValue(ROLE, _role);
    columns->GetValue(GROUPID, _group_id);
    columns->GetValue(EMAIL, _email);
    columns->GetValue(PHONE, _phone);
    columns->GetValue(LOCATION, _location);
    columns->GetValue(MAX_SESSION, _max_session);
   
    return 0;
}

/* -------------------------------------------------------------------------- */

int User::Select(SqlDB *db)
{
    ostringstream   oss;
    int             rc;
    int64           boid;

    SetCallback(static_cast<Callbackable::Callback>(&User::SelectCallback));

    oss << "SELECT " << k_db_names << " FROM " << k_table << " WHERE oid = " << _oid;

    boid = _oid;
    _oid  = -1;

    rc = db->Execute(oss, this);

    UnsetCallback();

    if ((rc != 0) || (_oid != boid ))
    {
        return -1;
    }

    return 0;
}

/* -------------------------------------------------------------------------- */

int User::Insert(SqlDB *db, string& error_str)
{
    int rc;

    _create_time.refresh();

    rc = insert_replace(db, false);

    if ( rc != 0 )
    {
        error_str = "Error inserting User in DB.";
        return rc;
    }

    return 0;
}

/* -------------------------------------------------------------------------- */

int User::Update(SqlDB *db)
{
    int rc;

    rc = insert_replace(db, true);

    if ( rc != 0 )
    {
        return rc;
    }

    return 0;
}

/* -------------------------------------------------------------------------- */

int User::insert_replace(SqlDB *db, bool replace)
{
    ostringstream   oss;

    int    rc;
    int   int_enabled = _enabled ? 1 : 0;

    SqlCommand sql(db,  k_table);
    sql.Add("oid", _oid);
    sql.Add("name", _username);
    sql.Add("description", _description);
    sql.Add("create_time", _create_time);
    sql.Add("password", _password);
    sql.Add("enabled",  int_enabled);
    sql.Add("role",     _role);
    sql.Add("group_id", _group_id);
    sql.Add("email",    _email);
    sql.Add("phone",    _phone);
    sql.Add("location", _location);
    sql.Add("max_session", _max_session);

    if(replace)
    {
       return sql.Update("WHERE oid = " + to_string<int64>(_oid, dec));
    }
    else
    {
       return sql.Insert();
    }    

    return rc;

}

/* -------------------------------------------------------------------------- */

int User::Dump(ostringstream& oss, SqlColumn * columns)
{
    if ( columns->get_column_num() != LIMIT )
    {
        return -1;
    }

    oss <<
        "<USER>"
             "<ID>"           << columns->GetValueInt64(OID)         <<"</ID>"           <<
             "<NAME>"         << columns->GetValueText(USERNAME)     <<"</NAME>"         <<
             "<DESCRIPTION>"  << columns->GetValueText(DESCRIPTION)  <<"</DESCRIPTION>"  <<
             "<CREATE_TIME>"  << columns->GetValueText(CREATE_TIME)  <<"</CREATE_TIME>"  <<
             "<ENABLED>"      << columns->GetValueInt(ENABLED)       <<"</ENABLED>"      <<
             "<ROLE>"         << columns->GetValueInt(ROLE)          <<"</ROLE>"         <<
             "<GROUPID>"      << columns->GetValueInt64(GROUPID)     <<"</GROUPID>"      <<
             "<EMAIL>"        << columns->GetValueText(EMAIL)        <<"</EMAIL>"        <<
             "<PHONE>"        << columns->GetValueText(PHONE)        <<"</PHONE>"        <<
             "<LOCATION>"     << columns->GetValueText(LOCATION)     <<"</LOCATION>"     <<
             "<MAX_SESSION>"  << columns->GetValueText(MAX_SESSION)  <<"</MAX_SESSION>"  <<             
        "</USER>";

    return 0;
}

/* -------------------------------------------------------------------------- */

int User::Drop(SqlDB * db)
{
    ostringstream oss;
    ostringstream move;
    int rc;

    move << "INSERT INTO " << k_table_userdel 
         << " SELECT * FROM " << k_table << " WHERE oid=" << _oid;

    rc = db->Execute(move);
    
    if ( rc != 0 )
    {
        OutPut(SYS_ALERT, "[%s] Execute failed!", move.str().c_str());
    }

    oss << "DELETE FROM " << k_table << " WHERE oid=" << _oid;

    rc = db->Execute(oss);

    if ( rc == 0 )
    {
        set_valid(false);
    }
    else
    {
        //如果用户删除失败,则已删除用户表中的数据要清除
        //不使用事务是因为在这个地方开启事务可能会造成内存泄露
        ostringstream rollback;
        rollback << "DELETE FROM " << k_table_userdel << " WHERE oid=" << _oid;
        db->Execute(rollback);
    }    

    return rc;
}

/* -------------------------------------------------------------------------- */

bool User::is_enabled() const
{
   UsergroupPool* gpool    = NULL;  
   bool  group_enabled     =  false;
   
   gpool = UsergroupPool::GetInstance();

   if(gpool != NULL)
   {
       UserGroup ug;
       int rc = gpool->Get(_group_id, &ug);
       if(rc == 0)
       {
           group_enabled = ug.is_enabled();
       }       
   }
   
   return _enabled && group_enabled;
}


/* ************************************************************************** */
/* User :: Misc                                                               */
/* ************************************************************************** */

ostream& operator<<(ostream& os, User& user)
{
    string user_str;

    os << user.to_xml(user_str);

    return os;
};

/* -------------------------------------------------------------------------- */

string& User::to_xml(string& xml) const
{
    ostringstream   oss;

    int  enabled_int = _enabled?1:0;

    oss <<
    "<USER>"
         "<ID>"           << _oid            <<"</ID>"        <<
         "<NAME>"         << _username       <<"</NAME>"      <<
         "<DESCRIPTION>"     << _description     <<"</DESCRIPTION>"  <<
         "<CREATE_TIME>"     << _create_time     <<"</CREATE_TIME>"  <<
         "<ENABLED>"      << enabled_int     <<"</ENABLED>"   <<
         "<ROLE>"         << _role           <<"</ROLE>"      <<
         "<GROUPID>"      << _group_id       <<"</GROUPID>"   <<
         "<EMAIL>"        << _email          <<"</EMAIL>"     <<
         "<PHONE>"        << _phone          <<"</PHONE>"     <<
         "<LOCATION>"     << _location       <<"</LOCATION>"  <<
         "<MAX_SESSION>"  << _max_session    <<"</MAX_SESSION>"  <<             
    "</USER>";

    xml = oss.str();

    return xml;
}

/* -------------------------------------------------------------------------- */

string& User::to_str(string& str) const
{
    ostringstream   os;

    string enabled_str = _enabled?"True":"False";

    os <<
        "ID = "        << _oid        << endl <<
        "NAME = "      << _username   << endl <<
        "DESCRIPTION = "  << _description   << endl <<
        "CREATE_TIME = "  << _create_time   << endl <<
        "ENABLED = "   << enabled_str << endl << 
        "ROLE = "      << _role       << endl <<
        "GROUPID ="    << _group_id   << endl <<
        "EMAIL ="      << _email      << endl <<
        "PHONE ="      << _phone      << endl <<
        "LOCATION ="   << _location   << endl <<
        "MAX_SESSION ="   << _max_session   << endl ;
        
    str = os.str();

    return str;
}

/* -------------------------------------------------------------------------- */

int User::SplitSecret(const string secret, string& user, string& pass)
{
    size_t pos;
    int    rc = -1;

    pos=secret.find(":");

    if (pos != string::npos)
    {
        user = secret.substr(0,pos);
        pass = secret.substr(pos+1);

        rc = 0;
    }

    return rc;
}

/* -------------------------------------------------------------------------- */

}
