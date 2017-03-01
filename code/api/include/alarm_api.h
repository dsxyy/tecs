/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：alarm_api.h
* 文件标识：
* 内容摘要：ApiHistoryAlarmInfo类定义文件
* 当前版本：1.0
* 作    者：
* 完成日期：
*
* 修改记录1：
*     修改日期：
*     版 本 号：V1.0
*     修 改 人：
*     修改内容：创建
*******************************************************************************/


#ifndef ALARM_API_H
#define ALARM_API_H
#include "rpcable.h"

using namespace std;

namespace zte_tecs
{

class ApiHistoryAlarmInfo : public Rpcable
{
public:
    ApiHistoryAlarmInfo(){};

    ~ApiHistoryAlarmInfo(){};

  TO_RPC
    {
        TO_RPC_BEGIN();

        TO_VALUE(AlarmID);
        TO_VALUE(AlarmCode);
        TO_VALUE(AlarmTime);
        TO_VALUE(AlarmAddr);
        TO_VALUE(SubSystem);
        TO_VALUE(Level1);
        TO_VALUE(Level2);
        TO_VALUE(Level3);
        TO_VALUE(Location);
        TO_VALUE(AddInfo);
        TO_VALUE(RestoreTime);
        TO_VALUE(RestoreType);

        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        
        FROM_VALUE(AlarmID);
        FROM_VALUE(AlarmCode);
        FROM_VALUE(AlarmTime);
        FROM_VALUE(AlarmAddr);
        FROM_VALUE(SubSystem);
        FROM_VALUE(Level1);
        FROM_VALUE(Level2);
        FROM_VALUE(Level3);
        FROM_VALUE(Location);
        FROM_VALUE(AddInfo);
        FROM_VALUE(RestoreTime);
        FROM_VALUE(RestoreType);

        FROM_RPC_END();
    };
    
    //历史告警字段
    int      AlarmID;         // 告警流水号
    int      AlarmCode;       // 告警码
    int      AlarmTime;       // 告警时间
    string   AlarmAddr;       // 告警位置
    string   SubSystem;
    string   Level1;          // TC名称
    string   Level2;          // CC名称
    string   Level3;          // HC名称
    string   Location;        // 告警位置描述信息    
    string   AddInfo;         // 告警附加信息
    int      RestoreTime;     // 恢复时间
    int      RestoreType;     // 告警恢复类型

};

}  //namespace zte_tecs

#endif // #ifndefALARM_API_H
