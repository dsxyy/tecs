/*********************************************************************
* 版权所有 (C)2010, 深圳市中兴通讯股份有限公司。
*
* 文件名称： usp_capi.h
* 文件标识： 见配置管理计划书
* 内容摘要： USP项目结构体定义 & 接口声明
* 其它说明： 无
* 当前版本： 0.1
* 作    者： tao.linjun@zte.com.cn
* 完成日期： 2010-11-2
**********************************************************************/

#ifndef __USP_CAPI_H__
#define __USP_CAPI_H__

#include "usp_common_macro.h"

#if defined(LINK_OSS)

#include "tulip.h"

#ifdef _PACKED_1_
/* 取消先前的_PACKED_1_宏定义 */
#undef _PACKED_1_
/* 重新定义宏_PACKED_1_ */
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
    CHAR ucName[FILE_PATH_LEN]; /* 包括路径 */
    BYTE ucFileType;            /* 文件类型 */
}_PACKED_1_ T_FileName;


typedef struct
{
    T_Time tTime;           //计划任务第一次运行任务,如果sdwSchedPeriod为“SCHED_IMMEDIATELY”，该时间不需指定
    SWORD32  sdwSchedPeriod; //计划任务的周期，取值SCHED_IMMEDIATELY、SCHED_ONCE、SCHED_DAYLY、SCHED_WEEKLY、SCHED_MONTHLY
}_PACKED_1_ T_Schedule;

/********************************* Time ***********************************/


typedef struct
{
    CHAR scNtpServerAddr[NTP_HOST_NAME_LEN];    /* NTP服务器地址 */
}_PACKED_1_ T_NTPServerInfo;

typedef struct
{
    CHAR scZoneCityName[TIME_ZONE_CITY_NAME_LEN];         /* 时区城市名 */
}_PACKED_1_ T_TimeZoneCityName;

/*
 * 将结构体字段修改为SWORD类型, 在设置时间时, 如果只设置date, 则将hour、minute和second初始化为-1;
 * 如果只设置time, 则将year、month和day初始化为-1
*/
typedef struct
{
    SWORD32 dwTmYear;    /* 年 ,公元纪年,比如2011*/
    SWORD32 dwTmMon;     /* 月 ,实际月份*/
    SWORD32 dwTmDay;     /* 日 */
    SWORD32 dwTmHour;    /* 时 */
    SWORD32 dwTmMin;      /* 分 */
    SWORD32 dwTmSec;      /* 秒 */
}_PACKED_1_ T_SysTime;    /* 系统时间 */

typedef struct
{
    T_TimeZoneCityName tCityName;  /* 时区城市名*/
    SWORD32 sdwNtpStatus;              /* 是否定时同步 ，自动:NTP_AUTO(1);  手动 :NTP_MANUAL(0)*/
    T_NTPServerInfo tServerAddr;      /* NTP服务器地址*/
}_PACKED_1_ T_NtpInfo;

typedef struct
{
    T_SysTime tSysTime;             /* 系统时间 */
    T_TimeZoneCityName tZoneCity;  /*时区城市名*/
}_PACKED_1_ T_TimeInfo;  /* 用于时间和时区的获取 */

typedef struct
{
 //   SWORD32 sdwDebug;    /* 1 -- 调试模式, 只获取时间服务器上的时间, 而不设置系统时间 */
    T_NTPServerInfo tServerInfo;     /* NTP服务器地址*/
}_PACKED_1_ T_NtpSync;    /* NTP时间同步参数 */

typedef struct
{
    SWORD32 sdwCityNum;        /* 系统提供的时区城市数目 */
    T_TimeZoneCityName tZoneCity[TIME_ZONE_CITY_NUM];      /*时区城市列表*/
}_PACKED_1_ T_ZoneCityList;

/***************************************DM*************************************/
typedef struct
{
    SWORD32  sdwSlotId;      /* 磁盘槽位号，取值0~27(ScanBox);  0~11(SPR10) */
    SWORD32  sdwEnclosureId;  /* 箱体Id，取值 0~实际箱体数*/
}_PACKED_1_ T_PdId;

typedef struct
{
    SWORD32  sdwNum;     /* 磁盘个数，取值MAX_ENC_NUM_IN_SYS(4) * MAX_PD_NUM_IN_ENC(16) */
    T_PdId       tPdId[MAX_PD_NUM_IN_SYS];
}_PACKED_1_ T_AllPdId;

#if 0
typedef struct
{
    SWORD32  sdwSlotId;     /* 磁盘槽位号，取值0~15 */
    SWORD32  sdwEnclosureId;/* 箱体Id，取值 0~实际箱体数*/
    SWORD64  sqwCapacity;   /* 磁盘容量 */
    SWORD32  sdwCache;      /* 硬盘cache状态 1 – 使能, 0 – 禁止, -1 –不支持 */
    SWORD32  sdwPowerSave;  /* 硬盘节能模式状态1 – 使能,0 – 禁止,-1 –不支持 */
    SWORD32  sdwSpeed;      /* 硬盘工作速率设置。-1 不支持 */
    SWORD32  sdwNcq;        /* NCQ开关，只对SATA盘有效. 1 – 打开, 0 – 关闭, -1 不支持 */
    /* SWORD32  sdwBadSecsFind;      */                                  /*检测到的坏扇区数目 */

    CHAR     ucModelName[MAX_PD_MODEL_NAME_LEN];    /* model名称 */
    CHAR     ucPhyType[MAX_PD_TYPE_LEN];            /* 硬盘物理类型(e.g. SATA1, SATA2, SAS, FC) */
    CHAR     ucFwVer[MAX_PD_FW_VER_LEN];            /* Firmware版本.*/
    CHAR     ucSerialNo[MAX_PD_SNO_LEN];            /* 序列号 */
    CHAR     ucStdVerNum[MAX_PD_STD_VER_LEN];       /* 标准版本号. */
    SWORD32  sdwReady;                               /* 硬盘使用模式 TRUE - 就绪, FALSE - 备用 */
    SWORD32  sdwHealthState;                      /* 磁盘的健康状态，即SMART 状态，取值为PD_GOOD(1)、PD_BROKEN(0)*/
}_PACKED_1_ T_PdHwInfo;


typedef struct
{
    SWORD32  sdwSlotId;      /* 磁盘槽位号，取值0~27 */
    SWORD32  sdwEnclosureId; /* 硬盘所在的柜子id */
    SWORD32  sdwPdSrc;       /* ePdSrc   eSrc;取值PD_LOCAL(0), PD_FOREIGN(2)*/
    SWORD32  sdwPdState;     /* ePdState eState;取值PD_CAN_NOT_READ(1), PD_NORMAL_RW(0) */
    SWORD32  sdwPdType;      /* ePdType  eType;取值PD_UNKOWN(3),PD_UNUSED(0),PD_HOTSPARE(4), PD_DATA(5)、PD_CONFLICT(6)*/
    SWORD32  sdwVdId;        /* 虚拟盘Id，取值范围0~MAX_VD_ID(32)   */
    SWORD32  sdwVisibleCtrl;   /*  对该盘可见的控制器及状态*/
}_PACKED_1_ T_PdSwInfo;

#endif

typedef struct
{
    SWORD32  sdwSlotId;     /* 磁盘槽位号，取值0~27(ScanBox);  0~11(SPR10) */
    SWORD32  sdwEnclosureId;/* 硬盘所在的柜子id */
    SWORD32  sdwCache;      /* 硬盘cache状态 1 – 使能, 0 – 禁止, -1 –不支持 */
    SWORD32  sdwPowerSave;  /* 硬盘节能模式状态1 – 使能,0 – 禁止,-1 –不支持 */
    SWORD32  sdwSpeed;      /* 硬盘工作速率设置。-1 不支持 */
}_PACKED_1_ T_PdSetHwInfo;



typedef struct
{
    SWORD32  sdwSlotId;     /* 磁盘槽位号，取值0~27(ScanBox);  0~11(SPR10) */
    SWORD32  sdwEnclosureId;/* 箱体Id，取值 0~实际箱体数*/
    SWORD64  sqwCapacity;   /* 磁盘容量 */
    SWORD32  sdwCache;      /* 硬盘cache状态 1 – 使能, 0 – 禁止, -1 –不支持 */
    SWORD32  sdwPowerSave;  /* 硬盘节能模式SCS_DM_DISK_POWER_ACTIVE  SCS_DM_DISK_POWER_IDLE SCS_DM_DISK_POWER_STANDBY SCS_DM_DISK_POWER_STOP_SLEEP*/
    SWORD32  sdwSpeed;      /* 硬盘工作速率设置。-1 不支持 */
    CHAR     ucModelName[MAX_PD_MODEL_NAME_LEN];    /* model名称 */
    CHAR     ucPhysicalType[MAX_PD_TYPE_LEN];            /* 硬盘物理类型(e.g. SATA1, SATA2, SAS, FC) */
    CHAR     ucFwVer[MAX_PD_FW_VER_LEN];            /* Firmware版本.*/
    CHAR     ucSerialNo[MAX_PD_SNO_LEN];            /* 序列号 */
    CHAR     ucStdVerNum[MAX_PD_STD_VER_LEN];       /* 标准版本号. */
    SWORD32  sdwHealthState;                      /* 磁盘的健康状态，包含可访问性和SMART 状态，取值为PD_GOOD(1)、PD_BROKEN(0)*/

    SWORD32  sdwSourceType;       /* ePdSrc   eSrc;取值PD_LOCAL(0), PD_FOREIGN(2)*/
    SWORD32  sdwLogicType;   /* ePdType  eType;取值PD_UNKOWN(3),PD_UNUSED(0),PD_HOTSPARE(4), PD_DATA(5)、PD_CONFLICT(6)*/
    CHAR  cVdName[MAX_BLK_NAME_LEN];            /* 所属虚拟盘名称*/
    SWORD32  sdwVisibleCtrl;   /*  对该盘可见的控制器及状态*/
}_PACKED_1_ T_HddInfo;



typedef struct
{
    SWORD32 sdwPdSlotId;     /* 磁盘槽位号，取值0~27(ScanBox);  0~11(SPR10) */
    SWORD32 sdwPdEnclosureId; /* 箱体Id */
    SWORD32 sdwPercent;       /* 扫描进度值，不带百分号 */
}_PACKED_1_ T_PdScanProgress;

typedef struct
{
    SWORD32 sdwPdNum;     /* 磁盘槽位号，取值0~27(ScanBox);  0~11(SPR10) */
    T_PdScanProgress tPdScanProgress[MAX_PD_NUM_IN_SYS];
}_PACKED_1_ T_AllPdScanProgress;


typedef struct
{
    SWORD32  sdwSlotId;      /* 磁盘槽位号，取值0~27(ScanBox);  0~11(SPR10) */
    SWORD32  sdwEnclosureId;  /* 箱体Id，取值 0~实际箱体数*/
    SWORD32  sdwCtrlFlg;          /* 扫描控制标志，取值:PD_SCAN_START(0)、PD_SCAN_PAUSE(1)、PD_SCAN_STOP(2)*/
    T_Schedule tSchedule;         /* 该类型值缺省时，表明扫描立即执行*/
}_PACKED_1_ T_PdScanCtrl;

typedef struct
{
    SWORD32 sdwPdSlotId;      /* 磁盘槽位号，取值0~27(ScanBox);  0~11(SPR10) */
    SWORD32 sdwPdEnclosureId;  /* 箱体Id */
    SWORD32 sdwControl;        /* 控制方式，ON-开启可视指示；OFF-关闭可视指示 */
}_PACKED_1_ T_PdLed;


typedef struct
{
    SWORD32 sdwWarnSwitch;       /* 控制方式，ON-开启；OFF-关闭 */
}_PACKED_1_ T_PdWarnSwitch;

typedef struct
{
    SWORD32 sdwPdEnclosureId;  /* 箱体Id */
    SWORD32 sdwPdSlotId;            /* 磁盘槽位号，取值0~27(ScanBox);  0~11(SPR10) */
    WORD32 dwFinishRate;  /* 磁盘扫描完成情况-1--100 */
    WORD32 dwBblNum;  /* 磁盘扫描坏块数目 */

}_PACKED_1_ T_PdScanStatus;

typedef struct
{
    WORD32 sdwPdBadBlockWarnNum;            /* 系统中磁盘的告警坏块数目，取值:>=0 */
}_PACKED_1_ T_PdBadBlockWarnNum;

/* 此结构体参考了smart工具的设计 */
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
    SWORD32 sdwPdSlotId;            /* 磁盘槽位号，取值0~27(ScanBox);  0~11(SPR10) */
    SWORD32 sdwPdEnclosureId;  /* 箱体Id */
    BYTE ucAucPdName[12];   /* 磁盘设备名 如sda */
    SWORD32 cIsSupportSmart;  /* 磁盘是否支持SMART */
    SWORD32 cIsSmartEnable;   /* 磁盘SMART功能是否开启 */
    SWORD32 cSmartStatus;     /* 磁盘的SMART状态是否OK */
    SWORD32 dwSmartInfoCount;/*SMART属性个数*/
    T_DmSmartDetailInfo tSmartDetailInfo[30];   /* 磁盘SMART属性详细信息 */
    SWORD32 sdwSelfTestTime;  /* 执行自检所需时间 */
}_PACKED_1_  T_PdSmartStateInfo;


typedef struct
{
    SWORD32 sdwPdSlotId;            /* 磁盘槽位号，取值0~27(ScanBox);  0~11(SPR10) */
    SWORD32 sdwPdEnclosureId;       /* 硬盘所在的柜子id */
    WORD32 dwRawReadErrorRate;      /* 01 数据读取错误率 最坏之应该大于阀门值 */
    WORD32 dwSpinUpTime;            /* 03 马达旋转到标准转速所需的时间 毫秒为单位 */
    WORD32 dwStartStopCount;        /* 04 启动、停止计数 仅供参考，无阀门值 */
    WORD32 dwRealloctedSectorsCount;    /* 05 重新分配的扇区数，越小越好 */
    WORD32 dwSeekErrorRate;             /* 07 寻道错误率 */
    WORD32 dwPowerOnHours;              /* 09 累计通电时间  越低越好，无阀门值 */
    WORD32 dwSpinRetryCount;            /* 10 马达重试次数 越低越好，无阀门值 */
    WORD32 dwCalibrationRetryCount;     /* 11 校准重试次数  越低越好，无阀门值 */
    WORD32 dwPowerCycleCount;           /* 12 通电周期计数（硬盘开关电源的次数） 仅供参考，无阀门值 */
    WORD32 dwPowerOffRetractCount;      /* 192 断电磁头缩回计数 越低越好 无门限值 富士通专有 */
    WORD32 dwLoadCycleCount;            /* 193 磁头伸出周期计数 */
    WORD32 dwTemperature;               /* 194 温度 */
    WORD32 dwRealloctedEventCount;      /* 196 重新映射的扇区事件计数 */
    WORD32 dwCurrentPendingSector;      /* 197 当前待映射的扇区数 */
    WORD32 dwUncorrectableSectorCount;  /* 198 脱机无法校验的扇区数 */
    WORD32 dwUDMACrcErrorCount;         /* 199 Ultra DMA CRC错误计数 */
    WORD32 dwWriteErrorRate;            /* 200 写扇区的错误计数 越低越好 */
}_PACKED_1_ T_PdSmartInfo;



typedef struct
{
    SWORD32     sdwPdNum;                     /* 磁盘个数，取值MAX_ENC_NUM_IN_SYS(4) * MAX_PD_NUM_IN_ENC(16) */
    T_HddInfo   tHddInfo[MAX_PD_NUM_IN_SYS];
}_PACKED_1_ T_AllHddInfo;


/***************************************Blk*************************************/

/***************************************Vol*************************************/
/* BLK与SCS不能通用该结构体 用户不需要填充Owner*/
typedef struct
{
    CHAR     cVdName[MAX_BLK_NAME_LEN];  /*VD 名称,不能为空*/
    CHAR     cVolName[MAX_BLK_NAME_LEN]; /* Vol名称，不能为空*/
    SWORD32  sdwChunkSize;  /* new chunk size,其为4的倍数，单位:KB， 取值MIN_CHUNK_SIZE(4)~MAX_CHUNK_SIZE(512 )*/
    SWORD64  sqwCapacity;   /* capacity of vol in sectors, KB */
    SWORD32  sdwCtlPrefer;  /* 首选控制器,暂时为0,1 */
    SWORD32  sdwCachePolicy;       /* CACHE 写回写穿策略, 取值CACHE_WRITE_BACK_POLICY(0),CACHE_WRITE_THROUGH_POLICY(1) */
    SWORD32  sdwAheadReadSize;  /* cache预读系数,取值0~2048 ，无单位*/
}_PACKED_1_ T_CreateVol;

/* BLK与SCS可以通用该结构体 */
typedef struct
{

    CHAR     cVdName[MAX_BLK_NAME_LEN];  /*VD 名称,不能为空，入参*/
    SWORD32   sdwVolType;                   /* 卷类型，包含普通卷（NORM_VOL）、快照卷（SNAPSHOT_VOL）、拷贝（CLONE_VOL） */
    SWORD32  sdwVolNum;           /* 卷的数目 */
    CHAR     cVolNames[MAX_VOL_NUM_ON_VD][MAX_BLK_NAME_LEN];   /* 指定VD 上的卷名称列表 */
}_PACKED_1_ T_AllVolNamesOnVd;


typedef struct
{
  CHAR   cVolName[MAX_BLK_NAME_LEN]; /* Vol名称，不能为空，入参*/
  WORD32 sdwReadTotalSectors;   /* 读扇区总数*/
  WORD32 sdwWriteTotalSectors;  /* 写扇区总数*/
  WORD32 sdwReadTotalIos;            /* 读IO总次数*/
  WORD32 sdwWriteTotalIos;            /* 写IO总次数*/

  WORD32 sdwReadErrorSectors;   /* 读出错扇区数目*/
  WORD32 sdwWriteErrorSectors;  /* 写出错扇区数目*/
  WORD32 sdwReadErrorIos;            /* 读出错IO 数目*/
  WORD32 sdwWriteErrorIos;            /* 写出错IO 数目*/
}_PACKED_1_ T_GetVolStat;


/* BLK与SCS可以通用该结构体 */
typedef struct
{
    CHAR    cVolName[MAX_BLK_NAME_LEN];  /* Vol Name ，入参 */
}_PACKED_1_ T_VolName;

typedef struct
{
    CHAR    cVolName[MAX_BLK_NAME_LEN];    /* Vol名称，不能为空 ，入参*/
    CHAR    cVolNewName[MAX_BLK_NAME_LEN]; /* 待修改的Vol名称，不能为空，入参*/
}_PACKED_1_ T_VolRename;

/* BLK与SCS可以通用该结构体 */
typedef struct
{

    CHAR     cVolName[MAX_BLK_NAME_LEN]; /* Vol名称，不能为空，入参*/
    SWORD64  sqwExpandSize; /* expand size  ，单位KB ，入参*/
}_PACKED_1_ T_ExpVol;

/* BLK与SCS可以通用该结构体 */
typedef struct
{
    CHAR     cVolName[MAX_BLK_NAME_LEN]; /* Vol名称，不能为空，入参*/
    SWORD32  sdwNewChunk; /* new chunk size,其为4的倍数,单位:KB ， 取值MIN_CHUNK_SIZE(4)~MAX_CHUNK_SIZE(512 )，入参*/
}_PACKED_1_ T_ReSegVol;


/* BLK与SCS可以通用该结构体 最后一个成员由SCS填充*/
typedef struct
{
    CHAR      cVolName[MAX_BLK_NAME_LEN]; /* Vol名称,不能为空*/
    CHAR      cVdName[MAX_BLK_NAME_LEN];  /*VD 名称,不能为空*/
    SWORD64   sqwCapacity;  /* vol capacity  ,KB*/
    SWORD32   sdwLevel;     /* eRaidlevel, RAID level, defined in struct eRaidlevel ,取值RAID0(0),RAID1(1),RAID3(3), RAID5(5),RAID6(6),
                                                   RAIDTP(7),RAID10(10),RAID30(30),RAID50(50),RAID60(60),NRAID(200),RAID_LEVEL_INVALID(0xffff) */
    SWORD32   sdwState;     /* eVolState , vol state defined in struct eVolState,取值VOL_S_GOOD(0),
                                               VOL_S_FAULT(1),  VOL_S_ERR(2),VOL_S_CHUNKSIZE_MIGRATION(3),
                                               VOL_S_EXPAND(4),VOL_S_UNREADY(5),VOL_S_INVALID(0xffff) */
    SWORD32   sdwStripeSize; /* vol chunk size ,KB*/
    SWORD32   sdwOwnerCtrl;                   /* owner control ,暂时为0,1 */
    SWORD32   sdwPreferCtrl;   /*  首选控制器,暂时为0,1 */
    T_Time    tCreateTime;    /* 创建时间  */
    SWORD32   sdwMapState;    /* 卷的映射状态，取值MAPPED_VOL(1),NO_MAPPED_VOL(0) */
    SWORD32   sdwCachePolicy; /* CACHE 写回写穿策略, 取值CACHE_WRITE_BACK_POLICY(0),CACHE_WRITE_THROUGH_POLICY(1) */
    SWORD32   sdwAheadReadSize;/* cache预读系数,取值0~2048 */
    SWORD32   sdwSvolNum;   /* 源卷上快照卷的个数 ，出参*/
    SWORD32   sdwCloneNum;   /* 源卷上克隆卷的个数 ，出参*/
}_PACKED_1_ T_VolInfo;

typedef struct
{
    CHAR    cVolName[MAX_BLK_NAME_LEN]; /* Vol名称,不能为空,入参*/
    SWORD32 sdwPrefeCtl;                /* 首选控制器,暂时为0,1  ，入参*/
}_PACKED_1_ T_VolPreferCtrl;


/* BLK与SCS可以通用该结构体 */
typedef struct
{
    CHAR     cVolName[MAX_BLK_NAME_LEN]; /* Vol名称,不能为空,  入参*/
    SWORD32  sdwTaskType;  /* eVolTaskType, task type, defined in struct eLdTaskType,
                                               取值VOL_TASK_CHUNK(0),VOL_TASK_RESHAPE(1) */
    T_Time   tStartTime;  /* 开始时间  */
    SWORD32  sdwPercent;   /* progress of task */
    T_Time   tFinishTime; /* 结束时间  */
    /* BYTE ucDelay;  */    /* reserved */
}_PACKED_1_ T_VolGetTaskInfo;


typedef struct
{
    CHAR    cVolName[MAX_BLK_NAME_LEN]; /* Vol/Svol名称,不能为空,入参*/
    SWORD32 sdwCachePolicy;  /* CACHE 写回写穿策略, 取值CACHE_WRITE_BACK_POLICY(0),CACHE_WRITE_THROUGH_POLICY(1) ，入参*/
    SWORD32 sdwAheadReadSize;/* cache预读系数 ，入参*/
}_PACKED_1_ T_VolCacheInfo;

/***************************************SNAP************************************/
/* BLK与SCS可以通用该结构体 */
typedef struct
{

    CHAR     cVolName[MAX_BLK_NAME_LEN];    /* Vol名称*/
    CHAR     cSvolName[MAX_BLK_NAME_LEN];   /* 快照卷名称 */
    SWORD32  sdwSvolChunkSize;              /*Svol chunk size ,快照卷条带，KB，取值同Rvol 条带*/

    SWORD32  sdwSvolThr;                    /* Svol 容量的报警阈值，百分比表示,比如80表示Rvol的空间被使用到80%时报警*/
    SWORD32  sdwSvolNotify;                 /*Svol空间不足时的处理策略，取值: FAIL_SVOL(0)、 FAIL_WRITE_TO_VOL(1)*/
    SWORD32  sdwCapacityPercentage;         /* 实体卷容量是源卷的百分比，最低10% */

}_PACKED_1_ T_SvolCreate;



/* BLK与SCS可以通用该结构体 */
typedef struct
{
    CHAR    cSvolName[MAX_BLK_NAME_LEN];    /* 快照卷名称，入参 */
}_PACKED_1_ T_SvolName;

typedef struct
{

    CHAR    cSvolName[MAX_BLK_NAME_LEN];       /* 快照卷名称，入参 */
    CHAR    cSvolNewName[MAX_BLK_NAME_LEN];    /* 快照卷新名称 ，入参*/
}_PACKED_1_ T_SvolRename;

/* BLK与SCS可以通用该结构体 */
typedef struct
{

    CHAR     cSvolName[MAX_BLK_NAME_LEN];  /*快照卷名称,入参 */
    CHAR     cVolName[MAX_BLK_NAME_LEN];   /* Vol名称*/
    SWORD32  sdwState;                     /* The state of the svol, the possible values are %SVOL_S_ACTIVE, %SVOL_S_FAULTY.*/
    T_Time   tCreateTime;                  /* Svol create time */
    SWORD64  uqwCapacity;                  /* Snapshot volume size (in sectors),快照卷容量,KB */
    SWORD64  uqwUsedCapacity;              /* Used Snapshot volume size (in sectors),已使用快照卷容量 */
    SWORD32  sdwSvolThr;                   /* 快照卷的报警阈值，百分比表示*/
    SWORD32  sdwSvolNotify;                /* Overflow treatment strategies of rvol ("fail svol" or "fail write to bvol")，容量溢出后的处理方式标记 */
    SWORD32 sdwLunMapState;                 /* 快照卷的映射状态，取值MAPPED_VOL(1),NO_MAPPED_VOL(0) ，出参*/

}_PACKED_1_ T_SvolInfo;

/* BLK与SCS可以通用该结构体 */
typedef struct
{
    CHAR       cVolName[MAX_BLK_NAME_LEN];  /*卷名称 ，入参*/
    SWORD32    sdwSvolNum;  /*快照卷数目*/
    T_SvolInfo tSvolInfo[MAX_SVOL_NUM_IN_VOL];    /*快照卷信息列表*/
}_PACKED_1_ T_AllSvolInfoOnVol;

typedef struct
{
    CHAR    cSvolName[MAX_BLK_NAME_LEN];  /*快照卷名称 ，入参*/
    SWORD32 sdwSvolThr;     /* Warning threshold of rvol ，入参*/
    SWORD32 sdwSvolNotify;  /* Overflow treatment strategies of Svol (fail svol or fail write to bvol)，入参*/
}_PACKED_1_ T_SvolThr;

#if 0
typedef struct
{
    SWORD32 sdwVdId;              /* 待恢复的源卷所在的Vd ID */
    SWORD32 sdwVolId;     /* 待恢复的源卷ID */
    SWORD32 sdwSvolId;    /* 待恢复的快照ID*/
}_PACKED_1_ T_SvolRestore;
#endif



/***************************************Clone Vol************************************/
/*CloneVol创建*/
typedef struct
{
    CHAR    scCvolName[MAX_BLK_NAME_LEN];   /* 拷贝卷名*/
    CHAR    scVdName[MAX_BLK_NAME_LEN];      /* 拷贝卷所在vd, 必须与源卷不同*/
    CHAR    scBvolName[MAX_BLK_NAME_LEN];    /* 源卷名*/
    SWORD32 sdwCvolCapacity;               /* clone卷的容量KB,要求不小于源卷 */
    SWORD32 sdwCvolChunkSize;   /* clone卷的chunk单位KB */
    SWORD32 sdwInitSync;     /* 是否进行初始同步 , 1-YES, 0--NO*/
    SWORD32 sdwProtectRestore;   /* 是否启动保护拷贝, 1-YES, 0--NO */
    SWORD32 sdwPri;        /* 同步或反同步任务优先级,取值:SYNC_LOW(0),SYNC_MEDIUM(1),SYNC_HIGH(2)*/
}_PACKED_1_ T_CreateCvol;

/* CloneVol信息*/
typedef struct
{
    CHAR    scCvolName[MAX_BLK_NAME_LEN];  /* 拷贝卷的名字 ,入参*/
    CHAR    scVdName[MAX_BLK_NAME_LEN];      /* 拷贝卷所在vd*/
    CHAR    scBVolName[MAX_BLK_NAME_LEN];   /* 源卷名*/
    SWORD32 sdwCvolCapacity;   /* clone卷的容量KB */
    SWORD32 sdwCvolChunkSize;   /* clone卷的chunk单位,KB */
    SWORD32 sdwInitSync;     /* 是否进行初始同步, 1-YES, 0--NO */
    SWORD32 sdwProtectRestore;   /* 是否启动保护拷贝, 1-YES, 0--NO */
    SWORD32 sdwPri;   /* 同步或反同步的速率 */
    SWORD32 sdwRelation;   /* 拷贝卷与源卷的关联关系, 1--关联, 0--分离 */
    SWORD32 CvolState;   /* 拷贝卷的状态 */
    SWORD32 sdwLunMapState;  /* 拷贝卷的映射状态，取值MAPPED_VOL(1),NO_MAPPED_VOL(0) ，出参*/
 }_PACKED_1_ T_CvolInfo;

/* 卷上的拷贝卷信息*/
typedef struct
{
    CHAR    scVolName[MAX_BLK_NAME_LEN];        /* 拷贝卷的名字 ,入参*/
    SWORD32 sdwCvolNum;   /* 卷上的所有拷贝卷数目 */
    CHAR    scCvolNames[MAX_CLONE_IN_VOL][MAX_BLK_NAME_LEN];   /* 卷上的所有拷贝名字列表 */
}_PACKED_1_ T_CvolNamesOnVol;

/*拷贝卷同步或者反同步的优先级(速率)*/
typedef struct
{
    CHAR    scCvolName[MAX_BLK_NAME_LEN];  /* 拷贝卷的名字 ，入参*/
    SWORD32 sdwPri;   /* 同步或反同步的优先级，取值:SYNC_LOW(0),SYNC_MEDIUM(1),SYNC_HIGH(2) */
}_PACKED_1_ T_CvolPriority;


typedef struct
{
    CHAR    scCvolName[MAX_BLK_NAME_LEN];  /* 拷贝卷的名字，入参 */
    SWORD32 sdwProtectRestore;   /* 是否启动保护拷贝, 1-YES, 0--NO */
}_PACKED_1_ T_CvolProtect;

typedef struct
{
    CHAR    scCvolName[MAX_BLK_NAME_LEN];  /* 拷贝卷的名字 */
}_PACKED_1_ T_CvolName;


typedef struct
{
    CHAR    scCvolName[MAX_BLK_NAME_LEN];           /* 拷贝卷名称，入参 */
    CHAR    scCvolNewName[MAX_BLK_NAME_LEN];    /* 拷贝卷新名称 ，入参*/
}_PACKED_1_ T_CvolRename;


typedef struct
{
    SWORD32 sdwVolObjectType;                   /* 卷对象类型 ,取值: NORM_VOL / SNAPSHOT_VOL / CLONE_VOL*/
    CHAR    scVolObjectName[MAX_BLK_NAME_LEN];  /* 卷对象的名称*/
}_PACKED_1_ T_VolObjectInfo;


typedef struct
{
    CHAR     scMapGrpName[MAX_MAPGRP_NAME_LEN]; /* 映射组名称，入参*/
    SWORD32 sdwVolObjectNum;                                /* 系统中未映射的卷对象数目*/
    T_VolObjectInfo    tVolObjectList[256];  /* 系统中未映射的卷对象的信息列表,  暂定512 个对象*/
}_PACKED_1_ T_VolObjectInfoList;






/***************************************Vd**************************************/
/*该结构体用的是slot id和enc id，另外blk需要的owner由SCS补充，所以blk不能直接使用该结构体*/
typedef struct
{
    CHAR     cVdName[MAX_BLK_NAME_LEN];  /*VD 名称,不能为空*/
    SWORD32  sdwVdRaidLevel;  /* eRaidlevel, RAID level, defined in struct eRaidlevel ,取值RAID0(0),RAID1(1),RAID3(3), RAID5(5),RAID6(6),
                                                   RAIDTP(7),RAID10(10),RAID30(30),RAID50(50),RAID60(60),NRAID(200),RAID_LEVEL_INVALID(0xffff) */
    SWORD32  sdwPdNum;                     /* 最大32 */
    T_PdId   tPdId[MAX_PD_NUM_IN_VD];      /* 磁盘信息列表 */
    SWORD32  sdwAutoRebuild;    /* 自动重建开关 ON自动重建，OFF不自动重建 */
}_PACKED_1_ T_CreateVd;

/* blk和SCS可以通用 */
typedef struct
{
    SWORD32  sdwVdNum;   /* 虚拟盘数目 */
    CHAR     cVdNames[MAX_VD_NUM_IN_SYS][MAX_BLK_NAME_LEN];    /*  所有虚拟盘name列表 */
}_PACKED_1_ T_AllVdNames;


/* blk与SCS可以通用 */
typedef struct
{
    CHAR     cVdName[MAX_BLK_NAME_LEN];    /*VD 名称,不能为空, 入参*/
    SWORD32  sdwVdRaidLevel;/* eRaidlevel, RAID level, defined in struct eRaidlevel ,取值RAID0(0),RAID1(1),RAID3(3), RAID5(5),RAID6(6),
                                                   RAIDTP(7),RAID10(10),RAID30(30),RAID50(50),RAID60(60),NRAID(200),RAID_LEVEL_INVALID(0xffff) */
    SWORD32  sdwFullPdNum;                  /*vd创建时,   即vd满配时,磁盘个数*/
    SWORD32  sdwPdNum;                      /*当前情况下VD 成员盘个数*/
    T_PdId   tPdId[MAX_PD_NUM_IN_VD];       /* 磁盘信息列表 */
    SWORD32  sdwAutoRebuild;                /* 自动重建开关ON(1)/OFF(0) */
    WORD64   sqwTotalCapacity;              /*  虚拟盘容量,单位KB */
    WORD64   sqwFreeCapacity;               /*  空闲容量,单位KB */
    WORD64   sqwMaxContiguousFreeSpace;     /*  最大连续空闲空间,单位KB */
    T_Time   tCreateTime;                   /* Vd 创建时间 */
    SWORD32  sdwState;                      /* VD的状态信息,目前支持的状态有:VD_GOOD(0)、VD_FAULT(1)、VD_DEGRADE(2)  */
    SWORD32  local;                         /* local为0表示外来VD,local为1表示本地VD */
}_PACKED_1_ T_VdInfo;


#if 0
typedef struct
{
    SWORD32    sdwCtlId;                   /*控制器ID*/
    SWORD32    sdwState;                   /* VD状态信息*/
    SWORD32    sdwPdNum;                   /*当前情况下VD 成员盘个数*/
    T_PdId   tPdId[MAX_PD_NUM_IN_VD];       /* 磁盘信息列表 */
}_PACKED_1_ T_VdSingleCtlInfo;

typedef struct
{
    CHAR         cVdName[MAX_BLK_NAME_LEN];    /*VD 名称,不能为空, 入参*/
    SWORD32      sdwVdRaidLevel;               /* eRaidlevel, RAID level, defined in struct eRaidlevel ,取值RAID0(0),RAID1(1),RAID3(3), RAID5(5),RAID6(6),
                                                   RAIDTP(7),RAID10(10),RAID30(30),RAID50(50),RAID60(60),NRAID(200),RAID_LEVEL_INVALID(0xffff) */
    SWORD32      sdwFullPdNum;                 /*vd创建时,   即vd满配时,磁盘个数*/
    SWORD32      sdwAutoRebuild;                /* 自动重建开关ON(1)/OFF(0) */
    WORD64       sqwTotalCapacity;              /*  虚拟盘容量,单位KB */
    WORD64       sqwFreeCapacity;               /*  空闲容量,单位KB */
    WORD64       sqwMaxContiguousFreeSpace;       /*  最大连续空闲空间,单位KB */
    T_Time       tCreateTime;                   /* Vd 创建时间 */
    SWORD32      local;                           /* local为0表示外来VD,local为1表示本地VD */
    T_VdSingleCtlInfo  tVdSingleCtlInfo[2];     /*VD两端控制器不对称信息,tVdSingleCtlInfo[0]表示本端，tVdSingleCtlInfo[1]表示对端*/
}_PACKED_1_ T_VdInfo;
#endif

typedef struct
{
    CHAR       cVdName[MAX_BLK_NAME_LEN];     /*VD 名称,不能为空, 入参*/
    CHAR       cMdType[MAX_PD_NUM_IN_VD];     /* 成员盘列表类型---1:local,2:peer,3:dual ，出参*/
    WORD32     dwMdNum;                       /* 成员盘数目，出参 */
    T_PdId     tPdId[MAX_PD_NUM_IN_VD];       /* 磁盘信息列表，出参 */
}_PACKED_1_ T_MdList;

/* BLK与SCS可以通用 */
typedef struct
{
    CHAR     cVdName[MAX_BLK_NAME_LEN];    /*VD 名称,不能为空, 入参*/
    /* 待商量确定*/
    SWORD32  sdwTaskType;  /*任务类型*/
    T_Time   tStartTime;  /* 开始时间  */
    SWORD32  sdwPercent;   /* 任务进度 */
    T_Time   tFinishTime; /* 结束时间  */
    /* BYTE ucDelay;  */    /* reserved */
}_PACKED_1_ T_VdGetTaskInfo;

/* BLK与SCS应该不可以通用, blk需要的是设备号*/
typedef struct
{
    CHAR      cVdName[MAX_BLK_NAME_LEN];    /*VD 名称,不能为空, 入参*/
    SWORD32   sdwSpareHdNum;                /* 热备盘数目，入参*/
    T_PdId    tPdId[MAX_PD_NUM_ADD];          /* 磁盘信息列表 ，入参*/
}_PACKED_1_ T_VdRebuild;/* 虚拟盘重建（恢复）*/



/* blk与SCS可以通用 */
typedef struct
{
    CHAR    cVdName[MAX_BLK_NAME_LEN];  /*VD 名称,不能为空，入参*/
}_PACKED_1_ T_VdName;

typedef struct
{
    CHAR     cVdName[MAX_BLK_NAME_LEN];     /*VD 名称,不能为空，入参*/
    CHAR     cVdNewName[MAX_BLK_NAME_LEN];  /*待修改的VD 名称,不能为空，入参*/
}_PACKED_1_ T_VdRename;

/* blk与SCS可以通用 */
#if 0
typedef struct
{
    SWORD32 sdwVdId; /* 虚拟盘Id取值范围0~MAX_VD_ID(32)-1   */
    SWORD32 sdwPreferCtlId; /* 首选控制器，暂时为0,1 */
}_PACKED_1_ T_VdPreferCtl;
#endif

/* blk与SCS可以通用 */
typedef struct
{
    CHAR    cVdName[MAX_BLK_NAME_LEN];  /*VD 名称,不能为空，入参*/
    SWORD32 sdwAutoRebuild;  /* 自动重建开关，ON(1)/OFF(0) ，入参*/
}_PACKED_1_ T_VdAutoRebuild;

/* blk与SCS不可以通用 上层是槽位号，blk是设备号*/
typedef struct
{
    CHAR       cVdName[MAX_BLK_NAME_LEN];  /*VD 名称,不能为空，入参*/
    SWORD32    sdwAddPdNum;                /*待增加磁盘数目，，入参*/
    T_PdId     tPdId[MAX_PD_NUM_ADD];      /* 磁盘信息列表 ，，入参*/
    SWORD32    sdwDefragFlg;               /* 扩展的过程中是否整理碎片，ON-碎片整理，OFF-不进行碎片整理，，可选入参 */
}_PACKED_1_ T_VdExpand;

/* SCS与blk可以通用 */
typedef struct
{

    CHAR    cVdName[MAX_BLK_NAME_LEN];  /*VD 名称,不能为空，，入参*/
    SWORD32 sdwVdRaidLevelNew;         /* 新的Raid级别 ，，入参*/
    SWORD32 sdwDefragFlg;              /* 变级的过程中是否整理碎片，ON-碎片整理，OFF-不进行碎片整理 ，可选入参*/
}_PACKED_1_ T_VdRelevel;




/* 后台任务相关*/
typedef struct
{
    SWORD32 sdwTaskId;           /*任务ID*/
    CHAR       cObjectName[MAX_BLK_NAME_LEN];  /*任务所属对象名*/
    WORD32   sdwTaskType;  /* 任务类型,取值见宏定义*/
    SWORD32   sdwTaskState;  /* 任务状态，取值TASK_RUNNING、TASK_PAUSE、TASK_DELAY*/
    T_Time      tStartTime;       /* 开始时间，年、月、日、时、分、秒*/
    SWORD32  sdwTaskProgress;     /* 任务进度，整数，不带%*/
}_PACKED_1_ T_TaskInfo;


typedef struct
{
    SWORD32 sdwTaskNum;           /*任务ID*/
    T_TaskInfo tTaskInfo[MAX_TASK_NUM_IN_SYS];    /*系统中所有后台任务列表*/
}_PACKED_1_ T_TaskInfoList;


typedef struct
{
    SWORD32 sdwTaskId;           /*任务ID*/
    SWORD32 sdwTaskCtrl;         /*后台任务操作，取值TASK_CTRL_RUN、TASK_CTRL_PAUSE、TASK_CTRL_DEL*/
}_PACKED_1_ T_TaskCtrl;

typedef struct
{
    CHAR        cObjectName[MAX_OBJECT_NAME_LEN];      /* 任务所属对象名,input */
    WORD32      sdwTaskType;                           /* 任务类型,取值见宏定义,input */
    SWORD32     sdwPrarmLen;                           /* 任务参数长度,input */
    CHAR        ucTaskPrarm[MAX_TASK_PRARM_LEN];       /* 任务所需参数,input */
}_PACKED_1_ T_TaskAddInfo;

/* 计划任务相关*/

typedef struct
{
    SWORD32 swTaskId;      //计划任务id号
    SWORD32 scTaskType;    //任务类型，参考eSchdTaskType定义
    CHAR    cObjectName[MAX_BLK_NAME_LEN];   //计划任务所属的对象名称
    T_Time  tStartTime;  //任务启动时间
    T_Time  tNextRunTime;  //任务下次运行时间
    SWORD32 sdwSchedPeriod; //计划任务的周期特性，取值SCHED_ONCE、SCHED_DAYLY、SCHED_WEEKLY、SCHED_MONTHLY
       CHAR    cTaskDescription[128];   // 计划任务简要描述
}_PACKED_1_ T_ScheduleTask;

typedef struct
{
    SWORD32 sdwNum; //计划任务数目
    T_ScheduleTask tScheduleTask[MAX_SCHEDULE_NUM]; //计划任务信息列表
}_PACKED_1_ T_ScheduleList;

typedef struct
{
    SWORD32 swTaskId; //计划任务id号，入参
}_PACKED_1_ T_ScheduleDel;


/***************************************STS*************************************/
#if 0

typedef struct
{
    SWORD32  sdwLunGlbId;  /* Lun全局Id,0~MAX_LUN_NUM_IN_SYS(1024)-1 */
    SWORD32  sdwTgtType; /* 卷or快照,宏定义为NORM_VOL和SNAP_ENTITY_VOL*/
    SWORD32  sdwTgtId;     /*VdId 、VolId 、SvolId的组合，其中VdId为第0字节，VolId为第1字节，SvolId为第2字节*/
    SWORD32  sdwCachePolicy; /* CACHE 写回写穿策略, 取值CACHE_WRITE_BACK_POLICY(0),CACHE_WRITE_THROUGH_POLICY(1) */
    SWORD32  sdwAheadReadSize;/* cache预读系数 */
}_PACKED_1_ T_LunSimpleInfo;

typedef struct
{
    SWORD32 sdwNum;
    SWORD32 sdwLunGlbId[MAX_LUN_NUM_IN_SYS]; /* Lun全局Id,0~MAX_LUN_NUM_IN_SYS(1024)-1 */
}_PACKED_1_ T_AllLunId;

typedef struct
{
    SWORD32 sdwLunGlbId;     /* Lun全局Id,0~MAX_LUN_NUM_IN_SYS(1024)-1 */
    SWORD32 sdwCachePolicy;  /* CACHE 写回写穿策略, 取值CACHE_WRITE_BACK_POLICY(0),CACHE_WRITE_THROUGH_POLICY(1) */
    SWORD32 sdwAheadReadSize;/* cache预读系数 */
}_PACKED_1_ T_LunCacheInfo;

typedef struct
{
    SWORD32 sdwLunGlbId;    /* Lun全局Id,0~MAX_LUN_NUM_IN_SYS(1024)-1 */
    SWORD32 sdwLunLocalId;  /* Lun的本地Id,int型值，唯一 */
    SWORD32 sdwMapGrpId;    /* Lun的所属MapGrpId,取值0~MAX_MAP_GROUP_NUM_IN_SYS(16)-1*/
    SWORD32 sdwBlockSize;   /* 数据块大小，取值范围512,512*2,512*4,512*8 */
    SWORD32 sdwAccessAttr;  /* 只读 or 读写 */
}_PACKED_1_ T_LunAddToGrp;

typedef struct
{
    SWORD32 sdwLunLocalId;  /* Lun的本地Id,int型值，唯一 */
    SWORD32 sdwMapGrpId;    /* Lun的所属MapGrpId,取值0~MAX_MAP_GROUP_NUM_IN_SYS(16)-1*/
}_PACKED_1_ T_LunDelFrmGrp;


/* 2010-12-02    xygang  */
typedef struct
{
    CHAR  ucHostName[MAX_INI_NAME_LEN];
    /* CHAP鉴权时Host用户名和密码 */
}_PACKED_1_ T_HostInfo;

/* 2010-12-02    xygang  */
typedef struct
{
    SWORD32 sdwMapGrpId;   /* 映射组的Id,取值0~MAX_MAP_GROUP_NUM_IN_SYS(16)-1 */
    SWORD32 sdwLunNum;     /* 逻辑单元个数*/
    SWORD32 sdwLunLocalId[MAX_LUN_NUM_IN_MAP_GROUP];/* Lun的本地Id,int型值，映射组内唯一 */
    SWORD32 sdwLunGlbId[MAX_LUN_NUM_IN_MAP_GROUP];/* Lun全局Id,0~MAX_LUN_NUM_IN_SYS(1024)-1 */
    SWORD32 sdwHostNum;    /* 主机的个数 */
    T_HostInfo  tHostInfo[MAX_INI_NUM_IN_MAP_GROUP];
}_PACKED_1_ T_MapGrpInfo;


typedef struct
{
    CHAR    ucInitName[MAX_INI_NAME_LEN];
    SWORD32 sdwMapGrpId;  /* Host所属MapGrpId,取值0~MAX_MAP_GROUP_NUM_IN_SYS(16)-1  */
}_PACKED_1_ T_HostInGrp;

typedef struct
{
    SWORD32 sdwNum;
    SWORD32 sdwMapGrpId[MAX_MAP_GROUP_NUM_IN_SYS];/* 映射组的Id,取值0~MAX_MAP_GROUP_NUM_IN_SYS(16)-1 */
}_PACKED_1_ T_AllGrpId;

typedef struct
{
    SWORD32     sdwMapGrpId;/* 映射组的Id,取值0~MAX_MAP_GROUP_NUM_IN_SYS(16)-1 */
    SWORD32     sdwNum;
    T_HostInfo  tHostInfo[MAX_INI_NUM_IN_MAP_GROUP];
}_PACKED_1_ T_AllHostInGrp;

typedef struct
{
    SWORD32 sdwMapGrpId;/* 映射组的Id,取值0~MAX_MAP_GROUP_NUM_IN_SYS(16)-1 */
    SWORD32 sdwNum;
    SWORD32 sdwLunLocalId[MAX_LUN_NUM_IN_MAP_GROUP];/* Lun的本地Id,int型值，唯一 */
    SWORD32 sdwLunGlbId[MAX_LUN_NUM_IN_MAP_GROUP];  /* Lun全局Id,0~MAX_LUN_NUM_IN_SYS(1024)-1 */
}_PACKED_1_ T_AllLunInGrp;

typedef struct
{
    CHAR    ucInitName[MAX_INI_NAME_LEN];
    SWORD32 sdwMapGrpId;/* 映射组的Id,取值0~MAX_MAP_GROUP_NUM_IN_SYS(16)-1 */
}_PACKED_1_ T_HostGrpId;

typedef struct
{
    SWORD32 sdwLunGlbId;   /* Lun全局Id,0~MAX_LUN_NUM_IN_SYS(1024)-1 */
    SWORD32 sdwNum;
    SWORD32 sdwMapGrpId[MAX_MAP_GROUP_NUM_IN_SYS];/* 映射组的Id,取值0~MAX_MAP_GROUP_NUM_IN_SYS(16)-1 */
}_PACKED_1_ T_LunGrpId;

typedef struct
{
    SWORD32 sdwCtrlId;        /* 控制器Id ,暂时定义为0、1，对应的宏定义CTRL0、CTRL1*/
    SWORD32 sdwLunGlbId;  /* Lun全局Id,0~MAX_LUN_NUM_IN_SYS(1024)-1 */
    CHAR    ucInitName[MAX_INI_NAME_LEN];    /* 主机名 */
    SWORD32 sdwAction;      /*操作标志: 0/1/2，对应的宏定义为"OFF"、 "ON"、"RES"，分别表示关闭、打开、重置。 */
}_PACKED_1_ T_LunIOStaticSwitch;

typedef struct
{
    SWORD32     sdwCtrlId;        /* 控制器Id ,暂时定义为0、1，对应的宏定义CTRL0、CTRL1*/
    SWORD32     sdwLunGlbId;              /* Lun的全局Id,int型值，唯一 */
    CHAR        ucInitName[MAX_INI_NAME_LEN];         /* 主机名 */
    WORD64      sqwStatReadCount;           /* 统计读的个数 */
    WORD64      sqwStatReadTransimitData;   /* 统计读数据量 */
    WORD64      sqwStatWriteCount;          /* 统计写的个数 */
    WORD64      sqwStatWriteTransimitData;  /* 统计写数据量 */
    WORD64      sqwStatTransimitDataAverage;    /* 平均数据速率 */
    WORD64      sqwStatTransimitDataAll;        /* 数据总量 */
    WORD64      sdwStatCmdTimeAverage;          /* 命令平均执行时间 */
    WORD64      sdwStatCmdTimeMax;              /* 命令最大执行时间 */
    WORD64      sdwCmdSuccessNum ;              /* 命令执行成功的个数 */
    WORD64      sdwCmdTimeAll;                  /* 命令执行的总时间 */
}_PACKED_1_ T_LunIOStaticData;

#endif

typedef struct
{
    SWORD32 sdwLunId;  /* Lun的本地Id, int型值，组内唯一 */
    CHAR    cVolName[MAX_BLK_NAME_LEN]; /* Vol/Svol名称,不能为空*/
    SWORD32 sdwBlockSize;   /* 数据块大小，取值范围512,512*2,512*4,512*8 */
    SWORD32 sdwAccessAttr;  /* 只读 or 读写 */
}_PACKED_1_ T_LunInfo;

typedef struct
{
    CHAR      cMapGrpName[MAX_MAPGRP_NAME_LEN]; /* 映射组名称,不能为空,入参*/
    T_LunInfo tLunInfo;                              /*Lun相关的信息，入参*/
}_PACKED_1_ T_AddVolToGrp;

typedef struct
{
    CHAR     cMapGrpName[MAX_MAPGRP_NAME_LEN]; /* Vol的所属MapGrp名称,入参*/
    SWORD32  sdwLunId;       /* Lun的本地Id，映射组内唯一 ,入参*/
}_PACKED_1_ T_DelVolFrmGrp;


/* 2010-12-02    xygang  */
typedef struct
{
    CHAR  ucHostName[MAX_INI_NAME_LEN];
    /* CHAP鉴权时Host用户名和密码 */
}_PACKED_1_ T_HostInfo;

/* 2010-12-02    xygang  */
typedef struct
{

    CHAR        cMapGrpName[MAX_MAPGRP_NAME_LEN]; /* 映射组名称,不能为空,入参*/
    SWORD32     sdwLunNum;     /* 逻辑单元个数*/
    T_LunInfo   tLunInfo[MAX_LUN_NUM_IN_MAP_GROUP];     /*Lun相关的信息*/
    SWORD32     sdwHostNum;    /* 主机的个数 */
    T_HostInfo  tHostInfo[MAX_INI_NUM_IN_MAP_GROUP];
}_PACKED_1_ T_MapGrpInfo;


typedef struct
{
    CHAR    ucInitName[MAX_INI_NAME_LEN];     /* 主机名 */
    CHAR    cMapGrpName[MAX_MAPGRP_NAME_LEN]; /* 映射组名称*/
}_PACKED_1_ T_HostInGrp;



typedef struct
{
    SWORD32  sdwMapGrpNum;    /* 系统中映射组数目 */
    CHAR     cMapGrpNames[MAX_MAP_GROUP_NUM_IN_SYS][MAX_MAPGRP_NAME_LEN]; /* 所有映射组名称列表*/
}_PACKED_1_ T_AllGrpNames;

typedef struct
{
    CHAR      cMapGrpName[MAX_MAPGRP_NAME_LEN]; /* 映射组名称，入参*/
}_PACKED_1_ T_MapGrpName;

typedef struct
{
    CHAR     cMapGrpName[MAX_MAPGRP_NAME_LEN]; /* 映射组名称，入参*/
    CHAR     cMapGrpNewName[MAX_MAPGRP_NAME_LEN]; /* 映射组新名称，入参*/
}_PACKED_1_ T_MapGrpRename;


typedef struct
{
    CHAR      cVolName[MAX_BLK_NAME_LEN]; /* Vol/Svol名称,不能为空,入参*/
    SWORD32   sdwMapGrpNum;
    CHAR      cMapGrpNames[MAX_MAP_GROUP_NUM_IN_SYS][MAX_MAPGRP_NAME_LEN]; /* 映射组名称*/
}_PACKED_1_ T_GrpNameByVol;

typedef struct
{
    //SWORD32 sdwCtrlId;        /* 控制器Id ,暂时定义为0、1，对应的宏定义CTRL0、CTRL1*/
    CHAR     cMapGrpName[MAX_MAPGRP_NAME_LEN]; /* 映射组名称，入参*/
    SWORD32  sdwLunId;       /* Lun的本地Id，映射组内唯一 ,入参*/
    CHAR     cInitName[MAX_INI_NAME_LEN];    /* 主机名 ，入参*/
    SWORD32  sdwAction;      /*操作标志: 0/1/2，对应的宏定义为"OFF"、 "ON"、"RES"，分别表示关闭、打开、重置。 入参*/
}_PACKED_1_ T_LunIOStaticSwitch;

typedef struct
{
    //SWORD32 sdwCtrlId;        /* 控制器Id ,暂时定义为0、1，对应的宏定义CTRL0、CTRL1*/
    CHAR        cMapGrpName[MAX_MAPGRP_NAME_LEN]; /* 映射组名称，入参*/
    SWORD32     sdwLunId;       /* Lun的本地Id，映射组内唯一 ,入参*/
    CHAR        cInitName[MAX_INI_NAME_LEN];    /* 主机名 ，入参*/
    WORD64      sqwStatReadCount;           /* 统计读的个数 */
    WORD64      sqwStatReadTransimitData;   /* 统计读数据量 ,单位:Byte*/
    WORD64      sqwStatWriteCount;          /* 统计写的个数 */
    WORD64      sqwStatWriteTransimitData;  /* 统计写数据量 ,单位:Byte*/
    WORD64      sqwStatTransimitDataAverage;    /* 平均数据速率 */
    WORD64      sqwStatTransimitDataAll;        /* 数据总量 */
    WORD64      sdwStatCmdTimeAverage;          /* 命令平均执行时间,单位:微秒 */
    WORD64      sdwStatCmdTimeMax;              /* 命令最大执行时间 ,单位:微秒*/
    WORD64      sdwCmdSuccessNum ;              /* 命令执行成功的个数 */
    WORD64      sdwCmdTimeAll;                  /* 命令执行的总时间,单位:微秒 */
}_PACKED_1_ T_LunIOStaticData;
/***************************************CBS*************************************/
typedef struct
{
    SWORD32 sdwRwRatio;      /* 读写所占空间比例中，写占的比例 */
    SWORD32 sdwUnitSize;     /* 缓存单元大小 ，单位:KB*/
    SWORD32 sdwWrPeriod;     /* 定时回写周期 */
    SWORD32 sdwRefreshRatio; /* 回写比例上线 */
    SWORD32 sdwWrEnable;     /* 镜像开关ON/OFF */
}_PACKED_1_ T_SetCacheGlbPolicy;

typedef struct
{
    SWORD32  sdwRwRatio;       /* 读写Cache比例 */
    SWORD32  sdwUnitSizeSet;   /* 最新设置的Cache单元大小，单位KB，值为4、8、16、32、64、128。(重启后生效) */
    SWORD32  sdwCurUnitSize;   /* 当前生效Cache单元大小（值同上） */
    SWORD32  sdwWrPeriod;      /* 刷新周期 */
    SWORD32  sdwRefreshRatio;  /* 刷新比例 */
    SWORD32  sdwWrEnable;      /* Cache镜像开关 值为ON（1）/OFF（1） */
}_PACKED_1_ T_GetCacheGlbPolicy;

typedef struct
{
    SWORD32 sdwTotalBuffs;/*  总的缓存个数 */
    SWORD32 sdwFreeBuffs;      /*  空闲的缓存个数 */
}_PACKED_1_ T_CacheBuffInfo;

typedef struct
{
    //SWORD32 sdwCtrlId;        /* 控制器Id ,暂时定义为0、1，对应的宏定义CTRL0、CTRL1*/
    SWORD32 sdwAction;      /*操作标志: 0/1/2，对应的宏定义为"OFF"、 "ON"、"RES"，分别表示关闭、打开、重置。 */
}_PACKED_1_ T_CacheIOStatSwitch;

typedef struct
{

    WORD64 uqwBcReadCmds;      /* 处理读命令总个数 */
    WORD64 uqwBcReadSize;      /* 读命令处理的总数据量(单位：sector，块)  */
    WORD64 uqwBcWriteCmds;     /* 处理写命令总个数 (单位：sector，块) */
    WORD64 uqwBcWriteSize;     /* 写命令处理的数据总量 */
    WORD64 uqwBcReadHits;      /* 读命令命中个数 */
    WORD64 uqwBcWriteMerge;    /* 合并写个数(目前这个变量未使用) */
    WORD64 uqwBcDirtyBuffers;  /* 脏缓存个数 */
    WORD64 uqwBcCleanBuffers;  /* 干净缓存个数 */
}_PACKED_1_ T_CacheIOStatData;


/***************************************ISCSI***********************************/


typedef struct
{
    CHAR     scIp[MAX_IP_LEN];   /*IP地址*/
    CHAR     scName[MAX_INI_NAME_LEN];
}_PACKED_1_ T_IscsiIniInfo;
typedef struct
{
    WORD32          wTid;/* target Id，目标端口Id */
    WORD32          wCount;
    T_IscsiIniInfo  tIscsiIInfo[MAX_INITIATOR_NUM];
}_PACKED_1_ T_IscsiIniInfoList;

typedef struct
{
    WORD64     uqwSessId;                  /* Session Id */
    WORD32     udwPortId;                  /* port  Id */
    CHAR       cPortIp[MAX_IP_LEN];        /* 端口的物理IP */
    CHAR       cInitIp[MAX_IP_LEN];          /* Initiator IP */
    CHAR       cInitName[MAX_INI_NAME_LEN];  /* Initiator Name */
}_PACKED_1_ T_IscsiSessInfo;

/*Target会话信息列表*/
typedef struct
{
    WORD32      udwTgtId;                            /*Target Id*/
    WORD32      udwSessCount;                      /*Session 数量*/
    T_IscsiSessInfo tIscsiSessInfo[MAX_ISCSI_SESSION_COUNT];        /*Session 信息列表*/
}_PACKED_1_ T_IscsiSessInfoList;

/*获取session 属性*/
typedef struct
{
    WORD64      uqwSessId;                /* Session Id, 入参*/
    WORD32      udwTgtId;               /*Target Id */
    WORD32      udwPortId;              /*port  Id */
    WORD32      udwSessionParam[session_key_last];    /*session 属性*/
}_PACKED_1_ T_GetIscsiSessAttrInfo;

typedef struct
{
    WORD64      uqwSessId;                /* Session Id, 入参*/
    WORD32      udwTgtId;               /*Target Id */
    WORD32      udwPortId;              /* port  Id */
    WORD32      udwParamType;      /*属性类型标识，表示是Session属性还是还是Target属性，该处仅表示Session属性 */
    WORD32      udwPartial;             /*设置Session属性标识，为0表示设置所有的参数属性，大于0时表示设置部分参数属性，每个bit分别对应一个参数属性*/
    WORD32      udwSessionParam[session_key_last];
    WORD32      udwTargetParam[target_key_last];
}_PACKED_1_ T_IscsiParamInfo;


/*端口流量信息*/
typedef struct
{
    SWORD32      sdwCtrlId;               /* 控制器Id ,暂时定义为0、1，对应的宏定义CTRL0、CTRL1*/
    WORD32       udwTgtId;                /* Target ID*/
    WORD32       udwPortId;              /*Target  port ID , 值为0表示所有端口，其他值为端口ID */
//    WORD32       udwBandWidth;      /*数据流带宽*/
    WORD64       uqwTxPDUCount;    /*发送完整(成功)PDU个数*/
    WORD64       uqwRxPDUCount;    /*接收完整(成功)PDU个数*/
    WORD64       uqwTxPktCount;     /*发送不完整(失败)PDU个数*/
    WORD64       uqwRxPktCount;     /*接收不完整(失败)PDU个数*/
    WORD64       uqwTxByteCount;   /*发送总字节数*/
    WORD64       uqwRxByteCount;   /*接收总字节数*/
    WORD64       uqwBandWidth;      /*数据流带宽,单位:Byte/s */
}_PACKED_1_ T_IscsiFlowStatInfo;

/*Target Name*/
typedef struct
{
    SWORD32       udwCtrlId;  /* CTRL ID*/
    CHAR         cTgtName[MAX_TGT_NAME_LEN];   /*Target Name*/
}_PACKED_1_ T_IscsiTargetInfo;


/*Iscsi 流量统计开关*/
typedef struct
{
    SWORD32     sdwCtrlId;    /* 控制器Id ,暂时定义为0、1，对应的宏定义CTRL0、CTRL1*/
    WORD32      udwTid;       /* Target ID*/
    WORD32      udwAction;    /* 操作标志: 0/1/2，对应的宏定义为"OFF"、 "ON"、"RES"，分别表示关闭、打开、重置。 */
}_PACKED_1_ T_IscsiFlowStatFlag;


/* Target 对应的端口信息列表 */
typedef struct
{
    WORD32       udwPortId;                     /* Port Id */
    CHAR         cPortIp[MAX_IP_LEN];  /* 端口的物理IP */
    WORD16       sServPort;                     /* 服务端口 */
}_PACKED_1_ T_IscsiPortalInfo;
typedef struct
{
   WORD32            udwTgtId;                                                                               /*Target Id*/
   WORD32            udwPortCount;                                                                        /* 端口数量 */
   T_IscsiPortalInfo tISCSIPortalInfo[MAX_ISCSI_PORT_NUM_PER_CTL];             /* 端口信息列表*/
}_PACKED_1_ T_IscsiPortalList;



#if 0
typedef struct
{
    WORD32 sdwPortId;  /* 网口号 */
    BYTE   ucPortIp[MAX_IP_LEN];/* 磁阵iSCSI网口IP地址 */
}_PACKED_1_ T_iSCSIPortInfo;

typedef struct
{
    WORD32 sdwTgtId;/* tareget ID */
    BYTE   ucTgtName;
    WORD32 sdwPortNum;
    T_iSCSIPortInfo tPortInfo[MAX_PORT_NUM];/* iSCSI目标器信息 */
}_PACKED_1_ T_iSCSITgtInfo;

typedef struct
{
    WORD64 sqwSid;/* iSCSI会话id */
    BYTE   ucTgtIp[MAX_IP_LEN];/* 磁阵iSCSI网口IP地址 */
}_PACKED_1_ T_iSCSIPerTgtInfo;

typedef struct
{
    WORD32 sdwTgtId;/* target ID */
    T_iSCSIPerTgtInfo iSCSIPerTgtInfo[MAX_PORT_NUM];/* iSCSI目标器信息 */
}_PACKED_1_ T_iSCSITgtInfoList;

typedef struct
{
    BYTE InitIp[MAX_IP_LEN];/* iSCSI启动器的IP地址 */
    BYTE InitName[MAX_INI_NAME_LEN];/* iSCSI启动器名字 */
}_PACKED_1_ T_iSCSIPerInitInfo;

typedef struct
{
    WORD32 Tid;/* target ID */
    T_iSCSIPerInitInfo iSCSIPerInitInfo[INICOUNT];/* iSCSI启动器信息 */
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
    T_SessionParam SessionParam[session_key_last];/* iSCSI会话属性 */
}_PACKED_1_ T_iSCSIParamInfo;

typedef struct
{
    BYTE ucName[CHAP_ACCOUNT_NAME_MAX];/* 用户名 */
    BYTE PssWd[CHAP_ACCOUNT_PASS_MAX];/* 密码 */
}_PACKED_1_ T_iSCSIChapAcct;

typedef struct
{
    WORD32 Tid;/* target ID */
    WORD32 PortId;/* 网口号 */
    WORD64 TxFrames;/* 发送的iSCSI PDU包个数 */
    WORD64 RxFrames;/* 接收的iSCSI PDU包个数 */
    WORD64 TxWords; /* 发送字节数 */
    WORD64 RxWords;/* 接收字节数 */
}_PACKED_1_ T_iSCSIFlowInfo;

typedef struct
{
    WORD32 Tid;/* target ID */
    WORD64 TxFrames;/* 发送的iSCSI PDU包个数 */
    WORD64 RxFrames;/* 接收的iSCSI PDU包个数 */
    WORD64 TxWords; /* 发送字节数 */
    WORD64 RxWords;/* 接收字节数 */
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
    SWORD32  sdwType;            /* USP_VER_TYPE/EXP_VER_TYPE/VPD_VER_TYPE/POW_VER_TYPE/BOOT_VER_TYPE 该字段目前只针对主柜即EncId=0有效 */
    SWORD32  sdwRebootMode;      /* 2--OFFLINE REBOOT, 1--ONLINE REBOOT, 出参 */
    CHAR     ucFileName[MAX_FW_FILE_NAME_LEN];
}_PACKED_1_ T_UpdateFirmware;

typedef struct
{
    SWORD32  sdwEnclosureId; /* enclosure identification,入参 */
    SWORD32  sdwCtlId;       /* ctl id or expander id，  入参 */
    SWORD32  sdwType;        /* USP_VER_TYPE/EXP_VER_TYPE/VPD_VER_TYPE/POW_VER_TYPE/BOOT_VER_TYPE 该字段目前只针对主柜即EncId=0有效 */
    CHAR     ucCurrentFileName[MAX_FW_FILE_NAME_LEN]; /* 当前版本名称 */
    CHAR     ucCurrentVersion[MAX_VERSION_LEN];       /* 当前版本号 */
    CHAR     ucStandbyFileName[MAX_FW_FILE_NAME_LEN]; /* 备用版本名称 */
    CHAR     ucStandbyVersion[MAX_VERSION_LEN];       /* 备用版本号 */
    CHAR     ucUpdateFileName[MAX_FW_FILE_NAME_LEN];  /* 升级版本名称 */
    CHAR     ucUpdateVersion[MAX_VERSION_LEN];        /* 升级版本号 */
}_PACKED_1_ T_FirmwareVersion;

typedef struct
{
    SWORD32  sdwEnclosureId;   /* 扩展柜箱体ID,  现默认为0 */
    SWORD32  sdwCtlId;              /* ctl id or expander id or ENC_ALL_CTL */
    SWORD32  sdwAction;           /* 在线(CTL_ONLINE_REBOOT)/非在(CTL_SINGLE_REBOOT)线重启标记CTL_SINGLE_REBOOT/CTL_ONLINE_REBOOT */
}_PACKED_1_ T_CtrlAction;

typedef struct
{
    SWORD32  sdwNum;
    SWORD32  sdwEnclosureId[MAX_ENC_NUM_IN_SYS]; /* 箱体Id，取值 0~实际箱体数*/
}_PACKED_1_ T_AllEncId;

typedef struct
{
    /* 待确定; */
    SWORD32 sdwCtlId;  /* 临时使用，为了配合Windows下的编译  */
}_PACKED_1_ T_CtlHwInfo;

typedef struct
{
    SWORD32 sdwFileType;                          /* eConfigFileType, 保存文件类型 */
    CHAR    ucFileName[MAX_CONFIG_FILE_NAME_LEN]; /* 仅文件名，不包括路径 */
}_PACKED_1_ T_ConfigFile;

/* *************************System config**************************** */

/* 网络信息 */
typedef struct
{
    WORD32 udwCtrlId;          /* 控制器ID */
    WORD32 udwRoleType;     /* 网络设备角色(作用类型)类型，即业务网口(NET_ROLE_BUSINESS)、管理网口 (NET_ROLE_MANAGEMENT)*/
    WORD32 udwDeviceId;         /* 设备编号 */
    CHAR   cIpAddr[MAX_IP_LEN]; /* IP地址 */
    CHAR   cNetmask[MAX_IP_LEN]; /* 子网掩码 */
    CHAR   cGateway[MAX_IP_LEN]; /* 网关 */
}_PACKED_1_ T_SystemNetCfg;

typedef struct
{
    SWORD32  sdwDeviceNum;                                                     /* 网络设备数量 */
    T_SystemNetCfg tSystemNetCfg[DUAL_MAX_NET_DEVICE_NUM];         /* 网络设备信息列表(双控)*/
}_PACKED_1_ T_SystemNetCfgList;


/* 端口MAC */
typedef struct{
    WORD32 udwCtrlId;                         /* 控制器ID */
    WORD32 dwPortNo;                          /*端口号，在SPR10中，这个编号为1~4*/
    BYTE   ucMacData[6];                       /*MAC 地址*/
}_PACKED_1_  T_IscsiPortMac;

typedef struct
{
    SWORD32  sdwIscsiPortNum;                               /* Iscsi  端口数量 */
    T_IscsiPortMac tIscsiPortMac[SPR10_DUAL_ISCSI_PORT_NUM];         /* Iscsi  端口Mac 信息列表*/
}_PACKED_1_ T_IscsiPortMacList;


/* 端口信息  */
typedef struct
{
    WORD32 udwCtrlId;                        /* 控制器ID ，入参*/
    WORD32 dwPortNo;                         /* 端口号，在SPR10中，这个编号为1~4，入参*/
    SWORD32 sdwSetFlg;                       /* 设置标志，暂取值0、1、2，分别表示设置MAC、设置MTU、设置MAC和MTU ,(该入参AMS 赋值)*/
    WORD32 udwPortMtu;                       /* 端口MTU ，可选入参*/
    BYTE   ucMacData[6];                     /* MAC 地址，可选入参*/
}_PACKED_1_  T_IscsiPortSet;


typedef struct
{
    WORD32 udwCtrlId;                         /* 控制器ID */
    WORD32 dwPortNo;                          /*端口号，在SPR10中，这个编号为1~4*/
    WORD32 udwPortState;                    /*端口状态，取值0(down)/1(up)*/
    WORD32 udwPortMtu;                       /*端口MTU */
    BYTE   ucMacData[6];                       /*MAC 地址*/
}_PACKED_1_  T_IscsiPortInfo;

typedef struct
{
    SWORD32  sdwIscsiPortNum;                               /* Iscsi  端口数量 */
    T_IscsiPortInfo tIscsiPortMac[SPR10_DUAL_ISCSI_PORT_NUM];         /* Iscsi  端口Mac 信息列表*/
}_PACKED_1_ T_IscsiPortInfoList;

/* 端口静态路由信息 */
typedef struct
{
    WORD32 dwPortId;                          /* 端口号，在SPR10中，这个编号为1~4*/
    CHAR   scDstIp[MAX_IP_LEN];               /* 目标IP地址 */
    CHAR   scMask[MAX_IP_LEN];                /* 掩码 */
    CHAR   scNextHop[MAX_IP_LEN];             /* 下一跳地址 */
}_PACKED_1_  T_IscsiPortRouteInfo;

typedef struct
{
    WORD32 udwCtrlId;                         /* 控制器ID */
    T_IscsiPortRouteInfo tIscsiPortRouteInfo; /* 路由信息*/
}_PACKED_1_  T_IscsiPortRoute;

typedef struct
{
    WORD32 udwCtrlId;                         /* 控制器ID */
    CHAR   scDstIp[MAX_IP_LEN];               /* IP地址 */
    CHAR   scMask[MAX_IP_LEN];                /* 掩码 */
}_PACKED_1_  T_IscsiPortRouteDel;


typedef struct
{
    WORD32 udwCtrlId;                         /* 控制器ID */
    SWORD32 sdwRouteNum;                      /* 静态路由数目 */
    T_IscsiPortRouteInfo tIscsiPortRouteInfo[SCSI_MAX_STATICROUTE_NUM];    /* 静态路由列表*/
}_PACKED_1_  T_IscsiPortRouteList;

typedef struct
{
    CHAR   scDstIp[MAX_IP_LEN];               /* 目标IP地址 ,入参*/ 
    WORD32 udwBytes;                          /* 数据长度 单位：字节,出参*/
    WORD32 udwTotTime;                        /* 往返时间 单位：us,出参*/
    WORD32 udwTtl;                            /* ttl ,出参*/                  
}_PACKED_1_  T_IscsiPingInfo;

/* IQN */
typedef struct
{
    SWORD32 sdwCtrlId;
    CHAR    acNewIqnname[MAX_TGT_NAME_LEN]; //iqn名称
}T_SetIscsiName;

/* 版本目录下剩余空间大小, 出参 */
typedef struct
{
    WORD32  dwFreeSize;
}_PACKED_1_ T_VerDirFreeSize;

typedef struct
{
    CHAR    cFileName[FILE_NAME_LEN];        /* 待上传文件的文件名, 入参*/
    WORD32  dwLocalFreeSize;                /* 本端剩余空间大小 */
    WORD32  dwPeerFreeSize;                 /* 对端剩余空间大小 */
}_PACKED_1_ T_VerFreeSpace;

typedef struct
{
    CHAR     cFileName[FILE_NAME_LEN];        /* 待上传文件的文件名, 入参*/
    BYTE     ucFileType;                      /* USP_VER_TYPE/EXP_VER_TYPE/VPD_VER_TYPE/POW_VER_TYPE/BOOT_VER_TYPE 该字段目前只针对主柜即EncId=0有效 */
    SWORD32  sdwIsValid;                      /* 校验结果，0---校验通过，其他--校验不通过，返回错误码, 出参 */
}_PACKED_1_ T_VerFileCheck;

typedef struct
{
    CHAR  cFileName[FILE_NAME_LEN];        /* 待上传文件的文件名, 入参*/
    BYTE  ucUploadType;                    /* VERSION_FILE_TPYE--版本文件 CONFIG_FILE_TPYE--配置文件 OTHER_FILE_TYPE--其他 */
}_PACKED_1_ T_UploadPreInfo;

/* ftp上传文件涉及的结构体 */
typedef struct
{
    CHAR  cUserName[USR_NAME_LEN];         /* ftp服务器登录用户名 ，入参*/
    CHAR  cPassWord[PASSWD_LEN];           /* ftp服务器登录密码 ，入参*/
    CHAR  cFtpServerIPAddr[MAX_IP_LEN];    /* ftp服务器IP 地址，入参*/
    CHAR  cFilePath[FILE_PATH_LEN];        /* 待上传文件的文件名 (带相对于FTP服务器目录的绝对路径)，入参*/
    BYTE  ucUploadType;                    /* VERSION_FILE_TPYE--版本文件 CONFIG_FILE_TPYE--配置文件 LOG_FILE_TPYE--日志文件 OTHER_FILE_TYPE--其他 */
}_PACKED_1_ T_UploadFileInfo;

typedef struct
{
    SWORD32 sdwFileType;                 /* VERSION_FILE_TPYE--版本文件 CONFIG_FILE_TPYE--配置文件 LOG_FILE_TPYE--日志文件 OTHER_FILE_TYPE 其他文件*/
    CHAR    cFileName[FILE_NAME_LEN];
}_PACKED_1_ T_FileToPeer;

/* 目前ftp服务器在AMS端，SCS端是ftp客户端。 上传下载是针对SCS来说 */
/* ftp下载文件涉及的结构体 */
typedef struct
{
    CHAR  cUserName[USR_NAME_LEN];         /* ftp服务器登录用户名 ，入参*/
    CHAR  cPassWord[PASSWD_LEN];           /* ftp服务器登录密码 ，入参*/
    CHAR  cFtpServerIPAddr[MAX_IP_LEN];    /* ftp服务器IP 地址，入参*/
    CHAR  cFilePath[FILE_PATH_LEN];        /* 文件存放的目的路径（不带文件名），将自动以srcFileName文件名保存，入参 */
    CHAR  cSrcFileName[FILE_NAME_LEN];     /* 要下载的文件文件名（不带路径），入参 */
    BYTE  ucDownloadType;                  /* VERSION_FILE_TPYE--版本文件 CONFIG_FILE_TPYE--配置文件 LOG_FILE_TPYE--日志文件 OTHER_FILE_TYPE--其他 */
}_PACKED_1_ T_DownloadFileInfo;

typedef struct
{
    WORD32  udwPeerProcess;                 /* 对端进度，出参 */
    WORD32  udwLocalProcess;                /* 本端进度，出参 */
    WORD32  udwFileSize;                    /* 文件大小，出参 */
    CHAR    cFileName[FILE_PATH_LEN];       /* 文件名(带路径) ，入参*/
}_PACKED_1_ T_UploadFileProgress;

typedef struct
{
    WORD32  udwDownloadProcess;             /* 下载进度 */
    CHAR    cFileName[FILE_NAME_LEN];       /* 文件名(不带路径) */
}_PACKED_1_ T_DownloadFileProgress;

typedef struct
{
    CHAR       cFileName[FILE_NAME_LEN];       /* 文件名(不带路径) ，入参*/
}_PACKED_1_ T_UploadFileName;

typedef struct
{
    CHAR       cFileName[FILE_NAME_LEN];       /* 文件名(不带路径) ，入参*/
    BYTE       ucExistType;                    /* 1:两端都存在,2:双控下仅本端存在,3:双控下仅对端存在,
                                                * 4:双控下两端都不存在,5:单控下存在,6:单控下不存在,
                                                * 7:单双控情况下文件名与当前主用或备用版本重名---出参 */
}_PACKED_1_ T_IsFileExist;

/* *************************DIAG**************************** */
/*单板诊断信息*/

typedef struct
{
    SWORD32  sdwDiagDeviceId;                                /* 诊断芯片ID */
    CHAR     cDiagDeviceName[MAX_DIAG_DEVICE_NAME_LEN];      /* 诊断对象名称 */
    SWORD32  sdwDiagPreStatus;                               /* 诊断上一次结果，正常为DEVICE_STATUS_OK，异常为DEVICE_STATUS_FALSE */
    SWORD32  sdwDiagCurStatus;                               /* 诊断本次结果， 正常为DEVICE_STATUS_OK，异常为DEVICE_STATUS_FALSE */
}_PACKED_1_ T_DiagOneDeviceInfo;

typedef struct
{
    SWORD32         sdwCtrlId;                                       /* 控制器ID,暂时定义为0、1，对应的宏定义CTRL0、CTRL1，出参*/
    SWORD32         sdwDeviceNum;                                    /* 传入参数后实际返回的设备数目 ,取值范围为0~MAX_DIAG_DEVICE_NUM，出参*/
    T_DiagOneDeviceInfo tDiagDeviceInfo[MAX_DIAG_DEVICE_NUM];        /* 诊断芯片的数组 ，数组中前sdwDeviceNum个值有效,   出参*/
}_PACKED_1_ T_DiagSingleDeviceInfo;

typedef struct
{
    SWORD32 sdwCtrlFlg;                              /* 控制器标志 ,暂时定义为0、1、2，对应的宏定义CTRL_LOCAL、CTRL_PEER、CTRL_DUAL， 入参*/
    SWORD32 sdwBoardNum;                             /* 诊断单板的数目，出参 */
    T_DiagSingleDeviceInfo tDiagSingleDeviceInfo[2]; /* 诊断信息列表，当 sdwCtrlFlg = 0/1 时,仅tDiagSingleDeviceInfoList[0]有值；当 sdwCtrlFlg = 2 时 ,tDiagSingleDeviceInfoList都有值，出参*/
}_PACKED_1_ T_DiagDeviceInfo;

/*BBU诊断信息*/
typedef struct
{
    SWORD32 sdwCtrlId;                              /* 控制器ID,暂时定义为0、1，对应的宏定义CTRL0、CTRL1*/
    SWORD32 sdwBbuPresentInfo;                      /* BBU是否在位 ，在位为DIAG_BBU_PRESENT，不在位为DIAG_BBU_NOPRESENT */
    SWORD32 sdwBbuwMv;                              /* BBU当前电压，单位：mV  */
    SWORD32 sdwBbuwCapacity;                        /* BBU剩余电量百分比 */
    SWORD32 sdwBbuDeviceStatus;                     /* BBU状态,正常为DEVICE_STATUS_OK，异常为DEVICE_STATUS_FALSE*/
}_PACKED_1_ T_DiagSingleBatteryInfo;
typedef struct
{
    SWORD32 sdwCtrlFlg;                                    /* 控制器标志 ,暂时定义为0、1、2，对应的宏定义CTRL_LOCAL、CTRL_PEER、CTRL_DUAL,   入参*/
    SWORD32 sdwBbuNum;                                     /* 诊断BBU电源的数目，出参 */
    T_DiagSingleBatteryInfo tDiagSingleBatteryInfo[2];     /* 诊断信息列表，当 sdwCtrlFlg = 0/1 时,仅tDiagSingleBatteryInfo[0]有值,当 sdwCtrlFlg = 2 时 ,tDiagSingleBatteryInfo都有值*/
}_PACKED_1_ T_DiagBatteryInfo;


/* EPLD诊断项*/
typedef struct
{
    SWORD32 sdwCtrlId;                                     /* 控制器ID,暂时定义为0、1，对应的宏定义CTRL0、CTRL1*/
    SWORD32 sdwEpldFirmVer;                                /* EPLD固件版本号*/
    CHAR    cEpldLocation[MAX_DIAG_DEVICE_NAME_LEN];       /* EPLD的位置       */
    SWORD32 sdwEpldDeviceStatus;                           /* EPLD设备状态,正常为DEVICE_STATUS_OK，异常为DEVICE_STATUS_FALSE*/
}_PACKED_1_ T_DiagSingleEpldInfo;

typedef struct
{
    SWORD32 sdwCtrlFlg;                                    /* 控制器标志 ,暂时定义为0、1、2，对应的宏定义CTRL_LOCAL、CTRL_PEER、CTRL_DUAL,   入参*/
    SWORD32 sdwEpldNum;                                    /* 诊断Epld的数目，出参 */
    T_DiagSingleEpldInfo tDiagSingleEpldInfo[2];           /* 诊断信息列表，当 sdwCtrlFlg = 0/1 时,仅tDiagSingleEpldInfo[0]有值,当 sdwCtrlFlg = 2 时 ,tDiagSingleEpldInfo都有值*/
}_PACKED_1_ T_DiagEpldInfo;


/* EEPROM诊断项  */
typedef struct
{
    SWORD32    sdwCtrlId;                                 /* 控制器ID,暂时定义为0、1，对应的宏定义CTRL0、CTRL1*/
    SWORD32    sdwEepromDeviceStatus;                     /* Eeprom设备状态,正常为DEVICE_STATUS_OK，异常为DEVICE_STATUS_FALSE*/
}_PACKED_1_ T_DiagSingleEepromInfo;

typedef struct
{
    SWORD32 sdwCtrlFlg;                                   /* 控制器标志 ,暂时定义为0、1、2，对应的宏定义CTRL_LOCAL、CTRL_PEER、CTRL_DUAL,   入参*/
    SWORD32 sdwEepromNum;                                 /* 诊断Eeprom的数目，出参 */
    T_DiagSingleEepromInfo tDiagSingleEepromInfo[2];      /* 诊断信息列表，当 sdwCtrlFlg = 0/1 时,仅tDiagSingleEepromInfo[0]有值,当 sdwCtrlFlg = 2 时 ,tDiagSingleEepromInfo都有值*/
}_PACKED_1_ T_DiagEepromInfo;


/* CRYSTAL诊断项 */
typedef struct
{
    SWORD32  sdwCtrlId;                                   /* 控制器ID,暂时定义为0、1，对应的宏定义CTRL0、CTRL1 */
    SWORD32  sdwCrystalTotalNum;                          /* 晶振总个数 */
    SWORD32  sdwCrystalDeviceStatus;                      /* Crystal设备状态,正常为DEVICE_STATUS_OK，异常为DEVICE_STATUS_FALSE */
}_PACKED_1_  T_DiagSingleCrystalInfo;

typedef struct
{
    SWORD32 sdwCtrlFlg;                                   /* 控制器标志 ,暂时定义为0、1、2，对应的宏定义CTRL_LOCAL、CTRL_PEER、CTRL_DUAL,   入参*/
    SWORD32 sdwCrystalNum;                                /* 诊断晶振的数目，出参 */
    T_DiagSingleCrystalInfo tDiagSingleCrystalInfo[2];    /* 诊断信息列表，当 sdwCtrlFlg = 0/1 时,仅tDiagSingleCrystalInfo[0]有值,当 sdwCtrlFlg = 2 时 ,tDiagSingleCrystalInfo都有值*/
}_PACKED_1_ T_DiagCrystalInfo;


/* RTC诊断项 */
typedef struct
{
    SWORD32     sdwCtrlId;                               /* 控制器ID,暂时定义为0、1，对应的宏定义CTRL0、CTRL1*/
    SWORD32     sdwRtcDeviceStatus;                      /* Rtc设备状态,正常为DEVICE_STATUS_OK，异常为DEVICE_STATUS_FALSE */
}_PACKED_1_  T_DiagSingleRtcInfo;

typedef struct
{
    SWORD32 sdwCtrlFlg;                                  /* 控制器标志 ,暂时定义为0、1、2，对应的宏定义CTRL_LOCAL、CTRL_PEER、CTRL_DUAL,   入参*/
    SWORD32 sdwRtcNum;                                   /* 诊断Rtc的数目，出参 */
    T_DiagSingleRtcInfo tDiagSingleRtcInfo[2];           /* 诊断信息列表，当 sdwCtrlFlg = 0/1 时,仅tDiagSingleRtcInfo[0]有值,当 sdwCtrlFlg = 2 时 ,tDiagSingleRtcInfo都有值*/
}_PACKED_1_ T_DiagRtcInfo;


/* 锁相环诊断项 */
typedef struct
{
    SWORD32     sdwCtrlId;                               /* 控制器ID,暂时定义为0、1，对应的宏定义CTRL0、CTRL1*/
    SWORD32     sdwClockDeviceStatus;                    /* 锁相环设备状态,正常为DEVICE_STATUS_OK，异常为DEVICE_STATUS_FALSE*/
}_PACKED_1_  T_DiagSingleClockInfo;

typedef struct
{
    SWORD32 sdwCtrlFlg;                                 /* 控制器标志 ,暂时定义为0、1、2，对应的宏定义CTRL_LOCAL、CTRL_PEER、CTRL_DUAL,   入参*/
    SWORD32 sdwClockNum;                                /* 诊断锁相环的数目，出参 */
    T_DiagSingleClockInfo tDiagSingleClockInfo[2];      /* 诊断信息列表，当 sdwCtrlFlg = 0/1 时,仅tDiagSingleClockInfo[0]有值,当 sdwCtrlFlg = 2 时 ,tDiagSingleClockInfo都有值*/
}_PACKED_1_ T_DiagClockInfo;

/* PM8001 诊断项 */
typedef struct
{
    SWORD32     sdwCtrlId;                              /* 控制器ID,暂时定义为0、1，对应的宏定义CTRL0、CTRL1*/
    SWORD32     sdwPmDeviceStatus;                      /* PM8001 设备状态,正常为DEVICE_STATUS_OK，异常为DEVICE_STATUS_FALSE*/
}_PACKED_1_  T_DiagSinglePmInfo;

typedef struct
{
    SWORD32 sdwCtrlFlg;                                 /* 控制器标志 ,暂时定义为0、1、2，对应的宏定义CTRL_LOCAL、CTRL_PEER、CTRL_DUAL,   入参*/
    SWORD32 sdwPmNum;                                   /* 诊断 PM8001 的数目，出参 */
    T_DiagSinglePmInfo tDiagSinglePmInfo[2];            /* 诊断信息列表，当 sdwCtrlFlg = 0/1 时,仅tDiagSinglePmInfo[0]有值,当 sdwCtrlFlg = 2 时 ,tDiagSinglePmInfo都有值*/
}_PACKED_1_ T_DiagPmInfo;

/* PCIE 诊断项 */
typedef struct
{
    SWORD32     sdwCtrlId;                             /* 控制器ID,暂时定义为0、1，对应的宏定义CTRL0、CTRL1*/
    SWORD32     sdwLinkSpeed;                          /* PCIE 连接速率*/
    SWORD32     sdwLinkWidth;                          /* PCIE 连接带宽x1~x4 x0表示无link */
    SWORD32     sdwBadTlpCount;                        /* TLP层坏包个数 */
    SWORD32     sdwBadDllpCount;                       /* DLLP层坏包个数 */
    SWORD32     sdwEepromStatus;                       /* PCIE 设备E2prom在位与否 有效与否 */
    SWORD32     sdwDllpStatus;                         /* DLLP 状态 */
    SWORD32     sdwPcieDeviceStatus;                   /* PCIE 设备状态,正常为DEVICE_STATUS_OK，异常为DEVICE_STATUS_FALSE*/
}_PACKED_1_  T_DiagSinglePcieInfo;

typedef struct
{
    SWORD32 sdwCtrlFlg;                                /* 控制器标志 ,暂时定义为0、1、2，对应的宏定义CTRL_LOCAL、CTRL_PEER、CTRL_DUAL,   入参*/
    SWORD32 sdwPcieNum;                                /* 诊断PCIE 的数目，出参 */
    T_DiagSinglePcieInfo tDiagSinglePcieInfo[2];       /* 诊断信息列表，当 sdwCtrlFlg = 0/1 时,仅tDiagSinglePcieInfo[0]有值,当 sdwCtrlFlg = 2 时 ,tDiagSinglePcieInfo都有值*/
}_PACKED_1_ T_DiagPcieInfo;

typedef struct
{
    CHAR    cDiagDeviceName[MAX_DIAG_DEVICE_NAME_LEN];    /* 诊断对象名称，   出参  */
    CHAR    cDiagProperty[MAX_PROP_NAME_LEN];             /* 诊断对象属性，   出参  */
    CHAR    cDiagPropertyValue[MAX_PROP_VALUE_LEN];       /* 诊断对象属性值， 出参  */
}_PACKED_1_ T_DiagDeviceProperty;

/* 通用诊断对象结构体 */
typedef struct
{
    SWORD32                 sdwDiagRetNum;                                /* 诊断对象返回的记录数量， 出参 */
    T_DiagDeviceProperty    tDiagDeviceProperty[MAX_PROP_REC_NUM];        /* 所有的查询记录 */
}_PACKED_1_ T_DiagSingleDeviceRecord;

typedef struct
{
    SWORD32 sdwCtrlFlg;                                   /* 控制器标志 ,暂时定义为0、1、2，对应的宏定义CTRL_LOCAL、CTRL_PEER、CTRL_DUAL,  入参  */
    SWORD32 sdwDiagDeviceId;                              /* 诊断对象ID,  入参  */
    SWORD32 sdwDeviceNum;                                 /* 诊断对象的数目,  出参  */
    T_DiagSingleDeviceRecord tDiagSingleDeviceRecord[2];  /* 诊断信息列表,当 sdwCtrlFlg = 0/1 时,仅tDiagSingleDeviceInfo[0]有值,当 sdwCtrlFlg = 2 时 ,tDiagSingleDeviceInfo都有值*/
}_PACKED_1_ T_DiagDeviceRecord;



/* *************************EM**************************** */
typedef struct
{
    WORD32   udwEncId;                                   /* 扩展柜Id */
    WORD64   uqwExpSasAddr;                              /* Expander SAS 地址 */
    WORD32   udwDiskNum;                                 /* Expander 连接的磁盘数目*/
    WORD32   udwSlotId[SCANBOX_MAX_DISK_NUM];            /* Expander 连接的磁盘槽位号列表*/
} _PACKED_1_ T_ExpanderInfo;

typedef struct
{
    WORD32   udwExpNum;                                  /* Expander 数目*/
    T_ExpanderInfo  tExpInfo[SCANBOX_MAX_EXP_NUM];       /* Expander 信息列表*/
} _PACKED_1_ T_ExpInfoList;

typedef struct
{
    WORD32   udwEncType;                    /* 扩展柜类型， */
} _PACKED_1_ T_EnclosureType;


/*风扇信息*/
typedef struct
{
    SWORD32 sdwLevel; /*转速级别*/
    WORD16 swSpeed; /*转速*/
}_PACKED_1_ T_EncFanInfo;

/*电源信息*/
typedef struct
{
    SWORD32 sdwBrokenFlag; /*电源故障,0、1*/
    SWORD32 sdwPresentFlag;  /*电源在位标志,0、1*/
}_PACKED_1_ T_EncPsInfo;


/*电压信息*/
typedef struct
{
    SWORD32 sdwHighCritical;  /*最高门限*/
    SWORD32 sdwHighWarning;  /*最高告警*/
    SWORD32 sdwLowWarning;  /*最低告警*/
    SWORD32 sdwLowCritical;  /*最低门限*/
    SWORD32 sdwValue;  /*实际值*/
    SWORD32 sdwWarnFlag;   /*告警标记，取值OUT_FAILURE、OUT_WARNING、UNDER_FAILURE、UNDER_WARNING*/
}_PACKED_1_ T_EncVoltInfo;

/*温度信息*/
typedef struct
{
    SWORD32 sdwHighCritical;  /*最高门限*/
    SWORD32 sdwHighWarning;  /*最高告警*/
    SWORD32 sdwLowWarning;  /*最低告警*/
    SWORD32 sdwLowCritical;  /*最低门限*/
    SWORD32 sdwValue;       /*实际值*/
    SWORD32 sdwWarnFlag;   /*告警标记，取值OUT_FAILURE、OUT_WARNING、UNDER_FAILURE、UNDER_WARNING*/
}_PACKED_1_ T_EncTempratureInfo;


/*磁盘信息*/
typedef struct
{
    WORD64  uqwSasAddr;  /*磁盘SAS地址*/
    SWORD32 sdwSlotId;  /*槽位号*/
}_PACKED_1_ T_EncDiskInfo;

/*箱体拓扑结构*/
typedef struct
{
    BYTE ucVisted;  /*访问标志*/
    BYTE ucChassisId;  /*箱体ID*/
    BYTE ucSlotId;  /*槽位ID*/
    BYTE ucDiskNum;  /*硬盘个数*/
    BYTE ucSubexpnum;  /*级联个数*/
    BYTE ucHeadflag;  /*头标志*/
    BYTE ucSubId[SUBID_LEN]; /*扩展柜ID*/
    BYTE ucSgName[MAX_DISKNAMELEN];  /*SG设备名*/

    WORD64   uqwSasAddr;  /*EXPsaS地址*/
    T_EncDiskInfo tDiskinfo[MAX_DISK_NUM];  /*磁盘信息*/
    BYTE ucSub[MAX_UP_SUB_ENCL];              /*级联箱体ID列表*/

}_PACKED_1_ T_EncTopoInfo;

/*所有箱体拓扑结构*/
typedef struct
{
    BYTE ucExpcount;  /*箱体个数*/
    T_EncTopoInfo tExpanders[MAX_UP_SUB_ENCL];  /*箱体链表头*/
}_PACKED_1_  T_EncTopoInfoList;




/* 单个磁盘的SMART信息 */
typedef struct
{
    SWORD32 sdwPdSlotId;            /* 磁盘槽位号，取值0~27(ScanBox);  0~11(SPR10) */
    SWORD32 sdwPdEnclosureId;  /* 箱体Id */
    BYTE ucAucPdName[12];   /* 磁盘设备名 如sda */
    BYTE ucIsSupportSmart;  /* 磁盘是否支持SMART */
    BYTE ucIsSmartEnable;   /* 磁盘SMART功能是否开启 */
    BYTE ucSmartStatus;     /* 磁盘的SMART状态是否OK */
    WORD32 dwSmartInfoCount;/*SMART属性个数*/
    T_DmSmartDetailInfo tSmartDetailInfo[30];   /* 磁盘SMART属性详细信息 */
    SWORD32 sdwSelfTestTime;  /* 执行自检所需时间 */
} _PACKED_1_ T_DmSmartInfo;

typedef struct
{
    //SWORD32 sdwCtrlId;        /* 控制器Id ,暂时定义为0、1，对应的宏定义CTRL0、CTRL1*/
    SWORD32 sdwAction;      /*操作标志: 0/1/2，对应的宏定义为"OFF"、 "ON"、 */
}_PACKED_1_ T_DmPowerStatSwitch;

typedef struct
{
    SWORD32 sdwPdSlotId;            /* 磁盘槽位号，取值0~27(ScanBox);  0~11(SPR10) */
    SWORD32 sdwPdEnclosureId;  /* 箱体Id */
    SWORD32 sdwTestType;        /*磁盘自检类型DM_SMART_SHORT_SELFTEST  DM_SMART_LONG_SELFTEST*/
    SWORD32 sdwTime;      /*返回自检事件 */
}_PACKED_1_ T_DmSmartTestCtl;

/*箱体设备信息*/
typedef struct
{
    BYTE ucChassisId;  /*箱体ID*/
    BYTE ucSlotId;  /*槽位ID*/
    BYTE ucFanNum;  /*风扇个数*/
    BYTE ucPsNum;  /*电源个数*/
    BYTE ucTemperatureNum;  /*温度传感器个数*/
    BYTE ucVoltNum;  /*电压传感器个数*/
    BYTE ucDiskNum;  /*硬盘个数*/

    BYTE ucSubId[SUBID_LEN]; /*扩展柜ID*/
    BYTE ucSgName[MAX_DISKNAMELEN];  /*SG设备名*/
    WORD64   uqwSasAddr;  /*EXPsaS地址*/

    T_EncFanInfo tFaninfo[MAX_FAN_NUM];  /*风扇信息*/
    T_EncPsInfo tPsinfo[MAX_PS_NUM];  /*电源信息*/
    T_EncVoltInfo tVoltinfo[MAX_VOLT_NUM];  /*电压信息*/
    T_EncTempratureInfo tTempratureinfo[MAX_TEMP_NUM];  /*温度信息*/

}_PACKED_1_ T_EncDeviceInfo;
/*所有箱体设备信息列表*/
typedef struct
{
    BYTE ucExpcount;  /*箱体个数*/
    T_EncDeviceInfo tExpanders[MAX_UP_SUB_ENCL];  /*箱体链表头*/
}_PACKED_1_  T_EncDeviceInfoList;

typedef struct
{
    BYTE ucflag;/*是否有事件发生0无事件 1有事件*/
    BYTE ucBrokenFlag;/*故障1恢复0*/

}_PACKED_1_  T_PsEvent;

typedef struct
{
    BYTE ucflag;/*是否有事件发生0无事件 1有事件*/
    BYTE ucBrokenFlag;/*故障1恢复0*/

}_PACKED_1_  T_FanEvent;

typedef struct
{
    BYTE ucflag;/*是否有事件发生0无事件 1有事件*/
    BYTE ucBrokenFlag;/*故障1恢复0*/
    BYTE ucWarnFlag; /*告警标记，取值WARNING_RECOVER OUT_FAILURE、OUT_WARNING、UNDER_FAILURE、UNDER_WARNING*/

}_PACKED_1_  T_VoltEvent;
typedef struct
{
    BYTE ucflag;/*是否有事件发生0无事件 1有事件*/
    BYTE ucBrokenFlag;/*故障1恢复0*/
    BYTE ucWarnFlag; /*告警标记，取值WARNING_RECOVER OUT_FAILURE、OUT_WARNING、UNDER_FAILURE、UNDER_WARNING*/

}_PACKED_1_  T_TempEvent;
typedef struct
{
    BYTE ucChassisId;
    BYTE ucEventCount;  /*事件个数*/
    T_PsEvent tPsevent[MAX_PS_NUM];/*电源事件*/
    T_FanEvent tFanevent[MAX_FAN_NUM];/*风扇事件*/
    T_VoltEvent tVoltevent[MAX_VOLT_NUM];/*电压事件*/
    T_TempEvent tTempevent[MAX_TEMP_NUM];/*温度事件*/
}_PACKED_1_  T_EncEvent;

typedef struct
{

    BYTE ucAllEventCount;  /*事件个数*/
    T_EncEvent  tExpanderEvent[MAX_UP_SUB_ENCL];  /*箱体链表头*/
}_PACKED_1_  T_EncEventList;

/* *************************LOG**************************** */
typedef struct
{
    T_Time  tLogTime;                                /* 日志解析后向上层传输的时间结构体 */
    BYTE    ucLogLevel;                              /* 日志级别 */
    BYTE    ucLogSrc;                                /* 日志来源 */
    SWORD16 swLogId;                                 /* 日志ID号（日志的唯一标识） */
    CHAR    ucEventData[MAX_LOG_LEN];                /* 日志解析后以文字描述的日志内容 */
    CHAR    cUserName[MAX_USER_NAME_LEN];            /* 查询日志的用户名 */
}_PACKED_1_ T_LogTableNode;

typedef struct
{
   SWORD32   sdwNum;                                 /* 日志列表中的日志数目 */
   T_LogTableNode tLog[MAX_LOG_NUM];                 /* 日志列表，以数组的形式呈现 */
}_PACKED_1_ T_LogTableList;

typedef struct
{
   SWORD32   sdwNum;                                 /* 单次查询日志时的日志列表中的日志数目 */
   T_LogTableNode tLog[MAX_LOG_NUM_ONCE];            /* 单次查询日志时的日志列表 */
}_PACKED_1_ T_LogTableOnce;

typedef struct
{
    BYTE        ucLogLevel;                          /* 日志查询时的日志级别设定（主要针对查询日志数量） */
    BYTE        ucInquireFlag;                       /* 日志查询标识，0为所有ucLogLevel以上日志，1为该级别日志（宏定义见usp_common_macro.h） */
    T_Time      tStartTime;                          /* 日志查询的开始时间 */
    T_Time      tEndTime;                            /* 日志查询的结束时间 */
    CHAR        cUserName[MAX_USER_NAME_LEN];        /* 查询的用户名称 */
}_PACKED_1_ T_LogGetNum;

typedef struct
{
   T_LogGetNum  tLogNumInfo;                         /* 日志查询时的日志级别设定 */
   WORD16       wInquireLoc;                         /* 日志查询时所要求的日志在日志链表中的位置 */
   WORD16       wInquireNum;                         /* 单次查询的数目 */
}_PACKED_1_ T_LogGet;

typedef struct
{
   CHAR  cFileName[MAX_LOG_EXPORT_NAME_LEN];        /* 日志导出时的文件名称 */
}_PACKED_1_ T_LogExport;

typedef struct
{
    BYTE    ucDelaySec;                              /* 日志告警邮件的延迟时间，用以合并日志内容 */
    BYTE    ucAlarmLevel;                            /* 日志告警邮件设置的告警日志级别 */
    BYTE    ucUserNum;                               /* 日志的告警邮件用户有效地址数目 */
    CHAR    cTo[MAX_ALARMAIL_USER][MAX_MAILTO_LEN];  /* 日志告警邮件发送的目的邮箱地址 */
}_PACKED_1_ T_LogAlarmMailCfgPack;

/*************************************MAIL Configration*********************************/
typedef struct
{
    BYTE   ucAddressFlag;                            /* 地址类型，IP地址字串或域名字串(0为IP地址，1为域名字串) */
    WORD16 wPort;                                    /* 端口号：远程邮件服务器的邮件服务端口号 */
    CHAR   cMailSendAcc[MAIL_SINGLE_ACCOUNT_LEN];    /* 发送邮件的含域名的邮箱全称 */
    CHAR   cPassword[MAIL_MAX_PASSLEN];              /* 发送邮件的密码 */
    CHAR   ucHost[MAIL_MAX_HOST_LEN];                /* 主机名字，和ucAddressFlag匹配的主机地址 */
} _PACKED_1_ T_MailConfPack;

typedef struct
{
    CHAR cTo[MAX_MAILTO_LEN];          /* 发送邮箱地址 */
} _PACKED_1_ T_VerifyMailTestTo;

/***************************************User management***********************************/

typedef struct T_SessionId
{
    SWORD32      sdwSessionId;  //用户登录的sessionid结构
}_PACKED_1_ T_SessionId;

typedef struct
{
  CHAR acUsername[MAX_USER_NAME_LEN]; //用户名(帐户)信息
  SWORD32 sdwUserType; //用户类型,参考定义eUserType
}_PACKED_1_ T_Account;
typedef struct
{
  SWORD32 scAccountNum; //当前所有帐户个数
  T_Account atAccount[MAX_USER_NUM]; //当前所有用户名(帐户)信息
}_PACKED_1_ T_AccountList;

typedef struct
{
  CHAR acUsername[MAX_USER_NAME_LEN]; //需修改名字的用户帐户
  CHAR acUserNewname[MAX_USER_NAME_LEN]; //新的用户名字
  CHAR acUsrlogname[MAX_USER_NAME_LEN]; //当前登陆用户
}_PACKED_1_ T_UserNameMody;


typedef struct
{
  CHAR acUsername[MAX_USER_NAME_LEN]; //需设置密码的用户帐户
  CHAR acOldPwd[MAX_USER_PSW_LEN]; //原始密码
  CHAR acNewPwd[MAX_USER_PSW_LEN]; //新设密码
  CHAR acUsrlogname[MAX_USER_NAME_LEN]; //当前登陆用户
}_PACKED_1_ T_PwdSet;

typedef struct
{
  CHAR acUsername[MAX_USER_NAME_LEN]; //发送密码邮件的用户名
  CHAR acPwdMail[MAX_USER_EMAIL_LEN]; //该用户设置的密码提醒Email地址
  CHAR acUsrlogname[MAX_USER_NAME_LEN]; //当前登陆用户
}_PACKED_1_ T_PwdMail;

typedef struct
{
    SWORD32 sdwUserId;//用户ID
    CHAR acUsername[MAX_USER_NAME_LEN ]; //用户名
    CHAR acPassword[MAX_USER_PSW_LEN]; //密码
    SWORD32 sdwUserType; //用户类型,参考定义eUserType
    SWORD32  sdwUserUuid;
}_PACKED_1_ T_AmsUser;

/***************************************Get System Information***********************************/
typedef  struct
{
    CHAR   cSysName[USP_SYSTEM_NAME_MAX_LEN];          /* 系统名称 , 默认值为 ZXATCA_USP*/
}_PACKED_1_ T_SysNameInfo;

typedef  struct
{
    T_SysNameInfo tSysNameInfo;                           /* 系统名称, 出参*/

    CHAR   cVendor[USP_SYSTEM_VENDOR_MAX_LEN];            /* 供应商,    默认值为 ZTE, 出参*/

    CHAR   cModel[USP_SYSTEM_MODEL_MAX_LEN];              /* 型号,      默认值为 USP, 出参*/

    CHAR   cVersionName[USP_SYSTEM_VERSION_MAX_LEN];      /* 产品版本,  默认值为 V1.0, 出参*/

    CHAR   cSerialNum[USP_SYSTEM_SERIAL_NUM_MAX_LEN];     /* 产品序列号, 默认值为1111-1111, 出参*/

    CHAR   cSysState[USP_SYSTEM_STATE_MAX_LEN];           /* 系统状态, 单控运行或双控运行两种情况: Single Running或Dual Running, 出参 */
}_PACKED_1_ T_SysInfo;

/************************************Get Detail Controller Information*******************************/
typedef  struct
{
    SWORD32   sdwCtrlId;                                       /*  控制器ID  */
    SWORD32   sdwRamCapacity;                                  /*  RAM大小, 单位是M  */
    SWORD32   sdwCtrlRole;                                     /*  控制器角色  */
    SWORD32   sdwCtrlState;                                    /*  控制器状态  */
    CHAR      cCtrlSerial[USP_CONTROLLER_SERIAL_MAX_LEN];      /*  控制器序列号, 默认为控制器UUID  */
    CHAR      cCtrlName[USP_CONTROLLER_NAME_MAX_LEN];          /*  控制器名称 默认刀片名称, 比如SPR10或SBCJ */

}_PACKED_1_ T_CtrlSingleInfo;

typedef  struct
{
    SWORD32   sdwCtrlNumb;                   /* 控制器数目,  出参*/
    SWORD32   sdwCtrlAction;                 /* CTRL_LOCAL--本端, CTRL_PEER--对端, CTRL_DUAL--两端，设置控制器名称时使用, 入参 */
    T_CtrlSingleInfo   tCtrlSingleInfo[2];   /*  控制器详细信息列表,  出参 */
}_PACKED_1_ T_CtrlInfo;

/************************************Get System Boot Done Time**************************************/
typedef  struct
{
    SWORD32   sdwCtrlId;                                    /*  控制器ID  */
    T_Time    tSysBootTime;                                 /*  系统启动完成时间  */
}_PACKED_1_ T_SysSingleBootTime;

typedef  struct
{
    SWORD32               sdwCtrlNumb;                      /* 控制器数目,  出参*/
    T_SysSingleBootTime   tSysSingleBootTime[2];            /* 系统启动完成时间列表,  出参 */
}_PACKED_1_ T_SysBootTime;
/***************************************Pd***************************************/
/**********************************************************************
* 功能描述： 获取所有磁盘列表
* 输入参数： 无
* 输出参数：全部为出参
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetAllPdId(T_AllPdId *ptAllPdId);
/**********************************************************************
* 功能描述： 获取硬盘硬属性
* 输入参数： ptPdHwInfo->sdwSLotId,pPdHwInfo->sdwEnclour
* 输出参数： 其他为出参
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
//SWORD32 GetPdHwInfo(T_PdHwInfo *ptPdHwInfo);

/**********************************************************************
* 功能描述： 获取磁盘信息
* 输入参数： ptPdHwInfo->sdwSLotId,pPdHwInfo->sdwEnclour
* 输出参数： 其他为出参
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetHddInfo(T_HddInfo *ptHddInfo);

/**********************************************************************
* 功能描述： 获取所有磁盘信息
* 输入参数： 无
* 输出参数： 均为出参
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetAllHddInfo(T_AllHddInfo *ptAllHddInfo);

/**********************************************************************
* 功能描述： 设置硬盘硬属性（传输速率、节能模式、cache开关）
* 输入参数： pPdHwInfo->sdwSlotId,pPdHwInfosdwEnclouserId
* 输出参数：其他为出参
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 SetPdHwInfo(T_PdSetHwInfo *pPdHwInfo);
/**********************************************************************
* 功能描述： 获取硬盘软属性
* 输入参数： ptPdSwInfo->sdwSlotId,ptPdSwInfo->sdwEnclouserId
* 输出参数：其他为出参
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
//SWORD32 GetPdSwInfo(T_PdSwInfo *ptPdSwInfo);

/**********************************************************************
* 功能描述： 空闲盘变成热备盘
* 输入参数：全部为入参
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/

SWORD32 SetPdUnusedToHotSpare(T_PdId *ptPdId);
/**********************************************************************
* 功能描述： 热备盘变成空闲盘
* 输入参数： 全部为入参
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 SetPdHotSpareToUnsed(T_PdId *ptPdId);

/**********************************************************************
* 功能描述： 未知盘和冲突盘转换为空闲盘
* 输入参数： 全部为入参
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 SetPdToUnsed(T_PdId *ptPdId);

/**********************************************************************
* 功能描述： 外来磁盘导入
* 输入参数： 全部为入参
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32  ImportPd(T_PdId *ptPdId);

/**********************************************************************
* 功能描述： 设置磁盘坏块告警数目
* 输入参数： 无
* 输出参数： BadBlockWarnNum
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetBadBlockWarnNum(T_PdBadBlockWarnNum *ptPdBadBlockWarnNum);

/**********************************************************************
* 功能描述： 查询磁盘坏块告警
* 输入参数： BadBlockWarnNum
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 SetBadBlockWarnNum(T_PdBadBlockWarnNum *ptPdBadBlockWarnNum);


/**********************************************************************
* 功能描述：获取磁盘的SMART状态信息（针对单个硬盘）
* 输入参数：EnclosureId  ,  SlotId
* 输出参数：其他为出参
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetPdSmartStateInfo(T_PdSmartStateInfo *ptPdSmartStateInfo);

/**********************************************************************
* 功能描述： 查询Smart信息
* 输入参数：EnclosureId  ,  SlotId
* 输出参数：其他为出参
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetSmartInfo(T_PdSmartInfo *ptSmartInfo);

/**********************************************************************
* 功能描述： 使能/ 禁止硬盘Smart告警开关
* 输入参数： ptPdWarnSwitch
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 SetPdWarnSwitchStatus(T_PdWarnSwitch *ptPdWarnSwitch);

/**********************************************************************
* 功能描述： 获取磁盘告警开关
* 输入参数： EnclosureId  ,  SlotId
* 输出参数： sdwSwitch
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetPdWarnSwitchStatus(T_PdWarnSwitch *ptPdWarnSwitch);

#if 0
/**********************************************************************
* 功能描述： 开始扫描（针对单个硬盘）
* 输入参数： EnclosureId  ,  SlotId
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 StartPdScan(T_PdId *ptPdId);

/**********************************************************************
* 功能描述： 停止扫描
* 输入参数： EnclosureId  ,  SlotId
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 StopPdScan(T_PdId *ptPdId);
#endif
/**********************************************************************
* 功能描述： 查询扫描任务进度
* 输入参数： EnclosureId  ,  SlotId
* 输出参数：  其他为出参
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetPdScanProgress(T_PdScanStatus *ptPdScanStatus);

/**********************************************************************
* 功能描述： 磁盘扫描
* 输入参数： sdwPdSlotId、sdwPdEnclosureId、sdwCtrlFlg
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 ScanPd(T_PdScanCtrl *ptPdScanCtrl);

/**********************************************************************
* 功能描述： 控制硬盘灯闪烁
* 输入参数： ptPdLed->sdwSlotId,sdwSlotId->sdwEnclouserId
* 输出参数： 其他为出参
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 CtrlPdLed(T_PdLed *ptPdLed);

/***************************************Blk***************************************/
/**********************************************************************
* 功能描述： 创建虚拟盘
* 输入参数： *ptCreateVd
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 CreateVd(T_CreateVd *ptCreateVd);
/**********************************************************************
* 功能描述： 删除虚拟盘
* 输入参数： sdwVdId
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 DelVd(T_VdName *ptVdName);
/**********************************************************************
* 功能描述： 获取虚拟盘个数及Id
* 输入参数： 无
* 输出参数： *ptAllVdId
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
//SWORD32 GetAllVdId(T_AllVdId *ptAllVdId);
SWORD32 GetAllVdNames(T_AllVdNames *ptAllVdNames);
/**********************************************************************
* 功能描述： 根据名称查询虚拟盘信息
* 输入参数： ptVdInfo->udwVdNum
* 输出参数： 其他为出参
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetVdInfo(T_VdInfo *ptVdInfo);

/**********************************************************************
* 功能描述： 获取虚拟盘上的所有VOL名称
* 输入参数： ptAllVolIdonVd->sdwVdID
* 输出参数：其他为出参
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
//SWORD32 GetVolIdOnVd(T_AllVolIdOnVd *ptAllVolIdOnVd);
SWORD32 GetVolNamesOnVd(T_AllVolNamesOnVd *ptAllVolNamesOnVd);
/**********************************************************************
* 功能描述： 获取虚拟盘上的卷ID
* 输入参数： ptAllVolIdonVd->sdwVdID
* 输出参数：其他为出参
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
//SWORD32 GetVolInfoOnVd(T_AllVolInfoOnVd *ptAllVolInfoOnVd);

/**********************************************************************
* 功能描述： 设置虚拟盘名称
* 输入参数： *ptVdName
* 输出参数：  无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 RenameVd(T_VdRename *ptVdRename);
/**********************************************************************
* 功能描述： 设置虚拟盘首选控制器
* 输入参数： *ptVdPreferVtl
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
//SWORD32 SetVdPreferCtl(T_VdPreferCtl *ptVdPreferVtl);
/**********************************************************************
* 功能描述： 设置虚拟盘自动重建开关
* 输入参数： *ptVdAuReb
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 SetVdAutoRebuild(T_VdAutoRebuild *ptVdAutoRebuild);
/**********************************************************************
* 功能描述： 虚拟盘数据一致性检查
* 输入参数： sdwVdId
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
//SWORD32 CheckVdConsistency(SWORD32 sdwVdId);
SWORD32 CheckVdConsistency(T_VdName *ptVdName);
/**********************************************************************
* 功能描述： 虚拟盘重新校验
* 输入参数： sdwVdId
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
//SWORD32 VdParityRegenerate(SWORD32 sdwVdId);
SWORD32 RegenerateParityVd(T_VdName *ptVdName);
/**********************************************************************
* 功能描述： 获取虚拟盘任务信息
* 输入参数： ptVdTaskInf->sdwVdId
* 输出参数：其他都为出参
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetVdTaskInfo(T_VdGetTaskInfo *ptVdTaskInfo);
/**********************************************************************
* 功能描述： 虚拟盘碎片整理
* 输入参数： sdwVdId
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
//SWORD32 VdDefragment(SWORD32 sdwVdId);
SWORD32 DefragmentVd(T_VdName *ptVdName);
/**********************************************************************
* 功能描述：虚拟盘手动重建
* 输入参数：
* 输出参数：
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
//SWORD32 VdRebuild(T_VdRebuild *);
SWORD32 RebuildVd(T_VdRebuild *ptVdRebuild);
/**********************************************************************
* 功能描述： 虚拟盘扩展
* 输入参数：  *ptVdExpand
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
//SWORD32 VdExpand(T_VdExpand *ptVdExpand);
SWORD32 ExpandVd(T_VdExpand *ptVdExpand);
/**********************************************************************
* 功能描述： 虚拟盘变级
* 输入参数： *ptVdRelevel
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
//SWORD32 VdRelevel(T_VdRelevel *ptVdRelevel);
SWORD32 RelevelVd(T_VdRelevel *ptVdRelevel);
/**********************************************************************
* 功能描述： 虚拟盘冻结
* 输入参数： *ptVdName
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 FreezeVd(T_VdName *ptVdName);
/**********************************************************************
* 功能描述： 虚拟盘解冻
* 输入参数： *ptVdName
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 UnFreezeVd(T_VdName *ptVdName);
/**********************************************************************
* 功能描述： 虚拟盘迁入
* 输入参数： *ptVdName
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 ImmigrateVd(T_VdName *ptVdName);
/**********************************************************************
* 功能描述： 创建卷
* 输入参数： *ptCreateVol
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 CreateVol(T_CreateVol *ptCreateVol);
/**********************************************************************
* 功能描述： 删除卷
* 输入参数： *ptDelVol
* 输出参数：无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
//SWORD32 DelVol(T_DelVol *ptDelVol);
SWORD32 DelVol(T_VolName *ptVolName);
/**********************************************************************
* 功能描述： 卷统计信息
* 输入参数： T_VolStat
* 输出参数：无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetVolStatData(T_GetVolStat *ptStatVolInfo);

/**********************************************************************
* 功能描述： 卷重命名
* 输入参数： 全部为入参
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 RenameVol(T_VolRename *ptVolRename);
/**********************************************************************
* 功能描述： 卷扩容
* 输入参数： 全部为入参
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 ExpandVol(T_ExpVol *ptExpVol);
/**********************************************************************
* 功能描述： 卷变条带
* 输入参数： 全部为入参
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 ReSegmentVol(T_ReSegVol *ptReSegVol);
/**********************************************************************
* 功能描述： 获取指定卷信息
* 输入参数： ptVolInf->sdwVolId
* 输出参数：其他为出参
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetVolInfo(T_VolInfo *ptVolInfo);
/**********************************************************************
* 功能描述： 卷写0初始化
* 输入参数： ptVolInit->sdwVdId,   ptVolInit->sdwVolId
* 输出参数：其他为出参
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 ZeroInitVol(T_VolName *VolName);
/**********************************************************************
* 功能描述： 设置卷prefer控制器
* 输入参数： 全部为入参
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 SetVolPreferCtrl(T_VolPreferCtrl *ptVolPreferCtrl);
/**********************************************************************
* 功能描述： 获取卷的任务信息
* 输入参数： ptVolTaskInf->sdwVdId, ptVolTaskInf->sdwVolId
* 输出参数：其他为出参
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetVolTaskInfo(T_VolGetTaskInfo *ptVolTaskInfo);
/**********************************************************************
* 功能描述： 创建快照
* 输入参数： 全部为入参
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 CreateSvol(T_SvolCreate *ptSvolCreate);
/**********************************************************************
* 功能描述： 删除快照
* 输入参数： 全部为入参
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 DelSvol(T_SvolName *ptSvolName);
/**********************************************************************
* 功能描述： 停止快照
* 输入参数： 全部为入参
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明： 底层SNAP暂时最小功能集中不做
***********************************************************************/
//SWORD32 StopSvol(T_SvolStop *ptSvolStop);
/**********************************************************************
* 功能描述： 重建快照
* 输入参数： sdwSvolId
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明： 底层SNAP暂时最小功能集中不做
***********************************************************************/
//SWORD32 RecreateSvol(T_SvolName *ptSvolName);
/**********************************************************************
* 功能描述： 恢复快照
* 输入参数： sdwSvolId
* 输出参数：无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：底层SNAP暂时最小功能集中不做
***********************************************************************/
SWORD32 RestoreSvol(T_SvolName *ptSvolName);
/**********************************************************************
* 功能描述： 重命名快照
* 输入参数： 全部为入参
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 RenameSvol(T_SvolRename *ptSvolRename);
/**********************************************************************
* 功能描述： 查看指定快照卷属性
* 输入参数： ptSvolInfo-> udwSvolId;
* 输出参数： 其他为出参
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetSvolInfo(T_SvolInfo *ptSvolInfo);

/**********************************************************************
* 功能描述： 查看卷上的所有快照卷信息
* 输入参数：
* 输出参数： 全部
* 返 回 值：  0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetAllSvolInfoOnVol(T_AllSvolInfoOnVol *ptAllSvolInfoOnVol);

/**********************************************************************
* 功能描述： 查看所有快照卷属性
* 输入参数：
* 输出参数： 全部
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
// SWORD32 GetAllSvolInfo(T_AllSvolInfo *ptAllSvolInfo);

/**********************************************************************
* 功能描述： 查看指定快照备份卷属性
* 输入参数： ptRvolInfo->udwRvolId
* 输出参数：其他为出参
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
//SWORD32 GetRvolInfo(T_RvolInfo *ptRvolInfo);
/**********************************************************************
* 功能描述： 设置快照卷告警阀值
* 输入参数： 全部为入参
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 SetSvolThr(T_SvolThr *ptSvolThr);

/**********************************************************************
* 功能描述：创建拷贝卷
* 输入参数：
* 输出参数： 无
* 返回值：    0   - SUCCESS    <0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 CreateCvol(T_CreateCvol  *ptCreateCvol);

/**********************************************************************
* 功能描述： 获取拷贝卷信息
* 输入参数：
* 输出参数： 无
* 返回值：    0   - SUCCESS    <0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetCvolInfo(T_CvolInfo *ptCvolInfo);

/**********************************************************************
* 功能描述： 获取源卷上的拷贝卷信息
* 输入参数：
* 输出参数： 无
* 返回值：    0   - SUCCESS    <0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetCvolNamesOnVol(T_CvolNamesOnVol *ptCvolNamesOnVol) ;

/**********************************************************************
* 功能描述： 设置拷贝卷同步或者反同步的优先级
* 输入参数：
* 输出参数： 无
* 返回值：    0   - SUCCESS    <0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 SetCvolPriority(T_CvolPriority *ptCvolPriority);

/**********************************************************************
* 功能描述： 设置是否启动保护拷贝
* 输入参数：
* 输出参数： 无
* 返回值：    0   - SUCCESS    <0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 SetCvolProtect (T_CvolProtect *ptCvolProtect);

/**********************************************************************
* 功能描述：删除拷贝卷
* 输入参数：
* 输出参数： 无
* 返回值：    0   - SUCCESS    <0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 DelCvol(T_CvolName *ptCvolName);

/**********************************************************************
* 功能描述： 分离拷贝卷
* 输入参数：
* 输出参数： 无
* 返回值：    0   - SUCCESS    <0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 FractureCvol(T_CvolName  *ptCvolName);

/**********************************************************************
* 功能描述： 同步拷贝卷
* 输入参数：
* 输出参数： 无
* 返回值：    0   - SUCCESS    <0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 SyncCvol(T_CvolName *ptCvolName);

/**********************************************************************
* 功能描述：反同步拷贝卷
* 输入参数：
* 输出参数： 无
* 返回值：    0   - SUCCESS    <0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 ReverseSyncCvol(T_CvolName *ptCvolName);

/**********************************************************************
* 功能描述： 重命名拷贝卷
* 输入参数： 全部为入参
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 RenameCvol(T_CvolRename *ptCvolRename);

/**********************************************************************
* 功能描述：获取系统中未被映射的卷对象(包括普通卷/快照/拷贝卷)信息
* 输入参数：
* 输出参数： 无
* 返回值：    0   - SUCCESS    <0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetVolUnmappedInfo(T_VolObjectInfoList *ptVolObjectInfoList);

/***************************************Lun*************************************/
/**********************************************************************
* 功能描述： 创建LUN
* 输入参数： 全部入参
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
//SWORD32 CreateLun(T_LunSimpleInfo *);
/**********************************************************************
* 功能描述： 删除LUN
* 输入参数： sdwLunGlbId
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
//SWORD32 DelLun(SWORD32 sdwLunGlbId);
/**********************************************************************
* 功能描述： 查询当前系统中存在的所有LUN的ID
* 输入参数： 无
* 输出参数： 全部为出参
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
//SWORD32 GetAllLunId(T_AllLunId *);
/**********************************************************************
* 功能描述： 根据ID查询指定LUN信息
* 输入参数： ptLunSimpleInfo->sdwLunGlbId
* 输出参数： 其他为出参
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
//SWORD32 GetLunSimpleInfo(T_LunSimpleInfo *ptLunSimpleInfo);
/**********************************************************************
* 功能描述： 修改VOL的Cache属性
* 输入参数： ptLunCacheInfo->sdwLunGlbId
* 输出参数： 其他为出参
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 SetVolCacheAttr(T_VolCacheInfo *ptVolCacheInfo);
/**********************************************************************
* 功能描述： 创建映射组
* 输入参数： sdwMapGrpId
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 CreateMapGrp(T_MapGrpName *ptMapGrpName);

/**********************************************************************
* 功能描述： 删除映射组
* 输入参数： sdwMapGrpId
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 DelMapGrp(T_MapGrpName *ptMapGrpName);

/**********************************************************************
* 功能描述： 重命令映射组
* 输入参数： sdwMapGrpId
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 RenameMapGrp(T_MapGrpRename *ptMapGrpRename);

/**********************************************************************
* 功能描述： 获取映射组信息
* 输入参数： ptMapGrpInfo
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetMapGrpInfo(T_MapGrpInfo *ptMapGrpInfo);

/**********************************************************************
* 功能描述： VOL加入映射组
* 输入参数： 全部为入参
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 AddVolToGrp(T_AddVolToGrp *ptAddVolToGrp);
/**********************************************************************
* 功能描述： VOL从映射组中删除
* 输入参数： 全部为入参
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 DelVolFromGrp(T_DelVolFrmGrp *ptDelVolFromGrp);
/**********************************************************************
* 功能描述： Host加入映射组
* 输入参数： 全部为入参
* 输出参数：  无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 AddHostToGrp(T_HostInGrp *ptHostInGrp);
/**********************************************************************
* 功能描述： Host从映射组中删除
* 输入参数：  全部为入参
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 DelHostFromGrp(T_HostInGrp *ptHostInGrp);
/**********************************************************************
* 功能描述： 查询所有映射组ID列表
* 输入参数： 无
* 输出参数： 全部为出参
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetGrpNamesList(T_AllGrpNames *ptAllGrpNames);

/**********************************************************************
* 功能描述： 查询指定映射组中所有host
* 输入参数： ptAllHostGrp->sdwMapGroup
* 输出参数： 其他为出参
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
//SWORD32 GetGrpHostList(T_AllHostInGrp *ptAllHostGrp);
/**********************************************************************
* 功能描述： 查询指定映射组中所有lun
* 输入参数： ptAllLunGrp->swMapGroupId
* 输出参数： 其他为出参
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
//SWORD32 GetGrpLunList(T_AllLunInGrp *ptAllLunGrp);
/**********************************************************************
* 功能描述： 查询指定host所在映射组名称
* 输入参数： ptHostGrpId->ucInitName
* 输出参数： 其他为出参
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetGrpNameOfHost(T_HostInGrp *ptHostInGrp);
/**********************************************************************
* 功能描述： 查询VOL所在的映射组列表
* 输入参数： ptLunGrpId->sdwLunGlbId
* 输出参数： 其他为出参
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetGrpNamesOfVol(T_GrpNameByVol *ptGrpNameByVol);
/**********************************************************************
* 功能描述： 打开/关闭映射组内Lun的IO统计开关
* 输入参数： 全部为入参
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 输出参数： 无
* 其它说明：
***********************************************************************/
SWORD32 SetLunIOStaticSwitchStatus(T_LunIOStaticSwitch *ptVolIOStaticSwitch);
/**********************************************************************
* 功能描述： 查询映射组内Lun的IO统计开关状态
* 输入参数： 全部为入参
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetLunIOStaticSwitchStatus(T_LunIOStaticSwitch *ptVolIOStaticSwitch);
/**********************************************************************
* 功能描述： 获取映射组内Lun的IO统计数据
* 输入参数：ptLunIoStaticData->sdwLunLocalId,ptLunIoStaticData->ucInitName
* 输出参数：其他为出参
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetLunIOStaticData(T_LunIOStaticData *ptVolIoStaticData);

/***************************************Cache***********************************/
/**********************************************************************
* 功能描述： 设置cache全局配置
* 输入参数： 全部为入参
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 SetCacheGlobalPolicy(T_SetCacheGlbPolicy *ptSetCacheGlbPolicy);
/**********************************************************************
* 功能描述： 获取cache全局配置
* 输入参数： 无
* 输出参数： 全部为出参
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetCacheGlobalPolicy(T_GetCacheGlbPolicy *ptGetCacheGlbPolicy);
/**********************************************************************
* 功能描述： 获取buffer的使用信息，
* 输入参数： 无
* 输出参数： 全部为出参
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetCacheBuffInfo(T_CacheBuffInfo *ptCacheBuffInfo);
/**********************************************************************
* 功能描述： 获得统计开关的状态
* 输入参数： 无
* 输出参数：sdwSwitch
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetCacheStatSwitchStaus(T_CacheIOStatSwitch *ptCacheIOStatSwitch);
/**********************************************************************
* 功能描述： 设置统计开关
* 输入参数： sdwSwitch
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 SetCacheStatSwitchStaus(T_CacheIOStatSwitch *ptCacheIOStatSwitch);
/**********************************************************************
* 功能描述： 获取IO统计信息，包括读写命令个数，大小，命中率
* 输入参数： 无
* 输出参数： 全部为出参
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetCacheStatData(T_CacheIOStatData *ptCacheIOStatData);

/***************************************Other***********************************/
/**********************************************************************
* 功能描述： 获取所有封装ID
* 输入参数： 无
* 输出参数： 全部为出参
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetAllEncId(T_AllEncId *ptAllEncId);

/* SWORD32 GetCtlHwInfo();*/
/**********************************************************************
* 功能描述： 扩展柜版本更新
* 输入参数： *ptUpdateFirmware
* 输出参数： 无
* 其它说明：
***********************************************************************/
SWORD32 UpdateFirmware(T_UpdateFirmware *ptUpdateFirmware);
/**********************************************************************
* 功能描述： 扩展柜版本查询
* 输入参数： 无
* 输出参数： *ptFirmwareVer
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetFirmwareVer(T_FirmwareVersion *ptFirmwareVer);
/**********************************************************************
* 功能描述： 重启控制器
* 输入参数： 全部为入参
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 RestartCtrl(T_CtrlAction *ptCtrlAction);
/**********************************************************************
* 功能描述： 关闭控制器
* 输入参数： 全部为入参
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 ShutdownCtrl(T_CtrlAction *ptCtrlAction);
/**********************************************************************
* 功能描述： 获取当前控制器ID
* 输入参数： 无
* 输出参数：*sdwCtlId
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetLocalCtlId(SWORD32 *sdwCtlId);
/**********************************************************************
* 功能描述： 获取当前控制器状态
* 输入参数： 无
* 输出参数： *LocalCtrlState
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetLocalCtrlState(SWORD32 *LocalCtrlState);
/**********************************************************************
* 功能描述： 获取对端控制器状态
* 输入参数： 无
* 输出参数： *PeerCtlState
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetPeerCtlState(SWORD32 *PeerCtlState);
/**********************************************************************
* 功能描述： 获取当前控制器角色
* 输入参数： 无
* 输出参数： *LocalCtlRole
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetLocalCtlRole(SWORD32 *LocalCtlRole);

/**********************************************************************
* 功能描述： 将配置保存为指定类型文件
* 输入参数： *ptConfigFile
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 SaveConfigFile(T_ConfigFile *ptConfigFile);
/**********************************************************************
* 功能描述： 恢复指定文件配置
* 输入参数： *ptConfigFile
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 RestoreConfigFile(T_ConfigFile *ptConfigFile);

/**********************************************************************
* 功能描述： 恢复出厂配置
* 输入参数： 无
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 RestoreOriginalConfig(void);
/**********************************************************************
* 功能描述： 设置最低日志级别
* 输入参数： sdwLevel
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 SetLogLevel(SWORD32 sdwLevel);
/**********************************************************************
* 功能描述： 根据预置的过滤条件查询日志的详细内容
* 输入参数： T_LogGet  *ptLogGetPara：入参预置的过滤条件细节
* 输出参数： T_LogTableOnce *ptLogTableList
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：用户界面上不指定用户的情况下，
                                 即非用户管理日志查询的条件下，
                                 用户名字段需要置位为LOG_USER_NONE_DEFAULT_NAME
***********************************************************************/
SWORD32 GetLogByLevel(T_LogGet  *ptLogGetPara, T_LogTableOnce *ptLogTableList);
/**********************************************************************
* 功能描述： 日志导出到指定目录文件
* 输入参数： 导出文件的名称：不带路径
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 ExportLog(T_LogExport *pcFileName);
/**********************************************************************
* 功能描述： 清除相应的日志
* 输入参数： T_LogUserName *ptUserName
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：用户界面上不指定用户的情况下，
                                 即非用户管理日志查询的条件下，
                                 用户名字段需要置位为LOG_USER_NONE_DEFAULT_NAME
***********************************************************************/
SWORD32 ClearLog(void);
/**********************************************************************
* 功能描述： 获得符合过滤条件的日志条数
* 输入参数： T_LogGetNum *ptLogNumInfo：入参预置的过滤条件细节
* 输出参数： 获得的日志数目wLogNum
* 返 回 值：  0      -   执行成功
*             非0    -   执行失败
* 其它说明：用户界面上不指定用户的情况下，
                                 即非用户管理日志查询的条件下，
                                 用户名字段需要置位为LOG_USER_NONE_DEFAULT_NAME
***********************************************************************/
SWORD32 GetLogNum(T_LogGetNum *ptLogNumInfo, WORD16 *pwLogNum);

/**********************************************************************
* 功能描述： 获得整板的诊断信息
* 输入参数： 诊断对象类型diagDeviceType，诊断发起者DiagSourceType
* 输出参数： 其它为出参
* 返 回 值：  0      -   执行成功
*             非0    -   执行失败
* 其它说明：无
***********************************************************************/
SWORD32 GetDiagDeviceInfo(T_DiagDeviceInfo *ptDiagDeviceInfo);

/**********************************************************************
* 功能描述： 获得BBU电源诊断信息
* 输入参数：无
* 输出参数： 全部
* 返 回 值：  0      -   执行成功
*             非0    -   执行失败
* 其它说明：无
***********************************************************************/
SWORD32 GetDiagBbuInfo(T_DiagBatteryInfo *ptDiagbatteryInfo);


/**********************************************************************
* 功能描述： 获得EPLD诊断信息
* 输入参数：无
* 输出参数： 全部
* 返 回 值：  0      -   执行成功
*             非0    -   执行失败
* 其它说明：无
***********************************************************************/
SWORD32 GetDiagEpldInfo(T_DiagEpldInfo *ptDiagEpldInfo);

/**********************************************************************
* 功能描述： 获得EEPROM诊断信息
* 输入参数：无
* 输出参数： 全部
* 返 回 值：  0      -   执行成功
*             非0    -   执行失败
* 其它说明：无
***********************************************************************/
SWORD32 GetDiagEepromInfo(T_DiagEepromInfo *ptDiagEepromInfo);

/**********************************************************************
* 功能描述： 获得Rtc设备诊断信息
* 输入参数：无
* 输出参数： 全部
* 返 回 值：  0      -   执行成功
*             非0    -   执行失败
* 其它说明：无
***********************************************************************/
SWORD32 GetDiagRtcInfo(T_DiagRtcInfo *ptDiagRtcInfo);

/**********************************************************************
* 功能描述： 获得Crystal设备诊断信息
* 输入参数：无
* 输出参数： 全部
* 返 回 值：  0      -   执行成功
*             非0    -   执行失败
* 其它说明：无
***********************************************************************/
SWORD32 GetDiagCrystalInfo(T_DiagCrystalInfo *ptDiagCrystalInfo);

/**********************************************************************
* 功能描述： 获得锁相环(时钟相关)诊断信息
* 输入参数：无
* 输出参数： 全部
* 返 回 值：  0      -   执行成功
*             非0    -   执行失败
* 其它说明：无
***********************************************************************/
SWORD32 GetDiagClockInfo(T_DiagClockInfo *ptDiagClockInfo);

/**********************************************************************
* 功能描述：获取诊断设备记录
* 输入参数：控制器标志--sdwCtrlFlg, 诊断器件ID--sdwDiagDeviceId
* 输出参数：诊断对象数目--sdwDeviceNum, 诊断对象记录--tDiagSingleDeviceRecord[2]
* 返 回 值：  0      -   执行成功
*             非0    -   执行失败
* 其它说明：无
***********************************************************************/
SWORD32 GetDiagDeviceRecord(T_DiagDeviceRecord *ptDiagDeviceRecord);

/**********************************************************************
* 功能描述：获取PM8001诊断信息
* 输入参数：控制器标志--sdwCtrlFlg
* 输出参数：诊断PM8001数目--sdwPmNum, 诊断信息列表--tDiagSinglePmInfo[2]
* 返 回 值：  0 - 执行成功
*           非0 - 执行失败
* 其它说明：无
***********************************************************************/
SWORD32 GetDiagPmInfo(T_DiagPmInfo *ptDiagPmInfo);

/**********************************************************************
* 功能描述：获取PCIE诊断信息
* 输入参数：控制器标志--sdwCtrlFlg
* 输出参数：诊断PCIE数目--sdwPcieNum, 诊断信息列表--tDiagSinglePcieInfo[2]
* 返 回 值：  0 - 执行成功
*           非0 - 执行失败
* 其它说明：无
***********************************************************************/
SWORD32 GetDiagPcieInfo(T_DiagPcieInfo *ptDiagPcieInfo);

/**********************************************************************
* 函数名称： SetLogMailCfg
* 功能描述： 设置日志告警邮件相关配置
* 输入参数： 日志告警邮件配置结构体
* 输出参数：
* 返 回 值：  0      -   执行成功
*             非0    -   执行失败
* 其它说明：无
* 修改日期        版本号     修改人       修改内容
***********************************************************************/
SWORD32 SetLogMailCfg(T_LogAlarmMailCfgPack *ptLogMailCfg);

/**********************************************************************
* 函数名称： SetLogMailCfg
* 功能描述： 设置日志告警邮件相关配置
* 输入参数： 日志配置结构体
* 输出参数：
* 返 回 值：  0      -   执行成功
*             非0    -   执行失败
* 其它说明：无
***********************************************************************/
SWORD32 GetLogMailCfg(T_LogAlarmMailCfgPack *ptLogMailCfg);

/**********************************************************************
* 函数名称： GetMailSwitch
* 功能描述： 获取当前的邮件开关使能设置
* 输入参数：
* 输出参数： 使能开关值指针
* 返 回 值：  0      -   执行成功
*             非0    -   执行失败
* 其它说明：无
***********************************************************************/
SWORD32 GetMailSwitch(BYTE *pucMailEnable);

/**********************************************************************
* 函数名称： SetMailSwitch
* 功能描述： 设置当前的邮件开关使能设置
* 输入参数： 使能开关值
* 输出参数：
* 返 回 值：  0      -   执行成功
*             非0    -   执行失败
* 其它说明：无
***********************************************************************/
SWORD32 SetMailSwitch(BYTE ucMailEnable);

/**********************************************************************
* 函数名称： GetMailConf
* 功能描述： 获取设置邮件的配置选项
* 输入参数： 无
* 输出参数： 邮件本身的配置项
* 返 回 值：  0      -   执行成功
*             非0    -   执行失败
* 其它说明：无
***********************************************************************/
SWORD32 GetMailConf (T_MailConfPack  *ptMailConf);

/**********************************************************************
* 函数名称： SetMailConf
* 功能描述： 设置邮件的配置选项
* 输入参数： 邮件本身的配置项
* 输出参数：
* 返 回 值：  0      -   执行成功
*             非0    -   执行失败
* 其它说明：无
***********************************************************************/
SWORD32 SetMailConf (T_MailConfPack  *ptMailConf);

/**********************************************************************
* 函数名称： scsVerifyMailTest
* 功能描述： 发送验证邮箱配置的测试邮件
* 输入参数： 发送邮件的目的地址（是长度为MAX_MAILTO_LEN字符串数组的指针）
* 输出参数：
* 返 回 值：  0      -   执行成功
*             非0    -   执行失败
* 其它说明：无
***********************************************************************/
SWORD32 VerifyMailTest(T_VerifyMailTestTo *pcTo);

/**********************************************************************
* 功能描述：根据targetId ,获取Initiator信息列表
* 输入参数：targetId
* 输出参数：
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetIscsiInitList(T_IscsiIniInfoList *ptIscsiIniInfoList);

/**********************************************************************
* 功能描述：根据targetId ,获取所有Session 信息列表
* 输入参数：targetId
* 输出参数：
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetIscsiAllSessInfoList(T_IscsiSessInfoList *ptIscsiSessList);

/**********************************************************************
* 功能描述：根据targetId ,获取所有Session 信息列表
* 输入参数：targetId
* 输出参数：
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetIscsiHistorySessInfoList(T_IscsiSessInfoList *ptIscsiHistorySessList);

/**********************************************************************
* 功能描述：根据Session Id ,获取所有Session属性信息
* 输入参数：Session Id
* 输出参数：
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetIscsiSessionAttr(T_GetIscsiSessAttrInfo *ptIscsiSessAttr);

/**********************************************************************
* 功能描述：根据targetId,统计指定端口/所有端口的流量信息
* 输入参数：targetId、Port Id
* 输出参数：
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetIscsiFlowInfoStat(T_IscsiFlowStatInfo *ptIscsiPortStatInfo);

/**********************************************************************
* 功能描述：根据targetId,获取Target Name
* 输入参数：targetId
* 输出参数：
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetIscsiTargetName(T_IscsiTargetInfo *ptIscsiTgtInfo);

/**********************************************************************
* 功能描述：根据targetId,获取流量统计开关状态
* 输入参数：targetId
* 输出参数：
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetIscsiFlowStatSwitchStatus(T_IscsiFlowStatFlag *ptIscsiTgtInfo);

/**********************************************************************
* 功能描述：根据targetId,设置流量统计开关状态
* 输入参数：targetId
* 输出参数：
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 SetIscsiFlowStatSwitchStatus(T_IscsiFlowStatFlag*ptIscsiTgtInfo);


/**********************************************************************
* 功能描述：根据targetId, 获取端口信息列表
* 输入参数：targetId
* 输出参数：
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetIscsiPortInfoList(T_IscsiPortalList *ptIscsiPortInfo);

/**********************************************************************
* 功能描述：获取扩展柜类型
* 输入参数：
* 输出参数：
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetEnclosureType(T_EnclosureType *ptEnclosureTyp);

/**********************************************************************
* 功能描述：获取Expander拓扑结构信息列表
* 输入参数：
* 输出参数：
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetExpanderTopoList(T_EncTopoInfoList * ptEncTopoInfoList);

/**********************************************************************
* 功能描述：诊断Expander设备列表
* 输入参数：
* 输出参数：
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 DiagExpanderList(T_EncDeviceInfoList * ptEncDevInfoList);

/**********************************************************************
* 功能描述：配置系统网络相关的信息
* 输入参数：
* 输出参数：
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 SetSystemNetCfg(T_SystemNetCfg *ptSystemNetCfg);

/**********************************************************************
* 功能描述：查询系统网络相关的信息
* 输入参数：
* 输出参数：
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetSystemNetCfg(T_SystemNetCfgList *ptSystemNetCfgList);


/**********************************************************************
* 功能描述：设置Iscsi 端口Mac 地址
* 输入参数：
* 输出参数：
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 SetIscsiPortMac(T_IscsiPortMac *ptIscsiPortMac);

/**********************************************************************
* 功能描述：查询Iscsi 端口Mac 地址列表
* 输入参数：
* 输出参数：
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetIscsiPortMac(T_IscsiPortMacList *ptIscsiPortMacList);

/**********************************************************************
* 功能描述：设置Iscsi 端口信息
* 输入参数：控制器ID、端口号、MAC、MTU
* 输出参数：无
* 返回值：    0   - SUCCESS   < 0     -  FAILURE
* 其它说明：该设置包括端口MAC、MTU,可以二者同时设置，也可设置其一
***********************************************************************/
SWORD32 SetIscsiPortInfo(T_IscsiPortSet *ptIscsiPortSet);

/**********************************************************************
* 功能描述：查询Iscsi 端口信息列表
* 输入参数：无
* 输出参数：全部为出参
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：该信息包括端口MAC、MTU、端口状态信息
***********************************************************************/
SWORD32 GetIscsiPortInfo(T_IscsiPortInfoList *ptIscsiPortInfoList);

/**********************************************************************
* 功能描述：添加Iscsi 端口路由信息
* 输入参数：控制器ID、端口号、目标IP、掩码、下一跳IP
* 输出参数：无
* 返回值：    0   - SUCCESS   <0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 AddIscsiPortRoute(T_IscsiPortRoute *ptIscsiPortRoute);

/**********************************************************************
* 功能描述：删除Iscsi 端口路由信息
* 输入参数：控制器ID、目标IP、掩码
* 输出参数：无
* 返回值：    0   - SUCCESS   <0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 DelIscsiPortRoute(T_IscsiPortRouteDel *ptIscsiPortRouteDel);

/**********************************************************************
* 功能描述：查询Iscsi 端口路由信息
* 输入参数：控制器ID
* 输出参数：路由数据、路由信息列表
* 返回值：    0   - SUCCESS   <0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetIscsiPortRoute(T_IscsiPortRouteList *ptIscsiPortRouteList);

/**********************************************************************
* 功能描述：Iscsi 端口ping 功能
* 输入参数：目标IP
* 输出参数：
* 返回值：    0   - SUCCESS   <0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetPingInfo(T_IscsiPingInfo *ptIscsiPingInfo);

/**********************************************************************
* 功能描述：修改Iscsi Name
* 输入参数：
* 输出参数：
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 SetIscsiName(T_SetIscsiName *ptSetIscsiName);

/**********************************************************************
* 功能描述：重启Iscsi服务
* 输入参数：
* 输出参数：
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 RestartIscsiService(SWORD32 *psdwCtrlId);

/**********************************************************************
* 功能描述： 获取版本目录剩余空间大小
* 输入参数： cFileName
* 输出参数： dwLocalFreeSize、dwPeerFreeSize
* 返 回 值： 0         -   空间够用
*            错误码    -   空间不够
* 其它说明：
***********************************************************************/
SWORD32 GetFreeSpace(T_VerFreeSpace *ptVerFreeSpace);
/**********************************************************************
* 函数名称： UpdatePreProcess
* 功能描述： 更新主柜版本前的预处理
* 输入参数： ucFileType
* 输出参数： 无
* 返 回 值： 0  -   成功  非0  -  失败
* 其它说明：
***********************************************************************/
SWORD32 UpdatePreProcess(BYTE ucFileType);

/**********************************************************************
* 功能描述： 版本文件校验
* 输入参数： ptVerFileCheck
* 输出参数： 校验结果
* 返 回 值： 0      -   执行成功
*            非0    -   执行失败
* 其它说明：
***********************************************************************/
SWORD32 VerFileCheck(T_VerFileCheck *ptVerFileCheck);

/**********************************************************************
* 功能描述：上传文件预处理
* 输入参数：
* 输出参数：
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：无
***********************************************************************/
SWORD32 UploadPreProcess(T_UploadPreInfo *ptUploadPreInfo);

/**********************************************************************
* 功能描述：通过Ftp 方式上传文件
* 输入参数：
* 输出参数：
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：无
***********************************************************************/
SWORD32 UploadFileByFtp(T_UploadFileInfo *ptUploadFileInfo);

/**********************************************************************
* 函数名称： DownloadFileByFtp
* 功能描述： 将配置文件从FTP客户端传递到FTP服务器端
* 访问的表：
* 修改的表：
* 输入参数：
* 输出参数：
* 返 回 值：  0   - 执行成功
*             非0 - 执行失败
* 其它说明：无
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2011/8/16       V1.0       陶林君             初次创建
***********************************************************************/
SWORD32 DownloadFileByFtp(T_DownloadFileInfo *ptUploadFileInfo);

/**********************************************************************
* 功能描述：获取文件上传进度
* 输入参数：
* 输出参数：
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：无
***********************************************************************/

SWORD32 GetFileUploadProgress(T_UploadFileProgress *ptUploadFileProgress);
/**********************************************************************
* 功能描述：获取文件下载进度
* 输入参数：
* 输出参数：
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：无
***********************************************************************/
SWORD32 GetFileDownloadProgress(T_DownloadFileProgress *ptDownloadFileProgress);

/***************************************************************
* 功能描述：删除版本目录下同类型文件
* 输入参数：T_UploadFileName
* 输出参数：无
* 返 回 值: 0--成功；非0--失败
* 其他说明：
***************************************************************/
SWORD32 DelDuplicate(T_UploadFileName *ptFileName);

/**********************************************************************
* 功能描述：取消文件上传
* 输入参数：
* 输出参数：
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：无
***********************************************************************/
SWORD32 CancelUploadFile(T_UploadFileName *ptFileName);
/**********************************************************************
* 功能描述：判断文件的存在性
* 输入参数：cFileName--文件名
* 输出参数：ucExistType---存在类型
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：无
***********************************************************************/
SWORD32 IsFileDualExist(T_IsFileExist *ptIsFileExist);
/**********************************************************************
* 功能描述：获取系统时间以及时区城市
* 输入参数：
* 输出参数：
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：无
***********************************************************************/
SWORD32 GetTimeInfo(T_TimeInfo *ptSysTimeInfo);

/**********************************************************************
* 功能描述：获取时区城市列表
* 输入参数：
* 输出参数：
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：无
***********************************************************************/
SWORD32 GetTimeZoneCityList(T_ZoneCityList *ptZoneCityList);

/**********************************************************************
* 功能描述：设置系统时间
* 输入参数：
* 输出参数：
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：无
***********************************************************************/
SWORD32 SetSysTime(T_SysTime *ptSysTime);

/**********************************************************************
* 功能描述：设置时区
* 输入参数：
* 输出参数：
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：无
***********************************************************************/
SWORD32 SetTimeZone(T_TimeZoneCityName *ptZoneCity);

/**********************************************************************
* 功能描述：获取NTP服务器配置
* 输入参数：
* 输出参数：
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：无
***********************************************************************/
SWORD32 GetNTPServerCfg(T_NtpInfo *ptNtpInfo);

/**********************************************************************
* 功能描述：设置NTP服务器配置
* 输入参数：
* 输出参数：
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：无
***********************************************************************/
SWORD32 SetNTPServerCfg(T_NtpInfo *ptNtpInfo);

/**********************************************************************
* 功能描述：同步系统时间
* 输入参数：
* 输出参数：
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：无
***********************************************************************/
SWORD32 SynchronizeSysTimeByNtp(T_NtpSync *ptNtpSyn);

/**********************************************************************
* 功能描述：获取所有后台任务信息
* 输入参数：无
* 输出参数：全是出参
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：无
***********************************************************************/
SWORD32 GetTaskInfo(T_TaskInfoList *ptTaskInfoList);

/**********************************************************************
* 功能描述：暂停/ 运行/ 删除后台任务
* 输入参数：sdwTaskId 、sdwTaskCtrl
* 输出参数：无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：无
***********************************************************************/
SWORD32 SetTask(T_TaskCtrl *PtTaskCtrl );

/**********************************************************************
* 功能描述：添加后台任务
* 输入参数：
* 输出参数：无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：无
***********************************************************************/
SWORD32 AddTask(T_TaskAddInfo *ptTaskAddInfo);

/**********************************************************************
* 功能描述：获取所有计划任务信息
* 输入参数：无
* 输出参数：全是出参
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：无
***********************************************************************/
SWORD32 GetScheduleInfo(T_ScheduleList *ptScheduleList);

/**********************************************************************
* 功能描述： 删除计划任务
* 输入参数：sdwTaskId
* 输出参数：无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：无
***********************************************************************/
SWORD32 DelSchedule(T_ScheduleDel *ptScheduleDel);

/**********************************************************************
* 功能描述：用户登录
* 输入参数：ptUser->acUsername  ptUser->adPassword
* 输出参数：无
* 返 回 值：   0      -   执行成功
*                                非0 - 执行失败
* 其它说明：
***********************************************************************/
SWORD32  Login(T_AmsUser *ptUser);

/**********************************************************************
* 功能描述：用户登出
* 输入参数：无
* 输出参数：无
* 返 回 值：   0    -   执行成功
*                非0 - 执行失败
* 其它说明：
***********************************************************************/
SWORD32 Logout(void);

/**********************************************************************
* 功能描述：创建session
* 输入参数：ptSession->sdwSessionId
* 输出参数：
* 返 回 值：   0      -   执行成功
*                                非0 - 执行失败
* 其它说明：
***********************************************************************/
SWORD32 CreateSession(T_SessionId * ptSession);

/**********************************************************************
* 功能描述：共享session
* 输入参数：ptSession->sdwSessionId
* 输出参数：无
* 返 回 值：   0    -   执行成功
*                非0 - 执行失败
* 其它说明：
***********************************************************************/
SWORD32 EnterShareSession(T_SessionId * ptSession);

/**********************************************************************
* 功能描述：退出session
* 输入参数：ptSession->sdwSessionId
* 输出参数：无
* 返 回 值：   0      -   执行成功
*                                非0 - 执行失败
* 其它说明：
***********************************************************************/
SWORD32  ExitShareSession(void);

/**********************************************************************
* 功能描述：删除session
* 输入参数：ptSession->sdwSessionId
* 输出参数：无
* 返 回 值：   0    -   执行成功
*                非0 - 执行失败
* 其它说明：
***********************************************************************/
SWORD32 DestroySession(T_SessionId * ptSession);

/**********************************************************************
* 功能描述：创建用户
* 输入参数：ptUser->acUsername  ptUser->adPassword  ptUser->sdwUserType
* 输出参数：无
* 返 回 值：   0      -   执行成功
*                                非0 - 执行失败
* 其它说明：
***********************************************************************/
SWORD32 CreateAccount(T_AmsUser * ptUser);

/**********************************************************************
* 功能描述：删除用户
* 输入参数：ptAccount->acUsername
* 输出参数：无
* 返 回 值：   0    -   执行成功
*                非0 - 执行失败
* 其它说明：
***********************************************************************/
SWORD32 DelAccount (T_Account * ptAccount);

/**********************************************************************
* 功能描述：获取用户列表
* 输入参数：无
* 输出参数：ptAccountList
* 返 回 值：   0      -   执行成功
*                                非0 - 执行失败
* 其它说明：
***********************************************************************/
SWORD32 GetAccountList(T_AccountList * ptAccountList);

/**********************************************************************
* 功能描述：获取登录者信息
* 输入参数：无
* 输出参数：ptAccountList
* 返 回 值：   0      -   执行成功
*                                非0 - 执行失败
* 其它说明：
***********************************************************************/
SWORD32 GetLoginedUser(T_AccountList * ptAccountList);

/**********************************************************************
* 功能描述：修改密码
* 输入参数：ptPasswdSet->acUsername  ptPasswdSet->acOldPwd  ptPasswdSet->acNewPwd
* 输出参数：无
* 返 回 值：   0    -   执行成功
*                非0 - 执行失败
* 其它说明：
***********************************************************************/
SWORD32 SetPasswd (T_PwdSet *ptPasswdSet);

/**********************************************************************
* 功能描述：修改名字
* 输入参数：ptUserNameMody->acUsername   ptUserNameMody->acUserNewname
* 输出参数：无
* 返 回 值：   0    -   执行成功
*                非0 - 执行失败
* 其它说明：
***********************************************************************/
SWORD32 ModifyUserName (T_UserNameMody *ptUserNameMody);

/**********************************************************************
* 功能描述：获取用户密码提醒邮箱
* 输入参数：ptPwdMail->acUsername
* 输出参数：无
* 返 回 值：   0      -   执行成功
*                                非0 - 执行失败
* 其它说明：
***********************************************************************/
SWORD32 GetPwdMail (T_PwdMail * ptPwdMail);

/**********************************************************************
* 功能描述：设置用户密码提醒邮箱
* 输入参数：ptPwdMail->acUsername ptPwdMail->acPwdMail
* 输出参数：无
* 返 回 值：   0    -   执行成功
*                非0 - 执行失败
* 其它说明：
***********************************************************************/
SWORD32 SetPwdMail (T_PwdMail * ptPwdMail);

/**********************************************************************
* 功能描述：发送密码提醒邮件
* 输入参数：ptPwdMail->acUsername
* 输出参数：无
* 返 回 值：   0      -   执行成功
*                                非0 - 执行失败
* 其它说明：
***********************************************************************/
SWORD32 SendPwdMail (T_PwdMail * ptPwdMail);

/**********************************************************************
* 功能描述：刷新SMA上的超时时间，重新计算UI超时
* 输入参数：Null
* 输出参数：
* 返 回 值：  NOERR, execute successfully
* 其它说明：
***********************************************************************/
SWORD32 RefreshConTime(void);
/**********************************************************************
* 功能描述：用于web的心跳
* 输入参数：WebCheckSidExistOrNot
* 输出参数：
* 返 回 值：  NOERR, execute successfully
                               -E_SMA_SESSION_NEXIST,sid不存在
* 其它说明：
***********************************************************************/
SWORD32 WebCheckSidExistOrNot(T_WebSessionIdExist * ptSession);


/**********************************************************************
* 功能描述：获取系统信息
* 输入参数：T_SysInfo
* 输出参数：
* 返 回 值：  0      -   执行成功
*                                非0 - 执行失败
* 其它说明：
***********************************************************************/
SWORD32 GetSysInfo(T_SysInfo *ptSysInfo);

/**********************************************************************
* 功能描述：修改系统名称
* 输入参数：T_SysNameInfo
* 输出参数：
* 返 回 值： 0      -   执行成功
*                                非0 - 执行失败
* 其它说明：
***********************************************************************/
SWORD32 SetSysName(T_SysNameInfo *ptSysName);

/**********************************************************************
* 功能描述：查看控制器详细信息
* 输入参数：T_CtrlInfo
* 输出参数：
* 返 回 值： 0      -   执行成功
*                                非0 - 执行失败
* 其它说明：
***********************************************************************/
SWORD32 GetCtrlInfo(T_CtrlInfo *ptCtrlInfo);

/**********************************************************************

* 功能描述：修改本端控制器名称
* 输入参数：T_SysNameInfo
* 输出参数：
* 返 回 值： 0      -   执行成功
*                                非0 - 执行失败
* 其它说明：
***********************************************************************/
SWORD32 SetCtrlName(T_CtrlInfo *ptCtrlInfo);

/**********************************************************************
* 功能描述：查看磁盘SMART信息
* 输入参数：T_DmSmartInfo
* 输出参数：
* 返 回 值： 0      -   执行成功
*                                非0 - 执行失败
* 其它说明：
***********************************************************************/
SWORD32 GetHddSmartInfo(T_DmSmartInfo * ptSmartInfo);
/**********************************************************************
* 功能描述：启动SMART扫描
* 输入参数：T_DmSmartTestCtl
* 输出参数：
* 返 回 值： 0      -   执行成功
*                                非0 - 执行失败
* 其它说明：
***********************************************************************/
SWORD32 ExecHddSmartTest(T_DmSmartTestCtl *ptSmartTest);
/**********************************************************************
* 功能描述：设置磁盘节能减排开关
* 输入参数：T_DmPowerStatSwitch
* 输出参数：
* 返 回 值： 0      -   执行成功
*                                非0 - 执行失败
* 其它说明：
***********************************************************************/
SWORD32 SetHddPowerSwitchStatus(T_DmPowerStatSwitch *ptPowSwitch);
/**********************************************************************
* 功能描述：获取磁盘节能减排开关
* 输入参数：T_DmPowerStatSwitch
* 输出参数：
* 返 回 值： 0      -   执行成功
*                                非0 - 执行失败
* 其它说明：
***********************************************************************/
SWORD32 GetHddPowerSwitchStatus(T_DmPowerStatSwitch *ptPowSwitch);
/**********************************************************************
* 功能描述：同步文件到对端
* 输入参数：文件名称(不带路径)
* 输出参数：
* 返 回 值： 0      -   执行成功
*                                非0 - 执行失败
* 其它说明：
***********************************************************************/
SWORD32 SyncFileToPeer(T_FileToPeer *ptFileToPeer);
/**********************************************************************
* 功能描述：同步文件(两端都可以同步)
* 输入参数：文件名称(不带路径)
* 输出参数：
* 返 回 值： 0      -   执行成功
*                                非0 - 执行失败
* 其它说明：
***********************************************************************/
SWORD32 SyncFileToDual(T_FileToPeer *ptFileToPeer);
/**********************************************************************
* 功能描述：获取系统启动完成时间
* 输入参数：无
* 输出参数：ptSysBootTime->tSysSingleBootTime[2]
* 返 回 值： 0 - 执行成功
*          非0 - 执行失败
* 其它说明：
***********************************************************************/
SWORD32 GetSysBootTime(T_SysBootTime *ptSysBootTime);
/**********************************************************************/

#endif

#endif
