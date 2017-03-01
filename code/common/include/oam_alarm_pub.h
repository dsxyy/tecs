/*********************************************************************
* 版权所有 (C)2008, 深圳市中兴通讯股份有限公司。
*
* 文件名称： OAM_ALARM_PUB.H
* 文件标识：
* 内容摘要：
* 其它说明： 无
* 当前版本： V1.0
* 作    者： 赵剑——3G平台
* 完成日期：
**********************************************************************/
#ifndef _OAM_ALARM_PUB_H_
#define _OAM_ALARM_PUB_H_
/***********************************************************
 *                    其它条件编译选项                     *
***********************************************************/
/***********************************************************
 *                   标准、非标准头文件                    *
***********************************************************/
#include "pub_alarm.h"
#include "vector"
#include "sky.h"

/* 定义VxWorks中的Packed(1)*/
#ifndef _PACKED_1_
#ifndef  VOS_WINNT
#define _PACKED_1_  __attribute__((packed))
#else
#define _PACKED_1_
#endif
#endif

/***********************************************************
 *                        常量定义                         *
***********************************************************/
/* 定义函数返回值 */
#define     OAM_ALARM_FAILED            (BYTE)0
#define     OAM_ALARM_SUCCESSED         (BYTE)1
#define     OAM_ALARM_POOLFULL          (BYTE)2    /* 告警池已满 */
#define     OAM_ALARM_REPEATED          (BYTE)3    /* 收到重复告警信息 */
#define     OAM_ALARM_NEEDREPORT        (BYTE)4    /* 需要上报告警消息 */
#define     OAM_ALARM_WOBBLEREPORT      (BYTE)5    /* 防抖结束需要上报告警恢复消息 */
#define     OAM_ALARM_NOTFOUND          (BYTE)6    /* 没有找到对应的告警 */
#define     OAM_ALARM_OPENFILE          (BYTE)7    /* 打开文件失败 */
#define     OAM_ALARM_WRITEFILE         (BYTE)8    /* 写入文件失败 */
#define     OAM_ALARM_OBJLENERROR       (BYTE)9    /* 告警对象长度错误 */
#define     OAM_ALARM_SEEKFILE          (BYTE)10   /* 定位文件失败 */

/* 定义告警标志 */
#define     OAM_REPORT_ALARM            (BYTE)1
#define     OAM_REPORT_RESTORE          (BYTE)2
#define     OAM_REPORT_INFORM           (BYTE)3

#define     ALARM_PER_PACKET_MAX        (BYTE)20
#define     ALARM_PER_CHANGE_MAX        (BYTE)200
#define     MAX_ALARM_PER_FRAME         (BYTE)200 

#define OAM_ALARM_DELETE_SUCCESS      (BYTE)0
#define OAM_ALARM_MNG_DELETE_FAILED   (BYTE)1
#define OAM_ALARM_AGT_DELETE_FAILED   (BYTE)2

/*恢复类型*/
#define NORMAL_ALARM_RESTORE        1   /*正常恢复*/
#define BOARD_POWERON_ALARM_RESTORE 2   /*单板上电恢复*/
#define SYN_ALARM_RESTORE           3   /*前后台同步时恢复*/
#define BCKGRDDEL_ALARM_RESTORE     4   /*后台删除告警*/
#define PPTOOMP_ALARM_RESTORE       5   /*OMP与单板同步导致的恢复*/
#define BOARD_CFG_DEL_RESTORE       6   /*后台配置删除单板引起告警恢复*/ 
#define JOB_WITHDRAWN_RESTORE       7   /*进程退出服务告警恢复*/
#define ALARM_RELATION_RESTORE      8   /*告警关联恢复*/
#define ALARM_OBJ_DEL_RESTORE       9   /*告警对象删除引起告警恢复*/

/* 告警查询相关宏定义 */
#define  MAX_ALARM_OBJ              (WORD16)120 /* 告警对象长度 */
#define  ALARM_SEARCH_SUCCESS       (WORD32)0 /* 成功 */
#define  ALARM_SEARCH_FAIL          (WORD32)1 /*失败*/
#define  ALARM_SEARCH_WRONG_PAMATER (WORD32)2 /* 查询入参错误 */

#define  ALARM_ADDRESS_LENGTH       (BYTE)64
#define  ALARM_TIME_LENGTH          (BYTE)32

/* 告警池满门限 */
#define  ALARMPOOL_OVERFLOW_ALARM_THRESHOLD     (WORD32)98  /* 告警门限 98%*/
#define  ALARMPOOL_OVERFLOW_RESTORE_THRESHOLD   (WORD32)95  /* 恢复门限 95%*/

/*---------------------------------------------------
告警附加信息的结构由各子系统自己整理,附加信息的规则参考pub_Alarm.h中的说明
长度不能超过ALARM_ADDINFO_UNION_MAX!!!
-------------------------------------------------- */
/**  
    @struct T_AddInfoStruct
    @brief  告警/通知附加信息
    <HR>
    @b 修改记录：
*/
typedef struct 
{
    char               aucMaxAddInfo[ALARM_ADDINFO_UNION_MAX];   /* 附加信息的最大长度 */
}T_AddInfoStruct;

/* 临时定义，保证编译 */
#define     INVALID_OBJTYPE           (WORD16)0XFFFF

/*---------------------------------------------------
         应用进程使用的结构，注意!!!
         对应函数OAM_SendAlarm;
-------------------------------------------------- */
/**  
    @struct T_AlarmAddress
    @brief  告警地址
    <HR>
    @b 修改记录：
*/
typedef struct 
{
    char strSubSystem[ALARM_ADDRESS_LENGTH];
    char strLevel1[ALARM_ADDRESS_LENGTH];  
    char strLevel2[ALARM_ADDRESS_LENGTH];      
    char strLevel3[ALARM_ADDRESS_LENGTH];  
    char strLocation[ALARM_ADDINFO_UNION_MAX];   //位置其他描述信息
}T_AlarmAddress;

/**  
    @struct T_AlarmReport
    @brief  告警上报消息中单个告警实体
    <HR>
    @b 修改记录：
*/
typedef struct
{
/**
    @li @b	参数			dwAlarmCode
    @li @b	类型			WORD32
    @li @b	解释			告警码
*/
    WORD32             dwAlarmCode;
/**
    @li @b	参数			ucAlarmFlag
    @li @b	类型			BYTE
    @li @b	解释			告警标志: 1-告警;2-恢复
*/
    BYTE               ucAlarmFlag;
/**
    @li @b	参数			aucPad
    @li @b	类型			BYTE
    @li @b	解释			填充字节
*/
    T_PAD              aucPad[3];        /* 填充字节 */
/**
    @li @b	参数			wKeyLen
    @li @b	类型			WORD16
    @li @b	解释			告警附加信息结构关键字长度
*/
    WORD16             wKeyLen;
/**
    @li @b	参数			wObjType
    @li @b	类型			WORD16
    @li @b	解释			系统对象类型
*/
    WORD16             wObjType;
/**
    @li @b	参数			T_AddInfoStruct
    @li @b	类型			tAddInfo
    @li @b	解释			告警体内容
*/
    T_AddInfoStruct   tAddInfo;
}T_AlarmReport;

/*---------------------------------------------------
         告警管理模块内部使用的结构
--------------------------------------------------*/
/**  
    @struct T_AgtInform
    @brief  AlarmAgent上报给AlarmManager单个告警通知信息
    <HR>
    @b 修改记录：
*/
typedef struct
{
/**
    @li @b	参数			dwAlarmCode
    @li @b	类型			WORD32
    @li @b	解释			通知码
*/
    WORD32            dwAlarmCode;
/**
    @li @b	参数			tAlarmTime
    @li @b	类型			Datetime
    @li @b	解释			通知上报时间
*/
    Datetime          tAlarmTime;
/**
    @li @b	参数			tAlarmAddr
    @li @b	类型			char
    @li @b	解释			通知发生位置
*/
    char              tAlarmAddr[ALARM_ADDRESS_LENGTH];
/**
    @li @b	参数			tSender
    @li @b	类型			JID
    @li @b	解释			发送通知的JID
*/
//    JID               tSender;
/**
    @li @b	参数			wAlarmObj
    @li @b	类型			WORD16
    @li @b	解释			系统对象类型
*/
    WORD16            wAlarmObj;
/**
    @li @b	参数			aucPad
    @li @b	类型			BYTE
    @li @b	解释			填充字节
*/
    T_PAD             aucPad[6];
/**
    @li @b	参数			tAddInfo
    @li @b	类型			T_AddInfoStruct
    @li @b	解释			通知附加信息内容
*/
    T_AddInfoStruct   tAddInfo;
}T_AgtInform;
/**  
    @struct T_AlarmInfo
    @brief  告警池告警单元中的告警信息（内部使用）
    <HR>
    @b 修改记录：
*/
typedef struct
{
/**
    @li @b	参数			dwAlarmCode
    @li @b	类型			WORD32
    @li @b	解释			告警码
*/
    WORD32            dwAlarmCode;
/**
    @li @b	参数			wObjType
    @li @b	类型			WORD16
    @li @b	解释			系统对象类型
*/
    WORD16            wObjType;
/**
    @li @b	参数			wKeyLen
    @li @b	类型			WORD16
    @li @b	解释			告警附加信息结构关键字长度
*/
    WORD16            wKeyLen;
/**
    @li @b	参数			ucFilterFlag
    @li @b	类型			BYTE
    @li @b	解释			过滤标识
*/
    BYTE              ucFilterFlag;
/**
    @li @b	参数			ucPad
    @li @b	类型			BYTE
    @li @b	解释			填充
*/
    T_PAD             ucPad[3];
/**
    @li @b	参数			tAlarmTime
    @li @b	类型			T_SysSoftClock
    @li @b	解释			告警上报时间
*/
    Datetime          tAlarmTime;
/**
    @li @b	参数			tAlarmLocation
    @li @b	类型			T_AlarmAddress
    @li @b	解释			告警发生位置
*/
    T_AlarmAddress    tAlarmLocation;
/**
    @li @b	参数			tSender
    @li @b	类型			JID
    @li @b	解释			发送告警的JID
*/
//    JID               tSender;
/**
    @li @b	参数			dwCRCCode
    @li @b	类型			WORD32
    @li @b	解释			CRC校验码
*/
    WORD32            dwCRCCode[4];
/**
    @li @b	参数			tAddInfo
    @li @b	类型			T_AddInfoStruct
    @li @b	解释			告警附加信息内容
*/
    T_AddInfoStruct   tAddInfo;
}T_AlarmInfo;

/**  
    @struct T_AgtAlarm
    @brief  AlarmAgent上报给AlarmManager单个告警信息
    <HR>
    @b 修改记录：
*/
typedef struct
{
/**
    @li @b	参数			tAlmInfo
    @li @b	类型			T_AlarmInfo
    @li @b	解释			告警池中告警信息
*/
    T_AlarmInfo       tAlmInfo;
/**
    @li @b	参数			tReportAddr
    @li @b	类型			char
    @li @b	解释			告警上报位置
*/
    char              tReportAddr[ALARM_ADDRESS_LENGTH];
/**
    @li @b	参数			wSendNum
    @li @b	类型			WORD16
    @li @b	解释			告警防抖期间抖动次数
*/
    WORD16            wSendNum;
/**
    @li @b	参数			ucIsLogAlm
    @li @b	类型			BYTE
    @li @b	解释			是否为逻辑告警，0-物理告警 1-逻辑告警
*/
    BYTE              ucIsLogAlm;
/**
    @li @b	参数			ucAlarmFlag
    @li @b	类型			BYTE
    @li @b	解释			1:告警  2:恢复
*/
    BYTE              ucAlarmFlag;
/**
    @li @b	参数			dwAlarmID
    @li @b	类型			WORD32
    @li @b	解释			告警流水号
*/
    WORD32            dwAlarmID;
/**
    @li @b	参数			ucRestoreType
    @li @b	类型			BYTE
    @li @b	解释			恢复类型
*/    
    BYTE              ucRestoreType;
/**
    @li @b	参数			aucPad
    @li @b	类型			BYTE
    @li @b	解释			填充字节
*/    
    T_PAD             aucPad[3];
}T_AgtAlarm;


#endif
