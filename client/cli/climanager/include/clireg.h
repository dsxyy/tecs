
#ifndef _OAM_CLI_REGISTER_H_
#define _OAM_CLI_REGISTER_H_

#include "pub_oam_cfg_event.h"

/* 信号量、共享内存名定义 */
#if 0
#define  OAM_SEM_CLI_REG                  "OAM_SemCliReg"       /* 平台应用注册DAT函数使用的信号量   */
#define  OAM_SHM_CLI_REG                  "OAM_ShmCliReg"       /* 平台应用注册DAT信息使用的共享内存 */
#define  OAM_SEM_CLI_REGEX                "OAM_SemCliRegEx"     /* 非平台业务注册使用的信号量   */
#define  OAM_SHM_CLI_REGEX                "OAM_ShmCliRegEx"     /* 非平台业务注册使用的共享内存 */
#endif

/* 共享内存块大小 */
/*#define  OAM_CLI_DAT_SHM_MAX_NUM                     100*/ /* 单板上平台应用使用的共享内存块 */
/*#define  OAM_CLI_APP_AGT_SHM_MAX_NUM                  1*/  /* 单板上非平台使用的共享内存块，每块单板只支持注册一个业务进程 */
/*#define  OAM_CLI_APP_MGT_SHM_MAX_NUM                 50*/  /* OMP上非平台使用的共享内存块 */

/*由于omp和其他单板支持的注册数目不一样，取大的作为共享内存中记录数目*/
/*#define  OAM_CLI_APP_SHM_NUM  (OAM_CLI_APP_MGT_SHM_MAX_NUM > OAM_CLI_APP_AGT_SHM_MAX_NUM ? OAM_CLI_APP_MGT_SHM_MAX_NUM : OAM_CLI_APP_AGT_SHM_MAX_NUM)*/

/*T_CliAppData中ucReadFlag的取值*/
#define  OAM_CLI_REG_NOT_READED        0
#define  OAM_CLI_REG_READED            1

/* 注册类型 */
#define  OAM_CLI_REG_TYPE_INVALID        0
#define  OAM_CLI_REG_TYPE_3GPLAT         1
#define  OAM_CLI_REG_TYPE_SPECIAL        2

#if 0
/* 注册DAT函数使用的共享内存池，平台使用 */
typedef struct
{
    JID             tAppJid;          /* 应用注册的JID */
    WORD32          dwDatFuncType;    /* 应用注册的DAT的功能版本类型 */
    BYTE          ucReadFlag;     /* Manager是否读取的标记 */
    BYTE          ucCmpFlag;       /* 是否已经比较过的标记 */
    BYTE          ucSaveType;     /*存盘类型1-TXT存盘；0-ZDB存盘*/
    BYTE          ucPad[1];
} T_CliAppData;

typedef struct
{
    WORD32          dwSemid;
    pid_t           platpid;                             /*管理进程pid，用来判断是否要清共享内存*/
    WORD32          dwRegCount;                            /* 当前注册个数 */
    T_CliAppData    tCliReg[OAM_CLI_DAT_SHM_MAX_NUM];      /* 注册数据 */
} T_CliAppReg;

/* 非平台业务注册，非平台使用 */
typedef struct
{
    JID             tAppJid;      /* 应用注册的JID */
    WORD32          dwNetId;    /* 应用注册的网元标识*/
    BYTE           ucAgingTime;   /* 此注册信息老化时间次数，如果超过3次同步时间，则认为老化 */
    BYTE           ucPad[3];
} T_CliAppDataEx;

typedef struct
{
    WORD32           dwSemid;                                                 /*信号量*/
    pid_t            apppid;                                                     /*管理进程pid，用来判断是否要清共享内存*/
    WORD32          dwRegCount;                            /* 当前注册个数 */
    T_CliAppDataEx  tCliReg[OAM_CLI_APP_SHM_NUM];     /* 注册数据 */
} T_CliAppAgtRegEx;
#endif

extern T_CliAppReg* Oam_GetRegTableAndLock(void);
extern BOOLEAN Oam_UnLockRegTable(T_CliAppReg *ptOamCliReg );
extern T_CliAppAgtRegEx* Oam_GetRegExTableAndLock(void);
extern BOOLEAN Oam_UnLockRegExTable(T_CliAppAgtRegEx *ptOamCliReg );

#endif

