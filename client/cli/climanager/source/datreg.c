/*********************************************************************
* 版权所有 (C)2008, 深圳市中兴通讯股份有限公司。
*
* 文件名称：DatReg.c
* 文件标识：
* 内容摘要：DAT数据区注册函数
* 其它说明：

* 当前版本：
* 作    者：kong.wei2   134463
* 完成日期：2008.6.11
*
* 修改记录1：
*    修改日期：
*    版 本 号：
*    修 改 人：
*    修改内容：
* 修改记录2：…
**********************************************************************/
#include "includes.h"

/*******************************************************************************/
static BOOL AppendDatLink(T_OAM_Cfg_Dat *ptCfgDat, BOOLEAN bRestore);
/*static BOOL DeleteDatLink(const JID);*/
static WORD16 SearchDatLink(const JID);
static void OAM_CFGAppendChildNodes(WORD16 parentofglobal, T_OAM_Cfg_Dat* cfgDat, WORD16 firstchild, TYPE_TREE_NODE* treenode_array);
static BOOLEAN RegisterDat(T_OAM_Cfg_Dat *pCfgDat, const JID tJid, BOOLEAN bRestore);
static void Int_Inner_Error_Init(T_OAM_Cfg_Dat *oamCfgDat);
/*******************************************************************************/
BYTE OAM_CFGDatRegisterForRestore(const JID tJid, const WORD32 dwDatFuncType)
{
    INT32 iRet = 0, i = 0;
    CHAR aucTmpFileName[FILE_PATH_LEN+FILE_NAME_LEN+2] = {0};
    T_Cli_LangType tDatLangType;

    if (FUNC_INVALID_DAT == dwDatFuncType)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "OAM_CFGDatRegister: invalid functype, functype = %d\n", dwDatFuncType);
        return OAM_CLI_FAIL;
    }

        /*如果要加载的是新dat脚本，从数据区中的版本信息里面找*/
        for(i = 0; i < g_ptOamIntVar->wDatSum; i++)
        {
        if (g_ptOamIntVar->tDatVerInfo[i].tFileInfo.wFuncType == dwDatFuncType)
            {
                break;
            }
        }

        if (i == g_ptOamIntVar->wDatSum)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, "OAM_CFGDatRegister: get file name by function type failed, functype = %d\n", dwDatFuncType);
            return OAM_CLI_FAIL;
        }

    XOS_snprintf(aucTmpFileName, sizeof(aucTmpFileName),"%s/%s",
        g_ptOamIntVar->tDatVerInfo[i].tFileInfo.acVerFilePath,
        g_ptOamIntVar->tDatVerInfo[i].tFileInfo.acVerFileName);

    memset(&g_ptOamIntVar->tCfgdat, 0, sizeof(g_ptOamIntVar->tCfgdat));
    iRet = ReadCmdScript(aucTmpFileName, &g_ptOamIntVar->tCfgdat, &tDatLangType);
    if (READERR_SUCCESS != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "OAM_CFGDatRegister: parse dat file failed, datfile = %s, ReadCmdScript return = %d\n", aucTmpFileName, iRet);
        return OAM_CLI_FAIL;
    }

    /*重新初始化全局dat内存*/
    if (!OAM_CFGInitGlobalDatMemForRestore(g_ptOamIntVar->tCfgdat.nModeSum, g_ptOamIntVar->tCfgdat.nTreeNodeSum))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "Failed to initialize global dat memory! all jobs can't register");
        return OAM_CLI_FAIL;
    }
    
    /* 注册 */
    if (!RegisterDat(&g_ptOamIntVar->tCfgdat, tJid, TRUE))
    {
        if (g_wRegDatCount > 0)
        {
            /*注册失败但是已经添加到g_tDatLink中，需要从中移出*/
            if ((g_tDatLink[g_wRegDatCount-1].datIndex.wCRC == g_ptOamIntVar->tCfgdat.wCRC) &&
                (g_tDatLink[g_wRegDatCount-1].datIndex.tJid.dwJno == g_ptOamIntVar->tCfgdat.tJid.dwJno))
            {
                memset(&(g_tDatLink[g_wRegDatCount-1].datIndex), 0, sizeof(g_tDatLink[g_wRegDatCount-1].datIndex));
                g_wRegDatCount--;
            
            }
        }
        OAM_CFGClearCmdScript(&g_ptOamIntVar->tCfgdat);
        
        return OAM_CLI_FAIL;
    }

    XOS_SysLog(OAM_CFG_LOG_NOTICE, 
                         "OAM_CFGDatRegister: register success, jid = %d, functype = %d\n datfile = %s",
                         tJid.dwJno,dwDatFuncType, aucTmpFileName);

    return OAM_CLI_SUCCESS;
    
}

BYTE OAM_CFGDatRegister(const JID tJid, const WORD32 dwDatFuncType)
{
    CHAR aucTmpFileName[FILE_PATH_LEN+FILE_NAME_LEN+2] = {0};
    T_OAM_Cfg_Dat tCfgdat;
    INT32 iRet = 0;
    INT32 i = 0;

    if (FUNC_INVALID_DAT == dwDatFuncType)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "######[OAM_CFGDatRegister]: invalid functype, functype = %d\n", dwDatFuncType);
        return OAM_CLI_FAIL;
    }

    for(i = 0; i < g_ptOamIntVar->wDatSum; i++)
    {
        if (g_ptOamIntVar->tDatVerInfo[i].tFileInfo.wFuncType == dwDatFuncType)
        {
            break;
        }
    }

    if (i == g_ptOamIntVar->wDatSum)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "######[OAM_CFGDatRegister]: get file name by function type failed, functype = %d\n", dwDatFuncType);
        return OAM_CLI_FAIL;
    }
	
    XOS_snprintf(aucTmpFileName, sizeof(aucTmpFileName),"%s/%s",
            g_ptOamIntVar->tDatVerInfo[i].tFileInfo.acVerFilePath,
            g_ptOamIntVar->tDatVerInfo[i].tFileInfo.acVerFileName);

    memset(&tCfgdat, 0, sizeof(tCfgdat));
    memset(&(g_ptOamIntVar->tDatVerInfo[i].tLang), 0, sizeof(g_ptOamIntVar->tDatVerInfo[i].tLang));
    iRet = ReadCmdScript(aucTmpFileName, &tCfgdat, &(g_ptOamIntVar->tDatVerInfo[i].tLang));
    if (READERR_SUCCESS != iRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "######[OAM_CFGDatRegister]: parse dat file failed, datfile = %s, ReadCmdScript return = %d\n", aucTmpFileName, iRet);
        return OAM_CLI_FAIL;
    }

    /* 注册 */
    if (!RegisterDat(&tCfgdat, tJid, FALSE))
    {
        if (g_wRegDatCount > 0)
        {
            /*注册失败但是已经添加到g_tDatLink中，需要从中移出*/
            if ((g_tDatLink[g_wRegDatCount-1].datIndex.wCRC == tCfgdat.wCRC) &&
                    (g_tDatLink[g_wRegDatCount-1].datIndex.tJid.dwJno == tCfgdat.tJid.dwJno))
            {
                memset(&(g_tDatLink[g_wRegDatCount-1].datIndex), 0, sizeof(g_tDatLink[g_wRegDatCount-1].datIndex));
                g_wRegDatCount--;

            }
        }
        OAM_CFGClearCmdScript(&tCfgdat);
        return OAM_CLI_FAIL;
    }
	
    if (OAM_CFG_GETCOMMONINFO_FUNC_TYPE == dwDatFuncType)
    {
        /*     memcpy(&g_ptOamIntVar->oamCfgDat, &tCfgdat, sizeof(tCfgdat));*/
        /* 常用字符串存放位置初始化*/
        Int_Inner_Error_Init(&tCfgdat);
    }
    XOS_SysLog(OAM_CFG_LOG_ERROR,"######[OAM_CFGDatRegister]: register success, jid = %d, functype = %d\n",tJid.dwJno,dwDatFuncType);
    XOS_SysLog(OAM_CFG_LOG_ERROR,"######[OAM_CFGDatRegister]: datfile = %s\n", aucTmpFileName);

    return OAM_CLI_SUCCESS;

}


BYTE OAM_CFGDatUnregister(const JID tJid)
{
    BYTE bRet = OAM_CLI_SUCCESS;

    T_DAT_LINK *pCfgDat = Oam_CliSearchDat(tJid);
    if (pCfgDat)
    {
        pCfgDat->datIndex.bIsValid = FALSE;
    }
    else
    {
        bRet = OAM_CLI_FAIL;
    }
    return bRet;
}

static BOOLEAN RegisterDat(T_OAM_Cfg_Dat *pCfgDat, const JID tJid, BOOLEAN bRestore)
{
  /*ddm功能需要多个功能类型版本对应一个jid*/
#if 0
    if (!bRestore)
    {
        /*如果内存中已经有jid相关的dat，重新置为有效即可*/
        T_DAT_LINK *pTmpCfgDat = Oam_CliSearchDat(tJid);
        if (pTmpCfgDat)
        {
            pTmpCfgDat->datIndex.bIsValid = TRUE;
            XOS_SysLog(OAM_CFG_LOG_NOTICE, "RegisterDat: input dat exist in datlink, reset valid ok!\n");
            return TRUE;
        }
    }
#endif
    /* 入参校验 */
    if (pCfgDat == NULL)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "RegisterDat: input dat is NULL, register fail!\n");
        return FALSE;
    }

    pCfgDat->tJid = tJid;
    pCfgDat->bIsValid = TRUE;

    /*下面函数会把pCfgDat拷贝到dat链表，因此isvalid要在上面就赋值*/
    if (!AppendDatLink(pCfgDat, bRestore))
    {
        pCfgDat->bIsValid = FALSE;
        XOS_SysLog(OAM_CFG_LOG_ERROR, "RegisterDat: AppendDatLink error, register fail!\n");
        return FALSE;
    }
    else
    {
        XOS_SysLog(OAM_CFG_LOG_NOTICE, "RegisterDat: register dat success!\n");
    }
	
    return TRUE;
}

T_DAT_LINK * Oam_CliSearchDat(const JID jid)
{
    /*消除pcLint，将index修改为index1*/
    WORD16 index1 = SearchDatLink(jid);
    if (index1 < g_wRegDatCount)
    {
        return g_tDatLink+index1;
    }
    else
    {
        return NULL;
    }
}

BOOL AppendDatLink(T_OAM_Cfg_Dat* ptCfgDat, BOOLEAN bRestore)
{
    if (!bRestore)
    {
        WORD16 wMaxDatCnt = g_ptOamIntVar->wDatSum;
 
        /* 溢出检验 */
        if (g_wRegDatCount >= wMaxDatCnt)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, "AppendDatLink: g_wRegDatCount(%d) > g_ptOamIntVar->wDatSum(%d), append dat failed",
                                 g_wRegDatCount, wMaxDatCnt);
            return FALSE;
        }

        /* 插入数据 */
	
        memset(&(g_tDatLink[g_wRegDatCount].datIndex), 0, sizeof(g_tDatLink[g_wRegDatCount].datIndex));
        memcpy(&(g_tDatLink[g_wRegDatCount].datIndex) , ptCfgDat, sizeof(g_tDatLink[g_wRegDatCount].datIndex));
    }

    /* 遍历模式节点 */
    {
        DWORD i;

        for (i=1;i < ptCfgDat->nModeSum;i++)
        {
            /* 添加模式，得到模式对应的树节点 */
            WORD16 modenode = OAM_CFGAppendMode(&ptCfgDat->tMode[i], bRestore);

            if ((modenode != 0) && (ptCfgDat->tMode[i].wTreeRootPos != 0))
            {
                /* 添加子节点 */
                /* 如果某个节点有子节点，OAM_CFGAppendChildNodes函数将递归添加子节点 */
                if (bRestore)
                    OAM_CFGAppendChildNodes(modenode, ptCfgDat,
                                            ptCfgDat->tCmdTree[ptCfgDat->tMode[i].wTreeRootPos].wNextSonPos, ptCfgDat->tCmdTree);
                else
                    OAM_CFGAppendChildNodes(modenode, &g_tDatLink[g_wRegDatCount].datIndex,
                                            ptCfgDat->tCmdTree[ptCfgDat->tMode[i].wTreeRootPos].wNextSonPos, ptCfgDat->tCmdTree);
                    
            }
        }
    }

    g_wRegDatCount++;

    return TRUE;
}

#if 0
BOOL DeleteDatLink(const JID jid)
{
    /* 查找指定所在的DAT索引 */
    WORD16 index = SearchDatLink(jid);
    if (index < g_wRegDatCount)
    {
        /* 删除命令 */
        DeleteChildNodes(&g_tDatLink[index].datIndex);
        /*UnRegisterCmd(jid);*/

        /* 删除节点，并且清除数据 */
        g_wRegDatCount--;
        if (index != g_wRegDatCount)
        {
            g_tDatLink[index] = g_tDatLink[g_wRegDatCount];
        }
        memset(&g_tDatLink[g_wRegDatCount], 0, sizeof(g_tDatLink[g_wRegDatCount]));
    }

    return TRUE;
}
#endif

WORD16 SearchDatLink(const JID jid)
{
    WORD16 index1;
    for (index1=0;index1<g_wRegDatCount;index1++)
    {
        if (EqualJID(&g_tDatLink[index1].datIndex.tJid, &jid))
        {
            break;
        }
    }
    return index1;
}

#if 0
void OAM_CFGAppendChildNodes(WORD16 parentofglobal, T_OAM_Cfg_Dat* cfgDat, WORD16 firstchild, TYPE_TREE_NODE* treenode_array)
{
    WORD16 treenodeofglobal;
    WORD16 nodetoinsert = firstchild;

    /* 插入第一个节点 */
    treenodeofglobal = OAM_CFGAppendChildNode(parentofglobal, &treenode_array[nodetoinsert], cfgDat);
    /* 递归 */
    if (treenode_array[nodetoinsert].wNextSonPos != 0)
    {
        OAM_CFGAppendChildNodes(treenodeofglobal, cfgDat, treenode_array[nodetoinsert].wNextSonPos, treenode_array);
    }
    /* 下一个需要插入的节点 */
    nodetoinsert = treenode_array[nodetoinsert].wNextNodePos;

    /* 插入兄弟节点 */
    while (nodetoinsert != 0)
    {
        /* 插入节点 */
        treenodeofglobal = AppendSiblingNode(treenodeofglobal, &treenode_array[nodetoinsert], cfgDat);
        /* 递归 */
        if (treenode_array[nodetoinsert].wNextSonPos != 0)
        {
            OAM_CFGAppendChildNodes(treenodeofglobal, cfgDat, treenode_array[nodetoinsert].wNextSonPos, treenode_array);
        }
        /* 下一个需要插入的节点 */
        nodetoinsert = treenode_array[nodetoinsert].wNextNodePos;
    }
}
#else
void OAM_CFGAppendChildNodes(WORD16 parentofglobal, T_OAM_Cfg_Dat* cfgDat, WORD16 firstchild, TYPE_TREE_NODE* treenode_array)
{
    WORD16 treenodeofglobal;
    WORD16 nodetoinsert = firstchild;

    /* 插入兄弟节点 */
    while (nodetoinsert != 0)
    {
        /* 插入节点 */
        treenodeofglobal = OAM_CFGAppendChildNode(parentofglobal, &treenode_array[nodetoinsert], cfgDat);
        /* 递归 */
        if (treenode_array[nodetoinsert].wNextSonPos != 0)
        {
            OAM_CFGAppendChildNodes(treenodeofglobal, cfgDat, treenode_array[nodetoinsert].wNextSonPos, treenode_array);
        }
        /* 下一个需要插入的节点 */
        nodetoinsert = treenode_array[nodetoinsert].wNextNodePos;
    }
}

/**************************************************************************
* 函数名称：VOID Inner_String_Init
* 功能描述：常用字符串存放位置初始化
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
static void Int_Inner_Error_Init(T_OAM_Cfg_Dat *oamCfgDat)
{
        CLI_szCLIHelp0           = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)1);
        CLI_szCLIHelp1           = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)2);
        CLI_szCLIHelp2           = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)3);
        CLI_szCLIHelp3           = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)4);
        CLI_szCLIHelp4           = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)5);
        CLI_szCLIHelp5           = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)6);
        CLI_szCLIHelp6           = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)7);
        CLI_szCLIHelp7           = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)8);
        CLI_szCLIHelp8           = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)9);
        CLI_szCLIHelp9           = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)10);
        CLI_szCLIHelp10          = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)11);
        CLI_szCLIHelp11          = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)12);
        CLI_szCLIHelp12          = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)13);
        CLI_szCLIHelp13          = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)14);
        CLI_szCLIHelp14          = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)15);
        CLI_szCLIHelp15          = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)16);
        CLI_szCLIHelp16          = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)17);
        CLI_szCLIHelp17          = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)18);
        CLI_szCLIHelp18          = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)19);
        CLI_szCLIHelp19          = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)20);
        CLI_szCLIHelp20          = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)21);
        CLI_szCLIHelp21          = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)22);
        CLI_szCLIHelp22          = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)23);
        CLI_szCLIHelp23          = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)24);
        CLI_szCLIHelp24          = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)25);
        CLI_szCLIHelp25          = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)26);

        Hdr_szEnterString        = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)200);
        Hdr_szUserName           = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)201);
        Hdr_szPassword           = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)202);
        Hdr_szCmdNoCmdTip        = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)203);
        Hdr_szTelInputText       = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)204);
        Hdr_szOsBadUserName      = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)205);
        Hdr_szOsBadPassword      = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)206);
        Hdr_szCanNotEnterConfig  = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)207);
        Hdr_szEnterConfigTip     = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)208);
        Hdr_szEnterConfigWarning = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)209);
        Hdr_szCanNotLogin        = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)210);
        Hdr_szExeNoReturn        = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)211);
        Hdr_szExeNoDisplay       = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)212);
        Hdr_szExeTimeout         = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)213);
        Hdr_szTelRequestOutMode  = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)214);
        Hdr_szCanNotChangMode    = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)215);
        Hdr_szCeasedTheCmdTip    = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)216);
        Hdr_szDisplayScriptErr   = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)217);
        Hdr_szUpdateDatTip       = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)218);
        Hdr_szNeNotSupportOnlineCfg = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)219);
        Hdr_szSavePrtclCfgOK     = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)220);
        Hdr_szSavePrtclCfgFail   = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)221);
        Hdr_szOpenSaveFileErr    = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)222);
        Hdr_szInsertCrcErr       = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)223);
        Hdr_szMSChangeoverTip    = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)224);
        Hdr_szCommandMutexTip    = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)225);
        Hdr_szSaveMutexTip       = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)226);
        Hdr_szDatUpdateMutexTip  = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)227);
        Hdr_szSlaveSaveOk        = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)228);
        Hdr_szSlaveSaveFail      = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)229);
        Hdr_szSlaveAckTimeout    = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)231);
        Hdr_szSaveZDBOK          = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)232);
        Hdr_szSaveZDBFail        = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)233);
        Hdr_szSaveZDBErrSaveBothPath = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)234);
        Hdr_szSaveZDBErrMainPath = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)235);
        Hdr_szSaveZDBErrBackup   = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)236);
        Hdr_szSaveZDBErrLocalSaving = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)237);
        Hdr_szSaveZDBErrMSSaving = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)238);
        Hdr_szSaveZDBErrOther    = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)239);

        Hdr_szNotAdmin              = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)240);
        Hdr_szCannotDelAdmin        = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)241);
        Hdr_szUserNotExists         = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)242);
        Hdr_szUserBeenDeleted       = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)243);
        Hdr_szCmdExecSucc           = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)244);
        Hdr_szCmdExecFailed         = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)245);
        Hdr_szCannotCreateAdmin     = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)246);
        Hdr_szUserDoesExists        = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)247);
        Hdr_szPasswordRule          = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)248);
        Hdr_szPwdNotMeetRule        = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)249);
        Hdr_szTwoInputPwdNotSame    = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)250);
        Hdr_szConfUserFull          = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)251);
        Hdr_szViewUserFull          = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)252);
        Hdr_szUserLocked            = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)253);
        Hdr_szCannotSetAdminRole    = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)254);
        Hdr_szUserOnline            = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)255);
        Hdr_szNeedNotSetRole        = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)256);
        Hdr_szCannotSetOtherUserPwd = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)257);
        Hdr_szUserNotLocked         = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)258);
        Hdr_szNoPermission          = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)259);
        Hdr_szSaveZDBErrNoTblSave      = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)260);
        Hdr_szSaveZDBErrReadFileErr    = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)261);
        Hdr_szSaveZDBErrTblNotNeedSave = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)262);
        Hdr_szSaveZDBErrPowerOnLoading = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)263);
        Hdr_szSaveZDBErrFtpLoading     = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)264);
        Hdr_szSaveZDBErrAppFtpLoading  = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)265);
        Hdr_szSaveZDBErrIOBusy         = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)266);
        Hdr_szSaveZDBErrInvalidHandle  = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)267);

        map_szAppMsgDatalengthErr     = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)270);
        map_szUserBeenLocked     = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)271);
        map_szUserOrPwdErr     = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)272);
        map_szGotoSupperModeErr     = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)273);
        map_szExePlanFmtStr     = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)274);
        map_szScriptReloaded     = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)275);
        map_szCeaseCmdFailed     = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)276);
        map_szCmdSendAgainFailed     = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)277);
        map_szDisposeCmdFailed     = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)278);
        map_szExeCmdFailed     = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)279);
        map_szPrtclRestoreNotify     = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)280);

        Err_szExeMsgHead         = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)400);
        Err_szReturnCode         = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)401);
        Err_szLinkChannel        = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)402);
        Err_szSeqNo              = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)403);
        Err_szLastPacket         = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)404);
        Err_szOutputMode         = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)405);
        Err_szCmdID              = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)406);
        Err_szNumber             = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)407);
        Err_szExecModeID         = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)408);
        Err_szIfNo               = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)409);
        Err_szFromIntpr          = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)410);
        Err_szNoNeedTheCmd       = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)411);
        Err_szTelMsgHead         = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)412);
        Err_szMsgId              = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)413);
        Err_szVtyNum             = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)414);
        Err_szDeterminer         = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)415);
        Err_szUndefined          = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)416);

        Hdr_szSaveMasterOnSlaveErr = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)418);
        Hdr_szCannotSaveOnSlave    = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)419);
        Hdr_szUnknownSaveType      = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)420);	 
        Hdr_szErrInvalidModule     = OamCfgConvertWordToString(oamCfgDat,MAP_TYPE_INT_INNER_ERROR, (WORD16)421);
        return;
}

#endif

