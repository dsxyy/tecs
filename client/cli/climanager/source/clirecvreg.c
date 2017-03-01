/*********************************************************************
* 版权所有 (C)2008, 深圳市中兴通讯股份有限公司。
*
* 文件名称： CliRecvReg.c
* 文件标识：
* 内容摘要：接收各单板注册/注销信息；接收各单板Agent注册信息的同步
*                           此文件功能属于CLIManager
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

#include "includes.h"
#include "pub_oam_cfg_event.h"
#include "clireg.h"
#include "clireglib.h"
#include "clirecvreg.h"

static void  OamCliScanShm(void);
static BOOLEAN LompCliAppRegInfoIsExist(WORD32 dwNetId);
static void UpdateCliAppRegInfo(WORD32 dwNetId,JID *ptJid);

extern void RegDatToVmm(void);
extern void ReqAllCfgVerInfo(void);
extern void Oam_CfgStopAllLinkExcutingCmd(void);
/*extern VOID OamCfgRemoveAllDat(VOID);*/

/**********************************************************************
* 函数名称：VOID  OamCliAgingShm
* 功能描述：Manager定时扫描非OMP的注册数据，把长时间
*                          没有同步的信息老化掉
* 访问的表：无
* 修改的表：无
* 输入参数：T_OAM_INTERPRET_VAR *ptVar：私有数据区
* 输出参数：无
* 返 回 值：OAM_CLI_SUCCESS(0)：成功
*           OAM_CLI_FAIL(1)：   失败
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* 2008-7-28             吴树高
* ---------------------------------------------------------------------
* 2008-7-28             吴树高      创建
************************************************************************/
void  OamCliAgingShm(T_OAM_INTERPRET_VAR *ptVar)
{
    /* 等业务那边接口确认后再做老化吧 */
    return;
}



/**********************************************************************
* 函数名称：VOID RecvAgtReg
* 功能描述：接收Agent注册信息，平台/T8000应用进行注册
* 访问的表：无
* 修改的表：无
* 输入参数：T_AlmAgtVars *ptVar：私有数据区
            LPVOID pMsgPara:     消息数据
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-7-24             吴树高      创建
************************************************************************/
void RecvAgtReg(T_OAM_INTERPRET_VAR *ptVar, LPVOID pMsgPara, BOOLEAN bIsSameEndian)
{
    WORD32          dwLoop = 0;
    WORD16          wRegDatCnt = 0;
    WORD32          *pdwRegNum = NULL;
    JID             tSenderJid;
    T_CliAppReg     *ptPriData = NULL;
    T_CliAppData *ptRecvAppData,tMsgRecvData;
//    DWORD dwVer = 0;
    WORD16 wLen = 0;
    WORD32          dwRegedCntOfShm = 0;
    
    /* 如果当前正在向VMM请求版本，则不处理注册消息。等后面扫描注册表时再加载 */
    if ((g_ptOamIntVar->ucVerInfoReqCnt > 0) || 
        (g_ptOamIntVar->ucVerNotifyRegReqCnt > 0) ||
        ((g_ptOamIntVar->ucVerInfoReqCnt == 0) && (g_ptOamIntVar->wGetCfgVerInfoTimer != INVALID_TIMER_ID)) ||
        ((g_ptOamIntVar->ucVerNotifyRegReqCnt == 0) && (g_ptOamIntVar->wRegOamsNotifyTimer != INVALID_TIMER_ID)))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "######[RecvAgtReg]: now is requesting dat ver...\n");
        return;
    }

    /******************** 消息兼容 开始 ********************/
    /* 消息结构默认初始化 */
    memset(&tMsgRecvData, 0xFF, sizeof(tMsgRecvData));

    /* 函数堆栈中申请消息数据结构，避免动态内存申请/释放 */
    ptRecvAppData  = &tMsgRecvData;

    /* 调用消息版本兼容转换函数 */
   // XOS_GetMsgAppVer((BYTE *)&dwVer);
    XOS_GetMsgDataLen(PROCTYPE_OAM_CLIMANAGER, (WORD16 *)&wLen);
 //   INVERT_MSG_T_CliAppData((BYTE **)&ptRecvAppData,pMsgPara,dwVer,wLen);
    /******************** 消息兼容 结束 ********************/
    
    /*字节序转换*/
    if (!bIsSameEndian)
    {
        ptRecvAppData->tAppJid.dwJno = XOS_InvertWord32(ptRecvAppData->tAppJid.dwJno);
        ptRecvAppData->tAppJid.dwDevId = XOS_InvertWord32(ptRecvAppData->tAppJid.dwDevId);
        ptRecvAppData->tAppJid.wModule = XOS_InvertWord16(ptRecvAppData->tAppJid.wModule);
        ptRecvAppData->tAppJid.wUnit = XOS_InvertWord16(ptRecvAppData->tAppJid.wUnit);
        ptRecvAppData->dwDatFuncType = XOS_InvertWord32(ptRecvAppData->dwDatFuncType);
    }

    /* 注意数据区结构体初使化 */
    memset(&tSenderJid, 0, sizeof(JID));
    XOS_Sender(&tSenderJid);

    /* OMP上的应用进程注册信息*/
    if (IsSameLogicAddr(&(g_ptOamIntVar->jidSelf), &(ptRecvAppData->tAppJid)))
    {
        /* OMP,只需要扫描注册表就可以了*/
        OamCliScanShm();
        return;
    }
    /* 目前所知应该只有RPU上有业务注册进程，先默认RPU吧 */
    else
    {
        pdwRegNum  = &ptVar->tPlatMpCliAppInfo.dwRegCount;
        ptPriData = &ptVar->tPlatMpCliAppInfo;
    }

    if (!OamRegCntOfShm(&dwRegedCntOfShm))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, 
            "RecvAgtReg: Exec func OamRegCntOfShm failed!\r");
        return;
    }

    /* 保存到数据区 */
    for(dwLoop=0; dwLoop<*pdwRegNum; dwLoop++)
    {
        if(dwLoop>=OAM_CLI_DAT_SHM_MAX_NUM)
            return;

        /* 直接认为已经注册过了 */
        if(ptRecvAppData->dwDatFuncType == ptPriData->tCliReg[dwLoop].dwDatFuncType)
        {
            /* added by fls on 2009-02-27 (CRDCR00481329 rpu重启之后,经常出现在线配置进入不了) */
            for (wRegDatCnt = 0; wRegDatCnt < g_wRegDatCount; wRegDatCnt++)
            {
                if (g_tDatLink[wRegDatCnt].datIndex.tJid.dwJno == ptRecvAppData->tAppJid.dwJno)
                {
                    memcpy(&g_tDatLink[wRegDatCnt].datIndex.tJid, &ptRecvAppData->tAppJid, sizeof(JID));
                    if (ptRecvAppData->dwDatFuncType == FUNC_SCRIPT_CLIS_BRS)
                        memcpy(&g_ptOamIntVar->jidProtocol, &ptRecvAppData->tAppJid, sizeof(JID));
                        
                    break;
                }
            }
            /* end of add */
            return;
        }
    }/*yinhao test开发 把for循环边界移到这里，否则第一次无法注册*/
	
    /* 出现错误了 */
    if(dwLoop != *pdwRegNum)
        return;

    /* 共享内存中和全局数据区中的已注册dat总数据不能超过最大值 */
    if ((dwRegedCntOfShm + (*pdwRegNum)) >= OAM_CLI_DAT_SHM_MAX_NUM)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, 
            "RecvAgtReg: Total of registered dats has reached max!\r");
        return;
    }

    /* 需要更新命令树上的JID */
    if(OAM_CLI_SUCCESS != OAM_CFGDatRegister(ptRecvAppData->tAppJid, ptRecvAppData->dwDatFuncType))
        return;

    /* 放到最后一个节点 */
    memcpy(&ptPriData->tCliReg[dwLoop].tAppJid, &ptRecvAppData->tAppJid, sizeof(JID));
    ptPriData->tCliReg[dwLoop].dwDatFuncType = ptRecvAppData->dwDatFuncType;
    ptPriData->tCliReg[dwLoop].ucReadFlag = OAM_CLI_REG_READED;
    (*pdwRegNum)++;

    return;
}

void RecvAgtRegSpecial(T_OAM_INTERPRET_VAR *ptVar, LPVOID pMsgPara, BOOLEAN bIsSameEndian)
{
    T_CliAppData *ptRecvAppData,tMsgRecvData;
//    DWORD dwVer = 0;
    WORD16 wLen = 0;
    WORD32       *pdwRegNum = NULL;
    T_CliAppReg  *ptPriData = NULL;
    WORD32       dwLoop = 0;
    WORD16       wRegDatCnt = 0;
    WORD32          dwRegedCntOfShm = 0;
    
    XOS_SysLog(OAM_CFG_LOG_NOTICE, "[RecvAgtRegSpecial] Recv msg: EV_OAMCLI_SPECIAL_REG......");

    /******************** 消息兼容 开始 ********************/
    /* 消息结构默认初始化 */
    memset(&tMsgRecvData, 0xFF, sizeof(tMsgRecvData));

    /* 函数堆栈中申请消息数据结构，避免动态内存申请/释放 */
    ptRecvAppData  = &tMsgRecvData;

    /* 调用消息版本兼容转换函数 */
  //  XOS_GetMsgAppVer((BYTE *)&dwVer);
    XOS_GetMsgDataLen(PROCTYPE_OAM_CLIMANAGER, (WORD16 *)&wLen);
    //INVERT_MSG_T_CliAppData((BYTE **)&ptRecvAppData,pMsgPara,dwVer,wLen);
    /******************** 消息兼容 结束 ********************/

    /*字节序转换*/
    if (!bIsSameEndian)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[RecvAgtRegSpecial] Need to invert......");
        ptRecvAppData->tAppJid.dwJno = XOS_InvertWord32(ptRecvAppData->tAppJid.dwJno);
        ptRecvAppData->tAppJid.dwDevId = XOS_InvertWord32(ptRecvAppData->tAppJid.dwDevId);
        ptRecvAppData->tAppJid.wModule = XOS_InvertWord16(ptRecvAppData->tAppJid.wModule);
        ptRecvAppData->tAppJid.wUnit = XOS_InvertWord16(ptRecvAppData->tAppJid.wUnit);
        ptRecvAppData->dwDatFuncType = XOS_InvertWord32(ptRecvAppData->dwDatFuncType);
    }

    /* 如果是非OMP注册，则需要加入到全局数据区里。如果是OMP注册，则已经加入到共享内存中 */
    if (!IsSameLogicAddr(&(g_ptOamIntVar->jidSelf), &(ptRecvAppData->tAppJid)))
    {
        pdwRegNum  = &ptVar->tPlatMpCliAppInfo.dwRegCount;
        ptPriData = &ptVar->tPlatMpCliAppInfo;

        if (!OamRegCntOfShm(&dwRegedCntOfShm))
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, 
                "RecvAgtRegSpecial: Exec func OamRegCntOfShm failed!\r");
            return;
        }

        /* 保存到数据区 */
        for(dwLoop=0; dwLoop<*pdwRegNum; dwLoop++)
        {
            if(dwLoop>=OAM_CLI_DAT_SHM_MAX_NUM)
                return;

            /* 直接认为已经注册过了 */
            if(ptRecvAppData->dwDatFuncType == ptPriData->tCliReg[dwLoop].dwDatFuncType)
            {
                for (wRegDatCnt = 0; wRegDatCnt < g_wRegDatCount; wRegDatCnt++)
                {
                    if (g_tDatLink[wRegDatCnt].datIndex.tJid.dwJno == ptRecvAppData->tAppJid.dwJno)
                    {
                        memcpy(&g_tDatLink[wRegDatCnt].datIndex.tJid, &ptRecvAppData->tAppJid, sizeof(JID));
                        break;
                    }
                }
                return;
            }
        }

        /* 出现错误了 */
        if(dwLoop != *pdwRegNum)
            return;

        /* 共享内存中和全局数据区中的已注册dat总数据不能超过最大值 */
        if ((dwRegedCntOfShm + (*pdwRegNum)) >= OAM_CLI_DAT_SHM_MAX_NUM)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, 
                "RecvAgtRegSpecial: Total of registered dats has reached max!\r");
            return;
        }

        /* 放到最后一个节点 */
        memcpy(&ptPriData->tCliReg[dwLoop].tAppJid, &ptRecvAppData->tAppJid, sizeof(JID));
        ptPriData->tCliReg[dwLoop].dwDatFuncType = ptRecvAppData->dwDatFuncType;
        ptPriData->tCliReg[dwLoop].ucReadFlag = OAM_CLI_REG_NOT_READED;
        ptPriData->tCliReg[dwLoop].ucRegType = OAM_CLI_REG_TYPE_SPECIAL;
        (*pdwRegNum)++;
        
    }
#if 0
    /* 停止所有正在执行的命令，断开所有连接 */
    Oam_CfgStopAllLinkExcutingCmd();

    /* 清除所有注册的dat内存 */
    OAM_CFGRemoveAllDat();
    /*如果是刀片环境，清除版本 信息，从vmm重新获取*/
    if (!Oam_CfgIfCurVersionIsPC())
    {
        g_ptOamIntVar->wDatSum = 0;
    }
    
    RegDatToVmm();
    ReqAllCfgVerInfo();
#endif    
    return;
}

/**********************************************************************
* 函数名称：VOID  OamCliScanShm
* 功能描述：Manager定时扫描OMP的共享内存，判断是否有DAT
*                          的FUNTYPE不为0，而flag为0时，重新把JID读到命令树
* 访问的表：无
* 修改的表：无
* 输入参数：T_OAM_INTERPRET_VAR *ptVar：私有数据区
* 输出参数：无
* 返 回 值：OAM_CLI_SUCCESS(0)：成功
*           OAM_CLI_FAIL(1)：   失败
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* 2008-7-28             吴树高
* ---------------------------------------------------------------------
* 2008-7-28             吴树高      创建
************************************************************************/
void  OamCliScanShm()
{
#if 0
    WORD16        wLoop = 0;
    T_CliAppReg   *ptOamCliReg = NULL;

    XOS_SysLog(OAM_CFG_LOG_NOTICE, "######[OamCliScanShm]: begin to scan register table in shm...");

    /*必需在使用后调用Oam_UnLockRegTable释放锁*/
    ptOamCliReg = Oam_GetRegTableAndLock();
    if (!ptOamCliReg)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "######[OamCliScanShm]: failed to create shm which contains rigister table and set lock on it!\r");
        return;
    }

    for(wLoop=0; wLoop<OAM_CLI_DAT_SHM_MAX_NUM; wLoop++)
    {
        if(0 == ptOamCliReg->tCliReg[wLoop].dwDatFuncType)
            break;

        /* 读取标识为0，认为这个节点没有注册过 */
        if(OAM_CLI_REG_NOT_READED == ptOamCliReg->tCliReg[wLoop].ucReadFlag)
        {
            if(OAM_CLI_SUCCESS != OAM_CFGDatRegister(ptOamCliReg->tCliReg[wLoop].tAppJid, ptOamCliReg->tCliReg[wLoop].dwDatFuncType))
            	{
            	    continue;
            	}

            ptOamCliReg->tCliReg[wLoop].ucReadFlag = OAM_CLI_REG_READED;
        }
    }

    /* 等所有都加载完成之后，再看看有没有SBCdat没有加载成功，如果有则需要向VMM请求 */
    for(wLoop = 0; wLoop < OAM_CLI_DAT_SHM_MAX_NUM; wLoop++)
    {
        if(0 == ptOamCliReg->tCliReg[wLoop].dwDatFuncType)
            break;
        
        if((OAM_CLI_REG_NOT_READED == ptOamCliReg->tCliReg[wLoop].ucReadFlag) &&
            (OAM_CLI_REG_TYPE_SPECIAL == ptOamCliReg->tCliReg[wLoop].ucRegType))
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, "######[OamCliScanShm]: Has special dat need to request to VMM...\r");
            
            /* 停止所有正在执行的命令，断开所有连接 */
            Oam_CfgStopAllLinkExcutingCmd();

            /* 清除所有注册的dat内存 */
            OAM_CFGRemoveAllDat();
            /*如果是刀片环境，清除版本 信息，从vmm重新获取*/
            if (!Oam_CfgIfCurVersionIsPC())
            {
                g_ptOamIntVar->wDatSum = 0;
            }
            
            RegDatToVmm();
            ReqAllCfgVerInfo();
            break;
        }
    }

    if (!Oam_UnLockRegTable(ptOamCliReg))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "######[OamCliScanShm]: failed to unlock shm which contains register table ");
    }
#endif	
    return;
}

/**********************************************************************
* 函数名称：VOID  Oam_CfgScanRegTable
* 功能描述：检查注册表，看是否有没有注册的，
                                 如果有，进行注册
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
 * 返 回 值： 无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2009-3-5           殷浩      创建
************************************************************************/
void Oam_CfgScanRegTable()
{
    T_CliAppReg *ptOtherMpRegInfo = NULL;  
    WORD16 wLoop = 0;

    if (NULL == g_ptOamIntVar)
    {
        return;
    }

     /*检查共享内存中本模块的注册表*/
    OamCliScanShm();

    /*检查数据区中其他模块的注册表*/
    ptOtherMpRegInfo = &(g_ptOamIntVar->tPlatMpCliAppInfo);
    for(wLoop=0; wLoop < ptOtherMpRegInfo->dwRegCount; wLoop++)
    {
        if(wLoop>=OAM_CLI_DAT_SHM_MAX_NUM)
        {
            break;
        }

        if (OAM_CLI_REG_NOT_READED == ptOtherMpRegInfo->tCliReg[wLoop].ucReadFlag)
        {
            if(OAM_CLI_SUCCESS != OAM_CFGDatRegister(ptOtherMpRegInfo->tCliReg[wLoop].tAppJid, ptOtherMpRegInfo->tCliReg[wLoop].dwDatFuncType))
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR, "[Oam_CfgScanRegTable] Job register fail, jid = %d, functype = %d", ptOtherMpRegInfo->tCliReg[wLoop].tAppJid, ptOtherMpRegInfo->tCliReg[wLoop].dwDatFuncType);
                continue;
            }
        
            ptOtherMpRegInfo->tCliReg[wLoop].ucReadFlag = OAM_CLI_REG_READED;
        }
    }

    /* 等所有都加载完成之后，再看看有没有SBCdat没有加载成功，如果有则需要向VMM请求 */
    for(wLoop = 0; wLoop < ptOtherMpRegInfo->dwRegCount; wLoop++)
    {
        if(wLoop >= OAM_CLI_DAT_SHM_MAX_NUM)
        {
            break;
        }

        if ((OAM_CLI_REG_NOT_READED == ptOtherMpRegInfo->tCliReg[wLoop].ucReadFlag) &&
            (OAM_CLI_REG_TYPE_SPECIAL == ptOtherMpRegInfo->tCliReg[wLoop].ucRegType))
        {
            /* 停止所有正在执行的命令，断开所有连接 */
            Oam_CfgStopAllLinkExcutingCmd();

            /* 清除所有注册的dat内存 */
            OAM_CFGRemoveAllDat();
            /*如果是刀片环境，清除版本 信息，从vmm重新获取*/
            if (!Oam_CfgIfCurVersionIsPC())
            {
                g_ptOamIntVar->wDatSum = 0;
            }
            
            RegDatToVmm();
            ReqAllCfgVerInfo();
            break;
        }
            
    }
}

/**********************************************************************
* 函数名称：VOID RecvAgtUnreg
* 功能描述：接收Agent注销信息，平台/T8000应用进行注销
* 访问的表：无
* 修改的表：无
* 输入参数：T_AlmAgtVars *ptVar：私有数据区
            LPVOID pMsgPara:     消息数据
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-7-24             吴树高      创建
************************************************************************/
void RecvAgtUnreg(T_OAM_INTERPRET_VAR *ptVar, LPVOID pMsgPara, BOOLEAN bIsSameEndian)
{
    WORD32          dwLoop = 0;
    WORD32          *pdwRegNum = NULL;
    /*T_CliAppData    *ptRecvAppData = NULL;*/
    JID             tSenderJid;
    T_CliAppReg     *ptPriData = NULL;

    T_CliAppData *ptRecvAppData,tMsgRecvData;
//    DWORD dwVer = 0;
    WORD16 wLen = 0;
    
    /******************** 消息兼容 开始 ********************/
    /* 消息结构默认初始化 */
    memset(&tMsgRecvData, 0xFF, sizeof(tMsgRecvData));

    /* 函数堆栈中申请消息数据结构，避免动态内存申请/释放 */
    ptRecvAppData  = &tMsgRecvData;

    /* 调用消息版本兼容转换函数 */
  //  XOS_GetMsgAppVer((BYTE *)&dwVer);
    XOS_GetMsgDataLen(PROCTYPE_OAM_CLIMANAGER, (WORD16 *)&wLen);
 //   INVERT_MSG_T_CliAppData((BYTE **)&ptRecvAppData,pMsgPara,dwVer,wLen);
    /******************** 消息兼容 结束 ********************/
#if 0
    if(XOS_SUCCESS != XOS_GetCurMsgData((BYTE**)&ptRecvAppData))
        return;
#endif
    /*字节序转换*/
    if (!bIsSameEndian)
    {
        ptRecvAppData->tAppJid.dwJno = XOS_InvertWord32(ptRecvAppData->tAppJid.dwJno);
        ptRecvAppData->tAppJid.dwDevId = XOS_InvertWord32(ptRecvAppData->tAppJid.dwDevId);
        ptRecvAppData->tAppJid.wModule = XOS_InvertWord16(ptRecvAppData->tAppJid.wModule);
        ptRecvAppData->tAppJid.wUnit = XOS_InvertWord16(ptRecvAppData->tAppJid.wUnit);
        ptRecvAppData->dwDatFuncType = XOS_InvertWord32(ptRecvAppData->dwDatFuncType);
    }
    
    /* 注意数据区结构体初使化 */
    memset(&tSenderJid, 0, sizeof(JID));
    /* OMP上的应用进程注册信息，无需再与Agent同步 */
    if (IsSameLogicAddr(&(g_ptOamIntVar->jidSelf), &(ptRecvAppData->tAppJid)))
    {
        if (OAM_CLI_SUCCESS == OAM_CFGDatUnregister(ptRecvAppData->tAppJid))
        {
            XOS_SysLog(OAM_CFG_LOG_NOTICE, "RecvAgtUnreg success, dat functype = %d\n", ptRecvAppData->dwDatFuncType);
        }
        else
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, "RecvAgtUnreg fail, dat functype = %d\n", ptRecvAppData->dwDatFuncType);
        }
        return;
    }
    /* 目前所知应该只有RPU上有业务注册进程，先默认RPU吧 */
    else
    {
        pdwRegNum  = &ptVar->tPlatMpCliAppInfo.dwRegCount;
        ptPriData = &ptVar->tPlatMpCliAppInfo;
    }

    /* 需要删除数据区内容 */
    for(dwLoop=0; dwLoop<*pdwRegNum; dwLoop++)
    {
        if(dwLoop>=OAM_CLI_DAT_SHM_MAX_NUM)
            return;

        /* 直接认为已经注册过了 */
        if(ptRecvAppData->dwDatFuncType == ptPriData->tCliReg[dwLoop].dwDatFuncType)
        {
            /* 需要更新命令树上的JID */
            if (OAM_CLI_SUCCESS == OAM_CFGDatUnregister(ptRecvAppData->tAppJid))
            {
                XOS_SysLog(OAM_CFG_LOG_NOTICE,"RecvAgtUnreg success, dat functype = %d\n", ptRecvAppData->dwDatFuncType);
            }
            else
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR, "RecvAgtUnreg fail, dat functype = %d\n", ptRecvAppData->dwDatFuncType);
            }
			
            memset(&ptPriData->tCliReg[dwLoop], 0, sizeof(T_CliAppData));
            (*pdwRegNum)--;

            return;
        }
    }

    return;
}

/**********************************************************************
* 函数名称：VOID RecvAgtRegEx
* 功能描述：接收Agent注册信息，非平台业务进程注册
* 访问的表：无
* 修改的表：无
* 输入参数：T_AlmAgtVars *ptVar：私有数据区
            LPVOID pMsgPara:     消息数据
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-7-24             吴树高      创建
************************************************************************/
void RecvAgtRegEx(T_OAM_INTERPRET_VAR *ptVar, LPVOID pMsgPara, BOOLEAN bIsSameEndian)
{
    WORD32          dwIndex = 0;
    /*T_CliAppDataEx  *ptRecvAppData = NULL;*/

    T_CliAppDataEx *ptRecvAppData,tMsgRecvData;
//    DWORD dwVer = 0;
    WORD16 wLen = 0;
    
    /******************** 消息兼容 开始 ********************/
    /* 消息结构默认初始化 */
    memset(&tMsgRecvData, 0xFF, sizeof(tMsgRecvData));

    /* 函数堆栈中申请消息数据结构，避免动态内存申请/释放 */
    ptRecvAppData  = &tMsgRecvData;

    /* 调用消息版本兼容转换函数 */
  //  XOS_GetMsgAppVer((BYTE *)&dwVer);
    XOS_GetMsgDataLen(PROCTYPE_OAM_CLIMANAGER, (WORD16 *)&wLen);
  //  INVERT_MSG_T_CliAppDataEx((BYTE **)&ptRecvAppData,pMsgPara,dwVer,wLen);
    /******************** 消息兼容 结束 ********************/
#if 0
    if(XOS_SUCCESS != XOS_GetCurMsgData((BYTE**)&ptRecvAppData))
        return;
#endif
    /*字节序转换*/
    if (!bIsSameEndian)
    {
        ptRecvAppData->tAppJid.dwJno = XOS_InvertWord32(ptRecvAppData->tAppJid.dwJno);
        ptRecvAppData->tAppJid.dwDevId = XOS_InvertWord32(ptRecvAppData->tAppJid.dwDevId);
        ptRecvAppData->tAppJid.wModule = XOS_InvertWord16(ptRecvAppData->tAppJid.wModule);
        ptRecvAppData->tAppJid.wUnit = XOS_InvertWord16(ptRecvAppData->tAppJid.wUnit);
        ptRecvAppData->dwNetId = XOS_InvertWord32(ptRecvAppData->dwNetId);
    }
    
    /* OMP上的应用进程注册信息*/
    if (IsSameLogicAddr(&(g_ptOamIntVar->jidSelf), &(ptRecvAppData->tAppJid)))
    {
        /* OMP,什么也不需要做*/
        return;
    }

    /*如果网元id已经被注册过，返回*/
    if (LompCliAppRegInfoIsExist(ptRecvAppData->dwNetId))
    {
    	  UpdateCliAppRegInfo(ptRecvAppData->dwNetId,&(ptRecvAppData->tAppJid));
        return;
    }
    
    /* 保存到数据区 */
    /* 放到最后一个节点 */
    dwIndex = ptVar->tLompCliAppRegInfo.dwRegCount;
    if (dwIndex >= OAM_CLI_APP_SHM_NUM)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "RecvAgtRegEx: register failed, ptVar->tLompCliAppRegInfo.dwRegCount too big(%d)\n", dwIndex);
    }
    else
    {
        memcpy(&(ptVar->tLompCliAppRegInfo.tCliReg[dwIndex].tAppJid), &(ptRecvAppData->tAppJid), sizeof(JID));
        ptVar->tLompCliAppRegInfo.tCliReg[dwIndex].dwNetId = ptRecvAppData->dwNetId;
        ptVar->tLompCliAppRegInfo.dwRegCount++;
    }

    return;
}


/**********************************************************************
* 函数名称：VOID Oam_CfgRecvRegA2M
* 功能描述：接收Agent给Manager的同步消息，平台/T8000业务进程注册的信息
*                           目前只有RPU(RPU与OMP不合一时)会向OMP同步
* 访问的表：无
* 修改的表：无
* 输入参数：T_AlmAgtVars *ptVar：私有数据区
            LPVOID pMsgPara:     消息数据
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-7-24             吴树高      创建
************************************************************************/
void Oam_CfgRecvRegA2M(T_OAM_INTERPRET_VAR *ptVar, LPVOID pMsgPara, BOOLEAN bIsSameEndian)
{
   /*消除pcLint告警，将sword32修改为word32 ,SWORD32        dwLoop = 0;*/
    WORD32         dwRet = 0;
    WORD32        dwLoop = 0;
    WORD16         wRegDatCnt = 0;
    WORD32         dwRegLoop = 0;
    JID tSenderJid;
    T_CliAppReg *ptRecvAppData,tMsgRecvData;
  //  DWORD dwVer = 0;
    WORD16 wLen = 0;
    WORD32       *pdwRegNum = NULL;
    T_CliAppReg  *ptPriData = NULL;
    BOOLEAN bHasSpecial = FALSE;
    WORD32         dwRegedCntOfShm = 0;
    
    /******************** 消息兼容 开始 ********************/
    /* 消息结构默认初始化 */
    memset(&tMsgRecvData, 0xFF, sizeof(tMsgRecvData));

    /* 函数堆栈中申请消息数据结构，避免动态内存申请/释放 */
    ptRecvAppData  = &tMsgRecvData;

    /* 调用消息版本兼容转换函数 */
  //  XOS_GetMsgAppVer((BYTE *)&dwVer);
    XOS_GetMsgDataLen(PROCTYPE_OAM_CLIMANAGER, (WORD16 *)&wLen);
  //  INVERT_MSG_T_CliAppReg((BYTE **)&ptRecvAppData,pMsgPara,dwVer,wLen);
    /******************** 消息兼容 结束 ********************/

    /*字节序转换*/
    if (!bIsSameEndian)
    {
        ptRecvAppData->dwSemid = XOS_InvertWord32(ptRecvAppData->dwSemid);
        ptRecvAppData->dwRegCount = XOS_InvertWord32(ptRecvAppData->dwRegCount);
        for(dwRegLoop=0; dwRegLoop<ptRecvAppData->dwRegCount; dwRegLoop++)
        {
            ptRecvAppData->tCliReg[dwRegLoop].tAppJid.dwJno = XOS_InvertWord32(ptRecvAppData->tCliReg[dwRegLoop].tAppJid.dwJno);
            ptRecvAppData->tCliReg[dwRegLoop].tAppJid.dwDevId = XOS_InvertWord32(ptRecvAppData->tCliReg[dwRegLoop].tAppJid.dwDevId);
            ptRecvAppData->tCliReg[dwRegLoop].tAppJid.wModule = XOS_InvertWord16(ptRecvAppData->tCliReg[dwRegLoop].tAppJid.wModule);
            ptRecvAppData->tCliReg[dwRegLoop].tAppJid.wUnit = XOS_InvertWord16(ptRecvAppData->tCliReg[dwRegLoop].tAppJid.wUnit);
            ptRecvAppData->tCliReg[dwRegLoop].dwDatFuncType = XOS_InvertWord32(ptRecvAppData->tCliReg[dwRegLoop].dwDatFuncType);
        }
    }
    
    /*获取发送者的jid*/
    memset(&tSenderJid, 0, sizeof(tSenderJid));
    dwRet = XOS_Sender(&tSenderJid);
    /*获取发送端JID失败*/
    if (XOS_SUCCESS != dwRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XOS_Sender Error,%d! ....................",__func__,__LINE__,dwRet);
        return;
    }
    if (NULL == g_ptOamIntVar)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return;
    }
    /* 如果是OMP上的同步消息，忽略 */
    if (IsSameLogicAddr(&(g_ptOamIntVar->jidSelf), &tSenderJid))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "Oam_CfgRecvRegA2M: receive register table from omp cliagent, ignored");
        return;
    }

    pdwRegNum  = &ptVar->tPlatMpCliAppInfo.dwRegCount;

    /*++++++++++++++++++++++++++++++++++++先将无效的注销，腾出空间++++++++++++++++++++++++++++++++++++++++++++++++*/
    /* 与同步消息发送方相同模块号的比较标记清除 */
    for(dwLoop = 0; dwLoop < *pdwRegNum; dwLoop++)
    {
        if(dwLoop >= OAM_CLI_DAT_SHM_MAX_NUM)
            break;

        if (IsSameLogicAddr(&(ptVar->tPlatMpCliAppInfo.tCliReg[dwLoop].tAppJid), &tSenderJid))
        {
            ptVar->tPlatMpCliAppInfo.tCliReg[dwLoop].ucCmpFlag = 0;
        }
    }

    /* 将同步过来的相同功能类型的打上标记 */
    for(dwRegLoop = 0; dwRegLoop < ptRecvAppData->dwRegCount; dwRegLoop++)
    {
        if(dwRegLoop >= OAM_CLI_DAT_SHM_MAX_NUM)
            break;

        for(dwLoop = 0; dwLoop < *pdwRegNum; dwLoop++)
        {
            if(dwLoop >= OAM_CLI_DAT_SHM_MAX_NUM)
                break;

            if (ptRecvAppData->tCliReg[dwRegLoop].dwDatFuncType == ptVar->tPlatMpCliAppInfo.tCliReg[dwLoop].dwDatFuncType)
                ptVar->tPlatMpCliAppInfo.tCliReg[dwLoop].ucCmpFlag = 1;
        }
        
    }

    /* 注销无效的注册 */
    for(dwLoop = (*pdwRegNum) - 1; dwLoop >= 0; dwLoop--)
    {
        if (dwLoop >= OAM_CLI_DAT_SHM_MAX_NUM)
        {
            break;
        }
        
        /* 表示没有打上标签，说明此注册信息应该无效了 */
        if(0 == ptVar->tPlatMpCliAppInfo.tCliReg[dwLoop].ucCmpFlag)
        {
            /*注销已失效的job*/
            if (OAM_CLI_SUCCESS == OAM_CFGDatUnregister(ptVar->tPlatMpCliAppInfo.tCliReg[dwLoop].tAppJid))
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR, "Oam_CfgRecvRegA2M: unreginster success, dat functype = %d\n", ptVar->tPlatMpCliAppInfo.tCliReg[dwLoop].dwDatFuncType);
            }
            else
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR, "Oam_CfgRecvRegA2M: unreginster failed, dat functype = %d\n", ptVar->tPlatMpCliAppInfo.tCliReg[dwLoop].dwDatFuncType);
                continue;
            }
        
            /* 把最后一个节点挪到此节点 */
            if(dwLoop != ptVar->tPlatMpCliAppInfo.dwRegCount-1)
            {
                memcpy(&ptVar->tPlatMpCliAppInfo.tCliReg[dwLoop].tAppJid,
                       &ptVar->tPlatMpCliAppInfo.tCliReg[ptVar->tPlatMpCliAppInfo.dwRegCount-1].tAppJid, sizeof(JID));
                ptVar->tPlatMpCliAppInfo.tCliReg[dwLoop].dwDatFuncType =
                    ptVar->tPlatMpCliAppInfo.tCliReg[ptVar->tPlatMpCliAppInfo.dwRegCount-1].dwDatFuncType;
            }
            /* 清除最后一个节点的数据 */
            memset(&ptVar->tPlatMpCliAppInfo.tCliReg[ptVar->tPlatMpCliAppInfo.dwRegCount-1], 0, sizeof(T_CliAppData));
            ptVar->tPlatMpCliAppInfo.dwRegCount--;
        }
    }
    
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

    pdwRegNum  = &ptVar->tPlatMpCliAppInfo.dwRegCount;
    /* 共享内存中和全局数据区中的已注册dat总数据不能超过最大值 */
    if (!OamRegCntOfShm(&dwRegedCntOfShm))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, 
            "Oam_CfgRecvRegA2M: Exec func OamRegCntOfShm failed!\r");
        return;
    }

    /* 如果同步过来的有SBC等业务dat信息，且数据区没有注册过的，需要向VMM请求版本 */
    for(dwRegLoop = 0; dwRegLoop < ptRecvAppData->dwRegCount; dwRegLoop++)
    {
        if(dwRegLoop >= OAM_CLI_DAT_SHM_MAX_NUM)
            break;

        if (ptRecvAppData->tCliReg[dwRegLoop].ucRegType == OAM_CLI_REG_TYPE_SPECIAL)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, "Oam_CfgRecvRegA2M: Has special dat in register table...");
            
            pdwRegNum  = &ptVar->tPlatMpCliAppInfo.dwRegCount;
            ptPriData = &ptVar->tPlatMpCliAppInfo;

            for(dwLoop = 0; dwLoop < *pdwRegNum; dwLoop++)
            {
                if(dwLoop > OAM_CLI_DAT_SHM_MAX_NUM)
                    break;
                
                if(ptRecvAppData->tCliReg[dwRegLoop].dwDatFuncType == ptPriData->tCliReg[dwLoop].dwDatFuncType)
                {
                    XOS_SysLog(OAM_CFG_LOG_ERROR, "Oam_CfgRecvRegA2M: The special dat in register table is exists...");
                    break;
                }
            }

            /* 在数据区里没有找到，需要添加进来 */
            if(dwLoop == *pdwRegNum)
            {
                /* 共享内存中和全局数据区中的已注册dat总数据不能超过最大值 */
                if ((dwRegedCntOfShm + (*pdwRegNum)) >= OAM_CLI_DAT_SHM_MAX_NUM)
                {
                    XOS_SysLog(OAM_CFG_LOG_ERROR, 
                        "Oam_CfgRecvRegA2M: Total of registered dats has reached max!\r");
                    break;
                }

                XOS_SysLog(OAM_CFG_LOG_ERROR, "Oam_CfgRecvRegA2M: Add special dat to gloable data...");
                memcpy(&ptPriData->tCliReg[dwLoop].tAppJid, &(ptRecvAppData->tCliReg[dwRegLoop].tAppJid), sizeof(JID));
                ptPriData->tCliReg[dwLoop].dwDatFuncType = ptRecvAppData->tCliReg[dwRegLoop].dwDatFuncType;
                ptPriData->tCliReg[dwLoop].ucReadFlag = OAM_CLI_REG_NOT_READED;
                ptPriData->tCliReg[dwLoop].ucRegType = OAM_CLI_REG_TYPE_SPECIAL;
                (*pdwRegNum)++;
                bHasSpecial = TRUE;
            }
        }
    }

    if (bHasSpecial)
    {
        /* 停止所有正在执行的命令，断开所有连接 */
        Oam_CfgStopAllLinkExcutingCmd();

        /* 清除所有注册的dat内存 */
        OAM_CFGRemoveAllDat();
        /*如果是刀片环境，清除版本 信息，从vmm重新获取*/
        if (!Oam_CfgIfCurVersionIsPC())
        {
            g_ptOamIntVar->wDatSum = 0;
        }
        
        RegDatToVmm();
        ReqAllCfgVerInfo();

        return;
    }

    for(dwRegLoop = 0; dwRegLoop < ptRecvAppData->dwRegCount; dwRegLoop++)
    {
        if(dwRegLoop >= OAM_CLI_DAT_SHM_MAX_NUM)
            break;

        for(dwLoop = 0; dwLoop < ptVar->tPlatMpCliAppInfo.dwRegCount; dwLoop++)
        {
            if(dwLoop >= OAM_CLI_DAT_SHM_MAX_NUM)
                break;

            /* 直接认为已经注册过了 */
            if(ptRecvAppData->tCliReg[dwRegLoop].dwDatFuncType == ptVar->tPlatMpCliAppInfo.tCliReg[dwLoop].dwDatFuncType)
            {
                /* added by fls on 2009-02-27 (CRDCR00481329 rpu重启之后,经常出现在线配置进入不了) */
                for (wRegDatCnt = 0; wRegDatCnt < g_wRegDatCount; wRegDatCnt++)
                {
                    if (g_tDatLink[wRegDatCnt].datIndex.tJid.dwJno == ptRecvAppData->tCliReg[dwRegLoop].tAppJid.dwJno)
                    {
                        memcpy(&g_tDatLink[wRegDatCnt].datIndex.tJid, &ptRecvAppData->tCliReg[dwRegLoop].tAppJid, sizeof(JID));
                        if (ptRecvAppData->tCliReg[dwRegLoop].dwDatFuncType == FUNC_SCRIPT_CLIS_BRS)
                            memcpy(&g_ptOamIntVar->jidProtocol, &ptRecvAppData->tCliReg[dwRegLoop].tAppJid, sizeof(JID));
                        break;
                    }
                }
                /* end of add */
                ptVar->tPlatMpCliAppInfo.tCliReg[dwLoop].ucCmpFlag = 1;
                break;
            }
        }

        /* 未比较出相同的功能类型，需要进行注册*/
        if(dwLoop == ptVar->tPlatMpCliAppInfo.dwRegCount)
        {
            /* 共享内存中和全局数据区中的已注册dat总数据不能超过最大值 */
            if ((dwRegedCntOfShm + ptVar->tPlatMpCliAppInfo.dwRegCount) >= OAM_CLI_DAT_SHM_MAX_NUM)
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR, 
                    "Oam_CfgRecvRegA2M: Total of registered dats has reached max!\r");
                break;
            }
                
            /*注册新增的job*/
            if(OAM_CLI_SUCCESS != OAM_CFGDatRegister(ptRecvAppData->tCliReg[dwRegLoop].tAppJid, ptRecvAppData->tCliReg[dwRegLoop].dwDatFuncType))
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR, "Oam_CfgRecvRegA2M: register failed, dat functype = %d\n", ptRecvAppData->tCliReg[dwRegLoop].dwDatFuncType);
                continue;
            }
            else
            {
                ptRecvAppData->tCliReg[dwRegLoop].ucReadFlag = OAM_CLI_REG_READED;
                XOS_SysLog(OAM_CFG_LOG_ERROR, "Oam_CfgRecvRegA2M: register success, dat functype = %d\n", ptRecvAppData->tCliReg[dwRegLoop].dwDatFuncType);
            }
            
            ptVar->tPlatMpCliAppInfo.tCliReg[dwLoop].ucCmpFlag = 1;
            memcpy(&(ptVar->tPlatMpCliAppInfo.tCliReg[dwLoop].tAppJid), &(ptRecvAppData->tCliReg[dwRegLoop].tAppJid), sizeof(JID));
            ptVar->tPlatMpCliAppInfo.tCliReg[dwLoop].dwDatFuncType = ptRecvAppData->tCliReg[dwRegLoop].dwDatFuncType;
            ptVar->tPlatMpCliAppInfo.dwRegCount++;
        }
    }

    return;
}


/**********************************************************************
* 函数名称：VOID RecvRegExA2M
* 功能描述：接收Agent给Manager的同步消息，非平台业务进程注册的信息
* 访问的表：无
* 修改的表：无
* 输入参数：T_AlmAgtVars *ptVar：私有数据区
            LPVOID pMsgPara:     消息数据
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2008-7-24             吴树高      创建
************************************************************************/
void RecvRegExA2M(T_OAM_INTERPRET_VAR *ptVar, LPVOID pMsgPara, BOOLEAN bIsSameEndian)
{
    WORD32          dwIndex = 0;
    WORD32          dwRegLoop = 0;
    /*T_CliAppAgtRegEx   *ptRecvAppData = NULL;*/
    JID                   tSenderJid;

    T_CliAppAgtRegEx *ptRecvAppData,tMsgRecvData;
//    DWORD dwVer = 0;
    WORD16 wLen = 0;
    
    /******************** 消息兼容 开始 ********************/
    /* 消息结构默认初始化 */
    memset(&tMsgRecvData, 0xFF, sizeof(tMsgRecvData));

    /* 函数堆栈中申请消息数据结构，避免动态内存申请/释放 */
    ptRecvAppData  = &tMsgRecvData;

    /* 调用消息版本兼容转换函数 */
 //   XOS_GetMsgAppVer((BYTE *)&dwVer);
    XOS_GetMsgDataLen(PROCTYPE_OAM_CLIMANAGER, (WORD16 *)&wLen);
//    INVERT_MSG_T_CliAppAgtRegEx((BYTE **)&ptRecvAppData,pMsgPara,dwVer,wLen);
    /******************** 消息兼容 结束 ********************/
#if 0
    if(XOS_SUCCESS != XOS_GetCurMsgData((BYTE**)&ptRecvAppData))
        return;
#endif
    /*字节序转换*/
    if (!bIsSameEndian)
    {
        ptRecvAppData->dwSemid = XOS_InvertWord32(ptRecvAppData->dwSemid);
        ptRecvAppData->dwRegCount = XOS_InvertWord32(ptRecvAppData->dwRegCount);
        for(dwRegLoop=0; dwRegLoop<ptRecvAppData->dwRegCount; dwRegLoop++)
        {
            ptRecvAppData->tCliReg[dwRegLoop].tAppJid.dwJno = XOS_InvertWord32(ptRecvAppData->tCliReg[dwRegLoop].tAppJid.dwJno);
            ptRecvAppData->tCliReg[dwRegLoop].tAppJid.dwDevId = XOS_InvertWord32(ptRecvAppData->tCliReg[dwRegLoop].tAppJid.dwDevId);
            ptRecvAppData->tCliReg[dwRegLoop].tAppJid.wModule = XOS_InvertWord16(ptRecvAppData->tCliReg[dwRegLoop].tAppJid.wModule);
            ptRecvAppData->tCliReg[dwRegLoop].tAppJid.wUnit = XOS_InvertWord16(ptRecvAppData->tCliReg[dwRegLoop].tAppJid.wUnit);
            ptRecvAppData->tCliReg[dwRegLoop].dwNetId = XOS_InvertWord32(ptRecvAppData->tCliReg[dwRegLoop].dwNetId);
        }
    }

    /*获取发送者的jid*/
    memset(&tSenderJid, 0, sizeof(tSenderJid));
    XOS_Sender(&tSenderJid);

    /*OMP上不需要同步*/
    if (IsSameLogicAddr(&(g_ptOamIntVar->jidSelf), &tSenderJid))
    {
        XOS_SysLog(OAM_CFG_LOG_NOTICE, "RecvRegExA2M: receive register table(ex) from omp cliagent, ignored");
        return;
    }

    /* 比较、且保存到数据区 */
    for(dwRegLoop=0; dwRegLoop<ptRecvAppData->dwRegCount; dwRegLoop++)
    {
        if(dwRegLoop >= OAM_CLI_APP_SHM_NUM)
            break;

        /*如果已经注册过了，更新注册信息*/
        if (LompCliAppRegInfoIsExist(ptRecvAppData->tCliReg[dwRegLoop].dwNetId))
        {
            UpdateCliAppRegInfo(ptRecvAppData->tCliReg[dwRegLoop].dwNetId,&(ptRecvAppData->tCliReg[dwRegLoop].tAppJid));
        }
        else
        {
            /* 未注册过，需要写入私有数据区 */
            dwIndex = ptVar->tLompCliAppRegInfo.dwRegCount;
            memcpy(&(ptVar->tLompCliAppRegInfo.tCliReg[dwIndex].tAppJid), &(ptRecvAppData->tCliReg[dwRegLoop].tAppJid), sizeof(JID));
            ptVar->tLompCliAppRegInfo.tCliReg[dwIndex].dwNetId = ptRecvAppData->tCliReg[dwRegLoop].dwNetId;
            ptVar->tLompCliAppRegInfo.dwRegCount++;
        }
    }

    return;
}

/**********************************************************************
* 函数名称：UpdateCliAppRegInfo
* 功能描述：更新业务注册表中信息
* 访问的表：无
* 修改的表：无
* 输入参数：WORD32 dwNetId : 需要检查的网元id
* 输出参数：无
* 返 回 值：   无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2009-8-11          齐龙兆    创建
************************************************************************/
static void UpdateCliAppRegInfo(WORD32 dwNetId,JID *ptJid)
{
    WORD32 dwLoop = 0;
    BOOLEAN bRet = FALSE;
    
    /*参数检查*/
    if (NULL == ptJid)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return;
    }

    if (g_ptOamIntVar)
    {        
        /*检查数据区中注册信息*/
        for(dwLoop=0; dwLoop< g_ptOamIntVar->tLompCliAppRegInfo.dwRegCount; dwLoop++)
        {
            if(dwLoop >= OAM_CLI_APP_SHM_NUM)
            {
                bRet = FALSE;
                break;
            }

            /* 已经注册过了 */
            if(dwNetId == g_ptOamIntVar->tLompCliAppRegInfo.tCliReg[dwLoop].dwNetId)
            {
                memcpy(&(g_ptOamIntVar->tLompCliAppRegInfo.tCliReg[dwLoop].tAppJid), ptJid, sizeof(JID));
                bRet = TRUE;
                break;
            }
        }
    }


    /*如果数据区注册表中没有，检查共享内存*/
    if(!bRet)
    {
        T_CliAppAgtRegEx *ptOamCliReg = NULL;
        /*必需在使用后调用Oam_UnLockRegTable释放锁*/
        ptOamCliReg = Oam_GetRegExTableAndLock();
        if (ptOamCliReg)
        {
            for(dwLoop = 0; dwLoop < ptOamCliReg->dwRegCount; dwLoop++)
            {
                if(dwLoop >= OAM_CLI_APP_SHM_NUM)
                {
                    bRet = FALSE;
                    break;
                }

                if (dwNetId == ptOamCliReg->tCliReg[dwLoop].dwNetId)
                {
                    memcpy(&(ptOamCliReg->tCliReg[dwLoop].tAppJid), ptJid, sizeof(JID));
                    bRet = TRUE;
                    break;
                }
            }

            Oam_UnLockRegExTable(ptOamCliReg);
        }
    }

    return;
}


/**********************************************************************
* 函数名称：LompCliAppRegInfoIsExist
* 功能描述：检查指定的网元是否在业务注册表中已经存在
* 访问的表：无
* 修改的表：无
* 输入参数：WORD32 dwNetId : 需要检查的网元id
* 输出参数：无
* 返 回 值：    TRUE - 已经注册过了
                                 FALSE -没有注册过
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2009-5-27          殷浩    创建
************************************************************************/
BOOLEAN LompCliAppRegInfoIsExist(WORD32 dwNetId)
{
    WORD32 dwLoop = 0;
    BOOLEAN bRet = FALSE;

    if (g_ptOamIntVar)
    {        
        /*检查数据区中注册信息*/
        for(dwLoop=0; dwLoop< g_ptOamIntVar->tLompCliAppRegInfo.dwRegCount; dwLoop++)
        {
            if(dwLoop >= OAM_CLI_APP_SHM_NUM)
            {
                bRet = FALSE;
                break;
            }

            /* 已经注册过了 */
            if(dwNetId == g_ptOamIntVar->tLompCliAppRegInfo.tCliReg[dwLoop].dwNetId)
            {
                bRet = TRUE;
                break;
            }
        }
    }


    /*如果数据区注册表中没有，检查共享内存*/
    if(!bRet)
    {
        T_CliAppAgtRegEx *ptOamCliReg = NULL;
        /*必需在使用后调用Oam_UnLockRegTable释放锁*/
        ptOamCliReg = Oam_GetRegExTableAndLock();
        if (ptOamCliReg)
        {
            for(dwLoop = 0; dwLoop < ptOamCliReg->dwRegCount; dwLoop++)
            {
                if(dwLoop >= OAM_CLI_APP_SHM_NUM)
                {
                    bRet = FALSE;
                    break;
                }

                if (dwNetId == ptOamCliReg->tCliReg[dwLoop].dwNetId)
                {
                    bRet = TRUE;
                    break;
                }
            }

            Oam_UnLockRegExTable(ptOamCliReg);
        }
    }

    return bRet;
}

/**********************************************************************
* 函数名称：OamRegCntOfShm
* 功能描述：获取共享内存中的dat注册个数
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值： WORD32 - 共享内存中已注册的dat个数
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2010-11-15          傅龙锁    创建
************************************************************************/
BOOLEAN OamRegCntOfShm(WORD32 *pdwRegedCnt)
{
#if 0
    T_CliAppReg   *ptOamCliReg = NULL;

    /*必需在使用后调用Oam_UnLockRegTable释放锁*/
    ptOamCliReg = Oam_GetRegTableAndLock();
    if (!ptOamCliReg)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "OamRegCntOfShm: failed to create shm which contains rigister table and set lock on it!\r");
        return FALSE;
    }

    (*pdwRegedCnt) = ptOamCliReg->dwRegCount;
    
    if (!Oam_UnLockRegTable(ptOamCliReg))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "OamRegCntOfShm: failed to unlock shm which contains register table ");
        return FALSE;
    }
#endif
    return TRUE;
}


