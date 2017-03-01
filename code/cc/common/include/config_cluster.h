/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：config_cluster.h
* 文件标识：见配置管理计划书
* 内容摘要：ConfigCluster类的声明文件
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
#ifndef HM_CONFIG_CLUSTER_H
#define HM_CONFIG_CLUSTER_H
#include "sky.h"
#include "sqldb.h"

// 禁用拷贝构造宏定义
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)
#endif

using namespace std;
namespace zte_tecs {

// 集群参数配置关键字，ConfigCluster类的name
#define CFG_TECS_NAME               "registered_tecs_name"
#define CFG_IMG_USAGE               "img_usage"
#define CFG_IMG_SRC_TYPE            "img_source_type"
#define CFG_IMG_SPC_EXP             "img_space_expand"
#define CFG_CLUSTER_VERIFY_ID       "cluster_verify_id"
#define CFG_SHARE_IMG_USAGE         "share_img_usage"

#define CFG_CLUSTER_SAVE_ENERGY_FLG         "cluster_save_energy_flg"
#define CFG_CLUSTER_POWERON_MIN             "cluster_poweron_min"
#define CFG_CLUSTER_POWERON_MAX             "cluster_poweron_max"
#define CFG_CLUSTER_POWERON_PER             "cluster_poweron_per"
#define CFG_CLUSTER_SAVE_ENERGY_INTERVAL    "cluster_save_energy_interval"

#define CFG_SCH_POLICY              "sch_policy"
#define CFG_RUN_SCH_MAX_VMS         "run_sch_max_vms"
#define CFG_RUN_SCH_CYCLE           "run_sch_cycle"

#define CFG_VMHA_HOST_DOWN_POLICY   "vmha_host_down_policy"
#define CFG_VMHA_HOST_FAULT_POLICY  "vmha_host_fault_policy"
#define CFG_VMHA_HOST_STORAGE_FAULT_POLICY \
                                    "vmha_host_storage_fault_policy"

/**
@brief 功能描述: 主机类定义
@li @b 其它说明：无
*/
class ConfigCluster: public Callbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static ConfigCluster *GetInstance()
    {
        return _instance;
    }

    static ConfigCluster* CreateInstance(SqlDB *db)
    {
        if(NULL == _instance)
        {
            _instance = new ConfigCluster(db); 
        }
        
        return _instance;
    };
    virtual ~ConfigCluster() { _instance = NULL; }

    const string Get(const string &name);
    int32 Set(const string &name, const string &value);
    int32 Drop(const string &name);

    const map<string, string> &get_configs() const
    {
        return _configs;
    }

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
        CONFIG_NAME     = 0,
        CONFIG_VALUE    = 1,
        LIMIT           = 2
    };

    map<string, string> _configs;

    static const char *_table;
    static const char *_db_names;
    static SqlDB      *_db;

    static ConfigCluster *_instance;
    ConfigCluster();
    ConfigCluster(SqlDB *pDb);

    int32 InsertReplace(const string &name, const string &value, bool replace);
    int32 InitCallback(void *nil, SqlColumn *columns);
    int SelectStringCallback(void *nil, SqlColumn *columns);
    int GetConfigValueFromConfigCluster(const string & config_name,string &config_value);

    DISALLOW_COPY_AND_ASSIGN(ConfigCluster);
};

}  /* end namespace zte_tecs */

#endif /* end HM_CONFIG_CLUSTER_H */

