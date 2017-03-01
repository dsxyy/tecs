/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：UserAllocate.cpp
* 文件标识：见配置管理计划书
* 内容摘要：UserAllocate类的实现文件
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
#include "userapi.h"
#include "authmanager.h"
#include "authrequest.h"
#include "api_error.h"
#include "vmcfg_pool.h"
#include "db_config.h"
#include "sessionmanager.h"
#include "license_common.h"


namespace zte_tecs
{

#define USER_ENABLE         1
#define USER_DISABLE        2
#define USERGROUP_ENABLE    1
#define USERGROUP_DISABLE   2


/******************************************************************************
* UserAllocate 类的实现
*******************************************************************************/
UserAllocateMethod::UserAllocateMethod(UserPool *upool)
{
     //_signature = "add user";
     _help = "Creates a new user";
     _method_name = "tecs.user.allocate";
     _user_id = 0;
     _upool = upool;
}

/* -------------------------------------------------------------------------- */

UserAllocateMethod::~UserAllocateMethod()
{
}

/* -------------------------------------------------------------------------- */

void UserAllocateMethod::MethodEntry(const xmlrpc_c::paramList& paramList,  xmlrpc_c::value* const retval)
{
    string username;
    string password;
    string confirm_pass;
    string groupname;
    int    role = 0;
    int    groupid = 0;

    string error_str;

    int64  uid = -1;
    int    rc = -1;
    int    error_code = TECS_ERROR;
    ostringstream  oss;
    UserGroup ug;
    User* user    = NULL;
    User* op_user = _upool->Get(_user_id, false);
    UsergroupPool* gpool = UsergroupPool::GetInstance();

    /*   -- RPC specific vars --  */
    vector<xmlrpc_c::value> arrayData;
    xmlrpc_c::value_array * arrayresult;

    // Get the parameters
    username = xmlrpc_c::value_string(paramList.getString(1));
    password = xmlrpc_c::value_string(paramList.getString(2));
    confirm_pass = xmlrpc_c::value_string(paramList.getString(3));
    role = xmlrpc_c::value_int(paramList.getInt(4));
    groupname = xmlrpc_c::value_string(paramList.getString(5));

    AuthRequest ar(_user_id);

    if(ValidateLicense(LICOBJ_USERS, 1) != 0)
    {
        error_code = TECS_ERR_LICENSE_INVALID;
        goto error_common;
    }

    if(!StringCheck::CheckExtendedName(username, 1, STR_LEN_64))
    {
        error_code = TECS_ERR_PARA_INVALID;
        oss << "Invalid parameter " << username <<".";
        goto error_common;
    }

    if(!StringCheck::CheckSize(password, 0, STR_LEN_32))
    {
        oss << "Invalid parameter " << password <<".";
        error_code = TECS_ERR_PARA_INVALID;
        goto error_common;
    }

    if(StringCheck::CheckIsAllSpace(password))
    {
        oss << "password can not be all space.";
        error_code = TECS_ERR_PARA_INVALID;
        goto error_common;
    }

    if(password != confirm_pass)
    {
        error_code = TECS_ERR_PASSWORD_NOT_SAME;
        oss << "Invalid password, two input must be consistent.";
        goto error_common;
    }

    if(!StringCheck::CheckExtendedName(groupname, 1, STR_LEN_64))
    {
        error_code = TECS_ERR_PARA_INVALID;
        oss << "Invalid parameter " << groupname <<".";
        goto error_common;
    }

    ar.AddAuthorize(AuthRequest::USER,
                0,
                AuthRequest::CREATE,
                0,
                false);

    if (_upool->Authorize(ar) == -1 || op_user == NULL)
    {
        error_code = TECS_ERR_AUTHORIZE_FAILED;
        oss << AuthorizeError(get_method_name(), "USER", "CREATE", _user_id, -1);
        goto error_common;
    }

    // Let's make sure that the user doesn't exist in the database
    user = _upool->Get(username,false);

    if (user != NULL)
    {
        error_code = TECS_ERR_USER_DUPLICATE;
        oss << ActionError(get_method_name(), "USER", "CREATE", -2, -1)
            << ". Reason: Existing user, cannot duplicate.";
        goto error_common;

    }

    rc = gpool->Get(groupname, &ug);

    if(rc != 0)
    {
        error_code = TECS_ERR_USERGROUP_NOT_EXIST;
        oss << "groupname does not exist.";
        goto error_common;
    }

    groupid = ug.get_gid();

    if(    op_user->get_role() == UserGroup::ACCOUNT_ADMINISTRATOR
        && op_user->get_group() != groupid )
    {
        error_code = TECS_ERR_AUTHORIZE_FAILED;
        oss << "account_admin can't add other group user.";
        goto error_common;
    }

    // Now let's add the user
    rc = _upool->Allocate(&uid, username, password, true, role, groupid, error_str);

    if (rc == -1)
    {
        error_code = TECS_ERR_OBJECT_ALLOCATE_FAILED;
        oss << ActionError(get_method_name(), "USER", "CREATE", -2, 0);
        oss << " " << error_str;  //error_str from SQLDB module
        goto error_common;

    }

    // All nice, return result to client
    arrayData.push_back(xmlrpc_c::value_int(TECS_SUCCESS));

    // Copy arrayresult into retval mem space
    arrayresult = new xmlrpc_c::value_array(arrayData);
    *retval = *arrayresult;

    delete arrayresult;

    return;

error_common:
    arrayData.push_back(xmlrpc_c::value_int(error_code));
    arrayData.push_back(xmlrpc_c::value_string(oss.str()));

    cout << oss.str() << endl;

    //Creat return message
    xmlrpc_c::value_array arrayresult_error(arrayData);

    *retval = arrayresult_error;

    return;
}

/******************************************************************************
* UserDelete 类的实现
*******************************************************************************/
UserDeleteMethod::UserDeleteMethod(UserPool *upool)
{
     //_signature = "deleteuser";
     _help = "Deletes a user account";
     _method_name = "tecs.user.delete";
     _user_id = 0;
     _upool = upool;
}

/* -------------------------------------------------------------------------- */

UserDeleteMethod::~UserDeleteMethod()
{
}

/* -------------------------------------------------------------------------- */
#define USER_DEL_NO_USED_IMAGE     0
#define USER_DEL_SELECT_ERROR      1
#define USER_DEL_HAS_USED_IMAGE    2

/* 判断用户是否有镜像，如果有反馈给WEB:此用户有镜像，建议先删除镜像，然后才可操作销户。 */
int UserHasUsedImage(int64 uid)
{
    ostringstream where_image_query;    

    int image_id = 0; 
    
    where_image_query << "uid = " << uid << "";
    
    SqlCallbackable image_sqlcb(GetDB());  
    
    int rc = image_sqlcb.SelectColumn("image_all_info", "image_id", where_image_query.str(), image_id);    
    if(rc == SQLDB_ERROR)
    {
        return USER_DEL_SELECT_ERROR;
    }
    
    if(image_id > 0)
    {
        return USER_DEL_HAS_USED_IMAGE;
    }

    return USER_DEL_NO_USED_IMAGE;
}

/* -------------------------------------------------------------------------- */

void UserDeleteMethod::MethodEntry(const xmlrpc_c::paramList& paramList,  xmlrpc_c::value* const retval)
{
    string  username;
    int64   uid = 0;
    User*   user = NULL;
    int     rc = -1;
    int     error_code = TECS_ERROR;
    ostringstream oss;
    int     vm_count = 0;
    int     query_image_rc = -1;    
    ostringstream where_vm_query;
    VmCfgPool *pvp = NULL;

    /*   -- RPC specific vars --  */
    vector<xmlrpc_c::value> arrayData;
    xmlrpc_c::value_array * arrayresult;

    // Get the parameters
    username = xmlrpc_c::value_string(paramList.getString(1));
    AuthRequest ar(_user_id);

    if(!StringCheck::CheckExtendedName(username, 1, STR_LEN_64))
    {
        error_code = TECS_ERR_PARA_INVALID;
        oss << "Invalid parameter " << username <<".";
        goto error_common;
    }

    // Now let's get the user
    user = _upool->Get(username, true);

    if (user == NULL)
    {
        error_code = TECS_ERR_USER_NOT_EXIST;
        oss << GetObjectError(get_method_name(), username, uid);
        goto error_common;

    }

    uid = user->get_uid();

    ar.AddAuthorize(AuthRequest::USER,
                0,
                AuthRequest::DELETE,
                uid,
                false);

    if(uid == 1)
    {
        user->UnLock();

        error_code = TECS_ERR_CANNOT_DEL_OR_DISABLE_DEFAULT;
        oss << "Error, the default user cannot be disabled or deleted.";
        goto error_common;
    }

    if(_user_id == uid)
    {
        user->UnLock();

        error_code = TECS_ERR_CANNOT_DEL_OR_DISABLE_SELF;
        oss << "Error, user cannot disable or delete his own account.";
        goto error_common;
    }

    if (_upool->Authorize(ar) == -1)
    {
        user->UnLock();

        error_code = TECS_ERR_AUTHORIZE_FAILED;
        oss << AuthorizeError(get_method_name(), "USER", "DELETE", _user_id, uid);
        goto error_common;

    }

    where_vm_query << " deployed_cluster IS NOT NULL AND user_name = '" << username << "'";

    pvp = VmCfgPool::GetInstance( );
    if (!pvp)
    {
        error_code = TECS_ERROR;
        oss << "Get Database instance failed.";
        goto error_common;
    }
    vm_count = pvp->GetVmCfgCount(where_vm_query.str());
    if(vm_count > 0)
    {
        user->UnLock();

        error_code = TECS_ERR_USER_HAS_RUNNING_VM;
        oss << "Error, the user has deployed vm.";
        goto error_common;
    }


    query_image_rc = UserHasUsedImage(uid);
    if(query_image_rc != USER_DEL_NO_USED_IMAGE)
    {
        user->UnLock();

        if(query_image_rc == USER_DEL_SELECT_ERROR)
        {
            error_code = TECS_ERR_DB_QUERY_FAILED;
        }
        else
        {
            error_code = TECS_ERR_USER_HAS_USED_IAMGE;
            oss << "Error, the user has image.";
        }

        goto error_common;
    }

    rc = _upool->Drop(user);

    user->UnLock();

    if (rc != 0)
    {
        error_code = TECS_ERR_OBJECT_DROP_FAILED;
        oss << ActionError(get_method_name(), username, "DELETE", uid, rc);
        goto error_common;

    }

    // All nice, return success
    arrayData.push_back(xmlrpc_c::value_int(TECS_SUCCESS));

    // Copy arrayresult into retval mem space
    arrayresult = new xmlrpc_c::value_array(arrayData);
    *retval     = *arrayresult;

    delete arrayresult;

    return;

error_common:
    arrayData.push_back(xmlrpc_c::value_int(error_code));
    arrayData.push_back(xmlrpc_c::value_string(oss.str()));

    cout << oss.str() << endl;

    xmlrpc_c::value_array arrayresult_error(arrayData);

    *retval = arrayresult_error;

    return;

}

/******************************************************************************
* UserUpdateMethod 类的实现
*******************************************************************************/
UserUpdateMethod::UserUpdateMethod(UserPool *upool)
{
     //_signature = "updateuser";
     _help = "update user";
     _method_name = "tecs.user.set";
     _user_id = 0;
     _upool = upool;
}

/* -------------------------------------------------------------------------- */

UserUpdateMethod::~UserUpdateMethod()
{
}

/* -------------------------------------------------------------------------- */

void UserUpdateMethod::MethodEntry(const xmlrpc_c::paramList& paramList,  xmlrpc_c::value* const retval)
{
    string username;
    string password;
    string confirm_pass;
    int role    = 0;
    int groupid = 0;
    string groupname;
    int enable  = 0;
    string phone;
    string email;
    string location;
    string description;
    int max_session = 1;

    string error_str;
    int  rc = -1;
    int  error_code = TECS_ERROR;
    ostringstream  oss;
    UserGroup      ug;
    User*          user  = NULL;
    User*          op_user = _upool->Get(_user_id, false);
    UsergroupPool* gpool = UsergroupPool::GetInstance();

    /*   -- RPC specific vars --  */
    vector<xmlrpc_c::value> arrayData;
    xmlrpc_c::value_array * arrayresult;

    // Get the parameters
    username = xmlrpc_c::value_string(paramList.getString(1));
    password = xmlrpc_c::value_string(paramList.getString(2));
    confirm_pass = xmlrpc_c::value_string(paramList.getString(3));
    role = xmlrpc_c::value_int(paramList.getInt(4));
    groupname = xmlrpc_c::value_string(paramList.getString(5));
    enable = xmlrpc_c::value_int(paramList.getInt(6));
    max_session = xmlrpc_c::value_int(paramList.getInt(7));    
    phone  = xmlrpc_c::value_string(paramList.getString(8));
    email  = xmlrpc_c::value_string(paramList.getString(9));
    location  = xmlrpc_c::value_string(paramList.getString(10));
    description = xmlrpc_c::value_string(paramList.getString(11));

    AuthRequest ar(_user_id);

    if(!StringCheck::CheckExtendedName(username, 1, STR_LEN_64))
    {
        error_code = TECS_ERR_PARA_INVALID;
        oss << "Invalid parameter " << username <<".";
        goto error_common;
    }

    // Let's make sure that the user doesn't exist in the database
    user = _upool->Get(username, true);

    if (user == NULL)
    {
        error_code = TECS_ERR_USER_NOT_EXIST;
        oss << ActionError(get_method_name(), "USER", "UPDATE", -2, -1)
            << ". Reason: Not Existing user";
        goto error_common;

    }

    ar.AddAuthorize(AuthRequest::USER,
                0,
                AuthRequest::MANAGE,
                user->get_uid(),
                false);

    if (_upool->Authorize(ar) == -1 || op_user == NULL)
    {
        user->UnLock();

        error_code = TECS_ERR_AUTHORIZE_FAILED;
        oss << AuthorizeError(get_method_name(), "USER", "UPDATE", _user_id, -1);
        goto error_common;

    }

    if(!password.empty())
    {
        if(!StringCheck::CheckSize(password, 0, STR_LEN_32))
        {
            user->UnLock();

            oss << "Invalid parameter " << password <<".";
            error_code = TECS_ERR_PARA_INVALID;
            goto error_common;
        }

        if(StringCheck::CheckIsAllSpace(password))
        {
            user->UnLock();
        
            oss << "password can not be all space.";
            error_code = TECS_ERR_PARA_INVALID;
            goto error_common;
        }

        if(password != confirm_pass)
        {
            user->UnLock();

            error_code = TECS_ERR_PASSWORD_NOT_SAME;
            oss << "Invalid password, two input must be consistent.";
            goto error_common;
        }

        user->set_password(password);
    }

    if(   role == UserGroup::CLOUD_ADMINISTRATOR
       || role == UserGroup::ACCOUNT_ADMINISTRATOR
       || role == UserGroup::ACCOUNT_USER  )
    {
        if(user->get_uid() == 1 && role != UserGroup::CLOUD_ADMINISTRATOR)
        {
            user->UnLock();

            error_code = TECS_ERR_DEFAULT_MUST_BE_CLOUDADMIN;
            oss << "Error, the default user must be a cloud administrator.";
            goto error_common;
        }

        if(   role < op_user->get_role()    /*  */
           || user->get_role() < op_user->get_role())
        {
            user->UnLock();

            error_code = TECS_ERR_AUTHORIZE_FAILED;
            oss << "Error, the current user does not have permission.";
            goto error_common;
        }

        user->set_role(role);
    }

    if(!groupname.empty())
    {
        if(!StringCheck::CheckExtendedName(groupname, 1, STR_LEN_64))
        {
            user->UnLock();

            oss << "Invalid parameter " << groupname <<".";
            error_code = TECS_ERR_PARA_INVALID;
            goto error_common;
        }

        rc = gpool->Get(groupname, &ug);

        if(rc != 0)
        {
            user->UnLock();

            error_code = TECS_ERR_USERGROUP_NOT_EXIST;
            oss << "groupname does not exist.";
            goto error_common;
        }

        groupid = ug.get_gid();
        user->set_group(groupid);
    }

    if(enable == USER_ENABLE)
    {
        user->enable();
    }
    else if(enable == USER_DISABLE)
    {
        if(user->get_uid() == 1)
        {
            user->UnLock();

            error_code = TECS_ERR_CANNOT_DEL_OR_DISABLE_DEFAULT;
            oss << "Error, the default user cannot be disabled or deleted.";
            goto error_common;
        }

        if(_user_id == user->get_uid())
        {
            user->UnLock();

            error_code = TECS_ERR_CANNOT_DEL_OR_DISABLE_SELF;
            oss << "Error, user cannot disable or delete his own account.";
            goto error_common;
        }

        user->disable();
    }

    if(!phone.empty() || !email.empty() || !location.empty())
    {
        if(_user_id != user->get_uid())
        {
            user->UnLock();

            error_code = TECS_ERR_AUTHORIZE_FAILED;
            oss << "Error, Not modify the information of others.";
            goto error_common;
        }
    }

    if(!phone.empty())
        user->set_phone(phone);
    if(!email.empty())
        user->set_email(email);
    if(!location.empty())
        user->set_location(location);
    if(!description.empty())
        user->set_description(description);

    if(max_session < 1 || max_session > 999)
    {
        error_code = TECS_ERR_PARA_INVALID;
        oss << "Invalid parameter max_session, must between 1~999.";
        goto error_common;
    }
    user->set_max_session(max_session);
        

    rc = _upool->Update(user);

    user->UnLock();

    if (rc != 0)
    {
        error_code = TECS_ERR_OBJECT_UPDATE_FAILED;
        oss << ActionError(get_method_name(), "USER", "UPDATE", -2, 0);
        oss << " " << error_str;  //error_str from SQLDB module
        goto error_common;

    }

    arrayData.push_back(xmlrpc_c::value_int(TECS_SUCCESS));

    // Copy arrayresult into retval mem space
    arrayresult = new xmlrpc_c::value_array(arrayData);
    *retval = *arrayresult;

    delete arrayresult;

    return;

error_common:
    arrayData.push_back(xmlrpc_c::value_int(error_code));
    arrayData.push_back(xmlrpc_c::value_string(oss.str()));

    cout << oss.str() << endl;

    //Creat return message
    xmlrpc_c::value_array arrayresult_error(arrayData);

    *retval = arrayresult_error;

    return;
}

/******************************************************************************
* UserInfoMethod 类的实现
*******************************************************************************/
UserInfoMethod::UserInfoMethod(UserPool *upool)
{
     //_signature = "userinfo";
     _help = "query userinfo";
     _method_name = "tecs.user.query";
     _user_id = 0;
     _upool = upool;
}

/* -------------------------------------------------------------------------- */

UserInfoMethod::~UserInfoMethod()
{
}

/* -------------------------------------------------------------------------- */

void UserInfoMethod::MethodEntry(const xmlrpc_c::paramList& paramList,  xmlrpc_c::value* const retval)
{
    string username;
    int uid = 0;
    string error_str;
    ostringstream  oss;
    int rc = -1;
    int  error_code = TECS_ERROR;
    int64 start_index, query_count;
    int64 next_index = -1, max_count = 0;
    ostringstream where;

    User* user    = NULL;
    User* op_user = _upool->Get(_user_id, false);
    UsergroupPool* gpool = UsergroupPool::GetInstance();

    /*   -- RPC specific vars --  */
    vector<xmlrpc_c::value> arrayData;
    vector<xmlrpc_c::value> arrayUser;
    xmlrpc_c::value_array * arrayresult;

    // Get the parameters
    start_index = paramList.getI8(1);
    query_count = paramList.getI8(2);
    username = paramList.getString(3);

    AuthRequest ar(_user_id);

    if(username.empty())
    {
        ar.AddAuthorize(AuthRequest::USER,
                    0,
                    AuthRequest::INFO,
                    0,
                    false);

        if (_upool->Authorize(ar) == -1 || op_user == NULL)
        {
            error_code = TECS_ERR_AUTHORIZE_FAILED;
            oss << AuthorizeError(get_method_name(), "USER", "INFO", _user_id, -1);
            goto error_common;
        }

        if( op_user->get_role() == UserGroup::ACCOUNT_ADMINISTRATOR )
        {

            where << "group_id = " << op_user->get_group();

            rc = _upool->Dump(oss, where.str());

            if (rc == -1)
            {
                error_code = TECS_ERR_OBJECT_GET_FAILED;
                oss << GetObjectError(get_method_name(), "USER", -1);
                goto error_common;
            }

            max_count = _upool->get_dump_users().size();

            where << " AND ";
        }
        else
        {
            max_count = _upool->get_known_users().size();
        }

        where << " 1 = 1 " <<
                 " order by oid limit " << query_count <<
                 " offset " << start_index;

        // Now let's get the info
        rc = _upool->Dump(oss, where.str());

        if (rc == -1)
        {
            error_code = TECS_ERR_OBJECT_GET_FAILED;
            oss << GetObjectError(get_method_name(), "USER", -1);
            goto error_common;
        }

        map<int64, UserInfo> users = _upool->get_dump_users();

        map<int64, UserInfo>::iterator it = users.begin();

        if ( users.size() >= (uint64)query_count)
        {
           next_index = start_index + query_count;
        }
        else
        {
           next_index = -1;
        }

        for( ; it != users.end();  it++)
        {
            query_count-- ;
            if(query_count < 0)
            {
                break;
            }

            map<string, xmlrpc_c::value> oneUser;
            oneUser.insert(make_pair("username", xmlrpc_c::value_string(it->second.username)));
            oneUser.insert(make_pair("password", xmlrpc_c::value_string("")));
            if(it->second.enabled)
                oneUser.insert(make_pair("enabled", xmlrpc_c::value_int(USER_ENABLE)));
            else
                oneUser.insert(make_pair("enabled", xmlrpc_c::value_int(USER_DISABLE)));
            oneUser.insert(make_pair("role", xmlrpc_c::value_int(it->second.role)));

            UserGroup ug;
            rc = gpool->Get(it->second.group_id, &ug);
            if(rc != 0)
            {
                oneUser.insert(make_pair("group", xmlrpc_c::value_string("Unknown")));
            }
            else
            {
                oneUser.insert(make_pair("group", xmlrpc_c::value_string(ug.get_groupname())));
            }

            oneUser.insert(make_pair("phone", xmlrpc_c::value_string(it->second.phone)));
            oneUser.insert(make_pair("email", xmlrpc_c::value_string(it->second.email)));
            oneUser.insert(make_pair("address", xmlrpc_c::value_string(it->second.location)));
            oneUser.insert(make_pair("description", xmlrpc_c::value_string(it->second.description)));
            oneUser.insert(make_pair("create_time", xmlrpc_c::value_string(it->second.create_time)));
            oneUser.insert(make_pair("max_session", xmlrpc_c::value_int(it->second.max_session)));
            arrayUser.push_back(xmlrpc_c::value_struct(oneUser));
        }

        // All nice, return the pool info
        arrayData.push_back(xmlrpc_c::value_int(TECS_SUCCESS));
        arrayData.push_back(xmlrpc_c::value_i8(next_index));
        arrayData.push_back(xmlrpc_c::value_i8(max_count));
        arrayData.push_back(xmlrpc_c::value_array(arrayUser));

        arrayresult = new xmlrpc_c::value_array(arrayData);

        // Copy arrayresult into retval mem space
        *retval = *arrayresult;

        delete arrayresult;

        return;
    }
    else
    {
        if(!StringCheck::CheckExtendedName(username, 1, STR_LEN_64))
        {
            error_code = TECS_ERR_PARA_INVALID;
            oss << "Invalid parameter " << username <<".";
            goto error_common;
        }

        // Let's make sure that the user doesn't exist in the database
        user = _upool->Get(username, false);

        if (user == NULL)
        {
            error_code = TECS_ERR_USER_NOT_EXIST;
            oss << GetObjectError(get_method_name(), "USER", uid);
            goto error_common;

        }

        uid = user->get_uid();

        ar.AddAuthorize(AuthRequest::USER,
                    0,
                    AuthRequest::INFO,
                    uid,
                    false);

        if (_upool->Authorize(ar) == -1)
        {
            error_code = TECS_ERR_AUTHORIZE_FAILED;
            oss << AuthorizeError(get_method_name(), "USER", "INFO", _user_id, -1);
            goto error_common;

        }

        //oss << *user;

        map<string, xmlrpc_c::value> oneUser;
        oneUser.insert(make_pair("username", xmlrpc_c::value_string(user->get_username())));
        oneUser.insert(make_pair("password", xmlrpc_c::value_string(user->get_password())));
        if(user->is_enabled())
            oneUser.insert(make_pair("enabled", xmlrpc_c::value_int(1)));
        else
            oneUser.insert(make_pair("enabled", xmlrpc_c::value_int(2)));
        oneUser.insert(make_pair("role", xmlrpc_c::value_int(user->get_role())));

        UserGroup ug;
        rc = gpool->Get(user->get_group(), &ug);
        if(rc != 0)
            oneUser.insert(make_pair("group", xmlrpc_c::value_string("Unknown")));
        else
            oneUser.insert(make_pair("group", xmlrpc_c::value_string(ug.get_groupname())));

        oneUser.insert(make_pair("phone", xmlrpc_c::value_string(user->get_phone())));
        oneUser.insert(make_pair("email", xmlrpc_c::value_string(user->get_email())));
        oneUser.insert(make_pair("address", xmlrpc_c::value_string(user->get_location())));
        oneUser.insert(make_pair("description", xmlrpc_c::value_string(user->get_description())));
        oneUser.insert(make_pair("create_time", xmlrpc_c::value_string(user->get_create_time())));
        oneUser.insert(make_pair("max_session", xmlrpc_c::value_int(user->get_max_session())));        
        arrayUser.push_back(xmlrpc_c::value_struct(oneUser));

        // All nice, return the user info
        arrayData.push_back(xmlrpc_c::value_int(TECS_SUCCESS));
        arrayData.push_back(xmlrpc_c::value_i8(-1));
        arrayData.push_back(xmlrpc_c::value_i8(1));
        arrayData.push_back(xmlrpc_c::value_array(arrayUser));

        // Copy arrayresult into retval mem space
        arrayresult = new xmlrpc_c::value_array(arrayData);
        *retval     = *arrayresult;

        delete arrayresult;

        return;
    }

error_common:
    arrayData.push_back(xmlrpc_c::value_int(error_code));
    arrayData.push_back(xmlrpc_c::value_string(oss.str()));

    cout << oss.str() << endl;

    xmlrpc_c::value_array arrayresult_error(arrayData);

    *retval = arrayresult_error;

    return;
}



/******************************************************************************
* UserGroupAllocateMethod 类的实现
*******************************************************************************/
UserGroupAllocateMethod::UserGroupAllocateMethod(UserPool *upool)
{
     //_signature = "add usergroup";
     _help = "Creates a new usergroup";
     _method_name = "tecs.usergroup.allocate";
     _user_id = 0;
     _upool = upool;

}

/* -------------------------------------------------------------------------- */

UserGroupAllocateMethod::~UserGroupAllocateMethod()
{
}

/* -------------------------------------------------------------------------- */

void UserGroupAllocateMethod::MethodEntry(const xmlrpc_c::paramList& paramList,  xmlrpc_c::value* const retval)
{
    string groupname;
    int grouptype = 0;
    string error_str;
    int64  ugid = 0;
    int64  rc = -1;
    int  error_code = TECS_ERROR;
    ostringstream  oss;
    UserGroup  ug;
    UsergroupPool* gpool     = NULL;


    /*   -- RPC specific vars --  */
    vector<xmlrpc_c::value> arrayData;
    xmlrpc_c::value_array * arrayresult;

    // Get the parameters
    groupname = xmlrpc_c::value_string(paramList.getString(1));
    grouptype = xmlrpc_c::value_int(paramList.getInt(2));

    AuthRequest ar(_user_id);

    if(!StringCheck::CheckExtendedName(groupname, 1, STR_LEN_64))
    {
        error_code = TECS_ERR_PARA_INVALID;
        oss << "Invalid parameter " << groupname <<".";
        goto error_common;
    }

    ar.AddAuthorize(AuthRequest::USERGROUP,
                0,
                AuthRequest::CREATE,
                0,
                false);

    if (_upool->Authorize(ar) == -1)
    {
        error_code = TECS_ERR_AUTHORIZE_FAILED;
        oss << AuthorizeError(get_method_name(), "USERGROUP", "CREATE", rc, -1);
        goto error_common;

    }

    // Let's make sure that the user doesn't exist in the database
    gpool = UsergroupPool::GetInstance();

    rc = gpool->Get(groupname, &ug);

    if (rc == 0)
    {
        error_code = TECS_ERR_USERGROUP_DUPLICATE;
        oss << ActionError(get_method_name(), "USERGROUP", "CREATE", -2, -1)
            << ". Reason: Existing usergroup, cannot duplicate.";
        goto error_common;

    }

    // Now let's add the usergroup
    rc = gpool->Allocate(&ugid, groupname, grouptype, true, error_str);

    if (rc == -1)
    {
        error_code = TECS_ERR_OBJECT_ALLOCATE_FAILED;
        oss << ActionError(get_method_name(), "USERGROUP", "CREATE", -2, 0);
        oss << " " << error_str;  //error_str from SQLDB module
        goto error_common;

    }

    // All nice, return the new uid to client
     arrayData.push_back(xmlrpc_c::value_int(TECS_SUCCESS));

    // Copy arrayresult into retval mem space
    arrayresult = new xmlrpc_c::value_array(arrayData);
    *retval = *arrayresult;

    delete arrayresult;

    return;

error_common:
    arrayData.push_back(xmlrpc_c::value_int(error_code));
    arrayData.push_back(xmlrpc_c::value_string(oss.str()));

    cout << oss.str() << endl;

    // Creat return message
    xmlrpc_c::value_array arrayresult_error(arrayData);

    *retval = arrayresult_error;

    return;

}

/******************************************************************************
* UserGroupDeleteMethod 类的实现
*******************************************************************************/
UserGroupDeleteMethod::UserGroupDeleteMethod(UserPool *upool)
{
     //_signature = "deleteusergroup";
     _help = "Deletes a usergroup account";
     _method_name = "tecs.usergroup.delete";
     _user_id = 0;
     _upool = upool;
}

/* -------------------------------------------------------------------------- */

UserGroupDeleteMethod::~UserGroupDeleteMethod()
{
}

/* -------------------------------------------------------------------------- */

void UserGroupDeleteMethod::MethodEntry(const xmlrpc_c::paramList& paramList,  xmlrpc_c::value* const retval)
{
    string        usergroupname;

    int  rc = -1;
    int  error_code = TECS_ERROR;
    ostringstream oss;
    ostringstream tmp, where;
    UserGroup     ug;
    UsergroupPool* gpool      = NULL;
    int           gid = 0;

    /*   -- RPC specific vars --  */
    vector<xmlrpc_c::value> arrayData;
    xmlrpc_c::value_array * arrayresult;

    // Get the parameters
    usergroupname = xmlrpc_c::value_string(paramList.getString(1));

    AuthRequest ar(_user_id);

    if(!StringCheck::CheckExtendedName(usergroupname, 1, STR_LEN_64))
    {
        error_code = TECS_ERR_PARA_INVALID;
        oss << "Invalid parameter " << usergroupname <<".";
        goto error_common;
    }

    ar.AddAuthorize(AuthRequest::USERGROUP,
                0,
                AuthRequest::DELETE,
                0,
                false);

    if (_upool->Authorize(ar) == -1)
    {
        error_code = TECS_ERR_AUTHORIZE_FAILED;
        oss << AuthorizeError(get_method_name(), "USERGROUP", "DELETE", rc, gid);
        goto error_common;

    }

    gpool = UsergroupPool::GetInstance();

    // Now let's get the usergroup
    rc = gpool->Get(usergroupname, &ug);

    if (rc != 0)
    {
        error_code = TECS_ERR_USERGROUP_NOT_EXIST;
        oss << GetObjectError(get_method_name(), usergroupname, gid);
        goto error_common;
    }

    gid = ug.get_gid();

    // Check group's user
    where << "group_id = " << gid;

    _upool->Dump(tmp, where.str());

    if(_upool->get_dump_users().size() > 0)
    {
        error_code = TECS_ERR_USERGROUP_HAS_USER;

        goto error_common;
    }

    // Delete it
    rc =  gpool->Drop(&ug);

    if (rc != 0)
    {
        error_code = TECS_ERR_OBJECT_DROP_FAILED;
        oss << ActionError(get_method_name(), "USERGROUP", "DELETE", gid, rc);
        goto error_common;

    }

    // All nice, return success
    arrayData.push_back(xmlrpc_c::value_int(TECS_SUCCESS));

    // Copy arrayresult into retval mem space
    arrayresult = new xmlrpc_c::value_array(arrayData);
    *retval     = *arrayresult;

    delete arrayresult;

    return;

error_common:
    arrayData.push_back(xmlrpc_c::value_int(error_code));
    arrayData.push_back(xmlrpc_c::value_string(oss.str()));

    cout << oss.str() << endl;

    xmlrpc_c::value_array arrayresult_error(arrayData);

    *retval = arrayresult_error;

    return;

}

/******************************************************************************
* UserGroupUpdateMethod 类的实现
*******************************************************************************/
UserGroupUpdateMethod::UserGroupUpdateMethod(UserPool *upool)
{
     //_signature = "update usergroup";
     _help = "Updates a user account";
     _method_name = "tecs.usergroup.set";
     _user_id = 0;
     _upool = upool;
}

/* -------------------------------------------------------------------------- */

UserGroupUpdateMethod::~UserGroupUpdateMethod()
{
}

/* -------------------------------------------------------------------------- */

void UserGroupUpdateMethod::MethodEntry(const xmlrpc_c::paramList& paramList,  xmlrpc_c::value* const retval)
{
    string groupname, description;
    int enable = 0;

    string error_str;
    int  rc = -1;
    int  error_code = TECS_ERROR;
    ostringstream  oss;
    ostringstream  tmp, where;
    UserGroup      ug;
    UsergroupPool* gpool     = NULL;


    /*   -- RPC specific vars --  */
    vector<xmlrpc_c::value> arrayData;
    xmlrpc_c::value_array * arrayresult;

    // Get the parameters
    groupname = xmlrpc_c::value_string(paramList.getString(1));
    enable = xmlrpc_c::value_int(paramList.getInt(2));
    description = xmlrpc_c::value_string(paramList.getString(3));

    AuthRequest ar(_user_id);

    if(!StringCheck::CheckExtendedName(groupname, 1, STR_LEN_64))
    {
        error_code = TECS_ERR_PARA_INVALID;
        oss << "Invalid parameter " << groupname <<".";
        goto error_common;
    }

    ar.AddAuthorize(AuthRequest::USERGROUP,
                0,
                AuthRequest::MANAGE,
                0,
                false);

    if (_upool->Authorize(ar) == -1)
    {
        error_code = TECS_ERR_AUTHORIZE_FAILED;
        oss << AuthorizeError(get_method_name(), "USERGROUP", "UPDATE", rc, -1);
        goto error_common;
    }

    // Let's make sure that the user doesn't exist in the database
    gpool = UsergroupPool::GetInstance();

    rc = gpool->Get(groupname, &ug);

    if (rc != 0)
    {
        error_code = TECS_ERR_USERGROUP_NOT_EXIST;
        oss << ActionError(get_method_name(), "USERGOUP", "UPDATE", -2, -1)
            << ". Reason: Not Existing usergroup";
        goto error_common;

    }

    if(enable == USERGROUP_ENABLE)
    {
        ug.enable();
    }
    else if(enable == USERGROUP_DISABLE)
    {
        ug.disable();
    }

    if(!description.empty())
    {
        ug.set_description(description);
    }

    rc = gpool->Update(&ug);


    if (rc != 0)
    {
        error_code = TECS_ERR_OBJECT_UPDATE_FAILED;
        oss << ActionError(get_method_name(), "USERGOUP", "UPDATE", -2, 0);
        oss << " " << error_str;  //error_str from SQLDB module
        goto error_common;

    }

    arrayData.push_back(xmlrpc_c::value_int(TECS_SUCCESS));

    // Copy arrayresult into retval mem space
    arrayresult = new xmlrpc_c::value_array(arrayData);
    *retval = *arrayresult;

    delete arrayresult;

    return;

error_common:
    arrayData.push_back(xmlrpc_c::value_int(error_code));
    arrayData.push_back(xmlrpc_c::value_string(oss.str()));

    cout << oss.str() << endl;

    // Creat return message
    xmlrpc_c::value_array arrayresult_error(arrayData);

    *retval = arrayresult_error;

    return;
}

/******************************************************************************
* UserGroupInfoMethod 类的实现
*******************************************************************************/
UserGroupInfoMethod::UserGroupInfoMethod(UserPool *upool)
{
     //_signature = "usergroupinfo";
     _help = "query usergroupinfo";
     _method_name = "tecs.usergroup.query";
     _user_id = 0;
     _upool = upool;
}

/* -------------------------------------------------------------------------- */

UserGroupInfoMethod::~UserGroupInfoMethod()
{
}

/* -------------------------------------------------------------------------- */

void UserGroupInfoMethod::MethodEntry(const xmlrpc_c::paramList& paramList,  xmlrpc_c::value* const retval)
{
    string groupname;
    int gid = 0;
    string error_str;
    ostringstream  oss;
    int rc = -1;
    int  error_code = TECS_ERROR;
    int64 start_index, query_count;
    int64 next_index = -1, max_count = 0;
    ostringstream where;
    UserGroup     ug;
    UsergroupPool* gpool     = NULL;


    /*   -- RPC specific vars --  */
    vector<xmlrpc_c::value> arrayData;
    vector<xmlrpc_c::value> arrayGroup;
    xmlrpc_c::value_array * arrayresult;

    // Get the parameters
    start_index = paramList.getI8(1);
    query_count = paramList.getI8(2);

    groupname = xmlrpc_c::value_string(paramList.getString(3));

    AuthRequest ar(_user_id);
    ar.AddAuthorize(AuthRequest::USERGROUP,
                0,
                AuthRequest::INFO,
                0,
                false);

    if(groupname.empty())
    {
        if (_upool->Authorize(ar) == -1)
        {
            error_code = TECS_ERR_AUTHORIZE_FAILED;
            oss << AuthorizeError(get_method_name(), "USERGROUP", "INFO", _user_id, -1);
            goto error_common;

        }

        // Now let's get the info
        gpool = UsergroupPool::GetInstance();

        max_count = gpool->GetGroupSize();
        where << " 1 = 1 " <<
                 " order by oid limit " << query_count <<
                 " offset " << start_index;

        rc = gpool->Dump(oss, where.str());

        if (rc == -1)
        {
            error_code = TECS_ERR_OBJECT_GET_FAILED;
            oss << GetObjectError(get_method_name(), "USERGROUP", -1);
            goto error_common;
        }

        map<int64, UsergroupInfo> groups = gpool->get_dump_groups();
        map<int64, UsergroupInfo>::iterator it = groups.begin();

        if ( groups.size() >= (uint64)query_count)
        {
           next_index = start_index + query_count;
        }
        else
        {
           next_index = -1;
        }

        for(; it != groups.end(); it++)
        {
            query_count--;
            if(query_count < 0)
            {
                break;
            }

            map<string, xmlrpc_c::value> oneGroup;
            oneGroup.insert(make_pair("groupname", xmlrpc_c::value_string(it->second.groupname)));
            oneGroup.insert(make_pair("grouptype", xmlrpc_c::value_int(it->second.grouptype)));
            oneGroup.insert(make_pair("description", xmlrpc_c::value_string(it->second.description)));
            if(it->second.enabled)
                oneGroup.insert(make_pair("enabled", xmlrpc_c::value_int(USERGROUP_ENABLE)));
            else
                oneGroup.insert(make_pair("enabled", xmlrpc_c::value_int(USERGROUP_DISABLE)));
            oneGroup.insert(make_pair("create_time", xmlrpc_c::value_string(it->second.create_time)));

            arrayGroup.push_back(xmlrpc_c::value_struct(oneGroup));
        }

        // All nice, return the pool info
        arrayData.push_back(xmlrpc_c::value_int(TECS_SUCCESS));
        arrayData.push_back(xmlrpc_c::value_i8(next_index));
        arrayData.push_back(xmlrpc_c::value_i8(max_count));
        arrayData.push_back(xmlrpc_c::value_array(arrayGroup));

        arrayresult = new xmlrpc_c::value_array(arrayData);

        // Copy arrayresult into retval mem space
        *retval = *arrayresult;

        delete arrayresult;

        return;
    }
    else
    {
        if(!StringCheck::CheckExtendedName(groupname, 1, STR_LEN_64))
        {
            error_code = TECS_ERR_PARA_INVALID;
            oss << "Invalid parameter " << groupname <<".";
            goto error_common;
        }

        if (_upool->Authorize(ar) == -1)
        {
            error_code = TECS_ERR_AUTHORIZE_FAILED;
            oss << AuthorizeError(get_method_name(), "USERGROUP", "INFO", _user_id, -1);
            goto error_common;

        }

        // Let's make sure that the user doesn't exist in the database
        gpool = UsergroupPool::GetInstance();
        rc = gpool->Get(groupname, &ug);

        if(rc != 0)
        {
            error_code = TECS_ERR_USERGROUP_NOT_EXIST;
            oss << GetObjectError(get_method_name(), "USERGROUP", gid);
            goto error_common;
        }

        gid = ug.get_gid();

        //oss << *usergroup;

        map<string, xmlrpc_c::value> oneGroup;

        oneGroup.insert(make_pair("groupname", xmlrpc_c::value_string(ug.get_groupname())));
        oneGroup.insert(make_pair("grouptype", xmlrpc_c::value_int(ug.get_grouptype())));
        oneGroup.insert(make_pair("description", xmlrpc_c::value_string(ug.get_description())));
        if(ug.is_enabled())
            oneGroup.insert(make_pair("enabled", xmlrpc_c::value_int(USERGROUP_ENABLE)));
        else
            oneGroup.insert(make_pair("enabled", xmlrpc_c::value_int(USERGROUP_DISABLE)));
        oneGroup.insert(make_pair("create_time", xmlrpc_c::value_string(ug.get_create_time())));
        arrayGroup.push_back(xmlrpc_c::value_struct(oneGroup));

        // All nice, return the info
        arrayData.push_back(xmlrpc_c::value_int(TECS_SUCCESS));
        arrayData.push_back(xmlrpc_c::value_i8(-1));
        arrayData.push_back(xmlrpc_c::value_i8(1));
        arrayData.push_back(xmlrpc_c::value_array(arrayGroup));

        // Copy arrayresult into retval mem space
        arrayresult = new xmlrpc_c::value_array(arrayData);
        *retval     = *arrayresult;

        delete arrayresult;

        return;
    }

error_common:
    arrayData.push_back(xmlrpc_c::value_int(error_code));
    arrayData.push_back(xmlrpc_c::value_string(oss.str()));

    cout << oss.str() << endl;

    xmlrpc_c::value_array arrayresult_error(arrayData);

    *retval = arrayresult_error;

    return;
}


/******************************************************************************
* SessionLogin 类的实现
******************************************************************************/
SessionLoginMethod::SessionLoginMethod(UserPool *upool)
{
     _help = "login";
     _method_name = "tecs.session.login";
     _upool = upool;
}

/* -------------------------------------------------------------------------- */

SessionLoginMethod::~SessionLoginMethod()
{
}

/* -------------------------------------------------------------------------- */

void SessionLoginMethod::MethodEntry(const xmlrpc_c::paramList& paramList, xmlrpc_c::value* const retval)
{
    int64  rc = -1;

    vector<xmlrpc_c::value> arrayData;
    string username, password;
    ostringstream oss;

    string session = xmlrpc_c::value_string(paramList.getString(0));

    User::SplitSecret(session, username, password);

    string sha1_pass = Sha1Digest(username, password);
    session = username + ":" + sha1_pass;
    rc = _upool->Authenticate(session);

    if(rc == -1)
    {
        oss << "User can not be authorized to login.";

        arrayData.push_back(xmlrpc_c::value_int(ERROR));
        arrayData.push_back(xmlrpc_c::value_string(oss.str()));
    }
    else
    {
        session = SessionManager::GetInstance()->NewSession(username, session);

        if(session.empty())
        {
            oss << "User can not get a session.";

            arrayData.push_back(xmlrpc_c::value_int(ERROR));
            arrayData.push_back(xmlrpc_c::value_string(oss.str()));
        }
        else
        {
            RunningStat stat;
            int license_status = -1;

            if(GetRunningStat(stat)== SUCCESS)
            {
                license_status = license_valid(stat, oss);
            }

            if(license_status == LICENSE_OK)
            {
                arrayData.push_back(xmlrpc_c::value_int(SUCCESS));
                arrayData.push_back(xmlrpc_c::value_string(session));
            }
            else if(license_status == LICENSE_OVERLIMIT)
            {
                arrayData.push_back(xmlrpc_c::value_int(TECS_ERR_LICENSE_OVERLIMIT));
                arrayData.push_back(xmlrpc_c::value_string(session));
            }
            else
            {
                arrayData.push_back(xmlrpc_c::value_int(TECS_ERR_LICENSE_INVALID));
                arrayData.push_back(xmlrpc_c::value_string(oss.str()));
            }
        }
    }

    xmlrpc_c::value_array arrayresult(arrayData);
    *retval = arrayresult;
}

/******************************************************************************
* SessionQuery 类的实现
******************************************************************************/

SessionQueryMethod::SessionQueryMethod(UserPool *upool)
{
    _help = "session query";
    _method_name = "tecs.session.query";
    _upool = upool;
}

SessionQueryMethod::~SessionQueryMethod()
{

}

void SessionQueryMethod::MethodEntry(const xmlrpc_c::paramList& paramList, xmlrpc_c::value* const retval)
{
    ostringstream  oss;

    const int64 MAX_COUNT = 1000;

    vector<xmlrpc_c::value> arrayData;
    vector<xmlrpc_c::value> arraySession;

    User* op_user = _upool->Get(_user_id, false);

    if(op_user->get_role() != UserGroup::CLOUD_ADMINISTRATOR)
    {
        oss << "User can not be authorized to query session infomation.";

        arrayData.push_back(xmlrpc_c::value_int(ERROR));
        arrayData.push_back(xmlrpc_c::value_string(oss.str()));

        xmlrpc_c::value_array arrayresult(arrayData);
        *retval = arrayresult;
        return;
    }

    int count = 0;

    map<string, SessionInfo> allsessions = SessionManager::GetInstance()->GetSessionsMap();
    map<string, SessionInfo>::iterator it = allsessions.begin();

    for(; it!=allsessions.end(); it++)
    {
        if(count++ > MAX_COUNT)
        {
            break;
        }

        if(it->second.user.empty())
        {
            continue;
        }

        map<string, xmlrpc_c::value> oneSession;
        oneSession.insert(make_pair("user", xmlrpc_c::value_string(it->second.user)));
        Datetime login_time = it->second.login_time;
        oneSession.insert(make_pair("login_time", xmlrpc_c::value_string(login_time.tostr())));

        arraySession.push_back(xmlrpc_c::value_struct(oneSession));
    }

    arrayData.push_back(xmlrpc_c::value_int(TECS_SUCCESS));
    arrayData.push_back(xmlrpc_c::value_array(arraySession));

    xmlrpc_c::value_array arrayresult(arrayData);
    *retval = arrayresult;
}


/******************************************************************************
* SessionLogout 类的实现
******************************************************************************/
SessionLogoutMethod::SessionLogoutMethod(UserPool *upool)
{
     _help = "logout";
     _method_name = "tecs.session.logout";
     _upool = upool;
}

/* -------------------------------------------------------------------------- */

SessionLogoutMethod::~SessionLogoutMethod()
{
}

/* -------------------------------------------------------------------------- */

void SessionLogoutMethod::MethodEntry(const xmlrpc_c::paramList& paramList, xmlrpc_c::value* const retval)
{
    vector<xmlrpc_c::value> arrayData;

    string session = xmlrpc_c::value_string(paramList.getString(0));
    SessionManager::GetInstance()->DeleteSession(session);

    arrayData.push_back(xmlrpc_c::value_int(SUCCESS));
    xmlrpc_c::value_array arrayresult(arrayData);
    *retval = arrayresult;
}

/* -------------------------------------------------------------------------- */

}
