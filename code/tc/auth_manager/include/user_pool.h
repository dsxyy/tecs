/******************************************************************************
* Copyright (c) 2011，深圳市中兴通讯股份有限公司
* All rights reserved.
* 
* 文件名称：userpool.h
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
#ifndef AUTH_USER_POOL_H
#define AUTH_USER_POOL_H
#include "pool_sql.h"
#include "user.h"


// 禁用拷贝构造宏定义
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)
#endif

using namespace std;

namespace zte_tecs
{

class AuthRequest; //Forward definition of AuthRequest

typedef struct tagUserInfo
{
    string username;
    string password;
    bool   enabled;
    int    role;
    int64  group_id;
    string email;
    string phone;
    string location;
    string description;
    string create_time;
    int max_session;
} UserInfo;

/**
@brief 功能描述: UserPool类负责用户资源池管理和用户会话鉴权和操作授权管理\n
@li @b 其它说明: 无
*/
class UserPool : public PoolSQL
{
public:
    static UserPool* GetInstance()
    {
        return instance;
    };

    static UserPool* CreateInstance(SqlDB *db)
    {
        if (NULL == instance)
        {
            instance = new UserPool(db);
        }

        return instance;
    };

    ~UserPool(){};

    STATUS Init();

    /**************************************************************************/
    /**
    @brief 功能描述:分配一个新的用户对象
    @li @b 入参列表：
    @verbatim
        无
    @endverbatim
    
    @li @b 出参列表：
    @verbatim
        无 
    @endverbatim

    @li @b 返回值列表：
    @verbatim
        无
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    int64 Allocate (
        int64*   oid,
        string  hostname,
        string  password,
        bool    enabled,
        int     role,
        int     groupid,
        string& error_str);

    /**************************************************************************/
    /**
    @brief 功能描述:通过用户id获取用户对象
    @li @b 入参列表：
    @verbatim
        无
    @endverbatim
    
    @li @b 出参列表：
    @verbatim
        无 
    @endverbatim

    @li @b 返回值列表：
    @verbatim
        无
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    User * Get(
        int64   oid,
        bool    lock)
    {
        User * user = static_cast<User *>(PoolSQL::Get(oid, lock));

        return user;
    }

    /**************************************************************************/
    /**
    @brief 功能描述:通过用户名获取用户对象
    @li @b 入参列表：
    @verbatim
        无
    @endverbatim
    
    @li @b 出参列表：
    @verbatim
        无 
    @endverbatim

    @li @b 返回值列表：
    @verbatim
        无
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    User * Get(
        string  username,
        bool    lock)
    {
        map<string, int64>::iterator index;

        index = _known_users.find(username);

        if ( index != _known_users.end() )
        {
            return Get((int)index->second, lock);
        }

        return 0;
    }

    /**************************************************************************/ 
    bool GetUserNameByID(int64 id, string& name)
    {
        User* u = Get(id, false);
        
        if(u == NULL)
        {
            name = "";
            return false;
        }

        name = u->get_username();

        return true;        
    }

    /**************************************************************************/ 
    bool GetUserIDByName(const string &name, int64& uid)
    {
        User* u = Get(name, false);
        
        if(u == NULL)
        {
            uid = -1;
            return false;
        }

        uid = u->get_uid();

        return true;
    }

    /**************************************************************************/
    /**
    @brief 功能描述:更新用户信息
    @li @b 入参列表：
    @verbatim
        无
    @endverbatim
    
    @li @b 出参列表：
    @verbatim
        无 
    @endverbatim

    @li @b 返回值列表：
    @verbatim
        无
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    int Update(User * user)
    {
        return user->Update(_db);
    };

    /**************************************************************************/
    /**
    @brief 功能描述:从数据库中删掉该用户
    @li @b 入参列表：
    @verbatim
        无
    @endverbatim
    
    @li @b 出参列表：
    @verbatim
        无 
    @endverbatim

    @li @b 返回值列表：
    @verbatim
        无
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    int Drop(User * user)
    {
        int rc = PoolSQL::Drop(user);

        if ( rc == 0)
        {
            _known_users.erase(user->get_username());
        }

        return rc;
    };

    /**************************************************************************/
    /**
    @brief 功能描述: 在apisvr以外的其它进程中需要认证时的调用接口
    @li @b 入参列表：
    @verbatim
        无
    @endverbatim
    
    @li @b 出参列表：
    @verbatim
        无 
    @endverbatim

    @li @b 返回值列表：
    @verbatim
        无
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    static int64 ExternAuthenticate(const string& session);

    static string ExternGetSecretBySession(const string& session);

    /**************************************************************************/
    /**
    @brief 功能描述:Returns whether there is a user with given username/password or not
    @li @b 入参列表：
    @verbatim
        无
    @endverbatim
    
    @li @b 出参列表：
    @verbatim
        无 
    @endverbatim

    @li @b 返回值列表：
    @verbatim
        无
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    int64 Authenticate(const string& session);

    /**************************************************************************/
    /**
    @brief 功能描述: Returns whether there is a user with given username/password or not
    @li @b 入参列表：
    @verbatim
        无
    @endverbatim
    
    @li @b 出参列表：
    @verbatim
        无 
    @endverbatim

    @li @b 返回值列表：
    @verbatim
        无
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    static int Authorize(AuthRequest& ar);

    /**************************************************************************/
    /**
    @brief 功能描述:Dumps the User pool in XML format.
    @li @b 入参列表：
    @verbatim
        无
    @endverbatim
    
    @li @b 出参列表：
    @verbatim
        无 
    @endverbatim

    @li @b 返回值列表：
    @verbatim
        无
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    int Dump(ostringstream& oss, const string& where);

    map<string, int64> get_known_users() { return _known_users; }

    map<int64, UserInfo> get_dump_users() { return _dump_users; }

    static string default_user;    

private:
    UserPool(SqlDB * db);
    static UserPool *instance;
    DISALLOW_COPY_AND_ASSIGN(UserPool);

    /**
     *  Factory method to produce User objects
     *    @return a pointer to the new User
     */
    PoolObjectSQL * Create()
    {
        return new User;
    };

    /**
     *  Callback function to build the knwon_user map (User::User)
     *    @param num the number of columns read from the DB
     *    @param names the column names
     *    @param vaues the column values
     *    @return 0 on success
     */
    int InitCallback(void *nil, SqlColumn * columns);

    /**
     *  Callback function to get output the user pool in XML format
     *  (User::dump)
     *    @param _oss pointer to the output stream
     *    @param num the number of columns read from the DB
     *    @param names the column names
     *    @param vaues the column values
     *    @return 0 on success
     */
    int DumpCallback(void * oss, SqlColumn * columns);

    /**
     *  This map stores the association between UIDs and Usernames
     */
    map<string, int64> _known_users;

    /**
     *
     */
    map<int64, UserInfo> _dump_users;
   
};

}

#endif /*USER_POOL_H_*/

