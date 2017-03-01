/**************************************************************************
* 版权所有 (C)2002, 深圳市中兴通讯股份有限公司。
*
* 文件名称：oam_cfg_crc16.c
* 文件标识：
* 内容摘要：配置代理程序中文件crc校验实现
* 其它说明：
            
* 当前版本：
* 作    者       ：殷浩
* 完成日期：20080328
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

/*lint -e618*/
/*lint -e570*/
/*lint -e539*/

/* CCITT CRC16 */
const static WORD16 CRC16_CCITT_Tab[] =
{    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
     0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
     0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
     0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
     0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
     0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
     0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
     0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
     0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
     0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
     0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
     0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
     0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
     0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
     0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
     0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
     0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
     0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
     0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
     0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
     0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
     0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
     0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
     0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
     0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
     0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
     0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
     0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
     0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
     0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
     0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
     0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0
};

static BYTE s_aucBuf[OAM_CRC16_RDFILE_BUF_SIZE] = {0};
/**************************************************************************
*                          全局函数原型                                   *
**************************************************************************/
/**************************************************************************
*                          局部函数原型                                   *
**************************************************************************/
static WORD16 crc16cont(BYTE *pData, WORD16 dwDataLen, WORD16 wCrc);


/**************************************************************************
* 函数名称：WORD32 CalCrc16CheckSum
* 功能描述：计算校验和
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
WORD32 CalCrc16CheckSum( CHAR * pcFileName, WORD16 *pwCRC)
{
    WORD16  wCRC = 0;
    WORD32 sdwByteCount = 0;
    WORD32 sdwLeftFileSize = 0;
    XOS_FD fdFile = NULL;
    WORD32 dwFileOperRtn = 0;

    assert(pcFileName != NULL);
    if (NULL == pcFileName)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "InParam's pointer is null\n");
        return OAM_CRC16_CACL_FAIL;
    }
    
    dwFileOperRtn = XOS_OpenFile(pcFileName, /*OSS_EXC |*/ XOS_RDONLY, &fdFile);
    if (XOS_SUCCESS != dwFileOperRtn)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "Open file [%s] failed\n", pcFileName);
        return OAM_CRC16_CACL_FAIL;
    }
	
    XOS_GetFileLength(pcFileName, (SWORD32 *)&sdwLeftFileSize);

    while (sdwLeftFileSize > 0)
    {
        dwFileOperRtn = XOS_ReadFile(fdFile,
                                     (void *)s_aucBuf,
                                     OAM_CRC16_RDFILE_BUF_SIZE,
                                     (SWORD32 *)&sdwByteCount);
        if (sdwLeftFileSize > OAM_CRC16_RDFILE_BUF_SIZE)
        {
            assert(sdwByteCount == OAM_CRC16_RDFILE_BUF_SIZE);
            if (sdwByteCount != OAM_CRC16_RDFILE_BUF_SIZE)
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR, "Read file failed!\n");
                XOS_CloseFile(fdFile);
                return OAM_CRC16_CACL_FAIL;
            }
            sdwLeftFileSize = sdwLeftFileSize-OAM_CRC16_RDFILE_BUF_SIZE;
        }
        else
        {
            assert(sdwByteCount == sdwLeftFileSize);
            if (sdwByteCount != sdwLeftFileSize)
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR, "Read file failed!\n");
                XOS_CloseFile(fdFile);
                return OAM_CRC16_CACL_FAIL;
            }

            sdwLeftFileSize = 0;
        }
        wCRC = crc16cont(s_aucBuf, (WORD16)sdwByteCount, wCRC);
    }
    XOS_CloseFile(fdFile);

    *pwCRC = wCRC;

    return OAM_CRC16_CACL_SUCCESS;
}

/**************************************************************************
* 函数名称： WORD16 crc16cont(BYTE *pData, 
*                             WORD32 dwDataLen, 
*                             WORD32 dwIncr, 
*                             WORD32 dwCrc)
* 功能描述： CRC16计算，采用CCITT查表数据
* 访问的表： 
* 修改的表： 
* 输入参数： pData      待校验的数据
*            dwDataLen  待校验的数据长度
*            dwIncr     每次递增的大小，一般为1
*            dwCrc      循环校验的返回值
* 输出参数： 
* 返 回 值： CRC16校验值
* 其它说明： 
* 修改日期    版本号     修改人         修改内容
* -----------------------------------------------
* 2002/10/17   V1.0a      MLJ            算法修改
* 2002/10/08   V1.0       MLJ            Created
**************************************************************************/
static WORD16 crc16cont(BYTE *pData, WORD16 dwDataLen, WORD16 wCrc)
{
    CHAR ucTemp;
    
    while (dwDataLen--)
    {
        ucTemp = (CHAR)(wCrc >> 8);
        wCrc <<= 8;
        wCrc ^= CRC16_CCITT_Tab[(*pData ^ ucTemp) & 0xff];
        pData++;
    }
    return wCrc;
}
/**************************************************************************
* 函数名称：BOOL Oam_GetDatCrcInTxt(CHAR *pcFileName, WORD32 *dwDatCrc)
* 功能描述：获取txt存盘文件中保存的dat文件crc校验码
* 访问的表：无
* 修改的表：无
* 输入参数：CHAR *pcFileName
* 输出参数：WORD32 *dwDatCrc
* 返 回 值   ：true-成功 false-失败
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2008/03/28   V1.0    殷浩      创建
**************************************************************************/
BOOL Oam_GetDatCrcInTxt(CHAR *pcFileName, WORD32 *dwDatCrc)
{
    XOS_FD fdTxtFile = OAM_INVALID_FD;
    BYTE aucRdBuf[50] = {0};

    WORD32 swCnt = 0;
    WORD32 dwRtn = 0;

    dwRtn = XOS_OpenFile(pcFileName, /*OSS_EXC |*/ XOS_RDONLY, &fdTxtFile);
    if (XOS_SUCCESS != dwRtn)
    {
        return FALSE;
    }

    dwRtn = XOS_ReadFile ( fdTxtFile, aucRdBuf, strlen(OAM_DATCRC_MARK_IN_SAVEFILE)+OAM_SIZEOF_DATCRC, (SWORD32 *)&swCnt);
    if (XOS_SUCCESS != dwRtn)
    {
        XOS_CloseFile(fdTxtFile);
        return FALSE;
    }

    if (strncmp((CHAR *)aucRdBuf, OAM_DATCRC_MARK_IN_SAVEFILE, strlen(OAM_DATCRC_MARK_IN_SAVEFILE)) != 0)
    {
        XOS_CloseFile(fdTxtFile);
        return FALSE;
    }

    memcpy(dwDatCrc, aucRdBuf + strlen(OAM_DATCRC_MARK_IN_SAVEFILE), OAM_SIZEOF_DATCRC);
    
    XOS_CloseFile(fdTxtFile);
    return TRUE;
}

/**************************************************************************
* 函数名称：BYTE Oam_CheckCrc16(CHAR *pcFileName)
* 功能描述：插入crc校验码到存盘文件中
* 访问的表：无
* 修改的表：无
* 输入参数：CHAR *pcFileName
* 输出参数：
                              #define OAM_CHECK_CRC_FAIL       0
                              #define OAM_CHECK_CRC_OK          1
                              #define OAM_CHECK_CRC_NOCRC    2
* 返 回 值   ：参见头文件定义
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2008/03/28   V1.0    殷浩      创建
**************************************************************************/
BYTE Oam_CheckTxtCrc16(CHAR *pcFileName)
{
    WORD32 dwRtn = 0;
    WORD16 wCrc16InFile = 0;
    WORD16 wCrc16OfFile = 0;
    XOS_FD fdBakFile = OAM_INVALID_FD;
    BYTE ucGetCrcRst = 0;
    BYTE ucCalCrcRst = 0;

    assert(pcFileName != NULL);
    if (!pcFileName)
    {
        return OAM_CHECK_CRC_FAIL;
    }

    /*打开待读取的存盘文件*/
    dwRtn = XOS_OpenFile(pcFileName, /*OSS_EXC |*/ XOS_RDONLY, &fdBakFile);
    if (XOS_SUCCESS != dwRtn)
    {
        if (XOS_FILE_ERR_FILE_NOT_EXIST == dwRtn)
        {
            return OAM_CHECK_CRC_FILE_NOT_EXIST;
        }
        else
        {
            return OAM_CHECK_CRC_FAIL;
        }
    }

    /*获取TXT文件中的TXT CRC校验和*/
    ucGetCrcRst = Oam_GetTxtCrcInTxt(fdBakFile, &wCrc16InFile);
    if (ucGetCrcRst != OAM_CHECK_CRC_OK)
    {
        XOS_CloseFile(fdBakFile);
        return ucGetCrcRst;
    }
    
    ucCalCrcRst = Oam_CalTxtCrcOfTxt(fdBakFile, &wCrc16OfFile);
    if (ucGetCrcRst != OAM_CHECK_CRC_OK)
    {
        XOS_CloseFile(fdBakFile);
        return ucCalCrcRst;
    }

    XOS_CloseFile(fdBakFile);
    
    if (wCrc16OfFile == wCrc16InFile)
    {
        return OAM_CHECK_CRC_OK;
    }
    else
    {
        return OAM_CHECK_CRC_FAIL;
    }
}

BYTE Oam_GetTxtCrcInTxt(XOS_FD fdTxtFile, WORD16 *wTxtCrc)
{
    SWORD32 swCnt = 0;
    XOS_STATUS dwRtn = 0;
    BYTE aucRdBuf[256] = {0};

    dwRtn = XOS_ReadFile (fdTxtFile, aucRdBuf, strlen(OAM_DATCRC_MARK_IN_SAVEFILE), &swCnt);
    if (XOS_SUCCESS != dwRtn)
    {
        return OAM_CHECK_CRC_FAIL;
    }

    /*如果没有crc校验码，返回*/
    if (strncmp((CHAR *)aucRdBuf, OAM_CRC_MARK, strlen(OAM_CRC_MARK)) != 0)
    {
        return OAM_CHECK_CRC_NOCRC;
    }
    else if (strncmp((CHAR *)aucRdBuf, OAM_DATCRC_MARK_IN_SAVEFILE, strlen(OAM_DATCRC_MARK_IN_SAVEFILE)) == 0)
    {
        /*如果开头是datcrc*/
        /*跳过文件头中datcrc校验码和回车换行:BBBB\n\r*/
        swCnt = 0;
        dwRtn = XOS_SeekFile(fdTxtFile, XOS_SEEK_CUR, OAM_SIZEOF_DATCRC);
        if (XOS_SUCCESS != dwRtn)
        {
            return OAM_CHECK_CRC_FAIL;
        }

        do
        {
            swCnt = 0;
            memset(aucRdBuf, 0, sizeof(aucRdBuf));
            dwRtn = XOS_ReadFile (fdTxtFile, aucRdBuf, 1, &swCnt);
            if (swCnt != 1)
            {
                break;	
            }
            
            if (XOS_SUCCESS != dwRtn)
            {
                return OAM_CHECK_CRC_FAIL;
            }   	
        }while(aucRdBuf[0] == 0x0A || aucRdBuf[0] == 0x0D);
        
        if(aucRdBuf[0] != '[')
        {
            return OAM_CHECK_CRC_NOCRC;
        }
        else
        {
            dwRtn = XOS_SeekFile(fdTxtFile, XOS_SEEK_CUR, -1);
            if (XOS_SUCCESS != dwRtn)
            {
                return OAM_CHECK_CRC_FAIL;
            }	
        }

		/*读取下一段crc头长度，看是否有txtcrc*/
        swCnt = 0;
        memset(aucRdBuf, 0, sizeof(aucRdBuf));
        dwRtn = XOS_ReadFile (fdTxtFile, aucRdBuf, strlen(OAM_TXTCRC_MARK_IN_SAVEFILE), &swCnt);
        if (XOS_SUCCESS != dwRtn)
        {
            return OAM_CHECK_CRC_FAIL;
        }
		
        if (strncmp((CHAR *)aucRdBuf, OAM_TXTCRC_MARK_IN_SAVEFILE, strlen(OAM_TXTCRC_MARK_IN_SAVEFILE)) != 0)
        {
            /*如果不是txtcrc,文件指针前移txtcrc头长度，说明没有txtcrc*/
            return OAM_CHECK_CRC_NOCRC;
        }
    }
    else if (strncmp((CHAR *)aucRdBuf, OAM_TXTCRC_MARK_IN_SAVEFILE, strlen(OAM_TXTCRC_MARK_IN_SAVEFILE)) == 0)
    {
        /*如果开头就是txtcrc,不用做任何操作，继续下面的操作*/
    }
		
    /*读取文件中的crc校验码*/
    dwRtn = XOS_ReadFile (fdTxtFile, wTxtCrc, sizeof(WORD16), &swCnt);
    if (XOS_SUCCESS != dwRtn)
    {
        return OAM_CHECK_CRC_FAIL;
    }

    return OAM_CHECK_CRC_OK;
    
}

BYTE Oam_CalTxtCrcOfTxt(XOS_FD fdTxtFile, WORD16 *wTxtCrc)
    {
    SWORD32 swCnt = 0;
    XOS_STATUS dwRtn = 0;
    BYTE aucRdBuf[256] = {0};
    CHAR aucTmpBakFile[100] = {0};
    XOS_FD fdTmpBakFile = OAM_INVALID_FD;
    SWORD32 swWriteCnt = 0;
    
    do
    {
        swCnt = 0;
        memset(aucRdBuf, 0, sizeof(aucRdBuf));
        dwRtn = XOS_ReadFile (fdTxtFile, aucRdBuf, 1, &swCnt);
        if (swCnt != 1)
        {
            break;	
        }
            
        if (XOS_SUCCESS != dwRtn)
        {
            return OAM_CHECK_CRC_FAIL;
        }   	
    }while(aucRdBuf[0] == 0x0A || aucRdBuf[0] == 0x0D);
        
    dwRtn = XOS_SeekFile(fdTxtFile, XOS_SEEK_CUR, -1);
    if (XOS_SUCCESS != dwRtn)
    {
        return OAM_CHECK_CRC_FAIL;
    }	
	
    /*打开待写入的临时文件*/
    /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
    XOS_snprintf(aucTmpBakFile,
                 sizeof(aucTmpBakFile) -1,
                 "%s.tmp",
                 OAM_CFG_PRTCL_SAVE_FILE_ORG);

    dwRtn = XOS_OpenFile(aucTmpBakFile, XOS_CREAT, &fdTmpBakFile);
    if (XOS_SUCCESS != dwRtn)
    {
        return OAM_CHECK_CRC_FAIL;
    }
	
    /*生成去除校验码的临时文件*/
    do
    {
        swCnt = 0;
        dwRtn = XOS_ReadFile (fdTxtFile, aucRdBuf, sizeof(aucRdBuf), &swCnt);
        if (XOS_SUCCESS != dwRtn)
        {
            XOS_CloseFile(fdTmpBakFile);
            XOS_DeleteFile(aucTmpBakFile);
            return OAM_CHECK_CRC_FAIL;
        }
        dwRtn = XOS_WriteFile (fdTmpBakFile, (void *)aucRdBuf, swCnt, &swWriteCnt);
        if (XOS_SUCCESS != dwRtn)
        {
            XOS_CloseFile(fdTmpBakFile);
            XOS_DeleteFile(aucTmpBakFile);
            return OAM_CHECK_CRC_FAIL;
        }
    }while(!((WORD32)swCnt < sizeof(aucRdBuf)) && dwRtn == XOS_SUCCESS);

    XOS_CloseFile(fdTmpBakFile);

    /*计算临时文件的校验码*/
    if (CalCrc16CheckSum(aucTmpBakFile, wTxtCrc) != OAM_CRC16_CACL_SUCCESS)
    {
        XOS_DeleteFile(aucTmpBakFile);
        return OAM_CHECK_CRC_FAIL;
    }
    XOS_DeleteFile(aucTmpBakFile);
	
    return OAM_CHECK_CRC_OK;
}

/*lint +e618*/
/*lint +e570*/
/*lint +e539*/


