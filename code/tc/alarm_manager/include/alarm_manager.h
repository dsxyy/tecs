/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：alarm_manager.h
* 文件标识：见配置管理计划书
* 内容摘要：AlarmManager类定义文件
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

#ifndef ALARM_MANAGER_H
#define ALARM_MANAGER_H
#include "event.h"
#include "mid.h"
#include "current_alarm.h"
#include "current_alarm_pool.h"
#include "history_alarm.h"
#include "history_alarm_pool.h"
#include "alarm_agent_report.h"
#include "alarm_sync.h"
#include "sky.h"

#define OAM_ALARM_RECENT_INFORM_MAX  1000
#define ALARM_MAMANGER_MA_SYNC   180000  /* 3分钟同步告警定时器 */

namespace zte_tecs
{
class AlarmManager : public MessageHandler
{
public:
	virtual ~AlarmManager();
	STATUS Receive(const MessageFrame& message);
	void SendAlarmMsg(uint alarmcode,int flag);
    
    static AlarmManager *GetInstance()
    {
        if(NULL == instance)
        {
            instance = new AlarmManager();
        }
        return instance;
    };

    STATUS Init()
    {
        _cap = CurrentAlarmPool::GetInstance();
        _hap = HistoryAlarmPool::GetInstance();       
        if (!_cap)
        {
            OutPut(SYS_ERROR,"_cap is NULL!\n");
            return ERROR_NOT_READY;
        }

        if (!_hap)
        {
            OutPut(SYS_ERROR,"_hap is NULL!\n");
            return ERROR_NOT_READY;
        }

        STATUS ret = StartMu(ALARM_MANAGER);
        if(SUCCESS != ret)
        {
            return ret;
        }
        return Start();
    }
    void AlarmMShowRecentInform(void);
    virtual void MessageEntry(const MessageFrame& message);

private:
    AlarmManager();
    STATUS StartMu(const string& name);
    STATUS Start();

    DISALLOW_COPY_AND_ASSIGN(AlarmManager);
    void AlarmMReceiveAlarm(AgentAlarm &alarmmsg);
    string& alarmkey_to_str(string& str, const AgentAlarm &alarmmsg);
    void AlarmMOmpSyncPP();
    void AlarmMFindBrdAlarm(const string &BoardAddr);
    STATUS JoinGroup();
    void BroadcastPoweronMsg();
    void AlarmMAddRecentInform(AgentAlarm &informmsg);
    static AlarmManager *instance;
    MessageUnit *mu;
    CurrentAlarmPool *_cap;
    HistoryAlarmPool *_hap;
    WORD32 _dwAlarmID;
    vector<T_AgtInform> _informpool;
};
}
#endif

