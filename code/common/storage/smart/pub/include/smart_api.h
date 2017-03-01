/*************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾
* All rights reserved.
* 
* �ļ����ƣ�smart_api.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ�����̼��ģ��ӿ�ͷ�ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�guojsh
* ������ڣ�2011��8��20��
*
* �޸ļ�¼1��
*  �޸����ڣ�2013-7-25
*  �� �� �ţ�2.0
*  �� �� �ˣ�л����
*  �޸����ݣ�����smartctl����ű��ķ�ʽ
*************************************************************************/

#ifndef _SMART_API_H
#define _SMART_API_H

/* ���̵Ľ���״̬��أ���SMART״̬ */
/* old */
#define PD_GOOD                      1   /* ����״̬��� */
#define PD_BROKEN                    0   /* ����״̬���� */
/* new */
#define PD_STATUS_NORMAL             1   /* ���� */
#define PD_STATUS_WARN               2   /* ����, ���޸� */
#define PD_STATUS_FAULT              3   /* ����, �������������ݲ�����Ӳ�� */

#define PD_SUPPORT_SMART             1   /* ֧��SMART */
#define PD_NOT_SUPPORT_SMART         0   /* ��֧��֧��SMART */

#define PD_SMART_ON                  1   /* SMART�� */
#define PD_SMART_OFF                 0   /* SMART�ر� */

#define PD_SHORT_SELFTEST            1    /* �̼�� */
#define PD_LONG_SELFTEST             2    /* ����� */

/* ʧ���� */
#define _SMART_SUCCESS               0
#define _SMART_E_RW                  1
#define _SMART_E_GET_STATUS          2
#define _SMART_E_SELFTEST            3
#define _SMART_ERROR                -1 

#define DM_PD_NAME_LEN               12    /* �豸���Ƴ��� */

#define DISK_TYPE_ATA                1 /* ATA Ӳ�� */
#define DISK_TYPE_SCSI               2 /* SCSI Ӳ�� */

/* �������̵�SMART��Ϣ */
typedef struct pdSmartInfo_t
{   /* �ýṹ��Ҫ���Ӷ�������������� */
    char aucPdName[DM_PD_NAME_LEN];   /* out �����豸�� �� /dev/sda */
    char ucDiskType;                  /* out �������� */
    char ucIsSupportSmart;            /* out �����Ƿ�֧��SMART */
    char ucIsSmartEnable;             /* out ����SMART�����Ƿ��� */
    char ucSmartStatus;               /* out ���̵�SMART״̬�Ƿ�OK */
    int sdwSelfTestTime;              /* out ִ���Լ�����ʱ�� */
} T_DmPdSmartInfo;


#endif


