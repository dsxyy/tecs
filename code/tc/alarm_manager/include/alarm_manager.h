/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�alarm_manager.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��AlarmManager�ඨ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�������
* ������ڣ�2011��7��27��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/7/27
*     �� �� �ţ�V1.0
*     �� �� �ˣ�������
*     �޸����ݣ�����
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
#define ALARM_MAMANGER_MA_SYNC   180000  /* 3����ͬ���澯��ʱ�� */

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

