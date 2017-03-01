/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：alarm_agent.cpp
* 文件标识：见配置管理计划书
* 内容摘要：AlarmAgent类实现文件
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

#include "alarm_agent.h"

namespace zte_tecs
{
AlarmAgent* AlarmAgent::instance = NULL;

/************************************************************
* 函数名称： AlarmAgent
* 功能描述： 构造函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2011/10         V1.0       曹亮         创建
***************************************************************/
AlarmAgent::AlarmAgent()
{
    mu = NULL;
}

/************************************************************
* 函数名称： StartMu
* 功能描述： 启动mu的函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2012/5         V1.0       lixiaocheng         创建
***************************************************************/
STATUS AlarmAgent::StartMu(const string& name)
{
    if(mu)
    {
        SkyAssert(false);
        return ERROR_DUPLICATED_OP;
    }
    
    // 消息单元的创建和初始化
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

    //给自己发送上电消息，促使消息单元状态切换到工作态
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
* 函数名称： ~AlarmAgent()
* 功能描述： 析构函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2011/10         V1.0       曹亮         创建
***************************************************************/
AlarmAgent::~AlarmAgent()
{
    if (mu)
        delete mu;
}

/************************************************************
* 函数名称： Receive
* 功能描述： 主动接收消息的函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2011/10         V1.0       曹亮         创建
***************************************************************/
STATUS AlarmAgent::Receive(const MessageFrame& message)
{
    return mu->Receive(message);
}

/************************************************************
* 函数名称： JoinGroup
* 功能描述： 将ALARM_AGENT加入消息组
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2011/10         V1.0       曹亮         创建
***************************************************************/
STATUS AlarmAgent::JoinGroup()
{
    //加入一个组播组
    return mu->JoinMcGroup(ALARM_GROUP);
}

/************************************************************
* 函数名称： MessageEntry
* 功能描述： 消息处理函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： message  消息信息
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2011/10         V1.0       曹亮         创建
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
* 函数名称： send
* 功能描述： 处理告警并将告警转发至Manager
* 访问的表： 无
* 修改的表： 无
* 输入参数： 告警消息体
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2011/07         V1.0       曹亮         创建
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
* 函数名称： start
* 功能描述： 启动函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2011/10         V1.0       曹亮         创建
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
* 函数名称： AlarmASetTimer
* 功能描述：封装定时设置
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2011/10         V1.0       曹亮         创建
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
* 函数名称： OamAlarmLibReceiveAlarm
* 功能描述： 收到告警时的处理
* 访问的表： 无
* 修改的表： 无
* 输入参数： *pOamAlarmPool  告警池中一个告警单元的地址
*            *pAlarmReport   告警上报消息中的一个告警实体
*            *ptAlarmLocation    告警发生的位置
* 输出参数： 无
* 返 回 值：
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2011/10         V1.0       曹亮         创建
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

    /* 生成PPAID*/
    _dwCurAid++;
    tAlmInfo.dwPPAid = _dwCurAid;

    AlarmAConstructReport(&tAlmInfo);

    AlarmASendAlarm();

}

/************************************************************
* 函数名称： OamAlarmLibReceiveRestore
* 功能描述： 收到告警恢复时的处理
* 访问的表： 无
* 修改的表： 无
* 输入参数： *pOamAlarmPool  告警池中一个告警单元的地址
*            *pAlarmReport   告警上报消息中的一个告警实体
*            *ptAlarmLocation    告警发生的位置
* 输出参数： 无
* 返 回 值： 错误码，统一规定
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2011/10         V1.0       曹亮         创建
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
* 函数名称： OamAlarmLibConstructReport
* 功能描述： 构造告警上报给AlarmManager的消息队列
* 访问的表： 无
* 修改的表： 无
* 输入参数： *pAlarmReport    准备上报给AlarmManager的告警消息队列
*            *pOamAlarmPool   告警池的地址
*            *pAlarmPoolInfo  告警池中的告警单元
* 输出参数： 无
* 返 回 值： 错误码，统一规定
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2011/10         V1.0       曹亮         创建
***************************************************************/
void AlarmAgent::AlarmAConstructReport(T_AlarmPool *pAlarmPoolInfo)
{
    OAM_RETURN_IF_FAIL(NULL != pAlarmPoolInfo);

    memset(&_tReport,0,sizeof(T_AgtAlarm));

    _tReport.tAlmInfo = pAlarmPoolInfo->tAlarmInfo;
    /* 只有在退出防抖周期上报告警恢复时才对抖动次数进行赋值，其他情况下上报的告警或者恢复消息中该抖动次数一律设置为无效值0xffff */
    _tReport.wSendNum = 0xffff;

    _tReport.ucIsLogAlm = ALARM_COMPARE_PHYSICAL;

    _tReport.ucAlarmFlag = OAM_REPORT_ALARM;
    memcpy(_tReport.tReportAddr,_strReportAddr.c_str(),_strReportAddr.length());
}

/************************************************************
* 函数名称： OamAlarmLibConstructRestore
* 功能描述： 构造告警恢复上报给AlarmManager的消息队列
* 访问的表： 无
* 修改的表： 无
* 输入参数： *pAlarmReport    准备上报给AlarmManager的告警消息队列
*            *pAlarmPoolInfo  告警池中的告警单元
             ucRestoreType    恢复类型
* 输出参数： 无
* 返 回 值： 错误码，统一规定
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2011/10         V1.0       曹亮         创建
***************************************************************/
void AlarmAgent::AlarmAConstructRestore( T_AlarmPool *pAlarmPoolInfo,BYTE ucRestoreType)
{
    OAM_RETURN_IF_FAIL(NULL != pAlarmPoolInfo);

    memset(&_tReport,0,sizeof(T_AgtAlarm));

    _tReport.tAlmInfo = pAlarmPoolInfo->tAlarmInfo;

    /* 只有在退出防抖周期上报告警恢复时才对抖动次数进行赋值，其他情况下上报的告警或者恢复消息中该抖动次数一律设置为无效值0xffff */
    _tReport.wSendNum = 0xffff;
    _tReport.ucIsLogAlm = ALARM_COMPARE_PHYSICAL;

    _tReport.ucAlarmFlag = OAM_REPORT_RESTORE;
    _tReport.ucRestoreType = ucRestoreType;
    _tReport.dwAlarmID = pAlarmPoolInfo->dwPPAid;
    memcpy(_tReport.tReportAddr,_strReportAddr.c_str(),_strReportAddr.length());

}

/**********************************************************************
* 函数名称：WORD32 OamAlarmAInformSend
* 功能描述：强制发送通知
* 访问的表：无
* 修改的表：无
* 输入参数： *pOamInformPool 通知信息
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2011/10         V1.0       曹亮         创建
************************************************************************/
void AlarmAgent::AlarmASendInform(const AlarmReport &inform)
{

    //开始发送消息
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
* 函数名称：VOID OamAlarmLibIsAPoolFull
* 功能描述：判断是否为告警门限值
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：无
* 返 回 值：无
* 其它说明：
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2011/10         V1.0       曹亮         创建
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
    /* 获取告警池门限 */
    dwAlarmTh = MAX_AGT_ALARM * ALARMPOOL_OVERFLOW_ALARM_THRESHOLD / 100;
    dwRestoreTh = MAX_AGT_ALARM * ALARMPOOL_OVERFLOW_RESTORE_THRESHOLD / 100;

    /* 判断告警还是恢复 */
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

    /* 构造告警数据
    tAlarmReport.dwAlarmCode = ALARM_OAM_PHYALARMPOOL_FULL;
      */
    tAlarmReport.wKeyLen = 0;
    tAlarmReport.wObjType = INVALID_OBJTYPE;

    /* 上报 */
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
* 函数名称： OamAlarmLibSaveInfo(T_AlarmInfo *pPoolAlarmInfo, T_AlarmReport *pAlarmReport)
* 功能描述： 告警/告警恢复上报函数中的信息记录入告警池的告警单元中
* 访问的表： 无
* 修改的表： 无
* 输入参数： *pPoolAlarmInfo 告警池告警单元中的告警信息结构信息
*            *pAlarmReport   上报的告警/告警恢复的详细信息
* 输出参数： 无
* 返 回 值： 错误码，统一规定
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2011/10         V1.0       曹亮         创建
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
* 函数名称： OamAlarmARecAidSync()
* 功能描述： 开始接收告警同步消息
* 访问的表： 无
* 修改的表： 无
* 输入参数： *pucEntry : 处理所需要的输入参数
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2011/10         V1.0       曹亮         创建
***************************************************************/
void AlarmAgent::AlarmARecAidSync(AlarmSync as)
{
    /* 单板上电10秒内不允许同步 */
    if (_ucPowerFlag == ALARM_S2M_SYNC_FORBID)
        OAM_RETURN;

    /* 暂不分包
    OAM_RETURN_IF_FAIL( as.ucFrameNo <= as.ucFrameCount);
    OAM_RETURN_IF_FAIL( as.wAlarmIDNum <= MAX_ALARM_PER_FRAME);*/

    if (_strReportAddr != as.tAlarmAddr)
    {
        OutPut(SYS_ERROR,"Agent address no same!SyncAddr:%s\n",as.tAlarmAddr.c_str());
        return;
    }

    /* 第1帧，全局变量+1 */
    if ( as.ucFrameNo == 1)
    {
        _gVisitFlag++;
    }

    /* 与期望帧号不一致退出 */
    if (_ucFrameNumber != (as.ucFrameNo - 1))
    {
        _ucFrameNumber = 0;
        OAM_RETURN;
    }

    if ( ALARM_COMPARE_LOGICAL == as.ucPhyOrLog)
    {
        _gM2ASyncFlag = _gM2ASyncFlag|SYNC_LOGIC;  /* 标记逻辑告警已经同步 */
    }
    else
    {
        _gM2ASyncFlag = _gM2ASyncFlag|SYNC_PHYSIC;  /* 标记物理告警已经同步 */
    }

    /* 比较该帧数据 */
    AlarmACompareOmpSynAlarmID(as);

    /* 期望帧号加1 */
    _ucFrameNumber++;

    /* 最后一帧，遍历告警树 */
    if (as.ucFrameNo == as.ucFrameCount)
    {
        AlarmATravelAlarmPool();
        _ucFrameNumber = 0;
    }

    OAM_RETURN;
}/* OamAlarmARecAidSync */

/************************************************************
* 函数名称： OamAlarmACompareOmpSynAlarmID()
* 功能描述： 比较该帧数据
* 访问的表： 无
* 修改的表： 无
* 输入参数： *pShmReg  告警池信息
*            as  同步数据
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2011/10         V1.0       曹亮         创建
***************************************************************/
void AlarmAgent::AlarmACompareOmpSynAlarmID(AlarmSync &as)
{
    T_SyncData            tSyncData = {0};
    WORD16                wLoopNum = 0;

    /* 在同步数组中遍历 */
    for ( wLoopNum = 0;wLoopNum < as.wAlarmIDNum;wLoopNum++)
    {
        /* 获取码流里的告警实体 */
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
        /* 查找到，比较物理地址是否一致 */
    }
}/* AlarmACompareOmpSynAlarmID */

/************************************************************
* 函数名称： OamAlarmATravelAlarmPool()
* 功能描述： 遍历选定告警池
* 访问的表： 无
* 修改的表： 无
* 输入参数： *pShmReg  告警池信息
*            as 同步数据
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2011/10         V1.0       曹亮         创建
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
* 函数名称： AlarmATravelAllAlarmPool()
* 功能描述： 遍历所有告警池
* 访问的表： 无
* 修改的表： 无
* 输入参数： *pShmReg  告警池信息
*            *pUserData  打包告警
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2011/10         V1.0       曹亮         创建
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
* 函数名称： AlarmASendAlarm()
* 功能描述： 发送告警信息至MANAGER
* 访问的表： 无
* 修改的表： 无
* 输入参数：
* 输出参数： 无
* 返 回 值： 0:相同
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2011/10         V1.0       曹亮         创建
***************************************************************/
void  AlarmAgent::AlarmASendAlarm()
{
    //开始发送消息
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
* 函数名称： AlarmACRC2Key
* 功能描述：关键字构造函数
* 访问的表： 无
* 修改的表： 无
* 输入参数：
* 输出参数：
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2011/10         V1.0       曹亮         创建
***************************************************************/
string& AlarmAgent::AlarmACRC2Key(string& str,  T_SyncData *data)
{
    ostringstream   os;

    os << "CRCCODE0 = "  <<  data->dwCRCCode[0] ;

    str = os.str();

    return str;
}
/************************************************************
* 函数名称： OamAlarmAShowAlarmPool()
* 功能描述： 遍历所有告警池
* 访问的表： 无
* 修改的表： 无
* 输入参数： *pShmReg  告警池信息
*            *pUserData  打包告警
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2011/10         V1.0       曹亮         创建
***************************************************************/
void  AlarmAgent::AlarmShowAlarmPool()
{
    /* 基于流水号的告警遍历 */
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
* 函数名称： SendDbgAlarm
* 功能描述： 调试函数发送告警信息
* 访问的表： 无
* 修改的表： 无
* 输入参数：
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2011/10         V1.0       曹亮         创建
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
* 函数名称： DbgShowAgentAlarm
* 功能描述： 调试函数显示AGENT上的告警信息
* 访问的表： 无
* 修改的表： 无
* 输入参数：
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2011/10         V1.0       曹亮         创建
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
