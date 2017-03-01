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

#ifndef __USP_CAPI_LIB_H__
#define __USP_CAPI_LIB_H__

#include "usp_common_macro.h"
#include "usp_common_typedef.h"

typedef struct
{
    SWORD32 sdwSvrPort;                 /*存储设备监听的端口号*/
    CHAR    ucSvrIpAddr[MAX_IP_LEN];    /*存储设备IP   地址*/
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

/******************************** SIC *****************************************/

/* 压缩包信息 */
typedef struct
{
    T_Time tCreatTime;    /* 出参：压缩包创建的时间 */
    CHAR scTarBallName[MAX_SIC_FILE_NAME_LEN];    /* 出参：压缩包名字 */
}_PACKED_1_ T_TarBallInfo;

/* 每个控制器上压缩包信息 */
typedef struct
{
    SWORD32 sdwCtrlId;    /* 压缩包所在的控制器ID */
    T_TarBallInfo tTarBallInfo;
}_PACKED_1_ T_TarBallInfoPerCtrl;

/* 系统中所有控制器上的压缩包信息 */
typedef struct
{
    WORD32 dwCtrlNum;    /* 系统中控制器的数量 */
    T_TarBallInfoPerCtrl tTarBallInfoPerCtrl[MAX_CTL_NUM];    /* 压缩包信息列表 */
}_PACKED_1_ T_TarBallInfoList;

/* 邮件发送的配置 */
typedef struct
{
    BYTE ucEmailEnable;    /* 邮件发送使能(设置时为入参, 获取接口中为出参) */
}_PACKED_1_ T_EmailCfg;

typedef struct
{
    SWORD32 sdwCtlId;  /* 入参：取值参考T_CtrlAction结构体 */
}_PACKED_1_ T_SicCtrlAction;


typedef struct
{
    CHAR    ucFileName[MAX_CONFIG_FILE_NAME_LEN]; /* 仅文件名，不包括路径 */
}_PACKED_1_ T_SysInfoTarBall;

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
    CHAR     ucModelName[MAX_PD_MODEL_NAME_LEN];  /* model名称 */
    CHAR     ucPhysicalType[MAX_PD_TYPE_LEN];     /* 硬盘物理类型(e.g. SATA1, SATA2, SAS, FC) */
    CHAR     ucFwVer[MAX_PD_FW_VER_LEN];          /* Firmware版本.*/
    CHAR     ucSerialNo[MAX_PD_SNO_LEN];          /* 序列号 */
    CHAR     ucStdVerNum[MAX_PD_STD_VER_LEN];     /* 标准版本号. */
    SWORD32  sdwHealthState;                      /* 磁盘的健康状态，包含可访问性和SMART 状态，取值为PD_GOOD(1)、PD_BROKEN(0)*/
    SWORD32  sdwSourceType;                       /* ePdSrc   eSrc;取值PD_LOCAL(0), PD_FOREIGN(2)*/
    SWORD32  sdwLogicType;                        /* ePdType  eType;取值PD_UNKOWN(3),PD_UNUSED(0),PD_HOTSPARE(4), PD_DATA(5)、PD_CONFLICT(6)*/
    SWORD32  ucObjectType;                        /* 磁盘所属类型，TYPE_VD 表示 vd，TYPE_POOL 表示 pool */
    CHAR     cVdName[MAX_BLK_NAME_LEN];           /* 虚拟盘名称 */
    SWORD32  sdwVisibleCtrl;                      /* 对该盘可见的控制器及状态 */
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
    //CHAR     cVolNames[MAX_VOL_NUM_ON_VD][MAX_BLK_NAME_LEN];   /* 指定VD 上的卷名称列表 */
    CHAR     cVolNames[MAX_ALLTYPE_VOL_NUM_ON_VD][MAX_BLK_NAME_LEN];   /* 指定VD 上的卷名称列表 */
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
typedef struct
{
    CHAR      cVolName[MAX_BLK_NAME_LEN]; /* Vol名称,不能为空*/
    SWORD32   sdwOwned;        /* 卷的所属，取值:VOL_IN_VD ---表示vol属于VD, VOL_IN_POOL ---表示vol属于pool */
}_PACKED_1_ T_VolAttr;

/* BLK与SCS可以通用该结构体 最后一个成员由SCS填充*/
typedef struct
{
    CHAR      cVolName[MAX_BLK_NAME_LEN]; /* Vol名称,不能为空*/
    CHAR      cVdName[MAX_BLK_NAME_LEN];  /*VD名称或POOL名称,不能为空*/
    BYTE      ucVolUuid[MAX_DEV_UUID_LEN];  /* 卷的UUID  */
    SWORD64   sqwCapacity;     /* vol capacity  ,KB*/
    SWORD32   sdwLevel;        /* eRaidlevel, RAID level, defined in struct eRaidlevel ,取值RAID0(0),RAID1(1),RAID3(3), RAID5(5),RAID6(6),
                                                   RAIDTP(7),RAID10(10),RAID30(30),RAID50(50),RAID60(60),NRAID(200),RAID_LEVEL_INVALID(0xffff) */
    SWORD32   sdwState;        /* eVolState , vol state defined in struct eVolState,取值VOL_S_GOOD(0),
                                               VOL_S_FAULT(1),  VOL_S_ERR(2),VOL_S_CHUNKSIZE_MIGRATION(3),
                                               VOL_S_EXPAND(4),VOL_S_UNREADY(5),VOL_S_INVALID(0xffff) */
    SWORD32   sdwOwned;        /* 卷的所属，取值:VOL_IN_VD ---表示vol属于VD, VOL_IN_POOL ---表示vol属于pool */
    SWORD32   sdwStripeSize;   /* vol chunk size ,KB*/
    SWORD32   sdwOwnerCtrl;    /* owner control ,暂时为0,1 */
    SWORD32   sdwPreferCtrl;   /*  首选控制器,暂时为0,1 */
    T_Time    tCreateTime;     /* 创建时间  */
    SWORD32   sdwMapState;     /* 卷的映射状态，取值MAPPED_VOL(1),NO_MAPPED_VOL(0) */
    SWORD32   sdwCachePolicy;  /* CACHE 写回写穿策略, 取值CACHE_WRITE_BACK_POLICY(0),CACHE_WRITE_THROUGH_POLICY(1) */
    SWORD32   sdwAheadReadSize;/* cache预读系数,取值0~2048 */
    SWORD32   sdwIsRvol;       /* 源卷是否已经存在快照资源空间 ，出参*/
    SWORD32   sdwSvolNum;      /* 源卷上快照卷的个数 ，出参*/
    SWORD32   sdwCloneNum;     /* 源卷上克隆卷的个数 ，出参*/
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



typedef struct
{
   CHAR  cBvolName[MAX_BLK_NAME_LEN]; /*源卷名称*/
   CHAR  cRvolName[MAX_BLK_NAME_LEN]; /*快照资源空间名称*/
   SWORD32 sdworiginPercent; /*快照资源空间占源卷容量初始百分比*/
   SWORD32 sdwrvolThr;		  /*快照资源空间告警阀值*/
   SWORD32 sdwrvolStrategy;		/*资源空间满后处理策略*/
   SWORD32 sdwautoExpand; 	  /*快照资源空间达到告警后是否自动扩容*/
   SWORD32 sdwexpandPercent; /*每次扩容多大百分比，默认10%，该字段仅在设置auotExpand为时候有效*/
}_PACKED_1_ T_RvolCreate;


typedef struct
{
    CHAR  cBvolName[MAX_BLK_NAME_LEN];  /*快照资源空间名称*/
}T_RvolDel;


typedef struct
{
    CHAR  cBvolName[MAX_BLK_NAME_LEN]; /*快照资源空间名称*/
    SWORD32 sdwrvolThr;    /*告警阈值*/
      SWORD32 sdwrvolStrategy; /*处理策略*/
    SWORD32 sdwautoExpand;  /*是否自动扩容*/
    SWORD32 sdwexpandPercent; /*当自动扩容时，每次扩容百分比*/
}_PACKED_1_ T_RvolSet;


typedef struct
{
    CHAR  cRvolName[MAX_BLK_NAME_LEN]; /*快照资源空间名称*/
    SWORD32 sdwexpandPercent; /*扩容百分比*/
}_PACKED_1_ T_RvolExpand;


typedef struct
{
    CHAR    cRvolName[MAX_BLK_NAME_LEN]; /*快照资源空间名称*/
    CHAR    cBvolName[MAX_BLK_NAME_LEN]; /*源卷名称*/
    SWORD64 sqwtotalCapacity;     	/*快照资源空间总容量*/
    SWORD64 sqwusedCapacity;	/*快照资源空间已使用容量*/
    SWORD32 sdwrvolThr;   /*告警阈值*/
    SWORD32 sdwrvolStrategy; /*处理策略*/
    SWORD32 sdwautoExpand; /*是否自动扩容*/
}_PACKED_1_ T_RvolInfo;



/* BLK与SCS可以通用该结构体 */
typedef struct
{

    CHAR     cVolName[MAX_BLK_NAME_LEN];    /* Vol名称*/
    CHAR     cSvolName[MAX_BLK_NAME_LEN];   /* 快照卷名称 */
    SWORD32 sdwAgentAttr;                   /* 1表示向快照代理发送快照通知，0表示不发送 */

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
    CHAR     cVdName[MAX_BLK_NAME_LEN];    /* Vd名称 */
    SWORD32  sdwState;                     /* The state of the svol, the possible values are %SVOL_S_ACTIVE, %SVOL_S_FAULTY.*/
    T_Time   tCreateTime;                  /* Svol create time */
    //SWORD64  uqwCapacity;                  /* Snapshot volume size (in sectors),快照卷容量,KB */
    //SWORD64  uqwUsedCapacity;              /* Used Snapshot volume size (in sectors),已使用快照卷容量 */
    //SWORD32  sdwSvolThr;                   /* 快照卷的报警阈值，百分比表示*/
    //SWORD32  sdwSvolNotify;                /* Overflow treatment strategies of rvol ("fail svol" or "fail write to bvol")，容量溢出后的处理方式标记 */
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
    SWORD32    sdwSvolNum;                       /* 快照卷数目*/
    T_SvolInfo tSvolInfo[MAX_SVOL_NUM_IN_SYS];   /* 快照卷信息列表*/
}_PACKED_1_ T_AllSvolInfo;

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
    WORD64 sdwCvolCapacity;               /* clone卷的容量KB,要求不小于源卷 */
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
    WORD64 sdwCvolCapacity;   /* clone卷的容量KB */
    SWORD32 sdwCvolChunkSize;   /* clone卷的chunk单位,KB */
    SWORD32 sdwInitSync;     /* 是否进行初始同步, 1-YES, 0--NO */
    SWORD32 sdwProtectRestore;   /* 是否启动保护拷贝, 1-YES, 0--NO */
    SWORD32 sdwPri;   /* 同步或反同步的速率 */
    SWORD32 sdwRelation;   /* 拷贝卷与源卷的关联关系, 1--关联, 0--分离 */
    SWORD32 CvolState;   /* 拷贝卷的状态 */
    SWORD32 sdwLunMapState;  /* 拷贝卷的映射状态，取值MAPPED_VOL(1),NO_MAPPED_VOL(0) ，出参*/
 }_PACKED_1_ T_CvolInfo;

typedef struct
{
    CHAR    scCloneName[MAX_BLK_NAME_LEN];      /* 拷贝卷的名字 */
    CHAR    scCloneVdName[MAX_BLK_NAME_LEN];    /* 拷贝卷所在vd*/
    CHAR    scBvolName[MAX_BLK_NAME_LEN];       /* 源卷名*/
    CHAR    scBvolVdName[MAX_BLK_NAME_LEN];     /* 源卷所在vd*/
    WORD64 sdwCloneCapacity;   /* clone卷的容量KB */
}T_CvolBaseInfo;

typedef struct
{
    SWORD32    sdwCvolNum;                       /* 拷贝卷数目*/
    T_CvolBaseInfo tCvolBaseInfo[MAX_CLONE_IN_SYS];      /* 拷贝卷信息列表*/
}_PACKED_1_ T_AllCvolInfo;



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
    T_VolObjectInfo    tVolObjectList[MAX_LUN_NUM_IN_SYS];  /* 系统中未映射的卷对象的信息列表*/
}_PACKED_1_ T_VolObjectInfoList;


typedef struct
{
    SWORD32  sdwSlotId;       /* 磁盘槽位号，取值0~27(ScanBox);  0~11(SPR10) */
    SWORD32  sdwEnclosureId;  /* 箱体Id，取值 0~实际箱体数*/
    SWORD64  sqwCapacity;     /* 磁盘容量 */
}_PACKED_1_  T_PdIdInfo;

typedef struct
{
    SWORD32      sdwUnusedPdNum;                    /* 空闲盘数目 */
    SWORD32      sdwSasPdNum;                       /* SAS成员盘数目 */
    T_PdIdInfo   tSasMemberPdId[MAX_PD_NUM_IN_VD];  /* SAS成员盘列表 */
    SWORD32      sdwSataPdNum;                      /* SATA成员盘数目 */
    T_PdIdInfo   tSataMemberPdId[MAX_PD_NUM_IN_VD]; /* SATA成员磁盘列表 */
}_PACKED_1_ T_AllUnusedPd;

typedef struct
{
    T_PdId       tPdId[MAX_PD_NUM_IN_VD];
    SWORD32      sdwPdNum;                  //补充了该字段，便于判断，省略再次检索。
    SWORD64      sqwCapacity;               /* 磁盘容量 */
}_PACKED_1_ T_FindPd;

typedef struct
{
    SWORD32      sdwCheckNum;                       /* 待记录检查的磁盘数目，需要大于sdwPdNum才有意义 */
    SWORD32      sdwPdNum;                          /* 指定用于创建虚拟盘的磁盘数目 */
    T_PdIdInfo   tPdIdInfo[MAX_PD_NUM_IN_VD];       /* 待记录磁盘信息 */
    T_PdIdInfo   tRightPdInfo[MAX_PD_NUM_IN_VD];    /* 找到的用于创建vd的磁盘信息，出参 */
}_PACKED_1_ T_RecordPd;


/***************************************卷虚拟化相关结构体**************************************/

typedef struct
{
    CHAR     scPoolName[MAX_BLK_NAME_LEN]; /* Pool名称,不能为空,入参*/
    SWORD32  sdwCreateType;                /* 创建方式：1 按磁盘数目(POOL_CREATE_BY_DISK_NUM)；2 按磁盘列表(POOL_CREATE_BY_DISK_LIST)*/
    SWORD32  sdwPdNum;                     /* 磁盘数目，最大256，入参*/
    T_PdId   tPdId[MAX_DISKS_IN_POOL];     /* 磁盘信息列表 ，当sdwCreateType 为POOL_CREATE_BY_DISK_LIST时有效，可选入参 */
    SWORD32  sdwAlarmThreshold;            /* 容量告警阈值 ，百分比值，比如值为5，表示剩余容量为5%时告警，入参*/
    SWORD32  sdwCapacityPolicy;            /* 空间保留策略 ,取值：POOL_CAPACITY_POLITY_NO_RESERVE，POOL_CAPACITY_POLITY_RESERVE_ONE，
                                              POOL_CAPACITY_POLITY_RESERVE_TWO，默认取值为POOL_CAPACITY_POLITY_RESERVE_ONE。入参 */
}_PACKED_1_ T_CreatePool;


typedef struct
{
    CHAR    scPoolName[MAX_BLK_NAME_LEN];  /* Pool 名称,不能为空，入参*/
}_PACKED_1_ T_PoolName;

typedef struct
{
    CHAR     scPoolName[MAX_BLK_NAME_LEN];  /* Pool 名称,不能为空*/
    CHAR     scVolName[MAX_BLK_NAME_LEN];   /* Vol名称，不能为空*/
    SWORD32  sdwRaidlevel;                  /* raid level */
    SWORD32  sdwChunkSize;                  /* new chunk size,其为4的倍数，单位:KB， 取值MIN_CHUNK_SIZE(4)~MAX_CHUNK_SIZE(512 )*/
    WORD64   qwCapacity;                    /* 卷容量, 单位 KB ，入参*/
    SWORD32  sdwCtrlPrefer;                 /* 首选控制器,暂时为0,1 */
    //SWORD32  sdwAutoRebuild;                /* 自动重建开关 ，ON(1)/OFF(0)，入参*/
    SWORD32  sdwCachePolicy;                /* Cache写回写穿策略, 取值CACHE_WRITE_BACK_POLICY(0),CACHE_WRITE_THROUGH_POLICY(1) */
    SWORD32  sdwAheadReadSize;              /* Cache预读系数,取值0~2048 ，无单位*/
}_PACKED_1_ T_CreateVolOnPool;

typedef struct
{
    CHAR     scPoolName[MAX_BLK_NAME_LEN];       /* Pool 名称,不能为空, 入参*/
    WORD32   dwstate;                            /* Pool的状态，POOL_DAMAGED，POOL_GOOD，POOL_REDISTRIBUTING，POOL_INSUFCAPACITY */
    SWORD32  sdwAllPdNum;                        /* 成员盘数目*/
    T_PdId   tAllMemberPdId[MAX_DISKS_IN_POOL];  /* 成员盘列表*/
    SWORD32  sdwFaultPdNum;                      /* 成员盘中故障盘数目*/
    T_PdId   tFaultMemberPdId[MAX_DISKS_IN_POOL];/* 故障成员磁盘列表*/
    WORD64   qwTotalCapacity;                    /* 存储池总容量,容量大小，以MB为单位,下同 */
    WORD64   qwUsedCapacity;                     /* 存储池已使用容量 */
    WORD64   qwFreeCapacity;                     /* 存储池可用容量，不包括保留空间 */
    WORD64   qwAlarmThreshold;                   /* 容量告警阈值 */
    SWORD32  sdwCapacityPolicy;                  /* 空间保留策略，取值：POOL_CAPACITY_POLITY_NO_RESERVE(无保留空间用于重建)，
                                                                       POOL_CAPACITY_POLITY_RESERVE_ONE(保留一块磁盘的空间用于重建)，
                                                                       POOL_CAPACITY_POLITY_RESERVE_TWO(保留两块磁盘的空间用于重建) */
    T_Time   tCreateTime;                        /* Pool 创建时间 */
}_PACKED_1_ T_PoolInfo;

typedef struct
{
    SWORD32 sdwPoolNum;                                          /* 系统中存储池总数 ，出参*/
    CHAR    sdwPoolNames[MAX_POOL_NUM_IN_SYS][MAX_BLK_NAME_LEN]; /* 存储池名字列表 ，出参*/
}_PACKED_1_ T_AllPoolNames;

typedef struct
{
    CHAR     scPoolName[MAX_BLK_NAME_LEN];  /* Pool名称,不能为空，入参*/
    SWORD32  sdwVolType;                    /* 卷类型，包含普通卷（NORM_VOL）、快照卷（SNAPSHOT_VOL）、拷贝卷（CLONE_VOL）,入参 */
    SWORD32  sdwVolNum;                     /* 卷的数目 */
    CHAR     scVolNames[MAX_VOL_NUM_ON_POOL][MAX_BLK_NAME_LEN];   /* 指定Pool上的卷名称列表 */
}_PACKED_1_ T_VolNamesOnPool;

typedef struct
{
    CHAR    scPoolName[MAX_BLK_NAME_LEN];   /* Pool名称 */
    CHAR    scNewName[MAX_BLK_NAME_LEN];    /* 存储池新名称，如果不更改名字，传入空串表示该值无效 */
    SWORD32 sdwAlarmThreshold;              /* 容量告警阈值，百分比值，如不需要修改，用-1表示该值无效 */
    SWORD32 sdwCapacityPolicy;              /* 空间保留策略，如不需要修改，用-1表示该值无效该值无效 */
}_PACKED_1_ T_ModifyPool;

typedef struct
{
    CHAR    scPoolName[MAX_BLK_NAME_LEN];   /*  Pool名称 */
    SWORD32 sdwModifyType;                  /*  修改方式：1 按磁盘数目(POOL_MODIFY_DISK_BY_NUM)；2 按磁盘列表(POOL_MODIFY_DISK_BY_LIST)*/
    SWORD32 sdwPdNum;                       /*  磁盘个数 ,入参*/
    T_PdId  tPdId[MAX_DISKS_IN_POOL];       /*  磁盘列表，最大256 ，如果按磁盘列表，该值有效，根据“sdwModifyType”可选入参*/
}_PACKED_1_ T_ModifyDiskOnPool;

typedef struct
{
    CHAR     scVolName[MAX_BLK_NAME_LEN]; /* Vol名称，不能为空，入参*/
    WORD64   qwExpandCapacity;            /* 新扩充的容量值，单位KB ，入参*/
}_PACKED_1_ T_ExpVolCapacity;

/***************************************卷虚拟化结构体定义结束**************************************/

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

typedef struct
{
    CHAR     cVdName[MAX_BLK_NAME_LEN];         /* Vd 名称,不能为空 */
    SWORD32  sdwPdNum;                          /* 最大32 */
    SWORD32  sdwVdRaidLevel;                    /* Vd的raid级别 */
    SWORD32  sdwAutoRebuild;                    /* 自动重建开关 ON自动重建，OFF不自动重建 */
}_PACKED_1_ T_AutoCreateVd;

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



/***************************************Task**************************************/
/* 后台任务相关*/
typedef struct
{
    SWORD32    sdwTaskId;                              /*任务ID*/
    CHAR       cObjectName[MAX_TASK_OBJECT_NAME_LEN];  /*任务所属对象名*/
    WORD32     sdwTaskType;                            /* 任务类型,取值见宏定义*/
    SWORD32    sdwTaskState;                           /* 任务状态，取值TASK_RUNNING、TASK_PAUSE、TASK_DELAY*/
    T_Time     tStartTime;                             /* 开始时间，年、月、日、时、分、秒*/
    SWORD32    sdwTaskProgress;                        /* 任务进度，整数，不带%*/
    SWORD32    sdwTaskAttr;                            /* 任务属性，按bit位表示，置位(1表示属性有效)有效，其中b0:可删除属性；b1:可暂停属性；b2:暂停后的可继续运行属性*/   
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
    CHAR        cObjectName[MAX_TASK_OBJECT_NAME_LEN];      /* 任务所属对象名,input */
    WORD32      sdwTaskType;                           /* 任务类型,取值见宏定义,input */
    SWORD32     sdwPrarmLen;                           /* 任务参数长度,input */
    CHAR        ucTaskPrarm[MAX_TASK_PRARM_LEN];       /* 任务所需参数,input */
}_PACKED_1_ T_TaskAddInfo;

/* 计划任务相关*/

typedef struct
{
    SWORD32 swTaskId;      //计划任务id号
    SWORD32 scTaskType;    //任务类型，参考eSchdTaskType定义
    CHAR    cObjectName[MAX_SCHEDULE_OBJECT_NAME_LEN];   //计划任务所属的对象名称
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



/***************************************Mirror**************************************/
#define MAX_UUID_LEN MAX_DEV_UUID_LEN+1
/* 创建远程镜像 */
typedef struct
{
    CHAR    scMirrorName[MAX_BLK_NAME_LEN];                      /* 镜像名称 ，入参*/
    CHAR    scSrcVolName[MAX_BLK_NAME_LEN];                      /* 镜像源卷名称 ，入参*/
    CHAR    ucRmtDiskUuid[MAX_UUID_LEN];                     /* 镜像目的卷UUID,即远程磁盘UUID ，入参*/
    SWORD32 sdwMirrorType;                                       /* 镜像的类型,取值：MIRROR_SYNCHRONOUS、MIRROR_ASYNCHRONOUS  ，入参*/
    SWORD32 sdwIsInitSync;                                       /* 初始同步标记,取值：MIRROR_NOT_INIT_SYNC、MIRROR_IS_INIT_SYNC ，入参*/
    SWORD32 sdwPolicy;                                           /* 镜像的恢复策略,取值：MIRROR_MANUAL_POLICY、MIRROR_AUTO_POLICY，入参*/
    SWORD32 sdwPriority;                                         /* 同步的优先级,取值：SYNC_LOW、SYNC_MEDIUM、SYNC_HIGH，入参*/
    SWORD32 sdwPeriod;                                           /* 异步镜像的同步频率,当镜像类型为MIRROR_ASYNCHRONOUS时有效,单位:s, 取值5~300 ，可选入参*/
}_PACKED_1_ T_CreateMirror;

/* 远程镜像名称 */
typedef struct
{
    CHAR  scMirrorName[MAX_BLK_NAME_LEN];  /* 镜像名称 ,入参*/
}_PACKED_1_ T_MirrorName;

/* 远程镜像信息 */
typedef struct
{
    CHAR    scMirrorName[MAX_BLK_NAME_LEN];                      /* 镜像名称 ，入参*/
    CHAR    scSrcVolName[MAX_BLK_NAME_LEN];                      /* 镜像源卷名称 */
    CHAR    ucRmtDiskUuid[MAX_UUID_LEN];                     /* 镜像目的卷UUID,即远程磁盘UUID ，入参*/
    SWORD64 sdwDestVolCap;                                       /* 镜像目的卷的容量 KB */
    SWORD32 sdwMirrorType;                                       /* 镜像的类型,取值：MIRROR_SYNCHRONOUS、MIRROR_ASYNCHRONOUS  */
    SWORD32 sdwInitSync;                                         /* 初始同步标记,取值：MIRROR_NOT_INIT_SYNC、MIRROR_IS_INIT_SYNC */
    SWORD32 sdwPolicy;                                           /* 镜像的恢复策略,取值：MIRROR_MANUAL_POLICY、MIRROR_AUTO_POLICY*/
    SWORD32 sdwPriority;                                         /* 同步的优先级: 见SYNC_LOW、SYNC_MEDIUM、SYNC_HIGH*/
    SWORD32 sdwPeriod;                                           /* 异步镜像的同步频率，单位:s, 取值5~300 */
    SWORD32 sdwRelation;                                         /* 镜像关系, 取值：FRACTURED、UNFRACTURED*/
    SWORD32 sdwState;                                            /* 镜像状态, 取值：INIT ~ OUT_OF_INIT*/
}_PACKED_1_ T_MirrorInfo;

/* 所有镜像名称列表 */
typedef struct
{
    SWORD32      sdwMirrorNum;                                     /* 镜像数目 */
    T_MirrorName tMirrorName[MAX_MIRROR_IN_SYS];                   /* 镜像名称列表 */
}_PACKED_1_ T_AllMirrorNames;

/* 修改远程镜像名称  */
typedef struct
{
    CHAR    scMirrorName[MAX_BLK_NAME_LEN];        /* 镜像名称，入参*/
    CHAR    scMirrorNewName[MAX_BLK_NAME_LEN];     /* 镜像新名称 ，入参*/
}_PACKED_1_ T_MirrorRename;

/* 修改远程镜像恢复策略 */
typedef struct
{
    CHAR    scMirrorName[MAX_BLK_NAME_LEN];     /* 镜像名称 ，入参*/
    SWORD32 sdwPolicy;                          /* 镜像的恢复策略,取值：MIRROR_MANUAL_POLICY、MIRROR_AUTO_POLICY，，入参*/
}_PACKED_1_ T_MirrorPolicy;

/* 修改远程镜像同步优先级  */
typedef struct
{
    CHAR    scMirrorName[MAX_BLK_NAME_LEN];     /* 镜像名称 ，入参*/
    SWORD32 sdwPriority;                        /* 同步的优先级: 见SYNC_LOW、SYNC_MEDIUM、SYNC_HIGH，入参*/
}_PACKED_1_ T_MirrorPriority;

/* 修改远程镜像同步频率(仅异步镜像) */
typedef struct
{
    CHAR    scMirrorName[MAX_BLK_NAME_LEN];  /* 镜像名称 ，入参*/
    SWORD32 sdwPeriod;                       /* 异步镜像的同步频率，单位:s, 取值5~300，入参 */
}_PACKED_1_ T_MirrorPeriod;

/*FC 方式的 镜像逻辑连接 */
typedef struct
{
    SWORD32  sdwCtrlId;                   /* 控制器ID，入参*/
    BYTE     ucWWPN[MAX_FC_WWN_LENGTH];  /* WWPN ，入参*/
}_PACKED_1_ T_FcMirrorConnectInfo;

typedef struct
{
    T_FcMirrorConnectInfo   tFcMirrorConnect;    /* 远程镜像FC方式的 连接信息 */
    SWORD32 sdwFcNexusStatus;                      /* 连接状态，PORT_STATE_UP、PORT_STATE_DOWN*/
}_PACKED_1_ T_FcNexusInfo;

/* 所有镜像链接 */
typedef struct
{
    SWORD32  sdwFcConnectNum;                                                    /* 以FC方式的镜像连接数目 ，出参*/
    T_FcNexusInfo   tFcMirrorConnectInfo[MAX_MIRROR_CONNECT_NUM];        /* 以FC方式的镜像连接信息 ，出参*/
    /*待支持ISCSI方式后再做添加*/
    //SWORD32  sdwIscsiConnectNum;                                               /* 以Iscsi方式的镜像连接数目 ，出参*/
    //T_IscsiMirrorConnectInfo  tIscsiMirrorConnectInfo[MAX_MIRROR_CONNECT_NUM]; /* 以Iscsi方式的镜像连接信息 ，出参*/
}_PACKED_1_ T_MirrorConnectList;

/* FC方式接入的远程磁盘信息 */
typedef struct
{
    CHAR     ucRmtDiskUuid[MAX_UUID_LEN];           /* 远程磁盘UUID */
    SWORD32  sdwUsedFlag;                                /* 远端磁盘的使用标志 ，取值REMOTE_DISK_UNUSED、REMOTE_DISK_USED*/
    SWORD32  sdwLunId;                                   /* 远端磁盘在所在映射组中的Lun Id 信息*/
    SWORD64  sqwCapacity;                               /* 远端磁盘的容量,KB*/
    //T_FcMirrorConnectInfo tFcMirrorConnectInfo[2];     /* 远端磁盘的FC连接多路径信息 */
    SWORD32  sdwRemoteDiskState;                         /* 远程磁盘的状态信息，取值 0(正常)、1(异常)*/
}_PACKED_1_ T_FcRdInfo;

/* 所有远程磁盘信息  */
typedef struct
{
    SWORD32      sdwFcRdNum;                                          /* 以FC方式连接的远程磁盘数目 ，出参*/
    T_FcRdInfo   tFcRdInfo[MAX_MIRROR_REMOTE_DISK_NUM];               /* 以FC方式连接的远程磁盘信息 ，出参*/
    /*待支持ISCSI方式后再添加定义*/
    //SWORD32    sdwIscsiRdNum;                                       /* 以ISCSI方式连接的远程磁盘数目 */
    //T_IscsiRdInfo tIscsiRdInfo[MAX_MIRROR_REMOTE_DISK_NUM];         /* 以ISCSI方式连接的远程磁盘数目*/
}_PACKED_1_ T_RemoteDiskList;

/* FC 端口信息 */
typedef struct
{
   WORD32  sdwPortNo;                                /* 端口编号,按FC接口面板顺序进行,从1开始编号 */
   WORD32  udwPortStatus;                            /* 标识该FC端口是否有效,取值:FC_PORT_INVALID 、FC_PORT_VALID ;当取值为FC_PORT_INVALID时，除了sdwPortNo值有效外，其他属性值均无效  */
   BYTE    ucPortID[MAX_FC_PORT_ID_LENGTH];          /* 端口ID,FC协议生成,显示方式三个字节按照16进制依次显示，形如为0x123456      */
   WORD32  udwPortRate;                              /* 端口速率，取值：FC_PORT_SPEED_INVALID、FC_PORT_SPEED_2G、FC_PORT_SPEED_4G、FC_PORT_SPEED_8G.*/
   WORD32  udwLinkStatus;                            /* 链路状态，取值：PORT_STATE_INVALID、PORT_STATE_UP、PORT_STATE_DOWN*/
   WORD32  udwTopoType;                              /* 端口拓扑类型：FC_TOPO_INVALID、FC_TOPO_PRIVATE_LOOP、FC_TOPO_PUBLIC_LOOP、FC_TOPO_FABRIC、FC_TOPO_P2P*/
   BYTE    ucWwpn[MAX_FC_WWN_LENGTH];                /* 端口WWPN，输出格式 A1：A2：A3：A4：A5：A6：A7：A8, 全0值无效*/
}_PACKED_1_ T_FcPortInfo;

/* 每个控制器上FC 端口的信息 */
typedef struct
{
    WORD32 sdwCtrlId;                          /* 端口所在的控制器ID */
    BYTE ucWwnn[MAX_FC_WWN_LENGTH];            /* 端口所在单板的WWNN */
    T_FcPortInfo tFcPortInfo[MAX_FC_PORT_NUM]; /* FC接口信息结构,每个端口占用一个数组条目 */
}_PACKED_1_ T_FcPortInfoPerCtrl;

/* 系统中所有FC 端口的信息 */
typedef struct
{
    WORD32             udwCtrlNum;                             /*环境中Ctrl控制器数量*/
    T_FcPortInfoPerCtrl tFcPortInfoPerCtrl[MAX_CTL_NUM];       /*FC端口信息列表*/
}_PACKED_1_ T_FcPortInfoList;


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
    SWORD32 sdwLunState; /* lun 的状态 */
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
    CHAR      cVolName[MAX_BLK_NAME_LEN]; /* Vol/Svol名称,不能为空,入参*/
    SWORD32   sdwMapGrpNum;
    CHAR      cMapGrpNames[MAX_MAP_GROUP_NUM_IN_SYS][MAX_MAPGRP_NAME_LEN]; /* 映射组名称*/
    SWORD32 LunLocalId[MAX_MAP_GROUP_NUM_IN_SYS]; /* lun在此映射组中的本地ID */
}_PACKED_1_ T_GrpNameAndLunidByVol;

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
    BYTE       ucISID[6];             /*  标识主机端信息ID  */
    WORD16     udwTSIH;               /* 标识Tgt端信息Id  */
    WORD16     udwPortalGroupTag;
    WORD16     udwSvrPort;            /* iSCSI专用端口 3260*/
    CHAR       scTgtPortIp[MAX_IP_LEN];   /* 业务端口的IP */
    CHAR       cInitIp[MAX_IP_LEN];   /* Initiator IP */
    CHAR       cInitName[MAX_INI_NAME_LEN];  /* Initiator Name */
    WORD16     udwIniPort; /* 启动器发起会话的端口号 */
    WORD16     udwConnId; /* 链接号 */
}_PACKED_1_ T_IscsiSessInfo;

/*Target会话信息列表*/
typedef struct
{
    SWORD32     sdwCtrlId;
    WORD32      udwTgtId;                            /*Target Id*/
    WORD32      udwSessCount;                      /*Session 数量*/
    T_IscsiSessInfo tIscsiSessInfo[MAX_ISCSI_SESSION_COUNT];        /*Session 信息列表*/
}_PACKED_1_ T_IscsiSessInfoList;
/* 会话改为1024个后结构体修改 */
#if 0
typedef struct
{
    WORD32      udwSessCount;                      /*Session 数量*/
    WORD32      udwSessCountLocal;                      /* 本端Session 数量*/
    WORD32      udwSessCountPeer;                      /*对端Session 数量*/
    T_IscsiSessionAll tIscsiSessListInfo[MAX_ISCSI_SESSION_COUNT*2];        /*Session 信息列表*/
}_PACKED_1_ T_IscsiSessDblList;
#endif
typedef struct
{
    WORD32      udwCtrlId;                      /*控制器Id */
    WORD32      udwTgtId;                      /* Tgt Id */
    WORD32      udwSessCount;                      /*Session 数量*/
}_PACKED_1_ T_GetIscsiSessNum;

typedef struct
{
    SWORD32     sdwCtrlId;
    SWORD32      udwTgtId;                            /*Target Id*/
    BYTE       ucISID[6];             /*  标识主机端信息ID  */
    WORD16     udwTSIH;               /* 标识Tgt端信息Id  */
    WORD16     udwPortalGroupTag;
    WORD16     udwSvrPort;            /* iSCSI专用端口 3260*/
    CHAR       scTgtPortIp[MAX_IP_LEN];   /* 业务端口的IP */
    CHAR       cInitIp[MAX_IP_LEN];   /* Initiator IP */
    CHAR       cInitName[MAX_INI_NAME_LEN];  /* Initiator Name */
    WORD16     udwIniPort; /* 启动器发起会话的端口号 */
    WORD16     udwConnId; /* 链接号 */
}_PACKED_1_ T_IscsiSessionInfo;


typedef struct
{
    WORD32      udwCtrlId;
    WORD32      udwTid;
    WORD32      udwSessCount;                      /*每次要获取的Session 数量*/
    WORD32      udwSessBgnIndex;                      /*要获取信息的起始位置*/
    T_IscsiSessionInfo tIscsiSessListInfo[MAX_ISCSI_SESSION_NUM_ONCE];        /*Session 信息列表*/
}_PACKED_1_ T_IscsiSessGetOnceInfo;

typedef struct
 {
    WORD32      udwCtrlId;      /*控制器Id ，入参*/
    WORD32      udwPortId;      /* 端口 Id ，入参*/
    WORD32      udwGpnCount;    /*Fc Gpn数量，出参*/
 }_PACKED_1_ T_GetFcGpnNum;

typedef struct
{
   BYTE       ucWWPN[FC_WWN_LENGTH];  /* FC World Wide长度 */
}_PACKED_1_ T_FcGpnInfo;


typedef struct
{
    WORD32      udwCtrlId;                         /*控制器Id ，入参*/
    WORD32      udwPortId;                         /* 端口 Id ，入参*/
    WORD16      udwGpnBgnIndex;                    /*要获取信息的起始位置，入参，从1开始*/
    WORD16      udwNextGetIndex;                   /*返回的下一次获取的起始位置，出参*/
    WORD32      udwGpnCount;                       /*返回的Gpn 数量，出参*/
    BYTE        udwEndFlag;                        /* 是否继续获取的标识，出参 */

    T_FcGpnInfo tFcGpnListInfo[MAX_FC_GPN_NUM_ONCE]; /*gpn 信息列表，出参*/
}_PACKED_1_ T_GetOnceFcGpnInfo;


#if 0
typedef struct
{
    T_IscsiSessInfoList tIscsiSessInfoLocal;        /*本端Session 信息列表*/
    T_IscsiSessInfoList tIscsiSessInfoPeer;        /*对端Session 信息列表*/
}_PACKED_1_ T_IscsiSessInfoDualList;
#endif
typedef struct
{
    WORD32      udwCtrlCount;                      /*Session 数量*/
    T_IscsiSessInfoList tIscsiSessListInfo[2];        /*Session 信息列表*/
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
    T_IscsiPduInfo   tiscsiPduInfo; /*PDU 信息列表*/
}_PACKED_1_ T_IscsiPduInfoList;

typedef struct
{
    WORD32      udwCtrlCount;            /* 控制器个数*/
    T_IscsiPduInfoList  tIscsiPduInfo[2];    /* PDU信息列表*/
}_PACKED_1_ T_IscsiAllPduStatisticInfo;


/*获取session 属性*/
typedef struct
{
    //WORD64      uqwSessId;                /* Session Id, 入参*/
    SWORD32     sdwCtrlId;
    BYTE        ucISID[6];                 /*  标识主机端信息ID  */
    WORD16      udwTSIH;                 /* 标识Tgt端信息Id  */
    WORD32      udwTgtId;               /*Target Id */
    //WORD32      udwPortId;              /*port  Id */
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

typedef struct
{
    WORD32             udwCtrlCount;  /* 控制器个数*/
    T_IscsiTargetInfo  tIscsiTargetInfo[2];   /*Target Name*/
}_PACKED_1_ T_IscsiTargetList;

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
    WORD16       sPGT;    /* PortalGroupTag */
    WORD16       sServPort;                     /* 服务端口 */
}_PACKED_1_ T_IscsiPortalInfo;
typedef struct
{
    SWORD32           sdwCtrlId;
   WORD32            udwTgtId;                                                                               /*Target Id*/
   WORD32            udwPortCount;                                                                        /* 端口数量 */
   T_IscsiPortalInfo tISCSIPortalInfo[MAX_ISCSI_PORT_NUM_PER_CTL];             /* 端口信息列表*/
}_PACKED_1_ T_IscsiPortalList;

typedef struct
{
    WORD32             udwCtrlCount;                        /*Ctrl控制器 数量*/
    T_IscsiPortalList  tIscsiPortListInfo[2];        /*Port信息列表*/
}_PACKED_1_ T_IscsiNetPortalList;


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
/***************************************统计项补充功能***********************************/
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
    SWORD32  sdwCtrlId;      /* 控制器Id：0、1，对应宏定义CTRL0、CTRL1 */
    SWORD32  sdwOccupy;      /* CPU占用率 */
}_PACKED_1_ T_CpuOccupyPerCtl;

typedef struct
{
    SWORD32             sdwCtrlNum;             /* 控制器数目,  出参 */
    T_CpuOccupyPerCtl   tCpuOccu[ENC_ALL_CTL];  /* ENC_ALL_CTL值为2 ，表示两个控制器 */
}_PACKED_1_ T_CpuOccupy;

typedef struct
{
    SWORD32  sdwSlotId;          /* 磁盘槽位号，取值0~27(ScanBox);  0~11(SPR10) */
    SWORD32  sdwEnclosureId;    /* 箱体Id，取值 0~实际箱体数 */
    SWORD32  sdwCtrlId;         /* 控制器Id：0、1，对应宏定义CTRL0、CTRL1*/
    WORD64   dwCmdCount;        /* IO命令个数 */
    WORD64   dwCmdDataSize;    /* IO数据携带的总数据量大小，以字节为单位 */
}_PACKED_1_ T_SasStatPerCtrl;

typedef struct
{
    SWORD32           sdwSlotId;                   /* 磁盘槽位号，取值0~27(ScanBox);  0~11(SPR10) */
    SWORD32           sdwEnclosureId;             /* 箱体Id，取值 0~实际箱体数 */
    SWORD32           sdwCtrlNum;                 /* 控制器数目,  出参*/
    T_SasStatPerCtrl  tSasStatInfo[ENC_ALL_CTL];  /* 分别表示两个控制器信息 */
}_PACKED_1_ T_SasStatInfo;

typedef struct
{
    SWORD32  sdwCtrlId;        /* 控制器Id：0、1，对应宏定义CTRL0、CTRL1*/
    WORD64   HscAverageSpeed; /* 从通道建立以来平均速度，单位Byte/s */
    WORD64   HscCurrentSpeed; /* 最近1s内流出通道的数据，对用户相当于瞬时速率单位Byte/s */
    WORD64   HscTotalData;    /* 从通道建立以来流出通道的总数据量，单位Byte/s */
}_PACKED_1_ T_PcieFlowStatusPerCtl;

typedef struct
{
    SWORD32                 sdwCtrlNum;             /* 控制器数目,  出参*/
    T_PcieFlowStatusPerCtl  tPcieFlow[ENC_ALL_CTL]; /*  ENC_ALL_CTL定义为2 */
}_PACKED_1_ T_PcieFlowStatus;

typedef struct
{
    WORD32 dwPortNo;           /* 端口号 */
	WORD64 qwRcvPkts;          /* 接收报文数 */
	WORD64 qwRcvBytes;         /* 接收字节数 */
	WORD64 qwSndPkts;          /* 发送报文数 */
	WORD64 qwSndBytes;         /* 发送字节数 */
}_PACKED_1_ T_TcsStat;
typedef struct
{
    SWORD32       sdwCtrlId;         /* 控制器Id：0、1，对应宏定义CTRL0、CTRL1*/
    SWORD32       sdwPortNum;        /* 端口数目，值是宏定义TCS_PORT_NUM */
    T_TcsStat    tTcsStat[TCS_PORT_NUM];
}_PACKED_1_ T_TcsStatInfoPerCtl;

typedef struct
{
    SWORD32             sdwCtrlNum;                /* 控制器数目,  出参*/
    T_TcsStatInfoPerCtl tTcsStatInfo[ENC_ALL_CTL]; /*  ENC_ALL_CTL定义为2 */
}_PACKED_1_ T_TcsStatInfo;


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
    SWORD32  sdwEnclosureId;      /* 扩展柜箱体ID,  现默认为0 */
    SWORD32  sdwCtlId;            /* ctl id or expander id or ENC_ALL_CTL */
    SWORD32  sdwAction;           /* 在线(CTL_ONLINE_REBOOT)/非在(CTL_SINGLE_REBOOT)线重启标记CTL_SINGLE_REBOOT/CTL_ONLINE_REBOOT */
}_PACKED_1_ T_CtrlAction;

typedef struct
{
    SWORD32  sdwNum;                             /* 箱体个数*/
    SWORD32  sdwEnclosureId[MAX_ENC_NUM_IN_SYS]; /* 箱体Id，取值 0~实际箱体数*/
}_PACKED_1_ T_AllEncId;

typedef struct
{
    /* 待确定; */
    SWORD32 sdwCtlId;            /* 临时使用，为了配合Windows下的编译  */
}_PACKED_1_ T_CtlHwInfo;

typedef struct
{
    SWORD32 sdwFileType;                          /* eConfigFileType, 保存文件类型 */
    CHAR    ucFileName[MAX_CONFIG_FILE_NAME_LEN]; /* 仅文件名，不包括路径 */
}_PACKED_1_ T_ConfigFile;

typedef struct
{
	SWORD32 sdwRebootEventId;        /* 重启事件ID */
	BYTE    cRebootType;             /* 0---代表正常, 1----代表诊断芯片异常, 2----代表软件启动异常 */
}_PACKED_1_ T_RebootAudit;

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


/*ISCSI 端口信息  */
typedef struct
{
    WORD32 udwCtrlId;                        /* 控制器ID ，入参*/
    WORD32 dwPortNo;                         /* 端口号，在SPR10中，这个编号为1~4，入参*/
    SWORD32 sdwSetFlg;                       /* 设置标志，暂取值0、1、2，分别表示设置MAC、设置MTU、设置MAC和MTU ,(该入参AMS 赋值)*/
    WORD32 udwPortMtu;                       /* 端口MTU ，可选入参*/
    BYTE   ucMacData[6];                     /* MAC 地址，可选入参*/
}_PACKED_1_  T_IscsiPortSet;

/* 端口的基本物理信息 */
typedef struct
{
    BYTE    ucMacData[PORT_MAC_DATA_SIZE];                      /* MAC 地址*/
    WORD32  udwPortMtu;                                         /* 端口MTU */
    WORD32  udwPortState;                                       /* 端口状态，取值PORT_STATE_UP、PORT_STATE_DOWN*/
    WORD32  udwNegMode;                                         /* 协商模式,取值PORT_NEG_FORCE、PORT_NEG_AUTO*/
    WORD32  udwSpeed;                                           /* 端口速率值,取值PORT_SPEED_10M ~ PORT_SPEED_16G*/
    WORD32  udwDuplexMode;                                      /* 双工模式,取值PORT_DUPLEX_FULL、PORT_DUPLEX_HALF*/
}_PACKED_1_  T_PortInfo;

/*一个ISCSI 端口的物理信息 */
typedef struct
{
    WORD32 dwPortNo;                           /*端口号，在SPR10中，这个编号为1~4*/
    T_PortInfo tIscsiPortInfo;                 /* Iscsi  端口信息*/
}_PACKED_1_ T_IscsiPortInfo;


/*一个控制器上的所有ISCSI端口的信息*/
typedef struct
{
    WORD32   udwCtrlId;                                           /* 控制器ID */
    SWORD32  sdwIscsiPortNum;                                     /* Iscsi  端口数量 */
    T_IscsiPortInfo tIscsiPortInfo[SPR10_ISCSI_PORT_NUM];         /* Iscsi  端口信息列表 */
}_PACKED_1_ T_SingleIscsiPortInfo;

/*系统中的所有控制器上ISCSI端口的信息*/
typedef struct
{
    WORD32 udwCtrlNum;                                            /* 系统中控制器数目,出参 */
    T_SingleIscsiPortInfo tSingleIscsiPortInfo[2];                /* 系统中Iscsi端口物理信息,出参 */
}_PACKED_1_ T_IscsiPortInfoList;


/*一个控制器上的管理网口的物理信息*/
typedef struct
{
    WORD32 udwCtrlId;                        /* 控制器ID */
    T_PortInfo tManePortInfo;                /* 管理口信息*/
}_PACKED_1_ T_SingleManaPortInfo;

/*系统中的所有控制器上管理网口的物理信息*/
typedef struct
{
    WORD32 udwCtrlNum;                                   /* 系统中控制器数目,出参*/
    T_SingleManaPortInfo tSingleManaPortInfo[2];         /* 系统中管理网口物理信息列表，出参*/
}_PACKED_1_ T_ManaPortInfoList;

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
    CHAR     cFileName[FILE_NAME_LEN];      /* 待上传文件的文件名, 入参*/
    WORD32  dwLocalFreeSize;                /* 本端剩余空间大小 */
    WORD32  dwPeerFreeSize;                 /* 对端剩余空间大小 */
}_PACKED_1_ T_VerFreeSpace;

typedef struct
{
    CHAR     cFileName[FILE_NAME_LEN];        /* 待上传文件的文件名, 入参*/
    WORD16   ucFileType;                      /* USP_VER_TYPE/EXP_VER_TYPE/VPD_VER_TYPE/POW_VER_TYPE/BOOT_VER_TYPE 该字段目前只针对主柜即EncId=0有效 */
    SWORD32  sdwIsValid;                      /* 校验结果，0---校验通过，其他--校验不通过，返回错误码, 出参 */
}_PACKED_1_ T_VerFileCheck;

typedef struct
{
    CHAR   cFileName[FILE_NAME_LEN];        /* 待上传文件的文件名, 入参*/
    BYTE   ucUploadType;                    /* VERSION_FILE_TPYE--版本文件 CONFIG_FILE_TPYE--配置文件 OTHER_FILE_TYPE--其他 */
    WORD16 uwVerType;                       /* USP_VER_TYPE, BOOT_VER_TYPE, EXP_VER_TYPE, VPD_VER_TYPE, POW_VER_TYPE */
}_PACKED_1_ T_UploadPreInfo;

/* ftp上传文件涉及的结构体 */
typedef struct
{
    CHAR   cUserName[USR_NAME_LEN];         /* ftp服务器登录用户名 ，入参*/
    CHAR   cPassWord[PASSWD_LEN];           /* ftp服务器登录密码 ，入参*/
    CHAR   cFtpServerIPAddr[MAX_IP_LEN];    /* ftp服务器IP 地址，入参*/
    CHAR   cFilePath[FILE_PATH_LEN];        /* 待上传文件的文件名 (带相对于FTP服务器目录的绝对路径)，入参*/
    WORD16 uwVerType;                       /* USP_VER_TYPE, BOOT_VER_TYPE, EXP_VER_TYPE, VPD_VER_TYPE, POW_VER_TYPE */
    BYTE   ucUploadType;                    /* VERSION_FILE_TPYE--版本文件 CONFIG_FILE_TPYE--配置文件 LOG_FILE_TPYE--日志文件 OTHER_FILE_TYPE--其他 */
}_PACKED_1_ T_UploadFileInfo;

typedef struct
{
    BYTE    ucFileType;                     /* VERSION_FILE_TPYE--版本文件 CONFIG_FILE_TPYE--配置文件 LOG_FILE_TPYE--日志文件 OTHER_FILE_TYPE 其他文件*/
    WORD16  uwVerType;                      /* USP_VER_TYPE, BOOT_VER_TYPE, EXP_VER_TYPE, VPD_VER_TYPE, POW_VER_TYPE */
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
    BYTE    ucPeerSyncState;                /* 对端文件同步状态 */
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

typedef struct
{
    CHAR       cFilePath[FILE_NAME_LEN];       /* 文件名(带路径) ，入参*/
    BYTE       ucExistType;                    /* FILE_EXIST_SINGLE(5):存在,FILE_NOT_EXIST_SINGLE(6):不存在, ---出参 */
}_PACKED_1_ T_IsFileLocalExist;

typedef struct
{
    SWORD32 sdwCtrlId;                                  /* 控制器ID,暂时定义为0、1，对应的宏定义CTRL0、CTRL1*/
    CHAR    cManufactureName[MAX_DIAG_DEVICE_NAME_LEN]; /* BBU厂家信息 */
    CHAR    cDeviceName[MAX_DIAG_DEVICE_NAME_LEN];      /* BBU型号 */
    SWORD32 sdwBbuPresentInfo;                          /* BBU是否在位 ，在位为DIAG_BBU_PRESENT(0)，不在位为DIAG_BBU_NOPRESENT(1) */
    //SWORD32 sdwBbuWorkFlag;                             /* BBU充放电标志，正在充放电为DIAG_BBU_CHARGING(0)，不在充放电为DIAG_BBU_NOT_CHARGING(1) */
    SWORD32 sdwBbuDeviceStatus;                         /* BBU状态，取值:正常(BBU_NORMAL,0)、异常(BBU_ABNORMAL,1)、电量不足(BBU_LOW_CAPACITY,2) */
}_PACKED_1_ T_SingleBbuInfo;

typedef struct
{
    SWORD32 sdwCtrlFlg;                /* 控制器标志 ,暂时定义为0、1、2，对应的宏定义CTRL_LOCAL、CTRL_PEER、CTRL_DUAL, 入参 */
    SWORD32 sdwBbuNum;                 /* BBU电源的数目，出参 */
    T_SingleBbuInfo tSingleBbuInfo[2]; /* 信息列表，当 sdwCtrlFlg = 0/1 时,仅tSingleBbuInfo[0]有值,当 sdwCtrlFlg = 2 时 , tSingleBbuInfo都有值 */
}_PACKED_1_ T_BbuInfo;

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
    SWORD32 sdwCtrlId;                              /* 控制器ID,暂时定义为0、1，对应的宏定义CTRL0、CTRL1 */
    SWORD32 sdwBbuwMv;                              /* BBU当前电压，单位：mV */
    SWORD32 sdwBbuwCapacity;                        /* BBU剩余电量百分比 */
    //SWORD32 sdwBbuWorkFlag;                         /* BBU充放电标志，正在充放电为DIAG_BBU_CHARGING(0)，不在充放电为DIAG_BBU_NOT_CHARGING(1) */
    SWORD32 sdwBbuPresentInfo;                      /* BBU是否在位，在位为DIAG_BBU_PRESENT(0)，不在位为DIAG_BBU_NOPRESENT(1) */
    SWORD32 sdwBbuCommunicate;                      /* BBU能否通信，能通信为BBU_COMMUNICATE_OK(0)，不能通信为BBU_COMMUNICATE_FAIL(2) */
    SWORD32 sdwBbuCharge;                           /* BBU能否充电，能充电为BBU_CHARGE_OK(0)，不能充电为BBU_CHARGE_FAIL(3) */
    SWORD32 sdwBbuDischarge;                        /* BBU能否放电，能放电为BBU_DISCHARGE_OK(0)，不能放电为BBU_DISCHARGE_FAIL(4) */
    //SWORD32 sdwBbuOverTemperature;                  /* BBU是否过温，正常温度为BBU_NORMAL_TEMPERATURE(0)，过温为BBU_OVER_TEMPERATURE(5) */
    SWORD32 sdwBbuInvalid;                          /* BBU是否失效,未失效为BBU_VALID(0),失效为BBU_INVALID(6) */
    SWORD32 sdwBbuCapacity;                         /* BBU当前电量值，单位: mah */
    SWORD32 sdwBbuDeviceStatus;                     /* BBU状态，取值:正常(BBU_NORMAL,0)、异常(BBU_ABNORMAL,1)、电量不足(BBU_LOW_CAPACITY,2) */
    SWORD32 sdwBbuFaultFlag;                        /* 仅当BBU状态为BBU_ABNORMAL时有效，用于标识BBU是何种异常，
                                                                              取值: DIAG_BBU_NOPRESENT   表示BBU不在位
                                                                                    BBU_COMMUNICATE_FAIL 表示BBU不能通信
                                                                                    BBU_CHARGE_FAIL      表示BBU不能充电
                                                                                    BBU_DISCHARGE_FAIL   表示BBU不能放电
                                                                                    BBU_OVER_TEMPERATURE 表示BBU温度过高 
                                                                                    BBU_INVALID          表示BBU失效 */
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

/* GE 诊断项 */
typedef struct
{
    SWORD32      sdwCtrlId;                             /* 控制器ID,暂时定义为0、1，对应的宏定义CTRL0、CTRL1*/
	BYTE         geStatus;      						/* GE端口总体状态 */
    SWORD32      sdwGeLInkStatus[4];                    /* GE端口使用,供ams单独使用 */
}_PACKED_1_  T_DiagSingleGeInfo;

typedef struct
{
    SWORD32 sdwCtrlFlg;                                /* 控制器标志 ,暂时定义为0、1、2，对应的宏定义CTRL_LOCAL、CTRL_PEER、CTRL_DUAL,   入参*/
    SWORD32 sdwGeNum;                                  /* 诊断GE 的数目，出参 */
    T_DiagSingleGeInfo tDiagSingleGeInfo[2];           /* 诊断信息列表，当 sdwCtrlFlg = 0/1 时,仅tDiagSingleGeInfo[0]有值,当 sdwCtrlFlg = 2 时 ,tDiagSingleGeInfo都有值*/
}_PACKED_1_ T_DiagGeInfo;

/* PCIE诊断 */
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


/* EEPROM诊断项  */
typedef struct
{
    SWORD32    sdwCtrlId;                             /* 控制器ID,暂时定义为0、1，对应的宏定义CTRL0、CTRL1*/
    SWORD32    sdwEepromDeviceStatus;                 /* Eeprom设备状态,正常为DEVICE_STATUS_OK，异常为DEVICE_STATUS_FALSE*/
}_PACKED_1_ T_DiagSingleEepromInfo;

typedef struct
{
    SWORD32 sdwCtrlFlg;                               /* 控制器标志 ,暂时定义为0、1、2，对应的宏定义CTRL_LOCAL、CTRL_PEER、CTRL_DUAL,   入参*/
    SWORD32 sdwEepromNum;                             /* 诊断Eeprom的数目，出参 */
    T_DiagSingleEepromInfo tDiagSingleEepromInfo[2];  /* 诊断信息列表，当 sdwCtrlFlg = 0/1 时,仅tDiagSingleEepromInfo[0]有值,当 sdwCtrlFlg = 2 时 ,tDiagSingleEepromInfo都有值*/
}_PACKED_1_ T_DiagEepromInfo;



/* 锁相环诊断项 */
typedef struct
{
    SWORD32  sdwCtrlId;                               /* 控制器ID,暂时定义为0、1，对应的宏定义CTRL0、CTRL1*/
    SWORD32  sdwClockDeviceStatus;                    /* 锁相环设备状态,正常为DEVICE_STATUS_OK，异常为DEVICE_STATUS_FALSE*/
}_PACKED_1_  T_DiagSingleClockInfo;

typedef struct
{
    SWORD32 sdwCtrlFlg;                                 /* 控制器标志 ,暂时定义为0、1、2，对应的宏定义CTRL_LOCAL、CTRL_PEER、CTRL_DUAL,   入参*/
    SWORD32 sdwClockNum;                                /* 诊断锁相环的数目，出参 */
    T_DiagSingleClockInfo tDiagSingleClockInfo[2];      /* 诊断信息列表，当 sdwCtrlFlg = 0/1 时,仅tDiagSingleClockInfo[0]有值,当 sdwCtrlFlg = 2 时 ,tDiagSingleClockInfo都有值*/
}_PACKED_1_ T_DiagClockInfo;

/* FC诊断项 */

typedef struct
{
    SWORD32       sdwCtrlId;
	SWORD32 	  sdwFcCardOnline;                      /* FC子卡是否在位, DIAG_FC_PRESENT代表在位, DIAG_FC_ABSENT代表不在 */
	BYTE 	   sdwFcPortStatus[4];
    BYTE 	   sdwFcLinkStatus[4];                      /* FC设备状态,正常为DEVICE_STATUS_OK，异常为DEVICE_STATUS_FALSE*/
}T_DiagSingleFcInfo;

typedef struct
{
    SWORD32 sdwCtrlFlg;                                 /* 控制器标志 ,暂时定义为0、1、2，对应的宏定义CTRL_LOCAL、CTRL_PEER、CTRL_DUAL,   入参*/
    SWORD32 sdwFcNum;                                   /* 诊断FC的数目，出参 */
    T_DiagSingleFcInfo  tDiagSingleFcInfo[2];           /* 诊断信息列表，当 sdwCtrlFlg = 0/1 时,仅tDiagSingleFcInfo[0]有值,当 sdwCtrlFlg = 2 时 ,tDiagSingleFcInfo都有值*/
}_PACKED_1_ T_DiagFcInfo;


/* IBMC诊断项 */
typedef struct
{
	SWORD32  sdwCtrlId;
	BYTE     sdwIbmcStatus;							    /* IBMC设备状态,正常为DEVICE_STATUS_OK，异常为DEVICE_STATUS_FALSE*/
}T_DiagSingleIbmcInfo;

typedef struct
{
	SWORD32 sdwCtrlFlg; 								/* 控制器标志 ,暂时定义为0、1、2，对应的宏定义CTRL_LOCAL、CTRL_PEER、CTRL_DUAL,   入参*/
	SWORD32 sdwIbmcNum;									/* 诊断IBMC的数目，出参 */
	T_DiagSingleIbmcInfo	tDiagSingleIbmcInfo[2];		/* 诊断信息列表，当 sdwCtrlFlg = 0/1 时,仅tDiagSingleIbmcInfo[0]有值,当 sdwCtrlFlg = 2 时 ,tDiagSingleIbmcInfo都有值*/
}_PACKED_1_ T_DiagIbmcInfo;



typedef struct
{
    CHAR    cDiagDeviceName[MAX_DIAG_DEVICE_NAME_LEN];    /* 诊断对象名称，   出参  */
    CHAR    cDiagProperty[MAX_PROP_NAME_LEN];             /* 诊断对象属性，   出参  */
    CHAR    cDiagPropertyValue[MAX_PROP_VALUE_LEN];       /* 诊断对象属性值， 出参  */
}_PACKED_1_ T_DiagDeviceProperty;

/* ECC开关设置 */
typedef struct
{
    SWORD32     sdwEccSwitch;                           /* 入参: ECC检测打开开关: DIAG_ECC_SWITCH_OPEN 打开开关; DIAG_ECC_SWITCH_CLOSE关闭开关 */
}_PACKED_1_  T_DiagSingleEccSwitch;


typedef struct
{
    SWORD32 sdwCtrlFlg;                                 /* 入参: 控制器标志 ,暂时定义为0、1、2，对应的宏定义CTRL_LOCAL、CTRL_PEER、CTRL_DUAL,   入参*/
    SWORD32 sdwEccSwitchNum;                            /* 出参: 需要设置ECC开关的个数，出参 */
    T_DiagSingleEccSwitch  tDiagSingleEccSwitch[2];     /* 入参: 诊断信息列表，当 sdwCtrlFlg = 0/1 时,仅tDiagSingleEccSwitch[0]有值,当 sdwCtrlFlg = 2 时 ,tDiagSingleEccSwitch都有值*/
}_PACKED_1_ T_DiagEccSwitch;


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

/* *************************Expander WUI Diag**************************** */
/*风扇信息*/
typedef struct
{
    SWORD32 sdwLevel;       /* 转速级别, 1-13级, 转速越快级别越高 */
    WORD16  sdwSpeed;       /* 转速, RPM, Round Per Minute */
    BYTE    bFanStatus;     /* 风扇状态,取值:DEVICE_STATUS_OK(值为0,风扇正常),DEVICE_STATUS_FALSE(值为1,风扇异常) */
}_PACKED_1_ T_FanInfo;

/*电源信息*/
typedef struct
{
	SWORD32 sdwVoltInCurrentValue;	   /* 输入电压当前值 */
	SWORD32 sdwVoltInMaxValue;	       /* 输入电压上限值 */
	SWORD32 sdwVoltInMinValue;	       /* 输入电压下限值 */
	SWORD32 sdwVoltInStatus;           /* 电源输入电压状态, 正常为DEVICE_STATUS_OK, 异常为DEVICE_STATUS_FALSE */

	SWORD32 sdwVoltOutHighCurrentValue;/* 输出高电压当前值 */
	SWORD32 sdwVoltOutHighMaxValue;	   /* 输出高电压上限值 */
	SWORD32 sdwVoltOutHighMinValue;	   /* 输出高电压下限值 */
	SWORD32 sdwVoltOutHighStatus;      /* 电源输出高电压状态, 正常为DEVICE_STATUS_OK, 异常为DEVICE_STATUS_FALSE */

	SWORD32 sdwVoltOutLowCurrentValue; /* 输出低电压当前值 */
	SWORD32 sdwVoltOutLowMaxValue;	   /* 输出低电压上限值 */
	SWORD32 sdwVoltOutLowMinValue;	   /* 输出低电压下限值 */
    SWORD32 sdwVoltOutLowStatus;       /* 电源输出低电压状态, 正常为DEVICE_STATUS_OK, 异常为DEVICE_STATUS_FALSE */

    SWORD32 sdwPowerTempValue;         /* 电源温度实际值, Centigrade*/
	SWORD32 sdwPowerTempMaxValue;      /* 电源温度上限值 */
	SWORD32 sdwPowerTempMinValue;	   /* 电源温度下限值 */
    SWORD32 sdwPowerTempStatus;        /* 电源温度状态, 正常为DEVICE_STATUS_OK, 异常为DEVICE_STATUS_FALSE */

    SWORD32 sdwPresentStatus;          /* 电源在位状态，正常为DEVICE_STATUS_OK, 异常为DEVICE_STATUS_FALSE */
}_PACKED_1_ T_PowerInfo;

/*单板温度信息*/
typedef struct
{
    SWORD32 sdwBoardTempValue;         /* 单板温度实际值, Centigrade */
	SWORD32 sdwBoardMaxTempValue;      /* 单板温度上限,即超过该温度应该告警 */
	SWORD32 sdwBoardMinTempValue;      /* 单板温度下限,即低于该温度应该告警 */
    SWORD32 sdwBoardTempStatus;        /* 单板温度状态, 正常为DEVICE_STATUS_OK, 异常为DEVICE_STATUS_FALSE */
	SWORD32 sdwCtrId;                  /* 单板槽位号,值为0 或者1 */
}_PACKED_1_ T_BoardInfo;

/*CPU温度信息*/
typedef struct
{
    SWORD32 sdwCpuTempValue;           /* CPU温度实际值, Centigrade   */
	SWORD32 sdwCpuMaxTempValue;        /* CPU温度上限,即超过该温度应该告警 */
	SWORD32 sdwCpuMinTempValue;        /* CPU温度下限,即低于该温度应该告警 */
    SWORD32 sdwCpuTempStatus;          /* CPU温度状态, 正常为DEVICE_STATUS_OK, 异常为DEVICE_STATUS_FALSE */
	SWORD32 sdwCtrId;                  /* 单板槽位号,值为0 或者1 */
}_PACKED_1_ T_CpuInfo;

/*箱体设备信息*/
typedef struct
{
    BYTE sdwChassisId;                 /*箱体ID*/
    BYTE sdwBoardNum;                  /*控制器单板数目*/
    T_FanInfo    tFanInfo[4];          /*风扇信息, 槽位0对应tFanInfo[0]和tFanInfo[1], 槽位1对应tFanInfo[2]和tFanInfo[3]*/
    T_PowerInfo  tPowerInfo[2];        /*电源信息, 槽位0对应tPowerInfo[0], 槽位1对应tPowerInfo[1]*/
    T_BoardInfo  tBoardInfo[2];        /*单板信息, 槽位0对应tBoardInfo[0], 单控环境下只有tBoardInfo[0]有值*/
    T_CpuInfo    tCpuInfo[2];          /*CPU信息, 槽位0对应tCpuInfo[0], 单控环境下只有tCpuInfo[0]有值*/
}_PACKED_1_ T_DiagExpanderInfo;

/*所有箱体设备信息列表*/
typedef struct
{
    BYTE                sdwExpanderNum;                     /*箱体个数, 出参*/
    BYTE                sdwQueryFlag;                       /*查询箱体标志, DIAG_ALL_EXPANDER查询所有箱体, DIAG_SINGLE_EXPANDER表示查询单个箱体, 入参*/
    BYTE                sdwSingleChassisId;                 /*需查询的单个箱体信息的ID号, 入参*/
    T_DiagExpanderInfo  tExpanderInfo[MAX_UP_SUB_ENCL];     /*所有箱体信息列表, 入参*/
}_PACKED_1_ T_ExpanderInfoList;

/* *************************Board WUI Diag**************************** */
/*整板状态信息*/
typedef struct
{
    SWORD32 sdwCtrlId;                                     /* 控制器ID, 暂时定义为0、1, 对应的宏定义CTRL0、CTRL1*/
    SWORD32 sdwBoardStatus;                                /* 整板状态, 正常为DEVICE_STATUS_OK, 异常为DEVICE_STATUS_FALSE*/
}_PACKED_1_ T_DiagSingleBoardStatus;

typedef struct
{
    SWORD32 sdwCtrlFlg;                                    /* 控制器标志 ,暂时定义为0、1、2, 对应的宏定义CTRL_LOCAL、CTRL_PEER、CTRL_DUAL, 入参*/
    SWORD32 sdwBoardNum;                                   /* 诊断整板的数目, 出参 */
    T_DiagSingleBoardStatus tDiagSingleBoardStatus[2];     /* 诊断信息列表, 当 sdwCtrlFlg = 0/1 时, tDiagSingleBoardStatus[0]有值,当 sdwCtrlFlg = 2 时, tDiagSingleBoardStatus都有值*/
}_PACKED_1_ T_DiagBoardStatus;

/* *************************Hdd WUI Diag**************************** */
/*磁盘状态信息*/
typedef struct
{
    SWORD32 sdwSlotId;                                     /* 磁盘槽位号，取值0~27(ScanBox);  0~11(SPR10) */
    SWORD32 sdwEnclosureId;                                /* 箱体Id, 取值 0~实际箱体数*/
    SWORD32 sdwHddStatus;                                  /* 磁盘状态, 正常为DEVICE_STATUS_OK, 异常为DEVICE_STATUS_FALSE*/
}_PACKED_1_ T_DiagSingleHddStatus;

typedef struct
{
    SWORD32 sdwHddNum;                                               /* 磁盘个数，取值MAX_ENC_NUM_IN_SYS(4) * MAX_PD_NUM_IN_ENC(16) */
    T_DiagSingleHddStatus tDiagSingleHddStatus[MAX_PD_NUM_IN_SYS];   /* 磁盘诊断信息列表 */
}_PACKED_1_ T_DiagHddStatus;

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
    SWORD32 sdwLevel;       /* 转速级别 */
    WORD16 swSpeed;         /* 转速 */
    BYTE    bFanStatus;     /* 风扇状态,取值:DEVICE_STATUS_OK(值为0,风扇正常),DEVICE_STATUS_FALSE(值为1,风扇异常) */
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
    BYTE ucPeerStatus;/*对板在位标志*/
    BYTE ucSubId[SUBID_LEN]; /*扩展柜ID*/
    BYTE ucSgName[MAX_DISKNAMELEN];  /*SG设备名*/
    WORD64   uqwSasAddr;  /*EXPsaS地址*/

    T_EncFanInfo tFaninfo[MAX_FAN_NUM];  /*风扇信息*/
    T_EncPsInfo tPsinfo[MAX_PS_NUM];  /*电源信息*/
    T_EncVoltInfo tVoltinfo[MAX_VOLT_NUM];  /*电压信息*/
    T_EncTempratureInfo tTempratureinfo[MAX_TEMP_NUM];  /*温度信息*/

}_PACKED_1_ T_EncDeviceInfo;
/*箱体设备信息列表*/
typedef struct
{
    BYTE            ucExpcount;                   /*箱体个数*/
    SWORD32         sdwCtrlId;                    /* 控制器ID,暂时定义为0、1，对应的宏定义CTRL0、CTRL1*/
    T_EncDeviceInfo tExpanders[MAX_UP_SUB_ENCL];  /*箱体链表头*/
}_PACKED_1_  T_EncDeviceInfoList;
/*所有箱体设备信息列表*/
typedef struct
{
    SWORD32 sdwCtrlFlg;                                 /* 控制器标志 ,暂时定义为0、1、2，对应的宏定义CTRL_LOCAL、CTRL_PEER、CTRL_DUAL,   入参*/
    SWORD32 sdwCtrlNum;                                 /* 控制器的数目，出参 */
    T_EncDeviceInfoList tEncInfoList[2];                /* 诊断信息列表，当 sdwCtrlFlg = 0/1 时,仅tEncInfoList[0]有值,当 sdwCtrlFlg = 2 时 ,tEncInfoList[2]都有值*/
}_PACKED_1_ T_AllEncInfoList;

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
    SWORD32     sdwCtrlFlag;                         /* 查询的控制器本对端标记 */
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
    BYTE    ucAlarmMailEnableFlag;
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
    SWORD32      sdwSessionId;  //用户登录的sessionid结构
    SWORD32      sdwExistFlag;  //0标识sid存在，1标识sid不存在
}_PACKED_1_ T_WebSessionIdExist;

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
    WORD32 sdwBroadType;          /* 单板类型，取值BROAD_UNKNOWN / BROAD_SBCJ / BROAD_SPR10 / BROAD_SPR11 / BROAD_PC */
}_PACKED_1_ T_SysBroadType;


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

/******************************* 获取和设置预设事件的cache自动回写策略 ************************************/
typedef struct
{
    /* Cache自动直写策略开关，ON表示预定，OFF表示关，RST表示保留原值 */
    BYTE bFanFaultEvt;         /* 风扇Cache自动直写策略开关 */
    BYTE bPowerFaultEvt;       /* 电源Cache自动直写策略开关 */
    BYTE bBbuFaultEvt;         /* BBU的Cache自动直写策略开关 */
    BYTE bUpsConnLossEvt;      /* UPS连接丢失Cache自动直写策略开关 */
    BYTE bUpsBattFaultEvt;     /* UPS电池失效Cache自动直写策略开关 */
    BYTE bUpsOverloadEvt;      /* UPS过载Cache自动直写策略开关 */
    BYTE bOher;                /* 其他Cache自动直写策略开关 */
}_PACKED_1_ T_AutoWrThrPreSet;

/******************************* 预设事件修改Cache策略设备状态 ************************************/
typedef struct
{
    BYTE    bPowerStatus;          /* 电源状态*/
    BYTE    bFanStatus;            /* 风扇状态*/
    BYTE    bBbuStatus;            /* BBU状态*/
}_PACKED_1_  T_PreEvtDeviceStatus;

/* SNMP 配置和TRAP配置 */
typedef struct
{
	SWORD32 sdwServerId; //Server ID.
    SWORD32 sdwAction; //SET - enable SNMP server, unable SNMP server setting.
	CHAR    scServerIp[MAX_IP_LEN]; //服务器主机名或ip地址
	SWORD32 sdwServerPort; //服务器端口,如果为INVALID,将使用默认端口号
	SWORD32 sdwSnmpVersion; //SNMP版本
	CHAR    scReadCommName[MAX_SNMP_COMMUNITY_NAME_LEN]; //读团队名称
}_PACKED_1_ T_SnmpTrapConfig;
typedef struct
{
    WORD32          udwServerCount;  /* 服务器个数*/
    T_SnmpTrapConfig  atSnmpRcv[MAX_SNMP_NOTIFY_NUM];
}_PACKED_1_ T_TrapCfgList;

typedef struct
{
	SWORD32 sdwServerId; //Server ID.
	SWORD32 sdwSnmpVersion; //SNMP版本
	CHAR    scServerIp[MAX_IP_LEN]; //服务器主机名或ip地址
	CHAR    scReadCommName[MAX_SNMP_COMMUNITY_NAME_LEN]; //读团队名称
}_PACKED_1_ T_TrapCfgMody;
/********************************MTS测试仪相关的结构体说明******************************************/
/********************************除特殊说明外, 结构体成员都为出参***********************************/

/****************************************单板公共项测试*********************************************/
/* 工作EPLD自检 */
typedef struct
{
    WORD32 dwStandardValue;   /* 标准值  */
	WORD32 dwReadValue;       /* 读出值  */
	WORD32 dwCheckResult;     /* 判断值  */
	WORD32 dwTimeoutMax;      /* 超时最大值 */
}_PACKED_1_ T_Mts_Check_Work_Epld_Self;

/* 工作EPLD信息读取 */
typedef struct
{
    WORD32 dwBoardId;         /* 单板ID        */
	WORD32 dwPcbVer;          /* 单板pcb版本号 */
	WORD32 dwBomVer;          /* 单板bom料单   */
	WORD32 dwTimeoutMax;      /* 超时最大值 */
}_PACKED_1_ T_Mts_Check_Work_Epld_Info;

/* 启动EPLD信息读取 */
typedef struct
{
    WORD32 dwBoardId;         /* 单板ID        */
	WORD32 dwPcbVer;          /* 单板pcb版本号 */
	WORD32 dwBomVer;          /* 单板bom料单   */
	WORD32 dwTimeoutMax;      /* 超时最大值 */
}_PACKED_1_ T_Mts_Check_Boot_Epld_Info;

/* 电子条形码扫描 */
typedef struct
{
    BYTE   cLableInfo[MAX_INFO_LABLE_LENGTH]; /* 条形码 */
	WORD32 dwTimeoutMax;                      /* 超时最大值 */
}_PACKED_1_ T_Mts_Check_Lable_Info;

/* 单板硬件地址读取 */
typedef struct
{
    WORD32   dwBackBoardId;    /* 6位测试背板ID */
	BYTE     cSlotId;          /* 槽位号 */
	WORD32   dwTimeoutMax;      /* 超时最大值 */
}_PACKED_1_ T_Mts_Check_Phy_Addr_Info;

/* 在位信号测试 */
typedef struct
{
	BYTE     cOnlieFlag;        /* 在位标志 */
	WORD32   dwTimeoutMax;      /* 超时最大值 */
}_PACKED_1_ T_Mts_Check_Online_Flag;

/* CPU外挂DDR3测试 */
typedef struct
{
	WORD16  wMtsType;      /* 入参，测试类型:MTS_FT,  功能测试:  MTS_HT,       高温       */
	BYTE    cCheckResult;  /* 出参，测试结果:MTS_DDR_GOOD 测试通过;  MTS_DDR_NOT_GOOD  测试不通过 */
	WORD32  dwTimeoutMax;  /* 超时最大值 */
}_PACKED_1_ T_Mts_Check_Ddr;

/* NVRAM芯片测试 */
typedef struct
{
   WORD16  wMtsType;       /* 入参，测试类型:MTS_FT,  功能测试: MTS_HT,      高温*/
   BYTE    cCheckResult;   /* 出参：测试结果:MTS_NVRAM_GOOD 测试通过; MTS_NVRAM_NOT_GOOD 测试不通过 */
   WORD32  dwTimeoutMax;   /* 超时最大值 */
}_PACKED_1_ T_Mts_Check_Nvram;

/* 版本FLASH测试 */
typedef struct
{
	WORD16  wMtsType;     /* 入参，测试类型:MTS_FT,  功能测试:
	                                        MTS_HT,  高温测试
                                            MTS_MO_COMPA   版本比较
	                      */
	BYTE    cCheckResult; /* 出参：测试结果 */
	WORD32  dwTimeoutMax; /* 超时最大值 */
}_PACKED_1_ T_Mts_Check_Verflash;

/* 板内时钟电路测试 */
typedef struct
{
   WORD16  wMtsClockType;  /* 入参：MTS_25M_CLOCK, MTS_33_CLOCK...*/
   BYTE    cCheckResult;   /* 出参：MTS_CLOCK_GOOD 	    测试通过
                                   MTS_CLOCK_NOT_GOOD	测试不通过*/
   WORD32  dwTimeoutMax;   /* 超时最大值 */
}_PACKED_1_ T_Mts_Check_Clock;

/* 面板指示灯测试 */
typedef struct
{
    WORD32  dwLedId;      /* < 入参:灯id，统一编号            */
    WORD32  dwLedColor;   /* < 入参:LED颜色，对单色灯取默认值 */
    WORD32  dwFlashType;  /* < 入参:设置当前闪灯方式          */
    WORD32  dwTimeoutMax; /* 超时最大值 */
}_PACKED_1_ T_Mts_Check_Led;

/* 看门狗复位测试 */
typedef struct
{
    WORD32  dwMtsType;    /* 入参，使能看门狗:MTS_ENABLE_WTG,  禁用看门狗: MTS_DISABLE_WTG, */
    WORD32  dwTimeoutMax; /* 超时最大值 */
}_PACKED_1_ T_Mts_Check_Wtg;

/* 温度传感器测试 */
typedef struct
{
   /* 数组下标0代表LM_93  1代表75_1, 2代表75_2, 3代表75_2 4代表75_3 */
   BYTE    cSenorStatus[5];  /* 出参; MTS_SENOR_GOOD正常  MTS_SENOR_NOT_GOOD 不正常 */
   SWORD32 sdwSenorValue[5]; /* 出参; 传感器的实际温度 */
   BYTE    cEncDevId;          /* 扩展柜号 */
   WORD32  dwTimeoutMax;     /* 超时最大值 */
}_PACKED_1_ T_Mts_Check_Senor;

/* RTC实时时钟电路测试 */
typedef struct
{
   BYTE    cCheckResult;   /* 出参：MTS_RTC_GOOD 	    测试通过
                                    MTS_RTC_NOT_GOOD	测试不通过 */
   WORD32  dwTimeoutMax;   /* 超时最大值 */
}_PACKED_1_ T_Mts_Check_Rtc;

/* 面板GE口环回测试 */
typedef struct
{
   WORD16  wMtsType;       /* 入参，测试类型:MTS_FT,  功能测试: MTS_HT,      高温*/
   BYTE    cCheckResult[4];   /* 出参：测试结果:MTS_GE_GOOD 测试通过; MTS_GE_NOT_GOOD 测试不通过 */
   WORD32  dwTimeoutMax;   /* 超时最大值 */
}_PACKED_1_ T_Mts_Check_Ge;

/* 面板调试网口作为通信链路测试 */
typedef struct
{
   BYTE    cCheckResult;   /* 出参：MTS_LINK_GOOD 	    测试通过
                                    MTS_LINK_NOT_GOOD	测试不通过 */
   WORD32  dwTimeoutMax;   /* 超时最大值 */
}_PACKED_1_ T_Mts_Check_Conn_Link;

/************************************单板特定项测试************************************/
/* Cache同步信号测试 */
typedef struct
{
    WORD32     dwPciePexId;          /* PEX8624厂家ID */
	BYTE       cCheckResult;		 /* PEX8624自检结果 */
    WORD32     dwPcieLinkSpeed;      /* PCIE 连接速率 G1/G2*/
    WORD32     dwPcieLinkWidth;      /* PCIE 连接带宽x1~x4 x0表示无link */
    WORD32     dwPcieBadTlpCount;    /* TLP层坏包个数 */
    WORD32     dwPcieBadDllpCount;   /* DLLP层坏包个数 */
    WORD32     dwPcieEepromStatus;   /* PCIE 设备E2prom在位与否、有效与否 */
    WORD32     dwPcieDllpStatus;     /* DLLP 状态 */
    BYTE       cPcieDeviceStatus;    /* PCIE设备状态 */
    WORD32     dwTimeoutMax;         /* 超时最大值 */
}_PACKED_1_  T_Mts_Check_Pcie_Info;

/* PHY芯片自检 */
typedef struct
{
	WORD16  wPhyDevId;  		/*  入参：宏定义MTS_PHY_88E1111和MTS_PHY_88E1119 */
    WORD32  dwPhyId;  			/* 出参：厂家ID */
	BYTE    cCheckResult;       /* 出参：测试结果:MTS_PHYSELF_GOOD 测试通过; MTS_PHYSELF_NOTGOOD 测试不通过 */
    WORD32  dwTimeoutMax;       /* 超时最大值 */
} _PACKED_1_ T_Mts_Check_Phyself;

/* PCIE芯片桥自检 */
//说明:同cache同步信号测试

/* SAS控制器PM8001自检 */
typedef struct
{
	WORD32	dwPmId; 	    /* 出参: PM8001厂家ID */
	BYTE	cCheckResult;	/* 出参: 检测结果 */
	WORD32	dwTimeoutMax;	/* 超时最大值 */
}_PACKED_1_ T_Mts_Check_Pm;

/************************************IO信号类************************************/
/* 硬盘指示灯测试 */
//暂无数据结构

/* 输入IO信号指示灯测试 */
typedef struct
{
    WORD32  dwInputIoSigType;	/* 输入IO信号类型测试  */
	BYTE    cCheckResult;		/* 状态返回值       */
    WORD32  dwTimeoutMax;       /* 超时最大值 */
}_PACKED_1_ T_Mts_Check_InputIo;


/* 输出IO信号指示灯测试 */
typedef struct
{
    WORD32  dwOutputIoSigType;	/* 输出IO信号类型测试  */
	BYTE    cInputValue;        /* 期望输出的信号0或1,入参 */
	BYTE    cCheckResult;		/* 状态返回值       */
    WORD32  dwTimeoutMax;       /* 超时最大值 */
}_PACKED_1_ T_Mts_Check_OutputIo;


/* 电源信号指示灯测试 */
typedef struct
{
	BYTE	cCheckResult;		/* 状态返回值 */
	WORD32	dwTimeoutMax;		/* 超时最大值 */
}_PACKED_1_ T_Mts_Check_Power_Light;

/* BBU信息读取 */
typedef struct
{
    WORD32  dwBbuId;	    /* BBU电池序列号     */
	WORD16  wBbuMv;			/* BBU当前电压       */
	WORD16  wBbuCapacity;	/* BBU剩余电量百分比 */
	BYTE    ucPresent;   	/* BBU在位信息  MTS_BBU_PRESENT;  在位	        MTS_BBU_NOTPRESENT:  不在位 */
	BYTE    ucWorkFlag;    	/* 是否正常工作	MTS_BBU_CHARGING: BBU正在充放电 MTS_BBU_NOT_CHARGING:BBU此时没有充放电 */
	BYTE    cCheckResult;   /* BBU状态 MTS_BBU_GOOD 正常 MTS_BBU_GOOD不正常 */
    WORD32  dwTimeoutMax;   /* 超时最大值 */
}_PACKED_1_ T_Mts_Check_Bbu;

/* 8005看门狗复位测试 */
typedef struct
{
	BYTE	cCheckResult;		/* 测试返回值 */
    BYTE    cEncDevId;            /* 扩展柜号 */
	WORD32	dwTimeoutMax;		/* 超时最大值 */
}_PACKED_1_ T_Mts_Check_Ezzf_Reset;

/* minisas接口自环测试 */
typedef struct
{
	BYTE	cCheckResult[4];    /* 测试速率返回值 */
    BYTE    cEncDevId;            /* 扩展柜号 */
	WORD32	dwTimeoutMax;		/* 超时最大值 */
}_PACKED_1_ T_Mts_Check_Ezzf_GetSpeed;


/* 背板SAS接口自环测试 */
typedef struct
{
	BYTE	cCheckResult[12];	/* 测试速率返回值 */
    BYTE    cEncDevId;            /* 扩展柜号 */
	WORD32	dwTimeoutMax;		/* 超时最大值 */
}_PACKED_1_ T_Mts_Check_Ezzf_BdGetSpeed;


/* TW0\TW1\TW2信号测试 */
typedef struct
{
	BYTE    cTwSigValue[6];
    BYTE    cEncDevId;            /* 扩展柜号 */
	WORD32	dwTimeoutMax;		/* 超时最大值 */
}_PACKED_1_ T_Mts_Check_Ezzf_Ttsig;


/* SAS扩展芯片PM8005自检 */
typedef struct
{
	BYTE	cSasAddr[8];		/* SAS地址 */
    BYTE    cEncDevId;            /* 扩展柜号 */
	WORD32	dwTimeoutMax;		/* 超时最大值 */
}_PACKED_1_ T_Mts_Check_Ezzf_Self;


/* 硬盘指示灯测试 */
typedef struct
{
	BYTE	cMtsType;		    /* 点灯or灭灯 */
	WORD32  dwHddLightType;     /* 点灯类型 */
    WORD32  dwHddIndexId;       /* 索引号 1~12*/
	WORD32	dwTimeoutMax;		/* 超时最大值 */
}_PACKED_1_ T_Mts_Check_Hdd_Light;

/* 内存大小测试 */
typedef struct
{
	WORD32  dwDdrSize;          /* 内存大小,单位:M */
	BYTE    cCoherenceResult;   /* 数据一致性测试 */
	BYTE    cCorrectResult;     /* 数据正确性测试 */
	WORD32	dwTimeoutMax;		/* 超时最大值 */
}_PACKED_1_ T_Mts_Check_Ddr_Size;

/* 版本查询 */
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

/* MAC地址 */
typedef  struct
{
    BYTE    cMacAddr[128];
	WORD32	dwTimeoutMax;
}_PACKED_1_ T_Mts_Check_MacAddr;
/************************************测试仪End******************************************/

/***********************************整机测试Begin***************************************/
/* 工作EPLD自检 */
typedef  struct
{
	BYTE    ucChassisId;   					/*箱体ID*/
	WORD32  dwStandardValue;          		/* Output parameter: standard value */
	WORD32  dwReadValue;              		/* Output parameter: Read-Out value */
	WORD32  dwCheckResult;            		/* Output parameter: CheckReust */
}_PACKED_1_ T_Cmt_Check_WorkEpldSelf;


typedef  struct
{
	//BYTE 	ucSlotId;	   			/*槽位ID*/
    //BYTE 	ucPeerStatus;			/*对版在位标志*/
	T_Cmt_Check_WorkEpldSelf    tCmtCheckWorkEpldSelf[2];
}_PACKED_1_ T_CmtWorkEpldSelf;



typedef  struct
{
	WORD32	dwTimeoutMax;		/* Input  parameter: Max-Timeout value */
	WORD32  dwEncNum;           /* 级联层次,最小为1,最大为5 */
	T_CmtWorkEpldSelf       tCheckWorkEpldSelf[CMT_CLASS_LEVEL];
}_PACKED_1_ T_WorkEpldSelfList;


/* 工作EPLD信息读取 */
typedef  struct
{
	BYTE   ucChassisId;   	  /* 箱体ID */
    WORD32 dwBoardId;         /* 单板ID        */
	WORD32 dwPcbVer;          /* 单板pcb版本号 */
	WORD32 dwBomVer;          /* 单板bom料单   */
}_PACKED_1_ T_Cmt_Check_WorkEpldInfo;

typedef  struct
{
	//BYTE 	ucSlotId;	   		/*槽位ID*/
    //BYTE 	ucPeerStatus;		/*对版在位标志*/
	T_Cmt_Check_WorkEpldInfo    tCmtCheckWorkEpldInfo[2];
}_PACKED_1_ T_CmtWorkEpldInfo;

typedef  struct
{
	WORD32	dwTimeoutMax;      /* Input  parameter: Max-Timeout value */
	WORD32  dwEncNum;          /* 级联层次,最小为1,最大为5 */
	T_CmtWorkEpldInfo          tCheckWorkEpldInfo[CMT_CLASS_LEVEL];
}_PACKED_1_ T_WorkEpldInfoList;


/* 工作EPLD信息读取 */
typedef  struct
{
	BYTE   ucChassisId;   	  /* 箱体ID */
    	WORD32  dwStandardValue;          		/* Output parameter: standard value */
	WORD32  dwReadValue;              		/* Output parameter: Read-Out value */
	WORD32  dwCheckResult;            		/* Output parameter: CheckReust */
       BYTE p33M;
       BYTE p75M;
       WORD32 dwBoardId;         /* 单板ID        */
	WORD32 dwPcbVer;          /* 单板pcb版本号 */
	WORD32 dwBomVer;          /* 单板bom料单   */
	WORD32   dwBackBoardId;    /* 6位测试背板ID */

}_PACKED_1_ T_SPR11_Cmt_Check_WorkEpldInfo;

typedef  struct
{
    T_SPR11_Cmt_Check_WorkEpldInfo    tCmtCheckWorkEpldInfo[2];
}_PACKED_1_ T_SPR11CmtWorkEpldInfo;

typedef  struct
{
	WORD32	dwTimeoutMax;      /* Input  parameter: Max-Timeout value */
	WORD32  dwEncNum;          /* 级联层次,最小为1,最大为5 */
	T_SPR11CmtWorkEpldInfo          tCheckWorkEpldInfo[CMT_CLASS_LEVEL];
}_PACKED_1_ T_SPR11WorkEpldInfoList;
/* 启动EPLD信息读取 */
typedef  struct
{
	BYTE   ucChassisId;   	  /* 箱体ID */
    WORD32 dwBoardId;         /* 单板ID        */
	WORD32 dwPcbVer;          /* 单板pcb版本号 */
	WORD32 dwBomVer;          /* 单板bom料单   */
}_PACKED_1_ T_Cmt_Check_BootEpldInfo;

typedef  struct
{
	//BYTE 	ucSlotId;	   		/*槽位ID*/
    //BYTE 	ucPeerStatus;		/*对版在位标志*/
	T_Cmt_Check_BootEpldInfo    tCmtCheckBootEpldInfo[2];
}_PACKED_1_ T_CmtBootEpldInfo;

typedef  struct
{
	WORD32	dwTimeoutMax;		/* Input  parameter: Max-Timeout value */
	WORD32  dwEncNum;           /* 级联层次,最小为1,最大为5，扩展柜中没有启动EPLD，该字段保留 */
	T_CmtBootEpldInfo     tCheckBootEpldInfo[CMT_CLASS_LEVEL];
}_PACKED_1_ T_BootEpldInfoList;


/* 电子标签信息 */
typedef  struct
{
	BYTE   ucChassisId; 					  /* 箱体号 */
	BYTE   cLableInfo[MAX_INFO_LABLE_LENGTH]; /* 条形码 */
}_PACKED_1_ T_Cmt_Check_Lable_Info;

typedef struct
{
	T_Cmt_Check_Lable_Info	    tCmtCheckLableInfo[2];
}_PACKED_1_ T_CmtLableInfo;

typedef  struct
{
	WORD32	dwTimeoutMax;		/* Input  parameter: Max-Timeout value */
	WORD32  dwEncNum;           /* 级联层次,最小为1,最大为5，扩展柜中没有启动EPLD，该字段保留 */
	T_CmtLableInfo      tCheckLableInfo[CMT_CLASS_LEVEL];
}_PACKED_1_ T_LableInfoList;

/* 单板硬件地址读取  */
typedef  struct
{
	BYTE     ucChassisId; 		   /* 箱体号 */
    WORD32   dwBackBoardId;        /* 6位测试背板ID */
	BYTE     cSlotId;              /* 槽位号 */
}_PACKED_1_ T_Cmt_Check_PhyAddr_Info;

typedef struct
{
	T_Cmt_Check_PhyAddr_Info	    tCmtCheckPhyAddrInfo[2];
}_PACKED_1_ T_CmtPhyAddrInfo;

typedef  struct
{
	WORD32	dwTimeoutMax;		/* Input  parameter: Max-Timeout value */
	WORD32  dwEncNum;           /* 级联层次,最小为1,最大为5，扩展柜中没有启动EPLD，该字段保留 */
	T_CmtPhyAddrInfo     tCheckPhyAddrInfo[CMT_CLASS_LEVEL];
}_PACKED_1_ T_PhyAddrInfoList;

/* 在位信号测试 */
typedef  struct
{
	BYTE     ucChassisId; 	    /* 箱体号 */
	BYTE     cOnlieFlag;        /* 在位标志 */
}_PACKED_1_ T_Cmt_Check_OnlineFlag_Info;

typedef struct
{
	T_Cmt_Check_OnlineFlag_Info	    tCmtCheckOnlieFlagInfo[2];
}_PACKED_1_ T_CmtOnlineFlagInfo;

typedef  struct
{
	WORD32	dwTimeoutMax;		/* Input  parameter: Max-Timeout value */
	WORD32  dwEncNum;           /* 级联层次,最小为1,最大为5，扩展柜中没有启动EPLD，该字段保留 */
	T_CmtOnlineFlagInfo    tCheckOnlieFlagInfo[CMT_CLASS_LEVEL];
}_PACKED_1_ T_OnlineFlagInfoList;

/* 板内时钟电路测试 */
typedef  struct
{
	BYTE    ucChassisId; 	/* 箱体号 */
	WORD16	wMtsClockType;	/* 入参：MTS_25M_CLOCK, MTS_33_CLOCK...*/
	BYTE	cCheckResult[5];	/* 出参：MTS_CLOCK_GOOD 	 测试通过
									 MTS_CLOCK_NOT_GOOD  测试不通过*/
}_PACKED_1_ T_Cmt_Check_Clock_Info;

typedef struct
{
	T_Cmt_Check_Clock_Info	    tCmtCheckClockInfo[2];
}_PACKED_1_ T_CmtClockInfo;

typedef  struct
{
	WORD32			dwTimeoutMax;		/* Input  parameter: Max-Timeout value */
	WORD32  		dwEncNum;           /* 级联层次,最小为1,最大为5，扩展柜中没有启动EPLD，该字段保留 */
	T_CmtClockInfo  tCheckClockInfo[CMT_CLASS_LEVEL];
}_PACKED_1_ T_ClockInfoList;

/* 温度传感器测试 */
typedef  struct
{
	BYTE    ucChassisId; 			/* 箱体号 */
	/* 数组下标0代表LM_93  1代表75_1, 2代表75_2, 3代表75_2 4代表75_3 */
	BYTE	cSenorStatus[5];  /* 出参; MTS_SENOR_GOOD正常  MTS_SENOR_NOT_GOOD 不正常 */

}_PACKED_1_ T_Cmt_Check_Senor_Info;

typedef struct
{
	T_Cmt_Check_Senor_Info	    tCmtCheckSenorInfo[2];
}_PACKED_1_ T_CmtSenorInfo;

typedef  struct
{
	WORD32			dwTimeoutMax;		/* Input  parameter: Max-Timeout value */
	WORD32  		dwEncNum;           /* 级联层次,最小为1,最大为5，扩展柜中没有启动EPLD，该字段保留 */
	T_CmtSenorInfo  tCheckSenorInfo[CMT_CLASS_LEVEL];
}_PACKED_1_ T_SenorInfoList;
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
    WORD64   uqwSasAddr;  /*EXPsaS地址*/
    T_EncFanInfo tFaninfo[MAX_FAN_NUM];  /*风扇信息*/
    T_EncPsInfo tPsinfo[MAX_PS_NUM];  /*电源信息*/
    T_EncVoltInfo tVoltinfo[MAX_VOLT_NUM];  /*电压信息*/
    T_EncTempratureInfo tTempratureinfo[MAX_TEMP_NUM];  /*温度信息*/

}_PACKED_1_ T_MtsEncDeviceInfo;
/*箱体设备信息列表*/
typedef struct
{
    WORD32	  dwTimeoutMax;		/* Input  parameter: Max-Timeout value */
    BYTE            ucExpcount;                   /*箱体个数*/
    T_MtsEncDeviceInfo tExpanders[CMT_CLASS_LEVEL];  /*箱体链表头*/
}_PACKED_1_  T_MtsEncDeviceInfoList;

typedef struct
{
    BYTE id;     /* 属性id */
    WORD16 flags;  /* 属性标识，可分析得到pre-fail和update方式 */
    BYTE currentvalue; /* 属性当前值 */
    BYTE worst;   /* 属性曾经出现的最坏值 */
    WORD64 raw;  /* 属性真实值 */
    BYTE thres;  /* 属性门限值 */
}_PACKED_1_ T_MtsDmPdSmartDetailInfo;

/*磁盘检测信息*/
typedef struct
{
    WORD32	  dwTimeoutMax;		/* Input  parameter: Max-Timeout value */
    WORD64   uqwSasAddr;
    WORD32 type;
      /*硬盘条码, 磁盘序列号*/
    CHAR cSerialNum[MAX_PD_SNO_LEN];

    /*RAID盘一致性检查*/

    /*硬盘SMART信息*/
    //T_HddSMARTInfo tHddSMARTInfo;
    BYTE ucIsSupportSmart;  /* 磁盘是否支持SMART */
    BYTE ucIsSmartEnable;   /* 磁盘SMART功能是否开启 */
    BYTE ucSmartStatus;     /* 磁盘的SMART状态是否OK */
    WORD32 PdSmartAttrNum;  /* 磁盘SMART属性个数 */
    T_MtsDmPdSmartDetailInfo tSmartDetailInfo[30];
    /*硬盘读速率*/
    WORD32 fReadSpeed;

    /*硬盘写速率*/
    WORD32 fWriteSpeed;

    /*硬盘固件版本*/
    CHAR cFwVer[MAX_PD_FW_VER_LEN];

    /*硬盘标准版本*/

    /*硬盘Model版本*/
    CHAR cModel[MAX_PD_MODEL_NAME_LEN];

    /*硬盘厂家信息*/
    CHAR cManufacture[64];
    CHAR  ucStdVerNum[MAX_PD_STD_VER_LEN];
    /*硬盘型号*/
    BYTE     ucPhyType[MAX_PD_TYPE_LEN];

    /*硬盘容量, 单位：块(512字节)*/
    WORD64 uCapacity;

    /*硬盘坏道数目*/
    SWORD32 sBadBlocks;
    SWORD32 scanPercentage;

    /*逻辑坏道修复测试*/
    BYTE ucBadBlockflag;
    /*硬盘在位信息, HDD_PRESENT表示在位, HDD_ABSENT表示不在位*/
    WORD32 wPresentFlag;
}_PACKED_1_  T_MtsDiskInfo;

/* RTC实时时钟电路测试 */
typedef  struct
{
	BYTE    ucChassisId; 			/* 箱体号 */
	BYTE	cCheckResult;			/* 出参：MTS_RTC_GOOD		 测试通过
									 MTS_RTC_NOT_GOOD	 测试不通过 */
}_PACKED_1_ T_Cmt_Check_Rtc_Info;


typedef struct
{
	T_Cmt_Check_Rtc_Info	    tCmtCheckRtcInfo[2];
}_PACKED_1_ T_CmtRtcInfo;

typedef  struct
{
	WORD32			dwTimeoutMax;		/* Input  parameter: Max-Timeout value */
	WORD32  		dwEncNum;           /* 级联层次,最小为1,最大为5，扩展柜中没有启动EPLD，该字段保留 */
	T_CmtRtcInfo    tCheckRtcInfo[CMT_CLASS_LEVEL];
}_PACKED_1_ T_RtcInfoList;

/* GE口链路速率，是否双控测试 */
typedef  struct
{
	BYTE    ucChassisId; 			/* 箱体号 */
	BYTE	cCheckResult[4];        /* 出参：测试结果:MTS_GE_GOOD 测试通过; MTS_GE_NOT_GOOD 测试不通过 */
}_PACKED_1_ T_Cmt_Check_Ge_Info;

typedef struct
{
	T_Cmt_Check_Ge_Info	    tCmtCheckGeInfo[2];
}_PACKED_1_ T_CmtGeInfo;

typedef  struct
{
	WORD32			dwTimeoutMax;		/* Input  parameter: Max-Timeout value */
	WORD32  		dwEncNum;           /* 级联层次,最小为1,最大为5，扩展柜中没有启动EPLD，该字段保留 */
	T_CmtGeInfo     tCheckGeInfo[CMT_CLASS_LEVEL];
}_PACKED_1_ T_GeInfoList;

/* PCIE测试 */
typedef  struct
{
	BYTE    ucChassisId; 		/* 箱体号 */
    WORD32  dwPciePexId;       	/* PEX8624厂家ID */
	BYTE    cCheckResult;		/* PEX8624自检结果 */
    WORD32  dwPcieLinkSpeed;    /* PCIE 连接速率 G1/G2*/
    WORD32  dwPcieLinkWidth;    /* PCIE 连接带宽x1~x4 x0表示无link */
    WORD32  dwPcieBadTlpCount; 	/* TLP层坏包个数 */
    WORD32  dwPcieBadDllpCount;	/* DLLP层坏包个数 */
    WORD32  dwPcieEepromStatus; /* PCIE 设备E2prom在位与否、有效与否 */
    WORD32  dwPcieDllpStatus;   /* DLLP 状态 */
    BYTE    cPcieDeviceStatus;  /* PCIE设备状态 */
}_PACKED_1_ T_Cmt_Check_Pcie_Info;

typedef struct
{
	T_Cmt_Check_Pcie_Info	    tCmtCheckPcieInfo[2];
}_PACKED_1_ T_CmtPcieInfo;

typedef  struct
{
	WORD32			dwTimeoutMax;		/* Input  parameter: Max-Timeout value */
	WORD32  		dwEncNum;           /* 级联层次,最小为1,最大为5，扩展柜中没有启动EPLD，该字段保留 */
	T_CmtPcieInfo   tCheckPcieInfo[CMT_CLASS_LEVEL];
}_PACKED_1_ T_PcieInfoList;


/* PHY自检 */
typedef  struct
{
	BYTE    ucChassisId; 		/* 箱体号 */
	//WORD16	wPhyDevId;		/*	入参：宏定义 MTS_PHY_88E1111 和 MTS_PHY_88E1119 */
	WORD32	dwPhyId;			/* 出参：厂家ID */
	BYTE	cCheckResult[2];	/* 出参：测试结果:MTS_PHYSELF_GOOD 测试通过; MTS_PHYSELF_NOTGOOD 测试不通过 */
}_PACKED_1_ T_Cmt_Check_Phy_Info;

typedef struct
{
	T_Cmt_Check_Phy_Info	    tCmtCheckPhyInfo[2];
}_PACKED_1_ T_CmtPhyInfo;

typedef  struct
{
	WORD32			dwTimeoutMax;		/* Input  parameter: Max-Timeout value */
	WORD32  		dwEncNum;           /* 级联层次,最小为1,最大为5，扩展柜中没有启动EPLD，该字段保留 */
	T_CmtPhyInfo    tCheckPhyInfo[CMT_CLASS_LEVEL];
}_PACKED_1_ T_PhyInfoList;

/* SAS控制器PM8001自检 */
typedef  struct
{
	BYTE    ucChassisId; 	/* 箱体号 */
	WORD32	dwPmId; 	    /* 出参: PM8001厂家ID */
	BYTE	cCheckResult;	/* 出参: 检测结果 */
}_PACKED_1_ T_Cmt_Check_Pm_Info;

typedef struct
{
	T_Cmt_Check_Pm_Info	    tCmtCheckPmInfo[2];
}_PACKED_1_ T_CmtPmInfo;

typedef  struct
{
	WORD32			dwTimeoutMax;		/* Input  parameter: Max-Timeout value */
	WORD32  		dwEncNum;           /* 级联层次,最小为1,最大为5，扩展柜中没有启动EPLD，该字段保留 */
	T_CmtPmInfo     tCheckPmInfo[CMT_CLASS_LEVEL];
}_PACKED_1_ T_PmInfoList;

/* BBU检测 */
typedef  struct
{
	BYTE    ucChassisId; 			/* 箱体号 */
    WORD32  dwBbuId;	          	/* BBU电池序列号     */
	WORD16  wBbuMv;					/* BBU当前电压       */
	WORD16  wBbuCapacity;			/* BBU剩余电量百分比 */
	BYTE    ucPresent;   			/* BBU在位信息  MTS_BBU_PRESENT;  在位	        MTS_BBU_NOTPRESENT:  不在位 */
	BYTE    ucWorkFlag;    	  	  	/* 是否正常工作	MTS_BBU_CHARGING: BBU正在充放电 MTS_BBU_NOT_CHARGING:BBU此时没有充放电 */
	BYTE    cCheckResult;         	/* BBU状态 MTS_BBU_GOOD 正常 MTS_BBU_GOOD不正常 */
}_PACKED_1_ T_Cmt_Check_Bbu_Info;

typedef struct
{
	T_Cmt_Check_Bbu_Info	    tCmtCheckBbuInfo[2];
}_PACKED_1_ T_CmtBbuInfo;

typedef  struct
{
	WORD32			dwTimeoutMax;		/* Input  parameter: Max-Timeout value */
	WORD32  		dwEncNum;           /* 级联层次,最小为1,最大为5，扩展柜中没有启动EPLD，该字段保留 */
	T_CmtBbuInfo    tCheckBbuInfo[CMT_CLASS_LEVEL];
}_PACKED_1_ T_BbuInfoList;


/* MINISAS和背部SAS接口测试0~3代表MINIsas速率,11~19代表背板SAS速率 */
typedef  struct
{
	BYTE    ucChassisId; 			/* 箱体号 */
	CHAR    ucDiskType[12][MAX_PD_TYPE_LEN];
   	BYTE	cCheckResult[36];		/* 硬盘SAS链路速度 */
}_PACKED_1_ T_Cmt_Check_Sas_Info;

typedef struct
{
	T_Cmt_Check_Sas_Info	    tCmtCheckSasInfo[2];
}_PACKED_1_ T_CmtSasInfo;

typedef  struct
{
	WORD32			dwTimeoutMax;		/* Input  parameter: Max-Timeout value */
	WORD32  		dwEncNum;           /* 级联层次,最小为1,最大为5，扩展柜中没有启动EPLD，该字段保留 */
	T_CmtSasInfo    tCheckSasInfo[CMT_CLASS_LEVEL];
}_PACKED_1_ T_SasInfoList;


/* HardDisk 自检 */
typedef  struct
{
	BYTE     ucChassisId; 								/* 箱体号       */
	SWORD32  sdwHddSoltId;								/* 硬盘槽位号   */
	CHAR     ucSerialNo[MAX_PD_SNO_LEN];				/* 硬盘序列号   */
	SWORD64  sdwHddCapcity;								/* 硬盘容量     */
	CHAR     ucStdVerNum[MAX_PD_STD_VER_LEN];			/* 标准版本号   */
	CHAR     ucFwVer[MAX_PD_FW_VER_LEN];				/* FirmWare版本 */
	CHAR     ucModelName[MAX_PD_MODEL_NAME_LEN];		/* Model名称    */
	SWORD32  sdwHealthState;							/* 磁盘的健康状态，包含可访问性和SMART 状态，取值为PD_GOOD(1)、PD_BROKEN(0)     */
}_PACKED_1_ T_Cmt_Check_Hdd_Info;

typedef struct
{
	T_Cmt_Check_Hdd_Info	    tCmtCheckHddInfo[16];
}_PACKED_1_ T_CmtHddInfo;

typedef  struct
{
	WORD32			dwTimeoutMax;		/* Input  parameter: Max-Timeout value */
	WORD32  		dwEncNum;           /* 级联层次,最小为1,最大为5，扩展柜中没有启动EPLD，该字段保留 */
	T_CmtHddInfo    tCheckHddInfo[CMT_CLASS_LEVEL];
}_PACKED_1_ T_HddInfoList;

/* 版本检测 */
typedef  struct
{
	BYTE    ucChassisId; 		/*  箱体号       	   */
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
	WORD32  		dwEncNum;           /* 级联层次,最小为1,最大为5，扩展柜中没有启动EPLD，该字段保留 */
	T_CmtVerInfo    tCheckVerInfo[CMT_CLASS_LEVEL];
}_PACKED_1_ T_VerInfoList;

/* 内存检测 */
typedef  struct
{
	BYTE    ucChassisId; 		/*  箱体号         */
	WORD32  dwDdrSize;          /* 内存大小,单位:M */
	BYTE    cCoherenceResult;   /* 数据一致性测试  */
	BYTE    cCorrectResult;     /* 数据正确性测试  */
}_PACKED_1_ T_Cmt_Check_Ddr_Info;

typedef struct
{
	T_Cmt_Check_Ddr_Info	    tCmtCheckDdrInfo[2];
}_PACKED_1_ T_CmtDdrInfo;

typedef  struct
{
	WORD32			dwTimeoutMax;		/* Input  parameter: Max-Timeout value */
	WORD32  		dwEncNum;           /* 级联层次,最小为1,最大为5，扩展柜中没有启动EPLD，该字段保留 */
	T_CmtDdrInfo    tCheckDdrInfo[CMT_CLASS_LEVEL];
}_PACKED_1_ T_DdrInfoList;

/* MAC地址 */
typedef  struct
{
	BYTE    ucChassisId; 	/*  箱体号  */
	BYTE    cMacAddr[128];
}_PACKED_1_ T_Cmt_Check_MacAddr_Info;

typedef struct
{
	T_Cmt_Check_MacAddr_Info	    tCmtCheckMacAddrInfo[2];
}_PACKED_1_ T_CmtMacAddrInfo;

typedef  struct
{
	WORD32			dwTimeoutMax;		/* Input  parameter: Max-Timeout value */
	WORD32  		dwEncNum;           /* 级联层次,最小为1,最大为5，扩展柜中没有启动EPLD，该字段保留 */
	T_CmtMacAddrInfo   tCheckMacAddrInfo[CMT_CLASS_LEVEL];
}_PACKED_1_ T_MacAddrInfoList;

/* 测试环境检测 */
typedef  struct
{
	BYTE    ucChassisId; 	/*  箱体号  */
	SWORD32 sdwPeerState;	/*  对端单板的状态 */
}_PACKED_1_ T_Cmt_Check_PeerState_Info;

typedef struct
{
	T_Cmt_Check_PeerState_Info	    tCmtCheckPeerStateInfo[2];
}_PACKED_1_ T_CmtPeerStateInfo;

typedef  struct
{
	WORD32			  dwTimeoutMax;		 /* Input  parameter: Max-Timeout value */
	WORD32  		  dwEncNum;          /* 级联层次,最小为1,最大为5，扩展柜中没有启动EPLD，该字段保留 */
	T_CmtPeerStateInfo   tCheckPeerStateInfo[CMT_CLASS_LEVEL];
}_PACKED_1_ T_PeerStateInfoList;

/* SAS地址测试, 8005自检 */
typedef  struct
{
	BYTE    ucChassisId; 	/*  箱体号  */
	BYTE    ucSasAddr[8];	/*  SAS地址 */
}_PACKED_1_ T_Cmt_Check_SasAddr_Info;

typedef struct
{
	T_Cmt_Check_SasAddr_Info	    tCmtCheckSasAddrInfo[2];
}_PACKED_1_ T_CmtSasAddrInfo;

typedef  struct
{
	WORD32			  dwTimeoutMax;		 /* Input  parameter: Max-Timeout value */
	WORD32  		  dwEncNum;          /* 级联层次,最小为1,最大为5，扩展柜中没有启动EPLD，该字段保留 */
	T_CmtSasAddrInfo   tCheckSasAddrInfo[CMT_CLASS_LEVEL];
}_PACKED_1_ T_SasAddrInfoList;

/***********************************整机测试End****************************************/
/***********************************spr11 测试仪****************************************/
/* CPU外挂DDR3测试 */
typedef struct
{
	WORD16  wMtsType;      /* 入参，测试类型:MTS_FT,  功能测试:  MTS_HT,       高温       */
	WORD32     CheckResult;  /* 出参，测试结果:MTS_DDR_GOOD 测试通过;  MTS_DDR_NOT_GOOD  测试不通过 */

	WORD32  dwTimeoutMax;  /* 超时最大值 */
}_PACKED_1_ T_Mts_SPR11_Check_Ddr;
/* 内存大小测试 */
typedef struct
{
	WORD32  dwDdrSize;          /* 内存大小,单位:M */
	BYTE    cCoherenceResult;   /* 数据一致性测试 */
	BYTE    cCorrectResult;     /* 数据正确性测试 */
	WORD32     cMemResult;     /* 内存容量校验*/
       WORD32     cCpuResult;     /* CPU多核功能检测*/
	WORD32	dwTimeoutMax;		/* 超时最大值 */
}_PACKED_1_ T_Mts_SPR11_Check_Ddr_Size;

typedef struct
{
	WORD32  dwDdrSize[2];          /* 内存大小,单位:M */
	WORD32	dwTimeoutMax;		/* 超时最大值 */
}_PACKED_1_ T_Cmt_SPR11_Check_Ddr_Size;

typedef struct
{
	WORD32	dwTimeoutMax;		/* 超时最大值 */
	WORD32     CheckResult;
}_PACKED_1_ T_Cmt_SPR11_Check_Hb;

/* 工作EPLD自检 */
typedef struct
{
    WORD32 dwStandardValue;   /* 标准值  */
	WORD32 dwReadValue;       /* 读出值  */
	WORD32 dwCheckResult;     /* 判断值  */
	WORD32 dwTimeoutMax;      /* 超时最大值 */
}_PACKED_1_ T_Mts_SPR11_Check_Work_Epld_Self;

/* MAC地址 */
typedef  struct
{
    BYTE    cMacAddr[6];
}_PACKED_1_ T_Mts_SPR11_MacAddr;

/* MAC地址 */
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
/* MAC地址 */
typedef  struct
{
    WORD32     type;
    WORD32     dwipaddr;
    WORD32     dwResult;
    WORD32	dwTimeoutMax;
}_PACKED_1_ T_Mts_SPR11_Check_Macband;

/* 温度传感器测试 */
typedef struct
{
   /* 数组下标0代表LM_93  1代表75_1, 2代表75_2, 3代表75_2 4代表75_3 */
   BYTE    cSenorStatus[4];  /* 出参; MTS_SENOR_GOOD正常  MTS_SENOR_NOT_GOOD 不正常 */
   SWORD32 sdwSenorValue[4]; /* 出参; 传感器的实际温度 */
   BYTE    cEncDevId;          /* 扩展柜号 */
   WORD32  dwTimeoutMax;     /* 超时最大值 */
}_PACKED_1_ T_Mts_SPR11_Check_Senor;


/* BBU信息读取 */
typedef struct
{
    WORD32  dwBbuId;	    /* BBU电池序列号     */
	WORD16  wBbuMv;			/* BBU当前电压       */
	WORD16  wBbuCapacity;	/* BBU剩余电量百分比 */
	BYTE    ucPresent;   	/* BBU在位信息  MTS_BBU_PRESENT;  在位	        MTS_BBU_NOTPRESENT:  不在位 */
	BYTE    ucWorkFlag;    	/* 是否正常工作	MTS_BBU_CHARGING: BBU正在充放电 MTS_BBU_NOT_CHARGING:BBU此时没有充放电 */
	BYTE    cCheckResult;   /* BBU状态 MTS_BBU_GOOD 正常 MTS_BBU_GOOD不正常 */
    WORD32  dwTimeoutMax;   /* 超时最大值 */
}_PACKED_1_ T_Mt_SPR11_Check_Bbu;

/* 温度传感器测试 */
typedef struct
{
   BYTE    cType;
   BYTE    cSenorStatus[40];  /* 出参; PHY速率*/
   BYTE    cEncDevId;          /* 扩展柜号 */
   WORD32  dwTimeoutMax;     /* 超时最大值 */
}_PACKED_1_ T_Mts_SPR11_SASLINK;

/* RTC实时时钟电路测试 */
typedef struct
{
   BYTE    cCheckResult;   /* 出参：MTS_RTC_GOOD 	    测试通过
                                    MTS_RTC_NOT_GOOD	测试不通过 */
   WORD32  dwTimeoutMax;   /* 超时最大值 */
}_PACKED_1_ T_Mts_SPR11_Check_Rtc;

/* 板内时钟电路测试 */
typedef struct
{
   WORD16  wMtsClockType;  /* 入参：MTS_25M_CLOCK, MTS_33_CLOCK...*/
   BYTE    cCheckResult;   /* 出参：MTS_CLOCK_GOOD 	    测试通过
                                   MTS_CLOCK_NOT_GOOD	测试不通过*/
   WORD32  dwTimeoutMax;   /* 超时最大值 */
}_PACKED_1_ T_Mts_SPR11_Check_Clock;

/* 硬盘指示灯测试 */
typedef struct
{
	BYTE	cMtsType;		    /* type */
	WORD32  dwLightMode;     /* 点灯模式电源OR DISK OR 面板*/
	WORD32  dwLightType;     /* color*/
       WORD32  dwIndexId;       /* 索引号 1~12*/
	WORD32	dwTimeoutMax;		/* 超时最大值 */
}_PACKED_1_ T_Mts_SPR11_Check_LED_Light;

/* 工作EPLD信息读取 */
typedef struct
{
      WORD32 dwBoardId;         /* 单板ID        */
	WORD32 dwPcbVer;          /* 单板pcb版本号 */
	WORD32 dwBomVer;          /* 单板bom料单   */
	BYTE    ucSlotId;               /*slotid*/
	WORD32  dwBackBoardTypeId;/*背板ID*/
	WORD32 dwTimeoutMax;      /* 超时最大值 */
}_PACKED_1_ T_Mts_SPR11_Check_Work_Epld_Info;

/* 工作EPLD信息读取 */
typedef struct
{

    WORD32 dwcpuid;         /* CPUid*/
	WORD32 dwsbrigeid;          /* 南桥芯片ID */
	WORD32 mac350id;          /* 350网口ID*/
	WORD32 mac82580id;          /* 82580网口ID*/
	  BYTE    cCheckResult[4];   /* 出参：MTS_CLOCK_GOOD 	    测试通过
                                   MTS_CLOCK_NOT_GOOD	测试不通过*/
	WORD32 dwTimeoutMax;      /* 超时最大值 */
}_PACKED_1_ T_Mts_SPR11_Check_Chips;

/* 工作EPLD信息读取 */
typedef struct
{
	WORD32 dwTimeoutMax;      /* 超时最大值 */
	T_Mts_SPR11_Check_Chips chips[2];
}_PACKED_1_ T_Cmt_SPR11_Check_Chips;
/* 电子条形码扫描 */
typedef struct
{
    BYTE   cLableInfo[MAX_INFO_LABLE_LENGTH]; /* 条形码 */
	WORD32 dwTimeoutMax;                      /* 超时最大值 */
}_PACKED_1_ T_Mts_SPR11_Check_Lable_Info;

/* 版本查询 */
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
    BYTE  ifPortValid;                /* 标识该端口是否存在.    0 : 端口不存在; 1 : 端口存在.           */
    BYTE  PortID[FC_PORT_ID_LENGTH];  /* 端口ID,FC协议生成,显示方式为0x123456      */
    BYTE  PortNo;                     /* 端口编号,按FC接口面板顺序进行,从1开始编号 */

    BYTE  DataRate;                   /* 端口速率                                  */
                                               /* #define FC_SPEED_INVALID 255              */
                                               /* #define FC_SPEED_2G       2               */
                                               /* #define FC_SPEED_4G       4               */
                                               /* #define FC_SPEED_8G       8               */

    BYTE  LinkStatus;                 /* 链路状态                                  */
                                               /* 0 : UP; 1 : DOWN.                         */

    BYTE  Topology;                   /* 端口拓扑                                  */
                                               /* FC端口拓扑为私有环     0x01               */
                                               /* FC端口拓扑为公有环     0x02               */
                                               /* FC端口拓扑为点对点     0x04               */
                                               /* FC端口拓扑为Fabric     0x08               */
    BYTE   LinkLength;
    BYTE   SpeedCapacity;
    BYTE   TransType;
    BYTE   TxStatus;
    BYTE   LOSStatus;
    BYTE   FaultStatus;
    BYTE   ifSFPONLine;
    BYTE  WWPN[FC_WWN_LENGTH];        /* 端口WWPN                                  */
    WORD16 deviceid;                   /* 设备id */
}_PACKED_1_ T_Mts_SPR11_FCPortInfo;


/* SCS获取FC端口信息索引结构 */
typedef  struct
{
    WORD32 CtrlNo;
    BYTE      WWNN[FC_WWN_LENGTH];         /* 端口所在单板的WWNN               */
    T_Mts_SPR11_FCPortInfo FCPortInfo[FC_MAX_PORT_NUM]; /* FC接口信息结构                   */
    WORD32	dwTimeoutMax;  //
}_PACKED_1_ T_Mts_SPR11_FCinfo;

/* SCS获取FC端口信息索引结构 */
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
    WORD32 CtrlNo;                                  /* 控制器号,值为0或1    */
    WORD32 PortNo;                                  /* 端口号，值为1-4      */

    BYTE   TxStatus;                                /* 设置的光模块发送状态 */
    BYTE   ifSFPONLine;
    BYTE   ReservedOne;                             /* 保留字段             */
    WORD16 ReservedTwo;                             /* 保留字段             */
}_PACKED_1_ T_Cmt_FCSetMSTSFPTXStatus;

/* 面板GE口环回测试 */
typedef struct
{
   WORD32 dwPortid;   /* 入参，FC端口号1-4 */
   WORD32  wMtsType;       /* 入参，测试类型:MTS_FT,  功能测试: MTS_HT,      高温*/
   WORD32 dwSndFrameSet;
    WORD32 dwSndFrameCount; /* 出参，发出帧个数 */
    WORD32 dwRcvFrameCount; /* 出参，收到帧个数 */
    WORD32 dwFrameCheckErrCount; /* 出参，收到的帧中校验有错的个数 */
   WORD32  dwTimeoutMax;   /* 超时最大值 */
}_PACKED_1_ T_Mts_Check_FC;

/* FC速率灯测试 */
typedef struct
{
       WORD32	cPortid;		/* 端口id*/
	WORD32	cLedid;		/* LEDid*/
	WORD32	cLedStatus;		/* LED status*/
	WORD32 dwPreFlag[2];  /* 出参，设置前的两个灯状态，值同上 */
       WORD32 dwCurFlag[2];  /* 出参，设置后的两个灯的状态，值同上 */
	WORD32	dwTimeoutMax;		/* 超时最大值 */
}_PACKED_1_ T_Mts_Check_FC_LED;

/* 工作EPLD自检 */
typedef struct
{
       WORD64 dwCapacity[2];/*磁盘容量*/
	BYTE aucPhyDevName[2][64]; /* 硬盘物理设备符名称 */
	BYTE aucModel[2][64];      /* 硬盘厂商名称 */
	BYTE aucSerialNum[2][64];  /* 硬盘设备序列号 */
	WORD32 dwformatResult;       /* 格式化测试  */
	WORD32 dwRWResult;     /* 读写测试  */
       WORD32 dwSmartResult;     /* SMART检测结果  */
	WORD32 dwTimeoutMax;      /* 超时最大值 */
	WORD32 dwtype;      /* 测试类型 */
}_PACKED_1_ T_Mts_SPR11_SSD;

/* 工作EPLD自检 */
typedef struct
{
	WORD32 dwTimeoutMax;      /* 超时最大值 */
	T_Mts_SPR11_SSD ssdinfo[2];
}_PACKED_1_ T_Cmt_SPR11_SSD;
/* 工作EPLD自检 */
typedef struct
{
       WORD32  dwPhyNum[2];     /* SAS控制器端口号 */
	WORD64 dwCapacity[2]; /* 硬盘容量 */
	BYTE aucPhyDevName[2][64]; /* 硬盘物理设备符名称 */
	BYTE aucModel[2][64];      /* 硬盘厂商名称 */
	BYTE aucSerialNum[2][64];  /* 硬盘设备序列号 */
	BYTE aucFirmVer[2][64];	/* 硬盘固件版本号 */
	WORD32 dwTimeoutMax;      /* 超时最大值 */
}_PACKED_1_ T_Mts_SPR11_USB;

/* 在位信号测试 */
typedef struct
{
	WORD32   dwOnlieFlag;        /* 在位标志 */
	WORD32   dwTimeoutMax;      /* 超时最大值 */
}_PACKED_1_ T_Mts_SPR11_Online_Flag;

/* 在位信号测试 */
typedef struct
{
	WORD32   dwResult;        /* 是否正常*/
	WORD32  wMtsType;       /* 入参*/
	WORD32   dwTimeoutMax;      /* 超时最大值 */
}_PACKED_1_ T_Mts_SPR11_ChipCLOCK;

/* 在位信号测试 */
typedef struct
{
	WORD32   dwTimeoutMax;      /* 超时最大值 */
	T_Mts_SPR11_ChipCLOCK chiplock[2];
}_PACKED_1_ T_Cmt_SPR11_ChipCLOCK;

/* 在位信号测试 */
typedef struct
{
	WORD32   dwResult;        /* 是否正常*/
	WORD32   dwTimeoutMax;      /* 超时最大值 */
}_PACKED_1_ T_Mts_SPR11_CHECKCOMMON;

/* 在位信号测试 */
typedef struct
{
	WORD32   dwResult;        /* 是否正常*/
	WORD32  wMtsType;       /* 入参，0 背板IO输出，1 背板IO输入，2对板上下点3磁盘在位信息*/
	WORD32   dwPsID;        /* 电源ID*/
	WORD32   dwSwitch;        /* 开关*/
	WORD32  dwSvrOp;
	WORD16  wPowerCtrl;
	BYTE ucDiskSlot[12];
	BYTE ucOnlineFlag[12];
	WORD32   dwTimeoutMax;      /* 超时最大值 */
}_PACKED_1_ T_Mts_SPR11_CHECKBACKPLANEIO;


typedef struct
{
    WORD32 sasAddressHi[2];       /* Expander SAS地址，高32位 */
    WORD32 sasAddressLow[2];      /* Expander SAS地址，低32位 */
    WORD32 chassisId;             /* Expander 箱体ID */
}_PACKED_1_ T_Mts_DspamExpanderInfo;

typedef struct
{
    WORD32 sasBaseAddressHi;      /* SAS基地址，高32位，入参 */
    WORD32 sasBaseAddressLow;     /* SAS基地址，低32位，入参 */
    WORD32 sasEnclosureNum;       /* 箱体个数，入参 */
    WORD32 sasDspamCardNum;       /* 待烧结子卡数目，入参 */
    WORD32 sasDspamOptCode;       /* 操作类型, 入参 */
    WORD32 sasDspamResult;        /* 烧结结果, 出参 */
    T_Mts_DspamExpanderInfo tDspamExpanderInfo[6]; /* 系统中Expander信息，入参 */
}_PACKED_1_ T_Mts_DspamUpdateSasAddrInfo;

/* 在位信号测试 */
typedef struct
{
    WORD32   dwTimeoutMax;        /* 是否正常*/
    T_Mts_DspamUpdateSasAddrInfo DspamInfo;
}_PACKED_1_ T_Mts_SPR11_DSPAM;

/***********************************spr11 测试仪end****************************************/


/*VPD FRU信息*/
typedef struct
{
    SWORD32  sdwEnclosureId;//箱体ID
    WORD16 uwBackplaneID;//背板ID
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
* 函数名称：GetSvrSocket
* 输入参数：
* 输出参数：ptSvrSoc
* 返 回 值：    0       -   执行成功             非0 -   执行失败
* 其它说明：无
***********************************************************************/
SWORD32 GetSvrSocket(T_SvrSocket *ptSvrSoc);
/**********************************************************************
* 函数名称：SetSvrSocket
* 输入参数：ptSvrSoc
* 输出参数：
* 返 回 值：    0       -   执行成功             非0 -   执行失败
* 其它说明：无
***********************************************************************/
SWORD32 SetSvrSocket(T_SvrSocket *ptSvrSoc);


/* FC会话相关 */
/* 目的端口的信息结构 */
typedef struct T_FCObjProtocolInfo
{
    BYTE   ucPortName[FC_WWN_LENGTH];/* 对端的端口名                               */
    BYTE   ucNodeName[FC_WWN_LENGTH]; /* 对端的节点名                             */
    BYTE   ucifTgtFun;                    /* 对端是否具有TARGET功能.    0:不具备;1:具备 */
    BYTE   ucifIniFun;                     /* 对端是否具有INITIATOR功能. 0:不具备;1:具备 */
    WORD16 Reserved;                 /* 保留字段                                   */
}T_FCObjProtocolInfo;

/*单个端口的FC协议信息结构 */
typedef struct T_GetFCProtocolIInfo
{
    SWORD32                   sdwCtrlId;                       /* 控制器号,值为0或1        */
    SWORD32                   sdwPortNo;
    SWORD32                   sdwValidNum;                  /* 有效的端口登录信息数     */
    /* 端口号，值为1-4          */
    T_FCObjProtocolInfo    tProtItem[FC_ALLOW_PLOGI_CNT];  /* 单条端口登录信息结构数组 */
}T_GetFCProtocolIInfo;


/* 单个端口的链路统计信息结构 */
typedef struct T_FCLinkStatisInfo
{

    WORD32  LinkUp; /* 链路UP计数统计                              */
    WORD32  LinkDown;/* 链路DOWN计数统计                           */
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


/* 单个端口的基本连接业务统计信息结构 */
typedef struct T_FCPortBLSStatisInfo
{
    WORD32  ABTSSendCnt;     /* 发送ABTS计数统计                  */
    WORD32  ABTSACCRcvCnt;   /* 接收ABTS BA_ACCEPT计数统计        */
    WORD32  ABTSRJTRcvCnt;   /* 接收ABTS BA_REJECT计数统计        */

    WORD32  ABTSRcvCnt;      /* 接收ABTS计数统计                  */
    WORD32  ABTSACCSendCnt;  /* 发送ABTS BA_ACCEPT计数统计        */
    WORD32  ABTSRJTSendCnt;  /* 发送ABTS BA_REJECT计数统计        */

    WORD32  NOPSendCnt;      /* 发送NOP计数统计                   */
    WORD32  NOPACCRcvCnt;    /* 接收NOP BA_ACCEPT计数统计         */
    WORD32  NOPRJTRcvCnt;    /* 接收NOP BA_REJECT计数统计         */

    WORD32  NOPRcvCnt;       /* 接收NOP计数统计                   */
    WORD32  NOPACCSendCnt;   /* 发送NOP BA_ACCEPT计数统计         */
    WORD32  NOPRJTSendCnt;   /* 发送NOP BA_REJECT计数统计         */

    WORD32  OtherACCRcvCnt;  /* 接收非ABST与NOP BA_ACCEPT计数统计 */
    WORD32  OtherACCSendCnt; /* 发送非ABST与NOP BA_ACCEPT计数统计 */
    WORD32  OtherRJTRcvCnt;  /* 接收非ABST与NOP BA_REJECT计数统计 */
    WORD32  OtherRJTSendCnt; /* 发送非ABST与NOP BA_REJECT计数统计 */
}T_FCPortBLSStatisInfo;


/* 单个端口的扩展连接业务统计信息结构 */
typedef struct T_FCPortELSStatisInfo
{
    WORD32  FLOGISendCnt;      /* 发送FLOGI计数统计           */
    WORD32  FLOGIACCRcvCnt;    /* 接收FLOGI LS_ACCEPT计数统计 */
    WORD32  FLOGIRJTRcvCnt;    /* 接收FLOGI LS_REJECT计数统计 */

    WORD32  FLOGIRcvCnt;       /* 接收FLOGI计数统计           */
    WORD32  FLOGIACCSendCnt;   /* 发送FLOGI LS_ACCEPT计数统计 */
    WORD32  FLOGIRJTSendCnt;   /* 发送FLOGI LS_REJECT计数统计 */

    WORD32  PLOGISendCnt;      /* 发送PLOGI计数统计           */
    WORD32  PLOGIACCRcvCnt;    /* 接收PLOGI LS_ACCEPT计数统计 */
    WORD32  PLOGIRJTRcvCnt;    /* 接收PLOGI LS_REJECT计数统计 */

    WORD32  PLOGIRcvCnt;       /* 接收PLOGI计数统计           */
    WORD32  PLOGIACCSendCnt;   /* 发送PLOGI LS_ACCEPT计数统计 */
    WORD32  PLOGIRJTSendCnt;   /* 发送PLOGI LS_REJECT计数统计 */

    WORD32  PRLISendCnt;       /* 发送PRLI计数统计            */
    WORD32  PRLIACCRcvCnt;     /* 接收PRLI LS_ACCEPT计数统计  */
    WORD32  PRLIRJTRcvCnt;     /* 接收PRLI LS_REJECT计数统计  */

    WORD32  PRLIRcvCnt;        /* 接收PRLI计数统计            */
    WORD32  PRLIACCSendCnt;    /* 发送PRLI LS_ACCEPT计数统计  */
    WORD32  PRLIRJTSendCnt;    /* 发送PRLI LS_REJECT计数统计  */

    WORD32  LOGOSendCnt;       /* 发送LOGO计数统计            */
    WORD32  LOGOACCRcvCnt;     /* 接收LOGO LS_ACCEPT计数统计  */
    WORD32  LOGORJTRcvCnt;     /* 接收LOGO LS_REJECT计数统计  */

    WORD32  LOGORcvCnt;        /* 接收LOGO计数统计            */
    WORD32  LOGOACCSendCnt;    /* 发送LOGO LS_ACCEPT计数统计  */
    WORD32  LOGORJTSendCnt;    /* 发送LOGO LS_REJECT计数统计  */

    WORD32  PRLOSendCnt;       /* 发送PRLO计数统计            */
    WORD32  PRLOACCRcvCnt;     /* 接收PRLO LS_ACCEPT计数统计  */
    WORD32  PRLORJTRcvCnt;     /* 接收PRLO LS_REJECT计数统计  */

    WORD32  PRLORcvCnt;        /* 接收PRLO计数统计            */
    WORD32  PRLOACCSendCnt;    /* 发送PRLO LS_ACCEPT计数统计  */
    WORD32  PRLORJTSendCnt;    /* 发送PRLO LS_REJECT计数统计  */

    WORD32  SCRSendCnt;        /* 发送SCR计数统计             */
    WORD32  SCRACCRcvCnt;      /* 接收SCR LS_ACCEPT计数统计   */
    WORD32  SCRRJTRcvCnt;      /* 接收SCR LS_REJECT计数统计   */

    WORD32  SCRRcvCnt;         /* 接收SCR计数统计             */
    WORD32  SCRACCSendCnt;     /* 发送SCR LS_ACCEPT计数统计   */
    WORD32  SCRRJTSendCnt;     /* 发送SCR LS_REJECT计数统计   */

    WORD32  RSCNSendCnt;       /* 发送RSCN计数统计            */
    WORD32  RSCNACCRcvCnt;     /* 接收RSCN LS_ACCEPT计数统计  */
    WORD32  RSCNRJTRcvCnt;     /* 接收RSCN LS_REJECT计数统计  */

    WORD32  RSCNRcvCnt;        /* 接收RSCN计数统计            */
    WORD32  RSCNACCSendCnt;    /* 发送RSCN LS_ACCEPT计数统计  */
    WORD32  RSCNRJTSendCnt;    /* 发送RSCN LS_REJECT计数统计  */

    WORD32  ECHOSendCnt;       /* 发送ECHO计数统计            */
    WORD32  ECHOACCRcvCnt;     /* 接收ECHO LS_ACCEPT计数统计  */
    WORD32  ECHORJTRcvCnt;     /* 接收ECHO LS_REJECT计数统计  */

    WORD32  ECHORcvCnt;        /* 接收ECHO计数统计            */
    WORD32  ECHOACCSendCnt;    /* 发送ECHO LS_ACCEPT计数统计  */
    WORD32  ECHORJTSendCnt;    /* 发送ECHO LS_REJECT计数统计  */

    WORD32  ADISCSendCnt;      /* 发送ADISC计数统计           */
    WORD32  ADISCACCRcvCnt;    /* 接收ADISC LS_ACCEPT计数统计 */
    WORD32  ADISCRJTRcvCnt;    /* 接收ADISC LS_REJECT计数统计 */

    WORD32  ADISCRcvCnt;       /* 接收ADISC计数统计           */
    WORD32  ADISCACCSendCnt;   /* 发送ADISC LS_ACCEPT计数统计 */
    WORD32  ADISCRJTSendCnt;   /* 发送ADISC LS_REJECT计数统计 */

    WORD32  PDISCSendCnt;      /* 发送PDISC计数统计           */
    WORD32  PDISCACCRcvCnt;    /* 接收PDISC LS_ACCEPT计数统计 */
    WORD32  PDISCRJTRcvCnt;    /* 接收PDISC LS_REJECT计数统计 */

    WORD32  PDISCRcvCnt;       /* 接收PDISC计数统计           */
    WORD32  PDISCACCSendCnt;   /* 发送PDISC LS_ACCEPT计数统计 */
    WORD32  PDISCRJTSendCnt;   /* 发送PDISC LS_REJECT计数统计 */


    WORD32  OtherELSRcvCnt;    /* 其它ELS接收计数统计         */
    WORD32  OtherELSSendCnt;   /* 其它ELS发送计数统计         */
    WORD32  OtherLSACCRcvCnt;  /* 其它LS_ACCEPT接收计数统计   */
    WORD32  OtherLSACCSendCnt; /* 其它LS_ACCEPT发送计数统计   */
    WORD32  OtherLSRJTRcvCnt;  /* 其它LS_REJECT接收计数统计   */
    WORD32  OtherLSRJTSendCnt; /* 其它LS_REJECT发送计数统计   */
}T_FCPortELSStatisInfo;



/*单个端口的通用业务统计信息结构 */
typedef struct T_FCPortGSStatisInfo
{
    WORD32  GIDPNSendCnt;      /* 发送GIDPN计数统计           */
    WORD32  GIDPNACCRcvCnt;    /* 接收GIDPN CT_ACCEPT计数统计 */
    WORD32  GIDPNRJTRcvCnt;    /* 接收GIDPN CT_REJECT计数统计 */

    WORD32  GPNIDSendCnt;      /* 发送GPNID计数统计           */
    WORD32  GPNIDACCRcvCnt;    /* 接收GPNID CT_ACCEPT计数统计 */
    WORD32  GPNIDRJTRcvCnt;    /* 接收GPNID CT_REJECT计数统计 */

    WORD32  RNNIDSendCnt;      /* 发送RNNID计数统计           */
    WORD32  RNNIDACCRcvCnt;    /* 接收RNNID CT_ACCEPT计数统计 */
    WORD32  RNNIDRJTRcvCnt;    /* 接收RNNID CT_REJECT计数统计 */

    WORD32  RFTIDSendCnt;      /* 发送RFTID计数统计           */
    WORD32  RFTIDACCRcvCnt;    /* 接收RFTID CT_ACCEPT计数统计 */
    WORD32  RFTIDRJTRcvCnt;    /* 接收RFTID CT_REJECT计数统计 */

    WORD32  OtherGSRcvCnt;     /* 其它GS接收计数统计          */
    WORD32  OtherGSSendCnt;    /* 其它GS发送计数统计          */
    WORD32  OtherCTACCRcvCnt;  /* 其它CT_ACCEPT接收计数统计   */
    WORD32  OtherCTACCSendCnt; /* 其它CT_ACCEPT发送计数统计   */
    WORD32  OtherCTRJTRcvCnt;  /* 其它CT_REJECT接收计数统计   */
    WORD32  OtherCTRJTSendCnt; /* 其它CT_REJECT发送计数统计   */
}T_FCPortGSStatisInfo;


/* 单个端口的FCP业务统计信息结构 */
typedef struct T_FCPortFCPStatisInfo
{
    WORD64  CMDSendCnt;     /* FCP命令发送计数统计             */
    WORD64  XFERSendCnt;    /* FCP XFER发送计数统计            */
    WORD64  RESPSendCnt;    /* FCP RESP发送计数统计            */

    WORD64  CMDRcvCnt;      /* FCP命令接收计数统计             */
    WORD64  DATARcvCnt;     /* FCP数据接收计数统计            */
    WORD64  RESPRcvCnt;     /* FCP RESP接收计数统计            */

    WORD64  CMDExeSuccCnt;  /* 命令执行成功计数统计            */
    WORD64  CMDExeFailCnt;  /* 命令执行失败计数统计            */

}T_FCPortFCPStatisInfo;

/*单个端口的统计信息结构 */
typedef struct T_FCGetPortStatisInfo
{
    SWORD32                  CtrlId;     /* 控制器号,值为0或1 */
    SWORD32                  PortNo;     /* 端口号，值为1-4   */

    T_FCLinkStatisInfo     tLinkStat;   /* 链路统计信息      */
    T_FCPortBLSStatisInfo  tBLSStat;    /* BLS统计信息       */
    T_FCPortELSStatisInfo  tELSStat;    /* ELS统计信息       */
    T_FCPortGSStatisInfo   tGSStat;     /* GS统计信息  */
    T_FCPortFCPStatisInfo  tFCPStat;    /* FCP统计信息  */
}T_FCGetPortStatisInfo;

/* 单个端口的统计信息结构 */
typedef struct T_ClearFCPortStatisInfo
{
    SWORD32    CtrlId;         /* 控制器号,值为0或1               */
    SWORD32    PortNo;         /* 端口号，值为1-4                 */
}T_ClearFCPortStatisInfo;
/*fc会话信息的呈现cli和web不同， 以下结构体提供给web使用 */

/* SCS获取单个端口的链路统计信息结构 */
typedef struct T_WebGetFCLinkStatisInf
{
    WORD32  LinkUp; /* 链路UP计数统计                              */
    WORD32  LinkDown;/* 链路DOWN计数统计                           */
}T_WebGetFCLinkStatisInf;


/* SCS获取单个端口的基本连接业务统计信息结构 */
typedef struct T_WebGetFCPortBLSStatisInf
{
    WORD32  BLSFrameSendCnt;   /* 发送BLS帧计数        */
    WORD32  BLSACCRcvCnt;      /* 接收BLS ACCEPT帧计数 */
    WORD32  BLSPRJRcvCnt;      /* 接收BLS REJECT帧计数 */

    WORD32  BLSFrameRcvCnt;    /* 接收BLS帧计数 */
    WORD32  BLSACCSendCnt;     /* 发送BLS ACCEPT帧计数 */
    WORD32  BLSPRJSendCnt;     /* 发送BLS REJECT帧计数 */
}T_WebGetFCPortBLSStatisInf;


/* SCS获取Web界面显示的单个端口的扩展连接业务统计信息结构 */
typedef struct T_WebGetFCPortELSStatisInf
{
    WORD32  ELSFrameSendCnt;   /* 发送ELS帧计数        */
    WORD32  ELSACCRcvCnt;      /* 接收ELS ACCEPT帧计数 */
    WORD32  ELSPRJRcvCnt;      /* 接收ELS REJECT帧计数 */

    WORD32  ELSFrameRcvCnt;    /* 接收ELS帧计数 */
    WORD32  ELSACCSendCnt;     /* 发送ELS ACCEPT帧计数 */
    WORD32  ELSPRJSendCnt;     /* 发送ELS REJECT帧计数 */
}T_WebGetFCPortELSStatisInf;


/* SCS获取Web界面显示的单个端口的通用业务统计信息结构 */
typedef struct T_WebGetFCPortGSStatisInf
{
    WORD32  GSFrameSendCnt;   /* 发送GS帧计数        */
    WORD32  GSACCRcvCnt;      /* 接收GS ACCEPT帧计数 */
    WORD32  GSPRJRcvCnt;      /* 接收GS REJECT帧计数 */

    WORD32  GSFrameRcvCnt;    /* 接收GS帧计数 */
    WORD32  GSACCSendCnt;     /* 发送GS ACCEPT帧计数 */
    WORD32  GSPRJSendCnt;     /* 发送GS REJECT帧计数 */
}T_WebGetFCPortGSStatisInf;

typedef struct T_WebGetFCPortStatisInf
{
    SWORD32                sdwCtrlId;           /* 控制器号,值为0或1 */
    SWORD32                sdwPortId;           /* 端口号，值为1-4   */

    T_FCPortFCPStatisInfo       tFCPStat;    /* FCP统计信息       */
    T_WebGetFCLinkStatisInf     tLinkStat;   /* 链路统计信息      */
    T_WebGetFCPortBLSStatisInf  tBLSStat;    /* BLS统计信息       */
    T_WebGetFCPortELSStatisInf  tELSStat;    /* ELS统计信息       */
    T_WebGetFCPortGSStatisInf   tGSStat;     /* GS统计信息        */
}T_WebGetFCPortStatisInf;


/**
   @struct     tagSYS_CLOCK
   @brief      系统软时钟结构，表示绝对时间
*/
typedef struct tagSYS_CLOCK
{
    WORD16  wSysYear;       /**< 年 */
    BYTE    ucSysMon;       /**< 月 */
    BYTE    ucSysDay;       /**< 日 */

    BYTE    ucSysHour;      /**< 时 */
    BYTE    ucPading0;      /**< 填充字*/
    BYTE    ucSysMin;       /**< 分 */
    BYTE    ucSysSec;       /**< 秒 */

    WORD16  wMilliSec;      /**< 毫秒 */
    BYTE    ucSysWeek;      /**< 周 */
    BYTE    ucPading1;      /**< 填充字*/
}T_SysSoftClock;


typedef struct
{
    BYTE  ucChkUpdateUsp;   /* 大版本更新检查 */
} _PACKED_1_ T_UpdateUspChk;

/**
    @struct	T_Alarm_Req
    @brief	请求上报告警
    <HR>
    @b 修改记录：
*/
typedef struct
{
/**
    @li @b	参数			dwRecNo
    @li @b	解释			请求的开始记录号
*/
    WORD32               dwRecNo;
/**
    @li @b	参数			dwReqNum
    @li @b	解释			请求的告警数
*/
    WORD32               dwReqNum;
/**
    @li @b	参数			dwLang
    @li @b	解释			语言类型
<TABLE>
    <TR  Align=Center><TD>语言类型</TD><TD>语言类型含义</TD></TR>
    <TR  Align=Center><TD>0</TD><TD>英文</TD></TR>
    <TR  Align=Center><TD>1</TD><TD>中文</TD></TR>
</TABLE>
*/
    WORD32              dwLang;

    /*后续字段为保留字段，可以不赋值*/

/**
    @li @b	参数			dwAlarmCode
    @li @b	解释			告警码
*/
    WORD32             dwAlarmCode;
/**
    @li @b	参数			acAlarmName
    @li @b	解释			告警名称
*/
	CHAR                acAlarmName[ALARM_NAME_LENGTH];
/**
    @li @b	参数			wAlarmLevel
    @li @b	解释			告警级别
<TABLE>
    <TR  Align=Center><TD>告警级别</TD><TD>告警级别中文</TD><TD>告警级别英文</TD></TR>
    <TR  Align=Center><TD>1</TD><TD>严重</TD><TD>Critical</TD></TR>
    <TR  Align=Center><TD>2</TD><TD>重要</TD><TD>Major</TD></TR>
    <TR  Align=Center><TD>3</TD><TD>一般</TD><TD>Minor</TD></TR>
    <TR  Align=Center><TD>4</TD><TD>轻微</TD><TD>Warning</TD></TR>
    <TR  Align=Center><TD>5</TD><TD>提示</TD><TD>Notice</TD></TR>
    <TR  Align=Center><TD>非上述值</TD><TD>未指定</TD><TD>Unspecified</TD></TR>
</TABLE>

*/
    WORD16             wAlarmLevel;
/**
    @li @b	参数			tAlarmTimeStart
    @li @b	解释			告警时间段开始时间
*/
    T_SysSoftClock    tAlarmTimeStart;
/**
    @li @b	参数			tAlarmTimeEnd
    @li @b	解释			告警时间段结束时间
*/
    T_SysSoftClock    tAlarmTimeEnd;
}_PACKED_1_ T_Alarm_Req;

/**
    @struct T_AlarmAddInfo
    @brief  告警/通知附加信息
    <HR>
    @b 修改记录：
*/
typedef struct
{
    CHAR aucAddInfo[ALARM_ADDINFO_UNION_MAX];   /* 附加信息为一个字符串呈现。WEB直接显示即可 */
}_PACKED_1_ T_AlarmAddInfo;

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
    @li @b	解释			告警码
*/
    WORD32             dwAlarmCode;
/**
    @li @b	参数			dwAlarmNo
    @li @b	解释			告警编号
*/
    WORD32             dwAlarmNo;
/**
    @li @b	参数			acAlarmName
    @li @b	解释			告警名称
*/
	CHAR                acAlarmName[ALARM_NAME_LENGTH];
/**
    @li @b	参数			ucAlarmFlag
    @li @b	解释			告警标志: 1-告警;2-恢复
*/
    BYTE               ucAlarmFlag;
/**
    @li @b	参数			aucPad
    @li @b	解释			填充字节
*/
    CHAR              aucPad[3];
/**
    @li @b	参数			wKeyLen
    @li @b	解释			告警附加信息结构关键字长度 保留，暂不使用
*/
    WORD16             wKeyLen;
/**
    @li @b	参数			wAlarmLevel
    @li @b	解释			告警级别
<TABLE>
    <TR  Align=Center><TD>告警级别</TD><TD>告警级别中文</TD><TD>告警级别英文</TD></TR>
    <TR  Align=Center><TD>1</TD><TD>严重</TD><TD>Critical</TD></TR>
    <TR  Align=Center><TD>2</TD><TD>重要</TD><TD>Major</TD></TR>
    <TR  Align=Center><TD>3</TD><TD>一般</TD><TD>Minor</TD></TR>
    <TR  Align=Center><TD>4</TD><TD>轻微</TD><TD>Warning</TD></TR>
    <TR  Align=Center><TD>5</TD><TD>提示</TD><TD>Notice</TD></TR>
    <TR  Align=Center><TD>非上述值</TD><TD>未指定</TD><TD>Unspecified</TD></TR>
</TABLE>

*/
    WORD16             wAlarmLevel;

/**
    @li @b	参数			tAlarmTime
    @li @b	解释			告警上报时间
*/
    T_SysSoftClock    tAlarmTime;

/**
    @li @b	参数			tAddInfo
    @li @b	解释			告警体内容
*/
    T_AlarmAddInfo    tAddInfo;
}_PACKED_1_ T_AlarmReport;


/**
    @struct	T_Agent_Pack_Alarm
    @brief	APP调用OAM库函数上报告警信息
    <HR>
    @b 修改记录：
*/
typedef struct
{
/**
    @li @b	参数			wRetCode
    @li @b	解释			操作结果 0：成功
*/
    WORD16              wRetCode;
/**
    @li @b	参数			ucAlarmNum
    @li @b	解释			本包中告警或者恢复的个数
*/
    BYTE               ucAlarmNum;
/**
    @li @b	参数			ucLastPacket 0:非最后一包.1:最后一包
    @li @b	解释			是否最后一包
*/
    BYTE              ucLastPacket;
/**
    @li @b	参数			wTotalAlarmNum
    @li @b	解释			总告警数
*/
    WORD16              wTotalAlarmNum;
/**
    @li @b	参数			tAlarmAddr
    @li @b	解释			单板告警发生位置

    T_AlarmAddress     tAlarmAddr;
*/
/**
    @li @b	参数			tAlarmReport
    @li @b	解释			告警上报的告警实体
*/
    T_AlarmReport      tAlarmReport[ALARM_PER_PACKET_MAX];
}_PACKED_1_ T_Alarm_Packet_Ack;

 /**
   @struct T_Alarm_Del_Alarm
   @brief 删除指定告警

   @li @b 功能 删除指定告警

   <HR>
   @b 修改记录：

 */
typedef struct
{
/**
    @li @b	参数			dwAlarmCode
    @li @b	解释			告警码
*/
    WORD32             dwAlarmCode;
/**
    @li @b	参数			dwAlarmNo
    @li @b	解释			告警编号
*/
    WORD32             dwAlarmNo;
/**
    @li @b	参数			ucPad
    @li @b	类型			BYTE
    @li @b	解释			填充
*/
    BYTE                   ucPad[24];
}_PACKED_1_ T_Alarm_Del_Alarm;

/**
   @struct T_Alarm_Del_Alarm_Ack
   @brief 删除指定告警应答

   @li @b 功能 删除指定告警应答给后台

   <HR>
   @b 修改记录：

*/
typedef struct
{
/**
    @li @b	参数			dwAlarmCode
    @li @b	解释			告警码
*/
    WORD32             dwAlarmCode;
/**
    @li @b	参数			dwAlarmNo
    @li @b	解释			告警编号
*/
    WORD32             dwAlarmNo;
/**
    @li @b	参数			ucResult
    @li @b	类型			BYTE
    @li @b	解释			应答结果
<TABLE>
    <TR  Align=Center><TD>ucResult</TD><TD>应答结果说明</TD></TR>
    <TR  Align=Center><TD>0</TD><TD>正常删除</TD></TR>
    <TR  Align=Center><TD>1</TD><TD>删除失败</TD></TR>
</TABLE>
*/
    BYTE                    ucResult;
/**
    @li @b	参数			aucPad
    @li @b	类型			BYTE
    @li @b	解释			填充字节
*/
    BYTE                   aucPad[23];
}_PACKED_1_ T_Alarm_Del_Alarm_Ack;
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
* 功能描述： 按照磁盘类型获取所有空闲盘信息
* 输入参数： 无
* 输出参数： 均为出参(sas和sata盘的数目，slotid和容量)
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetAllUnusedDisk(T_AllUnusedPd *ptAllUnusedPd);
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
* 功能描述： 创建池
* 输入参数： *ptCreatePool
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 CreatePool(T_CreatePool *ptCreatePool);

/**********************************************************************
* 功能描述： 删除存储池
* 输入参数： *ptPoolName
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 DelPool(T_PoolName *ptPoolName);
/**********************************************************************
* 功能描述： 查询存储池
* 输入参数： *scPoolName
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetPoolInfo(T_PoolInfo *ptPoolInfo);
/**********************************************************************
* 功能描述： 存储池上创建卷
* 输入参数： *ptCreateVolOnPool
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 CreateVolOnPool(T_CreateVolOnPool *ptCreateVolOnPool);
/**********************************************************************
* 功能描述：  查询系统中所有存储池名称列表
* 输入参数：  无
* 输出参数：  全部
* 返 回 值：  0      -   执行成功
*             非0    -   执行失败
* 其它说明：  无
***********************************************************************/
SWORD32 GetAllPoolNames(T_AllPoolNames *ptAllPoolNames);
/**********************************************************************
* 功能描述：  获取存储池上的虚拟卷名称列表
* 输入参数：  cPoolName:存储池名称
* 输出参数：  cVolNames
* 返 回 值：  0      -   执行成功
*             非0    -   执行失败
* 其它说明：  无
***********************************************************************/
SWORD32 GetVolNamesOnPool(T_VolNamesOnPool *ptAllVolNamesOnPool);
/**********************************************************************
* 功能描述：  修改存储池属性（名称、容量告警阈值、空间保留策略）
* 输入参数：  全部
* 输出参数：  无
* 返 回 值：  0      -   执行成功
*             非0    -   执行失败
* 其它说明：  无
***********************************************************************/
SWORD32 ModifyPool(T_ModifyPool *ptModifyPool);
/**********************************************************************
* 功能描述：  增加磁盘到存储池中（按磁盘数目增加、按磁盘列表增加）
* 输入参数：  全部
* 输出参数：  无
* 返 回 值：  0      -   执行成功
*             非0    -   执行失败
* 其它说明：  当按照磁盘列表修改时，结构体中的tPdId才有效
***********************************************************************/
SWORD32 AddPdToPool(T_ModifyDiskOnPool *ptModifyDiskOnPool);
/**********************************************************************
* 功能描述：  从存储池中删除磁盘（按磁盘数目删除、按磁盘列表删除）
* 输入参数：  全部
* 输出参数：  无
* 返 回 值：  0      -   执行成功
*             非0    -   执行失败
* 其它说明：  当按照磁盘列表修改时，结构体中的tPdId才有效
***********************************************************************/
SWORD32 DelPdFromPool(T_ModifyDiskOnPool *ptModifyDiskOnPool);
/**********************************************************************
* 功能描述：  存储池上的虚拟卷扩容
* 输入参数：  全部
* 输出参数：  无
* 返 回 值：  0      -   执行成功
*             非0    -   执行失败
* 其它说明：  无
***********************************************************************/
SWORD32 ExpandVolOnPool(T_ExpVolCapacity *ptExpVolCapacity);

/**********************************************************************
* 功能描述：系统自动选择磁盘创建虚拟盘
* 输入参数：ptAutoCreateVd
* 输出参数：
* 返 回 值：0      -   执行成功
*           非0    -   执行失败
* 其它说明：无
***********************************************************************/
SWORD32 AutoCreateVd(T_AutoCreateVd *ptAutoCreateVd);
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
* 功能描述：创建快照资源空间
* 输入参数：ptRvolCreate
* 输出参数：无
* 返回值：  0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 CreateRvol(T_RvolCreate *ptRvolCreate);
/**********************************************************************
* 功能描述： 删除快照资源空间
* 输入参数： ptRvolDel
* 输出参数： 无
* 返回值：   0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 DelRvol(T_RvolDel *ptRvolDel);
/**********************************************************************
* 功能描述： 获取快照资源空间信息
* 输入参数： ptRvolInfo
* 输出参数： 无
* 返回值：   0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetRvolInfo(T_RvolInfo *ptRvolInfo);
/**********************************************************************
* 功能描述： 设置快照资源空间
* 输入参数： ptRvolSet
* 输出参数： 其他为出参
* 返回值：   0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 SetRvol(T_RvolSet *ptRvolSet);
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
* 功能描述： 查看系统中所有快照卷属性
* 输入参数：
* 输出参数： 全部
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetAllSvolInfo(T_AllSvolInfo *ptAllSvolInfo);

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
* 功能描述： 获取拷贝卷信息
* 输入参数：
* 输出参数： 无
* 返回值：    0   - SUCCESS    <0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetAllCvolInfo(T_AllCvolInfo *ptAllCvolInfo);


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
* 功能描述： 创建远程镜像
* 输入参数： ptCreateMirror
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 CreateMirror(T_CreateMirror *ptCreateMirror);

/**********************************************************************
* 功能描述： 删除远程镜像
* 输入参数： ptMirrorName
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 DeleteMirror(T_MirrorName *ptMirrorName);

/**********************************************************************
* 功能描述： 显示远程镜像信息
* 输入参数： 无
* 输出参数： ptMirrorInfo
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetMirrorInfo(T_MirrorInfo *ptMirrorInfo);

/**********************************************************************
* 功能描述： 获取所有镜像名称列表
* 输入参数： 无
* 输出参数： ptAllMirrorNames
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetAllMirrorNames(T_AllMirrorNames *ptAllMirrorNames);

/**********************************************************************
* 功能描述： 修改远程镜像名称
* 输入参数： ptMirrorRename
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 SetMirrorName(T_MirrorRename *ptMirrorRename);

/**********************************************************************
* 功能描述： 修改远程镜像恢复策略
* 输入参数： ptMirrorPolicy
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 SetMirrorPolicy(T_MirrorPolicy *ptMirrorPolicy);

/**********************************************************************
* 功能描述： 修改远程镜像同步优先级
* 输入参数： ptMirrorPriority
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 SetMirrorPriority(T_MirrorPriority *ptMirrorPriority);

/**********************************************************************
* 功能描述： 修改远程镜像同步频率(仅异步镜像)
* 输入参数： ptMirrorPeriod
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 SetMirrorPeriod(T_MirrorPeriod *ptMirrorPeriod);

/**********************************************************************
* 功能描述： 停止同步远程镜像
* 输入参数： ptMirrorName
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 StopMirrorSync(T_MirrorName *ptMirrorName);

/**********************************************************************
* 功能描述： 启动同步远程镜像
* 输入参数： ptMirrorName
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 StartMirrorSync(T_MirrorName *ptMirrorName);

/**********************************************************************
* 功能描述： 设置镜像逻辑连接
* 输入参数： ptMirrorConnectInfo
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 SetFcMirrorConnect(T_FcMirrorConnectInfo *ptFcMirrorConnectInfo);

/**********************************************************************
* 功能描述： 删除镜像逻辑连接
* 输入参数： ptFcMirrorConnectInfo
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 DeleteFcMirrorConnect(T_FcMirrorConnectInfo *ptFcMirrorConnectInfo);

/**********************************************************************
* 功能描述： 查询所有镜像链接
* 输入参数： 无
* 输出参数： ptMirrorConnectList
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetAllMirrorConnect(T_MirrorConnectList *ptMirrorConnectList);

/**********************************************************************
* 功能描述： 查询所有目的卷信息
* 输入参数： 无
* 输出参数： ptRemoteDiskList
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetAllRemoteDiskInfo(T_RemoteDiskList *ptRemoteDiskList);

/**********************************************************************
* 功能描述： 查询FC 端口信息
* 输入参数： 无
* 输出参数： ptFcPortInfoList
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetFcPortInfo(T_FcPortInfoList *ptFcPortInfoList);

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
* 功能描述： VPD信息查询
* 输入参数： 无
* 输出参数： *ptVpd
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetVpd(T_VPD *ptVpd);
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
* 功能描述： 将系统信息包保存在指定路径
* 输入参数： *ptConfigFile
* 输出参数： 无
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 SaveSysInfoTarBall(T_SysInfoTarBall *ptSysInfoTarBall);

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
* 功能描述：获取所有箱体信息
* 输入参数：控制器标志--sdwCtrlFlg
* 输出参数：控制器数目--sdwCtrlNum, 诊断信息记录--tEncInfoList[2]
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetDiagAllEncInfoList(T_AllEncInfoList *ptAllEncInfoList);

/**********************************************************************
* 函数名称：GetBbuInfo
* 功能描述：获取BBU信息
* 输入参数：sdwCtrlFlg
* 输出参数：其他
* 返 回 值：0      -   执行成功
*           非0    -   执行失败
* 其它说明：无
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2012/8/16       V1.0       王京         初次创建
***********************************************************************/
SWORD32 GetBbuInfo(T_BbuInfo *ptBbuInfo);
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
* 函数名称：scsSetDiagEccSwitch
* 功能描述：
* 访问的表：
* 修改的表：
* 输入参数：
* 输出参数：
* 返 回 值：  0      -   执行成功
*                              非0 - 执行失败
* 其它说明：无
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2012/11/28              V1.0              wangwei             初次创建
***********************************************************************/
SWORD32 scsSetDiagEccSwitch(T_DiagEccSwitch *ptDiagEccSwitch);


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
* 功能描述：获取扩展柜诊断信息
* 输入参数：无
* 输出参数：诊断信息列表-- tExpanderInfo[MAX_UP_SUB_ENCL]
* 返 回 值：  0 - 执行成功
*           非0 - 执行失败
* 其它说明：无
***********************************************************************/
SWORD32 GetDiagExpanderInfoList(T_ExpanderInfoList *ptExpanderInfoList);

/**********************************************************************
* 功能描述：获取整板状态信息
* 输入参数：控制器标志--sdwCtrlFlg
* 输出参数：整板数目--sdwBoardNum, 诊断信息列表--tDiagSingleBoardStatus[2]
* 返 回 值：  0 - 执行成功
*           非0 - 执行失败
* 其它说明：无
***********************************************************************/
SWORD32 GetDiagBoardStatus(T_DiagBoardStatus *ptDiagBoardStatus);

/**********************************************************************
* 功能描述：获取磁盘状态信息
* 输入参数：无
* 输出参数：诊断磁盘数目--sdwHddNum
*           诊断信息列表--tDiagSingleHddStatus[MAX_PD_NUM_IN_SYS]
* 返 回 值：  0 - 执行成功
*           非0 - 执行失败
* 其它说明：无
***********************************************************************/
SWORD32 GetDiagHddStatus(T_DiagHddStatus *ptDiagHddStatus);

/**********************************************************************
* 功能描述：获取GE端口状态信息
* 输入参数：无
* 返 回 值：  0 - 执行成功
*           非0 - 执行失败
* 其它说明：无
***********************************************************************/
SWORD32 GetDiagGeInfo(T_DiagGeInfo *tDiagGeInfo);

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
* 功能描述： 获得EEPROM诊断信息
* 输入参数：无
* 输出参数： 全部
* 返 回 值：  0      -   执行成功
*             非0    -   执行失败
* 其它说明：无
***********************************************************************/
SWORD32 GetDiagEepromInfo(T_DiagEepromInfo *ptDiagEepromInfo);


/**********************************************************************
* 功能描述： 获得FC端口诊断信息
* 输入参数：无
* 输出参数： 全部
* 返 回 值：  0      -   执行成功
*             非0    -   执行失败
* 其它说明：无
***********************************************************************/
SWORD32 GetDiagFcInfo(T_DiagFcInfo *tDiagFcInfo);

/**********************************************************************
* 功能描述： 获得IMBC诊断信息
* 输入参数：无
* 输出参数： 全部
* 返 回 值：  0      -   执行成功
*             非0    -   执行失败
* 其它说明：无
***********************************************************************/
SWORD32 GetDiagIbmcInfo(T_DiagIbmcInfo *ptDiagIbmcInfo);

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
SWORD32 GetIscsiAllSessInfoList(T_IscsiSessInfoDualList *ptIscsiSessList);
SWORD32 GetIscsiAllSessNum(T_GetIscsiSessNum *ptIscsiAllSessNum);
SWORD32 GetIscsiSessOnceInfo(T_IscsiSessGetOnceInfo *ptIscsiSssGetOnceInfo);
SWORD32 GetIscsiAllHistorySessNum(T_GetIscsiSessNum *ptIscsiAllHisySessNum);
SWORD32 GetIscsiHisSessOnceInfo(T_IscsiSessGetOnceInfo *ptIscsiSssGetOnceInfo);

SWORD32 GetIscsiPduStatisticsInfo(T_IscsiAllPduStatisticInfo  *ptIscsiPduList);
SWORD32 GetFcGpnOnceInfo(T_GetOnceFcGpnInfo *ptFcGpnGetOnceInfo);

/**********************************************************************
* 功能描述：根据targetId ,获取所有Session 信息列表
* 输入参数：targetId
* 输出参数：
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：
***********************************************************************/
SWORD32 GetIscsiHistorySessInfoList(T_IscsiSessInfoDualList *ptIscsiHistorySessList);

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
SWORD32 GetIscsiTargetName(T_IscsiTargetList *ptIscsiTgtInfo);

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
SWORD32 GetIscsiPortInfoList(T_IscsiNetPortalList *ptIscsiPortInfo);

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
* 功能描述： 查询管理口端口信息列表
* 输入参数：无
* 输出参数：全部
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：无
***********************************************************************/
SWORD32 GetManaPortInfo(T_ManaPortInfoList *ptManaPortInfoList);

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
SWORD32 UpdatePreProcess(WORD16 ucFileType);

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
* 功能描述：上传文件后处理
* 输入参数：文件名、文件类型
* 输出参数：
* 返回值：    0   - SUCCESS   !0     -  FAILURE
* 其它说明：无
***********************************************************************/
SWORD32 UploadPostProcess(T_UploadPreInfo *ptUploadPreInfo);

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

* 功能描述：获取单板类型
* 输入参数：GetBroadType
* 输出参数：
* 返 回 值： 0      -   执行成功
*            非0    - 执行失败
* 其它说明：
***********************************************************************/
SWORD32  GetBroadType(T_SysBroadType *ptSysBroadType);

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
/**********************************************************************
* 功能描述：设置统计开关状态
* 输入参数：sdwStatFlag -- 统计开关
* 输出参数：无
* 返 回 值：0      -   执行成功
*           非0    -   执行失败
* 其它说明：
***********************************************************************/
SWORD32 SetStatAndWriteToCfg(SWORD32 sdwStatFlag);
/**********************************************************************
* 功能描述：查询统计开关状态
* 输入参数：无
* 输出参数：pStatFlag -- 统计开关
* 返 回 值：0      -   执行成功
*           非0    -   执行失败
* 其它说明：
***********************************************************************/
SWORD32 GetStatisticStatus(SWORD32 *pStatFlag);
/**********************************************************************
* 函数名称：GetCpuOccuRate
* 功能描述：查询CPU占用率
* 输入参数：无
* 输出参数：ptCpuOccupy
* 返 回 值：0      -   执行成功
*           非0    -   执行失败
* 其它说明：
***********************************************************************/
SWORD32 GetCpuOccupy(T_CpuOccupy  *ptCpuOccupy);
/**********************************************************************
* 函数名称：GetDiskStaticData
* 功能描述：查询磁盘SAS统计信息
* 输入参数：无
* 输出参数：ptSasStaticData
* 返 回 值：0      -   执行成功
*           非0    -   执行失败
* 其它说明：
***********************************************************************/
SWORD32 GetDiskStaticData(T_SasStatInfo *ptSasStatInfo);
/**********************************************************************
* 函数名称：GetPcieStaticData
* 功能描述：查询控制器内部PCIe吞吐情况
* 输入参数：无
* 输出参数：ptPcieSta
* 返 回 值：0      -   执行成功
*           非0    -   执行失败
* 其它说明：
***********************************************************************/
SWORD32 GetPcieStaticData(T_PcieFlowStatus *ptPcieSta);
/**********************************************************************
* 函数名称：GetTcsStaticData
* 功能描述：查询前端物理端口TCP/IP流量统计
* 输入参数：wSwitchFlag--统计开关
* 输出参数：ptPcieSta
* 返 回 值：0      -   执行成功
*           非0    -   执行失败
* 其它说明：
***********************************************************************/
SWORD32  GetTcsStaticData(T_TcsStatInfo *ptTcsStatInfo);
/**********************************************************************
* 功能描述：查询预设事件的cache自动直写策略
* 输入参数：无
* 输出参数：ptAutoWrThrPreSet
* 返 回 值：0      -   执行成功
*           非0    -   执行失败
* 其它说明：
***********************************************************************/
SWORD32 GetAutoWrThrEvt(T_AutoWrThrPreSet *ptAutoWrThrPreSet);
/**********************************************************************
* 功能描述：设置预设事件的cache自动直写策略
* 输入参数：ptAutoWrThrPreSet
* 输出参数：无
* 返 回 值：0      -   执行成功
*           非0    -   执行失败
* 其它说明：
***********************************************************************/
SWORD32 SetAutoWrThrEvt(T_AutoWrThrPreSet *ptAutoWrThrPreSet);

/**********************************************************************
* 功能描述：获取trap配置信息
* 输入参数：无
* 输出参数：ptSnmpTrapConfig
* 返 回 值：0      -   执行成功
*           非0    -   执行失败
* 其它说明：
***********************************************************************/
SWORD32 GetSnmpTrapConfig(T_TrapCfgList *ptSnmpTrapConfig);
/**********************************************************************
* 功能描述：添加trap配置信息
* 输入参数：ptSnmpTrapConfig
* 输出参数：无
* 返 回 值：0      -   执行成功
*           非0    -   执行失败
* 其它说明：
***********************************************************************/
SWORD32 AddSnmpTrapConfig(T_SnmpTrapConfig *ptTrapConfigAdd);
/**********************************************************************
* 功能描述：修改trap配置信息
* 输入参数：ptSnmpTrapConfig
* 输出参数：无
* 返 回 值：0      -   执行成功
*           非0    -   执行失败
* 其它说明：
***********************************************************************/
SWORD32 ModySnmpTrapConfig(T_TrapCfgMody *ptModyTrapConfig);
/**********************************************************************
* 功能描述：删除trap配置信息
* 输入参数：ServerId
* 输出参数：无
* 返 回 值：0      -   执行成功
*           非0    -   执行失败
* 其它说明：
***********************************************************************/
SWORD32 DelSnmpTrapCfg (T_TrapCfgMody * ptTrapDel);

/**********************************************************************
* 功能描述：本函数只提供给snmp调用，用于trap的发送，cli和web不可调用，另外有给cli和web提供的函数
* 输入参数：无
* 输出参数：ptSnmpTrapConfig
* 返 回 值： 0  -  执行成功
*          非0  -  执行失败
* 其它说明：无
***********************************************************************/
SWORD32 SnmpGetTrapConfig(T_TrapCfgList *ptSnmpTrapConfig);

/**********************************************************************/
/*********************MTS使用的CAPI接口*******************************/
/**********************************************************************
* 功能描述：工作EPLD自检
* 输入参数：无
* 输出参数：ptCheckWorkEpldSelf
* 返 回 值：0      -   执行成功
*           非0    -   执行失败
* 其它说明：
***********************************************************************/
SWORD32 mtsCapiCheckWorkEpldSelf(T_Mts_Check_Work_Epld_Self *ptCheckWorkEpldSelf);

/**********************************************************************
* 功能描述：工作EPLD信息读取
* 输入参数：无
* 输出参数：ptCheckWorkEpldInfo
* 返 回 值：0      -   执行成功
*           非0    -   执行失败
* 其它说明：
***********************************************************************/
SWORD32 mtsCapiCheckWorkEpldInfo(T_Mts_Check_Work_Epld_Info *ptCheckWorkEpldInfo);

/**********************************************************************
* 功能描述：启动EPLD信息读取
* 输入参数：无
* 输出参数：ptCheckBootEpldInfo
* 返 回 值：0      -   执行成功
*           非0    -   执行失败
* 其它说明：
***********************************************************************/
SWORD32 mtsCapiCheckBootEpldInfo(T_Mts_Check_Boot_Epld_Info *ptCheckBootEpldInfo);

/**********************************************************************
* 功能描述：电子条形码扫描
* 输入参数：无
* 输出参数：ptCheckLableInfo
* 返 回 值：0      -   执行成功
*           非0    -   执行失败
* 其它说明：
***********************************************************************/
SWORD32 mtsCapiCheckLableInfo(T_Mts_Check_Lable_Info *ptCheckLableInfo);

/**********************************************************************
* 功能描述：单板硬件地址读取
* 输入参数：无
* 输出参数：ptCheckPhyAddrInfo
* 返 回 值：0      -   执行成功
*           非0    -   执行失败
* 其它说明：
***********************************************************************/
SWORD32 mtsCapiCheckPhyAddrInfo(T_Mts_Check_Phy_Addr_Info *ptCheckPhyAddrInfo);

/**********************************************************************
* 功能描述：在位信号测试
* 输入参数：无
* 输出参数：ptCheckLocPreState
* 返 回 值：0      -   执行成功
*           非0    -   执行失败
* 其它说明：
***********************************************************************/
SWORD32 mtsCapiCheckLocalPreState(T_Mts_Check_Online_Flag *ptCheckLocPreState);

/**********************************************************************
* 功能描述：CPU外挂DDR3测试
* 输入参数：无
* 输出参数：ptCheckDdr
* 返 回 值：0      -   执行成功
*           非0    -   执行失败
* 其它说明：
***********************************************************************/
SWORD32 mtsCapiCheckDdr(T_Mts_Check_Ddr *ptCheckDdr);

/**********************************************************************
* 功能描述：NVRAM芯片测试
* 输入参数：无
* 输出参数：ptCheckNvram
* 返 回 值：0      -   执行成功
*           非0    -   执行失败
* 其它说明：
***********************************************************************/
SWORD32 mtsCapiCheckNvram(T_Mts_Check_Nvram *ptCheckNvram);

/**********************************************************************
* 功能描述：版本flash测试
* 输入参数：无
* 输出参数：ptCheckVerFlash
* 返 回 值：0      -   执行成功
*           非0    -   执行失败
* 其它说明：
***********************************************************************/
SWORD32 mtsCapiCheckVerFlash(T_Mts_Check_Verflash *ptCheckVerFlash);

/**********************************************************************
* 功能描述：板内时钟电路测试
* 输入参数：无
* 输出参数：ptCheckClock
* 返 回 值：0      -   执行成功
*           非0    -   执行失败
* 其它说明：
***********************************************************************/
SWORD32 mtsCapiCheckClock(T_Mts_Check_Clock *ptCheckClock);

/**********************************************************************
* 功能描述：面板指示灯测试
* 输入参数：无
* 输出参数：ptCheckLed
* 返 回 值：0      -   执行成功
*           非0    -   执行失败
* 其它说明：
***********************************************************************/
SWORD32 mtsCapiCheckLed(T_Mts_Check_Led *ptCheckLed);

/**********************************************************************
* 功能描述：看门狗复位测试
* 输入参数：无
* 输出参数：ptCheckWtg
* 返 回 值：0      -   执行成功
*           非0    -   执行失败
* 其它说明：
***********************************************************************/
SWORD32 mtsCapiCheckWtg(T_Mts_Check_Wtg *ptCheckWtg);

/**********************************************************************
* 功能描述：温度传感器
* 输入参数：无
* 输出参数：ptCheckTemp
* 返 回 值：0      -   执行成功
*           非0    -   执行失败
* 其它说明：
***********************************************************************/
SWORD32 mtsCapiCheckSenor(T_Mts_Check_Senor *ptCheckSenor);

/**********************************************************************
* 功能描述：RTC实时时钟电路测试
* 输入参数：无
* 输出参数：ptCheckRtc
* 返 回 值：0      -   执行成功
*           非0    -   执行失败
* 其它说明：
***********************************************************************/
SWORD32 mtsCapiCheckRtc(T_Mts_Check_Rtc *ptCheckRtc);

/**********************************************************************
* 功能描述：面板GE口环回测试
* 输入参数：无
* 输出参数：ptCheckGe
* 返 回 值：0      -   执行成功
*           非0    -   执行失败
* 其它说明：
***********************************************************************/
SWORD32 mtsCapiCheckGe(T_Mts_Check_Ge *ptCheckGe);


/**********************************************************************
* 功能描述：面板调试网口作为通信链路测试
* 输入参数：无
* 输出参数：ptCheckConnectLink
* 返 回 值：0      -   执行成功
*           非0    -   执行失败
* 其它说明：
***********************************************************************/
SWORD32 mtsCapiCheckConnectLink(T_Mts_Check_Conn_Link *ptCheckConnLink);


/**********************************************************************
* 功能描述：Cache同步信号测试/PCIE芯片桥自检
* 输入参数：无
* 输出参数：ptCheckPcieInfo
* 返 回 值：0      -   执行成功
*           非0    -   执行失败
* 其它说明：
***********************************************************************/
SWORD32 mtsCapiCheckPcie(T_Mts_Check_Pcie_Info *ptCheckPcieInfo);

/**********************************************************************
* 功能描述：phy芯片自检
* 输入参数：无
* 输出参数：ptPhySelf
* 返 回 值：0      -   执行成功
*           非0    -   执行失败
* 其它说明：
***********************************************************************/
SWORD32 mtsCapiCheckPhySelf(T_Mts_Check_Phyself *ptPhySelf);

/**********************************************************************
* 功能描述：PM8001自检
* 输入参数：无
* 输出参数：ptPmSelf
* 返 回 值：0      -   执行成功
*           非0    -   执行失败
* 其它说明：
***********************************************************************/
SWORD32 mtsCapiCheckPmSelf(T_Mts_Check_Pm *ptPmSelf);

/**********************************************************************
* 功能描述：输入IO信号指示灯测试
* 输入参数：无
* 输出参数：ptInputIo
* 返 回 值：0      -   执行成功
*           非0    -   执行失败
* 其它说明：
***********************************************************************/
SWORD32  mtsCapiCheckInputIo(T_Mts_Check_InputIo *ptInputIo);

/**********************************************************************
* 功能描述：输出IO信号指示灯测试
* 输入参数：无
* 输出参数：ptOutputIo
* 返 回 值：0      -   执行成功
*           非0    -   执行失败
* 其它说明：
***********************************************************************/
SWORD32  mtsCapiCheckOutputIo(T_Mts_Check_OutputIo *ptOutputIo);

/**********************************************************************
* 功能描述：电源信号指示灯测试
* 输入参数：无
* 输出参数：ptPowerLight
* 返 回 值：0      -   执行成功
*           非0    -   执行失败
* 其它说明：
***********************************************************************/
SWORD32  mtsCapiCheckPowerLight(T_Mts_Check_Power_Light *ptPowerLight);

/**********************************************************************
* 功能描述：获取BBU信息
* 输入参数：无
* 输出参数：ptMtsCheckBbu
* 返 回 值：0      -   执行成功
*           非0    -   执行失败
* 其它说明：
***********************************************************************/
SWORD32 mtsCapiGetWorkBbuInfo(T_Mts_Check_Bbu *ptCheckBbu);

/**********************************************************************
* 函数名称：mtsCapiEzzfReset
* 功能描述：8005看门狗复位测试
* 输入参数：无
* 输出参数：ptMtsCheckBbu
* 返 回 值： 0  -  执行成功
*          非0  -  执行失败
* 其它说明：无
***********************************************************************/
SWORD32 mtsCapiEzzfReset(T_Mts_Check_Ezzf_Reset *ptMtsEzzfReset);


/**********************************************************************
* 函数名称：mtsCapiEzzfGetSpeed
* 功能描述：minisas接口自环测试
* 输入参数：无
* 输出参数：ptMtsCheckBbu
* 返 回 值： 0  -  执行成功
*          非0  -  执行失败
* 其它说明：无
***********************************************************************/
SWORD32 mtsCapiEzzfGetSpeed(T_Mts_Check_Ezzf_GetSpeed *ptMtsEzzfGetSpeed);

/**********************************************************************
* 函数名称：mtsCapiEzzfBbGetSpeed
* 功能描述：背板SAS接口自环测试
* 输入参数：无
* 输出参数：ptMtsCheckBbu
* 返 回 值： 0  -  执行成功
*          非0  -  执行失败
* 其它说明：无
***********************************************************************/
SWORD32 mtsCapiEzzfBbGetSpeed(T_Mts_Check_Ezzf_BdGetSpeed *ptMtsEzzfBdGetSpeed);

/**********************************************************************
* 函数名称：mtsCapiEzzfTtSigTest
* 功能描述：TW0\TW1\TW2信号测试
* 输入参数：无
* 输出参数：ptMtsCheckBbu
* 返 回 值： 0  -  执行成功
*          非0  -  执行失败
* 其它说明：无
***********************************************************************/
SWORD32 mtsCapiEzzfTtSigTest(T_Mts_Check_Ezzf_Ttsig *ptMtsEzzfTtsig);

/**********************************************************************
* 函数名称：mtsCapiEzzfSasSelf
* 功能描述：SAS扩展芯片PM8005自检
* 输入参数：无
* 输出参数：ptMtsCheckBbu
* 返 回 值： 0  -  执行成功
*          非0  -  执行失败
* 其它说明：无
***********************************************************************/
SWORD32 mtsCapiEzzfSasSelf(T_Mts_Check_Ezzf_Self *ptMtsEzzfSelf);

/**********************************************************************
* 函数名称：mtsCapiHddLight
* 功能描述：硬盘指示灯测试
* 输入参数：无
* 输出参数：ptMtsCheckBbu
* 返 回 值： 0  -  执行成功
*          非0  -  执行失败
* 其它说明：无
***********************************************************************/
SWORD32 mtsCapiHddLight(T_Mts_Check_Hdd_Light *ptMtsHddLight);


/**********************************************************************
* 函数名称：mtsCapiDdrSize
* 功能描述：内存大小检测
* 输入参数：无
* 输出参数：ptMtsDdrSize
* 返 回 值： 0  -  执行成功
*          非0  -  执行失败
* 其它说明：无
***********************************************************************/
SWORD32 mtsCapiDdrSize(T_Mts_Check_Ddr_Size *ptMtsDdrSize);

/**********************************************************************
* 函数名称：mtsCapiCheckVer
* 功能描述：检测版本
* 输入参数：无
* 输出参数：ptMtsDdrSize
* 返 回 值： 0  -  执行成功
*          非0  -  执行失败
* 其它说明：无
***********************************************************************/
SWORD32 mtsCapiCheckVer(T_Mts_Check_Ver *ptMtsVer);

/**********************************************************************
* 函数名称：mtsCapiCheckMacAddr
* 功能描述：MAC地址获取
* 输入参数：无
* 输出参数：ptMtsMacAddr
* 返 回 值： 0  -  执行成功
*          非0  -  执行失败
* 其它说明：无
***********************************************************************/
SWORD32 mtsCapiCheckMacAddr(T_Mts_Check_MacAddr *ptMtsMacAddr);

/*****************************整机测试*********************************/
/**********************************************************************
* 函数名称：cmtCapiWorkEpldSelf
* 功能描述：工作EPLD整机测试
* 输入参数：无
* 输出参数：ptMtsWorkEpldSelf
* 返 回 值： 0  -  执行成功
*          非0  -  执行失败
* 其它说明：无
***********************************************************************/
SWORD32 cmtCapiWorkEpldSelf(T_WorkEpldSelfList *ptCmtWorkEpldSelf);

/**********************************************************************
* 函数名称：cmtCapiWorkEpldInfo
* 功能描述：工作EPLD信息读取
* 输入参数：无
* 输出参数：ptCmtWorkEpldInfo
* 返 回 值： 0  -  执行成功
*          非0  -  执行失败
* 其它说明：无
***********************************************************************/
SWORD32 cmtCapiWorkEpldInfo(T_WorkEpldInfoList *ptCmtWorkEpldInfo);

/**********************************************************************
* 函数名称：cmtCapiBootEpldInfo
* 功能描述：启动EPLD整机测试信息获取
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：ptMtsWorkEpldSelf
* 返 回 值： 0  -  执行成功
*          非0  -  执行失败
* 其它说明：无
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2012/05/12      V1.0       wangwei      初次创建
***********************************************************************/
SWORD32 cmtCapiBootEpldInfo(T_BootEpldInfoList *ptCmtBootEpldInfo);

/**********************************************************************
* 函数名称：cmtCapiLableInfo
* 功能描述：电子标签信息
* 输入参数：无
* 输出参数：ptMtsWorkEpldSelf
* 返 回 值： 0  -  执行成功
*          非0  -  执行失败
* 其它说明：无
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2012/05/12      V1.0       wangwei      初次创建
***********************************************************************/
SWORD32 cmtCapiLableInfo(T_LableInfoList *ptCmtLableInfo);

/**********************************************************************
* 函数名称：cmtCapiPhyAddrInfo
* 功能描述：单板硬件地址读取
* 输入参数：无
* 输出参数：ptMtsWorkEpldSelf
* 返 回 值： 0  -  执行成功
*          非0  -  执行失败
* 其它说明：无
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2012/05/12      V1.0       wangwei      初次创建
***********************************************************************/
SWORD32 cmtCapiPhyAddrInfo(T_PhyAddrInfoList *ptCmtPhyAddrInfo);

/**********************************************************************
* 函数名称：cmtCapiOnlineFlagInfo
* 功能描述：在位信号测试
* 输入参数：无
* 输出参数：ptMtsWorkEpldSelf
* 返 回 值： 0  -  执行成功
*          非0  -  执行失败
* 其它说明：无
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2012/05/12      V1.0       wangwei      初次创建
***********************************************************************/
SWORD32 cmtCapiOnlineFlagInfo(T_OnlineFlagInfoList *ptCmtOnlieFlagInfo);

/**********************************************************************
* 函数名称：cmtCapiClockInfo
* 功能描述：板内时钟电路测试
* 输入参数：无
* 输出参数：ptMtsWorkEpldSelf
* 返 回 值： 0  -  执行成功
*          非0  -  执行失败
* 其它说明：无
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2012/05/12      V1.0       wangwei      初次创建
***********************************************************************/
SWORD32 cmtCapiClockInfo(T_ClockInfoList *ptCmtClockInfo);

/**********************************************************************
* 函数名称：cmtCapiSenorInfo
* 功能描述：传感器信息获取
* 输入参数：无
* 输出参数：ptMtsWorkEpldSelf
* 返 回 值： 0  -  执行成功
*          非0  -  执行失败
* 其它说明：无
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2012/05/12      V1.0       wangwei      初次创建
***********************************************************************/
SWORD32 cmtCapiSenorInfo(T_SenorInfoList *ptCmtSenorInfo);
SWORD32 cmtCapiExpInfo(T_MtsEncDeviceInfoList *ptCmtExpInfo);
SWORD32 cmtCapiDiskInfo(T_MtsDiskInfo *ptCmtDiskInfo);

/**********************************************************************
* 函数名称：cmtCapiRtcInfo
* 功能描述：RTC实时时钟电路测试
* 输入参数：无
* 输出参数：ptMtsWorkEpldSelf
* 返 回 值： 0  -  执行成功
*          非0  -  执行失败
* 其它说明：无
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2012/05/12      V1.0       wangwei      初次创建
***********************************************************************/
SWORD32 cmtCapiRtcInfo(T_RtcInfoList *ptCmtRtcInfo);


/**********************************************************************
* 函数名称：cmtCapiGeInfo
* 功能描述：GE口链路速率，是否双控测试
* 输入参数：无
* 输出参数：ptMtsWorkEpldSelf
* 返 回 值： 0  -  执行成功
*          非0  -  执行失败
* 其它说明：无
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2012/05/12      V1.0       wangwei      初次创建
***********************************************************************/
SWORD32 cmtCapiGeInfo(T_GeInfoList *ptCmtGeInfo);

/**********************************************************************
* 函数名称：cmtCapiPcieInfo
* 功能描述：PCIE测试
* 输入参数：无
* 输出参数：ptMtsWorkEpldSelf
* 返 回 值： 0  -  执行成功
*          非0  -  执行失败
* 其它说明：无
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2012/05/12      V1.0       wangwei      初次创建
***********************************************************************/
SWORD32 cmtCapiPcieInfo(T_PcieInfoList *ptCmtPcieInfo);

/**********************************************************************
* 函数名称：cmtCapiPhyInfo
* 功能描述：PHY自检
* 输入参数：无
* 输出参数：ptMtsWorkEpldSelf
* 返 回 值： 0  -  执行成功
*          非0  -  执行失败
* 其它说明：无
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2012/05/12      V1.0       wangwei      初次创建
***********************************************************************/
SWORD32 cmtCapiPhyInfo(T_PhyInfoList *ptCmtPhyInfo);


/**********************************************************************
* 函数名称：cmtCapiPmInfo
* 功能描述：SAS控制器PM8001自检
* 输入参数：无
* 输出参数：ptCmtPmInfo
* 返 回 值： 0  -  执行成功
*          非0  -  执行失败
* 其它说明：无
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2012/05/12      V1.0       wangwei      初次创建
***********************************************************************/
SWORD32 cmtCapiPmInfo(T_PmInfoList *ptCmtPmInfo);

/**********************************************************************
* 函数名称：cmtCapiBbuInfo
* 功能描述：BBU检测
* 输入参数：无
* 输出参数：ptCmtBbuInfo
* 返 回 值： 0  -  执行成功
*          非0  -  执行失败
* 其它说明：无
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2012/05/12      V1.0       wangwei      初次创建
***********************************************************************/
SWORD32 cmtCapiBbuInfo(T_BbuInfoList *ptCmtBbuInfo);

/**********************************************************************
* 函数名称：cmtCapiSasInfo
* 功能描述：MINISAS和背部SAS接口测试
* 输入参数：无
* 输出参数：ptCmtSasInfo
* 返 回 值： 0  -  执行成功
*          非0  -  执行失败
* 其它说明：无
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2012/05/12      V1.0       wangwei      初次创建
***********************************************************************/
SWORD32 cmtCapiSasInfo(T_SasInfoList *ptCmtSasInfo);

/**********************************************************************
* 函数名称：cmtCapiHddInfo
* 功能描述：HardDisk 自检
* 输入参数：无
* 输出参数：ptCmtHddInfo
* 返 回 值： 0  -  执行成功
*          非0  -  执行失败
* 其它说明：无
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2012/05/12      V1.0       wangwei      初次创建
***********************************************************************/
SWORD32 cmtCapiHddInfo(T_HddInfoList *ptCmtHddInfo);

/**********************************************************************
* 函数名称：cmtCapiVerInfo
* 功能描述：版本检测
* 输入参数：无
* 输出参数：cmtCapiVerInfo
* 返 回 值： 0  -  执行成功
*          非0  -  执行失败
* 其它说明：无
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2012/05/12      V1.0       wangwei      初次创建
***********************************************************************/
SWORD32 cmtCapiVerInfo(T_VerInfoList *ptCmtVerInfo);

/**********************************************************************
* 函数名称：cmtCapiDdrInfo
* 功能描述：内存检测
* 输入参数：无
* 输出参数：ptMtsWorkEpldSelf
* 返 回 值： 0  -  执行成功
*          非0  -  执行失败
* 其它说明：无
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2012/05/12      V1.0       wangwei      初次创建
***********************************************************************/
SWORD32 cmtCapiDdrInfo(T_DdrInfoList *ptCmtDdrInfo);

/**********************************************************************
* 函数名称：cmtCapiMacAddrInfo
* 功能描述：MAC地址
* 输入参数：无
* 输出参数：ptMtsWorkEpldSelf
* 返 回 值： 0  -  执行成功
*          非0  -  执行失败
* 其它说明：无
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2012/05/12      V1.0       wangwei      初次创建
***********************************************************************/
SWORD32 cmtCapiMacAddrInfo(T_MacAddrInfoList *ptCmtMacAddrInfo);

/**********************************************************************
* 函数名称：cmtCapiPeerStateInfo
* 功能描述：测试环境检测
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：ptMtsWorkEpldSelf
* 返 回 值： 0  -  执行成功
*          非0  -  执行失败
* 其它说明：无
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2012/05/12      V1.0       wangwei      初次创建
***********************************************************************/
SWORD32 cmtCapiPeerStateInfo(T_PeerStateInfoList *ptCmtPeerStateInfo);

/**********************************************************************
* 函数名称：cmtCapiSasAddrInfo
* 功能描述：SAS地址检测,即8005自检
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：ptMtsWorkEpldSelf
* 返 回 值： 0  -  执行成功
*          非0  -  执行失败
* 其它说明：无
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2012/05/12      V1.0       wangwei      初次创建
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
* 函数名称：CollectSysInfo
* 功能描述：系统信息采集
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：ptMtsWorkEpldSelf
* 返 回 值： 0  -  执行成功
*          非0  -  执行失败
* 其它说明：无
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2012/06/12      V1.0       姚冬      初次创建
***********************************************************************/
SWORD32 CollectSysInfo(T_SicCtrlAction *ptCtrlAction);

/**********************************************************************
* 函数名称：GetTarBallInfo
* 功能描述：系统信息采集压缩包信息获取
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：ptMtsWorkEpldSelf
* 返 回 值： 0  -  执行成功
*          非0  -  执行失败
* 其它说明：无
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2012/06/12      V1.0       姚冬      初次创建
***********************************************************************/
SWORD32 GetTarBallInfo(T_TarBallInfoList *ptTarBallInfoList);

/**********************************************************************
* 函数名称：SetSicEmailCfg
* 功能描述：设置系统信息采集功能邮件发送配置
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：ptMtsWorkEpldSelf
* 返 回 值： 0  -  执行成功
*          非0  -  执行失败
* 其它说明：无
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2012/06/12      V1.0       姚冬      初次创建
***********************************************************************/
SWORD32 SetSicEmailCfg(T_EmailCfg *ptEmailCfg);

/**********************************************************************
* 函数名称：GetSicEmailCfg
* 功能描述：获取系统信息采集功能邮件发送配置
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：ptMtsWorkEpldSelf
* 返 回 值： 0  -  执行成功
*          非0  -  执行失败
* 其它说明：无
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2012/06/12      V1.0       姚冬      初次创建
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
/* 以下为fc会话呈现提供的接口函数 */
/**********************************************************************
* 函数名称：GetFCProtocalInfo
* 功能描述：获取FC会话协议信息
* 访问的表：无
* 修改的表：无
* 输入参数：CtrlId,PortId
* 输出参数：协议信息
* 返 回 值： 0  -  执行成功
*          非0  -  执行失败
* 其它说明：无
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2012/08/30      V1.1      qinping      初次创建
***********************************************************************/
SWORD32 GetFCProtocalInfo(T_GetFCProtocolIInfo *ptFcProtocalInfo);
/**********************************************************************
* 函数名称：GetFCProtocalInfo
* 功能描述：获取FC会话统计信息
* 访问的表：无
* 修改的表：无
* 输入参数：CtrlId,PortId
* 输出参数：协议信息
* 返 回 值： 0  -  执行成功
*          非0  -  执行失败
* 其它说明：无
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2012/08/30      V1.1      qinping      初次创建
***********************************************************************/
SWORD32 GetFCPortStatisticInfo(T_FCGetPortStatisInfo *ptFcPortStatisInfo);
/**********************************************************************
* 函数名称：GetFCProtocalInfo
* 功能描述：清除FC会话统计信息
* 访问的表：无
* 修改的表：无
* 输入参数：CtrlId,PortId
* 输出参数：协议信息
* 返 回 值： 0  -  执行成功
*          非0  -  执行失败
* 其它说明：无
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2012/08/30      V1.1      qinping      初次创建
***********************************************************************/
SWORD32 ClearFCPortStatisticInfo(T_ClearFCPortStatisInfo *ptClearFcPortStatisInfo);

/**********************************************************************
* 函数名称：GetFCProtocalInfo
* 功能描述：获取FC会话统计信息，提供给web调用
* 访问的表：无
* 修改的表：无
* 输入参数：CtrlId,PortId
* 输出参数：协议信息
* 返 回 值： 0  -  执行成功
*          非0  -  执行失败
* 其它说明：无
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2012/08/30      V1.1      qinping      初次创建
***********************************************************************/
SWORD32 GetFCPortStatisticInfoForWeb(T_WebGetFCPortStatisInf *ptFcPortStatisInfo);

/**********************************************************************
* 函数名称：GetAlarmInfo
* 功能描述：分包获取告警信息
* 访问的表：无
* 修改的表：无
* 输入参数：T_Alarm_Req *ptAlarmReq
* 输出参数：T_Alarm_Packet_Ack *ptAlarmAck
* 返 回 值： 0  -  执行成功
*          非0  -  执行失败
* 其它说明：无
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2012/11/11     V1.1      yangcl      初次创建
***********************************************************************/
SWORD32 GetAlarmInfo(T_Alarm_Req *ptAlarmReq,T_Alarm_Packet_Ack *ptAlarmAck);

/**********************************************************************
* 函数名称：  DismissAlarmInfo
* 功能描述：解除告警
* 输入参数：T_Alarm_Del_Alarm *ptDelAlarmReq
* 输出参数：T_Alarm_Del_Alarm_Ack *ptDelAlarmAck
* 返 回 值：  0      -   执行成功
*             非0    -   执行失败
* 其它说明：无
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2012/11/11     V1.1      yangcl      初次创建
***********************************************************************/
SWORD32 DismissAlarmInfo(T_Alarm_Del_Alarm *ptDelAlarmReq,T_Alarm_Del_Alarm_Ack *ptDelAlarmAck);

/**********************************************************************
* 函数名称：  GetUspUpdateChkStatus
* 功能描述：获取USP更新前是否具备更新条件
* 输入参数：T_UpdateUspChk *ptUpdateUspChk
* 输出参数：T_UpdateUspChk *ptUpdateUspChk
* 返 回 值：  0      -   执行成功
*             非0    -   执行失败
* 其它说明：无
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/03/12     V1.1      huangan      初次创建
***********************************************************************/
SWORD32 GetUspUpdateChkStatus(T_UpdateUspChk *ptUpdateUspChk);
#endif
