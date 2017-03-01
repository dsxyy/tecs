/******************************************************************************
* Copyright (c) 2011，深圳市中兴通讯股份有限公司
* All rights reserved.
* 
* 文件名称：user.h
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


#ifndef AUTH_USER_H
#define AUTH_USER_H

#include "pool_sql.h"
#include "crypt.h"
#include "datetime.h"

// 禁用拷贝构造宏定义
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)
#endif


using namespace std;

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace zte_tecs
{

/**
@brief 功能描述: 用户对象信息持久化类\n
@li @b 其它说明: 无
*/	
class User : public PoolObjectSQL
{

    friend class UserPool;

public:

    /**************************************************************************/
    /**
    @brief 功能描述:重载<<操作符
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
    friend ostream& operator<<(ostream& os, User& u);

    /**************************************************************************/
    /**
    @brief 功能描述:print the User object into a string in plain text
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
    string& to_str(string& str) const;

    /**************************************************************************/
    /**
    @brief 功能描述:print the User object into a string in XML format
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
    string& to_xml(string& xml) const;

    /**************************************************************************/
    /**
    @brief 功能描述:获取用户唯一ID
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
    int64 get_uid() const
    {
        return _oid;
    }

    /**************************************************************************/
    /**
    @brief 功能描述:判断用户是否已启用
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
     bool is_enabled() const;


    /**************************************************************************/
    /**
    @brief 功能描述:获取用户名
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
    const string& get_username() const
    {
        return _username;
    }

    const string& get_description() const
    {
        return _description;
    }

    string get_create_time()
    {
        return _create_time.tostr(true);
    }

    /**************************************************************************/
    /**
    @brief 功能描述:获取用户密码
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
    const string& get_password() const
    {
        return _password;
    }

    /**************************************************************************/
    /**
    @brief 功能描述:设置用户角色
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
    int get_role() const
    {
        return _role;
    }

    /**************************************************************************/
    /**
    @brief 功能描述:获取用户email信息
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
    const string& get_email() const
    {
        return _email;
    }

    /**************************************************************************/
    /**
    @brief 功能描述:获取电话号码
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
    const string& get_phone() const
    {
        return _phone;
    }

    /**************************************************************************/
    /**
    @brief 功能描述:获取用户组ID
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
    int get_group() const
    {
        return _group_id;
    }

    /**************************************************************************/
    /**
    @brief 功能描述:获取用户地址信息
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
    const string& get_location() const
    {
        return _location;
    }


    /**************************************************************************/
    /**
    @brief 功能描述:获取用户最大同时登录次数
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
    const int& get_max_session() const
    {
        return _max_session;
    }
    
    
    /**************************************************************************/
    /**
    @brief 功能描述:启用当前用户
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
    void enable()
    {
        _enabled = true;
    }

    /**************************************************************************/
    /**
    @brief 功能描述:禁用当前用户
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
    void disable()
    {
        _enabled = false;
    }

    /**************************************************************************/
    /**
    @brief 功能描述:设置用户名
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
    void set_username(const string &username)
    {
        _username = username;
    }

    /**************************************************************************/
    /**
    @brief 功能描述:设置用户密码
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
    void set_password(const string & password)
    {
        _password = Sha1Digest(_username, password);
    }

    void set_description(const string & description)
    {
        _description = description;
    }

    /**************************************************************************/
    /**
    @brief 功能描述:设置用户角色
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
    void set_role(int role)
    {
        _role = role;
    }

    /**************************************************************************/
    /**
    @brief 功能描述:设置用户email
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
    void set_email(const string & email)
    {
        _email = email;
    }

    /**************************************************************************/
    /**
    @brief 功能描述:设置用户电话
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
    void set_phone(const string & phone)
    {
        _phone = phone;
    }

    /**************************************************************************/
    /**
    @brief 功能描述:设置用户组ID
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
    void set_group(int group_id)
    {
        _group_id = group_id;
    }

    /**************************************************************************/
    /**
    @brief 功能描述:设置用户地址信息
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
    void set_location(const string & location)
    {
        _location = location;
    }

    /**************************************************************************/
    /**
    @brief 功能描述:设置用户最大同时登录次数
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
    void set_max_session(int max_session)
    {
        _max_session = max_session;
    }
    
    /**************************************************************************/
    /**
    @brief 功能描述: Splits an authentication token (<usr>:<pass>)
    @li @b 入参列表：
    @verbatim
        @param the authentication token
    @endverbatim
    
    @li @b 出参列表：
    @verbatim
        @param username
        @param password
    @endverbatim

    @li @b 返回值列表：
    @verbatim
        @return 0 on success
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    static int SplitSecret(const string secret, string& user, string& pass);

protected:

    // *************************************************************************
    // Constructor
    // *************************************************************************

    User(int64   id=-1,
         string _username="",
         string _password="",
         bool   _enabled=true);

    virtual ~User();

    // *************************************************************************
    // DataBase implementation
    // *************************************************************************

    enum ColNames
    {
        OID             = 0,
        USERNAME        = 1,
        DESCRIPTION     = 2,
        CREATE_TIME     = 3,
        ENABLED         = 4,     // 0 = false, 1 = true
        ROLE            = 5,
        GROUPID         = 6,
        PASSWORD        = 7,
        EMAIL           = 8,
        PHONE           = 9,
        LOCATION        = 10,
        MAX_SESSION     = 11,
        LIMIT           = 12
    };

    static const char * k_table;
    static const char * k_db_names;
    static const char * k_table_userdel;

    /**
     *  Reads the User (identified with its OID=UID) from the database.
     *    @param db pointer to the db
     *    @return 0 on success
     */
    virtual int Select(SqlDB *db);

    /**
     *  Writes the User in the database.
     *    @param db pointer to the db
     *    @return 0 on success
     */
    virtual int Insert(SqlDB *db, string& error_str);

    /**
     *  Writes/updates the User data fields in the database.
     *    @param db pointer to the db
     *    @return 0 on success
     */
    virtual int Update(SqlDB *db);

    /**
     *  Drops USer from the database
     *    @param db pointer to the db
     *    @return 0 on success
     */
    virtual int Drop(SqlDB *db);

    /**
     *  Function to output a User object in to an stream in XML format
     *    @param oss the output stream
     *    @param num the number of columns read from the DB
     *    @param names the column names
     *    @param vaues the column values
     *    @return 0 on success
     */
    static int Dump(ostringstream& oss, SqlColumn * columns);


private:

    DISALLOW_COPY_AND_ASSIGN(User);

    /**
     *  User's username
     */
    string      _username;

    string      _description;

    Datetime    _create_time;

    /**
     *  User's password
     */
    string      _password;

    /**
     * Flag marking user enabled/disabled
     */
    bool        _enabled;

    /**
     * 用户角色
     */
    int         _role;

    /**
     * 用户email
     */
    string      _email;
    
    /**
     * 用户组ID
     */
    int64        _group_id;
    
    /**
     * 用户地址
     */
    string      _location;

    /**
     * 用户电话
     */
    string      _phone;

    /**
     * 最大同时登录次数
     */
    int         _max_session;

    /**
     *  Execute an INSERT or REPLACE Sql query.
     *    @param db The SQL DB
     *    @param replace Execute an INSERT or a REPLACE
     *    @return 0 one success
     */
    int insert_replace(SqlDB *db, bool replace);

    /**
     *  Callback function to unmarshall a User object (User::select)
     *    @param columns the columns read from the DB
     *    @return 0 on success
     */
    int SelectCallback(void *nil, SqlColumn * columns);
 
};

}
#endif /*USER_H_*/
