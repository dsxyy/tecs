/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：current_alarm.h
* 文件标识：见配置管理计划书
* 内容摘要：CurrentAlarm类定义文件
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


#ifndef ALARM_CURRENT_ALARM_H
#define ALARM_CURRENT_ALARM_H

#include "sky.h"
#include "sqldb.h"
#include "pool_object_sql.h"
#include "tecs_rpc_method.h"

//#include <sstream>
//#include <ctime>

// 禁用拷贝构造宏定义
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)
#endif

using namespace std;


namespace zte_tecs
{

// T_CurrentAlarm数据结构定义
struct T_CurrentAlarm
{
    unsigned int   AlarmID;         // 告警流水号
    unsigned int   AlarmCode;       // 告警码
    time_t         AlarmTime;       // 告警时间
    string         AlarmAddr;        // 告警发送位置
    string         SubSystem;
    string         Level1;           
    string         Level2;          
    string         Level3;          
    string         Location;     // 告警位置描述信息
    string         AddInfo;         // 告警附加信息
    string         AlarmKey;        // 告警关键字, 告警代理根据告警附加信息计算
    unsigned int   dwCRCCode0;      // CRC校验码
    unsigned int   dwCRCCode1;      // CRC校验码
    unsigned int   dwCRCCode2;      // CRC校验码
    unsigned int   dwCRCCode3;      // CRC校验码
    
    // 构造方法

    T_CurrentAlarm (const T_CurrentAlarm& ca) :
                    AlarmID(ca.AlarmID),
                    AlarmCode(ca.AlarmCode),
                    AlarmTime(ca.AlarmTime),
                    AlarmAddr(ca.AlarmAddr),
                    SubSystem(ca.SubSystem),
                    Level1(ca.Level1),
                    Level2(ca.Level2),
                    Level3(ca.Level3),
                    Location(ca.Location),
                    AddInfo(ca.AddInfo),
                    AlarmKey(ca.AlarmKey),
                    dwCRCCode0(ca.dwCRCCode0),
                    dwCRCCode1(ca.dwCRCCode1),
                    dwCRCCode2(ca.dwCRCCode2),
                    dwCRCCode3(ca.dwCRCCode3){};

    // 构造方法
    T_CurrentAlarm (unsigned int   aAlarmID = 0,
                    unsigned int   aAlarmCode = 0,
                    time_t         aAlarmTime = 0,
                    const string&  aAlarmAddr = "",
                    const string&  aSubSystem = "",
                    const string&  aLevel1 = "",
                    const string&  aLevel2 = "",
                    const string&  aLevel3 = "",
                    const string&  aLocation = "",
                    const string&  aAddInfo = "",
                    const string&  aAlarmKey = "",
                    unsigned int   aCRCCode0 = 0,
                    unsigned int   aCRCCode1 = 0,
                    unsigned int   aCRCCode2 = 0,
                    unsigned int   aCRCCode3 = 0) :
                    AlarmID(aAlarmID),
                    AlarmCode(aAlarmCode),
                    AlarmTime(aAlarmTime),
                    AlarmAddr(aAlarmAddr),
                    SubSystem(aSubSystem),
                    Level1(aLevel1),
                    Level2(aLevel2),
                    Level3(aLevel3),
                    Location(aLocation),
                    AddInfo(aAddInfo),
                    AlarmKey(aAlarmKey),
                    dwCRCCode0(aCRCCode0),
                    dwCRCCode1(aCRCCode1),
                    dwCRCCode2(aCRCCode2),
                    dwCRCCode3(aCRCCode3){};

};


class CurrentAlarm : public PoolObjectSQL
{

public:

    /**************************************************************************/
    /**
    @brief 功能描述: CurrentAlarm对象转储XML格式操作符

    @li @b 入参列表：
    @verbatim
        os    CurrentAlarm对象转储XML格式输出字符串
    @endverbatim

    @li @b 出参列表：
    @verbatim
        无
    @endverbatim

    @li @b 返回值列表：
    @verbatim
        CurrentAlarm对象转储XML格式输出字符串
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    /**************************************************************************/
    friend ostream& operator<<(ostream& os, CurrentAlarm& alarm);


    /**************************************************************************/
    /**
    @brief 功能描述: 转储CurrentAlarm对象到格式化字符串

    @li @b 入参列表：
    @verbatim
        xml   CurrentAlarm对象格式化字符串
    @endverbatim

    @li @b 出参列表：
    @verbatim
        无
    @endverbatim

    @li @b 返回值列表：
    @verbatim
        CurrentAlarm对象格式化字符串
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    /**************************************************************************/
    string& to_str(string& str) const;

    /**************************************************************************/
    /**
    @brief 功能描述: 转储CurrentAlarm对象到XML字符串

    @li @b 入参列表：
    @verbatim
        xml   CurrentAlarm对象XML字符串
    @endverbatim

    @li @b 出参列表：
    @verbatim
        无
    @endverbatim

    @li @b 返回值列表：
    @verbatim
        CurrentAlarm对象XML字符串
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    /**************************************************************************/
    string& to_xml(string& xml) const;

    /**************************************************************************/
    /**
    @brief 功能描述: 获取CurrentAlarm对象数据库记录标识

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
        CurrentAlarm对象数据库记录标识
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    /**************************************************************************/
    int64 get_id() const
    {
        return _oid;
    };
    
    /**************************************************************************/
    /**
    @brief 功能描述: 获取CurrentAlarm对象告警信息结构

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
        T_CurrentAlarm结构
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    /**************************************************************************/
    const T_CurrentAlarm & get_current_alarm() const
    {
        return _current_alarm;
    }
    
    /**************************************************************************/
    /**
    @brief 功能描述: 设置CurrentAlarm对象告警信息

    @li @b 入参列表：
    @verbatim
        current_alarm  T_CurrentAlarm结构
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
    /**************************************************************************/
    void set_current_alarm(const T_CurrentAlarm & current_alarm)
    {
        _current_alarm = current_alarm;
    }

    /**************************************************************************/
    /**
    @brief 功能描述: 获取CurrentAlarm对象告警关键字

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
        CurrentAlarm对象告警关键字
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    /**************************************************************************/
    const string& get_alarmkey() const
    {
        return _current_alarm.AlarmKey;
    };

protected:

   // 默认构造函数
    CurrentAlarm(int64 id = -1, const T_CurrentAlarm& current_alarm = 0);
    // 默认析构函数
    virtual ~CurrentAlarm(){};

    // 数据库字段定义
    enum ColNames
    {
        OID            = 0,   // 对象标识
        ALARMID        = 1,   // 告警流水号
        ALARMCODE      = 2,   // 告警码
        ALARMTIME      = 3,   // 告警时间
        ALARMADDR      = 4,   // 告警位置
        LEVEL1         = 5,   
        LEVEL2         = 6,   
        LEVEL3         = 7,   
        LOCATION       = 8,   
        ADDINFO        = 9,   // 告警附加信息
        ALARMKEY       = 10,   // 告警关键字
        CRCCODE0       = 11,   // CRC校验码0
        CRCCODE1       = 12,  // CRC校验码1	
        CRCCODE2       = 13,  // CRC校验码2
        CRCCODE3       = 14,  // CRC校验码3
        SUBSYSTEM      = 15,
        LIMIT          = 16
    };

    static const char* _db_names;       // 数据库字段定义
    static const char* _table;          // 数据库表定义

    /**************************************************************************/
    /**
    @brief 功能描述: 从数据库读取CurrentAlarm对象

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
    virtual int Select(SqlDB* db);

    /**************************************************************************/
    /**
    @brief 功能描述: 插入CurrentAlarm对象到数据库

    @li @b 入参列表：
    @verbatim
        db         SQL数据库指针
        error_str  错误信息字符串
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
    virtual int Insert(SqlDB* db, string& error_str);

    /**************************************************************************/
    /**
    @brief 功能描述: 更新CurrentAlarm对象到数据库

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
    virtual int Update(SqlDB* db);

    /**************************************************************************/
    /**
    @brief 功能描述: 从数据库删除CurrentAlarm对象

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
    virtual int Drop(SqlDB* db);
    
    /**************************************************************************/
    /**
    @brief 功能描述: 从数据库删除表中所有对象

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
    int DeleteTable(SqlDB* db);

    /**************************************************************************/
    /**
    @brief 功能描述: 转储CurrentAlarm对象到XML字符串

    @li @b 入参列表：
    @verbatim
        columns  数据库字段指针
    @endverbatim

    @li @b 出参列表：
    @verbatim
        oss     转储字符串流
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
    static  int Dump(ostringstream& oss, SqlColumn * columns);
    
        /**************************************************************************/
    /**
    @brief 功能描述: 转储CurrentAlarm对象到T_CurrentAlarm结构

    @li @b 入参列表：
    @verbatim
        columns  数据库字段指针
    @endverbatim

    @li @b 出参列表：
    @verbatim
        T_CurrentAlarm     结构
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
    static  int Dump(T_CurrentAlarm *pCurrentAlarm, SqlColumn * columns);
    
    static  int Dump(vector<xmlrpc_c::value>& arrayAlarm, SqlColumn* columns);

private:

    friend class CurrentAlarmPool;

    T_CurrentAlarm _current_alarm;     // 当前告警结构

    /**************************************************************************/
    /**
    @brief 功能描述: 执行数据库Insert或Replace SQL命令

    @li @b 入参列表：
    @verbatim
        db        SQL数据库指针
        replace   是否替换
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
    int Insert_Replace(SqlDB* db, bool replace);

    /**************************************************************************/
    /**
    @brief 功能描述: 数据库字段到CurrentAlarm对象转换回调函数

    @li @b 入参列表：
    @verbatim
        nil      无效指针
        columns  数据库字段指针
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
    int SelectCallback(void* nil, SqlColumn * columns);

    // 禁用拷贝构造
    DISALLOW_COPY_AND_ASSIGN(CurrentAlarm);
};

}  //namespace zte_tecs

#endif // #ifndefALARM_CURRENT_ALARM_H

