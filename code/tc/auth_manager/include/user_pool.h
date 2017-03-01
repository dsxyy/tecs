/******************************************************************************
* Copyright (c) 2011������������ͨѶ�ɷ����޹�˾
* All rights reserved.
* 
* �ļ����ƣ�userpool.h
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
#ifndef AUTH_USER_POOL_H
#define AUTH_USER_POOL_H
#include "pool_sql.h"
#include "user.h"


// ���ÿ�������궨��
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
@brief ��������: UserPool�ฺ���û���Դ�ع�����û��Ự��Ȩ�Ͳ�����Ȩ����\n
@li @b ����˵��: ��
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
    @brief ��������:����һ���µ��û�����
    @li @b ����б�
    @verbatim
        ��
    @endverbatim
    
    @li @b �����б�
    @verbatim
        �� 
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
        ��
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
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
    @brief ��������:ͨ���û�id��ȡ�û�����
    @li @b ����б�
    @verbatim
        ��
    @endverbatim
    
    @li @b �����б�
    @verbatim
        �� 
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
        ��
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
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
    @brief ��������:ͨ���û�����ȡ�û�����
    @li @b ����б�
    @verbatim
        ��
    @endverbatim
    
    @li @b �����б�
    @verbatim
        �� 
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
        ��
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
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
    @brief ��������:�����û���Ϣ
    @li @b ����б�
    @verbatim
        ��
    @endverbatim
    
    @li @b �����б�
    @verbatim
        �� 
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
        ��
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    int Update(User * user)
    {
        return user->Update(_db);
    };

    /**************************************************************************/
    /**
    @brief ��������:�����ݿ���ɾ�����û�
    @li @b ����б�
    @verbatim
        ��
    @endverbatim
    
    @li @b �����б�
    @verbatim
        �� 
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
        ��
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
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
    @brief ��������: ��apisvr�����������������Ҫ��֤ʱ�ĵ��ýӿ�
    @li @b ����б�
    @verbatim
        ��
    @endverbatim
    
    @li @b �����б�
    @verbatim
        �� 
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
        ��
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    static int64 ExternAuthenticate(const string& session);

    static string ExternGetSecretBySession(const string& session);

    /**************************************************************************/
    /**
    @brief ��������:Returns whether there is a user with given username/password or not
    @li @b ����б�
    @verbatim
        ��
    @endverbatim
    
    @li @b �����б�
    @verbatim
        �� 
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
        ��
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    int64 Authenticate(const string& session);

    /**************************************************************************/
    /**
    @brief ��������: Returns whether there is a user with given username/password or not
    @li @b ����б�
    @verbatim
        ��
    @endverbatim
    
    @li @b �����б�
    @verbatim
        �� 
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
        ��
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    static int Authorize(AuthRequest& ar);

    /**************************************************************************/
    /**
    @brief ��������:Dumps the User pool in XML format.
    @li @b ����б�
    @verbatim
        ��
    @endverbatim
    
    @li @b �����б�
    @verbatim
        �� 
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
        ��
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
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

