/*********************************************************************
* 版权所有 (C)2002, 深圳市中兴通讯股份有限公司。
*
* 文件名称：oam_cfg_crc16.h
* 文件标识：
* 内容摘要：crc校验码计算
* 其它说明：
            
* 当前版本：
* 作    者       ：殷浩
* 完成日期：20080328
*
* 修改记录1：// 修改历史记录，包括修改日期、修改者及修改内容
*    修改日期：
*    版 本 号：
*    修 改 人：
*    修改内容：
* 修改记录2：…
**********************************************************************/
#ifndef __OAM_CFG_CRC16_H__
#define __OAM_CFG_CRC16_H__

#ifndef WIN32
    #ifndef _PACKED_1_
        #define _PACKED_1_ __attribute__ ((packed)) 
    #endif
#else
    #ifndef _PACKED_1_
        #define _PACKED_1_
    #endif
#endif

#ifdef WIN32
    #pragma pack(1)
#endif

#define OAM_SIZEOF_DATCRC        (sizeof(WORD32))
#define OAM_SIZEOF_TXTCRC         (sizeof(WORD16))

#define OAM_CRC16_RDFILE_BUF_SIZE     512

#define OAM_CRC16_CACL_FAIL            -1
#define OAM_CRC16_CACL_SUCCESS     0
WORD32 CalCrc16CheckSum( CHAR *pcFileName, WORD16 *pwCRC);

/*检查文件crc是否正确的返回值Oam_CheckCrc16 用*/
#define OAM_CHECK_CRC_FAIL       0
#define OAM_CHECK_CRC_OK          1
#define OAM_CHECK_CRC_NOCRC    2
#define OAM_CHECK_CRC_FILE_NOT_EXIST 3
BYTE Oam_CheckTxtCrc16(CHAR *pcFileName);

BOOL Oam_GetDatCrcInTxt(CHAR *pcFileName, WORD32 *dwDatCrc);
	
BYTE Oam_GetTxtCrcInTxt(XOS_FD fdTxtFile, WORD16 *wTxtCrc);
BYTE Oam_CalTxtCrcOfTxt(XOS_FD fdTxtFile, WORD16 *wTxtCrc);

#ifdef WIN32
    #pragma pack()
#endif

#endif /* __OAM_CFG_CRC16_H__*/

