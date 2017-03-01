/*********************************************************************
* 版权所有 (C)2008, 深圳市中兴通讯股份有限公司。
*
* 文件名称： CliRegLib.c
* 文件标识：
* 内容摘要：CLI配置模块提供给应用的注册/注销函数库
* 其它说明： 无
* 当前版本： V1.0
* 作    者： 吴树高——3G平台
* 完成日期：
*
* 修改记录1：
*    修改日期：2008-7-25 08:45下午
*    版 本 号：V1.0
*    修 改 人：吴树高
*    修改内容：创建
**********************************************************************/

#include <unistd.h>
#include <string.h>
#include <assert.h>
#include "clireg.h"
#include "clireglib.h"
#include "pub_tmp.h"
#include "pub_addition.h"
#include "oam_cfg_common.h"

static BOOLEAN GetCliManagerJid(JID *ptJid)
{
    BYTE                   ucBoardStyle;
    if (!ptJid)
    {
        return FALSE;
    }

    /* 构造climanager 进程的JID*/
    memset(ptJid, 0, sizeof(JID));
    /* 如果是CMM独立单板，则构造本板的JID */
    XOS_GetBoardStyle(&ucBoardStyle);
    if (BOARD_DEPEND != ucBoardStyle)
    {
        XOS_GetSelfJID(ptJid);
        ptJid->dwJno = XOS_ConstructJNO(PROCTYPE_OAM_CLIMANAGER, 1);
    }
    /* 否则构造OMP的JID */
    else
    {
        ptJid->wModule = 1;
        ptJid->dwJno = XOS_ConstructJNO(PROCTYPE_OAM_CLIMANAGER, 1);
        ptJid->wUnit   = INVALID_UNIT;
        ptJid->ucSUnit = INVALID_SUNIT;
        ptJid->ucSubSystem = INVALID_SUBSYS;
        ptJid->ucRouteType = COMM_MASTER_SERVICE;
    }
    return TRUE;
}

/**********************************************************************
* 函数名称：BYTE  OamCliRegFunc
* 功能描述：平台应用向OAM进行注册需要进行配置的信息
* 访问的表：无
* 修改的表：无
* 输入参数：JID tJobID：          与OAM进行配置交互的JID
*           WORD32 dwDatFuncType：此JOB与OAM进行配置交互时，使用到的DAT脚本类型
* 输出参数：无
* 返 回 值：OAM_CLI_SUCCESS(0)：成功
*           OAM_CLI_FAIL(1)：   失败
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* 2008-7-24             吴树高
* ---------------------------------------------------------------------
* 2006-4-11 18:57    V1.0    wushg      创建
************************************************************************/
BYTE  OamCliRegFunc(JID tJobID, WORD32 dwDatFuncType)
{
#if 0
    WORD16        wLoop = 0;
    JID           tCliMJid;
    T_CliAppReg   *ptOamCliReg = NULL;
    BOOLEAN       bRet = FALSE;

    /*必需在使用后调用Oam_UnLockRegTable释放锁*/
    ptOamCliReg = Oam_GetRegTableAndLock();
    if (!ptOamCliReg)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,"[OamCliRegFunc] failed to create shm which contains rigister table and set lock on it!\r");
        return OAM_CLI_FAIL;
    }

    bRet = GetCliManagerJid(&tCliMJid);
    if (!bRet)
    {
        Oam_UnLockRegTable(ptOamCliReg);
        return OAM_CLI_FAIL;
    }

    for(wLoop=0; wLoop<OAM_CLI_DAT_SHM_MAX_NUM; wLoop++)
    {
        if(0 == ptOamCliReg->tCliReg[wLoop].dwDatFuncType)
        {
            ptOamCliReg->tCliReg[wLoop].dwDatFuncType = dwDatFuncType;
            ptOamCliReg->tCliReg[wLoop].ucReadFlag = OAM_CLI_REG_NOT_READED;
            ptOamCliReg->tCliReg[wLoop].ucRegType = OAM_CLI_REG_TYPE_3GPLAT;
            memset(&ptOamCliReg->tCliReg[wLoop].tAppJid, 0, sizeof(JID));
            memcpy(&ptOamCliReg->tCliReg[wLoop].tAppJid, &tJobID, sizeof(JID));
            if (tJobID.ucRouteType != COMM_BACK_SERVICE)
                ptOamCliReg->tCliReg[wLoop].tAppJid.ucRouteType = COMM_MASTER_SERVICE;
            ++(ptOamCliReg->dwRegCount);

            /* 只有主板才发 */
            if(BOARD_MASTER == XOS_GetBoardMSState())
            {
                XOS_SendAsynMsg(EV_OAMCLI_APP_REG, (BYTE*)(&(ptOamCliReg->tCliReg[wLoop])),
                                sizeof(T_CliAppData), 0, XOS_MSG_MEDIUM, (void*)&tCliMJid);
            }
            Oam_UnLockRegTable(ptOamCliReg);
            return  OAM_CLI_SUCCESS;
        }
        /* 曾经注册过 */
        else if(ptOamCliReg->tCliReg[wLoop].dwDatFuncType == dwDatFuncType)
        {
            Oam_UnLockRegTable(ptOamCliReg);
            /* 不允许出现多个JOB注册同一个DAT */
            if(ptOamCliReg->tCliReg[wLoop].tAppJid.dwJno != tJobID.dwJno)
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR, 
                    "OamCliRegFunc: A dat is only registered by one job, can not by much job!\r");
                return OAM_CLI_FAIL;
            }
            else
                return OAM_CLI_SUCCESS;   /* 重复注册认为成功 */
        }
    }

    /* 已经达到最大注册数目 */
    XOS_SysLog(OAM_CFG_LOG_ERROR, 
        "OamCliRegFunc: Number of registered dat has reached max!\r");

    Oam_UnLockRegTable(ptOamCliReg);
    return OAM_CLI_FAIL;
#endif 
    return OAM_CLI_SUCCESS;
}

/**********************************************************************
* 函数名称：BYTE  OamCliUnRegFunc
* 功能描述：平台应用向OAM进行注销进行配置过的信息
            或者OAM接收到OSS
* 访问的表：无
* 修改的表：无
* 输入参数：JID tJobID：          与OAM进行配置交互的JID
* 输出参数：无
* 返 回 值：OAM_CLI_SUCCESS(0)：成功
*           OAM_CLI_FAIL(1)：   失败
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* 2008-7-24             吴树高
* ---------------------------------------------------------------------
* 2006-4-11 18:57    V1.0    wushg      创建
************************************************************************/
BYTE  OamCliUnRegFunc(JID tJobID)
{
#if 0
    WORD16        wLoop = 0;
    JID           tCliMJid;
    T_CliAppReg   *ptOamCliReg = NULL;

    /*必需在使用后调用Oam_UnLockRegTable释放锁*/
    ptOamCliReg = Oam_GetRegTableAndLock();
    if (!ptOamCliReg)
    {
        XOS_SysLog(OAM_CFG_LOG_DEBUG,"failed to create shm which contains rigister table and set lock on it!\r");
        return OAM_CLI_FAIL;
    }

    /* 共享内存里没有数据，就认为成功 */
    if(0 == ptOamCliReg->dwRegCount)
    {
        Oam_UnLockRegTable(ptOamCliReg);
        return OAM_CLI_SUCCESS;
    }

    for(wLoop=0; wLoop<OAM_CLI_DAT_SHM_MAX_NUM; wLoop++)
    {
        /* 找到相同进程号，就删除节点 */
        if(ptOamCliReg->tCliReg[wLoop].tAppJid.dwJno == tJobID.dwJno)
        {
            /* 给CLIManager发注销消息 */
            /* 只有主板才发 */
            if(BOARD_MASTER == XOS_GetBoardMSState())
            {
                BOOLEAN bRet = GetCliManagerJid(&tCliMJid);
                if (!bRet)
                {
                    Oam_UnLockRegTable(ptOamCliReg);
                    return OAM_CLI_FAIL;
                }

                XOS_SendAsynMsg(EV_OAMCLI_APP_UNREG, (BYTE*)(&(ptOamCliReg->tCliReg[wLoop])),
                                sizeof(T_CliAppData), 0, XOS_MSG_MEDIUM, (void*)&tCliMJid);
            }

            memset(&ptOamCliReg->tCliReg[wLoop], 0, sizeof(T_CliAppReg));
            /* 不是最后一个节点 */
            if(1 != ptOamCliReg->dwRegCount - wLoop)
            {
                /* 把最后一个节点挪到此节点 */
                memcpy(&ptOamCliReg->tCliReg[wLoop].tAppJid, &ptOamCliReg->tCliReg[ptOamCliReg->dwRegCount-1].tAppJid, sizeof(JID));
                ptOamCliReg->tCliReg[wLoop].dwDatFuncType = ptOamCliReg->tCliReg[ptOamCliReg->dwRegCount-1].dwDatFuncType;

                /* 清除最后一个节点的数据 */
                memset(&ptOamCliReg->tCliReg[ptOamCliReg->dwRegCount-1], 0, sizeof(T_CliAppData));

            }

            --(ptOamCliReg->dwRegCount);
            Oam_UnLockRegTable(ptOamCliReg);
            return OAM_CLI_SUCCESS;
        }
    }

    Oam_UnLockRegTable(ptOamCliReg);
    return OAM_CLI_FAIL;
#endif
    return OAM_CLI_SUCCESS;
}

/**********************************************************************
* 函数名称：BYTE  OamCliRegFuncEx
* 功能描述：非平台的应用/业务进程向OAM进行注册需要进行配置的信息
* 访问的表：无
* 修改的表：无
* 输入参数：JID tJobID：          与OAM进行配置交互的JID
*                            WORD32 dwNetId：    LOMP所在的网元标示
* 输出参数：无
* 返 回 值：OAM_CLI_SUCCESS(0)：成功
*           OAM_CLI_FAIL(1)：   失败
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* 2008-7-24             吴树高
* ---------------------------------------------------------------------
* 2006-4-11 18:57    V1.0    wushg      创建
************************************************************************/
BYTE  OamCliRegFuncEx(JID tJobID, WORD32 dwNetId)
{
    WORD16        wLoop = 0;
    JID           tCliMJid;
    T_CliAppAgtRegEx *ptOamCliReg = NULL;
    WORD16        wMaxRegRecord = 0;
    BOOLEAN       bRet  = FALSE;
    BYTE        ucRetValue = OAM_CLI_SUCCESS;

    /*获取climanager的jid*/
    bRet = GetCliManagerJid(&tCliMJid);
    if (!bRet && (BOARD_MASTER == XOS_GetBoardMSState()))
    {
        return OAM_CLI_FAIL;
    }

    /*OMP和其他单板上所支持的业务注册数量不同*/
    if (tJobID.wModule == tCliMJid.wModule)
    {
        wMaxRegRecord = OAM_CLI_APP_MGT_SHM_MAX_NUM;
    }
    else
    {
        wMaxRegRecord = OAM_CLI_APP_AGT_SHM_MAX_NUM;
    }
    
    /*必需在使用后调用Oam_UnLockRegTable释放锁*/
    ptOamCliReg = Oam_GetRegExTableAndLock();
    if (!ptOamCliReg)
    {
        XOS_SysLog(OAM_CFG_LOG_DEBUG,"failed to create shm which contains rigister table and set lock on it!\r");
        return OAM_CLI_FAIL;
    }

    for(wLoop=0; wLoop<wMaxRegRecord; wLoop++)
    {    
        /* 曾经注册过 */
        if(ptOamCliReg->tCliReg[wLoop].dwNetId == dwNetId)
        {
            /* 不允许出现多个JOB注册同一个网元类型 */
            if(ptOamCliReg->tCliReg[wLoop].tAppJid.dwJno != tJobID.dwJno)
            {
                ucRetValue = OAM_CLI_FAIL;
            }
            else
            {
                ucRetValue = OAM_CLI_SUCCESS; /* 重复注册认为成功 */
            }

            break;
        }
        /* 找到第一个网元功能类型为0，认为这个节点没有注册过 */
        else if(0 == ptOamCliReg->tCliReg[wLoop].dwNetId)
        {

            ptOamCliReg->tCliReg[wLoop].dwNetId = dwNetId;
            memcpy(&ptOamCliReg->tCliReg[wLoop].tAppJid, &tJobID, sizeof(JID));
            if (tJobID.ucRouteType != COMM_BACK_SERVICE)
                ptOamCliReg->tCliReg[wLoop].tAppJid.ucRouteType = COMM_MASTER_SERVICE;
            ++(ptOamCliReg->dwRegCount);

            /* 给CLIManager发消息 */
            /* 只有主板才发 */
            if(BOARD_MASTER == XOS_GetBoardMSState())
            {
                XOS_SendAsynMsg(EV_OAMCLI_APP_REGEX, (BYTE*)(&(ptOamCliReg->tCliReg[wLoop])),
                                sizeof(ptOamCliReg->tCliReg[wLoop]), 0, XOS_MSG_MEDIUM,(void*)&tCliMJid);
            }
            
            ucRetValue = OAM_CLI_SUCCESS; 
            break;
        }
    }

    Oam_UnLockRegExTable(ptOamCliReg);
    return ucRetValue;

}

/**************************************************************************
* 函数名称：T_CliAppReg* GetRegTableAndLock(VOID)
* 功能描述：获取非平台共享内存中注册表地址并加锁
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：    NULL -操作失败
                              非NULL -非平台共享内存地址
* 其它说明：需要和Oam_UnLockRegExTable 一起使用
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/03/17    V1.0    齐龙兆      创建
**************************************************************************/
T_CliAppAgtRegEx* Oam_GetRegExTableAndLock(void)
{
 //   DWORD     dwRet = 0;
  //  WORD32 dwSemid = 0;
    /*CRDCM00034213 防止共享内存被重复映射yinhao@20090324*/
    static T_CliAppAgtRegEx *ptOamCliReg = NULL;

#if 0
    /* 创建进程互斥信号量，用于JOB信息访问 */
    dwSemid = XOS_CreateProcessSem(OAM_SEM_CLI_REGEX, 1, MUTEX_STYLE);
    if(dwSemid == XOS_INVALID_VOSSEM)
    {
        XOS_SysLog(OAM_CFG_LOG_DEBUG, "Oam_GetRegExTableAndLock: failed to CreateNameSem , return %u", dwSemid);
        return NULL;
    }

    /*CRDCM00034213 防止共享内存被重复映射yinhao@20090324*/
    if (!ptOamCliReg)
    {
        /* 创建保存JOB信息的共享内存 */
        dwRet = XOS_CreateShmByIndex(OAM_SHM_CLI_REGEX,
                              sizeof(*ptOamCliReg),
                              XOS_RDWR | XOS_CREAT,
                              (BYTE**)&ptOamCliReg);

        if ((XOS_SUCCESS != dwRet) && (XOS_VOS_SHM_EXIST != dwRet))
        {
            XOS_SysLog(OAM_CFG_LOG_DEBUG, "Oam_GetRegExTableAndLock: failed to c CreateShm , return %d",dwRet);
            return NULL;
        }    

        /*
        检查共享内存是否需要更新:
        如果管理进程是新的，就更新
        */
        #ifdef OS_VXWORKS
        {
            /*vxworks 可以根据返回值判断是否要初始化*/
            if (XOS_SUCCESS == dwRet)	
            {
                XOS_SysLog(OAM_CFG_LOG_DEBUG,
                       "clear shm which contains register table");

                memset(ptOamCliReg, 0, sizeof(T_CliAppAgtRegEx));
            }
            else
            {
                XOS_SysLog(OAM_CFG_LOG_DEBUG, "GetRegTableAndLock: shm does not need clear");
            }
        }
        #else
        {
            pid_t platpid = getppid();
            if (platpid != ptOamCliReg->apppid)
            {
                XOS_SysLog(OAM_CFG_LOG_DEBUG,
                           "clear shm which contains register table, oldapppid=%u,newapppid=%u",
                           ptOamCliReg->apppid,
                           platpid);
                memset(ptOamCliReg, 0, sizeof(T_CliAppAgtRegEx));
                ptOamCliReg->apppid = platpid;
            }
            else
            {
                XOS_SysLog(OAM_CFG_LOG_DEBUG, "Oam_GetRegExTableAndLock: shm does not need clear");
            }
        }
        #endif
    }
    ptOamCliReg->dwSemid = dwSemid;
    if (!XOS_ProcessSemP(ptOamCliReg->dwSemid, WAIT_FOREVER))
    {
        XOS_SysLog(OAM_CFG_LOG_DEBUG, "Oam_GetRegExTableAndLock: failed to XOS_ProcessSemP ");
        return NULL;
    }
#endif
    return ptOamCliReg;
}

/**************************************************************************
* 函数名称：VOID Oam_UnLockRegExTable(T_CliAppAgtRegEx *ptOamCliReg )
* 功能描述：去除对非平台共享内存中注册表地址的锁定
* 访问的表：无
* 修改的表：无
* 输入参数：ptOamCliReg:注册表地址
* 输出参数：无
* 返 回 值：    无
* 其它说明：需要和Oam_GetRegExTableAndLock(VOID)一起使用
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/03/17    V1.0    齐龙兆      创建
**************************************************************************/
BOOLEAN Oam_UnLockRegExTable(T_CliAppAgtRegEx *ptOamCliReg )
{
    assert(ptOamCliReg != NULL);
    if (!ptOamCliReg)
    {
        return FALSE;
    }

    return XOS_ProcessSemV(ptOamCliReg->dwSemid);
}

/**************************************************************************
* 函数名称：T_CliAppReg* GetRegTableAndLock(VOID)
* 功能描述：获取共享内存中注册表地址并加锁
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：    NULL -操作失败
                              非NULL -共享内存地址
* 其它说明：需要和Oam_UnLockRegTable 一起使用
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2008/10/20    V1.0    殷浩      创建
**************************************************************************/
T_CliAppReg* Oam_GetRegTableAndLock(void)
{
 //   DWORD     dwRet = 0;
//    WORD32 dwSemid = 0;
    /*CRDCM00034213 防止共享内存被重复映射yinhao@20090324*/
    static T_CliAppReg   *ptOamCliReg = NULL;
#if 0
    /* 创建进程互斥信号量，用于JOB信息访问 */
    dwSemid = XOS_CreateProcessSem(OAM_SEM_CLI_REG, 1, MUTEX_STYLE);
    if(dwSemid == XOS_INVALID_VOSSEM)
    {
        XOS_SysLog(OAM_CFG_LOG_DEBUG, "GetRegTableAndLock: failed to CreateNameSem , return %u", dwSemid);
        return NULL;
    }

    /*CRDCM00034213 防止共享内存被重复映射yinhao@20090324*/
    if (!ptOamCliReg)
    {
        /* 创建保存JOB信息的共享内存 */
        dwRet = XOS_CreateShmByIndex(OAM_SHM_CLI_REG,
                              sizeof(T_CliAppReg),
                              XOS_RDWR | XOS_CREAT,
                              (BYTE**)&ptOamCliReg);

        if ((XOS_SUCCESS != dwRet) && (XOS_VOS_SHM_EXIST != dwRet))
        {
            XOS_SysLog(OAM_CFG_LOG_DEBUG, "GetRegTableAndLock: failed to c CreateShm , return %d",dwRet);
            return NULL;
        }

        /*
        检查共享内存是否需要更新:
        如果管理进程是新的，就更新
        */
        #ifdef OS_VXWORKS
        {
            /*vxworks 可以根据返回值判断是否要初始化*/
            if (XOS_SUCCESS == dwRet)	
            {
                XOS_SysLog(OAM_CFG_LOG_DEBUG,
                       "clear shm which contains register table");

                memset(ptOamCliReg, 0, sizeof(T_CliAppReg));
            }
            else
            {
                XOS_SysLog(OAM_CFG_LOG_DEBUG, "GetRegTableAndLock: shm does not need clear");
            }
        }
        #else
        {
            pid_t platpid = getppid(); 
            if (platpid != ptOamCliReg->platpid)
            {
                XOS_SysLog(OAM_CFG_LOG_DEBUG,
                           "clear shm which contains register table, oldplatpid=%u,newplatpid=%u",
                           ptOamCliReg->platpid,
                           platpid);

                memset(ptOamCliReg, 0, sizeof(T_CliAppReg));
                ptOamCliReg->platpid = platpid;
            }
            else
            {
                XOS_SysLog(OAM_CFG_LOG_DEBUG, "GetRegTableAndLock: shm does not need clear");
            }
        }
        #endif
   
    }


    ptOamCliReg->dwSemid = dwSemid;
    if (!XOS_ProcessSemP(ptOamCliReg->dwSemid, WAIT_FOREVER))
    {
        XOS_SysLog(OAM_CFG_LOG_DEBUG, "GetRegTableAndLock: failed to XOS_ProcessSemP ");
        return NULL;
    }
#endif
    return ptOamCliReg;
}

/**************************************************************************
* 函数名称：VOID UnLockRegTable(VOID)
* 功能描述：去除对共享内存中注册表地址的锁定
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：    无
* 其它说明：需要和Oam_GetRegTableAndLock(VOID)一起使用
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2008/10/20    V1.0    殷浩      创建
**************************************************************************/
BOOLEAN Oam_UnLockRegTable(T_CliAppReg *ptOamCliReg )
{
    assert(ptOamCliReg != NULL);
    if (!ptOamCliReg)
    {
        return FALSE;
    }

    return XOS_ProcessSemV(ptOamCliReg->dwSemid);
}

#if 0

/*
下面两个函数注册库使用，manager也用
为了减小库大小，放在这里定义，在clireg.h中声明
*/

static CHAR *OAM_RTrim(CHAR *pStr)
{
    CHAR *pRet = pStr;
    WORD16 wLen = 0;
    SWORD16 i = 0;
    if (NULL == pStr)/*输入的空串，直接返回*/
    {
        return NULL;
    }
    wLen = strlen(pStr);
    for(i = wLen - 1; i >= 0; i--)
    {
        if((*(pStr + i) == ' ') ||
                (*(pStr + i) == '\t') ||
                (*(pStr + i) == '\r') ||
                (*(pStr + i) == '\n'))
        {
            *(pRet + i) = '\0';
        }
        else
        {
            break;
        }
    }

    return pStr;
}

static CHAR *OAM_LTrim(CHAR *pStr )
{
    CHAR *pRet = pStr;
    WORD16 wLen = 0;
    SWORD16 i = 0;
    if (NULL == pStr)/*输入的空串，直接返回*/
    {
        return NULL;
    }
    wLen = strlen(pStr);
    for(i = 0; i < wLen; i++)
    {
        if((*(pStr + i) == ' ') ||
                (*(pStr + i) == '\t') ||
                (*(pStr + i) == '\r') ||
                (*(pStr + i) == '\n'))
        {
            pRet++;
        }
        else
        {
            break;
        }
    }

    return pRet;
}

static int StrCmpNoCase(CHAR * pInStr, CHAR * pPatternStr)
{
    CHAR * pInChar = pInStr;
    CHAR * pPatternChar = pPatternStr;
    CHAR cLowerIn, cLowerPattern;
    int nLenInStr = strlen(pInStr);
    int nLenPatternStr = strlen(pPatternStr);
    int i = 1;

    while (i <= nLenInStr && i <= nLenPatternStr)
    {
        cLowerIn = ((*pInChar) >= 'A' && (*pInChar) <= 'Z') ? ((*pInChar) + 32) : (*pInChar);
        cLowerPattern = ((*pPatternChar) >= 'A' && (*pPatternChar) <= 'Z') ? ((*pPatternChar) + 32) : (*pPatternChar);
        if (cLowerIn == cLowerPattern)
        {
            pInChar++;
            pPatternChar++;
        }
        else
            return (cLowerIn < cLowerPattern) ? - 1 : 1;
        i++;
    }

    if (nLenInStr == nLenPatternStr)
        return 0;
    else
        return (nLenInStr < nLenPatternStr) ? - 1 : 1;
}


/*暂不支持根据名称访问参数*/
/**********************************************************************
* 函数名称： Oam_GetParaByName
* 功能描述：根据名称从MSG_COMM_OAM结构消息里面获取参数信息
* 访问的表：无
* 修改的表：无
* 输入参数：MSG_COMM_OAM *pMsg : 与OAM进行配置交互的消息
*                            CHAR *pucParaName : 参数名称，不区分大小写
* 输出参数：无
* 返 回 值：OPR_DATA结构指针：成功
*                         NULL：   失败
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* 2008-8-21           殷浩
* ---------------------------------------------------------------------
* 2008-8-21 18:57    V1.0   殷浩    创建
************************************************************************/
OPR_DATA *Oam_GetParaByName(MSG_COMM_OAM *pMsg, CHAR *pucParaName)
{
    INT i = 0;
    CHAR *pucName1 = NULL, *pucName2 = NULL;
    OPR_DATA *pRtnOprData = NULL;

    if (!pMsg || !pucParaName)
    {
        return NULL;
    }

    pRtnOprData = (OPR_DATA *)pMsg->Data;

    for (i = 0; i<pMsg->Number; i++)
    {
        pucName1 = OAM_RTrim(OAM_LTrim(pucParaName));
        pucName2 = OAM_RTrim(OAM_LTrim(pRtnOprData->sName));
        if (0 == StrCmpNoCase(pucName1, pucName2))
        {
            break;
        }

        pRtnOprData = (OPR_DATA *)((BYTE *)pRtnOprData->Data + pRtnOprData->Len);
        if (!pRtnOprData)
        {
            return NULL;
        }
    }

    if ( i == pMsg->Number)
    {
        return NULL;
    }
    else
    {
        return pRtnOprData;
    }

}
#endif 

/**********************************************************************
* 函数名称：BYTE  OamCliRegFuncSpecial
* 功能描述：SBC业务向OAM进行注册需要进行配置的信息
* 访问的表：无
* 修改的表：无
* 输入参数：JID tJobID：          与OAM进行配置交互的JID
*           WORD32 dwDatFuncType：此JOB与OAM进行配置交互时，使用到的DAT脚本类型
* 输出参数：无
* 返 回 值：OAM_CLI_SUCCESS(0)：成功
*           OAM_CLI_FAIL(1)：   失败
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
*
* ---------------------------------------------------------------------
* 2010-8-30 11:16    V1.0    fuls      创建
************************************************************************/
BYTE  OamCliRegFuncSpecial(JID tJobID, WORD32 dwDatFuncType)
{
#if 0
    WORD16        wLoop = 0;
    JID           tCliMJid;
    T_CliAppReg   *ptOamCliReg = NULL;
    BOOLEAN       bRet = FALSE;

    /*必需在使用后调用Oam_UnLockRegTable释放锁*/
    ptOamCliReg = Oam_GetRegTableAndLock();
    if (NULL == ptOamCliReg)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
            "######[OamCliRegFuncSpecial]: failed to create shm which contains rigister table and set lock on it!\r");
        return OAM_CLI_FAIL;
    }

    bRet = GetCliManagerJid(&tCliMJid);
    if (!bRet)
    {
        Oam_UnLockRegTable(ptOamCliReg);
        return OAM_CLI_FAIL;
    }

    /* 加入共享内存 */
    for(wLoop = 0; wLoop < OAM_CLI_DAT_SHM_MAX_NUM; wLoop++)
    {
        if(0 == ptOamCliReg->tCliReg[wLoop].dwDatFuncType)
        {
            ptOamCliReg->tCliReg[wLoop].dwDatFuncType = dwDatFuncType;
            ptOamCliReg->tCliReg[wLoop].ucReadFlag = OAM_CLI_REG_NOT_READED;
            ptOamCliReg->tCliReg[wLoop].ucRegType = OAM_CLI_REG_TYPE_SPECIAL;
            memset(&ptOamCliReg->tCliReg[wLoop].tAppJid, 0, sizeof(JID));
            memcpy(&ptOamCliReg->tCliReg[wLoop].tAppJid, &tJobID, sizeof(JID));
            if (tJobID.ucRouteType != COMM_BACK_SERVICE)
                ptOamCliReg->tCliReg[wLoop].tAppJid.ucRouteType = COMM_MASTER_SERVICE;
            ++(ptOamCliReg->dwRegCount);

            /* 只有主板才发 */
            if(BOARD_MASTER == XOS_GetBoardMSState())
            {
                XOS_SendAsynMsg(EV_OAMCLI_SPECIAL_REG, (BYTE*)(&(ptOamCliReg->tCliReg[wLoop])),
                                sizeof(T_CliAppData), 0, XOS_MSG_MEDIUM, (void*)&tCliMJid);
            }
            Oam_UnLockRegTable(ptOamCliReg);
            return  OAM_CLI_SUCCESS;
        }
        /* 曾经注册过 */
        else if(ptOamCliReg->tCliReg[wLoop].dwDatFuncType == dwDatFuncType)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR,
                "######[OamCliRegFuncSpecial]: The Function Type of Dat has exists!\n");

            Oam_UnLockRegTable(ptOamCliReg);
            /* 不允许出现多个JOB注册同一个DAT */
            if(ptOamCliReg->tCliReg[wLoop].tAppJid.dwJno != tJobID.dwJno)
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR,
                "######[OamCliRegFuncSpecial]: This dat can not be registered with a few JOB!\n");
                return OAM_CLI_FAIL;
            }
            else
            {
                return OAM_CLI_SUCCESS;   /* 重复注册认为成功 */
            }
        }
    }

    /* 已经达到最大注册数目 */
    XOS_SysLog(OAM_CFG_LOG_ERROR, 
        "######[OamCliRegFuncSpecial]: Number of registered dat has reached max!\r");

    Oam_UnLockRegTable(ptOamCliReg);
    return OAM_CLI_FAIL;
#endif
    return OAM_CLI_SUCCESS;
}
