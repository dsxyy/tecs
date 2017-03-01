/*********************************************************************
* ��Ȩ���� (C)2012, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ� igmpv3_report.c
* �ļ���ʶ�� 
* ����˵���� 
* ��ǰ�汾�� V1.0
* ��    �ߣ� liuxuefeng
* ������ڣ� 
*
* �޸ļ�¼1��
*    �޸����ڣ�2012��03��28��
*    �� �� �ţ�V1.0
*    �� �� �ˣ�liuxuefeng
*    �޸����ݣ�����
**********************************************************************/
/**************************************************************************
*                            ͷ�ļ�                                        *
**************************************************************************/
#include "igmpv3_report.h"

#include <assert.h>
#include <semaphore.h>
#include <sys/ioctl.h> 

#include <netinet/in.h>
#include <net/if_arp.h>
#include <arpa/inet.h>

#include <linux/if_ether.h>
#include <linux/if_packet.h>

/**************************************************************************
*                            ����                                        *
**************************************************************************/
#define MAX_ISS_MG_REG_RED                   ((BYTE)20)           /*ISS �����˳��鲥������¼����*/

#define RCV_AIS_MSG_ADD_UDP_MULTI            ((BYTE)0x02)         /*����udp�鲥��*/
#define RCV_AIS_MSG_EXIT_UDP_MULTI           ((BYTE)0x03)         /*�˳�udp�鲥��*/
#define  GPS_SYNC_LOCAL_INTERVAL             ((BYTE)16)           /*ÿ��16��ͬ��һ�α���ʱ���*/

#define ISS_IGMP_REPORT_INTERVAL             ((BYTE)10)           /*����IGMPREPORT���*/
#define ISS_SDER_TYPE_JOB          ((BYTE)1) /**< JOB */
#define ISS_SDER_TYPE_THREAD       ((BYTE)2) /**< THREAD */

#define AIS_CMD_TYPE_ADD_UDP_MULTI  ((BYTE)0x01) /**< �����鲥�� */
#define AIS_CMD_TYPE_EXIT_UDP_MULTI ((BYTE)0x02) /**< �˳��鲥�� */

/**************************************************************************
*                            ��                                          *
**************************************************************************/

/**************************************************************************
*                          ��������                                       *
**************************************************************************/


/**************************************************************************
*                           ȫ�ֺ���                                  *
**************************************************************************/
extern   WORD32  IssGroupNoToMultiCastIP(WORD16 wGroupNo);
extern VOID AddIssMgRegRecord(JID *ptJid,WORD16 wGroupNo,BYTE ucRegType,WORD32 dwStatus);
extern BOOLEAN  InitIgmpv3(VOID);
/**************************************************************************
*                           ȫ�ֱ���                                      *
**************************************************************************/
BOOLEAN              g_bIssIgmpPrnEn = TRUE;                          /*ISS��Ϣ��ӡ����*/
T_ISS_MG_CTRL  *g_ptIssMgCtrl        = NULL;     /*ISS�鲥����ƽṹ*/
INT            g_iIssSndSocket       = -1;       /*ISS�鲥����socket*/
extern char g_ucIfname[IFNAMSIZ];

/**************************************************************************
*                           ���ر���                                      *
**************************************************************************/
static  BYTE                 s_ucIssMgRegRedPos = 0;                   /*��¼�鲥������˳���¼��������*/
static  T_IssMgRegRed  s_atIssMgRegRed[MAX_ISS_MG_REG_RED];      /*ISS�����˳��鲥���¼����*/
/**************************************************************************
*                           �ֲ�����ԭ��                                  *
**************************************************************************/


/**************************************************************************
*                           ����ʵ��                                  *
**************************************************************************/

/* VOS�ķ�װ����ʱ�ȷ����--liuxf */
/* �ź������� */
#define COUNT_SEM_STYLE             (WORD32)0x01        /**< �����ź��� */
#define SYNC_SEM_STYLE              (WORD32)0x10        /**< ͬ���ź��� */
#define MUTEX_STYLE                 (WORD32)0x20        /**< ���⣬VxWorks��Ӧ���ǻ����ź�����NT�¶�Ӧ���ǻ������ */
#define RWLOCK_STYLE                 (WORD32)0x40       /**< ��д������ */
    
#define RWLOCK_READFITST_MODE              0x01               /**< ��д��������ģʽ */
#define RWLOCK_WRITEFITST_MODE             0x10               /**< ��д��д����ģʽ */
/* �ֲ��� */
#define  DEF_OBJ_COUNT             1000
/* 2.2 �ź����������ú� */
#define VOS_SEM_CTL_USED               (BYTE)0x01  /* ���п��ƿ鱻ʹ�� */
#define VOS_SEM_CTL_NO_USED         (BYTE)0x10  /* ���п��ƿ�δ��ʹ�� */

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
    WORD32            dwTaskId;         /* ӵ�д˽������ź������߳�tid */
    WORD32            dwPtick;          /* �������ź���P�����ɹ�������tickֵ */
    WORD32            dwMode;
	WORD32            dwCreate;
    INT               iErrno;
    BYTE              ucUsed;
    BYTE              ucRwFlag; 
    BYTE              ucPad[2];
}T_Obj_MutexType;
static      pthread_mutex_t         s_tMutexLock;
T_Obj_MutexType       *g_atMutex;  /* todo zhugw: ��Ҫmalloc�����С*/
WORD32      g_dwMaxSem=DEF_OBJ_COUNT;
WORD32      g_dwSemCount=0;

/**********************************************************************
* �������ƣ�Vos_CreateSemaphore
* ���������������ź���
* ���ʵı���
* �޸ĵı���
* ���������
  WORD32 dwInitVal���ź����ĳ�ʼֵ
  WORD32 dwMaxVal��NT�µĲ�������ʾ�ź����������Դﵽ�����ֵ
  WORD32 dwFlag�������������͵��ź���
                 COUNT_SEM_STYLE�����������ź���
                 SYNC_SEM_STYLE������ͬ���ź���
                 MUTEX_STYLE�������������VxWorks��Ӧ���ǻ����ź�����
                              NT�¶�Ӧ���ǻ������
  WORD32 *pdwSemID�����ڲ������ź���ID
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2002/10/14    V1.0    ��Ԫ��      ����
* 2002/11/30    V1.0    ��Ԫ��      �������
************************************************************************/
BOOLEAN Vos_CreateSemaphore(
    WORD32 dwInitVal,
    WORD32 dwMaxVal,
    WORD32 dwFlag,
    WORD32 *pdwSemID
)
{
    WORD32           dwLoop   = 0;
    WORD32           dwSemId  = 0;
    T_Obj_MutexType  *ptMutex = NULL;

    if ((COUNT_SEM_STYLE != dwFlag)
               && (SYNC_SEM_STYLE != dwFlag)
               && (MUTEX_STYLE != dwFlag)
               )
    {
        assert((COUNT_SEM_STYLE == dwFlag)
               || (SYNC_SEM_STYLE == dwFlag)
               || (MUTEX_STYLE == dwFlag)
               );
        IGMP_Print("Vos_CreateSemaphore: flag error!\n");
        return FALSE;
    }
    if (NULL == pdwSemID)
    {
        assert (NULL != pdwSemID);
        return FALSE;
    }
    *pdwSemID=0;
    /* ����g_atMutexʱ���ӻ��Ᵽ�� */
    if (pthread_mutex_lock(&s_tMutexLock)!=0)
    {

        IGMP_Print("Vos_CreateSemaphore:mutex_lock failed!\n");
        return FALSE;
    }
    /* ���ҿ����ź������ƿ� */
    for (dwLoop = 0; dwLoop < g_dwMaxSem; dwLoop ++)
    {
        if (g_atMutex[dwLoop].ucUsed == VOS_SEM_CTL_NO_USED)
        {
            /* �ҵ����õ��ź������ƿ� */
            dwSemId  =  dwLoop;
            // To do : ��Ҫ���������С
            ptMutex  =  &g_atMutex[dwLoop];
            break;
        }
    }

    if (g_dwMaxSem == dwLoop)
    {
        /* ˵��û���ҵ����õ��ź������ƿ� */
        pthread_mutex_unlock(&s_tMutexLock);
        IGMP_Print("Vos_CreateSemaphore: No more mutex  control block is available!\n");
        return FALSE;
    }

    /*make sure the procedure self mutex */


    switch (dwFlag)
    {
    case COUNT_SEM_STYLE:
        if (0 == sem_init(&(ptMutex->obj.sem),0,dwInitVal))
        {
            *pdwSemID = dwLoop+1;
        }
        else
        {
            pthread_mutex_unlock(&s_tMutexLock);
            return FALSE;
        }
        break;
    case SYNC_SEM_STYLE:
        if (0 == sem_init(&(ptMutex->obj.sem),0,0))
        {
            *pdwSemID = dwLoop+1;
        }
        else
        {
            pthread_mutex_unlock(&s_tMutexLock);
            return FALSE;
        }
        break;
    case MUTEX_STYLE:
    {
        /* lizl, �û����������� */
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);
        pthread_mutexattr_setrobust_np(&attr, PTHREAD_MUTEX_ROBUST_NP);
        /*���ж����ȼ���ת֧�ֻ������⣬��ʱȥ��*/
        /*pthread_mutexattr_setprotocol(&attr,PTHREAD_PRIO_INHERIT);*/

        if (0 == pthread_mutex_init(&(ptMutex->obj.mutex),&attr))
        {
            pthread_mutexattr_destroy(&attr);
            *pdwSemID   = dwLoop+1;
        }
        else
        {
            pthread_mutexattr_destroy(&attr);
            pthread_mutex_unlock(&s_tMutexLock);
            return FALSE;
        }
    }
    break;
    default:
        *pdwSemID = 0;
        pthread_mutex_unlock(&s_tMutexLock);
        IGMP_Print("Vos_CreateSemaphore: Flag of semaphore is invalid!\n");
        return FALSE;
        break;
    }

    ptMutex->dwFlag     = dwFlag;
    ptMutex->ucUsed  = VOS_SEM_CTL_USED;
    g_dwSemCount++;
    pthread_mutex_unlock(&s_tMutexLock);

    return TRUE; /* ֻ��Ϊ���ڱ����ʱ�򲻲����澯 */
}

/**********************************************************************
* �������ƣ�Vos_DestroySemaphore
* ����������ɾ���ź���
* ���ʵı���
* �޸ĵı���
* ���������
  WORD32 dwSemID
* ���������
* �� �� ֵ��TRUE��ʾɾ���ź����ɹ���FALSE��ʾɾ���ź���ʧ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2002/10/14    V1.0    ��Ԫ��      ����
************************************************************************/
BOOLEAN Vos_DestroySemaphore(WORD32 dwSemID)
{
    if ((dwSemID > g_dwMaxSem)||(dwSemID ==0))
    {
        assert(FALSE);
        return FALSE;
    }
    dwSemID--;
    T_Obj_MutexType *ptSemCtl = &g_atMutex[dwSemID];
    if (pthread_mutex_lock(&s_tMutexLock)!=0)
    {

        IGMP_Print("Vos_DestroySemaphore:mutex_lock failed!\n");
        return FALSE;
    }
    if (VOS_SEM_CTL_USED != ptSemCtl->ucUsed)
    {
        pthread_mutex_unlock(&s_tMutexLock);
        return FALSE;
    }

    switch (ptSemCtl->dwFlag)
    {
    case COUNT_SEM_STYLE:
    {
        if (0 != sem_destroy(&(ptSemCtl->obj.sem)))
        {
            pthread_mutex_unlock(&s_tMutexLock);
            return FALSE;
        }
        break;
    }
    case SYNC_SEM_STYLE:
    {
        if (0 != sem_destroy(&(ptSemCtl->obj.sem)))
        {
            pthread_mutex_unlock(&s_tMutexLock);
            return FALSE;
        }
        break;
    }
    case MUTEX_STYLE:
    {
        pthread_mutex_destroy(&(ptSemCtl->obj.mutex));
        break;
    }
    default:
    {
        IGMP_Print("XOS_CloseSemaphore: Flag of semaphore is invalid!\n");
        pthread_mutex_unlock(&s_tMutexLock);
        return FALSE;
        break;
    }
    }
    ptSemCtl->ucUsed = VOS_SEM_CTL_NO_USED;
    g_dwSemCount--;
    pthread_mutex_unlock(&s_tMutexLock);
    return TRUE;
}

/**********************************************************************
* �������ƣ�Vos_SemaphoreP
* ����������P����
* ���ʵı���
* �޸ĵı���
* ���������
  WORD32 dwSemID���ź���ID
  WORD32 dwTimeout����ʱֵ����λΪ����
* ���������
* �� �� ֵ��TRUE��ʾP�����ɹ���FALSE��ʾP����ʧ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2002/10/14    V1.0    ��Ԫ��      ����
************************************************************************/
BOOLEAN Vos_SemaphoreP(WORD32 dwSemID, WORD32 dwTimeout)
{
    WORD16 ret = 0;
    if ((dwSemID > g_dwMaxSem)||(dwSemID ==0))
    {
        assert(FALSE);
        return FALSE;
    }
    dwSemID--;
    T_Obj_MutexType  *ptMutex = &g_atMutex[dwSemID];

    if (ptMutex->ucUsed  != VOS_SEM_CTL_USED)
    {
        return FALSE;
    }

    switch (ptMutex->dwFlag)
    {
    case COUNT_SEM_STYLE:
    case SYNC_SEM_STYLE:
        /* �������ʱ���źŴ�ϣ�����ִ�в��������Ľ� */
        /* �������ʱ���źŴ�ϣ�����ִ�в��������Ľ� */
        if (WAIT_FOREVER == dwTimeout)
        {
            /* �������ʱ���źŴ�ϣ�����ִ�в��������Ľ� */
            do
            {
                ret =  sem_wait(&(ptMutex->obj.sem));
            }
            while ((ret != 0) && (errno == EINTR));
        }
        else if (NO_WAIT == dwTimeout)
        {
            do
            {
                ret =  sem_trywait(&(ptMutex->obj.sem));
            }
            while ((ret != 0) && (errno == EINTR));
        }
        else
        {
            /* errno==EINTR���������ź��¼�����ĵȴ��ж� �ޱ� 20081115 */
            struct timespec ts;
            WORD32   dwTmpnsec = 0; 
    
            clock_gettime(CLOCK_REALTIME, &ts );           /* ��ȡ��ǰʱ��       */
            ts.tv_sec += (dwTimeout / 1000 );              /* ���ϵȴ�ʱ������� */
         dwTmpnsec =  (dwTimeout%1000)*1000*1000 + ts.tv_nsec ;
            ts.tv_sec += dwTmpnsec/(1000*1000*1000);  
            ts.tv_nsec = dwTmpnsec%(1000*1000*1000);   /* ���ϵȴ�ʱ�������� */
            do
            {
                ret =  sem_timedwait(&(ptMutex->obj.sem), &ts);
            }while ((ret != 0) && (errno == EINTR));

        }
        if (ret != 0)
        {
            ptMutex->iErrno=errno;            
           return   FALSE;
        }

        break;
    case MUTEX_STYLE:
        if (WAIT_FOREVER == dwTimeout)
        {
            ret =  pthread_mutex_lock(&(ptMutex->obj.mutex));
        }
        else if (NO_WAIT == dwTimeout)
        {
            ret =  pthread_mutex_trylock(&(ptMutex->obj.mutex));
        }
        else
        {
            struct timespec ts;
         WORD32   dwTmpnsec = 0; 
         
            clock_gettime(CLOCK_REALTIME, &ts );           /* ��ȡ��ǰʱ��       */
            ts.tv_sec += (dwTimeout / 1000 );              /* ���ϵȴ�ʱ������� */
         dwTmpnsec =  (dwTimeout%1000)*1000*1000 + ts.tv_nsec ;
         ts.tv_sec += dwTmpnsec/(1000*1000*1000);  
            ts.tv_nsec =  dwTmpnsec % (1000 * 1000*1000);     /* ���ϵȴ�ʱ�������� */
            ret =  pthread_mutex_timedlock(&(ptMutex->obj.mutex), &ts);
        }
        if(ret==EOWNERDEAD)
        {           
            pthread_mutex_consistent_np(&(ptMutex->obj.mutex));
            
        }
        else if (ret != 0)
        {
            ptMutex->iErrno=ret;
            return   FALSE;
        }

        break;
    default:
        IGMP_Print("Vos_SemaphoreP: Flag of semaphore is invalid!\n");
        return FALSE;
        break;
    }

    return TRUE;
}

/**********************************************************************
* �������ƣ�Vos_SemaphoreV
* ����������V����
* ���ʵı���
* �޸ĵı���
* ���������
  WORD32 dwSemID���ź���ID
* ���������
* �� �� ֵ��TRUE��ʾV�����ɹ���FALSE��ʾV����ʧ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2002/10/14    V1.0    ��Ԫ��      ����
************************************************************************/
BOOLEAN Vos_SemaphoreV(WORD32 dwSemID)
{
    INT ret=0;
    if ((dwSemID > g_dwMaxSem)||(dwSemID ==0))
    {
        assert(FALSE);
        return FALSE;
    }
    dwSemID--;
    T_Obj_MutexType  *ptMutex = &g_atMutex[dwSemID];

    if (ptMutex->ucUsed  != VOS_SEM_CTL_USED)
    {
        return FALSE;
    }
    switch (ptMutex->dwFlag)
    {
    case COUNT_SEM_STYLE:
        if (0 != (ret=sem_post(&(ptMutex->obj.sem))))
        {
            ptMutex->iErrno=ret;
            return FALSE;
        }
        break;
    case SYNC_SEM_STYLE:
        if (0 != (ret=sem_post(&(ptMutex->obj.sem))))
        {
            ptMutex->iErrno=ret;
            return FALSE;
        }
        break;
    case MUTEX_STYLE:
        if (0!=(ret=pthread_mutex_unlock(&(ptMutex->obj.mutex))))
        {
            ptMutex->iErrno=ret;
            return FALSE;
        }
        if(ptMutex->obj.mutex.__data.__owner==0)
        {
            ptMutex->dwPtick = 0; /* ���ź����ѱ��ͷ� */
            ptMutex->dwTaskId = 0;
        }
        break;
    default:
        IGMP_Print("Vos_SemaphoreV: Flag of semaphore is invalid!\n");
        return FALSE;
        break;
    }
    return TRUE;
}
/**********************************************************************
 * �������ƣ�InterFaceGetMac
 * ������������ȡ�ӿڵ�MAC
 * ���������
 * ���������
 * �� �� ֵ��
 * ����˵������
 * �޸�����      �汾��  �޸���      �޸�����
 * ---------------------------------------------------------------------
 * 2012/03/28    V1.0    ��ѩ��      ����
 ************************************************************************/
OSS_STATUS InterFaceGetMac(const char  *ifname, CHAR *ucMac)
{
    struct  ifreq   ifr;
    int     fd;
    
    IGMP_Print("InterFaceGetMac is called,this is entry! \n");
    if(NULL == ifname)
    {
        IGMP_Print("InterFaceGetMac ifname is NULL! \n");
        assert(0);
        return OSS_ERROR_UNKNOWN;
    }

    memset(&ifr, 0, sizeof(ifr));

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd == -1)
    {
        IGMP_Print("InterFaceGetMac socket fail! \n");
        assert(0);
        return OSS_ERROR_UNKNOWN;
    }
    /* Get  mac */
    strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
    ifr.ifr_name[IFNAMSIZ - 1] = 0;
    if(ioctl(fd, SIOCGIFHWADDR, &ifr)   <   0)
    {
        IGMP_Print("InterFaceGetMac ioctl fail! \n");
        close(fd);
        return OSS_ERROR_SYSCALL;
    }
    
    close(fd);
    memcpy(ucMac, ifr.ifr_hwaddr.sa_data, ETH_HW_ADDR_LEN);
    IGMP_Print("InterFaceGetMac  is successfully called! \n");
    return OSS_SUCCESS;
}
/**********************************************************************
 * �������ƣ�InterFaceGetIp
 * ������������ȡ�ӿڵ�IP
 * ���������
 * ���������
 * �� �� ֵ��
 * ����˵������
 * �޸�����      �汾��  �޸���      �޸�����
 * ---------------------------------------------------------------------
 * 2012/03/28    V1.0    ��ѩ��      ����
 ************************************************************************/
OSS_STATUS InterFaceGetIp(const char  *ifname, WORD32 *dwIp)
{
    struct  ifreq   ifr;
    int     fd;

    IGMP_Print("InterFaceGetIp is called,this is entry! \n");
    if(NULL == ifname)
    {
        IGMP_Print("InterFaceGetIp ifname is NULL! \n");
        assert(0);
        return OSS_ERROR_UNKNOWN;
    }

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd == -1)
    {
        IGMP_Print("InterFaceGetIp socket fail! \n");
        assert(0);
        return OSS_ERROR_UNKNOWN;
    }

    /* Get  ip */
    strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
    ifr.ifr_name[IFNAMSIZ - 1] = 0;
    if(ioctl(fd, SIOCGIFADDR, &ifr)   <   0)
    {
        IGMP_Print("InterFaceGetIp ioctl fail! \n");
        close(fd);
        return OSS_ERROR_SYSCALL;
    }   
    close(fd);
    memcpy(dwIp,ifr.ifr_addr.sa_data + 2,sizeof(*dwIp));
    IGMP_Print("InterFaceGetIp  is successfully called ! \n");
    return OSS_SUCCESS;
}

/**********************************************************************
* �������ƣ�R_AddToUdpMultiGroup
* ��������������UDP�鲥��,OSS�ڲ��ӿڹ���ʵ��
* ���ʵı���
* �޸ĵı���
* ���������WORD16 wGroupNo �鲥���
            JID *ptJid jid ���Ϊ�ձ�ʾ�߳�
* �����������
* �� �� ֵ��OSS_SUCCESS �ɹ�
            OSS_ERR_UNKNOWN ʧ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2012/03/28    V1.0    ��ѩ��      ����
************************************************************************/
OSS_STATUS R_AddToUdpMultiGroup(WORD16 wGroupNo,JID *ptJid)
{
    WORD32    dwFind    = ISS_MAX_MGROUP_INFO;
    WORD32    dwLoop    = 0;
    WORD32    dwMultiIp = 0;
    //SWORD32   sdwResult = -1;
    struct ip_mreqn   tIpMreg;
    T_ISS_MG_CTRL     *ptIssMgCtrl = NULL;

    ptIssMgCtrl = g_ptIssMgCtrl;
    if (NULL == ptIssMgCtrl)
    {     
            AddIssMgRegRecord(ptJid,wGroupNo,RCV_AIS_MSG_ADD_UDP_MULTI,OSS_ERR_UNKNOWN);
            return OSS_ERR_UNKNOWN;
    }
    if (ISS_MAX_MGROUP_NO <= wGroupNo)
    {
        return OSS_ERR_UNKNOWN;
    }

    Vos_SemaphoreP(ptIssMgCtrl->dwLock, WAIT_FOREVER);
    for (dwLoop = 0;dwLoop < ISS_MAX_MGROUP_INFO;dwLoop++)
    {
        if (ptIssMgCtrl->atIssMgInfo[dwLoop].wGroupNo == wGroupNo)
        {
            if (TRUE == ptIssMgCtrl->atIssMgInfo[dwLoop].bIsJoin)
            {/*�Ѿ����뱾�鲥�� ��ֻ�������ü���*/
                ptIssMgCtrl->atIssMgInfo[dwLoop].dwRefCount++;
                Vos_SemaphoreV(ptIssMgCtrl->dwLock);
                AddIssMgRegRecord(ptJid,wGroupNo,RCV_AIS_MSG_ADD_UDP_MULTI,OSS_SUCCESS);
                return OSS_SUCCESS;
            }
            /*�ҵ�δ���� ������ѭ�������������*/
            break;
        }
        if (ISS_MAX_MGROUP_INFO == dwFind)
        {
            if (FALSE == ptIssMgCtrl->atIssMgInfo[dwLoop].bIsJoin)
            {/*�ݴ�һ�����еı���*/
                dwFind = dwLoop;
            }
        }
    }

    if (ISS_MAX_MGROUP_INFO == dwLoop)
    {
        if (ISS_MAX_MGROUP_INFO != dwFind)
        {
            dwLoop = dwFind;
        }
        else
        {/*����*/
            Vos_SemaphoreV(ptIssMgCtrl->dwLock);
            AddIssMgRegRecord(ptJid,wGroupNo,RCV_AIS_MSG_ADD_UDP_MULTI,OSS_ERR_UNKNOWN);
            return OSS_ERR_UNKNOWN;
        }
    }

    /* �����鲥��ż����鲥IP��ַ */
    dwMultiIp = IssGroupNoToMultiCastIP(wGroupNo);
    
    /* ͨ���׽����򱾵������豸ע���鲥��ַ */
    /* 1.ָ�������鲥����Ϣ��Ҫ�����ľ�������ӿ� */
    /* 2.ָ������Ҫ������鲥��IP */
    tIpMreg.imr_multiaddr.s_addr   = htonl(dwMultiIp);
    tIpMreg.imr_address.s_addr     = htonl(INADDR_ANY);
    tIpMreg.imr_ifindex  =  ptIssMgCtrl->iPortId;
#if 0/* OpenAis�ݲ�ָ���շ�socket --liuxf */
    sdwResult = setsockopt(ptIssMgCtrl->iRcvSocket, 
                           IPPROTO_IP, 
                           IP_ADD_MEMBERSHIP, 
                           (CHAR *)&tIpMreg, 
                           sizeof(tIpMreg)
                           );
    if (-1 == sdwResult)
    {
        Vos_SemaphoreV(ptIssMgCtrl->dwLock);
        AddIssMgRegRecord(ptJid,wGroupNo,RCV_AIS_MSG_ADD_UDP_MULTI,OSS_ERR_UNKNOWN);
        return OSS_ERR_UNKNOWN;
    }
#endif
    ptIssMgCtrl->atIssMgInfo[dwLoop].dwRefCount++;
    ptIssMgCtrl->atIssMgInfo[dwLoop].wGroupNo = wGroupNo;
    ptIssMgCtrl->atIssMgInfo[dwLoop].bIsJoin = TRUE;

    if (0 != ptIssMgCtrl->atIssMgInfo[dwLoop].ucExitSnds)
    {
        ptIssMgCtrl->atIssMgInfo[dwLoop].ucExitSnds = 0;
        if (0 < ptIssMgCtrl->wExitCount)
        {
            ptIssMgCtrl->wExitCount--;
        }
    }
    ptIssMgCtrl->wJoinCount++;
    Vos_SemaphoreV(ptIssMgCtrl->dwLock);
    IssSendIgmpReport();
    AddIssMgRegRecord(ptJid,wGroupNo,RCV_AIS_MSG_ADD_UDP_MULTI,OSS_SUCCESS);
    IGMP_Print("R_AddToUdpMultiGroup LocalCtlIp 0x%lx GroupNo 0x%x MultiCastIp 0x%lx\r\n",htonl(ptIssMgCtrl->dwLocalCtlIp),wGroupNo,dwMultiIp);
    return OSS_SUCCESS;
}
/**********************************************************************
* �������ƣ�R_ExitFromUdpMultiGroup
* �����������˳�UDP�鲥��,OSS�ڲ��ӿڹ���ʵ��
* ���ʵı���
* �޸ĵı���
* ���������WORD16 wGroupNo �鲥���
            JID *ptJid jid ���Ϊ�ձ�ʾ�߳�
* �����������
* �� �� ֵ��OSS_SUCCESS �ɹ�
            OSS_ERR_UNKNOWN ʧ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2012/03/28    V1.0    ��ѩ��      ����
************************************************************************/
OSS_STATUS R_ExitFromUdpMultiGroup(WORD16 wGroupNo,JID *ptJid)
{
    WORD32    dwLoop    = 0;
    WORD32    dwMulitIP = 0;
    //SWORD32   sdwResult = -1;
    struct ip_mreqn  tIpMreg;
    T_ISS_MG_CTRL    *ptIssMgCtrl = NULL;

    ptIssMgCtrl = g_ptIssMgCtrl;
    if (NULL == ptIssMgCtrl)
    {
        if (!InitIgmpv3())
        {
            AddIssMgRegRecord(ptJid,wGroupNo,RCV_AIS_MSG_EXIT_UDP_MULTI,OSS_ERR_UNKNOWN);
            return OSS_ERR_UNKNOWN;
        }
    }
    if (ISS_MAX_MGROUP_NO <= wGroupNo)
    {
        return OSS_ERR_UNKNOWN;
    }

    Vos_SemaphoreP(ptIssMgCtrl->dwLock, WAIT_FOREVER);
    for (dwLoop = 0;dwLoop < ISS_MAX_MGROUP_INFO;dwLoop++)
    {
        if (ptIssMgCtrl->atIssMgInfo[dwLoop].wGroupNo == wGroupNo)
        {
            if (FALSE == ptIssMgCtrl->atIssMgInfo[dwLoop].bIsJoin)
            {/*�Ѿ��˳��鲥����*/
                Vos_SemaphoreV(ptIssMgCtrl->dwLock);
                AddIssMgRegRecord(ptJid,wGroupNo,RCV_AIS_MSG_EXIT_UDP_MULTI,OSS_SUCCESS);
                return OSS_SUCCESS;
            }
            /*�ҵ� ��û�˳� ������ѭ�������˳�����*/
            break;
        }
    }

    if (ISS_MAX_MGROUP_INFO == dwLoop)
    {/*δ�ҵ� ֱ�ӷ���*/
        Vos_SemaphoreV(ptIssMgCtrl->dwLock);
        return OSS_SUCCESS;
    }

    ptIssMgCtrl->atIssMgInfo[dwLoop].dwRefCount--;
    if (0 != ptIssMgCtrl->atIssMgInfo[dwLoop].dwRefCount)
    {/*���ü����ȼ�1 �����Ϊ0 ���ʾ��������Ӧ��������鲥����*/
        Vos_SemaphoreV(ptIssMgCtrl->dwLock);
        return OSS_SUCCESS;
    }

    /* �����鲥��ż����鲥IP��ַ */
    dwMulitIP  = IssGroupNoToMultiCastIP(wGroupNo);
    /* ͨ���׽��ִӱ��������豸ɾ���鲥��ַ */
    /* 1.ָ�������鲥����Ϣ��Ҫ�����ľ�������ӿ� */
    /* 2.ָ������Ҫ������鲥��IP */
    tIpMreg.imr_multiaddr.s_addr   = htonl(dwMulitIP);
    tIpMreg.imr_address.s_addr     = htonl(INADDR_ANY);
    tIpMreg.imr_ifindex = ptIssMgCtrl->iPortId;
#if 0/* OpenAis�ݲ�ָ���շ�socket --liuxf */
    sdwResult= setsockopt(ptIssMgCtrl->iRcvSocket, 
                          IPPROTO_IP, 
                          IP_DROP_MEMBERSHIP, 
                          (CHAR *)&tIpMreg, 
                          sizeof(tIpMreg)
                          );

    if (-1 == sdwResult)
    {
        Vos_SemaphoreV(ptIssMgCtrl->dwLock);
        AddIssMgRegRecord(ptJid,wGroupNo,RCV_AIS_MSG_EXIT_UDP_MULTI,OSS_ERR_UNKNOWN);
        return OSS_ERR_UNKNOWN;
    }
#endif
    /*�鲥��Ż����ܱ� �Ա㷢�˳�������Ϣ*/
    ptIssMgCtrl->atIssMgInfo[dwLoop].bIsJoin = FALSE;
    /*�˳���������ʹ��� ISS_MAX_EXIT_SNDS,�������������û�˳��ɹ�,�����������������ϻ���¼*/
    ptIssMgCtrl->atIssMgInfo[dwLoop].ucExitSnds = ISS_MAX_EXIT_SNDS;
    ptIssMgCtrl->wJoinCount--;
    ptIssMgCtrl->wExitCount++;

    Vos_SemaphoreV(ptIssMgCtrl->dwLock);
    IssSendIgmpReport();
    AddIssMgRegRecord(ptJid,wGroupNo,RCV_AIS_MSG_EXIT_UDP_MULTI,OSS_SUCCESS);
    IGMP_Print("R_ExitFromUdpMultiGroup LocalCtlIp 0x%lx GroupNo 0x%x MultiCastIp 0x%lx\r\n",htonl(ptIssMgCtrl->dwLocalCtlIp),wGroupNo,dwMulitIP);
    return OSS_SUCCESS;
}
/**********************************************************************
* �������ƣ�MgRefcountGet
* ������������ȡ�鲥�����ü���
* ���ʵı���
* �޸ĵı���
* ���������WORD16 wGroupNo �鲥���
* �����������
* �� �� ֵ����Ӧ�����ü���
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2012/03/28    V1.0    ��ѩ��      ����
************************************************************************/
WORD32 MgRefcountGet(WORD16 wGroupNo)
{
    WORD32 dwLoop = 0;
    T_ISS_MG_CTRL     *ptIssMgCtrl = NULL;

    ptIssMgCtrl = g_ptIssMgCtrl;
    if (NULL == ptIssMgCtrl)
    {
        return 0;
    }

    Vos_SemaphoreP(ptIssMgCtrl->dwLock, WAIT_FOREVER);
    for (dwLoop = 0;dwLoop < ISS_MAX_MGROUP_INFO;dwLoop++)
    {
        if (ptIssMgCtrl->atIssMgInfo[dwLoop].wGroupNo == wGroupNo)
        {
            break;
        }
    }
    Vos_SemaphoreV(ptIssMgCtrl->dwLock);

    if (dwLoop < ISS_MAX_MGROUP_INFO)
    {
        return ptIssMgCtrl->atIssMgInfo[dwLoop].dwRefCount;
    }
    else
    {
        return 0;
    }
}
/**********************************************************************
* �������ƣ�CheckAddOrExitMultiGroup
* ��������������ע�����Ϣ������˳��鲥��
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ��OSS_SUCCESS �ɹ� ����ֵ ʧ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2012/03/28    V1.0    ��ѩ��      ����
************************************************************************/
OSS_STATUS CheckAddOrExitMultiGroup(VOID)
{
    OSS_STATUS tRet = OSS_SUCCESS;
#if 0/* ��������ʱ����--liuxf */
#endif
    return tRet;
}

/**********************************************************************
* �������ƣ�IssGetCtrlEthInfo
* ������������ȡ��������������
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ����������, ȫf��ʾʧ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2012/03/28    V1.0    ��ѩ��      ����
************************************************************************/
INT IssGetCtrlEthInfo(VOID)
{

    INT     iSocket  = -1;
    INT     iRet     = LEOF;
    WORD32  iIfIndex = LEOF;
    CHAR    *pcIfName;
    struct  ifreq    ifr;
    CHAR  aucIfName[IFNAMSIZ]={0};

    strncpy(aucIfName, g_ucIfname, IFNAMSIZ);/* ��ʱ�����ڹ̻�ͬ����eth6����������--liuxf */
    pcIfName = strchr(aucIfName,':');

    if (pcIfName)
    {
        pcIfName++;
    }
    else
    {
        pcIfName  = aucIfName;
    }
        
    iSocket = socket(AF_PACKET,SOCK_RAW,htons(ETH_P_ALL));
    if(-1 == iSocket)
    {
        return LEOF;
    }

    memset(&ifr,0,sizeof(struct ifreq));
    strncpy(ifr.ifr_name, pcIfName, IFNAMSIZ);
    if (0 == ioctl(iSocket, SIOCGIFINDEX, &ifr))
    {
        iIfIndex = ifr.ifr_ifindex;
        if (0 == ioctl(iSocket, SIOCGIFHWADDR, &ifr))
        {
            iRet = iIfIndex;
        }
    }
    close(iSocket);
    return iRet;

}

/**********************************************************************
* �������ƣ�WORD32  IssGroupNoToMultiCastIP
* ���������������鲥������鲥IP��ַ������Ϊ�����ֽ���
* ���������
  WORD16 wGroupNo���鲥���
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2012/03/28    V1.0    ��ѩ��      ����
************************************************************************/
/*lint -e119*/
WORD32  IssGroupNoToMultiCastIP(WORD16 wGroupNo)
{
    WORD32 dwMultiCastIp;

    dwMultiCastIp = inet_addr(ISS_MUTICAST_BASE_ADDR);

    return (ntohl(dwMultiCastIp) + wGroupNo);
}
/*lint +e119*/
/**********************************************************************
* �������ƣ�CalcCheckSum
* ��������������У���
* ���������WORD16 *pwAddr �����׵�ַ
            BYTE   ucCount ���ĵĳ���
* �����������
* �� �� ֵ��WORD16   �����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2012/03/28    V1.0    ��ѩ��      ����
************************************************************************/
WORD16 IssCalcCheckSum(WORD16 *pwAddr, BYTE ucCount)
{
    BYTE     ucTempCout = 0; 
    WORD32   dwSum = 0;
    WORD16   *pwTempAddr = NULL;

    if(NULL == pwAddr)
    {
        return (WORD16)0;      
    }

    ucTempCout = ucCount;
    pwTempAddr = pwAddr;
    while (ucTempCout > 0)
    {
        /*�����ֽ����������ֽ���ת������������*/
        dwSum += ntohs(*pwTempAddr);
        ucTempCout--;
        pwTempAddr++;
    }
    /* ��16λ + ��16λ�����ȡ��16λ */
    dwSum  = (dwSum >> 16) + (dwSum & 0x0000FFFF); 
    dwSum += (dwSum>>16);
    dwSum  = dwSum & 0x0000FFFF;
    return   ~((WORD16)dwSum);
}
/**********************************************************************
* �������ƣ�IssSendIgmpPacket
* ��������������IGMP ����
* ���ʵı���
* �޸ĵı���
* ���������VOID *ptData :��������ͷָ��
            WORD32 dwLen :���ĳ���
            INT iPortId  :�˿ں�
* �����������
* �� �� ֵ��OSS_SUCCESS �ɹ� ���� ʧ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2012/03/28    V1.0    ��ѩ��      ����
************************************************************************/
OSS_STATUS IssSendIgmpPacket(VOID *ptData,WORD32 dwLen,INT iPortId)
{
    BYTE      *pucSend = NULL;
    WORD32    dwLoop   = 0;
    SOCKET    iSocket  = -1;
    INT       iRet     = -1;
    struct sockaddr_ll tDstaddr;

    iSocket  = socket(AF_PACKET,SOCK_RAW,htons(ETH_P_ALL));
    if (-1 == iSocket)
    {
        IGMP_Print("IssSendIgmpPacket socket Failed\n");
        return OSS_ERR_UNKNOWN;
    }
    memset(&tDstaddr,0,sizeof(tDstaddr));
    tDstaddr.sll_family   = PF_PACKET;
    tDstaddr.sll_ifindex  = iPortId;

    iRet = sendto(iSocket, 
                  ptData, 
                  dwLen, 
                  0,
                  (struct sockaddr*)&tDstaddr, 
                  sizeof(struct sockaddr_ll)
                  );
    if(iRet < 0)
    {
        IGMP_Print("IssSendIgmpPacket sendto Failed,%s\n",strerror(errno));
        return OSS_ERR_UNKNOWN;
    }
    close(iSocket);
    if (g_bIssIgmpPrnEn)
    {
        /*����IGMP���Ĵ�ӡ*/
        IGMP_Print("IssSendIgmpPacket %lu Bytes Send\r\n",dwLen);
        pucSend = (BYTE *)ptData;
        for (dwLoop = 0;dwLoop < dwLen;dwLoop++)
        {
            if ((0 != dwLoop) && (0 == (dwLoop%10)))
            {
                IGMP_Print("\r\n");
            }
            if ((0 != dwLoop) && (0 == (dwLoop%2)) && (0 != (dwLoop%10)))
            {
                IGMP_Print(" ");
            }
            IGMP_Print("%.2x",*pucSend);
            pucSend++;
        }
        IGMP_Print("\r\n");
    }
    return OSS_SUCCESS;
}
/**********************************************************************
* �������ƣ�IssSendIgmpReport
* ��������������IGMP report����
* ���������
  WORD16 wGroupNo���鲥���
* �����������
* �� �� ֵ��OSS_SUCCESS �ɹ� ���� ʧ��
* ����˵��������������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2012/03/28    V1.0    ��ѩ��      ����
************************************************************************/
OSS_STATUS IssSendIgmpReport(VOID)
{
    WORD16 wIgmpPkgLen = 0;
    WORD16 wCheckSum   = 0;
    WORD32 dwLoop = 0;
    WORD32 dwRecordCount = 0;
    WORD32 dwMultiCastIp = 0;
    T_ISS_MG_CTRL *ptIssMgCtrl = NULL;
    T_ISS_IGMPV3_PKT *ptIssIgpmPkt = NULL;
    T_ISS_IGMPV3_Report *ptIssIgmpReport = NULL;
    OSS_STATUS tRet = OSS_ERR_UNKNOWN;
    T_ISS_Group_Record  tGroupRecord;

    ptIssMgCtrl = g_ptIssMgCtrl;
    if (NULL == ptIssMgCtrl)
    {
        return OSS_ERR_UNKNOWN;
    }

    /* ��ȡ�����ź��� �������� ���ξͲ������� ���´��ٷ�
    *  Ŀǰ���ź���ֻ��һ���߳���ʹ�ã�����ÿ�ζ����Ի�ȡ��
    */
    if (FALSE == Vos_SemaphoreP(ptIssMgCtrl->dwLock, NO_WAIT))
    {
        return OSS_ERR_UNKNOWN;
    }
    if (0 != ptIssMgCtrl->wExitCount)
    {
        memset(&tGroupRecord,0x0,sizeof(T_ISS_Group_Record));
        tGroupRecord.ucRecordType = ISS_IGMP_EXIT_REC_TYPE;
    
        ptIssIgpmPkt = (T_ISS_IGMPV3_PKT *)&ptIssMgCtrl->tIssIgmpV3Pkt;
    
        ptIssIgpmPkt->tIPHead.dwDestAddr = DEF_ISS_IGMP_DSTIP;
        memcpy(ptIssIgpmPkt->acDestMac,DEF_ISS_IGMP_DSTMAC, ETH_HW_ADDR_LEN);
    
        ptIssIgmpReport = (T_ISS_IGMPV3_Report *)&ptIssIgpmPkt->tIGMPV3;
        ptIssIgmpReport->wGroupRecordNum = 0;
    
        for (dwLoop = 0;dwLoop < ISS_MAX_MGROUP_INFO;dwLoop++)
        {
            if (0 != ptIssMgCtrl->atIssMgInfo[dwLoop].ucExitSnds)
            {
                dwMultiCastIp = IssGroupNoToMultiCastIP(ptIssMgCtrl->atIssMgInfo[dwLoop].wGroupNo);
                tGroupRecord.dwMultiCastAddress = htonl(dwMultiCastIp);
                memcpy(&ptIssIgmpReport->atGroupRecord[dwRecordCount],
                       &tGroupRecord,
                       sizeof(T_ISS_Group_Record));
                dwRecordCount++;
                ptIssMgCtrl->atIssMgInfo[dwLoop].ucExitSnds--;
                if (0 == ptIssMgCtrl->atIssMgInfo[dwLoop].ucExitSnds)
                {
                    ptIssMgCtrl->atIssMgInfo[dwLoop].wGroupNo = ISS_INVALID_GROUP_NO;
                    ptIssMgCtrl->wExitCount--;
                    if (0 == ptIssMgCtrl->wExitCount)
                    {
                        break;
                    }
                }
            }
        }

        ptIssIgmpReport->wGroupRecordNum = htons(dwRecordCount); 
    
        wIgmpPkgLen = ISS_FRAME_LEN +  ISS_IP_LEN + ISS_IGMP_HEAD_LEN\
                   + sizeof(T_ISS_Group_Record)*dwRecordCount; 
        ptIssIgpmPkt->tIPHead.wTotalLen = htons(wIgmpPkgLen - ISS_FRAME_LEN);
    
        /* ����IPͷ��У���,����0 */
        ptIssIgpmPkt->tIPHead.wCheckSum = 0;
        wCheckSum = IssCalcCheckSum((WORD16 *)(&ptIssIgpmPkt->tIPHead),ISS_IP_LEN>>1);
        ptIssIgpmPkt->tIPHead.wCheckSum = htons(wCheckSum);
        /* ����IGMPͷ��У��� */
        ptIssIgpmPkt->tIGMPV3.wChecksum = 0;
        wCheckSum = IssCalcCheckSum((WORD16*)(&ptIssIgpmPkt->tIGMPV3),(wIgmpPkgLen - ISS_FRAME_LEN - ISS_IP_LEN)>>1);
        ptIssIgpmPkt->tIGMPV3.wChecksum = htons(wCheckSum);
    
        tRet = IssSendIgmpPacket(ptIssIgpmPkt,wIgmpPkgLen,ptIssMgCtrl->iPortId);
        if (OSS_SUCCESS != tRet)
        {
            ptIssMgCtrl->tIssMgSts.dwSndExitFails++;
            Vos_SemaphoreV(ptIssMgCtrl->dwLock);
            return tRet;
        }
        ptIssMgCtrl->tIssMgSts.dwSndExitSuccesss++;
    }

    if (0 != ptIssMgCtrl->wJoinCount)
    {
        dwRecordCount = 0;
        memset(&tGroupRecord,0x0,sizeof(T_ISS_Group_Record));
        tGroupRecord.ucRecordType = ISS_IGMP_JOIN_REC_TYPE;
    
        ptIssIgpmPkt = (T_ISS_IGMPV3_PKT *)&ptIssMgCtrl->tIssIgmpV3Pkt;
    
        ptIssIgpmPkt->tIPHead.dwDestAddr = DEF_ISS_IGMP_DSTIP;
        memcpy(ptIssIgpmPkt->acDestMac,DEF_ISS_IGMP_DSTMAC, ETH_HW_ADDR_LEN);
    
        ptIssIgmpReport = (T_ISS_IGMPV3_Report *)&ptIssIgpmPkt->tIGMPV3;
        ptIssIgmpReport->wGroupRecordNum = 0;
    
        for (dwLoop = 0;dwLoop < ISS_MAX_MGROUP_INFO;dwLoop++)
        {
            if (TRUE == ptIssMgCtrl->atIssMgInfo[dwLoop].bIsJoin)
            {
                dwMultiCastIp = IssGroupNoToMultiCastIP(ptIssMgCtrl->atIssMgInfo[dwLoop].wGroupNo);
                tGroupRecord.dwMultiCastAddress = htonl(dwMultiCastIp);
                memcpy(&ptIssIgmpReport->atGroupRecord[dwRecordCount],
                       &tGroupRecord,
                       sizeof(T_ISS_Group_Record));
                dwRecordCount++;
            }
        }

        ptIssIgmpReport->wGroupRecordNum = htons(dwRecordCount); 
    
        wIgmpPkgLen = ISS_FRAME_LEN +  ISS_IP_LEN + ISS_IGMP_HEAD_LEN\
                   + sizeof(T_ISS_Group_Record)*dwRecordCount; 
        ptIssIgpmPkt->tIPHead.wTotalLen = htons(wIgmpPkgLen - ISS_FRAME_LEN);
    
        /* ����IPͷ��У���,����0 */
        ptIssIgpmPkt->tIPHead.wCheckSum = 0;
        wCheckSum = IssCalcCheckSum((WORD16 *)(&ptIssIgpmPkt->tIPHead),ISS_IP_LEN>>1);
        ptIssIgpmPkt->tIPHead.wCheckSum = htons(wCheckSum);
        /* ����IGMPͷ��У��� */
        ptIssIgpmPkt->tIGMPV3.wChecksum = 0;
        wCheckSum = IssCalcCheckSum((WORD16*)(&ptIssIgpmPkt->tIGMPV3),(wIgmpPkgLen - ISS_FRAME_LEN - ISS_IP_LEN)>>1);
        ptIssIgpmPkt->tIGMPV3.wChecksum = htons(wCheckSum);
    
        tRet = IssSendIgmpPacket(ptIssIgpmPkt,wIgmpPkgLen,ptIssMgCtrl->iPortId);
        if (OSS_SUCCESS != tRet)
        {
            ptIssMgCtrl->tIssMgSts.dwSndJointFails++;
            Vos_SemaphoreV(ptIssMgCtrl->dwLock);
            return tRet;
        }
        ptIssMgCtrl->tIssMgSts.dwSndJointSuccesss++;
    }
    Vos_SemaphoreV(ptIssMgCtrl->dwLock);
    return OSS_SUCCESS;
}
/**********************************************************************
* �������ƣ�XOS_DbgIssIgmpPrnEnable
* ������������ӡIss��Ϣ
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����        �汾��     �޸���          �޸�����
* -----------------------------------------------
* 2009/10/09       V1.0       ��ѩ��          ����
************************************************************************/
VOID XOS_DbgIssIgmpPrnEnable(VOID)
{
    g_bIssIgmpPrnEn = TRUE;    
}
/**********************************************************************
* �������ƣ�XOS_DbgIssIgmpPrnDisable
* �����������رմ�ӡIss��Ϣ
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����        �汾��     �޸���          �޸�����
* -----------------------------------------------
* 2009/10/09       V1.0       ��ѩ��          ����
************************************************************************/
VOID XOS_DbgIssIgmpPrnDisable(VOID)
{
    g_bIssIgmpPrnEn = FALSE;
}
/**********************************************************************
* �������ƣ�AddIssMgRegRecord
* ��������������鲥������˳���¼
* ���ʵı���
* �޸ĵı���
* ���������
    JID *ptJid     JOB ID
    WORD16 wGroupNo �鲥���
    BYTE ucRegType ע������
    WORD32 dwStatus  ע�������
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����          �汾��     �޸���          �޸�����
* -----------------------------------------------------
* 2009/07/03        V1.0       ��ѩ��            ����
************************************************************************/
VOID AddIssMgRegRecord(JID *ptJid,WORD16 wGroupNo,BYTE ucRegType,WORD32 dwStatus)
{
    IGMP_Print("AddIssMgRegRecord entry! \r\n");
    if (ISS_MAX_MGROUP_NO <= wGroupNo)
    {
        return ;
    }
    if ((RCV_AIS_MSG_ADD_UDP_MULTI != ucRegType)
        && (RCV_AIS_MSG_EXIT_UDP_MULTI != ucRegType))
    {
        return;
    }
    s_ucIssMgRegRedPos = s_ucIssMgRegRedPos % MAX_ISS_MG_REG_RED;
    memset(&(s_atIssMgRegRed[s_ucIssMgRegRedPos].tTime),0,sizeof(time_t));
    time(&(s_atIssMgRegRed[s_ucIssMgRegRedPos].tTime));
    if (NULL == ptJid)
    {
        s_atIssMgRegRed[s_ucIssMgRegRedPos].ucType = ISS_SDER_TYPE_THREAD;
    }
    else
    {
        IGMP_Print("AddIssMgRegRecord ptJid need NULL \r\n");
    }
    s_atIssMgRegRed[s_ucIssMgRegRedPos].wGroupNo  = wGroupNo;
    s_atIssMgRegRed[s_ucIssMgRegRedPos].ucRegType = ucRegType;
    s_atIssMgRegRed[s_ucIssMgRegRedPos].dwStatus  = dwStatus;

    s_ucIssMgRegRedPos++;

    return;
}
/**********************************************************************
* �������ƣ�InitIgmpv3
* ������������ʼ��Igmpv3
* ���ʵı���
* �޸ĵı���
* ���������
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����        �汾��     �޸���          �޸�����
* -----------------------------------------------
* 2009/10/09      V1.0       ��ѩ��          ����
************************************************************************/
BOOLEAN  InitIgmpv3(VOID)
{
    BOOLEAN       bSemP = FALSE;
    BOOLEAN       bRet     = FALSE;
    WORD32        dwLoop = 0;
    T_ISS_MG_CTRL *ptIssMgCtrl = NULL;
    T_ISS_IGMPV3_PKT *ptIssIgmpPkt = NULL;
    CHAR ucMac[ETH_HW_ADDR_LEN]={0};
    OSS_STATUS    tRet = OSS_ERR_UNKNOWN;

    /* ��ʼ�������ͺ��������濼�ǲ�ʹ�÷�װ�ź����ӿ�--liuxf */
    g_atMutex=(T_Obj_MutexType *)malloc(sizeof(T_Obj_MutexType)*g_dwMaxSem);    
    if(NULL==g_atMutex)
    {
        IGMP_Print("InitIgmpv3:malloc for g_atMutex err\n");
        return FALSE;
    }
    memset(g_atMutex,0,sizeof(T_Obj_MutexType)*g_dwMaxSem);    
    for (dwLoop = 0; dwLoop < g_dwMaxSem; dwLoop++)
    {
        g_atMutex[dwLoop].ucUsed = VOS_SEM_CTL_NO_USED;
    }    
    
    g_ptIssMgCtrl = (T_ISS_MG_CTRL *)malloc(sizeof(T_ISS_MG_CTRL));
    if (NULL == g_ptIssMgCtrl)
    {
        IGMP_Print("InitIgmpv3 malloc g_ptIssMgCtrl Failed\n");
        goto ERRRET;
    }
    ptIssMgCtrl = g_ptIssMgCtrl;
    memset((BYTE *)ptIssMgCtrl,0,sizeof(T_ISS_MG_CTRL));
    
    bRet = Vos_CreateSemaphore(0, 0, MUTEX_STYLE, &ptIssMgCtrl->dwLock);
    if (TRUE != bRet)
    {
        IGMP_Print("InitIgmpv3 Vos_CreateSemaphore Failed\n");
        goto ERRRET;
    }

    ptIssMgCtrl->pucRcvBuf = (BYTE *)malloc(ISS_MSG_MAX_LEN);
    if (NULL == ptIssMgCtrl->pucRcvBuf)
    {
        IGMP_Print("InitIgmpv3 malloc pucRcvBuf Failed\n");
        goto ERRRET;
    }

    Vos_SemaphoreP(ptIssMgCtrl->dwLock, WAIT_FOREVER);
    bSemP = TRUE;
    ptIssMgCtrl->iRcvSocket = -1;
    ptIssMgCtrl->iSndSocket = -1;
    for (dwLoop = 0;dwLoop < ISS_MAX_MGROUP_INFO;dwLoop++)
    {
        ptIssMgCtrl->atIssMgInfo[dwLoop].wGroupNo = ISS_INVALID_GROUP_NO;
    }
    ptIssMgCtrl->dwMgBaseIp = inet_addr(ISS_MUTICAST_BASE_ADDR);

    ptIssIgmpPkt = (T_ISS_IGMPV3_PKT *)&ptIssMgCtrl->tIssIgmpV3Pkt;

    ptIssIgmpPkt->wType                = htons(ISS_FRAME_TYPE);
    ptIssIgmpPkt->tIPHead.ucVerAndHLen = ISS_IP_VER;
    ptIssIgmpPkt->tIPHead.ucTTL             = ISS_IP_IGMP_TTL;
    ptIssIgmpPkt->tIPHead.ucType           = ISS_IP_IGMP_TYPE;
    ptIssIgmpPkt->tIGMPV3.ucType          = ISS_IGMPV3_VER;
    /* ��ȡ������mac��ַ */    
    tRet = InterFaceGetMac(g_ucIfname,ucMac);
    if (OSS_SUCCESS != tRet)
    {
        IGMP_Print("InitIgmpv3 InterFaceGetMac Failed,tRet 0x%lx \n",tRet);
        goto ERRRET;
    }
    IGMP_Print("InitIgmpv3:Mac is %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x. \n",
                   ucMac[0], ucMac[1], ucMac[2], ucMac[3], ucMac[4], ucMac[5]);;
    memcpy(ptIssIgmpPkt->acSourceMac,ucMac,ETH_HW_ADDR_LEN);
    
    /* ��ȡ������ip��ַ */    
    tRet = InterFaceGetIp(g_ucIfname,&ptIssMgCtrl->dwLocalCtlIp);
    if (OSS_SUCCESS != tRet)
    {
        IGMP_Print("InitIgmpv3 InterFaceGetIp Failed,tRet 0x%lx \n",tRet);
        goto ERRRET;
    }
    ptIssIgmpPkt->tIPHead.dwSourceAddr = htonl(ptIssMgCtrl->dwLocalCtlIp); 
    IGMP_Print("InitIgmpv3: Ip = 0x%x!\n",ptIssIgmpPkt->tIPHead.dwSourceAddr);
    /* ��ȡ�������������� */
    ptIssMgCtrl->iPortId = IssGetCtrlEthInfo(); 
    if (LEOF == (WORD32)ptIssMgCtrl->iPortId)
    {
        IGMP_Print("InitIgmpv3 IssGetCtrlEthInfo Failed! \n"); 
        goto ERRRET;
    }


    Vos_SemaphoreV(ptIssMgCtrl->dwLock);
    s_ucIssMgRegRedPos = 0;
    memset(&s_atIssMgRegRed,0,MAX_ISS_MG_REG_RED*sizeof(T_IssMgRegRed));
    return TRUE;

ERRRET:
    if (TRUE == bSemP)
    {
        Vos_SemaphoreV(ptIssMgCtrl->dwLock);
    }
    if (TRUE == bRet)
    {
        Vos_DestroySemaphore(ptIssMgCtrl->dwLock);
    }
    if (NULL != ptIssMgCtrl)
    {
        if (NULL != ptIssMgCtrl->pucRcvBuf)
        {
            free(ptIssMgCtrl->pucRcvBuf);
            ptIssMgCtrl->pucRcvBuf = NULL;
        }
        free((BYTE *)ptIssMgCtrl);
        g_ptIssMgCtrl = NULL;
    }

    return FALSE;
}


