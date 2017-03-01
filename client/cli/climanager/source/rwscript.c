/**************************************************************************
* 版权所有 (C)2002, 深圳市中兴通讯股份有限公司。
*
* 文件名称：RWScript.c
* 文件标识：
* 内容摘要：读写脚本
* 其它说明：
            
* 当前版本：
* 作    者：马俊辉
* 完成日期：2007.6.5
*
* 修改记录1：// 修改历史记录，包括修改日期、修改者及修改内容
*    修改日期：
*    版 本 号：
*    修 改 人：
*    修改内容：
* 修改记录2：
**************************************************************************/
/**************************************************************************
*                           头文件                                        *
**************************************************************************/
#include "includes.h"
#include "oam_execute.h"
/*#include "vmm_file.h"
#include "oss_file.h"*/
#include "datreg.h"

#include <dirent.h>
#include <sys/stat.h>
/*#include "pub_scs_lib.h"*/

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
/*命令脚本和配置文件目录*/
CHAR g_aucScriptPath[MAX_PATH_LEN] = "./";

/* CLI命令提示 */
CHAR *CLI_szCLIHelp0           = NULL;
CHAR *CLI_szCLIHelp1           = NULL;
CHAR *CLI_szCLIHelp2           = NULL;
CHAR *CLI_szCLIHelp3           = NULL;
CHAR *CLI_szCLIHelp4           = NULL;
CHAR *CLI_szCLIHelp5           = NULL;
CHAR *CLI_szCLIHelp6           = NULL;
CHAR *CLI_szCLIHelp7           = NULL;
CHAR *CLI_szCLIHelp8           = NULL;
CHAR *CLI_szCLIHelp9           = NULL;
CHAR *CLI_szCLIHelp10          = NULL;
CHAR *CLI_szCLIHelp11          = NULL;
CHAR *CLI_szCLIHelp12          = NULL;
CHAR *CLI_szCLIHelp13          = NULL;
CHAR *CLI_szCLIHelp14          = NULL;
CHAR *CLI_szCLIHelp15          = NULL;
CHAR *CLI_szCLIHelp16          = NULL;
CHAR *CLI_szCLIHelp17          = NULL;
CHAR *CLI_szCLIHelp18          = NULL;
CHAR *CLI_szCLIHelp19          = NULL;
CHAR *CLI_szCLIHelp20          = NULL;
CHAR *CLI_szCLIHelp21          = NULL;
CHAR *CLI_szCLIHelp22          = NULL;
CHAR *CLI_szCLIHelp23          = NULL;
CHAR *CLI_szCLIHelp24          = NULL;
CHAR *CLI_szCLIHelp25          = NULL;

/* 字符串常量 */
CHAR *Hdr_szEnterString        = NULL;
CHAR *Hdr_szUserName           = NULL;
CHAR *Hdr_szPassword           = NULL;
CHAR *Hdr_szCmdNoCmdTip        = NULL;
CHAR *Hdr_szTelInputText       = NULL;
CHAR *Hdr_szOsBadUserName      = NULL;
CHAR *Hdr_szOsBadPassword      = NULL;
CHAR *Hdr_szCanNotEnterConfig  = NULL;
CHAR *Hdr_szEnterConfigTip     = NULL;
CHAR *Hdr_szEnterConfigWarning = NULL;
CHAR *Hdr_szCanNotLogin        = NULL;
CHAR *Hdr_szExeNoReturn        = NULL;
CHAR *Hdr_szExeNoDisplay       = NULL;
CHAR *Hdr_szExeTimeout         = NULL;
CHAR *Hdr_szSlaveAckTimeout    = NULL;
CHAR *Hdr_szTelRequestOutMode  = NULL;
CHAR *Hdr_szCanNotChangMode    = NULL;
CHAR *Hdr_szDisplayScriptErr   = NULL;
CHAR *Hdr_szCeasedTheCmdTip    = NULL;
CHAR *Hdr_szUpdateDatTip       = NULL;
CHAR *Hdr_szNeNotSupportOnlineCfg = NULL;
CHAR *Hdr_szSavePrtclCfgOK     = NULL;
CHAR *Hdr_szSavePrtclCfgFail   = NULL;
CHAR *Hdr_szOpenSaveFileErr    = NULL;
CHAR *Hdr_szInsertCrcErr       = NULL;
CHAR *Hdr_szMSChangeoverTip    = NULL;
CHAR *Hdr_szCommandMutexTip    = NULL;
CHAR *Hdr_szSaveMutexTip       = NULL;
CHAR *Hdr_szDatUpdateMutexTip  = NULL;
CHAR *Hdr_szSlaveSaveOk        = NULL;
CHAR *Hdr_szSlaveSaveFail      = NULL;
CHAR *Hdr_szSaveMasterOnSlaveErr = NULL;
CHAR *Hdr_szCannotSaveOnSlave  = NULL;
CHAR *Hdr_szUnknownSaveType    = NULL;
CHAR *Hdr_szSaveZDBOK          = NULL;
CHAR *Hdr_szSaveZDBFail        = NULL;
CHAR *Hdr_szSaveZDBErrSaveBothPath = NULL;
CHAR *Hdr_szSaveZDBErrMainPath = NULL;
CHAR *Hdr_szSaveZDBErrBackup   = NULL;
CHAR *Hdr_szSaveZDBErrLocalSaving = NULL;
CHAR *Hdr_szSaveZDBErrMSSaving = NULL;
CHAR *Hdr_szSaveZDBErrOther    = NULL;
CHAR *Hdr_szSaveZDBErrNoTblSave         = NULL;
CHAR *Hdr_szSaveZDBErrReadFileErr       = NULL;
CHAR *Hdr_szSaveZDBErrTblNotNeedSave    = NULL;
CHAR *Hdr_szSaveZDBErrPowerOnLoading    = NULL;
CHAR *Hdr_szSaveZDBErrFtpLoading        = NULL;
CHAR *Hdr_szSaveZDBErrAppFtpLoading     = NULL;
CHAR *Hdr_szSaveZDBErrIOBusy            = NULL;
CHAR *Hdr_szSaveZDBErrInvalidHandle     = NULL;

CHAR *Hdr_szErrInvalidModule   = NULL;

CHAR *Hdr_szNotAdmin              = NULL;
CHAR *Hdr_szCannotDelAdmin        = NULL;
CHAR *Hdr_szUserNotExists         = NULL;
CHAR *Hdr_szUserBeenDeleted       = NULL;
CHAR *Hdr_szCmdExecSucc           = NULL;
CHAR *Hdr_szCmdExecFailed         = NULL;
CHAR *Hdr_szCannotCreateAdmin     = NULL;
CHAR *Hdr_szUserDoesExists        = NULL;
CHAR *Hdr_szPasswordRule          = NULL;
CHAR *Hdr_szPwdNotMeetRule        = NULL;
CHAR *Hdr_szTwoInputPwdNotSame    = NULL;
CHAR *Hdr_szConfUserFull          = NULL;
CHAR *Hdr_szViewUserFull          = NULL;
CHAR *Hdr_szUserLocked            = NULL;
CHAR *Hdr_szCannotSetAdminRole    = NULL;
CHAR *Hdr_szUserOnline            = NULL;
CHAR *Hdr_szNeedNotSetRole        = NULL;
CHAR *Hdr_szCannotSetOtherUserPwd = NULL;
CHAR *Hdr_szUserNotLocked         = NULL;
CHAR *Hdr_szNoPermission          = NULL;

/* 错误信息 */
CHAR *Err_szExeMsgHead         = NULL;
CHAR *Err_szReturnCode         = NULL;
CHAR *Err_szLinkChannel        = NULL;
CHAR *Err_szSeqNo              = NULL;
CHAR *Err_szLastPacket         = NULL;
CHAR *Err_szOutputMode         = NULL;
CHAR *Err_szCmdID              = NULL;
CHAR *Err_szNumber             = NULL;
CHAR *Err_szExecModeID         = NULL;
CHAR *Err_szIfNo               = NULL;
CHAR *Err_szFromIntpr          = NULL;
CHAR *Err_szNoNeedTheCmd       = NULL;
CHAR *Err_szTelMsgHead         = NULL;
CHAR *Err_szMsgId              = NULL;
CHAR *Err_szVtyNum             = NULL;
CHAR *Err_szDeterminer         = NULL;
CHAR *Err_szUndefined          = NULL;

CHAR *map_szAppMsgDatalengthErr    = NULL;
CHAR *map_szUserBeenLocked            = NULL;
CHAR *map_szUserOrPwdErr                = NULL;
CHAR *map_szGotoSupperModeErr      = NULL;
CHAR *map_szExePlanFmtStr               = NULL;
CHAR *map_szScriptReloaded              = NULL;
CHAR *map_szCeaseCmdFailed            = NULL;
CHAR *map_szCmdSendAgainFailed    = NULL;
CHAR *map_szDisposeCmdFailed         = NULL;
CHAR *map_szExeCmdFailed               = NULL;
CHAR *map_szPrtclRestoreNotify        = NULL;

extern BOOL Oam_SendCfgFileToSlave(void);

/**************************************************************************
*                          本地变量                                       *
**************************************************************************/
/**************************************************************************
*                          局部函数原型                                   *
**************************************************************************/
static BOOLEAN GetCfgFileField(CHAR *pcInput, CHAR *pcFieldName, CHAR **pcOutput, WORD16 *pwLen);
static void Set_Inner_Error_NULL(void);
static BOOLEAN GetFilePathFromFullName(CHAR *pcFullName, CHAR **ppcPath, WORD16 wMaxPathLen);
static BOOLEAN GetFileNameFromFullName(CHAR *pcFullName, CHAR **ppcName, WORD16 wMaxNameLen);
static CHAR *ReadMapValueOfDat(WORD32 dwDatFuncType,WORD16 wMapType,WORD16 wMapKey);
static BOOLEAN GetDatUsableLangOffSetAndType(CHAR *pcFileName, T_Cli_LangType *ptOutLoadLangType);
/**************************************************************************
*                          局部函数实现                                   *
**************************************************************************/
/*lint -e1717*/
/*lint -e1917*/
/*lint -e578*/
/*lint -e524*/
/*lint -e747*/
/*lint -e539*/

/*~暂时不使用*/
#if 1
/**************************************************************************
* 函数名称：BOOL Oam_CfgIsNeedInvertOrder
* 功能描述：判断是否需要字节序转换
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
BOOL Oam_CfgIsNeedInvertOrder()
{
    WORD16 wOrderTest = 0x1234;
    if (0x12 == *(BYTE*)&wOrderTest)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
#endif

#if 1
/**************************************************************************
* 函数名称：VOID InvertCmdFileHead
* 功能描述：转换命令文件头结构字节序
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_FILE_HEAD *head
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
static void InvertCmdFileHead(TYPE_FILE_HEAD *head)
{
    if (!head)
        return;

    if (!Oam_CfgIsNeedInvertOrder())
        return;

    head->dFormatDef      = XOS_InvertWord32(head->dFormatDef);
    head->dVersion        = XOS_InvertWord32(head->dVersion);
    head->dData           = XOS_InvertWord32(head->dData);
    head->dTime           = XOS_InvertWord32(head->dTime);
    head->dPhyBoardType   = XOS_InvertWord32(head->dPhyBoardType);
    head->dLogicBoardType = XOS_InvertWord32(head->dLogicBoardType);
    head->dwCurLangDatSize = XOS_InvertWord32(head->dwCurLangDatSize);
    /*
    head->dPad[0]         = XOS_InvertWord32(head->dPad[0]);
    head->dPad[1]         = XOS_InvertWord32(head->dPad[1]);
    head->dPad[2]         = XOS_InvertWord32(head->dPad[2]);
    head->dPad[3]         = XOS_InvertWord32(head->dPad[3]);
    head->dPad[4]         = XOS_InvertWord32(head->dPad[4]);
    head->dPad[5]         = XOS_InvertWord32(head->dPad[5]);
    */
    head->dModeBegin      = XOS_InvertWord32(head->dModeBegin);
    head->dCmdTreeBegin   = XOS_InvertWord32(head->dCmdTreeBegin);
    head->dCmdAttrBegin   = XOS_InvertWord32(head->dCmdAttrBegin);
    head->dCmdParaBegin   = XOS_InvertWord32(head->dCmdParaBegin);
    head->dFormatBegin    = XOS_InvertWord32(head->dFormatBegin);
    head->dMapTypeBegin   = XOS_InvertWord32(head->dMapTypeBegin);
    head->dMapItemBegin   = XOS_InvertWord32(head->dMapItemBegin);
    head->dMapStrBegin    = XOS_InvertWord32(head->dMapStrBegin);

    return;
}
#endif

/*~暂时未使用*/
#if 1
/**************************************************************************
* 函数名称：VOID InvertCmdMode
* 功能描述：转换命令模式字节序
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_MODE *mode
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
static void InvertCmdMode(TYPE_MODE *mode)
{
    if (!mode)
        return;

    if (!Oam_CfgIsNeedInvertOrder())
        return;

    mode->wModeClass   = XOS_InvertWord16(mode->wModeClass);
    mode->wTreeRootPos = XOS_InvertWord16(mode->wTreeRootPos);

    return;
}

/**************************************************************************
* 函数名称：VOID InvertCmdTreeNode
* 功能描述：转换命令树节点字节序
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_TREE_NODE *node
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
static void InvertCmdTreeNode(TYPE_TREE_NODE *node)
{
    if (!node)
        return;

    if (!Oam_CfgIsNeedInvertOrder())
        return;

    node->wNextNodePos = XOS_InvertWord16(node->wNextNodePos);
    node->wNextSonPos  = XOS_InvertWord16(node->wNextSonPos);
    node->wCmdAttrPos  = XOS_InvertWord16(node->wCmdAttrPos);

    return;
}

/**************************************************************************
* 函数名称：VOID InvertCmdAttr
* 功能描述：转换命令属性字节序
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_CMD_ATTR *attr
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
static void InvertCmdAttr(TYPE_CMD_ATTR *attr)
{
    if (!attr)
        return;

    if (!Oam_CfgIsNeedInvertOrder())
        return;

    attr->dExeID        = XOS_InvertWord32(attr->dExeID);
    attr->wAllParaCount = XOS_InvertWord16(attr->wAllParaCount);
    attr->dwParaPos     = XOS_InvertWord32(attr->dwParaPos);
    attr->wCmdParaCount = XOS_InvertWord16(attr->wCmdParaCount);
    attr->wNoParaCount  = XOS_InvertWord16(attr->wNoParaCount);
    attr->wToShowLen    = XOS_InvertWord16(attr->wToShowLen);
    attr->dwToShowPos   = XOS_InvertWord32(attr->dwToShowPos);

    return;
}

/**************************************************************************
* 函数名称：VOID InvertCmdPara
* 功能描述：转换命令参数字节序
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_CMD_PARA *para
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
static void InvertCmdPara(TYPE_CMD_PARA *para)
{
    if (!para)
        return;

    if (!Oam_CfgIsNeedInvertOrder())
        return;

    para->ParaNo = XOS_InvertWord16(para->ParaNo);
    if (para->bStructType == STRUCTTYPE_PARA)
    {
        para->uContent.tPara.wLen = XOS_InvertWord16(para->uContent.tPara.wLen);
        if (para->uContent.tPara.bParaType == DATA_TYPE_FLOAT)
        {
            para->uContent.tPara.Min.fMinVal = XOS_InvertWord32((WORD32)para->uContent.tPara.Min.fMinVal);
            para->uContent.tPara.Max.fMaxVal = XOS_InvertWord32((WORD32)para->uContent.tPara.Max.fMaxVal);
        }
        else
        {
            para->uContent.tPara.Min.qdwMinVal = XOS_InvertWord64(para->uContent.tPara.Min.qdwMinVal);
            para->uContent.tPara.Max.qdwMaxVal = XOS_InvertWord64(para->uContent.tPara.Max.qdwMaxVal);
        }
    }

    return;
}

/**************************************************************************
* 函数名称：VOID InvertCmdParaOld
* 功能描述：转换命令参数字节序
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_CMD_PARA_OLD *para
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/11/24    V1.0    傅龙锁      创建
**************************************************************************/
static void InvertCmdParaOld(TYPE_CMD_PARA_OLD *para)
{
    if (!para)
        return;

    if (!Oam_CfgIsNeedInvertOrder())
        return;

    para->ParaNo = XOS_InvertWord16(para->ParaNo);
    if (para->bStructType == STRUCTTYPE_PARA)
    {
        para->uContent.tPara.wLen = XOS_InvertWord16(para->uContent.tPara.wLen);
        /*if (para->uContent.tPara.bParaType == DATA_TYPE_FLOAT)
        {
            para->uContent.tPara.Min.fMinVal = XOS_InvertWord32(para->uContent.tPara.Min.fMinVal);
            para->uContent.tPara.Max.fMaxVal = XOS_InvertWord32(para->uContent.tPara.Max.fMaxVal);
        }
        else*/
        {
            para->uContent.tPara.Min.dwMinVal = XOS_InvertWord32(para->uContent.tPara.Min.dwMinVal);
            para->uContent.tPara.Max.dwMaxVal = XOS_InvertWord32(para->uContent.tPara.Max.dwMaxVal);
        }
    }

    return;
}

/**************************************************************************
* 函数名称：VOID InvertCmdMapType
* 功能描述：转换命令MapType字节序
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_MAP_TYPE *type
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
static void InvertCmdMapType(TYPE_MAP_TYPE *type)
{
    if (!type)
        return;

    if (!Oam_CfgIsNeedInvertOrder())
        return;

    type->wType     = XOS_InvertWord16(type->wType);
    type->wCount    = XOS_InvertWord16(type->wCount);
    type->dwItemPos = XOS_InvertWord32(type->dwItemPos);

    return;
}

/**************************************************************************
* 函数名称：VOID InvertCmdMapItem
* 功能描述：转换命令MapItem字节序
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_MAP_ITEM *item
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
static void InvertCmdMapItem(TYPE_MAP_ITEM *item)
{
    if (!item)
        return;

    if (!Oam_CfgIsNeedInvertOrder())
        return;

    item->wValue       = XOS_InvertWord16(item->wValue);
    item->dwKeyPos     = XOS_InvertWord32(item->dwKeyPos);
    item->dwCommentPos = XOS_InvertWord32(item->dwCommentPos);

    return;
}
#endif
#if 1
/**************************************************************************
* 函数名称：BOOL Read_FileHead
* 功能描述：读命令脚本文件头信息
* 访问的表：无
* 修改的表：无
* 输入参数：XOS_FD fd, TYPE_FILE_HEAD *head
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
static BOOL Read_CmdFileHead(XOS_FD fd, TYPE_FILE_HEAD *head, SWORD32 swBeginOffSet)
{
    SWORD32 sdwByteCount;

    XOS_SeekFile(fd, XOS_SEEK_SET, swBeginOffSet+OAM_VMM_FILE_HEAD_SIZE);
    XOS_ReadFile(fd, (CHAR *)head,sizeof(TYPE_FILE_HEAD), &sdwByteCount);
    assert(sdwByteCount == sizeof(TYPE_FILE_HEAD));
    if (sdwByteCount != sizeof(TYPE_FILE_HEAD))
        return FALSE;

    InvertCmdFileHead(head);

    return TRUE;
}

/**************************************************************************
* 函数名称：BOOL Read_Mode
* 功能描述：读脚本文件模式信息
* 访问的表：无
* 修改的表：无
* 输入参数：XOS_FD fd, TYPE_MODE *mode, DWORD dwCount
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
static BOOL Read_CmdMode(XOS_FD fd, TYPE_MODE *mode, DWORD dwCount)
{
    DWORD i;
    SWORD32 sdwByteCount;

    for (i = 0; i < dwCount; i++)
    {
        XOS_ReadFile(fd, (CHAR *)&mode[i],sizeof(TYPE_MODE), &sdwByteCount);
        assert(sdwByteCount == sizeof(TYPE_MODE));
        if (sdwByteCount != sizeof(TYPE_MODE))
            return FALSE;

        InvertCmdMode(&mode[i]);

    }
    return TRUE;
}

/**************************************************************************
* 函数名称：BOOL Read_TreeNode
* 功能描述：读脚本文件树节点信息
* 访问的表：无
* 修改的表：无
* 输入参数：XOS_FD fd, TYPE_TREE_NODE *node, DWORD dwCount
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
static BOOL Read_CmdTreeNode(XOS_FD fd, TYPE_TREE_NODE *node, DWORD dwCount)
{
    DWORD i;
    SWORD32 sdwByteCount;

    for (i = 0; i < dwCount; i++)
    {
        XOS_ReadFile(fd, (CHAR *)&node[i],sizeof(TYPE_TREE_NODE), &sdwByteCount);
        assert(sdwByteCount == sizeof(TYPE_TREE_NODE));
        if (sdwByteCount != sizeof(TYPE_TREE_NODE))
            return FALSE;

        InvertCmdTreeNode(&node[i]);

    }
    return TRUE;
}

/**************************************************************************
* 函数名称：BOOL Read_CmdAttr
* 功能描述：读脚本文件命令属性信息
* 访问的表：无
* 修改的表：无
* 输入参数：XOS_FD fd, TYPE_CMD_ATTR *attr, DWORD dwCount
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
static BOOL Read_CmdAttr(XOS_FD fd, TYPE_CMD_ATTR *attr, DWORD dwCount)
{
    DWORD i;
    SWORD32 sdwByteCount;

    for (i = 0; i < dwCount; i++)
    {
        XOS_ReadFile(fd, (CHAR *)&attr[i],sizeof(TYPE_CMD_ATTR), &sdwByteCount);
        assert(sdwByteCount == sizeof(TYPE_CMD_ATTR));
        if (sdwByteCount != sizeof(TYPE_CMD_ATTR))
            return FALSE;

        InvertCmdAttr(&attr[i]);

    }
    return TRUE;
}

/**************************************************************************
* 函数名称：BOOL Read_CmdPara
* 功能描述：读脚本文件命令参数信息
* 访问的表：无
* 修改的表：无
* 输入参数：XOS_FD fd, TYPE_CMD_PARA *para, DWORD dwCount
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
static BOOL Read_CmdPara(XOS_FD fd, TYPE_CMD_PARA *para, DWORD dwCount, BOOL bOldVerDat)
{
    DWORD i;
    SWORD32 sdwByteCount;
    TYPE_CMD_PARA_OLD *paraOld = NULL;

    if (bOldVerDat)
    {
        paraOld = (TYPE_CMD_PARA_OLD *) XOS_GetUB(sizeof(TYPE_CMD_PARA_OLD) * (dwCount+1));
        assert(paraOld);    
        if (!paraOld)
        {
            XOS_CloseFile(fd);
            return FALSE;
        }
        memset(paraOld, 0, sizeof(TYPE_CMD_PARA_OLD) * (dwCount+1));
    }

    for (i = 0; i < dwCount; i++)
    {
        /*新程序兼容老DAT*/
        if (bOldVerDat)
        {
            XOS_ReadFile(fd, (CHAR *)&paraOld[i],sizeof(TYPE_CMD_PARA_OLD), &sdwByteCount);
            assert(sdwByteCount == sizeof(TYPE_CMD_PARA_OLD));
            if (sdwByteCount != sizeof(TYPE_CMD_PARA_OLD))
                return FALSE;

            InvertCmdParaOld(&paraOld[i]);

            para[i].bStructType = paraOld[i].bStructType;
            para[i].bIsNoCmdPara = paraOld[i].bIsNoCmdPara;
            para[i].ParaNo = paraOld[i].ParaNo;
            if (paraOld[i].bStructType == STRUCTTYPE_PRAMMAR)
            {
                memcpy(&(para[i].uContent.tPrammar), &(paraOld[i].uContent.tPrammar), sizeof(TYPE_PRAMMAR));
            }
            else
            {
                para[i].uContent.tPara.bParaType = paraOld[i].uContent.tPara.bParaType;
                para[i].uContent.tPara.wLen = paraOld[i].uContent.tPara.wLen;
                if ((paraOld[i].uContent.tPara.bParaType == DATA_TYPE_BYTE) ||
                    (paraOld[i].uContent.tPara.bParaType == DATA_TYPE_WORD) ||
                    (paraOld[i].uContent.tPara.bParaType == DATA_TYPE_DWORD) ||
                    (paraOld[i].uContent.tPara.bParaType == DATA_TYPE_INT) ||
                    (paraOld[i].uContent.tPara.bParaType == DATA_TYPE_MAP) ||
                    (paraOld[i].uContent.tPara.bParaType == DATA_TYPE_STRING))
                {
                    para[i].uContent.tPara.Min.qdwMinVal = paraOld[i].uContent.tPara.Min.dwMinVal;
                    para[i].uContent.tPara.Max.qdwMaxVal = paraOld[i].uContent.tPara.Max.dwMaxVal;
                }
                else if (paraOld[i].uContent.tPara.bParaType == DATA_TYPE_FLOAT)
                {
                    para[i].uContent.tPara.Min.fMinVal = paraOld[i].uContent.tPara.Min.fMinVal;
                    para[i].uContent.tPara.Max.fMaxVal = paraOld[i].uContent.tPara.Max.fMaxVal;
                }

                memcpy(para[i].uContent.tPara.sName, paraOld[i].uContent.tPara.sName, MAX_PARANAME);
                memcpy(para[i].uContent.tPara.sComment, paraOld[i].uContent.tPara.sComment, MAX_PARACOMMENT);
            }
        }
        else
        {
        XOS_ReadFile(fd, (CHAR *)&para[i],sizeof(TYPE_CMD_PARA), &sdwByteCount);
        assert(sdwByteCount == sizeof(TYPE_CMD_PARA));
        if (sdwByteCount != sizeof(TYPE_CMD_PARA))
            return FALSE;

        InvertCmdPara(&para[i]);
        }

    }

    if (bOldVerDat)
    {
        if (paraOld)
        {
            OAM_RETUB(paraOld);
            paraOld = NULL;
    }
    }
    
    return TRUE;
}

/**************************************************************************
* 函数名称：BOOL Read_MapType
* 功能描述：读脚本文件MapType信息
* 访问的表：无
* 修改的表：无
* 输入参数：XOS_FD fd, TYPE_MAP_TYPE *type, DWORD dwCount
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
static BOOL Read_CmdMapType(XOS_FD fd, TYPE_MAP_TYPE *type, DWORD dwCount)
{
    DWORD i;
    SWORD32 sdwByteCount;

    for (i = 0; i < dwCount; i++)
    {
        XOS_ReadFile(fd, (CHAR *)&type[i],sizeof(TYPE_MAP_TYPE), &sdwByteCount);
        assert(sdwByteCount == sizeof(TYPE_MAP_TYPE));
        if (sdwByteCount != sizeof(TYPE_MAP_TYPE))
            return FALSE;

        InvertCmdMapType(&type[i]);

    }
    return TRUE;
}

/**************************************************************************
* 函数名称：BOOL Read_MapItem
* 功能描述：读脚本文件MapItem信息
* 访问的表：无
* 修改的表：无
* 输入参数：XOS_FD fd, TYPE_MAP_ITEM *item, DWORD dwCount
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
static BOOL Read_CmdMapItem(XOS_FD fd, TYPE_MAP_ITEM *item, DWORD dwCount)
{
    DWORD i;
    SWORD32 sdwByteCount;

    for (i = 0; i < dwCount; i++)
    {
        XOS_ReadFile(fd, (CHAR *)&item[i],sizeof(TYPE_MAP_ITEM), &sdwByteCount);
        assert(sdwByteCount == sizeof(TYPE_MAP_ITEM));
        if (sdwByteCount != sizeof(TYPE_MAP_ITEM))
            return FALSE;

        InvertCmdMapItem(&item[i]);

    }

    return TRUE;
}
#endif

/**************************************************************************
*                          全局函数实现                                   *
**************************************************************************/
#if 1
/**************************************************************************
* 函数名称：int ReadCmdScript
* 功能描述：读命令脚本文件
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：ptOutLangType --所加载的语言类型
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
INT32 ReadCmdScript(CHAR *pcFileName, T_OAM_Cfg_Dat *ptCfgDat, T_Cli_LangType *ptOutLangType)
{
    XOS_FD fd;
    TYPE_FILE_HEAD  tFileHead;
    DWORD dwItemTotal;
    SWORD32 sdwByteCount;
    SWORD32 gReadCmdResult;
    XOS_STATUS dwRet = XOS_SUCCESS;
    BOOL bOldVerDat = FALSE;
    BOOLEAN bRet = FALSE;

    gReadCmdResult = READERR_NONE;
    XOS_SysLog(OAM_CFG_LOG_DEBUG, "loading %s ...\n", pcFileName);
    assert(pcFileName && ptCfgDat);
    if (!(pcFileName && ptCfgDat))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,"ReadCmdScript: input parameter is null!\n");
        return gReadCmdResult;
    }

    /*根据配置获取文件语言类型和偏移量*/
    bRet = GetDatUsableLangOffSetAndType(pcFileName, ptOutLangType);
    if (!bRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "ReadCmdScript: GetDatUsableLangOffSetAndType of %s Fail!\n",
                   pcFileName);
        gReadCmdResult = READERR_READHEAD;
        return gReadCmdResult;
    }
	
    /* open the script file */
    dwRet = XOS_OpenFile(pcFileName, XOS_RDONLY, &fd);
    if (dwRet != XOS_SUCCESS)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "ReadCmdScript: Read %s Fail! return %d",
                   pcFileName, dwRet);
        gReadCmdResult = READERR_OPENFILE;
        return gReadCmdResult;
    }

    /* 0.File Head */
    if (!Read_CmdFileHead(fd, &tFileHead, ptOutLangType->swLangOffSet))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,"ReadCmdScript: Read CmdFileHead Fail!");
        XOS_CloseFile(fd);
        gReadCmdResult = READERR_READHEAD;
        return gReadCmdResult;
    }

    memcpy(&(ptCfgDat->tFileHead),&tFileHead,sizeof(TYPE_FILE_HEAD));

    if (tFileHead.dFormatDef != FILE_FORMAT)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,"ReadCmdScript: FILE FORMAT is Wrong !");
        XOS_CloseFile(fd);
        gReadCmdResult = READERR_FORMAT;
        return gReadCmdResult;
    }

    if (tFileHead.dVersion != FILE_VERSION)
    {
        if (tFileHead.dVersion == FILE_VERSION_OLD)
        {
            XOS_SysLog(OAM_CFG_LOG_DEBUG,"ReadCmdScript: File Version is old!");
            bOldVerDat = TRUE;
        }
        else
        {
        XOS_SysLog(OAM_CFG_LOG_ERROR,"ReadCmdScript: File Version is Wrong!");
        XOS_CloseFile(fd);
        gReadCmdResult = READERR_VERSION;
        return gReadCmdResult;
    }
    }

    if (!(sizeof(TYPE_FILE_HEAD) <= tFileHead.dModeBegin     &&
            tFileHead.dModeBegin     <= tFileHead.dCmdTreeBegin  &&
            tFileHead.dCmdTreeBegin  <= tFileHead.dCmdAttrBegin  &&
            tFileHead.dCmdAttrBegin  <= tFileHead.dCmdParaBegin  &&
            tFileHead.dCmdParaBegin  <= tFileHead.dFormatBegin   &&
            tFileHead.dFormatBegin   <= tFileHead.dMapTypeBegin  &&
            tFileHead.dMapTypeBegin  <= tFileHead.dMapItemBegin  &&
            tFileHead.dMapItemBegin  <= tFileHead.dMapStrBegin))
    {
        XOS_CloseFile(fd);
        gReadCmdResult = READERR_OFFSET;
        return gReadCmdResult;
    }

    /*根据语言类型偏移量修改文件头里面的各部分开始位置*/
    tFileHead.dModeBegin        += ptOutLangType->swLangOffSet;
    tFileHead.dCmdTreeBegin  += ptOutLangType->swLangOffSet;
    tFileHead.dCmdAttrBegin   += ptOutLangType->swLangOffSet;
    tFileHead.dCmdParaBegin  += ptOutLangType->swLangOffSet;
    tFileHead.dFormatBegin     += ptOutLangType->swLangOffSet;
    tFileHead.dMapTypeBegin  += ptOutLangType->swLangOffSet;
    tFileHead.dMapItemBegin  += ptOutLangType->swLangOffSet;
    tFileHead.dMapStrBegin     += ptOutLangType->swLangOffSet;

    /* 1.Mode Information */
    if (XOS_SeekFile(fd, XOS_SEEK_SET, tFileHead.dModeBegin + OAM_VMM_FILE_HEAD_SIZE) != XOS_SUCCESS)
    {
        XOS_CloseFile(fd);
        gReadCmdResult = READERR_FILESEEK;
        return gReadCmdResult;
    }

    XOS_ReadFile(fd, (CHAR *)&dwItemTotal,sizeof(DWORD), &sdwByteCount);
    assert(sdwByteCount == sizeof(DWORD));
    if (sdwByteCount != sizeof(DWORD))
    {
        XOS_CloseFile(fd);
        gReadCmdResult = READERR_FREAD;
        return gReadCmdResult;
    }

    if (Oam_CfgIsNeedInvertOrder())
    {
        dwItemTotal = XOS_InvertWord32(dwItemTotal);
    }

    ptCfgDat->nModeSum = dwItemTotal + 1;
    ptCfgDat->tMode = (TYPE_MODE *) XOS_GetUB(sizeof(TYPE_MODE) * ptCfgDat->nModeSum);
    assert(ptCfgDat->tMode);
    if (!ptCfgDat->tMode)
    {
        XOS_CloseFile(fd);
        gReadCmdResult = READERR_MEM;
        return gReadCmdResult;
    }

    memset(ptCfgDat->tMode, 0, sizeof(TYPE_MODE) * ptCfgDat->nModeSum);
    if (!Read_CmdMode(fd, ptCfgDat->tMode+1, dwItemTotal))
    {
        XOS_CloseFile(fd);
        gReadCmdResult = READERR_FREAD;
        return gReadCmdResult;
    }

    /* 2.Command Tree */
    if (XOS_SeekFile(fd, XOS_SEEK_SET, tFileHead.dCmdTreeBegin + OAM_VMM_FILE_HEAD_SIZE) != XOS_SUCCESS)
    {
        XOS_CloseFile(fd);
        gReadCmdResult = READERR_FILESEEK;
        return gReadCmdResult;
    }

    XOS_ReadFile(fd, (CHAR *)&dwItemTotal,sizeof(DWORD), &sdwByteCount);
    assert(sdwByteCount == sizeof(DWORD));
    if (sdwByteCount != sizeof(DWORD))
    {
        XOS_CloseFile(fd);
        gReadCmdResult = READERR_FREAD;
        return gReadCmdResult;
    }

    if (Oam_CfgIsNeedInvertOrder())
    {
        dwItemTotal = XOS_InvertWord32(dwItemTotal);
    }

    ptCfgDat->nTreeNodeSum = dwItemTotal + 1;
    ptCfgDat->tCmdTree = (TYPE_TREE_NODE *) XOS_GetUB(sizeof(TYPE_TREE_NODE) * ptCfgDat->nTreeNodeSum);
    assert(ptCfgDat->tCmdTree);
    if (!ptCfgDat->tCmdTree)
    {
        XOS_CloseFile(fd);
        gReadCmdResult = READERR_MEM;
        return gReadCmdResult;
    }

    memset(ptCfgDat->tCmdTree, 0, sizeof(TYPE_TREE_NODE) * ptCfgDat->nTreeNodeSum);
    if (!Read_CmdTreeNode(fd, ptCfgDat->tCmdTree+1, dwItemTotal))
    {
        XOS_CloseFile(fd);
        gReadCmdResult = READERR_FREAD;
        return gReadCmdResult;
    }

    /* 3.Command Attribution */
    if (XOS_SeekFile(fd, XOS_SEEK_SET, tFileHead.dCmdAttrBegin + OAM_VMM_FILE_HEAD_SIZE) != XOS_SUCCESS)
    {
        XOS_CloseFile(fd);
        gReadCmdResult = READERR_FILESEEK;
        return gReadCmdResult;
    }

    XOS_ReadFile(fd, (CHAR *)&dwItemTotal,sizeof(DWORD), &sdwByteCount);
    assert(sdwByteCount == sizeof(DWORD));
    if (sdwByteCount != sizeof(DWORD))
    {
        XOS_CloseFile(fd);
        gReadCmdResult = READERR_FREAD;
        return gReadCmdResult;
    }

    if (Oam_CfgIsNeedInvertOrder())
    {
        dwItemTotal = XOS_InvertWord32(dwItemTotal);
    }

    ptCfgDat->nCmdAttrSum = dwItemTotal + 1;
    ptCfgDat->tCmdAttr = (TYPE_CMD_ATTR *) XOS_GetUB(sizeof(TYPE_CMD_ATTR) * ptCfgDat->nCmdAttrSum);
    assert(ptCfgDat->tCmdAttr);
    if (!ptCfgDat->tCmdAttr)
    {
        XOS_CloseFile(fd);
        gReadCmdResult = READERR_MEM;
        return gReadCmdResult;
    }

    memset(ptCfgDat->tCmdAttr, 0, sizeof(TYPE_CMD_ATTR) * ptCfgDat->nCmdAttrSum);
    /*OSS_Printf(PRINT_OAM_PLATCFGAGT, PRN_NORMAL, PRN_LEVEL_NORMAL,
    		  "RWScript:Enter Into Function ReadCmdScript !gnCmdAttrSum=%ul\n",gnCmdAttrSum);*/
    if (!Read_CmdAttr(fd, ptCfgDat->tCmdAttr+1, dwItemTotal))
    {
        XOS_CloseFile(fd);
        gReadCmdResult = READERR_FREAD;
        return gReadCmdResult;
    }

    /* 4.Parameter Attribution */
    if (XOS_SeekFile(fd, XOS_SEEK_SET, tFileHead.dCmdParaBegin + OAM_VMM_FILE_HEAD_SIZE) != XOS_SUCCESS)
    {
        XOS_CloseFile(fd);
        gReadCmdResult = READERR_FILESEEK;
        return gReadCmdResult;
    }

    XOS_ReadFile(fd, (CHAR *)&dwItemTotal,sizeof(DWORD), &sdwByteCount);
    assert(sdwByteCount == sizeof(DWORD));
    if (sdwByteCount != sizeof(DWORD))
    {
        XOS_CloseFile(fd);
        gReadCmdResult = READERR_FREAD;
        return gReadCmdResult;
    }

    if (Oam_CfgIsNeedInvertOrder())
    {
        dwItemTotal = XOS_InvertWord32(dwItemTotal);
    }

    ptCfgDat->nCmdParaSum = dwItemTotal + 1;
    ptCfgDat->tCmdPara = (TYPE_CMD_PARA *) XOS_GetUB(sizeof(TYPE_CMD_PARA) * ptCfgDat->nCmdParaSum);
    assert(ptCfgDat->tCmdPara);
    if (!ptCfgDat->tCmdPara)
    {
        XOS_CloseFile(fd);
        gReadCmdResult = READERR_MEM;
        return gReadCmdResult;
    }

    memset(ptCfgDat->tCmdPara, 0, sizeof(TYPE_CMD_PARA) * ptCfgDat->nCmdParaSum);
    if (!Read_CmdPara(fd, ptCfgDat->tCmdPara+1, dwItemTotal, bOldVerDat))
    {
        XOS_CloseFile(fd);
        gReadCmdResult = READERR_FREAD;
        return gReadCmdResult;
    }

    /* 5.Format Information */
    if (XOS_SeekFile(fd, XOS_SEEK_SET, tFileHead.dFormatBegin + OAM_VMM_FILE_HEAD_SIZE) != XOS_SUCCESS)
    {
        XOS_CloseFile(fd);
        gReadCmdResult = READERR_FILESEEK;
        return gReadCmdResult;
    }

    XOS_ReadFile(fd, (CHAR *)&dwItemTotal,sizeof(DWORD), &sdwByteCount);
    assert(sdwByteCount == sizeof(DWORD));
    if (sdwByteCount != sizeof(DWORD))
    {
        XOS_CloseFile(fd);
        gReadCmdResult = READERR_FREAD;
        return gReadCmdResult;
    }

    if (Oam_CfgIsNeedInvertOrder())
    {
        dwItemTotal = XOS_InvertWord32(dwItemTotal);
    }

    ptCfgDat->nFormatSizeSum = dwItemTotal + 1;
    ptCfgDat->tFormat = (CHAR *) XOS_GetUB(sizeof(BYTE) * ptCfgDat->nFormatSizeSum);
    assert(ptCfgDat->tFormat);
    if (!ptCfgDat->tFormat)
    {
        XOS_CloseFile(fd);
        gReadCmdResult = READERR_MEM;
        return gReadCmdResult;
    }

    memset(ptCfgDat->tFormat, 0, sizeof(BYTE) * ptCfgDat->nFormatSizeSum);
    XOS_ReadFile(fd, (CHAR *)(ptCfgDat->tFormat+1),(sizeof(BYTE)*dwItemTotal), &sdwByteCount);
    assert((WORD32)sdwByteCount == (sizeof(BYTE)*dwItemTotal));
    if ((WORD32)sdwByteCount != (sizeof(BYTE)*dwItemTotal))
    {
        XOS_CloseFile(fd);
        gReadCmdResult = READERR_FREAD;
        return gReadCmdResult;
    }

    /* 6.Map type */
    if (XOS_SeekFile(fd, XOS_SEEK_SET, tFileHead.dMapTypeBegin + OAM_VMM_FILE_HEAD_SIZE) != XOS_SUCCESS)
    {
        XOS_CloseFile(fd);
        gReadCmdResult = READERR_FILESEEK;
        return gReadCmdResult;
    }

    XOS_ReadFile(fd, (CHAR *)&dwItemTotal,sizeof(DWORD), &sdwByteCount);
    assert((WORD32)sdwByteCount == sizeof(DWORD));
    if ((WORD32)sdwByteCount != sizeof(DWORD))
    {
        XOS_CloseFile(fd);
        gReadCmdResult = READERR_FREAD;
        return gReadCmdResult;
    }

    if (Oam_CfgIsNeedInvertOrder())
    {
        dwItemTotal = XOS_InvertWord32(dwItemTotal);
    }

    ptCfgDat->nMapTypeSum = dwItemTotal + 1;
    ptCfgDat->tMapType = (TYPE_MAP_TYPE *) XOS_GetUB(sizeof(TYPE_MAP_TYPE) * ptCfgDat->nMapTypeSum);
    assert(ptCfgDat->tMapType);
    if (!ptCfgDat->tMapType)
    {
        XOS_CloseFile(fd);
        gReadCmdResult = READERR_MEM;
        return gReadCmdResult;
    }

    memset(ptCfgDat->tMapType, 0, sizeof(TYPE_MAP_TYPE) * ptCfgDat->nMapTypeSum);
    if (!Read_CmdMapType(fd, ptCfgDat->tMapType+1, dwItemTotal))
    {
        XOS_CloseFile(fd);
        gReadCmdResult = READERR_FREAD;
        return gReadCmdResult;
    }

    /* 7.Map item */
    if (XOS_SeekFile(fd, XOS_SEEK_SET, tFileHead.dMapItemBegin + OAM_VMM_FILE_HEAD_SIZE) != XOS_SUCCESS)
    {
        XOS_CloseFile(fd);
        gReadCmdResult = READERR_FILESEEK;
        return gReadCmdResult;
    }

    XOS_ReadFile(fd, (CHAR *)&dwItemTotal,sizeof(DWORD), &sdwByteCount);
    assert((WORD32)sdwByteCount == sizeof(DWORD));
    if ((WORD32)sdwByteCount != sizeof(DWORD))
    {
        XOS_CloseFile(fd);
        gReadCmdResult = READERR_FREAD;
        return gReadCmdResult;
    }

    if (Oam_CfgIsNeedInvertOrder())
    {
        dwItemTotal = XOS_InvertWord32(dwItemTotal);
    }

    ptCfgDat->nMapItemSum = dwItemTotal + 1;
    ptCfgDat->tMapItem = (TYPE_MAP_ITEM *) XOS_GetUB(sizeof(TYPE_MAP_ITEM) * ptCfgDat->nMapItemSum);
    assert(ptCfgDat->tMapItem);
    if (!ptCfgDat->tMapItem)
    {
        XOS_CloseFile(fd);
        gReadCmdResult = READERR_MEM;
        return gReadCmdResult;
    }

    memset(ptCfgDat->tMapItem, 0, sizeof(TYPE_MAP_ITEM) * ptCfgDat->nMapItemSum);
    if (!Read_CmdMapItem(fd, ptCfgDat->tMapItem+1, dwItemTotal))
    {
        XOS_CloseFile(fd);
        gReadCmdResult = READERR_FREAD;
        return gReadCmdResult;
    }

    /* 8.Map string */
    if (XOS_SeekFile(fd, XOS_SEEK_SET, tFileHead.dMapStrBegin + OAM_VMM_FILE_HEAD_SIZE) != XOS_SUCCESS)
    {
        XOS_CloseFile(fd);
        gReadCmdResult = READERR_FILESEEK;
        return gReadCmdResult;
    }

    XOS_ReadFile(fd, (CHAR *)&dwItemTotal,sizeof(DWORD), &sdwByteCount);
    assert((WORD32)sdwByteCount == sizeof(DWORD));
    if ((WORD32)sdwByteCount != sizeof(DWORD))
    {
        XOS_CloseFile(fd);
        gReadCmdResult = READERR_FREAD;
        return gReadCmdResult;
    }

    if (Oam_CfgIsNeedInvertOrder())
    {
        dwItemTotal = XOS_InvertWord32(dwItemTotal);
    }

    ptCfgDat->nMapStringSum = dwItemTotal + 1;
    ptCfgDat->tMapString = (CHAR *) XOS_GetUB(sizeof(BYTE)* ptCfgDat->nMapStringSum);
    assert(ptCfgDat->tMapString);
    if (!ptCfgDat->tMapString)
    {
        XOS_CloseFile(fd);
        gReadCmdResult = READERR_MEM;
        return gReadCmdResult;
    }

    memset(ptCfgDat->tMapString, 0, sizeof(BYTE) * ptCfgDat->nMapStringSum);
    XOS_ReadFile(fd, (CHAR *)(ptCfgDat->tMapString+1),(sizeof(BYTE)*dwItemTotal), &sdwByteCount);
    assert((WORD32)sdwByteCount == (sizeof(BYTE)*dwItemTotal));
    if ((WORD32)sdwByteCount != (sizeof(BYTE)*dwItemTotal))
    {
        XOS_CloseFile(fd);
        gReadCmdResult = READERR_FREAD;
        return gReadCmdResult;
    }

    /* close the script file */
    XOS_CloseFile(fd);

    gReadCmdResult = READERR_SUCCESS;
    return gReadCmdResult;
}

#endif

/**************************************************************************
* 函数名称：VOID OAM_CFGClearCmdScript
* 功能描述：释放命令脚本占用内存
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
void OAM_CFGClearCmdScript(T_OAM_Cfg_Dat *ptCfgDat)
{
    if (!ptCfgDat)
        return; 
    
    OAM_RETUB(ptCfgDat->tMode);
    OAM_RETUB(ptCfgDat->tCmdTree);
    OAM_RETUB(ptCfgDat->tCmdAttr);
    OAM_RETUB(ptCfgDat->tCmdPara);
    OAM_RETUB(ptCfgDat->tFormat);
    OAM_RETUB(ptCfgDat->tMapType);
    OAM_RETUB(ptCfgDat->tMapItem);
    OAM_RETUB(ptCfgDat->tMapString);

    memset(ptCfgDat, 0, sizeof(T_OAM_Cfg_Dat));
}

#if 0
/**************************************************************************
* 函数名称：BOOL Read_CfgFileHead
* 功能描述：读配置脚本文件头信息
* 访问的表：无
* 修改的表：无
* 输入参数：XOS_FD fd, TYPE_CFG_FILE_HEAD *head
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
static BOOL Read_CfgFileHead(XOS_FD fd, TYPE_CFG_FILE_HEAD *head)
{
    SWORD32 sdwByteCount;

    XOS_ReadFile(fd, (CHAR *)head,sizeof(TYPE_CFG_FILE_HEAD), &sdwByteCount);
    assert(sdwByteCount == sizeof(TYPE_CFG_FILE_HEAD));
    if (sdwByteCount != sizeof(TYPE_CFG_FILE_HEAD))
        return FALSE;

    return TRUE;
}
#endif

/**************************************************************************
* 函数名称：BOOL Read_CommonInfo
* 功能描述：读配置脚本文件通用信息
* 访问的表：无
* 修改的表：无
* 输入参数：XOS_FD fd, COMMON_INFO *common
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
static BOOL Read_CfgCommon(XOS_FD fd, COMMON_INFO *common)
{
    SWORD32 sdwByteCount;
    CHAR aucTmp[1024] = {0};
    CHAR *pcTmpCommon = NULL, *pcTmp = NULL, *pcCommonEndPos = NULL;
    CHAR aucTmpNumStr[100] = {0};
    WORD16 wTmp = 0;

    if (XOS_ReadFile(fd, (CHAR *)aucTmp, sizeof(aucTmp)-1, &sdwByteCount) != XOS_SUCCESS)
    {
        return FALSE;
    }

    if(!GetCfgFileField(aucTmp, CFG_FILE_COMMON_MARK, &pcTmpCommon, &wTmp))
    {
        return FALSE;
    }
    pcCommonEndPos = pcTmpCommon + wTmp + strlen(CFG_FILE_COMMON_MARK) + 3;

    if(!GetCfgFileField(pcTmpCommon, CFG_FILE_BANNER_NAME, &pcTmp, &wTmp))
    {
        return FALSE;
    }
    memcpy(common->sBanner, pcTmp, wTmp);

    if(!GetCfgFileField(pcTmpCommon, CFG_FILE_PROMPT_NAME, &pcTmp, &wTmp))
    {
        return FALSE;
    }
    memcpy(common->sPrompt, pcTmp, wTmp);

    if(!GetCfgFileField(pcTmpCommon, CFG_FILE_ENABLEPWD_NAME, &pcTmp, &wTmp))
    {
        return FALSE;
    }
    memcpy(common->sEnablePassword, pcTmp, wTmp);

    if(!GetCfgFileField(pcTmpCommon, CFG_FILE_CONSOLE_ABS_TIMEOUT, &pcTmp, &wTmp))
    {
        return FALSE;
    }
    memset(aucTmpNumStr, 0, sizeof(aucTmpNumStr));
    memcpy(aucTmpNumStr, pcTmp, wTmp);
    common->wConsoleAbsTimeout = atoi(OAM_LTrim(OAM_RTrim(aucTmpNumStr)));

    if(!GetCfgFileField(pcTmpCommon, CFG_FILE_CONSOLE_IDLE_TIMEOUT, &pcTmp, &wTmp))
    {
        return FALSE;
    }
    memset(aucTmpNumStr, 0, sizeof(aucTmpNumStr));
    memcpy(aucTmpNumStr, pcTmp, wTmp);
    common->wConsoleIdleTimeout = atoi(OAM_LTrim(OAM_RTrim(aucTmpNumStr)));

    if(!GetCfgFileField(pcTmpCommon, CFG_FILE_TELNET_ABS_TIMEOUT, &pcTmp, &wTmp))
    {
        return FALSE;
    }
    memset(aucTmpNumStr, 0, sizeof(aucTmpNumStr));
    memcpy(aucTmpNumStr, pcTmp, wTmp);
    common->wTelnetAbsTimeout = atoi(OAM_LTrim(OAM_RTrim(aucTmpNumStr)));

    if(!GetCfgFileField(pcTmpCommon, CFG_FILE_TELNET_IDLE_TIMEOUT, &pcTmp, &wTmp))
    {
        return FALSE;
    }
    memset(aucTmpNumStr, 0, sizeof(aucTmpNumStr));
    memcpy(aucTmpNumStr, pcTmp, wTmp);
    common->wTelnetIdleTimeout = atoi(OAM_LTrim(OAM_RTrim(aucTmpNumStr)));

    wTmp = strlen(aucTmp) - (pcCommonEndPos - aucTmp);

    XOS_SeekFile(fd, XOS_SEEK_CUR, 0 - wTmp);
#if 0
    XOS_ReadFile(fd, (CHAR *)common,sizeof(COMMON_INFO), &sdwByteCount);
    assert(sdwByteCount == sizeof(COMMON_INFO));
    if (sdwByteCount != sizeof(COMMON_INFO))
        return FALSE;
#endif

    return TRUE;
}

/**************************************************************************
* 函数名称：BOOL Read_UserInfo
* 功能描述：读配置脚本文件用户信息
* 访问的表：无
* 修改的表：无
* 输入参数：XOS_FD fd, DWORD dwCount
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
static BOOL Read_CfgUser(XOS_FD fd, DWORD dwCount)
{
    SWORD32 sdwByteCount;
    CHAR aucTmp[512] = {0};
    CHAR *pcTmpUser = NULL, *pcTmp = NULL, *pcUserEndPos = NULL;
    CHAR aucTmpNumStr[20] = {0};
    WORD16 wTmp = 0;
    USER_NODE *pNode = NULL;

    while(1)
    {
        memset(aucTmp, 0, sizeof(aucTmp));
        if (XOS_ReadFile(fd, (CHAR *)aucTmp, sizeof(aucTmp) - 1, &sdwByteCount) != XOS_SUCCESS)
        {
            return FALSE;
        }

        if(!GetCfgFileField(aucTmp, CFG_FILE_USER_MARK, &pcTmpUser, &wTmp))
        {
            break;
        }
        pcUserEndPos = pcTmpUser + wTmp + strlen(CFG_FILE_USER_MARK) + 3;

        pNode = (USER_NODE *)XOS_GetUB(sizeof(USER_NODE));
        memset(pNode, 0, sizeof(USER_NODE));
		
        if(!GetCfgFileField(pcTmpUser, CFG_FILE_USERNAME_NAME, &pcTmp, &wTmp))
        {
            OAM_RETUB( pNode);
            return FALSE;
        }
        memcpy(pNode->tUserInfo.sUsername, pcTmp, wTmp);

        if(!GetCfgFileField(pcTmpUser, CFG_FILE_PASSWORD_NAME, &pcTmp, &wTmp))
        {
            OAM_RETUB( pNode);
            return FALSE;
        }
        memcpy(pNode->tUserInfo.sPassword, pcTmp, wTmp);

        if(!GetCfgFileField(pcTmpUser, CFG_FILE_PRIVILEGE_NAME, &pcTmp, &wTmp))
        {
            OAM_RETUB( pNode);
            return FALSE;
        }
        memset(aucTmpNumStr, 0, sizeof(aucTmpNumStr));
        memcpy(aucTmpNumStr, pcTmp, wTmp);
        pNode->tUserInfo.ucRight = atoi(OAM_LTrim(OAM_RTrim(aucTmpNumStr)));

        AddUserToLink(pNode);

        wTmp = strlen(aucTmp) - (pcUserEndPos - aucTmp);
        if (wTmp == 0)
        {
            break;
        }
        XOS_SeekFile(fd, XOS_SEEK_CUR, 0 - wTmp);
    }

#if 0
    for (i=0; i<dwCount; i++)
    {
        USER_NODE *pNode = (USER_NODE *)XOS_GetUB(sizeof(USER_NODE));
        assert(pNode);
        if (!pNode)
            return FALSE;
        memset(pNode,0,sizeof(USER_NODE));
        XOS_ReadFile(fd, (CHAR *)&pNode->tUserInfo,sizeof(USER_INFO), &sdwByteCount);
        assert(sdwByteCount == sizeof(USER_INFO));
        if (sdwByteCount != sizeof(USER_INFO))
            return FALSE;

        AddUserToLink(pNode);
    }
#endif

    return TRUE;
}

#if 0
/**************************************************************************
* 函数名称：BOOL Write_CfgFileHead
* 功能描述：写配置脚本文件头信息
* 访问的表：无
* 修改的表：无
* 输入参数：XOS_FD fd, TYPE_CFG_FILE_HEAD *head
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
static BOOL Write_CfgFileHead(XOS_FD fd, TYPE_CFG_FILE_HEAD *head)
{
    SWORD32 sdwByteCount;

    XOS_WriteFile(fd, (CHAR *)head,sizeof(TYPE_CFG_FILE_HEAD), &sdwByteCount);
    assert(sdwByteCount == sizeof(TYPE_CFG_FILE_HEAD));
    if (sdwByteCount != sizeof(TYPE_CFG_FILE_HEAD))
        return FALSE;
    return TRUE;
}
#endif

/**************************************************************************
* 函数名称：BOOL Write_CommonInfo
* 功能描述：写配置脚本文件通用信息
* 访问的表：无
* 修改的表：无
* 输入参数：XOS_FD fd, COMMON_INFO *common
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
static BOOL Write_CfgCommon(XOS_FD fd, COMMON_INFO *common)
{
    SWORD32 sdwByteCount;
    CHAR aucTmp[1024] = {0};

    snprintf(aucTmp,  sizeof(aucTmp), 
                "<"CFG_FILE_COMMON_MARK">\n\r"
                 "\t<"CFG_FILE_BANNER_NAME">""%s""</"CFG_FILE_BANNER_NAME">\n\r"
                 "\t<"CFG_FILE_PROMPT_NAME">""%s""</"CFG_FILE_PROMPT_NAME">\n\r"
                 "\t<"CFG_FILE_ENABLEPWD_NAME">""%s""</"CFG_FILE_ENABLEPWD_NAME">\n\r"
                 "\t<"CFG_FILE_CONSOLE_ABS_TIMEOUT">""%d""</"CFG_FILE_CONSOLE_ABS_TIMEOUT">\n\r"
                 "\t<"CFG_FILE_CONSOLE_IDLE_TIMEOUT">""%d""</"CFG_FILE_CONSOLE_IDLE_TIMEOUT">\n\r"
                 "\t<"CFG_FILE_TELNET_ABS_TIMEOUT">""%d""</"CFG_FILE_TELNET_ABS_TIMEOUT">\n\r"
                 "\t<"CFG_FILE_TELNET_IDLE_TIMEOUT">""%d""</"CFG_FILE_TELNET_IDLE_TIMEOUT">\n\r"
                 "</"CFG_FILE_COMMON_MARK">\n\r\n\r",
                 common->sBanner,
                 common->sPrompt,
                 common->sEnablePassword,
                 common->wConsoleAbsTimeout,
                 common->wConsoleIdleTimeout,
                 common->wTelnetAbsTimeout,
                 common->wTelnetIdleTimeout
                );

    XOS_WriteFile(fd, aucTmp, strlen(aucTmp), &sdwByteCount);
    assert((WORD32)sdwByteCount == strlen(aucTmp));
    if ((WORD32)sdwByteCount != strlen(aucTmp))
        return FALSE;

#if 0
    XOS_WriteFile(fd, (CHAR *)common,sizeof(COMMON_INFO), &sdwByteCount);
    assert(sdwByteCount == sizeof(COMMON_INFO));
    if (sdwByteCount != sizeof(COMMON_INFO))
        return FALSE;
 #endif

    return TRUE;
}

/**************************************************************************
* 函数名称：BOOL Write_UserInfo
* 功能描述：写配置脚本文件用户信息
* 访问的表：无
* 修改的表：无
* 输入参数：XOS_FD fd
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
static BOOL Write_CfgUser(XOS_FD fd, DWORD *dwItemTotal)
{
    SWORD32 sdwByteCount;
    USER_NODE *pNode = pUserLinkHead;
    CHAR aucTmp[1024] = {0};


    (*dwItemTotal) = 0;
    while(pNode)
    {
        memset(aucTmp, 0, sizeof(aucTmp));
        snprintf(aucTmp,  sizeof(aucTmp),"<"CFG_FILE_USER_MARK">\n\r"
                     "\t<"CFG_FILE_USERNAME_NAME">""%s""</"CFG_FILE_USERNAME_NAME">\n\r"
                     "\t<"CFG_FILE_PASSWORD_NAME">""%s""</"CFG_FILE_PASSWORD_NAME">\n\r"
                     "\t<"CFG_FILE_PRIVILEGE_NAME">""%d""</"CFG_FILE_PRIVILEGE_NAME">\n\r"
                     "</"CFG_FILE_USER_MARK">\n\r\n\r",
                     pNode->tUserInfo.sUsername,
                     pNode->tUserInfo.sPassword,
                     pNode->tUserInfo.ucRight
                    );

        XOS_WriteFile(fd, aucTmp, strlen(aucTmp), &sdwByteCount);
        assert((WORD32)sdwByteCount == strlen(aucTmp));
        if ((WORD32)sdwByteCount != strlen(aucTmp))
            return FALSE;

        pNode = pNode->pNext;
        (*dwItemTotal)++;
    }

#if 0
    (*dwItemTotal) = 0;
    while(pNode)
    {
        XOS_WriteFile(fd, (CHAR *)&pNode->tUserInfo,sizeof(USER_INFO), &sdwByteCount);
        assert(sdwByteCount == sizeof(USER_INFO));
        if (sdwByteCount != sizeof(USER_INFO))
            return FALSE;
        pNode = pNode->pNext;
        (*dwItemTotal)++;
    }
#endif

    return TRUE;
}

/**************************************************************************
* 函数名称：int WriteCfgScript
* 功能描述：写配置脚本文件
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
static int WriteCfgScript()
{
    XOS_FD fd;
    DWORD dwItemTotal;
    XOS_STATUS dwRtn;
    CHAR  acOamDir[FILE_PATH_LEN] = {'\0'}; 
    CHAR  acMateDir[FILE_PATH_LEN] = {'\0'}; 
    CHAR  acCfgFile[MAX_ABS_PATH_LEN] = {'\0'};
    CHAR  acCfgFileTemp[MAX_ABS_PATH_LEN] = {'\0'};

    if (!OamGetPath(acOamDir, acMateDir))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[WriteCfgScript]OamGetPath fail!\n");
        return WRITEERR_OPENFILE;
    }
    snprintf(acCfgFile, MAX_ABS_PATH_LEN, "%s%s", acOamDir, OAM_CFG_SCRIPT_FILE);
    snprintf(acCfgFileTemp, MAX_ABS_PATH_LEN, "%s%s", acOamDir, OAM_CFG_SCRIPT_TEMP);

#if 0
    dwRtn = XOS_CreateDirectory(OAM_FILE_DIR);
    if (XOS_SUCCESS != dwRtn && XOS_FILE_ERR_DIR_EXIST != dwRtn)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,"create dir ["OAM_FILE_DIR"] error\n");
    }

    dwRtn = XOS_CreateFile(OAM_CFG_SCRIPT_TEMP);
    if (XOS_SUCCESS != dwRtn)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,"create file ["OAM_CFG_SCRIPT_TEMP"] error, return %d\n", dwRtn);
    }

    dwRtn = XOS_OpenFile(OAM_CFG_SCRIPT_TEMP, XOS_WRONLY|XOS_CREAT|XOS_TRUNC, &fd);
    if (dwRtn != XOS_SUCCESS)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,"open  file ["OAM_CFG_SCRIPT_TEMP"] error return %d\n", dwRtn);
        return WRITEERR_OPENFILE;
    }
#endif
    /* 打开文件，如果文件不存在则创建该文件 */
    dwRtn = XOS_OpenFile(acCfgFileTemp, XOS_CREAT, (XOS_FD *)&fd);
    if (XOS_SUCCESS != dwRtn)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[WriteCfgScript]Open file fail! dwRtn = %d\n", dwRtn);
        return WRITEERR_OPENFILE;
    }

    if (!Write_CfgCommon(fd, &gtCommonInfo))
    {
        XOS_CloseFile(fd);
        return WRITEERR_FWRITE;
    }

#if 0
    /* User Info */
    if (XOS_SeekFile(fd, XOS_SEEK_SET, tFileHead.dUserBegin + sizeof(DWORD)) != XOS_SUCCESS)
    {
        XOS_CloseFile(fd);
        return WRITEERR_FILESEEK;
    }
#endif
    if (!Write_CfgUser(fd,&dwItemTotal))
    {
        XOS_CloseFile(fd);
        return WRITEERR_FWRITE;
    }

#if 0
    /* User Count */
    if (XOS_SeekFile(fd, XOS_SEEK_SET, tFileHead.dUserBegin) != XOS_SUCCESS)
    {
        XOS_CloseFile(fd);
        return WRITEERR_FILESEEK;
    }

    XOS_WriteFile(fd, (CHAR *)&dwItemTotal,sizeof(DWORD), &sdwByteCount);
    assert(sdwByteCount == sizeof(DWORD));
    if (sdwByteCount != sizeof(DWORD))
    {
        XOS_CloseFile(fd);
        return WRITEERR_FWRITE;
    }
#endif

    XOS_CloseFile(fd);
#if 0
    if (XOS_CopyFile(OAM_CFG_SCRIPT_FILE,OAM_CFG_SCRIPT_TEMP,XOS_FILE_OVER_COPY) != XOS_SUCCESS)
        return WRITEERR_COPYFILE;
#endif

    /* 更名 */
    dwRtn = XOS_DeleteFile(acCfgFile);
    if (XOS_SUCCESS != dwRtn)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, 
            "[WriteCfgScript]XOS_DeleteFile fail! dwRtn = %d\n", dwRtn);
    }

    dwRtn = XOS_RenameFile(acCfgFileTemp, acCfgFile);
    if (XOS_SUCCESS != dwRtn)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, 
            "[WriteCfgScript]Rename file fail! dwRtn = %d\n", dwRtn);
        return WRITEERR_COPYFILE;
    }

    return WRITEERR_SUCCESS;
}

/**************************************************************************
* 函数名称：int ReadCfgScript
* 功能描述：读配置脚本文件
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
int ReadCfgScript()
{
    XOS_FD fd;
    /*TYPE_CFG_FILE_HEAD  tFileHead;*/
    DWORD dwItemTotal = 0;
    CHAR aucTmpBanner[MAX_BANNER_LEN + 2] = {0};
    CHAR  acOamDir[FILE_PATH_LEN] = {'\0'}; 
    CHAR  acMateDir[FILE_PATH_LEN] = {'\0'}; 
    CHAR  acCfgFile[MAX_ABS_PATH_LEN] = {'\0'};

    if (!OamGetPath(acOamDir, acMateDir))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[ReadCfgScript]OamGetPath fail!\n");
        return READERR_OPENFILE;
    }
    snprintf(acCfgFile, MAX_ABS_PATH_LEN, "%s%s", acOamDir, OAM_CFG_SCRIPT_FILE);

    ClearCfgScript();

    /* open the script file */
    if (XOS_OpenFile(acCfgFile, XOS_RDONLY, &fd) != XOS_SUCCESS)
        return READERR_OPENFILE;

    memset(&gtCommonInfo, 0, sizeof(gtCommonInfo));

    if (!Read_CfgCommon(fd, &gtCommonInfo))
    {
        XOS_CloseFile(fd);
        return READERR_FREAD;
    }

    if (gtCommonInfo.sBanner[0] != '\n' && gtCommonInfo.sBanner[0] != '\r' )
    {
        strncpy(aucTmpBanner, "\n\r", sizeof(aucTmpBanner));
        strncat(aucTmpBanner, gtCommonInfo.sBanner, sizeof(aucTmpBanner));
        memset(gtCommonInfo.sBanner, 0, sizeof(gtCommonInfo.sBanner));
        memcpy(gtCommonInfo.sBanner, aucTmpBanner, sizeof(gtCommonInfo.sBanner) - 3);
    }

#if 0
    /* User Count */
    if (XOS_SeekFile(fd, XOS_SEEK_SET, tFileHead.dUserBegin) != XOS_SUCCESS)
    {
        XOS_CloseFile(fd);
        return READERR_FILESEEK;
    }

    XOS_ReadFile(fd, (CHAR *)&dwItemTotal,sizeof(DWORD), &sdwByteCount);
    assert(sdwByteCount == sizeof(DWORD));
    if (sdwByteCount != sizeof(DWORD))
    {
        XOS_CloseFile(fd);
        return READERR_FREAD;
    }
#endif

    /* User Info */
    if (!Read_CfgUser(fd, dwItemTotal))
    {
        XOS_CloseFile(fd);
        return READERR_FREAD;
    }

    /* close the script file */
    XOS_CloseFile(fd);

    return READERR_SUCCESS;
}



/**************************************************************************
* 函数名称：VOID ClearCfgScript
* 功能描述：释放配置脚本占用内存
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
void ClearCfgScript()
{
    USER_NODE *pNode = NULL;

    strncpy(gtCommonInfo.sPrompt,DEFAULT_PROMPT, sizeof(gtCommonInfo.sPrompt));
    strncpy(gtCommonInfo.sBanner,DEFAULT_BANNER, sizeof(gtCommonInfo.sBanner));
    strncpy(gtCommonInfo.sEnablePassword,DEFAULT_ENABLE_PASSWORD, sizeof(gtCommonInfo.sEnablePassword));
    gtCommonInfo.wConsoleAbsTimeout  = DEFAULT_CONSOLE_ABS_TIMEOUT;
    gtCommonInfo.wConsoleIdleTimeout = DEFAULT_CONSOLE_IDLE_TIMEOUT;
    gtCommonInfo.wTelnetAbsTimeout   = DEFAULT_TELNET_ABS_TIMEOUT;
    gtCommonInfo.wTelnetIdleTimeout  = DEFAULT_TELNET_IDLE_TIMEOUT;

    pNode = pUserLinkHead;
    while(pNode)
    {
        pUserLinkHead = pNode->pNext;
        OAM_RETUB(pNode);
        pNode = pUserLinkHead;
    }
    pUserLinkHead = NULL;
    pUserLinkTail = NULL;
    pNode = (USER_NODE *)XOS_GetUB(sizeof(USER_NODE));
    assert(pNode);
    if (!pNode)
        return;
    memset(pNode,0,sizeof(USER_NODE));
    strncpy(pNode->tUserInfo.sUsername, DEFAULT_USERNAME, sizeof(pNode->tUserInfo.sUsername));
    strncpy(pNode->tUserInfo.sPassword, DEFAULT_PASSWORD, sizeof(pNode->tUserInfo.sPassword));
    pNode->tUserInfo.ucRight = DEFAULT_RITHT;
    AddUserToLink(pNode);
    return;
}

/**************************************************************************
* 函数名称：int SaveOamCfgInfo
* 功能描述：保存OAM配置信息
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
int SaveOamCfgInfo()
{
    INT32 iWriteCfgResult = WriteCfgScript();
    /*XOS_DeleteFile(OAM_CFG_SCRIPT_TEMP);*/

    /*如果是主板,要同步用户配置信息到备板*/
    if (BOARD_MASTER ==  XOS_GetBoardMSState())
    {
        Oam_SendCfgFileToSlave();
    }

    return iWriteCfgResult;
}

/**************************************************************************
* 函数名称：VOID InitOamFilePath
* 功能描述：初试化Oam文件目录
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
void InitOamFilePath()
{
    XOS_STATUS tStatus;
    XOS_DIR    tDir;

    tStatus = XOS_OpenDir(OAM_FILE_DIR, &tDir);
    if (XOS_SUCCESS != tStatus)
    {
        /*OSS_Printf(PRINT_OAM_PLATCFGAGT, PRN_NORMAL, PRN_LEVEL_NORMAL,
                      "Oam Dir not exist, create Oam Dir!\n");*/

        tStatus = XOS_CreateDirectory(OAM_FILE_DIR);
        if (XOS_SUCCESS != tStatus)
        {
            /*OSS_Printf(PRINT_OAM_PLATCFGAGT, PRN_NORMAL, PRN_LEVEL_NORMAL,
                                 "Create Oam Dir %s failed!\n", OAM_FILE_DIR);*/
        }
        else
        {
            /*OSS_Printf(PRINT_OAM_PLATCFGAGT, PRN_NORMAL, PRN_LEVEL_NORMAL,
                          "Create Oam Dir %s succ!\n", OAM_FILE_DIR);*/
        }
    }
    else if (XOS_SUCCESS == tStatus)
    {
        /*OSS_Printf(PRINT_OAM_PLATCFGAGT, PRN_NORMAL, PRN_LEVEL_NORMAL,
                      "Oam Dir %s already exist!\n", OAM_FILE_DIR);*/
        XOS_CloseDir(tDir);
    }
}

/**************************************************************************
* 函数名称：Oam_SendCfgFileToSlave
* 功能描述：发送用户配置文件给备板
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：true- 成功
            false -失败
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2008/04/03   V1.0    殷浩      创建
**************************************************************************/
BOOL Oam_SendCfgFileToSlave()
{
    XOS_FD swFdCfgFile = OAM_INVALID_FD;
    WORD32 dwRtn = 0;
    SWORD32 sdwCountReaded = 0;
    BYTE aucReadBuf[OAM_CFG_MAX_MSG_LEN] = {0};
    MSG_MS_CFGFILE_SYN tCfgSynMsg;
    CHAR  acOamDir[FILE_PATH_LEN] = {'\0'}; 
    CHAR  acMateDir[FILE_PATH_LEN] = {'\0'}; 
    CHAR  acCfgFile[MAX_ABS_PATH_LEN] = {'\0'};

    if (!OamGetPath(acOamDir, acMateDir))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[Oam_SendCfgFileToSlave]OamGetPath fail!\n");
        return FALSE;
    }
    snprintf(acCfgFile, MAX_ABS_PATH_LEN, "%s%s", acOamDir, OAM_CFG_SCRIPT_FILE);

    dwRtn = XOS_OpenFile(acCfgFile, /*~OSS_EXC|*/XOS_RDONLY, &swFdCfgFile);
    if (XOS_SUCCESS != dwRtn)
    {
        return FALSE;
    }

    memset(aucReadBuf, 0, sizeof(aucReadBuf));
    dwRtn = XOS_ReadFile(swFdCfgFile,
                         (void *)aucReadBuf,
                         sizeof(aucReadBuf),
                         &sdwCountReaded);
    if (XOS_SUCCESS != dwRtn || (WORD32)sdwCountReaded >= sizeof(aucReadBuf))
    {
        /*如果读取失败,或者读取到字符数大于等于最大长度,算失败*/
        XOS_CloseFile(swFdCfgFile);
        return FALSE;
    }
    XOS_CloseFile(swFdCfgFile);

    memset(&tCfgSynMsg, 0, sizeof(tCfgSynMsg));
    tCfgSynMsg.wDataLen = sdwCountReaded;
    memcpy(tCfgSynMsg.aucData, aucReadBuf, tCfgSynMsg.wDataLen);

    if (g_ptOamIntVar)
    {
        XOS_Msend(OAM_MSG_MS_CFGFILE_SYN, (BYTE *)&tCfgSynMsg, sizeof(tCfgSynMsg), XOS_MSG_VER0, XOS_MSG_MEDIUM, &(g_ptOamIntVar->jidSelf));
    }

    return TRUE;
}

/**************************************************************************
* 函数名称：Oam_RcvCfgFileFromMaster
* 功能描述：接收主板发来的用户配置文件
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：true- 成功
            false -失败
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2008/04/03   V1.0    殷浩      创建
**************************************************************************/
BOOL Oam_RcvCfgFileFromMaster(LPVOID pMsgPara)
{
    MSG_MS_CFGFILE_SYN *ptCfgSynMsg = (MSG_MS_CFGFILE_SYN *)pMsgPara;
    XOS_FD swFdCfgFile = OAM_INVALID_FD;
    WORD32 dwWriteCnt = 0;
    WORD32 dwRtn = 0;
    CHAR  acOamDir[FILE_PATH_LEN] = {'\0'}; 
    CHAR  acMateDir[FILE_PATH_LEN] = {'\0'}; 
    CHAR  acCfgFile[MAX_ABS_PATH_LEN] = {'\0'};

    if (!OamGetPath(acOamDir, acMateDir))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[Oam_RcvCfgFileFromMaster]OamGetPath fail!\n");
        return FALSE;
    }
    snprintf(acCfgFile, MAX_ABS_PATH_LEN, "%s%s", acOamDir, OAM_CFG_SCRIPT_FILE);

    if (!Oam_CfgCheckRecvMsgMinLen(sizeof(MSG_MS_CFGFILE_SYN)))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "Oam_RcvCfgFileFromMaster:The length of message received is less than the length of interface struct! ");
        return FALSE; 
    }

    dwRtn = XOS_OpenFile(acCfgFile, XOS_CREAT, &swFdCfgFile);
    if (XOS_SUCCESS != dwRtn)
    {
        return FALSE;
    }

    dwRtn = XOS_WriteFile (swFdCfgFile, (void *)ptCfgSynMsg->aucData, ptCfgSynMsg->wDataLen, (SWORD32 *)&dwWriteCnt);
    if (XOS_SUCCESS != dwRtn)
    {
        XOS_CloseFile(swFdCfgFile);
        return FALSE;
    }
    XOS_CloseFile(swFdCfgFile);

    return TRUE;
}

/*lint +e1717*/
/*lint +e1917*/
/*lint +e578*/
/*lint +e524*/
/*lint +e747*/
/*lint +e539*/

static BOOLEAN GetModeAndCmdNodeCntFromFile(CHAR *pcFileName, WORD32 *pdwModeCnt, WORD32 *pdwTreeNodeCnt, SWORD32 swDatOffSet)
{
    XOS_FD fd;
    TYPE_FILE_HEAD  tFileHead;
    SWORD32 sdwByteCount;

    assert(pcFileName && pdwModeCnt && pdwTreeNodeCnt);
    if (!(pcFileName && pdwModeCnt && pdwTreeNodeCnt))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,"######[GetModeAndCmdNodeCntFromFile]: input parameter is null!\n");
        return FALSE;
    }

    /* open the script file */
    if (XOS_OpenFile(pcFileName, XOS_RDONLY, &fd) != XOS_SUCCESS)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,"######[GetModeAndCmdNodeCntFromFile]: Open file %s fail!\n", pcFileName);
        return FALSE;
    }

    /* 0.File Head */
    if (!Read_CmdFileHead(fd, &tFileHead, swDatOffSet))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "######[GetModeAndCmdNodeCntFromFile]: read file head error");
        XOS_CloseFile(fd);
        return FALSE;
    }

    if (tFileHead.dFormatDef != FILE_FORMAT)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "######[GetModeAndCmdNodeCntFromFile]: file format is wrong");
        XOS_CloseFile(fd);
        return FALSE;
    }

    if ((tFileHead.dVersion != FILE_VERSION) && (tFileHead.dVersion != FILE_VERSION_OLD))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "######[GetModeAndCmdNodeCntFromFile]: file version is wrong");
        XOS_CloseFile(fd);
        return FALSE;
    }

    if (!(sizeof(TYPE_FILE_HEAD) <= tFileHead.dModeBegin     &&
            tFileHead.dModeBegin     <= tFileHead.dCmdTreeBegin  &&
            tFileHead.dCmdTreeBegin  <= tFileHead.dCmdAttrBegin  &&
            tFileHead.dCmdAttrBegin  <= tFileHead.dCmdParaBegin  &&
            tFileHead.dCmdParaBegin  <= tFileHead.dFormatBegin   &&
            tFileHead.dFormatBegin   <= tFileHead.dMapTypeBegin  &&
            tFileHead.dMapTypeBegin  <= tFileHead.dMapItemBegin  &&
            tFileHead.dMapItemBegin  <= tFileHead.dMapStrBegin))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "######[GetModeAndCmdNodeCntFromFile]: some offset in file head is wrong");
        XOS_CloseFile(fd);
        return FALSE;
    }

    /* 1.Mode Information */
    if (XOS_SeekFile(fd, XOS_SEEK_SET, tFileHead.dModeBegin + OAM_VMM_FILE_HEAD_SIZE) != XOS_SUCCESS)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,"######[GetModeAndCmdNodeCntFromFile]: Seek file failed first!\n");
        XOS_CloseFile(fd);
        return FALSE;
    }

    XOS_ReadFile(fd, (CHAR *)pdwModeCnt,sizeof(DWORD), &sdwByteCount);
    assert(sdwByteCount == sizeof(DWORD));
    if (sdwByteCount != sizeof(DWORD))
    {
        XOS_CloseFile(fd);
        return FALSE;
    }

    if (Oam_CfgIsNeedInvertOrder())
    {
        *pdwModeCnt = XOS_InvertWord32(*pdwModeCnt);
    }

    *pdwModeCnt += 1;

    /* 2.Command Tree */
    if (XOS_SeekFile(fd, XOS_SEEK_SET, tFileHead.dCmdTreeBegin + OAM_VMM_FILE_HEAD_SIZE) != XOS_SUCCESS)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,"######[GetModeAndCmdNodeCntFromFile]: Seek file failed second!\n");
        XOS_CloseFile(fd);
        return FALSE;
    }

    XOS_ReadFile(fd, (CHAR *)pdwTreeNodeCnt, sizeof(DWORD), &sdwByteCount);
    assert(sdwByteCount == sizeof(DWORD));
    if (sdwByteCount != sizeof(DWORD))
    {
        XOS_CloseFile(fd);
        return FALSE;
    }

    if (Oam_CfgIsNeedInvertOrder())
    {
        *pdwTreeNodeCnt = XOS_InvertWord32(*pdwTreeNodeCnt);
    }

    *pdwTreeNodeCnt += 1;

    XOS_CloseFile(fd);
    return TRUE;
}

/**************************************************************************
* 函数名称：OAM_CFGGetModeCntInAllDat
* 功能描述：获取所有功能类型的dat，并统计总
                              文件数、模式数、节点数
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：true- 成功
            false -失败
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2008/04/03   V1.0    殷浩      创建
**************************************************************************/
BOOLEAN  OAM_CFGGetDatAndModeAndTreeSum(void)
{ 
    WORD32 dwModeCntTmp = 0, dwTreeNodeCntTmp = 0;
    INT32 i = 0;
    CHAR aucTmpFileName[FILE_PATH_LEN + FILE_NAME_LEN + 2] = {0};

    if (0 == g_ptOamIntVar->wDatSum)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "######[OAM_CFGGetDatAndModeAndTreeSum]: g_ptOamIntVar->wDatSum = 0.\n");
        return FALSE;
    }
	
    g_ptOamIntVar->dwModeSum = 0;
    g_ptOamIntVar->dwTreeNodeSum = 0;
    for (i = 0; i < g_ptOamIntVar->wDatSum; i++)
    {
        memset(aucTmpFileName, 0, sizeof(aucTmpFileName));
        snprintf(aucTmpFileName, sizeof(aucTmpFileName),"%s/%s", g_ptOamIntVar->tDatVerInfo[i].tFileInfo.acVerFilePath, g_ptOamIntVar->tDatVerInfo[i].tFileInfo.acVerFileName);
        if (GetModeAndCmdNodeCntFromFile(aucTmpFileName, &dwModeCntTmp, &dwTreeNodeCntTmp, g_ptOamIntVar->tDatVerInfo[i].tLang.swLangOffSet))
        {
            g_ptOamIntVar->dwModeSum += dwModeCntTmp;
            g_ptOamIntVar->dwTreeNodeSum += dwTreeNodeCntTmp;
        }
    }
    
    return TRUE;
}

/**************************************************************************
* 函数名称：OAM_CFGGetModeCntInAllDat
* 功能描述：从各dat头读取模式数目加到一起
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：true- 成功
            false -失败
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2008/04/03   V1.0    殷浩      创建
**************************************************************************/
WORD32 OAM_CFGGetModeCntInAllDat(void)
{
    /* CRDCM00025960 OMP主备多次倒换触发OAM_Interpret异常，并且打印函数调用链失败 */
    return g_ptOamIntVar->dwModeSum;
}

/**************************************************************************
* 函数名称：OAM_CFGGetTreeNodeCntInAllDat
* 功能描述：依次读取dat文件头获取命令节点数目，
                              加上模式数目就是所有节点数目
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：true- 成功
            false -失败
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2008/04/03   V1.0    殷浩      创建
**************************************************************************/
WORD32 OAM_CFGGetTreeNodeCntInAllDat(void)
{
    return 3000;
}

static void Set_Inner_Error_NULL()
{
    CLI_szCLIHelp0               = NULL;
    CLI_szCLIHelp1               = NULL;
    CLI_szCLIHelp2               = NULL;
    CLI_szCLIHelp3               = NULL;
    CLI_szCLIHelp4               = NULL;
    CLI_szCLIHelp5               = NULL;
    CLI_szCLIHelp6               = NULL;
    CLI_szCLIHelp7               = NULL;
    CLI_szCLIHelp8               = NULL;
    CLI_szCLIHelp9               = NULL;
    CLI_szCLIHelp10              = NULL;
    CLI_szCLIHelp11              = NULL;
    CLI_szCLIHelp12              = NULL;
    CLI_szCLIHelp13              = NULL;
    CLI_szCLIHelp14              = NULL;
    CLI_szCLIHelp15              = NULL;
    CLI_szCLIHelp16              = NULL;
    CLI_szCLIHelp17              = NULL;
    CLI_szCLIHelp18              = NULL;
    CLI_szCLIHelp19              = NULL;
    CLI_szCLIHelp20              = NULL;
    CLI_szCLIHelp21              = NULL;
    CLI_szCLIHelp22              = NULL;
    CLI_szCLIHelp23              = NULL;
    CLI_szCLIHelp24              = NULL;
    CLI_szCLIHelp25              = NULL;
                                 
    Hdr_szEnterString            = NULL;
    Hdr_szUserName               = NULL;
    Hdr_szPassword               = NULL;
    Hdr_szCmdNoCmdTip            = NULL;
    Hdr_szTelInputText           = NULL;
    Hdr_szOsBadUserName          = NULL;
    Hdr_szOsBadPassword          = NULL;
    Hdr_szCanNotEnterConfig      = NULL;
    Hdr_szEnterConfigTip         = NULL;
    Hdr_szEnterConfigWarning     = NULL;
    Hdr_szCanNotLogin            = NULL;
    Hdr_szExeNoReturn            = NULL;
    Hdr_szExeNoDisplay           = NULL;
    Hdr_szExeTimeout             = NULL;
    Hdr_szTelRequestOutMode      = NULL;
    Hdr_szCanNotChangMode        = NULL;
    Hdr_szCeasedTheCmdTip        = NULL;
    Hdr_szDisplayScriptErr       = NULL;
    Hdr_szUpdateDatTip           = NULL;
    Hdr_szNeNotSupportOnlineCfg  = NULL;
    Hdr_szSavePrtclCfgOK         = NULL;
    Hdr_szSavePrtclCfgFail       = NULL;
    Hdr_szOpenSaveFileErr        = NULL;
    Hdr_szInsertCrcErr           = NULL;
    Hdr_szMSChangeoverTip        = NULL;
    Hdr_szCommandMutexTip        = NULL;
    Hdr_szSaveMutexTip           = NULL;
    Hdr_szDatUpdateMutexTip      = NULL;
    Hdr_szSlaveSaveOk            = NULL;
    Hdr_szSlaveSaveFail          = NULL;
    Hdr_szSlaveAckTimeout        = NULL;
    Hdr_szSaveZDBOK              = NULL;
    Hdr_szSaveZDBFail            = NULL;
    Hdr_szSaveZDBErrSaveBothPath = NULL;
    Hdr_szSaveZDBErrMainPath     = NULL;
    Hdr_szSaveZDBErrBackup       = NULL;
    Hdr_szSaveZDBErrLocalSaving  = NULL;
    Hdr_szSaveZDBErrMSSaving     = NULL;
    Hdr_szSaveZDBErrOther        = NULL;
    Hdr_szSaveZDBErrNoTblSave         = NULL;
    Hdr_szSaveZDBErrReadFileErr       = NULL;
    Hdr_szSaveZDBErrTblNotNeedSave    = NULL;
    Hdr_szSaveZDBErrPowerOnLoading    = NULL;
    Hdr_szSaveZDBErrFtpLoading        = NULL;
    Hdr_szSaveZDBErrAppFtpLoading     = NULL;
    Hdr_szSaveZDBErrIOBusy            = NULL;
    Hdr_szSaveZDBErrInvalidHandle     = NULL;
                                 
    Hdr_szNotAdmin               = NULL;
    Hdr_szCannotDelAdmin         = NULL;
    Hdr_szUserNotExists          = NULL;
    Hdr_szUserBeenDeleted        = NULL;
    Hdr_szCmdExecSucc            = NULL;
    Hdr_szCmdExecFailed          = NULL;
    Hdr_szCannotCreateAdmin      = NULL;
    Hdr_szUserDoesExists         = NULL;
    Hdr_szPasswordRule           = NULL;
    Hdr_szPwdNotMeetRule         = NULL;
    Hdr_szTwoInputPwdNotSame     = NULL;
    Hdr_szConfUserFull           = NULL;
    Hdr_szViewUserFull           = NULL;
    Hdr_szUserLocked             = NULL;
    Hdr_szCannotSetAdminRole     = NULL;
    Hdr_szUserOnline             = NULL;
    Hdr_szNeedNotSetRole         = NULL;
    Hdr_szCannotSetOtherUserPwd  = NULL;
    Hdr_szUserNotLocked          = NULL;
    Hdr_szNoPermission           = NULL;	
                                 
    Err_szExeMsgHead             = NULL;
    Err_szReturnCode             = NULL;
    Err_szLinkChannel            = NULL;
    Err_szSeqNo                  = NULL;
    Err_szLastPacket             = NULL;
    Err_szOutputMode             = NULL;
    Err_szCmdID                  = NULL;
    Err_szNumber                 = NULL;
    Err_szExecModeID             = NULL;
    Err_szIfNo                   = NULL;
    Err_szFromIntpr              = NULL;
    Err_szNoNeedTheCmd           = NULL;
    Err_szTelMsgHead             = NULL;
    Err_szMsgId                  = NULL;
    Err_szVtyNum                 = NULL;
    Err_szDeterminer             = NULL;
    Err_szUndefined              = NULL;
                               
    Hdr_szSaveMasterOnSlaveErr   = NULL;
    Hdr_szCannotSaveOnSlave      = NULL;
    Hdr_szUnknownSaveType        = NULL;    
    Hdr_szErrInvalidModule       = NULL;
                               
    map_szAppMsgDatalengthErr    = NULL;     
    map_szUserBeenLocked            = NULL;  
    map_szUserOrPwdErr                = NULL;
    map_szGotoSupperModeErr      = NULL;     
    map_szExePlanFmtStr               = NULL;
    map_szScriptReloaded              = NULL;
    map_szCeaseCmdFailed            = NULL;  
    map_szCmdSendAgainFailed    = NULL;      
    map_szDisposeCmdFailed         = NULL;   
    map_szExeCmdFailed               = NULL; 
    map_szPrtclRestoreNotify        = NULL;  


    return;                       
}      

/**************************************************************************
* 函数名称：OAM_CFGRemoveAllDat
* 功能描述：释放所有dat相关的内存
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：true- 成功
            false -失败
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2008/04/03   V1.0    殷浩      创建
**************************************************************************/
void OAM_CFGRemoveAllDat(void)
{
    WORD32 wCnt = 0;

    /*清除所有注册的dat内存*/
    if (g_tDatLink)
    {
        for (wCnt = 0; wCnt < g_ptOamIntVar->wDatSum; wCnt++)
        {
            OAM_CFGClearCmdScript(&(g_tDatLink[wCnt].datIndex));
        }
    }

    /*清除几个全局内存空间*/
    OAM_RETUB(gtMode);
    g_dwModeCount = 0;

    OAM_RETUB(gtTreeNode);
    g_wFreeNode = 1;

    OAM_RETUB(g_tDatLink);
    g_wRegDatCount = 0;

    g_ptOamIntVar->dwModeSum = 0;
    g_ptOamIntVar->dwTreeNodeSum = 0;

    Set_Inner_Error_NULL();
        
    XOS_SysLog(OAM_CFG_LOG_NOTICE, "All memory allocated to dat process were freed");
}

/**************************************************************************
* 函数名称：OAM_CFGInitGlobalDatMem
* 功能描述：初始化全局dat内存
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：true- 成功
            false -失败
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2008/04/03   V1.0    殷浩      创建
**************************************************************************/
BOOL OAM_CFGInitGlobalDatMem(void)
{
    /*获取所有dat版本的对象数量*/
    if (!OAM_CFGGetDatAndModeAndTreeSum())
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "######[OAM_CFGInitGlobalDatMem]: OAM_CFGGetDatAndModeAndTreeSum failed...");
        return FALSE;
    }
    
    XOS_SysLog(OAM_CFG_LOG_NOTICE,
           "OAM_CFGInitGlobalDatMem: DAT files number: %d \n"
           "                     modes number in all dat:%d\n"
           "                     treenodes number in all dat:%d\n",
           g_ptOamIntVar->wDatSum, 
           g_ptOamIntVar->dwModeSum, 
           g_ptOamIntVar->dwTreeNodeSum);

    /*根据获取的对象数量分配相关全局内存空间*/
    g_tDatLink = (T_DAT_LINK *) XOS_GetUB(sizeof(T_DAT_LINK) * g_ptOamIntVar->wDatSum);
    if (!g_tDatLink)
    {
        return FALSE;
    }
    memset(g_tDatLink, 0, sizeof(T_DAT_LINK) * g_ptOamIntVar->wDatSum);
    g_wRegDatCount = 0;

    gtMode = (TYPE_MODE *) XOS_GetUB(sizeof(TYPE_MODE) *g_ptOamIntVar->dwModeSum);
    if (!gtMode)
    {
        return FALSE;
    }
    memset(gtMode, 0, sizeof(TYPE_MODE) * (g_ptOamIntVar->dwModeSum));

    gtTreeNode = (T_TreeNode *) XOS_GetUB(sizeof(T_TreeNode) *g_ptOamIntVar->dwTreeNodeSum);
    if (!gtTreeNode)
    {
        return FALSE;
    }

    OAM_CFGInitTreeNode();
    return TRUE;
}

BOOL OAM_CFGInitGlobalDatMemForRestore(WORD32 dwModeSum, WORD32 dwTreeNodeSum)
{
    gtMode= (TYPE_MODE *) XOS_GetUB(sizeof(TYPE_MODE)*dwModeSum);
    if (!gtMode)
    {
        return FALSE;
    }
    memset(gtMode, 0, sizeof(TYPE_MODE)*dwModeSum);

    gtTreeNode = (T_TreeNode *) XOS_GetUB(sizeof(T_TreeNode)*dwTreeNodeSum);
    if (!gtTreeNode)
    {
        return FALSE;
    }

    OAM_CFGInitTreeNodeForRestore(dwTreeNodeSum);
    return TRUE;
}


/**************************************************************************
* 函数名称：GetCfgFileField
* 功能描述：根据标签名称从用户配置文件中获取数据
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：true- 成功
                             false -失败
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2008/09/03   V1.0    殷浩      创建
**************************************************************************/
static BOOLEAN GetCfgFileField(CHAR *pcInput, CHAR *pcFieldName, CHAR **pcOutput, WORD16 *pwLen)
{
    CHAR *pcBeginPos = NULL, *pcEndPos = NULL;
    CHAR aucTmp[100] = {0};

    if(!pcInput || !pcFieldName || !pwLen)
    {
        return FALSE;
    }

    *pwLen = 0;

    snprintf(aucTmp, sizeof(aucTmp), "<%s>", OAM_LTrim(OAM_RTrim(pcFieldName)));
    pcBeginPos = strstr(pcInput, aucTmp);
    if (!pcBeginPos)
    {
        return FALSE;
    }
    pcBeginPos += strlen(aucTmp);

    memset(aucTmp, 0, sizeof(aucTmp));
    snprintf(aucTmp, sizeof(aucTmp), "</%s>", OAM_LTrim(OAM_RTrim(pcFieldName)));
    pcEndPos = strstr(pcBeginPos, aucTmp);
    if (!pcBeginPos)
    {
        return FALSE;
    }

    *pcOutput = pcBeginPos;
    *pwLen = pcEndPos - pcBeginPos;

    return TRUE;
}

/**************************************************************************
* 函数名称：Oam_CfgGetDatVerInfoFromIniFile
* 功能描述：从oamdatcfg.ini文件中获取版本信息到私有数据区结构体中
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：true- 成功
                            false -失败
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/06/08   V1.0    殷浩      创建
**************************************************************************/
BOOLEAN Oam_CfgGetDatVerInfoFromIniFile(void)
{
    FILE *fp = NULL;
    CHAR *pcFileNameTmp = NULL;
    CHAR acLine[MAX_LINE_STR_LEN] = {0};
    CHAR acFucTypeTmp[MAX_LINE_STR_LEN] = {0};
    CHAR *pcTmpLine = NULL;
    WORD16 wIdx = 0;
    T_OamsVerInfo *ptVerInfoTmp = NULL;
    CHAR *pcStrTmp = NULL;

    CHAR  acOamDatCfgFile[FILE_PATH_LEN] = {'\0'};
    sprintf(acOamDatCfgFile, "%s/%s", g_aucScriptPath, OAM_CFG_DAT_CONFIG_FILE);	
    fp = fopen(acOamDatCfgFile, "r");
    if (!fp)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "Oam_CfgGetDatVerInfoFromIniFile: open %s error!\n", OAM_CFG_DAT_CONFIG_FILE);
        return FALSE;
    }

    memset(g_ptOamIntVar->tDatVerInfo, 0, sizeof(g_ptOamIntVar->tDatVerInfo));
    g_ptOamIntVar->wDatSum = 0;
    /*
    oamdatcfg.ini 文件内每行结构:
    functiontype,filename
    比如:
    1, /home/oam/cfg/oamzx3gplatcmd.dat
    2, /home//oam/cfg/brszx3gplatcmd.dat
    */
    wIdx = 0;
    while (0 == feof(fp))
    {
        memset(acLine, 0, sizeof(acLine));
        if (NULL == fgets(acLine, MAX_LINE_STR_LEN - 1, fp))
        {
            break;
        }
        pcTmpLine = OAM_RTrim(OAM_LTrim(acLine));
  
        /*跳过注释 -- 开头的*/
        if (strlen(pcTmpLine) < 3)
        {
            continue;
        }
        else if (pcTmpLine[0] == '-' && pcTmpLine[1] == '-')
        {
            continue;
        }

        pcFileNameTmp = strchr(pcTmpLine, ',');
        if (!pcFileNameTmp)
        {
            continue;
        }
        memset(acFucTypeTmp, 0, sizeof(acFucTypeTmp));
        memcpy(acFucTypeTmp, pcTmpLine, pcFileNameTmp - pcTmpLine);
        if ((WORD32)(pcFileNameTmp - pcTmpLine + 1) >= strlen(pcTmpLine))
        {
            continue;
        }
        pcFileNameTmp++;
        pcFileNameTmp = OAM_RTrim(OAM_LTrim(pcFileNameTmp));
        
        if (wIdx >= MAX_VER_INFO_NUM)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, "Oam_CfgGetDatVerInfoFromIniFile: too many dat ver info in %s!\n", OAM_CFG_DAT_CONFIG_FILE);
            break;
        }
        else
        {
            ptVerInfoTmp = &g_ptOamIntVar->tDatVerInfo[wIdx].tFileInfo;
        }
    
        ptVerInfoTmp->wVerType = VER_TYPE_SCRIPT;
        ptVerInfoTmp->wFuncType = atoi(acFucTypeTmp);
        pcStrTmp = ptVerInfoTmp->acVerFilePath;
        GetFilePathFromFullName(pcFileNameTmp, &pcStrTmp, sizeof(ptVerInfoTmp->acVerFilePath) - 1);     
        pcStrTmp = ptVerInfoTmp->acVerFileName;
        GetFileNameFromFullName(pcFileNameTmp, &pcStrTmp, sizeof(ptVerInfoTmp->acVerFileName) - 1);
        wIdx++;
        
        XOS_SysLog(OAM_CFG_LOG_DEBUG, "Oam_CfgGetDatVerInfoFromIniFile: dat info[%d]: func type=%d, filename = %s/%s\n", 
                             wIdx, ptVerInfoTmp->wFuncType, ptVerInfoTmp->acVerFilePath, ptVerInfoTmp->acVerFileName);
    }

    fclose(fp);
    
    g_ptOamIntVar->wDatSum = wIdx;

    if (0 == g_ptOamIntVar->wDatSum)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "Oam_CfgGetDatVerInfoFromIniFile: no dat info in %s !\n", OAM_CFG_DAT_CONFIG_FILE);
        return FALSE;
    }

    return TRUE;
}

BOOLEAN GetFilePathFromFullName(CHAR *pcFullName, CHAR **ppcPath, WORD16 wMaxPathLen)
{
    CHAR *pcTmp = NULL;
    CHAR *pcFileName = NULL;
    WORD16 wLen = 0;
    
    if (!(pcFullName) || !(ppcPath) || !(*ppcPath))
    {
        return FALSE;
    }

    pcFileName = pcFullName;
    do
    {
        pcTmp = strchr(pcFileName, '/');
        if (pcTmp)
        {
            pcFileName = pcTmp + 1;
        }
    } while (pcTmp);

    wLen = pcFileName - pcFullName;
    wLen = OAM_CFG_MIN(wLen, wMaxPathLen);
    memcpy(*ppcPath, pcFullName, wLen);

    return TRUE;
}

BOOLEAN GetFileNameFromFullName(CHAR *pcFullName, CHAR **ppcName, WORD16 wMaxNameLen)
{
    CHAR *pcTmp = NULL;
    CHAR *pcFileName = NULL;
    WORD16 wLen = 0;
    
    if (!(pcFullName) || !(ppcName) || !(*ppcName))
    {
        return FALSE;
    }

    pcFileName = pcFullName;
    do
    {
        pcTmp = strchr(pcFileName, '/');
        if (pcTmp)
        {
            pcFileName = pcTmp + 1;
        }
    } while (pcTmp);

    wLen = OAM_CFG_MIN(strlen(pcFileName), wMaxNameLen);

    memcpy(*ppcName, pcFileName, wLen);

    return TRUE;

}

/**************************************************************************
* 函数名称：CHAR *ReadMapValueOfDat
* 功能描述：读取脚本中的MAP值
* 访问的表：无
* 修改的表：无
* 输入参数：dwDatFuncType--功能类型
                             wMapType--MAP类型
                             wMapKey--MAP值
* 输出参数：无
* 返 回 值：CHAR *
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2010/07/14    V1.0    齐龙兆      创建
**************************************************************************/
static CHAR *ReadMapValueOfDat(WORD32 dwDatFuncType,WORD16 wMapType,WORD16 wMapKey)
{
#if 0
    CHAR *ptrMapValue = NULL;
    WORD16 wLoop = 0;
    T_CliAppReg     *ptOamCliReg = NULL;
    
    /*必需在使用后调用Oam_UnLockRegTable释放锁*/
    ptOamCliReg = Oam_GetRegTableAndLock();
    if (!ptOamCliReg)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "failed to create shm which contains rigister table and set lock on it!\r");
        return STRING_NOTFINDMAPKEY;
    }    
    /*遍历共享内存*/ 
    for(wLoop = 0; wLoop < OAM_CLI_DAT_SHM_MAX_NUM; wLoop++)
    {        
        if(dwDatFuncType == ptOamCliReg->tCliReg[wLoop].dwDatFuncType)
        {            
            WORD16 wRegDatCnt = 0;
            for (wRegDatCnt = 0;wRegDatCnt < g_wRegDatCount; wRegDatCnt++)
            {
                /*JID相等*/		   
                if (g_tDatLink[wRegDatCnt].datIndex.tJid.dwJno == ptOamCliReg->tCliReg[wLoop].tAppJid.dwJno)
                {                    
                    ptrMapValue = OamCfgConvertWordToString(&(g_tDatLink[wRegDatCnt].datIndex),wMapType,wMapKey);			
			break;
                }
            }
            break;
        }
    }
    /*释放锁*/
    if (!Oam_UnLockRegTable(ptOamCliReg))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "failed to unlock shm which contains register table ");
    }
    if (NULL != ptrMapValue)
    {
        return ptrMapValue;
    }
    if (!g_ptOamIntVar)
    {
        return STRING_NOTFINDMAPKEY;
    }
    ptOamCliReg = &g_ptOamIntVar->tPlatMpCliAppInfo;
    /*遍历全局数据区*/    
    for(wLoop = 0; wLoop < OAM_CLI_DAT_SHM_MAX_NUM; wLoop++)
    {        
        if(dwDatFuncType == ptOamCliReg->tCliReg[wLoop].dwDatFuncType)
        {
            WORD16 wRegDatCnt = 0;
            for (wRegDatCnt = 0;wRegDatCnt < g_wRegDatCount; wRegDatCnt++)
            {
                /*JID相等*/
                if (g_tDatLink[wRegDatCnt].datIndex.tJid.dwJno == ptOamCliReg->tCliReg[wLoop].tAppJid.dwJno)
                {
                    ptrMapValue = OamCfgConvertWordToString(&(g_tDatLink[wRegDatCnt].datIndex),wMapType,wMapKey);
			break;
                }
            }
            break;
        }
    }
    if (NULL !=ptrMapValue)
    {
        return ptrMapValue;
    }
#endif
    return STRING_NOTFINDMAPKEY;
}

static BOOLEAN WriteUserFileHead(XOS_FD fd, TYPE_USER_FILE_HEAD *head)
{
    SWORD32 sdwByteCount = 0;
    XOS_STATUS dwRtn = XOS_SUCCESS;

    dwRtn = XOS_WriteFile(fd, (CHAR *)head,sizeof(TYPE_USER_FILE_HEAD), &sdwByteCount);
    if (dwRtn != XOS_SUCCESS)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[WriteUserFileHead]: Write user file head failed! dwRtn = %d\n", dwRtn);
        return FALSE;
    }
    assert(sdwByteCount == sizeof(TYPE_USER_FILE_HEAD));
    if (sdwByteCount != sizeof(TYPE_USER_FILE_HEAD))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[WriteUserFileHead]: sdwByteCount != sizeof(TYPE_USER_FILE_HEAD) \n");
        return FALSE;
    }
    return TRUE;
}

static BOOLEAN WriteUserInfo(XOS_FD fd, TYPE_USER_INFO *user)
{
    SWORD32 sdwByteCount = 0;
    XOS_STATUS dwRtn = XOS_SUCCESS;

    dwRtn = XOS_WriteFile(fd, (CHAR *)user,sizeof(TYPE_USER_INFO), &sdwByteCount);
    if (dwRtn != XOS_SUCCESS)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[WriteUserInfo]: Write user file failed! dwRtn = %d\n", dwRtn);
        return FALSE;
    }
    assert(sdwByteCount == sizeof(TYPE_USER_INFO));
    if (sdwByteCount != sizeof(TYPE_USER_INFO))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[WriteUserInfo]: sdwByteCount != sizeof(TYPE_USER_INFO) \n");
        return FALSE;
    }

    return TRUE;
}

static BOOLEAN ReadUserFileHead(XOS_FD fd, TYPE_USER_FILE_HEAD *head)
{
    SWORD32 sdwByteCount = 0;
    XOS_STATUS dwRtn = XOS_SUCCESS;

    dwRtn = XOS_ReadFile(fd, (CHAR *)head,sizeof(TYPE_USER_FILE_HEAD), &sdwByteCount);
    if (dwRtn != XOS_SUCCESS)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[ReadUserFileHead]: Read user file head failed! dwRtn = %d\n", dwRtn);
        return FALSE;
    }
    assert(sdwByteCount == sizeof(TYPE_USER_FILE_HEAD));
    if (sdwByteCount != sizeof(TYPE_USER_FILE_HEAD))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[ReadUserFileHead]: sdwByteCount != sizeof(TYPE_USER_FILE_HEAD) \n");
        return FALSE;
    }

    return TRUE;
}

static BOOLEAN ReadUserInfo(XOS_FD fd, DWORD dwCount)
{
#if 0
    SWORD32 sdwByteCount;
    DWORD i;
    TYPE_USER_INFO tSSHUserInfo;
    XOS_STATUS dwRtn = XOS_SUCCESS;
    
    for (i=0; i<dwCount; i++)
    {
        memset(&tSSHUserInfo, 0, sizeof(TYPE_USER_INFO));
        dwRtn = XOS_ReadFile(fd, (CHAR *)&tSSHUserInfo, sizeof(TYPE_USER_INFO), &sdwByteCount);
        if (dwRtn != XOS_SUCCESS)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, "[ReadUserInfo]: Read user info failed! dwRtn = %d\n", dwRtn);
            return FALSE;
        }
        assert(sdwByteCount == sizeof(TYPE_USER_INFO));
        if (sdwByteCount != sizeof(TYPE_USER_INFO))
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, "[ReadUserInfo]: sdwByteCount != sizeof(TYPE_USER_INFO) \n");
            return FALSE;
        }

        AddOneUserToLink(&tSSHUserInfo);
    }
#endif
    return TRUE;
}

BOOLEAN OamUserFileExists(CHAR *pcFileName)
{
    XOS_STATUS dwRtn = XOS_SUCCESS;
    SWORD32 sdwFileLen = 0;

    dwRtn = XOS_GetFileLength(pcFileName, &sdwFileLen);
    if (XOS_FILE_ERR_FILE_NOT_EXIST == dwRtn)
        return FALSE;
    else if (XOS_SUCCESS == dwRtn)
        return TRUE;
    else
        return FALSE;       
}

BOOLEAN OamLoadUserFile(void)
{
    XOS_FD fdUserFile;
    BOOLEAN bUserFileExist = FALSE;
    XOS_STATUS dwRtn = XOS_SUCCESS;
    TYPE_USER_FILE_HEAD tFileHead;
    TYPE_USER_INFO tAdminUser;
    CHAR  acOamDir[FILE_PATH_LEN] = {'\0'}; 
    CHAR  acMateDir[FILE_PATH_LEN] = {'\0'}; 
    CHAR  acUserFile[MAX_ABS_PATH_LEN] = {'\0'};

    if (!OamGetPath(acOamDir, acMateDir))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[OamLoadUserFile]OamGetPath fail!\n");
        return FALSE;
    }
    snprintf(acUserFile, MAX_ABS_PATH_LEN, "%s%s", acOamDir, OAM_USER_INFO_FILE);

    bUserFileExist = OamUserFileExists(acUserFile);
    if (bUserFileExist)
        dwRtn = XOS_OpenFile(acUserFile, XOS_RDONLY, &fdUserFile);
    else
        dwRtn = XOS_OpenFile(acUserFile, XOS_CREAT, &fdUserFile);

    if (dwRtn != XOS_SUCCESS)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[OamLoadUserFile]: Open user file failed! dwRtn = %d\n", dwRtn);
        return FALSE;
    }

    /* 文件不存在，创建文件，并写入默认用户 */
    if (!bUserFileExist)
    {
        /* 写文件头 */
        tFileHead.dwFormatDef = USER_FILE_FORMAT;
        tFileHead.dwVersion = USER_FILE_VERSION;
        memset(tFileHead.szIPAddr, 0, LEN_IP_ADDR_STR);
        strncpy(tFileHead.szIPAddr, "128.1.1.100", sizeof(tFileHead.szIPAddr));
        memset(g_szSpecifyIP, 0, LEN_IP_ADDR_STR);
        strncpy(g_szSpecifyIP, "128.1.1.100", sizeof(g_szSpecifyIP));
        tFileHead.dwUserBegin = sizeof(TYPE_USER_FILE_HEAD);
        tFileHead.dwUserNum = 1;
        tFileHead.dwUserInfoLen = sizeof(TYPE_USER_INFO);
        memset(tFileHead.ucPad, 0, sizeof(tFileHead.ucPad));
	 gnPasswordIntensity =INTENSITY_OAM_STRONGPWD;
	 gnNorPasslesslen = MIN_OAM_USER_SIMPLEPWD_DEFLEN;
        tFileHead.ucPasswordIntensity = gnPasswordIntensity;
        tFileHead.ucNorPasslesslen = gnNorPasslesslen;	
        snprintf(g_aucScriptLanguage, sizeof(g_aucScriptLanguage), OAM_CLI_LANGTYPE_ENGLISH );
        snprintf(tFileHead.aucCliLanguage, sizeof(tFileHead.aucCliLanguage), "%s", g_aucScriptLanguage );

        if (!WriteUserFileHead(fdUserFile, &tFileHead))
        {
            XOS_CloseFile(fdUserFile);
            return FALSE;
        }

        /* 写管理员用户信息 */
        tAdminUser.ucRole = USER_ROLE_ADMIN;
        tAdminUser.bLocked = FALSE;
        memset(tAdminUser.szUserName, 0, MAX_OAM_USER_NAME_LEN);
        strncpy(tAdminUser.szUserName, DEFAULT_USER_NAME, sizeof(tAdminUser.szUserName));
        memset(tAdminUser.szPassWord, 0, MAX_OAM_USER_PWD_ENCRYPT_LEN);
        /* 需要将密码用MD5方式加密 */
        strncpy(tAdminUser.szPassWord, MDString(DEFAULT_USER_PASSWORD), sizeof(tAdminUser.szPassWord));

        if (XOS_SeekFile(fdUserFile, XOS_SEEK_SET, tFileHead.dwUserBegin) != XOS_SUCCESS)
        {
            XOS_CloseFile(fdUserFile);
            return FALSE;
        }

        if (!WriteUserInfo(fdUserFile, &tAdminUser))
        {
            XOS_CloseFile(fdUserFile);
            return FALSE;
        }

        XOS_CloseFile(fdUserFile);

        gnUserNum++;
        gtUserInfo[0].ucRole = USER_ROLE_ADMIN;
        gtUserInfo[0].bLocked = FALSE;
        strncpy(gtUserInfo[0].szUserName, DEFAULT_USER_NAME, sizeof(gtUserInfo[0].szUserName));
        strncpy(gtUserInfo[0].szPassWord, MDString(DEFAULT_USER_PASSWORD), sizeof(gtUserInfo[0].szPassWord));
    }
    else
    {
        /* File Head */
        if (!ReadUserFileHead(fdUserFile, &tFileHead))
        {
            XOS_CloseFile(fdUserFile);
            return FALSE;
        }

        if (tFileHead.dwFormatDef != USER_FILE_FORMAT)
        {
            XOS_CloseFile(fdUserFile);
            return FALSE;
        }

        if (tFileHead.dwVersion != USER_FILE_VERSION)
        {
            XOS_CloseFile(fdUserFile);
            return FALSE;
        }

        if (!(sizeof(TYPE_USER_FILE_HEAD)<= tFileHead.dwUserBegin))
        {
            XOS_CloseFile(fdUserFile);
            return FALSE;
        }

        /*gnUserNum = tFileHead.dwUserNum;*/
        memset(g_szSpecifyIP, 0, LEN_IP_ADDR_STR);
        strncpy(g_szSpecifyIP, tFileHead.szIPAddr, sizeof(g_szSpecifyIP));
        gnPasswordIntensity =  tFileHead.ucPasswordIntensity;
        gnNorPasslesslen =  tFileHead.ucNorPasslesslen;	
	if(gnNorPasslesslen == ERR_OAM_USER_PWD_LEN) /*对老数据兼容，当不存在密码强度和最少弱密码长度*/
	{
	    gnPasswordIntensity=INTENSITY_OAM_STRONGPWD;
	    gnNorPasslesslen =MIN_OAM_USER_SIMPLEPWD_DEFLEN;
	}

        memset(g_aucScriptLanguage, 0, sizeof(g_aucScriptLanguage));
        if (tFileHead.aucCliLanguage[0] == '\0')
        {
            snprintf(g_aucScriptLanguage, sizeof(g_aucScriptLanguage), OAM_CLI_LANGTYPE_ENGLISH );
        }
        else
        {
            snprintf(g_aucScriptLanguage, sizeof(g_aucScriptLanguage), "%s", tFileHead.aucCliLanguage);
        }

        if (XOS_SeekFile(fdUserFile, XOS_SEEK_SET, tFileHead.dwUserBegin) != XOS_SUCCESS)
        {
            XOS_CloseFile(fdUserFile);
            return FALSE;
        }

        /* User Info */
        if (!ReadUserInfo(fdUserFile, tFileHead.dwUserNum))
        {
            XOS_CloseFile(fdUserFile);
            return FALSE;
        }

        XOS_CloseFile(fdUserFile);

    }
    
    return TRUE;
}

void UseDefaultUserInfo(void)
{
    gnUserNum = 1;
    gtUserInfo[0].ucRole = USER_ROLE_ADMIN;
    gtUserInfo[0].bLocked = FALSE;
    gnPasswordIntensity =INTENSITY_OAM_STRONGPWD;
    gnNorPasslesslen = MIN_OAM_USER_SIMPLEPWD_DEFLEN;	
    strncpy(gtUserInfo[0].szUserName, DEFAULT_USER_NAME, sizeof(gtUserInfo[0].szUserName));
    strncpy(gtUserInfo[0].szPassWord, MDString(DEFAULT_USER_PASSWORD), sizeof(gtUserInfo[0].szPassWord));
    snprintf(g_aucScriptLanguage, sizeof(g_aucScriptLanguage), OAM_CLI_LANGTYPE_ENGLISH );
}

BOOL LockUser(CHAR *pName)
{
    WORD16 i = 0;

    for (i = 0; i < sizeof(gtUserInfo)/sizeof(TYPE_USER_INFO); i++)
    {
        if (strcmp(pName, gtUserInfo[i].szUserName) == 0)
        {
            gtUserInfo[i].bLocked = TRUE;
            break;
        }
    }

    if (!WriteUserScriptFile())
    {
        return FALSE;
    }
    
    return TRUE;
}

BOOL UnLockUser(CHAR *pName)
{
    WORD16 i = 0;

    for (i = 0; i < sizeof(gtUserInfo)/sizeof(TYPE_USER_INFO); i++)
    {
        if (strcmp(pName, gtUserInfo[i].szUserName) == 0)
        {
            gtUserInfo[i].bLocked = FALSE;
            break;
        }
    }

    if (!WriteUserScriptFile())
    {
        return FALSE;
    }
    
    return TRUE;
}

BOOL WriteUserScriptFile(void)
{
    XOS_FD fd;
    XOS_STATUS dwRtn = 0;
    TYPE_USER_FILE_HEAD tFileHead;
    BYTE i = 0;
    DWORD dwOffset = 0;
    CHAR  acOamDir[FILE_PATH_LEN] = {'\0'}; 
    CHAR  acMateDir[FILE_PATH_LEN] = {'\0'}; 
    CHAR  acUserFile[MAX_ABS_PATH_LEN] = {'\0'};
    CHAR  acUserFileTmp[MAX_ABS_PATH_LEN] = {'\0'};

    if (!OamGetPath(acOamDir, acMateDir))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[OamLoadUserFile]OamGetPath fail!\n");
        return FALSE;
    }
    snprintf(acUserFile, MAX_ABS_PATH_LEN, "%s%s", acOamDir, OAM_USER_INFO_FILE);
    snprintf(acUserFileTmp, MAX_ABS_PATH_LEN, "%s%s", acOamDir, OAM_USER_INFO_FILE_TEMP);

    dwRtn = XOS_OpenFile(acUserFileTmp, XOS_CREAT, &fd);
    if (dwRtn != XOS_SUCCESS)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[WriteUserScriptFile]: Open user file failed! dwRtn = %d\n", dwRtn);
        return FALSE;
    }

    /* 写文件头 */
    tFileHead.dwFormatDef = USER_FILE_FORMAT;
    tFileHead.dwVersion = USER_FILE_VERSION;
    memset(tFileHead.szIPAddr, 0, LEN_IP_ADDR_STR);
    strncpy(tFileHead.szIPAddr, g_szSpecifyIP, sizeof(tFileHead.szIPAddr));
    tFileHead.dwUserBegin = sizeof(TYPE_USER_FILE_HEAD);
    tFileHead.dwUserNum = gnUserNum;
    tFileHead.dwUserInfoLen = sizeof(TYPE_USER_INFO);
    memset(tFileHead.ucPad, 0, sizeof(tFileHead.ucPad));
    tFileHead.ucPasswordIntensity = gnPasswordIntensity;
    tFileHead.ucNorPasslesslen = gnNorPasslesslen;
	
    snprintf(tFileHead.aucCliLanguage, sizeof(tFileHead.aucCliLanguage), "%s", g_aucScriptLanguage );
		
    if (!WriteUserFileHead(fd, &tFileHead))
    {
        XOS_CloseFile(fd);
        return FALSE;
    }

    dwOffset = tFileHead.dwUserBegin;
    for (i = 0; i < sizeof(gtUserInfo)/sizeof(TYPE_USER_INFO); i++)
    {
        if (gtUserInfo[i].ucRole != 0)
        {
            if (XOS_SeekFile(fd, XOS_SEEK_SET, dwOffset) != XOS_SUCCESS)
            {
                XOS_CloseFile(fd);
                return FALSE;
            }
            if (!WriteUserInfo(fd, &gtUserInfo[i]))
            {
                XOS_CloseFile(fd);
                return FALSE;
            }
            dwOffset = dwOffset + sizeof(TYPE_USER_INFO);
        }
    }

    XOS_CloseFile(fd);

    /*删除原先的dat文件*/
    dwRtn = XOS_DeleteFile(acUserFile);
    if (XOS_SUCCESS != dwRtn)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[WriteUserScriptFile]: Delete org file failed! dwRtn = %d\n", dwRtn);
    }

    /*重命名*/
    dwRtn = XOS_RenameFile(acUserFileTmp, acUserFile);
    if (XOS_SUCCESS != dwRtn)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[WriteUserScriptFile]: Rename temp file failed! dwRtn = %d\n", dwRtn);
        return FALSE;
    }

    /* 用户信息文件发生更新，需要同步到备板 */
    dwRtn = XOS_SendAsynMsg(EV_CFG_USERFILE_REQ, NULL, 0, XOS_MSG_VER0, XOS_MSG_MEDIUM, &g_ptOamIntVar->jidSelf);
    if (XOS_SUCCESS != dwRtn)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[WriteUserScriptFile]: XOS_SendAsynMsg failed! dwRtn = %d\n", dwRtn);
    }

    return TRUE;
}

BOOL OamSendUserfileToSlave(void)
{
    XOS_FD swFdUserFile = OAM_INVALID_FD;
    WORD32 dwRtn = 0;
    WORD16 wMsgLen = 0;
    SWORD32 sdwCountReaded = 0;
    MSG_MS_FILE_SYN *ptFileSynMsg = (MSG_MS_FILE_SYN *)g_ptOamIntVar->aucSendBuf;
    BYTE *pcReadBuf = ptFileSynMsg->aucData;
    /*读取缓存长度为最大长度减去消息头长度*/
    WORD16 wBufLen = MAX_OAMMSG_LEN - (sizeof(MSG_MS_FILE_SYN) - sizeof( ptFileSynMsg->aucData));
    XOS_STATUS swFileLen = 0;
    WORD32 swReadLen = 0;   /*已发送长度*/
    CHAR  acOamDir[FILE_PATH_LEN] = {'\0'}; 
    CHAR  acMateDir[FILE_PATH_LEN] = {'\0'}; 
    CHAR  acUserFile[MAX_ABS_PATH_LEN] = {'\0'};

    if (!OamGetPath(acOamDir, acMateDir))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[OamSendUserfileToSlave]OamGetPath fail!\n");
        return FALSE;
    }
    snprintf(acUserFile, MAX_ABS_PATH_LEN, "%s%s", acOamDir, OAM_USER_INFO_FILE);

    dwRtn = XOS_OpenFile(acUserFile, XOS_RDONLY, &swFdUserFile);
    if (XOS_SUCCESS != dwRtn)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[OamSendUserfileToSlave]XOS_OpenFile exec fail! dwRtn = %d\n", dwRtn);
        return FALSE;
    }

    /*获取文件长度*/
    dwRtn = XOS_GetFileLength(acUserFile, (SWORD32 *)&swFileLen);
    if (XOS_SUCCESS != dwRtn)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[OamSendUserfileToSlave]XOS_GetFileLength exec fail! dwRtn = %d\n", dwRtn);
        return FALSE;
    }

    while (swReadLen < (WORD32)swFileLen)
    {        
        memset(pcReadBuf, 0, wBufLen);
        dwRtn = XOS_ReadFile(swFdUserFile,
                         (VOID *)pcReadBuf,
                         wBufLen,
                         &sdwCountReaded);
        if (XOS_SUCCESS != dwRtn || sdwCountReaded > wBufLen)
        {
            if (XOS_SUCCESS != dwRtn)
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR, "[OamSendUserfileToSlave]XOS_ReadFile exec fail! dwRtn = %d\n", dwRtn);
            }
            else
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR, "[OamSendUserfileToSlave]sdwCountReaded > wBufLen!\n");
            }

            /*如果读取失败,或者读取到字符数大于等于最大长度,算失败*/
            XOS_CloseFile(swFdUserFile);
          
            return FALSE;
        }

        ptFileSynMsg->wDataLen = (WORD16)sdwCountReaded;
        swReadLen = swReadLen + sdwCountReaded;
        if (swReadLen >= (WORD32)swFileLen)
            ptFileSynMsg->bLastPacket = TRUE;
        else
            ptFileSynMsg->bLastPacket = FALSE;

        wMsgLen = sizeof(MSG_MS_FILE_SYN) - sizeof(ptFileSynMsg->aucData) + ptFileSynMsg->wDataLen;
        XOS_Msend(EV_CFG_USERFILE_ACK, (BYTE *)ptFileSynMsg, wMsgLen, XOS_MSG_VER0, XOS_MSG_MEDIUM, &(g_ptOamIntVar->jidSelf));

        dwRtn = XOS_Delay(1000);
    }
    
    XOS_CloseFile(swFdUserFile);
    
    return TRUE;
}

BOOL OamRecvUserFileFromMaster(LPVOID pMsgPara)
{
    MSG_MS_FILE_SYN *ptFileSynMsg = (MSG_MS_FILE_SYN *)pMsgPara;
    static XOS_FD dwFdUserFile = OAM_INVALID_FD;
    WORD32 dwWriteCnt = 0;
    WORD32 dwRtn = 0;
    CHAR  acOamDir[FILE_PATH_LEN] = {'\0'}; 
    CHAR  acMateDir[FILE_PATH_LEN] = {'\0'}; 
    CHAR  acUserFile[MAX_ABS_PATH_LEN] = {'\0'};

    /* 只要接收到主板应答，就立即杀掉定时器 */
 /*   XOS_KillTimer(TIMER_REQ_USER_FILE, PARAM_NULL);*/
    
    if (!OamGetPath(acOamDir, acMateDir))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[OamRecvUserFileFromMaster]OamGetPath fail!\n");
        return FALSE;
    }
    snprintf(acUserFile, MAX_ABS_PATH_LEN, "%s%s", acOamDir, OAM_USER_INFO_FILE);

    if (!Oam_CfgCheckRecvMsgMinLen(sizeof(MSG_MS_FILE_SYN)))
    {
        /*收到错误数据要继续处理*/
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[OamSendUserfileToSlave]The length of message received is less than the length of interface struct!\n");
        return TRUE;
    }

    if (dwFdUserFile == OAM_INVALID_FD)
    {
        /* 如果文件存在，则先删除 */
        XOS_DeleteFile(acUserFile);
        
        dwRtn = XOS_OpenFile(acUserFile, XOS_CREAT, &dwFdUserFile);
        if (XOS_SUCCESS != dwRtn)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, "[OamRecvUserFileFromMaster]XOS_OpenFile exec fail! dwRtn = %d\n", dwRtn);
            return FALSE;
        }
    }
    
    dwRtn = XOS_WriteFile (dwFdUserFile, (void *)ptFileSynMsg->aucData, ptFileSynMsg->wDataLen, (SWORD32 *)&dwWriteCnt);
    if (XOS_SUCCESS != dwRtn)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[OamRecvUserFileFromMaster]XOS_WriteFile exec fail! dwRtn = %d\n", dwRtn);
        XOS_CloseFile(dwFdUserFile);
        return FALSE;
    }

    if (ptFileSynMsg->bLastPacket)
    {
        XOS_SysLog(OAM_CFG_LOG_DEBUG, "[OamRecvUserFileFromMaster]Receive the last packet!\n");
        XOS_CloseFile(dwFdUserFile);
        dwFdUserFile = OAM_INVALID_FD;
    }

    return TRUE;
}

/**************************************************************************
* 函数名称：CHAR *ReadMapValueOfOamDat
* 功能描述：读取OAM脚本中的MAP值
* 访问的表：无
* 修改的表：无
* 输入参数：WORD16 wMapType--MAP类型
                             WORD16 wMapKey--MAP值
* 输出参数：无
* 返 回 值：CHAR *
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
CHAR *ReadMapValueOfOamDat(WORD16 wMapType,WORD16 wMapKey)
{
    return(ReadMapValueOfDat(FUNC_SCRIPT_CLIS_OAM,wMapType,wMapKey));
}

/**************************************************************************
* 函数名称：CHAR *ReadMapValueOfCmmDat
* 功能描述：读取CMM脚本中的MAP值
* 访问的表：无
* 修改的表：无
* 输入参数：WORD16 wMapType--MAP类型
                             WORD16 wMapKey--MAP值
* 输出参数：无
* 返 回 值：CHAR *
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
CHAR *ReadMapValueOfCmmDat(WORD16 wMapType,WORD16 wMapKey)
{
    return(ReadMapValueOfDat(FUNC_SCRIPT_CLIS_CMM,wMapType,wMapKey));
}

BOOL OamGetPath(CHAR *pPathName, CHAR *pMatePath)
{
    strncpy(pPathName , OAM_CLI_PATH, FILE_PATH_LEN);
    strncat(pPathName , "/dir_cli/", FILE_PATH_LEN);
    strncpy(pMatePath , OAM_CLI_PATH, FILE_PATH_LEN);
    strncat(pMatePath , "/dir_matecli/", FILE_PATH_LEN);
#if 0
    XOS_STATUS  dwRtn = XOS_SUCCESS;
    BYTE        ucHardDiskNum   = 0;
    BYTE        ucFlashDiskNum  = 0;

    T_DevDesc   atHardDiskList[XOS_MAX_HARDDISK_NUM];
    T_DevDesc   atFlashDiskList[XOS_MAX_FLASH_NUM];

    memset(atHardDiskList, 0, XOS_MAX_HARDDISK_NUM*sizeof(T_DevDesc));
    memset(atFlashDiskList, 0, XOS_MAX_FLASH_NUM*sizeof(T_DevDesc));
    /* 获取路径 */
    dwRtn = XOS_GetDevList(atHardDiskList, atFlashDiskList, &ucHardDiskNum, &ucFlashDiskNum);
    if(XOS_SUCCESS != dwRtn)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[OamGetPath]XOS_GetDevList fail! dwRtn = %d\n", dwRtn);
        return FALSE;
    }

    /* 没有块设备，则在内存文件系统创建路径 */
    if((0 == ucHardDiskNum) && (0 == ucFlashDiskNum))
    {
        strncpy(pPathName , "/IDE0/oam/cli/", FILE_PATH_LEN);
        strncpy(pMatePath , "/IDE0/oam/matecli/", FILE_PATH_LEN);
    }

    /* 有硬盘块设备，在硬盘创建路径 */
    if(ucHardDiskNum > 0)
    {
        snprintf(pPathName, FILE_PATH_LEN, "%s/oam/cli/", atHardDiskList[0].aucDevName);
        snprintf(pMatePath, FILE_PATH_LEN, "%s/oam/matecli/", atHardDiskList[0].aucDevName);
    }
    /* 有FLASH块设备无硬盘，在FLASH创建路径 */
    else if(ucFlashDiskNum > 0)
    {
        snprintf(pPathName, FILE_PATH_LEN, "%s/oam/cli/", atFlashDiskList[0].aucDevName);
        snprintf(pMatePath, FILE_PATH_LEN, "%s/oam/matecli/", atFlashDiskList[0].aucDevName);
    }
#endif
    return TRUE;
}

/**** 611001079378 屏蔽NFS绑定功能 ****/
#if 0
VOID OamMountMateDir(VOID)
{
    XOS_STATUS dwXosRtn = 0;
    SCS_STATUS dwScsRtn = 0;
    T_PhysAddress tPhysAddress = {0};
    T_ClientAddr tClientAddr = {0};
    CHAR acIPAddr[16] = {0};
    CHAR acOamDir[FILE_PATH_LEN] = {'\0'};
    CHAR acMateDir[FILE_PATH_LEN] = {'\0'};

    if (!OamGetPath(acOamDir, acMateDir))
    {
        OAM_CliLog(OAM_CFG_LOG_ERROR, "[OamMountMateDir]OamGetPath fail!\n");
        return;
    }

    /* 获取对板物理地址 */
    dwXosRtn = XOS_GetMatePhyAddress(&tPhysAddress);
    if (XOS_SUCCESS != dwXosRtn)
    {
        OAM_CliLog(OAM_CFG_LOG_ERROR, "[OamMountMateDir] XOS_GetMatePhyAddress fail! dwXosRtn = %d\n", dwXosRtn);
        return;
    }
    else
    {
        OAM_CliLog(OAM_CFG_LOG_DEBUG, "[OamMountMateDir] XOS_GetMatePhyAddress success! PhyAddr is: %d-%d-%d-%d\n", 
            tPhysAddress.ucRackId, tPhysAddress.ucShelfId, tPhysAddress.ucSlotId, tPhysAddress.ucCpuId);
    }

    /* 获取对板控制面IP地址接口 */
    dwScsRtn = SCS_GetCtrlIfCfgbyPA(&tPhysAddress, 1, TRUE, &tClientAddr);
    if (SCS_OK != dwScsRtn)
    {
        OAM_CliLog(OAM_CFG_LOG_ERROR, "[OamMountMateDir] SCS_GetCtrlIfCfgbyPA fail! dwScsRtn = %d\n", dwScsRtn);
        return;
    }
    else
    {
        OAM_CliLog(OAM_CFG_LOG_DEBUG, "[OamMountMateDir] SCS_GetCtrlIfCfgbyPA success!\n");
    }
    
    /* 绑定对板cli目录到挂载目录 */
    snprintf(acIPAddr, (WORD32)16, "%d.%d.%d.%d", 
        (tClientAddr.dwClientIP>>24)&0x000000ff, (tClientAddr.dwClientIP>>16)&0x000000ff,
        (tClientAddr.dwClientIP>>8)&0x000000ff, tClientAddr.dwClientIP&0x000000ff);

    OAM_CliLog(OAM_CFG_LOG_DEBUG, "[OamMountMateDir] acIPAddr = [%s]\n", acIPAddr);
    
    dwXosRtn = XOS_NfsMountWT(acIPAddr, acOamDir, acMateDir, 1, 1);
    if (XOS_SUCCESS != dwXosRtn)
    {
        OAM_CliLog(OAM_CFG_LOG_ERROR, "[OamMountMateDir] XOS_NfsMountWT fail! dwXosRtn = %d\n", dwXosRtn);
    }
    else
    {
        OAM_CliLog(OAM_CFG_LOG_DEBUG, "[OamMountMateDir] XOS_NfsMountWT success!\n");
    }
    
    return;
}
#endif

BOOL InitUserLogFile(void)
{
    BOOL bIsFileExists = FALSE;
    XOS_FD fd;
    XOS_FD fdTemp;
    XOS_STATUS ulRet = 0;
    TYPE_USER_LOG_HEAD tFileHead;
    TYPE_USER_LOG_HEAD tFileHeadTemp;
    TYPE_USER_LOG tUserLog;
    SWORD32 sdwByteCount;
    WORD16 i = 0;
    DWORD dwOffset = 0;
    CHAR  acOamDir[FILE_PATH_LEN] = {'\0'}; 
    CHAR  acMateDir[FILE_PATH_LEN] = {'\0'}; 
    CHAR  acUserLogFile[MAX_ABS_PATH_LEN] = {'\0'};
    CHAR  acUserLogFileTemp[MAX_ABS_PATH_LEN] = {'\0'};

    if (!OamGetPath(acOamDir, acMateDir))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[InitUserLogFile]OamGetPath fail!\n");
        return FALSE;
    }
    snprintf(acUserLogFile, MAX_ABS_PATH_LEN, "%s%s", acOamDir, OAM_USER_LOG_FILE);
    snprintf(acUserLogFileTemp, MAX_ABS_PATH_LEN, "%s%s", acOamDir, OAM_USER_LOG_FILE_TEMP);

    /* 首先判断日志文件是否存在 */
    bIsFileExists = OamUserFileExists(acUserLogFile);
    /* 如果文件不存在，则创建该文件 */
    if (!bIsFileExists)
    {
        ulRet = XOS_OpenFile(acUserLogFile, XOS_CREAT, &fd);
        if (ulRet != XOS_SUCCESS)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, "[InitUserLogFile] XOS_OpenFile exec failed! ulRet = %d, line = %d\n", ulRet, __LINE__);
            return FALSE;
        }

        /* 写文件头 */
        tFileHead.dwFormatDef = USER_LOG_FILE_FORMAT;
        tFileHead.dwVersion = USER_LOG_FILE_VERSION;
        tFileHead.dwLogBegin = sizeof(TYPE_USER_LOG_HEAD);
        tFileHead.dwLogNum = 0;

        ulRet = XOS_WriteFile(fd, (CHAR *)&tFileHead,sizeof(TYPE_USER_LOG_HEAD), &sdwByteCount);
        if (ulRet != XOS_SUCCESS)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, "[InitUserLogFile] XOS_WriteFile exec failed! ulRet = %d, line = %d\n", ulRet, __LINE__);
            XOS_CloseFile(fd);
            return FALSE;
        }

        if (sdwByteCount != sizeof(TYPE_USER_LOG_HEAD))
        {
            XOS_CloseFile(fd);
            return FALSE;
        }
        
        XOS_CloseFile(fd);
    }
    else
    {
        /* 如果存在，将记录中所有退出原因为0的修改为异常退出 */
        ulRet = XOS_OpenFile(acUserLogFile, XOS_RDONLY, &fd);
        if (ulRet != XOS_SUCCESS)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, 
                "[InitUserLogFile] XOS_OpenFile exec failed! ulRet = %d, line = %d\n", ulRet, __LINE__);
            return FALSE;
        }

        /* 读取文件头 */
        ulRet = XOS_ReadFile(fd, (CHAR *)&tFileHead,sizeof(TYPE_USER_LOG_HEAD), &sdwByteCount);
        if (ulRet != XOS_SUCCESS)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, 
                "[InitUserLogFile] XOS_ReadFile exec failed! ulRet = %d, line = %d\n", ulRet, __LINE__);
            XOS_CloseFile(fd);
            return FALSE;
        }
        if (sdwByteCount != sizeof(TYPE_USER_LOG_HEAD))
        {
            XOS_CloseFile(fd);
            return FALSE;
        }
        if (tFileHead.dwFormatDef != USER_LOG_FILE_FORMAT)
        {
            XOS_CloseFile(fd);
            return FALSE;
        }
        if (tFileHead.dwVersion != USER_LOG_FILE_VERSION)
        {
            XOS_CloseFile(fd);
            return FALSE;
        }
        if (!(sizeof(TYPE_USER_LOG_HEAD)<= tFileHead.dwLogBegin))
        {
            XOS_CloseFile(fd);
            return FALSE;
        }

        /* 新建临时文件 */
        ulRet = XOS_OpenFile(acUserLogFileTemp, XOS_CREAT, &fdTemp);
        if (ulRet != XOS_SUCCESS)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, 
                "[InitUserLogFile] XOS_OpenFile exec failed! ulRet = %d, line = %d\n", ulRet, __LINE__);
            XOS_CloseFile(fd);
            return FALSE;
        }

        /* 写临时文件文件头 */
        tFileHeadTemp.dwFormatDef = USER_LOG_FILE_FORMAT;
        tFileHeadTemp.dwVersion = USER_LOG_FILE_VERSION;
        tFileHeadTemp.dwLogBegin = sizeof(TYPE_USER_LOG_HEAD);
        tFileHeadTemp.dwLogNum = tFileHead.dwLogNum;

        ulRet = XOS_WriteFile(fdTemp, (CHAR *)&tFileHeadTemp,sizeof(TYPE_USER_LOG_HEAD), &sdwByteCount);
        if (ulRet != XOS_SUCCESS)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, 
                "[InitUserLogFile] XOS_WriteFile exec failed! ulRet = %d, line = %d\n", ulRet, __LINE__);
            XOS_CloseFile(fdTemp);
            XOS_CloseFile(fd);
            return FALSE;
        }
        if (sdwByteCount != sizeof(TYPE_USER_LOG_HEAD))
        {
            XOS_CloseFile(fdTemp);
            XOS_CloseFile(fd);
            return FALSE;
        }

        ulRet = XOS_SeekFile(fd, XOS_SEEK_SET, tFileHead.dwLogBegin);
        if (ulRet != XOS_SUCCESS)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, 
                "[InitUserLogFile] XOS_SeekFile exec failed! ulRet = %d, line = %d\n", ulRet, __LINE__);
            XOS_CloseFile(fdTemp);
        XOS_CloseFile(fd);
            return FALSE;
        }
        
        dwOffset = tFileHeadTemp.dwLogBegin;
        for (i = 0; i < tFileHead.dwLogNum; i++)
        {
            /* 从原文件中读取每条记录 */
            memset(&tUserLog, 0, sizeof(TYPE_USER_LOG));
            ulRet = XOS_ReadFile(fd, (CHAR *)&tUserLog, sizeof(TYPE_USER_LOG), &sdwByteCount);
            if (ulRet != XOS_SUCCESS)
            {
                XOS_CloseFile(fdTemp);
                XOS_CloseFile(fd);
                return FALSE;
            }
            if (sdwByteCount != sizeof(TYPE_USER_LOG))
            {
                XOS_CloseFile(fdTemp);
                XOS_CloseFile(fd);
                return FALSE;
            }

            /* 如果退出理由是0，则需要修改为ERROR */
            if (USER_LOGOUT_NONE == tUserLog.ucQuitReason)
                tUserLog.ucQuitReason = USER_LOGOUT_ERROR;

            /* 写入临时文件 */
            if (XOS_SeekFile(fdTemp, XOS_SEEK_SET, dwOffset) != XOS_SUCCESS)
            {
                XOS_CloseFile(fdTemp);
                XOS_CloseFile(fd);
                return FALSE;
            }

            ulRet = XOS_WriteFile(fdTemp, (CHAR *)&tUserLog,sizeof(TYPE_USER_LOG), &sdwByteCount);
            if (ulRet != XOS_SUCCESS)
            {
                XOS_CloseFile(fdTemp);
                XOS_CloseFile(fd);
                return FALSE;
            }
            if (sdwByteCount != sizeof(TYPE_USER_LOG))
            {
                XOS_CloseFile(fdTemp);
                XOS_CloseFile(fd);
                return FALSE;
            }
            dwOffset = dwOffset + sizeof(TYPE_USER_LOG);
    
}

        XOS_CloseFile(fdTemp);
        XOS_CloseFile(fd);

        /* 删除原文件，将临时文件更名为正式文件 */
        XOS_DeleteFile(acUserLogFile);
        XOS_RenameFile(acUserLogFileTemp, acUserLogFile);

    }
    
    return TRUE;
}


/**************************************************************************
* 函数名称：GetDatUsableLangOffSetAndType
* 功能描述：根据语言类型获取指定文件中可用语言类型的偏移量和可用语言类型
* 访问的表：无
* 修改的表：无
* 输入参数：pcFileName -- dat文件名
* 输出参数：pcOutLoadLangType --可用的语言类型名
                              dwOutOffSet -- 可用语言类型在文件中的偏移量
* 返 回 值：   true --成功
                              false -- 失败
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2010/12/22   V1.0    殷浩      创建
**************************************************************************/
static BOOLEAN GetDatUsableLangOffSetAndType(CHAR *pcFileName, T_Cli_LangType *ptOutLoadLangType)
{
    XOS_STATUS dwRet = XOS_SUCCESS;
    XOS_FD fdDat;
    TYPE_FILE_HEAD tFileHead;
    SWORD32 dwOffsetEnglish = -1;
    SWORD32 dwOffsetChinese = -1;
    SWORD32 sdwFileLength = 0;
    BYTE aucFirstLang[OAM_CLI_LANGTYPE_LEN] = {0};
    T_Cli_LangType tLangType;
	
    if (!pcFileName || !ptOutLoadLangType)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "GetDatUsableLangOffSetAndType: parameter error!");
        return FALSE;
    }

    if (!OAM_CliGetLanguageType(&tLangType))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "GetDatUsableLangOffSetAndType: get system lang type from database failed!");
        return FALSE;
    }
	
    dwRet = XOS_GetFileLength(pcFileName, &sdwFileLength);
    if (dwRet != XOS_SUCCESS)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "GetDatUsableLangOffSetAndType: get length of file %s Fail! return %d",
                   pcFileName, dwRet);
        return FALSE;
    }
	
    /* open the script file */
    dwRet = XOS_OpenFile(pcFileName, XOS_RDONLY, &fdDat);
    if (dwRet != XOS_SUCCESS)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "GetDatUsableLangOffSetAndType: Read %s Fail! return %d",
                   pcFileName, dwRet);
        return FALSE;
    }
	
    /*初始化出参*/
    ptOutLoadLangType->swLangOffSet = 0;
    while (ptOutLoadLangType->swLangOffSet < sdwFileLength)
    {	
        /* File Head */
        memset(&tFileHead, 0, sizeof(tFileHead));
        if (!Read_CmdFileHead(fdDat, &tFileHead, ptOutLoadLangType->swLangOffSet))
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR,"GetDatUsableLangOffSetAndType: Read CmdFileHead Fail!");
            XOS_CloseFile(fdDat);
            return FALSE;
        }

        if (ptOutLoadLangType->swLangOffSet== 0)
        { 
            /*记录第一个脚本的语言名称*/
            snprintf((CHAR *)aucFirstLang, OAM_CLI_LANGTYPE_LEN, "%s", tFileHead.aucLanguageVer);
        }
    
        if ((strlen((CHAR *)tFileHead.aucLanguageVer) == 0) && (ptOutLoadLangType->swLangOffSet == 0))
        { 
            /*是无语言类型的老格式脚本*/
            snprintf(ptOutLoadLangType->aucLangType, OAM_CLI_LANGTYPE_LEN, OAM_CLI_LANGTYPE_OLDFMT);
            break;
        }
        else if (Oam_String_Compare_Nocase((CHAR *)tFileHead.aucLanguageVer, tLangType.aucLangType, OAM_CLI_LANGTYPE_LEN) == 0)
        {  
            /*找到匹配格式的脚本*/
            snprintf(ptOutLoadLangType->aucLangType, OAM_CLI_LANGTYPE_LEN, "%s", tLangType.aucLangType);
            break;
        }
        else if (Oam_String_Compare_Nocase((CHAR *)tFileHead.aucLanguageVer, OAM_CLI_LANGTYPE_ENGLISH, OAM_CLI_LANGTYPE_LEN) == 0)
        {
            /*记录英文脚本偏移量*/
            dwOffsetEnglish = ptOutLoadLangType->swLangOffSet;
        }
        else if (Oam_String_Compare_Nocase((CHAR *)tFileHead.aucLanguageVer, OAM_CLI_LANGTYPE_CHINESE, OAM_CLI_LANGTYPE_LEN) == 0)
        {
            /*记录中文脚本偏移量*/
            dwOffsetChinese = ptOutLoadLangType->swLangOffSet;
        }
		
         ptOutLoadLangType->swLangOffSet += tFileHead.dwCurLangDatSize;
    }

    /*如果没有匹配到，按照english/chinese/第一个 这样的顺序取*/
    if (ptOutLoadLangType->swLangOffSet >= sdwFileLength)
    {
        if (dwOffsetEnglish >= 0)
        {
            snprintf(ptOutLoadLangType->aucLangType, OAM_CLI_LANGTYPE_LEN, OAM_CLI_LANGTYPE_ENGLISH);
            ptOutLoadLangType->swLangOffSet = dwOffsetEnglish;
        }
        else if (dwOffsetChinese >= 0)
        {
            snprintf(ptOutLoadLangType->aucLangType, OAM_CLI_LANGTYPE_LEN, OAM_CLI_LANGTYPE_CHINESE);
            ptOutLoadLangType->swLangOffSet = dwOffsetChinese;
        }
        else
        {
            snprintf(ptOutLoadLangType->aucLangType, OAM_CLI_LANGTYPE_LEN, "%s", aucFirstLang);
            ptOutLoadLangType->swLangOffSet = 0;
        }
    }

    XOS_CloseFile(fdDat);
    return TRUE;
}

/*
功能：根据map type+value 获取对应的字符串
参数：wType  -- map类型
      wValue -- map值
返回：
      NULL -- 获取失败
      非NULL -- 字符串指针
*/
CHAR * GetMapString(WORD16 wType, WORD16 wValue)
{
    int iCnt;
    if (g_tDatLink == NULL)
    {
        return NULL;
    }

    for (iCnt = 0; iCnt < g_ptOamIntVar->wDatSum; iCnt++)
    {
        T_OAM_Cfg_Dat *pDatTmp = &(g_tDatLink[iCnt].datIndex);
		
        /* find */
        WORD32 dwItemPos = FindMapItemPos(pDatTmp, FindMapTypePos(pDatTmp, wType), wValue);
        /* return */
        if (dwItemPos > 0)
        {
            return OAM_LTrim(OAM_RTrim(pDatTmp->tMapString + pDatTmp->tMapItem[dwItemPos].dwKeyPos));
        }
    }

    return NULL;
}

/*扫描指定目录下的dat文件，生成文件信息列表*/
#define OAM_SCRIPT_FILE_NAME    "oam-NLS.dat"
BOOLEAN GetScriptFileList(char *CfgDir)
{
    CHAR  acOamDatCfgFile[FILE_PATH_LEN] = {'\0'};
    sprintf(acOamDatCfgFile, "%s/%s", CfgDir, OAM_CFG_DAT_CONFIG_FILE);	
    XOS_DeleteFile(acOamDatCfgFile);
    XOS_FD cfgFd;
		
    XOS_STATUS dwRet = XOS_OpenFile(acOamDatCfgFile, XOS_CREAT, &cfgFd);
    if (dwRet != XOS_SUCCESS)
    {   
        XOS_SysLog(OAM_CFG_LOG_ERROR, "ERROR::Open file %s failed!", acOamDatCfgFile);
        return FALSE;
    }

    struct dirent *dir_env;

    DIR *dir = opendir(CfgDir);   

    CHAR aucTmpRecord[1024] = {0};
    int iIdx = 2;

    while((dir_env = readdir(dir)))
    {
      if (strstr(dir_env->d_name, ".dat") == NULL)
        {
            continue;
        }
 
        int iTmpIdx;
        if ((Oam_String_Compare_Nocase(dir_env->d_name, OAM_SCRIPT_FILE_NAME, strlen(OAM_SCRIPT_FILE_NAME))) == 0)
        {
            iTmpIdx = 1;
        }
        else
        {
            iTmpIdx = iIdx;
            iIdx++;
        }

        memset(aucTmpRecord, 0, sizeof(aucTmpRecord));
        XOS_snprintf(aucTmpRecord, sizeof(aucTmpRecord), "%u, %s/%s\n", iTmpIdx, CfgDir, dir_env->d_name);

        SWORD32 dwCountBeWrite;
        XOS_WriteFile(cfgFd, aucTmpRecord, strlen(aucTmpRecord), &dwCountBeWrite);
    }

    XOS_CloseFile(cfgFd);

    return TRUE;
}
