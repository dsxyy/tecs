/******************************************************************************
* Copyright (c) 2011������������ͨѶ�ɷ����޹�˾
* All rights reserved.
* 
* �ļ����ƣ�user.h
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


#ifndef AUTH_USER_H
#define AUTH_USER_H

#include "pool_sql.h"
#include "crypt.h"
#include "datetime.h"

// ���ÿ�������궨��
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
@brief ��������: �û�������Ϣ�־û���\n
@li @b ����˵��: ��
*/	
class User : public PoolObjectSQL
{

    friend class UserPool;

public:

    /**************************************************************************/
    /**
    @brief ��������:����<<������
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
    friend ostream& operator<<(ostream& os, User& u);

    /**************************************************************************/
    /**
    @brief ��������:print the User object into a string in plain text
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
    string& to_str(string& str) const;

    /**************************************************************************/
    /**
    @brief ��������:print the User object into a string in XML format
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
    string& to_xml(string& xml) const;

    /**************************************************************************/
    /**
    @brief ��������:��ȡ�û�ΨһID
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
    int64 get_uid() const
    {
        return _oid;
    }

    /**************************************************************************/
    /**
    @brief ��������:�ж��û��Ƿ�������
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
     bool is_enabled() const;


    /**************************************************************************/
    /**
    @brief ��������:��ȡ�û���
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
    @brief ��������:��ȡ�û�����
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
    const string& get_password() const
    {
        return _password;
    }

    /**************************************************************************/
    /**
    @brief ��������:�����û���ɫ
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
    int get_role() const
    {
        return _role;
    }

    /**************************************************************************/
    /**
    @brief ��������:��ȡ�û�email��Ϣ
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
    const string& get_email() const
    {
        return _email;
    }

    /**************************************************************************/
    /**
    @brief ��������:��ȡ�绰����
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
    const string& get_phone() const
    {
        return _phone;
    }

    /**************************************************************************/
    /**
    @brief ��������:��ȡ�û���ID
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
    int get_group() const
    {
        return _group_id;
    }

    /**************************************************************************/
    /**
    @brief ��������:��ȡ�û���ַ��Ϣ
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
    const string& get_location() const
    {
        return _location;
    }


    /**************************************************************************/
    /**
    @brief ��������:��ȡ�û����ͬʱ��¼����
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
    const int& get_max_session() const
    {
        return _max_session;
    }
    
    
    /**************************************************************************/
    /**
    @brief ��������:���õ�ǰ�û�
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
    void enable()
    {
        _enabled = true;
    }

    /**************************************************************************/
    /**
    @brief ��������:���õ�ǰ�û�
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
    void disable()
    {
        _enabled = false;
    }

    /**************************************************************************/
    /**
    @brief ��������:�����û���
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
    void set_username(const string &username)
    {
        _username = username;
    }

    /**************************************************************************/
    /**
    @brief ��������:�����û�����
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
    @brief ��������:�����û���ɫ
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
    void set_role(int role)
    {
        _role = role;
    }

    /**************************************************************************/
    /**
    @brief ��������:�����û�email
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
    void set_email(const string & email)
    {
        _email = email;
    }

    /**************************************************************************/
    /**
    @brief ��������:�����û��绰
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
    void set_phone(const string & phone)
    {
        _phone = phone;
    }

    /**************************************************************************/
    /**
    @brief ��������:�����û���ID
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
    void set_group(int group_id)
    {
        _group_id = group_id;
    }

    /**************************************************************************/
    /**
    @brief ��������:�����û���ַ��Ϣ
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
    void set_location(const string & location)
    {
        _location = location;
    }

    /**************************************************************************/
    /**
    @brief ��������:�����û����ͬʱ��¼����
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
    void set_max_session(int max_session)
    {
        _max_session = max_session;
    }
    
    /**************************************************************************/
    /**
    @brief ��������: Splits an authentication token (<usr>:<pass>)
    @li @b ����б�
    @verbatim
        @param the authentication token
    @endverbatim
    
    @li @b �����б�
    @verbatim
        @param username
        @param password
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
        @return 0 on success
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
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
     * �û���ɫ
     */
    int         _role;

    /**
     * �û�email
     */
    string      _email;
    
    /**
     * �û���ID
     */
    int64        _group_id;
    
    /**
     * �û���ַ
     */
    string      _location;

    /**
     * �û��绰
     */
    string      _phone;

    /**
     * ���ͬʱ��¼����
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
