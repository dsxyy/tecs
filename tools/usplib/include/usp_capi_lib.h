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

#ifndef __USP_CAPI_LIB_H__
#define __USP_CAPI_LIB_H__

#include "usp_common_macro.h"
#include "usp_common_typedef.h"

typedef struct
{
    SWORD32 sdwSvrPort;                 /*�洢�豸�����Ķ˿ں�*/
    CHAR    ucSvrIpAddr[MAX_IP_LEN];    /*�洢�豸IP   ��ַ*/
}_PACKED_1_ T_SvrSocket;


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

/******************************** SIC *****************************************/

/* ѹ������Ϣ */
typedef struct
{
    T_Time tCreatTime;    /* ���Σ�ѹ����������ʱ�� */
    CHAR scTarBallName[MAX_SIC_FILE_NAME_LEN];    /* ���Σ�ѹ�������� */
}_PACKED_1_ T_TarBallInfo;

/* ÿ����������ѹ������Ϣ */
typedef struct
{
    SWORD32 sdwCtrlId;    /* ѹ�������ڵĿ�����ID */
    T_TarBallInfo tTarBallInfo;
}_PACKED_1_ T_TarBallInfoPerCtrl;

/* ϵͳ�����п������ϵ�ѹ������Ϣ */
typedef struct
{
    WORD32 dwCtrlNum;    /* ϵͳ�п����������� */
    T_TarBallInfoPerCtrl tTarBallInfoPerCtrl[MAX_CTL_NUM];    /* ѹ������Ϣ�б� */
}_PACKED_1_ T_TarBallInfoList;

/* �ʼ����͵����� */
typedef struct
{
    BYTE ucEmailEnable;    /* �ʼ�����ʹ��(����ʱΪ���, ��ȡ�ӿ���Ϊ����) */
}_PACKED_1_ T_EmailCfg;

typedef struct
{
    SWORD32 sdwCtlId;  /* ��Σ�ȡֵ�ο�T_CtrlAction�ṹ�� */
}_PACKED_1_ T_SicCtrlAction;


typedef struct
{
    CHAR    ucFileName[MAX_CONFIG_FILE_NAME_LEN]; /* ���ļ�����������·�� */
}_PACKED_1_ T_SysInfoTarBall;

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
    CHAR     ucModelName[MAX_PD_MODEL_NAME_LEN];  /* model���� */
    CHAR     ucPhysicalType[MAX_PD_TYPE_LEN];     /* Ӳ����������(e.g. SATA1, SATA2, SAS, FC) */
    CHAR     ucFwVer[MAX_PD_FW_VER_LEN];          /* Firmware�汾.*/
    CHAR     ucSerialNo[MAX_PD_SNO_LEN];          /* ���к� */
    CHAR     ucStdVerNum[MAX_PD_STD_VER_LEN];     /* ��׼�汾��. */
    SWORD32  sdwHealthState;                      /* ���̵Ľ���״̬�������ɷ����Ժ�SMART ״̬��ȡֵΪPD_GOOD(1)��PD_BROKEN(0)*/
    SWORD32  sdwSourceType;                       /* ePdSrc   eSrc;ȡֵPD_LOCAL(0), PD_FOREIGN(2)*/
    SWORD32  sdwLogicType;                        /* ePdType  eType;ȡֵPD_UNKOWN(3),PD_UNUSED(0),PD_HOTSPARE(4), PD_DATA(5)��PD_CONFLICT(6)*/
    SWORD32  ucObjectType;                        /* �����������ͣ�TYPE_VD ��ʾ vd��TYPE_POOL ��ʾ pool */
    CHAR     cVdName[MAX_BLK_NAME_LEN];           /* ���������� */
    SWORD32  sdwVisibleCtrl;                      /* �Ը��̿ɼ��Ŀ�������״̬ */
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
    //CHAR     cVolNames[MAX_VOL_NUM_ON_VD][MAX_BLK_NAME_LEN];   /* ָ��VD �ϵľ������б� */
    CHAR     cVolNames[MAX_ALLTYPE_VOL_NUM_ON_VD][MAX_BLK_NAME_LEN];   /* ָ��VD �ϵľ������б� */
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
typedef struct
{
    CHAR      cVolName[MAX_BLK_NAME_LEN]; /* Vol����,����Ϊ��*/
    SWORD32   sdwOwned;        /* ���������ȡֵ:VOL_IN_VD ---��ʾvol����VD, VOL_IN_POOL ---��ʾvol����pool */
}_PACKED_1_ T_VolAttr;

/* BLK��SCS����ͨ�øýṹ�� ���һ����Ա��SCS���*/
typedef struct
{
    CHAR      cVolName[MAX_BLK_NAME_LEN]; /* Vol����,����Ϊ��*/
    CHAR      cVdName[MAX_BLK_NAME_LEN];  /*VD���ƻ�POOL����,����Ϊ��*/
    BYTE      ucVolUuid[MAX_DEV_UUID_LEN];  /* ���UUID  */
    SWORD64   sqwCapacity;     /* vol capacity  ,KB*/
    SWORD32   sdwLevel;        /* eRaidlevel, RAID level, defined in struct eRaidlevel ,ȡֵRAID0(0),RAID1(1),RAID3(3), RAID5(5),RAID6(6),
                                                   RAIDTP(7),RAID10(10),RAID30(30),RAID50(50),RAID60(60),NRAID(200),RAID_LEVEL_INVALID(0xffff) */
    SWORD32   sdwState;        /* eVolState , vol state defined in struct eVolState,ȡֵVOL_S_GOOD(0),
                                               VOL_S_FAULT(1),  VOL_S_ERR(2),VOL_S_CHUNKSIZE_MIGRATION(3),
                                               VOL_S_EXPAND(4),VOL_S_UNREADY(5),VOL_S_INVALID(0xffff) */
    SWORD32   sdwOwned;        /* ���������ȡֵ:VOL_IN_VD ---��ʾvol����VD, VOL_IN_POOL ---��ʾvol����pool */
    SWORD32   sdwStripeSize;   /* vol chunk size ,KB*/
    SWORD32   sdwOwnerCtrl;    /* owner control ,��ʱΪ0,1 */
    SWORD32   sdwPreferCtrl;   /*  ��ѡ������,��ʱΪ0,1 */
    T_Time    tCreateTime;     /* ����ʱ��  */
    SWORD32   sdwMapState;     /* ���ӳ��״̬��ȡֵMAPPED_VOL(1),NO_MAPPED_VOL(0) */
    SWORD32   sdwCachePolicy;  /* CACHE д��д������, ȡֵCACHE_WRITE_BACK_POLICY(0),CACHE_WRITE_THROUGH_POLICY(1) */
    SWORD32   sdwAheadReadSize;/* cacheԤ��ϵ��,ȡֵ0~2048 */
    SWORD32   sdwIsRvol;       /* Դ���Ƿ��Ѿ����ڿ�����Դ�ռ� ������*/
    SWORD32   sdwSvolNum;      /* Դ���Ͽ��վ�ĸ��� ������*/
    SWORD32   sdwCloneNum;     /* Դ���Ͽ�¡��ĸ��� ������*/
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



typedef struct
{
   CHAR  cBvolName[MAX_BLK_NAME_LEN]; /*Դ������*/
   CHAR  cRvolName[MAX_BLK_NAME_LEN]; /*������Դ�ռ�����*/
   SWORD32 sdworiginPercent; /*������Դ�ռ�ռԴ��������ʼ�ٷֱ�*/
   SWORD32 sdwrvolThr;		  /*������Դ�ռ�澯��ֵ*/
   SWORD32 sdwrvolStrategy;		/*��Դ�ռ����������*/
   SWORD32 sdwautoExpand; 	  /*������Դ�ռ�ﵽ�澯���Ƿ��Զ�����*/
   SWORD32 sdwexpandPercent; /*ÿ�����ݶ��ٷֱȣ�Ĭ��10%�����ֶν�������auotExpandΪʱ����Ч*/
}_PACKED_1_ T_RvolCreate;


typedef struct
{
    CHAR  cBvolName[MAX_BLK_NAME_LEN];  /*������Դ�ռ�����*/
}T_RvolDel;


typedef struct
{
    CHAR  cBvolName[MAX_BLK_NAME_LEN]; /*������Դ�ռ�����*/
    SWORD32 sdwrvolThr;    /*�澯��ֵ*/
      SWORD32 sdwrvolStrategy; /*�������*/
    SWORD32 sdwautoExpand;  /*�Ƿ��Զ�����*/
    SWORD32 sdwexpandPercent; /*���Զ�����ʱ��ÿ�����ݰٷֱ�*/
}_PACKED_1_ T_RvolSet;


typedef struct
{
    CHAR  cRvolName[MAX_BLK_NAME_LEN]; /*������Դ�ռ�����*/
    SWORD32 sdwexpandPercent; /*���ݰٷֱ�*/
}_PACKED_1_ T_RvolExpand;


typedef struct
{
    CHAR    cRvolName[MAX_BLK_NAME_LEN]; /*������Դ�ռ�����*/
    CHAR    cBvolName[MAX_BLK_NAME_LEN]; /*Դ������*/
    SWORD64 sqwtotalCapacity;     	/*������Դ�ռ�������*/
    SWORD64 sqwusedCapacity;	/*������Դ�ռ���ʹ������*/
    SWORD32 sdwrvolThr;   /*�澯��ֵ*/
    SWORD32 sdwrvolStrategy; /*�������*/
    SWORD32 sdwautoExpand; /*�Ƿ��Զ�����*/
}_PACKED_1_ T_RvolInfo;



/* BLK��SCS����ͨ�øýṹ�� */
typedef struct
{

    CHAR     cVolName[MAX_BLK_NAME_LEN];    /* Vol����*/
    CHAR     cSvolName[MAX_BLK_NAME_LEN];   /* ���վ����� */
    SWORD32 sdwAgentAttr;                   /* 1��ʾ����մ����Ϳ���֪ͨ��0��ʾ������ */

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
    CHAR     cVdName[MAX_BLK_NAME_LEN];    /* Vd���� */
    SWORD32  sdwState;                     /* The state of the svol, the possible values are %SVOL_S_ACTIVE, %SVOL_S_FAULTY.*/
    T_Time   tCreateTime;                  /* Svol create time */
    //SWORD64  uqwCapacity;                  /* Snapshot volume size (in sectors),���վ�����,KB */
    //SWORD64  uqwUsedCapacity;              /* Used Snapshot volume size (in sectors),��ʹ�ÿ��վ����� */
    //SWORD32  sdwSvolThr;                   /* ���վ�ı�����ֵ���ٷֱȱ�ʾ*/
    //SWORD32  sdwSvolNotify;                /* Overflow treatment strategies of rvol ("fail svol" or "fail write to bvol")�����������Ĵ���ʽ��� */
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
    SWORD32    sdwSvolNum;                       /* ���վ���Ŀ*/
    T_SvolInfo tSvolInfo[MAX_SVOL_NUM_IN_SYS];   /* ���վ���Ϣ�б�*/
}_PACKED_1_ T_AllSvolInfo;

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
    WORD64 sdwCvolCapacity;               /* clone�������KB,Ҫ��С��Դ�� */
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
    WORD64 sdwCvolCapacity;   /* clone�������KB */
    SWORD32 sdwCvolChunkSize;   /* clone���chunk��λ,KB */
    SWORD32 sdwInitSync;     /* �Ƿ���г�ʼͬ��, 1-YES, 0--NO */
    SWORD32 sdwProtectRestore;   /* �Ƿ�������������, 1-YES, 0--NO */
    SWORD32 sdwPri;   /* ͬ����ͬ�������� */
    SWORD32 sdwRelation;   /* ��������Դ��Ĺ�����ϵ, 1--����, 0--���� */
    SWORD32 CvolState;   /* �������״̬ */
    SWORD32 sdwLunMapState;  /* �������ӳ��״̬��ȡֵMAPPED_VOL(1),NO_MAPPED_VOL(0) ������*/
 }_PACKED_1_ T_CvolInfo;

typedef struct
{
    CHAR    scCloneName[MAX_BLK_NAME_LEN];      /* ����������� */
    CHAR    scCloneVdName[MAX_BLK_NAME_LEN];    /* ����������vd*/
    CHAR    scBvolName[MAX_BLK_NAME_LEN];       /* Դ����*/
    CHAR    scBvolVdName[MAX_BLK_NAME_LEN];     /* Դ������vd*/
    WORD64 sdwCloneCapacity;   /* clone�������KB */
}T_CvolBaseInfo;

typedef struct
{
    SWORD32    sdwCvolNum;                       /* ��������Ŀ*/
    T_CvolBaseInfo tCvolBaseInfo[MAX_CLONE_IN_SYS];      /* ��������Ϣ�б�*/
}_PACKED_1_ T_AllCvolInfo;



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
    T_VolObjectInfo    tVolObjectList[MAX_LUN_NUM_IN_SYS];  /* ϵͳ��δӳ��ľ�������Ϣ�б�*/
}_PACKED_1_ T_VolObjectInfoList;


typedef struct
{
    SWORD32  sdwSlotId;       /* ���̲�λ�ţ�ȡֵ0~27(ScanBox);  0~11(SPR10) */
    SWORD32  sdwEnclosureId;  /* ����Id��ȡֵ 0~ʵ��������*/
    SWORD64  sqwCapacity;     /* �������� */
}_PACKED_1_  T_PdIdInfo;

typedef struct
{
    SWORD32      sdwUnusedPdNum;                    /* ��������Ŀ */
    SWORD32      sdwSasPdNum;                       /* SAS��Ա����Ŀ */
    T_PdIdInfo   tSasMemberPdId[MAX_PD_NUM_IN_VD];  /* SAS��Ա���б� */
    SWORD32      sdwSataPdNum;                      /* SATA��Ա����Ŀ */
    T_PdIdInfo   tSataMemberPdId[MAX_PD_NUM_IN_VD]; /* SATA��Ա�����б� */
}_PACKED_1_ T_AllUnusedPd;

typedef struct
{
    T_PdId       tPdId[MAX_PD_NUM_IN_VD];
    SWORD32      sdwPdNum;                  //�����˸��ֶΣ������жϣ�ʡ���ٴμ�����
    SWORD64      sqwCapacity;               /* �������� */
}_PACKED_1_ T_FindPd;

typedef struct
{
    SWORD32      sdwCheckNum;                       /* ����¼���Ĵ�����Ŀ����Ҫ����sdwPdNum�������� */
    SWORD32      sdwPdNum;                          /* ָ�����ڴ��������̵Ĵ�����Ŀ */
    T_PdIdInfo   tPdIdInfo[MAX_PD_NUM_IN_VD];       /* ����¼������Ϣ */
    T_PdIdInfo   tRightPdInfo[MAX_PD_NUM_IN_VD];    /* �ҵ������ڴ���vd�Ĵ�����Ϣ������ */
}_PACKED_1_ T_RecordPd;


/***************************************�����⻯��ؽṹ��**************************************/

typedef struct
{
    CHAR     scPoolName[MAX_BLK_NAME_LEN]; /* Pool����,����Ϊ��,���*/
    SWORD32  sdwCreateType;                /* ������ʽ��1 ��������Ŀ(POOL_CREATE_BY_DISK_NUM)��2 �������б�(POOL_CREATE_BY_DISK_LIST)*/
    SWORD32  sdwPdNum;                     /* ������Ŀ�����256�����*/
    T_PdId   tPdId[MAX_DISKS_IN_POOL];     /* ������Ϣ�б� ����sdwCreateType ΪPOOL_CREATE_BY_DISK_LISTʱ��Ч����ѡ��� */
    SWORD32  sdwAlarmThreshold;            /* �����澯��ֵ ���ٷֱ�ֵ������ֵΪ5����ʾʣ������Ϊ5%ʱ�澯�����*/
    SWORD32  sdwCapacityPolicy;            /* �ռ䱣������ ,ȡֵ��POOL_CAPACITY_POLITY_NO_RESERVE��POOL_CAPACITY_POLITY_RESERVE_ONE��
                                              POOL_CAPACITY_POLITY_RESERVE_TWO��Ĭ��ȡֵΪPOOL_CAPACITY_POLITY_RESERVE_ONE����� */
}_PACKED_1_ T_CreatePool;


typedef struct
{
    CHAR    scPoolName[MAX_BLK_NAME_LEN];  /* Pool ����,����Ϊ�գ����*/
}_PACKED_1_ T_PoolName;

typedef struct
{
    CHAR     scPoolName[MAX_BLK_NAME_LEN];  /* Pool ����,����Ϊ��*/
    CHAR     scVolName[MAX_BLK_NAME_LEN];   /* Vol���ƣ�����Ϊ��*/
    SWORD32  sdwRaidlevel;                  /* raid level */
    SWORD32  sdwChunkSize;                  /* new chunk size,��Ϊ4�ı�������λ:KB�� ȡֵMIN_CHUNK_SIZE(4)~MAX_CHUNK_SIZE(512 )*/
    WORD64   qwCapacity;                    /* ������, ��λ KB �����*/
    SWORD32  sdwCtrlPrefer;                 /* ��ѡ������,��ʱΪ0,1 */
    //SWORD32  sdwAutoRebuild;                /* �Զ��ؽ����� ��ON(1)/OFF(0)�����*/
    SWORD32  sdwCachePolicy;                /* Cacheд��д������, ȡֵCACHE_WRITE_BACK_POLICY(0),CACHE_WRITE_THROUGH_POLICY(1) */
    SWORD32  sdwAheadReadSize;              /* CacheԤ��ϵ��,ȡֵ0~2048 ���޵�λ*/
}_PACKED_1_ T_CreateVolOnPool;

typedef struct
{
    CHAR     scPoolName[MAX_BLK_NAME_LEN];       /* Pool ����,����Ϊ��, ���*/
    WORD32   dwstate;                            /* Pool��״̬��POOL_DAMAGED��POOL_GOOD��POOL_REDISTRIBUTING��POOL_INSUFCAPACITY */
    SWORD32  sdwAllPdNum;                        /* ��Ա����Ŀ*/
    T_PdId   tAllMemberPdId[MAX_DISKS_IN_POOL];  /* ��Ա���б�*/
    SWORD32  sdwFaultPdNum;                      /* ��Ա���й�������Ŀ*/
    T_PdId   tFaultMemberPdId[MAX_DISKS_IN_POOL];/* ���ϳ�Ա�����б�*/
    WORD64   qwTotalCapacity;                    /* �洢��������,������С����MBΪ��λ,��ͬ */
    WORD64   qwUsedCapacity;                     /* �洢����ʹ������ */
    WORD64   qwFreeCapacity;                     /* �洢�ؿ��������������������ռ� */
    WORD64   qwAlarmThreshold;                   /* �����澯��ֵ */
    SWORD32  sdwCapacityPolicy;                  /* �ռ䱣�����ԣ�ȡֵ��POOL_CAPACITY_POLITY_NO_RESERVE(�ޱ����ռ������ؽ�)��
                                                                       POOL_CAPACITY_POLITY_RESERVE_ONE(����һ����̵Ŀռ������ؽ�)��
                                                                       POOL_CAPACITY_POLITY_RESERVE_TWO(����������̵Ŀռ������ؽ�) */
    T_Time   tCreateTime;                        /* Pool ����ʱ�� */
}_PACKED_1_ T_PoolInfo;

typedef struct
{
    SWORD32 sdwPoolNum;                                          /* ϵͳ�д洢������ ������*/
    CHAR    sdwPoolNames[MAX_POOL_NUM_IN_SYS][MAX_BLK_NAME_LEN]; /* �洢�������б� ������*/
}_PACKED_1_ T_AllPoolNames;

typedef struct
{
    CHAR     scPoolName[MAX_BLK_NAME_LEN];  /* Pool����,����Ϊ�գ����*/
    SWORD32  sdwVolType;                    /* �����ͣ�������ͨ��NORM_VOL�������վ�SNAPSHOT_VOL����������CLONE_VOL��,��� */
    SWORD32  sdwVolNum;                     /* �����Ŀ */
    CHAR     scVolNames[MAX_VOL_NUM_ON_POOL][MAX_BLK_NAME_LEN];   /* ָ��Pool�ϵľ������б� */
}_PACKED_1_ T_VolNamesOnPool;

typedef struct
{
    CHAR    scPoolName[MAX_BLK_NAME_LEN];   /* Pool���� */
    CHAR    scNewName[MAX_BLK_NAME_LEN];    /* �洢�������ƣ�������������֣�����մ���ʾ��ֵ��Ч */
    SWORD32 sdwAlarmThreshold;              /* �����澯��ֵ���ٷֱ�ֵ���粻��Ҫ�޸ģ���-1��ʾ��ֵ��Ч */
    SWORD32 sdwCapacityPolicy;              /* �ռ䱣�����ԣ��粻��Ҫ�޸ģ���-1��ʾ��ֵ��Ч��ֵ��Ч */
}_PACKED_1_ T_ModifyPool;

typedef struct
{
    CHAR    scPoolName[MAX_BLK_NAME_LEN];   /*  Pool���� */
    SWORD32 sdwModifyType;                  /*  �޸ķ�ʽ��1 ��������Ŀ(POOL_MODIFY_DISK_BY_NUM)��2 �������б�(POOL_MODIFY_DISK_BY_LIST)*/
    SWORD32 sdwPdNum;                       /*  ���̸��� ,���*/
    T_PdId  tPdId[MAX_DISKS_IN_POOL];       /*  �����б����256 ������������б���ֵ��Ч�����ݡ�sdwModifyType����ѡ���*/
}_PACKED_1_ T_ModifyDiskOnPool;

typedef struct
{
    CHAR     scVolName[MAX_BLK_NAME_LEN]; /* Vol���ƣ�����Ϊ�գ����*/
    WORD64   qwExpandCapacity;            /* �����������ֵ����λKB �����*/
}_PACKED_1_ T_ExpVolCapacity;

/***************************************�����⻯�ṹ�嶨�����**************************************/

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

typedef struct
{
    CHAR     cVdName[MAX_BLK_NAME_LEN];         /* Vd ����,����Ϊ�� */
    SWORD32  sdwPdNum;                          /* ���32 */
    SWORD32  sdwVdRaidLevel;                    /* Vd��raid���� */
    SWORD32  sdwAutoRebuild;                    /* �Զ��ؽ����� ON�Զ��ؽ���OFF���Զ��ؽ� */
}_PACKED_1_ T_AutoCreateVd;

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



/***************************************Task**************************************/
/* ��̨�������*/
typedef struct
{
    SWORD32    sdwTaskId;                              /*����ID*/
    CHAR       cObjectName[MAX_TASK_OBJECT_NAME_LEN];  /*��������������*/
    WORD32     sdwTaskType;                            /* ��������,ȡֵ���궨��*/
    SWORD32    sdwTaskState;                           /* ����״̬��ȡֵTASK_RUNNING��TASK_PAUSE��TASK_DELAY*/
    T_Time     tStartTime;                             /* ��ʼʱ�䣬�ꡢ�¡��ա�ʱ���֡���*/
    SWORD32    sdwTaskProgress;                        /* ������ȣ�����������%*/
    SWORD32    sdwTaskAttr;                            /* �������ԣ���bitλ��ʾ����λ(1��ʾ������Ч)��Ч������b0:��ɾ�����ԣ�b1:����ͣ���ԣ�b2:��ͣ��Ŀɼ�����������*/   
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
    CHAR        cObjectName[MAX_TASK_OBJECT_NAME_LEN];      /* ��������������,input */
    WORD32      sdwTaskType;                           /* ��������,ȡֵ���궨��,input */
    SWORD32     sdwPrarmLen;                           /* �����������,input */
    CHAR        ucTaskPrarm[MAX_TASK_PRARM_LEN];       /* �����������,input */
}_PACKED_1_ T_TaskAddInfo;

/* �ƻ��������*/

typedef struct
{
    SWORD32 swTaskId;      //�ƻ�����id��
    SWORD32 scTaskType;    //�������ͣ��ο�eSchdTaskType����
    CHAR    cObjectName[MAX_SCHEDULE_OBJECT_NAME_LEN];   //�ƻ����������Ķ�������
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



/***************************************Mirror**************************************/
#define MAX_UUID_LEN MAX_DEV_UUID_LEN+1
/* ����Զ�̾��� */
typedef struct
{
    CHAR    scMirrorName[MAX_BLK_NAME_LEN];                      /* �������� �����*/
    CHAR    scSrcVolName[MAX_BLK_NAME_LEN];                      /* ����Դ������ �����*/
    CHAR    ucRmtDiskUuid[MAX_UUID_LEN];                     /* ����Ŀ�ľ�UUID,��Զ�̴���UUID �����*/
    SWORD32 sdwMirrorType;                                       /* ���������,ȡֵ��MIRROR_SYNCHRONOUS��MIRROR_ASYNCHRONOUS  �����*/
    SWORD32 sdwIsInitSync;                                       /* ��ʼͬ�����,ȡֵ��MIRROR_NOT_INIT_SYNC��MIRROR_IS_INIT_SYNC �����*/
    SWORD32 sdwPolicy;                                           /* ����Ļָ�����,ȡֵ��MIRROR_MANUAL_POLICY��MIRROR_AUTO_POLICY�����*/
    SWORD32 sdwPriority;                                         /* ͬ�������ȼ�,ȡֵ��SYNC_LOW��SYNC_MEDIUM��SYNC_HIGH�����*/
    SWORD32 sdwPeriod;                                           /* �첽�����ͬ��Ƶ��,����������ΪMIRROR_ASYNCHRONOUSʱ��Ч,��λ:s, ȡֵ5~300 ����ѡ���*/
}_PACKED_1_ T_CreateMirror;

/* Զ�̾������� */
typedef struct
{
    CHAR  scMirrorName[MAX_BLK_NAME_LEN];  /* �������� ,���*/
}_PACKED_1_ T_MirrorName;

/* Զ�̾�����Ϣ */
typedef struct
{
    CHAR    scMirrorName[MAX_BLK_NAME_LEN];                      /* �������� �����*/
    CHAR    scSrcVolName[MAX_BLK_NAME_LEN];                      /* ����Դ������ */
    CHAR    ucRmtDiskUuid[MAX_UUID_LEN];                     /* ����Ŀ�ľ�UUID,��Զ�̴���UUID �����*/
    SWORD64 sdwDestVolCap;                                       /* ����Ŀ�ľ������ KB */
    SWORD32 sdwMirrorType;                                       /* ���������,ȡֵ��MIRROR_SYNCHRONOUS��MIRROR_ASYNCHRONOUS  */
    SWORD32 sdwInitSync;                                         /* ��ʼͬ�����,ȡֵ��MIRROR_NOT_INIT_SYNC��MIRROR_IS_INIT_SYNC */
    SWORD32 sdwPolicy;                                           /* ����Ļָ�����,ȡֵ��MIRROR_MANUAL_POLICY��MIRROR_AUTO_POLICY*/
    SWORD32 sdwPriority;                                         /* ͬ�������ȼ�: ��SYNC_LOW��SYNC_MEDIUM��SYNC_HIGH*/
    SWORD32 sdwPeriod;                                           /* �첽�����ͬ��Ƶ�ʣ���λ:s, ȡֵ5~300 */
    SWORD32 sdwRelation;                                         /* �����ϵ, ȡֵ��FRACTURED��UNFRACTURED*/
    SWORD32 sdwState;                                            /* ����״̬, ȡֵ��INIT ~ OUT_OF_INIT*/
}_PACKED_1_ T_MirrorInfo;

/* ���о��������б� */
typedef struct
{
    SWORD32      sdwMirrorNum;                                     /* ������Ŀ */
    T_MirrorName tMirrorName[MAX_MIRROR_IN_SYS];                   /* ���������б� */
}_PACKED_1_ T_AllMirrorNames;

/* �޸�Զ�̾�������  */
typedef struct
{
    CHAR    scMirrorName[MAX_BLK_NAME_LEN];        /* �������ƣ����*/
    CHAR    scMirrorNewName[MAX_BLK_NAME_LEN];     /* ���������� �����*/
}_PACKED_1_ T_MirrorRename;

/* �޸�Զ�̾���ָ����� */
typedef struct
{
    CHAR    scMirrorName[MAX_BLK_NAME_LEN];     /* �������� �����*/
    SWORD32 sdwPolicy;                          /* ����Ļָ�����,ȡֵ��MIRROR_MANUAL_POLICY��MIRROR_AUTO_POLICY�������*/
}_PACKED_1_ T_MirrorPolicy;

/* �޸�Զ�̾���ͬ�����ȼ�  */
typedef struct
{
    CHAR    scMirrorName[MAX_BLK_NAME_LEN];     /* �������� �����*/
    SWORD32 sdwPriority;                        /* ͬ�������ȼ�: ��SYNC_LOW��SYNC_MEDIUM��SYNC_HIGH�����*/
}_PACKED_1_ T_MirrorPriority;

/* �޸�Զ�̾���ͬ��Ƶ��(���첽����) */
typedef struct
{
    CHAR    scMirrorName[MAX_BLK_NAME_LEN];  /* �������� �����*/
    SWORD32 sdwPeriod;                       /* �첽�����ͬ��Ƶ�ʣ���λ:s, ȡֵ5~300����� */
}_PACKED_1_ T_MirrorPeriod;

/*FC ��ʽ�� �����߼����� */
typedef struct
{
    SWORD32  sdwCtrlId;                   /* ������ID�����*/
    BYTE     ucWWPN[MAX_FC_WWN_LENGTH];  /* WWPN �����*/
}_PACKED_1_ T_FcMirrorConnectInfo;

typedef struct
{
    T_FcMirrorConnectInfo   tFcMirrorConnect;    /* Զ�̾���FC��ʽ�� ������Ϣ */
    SWORD32 sdwFcNexusStatus;                      /* ����״̬��PORT_STATE_UP��PORT_STATE_DOWN*/
}_PACKED_1_ T_FcNexusInfo;

/* ���о������� */
typedef struct
{
    SWORD32  sdwFcConnectNum;                                                    /* ��FC��ʽ�ľ���������Ŀ ������*/
    T_FcNexusInfo   tFcMirrorConnectInfo[MAX_MIRROR_CONNECT_NUM];        /* ��FC��ʽ�ľ���������Ϣ ������*/
    /*��֧��ISCSI��ʽ���������*/
    //SWORD32  sdwIscsiConnectNum;                                               /* ��Iscsi��ʽ�ľ���������Ŀ ������*/
    //T_IscsiMirrorConnectInfo  tIscsiMirrorConnectInfo[MAX_MIRROR_CONNECT_NUM]; /* ��Iscsi��ʽ�ľ���������Ϣ ������*/
}_PACKED_1_ T_MirrorConnectList;

/* FC��ʽ�����Զ�̴�����Ϣ */
typedef struct
{
    CHAR     ucRmtDiskUuid[MAX_UUID_LEN];           /* Զ�̴���UUID */
    SWORD32  sdwUsedFlag;                                /* Զ�˴��̵�ʹ�ñ�־ ��ȡֵREMOTE_DISK_UNUSED��REMOTE_DISK_USED*/
    SWORD32  sdwLunId;                                   /* Զ�˴���������ӳ�����е�Lun Id ��Ϣ*/
    SWORD64  sqwCapacity;                               /* Զ�˴��̵�����,KB*/
    //T_FcMirrorConnectInfo tFcMirrorConnectInfo[2];     /* Զ�˴��̵�FC���Ӷ�·����Ϣ */
    SWORD32  sdwRemoteDiskState;                         /* Զ�̴��̵�״̬��Ϣ��ȡֵ 0(����)��1(�쳣)*/
}_PACKED_1_ T_FcRdInfo;

/* ����Զ�̴�����Ϣ  */
typedef struct
{
    SWORD32      sdwFcRdNum;                                          /* ��FC��ʽ���ӵ�Զ�̴�����Ŀ ������*/
    T_FcRdInfo   tFcRdInfo[MAX_MIRROR_REMOTE_DISK_NUM];               /* ��FC��ʽ���ӵ�Զ�̴�����Ϣ ������*/
    /*��֧��ISCSI��ʽ������Ӷ���*/
    //SWORD32    sdwIscsiRdNum;                                       /* ��ISCSI��ʽ���ӵ�Զ�̴�����Ŀ */
    //T_IscsiRdInfo tIscsiRdInfo[MAX_MIRROR_REMOTE_DISK_NUM];         /* ��ISCSI��ʽ���ӵ�Զ�̴�����Ŀ*/
}_PACKED_1_ T_RemoteDiskList;

/* FC �˿���Ϣ */
typedef struct
{
   WORD32  sdwPortNo;                                /* �˿ڱ��,��FC�ӿ����˳�����,��1��ʼ��� */
   WORD32  udwPortStatus;                            /* ��ʶ��FC�˿��Ƿ���Ч,ȡֵ:FC_PORT_INVALID ��FC_PORT_VALID ;��ȡֵΪFC_PORT_INVALIDʱ������sdwPortNoֵ��Ч�⣬��������ֵ����Ч  */
   BYTE    ucPortID[MAX_FC_PORT_ID_LENGTH];          /* �˿�ID,FCЭ������,��ʾ��ʽ�����ֽڰ���16����������ʾ������Ϊ0x123456      */
   WORD32  udwPortRate;                              /* �˿����ʣ�ȡֵ��FC_PORT_SPEED_INVALID��FC_PORT_SPEED_2G��FC_PORT_SPEED_4G��FC_PORT_SPEED_8G.*/
   WORD32  udwLinkStatus;                            /* ��·״̬��ȡֵ��PORT_STATE_INVALID��PORT_STATE_UP��PORT_STATE_DOWN*/
   WORD32  udwTopoType;                              /* �˿��������ͣ�FC_TOPO_INVALID��FC_TOPO_PRIVATE_LOOP��FC_TOPO_PUBLIC_LOOP��FC_TOPO_FABRIC��FC_TOPO_P2P*/
   BYTE    ucWwpn[MAX_FC_WWN_LENGTH];                /* �˿�WWPN�������ʽ A1��A2��A3��A4��A5��A6��A7��A8, ȫ0ֵ��Ч*/
}_PACKED_1_ T_FcPortInfo;

/* ÿ����������FC �˿ڵ���Ϣ */
typedef struct
{
    WORD32 sdwCtrlId;                          /* �˿����ڵĿ�����ID */
    BYTE ucWwnn[MAX_FC_WWN_LENGTH];            /* �˿����ڵ����WWNN */
    T_FcPortInfo tFcPortInfo[MAX_FC_PORT_NUM]; /* FC�ӿ���Ϣ�ṹ,ÿ���˿�ռ��һ��������Ŀ */
}_PACKED_1_ T_FcPortInfoPerCtrl;

/* ϵͳ������FC �˿ڵ���Ϣ */
typedef struct
{
    WORD32             udwCtrlNum;                             /*������Ctrl����������*/
    T_FcPortInfoPerCtrl tFcPortInfoPerCtrl[MAX_CTL_NUM];       /*FC�˿���Ϣ�б�*/
}_PACKED_1_ T_FcPortInfoList;


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
    SWORD32 sdwLunState; /* lun ��״̬ */
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
    CHAR      cVolName[MAX_BLK_NAME_LEN]; /* Vol/Svol����,����Ϊ��,���*/
    SWORD32   sdwMapGrpNum;
    CHAR      cMapGrpNames[MAX_MAP_GROUP_NUM_IN_SYS][MAX_MAPGRP_NAME_LEN]; /* ӳ��������*/
    SWORD32 LunLocalId[MAX_MAP_GROUP_NUM_IN_SYS]; /* lun�ڴ�ӳ�����еı���ID */
}_PACKED_1_ T_GrpNameAndLunidByVol;

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
    BYTE       ucISID[6];             /*  ��ʶ��������ϢID  */
    WORD16     udwTSIH;               /* ��ʶTgt����ϢId  */
    WORD16     udwPortalGroupTag;
    WORD16     udwSvrPort;            /* iSCSIר�ö˿� 3260*/
    CHAR       scTgtPortIp[MAX_IP_LEN];   /* ҵ��˿ڵ�IP */
    CHAR       cInitIp[MAX_IP_LEN];   /* Initiator IP */
    CHAR       cInitName[MAX_INI_NAME_LEN];  /* Initiator Name */
    WORD16     udwIniPort; /* ����������Ự�Ķ˿ں� */
    WORD16     udwConnId; /* ���Ӻ� */
}_PACKED_1_ T_IscsiSessInfo;

/*Target�Ự��Ϣ�б�*/
typedef struct
{
    SWORD32     sdwCtrlId;
    WORD32      udwTgtId;                            /*Target Id*/
    WORD32      udwSessCount;                      /*Session ����*/
    T_IscsiSessInfo tIscsiSessInfo[MAX_ISCSI_SESSION_COUNT];        /*Session ��Ϣ�б�*/
}_PACKED_1_ T_IscsiSessInfoList;
/* �Ự��Ϊ1024����ṹ���޸� */
#if 0
typedef struct
{
    WORD32      udwSessCount;                      /*Session ����*/
    WORD32      udwSessCountLocal;                      /* ����Session ����*/
    WORD32      udwSessCountPeer;                      /*�Զ�Session ����*/
    T_IscsiSessionAll tIscsiSessListInfo[MAX_ISCSI_SESSION_COUNT*2];        /*Session ��Ϣ�б�*/
}_PACKED_1_ T_IscsiSessDblList;
#endif
typedef struct
{
    WORD32      udwCtrlId;                      /*������Id */
    WORD32      udwTgtId;                      /* Tgt Id */
    WORD32      udwSessCount;                      /*Session ����*/
}_PACKED_1_ T_GetIscsiSessNum;

typedef struct
{
    SWORD32     sdwCtrlId;
    SWORD32      udwTgtId;                            /*Target Id*/
    BYTE       ucISID[6];             /*  ��ʶ��������ϢID  */
    WORD16     udwTSIH;               /* ��ʶTgt����ϢId  */
    WORD16     udwPortalGroupTag;
    WORD16     udwSvrPort;            /* iSCSIר�ö˿� 3260*/
    CHAR       scTgtPortIp[MAX_IP_LEN];   /* ҵ��˿ڵ�IP */
    CHAR       cInitIp[MAX_IP_LEN];   /* Initiator IP */
    CHAR       cInitName[MAX_INI_NAME_LEN];  /* Initiator Name */
    WORD16     udwIniPort; /* ����������Ự�Ķ˿ں� */
    WORD16     udwConnId; /* ���Ӻ� */
}_PACKED_1_ T_IscsiSessionInfo;


typedef struct
{
    WORD32      udwCtrlId;
    WORD32      udwTid;
    WORD32      udwSessCount;                      /*ÿ��Ҫ��ȡ��Session ����*/
    WORD32      udwSessBgnIndex;                      /*Ҫ��ȡ��Ϣ����ʼλ��*/
    T_IscsiSessionInfo tIscsiSessListInfo[MAX_ISCSI_SESSION_NUM_ONCE];        /*Session ��Ϣ�б�*/
}_PACKED_1_ T_IscsiSessGetOnceInfo;

typedef struct
 {
    WORD32      udwCtrlId;      /*������Id �����*/
    WORD32      udwPortId;      /* �˿� Id �����*/
    WORD32      udwGpnCount;    /*Fc Gpn����������*/
 }_PACKED_1_ T_GetFcGpnNum;

typedef struct
{
   BYTE       ucWWPN[FC_WWN_LENGTH];  /* FC World Wide���� */
}_PACKED_1_ T_FcGpnInfo;


typedef struct
{
    WORD32      udwCtrlId;                         /*������Id �����*/
    WORD32      udwPortId;                         /* �˿� Id �����*/
    WORD16      udwGpnBgnIndex;                    /*Ҫ��ȡ��Ϣ����ʼλ�ã���Σ���1��ʼ*/
    WORD16      udwNextGetIndex;                   /*���ص���һ�λ�ȡ����ʼλ�ã�����*/
    WORD32      udwGpnCount;                       /*���ص�Gpn ����������*/
    BYTE        udwEndFlag;                        /* �Ƿ������ȡ�ı�ʶ������ */

    T_FcGpnInfo tFcGpnListInfo[MAX_FC_GPN_NUM_ONCE]; /*gpn ��Ϣ�б�����*/
}_PACKED_1_ T_GetOnceFcGpnInfo;


#if 0
typedef struct
{
    T_IscsiSessInfoList tIscsiSessInfoLocal;        /*����Session ��Ϣ�б�*/
    T_IscsiSessInfoList tIscsiSessInfoPeer;        /*�Զ�Session ��Ϣ�б�*/
}_PACKED_1_ T_IscsiSessInfoDualList;
#endif
typedef struct
{
    WORD32      udwCtrlCount;                      /*Session ����*/
    T_IscsiSessInfoList tIscsiSessListInfo[2];        /*Session ��Ϣ�б�*/
}_PACKED_1_ T_IscsiSessInfoDualList;
typedef struct
{
    WORD64                 LogInReq;
    WORD64                 TextReq;
    WORD64                 NopOut;
    WORD64                 LogOutReq;
    WORD64                 ScsiCmdReq;
    WORD64                 ScsiMgmtCmdReq;
    WORD64                 DataOut;

    WORD64                 NopOut_Reject;
    WORD64                 ScsiCmdReq_Reject;
    WORD64                 ScsiMgmtCmdReq_Reject;
    WORD64                 DataOut_Reject;

    WORD64                 LogInRsp;
    WORD64                 TextRsp;
    WORD64                 LogOutRsp;
    WORD64                 NopIn;
    WORD64                 ScsiCmdRsp;
    WORD64                 ScsiMgmtCmdRsp;
    WORD64                 R2t;
    WORD64                 DataIn;
    WORD64                 Snack;
    WORD64                 Async;
    WORD64                 Reject;
}_PACKED_1_ T_IscsiPduInfo;

typedef struct
{
    SWORD32     sdwCtrlId;
    T_IscsiPduInfo   tiscsiPduInfo; /*PDU ��Ϣ�б�*/
}_PACKED_1_ T_IscsiPduInfoList;

typedef struct
{
    WORD32      udwCtrlCount;            /* ����������*/
    T_IscsiPduInfoList  tIscsiPduInfo[2];    /* PDU��Ϣ�б�*/
}_PACKED_1_ T_IscsiAllPduStatisticInfo;


/*��ȡsession ����*/
typedef struct
{
    //WORD64      uqwSessId;                /* Session Id, ���*/
    SWORD32     sdwCtrlId;
    BYTE        ucISID[6];                 /*  ��ʶ��������ϢID  */
    WORD16      udwTSIH;                 /* ��ʶTgt����ϢId  */
    WORD32      udwTgtId;               /*Target Id */
    //WORD32      udwPortId;              /*port  Id */
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

typedef struct
{
    WORD32             udwCtrlCount;  /* ����������*/
    T_IscsiTargetInfo  tIscsiTargetInfo[2];   /*Target Name*/
}_PACKED_1_ T_IscsiTargetList;

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
    WORD16       sPGT;    /* PortalGroupTag */
    WORD16       sServPort;                     /* ����˿� */
}_PACKED_1_ T_IscsiPortalInfo;
typedef struct
{
    SWORD32           sdwCtrlId;
   WORD32            udwTgtId;                                                                               /*Target Id*/
   WORD32            udwPortCount;                                                                        /* �˿����� */
   T_IscsiPortalInfo tISCSIPortalInfo[MAX_ISCSI_PORT_NUM_PER_CTL];             /* �˿���Ϣ�б�*/
}_PACKED_1_ T_IscsiPortalList;

typedef struct
{
    WORD32             udwCtrlCount;                        /*Ctrl������ ����*/
    T_IscsiPortalList  tIscsiPortListInfo[2];        /*Port��Ϣ�б�*/
}_PACKED_1_ T_IscsiNetPortalList;


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
/***************************************ͳ����书��***********************************/
typedef struct
{
    WORD64 usr;
    WORD64 nic;
    WORD64 sys;
    WORD64 idle;
    WORD64 iowait;
    WORD64 irq;
    WORD64 softirq;
    WORD64 steal;
    WORD64 total;
    WORD64 busy;
}_PACKED_1_ T_JiffyCounts;

typedef struct
{
    SWORD32  sdwCtrlId;      /* ������Id��0��1����Ӧ�궨��CTRL0��CTRL1 */
    SWORD32  sdwOccupy;      /* CPUռ���� */
}_PACKED_1_ T_CpuOccupyPerCtl;

typedef struct
{
    SWORD32             sdwCtrlNum;             /* ��������Ŀ,  ���� */
    T_CpuOccupyPerCtl   tCpuOccu[ENC_ALL_CTL];  /* ENC_ALL_CTLֵΪ2 ����ʾ���������� */
}_PACKED_1_ T_CpuOccupy;

typedef struct
{
    SWORD32  sdwSlotId;          /* ���̲�λ�ţ�ȡֵ0~27(ScanBox);  0~11(SPR10) */
    SWORD32  sdwEnclosureId;    /* ����Id��ȡֵ 0~ʵ�������� */
    SWORD32  sdwCtrlId;         /* ������Id��0��1����Ӧ�궨��CTRL0��CTRL1*/
    WORD64   dwCmdCount;        /* IO������� */
    WORD64   dwCmdDataSize;    /* IO����Я��������������С�����ֽ�Ϊ��λ */
}_PACKED_1_ T_SasStatPerCtrl;

typedef struct
{
    SWORD32           sdwSlotId;                   /* ���̲�λ�ţ�ȡֵ0~27(ScanBox);  0~11(SPR10) */
    SWORD32           sdwEnclosureId;             /* ����Id��ȡֵ 0~ʵ�������� */
    SWORD32           sdwCtrlNum;                 /* ��������Ŀ,  ����*/
    T_SasStatPerCtrl  tSasStatInfo[ENC_ALL_CTL];  /* �ֱ��ʾ������������Ϣ */
}_PACKED_1_ T_SasStatInfo;

typedef struct
{
    SWORD32  sdwCtrlId;        /* ������Id��0��1����Ӧ�궨��CTRL0��CTRL1*/
    WORD64   HscAverageSpeed; /* ��ͨ����������ƽ���ٶȣ���λByte/s */
    WORD64   HscCurrentSpeed; /* ���1s������ͨ�������ݣ����û��൱��˲ʱ���ʵ�λByte/s */
    WORD64   HscTotalData;    /* ��ͨ��������������ͨ����������������λByte/s */
}_PACKED_1_ T_PcieFlowStatusPerCtl;

typedef struct
{
    SWORD32                 sdwCtrlNum;             /* ��������Ŀ,  ����*/
    T_PcieFlowStatusPerCtl  tPcieFlow[ENC_ALL_CTL]; /*  ENC_ALL_CTL����Ϊ2 */
}_PACKED_1_ T_PcieFlowStatus;

typedef struct
{
    WORD32 dwPortNo;           /* �˿ں� */
	WORD64 qwRcvPkts;          /* ���ձ����� */
	WORD64 qwRcvBytes;         /* �����ֽ��� */
	WORD64 qwSndPkts;          /* ���ͱ����� */
	WORD64 qwSndBytes;         /* �����ֽ��� */
}_PACKED_1_ T_TcsStat;
typedef struct
{
    SWORD32       sdwCtrlId;         /* ������Id��0��1����Ӧ�궨��CTRL0��CTRL1*/
    SWORD32       sdwPortNum;        /* �˿���Ŀ��ֵ�Ǻ궨��TCS_PORT_NUM */
    T_TcsStat    tTcsStat[TCS_PORT_NUM];
}_PACKED_1_ T_TcsStatInfoPerCtl;

typedef struct
{
    SWORD32             sdwCtrlNum;                /* ��������Ŀ,  ����*/
    T_TcsStatInfoPerCtl tTcsStatInfo[ENC_ALL_CTL]; /*  ENC_ALL_CTL����Ϊ2 */
}_PACKED_1_ T_TcsStatInfo;


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
    SWORD32  sdwEnclosureId;      /* ��չ������ID,  ��Ĭ��Ϊ0 */
    SWORD32  sdwCtlId;            /* ctl id or expander id or ENC_ALL_CTL */
    SWORD32  sdwAction;           /* ����(CTL_ONLINE_REBOOT)/����(CTL_SINGLE_REBOOT)���������CTL_SINGLE_REBOOT/CTL_ONLINE_REBOOT */
}_PACKED_1_ T_CtrlAction;

typedef struct
{
    SWORD32  sdwNum;                             /* �������*/
    SWORD32  sdwEnclosureId[MAX_ENC_NUM_IN_SYS]; /* ����Id��ȡֵ 0~ʵ��������*/
}_PACKED_1_ T_AllEncId;

typedef struct
{
    /* ��ȷ��; */
    SWORD32 sdwCtlId;            /* ��ʱʹ�ã�Ϊ�����Windows�µı���  */
}_PACKED_1_ T_CtlHwInfo;

typedef struct
{
    SWORD32 sdwFileType;                          /* eConfigFileType, �����ļ����� */
    CHAR    ucFileName[MAX_CONFIG_FILE_NAME_LEN]; /* ���ļ�����������·�� */
}_PACKED_1_ T_ConfigFile;

typedef struct
{
	SWORD32 sdwRebootEventId;        /* �����¼�ID */
	BYTE    cRebootType;             /* 0---��������, 1----�������оƬ�쳣, 2----������������쳣 */
}_PACKED_1_ T_RebootAudit;

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


/*ISCSI �˿���Ϣ  */
typedef struct
{
    WORD32 udwCtrlId;                        /* ������ID �����*/
    WORD32 dwPortNo;                         /* �˿ںţ���SPR10�У�������Ϊ1~4�����*/
    SWORD32 sdwSetFlg;                       /* ���ñ�־����ȡֵ0��1��2���ֱ��ʾ����MAC������MTU������MAC��MTU ,(�����AMS ��ֵ)*/
    WORD32 udwPortMtu;                       /* �˿�MTU ����ѡ���*/
    BYTE   ucMacData[6];                     /* MAC ��ַ����ѡ���*/
}_PACKED_1_  T_IscsiPortSet;

/* �˿ڵĻ���������Ϣ */
typedef struct
{
    BYTE    ucMacData[PORT_MAC_DATA_SIZE];                      /* MAC ��ַ*/
    WORD32  udwPortMtu;                                         /* �˿�MTU */
    WORD32  udwPortState;                                       /* �˿�״̬��ȡֵPORT_STATE_UP��PORT_STATE_DOWN*/
    WORD32  udwNegMode;                                         /* Э��ģʽ,ȡֵPORT_NEG_FORCE��PORT_NEG_AUTO*/
    WORD32  udwSpeed;                                           /* �˿�����ֵ,ȡֵPORT_SPEED_10M ~ PORT_SPEED_16G*/
    WORD32  udwDuplexMode;                                      /* ˫��ģʽ,ȡֵPORT_DUPLEX_FULL��PORT_DUPLEX_HALF*/
}_PACKED_1_  T_PortInfo;

/*һ��ISCSI �˿ڵ�������Ϣ */
typedef struct
{
    WORD32 dwPortNo;                           /*�˿ںţ���SPR10�У�������Ϊ1~4*/
    T_PortInfo tIscsiPortInfo;                 /* Iscsi  �˿���Ϣ*/
}_PACKED_1_ T_IscsiPortInfo;


/*һ���������ϵ�����ISCSI�˿ڵ���Ϣ*/
typedef struct
{
    WORD32   udwCtrlId;                                           /* ������ID */
    SWORD32  sdwIscsiPortNum;                                     /* Iscsi  �˿����� */
    T_IscsiPortInfo tIscsiPortInfo[SPR10_ISCSI_PORT_NUM];         /* Iscsi  �˿���Ϣ�б� */
}_PACKED_1_ T_SingleIscsiPortInfo;

/*ϵͳ�е����п�������ISCSI�˿ڵ���Ϣ*/
typedef struct
{
    WORD32 udwCtrlNum;                                            /* ϵͳ�п�������Ŀ,���� */
    T_SingleIscsiPortInfo tSingleIscsiPortInfo[2];                /* ϵͳ��Iscsi�˿�������Ϣ,���� */
}_PACKED_1_ T_IscsiPortInfoList;


/*һ���������ϵĹ������ڵ�������Ϣ*/
typedef struct
{
    WORD32 udwCtrlId;                        /* ������ID */
    T_PortInfo tManePortInfo;                /* �������Ϣ*/
}_PACKED_1_ T_SingleManaPortInfo;

/*ϵͳ�е����п������Ϲ������ڵ�������Ϣ*/
typedef struct
{
    WORD32 udwCtrlNum;                                   /* ϵͳ�п�������Ŀ,����*/
    T_SingleManaPortInfo tSingleManaPortInfo[2];         /* ϵͳ�й�������������Ϣ�б�����*/
}_PACKED_1_ T_ManaPortInfoList;

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
    CHAR     cFileName[FILE_NAME_LEN];      /* ���ϴ��ļ����ļ���, ���*/
    WORD32  dwLocalFreeSize;                /* ����ʣ��ռ��С */
    WORD32  dwPeerFreeSize;                 /* �Զ�ʣ��ռ��С */
}_PACKED_1_ T_VerFreeSpace;

typedef struct
{
    CHAR     cFileName[FILE_NAME_LEN];        /* ���ϴ��ļ����ļ���, ���*/
    WORD16   ucFileType;                      /* USP_VER_TYPE/EXP_VER_TYPE/VPD_VER_TYPE/POW_VER_TYPE/BOOT_VER_TYPE ���ֶ�Ŀǰֻ�������EncId=0��Ч */
    SWORD32  sdwIsValid;                      /* У������0---У��ͨ��������--У�鲻ͨ�������ش�����, ���� */
}_PACKED_1_ T_VerFileCheck;

typedef struct
{
    CHAR   cFileName[FILE_NAME_LEN];        /* ���ϴ��ļ����ļ���, ���*/
    BYTE   ucUploadType;                    /* VERSION_FILE_TPYE--�汾�ļ� CONFIG_FILE_TPYE--�����ļ� OTHER_FILE_TYPE--���� */
    WORD16 uwVerType;                       /* USP_VER_TYPE, BOOT_VER_TYPE, EXP_VER_TYPE, VPD_VER_TYPE, POW_VER_TYPE */
}_PACKED_1_ T_UploadPreInfo;

/* ftp�ϴ��ļ��漰�Ľṹ�� */
typedef struct
{
    CHAR   cUserName[USR_NAME_LEN];         /* ftp��������¼�û��� �����*/
    CHAR   cPassWord[PASSWD_LEN];           /* ftp��������¼���� �����*/
    CHAR   cFtpServerIPAddr[MAX_IP_LEN];    /* ftp������IP ��ַ�����*/
    CHAR   cFilePath[FILE_PATH_LEN];        /* ���ϴ��ļ����ļ��� (�������FTP������Ŀ¼�ľ���·��)�����*/
    WORD16 uwVerType;                       /* USP_VER_TYPE, BOOT_VER_TYPE, EXP_VER_TYPE, VPD_VER_TYPE, POW_VER_TYPE */
    BYTE   ucUploadType;                    /* VERSION_FILE_TPYE--�汾�ļ� CONFIG_FILE_TPYE--�����ļ� LOG_FILE_TPYE--��־�ļ� OTHER_FILE_TYPE--���� */
}_PACKED_1_ T_UploadFileInfo;

typedef struct
{
    BYTE    ucFileType;                     /* VERSION_FILE_TPYE--�汾�ļ� CONFIG_FILE_TPYE--�����ļ� LOG_FILE_TPYE--��־�ļ� OTHER_FILE_TYPE �����ļ�*/
    WORD16  uwVerType;                      /* USP_VER_TYPE, BOOT_VER_TYPE, EXP_VER_TYPE, VPD_VER_TYPE, POW_VER_TYPE */
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
    BYTE    ucPeerSyncState;                /* �Զ��ļ�ͬ��״̬ */
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

typedef struct
{
    CHAR       cFilePath[FILE_NAME_LEN];       /* �ļ���(��·��) �����*/
    BYTE       ucExistType;                    /* FILE_EXIST_SINGLE(5):����,FILE_NOT_EXIST_SINGLE(6):������, ---���� */
}_PACKED_1_ T_IsFileLocalExist;

typedef struct
{
    SWORD32 sdwCtrlId;                                  /* ������ID,��ʱ����Ϊ0��1����Ӧ�ĺ궨��CTRL0��CTRL1*/
    CHAR    cManufactureName[MAX_DIAG_DEVICE_NAME_LEN]; /* BBU������Ϣ */
    CHAR    cDeviceName[MAX_DIAG_DEVICE_NAME_LEN];      /* BBU�ͺ� */
    SWORD32 sdwBbuPresentInfo;                          /* BBU�Ƿ���λ ����λΪDIAG_BBU_PRESENT(0)������λΪDIAG_BBU_NOPRESENT(1) */
    //SWORD32 sdwBbuWorkFlag;                             /* BBU��ŵ��־�����ڳ�ŵ�ΪDIAG_BBU_CHARGING(0)�����ڳ�ŵ�ΪDIAG_BBU_NOT_CHARGING(1) */
    SWORD32 sdwBbuDeviceStatus;                         /* BBU״̬��ȡֵ:����(BBU_NORMAL,0)���쳣(BBU_ABNORMAL,1)����������(BBU_LOW_CAPACITY,2) */
}_PACKED_1_ T_SingleBbuInfo;

typedef struct
{
    SWORD32 sdwCtrlFlg;                /* ��������־ ,��ʱ����Ϊ0��1��2����Ӧ�ĺ궨��CTRL_LOCAL��CTRL_PEER��CTRL_DUAL, ��� */
    SWORD32 sdwBbuNum;                 /* BBU��Դ����Ŀ������ */
    T_SingleBbuInfo tSingleBbuInfo[2]; /* ��Ϣ�б��� sdwCtrlFlg = 0/1 ʱ,��tSingleBbuInfo[0]��ֵ,�� sdwCtrlFlg = 2 ʱ , tSingleBbuInfo����ֵ */
}_PACKED_1_ T_BbuInfo;

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
    SWORD32 sdwCtrlId;                              /* ������ID,��ʱ����Ϊ0��1����Ӧ�ĺ궨��CTRL0��CTRL1 */
    SWORD32 sdwBbuwMv;                              /* BBU��ǰ��ѹ����λ��mV */
    SWORD32 sdwBbuwCapacity;                        /* BBUʣ������ٷֱ� */
    //SWORD32 sdwBbuWorkFlag;                         /* BBU��ŵ��־�����ڳ�ŵ�ΪDIAG_BBU_CHARGING(0)�����ڳ�ŵ�ΪDIAG_BBU_NOT_CHARGING(1) */
    SWORD32 sdwBbuPresentInfo;                      /* BBU�Ƿ���λ����λΪDIAG_BBU_PRESENT(0)������λΪDIAG_BBU_NOPRESENT(1) */
    SWORD32 sdwBbuCommunicate;                      /* BBU�ܷ�ͨ�ţ���ͨ��ΪBBU_COMMUNICATE_OK(0)������ͨ��ΪBBU_COMMUNICATE_FAIL(2) */
    SWORD32 sdwBbuCharge;                           /* BBU�ܷ��磬�ܳ��ΪBBU_CHARGE_OK(0)�����ܳ��ΪBBU_CHARGE_FAIL(3) */
    SWORD32 sdwBbuDischarge;                        /* BBU�ܷ�ŵ磬�ܷŵ�ΪBBU_DISCHARGE_OK(0)�����ܷŵ�ΪBBU_DISCHARGE_FAIL(4) */
    //SWORD32 sdwBbuOverTemperature;                  /* BBU�Ƿ���£������¶�ΪBBU_NORMAL_TEMPERATURE(0)������ΪBBU_OVER_TEMPERATURE(5) */
    SWORD32 sdwBbuInvalid;                          /* BBU�Ƿ�ʧЧ,δʧЧΪBBU_VALID(0),ʧЧΪBBU_INVALID(6) */
    SWORD32 sdwBbuCapacity;                         /* BBU��ǰ����ֵ����λ: mah */
    SWORD32 sdwBbuDeviceStatus;                     /* BBU״̬��ȡֵ:����(BBU_NORMAL,0)���쳣(BBU_ABNORMAL,1)����������(BBU_LOW_CAPACITY,2) */
    SWORD32 sdwBbuFaultFlag;                        /* ����BBU״̬ΪBBU_ABNORMALʱ��Ч�����ڱ�ʶBBU�Ǻ����쳣��
                                                                              ȡֵ: DIAG_BBU_NOPRESENT   ��ʾBBU����λ
                                                                                    BBU_COMMUNICATE_FAIL ��ʾBBU����ͨ��
                                                                                    BBU_CHARGE_FAIL      ��ʾBBU���ܳ��
                                                                                    BBU_DISCHARGE_FAIL   ��ʾBBU���ܷŵ�
                                                                                    BBU_OVER_TEMPERATURE ��ʾBBU�¶ȹ��� 
                                                                                    BBU_INVALID          ��ʾBBUʧЧ */
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

/* GE ����� */
typedef struct
{
    SWORD32      sdwCtrlId;                             /* ������ID,��ʱ����Ϊ0��1����Ӧ�ĺ궨��CTRL0��CTRL1*/
	BYTE         geStatus;      						/* GE�˿�����״̬ */
    SWORD32      sdwGeLInkStatus[4];                    /* GE�˿�ʹ��,��ams����ʹ�� */
}_PACKED_1_  T_DiagSingleGeInfo;

typedef struct
{
    SWORD32 sdwCtrlFlg;                                /* ��������־ ,��ʱ����Ϊ0��1��2����Ӧ�ĺ궨��CTRL_LOCAL��CTRL_PEER��CTRL_DUAL,   ���*/
    SWORD32 sdwGeNum;                                  /* ���GE ����Ŀ������ */
    T_DiagSingleGeInfo tDiagSingleGeInfo[2];           /* �����Ϣ�б��� sdwCtrlFlg = 0/1 ʱ,��tDiagSingleGeInfo[0]��ֵ,�� sdwCtrlFlg = 2 ʱ ,tDiagSingleGeInfo����ֵ*/
}_PACKED_1_ T_DiagGeInfo;

/* PCIE��� */
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


/* EEPROM�����  */
typedef struct
{
    SWORD32    sdwCtrlId;                             /* ������ID,��ʱ����Ϊ0��1����Ӧ�ĺ궨��CTRL0��CTRL1*/
    SWORD32    sdwEepromDeviceStatus;                 /* Eeprom�豸״̬,����ΪDEVICE_STATUS_OK���쳣ΪDEVICE_STATUS_FALSE*/
}_PACKED_1_ T_DiagSingleEepromInfo;

typedef struct
{
    SWORD32 sdwCtrlFlg;                               /* ��������־ ,��ʱ����Ϊ0��1��2����Ӧ�ĺ궨��CTRL_LOCAL��CTRL_PEER��CTRL_DUAL,   ���*/
    SWORD32 sdwEepromNum;                             /* ���Eeprom����Ŀ������ */
    T_DiagSingleEepromInfo tDiagSingleEepromInfo[2];  /* �����Ϣ�б��� sdwCtrlFlg = 0/1 ʱ,��tDiagSingleEepromInfo[0]��ֵ,�� sdwCtrlFlg = 2 ʱ ,tDiagSingleEepromInfo����ֵ*/
}_PACKED_1_ T_DiagEepromInfo;



/* ���໷����� */
typedef struct
{
    SWORD32  sdwCtrlId;                               /* ������ID,��ʱ����Ϊ0��1����Ӧ�ĺ궨��CTRL0��CTRL1*/
    SWORD32  sdwClockDeviceStatus;                    /* ���໷�豸״̬,����ΪDEVICE_STATUS_OK���쳣ΪDEVICE_STATUS_FALSE*/
}_PACKED_1_  T_DiagSingleClockInfo;

typedef struct
{
    SWORD32 sdwCtrlFlg;                                 /* ��������־ ,��ʱ����Ϊ0��1��2����Ӧ�ĺ궨��CTRL_LOCAL��CTRL_PEER��CTRL_DUAL,   ���*/
    SWORD32 sdwClockNum;                                /* ������໷����Ŀ������ */
    T_DiagSingleClockInfo tDiagSingleClockInfo[2];      /* �����Ϣ�б��� sdwCtrlFlg = 0/1 ʱ,��tDiagSingleClockInfo[0]��ֵ,�� sdwCtrlFlg = 2 ʱ ,tDiagSingleClockInfo����ֵ*/
}_PACKED_1_ T_DiagClockInfo;

/* FC����� */

typedef struct
{
    SWORD32       sdwCtrlId;
	SWORD32 	  sdwFcCardOnline;                      /* FC�ӿ��Ƿ���λ, DIAG_FC_PRESENT������λ, DIAG_FC_ABSENT������ */
	BYTE 	   sdwFcPortStatus[4];
    BYTE 	   sdwFcLinkStatus[4];                      /* FC�豸״̬,����ΪDEVICE_STATUS_OK���쳣ΪDEVICE_STATUS_FALSE*/
}T_DiagSingleFcInfo;

typedef struct
{
    SWORD32 sdwCtrlFlg;                                 /* ��������־ ,��ʱ����Ϊ0��1��2����Ӧ�ĺ궨��CTRL_LOCAL��CTRL_PEER��CTRL_DUAL,   ���*/
    SWORD32 sdwFcNum;                                   /* ���FC����Ŀ������ */
    T_DiagSingleFcInfo  tDiagSingleFcInfo[2];           /* �����Ϣ�б��� sdwCtrlFlg = 0/1 ʱ,��tDiagSingleFcInfo[0]��ֵ,�� sdwCtrlFlg = 2 ʱ ,tDiagSingleFcInfo����ֵ*/
}_PACKED_1_ T_DiagFcInfo;


/* IBMC����� */
typedef struct
{
	SWORD32  sdwCtrlId;
	BYTE     sdwIbmcStatus;							    /* IBMC�豸״̬,����ΪDEVICE_STATUS_OK���쳣ΪDEVICE_STATUS_FALSE*/
}T_DiagSingleIbmcInfo;

typedef struct
{
	SWORD32 sdwCtrlFlg; 								/* ��������־ ,��ʱ����Ϊ0��1��2����Ӧ�ĺ궨��CTRL_LOCAL��CTRL_PEER��CTRL_DUAL,   ���*/
	SWORD32 sdwIbmcNum;									/* ���IBMC����Ŀ������ */
	T_DiagSingleIbmcInfo	tDiagSingleIbmcInfo[2];		/* �����Ϣ�б��� sdwCtrlFlg = 0/1 ʱ,��tDiagSingleIbmcInfo[0]��ֵ,�� sdwCtrlFlg = 2 ʱ ,tDiagSingleIbmcInfo����ֵ*/
}_PACKED_1_ T_DiagIbmcInfo;



typedef struct
{
    CHAR    cDiagDeviceName[MAX_DIAG_DEVICE_NAME_LEN];    /* ��϶������ƣ�   ����  */
    CHAR    cDiagProperty[MAX_PROP_NAME_LEN];             /* ��϶������ԣ�   ����  */
    CHAR    cDiagPropertyValue[MAX_PROP_VALUE_LEN];       /* ��϶�������ֵ�� ����  */
}_PACKED_1_ T_DiagDeviceProperty;

/* ECC�������� */
typedef struct
{
    SWORD32     sdwEccSwitch;                           /* ���: ECC���򿪿���: DIAG_ECC_SWITCH_OPEN �򿪿���; DIAG_ECC_SWITCH_CLOSE�رտ��� */
}_PACKED_1_  T_DiagSingleEccSwitch;


typedef struct
{
    SWORD32 sdwCtrlFlg;                                 /* ���: ��������־ ,��ʱ����Ϊ0��1��2����Ӧ�ĺ궨��CTRL_LOCAL��CTRL_PEER��CTRL_DUAL,   ���*/
    SWORD32 sdwEccSwitchNum;                            /* ����: ��Ҫ����ECC���صĸ��������� */
    T_DiagSingleEccSwitch  tDiagSingleEccSwitch[2];     /* ���: �����Ϣ�б��� sdwCtrlFlg = 0/1 ʱ,��tDiagSingleEccSwitch[0]��ֵ,�� sdwCtrlFlg = 2 ʱ ,tDiagSingleEccSwitch����ֵ*/
}_PACKED_1_ T_DiagEccSwitch;


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

/* *************************Expander WUI Diag**************************** */
/*������Ϣ*/
typedef struct
{
    SWORD32 sdwLevel;       /* ת�ټ���, 1-13��, ת��Խ�켶��Խ�� */
    WORD16  sdwSpeed;       /* ת��, RPM, Round Per Minute */
    BYTE    bFanStatus;     /* ����״̬,ȡֵ:DEVICE_STATUS_OK(ֵΪ0,��������),DEVICE_STATUS_FALSE(ֵΪ1,�����쳣) */
}_PACKED_1_ T_FanInfo;

/*��Դ��Ϣ*/
typedef struct
{
	SWORD32 sdwVoltInCurrentValue;	   /* �����ѹ��ǰֵ */
	SWORD32 sdwVoltInMaxValue;	       /* �����ѹ����ֵ */
	SWORD32 sdwVoltInMinValue;	       /* �����ѹ����ֵ */
	SWORD32 sdwVoltInStatus;           /* ��Դ�����ѹ״̬, ����ΪDEVICE_STATUS_OK, �쳣ΪDEVICE_STATUS_FALSE */

	SWORD32 sdwVoltOutHighCurrentValue;/* ����ߵ�ѹ��ǰֵ */
	SWORD32 sdwVoltOutHighMaxValue;	   /* ����ߵ�ѹ����ֵ */
	SWORD32 sdwVoltOutHighMinValue;	   /* ����ߵ�ѹ����ֵ */
	SWORD32 sdwVoltOutHighStatus;      /* ��Դ����ߵ�ѹ״̬, ����ΪDEVICE_STATUS_OK, �쳣ΪDEVICE_STATUS_FALSE */

	SWORD32 sdwVoltOutLowCurrentValue; /* ����͵�ѹ��ǰֵ */
	SWORD32 sdwVoltOutLowMaxValue;	   /* ����͵�ѹ����ֵ */
	SWORD32 sdwVoltOutLowMinValue;	   /* ����͵�ѹ����ֵ */
    SWORD32 sdwVoltOutLowStatus;       /* ��Դ����͵�ѹ״̬, ����ΪDEVICE_STATUS_OK, �쳣ΪDEVICE_STATUS_FALSE */

    SWORD32 sdwPowerTempValue;         /* ��Դ�¶�ʵ��ֵ, Centigrade*/
	SWORD32 sdwPowerTempMaxValue;      /* ��Դ�¶�����ֵ */
	SWORD32 sdwPowerTempMinValue;	   /* ��Դ�¶�����ֵ */
    SWORD32 sdwPowerTempStatus;        /* ��Դ�¶�״̬, ����ΪDEVICE_STATUS_OK, �쳣ΪDEVICE_STATUS_FALSE */

    SWORD32 sdwPresentStatus;          /* ��Դ��λ״̬������ΪDEVICE_STATUS_OK, �쳣ΪDEVICE_STATUS_FALSE */
}_PACKED_1_ T_PowerInfo;

/*�����¶���Ϣ*/
typedef struct
{
    SWORD32 sdwBoardTempValue;         /* �����¶�ʵ��ֵ, Centigrade */
	SWORD32 sdwBoardMaxTempValue;      /* �����¶�����,���������¶�Ӧ�ø澯 */
	SWORD32 sdwBoardMinTempValue;      /* �����¶�����,�����ڸ��¶�Ӧ�ø澯 */
    SWORD32 sdwBoardTempStatus;        /* �����¶�״̬, ����ΪDEVICE_STATUS_OK, �쳣ΪDEVICE_STATUS_FALSE */
	SWORD32 sdwCtrId;                  /* �����λ��,ֵΪ0 ����1 */
}_PACKED_1_ T_BoardInfo;

/*CPU�¶���Ϣ*/
typedef struct
{
    SWORD32 sdwCpuTempValue;           /* CPU�¶�ʵ��ֵ, Centigrade   */
	SWORD32 sdwCpuMaxTempValue;        /* CPU�¶�����,���������¶�Ӧ�ø澯 */
	SWORD32 sdwCpuMinTempValue;        /* CPU�¶�����,�����ڸ��¶�Ӧ�ø澯 */
    SWORD32 sdwCpuTempStatus;          /* CPU�¶�״̬, ����ΪDEVICE_STATUS_OK, �쳣ΪDEVICE_STATUS_FALSE */
	SWORD32 sdwCtrId;                  /* �����λ��,ֵΪ0 ����1 */
}_PACKED_1_ T_CpuInfo;

/*�����豸��Ϣ*/
typedef struct
{
    BYTE sdwChassisId;                 /*����ID*/
    BYTE sdwBoardNum;                  /*������������Ŀ*/
    T_FanInfo    tFanInfo[4];          /*������Ϣ, ��λ0��ӦtFanInfo[0]��tFanInfo[1], ��λ1��ӦtFanInfo[2]��tFanInfo[3]*/
    T_PowerInfo  tPowerInfo[2];        /*��Դ��Ϣ, ��λ0��ӦtPowerInfo[0], ��λ1��ӦtPowerInfo[1]*/
    T_BoardInfo  tBoardInfo[2];        /*������Ϣ, ��λ0��ӦtBoardInfo[0], ���ػ�����ֻ��tBoardInfo[0]��ֵ*/
    T_CpuInfo    tCpuInfo[2];          /*CPU��Ϣ, ��λ0��ӦtCpuInfo[0], ���ػ�����ֻ��tCpuInfo[0]��ֵ*/
}_PACKED_1_ T_DiagExpanderInfo;

/*���������豸��Ϣ�б�*/
typedef struct
{
    BYTE                sdwExpanderNum;                     /*�������, ����*/
    BYTE                sdwQueryFlag;                       /*��ѯ�����־, DIAG_ALL_EXPANDER��ѯ��������, DIAG_SINGLE_EXPANDER��ʾ��ѯ��������, ���*/
    BYTE                sdwSingleChassisId;                 /*���ѯ�ĵ���������Ϣ��ID��, ���*/
    T_DiagExpanderInfo  tExpanderInfo[MAX_UP_SUB_ENCL];     /*����������Ϣ�б�, ���*/
}_PACKED_1_ T_ExpanderInfoList;

/* *************************Board WUI Diag**************************** */
/*����״̬��Ϣ*/
typedef struct
{
    SWORD32 sdwCtrlId;                                     /* ������ID, ��ʱ����Ϊ0��1, ��Ӧ�ĺ궨��CTRL0��CTRL1*/
    SWORD32 sdwBoardStatus;                                /* ����״̬, ����ΪDEVICE_STATUS_OK, �쳣ΪDEVICE_STATUS_FALSE*/
}_PACKED_1_ T_DiagSingleBoardStatus;

typedef struct
{
    SWORD32 sdwCtrlFlg;                                    /* ��������־ ,��ʱ����Ϊ0��1��2, ��Ӧ�ĺ궨��CTRL_LOCAL��CTRL_PEER��CTRL_DUAL, ���*/
    SWORD32 sdwBoardNum;                                   /* ����������Ŀ, ���� */
    T_DiagSingleBoardStatus tDiagSingleBoardStatus[2];     /* �����Ϣ�б�, �� sdwCtrlFlg = 0/1 ʱ, tDiagSingleBoardStatus[0]��ֵ,�� sdwCtrlFlg = 2 ʱ, tDiagSingleBoardStatus����ֵ*/
}_PACKED_1_ T_DiagBoardStatus;

/* *************************Hdd WUI Diag**************************** */
/*����״̬��Ϣ*/
typedef struct
{
    SWORD32 sdwSlotId;                                     /* ���̲�λ�ţ�ȡֵ0~27(ScanBox);  0~11(SPR10) */
    SWORD32 sdwEnclosureId;                                /* ����Id, ȡֵ 0~ʵ��������*/
    SWORD32 sdwHddStatus;                                  /* ����״̬, ����ΪDEVICE_STATUS_OK, �쳣ΪDEVICE_STATUS_FALSE*/
}_PACKED_1_ T_DiagSingleHddStatus;

typedef struct
{
    SWORD32 sdwHddNum;                                               /* ���̸�����ȡֵMAX_ENC_NUM_IN_SYS(4) * MAX_PD_NUM_IN_ENC(16) */
    T_DiagSingleHddStatus tDiagSingleHddStatus[MAX_PD_NUM_IN_SYS];   /* ���������Ϣ�б� */
}_PACKED_1_ T_DiagHddStatus;

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
    SWORD32 sdwLevel;       /* ת�ټ��� */
    WORD16 swSpeed;         /* ת�� */
    BYTE    bFanStatus;     /* ����״̬,ȡֵ:DEVICE_STATUS_OK(ֵΪ0,��������),DEVICE_STATUS_FALSE(ֵΪ1,�����쳣) */
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
    BYTE ucPeerStatus;/*�԰���λ��־*/
    BYTE ucSubId[SUBID_LEN]; /*��չ��ID*/
    BYTE ucSgName[MAX_DISKNAMELEN];  /*SG�豸��*/
    WORD64   uqwSasAddr;  /*EXPsaS��ַ*/

    T_EncFanInfo tFaninfo[MAX_FAN_NUM];  /*������Ϣ*/
    T_EncPsInfo tPsinfo[MAX_PS_NUM];  /*��Դ��Ϣ*/
    T_EncVoltInfo tVoltinfo[MAX_VOLT_NUM];  /*��ѹ��Ϣ*/
    T_EncTempratureInfo tTempratureinfo[MAX_TEMP_NUM];  /*�¶���Ϣ*/

}_PACKED_1_ T_EncDeviceInfo;
/*�����豸��Ϣ�б�*/
typedef struct
{
    BYTE            ucExpcount;                   /*�������*/
    SWORD32         sdwCtrlId;                    /* ������ID,��ʱ����Ϊ0��1����Ӧ�ĺ궨��CTRL0��CTRL1*/
    T_EncDeviceInfo tExpanders[MAX_UP_SUB_ENCL];  /*��������ͷ*/
}_PACKED_1_  T_EncDeviceInfoList;
/*���������豸��Ϣ�б�*/
typedef struct
{
    SWORD32 sdwCtrlFlg;                                 /* ��������־ ,��ʱ����Ϊ0��1��2����Ӧ�ĺ궨��CTRL_LOCAL��CTRL_PEER��CTRL_DUAL,   ���*/
    SWORD32 sdwCtrlNum;                                 /* ����������Ŀ������ */
    T_EncDeviceInfoList tEncInfoList[2];                /* �����Ϣ�б��� sdwCtrlFlg = 0/1 ʱ,��tEncInfoList[0]��ֵ,�� sdwCtrlFlg = 2 ʱ ,tEncInfoList[2]����ֵ*/
}_PACKED_1_ T_AllEncInfoList;

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
    SWORD32     sdwCtrlFlag;                         /* ��ѯ�Ŀ��������Զ˱�� */
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
    BYTE    ucAlarmMailEnableFlag;
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
    SWORD32      sdwSessionId;  //�û���¼��sessionid�ṹ
    SWORD32      sdwExistFlag;  //0��ʶsid���ڣ�1��ʶsid������
}_PACKED_1_ T_WebSessionIdExist;

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
    WORD32 sdwBroadType;          /* �������ͣ�ȡֵBROAD_UNKNOWN / BROAD_SBCJ / BROAD_SPR10 / BROAD_SPR11 / BROAD_PC */
}_PACKED_1_ T_SysBroadType;


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

/******************************* ��ȡ������Ԥ���¼���cache�Զ���д���� ************************************/
typedef struct
{
    /* Cache�Զ�ֱд���Կ��أ�ON��ʾԤ����OFF��ʾ�أ�RST��ʾ����ԭֵ */
    BYTE bFanFaultEvt;         /* ����Cache�Զ�ֱд���Կ��� */
    BYTE bPowerFaultEvt;       /* ��ԴCache�Զ�ֱд���Կ��� */
    BYTE bBbuFaultEvt;         /* BBU��Cache�Զ�ֱд���Կ��� */
    BYTE bUpsConnLossEvt;      /* UPS���Ӷ�ʧCache�Զ�ֱд���Կ��� */
    BYTE bUpsBattFaultEvt;     /* UPS���ʧЧCache�Զ�ֱд���Կ��� */
    BYTE bUpsOverloadEvt;      /* UPS����Cache�Զ�ֱд���Կ��� */
    BYTE bOher;                /* ����Cache�Զ�ֱд���Կ��� */
}_PACKED_1_ T_AutoWrThrPreSet;

/******************************* Ԥ���¼��޸�Cache�����豸״̬ ************************************/
typedef struct
{
    BYTE    bPowerStatus;          /* ��Դ״̬*/
    BYTE    bFanStatus;            /* ����״̬*/
    BYTE    bBbuStatus;            /* BBU״̬*/
}_PACKED_1_  T_PreEvtDeviceStatus;

/* SNMP ���ú�TRAP���� */
typedef struct
{
	SWORD32 sdwServerId; //Server ID.
    SWORD32 sdwAction; //SET - enable SNMP server, unable SNMP server setting.
	CHAR    scServerIp[MAX_IP_LEN]; //��������������ip��ַ
	SWORD32 sdwServerPort; //�������˿�,���ΪINVALID,��ʹ��Ĭ�϶˿ں�
	SWORD32 sdwSnmpVersion; //SNMP�汾
	CHAR    scReadCommName[MAX_SNMP_COMMUNITY_NAME_LEN]; //���Ŷ�����
}_PACKED_1_ T_SnmpTrapConfig;
typedef struct
{
    WORD32          udwServerCount;  /* ����������*/
    T_SnmpTrapConfig  atSnmpRcv[MAX_SNMP_NOTIFY_NUM];
}_PACKED_1_ T_TrapCfgList;

typedef struct
{
	SWORD32 sdwServerId; //Server ID.
	SWORD32 sdwSnmpVersion; //SNMP�汾
	CHAR    scServerIp[MAX_IP_LEN]; //��������������ip��ַ
	CHAR    scReadCommName[MAX_SNMP_COMMUNITY_NAME_LEN]; //���Ŷ�����
}_PACKED_1_ T_TrapCfgMody;
/********************************MTS��������صĽṹ��˵��******************************************/
/********************************������˵����, �ṹ���Ա��Ϊ����***********************************/

/****************************************���幫�������*********************************************/
/* ����EPLD�Լ� */
typedef struct
{
    WORD32 dwStandardValue;   /* ��׼ֵ  */
	WORD32 dwReadValue;       /* ����ֵ  */
	WORD32 dwCheckResult;     /* �ж�ֵ  */
	WORD32 dwTimeoutMax;      /* ��ʱ���ֵ */
}_PACKED_1_ T_Mts_Check_Work_Epld_Self;

/* ����EPLD��Ϣ��ȡ */
typedef struct
{
    WORD32 dwBoardId;         /* ����ID        */
	WORD32 dwPcbVer;          /* ����pcb�汾�� */
	WORD32 dwBomVer;          /* ����bom�ϵ�   */
	WORD32 dwTimeoutMax;      /* ��ʱ���ֵ */
}_PACKED_1_ T_Mts_Check_Work_Epld_Info;

/* ����EPLD��Ϣ��ȡ */
typedef struct
{
    WORD32 dwBoardId;         /* ����ID        */
	WORD32 dwPcbVer;          /* ����pcb�汾�� */
	WORD32 dwBomVer;          /* ����bom�ϵ�   */
	WORD32 dwTimeoutMax;      /* ��ʱ���ֵ */
}_PACKED_1_ T_Mts_Check_Boot_Epld_Info;

/* ����������ɨ�� */
typedef struct
{
    BYTE   cLableInfo[MAX_INFO_LABLE_LENGTH]; /* ������ */
	WORD32 dwTimeoutMax;                      /* ��ʱ���ֵ */
}_PACKED_1_ T_Mts_Check_Lable_Info;

/* ����Ӳ����ַ��ȡ */
typedef struct
{
    WORD32   dwBackBoardId;    /* 6λ���Ա���ID */
	BYTE     cSlotId;          /* ��λ�� */
	WORD32   dwTimeoutMax;      /* ��ʱ���ֵ */
}_PACKED_1_ T_Mts_Check_Phy_Addr_Info;

/* ��λ�źŲ��� */
typedef struct
{
	BYTE     cOnlieFlag;        /* ��λ��־ */
	WORD32   dwTimeoutMax;      /* ��ʱ���ֵ */
}_PACKED_1_ T_Mts_Check_Online_Flag;

/* CPU���DDR3���� */
typedef struct
{
	WORD16  wMtsType;      /* ��Σ���������:MTS_FT,  ���ܲ���:  MTS_HT,       ����       */
	BYTE    cCheckResult;  /* ���Σ����Խ��:MTS_DDR_GOOD ����ͨ��;  MTS_DDR_NOT_GOOD  ���Բ�ͨ�� */
	WORD32  dwTimeoutMax;  /* ��ʱ���ֵ */
}_PACKED_1_ T_Mts_Check_Ddr;

/* NVRAMоƬ���� */
typedef struct
{
   WORD16  wMtsType;       /* ��Σ���������:MTS_FT,  ���ܲ���: MTS_HT,      ����*/
   BYTE    cCheckResult;   /* ���Σ����Խ��:MTS_NVRAM_GOOD ����ͨ��; MTS_NVRAM_NOT_GOOD ���Բ�ͨ�� */
   WORD32  dwTimeoutMax;   /* ��ʱ���ֵ */
}_PACKED_1_ T_Mts_Check_Nvram;

/* �汾FLASH���� */
typedef struct
{
	WORD16  wMtsType;     /* ��Σ���������:MTS_FT,  ���ܲ���:
	                                        MTS_HT,  ���²���
                                            MTS_MO_COMPA   �汾�Ƚ�
	                      */
	BYTE    cCheckResult; /* ���Σ����Խ�� */
	WORD32  dwTimeoutMax; /* ��ʱ���ֵ */
}_PACKED_1_ T_Mts_Check_Verflash;

/* ����ʱ�ӵ�·���� */
typedef struct
{
   WORD16  wMtsClockType;  /* ��Σ�MTS_25M_CLOCK, MTS_33_CLOCK...*/
   BYTE    cCheckResult;   /* ���Σ�MTS_CLOCK_GOOD 	    ����ͨ��
                                   MTS_CLOCK_NOT_GOOD	���Բ�ͨ��*/
   WORD32  dwTimeoutMax;   /* ��ʱ���ֵ */
}_PACKED_1_ T_Mts_Check_Clock;

/* ���ָʾ�Ʋ��� */
typedef struct
{
    WORD32  dwLedId;      /* < ���:��id��ͳһ���            */
    WORD32  dwLedColor;   /* < ���:LED��ɫ���Ե�ɫ��ȡĬ��ֵ */
    WORD32  dwFlashType;  /* < ���:���õ�ǰ���Ʒ�ʽ          */
    WORD32  dwTimeoutMax; /* ��ʱ���ֵ */
}_PACKED_1_ T_Mts_Check_Led;

/* ���Ź���λ���� */
typedef struct
{
    WORD32  dwMtsType;    /* ��Σ�ʹ�ܿ��Ź�:MTS_ENABLE_WTG,  ���ÿ��Ź�: MTS_DISABLE_WTG, */
    WORD32  dwTimeoutMax; /* ��ʱ���ֵ */
}_PACKED_1_ T_Mts_Check_Wtg;

/* �¶ȴ��������� */
typedef struct
{
   /* �����±�0����LM_93  1����75_1, 2����75_2, 3����75_2 4����75_3 */
   BYTE    cSenorStatus[5];  /* ����; MTS_SENOR_GOOD����  MTS_SENOR_NOT_GOOD ������ */
   SWORD32 sdwSenorValue[5]; /* ����; ��������ʵ���¶� */
   BYTE    cEncDevId;          /* ��չ��� */
   WORD32  dwTimeoutMax;     /* ��ʱ���ֵ */
}_PACKED_1_ T_Mts_Check_Senor;

/* RTCʵʱʱ�ӵ�·���� */
typedef struct
{
   BYTE    cCheckResult;   /* ���Σ�MTS_RTC_GOOD 	    ����ͨ��
                                    MTS_RTC_NOT_GOOD	���Բ�ͨ�� */
   WORD32  dwTimeoutMax;   /* ��ʱ���ֵ */
}_PACKED_1_ T_Mts_Check_Rtc;

/* ���GE�ڻ��ز��� */
typedef struct
{
   WORD16  wMtsType;       /* ��Σ���������:MTS_FT,  ���ܲ���: MTS_HT,      ����*/
   BYTE    cCheckResult[4];   /* ���Σ����Խ��:MTS_GE_GOOD ����ͨ��; MTS_GE_NOT_GOOD ���Բ�ͨ�� */
   WORD32  dwTimeoutMax;   /* ��ʱ���ֵ */
}_PACKED_1_ T_Mts_Check_Ge;

/* ������������Ϊͨ����·���� */
typedef struct
{
   BYTE    cCheckResult;   /* ���Σ�MTS_LINK_GOOD 	    ����ͨ��
                                    MTS_LINK_NOT_GOOD	���Բ�ͨ�� */
   WORD32  dwTimeoutMax;   /* ��ʱ���ֵ */
}_PACKED_1_ T_Mts_Check_Conn_Link;

/************************************�����ض������************************************/
/* Cacheͬ���źŲ��� */
typedef struct
{
    WORD32     dwPciePexId;          /* PEX8624����ID */
	BYTE       cCheckResult;		 /* PEX8624�Լ��� */
    WORD32     dwPcieLinkSpeed;      /* PCIE �������� G1/G2*/
    WORD32     dwPcieLinkWidth;      /* PCIE ���Ӵ���x1~x4 x0��ʾ��link */
    WORD32     dwPcieBadTlpCount;    /* TLP�㻵������ */
    WORD32     dwPcieBadDllpCount;   /* DLLP�㻵������ */
    WORD32     dwPcieEepromStatus;   /* PCIE �豸E2prom��λ�����Ч��� */
    WORD32     dwPcieDllpStatus;     /* DLLP ״̬ */
    BYTE       cPcieDeviceStatus;    /* PCIE�豸״̬ */
    WORD32     dwTimeoutMax;         /* ��ʱ���ֵ */
}_PACKED_1_  T_Mts_Check_Pcie_Info;

/* PHYоƬ�Լ� */
typedef struct
{
	WORD16  wPhyDevId;  		/*  ��Σ��궨��MTS_PHY_88E1111��MTS_PHY_88E1119 */
    WORD32  dwPhyId;  			/* ���Σ�����ID */
	BYTE    cCheckResult;       /* ���Σ����Խ��:MTS_PHYSELF_GOOD ����ͨ��; MTS_PHYSELF_NOTGOOD ���Բ�ͨ�� */
    WORD32  dwTimeoutMax;       /* ��ʱ���ֵ */
} _PACKED_1_ T_Mts_Check_Phyself;

/* PCIEоƬ���Լ� */
//˵��:ͬcacheͬ���źŲ���

/* SAS������PM8001�Լ� */
typedef struct
{
	WORD32	dwPmId; 	    /* ����: PM8001����ID */
	BYTE	cCheckResult;	/* ����: ����� */
	WORD32	dwTimeoutMax;	/* ��ʱ���ֵ */
}_PACKED_1_ T_Mts_Check_Pm;

/************************************IO�ź���************************************/
/* Ӳ��ָʾ�Ʋ��� */
//�������ݽṹ

/* ����IO�ź�ָʾ�Ʋ��� */
typedef struct
{
    WORD32  dwInputIoSigType;	/* ����IO�ź����Ͳ���  */
	BYTE    cCheckResult;		/* ״̬����ֵ       */
    WORD32  dwTimeoutMax;       /* ��ʱ���ֵ */
}_PACKED_1_ T_Mts_Check_InputIo;


/* ���IO�ź�ָʾ�Ʋ��� */
typedef struct
{
    WORD32  dwOutputIoSigType;	/* ���IO�ź����Ͳ���  */
	BYTE    cInputValue;        /* ����������ź�0��1,��� */
	BYTE    cCheckResult;		/* ״̬����ֵ       */
    WORD32  dwTimeoutMax;       /* ��ʱ���ֵ */
}_PACKED_1_ T_Mts_Check_OutputIo;


/* ��Դ�ź�ָʾ�Ʋ��� */
typedef struct
{
	BYTE	cCheckResult;		/* ״̬����ֵ */
	WORD32	dwTimeoutMax;		/* ��ʱ���ֵ */
}_PACKED_1_ T_Mts_Check_Power_Light;

/* BBU��Ϣ��ȡ */
typedef struct
{
    WORD32  dwBbuId;	    /* BBU������к�     */
	WORD16  wBbuMv;			/* BBU��ǰ��ѹ       */
	WORD16  wBbuCapacity;	/* BBUʣ������ٷֱ� */
	BYTE    ucPresent;   	/* BBU��λ��Ϣ  MTS_BBU_PRESENT;  ��λ	        MTS_BBU_NOTPRESENT:  ����λ */
	BYTE    ucWorkFlag;    	/* �Ƿ���������	MTS_BBU_CHARGING: BBU���ڳ�ŵ� MTS_BBU_NOT_CHARGING:BBU��ʱû�г�ŵ� */
	BYTE    cCheckResult;   /* BBU״̬ MTS_BBU_GOOD ���� MTS_BBU_GOOD������ */
    WORD32  dwTimeoutMax;   /* ��ʱ���ֵ */
}_PACKED_1_ T_Mts_Check_Bbu;

/* 8005���Ź���λ���� */
typedef struct
{
	BYTE	cCheckResult;		/* ���Է���ֵ */
    BYTE    cEncDevId;            /* ��չ��� */
	WORD32	dwTimeoutMax;		/* ��ʱ���ֵ */
}_PACKED_1_ T_Mts_Check_Ezzf_Reset;

/* minisas�ӿ��Ի����� */
typedef struct
{
	BYTE	cCheckResult[4];    /* �������ʷ���ֵ */
    BYTE    cEncDevId;            /* ��չ��� */
	WORD32	dwTimeoutMax;		/* ��ʱ���ֵ */
}_PACKED_1_ T_Mts_Check_Ezzf_GetSpeed;


/* ����SAS�ӿ��Ի����� */
typedef struct
{
	BYTE	cCheckResult[12];	/* �������ʷ���ֵ */
    BYTE    cEncDevId;            /* ��չ��� */
	WORD32	dwTimeoutMax;		/* ��ʱ���ֵ */
}_PACKED_1_ T_Mts_Check_Ezzf_BdGetSpeed;


/* TW0\TW1\TW2�źŲ��� */
typedef struct
{
	BYTE    cTwSigValue[6];
    BYTE    cEncDevId;            /* ��չ��� */
	WORD32	dwTimeoutMax;		/* ��ʱ���ֵ */
}_PACKED_1_ T_Mts_Check_Ezzf_Ttsig;


/* SAS��չоƬPM8005�Լ� */
typedef struct
{
	BYTE	cSasAddr[8];		/* SAS��ַ */
    BYTE    cEncDevId;            /* ��չ��� */
	WORD32	dwTimeoutMax;		/* ��ʱ���ֵ */
}_PACKED_1_ T_Mts_Check_Ezzf_Self;


/* Ӳ��ָʾ�Ʋ��� */
typedef struct
{
	BYTE	cMtsType;		    /* ���or��� */
	WORD32  dwHddLightType;     /* ������� */
    WORD32  dwHddIndexId;       /* ������ 1~12*/
	WORD32	dwTimeoutMax;		/* ��ʱ���ֵ */
}_PACKED_1_ T_Mts_Check_Hdd_Light;

/* �ڴ��С���� */
typedef struct
{
	WORD32  dwDdrSize;          /* �ڴ��С,��λ:M */
	BYTE    cCoherenceResult;   /* ����һ���Բ��� */
	BYTE    cCorrectResult;     /* ������ȷ�Բ��� */
	WORD32	dwTimeoutMax;		/* ��ʱ���ֵ */
}_PACKED_1_ T_Mts_Check_Ddr_Size;

/* �汾��ѯ */
typedef  struct
{
	BYTE	cEncDevId;	        //  expansion ID
	SWORD32 sdwVerType;         //  version   type
	WORD32  dwWorkEpldVer;      //  work epld version
	WORD32  dwBootEpldVer;      //  boot epld version
	CHAR    dwZbootVer[128];    //  Zboot     version
	CHAR    dwUspVer[128];      //  USP       version
	CHAR    dwExpVer[128];      //  8005      version
	WORD32	dwTimeoutMax;  //
}_PACKED_1_ T_Mts_Check_Ver;

/* MAC��ַ */
typedef  struct
{
    BYTE    cMacAddr[128];
	WORD32	dwTimeoutMax;
}_PACKED_1_ T_Mts_Check_MacAddr;
/************************************������End******************************************/

/***********************************��������Begin***************************************/
/* ����EPLD�Լ� */
typedef  struct
{
	BYTE    ucChassisId;   					/*����ID*/
	WORD32  dwStandardValue;          		/* Output parameter: standard value */
	WORD32  dwReadValue;              		/* Output parameter: Read-Out value */
	WORD32  dwCheckResult;            		/* Output parameter: CheckReust */
}_PACKED_1_ T_Cmt_Check_WorkEpldSelf;


typedef  struct
{
	//BYTE 	ucSlotId;	   			/*��λID*/
    //BYTE 	ucPeerStatus;			/*�԰���λ��־*/
	T_Cmt_Check_WorkEpldSelf    tCmtCheckWorkEpldSelf[2];
}_PACKED_1_ T_CmtWorkEpldSelf;



typedef  struct
{
	WORD32	dwTimeoutMax;		/* Input  parameter: Max-Timeout value */
	WORD32  dwEncNum;           /* �������,��СΪ1,���Ϊ5 */
	T_CmtWorkEpldSelf       tCheckWorkEpldSelf[CMT_CLASS_LEVEL];
}_PACKED_1_ T_WorkEpldSelfList;


/* ����EPLD��Ϣ��ȡ */
typedef  struct
{
	BYTE   ucChassisId;   	  /* ����ID */
    WORD32 dwBoardId;         /* ����ID        */
	WORD32 dwPcbVer;          /* ����pcb�汾�� */
	WORD32 dwBomVer;          /* ����bom�ϵ�   */
}_PACKED_1_ T_Cmt_Check_WorkEpldInfo;

typedef  struct
{
	//BYTE 	ucSlotId;	   		/*��λID*/
    //BYTE 	ucPeerStatus;		/*�԰���λ��־*/
	T_Cmt_Check_WorkEpldInfo    tCmtCheckWorkEpldInfo[2];
}_PACKED_1_ T_CmtWorkEpldInfo;

typedef  struct
{
	WORD32	dwTimeoutMax;      /* Input  parameter: Max-Timeout value */
	WORD32  dwEncNum;          /* �������,��СΪ1,���Ϊ5 */
	T_CmtWorkEpldInfo          tCheckWorkEpldInfo[CMT_CLASS_LEVEL];
}_PACKED_1_ T_WorkEpldInfoList;


/* ����EPLD��Ϣ��ȡ */
typedef  struct
{
	BYTE   ucChassisId;   	  /* ����ID */
    	WORD32  dwStandardValue;          		/* Output parameter: standard value */
	WORD32  dwReadValue;              		/* Output parameter: Read-Out value */
	WORD32  dwCheckResult;            		/* Output parameter: CheckReust */
       BYTE p33M;
       BYTE p75M;
       WORD32 dwBoardId;         /* ����ID        */
	WORD32 dwPcbVer;          /* ����pcb�汾�� */
	WORD32 dwBomVer;          /* ����bom�ϵ�   */
	WORD32   dwBackBoardId;    /* 6λ���Ա���ID */

}_PACKED_1_ T_SPR11_Cmt_Check_WorkEpldInfo;

typedef  struct
{
    T_SPR11_Cmt_Check_WorkEpldInfo    tCmtCheckWorkEpldInfo[2];
}_PACKED_1_ T_SPR11CmtWorkEpldInfo;

typedef  struct
{
	WORD32	dwTimeoutMax;      /* Input  parameter: Max-Timeout value */
	WORD32  dwEncNum;          /* �������,��СΪ1,���Ϊ5 */
	T_SPR11CmtWorkEpldInfo          tCheckWorkEpldInfo[CMT_CLASS_LEVEL];
}_PACKED_1_ T_SPR11WorkEpldInfoList;
/* ����EPLD��Ϣ��ȡ */
typedef  struct
{
	BYTE   ucChassisId;   	  /* ����ID */
    WORD32 dwBoardId;         /* ����ID        */
	WORD32 dwPcbVer;          /* ����pcb�汾�� */
	WORD32 dwBomVer;          /* ����bom�ϵ�   */
}_PACKED_1_ T_Cmt_Check_BootEpldInfo;

typedef  struct
{
	//BYTE 	ucSlotId;	   		/*��λID*/
    //BYTE 	ucPeerStatus;		/*�԰���λ��־*/
	T_Cmt_Check_BootEpldInfo    tCmtCheckBootEpldInfo[2];
}_PACKED_1_ T_CmtBootEpldInfo;

typedef  struct
{
	WORD32	dwTimeoutMax;		/* Input  parameter: Max-Timeout value */
	WORD32  dwEncNum;           /* �������,��СΪ1,���Ϊ5����չ����û������EPLD�����ֶα��� */
	T_CmtBootEpldInfo     tCheckBootEpldInfo[CMT_CLASS_LEVEL];
}_PACKED_1_ T_BootEpldInfoList;


/* ���ӱ�ǩ��Ϣ */
typedef  struct
{
	BYTE   ucChassisId; 					  /* ����� */
	BYTE   cLableInfo[MAX_INFO_LABLE_LENGTH]; /* ������ */
}_PACKED_1_ T_Cmt_Check_Lable_Info;

typedef struct
{
	T_Cmt_Check_Lable_Info	    tCmtCheckLableInfo[2];
}_PACKED_1_ T_CmtLableInfo;

typedef  struct
{
	WORD32	dwTimeoutMax;		/* Input  parameter: Max-Timeout value */
	WORD32  dwEncNum;           /* �������,��СΪ1,���Ϊ5����չ����û������EPLD�����ֶα��� */
	T_CmtLableInfo      tCheckLableInfo[CMT_CLASS_LEVEL];
}_PACKED_1_ T_LableInfoList;

/* ����Ӳ����ַ��ȡ  */
typedef  struct
{
	BYTE     ucChassisId; 		   /* ����� */
    WORD32   dwBackBoardId;        /* 6λ���Ա���ID */
	BYTE     cSlotId;              /* ��λ�� */
}_PACKED_1_ T_Cmt_Check_PhyAddr_Info;

typedef struct
{
	T_Cmt_Check_PhyAddr_Info	    tCmtCheckPhyAddrInfo[2];
}_PACKED_1_ T_CmtPhyAddrInfo;

typedef  struct
{
	WORD32	dwTimeoutMax;		/* Input  parameter: Max-Timeout value */
	WORD32  dwEncNum;           /* �������,��СΪ1,���Ϊ5����չ����û������EPLD�����ֶα��� */
	T_CmtPhyAddrInfo     tCheckPhyAddrInfo[CMT_CLASS_LEVEL];
}_PACKED_1_ T_PhyAddrInfoList;

/* ��λ�źŲ��� */
typedef  struct
{
	BYTE     ucChassisId; 	    /* ����� */
	BYTE     cOnlieFlag;        /* ��λ��־ */
}_PACKED_1_ T_Cmt_Check_OnlineFlag_Info;

typedef struct
{
	T_Cmt_Check_OnlineFlag_Info	    tCmtCheckOnlieFlagInfo[2];
}_PACKED_1_ T_CmtOnlineFlagInfo;

typedef  struct
{
	WORD32	dwTimeoutMax;		/* Input  parameter: Max-Timeout value */
	WORD32  dwEncNum;           /* �������,��СΪ1,���Ϊ5����չ����û������EPLD�����ֶα��� */
	T_CmtOnlineFlagInfo    tCheckOnlieFlagInfo[CMT_CLASS_LEVEL];
}_PACKED_1_ T_OnlineFlagInfoList;

/* ����ʱ�ӵ�·���� */
typedef  struct
{
	BYTE    ucChassisId; 	/* ����� */
	WORD16	wMtsClockType;	/* ��Σ�MTS_25M_CLOCK, MTS_33_CLOCK...*/
	BYTE	cCheckResult[5];	/* ���Σ�MTS_CLOCK_GOOD 	 ����ͨ��
									 MTS_CLOCK_NOT_GOOD  ���Բ�ͨ��*/
}_PACKED_1_ T_Cmt_Check_Clock_Info;

typedef struct
{
	T_Cmt_Check_Clock_Info	    tCmtCheckClockInfo[2];
}_PACKED_1_ T_CmtClockInfo;

typedef  struct
{
	WORD32			dwTimeoutMax;		/* Input  parameter: Max-Timeout value */
	WORD32  		dwEncNum;           /* �������,��СΪ1,���Ϊ5����չ����û������EPLD�����ֶα��� */
	T_CmtClockInfo  tCheckClockInfo[CMT_CLASS_LEVEL];
}_PACKED_1_ T_ClockInfoList;

/* �¶ȴ��������� */
typedef  struct
{
	BYTE    ucChassisId; 			/* ����� */
	/* �����±�0����LM_93  1����75_1, 2����75_2, 3����75_2 4����75_3 */
	BYTE	cSenorStatus[5];  /* ����; MTS_SENOR_GOOD����  MTS_SENOR_NOT_GOOD ������ */

}_PACKED_1_ T_Cmt_Check_Senor_Info;

typedef struct
{
	T_Cmt_Check_Senor_Info	    tCmtCheckSenorInfo[2];
}_PACKED_1_ T_CmtSenorInfo;

typedef  struct
{
	WORD32			dwTimeoutMax;		/* Input  parameter: Max-Timeout value */
	WORD32  		dwEncNum;           /* �������,��СΪ1,���Ϊ5����չ����û������EPLD�����ֶα��� */
	T_CmtSenorInfo  tCheckSenorInfo[CMT_CLASS_LEVEL];
}_PACKED_1_ T_SenorInfoList;
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
    WORD64   uqwSasAddr;  /*EXPsaS��ַ*/
    T_EncFanInfo tFaninfo[MAX_FAN_NUM];  /*������Ϣ*/
    T_EncPsInfo tPsinfo[MAX_PS_NUM];  /*��Դ��Ϣ*/
    T_EncVoltInfo tVoltinfo[MAX_VOLT_NUM];  /*��ѹ��Ϣ*/
    T_EncTempratureInfo tTempratureinfo[MAX_TEMP_NUM];  /*�¶���Ϣ*/

}_PACKED_1_ T_MtsEncDeviceInfo;
/*�����豸��Ϣ�б�*/
typedef struct
{
    WORD32	  dwTimeoutMax;		/* Input  parameter: Max-Timeout value */
    BYTE            ucExpcount;                   /*�������*/
    T_MtsEncDeviceInfo tExpanders[CMT_CLASS_LEVEL];  /*��������ͷ*/
}_PACKED_1_  T_MtsEncDeviceInfoList;

typedef struct
{
    BYTE id;     /* ����id */
    WORD16 flags;  /* ���Ա�ʶ���ɷ����õ�pre-fail��update��ʽ */
    BYTE currentvalue; /* ���Ե�ǰֵ */
    BYTE worst;   /* �����������ֵ��ֵ */
    WORD64 raw;  /* ������ʵֵ */
    BYTE thres;  /* ��������ֵ */
}_PACKED_1_ T_MtsDmPdSmartDetailInfo;

/*���̼����Ϣ*/
typedef struct
{
    WORD32	  dwTimeoutMax;		/* Input  parameter: Max-Timeout value */
    WORD64   uqwSasAddr;
    WORD32 type;
      /*Ӳ������, �������к�*/
    CHAR cSerialNum[MAX_PD_SNO_LEN];

    /*RAID��һ���Լ��*/

    /*Ӳ��SMART��Ϣ*/
    //T_HddSMARTInfo tHddSMARTInfo;
    BYTE ucIsSupportSmart;  /* �����Ƿ�֧��SMART */
    BYTE ucIsSmartEnable;   /* ����SMART�����Ƿ��� */
    BYTE ucSmartStatus;     /* ���̵�SMART״̬�Ƿ�OK */
    WORD32 PdSmartAttrNum;  /* ����SMART���Ը��� */
    T_MtsDmPdSmartDetailInfo tSmartDetailInfo[30];
    /*Ӳ�̶�����*/
    WORD32 fReadSpeed;

    /*Ӳ��д����*/
    WORD32 fWriteSpeed;

    /*Ӳ�̹̼��汾*/
    CHAR cFwVer[MAX_PD_FW_VER_LEN];

    /*Ӳ�̱�׼�汾*/

    /*Ӳ��Model�汾*/
    CHAR cModel[MAX_PD_MODEL_NAME_LEN];

    /*Ӳ�̳�����Ϣ*/
    CHAR cManufacture[64];
    CHAR  ucStdVerNum[MAX_PD_STD_VER_LEN];
    /*Ӳ���ͺ�*/
    BYTE     ucPhyType[MAX_PD_TYPE_LEN];

    /*Ӳ������, ��λ����(512�ֽ�)*/
    WORD64 uCapacity;

    /*Ӳ�̻�����Ŀ*/
    SWORD32 sBadBlocks;
    SWORD32 scanPercentage;

    /*�߼������޸�����*/
    BYTE ucBadBlockflag;
    /*Ӳ����λ��Ϣ, HDD_PRESENT��ʾ��λ, HDD_ABSENT��ʾ����λ*/
    WORD32 wPresentFlag;
}_PACKED_1_  T_MtsDiskInfo;

/* RTCʵʱʱ�ӵ�·���� */
typedef  struct
{
	BYTE    ucChassisId; 			/* ����� */
	BYTE	cCheckResult;			/* ���Σ�MTS_RTC_GOOD		 ����ͨ��
									 MTS_RTC_NOT_GOOD	 ���Բ�ͨ�� */
}_PACKED_1_ T_Cmt_Check_Rtc_Info;


typedef struct
{
	T_Cmt_Check_Rtc_Info	    tCmtCheckRtcInfo[2];
}_PACKED_1_ T_CmtRtcInfo;

typedef  struct
{
	WORD32			dwTimeoutMax;		/* Input  parameter: Max-Timeout value */
	WORD32  		dwEncNum;           /* �������,��СΪ1,���Ϊ5����չ����û������EPLD�����ֶα��� */
	T_CmtRtcInfo    tCheckRtcInfo[CMT_CLASS_LEVEL];
}_PACKED_1_ T_RtcInfoList;

/* GE����·���ʣ��Ƿ�˫�ز��� */
typedef  struct
{
	BYTE    ucChassisId; 			/* ����� */
	BYTE	cCheckResult[4];        /* ���Σ����Խ��:MTS_GE_GOOD ����ͨ��; MTS_GE_NOT_GOOD ���Բ�ͨ�� */
}_PACKED_1_ T_Cmt_Check_Ge_Info;

typedef struct
{
	T_Cmt_Check_Ge_Info	    tCmtCheckGeInfo[2];
}_PACKED_1_ T_CmtGeInfo;

typedef  struct
{
	WORD32			dwTimeoutMax;		/* Input  parameter: Max-Timeout value */
	WORD32  		dwEncNum;           /* �������,��СΪ1,���Ϊ5����չ����û������EPLD�����ֶα��� */
	T_CmtGeInfo     tCheckGeInfo[CMT_CLASS_LEVEL];
}_PACKED_1_ T_GeInfoList;

/* PCIE���� */
typedef  struct
{
	BYTE    ucChassisId; 		/* ����� */
    WORD32  dwPciePexId;       	/* PEX8624����ID */
	BYTE    cCheckResult;		/* PEX8624�Լ��� */
    WORD32  dwPcieLinkSpeed;    /* PCIE �������� G1/G2*/
    WORD32  dwPcieLinkWidth;    /* PCIE ���Ӵ���x1~x4 x0��ʾ��link */
    WORD32  dwPcieBadTlpCount; 	/* TLP�㻵������ */
    WORD32  dwPcieBadDllpCount;	/* DLLP�㻵������ */
    WORD32  dwPcieEepromStatus; /* PCIE �豸E2prom��λ�����Ч��� */
    WORD32  dwPcieDllpStatus;   /* DLLP ״̬ */
    BYTE    cPcieDeviceStatus;  /* PCIE�豸״̬ */
}_PACKED_1_ T_Cmt_Check_Pcie_Info;

typedef struct
{
	T_Cmt_Check_Pcie_Info	    tCmtCheckPcieInfo[2];
}_PACKED_1_ T_CmtPcieInfo;

typedef  struct
{
	WORD32			dwTimeoutMax;		/* Input  parameter: Max-Timeout value */
	WORD32  		dwEncNum;           /* �������,��СΪ1,���Ϊ5����չ����û������EPLD�����ֶα��� */
	T_CmtPcieInfo   tCheckPcieInfo[CMT_CLASS_LEVEL];
}_PACKED_1_ T_PcieInfoList;


/* PHY�Լ� */
typedef  struct
{
	BYTE    ucChassisId; 		/* ����� */
	//WORD16	wPhyDevId;		/*	��Σ��궨�� MTS_PHY_88E1111 �� MTS_PHY_88E1119 */
	WORD32	dwPhyId;			/* ���Σ�����ID */
	BYTE	cCheckResult[2];	/* ���Σ����Խ��:MTS_PHYSELF_GOOD ����ͨ��; MTS_PHYSELF_NOTGOOD ���Բ�ͨ�� */
}_PACKED_1_ T_Cmt_Check_Phy_Info;

typedef struct
{
	T_Cmt_Check_Phy_Info	    tCmtCheckPhyInfo[2];
}_PACKED_1_ T_CmtPhyInfo;

typedef  struct
{
	WORD32			dwTimeoutMax;		/* Input  parameter: Max-Timeout value */
	WORD32  		dwEncNum;           /* �������,��СΪ1,���Ϊ5����չ����û������EPLD�����ֶα��� */
	T_CmtPhyInfo    tCheckPhyInfo[CMT_CLASS_LEVEL];
}_PACKED_1_ T_PhyInfoList;

/* SAS������PM8001�Լ� */
typedef  struct
{
	BYTE    ucChassisId; 	/* ����� */
	WORD32	dwPmId; 	    /* ����: PM8001����ID */
	BYTE	cCheckResult;	/* ����: ����� */
}_PACKED_1_ T_Cmt_Check_Pm_Info;

typedef struct
{
	T_Cmt_Check_Pm_Info	    tCmtCheckPmInfo[2];
}_PACKED_1_ T_CmtPmInfo;

typedef  struct
{
	WORD32			dwTimeoutMax;		/* Input  parameter: Max-Timeout value */
	WORD32  		dwEncNum;           /* �������,��СΪ1,���Ϊ5����չ����û������EPLD�����ֶα��� */
	T_CmtPmInfo     tCheckPmInfo[CMT_CLASS_LEVEL];
}_PACKED_1_ T_PmInfoList;

/* BBU��� */
typedef  struct
{
	BYTE    ucChassisId; 			/* ����� */
    WORD32  dwBbuId;	          	/* BBU������к�     */
	WORD16  wBbuMv;					/* BBU��ǰ��ѹ       */
	WORD16  wBbuCapacity;			/* BBUʣ������ٷֱ� */
	BYTE    ucPresent;   			/* BBU��λ��Ϣ  MTS_BBU_PRESENT;  ��λ	        MTS_BBU_NOTPRESENT:  ����λ */
	BYTE    ucWorkFlag;    	  	  	/* �Ƿ���������	MTS_BBU_CHARGING: BBU���ڳ�ŵ� MTS_BBU_NOT_CHARGING:BBU��ʱû�г�ŵ� */
	BYTE    cCheckResult;         	/* BBU״̬ MTS_BBU_GOOD ���� MTS_BBU_GOOD������ */
}_PACKED_1_ T_Cmt_Check_Bbu_Info;

typedef struct
{
	T_Cmt_Check_Bbu_Info	    tCmtCheckBbuInfo[2];
}_PACKED_1_ T_CmtBbuInfo;

typedef  struct
{
	WORD32			dwTimeoutMax;		/* Input  parameter: Max-Timeout value */
	WORD32  		dwEncNum;           /* �������,��СΪ1,���Ϊ5����չ����û������EPLD�����ֶα��� */
	T_CmtBbuInfo    tCheckBbuInfo[CMT_CLASS_LEVEL];
}_PACKED_1_ T_BbuInfoList;


/* MINISAS�ͱ���SAS�ӿڲ���0~3����MINIsas����,11~19������SAS���� */
typedef  struct
{
	BYTE    ucChassisId; 			/* ����� */
	CHAR    ucDiskType[12][MAX_PD_TYPE_LEN];
   	BYTE	cCheckResult[36];		/* Ӳ��SAS��·�ٶ� */
}_PACKED_1_ T_Cmt_Check_Sas_Info;

typedef struct
{
	T_Cmt_Check_Sas_Info	    tCmtCheckSasInfo[2];
}_PACKED_1_ T_CmtSasInfo;

typedef  struct
{
	WORD32			dwTimeoutMax;		/* Input  parameter: Max-Timeout value */
	WORD32  		dwEncNum;           /* �������,��СΪ1,���Ϊ5����չ����û������EPLD�����ֶα��� */
	T_CmtSasInfo    tCheckSasInfo[CMT_CLASS_LEVEL];
}_PACKED_1_ T_SasInfoList;


/* HardDisk �Լ� */
typedef  struct
{
	BYTE     ucChassisId; 								/* �����       */
	SWORD32  sdwHddSoltId;								/* Ӳ�̲�λ��   */
	CHAR     ucSerialNo[MAX_PD_SNO_LEN];				/* Ӳ�����к�   */
	SWORD64  sdwHddCapcity;								/* Ӳ������     */
	CHAR     ucStdVerNum[MAX_PD_STD_VER_LEN];			/* ��׼�汾��   */
	CHAR     ucFwVer[MAX_PD_FW_VER_LEN];				/* FirmWare�汾 */
	CHAR     ucModelName[MAX_PD_MODEL_NAME_LEN];		/* Model����    */
	SWORD32  sdwHealthState;							/* ���̵Ľ���״̬�������ɷ����Ժ�SMART ״̬��ȡֵΪPD_GOOD(1)��PD_BROKEN(0)     */
}_PACKED_1_ T_Cmt_Check_Hdd_Info;

typedef struct
{
	T_Cmt_Check_Hdd_Info	    tCmtCheckHddInfo[16];
}_PACKED_1_ T_CmtHddInfo;

typedef  struct
{
	WORD32			dwTimeoutMax;		/* Input  parameter: Max-Timeout value */
	WORD32  		dwEncNum;           /* �������,��СΪ1,���Ϊ5����չ����û������EPLD�����ֶα��� */
	T_CmtHddInfo    tCheckHddInfo[CMT_CLASS_LEVEL];
}_PACKED_1_ T_HddInfoList;

/* �汾��� */
typedef  struct
{
	BYTE    ucChassisId; 		/*  �����       	   */
	SWORD32 sdwVerType;         /*  version   type     */
	WORD32  dwWorkEpldVer;      /*  work epld version  */
	WORD32  dwBootEpldVer;      /*  boot epld version  */
	CHAR    dwZbootVer[128];    /*  Zboot     version  */
	CHAR    dwUspVer[128];      /*  USP       version  */
	CHAR    dwExpVer[128];      /*  8005      version  */
}_PACKED_1_ T_Cmt_Check_Ver_Info;

typedef struct
{
	T_Cmt_Check_Ver_Info	    tCmtCheckVerInfo[2];
}_PACKED_1_ T_CmtVerInfo;

typedef  struct
{
	WORD32			dwTimeoutMax;		/* Input  parameter: Max-Timeout value */
	WORD32  		dwEncNum;           /* �������,��СΪ1,���Ϊ5����չ����û������EPLD�����ֶα��� */
	T_CmtVerInfo    tCheckVerInfo[CMT_CLASS_LEVEL];
}_PACKED_1_ T_VerInfoList;

/* �ڴ��� */
typedef  struct
{
	BYTE    ucChassisId; 		/*  �����         */
	WORD32  dwDdrSize;          /* �ڴ��С,��λ:M */
	BYTE    cCoherenceResult;   /* ����һ���Բ���  */
	BYTE    cCorrectResult;     /* ������ȷ�Բ���  */
}_PACKED_1_ T_Cmt_Check_Ddr_Info;

typedef struct
{
	T_Cmt_Check_Ddr_Info	    tCmtCheckDdrInfo[2];
}_PACKED_1_ T_CmtDdrInfo;

typedef  struct
{
	WORD32			dwTimeoutMax;		/* Input  parameter: Max-Timeout value */
	WORD32  		dwEncNum;           /* �������,��СΪ1,���Ϊ5����չ����û������EPLD�����ֶα��� */
	T_CmtDdrInfo    tCheckDdrInfo[CMT_CLASS_LEVEL];
}_PACKED_1_ T_DdrInfoList;

/* MAC��ַ */
typedef  struct
{
	BYTE    ucChassisId; 	/*  �����  */
	BYTE    cMacAddr[128];
}_PACKED_1_ T_Cmt_Check_MacAddr_Info;

typedef struct
{
	T_Cmt_Check_MacAddr_Info	    tCmtCheckMacAddrInfo[2];
}_PACKED_1_ T_CmtMacAddrInfo;

typedef  struct
{
	WORD32			dwTimeoutMax;		/* Input  parameter: Max-Timeout value */
	WORD32  		dwEncNum;           /* �������,��СΪ1,���Ϊ5����չ����û������EPLD�����ֶα��� */
	T_CmtMacAddrInfo   tCheckMacAddrInfo[CMT_CLASS_LEVEL];
}_PACKED_1_ T_MacAddrInfoList;

/* ���Ի������ */
typedef  struct
{
	BYTE    ucChassisId; 	/*  �����  */
	SWORD32 sdwPeerState;	/*  �Զ˵����״̬ */
}_PACKED_1_ T_Cmt_Check_PeerState_Info;

typedef struct
{
	T_Cmt_Check_PeerState_Info	    tCmtCheckPeerStateInfo[2];
}_PACKED_1_ T_CmtPeerStateInfo;

typedef  struct
{
	WORD32			  dwTimeoutMax;		 /* Input  parameter: Max-Timeout value */
	WORD32  		  dwEncNum;          /* �������,��СΪ1,���Ϊ5����չ����û������EPLD�����ֶα��� */
	T_CmtPeerStateInfo   tCheckPeerStateInfo[CMT_CLASS_LEVEL];
}_PACKED_1_ T_PeerStateInfoList;

/* SAS��ַ����, 8005�Լ� */
typedef  struct
{
	BYTE    ucChassisId; 	/*  �����  */
	BYTE    ucSasAddr[8];	/*  SAS��ַ */
}_PACKED_1_ T_Cmt_Check_SasAddr_Info;

typedef struct
{
	T_Cmt_Check_SasAddr_Info	    tCmtCheckSasAddrInfo[2];
}_PACKED_1_ T_CmtSasAddrInfo;

typedef  struct
{
	WORD32			  dwTimeoutMax;		 /* Input  parameter: Max-Timeout value */
	WORD32  		  dwEncNum;          /* �������,��СΪ1,���Ϊ5����չ����û������EPLD�����ֶα��� */
	T_CmtSasAddrInfo   tCheckSasAddrInfo[CMT_CLASS_LEVEL];
}_PACKED_1_ T_SasAddrInfoList;

/***********************************��������End****************************************/
/***********************************spr11 ������****************************************/
/* CPU���DDR3���� */
typedef struct
{
	WORD16  wMtsType;      /* ��Σ���������:MTS_FT,  ���ܲ���:  MTS_HT,       ����       */
	WORD32     CheckResult;  /* ���Σ����Խ��:MTS_DDR_GOOD ����ͨ��;  MTS_DDR_NOT_GOOD  ���Բ�ͨ�� */

	WORD32  dwTimeoutMax;  /* ��ʱ���ֵ */
}_PACKED_1_ T_Mts_SPR11_Check_Ddr;
/* �ڴ��С���� */
typedef struct
{
	WORD32  dwDdrSize;          /* �ڴ��С,��λ:M */
	BYTE    cCoherenceResult;   /* ����һ���Բ��� */
	BYTE    cCorrectResult;     /* ������ȷ�Բ��� */
	WORD32     cMemResult;     /* �ڴ�����У��*/
       WORD32     cCpuResult;     /* CPU��˹��ܼ��*/
	WORD32	dwTimeoutMax;		/* ��ʱ���ֵ */
}_PACKED_1_ T_Mts_SPR11_Check_Ddr_Size;

typedef struct
{
	WORD32  dwDdrSize[2];          /* �ڴ��С,��λ:M */
	WORD32	dwTimeoutMax;		/* ��ʱ���ֵ */
}_PACKED_1_ T_Cmt_SPR11_Check_Ddr_Size;

typedef struct
{
	WORD32	dwTimeoutMax;		/* ��ʱ���ֵ */
	WORD32     CheckResult;
}_PACKED_1_ T_Cmt_SPR11_Check_Hb;

/* ����EPLD�Լ� */
typedef struct
{
    WORD32 dwStandardValue;   /* ��׼ֵ  */
	WORD32 dwReadValue;       /* ����ֵ  */
	WORD32 dwCheckResult;     /* �ж�ֵ  */
	WORD32 dwTimeoutMax;      /* ��ʱ���ֵ */
}_PACKED_1_ T_Mts_SPR11_Check_Work_Epld_Self;

/* MAC��ַ */
typedef  struct
{
    BYTE    cMacAddr[6];
}_PACKED_1_ T_Mts_SPR11_MacAddr;

/* MAC��ַ */
typedef  struct
{
    BYTE ucPortNum;
    BYTE ucPortType;
    T_Mts_SPR11_MacAddr    MacAddr[8];
    WORD32	dwTimeoutMax;
}_PACKED_1_ T_Mts_SPR11_Check_MacAddr;

typedef  struct
{
    WORD32	dwTimeoutMax;
    T_Mts_SPR11_Check_MacAddr    MacAddrs[2];

}_PACKED_1_ T_Cmt_SPR11_Check_MacAddr;
/* MAC��ַ */
typedef  struct
{
    WORD32     type;
    WORD32     dwipaddr;
    WORD32     dwResult;
    WORD32	dwTimeoutMax;
}_PACKED_1_ T_Mts_SPR11_Check_Macband;

/* �¶ȴ��������� */
typedef struct
{
   /* �����±�0����LM_93  1����75_1, 2����75_2, 3����75_2 4����75_3 */
   BYTE    cSenorStatus[4];  /* ����; MTS_SENOR_GOOD����  MTS_SENOR_NOT_GOOD ������ */
   SWORD32 sdwSenorValue[4]; /* ����; ��������ʵ���¶� */
   BYTE    cEncDevId;          /* ��չ��� */
   WORD32  dwTimeoutMax;     /* ��ʱ���ֵ */
}_PACKED_1_ T_Mts_SPR11_Check_Senor;


/* BBU��Ϣ��ȡ */
typedef struct
{
    WORD32  dwBbuId;	    /* BBU������к�     */
	WORD16  wBbuMv;			/* BBU��ǰ��ѹ       */
	WORD16  wBbuCapacity;	/* BBUʣ������ٷֱ� */
	BYTE    ucPresent;   	/* BBU��λ��Ϣ  MTS_BBU_PRESENT;  ��λ	        MTS_BBU_NOTPRESENT:  ����λ */
	BYTE    ucWorkFlag;    	/* �Ƿ���������	MTS_BBU_CHARGING: BBU���ڳ�ŵ� MTS_BBU_NOT_CHARGING:BBU��ʱû�г�ŵ� */
	BYTE    cCheckResult;   /* BBU״̬ MTS_BBU_GOOD ���� MTS_BBU_GOOD������ */
    WORD32  dwTimeoutMax;   /* ��ʱ���ֵ */
}_PACKED_1_ T_Mt_SPR11_Check_Bbu;

/* �¶ȴ��������� */
typedef struct
{
   BYTE    cType;
   BYTE    cSenorStatus[40];  /* ����; PHY����*/
   BYTE    cEncDevId;          /* ��չ��� */
   WORD32  dwTimeoutMax;     /* ��ʱ���ֵ */
}_PACKED_1_ T_Mts_SPR11_SASLINK;

/* RTCʵʱʱ�ӵ�·���� */
typedef struct
{
   BYTE    cCheckResult;   /* ���Σ�MTS_RTC_GOOD 	    ����ͨ��
                                    MTS_RTC_NOT_GOOD	���Բ�ͨ�� */
   WORD32  dwTimeoutMax;   /* ��ʱ���ֵ */
}_PACKED_1_ T_Mts_SPR11_Check_Rtc;

/* ����ʱ�ӵ�·���� */
typedef struct
{
   WORD16  wMtsClockType;  /* ��Σ�MTS_25M_CLOCK, MTS_33_CLOCK...*/
   BYTE    cCheckResult;   /* ���Σ�MTS_CLOCK_GOOD 	    ����ͨ��
                                   MTS_CLOCK_NOT_GOOD	���Բ�ͨ��*/
   WORD32  dwTimeoutMax;   /* ��ʱ���ֵ */
}_PACKED_1_ T_Mts_SPR11_Check_Clock;

/* Ӳ��ָʾ�Ʋ��� */
typedef struct
{
	BYTE	cMtsType;		    /* type */
	WORD32  dwLightMode;     /* ���ģʽ��ԴOR DISK OR ���*/
	WORD32  dwLightType;     /* color*/
       WORD32  dwIndexId;       /* ������ 1~12*/
	WORD32	dwTimeoutMax;		/* ��ʱ���ֵ */
}_PACKED_1_ T_Mts_SPR11_Check_LED_Light;

/* ����EPLD��Ϣ��ȡ */
typedef struct
{
      WORD32 dwBoardId;         /* ����ID        */
	WORD32 dwPcbVer;          /* ����pcb�汾�� */
	WORD32 dwBomVer;          /* ����bom�ϵ�   */
	BYTE    ucSlotId;               /*slotid*/
	WORD32  dwBackBoardTypeId;/*����ID*/
	WORD32 dwTimeoutMax;      /* ��ʱ���ֵ */
}_PACKED_1_ T_Mts_SPR11_Check_Work_Epld_Info;

/* ����EPLD��Ϣ��ȡ */
typedef struct
{

    WORD32 dwcpuid;         /* CPUid*/
	WORD32 dwsbrigeid;          /* ����оƬID */
	WORD32 mac350id;          /* 350����ID*/
	WORD32 mac82580id;          /* 82580����ID*/
	  BYTE    cCheckResult[4];   /* ���Σ�MTS_CLOCK_GOOD 	    ����ͨ��
                                   MTS_CLOCK_NOT_GOOD	���Բ�ͨ��*/
	WORD32 dwTimeoutMax;      /* ��ʱ���ֵ */
}_PACKED_1_ T_Mts_SPR11_Check_Chips;

/* ����EPLD��Ϣ��ȡ */
typedef struct
{
	WORD32 dwTimeoutMax;      /* ��ʱ���ֵ */
	T_Mts_SPR11_Check_Chips chips[2];
}_PACKED_1_ T_Cmt_SPR11_Check_Chips;
/* ����������ɨ�� */
typedef struct
{
    BYTE   cLableInfo[MAX_INFO_LABLE_LENGTH]; /* ������ */
	WORD32 dwTimeoutMax;                      /* ��ʱ���ֵ */
}_PACKED_1_ T_Mts_SPR11_Check_Lable_Info;

/* �汾��ѯ */
typedef  struct
{
	BYTE	cEncDevId;	        //  expansion ID
	SWORD32 sdwVerType;         //  version   type
	WORD32  dwWorkEpldVer;      //  work epld version
	WORD32  dwBootEpldVer;      //  boot epld version
	CHAR    dwZbootVer[128];    //  Zboot     version
	CHAR    dwUspVer[128];      //  USP       version
	CHAR    dwExpVer[128];      //  8005      version
	WORD32	dwTimeoutMax;  //
}_PACKED_1_ T_Mts_SPR11_Check_Ver;

typedef struct
{
    BYTE  ifPortValid;                /* ��ʶ�ö˿��Ƿ����.    0 : �˿ڲ�����; 1 : �˿ڴ���.           */
    BYTE  PortID[FC_PORT_ID_LENGTH];  /* �˿�ID,FCЭ������,��ʾ��ʽΪ0x123456      */
    BYTE  PortNo;                     /* �˿ڱ��,��FC�ӿ����˳�����,��1��ʼ��� */

    BYTE  DataRate;                   /* �˿�����                                  */
                                               /* #define FC_SPEED_INVALID 255              */
                                               /* #define FC_SPEED_2G       2               */
                                               /* #define FC_SPEED_4G       4               */
                                               /* #define FC_SPEED_8G       8               */

    BYTE  LinkStatus;                 /* ��·״̬                                  */
                                               /* 0 : UP; 1 : DOWN.                         */

    BYTE  Topology;                   /* �˿�����                                  */
                                               /* FC�˿�����Ϊ˽�л�     0x01               */
                                               /* FC�˿�����Ϊ���л�     0x02               */
                                               /* FC�˿�����Ϊ��Ե�     0x04               */
                                               /* FC�˿�����ΪFabric     0x08               */
    BYTE   LinkLength;
    BYTE   SpeedCapacity;
    BYTE   TransType;
    BYTE   TxStatus;
    BYTE   LOSStatus;
    BYTE   FaultStatus;
    BYTE   ifSFPONLine;
    BYTE  WWPN[FC_WWN_LENGTH];        /* �˿�WWPN                                  */
    WORD16 deviceid;                   /* �豸id */
}_PACKED_1_ T_Mts_SPR11_FCPortInfo;


/* SCS��ȡFC�˿���Ϣ�����ṹ */
typedef  struct
{
    WORD32 CtrlNo;
    BYTE      WWNN[FC_WWN_LENGTH];         /* �˿����ڵ����WWNN               */
    T_Mts_SPR11_FCPortInfo FCPortInfo[FC_MAX_PORT_NUM]; /* FC�ӿ���Ϣ�ṹ                   */
    WORD32	dwTimeoutMax;  //
}_PACKED_1_ T_Mts_SPR11_FCinfo;

/* SCS��ȡFC�˿���Ϣ�����ṹ */
typedef  struct
{
    WORD32	dwTimeoutMax;  //
    T_Mts_SPR11_FCinfo fcinfo[2];
}_PACKED_1_ T_Cmt_SPR11_FCinfo;

typedef  struct
{
    WORD32  dwTimeoutMax;  //
    BYTE   encid;   
    WORD16 voltinfo[32];
}_PACKED_1_ T_Cmt_SPR11_Voltinfo;

typedef struct
{
    WORD32	dwTimeoutMax;
    WORD32 CtrlNo;                                  /* ��������,ֵΪ0��1    */
    WORD32 PortNo;                                  /* �˿ںţ�ֵΪ1-4      */

    BYTE   TxStatus;                                /* ���õĹ�ģ�鷢��״̬ */
    BYTE   ifSFPONLine;
    BYTE   ReservedOne;                             /* �����ֶ�             */
    WORD16 ReservedTwo;                             /* �����ֶ�             */
}_PACKED_1_ T_Cmt_FCSetMSTSFPTXStatus;

/* ���GE�ڻ��ز��� */
typedef struct
{
   WORD32 dwPortid;   /* ��Σ�FC�˿ں�1-4 */
   WORD32  wMtsType;       /* ��Σ���������:MTS_FT,  ���ܲ���: MTS_HT,      ����*/
   WORD32 dwSndFrameSet;
    WORD32 dwSndFrameCount; /* ���Σ�����֡���� */
    WORD32 dwRcvFrameCount; /* ���Σ��յ�֡���� */
    WORD32 dwFrameCheckErrCount; /* ���Σ��յ���֡��У���д�ĸ��� */
   WORD32  dwTimeoutMax;   /* ��ʱ���ֵ */
}_PACKED_1_ T_Mts_Check_FC;

/* FC���ʵƲ��� */
typedef struct
{
       WORD32	cPortid;		/* �˿�id*/
	WORD32	cLedid;		/* LEDid*/
	WORD32	cLedStatus;		/* LED status*/
	WORD32 dwPreFlag[2];  /* ���Σ�����ǰ��������״̬��ֵͬ�� */
       WORD32 dwCurFlag[2];  /* ���Σ����ú�������Ƶ�״̬��ֵͬ�� */
	WORD32	dwTimeoutMax;		/* ��ʱ���ֵ */
}_PACKED_1_ T_Mts_Check_FC_LED;

/* ����EPLD�Լ� */
typedef struct
{
       WORD64 dwCapacity[2];/*��������*/
	BYTE aucPhyDevName[2][64]; /* Ӳ�������豸������ */
	BYTE aucModel[2][64];      /* Ӳ�̳������� */
	BYTE aucSerialNum[2][64];  /* Ӳ���豸���к� */
	WORD32 dwformatResult;       /* ��ʽ������  */
	WORD32 dwRWResult;     /* ��д����  */
       WORD32 dwSmartResult;     /* SMART�����  */
	WORD32 dwTimeoutMax;      /* ��ʱ���ֵ */
	WORD32 dwtype;      /* �������� */
}_PACKED_1_ T_Mts_SPR11_SSD;

/* ����EPLD�Լ� */
typedef struct
{
	WORD32 dwTimeoutMax;      /* ��ʱ���ֵ */
	T_Mts_SPR11_SSD ssdinfo[2];
}_PACKED_1_ T_Cmt_SPR11_SSD;
/* ����EPLD�Լ� */
typedef struct
{
       WORD32  dwPhyNum[2];     /* SAS�������˿ں� */
	WORD64 dwCapacity[2]; /* Ӳ������ */
	BYTE aucPhyDevName[2][64]; /* Ӳ�������豸������ */
	BYTE aucModel[2][64];      /* Ӳ�̳������� */
	BYTE aucSerialNum[2][64];  /* Ӳ���豸���к� */
	BYTE aucFirmVer[2][64];	/* Ӳ�̹̼��汾�� */
	WORD32 dwTimeoutMax;      /* ��ʱ���ֵ */
}_PACKED_1_ T_Mts_SPR11_USB;

/* ��λ�źŲ��� */
typedef struct
{
	WORD32   dwOnlieFlag;        /* ��λ��־ */
	WORD32   dwTimeoutMax;      /* ��ʱ���ֵ */
}_PACKED_1_ T_Mts_SPR11_Online_Flag;

/* ��λ�źŲ��� */
typedef struct
{
	WORD32   dwResult;        /* �Ƿ�����*/
	WORD32  wMtsType;       /* ���*/
	WORD32   dwTimeoutMax;      /* ��ʱ���ֵ */
}_PACKED_1_ T_Mts_SPR11_ChipCLOCK;

/* ��λ�źŲ��� */
typedef struct
{
	WORD32   dwTimeoutMax;      /* ��ʱ���ֵ */
	T_Mts_SPR11_ChipCLOCK chiplock[2];
}_PACKED_1_ T_Cmt_SPR11_ChipCLOCK;

/* ��λ�źŲ��� */
typedef struct
{
	WORD32   dwResult;        /* �Ƿ�����*/
	WORD32   dwTimeoutMax;      /* ��ʱ���ֵ */
}_PACKED_1_ T_Mts_SPR11_CHECKCOMMON;

/* ��λ�źŲ��� */
typedef struct
{
	WORD32   dwResult;        /* �Ƿ�����*/
	WORD32  wMtsType;       /* ��Σ�0 ����IO�����1 ����IO���룬2�԰����µ�3������λ��Ϣ*/
	WORD32   dwPsID;        /* ��ԴID*/
	WORD32   dwSwitch;        /* ����*/
	WORD32  dwSvrOp;
	WORD16  wPowerCtrl;
	BYTE ucDiskSlot[12];
	BYTE ucOnlineFlag[12];
	WORD32   dwTimeoutMax;      /* ��ʱ���ֵ */
}_PACKED_1_ T_Mts_SPR11_CHECKBACKPLANEIO;


typedef struct
{
    WORD32 sasAddressHi[2];       /* Expander SAS��ַ����32λ */
    WORD32 sasAddressLow[2];      /* Expander SAS��ַ����32λ */
    WORD32 chassisId;             /* Expander ����ID */
}_PACKED_1_ T_Mts_DspamExpanderInfo;

typedef struct
{
    WORD32 sasBaseAddressHi;      /* SAS����ַ����32λ����� */
    WORD32 sasBaseAddressLow;     /* SAS����ַ����32λ����� */
    WORD32 sasEnclosureNum;       /* ������������ */
    WORD32 sasDspamCardNum;       /* ���ս��ӿ���Ŀ����� */
    WORD32 sasDspamOptCode;       /* ��������, ��� */
    WORD32 sasDspamResult;        /* �ս���, ���� */
    T_Mts_DspamExpanderInfo tDspamExpanderInfo[6]; /* ϵͳ��Expander��Ϣ����� */
}_PACKED_1_ T_Mts_DspamUpdateSasAddrInfo;

/* ��λ�źŲ��� */
typedef struct
{
    WORD32   dwTimeoutMax;        /* �Ƿ�����*/
    T_Mts_DspamUpdateSasAddrInfo DspamInfo;
}_PACKED_1_ T_Mts_SPR11_DSPAM;

/***********************************spr11 ������end****************************************/


/*VPD FRU��Ϣ*/
typedef struct
{
    SWORD32  sdwEnclosureId;//����ID
    WORD16 uwBackplaneID;//����ID
    WORD16 uwChassisType;// ChassisType
    CHAR    ucChassisPartNumber[36];
    CHAR    ucChassisSerialNumber[36];
    BYTE    ucLanguageCode;
    BYTE    ucManufacturingDate[3];
    CHAR    ucBoardManufacturer[36];
    CHAR    ucBoardProductName[36];
    CHAR    ucBoardSerialNumber[36];
    CHAR    ucBoardPartNumber[36];
    CHAR    ucVersion[36];
    BYTE    ucBomid;
    BYTE    ucPcbversion;
    CHAR    ucIncumbentCanisterID[24];
    WORD16 uwQualifiedDriveInterfaces;
    WORD16 uwQualifiedIntercanister;
    BYTE    ucNumberSBBCanisterSlots;
    BYTE    ucVentConfiguration1[4];
    BYTE    ucVentConfiguration2[4];
    BYTE    ucVentConfiguration3[4];
    BYTE    ucVentConfiguration4[4];
    BYTE    ucSBBSlotAMax12VCurrent;
    BYTE    ucSBBSlotBMax12VCurrent;
    BYTE    ucNumberPSSupplySlots;
    BYTE    ucMaxDrivespinup;
    BYTE    ucReducedPSMaxspinup;
    WORD16 uwPSVPDAddrpssupply1;
    WORD16 uwPSVPDAddrpssupply2;
    BYTE    ucPSSupply1CtlTWIAddress;
    BYTE    ucPSSupply1CtlTWIBusProtocol;
    BYTE    ucPSSupply2CtlTWIAddress;
    BYTE    ucPSSupply2CtlTWIBusProtocol;
    BYTE    ucSBBSlotAPortMapTable[12];
    BYTE    ucSBBSlotBPortMapTable[12];
    CHAR    ucSBBSignalUsage[36];
    CHAR    ucSBBSGPIOUsage[82];
    CHAR    ucSBBVendorData[36];
}_PACKED_1_ T_VPD;
/**********************************************************************
* �������ƣ�GetSvrSocket
* ���������
* ���������ptSvrSoc
* �� �� ֵ��    0       -   ִ�гɹ�             ��0 -   ִ��ʧ��
* ����˵������
***********************************************************************/
SWORD32 GetSvrSocket(T_SvrSocket *ptSvrSoc);
/**********************************************************************
* �������ƣ�SetSvrSocket
* ���������ptSvrSoc
* ���������
* �� �� ֵ��    0       -   ִ�гɹ�             ��0 -   ִ��ʧ��
* ����˵������
***********************************************************************/
SWORD32 SetSvrSocket(T_SvrSocket *ptSvrSoc);


/* FC�Ự��� */
/* Ŀ�Ķ˿ڵ���Ϣ�ṹ */
typedef struct T_FCObjProtocolInfo
{
    BYTE   ucPortName[FC_WWN_LENGTH];/* �Զ˵Ķ˿���                               */
    BYTE   ucNodeName[FC_WWN_LENGTH]; /* �Զ˵Ľڵ���                             */
    BYTE   ucifTgtFun;                    /* �Զ��Ƿ����TARGET����.    0:���߱�;1:�߱� */
    BYTE   ucifIniFun;                     /* �Զ��Ƿ����INITIATOR����. 0:���߱�;1:�߱� */
    WORD16 Reserved;                 /* �����ֶ�                                   */
}T_FCObjProtocolInfo;

/*�����˿ڵ�FCЭ����Ϣ�ṹ */
typedef struct T_GetFCProtocolIInfo
{
    SWORD32                   sdwCtrlId;                       /* ��������,ֵΪ0��1        */
    SWORD32                   sdwPortNo;
    SWORD32                   sdwValidNum;                  /* ��Ч�Ķ˿ڵ�¼��Ϣ��     */
    /* �˿ںţ�ֵΪ1-4          */
    T_FCObjProtocolInfo    tProtItem[FC_ALLOW_PLOGI_CNT];  /* �����˿ڵ�¼��Ϣ�ṹ���� */
}T_GetFCProtocolIInfo;


/* �����˿ڵ���·ͳ����Ϣ�ṹ */
typedef struct T_FCLinkStatisInfo
{

    WORD32  LinkUp; /* ��·UP����ͳ��                              */
    WORD32  LinkDown;/* ��·DOWN����ͳ��                           */
    WORD32  ORFECC; /* Outbound Reroute FIFO ECC Correction Count  */
    WORD32  LCFECC; /* Link Control FIFO ECC Correction Count      */
    WORD32  IECC;   /* IFM ECC Correction Count                    */
    WORD32  OECC;   /* OFM ECC Error Count                         */
    WORD32  OCRECC; /* OKey Cache RAM ECC Correction Count         */
    WORD32  MORECC; /* Multi Outbound Request ECC Correction Count */
    WORD32  SCECC;  /* SEST Cache ECC Correction Count             */

    WORD32  RFOC;   /* Reroute FIFO Overflow Count                 */
    WORD32  RPEC;   /* Reroute Parity Error Count                  */
    WORD32  RCEC;   /* Reroute CRC Error Count                     */
    WORD32  SLC;    /* SEST LRU Count                              */
    WORD32  DFC;    /* Discarded Frame Counter                     */
    WORD32  LOS;    /* Loss of Signal Count                        */
    WORD32  BRCC;   /* Bad Received Character Count                */
    WORD32  LOSC;   /* Loss of Synchronization Count               */
    WORD32  LFC;    /* Link Fail Count                             */

    WORD32  REOFa;  /* Received EOFa                               */
    WORD32  DF;     /* Discarded Frames                            */
    WORD32  BCC;    /* Bad CRC Count                               */
    WORD32  PEC;    /* Protocol Error Count                        */
    WORD32  EF;     /* Expired Frames                              */

}T_FCLinkStatisInfo;


/* �����˿ڵĻ�������ҵ��ͳ����Ϣ�ṹ */
typedef struct T_FCPortBLSStatisInfo
{
    WORD32  ABTSSendCnt;     /* ����ABTS����ͳ��                  */
    WORD32  ABTSACCRcvCnt;   /* ����ABTS BA_ACCEPT����ͳ��        */
    WORD32  ABTSRJTRcvCnt;   /* ����ABTS BA_REJECT����ͳ��        */

    WORD32  ABTSRcvCnt;      /* ����ABTS����ͳ��                  */
    WORD32  ABTSACCSendCnt;  /* ����ABTS BA_ACCEPT����ͳ��        */
    WORD32  ABTSRJTSendCnt;  /* ����ABTS BA_REJECT����ͳ��        */

    WORD32  NOPSendCnt;      /* ����NOP����ͳ��                   */
    WORD32  NOPACCRcvCnt;    /* ����NOP BA_ACCEPT����ͳ��         */
    WORD32  NOPRJTRcvCnt;    /* ����NOP BA_REJECT����ͳ��         */

    WORD32  NOPRcvCnt;       /* ����NOP����ͳ��                   */
    WORD32  NOPACCSendCnt;   /* ����NOP BA_ACCEPT����ͳ��         */
    WORD32  NOPRJTSendCnt;   /* ����NOP BA_REJECT����ͳ��         */

    WORD32  OtherACCRcvCnt;  /* ���շ�ABST��NOP BA_ACCEPT����ͳ�� */
    WORD32  OtherACCSendCnt; /* ���ͷ�ABST��NOP BA_ACCEPT����ͳ�� */
    WORD32  OtherRJTRcvCnt;  /* ���շ�ABST��NOP BA_REJECT����ͳ�� */
    WORD32  OtherRJTSendCnt; /* ���ͷ�ABST��NOP BA_REJECT����ͳ�� */
}T_FCPortBLSStatisInfo;


/* �����˿ڵ���չ����ҵ��ͳ����Ϣ�ṹ */
typedef struct T_FCPortELSStatisInfo
{
    WORD32  FLOGISendCnt;      /* ����FLOGI����ͳ��           */
    WORD32  FLOGIACCRcvCnt;    /* ����FLOGI LS_ACCEPT����ͳ�� */
    WORD32  FLOGIRJTRcvCnt;    /* ����FLOGI LS_REJECT����ͳ�� */

    WORD32  FLOGIRcvCnt;       /* ����FLOGI����ͳ��           */
    WORD32  FLOGIACCSendCnt;   /* ����FLOGI LS_ACCEPT����ͳ�� */
    WORD32  FLOGIRJTSendCnt;   /* ����FLOGI LS_REJECT����ͳ�� */

    WORD32  PLOGISendCnt;      /* ����PLOGI����ͳ��           */
    WORD32  PLOGIACCRcvCnt;    /* ����PLOGI LS_ACCEPT����ͳ�� */
    WORD32  PLOGIRJTRcvCnt;    /* ����PLOGI LS_REJECT����ͳ�� */

    WORD32  PLOGIRcvCnt;       /* ����PLOGI����ͳ��           */
    WORD32  PLOGIACCSendCnt;   /* ����PLOGI LS_ACCEPT����ͳ�� */
    WORD32  PLOGIRJTSendCnt;   /* ����PLOGI LS_REJECT����ͳ�� */

    WORD32  PRLISendCnt;       /* ����PRLI����ͳ��            */
    WORD32  PRLIACCRcvCnt;     /* ����PRLI LS_ACCEPT����ͳ��  */
    WORD32  PRLIRJTRcvCnt;     /* ����PRLI LS_REJECT����ͳ��  */

    WORD32  PRLIRcvCnt;        /* ����PRLI����ͳ��            */
    WORD32  PRLIACCSendCnt;    /* ����PRLI LS_ACCEPT����ͳ��  */
    WORD32  PRLIRJTSendCnt;    /* ����PRLI LS_REJECT����ͳ��  */

    WORD32  LOGOSendCnt;       /* ����LOGO����ͳ��            */
    WORD32  LOGOACCRcvCnt;     /* ����LOGO LS_ACCEPT����ͳ��  */
    WORD32  LOGORJTRcvCnt;     /* ����LOGO LS_REJECT����ͳ��  */

    WORD32  LOGORcvCnt;        /* ����LOGO����ͳ��            */
    WORD32  LOGOACCSendCnt;    /* ����LOGO LS_ACCEPT����ͳ��  */
    WORD32  LOGORJTSendCnt;    /* ����LOGO LS_REJECT����ͳ��  */

    WORD32  PRLOSendCnt;       /* ����PRLO����ͳ��            */
    WORD32  PRLOACCRcvCnt;     /* ����PRLO LS_ACCEPT����ͳ��  */
    WORD32  PRLORJTRcvCnt;     /* ����PRLO LS_REJECT����ͳ��  */

    WORD32  PRLORcvCnt;        /* ����PRLO����ͳ��            */
    WORD32  PRLOACCSendCnt;    /* ����PRLO LS_ACCEPT����ͳ��  */
    WORD32  PRLORJTSendCnt;    /* ����PRLO LS_REJECT����ͳ��  */

    WORD32  SCRSendCnt;        /* ����SCR����ͳ��             */
    WORD32  SCRACCRcvCnt;      /* ����SCR LS_ACCEPT����ͳ��   */
    WORD32  SCRRJTRcvCnt;      /* ����SCR LS_REJECT����ͳ��   */

    WORD32  SCRRcvCnt;         /* ����SCR����ͳ��             */
    WORD32  SCRACCSendCnt;     /* ����SCR LS_ACCEPT����ͳ��   */
    WORD32  SCRRJTSendCnt;     /* ����SCR LS_REJECT����ͳ��   */

    WORD32  RSCNSendCnt;       /* ����RSCN����ͳ��            */
    WORD32  RSCNACCRcvCnt;     /* ����RSCN LS_ACCEPT����ͳ��  */
    WORD32  RSCNRJTRcvCnt;     /* ����RSCN LS_REJECT����ͳ��  */

    WORD32  RSCNRcvCnt;        /* ����RSCN����ͳ��            */
    WORD32  RSCNACCSendCnt;    /* ����RSCN LS_ACCEPT����ͳ��  */
    WORD32  RSCNRJTSendCnt;    /* ����RSCN LS_REJECT����ͳ��  */

    WORD32  ECHOSendCnt;       /* ����ECHO����ͳ��            */
    WORD32  ECHOACCRcvCnt;     /* ����ECHO LS_ACCEPT����ͳ��  */
    WORD32  ECHORJTRcvCnt;     /* ����ECHO LS_REJECT����ͳ��  */

    WORD32  ECHORcvCnt;        /* ����ECHO����ͳ��            */
    WORD32  ECHOACCSendCnt;    /* ����ECHO LS_ACCEPT����ͳ��  */
    WORD32  ECHORJTSendCnt;    /* ����ECHO LS_REJECT����ͳ��  */

    WORD32  ADISCSendCnt;      /* ����ADISC����ͳ��           */
    WORD32  ADISCACCRcvCnt;    /* ����ADISC LS_ACCEPT����ͳ�� */
    WORD32  ADISCRJTRcvCnt;    /* ����ADISC LS_REJECT����ͳ�� */

    WORD32  ADISCRcvCnt;       /* ����ADISC����ͳ��           */
    WORD32  ADISCACCSendCnt;   /* ����ADISC LS_ACCEPT����ͳ�� */
    WORD32  ADISCRJTSendCnt;   /* ����ADISC LS_REJECT����ͳ�� */

    WORD32  PDISCSendCnt;      /* ����PDISC����ͳ��           */
    WORD32  PDISCACCRcvCnt;    /* ����PDISC LS_ACCEPT����ͳ�� */
    WORD32  PDISCRJTRcvCnt;    /* ����PDISC LS_REJECT����ͳ�� */

    WORD32  PDISCRcvCnt;       /* ����PDISC����ͳ��           */
    WORD32  PDISCACCSendCnt;   /* ����PDISC LS_ACCEPT����ͳ�� */
    WORD32  PDISCRJTSendCnt;   /* ����PDISC LS_REJECT����ͳ�� */


    WORD32  OtherELSRcvCnt;    /* ����ELS���ռ���ͳ��         */
    WORD32  OtherELSSendCnt;   /* ����ELS���ͼ���ͳ��         */
    WORD32  OtherLSACCRcvCnt;  /* ����LS_ACCEPT���ռ���ͳ��   */
    WORD32  OtherLSACCSendCnt; /* ����LS_ACCEPT���ͼ���ͳ��   */
    WORD32  OtherLSRJTRcvCnt;  /* ����LS_REJECT���ռ���ͳ��   */
    WORD32  OtherLSRJTSendCnt; /* ����LS_REJECT���ͼ���ͳ��   */
}T_FCPortELSStatisInfo;



/*�����˿ڵ�ͨ��ҵ��ͳ����Ϣ�ṹ */
typedef struct T_FCPortGSStatisInfo
{
    WORD32  GIDPNSendCnt;      /* ����GIDPN����ͳ��           */
    WORD32  GIDPNACCRcvCnt;    /* ����GIDPN CT_ACCEPT����ͳ�� */
    WORD32  GIDPNRJTRcvCnt;    /* ����GIDPN CT_REJECT����ͳ�� */

    WORD32  GPNIDSendCnt;      /* ����GPNID����ͳ��           */
    WORD32  GPNIDACCRcvCnt;    /* ����GPNID CT_ACCEPT����ͳ�� */
    WORD32  GPNIDRJTRcvCnt;    /* ����GPNID CT_REJECT����ͳ�� */

    WORD32  RNNIDSendCnt;      /* ����RNNID����ͳ��           */
    WORD32  RNNIDACCRcvCnt;    /* ����RNNID CT_ACCEPT����ͳ�� */
    WORD32  RNNIDRJTRcvCnt;    /* ����RNNID CT_REJECT����ͳ�� */

    WORD32  RFTIDSendCnt;      /* ����RFTID����ͳ��           */
    WORD32  RFTIDACCRcvCnt;    /* ����RFTID CT_ACCEPT����ͳ�� */
    WORD32  RFTIDRJTRcvCnt;    /* ����RFTID CT_REJECT����ͳ�� */

    WORD32  OtherGSRcvCnt;     /* ����GS���ռ���ͳ��          */
    WORD32  OtherGSSendCnt;    /* ����GS���ͼ���ͳ��          */
    WORD32  OtherCTACCRcvCnt;  /* ����CT_ACCEPT���ռ���ͳ��   */
    WORD32  OtherCTACCSendCnt; /* ����CT_ACCEPT���ͼ���ͳ��   */
    WORD32  OtherCTRJTRcvCnt;  /* ����CT_REJECT���ռ���ͳ��   */
    WORD32  OtherCTRJTSendCnt; /* ����CT_REJECT���ͼ���ͳ��   */
}T_FCPortGSStatisInfo;


/* �����˿ڵ�FCPҵ��ͳ����Ϣ�ṹ */
typedef struct T_FCPortFCPStatisInfo
{
    WORD64  CMDSendCnt;     /* FCP����ͼ���ͳ��             */
    WORD64  XFERSendCnt;    /* FCP XFER���ͼ���ͳ��            */
    WORD64  RESPSendCnt;    /* FCP RESP���ͼ���ͳ��            */

    WORD64  CMDRcvCnt;      /* FCP������ռ���ͳ��             */
    WORD64  DATARcvCnt;     /* FCP���ݽ��ռ���ͳ��            */
    WORD64  RESPRcvCnt;     /* FCP RESP���ռ���ͳ��            */

    WORD64  CMDExeSuccCnt;  /* ����ִ�гɹ�����ͳ��            */
    WORD64  CMDExeFailCnt;  /* ����ִ��ʧ�ܼ���ͳ��            */

}T_FCPortFCPStatisInfo;

/*�����˿ڵ�ͳ����Ϣ�ṹ */
typedef struct T_FCGetPortStatisInfo
{
    SWORD32                  CtrlId;     /* ��������,ֵΪ0��1 */
    SWORD32                  PortNo;     /* �˿ںţ�ֵΪ1-4   */

    T_FCLinkStatisInfo     tLinkStat;   /* ��·ͳ����Ϣ      */
    T_FCPortBLSStatisInfo  tBLSStat;    /* BLSͳ����Ϣ       */
    T_FCPortELSStatisInfo  tELSStat;    /* ELSͳ����Ϣ       */
    T_FCPortGSStatisInfo   tGSStat;     /* GSͳ����Ϣ  */
    T_FCPortFCPStatisInfo  tFCPStat;    /* FCPͳ����Ϣ  */
}T_FCGetPortStatisInfo;

/* �����˿ڵ�ͳ����Ϣ�ṹ */
typedef struct T_ClearFCPortStatisInfo
{
    SWORD32    CtrlId;         /* ��������,ֵΪ0��1               */
    SWORD32    PortNo;         /* �˿ںţ�ֵΪ1-4                 */
}T_ClearFCPortStatisInfo;
/*fc�Ự��Ϣ�ĳ���cli��web��ͬ�� ���½ṹ���ṩ��webʹ�� */

/* SCS��ȡ�����˿ڵ���·ͳ����Ϣ�ṹ */
typedef struct T_WebGetFCLinkStatisInf
{
    WORD32  LinkUp; /* ��·UP����ͳ��                              */
    WORD32  LinkDown;/* ��·DOWN����ͳ��                           */
}T_WebGetFCLinkStatisInf;


/* SCS��ȡ�����˿ڵĻ�������ҵ��ͳ����Ϣ�ṹ */
typedef struct T_WebGetFCPortBLSStatisInf
{
    WORD32  BLSFrameSendCnt;   /* ����BLS֡����        */
    WORD32  BLSACCRcvCnt;      /* ����BLS ACCEPT֡���� */
    WORD32  BLSPRJRcvCnt;      /* ����BLS REJECT֡���� */

    WORD32  BLSFrameRcvCnt;    /* ����BLS֡���� */
    WORD32  BLSACCSendCnt;     /* ����BLS ACCEPT֡���� */
    WORD32  BLSPRJSendCnt;     /* ����BLS REJECT֡���� */
}T_WebGetFCPortBLSStatisInf;


/* SCS��ȡWeb������ʾ�ĵ����˿ڵ���չ����ҵ��ͳ����Ϣ�ṹ */
typedef struct T_WebGetFCPortELSStatisInf
{
    WORD32  ELSFrameSendCnt;   /* ����ELS֡����        */
    WORD32  ELSACCRcvCnt;      /* ����ELS ACCEPT֡���� */
    WORD32  ELSPRJRcvCnt;      /* ����ELS REJECT֡���� */

    WORD32  ELSFrameRcvCnt;    /* ����ELS֡���� */
    WORD32  ELSACCSendCnt;     /* ����ELS ACCEPT֡���� */
    WORD32  ELSPRJSendCnt;     /* ����ELS REJECT֡���� */
}T_WebGetFCPortELSStatisInf;


/* SCS��ȡWeb������ʾ�ĵ����˿ڵ�ͨ��ҵ��ͳ����Ϣ�ṹ */
typedef struct T_WebGetFCPortGSStatisInf
{
    WORD32  GSFrameSendCnt;   /* ����GS֡����        */
    WORD32  GSACCRcvCnt;      /* ����GS ACCEPT֡���� */
    WORD32  GSPRJRcvCnt;      /* ����GS REJECT֡���� */

    WORD32  GSFrameRcvCnt;    /* ����GS֡���� */
    WORD32  GSACCSendCnt;     /* ����GS ACCEPT֡���� */
    WORD32  GSPRJSendCnt;     /* ����GS REJECT֡���� */
}T_WebGetFCPortGSStatisInf;

typedef struct T_WebGetFCPortStatisInf
{
    SWORD32                sdwCtrlId;           /* ��������,ֵΪ0��1 */
    SWORD32                sdwPortId;           /* �˿ںţ�ֵΪ1-4   */

    T_FCPortFCPStatisInfo       tFCPStat;    /* FCPͳ����Ϣ       */
    T_WebGetFCLinkStatisInf     tLinkStat;   /* ��·ͳ����Ϣ      */
    T_WebGetFCPortBLSStatisInf  tBLSStat;    /* BLSͳ����Ϣ       */
    T_WebGetFCPortELSStatisInf  tELSStat;    /* ELSͳ����Ϣ       */
    T_WebGetFCPortGSStatisInf   tGSStat;     /* GSͳ����Ϣ        */
}T_WebGetFCPortStatisInf;


/**
   @struct     tagSYS_CLOCK
   @brief      ϵͳ��ʱ�ӽṹ����ʾ����ʱ��
*/
typedef struct tagSYS_CLOCK
{
    WORD16  wSysYear;       /**< �� */
    BYTE    ucSysMon;       /**< �� */
    BYTE    ucSysDay;       /**< �� */

    BYTE    ucSysHour;      /**< ʱ */
    BYTE    ucPading0;      /**< �����*/
    BYTE    ucSysMin;       /**< �� */
    BYTE    ucSysSec;       /**< �� */

    WORD16  wMilliSec;      /**< ���� */
    BYTE    ucSysWeek;      /**< �� */
    BYTE    ucPading1;      /**< �����*/
}T_SysSoftClock;


typedef struct
{
    BYTE  ucChkUpdateUsp;   /* ��汾���¼�� */
} _PACKED_1_ T_UpdateUspChk;

/**
    @struct	T_Alarm_Req
    @brief	�����ϱ��澯
    <HR>
    @b �޸ļ�¼��
*/
typedef struct
{
/**
    @li @b	����			dwRecNo
    @li @b	����			����Ŀ�ʼ��¼��
*/
    WORD32               dwRecNo;
/**
    @li @b	����			dwReqNum
    @li @b	����			����ĸ澯��
*/
    WORD32               dwReqNum;
/**
    @li @b	����			dwLang
    @li @b	����			��������
<TABLE>
    <TR  Align=Center><TD>��������</TD><TD>�������ͺ���</TD></TR>
    <TR  Align=Center><TD>0</TD><TD>Ӣ��</TD></TR>
    <TR  Align=Center><TD>1</TD><TD>����</TD></TR>
</TABLE>
*/
    WORD32              dwLang;

    /*�����ֶ�Ϊ�����ֶΣ����Բ���ֵ*/

/**
    @li @b	����			dwAlarmCode
    @li @b	����			�澯��
*/
    WORD32             dwAlarmCode;
/**
    @li @b	����			acAlarmName
    @li @b	����			�澯����
*/
	CHAR                acAlarmName[ALARM_NAME_LENGTH];
/**
    @li @b	����			wAlarmLevel
    @li @b	����			�澯����
<TABLE>
    <TR  Align=Center><TD>�澯����</TD><TD>�澯��������</TD><TD>�澯����Ӣ��</TD></TR>
    <TR  Align=Center><TD>1</TD><TD>����</TD><TD>Critical</TD></TR>
    <TR  Align=Center><TD>2</TD><TD>��Ҫ</TD><TD>Major</TD></TR>
    <TR  Align=Center><TD>3</TD><TD>һ��</TD><TD>Minor</TD></TR>
    <TR  Align=Center><TD>4</TD><TD>��΢</TD><TD>Warning</TD></TR>
    <TR  Align=Center><TD>5</TD><TD>��ʾ</TD><TD>Notice</TD></TR>
    <TR  Align=Center><TD>������ֵ</TD><TD>δָ��</TD><TD>Unspecified</TD></TR>
</TABLE>

*/
    WORD16             wAlarmLevel;
/**
    @li @b	����			tAlarmTimeStart
    @li @b	����			�澯ʱ��ο�ʼʱ��
*/
    T_SysSoftClock    tAlarmTimeStart;
/**
    @li @b	����			tAlarmTimeEnd
    @li @b	����			�澯ʱ��ν���ʱ��
*/
    T_SysSoftClock    tAlarmTimeEnd;
}_PACKED_1_ T_Alarm_Req;

/**
    @struct T_AlarmAddInfo
    @brief  �澯/֪ͨ������Ϣ
    <HR>
    @b �޸ļ�¼��
*/
typedef struct
{
    CHAR aucAddInfo[ALARM_ADDINFO_UNION_MAX];   /* ������ϢΪһ���ַ������֡�WEBֱ����ʾ���� */
}_PACKED_1_ T_AlarmAddInfo;

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
    @li @b	����			�澯��
*/
    WORD32             dwAlarmCode;
/**
    @li @b	����			dwAlarmNo
    @li @b	����			�澯���
*/
    WORD32             dwAlarmNo;
/**
    @li @b	����			acAlarmName
    @li @b	����			�澯����
*/
	CHAR                acAlarmName[ALARM_NAME_LENGTH];
/**
    @li @b	����			ucAlarmFlag
    @li @b	����			�澯��־: 1-�澯;2-�ָ�
*/
    BYTE               ucAlarmFlag;
/**
    @li @b	����			aucPad
    @li @b	����			����ֽ�
*/
    CHAR              aucPad[3];
/**
    @li @b	����			wKeyLen
    @li @b	����			�澯������Ϣ�ṹ�ؼ��ֳ��� �������ݲ�ʹ��
*/
    WORD16             wKeyLen;
/**
    @li @b	����			wAlarmLevel
    @li @b	����			�澯����
<TABLE>
    <TR  Align=Center><TD>�澯����</TD><TD>�澯��������</TD><TD>�澯����Ӣ��</TD></TR>
    <TR  Align=Center><TD>1</TD><TD>����</TD><TD>Critical</TD></TR>
    <TR  Align=Center><TD>2</TD><TD>��Ҫ</TD><TD>Major</TD></TR>
    <TR  Align=Center><TD>3</TD><TD>һ��</TD><TD>Minor</TD></TR>
    <TR  Align=Center><TD>4</TD><TD>��΢</TD><TD>Warning</TD></TR>
    <TR  Align=Center><TD>5</TD><TD>��ʾ</TD><TD>Notice</TD></TR>
    <TR  Align=Center><TD>������ֵ</TD><TD>δָ��</TD><TD>Unspecified</TD></TR>
</TABLE>

*/
    WORD16             wAlarmLevel;

/**
    @li @b	����			tAlarmTime
    @li @b	����			�澯�ϱ�ʱ��
*/
    T_SysSoftClock    tAlarmTime;

/**
    @li @b	����			tAddInfo
    @li @b	����			�澯������
*/
    T_AlarmAddInfo    tAddInfo;
}_PACKED_1_ T_AlarmReport;


/**
    @struct	T_Agent_Pack_Alarm
    @brief	APP����OAM�⺯���ϱ��澯��Ϣ
    <HR>
    @b �޸ļ�¼��
*/
typedef struct
{
/**
    @li @b	����			wRetCode
    @li @b	����			������� 0���ɹ�
*/
    WORD16              wRetCode;
/**
    @li @b	����			ucAlarmNum
    @li @b	����			�����и澯���߻ָ��ĸ���
*/
    BYTE               ucAlarmNum;
/**
    @li @b	����			ucLastPacket 0:�����һ��.1:���һ��
    @li @b	����			�Ƿ����һ��
*/
    BYTE              ucLastPacket;
/**
    @li @b	����			wTotalAlarmNum
    @li @b	����			�ܸ澯��
*/
    WORD16              wTotalAlarmNum;
/**
    @li @b	����			tAlarmAddr
    @li @b	����			����澯����λ��

    T_AlarmAddress     tAlarmAddr;
*/
/**
    @li @b	����			tAlarmReport
    @li @b	����			�澯�ϱ��ĸ澯ʵ��
*/
    T_AlarmReport      tAlarmReport[ALARM_PER_PACKET_MAX];
}_PACKED_1_ T_Alarm_Packet_Ack;

 /**
   @struct T_Alarm_Del_Alarm
   @brief ɾ��ָ���澯

   @li @b ���� ɾ��ָ���澯

   <HR>
   @b �޸ļ�¼��

 */
typedef struct
{
/**
    @li @b	����			dwAlarmCode
    @li @b	����			�澯��
*/
    WORD32             dwAlarmCode;
/**
    @li @b	����			dwAlarmNo
    @li @b	����			�澯���
*/
    WORD32             dwAlarmNo;
/**
    @li @b	����			ucPad
    @li @b	����			BYTE
    @li @b	����			���
*/
    BYTE                   ucPad[24];
}_PACKED_1_ T_Alarm_Del_Alarm;

/**
   @struct T_Alarm_Del_Alarm_Ack
   @brief ɾ��ָ���澯Ӧ��

   @li @b ���� ɾ��ָ���澯Ӧ�����̨

   <HR>
   @b �޸ļ�¼��

*/
typedef struct
{
/**
    @li @b	����			dwAlarmCode
    @li @b	����			�澯��
*/
    WORD32             dwAlarmCode;
/**
    @li @b	����			dwAlarmNo
    @li @b	����			�澯���
*/
    WORD32             dwAlarmNo;
/**
    @li @b	����			ucResult
    @li @b	����			BYTE
    @li @b	����			Ӧ����
<TABLE>
    <TR  Align=Center><TD>ucResult</TD><TD>Ӧ����˵��</TD></TR>
    <TR  Align=Center><TD>0</TD><TD>����ɾ��</TD></TR>
    <TR  Align=Center><TD>1</TD><TD>ɾ��ʧ��</TD></TR>
</TABLE>
*/
    BYTE                    ucResult;
/**
    @li @b	����			aucPad
    @li @b	����			BYTE
    @li @b	����			����ֽ�
*/
    BYTE                   aucPad[23];
}_PACKED_1_ T_Alarm_Del_Alarm_Ack;
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
* ���������� ���մ������ͻ�ȡ���п�������Ϣ
* ��������� ��
* ��������� ��Ϊ����(sas��sata�̵���Ŀ��slotid������)
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetAllUnusedDisk(T_AllUnusedPd *ptAllUnusedPd);
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
* ���������� ������
* ��������� *ptCreatePool
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 CreatePool(T_CreatePool *ptCreatePool);

/**********************************************************************
* ���������� ɾ���洢��
* ��������� *ptPoolName
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 DelPool(T_PoolName *ptPoolName);
/**********************************************************************
* ���������� ��ѯ�洢��
* ��������� *scPoolName
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetPoolInfo(T_PoolInfo *ptPoolInfo);
/**********************************************************************
* ���������� �洢���ϴ�����
* ��������� *ptCreateVolOnPool
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 CreateVolOnPool(T_CreateVolOnPool *ptCreateVolOnPool);
/**********************************************************************
* ����������  ��ѯϵͳ�����д洢�������б�
* ���������  ��
* ���������  ȫ��
* �� �� ֵ��  0      -   ִ�гɹ�
*             ��0    -   ִ��ʧ��
* ����˵����  ��
***********************************************************************/
SWORD32 GetAllPoolNames(T_AllPoolNames *ptAllPoolNames);
/**********************************************************************
* ����������  ��ȡ�洢���ϵ�����������б�
* ���������  cPoolName:�洢������
* ���������  cVolNames
* �� �� ֵ��  0      -   ִ�гɹ�
*             ��0    -   ִ��ʧ��
* ����˵����  ��
***********************************************************************/
SWORD32 GetVolNamesOnPool(T_VolNamesOnPool *ptAllVolNamesOnPool);
/**********************************************************************
* ����������  �޸Ĵ洢�����ԣ����ơ������澯��ֵ���ռ䱣�����ԣ�
* ���������  ȫ��
* ���������  ��
* �� �� ֵ��  0      -   ִ�гɹ�
*             ��0    -   ִ��ʧ��
* ����˵����  ��
***********************************************************************/
SWORD32 ModifyPool(T_ModifyPool *ptModifyPool);
/**********************************************************************
* ����������  ���Ӵ��̵��洢���У���������Ŀ���ӡ��������б����ӣ�
* ���������  ȫ��
* ���������  ��
* �� �� ֵ��  0      -   ִ�гɹ�
*             ��0    -   ִ��ʧ��
* ����˵����  �����մ����б��޸�ʱ���ṹ���е�tPdId����Ч
***********************************************************************/
SWORD32 AddPdToPool(T_ModifyDiskOnPool *ptModifyDiskOnPool);
/**********************************************************************
* ����������  �Ӵ洢����ɾ�����̣���������Ŀɾ�����������б�ɾ����
* ���������  ȫ��
* ���������  ��
* �� �� ֵ��  0      -   ִ�гɹ�
*             ��0    -   ִ��ʧ��
* ����˵����  �����մ����б��޸�ʱ���ṹ���е�tPdId����Ч
***********************************************************************/
SWORD32 DelPdFromPool(T_ModifyDiskOnPool *ptModifyDiskOnPool);
/**********************************************************************
* ����������  �洢���ϵ����������
* ���������  ȫ��
* ���������  ��
* �� �� ֵ��  0      -   ִ�гɹ�
*             ��0    -   ִ��ʧ��
* ����˵����  ��
***********************************************************************/
SWORD32 ExpandVolOnPool(T_ExpVolCapacity *ptExpVolCapacity);

/**********************************************************************
* ����������ϵͳ�Զ�ѡ����̴���������
* ���������ptAutoCreateVd
* ���������
* �� �� ֵ��0      -   ִ�гɹ�
*           ��0    -   ִ��ʧ��
* ����˵������
***********************************************************************/
SWORD32 AutoCreateVd(T_AutoCreateVd *ptAutoCreateVd);
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
* ��������������������Դ�ռ�
* ���������ptRvolCreate
* �����������
* ����ֵ��  0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 CreateRvol(T_RvolCreate *ptRvolCreate);
/**********************************************************************
* ���������� ɾ��������Դ�ռ�
* ��������� ptRvolDel
* ��������� ��
* ����ֵ��   0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 DelRvol(T_RvolDel *ptRvolDel);
/**********************************************************************
* ���������� ��ȡ������Դ�ռ���Ϣ
* ��������� ptRvolInfo
* ��������� ��
* ����ֵ��   0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetRvolInfo(T_RvolInfo *ptRvolInfo);
/**********************************************************************
* ���������� ���ÿ�����Դ�ռ�
* ��������� ptRvolSet
* ��������� ����Ϊ����
* ����ֵ��   0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 SetRvol(T_RvolSet *ptRvolSet);
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
* ���������� �鿴ϵͳ�����п��վ�����
* ���������
* ��������� ȫ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetAllSvolInfo(T_AllSvolInfo *ptAllSvolInfo);

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
* ���������� ��ȡ��������Ϣ
* ���������
* ��������� ��
* ����ֵ��    0   - SUCCESS    <0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetAllCvolInfo(T_AllCvolInfo *ptAllCvolInfo);


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
* ���������� ����Զ�̾���
* ��������� ptCreateMirror
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 CreateMirror(T_CreateMirror *ptCreateMirror);

/**********************************************************************
* ���������� ɾ��Զ�̾���
* ��������� ptMirrorName
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 DeleteMirror(T_MirrorName *ptMirrorName);

/**********************************************************************
* ���������� ��ʾԶ�̾�����Ϣ
* ��������� ��
* ��������� ptMirrorInfo
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetMirrorInfo(T_MirrorInfo *ptMirrorInfo);

/**********************************************************************
* ���������� ��ȡ���о��������б�
* ��������� ��
* ��������� ptAllMirrorNames
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetAllMirrorNames(T_AllMirrorNames *ptAllMirrorNames);

/**********************************************************************
* ���������� �޸�Զ�̾�������
* ��������� ptMirrorRename
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 SetMirrorName(T_MirrorRename *ptMirrorRename);

/**********************************************************************
* ���������� �޸�Զ�̾���ָ�����
* ��������� ptMirrorPolicy
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 SetMirrorPolicy(T_MirrorPolicy *ptMirrorPolicy);

/**********************************************************************
* ���������� �޸�Զ�̾���ͬ�����ȼ�
* ��������� ptMirrorPriority
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 SetMirrorPriority(T_MirrorPriority *ptMirrorPriority);

/**********************************************************************
* ���������� �޸�Զ�̾���ͬ��Ƶ��(���첽����)
* ��������� ptMirrorPeriod
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 SetMirrorPeriod(T_MirrorPeriod *ptMirrorPeriod);

/**********************************************************************
* ���������� ֹͣͬ��Զ�̾���
* ��������� ptMirrorName
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 StopMirrorSync(T_MirrorName *ptMirrorName);

/**********************************************************************
* ���������� ����ͬ��Զ�̾���
* ��������� ptMirrorName
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 StartMirrorSync(T_MirrorName *ptMirrorName);

/**********************************************************************
* ���������� ���þ����߼�����
* ��������� ptMirrorConnectInfo
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 SetFcMirrorConnect(T_FcMirrorConnectInfo *ptFcMirrorConnectInfo);

/**********************************************************************
* ���������� ɾ�������߼�����
* ��������� ptFcMirrorConnectInfo
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 DeleteFcMirrorConnect(T_FcMirrorConnectInfo *ptFcMirrorConnectInfo);

/**********************************************************************
* ���������� ��ѯ���о�������
* ��������� ��
* ��������� ptMirrorConnectList
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetAllMirrorConnect(T_MirrorConnectList *ptMirrorConnectList);

/**********************************************************************
* ���������� ��ѯ����Ŀ�ľ���Ϣ
* ��������� ��
* ��������� ptRemoteDiskList
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetAllRemoteDiskInfo(T_RemoteDiskList *ptRemoteDiskList);

/**********************************************************************
* ���������� ��ѯFC �˿���Ϣ
* ��������� ��
* ��������� ptFcPortInfoList
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetFcPortInfo(T_FcPortInfoList *ptFcPortInfoList);

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
* ���������� VPD��Ϣ��ѯ
* ��������� ��
* ��������� *ptVpd
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetVpd(T_VPD *ptVpd);
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
* ���������� ��ϵͳ��Ϣ��������ָ��·��
* ��������� *ptConfigFile
* ��������� ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 SaveSysInfoTarBall(T_SysInfoTarBall *ptSysInfoTarBall);

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
* ������������ȡ����������Ϣ
* �����������������־--sdwCtrlFlg
* �����������������Ŀ--sdwCtrlNum, �����Ϣ��¼--tEncInfoList[2]
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetDiagAllEncInfoList(T_AllEncInfoList *ptAllEncInfoList);

/**********************************************************************
* �������ƣ�GetBbuInfo
* ������������ȡBBU��Ϣ
* ���������sdwCtrlFlg
* �������������
* �� �� ֵ��0      -   ִ�гɹ�
*           ��0    -   ִ��ʧ��
* ����˵������
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2012/8/16       V1.0       ����         ���δ���
***********************************************************************/
SWORD32 GetBbuInfo(T_BbuInfo *ptBbuInfo);
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
* �������ƣ�scsSetDiagEccSwitch
* ����������
* ���ʵı�
* �޸ĵı�
* ���������
* ���������
* �� �� ֵ��  0      -   ִ�гɹ�
*                              ��0 - ִ��ʧ��
* ����˵������
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2012/11/28              V1.0              wangwei             ���δ���
***********************************************************************/
SWORD32 scsSetDiagEccSwitch(T_DiagEccSwitch *ptDiagEccSwitch);


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
* ������������ȡ��չ�������Ϣ
* �����������
* ��������������Ϣ�б�-- tExpanderInfo[MAX_UP_SUB_ENCL]
* �� �� ֵ��  0 - ִ�гɹ�
*           ��0 - ִ��ʧ��
* ����˵������
***********************************************************************/
SWORD32 GetDiagExpanderInfoList(T_ExpanderInfoList *ptExpanderInfoList);

/**********************************************************************
* ������������ȡ����״̬��Ϣ
* �����������������־--sdwCtrlFlg
* ���������������Ŀ--sdwBoardNum, �����Ϣ�б�--tDiagSingleBoardStatus[2]
* �� �� ֵ��  0 - ִ�гɹ�
*           ��0 - ִ��ʧ��
* ����˵������
***********************************************************************/
SWORD32 GetDiagBoardStatus(T_DiagBoardStatus *ptDiagBoardStatus);

/**********************************************************************
* ������������ȡ����״̬��Ϣ
* �����������
* �����������ϴ�����Ŀ--sdwHddNum
*           �����Ϣ�б�--tDiagSingleHddStatus[MAX_PD_NUM_IN_SYS]
* �� �� ֵ��  0 - ִ�гɹ�
*           ��0 - ִ��ʧ��
* ����˵������
***********************************************************************/
SWORD32 GetDiagHddStatus(T_DiagHddStatus *ptDiagHddStatus);

/**********************************************************************
* ������������ȡGE�˿�״̬��Ϣ
* �����������
* �� �� ֵ��  0 - ִ�гɹ�
*           ��0 - ִ��ʧ��
* ����˵������
***********************************************************************/
SWORD32 GetDiagGeInfo(T_DiagGeInfo *tDiagGeInfo);

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
* ���������� ���EEPROM�����Ϣ
* �����������
* ��������� ȫ��
* �� �� ֵ��  0      -   ִ�гɹ�
*             ��0    -   ִ��ʧ��
* ����˵������
***********************************************************************/
SWORD32 GetDiagEepromInfo(T_DiagEepromInfo *ptDiagEepromInfo);


/**********************************************************************
* ���������� ���FC�˿������Ϣ
* �����������
* ��������� ȫ��
* �� �� ֵ��  0      -   ִ�гɹ�
*             ��0    -   ִ��ʧ��
* ����˵������
***********************************************************************/
SWORD32 GetDiagFcInfo(T_DiagFcInfo *tDiagFcInfo);

/**********************************************************************
* ���������� ���IMBC�����Ϣ
* �����������
* ��������� ȫ��
* �� �� ֵ��  0      -   ִ�гɹ�
*             ��0    -   ִ��ʧ��
* ����˵������
***********************************************************************/
SWORD32 GetDiagIbmcInfo(T_DiagIbmcInfo *ptDiagIbmcInfo);

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
SWORD32 GetIscsiAllSessInfoList(T_IscsiSessInfoDualList *ptIscsiSessList);
SWORD32 GetIscsiAllSessNum(T_GetIscsiSessNum *ptIscsiAllSessNum);
SWORD32 GetIscsiSessOnceInfo(T_IscsiSessGetOnceInfo *ptIscsiSssGetOnceInfo);
SWORD32 GetIscsiAllHistorySessNum(T_GetIscsiSessNum *ptIscsiAllHisySessNum);
SWORD32 GetIscsiHisSessOnceInfo(T_IscsiSessGetOnceInfo *ptIscsiSssGetOnceInfo);

SWORD32 GetIscsiPduStatisticsInfo(T_IscsiAllPduStatisticInfo  *ptIscsiPduList);
SWORD32 GetFcGpnOnceInfo(T_GetOnceFcGpnInfo *ptFcGpnGetOnceInfo);

/**********************************************************************
* ��������������targetId ,��ȡ����Session ��Ϣ�б�
* ���������targetId
* ���������
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵����
***********************************************************************/
SWORD32 GetIscsiHistorySessInfoList(T_IscsiSessInfoDualList *ptIscsiHistorySessList);

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
SWORD32 GetIscsiTargetName(T_IscsiTargetList *ptIscsiTgtInfo);

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
SWORD32 GetIscsiPortInfoList(T_IscsiNetPortalList *ptIscsiPortInfo);

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
* ���������� ��ѯ����ڶ˿���Ϣ�б�
* �����������
* ���������ȫ��
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵������
***********************************************************************/
SWORD32 GetManaPortInfo(T_ManaPortInfoList *ptManaPortInfoList);

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
SWORD32 UpdatePreProcess(WORD16 ucFileType);

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
* �����������ϴ��ļ�����
* ����������ļ������ļ�����
* ���������
* ����ֵ��    0   - SUCCESS   !0     -  FAILURE
* ����˵������
***********************************************************************/
SWORD32 UploadPostProcess(T_UploadPreInfo *ptUploadPreInfo);

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

* ������������ȡ��������
* ���������GetBroadType
* ���������
* �� �� ֵ�� 0      -   ִ�гɹ�
*            ��0    - ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32  GetBroadType(T_SysBroadType *ptSysBroadType);

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
/**********************************************************************
* ��������������ͳ�ƿ���״̬
* ���������sdwStatFlag -- ͳ�ƿ���
* �����������
* �� �� ֵ��0      -   ִ�гɹ�
*           ��0    -   ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 SetStatAndWriteToCfg(SWORD32 sdwStatFlag);
/**********************************************************************
* ������������ѯͳ�ƿ���״̬
* �����������
* ���������pStatFlag -- ͳ�ƿ���
* �� �� ֵ��0      -   ִ�гɹ�
*           ��0    -   ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 GetStatisticStatus(SWORD32 *pStatFlag);
/**********************************************************************
* �������ƣ�GetCpuOccuRate
* ������������ѯCPUռ����
* �����������
* ���������ptCpuOccupy
* �� �� ֵ��0      -   ִ�гɹ�
*           ��0    -   ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 GetCpuOccupy(T_CpuOccupy  *ptCpuOccupy);
/**********************************************************************
* �������ƣ�GetDiskStaticData
* ������������ѯ����SASͳ����Ϣ
* �����������
* ���������ptSasStaticData
* �� �� ֵ��0      -   ִ�гɹ�
*           ��0    -   ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 GetDiskStaticData(T_SasStatInfo *ptSasStatInfo);
/**********************************************************************
* �������ƣ�GetPcieStaticData
* ������������ѯ�������ڲ�PCIe�������
* �����������
* ���������ptPcieSta
* �� �� ֵ��0      -   ִ�гɹ�
*           ��0    -   ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 GetPcieStaticData(T_PcieFlowStatus *ptPcieSta);
/**********************************************************************
* �������ƣ�GetTcsStaticData
* ������������ѯǰ������˿�TCP/IP����ͳ��
* ���������wSwitchFlag--ͳ�ƿ���
* ���������ptPcieSta
* �� �� ֵ��0      -   ִ�гɹ�
*           ��0    -   ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32  GetTcsStaticData(T_TcsStatInfo *ptTcsStatInfo);
/**********************************************************************
* ������������ѯԤ���¼���cache�Զ�ֱд����
* �����������
* ���������ptAutoWrThrPreSet
* �� �� ֵ��0      -   ִ�гɹ�
*           ��0    -   ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 GetAutoWrThrEvt(T_AutoWrThrPreSet *ptAutoWrThrPreSet);
/**********************************************************************
* ��������������Ԥ���¼���cache�Զ�ֱд����
* ���������ptAutoWrThrPreSet
* �����������
* �� �� ֵ��0      -   ִ�гɹ�
*           ��0    -   ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 SetAutoWrThrEvt(T_AutoWrThrPreSet *ptAutoWrThrPreSet);

/**********************************************************************
* ������������ȡtrap������Ϣ
* �����������
* ���������ptSnmpTrapConfig
* �� �� ֵ��0      -   ִ�гɹ�
*           ��0    -   ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 GetSnmpTrapConfig(T_TrapCfgList *ptSnmpTrapConfig);
/**********************************************************************
* �������������trap������Ϣ
* ���������ptSnmpTrapConfig
* �����������
* �� �� ֵ��0      -   ִ�гɹ�
*           ��0    -   ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 AddSnmpTrapConfig(T_SnmpTrapConfig *ptTrapConfigAdd);
/**********************************************************************
* �����������޸�trap������Ϣ
* ���������ptSnmpTrapConfig
* �����������
* �� �� ֵ��0      -   ִ�гɹ�
*           ��0    -   ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 ModySnmpTrapConfig(T_TrapCfgMody *ptModyTrapConfig);
/**********************************************************************
* ����������ɾ��trap������Ϣ
* ���������ServerId
* �����������
* �� �� ֵ��0      -   ִ�гɹ�
*           ��0    -   ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 DelSnmpTrapCfg (T_TrapCfgMody * ptTrapDel);

/**********************************************************************
* ����������������ֻ�ṩ��snmp���ã�����trap�ķ��ͣ�cli��web���ɵ��ã������и�cli��web�ṩ�ĺ���
* �����������
* ���������ptSnmpTrapConfig
* �� �� ֵ�� 0  -  ִ�гɹ�
*          ��0  -  ִ��ʧ��
* ����˵������
***********************************************************************/
SWORD32 SnmpGetTrapConfig(T_TrapCfgList *ptSnmpTrapConfig);

/**********************************************************************/
/*********************MTSʹ�õ�CAPI�ӿ�*******************************/
/**********************************************************************
* ��������������EPLD�Լ�
* �����������
* ���������ptCheckWorkEpldSelf
* �� �� ֵ��0      -   ִ�гɹ�
*           ��0    -   ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 mtsCapiCheckWorkEpldSelf(T_Mts_Check_Work_Epld_Self *ptCheckWorkEpldSelf);

/**********************************************************************
* ��������������EPLD��Ϣ��ȡ
* �����������
* ���������ptCheckWorkEpldInfo
* �� �� ֵ��0      -   ִ�гɹ�
*           ��0    -   ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 mtsCapiCheckWorkEpldInfo(T_Mts_Check_Work_Epld_Info *ptCheckWorkEpldInfo);

/**********************************************************************
* ��������������EPLD��Ϣ��ȡ
* �����������
* ���������ptCheckBootEpldInfo
* �� �� ֵ��0      -   ִ�гɹ�
*           ��0    -   ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 mtsCapiCheckBootEpldInfo(T_Mts_Check_Boot_Epld_Info *ptCheckBootEpldInfo);

/**********************************************************************
* ��������������������ɨ��
* �����������
* ���������ptCheckLableInfo
* �� �� ֵ��0      -   ִ�гɹ�
*           ��0    -   ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 mtsCapiCheckLableInfo(T_Mts_Check_Lable_Info *ptCheckLableInfo);

/**********************************************************************
* ��������������Ӳ����ַ��ȡ
* �����������
* ���������ptCheckPhyAddrInfo
* �� �� ֵ��0      -   ִ�гɹ�
*           ��0    -   ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 mtsCapiCheckPhyAddrInfo(T_Mts_Check_Phy_Addr_Info *ptCheckPhyAddrInfo);

/**********************************************************************
* ������������λ�źŲ���
* �����������
* ���������ptCheckLocPreState
* �� �� ֵ��0      -   ִ�гɹ�
*           ��0    -   ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 mtsCapiCheckLocalPreState(T_Mts_Check_Online_Flag *ptCheckLocPreState);

/**********************************************************************
* ����������CPU���DDR3����
* �����������
* ���������ptCheckDdr
* �� �� ֵ��0      -   ִ�гɹ�
*           ��0    -   ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 mtsCapiCheckDdr(T_Mts_Check_Ddr *ptCheckDdr);

/**********************************************************************
* ����������NVRAMоƬ����
* �����������
* ���������ptCheckNvram
* �� �� ֵ��0      -   ִ�гɹ�
*           ��0    -   ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 mtsCapiCheckNvram(T_Mts_Check_Nvram *ptCheckNvram);

/**********************************************************************
* �����������汾flash����
* �����������
* ���������ptCheckVerFlash
* �� �� ֵ��0      -   ִ�гɹ�
*           ��0    -   ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 mtsCapiCheckVerFlash(T_Mts_Check_Verflash *ptCheckVerFlash);

/**********************************************************************
* ��������������ʱ�ӵ�·����
* �����������
* ���������ptCheckClock
* �� �� ֵ��0      -   ִ�гɹ�
*           ��0    -   ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 mtsCapiCheckClock(T_Mts_Check_Clock *ptCheckClock);

/**********************************************************************
* �������������ָʾ�Ʋ���
* �����������
* ���������ptCheckLed
* �� �� ֵ��0      -   ִ�гɹ�
*           ��0    -   ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 mtsCapiCheckLed(T_Mts_Check_Led *ptCheckLed);

/**********************************************************************
* �������������Ź���λ����
* �����������
* ���������ptCheckWtg
* �� �� ֵ��0      -   ִ�гɹ�
*           ��0    -   ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 mtsCapiCheckWtg(T_Mts_Check_Wtg *ptCheckWtg);

/**********************************************************************
* �����������¶ȴ�����
* �����������
* ���������ptCheckTemp
* �� �� ֵ��0      -   ִ�гɹ�
*           ��0    -   ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 mtsCapiCheckSenor(T_Mts_Check_Senor *ptCheckSenor);

/**********************************************************************
* ����������RTCʵʱʱ�ӵ�·����
* �����������
* ���������ptCheckRtc
* �� �� ֵ��0      -   ִ�гɹ�
*           ��0    -   ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 mtsCapiCheckRtc(T_Mts_Check_Rtc *ptCheckRtc);

/**********************************************************************
* �������������GE�ڻ��ز���
* �����������
* ���������ptCheckGe
* �� �� ֵ��0      -   ִ�гɹ�
*           ��0    -   ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 mtsCapiCheckGe(T_Mts_Check_Ge *ptCheckGe);


/**********************************************************************
* ����������������������Ϊͨ����·����
* �����������
* ���������ptCheckConnectLink
* �� �� ֵ��0      -   ִ�гɹ�
*           ��0    -   ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 mtsCapiCheckConnectLink(T_Mts_Check_Conn_Link *ptCheckConnLink);


/**********************************************************************
* ����������Cacheͬ���źŲ���/PCIEоƬ���Լ�
* �����������
* ���������ptCheckPcieInfo
* �� �� ֵ��0      -   ִ�гɹ�
*           ��0    -   ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 mtsCapiCheckPcie(T_Mts_Check_Pcie_Info *ptCheckPcieInfo);

/**********************************************************************
* ����������phyоƬ�Լ�
* �����������
* ���������ptPhySelf
* �� �� ֵ��0      -   ִ�гɹ�
*           ��0    -   ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 mtsCapiCheckPhySelf(T_Mts_Check_Phyself *ptPhySelf);

/**********************************************************************
* ����������PM8001�Լ�
* �����������
* ���������ptPmSelf
* �� �� ֵ��0      -   ִ�гɹ�
*           ��0    -   ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 mtsCapiCheckPmSelf(T_Mts_Check_Pm *ptPmSelf);

/**********************************************************************
* ��������������IO�ź�ָʾ�Ʋ���
* �����������
* ���������ptInputIo
* �� �� ֵ��0      -   ִ�гɹ�
*           ��0    -   ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32  mtsCapiCheckInputIo(T_Mts_Check_InputIo *ptInputIo);

/**********************************************************************
* �������������IO�ź�ָʾ�Ʋ���
* �����������
* ���������ptOutputIo
* �� �� ֵ��0      -   ִ�гɹ�
*           ��0    -   ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32  mtsCapiCheckOutputIo(T_Mts_Check_OutputIo *ptOutputIo);

/**********************************************************************
* ������������Դ�ź�ָʾ�Ʋ���
* �����������
* ���������ptPowerLight
* �� �� ֵ��0      -   ִ�гɹ�
*           ��0    -   ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32  mtsCapiCheckPowerLight(T_Mts_Check_Power_Light *ptPowerLight);

/**********************************************************************
* ������������ȡBBU��Ϣ
* �����������
* ���������ptMtsCheckBbu
* �� �� ֵ��0      -   ִ�гɹ�
*           ��0    -   ִ��ʧ��
* ����˵����
***********************************************************************/
SWORD32 mtsCapiGetWorkBbuInfo(T_Mts_Check_Bbu *ptCheckBbu);

/**********************************************************************
* �������ƣ�mtsCapiEzzfReset
* ����������8005���Ź���λ����
* �����������
* ���������ptMtsCheckBbu
* �� �� ֵ�� 0  -  ִ�гɹ�
*          ��0  -  ִ��ʧ��
* ����˵������
***********************************************************************/
SWORD32 mtsCapiEzzfReset(T_Mts_Check_Ezzf_Reset *ptMtsEzzfReset);


/**********************************************************************
* �������ƣ�mtsCapiEzzfGetSpeed
* ����������minisas�ӿ��Ի�����
* �����������
* ���������ptMtsCheckBbu
* �� �� ֵ�� 0  -  ִ�гɹ�
*          ��0  -  ִ��ʧ��
* ����˵������
***********************************************************************/
SWORD32 mtsCapiEzzfGetSpeed(T_Mts_Check_Ezzf_GetSpeed *ptMtsEzzfGetSpeed);

/**********************************************************************
* �������ƣ�mtsCapiEzzfBbGetSpeed
* ��������������SAS�ӿ��Ի�����
* �����������
* ���������ptMtsCheckBbu
* �� �� ֵ�� 0  -  ִ�гɹ�
*          ��0  -  ִ��ʧ��
* ����˵������
***********************************************************************/
SWORD32 mtsCapiEzzfBbGetSpeed(T_Mts_Check_Ezzf_BdGetSpeed *ptMtsEzzfBdGetSpeed);

/**********************************************************************
* �������ƣ�mtsCapiEzzfTtSigTest
* ����������TW0\TW1\TW2�źŲ���
* �����������
* ���������ptMtsCheckBbu
* �� �� ֵ�� 0  -  ִ�гɹ�
*          ��0  -  ִ��ʧ��
* ����˵������
***********************************************************************/
SWORD32 mtsCapiEzzfTtSigTest(T_Mts_Check_Ezzf_Ttsig *ptMtsEzzfTtsig);

/**********************************************************************
* �������ƣ�mtsCapiEzzfSasSelf
* ����������SAS��չоƬPM8005�Լ�
* �����������
* ���������ptMtsCheckBbu
* �� �� ֵ�� 0  -  ִ�гɹ�
*          ��0  -  ִ��ʧ��
* ����˵������
***********************************************************************/
SWORD32 mtsCapiEzzfSasSelf(T_Mts_Check_Ezzf_Self *ptMtsEzzfSelf);

/**********************************************************************
* �������ƣ�mtsCapiHddLight
* ����������Ӳ��ָʾ�Ʋ���
* �����������
* ���������ptMtsCheckBbu
* �� �� ֵ�� 0  -  ִ�гɹ�
*          ��0  -  ִ��ʧ��
* ����˵������
***********************************************************************/
SWORD32 mtsCapiHddLight(T_Mts_Check_Hdd_Light *ptMtsHddLight);


/**********************************************************************
* �������ƣ�mtsCapiDdrSize
* �����������ڴ��С���
* �����������
* ���������ptMtsDdrSize
* �� �� ֵ�� 0  -  ִ�гɹ�
*          ��0  -  ִ��ʧ��
* ����˵������
***********************************************************************/
SWORD32 mtsCapiDdrSize(T_Mts_Check_Ddr_Size *ptMtsDdrSize);

/**********************************************************************
* �������ƣ�mtsCapiCheckVer
* �������������汾
* �����������
* ���������ptMtsDdrSize
* �� �� ֵ�� 0  -  ִ�гɹ�
*          ��0  -  ִ��ʧ��
* ����˵������
***********************************************************************/
SWORD32 mtsCapiCheckVer(T_Mts_Check_Ver *ptMtsVer);

/**********************************************************************
* �������ƣ�mtsCapiCheckMacAddr
* ����������MAC��ַ��ȡ
* �����������
* ���������ptMtsMacAddr
* �� �� ֵ�� 0  -  ִ�гɹ�
*          ��0  -  ִ��ʧ��
* ����˵������
***********************************************************************/
SWORD32 mtsCapiCheckMacAddr(T_Mts_Check_MacAddr *ptMtsMacAddr);

/*****************************��������*********************************/
/**********************************************************************
* �������ƣ�cmtCapiWorkEpldSelf
* ��������������EPLD��������
* �����������
* ���������ptMtsWorkEpldSelf
* �� �� ֵ�� 0  -  ִ�гɹ�
*          ��0  -  ִ��ʧ��
* ����˵������
***********************************************************************/
SWORD32 cmtCapiWorkEpldSelf(T_WorkEpldSelfList *ptCmtWorkEpldSelf);

/**********************************************************************
* �������ƣ�cmtCapiWorkEpldInfo
* ��������������EPLD��Ϣ��ȡ
* �����������
* ���������ptCmtWorkEpldInfo
* �� �� ֵ�� 0  -  ִ�гɹ�
*          ��0  -  ִ��ʧ��
* ����˵������
***********************************************************************/
SWORD32 cmtCapiWorkEpldInfo(T_WorkEpldInfoList *ptCmtWorkEpldInfo);

/**********************************************************************
* �������ƣ�cmtCapiBootEpldInfo
* ��������������EPLD����������Ϣ��ȡ
* ���ʵı���
* �޸ĵı���
* �����������
* ���������ptMtsWorkEpldSelf
* �� �� ֵ�� 0  -  ִ�гɹ�
*          ��0  -  ִ��ʧ��
* ����˵������
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2012/05/12      V1.0       wangwei      ���δ���
***********************************************************************/
SWORD32 cmtCapiBootEpldInfo(T_BootEpldInfoList *ptCmtBootEpldInfo);

/**********************************************************************
* �������ƣ�cmtCapiLableInfo
* �������������ӱ�ǩ��Ϣ
* �����������
* ���������ptMtsWorkEpldSelf
* �� �� ֵ�� 0  -  ִ�гɹ�
*          ��0  -  ִ��ʧ��
* ����˵������
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2012/05/12      V1.0       wangwei      ���δ���
***********************************************************************/
SWORD32 cmtCapiLableInfo(T_LableInfoList *ptCmtLableInfo);

/**********************************************************************
* �������ƣ�cmtCapiPhyAddrInfo
* ��������������Ӳ����ַ��ȡ
* �����������
* ���������ptMtsWorkEpldSelf
* �� �� ֵ�� 0  -  ִ�гɹ�
*          ��0  -  ִ��ʧ��
* ����˵������
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2012/05/12      V1.0       wangwei      ���δ���
***********************************************************************/
SWORD32 cmtCapiPhyAddrInfo(T_PhyAddrInfoList *ptCmtPhyAddrInfo);

/**********************************************************************
* �������ƣ�cmtCapiOnlineFlagInfo
* ������������λ�źŲ���
* �����������
* ���������ptMtsWorkEpldSelf
* �� �� ֵ�� 0  -  ִ�гɹ�
*          ��0  -  ִ��ʧ��
* ����˵������
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2012/05/12      V1.0       wangwei      ���δ���
***********************************************************************/
SWORD32 cmtCapiOnlineFlagInfo(T_OnlineFlagInfoList *ptCmtOnlieFlagInfo);

/**********************************************************************
* �������ƣ�cmtCapiClockInfo
* ��������������ʱ�ӵ�·����
* �����������
* ���������ptMtsWorkEpldSelf
* �� �� ֵ�� 0  -  ִ�гɹ�
*          ��0  -  ִ��ʧ��
* ����˵������
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2012/05/12      V1.0       wangwei      ���δ���
***********************************************************************/
SWORD32 cmtCapiClockInfo(T_ClockInfoList *ptCmtClockInfo);

/**********************************************************************
* �������ƣ�cmtCapiSenorInfo
* ������������������Ϣ��ȡ
* �����������
* ���������ptMtsWorkEpldSelf
* �� �� ֵ�� 0  -  ִ�гɹ�
*          ��0  -  ִ��ʧ��
* ����˵������
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2012/05/12      V1.0       wangwei      ���δ���
***********************************************************************/
SWORD32 cmtCapiSenorInfo(T_SenorInfoList *ptCmtSenorInfo);
SWORD32 cmtCapiExpInfo(T_MtsEncDeviceInfoList *ptCmtExpInfo);
SWORD32 cmtCapiDiskInfo(T_MtsDiskInfo *ptCmtDiskInfo);

/**********************************************************************
* �������ƣ�cmtCapiRtcInfo
* ����������RTCʵʱʱ�ӵ�·����
* �����������
* ���������ptMtsWorkEpldSelf
* �� �� ֵ�� 0  -  ִ�гɹ�
*          ��0  -  ִ��ʧ��
* ����˵������
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2012/05/12      V1.0       wangwei      ���δ���
***********************************************************************/
SWORD32 cmtCapiRtcInfo(T_RtcInfoList *ptCmtRtcInfo);


/**********************************************************************
* �������ƣ�cmtCapiGeInfo
* ����������GE����·���ʣ��Ƿ�˫�ز���
* �����������
* ���������ptMtsWorkEpldSelf
* �� �� ֵ�� 0  -  ִ�гɹ�
*          ��0  -  ִ��ʧ��
* ����˵������
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2012/05/12      V1.0       wangwei      ���δ���
***********************************************************************/
SWORD32 cmtCapiGeInfo(T_GeInfoList *ptCmtGeInfo);

/**********************************************************************
* �������ƣ�cmtCapiPcieInfo
* ����������PCIE����
* �����������
* ���������ptMtsWorkEpldSelf
* �� �� ֵ�� 0  -  ִ�гɹ�
*          ��0  -  ִ��ʧ��
* ����˵������
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2012/05/12      V1.0       wangwei      ���δ���
***********************************************************************/
SWORD32 cmtCapiPcieInfo(T_PcieInfoList *ptCmtPcieInfo);

/**********************************************************************
* �������ƣ�cmtCapiPhyInfo
* ����������PHY�Լ�
* �����������
* ���������ptMtsWorkEpldSelf
* �� �� ֵ�� 0  -  ִ�гɹ�
*          ��0  -  ִ��ʧ��
* ����˵������
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2012/05/12      V1.0       wangwei      ���δ���
***********************************************************************/
SWORD32 cmtCapiPhyInfo(T_PhyInfoList *ptCmtPhyInfo);


/**********************************************************************
* �������ƣ�cmtCapiPmInfo
* ����������SAS������PM8001�Լ�
* �����������
* ���������ptCmtPmInfo
* �� �� ֵ�� 0  -  ִ�гɹ�
*          ��0  -  ִ��ʧ��
* ����˵������
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2012/05/12      V1.0       wangwei      ���δ���
***********************************************************************/
SWORD32 cmtCapiPmInfo(T_PmInfoList *ptCmtPmInfo);

/**********************************************************************
* �������ƣ�cmtCapiBbuInfo
* ����������BBU���
* �����������
* ���������ptCmtBbuInfo
* �� �� ֵ�� 0  -  ִ�гɹ�
*          ��0  -  ִ��ʧ��
* ����˵������
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2012/05/12      V1.0       wangwei      ���δ���
***********************************************************************/
SWORD32 cmtCapiBbuInfo(T_BbuInfoList *ptCmtBbuInfo);

/**********************************************************************
* �������ƣ�cmtCapiSasInfo
* ����������MINISAS�ͱ���SAS�ӿڲ���
* �����������
* ���������ptCmtSasInfo
* �� �� ֵ�� 0  -  ִ�гɹ�
*          ��0  -  ִ��ʧ��
* ����˵������
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2012/05/12      V1.0       wangwei      ���δ���
***********************************************************************/
SWORD32 cmtCapiSasInfo(T_SasInfoList *ptCmtSasInfo);

/**********************************************************************
* �������ƣ�cmtCapiHddInfo
* ����������HardDisk �Լ�
* �����������
* ���������ptCmtHddInfo
* �� �� ֵ�� 0  -  ִ�гɹ�
*          ��0  -  ִ��ʧ��
* ����˵������
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2012/05/12      V1.0       wangwei      ���δ���
***********************************************************************/
SWORD32 cmtCapiHddInfo(T_HddInfoList *ptCmtHddInfo);

/**********************************************************************
* �������ƣ�cmtCapiVerInfo
* �����������汾���
* �����������
* ���������cmtCapiVerInfo
* �� �� ֵ�� 0  -  ִ�гɹ�
*          ��0  -  ִ��ʧ��
* ����˵������
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2012/05/12      V1.0       wangwei      ���δ���
***********************************************************************/
SWORD32 cmtCapiVerInfo(T_VerInfoList *ptCmtVerInfo);

/**********************************************************************
* �������ƣ�cmtCapiDdrInfo
* �����������ڴ���
* �����������
* ���������ptMtsWorkEpldSelf
* �� �� ֵ�� 0  -  ִ�гɹ�
*          ��0  -  ִ��ʧ��
* ����˵������
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2012/05/12      V1.0       wangwei      ���δ���
***********************************************************************/
SWORD32 cmtCapiDdrInfo(T_DdrInfoList *ptCmtDdrInfo);

/**********************************************************************
* �������ƣ�cmtCapiMacAddrInfo
* ����������MAC��ַ
* �����������
* ���������ptMtsWorkEpldSelf
* �� �� ֵ�� 0  -  ִ�гɹ�
*          ��0  -  ִ��ʧ��
* ����˵������
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2012/05/12      V1.0       wangwei      ���δ���
***********************************************************************/
SWORD32 cmtCapiMacAddrInfo(T_MacAddrInfoList *ptCmtMacAddrInfo);

/**********************************************************************
* �������ƣ�cmtCapiPeerStateInfo
* �������������Ի������
* ���ʵı���
* �޸ĵı���
* �����������
* ���������ptMtsWorkEpldSelf
* �� �� ֵ�� 0  -  ִ�гɹ�
*          ��0  -  ִ��ʧ��
* ����˵������
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2012/05/12      V1.0       wangwei      ���δ���
***********************************************************************/
SWORD32 cmtCapiPeerStateInfo(T_PeerStateInfoList *ptCmtPeerStateInfo);

/**********************************************************************
* �������ƣ�cmtCapiSasAddrInfo
* ����������SAS��ַ���,��8005�Լ�
* ���ʵı���
* �޸ĵı���
* �����������
* ���������ptMtsWorkEpldSelf
* �� �� ֵ�� 0  -  ִ�гɹ�
*          ��0  -  ִ��ʧ��
* ����˵������
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2012/05/12      V1.0       wangwei      ���δ���
***********************************************************************/
SWORD32 cmtCapiSasAddrInfo(T_SasAddrInfoList *ptCmtSasAddrInfo);

SWORD32 cmtCapispr11DdrSize(T_Cmt_SPR11_Check_Ddr_Size *ptMtsDdrSize);
SWORD32 cmtCapiSPR11WorkEpldInfo(T_SPR11WorkEpldInfoList *ptCmtWorkEpldInfo);
SWORD32 cmtCapispr11SSDInfo(T_Cmt_SPR11_SSD *ptSsdInfo);
SWORD32 cmtCapispr11CheckMacAddr(T_Cmt_SPR11_Check_MacAddr *ptMtsMacAddr);
SWORD32 cmtCapispr11CheckChipClock(T_Cmt_SPR11_ChipCLOCK *ptMtsChipClock);
SWORD32 cmtCapispr11CheckHb(T_Cmt_SPR11_Check_Hb *ptMtsHb);
SWORD32 cmtCapispr11CheckChips(T_Cmt_SPR11_Check_Chips *ptCheckChips);
SWORD32 cmtCapispr11FCInfo(T_Cmt_SPR11_FCinfo *ptFcInfo);
SWORD32 cmtCapispr11VoltInfo(T_Cmt_SPR11_Voltinfo *ptVoltInfo);
SWORD32 cmtCapispr11FCCtl(T_Cmt_FCSetMSTSFPTXStatus *ptFCtl);
/**********************************************************************
* �������ƣ�CollectSysInfo
* ����������ϵͳ��Ϣ�ɼ�
* ���ʵı���
* �޸ĵı���
* �����������
* ���������ptMtsWorkEpldSelf
* �� �� ֵ�� 0  -  ִ�гɹ�
*          ��0  -  ִ��ʧ��
* ����˵������
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2012/06/12      V1.0       Ҧ��      ���δ���
***********************************************************************/
SWORD32 CollectSysInfo(T_SicCtrlAction *ptCtrlAction);

/**********************************************************************
* �������ƣ�GetTarBallInfo
* ����������ϵͳ��Ϣ�ɼ�ѹ������Ϣ��ȡ
* ���ʵı���
* �޸ĵı���
* �����������
* ���������ptMtsWorkEpldSelf
* �� �� ֵ�� 0  -  ִ�гɹ�
*          ��0  -  ִ��ʧ��
* ����˵������
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2012/06/12      V1.0       Ҧ��      ���δ���
***********************************************************************/
SWORD32 GetTarBallInfo(T_TarBallInfoList *ptTarBallInfoList);

/**********************************************************************
* �������ƣ�SetSicEmailCfg
* ��������������ϵͳ��Ϣ�ɼ������ʼ���������
* ���ʵı���
* �޸ĵı���
* �����������
* ���������ptMtsWorkEpldSelf
* �� �� ֵ�� 0  -  ִ�гɹ�
*          ��0  -  ִ��ʧ��
* ����˵������
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2012/06/12      V1.0       Ҧ��      ���δ���
***********************************************************************/
SWORD32 SetSicEmailCfg(T_EmailCfg *ptEmailCfg);

/**********************************************************************
* �������ƣ�GetSicEmailCfg
* ������������ȡϵͳ��Ϣ�ɼ������ʼ���������
* ���ʵı���
* �޸ĵı���
* �����������
* ���������ptMtsWorkEpldSelf
* �� �� ֵ�� 0  -  ִ�гɹ�
*          ��0  -  ִ��ʧ��
* ����˵������
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2012/06/12      V1.0       Ҧ��      ���δ���
***********************************************************************/
SWORD32 GetSicEmailCfg(T_EmailCfg *ptEmailCfg);

SWORD32 mtsCapispr11CheckDdr(T_Mts_SPR11_Check_Ddr *ptCheckDdr);
SWORD32 mtsCapispr11DdrSize(T_Mts_SPR11_Check_Ddr_Size *ptMtsDdrSize);
SWORD32 mtsCapispr11CheckWorkEpldSelf(T_Mts_SPR11_Check_Work_Epld_Self *ptCheckWorkEpldSelf);
SWORD32 mtsCapispr11CheckMacAddr(T_Mts_SPR11_Check_MacAddr *ptMtsMacAddr);
SWORD32 mtsCapispr11CheckSenor(T_Mts_SPR11_Check_Senor *ptCheckSenor);
SWORD32 mtsCapispr11GetWorkBbuInfo(T_Mt_SPR11_Check_Bbu *ptMtsCheckBbu);
SWORD32 mtsCapispr11GetSasLinkSpeed(T_Mts_SPR11_SASLINK *ptMtsSasLinkSpeed);
SWORD32 mtsCapispr11CheckRtc(T_Mts_SPR11_Check_Rtc *ptCheckRtc);
SWORD32 mtsCapispr11CheckClock(T_Mts_SPR11_Check_Clock *ptCheckClock);
SWORD32 mtsCapispr11Light(T_Mts_SPR11_Check_LED_Light *ptMtsHddLight);
SWORD32 mtsCapispr11CheckWorkEpldInfo(T_Mts_SPR11_Check_Work_Epld_Info *ptCheckWorkEpldInfo);
SWORD32 mtsCapispr11CheckChips(T_Mts_SPR11_Check_Chips *ptCheckChips);
SWORD32 mtsCapispr11CheckLableInfo(T_Mts_SPR11_Check_Lable_Info *ptCheckLableInfo);
SWORD32 mtsCapispr11CheckWtg(T_Mts_Check_Wtg *ptCheckWtg);
SWORD32 mtsCapispr11CheckVer(T_Mts_SPR11_Check_Ver *ptMtsVer);
SWORD32 mtsCapispr11FCInfo(T_Mts_SPR11_FCinfo *ptFcInfo);
SWORD32 mtsCapispr11CheckFC(T_Mts_Check_FC *ptCheckFC);
SWORD32 mtsCapispr11CheckFCLight(T_Mts_Check_FC_LED *ptFCLight);
SWORD32 mtsCapispr11SSDInfo(T_Mts_SPR11_SSD *ptSsdInfo);
SWORD32 mtsCapispr11CheckMacBand(T_Mts_SPR11_Check_Macband *ptMtsMacBand);
SWORD32 mtsCapispr11GetUSB(T_Mts_SPR11_USB *ptMtsUSB);
SWORD32 mtsCapispr11CheckPresent(T_Mts_SPR11_Online_Flag *ptMtsPresent);
SWORD32 mtsCapispr11CheckChipClock(T_Mts_SPR11_ChipCLOCK *ptMtsChipClock);
SWORD32 mtsCapispr11Checkcpld(T_Mts_SPR11_CHECKCOMMON *ptMtsCPLD);
SWORD32 mtsCapispr11Dspam(T_Mts_SPR11_DSPAM *ptMtsDspam);
SWORD32 mtsCapispr11BackPlaneIO(T_Mts_SPR11_CHECKBACKPLANEIO *ptBackplaneio);
/* ����Ϊfc�Ự�����ṩ�Ľӿں��� */
/**********************************************************************
* �������ƣ�GetFCProtocalInfo
* ������������ȡFC�ỰЭ����Ϣ
* ���ʵı���
* �޸ĵı���
* ���������CtrlId,PortId
* ���������Э����Ϣ
* �� �� ֵ�� 0  -  ִ�гɹ�
*          ��0  -  ִ��ʧ��
* ����˵������
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2012/08/30      V1.1      qinping      ���δ���
***********************************************************************/
SWORD32 GetFCProtocalInfo(T_GetFCProtocolIInfo *ptFcProtocalInfo);
/**********************************************************************
* �������ƣ�GetFCProtocalInfo
* ������������ȡFC�Ựͳ����Ϣ
* ���ʵı���
* �޸ĵı���
* ���������CtrlId,PortId
* ���������Э����Ϣ
* �� �� ֵ�� 0  -  ִ�гɹ�
*          ��0  -  ִ��ʧ��
* ����˵������
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2012/08/30      V1.1      qinping      ���δ���
***********************************************************************/
SWORD32 GetFCPortStatisticInfo(T_FCGetPortStatisInfo *ptFcPortStatisInfo);
/**********************************************************************
* �������ƣ�GetFCProtocalInfo
* �������������FC�Ựͳ����Ϣ
* ���ʵı���
* �޸ĵı���
* ���������CtrlId,PortId
* ���������Э����Ϣ
* �� �� ֵ�� 0  -  ִ�гɹ�
*          ��0  -  ִ��ʧ��
* ����˵������
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2012/08/30      V1.1      qinping      ���δ���
***********************************************************************/
SWORD32 ClearFCPortStatisticInfo(T_ClearFCPortStatisInfo *ptClearFcPortStatisInfo);

/**********************************************************************
* �������ƣ�GetFCProtocalInfo
* ������������ȡFC�Ựͳ����Ϣ���ṩ��web����
* ���ʵı���
* �޸ĵı���
* ���������CtrlId,PortId
* ���������Э����Ϣ
* �� �� ֵ�� 0  -  ִ�гɹ�
*          ��0  -  ִ��ʧ��
* ����˵������
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2012/08/30      V1.1      qinping      ���δ���
***********************************************************************/
SWORD32 GetFCPortStatisticInfoForWeb(T_WebGetFCPortStatisInf *ptFcPortStatisInfo);

/**********************************************************************
* �������ƣ�GetAlarmInfo
* �����������ְ���ȡ�澯��Ϣ
* ���ʵı���
* �޸ĵı���
* ���������T_Alarm_Req *ptAlarmReq
* ���������T_Alarm_Packet_Ack *ptAlarmAck
* �� �� ֵ�� 0  -  ִ�гɹ�
*          ��0  -  ִ��ʧ��
* ����˵������
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2012/11/11     V1.1      yangcl      ���δ���
***********************************************************************/
SWORD32 GetAlarmInfo(T_Alarm_Req *ptAlarmReq,T_Alarm_Packet_Ack *ptAlarmAck);

/**********************************************************************
* �������ƣ�  DismissAlarmInfo
* ��������������澯
* ���������T_Alarm_Del_Alarm *ptDelAlarmReq
* ���������T_Alarm_Del_Alarm_Ack *ptDelAlarmAck
* �� �� ֵ��  0      -   ִ�гɹ�
*             ��0    -   ִ��ʧ��
* ����˵������
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2012/11/11     V1.1      yangcl      ���δ���
***********************************************************************/
SWORD32 DismissAlarmInfo(T_Alarm_Del_Alarm *ptDelAlarmReq,T_Alarm_Del_Alarm_Ack *ptDelAlarmAck);

/**********************************************************************
* �������ƣ�  GetUspUpdateChkStatus
* ������������ȡUSP����ǰ�Ƿ�߱���������
* ���������T_UpdateUspChk *ptUpdateUspChk
* ���������T_UpdateUspChk *ptUpdateUspChk
* �� �� ֵ��  0      -   ִ�гɹ�
*             ��0    -   ִ��ʧ��
* ����˵������
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/03/12     V1.1      huangan      ���δ���
***********************************************************************/
SWORD32 GetUspUpdateChkStatus(T_UpdateUspChk *ptUpdateUspChk);
#endif
