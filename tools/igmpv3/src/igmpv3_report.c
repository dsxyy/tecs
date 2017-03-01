/*********************************************************************
* 版权所有 (C)2012, 深圳市中兴通讯股份有限公司。
*
* 文件名称： igmpv3_report.c
* 文件标识： 
* 其它说明： 
* 当前版本： V1.0
* 作    者： liuxuefeng
* 完成日期： 
*
* 修改记录1：
*    修改日期：2012年03月28日
*    版 本 号：V1.0
*    修 改 人：liuxuefeng
*    修改内容：创建
**********************************************************************/
/**************************************************************************
*                            头文件                                        *
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
*                            常量                                        *
**************************************************************************/
#define MAX_ISS_MG_REG_RED                   ((BYTE)20)           /*ISS 加入退出组播组最大记录条数*/

#define RCV_AIS_MSG_ADD_UDP_MULTI            ((BYTE)0x02)         /*加入udp组播组*/
#define RCV_AIS_MSG_EXIT_UDP_MULTI           ((BYTE)0x03)         /*退出udp组播组*/
#define  GPS_SYNC_LOCAL_INTERVAL             ((BYTE)16)           /*每隔16秒同步一次本地时间戳*/

#define ISS_IGMP_REPORT_INTERVAL             ((BYTE)10)           /*发送IGMPREPORT间隔*/
#define ISS_SDER_TYPE_JOB          ((BYTE)1) /**< JOB */
#define ISS_SDER_TYPE_THREAD       ((BYTE)2) /**< THREAD */

#define AIS_CMD_TYPE_ADD_UDP_MULTI  ((BYTE)0x01) /**< 加入组播组 */
#define AIS_CMD_TYPE_EXIT_UDP_MULTI ((BYTE)0x02) /**< 退出组播组 */

/**************************************************************************
*                            宏                                          *
**************************************************************************/

/**************************************************************************
*                          数据类型                                       *
**************************************************************************/


/**************************************************************************
*                           全局函数                                  *
**************************************************************************/
extern   WORD32  IssGroupNoToMultiCastIP(WORD16 wGroupNo);
extern VOID AddIssMgRegRecord(JID *ptJid,WORD16 wGroupNo,BYTE ucRegType,WORD32 dwStatus);
extern BOOLEAN  InitIgmpv3(VOID);
/**************************************************************************
*                           全局变量                                      *
**************************************************************************/
BOOLEAN              g_bIssIgmpPrnEn = TRUE;                          /*ISS消息打印开关*/
T_ISS_MG_CTRL  *g_ptIssMgCtrl        = NULL;     /*ISS组播组控制结构*/
INT            g_iIssSndSocket       = -1;       /*ISS组播发送socket*/
extern char g_ucIfname[IFNAMSIZ];

/**************************************************************************
*                           本地变量                                      *
**************************************************************************/
static  BYTE                 s_ucIssMgRegRedPos = 0;                   /*记录组播组加入退出记录数组索引*/
static  T_IssMgRegRed  s_atIssMgRegRed[MAX_ISS_MG_REG_RED];      /*ISS加入退出组播组记录数组*/
/**************************************************************************
*                           局部函数原型                                  *
**************************************************************************/


/**************************************************************************
*                           函数实现                                  *
**************************************************************************/

/* VOS的封装，暂时先放这儿--liuxf */
/* 信号量类型 */
#define COUNT_SEM_STYLE             (WORD32)0x01        /**< 计数信号量 */
#define SYNC_SEM_STYLE              (WORD32)0x10        /**< 同步信号量 */
#define MUTEX_STYLE                 (WORD32)0x20        /**< 互斥，VxWorks对应的是互斥信号量，NT下对应的是互斥对象 */
#define RWLOCK_STYLE                 (WORD32)0x40       /**< 读写锁类型 */
    
#define RWLOCK_READFITST_MODE              0x01               /**< 读写锁读优先模式 */
#define RWLOCK_WRITEFITST_MODE             0x10               /**< 读写锁写优先模式 */
/* 局部宏 */
#define  DEF_OBJ_COUNT             1000
/* 2.2 信号量操作所用宏 */
#define VOS_SEM_CTL_USED               (BYTE)0x01  /* 队列控制块被使用 */
#define VOS_SEM_CTL_NO_USED         (BYTE)0x10  /* 队列控制块未被使用 */

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
    WORD32            dwTaskId;         /* 拥有此进程内信号量的线程tid */
    WORD32            dwPtick;          /* 进程内信号量P操作成功发生的tick值 */
    WORD32            dwMode;
	WORD32            dwCreate;
    INT               iErrno;
    BYTE              ucUsed;
    BYTE              ucRwFlag; 
    BYTE              ucPad[2];
}T_Obj_MutexType;
static      pthread_mutex_t         s_tMutexLock;
T_Obj_MutexType       *g_atMutex;  /* todo zhugw: 需要malloc申请大小*/
WORD32      g_dwMaxSem=DEF_OBJ_COUNT;
WORD32      g_dwSemCount=0;

/**********************************************************************
* 函数名称：Vos_CreateSemaphore
* 功能描述：创建信号量
* 访问的表：无
* 修改的表：无
* 输入参数：
  WORD32 dwInitVal：信号量的初始值
  WORD32 dwMaxVal：NT下的参数，表示信号量计数可以达到的最大值
  WORD32 dwFlag：创建何种类型的信号量
                 COUNT_SEM_STYLE：创建计数信号量
                 SYNC_SEM_STYLE：创建同步信号量
                 MUTEX_STYLE：创建互斥对象，VxWorks对应的是互斥信号量，
                              NT下对应的是互斥对象
  WORD32 *pdwSemID：出口参数，信号量ID
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2002/10/14    V1.0    周元庆      创建
* 2002/11/30    V1.0    周元庆      加入断言
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
    /* 访问g_atMutex时增加互斥保护 */
    if (pthread_mutex_lock(&s_tMutexLock)!=0)
    {

        IGMP_Print("Vos_CreateSemaphore:mutex_lock failed!\n");
        return FALSE;
    }
    /* 查找空闲信号量控制块 */
    for (dwLoop = 0; dwLoop < g_dwMaxSem; dwLoop ++)
    {
        if (g_atMutex[dwLoop].ucUsed == VOS_SEM_CTL_NO_USED)
        {
            /* 找到可用的信号量控制块 */
            dwSemId  =  dwLoop;
            // To do : 需要事先申请大小
            ptMutex  =  &g_atMutex[dwLoop];
            break;
        }
    }

    if (g_dwMaxSem == dwLoop)
    {
        /* 说明没有找到可用的信号量控制块 */
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
        /* lizl, 让互斥锁可重入 */
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);
        pthread_mutexattr_setrobust_np(&attr, PTHREAD_MUTEX_ROBUST_NP);
        /*成研对优先级反转支持还有问题，暂时去掉*/
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

    return TRUE; /* 只是为了在编译的时候不产生告警 */
}

/**********************************************************************
* 函数名称：Vos_DestroySemaphore
* 功能描述：删除信号量
* 访问的表：无
* 修改的表：无
* 输入参数：
  WORD32 dwSemID
* 输出参数：
* 返 回 值：TRUE表示删除信号量成功，FALSE表示删除信号量失败
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2002/10/14    V1.0    周元庆      创建
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
* 函数名称：Vos_SemaphoreP
* 功能描述：P操作
* 访问的表：无
* 修改的表：无
* 输入参数：
  WORD32 dwSemID：信号量ID
  WORD32 dwTimeout：超时值，单位为毫秒
* 输出参数：
* 返 回 值：TRUE表示P操作成功，FALSE表示P操作失败
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2002/10/14    V1.0    周元庆      创建
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
        /* 如果阻塞时被信号打断，重新执行操作。张文剑 */
        /* 如果阻塞时被信号打断，重新执行操作。张文剑 */
        if (WAIT_FOREVER == dwTimeout)
        {
            /* 如果阻塞时被信号打断，重新执行操作。张文剑 */
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
            /* errno==EINTR屏蔽其他信号事件引起的等待中断 罗斌 20081115 */
            struct timespec ts;
            WORD32   dwTmpnsec = 0; 
    
            clock_gettime(CLOCK_REALTIME, &ts );           /* 获取当前时间       */
            ts.tv_sec += (dwTimeout / 1000 );              /* 加上等待时间的秒数 */
         dwTmpnsec =  (dwTimeout%1000)*1000*1000 + ts.tv_nsec ;
            ts.tv_sec += dwTmpnsec/(1000*1000*1000);  
            ts.tv_nsec = dwTmpnsec%(1000*1000*1000);   /* 加上等待时间纳秒数 */
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
         
            clock_gettime(CLOCK_REALTIME, &ts );           /* 获取当前时间       */
            ts.tv_sec += (dwTimeout / 1000 );              /* 加上等待时间的秒数 */
         dwTmpnsec =  (dwTimeout%1000)*1000*1000 + ts.tv_nsec ;
         ts.tv_sec += dwTmpnsec/(1000*1000*1000);  
            ts.tv_nsec =  dwTmpnsec % (1000 * 1000*1000);     /* 加上等待时间纳秒数 */
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
* 函数名称：Vos_SemaphoreV
* 功能描述：V操作
* 访问的表：无
* 修改的表：无
* 输入参数：
  WORD32 dwSemID：信号量ID
* 输出参数：
* 返 回 值：TRUE表示V操作成功，FALSE表示V操作失败
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2002/10/14    V1.0    周元庆      创建
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
            ptMutex->dwPtick = 0; /* 本信号量已被释放 */
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
 * 函数名称：InterFaceGetMac
 * 功能描述：获取接口的MAC
 * 输入参数：
 * 输出参数：
 * 返 回 值：
 * 其它说明：无
 * 修改日期      版本号  修改人      修改内容
 * ---------------------------------------------------------------------
 * 2012/03/28    V1.0    刘雪峰      创建
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
 * 函数名称：InterFaceGetIp
 * 功能描述：获取接口的IP
 * 输入参数：
 * 输出参数：
 * 返 回 值：
 * 其它说明：无
 * 修改日期      版本号  修改人      修改内容
 * ---------------------------------------------------------------------
 * 2012/03/28    V1.0    刘雪峰      创建
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
* 函数名称：R_AddToUdpMultiGroup
* 功能描述：加入UDP组播组,OSS内部接口功能实现
* 访问的表：无
* 修改的表：无
* 输入参数：WORD16 wGroupNo 组播组号
            JID *ptJid jid 如果为空表示线程
* 输出参数：无
* 返 回 值：OSS_SUCCESS 成功
            OSS_ERR_UNKNOWN 失败
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2012/03/28    V1.0    刘雪峰      创建
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
            {/*已经加入本组播组 则只增加引用计数*/
                ptIssMgCtrl->atIssMgInfo[dwLoop].dwRefCount++;
                Vos_SemaphoreV(ptIssMgCtrl->dwLock);
                AddIssMgRegRecord(ptJid,wGroupNo,RCV_AIS_MSG_ADD_UDP_MULTI,OSS_SUCCESS);
                return OSS_SUCCESS;
            }
            /*找到未加入 则跳出循环继续加入操作*/
            break;
        }
        if (ISS_MAX_MGROUP_INFO == dwFind)
        {
            if (FALSE == ptIssMgCtrl->atIssMgInfo[dwLoop].bIsJoin)
            {/*暂存一个空闲的备用*/
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
        {/*满了*/
            Vos_SemaphoreV(ptIssMgCtrl->dwLock);
            AddIssMgRegRecord(ptJid,wGroupNo,RCV_AIS_MSG_ADD_UDP_MULTI,OSS_ERR_UNKNOWN);
            return OSS_ERR_UNKNOWN;
        }
    }

    /* 根据组播组号计算组播IP地址 */
    dwMultiIp = IssGroupNoToMultiCastIP(wGroupNo);
    
    /* 通过套接字向本地网络设备注册组播地址 */
    /* 1.指定接收组播组消息所要依附的具体网络接口 */
    /* 2.指定本地要加入的组播组IP */
    tIpMreg.imr_multiaddr.s_addr   = htonl(dwMultiIp);
    tIpMreg.imr_address.s_addr     = htonl(INADDR_ANY);
    tIpMreg.imr_ifindex  =  ptIssMgCtrl->iPortId;
#if 0/* OpenAis暂不指定收发socket --liuxf */
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
* 函数名称：R_ExitFromUdpMultiGroup
* 功能描述：退出UDP组播组,OSS内部接口功能实现
* 访问的表：无
* 修改的表：无
* 输入参数：WORD16 wGroupNo 组播组号
            JID *ptJid jid 如果为空表示线程
* 输出参数：无
* 返 回 值：OSS_SUCCESS 成功
            OSS_ERR_UNKNOWN 失败
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2012/03/28    V1.0    刘雪峰      创建
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
            {/*已经退出组播组了*/
                Vos_SemaphoreV(ptIssMgCtrl->dwLock);
                AddIssMgRegRecord(ptJid,wGroupNo,RCV_AIS_MSG_EXIT_UDP_MULTI,OSS_SUCCESS);
                return OSS_SUCCESS;
            }
            /*找到 还没退出 则跳出循环继续退出操作*/
            break;
        }
    }

    if (ISS_MAX_MGROUP_INFO == dwLoop)
    {/*未找到 直接返回*/
        Vos_SemaphoreV(ptIssMgCtrl->dwLock);
        return OSS_SUCCESS;
    }

    ptIssMgCtrl->atIssMgInfo[dwLoop].dwRefCount--;
    if (0 != ptIssMgCtrl->atIssMgInfo[dwLoop].dwRefCount)
    {/*引用计数先减1 如果不为0 则表示还有其他应用在这个组播组中*/
        Vos_SemaphoreV(ptIssMgCtrl->dwLock);
        return OSS_SUCCESS;
    }

    /* 根据组播组号计算组播IP地址 */
    dwMulitIP  = IssGroupNoToMultiCastIP(wGroupNo);
    /* 通过套接字从本地网络设备删除组播地址 */
    /* 1.指定接收组播组消息所要依附的具体网络接口 */
    /* 2.指定本地要加入的组播组IP */
    tIpMreg.imr_multiaddr.s_addr   = htonl(dwMulitIP);
    tIpMreg.imr_address.s_addr     = htonl(INADDR_ANY);
    tIpMreg.imr_ifindex = ptIssMgCtrl->iPortId;
#if 0/* OpenAis暂不指定收发socket --liuxf */
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
    /*组播组号还不能变 以便发退出报告消息*/
    ptIssMgCtrl->atIssMgInfo[dwLoop].bIsJoin = FALSE;
    /*退出报告最大发送次数 ISS_MAX_EXIT_SNDS,如果超过次数还没退出成功,则依赖交换板自行老化记录*/
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
* 函数名称：MgRefcountGet
* 功能描述：获取组播组引用计数
* 访问的表：无
* 修改的表：无
* 输入参数：WORD16 wGroupNo 组播组号
* 输出参数：无
* 返 回 值：对应的引用计数
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2012/03/28    V1.0    刘雪峰      创建
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
* 函数名称：CheckAddOrExitMultiGroup
* 功能描述：根据注册表信息加入或退出组播组
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：OSS_SUCCESS 成功 其他值 失败
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2012/03/28    V1.0    刘雪峰      创建
************************************************************************/
OSS_STATUS CheckAddOrExitMultiGroup(VOID)
{
    OSS_STATUS tRet = OSS_SUCCESS;
#if 0/* 本功能暂时不用--liuxf */
#endif
    return tRet;
}

/**********************************************************************
* 函数名称：IssGetCtrlEthInfo
* 功能描述：获取控制面网卡索引
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：网卡索引, 全f表示失败
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2012/03/28    V1.0    刘雪峰      创建
************************************************************************/
INT IssGetCtrlEthInfo(VOID)
{

    INT     iSocket  = -1;
    INT     iRet     = LEOF;
    WORD32  iIfIndex = LEOF;
    CHAR    *pcIfName;
    struct  ifreq    ifr;
    CHAR  aucIfName[IFNAMSIZ]={0};

    strncpy(aucIfName, g_ucIfname, IFNAMSIZ);/* 临时跟网口固化同步用eth6，后续整改--liuxf */
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
* 函数名称：WORD32  IssGroupNoToMultiCastIP
* 功能描述：根据组播组计算组播IP地址，返回为本地字节序
* 输入参数：
  WORD16 wGroupNo：组播组号
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2012/03/28    V1.0    刘雪峰      创建
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
* 函数名称：CalcCheckSum
* 功能描述：计算校验和
* 输入参数：WORD16 *pwAddr 报文首地址
            BYTE   ucCount 报文的长度
* 输出参数：无
* 返 回 值：WORD16   检验和
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2012/03/28    V1.0    刘雪峰      创建
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
        /*报文字节序是网络字节序，转换本地来计算*/
        dwSum += ntohs(*pwTempAddr);
        ucTempCout--;
        pwTempAddr++;
    }
    /* 高16位 + 低16位，最后取低16位 */
    dwSum  = (dwSum >> 16) + (dwSum & 0x0000FFFF); 
    dwSum += (dwSum>>16);
    dwSum  = dwSum & 0x0000FFFF;
    return   ~((WORD16)dwSum);
}
/**********************************************************************
* 函数名称：IssSendIgmpPacket
* 功能描述：发送IGMP 报文
* 访问的表：无
* 修改的表：无
* 输入参数：VOID *ptData :报文数据头指针
            WORD32 dwLen :报文长度
            INT iPortId  :端口号
* 输出参数：无
* 返 回 值：OSS_SUCCESS 成功 其他 失败
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2012/03/28    V1.0    刘雪峰      创建
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
        /*发送IGMP报文打印*/
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
* 函数名称：IssSendIgmpReport
* 功能描述：发送IGMP report报文
* 输入参数：
  WORD16 wGroupNo：组播组号
* 输出参数：无
* 返 回 值：OSS_SUCCESS 成功 其他 失败
* 其它说明：功能主函数
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2012/03/28    V1.0    刘雪峰      创建
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

    /* 获取不到信号量 立即返回 本次就不发报告 等下次再发
    *  目前该信号量只在一个线程下使用，所以每次都可以获取到
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
    
        /* 计算IP头部校验和,先清0 */
        ptIssIgpmPkt->tIPHead.wCheckSum = 0;
        wCheckSum = IssCalcCheckSum((WORD16 *)(&ptIssIgpmPkt->tIPHead),ISS_IP_LEN>>1);
        ptIssIgpmPkt->tIPHead.wCheckSum = htons(wCheckSum);
        /* 计算IGMP头部校验和 */
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
    
        /* 计算IP头部校验和,先清0 */
        ptIssIgpmPkt->tIPHead.wCheckSum = 0;
        wCheckSum = IssCalcCheckSum((WORD16 *)(&ptIssIgpmPkt->tIPHead),ISS_IP_LEN>>1);
        ptIssIgpmPkt->tIPHead.wCheckSum = htons(wCheckSum);
        /* 计算IGMP头部校验和 */
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
* 函数名称：XOS_DbgIssIgmpPrnEnable
* 功能描述：打印Iss消息
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期        版本号     修改人          修改内容
* -----------------------------------------------
* 2009/10/09       V1.0       刘雪峰          创建
************************************************************************/
VOID XOS_DbgIssIgmpPrnEnable(VOID)
{
    g_bIssIgmpPrnEn = TRUE;    
}
/**********************************************************************
* 函数名称：XOS_DbgIssIgmpPrnDisable
* 功能描述：关闭打印Iss消息
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期        版本号     修改人          修改内容
* -----------------------------------------------
* 2009/10/09       V1.0       刘雪峰          创建
************************************************************************/
VOID XOS_DbgIssIgmpPrnDisable(VOID)
{
    g_bIssIgmpPrnEn = FALSE;
}
/**********************************************************************
* 函数名称：AddIssMgRegRecord
* 功能描述：添加组播组加入退出记录
* 访问的表：无
* 修改的表：无
* 输入参数：
    JID *ptJid     JOB ID
    WORD16 wGroupNo 组播组号
    BYTE ucRegType 注册类型
    WORD32 dwStatus  注册错误码
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期          版本号     修改人          修改内容
* -----------------------------------------------------
* 2009/07/03        V1.0       刘雪峰            创建
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
* 函数名称：InitIgmpv3
* 功能描述：初始化Igmpv3
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期        版本号     修改人          修改内容
* -----------------------------------------------
* 2009/10/09      V1.0       刘雪峰          创建
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

    /* 初始化互斥型号量，后面考虑不使用封装信号量接口--liuxf */
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
    /* 获取控制面mac地址 */    
    tRet = InterFaceGetMac(g_ucIfname,ucMac);
    if (OSS_SUCCESS != tRet)
    {
        IGMP_Print("InitIgmpv3 InterFaceGetMac Failed,tRet 0x%lx \n",tRet);
        goto ERRRET;
    }
    IGMP_Print("InitIgmpv3:Mac is %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x. \n",
                   ucMac[0], ucMac[1], ucMac[2], ucMac[3], ucMac[4], ucMac[5]);;
    memcpy(ptIssIgmpPkt->acSourceMac,ucMac,ETH_HW_ADDR_LEN);
    
    /* 获取控制面ip地址 */    
    tRet = InterFaceGetIp(g_ucIfname,&ptIssMgCtrl->dwLocalCtlIp);
    if (OSS_SUCCESS != tRet)
    {
        IGMP_Print("InitIgmpv3 InterFaceGetIp Failed,tRet 0x%lx \n",tRet);
        goto ERRRET;
    }
    ptIssIgmpPkt->tIPHead.dwSourceAddr = htonl(ptIssMgCtrl->dwLocalCtlIp); 
    IGMP_Print("InitIgmpv3: Ip = 0x%x!\n",ptIssIgmpPkt->tIPHead.dwSourceAddr);
    /* 获取控制面网口索引 */
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


