/*********************************************************************
* ��Ȩ���� (C)2010, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ� usp_capi.h
* �ļ���ʶ�� �����ù���ƻ���
* ����ժҪ�� USP��Ŀ�ṹ�嶨�� & �ӿ�����
* ����˵���� ��
* ��ǰ�汾�� 0.1
* ��    �ߣ� tao.linjun@zte.com.cn
* ������ڣ� 2010-11-2
**********************************************************************/

#ifndef __USP_CAPI_H__
#define __USP_CAPI_H__

#include "usp_common_macro.h"

#if defined(LINK_OSS)

#include "tulip.h"

#ifdef _PACKED_1_
/* ȡ����ǰ��_PACKED_1_�궨�� */
#undef _PACKED_1_
/* ���¶����_PACKED_1_ */
#if defined(WIN32)
#define _PACKED_1_
#else
#define _PACKED_1_ __attribute__((packed))
#endif
#endif

#else

#include "usp_common_typedef.h"
#if defined(WIN32)
#define _PACKED_1_
#else
#define _PACKED_1_ __attribute__((packed))
#endif

#endif

#include "usp_capi_lib.h"


#if 0

typedef struct
{
    SWORD32 sdwYear;
    SWORD32 sdwMon;
    SWORD32 sdwDay;
    SWORD32 sdwHour;
    SWORD32 sdwMin;
    SWORD32 sdwSec;
}_PACKED_1_ T_Time;

typedef struct
{
    CHAR ucName[FILE_PATH_LEN]; /* ����·�� */
    BYTE ucFileType;            /* �ļ����� */
}_PACKED_1_ T_FileName;


typedef struct
{
    T_Time tTime;           //�ƻ������һ����������,���sdwSchedPeriodΪ��SCHED_IMMEDIATELY������ʱ�䲻��ָ��
    SWORD32  sdwSchedPeriod; //�ƻ���������ڣ�ȡֵSCHED_IMMEDIATELY��SCHED_ONCE��SCHED_DAYLY��SCHED_WEEKLY��SCHED_MONTHLY
}_PACKED_1_ T_Schedule;

/********************************* Time ***********************************/


typedef struct
{
    CHAR scNtpServerAddr[NTP_HOST_NAME_LEN];    /* NTP��������ַ */
}_PACKED_1_ T_NTPServerInfo;

typedef struct
{
    CHAR scZoneCityName[TIME_ZONE_CITY_NAME_LEN];         /* ʱ�������� */
}_PACKED_1_ T_TimeZoneCityName;

/*
 * ���ṹ���ֶ��޸�ΪSWORD����, ������ʱ��ʱ, ���ֻ����date, ��hour��minute��second��ʼ��Ϊ-1;
 * ���ֻ����time, ��year��month��day��ʼ��Ϊ-1
*/
typedef struct
{
    SWORD32 dwTmYear;    /* �� ,��Ԫ����,����2011*/
    SWORD32 dwTmMon;     /* �� ,ʵ���·�*/
    SWORD32 dwTmDay;     /* �� */
    SWORD32 dwTmHour;    /* ʱ */
    SWORD32 dwTmMin;      /* �� */
    SWORD32 dwTmSec;      /* �� */
}_PACKED_1_ T_SysTime;    /* ϵͳʱ�� */

typedef struct
{
    T_TimeZoneCityName tCityName;  /* ʱ��������*/
    SWORD32 sdwNtpStatus;              /* �Ƿ�ʱͬ�� ���Զ�:NTP_AUTO(1);  �ֶ� :NTP_MANUAL(0)*/
    T_NTPServerInfo tServerAddr;      /* NTP��������ַ*/
}_PACKED_1_ T_NtpInfo;

typedef struct
{
    T_SysTime tSysTime;             /* ϵͳʱ�� */
    T_TimeZoneCityName tZoneCity;  /*ʱ��������*/
}_PACKED_1_ T_TimeInfo;  /* ����ʱ���ʱ���Ļ�ȡ */

typedef struct
{
 //   SWORD32 sdwDebug;    /* 1 -- ����ģʽ, ֻ��ȡʱ��������ϵ�ʱ��, ��������ϵͳʱ�� */
    T_NTPServerInfo tServerInfo;     /* NTP��������ַ*/
}_PACKED_1_ T_NtpSync;    /* NTPʱ��ͬ������ */

typedef struct
{
    SWORD32 sdwCityNum;        /* ϵͳ�ṩ��ʱ��������Ŀ */
    T_TimeZoneCityName tZoneCity[TIME_ZONE_CITY_NUM];      /*ʱ�������б�*/
}_PACKED_1_ T_ZoneCityList;

/***************************************DM*************************************/
typedef struct
{
    SWORD32  sdwSlotId;      /* ���̲�λ�ţ�ȡֵ0~27(ScanBox);  0~11(SPR10) */
    SWORD32  sdwEnclosureId;  /* ����Id��ȡֵ 0~ʵ��������*/
}_PACKED_1_ T_PdId;

typedef struct
{
    SWORD32  sdwNum;     /* ���̸�����ȡֵMAX_ENC_NUM_IN_SYS(4) * MAX_PD_NUM_IN_ENC(16) */
    T_PdId       tPdId[MAX_PD_NUM_IN_SYS];
}_PACKED_1_ T_AllPdId;

#if 0
typedef struct
{
    SWORD32  sdwSlotId;     /* ���̲�λ�ţ�ȡֵ0~15 */
    SWORD32  sdwEnclosureId;/* ����Id��ȡֵ 0~ʵ��������*/
    SWORD64  sqwCapacity;   /* �������� */
    SWORD32  sdwCache;      /* Ӳ��cache״̬ 1 �C ʹ��, 0 �C ��ֹ, -1 �C��֧�� */
    SWORD32  sdwPowerSave;  /* Ӳ�̽���ģʽ״̬1 �C ʹ��,0 �C ��ֹ,-1 �C��֧�� */
    SWORD32  sdwSpeed;      /* Ӳ�̹����������á�-1 ��֧�� */
    SWORD32  sdwNcq;        /* NCQ���أ�ֻ��SATA����Ч. 1 �C ��, 0 �C �ر�, -1 ��֧�� */
    /* SWORD32  sdwBadSecsFind;      */                                  /*��⵽�Ļ�������Ŀ */

    CHAR     ucModelName[MAX_PD_MODEL_NAME_LEN];    /* model���� */
    CHAR     ucPhyType[MAX_PD_TYPE_LEN];            /* Ӳ����������(e.g. SATA1, SATA2, SAS, FC) */
    CHAR     ucFwVer[MAX_PD_FW_VER_LEN];            /* Firmware�汾.*/
    CHAR     ucSerialNo[MAX_PD_SNO_LEN];            /* ���к� */
    CHAR     ucStdVerNum[MAX_PD_STD_VER_LEN];       /* ��׼�汾��. */
    SWORD32  sdwReady;                               /* Ӳ��ʹ��ģʽ TRUE - ����, FALSE - ���� */
    SWORD32  sdwHealthState;                      /* ���̵Ľ���״̬����SMART ״̬��ȡֵΪPD_GOOD(1)��PD_BROKEN(0)*/
}_PACKED_1_ T_PdHwInfo;


typedef struct
{
    SWORD32  sdwSlotId;      /* ���̲�λ�ţ�ȡֵ0~27 */
    SWORD32  sdwEnclosureId; /* Ӳ�����ڵĹ���id */
    SWORD32  sdwPdSrc;       /* ePdSrc   eSrc;ȡֵPD_LOCAL(0), PD_FOREIGN(2)*/
    SWORD32  sdwPdState;     /* ePdState eState;ȡֵPD_CAN_NOT_READ(1), PD_NORMAL_RW(0) */
    SWORD32  sdwPdType;      /* ePdType  eType;ȡֵPD_UNKOWN(3),PD_UNUSED(0),PD_HOTSPARE(4), PD_DATA(5)��PD_CONFLICT(6)*/
    SWORD32  sdwVdId;        /* ������Id��ȡֵ��Χ0~MAX_VD_ID(32)   */
    SWORD32  sdwVisibleCtrl;   /*  �Ը��̿ɼ��Ŀ�������״̬*/
}_PACKED_1_ T_PdSwInfo;

#endif

typedef struct
{
    SWORD32  sdwSlotId;     /* ���̲�λ�ţ�ȡֵ0~27(ScanBox);  0~11(SPR10) */
    SWORD32  sdwEnclosureId;/* Ӳ�����ڵĹ���id */
    SWORD32  sdwCache;      /* Ӳ��cache״̬ 1 �C ʹ��, 0 �C ��ֹ, -1 �C��֧�� */
    SWORD32  sdwPowerSave;  /* Ӳ�̽���ģʽ״̬1 �C ʹ��,0 �C ��ֹ,-1 �C��֧�� */
    SWORD32  sdwSpeed;      /* Ӳ�̹����������á�-1 ��֧�� */
}_PACKED_1_ T_PdSetHwInfo;



typedef struct
{
    SWORD32  sdwSlotId;     /* ���̲�λ�ţ�ȡֵ0~27(ScanBox);  0~11(SPR10) */
    SWORD32  sdwEnclosureId;/* ����Id��ȡֵ 0~ʵ��������*/
    SWORD64  sqwCapacity;   /* �������� */
    SWORD32  sdwCache;      /* Ӳ��cache״̬ 1 �C ʹ��, 0 �C ��ֹ, -1 �C��֧�� */
    SWORD32  sdwPowerSave;  /* Ӳ�̽���ģʽSCS_DM_DISK_POWER_ACTIVE  SCS_DM_DISK_POWER_IDLE SCS_DM_DISK_POWER_STANDBY SCS_DM_DISK_POWER_STOP_SLEEP*/
    SWORD32  sdwSpeed;      /* Ӳ�̹����������á�-1 ��֧�� */
    CHAR     ucModelName[MAX_PD_MODEL_NAME_LEN];    /* model���� */
    CHAR     ucPhysicalType[MAX_PD_TYPE_LEN];            /* Ӳ����������(e.g. SATA1, SATA2, SAS, FC) */
    CHAR     ucFwVer[MAX_PD_FW_VER_LEN];            /* Firmware�汾.*/
    CHAR     ucSerialNo[MAX_PD_SNO_LEN];            /* ���к� */
    CHAR     ucStdVerNum[MAX_PD_STD_VER_LEN];       /* ��׼�汾��. */
    SWORD32  sdwHealthState;                      /* ���̵Ľ���״̬�������ɷ����Ժ�SMART ״̬��ȡֵΪPD_GOOD(1)��PD_BROKEN(0)*/

    SWORD32  sdwSourceType;       /* ePdSrc   eSrc;ȡֵPD_LOCAL(0), PD_FOREIGN(2)*/
    SWORD32  sdwLogicType;   /* ePdType  eType;ȡֵPD_UNKOWN(3),PD_UNUSED(0),PD_HOTSPARE(4), PD_DATA(5)��PD_CONFLICT(6)*/
    CHAR  cVdName[MAX_BLK_NAME_LEN];            /* ��������������*/
    SWORD32  sdwVisibleCtrl;   /*  �Ը��̿ɼ��Ŀ�������״̬*/
}_PACKED_1_ T_HddInfo;



typedef struct
{
    SWORD32 sdwPdSlotId;     /* ���̲�λ�ţ�ȡֵ0~27(ScanBox);  0~11(SPR10) */
    SWORD32 sdwPdEnclosureId; /* ����Id */
    SWORD32 sdwPercent;       /* ɨ�����ֵ�������ٷֺ� */
}_PACKED_1_ T_PdScanProgress;

typedef struct
{
    SWORD32 sdwPdNum;     /* ���̲�λ�ţ�ȡֵ0~27(ScanBox);  0~11(SPR10) */
    T_PdScanProgress tPdScanProgress[MAX_PD_NUM_IN_SYS];
}_PACKED_1_ T_AllPdScanProgress;


typedef struct
{
    SWORD32  sdwSlotId;      /* ���̲�λ�ţ�ȡֵ0~27(ScanBox);  0~11(SPR10) */
    SWORD32  sdwEnclosureId;  /* ����Id��ȡֵ 0~ʵ��������*/
    SWORD32  sdwCtrlFlg;          /* ɨ����Ʊ�־��ȡֵ:PD_SCAN_START(0)��PD_SCAN_PAUSE(1)��PD_SCAN_STOP(2)*/
    T_Schedule tSchedule;         /* ������ֵȱʡʱ������ɨ������ִ��*/
}_PACKED_1_ T_PdScanCtrl;

typedef struct
{
    SWORD32 sdwPdSlotId;      /* ���̲�λ�ţ�ȡֵ0~27(ScanBox);  0~11(SPR10) */
    SWORD32 sdwPdEnclosureId;  /* ����Id */
    SWORD32 sdwControl;        /* ���Ʒ�ʽ��ON-��������ָʾ��OFF-�رտ���ָʾ */
}_PACKED_1_ T_PdLed;


typedef struct
{
    SWORD32 sdwWarnSwitch;       /* ���Ʒ�ʽ��ON-������OFF-�ر� */
}_PACKED_1_ T_PdWarnSwitch;

typedef struct
{
    SWORD32 sdwPdEnclosureId;  /* ����Id */
    SWORD32 sdwPdSlotId;            /* ���̲�λ�ţ�ȡֵ0~27(ScanBox);  0~11(SPR10) */
    WORD32 dwFinishRate;  /* ����ɨ��������-1--100 */
    WORD32 dwBblNum;  /* ����ɨ�軵����Ŀ */

}_PACKED_1_ T_PdScanStatus;

typedef struct
{
    WORD32 sdwPdBadBlockWarnNum;            /* ϵͳ�д��̵ĸ澯������Ŀ��ȡֵ:>=0 */
}_PACKED_1_ T_PdBadBlockWarnNum;

/* �˽ṹ��ο���smart���ߵ���� */
typedef struct
{
    BYTE ucId;
    WORD16 uwFlags;
    BYTE ucCurrent;
    BYTE ucWorst;
    WORD64 ucRaw;
    BYTE ucThres;
}_PACKED_1_ T_DmSmartDetailInfo;

typedef struct
{
    SWORD32 sdwPdSlotId;            /* ���̲�λ�ţ�ȡֵ0~27(ScanBox);  0~11(SPR10) */
    SWORD32 sdwPdEnclosureId;  /* ����Id */
    BYTE ucAucPdName[12];   /* �����豸�� ��sda */
    SWORD32 cIsSupportSmart;  /* �����Ƿ�֧��SMART */
    SWORD32 cIsSmartEnable;   /* ����SMART�����Ƿ��� */
    SWORD32 cSmartStatus;     /* ���̵�SMART״̬�Ƿ�OK */
    SWORD32 dwSmartInfoCount;/*SMART���Ը���*/
    T_DmSmartDetailInfo tSmartDetailInfo[30];   /* ����SMART������ϸ��Ϣ */
    SWORD32 sdwSelfTestTime;  /* ִ���Լ�����ʱ�� */
}_PACKED_1_  T_PdSmartStateInfo;


typedef struct
{
    SWORD32 sdwPdSlotId;            /* ���̲�λ�ţ�ȡֵ0~27(ScanBox);  0~11(SPR10) */
    SWORD32 sdwPdEnclosureId;       /* Ӳ�����ڵĹ���id */
    WORD32 dwRawReadErrorRate;      /* 01 ���ݶ�ȡ������ �֮Ӧ�ô��ڷ���ֵ */
    WORD32 dwSpinUpTime;            /* 03 �����ת����׼ת�������ʱ�� ����Ϊ��λ */
    WORD32 dwStartStopCount;        /* 04 ������ֹͣ���� �����ο����޷���ֵ */
    WORD32 dwRealloctedSectorsCount;    /* 05 ���·������������ԽСԽ�� */
    WORD32 dwSeekErrorRate;             /* 07 Ѱ�������� */
    WORD32 dwPowerOnHours;              /* 09 �ۼ�ͨ��ʱ��  Խ��Խ�ã��޷���ֵ */
    WORD32 dwSpinRetryCount;            /* 10 ������Դ��� Խ��Խ�ã��޷���ֵ */
    WORD32 dwCalibrationRetryCount;     /* 11 У׼���Դ���  Խ��Խ�ã��޷���ֵ */
    WORD32 dwPowerCycleCount;           /* 12 ͨ�����ڼ�����Ӳ�̿��ص�Դ�Ĵ����� �����ο����޷���ֵ */
    WORD32 dwPowerOffRetractCount;      /* 192 �ϵ��ͷ���ؼ��� Խ��Խ�� ������ֵ ��ʿͨר�� */
    WORD32 dwLoadCycleCount;            /* 193 ��ͷ������ڼ��� */
    WORD32 dwTemperature;               /* 194 �¶� */
    WORD32 dwRealloctedEventCount;      /* 196 ����ӳ��������¼����� */
    WORD32 dwCurrentPendingSector;      /* 197 ��ǰ��ӳ��������� */
    WORD32 dwUncorrectableSectorCount;  /* 198 �ѻ��޷�У��������� */
    WORD32 dwUDMACrcErrorCount;         /* 199 Ultra DMA CRC������� */
    WORD32 dwWriteErrorRate;            /* 200 д�����Ĵ������ Խ��Խ�� */
}_PACKED_1_ T_PdSmartInfo;



typedef struct
{
    SWORD32     sdwPdNum;                     /* ���̸�����ȡֵMAX_ENC_NUM_IN_SYS(4) * MAX_PD_NUM_IN_ENC(16) */
    T_HddInfo   tHddInfo[MAX_PD_NUM_IN_SYS];
}_PACKED_1_ T_AllHddInfo;


/***************************************Blk*************************************/

/***************************************Vol*************************************/
/* BLK��SCS����ͨ�øýṹ�� �û�����Ҫ���Owner*/
typedef struct
{
    CHAR     cVdName[MAX_BLK_NAME_LEN];  /*VD ����,����Ϊ��*/
    CHAR     cVolName[MAX_BLK_NAME_LEN]; /* Vol���ƣ�����Ϊ��*/
    SWORD32  sdwChunkSize;  /* new chunk size,��Ϊ4�ı�������λ:KB�� ȡֵMIN_CHUNK_SIZE(4)~MAX_CHUNK_SIZE(512 )*/
    SWORD64  sqwCapacity;   /* capacity of vol in sectors, KB */
    SWORD32  sdwCtlPrefer;  /* ��ѡ������,��ʱΪ0,1 */
    SWORD32  sdwCachePolicy;       /* CACHE д��д������, ȡֵCACHE_WRITE_BACK_POLICY(0),CACHE_WRITE_THROUGH_POLICY(1) */
    SWORD32  sdwAheadReadSize;  /* cacheԤ��ϵ��,ȡֵ0~2048 ���޵�λ*/
}_PACKED_1_ T_CreateVol;

/* BLK��SCS����ͨ�øýṹ�� */
typedef struct
{

    CHAR     cVdName[MAX_BLK_NAME_LEN];  /*VD ����,����Ϊ�գ����*/
    SWORD32   sdwVolType;                   /* �����ͣ�������ͨ��NORM_VOL�������վ�SNAPSHOT_VOL����������CLONE_VOL�� */
    SWORD32  sdwVolNum;           /* �����Ŀ */
    CHAR     cVolNames[MAX_VOL_NUM_ON_VD][MAX_BLK_NAME_LEN];   /* ָ��VD �ϵľ������б� */
}_PACKED_1_ T_AllVolNamesOnVd;


typedef struct
{
  CHAR   cVolName[MAX_BLK_NAME_LEN]; /* Vol���ƣ�����Ϊ�գ����*/
  WORD32 sdwReadTotalSectors;   /* ����������*/
  WORD32 sdwWriteTotalSectors;  /* д��������*/
  WORD32 sdwReadTotalIos;            /* ��IO�ܴ���*/
  WORD32 sdwWriteTotalIos;            /* дIO�ܴ���*/

  WORD32 sdwReadErrorSectors;   /* ������������Ŀ*/
  WORD32 sdwWriteErrorSectors;  /* д����������Ŀ*/
  WORD32 sdwReadErrorIos;            /* ������IO ��Ŀ*/
  WORD32 sdwWriteErrorIos;            /* д����IO ��Ŀ*/
}_PACKED_1_ T_GetVolStat;


/* BLK��SCS����ͨ�øýṹ�� */
typedef struct
{
    CHAR    cVolName[MAX_BLK_NAME_LEN];  /* Vol Name ����� */
}_PACKED_1_ T_VolName;

typedef struct
{
    CHAR    cVolName[MAX_BLK_NAME_LEN];    /* Vol���ƣ�����Ϊ�� �����*/
    CHAR    cVolNewName[MAX_BLK_NAME_LEN]; /* ���޸ĵ�Vol���ƣ�����Ϊ�գ����*/
}_PACKED_1_ T_VolRename;

/* BLK��SCS����ͨ�øýṹ�� */
typedef struct
{

    CHAR     cVolName[MAX_BLK_NAME_LEN]; /* Vol���ƣ�����Ϊ�գ����*/
    SWORD64  sqwExpandSize; /* expand size  ����λKB �����*/
}_PACKED_1_ T_ExpVol;

/* BLK��SCS����ͨ�øýṹ�� */
typedef struct
{
    CHAR     cVolName[MAX_BLK_NAME_LEN]; /* Vol���ƣ�����Ϊ�գ����*/
    SWORD32  sdwNewChunk; /* new chunk size,��Ϊ4�ı���,��λ:KB �� ȡֵMIN_CHUNK_SIZE(4)~MAX_CHUNK_SIZE(512 )�����*/
}_PACKED_1_ T_ReSegVol;


/* BLK��SCS����ͨ�øýṹ�� ���һ����Ա��SCS���*/
typedef struct
{
    CHAR      cVolName[MAX_BLK_NAME_LEN]; /* Vol����,����Ϊ��*/
    CHAR      cVdName[MAX_BLK_NAME_LEN];  /*VD ����,����Ϊ��*/
    SWORD64   sqwCapacity;  /* vol capacity  ,KB*/
    SWORD32   sdwLevel;     /* eRaidlevel, RAID level, defined in struct eRaidlevel ,ȡֵRAID0(0),RAID1(1),RAID3(3), RAID5(5),RAID6(6),
                                                   RAIDTP(7),RAID10(10),RAID30(30),RAID50(50),RAID60(60),NRAID(200),RAID_LEVEL_INVALID(0xffff) */
    SWORD32   sdwState;     /* eVolState , vol state defined in struct eVolState,ȡֵVOL_S_GOOD(0),
                                               VOL_S_FAULT(1),  VOL_S_ERR(2),VOL_S_CHUNKSIZE_MIGRATION(3),
                                               VOL_S_EXPAND(4),VOL_S_UNREADY(5),VOL_S_INVALID(0xffff) */
    SWORD32   sdwStripeSize; /* vol chunk size ,KB*/
    SWORD32   sdwOwnerCtrl;                   /* owner control ,��ʱΪ0,1 */
    SWORD32   sdwPreferCtrl;   /*  ��ѡ������,��ʱΪ0,1 */
    T_Time    tCreateTime;    /* ����ʱ��  */
    SWORD32   sdwMapState;    /* ���ӳ��״̬��ȡֵMAPPED_VOL(1),NO_MAPPED_VOL(0) */
    SWORD32   sdwCachePolicy; /* CACHE д��д������, ȡֵCACHE_WRITE_BACK_POLICY(0),CACHE_WRITE_THROUGH_POLICY(1) */
    SWORD32   sdwAheadReadSize;/* cacheԤ��ϵ��,ȡֵ0~2048 */
    SWORD32   sdwSvolNum;   /* Դ���Ͽ��վ�ĸ��� ������*/
    SWORD32   sdwCloneNum;   /* Դ���Ͽ�¡��ĸ��� ������*/
}_PACKED_1_ T_VolInfo;

typedef struct
{
    CHAR    cVolName[MAX_BLK_NAME_LEN]; /* Vol����,����Ϊ��,���*/
    SWORD32 sdwPrefeCtl;                /* ��ѡ������,��ʱΪ0,1  �����*/
}_PACKED_1_ T_VolPreferCtrl;


/* BLK��SCS����ͨ�øýṹ�� */
typedef struct
{
    CHAR     cVolName[MAX_BLK_NAME_LEN]; /* Vol����,����Ϊ��,  ���*/
    SWORD32  sdwTaskType;  /* eVolTaskType, task type, defined in struct eLdTaskType,
                                               ȡֵVOL_TASK_CHUNK(0),VOL_TASK_RESHAPE(1) */
    T_Time   tStartTime;  /* ��ʼʱ��  */
    SWORD32  sdwPercent;   /* progress of task */
    T_Time   tFinishTime; /* ����ʱ��  */
    /* BYTE ucDelay;  */    /* reserved */
}_PACKED_1_ T_VolGetTaskInfo;


typedef struct
{
    CHAR    cVolName[MAX_BLK_NAME_LEN]; /* Vol/Svol����,����Ϊ��,���*/
    SWORD32 sdwCachePolicy;  /* CACHE д��д������, ȡֵCACHE_WRITE_BACK_POLICY(0),CACHE_WRITE_THROUGH_POLICY(1) �����*/
    SWORD32 sdwAheadReadSize;/* cacheԤ��ϵ�� �����*/
}_PACKED_1_ T_VolCacheInfo;

/***************************************SNAP************************************/
/* BLK��SCS����ͨ�øýṹ�� */
typedef struct
{

    CHAR     cVolName[MAX_BLK_NAME_LEN];    /* Vol����*/
    CHAR     cSvolName[MAX_BLK_NAME_LEN];   /* ���վ����� */
    SWORD32  sdwSvolChunkSize;              /*Svol chunk size ,���վ�������KB��ȡֵͬRvol ����*/

    SWORD32  sdwSvolThr;                    /* Svol �����ı�����ֵ���ٷֱȱ�ʾ,����80��ʾRvol�Ŀռ䱻ʹ�õ�80%ʱ����*/
    SWORD32  sdwSvolNotify;                 /*Svol�ռ䲻��ʱ�Ĵ�����ԣ�ȡֵ: FAIL_SVOL(0)�� FAIL_WRITE_TO_VOL(1)*/
    SWORD32  sdwCapacityPercentage;         /* ʵ���������Դ��İٷֱȣ����10% */

}_PACKED_1_ T_SvolCreate;



/* BLK��SCS����ͨ�øýṹ�� */
typedef struct
{
    CHAR    cSvolName[MAX_BLK_NAME_LEN];    /* ���վ����ƣ���� */
}_PACKED_1_ T_SvolName;

typedef struct
{

    CHAR    cSvolName[MAX_BLK_NAME_LEN];       /* ���վ����ƣ���� */
    CHAR    cSvolNewName[MAX_BLK_NAME_LEN];    /* ���վ������� �����*/
}_PACKED_1_ T_SvolRename;

/* BLK��SCS����ͨ�øýṹ�� */
typedef struct
{

    CHAR     cSvolName[MAX_BLK_NAME_LEN];  /*���վ�����,��� */
    CHAR     cVolName[MAX_BLK_NAME_LEN];   /* Vol����*/
    SWORD32  sdwState;                     /* The state of the svol, the possible values are %SVOL_S_ACTIVE, %SVOL_S_FAULTY.*/
    T_Time   tCreateTime;                  /* Svol create time */
    SWORD64  uqwCapacity;                  /* Snapshot volume size (in sectors),���վ�����,KB */
    SWORD64  uqwUsedCapacity;              /* Used Snapshot volume size (in sectors),��ʹ�ÿ��վ����� */
    SWORD32  sdwSvolThr;                   /* ���վ�ı�����ֵ���ٷֱȱ�ʾ*/
    SWORD32  sdwSvolNotify;                /* Overflow treatment strategies of rvol ("fail svol" or "fail write to bvol")�����������Ĵ���ʽ��� */
    SWORD32 sdwLunMapState;                 /* ���վ��ӳ��״̬��ȡֵMAPPED_VOL(1),NO_MAPPED_VOL(0) ������*/

}_PACKED_1_ T_SvolInfo;

/* BLK��SCS����ͨ�øýṹ�� */
typedef struct
{
    CHAR       cVolName[MAX_BLK_NAME_LEN];  /*������ �����*/
    SWORD32    sdwSvolNum;  /*���վ���Ŀ*/
    T_SvolInfo tSvolInfo[MAX_SVOL_NUM_IN_VOL];    /*���վ���Ϣ�б�*/
}_PACKED_1_ T_AllSvolInfoOnVol;

typedef struct
{
    CHAR    cSvolName[MAX_BLK_NAME_LEN];  /*���վ����� �����*/
    SWORD32 sdwSvolThr;     /* Warning threshold of rvol �����*/
    SWORD32 sdwSvolNotify;  /* Overflow treatment strategies of Svol (fail svol or fail write to bvol)�����*/
}_PACKED_1_ T_SvolThr;

#if 0
typedef struct
{
    SWORD32 sdwVdId;              /* ���ָ���Դ�����ڵ�Vd ID */
    SWORD32 sdwVolId;     /* ���ָ���Դ��ID */
    SWORD32 sdwSvolId;    /* ���ָ��Ŀ���ID*/
}_PACKED_1_ T_SvolRestore;
#endif



/***************************************Clone Vol************************************/
/*CloneVol����*/
typedef struct
{
    CHAR    scCvolName[MAX_BLK_NAME_LEN];   /* ��������*/
    CHAR    scVdName[MAX_BLK_NAME_LEN];      /* ����������vd, ������Դ��ͬ*/
    CHAR    scBvolName[MAX_BLK_NAME_LEN];    /* Դ����*/
    SWORD32 sdwCvolCapacity;               /* clone�������KB,Ҫ��С��Դ�� */
    SWORD32 sdwCvolChunkSize;   /* clone���chunk��λKB */
    SWORD32 sdwInitSync;     /* �Ƿ���г�ʼͬ�� , 1-YES, 0--NO*/
    SWORD32 sdwProtectRestore;   /* �Ƿ�������������, 1-YES, 0--NO */
    SWORD32 sdwPri;        /* ͬ����ͬ���������ȼ�,ȡֵ:SYNC_LOW(0),SYNC_MEDIUM(1),SYNC_HIGH(2)*/
}_PACKED_1_ T_CreateCvol;

/* CloneVol��Ϣ*/
typedef struct
{
    CHAR    scCvolName[MAX_BLK_NAME_LEN];  /* ����������� ,���*/
    CHAR    scVdName[MAX_BLK_NAME_LEN];      /* ����������vd*/
    CHAR    scBVolName[MAX_BLK_NAME_LEN];   /* Դ����*/
    SWORD32 sdwCvolCapacity;   /* clone�������KB */
    SWORD32 sdwCvolChunkSize;   /* clone���chunk��λ,KB */
    SWORD32 sdwInitSync;     /* �Ƿ���г�ʼͬ��, 1-YES, 0--NO */
    SWORD32 sdwProtectRestore;   /* �Ƿ�������������, 1-YES, 0--NO */
    SWORD32 sdwPri;   /* ͬ����ͬ�������� */
    SWORD32 sdwRelation;   /* ��������Դ��Ĺ�����ϵ, 1--����, 0--���� */
    SWORD32 CvolState;   /* �������״̬ */
    SWORD32 sdwLunMapState;  /* �������ӳ��״̬��ȡֵMAPPED_VOL(1),NO_MAPPED_VOL(0) ������*/
 }_PACKED_1_ T_CvolInfo;

/* ���ϵĿ�������Ϣ*/
typedef struct
{
    CHAR    scVolName[MAX_BLK_NAME_LEN];        /* ����������� ,���*/
    SWORD32 sdwCvolNum;   /* ���ϵ����п�������Ŀ */
    CHAR    scCvolNames[MAX_CLONE_IN_VOL][MAX_BLK_NAME_LEN];   /* ���ϵ����п��������б� */
}_PACKED_1_ T_CvolNamesOnVol;

/*������ͬ�����߷�ͬ�������ȼ�(����)*/
typedef struct
{
    CHAR    scCvolName[MAX_BLK_NAME_LEN];  /* ����������� �����*/
    SWORD32 sdwPri;   /* ͬ����ͬ�������ȼ���ȡֵ:SYNC_LOW(0),SYNC_MEDIUM(1),SYNC_HIGH(2) */
}_PACKED_1_ T_CvolPriority;


typedef struct
{
    CHAR    scCvolName[MAX_BLK_NAME_LEN];  /* ����������֣���� */
    SWORD32 sdwProtectRestore;   /* �Ƿ�������������, 1-YES, 0--NO */
}_PACKED_1_ T_CvolProtect;

typedef struct
{
    CHAR    scCvolName[MAX_BLK_NAME_LEN];  /* ����������� */
}_PACKED_1_ T_CvolName;


typedef struct
{
    CHAR    scCvolName[MAX_BLK_NAME_LEN];           /* ���������ƣ���� */
    CHAR    scCvolNewName[MAX_BLK_NAME_LEN];    /* ������������ �����*/
}_PACKED_1_ T_CvolRename;


typedef struct
{
    SWORD32 sdwVolObjectType;                   /* ��������� ,ȡֵ: NORM_VOL / SNAPSHOT_VOL / CLONE_VOL*/
    CHAR    scVolObjectName[MAX_BLK_NAME_LEN];  /* ����������*/
}_PACKED_1_ T_VolObjectInfo;


typedef struct
{
    CHAR     scMapGrpName[MAX_MAPGRP_NAME_LEN]; /* ӳ�������ƣ����*/
    SWORD32 sdwVolObjectNum;                                /* ϵͳ��δӳ��ľ������Ŀ*/
    T_VolObjectInfo    tVolObjectList[256];  /* ϵͳ��δӳ��ľ�������Ϣ�б�,  �ݶ�512 ������*/
}_PACKED_1_ T_VolObjectInfoList;






/***************************************Vd**************************************/
/*�ýṹ���õ���slot id��enc id������blk��Ҫ��owner��SCS���䣬����blk����ֱ��ʹ�øýṹ��*/
typedef struct
{
    CHAR     cVdName[MAX_BLK_NAME_LEN];  /*VD ����,����Ϊ��*/
    SWORD32  sdwVdRaidLevel;  /* eRaidlevel, RAID level, defined in struct eRaidlevel ,ȡֵRAID0(0),RAID1(1),RAID3(3), RAID5(5),RAID6(6),
                                                   RAIDTP(7),RAID10(10),RAID30(30),RAID50(50),RAID60(60),NRAID(200),RAID_LEVEL_INVALID(0xffff) */
    SWORD32  sdwPdNum;                     /* ���32 */
    T_PdId   tPdId[MAX_PD_NUM_IN_VD];      /* ������Ϣ�б� */
    SWORD32  sdwAutoRebuild;    /* �Զ��ؽ����� ON�Զ��ؽ���OFF���Զ��ؽ� */
}_PACKED_1_ T_CreateVd;

/* blk��SCS����ͨ�� */
typedef struct
{
    SWORD32  sdwVdNum;   /* ��������Ŀ */
    CHAR     cVdNames[MAX_VD_NUM_IN_SYS][MAX_BLK_NAME_LEN];    /*  ����������name�б� */
}_PACKED_1_ T_AllVdNames;


/* blk��SCS����ͨ�� */
typedef struct
{
    CHAR     cVdName[MAX_BLK_NAME_LEN];    /*VD ����,����Ϊ��, ���*/
    SWORD32  sdwVdRaidLevel;/* eRaidlevel, RAID level, defined in struct eRaidlevel ,ȡֵRAID0(0),RAID1(1),RAID3(3), RAID5(5),RAID6(6),
                                                   RAIDTP(7),RAID10(10),RAID30(30),RAID50(50),RAID60(60),NRAID(200),RAID_LEVEL_INVALID(0xffff) */
    SWORD32  sdwFullPdNum;                  /*vd����ʱ,   ��vd����ʱ,���̸���*/
    SWORD32  sdwPdNum;                      /*��ǰ�����VD ��Ա�̸���*/
    T_PdId   tPdId[MAX_PD_NUM_IN_VD];       /* ������Ϣ�б� */
    SWORD32  sdwAutoRebuild;                /* �Զ��ؽ�����ON(1)/OFF(0) */
    WORD64   sqwTotalCapacity;              /*  ����������,��λKB */
    WORD64   sqwFreeCapacity;               /*  ��������,��λKB */
    WORD64   sqwMaxContiguousFreeSpace;     /*  ����������пռ�,��λKB */
    T_Time   tCreateTime;                   /* Vd ����ʱ�� */
    SWORD32  sdwState;                      /* VD��״̬��Ϣ,Ŀǰ֧�ֵ�״̬��:VD_GOOD(0)��VD_FAULT(1)��VD_DEGRADE(2)  */
    SWORD32  local;                         /* localΪ0��ʾ����VD,localΪ1��ʾ����VD */
}_PACKED_1_ T_VdInfo;


#if 0
typedef struct
{
    SWORD32    sdwCtlId;                   /*������ID*/
    SWORD32    sdwState;                   /* VD״̬��Ϣ*/
    SWORD32    sdwPdNum;                   /*��ǰ�����VD ��Ա�̸���*/
    T_PdId   tPdId[MAX_PD_NUM_IN_VD];       /* ������Ϣ�б� */
}_PACKED_1_ T_VdSingleCtlInfo;

typedef struct
{
    CHAR         cVdName[MAX_BLK_NAME_LEN];    /*VD ����,����Ϊ��, ���*/
    SWORD32      sdwVdRaidLevel;               /* eRaidlevel, RAID level, defined in struct eRaidlevel ,ȡֵRAID0(0),RAID1(1),RAID3(3), RAID5(5),RAID6(6),
                                                   RAIDTP(7),RAID10(10),RAID30(30),RAID50(50),RAID60(60),NRAID(200),RAID_LEVEL_INVALID(0xffff) */
    SWORD32      sdwFullPdNum;                 /*vd����ʱ,   ��vd����ʱ,���̸���*/
    SWORD32      sdwAutoRebuild;                /* �Զ��ؽ�����ON(1)/OFF(0) */
    WORD64       sqwTotalCapacity;              /*  ����������,��λKB */
    WORD64       sqwFreeCapacity;               /*  ��������,��λKB */
    WORD64       sqwMaxContiguousFreeSpace;       /*  ����������пռ�,��λKB */
    T_Time       tCreateTime;                   /* Vd ����ʱ�� */
    SWORD32      local;                           /* localΪ0��ʾ����VD,localΪ1��ʾ����VD */
    T_VdSingleCtlInfo  tVdSingleCtlInfo[2];     /*VD���˿��������Գ���Ϣ,tVdSingleCtlInfo[0]��ʾ���ˣ�tVdSingleCtlInfo[1]��ʾ�Զ�*/
}_PACKED_1_ T_VdInfo;
#endif

typedef struct
{
    CHAR       cVdName[MAX_BLK_NAME_LEN];     /*VD ����,����Ϊ��, ���*/
    CHAR       cMdType[MAX_PD_NUM_IN_VD];     /* ��Ա���б�����---1:local,2:peer,3:dual ������*/
    WORD32     dwMdNum;                       /* ��Ա����Ŀ������ */
    T_PdId     tPdId[MAX_PD_NUM_IN_VD];       /* ������Ϣ�б����� */
}_PACKED_1_ T_MdList;

/* BLK��SCS����ͨ�� */
typedef struct
{
    CHAR     cVdName[MAX_BLK_NAME_LEN];    /*VD ����,����Ϊ��, ���*/
    /* ������ȷ��*/
    SWORD32  sdwTaskType;  /*��������*/
    T_Time   tStartTime;  /* ��ʼʱ��  */
    SWORD32  sdwPercent;   /* ������� */
    T_Time   tFinishTime; /* ����ʱ��  */
    /* BYTE ucDelay;  */    /* reserved */
}_PACKED_1_ T_VdGetTaskInfo;

/* BLK��SCSӦ�ò�����ͨ��, blk��Ҫ�����豸��*/
typedef struct
{
    CHAR      cVdName[MAX_BLK_NAME_LEN];    /*VD ����,����Ϊ��, ���*/
    SWORD32   sdwSpareHdNum;                /* �ȱ�����Ŀ�����*/
    T_PdId    tPdId[MAX_PD_NUM_ADD];          /* ������Ϣ�б� �����*/
}_PACKED_1_ T_VdRebuild;/* �������ؽ����ָ���*/



/* blk��SCS����ͨ�� */
typedef struct
{
    CHAR    cVdName[MAX_BLK_NAME_LEN];  /*VD ����,����Ϊ�գ����*/
}_PACKED_1_ T_VdName;

typedef struct
{
    CHAR     cVdName[MAX_BLK_NAME_LEN];     /*VD ����,����Ϊ�գ����*/
    CHAR     cVdNewName[MAX_BLK_NAME_LEN];  /*���޸ĵ�VD ����,����Ϊ�գ����*/
}_PACKED_1_ T_VdRename;

/* blk��SCS����ͨ�� */
#if 0
typedef struct
{
    SWORD32 sdwVdId; /* ������Idȡֵ��Χ0~MAX_VD_ID(32)-1   */
    SWORD32 sdwPreferCtlId; /* ��ѡ����������ʱΪ0,1 */
}_PACKED_1_ T_VdPreferCtl;
#endif

/* blk��SCS����ͨ�� */
typedef struct
{
    CHAR    cVdName[MAX_BLK_NAME_LEN];  /*VD ����,����Ϊ�գ����*/
    SWORD32 sdwAutoRebuild;  /* �Զ��ؽ����أ�ON(1)/OFF(0) �����*/
}_PACKED_1_ T_VdAutoRebuild;

/* blk��SCS������ͨ�� �ϲ��ǲ�λ�ţ�blk���豸��*/
typedef struct
{
    CHAR       cVdName[MAX_BLK_NAME_LEN];  /*VD ����,����Ϊ�գ����*/
    SWORD32    sdwAddPdNum;                /*�����Ӵ�����Ŀ�������*/
    T_PdId     tPdId[MAX_PD_NUM_ADD];      /* ������Ϣ�б� �������*/
    SWORD32    sdwDefragFlg;               /* ��չ�Ĺ������Ƿ�������Ƭ��ON-��Ƭ����OFF-��������Ƭ��������ѡ��� */
}_PACKED_1_ T_VdExpand;

/* SCS��blk����ͨ�� */
typedef struct
{

    CHAR    cVdName[MAX_BLK_NAME_LEN];  /*VD ����,����Ϊ�գ������*/
    SWORD32 sdwVdRaidLevelNew;         /* �µ�Raid���� �������*/
    SWORD32 sdwDefragFlg;              /* �伶�Ĺ������Ƿ�������Ƭ��ON-��Ƭ����OFF-��������Ƭ���� ����ѡ���*/
}_PACKED_1_ T_VdRelevel;




/* ��̨�������*/
typedef struct
{
    SWORD32 sdwTaskId;           /*����ID*/
    CHAR       cObjectName[MAX_BLK_NAME_LEN];  /*��������������*/
    WORD32   sdwTaskType;  /* ��������,ȡֵ���궨��*/
    SWORD32   sdwTaskState;  /* ����״̬��ȡֵTASK_RUNNING��TASK_PAUSE��TASK_DELAY*/
    T_Time      tStartTime;       /* ��ʼʱ�䣬�ꡢ�¡��ա�ʱ���֡���*/
    SWORD32  sdwTaskProgress;     /* ������ȣ�����������%*/
}_PACKED_1_ T_TaskInfo;


typedef struct
{
    SWORD32 sdwTaskNum;           /*����ID*/
    T_TaskInfo tTaskInfo[MAX_TASK_NUM_IN_SYS];    /*ϵͳ�����к�̨�����б�*/
}_PACKED_1_ T_TaskInfoList;


typedef struct
{
    SWORD32 sdwTaskId;           /*����ID*/
    SWORD32 sdwTaskCtrl;         /*��̨���������ȡֵTASK_CTRL_RUN��TASK_CTRL_PAUSE��TASK_CTRL_DEL*/
}_PACKED_1_ T_TaskCtrl;

typedef struct
{
    CHAR        cObjectName[MAX_OBJECT_NAME_LEN];      /* ��������������,input */
    WORD32      sdwTaskType;                           /* ��������,ȡֵ���궨��,input */
    SWORD32     sdwPrarmLen;                           /* �����������,input */
    CHAR        ucTaskPrarm[MAX_TASK_PRARM_LEN];       /* �����������,input */
}_PACKED_1_ T_TaskAddInfo;

/* �ƻ��������*/

typedef struct
{
    SWORD32 swTaskId;      //�ƻ�����id��
    SWORD32 scTaskType;    //�������ͣ��ο�eSchdTaskType����
    CHAR    cObjectName[MAX_BLK_NAME_LEN];   //�ƻ����������Ķ�������
    T_Time  tStartTime;  //��������ʱ��
    T_Time  tNextRunTime;  //�����´�����ʱ��
    SWORD32 sdwSchedPeriod; //�ƻ�������������ԣ�ȡֵSCHED_ONCE��SCHED_DAYLY��SCHED_WEEKLY��SCHED_MONTHLY
       CHAR    cTaskDescription[128];   // �ƻ������Ҫ����
}_PACKED_1_ T_ScheduleTask;

typedef struct
{
    SWORD32 sdwNum; //�ƻ�������Ŀ
    T_ScheduleTask tScheduleTask[MAX_SCHEDULE_NUM]; //�ƻ�������Ϣ�б�
}_PACKED_1_ T_ScheduleList;

typedef struct
{
    SWORD32 swTaskId; //�ƻ�����id�ţ����
}_PACKED_1_ T_ScheduleDel;


/***************************************STS*************************************/
#if 0

typedef struct
{
    SWORD32  sdwLunGlbId;  /* Lunȫ��Id,0~MAX_LUN_NUM_IN_SYS(1024)-1 */
    SWORD32  sdwTgtType; /* ��or����,�궨��ΪNORM_VOL��SNAP_ENTITY_VOL*/
    SWORD32  sdwTgtId;     /*VdId ��VolId ��SvolId����ϣ�����VdIdΪ��0�ֽڣ�VolIdΪ��1�ֽڣ�SvolIdΪ��2�ֽ�*/
    SWORD32  sdwCachePolicy; /* CACHE д��д������, ȡֵCACHE_WRITE_BACK_POLICY(0),CACHE_WRITE_THROUGH_POLICY(1) */
    SWORD32  sdwAheadReadSize;/* cacheԤ��ϵ�� */
}_PACKED_1_ T_LunSimpleInfo;

typedef struct
{
    SWORD32 sdwNum;
    SWORD32 sdwLunGlbId[MAX_LUN_NUM_IN_SYS]; /* Lunȫ��Id,0~MAX_LUN_NUM_IN_SYS(1024)-1 */
}_PACKED_1_ T_AllLunId;

typedef struct
{
    SWORD32 sdwLunGlbId;     /* Lunȫ��Id,0~MAX_LUN_NUM_IN_SYS(1024)-1 */
    SWORD32 sdwCachePolicy;  /* CACHE д��д������, ȡֵCACHE_WRITE_BACK_POLICY(0),CACHE_WRITE_THROUGH_POLICY(1) */
    SWORD32 sdwAheadReadSize;/* cacheԤ��ϵ�� */
}_PACKED_1_ T_LunCacheInfo;

typedef struct
{
    SWORD32 sdwLunGlbId;    /* Lunȫ��Id,0~MAX_LUN_NUM_IN_SYS(1024)-1 */
    SWORD32 sdwLunLocalId;  /* Lun�ı���Id,int��ֵ��Ψһ */
    SWORD32 sdwMapGrpId;    /* Lun������MapGrpId,ȡֵ0~MAX_MAP_GROUP_NUM_IN_SYS(16)-1*/
    SWORD32 sdwBlockSize;   /* ���ݿ��С��ȡֵ��Χ512,512*2,512*4,512*8 */
    SWORD32 sdwAccessAttr;  /* ֻ�� or ��д */
}_PACKED_1_ T_LunAddToGrp;

typedef struct
{
    SWORD32 sdwLunLocalId;  /* Lun�ı���Id,int��ֵ��Ψһ */
    SWORD32 sdwMapGrpId;    /* Lun������MapGrpId,ȡֵ0~MAX_MAP_GROUP_NUM_IN_SYS(16)-1*/
}_PACKED_1_ T_LunDelFrmGrp;


/* 2010-12-02    xygang  */
typedef struct
{
    CHAR  ucHostName[MAX_INI_NAME_LEN];
    /* CHAP��ȨʱHost�û��������� */
}_PACKED_1_ T_HostInfo;

/* 2010-12-02    xygang  */
typedef struct
{
    SWORD32 sdwMapGrpId;   /* ӳ�����Id,ȡֵ0~MAX_MAP_GROUP_NUM_IN_SYS(16)-1 */
    SWORD32 sdwLunNum;     /* �߼���Ԫ����*/
    SWORD32 sdwLunLocalId[MAX_LUN_NUM_IN_MAP_GROUP];/* Lun�ı���Id,int��ֵ��ӳ������Ψһ */
    SWORD32 sdwLunGlbId[MAX_LUN_NUM_IN_MAP_GROUP];/* Lunȫ��Id,0~MAX_LUN_NUM_IN_SYS(1024)-1 */
    SWORD32 sdwHostNum;    /* �����ĸ��� */
    T_HostInfo  tHostInfo[MAX_INI_NUM_IN_MAP_GROUP];
}_PACKED_1_ T_MapGrpInfo;


typedef struct
{
    CHAR    ucInitName[MAX_INI_NAME_LEN];
    SWORD32 sdwMapGrpId;  /* Host����MapGrpId,ȡֵ0~MAX_MAP_GROUP_NUM_IN_SYS(16)-1  */
}_PACKED_1_ T_HostInGrp;

typedef struct
{
    SWORD32 sdwNum;
    SWORD32 sdwMapGrpId[MAX_MAP_GROUP_NUM_IN_SYS];/* ӳ�����Id,ȡֵ0~MAX_MAP_GROUP_NUM_IN_SYS(16)-1 */
}_PACKED_1_ T_AllGrpId;

typedef struct
{
    SWORD32     sdwMapGrpId;/* ӳ�����Id,ȡֵ0~MAX_MAP_GROUP_NUM_IN_SYS(16)-1 */
    SWORD32     sdwNum;
    T_HostInfo  tHostInfo[MAX_INI_NUM_IN_MAP_GROUP];
}_PACKED_1_ T_AllHostInGrp;

typedef struct
{
    SWORD32 sdwMapGrpId;/* ӳ�����Id,ȡֵ0~MAX_MAP_GROUP_NUM_IN_SYS(16)-1 */
    SWORD32 sdwNum;
    SWORD32 sdwLunLocalId[MAX_LUN_NUM_IN_MAP_GROUP];/* Lun�ı���Id,int��ֵ��Ψһ */
    SWORD32 sdwLunGlbId[MAX_LUN_NUM_IN_MAP_GROUP];  /* Lunȫ��Id,0~MAX_LUN_NUM_IN_SYS(1024)-1 */
}_PACKED_1_ T_AllLunInGrp;

typedef struct
{
    CHAR    ucInitName[MAX_INI_NAME_LEN];
    SWORD32 sdwMapGrpId;/* ӳ�����Id,ȡֵ0~MAX_MAP_GROUP_NUM_IN_SYS(16)-1 */
}_PACKED_1_ T_HostGrpId;

typedef struct
{
    SWORD32 sdwLunGlbId;   /* Lunȫ��Id,0~MAX_LUN_NUM_IN_SYS(1024)-1 */
    SWORD32 sdwNum;
    SWORD32 sdwMapGrpId[MAX_MAP_GROUP_NUM_IN_SYS];/* ӳ�����Id,ȡֵ0~MAX_MAP_GROUP_NUM_IN_SYS(16)-1 */
}_PACKED_1_ T_LunGrpId;

typedef struct
{
    SWORD32 sdwCtrlId;        /* ������Id ,��ʱ����Ϊ0��1����Ӧ�ĺ궨��CTRL0��CTRL1*/
    SWORD32 sdwLunGlbId;  /* Lunȫ��Id,0~MAX_LUN_NUM_IN_SYS(1024)-1 */
    CHAR    ucInitName[MAX_INI_NAME_LEN];    /* ������ */
    SWORD32 sdwAction;      /*������־: 0/1/2����Ӧ�ĺ궨��Ϊ"OFF"�� "ON"��"RES"���ֱ��ʾ�رա��򿪡����á� */
}_PACKED_1_ T_LunIOStaticSwitch;

typedef struct
{
    SWORD32     sdwCtrlId;        /* ������Id ,��ʱ����Ϊ0��1����Ӧ�ĺ궨��CTRL0��CTRL1*/
    SWORD32     sdwLunGlbId;              /* Lun��ȫ��Id,int��ֵ��Ψһ */
    CHAR        ucInitName[MAX_INI_NAME_LEN];         /* ������ */
    WORD64      sqwStatReadCount;           /* ͳ�ƶ��ĸ��� */
    WORD64      sqwStatReadTransimitData;   /* ͳ�ƶ������� */
    WORD64      sqwStatWriteCount;          /* ͳ��д�ĸ��� */
    WORD64      sqwStatWriteTransimitData;  /* ͳ��д������ */
    WORD64      sqwStatTransimitDataAverage;    /* ƽ���������� */
    WORD64      sqwStatTransimitDataAll;        /* �������� */
    WORD64      sdwStatCmdTimeAverage;          /* ����ƽ��ִ��ʱ�� */
    WORD64      sdwStatCmdTimeMax;              /* �������ִ��ʱ�� */
    WORD64      sdwCmdSuccessNum ;              /* ����ִ�гɹ��ĸ��� */
    WORD64      sdwCmdTimeAll;                  /* ����ִ�е���ʱ�� */
}_PACKED_1_ T_LunIOStaticData;

#endif

typedef struct
{
    SWORD32 sdwLunId;  /* Lun�ı���Id, int��ֵ������Ψһ */
    CHAR    cVolName[MAX_BLK_NAME_LEN]; /* Vol/Svol����,����Ϊ��*/
    SWORD32 sdwBlockSize;   /* ���ݿ��С��ȡֵ��Χ512,512*2,512*4,512*8 */
    SWORD32 sdwAccessAttr;  /* ֻ�� or ��д */
}_PACKED_1_ T_LunInfo;

typedef struct
{
    CHAR      cMapGrpName[MAX_MAPGRP_NAME_LEN]; /* ӳ��������,����Ϊ��,���*/
    T_LunInfo tLunInfo;                              /*Lun��ص���Ϣ�����*/
}_PACKED_1_ T_AddVolToGrp;

typedef struct
{
    CHAR     cMapGrpName[MAX_MAPGRP_NAME_LEN]; /* Vol������MapGrp����,���*/
    SWORD32  sdwLunId;       /* Lun�ı���Id��ӳ������Ψһ ,���*/
}_PACKED_1_ T_DelVolFrmGrp;


/* 2010-12-02    xygang  */
typedef struct
{
    CHAR  ucHostName[MAX_INI_NAME_LEN];
    /* CHAP��ȨʱHost�û��������� */
}_PACKED_1_ T_HostInfo;

/* 2010-12-02    xygang  */
typedef struct
{

    CHAR        cMapGrpName[MAX_MAPGRP_NAME_LEN]; /* ӳ��������,����Ϊ��,���*/
    SWORD32     sdwLunNum;     /* �߼���Ԫ����*/
    T_LunInfo   tLunInfo[MAX_LUN_NUM_IN_MAP_GROUP];     /*Lun��ص���Ϣ*/
    SWORD32     sdwHostNum;    /* �����ĸ��� */
    T_HostInfo  tHostInfo[MAX_INI_NUM_IN_MAP_GROUP];
}_PACKED_1_ T_MapGrpInfo;


typedef struct
{
    CHAR    ucInitName[MAX_INI_NAME_LEN];     /* ������ */
    CHAR    cMapGrpName[MAX_MAPGRP_NAME_LEN]; /* ӳ��������*/
}_PACKED_1_ T_HostInGrp;



typedef struct
{
    SWORD32  sdwMapGrpNum;    /* ϵͳ��ӳ������Ŀ */
    CHAR     cMapGrpNames[MAX_MAP_GROUP_NUM_IN_SYS][MAX_MAPGRP_NAME_LEN]; /* ����ӳ���������б�*/
}_PACKED_1_ T_AllGrpNames;

typedef struct
{
    CHAR      cMapGrpName[MAX_MAPGRP_NAME_LEN]; /* ӳ�������ƣ����*/
}_PACKED_1_ T_MapGrpName;

typedef struct
{
    CHAR     cMapGrpName[MAX_MAPGRP_NAME_LEN]; /* ӳ�������ƣ����*/
    CHAR     cMapGrpNewName[MAX_MAPGRP_NAME_LEN]; /* ӳ���������ƣ����*/
}_PACKED_1_ T_MapGrpRename;


typedef struct
{
    CHAR      cVolName[MAX_BLK_NAME_LEN]; /* Vol/Svol����,����Ϊ��,���*/
    SWORD32   sdwMapGrpNum;
    CHAR      cMapGrpNames[MAX_MAP_GROUP_NUM_IN_SYS][MAX_MAPGRP_NAME_LEN]; /* ӳ��������*/
}_PACKED_1_ T_GrpNameByVol;

typedef struct
{
    //SWORD32 sdwCtrlId;        /* ������Id ,��ʱ����Ϊ0��1����Ӧ�ĺ궨��CTRL0��CTRL1*/
    CHAR     cMapGrpName[MAX_MAPGRP_NAME_LEN]; /* ӳ�������ƣ����*/
    SWORD32  sdwLunId;       /* Lun�ı���Id��ӳ������Ψһ ,���*/
    CHAR     cInitName[MAX_INI_NAME_LEN];    /* ������ �����*/
    SWORD32  sdwAction;      /*������־: 0/1/2����Ӧ�ĺ궨��Ϊ"OFF"�� "ON"��"RES"���ֱ��ʾ�رա��򿪡����á� ���*/
}_PACKED_1_ T_LunIOStaticSwitch;

typedef struct
{
    //SWORD32 sdwCtrlId;        /* ������Id ,��ʱ����Ϊ0��1����Ӧ�ĺ궨��CTRL0��CTRL1*/
    CHAR        cMapGrpName[MAX_MAPGRP_NAME_LEN]; /* ӳ�������ƣ����*/
    SWORD32     sdwLunId;       /* Lun�ı���Id��ӳ������Ψһ ,���*/
    CHAR        cInitName[MAX_INI_NAME_LEN];    /* ������ �����*/
    WORD64      sqwStatReadCount;           /* ͳ�ƶ��ĸ��� */
    WORD64      sqwStatReadTransimitData;   /* ͳ�ƶ������� ,��λ:Byte*/
    WORD64      sqwStatWriteCount;          /* ͳ��д�ĸ��� */
    WORD64      sqwStatWriteTransimitData;  /* ͳ��д������ ,��λ:Byte*/
    WORD64      sqwStatTransimitDataAverage;    /* ƽ���������� */
    WORD64      sqwStatTransimitDataAll;        /* �������� */
    WORD64      sdwStatCmdTimeAverage;          /* ����ƽ��ִ��ʱ��,��λ:΢�� */
    WORD64      sdwStatCmdTimeMax;              /* �������ִ��ʱ�� ,��λ:΢��*/
    WORD64      sdwCmdSuccessNum ;              /* ����ִ�гɹ��ĸ��� */
    WORD64      sdwCmdTimeAll;                  /* ����ִ�е���ʱ��,��λ:΢�� */
}_PACKED_1_ T_LunIOStaticData;
/***************************************CBS*************************************/
typedef struct
{
    SWORD32 sdwRwRatio;      /* ��д��ռ�ռ�����У�дռ�ı��� */
    SWORD32 sdwUnitSize;     /* ���浥Ԫ��С ����λ:KB*/
    SWORD32 sdwWrPeriod;     /* ��ʱ��д���� */
    SWORD32 sdwRefreshRatio; /* ��д�������� */
    SWORD32 sdwWrEnable;     /* ���񿪹�ON/OFF */
}_PACKED_1_ T_SetCacheGlbPolicy;

typedef struct
{
    SWORD32  sdwRwRatio;       /* ��дCache���� */
    SWORD32  sdwUnitSizeSet;   /* �������õ�Cache��Ԫ��С����λKB��ֵΪ4��8��16��32��64��128��(��������Ч) */
    SWORD32  sdwCurUnitSize;   /* ��ǰ��ЧCache��Ԫ��С��ֵͬ�ϣ� */
    SWORD32  sdwWrPeriod;      /* ˢ������ */
    SWORD32  sdwRefreshRatio;  /* ˢ�±��� */
    SWORD32  sdwWrEnable;      /* Cache���񿪹� ֵΪON��1��/OFF��1�� */
}_PACKED_1_ T_GetCacheGlbPolicy;

typedef struct
{
    SWORD32 sdwTotalBuffs;/*  �ܵĻ������ */
    SWORD32 sdwFreeBuffs;      /*  ���еĻ������ */
}_PACKED_1_ T_CacheBuffInfo;

typedef struct
{
    //SWORD32 sdwCtrlId;        /* ������Id ,��ʱ����Ϊ0��1����Ӧ�ĺ궨��CTRL0��CTRL1*/
    SWORD32 sdwAction;      /*������־: 0/1/2����Ӧ�ĺ궨��Ϊ"OFF"�� "ON"��"RES"���ֱ��ʾ�رա��򿪡����á� */
}_PACKED_1_ T_CacheIOStatSwitch;

typedef struct
{

    WORD64 uqwBcReadCmds;      /* ����������ܸ��� */
    WORD64 uqwBcReadSize;      /* ����������������(��λ��sector����)  */
    WORD64 uqwBcWriteCmds;     /* ����д�����ܸ��� (��λ��sector����) */
    WORD64 uqwBcWriteSize;     /* д�������������� */
    WORD64 uqwBcReadHits;      /* ���������и��� */
    WORD64 uqwBcWriteMerge;    /* �ϲ�д����(Ŀǰ�������δʹ��) */
    WORD64 uqwBcDirtyBuffers;  /* �໺����� */
    WORD64 uqwBcCleanBuffers;  /* �ɾ�������� */
}_PACKED_1_ T_CacheIOStatData;


/***************************************ISCSI***********************************/


typedef struct
{
    CHAR     scIp[MAX_IP_LEN];   /*IP��ַ*/
    CHAR     scName[MAX_INI_NAME_LEN];
}_PACKED_1_ T_IscsiIniInfo;
typedef struct
{
    WORD32          wTid;/* target Id��Ŀ��˿�Id */
    WORD32          wCount;
    T_IscsiIniInfo  tIscsiIInfo[MAX_INITIATOR_NUM];
}_PACKED_1_ T_IscsiIniInfoList;

typedef struct
{
    WORD64     uqwSessId;                  /* Session Id */
    WORD32     udwPortId;                  /* port  Id */
    CHAR       cPortIp[MAX_IP_LEN];        /* �˿ڵ�����IP */
    CHAR       cInitIp[MAX_IP_LEN];          /* Initiator IP */
    CHAR       cInitName[MAX_INI_NAME_LEN];  /* Initiator Name */
}_PACKED_1_ T_IscsiSessInfo;

/*Target�Ự��Ϣ�б�*/
typedef struct
{
    WORD32      udwTgtId;                            /*Target Id*/
    WORD32      udwSessCount;                      /*Session ����*/
    T_IscsiSessInfo tIscsiSessInfo[MAX_ISCSI_SESSION_COUNT];        /*Session ��Ϣ�б�*/
}_PACKED_1_ T_IscsiSessInfoList;

/*��ȡsession ����*/
typedef struct
{
    WORD64      uqwSessId;                /* Session Id, ���*/
    WORD32      udwTgtId;               /*Target Id */
    WORD32      udwPortId;              /*port  Id */
    WORD32      udwSessionParam[session_key_last];    /*session ����*/
}_PACKED_1_ T_GetIscsiSessAttrInfo;

typedef struct
{
    WORD64      uqwSessId;                /* Session Id, ���*/
    WORD32      udwTgtId;               /*Target Id */
    WORD32      udwPortId;              /* port  Id */
    WORD32      udwParamType;      /*�������ͱ�ʶ����ʾ��Session���Ի��ǻ���Target���ԣ��ô�����ʾSession���� */
    WORD32      udwPartial;             /*����Session���Ա�ʶ��Ϊ0��ʾ�������еĲ������ԣ�����0ʱ��ʾ���ò��ֲ������ԣ�ÿ��bit�ֱ��Ӧһ����������*/
    WORD32      udwSessionParam[session_key_last];
    WORD32      udwTargetParam[target_key_last];
}_PACKED_1_ T_IscsiParamInfo;


/*�˿�������Ϣ*/
typedef struct
{
    SWORD32      sdwCtrlId;               /* ������Id ,��ʱ����Ϊ0��1����Ӧ�ĺ궨��CTRL0��CTRL1*/
    WORD32       udwTgtId;                /* Target ID*/
    WORD32       udwPortId;              /*Target  port ID , ֵΪ0��ʾ���ж˿ڣ�����ֵΪ�˿�ID */
//    WORD32       udwBandWidth;      /*����������*/
    WORD64       uqwTxPDUCount;    /*��������(�ɹ�)PDU����*/
    WORD64       uqwRxPDUCount;    /*��������(�ɹ�)PDU����*/
    WORD64       uqwTxPktCount;     /*���Ͳ�����(ʧ��)PDU����*/
    WORD64       uqwRxPktCount;     /*���ղ�����(ʧ��)PDU����*/
    WORD64       uqwTxByteCount;   /*�������ֽ���*/
    WORD64       uqwRxByteCount;   /*�������ֽ���*/
    WORD64       uqwBandWidth;      /*����������,��λ:Byte/s */
}_PACKED_1_ T_IscsiFlowStatInfo;

/*Target Name*/
typedef struct
{
    SWORD32       udwCtrlId;  /* CTRL ID*/
    CHAR         cTgtName[MAX_TGT_NAME_LEN];   /*Target Name*/
}_PACKED_1_ T_IscsiTargetInfo;


/*Iscsi ����ͳ�ƿ���*/
typedef struct
{
    SWORD32     sdwCtrlId;    /* ������Id ,��ʱ����Ϊ0��1����Ӧ�ĺ궨��CTRL0��CTRL1*/
    WORD32      udwTid;       /* Target ID*/
    WORD32      udwAction;    /* ������־: 0/1/2����Ӧ�ĺ궨��Ϊ"OFF"�� "ON"��"RES"���ֱ��ʾ�رա��򿪡����á� */
}_PACKED_1_ T_IscsiFlowStatFlag;


/* Target ��Ӧ�Ķ˿���Ϣ�б� */
typedef struct
{
    WORD32       udwPortId;                     /* Port Id */
    CHAR         cPortIp[MAX_IP_LEN];  /* �˿ڵ�����IP */
    WORD16       sServPort;                     /* ����˿� */
}_PACKED_1_ T_IscsiPortalInfo;
typedef struct
{
   WORD32            udwTgtId;                                                                               /*Target Id*/
   WORD32            udwPortCount;                                                                        /* �˿����� */
   T_IscsiPortalInfo tISCSIPortalInfo[MAX_ISCSI_PORT_NUM_PER_CTL];             /* �˿���Ϣ�б�*/
}_PACKED_1_ T_IscsiPortalList;



#if 0
typedef struct
{
    WORD32 sdwPortId;  /* ���ں� */
    BYTE   ucPortIp[MAX_IP_LEN];/* ����iSCSI����IP��ַ */
}_PACKED_1_ T_iSCSIPortInfo;

typedef struct
{
    WORD32 sdwTgtId;/* tareget ID */
    BYTE   ucTgtName;
    WORD32 sdwPortNum;
    T_iSCSIPortInfo tPortInfo[MAX_PORT_NUM];/* iSCSIĿ������Ϣ */
}_PACKED_1_ T_iSCSITgtInfo;

typedef struct
{
    WORD64 sqwSid;/* iSCSI�Ựid */
    BYTE   ucTgtIp[MAX_IP_LEN];/* ����iSCSI����IP��ַ */
}_PACKED_1_ T_iSCSIPerTgtInfo;

typedef struct
{
    WORD32 sdwTgtId;/* target ID */
    T_iSCSIPerTgtInfo iSCSIPerTgtInfo[MAX_PORT_NUM];/* iSCSIĿ������Ϣ */
}_PACKED_1_ T_iSCSITgtInfoList;

typedef struct
{
    BYTE InitIp[MAX_IP_LEN];/* iSCSI��������IP��ַ */
    BYTE InitName[MAX_INI_NAME_LEN];/* iSCSI���������� */
}_PACKED_1_ T_iSCSIPerInitInfo;

typedef struct
{
    WORD32 Tid;/* target ID */
    T_iSCSIPerInitInfo iSCSIPerInitInfo[INICOUNT];/* iSCSI��������Ϣ */
}_PACKED_1_ T_iSCSIInitInfoList;

typedef struct
{
    WORD32 key_initial_r2t,
    WORD32 key_immediate_data,
    WORD32  key_max_connections,
    WORD32  key_max_recv_data_length,
    key_max_xmit_data_length,
    key_max_burst_length,
    key_first_burst_length,
    key_default_wait_time,
    key_default_retain_time,
    key_max_outstanding_r2t,
    key_data_pdu_inorder,
    key_data_sequence_inorder,
    key_error_recovery_level,
    key_header_digest,
    key_data_digest,
    key_ofmarker,
    key_ifmarker,
    key_ofmarkint,
    key_ifmarkint,
    session_key_last,
}_PACKED_1_ T_SessionParam;

typedef struct
{
    WORD32 Tid;/* target ID */
    T_SessionParam SessionParam[session_key_last];/* iSCSI�Ự���� */
}_PACKED_1_ T_iSCSIParamInfo;

typedef struct
{
    BYTE ucName[CHAP_ACCOUNT_NAME_MAX];/* �û��� */
    BYTE PssWd[CHAP_ACCOUNT_PASS_MAX];/* ���� */
}_PACKED_1_ T_iSCSIChapAcct;

typedef struct
{
    WORD32 Tid;/* target ID */
    WORD32 PortId;/* ���ں� */
    WORD64 TxFrames;/* ���͵�iSCSI PDU������ */
    WORD64 RxFrames;/* ���յ�iSCSI PDU������ */
    WORD64 TxWords; /* �����ֽ��� */
    WORD64 RxWords;/* �����ֽ��� */
}_PACKED_1_ T_iSCSIFlowInfo;

typedef struct
{
    WORD32 Tid;/* target ID */
    WORD64 TxFrames;/* ���͵�iSCSI PDU������ */
    WORD64 RxFrames;/* ���յ�iSCSI PDU������ */
    WORD64 TxWords; /* �����ֽ��� */
    WORD64 RxWords;/* �����ֽ��� */
}_PACKED_1_ T_iSCSIFlowInfo;

typedef struct
{
    BYTE ucIp[MAX_IP_LEN];
    BYTE ucName[MAX_INI_NAME_LEN];
}_PACKED_1_ T_IscsiIniInfo;

typedef struct
{
    /* Tid; */
    SWORD32 sdwIniCount;
    T_IscsiIniInfo tIscsiIniInfo[MAX_INI_COUNT];
}_PACKED_1_ T_IscsiCtlIniInfoList;

typedef struct
{
    U_DWORD sdwCtlCount;
    T_IscsiCtlIniInfoList tIscsiCtlIniInfoList[MAX_CTL_NUM];
}_PACKED_1_ T_IscsiIniList;

#endif
/***************************************other***********************************/
typedef struct
{
    SWORD32  sdwEnclosureId;     /* enclosure identification */
    SWORD32  sdwCtlId;           /* ctl id or expander id or ENC_ALL_CTL */
    SWORD32  sdwType;            /* USP_VER_TYPE/EXP_VER_TYPE/VPD_VER_TYPE/POW_VER_TYPE/BOOT_VER_TYPE ���ֶ�Ŀǰֻ�������EncId=0��Ч */
    SWORD32  sdwRebootMode;      /* 2--OFFLINE REBOOT, 1--ONLINE REBOOT, ���� */
    CHAR     ucFileName[MAX_FW_FILE_NAME_LEN];
}_PACKED_1_ T_UpdateFirmware;

typedef struct
{
    SWORD32  sdwEnclosureId; /* enclosure identification,��� */
    SWORD32  sdwCtlId;       /* ctl id or expander id��  ��� */
    SWORD32  sdwType;        /* USP_VER_TYPE/EXP_VER_TYPE/VPD_VER_TYPE/POW_VER_TYPE/BOOT_VER_TYPE ���ֶ�Ŀǰֻ�������EncId=0��Ч */
    CHAR     ucCurrentFileName[MAX_FW_FILE_NAME_LEN]; /* ��ǰ�汾���� */
    CHAR     ucCurrentVersion[MAX_VERSION_LEN];       /* ��ǰ�汾�� */
    CHAR     ucStandbyFileName[MAX_FW_FILE_NAME_LEN]; /* ���ð汾���� */
    CHAR     ucStandbyVersion[MAX_VERSION_LEN];       /* ���ð汾�� */
    CHAR     ucUpdateFileName[MAX_FW_FILE_NAME_LEN];  /* �����汾���� */
    CHAR     ucUpdateVersion[MAX_VERSION_LEN];        /* �����汾�� */
}_PACKED_1_ T_FirmwareVersion;

typedef struct
{
    SWORD32  sdwEnclosureId;   /* ��չ������ID,  ��Ĭ��Ϊ0 */
    SWORD32  sdwCtlId;              /* ctl id or expander id or ENC_ALL_CTL */
    SWORD32  sdwAction;           /* ����(CTL_ONLINE_REBOOT)/����(CTL_SINGLE_REBOOT)���������CTL_SINGLE_REBOOT/CTL_ONLINE_REBOOT */
}_PACKED_1_ T_CtrlAction;

typedef struct
{
    SWORD32  sdwNum;
    SWORD32  sdwEnclosureId[MAX_ENC_NUM_IN_SYS]; /* ����Id��ȡֵ 0~ʵ��������*/
}_PACKED_1_ T_AllEncId;

typedef struct
{
    /* ��ȷ��; */
    SWORD32 sdwCtlId;  /* ��ʱʹ�ã�Ϊ�����Windows�µı���  */
}_PACKED_1_ T_CtlHwInfo;

typedef struct
{
    SWORD32 sdwFileType;                          /* eConfigFileType, �����ļ����� */
    CHAR    ucFileName[MAX_CONFIG_FILE_NAME_LEN]; /* ���ļ�����������·�� */
}_PACKED_1_ T_ConfigFile;

/* *************************System config**************************** */

/* ������Ϣ */
typedef struct
{
    WORD32 udwCtrlId;          /* ������ID */
    WORD32 udwRoleType;     /* �����豸��ɫ(��������)���ͣ���ҵ������(NET_ROLE_BUSINESS)���������� (NET_ROLE_MANAGEMENT)*/
    WORD32 udwDeviceId;         /* �豸��� */
    CHAR   cIpAddr[MAX_IP_LEN]; /* IP��ַ */
    CHAR   cNetmask[MAX_IP_LEN]; /* �������� */
    CHAR   cGateway[MAX_IP_LEN]; /* ���� */
}_PACKED_1_ T_SystemNetCfg;

typedef struct
{
    SWORD32  sdwDeviceNum;                                                     /* �����豸���� */
    T_SystemNetCfg tSystemNetCfg[DUAL_MAX_NET_DEVICE_NUM];         /* �����豸��Ϣ�б�(˫��)*/
}_PACKED_1_ T_SystemNetCfgList;


/* �˿�MAC */
typedef struct{
    WORD32 udwCtrlId;                         /* ������ID */
    WORD32 dwPortNo;                          /*�˿ںţ���SPR10�У�������Ϊ1~4*/
    BYTE   ucMacData[6];                       /*MAC ��ַ*/
}_PACKED_1_  T_IscsiPortMac;

typedef struct
{
    SWORD32  sdwIscsiPortNum;                               /* Iscsi  �˿����� */
    T_IscsiPortMac tIscsiPortMac[SPR10_DUAL_ISCSI_PORT_NUM];         /* Iscsi  �˿�Mac ��Ϣ�б�*/
}_PACKED_1_ T_IscsiPortMacList;


/* �˿���Ϣ  */
typedef struct
{
    WORD32 udwCtrlId;                        /* ������ID �����*/
    WORD32 dwPortNo;                         /* �˿ںţ���SPR10�У�������Ϊ1~4�����*/
    SWORD32 sdwSetFlg;                       /* ���ñ�־����ȡֵ0��1��2���ֱ��ʾ����MAC������MTU������MAC��MTU ,(�����AMS ��ֵ)*/
    WORD32 udwPortMtu;                       /* �˿�MTU ����ѡ���*/
    BYTE   ucMacData[6];                     /* MAC ��ַ����ѡ���*/
}_PACKED_1_  T_IscsiPortSet;


typedef struct
{
    WORD32 udwCtrlId;                         /* ������ID */
    WORD32 dwPortNo;                          /*�˿ںţ���SPR10�У�������Ϊ1~4*/
    WORD32 udwPortState;                    /*�˿�״̬��ȡֵ0(down)/1(up)*/
    WORD32 udwPortMtu;                       /*�˿�MTU */
    BYTE   ucMacData[6];                       /*MAC ��ַ*/
}_PACKED_1_  T_IscsiPortInfo;

typedef struct
{
    SWORD32  sdwIscsiPortNum;                               /* Iscsi  �˿����� */
    T_IscsiPortInfo tIscsiPortMac[SPR10_DUAL_ISCSI_PORT_NUM];         /* Iscsi  �˿�Mac ��Ϣ�б�*/
}_PACKED_1_ T_IscsiPortInfoList;

/* �˿ھ�̬·����Ϣ */
typedef struct
{
    WORD32 dwPortId;                          /* �˿ںţ���SPR10�У�������Ϊ1~4*/
    CHAR   scDstIp[MAX_IP_LEN];               /* Ŀ��IP��ַ */
    CHAR   scMask[MAX_IP_LEN];                /* ���� */
    CHAR   scNextHop[MAX_IP_LEN];             /* ��һ����ַ */
}_PACKED_1_  T_IscsiPortRouteInfo;

typedef struct
{
    WORD32 udwCtrlId;                         /* ������ID */
    T_IscsiPortRouteInfo tIscsiPortRouteInfo; /* ·����Ϣ*/
}_PACKED_1_  T_IscsiPortRoute;

typedef struct
{
    WORD32 udwCtrlId;                         /* ������ID */
    CHAR   scDstIp[MAX_IP_LEN];               /* IP��ַ */
    CHAR   scMask[MAX_IP_LEN];                /* ���� */
}_PACKED_1_  T_IscsiPortRouteDel;


typedef struct
{
    WORD32 udwCtrlId;                         /* ������ID */
    SWORD32 sdwRouteNum;                      /* ��̬·����Ŀ */
    T_IscsiPortRouteInfo tIscsiPortRouteInfo[SCSI_MAX_STATICROUTE_NUM];    /* ��̬·���б�*/
}_PACKED_1_  T_IscsiPortRouteList;

typedef struct
{
    CHAR   scDstIp[MAX_IP_LEN];               /* Ŀ��IP��ַ ,���*/ 
    WORD32 udwBytes;                          /* ���ݳ��� ��λ���ֽ�,����*/
    WORD32 udwTotTime;                        /* ����ʱ�� ��λ��us,����*/
    WORD32 udwTtl;                            /* ttl ,����*/                  
}_PACKED_1_  T_IscsiPingInfo;

/* IQN */
typedef struct
{
    SWORD32 sdwCtrlId;
    CHAR    acNewIqnname[MAX_TGT_NAME_LEN]; //iqn����
}T_SetIscsiName;

/* �汾Ŀ¼��ʣ��ռ��С, ���� */
typedef struct
{
    WORD32  dwFreeSize;
}_PACKED_1_ T_VerDirFreeSize;

typedef struct
{
    CHAR    cFileName[FILE_NAME_LEN];        /* ���ϴ��ļ����ļ���, ���*/
    WORD32  dwLocalFreeSize;                /* ����ʣ��ռ��С */
    WORD32  dwPeerFreeSize;                 /* �Զ�ʣ��ռ��С */
}_PACKED_1_ T_VerFreeSpace;

typedef struct
{
    CHAR     cFileName[FILE_NAME_LEN];        /* ���ϴ��ļ����ļ���, ���*/
    BYTE     ucFileType;                      /* USP_VER_TYPE/EXP_VER_TYPE/VPD_VER_TYPE/POW_VER_TYPE/BOOT_VER_TYPE ���ֶ�Ŀǰֻ�������EncId=0��Ч */
    SWORD32  sdwIsValid;                      /* У������0---У��ͨ��������--У�鲻ͨ�������ش�����, ���� */
}_PACKED_1_ T_VerFileCheck;

typedef struct
{
    CHAR  cFileName[FILE_NAME_LEN];        /* ���ϴ��ļ����ļ���, ���*/
    BYTE  ucUploadType;                    /* VERSION_FILE_TPYE--�汾�ļ� CONFIG_FILE_TPYE--�����ļ� OTHER_FILE_TYPE--���� */
}_PACKED_1_ T_UploadPreInfo;

/* ftp�ϴ��ļ��漰�Ľṹ�� */
typedef struct
{
    CHAR  cUserName[USR_NAME_LEN];         /* ftp��������¼�û��� �����*/
    CHAR  cPassWord[PASSWD_LEN];           /* ftp��������¼���� �����*/
    CHAR  cFtpServerIPAddr[MAX_IP_LEN];    /* ftp������IP ��ַ�����*/
    CHAR  cFilePath[FILE_PATH_LEN];        /* ���ϴ��ļ����ļ��� (�������FTP������Ŀ¼�ľ���·��)�����*/
    BYTE  ucUploadType;                    /* VERSION_FILE_TPYE--�汾�ļ� CONFIG_FILE_TPYE--�����ļ� LOG_FILE_TPYE--��־�ļ� OTHER_FILE_TYPE--���� */
}_PACKED_1_ T_UploadFileInfo;

typedef struct
{
    SWORD32 sdwFileType;                 /* VERSION_FILE_TPYE--�汾�ļ� CONFIG_FILE_TPYE--�����ļ� LOG_FILE_TPYE--��־�ļ� OTHER_FILE_TYPE �����ļ�*/
    CHAR    cFileName[FILE_NAME_LEN];
}_PACKED_1_ T_FileToPeer;

/* Ŀǰftp��������AMS�ˣ�SCS����ftp�ͻ��ˡ� �ϴ����������SCS��˵ */
/* ftp�����ļ��漰�Ľṹ�� */
typedef struct
{
    CHAR  cUserName[USR_NAME_LEN];         /* ftp��������¼�û��� �����*/
    CHAR  cPassWord[PASSWD_LEN];           /* ftp��������¼���� �����*/
    CHAR  cFtpServerIPAddr[MAX_IP_LEN];    /* ftp������IP ��ַ�����*/
    CHAR  cFilePath[FILE_PATH_LEN];        /* �ļ���ŵ�Ŀ��·���������ļ����������Զ���srcFileName�ļ������棬��� */
    CHAR  cSrcFileName[FILE_NAME_LEN];     /* Ҫ���ص��ļ��ļ���������·��������� */
    BYTE  ucDownloadType;                  /* VERSION_FILE_TPYE--�汾�ļ� CONFIG_FILE_TPYE--�����ļ� LOG_FILE_TPYE--��־�ļ� OTHER_FILE_TYPE--���� */
}_PACKED_1_ T_DownloadFileInfo;

typedef struct
{
    WORD32  udwPeerProcess;                 /* �Զ˽��ȣ����� */
    WORD32  udwLocalProcess;                /* ���˽��ȣ����� */
    WORD32  udwFileSize;                    /* �ļ���С������ */
    CHAR    cFileName[FILE_PATH_LEN];       /* �ļ���(��·��) �����*/
}_PACKED_1_ T_UploadFileProgress;

typedef struct
{
    WORD32  udwDownloadProcess;             /* ���ؽ��� */
    CHAR    cFileName[FILE_NAME_LEN];       /* �ļ���(����·��) */
}_PACKED_1_ T_DownloadFileProgress;

typedef struct
{
    CHAR       cFileName[FILE_NAME_LEN];       /* �ļ���(����·��) �����*/
}_PACKED_1_ T_UploadFileName;

typedef struct
{
    CHAR       cFileName[FILE_NAME_LEN];       /* �ļ���(����·��) �����*/
    BYTE       ucExistType;                    /* 1:���˶�����,2:˫���½����˴���,3:˫���½��Զ˴���,
                                                * 4:˫�������˶�������,5:�����´���,6:�����²�����,
                                                * 7:��˫��������ļ����뵱ǰ���û��ð汾����---���� */
}_PACKED_1_ T_IsFileExist;

/* *************************DIAG**************************** */
/*���������Ϣ*/

typedef struct
{
    SWORD32  sdwDiagDeviceId;                                /* ���оƬID */
    CHAR     cDiagDeviceName[MAX_DIAG_DEVICE_NAME_LEN];      /* ��϶������� */
    SWORD32  sdwDiagPreStatus;                               /* �����һ�ν��������ΪDEVICE_STATUS_OK���쳣ΪDEVICE_STATUS_FALSE */
    SWORD32  sdwDiagCurStatus;                               /* ��ϱ��ν���� ����ΪDEVICE_STATUS_OK���쳣ΪDEVICE_STATUS_FALSE */
}_PACKED_1_ T_DiagOneDeviceInfo;

typedef struct
{
    SWORD32         sdwCtrlId;                                       /* ������ID,��ʱ����Ϊ0��1����Ӧ�ĺ궨��CTRL0��CTRL1������*/
    SWORD32         sdwDeviceNum;                                    /* ���������ʵ�ʷ��ص��豸��Ŀ ,ȡֵ��ΧΪ0~MAX_DIAG_DEVICE_NUM������*/
    T_DiagOneDeviceInfo tDiagDeviceInfo[MAX_DIAG_DEVICE_NUM];        /* ���оƬ������ ��������ǰsdwDeviceNum��ֵ��Ч,   ����*/
}_PACKED_1_ T_DiagSingleDeviceInfo;

typedef struct
{
    SWORD32 sdwCtrlFlg;                              /* ��������־ ,��ʱ����Ϊ0��1��2����Ӧ�ĺ궨��CTRL_LOCAL��CTRL_PEER��CTRL_DUAL�� ���*/
    SWORD32 sdwBoardNum;                             /* ��ϵ������Ŀ������ */
    T_DiagSingleDeviceInfo tDiagSingleDeviceInfo[2]; /* �����Ϣ�б��� sdwCtrlFlg = 0/1 ʱ,��tDiagSingleDeviceInfoList[0]��ֵ���� sdwCtrlFlg = 2 ʱ ,tDiagSingleDeviceInfoList����ֵ������*/
}_PACKED_1_ T_DiagDeviceInfo;

/*BBU�����Ϣ*/
typedef struct
{
    SWORD32 sdwCtrlId;                              /* ������ID,��ʱ����Ϊ0��1����Ӧ�ĺ궨��CTRL0��CTRL1*/
    SWORD32 sdwBbuPresentInfo;                      /* BBU�Ƿ���λ ����λΪDIAG_BBU_PRESENT������λΪDIAG_BBU_NOPRESENT */
    SWORD32 sdwBbuwMv;                              /* BBU��ǰ��ѹ����λ��mV  */
    SWORD32 sdwBbuwCapacity;                        /* BBUʣ������ٷֱ� */
    SWORD32 sdwBbuDeviceStatus;                     /* BBU״̬,����ΪDEVICE_STATUS_OK���쳣ΪDEVICE_STATUS_FALSE*/
}_PACKED_1_ T_DiagSingleBatteryInfo;
typedef struct
{
    SWORD32 sdwCtrlFlg;                                    /* ��������־ ,��ʱ����Ϊ0��1��2����Ӧ�ĺ궨��CTRL_LOCAL��CTRL_PEER��CTRL_DUAL,   ���*/
    SWORD32 sdwBbuNum;                                     /* ���BBU��Դ����Ŀ������ */
    T_DiagSingleBatteryInfo tDiagSingleBatteryInfo[2];     /* �����Ϣ�б��� sdwCtrlFlg = 0/1 ʱ,��tDiagSingleBatteryInfo[0]��ֵ,�� sdwCtrlFlg = 2 ʱ ,tDiagSingleBatteryInfo����ֵ*/
}_PACKED_1_ T_DiagBatteryInfo;


/* EPLD�����*/
typedef struct
{
    SWORD32 sdwCtrlId;                                     /* ������ID,��ʱ����Ϊ0��1����Ӧ�ĺ궨��CTRL0��CTRL1*/
    SWORD32 sdwEpldFirmVer;                                /* EPLD�̼��汾��*/
    CHAR    cEpldLocation[MAX_DIAG_DEVICE_NAME_LEN];       /* EPLD��λ��       */
    SWORD32 sdwEpldDeviceStatus;                           /* EPLD�豸״̬,����ΪDEVICE_STATUS_OK���쳣ΪDEVICE_STATUS_FALSE*/
}_PACKED_1_ T_DiagSingleEpldInfo;

typedef struct
{
    SWORD32 sdwCtrlFlg;                                    /* ��������־ ,��ʱ����Ϊ0��1��2����Ӧ�ĺ궨��CTRL_LOCAL��CTRL_PEER��CTRL_DUAL,   ���*/
    SWORD32 sdwEpldNum;                                    /* ���Epld����Ŀ������ */
    T_DiagSingleEpldInfo tDiagSingleEpldInfo[2];           /* �����Ϣ�б��� sdwCtrlFlg = 0/1 ʱ,��tDiagSingleEpldInfo[0]��ֵ,�� sdwCtrlFlg = 2 ʱ ,tDiagSingleEpldInfo����ֵ*/
}_PACKED_1_ T_DiagEpldInfo;


/* EEPROM�����  */
typedef struct
{
    SWORD32    sdwCtrlId;                                 /* ������ID,��ʱ����Ϊ0��1����Ӧ�ĺ궨��CTRL0��CTRL1*/
    SWORD32    sdwEepromDeviceStatus;                     /* Eeprom�豸״̬,����ΪDEVICE_STATUS_OK���쳣ΪDEVICE_STATUS_FALSE*/
}_PACKED_1_ T_DiagSingleEepromInfo;

typedef struct
{
    SWORD32 sdwCtrlFlg;                                   /* ��������־ ,��ʱ����Ϊ0��1��2����Ӧ�ĺ궨��CTRL_LOCAL��CTRL_PEER��CTRL_DUAL,   ���*/
    SWORD32 sdwEepromNum;                                 /* ���Eeprom����Ŀ������ */
    T_DiagSingleEepromInfo tDiagSingleEepromInfo[2];      /* �����Ϣ�б��� sdwCtrlFlg = 0/1 ʱ,��tDiagSingleEepromInfo[0]��ֵ,�� sdwCtrlFlg = 2 ʱ ,tDiagSingleEepromInfo����ֵ*/
}_PACKED_1_ T_DiagEepromInfo;


/* CRYSTAL����� */
typedef struct
{
    SWORD32  sdwCtrlId;                                   /* ������ID,��ʱ����Ϊ0��1����Ӧ�ĺ궨��CTRL0��CTRL1 */
    SWORD32  sdwCrystalTotalNum;                          /* �����ܸ��� */
    SWORD32  sdwCrystalDeviceStatus;                      /* Crystal�豸״̬,����ΪDEVICE_STATUS_OK���쳣ΪDEVICE_STATUS_FALSE */
}_PACKED_1_  T_DiagSingleCrystalInfo;

typedef struct
{
    SWORD32 sdwCtrlFlg;                                   /* ��������־ ,��ʱ����Ϊ0��1��2����Ӧ�ĺ궨��CTRL_LOCAL��CTRL_PEER��CTRL_DUAL,   ���*/
    SWORD32 sdwCrystalNum;                                /* ��Ͼ������Ŀ������ */
    T_DiagSingleCrystalInfo tDiagSingleCrystalInfo[2];    /* �����Ϣ�б��� sdwCtrlFlg = 0/1 ʱ,��tDiagSingleCrystalInfo[0]��ֵ,�� sdwCtrlFlg = 2 ʱ ,tDiagSingleCrystalInfo����ֵ*/
}_PACKED_1_ T_DiagCrystalInfo;


/* RTC����� */
typedef struct
{
    SWORD32     sdwCtrlId;                               /* ������ID,��ʱ����Ϊ0��1����Ӧ�ĺ궨��CTRL0��CTRL1*/
    SWORD32     sdwRtcDeviceStatus;                      /* Rtc�豸״̬,����ΪDEVICE_STATUS_OK���쳣ΪDEVICE_STATUS_FALSE */
}_PACKED_1_  T_DiagSingleRtcInfo;

typedef struct
{
    SWORD32 sdwCtrlFlg;                                  /* ��������־ ,��ʱ����Ϊ0��1��2����Ӧ�ĺ궨��CTRL_LOCAL��CTRL_PEER��CTRL_DUAL,   ���*/
    SWORD32 sdwRtcNum;                                   /* ���Rtc����Ŀ������ */
    T_DiagSingleRtcInfo tDiagSingleRtcInfo[2];           /* �����Ϣ�б��� sdwCtrlFlg = 0/1 ʱ,��tDiagSingleRtcInfo[0]��ֵ,�� sdwCtrlFlg = 2 ʱ ,tDiagSingleRtcInfo����ֵ*/
}_PACKED_1_ T_DiagRtcInfo;


/* ���໷����� */
typedef struct
{
    SWORD32     sdwCtrlId;                               /* ������ID,��ʱ����Ϊ0��1����Ӧ�ĺ궨��CTRL0��CTRL1*/
    SWORD32     sdwClockDeviceStatus;                    /* ���໷�豸״̬,����ΪDEVICE_STATUS_OK���쳣ΪDEVICE_STATUS_FALSE*/
}_PACKED_1_  T_DiagSingleClockInfo;

typedef struct
{
    SWORD32 sdwCtrlFlg;                                 /* ��������־ ,��ʱ����Ϊ0��1��2����Ӧ�ĺ궨��CTRL_LOCAL��CTRL_PEER��CTRL_DUAL,   ���*/
    SWORD32 sdwClockNum;                                /* ������໷����Ŀ������ */
    T_DiagSingleClockInfo tDiagSingleClockInfo[2];      /* �����Ϣ�б��� sdwCtrlFlg = 0/1 ʱ,��tDiagSingleClockInfo[0]��ֵ,�� sdwCtrlFlg = 2 ʱ ,tDiagSingleClockInfo����ֵ*/
}_PACKED_1_ T_DiagClockInfo;

/* PM8001 ����� */
typedef struct
{
    SWORD32     sdwCtrlId;                              /* ������ID,��ʱ����Ϊ0��1����Ӧ�ĺ궨��CTRL0��CTRL1*/
    SWORD32     sdwPmDeviceStatus;                      /* PM8001 �豸״̬,����ΪDEVICE_STATUS_OK���쳣ΪDEVICE_STATUS_FALSE*/
}_PACKED_1_  T_DiagSinglePmInfo;

typedef struct
{
    SWORD32 sdwCtrlFlg;                                 /* ��������־ ,��ʱ����Ϊ0��1��2����Ӧ�ĺ궨��CTRL_LOCAL��CTRL_PEER��CTRL_DUAL,   ���*/
    SWORD32 sdwPmNum;                                   /* ��� PM8001 ����Ŀ������ */
    T_DiagSinglePmInfo tDiagSinglePmInfo[2];            /* �����Ϣ�б��� sdwCtrlFlg = 0/1 ʱ,��tDiagSinglePmInfo[0]��ֵ,�� sdwCtrlFlg = 2 ʱ ,tDiagSinglePmInfo����ֵ*/
}_PACKED_1_ T_DiagPmInfo;

/* PCIE ����� */
typedef struct
{
    SWORD32     sdwCtrlId;                             /* ������ID,��ʱ����Ϊ0��1����Ӧ�ĺ궨��CTRL0��CTRL1*/
    SWORD32     sdwLinkSpeed;                          /* PCIE ��������*/
    SWORD32     sdwLinkWidth;                          /* PCIE ���Ӵ���x1~x4 x0��ʾ��link */
    SWORD32     sdwBadTlpCount;                        /* TLP�㻵������ */
    SWORD32     sdwBadDllpCount;                       /* DLLP�㻵������ */
    SWORD32     sdwEepromStatus;                       /* PCIE �豸E2prom��λ��� ��Ч��� */
    SWORD32     sdwDllpStatus;                         /* DLLP ״̬ */
    SWORD32     sdwPcieDeviceStatus;                   /* PCIE �豸״̬,����ΪDEVICE_STATUS_OK���쳣ΪDEVICE_STATUS_FALSE*/
}_PACKED_1_  T_DiagSinglePcieInfo;

typedef struct
{
    SWORD32 sdwCtrlFlg;                                /* ��������־ ,��ʱ����Ϊ0��1��2����Ӧ�ĺ궨��CTRL_LOCAL��CTRL_PEER��CTRL_DUAL,   ���*/
    SWORD32 sdwPcieNum;                                /* ���PCIE ����Ŀ������ */
    T_DiagSinglePcieInfo tDiagSinglePcieInfo[2];       /* �����Ϣ�б��� sdwCtrlFlg = 0/1 ʱ,��tDiagSinglePcieInfo[0]��ֵ,�� sdwCtrlFlg = 2 ʱ ,tDiagSinglePcieInfo����ֵ*/
}_PACKED_1_ T_DiagPcieInfo;

typedef struct
{
    CHAR    cDiagDeviceName[MAX_DIAG_DEVICE_NAME_LEN];    /* ��϶������ƣ�   ����  */
    CHAR    cDiagProperty[MAX_PROP_NAME_LEN];             /* ��϶������ԣ�   ����  */
    CHAR    cDiagPropertyValue[MAX_PROP_VALUE_LEN];       /* ��϶�������ֵ�� ����  */
}_PACKED_1_ T_DiagDeviceProperty;

/* ͨ����϶���ṹ�� */
typedef struct
{
    SWORD32                 sdwDiagRetNum;                                /* ��϶��󷵻صļ�¼������ ���� */
    T_DiagDeviceProperty    tDiagDeviceProperty[MAX_PROP_REC_NUM];        /* ���еĲ�ѯ��¼ */
}_PACKED_1_ T_DiagSingleDeviceRecord;

typedef struct
{
    SWORD32 sdwCtrlFlg;                                   /* ��������־ ,��ʱ����Ϊ0��1��2����Ӧ�ĺ궨��CTRL_LOCAL��CTRL_PEER��CTRL_DUAL,  ���  */
    SWORD32 sdwDiagDeviceId;                              /* ��϶���ID,  ���  */
    SWORD32 sdwDeviceNum;                                 /* ��϶������Ŀ,  ����  */
    T_DiagSingleDeviceRecord tDiagSingleDeviceRecord[2];  /* �����Ϣ�б�,�� sdwCtrlFlg = 0/1 ʱ,��tDiagSingleDeviceInfo[0]��ֵ,�� sdwCtrlFlg = 2 ʱ ,tDiagSingleDeviceInfo����ֵ*/
}_PACKED_1_ T_DiagDeviceRecord;



/* *************************EM**************************** */
typedef struct
{
    WORD32   udwEncId;                                   /* ��չ��Id */
    WORD64   uqwExpSasAddr;                              /* Expander SAS ��ַ */
    WORD32   udwDiskNum;                                 /* Expander ���ӵĴ�����Ŀ*/
    WORD32   udwSlotId[SCANBOX_MAX_DISK_NUM];            /* Expander ���ӵĴ��̲�λ���б�*/
} _PACKED_1_ T_ExpanderInfo;

typedef struct
{
    WORD32   udwExpNum;                                  /* Expander ��Ŀ*/
    T_ExpanderInfo  tExpInfo[SCANBOX_MAX_EXP_NUM];       /* Expander ��Ϣ�б�*/
} _PACKED_1_ T_ExpInfoList;

typedef struct
{
    WORD32   udwEncType;                    /* ��չ�����ͣ� */
} _PACKED_1_ T_EnclosureType;


/*������Ϣ*/
typedef struct
{
    SWORD32 sdwLevel; /*ת�ټ���*/
    WORD16 swSpeed; /*ת��*/
}_PACKED_1_ T_EncFanInfo;

/*��Դ��Ϣ*/
typedef struct
{
    SWORD32 sdwBrokenFlag; /*��Դ����,0��1*/
    SWORD32 sdwPresentFlag;  /*��Դ��λ��־,0��1*/
}_PACKED_1_ T_EncPsInfo;


/*��ѹ��Ϣ*/
typedef struct
{
    SWORD32 sdwHighCritical;  /*�������*/
    SWORD32 sdwHighWarning;  /*��߸澯*/
    SWORD32 sdwLowWarning;  /*��͸澯*/
    SWORD32 sdwLowCritical;  /*�������*/
    SWORD32 sdwValue;  /*ʵ��ֵ*/
    SWORD32 sdwWarnFlag;   /*�澯��ǣ�ȡֵOUT_FAILURE��OUT_WARNING��UNDER_FAILURE��UNDER_WARNING*/
}_PACKED_1_ T_EncVoltInfo;

/*�¶���Ϣ*/
typedef struct
{
    SWORD32 sdwHighCritical;  /*�������*/
    SWORD32 sdwHighWarning;  /*��߸澯*/
    SWORD32 sdwLowWarning;  /*��͸澯*/
    SWORD32 sdwLowCritical;  /*�������*/
    SWORD32 sdwValue;       /*ʵ��ֵ*/
    SWORD32 sdwWarnFlag;   /*�澯��ǣ�ȡֵOUT_FAILURE��OUT_WARNING��UNDER_FAILURE��UNDER_WARNING*/
}_PACKED_1_ T_EncTempratureInfo;


/*������Ϣ*/
typedef struct
{
    WORD64  uqwSasAddr;  /*����SAS��ַ*/
    SWORD32 sdwSlotId;  /*��λ��*/
}_PACKED_1_ T_EncDiskInfo;

/*�������˽ṹ*/
typedef struct
{
    BYTE ucVisted;  /*���ʱ�־*/
    BYTE ucChassisId;  /*����ID*/
    BYTE ucSlotId;  /*��λID*/
    BYTE ucDiskNum;  /*Ӳ�̸���*/
    BYTE ucSubexpnum;  /*��������*/
    BYTE ucHeadflag;  /*ͷ��־*/
    BYTE ucSubId[SUBID_LEN]; /*��չ��ID*/
    BYTE ucSgName[MAX_DISKNAMELEN];  /*SG�豸��*/

    WORD64   uqwSasAddr;  /*EXPsaS��ַ*/
    T_EncDiskInfo tDiskinfo[MAX_DISK_NUM];  /*������Ϣ*/
    BYTE ucSub[MAX_UP_SUB_ENCL];              /*��������ID�б�*/

}_PACKED_1_ T_EncTopoInfo;

/*�����������˽ṹ*/
typedef struct
{
    BYTE ucExpcount;  /*�������*/
    T_EncTopoInfo tExpanders[MAX_UP_SUB_ENCL];  /*��������ͷ*/
}_PACKED_1_  T_EncTopoInfoList;




/* �������̵�SMART��Ϣ */
typedef struct
{
    SWORD32 sdwPdSlotId;            /* ���̲�λ�ţ�ȡֵ0~27(ScanBox);  0~11(SPR10) */
    SWORD32 sdwPdEnclosureId;  /* ����Id */
    BYTE ucAucPdName[12];   /* �����豸�� ��sda */
    BYTE ucIsSupportSmart;  /* �����Ƿ�֧��SMART */
    BYTE ucIsSmartEnable;   /* ����SMART�����Ƿ��� */
    BYTE ucSmartStatus;     /* ���̵�SMART״̬�Ƿ�OK */
    WORD32 dwSmartInfoCount;/*SMART���Ը���*/
    T_DmSmartDetailInfo tSmartDetailInfo[30];   /* ����SMART������ϸ��Ϣ */
    SWORD32 sdwSelfTestTime;  /* ִ���Լ�����ʱ�� */
} _PACKED_1_ T_DmSmartInfo;

typedef struct
{
    //SWORD32 sdwCtrlId;        /* ������Id ,��ʱ����Ϊ0��1����Ӧ�ĺ궨��CTRL0��CTRL1*/
    SWORD32 sdwAction;      /*������־: 0/1/2����Ӧ�ĺ궨��Ϊ"OFF"�� "ON"�� */
}_PACKED_1_ T_DmPowerStatSwitch;

typedef struct
{
    SWORD32 sdwPdSlotId;            /* ���̲�λ�ţ�ȡֵ0~27(ScanBox);  0~11(SPR10) */
    SWORD32 sdwPdEnclosureId;  /* ����Id */
    SWORD32 sdwTestType;        /*�����Լ�����DM_SMART_SHORT_SELFTEST  DM_SMART_LONG_SELFTEST*/
    SWORD32 sdwTime;      /*�����Լ��¼� */
}_PACKED_1_ T_DmSmartTestCtl;

/*�����豸��Ϣ*/
typedef struct
{
    BYTE ucChassisId;  /*����ID*/
    BYTE ucSlotId;  /*��λID*/
    BYTE ucFanNum;  /*���ȸ���*/
    BYTE ucPsNum;  /*��Դ����*/
    BYTE ucTemperatureNum;  /*�¶ȴ���������*/
    BYTE ucVoltNum;  /*��ѹ����������*/
    BYTE ucDiskNum;  /*Ӳ�̸���*/

    BYTE ucSubId[SUBID_LEN]; /*��չ��ID*/
    BYTE ucSgName[MAX_DISKNAMELEN];  /*SG�豸��*/
    WORD64   uqwSasAddr;  /*EXPsaS��ַ*/

    T_EncFanInfo tFaninfo[MAX_FAN_NUM];  /*������Ϣ*/
    T_EncPsInfo tPsinfo[MAX_PS_NUM];  /*��Դ��Ϣ*/
    T_EncVoltInfo tVoltinfo[MAX_VOLT_NUM];  /*��ѹ��Ϣ*/
    T_EncTempratureInfo tTempratureinfo[MAX_TEMP_NUM];  /*�¶���Ϣ*/

}_PACKED_1_ T_EncDeviceInfo;
/*���������豸��Ϣ�б�*/
typedef struct
{
    BYTE ucExpcount;  /*�������*/
    T_EncDeviceInfo tExpanders[MAX_UP_SUB_ENCL];  /*��������ͷ*/
}_PACKED_1_  T_EncDeviceInfoList;

typedef struct
{
    BYTE ucflag;/*�Ƿ����¼�����0���¼� 1���¼�*/
    BYTE ucBrokenFlag;/*����1�ָ�0*/

}_PACKED_1_  T_PsEvent;

typedef struct
{
    BYTE ucflag;/*�Ƿ����¼�����0���¼� 1���¼�*/
    BYTE ucBrokenFlag;/*����1�ָ�0*/

}_PACKED_1_  T_FanEvent;

typedef struct
{
    BYTE ucflag;/*�Ƿ����¼�����0���¼� 1���¼�*/
    BYTE ucBrokenFlag;/*����1�ָ�0*/
    BYTE ucWarnFlag; /*�澯��ǣ�ȡֵWARNING_RECOVER OUT_FAILURE��OUT_WARNING��UNDER_FAILURE��UNDER_WARNING*/

}_PACKED_1_  T_VoltEvent;
typedef struct
{
    BYTE ucflag;/*�Ƿ����¼�����0���¼� 1���¼�*/
    BYTE ucBrokenFlag;/*����1�ָ�0*/
    BYTE ucWarnFlag; /*�澯��ǣ�ȡֵWARNING_RECOVER OUT_FAILURE��OUT_WARNING��UNDER_FAILURE��UNDER_WARNING*/

}_PACKED_1_  T_TempEvent;
typedef struct
{
    BYTE ucChassisId;
    BYTE ucEventCount;  /*�¼�����*/
    T_PsEvent tPsevent[MAX_PS_NUM];/*��Դ�¼�*/
    T_FanEvent tFanevent[MAX_FAN_NUM];/*�����¼�*/
    T_VoltEvent tVoltevent[MAX_VOLT_NUM];/*��ѹ�¼�*/
    T_TempEvent tTempevent[MAX_TEMP_NUM];/*�¶��¼�*/
}_PACKED_1_  T_EncEvent;

typedef struct
{

    BYTE ucAllEventCount;  /*�¼�����*/
    T_EncEvent  tExpanderEvent[MAX_UP_SUB_ENCL];  /*��������ͷ*/
}_PACKED_1_  T_EncEventList;

/* *************************LOG**************************** */
typedef struct
{
    T_Time  tLogTime;                                /* ��־���������ϲ㴫���ʱ��ṹ�� */
    BYTE    ucLogLevel;                              /* ��־���� */
    BYTE    ucLogSrc;                                /* ��־��Դ */
    SWORD16 swLogId;                                 /* ��־ID�ţ���־��Ψһ��ʶ�� */
    CHAR    ucEventData[MAX_LOG_LEN];                /* ��־��������������������־���� */
    CHAR    cUserName[MAX_USER_NAME_LEN];            /* ��ѯ��־���û��� */
}_PACKED_1_ T_LogTableNode;

typedef struct
{
   SWORD32   sdwNum;                                 /* ��־�б��е���־��Ŀ */
   T_LogTableNode tLog[MAX_LOG_NUM];                 /* ��־�б����������ʽ���� */
}_PACKED_1_ T_LogTableList;

typedef struct
{
   SWORD32   sdwNum;                                 /* ���β�ѯ��־ʱ����־�б��е���־��Ŀ */
   T_LogTableNode tLog[MAX_LOG_NUM_ONCE];            /* ���β�ѯ��־ʱ����־�б� */
}_PACKED_1_ T_LogTableOnce;

typedef struct
{
    BYTE        ucLogLevel;                          /* ��־��ѯʱ����־�����趨����Ҫ��Բ�ѯ��־������ */
    BYTE        ucInquireFlag;                       /* ��־��ѯ��ʶ��0Ϊ����ucLogLevel������־��1Ϊ�ü�����־���궨���usp_common_macro.h�� */
    T_Time      tStartTime;                          /* ��־��ѯ�Ŀ�ʼʱ�� */
    T_Time      tEndTime;                            /* ��־��ѯ�Ľ���ʱ�� */
    CHAR        cUserName[MAX_USER_NAME_LEN];        /* ��ѯ���û����� */
}_PACKED_1_ T_LogGetNum;

typedef struct
{
   T_LogGetNum  tLogNumInfo;                         /* ��־��ѯʱ����־�����趨 */
   WORD16       wInquireLoc;                         /* ��־��ѯʱ��Ҫ�����־����־�����е�λ�� */
   WORD16       wInquireNum;                         /* ���β�ѯ����Ŀ */
}_PACKED_1_ T_LogGet;

typedef struct
{
   CHAR  cFileName[MAX_LOG_EXPORT_NAME_LEN];        /* ��־����ʱ���ļ����� */
}_PACKED_1_ T_LogExport;

typedef struct
{
    BYTE    ucDelaySec;                              /* ��־�澯�ʼ����ӳ�ʱ�䣬���Ժϲ���־���� */
    BYTE    ucAlarmLevel;                            /* ��־�澯�ʼ����õĸ澯��־���� */
    BYTE    ucUserNum;                               /* ��־�ĸ澯�ʼ��û���Ч��ַ��Ŀ */
    CHAR    cTo[MAX_ALARMAIL_USER][MAX_MAILTO_LEN];  /* ��־�澯�ʼ����͵�Ŀ�������ַ */
}_PACKED_1_ T_LogAlarmMailCfgPack;

/*************************************MAIL Configration*********************************/
typedef struct
{
    BYTE   ucAddressFlag;                            /* ��ַ���ͣ�IP��ַ�ִ��������ִ�(0ΪIP��ַ��1Ϊ�����ִ�) */
    WORD16 wPort;                                    /* �˿ںţ�Զ���ʼ����������ʼ�����˿ں� */
    CHAR   cMailSendAcc[MAIL_SINGLE_ACCOUNT_LEN];    /* �����ʼ��ĺ�����������ȫ�� */
    CHAR   cPassword[MAIL_MAX_PASSLEN];              /* �����ʼ������� */
    CHAR   ucHost[MAIL_MAX_HOST_LEN];                /* �������֣���ucAddressFlagƥ���������ַ */
} _PACKED_1_ T_MailConfPack;

typedef struct
{
    CHAR cTo[MAX_MAILTO_LEN];          /* ���������ַ */
} _PACKED_1_ T_VerifyMailTestTo;

/***************************************User management***********************************/

typedef struct T_SessionId
{
    SWORD32      sdwSessionId;  //�û���¼��sessionid�ṹ
}_PACKED_1_ T_SessionId;

typedef struct
{
  CHAR acUsername[MAX_USER_NAME_LEN]; //�û���(�ʻ�)��Ϣ
  SWORD32 sdwUserType; //�û�����,�ο�����eUserType
}_PACKED_1_ T_Account;
typedef struct
{
  SWORD32 scAccountNum; //��ǰ�����ʻ�����
  T_Account atAccount[MAX_USER_NUM]; //��ǰ�����û���(�ʻ�)��Ϣ
}_PACKED_1_ T_AccountList;

typedef struct
{
  CHAR acUsername[MAX_USER_NAME_LEN]; //���޸����ֵ��û��ʻ�
  CHAR acUserNewname[MAX_USER_NAME_LEN]; //�µ��û�����
  CHAR acUsrlogname[MAX_USER_NAME_LEN]; //��ǰ��½�û�
}_PACKED_1_ T_UserNameMody;


typedef struct
{
  CHAR acUsername[MAX_USER_NAME_LEN]; //������������û��ʻ�
  CHAR acOldPwd[MAX_USER_PSW_LEN]; //ԭʼ����
  CHAR acNewPwd[MAX_USER_PSW_LEN]; //��������
  CHAR acUsrlogname[MAX_USER_NAME_LEN]; //��ǰ��½�û�
}_PACKED_1_ T_PwdSet;

typedef struct
{
  CHAR acUsername[MAX_USER_NAME_LEN]; //���������ʼ����û���
  CHAR acPwdMail[MAX_USER_EMAIL_LEN]; //���û����õ���������Email��ַ
  CHAR acUsrlogname[MAX_USER_NAME_LEN]; //��ǰ��½�û�
}_PACKED_1_ T_PwdMail;

typedef struct
{
    SWORD32 sdwUserId;//�û�ID
    CHAR acUsername[MAX_USER_NAME_LEN ]; //�û���
    CHAR acPassword[MAX_USER_PSW_LEN]; //����
    SWORD32 sdwUserType; //�û�����,�ο�����eUserType
    SWORD32  sdwUserUuid;
}_PACKED_1_ T_AmsUser;

/***************************************Get System Information***********************************/
typedef  struct
{
    CHAR   cSysName[USP_SYSTEM_NAME_MAX_LEN];          /* ϵͳ���� , Ĭ��ֵΪ ZXATCA_USP*/
}_PACKED_1_ T_SysNameInfo;

typedef  struct
{
    T_SysNameInfo tSysNameInfo;                           /* ϵͳ����, ����*/

    CHAR   cVendor[USP_SYSTEM_VENDOR_MAX_LEN];            /* ��Ӧ��,    Ĭ��ֵΪ ZTE, ����*/

    CHAR   cModel[USP_SYSTEM_MODEL_MAX_LEN];              /* �ͺ�,      Ĭ��ֵΪ USP, ����*/

    CHAR   cVersionName[USP_SYSTEM_VERSION_MAX_LEN];      /* ��Ʒ�汾,  Ĭ��ֵΪ V1.0, ����*/

    CHAR   cSerialNum[USP_SYSTEM_SERIAL_NUM_MAX_LEN];     /* ��Ʒ���к�, Ĭ��ֵΪ1111-1111, ����*/

    CHAR   cSysState[USP_SYSTEM_STATE_MAX_LEN];           /* ϵͳ״̬, �������л�˫�������������: Single Running��Dual Running, ���� */
}_PACKED_1_ T_SysInfo;

/************************************Get Detail Controller Information*******************************/
typedef  struct
{
    SWORD32   sdwCtrlId;                                       /*  ������ID  */
    SWORD32   sdwRamCapacity;                                  /*  RAM��С, ��λ��M  */
    SWORD32   sdwCtrlRole;                                     /*  ��������ɫ  */
    SWORD32   sdwCtrlState;                                    /*  ������״̬  */
    CHAR      cCtrlSerial[USP_CONTROLLER_SERIAL_MAX_LEN];      /*  ���������к�, Ĭ��Ϊ������UUID  */
    CHAR      cCtrlName[USP_CONTROLLER_NAME_MAX_LEN];          /*  ���������� Ĭ�ϵ�Ƭ����, ����SPR10��SBCJ */

}_PACKED_1_ T_CtrlSingleInfo;

typedef  struct
{
    SWORD32   sdwCtrlNumb;                   /* ��������Ŀ,  ����*/
    SWORD32   sdwCtrlAction;                 /* CTRL_LOCAL--����, CTRL_PEER--�Զ�, CTRL_DUAL--���ˣ����ÿ���������ʱʹ��, ��� */
    T_CtrlSingleInfo   tCtrlSingleInfo[2];   /*  ��������ϸ��Ϣ�б�,  ���� */
}_PACKED_1_ T_CtrlInfo;

/************************************Get System Boot Done Time**************************************/
typedef  struct
{
    SWORD32   sdwCtrlId;                                    /*  ������ID  */
    T_Time    tSysBootTime;                                 /*  ϵͳ�������ʱ��  */
}_PACKED_1_ T_SysSingleBootTime;

typedef  struct
{
    SWORD32               sdwCtrlNumb;                      /* ��������Ŀ,  ����*/
    T_SysSingleBootTime   tSysSingleBootTime[2];            /* ϵͳ�������ʱ���б�,  ���� */
}_PACKED_1_ T_SysBootTime;
/***************************************Pd***************************************/
/**********************************************************************
* ���������� ��ȡ���д����б�
* ��������� ��
* ���������ȫ��Ϊ����
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetAllPdId(T_AllPdId *ptAllPdId);
/**********************************************************************
* ���������� ��ȡӲ��Ӳ����
* ��������� ptPdHwInfo->sdwSLotId,pPdHwInfo->sdwEnclour
* ��������� ����Ϊ����
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
//SWORD32 GetPdHwInfo(T_PdHwInfo *ptPdHwInfo);

/**********************************************************************
* ���������� ��ȡ������Ϣ
* ��������� ptPdHwInfo->sdwSLotId,pPdHwInfo->sdwEnclour
* ��������� ����Ϊ����
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetHddInfo(T_HddInfo *ptHddInfo);

/**********************************************************************
* ���������� ��ȡ���д�����Ϣ
* ��������� ��
* ��������� ��Ϊ����
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetAllHddInfo(T_AllHddInfo *ptAllHddInfo);

/**********************************************************************
* ���������� ����Ӳ��Ӳ���ԣ��������ʡ�����ģʽ��cache���أ�
* ��������� pPdHwInfo->sdwSlotId,pPdHwInfosdwEnclouserId
* �������������Ϊ����
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 SetPdHwInfo(T_PdSetHwInfo *pPdHwInfo);
/**********************************************************************
* ���������� ��ȡӲ��������
* ��������� ptPdSwInfo->sdwSlotId,ptPdSwInfo->sdwEnclouserId
* �������������Ϊ����
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
//SWORD32 GetPdSwInfo(T_PdSwInfo *ptPdSwInfo);

/**********************************************************************
* ���������� �����̱���ȱ���
* ���������ȫ��Ϊ���
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/

SWORD32 SetPdUnusedToHotSpare(T_PdId *ptPdId);
/**********************************************************************
* ���������� �ȱ��̱�ɿ�����
* ��������� ȫ��Ϊ���
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 SetPdHotSpareToUnsed(T_PdId *ptPdId);

/**********************************************************************
* ���������� δ֪�̺ͳ�ͻ��ת��Ϊ������
* ��������� ȫ��Ϊ���
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 SetPdToUnsed(T_PdId *ptPdId);

/**********************************************************************
* ���������� �������̵���
* ��������� ȫ��Ϊ���
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32  ImportPd(T_PdId *ptPdId);

/**********************************************************************
* ���������� ���ô��̻���澯��Ŀ
* ��������� ��
* ��������� BadBlockWarnNum
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetBadBlockWarnNum(T_PdBadBlockWarnNum *ptPdBadBlockWarnNum);

/**********************************************************************
* ���������� ��ѯ���̻���澯
* ��������� BadBlockWarnNum
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 SetBadBlockWarnNum(T_PdBadBlockWarnNum *ptPdBadBlockWarnNum);


/**********************************************************************
* ������������ȡ���̵�SMART״̬��Ϣ����Ե���Ӳ�̣�
* ���������EnclosureId  ,  SlotId
* �������������Ϊ����
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetPdSmartStateInfo(T_PdSmartStateInfo *ptPdSmartStateInfo);

/**********************************************************************
* ���������� ��ѯSmart��Ϣ
* ���������EnclosureId  ,  SlotId
* �������������Ϊ����
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetSmartInfo(T_PdSmartInfo *ptSmartInfo);

/**********************************************************************
* ���������� ʹ��/ ��ֹӲ��Smart�澯����
* ��������� ptPdWarnSwitch
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 SetPdWarnSwitchStatus(T_PdWarnSwitch *ptPdWarnSwitch);

/**********************************************************************
* ���������� ��ȡ���̸澯����
* ��������� EnclosureId  ,  SlotId
* ��������� sdwSwitch
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetPdWarnSwitchStatus(T_PdWarnSwitch *ptPdWarnSwitch);

#if 0
/**********************************************************************
* ���������� ��ʼɨ�裨��Ե���Ӳ�̣�
* ��������� EnclosureId  ,  SlotId
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 StartPdScan(T_PdId *ptPdId);

/**********************************************************************
* ���������� ֹͣɨ��
* ��������� EnclosureId  ,  SlotId
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 StopPdScan(T_PdId *ptPdId);
#endif
/**********************************************************************
* ���������� ��ѯɨ���������
* ��������� EnclosureId  ,  SlotId
* ���������  ����Ϊ����
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetPdScanProgress(T_PdScanStatus *ptPdScanStatus);

/**********************************************************************
* ���������� ����ɨ��
* ��������� sdwPdSlotId��sdwPdEnclosureId��sdwCtrlFlg
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 ScanPd(T_PdScanCtrl *ptPdScanCtrl);

/**********************************************************************
* ���������� ����Ӳ�̵���˸
* ��������� ptPdLed->sdwSlotId,sdwSlotId->sdwEnclouserId
* ��������� ����Ϊ����
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 CtrlPdLed(T_PdLed *ptPdLed);

/***************************************Blk***************************************/
/**********************************************************************
* ���������� ����������
* ��������� *ptCreateVd
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 CreateVd(T_CreateVd *ptCreateVd);
/**********************************************************************
* ���������� ɾ��������
* ��������� sdwVdId
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 DelVd(T_VdName *ptVdName);
/**********************************************************************
* ���������� ��ȡ�����̸�����Id
* ��������� ��
* ��������� *ptAllVdId
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
//SWORD32 GetAllVdId(T_AllVdId *ptAllVdId);
SWORD32 GetAllVdNames(T_AllVdNames *ptAllVdNames);
/**********************************************************************
* ���������� �������Ʋ�ѯ��������Ϣ
* ��������� ptVdInfo->udwVdNum
* ��������� ����Ϊ����
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetVdInfo(T_VdInfo *ptVdInfo);

/**********************************************************************
* ���������� ��ȡ�������ϵ�����VOL����
* ��������� ptAllVolIdonVd->sdwVdID
* �������������Ϊ����
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
//SWORD32 GetVolIdOnVd(T_AllVolIdOnVd *ptAllVolIdOnVd);
SWORD32 GetVolNamesOnVd(T_AllVolNamesOnVd *ptAllVolNamesOnVd);
/**********************************************************************
* ���������� ��ȡ�������ϵľ�ID
* ��������� ptAllVolIdonVd->sdwVdID
* �������������Ϊ����
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
//SWORD32 GetVolInfoOnVd(T_AllVolInfoOnVd *ptAllVolInfoOnVd);

/**********************************************************************
* ���������� ��������������
* ��������� *ptVdName
* ���������  ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 RenameVd(T_VdRename *ptVdRename);
/**********************************************************************
* ���������� ������������ѡ������
* ��������� *ptVdPreferVtl
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
//SWORD32 SetVdPreferCtl(T_VdPreferCtl *ptVdPreferVtl);
/**********************************************************************
* ���������� �����������Զ��ؽ�����
* ��������� *ptVdAuReb
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 SetVdAutoRebuild(T_VdAutoRebuild *ptVdAutoRebuild);
/**********************************************************************
* ���������� ����������һ���Լ��
* ��������� sdwVdId
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
//SWORD32 CheckVdConsistency(SWORD32 sdwVdId);
SWORD32 CheckVdConsistency(T_VdName *ptVdName);
/**********************************************************************
* ���������� ����������У��
* ��������� sdwVdId
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
//SWORD32 VdParityRegenerate(SWORD32 sdwVdId);
SWORD32 RegenerateParityVd(T_VdName *ptVdName);
/**********************************************************************
* ���������� ��ȡ������������Ϣ
* ��������� ptVdTaskInf->sdwVdId
* ���������������Ϊ����
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetVdTaskInfo(T_VdGetTaskInfo *ptVdTaskInfo);
/**********************************************************************
* ���������� ��������Ƭ����
* ��������� sdwVdId
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
//SWORD32 VdDefragment(SWORD32 sdwVdId);
SWORD32 DefragmentVd(T_VdName *ptVdName);
/**********************************************************************
* �����������������ֶ��ؽ�
* ���������
* ���������
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
//SWORD32 VdRebuild(T_VdRebuild *);
SWORD32 RebuildVd(T_VdRebuild *ptVdRebuild);
/**********************************************************************
* ���������� ��������չ
* ���������  *ptVdExpand
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
//SWORD32 VdExpand(T_VdExpand *ptVdExpand);
SWORD32 ExpandVd(T_VdExpand *ptVdExpand);
/**********************************************************************
* ���������� �����̱伶
* ��������� *ptVdRelevel
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
//SWORD32 VdRelevel(T_VdRelevel *ptVdRelevel);
SWORD32 RelevelVd(T_VdRelevel *ptVdRelevel);
/**********************************************************************
* ���������� �����̶���
* ��������� *ptVdName
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 FreezeVd(T_VdName *ptVdName);
/**********************************************************************
* ���������� �����̽ⶳ
* ��������� *ptVdName
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 UnFreezeVd(T_VdName *ptVdName);
/**********************************************************************
* ���������� ������Ǩ��
* ��������� *ptVdName
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 ImmigrateVd(T_VdName *ptVdName);
/**********************************************************************
* ���������� ������
* ��������� *ptCreateVol
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 CreateVol(T_CreateVol *ptCreateVol);
/**********************************************************************
* ���������� ɾ����
* ��������� *ptDelVol
* �����������
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
//SWORD32 DelVol(T_DelVol *ptDelVol);
SWORD32 DelVol(T_VolName *ptVolName);
/**********************************************************************
* ���������� ��ͳ����Ϣ
* ��������� T_VolStat
* �����������
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetVolStatData(T_GetVolStat *ptStatVolInfo);

/**********************************************************************
* ���������� ��������
* ��������� ȫ��Ϊ���
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 RenameVol(T_VolRename *ptVolRename);
/**********************************************************************
* ���������� ������
* ��������� ȫ��Ϊ���
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 ExpandVol(T_ExpVol *ptExpVol);
/**********************************************************************
* ���������� �������
* ��������� ȫ��Ϊ���
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 ReSegmentVol(T_ReSegVol *ptReSegVol);
/**********************************************************************
* ���������� ��ȡָ������Ϣ
* ��������� ptVolInf->sdwVolId
* �������������Ϊ����
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetVolInfo(T_VolInfo *ptVolInfo);
/**********************************************************************
* ���������� ��д0��ʼ��
* ��������� ptVolInit->sdwVdId,   ptVolInit->sdwVolId
* �������������Ϊ����
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 ZeroInitVol(T_VolName *VolName);
/**********************************************************************
* ���������� ���þ�prefer������
* ��������� ȫ��Ϊ���
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 SetVolPreferCtrl(T_VolPreferCtrl *ptVolPreferCtrl);
/**********************************************************************
* ���������� ��ȡ���������Ϣ
* ��������� ptVolTaskInf->sdwVdId, ptVolTaskInf->sdwVolId
* �������������Ϊ����
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetVolTaskInfo(T_VolGetTaskInfo *ptVolTaskInfo);
/**********************************************************************
* ���������� ��������
* ��������� ȫ��Ϊ���
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 CreateSvol(T_SvolCreate *ptSvolCreate);
/**********************************************************************
* ���������� ɾ������
* ��������� ȫ��Ϊ���
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 DelSvol(T_SvolName *ptSvolName);
/**********************************************************************
* ���������� ֹͣ����
* ��������� ȫ��Ϊ���
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵���� �ײ�SNAP��ʱ��С���ܼ��в���
***********************************************************************/
//SWORD32 StopSvol(T_SvolStop *ptSvolStop);
/**********************************************************************
* ���������� �ؽ�����
* ��������� sdwSvolId
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵���� �ײ�SNAP��ʱ��С���ܼ��в���
***********************************************************************/
//SWORD32 RecreateSvol(T_SvolName *ptSvolName);
/**********************************************************************
* ���������� �ָ�����
* ��������� sdwSvolId
* �����������
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵�����ײ�SNAP��ʱ��С���ܼ��в���
***********************************************************************/
SWORD32 RestoreSvol(T_SvolName *ptSvolName);
/**********************************************************************
* ���������� ����������
* ��������� ȫ��Ϊ���
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 RenameSvol(T_SvolRename *ptSvolRename);
/**********************************************************************
* ���������� �鿴ָ�����վ�����
* ��������� ptSvolInfo-> udwSvolId;
* ��������� ����Ϊ����
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetSvolInfo(T_SvolInfo *ptSvolInfo);

/**********************************************************************
* ���������� �鿴���ϵ����п��վ���Ϣ
* ���������
* ��������� ȫ��
* �� �� ֵ��  0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetAllSvolInfoOnVol(T_AllSvolInfoOnVol *ptAllSvolInfoOnVol);

/**********************************************************************
* ���������� �鿴���п��վ�����
* ���������
* ��������� ȫ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
// SWORD32 GetAllSvolInfo(T_AllSvolInfo *ptAllSvolInfo);

/**********************************************************************
* ���������� �鿴ָ�����ձ��ݾ�����
* ��������� ptRvolInfo->udwRvolId
* �������������Ϊ����
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
//SWORD32 GetRvolInfo(T_RvolInfo *ptRvolInfo);
/**********************************************************************
* ���������� ���ÿ��վ�澯��ֵ
* ��������� ȫ��Ϊ���
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 SetSvolThr(T_SvolThr *ptSvolThr);

/**********************************************************************
* ��������������������
* ���������
* ��������� ��
* ����ֵ��    0   - SUCCESS    <0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 CreateCvol(T_CreateCvol  *ptCreateCvol);

/**********************************************************************
* ���������� ��ȡ��������Ϣ
* ���������
* ��������� ��
* ����ֵ��    0   - SUCCESS    <0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetCvolInfo(T_CvolInfo *ptCvolInfo);

/**********************************************************************
* ���������� ��ȡԴ���ϵĿ�������Ϣ
* ���������
* ��������� ��
* ����ֵ��    0   - SUCCESS    <0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetCvolNamesOnVol(T_CvolNamesOnVol *ptCvolNamesOnVol) ;

/**********************************************************************
* ���������� ���ÿ�����ͬ�����߷�ͬ�������ȼ�
* ���������
* ��������� ��
* ����ֵ��    0   - SUCCESS    <0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 SetCvolPriority(T_CvolPriority *ptCvolPriority);

/**********************************************************************
* ���������� �����Ƿ�������������
* ���������
* ��������� ��
* ����ֵ��    0   - SUCCESS    <0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 SetCvolProtect (T_CvolProtect *ptCvolProtect);

/**********************************************************************
* ����������ɾ��������
* ���������
* ��������� ��
* ����ֵ��    0   - SUCCESS    <0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 DelCvol(T_CvolName *ptCvolName);

/**********************************************************************
* ���������� ���뿽����
* ���������
* ��������� ��
* ����ֵ��    0   - SUCCESS    <0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 FractureCvol(T_CvolName  *ptCvolName);

/**********************************************************************
* ���������� ͬ��������
* ���������
* ��������� ��
* ����ֵ��    0   - SUCCESS    <0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 SyncCvol(T_CvolName *ptCvolName);

/**********************************************************************
* ������������ͬ��������
* ���������
* ��������� ��
* ����ֵ��    0   - SUCCESS    <0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 ReverseSyncCvol(T_CvolName *ptCvolName);

/**********************************************************************
* ���������� ������������
* ��������� ȫ��Ϊ���
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 RenameCvol(T_CvolRename *ptCvolRename);

/**********************************************************************
* ������������ȡϵͳ��δ��ӳ��ľ����(������ͨ��/����/������)��Ϣ
* ���������
* ��������� ��
* ����ֵ��    0   - SUCCESS    <0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetVolUnmappedInfo(T_VolObjectInfoList *ptVolObjectInfoList);

/***************************************Lun*************************************/
/**********************************************************************
* ���������� ����LUN
* ��������� ȫ�����
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
//SWORD32 CreateLun(T_LunSimpleInfo *);
/**********************************************************************
* ���������� ɾ��LUN
* ��������� sdwLunGlbId
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
//SWORD32 DelLun(SWORD32 sdwLunGlbId);
/**********************************************************************
* ���������� ��ѯ��ǰϵͳ�д��ڵ�����LUN��ID
* ��������� ��
* ��������� ȫ��Ϊ����
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
//SWORD32 GetAllLunId(T_AllLunId *);
/**********************************************************************
* ���������� ����ID��ѯָ��LUN��Ϣ
* ��������� ptLunSimpleInfo->sdwLunGlbId
* ��������� ����Ϊ����
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
//SWORD32 GetLunSimpleInfo(T_LunSimpleInfo *ptLunSimpleInfo);
/**********************************************************************
* ���������� �޸�VOL��Cache����
* ��������� ptLunCacheInfo->sdwLunGlbId
* ��������� ����Ϊ����
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 SetVolCacheAttr(T_VolCacheInfo *ptVolCacheInfo);
/**********************************************************************
* ���������� ����ӳ����
* ��������� sdwMapGrpId
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 CreateMapGrp(T_MapGrpName *ptMapGrpName);

/**********************************************************************
* ���������� ɾ��ӳ����
* ��������� sdwMapGrpId
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 DelMapGrp(T_MapGrpName *ptMapGrpName);

/**********************************************************************
* ���������� ������ӳ����
* ��������� sdwMapGrpId
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 RenameMapGrp(T_MapGrpRename *ptMapGrpRename);

/**********************************************************************
* ���������� ��ȡӳ������Ϣ
* ��������� ptMapGrpInfo
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetMapGrpInfo(T_MapGrpInfo *ptMapGrpInfo);

/**********************************************************************
* ���������� VOL����ӳ����
* ��������� ȫ��Ϊ���
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 AddVolToGrp(T_AddVolToGrp *ptAddVolToGrp);
/**********************************************************************
* ���������� VOL��ӳ������ɾ��
* ��������� ȫ��Ϊ���
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 DelVolFromGrp(T_DelVolFrmGrp *ptDelVolFromGrp);
/**********************************************************************
* ���������� Host����ӳ����
* ��������� ȫ��Ϊ���
* ���������  ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 AddHostToGrp(T_HostInGrp *ptHostInGrp);
/**********************************************************************
* ���������� Host��ӳ������ɾ��
* ���������  ȫ��Ϊ���
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 DelHostFromGrp(T_HostInGrp *ptHostInGrp);
/**********************************************************************
* ���������� ��ѯ����ӳ����ID�б�
* ��������� ��
* ��������� ȫ��Ϊ����
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetGrpNamesList(T_AllGrpNames *ptAllGrpNames);

/**********************************************************************
* ���������� ��ѯָ��ӳ����������host
* ��������� ptAllHostGrp->sdwMapGroup
* ��������� ����Ϊ����
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
//SWORD32 GetGrpHostList(T_AllHostInGrp *ptAllHostGrp);
/**********************************************************************
* ���������� ��ѯָ��ӳ����������lun
* ��������� ptAllLunGrp->swMapGroupId
* ��������� ����Ϊ����
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
//SWORD32 GetGrpLunList(T_AllLunInGrp *ptAllLunGrp);
/**********************************************************************
* ���������� ��ѯָ��host����ӳ��������
* ��������� ptHostGrpId->ucInitName
* ��������� ����Ϊ����
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetGrpNameOfHost(T_HostInGrp *ptHostInGrp);
/**********************************************************************
* ���������� ��ѯVOL���ڵ�ӳ�����б�
* ��������� ptLunGrpId->sdwLunGlbId
* ��������� ����Ϊ����
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetGrpNamesOfVol(T_GrpNameByVol *ptGrpNameByVol);
/**********************************************************************
* ���������� ��/�ر�ӳ������Lun��IOͳ�ƿ���
* ��������� ȫ��Ϊ���
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ��������� ��
* ����˵����
***********************************************************************/
SWORD32 SetLunIOStaticSwitchStatus(T_LunIOStaticSwitch *ptVolIOStaticSwitch);
/**********************************************************************
* ���������� ��ѯӳ������Lun��IOͳ�ƿ���״̬
* ��������� ȫ��Ϊ���
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetLunIOStaticSwitchStatus(T_LunIOStaticSwitch *ptVolIOStaticSwitch);
/**********************************************************************
* ���������� ��ȡӳ������Lun��IOͳ������
* ���������ptLunIoStaticData->sdwLunLocalId,ptLunIoStaticData->ucInitName
* �������������Ϊ����
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetLunIOStaticData(T_LunIOStaticData *ptVolIoStaticData);

/***************************************Cache***********************************/
/**********************************************************************
* ���������� ����cacheȫ������
* ��������� ȫ��Ϊ���
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 SetCacheGlobalPolicy(T_SetCacheGlbPolicy *ptSetCacheGlbPolicy);
/**********************************************************************
* ���������� ��ȡcacheȫ������
* ��������� ��
* ��������� ȫ��Ϊ����
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetCacheGlobalPolicy(T_GetCacheGlbPolicy *ptGetCacheGlbPolicy);
/**********************************************************************
* ���������� ��ȡbuffer��ʹ����Ϣ��
* ��������� ��
* ��������� ȫ��Ϊ����
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetCacheBuffInfo(T_CacheBuffInfo *ptCacheBuffInfo);
/**********************************************************************
* ���������� ���ͳ�ƿ��ص�״̬
* ��������� ��
* ���������sdwSwitch
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetCacheStatSwitchStaus(T_CacheIOStatSwitch *ptCacheIOStatSwitch);
/**********************************************************************
* ���������� ����ͳ�ƿ���
* ��������� sdwSwitch
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 SetCacheStatSwitchStaus(T_CacheIOStatSwitch *ptCacheIOStatSwitch);
/**********************************************************************
* ���������� ��ȡIOͳ����Ϣ��������д�����������С��������
* ��������� ��
* ��������� ȫ��Ϊ����
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetCacheStatData(T_CacheIOStatData *ptCacheIOStatData);

/***************************************Other***********************************/
/**********************************************************************
* ���������� ��ȡ���з�װID
* ��������� ��
* ��������� ȫ��Ϊ����
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetAllEncId(T_AllEncId *ptAllEncId);

/* SWORD32 GetCtlHwInfo();*/
/**********************************************************************
* ���������� ��չ��汾����
* ��������� *ptUpdateFirmware
* ��������� ��
* ����˵����
***********************************************************************/
SWORD32 UpdateFirmware(T_UpdateFirmware *ptUpdateFirmware);
/**********************************************************************
* ���������� ��չ��汾��ѯ
* ��������� ��
* ��������� *ptFirmwareVer
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetFirmwareVer(T_FirmwareVersion *ptFirmwareVer);
/**********************************************************************
* ���������� ����������
* ��������� ȫ��Ϊ���
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 RestartCtrl(T_CtrlAction *ptCtrlAction);
/**********************************************************************
* ���������� �رտ�����
* ��������� ȫ��Ϊ���
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 ShutdownCtrl(T_CtrlAction *ptCtrlAction);
/**********************************************************************
* ���������� ��ȡ��ǰ������ID
* ��������� ��
* ���������*sdwCtlId
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetLocalCtlId(SWORD32 *sdwCtlId);
/**********************************************************************
* ���������� ��ȡ��ǰ������״̬
* ��������� ��
* ��������� *LocalCtrlState
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetLocalCtrlState(SWORD32 *LocalCtrlState);
/**********************************************************************
* ���������� ��ȡ�Զ˿�����״̬
* ��������� ��
* ��������� *PeerCtlState
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetPeerCtlState(SWORD32 *PeerCtlState);
/**********************************************************************
* ���������� ��ȡ��ǰ��������ɫ
* ��������� ��
* ��������� *LocalCtlRole
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetLocalCtlRole(SWORD32 *LocalCtlRole);

/**********************************************************************
* ���������� �����ñ���Ϊָ�������ļ�
* ��������� *ptConfigFile
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 SaveConfigFile(T_ConfigFile *ptConfigFile);
/**********************************************************************
* ���������� �ָ�ָ���ļ�����
* ��������� *ptConfigFile
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 RestoreConfigFile(T_ConfigFile *ptConfigFile);

/**********************************************************************
* ���������� �ָ���������
* ��������� ��
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 RestoreOriginalConfig(void);
/**********************************************************************
* ���������� ���������־����
* ��������� sdwLevel
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 SetLogLevel(SWORD32 sdwLevel);
/**********************************************************************
* ���������� ����Ԥ�õĹ���������ѯ��־����ϸ����
* ��������� T_LogGet  *ptLogGetPara�����Ԥ�õĹ�������ϸ��
* ��������� T_LogTableOnce *ptLogTableList
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵�����û������ϲ�ָ���û�������£�
                                 �����û�������־��ѯ�������£�
                                 �û����ֶ���Ҫ��λΪLOG_USER_NONE_DEFAULT_NAME
***********************************************************************/
SWORD32 GetLogByLevel(T_LogGet  *ptLogGetPara, T_LogTableOnce *ptLogTableList);
/**********************************************************************
* ���������� ��־������ָ��Ŀ¼�ļ�
* ��������� �����ļ������ƣ�����·��
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 ExportLog(T_LogExport *pcFileName);
/**********************************************************************
* ���������� �����Ӧ����־
* ��������� T_LogUserName *ptUserName
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵�����û������ϲ�ָ���û�������£�
                                 �����û�������־��ѯ�������£�
                                 �û����ֶ���Ҫ��λΪLOG_USER_NONE_DEFAULT_NAME
***********************************************************************/
SWORD32 ClearLog(void);
/**********************************************************************
* ���������� ��÷��Ϲ�����������־����
* ��������� T_LogGetNum *ptLogNumInfo�����Ԥ�õĹ�������ϸ��
* ��������� ��õ���־��ĿwLogNum
* �� �� ֵ��  0      -   ִ�гɹ�
*             ��0    -   ִ��ʧ��
* ����˵�����û������ϲ�ָ���û�������£�
                                 �����û�������־��ѯ�������£�
                                 �û����ֶ���Ҫ��λΪLOG_USER_NONE_DEFAULT_NAME
***********************************************************************/
SWORD32 GetLogNum(T_LogGetNum *ptLogNumInfo, WORD16 *pwLogNum);

/**********************************************************************
* ���������� �������������Ϣ
* ��������� ��϶�������diagDeviceType����Ϸ�����DiagSourceType
* ��������� ����Ϊ����
* �� �� ֵ��  0      -   ִ�гɹ�
*             ��0    -   ִ��ʧ��
* ����˵������
***********************************************************************/
SWORD32 GetDiagDeviceInfo(T_DiagDeviceInfo *ptDiagDeviceInfo);

/**********************************************************************
* ���������� ���BBU��Դ�����Ϣ
* �����������
* ��������� ȫ��
* �� �� ֵ��  0      -   ִ�гɹ�
*             ��0    -   ִ��ʧ��
* ����˵������
***********************************************************************/
SWORD32 GetDiagBbuInfo(T_DiagBatteryInfo *ptDiagbatteryInfo);


/**********************************************************************
* ���������� ���EPLD�����Ϣ
* �����������
* ��������� ȫ��
* �� �� ֵ��  0      -   ִ�гɹ�
*             ��0    -   ִ��ʧ��
* ����˵������
***********************************************************************/
SWORD32 GetDiagEpldInfo(T_DiagEpldInfo *ptDiagEpldInfo);

/**********************************************************************
* ���������� ���EEPROM�����Ϣ
* �����������
* ��������� ȫ��
* �� �� ֵ��  0      -   ִ�гɹ�
*             ��0    -   ִ��ʧ��
* ����˵������
***********************************************************************/
SWORD32 GetDiagEepromInfo(T_DiagEepromInfo *ptDiagEepromInfo);

/**********************************************************************
* ���������� ���Rtc�豸�����Ϣ
* �����������
* ��������� ȫ��
* �� �� ֵ��  0      -   ִ�гɹ�
*             ��0    -   ִ��ʧ��
* ����˵������
***********************************************************************/
SWORD32 GetDiagRtcInfo(T_DiagRtcInfo *ptDiagRtcInfo);

/**********************************************************************
* ���������� ���Crystal�豸�����Ϣ
* �����������
* ��������� ȫ��
* �� �� ֵ��  0      -   ִ�гɹ�
*             ��0    -   ִ��ʧ��
* ����˵������
***********************************************************************/
SWORD32 GetDiagCrystalInfo(T_DiagCrystalInfo *ptDiagCrystalInfo);

/**********************************************************************
* ���������� ������໷(ʱ�����)�����Ϣ
* �����������
* ��������� ȫ��
* �� �� ֵ��  0      -   ִ�гɹ�
*             ��0    -   ִ��ʧ��
* ����˵������
***********************************************************************/
SWORD32 GetDiagClockInfo(T_DiagClockInfo *ptDiagClockInfo);

/**********************************************************************
* ������������ȡ����豸��¼
* �����������������־--sdwCtrlFlg, �������ID--sdwDiagDeviceId
* �����������϶�����Ŀ--sdwDeviceNum, ��϶����¼--tDiagSingleDeviceRecord[2]
* �� �� ֵ��  0      -   ִ�гɹ�
*             ��0    -   ִ��ʧ��
* ����˵������
***********************************************************************/
SWORD32 GetDiagDeviceRecord(T_DiagDeviceRecord *ptDiagDeviceRecord);

/**********************************************************************
* ������������ȡPM8001�����Ϣ
* �����������������־--sdwCtrlFlg
* ������������PM8001��Ŀ--sdwPmNum, �����Ϣ�б�--tDiagSinglePmInfo[2]
* �� �� ֵ��  0 - ִ�гɹ�
*           ��0 - ִ��ʧ��
* ����˵������
***********************************************************************/
SWORD32 GetDiagPmInfo(T_DiagPmInfo *ptDiagPmInfo);

/**********************************************************************
* ������������ȡPCIE�����Ϣ
* �����������������־--sdwCtrlFlg
* ������������PCIE��Ŀ--sdwPcieNum, �����Ϣ�б�--tDiagSinglePcieInfo[2]
* �� �� ֵ��  0 - ִ�гɹ�
*           ��0 - ִ��ʧ��
* ����˵������
***********************************************************************/
SWORD32 GetDiagPcieInfo(T_DiagPcieInfo *ptDiagPcieInfo);

/**********************************************************************
* �������ƣ� SetLogMailCfg
* ���������� ������־�澯�ʼ��������
* ��������� ��־�澯�ʼ����ýṹ��
* ���������
* �� �� ֵ��  0      -   ִ�гɹ�
*             ��0    -   ִ��ʧ��
* ����˵������
* �޸�����        �汾��     �޸���       �޸�����
***********************************************************************/
SWORD32 SetLogMailCfg(T_LogAlarmMailCfgPack *ptLogMailCfg);

/**********************************************************************
* �������ƣ� SetLogMailCfg
* ���������� ������־�澯�ʼ��������
* ��������� ��־���ýṹ��
* ���������
* �� �� ֵ��  0      -   ִ�гɹ�
*             ��0    -   ִ��ʧ��
* ����˵������
***********************************************************************/
SWORD32 GetLogMailCfg(T_LogAlarmMailCfgPack *ptLogMailCfg);

/**********************************************************************
* �������ƣ� GetMailSwitch
* ���������� ��ȡ��ǰ���ʼ�����ʹ������
* ���������
* ��������� ʹ�ܿ���ֵָ��
* �� �� ֵ��  0      -   ִ�гɹ�
*             ��0    -   ִ��ʧ��
* ����˵������
***********************************************************************/
SWORD32 GetMailSwitch(BYTE *pucMailEnable);

/**********************************************************************
* �������ƣ� SetMailSwitch
* ���������� ���õ�ǰ���ʼ�����ʹ������
* ��������� ʹ�ܿ���ֵ
* ���������
* �� �� ֵ��  0      -   ִ�гɹ�
*             ��0    -   ִ��ʧ��
* ����˵������
***********************************************************************/
SWORD32 SetMailSwitch(BYTE ucMailEnable);

/**********************************************************************
* �������ƣ� GetMailConf
* ���������� ��ȡ�����ʼ�������ѡ��
* ��������� ��
* ��������� �ʼ������������
* �� �� ֵ��  0      -   ִ�гɹ�
*             ��0    -   ִ��ʧ��
* ����˵������
***********************************************************************/
SWORD32 GetMailConf (T_MailConfPack  *ptMailConf);

/**********************************************************************
* �������ƣ� SetMailConf
* ���������� �����ʼ�������ѡ��
* ��������� �ʼ������������
* ���������
* �� �� ֵ��  0      -   ִ�гɹ�
*             ��0    -   ִ��ʧ��
* ����˵������
***********************************************************************/
SWORD32 SetMailConf (T_MailConfPack  *ptMailConf);

/**********************************************************************
* �������ƣ� scsVerifyMailTest
* ���������� ������֤�������õĲ����ʼ�
* ��������� �����ʼ���Ŀ�ĵ�ַ���ǳ���ΪMAX_MAILTO_LEN�ַ��������ָ�룩
* ���������
* �� �� ֵ��  0      -   ִ�гɹ�
*             ��0    -   ִ��ʧ��
* ����˵������
***********************************************************************/
SWORD32 VerifyMailTest(T_VerifyMailTestTo *pcTo);

/**********************************************************************
* ��������������targetId ,��ȡInitiator��Ϣ�б�
* ���������targetId
* ���������
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetIscsiInitList(T_IscsiIniInfoList *ptIscsiIniInfoList);

/**********************************************************************
* ��������������targetId ,��ȡ����Session ��Ϣ�б�
* ���������targetId
* ���������
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetIscsiAllSessInfoList(T_IscsiSessInfoList *ptIscsiSessList);

/**********************************************************************
* ��������������targetId ,��ȡ����Session ��Ϣ�б�
* ���������targetId
* ���������
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetIscsiHistorySessInfoList(T_IscsiSessInfoList *ptIscsiHistorySessList);

/**********************************************************************
* ��������������Session Id ,��ȡ����Session������Ϣ
* ���������Session Id
* ���������
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetIscsiSessionAttr(T_GetIscsiSessAttrInfo *ptIscsiSessAttr);

/**********************************************************************
* ��������������targetId,ͳ��ָ���˿�/���ж˿ڵ�������Ϣ
* ���������targetId��Port Id
* ���������
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetIscsiFlowInfoStat(T_IscsiFlowStatInfo *ptIscsiPortStatInfo);

/**********************************************************************
* ��������������targetId,��ȡTarget Name
* ���������targetId
* ���������
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetIscsiTargetName(T_IscsiTargetInfo *ptIscsiTgtInfo);

/**********************************************************************
* ��������������targetId,��ȡ����ͳ�ƿ���״̬
* ���������targetId
* ���������
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetIscsiFlowStatSwitchStatus(T_IscsiFlowStatFlag *ptIscsiTgtInfo);

/**********************************************************************
* ��������������targetId,��������ͳ�ƿ���״̬
* ���������targetId
* ���������
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 SetIscsiFlowStatSwitchStatus(T_IscsiFlowStatFlag*ptIscsiTgtInfo);


/**********************************************************************
* ��������������targetId, ��ȡ�˿���Ϣ�б�
* ���������targetId
* ���������
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetIscsiPortInfoList(T_IscsiPortalList *ptIscsiPortInfo);

/**********************************************************************
* ������������ȡ��չ������
* ���������
* ���������
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetEnclosureType(T_EnclosureType *ptEnclosureTyp);

/**********************************************************************
* ������������ȡExpander���˽ṹ��Ϣ�б�
* ���������
* ���������
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetExpanderTopoList(T_EncTopoInfoList * ptEncTopoInfoList);

/**********************************************************************
* �������������Expander�豸�б�
* ���������
* ���������
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 DiagExpanderList(T_EncDeviceInfoList * ptEncDevInfoList);

/**********************************************************************
* ��������������ϵͳ������ص���Ϣ
* ���������
* ���������
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 SetSystemNetCfg(T_SystemNetCfg *ptSystemNetCfg);

/**********************************************************************
* ������������ѯϵͳ������ص���Ϣ
* ���������
* ���������
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetSystemNetCfg(T_SystemNetCfgList *ptSystemNetCfgList);


/**********************************************************************
* ��������������Iscsi �˿�Mac ��ַ
* ���������
* ���������
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 SetIscsiPortMac(T_IscsiPortMac *ptIscsiPortMac);

/**********************************************************************
* ������������ѯIscsi �˿�Mac ��ַ�б�
* ���������
* ���������
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetIscsiPortMac(T_IscsiPortMacList *ptIscsiPortMacList);

/**********************************************************************
* ��������������Iscsi �˿���Ϣ
* ���������������ID���˿ںš�MAC��MTU
* �����������
* ����ֵ��    0   - SUCCESS   < 0     -  FAILURE
* ����˵���������ð����˿�MAC��MTU,���Զ���ͬʱ���ã�Ҳ��������һ
***********************************************************************/
SWORD32 SetIscsiPortInfo(T_IscsiPortSet *ptIscsiPortSet);

/**********************************************************************
* ������������ѯIscsi �˿���Ϣ�б�
* �����������
* ���������ȫ��Ϊ����
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵��������Ϣ�����˿�MAC��MTU���˿�״̬��Ϣ
***********************************************************************/
SWORD32 GetIscsiPortInfo(T_IscsiPortInfoList *ptIscsiPortInfoList);

/**********************************************************************
* �������������Iscsi �˿�·����Ϣ
* ���������������ID���˿ںš�Ŀ��IP�����롢��һ��IP
* �����������
* ����ֵ��    0   - SUCCESS   <0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 AddIscsiPortRoute(T_IscsiPortRoute *ptIscsiPortRoute);

/**********************************************************************
* ����������ɾ��Iscsi �˿�·����Ϣ
* ���������������ID��Ŀ��IP������
* �����������
* ����ֵ��    0   - SUCCESS   <0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 DelIscsiPortRoute(T_IscsiPortRouteDel *ptIscsiPortRouteDel);

/**********************************************************************
* ������������ѯIscsi �˿�·����Ϣ
* ���������������ID
* ���������·�����ݡ�·����Ϣ�б�
* ����ֵ��    0   - SUCCESS   <0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetIscsiPortRoute(T_IscsiPortRouteList *ptIscsiPortRouteList);

/**********************************************************************
* ����������Iscsi �˿�ping ����
* ���������Ŀ��IP
* ���������
* ����ֵ��    0   - SUCCESS   <0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetPingInfo(T_IscsiPingInfo *ptIscsiPingInfo);

/**********************************************************************
* �����������޸�Iscsi Name
* ���������
* ���������
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 SetIscsiName(T_SetIscsiName *ptSetIscsiName);

/**********************************************************************
* ��������������Iscsi����
* ���������
* ���������
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 RestartIscsiService(SWORD32 *psdwCtrlId);

/**********************************************************************
* ���������� ��ȡ�汾Ŀ¼ʣ��ռ��С
* ��������� cFileName
* ��������� dwLocalFreeSize��dwPeerFreeSize
* �� �� ֵ�� 0         -   �ռ乻��
*            ������    -   �ռ䲻��
* ����˵����
***********************************************************************/
SWORD32 GetFreeSpace(T_VerFreeSpace *ptVerFreeSpace);
/**********************************************************************
* �������ƣ� UpdatePreProcess
* ���������� ��������汾ǰ��Ԥ����
* ��������� ucFileType
* ��������� ��
* �� �� ֵ�� 0  -   �ɹ�  ��0  -  ʧ��
* ����˵����
***********************************************************************/
SWORD32 UpdatePreProcess(BYTE ucFileType);

/**********************************************************************
* ���������� �汾�ļ�У��
* ��������� ptVerFileCheck
* ��������� У����
* �� �� ֵ�� 0      -   ִ�гɹ�
*            ��0    -   ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 VerFileCheck(T_VerFileCheck *ptVerFileCheck);

/**********************************************************************
* �����������ϴ��ļ�Ԥ����
* ���������
* ���������
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵������
***********************************************************************/
SWORD32 UploadPreProcess(T_UploadPreInfo *ptUploadPreInfo);

/**********************************************************************
* ����������ͨ��Ftp ��ʽ�ϴ��ļ�
* ���������
* ���������
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵������
***********************************************************************/
SWORD32 UploadFileByFtp(T_UploadFileInfo *ptUploadFileInfo);

/**********************************************************************
* �������ƣ� DownloadFileByFtp
* ���������� �������ļ���FTP�ͻ��˴��ݵ�FTP��������
* ���ʵı�
* �޸ĵı�
* ���������
* ���������
* �� �� ֵ��  0   - ִ�гɹ�
*             ��0 - ִ��ʧ��
* ����˵������
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2011/8/16       V1.0       ���־�             ���δ���
***********************************************************************/
SWORD32 DownloadFileByFtp(T_DownloadFileInfo *ptUploadFileInfo);

/**********************************************************************
* ������������ȡ�ļ��ϴ�����
* ���������
* ���������
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵������
***********************************************************************/

SWORD32 GetFileUploadProgress(T_UploadFileProgress *ptUploadFileProgress);
/**********************************************************************
* ������������ȡ�ļ����ؽ���
* ���������
* ���������
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵������
***********************************************************************/
SWORD32 GetFileDownloadProgress(T_DownloadFileProgress *ptDownloadFileProgress);

/***************************************************************
* ����������ɾ���汾Ŀ¼��ͬ�����ļ�
* ���������T_UploadFileName
* �����������
* �� �� ֵ: 0--�ɹ�����0--ʧ��
* ����˵����
***************************************************************/
SWORD32 DelDuplicate(T_UploadFileName *ptFileName);

/**********************************************************************
* ����������ȡ���ļ��ϴ�
* ���������
* ���������
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵������
***********************************************************************/
SWORD32 CancelUploadFile(T_UploadFileName *ptFileName);
/**********************************************************************
* �����������ж��ļ��Ĵ�����
* ���������cFileName--�ļ���
* ���������ucExistType---��������
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵������
***********************************************************************/
SWORD32 IsFileDualExist(T_IsFileExist *ptIsFileExist);
/**********************************************************************
* ������������ȡϵͳʱ���Լ�ʱ������
* ���������
* ���������
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵������
***********************************************************************/
SWORD32 GetTimeInfo(T_TimeInfo *ptSysTimeInfo);

/**********************************************************************
* ������������ȡʱ�������б�
* ���������
* ���������
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵������
***********************************************************************/
SWORD32 GetTimeZoneCityList(T_ZoneCityList *ptZoneCityList);

/**********************************************************************
* ��������������ϵͳʱ��
* ���������
* ���������
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵������
***********************************************************************/
SWORD32 SetSysTime(T_SysTime *ptSysTime);

/**********************************************************************
* ��������������ʱ��
* ���������
* ���������
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵������
***********************************************************************/
SWORD32 SetTimeZone(T_TimeZoneCityName *ptZoneCity);

/**********************************************************************
* ������������ȡNTP����������
* ���������
* ���������
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵������
***********************************************************************/
SWORD32 GetNTPServerCfg(T_NtpInfo *ptNtpInfo);

/**********************************************************************
* ��������������NTP����������
* ���������
* ���������
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵������
***********************************************************************/
SWORD32 SetNTPServerCfg(T_NtpInfo *ptNtpInfo);

/**********************************************************************
* ����������ͬ��ϵͳʱ��
* ���������
* ���������
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵������
***********************************************************************/
SWORD32 SynchronizeSysTimeByNtp(T_NtpSync *ptNtpSyn);

/**********************************************************************
* ������������ȡ���к�̨������Ϣ
* �����������
* ���������ȫ�ǳ���
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵������
***********************************************************************/
SWORD32 GetTaskInfo(T_TaskInfoList *ptTaskInfoList);

/**********************************************************************
* ������������ͣ/ ����/ ɾ����̨����
* ���������sdwTaskId ��sdwTaskCtrl
* �����������
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵������
***********************************************************************/
SWORD32 SetTask(T_TaskCtrl *PtTaskCtrl );

/**********************************************************************
* ������������Ӻ�̨����
* ���������
* �����������
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵������
***********************************************************************/
SWORD32 AddTask(T_TaskAddInfo *ptTaskAddInfo);

/**********************************************************************
* ������������ȡ���мƻ�������Ϣ
* �����������
* ���������ȫ�ǳ���
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵������
***********************************************************************/
SWORD32 GetScheduleInfo(T_ScheduleList *ptScheduleList);

/**********************************************************************
* ���������� ɾ���ƻ�����
* ���������sdwTaskId
* �����������
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵������
***********************************************************************/
SWORD32 DelSchedule(T_ScheduleDel *ptScheduleDel);

/**********************************************************************
* �����������û���¼
* ���������ptUser->acUsername  ptUser->adPassword
* �����������
* �� �� ֵ��   0      -   ִ�гɹ�
*                                ��0 - ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32  Login(T_AmsUser *ptUser);

/**********************************************************************
* �����������û��ǳ�
* �����������
* �����������
* �� �� ֵ��   0    -   ִ�гɹ�
*                ��0 - ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 Logout(void);

/**********************************************************************
* ��������������session
* ���������ptSession->sdwSessionId
* ���������
* �� �� ֵ��   0      -   ִ�гɹ�
*                                ��0 - ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 CreateSession(T_SessionId * ptSession);

/**********************************************************************
* ��������������session
* ���������ptSession->sdwSessionId
* �����������
* �� �� ֵ��   0    -   ִ�гɹ�
*                ��0 - ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 EnterShareSession(T_SessionId * ptSession);

/**********************************************************************
* �����������˳�session
* ���������ptSession->sdwSessionId
* �����������
* �� �� ֵ��   0      -   ִ�гɹ�
*                                ��0 - ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32  ExitShareSession(void);

/**********************************************************************
* ����������ɾ��session
* ���������ptSession->sdwSessionId
* �����������
* �� �� ֵ��   0    -   ִ�гɹ�
*                ��0 - ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 DestroySession(T_SessionId * ptSession);

/**********************************************************************
* ���������������û�
* ���������ptUser->acUsername  ptUser->adPassword  ptUser->sdwUserType
* �����������
* �� �� ֵ��   0      -   ִ�гɹ�
*                                ��0 - ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 CreateAccount(T_AmsUser * ptUser);

/**********************************************************************
* ����������ɾ���û�
* ���������ptAccount->acUsername
* �����������
* �� �� ֵ��   0    -   ִ�гɹ�
*                ��0 - ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 DelAccount (T_Account * ptAccount);

/**********************************************************************
* ������������ȡ�û��б�
* �����������
* ���������ptAccountList
* �� �� ֵ��   0      -   ִ�гɹ�
*                                ��0 - ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 GetAccountList(T_AccountList * ptAccountList);

/**********************************************************************
* ������������ȡ��¼����Ϣ
* �����������
* ���������ptAccountList
* �� �� ֵ��   0      -   ִ�гɹ�
*                                ��0 - ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 GetLoginedUser(T_AccountList * ptAccountList);

/**********************************************************************
* �����������޸�����
* ���������ptPasswdSet->acUsername  ptPasswdSet->acOldPwd  ptPasswdSet->acNewPwd
* �����������
* �� �� ֵ��   0    -   ִ�гɹ�
*                ��0 - ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 SetPasswd (T_PwdSet *ptPasswdSet);

/**********************************************************************
* �����������޸�����
* ���������ptUserNameMody->acUsername   ptUserNameMody->acUserNewname
* �����������
* �� �� ֵ��   0    -   ִ�гɹ�
*                ��0 - ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 ModifyUserName (T_UserNameMody *ptUserNameMody);

/**********************************************************************
* ������������ȡ�û�������������
* ���������ptPwdMail->acUsername
* �����������
* �� �� ֵ��   0      -   ִ�гɹ�
*                                ��0 - ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 GetPwdMail (T_PwdMail * ptPwdMail);

/**********************************************************************
* ���������������û�������������
* ���������ptPwdMail->acUsername ptPwdMail->acPwdMail
* �����������
* �� �� ֵ��   0    -   ִ�гɹ�
*                ��0 - ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 SetPwdMail (T_PwdMail * ptPwdMail);

/**********************************************************************
* �����������������������ʼ�
* ���������ptPwdMail->acUsername
* �����������
* �� �� ֵ��   0      -   ִ�гɹ�
*                                ��0 - ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 SendPwdMail (T_PwdMail * ptPwdMail);

/**********************************************************************
* ����������ˢ��SMA�ϵĳ�ʱʱ�䣬���¼���UI��ʱ
* ���������Null
* ���������
* �� �� ֵ��  NOERR, execute successfully
* ����˵����
***********************************************************************/
SWORD32 RefreshConTime(void);
/**********************************************************************
* ��������������web������
* ���������WebCheckSidExistOrNot
* ���������
* �� �� ֵ��  NOERR, execute successfully
                               -E_SMA_SESSION_NEXIST,sid������
* ����˵����
***********************************************************************/
SWORD32 WebCheckSidExistOrNot(T_WebSessionIdExist * ptSession);


/**********************************************************************
* ������������ȡϵͳ��Ϣ
* ���������T_SysInfo
* ���������
* �� �� ֵ��  0      -   ִ�гɹ�
*                                ��0 - ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 GetSysInfo(T_SysInfo *ptSysInfo);

/**********************************************************************
* �����������޸�ϵͳ����
* ���������T_SysNameInfo
* ���������
* �� �� ֵ�� 0      -   ִ�гɹ�
*                                ��0 - ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 SetSysName(T_SysNameInfo *ptSysName);

/**********************************************************************
* �����������鿴��������ϸ��Ϣ
* ���������T_CtrlInfo
* ���������
* �� �� ֵ�� 0      -   ִ�гɹ�
*                                ��0 - ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 GetCtrlInfo(T_CtrlInfo *ptCtrlInfo);

/**********************************************************************

* �����������޸ı��˿���������
* ���������T_SysNameInfo
* ���������
* �� �� ֵ�� 0      -   ִ�гɹ�
*                                ��0 - ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 SetCtrlName(T_CtrlInfo *ptCtrlInfo);

/**********************************************************************
* �����������鿴����SMART��Ϣ
* ���������T_DmSmartInfo
* ���������
* �� �� ֵ�� 0      -   ִ�гɹ�
*                                ��0 - ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 GetHddSmartInfo(T_DmSmartInfo * ptSmartInfo);
/**********************************************************************
* ��������������SMARTɨ��
* ���������T_DmSmartTestCtl
* ���������
* �� �� ֵ�� 0      -   ִ�гɹ�
*                                ��0 - ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 ExecHddSmartTest(T_DmSmartTestCtl *ptSmartTest);
/**********************************************************************
* �������������ô��̽��ܼ��ſ���
* ���������T_DmPowerStatSwitch
* ���������
* �� �� ֵ�� 0      -   ִ�гɹ�
*                                ��0 - ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 SetHddPowerSwitchStatus(T_DmPowerStatSwitch *ptPowSwitch);
/**********************************************************************
* ������������ȡ���̽��ܼ��ſ���
* ���������T_DmPowerStatSwitch
* ���������
* �� �� ֵ�� 0      -   ִ�гɹ�
*                                ��0 - ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 GetHddPowerSwitchStatus(T_DmPowerStatSwitch *ptPowSwitch);
/**********************************************************************
* ����������ͬ���ļ����Զ�
* ����������ļ�����(����·��)
* ���������
* �� �� ֵ�� 0      -   ִ�гɹ�
*                                ��0 - ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 SyncFileToPeer(T_FileToPeer *ptFileToPeer);
/**********************************************************************
* ����������ͬ���ļ�(���˶�����ͬ��)
* ����������ļ�����(����·��)
* ���������
* �� �� ֵ�� 0      -   ִ�гɹ�
*                                ��0 - ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 SyncFileToDual(T_FileToPeer *ptFileToPeer);
/**********************************************************************
* ������������ȡϵͳ�������ʱ��
* �����������
* ���������ptSysBootTime->tSysSingleBootTime[2]
* �� �� ֵ�� 0 - ִ�гɹ�
*          ��0 - ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 GetSysBootTime(T_SysBootTime *ptSysBootTime);
/**********************************************************************/

#endif

#endif
