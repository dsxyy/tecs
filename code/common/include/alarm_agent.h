/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：alarm_agent.h
* 文件标识：见配置管理计划书
* 内容摘要：AlarmAgent类定义文件
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
#ifndef ALARM_AGENT_H
#define ALARM_AGENT_H
#include "oam_pub_type.h"
#include "oam_alarm_pub.h"
#include "oam_crc32.h"
#include "event.h"
#include "mid.h"
#include "alarm_report.h"
#include "alarm_agent_report.h"
#include "alarm_sync.h"
#include "log_agent.h"
#include "registered_system.h"
#include "sky.h"

namespace zte_tecs
{
/***********************************************************
 *                        常量定义                         *
***********************************************************/
#define     MAX_AGT_ALARM               (WORD16) 2000 

/* ucWobbleFlag标志 */
#define     OAM_ALARM_NOWOBBLE      (BYTE)0   /* 无防抖控制 */
#define     OAM_ALARM_WOBBLEEND     (BYTE)1   /* 防抖结束 */

/* ucCheckWobble标志 */
#define     OAM_ALARM_CHECKSCHE     (BYTE)0   /* 检测期 */
#define     OAM_ALARM_WOBBLESCHE    (BYTE)1   /* 防抖控制期 */

/* CRC */
#define  PRE_CRC                    (WORD32)0xFFFFFFFF

#define     ALARM_S2M_SYNC_FORBID           1
#define     ALARM_S2M_SYNC_PERMIT           0

#define SYNC_LOGIC        (BYTE)0xF0
#define SYNC_PHYSIC       (BYTE)0x0F

#define ALARM_AGENT_SEND_INFORM 1000
#define TIMER_POWERON_SYNC 300000   /* 单板上电5分钟内不允许同步 */
#define TIMER_A2MSYNC 540000    /* 单板9分钟主动上报告警 */

typedef struct 
{
    BYTE                 ucWobbleAlmStatus;
    BYTE                 ucCheckWobble;
    WORD16               wWobbleMax;
    WORD16               wCheckNum;
    WORD16               wCheckTTL;
    WORD16               wSendNum;
    WORD32               dwWobbleTTL;
    T_PAD                ucPad[4];
    Datetime            tRestoreTime;
}T_AlarmWobble;

/* 二叉树和链表的偏移量在代码中需要用到，因此将这两个变量放在结构的最前方，便于计算，若新增结构切忌不要在最前面增加 */
typedef struct 
{
    T_AlarmInfo          tAlarmInfo;
    T_AlarmWobble        tAlarmWobble;
    WORD32               dwPPAid;
    BYTE                 ucSyncFlag;
    BYTE                 ucErrorType;
    BYTE                 ucAlarmType;
    T_PAD                ucPad;
}T_AlarmPool;

typedef struct
{
    WORD32             dwAlarmCode;
    WORD32             dwCRCCode[4];
}T_SyncData;

typedef struct 
{
    WORD32               dwAlarmCode;              /* 告警码 */
    char strSubSystem[ALARM_ADDRESS_LENGTH];
    char strLevel1[ALARM_ADDRESS_LENGTH];  //TC名称
    char strLevel2[ALARM_ADDRESS_LENGTH];  //CC名称    
    char strLevel3[ALARM_ADDRESS_LENGTH];  //HC名称
    WORD16               wObjType;                 /* 系统对象类型 */
    WORD16               wKeyLen;                  /* 告警附加信息结构关键字长度 */
}T_AlmCRCHead;

typedef struct 
{
    T_AlmCRCHead      tAlarmCrcHead;            /* CRC校验码头 */
    BYTE                 ucData[ALARM_ADDINFO_UNION_MAX];              /* 附加信息关键字，暂定120 */
}T_CRCAlmData;

class AlarmAgent : public MessageHandler
{
public:
    virtual ~AlarmAgent();
    STATUS Receive(const MessageFrame& message);
    void Send(const AlarmReport& alarm);
    void AlarmShowAlarmPool();
    
    const string& GetReportAddr() const 
    {
        return _strReportAddr;
    };
    
    static AlarmAgent *GetInstance()
    {
        if(NULL == instance)
        {
            instance = new AlarmAgent();
        }
        return instance;
    };

    STATUS Init()
    {
        //启动消息单元工作线程
        STATUS ret = StartMu(ALARM_AGENT);
        if(SUCCESS != ret)
        {
            return ret;
        }
        return Start();
    }
    
    virtual void MessageEntry(const MessageFrame& message);

private:
    STATUS StartMu(const string& name);
    AlarmAgent();
    DISALLOW_COPY_AND_ASSIGN(AlarmAgent);
    STATUS Start();
    void AlarmAReceiveAlarm(T_AlarmReport *pAlarmReport, T_AlarmAddress *ptAlarmLocation);
    void AlarmAReceiveRestore(T_AlarmReport *pAlarmReport, T_AlarmAddress *ptAlarmLocation);
    void AlarmAConstructReport(T_AlarmPool *pAlarmPoolInfo);
    void AlarmAConstructRestore(T_AlarmPool *pAlarmPoolInfo,BYTE ucRestoreType);
    void AlarmAPoolFull();
    BYTE OamAlarmLibSaveInfo(T_AlarmInfo *pPoolAlarmInfo, T_AlarmReport *pAlarmReport);
    void AlarmARecAidSync(AlarmSync as);
    void AlarmACompareOmpSynAlarmID( AlarmSync &as);
    void AlarmATravelAlarmPool();
    void AlarmATravelAllAlarmPool();
    void AlarmASendInform(const AlarmReport &inform);
    void  AlarmASendAlarm();
    string& AlarmACRC2Key(string& str,  T_SyncData *data);
    STATUS JoinGroup();
    STATUS AlarmASetTimer(uint32  type,  uint32    duration, uint32    msgid);
    static AlarmAgent *instance;
    MessageUnit *mu;
    BYTE _ucPowerFlag;              /* 上电10秒内不允许同步 0-允许同步 1-禁止同步 */
    BYTE _ucFrameNumber;
    string _strReportAddr;
    T_AgtAlarm    _tReport;
    BOOLEAN   _bIsAlarmPoolFull;            /* 告警池满标志 */
    BYTE _gM2ASyncFlag;  /*高四位表示逻辑告警是否同步，低四位表示物理告警是否同步，置1表示已经同步*/
    BYTE _gVisitFlag;
    WORD32  _dwCurAid;                    /* 本板当前的流水号信息 */
    map <string,T_AlarmPool> _pool;

};
}
#endif
