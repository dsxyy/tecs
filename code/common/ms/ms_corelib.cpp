/*********************************************************************
 * ��Ȩ���� (C)2007, ����������ͨѶ�ɷ����޹�˾��
 *
 * �ļ����ƣ� ms_corelib.c
 * �ļ���ʶ�� 
 * ����˵���� ����������
 * ��ǰ�汾�� V1.0
 * ��    �ߣ� 
 * ������ڣ� 
 *
 * �޸ļ�¼1��
 *    �޸����ڣ�2011��07��26��
 *    �� �� �ţ�V1.0
 *    �� �� �ˣ�New
 *    �޸����ݣ�����
 **********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/un.h>
#include <sys/time.h>
#include <time.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/poll.h>
#include "../include/saAis.h"
#include "../include/saCkpt.h"
#include "../include/saClm.h"
#include "../include/saLck.h"
#include "../include/saEvt.h"
#include "ms_corelib.h"
#include "../../sky/include/sky.h"

#define   MS_Malloc       malloc
#define   MS_Free         free
#define   MS_Printf       printf

#define   MS_ACTIVE_N        ((uint32)1)   /* N+M���ݣ�Ĭ��1+1,N=1 */
#define   MS_STANDBY_M       ((uint32)1)   /* N+M���ݣ�Ĭ��1+1,M=1 */
#define   MS_EVT_LEN         ((uint32)20)  /* �¼����ɳ���         */


typedef struct tagMS_CkptData
{
    uint32                     nodeid;      /* �÷�������б��               */
    uint32                     dwNum;       /* �÷������͵ĳ�Ա����           */
    uint32                     dwUser;      /* �û���ʶ�����ֶ���̣�������չ */
    uint32                     dwPad;       /* ��չ�ֶ�                       */    
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

typedef struct tagMS_EVENT
{
    uint32                     dwserv;      /* �÷�������б��                */
    uint32                     nodeid;      /* ȫ�ֽڵ��ʶ�����ּ�Ⱥ�ڽڵ�    */
    uint32                     dwUser;      /* �����û���ʶ�����ֱ��ض����    */
    uint32                     dwType;      /* �¼�����                       */
    uint32                     id;          /* �Զ����ʶ */
    uint32                     dwlen;       /* �¼����ȣ����ݿ���             */
    uint32                     dwData[MS_EVT_LEN];  /* �¼��������� */
    
}T_MS_Event;

SaCkptCheckpointCreationAttributesT checkpointCreationAttributes = {
	SA_CKPT_WR_ALL_REPLICAS,
	100000,
	SA_TIME_ONE_SECOND,
	1,
	200000,
	20
};

/* ע���ڴ渲����������ȡT_MS_CkptDataǰ����ֵ */
SaCkptIOVectorElementT ReadVectorElements[MS_ACTIVE_N] = {
	{
		SA_CKPT_DEFAULT_SECTION_ID,
		0,
		sizeof (T_MS_CkptData)-sizeof(SaCkptCheckpointHandleT)-sizeof(SaLckResourceHandleT)-sizeof(SaLckLockIdT),
		0, 
		0
	}
};

#define _patt1      "ms_evt0"
#define patt1       ((byte *) _patt1)
#define patt1_size  sizeof(_patt1)
SaEvtEventPatternT evt_patts[] = {
		{patt1_size, patt1_size, patt1}
		};
SaEvtEventPatternArrayT	evt_pat_array = { sizeof(evt_patts)/sizeof(SaEvtEventPatternT), sizeof(evt_patts)/sizeof(SaEvtEventPatternT), evt_patts };
SaEvtEventFilterT ms_filters[] = {
	{SA_EVT_PREFIX_FILTER, {patt1_size, patt1_size, patt1}}

};

SaEvtEventFilterArrayT subscribe_filters = {
	sizeof(ms_filters)/sizeof(SaEvtEventFilterT),
	ms_filters 
};
SaEvtSubscriptionIdT my_subscription_id = 0x1234;

/*********************��������*********************************/
void    OpenCallBack (SaInvocationT invocation, const SaCkptCheckpointHandleT ckptHnd, SaAisErrorT error);
void    ms_LckResourceOpenCallback (SaInvocationT invo, SaLckResourceHandleT lockResourceHandle,SaAisErrorT error);
void    ms_LckLockGrantCallback (SaInvocationT invo,SaLckLockStatusT lockStatus,SaAisErrorT error);
void    ms_LckLockWaiterCallback (SaLckWaiterSignalT waiterSignal,SaLckLockIdT lockId,SaLckLockModeT modeHeld,SaLckLockModeT modeRequested);
void    ms_LckResourceUnlockCallback (SaInvocationT invocation, SaAisErrorT error);
void    ms_evtopencallback(SaInvocationT invocation,SaEvtChannelHandleT channelHandle,	SaAisErrorT error);
void    ms_evtcallback(SaEvtSubscriptionIdT subscriptionId,	const SaEvtEventHandleT eventHandle,const SaSizeT eventDataSize);
uint32  ms_evt(uint32 id, byte *p, uint32 len);
uint32  ms_evtSub(uint32 id);
void    SyncCallBack (SaInvocationT invocation, SaAisErrorT error);
void    *msscan_thread(byte *arg);
void    *evt_dispatch(byte *arg);
void    Vos_Delay(uint32 dwTimeout);
uint32  ms_compete(uint32 dwType);

SaLckCallbacksT ms_lckcallbacks = {
	ms_LckResourceOpenCallback,
	ms_LckLockGrantCallback,
	ms_LckLockWaiterCallback,
	ms_LckResourceUnlockCallback
};
SaEvtCallbacksT ms_evtcallbacks = {
	ms_evtopencallback,
	ms_evtcallback
};
/****************ȫ�ֱ�������************************************/

T_MS_CoreData       *g_ptMS_CoreData  = NULL;  
SaCkptCallbacksT    MS_OpenCallback   = {&OpenCallBack, &SyncCallBack};
SaVersionT          MS_Version        = { 'B', 1, 1 };
SaInvocationT       open_invocation   = 16;
uint32              g_dwTimeOut       = 3000;
uint64              g_evtTimeOut      = 30000000000ULL; /* 30 seconds */
/* ���ú������� */
 

/**********************************************************************
 * �������ƣ�ms_init
 * ����������������ʼ��
 * ���ʵı���
 * �޸ĵı���
 * ���������uint32 dwServ   ��������
            uint32 ms_N      N+M�ġ�N��
            uint32 ms_M      N+M�ġ�M��
            uint32 dwUser    �û����
            uint32 dwType    ��չģ�����ͣ�������0
 * �����������
 * �� �� ֵ��
 * ����˵����
 * �޸�����      �汾��  �޸���      �޸�����
 * ---------------------------------------------------------------------
 * 2011/07/26    V1.0    New          ����
 ************************************************************************/
uint32  ms_init(uint32 dwServ, uint32 ms_N, uint32 ms_M, uint32 dwUser,uint32 dwType)
{
    uint32               dwRet = 0xffffffff;    
    SaAisErrorT          error;  
    SaClmHandleT         handle;
    SaClmClusterNodeT    clusterNode;  
    
    error  = saClmInitialize (&handle, NULL, &MS_Version);
    if(error != SA_AIS_OK)
    {
        MS_Printf("ms_init failed in saClmInitialize,err %d!\n",errno);
        goto  init_end;
    }
    error  = saClmClusterNodeGet (handle, SA_CLM_LOCAL_NODE_ID, SA_TIME_END, &clusterNode);
    if(error != SA_AIS_OK)
    {
        MS_Printf("ms_init failed in saClmClusterNodeGet,err %d!\n",errno);
    }
    error  = saClmFinalize (handle);
    
    g_ptMS_CoreData =  (T_MS_CoreData    *)MS_Malloc(sizeof(T_MS_CoreData));
    if (NULL == g_ptMS_CoreData)
    {
    	MS_Printf("ms_init failed in g_ptMS_CoreData,err %d!\n",errno);
    	goto  init_end;
    }
    
    memset(g_ptMS_CoreData, 0, sizeof(T_MS_CoreData));
    g_ptMS_CoreData->dwms_N    = ms_N;
    g_ptMS_CoreData->dwms_M    = ms_M;
    g_ptMS_CoreData->dwserv    = dwServ;
    g_ptMS_CoreData->nodeid    = clusterNode.nodeId;   /* �����޸�ΪCLM��nodeid,�Ѿ��޸� */
    g_ptMS_CoreData->dwUser    = dwUser;
    g_ptMS_CoreData->dwType    = dwType;
    
    /* config N+M check */
    if((ms_N ==0) && (ms_M ==0))
    {
    	g_ptMS_CoreData->dwms_N    = MS_ACTIVE_N;
    	g_ptMS_CoreData->dwms_M    = MS_STANDBY_M;
    }
    else if((ms_N == 0) || (ms_M == 0))
    {
        MS_Printf("ms_init failed in N+M check,err %d!\n",errno);
        goto  init_end;
    }
    MS_Printf("ms_init in N+M check,%d + %d!\n",g_ptMS_CoreData->dwms_N,g_ptMS_CoreData->dwms_M);
    
    g_ptMS_CoreData->ptlkName = (SaNameT*)MS_Malloc(g_ptMS_CoreData->dwms_N * sizeof(SaNameT));
    g_ptMS_CoreData->ptSaName = (SaNameT*)MS_Malloc(g_ptMS_CoreData->dwms_N * sizeof(SaNameT));
    g_ptMS_CoreData->ptevtName = (SaNameT*)MS_Malloc(g_ptMS_CoreData->dwms_N * sizeof(SaNameT));    
    g_ptMS_CoreData->ptCkpt   = (T_MS_CkptData*)MS_Malloc(g_ptMS_CoreData->dwms_N * sizeof(T_MS_CkptData));
    
    if((g_ptMS_CoreData->ptSaName == NULL)||(g_ptMS_CoreData->ptCkpt == NULL)|| \
       (g_ptMS_CoreData->ptlkName == NULL)||(g_ptMS_CoreData->ptevtName==NULL))
    {
        MS_Printf("ms_init failed in ptSaName or ptCkpt,err %d!\n",errno);
    	goto  init_end;
    }
    memset(g_ptMS_CoreData->ptlkName, 0, g_ptMS_CoreData->dwms_N * sizeof(SaNameT));
    memset(g_ptMS_CoreData->ptSaName, 0, g_ptMS_CoreData->dwms_N * sizeof(SaNameT));
    memset(g_ptMS_CoreData->ptevtName,0, sizeof(SaNameT));  /* ͬһ����Ľڵ㣬����һ���¼�ͨ�� */
    memset(g_ptMS_CoreData->ptCkpt,   0, g_ptMS_CoreData->dwms_N * sizeof(T_MS_CkptData));
    
    
    error = saCkptInitialize (&g_ptMS_CoreData->ckptHandle, &MS_OpenCallback, &MS_Version);
    if (error != SA_AIS_OK)
    {
        MS_Printf("Maybe aisexec exit,please check.ms_init failed in saCkptInitialize,err %d,error %d!\n",errno,error);
        goto  init_end;
    }
    
    /* ֻ������Ӧ�÷����ṩ�������� */
    if(0 == dwType)
    {
        error = saLckInitialize (&g_ptMS_CoreData->lckhandle, &ms_lckcallbacks, &MS_Version);
    }
    if (error != SA_AIS_OK)
    {
        MS_Printf("ms_init failed in saLckInitialize,err %d,error %d!\n",errno,error);
        goto  init_end;
    }
    error = saEvtInitialize (&g_ptMS_CoreData->evthandle, &ms_evtcallbacks, &MS_Version);
    if (error != SA_AIS_OK)
    {
        MS_Printf("ms_init failed in saEvtInitialize,err %d,error %d!\n",errno,error);
        goto  init_end;
    }
            
    dwRet = ms_compete(dwType);    
    if(dwRet == 1)
    {
        MS_Printf("ms_init master in ms_compete,err %d!\n",errno);        
    }
    else if(dwRet == 0)
    {
        MS_Printf("ms_init slave in ms_compete,err %d!\n",errno);    
    }
    else
    {
        MS_Printf("ms_init failed in ms_compete,err %d!\n",errno);
        goto  init_end;
    }
    
    if(0 != pthread_create (&g_ptMS_CoreData->dispatch_th, NULL, (void* (*)(void*))evt_dispatch, (byte *)NULL))
    {
        MS_Printf("ms_init failed in pthread_create dispatch,err %d!\n",errno);
        goto  init_end;
    }
    /* ֻ������Ӧ�÷����ṩɨ�蹦��,����Ӧ�÷����ṩ��ѯ��ͨ�湦�� */
    if(0 != pthread_create (&g_ptMS_CoreData->scan_th, NULL, (void* (*)(void*))msscan_thread, (byte *)NULL))
    {
        MS_Printf("ms_init failed in pthread_create scan,err %d!\n",errno);
        goto  init_end;
    }
    return dwRet;
init_end:
    if(g_ptMS_CoreData)
    {
        if(g_ptMS_CoreData->ptSaName)   MS_Free(g_ptMS_CoreData->ptSaName);
        if(g_ptMS_CoreData->ptCkpt)     MS_Free(g_ptMS_CoreData->ptCkpt);
        if(g_ptMS_CoreData->ptlkName)   MS_Free(g_ptMS_CoreData->ptlkName);
        if(g_ptMS_CoreData->ptevtName)  MS_Free(g_ptMS_CoreData->ptevtName);
        MS_Free(g_ptMS_CoreData);
    }
    return dwRet;
}

void OpenCallBack (SaInvocationT invocation, const SaCkptCheckpointHandleT ckptHnd, SaAisErrorT error)
{
	
    MS_Printf("This is a call back for open for invocation = %d\n",(int)invocation);	

    /* if(g_ptMS_CoreData->ckptHnd == ckptHnd) */

}
void SyncCallBack (SaInvocationT invocation, SaAisErrorT error)
{
	
    MS_Printf("This is a call back for sync for invocation = %d\n",(int)invocation);	

    /* if(g_ptMS_CoreData->ckptHnd == ckptHnd) */

}

/**********************************************************************
 * �������ƣ�ms_compete
 * ������������ʼ����������
 * ���ʵı���
 * �޸ĵı���
 * �����������
 * �����������
 * �� �� ֵ��
 * ����˵����
 * �޸�����      �汾��  �޸���      �޸�����
 * ---------------------------------------------------------------------
 * 2011/07/26    V1.0    New          ����
 ************************************************************************/
uint32  ms_compete(uint32 dwType)
{
    uint32               dwFlags     = 0;  /* ���ش�����־ */
    SaNameT              *ptSaName,*ptlkName,*ptevtName;
    T_MS_CkptData        *ptCkpt;
    SaUint32T            VectorIndex = 0;
    uint32               index       = 0;
    SaAisErrorT          error = SA_AIS_OK;
    SaLckLockStatusT     status;  
    
    ptlkName  =  g_ptMS_CoreData->ptlkName;
    ptSaName  =  g_ptMS_CoreData->ptSaName;
    ptevtName =  g_ptMS_CoreData->ptevtName;
    ptCkpt    =  g_ptMS_CoreData->ptCkpt;
    
    /* �ȴ����¼����� */    
    ptevtName->length                = 8;
    *(uint32 *)(ptevtName->value)    = htonl(g_ptMS_CoreData->dwserv);
    *(uint32 *)(ptevtName->value+4)  = htonl(index+0xffffff00);
    snprintf((char *)ptevtName->value, SA_MAX_NAME_LENGTH, "%x%x",*(uint32 *)(ptevtName->value),*(uint32 *)(ptevtName->value+4));
    error = saEvtChannelOpen(g_ptMS_CoreData->evthandle, ptevtName,SA_EVT_CHANNEL_PUBLISHER|SA_EVT_CHANNEL_SUBSCRIBER|SA_EVT_CHANNEL_CREATE, \
                  SA_TIME_MAX, &g_ptMS_CoreData->evtChHnd);
    if(SA_AIS_OK != error)
    {
        MS_Printf("ms_compete failed in saEvtChannelOpen,err %d,%d,return %d!\n",errno,error,dwFlags); 
        goto  compete_end;
    }
    ms_evtSub(index);
    
    for(index = 0;index < g_ptMS_CoreData->dwms_N;index ++,ptSaName ++,ptlkName++,ptCkpt ++)
    {
        ptSaName->length                 = 8;
        *(uint32 *)(ptSaName->value)     = htonl(g_ptMS_CoreData->dwserv);
        *(uint32 *)(ptSaName->value+4)   = htonl(index+0xffff0000);
        snprintf((char *)ptSaName->value, SA_MAX_NAME_LENGTH, "%x%x",*(uint32 *)(ptSaName->value),*(uint32 *)(ptSaName->value+4));
        ptlkName->length                 = 8;
        *(uint32 *)(ptlkName->value)     = htonl(g_ptMS_CoreData->dwserv);
        *(uint32 *)(ptlkName->value+4)   = htonl(index+0xffff0000);
        snprintf((char *)ptlkName->value, SA_MAX_NAME_LENGTH, "%x%x",*(uint32 *)(ptlkName->value),*(uint32 *)(ptlkName->value+4));
        ReadVectorElements[0].dataBuffer = ptCkpt;   
        
        /* ����checkpoint */
        error = saCkptCheckpointOpen (g_ptMS_CoreData->ckptHandle, ptSaName,  &checkpointCreationAttributes, \
                SA_CKPT_CHECKPOINT_CREATE|SA_CKPT_CHECKPOINT_READ|SA_CKPT_CHECKPOINT_WRITE, 0, &ptCkpt->ckptHnd);
        if(SA_AIS_OK != error)
        {
            MS_Printf("ms_compete failed in saCkptCheckpointOpen,err %d,%d,return %d!\n",errno,error,dwFlags); 
            goto  compete_end;
        }
        /* ��������ֻ��Ҫ��ȡ�������ݼ��� */
        if(dwType != 0)
        {
            ptCkpt->lock_id = 0;
            error = saCkptCheckpointRead (ptCkpt->ckptHnd, ReadVectorElements, 1,&VectorIndex);
            if(SA_AIS_OK != error)
            {
                MS_Printf("ms_compete failed 1 in saCkptCheckpointRead,err %d,%d!\n",errno,error);
            }
            continue;
        }
                
        /* ����ȫ���� */
        error = saLckResourceOpen (g_ptMS_CoreData->lckhandle,	ptlkName, \
                SA_LCK_RESOURCE_CREATE, SA_TIME_ONE_SECOND, &ptCkpt->resHnd);	
        if(SA_AIS_OK != error)
        {
            MS_Printf("ms_compete failed in saLckResourceOpen,err %d,%d,return %d!\n",errno,error,dwFlags); 
            goto  compete_end;
        }  

        MS_Printf("ms_compete prepare in saLckResourceLock,Flag %d,err %d,%d!\n",dwFlags,errno,error);
        /* �Ѿ���Ϊ���õ�,ֻ��Ҫ���Ѿ����ڵ������л�ȡ����,����չ���Ǹ��ɷֵ�֮�� */
        if(dwFlags)
        {
            error = saCkptCheckpointRead (ptCkpt->ckptHnd, ReadVectorElements, 1,&VectorIndex);
            if(SA_AIS_OK != error)
            {
                MS_Printf("ms_compete failed 2 in saCkptCheckpointRead,err %d,%d!\n",errno,error);
            }
            continue;
        }
        /* ��ռ����,SA_TIME_END��ʾ�ȴ��ܳ�ʱ�� */
        error = saLckResourceLock (ptCkpt->resHnd,&ptCkpt->lock_id, \
                 SA_LCK_EX_LOCK_MODE,SA_LCK_LOCK_NO_QUEUE,55,SA_TIME_ONE_SECOND,&status);  
/*        error = saLckResourceLockAsync (ptCkpt->resHnd,	index,&ptCkpt->lock_id,
		SA_LCK_EX_LOCK_MODE,0,55);  */
        Vos_Delay(1000);
        MS_Printf("ms_compete after in saLckResourceLock,Flag %d,err %d,%d!\n",dwFlags,errno,error);
        if(SA_AIS_OK  == error)
        {            
            /* ���ɹ���д�뱾�ؽڵ�źͱ��ؽ��̱�ʶ,���뱾������¼����ֹ�쳣�ͷ� */
            ptCkpt->dwNum    = index + 1;
            ptCkpt->nodeid   = g_ptMS_CoreData->nodeid;
            ptCkpt->dwUser   = g_ptMS_CoreData->dwUser;
            g_ptMS_CoreData->lock_id = ptCkpt->lock_id;
            error = saCkptCheckpointWrite (ptCkpt->ckptHnd, ReadVectorElements, 1, &VectorIndex);
            if(SA_AIS_OK != error)
            {
                MS_Printf("ms_compete failed in saCkptCheckpointWrite,err %d,%d!\n",errno,error);
            }
            else
            {
                MS_Printf("ms_compete succeed in saCkptCheckpointWrite,err %d,%d!\n",errno,error);
            }
            dwFlags          = 1;            
        }
        else
        {
            ptCkpt->lock_id = 0;
            error = saCkptCheckpointRead (ptCkpt->ckptHnd, ReadVectorElements, 1,&VectorIndex);
            if(SA_AIS_OK != error)
            {
                MS_Printf("ms_compete failed 3 in saCkptCheckpointRead,err %d,%d!\n",errno,error);
            }
            {
                MS_Printf("ms_compete succeed in saCkptCheckpointRead,err %d,%d,return %d!\n",errno,error,dwFlags);
            }
        }        
    } 

    
compete_end:
    if(dwFlags>1)
    {
        ptCkpt    =  g_ptMS_CoreData->ptCkpt;
        for(index = 0;index < g_ptMS_CoreData->dwms_N;index ++)    
        {
            if(ptCkpt->ckptHnd)      
            {
                saCkptCheckpointClose(ptCkpt->ckptHnd);
                ptCkpt->ckptHnd = 0;
            }
            if(ptCkpt->resHnd)      
            {
                saLckResourceClose(ptCkpt->resHnd);
                ptCkpt->resHnd  = 0;
            }
        }
        saEvtFinalize(g_ptMS_CoreData->evtChHnd);
        MS_Printf("ms_compete failed at compete_end,err %d,%d,return %d!\n",errno,error,dwFlags);
    }
    return  dwFlags;
}
/**********************************************************************
 * �������ƣ�ms_scan
 * ��������������ɨ��
 * ���ʵı���
 * �޸ĵı���
 * ���������dwMS  ,1 ���ã�0 ����
 * �����������
 * �� �� ֵ��
 * ����˵����
 * �޸�����      �汾��  �޸���      �޸�����
 * ---------------------------------------------------------------------
 * 2011/07/26    V1.0    New          ����
 ************************************************************************/
uint32  ms_scan(uint32 dwMS)
{
    uint32               dwFlags     = dwMS;  /* ���ش�����־ */
    SaNameT              *ptSaName,*ptlkName,*ptevtName;
    T_MS_CkptData        *ptCkpt;
    SaUint32T            VectorIndex = 0;
    uint32               index = 0;
    SaAisErrorT          error;
    SaLckLockStatusT     status; 
    
    ptlkName  =  g_ptMS_CoreData->ptlkName;
    ptSaName  =  g_ptMS_CoreData->ptSaName;
    ptevtName =  g_ptMS_CoreData->ptevtName;
    ptCkpt    =  g_ptMS_CoreData->ptCkpt;
    
    ptevtName->length                = 8;
    *(uint32 *)(ptevtName->value)    = htonl(g_ptMS_CoreData->dwserv);
    *(uint32 *)(ptevtName->value+4)  = htonl(index+0xffffff00);
    snprintf((char *)ptevtName->value, SA_MAX_NAME_LENGTH, "%x%x",*(uint32 *)(ptevtName->value),*(uint32 *)(ptevtName->value+4));
    for(index = 0;index < g_ptMS_CoreData->dwms_N;index++,ptSaName++,ptlkName++,ptCkpt++)
    {
        ptSaName->length                 = 8;
        *(uint32 *)(ptSaName->value)     = htonl(g_ptMS_CoreData->dwserv);
        *(uint32 *)(ptSaName->value+4)   = htonl(index+0xffff0000);
        snprintf((char *)ptSaName->value, SA_MAX_NAME_LENGTH, "%x%x",*(uint32 *)(ptSaName->value),*(uint32 *)(ptSaName->value+4));
        ptlkName->length                 = 8;
        *(uint32 *)(ptlkName->value)     = htonl(g_ptMS_CoreData->dwserv);
        *(uint32 *)(ptlkName->value+4)   = htonl(index + 0xffff0000);
        snprintf((char *)ptlkName->value, SA_MAX_NAME_LENGTH, "%x%x",*(uint32 *)(ptlkName->value),*(uint32 *)(ptlkName->value+4));

        ReadVectorElements[0].dataBuffer = ptCkpt; 
        error = saCkptCheckpointRead (ptCkpt->ckptHnd, ReadVectorElements, 1,&VectorIndex);
        if(SA_AIS_OK != error)
        {
            MS_Printf("ms_scan failed in saCkptCheckpointRead,err %d,%d!\n",errno,error);
        }
              
        if(g_ptMS_CoreData->dwType != 0)
        {            
            continue;
        }
        
        /* ��Ա��س�������û�г��зֱ��ǣ� */
        if(0 == g_ptMS_CoreData->lock_id)
        {
            error = saLckResourceLock (ptCkpt->resHnd,&ptCkpt->lock_id, \
                 SA_LCK_EX_LOCK_MODE,SA_LCK_LOCK_NO_QUEUE,55,SA_TIME_ONE_SECOND,&status);  

            if(SA_AIS_OK  == error)
            {            
                /* ���ɹ���д�뱾�ؽڵ�� */                  
                ptCkpt->nodeid   = g_ptMS_CoreData->nodeid;
                ptCkpt->dwUser   = g_ptMS_CoreData->dwUser;
                g_ptMS_CoreData->lock_id = ptCkpt->lock_id;
                error = saCkptCheckpointWrite (ptCkpt->ckptHnd, ReadVectorElements, 1, &VectorIndex);
                if(SA_AIS_OK != error)
                {
                    MS_Printf("ms_scan failed in saCkptCheckpointWrite,err %d,%d!\n",errno,error);
                }
                else
                {
                    MS_Printf("ms_scan succeed in saCkptCheckpointWrite,err %d,%d!\n",errno,error);
                }
                dwFlags          = 1;     
                ms_evt(index,(byte *)&dwFlags,sizeof(dwFlags));   
                continue;       
            }
            else
            {
                MS_Printf("ms_scan failed in saLckResourceLock,err %d,%d!\n",errno,error);
                ptCkpt->lock_id = 0;  /* ���봦��AIS��BUG */
            }
        }
        else
        {
 
            /* ������ʵ����Ҫ���κι���������У��һ������ */
            if((ptCkpt->nodeid   != g_ptMS_CoreData->nodeid)  || \
               (ptCkpt->dwUser   != g_ptMS_CoreData->dwUser)  || \
               (ptCkpt->lock_id  != g_ptMS_CoreData->lock_id))
            {
                /* ˵��������һ�����ѵ�ѡ������ѡ������ͷű��������¾����ķ�ʽ */
                error = saLckResourceUnlock(g_ptMS_CoreData->lock_id,SA_TIME_ONE_SECOND);
                if(SA_AIS_OK != error)
                {
                    MS_Printf("ms_scan failed in saLckResourceUnlock,id %ld,err %d,%d!\n",ptCkpt->lock_id,errno,error);
                }
                ptCkpt->lock_id          = 0;
                ptCkpt->nodeid           = 0;
                g_ptMS_CoreData->lock_id = 0;
                dwFlags          = 0;   /* ����������--liuxf */
                ms_evt(index,(byte *)&dwFlags,sizeof(dwFlags));
                Vos_Delay(5000);
                continue;
/*                ptCkpt->nodeid   = g_ptMS_CoreData->nodeid;
                ptCkpt->dwUser   = g_ptMS_CoreData->dwUser;
                ptCkpt->lock_id  = g_ptMS_CoreData->lock_id;
                error = saCkptCheckpointWrite (ptCkpt->ckptHnd, ReadVectorElements, 1, &VectorIndex);
                MS_Printf("ms_scan in saCkptCheckpointWrite,err %d,%d,return %d!\n",errno,error,dwFlags);
*/
            }
        } 
    } 

    return dwFlags;
}
/**********************************************************************
 * �������ƣ�ms_change
 * ���������������л�
 * ���ʵı���
 * �޸ĵı���
 * ���������dwMaster  ,1 ���ã�0 ����
 * �����������
 * �� �� ֵ��
 * ����˵����
 * �޸�����      �汾��  �޸���      �޸�����
 * ---------------------------------------------------------------------
 * 2011/07/26    V1.0    New          ����
 ************************************************************************/
uint32 ms_change(uint32 dwMaster)
{
    T_MS_CkptData        *ptCkpt;
    uint32               index;
    SaAisErrorT          error=(SaAisErrorT)0;
    uint32               dwFlags = 0;

    if(NULL == g_ptMS_CoreData)
    {
        MS_Printf("ms_change:Can't ms_change, maybe not in ms mode!\n");
        return 0xffffffff;
    }
    ptCkpt    =  g_ptMS_CoreData->ptCkpt;
    if(dwMaster == 0)
    {
        return 0;
    }
    
    for(index = 0;index < g_ptMS_CoreData->dwms_N;index++,ptCkpt++)
    {        
        {
            if((g_ptMS_CoreData->lock_id == ptCkpt->lock_id) && \
               (g_ptMS_CoreData->lock_id != 0))
            {
                error = saLckResourceUnlock(ptCkpt->lock_id,SA_TIME_ONE_SECOND);
                if(SA_AIS_OK != error)
                {
                    MS_Printf("ms_change failed in saLckResourceUnlock,index %d,err %d,%d!\n",index,errno,error);
                }
                else
                {
                    MS_Printf("ms_change succeed in saLckResourceUnlock,index %d,err %d,%d!\n",index,errno,error);
                }
                ptCkpt->lock_id          = 0;
                g_ptMS_CoreData->lock_id = 0;
                ms_evt(index,(byte *)&dwFlags,sizeof(dwFlags));
                return 0;
            }
        }
    }
    MS_Printf("ms_change impossible,index %d,err %d,%d!\n",index,errno,error);
    return 1;
}
/**********************************************************************
 * �������ƣ�ms_qry
 * ����������������ѯ
 * ���ʵı���
 * �޸ĵı���
 * �����������
 * �����������
 * �� �� ֵ��1��Master, 0:StandBy, Other:ERROR
 * ����˵����
 * �޸�����      �汾��  �޸���      �޸�����
 * ---------------------------------------------------------------------
 * 2011/07/26    V1.0    New          ����
 ************************************************************************/
uint32 ms_qry(uint32 bIsPrint)
{
    T_MS_CkptData        *ptCkpt;
    uint32               index = 0;
    SaAisErrorT          error = (SaAisErrorT)0;

    if(NULL == g_ptMS_CoreData)
    {
        MS_Printf("ms_qry failed in initialize,index %d,err %d,%d!\n",index,errno,error);
        return 0xffffffff;
    }
    ptCkpt    =  g_ptMS_CoreData->ptCkpt;
    
    
    for(index = 0;index < g_ptMS_CoreData->dwms_N;index ++,ptCkpt ++)
    {
        if(ptCkpt)
        {
            if(TRUE == bIsPrint)
            {
                 MS_Printf("ms_qry data,nodeid 0x%x,0x%x;dwUser %d,%d!\n",ptCkpt->nodeid,g_ptMS_CoreData->nodeid,ptCkpt->dwUser,g_ptMS_CoreData->dwUser); 
            }
            /* ��ǰ����ʵ���������̣�����ȫ������ */   
            if((ptCkpt->nodeid == g_ptMS_CoreData->nodeid) && \
               (ptCkpt->dwUser == g_ptMS_CoreData->dwUser) && \
               (g_ptMS_CoreData->lock_id != 0))
            {
                return 1;
            }
            /* ��ǰ����Ĵӽ��̣���ȫ����������������ƥ�� */
            if((ptCkpt->nodeid == g_ptMS_CoreData->nodeid) && \
               (ptCkpt->dwUser == g_ptMS_CoreData->dwUser) && \
               (g_ptMS_CoreData->dwType != 0))
            {
                return 1;
            }
        }
    }
    return 0;
}
/**********************************************************************
 * �������ƣ�ms_changeflagsSet
 * ���������������л���ʶ
 * ���ʵı���
 * �޸ĵı���
 * ���������dwMaster  ,1 ���ã�0 ����
 * �����������
 * �� �� ֵ��
 * ����˵����
 * �޸�����      �汾��  �޸���      �޸�����
 * ---------------------------------------------------------------------
 * 2011/07/26    V1.0    New          ����
 ************************************************************************/
uint32 ms_changeflagsSet(uint32 dwMaster)
{
    if((dwMaster == 0)|| (g_ptMS_CoreData->dwType != 0))
    {
        MS_Printf("ms_changeflagsSet can't change master %d type %d!\n",dwMaster,g_ptMS_CoreData->dwType);
        return 0;
    }
    g_ptMS_CoreData->dwflags = 1;
    return 1;
}
/**********************************************************************
 * �������ƣ�ms_changeflagsGet
 * ���������������л���ʶ
 * ���ʵı���
 * �޸ĵı���
 * ���������dwMaster  ,1 ���ã�0 ����
 * �����������
 * �� �� ֵ��
 * ����˵����
 * �޸�����      �汾��  �޸���      �޸�����
 * ---------------------------------------------------------------------
 * 2011/07/26    V1.0    New          ����
 ************************************************************************/
uint32 ms_changeflagsGet()
{
    return g_ptMS_CoreData->dwflags;
}
/**********************************************************************
 * �������ƣ�ms_evt
 * �����������¼�����
 * ���ʵı���
 * �޸ĵı���
 * ���������byte *p, uint32 len
 * �����������
 * �� �� ֵ��
 * ����˵����
 * �޸�����      �汾��  �޸���      �޸�����
 * ---------------------------------------------------------------------
 * 2011/07/26    V1.0    New          ����
 ************************************************************************/
uint32  ms_evt(uint32 id, byte *p, uint32 len)
{
    SaAisErrorT         error;
    SaEvtEventHandleT   event_handle;  
    SaEvtEventIdT       event_id;
    SaNameT             *ptevtName;
    T_MS_Event          tEvt;

    
    ptevtName = g_ptMS_CoreData->ptevtName;
    
    error     = saEvtEventAllocate(g_ptMS_CoreData->evtChHnd, &event_handle);
    if(SA_AIS_OK != error)
    {
        MS_Printf("ms_evt Allocate failed,nodeid 0x%x;error %d!\n",g_ptMS_CoreData->nodeid,error); 
        return error;   
    }
    
    error = saEvtEventAttributesSet(event_handle,&evt_pat_array,1,g_evtTimeOut, ptevtName);
    if(SA_AIS_OK != error)
    {
        MS_Printf("ms_evt AttributesSet failed,nodeid 0x%x;error %d!\n",g_ptMS_CoreData->nodeid,error); 
        saEvtEventFree(event_handle);
        return error;   
    }
    
    event_id    = 0;
    tEvt.dwlen  = sizeof(T_MS_Event);
    tEvt.dwserv = g_ptMS_CoreData->dwserv;
    tEvt.nodeid = g_ptMS_CoreData->nodeid;
    tEvt.dwUser = g_ptMS_CoreData->dwUser;
    tEvt.id     = id;
    tEvt.dwType = 0;
    if(len > MS_EVT_LEN*sizeof(uint32))
    {
        len = MS_EVT_LEN*sizeof(uint32);
    }
    memcpy((byte *)&tEvt.dwData[0],p,len);
    error = saEvtEventPublish(event_handle, &tEvt,  sizeof(tEvt), &event_id);
    if(SA_AIS_OK != error)
    {
        MS_Printf("ms_evt saEvtEventPublish failed,nodeid 0x%x;error %d!\n",g_ptMS_CoreData->nodeid,error); 
        saEvtEventFree(event_handle);
        return error;   
    }
    MS_Printf("ms_evt send success ,nodeid 0x%x;error %d!\n",g_ptMS_CoreData->nodeid,error); 

    return 0;
}
/**********************************************************************
 * �������ƣ�ms_evtSub
 * �����������¼�����
 * ���ʵı���
 * �޸ĵı���
 * �����������
 * �����������
 * �� �� ֵ��
 * ����˵����
 * �޸�����      �汾��  �޸���      �޸�����
 * ---------------------------------------------------------------------
 * 2011/07/26    V1.0    New          ����
 ************************************************************************/
uint32  ms_evtSub(uint32 id)
{
    SaAisErrorT          error;
/*    SaEvtEventHandleT    event_handle;  
    SaEvtEventIdT        event_id; */
    SaEvtSubscriptionIdT subscription_id;

    subscription_id  = id;
    error = saEvtEventSubscribe(g_ptMS_CoreData->evtChHnd,	&subscribe_filters,subscription_id);
    if(SA_AIS_OK != error)
    {
        MS_Printf("ms_evtSub data,nodeid 0x%x;error %d!\n",g_ptMS_CoreData->nodeid,error); 
        return error;   
    }
    return 0;
}

void    ms_LckResourceOpenCallback (SaInvocationT invo, SaLckResourceHandleT lockResourceHandle,SaAisErrorT error)
{
    MS_Printf ("testLckResourceOpenCallback invocation %llu error %d\n", \
               (unsigned long long)invo, error);
    lockResourceHandle = lockResourceHandle;
}

void    ms_LckLockGrantCallback (SaInvocationT invo,SaLckLockStatusT lockStatus,SaAisErrorT error)
{
    T_MS_CkptData        *ptCkpt;
    MS_Printf ("testLckLockGrantCallback invocation %llu status %d error %d\n", \
	        (unsigned long long)invo, lockStatus, error);
    if(SA_LCK_LOCK_NOT_QUEUED !=lockStatus)
    {
        ptCkpt         = g_ptMS_CoreData->ptCkpt + (uint32)invo;
        ptCkpt->dwPad  = lockStatus;
    }
}

void ms_LckLockWaiterCallback (SaLckWaiterSignalT waiterSignal,SaLckLockIdT lockId,SaLckLockModeT modeHeld,SaLckLockModeT modeRequested)
{
    int result =0;
    printf ("waiter callback mode held %d mode requested %d lock id %llu\n", \
		       modeHeld, modeRequested, (unsigned long long)lockId);
    printf ("pr lock id %llu\n", (unsigned long long)lockId);
    printf ("ms_LckLockWaiterCallback:saLckResourceUnlock result %d (should be 1)\n", result);
}

void ms_LckResourceUnlockCallback (SaInvocationT invocation, SaAisErrorT error)
{
	printf ("testLckResourceUnlockCallback async invocation %llu error %d\n", \
		(unsigned long long)invocation, error);
}

void    ms_evtopencallback(SaInvocationT invo,SaEvtChannelHandleT channelHandle,	SaAisErrorT error)
{
    return;
}

void    ms_evtcallback(SaEvtSubscriptionIdT subId,	const SaEvtEventHandleT evtHandle,const SaSizeT DataSize)
{
    T_MS_Event          tEvt;
    SaSizeT             data_size= sizeof(tEvt);
    SaAisErrorT         error;
    T_MS_CallArg        CallArg;
    
    error = saEvtEventDataGet(evtHandle, (void *)&tEvt, (SaSizeT *)&data_size);
    if(SA_AIS_OK != error)
    {
        MS_Printf("ms_evtcallback data,id %d;error %d!\n",subId,error); 
        return;   
    }
    CallArg.nodeid = tEvt.nodeid;
    CallArg.dwserv = tEvt.dwserv;
    CallArg.dwUser = tEvt.dwUser;
    CallArg.dwEvt  = tEvt.dwData[0];
    MS_Printf("ms_evtcallback data,serv %d;nodeid 0x%x;Data %d!\n",tEvt.dwserv,tEvt.nodeid,tEvt.dwData[0]); 
    if(g_ptMS_CoreData->usercall)
    {
        g_ptMS_CoreData->usercall(&CallArg);
    }
    saEvtEventFree(evtHandle); 
    return;
}
/**********************************************************************
 * �������ƣ�evt_dispatch
 * �����������¼����Ĵ����߳�,����ȫ���Է������¼�
 * ���ʵı���
 * �޸ĵı���
 * �����������
 * �����������
 * �� �� ֵ��NULL
 * ����˵����
 * �޸�����      �汾��  �޸���      �޸�����
 * ---------------------------------------------------------------------
 * 2011/07/26    V1.0    New          ����
 ************************************************************************/
void *evt_dispatch(byte *arg)
{
    struct pollfd       pfd;
    int                 nfd;
    SaSelectionObjectT  fd;
    SaAisErrorT         error;
    
    while(1)
    {
        do 
        {
            error = saEvtSelectionObjectGet(g_ptMS_CoreData->evthandle, &fd);
            if(error == SA_AIS_ERR_TRY_AGAIN)
            {
                Vos_Delay(5000);
            }
            else
            {
                break;
            }
        } while (1);
        if (error != SA_AIS_OK) 
        {
            MS_Printf("ERROR: saEvtSelectionObject error %d\n", error);
            continue;
        }
        pfd.fd = fd;
        pfd.events = POLLIN;
        nfd = poll(&pfd, 1, g_dwTimeOut);
        if (nfd <= 0) {
            /* MS_Printf("ERROR: poll fds %d\n", nfd); */
            if (nfd < 0) {
                perror("ERROR: poll error");
            }
            continue;
	     }
        
        do {
            error = saEvtDispatch(g_ptMS_CoreData->evthandle, SA_DISPATCH_ONE);
            if(error == SA_AIS_ERR_TRY_AGAIN)
            {
                Vos_Delay(5000);
            }
            else
            {
                break;
            }
        } while (1);
        if (error != SA_AIS_OK) {
            MS_Printf("ERROR: saEvtDispatch error %d\n", error);
            Vos_Delay(5000);
            continue;
        }
    }
    return NULL;
}
/**********************************************************************
 * �������ƣ�msscan_thread
 * �������������ɨ���߳�
 * ���ʵı���
 * �޸ĵı���
 * �����������
 * �����������
 * �� �� ֵ��
 * ����˵������ȫ�����ݵĲ��������ڸ��߳��д���
 * �޸�����      �汾��  �޸���      �޸�����
 * ---------------------------------------------------------------------
 * 2011/07/26    V1.0    New          ����
 ************************************************************************/
void *msscan_thread(byte *arg)
{
    do
    {
        /* Ŀǰ�Ȳ���ɨ�跽ʽ�����������ʵʱҪ�󣬿������ź�����ʽ���� */
        if(g_ptMS_CoreData->dwflags)
        {
            ms_change(ms_qry(TRUE));
            g_ptMS_CoreData->dwflags = 0;
            Vos_Delay(5000);
        }
        ms_scan(ms_qry(FALSE));
        Vos_Delay(1000);
    }while(1);
    return NULL;
}
/**********************************************************************
 * �������ƣ�Vos_Delay
 * ������������ʱ����
 * ���ʵı���
 * �޸ĵı���
 * �����������
 * �����������
 * �� �� ֵ��
 * ����˵����
 * �޸�����      �汾��  �޸���      �޸�����
 * ---------------------------------------------------------------------
 * 2011/07/26    V1.0    New          ����
 ************************************************************************/
void Vos_Delay(uint32 dwTimeout)
{
    struct timespec delay;
    struct timespec rem;
    int             n   = -1;
    delay.tv_sec    = dwTimeout/1000;
    delay.tv_nsec   = (dwTimeout - delay.tv_sec*1000)*1000*1000;
    if (dwTimeout == 0)
    {
        delay.tv_nsec  = 1;
    }
    do
    {
        n = nanosleep(&delay,&rem);
        delay=rem;
    }
    while (n < 0 && errno == EINTR);    

}
/**********************************************************************
 * �������ƣ�ms_registercall
 * ��������������֪ͨע��ص��ӿ�
 * ���ʵı���
 * �޸ĵı���
 * ���������MS_NoticeCallback usercall
 * �����������
 * �� �� ֵ��0���ɹ��� ������ʧ��
 * ����˵����
 * �޸�����      �汾��  �޸���      �޸�����
 * ---------------------------------------------------------------------
 * 2011/07/26    V1.0    New          ����
 ************************************************************************/
uint32  ms_registercall(MS_NoticeCallback usercall)
{
    if(g_ptMS_CoreData)
    {
        g_ptMS_CoreData->usercall = usercall;
        return 0;
    }
    return -1;
}

/**********************************************************************
 * �������ƣ�MS_DbgMsQry()
 * ����������������ѯ����
 * ���ʵı���
 * �޸ĵı���
 * ���������byte *p, uint32 len
 * �����������
 * �� �� ֵ��
 * ����˵����
 * �޸�����      �汾��  �޸���      �޸�����
 * ---------------------------------------------------------------------
 * 2012/02/13    V1.0    ��ѩ��      ����
 ************************************************************************/
 void  MS_DbgMsQry(void)
{
    uint32 dwQryRet;

    dwQryRet = ms_qry(TRUE);
    if(1 == dwQryRet)
    {
        MS_Printf("MS_DbgMsQry:Current is Master!\n");
    }
    else if(0xffffffff == dwQryRet)
    {
        MS_Printf("MS_DbgMsQry:MS is not inited ,maybe not in msconfig mode!\n");
    }
    else
    {
        MS_Printf("MS_DbgMsQry:Current is Slave!\n");
    }
    return;
}

void MS_DbgMsChange(uint32 dwMaster)
{
    if(0 == dwMaster)
    {
         MS_Printf("Please input no zero to push ms change!\n");
         return;
    }
    ms_change(dwMaster);
    return;
}
DEFINE_DEBUG_FUNC(MS_DbgMsQry);
DEFINE_DEBUG_FUNC(MS_DbgMsChange);



