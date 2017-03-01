/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：config_host_statistics.h
* 文件标识：见配置管理计划书
* 内容摘要：ConfigHostStatistics 和 ConfigHostStatisticsNics 类的头文件
* 当前版本：1.0
* 作    者：xiett
* 完成日期：2013年7月30日
*
* 修改记录1：
*     修改日期：2011/7/30
*     版 本 号：V1.0
*     修 改 人：xiett
*     修改内容：创建
*******************************************************************************/
#ifndef CONFIG_HOST_STATISTICS_H
#define CONFIG_HOST_STATISTICS_H

#include "sky.h"
#include "sqldb.h"
#include "db_config.h"
#include "pool_sql.h"
#include "msg_host_manager_with_host_agent.h"

using namespace std;
namespace zte_tecs {

/**
@brief 功能描述: ConfigHostStatistics类定义
@li @b 其它说明：物理机资源使用监控与统计
*/
class ConfigHostStatistics : public Callbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static ConfigHostStatistics *GetInstance()
    {
        return _instance;
    }

    static ConfigHostStatistics* CreateInstance(SqlDB *db)
    {
        if(NULL == _instance)
        {
            _instance = new ConfigHostStatistics(db); 
        }
        
        return _instance;
    };

    virtual ~ConfigHostStatistics() { _instance = NULL; };
    int32 SelectCallback(void *nil, SqlColumn *columns);
    int32 Search(vector<ResourceStatistics>& vec_host_statistics,
                                   const string where);
    int32 Insert(const ResourceStatistics& host_statistics,
                 int64 hid,
                 const string& save_time);
    int32 Delete(const string& statistics_time);

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
        HID = 0,
        STATISTICS_TIME,
        SAVE_TIME,
        CPU_USED_RATE,
        MEM_USED_RATE,
        LIMIT
    };
    static const char *_table;
    static const char *_db_names;
    static SqlDB      *_db;

    static ConfigHostStatistics *_instance;

    ConfigHostStatistics() {};
    ConfigHostStatistics(SqlDB *pDb);
    DISALLOW_COPY_AND_ASSIGN(ConfigHostStatistics);
};

/**
@brief 功能描述: ConfigHostStatisticsNics类定义
@li @b 其它说明：物理机资源使用监控与统计
*/
class ConfigHostStatisticsNics : public Callbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static ConfigHostStatisticsNics *GetInstance()
    {
        return _instance;
    }

    static ConfigHostStatisticsNics* CreateInstance(SqlDB *db)
    {
        if(NULL == _instance)
        {
            _instance = new ConfigHostStatisticsNics(db); 
        }
        
        return _instance;
    };

    virtual ~ConfigHostStatisticsNics() { _instance = NULL; };
    int32 SelectCallback(void *nil, SqlColumn *columns);
    int32 Search(vector<NicInfo>& vec_nic_info, const string where);
    int32 Insert(const NicInfo& nic_info, 
                 int64 hid,
                 const string& statistics_time,
                 const string& save_time);
    int32 Delete(const string& statistics_time);

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
        HID = 0,
        NAME,
        STATISTICS_TIME,
        SAVE_TIME,
        USED_RATE,
        LIMIT
    };
    static const char *_table;
    static const char *_db_names;
    static SqlDB      *_db;

    static ConfigHostStatisticsNics *_instance;

    ConfigHostStatisticsNics() {};
    ConfigHostStatisticsNics(SqlDB *pDb);
    DISALLOW_COPY_AND_ASSIGN(ConfigHostStatisticsNics);
};

}
#endif
