/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：config_tcu.h
* 文件标识：见配置管理计划书
* 内容摘要：ConfigTcu类的声明文件
* 当前版本：1.0
* 作    者：袁进坤
* 完成日期：2011年8月21日
*
* 修改记录1：
*     修改日期：2011/8/21
*     版 本 号：V1.0
*     修 改 人：袁进坤
*     修改内容：创建
*******************************************************************************/
#ifndef HM_CONFIG_TCU_H
#define HM_CONFIG_TCU_H
#include "sky.h"
#include "sqldb.h"
#include "msg_host_manager_with_api_method.h"
#include "msg_cluster_agent_with_api_method.h" 

// 禁用拷贝构造宏定义
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)
#endif

using namespace std;
namespace zte_tecs {

/**
@brief 功能描述: 主机类定义
@li @b 其它说明：无
*/
class ConfigTcu: public Callbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static ConfigTcu *GetInstance()
    {
        return _instance;
    }

    static ConfigTcu* CreateInstance(SqlDB *db)
    {
        if(NULL == _instance)
        {
            _instance = new ConfigTcu(db); 
        }
        
        return _instance;
    };
    virtual ~ConfigTcu() { _instance = NULL; }

    int32 Get(int64 cpu_info_id, TcuInfoType &tcu_info);
    int32 Set(int64 cpu_info_id, int32 tcu_num, const string &description);
    int32 Drop(int64 cpu_info_id);

    int32 Get(const map<string, string> &cpu_info, TcuInfoType &tcu_info);
    int32 Set(const map<string, string> &cpu_info, 
              int32 tcu_num, const string &description);
    int32 Drop(const map<string, string> &cpu_info);
    int64 Count();

    int32 Search(vector<TcuInfoType> &tcu_infos, const string &where);
    int32 GetSystemTcuNumByCpuId(int64 cpuid);

    void Print();

/*******************************************************************************
* 2. protected section
*******************************************************************************/
// protected:

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:
    /* 各成员在数据库表中的列位置 */
    enum ColNames
    {
        CPU_INFO_ID     = 0,
        TCU_NUM         = 1,
        DESCRIPTION     = 2,
        LIMIT           = 3
    };

    static const char *_table;
    static const char *_db_names;
    static const char *_view_config_tcu_select_names;
    
    static const char *_view;
    static SqlDB      *_db;

    static ConfigTcu *_instance;
    ConfigTcu() {}
    ConfigTcu(SqlDB *pDb);

    int32 InsertReplace(int64 cpu_info_id, int32 tcu_num, 
                        const string &description, bool replace);
    int32 SelectCallback(void *nil, SqlColumn *columns);
    int32 SearchCallback(void *nil, SqlColumn *columns);
    int32 CountCallback(void *nil, SqlColumn *columns);

    DISALLOW_COPY_AND_ASSIGN(ConfigTcu);
};

}  /* end namespace zte_tecs */

#endif /* end HM_CONFIG_TCU_H */

