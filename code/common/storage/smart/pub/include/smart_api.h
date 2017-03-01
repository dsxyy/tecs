/*************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司
* All rights reserved.
* 
* 文件名称：smart_api.h
* 文件标识：见配置管理计划书
* 内容摘要：磁盘检测模块接口头文件
* 当前版本：1.0
* 作    者：guojsh
* 完成日期：2011年8月20日
*
* 修改记录1：
*  修改日期：2013-7-25
*  版 本 号：2.0
*  修 改 人：谢涛涛
*  修改内容：改用smartctl命令脚本的方式
*************************************************************************/

#ifndef _SMART_API_H
#define _SMART_API_H

/* 磁盘的健康状态相关，即SMART状态 */
/* old */
#define PD_GOOD                      1   /* 磁盘状态完好 */
#define PD_BROKEN                    0   /* 磁盘状态故障 */
/* new */
#define PD_STATUS_NORMAL             1   /* 正常 */
#define PD_STATUS_WARN               2   /* 警告, 可修复 */
#define PD_STATUS_FAULT              3   /* 故障, 需立即备份数据并更换硬盘 */

#define PD_SUPPORT_SMART             1   /* 支持SMART */
#define PD_NOT_SUPPORT_SMART         0   /* 不支持支持SMART */

#define PD_SMART_ON                  1   /* SMART打开 */
#define PD_SMART_OFF                 0   /* SMART关闭 */

#define PD_SHORT_SELFTEST            1    /* 短检测 */
#define PD_LONG_SELFTEST             2    /* 长检测 */

/* 失败码 */
#define _SMART_SUCCESS               0
#define _SMART_E_RW                  1
#define _SMART_E_GET_STATUS          2
#define _SMART_E_SELFTEST            3
#define _SMART_ERROR                -1 

#define DM_PD_NAME_LEN               12    /* 设备名称长度 */

#define DISK_TYPE_ATA                1 /* ATA 硬盘 */
#define DISK_TYPE_SCSI               2 /* SCSI 硬盘 */

/* 单个磁盘的SMART信息 */
typedef struct pdSmartInfo_t
{   /* 该结构不要增加对象或容器变量等 */
    char aucPdName[DM_PD_NAME_LEN];   /* out 磁盘设备名 如 /dev/sda */
    char ucDiskType;                  /* out 磁盘类型 */
    char ucIsSupportSmart;            /* out 磁盘是否支持SMART */
    char ucIsSmartEnable;             /* out 磁盘SMART功能是否开启 */
    char ucSmartStatus;               /* out 磁盘的SMART状态是否OK */
    int sdwSelfTestTime;              /* out 执行自检所需时间 */
} T_DmPdSmartInfo;


#endif


