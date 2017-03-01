/*********************************************************************
* ��Ȩ���� (C)2002, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ�oam_fd.h
* �ļ���ʶ��
* ����ժҪ��OAM�ص���������
* ����˵����
            
* ��ǰ�汾��
* ��    �ߣ�����
* ������ڣ�2007.6.5
*
* �޸ļ�¼1��// �޸���ʷ��¼�������޸����ڡ��޸��߼��޸�����
*    �޸����ڣ�
*    �� �� �ţ�
*    �� �� �ˣ�
*    �޸����ݣ�
* �޸ļ�¼2����
**********************************************************************/
#ifndef __OAM_FD__
#define __OAM_FD__

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

#define OAM_OK          0
#define OAM_ERROR       (-1)

#define MAX_BUF_LEN     512

typedef struct tagOAM_FD_MSG
{
    WORD16    fd;      /*�ļ�������*/
    WORD16    flag;    /*�������� 1-���� 2-TELNET 3-HTTP 4-SSH*/
    WORD16    len;     /*��Ϣ����*/
    BYTE      buf[MAX_BUF_LEN]; /*��Ϣ��*/
}_PACKED_1_ OAM_FD_MSG;

typedef struct 
{
    WORD16    fd;      /*�ļ�������*/
    WORD16    flag;    /*�������� 1-���� 2-TELNET 3-HTTP 4-SSH*/
    WORD16    len;     /*��Ϣ����*/
    BYTE      buf[1];  /*��Ϣ��*/
}_PACKED_1_ OAM_FD_MSG_EX;

typedef struct tagOAM_SSH_TEL_MSG
{
    WORD16    fd;      /*�ļ�������*/
    WORD16    result;  /*�ɹ����1-�ɹ�,0-ʧ�� */
    WORD16    len;     /*��Ϣ����*/
    BYTE      buf[MAX_BUF_LEN]; /*��Ϣ��*/
}_PACKED_1_ OAM_SSH_TEL_MSG;


extern WORD32 OamRegCallback(void);
extern WORD32 OamRegConsoleCallback(void);

#ifdef WIN32
    #pragma pack()
#endif

#endif
