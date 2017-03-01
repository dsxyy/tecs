/******************************************************************************
* Copyright (c) 2011，深圳市中兴通讯股份有限公司
* All rights reserved.
* 
* 文件名称：usergrouppool.h
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
#ifndef AUTH_USERGROUP_POOL_H
#define AUTH_USERGROUP_POOL_H

#include "pool_sql.h"
#include "usergroup.h"

// 禁用拷贝构造宏定义
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
@brief 功能描述: UserGroupPool类负责用户组资源池管理\n
@li @b 其它说明: 无
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
    @brief 功能描述:分配一个用户组对象
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
    int64 Allocate(int64* oid, string groupname, 
                 int grouptype, bool enable, string& err_str);
    

    /**************************************************************************/
    /**
    @brief 功能描述:通过ID获取用户组
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
    int Get(int64 oid, UserGroup* ug);
    

    /**************************************************************************/
    /**
    @brief 功能描述:通过组名获取用户组
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
    int Get(string groupname, UserGroup* ug);
    

    /**************************************************************************/
    /**
    @brief 功能描述:更新用户组信息
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
    int  Update(UserGroup* ug);
    

    /**************************************************************************/
    /**
    @brief 功能描述:删除用户组
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
    int  Drop(UserGroup* ug);
    

    /**************************************************************************/
    /**
    @brief 功能描述:Dumps the Usergroup pool in XML format
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
    *  保存用户组名与信息对应关系
    */
    map<int64, UsergroupInfo> _dump_groups;
};

}
#endif

