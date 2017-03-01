/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：config_vm_statistics.h
* 文件标识：见配置管理计划书
* 内容摘要：ConfigVmStatistics 和 ConfigVmStatisticsNics 类的头文件
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
#ifndef CONFIG_VM_STATISTICS_H
#define CONFIG_VM_STATISTICS_H

#include "sky.h"
#include "sqldb.h"
#include "db_config.h"
#include "pool_sql.h"
#include "vm_messages.h"
#include "msg_project_manager_with_api_method.h"

using namespace std;
namespace zte_tecs {

/**
@brief 功能描述: ConfigVmStatistics类定义
@li @b 其它说明：物理机资源使用监控与统计
*/
class ConfigVmStatistics : public Callbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static ConfigVmStatistics *GetInstance()
    {
        return _instance;
    }

    static ConfigVmStatistics* CreateInstance(SqlDB *db)
    {
        if(NULL == _instance)
        {
            _instance = new ConfigVmStatistics(db); 
        }
        
        return _instance;
    };
    virtual ~ConfigVmStatistics() { _instance = NULL; };
    int32 SelectCallback(void *nil, SqlColumn *columns);
    int32 Search(vector<ResourceStatistics>& vec_vm_statistics,
                 const string where);
    int32 Insert(const ResourceStatistics& vm_statistics,
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
        VID = 0,
        STATISTICS_TIME,
        SAVE_TIME,
        CPU_USED_RATE,
        MEM_USED_RATE,
        LIMIT
    };
    static const char *_table;
    static const char *_db_names;
    static SqlDB      *_db;

    static ConfigVmStatistics *_instance;

    ConfigVmStatistics() {};
    ConfigVmStatistics(SqlDB *pDb);
    DISALLOW_COPY_AND_ASSIGN(ConfigVmStatistics);
};

/**
@brief 功能描述: ConfigVmStatisticsNics类定义
@li @b 其它说明：物理机资源使用监控与统计
*/
class ConfigVmStatisticsNics : public Callbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static ConfigVmStatisticsNics *GetInstance()
    {
        return _instance;
    }

    static ConfigVmStatisticsNics* CreateInstance(SqlDB *db)
    {
        if(NULL == _instance)
        {
            _instance = new ConfigVmStatisticsNics(db); 
        }
        
        return _instance;
    };
    virtual ~ConfigVmStatisticsNics() { _instance = NULL; };
    int32 SelectCallback(void *nil, SqlColumn *columns);
    //int32 Search(vector<NicInfo>& vec_nic_info, const string where);
    int32 Insert(const NicInfo& nic_info, 
                 int64 vid,
                 const string& statistics_time,
                 const string& save_time);
    int32 Delete(const string& statistics_time);
    int GetVmStatisticsNics(vector<VmStatisticsNics>& vec_info,const string where);

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
        VID = 0,
        NAME,
        STATISTICS_TIME,
        SAVE_TIME,
        USED_RATE,
        LIMIT
    };
    static const char *_table;
    static const char *_db_names;
    static SqlDB      *_db;

    static ConfigVmStatisticsNics *_instance;

    ConfigVmStatisticsNics() {};
    ConfigVmStatisticsNics(SqlDB *pDb);
    DISALLOW_COPY_AND_ASSIGN(ConfigVmStatisticsNics);
};

/**
@brief 功能描述: VmDynamicData类定义
@li @b 其它说明：vm动态数据统计
*/
class VmDynamicData : public Callbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static VmDynamicData *GetInstance()
    {
        return _instance;
    }

    static VmDynamicData* CreateInstance(SqlDB *db)
    {
        if(NULL == _instance)
        {
            _instance = new VmDynamicData(db); 
        }
        
        return _instance;
    };

    virtual ~VmDynamicData() { _instance = NULL; };
    VmDynamicData() {};
    int32 GetProjectDynamicData(const string &where, vector<ProjectDynamicData> &project_data);
    int32 ProjectDataCallback(void *nil, SqlColumn *columns);
    int32 CountProjectDynamicData(void);
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
        VID = 0,
        NIC_USED_RATE,
        CPU_USED_RATE,
        MEM_USED_RATE,
        LIMIT
    };
    static const char *_table;
    static const char *_db_names;
    static SqlDB      *_db;
    static VmDynamicData *_instance;
    VmDynamicData(SqlDB *pDb);	
    DISALLOW_COPY_AND_ASSIGN(VmDynamicData);
};

}
#endif
