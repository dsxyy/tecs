/*********************************************************************
* ��Ȩ���� (C)2008, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ�DatReg.c
* �ļ���ʶ��
* ����ժҪ��DAT������ע�ắ��
* ����˵����

* ��ǰ�汾��
* ��    �ߣ�kong.wei2   134463
* ������ڣ�2008.6.11
*
* �޸ļ�¼1��
*    �޸����ڣ�
*    �� �� �ţ�
*    �� �� �ˣ�
*    �޸����ݣ�
* �޸ļ�¼2����
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

        /*���Ҫ���ص�����dat�ű������������еİ汾��Ϣ������*/
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

    /*���³�ʼ��ȫ��dat�ڴ�*/
    if (!OAM_CFGInitGlobalDatMemForRestore(g_ptOamIntVar->tCfgdat.nModeSum, g_ptOamIntVar->tCfgdat.nTreeNodeSum))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "Failed to initialize global dat memory! all jobs can't register");
        return OAM_CLI_FAIL;
    }
    
    /* ע�� */
    if (!RegisterDat(&g_ptOamIntVar->tCfgdat, tJid, TRUE))
    {
        if (g_wRegDatCount > 0)
        {
            /*ע��ʧ�ܵ����Ѿ���ӵ�g_tDatLink�У���Ҫ�����Ƴ�*/
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

    /* ע�� */
    if (!RegisterDat(&tCfgdat, tJid, FALSE))
    {
        if (g_wRegDatCount > 0)
        {
            /*ע��ʧ�ܵ����Ѿ���ӵ�g_tDatLink�У���Ҫ�����Ƴ�*/
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
        /* �����ַ������λ�ó�ʼ��*/
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
  /*ddm������Ҫ����������Ͱ汾��Ӧһ��jid*/
#if 0
    if (!bRestore)
    {
        /*����ڴ����Ѿ���jid��ص�dat��������Ϊ��Ч����*/
        T_DAT_LINK *pTmpCfgDat = Oam_CliSearchDat(tJid);
        if (pTmpCfgDat)
        {
            pTmpCfgDat->datIndex.bIsValid = TRUE;
            XOS_SysLog(OAM_CFG_LOG_NOTICE, "RegisterDat: input dat exist in datlink, reset valid ok!\n");
            return TRUE;
        }
    }
#endif
    /* ���У�� */
    if (pCfgDat == NULL)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "RegisterDat: input dat is NULL, register fail!\n");
        return FALSE;
    }

    pCfgDat->tJid = tJid;
    pCfgDat->bIsValid = TRUE;

    /*���溯�����pCfgDat������dat�������isvalidҪ������͸�ֵ*/
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
    /*����pcLint����index�޸�Ϊindex1*/
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
 
        /* ������� */
        if (g_wRegDatCount >= wMaxDatCnt)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, "AppendDatLink: g_wRegDatCount(%d) > g_ptOamIntVar->wDatSum(%d), append dat failed",
                                 g_wRegDatCount, wMaxDatCnt);
            return FALSE;
        }

        /* �������� */
	
        memset(&(g_tDatLink[g_wRegDatCount].datIndex), 0, sizeof(g_tDatLink[g_wRegDatCount].datIndex));
        memcpy(&(g_tDatLink[g_wRegDatCount].datIndex) , ptCfgDat, sizeof(g_tDatLink[g_wRegDatCount].datIndex));
    }

    /* ����ģʽ�ڵ� */
    {
        DWORD i;

        for (i=1;i < ptCfgDat->nModeSum;i++)
        {
            /* ���ģʽ���õ�ģʽ��Ӧ�����ڵ� */
            WORD16 modenode = OAM_CFGAppendMode(&ptCfgDat->tMode[i], bRestore);

            if ((modenode != 0) && (ptCfgDat->tMode[i].wTreeRootPos != 0))
            {
                /* ����ӽڵ� */
                /* ���ĳ���ڵ����ӽڵ㣬OAM_CFGAppendChildNodes�������ݹ�����ӽڵ� */
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
    /* ����ָ�����ڵ�DAT���� */
    WORD16 index = SearchDatLink(jid);
    if (index < g_wRegDatCount)
    {
        /* ɾ������ */
        DeleteChildNodes(&g_tDatLink[index].datIndex);
        /*UnRegisterCmd(jid);*/

        /* ɾ���ڵ㣬����������� */
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

    /* �����һ���ڵ� */
    treenodeofglobal = OAM_CFGAppendChildNode(parentofglobal, &treenode_array[nodetoinsert], cfgDat);
    /* �ݹ� */
    if (treenode_array[nodetoinsert].wNextSonPos != 0)
    {
        OAM_CFGAppendChildNodes(treenodeofglobal, cfgDat, treenode_array[nodetoinsert].wNextSonPos, treenode_array);
    }
    /* ��һ����Ҫ����Ľڵ� */
    nodetoinsert = treenode_array[nodetoinsert].wNextNodePos;

    /* �����ֵܽڵ� */
    while (nodetoinsert != 0)
    {
        /* ����ڵ� */
        treenodeofglobal = AppendSiblingNode(treenodeofglobal, &treenode_array[nodetoinsert], cfgDat);
        /* �ݹ� */
        if (treenode_array[nodetoinsert].wNextSonPos != 0)
        {
            OAM_CFGAppendChildNodes(treenodeofglobal, cfgDat, treenode_array[nodetoinsert].wNextSonPos, treenode_array);
        }
        /* ��һ����Ҫ����Ľڵ� */
        nodetoinsert = treenode_array[nodetoinsert].wNextNodePos;
    }
}
#else
void OAM_CFGAppendChildNodes(WORD16 parentofglobal, T_OAM_Cfg_Dat* cfgDat, WORD16 firstchild, TYPE_TREE_NODE* treenode_array)
{
    WORD16 treenodeofglobal;
    WORD16 nodetoinsert = firstchild;

    /* �����ֵܽڵ� */
    while (nodetoinsert != 0)
    {
        /* ����ڵ� */
        treenodeofglobal = OAM_CFGAppendChildNode(parentofglobal, &treenode_array[nodetoinsert], cfgDat);
        /* �ݹ� */
        if (treenode_array[nodetoinsert].wNextSonPos != 0)
        {
            OAM_CFGAppendChildNodes(treenodeofglobal, cfgDat, treenode_array[nodetoinsert].wNextSonPos, treenode_array);
        }
        /* ��һ����Ҫ����Ľڵ� */
        nodetoinsert = treenode_array[nodetoinsert].wNextNodePos;
    }
}

/**************************************************************************
* �������ƣ�VOID Inner_String_Init
* ���������������ַ������λ�ó�ʼ��
* ���ʵı���
* �޸ĵı���
* ���������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
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

