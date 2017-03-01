/******************************************************************************
* Copyright (c) 2011������������ͨѶ�ɷ����޹�˾
* All rights reserved.
* 
* �ļ����ƣ�usergrouppool.h
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
#ifndef AUTH_USERGROUP_POOL_H
#define AUTH_USERGROUP_POOL_H

#include "pool_sql.h"
#include "usergroup.h"

// ���ÿ�������궨��
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)
#endif

namespace zte_tecs
{

typedef struct tagUsergroupInfo
{
    string groupname;
    int    grouptype;
    bool   enabled;
    string description;
    string create_time;
} UsergroupInfo;

/**
@brief ��������: UserGroupPool�ฺ���û�����Դ�ع���\n
@li @b ����˵��: ��
*/
class UsergroupPool : public SqlCallbackable
{
public:
    static UsergroupPool* GetInstance()
    {
        return instance;
    };

    static UsergroupPool* CreateInstance(SqlDB *db)
    {
        if (NULL == instance)
        {
            instance = new UsergroupPool(db);
        }

        return instance;
    };
    
    STATUS Init();
    
    ~UsergroupPool(){};

    /**************************************************************************/
    /**
    @brief ��������:����һ���û������
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
    int64 Allocate(int64* oid, string groupname, 
                 int grouptype, bool enable, string& err_str);
    

    /**************************************************************************/
    /**
    @brief ��������:ͨ��ID��ȡ�û���
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
    int Get(int64 oid, UserGroup* ug);
    

    /**************************************************************************/
    /**
    @brief ��������:ͨ��������ȡ�û���
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
    int Get(string groupname, UserGroup* ug);
    

    /**************************************************************************/
    /**
    @brief ��������:�����û�����Ϣ
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
    int  Update(UserGroup* ug);
    

    /**************************************************************************/
    /**
    @brief ��������:ɾ���û���
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
    int  Drop(UserGroup* ug);
    

    /**************************************************************************/
    /**
    @brief ��������:Dumps the Usergroup pool in XML format
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

    int64 GetGroupSize();

    map<int64, UsergroupInfo> get_dump_groups() { return _dump_groups; }

private:
    explicit UsergroupPool(SqlDB* db);
    static UsergroupPool *instance;

    DISALLOW_COPY_AND_ASSIGN(UsergroupPool);
    

    // *************************************************************************
    // DataBase implementation
    // *************************************************************************
    enum ColNames
    {
        OID       = 0,
        GROUPNAME = 1,
        DESCRIPTION = 2,
        CREATE_TIME = 3,
        GROUPTYPE = 4,
        ENABLED   = 5,
        LIMIT     = 6
    };
    
    static const char* k_table;       
    static const char* k_db_names;

    int64 _lastOid;
    
    int DumpCallback(void * oss, SqlColumn * columns);

    int SelectCallback(void *nil, SqlColumn * columns);

    /**
    *  �����û���������Ϣ��Ӧ��ϵ
    */
    map<int64, UsergroupInfo> _dump_groups;
};

}
#endif

