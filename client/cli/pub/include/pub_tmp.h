#ifndef _PUB_TMP_H_
#define _PUB_TMP_H_

#include <string.h>
#include <stdlib.h>
#include<fcntl.h> 
#include<sys/stat.h>
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <stdarg.h>
#include <ctype.h>
#include "oam_typedef.h"

#define  EV_BEGIN                             (WORD32)0
#define  EV_CFG_USERFILE_ACK                  (WORD32)(EV_BEGIN + 3)
#define  EV_CFG_USERFILE_REQ                  (WORD32)(EV_BEGIN + 4)
#define  EV_COMM_ABORT                        (WORD32)(EV_BEGIN + 5)
#define  EV_COMM_OK                           (WORD32)(EV_BEGIN + 6)
#define  EV_MASTER_POWER_ON                   (WORD32)(EV_BEGIN + 7)
#define  EV_MASTER_TO_SLAVE                   (WORD32)(EV_BEGIN + 8)
#define  EV_MATE_COMM_ABORT_OAM               (WORD32)(EV_BEGIN + 9)
#define  EV_MATE_COMM_OK_OAM                  (WORD32)(EV_BEGIN + 10)
#define  EV_OAM_CFG_CFG                       (WORD32)(EV_BEGIN + 11)
#define  EV_OAM_CFG_PLAN                      (WORD32)(EV_BEGIN + 12)

#define EV_OAM_CEASE_TO_APP        (WORD32)(EV_OAMCFGBEGIN+40)
#define  EV_OAM_CLI_SCRIPT_LANGUAGE_CHANGED   (WORD32)(EV_BEGIN + 13)
#define  EV_OAMCLI_APP_REG                    (WORD32)(EV_BEGIN + 14)
#define  EV_OAMCLI_APP_REG_A2M                (WORD32)(EV_BEGIN + 15)
#define  EV_OAMCLI_APP_REGEX                  (WORD32)(EV_BEGIN + 16)
#define  EV_OAMCLI_APP_REGEX_A2M              (WORD32)(EV_BEGIN + 17)
#define  EV_OAMCLI_APP_UNREG                  (WORD32)(EV_BEGIN + 18)
#define  EV_OAMCLI_SPECIAL_REG                (WORD32)(EV_BEGIN + 19)
#define  EV_OAMS_VER_UPDATE_NOTIFY            (WORD32)(EV_BEGIN + 20)
#define  EV_OAMS_VERINFO_ACK                  (WORD32)(EV_BEGIN + 21)
#define  EV_SLAVE_POWER_ON                    (WORD32)(EV_BEGIN + 22)
#define  EV_SLAVE_TO_MASTER                   (WORD32)(EV_BEGIN + 23)
#define  EV_STARTUP                           (WORD32)(EV_BEGIN + 24)
#define  EV_SUBSCR_EVENT_TOAPP                (WORD32)(EV_BEGIN + 25)
#define  EV_OAMS_VERINFO_REQ                  (WORD32)(EV_BEGIN + 26)
#define  EV_TXTCFG_REQ                        (WORD32)(EV_BEGIN + 38)
#define  EV_VER_OAMS_NOTIFYREG_ACK            (WORD32)(EV_BEGIN + 39)
#define  EV_XML_CEASECMD_REQ                  (WORD32)(EV_BEGIN + 40)
#define  EV_XML_HANDSHAKE_REQ                 (WORD32)(EV_BEGIN + 41)
#define  EV_XML_LINKCFG_REQ                   (WORD32)(EV_BEGIN + 42)
#define  EV_XML_OMMCFG_ACK                    (WORD32)(EV_BEGIN + 43)
#define  EV_XML_OMMCFG_REQ                    (WORD32)(EV_BEGIN + 44)
#define  EV_XML_REGISTER_REQ                  (WORD32)(EV_BEGIN + 45)
#define  EV_XML_UNREGISTER_REQ                (WORD32)(EV_BEGIN + 46)
#define  EV_TIMER_1                           (WORD32)(EV_BEGIN + 47)
#define  EV_TIMER_2                           (WORD32)(EV_BEGIN + 48)
#define  EV_TIMER_3                           (WORD32)(EV_BEGIN + 49)
#define  EV_TIMER_4                           (WORD32)(EV_BEGIN + 50)
#define  EV_TIMER_5                           (WORD32)(EV_BEGIN + 51)
#define  EV_TIMER_6                           (WORD32)(EV_BEGIN + 52)
#define  EV_TIMER_7                           (WORD32)(EV_BEGIN + 53)
#define  EV_TIMER_8                           (WORD32)(EV_BEGIN + 54)
#define  EV_TIMER_9                           (WORD32)(EV_BEGIN + 55)
#define  EV_TIMER_10                          (WORD32)(EV_BEGIN + 56)
#define  EV_TIMER_11                          (WORD32)(EV_BEGIN + 57)
#define  EV_TIMER_12                          (WORD32)(EV_BEGIN + 58)
#define  EV_TIMER_13                          (WORD32)(EV_BEGIN + 59)
#define  EV_TIMER_20                          (WORD32)(EV_BEGIN + 60)
#define  EV_TIMER_21                          (WORD32)(EV_BEGIN + 61)

#define EV_DBS_BEGIN                 (WORD32)40001
#define EV_DBMS_BEGIN                (WORD32)EV_DBS_BEGIN             /* 数据库DBMS消息起始号, 40001  */

#define EV_DBINIT                    (WORD32)( EV_DBMS_BEGIN + 0    ) /* 数据库初始化进程消息起始号: 40001  */
#define EV_DBRECEIVER                (WORD32)( EV_DBMS_BEGIN + 200  ) /* 数据库静态同步消息起始号:   40201  */
#define EV_DBMSDYNSYNC               (WORD32)( EV_DBMS_BEGIN + 2000 ) /* 数据库动态同步消息起始号:   42001  */
#define EV_DBPROBE                   (WORD32)( EV_DBMS_BEGIN + 3000 ) /* 数据库探针消息起始号:       43001  */
#define EV_DBNOTIFY                  (WORD32)( EV_DBMS_BEGIN + 4000 ) /* 数据库配置更改消息起始号:   44001  */
#define EV_DBIO                      (WORD32)( EV_DBMS_BEGIN + 5000 ) /* 数据库存盘消息起始号:       45001  */
#define EV_DBLOCK                    (WORD32)( EV_DBMS_BEGIN + 6000 ) /* 数据库死锁检测消息起始号:   46001  */
#define EV_DBUPDATE                  (WORD32)( EV_DBMS_BEGIN + 7000 )
#define EV_DBACCESS                  (WORD32)( EV_DBMS_BEGIN + 9000 )/* 数据库新dbaccess消息 */
#define EV_DBMS_END                  (WORD32)( EV_DBMS_BEGIN + 9999 ) /* 数据库DBMS消息结束号, 50000  */

#define EV_DBAPP_BEGIN               (WORD32)( EV_DBMS_END    + 1   ) /* 数据库应用消息起始号  50001  */

#define EV_DBACCESS_BEGIN            (WORD32)( EV_DBAPP_BEGIN +1    ) /* 数据库应用接口消息起始号 50001  */
#define EV_DBUPDATE_BEGIN            (WORD32)( EV_DBAPP_BEGIN +1000)  /* 数据库update消息起始号  51001  */

#define EV_DBS_END                   (WORD32) 60000                   /* 数据库消息结束号 60000 */

/* STRAT进程的事件号         0x1B80 ~ 0x1BCF */
#define EV_DBSTART              (WORD16)(4000 + 3040 )        /* 7040 = 0x1B80 */

#define EV_MasterSave               (WORD16)( (EV_DBSTART)+40 )   /* 7080 = 0x1BA8 */
#define EV_MasterSaveOK             (WORD16)( (EV_DBSTART)+41 )   /* 7081 = 0x1BA9 */
#define EV_MasterSaveFail           (WORD16)( (EV_DBSTART)+42 )   /* 7082 = 0x1BAA */
#define EV_SlaveSave                (WORD16)( (EV_DBSTART)+43 )   /* 7083 = 0x1BAB */
#define EV_SlaveSaveOK              (WORD16)( (EV_DBSTART)+44 )   /* 7084 = 0x1BAC */
#define EV_SlaveSaveFail            (WORD16)( (EV_DBSTART)+45 )   /* 7085 = 0x1BAD */
#define EV_MToSSync                 (WORD16)( (EV_DBSTART)+46 )   /* 7086 = 0x1BAE */
#define EV_MToSSyncOK               (WORD16)( (EV_DBSTART)+47 )   /* 7087 = 0x1BAF */
#define EV_MToSSyncFail             (WORD16)( (EV_DBSTART)+48 )   /* 7088 = 0x1BB0 */
#define EV_MToSSyncEnable           (WORD16)( (EV_DBSTART)+49 )   /* 7089 = 0x1BB1 */
#define EV_MToSSyncEnableAck        (WORD16)( (EV_DBSTART)+50 )   /* 7090 = 0x1BB2 */
#define EV_CycleSaveEnable          (WORD16)( (EV_DBSTART)+51 )   /* 7091 = 0x1BB3 */
#define EV_CycleSaveEnableAck       (WORD16)( (EV_DBSTART)+52 )   /* 7092 = 0x1BB4 */
#define EV_ImmediatelySaveEnable    (WORD16)( (EV_DBSTART)+53 )   /* 7093 = 0x1BB5 */
#define EV_ImmediatelySaveEnableAck (WORD16)( (EV_DBSTART)+54 )   /* 7094 = 0x1BB6 */
#define EV_MtoSSyncDisable          (WORD16)( (EV_DBSTART)+55 )   /* 7095 = 0x1BB7 */
#define EV_MtoSSyncDisableAck       (WORD16)( (EV_DBSTART)+56 )   /* 7096 = 0x1BB8 */
#define EV_CycleSaveDisable         (WORD16)( (EV_DBSTART)+57 )   /* 7097 = 0x1BB9 */
#define EV_CycleSaveDisableAck      (WORD16)( (EV_DBSTART)+58 )   /* 7098 = 0x1BBA */
#define EV_ImmediatelySaveDisable   (WORD16)( (EV_DBSTART)+59 )   /* 7099 = 0x1BBB */
#define EV_ImmediatelySaveDisableAck  (WORD16)( (EV_DBSTART)+60 )   /* 7100 = 0x1BBC */
#define EV_ERRBACKCMDONMPSTART_ACK  (WORD16)( (EV_DBSTART)+63 )   /* 7103 = 0x1BBF MP忙，正在上电*/
#define EV_FTPLOADBUINFO_REQ        (WORD16)( (EV_DBSTART)+65 )   /* 7105 = 0x1BC1 由rx进程借用,大区制倒换时,加载指定目录请求消息 */
#define EV_FTPLOADBUINFO_ACK        (WORD16)( (EV_DBSTART)+66 )   /* 7106 = 0x1BC2 由rx进程借用,大区制倒换时,加载指定目录应答消息 */


/*应用消息的版本号定义*/
#define XOS_MSG_VER0            0				/**< 消息版本号0 */
#define XOS_MSG_VER1            1				/**< 消息版本号1 */
#define XOS_MSG_VER2            2				/**< 消息版本号2 */

#define XOS_FILE_ERR_BEGIN     0+0x400    
#define XOS_FILE_ERR_FILE_NOT_EXIST             (XOS_FILE_ERR_BEGIN +12) /**< 0x40c文件不存在 */


#define TIMER_NO_1       1 
#define TIMER_NO_2       2 
#define TIMER_NO_3       3 
#define TIMER_NO_4       4 
#define TIMER_NO_5       5 
#define TIMER_NO_6       6 
#define TIMER_NO_7       7 
#define TIMER_NO_8       8 
#define TIMER_NO_9       9 
#define TIMER_NO_10      10
#define TIMER_NO_11      11
#define TIMER_NO_12      12
#define TIMER_NO_13      13
#define TIMER_NO_14      14
#define TIMER_NO_15      15
#define TIMER_NO_16      16
#define TIMER_NO_17      17
#define TIMER_NO_18      18
#define TIMER_NO_19      19
#define TIMER_NO_20      20
#define TIMER_NO_21      21
#define TIMER_NO_22      22

#define PARAM_NULL   255

#define BOARD_DEPEND          0         /**< 非独立单板 */
#define INVALID_UNIT    65535     /**< 无效单元*/
#define INVALID_SUNIT   255       /**< 无效子单元*/
#define XOS_MSG_MEDIUM          2       /**< 中优先级消息 */

#define        OAM_CLI_PATH                      "./oam"
#define         FILE_PATH_LEN                   80           /**< 文件路径最大长度 */
#define         FILE_NAME_LEN                   80            /**< 文件名最大长度*/
#define         MAX_ABS_PATH_LEN           (FILE_NAME_LEN+FILE_PATH_LEN)    /**< 绝对路径最大长度 */

/*路由类型*/
#define COMM_INNER_UNIT             0 /**< 本逻辑单元实体内消息发送，确切讲是本处理器内部发送 */
#define COMM_MASTER_SERVICE     1 /**< 向主逻辑实体发送的业务消息 */
#define COMM_SLAVE_SERVICE        2 /**< 向备逻辑实体发送的业务消息 */
#define COMM_LEFT_CONTROL         3 /**< 向左逻辑实体发送的管理消息 */
#define COMM_RIGHT_CONTROL       4 /**< 向右逻辑实体发送的管理消息 */   
#define COMM_BACK_SERVICE       5 /**< 向后台发送的业务消息 */   
#define COMM_BACK_SLAVE_SERVICE      6 /**< 向后台备板发送的业务消息 */  

#define MUTEX_STYLE                 (WORD32)0x20        /**< 互斥，VxWorks对应的是互斥信号量，NT下对应的是互斥对象 */

#define XOS_RDONLY              "r"  /**< 文件只读  */
#define XOS_WRONLY             "w"  /**< 文件只写  */
#define XOS_RDWR                 "rw"  /**< 文件可读写 */
#define XOS_SYNCFS              (WORD32)0x0100  /*版本文件需要,确保元数据变动刷到硬盘*/
#define XOS_CREAT                 "w+"  /**< 如果文件不存在，则创建 */
#define XOS_TRUNC               (WORD32)0x0400  /**< 如果文件存在，则截断 */

typedef WORD32  XOS_STATUS;    
typedef FILE       *XOS_FD;        /**< 支撑定义的文件描述符 */
typedef WORD32  XOS_DIR;       /**< 支撑定义的目录描述符 */

#define SCS_VM_MGT                     5
#define  VER_TYPE_SCRIPT                (WORD16)9
#define OAMS_VER_MAX 50

#define  OAMS_VER_NEW       0   /* 新OAM脚本文件 */
#define  OAMS_VER_OLD       1   /* 老OAM脚本文件，只有FUNC_SCRIPT_CLIS_BRS可以请求老版本 */

/* 命令行脚本类型分配: 1~50 */
#define FUNC_SCRIPT_CLIS_OAM             (WORD16)1    /**< OAM命令行脚本 */
#define FUNC_SCRIPT_CLIS_PHYCFG          (WORD16)2    /**< 物理配置命令行脚本 */
#define FUNC_SCRIPT_CLIS_VMM             (WORD16)3    /**< VMM命令行脚本 */
#define FUNC_SCRIPT_CLIS_SIG             (WORD16)4    /**< SIG命令行脚本 */
#define FUNC_SCRIPT_CLIS_BRS             (WORD16)5    /**< BRS命令行脚本 */
#define FUNC_SCRIPT_CLIS_OAM_NOCFG       (WORD16)6    /**< OAM信令跟踪和告警模块等模块命令行脚本 */
#define FUNC_SCRIPT_CLIS_ATM             (WORD16)7    /**< ATM命令行脚本 */
#define FUNC_SCRIPT_CLIS_MCS             (WORD16)8    /**< MCS命令行脚本 */
#define FUNC_SCRIPT_CLIS_IP_DISP         (WORD16)9   /**< 分发表配置命令行脚本 */
#define FUNC_SCRIPT_CLIS_CMM             (WORD16)10  /**< CMM命令行脚本 */
#define FUNC_SCRIPT_CLIS_ATMCFG          (WORD16)11   /**< ATMCFG命令行脚本 */
#define FUNC_SCRIPT_CLIS_ALCAP           (WORD16)12   /**< ALCAP命令行脚本 */
#define FUNC_SCRIPT_DDMDAT_OAM             (WORD16)401 
#define FUNC_SCRIPT_DDM_OAM             (WORD16)401  
#define FUNC_SCRIPT_DDMDAT_VMM             (WORD16)403    /**< VMM DDMDAT  命令行脚本 */
#define XOS_SUCCESS                  0             
#define XOS_FAILED                     0xff      


#define XOS_SEEK_SET            SEEK_SET       /**< 从开始定位 */
#define XOS_SEEK_CUR            SEEK_CUR       /**< 从当前位置定位 */
#define XOS_SEEK_END            SEEK_END       /**< 从文件末尾定位 */

#define XOS_LEFT_BOARD           0      /**< 左板 */
#define XOS_RIGHT_BOARD          1      /**< 右板 */

#define BOARD_MASTER 1
#define BOARD_SLAVE  0

#define XOS_FILE_ERR_DIR_EXIST 100

#define XOS_INVALID_VOSSEM                      0xffffffff	/**< 无效semID*/
#define  XOS_VOS_SHM_EXIST (0 +12)
#define WAIT_FOREVER ((WORD32)0xFFFFFFFF)

typedef struct  
{
    WORD16    wVerType;                 
    BYTE      ucNewOrOld;                /**< 请求当前切换的版本或老版本，
                                          * \n 0为新版本;
                                          * \n 1为老版本；
                                          * \n 只有FUNC_SCRIPT_CLIS_BRS可以请求老版本 
                                          **/
    BYTE      ucVerNum;                  /**< 请求的版本数量 */
    WORD16    wFuncType[OAMS_VER_MAX];  
    BYTE      aucReserve[40];
} T_OamsVerInfoReq;

typedef struct 
{
    WORD16        wVerType;                 
    WORD16        wRecordsNum;                 /**< 本次查询返回的记录数 */
    BYTE          ucNewOrOld;                  /**< 请求当前正在运行的版本或老版本 
                                                * \n 0 为新版本
                                                * \n 1 为老版本，只有FUNC_SCRIPT_CLIS_BRS可以请求老版本 ；
                                                **/
    BYTE          aucPad[3];                   /**< 保留字段 */   
    int tOamsVerInfo[10];  /**< 版本信息，数组大小为50 */
    WORD32        dwRet;                       /**< 返回值
                                                * \n SCS_OK  0 请求成功；
                                                * \n SCS_ERR 1 请求失败
                                                **/
    BYTE          aucReserve[40];              /**< 保留字段 */   
} T_OamsVerInfoAck;


 typedef struct tagT_PhysAddress
 {
	 BYTE	 ucRackId;		 /**< rack no */
	 BYTE	 ucShelfId; 	 /**< shelf no */
	 BYTE	 ucSlotId;		 /**< slot no */ 
	 BYTE	 ucCpuId;		 /**< cpu no */  
 }T_PhysAddress;
 
typedef struct tagLogicalAddr
{
    WORD16  wSystem;       /**<   系统号 */
    WORD16  wModule;        /**< MP编号 */
    WORD16  wUnit;          /**< 单元号 */
    WORD16  wIndex;        /**< 索引号，在子单元号内部编号 */
    BYTE    ucSubSystem;    /**< 子系统号 */
    BYTE    ucSUnit;        /**< 单板上的处理器和业务处理芯片编号 */
    BYTE    ucPad[2];      /**< 填充字节，拼凑成DWORD*/
}T_LogicalAddr;

typedef struct
{
    WORD16  wModule;        /**< MP编号 */
    T_PAD   ucPad[2];       /**< 填充字段  */  
}_PACKED_1_ T_LogicalAddrM;
/**
  @struct	T_LogicalAddrU
  @brief	单元逻辑地址
*/
typedef struct
{
    WORD16  wUnit;          /**< 单元号   */
    BYTE    ucPad[2];       /**< 填充字段 */
    BYTE    ucSubSystem;    /**< 子系统号 */
    BYTE    ucSUnit;        /**< 单板上的处理器和业务处理芯片编号 */
    BYTE    ucPad1[2];      /**< 填充字段 */
}_PACKED_1_ T_LogicalAddrU;

typedef struct tagT_IPVXADDR
{
    BYTE   ucIpType;     /* IPADDR_IPNONE, IPADDR_IPV4, IPADDR_IPV6 */
    BYTE   ucPad;
    WORD16   wVpnId;   /* 0(INVALID VPNID), 1-65535(VALID VPNID) */
    BYTE   aucAddr[IPADDR_LEN_MAX];
}_PACKED_1_ T_IPVXADDR, * LP_T_IPVXADDR;


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
    WORD16    wVerType;                     
    WORD16    wFuncType;                  
    WORD32    dwVersionNo;                    /**< 版本的版本号 */
    CHAR      acVerFileName[50];   /**< 版本的文件名，数组大小为80，保活字符串结束符*/
    CHAR      acVerFilePath[50];   /**< 版本文件存放路径，数组大小为80，保活字符串结束符*/
    WORD32    dwFileSize;                     /**< 版本的文件大小 (剥离头后)*/
    WORD32    dwVerBodyCrc;                   /**< 版本的校验和（剥离头后的校验和）*/
    WORD32    dwVerHeadCrc;                   /**< 文件头校验和，目前不用，保留 */
    WORD32    dwMakeTime;                     /**< 版本制作时间 */
    BYTE      aucReserve[8];                  /**< 保留字段 */   
} T_OamsVerInfo;


typedef struct
{
    BOOL               ucNeedSaveAllFlg;  /**<  本数据库是否全部表存盘标记：TRUE：表示全部表存盘; FALSE：表示增量表存盘 */
    BYTE                 ucPad; /**< 填充字节 */
    WORD                 wTranId;/**< 仅供主备存盘时，主向备发起的存盘请求中使用，用于标识备机是对哪次存盘请求的应答 */
    WORD32               dwReserve; /**< 预留字段，八字节对齐 */
    CHAR                 dbName[50];/**< 数据库名称 */
}_PACKED_1_  _db_t_save_db_item, * lp_db_t_save_db_item;
/** 
@brief   数据库存盘请求信息，延时存盘也是用此结构体
*/
typedef struct
{
    JID                   BackMgtJid;   /**< 接收存盘应答的job的JID*/
    BOOL                isAllDbSave;  /**< 是否本CPU的所有库都存盘，TRUE：本CPU的所有数据库都存盘; FALSE：表示ucDatabaseNum和tDatabaseSave代表的数据库存盘*/
    BYTE                  ucMode;       /**< 存盘请求模式，参见: ::_DB_SAVE_MODE_LOCAL，::_DB_SAVE_MODE_MS，::_DB_SAVE_MODE_M2S */
    WORD                  wDbNum;      /**< 本次存盘的数据库实例数目,目前一个CPU上最多64个数据库*/
    BOOL                isReport;    /**< 是否向omc进行进度上报> */
    BYTE                  ucTrigger;   /**< 存盘请求的触发类型，参见：::_DB_SAVE_FROM_NORMAL,::_DB_SAVE_FROM_DELAY */
    BYTE                  aucPad[2];   /**< 预留字段，8字节对齐> */
    _db_t_save_db_item    tDatabaseSave[100]; /**< 各数据库的具体存盘请求信息， 参见: ::  _db_t_save_db_item*/
    
}_PACKED_1_  _db_t_back_save_req, * lp_db_t_back_save_req;
#define pNZDB_IO                     ((WORD16)500)
#define   EV_SAVETABLE        (EV_DBIO + 0)
#define   EV_SAVEOK           (EV_DBIO + 1)
#define   EV_SAVEFAIL         (EV_DBIO + 2)

#define XOS_ERR_UNKNOWN        0+1         
#define XOS_ERROR_UNKNOWN    XOS_ERR_UNKNOWN  

#define SYNC_SEM_STYLE              (WORD32)0x10        /**< 同步信号量 */
#define MUTEX_STYLE                 (WORD32)0x20        /**< 互斥，VxWorks对应的是互斥信号量，NT下对应的是互斥对象 */


typedef struct tagPID
{
    WORD32  dwPno;          /**< JOB号( 对应V3 进程号), 高16位为JOB 类型, 低16位为JOB 实例号  */
    WORD32  dwDevId;       /**< 设备ID, 包含本局的局号信息, 高8位为局号，不支持传递其他信息*/
    WORD16  wModule;        /**< MP编号 */
    WORD16  wUnit;          /**< 单板号 */
    BYTE    ucSUnit;        /**< 单板上的处理器号 */
    BYTE    ucSubSystem;    /**< 子系统号 */
    BYTE    ucRouteType;    /**< 路由类型，区别业务/管理消息，左右板位，主备通信*/
    BYTE    ucExtendFlag;  /**< 扩展标志 */
}PID;

extern int XOS_SendAsynMsg(WORD32   dwMsgId,
                            BYTE     *pucData,
                            WORD16   wMsgLen,
                            BYTE     ucVer,
                            BYTE     ucPriority,
                            void     *pReceiver);
extern int XOS_SysLog(BYTE ucLevel,CHAR* pcFmt, ...);
#define BOARD_MASTER 1
#define BOARD_SLAVE  0
extern int XOS_GetBoardMSState() ;
extern void XOS_SetNextState(WORD16 wJno, WORD16 wState);
extern WORD32 XOS_GetConstParaOfCurTimer(LPVOID);
extern int  XOS_Msend(WORD32    dwMsgId,
                      BYTE      *pucData,
                      WORD16    wMsgLen,
                      BYTE      ucVer,
                      BYTE      ucPriority,
                      void      *pReceiver)
                      ;
extern WORD16  XOS_InvertWord16(WORD16 wWord16);
extern void XOS_SetDefaultNextState();
extern XOS_STATUS XOS_GetSelfJID(void *pvJid) ;
extern WORD32     XOS_ConstructJNO(WORD16 wJobType, WORD16 wInstanceNo) ;

#define XOS_FILE_ERR_DIR_EXIST 100
extern XOS_STATUS XOS_CreateDirectoryEx(CHAR *pcDirName) ;

#define INVALID_TIMER_ID 255

extern BYTE *XOS_GetUB(WORD32 dwSize);
extern XOS_TIMER_ID XOS_SetLoopTimer(WORD16 wJno,WORD32 dwTimerNo, SWORD32 sdwDuration, WORD32 dwParam);
extern XOS_TIMER_ID  XOS_SetRelativeTimer(WORD16 wJno, WORD32 dwTimerNo, SWORD32 sdwDuration, WORD32 dwParam);
extern XOS_STATUS  XOS_Sender(void *pSender);
extern void XOS_GetCurrentTime(T_Time *ptTime);
extern XOS_STATUS XOS_CloseFile(XOS_FD sdwFd);
extern XOS_STATUS XOS_GetMsgDataLen(WORD32 dwJno, WORD16 *pwMsgLen);
extern XOS_STATUS XOS_KillTimer(WORD16 wJno, WORD32 dwTimerNo,WORD32 dwParam);
extern XOS_STATUS XOS_KillTimerByTimerId(XOS_TIMER_ID dwTimerId);
extern WORD32  XOS_InvertWord32(WORD32 dwWord32);
extern XOS_STATUS XOS_DeleteFile(CHAR *pcFileName);
extern XOS_STATUS XOS_Delay(WORD32 dwTimeout);
extern XOS_STATUS XOS_GetBoardStyle(BYTE*  ucBoardStyle);
extern WORD32 XOS_CreateProcessSem(WORD32 index,WORD32 dwInitVal,WORD32 dwFlag );
#define XOS_INVALID_VOSSEM                      0xffffffff	/**< 无效semID*/
#define  XOS_VOS_SHM_EXIST (0 +12)

#ifndef WAIT_FOREVER
# define WAIT_FOREVER ((WORD32)0xFFFFFFFF)
#endif  /* WAIT_FOREVER */

#ifndef NO_WAIT
# define NO_WAIT ((WORD32)0)
#endif  /* NO_WAIT */

#define INVALID_SYS_TASKID          (XOS_TASK_T)0           /**< 无效的系统任务ID */

extern XOS_STATUS XOS_CreateShmByIndex(WORD32   index, WORD32 tShareSize, WORD32 dwFlag, BYTE **ppucShareBuf);
extern BOOLEAN XOS_ProcessSemP(WORD32 semid,WORD32 dwTimeOut);
extern BOOLEAN XOS_ProcessSemV(WORD32 semid);
extern XOS_STATUS XOS_GetSysClock(T_SysSoftClock *ptSysClock);
extern WORD16 XOS_GetCurState(WORD32 dwJno);
extern XOS_STATUS XOS_SendAsynMsgNJ(WORD32   dwMsgId,
                             BYTE     *pucData,
                             WORD16   wMsgLen,
                             BYTE     ucVer,
                             BYTE     ucPriority,
                             void     *pSender,
                             void     *pReceiver
                             );
extern XOS_STATUS XOS_OpenFile(CHAR* pcFileName,CHAR * sdwFlag,XOS_FD *ptFd);
extern XOS_STATUS XOS_WriteFile(XOS_FD sdwFd, void *pBuffer, SWORD32 sdwCount, SWORD32 *psdwCountBeWrite);
extern XOS_STATUS XOS_GetFileLength(CHAR *pcFileName,SWORD32 *psdwLen );
extern XOS_STATUS XOS_ReadFile(XOS_FD sdwFd, void *pBuffer, SWORD32 sdwCount, SWORD32 *psdwCountBeRead);
extern XOS_STATUS XOS_SeekFile(XOS_FD sdwFd, SWORD32 sdwOrigin, SWORD32 sdwOffset);
extern XOS_STATUS XOS_RenameFile(CHAR *pcOldFileName, CHAR *pcNewFileName);
extern XOS_STATUS XOS_GetBoardPosition(BYTE *pucBoardPosition);
extern WORD64  XOS_InvertWord64(WORD64 qwWord64);
extern XOS_STATUS XOS_OpenDir(CHAR *pcDirName,XOS_DIR *ptDir);
extern XOS_STATUS XOS_CreateDirectory(CHAR *pcDirName);
extern XOS_STATUS XOS_CloseDir(XOS_DIR sdwDirIndex);
extern XOS_STATUS XOS_TimeToClock(T_Time *ptSecMillisec,T_SysSoftClock *ptClock);

typedef struct tagT_ShellCallBack
{
    XOS_STATUS (*pRecvFunc)(BOOLEAN,CHAR); /* 输入字符接收回调 */
    XOS_STATUS (*pErrProcFunc)(BOOLEAN);   /* 输入错误处理回调 */
    BYTE        ucTulipBgPrintLevel;       /* 设置ushell disabled时R_Printf的打印级别 */
    BOOLEAN     bConExcClosed;             /* ushell disabled时是否连异常打印都关闭? */
    BYTE        ucPad[2];
}T_ShellCallBack;

XOS_STATUS XOS_TakeOverConsole(T_ShellCallBack *ptShellCallBack,BOOLEAN bEnableUshell);
BOOLEAN XOS_CreateSemaphore(
                            WORD32 dwInitVal,
                            WORD32 dwMaxVal,
                            WORD32 dwFlag,
                            WORD32 *pdwSemID
                            );

typedef VOID (*TaskEntryProto)(void *);
XOS_TASK_T XOS_StartTask(
                     CHAR           *pucName,
                     WORD16         wPriority,
                     WORD32         dwStacksize,
                     SWORD32        sdwOptions,
                     TaskEntryProto tTaskEntry,
                     WORDPTR        ptTaskPara1
                     );
BOOLEAN XOS_SemaphoreP(WORD32 dwSemID, WORD32 dwTimeOut);
BOOLEAN XOS_SemaphoreV(WORD32 dwSemID);
BOOLEAN XOS_DeleteTask(XOS_TASK_T TaskId);
XOS_STATUS oss_retub(BYTE *pucBuf);
XOS_STATUS XOS_RetUB(BYTE *pucBuf);


typedef struct tObj_MutexType
{
    union
    {
        pthread_mutex_t   mutex;
        sem_t             sem;
        pthread_rwlock_t  rwlock;
    }obj;
    WORD32            dwFlag;
    WORD32            dwPno;
    WORD32            SvrId;            /* 拥有此进程内信号量的jno*/
    WORD32            dwMsgId;          /* 拥有此进程内信号量的msgid*/
    WORD32            dwTaskId;         /* 拥有此进程内信号量的线程tid */
    WORD32            dwPtick;          /* 进程内信号量P操作成功发生的tick值 */
    WORD32            dwMode;
    WORD32            dwCreate;
    INT               iErrno;
    BYTE              ucUsed;
    BYTE              ucRwFlag; 
    BYTE              ucPad[2];
}T_Obj_MutexType;

/* 2.2 信号量操作所用宏 */
#define VOS_SEM_CTL_USED               (BYTE)0x01  /* 队列控制块被使用 */
#define VOS_SEM_CTL_NO_USED         (BYTE)0x10  /* 队列控制块未被使用 */

extern T_Obj_MutexType *g_atMutex;
extern int g_iSyslogLevel;

/* 4. 时钟操作所用宏 */
#define MSINSECOND                  1000

SWORD32 XOS_snprintf(CHAR       *pcBuffer,
                          WORD32     dwMaxCount,
                          const char *pcFmt,
                          ...);


#define XOS_INVERT_WORD16(x) \
            ((((x) & 0x00ff) << 8) | \
            (((x) & 0xff00) >> 8))

#define XOS_INVERT_WORD32(x) \
            ((((x) & 0x000000ff) << 24) | \
            (((x) & 0x0000ff00) <<  8) | \
            (((x) & 0x00ff0000) >>  8) | \
            (((x) & 0xff000000) >> 24))

#define XOS_INVERT_WORD64(x) \
            ((((x) & 0x00000000000000ffull) << 56) | \
            (((x) & 0x000000000000ff00ull) <<  40) | \
            (((x) & 0x0000000000ff0000ull) <<  24) | \
            (((x) & 0x00000000ff000000ull) <<  8) | \
            (((x) & 0x000000ff00000000ull) >>  8) | \
            (((x) & 0x0000ff0000000000ull) >>  24) | \
            (((x) & 0x00ff000000000000ull) >>  40) | \
            (((x) & 0xff00000000000000ull) >> 56))
            

#define TSK_PRIO_PTYS_CLI                    60

#endif

