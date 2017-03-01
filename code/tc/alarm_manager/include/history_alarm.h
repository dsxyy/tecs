/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：history_alarm.h
* 文件标识：见配置管理计划书
* 内容摘要：HistoryAlarm类定义文件
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


#ifndef ALARM_HISTORY_ALARM_H
#define ALARM_HISTORY_ALARM_H

#include "sky.h"
#include "alarm_api.h"
#include "tecs_pub.h"

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

class HistoryAlarm : public Serializable
{
    friend class HistoryAlarmPool;

public:

    //对象唯一标识
    int64  _oid;


    int        AlarmID;         // 告警流水号
    int        AlarmCode;       // 告警码
    int        AlarmTime;       // 告警时间
    string     AlarmAddr;       // 告警位置
    string     SubSystem;
    string     Level1;          
    string     Level2;          
    string     Level3;          
    string     Location;        // 告警位置描述信息    
    string     AddInfo;         // 告警附加信息
    int        RestoreTime;     // 恢复时间
    int        RestoreType;     // 告警恢复类型


   // 默认构造函数
    HistoryAlarm()
    {
        _oid = INVALID_OID;
        AlarmID = 0;
        AlarmCode = 0;
        AlarmTime = 0;
        RestoreTime = 0;
        RestoreType = 0;
    };

    HistoryAlarm(int aAlarmID,
                    int aAlarmCode,
                    int aAlarmTime,
                    const string&  aAlarmAddr,
                    const string&  aSubSystem,
                    const string&  aLevel1,
                    const string&  aLevel2,
                    const string&  aLevel3,
                    const string&  aLocation,
                    const string&  aAddInfo,
                    int         aRestoreTime,
                    int  aRestoreType)
    {
        AlarmID = aAlarmID;
        AlarmCode = aAlarmCode;
        AlarmTime = aAlarmTime;
        AlarmAddr = aAlarmAddr;
        SubSystem = aSubSystem;
        Level1 = aLevel1;
        Level2 = aLevel2;
        Level3 = aLevel3;
        Location = aLocation;
        AddInfo = aAddInfo;
        RestoreTime = aRestoreTime;
        RestoreType = aRestoreType;
    };

    // 默认析构函数
    ~HistoryAlarm(){};

    SERIALIZE
    {
        SERIALIZE_BEGIN(HistoryAlarm);
        WRITE_VALUE(AlarmID);
        WRITE_VALUE(AlarmCode);
        WRITE_VALUE(AlarmTime);
        WRITE_VALUE(AlarmAddr);
        WRITE_VALUE(SubSystem);
        WRITE_VALUE(Level1);        
        WRITE_VALUE(Level2);        
        WRITE_VALUE(Level3);        
        WRITE_VALUE(Location);
        WRITE_VALUE(AddInfo);
        WRITE_VALUE(RestoreTime);
        WRITE_VALUE(RestoreType);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(HistoryAlarm);
        READ_VALUE(AlarmID);
        READ_VALUE(AlarmCode);
        READ_VALUE(AlarmTime);
        READ_VALUE(AlarmAddr);
        READ_VALUE(SubSystem);
        READ_VALUE(Level1);        
        READ_VALUE(Level2);        
        READ_VALUE(Level3);        
        READ_VALUE(Location);
        READ_VALUE(AddInfo);
        READ_VALUE(RestoreTime);
        READ_VALUE(RestoreType);        
        DESERIALIZE_END();
    };

private:

    // 禁用拷贝构造
    DISALLOW_COPY_AND_ASSIGN(HistoryAlarm);

};

}  //namespace zte_tecs

#endif // #ifndefALARM_HISTORY_ALARM_H
