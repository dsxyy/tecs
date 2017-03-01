/*********************************************************************
* ��Ȩ���� (C)2008, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ� OAM_ALARM_PUB.H
* �ļ���ʶ��
* ����ժҪ��
* ����˵���� ��
* ��ǰ�汾�� V1.0
* ��    �ߣ� �Խ�����3Gƽ̨
* ������ڣ�
**********************************************************************/
#ifndef _OAM_ALARM_PUB_H_
#define _OAM_ALARM_PUB_H_
/***********************************************************
 *                    ������������ѡ��                     *
***********************************************************/
/***********************************************************
 *                   ��׼���Ǳ�׼ͷ�ļ�                    *
***********************************************************/
#include "pub_alarm.h"
#include "vector"
#include "sky.h"

/* ����VxWorks�е�Packed(1)*/
#ifndef _PACKED_1_
#ifndef  VOS_WINNT
#define _PACKED_1_  __attribute__((packed))
#else
#define _PACKED_1_
#endif
#endif

/***********************************************************
 *                        ��������                         *
***********************************************************/
/* ���庯������ֵ */
#define     OAM_ALARM_FAILED            (BYTE)0
#define     OAM_ALARM_SUCCESSED         (BYTE)1
#define     OAM_ALARM_POOLFULL          (BYTE)2    /* �澯������ */
#define     OAM_ALARM_REPEATED          (BYTE)3    /* �յ��ظ��澯��Ϣ */
#define     OAM_ALARM_NEEDREPORT        (BYTE)4    /* ��Ҫ�ϱ��澯��Ϣ */
#define     OAM_ALARM_WOBBLEREPORT      (BYTE)5    /* ����������Ҫ�ϱ��澯�ָ���Ϣ */
#define     OAM_ALARM_NOTFOUND          (BYTE)6    /* û���ҵ���Ӧ�ĸ澯 */
#define     OAM_ALARM_OPENFILE          (BYTE)7    /* ���ļ�ʧ�� */
#define     OAM_ALARM_WRITEFILE         (BYTE)8    /* д���ļ�ʧ�� */
#define     OAM_ALARM_OBJLENERROR       (BYTE)9    /* �澯���󳤶ȴ��� */
#define     OAM_ALARM_SEEKFILE          (BYTE)10   /* ��λ�ļ�ʧ�� */

/* ����澯��־ */
#define     OAM_REPORT_ALARM            (BYTE)1
#define     OAM_REPORT_RESTORE          (BYTE)2
#define     OAM_REPORT_INFORM           (BYTE)3

#define     ALARM_PER_PACKET_MAX        (BYTE)20
#define     ALARM_PER_CHANGE_MAX        (BYTE)200
#define     MAX_ALARM_PER_FRAME         (BYTE)200 

#define OAM_ALARM_DELETE_SUCCESS      (BYTE)0
#define OAM_ALARM_MNG_DELETE_FAILED   (BYTE)1
#define OAM_ALARM_AGT_DELETE_FAILED   (BYTE)2

/*�ָ�����*/
#define NORMAL_ALARM_RESTORE        1   /*�����ָ�*/
#define BOARD_POWERON_ALARM_RESTORE 2   /*�����ϵ�ָ�*/
#define SYN_ALARM_RESTORE           3   /*ǰ��̨ͬ��ʱ�ָ�*/
#define BCKGRDDEL_ALARM_RESTORE     4   /*��̨ɾ���澯*/
#define PPTOOMP_ALARM_RESTORE       5   /*OMP�뵥��ͬ�����µĻָ�*/
#define BOARD_CFG_DEL_RESTORE       6   /*��̨����ɾ����������澯�ָ�*/ 
#define JOB_WITHDRAWN_RESTORE       7   /*�����˳�����澯�ָ�*/
#define ALARM_RELATION_RESTORE      8   /*�澯�����ָ�*/
#define ALARM_OBJ_DEL_RESTORE       9   /*�澯����ɾ������澯�ָ�*/

/* �澯��ѯ��غ궨�� */
#define  MAX_ALARM_OBJ              (WORD16)120 /* �澯���󳤶� */
#define  ALARM_SEARCH_SUCCESS       (WORD32)0 /* �ɹ� */
#define  ALARM_SEARCH_FAIL          (WORD32)1 /*ʧ��*/
#define  ALARM_SEARCH_WRONG_PAMATER (WORD32)2 /* ��ѯ��δ��� */

#define  ALARM_ADDRESS_LENGTH       (BYTE)64
#define  ALARM_TIME_LENGTH          (BYTE)32

/* �澯�������� */
#define  ALARMPOOL_OVERFLOW_ALARM_THRESHOLD     (WORD32)98  /* �澯���� 98%*/
#define  ALARMPOOL_OVERFLOW_RESTORE_THRESHOLD   (WORD32)95  /* �ָ����� 95%*/

/*---------------------------------------------------
�澯������Ϣ�Ľṹ�ɸ���ϵͳ�Լ�����,������Ϣ�Ĺ���ο�pub_Alarm.h�е�˵��
���Ȳ��ܳ���ALARM_ADDINFO_UNION_MAX!!!
-------------------------------------------------- */
/**  
    @struct T_AddInfoStruct
    @brief  �澯/֪ͨ������Ϣ
    <HR>
    @b �޸ļ�¼��
*/
typedef struct 
{
    char               aucMaxAddInfo[ALARM_ADDINFO_UNION_MAX];   /* ������Ϣ����󳤶� */
}T_AddInfoStruct;

/* ��ʱ���壬��֤���� */
#define     INVALID_OBJTYPE           (WORD16)0XFFFF

/*---------------------------------------------------
         Ӧ�ý���ʹ�õĽṹ��ע��!!!
         ��Ӧ����OAM_SendAlarm;
-------------------------------------------------- */
/**  
    @struct T_AlarmAddress
    @brief  �澯��ַ
    <HR>
    @b �޸ļ�¼��
*/
typedef struct 
{
    char strSubSystem[ALARM_ADDRESS_LENGTH];
    char strLevel1[ALARM_ADDRESS_LENGTH];  
    char strLevel2[ALARM_ADDRESS_LENGTH];      
    char strLevel3[ALARM_ADDRESS_LENGTH];  
    char strLocation[ALARM_ADDINFO_UNION_MAX];   //λ������������Ϣ
}T_AlarmAddress;

/**  
    @struct T_AlarmReport
    @brief  �澯�ϱ���Ϣ�е����澯ʵ��
    <HR>
    @b �޸ļ�¼��
*/
typedef struct
{
/**
    @li @b	����			dwAlarmCode
    @li @b	����			WORD32
    @li @b	����			�澯��
*/
    WORD32             dwAlarmCode;
/**
    @li @b	����			ucAlarmFlag
    @li @b	����			BYTE
    @li @b	����			�澯��־: 1-�澯;2-�ָ�
*/
    BYTE               ucAlarmFlag;
/**
    @li @b	����			aucPad
    @li @b	����			BYTE
    @li @b	����			����ֽ�
*/
    T_PAD              aucPad[3];        /* ����ֽ� */
/**
    @li @b	����			wKeyLen
    @li @b	����			WORD16
    @li @b	����			�澯������Ϣ�ṹ�ؼ��ֳ���
*/
    WORD16             wKeyLen;
/**
    @li @b	����			wObjType
    @li @b	����			WORD16
    @li @b	����			ϵͳ��������
*/
    WORD16             wObjType;
/**
    @li @b	����			T_AddInfoStruct
    @li @b	����			tAddInfo
    @li @b	����			�澯������
*/
    T_AddInfoStruct   tAddInfo;
}T_AlarmReport;

/*---------------------------------------------------
         �澯����ģ���ڲ�ʹ�õĽṹ
--------------------------------------------------*/
/**  
    @struct T_AgtInform
    @brief  AlarmAgent�ϱ���AlarmManager�����澯֪ͨ��Ϣ
    <HR>
    @b �޸ļ�¼��
*/
typedef struct
{
/**
    @li @b	����			dwAlarmCode
    @li @b	����			WORD32
    @li @b	����			֪ͨ��
*/
    WORD32            dwAlarmCode;
/**
    @li @b	����			tAlarmTime
    @li @b	����			Datetime
    @li @b	����			֪ͨ�ϱ�ʱ��
*/
    Datetime          tAlarmTime;
/**
    @li @b	����			tAlarmAddr
    @li @b	����			char
    @li @b	����			֪ͨ����λ��
*/
    char              tAlarmAddr[ALARM_ADDRESS_LENGTH];
/**
    @li @b	����			tSender
    @li @b	����			JID
    @li @b	����			����֪ͨ��JID
*/
//    JID               tSender;
/**
    @li @b	����			wAlarmObj
    @li @b	����			WORD16
    @li @b	����			ϵͳ��������
*/
    WORD16            wAlarmObj;
/**
    @li @b	����			aucPad
    @li @b	����			BYTE
    @li @b	����			����ֽ�
*/
    T_PAD             aucPad[6];
/**
    @li @b	����			tAddInfo
    @li @b	����			T_AddInfoStruct
    @li @b	����			֪ͨ������Ϣ����
*/
    T_AddInfoStruct   tAddInfo;
}T_AgtInform;
/**  
    @struct T_AlarmInfo
    @brief  �澯�ظ澯��Ԫ�еĸ澯��Ϣ���ڲ�ʹ�ã�
    <HR>
    @b �޸ļ�¼��
*/
typedef struct
{
/**
    @li @b	����			dwAlarmCode
    @li @b	����			WORD32
    @li @b	����			�澯��
*/
    WORD32            dwAlarmCode;
/**
    @li @b	����			wObjType
    @li @b	����			WORD16
    @li @b	����			ϵͳ��������
*/
    WORD16            wObjType;
/**
    @li @b	����			wKeyLen
    @li @b	����			WORD16
    @li @b	����			�澯������Ϣ�ṹ�ؼ��ֳ���
*/
    WORD16            wKeyLen;
/**
    @li @b	����			ucFilterFlag
    @li @b	����			BYTE
    @li @b	����			���˱�ʶ
*/
    BYTE              ucFilterFlag;
/**
    @li @b	����			ucPad
    @li @b	����			BYTE
    @li @b	����			���
*/
    T_PAD             ucPad[3];
/**
    @li @b	����			tAlarmTime
    @li @b	����			T_SysSoftClock
    @li @b	����			�澯�ϱ�ʱ��
*/
    Datetime          tAlarmTime;
/**
    @li @b	����			tAlarmLocation
    @li @b	����			T_AlarmAddress
    @li @b	����			�澯����λ��
*/
    T_AlarmAddress    tAlarmLocation;
/**
    @li @b	����			tSender
    @li @b	����			JID
    @li @b	����			���͸澯��JID
*/
//    JID               tSender;
/**
    @li @b	����			dwCRCCode
    @li @b	����			WORD32
    @li @b	����			CRCУ����
*/
    WORD32            dwCRCCode[4];
/**
    @li @b	����			tAddInfo
    @li @b	����			T_AddInfoStruct
    @li @b	����			�澯������Ϣ����
*/
    T_AddInfoStruct   tAddInfo;
}T_AlarmInfo;

/**  
    @struct T_AgtAlarm
    @brief  AlarmAgent�ϱ���AlarmManager�����澯��Ϣ
    <HR>
    @b �޸ļ�¼��
*/
typedef struct
{
/**
    @li @b	����			tAlmInfo
    @li @b	����			T_AlarmInfo
    @li @b	����			�澯���и澯��Ϣ
*/
    T_AlarmInfo       tAlmInfo;
/**
    @li @b	����			tReportAddr
    @li @b	����			char
    @li @b	����			�澯�ϱ�λ��
*/
    char              tReportAddr[ALARM_ADDRESS_LENGTH];
/**
    @li @b	����			wSendNum
    @li @b	����			WORD16
    @li @b	����			�澯�����ڼ䶶������
*/
    WORD16            wSendNum;
/**
    @li @b	����			ucIsLogAlm
    @li @b	����			BYTE
    @li @b	����			�Ƿ�Ϊ�߼��澯��0-����澯 1-�߼��澯
*/
    BYTE              ucIsLogAlm;
/**
    @li @b	����			ucAlarmFlag
    @li @b	����			BYTE
    @li @b	����			1:�澯  2:�ָ�
*/
    BYTE              ucAlarmFlag;
/**
    @li @b	����			dwAlarmID
    @li @b	����			WORD32
    @li @b	����			�澯��ˮ��
*/
    WORD32            dwAlarmID;
/**
    @li @b	����			ucRestoreType
    @li @b	����			BYTE
    @li @b	����			�ָ�����
*/    
    BYTE              ucRestoreType;
/**
    @li @b	����			aucPad
    @li @b	����			BYTE
    @li @b	����			����ֽ�
*/    
    T_PAD             aucPad[3];
}T_AgtAlarm;


#endif
