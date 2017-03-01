/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：history_alarm_pool.h
* 文件标识：见配置管理计划书
* 内容摘要：HistoryAlarmPoll类定义文件
* 当前版本：1.0
* 作    者：马兆勉
* 完成日期：2011年7月27日
*
* 修改记录1：
*     修改日期：2011/7/27
*     版 本 号：V1.0
*     修 改 人：马兆勉
*     修改内容：创建
*******************************************************************************/


#ifndef ALARM_HISTORY_ALARM_POOL_H
#define ALARM_HISTORY_ALARM_POOL_H

#include "sky.h"
#include "sqldb.h"
#include "pool_sql.h"
#include "history_alarm.h"
#include "alarm_api.h"
#include "pool_object_sql.h"

//#include <time.h>
//#include <sstream>

//#include <iostream>

//#include <vector>

// 禁用拷贝构造宏定义
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)
#endif

using namespace std;


namespace zte_tecs
{

class HistoryAlarmPool : public SqlCallbackable
{

public:

    // 数据库字段定义
    enum ColNames
    {
        OID            = 0,   // 对象标识
        ALARMID        = 1,   // 告警流水号
        ALARMCODE      = 2,   // 告警码
        ALARMTIME      = 3,   // 告警原因
        ALARMADDR      = 4,   // 告警类型
        LEVEL1         = 5,   // 
        LEVEL2         = 6,   // 
        LEVEL3         = 7,   // 
        LOCATION       = 8,   // 告警位置
        ADDINFO        = 9,   // 
        RESTORETIME    = 10,  // 告警恢复时间
        RESTORETYPE    = 11,  // 告警恢复类型
        SUBSYSTEM      = 12,
        LIMIT          = 13
    };


    ~HistoryAlarmPool(){};         // 默认析构函数

    STATUS Init();

    /**************************************************************************/
    /**
    @brief 功能描述: 分配HistoryAlarm对象

    @li @b 入参列表：
    @verbatim
        alarm    HistoryAlarm对象指针
    @endverbatim

    @li @b 出参列表：
    @verbatim
    @endverbatim

    @li @b 返回值列表：
    @verbatim
        0           成功
        1          失败
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    /**************************************************************************/
    STATUS Allocate(HistoryAlarm &alarm);

    //单个接口查询
    STATUS Get(int64 oid, HistoryAlarm &history_alarm);

    //批量查询
    STATUS Show(vector<ApiHistoryAlarmInfo> &vec_api_alarm, const string& where);


    /**************************************************************************/
    /**
    @brief 功能描述: 从数据库删除HistoryAlarm对象

    @li @b 入参列表：
    @verbatim
        alarm    HistoryAlarm对象指针
    @endverbatim

    @li @b 出参列表：
    @verbatim
        无
    @endverbatim

    @li @b 返回值列表：
    @verbatim
        0           成功
        -1          失败
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    /**************************************************************************/
    STATUS Drop(const HistoryAlarm &alarm);

    /**************************************************************************/
    /**
    @brief 功能描述: 获取HistoryAlarmPool对象实例

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
        HistoryAlarmPool对象实例
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    /**************************************************************************/    
    static HistoryAlarmPool *GetInstance()
    {
        SkyAssert(NULL != instance);
        return instance;
    };

    /**************************************************************************/
    /**
    @brief 功能描述: 创建或者获取HistoryAlarmPool对象实例

    @li @b 入参列表：
    @verbatim
        db 连接的数据库对象指针
    @endverbatim

    @li @b 出参列表：
    @verbatim
        无
    @endverbatim

    @li @b 返回值列表：
    @verbatim
        HistoryAlarmPool对象实例
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    /**************************************************************************/   
    static HistoryAlarmPool* CreateInstance(SqlDB *db)
    {
        if(NULL == instance)
        {
            instance = new HistoryAlarmPool(db); 
        }
        
        return instance;
    };

    /**************************************************************************/
    /**
    @brief 功能描述: 更新HistoryAlarm对象到数据库

    @li @b 入参列表：
    @verbatim
        alarm    HistoryAlarm对象指针
    @endverbatim

    @li @b 出参列表：
    @verbatim
        无
    @endverbatim

    @li @b 返回值列表：
    @verbatim
        0    成功
        -1   失败
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    /**************************************************************************/
    STATUS Update(const HistoryAlarm &alarm); 

    static void Lock()
    {
        pthread_mutex_lock(&_mutex);
    };

    static void UnLock()
    {
        pthread_mutex_unlock(&_mutex);
    };


private:

    HistoryAlarmPool(SqlDB *pDb):SqlCallbackable(pDb){};   // 默认构造函数
    DISALLOW_COPY_AND_ASSIGN(HistoryAlarmPool);
    static HistoryAlarmPool *instance;

    int GetHistoryAlarmCallback(void *nil, SqlColumn *columns);
    int ShowHistoryAlarmCallback(void * arg, SqlColumn * columns);


    //最大对象标识
    static int64 _lastOID;
    static pthread_mutex_t  _mutex;

protected:

    static const char* _db_names;       // 数据库字段定义
    static const char* _table;          // 数据库表定义

};

}  // namespace zte_tecs
#endif // #ifndef ALARM_HISTORY_ALARM_POOL_H

