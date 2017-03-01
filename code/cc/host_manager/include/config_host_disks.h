/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：config_host_disks.h
* 文件标识：
* 内容摘要：ConfigHostDisks 类的定义文件
* 当前版本：1.0
* 作    者：liuyi
* 完成日期：2013年7月25日
*
* 修改记录1：
*     修改日期：2013/7/25
*     版 本 号：V1.0
*     修 改 人：xiett
*     修改内容：创建
*******************************************************************************/
#ifndef CONFIG_HOST_DISKS_H
#define CONFIG_HOST_DISKS_H
#include "sky.h"
#include "sqldb.h"
#include "db_config.h"
#include "pool_sql.h"
#include "msg_host_manager_with_host_agent.h"

using namespace std;
namespace zte_tecs {

/**
@brief 功能描述: HC硬盘信息类
@li @b 其它说明：无
*/
class ConfigHostDisks : public Callbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static ConfigHostDisks *GetInstance()
    {
        return _instance;
    }

    static ConfigHostDisks* CreateInstance(SqlDB *db)
    {
        if(NULL == _instance)
        {
            _instance = new ConfigHostDisks(db); 
        }
        
        return _instance;
    };

    virtual ~ConfigHostDisks() { _instance = NULL; };
    int32 Insert(const DiskInfo& disk_info, int64 hid);
    int32 Delete(int64 hid);

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
        TYPE,
        STATUS,
        LIMIT
    };
    static const char *_table;
    static const char *_db_names;
    static SqlDB      *_db;

    static ConfigHostDisks *_instance;
    ConfigHostDisks() {};
    ConfigHostDisks(SqlDB *pDb);
    DISALLOW_COPY_AND_ASSIGN(ConfigHostDisks);
};

}
#endif
