/********************************************************************************
*   FileName:   pe_Para_Convert.c
*   Project:    CLI Parsing Engine
*   Author:
*   CreateTime: 01/2/28
*   Note:       This file is the source file of converting string to parameter.
********************************************************************************/
#include <math.h>
#include "includes.h"

/*lint -e123*/
/*lint -e959*/
/*lint -e968*/
/*lint -e922*/
/*lint -e578*/
/*lint -e539*/
/*lint -e931*/

/*和brs/pub/include/sockets.h 一致*/
#define BRS_AF_INET6    24      /* IP version 6 */

static CHAR gpCnvtTempStr1[MAX_STRING_LEN];
static CHAR gpCnvtTempStr2[MAX_STRING_LEN];
static CHAR gpCnvtTempStr3[MAX_STRING_LEN];

static int ConvertStrToTypeByte(BYTE ucLinkID,CHAR *pString, TYPE_PARA* pParaInfo,WORD16 wParaNo,CMDPARA *pCmdPara);
static int ConvertStrToTypeWord(BYTE ucLinkID,CHAR *pString,TYPE_PARA* pParaInfo,WORD16 wParaNo, CMDPARA *pCmdPara);
static int ConvertStrToTypeDWord(BYTE ucLinkID,CHAR *pString, TYPE_PARA* pParaInfo,WORD16 wParaNo,CMDPARA *pCmdPara);
static int ConvertStrToTypeChar(BYTE ucLinkID,CHAR *pString,TYPE_PARA* pParaInfo,WORD16 wParaNo,CMDPARA *pCmdPara);
static int ConvertStrToTypeString(BYTE ucLinkID,CHAR *pString, TYPE_PARA* pParaInfo,WORD16 wParaNo,CMDPARA *pCmdPara,BYTE bDeterminer);
static int ConvertStrToTypeIpaddr(BYTE ucLinkID,CHAR * pString,TYPE_PARA* pParaInfo,WORD16 wParaNo,CMDPARA *pCmdPara);
static int ConvertStrToTypeDate(BYTE ucLinkID,CHAR * pString,TYPE_PARA* pParaInfo,WORD16 wParaNo,CMDPARA *pCmdPara);
static int ConvertStrToTypeTime(BYTE ucLinkID,CHAR * pString,TYPE_PARA* pParaInfo,WORD16 wParaNo,CMDPARA *pCmdPara);
static int ConvertStrToTypeMap(BYTE ucLinkID,CHAR *pString,TYPE_PARA* pParaInfo,T_OAM_Cfg_Dat * cfgDat,WORD16 wParaNo,CMDPARA *pCmdPara);
static int ConvertStrToTypeConst(CHAR *pString, TYPE_PARA* pParaInfo,CMDPARA *pCmdPara);
static int ConvertStrToTypeHex(BYTE ucLinkID,CHAR * pString, TYPE_PARA* pParaInfo,WORD16 wParaNo,CMDPARA *pCmdPara);
static int ConvertStrToTypeIpaddrMask(BYTE ucLinkID,CHAR * pString, TYPE_PARA* pParaInfo,WORD16 wParaNo,CMDPARA *pCmdPara);
static int ConvertStrToTypeIpaddrIMask(BYTE ucLinkID,CHAR * pString, TYPE_PARA* pParaInfo,WORD16 wParaNo,CMDPARA *pCmdPara);
static int ConvertStrToTypeMacAddr(BYTE ucLinkID,CHAR * pString,TYPE_PARA* pParaInfo,WORD16 wParaNo,CMDPARA *pCmdPara);
static int ConvertStrToTypeFloat(BYTE ucLinkID,CHAR *pString, TYPE_PARA* pParaInfo,WORD16 wParaNo,CMDPARA *pCmdPara);
static int ConvertStrToTypeIpv6prefix(BYTE ucLinkID,CHAR * pString,TYPE_PARA* pParaInfo,WORD16 wParaNo,CMDPARA *pCmdPara);
static int ConvertStrToTypeIpv6addr(BYTE ucLinkID,CHAR * pString,TYPE_PARA* pParaInfo,WORD16 wParaNo,CMDPARA *pCmdPara);
static int ConvertStrToTypeVPNASN(BYTE ucLinkID,CHAR * pString,TYPE_PARA* pParaInfo,WORD16 wParaNo,CMDPARA *pCmdPara);
static int ConvertStrToTypeVPNIP(BYTE ucLinkID,CHAR * pString,TYPE_PARA* pParaInfo,WORD16 wParaNo,CMDPARA *pCmdPara);
static int ConvertStrToTypeWord64(BYTE ucLinkID,CHAR * pString,TYPE_PARA* pParaInfo,WORD16 wParaNo,CMDPARA *pCmdPara);
static int ConvertStrToTypeSword(BYTE ucLinkID,CHAR * pString,TYPE_PARA* pParaInfo,WORD16 wParaNo,CMDPARA *pCmdPara);
static int ConvertStrToTypeLogicAddrM(BYTE ucLinkID,CHAR * pString,TYPE_PARA* pParaInfo,WORD16 wParaNo,CMDPARA *pCmdPara);
static int ConvertStrToTypeLogicAddrU(BYTE ucLinkID,CHAR * pString,TYPE_PARA* pParaInfo,WORD16 wParaNo,CMDPARA *pCmdPara);
static int ConvertStrToTypeIpvxaddr(BYTE ucLinkID,CHAR * pString,TYPE_PARA* pParaInfo,WORD16 wParaNo,CMDPARA *pCmdPara);
static int ConvertStrToTypeSysClock(BYTE ucLinkID,CHAR * pString,TYPE_PARA* pParaInfo,WORD16 wParaNo,CMDPARA *pCmdPara);

static int ConvertStrToAddr(CHAR * pString, BYTE Address[]);
static int ConvertStrToIpv6Prefix(CHAR * pString, BYTE Address[], BYTE *pPrefixLen);
static int ConvertStrToIpv6Addr(CHAR * pString, BYTE Address[]);
static int ConvertStrToAddrArray(CHAR * pString, BYTE Address[]);
    
static int ConvertStrToTypePhyAddress(BYTE ucLinkID,CHAR * pString,TYPE_PARA* pParaInfo,WORD16 wParaNo,CMDPARA *pCmdPara);
static int ConvertStrToTypeLogicAddress(BYTE ucLinkID,CHAR * pString,TYPE_PARA* pParaInfo,WORD16 wParaNo,CMDPARA *pCmdPara);

static int MaskJudgement(BYTE mask[]);
static int Mac_GetHexValue(BYTE *szHex, int iLen, BYTE *bHex);

static int Convert_atoi(CHAR * pString, WORD64 * pQdword);
static int Convert_atoh(CHAR * pString, BYTE * pValue1, BYTE * pValue2);
static BOOL CheckDataString(const CHAR *strData);

/*用来控制是否打印出dat的匹配中的匹配到的type和value*/
BYTE gDatmatch=0;

/* convert strings to data type byte */
int ConvertStrToTypeByte(BYTE ucLinkID,CHAR *pString, TYPE_PARA* pParaInfo,WORD16 wParaNo,CMDPARA *pCmdPara)
{
    int nReturnVal = 0;
    BYTE *pStore = NULL;
    WORD64 qdwTemp = 0;
    
    if ((pString == NULL) || (pParaInfo == NULL) || (pCmdPara == NULL))
    {
        return PARAERR_CONVERT;
    }
    
    /* 1.convert */
    if (PARA_SUCCESS != Convert_atoi(pString, &qdwTemp))
    {
        nReturnVal = PARAERR_CONVERT;
        return nReturnVal;
    }
    /* 2.verify */
    if (qdwTemp > pParaInfo->Max.qdwMaxVal || qdwTemp < pParaInfo->Min.qdwMinVal)
    {
        nReturnVal = PARAERR_OVERFLOW;
        return nReturnVal;
    }
    /* 3.store  */
    pStore = (BYTE *) Oam_InptGetBuf(sizeof(BYTE),ucLinkID);
    *pStore = (BYTE)qdwTemp;
    pCmdPara->ParaNo = wParaNo;
    pCmdPara->Type   = DATA_TYPE_BYTE;
    pCmdPara->Len    = sizeof(BYTE);
    pCmdPara->pValue = (BYTE *)pStore;

    return nReturnVal;
}

/* convert strings to data type word*/
int ConvertStrToTypeWord(BYTE ucLinkID,CHAR *pString,TYPE_PARA* pParaInfo,WORD16 wParaNo, CMDPARA *pCmdPara)
{
    int nReturnVal = 0;
    WORD16 *pStore = NULL;
    WORD64 qdwTemp = 0;
    
    if ((pString == NULL) || (pParaInfo == NULL) || (pCmdPara == NULL))
    {
        return PARAERR_CONVERT;
    }    

    /* 1.convert */
    if (PARA_SUCCESS != Convert_atoi(pString, &qdwTemp))
    {
        nReturnVal = PARAERR_CONVERT;
        return nReturnVal;
    }
    /* 2.verify */
    if (qdwTemp > pParaInfo->Max.qdwMaxVal || qdwTemp < pParaInfo->Min.qdwMinVal)
    {
        nReturnVal = PARAERR_OVERFLOW;
        return nReturnVal;
    }
    /* 3.store  */
    pStore = (WORD16 *) Oam_InptGetBuf(sizeof(WORD16),ucLinkID);
    *pStore = (WORD16) qdwTemp;
    pCmdPara->ParaNo = wParaNo;
    pCmdPara->Type   = DATA_TYPE_WORD;
    pCmdPara->Len    = sizeof(WORD16);
    pCmdPara->pValue = (BYTE *)pStore;

    return nReturnVal;
}

/* convert strings to data type dword or int */
int ConvertStrToTypeDWord(BYTE ucLinkID,CHAR *pString, TYPE_PARA* pParaInfo,WORD16 wParaNo,CMDPARA *pCmdPara)
{
    int nReturnVal = 0;
    int *pStore = NULL;
    WORD64 qdwTemp = 0;

    if ((pString == NULL) || (pParaInfo == NULL) || (pCmdPara == NULL))
    {
        return PARAERR_CONVERT;
    }	
    
    /* 1.convert */
    if (PARA_SUCCESS != Convert_atoi(pString, &qdwTemp))
    {
        nReturnVal = PARAERR_CONVERT;
        return nReturnVal;
    }
    /* 2.verify */
    if (qdwTemp > pParaInfo->Max.qdwMaxVal || qdwTemp < pParaInfo->Min.qdwMinVal)
    {
        nReturnVal = PARAERR_OVERFLOW;
        return nReturnVal;
    }
    /* 3.store  */
    pStore = (int *) Oam_InptGetBuf(sizeof(int),ucLinkID);
    * pStore = (DWORD)qdwTemp;
    pCmdPara->ParaNo = wParaNo;
    pCmdPara->Type   = pParaInfo->bParaType;
    pCmdPara->Len    = sizeof(int);
    pCmdPara->pValue = (BYTE *)pStore;

    return nReturnVal;
}

/* convert strings to data type CHAR */
int ConvertStrToTypeChar(BYTE ucLinkID,CHAR *pString,TYPE_PARA* pParaInfo,WORD16 wParaNo,CMDPARA *pCmdPara)
{
    int nReturnVal = 0;
    BYTE *pStore = NULL;
    CHAR cChar = 0;
    int nLen = 0;

    if ((pString == NULL) || (pParaInfo == NULL) || (pCmdPara == NULL))
    {
        return PARAERR_CONVERT;
    }
    nLen = strlen(pString);	
    /* 1.convert */
    if (nLen != 1)
    {
        nReturnVal = PARAERR_CONVERT;
        return nReturnVal;
    }
    cChar = *pString;
    /* 2.verify */
    if ((BYTE)cChar > pParaInfo->Max.qdwMaxVal || (BYTE)cChar < pParaInfo->Min.qdwMinVal)
    {
        nReturnVal = PARAERR_OVERFLOW;
        return nReturnVal;
    }
    /* 3.store  */
    pStore = (BYTE *) Oam_InptGetBuf(sizeof(BYTE),ucLinkID);
    *pStore = (BYTE)cChar;
    pCmdPara->ParaNo = wParaNo;
    pCmdPara->Type   = DATA_TYPE_BYTE;
    pCmdPara->Len    = sizeof(BYTE);
    pCmdPara->pValue = (BYTE *)pStore;

    return nReturnVal;

}

/* convert strings to data type string */
int ConvertStrToTypeString(BYTE ucLinkID,CHAR *pString, TYPE_PARA* pParaInfo,WORD16 wParaNo,CMDPARA *pCmdPara,BYTE bDeterminer)
{
    int nReturnVal = PARA_SUCCESS;
    int nLen = 0;
    CHAR * pStore = NULL;
    CHAR aucTmpStr[MAX_STRING_LEN+3] = {0};
	
    if ((pString == NULL) || (pParaInfo == NULL) || (pCmdPara == NULL) )
    {
        return PARAERR_CONVERT;
    }
    nLen = strlen(pString);
    /*对输入字符串中的引号、转义符进行处理*/
    if ((*pString != MARK_TYPE_STRING) || 
         !g_ptOamIntVar->bSupportStringWithBlank ||
         (bDeterminer == DM_QUESTION))
    {
        /*如果不是引号开头，
           或不支持空格字符串，
           或是question请求，
           直接拷贝到本地变量*/
        if (nLen > (MAX_STRING_LEN - 1))
        {
            return PARAERR_CONVERT;
        }
        memcpy(aucTmpStr, pString, (size_t)nLen); 
    }
    else
    {
        WORD wCnt = 0;
        BYTE ucTmp = 0;
        CHAR *pcTmpIdx = aucTmpStr;
        BOOLEAN bPreIsTrans = FALSE;

        /*算上两个引号*/
        if (nLen > (MAX_STRING_LEN - 1 + 2))
        {
            return PARAERR_CONVERT;
        }
        
        /*跳过开头引号, 从1开始算*/
        for (wCnt = 1; wCnt < nLen; wCnt++)
        {
            ucTmp = *(pString + wCnt);

            if (MARK_TYPE_STRING == ucTmp)
            {
                if (bPreIsTrans)
                {/*如果前一个读入的是转义符，替换前一个读取的为引号*/
                    *(pcTmpIdx-1) = ucTmp;
                    bPreIsTrans = FALSE;
                    continue;
                }
                else
                {/*如果前一个不是转义符，结束读取操作*/
                    break;
                }
            }
            else if (MARK_STRING_TRANS == ucTmp)
            {
                if (bPreIsTrans)
                {/*如果前一个读入的是转义符，对当前字符转义为普通字符*/
                    bPreIsTrans = FALSE;
                    continue;
                }
                else
                {
                    *pcTmpIdx = ucTmp;
                    pcTmpIdx++;
                    bPreIsTrans = TRUE;
                }
            }
            else if (MARK_CHAR_QUESTION == ucTmp)
            {
                if (bPreIsTrans)
                {/*如果前一个读入的是转义符，替换前一个读取的为问号*/
                    *(pcTmpIdx-1) = ucTmp;
                    bPreIsTrans = FALSE;
                    continue;
                }
                else
                {
                    *pcTmpIdx = ucTmp;
                    pcTmpIdx++;
                }
            }
            else
            {/*普通字符，直接读入*/
                /*如果转义符后跟普通字符，报错*/
                if (bPreIsTrans)
                {
                    /*bPreIsTrans = FALSE;*/
                    return PARAERR_CONVERT;
                }

                *pcTmpIdx = ucTmp;    
                pcTmpIdx++;            
            }
        }

        /*如果是中间退出，返回失败
        */
        if ((wCnt == nLen) || 
            ((wCnt < nLen) && *(pString + wCnt + 1) != '\0'))
        {
            return PARAERR_CONVERT;
        }
    }

    nLen = strlen(aucTmpStr);    
    /* 1.convert */
    if (nLen == 0)
    {
        nReturnVal = PARAERR_NULLSTRING;
        return nReturnVal;
    }
    /* 2.verify */
    if ((unsigned)nLen < pParaInfo->Min.qdwMinVal || (unsigned)nLen > pParaInfo->Max.qdwMaxVal)
    {       
        nReturnVal = PARAERR_OVERFLOW;
        return nReturnVal;
    }
    /* 3.store  */
    nLen++; /* MARK_STRINGEND */
    pStore = (CHAR *) Oam_InptGetBuf(nLen,ucLinkID);
    memcpy(pStore, aucTmpStr, (size_t)(nLen - 1)); /* MARK_STRINGEND */
    pStore[nLen - 1] = MARK_STRINGEND;
    pCmdPara->ParaNo = wParaNo;
    pCmdPara->Type   = DATA_TYPE_STRING;
    pCmdPara->Len    = nLen;
    pCmdPara->pValue = (BYTE *)pStore;

    return nReturnVal;
}

#if 0
/* convert strings to data type text */
int ConvertStrToTypeText(CHAR *pString, TYPE_MATCH_RESULT *pMatchResult, TYPE_CMD_LINE *pCmdLine)
{
    TYPE_PARA * pParaInfo = NULL;   /* information about the command parameter */
    CMDPARA *   pCmdPara = NULL;    /* store one matched parameter */
    WORD16 wParaNo = 0;
    T_OAM_Cfg_Dat * cfgDat = pMatchResult->cfgDat;

    int nReturnVal = 0;
    int nTotalLen = 0;
    int nLen = 0;
    CHAR * pStore = NULL;
    CHAR *strTemp = gpCnvtTempStr1;
    CHAR *strSpace = gpCnvtTempStr2;

    *strTemp = MARK_STRINGEND;
    *strSpace = MARK_STRINGEND;
    pParaInfo = (TYPE_PARA *) &(cfgDat->tCmdPara[pMatchResult->wCurParaPos].uContent.tPara);
    pCmdPara = (CMDPARA *) &(pMatchResult->tPara[pMatchResult->wParaSum]);
    wParaNo = cfgDat->tCmdPara[pMatchResult->wCurParaPos].ParaNo;

    while(*pString != MARK_STRINGEND)
    {
        /* get the length */
        nLen = strlen(pString);
        nTotalLen += strlen(pString);
        if (nTotalLen > pParaInfo->wLen)
        {
            nReturnVal = PARAERR_OVERFLOW;
            break;
        }
        if (nTotalLen == 0)
        {
            nReturnVal = PARAERR_NULLSTRING;
            break;
        }

        strncat(strTemp, pString, (size_t)nLen);

        pString = pString + ((nLen >= MAX_STRING_LEN) ? MAX_STRING_LEN : nLen);
        if(*pString != MARK_STRINGEND)
        {
            pMatchResult->wDepth++;
            memset(strSpace, MARK_BLANK, (size_t)pCmdLine->wBlankSum[pMatchResult->wDepth]);
            nTotalLen += pCmdLine->wBlankSum[pMatchResult->wDepth];
            strncat(strTemp, strSpace, (size_t)pCmdLine->wBlankSum[pMatchResult->wDepth]);
        }
        else
        {
            memset(strSpace, MARK_BLANK, (size_t)pCmdLine->wBlankSum[pMatchResult->wDepth + 1]);
            strncat(strTemp, strSpace, (size_t)pCmdLine->wBlankSum[pMatchResult->wDepth + 1]);
        }
    }

    nLen = strlen(strTemp);
    nLen++;
    pStore = (CHAR *) Oam_InptGetBuf(nLen);
    memset(pStore, 0, (size_t)nLen);
    strncat(pStore, strTemp, (size_t)nLen);

    pCmdPara->ParaNo    = wParaNo;
    pCmdPara->Type      = DATA_TYPE_TEXT;
    pCmdPara->Len       = nLen;
    pCmdPara->pValue    = (BYTE *)pStore;

    return nReturnVal;
}
#endif

/* convert string to data type ip address */
int ConvertStrToTypeIpaddr(BYTE ucLinkID,CHAR * pString,TYPE_PARA* pParaInfo,WORD16 wParaNo,CMDPARA *pCmdPara)
{
    int nReturnVal ;
    WORD32 dwIPAddr = 0;
    BYTE ipAddr[LEN_IPADDR];
    CHAR *pSubStr = NULL;
    BYTE * pValue = NULL;

    if ((pString == NULL) || (pParaInfo == NULL) || (pCmdPara == NULL))
    {
        return PARAERR_CONVERT;
    }   

    /* set initial value */
    memset(ipAddr, 0, (size_t)LEN_IPADDR);
    pSubStr = pString;

    nReturnVal = ConvertStrToAddr(pSubStr, ipAddr);
    if(nReturnVal != PARA_SUCCESS)
        return nReturnVal;

    /*例如1.2.3.4 读在数组里面从0~3分别放 1 2 3 4
	这里组成主机字节序发送，即 0x01020304*/
    dwIPAddr = (ipAddr[0] << 24) | (ipAddr[1] << 16)| (ipAddr[2] << 8)| ipAddr[3] ;
	
    pValue = (BYTE *) Oam_InptGetBuf(LEN_IPADDR,ucLinkID);
    /*memcpy(pValue, ipAddr, LEN_IPADDR);*/
    pCmdPara->ParaNo    = wParaNo;
    pCmdPara->Type      = DATA_TYPE_IPADDR;
    pCmdPara->Len       = LEN_IPADDR;

    /*按照主机序发送
    dwIPAddr = XOS_InvertWord32(dwIPAddr);*/
    memcpy(pValue, &dwIPAddr,LEN_IPADDR);
    pCmdPara->pValue    = pValue;

    return PARA_SUCCESS;
}

/* convert string to data type date */
int ConvertStrToTypeDate(BYTE ucLinkID,CHAR * pString,TYPE_PARA* pParaInfo,WORD16 wParaNo,CMDPARA *pCmdPara)
{
    CHAR *pTempStr = gpCnvtTempStr1;
    CHAR *pSubStr = NULL;
    int nReturnVal = PARA_SUCCESS;
    int nPos = 0;
    WORD64 qdwTemp = 0;

    WORD16 wYear  = 0;
    BYTE bMonth = 0;
    BYTE bDay   = 0;
    WORD32 dDate = 0;
    OAM_Clock_STRUCT tAbsClock;
    WORD32 * pValue = NULL;

    if ((pString == NULL) || (pParaInfo == NULL) || (pCmdPara == NULL))
    {
        return PARAERR_CONVERT;
    }

    /* set initial value */
    *pTempStr = MARK_STRINGEND;
    pSubStr = pString;

    /* get and check the year, month and day */
    /* 1.month */
    nReturnVal = OamCfgFindCharInString(pSubStr, (CHAR)MARK_HYPHEN, &nPos);
    if (nReturnVal != PARA_SUCCESS)
        return nReturnVal;
    memcpy(pTempStr, pSubStr, (size_t)nPos);
    pTempStr[nPos] = MARK_STRINGEND;
    if (PARA_SUCCESS != Convert_atoi(pTempStr, &qdwTemp))
        return PARAERR_CONVERT;
    if (qdwTemp < 1 || qdwTemp > 12) /* check */
        return PARAERR_OVERFLOW;
    bMonth = (BYTE) qdwTemp;
    pSubStr += nPos + 1;
    nPos = 0;
    *pTempStr = MARK_STRINGEND;
    /* 2.day */
    nReturnVal = OamCfgFindCharInString(pSubStr, (CHAR)MARK_HYPHEN, &nPos);
    if (nReturnVal != PARA_SUCCESS)
        return nReturnVal;
    memcpy(pTempStr, pSubStr, (size_t)nPos);
    pTempStr[nPos] = MARK_STRINGEND;
    if (PARA_SUCCESS != Convert_atoi(pTempStr, &qdwTemp))
        return PARAERR_CONVERT;
    if (qdwTemp < 1 || qdwTemp > 31) /* check */
        return PARAERR_OVERFLOW;
    bDay = (BYTE) qdwTemp;
    pSubStr += nPos + 1;
    nPos = 0;
    *pTempStr = MARK_STRINGEND;
    /* 3.year */
    strncpy(pTempStr, pSubStr, sizeof(gpCnvtTempStr1));
    if (PARA_SUCCESS != Convert_atoi(pTempStr, &qdwTemp))
        return PARAERR_CONVERT;
    if (qdwTemp < 2000 || qdwTemp > 2130) /* check */
        return PARAERR_OVERFLOW;
    wYear = (WORD16) qdwTemp;
    /* check the rear ??? */
    pSubStr += strlen(pTempStr);
    if (strlen(pSubStr) != 0)
        return PARAERR_CONVERT;

    /* absolute time to relative */
    memset(&tAbsClock, 0, sizeof(OAM_Clock_STRUCT));
    tAbsClock.year  = wYear;
    tAbsClock.mon   = bMonth;
    tAbsClock.mday  = bDay;
    if (0 != OAM_ClockToDword(&tAbsClock, &dDate))
        return PARAERR_CONVERT;

    /* save the date */
    pValue = (WORD32 *) Oam_InptGetBuf(sizeof(WORD32),ucLinkID);
    * pValue = dDate;
    pCmdPara->ParaNo    = wParaNo;
    pCmdPara->Type      = DATA_TYPE_DATE;
    pCmdPara->Len       = sizeof(WORD32);
    pCmdPara->pValue    = (BYTE *)pValue;

    return PARA_SUCCESS;
}

/* convert string to data type time */
int ConvertStrToTypeTime(BYTE ucLinkID,CHAR * pString,TYPE_PARA* pParaInfo,WORD16 wParaNo,CMDPARA *pCmdPara)
{
    CHAR *pTempStr = gpCnvtTempStr1;
    CHAR *pSubStr = NULL;
    int nReturnVal = PARA_SUCCESS;
    int nPos = 0;
    WORD64 qdwTemp = 0;

    BYTE bHour      = 0;
    BYTE bMinute    = 0;
    BYTE bSecond    = 0;
    WORD32 dTime    = 0;
    OAM_Clock_STRUCT tAbsClock;
    WORD32 * pValue = NULL;

    if ((pString == NULL) || (pParaInfo == NULL) || (pCmdPara == NULL))
    {
        return PARAERR_CONVERT;
    }
    /* set initial value */
    *pTempStr = MARK_STRINGEND;
    pSubStr = pString;

    /* get and check the hour, minute and second */
    /* 1.hour */
    nReturnVal = OamCfgFindCharInString(pSubStr, (CHAR)MARK_COLON, &nPos);
    if (nReturnVal != PARA_SUCCESS)
        return nReturnVal;
    memcpy(pTempStr, pSubStr, (size_t)nPos);
    pTempStr[nPos] = MARK_STRINGEND;
    if (strlen(pTempStr) == 0 || strlen(pTempStr) > 2)
        return PARAERR_CONVERT;
    if (PARA_SUCCESS != Convert_atoi(pTempStr, &qdwTemp))
        return PARAERR_CONVERT;
    if (qdwTemp < 0 || qdwTemp > 23) /* check */
        return PARAERR_OVERFLOW;
    bHour = (BYTE) qdwTemp;
    pSubStr += nPos + 1;
    nPos = 0;
    *pTempStr = MARK_STRINGEND;
    /* 2.minute */
    nReturnVal = OamCfgFindCharInString(pSubStr, (CHAR)MARK_COLON, &nPos);
    if (nReturnVal != PARA_SUCCESS)
        return nReturnVal;
    memcpy(pTempStr, pSubStr, (size_t)nPos);
    pTempStr[nPos] = MARK_STRINGEND;
    if (strlen(pTempStr) == 0 || strlen(pTempStr) > 2)
        return PARAERR_CONVERT;
    if (PARA_SUCCESS != Convert_atoi(pTempStr, &qdwTemp))
        return PARAERR_CONVERT;
    if (qdwTemp < 0 || qdwTemp > 60) /* check */
        return PARAERR_OVERFLOW;
    bMinute = (BYTE) qdwTemp;
    pSubStr += nPos + 1;
    nPos = 0;
    *pTempStr = MARK_STRINGEND;
    /* 3.second */
    strncpy(pTempStr, pSubStr, sizeof(gpCnvtTempStr1));
    if (strlen(pTempStr) == 0 || strlen(pTempStr) > 2)
        return PARAERR_CONVERT;
    if (PARA_SUCCESS != Convert_atoi(pTempStr, &qdwTemp))
        return PARAERR_CONVERT;
    if (qdwTemp < 0 || qdwTemp > 60) /* check */
        return PARAERR_OVERFLOW;
    bSecond = (BYTE) qdwTemp;
    /* check the rear ??? */
    pSubStr += strlen(pTempStr);
    if (strlen(pSubStr) != 0)
        return PARAERR_CONVERT;

    /* absolute time to relative */
    memset(&tAbsClock, 0, sizeof(OAM_Clock_STRUCT));
    tAbsClock.hour  = bHour;
    tAbsClock.min   = bMinute;
    tAbsClock.sec   = bSecond;
    if (0 != OAM_TimeToDword(&tAbsClock, &dTime))
        return PARAERR_CONVERT;

    /* save the time */
    pValue = (WORD32 *) Oam_InptGetBuf(sizeof(WORD32),ucLinkID);
    * pValue = dTime;
    pCmdPara->ParaNo    = wParaNo;
    pCmdPara->Type  = DATA_TYPE_TIME;
    pCmdPara->Len       = sizeof(WORD32);
    pCmdPara->pValue    = (BYTE *)pValue;
    return PARA_SUCCESS;
}

/* convert strings to data type map */
int ConvertStrToTypeMap(BYTE ucLinkID,CHAR *pString,TYPE_PARA* pParaInfo,T_OAM_Cfg_Dat * cfgDat,WORD16 wParaNo,CMDPARA *pCmdPara)
{
    int nReturnVal = PARA_SUCCESS;    
    WORD16 * pStore = NULL;
    WORD16 wLen = 0;

    if ((pString == NULL) || (pParaInfo == NULL) || (cfgDat == NULL) || (pCmdPara == NULL))
    {
        return PARAERR_CONVERT;
    }
    pStore = (WORD16 *) Oam_InptGetBuf(sizeof(WORD16),ucLinkID);
    wLen = strlen(pString);
    /* 1.convert */
    nReturnVal = ConvertStringToWord(pParaInfo, pString, pStore, &wLen, cfgDat);
    if (nReturnVal != PARA_SUCCESS)
        return nReturnVal;
    /* 2.verify */
    if (*pStore > pParaInfo->Max.qdwMaxVal || *pStore < pParaInfo->Min.qdwMinVal)
    {
        nReturnVal = PARAERR_OVERFLOW;
        return nReturnVal;
    }
    /* 3.store  */
    pCmdPara->ParaNo = wParaNo;
    pCmdPara->Type   = DATA_TYPE_WORD;
    pCmdPara->Len    = sizeof(WORD16);
    pCmdPara->pValue = (BYTE *)pStore;

    return nReturnVal;

}

/* convert strings to data type const */
int ConvertStrToTypeConst(CHAR *pString, TYPE_PARA* pParaInfo,CMDPARA *pCmdPara)
{
    int nReturnVal = PARA_SUCCESS;    

    /* 此处仅仅使用是否匹配，不再加入匹配参数列表，和语法同级进行再匹配 */
    int nInLen = strlen(pString);
    int nNameLen = strlen(pParaInfo->sName);
    int nLen = 0;
    int wLen = 0;
    
    if ((pString == NULL) || (pParaInfo == NULL) || (pCmdPara == NULL))
    {
        return PARAERR_CONVERT;
    }
    wLen = strlen(pString);
    if (nInLen <= nNameLen)
        nLen = nInLen;
    else
        nLen = nNameLen;
    if (nLen == 0)
    {
        nReturnVal = PARAERR_NULLSTRING;
        return nReturnVal;
    }

    return nReturnVal;
}

#if 0
/* convert strings to data type ifport */
int ConvertStrToTypeIfport(CHAR * pString, TYPE_MATCH_RESULT * pMatchResult)
{
    T_OAM_Cfg_Dat * cfgDat = pMatchResult->cfgDat;
    TYPE_PARA * pParaInfo = NULL;   /* information about the command parameter */
    CMDPARA *   pCmdPara = NULL;    /* store one matched parameter */
    int nReturnVal = PARA_SUCCESS;
    WORD16 wParaNo = 0;

    CHAR *pBoardName = gpCnvtTempStr1;
    CHAR * pIfNum = NULL;
    CHAR * pTempStr = NULL;
    WORD32 dTemp = 0;
    BYTE bIfNum = 0;
    BYTE * pStore = NULL;
    int i = 0;
    int nStrLen = 0;

    *pBoardName = MARK_STRINGEND;
    /* set proper value */
    nReturnVal = PARA_SUCCESS;
    pParaInfo = (TYPE_PARA *) &(cfgDat->tCmdPara[pMatchResult->wCurParaPos].uContent.tPara);
    pCmdPara = (CMDPARA *) &(pMatchResult->tPara[pMatchResult->wParaSum]);
    wParaNo = cfgDat->tCmdPara[pMatchResult->wCurParaPos].ParaNo;

    pTempStr = pString;
    pIfNum = strrchr(pTempStr, MARK_VIRGULE);
    if (pIfNum == NULL)     /* no MARK_VIRGULE */
        return PARAERR_VIRGULE;

    /* get BoardName from pString */ 
    nStrLen = pIfNum - pTempStr;
    for(i = 0; i < nStrLen; i++)
    {
        pBoardName[i] = * pTempStr;
        pTempStr++;
    }
    pBoardName[nStrLen] = MARK_STRINGEND;
    if(strlen(pBoardName) > MAX_BOARDNAME)
        return PARAERR_CONVERT;

    /* get ifNumStr from pString */
    pIfNum++;
    /* 1.convert */
    if (PARA_SUCCESS != Convert_atoi(pIfNum, &dTemp))
        return PARAERR_CONVERT;
    
    /* 端口号最大为1-16 */
    if(dTemp < 1 || dTemp > 16)
        return PARAERR_CONVERT;

    bIfNum = (BYTE) dTemp;

    /* save BoardName */
    nStrLen++;
    pStore = (BYTE *) Oam_InptGetBuf(nStrLen);
    strncpy((CHAR *)pStore, pBoardName);
    pCmdPara->ParaNo    = wParaNo;
    pCmdPara->Type      = DATA_TYPE_STRING;
    pCmdPara->Len       = nStrLen;
    pCmdPara->pValue    = (BYTE *) pStore;

    /* save ifNumStr */
    pMatchResult->wParaSum++;
    pCmdPara = (CMDPARA *) &(pMatchResult->tPara[pMatchResult->wParaSum]);
    pStore = (BYTE *) Oam_InptGetBuf(sizeof(BYTE));
    * pStore = bIfNum;
    pCmdPara->ParaNo    = wParaNo + 1;
    pCmdPara->Type  = DATA_TYPE_BYTE;
    pCmdPara->Len       = sizeof(BYTE);
    pCmdPara->pValue    = (BYTE *) pStore;
    return nReturnVal;
}
#endif

/* convert strings to data type hex */
int ConvertStrToTypeHex(BYTE ucLinkID,CHAR * pString, TYPE_PARA* pParaInfo,WORD16 wParaNo,CMDPARA *pCmdPara)
{
    int nReturnVal = PARA_SUCCESS;
    int nParaLen = 0;
    int nStrLen = 0;
    int nPos = 0;
    int i = 0;
    int j = 0;

    BYTE bValue[13] = {0};
    BYTE * pValue = NULL;
    BYTE bValue1 = 0;
    BYTE bValue2 = 0;

    CHAR *strNoPoint = gpCnvtTempStr1;
    CHAR *pTempStr = gpCnvtTempStr2;
    CHAR *pSubStr = NULL;

    if ((pString == NULL) || (pParaInfo == NULL) || (pCmdPara == NULL))
    {
        return PARAERR_CONVERT;
    }

    /* set initial value */
    *strNoPoint = MARK_STRINGEND;
    *pTempStr = MARK_STRINGEND;
    nParaLen = (int)pParaInfo->wLen;
    pSubStr = pString;
    nStrLen = strlen(pSubStr);

    if(nParaLen == 6)
    {
        if(nStrLen != 14)
            return PARAERR_CONVERT;
    }
    else
        if(nStrLen <= 0 || nStrLen > 32)
            return PARAERR_CONVERT;

    /* get sub string */
    nReturnVal = OamCfgFindCharInString(pSubStr, (CHAR)MARK_POINT, &nPos);
    if (nReturnVal != PARA_SUCCESS)
        if(nParaLen != 13)      /* xx.xxxx.xxxx... 可以只输入xx */
            return nReturnVal;
    
    if( nParaLen == 13)
    {
        if(nPos != 0 && nPos != 2 && nPos != 4)
            return PARAERR_CONVERT;

        if (nPos == 0)
        {
            if (nStrLen != 2 && nStrLen != 4)
                return PARAERR_CONVERT;
        }
    }
    else
    {
        if(nPos != 4)   /* xxxx.xxxx.xxxx 必须完整输入 */
            return PARAERR_CONVERT;
    }

    if(nPos == 0)
        strncpy(pTempStr, pSubStr, sizeof(gpCnvtTempStr2));
    else
    {
        memcpy(pTempStr, pSubStr, (size_t)nPos);
        pTempStr[nPos] = MARK_STRINGEND;
    }

    /* get address from the string */

    for(i = 0; i < (nParaLen + 1)/2; i++)
    {
        XOS_snprintf(strNoPoint, MAX_STRING_LEN - 1, "%s%s", strNoPoint,pTempStr);

        if(Convert_atoh(pTempStr, &bValue1, &bValue2) != PARA_SUCCESS)
            return PARAERR_CONVERT;
        
        if(nPos == 0) /* 最后一组数据 */
        {
            if(i == 0 && nStrLen == 2)  /* xx */
            {
                bValue[j] = bValue1;
            }
            else    /* xxxx */
            {
                bValue[j]   = bValue1;
                bValue[++j] = bValue2;
            }
            break;
        }
        else if(nPos == 2)
        {
            bValue[j] = bValue1;
            j++;
        }
        else
        {
            bValue[j]   = bValue1;
            bValue[++j] = bValue2;
            j++;
        }

        pSubStr += nPos + 1;
            
        nReturnVal = OamCfgFindCharInString(pSubStr, (CHAR)MARK_POINT, &nPos);
        if(nReturnVal == PARAERR_NOFIND) /* xxxx */
        {
            if(strlen(pSubStr) != 4)
                return PARAERR_CONVERT; 
        }
        else if(nPos != 4)
            return PARAERR_CONVERT;
        
        memcpy(pTempStr, pSubStr, (size_t)4);
        pTempStr[4] = MARK_STRINGEND;
    }

    nStrLen = strlen(strNoPoint)/2;

    pValue = (BYTE *) Oam_InptGetBuf(nStrLen,ucLinkID);
    memcpy(pValue, bValue, (size_t)nStrLen);
    pCmdPara->ParaNo    = wParaNo;
    pCmdPara->Type      = DATA_TYPE_HEX;
    pCmdPara->Len       = nStrLen;
    pCmdPara->pValue    = pValue;

    return PARA_SUCCESS;
}

/* Convert the string to mac ipaddrmask */
int ConvertStrToTypeIpaddrMask(BYTE ucLinkID,CHAR * pString, TYPE_PARA* pParaInfo,WORD16 wParaNo,CMDPARA *pCmdPara)
{
    int nReturnVal = PARA_SUCCESS;
    WORD32 dwIPAddr = 0;

    BYTE maskAddr[LEN_IPADDR];
    CHAR * pSubStr = NULL;
    BYTE * pValue = NULL;

    if ((pString == NULL) || (pParaInfo == NULL) || (pCmdPara == NULL))
    {
        return PARAERR_CONVERT;
    }

    /* set initial value */
    memset(maskAddr, 0, (size_t)LEN_IPADDR);

    pSubStr = pString;

    nReturnVal = ConvertStrToAddr(pSubStr, maskAddr);
    if(nReturnVal != PARA_SUCCESS)
        return nReturnVal;

    /* 掩码识别 */
    if(MaskJudgement(maskAddr) != PARA_SUCCESS)
        return PARAERR_CONVERT;

    pValue = (BYTE *) Oam_InptGetBuf(LEN_IPADDR,ucLinkID);
    memcpy(pValue, maskAddr, LEN_IPADDR);
    pCmdPara->ParaNo    = wParaNo;
    pCmdPara->Type      = DATA_TYPE_IPADDR;
    pCmdPara->Len       = LEN_IPADDR;

    memcpy(&dwIPAddr, pValue, LEN_IPADDR);
    dwIPAddr = XOS_InvertWord32(dwIPAddr);
    memcpy(pValue, &dwIPAddr, LEN_IPADDR);
    pCmdPara->pValue    = pValue;

    return nReturnVal;
}

/* Convert the string to mac ipaddrimask */
int ConvertStrToTypeIpaddrIMask(BYTE ucLinkID,CHAR * pString, TYPE_PARA* pParaInfo,WORD16 wParaNo,CMDPARA *pCmdPara)
{
    int nReturnVal = PARA_SUCCESS;
    BYTE rMaskAddr[LEN_IPADDR];
    BYTE maskAddr[LEN_IPADDR];
    BYTE * pValue = NULL;
    CHAR * pSubStr = NULL;
    WORD32 dwIPAddr = 0;
    int i = 0;
    
    if ((pString == NULL) || (pParaInfo == NULL) || (pCmdPara == NULL))
    {
        return PARAERR_CONVERT;
    }

    /* set initial value */
    memset(rMaskAddr, 0, (size_t)LEN_IPADDR);
    pSubStr = pString;
    nReturnVal = ConvertStrToAddr(pSubStr, rMaskAddr);
    if(nReturnVal != PARA_SUCCESS)
        return nReturnVal;

    /* 反掩码识别,先转换成正常掩码 */
    for(i = 0; i < LEN_IPADDR; i++)
        maskAddr[i] = ~ rMaskAddr[i];
    
    if(MaskJudgement(maskAddr) != PARA_SUCCESS)
        return PARAERR_CONVERT;

    pValue = (BYTE *) Oam_InptGetBuf(LEN_IPADDR,ucLinkID);
    memcpy(pValue, rMaskAddr, LEN_IPADDR);
    pCmdPara->ParaNo    = wParaNo;
    pCmdPara->Type      = DATA_TYPE_IPADDR;
    pCmdPara->Len       = LEN_IPADDR;

    memcpy(&dwIPAddr, pValue, LEN_IPADDR);
    dwIPAddr = XOS_InvertWord32(dwIPAddr);
    memcpy(&pValue, &dwIPAddr, LEN_IPADDR);
    pCmdPara->pValue    = pValue;

    return nReturnVal;
}

/* Convert the string to mac address */
int ConvertStrToTypeMacAddr(BYTE ucLinkID,CHAR * pString,TYPE_PARA* pParaInfo,WORD16 wParaNo,CMDPARA *pCmdPara)
{
    int nReturnVal;
    int iAddrLen;
    BYTE macAddr[LEN_MACADDR];
    CHAR *pSubStr = NULL;
    BYTE *pValue = NULL;
    BYTE szHex[2] = {0};
    int iLen;
    
    if ((pString == NULL) || (pParaInfo == NULL) || (pCmdPara == NULL))
    {
        return PARAERR_CONVERT;
    }

    /* set initial value */
    memset(macAddr, 0, (size_t)LEN_MACADDR);

    iAddrLen = 0;
    iLen = 0;
    for (pSubStr = pString; (*pSubStr) != MARK_STRINGEND; pSubStr++)
    {
        if ((*pSubStr) == MARK_HYPHEN)
        {
            if ((nReturnVal = Mac_GetHexValue(szHex, iLen, &macAddr[iAddrLen++]))
                != PARA_SUCCESS)
                return nReturnVal;
            iLen = 0;
        }
        else
        {	
            if (iLen >= 2)
            {
                return PARAERR_MACLEN;
            }
            szHex[iLen++] = ((*pSubStr) >= 'a') && ((*pSubStr) <= 'z') ? ((*pSubStr) - 32) : (*pSubStr);
        }
    }
    
    if ((nReturnVal = Mac_GetHexValue(szHex, iLen, &macAddr[iAddrLen++]))
        != PARA_SUCCESS)
        return nReturnVal;

    if (iAddrLen != LEN_MACADDR)
        return PARAERR_MACLEN;
    
    pValue = (BYTE *) Oam_InptGetBuf(LEN_MACADDR,ucLinkID);
    memcpy(pValue, macAddr, LEN_MACADDR);
    pCmdPara->ParaNo    = wParaNo;
    pCmdPara->Type      = DATA_TYPE_MACADDR;
    pCmdPara->Len       = LEN_MACADDR;
    pCmdPara->pValue    = pValue;
    return PARA_SUCCESS;
}

/* convert strings to data type float */
int ConvertStrToTypeFloat(BYTE ucLinkID,CHAR *pString, TYPE_PARA* pParaInfo,WORD16 wParaNo,CMDPARA *pCmdPara)
{
    int nReturnVal = 0;
    float *pStore = NULL;
    float dTemp = 0;
	
    if ((pString == NULL) || (pParaInfo == NULL) || (pCmdPara == NULL))
    {
        return PARAERR_CONVERT;
    }
    /* 1.convert */
    if (!CheckDataString(pString))
    {
        nReturnVal = PARAERR_CONVERT;
        return nReturnVal;
    }
    /* 2.verify */
    dTemp = (float)atof(pString);
    if (dTemp > pParaInfo->Max.fMaxVal || dTemp < pParaInfo->Min.fMinVal)
    {
        nReturnVal = PARAERR_OVERFLOW;
        return nReturnVal;
    }

    /* 3.store  */
    pStore = (float *) Oam_InptGetBuf(sizeof(float),ucLinkID);
    *pStore = (float)dTemp;
    pCmdPara->ParaNo = wParaNo;
    pCmdPara->Type   = pParaInfo->bParaType;
    pCmdPara->Len    = sizeof(float);
    pCmdPara->pValue = (BYTE *)pStore;

    return nReturnVal;
}

/* convert string to data type ip address */
int ConvertStrToTypeIpv6prefix(BYTE ucLinkID,CHAR * pString,TYPE_PARA* pParaInfo,WORD16 wParaNo,CMDPARA *pCmdPara)
{
    int nReturnVal ;
    BYTE ipAddr[LEN_IPV6ADDR] = {0};
    BYTE PrefixLen = 0;
    CHAR *pSubStr = NULL;
    BYTE * pValue = NULL;
    PREFIX_IPV6 ipv6;

    if ((pString == NULL) || (pParaInfo == NULL) || (pCmdPara == NULL))
    {
        return PARAERR_CONVERT;
    }
    /* set initial value */
    memset(ipAddr, 0, (size_t)LEN_IPV6ADDR);
    pSubStr = pString;

    nReturnVal = ConvertStrToIpv6Prefix(pSubStr, ipAddr, &PrefixLen);
    if(nReturnVal != PARA_SUCCESS)
        return nReturnVal;

    memset(&ipv6, 0, sizeof(ipv6));
    ipv6.family  = BRS_AF_INET6;
    ipv6.prefixlen = PrefixLen;
    memcpy(&ipv6.prefix, ipAddr, 16 * sizeof(BYTE));

    pValue = (BYTE *) Oam_InptGetBuf(sizeof(PREFIX_IPV6),ucLinkID);
    memcpy(pValue, &ipv6, sizeof(PREFIX_IPV6));
    pCmdPara->ParaNo    = wParaNo;
    pCmdPara->Type      = DATA_TYPE_IPV6PREFIX;
    pCmdPara->Len       = sizeof(PREFIX_IPV6);  
    pCmdPara->pValue    = pValue;

    return PARA_SUCCESS;
}

/* convert string to data type ip address */
int ConvertStrToTypeIpv6addr(BYTE ucLinkID,CHAR * pString,TYPE_PARA* pParaInfo,WORD16 wParaNo,CMDPARA *pCmdPara)
{
    int nReturnVal ;
    BYTE ipAddr[LEN_IPV6ADDR] = {0};
    CHAR *pSubStr = NULL;
    BYTE * pValue = NULL;
    IN6_ADDR ipv6;

    if ((pString == NULL) || (pParaInfo == NULL) || (pCmdPara == NULL))
    {
        return PARAERR_CONVERT;
    }

    /* set initial value */
    memset(ipAddr, 0, (size_t)LEN_IPV6ADDR);
    pSubStr = pString;

    nReturnVal = ConvertStrToIpv6Addr(pSubStr, ipAddr);
    if(nReturnVal != PARA_SUCCESS)
        return nReturnVal;

    memset(&ipv6, 0, sizeof(ipv6));
    memcpy(&ipv6, ipAddr, 16 * sizeof(BYTE));

    pValue = (BYTE *) Oam_InptGetBuf(sizeof(IN6_ADDR),ucLinkID);
    memcpy(pValue, &ipv6, sizeof(IN6_ADDR));
    pCmdPara->ParaNo    = wParaNo;
    pCmdPara->Type      = DATA_TYPE_IPV6ADDR;
    pCmdPara->Len       = sizeof(IN6_ADDR); 
    pCmdPara->pValue    = pValue;

    return PARA_SUCCESS;
}

/* convert string to vpn asn */
int ConvertStrToTypeVPNASN(BYTE ucLinkID,CHAR * pString,TYPE_PARA* pParaInfo,WORD16 wParaNo,CMDPARA *pCmdPara)
{
    CHAR *pTempStr = gpCnvtTempStr1;
    CHAR *pSubStr = NULL;
    int nReturnVal = PARA_SUCCESS;
    int nPos = 0;
    WORD64 qdwTemp = 0;
    WORD16  asn       = 0;
    WORD64  nn        = 0;
    VPN_ASN * pValue = NULL;

    if ((pString == NULL) || (pParaInfo == NULL) || (pCmdPara == NULL))
    {
        return PARAERR_CONVERT;
    }

    /* set initial value */
    *pTempStr = MARK_STRINGEND;
    pSubStr = pString;

    /* get and check the hour, minute and second */
    /* 1.asn */
    nReturnVal = OamCfgFindCharInString(pSubStr, (CHAR)MARK_COLON, &nPos);
    if (nReturnVal != PARA_SUCCESS)
        return nReturnVal;
    memcpy(pTempStr, pSubStr, (size_t)nPos);
    pTempStr[nPos] = MARK_STRINGEND;
    if (strlen(pTempStr) == 0 || strlen(pTempStr) > 5)
        return PARAERR_CONVERT;
    if (PARA_SUCCESS != Convert_atoi(pTempStr, &qdwTemp))
        return PARAERR_CONVERT;
    asn = (WORD16)qdwTemp;

    if (asn < 1) 
        return PARAERR_OVERFLOW;


    pSubStr += nPos + 1;
    nPos = 0;
    *pTempStr = MARK_STRINGEND;

    /* 2.nn */
    strncpy(pTempStr, pSubStr, sizeof(gpCnvtTempStr1));
    pTempStr[strlen(pSubStr)] = MARK_STRINGEND;
    if (strlen(pTempStr) == 0 || strlen(pTempStr) > 10)
        return PARAERR_CONVERT;
    if (PARA_SUCCESS != Convert_atoi(pTempStr, &nn))
        return PARAERR_CONVERT;

    /*611001115993 检查范围*/
    if (nn > 0xFFFFFFFF)
        return PARAERR_OVERFLOW;
	
    /* check the rear ??? */
    pSubStr += strlen(pTempStr);
    if (strlen(pSubStr) != 0)
        return PARAERR_CONVERT;

    /* save the time */
    pValue = (VPN_ASN *) Oam_InptGetBuf(sizeof(VPN_ASN),ucLinkID);
    pValue->asn = asn;
    pValue->nn  = (WORD32)nn;
    pCmdPara->ParaNo    = wParaNo;
    pCmdPara->Type      = DATA_TYPE_VPN_ASN;
    pCmdPara->Len       = sizeof(VPN_ASN);
    pCmdPara->pValue    = (BYTE *)pValue;

    return PARA_SUCCESS;
}

int ConvertStrToTypeLogicAddrM(BYTE ucLinkID,CHAR * pString,TYPE_PARA* pParaInfo,WORD16 wParaNo,CMDPARA *pCmdPara)
{
    T_LogicalAddrM *pValue = NULL;
    int nReturnVal = 0;
    WORD64 qdwTemp = 0;
    
    if ((pString == NULL) || (pParaInfo == NULL) || (pCmdPara == NULL))
    {
        return PARAERR_CONVERT;
    }    

    /* 1.convert */
    if (PARA_SUCCESS != Convert_atoi(pString, &qdwTemp))
    {
        nReturnVal = PARAERR_CONVERT;
        return nReturnVal;
    }
    /* 3.store  */
    pValue = (T_LogicalAddrM *) Oam_InptGetBuf(sizeof(T_LogicalAddrM),ucLinkID);
    memset(pValue, 0, sizeof(T_LogicalAddrM));
    pValue->wModule = (WORD16) qdwTemp;
    pCmdPara->ParaNo = wParaNo;
    pCmdPara->Type   = DATA_TYPE_LOGIC_ADDR_M;
    pCmdPara->Len    = sizeof(T_LogicalAddrM);
    pCmdPara->pValue = (BYTE *)pValue;

    return nReturnVal;
}

int ConvertStrToTypeLogicAddrU(BYTE ucLinkID,CHAR * pString,TYPE_PARA* pParaInfo,WORD16 wParaNo,CMDPARA *pCmdPara)
{
    T_LogicalAddrU *pValue = NULL;
    WORD64 qdwTemp = 0;
    CHAR *pSubStr = NULL;
    int nLen = 0;
    int i = 0;
    int j = 0;
    int nHyphenNum = 0; /* '-'的数量 */
    CHAR szSubStr[3][6] = {{0}};
    
    if ((pString == NULL) || (pParaInfo == NULL) || (pCmdPara == NULL))
    {
        return PARAERR_CONVERT;
    }

    pSubStr = pString;
    nLen = strlen(pSubStr);
    if (nLen > 13 || nLen < 5) /* 该类型输入格式: subSystem-unit-sunit (xxx-xxxxx-xxx) */
        return PARAERR_CONVERT;

    while(*pSubStr != MARK_STRINGEND)
    {
        if (*pSubStr == MARK_HYPHEN)
        {
            nHyphenNum++;
            if (nHyphenNum > 2) /* 输入格式中'-'的数目不能大于2个 */
                return PARAERR_CONVERT;
            i++;
            j = 0;
        }
        else
        {
            if (i > 2 || j > 4) /* 输入格式中，'-'隔开的单词数目不能大于3个，每个单词长度不能大于5个字符 */
                return PARAERR_CONVERT;
            
            szSubStr[i][j] = *pSubStr;
            j++;
        }
        pSubStr++;
    }
    
    if (nHyphenNum != 2)
        return PARAERR_CONVERT;

    pValue = (T_LogicalAddrU *) Oam_InptGetBuf(sizeof(T_LogicalAddrU),ucLinkID);
    memset(pValue, 0, sizeof(T_LogicalAddrU));

    for (i = 0; i < 3; i++)
    {
        qdwTemp = 0;
        /* 1.convert */
        if (PARA_SUCCESS != Convert_atoi(szSubStr[i], &qdwTemp))
        {
            return PARAERR_CONVERT;
        }
        if (i == 0)
            pValue->ucSubSystem = (BYTE)qdwTemp;
        else if (i == 1)
            pValue->wUnit = (WORD16)qdwTemp;
        else
            pValue->ucSUnit = (BYTE)qdwTemp;
    }

    pCmdPara->ParaNo = wParaNo;
    pCmdPara->Type   = DATA_TYPE_LOGIC_ADDR_U;
    pCmdPara->Len    = sizeof(T_LogicalAddrU);
    pCmdPara->pValue = (BYTE *)pValue;

    return PARA_SUCCESS;
}

/* 输入格式: IpType-VpnId-IPAddr，如: 1-1-128.0.31.1*/
/* 附:IPv6 地址的表示形式：
　　1. 优先选用的形式是 X:X:X:X:X:X:X:X，其中X是1个16位地址段的十六进制值。例如： 
　　    FEDC:BA98:7654:4210:FEDC:BA98:7654:3210 
　　    2001:0:0:0:0:8:800: 201C : 417A 
　　每一组数值前面的0可以省略。如0008写成8

    2. 在分配某种形式的IPv6地址时，会发生包含长串0位的地址。为了简化包含0位地址的书写，
    可以使用 “::” 符号简化多个0位的16位组。 
    “::” 符号在一个地址中只能出现一次。该符号也可以用来压缩地址中前部和尾部的0。
    举例如下： 
　　    FF01:0:0:0:0:0:0:101 多点传送地址 
　　    0:0:0:0:0:0:0:1 回送地址 
　　    0:0:0:0:0:0:0:0 未指定地址 
　　可用下面的压缩形式表示： 
　　    FF01::101 多点传送地址 
　　    ::1 回送地址 
　　    :: 未指定地址
 */
int ConvertStrToTypeIpvxaddr(BYTE ucLinkID,CHAR * pString,TYPE_PARA* pParaInfo,WORD16 wParaNo,CMDPARA *pCmdPara)
{
    int nReturnVal = 0;
    T_IPVXADDR *pValue = NULL;
    CHAR *pSubStr = NULL;
    WORD64 qdwTemp = 0;
    int nLen = 0;
    int i = 0;
    int j = 0;
    int nHyphenNum = 0; /* '-'的数量 */
    CHAR szSubStr[3][40] = {{0}};
    BYTE ipv4Addr[LEN_IPADDR];
    WORD32 dwIPV4Addr = 0;
    BYTE ipv6Addr[LEN_IPV6ADDR] = {0};
    IN6_ADDR ipv6;

    if ((pString == NULL) || (pParaInfo == NULL) || (pCmdPara == NULL))
    {
        return PARAERR_CONVERT;
    }

    pSubStr = pString;
    nLen = strlen(pSubStr);
    if (nLen > 49 || nLen < 6) /* 最长格式: xxx-xxxxx-xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx
                                  最短格式: x-x-::*/
        return PARAERR_CONVERT;

    while(*pSubStr != MARK_STRINGEND)
    {
        if (*pSubStr == MARK_HYPHEN)
        {
            nHyphenNum++;
            if (nHyphenNum > 2) /* 输入格式中'-'的数目不能大于2个 */
                return PARAERR_CONVERT;
            i++;
            j = 0;
        }
        else
        {
            if (i > 2 || j > 38) /* 输入格式中，'-'隔开的单词数目不能大于3个，每个单词长度不能大于5个字符 */
                return PARAERR_CONVERT;
            
            szSubStr[i][j] = *pSubStr;
            j++;
        }
        pSubStr++;
    }

    if (nHyphenNum != 2)
        return PARAERR_CONVERT;
    
    pValue = (T_IPVXADDR *) Oam_InptGetBuf(sizeof(T_IPVXADDR),ucLinkID);
    memset(pValue, 0, sizeof(T_IPVXADDR));

    /* 获取IpType */
    if (PARA_SUCCESS != Convert_atoi(szSubStr[0], &qdwTemp))
        return PARAERR_CONVERT;

    pValue->ucIpType = (BYTE)qdwTemp;

    if (IPADDR_TYPE_IPV4 != pValue->ucIpType &&
        IPADDR_TYPE_IPV6 != pValue->ucIpType)
        return PARAERR_OVERFLOW;

    /* 获取VpnId */
    if (PARA_SUCCESS != Convert_atoi(szSubStr[1], &qdwTemp))
        return PARAERR_CONVERT;

    pValue->wVpnId = (WORD16)qdwTemp;
/*
    if (pValue->wVpnId == 0) 
        return PARAERR_OVERFLOW;
   */   
    /* 获取IP地址 */
    if (IPADDR_TYPE_IPV4 == pValue->ucIpType)
    {
        memset(ipv4Addr, 0, (size_t)LEN_IPADDR);

        nReturnVal = ConvertStrToAddr(szSubStr[2], ipv4Addr);
        if(nReturnVal != PARA_SUCCESS)
            return nReturnVal;

        memcpy(&pValue->aucAddr, ipv4Addr, LEN_IPADDR);
        memcpy(&dwIPV4Addr, &pValue->aucAddr, LEN_IPADDR);
    /*    dwIPV4Addr = XOS_InvertWord32(dwIPV4Addr);*/
        memcpy(&pValue->aucAddr, &dwIPV4Addr,LEN_IPADDR);
        
    }
    else
    {
        memset(ipv6Addr, 0, (size_t)LEN_IPV6ADDR);
        nReturnVal = ConvertStrToIpv6Addr(szSubStr[2], ipv6Addr);
        if(nReturnVal != PARA_SUCCESS)
            return nReturnVal;

        memset(&ipv6, 0, sizeof(ipv6));
        memcpy(&ipv6, ipv6Addr, 16 * sizeof(BYTE));
        memcpy(&pValue->aucAddr, &ipv6, sizeof(IN6_ADDR));
        
    }

    pCmdPara->ParaNo    = wParaNo;
    pCmdPara->Type      = DATA_TYPE_IPVXADDR;
    pCmdPara->Len       = sizeof(T_IPVXADDR); 
    pCmdPara->pValue    = (BYTE *)pValue;

    return PARA_SUCCESS;
}

/* 输入格式: YYYY-MM-DD-HH-MI-SS-0~999*/
int ConvertStrToTypeSysClock(BYTE ucLinkID,CHAR * pString,TYPE_PARA* pParaInfo,WORD16 wParaNo,CMDPARA *pCmdPara)
{
    T_SysSoftClock *pValue = NULL;
	
    CHAR *pSubStr = NULL;
    WORD64 qdwTemp = 0;
    int nLen = 0;
    int i = 0; 
    int j = 0;
    int nHyphenNum = 0; /* '-'的数量 */
    CHAR szSubStr[7][5] = {{0}};
   
    if ((pString == NULL) || (pParaInfo == NULL) || (pCmdPara == NULL))
    {
        return PARAERR_CONVERT;
    }

    pSubStr = pString;
    nLen = strlen(pSubStr);
    if (nLen > 23 || nLen < 13)
        return PARAERR_CONVERT;

    while(*pSubStr != MARK_STRINGEND)
    {
        if (*pSubStr == MARK_HYPHEN)
        {
            nHyphenNum++;
            if (nHyphenNum > 6) /* 输入格式中'-'的数目不能大于6个 */
                return PARAERR_CONVERT;
            i++;
            j = 0;
        }
        else
        {
            if (i > 6 || j > 3) /* 输入格式中，'-'隔开的单词数目不能大于3个，每个单词长度不能大于5个字符 */
                return PARAERR_CONVERT;
            
            szSubStr[i][j] = *pSubStr;
            j++;
        }
        pSubStr++;
    }

    if (nHyphenNum != 6)
        return PARAERR_CONVERT;
    
    pValue = (T_SysSoftClock *) Oam_InptGetBuf(sizeof(T_SysSoftClock),ucLinkID);
    memset(pValue, 0, sizeof(T_SysSoftClock));

    /* 获取year */
    if (PARA_SUCCESS != Convert_atoi(szSubStr[0], &qdwTemp))
        return PARAERR_CONVERT;

    pValue->wSysYear = (WORD16)qdwTemp;

    /* 获取month */
    if (PARA_SUCCESS != Convert_atoi(szSubStr[1], &qdwTemp))
        return PARAERR_CONVERT;

    pValue->ucSysMon = (BYTE)qdwTemp;

    if ((pValue->ucSysMon < 1) || (pValue->ucSysMon >12)) 
        return PARAERR_OVERFLOW;
      
    /* 获取day*/
    if (PARA_SUCCESS != Convert_atoi(szSubStr[2], &qdwTemp))
        return PARAERR_CONVERT;

    pValue->ucSysDay = (BYTE)qdwTemp;

    if ((pValue->ucSysDay < 1) || (pValue->ucSysDay >31)) 
        return PARAERR_OVERFLOW;

    /* 获取hour*/
    if (PARA_SUCCESS != Convert_atoi(szSubStr[3], &qdwTemp))
        return PARAERR_CONVERT;

    pValue->ucSysHour = (BYTE)qdwTemp;

    if (pValue->ucSysHour >23) 
        return PARAERR_OVERFLOW;

    /* 获取min*/
    if (PARA_SUCCESS != Convert_atoi(szSubStr[4], &qdwTemp))
        return PARAERR_CONVERT;

    pValue->ucSysMin = (BYTE)qdwTemp;

    if (pValue->ucSysMin >59) 
        return PARAERR_OVERFLOW;
	
    /* 获取sec*/
    if (PARA_SUCCESS != Convert_atoi(szSubStr[5], &qdwTemp))
        return PARAERR_CONVERT;

    pValue->ucSysSec = (BYTE)qdwTemp;

    if (pValue->ucSysSec >59) 
        return PARAERR_OVERFLOW;
	
    /* 获取milisec*/
    if (PARA_SUCCESS != Convert_atoi(szSubStr[6], &qdwTemp))
        return PARAERR_CONVERT;

    pValue->wMilliSec = (WORD16)qdwTemp;

    if (pValue->wMilliSec >999) 
        return PARAERR_OVERFLOW;
	
    pCmdPara->ParaNo    = wParaNo;
    pCmdPara->Type      = DATA_TYPE_SYSCLOCK;
    pCmdPara->Len       = sizeof(T_SysSoftClock); 
    pCmdPara->pValue    = (BYTE *)pValue;

    return PARA_SUCCESS;
}

/* convert string to vpn ip */
int ConvertStrToTypeVPNIP(BYTE ucLinkID,CHAR * pString,TYPE_PARA* pParaInfo,WORD16 wParaNo,CMDPARA *pCmdPara)
{
    CHAR *pTempStr = gpCnvtTempStr1;
    CHAR *pSubStr = NULL;
    int nReturnVal = PARA_SUCCESS;
    int nPos = 0;
    WORD64 qdwTemp = 0;
    BYTE ipAddr[LEN_IPADDR];
    WORD16  nn        = 0;
    VPN_IP * pValue = NULL;

    if ((pString == NULL) || (pParaInfo == NULL) || (pCmdPara == NULL))
    {
        return PARAERR_CONVERT;
    }

    /* set initial value */
    *pTempStr = MARK_STRINGEND;
    pSubStr = pString;
    memset(ipAddr, 0, (size_t)LEN_IPADDR);

    /* get and check the hour, minute and second */
    /* 1.ip */
    nReturnVal = OamCfgFindCharInString(pSubStr, (CHAR)MARK_COLON, &nPos);
    if (nReturnVal != PARA_SUCCESS)
        return nReturnVal;

    memcpy(pTempStr, pSubStr, (size_t)nPos);
    pTempStr[nPos] = MARK_STRINGEND;
    if (strlen(pTempStr) == 0 || strlen(pTempStr) > 15)
        return PARAERR_CONVERT;

    nReturnVal = ConvertStrToAddr(pTempStr, ipAddr);
    if(nReturnVal != PARA_SUCCESS)
        return nReturnVal;

    pSubStr += nPos + 1;
    nPos = 0;
    *pTempStr = MARK_STRINGEND;

    /* 2.nn */
    strncpy(pTempStr, pSubStr, sizeof(gpCnvtTempStr1));
    pTempStr[strlen(pSubStr)] = MARK_STRINGEND;
    if (strlen(pTempStr) == 0 || strlen(pTempStr) > 5)
        return PARAERR_CONVERT;
    if (PARA_SUCCESS != Convert_atoi(pTempStr, &qdwTemp))
        return PARAERR_CONVERT;
    nn = (WORD16)qdwTemp;
    if (qdwTemp < 0 || qdwTemp > 65535) /* check */
        return PARAERR_OVERFLOW;
 
    /* check the rear ??? */
    pSubStr += strlen(pTempStr);
    if (strlen(pSubStr) != 0)
        return PARAERR_CONVERT;

    /* save the time */
    pValue = (VPN_IP *) Oam_InptGetBuf(sizeof(VPN_IP),ucLinkID);
    memcpy(&(pValue->ipaddr), ipAddr, LEN_IPADDR);
    pValue->nn  = nn;
    pCmdPara->ParaNo    = wParaNo;
    pCmdPara->Type      = DATA_TYPE_VPN_IP;
    pCmdPara->Len       = sizeof(VPN_IP);
    pCmdPara->pValue    = (BYTE *)pValue;

    return PARA_SUCCESS;
}

int ConvertStrToAddr(CHAR * pString, BYTE Address[])
{
    int nReturnVal = PARA_SUCCESS;

    CHAR *pTempStr = gpCnvtTempStr3;
    CHAR *pSubStr = NULL;
    int nPos = 0;
    WORD64 qdwTemp = 0;
    int i = 0;

    /* set initial value */
    *pTempStr = MARK_STRINGEND;
    memset(Address, 0, (size_t)LEN_IPADDR); 
    pSubStr = pString;

    /* get sub string */
    nReturnVal = OamCfgFindCharInString(pSubStr, (CHAR)MARK_POINT, &nPos);
    if (nReturnVal != PARA_SUCCESS)
    {
        nReturnVal = Convert_atoi(pSubStr, &qdwTemp);
        if(nReturnVal == PARA_SUCCESS )
            return PARAERR_NOFIND;
        else
            return nReturnVal;
    }    
    
    memcpy(pTempStr, pSubStr, (size_t)nPos);
    pTempStr[nPos] = MARK_STRINGEND;
    /* get ipaddr from the string */
    for (i = 0 ; i < LEN_IPADDR; i++)
    {
        /* convert the sub string to a byte */
        if (PARA_SUCCESS != Convert_atoi(pTempStr, &qdwTemp))
        {
            nReturnVal = PARAERR_CONVERT;
            break;
        }
        if (qdwTemp < 0 || qdwTemp > 255)
        {
            nReturnVal = PARAERR_OVERFLOW;
            break;
        }
        Address[i] = (BYTE) qdwTemp;
        /* change the pSubStr */
        if (i == LEN_IPADDR - 1)
            break;
        pSubStr += nPos + 1;
        
        if(strlen(pSubStr) == 0)  /* IP地址(或掩码)不完整 */
            return PARAERR_NOFIND;

        nPos = 0;
        /* get new sub string */
        *pTempStr = MARK_STRINGEND;
        if (i != LEN_IPADDR - 2)
        {
            nReturnVal = OamCfgFindCharInString(pSubStr, (CHAR)MARK_POINT, &nPos);
            if (nReturnVal != PARA_SUCCESS)
                break;
            memcpy(pTempStr, pSubStr, (size_t)nPos);
            pTempStr[nPos] = MARK_STRINGEND;
        }
        else
            strncpy(pTempStr, pSubStr, sizeof(gpCnvtTempStr3));
    }
    if (nReturnVal != PARA_SUCCESS)
        return nReturnVal;
    pSubStr += strlen(pTempStr);
    if (strlen(pSubStr) != 0)
        return PARAERR_CONVERT;

    return nReturnVal;
}


int ConvertStrToIpv6Prefix(CHAR * pString, BYTE Address[], BYTE *pPrefixLen)
{
    CHAR strAddr[MAX_STRING_LEN] = {0};
    CHAR *strPrefix = NULL;
    WORD64 qdwPreficLen = 0;
	
    /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
    XOS_snprintf(strAddr,
                 sizeof(strAddr) -1,
                 "%s",
                 pString);
    strPrefix = strstr(strAddr, "/");
    
    if(strPrefix == NULL)
        return PARAERR_CONVERT;
    else
    {
        CHAR *prefix = &strPrefix[1];
        if(*prefix == '\0')
            return PARAERR_CONVERT;
        if(strlen(prefix) > 3)
            return PARAERR_CONVERT;
        if (Convert_atoi(prefix, &qdwPreficLen) != PARA_SUCCESS)
        {        
            return PARAERR_CONVERT;
        }

        *pPrefixLen = (BYTE)qdwPreficLen;
        if(*pPrefixLen > 128)
            return PARAERR_CONVERT;

        memset(strPrefix, 0, strlen(strPrefix));
    }
    return ConvertStrToAddrArray(strAddr, Address);
}

int ConvertStrToIpv6Addr(CHAR * pString, BYTE Address[])
{
    return ConvertStrToAddrArray(pString, Address);
}

int ConvertStrToAddrArray(CHAR * pString, BYTE Address[])
{
    CHAR *pSubStr = pString;
    /* get sub string */
    CHAR strSection[8][5];/*8个被冒号分开的字段字符串*/
    CHAR strColon[8][40];/*冒号字符串*/
    int setctionNum = 0, i = 0, j = 0, k = 0, l = 0;/*实际的字段总个数*/
    int dbColonPos = -1;/*双冒号的位置*/
    int dbColonNum = 1;/*双冒号代表的0字段的个数*/
    int cntColon = 0, cntDbColon = 0;/*单双冒号计数*/
    CHAR *pStr = NULL;

    /*判断合法性*/
    int nLen = strlen(pSubStr);
    if(nLen < 2 || nLen > 39)/*Ipv6地址长度不正确*/
        return PARAERR_CONVERT;
    if((*pSubStr == ':' && *(pSubStr + 1)!= ':')/*首尾为单冒号*/
        ||(*(pSubStr + nLen - 1) == ':' && *(pSubStr + nLen - 2)!= ':'))
        return PARAERR_CONVERT;

    memset(strSection,0,sizeof(strSection));
    memset(strColon,0,sizeof(strColon));

    pStr = pSubStr;
    while(*pStr != '\0')
    {
        CHAR ch = *pStr;
        if((ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F') 
            || (ch >= '0' && ch <= '9') || ch == ':' )
        {
            if(ch == ':')
            {
                strColon[j][l++] = ch;
                if(*(pStr + 1) == ':')
                {
                    if(*pSubStr == ':' && *(pSubStr + 1)== ':')/*::位于首部*/
                        dbColonPos = -1;
                    else
                        dbColonPos = j;
                    pStr++;
                    continue;
                }

                strSection[j][k] = '\0';
                if(strlen(strSection[j]) != 0)
                {
                    setctionNum++;
                    j++;
                    k = 0;

                    if(j >=8)
                        return PARAERR_CONVERT;
                }
            }
            else
            {
                l = 0;

                strSection[j][k] = ch;
                k++;
                if(*(pStr + 1) == '\0')
                {
                    setctionNum++;  
                }
            }
        }
        else
            return PARAERR_CONVERT;

        pStr++;
    }

    dbColonNum = 8 - setctionNum;
    /*检查冒号和字段*/
    for(i = 0; i < 8; i++)
    {
        if(strlen(strColon[i]) > 2)
            return PARAERR_CONVERT;
        else if(strlen(strColon[i]) == 2)
        {
            cntDbColon++;
        }
        else if(strlen(strColon[i]) == 1)
            cntColon++;

        if(strlen(strSection[i]) > 4)
            return PARAERR_CONVERT;
    }
    if(cntDbColon > 1)
        return PARAERR_CONVERT;
    else if(cntDbColon == 1 && cntColon >= 7)
        return PARAERR_CONVERT;
    else if(cntDbColon == 0 && cntColon !=7)
        return PARAERR_CONVERT;

    /*开始将"::"翻译为0字段*/
    if(cntDbColon)
    {
        k = 0;
        l = dbColonPos+1;
        for(k = 0, j = 7 ; k < setctionNum - l; k ++, j--)
        {
            memset(strSection[j], 0, 5);
            strncpy(strSection[j], strSection[j - dbColonNum], sizeof(strSection[j]));
            strncpy(strSection[j - dbColonNum], "0", sizeof(strSection[j - dbColonNum]));
        }
        for(k = 0; k < dbColonNum; k++,l++)
        {
            memset(strSection[l], 0, 5);
            strncpy(strSection[l], "0", sizeof(strSection[l]));
        }
    }
    /*将字符串转换为数字*/

    /*每个字段补足4位*/
    for(i = 0; i < 8; i++)
    {
        int nLen = strlen(strSection[i]);
        if(nLen != 4)
        {
            int zero = 4 - nLen;
            int k = 4-1;
            for(j = 0; j < nLen ; j++, k--)
            {
                strSection[i][k] = strSection[i][k - zero];
            }
            for(l = 0; l < zero; l++)
                strSection[i][l] = '0';
        }
    }

    /*按位计算数值*/
    {
        BYTE ucData[16] = {0};
        unsigned short usData[8] = {0};
        k = 0; l = 0;
        for(i = 0; i < 8; i++)
        {
            BYTE onebit[4] = {0};
            for(j = 0; j < 4; j++)
            {
                CHAR ch = strSection[i][j];
                if( ch >= '0' && ch <= '9')
                    onebit[j] = ch - '0';
                else if( ch >= 'a' && ch <= 'f')
                    onebit[j] = 10 + (ch - 'a');
                else if( ch >= 'A' && ch <= 'F') 
                    onebit[j] = 10 + (ch - 'A');
            }
            ucData[k] = onebit[0] * 16 + onebit[1];
            ucData[k+1] = onebit[2] * 16 + onebit[3];
            Address[k] = ucData[k];
            Address[k+1] = ucData[k+1];
    
            usData[l++] = ucData[k] * 256 + ucData[k + 1];
    
            k += 2;
        }
    }

    /*for Debug */
    return PARA_SUCCESS;
}

/*convert string to T_LogicalAddr*/
int ConvertStrToTypeLogicAddress(BYTE ucLinkID,CHAR * pString,TYPE_PARA* pParaInfo,WORD16 wParaNo,CMDPARA *pCmdPara)
{
    CHAR *pTempStr = gpCnvtTempStr1;
    CHAR *pSubStr = NULL;
    int nReturnVal = PARA_SUCCESS;
    int nPos = 0;
    WORD64 qdwTemp = 0;
    T_LogicalAddr* pValue = NULL;
    WORD16 wSystem;
    WORD16 wModule;
    WORD16 wUnit;
    WORD16 wIndex;
    BYTE ucSubSystem;
    BYTE ucSunit;

    if ((pString == NULL) || (pParaInfo == NULL) || (pCmdPara == NULL))
    {
        return PARAERR_CONVERT;
    }

    /* set initial value */
    *pTempStr = MARK_STRINGEND;
    pSubStr = pString;

    /*输入格式:system-subsystem-module-unit-sunit-index*/
    
    /*wSystem*/
    nReturnVal = OamCfgFindCharInString(pSubStr, (CHAR)MARK_HYPHEN, &nPos);
    if (nReturnVal != PARA_SUCCESS)
        return nReturnVal;
    memcpy(pTempStr, pSubStr, (size_t)nPos);
    pTempStr[nPos] = MARK_STRINGEND;
    if (strlen(pTempStr) == 0 || strlen(pTempStr) > 5)
        return PARAERR_CONVERT;
    if (PARA_SUCCESS != Convert_atoi(pTempStr, &qdwTemp))
        return PARAERR_CONVERT;
    if (qdwTemp > 65535)
        return PARAERR_CONVERT;

    wSystem = (WORD16)qdwTemp;

    pSubStr += nPos + 1;
    nPos = 0;
    *pTempStr = MARK_STRINGEND;

    /*ucSubSystem*/
    nReturnVal = OamCfgFindCharInString(pSubStr, (CHAR)MARK_HYPHEN, &nPos);
    if (nReturnVal != PARA_SUCCESS)
        return nReturnVal;
    memcpy(pTempStr, pSubStr, (size_t)nPos);
    pTempStr[nPos] = MARK_STRINGEND;
    if (strlen(pTempStr) == 0 || strlen(pTempStr) > 3)
        return PARAERR_CONVERT;
    if (PARA_SUCCESS != Convert_atoi(pTempStr, &qdwTemp))
        return PARAERR_CONVERT;
    if (qdwTemp > 255)
        return PARAERR_CONVERT;

    ucSubSystem= (BYTE)qdwTemp;

    pSubStr += nPos + 1;
    nPos = 0;
    *pTempStr = MARK_STRINGEND;
    
    /*wModule*/
    nReturnVal = OamCfgFindCharInString(pSubStr, (CHAR)MARK_HYPHEN, &nPos);
    if (nReturnVal != PARA_SUCCESS)
        return nReturnVal;
    memcpy(pTempStr, pSubStr, (size_t)nPos);
    pTempStr[nPos] = MARK_STRINGEND;
    if (strlen(pTempStr) == 0 || strlen(pTempStr) > 5)
        return PARAERR_CONVERT;
    if (PARA_SUCCESS != Convert_atoi(pTempStr, &qdwTemp))
        return PARAERR_CONVERT;
    if (qdwTemp > 65535)
        return PARAERR_CONVERT;

    wModule = (WORD16)qdwTemp;

    pSubStr += nPos + 1;
    nPos = 0;
    *pTempStr = MARK_STRINGEND;

    /*wUnit*/
    nReturnVal = OamCfgFindCharInString(pSubStr, (CHAR)MARK_HYPHEN, &nPos);
    if (nReturnVal != PARA_SUCCESS)
        return nReturnVal;
    memcpy(pTempStr, pSubStr, (size_t)nPos);
    pTempStr[nPos] = MARK_STRINGEND;
    if (strlen(pTempStr) == 0 || strlen(pTempStr) > 5)
        return PARAERR_CONVERT;
    if (PARA_SUCCESS != Convert_atoi(pTempStr, &qdwTemp))
        return PARAERR_CONVERT;
    if (qdwTemp > 65535)
        return PARAERR_CONVERT;

    wUnit= (WORD16)qdwTemp;

    pSubStr += nPos + 1;
    nPos = 0;
    *pTempStr = MARK_STRINGEND;

    /*ucSunit*/
  /*  nReturnVal = OamCfgFindCharInString(pSubStr, (CHAR)MARK_HYPHEN, &nPos);
    if (nReturnVal != PARA_SUCCESS)
        return nReturnVal;*/
    nPos = strlen(pSubStr);
    memcpy(pTempStr, pSubStr, (size_t)nPos);
    pTempStr[nPos] = MARK_STRINGEND;
    if (strlen(pTempStr) == 0 || strlen(pTempStr) > 3)
        return PARAERR_CONVERT;
    if (PARA_SUCCESS != Convert_atoi(pTempStr, &qdwTemp))
        return PARAERR_CONVERT;
    if (qdwTemp > 255)
        return PARAERR_CONVERT;

    ucSunit = (BYTE)qdwTemp;

    pSubStr += nPos + 1;
    nPos = 0;
    *pTempStr = MARK_STRINGEND;

/*V02.01.10_ZXOAM_PLAT_002 命令行易用性改进 :逻辑地址不需要index*/
    /*wIndex*/
/*
    nPos = strlen(pSubStr);
    memcpy(pTempStr, pSubStr, (size_t)nPos);
    pTempStr[nPos] = MARK_STRINGEND;
    if (strlen(pTempStr) == 0 || strlen(pTempStr) > 5)
        return PARAERR_CONVERT;
    if (PARA_SUCCESS != Convert_atoi(pTempStr, &qdwTemp))
        return PARAERR_CONVERT;
    if (qdwTemp > 65535)
        return PARAERR_CONVERT;

    wIndex = (WORD16)qdwTemp;

    pSubStr += nPos + 1;
    nPos = 0;
    *pTempStr = MARK_STRINGEND;
    */
    wIndex = 0xFFFF;/*index默认为全f*/

    /* check the rear ??? */
    pSubStr += strlen(pTempStr);
    if (strlen(pSubStr) != 0)
        return PARAERR_CONVERT;

    /* save the T_PhysAddress */
    pValue = (T_LogicalAddr *) Oam_InptGetBuf(sizeof(T_LogicalAddr),ucLinkID);
    memset(pValue, 0, sizeof(T_LogicalAddr));
    pValue->wSystem = wSystem;
    pValue->ucSubSystem = ucSubSystem;
    pValue->wModule = wModule;
    pValue->wUnit = wUnit;
    pValue->ucSUnit = ucSunit;
    pValue->wIndex = wIndex;

    pCmdPara->ParaNo    = wParaNo;
    pCmdPara->Type      = DATA_TYPE_LOGICADDRESS;
    pCmdPara->Len       = sizeof(T_LogicalAddr);
    pCmdPara->pValue    = (BYTE *)pValue;

    return PARA_SUCCESS;
}

/* convert string to T_PhysAddress*/
int ConvertStrToTypePhyAddress(BYTE ucLinkID,CHAR * pString,TYPE_PARA* pParaInfo,WORD16 wParaNo,CMDPARA *pCmdPara)
{
    CHAR *pTempStr = gpCnvtTempStr1;
    CHAR *pSubStr = NULL;
    int nReturnVal = PARA_SUCCESS;
    int nPos = 0;
    WORD64 qdwTemp = 0;
    T_PhysAddress* pValue = NULL;
    BYTE ucRack, ucShelf, ucSlot, ucCpu;

    if ((pString == NULL) || (pParaInfo == NULL) || (pCmdPara == NULL))
    {
        return PARAERR_CONVERT;
    }

    /*rack-shelf-slot-cpu*/

    /* set initial value */
    *pTempStr = MARK_STRINGEND;
    pSubStr = pString;

    /*rack*/
    nReturnVal = OamCfgFindCharInString(pSubStr, (CHAR)MARK_HYPHEN, &nPos);
    if (nReturnVal != PARA_SUCCESS)
        return nReturnVal;
    memcpy(pTempStr, pSubStr, (size_t)nPos);
    pTempStr[nPos] = MARK_STRINGEND;
    if (strlen(pTempStr) == 0 || strlen(pTempStr) > 3)
        return PARAERR_CONVERT;
    if (PARA_SUCCESS != Convert_atoi(pTempStr, &qdwTemp))
        return PARAERR_CONVERT;
    if (qdwTemp > 255)
        return PARAERR_CONVERT;

    ucRack = (BYTE)qdwTemp;

    pSubStr += nPos + 1;
    nPos = 0;
    *pTempStr = MARK_STRINGEND;

    /*shelf*/
    nReturnVal = OamCfgFindCharInString(pSubStr, (CHAR)MARK_HYPHEN, &nPos);
    if (nReturnVal != PARA_SUCCESS)
        return nReturnVal;
    memcpy(pTempStr, pSubStr, (size_t)nPos);
    pTempStr[nPos] = MARK_STRINGEND;
    if (strlen(pTempStr) == 0 || strlen(pTempStr) > 3)
        return PARAERR_CONVERT;
    if (PARA_SUCCESS != Convert_atoi(pTempStr, &qdwTemp))
        return PARAERR_CONVERT;
    if (qdwTemp > 255)
        return PARAERR_CONVERT;

    ucShelf= (BYTE)qdwTemp;

    pSubStr += nPos + 1;
    nPos = 0;
    *pTempStr = MARK_STRINGEND;

    /*slot*/
    nReturnVal = OamCfgFindCharInString(pSubStr, (CHAR)MARK_HYPHEN, &nPos);
    if (nReturnVal != PARA_SUCCESS)
        return nReturnVal;
    memcpy(pTempStr, pSubStr, (size_t)nPos);
    pTempStr[nPos] = MARK_STRINGEND;
    if (strlen(pTempStr) == 0 || strlen(pTempStr) > 3)
        return PARAERR_CONVERT;
    if (PARA_SUCCESS != Convert_atoi(pTempStr, &qdwTemp))
        return PARAERR_CONVERT;
    if (qdwTemp > 255)
        return PARAERR_CONVERT;

    ucSlot= (BYTE)qdwTemp;

    pSubStr += nPos + 1;
    nPos = 0;
    *pTempStr = MARK_STRINGEND;

    /*cpu*/
    nPos = strlen(pSubStr);
    memcpy(pTempStr, pSubStr, (size_t)nPos);
    pTempStr[nPos] = MARK_STRINGEND;
    if (strlen(pTempStr) == 0 || strlen(pTempStr) > 3)
        return PARAERR_CONVERT;
    if (PARA_SUCCESS != Convert_atoi(pTempStr, &qdwTemp))
        return PARAERR_CONVERT;
    if (qdwTemp > 255)
        return PARAERR_CONVERT;

    ucCpu = (BYTE)qdwTemp;

    pSubStr += nPos + 1;
    nPos = 0;
    *pTempStr = MARK_STRINGEND;

    /* check the rear ??? */
    pSubStr += strlen(pTempStr);
    if (strlen(pSubStr) != 0)
        return PARAERR_CONVERT;

    /* save the T_PhysAddress */
    pValue = (T_PhysAddress *) Oam_InptGetBuf(sizeof(T_PhysAddress),ucLinkID);
    pValue->ucRackId = ucRack;
    pValue->ucShelfId = ucShelf;
    pValue->ucSlotId = ucSlot;
    pValue->ucCpuId = ucCpu;

    pCmdPara->ParaNo    = wParaNo;
    pCmdPara->Type      = DATA_TYPE_PHYADDRESS;
    pCmdPara->Len       = sizeof(T_PhysAddress);
    pCmdPara->pValue    = (BYTE *)pValue;

    return PARA_SUCCESS;
}

int MaskJudgement(BYTE mask[])
{
    CHAR strMask[33] = {0};
    CHAR tempStr[9] = {0};
    CHAR * pstrRear = NULL;
    int i = 0;
    int j = 0;
    int bit = 0;
    int mark = 0x80;    /* 屏蔽字 */

    for(i = 0; i < LEN_IPADDR; i++)
    {
        /* 转换为二进制数 */
        for(j = 0; j < 8; j++)
        {
            bit = (mark & mask[i]) ? 1 : 0;
            XOS_snprintf(tempStr + strlen(tempStr), sizeof(tempStr) - strlen(tempStr),"%d", bit);
            mark >>= 1;
        }
        strncat(strMask, tempStr, sizeof(strMask));
        /* reset */
        tempStr[0] = MARK_STRINGEND; 
        mark = 0x80;
    }
    /* judge */
    pstrRear = strchr(strMask, '0');
    if(pstrRear != NULL && strchr(pstrRear, '1') != NULL)
        return PARAERR_CONVERT;

    return PARA_SUCCESS;
}

int Mac_GetHexValue(BYTE *szHex, int iLen, BYTE *bHex)
{
    int iLoop;
    
    if ((iLen == 0) || (iLen > 2))
        return PARAERR_MACLEN;
    else if (iLen == 1)
    {
        szHex[1] = szHex[0];
        szHex[0] = '0';
        iLen = 2;
    }
    
    for (iLoop = 0; iLoop < iLen; iLoop++)
    {
        if ((szHex[iLoop] >= MARK_ZERO) && (szHex[iLoop] <= MARK_NINE))
            szHex[iLoop] -= MARK_ZERO;
        else if ((szHex[iLoop] >= 'A') && (szHex[iLoop] <= 'F'))
            szHex[iLoop] = szHex[iLoop] - 'A' + 10;
        else
            return PARAERR_MACADDR;
    }
    *bHex = szHex[1] + szHex[0] * 16;
    
    return PARA_SUCCESS;
}

/* convert string to int, just act as function atoi */
int Convert_atoi(CHAR * pString, WORD64 * pQdword)
{
    CHAR *pChar = pString;
    WORD64 qdwVal = 0;
    int nDigit = 0;
    int nLength = 0;

    *pQdword = 0;
    nLength = strlen(pString);
    if (nLength == 0)
        return PARAERR_NULLSTRING;

    nLength = 0;

    while (pChar != NULL && *pChar != MARK_STRINGEND)
    {
        /* not a digit */
        if (*pChar < MARK_ZERO || *pChar > MARK_NINE)
            return PARAERR_CONVERT;

        nDigit = *pChar - MARK_ZERO;

        /* overflow */
        if (qdwVal > (MAX_LONG_LONG_VALUE - nDigit) / 10)
            return PARAERR_CONVERT;
        qdwVal = qdwVal * 10 + nDigit;
        if (qdwVal > 0)
            nLength++;

        /* too many digits */
        if (nLength > MAX_LONG_LONG_LENGTH)
            return PARAERR_CONVERT;
        pChar++;
    }

    *pQdword = qdwVal;
    return PARA_SUCCESS;
}

int Convert_atoh(CHAR * pString, BYTE * pValue1, BYTE * pValue2)
{
    CHAR * pChar = pString;
    CHAR tempChar;
    int nLen = 0;
    int nValue = 0;
    int nTempValue = 0;

    if(strlen(pString) == 2)
        *pValue2 = 0;

    while (pChar != NULL && *pChar != MARK_STRINGEND)
    {
        nLen++; /* 字符长度计数 */
        tempChar = *pChar;
        
        /* judge if pChar is a hex digit */
        if ( *pChar < MARK_ZERO || *pChar > MARK_NINE )
            if( *pChar < 'A' || *pChar > 'F' )
                if( *pChar < 'a' || *pChar > 'f' )
                    return PARAERR_CONVERT;

        if(*pChar == 'A' || *pChar == 'a')
            nTempValue = 10;
        else if(*pChar == 'B' || *pChar == 'b')
            nTempValue = 11;
        else if(*pChar == 'C' || *pChar == 'c')
            nTempValue = 12;
        else if(*pChar == 'D' || *pChar == 'd')
            nTempValue = 13;
        else if(*pChar == 'E' || *pChar == 'e')
            nTempValue = 14;
        else if(*pChar == 'F' || *pChar == 'f')
            nTempValue = 15;
        else
            nTempValue = tempChar - '0';/*atoi(&tempChar);*/

        nValue = nValue * 16 + nTempValue;
        if(nLen == 2)
        {
            *pValue1 = nValue;
            nValue = 0;
        }
        else if(nLen == 4)
            *pValue2 = nValue;

        pChar++;
    }
    return PARA_SUCCESS;
}

/*检查字符串是否为负数或小数*/
BOOL CheckDataString(const CHAR *strData)
{
    int i = 0;
    if (strData[0] == '\0')
        return FALSE;
    while(*strData != '\0')
    {

    CHAR ch = *strData;
    if ((ch < '0' || ch > '9' ) && ch != '.' && ch != '-')
        return FALSE;

    if (ch == '-' && i != 0)
        return FALSE;

    if (ch == '.' && *(strData + 1) == '\0')
        return FALSE;

        strData++;
        i++;
    }
    return TRUE;
}


WORD32 FindMapTypePos(T_OAM_Cfg_Dat * cfgDat, WORD16 wType)
{
    DWORD iIdx;

    if (NULL == cfgDat)
    {
        return 0;
    }

/*611000274914 改用遍历方法查找maptype位置*/
#if 0
    WORD32 dwBegin = 1;
    WORD32 dwMid = cfgDat->nMapTypeSum - 1;
    WORD32 dwEnd = cfgDat->nMapTypeSum;
    WORD32 dwLastMid = 0;
    
    if (wType < cfgDat->tMapType[dwBegin].wType || wType > cfgDat->tMapType[dwMid].wType)
        return 0;

    /* 二分查找 */
    while(dwBegin < dwEnd)
    {
        dwMid = (dwBegin + dwEnd) / 2;
        if (dwMid == dwLastMid)
            break;
        if (cfgDat->tMapType[dwMid].wType == wType)
        {
            dwSavedPos = dwMid;
            return dwMid;
        }
        else if (cfgDat->tMapType[dwMid].wType > wType)
            dwEnd = dwMid;
        else
            dwBegin = dwMid;

        dwLastMid = dwMid;
    }
#endif

    /*查找maptype元素，注意maptype数组有效下标是从 1开始*/
    for (iIdx = 1; iIdx < cfgDat->nMapTypeSum; iIdx++)
    {
        if(gDatmatch)
        {
            printf("cfgDat->tMapType[iIdx].wType =%d,wType=%d ,iIdx=%d\n",cfgDat->tMapType[iIdx].wType,wType,iIdx);
        }
        if (cfgDat->tMapType[iIdx].wType == wType)
        {
            return iIdx;
        }
    }
    
    return 0;
}

WORD32 FindMapItemPos(T_OAM_Cfg_Dat * cfgDat, WORD32 dwTypePos, WORD16 wValue)
{
    WORD32 dwBegin = 0;
    WORD32 dwEnd = 0;
    WORD32 i = 0;
    WORD32 dwRet = 0;
    if (dwTypePos == 0)
        return 0;

    /*内存优化，改为遍历查找，不用二分法*/
    dwBegin = cfgDat->tMapType[dwTypePos].dwItemPos;
    dwEnd = dwBegin + cfgDat->tMapType[dwTypePos].wCount;

    dwRet = 0;
    for(i = dwBegin; i < dwEnd ; i++)
    {
         if(gDatmatch)
        {
            printf("cfgDat->tMapItem[i].wValue =%d,wValue=%d \n",cfgDat->tMapItem[i].wValue,wValue);
        }
        if (cfgDat->tMapItem[i].wValue == wValue)
        {
            dwRet = i;
            break;
        }
    }

    return dwRet;
}

/* Find a character in a string and get it position */
int OamCfgFindCharInString(CHAR * pString, CHAR cChar, int * pPos)
{
    int i = 0;
    CHAR * pTemp = pString;

    if (pString == NULL)
        return PARAERR_NULLSTRING;
    while (pTemp != NULL && * pTemp != MARK_STRINGEND && * pTemp != cChar)
    {
        pTemp++;
        i++;
    }
    if (pTemp == NULL || * pTemp == MARK_STRINGEND)
    {
        * pPos = 0;
        return PARAERR_NOFIND;
    }
    *pPos = i;
    return PARA_SUCCESS;
}


/* convert the byte value to a string in order to display */
CHAR *OamCfgConvertWordToString(T_OAM_Cfg_Dat * cfgDat, WORD16 wType, WORD16 wValue)
{
    static CHAR aucMapErr[30];
    WORD32 dwItemPos = 0;
    memset(aucMapErr, 0, sizeof(aucMapErr));
    XOS_snprintf(aucMapErr, sizeof(aucMapErr), "MapErr[type=%d,value=%d]", wType, wValue);
    
    /* find */
    dwItemPos = FindMapItemPos(cfgDat, FindMapTypePos(cfgDat, wType), wValue);

    /* return */
    if (dwItemPos > 0)
    {
        /*return &(cfgDat->tMapString[cfgDat->tMapItem[dwItemPos].dwKeyPos]);*/
        return OAM_LTrim(OAM_RTrim(cfgDat->tMapString+cfgDat->tMapItem[dwItemPos].dwKeyPos));
    }
    else
        return aucMapErr;
}


/* convert the string value to a byte */
int ConvertStringToWord(TYPE_PARA *pPara, CHAR * pValue, WORD16 * pWValue, WORD16 * pMatchLen, T_OAM_Cfg_Dat * cfgDat)
{
    WORD32 i, dwTypePos, dwEndItemPos;
    CHAR * pString = NULL;
    int nLen;
    int nMatchs = 0;
    WORD32 dwTemp = 0;
    WORD16 wType = 0;
    WORD32 dwMinValue = 0;
    WORD32 dwMaxValue = 0;

    if (!pPara)
    {
        return PARAERR_DATATYPE;
    }

    wType = pPara->wLen;
    dwMinValue = (WORD32)pPara->Min.qdwMinVal;
    dwMaxValue = (WORD32)pPara->Max.qdwMaxVal;
    
    /* whether valid */
    if (wType == 0)
        return PARAERR_DATATYPE;

    dwTypePos = FindMapTypePos(cfgDat, wType);

    if (dwTypePos == 0)
        return PARAERR_DATATYPE;

    /* find */
    dwEndItemPos = cfgDat->tMapType[dwTypePos].dwItemPos + cfgDat->tMapType[dwTypePos].wCount;
    nLen = strlen(pValue);
    
    /*2004.10.11 modify by kongyj*/
    /*由于存在模糊匹配的情况, 所以可能一个参数匹配到多项, 这是需要再作一次精确匹配*/
    for (i = cfgDat->tMapType[dwTypePos].dwItemPos; i < dwEndItemPos; i++)
    {
        /*如果map值不在指定的范围内，不匹配*/
        if ((cfgDat->tMapItem[i].wValue < dwMinValue) || (cfgDat->tMapItem[i].wValue > dwMaxValue))
        {
            continue;
        }
        
        pString = (CHAR *) &cfgDat->tMapString[cfgDat->tMapItem[i].dwKeyPos];
        
        if (MatchTwoString(pValue, pString, pMatchLen) == 0)
        {
            nMatchs++;
            dwTemp = i; /*保存只匹配到一个的索引*/
        }
    }

    if(nMatchs == 1)
    {
        i = dwTemp; /*恢复索引*/
    }
    else /*精确匹配*/
    {
        for (i = cfgDat->tMapType[dwTypePos].dwItemPos; i < dwEndItemPos; i++)
        {
            pString = (CHAR *) &cfgDat->tMapString[cfgDat->tMapItem[i].dwKeyPos];
            if (strlen(pString) == strlen(pValue) && 
                MatchTwoString(pValue, pString, pMatchLen) == 0)
            {
                break;
            }
        }
    }

    /* return */
    if (i < dwEndItemPos)
    {
        *pWValue = cfgDat->tMapItem[i].wValue;
        return PARA_SUCCESS;
    }
    else
    {
        *pWValue = 0;
        return PARAERR_CONVERT;
    }
}

/* match an input string with the pattern string */
int MatchTwoString(CHAR *pInStr, CHAR *pPatternStr, WORD16 *pMatchLen)
{
    CHAR *pInChar = pInStr;
    CHAR *pPatternChar = pPatternStr;
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
        {
            if (*pMatchLen < i) 
                *pMatchLen = i;
            return 1;
        }
        i++;
    }

    *pMatchLen = i;

    if (nLenInStr <= nLenPatternStr)
        return 0;
    else
        return 1;
}

int StrCmpNoCase(CHAR * pInStr, CHAR * pPatternStr)
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

/*删除小数字符串小数点后面的若干'0'*/
void DeleteNouseZero(CHAR *str)
{
    CHAR *pStr = str;
    int iLen, i;
    while(*pStr != '\0')
    {
        if(*pStr == '.')
            break;
        pStr++;
    }
    if(*pStr == '.')
    {
        pStr++;

        iLen = strlen(pStr);
        i = iLen - 1;
        while(i > 0)
        {
            if(*(pStr + i) == '0')
                *(pStr + i) = *(pStr + i + 1);
            else if(*(pStr + i) != ' ') break;

            i--;
        }
    }
}

/*
parameter:str->value by paratype
1.convert str to a temp value according to the paramter type
2.verify the value beside the range of the command parameter
3.store the value to pass to the execute part.
*/
/* Function : ConvertStrToCmdParaByParaType
*   comment: convert the string to the command parameter according
*            to the type and the range of the command parameter
*   Input parameter : string, the command parater, the CMDPARA structure
*   Output: 0: success
*           1: convert error
*           2: overflow
*/
int ConvertStrToCmdParaByParaType(BYTE ucLinkID,CHAR *pString,T_OAM_Cfg_Dat * pcfgDat,TYPE_PARA* pParaInfo,WORD16 wParaNo,CMDPARA *pCmdPara,BYTE bDeterminer)
{
    int nReturnVal = PARA_SUCCESS;
    CHAR *pStringTmp = NULL;
    T_OAM_Cfg_Dat *pCfgDatTmp = NULL;
    TYPE_PARA* pParaInfoTmp = NULL;

    if((pParaInfo == NULL) || (pCmdPara == NULL))
    {
        return PARAERR_NULLSTRING;
    }

    pStringTmp = pString;
    pCfgDatTmp = pcfgDat;
    pParaInfoTmp = pParaInfo;

    /* 1.convert the string to a temp value according to the parameter type */
    /* 2.verify the temp value */
    /* 3.store the value to CMDPARA */
    switch (pParaInfo->bParaType)
    {
    case DATA_TYPE_BYTE:
            nReturnVal = ConvertStrToTypeByte(ucLinkID,pStringTmp,pParaInfoTmp,wParaNo, pCmdPara);
        break;
    case DATA_TYPE_WORD:
            nReturnVal = ConvertStrToTypeWord(ucLinkID,pStringTmp,pParaInfoTmp,wParaNo, pCmdPara);
        break;
    case DATA_TYPE_DWORD:
    case DATA_TYPE_INT:
            nReturnVal = ConvertStrToTypeDWord(ucLinkID,pStringTmp,pParaInfoTmp,wParaNo, pCmdPara);
        break;
    case DATA_TYPE_CHAR:
            nReturnVal = ConvertStrToTypeChar(ucLinkID,pStringTmp,pParaInfoTmp,wParaNo, pCmdPara);
        break;
    case DATA_TYPE_STRING:
            nReturnVal = ConvertStrToTypeString(ucLinkID,pStringTmp,pParaInfoTmp,wParaNo, pCmdPara,bDeterminer);
        break;
    case DATA_TYPE_TEXT:
            /*nReturnVal = ConvertStrToTypeText(pString, pMatchResult, pCmdLine);*/
        break;
    case DATA_TYPE_IPADDR:
            nReturnVal = ConvertStrToTypeIpaddr(ucLinkID,pStringTmp,pParaInfoTmp,wParaNo, pCmdPara);
        break;
    case DATA_TYPE_DATE:
            nReturnVal = ConvertStrToTypeDate(ucLinkID,pStringTmp,pParaInfoTmp,wParaNo, pCmdPara);
        break;
    case DATA_TYPE_TIME:
            nReturnVal = ConvertStrToTypeTime(ucLinkID,pStringTmp,pParaInfoTmp,wParaNo, pCmdPara);
        break;
    case DATA_TYPE_MAP:
            nReturnVal = ConvertStrToTypeMap(ucLinkID,pStringTmp,pParaInfoTmp,pcfgDat,wParaNo, pCmdPara);
        break;
    case DATA_TYPE_CONST:
            nReturnVal = ConvertStrToTypeConst(pStringTmp,pParaInfoTmp, pCmdPara);
        break;
    case DATA_TYPE_IFPORT:
            /*nReturnVal = ConvertStrToTypeIfport(pString, pCmdPara);*/
        break;
    case DATA_TYPE_HEX:
            nReturnVal = ConvertStrToTypeHex(ucLinkID,pStringTmp,pParaInfoTmp,wParaNo, pCmdPara);
        break;
    case DATA_TYPE_MASK:
            nReturnVal = ConvertStrToTypeIpaddrMask(ucLinkID,pStringTmp,pParaInfoTmp,wParaNo, pCmdPara);
        break;
    case DATA_TYPE_IMASK:
            nReturnVal = ConvertStrToTypeIpaddrIMask(ucLinkID,pStringTmp,pParaInfoTmp,wParaNo, pCmdPara);
        break;
    case DATA_TYPE_MACADDR:
            nReturnVal = ConvertStrToTypeMacAddr(ucLinkID,pStringTmp,pParaInfoTmp,wParaNo, pCmdPara);
        break;
    case DATA_TYPE_FLOAT:
            nReturnVal = ConvertStrToTypeFloat(ucLinkID,pStringTmp,pParaInfoTmp,wParaNo, pCmdPara);
        break;
    case DATA_TYPE_IPV6PREFIX:
            nReturnVal = ConvertStrToTypeIpv6prefix(ucLinkID,pStringTmp,pParaInfoTmp,wParaNo, pCmdPara);
        break;
    case DATA_TYPE_IPV6ADDR:
            nReturnVal = ConvertStrToTypeIpv6addr(ucLinkID,pStringTmp,pParaInfoTmp,wParaNo, pCmdPara);
        break;
    case DATA_TYPE_VPN_ASN:
            nReturnVal = ConvertStrToTypeVPNASN(ucLinkID,pStringTmp,pParaInfoTmp,wParaNo, pCmdPara);
        break;
    case DATA_TYPE_VPN_IP:
            nReturnVal = ConvertStrToTypeVPNIP(ucLinkID,pStringTmp,pParaInfoTmp,wParaNo, pCmdPara);
        break;
    case DATA_TYPE_WORD64:
        nReturnVal = ConvertStrToTypeWord64(ucLinkID,pStringTmp,pParaInfoTmp,wParaNo, pCmdPara);
        break;
    case DATA_TYPE_SWORD:
        nReturnVal = ConvertStrToTypeSword(ucLinkID,pStringTmp,pParaInfoTmp,wParaNo, pCmdPara);
        break;
    case DATA_TYPE_LOGIC_ADDR_M:
        nReturnVal = ConvertStrToTypeLogicAddrM(ucLinkID,pStringTmp,pParaInfoTmp,wParaNo, pCmdPara);
        break;
    case DATA_TYPE_LOGIC_ADDR_U:
        nReturnVal = ConvertStrToTypeLogicAddrU(ucLinkID,pStringTmp,pParaInfoTmp,wParaNo, pCmdPara);
        break;
    case DATA_TYPE_IPVXADDR:
        nReturnVal = ConvertStrToTypeIpvxaddr(ucLinkID,pStringTmp,pParaInfoTmp,wParaNo, pCmdPara);
        break;
    case DATA_TYPE_SYSCLOCK:
        nReturnVal = ConvertStrToTypeSysClock(ucLinkID,pStringTmp,pParaInfoTmp,wParaNo, pCmdPara);
        break;
    case DATA_TYPE_PARA:
        break;
    case DATA_TYPE_PHYADDRESS:
        nReturnVal = ConvertStrToTypePhyAddress(ucLinkID,pStringTmp,pParaInfoTmp,wParaNo, pCmdPara);
        break;
    case DATA_TYPE_LOGICADDRESS:
        nReturnVal = ConvertStrToTypeLogicAddress(ucLinkID,pStringTmp,pParaInfoTmp,wParaNo, pCmdPara);
        break;
    default:
        nReturnVal =  PARAERR_CONVERT;
        break;
    }
    memset(pCmdPara->sName, 0, sizeof(pCmdPara->sName));
    strncpy(pCmdPara->sName, pParaInfo->sName, sizeof(pCmdPara->sName));
    return nReturnVal;
}

/*
parameter:str->value
1.convert str to a temp value according to the paramter type
2.verify the value beside the range of the command parameter
3.store the value to pass to the execute part.
*/
/* Function : ConvertStrToValue
*   comment: convert the string to the command parameter according
*            to the type and the range of the command parameter
*   Input parameter : string, the command parater, the CMDPARA structure
*   Output: 0: success
*           1: convert error
*           2: overflow
*/
int ConvertStrToCmdPara(CHAR *pString, TYPE_LINK_STATE * pLinkState, T_OAM_Cfg_Dat * cfgDat)
{
    TYPE_MATCH_RESULT * pMatchResult = &(pLinkState->tMatchResult);
    TYPE_CMD_LINE * pCmdLine = &(pLinkState->tCmdLine);
    TYPE_PARA *pParaInfo = NULL;    /* information about the command parameter */
    CMDPARA *pCmdPara = NULL;   /* store one matched parameter */
    int nReturnVal = PARA_SUCCESS;
    WORD16 wParaNo = 0;
    int nWordLen = 0;    

    /* check struct type */
    if (cfgDat->tCmdPara[pMatchResult->wCurParaPos].bStructType != STRUCTTYPE_PARA)
        return PARAERR_MISUSESTRUCT;

    /* check the pString */
    nWordLen = strlen(pString);
    if (nWordLen == 0)
        return PARAERR_NULLSTRING;

    /* set proper value */
    nReturnVal = PARA_SUCCESS;
    pParaInfo = (TYPE_PARA *) &(cfgDat->tCmdPara[pMatchResult->wCurParaPos].uContent.tPara); 
    wParaNo = cfgDat->tCmdPara[pMatchResult->wCurParaPos].ParaNo;
    pCmdPara = (CMDPARA *) &(pMatchResult->tPara[pMatchResult->wParaSum]);
	
    nReturnVal = ConvertStrToCmdParaByParaType(pLinkState->ucLinkID,pString,cfgDat,pParaInfo,wParaNo,pCmdPara,pCmdLine->bDeterminer);	
    if (PARA_SUCCESS != nReturnVal)
    {
        return nReturnVal;
    }
    switch(pParaInfo->bParaType)
    {
        case DATA_TYPE_CONST:
	     if (PARA_SUCCESS != MatchTwoString(pString, pParaInfo->sName, &(pMatchResult->wLength)))
	     {
	         return PARAERR_CONSTANT;
	     }
	     pMatchResult->wLength = 1;
            pMatchResult->wParaSum--; /* 不加入匹配参数列表 */
	     break;
        default:
	     break;
    }	    
		
    /* modify the pMatchResult */
    if (nReturnVal == PARA_SUCCESS)
    {
        pMatchResult->wParaSum++;
        pMatchResult->wLength = 1;
    }
    return nReturnVal;
}

/* convert strings to data type WORD64 */
int ConvertStrToTypeWord64(BYTE ucLinkID,CHAR *pString, TYPE_PARA* pParaInfo,WORD16 wParaNo,CMDPARA *pCmdPara)
{
    int nReturnVal = 0;
    WORD64 *pdwStore = NULL;
    WORD64 qdwTemp = 0;

    if ((pString == NULL) || (pParaInfo == NULL) || (pCmdPara == NULL))
    {
        return PARAERR_CONVERT;
    }	
    
    /* 1.convert */
    if (PARA_SUCCESS != Convert_atoi(pString, &qdwTemp))
    {
        nReturnVal = PARAERR_CONVERT;
        return nReturnVal;
    }
    /* 2.verify */
    if (qdwTemp > pParaInfo->Max.qdwMaxVal || qdwTemp < pParaInfo->Min.qdwMinVal)
    {
        nReturnVal = PARAERR_OVERFLOW;
        return nReturnVal;
    }
    /* 3.store  */
    pdwStore = (WORD64 *) Oam_InptGetBuf(sizeof(WORD64),ucLinkID);
    * pdwStore = (WORD64)qdwTemp;
    pCmdPara->ParaNo = wParaNo;
    pCmdPara->Type   = pParaInfo->bParaType;
    pCmdPara->Len    = sizeof(WORD64);
    pCmdPara->pValue = (BYTE *)pdwStore;

    return nReturnVal;
}

int ConvertStrToTypeSword(BYTE ucLinkID,CHAR * pString,TYPE_PARA* pParaInfo,WORD16 wParaNo,CMDPARA *pCmdPara)
{
    CHAR *pTmpStr = NULL;
    SWORD16 swValue = 0;
    WORD16 wTmpValue = 0;
    BOOLEAN bNegative = FALSE;
    int nReturnVal = 0;
    WORD64 qdwTemp = 0;
    SWORD16 *pStore = NULL;
    SWORD16 swCurTmp;
    
    if ((pString == NULL) || (pParaInfo == NULL) || (pCmdPara == NULL))
    {
        return PARAERR_CONVERT;
    }	

    pTmpStr = pString;
    if (*pTmpStr == MARK_HYPHEN)
    {
        bNegative = TRUE;
        pTmpStr++; /* 去掉负号 */
    }

    /* 将去掉负号的字符串转换为整数 */
    if (PARA_SUCCESS != Convert_atoi(pTmpStr, &qdwTemp))
    {
        nReturnVal = PARAERR_CONVERT;
        return nReturnVal;
    }
    wTmpValue = (WORD16)qdwTemp;

    if (bNegative) /* 负数 */
    {
        swCurTmp = 0 - wTmpValue;
        if (wTmpValue > 32768)
            return PARAERR_OVERFLOW;
    }
    else  /* 非负数 */
    {
        swCurTmp = wTmpValue;
        if (wTmpValue > 32767)
            return PARAERR_OVERFLOW;
    }

    /* 2.verify */
    if (swCurTmp > (SWORD16)pParaInfo->Max.qdwMaxVal || swCurTmp < (SWORD16)pParaInfo->Min.qdwMinVal)
    {
        nReturnVal = PARAERR_OVERFLOW;
        return nReturnVal;
    }

    swValue = atoi(pString);
    pStore = (SWORD16 *) Oam_InptGetBuf(sizeof(SWORD16),ucLinkID);
    *pStore = swValue;
    
    pCmdPara->ParaNo = wParaNo;
    pCmdPara->Type   = DATA_TYPE_SWORD;
    pCmdPara->Len    = sizeof(SWORD16);
    pCmdPara->pValue = (BYTE *)pStore;
    
    return PARA_SUCCESS;
}

CHAR * GetFormatByCmdID(WORD32 dwCmdId, T_OAM_Cfg_Dat * cfgDat)
{
    WORD16 i;
    CHAR * pFormatStr = NULL;
    
    for (i=0; i<cfgDat->nCmdAttrSum; i++)
    {
        if (cfgDat->tCmdAttr[i].dExeID == dwCmdId)
        {
            pFormatStr =  GetFormatByCmdAttrPos(i, cfgDat);
            break;
        }
    }
    return pFormatStr; 
}

CHAR *GetFormatByCmdAttrPos(WORD16 wCmdAttrPos, T_OAM_Cfg_Dat * cfgDat)
{
    CHAR * pFormatStr = NULL;

    if (wCmdAttrPos == INVALID_CMDINDEX)
        return pFormatStr;
    
    if (cfgDat->tCmdAttr[wCmdAttrPos].wToShowLen == 0)
        return pFormatStr;

    pFormatStr = &cfgDat->tFormat[cfgDat->tCmdAttr[wCmdAttrPos].dwToShowPos];

    return pFormatStr; 

}

/**************************************************************************
* 函数名称：VOID Oam_DbgShowDatMatch
* 功能描述：打开Oam_DbgShowDatMatch开关
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
void Oam_DbgShowDatMatch(void)
{
    gDatmatch = 1;
}

/**************************************************************************
* 函数名称：VOID Oam_DbgCloseDatMatch
* 功能描述：关闭Oam_DbgCloseDatMatch开关
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
void Oam_DbgCloseDatMatch(void)
{
    gDatmatch = 0;
}

/**************************************************************************
* 函数名称：VOID ShowOAMDebugMode
* 功能描述：显示OAMDebugMode开关
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
void ShowOAMDebugDatMatch(void)
{
    if (gDatmatch)
        printf("gDatmatch = 1 \n");
    else
        printf("gDatmatch = 0 \n");
}


/*lint +e123*/
/*lint +e959*/
/*lint +e968*/
/*lint +e922*/
/*lint +e578*/
/*lint +e539*/
/*lint +e931*/
