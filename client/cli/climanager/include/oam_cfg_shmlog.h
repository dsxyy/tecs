/*********************************************************************
* 版权所有 (C)2002, 深圳市中兴通讯股份有限公司。
*
* 文件名称：oam_cfg_shmlog.h
* 文件标识：
* 内容摘要：共享内存运行日志模块
* 其它说明：
            
* 当前版本：
* 作    者       ：殷浩
* 完成日期：20090604
*
* 修改记录1：// 修改历史记录，包括修改日期、修改者及修改内容
*    修改日期：
*    版 本 号：
*    修 改 人：
*    修改内容：
* 修改记录2：…
**********************************************************************/
#ifndef __OAM_CFG_SHMLOG_H__
#define __OAM_CFG_SHMLOG_H__

#ifndef WIN32
    #ifndef _PACKED_1_
        #define _PACKED_1_ __attribute__ ((packed)) 
    #endif
#else
    #ifndef _PACKED_1_
        #define _PACKED_1_
    #endif
#endif

#ifdef WIN32
    #pragma pack(1)
#endif

/*共享内存中保存的日志记录数*/
#define OAM_MAX_SHM_LOG_NUM         1000

/*单条日志记录结构*/
typedef struct tag_T_OamCfgShmlogRecord
{
    BYTE         ucMSState;    /*climanager 主备状态*/
    BYTE         ucPad;
    WORD16  wJobState;    /*climanager JOB 状态*/
    WORD32  dwMsgId;       /*climanager 收到的消息id*/
}_PACKED_1_  T_OamCfgShmlogRecord;

/*共享内存结构*/
typedef struct tag_T_OamCfgLogShm
{
    WORD32                            dwSemid;    /*访问互斥信号量*/
    WORD32                            dwCurIdx;   /*日志数组当前可用下标*/
    T_OamCfgShmlogRecord   atLogs[OAM_MAX_SHM_LOG_NUM]; /*日志数组*/
}_PACKED_1_  T_OamCfgLogShm;

extern void Oam_CfgWriteShmLog(WORD16 wState, WORD32 dwMsgId);

#ifdef WIN32
    #pragma pack()
#endif

#endif /* __OAM_CFG_SHMLOG_H__*/
 
