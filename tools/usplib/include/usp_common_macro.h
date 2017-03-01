/*********************************************************************
* 版权所有 (C)2010, 深圳市中兴通讯股份有限公司。
*
* 文件名称： usp_capi_micro.h
* 文件标识： 见配置管理计划书
* 内容摘要： USP项目宏定义
* 其它说明： 无
* 当前版本： 0.1
* 作    者： tao.linjun@zte.com.cn
* 完成日期： 2010-11-2
**********************************************************************/

#ifndef __USP_CAPI_MICRO_H__
#define __USP_CAPI_MICRO_H__

#define MAX_ENC_NUM_IN_SYS          MAX_UP_SUB_ENCL  /* 上下支持级联个数 */
#define MAX_PD_NUM_IN_ENC           16
#define MAX_PD_NUM_IN_SYS           132              /* 适应SPR11支持级联10级扩展柜最多支持132块磁盘而做的修改*/
#define MAX_LUN_NUM_IN_SYS          1024
#define MAX_MAP_GROUP_NUM_IN_SYS    16
#define MAX_INI_NUM_IN_MAP_GROUP    8
#define MAX_LUN_NUM_IN_MAP_GROUP    256
#define MAX_MAP_GRP_NUM_LUN_IN      10
#define MAX_MAP_GRP_NUM_LUN_ADDED    8          /* LUN 允许加入的最大映射组数目 */

#define MAX_VERSION_LEN             58
#define MAX_CONFIG_FILE_NAME_LEN    52

#define MAX_LOG_LEN                 256    /* 日志内容描述的最大长度 */
#define MAX_LOG_NUM                 1000
#define MAX_LOG_NUM_ONCE            50     /* 每次向SMA提供的日志数目 */
#define MAX_LOG_EXPORT_NAME_LEN     58     /* 导出日志文件的最大长度 */

#define MAX_IP_LEN                  16
#define MAX_LINE_LEN                80     /* 按行读取配置文件 */
#define NET_NAME_MAX                12     /* 网口设备名称长度 */

/* CACHE 写回写穿策略 */
#define CACHE_WRITE_BACK_POLICY     0
#define CACHE_WRITE_THROUGH_POLICY  1

/* CACHE策略是否写内存标志 */
#define CACHE_WRITE_TO_MEMORY          0      /* 写内存 */
#define CACHE_NOT_WRITE_MEMORY         1      /* 不写内存 */

/*取决于DM*/
//#define MAX_PD_GUID_LEN            4
#define MAX_PD_MODEL_NAME_LEN       42
#define MAX_PD_TYPE_LEN             8
#define MAX_PD_FW_VER_LEN           10
#define MAX_PD_SNO_LEN              22
#define MAX_PD_STD_VER_LEN          4



/* 元数据相关的宏定义，BLK, DM 共同使用 */
#define SB_QUANTA       1024         /* KB */   /* 将磁盘的原始容量按1M 对齐 */
#define BLK_SB_SECTION  32*1024     /* KB */   /* BLK 元数据区大小32M */
#define SPARE_SB_OFFSET  256*1024   /* KB */   /* 备用元数据区起始地址 */
#define USC_RESERVED     512*1024   /* KB */   /* 保留512M用于存放元数据信息和坏块信息 */
#define DM_SB_SIZE       1024       /* KB */   /* DM元数据区所占的大小 */


/*取决于Blk*/

#define MAJOR_VOL  18
#define MAJOR_VD        17
#define MAJOR_SNAP  's'
#define MAJOR_REPOSITORY  'r'

#define VDKCDEV_MAJOR 52
#define VDKCDEV_MINOR   0

#define MIRROR_CDEV_MAJOR 53
#define MIRROR_CDEV_MINOR  0
/* 存储池添加开始 */
#define POOLS_MAJOR  68
#define POOLS_MINOR  8

#define MAX_DISKS_IN_POOL             256
#define MIN_DISKS_IN_POOL             8
#define MAX_POOL_NUM_IN_SYS           8      /*//< 系统中最大存储池数目*/
#define MAX_VOL_NUM_ON_POOL           1024

/*修改存储池属性时不需要修改的参数*/
#define NO_MODIFY -1

/* 存储池空间保留策略*/
#define POOL_CAPACITY_POLITY_NO_RESERVE           1        //不保留
#define POOL_CAPACITY_POLITY_RESERVE_ONE          2        //保留一块磁盘的空间
#define POOL_CAPACITY_POLITY_RESERVE_TWO          3        //保留两块磁盘的空间

/*存储池容量最大告警阈值*/
#define MAX_POOL_ALARM_THR     50

/* 用于判断磁盘创建了vd 或 pool */
#define TYPE_VD                     1         //磁盘创建了vd
#define TYPE_POOL                   2         //磁盘创建了pool

/* 用于判断vol的所属，属于vd 或 pool */
#define VOL_IN_VD                  VOL_OWNED_BY_VD    //vol属于vd，取值1
#define VOL_IN_POOL                VOL_OWNED_BY_POOL  //vol属于pool，取值2

/* 存储池修改磁盘类型 */
#define ADD_DISK_TO_POOL           1       //增加磁盘到存储池中
#define DEL_DISK_FROM_POOL         2       //从存储池中删除磁盘

/* pool的修改标记 */
#define POOL_MODIFY_ATTR_NAME       1
#define POOL_MODIFY_ATTR_ALARM_THR  2
#define POOL_MODIFY_ATTR_CAPA_POL   3

/* 创建存储池的方式 */
#define  POOL_CREATE_BY_DISK_NUM                  1         //按磁盘数目创建存储池
#define  POOL_CREATE_BY_DISK_LIST                 2         //按磁盘信息列表创建存储池

/* 修改存储池的磁盘方式 */
#define  POOL_MODIFY_DISK_BY_NUM                  1         //按磁盘数目修改存储池的成员盘
#define  POOL_MODIFY_DISK_BY_LIST                 2         //按磁盘信息列表修改存储池的成员盘
/*存储池的状态*/

#define  POOL_DAMAGED         0                  /*存储池数据损坏*/
#define  POOL_GOOD            1                          /*存储池正常可用*/
#define  POOL_REDISTRIBUTING  2          /*存储池在重分布中*/
#define  POOL_INSUFCAPACITY   3          /*存储池容量不满足条件或者磁盘数小于8*/

#define  MAX_UNUSED_PD        MAX_PD_NUM_IN_SYS /* 与系统中最大磁盘数保持一致 */
/*存储池中允许最大的拔盘数*/
#define MAX_RAID0_UNPLUG_DISKS  0
#define MAX_RAIDELSE_UNPLUG_DISKS  1
#define MAX_RAID6_UNPLUG_DISKS  2
/* 存储池添加结束 */

#define MAX_VD_NUM_IN_SYS           32      ///< 系统中最大虚拟盘数
#define MAX_PD_NUM_IN_VD            32      ///< 虚拟盘中最大物理磁盘数
#define MIN_PD_NUM_IN_VD            1       ///< 虚拟盘中最小物理磁盘数
#define MAX_BLK_NAME_LEN            32
#define MAX_VD_IMPORT_PD_NUM        2       ///< 虚拟盘最大允许导入盘数目
#define MAX_VD_EXPORT_PD_NUM        2       ///< 虚拟盘最大允许导出盘数目
#define MAX_PD_NUM_ADD              4       ///< 虚拟盘每次最多增加的物理磁盘数目
#define MAX_VOL_NUM_ON_VD           32
#define MAX_NUM_ONEVOLTYPE_ON_VD           128 /*vd上指定类型的卷最多为128个*/

#define MAX_ALLTYPE_VOL_NUM_ON_VD        (MAX_NUM_ONEVOLTYPE_ON_VD*2+MAX_VOL_NUM_ON_VD)  /*vd上指定类型的各种类型的卷最多为128+128+32个*/

/********** 任务类型 add by yaodong *********/
#define VD_DISK_DEFRAG_TYPE         (WORD32)0  /* 虚拟盘磁盘碎片整理 */
#define VD_RECOVERY_TYPE            (WORD32)1  /* 虚拟盘恢复（重建） */
#define VD_EXPAND_TYPE              (WORD32)2  /* 虚拟盘扩展 */
#define VD_RELEVEL_TYPE             (WORD32)3  /* 虚拟盘变级 */
#define VOL_EXPAND_TYPE             (WORD32)4  /* 卷扩容 */
#define VOL_CHUNK_CHANGE_TYPE       (WORD32)5  /* 变条带 */
#define CLONE_INIT_TYPE             (WORD32)6  /* 拷贝卷初始化 */
#define CLONE_SYNC_TYPE             (WORD32)7  /* 拷贝卷同步 */
#define CLONE_RSYNC_TYPE            (WORD32)8  /* 拷贝卷反同步 */
#define DISK_SCAN_TYPE              (WORD32)9  /* 磁盘扫描 */
/* add by huangan */
#define FILE_UPLOAD_TYPE            (WORD32)10   /* 文件上传 */
#define MIRROR_SYNC_TYPE            (WORD32)11   /* 远程镜像同步 */
#define DATA_REDISTRIBUTE_TYPE      (WORD32)12 /*数据重分布*/
#define SNAP_RESTORE_TYPE           (WORD32)13  /* 源卷数据回滚 */
#define SYS_MSG_COLLECT_TYPE        (WORD32)14

/* for vol */
#define MAX_VOL_ID                  32
#define MIN_VD_ID                   0
#define MAX_VD_ID                   32
#define MIN_CHUNK_SIZE              4
#define MAX_CHUNK_SIZE              512

#define MAX_VOL_NUM_IN_SYS         (MAX_VOL_NUM_ON_VD *  MAX_VD_NUM_IN_SYS)         //xygang 2010-11-19

/* for snapshot*/
#define MAX_SVOL_NUM_IN_SYS         128     ///< 系统中最大快照数
#define MAX_SVOL_NUM_IN_VOL         8      ///< 卷中最大快照数
#define MAX_COW_CHUNKSIZE           (2*1024)  /*cow_chunk最大为2*1024KB*/

/* for clone */
#define MAX_CLONE_IN_VOL           8
#define MAX_CLONE_IN_SYS         128
#define MIN_PRIVATE_CAP            4  /* 私有卷的最小容量 一个page 4KB */

/* 同步或反同步的优先级*/
#define SYNC_LOW     0
#define SYNC_MEDIUM  1
#define SYNC_HIGH    2

#define CLONE_INIT          0  /* 初始化状态, 创建clone 后的状态, 后台写0或写1*/
#define CONSISTENT          1  /* 拷贝卷和源卷有一致性数据, 保持一致性*/
#define SYNCING             2  /* 正在进行同步*/
#define REVERSE_SYNCING     3  /* 正在进行反同步*/
#define SYNCED              4  /* 拷贝卷和源卷是同步的, 数据完成一致 */
#define OUT_OF_SYNC         5  /* 同步时失败*/
#define OUT_OF_REVERSE_SYNC 6  /* 反同步时失败*/
#define OUT_OF_INIT         7  /*  初始化失败*/


/* 源卷和clone卷的关联状态 */
#define FRACTURED   0       /* 分离 */
#define UNFRACTURED 1       /* 关联 */

#define NOT_INIT_SYNC   0     /* 不做初始同步 */
#define IS_INIT_SYNC    1     /* 做初始同步 */

#define MANUAL_POLICY   0     /* 手动恢复策略 */
#define AUTO_POLICY     1     /* 自动恢复策略 */

/* for MapGrp*/
#define MAX_MAPGRP_NAME_LEN         32

/*BLK对象中是否存在任务 */
#define BLK_TASK_EXIST      1    /* 任务存在标记  */

/*************** used by iSCSI/SCS/CLI,iSCSI负责定义 ****************************/
#define MAX_IQN_LEN                 ((WORD32)223)    /* 依据协议最大支持iqn长度(同时适用于initiator端和target端) */
#define MAX_TGT_NAME_LEN_OLD        ((WORD32)64)     /* 最大支持targetname长度 */
#define MAX_TGT_NAME_LEN            ((WORD32)224)     /* 最大支持targetname长度 */
#define MAX_INI_NAME_LEN_OLD        ((WORD32)128)    /* 最大支持initiator名字长度 */
#define MAX_INI_NAME_LEN            ((WORD32)224)     /* 最大支持initiator长度 */

#define MAX_REGISTER_INI_NAME_LEN   (WORD32)(MAX_INI_NAME_LEN + 3 + 15 + 1)
#define MAX_REGISTER_TGT_NAME_LEN   (WORD32)(MAX_TGT_NAME_LEN + 3 + 5 + 1)
/* 不可以为256;当为256时，tcs_common.h中的结构T_IscsiTidInfo会变的太大(19548)，在使用_IO转换IOCTLNUM时失败(size有14bit,16384);
   编译时会报错 " case label does not reduce to an integer constant " */
#define MAX_INITIATOR_NUM           ((WORD32)32)    /* 最大支持initiator个数 */
#define ISCSI_MAX_REC_INITIATOR_NUM ((WORD32)32)    /* 最大支持记录下来的initiator个数 */
#define ISCSI_MAX_REC_SESSION_NUM   ((WORD32)1024)    /* 最大支持查看的Session个数 */
#if 0
#define CHAP_ACCOUNT_NAME_MAX       32  /* 暂未使用 */
#define CHAP_ACCOUNT_PASS_MAX       14  /* 暂未使用 */
#endif

#define MAX_ISCSI_PORT_NUM_PER_CTL  6
#define MAX_ISCSI_SESSION_COUNT     1024
#define MAX_ISCSI_SESSION_NUM_ONCE  10//5/* 每次获取会话个数 */
#define MAX_FC_GPN_NUM_ONCE  60//5/* 每次获取FC WWPN个数 */
enum
{
    key_initial_r2t,          /* 0  */
    key_immediate_data,       /* 1  */
    key_max_connections,      /* 2  */
    key_max_recv_data_length, /* 3  */
    key_max_xmit_data_length, /* 4  */
    key_max_burst_length,     /* 5  */
    key_first_burst_length,   /* 6  */
    key_default_wait_time,    /* 7  */
    key_default_retain_time,  /* 8  */
    key_max_outstanding_r2t,  /* 9  */
    key_data_pdu_inorder,     /* 10 */
    key_data_sequence_inorder,/* 11 */
    key_error_recovery_level, /* 12 */
    key_header_digest,        /* 13 */
    key_data_digest,          /* 14 */
    key_ofmarker,             /* 15 */
    key_ifmarker,             /* 16 */
    key_ofmarkint,            /* 17 */
    key_ifmarkint,            /* 18 */
    session_key_last          /* 19 */
};
enum
{
    key_wthreads,             /* 0  */
    key_target_type,          /* 1  */
    key_queued_cmnds,         /* 2  */
    target_key_last           /* 3  */
};
/*******************************************************************************/
#define MAX_FC_PORT_NUM_PER_CTL     0
#define MAX_PORT_NUM_PER_CTL        (MAX_FC_PORT_NUM_PER_CTL + MAX_ISCSI_PORT_NUM_PER_CTL)
#define MAX_CTL_NUM                 2
#define MAX_ISCSI_PORT_NUM          (MAX_ISCSI_PORT_NUM_PER_CTL * MAX_CTL_NUM)
//#define MAX_FC_PORT_NUM             (MAX_FC_PORT_NUM_PER_CTL * MAX_CTL_NUM)
#define MAX_PORT_NUM                (MAX_PORT_NUM_PER_CTL * MAX_CTL_NUM)
/********************************************************************************/

#define ENC_ALL_CTL                 (2)
#define MAIN_ENC_EXPANDER           (-1)

#define MAX_DEV_PATH_LEN            32
#define MAX_DEV_UUID_LEN            16

#define MAX_FILE_NAME               20
/***************************************enum************************************/
/* eSwitch */
#define OFF                         0                  /* 关*/
#define ON                          1                   /* 开*/
#define RST                         2                  /*  重置*/

#define INVALID_VALUE               (-1)

#define DEFAULT_VALUE             0XFFFF                         //缺省值

/* ePdSrc */
#define PD_LOCAL                    0
#define PD_FOREIGN                  2

/* ePdState */
#define PD_CAN_NOT_READ             1
#define PD_NORMAL_RW                0

/* ePdType */
#define PD_UNKNOWN                  3
#define PD_UNUSED                   0
#define PD_HOTSPARE                 4
#define PD_DATA                     5

/* 磁盘的健康状态相关，即SMART状态 */
#define PD_GOOD                     1
#define PD_BROKEN                   0

#define PD_SUPPORT_SMART            1
#define PD_NOT_SUPPORT_SMART        0
#define PD_SMART_ON                 1
#define PD_SMART_OFF                0

#define PD_FIT        1
#define PD_UNFIT      2

/* 磁盘扫描相关的控制标志 */
#define PD_SCAN_START               0                        //开始扫描
#define PD_SCAN_PAUSE               1                        //暂停扫描
#define PD_SCAN_STOP                2                        // 停止扫描

/* 后台任务所处的状态 */
#define TASK_RUNNING                0                       //运行状态
#define TASK_PAUSE                  1                       //暂停状态
#define TASK_DELAY                  2                       // 延期状态

/* 后台任务的操作 */
#define TASK_CTRL_RUN               0                        //运行被暂停的后台任务
#define TASK_CTRL_PAUSE             1                        //暂停后台任务
#define TASK_CTRL_DEL               2                        //删除后台任务

/* 系统最多后台任务数目 */
#define MAX_TASK_NUM_IN_SYS         512                     //系统中最大后台任务数目，暂定
#define TASK_UPPER_LIMIT            16                     //系统中运行的最大后台任务数目，暂定
#define DM_TASK_UPPER_LIMIT         8                      //系统中运行的最大扫描扫描任务数
#define TASK_PER_VD                 2                      //每个vd上最多运行的后台任务
#define MAX_TASK_PRARM_LEN          128                    // 计划/后台任务参数的最大长度
#define MAX_OBJECT_NAME_LEN         32                     // (计划/后台任务)对象名称的最大长度

#define MAX_TASK_OBJECT_NAME_LEN            80                     // 后台任务对象名称的最大长度
#define MAX_SCHEDULE_OBJECT_NAME_LEN        32                     // 计划任务对象名称的最大长度

/* eVdState */
#define VD_GOOD                     0
#define VD_FAULT                    1
#define VD_DEGRADE                  2
#define VD_RECING                   3
#define VD_RECERR                   4
#define VD_SCSFAULT                 5
#define VD_SCSINITING                6
/* vd->local */
#define ISFREEZE                  1         /*表示本地已经冻结，ams可根据这个值来进行设置web可操作选择为:查询和解冻两种*/
#define ISLOCAL                   0          /*表示本地，执行迁入实际上包括解冻操作，ams可根据这个值设置所有的操作*/
#define FOREIGN                   2           /*表示外地，ams可根据这个值来进行设置web可操作选择为:查询和迁入两种*/

/* eVolIOtype */
#define CHUNK_CUSTOM                0  /* custom chunksize */
#define CHUNK_FILESYSTEM            1  /* set chunksize 128K*/
#define CHUNK_DATABASE              2  /* set chunksize 64K */
#define CHUNK_MULTIMEDIA            3  /* set chunksize 512K */
#define CHUNK_INVALID               0xffff

/*eRvolNotify*/
#define FAIL_SVOL                  0           /*Fail snapshot volume */
#define FAIL_WRITE_TO_VOL          1           /*Fail write to base volume*/

#define TCS_PORT_NUM                    4     /* 物理端口数 */

/* eVolType */
#define set_vol_type(var, type)    do {   \
    (var) |= (1 << (type)) ;                  \
}while(0)

#define clear_vol_type(var, type)    do {   \
    (var) &= (~(1 << (type)));                  \
}while(0)

#define is_vol_type(var, type)     ( (var) & (1 << (type)) )

/* note:  位模式, 使用上面的宏来操作*/
#define NORM_VOL                   0
#define SNAP_SOURCE                1     /* note:  blk内部使用,引导IO流 */
#define SNAPSHOT_VOL               2
#define SNAP_ENTITY_VOL            3
#define CLONE_VOL                  4
#define PRIVATE_VOL                5
#define INVALID_VOL                6     /* 每次添加新vol类型时, 更新该值 */


/* eRaidlevel */
#define RAID0                      0
#define RAID1                      1
#define RAID3                      3
#define RAID5                      5
#define RAID6                      6
#define RAIDTP                     7
#define RAID10                     10
#define RAID30                     30
#define RAID50                     50
#define RAID60                     60
#define NRAID                      200
#define RAID_LEVEL_INVALID         0xffff

/* eVolState */
#define VOL_S_GOOD                 0
#define VOL_S_FAULT                1
#define VOL_S_ERR                  2
#define VOL_S_CHUNKSIZE_MIGRATION  3
#define VOL_S_EXPAND               4
#define VOL_S_UNREADY              5
#define VOL_S_INVALID              1000
#define VOL_PARTIAL      10       /* 只有前端lun，没有后端vol的情况 */
#define VOL_ENTIRE    11         /* 全端，后端都存在情况，只有scs可见 */
#define VOL_S_RESTORING    7         /* 用于快照恢复中不允许进行删除快照 */


/* eVolMapState */
#define MAPPED_VOL                 1
#define NO_MAPPED_VOL              0

/* eVolTaskType */
#define VOL_TASK_CHUNK             0    /* segment migration */
#define VOL_TASK_RESHAPE           1

/* eSvolState */
#define SVOL_S_ACTIVE              1
#define SVOL_S_STOP                0
#define SVOL_S_FAULTY              2

/* eCtlAction */
#define CTL_SINGLE_REBOOT          2
#define CTL_ONLINE_REBOOT          1
//#define CTL_CLOSE                  0

/* eCtlState */
#define CTL_POWEROFF             (-1)   /* 对端下电状态 */
#define CTL_NOEXIST                0    /* 对端不存在状态，获取对端状态时才会出现此值 */
#define CTL_EXIST                  1    /* 对端在位 */
#define CTL_BOOTING                2    /* 启动状态 */
#define CTL_RUNNING                3    /* 运行状态 */
#define CTL_UNKNOWN                4    /* 对端未知状态*/

/* 系统状态 */
#define SYS_STATE_UNKOWN           0
#define SYS_SINGLE_BOOTING         1
#define SYS_SINGLE_RUNNING         2
#define SYS_DUAL_BOOTING           3
#define SYS_DUAL_RUNNING           4
#define SYS_FAIL_OVER              5
#define SYS_FAIL_BACK              6

/* 通道状态 */
#define CTL_MON_RCC_STATE_FAULT    0
#define CTL_MON_RCC_STATE_SETUP    1
/* eCtlRole */
#define CTL_SLAVE                  0
#define CTL_MASTER                 1

/* 日志告警邮件的邮件用户数及地址长度 */
#define MAX_ALARMAIL_USER          3
#define MAX_MAILTO_LEN             128
#define DEFUALT_ALARM_MAIL_PERIOD  30
#define DEFUALT_ALARM_MAIL_TO      "uspsystem@storage.usp.com"
#define LOG_INQUIRE_ABOVE_LEV_FLAG 0
#define LOG_INQUIRE_ONE_LEV_FLAG   1
#define LOG_USER_NONE_DEFAULT_NAME "<unknown>"
/* 邮件设置相关宏 */
#define MAIL_SINGLE_ACCOUNT_LEN    128                  /* 最长的单个邮件账号的邮箱完整名称 */
#define MAIL_MAX_PASSLEN           64                   /* 密码准许长度 */
#define MAIL_MAX_HOST_LEN          128                  /* 准许最大主机名称长度 */
#define MAIL_SEND_ENABLE           (BYTE)1              /* 邮件或者告警邮件使能宏 */
#define MAIL_SEND_DISABLE          (BYTE)0              /* 邮件或者告警邮件禁用宏 */
#define MAIL_SMTP_PORT             25                   /* SMTP邮件默认端口 */
#define MAIL_HOST_ADDRESS_TYPE     0                    /* 默认为IP地址 */
#define DEFUALT_MAILBOX_PASSDWD    "123456"             /* 默认的邮箱密码 */
#define DEFUALT_CODEC_MAIL_PASSWD  "******"             /* 加密的密码 */
#define DEFUALT_MAIL_HOST_NAME     "192.168.1.1"        /* 默认的主机地址 */

/* eConfigFileType */
#define HTML_FILE_TYPE             1
#define BIN_FILE_TYPE              2

/* E_LogLevel 级别越高 数值越小 */
#define E_LOG_NOTICE               5
#define E_LOG_WARNING              4
#define E_LOG_ERROR                3
#define E_LOG_CRITICAL             2

/* 写内存和写元数据区的标志 */
#define WRITE_MDA_FLG               (SWORD32)0
#define WRITE_MEM_FLG               (SWORD32)1

/* 硬盘在系统中的可见状态宏定义 */
#define CTRL0                       (SWORD32)0               //单控环境下，控制器0可见
#define CTRL1                       (SWORD32)1               //单控环境下，控制器1可见
#define CTRL_DUAL                   (SWORD32)2               //双控环境下，两个控制器都可见
#define CTRL0_ABNORMAL              (SWORD32)3               //双控环境下，仅控制器0可见，异常状态
#define CTRL1_ABNORMAL              (SWORD32)4               //双控环境下，仅控制器1可见，异常状态

/* 用户管理相关的宏定义 */
#define MAX_USER_NAME_LEN           24
#define MAX_USER_EMAIL_LEN          128

#define MAX_USER_PSW_LEN            16

#define MAX_ADMIN_NUM               1
#define MAX_COMMON_USER_NUM         3
#define MAX_GUEST_USER_NUM          5
#define MAX_USER_NUM                (MAX_ADMIN_NUM + MAX_COMMON_USER_NUM + MAX_GUEST_USER_NUM)

/* 重启/ 关闭控制器使用的宏定义 */
#define CTRL_LOCAL               0                              //本端标志
#define CTRL_PEER                1                              //对端标志
#define CTRL_DEFAULT             0XFFFF                         //控制器信息的缺省默认标志


/* ScanBox Expander 拓扑结构相关 */
#define SCANBOX_MAX_EXP_NUM      2        /* ScanBox最大的Expander数目 */
#define SCANBOX_MAX_DISK_NUM     28       /* ScanBox最大磁盘数目 */

#define ENC_SCANBOX              1        /* ScanBox类型 */
#define ENC_USP                  3        /* USP类型 */


/* 系统网络相关的宏定义 */
#define  MAX_NET_DEVICE_NUM           6                             /*系统中可配置的网络设备最大数量*/
#define  DUAL_MAX_NET_DEVICE_NUM      (MAX_NET_DEVICE_NUM*2)        /*双控网络设备数目最大数目*/

#define  NET_ROLE_BUSINESS            0        /*业务类型的网络设备*/
#define  NET_ROLE_MANAGEMENT          1        /*管理类型的网络设备*/
#define  MAX_NET_DEVICE_NAME_LEN      6        /* 网络设备名称最大长度 */

#define  SPR10_ISCSI_PORT_NUM         4                           /*ISCSI 端口数目*/
#define  SPR10_DUAL_ISCSI_PORT_NUM    (SPR10_ISCSI_PORT_NUM*2)    /*双端ISCSI 端口总数目*/

#define  SCSI_MAX_STATICROUTE_NUM     128          /* ISCSI 最大静态路由数目 */

#define  ISCSI_PING_TIMEOUT              1               /* Ping 超时标记*/
#define  ISCSI_MAC_ISSET              0xA5


#define  BROAD_UNKNOWN                0          /*未知单板*/
#define  BROAD_SBCJ                   1          /*SBCJ板*/
#define  BROAD_SPR10                  2          /*SPR10单板*/
#define  BROAD_SPR11                  3          /*SPR11单板*/
#define  BROAD_PC                     4          /*PC 环境*/


/* 时间设置 */
#define  NTP_HOST_NAME_LEN          (WORD32)64  /* max host name len */
#define  TIME_ZONE_CITY_NAME_LEN    (WORD32)64
#define  TIME_ZONE_CITY_NUM         (WORD32)100

#define  NTP_DEBUG            (WORD32)1
#define  NTP_AUTO             (BYTE)1
#define  NTP_MANUAL           (BYTE)0

/* 物理单板定义 */
#define USP_BT_PC          (0xFFFF)   /* PC调试 */
#define USP_BT_SBCW        (0x0214)   /* SBCW单板 */
#define USP_BT_SBCJ        (0x0219)   /* SBCJ单板 */
#define USP_BT_SPR10       (0x6701)   /* SPR10单板 */
#define USP_BT_SPR11       (0x6718)   /* SPR11单板 */

/* 版本相关宏 */
#define  MEDIA_MOUNT_DIR "/IDE0"
#define  VERSION_DIR     "/IDE0/localver/"
#define  CONFIG_DIR      "/IDE0/uspcfg/"
#define  LOG_EXP_DIR     "/IDE0/log/"
#define  WORK_DIR        "/usr/sbin/"
#define  SCS_TMP_DIR     "/usr/sbin/tmp/"
#define  USP_VER_TYPE    1
#define  BOOT_VER_TYPE   8
#define  EXP_VER_TYPE    8500
#define  VPD_VER_TYPE    8501
#define  POW_VER_TYPE    8502
#define  UPDATE_EXP_RUNNING     (BYTE)(0x1A)
#define  UPDATE_EXP_NORUNNING   (BYTE)(0xA1)
#define  UPDATE_BOOT_RUNNING    (BYTE)(0x1B)
#define  UPDATE_BOOT_NORUNNING  (BYTE)(0xB1)
#define  UPDATE_USP_START       (BYTE)(0x11)
#define  UPDATE_USP_ERROR       (BYTE)(0x22)
#define  UPDATE_USP_STOP        (BYTE)(0x33)
#define  UPDATE_USP_INIT        (BYTE)(0x44)
#define  ALLOW_USP_UPDATE       (BYTE)(1)
#define  QUESTION_USP_UPDATE    (BYTE)(2)

#define  FILE_EXIST_DUAL        1
#define  FILE_EXIST_LOCAL       2
#define  FILE_EXIST_PEER        3
#define  FILE_NOT_EXIST_DUAL    4
#define  FILE_EXIST_SINGLE      5
#define  FILE_NOT_EXIST_SINGLE  6
#define  CUR_OR_STAND_NAME_SAME 7

#define  VERSION_FILE_TYPE     1
#define  CONFIG_FILE_TYPE      2
#define  OTHER_FILE_TYPE       3
#define  LOG_FILE_TYPE         4

/* FTP相关宏 */
#define  FILE_NAME_LEN         (BYTE)(52)
#define  FILE_PATH_LEN         (BYTE)(80)
#define  USR_NAME_LEN          (BYTE)(20)
#define  PASSWD_LEN            (BYTE)(20)
#define  MAX_FW_FILE_NAME_LEN  FILE_NAME_LEN

/* 系统诊断相关宏定义 */
#define  MAX_DIAG_DEVICE_NAME_LEN    64   /* 最大的设备名称长度 */
#define  MAX_DIAG_DEVICE_NUM         32   /* 最多的设备数目 */

/********* BBU相关 ***********/

/* BBU是否在位 */
#define  DIAG_BBU_PRESENT            0    /* BBU在位 */
#define  DIAG_BBU_NOPRESENT          1    /* BBU不在位 */

/* BBU能否通信 */
#define  BBU_COMMUNICATE_OK          0    /* BBU能正常通信 */
#define  BBU_COMMUNICATE_FAIL        2    /* BBU不能正常通信 */

/* BBU能否充电 */
#define  BBU_CHARGE_OK               0    /* BBU能正常充电 */
#define  BBU_CHARGE_FAIL             3    /* BBU不能正常充电 */

/* BBU能否放电 */
#define  BBU_DISCHARGE_OK            0    /* BBU能正常放电 */
#define  BBU_DISCHARGE_FAIL          4    /* BBU不能正常放电 */

/* BBU是否过温 */
#define BBU_NORMAL_TEMPERATURE       0    /* BBU温度正常 */
#define BBU_OVER_TEMPERATURE         5    /* BBU温度过高 */

/* BBU是否失效 */
#define BBU_VALID                    0    /* BBU未失效 */
#define BBU_INVALID                  6    /* BBU失效，包括两种情况:一是BBU永久性不可恢复故障;二是充电达不到USP性能所要求的最低电量要求 */


/* BBU的三种状态 */
#define BBU_NORMAL                   0    /* BBU正常 */
#define BBU_ABNORMAL                 1    /* BBU异常 */
#define BBU_LOW_CAPACITY             2    /* BBU电量不足 */



#define  DIAG_BBU_CHARGING           0    /* BBU正在充放电 */
#define  DIAG_BBU_NOT_CHARGING       1    /* BBU不在充放电 */

#define  DIAG_ALL_EXPANDER           0    /* 获取所有箱体诊断信息 */
#define  DIAG_SINGLE_EXPANDER        1    /* 获取单个箱体诊断信息 */

#define  DEVICE_STATUS_OK            0    /* 设备状态正常 */
#define  DEVICE_STATUS_FALSE         1    /* 设备状态异常 */
#define  DEVICE_STATUS_DOWN         2    /* 设备未连接 */

#define  DIAG_FC_PRESENT             3    /* FC子卡  在位 */
#define  DIAG_FC_ABSENT              4    /* FC子卡不在位 */
#define  DIAG_SOURCE_USER            0    /* 用户下发的系统诊断 */
#define  DIAG_SOURCE_SCS             1    /* 系统定时诊断 */

#define  MAX_PROP_REC_NUM            48   /* 诊断对象的属性最大记录数 */
#define  MAX_PROP_NAME_LEN           48   /* 诊断对象的属性名称最大长度 */
#define  MAX_PROP_VALUE_LEN          48   /* 诊断对象的属性值最大长度*/
#define  DIAG_DEVICE_BBU             0    /* 诊断BBU */
#define  DIAG_DEVICE_EPLD            1    /* 诊断EPLD */
#define  DIAG_DEVICE_EEPROM          2    /* 诊断EEPROM */
#define  DIAG_DEVICE_CRYSTAL         3    /* 诊断晶振 */
#define  DIAG_DEVICE_RTC             4    /* 诊断RTC */
#define  DIAG_DEVICE_CLOCK           5    /* 诊断锁相环 */
#define  DIAG_DEVICE_PCIE            6    /* 诊断PCIE */
#define  DIAG_DEVICE_PM              7    /* 诊断PM8001 */
#define  DIAG_DEVICE_FC              8    /* 诊断FC */
#define  DIAG_DEVICE_IBMC            9    /* 诊断IBMC */
#define  DIAG_DEVICE_GE              10   /* 诊断GE */

#define  DIAG_ECC_SWITCH_OPEN        1    /* 打开ECC检测开关 */
#define  DIAG_ECC_SWITCH_CLOSE       2    /* 关闭ECC检测开关 */



/* 系统相关信息宏定义 */
#define  USP_SYS_NAME                     "ZXUSP"                    /* 系统名称 ,  默认值为 ZXATCA_USP*/
#define  USP_VENDOR                       "ZTE"                           /* 供应商,     默认值为 ZTE*/
#define  USP_MODEL                        "ZXUSP"                   /* 产品型号,   默认值为 ZXATCA(USP) */
#define  USP_VERSION_NAME                 "V1.0"                          /* 产品版本,   默认值为 V1.0 */
#define  USP_SERIAL_NUM                   "2012-0001"                     /* 产品序列号, 默认值为 2012-0001 */
#define  USP_SYS_STATE_SINGLE             "Single Running"                /* 系统状态, 单控运行, 默认值为Single Running */
#define  USP_SYS_STATE_DUAL               "Dual Running"                  /* 系统状态, 双控运行, 默认值为Dual Running */
#define  USP_SYS_INFO_CFG_FILE            "/IDE0/uspcfg/sys_info.conf"    /* 系统相关信息配置文件 */
#define  USP_SYS_INFO_CFG_SIZE            256                             /* 系统相关信息配置文件大小 */

/* 系统相关信息长度宏定义 */
#define  USP_SYSTEM_NAME_MAX_LEN          32       /* 系统名称字符串最大长度 */
#define  USP_SYSTEM_VENDOR_MAX_LEN        32       /* 供应商字符串最大长度 */
#define  USP_SYSTEM_MODEL_MAX_LEN         32       /* 产品型号字符串最大长度 */
#define  USP_SYSTEM_VERSION_MAX_LEN       32       /* 产品版本字符串最大长度 */
#define  USP_SYSTEM_SERIAL_NUM_MAX_LEN    32       /* 产品序列号字符串最大长度 */
#define  USP_SYSTEM_STATE_MAX_LEN         32       /* 系统状态字符串最大长度 */

/* 控制器序列号和名字长度宏定义 */
#define  USP_CONTROLLER_SERIAL_MAX_LEN    32       /* 控制器序列号字符串最大长度*/
#define  USP_CONTROLLER_NAME_MAX_LEN      32       /* 控制器名称字符串最大长度*/

/* 扩展柜信息相关宏定义*/
#define  MAX_DISKNAMELEN       12   /*磁盘设备描述符长度限制*/

#define  MAX_BUSIDLEN          20   /*BUSID长度*/
#define  MAX_FAN_NUM           4    /*风扇最多支持个数*/
#define  MAX_PS_NUM            2    /*电源最多支持的个数*/
#define  MAX_VOLT_NUM          6    /*电压传感器支持的个数*/
#define  MAX_TEMP_NUM          16   /*温度传感器支持的个数*/
#define  MAX_DISK_NUM          28   /*磁盘支持的个数*/
#define  MAX_UP_SUB_ENCL       24   /*上下支持级联个数*/
#define  SUBID_LEN             16   /*扩展柜ID长度*/

#define  WARNING_RECOVER       6    /*告警恢复*/
#define  USP_EM_FAIL           5    /*设备故障*/
#define  OUT_FAILURE           1    /*超过最高门限*/
#define  OUT_WARNING           2    /*超过最高警告*/
#define  UNDER_FAILURE         3    /*低于最低门限*/
#define  UNDER_WARNING         4    /*低于最低告警*/
#define  USP_EM_FAN            0x03 /*风扇类型*/
#define  USP_EM_PS             0x02 /*电源类型*/
#define  USP_EM_VOLT           0x12 /*电压类型*/
#define  USP_EM_TEMP           0x04 /*温度类型*/
/*DM相关宏定义*/
#define  DM_SMART_SHORT_SELFTEST       0x1
#define  DM_SMART_LONG_SELFTEST        0x2
#define  SCS_DM_DISK_POWER_ACTIVE      0x00
#define  SCS_DM_DISK_POWER_IDLE        0x01
#define  SCS_DM_DISK_POWER_STANDBY     0x02
#define  SCS_DM_DISK_POWER_STOP_SLEEP  0x03


/*计划任务周期值*/
#define  MAX_SCHEDULE_NUM        32   //系统中支持的最大计划任务数目，暂定

#define  SCHED_IMMEDIATELY       0    // execute IMMEDIATELY
#define  SCHED_ONCE              1    // execute once
#define  SCHED_DAILY             2    // dayly task
#define  SCHED_WEEKLY            3    // weekly task
#define  SCHED_MONTHLY           4    // monthly task

/* 计划相关描述*/
#define  SCHED_DESC_DISK_SCAN    "Scan disk for bad blocks."

/* 卷的类型 */
#define  TYPE_VOL      0    /* 源卷 */
#define  TYPE_CVOL     1    /* 拷贝卷 */
#define  TYPE_SVOL     2    /* 快照卷 */

/* 成员盘列表类型---1:local,2:peer,3:dual */
#define  MD_LOCAL      1    /* 成员盘在本端 */
#define  MD_PEER       2    /* 成员盘在对端 */
#define  MD_DUAL       3    /* 成员盘两端都存在 */

/* 端口状态 */
#define  PORT_STATUS_UP               (1)                /* Link状态之链接OK */
#define  PORT_STATUS_DOWN             (0)                /* Link状态之链接错误 */
#define  USP_PORT_STATUS_UP           PORT_STATUS_UP     /* Link状态之链接OK */
#define  USP_PORT_STATUS_DOWN         PORT_STATUS_DOWN   /* Link状态之链接错误 */


/*端口物理状态信息相关*/

#define PORT_VALUE_UNKNOWN                 0                  /* 未知信息*/

#define PORT_DUPLEX_FULL                   1                  /* 全双工模式*/
#define PORT_DUPLEX_HALF                   2                  /* 半全双工模式*/

#define PORT_NEG_FORCE                     1                  /* 强制协商模式*/
#define PORT_NEG_AUTO                      2                  /* 自协商模式*/

#define PORT_SPEED_10M                     1                  /* 10M 速率端口*/
#define PORT_SPEED_100M                    2                  /* 100M 速率端口*/
#define PORT_SPEED_1G                      3                  /* 1G 速率端口*/
#define PORT_SPEED_10G                     4                  /* 10G 速率端口*/
#define PORT_SPEED_12G                     5                  /* 12G 速率端口*/
#define PORT_SPEED_16G                     6                  /* 16G 速率端口*/

#define PORT_STATE_INVALID                 0                  /* 无效端口状态*/
#define PORT_STATE_UP                      1                  /* 端口状态 UP*/
#define PORT_STATE_DOWN                    2                  /* 端口状态 DOWN*/

#define PORT_MAC_DATA_SIZE                 6                  /* 端口MAC 数据段大小*/

/* snmp 相关 */
#define MAX_SNMP_COMMUNITY_NAME_LEN  16
#define MAX_SNMP_NOTIFY_NUM   5


/* 远程镜像相关的宏 */
#define MAX_MIRROR_IN_SYS             32             //系统中镜像的最大数目
#define MAX_MIRROR_CONNECT_NUM        8              //远程连接的最大数目
#define MAX_MIRROR_REMOTE_DISK_NUM    64             //远程磁盘的最大数目

#define MAX_MIRROR_REMOTE_DISK_NAME_LEN    16        //远程磁盘名称的最大长度

#define REMOTE_DISK_UNUSED                   1       //远程磁盘未被使用
#define REMOTE_DISK_USED                     2       //远程磁盘已被使用

#define MIRROR_SYNCHRONOUS      0     /* 同步镜像 */
#define MIRROR_ASYNCHRONOUS     1     /* 异步镜像 */

#define MIRROR_NOT_INIT_SYNC   NOT_INIT_SYNC     /* 不做初始同步 */
#define MIRROR_IS_INIT_SYNC    IS_INIT_SYNC      /* 做初始同步 */

#define MIRROR_MANUAL_POLICY   MANUAL_POLICY     /* 手动恢复策略 */
#define MIRROR_AUTO_POLICY     AUTO_POLICY       /* 自动恢复策略 */

/* FC 相关的宏 */
#define MAX_FC_WWN_LENGTH            8              //FC 端口WWN(包括WWPN、WWNN)的最大长度
#define MAX_FC_PORT_NUM              4              //FC 端口的最大数目
#define MAX_FC_PORT_ID_LENGTH        3              //FC 端口ID 的最大长度

#define FC_PORT_SPEED_INVALID        0                 /* 无效速率值 */
#define FC_PORT_SPEED_2G             1                 /* 2G 速率FC 端口 */
#define FC_PORT_SPEED_4G             2                 /* 4G 速率FC 端口 */
#define FC_PORT_SPEED_8G             3                 /* 8G 速率FC 端口*/

#define FC_TOPO_INVALID              0                 /*  FC端口拓扑结构无效 */
#define FC_TOPO_PRIVATE_LOOP         1                 /*  FC端口拓扑为私有环 */
#define FC_TOPO_PUBLIC_LOOP          2                 /*  FC端口拓扑为共有环 */
#define FC_TOPO_FABRIC               3                 /*  FC端口拓扑为Fabric  */
#define FC_TOPO_P2P                  4                 /*  FC端口拓扑为点对点 */

#define FC_PORT_INVALID              0                 /* FC 端口无效 */
#define FC_PORT_VALID                1                 /* FC 端口有效 */




/********************************************************************************************
*************************************MTS相关宏定义*******************************************
********************************************************************************************/

/* 电子标签的最大长度 */
#define  MAX_INFO_LABLE_LENGTH     29                 /* 同BSP_SODA_SP_LSSP中宏定义 */

/*  测试类型 */
#define  MTS_HT                    0X00                  /* 高温测试 */
#define  MTS_FT                    0X01                  /* 功能测试 */

/* EPLD自检 */
#define MTS_EPLDSELF_GOOD          0X02
#define MTS_EPLDSELF_ERROR         0X03


/* 本端单板在位信号测试 */
#define  MTS_BOARD_POWER_ON        0X04                  /* 单板在位 */
#define  MTS_BOARD_POWER_OFF       0X05                  /* 单板不在位 */

/* 版本FLASH状态 */
#define  MTS_FLASH_GOOD            0X06                  /* 正常或版本一致 */
#define  MTS_FLASH_NOT_GOOD        0X07                  /* 不正常或版本不一致 */

/* DDR内存信息检测 */
#define  MTS_BOARD_DDR_OK          0X08
#define  MTS_BOARD_DDR_ERROR       0X09

/* NVRAM信息检测 */
#define  MTS_NVRAM_GOOD            0X0A
#define  MTS_NVRAM_ERROR           0X0B

/* CLOCK信息检测 */
#define  MTS_CLOCK_GOOD            0X0C
#define  MTS_CLOCK_ERROR           0X0D

#define  MTS_MO_COMPA              0X0E                  /* 版本FLASH比较 */

/* 输入信号检测 */
#define  MTS_INPUTIO_GOOD          0X0F
#define  MTS_INPUTIO_ERROR         0X10

/* 输出信号检测 */
#define  MTS_OUTPUTIO_GOOD         0X11
#define  MTS_OUTPUTIO_ERROR        0X12

/* 电源信号灯检测 */
#define  MTS_POWER_LED_GOOD        0X13
#define  MTS_POWER_LED_ERROR       0X14



/* 板内时钟类型 */
#define  MTS_CLOCK_25M             0X15
#define  MTS_CLOCK_33M             0X16
#define  MTS_CLOCK_75M             0X17
#define  MTS_CLOCK_100M            0X18
#define  MTS_CLOCK_125M            0X19
#define  MTS_CLOCK_133M            0X1A

/* 看门狗使能, 禁用, 喂狗 */
#define  MTS_DISABLE_WTG           0X1B
#define  MTS_ENABLE_WTG            0X1C
#define  MTS_FEDD_WTG              0X1D

/* RTC时钟电路 */
#define  MTS_RTC_GOOD              0X1E
#define  MTS_RTC_ERROR             0X1F

/* Link status */
#define  MTS_LINK_GOOD             0X20
#define  MTS_LINK_ERROR            0X21

/* pcie cache同步信号测试 */
#define  MTS_PCIE_CACHE_GOOD       0X22
#define  MTS_PCIE_CACHE_ERROR      0X23

/* 面板GE口环回测试 */
#define  MTS_LOOP_BACK_GOOD        0X24
#define  MTS_LOOP_BACK_NOT_GOOD    0X25

/* 8005复位成功标志 */
#define  MTS_EZZF_RESET_GOOD       0X26
#define  MTS_EZZF_RESET_NOT_GOOD   0X27

/* minisas接口自环测试 */
#define  MTS_MINISAS_SPEED_GOOD      0X28
#define  MTS_MINISAS_SPEED_NOT_GOOD  0X29

/* 背板SAS接口自环测试 */
#define  MTS_BBSAS_SPEED_GOOD        0X2A
#define  MTS_BBSAS_SPEED_NOT_GOOD    0X2B


/* TW1, TW2, TW3 信号 */
#define  MTS_TWSIG_GOOD             0X2C
#define  MTS_TWSIG_NOT_GOOD         0X2D

/* 硬盘点灯或灭灯 */
#define  MTS_HDDLIGHT_ON            0X2E
#define  MTS_HDDLIGHT_OFF           0X2F

/* PHY自检 */
#define  MTS_PHYSELF_GOOD             0X30
#define  MTS_PHYSELF_NOTGOOD          0X31

/* PCIE自检结果 */
#define  MTS_PCIESELF_GOOD            0X32
#define  MTS_PCIESELF_NOTGOOD         0X33


/* PM8001自检结果 */
#define  MTS_PMSELF_GOOD              0X34
#define  MTS_PMSELF_NOTGOOD           0X35

/* BBU自检结果 */
#define  MTS_BBU_GOOD                 0X36
#define  MTS_BBU_NOTGOOD              0X37

/* 传感器状态 */
#define MTS_SENOR_GOOD                0X38
#define MTS_SENOR_NOT_GOOD            0X39

#define MTS_8001           0X00
#define MTS_8005           0X01

/* 内存数据一致性，内存数据正确性 */
#define MTS_DDR_COHERENCE_CORRECT_GOOD    0X3A
#define MTS_DDR_COHERENCE_CORRECT_NOTGOOD 0X3B

/* 对板状态 */
#define MTS_PEER_POWERON                  0X3C
#define MTS_PEER_POWEROFF                 0X3D

/* SPR10箱体号  */
#define  MTS_SPR10_CLASSSISID      0   /* 固定值为0 */

#define  CMT_CLASS_LEVEL           6

#define  MTS_WORKEPLD_VER          0
#define  MTS_BOOTEPLD_VER          1
#define  MTS_USP_VER               2
#define  MTS_EXP_VER               3
#define  MTS_ZBOOT_VER             4

#define   FC_MAX_PORT_NUM          (4)                  /* 最大FC端口数 */
#define   FC_MAC_LENGTH             (6)        /* MAC地址长度定义 */
#define   FC_WWN_LENGTH             (8)        /* FC World Wide长度 */
#define   FC_PORT_ID_LENGTH         (3)                  /* FC端口ID的长度(D_ID/S_ID) */

#define  MTS_82580          0
#define  MTS_I350          1

#define  MTS_CK420          0
#define  MTS_DB1900          1
#define  MTS_RTC          2
#define  MTS_CRYSTAL          3

#define  MTS_LED_CACHE          0
#define  MTS_LED_HOTSWAP                1
#define  MTS_LED_ALM          2
#define  MTS_LED_BBU          3
#define  MTS_LED_POWER     4
#define  MTS_LED_MINISAS_STA1    5
#define  MTS_LED_MINISAS_EXC1    6
#define  MTS_LED_MINISAS_STA2    7
#define  MTS_LED_MINISAS_EXC2    8
#define  MTS_LED_DISK    9

#define  MTS_LED_COLOR_GREEN         0
#define  MTS_LED_COLOR_BLUE                1
#define  MTS_LED_COLOR_RED          2
#define  MTS_LED_COLOR_ORANGE          3
#define  MTS_LED_MULTI_COLOR          4

#define  MTS_DISK_TYPE_ACTIVE        0
#define  MTS_DISK_TYPE_FAULT                1

#define  MTS_LED_OFF        0
#define  MTS_LED_ON               1

/********************************************************************************************
*************************************SIC相关宏定义*******************************************
********************************************************************************************/

#define MAX_SIC_FILE_NAME_LEN        64  /* 文件名长度(包括路径) */

#define SIC_TASK_NAME                "system information collect"
#define TASK_DUAL_OWNER              (SWORD32)2

/* 命令ID:
 * SCS_COLLECT_SYS_INFO: 系统信息采集命令
 * SCS_SIS_TASK_COMPLETE: 系统信息采集任务完成命令
 */
#define SCS_SIC_COLLECT_SYS_INFO     (BYTE)0x00
#define SCS_SIS_TASK_COMPLETE        (BYTE)0x01
#define SCS_SIC_GET_TASK_PROC        (BYTE)0x02
#define SCS_SIC_GET_TARBALL_INFO     (BYTE)0x03
#define SCS_SIC_SET_EMAIL_CFG        (BYTE)0x04
#define SCS_SIC_GET_EMAIL_CFG        (BYTE)0x05

/* 子系统类型 */
#define SIC_SAS                      (WORD32)1
#define SIC_DM                       (WORD32)2
#define SIC_BLK                      (WORD32)3
#define SIC_STS                      (WORD32)4
#define SIC_CBS                      (WORD32)5
#define SIC_TCS                      (WORD32)6
/* SCS子系统内部分多个配置文件: BLK、STS、DM、告警日志 */
#define SIC_SCS                      (WORD32)7    /* SCS配置内容 */
#define SIC_BLK_CFG                  (WORD32)8    /* BLK配置内容 */
#define SIC_STS_CFG                  (WORD32)9    /* STS配置内容 */
#define SIC_DM_CFG                   (WORD32)10   /* DM配置内容 */
#define SIC_WARN_LOG                 (WORD32)11   /* 告警日志 */
#define SIC_All_TASK                 (WORD32)12   /* 后台任务计划任务信息 */

/* 消息类型 */
#define SIC_CMD                      (BYTE)1
#define SIC_GRAND                    (BYTE)2

/* ------------------------- FC相关宏定义 ------------------------------ */
#define FC_ALLOW_PLOGI_CNT               (256)
#define FC_RESERVE_GPN_FT_LIST_CNT        (5000)
#define  FC_ALLOW_LIRR_CNT                (1024)
#define FC_WWN_LENGTH               (8)


#define     ALARM_ADDINFO_UNION_MAX     (WORD16)256
#define     ALARM_PER_PACKET_MAX        (BYTE)20
#define     ALARM_NAME_LENGTH                   256         /* 告警名称长度 */

#define     ALARM_TYPE_ALARM            (BYTE)1 /*告警*/
#define     ALARM_TYPE_RESTORE          (BYTE)2 /*恢复*/

#define     ALARM_LASTPACKET_YES         (BYTE)1 /*最后一包*/
#define     ALARM_LASTPACKET_NO          (BYTE)0 /*非最后一包*/

#define     ALARM_LAN_EN            (BYTE)0 /*英文*/
#define     ALARM_LAN_CH            (BYTE)1 /*中文*/

#endif
