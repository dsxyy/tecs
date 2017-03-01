/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�alarm_agent.cpp
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��AlarmAgent��ʵ���ļ�
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

#include "alarm_agent.h"

namespace zte_tecs
{
AlarmAgent* AlarmAgent::instance = NULL;

/************************************************************
* �������ƣ� AlarmAgent
* ���������� ���캯��
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2011/10         V1.0       ����         ����
***************************************************************/
AlarmAgent::AlarmAgent()
{
    mu = NULL;
}

/************************************************************
* �������ƣ� StartMu
* ���������� ����mu�ĺ���
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2012/5         V1.0       lixiaocheng         ����
***************************************************************/
STATUS AlarmAgent::StartMu(const string& name)
{
    if(mu)
    {
        SkyAssert(false);
        return ERROR_DUPLICATED_OP;
    }
    
    // ��Ϣ��Ԫ�Ĵ����ͳ�ʼ��
    mu = new MessageUnit(name);
    if (!mu)
    {
        OutPut(SYS_EMERGENCY, "Create mu %s failed!\n",name.c_str());
        return ERROR_NO_MEMORY;
    }

    STATUS ret = mu->SetHandler(this);
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "SetHandler mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }

    ret = mu->Run();
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "Run mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }

    ret = mu->Register();
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "Register mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }

    //���Լ������ϵ���Ϣ����ʹ��Ϣ��Ԫ״̬�л�������̬
    MessageFrame message(SkyInt32(0), EV_POWER_ON);
    ret = mu->Receive(message);
    if (SUCCESS != ret)
    {
        SkyAssert(0);
        return ret;
    }
    mu->Print();
    return SUCCESS;
}

/************************************************************
* �������ƣ� ~AlarmAgent()
* ���������� ��������
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2011/10         V1.0       ����         ����
***************************************************************/
AlarmAgent::~AlarmAgent()
{
    if (mu)
        delete mu;
}

/************************************************************
* �������ƣ� Receive
* ���������� ����������Ϣ�ĺ���
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2011/10         V1.0       ����         ����
***************************************************************/
STATUS AlarmAgent::Receive(const MessageFrame& message)
{
    return mu->Receive(message);
}

/************************************************************
* �������ƣ� JoinGroup
* ���������� ��ALARM_AGENT������Ϣ��
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2011/10         V1.0       ����         ����
***************************************************************/
STATUS AlarmAgent::JoinGroup()
{
    //����һ���鲥��
    return mu->JoinMcGroup(ALARM_GROUP);
}

/************************************************************
* �������ƣ� MessageEntry
* ���������� ��Ϣ������
* ���ʵı� ��
* �޸ĵı� ��
* ��������� message  ��Ϣ��Ϣ
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2011/10         V1.0       ����         ����
***************************************************************/
void AlarmAgent::MessageEntry(const MessageFrame& message)
{
    switch (mu->get_state())
    {
    case S_Startup:
    {
        switch (message.option.id())
        {
        case EV_POWER_ON:
        {
            mu->set_state(S_Work);
            OutPut(SYS_NOTICE, "%s power on!\n",mu->name().c_str());
            break;
        }
        default:
            break;
        }
        break;
    }

    case S_Work:
    {
        switch (message.option.id())
        {
        case EV_ALARM_M2A_SYNC_REQ:
        {
            AlarmSync as;
            as.deserialize(message.data);
            AlarmARecAidSync(as);
            break;
        }
        case EV_ALARM_BROADCAST:
        {
            AlarmATravelAllAlarmPool();
            break;
        }
        case EV_ALARM_TIMER_POWERON_SYNC:
        {
            _ucPowerFlag = ALARM_S2M_SYNC_PERMIT;
            break;
        }
        case EV_ALARM_TIMER_A2MSYNC:
        {
            if (0 == _gM2ASyncFlag)
            {
                AlarmATravelAllAlarmPool();
            }
            else
            {
                _gM2ASyncFlag = 0;
            }
            break;
        }
        default:
            break;
        }
        break;
    }

    default:
        break;
    }
}

/************************************************************
* �������ƣ� send
* ���������� ����澯�����澯ת����Manager
* ���ʵı� ��
* �޸ĵı� ��
* ��������� �澯��Ϣ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2011/07         V1.0       ����         ����
***************************************************************/
void AlarmAgent::Send(const AlarmReport& alarm)
{
    T_AlarmReport      tAlarmReport;
    T_AlarmAddress  tAlarmLocation;

    memset(&tAlarmReport,0,sizeof(T_AlarmReport));
    memset(&tAlarmLocation,0,sizeof(T_AlarmAddress));

    tAlarmReport.dwAlarmCode = alarm.dwAlarmCode;
    tAlarmReport.ucAlarmFlag = alarm.ucAlarmFlag;
    tAlarmReport.wKeyLen = alarm.wKeyLen;
    tAlarmReport.wObjType = alarm.wObjType;
    if (alarm.alarm_location.strSubSystem.length()<ALARM_ADDRESS_LENGTH)
        memcpy(tAlarmLocation.strSubSystem,alarm.alarm_location.strSubSystem.c_str(),
               alarm.alarm_location.strSubSystem.length());    
    if (alarm.alarm_location.strLevel1.length()<ALARM_ADDRESS_LENGTH)
        memcpy(tAlarmLocation.strLevel1,alarm.alarm_location.strLevel1.c_str(),
               alarm.alarm_location.strLevel1.length());
    if (alarm.alarm_location.strLevel2.length()<ALARM_ADDRESS_LENGTH)
        memcpy(tAlarmLocation.strLevel2,alarm.alarm_location.strLevel2.c_str(),
               alarm.alarm_location.strLevel2.length());
    if (alarm.alarm_location.strLevel3.length()<ALARM_ADDRESS_LENGTH)
        memcpy(tAlarmLocation.strLevel3,alarm.alarm_location.strLevel3.c_str(),
               alarm.alarm_location.strLevel3.length());
    if (alarm.alarm_location.strLocation.length()<ALARM_ADDINFO_UNION_MAX)
        memcpy(tAlarmLocation.strLocation,alarm.alarm_location.strLocation.c_str(),
               alarm.alarm_location.strLocation.length());
    if (alarm.aucMaxAddInfo.length() < ALARM_ADDINFO_UNION_MAX)
        memcpy(tAlarmReport.tAddInfo.aucMaxAddInfo,
               alarm.aucMaxAddInfo.c_str(),
               alarm.aucMaxAddInfo.length());
    else
        OutPut(SYS_ERROR, "addinfo is too long");


    switch (tAlarmReport.ucAlarmFlag)
    {
    case OAM_REPORT_ALARM:
    {
        OutPut(SYS_DEBUG, "Receive alarm:code = %d!\n",alarm.dwAlarmCode);
        AlarmAReceiveAlarm(&tAlarmReport, &tAlarmLocation);
        break;
    }
    case OAM_REPORT_RESTORE:
    {
        OutPut(SYS_DEBUG, "Receive restore:code = %d!\n",alarm.dwAlarmCode);
        AlarmAReceiveRestore(&tAlarmReport,  &tAlarmLocation);
        break;
    }
    case OAM_REPORT_INFORM:
    {
        OutPut(SYS_DEBUG, "Receive Inform:code = %d!\n",alarm.dwAlarmCode);
        AlarmASendInform(alarm);
        break;
    }
    default:
        OutPut(SYS_ERROR, "No Such Alarm Flag!\n");
        break;
    }

}

/************************************************************
* �������ƣ� start
* ���������� ��������
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2011/10         V1.0       ����         ����
***************************************************************/
STATUS AlarmAgent::Start()
{
    if (SUCCESS != AlarmASetTimer(RELATIVE_TIMER,TIMER_POWERON_SYNC,EV_ALARM_TIMER_POWERON_SYNC))
    {
        OutPut(SYS_ERROR, "Set timer for alarm agent power on sync failed!\n");
        return ERROR_NO_TIMER;
    }

    if (SUCCESS != AlarmASetTimer(LOOP_TIMER,TIMER_A2MSYNC,EV_ALARM_TIMER_A2MSYNC))
    {
        OutPut(SYS_ERROR, "Set timer for alarm agent sync to manager failed!\n");
        return ERROR_NO_TIMER;
    }

    _ucFrameNumber = 0;
    _dwCurAid = 0;
    _bIsAlarmPoolFull = false;
    _ucPowerFlag = ALARM_S2M_SYNC_FORBID;
    _gM2ASyncFlag = 0;
    _gVisitFlag = 0;

    _strReportAddr.append(ApplicationName());
    _strReportAddr.append("/");
    _strReportAddr.append(ProcessName());
    _strReportAddr.append("/");
    _strReportAddr.append(ALARM_AGENT);

    memset(&_tReport,0,sizeof(T_AgtAlarm));

    return JoinGroup();
}

/************************************************************
* �������ƣ� AlarmASetTimer
* ������������װ��ʱ����
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2011/10         V1.0       ����         ����
***************************************************************/
STATUS AlarmAgent::AlarmASetTimer(uint32  type,  uint32    duration, uint32    msgid)
{
    TimeOut timeout;
    timeout.duration = duration;
    timeout.msgid = msgid;
    timeout.type = type;

    TIMER_ID mytid = mu->CreateTimer();
    if (INVALID_TIMER_ID == mytid)
    {
        OutPut(SYS_ERROR, "Create timer failed!\n");
        return ERROR;
    }
    else
    {
        return mu->SetTimer(mytid,timeout);
    }
}
/************************************************************
* �������ƣ� OamAlarmLibReceiveAlarm
* ���������� �յ��澯ʱ�Ĵ���
* ���ʵı� ��
* �޸ĵı� ��
* ��������� *pOamAlarmPool  �澯����һ���澯��Ԫ�ĵ�ַ
*            *pAlarmReport   �澯�ϱ���Ϣ�е�һ���澯ʵ��
*            *ptAlarmLocation    �澯������λ��
* ��������� ��
* �� �� ֵ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2011/10         V1.0       ����         ����
***************************************************************/
void AlarmAgent::AlarmAReceiveAlarm(T_AlarmReport *pAlarmReport, T_AlarmAddress  *ptAlarmLocation)
{

    T_AlarmPool         tAlmInfo;

    if (MAX_AGT_ALARM <= _pool.size())
    {
        OutPut(SYS_ERROR, " alarm pool is full!pool size:%d\n",_pool.size());
        return;
    }

    memset(&tAlmInfo,    0, sizeof(T_AlarmPool));

    memcpy(&tAlmInfo.tAlarmInfo.tAlarmLocation,ptAlarmLocation,sizeof(T_AlarmAddress));

    OamAlarmLibSaveInfo(&tAlmInfo.tAlarmInfo,  pAlarmReport);

    T_SyncData            tSyncData;
    tSyncData.dwAlarmCode = tAlmInfo.tAlarmInfo.dwAlarmCode;
    tSyncData.dwCRCCode[0] = tAlmInfo.tAlarmInfo.dwCRCCode[0];
    tSyncData.dwCRCCode[1] = tAlmInfo.tAlarmInfo.dwCRCCode[1];
    tSyncData.dwCRCCode[2] = tAlmInfo.tAlarmInfo.dwCRCCode[2];
    tSyncData.dwCRCCode[3] = tAlmInfo.tAlarmInfo.dwCRCCode[3];

    /* ����PPAID*/
    _dwCurAid++;
    tAlmInfo.dwPPAid = _dwCurAid;

    AlarmAConstructReport(&tAlmInfo);

    AlarmASendAlarm();

}

/************************************************************
* �������ƣ� OamAlarmLibReceiveRestore
* ���������� �յ��澯�ָ�ʱ�Ĵ���
* ���ʵı� ��
* �޸ĵı� ��
* ��������� *pOamAlarmPool  �澯����һ���澯��Ԫ�ĵ�ַ
*            *pAlarmReport   �澯�ϱ���Ϣ�е�һ���澯ʵ��
*            *ptAlarmLocation    �澯������λ��
* ��������� ��
* �� �� ֵ�� �����룬ͳһ�涨
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2011/10         V1.0       ����         ����
***************************************************************/
void AlarmAgent::AlarmAReceiveRestore(T_AlarmReport *pAlarmReport, T_AlarmAddress  *ptAlarmLocation)
{
    T_AlarmPool         tAlmInfo;

    memset(&tAlmInfo,    0, sizeof(T_AlarmPool));

    memcpy(&tAlmInfo.tAlarmInfo.tAlarmLocation,ptAlarmLocation,sizeof(T_AlarmAddress));

    OamAlarmLibSaveInfo(&tAlmInfo.tAlarmInfo,  pAlarmReport);

    T_SyncData            tSyncData;
    tSyncData.dwAlarmCode = tAlmInfo.tAlarmInfo.dwAlarmCode;
    tSyncData.dwCRCCode[0] = tAlmInfo.tAlarmInfo.dwCRCCode[0];
    tSyncData.dwCRCCode[1] = tAlmInfo.tAlarmInfo.dwCRCCode[1];
    tSyncData.dwCRCCode[2] = tAlmInfo.tAlarmInfo.dwCRCCode[2];
    tSyncData.dwCRCCode[3] = tAlmInfo.tAlarmInfo.dwCRCCode[3];

    AlarmAConstructRestore(&tAlmInfo, NORMAL_ALARM_RESTORE);
	
    AlarmASendAlarm();

}

/************************************************************
* �������ƣ� OamAlarmLibConstructReport
* ���������� ����澯�ϱ���AlarmManager����Ϣ����
* ���ʵı� ��
* �޸ĵı� ��
* ��������� *pAlarmReport    ׼���ϱ���AlarmManager�ĸ澯��Ϣ����
*            *pOamAlarmPool   �澯�صĵ�ַ
*            *pAlarmPoolInfo  �澯���еĸ澯��Ԫ
* ��������� ��
* �� �� ֵ�� �����룬ͳһ�涨
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2011/10         V1.0       ����         ����
***************************************************************/
void AlarmAgent::AlarmAConstructReport(T_AlarmPool *pAlarmPoolInfo)
{
    OAM_RETURN_IF_FAIL(NULL != pAlarmPoolInfo);

    memset(&_tReport,0,sizeof(T_AgtAlarm));

    _tReport.tAlmInfo = pAlarmPoolInfo->tAlarmInfo;
    /* ֻ�����˳����������ϱ��澯�ָ�ʱ�ŶԶ����������и�ֵ������������ϱ��ĸ澯���߻ָ���Ϣ�иö�������һ������Ϊ��Чֵ0xffff */
    _tReport.wSendNum = 0xffff;

    _tReport.ucIsLogAlm = ALARM_COMPARE_PHYSICAL;

    _tReport.ucAlarmFlag = OAM_REPORT_ALARM;
    memcpy(_tReport.tReportAddr,_strReportAddr.c_str(),_strReportAddr.length());
}

/************************************************************
* �������ƣ� OamAlarmLibConstructRestore
* ���������� ����澯�ָ��ϱ���AlarmManager����Ϣ����
* ���ʵı� ��
* �޸ĵı� ��
* ��������� *pAlarmReport    ׼���ϱ���AlarmManager�ĸ澯��Ϣ����
*            *pAlarmPoolInfo  �澯���еĸ澯��Ԫ
             ucRestoreType    �ָ�����
* ��������� ��
* �� �� ֵ�� �����룬ͳһ�涨
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2011/10         V1.0       ����         ����
***************************************************************/
void AlarmAgent::AlarmAConstructRestore( T_AlarmPool *pAlarmPoolInfo,BYTE ucRestoreType)
{
    OAM_RETURN_IF_FAIL(NULL != pAlarmPoolInfo);

    memset(&_tReport,0,sizeof(T_AgtAlarm));

    _tReport.tAlmInfo = pAlarmPoolInfo->tAlarmInfo;

    /* ֻ�����˳����������ϱ��澯�ָ�ʱ�ŶԶ����������и�ֵ������������ϱ��ĸ澯���߻ָ���Ϣ�иö�������һ������Ϊ��Чֵ0xffff */
    _tReport.wSendNum = 0xffff;
    _tReport.ucIsLogAlm = ALARM_COMPARE_PHYSICAL;

    _tReport.ucAlarmFlag = OAM_REPORT_RESTORE;
    _tReport.ucRestoreType = ucRestoreType;
    _tReport.dwAlarmID = pAlarmPoolInfo->dwPPAid;
    memcpy(_tReport.tReportAddr,_strReportAddr.c_str(),_strReportAddr.length());

}

/**********************************************************************
* �������ƣ�WORD32 OamAlarmAInformSend
* ����������ǿ�Ʒ���֪ͨ
* ���ʵı���
* �޸ĵı���
* ��������� *pOamInformPool ֪ͨ��Ϣ
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2011/10         V1.0       ����         ����
************************************************************************/
void AlarmAgent::AlarmASendInform(const AlarmReport &inform)
{

    //��ʼ������Ϣ
    AgentAlarm p ;
    p.dwAlarmCode = inform.dwAlarmCode;
    p.wObjType = inform.wObjType;
    p.tAlarmAddr = _strReportAddr;
    p.alarm_location = inform.alarm_location;
    p.aucMaxAddInfo = inform.aucMaxAddInfo;
    p.wAlarmFlag = OAM_REPORT_INFORM;

    p.serialize();

    string str;
    GetRegisteredSystem(str);

    MID receiver(str,PROC_APISVR,ALARM_MANAGER);
    MessageOption option(receiver,EV_INFORM_REPORT,0,0);
    mu->Send(p,option);

}
/**********************************************************************
* �������ƣ�VOID OamAlarmLibIsAPoolFull
* �����������ж��Ƿ�Ϊ�澯����ֵ
* ���ʵı���
* �޸ĵı���
* ���������
* �����������
* �� �� ֵ����
* ����˵����
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2011/10         V1.0       ����         ����
**********************************************************************/
void AlarmAgent::AlarmAPoolFull()
{
    WORD32              dwAlarmTh  = 0;
    WORD32              dwRestoreTh  = 0;
    WORD32              dwAlarmNum  = 0;
    T_AlarmReport       tAlarmReport = {0};
    T_AlarmPool         tAlmInfo;

    memset(&tAlmInfo,0,sizeof(tAlmInfo));

    dwAlarmNum = _pool.size();
    /* ��ȡ�澯������ */
    dwAlarmTh = MAX_AGT_ALARM * ALARMPOOL_OVERFLOW_ALARM_THRESHOLD / 100;
    dwRestoreTh = MAX_AGT_ALARM * ALARMPOOL_OVERFLOW_RESTORE_THRESHOLD / 100;

    /* �жϸ澯���ǻָ� */
    if ((dwAlarmTh <= dwAlarmNum) && (false == _bIsAlarmPoolFull))
    {
        _bIsAlarmPoolFull = true;
        tAlarmReport.ucAlarmFlag = OAM_REPORT_ALARM;
    }
    else if ((dwRestoreTh >= dwAlarmNum) && (true == _bIsAlarmPoolFull))
    {
        _bIsAlarmPoolFull = false;
        tAlarmReport.ucAlarmFlag = OAM_REPORT_RESTORE;
    }
    else
    {
        OAM_RETURN;
    }

    /* ����澯����
    tAlarmReport.dwAlarmCode = ALARM_OAM_PHYALARMPOOL_FULL;
      */
    tAlarmReport.wKeyLen = 0;
    tAlarmReport.wObjType = INVALID_OBJTYPE;

    /* �ϱ� */
    if (OAM_REPORT_ALARM == tAlarmReport.ucAlarmFlag)
    {
        AlarmAConstructReport(&tAlmInfo);
    }
    else
    {
        AlarmAConstructRestore(&tAlmInfo, NORMAL_ALARM_RESTORE);
    }

    AlarmASendAlarm();
}

/************************************************************
* �������ƣ� OamAlarmLibSaveInfo(T_AlarmInfo *pPoolAlarmInfo, T_AlarmReport *pAlarmReport)
* ���������� �澯/�澯�ָ��ϱ������е���Ϣ��¼��澯�صĸ澯��Ԫ��
* ���ʵı� ��
* �޸ĵı� ��
* ��������� *pPoolAlarmInfo �澯�ظ澯��Ԫ�еĸ澯��Ϣ�ṹ��Ϣ
*            *pAlarmReport   �ϱ��ĸ澯/�澯�ָ�����ϸ��Ϣ
* ��������� ��
* �� �� ֵ�� �����룬ͳһ�涨
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2011/10         V1.0       ����         ����
***************************************************************/
BYTE AlarmAgent::OamAlarmLibSaveInfo(T_AlarmInfo *pPoolAlarmInfo, T_AlarmReport *pAlarmReport)
{
    T_CRCAlmData  tAlarmCRCData;
    WORD32           dwCrcLen = 0;
    WORD32           dwStatus = 0;

    OAM_RETURN_VAL_IF_FAIL(NULL != pPoolAlarmInfo, OAM_ALARM_FAILED);
    OAM_RETURN_VAL_IF_FAIL(NULL != pAlarmReport, OAM_ALARM_FAILED);

    memset(&tAlarmCRCData,  0, sizeof(T_CRCAlmData));

    Datetime          tAlarmTime;
    pPoolAlarmInfo->tAlarmTime = tAlarmTime;

    if (dwStatus != 0)
    {
        OutPut(SYS_ERROR, "GetSysClock fail");
    }
    pPoolAlarmInfo->dwAlarmCode = pAlarmReport->dwAlarmCode;
    pPoolAlarmInfo->wObjType    = pAlarmReport->wObjType;
    pPoolAlarmInfo->wKeyLen     = pAlarmReport->wKeyLen;
    pPoolAlarmInfo->tAddInfo    = pAlarmReport->tAddInfo;
    if (pPoolAlarmInfo->wKeyLen > ALARM_ADDINFO_UNION_MAX)
    {
        pPoolAlarmInfo->wKeyLen = ALARM_ADDINFO_UNION_MAX;
        OutPut(SYS_ERROR, "wKeyLen larger than ALARM_ADDINFO_UNION_MAX,value%d!!!\n",pAlarmReport->wKeyLen);
    }

    memcpy(tAlarmCRCData.tAlarmCrcHead.strSubSystem,pPoolAlarmInfo->tAlarmLocation.strSubSystem,ALARM_ADDRESS_LENGTH);
    memcpy(tAlarmCRCData.tAlarmCrcHead.strLevel1,pPoolAlarmInfo->tAlarmLocation.strLevel1,ALARM_ADDRESS_LENGTH);
    memcpy(tAlarmCRCData.tAlarmCrcHead.strLevel2,pPoolAlarmInfo->tAlarmLocation.strLevel2,ALARM_ADDRESS_LENGTH);
    memcpy(tAlarmCRCData.tAlarmCrcHead.strLevel3,pPoolAlarmInfo->tAlarmLocation.strLevel3,ALARM_ADDRESS_LENGTH);
    tAlarmCRCData.tAlarmCrcHead.dwAlarmCode = pPoolAlarmInfo->dwAlarmCode;
    tAlarmCRCData.tAlarmCrcHead.wKeyLen = pPoolAlarmInfo->wKeyLen;
    if (pPoolAlarmInfo->wKeyLen !=  0)
    {
        memcpy(tAlarmCRCData.ucData, pPoolAlarmInfo->tAddInfo.aucMaxAddInfo, pPoolAlarmInfo->wKeyLen);
        dwCrcLen = sizeof(T_AlmCRCHead) + pPoolAlarmInfo->wKeyLen;
    }
    else
    {
        memcpy(tAlarmCRCData.ucData, pPoolAlarmInfo->tAddInfo.aucMaxAddInfo, ALARM_ADDINFO_UNION_MAX);
        dwCrcLen = sizeof(tAlarmCRCData);
    }
    pPoolAlarmInfo->dwCRCCode[0] = Oam_CountCRC32((BYTE *)&tAlarmCRCData , dwCrcLen, PRE_CRC);

    return OAM_ALARM_SUCCESSED;
}

/************************************************************
* �������ƣ� OamAlarmARecAidSync()
* ���������� ��ʼ���ո澯ͬ����Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� *pucEntry : ��������Ҫ���������
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2011/10         V1.0       ����         ����
***************************************************************/
void AlarmAgent::AlarmARecAidSync(AlarmSync as)
{
    /* �����ϵ�10���ڲ�����ͬ�� */
    if (_ucPowerFlag == ALARM_S2M_SYNC_FORBID)
        OAM_RETURN;

    /* �ݲ��ְ�
    OAM_RETURN_IF_FAIL( as.ucFrameNo <= as.ucFrameCount);
    OAM_RETURN_IF_FAIL( as.wAlarmIDNum <= MAX_ALARM_PER_FRAME);*/

    if (_strReportAddr != as.tAlarmAddr)
    {
        OutPut(SYS_ERROR,"Agent address no same!SyncAddr:%s\n",as.tAlarmAddr.c_str());
        return;
    }

    /* ��1֡��ȫ�ֱ���+1 */
    if ( as.ucFrameNo == 1)
    {
        _gVisitFlag++;
    }

    /* ������֡�Ų�һ���˳� */
    if (_ucFrameNumber != (as.ucFrameNo - 1))
    {
        _ucFrameNumber = 0;
        OAM_RETURN;
    }

    if ( ALARM_COMPARE_LOGICAL == as.ucPhyOrLog)
    {
        _gM2ASyncFlag = _gM2ASyncFlag|SYNC_LOGIC;  /* ����߼��澯�Ѿ�ͬ�� */
    }
    else
    {
        _gM2ASyncFlag = _gM2ASyncFlag|SYNC_PHYSIC;  /* �������澯�Ѿ�ͬ�� */
    }

    /* �Ƚϸ�֡���� */
    AlarmACompareOmpSynAlarmID(as);

    /* ����֡�ż�1 */
    _ucFrameNumber++;

    /* ���һ֡�������澯�� */
    if (as.ucFrameNo == as.ucFrameCount)
    {
        AlarmATravelAlarmPool();
        _ucFrameNumber = 0;
    }

    OAM_RETURN;
}/* OamAlarmARecAidSync */

/************************************************************
* �������ƣ� OamAlarmACompareOmpSynAlarmID()
* ���������� �Ƚϸ�֡����
* ���ʵı� ��
* �޸ĵı� ��
* ��������� *pShmReg  �澯����Ϣ
*            as  ͬ������
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2011/10         V1.0       ����         ����
***************************************************************/
void AlarmAgent::AlarmACompareOmpSynAlarmID(AlarmSync &as)
{
    T_SyncData            tSyncData = {0};
    WORD16                wLoopNum = 0;

    /* ��ͬ�������б��� */
    for ( wLoopNum = 0;wLoopNum < as.wAlarmIDNum;wLoopNum++)
    {
        /* ��ȡ������ĸ澯ʵ�� */
        tSyncData.dwAlarmCode = as.dwData[wLoopNum*5];
        tSyncData.dwCRCCode[0] = as.dwData[wLoopNum*5+1];
        tSyncData.dwCRCCode[1] = as.dwData[wLoopNum*5+2];
        tSyncData.dwCRCCode[2] = as.dwData[wLoopNum*5+3];
        tSyncData.dwCRCCode[3] = as.dwData[wLoopNum*5+4];

        string str;
        AlarmACRC2Key(str,&tSyncData);

        map<string, T_AlarmPool>::iterator index = _pool.find(str);

        if (_pool.end() != index)
        {
            index->second.ucSyncFlag = _gVisitFlag;
        }
        else
        {
            OAM_MEMSET(&_tReport,0,sizeof(T_AgtAlarm));
            _tReport.ucIsLogAlm = as.ucPhyOrLog;
            _tReport.ucAlarmFlag =  OAM_REPORT_RESTORE;
            _tReport.ucRestoreType =   PPTOOMP_ALARM_RESTORE;
            _tReport.wSendNum  =   0xffff;
            _tReport.tAlmInfo.dwAlarmCode =   tSyncData.dwAlarmCode;
            _tReport.tAlmInfo.dwCRCCode[0] =  tSyncData.dwCRCCode[0];
            _tReport.tAlmInfo.dwCRCCode[1] =   tSyncData.dwCRCCode[1];
            _tReport.tAlmInfo.dwCRCCode[2] =   tSyncData.dwCRCCode[2];
            _tReport.tAlmInfo.dwCRCCode[3] =   tSyncData.dwCRCCode[3];
            memcpy(_tReport.tReportAddr,as.tAlarmAddr.c_str(),  as.tAlarmAddr.length());

            AlarmASendAlarm();
        }
        /* ���ҵ����Ƚ������ַ�Ƿ�һ�� */
    }
}/* AlarmACompareOmpSynAlarmID */

/************************************************************
* �������ƣ� OamAlarmATravelAlarmPool()
* ���������� ����ѡ���澯��
* ���ʵı� ��
* �޸ĵı� ��
* ��������� *pShmReg  �澯����Ϣ
*            as ͬ������
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2011/10         V1.0       ����         ����
***************************************************************/
void AlarmAgent::AlarmATravelAlarmPool()
{
    map<string, T_AlarmPool>::iterator index = _pool.begin();

    for (; index != _pool.end(); ++index)
    {
        T_AlarmPool   tAlmInfo = index->second;

        if (_gVisitFlag != tAlmInfo.ucSyncFlag)
        {
            AlarmAConstructReport(&tAlmInfo);

            AlarmASendAlarm();
        }
    }
}/* AlarmATravelAlarmPool */

/************************************************************
* �������ƣ� AlarmATravelAllAlarmPool()
* ���������� �������и澯��
* ���ʵı� ��
* �޸ĵı� ��
* ��������� *pShmReg  �澯����Ϣ
*            *pUserData  ����澯
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2011/10         V1.0       ����         ����
***************************************************************/
void AlarmAgent:: AlarmATravelAllAlarmPool( )
{
    map<string, T_AlarmPool>::iterator index = _pool.begin();

    for (; index != _pool.end(); ++index)
    {
        T_AlarmPool   tAlmInfo = index->second;
        AlarmAConstructReport(&tAlmInfo);

        AlarmASendAlarm();
    }

    _gM2ASyncFlag = 0;
}/* AlarmATravelAllAlarmPool */

/************************************************************
* �������ƣ� AlarmASendAlarm()
* ���������� ���͸澯��Ϣ��MANAGER
* ���ʵı� ��
* �޸ĵı� ��
* ���������
* ��������� ��
* �� �� ֵ�� 0:��ͬ
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2011/10         V1.0       ����         ����
***************************************************************/
void  AlarmAgent::AlarmASendAlarm()
{
    //��ʼ������Ϣ
    AgentAlarm p ;
    p.dwAlarmCode = _tReport.tAlmInfo.dwAlarmCode;
    p.wObjType = _tReport.tAlmInfo.wObjType;
    p.wKeyLen = _tReport.tAlmInfo.wKeyLen;
    p.wFilterFlag = _tReport.tAlmInfo.ucFilterFlag;
    p.tAlarmTime = _tReport.tAlmInfo.tAlarmTime;
    p.tAlarmAddr = _strReportAddr;
    p.alarm_location.strLevel1 = _tReport.tAlmInfo.tAlarmLocation.strLevel1;
    p.alarm_location.strLevel2 = _tReport.tAlmInfo.tAlarmLocation.strLevel2;
    p.alarm_location.strLevel3 = _tReport.tAlmInfo.tAlarmLocation.strLevel3;
    p.alarm_location.strLocation = _tReport.tAlmInfo.tAlarmLocation.strLocation;
    p.alarm_location.strSubSystem = _tReport.tAlmInfo.tAlarmLocation.strSubSystem;
    p.dwCRCCode[0] = _tReport.tAlmInfo.dwCRCCode[0];
    p.dwCRCCode[1] = _tReport.tAlmInfo.dwCRCCode[1];
    p.dwCRCCode[2] = _tReport.tAlmInfo.dwCRCCode[2];
    p.dwCRCCode[3] = _tReport.tAlmInfo.dwCRCCode[3];
    p.aucMaxAddInfo = _tReport.tAlmInfo.tAddInfo.aucMaxAddInfo;
    p.wSendNum = _tReport.wSendNum;
    p.wIsLogAlm = _tReport.ucIsLogAlm;
    p.wAlarmFlag = _tReport.ucAlarmFlag;
    p.dwAlarmID = _tReport.dwAlarmID;
    p.wRestoreType = _tReport.ucRestoreType;

    p.serialize();

    string str ;
    GetRegisteredSystem(str);

    MID receiver(str,PROC_APISVR,ALARM_MANAGER);
    MessageOption option(receiver,EV_ALARM_REPORT,0,0);
    mu->Send(p,option);
}/* AlarmASendAlarm */

/************************************************************
* �������ƣ� AlarmACRC2Key
* �����������ؼ��ֹ��캯��
* ���ʵı� ��
* �޸ĵı� ��
* ���������
* ���������
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2011/10         V1.0       ����         ����
***************************************************************/
string& AlarmAgent::AlarmACRC2Key(string& str,  T_SyncData *data)
{
    ostringstream   os;

    os << "CRCCODE0 = "  <<  data->dwCRCCode[0] ;

    str = os.str();

    return str;
}
/************************************************************
* �������ƣ� OamAlarmAShowAlarmPool()
* ���������� �������и澯��
* ���ʵı� ��
* �޸ĵı� ��
* ��������� *pShmReg  �澯����Ϣ
*            *pUserData  ����澯
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2011/10         V1.0       ����         ����
***************************************************************/
void  AlarmAgent::AlarmShowAlarmPool()
{
    /* ������ˮ�ŵĸ澯���� */
    cout <<"------------------------Start---------------------"<<endl;

    map <string,T_AlarmPool>::iterator itpool = _pool.begin();

    for (; itpool != _pool.end(); ++itpool)
    {
        T_AlarmPool AlarmPoolInfo = itpool->second;
        cout<<"PPAID = "<<AlarmPoolInfo.dwPPAid<<
        ", ACode = "<<AlarmPoolInfo.tAlarmInfo.dwAlarmCode<<
        ", ObjType ="<<AlarmPoolInfo.tAlarmInfo.wObjType<<
        ", CRCCode ="<<AlarmPoolInfo.tAlarmInfo.dwCRCCode[0] <<
        ":"<<AlarmPoolInfo.tAlarmInfo.dwCRCCode[1] <<
        ":"<<AlarmPoolInfo.tAlarmInfo.dwCRCCode[2] <<
        ":"<<AlarmPoolInfo.tAlarmInfo.dwCRCCode[3] <<endl;
        cout<<"Alarm Addr = "<<AlarmPoolInfo.tAlarmInfo.tAlarmLocation.strLevel1<<
        ":"<<AlarmPoolInfo.tAlarmInfo.tAlarmLocation.strLevel2 <<
        ":"<<AlarmPoolInfo.tAlarmInfo.tAlarmLocation.strLevel3 <<
        ", "<<AlarmPoolInfo.tAlarmInfo.tAlarmLocation.strLocation<<endl;
        cout<<"Alarm Time = "<<AlarmPoolInfo.tAlarmInfo.tAlarmTime.tostr()<<endl;
        cout<<"AddInfo = "<<AlarmPoolInfo.tAlarmInfo.tAddInfo.aucMaxAddInfo<<endl;
        cout<<"Map key :"<<itpool->first<<endl;
        cout<<endl;
    }

    cout <<"Alarm Query Num = "<<_pool.size()<<endl;
    cout <<"-------------------------End----------------------"<<endl;

}/* AlarmShowAlarmPool */

/************************************************************
* �������ƣ� SendDbgAlarm
* ���������� ���Ժ������͸澯��Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ���������
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2011/10         V1.0       ����         ����
***************************************************************/
void SendDbgAlarm(BYTE ucFlag,WORD32 dwAlarmCode)
{
    AlarmReport ar;

    ar.dwAlarmCode = dwAlarmCode;
    ar.ucAlarmFlag = ucFlag;
    ar.alarm_location.strLevel2 = ApplicationName();
    
    if(dwAlarmCode == ALM_TECS_ETHERNET_PORT_DOWN)
    {
        AlarmPortDown info("eth1");
        ar.aucMaxAddInfo = info.serialize();
    }

    AlarmAgent *aa = AlarmAgent::GetInstance();

    if (NULL != aa)
        aa->Send(ar);
    else
        cout<<"AlarmAgent instance is NULL"<<endl;

}
DEFINE_DEBUG_FUNC(SendDbgAlarm);

/************************************************************
* �������ƣ� DbgShowAgentAlarm
* ���������� ���Ժ�����ʾAGENT�ϵĸ澯��Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ���������
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2011/10         V1.0       ����         ����
***************************************************************/
void DbgShowAgentAlarm()
{
    AlarmAgent *aa = AlarmAgent::GetInstance();

    if (NULL != aa)
        aa->AlarmShowAlarmPool();
    else
        cout<<"AlarmAgent instance is NULL"<<endl;
}
DEFINE_DEBUG_FUNC(DbgShowAgentAlarm);

void DbgShowAgentData()
{
    AlarmAgent *aa = AlarmAgent::GetInstance();

    if (NULL != aa)
    {
        cout<<"Alarm Report Addr : "<<aa->GetReportAddr()<<endl;
    }
    else
        cout<<"AlarmAgent instance is NULL"<<endl;
}
DEFINE_DEBUG_FUNC(DbgShowAgentData);
}
