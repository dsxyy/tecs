/*********************************************************************
* ��Ȩ���� (C)2006, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ� convertencoding.h
* �ļ���ʶ��
* ����ժҪ��
* ����˵���� ��
* ��ǰ�汾�� 
* ��    �ߣ� �����ס���3Gƽ̨
*��������: 2008/9/10
**********************************************************************/

/***********************************************************
 *                    ������������ѡ��                     *
***********************************************************/
#ifndef _CONVERTENCODING_H_
#define _CONVERTENCODING_H_


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

extern BOOLEAN  Gb2312ToUtf8(const CHAR *ptrGBBuf,WORD16 wDataLen,CHAR *ptrUTF8Buf);/*GB2312ת��ΪUTF-8*/

#ifdef WIN32
    #pragma pack()
#endif

#endif /* __PARA_CNVT_H__ */



