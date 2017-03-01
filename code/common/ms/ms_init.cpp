/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�ms_init.cpp
* �ļ���ʶ��
* ����ժҪ����������������������ش���
* ��ǰ�汾��1.0
* ��    �ߣ�Ԭ����
* ������ڣ�2011��9��23��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/9/23
*     �� �� �ţ�V1.0
*     �� �� �ˣ�Ԭ����
*     �޸����ݣ�����
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
    WORD32                     nodeid;      /* �÷�������б��               */
    WORD32                     dwNum;       /* �÷������͵ĳ�Ա����           */
    WORD32                     dwUser;      /* �û���ʶ�����ֶ���̣�������չ */
    WORD32                     dwPad;       /* ��չ�ֶ�                       */    
    SaLckLockIdT               lock_id;     /* ȫ�������                     */
    SaLckResourceHandleT       resHnd;      /* lck  resource handle           */
    SaCkptCheckpointHandleT    ckptHnd;     /* ckpt resource  handle          */
}T_MS_CkptData;

typedef struct tagMS_CoreData
{
    uint32                     dwserv;      /* �÷�������б��               */
    uint32                     dwms_N;      /* �÷������͵ĳ�Ա����           */
    uint32                     dwms_M;      /* �÷������ʹ���active�ĳ�Ա���� */
    uint32                     nodeid;      /* ȫ�ֽڵ��ʶ�����ּ�Ⱥ�ڽڵ�   */
    uint32                     dwUser;      /* �����û���ʶ�����ֱ��ض����   */
    SaLckLockIdT               lock_id;     /* ����ȫ�������                     */
    SaCkptHandleT              ckptHandle;  /* �÷����CKPT�ͻ���             */
    SaLckHandleT               lckhandle;   /* �÷����Ӧ��ȫ�ֲַ�ʽ��       */
    SaEvtHandleT               evthandle;   /* �÷����Ӧ���¼�����           */
    SaNameT                    *ptSaName;   /* �÷������Ͷ�Ӧ��checkpoint�б� */
    SaNameT                    *ptlkName;   /* �÷������Ͷ�Ӧ��ȫ�����б�     */
    SaNameT                    *ptevtName;  /* �÷������Ͷ�Ӧ���¼�����       */
    T_MS_CkptData              *ptCkpt;     /* �÷������Ͷ�Ӧ��checkpoint���� */
    SaEvtChannelHandleT        evtChHnd;    /* event resource  handle        */
    pthread_t                  dispatch_th; /* �¼������߳�                   */
    pthread_t                  scan_th;     /* ���ɨ���߳�                   */
    uint32                     dwflags;     /* ������ʶ                       */
    MS_NoticeCallback          usercall;    /* �û������߳�                   */
    uint32                     dwType;      /* ��������                       */
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

