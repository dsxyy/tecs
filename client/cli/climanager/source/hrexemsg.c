/**************************************************************************
* 版权所有 (C)2002, 深圳市中兴通讯股份有限公司。
*
* 文件名称：HrExeMsg.c
* 文件标识：
* 内容摘要：处理来自Execute的消息
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
#include "saveprtclcfg.h"
#include "convertencoding.h"

/*lint -e420*/
/*lint -e669*/
/*lint -e416*/

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
/**************************************************************************
*                          局部函数原型                                   *
**************************************************************************/
/**************************************************************************
*                          局部函数实现                                   *
**************************************************************************/

/**************************************************************************
* 函数名称：int XmlProcess_Return_Data
* 功能描述：处理返回数据(专用于XML处理)
* 访问的表：无
* 修改的表：无
* 输入参数：TYPE_LINK_STATE *pLinkState,MSG_COMM_OAM * ptMsg
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/03/02    V1.0    齐龙兆      创建
**************************************************************************/
int XmlProcess_Return_Data(TYPE_XMLLINKSTATE *pXmLinkState,MSG_COMM_OAM * ptMsg)
{
    TYPE_LINK_STATE *pLinkStateTmp = NULL;
    CHAR *ptrOutputResultBuf = NULL;/*回显串缓冲区*/
    INT iBufLen = 0;                            /*回显串缓冲区大小*/
    CHAR *ptrUTF8Buf = NULL;            /*UTF-8格式字符串缓冲区*/
    CHAR *pFormatStr = NULL;
    int iRtnValue = DISPLAY_SUCCESS;

    /* 获取回显格式串 */
    pFormatStr = GetFormatByCmdAttrPos(pXmLinkState->tCurCMDInfo.wCmdAttrPos, pXmLinkState->tCurCMDInfo.cfgDat);    
    if (!pFormatStr)
    {
        return ERROR_INDIS;
    }
    pLinkStateTmp = &gtLinkState[MAX_CLI_VTY_NUMBER];
    Oam_InitOneLinkForTxtRestore(pLinkStateTmp);
    /* 根据回显格式串生成回显字符串 */
    pLinkStateTmp->tMatchResult.cfgDat = pXmLinkState->tCurCMDInfo.cfgDat;
    iRtnValue = OamCfgintpSendToBuffer(pLinkStateTmp, ptMsg, pFormatStr,&pLinkStateTmp->tDispGlobalPara);
    /*将获取的回显串转换为UTF-8格式*/	
    ptrOutputResultBuf = pXmLinkState->tDispGlobalPara.pDisplayBuffer;
    if ('\0' == ptrOutputResultBuf[0])
    {
        return iRtnValue;
    }
    iBufLen = strlen(ptrOutputResultBuf);
    iBufLen *= 2;
    ptrUTF8Buf = (CHAR *)XOS_GetUB(iBufLen);
    if (NULL == ptrUTF8Buf)
    {
        return ERROR_INDIS;
    }
    memset(ptrUTF8Buf,0,iBufLen);
    if (!Gb2312ToUtf8(ptrOutputResultBuf, iBufLen,ptrUTF8Buf))
    {
        OAM_RETUB(ptrUTF8Buf);
	 return ERROR_INDIS;
    }
    memset(ptrOutputResultBuf,0,MAX_OUTPUT_RESULT);
    strncpy(ptrOutputResultBuf,ptrUTF8Buf,MAX_OUTPUT_RESULT);    
    /*释放缓冲区*/	
    OAM_RETUB(ptrUTF8Buf);    
    return iRtnValue;
}

/**************************************************************************
*                          全局函数实现                                   *
**************************************************************************/

/*lint +e420*/
/*lint +e669*/
/*lint +e416*/



