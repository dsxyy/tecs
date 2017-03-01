/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：config_cmm_pool.h
* 文件标识：见配置管理计划书
* 内容摘要：ConfigCmmPool类的声明文件
* 当前版本：1.0
* 作    者：liuyi
* 完成日期：2011年8月21日
*
* 修改记录1：
*     修改日期：2012/12/21
*     版 本 号：V1.0
*     修 改 人：liuyi
*     修改内容：创建
*******************************************************************************/
#ifndef HM_CONFIG_CMM_POOL_H
#define HM_CONFIG_CMM_POOL_H
#include "sky.h"
#include "sqldb.h"
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
class ConfigCmmPool: public Callbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static ConfigCmmPool *GetInstance()
    {
        return _instance;
    }

    static ConfigCmmPool* CreateInstance(SqlDB *db)
    {
        if(NULL == _instance)
        {
            _instance = new ConfigCmmPool(db); 
        }
        
        return _instance;
    };
    virtual ~ConfigCmmPool() { _instance = NULL; }
    int32 SelectCallback(void *nil, SqlColumn * columns);
    int32 Set(const CmmConfig &cmm_config);
    int32 Add(const CmmConfig &cmm_config);
    int32 Delete(const int32  bureau,const int32  rack,const int32  shelf);
    int32 Search(vector<CmmConfig> &cmm_infos,const string  where);
    int32 Count();
    string GetCmmIP(const int32  bureau,const int32  rack,const int32  shelf);
    //string GetCmmStringIP(const int32  bureau,const int32  rack,const int32  shelf);
    string GetShelfType(const int32  bureau,const int32  rack,const int32  shelf);


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
        BUREAU         = 0,
        RACK           = 1,
        SHELF          = 2,
        TYPE           = 3,
        IP_ADDRESS     = 4,
        DESCRIPTION    = 5
    };

    static const char *_table;
    static const char *_db_names;
    static SqlDB      *_db;

    static ConfigCmmPool *_instance;
    ConfigCmmPool() {}
    ConfigCmmPool(SqlDB *pDb);

    DISALLOW_COPY_AND_ASSIGN(ConfigCmmPool);
};

}  /* end namespace zte_tecs */

#endif /* end HM_CONFIG_TCU_H */

