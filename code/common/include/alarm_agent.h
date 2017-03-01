/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�alarm_agent.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��AlarmAgent�ඨ���ļ�
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
 *                        ��������                         *
***********************************************************/
#define     MAX_AGT_ALARM               (WORD16) 2000 

/* ucWobbleFlag��־ */
#define     OAM_ALARM_NOWOBBLE      (BYTE)0   /* �޷������� */
#define     OAM_ALARM_WOBBLEEND     (BYTE)1   /* �������� */

/* ucCheckWobble��־ */
#define     OAM_ALARM_CHECKSCHE     (BYTE)0   /* ����� */
#define     OAM_ALARM_WOBBLESCHE    (BYTE)1   /* ���������� */

/* CRC */
#define  PRE_CRC                    (WORD32)0xFFFFFFFF

#define     ALARM_S2M_SYNC_FORBID           1
#define     ALARM_S2M_SYNC_PERMIT           0

#define SYNC_LOGIC        (BYTE)0xF0
#define SYNC_PHYSIC       (BYTE)0x0F

#define ALARM_AGENT_SEND_INFORM 1000
#define TIMER_POWERON_SYNC 300000   /* �����ϵ�5�����ڲ�����ͬ�� */
#define TIMER_A2MSYNC 540000    /* ����9���������ϱ��澯 */

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

/* �������������ƫ�����ڴ�������Ҫ�õ�����˽��������������ڽṹ����ǰ�������ڼ��㣬�������ṹ�мɲ�Ҫ����ǰ������ */
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
    WORD32               dwAlarmCode;              /* �澯�� */
    char strSubSystem[ALARM_ADDRESS_LENGTH];
    char strLevel1[ALARM_ADDRESS_LENGTH];  //TC����
    char strLevel2[ALARM_ADDRESS_LENGTH];  //CC����    
    char strLevel3[ALARM_ADDRESS_LENGTH];  //HC����
    WORD16               wObjType;                 /* ϵͳ�������� */
    WORD16               wKeyLen;                  /* �澯������Ϣ�ṹ�ؼ��ֳ��� */
}T_AlmCRCHead;

typedef struct 
{
    T_AlmCRCHead      tAlarmCrcHead;            /* CRCУ����ͷ */
    BYTE                 ucData[ALARM_ADDINFO_UNION_MAX];              /* ������Ϣ�ؼ��֣��ݶ�120 */
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
        //������Ϣ��Ԫ�����߳�
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
    BYTE _ucPowerFlag;              /* �ϵ�10���ڲ�����ͬ�� 0-����ͬ�� 1-��ֹͬ�� */
    BYTE _ucFrameNumber;
    string _strReportAddr;
    T_AgtAlarm    _tReport;
    BOOLEAN   _bIsAlarmPoolFull;            /* �澯������־ */
    BYTE _gM2ASyncFlag;  /*����λ��ʾ�߼��澯�Ƿ�ͬ��������λ��ʾ����澯�Ƿ�ͬ������1��ʾ�Ѿ�ͬ��*/
    BYTE _gVisitFlag;
    WORD32  _dwCurAid;                    /* ���嵱ǰ����ˮ����Ϣ */
    map <string,T_AlarmPool> _pool;

};
}
#endif
