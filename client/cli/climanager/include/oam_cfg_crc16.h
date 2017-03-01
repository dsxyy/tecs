/*********************************************************************
* ��Ȩ���� (C)2002, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ�oam_cfg_crc16.h
* �ļ���ʶ��
* ����ժҪ��crcУ�������
* ����˵����
            
* ��ǰ�汾��
* ��    ��       �����
* ������ڣ�20080328
*
* �޸ļ�¼1��// �޸���ʷ��¼�������޸����ڡ��޸��߼��޸�����
*    �޸����ڣ�
*    �� �� �ţ�
*    �� �� �ˣ�
*    �޸����ݣ�
* �޸ļ�¼2����
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

/*����ļ�crc�Ƿ���ȷ�ķ���ֵOam_CheckCrc16 ��*/
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

