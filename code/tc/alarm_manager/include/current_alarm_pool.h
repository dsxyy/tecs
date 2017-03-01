/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：current_alarm_pool.h
* 文件标识：见配置管理计划书
* 内容摘要：CurrentAlarmPoll类定义文件
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


#ifndef ALARM_CURRENT_ALARM_POOL_H
#define ALARM_CURRENT_ALARM_POOL_H

#include "sky.h"
#include "sqldb.h"
#include "pool_sql.h"
#include "current_alarm.h"

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

#define SYNC_AGENT_MAX_NUMBER     200

typedef struct
{
    unsigned int   AlarmCode;       // 告警码
    unsigned int   dwCRCCode0;    // CRC校验码
    unsigned int   dwCRCCode1;    // CRC校验码
    unsigned int   dwCRCCode2;    // CRC校验码
    unsigned int   dwCRCCode3;    // CRC校验码
    time_t         AlarmTime;    //告警时间
    string         Location;  //告警位置
    string         AddInfo;	       //告警附加信息
}T_SyncCurrentAlarm;

typedef struct
{
    unsigned int          dwAlarmNum;                 /* 告警个数 */
    T_SyncCurrentAlarm    tCurrentAlarm[SYNC_AGENT_MAX_NUMBER];
}T_CurrentAlarmPool;


class CurrentAlarmPool : public PoolSQL
{

public:
    ~CurrentAlarmPool(){};         // 默认析构函数

    /**************************************************************************/
    /**
    @brief 功能描述: 分配CurrentAlarm对象

    @li @b 入参列表：
    @verbatim
        os          CurrentAlarm对象转储XML格式输出字符串
    @endverbatim

    @li @b 出参列表：
    @verbatim
        error_str   错误信息
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
    int64 Allocate(int64* oid, const T_CurrentAlarm& cur_alarm, string& error_str);

    /**************************************************************************/
    /**
    @brief 功能描述: 根据数据库记录ID获取CurrentAlarm对象

    @li @b 入参列表：
    @verbatim
        oid         数据库记录ID
        lock        是否加锁
    @endverbatim

    @li @b 出参列表：
    @verbatim
        无
    @endverbatim

    @li @b 返回值列表：
    @verbatim
        CurrentAlarm对象指针
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    /**************************************************************************/
    CurrentAlarm * Get(int64 oid, bool lock)
    {
        CurrentAlarm * alarm = static_cast<CurrentAlarm *>(PoolSQL::Get(oid,lock));

        return alarm;
    };

    /**************************************************************************/
    /**
    @brief 功能描述: 根据告警关键字获取CurrentAlarm对象

    @li @b 入参列表：
    @verbatim
        alarmkey        告警关键字
        lock            是否加锁
    @endverbatim

    @li @b 出参列表：
    @verbatim
        无
    @endverbatim

    @li @b 返回值列表：
    @verbatim
        CurrentAlarm对象指针
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    /**************************************************************************/
    CurrentAlarm * Get(string& alarmkey, bool lock)
    {
        map<string, int64>::iterator index;

        index = _known_alarms.find(alarmkey);

        if (_known_alarms.end() != index)
        {
            return Get((int64)index->second,lock);
        }

        return 0;
    };

    /**************************************************************************/
    /**
    @brief 功能描述: 更新CurrentAlarm对象到数据库

    @li @b 入参列表：
    @verbatim
        cur_alarm    CurrentAlarm对象指针
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
    int Update(CurrentAlarm* cur_alarm)
    {
        return cur_alarm->Update(_db);
    };

    /**************************************************************************/
    /**
    @brief 功能描述: 从数据库删除CurrentAlarm对象

    @li @b 入参列表：
    @verbatim
        cur_alarm    CurrentAlarm对象指针
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
    int Drop(CurrentAlarm* cur_alarm)
    {
        int rc = PoolSQL::Drop(cur_alarm);

        if (0 == rc)
        {
            _known_alarms.erase(cur_alarm->get_alarmkey());
        }

        return rc;
    };
  
    /**************************************************************************/
    /**
    @brief 功能描述: 启动CurrentAlarm数据库初始化

    @li @b 入参列表：
    @verbatim
        db    SQL数据库指针
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
    int Init(SqlDB* db)
    {
        _known_alarms.clear();
        CurrentAlarm* cur_alarm = new CurrentAlarm();
        int rc = cur_alarm->DeleteTable(db);
        delete cur_alarm;
        return rc;
    };
    
    /**************************************************************************/
    /**
    @brief 功能描述: 转储所有CurrentAlarm对象

    @li @b 入参列表：
    @verbatim
        oss     对象转储字符串流
        where   位置字符串
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
    int Dump(ostringstream& oss, const string& where);
    
    
    /**************************************************************************/
    /**
    @brief 功能描述: 转储所有CurrentAlarm对象

    @li @b 入参列表：
    @verbatim
        tCurrentAlarmPool     对象转储结构
        where   位置字符串
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
    int Dump(vector<xmlrpc_c::value>& arrayAlarm, const string& where);

    /**************************************************************************/
    /**
    @brief 功能描述: 获取CurrentAlarmPool对象实例

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
        CurrentAlarmPool对象实例
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    /**************************************************************************/    
    static CurrentAlarmPool *GetInstance()
    {
        return instance;
    };

     /**************************************************************************/
    /**
    @brief 功能描述: 创建或者获取CurrentAlarmPool对象实例

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
        CurrentAlarmPool对象实例
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    /**************************************************************************/    
	static CurrentAlarmPool* CreateInstance(SqlDB *db)
    {
        if(NULL == instance)
        {
            instance = new CurrentAlarmPool(db); 
        }
        
        return instance;
    };
    
    void ShowMemoryAlarm();

private:

    map<string, int64> _known_alarms;  //告警关键字-数据库记录ID映射
    CurrentAlarmPool(SqlDB* db);   // 默认构造函数
    DISALLOW_COPY_AND_ASSIGN(CurrentAlarmPool);
    static CurrentAlarmPool *instance;

    /**************************************************************************/
    /**
    @brief 功能描述: PoolSQL虚拟接口实现

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
        PoolObjectSQL对象指针
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    /**************************************************************************/
    PoolObjectSQL * Create()
    {
        return  new CurrentAlarm();
    };

    /**************************************************************************/
    /**
    @brief 功能描述: 对象转储控制回调函数

    @li @b 入参列表：
    @verbatim
        num         参数总数
        values      参数取值数组指针
        names       字段名称数组指针
    @endverbatim

    @li @b 出参列表：
    @verbatim
      oss         ostringstream对象指针
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
    int DumpCallback(void* oss, SqlColumn* columns);
    
    /**************************************************************************/
    /**
    @brief 功能描述: 对象转储控制回调函数

    @li @b 入参列表：
    @verbatim
        num         参数总数
        values      参数取值数组指针
        names       字段名称数组指针
    @endverbatim

    @li @b 出参列表：
    @verbatim
      T_CurrentAlarmPool         pCurrentAlarmPool告警池结构指针
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
    int DumpStructCallback(void *arrayAlarm, SqlColumn* columns);

    /**************************************************************************/
    /**
    @brief 功能描述: CurrentAlarm数据库初始化回调函数

    @li @b 入参列表：
    @verbatim
        num         参数总数
        values      参数取值数组指针
        names       字段名称数组指针    @endverbatim
        @li @b 出参列表：
    @verbatim
        nil         无指针
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
    int InitCallback(void* nil, SqlColumn* columns);

};

}  // namespace zte_tecs
#endif // #ifndef ALARM_CURRENT_ALARM_POOL_H
