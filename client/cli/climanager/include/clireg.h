
#ifndef _OAM_CLI_REGISTER_H_
#define _OAM_CLI_REGISTER_H_

#include "pub_oam_cfg_event.h"

/* �ź����������ڴ������� */
#if 0
#define  OAM_SEM_CLI_REG                  "OAM_SemCliReg"       /* ƽ̨Ӧ��ע��DAT����ʹ�õ��ź���   */
#define  OAM_SHM_CLI_REG                  "OAM_ShmCliReg"       /* ƽ̨Ӧ��ע��DAT��Ϣʹ�õĹ����ڴ� */
#define  OAM_SEM_CLI_REGEX                "OAM_SemCliRegEx"     /* ��ƽ̨ҵ��ע��ʹ�õ��ź���   */
#define  OAM_SHM_CLI_REGEX                "OAM_ShmCliRegEx"     /* ��ƽ̨ҵ��ע��ʹ�õĹ����ڴ� */
#endif

/* �����ڴ���С */
/*#define  OAM_CLI_DAT_SHM_MAX_NUM                     100*/ /* ������ƽ̨Ӧ��ʹ�õĹ����ڴ�� */
/*#define  OAM_CLI_APP_AGT_SHM_MAX_NUM                  1*/  /* �����Ϸ�ƽ̨ʹ�õĹ����ڴ�飬ÿ�鵥��ֻ֧��ע��һ��ҵ����� */
/*#define  OAM_CLI_APP_MGT_SHM_MAX_NUM                 50*/  /* OMP�Ϸ�ƽ̨ʹ�õĹ����ڴ�� */

/*����omp����������֧�ֵ�ע����Ŀ��һ����ȡ�����Ϊ�����ڴ��м�¼��Ŀ*/
/*#define  OAM_CLI_APP_SHM_NUM  (OAM_CLI_APP_MGT_SHM_MAX_NUM > OAM_CLI_APP_AGT_SHM_MAX_NUM ? OAM_CLI_APP_MGT_SHM_MAX_NUM : OAM_CLI_APP_AGT_SHM_MAX_NUM)*/

/*T_CliAppData��ucReadFlag��ȡֵ*/
#define  OAM_CLI_REG_NOT_READED        0
#define  OAM_CLI_REG_READED            1

/* ע������ */
#define  OAM_CLI_REG_TYPE_INVALID        0
#define  OAM_CLI_REG_TYPE_3GPLAT         1
#define  OAM_CLI_REG_TYPE_SPECIAL        2

#if 0
/* ע��DAT����ʹ�õĹ����ڴ�أ�ƽ̨ʹ�� */
typedef struct
{
    JID             tAppJid;          /* Ӧ��ע���JID */
    WORD32          dwDatFuncType;    /* Ӧ��ע���DAT�Ĺ��ܰ汾���� */
    BYTE          ucReadFlag;     /* Manager�Ƿ��ȡ�ı�� */
    BYTE          ucCmpFlag;       /* �Ƿ��Ѿ��ȽϹ��ı�� */
    BYTE          ucSaveType;     /*��������1-TXT���̣�0-ZDB����*/
    BYTE          ucPad[1];
} T_CliAppData;

typedef struct
{
    WORD32          dwSemid;
    pid_t           platpid;                             /*�������pid�������ж��Ƿ�Ҫ�干���ڴ�*/
    WORD32          dwRegCount;                            /* ��ǰע����� */
    T_CliAppData    tCliReg[OAM_CLI_DAT_SHM_MAX_NUM];      /* ע������ */
} T_CliAppReg;

/* ��ƽ̨ҵ��ע�ᣬ��ƽ̨ʹ�� */
typedef struct
{
    JID             tAppJid;      /* Ӧ��ע���JID */
    WORD32          dwNetId;    /* Ӧ��ע�����Ԫ��ʶ*/
    BYTE           ucAgingTime;   /* ��ע����Ϣ�ϻ�ʱ��������������3��ͬ��ʱ�䣬����Ϊ�ϻ� */
    BYTE           ucPad[3];
} T_CliAppDataEx;

typedef struct
{
    WORD32           dwSemid;                                                 /*�ź���*/
    pid_t            apppid;                                                     /*�������pid�������ж��Ƿ�Ҫ�干���ڴ�*/
    WORD32          dwRegCount;                            /* ��ǰע����� */
    T_CliAppDataEx  tCliReg[OAM_CLI_APP_SHM_NUM];     /* ע������ */
} T_CliAppAgtRegEx;
#endif

extern T_CliAppReg* Oam_GetRegTableAndLock(void);
extern BOOLEAN Oam_UnLockRegTable(T_CliAppReg *ptOamCliReg );
extern T_CliAppAgtRegEx* Oam_GetRegExTableAndLock(void);
extern BOOLEAN Oam_UnLockRegExTable(T_CliAppAgtRegEx *ptOamCliReg );

#endif

