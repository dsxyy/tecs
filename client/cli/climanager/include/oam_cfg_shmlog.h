/*********************************************************************
* ��Ȩ���� (C)2002, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ�oam_cfg_shmlog.h
* �ļ���ʶ��
* ����ժҪ�������ڴ�������־ģ��
* ����˵����
            
* ��ǰ�汾��
* ��    ��       �����
* ������ڣ�20090604
*
* �޸ļ�¼1��// �޸���ʷ��¼�������޸����ڡ��޸��߼��޸�����
*    �޸����ڣ�
*    �� �� �ţ�
*    �� �� �ˣ�
*    �޸����ݣ�
* �޸ļ�¼2����
**********************************************************************/
#ifndef __OAM_CFG_SHMLOG_H__
#define __OAM_CFG_SHMLOG_H__

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

/*�����ڴ��б������־��¼��*/
#define OAM_MAX_SHM_LOG_NUM         1000

/*������־��¼�ṹ*/
typedef struct tag_T_OamCfgShmlogRecord
{
    BYTE         ucMSState;    /*climanager ����״̬*/
    BYTE         ucPad;
    WORD16  wJobState;    /*climanager JOB ״̬*/
    WORD32  dwMsgId;       /*climanager �յ�����Ϣid*/
}_PACKED_1_  T_OamCfgShmlogRecord;

/*�����ڴ�ṹ*/
typedef struct tag_T_OamCfgLogShm
{
    WORD32                            dwSemid;    /*���ʻ����ź���*/
    WORD32                            dwCurIdx;   /*��־���鵱ǰ�����±�*/
    T_OamCfgShmlogRecord   atLogs[OAM_MAX_SHM_LOG_NUM]; /*��־����*/
}_PACKED_1_  T_OamCfgLogShm;

extern void Oam_CfgWriteShmLog(WORD16 wState, WORD32 dwMsgId);

#ifdef WIN32
    #pragma pack()
#endif

#endif /* __OAM_CFG_SHMLOG_H__*/
 
