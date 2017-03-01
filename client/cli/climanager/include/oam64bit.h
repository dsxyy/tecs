/*********************************************************************
* ��Ȩ���� (C)2002, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ�Oam64Bit.h
* �ļ���ʶ��
* ����ժҪ��
* ����˵����
            
* ��ǰ�汾��
* ��    �ߣ����� ��ֲ����
* ������ڣ�2007.6.5
*
* �޸ļ�¼1��// �޸���ʷ��¼�������޸����ڡ��޸��߼��޸�����
*    �޸����ڣ�
*    �� �� �ţ�
*    �� �� �ˣ�
*    �޸����ݣ�
* �޸ļ�¼2����
**********************************************************************/
#ifndef __OAM_64BIT_H__
#define __OAM_64BIT_H__

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

#define OAM64bit_SUCCESS        0

/* ���Ⱪ¶�ӿ� */
extern INT32 OamCfgintpGetStringFrom64bit(UINT32 Low32bit,        /* ��32λ */
                                    UINT32 High32bit,       /* ��32λ */
                                    CHAR * pResultString    /* ��� */
                                    );

extern BYTE int2Str_Of64bit(WORD32 dwHigh, WORD32 dwLow, CHAR *pResult );  
extern BYTE getWord64Str(CHAR *pInput, CHAR *pResult );  

#ifdef WIN32
    #pragma pack()
#endif

#endif  /* __OAM_64BIT_H__ */

