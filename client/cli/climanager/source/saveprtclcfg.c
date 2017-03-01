/**************************************************************************
* 版权所有 (C)2002, 深圳市中兴通讯股份有限公司。
*
* 文件名称：SavePrtclCfg.c
* 文件标识：
* 内容摘要：在线配置命令存盘
* 其它说明：
            
* 当前版本：
* 作    者       ：殷浩
* 完成日期：20080307
*
* 修改记录1 ：// 修改历史记录，包括修改日期、修改者及修改内容
*    修改日期：
*    版 本 号    ：
*    修 改 人    ：
*    修改内容 :
* 修改记录2：
**************************************************************************/
/**************************************************************************
*                           头文件                                        *
**************************************************************************/
#include "includes.h"
#include "saveprtclcfg.h"
#include "oam_cfg_crc16.h"
#include "oam_execute.h"
#include "xmlagent.h"

/*lint -e762*/
/*lint -e1717*/
/*lint -e413*/
/*lint -e1917*/
/*lint -e931*/
/*lint -e58*/
/*lint -e727*/
/*lint -e666*/

/**************************************************************************
*                          常量                                           *
**************************************************************************/
/**************************************************************************
*                          宏                                             *
**************************************************************************/
/**************************************************************************
*                          数据类型                                       *
**************************************************************************/
/**************************************************************************
*                          全局变量                                       *
**************************************************************************/

/**************************************************************************
*                          本地变量                                       *
**************************************************************************/
static T_Time g_tRstrBgnTime;
static WORD16 g_wCmdToBrsCnt = 0;
/**************************************************************************
*                          全局函数原型                                   *
**************************************************************************/
void Oam_InitOneLinkForTxtRestore(TYPE_LINK_STATE *pLinkState);
void Oam_PrintRstrLog(CHAR *pcLog);
void Oam_CloseRestoreLogFile(void);
BOOL Oam_CheckDatFileIsExist(void);
BYTE Oam_GetCurModeId(TYPE_LINK_STATE *pLinkState);
WORD32 Oam_WriteStrToFile(XOS_FD *pSaveFileFd, CHAR *strCmdLines);
WORD32 Oam_RestorePrtclCfgPrepare(void);

static BOOLEAN  GetTxtVersionFromTxtLine(CHAR *pcTxtLine, BYTE *pucVersion);
/**************************************************************************
*                          局部函数原型                                   *
**************************************************************************/

/**************************************************************************
* 函数名称：Oam_Duration
* 功能描述：计算两个时间点间的毫秒数
* 访问的表：无
* 修改的表：无
* 输入参数：T_Time tBegin - 开始时间点
            T_Time tEnd - 结束时间点
* 输出参数：
* 返 回 值：开始到结束的毫秒数
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2008/04/24  V1.0    殷浩      创建
**************************************************************************/
WORD32 Oam_Duration(T_Time tBegin, T_Time tEnd)
{
    return (tEnd.dwSecond*1000+tEnd.wMilliSec)-(tBegin.dwSecond*1000+tBegin.wMilliSec);
}

/**************************************************************************
* 函数名称：WORD32 Oam_SaveCfgCmdLine
* 功能描述：把输入的命令行存盘
* 访问的表：无
* 修改的表：无
* 输入参数：CHAR *strCmdLines - 待存盘的命令行字符串
* 输出参数：
* 返 回 值   ：参见头文件定义
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2008/03/17   V1.0    殷浩      创建
**************************************************************************/
WORD32 Oam_WriteStrToFile(XOS_FD *pSaveFileFd, CHAR *strCmdLines)
{
    WORD32 dwRtn = OAM_SAVE_SUCESS;
    SWORD32 swCnt = 0;

    assert(pSaveFileFd >= 0);
    if (pSaveFileFd < 0)
    {
        return OAM_SAVE_ERR_NULL_FD;
    }

    dwRtn = XOS_SeekFile(*pSaveFileFd, XOS_SEEK_END, 0);
    if (XOS_SUCCESS != dwRtn)
    {
        return OAM_SAVE_ERR_SEEK_END;
    }

    dwRtn = XOS_WriteFile (*pSaveFileFd, (void *)strCmdLines, strlen(strCmdLines), &swCnt);
    if (XOS_SUCCESS != dwRtn)
    {
        /*直接返回写文件错误的返回值*/
        return dwRtn;
    }

    return OAM_SAVE_SUCESS;
}

BOOL Oam_CheckDatFileIsExist(void)
{
    WORD32 dwRtn = 0;
    XOS_FD fdDatFile = OAM_INVALID_FD;
    CHAR aucDatFileName[FILE_PATH_LEN+FILE_NAME_LEN+2] = {0};
    INT32 i = 0;
    
    /*判断BRS的DAT文件是否存在*/
        for(i = 0; i < g_ptOamIntVar->wDatSum; i++)
        {
        if (g_ptOamIntVar->tDatVerInfo[i].tFileInfo.wFuncType == FUNC_SCRIPT_CLIS_BRS)
            {
            snprintf(aucDatFileName, sizeof(aucDatFileName),"%s/%s",
                    g_ptOamIntVar->tDatVerInfo[i].tFileInfo.acVerFilePath,
                    g_ptOamIntVar->tDatVerInfo[i].tFileInfo.acVerFileName);
            break;
        }
        }
        
    if(i == g_ptOamIntVar->wDatSum)
    {
        return FALSE;
    }
    

    /*打开待读取的DAT文件*/
    dwRtn = XOS_OpenFile(aucDatFileName, XOS_RDONLY, &fdDatFile);
    if (XOS_SUCCESS != dwRtn)
    {
        return FALSE;
    }
    XOS_CloseFile(fdDatFile);
    
    return TRUE;
}

/**************************************************************************
* 函数名称：WORD32 Oam_RestorePrtclCfgPrepare
* 功能描述：恢复数据的预处理
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值   ：参见头文件定义
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2008/03/17   V1.0    殷浩      创建
**************************************************************************/
WORD32 Oam_RestorePrtclCfgPrepare(void)
{
    WORD32 dwRtn = 0;
    BYTE ucCrcCheckResult = 0;
    CHAR aucLogStr[OAM_MAX_RESTORE_LOG_LEN] = {0};
    CHAR *pcTxtFileName = OAM_CFG_PRTCL_SAVE_FILE_ORG;

    /*对文件进行crc校验*/
    ucCrcCheckResult = Oam_CheckTxtCrc16(OAM_CFG_PRTCL_SAVE_FILE_ORG);
    if (OAM_CHECK_CRC_OK != ucCrcCheckResult && OAM_CHECK_CRC_NOCRC != ucCrcCheckResult)
    {
        memset(aucLogStr, 0, sizeof(aucLogStr));
        snprintf(aucLogStr, sizeof(aucLogStr),"File %s CRC check fail.\n", pcTxtFileName);
        Oam_PrintRstrLog(aucLogStr);

        pcTxtFileName = OAM_CFG_PRTCL_SAVE_FILE_BAK;
        /*如果存盘文件校验错误，取备份的存盘文件*/
        ucCrcCheckResult = Oam_CheckTxtCrc16(OAM_CFG_PRTCL_SAVE_FILE_BAK);
    }
    /*根据crc检查情况进行不同处理*/
    if (OAM_CHECK_CRC_FAIL == ucCrcCheckResult)
    {
        memset(aucLogStr, 0, sizeof(aucLogStr));
        snprintf(aucLogStr, sizeof(aucLogStr),"File %s CRC check fail!\n", pcTxtFileName);
        Oam_PrintRstrLog(aucLogStr);

        /*OSS_Printf(PRINT_OAM_PLATCFGAGT, PRN_NORMAL, PRN_LEVEL_HIGHEST,
                   "Warning!Check CRC Fail.\n");*/
        return OAM_RESTORE_PREPARE_CRC_ERR;
    }
    else if (OAM_CHECK_CRC_FILE_NOT_EXIST == ucCrcCheckResult)
    {
        memset(aucLogStr, 0, sizeof(aucLogStr));
        snprintf(aucLogStr, sizeof(aucLogStr),"File %s CRC check fail!\n", pcTxtFileName);
        Oam_PrintRstrLog(aucLogStr);
        Oam_PrintRstrLog("Both orginal save file and backup save file not exist!\n");
        return OAM_RESTORE_PREPARE_FILE_NOT_EXIST;
    }

    memset(aucLogStr, 0, sizeof(aucLogStr));
    snprintf(aucLogStr, sizeof(aucLogStr),"Save file is %s , CRC check ok!\n", pcTxtFileName);
    Oam_PrintRstrLog(aucLogStr);

    if (g_ptOamIntVar->dwTxtFileFd == OAM_INVALID_FD)
    {
        dwRtn = XOS_OpenFile(pcTxtFileName, XOS_RDONLY, (XOS_FD *)&g_ptOamIntVar->dwTxtFileFd);
        if (XOS_SUCCESS != dwRtn)
        {
            memset(aucLogStr, 0, sizeof(aucLogStr));
            g_ptOamIntVar->dwTxtFileFd = OAM_INVALID_FD;
            if (XOS_FILE_ERR_FILE_NOT_EXIST == dwRtn)
            {
                snprintf(aucLogStr, sizeof(aucLogStr),"File %s not exist!\n", pcTxtFileName);
                return OAM_RESTORE_PREPARE_FILE_NOT_EXIST;
            }
            else
            {
                snprintf(aucLogStr, sizeof(aucLogStr),"File %s open failed!\n", pcTxtFileName);
                return OAM_RESTORE_PREPARE_RD_ERR;
            }
            Oam_PrintRstrLog(aucLogStr);
        }
    }

    if (OAM_CHECK_CRC_OK == ucCrcCheckResult)
    {
        /*如果文件有crc校验码，跳过头部的mark信息和校验码*/
        dwRtn = XOS_SeekFile(g_ptOamIntVar->dwTxtFileFd,
                             XOS_SEEK_SET,
                             OAM_TXT_CRC_HEAD_LEN);
        if (XOS_SUCCESS != dwRtn)
        {
            Oam_PrintRstrLog("Seek file header failed!");
            XOS_CloseFile(g_ptOamIntVar->dwTxtFileFd);
            g_ptOamIntVar->dwTxtFileFd = OAM_INVALID_FD;
            return OAM_RESTORE_PREPARE_RD_ERR;
        }
    }

    g_ptOamIntVar->pcTxtFileBuf = (CHAR *)XOS_GetUB(OAM_CFG_RESTORE_IO_BUF_LEN);
    assert(g_ptOamIntVar->pcTxtFileBuf);
    if (!g_ptOamIntVar->pcTxtFileBuf)
    {
        Oam_PrintRstrLog("Get UB for g_ptOamIntVar->pcTxtFileBuf failed!");
        return OAM_RESTORE_PREPARE_OTHERERR;
    }
    memset(g_ptOamIntVar->pcTxtFileBuf, 0, OAM_CFG_RESTORE_IO_BUF_LEN);
    g_ptOamIntVar->pcTxtFilePos = g_ptOamIntVar->pcTxtFileBuf;

    /*判断BRS的DAT文件是否存在*/
    if(!Oam_CheckDatFileIsExist())
    {
        return OAM_RESTORE_PREPARE_NO_DAT;
    }

    return OAM_RESTORE_PREPARE_OK;
}

static BOOL Oam_ReadTxtToBuf(WORD16 *pwLeftBufLen)
{
    CHAR *pcBufHead = g_ptOamIntVar->pcTxtFileBuf;
    CHAR *pcBufPos = g_ptOamIntVar->pcTxtFilePos ? g_ptOamIntVar->pcTxtFilePos : pcBufHead;
    CHAR aucMemForCmp[OAM_CFG_RESTORE_IO_BUF_LEN] = {0};
    WORD16 wLeftBufLen = OAM_CFG_RESTORE_IO_BUF_LEN - (pcBufPos - pcBufHead);
    WORD32 dwRtn = 0;
    SWORD32 sdwCountReaded = 0;

    if((g_ptOamIntVar->dwTxtFileFd != OAM_INVALID_FD) && 
        memcmp(pcBufPos, aucMemForCmp, wLeftBufLen) == 0)
    {
        memset(g_ptOamIntVar->pcTxtFileBuf, 0, OAM_CFG_RESTORE_IO_BUF_LEN);
        pcBufPos = g_ptOamIntVar->pcTxtFileBuf;
        g_ptOamIntVar->pcTxtFilePos = g_ptOamIntVar->pcTxtFileBuf;
        dwRtn = XOS_ReadFile(g_ptOamIntVar->dwTxtFileFd,
                                     (void *)g_ptOamIntVar->pcTxtFileBuf,
                                     OAM_CFG_RESTORE_IO_BUF_LEN - 1,
                                     &sdwCountReaded);
        if (XOS_SUCCESS != dwRtn)
        {
            return FALSE;
        }

        if(sdwCountReaded < (OAM_CFG_RESTORE_IO_BUF_LEN - 1))
        {/*如果文件读取结束，关闭文件*/
            if (g_ptOamIntVar->dwTxtFileFd != OAM_INVALID_FD)
            {
                XOS_CloseFile(g_ptOamIntVar->dwTxtFileFd);
                g_ptOamIntVar->dwTxtFileFd = OAM_INVALID_FD;
            }
            /*防止最后没有回车换行，加上一个换行符*/
            *(g_ptOamIntVar->pcTxtFileBuf + sdwCountReaded) = '\n';
        }
        else
        {/*如果读取的字符数不小于缓存大小，回退最后一段不是完整一行的*/
            WORD16 wTailLen = 0;
            wLeftBufLen = (WORD16)sdwCountReaded;
            while((*(pcBufHead + wLeftBufLen -1) != '\n') && (*(pcBufHead + wLeftBufLen -1) != '\r'))
            {
                *(pcBufHead + wLeftBufLen -1) = 0;
                wLeftBufLen--;
                wTailLen++;
                if(wLeftBufLen <= 0)
                {
                    break;
                }
            }
            XOS_SeekFile(g_ptOamIntVar->dwTxtFileFd, XOS_SEEK_CUR, (0 - wTailLen));
        }
    }
    *pwLeftBufLen = wLeftBufLen;

    return TRUE;
}

static WORD32 Oam_GetLineFromBuf(CHAR *pcInBuffer, WORD16 wInBufLenOutBytesRead, WORD16 wLeftBufLen)
    {
    CHAR *pcBufHead = g_ptOamIntVar->pcTxtFileBuf;
    CHAR *pcBufPos = g_ptOamIntVar->pcTxtFilePos ? g_ptOamIntVar->pcTxtFilePos : pcBufHead;
    CHAR *pcFirstLineEnd = NULL;
    CHAR aucMemForCmp[OAM_CFG_RESTORE_IO_BUF_LEN] = {0};
        CHAR *pcNPos = strchr(pcBufPos, '\n');
        CHAR *pcRPos = strchr(pcBufPos, '\r');

        if (!pcNPos && !pcRPos)
        {
        if((g_ptOamIntVar->dwTxtFileFd == OAM_INVALID_FD) && 
            memcmp(pcBufPos, aucMemForCmp, wLeftBufLen) == 0)
            {/*如果剩下的全是0*/
                return OAM_GET_NEXT_LINE_OK_EOF;
            }
            else
            {
                return OAM_GET_NEXT_LINE_FAIL;
            }
        }
        else if(!pcNPos)
        {
            pcFirstLineEnd = pcRPos;
        }
        else if(!pcRPos)
        {
            pcFirstLineEnd = pcNPos;
        }
        else
        {
            pcFirstLineEnd = OAM_CFG_MIN(pcNPos, pcRPos);
        }
        *pcFirstLineEnd = 0;
        strncpy(pcInBuffer, pcBufPos, wInBufLenOutBytesRead);
        wLeftBufLen -= (pcFirstLineEnd - pcBufPos);
        if (pcFirstLineEnd < (pcBufHead + OAM_CFG_RESTORE_IO_BUF_LEN - 1))
        {
            pcFirstLineEnd++;
        }
        pcBufPos = pcFirstLineEnd;

    /*跳过连续的回车换行*/
    while('\n' == *(pcBufPos) || '\r' == *(pcBufPos))
    {
        pcBufPos++;
        wLeftBufLen--;
    }
    g_ptOamIntVar->pcTxtFilePos = pcBufPos;

    return OAM_GET_NEXT_LINE_OK;
}

/**************************************************************************
* 函数名称：WORD32 Oam_GetNextLineFromFile
* 功能描述：从存盘文件获取一行命令
* 访问的表：无
* 修改的表：无
* 输入参数：dwFileFd - 存盘文件的fd
                              pcBuffer - 命令行的输出缓冲
                              pwInBufLenOutBytesRead - 命令行缓冲大小
* 输出参数：pwInBufLenOutBytesRead - 实际的命令行大小
* 返 回 值   ：参见头文件定义
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2008/03/17   V1.0    殷浩      创建
**************************************************************************/
static WORD32 Oam_GetNextLineFromFile(CHAR *pcInBuffer, WORD16 *pwInBufLenOutBytesRead)
{
    WORD32 dwRtn = 0;
    WORD16 wLeftBufLen = 0;

    if (!Oam_ReadTxtToBuf(&wLeftBufLen))
    {
        return OAM_GET_NEXT_LINE_FAIL;
    }

    dwRtn = Oam_GetLineFromBuf(pcInBuffer, *pwInBufLenOutBytesRead, wLeftBufLen);
    return dwRtn;
}

/**************************************************************************
* 函数名称：VOID ClosePrtclSaveFile
* 功能描述：关闭存盘文件
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2008/03/25    V1.0    殷浩      创建
**************************************************************************/
static void ExitRestoreState(TYPE_LINK_STATE *pLinkState, WORD32 dwRsltToBrs, CHAR *pcResult)
{
    WORD16 wExitMode = 0;
    T_Time tStart, tEnd;
    CHAR aucRsltStr[OAM_MAX_RESTORE_LOG_LEN] = {0};
    snprintf(aucRsltStr, sizeof(aucRsltStr),"Return code to brs is %u\n", (unsigned)dwRsltToBrs);

    /*如果是刀片版本，需要重新加载配置用的dat文件*/
    if (!Oam_CfgIfCurVersionIsPC())
    {
        OAM_CFGClearCmdScript(&(g_ptOamIntVar->tCfgdat));
        OAM_CFGRemoveAllDat();

        OAM_CliReRegisterAllRegistedJobs();
    }

    XOS_GetCurrentTime(&tStart);

    if (g_ptOamIntVar->dwTxtFileFd != OAM_INVALID_FD)
    {
        Oam_PrintRstrLog("\nClose dat file!\n");
        XOS_CloseFile(g_ptOamIntVar->dwTxtFileFd);
        g_ptOamIntVar->dwTxtFileFd = OAM_INVALID_FD;
    }

    
    Oam_PrintRstrLog("\n-------- Restore result -------- \n");
    if (!pLinkState)
    {
/*        WORD32 dwRestoreErr = OAM_TXT_CFGAGENT_UNKNOWN;*/
        Oam_PrintRstrLog("Unknown error occured, restore failed, return to work state\n");
        Oam_PrintRstrLog(aucRsltStr);
        
        XOS_GetCurrentTime(&tEnd);
        g_ptOamIntVar->dwMillSecOnExit += Oam_Duration(tStart, tEnd);

        Oam_CloseRestoreLogFile();

        XOS_SetNextState(PROCTYPE_OAM_CLIMANAGER, S_Work);
        return;
    }

    Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_IDLE);
    Oam_LinkSetInputState(pLinkState, INMODE_NORMAL);

    wExitMode = Oam_GetCurModeId(pLinkState);

    if (OAM_RESTORE_PREPARE_OK == dwRsltToBrs)
    {
        Oam_PrintRstrLog("Restore success, return to S_Work state\n");
    }
    else
    {
        Oam_PrintRstrLog("Restore error, return to S_Work state\n");
    }
    Oam_PrintRstrLog(aucRsltStr);
    XOS_SendAsynMsg(EV_TXTCFG_ACK, (BYTE *)&dwRsltToBrs, sizeof(dwRsltToBrs), XOS_MSG_VER0, XOS_MSG_MEDIUM, &g_ptOamIntVar->jidProtocol);
    
    XOS_GetCurrentTime(&tEnd);
    g_ptOamIntVar->dwMillSecOnExit += Oam_Duration(tStart, tEnd);

    Oam_CloseRestoreLogFile();

    XOS_SetNextState(PROCTYPE_OAM_CLIMANAGER, S_Work);
}

/**************************************************************************
* 函数名称：VOID RecvFromRestore
* 功能描述：接收恢复操作的消息
* 访问的表：无
* 修改的表：无
* 输入参数：
  TYPE_LINK_STATE *pLinkState
  WORD16 wMsgId,
  LPVOID pMsgPara
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2008/03/25    V1.0    殷浩      创建
**************************************************************************/
void RecvFromRestore(WORD32 dwMsgId, LPVOID pMsgPara, BOOLEAN bIsSameEndian)
{
    TYPE_LINK_STATE *pLinkState = NULL;
    CHAR aucCmdLineBuf[OAM_MAX_CMDLINE_LEN] = {0};
    CHAR *pcTmpBuf = NULL;
    WORD16 wBufInLen_BytesOut = OAM_MAX_CMDLINE_LEN - 1;
    static WORD32 dwGetCmdRtn = 0;
    BOOL bNeedCheckEOF = TRUE;
    WORD16 wStrLenTmp = 0;
    static BOOL bCurCmdLineIsFirst = FALSE;

    if (EV_OAM_CFG_CFG == dwMsgId)
    {
        if (!Oam_CfgCheckCommOamMsgLength())
        {
            return;
        }
    }

    pLinkState = OamCfgGetLinkState(dwMsgId,pMsgPara);
    assert(pLinkState != NULL);
    if(!pLinkState)
    {
        CHAR *pcResult = "Get null linkstate restore failed!\n";
        Oam_PrintRstrLog(pcResult);
        ExitRestoreState(pLinkState, OAM_TXT_CFGAGENT_UNKNOWN, pcResult);
        return;
    }

    /*如果是刚进入恢复状态，初始化linkstate*/
    if (OAM_MSG_RESTORE_PREPARE_OK == dwMsgId)
    {
        Oam_InitRestore(pLinkState);
        bCurCmdLineIsFirst = TRUE;
    }

    switch(dwMsgId)
    {
    case OAM_MSG_RESTORE_PREPARE_OK:
    case OAM_MSG_RESTORE_CONTINUE:
    case EV_OAM_CFG_CFG:
        if (EV_OAM_CFG_CFG == dwMsgId)
        {
            JID tSenderJid;
            MSG_COMM_OAM *ptMsgBuf = NULL;

            /*如果收到的不是brs返回的消息，不处理*/
            XOS_Sender(&tSenderJid);
            if (tSenderJid.dwJno != g_ptOamIntVar->jidProtocol.dwJno)
            { 
                XOS_SysLog(OAM_CFG_LOG_NOTICE, "RecvFromRestore: reveive EV_OAM_CFG_CFG, but not from protocol, discarded. [sender: %d brs: %d]\n", tSenderJid.dwJno, g_ptOamIntVar->jidProtocol.dwJno);
                return;
            }
            
            ptMsgBuf  = (MSG_COMM_OAM *)pMsgPara;
            Oam_RecvAppOfRestore(pLinkState, ptMsgBuf, tSenderJid, bIsSameEndian);
        }

        /*获取下一行*/
        wBufInLen_BytesOut = OAM_MAX_CMDLINE_LEN;
        dwGetCmdRtn = Oam_GetNextLineFromFile(aucCmdLineBuf, &wBufInLen_BytesOut);
        pcTmpBuf = OAM_LTrim(OAM_RTrim(aucCmdLineBuf));
        wStrLenTmp = OAM_CFG_MIN(strlen(pcTmpBuf)+1, OAM_MAX_CMDLINE_LEN);
        memcpy(aucCmdLineBuf, pcTmpBuf, wStrLenTmp);

        /*如果获取命令行失败，显示提示，退出存盘状态*/
        if (OAM_GET_NEXT_LINE_FAIL == dwGetCmdRtn)
        {
            CHAR *pcResult = "Get command line from txt file error\n";
            Oam_PrintRstrLog(pcResult);
            ExitRestoreState(pLinkState, OAM_TXT_READ_ERROR, pcResult);
            return;
        }

        /*如果是第一行，看是否能够从中获取版本号*/
        if (bCurCmdLineIsFirst && (strlen(aucCmdLineBuf) > 0))
        {
            BYTE ucTxtVersion = OAM_TXT_VERSION_0;

            bCurCmdLineIsFirst = FALSE;
            /*如果第一行是版本号，获取版本号后，读取下一行*/
            if(GetTxtVersionFromTxtLine(aucCmdLineBuf, &ucTxtVersion))
            {
                g_ptOamIntVar->bSupportStringWithBlank = (ucTxtVersion > OAM_TXT_VERSION_0) ? TRUE : FALSE;
                /* 给自己发送继续恢复的消息 ,这样就可以继续读取下一行*/
                XOS_SendAsynMsg(OAM_MSG_RESTORE_CONTINUE, NULL, 0, XOS_MSG_VER0, XOS_MSG_MEDIUM, &g_ptOamIntVar->jidSelf);
                return;
            }
        }

        if (strlen(aucCmdLineBuf) == 0)
        {
            /* 给自己发送继续恢复的消息 ,这样就可以继续读取下一行*/
            XOS_SendAsynMsg(OAM_MSG_RESTORE_CONTINUE, NULL, 0, XOS_MSG_VER0, XOS_MSG_MEDIUM, &g_ptOamIntVar->jidSelf);
        }
        else if ('!' == aucCmdLineBuf[0])
        {
            Oam_ParseExcalmatoryPointOfRestore(pLinkState);
        }
        else if ('$' == aucCmdLineBuf[0])
        {
            Oam_ParseDollarCharOfRestore(pLinkState);
        }
        else
        {
            Oam_ParseCmdLineOfRestore(pLinkState, aucCmdLineBuf);
        }

        /*如果存盘文件读完了，显示提示，退出存盘状态*/
        if (bNeedCheckEOF && OAM_GET_NEXT_LINE_OK_EOF == dwGetCmdRtn)
        {
            CHAR *pcResult = "Restore success!\n";
            ExitRestoreState(pLinkState, OAM_TXT_SUCCESS, pcResult);
            return;
        }

        break;

    case EV_TIMER_EXECUTE_NORMAL:
        {
            CHAR aucResult[MAX_CMDLINE_LEN*2] = {0};
            snprintf(aucResult,sizeof(aucResult),
                    "%s \nExecute time out!\n",
                    aucCmdLineBuf);
            Oam_PrintRstrLog("Execute time out!\n");
            /*?? 下面是测试代码，合入时候这种情况返回失败*/
            ExitRestoreState(pLinkState, OAM_TXT_TIMEOUT, aucResult);
        }
        break;
    default:
        break;
    }
}

/**************************************************************************
* 函数名称：Oam_InitNullLinkState
* 功能描述：初始化一个空的linkstate，用来恢复和背板存盘(此时没有用户登录)
* 访问的表：无
* 修改的表：无
* 输入参数：
  TYPE_LINK_STATE *pLinkState
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2008/04/23    V1.0    殷浩      创建
**************************************************************************/
void Oam_InitOneLinkForTxtRestore(TYPE_LINK_STATE *pLinkState)
{
    Oam_LinkInitializeForNewConn(pLinkState);
		
    /* 初始化为用户模式 */
    pLinkState->dwExeSeqNo = 0;
    Oam_LinkModeStack_Init(pLinkState);
    Oam_LinkModeStack_Push(pLinkState, CMD_MODE_USER);
    Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_TELNET);
    Oam_LinkSetLinkId(pLinkState, 1);
    pLinkState->ucSavedRight = DEFAULT_RITHT;
    pLinkState->ucUserRight = DEFAULT_ENABLE_RITHT;
}

/**************************************************************************
* 函数名称：Oam_PrintRstrLog
* 功能描述：打印恢复操作日志
* 访问的表：无
* 修改的表：无
* 输入参数：CHAR *pcLog - 日志字符串
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2008/04/23    V1.0    殷浩      创建
**************************************************************************/
void Oam_PrintRstrLog(CHAR *pcLog)
{
    /*如果是第一次打印，写入开始日期*/
    if (g_ptOamIntVar->bRestoreFirstPrint)
    {
        CHAR aucCurDate[20] = {0};
        CHAR aucCurTime[20] = {0};
        CHAR aucTmpLog[150] = {0};

        memset(&g_tRstrBgnTime, 0, sizeof(g_tRstrBgnTime));
        XOS_GetCurrentTime(&g_tRstrBgnTime);
        Oam_GetDateStrFromSysTime(&g_tRstrBgnTime, aucCurDate, sizeof(aucCurDate));
        Oam_GetTimeStrFromSysTime(&g_tRstrBgnTime, aucCurTime, sizeof(aucCurTime));

        snprintf(aucTmpLog,sizeof(aucTmpLog),
                "********************************************\n"
                "RESTORE BEGIN AT %s %s\n"
                "********************************************\n",
                aucCurDate, aucCurTime);
     /*   XOS_SysLog(OAM_CFG_LOG_DEBUG, aucTmpLog);*/

        g_ptOamIntVar->bRestoreFirstPrint = FALSE;
    }

    /*打印收到消息*/
    if (pcLog)
    {
        XOS_SysLog(OAM_CFG_LOG_NOTICE, pcLog);
    }
}

/**************************************************************************
* 函数名称：Oam_CloseRestoreLogFile
* 功能描述：结束打印恢复日志文件
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2008/04/23    V1.0    殷浩      创建
**************************************************************************/
void Oam_CloseRestoreLogFile(void)
{
#if 0
    T_Time tCurTime;
    CHAR aucCurDate[20] = {0};
    CHAR aucCurTime[20] = {0};
    CHAR aucTmpLog[400] = {0};

    memset(&tCurTime, 0, sizeof(tCurTime));
    XOS_GetCurrentTime(&tCurTime);
    Oam_GetDateStrFromSysTime(&tCurTime, aucCurDate, sizeof(aucCurDate));
    Oam_GetTimeStrFromSysTime(&tCurTime, aucCurTime, sizeof(aucCurTime));

    snprintf(aucTmpLog,sizeof(aucTmpLog),
            "********************************************\n"
            "RESTORE END AT %s %s\n"
            "%ld millisec used in total\n"
            "%ld millisec used on prepare process\n"
            "%ld millisec used on brs process\n"
            "%ld millisec used on parse process\n"
            "%ld millisec used on txt process\n"
            "%ld millisec used on exit process\n"
            "%d commands sended to brs\n"
            "********************************************\n",
            aucCurDate, aucCurTime,
            Oam_Duration(g_tRstrBgnTime,tCurTime),
            g_ptOamIntVar->dwMillSecOnPrepare,
            g_ptOamIntVar->dwMillSecOnBrs,
            g_ptOamIntVar->dwMillSecOnParse,
            g_ptOamIntVar->dwMillSecOnReadTxt,
            g_ptOamIntVar->dwMillSecOnExit,
            g_wCmdToBrsCnt);

    XOS_SysLog(OAM_CFG_LOG_NOTICE, aucTmpLog);
    g_ptOamIntVar->bRestoreFirstPrint = TRUE;
#endif
}

/**************************************************************************
* 函数名称：Oam_GetDateStrFromSysTime
* 功能描述：根据输入的time结构生成日期字符串 YYYY-MM-DD
* 访问的表：无
* 修改的表：无
* 输入参数：T_Time *ptInTime
* 输出参数：CHAR *pcOutSoftClock
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2008/04/23    V1.0    殷浩      创建
**************************************************************************/
BOOL Oam_GetDateStrFromSysTime(T_Time *ptInTime, CHAR *pcOutSoftClock, WORD32 dwBufSize)
{
    T_SysSoftClock tSoftClock;
    memset(&tSoftClock, 0, sizeof(tSoftClock));

    assert(pcOutSoftClock && ptInTime);
    if (!pcOutSoftClock || !ptInTime)
    {
        return FALSE;
    }

    XOS_TimeToClock(ptInTime, &tSoftClock);
    snprintf(pcOutSoftClock,dwBufSize,
            "%04d-%02d-%02d",
            tSoftClock.wSysYear,
            tSoftClock.ucSysMon,
            tSoftClock.ucSysDay);
    return TRUE;
}

/**************************************************************************
* 函数名称：Oam_GetTimeStrFromSysTime
* 功能描述：根据输入的time结构生成时间字符串 hh-mi-ss
* 访问的表：无
* 修改的表：无
* 输入参数：CHAR *pcLog - 日志字符串
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2008/04/23    V1.0    殷浩      创建
**************************************************************************/
BOOL Oam_GetTimeStrFromSysTime(T_Time *ptInTime, CHAR *pcOutSoftClock, WORD32 dwBufSize)
{
    T_SysSoftClock tSoftClock;
    memset(&tSoftClock, 0, sizeof(tSoftClock));

    assert(pcOutSoftClock && ptInTime);
    if (!pcOutSoftClock || !ptInTime)
    {
        return FALSE;
    }

    XOS_TimeToClock(ptInTime, &tSoftClock);
    snprintf(pcOutSoftClock,dwBufSize,
            "%02d:%02d:%02d %03d",
            tSoftClock.ucSysHour,
            tSoftClock.ucSysMin,
            tSoftClock.ucSysSec,
            tSoftClock.wMilliSec);
    return TRUE;
}

/*下面是日志功能暂未使用*/
#if 0
void InitLogFile(TYPE_LINK_STATE *pLinkState)
{
    T_Time tCurTime;
    DWORD dwRtn = 0;
    CHAR aucFileName[100] = {0};
    CHAR aucTmpDateStr[20] = {0};
    CHAR aucTmpTimeStr[20] = {0};
    CHAR aucTmpStr[500] = {0};
    XOS_FD fdFile;

    memset(&tCurTime, 0, sizeof(tCurTime));
    XOS_GetCurrentTime(&tCurTime);
    Oam_GetDateStrFromSysTime(&tCurTime, aucTmpDateStr);

    dwRtn = XOS_CreateDirectory(OAM_CFG_PRTCL_CFG_LOG_DIR);
    if (XOS_SUCCESS != dwRtn && XOS_FILE_ERR_DIR_EXIST != dwRtn)
    {
        pLinkState->bLogFileOK = FALSE;
        return;
    }

    /*create the log file*/
    snprintf(aucFileName, "%s/[%s]%s.log",
            OAM_CFG_PRTCL_CFG_LOG_DIR,pLinkState->sUserName,aucTmpStr);

    dwRtn = XOS_OpenFile(aucFileName, XOS_WRONLY|XOS_CREAT|XOS_TRUNC, &fdFile);
    if (XOS_SUCCESS != dwRtn)
    {
        pLinkState->bLogFileOK = FALSE;
        return;
    }
    Oam_GetTimeStrFromSysTime(&tCurTime, aucTmpTimeStr);
    snprintf(aucTmpStr,
            "********************************************************************************\n"\
            " Log of config agent, BEGIN at %s %s .\n"\
            "********************************************************************************\n",
            aucTmpDateStr, aucTmpTimeStr);
    dwRtn = Oam_WriteStrToFile(&fdFile, aucTmpStr);
    if (OAM_SAVE_SUCESS != dwRtn )
    {
        pLinkState->bLogFileOK = FALSE;
        return;
    }

    XOS_CloseFile(fdFile);
    pLinkState->bLogFileOK = TRUE;
}
void SaveLogFile(TYPE_LINK_STATE *pLinkState)
{
    T_Time tCurTime;
    DWORD dwRtn = 0;
    CHAR aucFileName[100] = {0};
    CHAR aucTmpDateStr[20] = {0};
    CHAR aucTmpStr[500] = {0};
    XOS_FD fdFile;
    CHAR strInfo[1024] = {0};
    int iStep = 0;
    int i = 0;

    if (!pLinkState->bLogFileOK)
    {
        return;
    }
    memset(&tCurTime, 0, sizeof(tCurTime));
    XOS_GetCurrentTime(&tCurTime);
    Oam_GetDateStrFromSysTime(&tCurTime, aucTmpDateStr);

    /*create the log file*/
    snprintf(aucFileName, "%s/[%s]%s.log",
            OAM_CFG_PRTCL_CFG_LOG_DIR,pLinkState->sUserName,aucTmpStr);

    dwRtn = XOS_OpenFile(aucFileName, XOS_WRONLY, &fdFile);
    if (XOS_FILE_ERR_FILE_NOT_EXIST == dwRtn)
    {/*如果文件不存在,认为是跨一日,获取前一天的日期*/
        T_Time tPreTime;
        memset(&tPreTime, 0, sizeof(tPreTime));
        memcpy(&tPreTime, &tCurTime, sizeof(tPreTime));
        tPreTime.dwSecond -= 24*3600;
        Oam_GetDateStrFromSysTime(&tPreTime, aucTmpDateStr);
        snprintf(aucFileName, "%s/[%s]%s.log",
                OAM_CFG_PRTCL_CFG_LOG_DIR,pLinkState->sUserName,aucTmpStr);

        dwRtn = XOS_OpenFile(aucFileName, XOS_WRONLY, &fdFile);
    }
    if (XOS_SUCCESS != dwRtn)
    {
        return;
    }

    for(i = 0; i < pLinkState->tCmdLine.wWordSum; i++)
    {
        iStep += snprintf(strInfo + iStep,"%s ",
                         pLinkState->tCmdLine.sCmdWord[i]);
    }
    strInfo[iStep] = '\n';

    dwRtn = Oam_WriteStrToFile(&fdFile, strInfo);
    if (OAM_SAVE_SUCESS != dwRtn )
    {
        return;
    }

    XOS_CloseFile(fdFile);
}
void ExitLogFile(TYPE_LINK_STATE *pLinkState)
{
    T_Time tCurTime;
    DWORD dwRtn = 0;
    CHAR aucFileName[100] = {0};
    CHAR aucTmpDateStr[20] = {0};
    CHAR aucTmpTimeStr[20] = {0};
    CHAR aucTmpStr[500] = {0};
    XOS_FD fdFile;

    memset(&tCurTime, 0, sizeof(tCurTime));
    XOS_GetCurrentTime(&tCurTime);
    Oam_GetDateStrFromSysTime(&tCurTime, aucTmpDateStr);

    snprintf(aucFileName, "%s/[%s]%s.log",
            OAM_CFG_PRTCL_CFG_LOG_DIR,pLinkState->sUserName,aucTmpStr);

    dwRtn = XOS_OpenFile(aucFileName, XOS_WRONLY, &fdFile);
    if (XOS_FILE_ERR_FILE_NOT_EXIST == dwRtn)
    {/*如果文件不存在,认为是跨一日,获取前一天的日期*/
        T_Time tPreTime;
        memset(&tPreTime, 0, sizeof(tPreTime));
        memcpy(&tPreTime, &tCurTime, sizeof(tPreTime));
        tPreTime.dwSecond -= 24*3600;
        Oam_GetDateStrFromSysTime(&tPreTime, aucTmpDateStr);
        snprintf(aucFileName, "%s/[%s]%s.log",
                OAM_CFG_PRTCL_CFG_LOG_DIR,pLinkState->sUserName,aucTmpStr);

        dwRtn = XOS_OpenFile(aucFileName, XOS_WRONLY, &fdFile);
    }
    if (XOS_SUCCESS != dwRtn)
    {
        pLinkState->bLogFileOK = FALSE;
        return;
    }

    Oam_GetTimeStrFromSysTime(&tCurTime, aucTmpTimeStr);
    snprintf(aucTmpStr,
            "********************************************************************************\n"\
            "Log of config agent, END at %s %s .\n"\
            "********************************************************************************\n",
            aucTmpDateStr, aucTmpTimeStr);
    dwRtn = Oam_WriteStrToFile(&fdFile, aucTmpStr);
    if (OAM_SAVE_SUCESS != dwRtn )
    {
        pLinkState->bLogFileOK = FALSE;
        return;
    }

    XOS_CloseFile(fdFile);
}
#endif

void Oam_InitRestore(TYPE_LINK_STATE *pLinkState)
{
    CHAR aucLogStr[OAM_MAX_RESTORE_LOG_LEN] = {0};
    
    Oam_CfgCloseAllLink(map_szPrtclRestoreNotify);
    Oam_InitOneLinkForTxtRestore(pLinkState);

    /*切换模式到协议栈配置模式*/
    Oam_LinkModeStack_Init(pLinkState);
    Oam_LinkModeStack_Push(pLinkState, CMD_MODE_PROTOCOL);
    Oam_PrintRstrLog("\nInitial linkstate for restore, and change config mode to protocol\n");
    memset(aucLogStr, 0, sizeof(aucLogStr));
    snprintf(aucLogStr, sizeof(aucLogStr),"Current mode id is %d\n-------------------------\n",Oam_GetCurModeId(pLinkState));
    Oam_PrintRstrLog(aucLogStr);
}

void Oam_RecvAppOfRestore(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *ptRcvMsg, JID tJid, BOOLEAN bIsSameEndian)
{
    CHAR aucLogStr[OAM_MAX_RESTORE_LOG_LEN] = {0};
    T_Time tCurTime;
    static T_Time tTimeSendToBrs;
	
     /* added by fls on 2009-06-29 (CRDCM00197905 SSC2单板联调，OAM发送字节序固定为小尾字节序，没有按照OMP的字节序发送) */
    /*if (Oam_CfgIsNeedInvertOrder())*/
    if (!bIsSameEndian)
    {
        CfgMsgTransByteOder(ptRcvMsg);
    }
    /* end of add */   
	
    memset(aucLogStr, 0, sizeof(aucLogStr));
    snprintf(aucLogStr, sizeof(aucLogStr),"[brs return] cmdid = 0x%x, returncode = %u\n", (unsigned)ptRcvMsg->CmdID, (unsigned)ptRcvMsg->ReturnCode);
    Oam_PrintRstrLog(aucLogStr);

    XOS_GetCurrentTime(&tCurTime);
    g_ptOamIntVar->dwMillSecOnBrs += Oam_Duration(tTimeSendToBrs, tCurTime);

    if (ptRcvMsg->ucOamTag != OAMCFG_TAG_TXT_RESTORE)
    {
        XOS_SysLog(OAM_CFG_LOG_DEBUG, "Oam_RecvAppOfRestore: Not restore command!\n");
        return;
    }

    /* 收到应用的返回结果之后，杀掉进度上报等待定时器 */
    Oam_KillCmdPlanTimer(pLinkState);
    
    Oam_RecvFromApp(pLinkState,ptRcvMsg);

    memset(aucLogStr, 0, sizeof(aucLogStr));
    snprintf(aucLogStr, sizeof(aucLogStr),"Current modeid = %d\n", Oam_GetCurModeId(pLinkState));
    Oam_PrintRstrLog(aucLogStr);
}

void Oam_ParseDollarCharOfRestore(TYPE_LINK_STATE *pLinkState)
{
    BYTE bTmpLastData=2;
    CHAR aucLogStr[OAM_MAX_RESTORE_LOG_LEN] = {0};
    
    if ((!Oam_LinkModeStatck_IsEmpty(pLinkState)) && 
            ((Oam_GetCurModeId(pLinkState) == CMD_MODE_ADDRIPV4) || (Oam_GetCurModeId(pLinkState) == CMD_MODE_ADDRVPNV4)))
    {
        if ((pLinkState->bModeStackTop > 2) && 
            (pLinkState->bModeID[pLinkState->bModeStackTop - 2]) == CMD_MODE_BGP)
        {
            memset(&pLinkState->LastData, 0, sizeof(pLinkState->LastData));
            memcpy(&pLinkState->LastData,(BYTE*)&bTmpLastData,sizeof(BYTE));
        }
    }

    while (!Oam_LinkModeStatck_IsEmpty(pLinkState))
    {
        if (Oam_GetCurModeId(pLinkState) == CMD_MODE_BGP)
            break;

        Oam_LinkModeStack_Pop(pLinkState);            
    }

    Oam_PrintRstrLog("\n!\nChange config mode to BGP\n");
    memset(aucLogStr, 0, sizeof(aucLogStr));
    snprintf(aucLogStr, sizeof(aucLogStr),"Current mode id is %d\n",Oam_GetCurModeId(pLinkState));
    Oam_PrintRstrLog(aucLogStr);
    /* 给自己发送继续恢复的消息 ,这样就可以继续读取下一行*/
    XOS_SendAsynMsg(OAM_MSG_RESTORE_CONTINUE, NULL, 0, XOS_MSG_VER0, XOS_MSG_MEDIUM, &g_ptOamIntVar->jidSelf);
}

void Oam_ParseExcalmatoryPointOfRestore(TYPE_LINK_STATE *pLinkState)
{
    CHAR aucLogStr[OAM_MAX_RESTORE_LOG_LEN] = {0};
    
    /*存盘文件遇到!，回到协议配置模式*/
    while (!Oam_LinkModeStatck_IsEmpty(pLinkState))
    {
        if (Oam_GetCurModeId(pLinkState) == CMD_MODE_CONFIG)
        {
            break;
        }
        Oam_LinkModeStack_Pop(pLinkState);
    }
    Oam_PrintRstrLog("\n!\nChange config mode to protocol\n");
    memset(aucLogStr, 0, sizeof(aucLogStr));
    snprintf(aucLogStr, sizeof(aucLogStr),"Current mode id is %d\n",Oam_GetCurModeId(pLinkState));
    Oam_PrintRstrLog(aucLogStr);
    /* 给自己发送继续恢复的消息 ,这样就可以继续读取下一行*/
    XOS_SendAsynMsg(OAM_MSG_RESTORE_CONTINUE, NULL, 0, XOS_MSG_VER0, XOS_MSG_MEDIUM, &g_ptOamIntVar->jidSelf);
}

void Oam_ParseCmdLineOfRestore(TYPE_LINK_STATE *pLinkState, CHAR *pCmdLine)
{
    BOOL bNeedCheckEOF = TRUE;
    CHAR aucLogStr[OAM_MAX_RESTORE_LOG_LEN] = {0};
    T_Time tCurTime;
    static T_Time tTimeSendToBrs;
    TYPE_MATCH_RESULT *pMatchResult  = NULL;
    
    bNeedCheckEOF = FALSE; /*如果取到命令，不检查文件是否结束，等brs回复后检查*/
    memset(aucLogStr, 0, sizeof(aucLogStr));
    snprintf(aucLogStr, OAM_MAX_RESTORE_LOG_LEN, "\n%s\n", pCmdLine);

    Oam_PrintRstrLog(aucLogStr);

    Oam_InptFreeBuf(pLinkState->ucLinkID);
    pLinkState->dwExeSeqNo++;

    XOS_GetCurrentTime(&tCurTime);

    /*解析执行命令字符串*/
    ParseCmdLine(pLinkState, pCmdLine, DM_NORMAL, TRUE);

    XOS_GetCurrentTime(&tTimeSendToBrs);
    g_ptOamIntVar->dwMillSecOnParse += Oam_Duration(tCurTime, tTimeSendToBrs);

    pMatchResult  = &(pLinkState->tMatchResult);
    /*如果解析命令行失败，显示提示，退出存盘状态*/
    if (pMatchResult->wParseState != PARSESTATE_FINISH_PARSE)
    {
        CHAR aucResult[MAX_CMDLINE_LEN*2] = {0};
        snprintf(aucResult,sizeof(aucResult),
                "%s \n[Parse error, check whether this command match with dat file!]\n",
                pCmdLine);
        Oam_PrintRstrLog("[Parse error, check whether this command match with dat file!]\n");
        ExitRestoreState(pLinkState, OAM_TXT_PARSE_ERROR, aucResult);
        return;
    }
    g_wCmdToBrsCnt++;
}

/**************************************************************************
* 函数名称：BOOLEAN  GetTxtVersionFromTxtLine
* 功能描述：从命令行获取txt版本号，如果是版本信息行的话
* 访问的表：无
* 修改的表：无
* 输入参数：
    pcTxtLine --命令行    
* 输出参数：
    pucVersion -- txt版本号
* 返 回 值：
true -- 获取成功
false --获取失败
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/06/25    V1.0    傅龙锁      创建
**************************************************************************/
static BOOLEAN  GetTxtVersionFromTxtLine(CHAR *pcTxtLine, BYTE *pucVersion)
{
    int  iVersion;
    SWORD32 sdwRet = 0;
    
    if (!pcTxtLine || 
        !pucVersion ||
        (strlen(pcTxtLine) <= strlen(OAM_VERSION_IN_SAVEFILE)))
    {
        return FALSE;
    }
    
    if (strncmp(pcTxtLine, OAM_VERSION_IN_SAVEFILE, strlen(OAM_VERSION_IN_SAVEFILE)) != 0)
    {
        return FALSE;
    }

    sdwRet = sscanf(pcTxtLine,"TXT_VERSION = %3d", &iVersion);
    if (sdwRet < 0)
    {
        return FALSE;
    }
    *pucVersion = (BYTE)iVersion;

    return TRUE;
}
/*lint +e762*/
/*lint +e1717*/
/*lint +e413*/
/*lint +e1917*/
/*lint +e931*/
/*lint +e58*/
/*lint +e727*/
/*lint +e666*/

