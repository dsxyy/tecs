/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：ms_init.cpp
* 文件标识：
* 内容摘要：进程启动流程中主备相关处理
* 当前版本：1.0
* 作    者：袁进坤
* 完成日期：2011年9月23日
*
* 修改记录1：
*     修改日期：2011/9/23
*     版 本 号：V1.0
*     修 改 人：袁进坤
*     修改内容：创建
*******************************************************************************/
#include "ms_corelib.h"
#include "tecs_config.h"

#include "../include/saAis.h"
#include "../include/saCkpt.h"
#include "../include/saClm.h"
#include "../include/saLck.h"
#include "../include/saEvt.h"
#include <pthread.h>

using namespace std;
typedef struct tagMS_CkptData
{
    WORD32                     nodeid;      /* 该服务的序列编号               */
    WORD32                     dwNum;       /* 该服务类型的成员数量           */
    WORD32                     dwUser;      /* 用户标识，区分多进程，后续扩展 */
    WORD32                     dwPad;       /* 扩展字段                       */    
    SaLckLockIdT               lock_id;     /* 全局锁编号                     */
    SaLckResourceHandleT       resHnd;      /* lck  resource handle           */
    SaCkptCheckpointHandleT    ckptHnd;     /* ckpt resource  handle          */
}T_MS_CkptData;

typedef struct tagMS_CoreData
{
    uint32                     dwserv;      /* 该服务的序列编号               */
    uint32                     dwms_N;      /* 该服务类型的成员数量           */
    uint32                     dwms_M;      /* 该服务类型处于active的成员数量 */
    uint32                     nodeid;      /* 全局节点标识，区分集群内节点   */
    uint32                     dwUser;      /* 本地用户标识，区分本地多进程   */
    SaLckLockIdT               lock_id;     /* 本地全局锁编号                     */
    SaCkptHandleT              ckptHandle;  /* 该服务的CKPT客户端             */
    SaLckHandleT               lckhandle;   /* 该服务对应的全局分布式锁       */
    SaEvtHandleT               evthandle;   /* 该服务对应的事件发布           */
    SaNameT                    *ptSaName;   /* 该服务类型对应的checkpoint列表 */
    SaNameT                    *ptlkName;   /* 该服务类型对应的全局锁列表     */
    SaNameT                    *ptevtName;  /* 该服务类型对应的事件发布       */
    T_MS_CkptData              *ptCkpt;     /* 该服务类型对应的checkpoint内容 */
    SaEvtChannelHandleT        evtChHnd;    /* event resource  handle        */
    pthread_t                  dispatch_th; /* 事件服务线程                   */
    pthread_t                  scan_th;     /* 伙伴扫描线程                   */
    uint32                     dwflags;     /* 倒换标识                       */
    MS_NoticeCallback          usercall;    /* 用户处理线程                   */
    uint32                     dwType;      /* 进程类型                       */
}T_MS_CoreData;

extern T_MS_CoreData       *g_ptMS_CoreData;
uint32 MsCall(T_MS_CallArg *ptArgs)
{
    if (1 != ms_qry(TRUE))
    {
        cout << "MS: give up master, process reboot." << endl;
        exit(-1);
    }
    return 0;
}

int MsInit(const MsConfig &msconfig)
{
    SkyAssert(DEFAULT_MS_SERV_NOT_CFG != msconfig.backup_serv);
    SkyAssert(DEFAULT_MS_BACKUP_COUNT != msconfig.backup_count);

    uint32 dwUser = msconfig.backup_inst;
    if (DEFAULT_MS_INST_NOT_CFG == (int32)dwUser)
    {
        TecsConfig *config = TecsConfig::Get();
        SkyAssert(NULL != config);
        if (SUCCESS != GetIpAddress(config->get_management_interface(), dwUser))
        {
            return ERROR;
        }
        config->SetBackupInst((int32)dwUser);        
    }
    
    if(0xffffffff == ms_init(msconfig.backup_serv, 1, msconfig.backup_count, dwUser, 0))
    {
        cout << "ms_init fail, check depend service is ok.process reboot." << endl;
        return ERROR;;
    }
    ms_registercall(MsCall);

    cerr << "MS: waite for getting master......";
    for (;;)
    {
        if (1 == ms_qry(TRUE))
        {
            cerr << endl << "MS: get master, process startting." << endl;
            break;
        }
        sleep(1);
        cerr << ".";
    }

    return SUCCESS;
}

