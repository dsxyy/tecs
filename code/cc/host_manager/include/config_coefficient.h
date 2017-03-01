/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：config_coefficient.h
* 文件标识：见配置管理计划书
* 内容摘要：config_coefficient类的声明文件
* 当前版本：1.0
* 作    者：刘毅
* 完成日期：2012年3月21日
*
* 修改记录1：
*     修改日期：2012年3月21日
*     版 本 号：V1.0
*     修 改 人：
*     修改内容：创建
*******************************************************************************/
#ifndef HM_CONFIG_COEFFICIENT_H
#define HM_CONFIG_COEFFICIENT_H
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

/**
@brief 功能描述: 主机类定义
@li @b 其它说明：无
*/
class config_coefficient: public Callbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static config_coefficient *GetInstance()
    {
        return _instance;
    }

    static config_coefficient* CreateInstance(SqlDB *db)
    {
        if(NULL == _instance)
        {
            _instance = new config_coefficient(db); 
        }
        
        return _instance;
    };
    virtual ~config_coefficient() { _instance = NULL; }

    int32 Set(const string  &coefficient_type, 
               int32 coefficient_value, 
               const string &description);


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
        TECS_BENCH      = 0,
        DESCRIPTION     = 1,
        LIMIT           = 2
    };

    static const char *_table;
    static const char *_db_names;
    static const char *_view;
    static SqlDB      *_db;

    static config_coefficient *_instance;
    config_coefficient();
    config_coefficient(SqlDB *pDb);
    int32 InsertReplace(const string &coefficient_type, int32 coefficient_value, 
                        const string &description, bool replace);

    DISALLOW_COPY_AND_ASSIGN(config_coefficient);
};

}  /* end namespace zte_tecs */

#endif /* end CONFIG_COEFFIEIENT_H */

