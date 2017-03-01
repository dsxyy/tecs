/*********************************************************************
* ��Ȩ���� (C)2010, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ� usp_capi_micro.h
* �ļ���ʶ�� �����ù���ƻ���
* ����ժҪ�� USP��Ŀ�궨��
* ����˵���� ��
* ��ǰ�汾�� 0.1
* ��    �ߣ� tao.linjun@zte.com.cn
* ������ڣ� 2010-11-2
**********************************************************************/

#ifndef __USP_CAPI_MICRO_H__
#define __USP_CAPI_MICRO_H__

#define MAX_ENC_NUM_IN_SYS          MAX_UP_SUB_ENCL  /* ����֧�ּ������� */
#define MAX_PD_NUM_IN_ENC           16
#define MAX_PD_NUM_IN_SYS           132              /* ��ӦSPR11֧�ּ���10����չ�����֧��132����̶������޸�*/
#define MAX_LUN_NUM_IN_SYS          1024
#define MAX_MAP_GROUP_NUM_IN_SYS    16
#define MAX_INI_NUM_IN_MAP_GROUP    8
#define MAX_LUN_NUM_IN_MAP_GROUP    256
#define MAX_MAP_GRP_NUM_LUN_IN      10
#define MAX_MAP_GRP_NUM_LUN_ADDED    8          /* LUN �����������ӳ������Ŀ */

#define MAX_VERSION_LEN             58
#define MAX_CONFIG_FILE_NAME_LEN    52

#define MAX_LOG_LEN                 256    /* ��־������������󳤶� */
#define MAX_LOG_NUM                 1000
#define MAX_LOG_NUM_ONCE            50     /* ÿ����SMA�ṩ����־��Ŀ */
#define MAX_LOG_EXPORT_NAME_LEN     58     /* ������־�ļ�����󳤶� */

#define MAX_IP_LEN                  16
#define MAX_LINE_LEN                80     /* ���ж�ȡ�����ļ� */
#define NET_NAME_MAX                12     /* �����豸���Ƴ��� */

/* CACHE д��д������ */
#define CACHE_WRITE_BACK_POLICY     0
#define CACHE_WRITE_THROUGH_POLICY  1

/* CACHE�����Ƿ�д�ڴ��־ */
#define CACHE_WRITE_TO_MEMORY          0      /* д�ڴ� */
#define CACHE_NOT_WRITE_MEMORY         1      /* ��д�ڴ� */

/*ȡ����DM*/
//#define MAX_PD_GUID_LEN            4
#define MAX_PD_MODEL_NAME_LEN       42
#define MAX_PD_TYPE_LEN             8
#define MAX_PD_FW_VER_LEN           10
#define MAX_PD_SNO_LEN              22
#define MAX_PD_STD_VER_LEN          4



/* Ԫ������صĺ궨�壬BLK, DM ��ͬʹ�� */
#define SB_QUANTA       1024         /* KB */   /* �����̵�ԭʼ������1M ���� */
#define BLK_SB_SECTION  32*1024     /* KB */   /* BLK Ԫ��������С32M */
#define SPARE_SB_OFFSET  256*1024   /* KB */   /* ����Ԫ��������ʼ��ַ */
#define USC_RESERVED     512*1024   /* KB */   /* ����512M���ڴ��Ԫ������Ϣ�ͻ�����Ϣ */
#define DM_SB_SIZE       1024       /* KB */   /* DMԪ��������ռ�Ĵ�С */


/*ȡ����Blk*/

#define MAJOR_VOL  18
#define MAJOR_VD        17
#define MAJOR_SNAP  's'
#define MAJOR_REPOSITORY  'r'

#define VDKCDEV_MAJOR 52
#define VDKCDEV_MINOR   0

#define MIRROR_CDEV_MAJOR 53
#define MIRROR_CDEV_MINOR  0
/* �洢����ӿ�ʼ */
#define POOLS_MAJOR  68
#define POOLS_MINOR  8

#define MAX_DISKS_IN_POOL             256
#define MIN_DISKS_IN_POOL             8
#define MAX_POOL_NUM_IN_SYS           8      /*//< ϵͳ�����洢����Ŀ*/
#define MAX_VOL_NUM_ON_POOL           1024

/*�޸Ĵ洢������ʱ����Ҫ�޸ĵĲ���*/
#define NO_MODIFY -1

/* �洢�ؿռ䱣������*/
#define POOL_CAPACITY_POLITY_NO_RESERVE           1        //������
#define POOL_CAPACITY_POLITY_RESERVE_ONE          2        //����һ����̵Ŀռ�
#define POOL_CAPACITY_POLITY_RESERVE_TWO          3        //����������̵Ŀռ�

/*�洢���������澯��ֵ*/
#define MAX_POOL_ALARM_THR     50

/* �����жϴ��̴�����vd �� pool */
#define TYPE_VD                     1         //���̴�����vd
#define TYPE_POOL                   2         //���̴�����pool

/* �����ж�vol������������vd �� pool */
#define VOL_IN_VD                  VOL_OWNED_BY_VD    //vol����vd��ȡֵ1
#define VOL_IN_POOL                VOL_OWNED_BY_POOL  //vol����pool��ȡֵ2

/* �洢���޸Ĵ������� */
#define ADD_DISK_TO_POOL           1       //���Ӵ��̵��洢����
#define DEL_DISK_FROM_POOL         2       //�Ӵ洢����ɾ������

/* pool���޸ı�� */
#define POOL_MODIFY_ATTR_NAME       1
#define POOL_MODIFY_ATTR_ALARM_THR  2
#define POOL_MODIFY_ATTR_CAPA_POL   3

/* �����洢�صķ�ʽ */
#define  POOL_CREATE_BY_DISK_NUM                  1         //��������Ŀ�����洢��
#define  POOL_CREATE_BY_DISK_LIST                 2         //��������Ϣ�б����洢��

/* �޸Ĵ洢�صĴ��̷�ʽ */
#define  POOL_MODIFY_DISK_BY_NUM                  1         //��������Ŀ�޸Ĵ洢�صĳ�Ա��
#define  POOL_MODIFY_DISK_BY_LIST                 2         //��������Ϣ�б��޸Ĵ洢�صĳ�Ա��
/*�洢�ص�״̬*/

#define  POOL_DAMAGED         0                  /*�洢��������*/
#define  POOL_GOOD            1                          /*�洢����������*/
#define  POOL_REDISTRIBUTING  2          /*�洢�����طֲ���*/
#define  POOL_INSUFCAPACITY   3          /*�洢�������������������ߴ�����С��8*/

#define  MAX_UNUSED_PD        MAX_PD_NUM_IN_SYS /* ��ϵͳ��������������һ�� */
/*�洢�����������İ�����*/
#define MAX_RAID0_UNPLUG_DISKS  0
#define MAX_RAIDELSE_UNPLUG_DISKS  1
#define MAX_RAID6_UNPLUG_DISKS  2
/* �洢����ӽ��� */

#define MAX_VD_NUM_IN_SYS           32      ///< ϵͳ�������������
#define MAX_PD_NUM_IN_VD            32      ///< ��������������������
#define MIN_PD_NUM_IN_VD            1       ///< ����������С���������
#define MAX_BLK_NAME_LEN            32
#define MAX_VD_IMPORT_PD_NUM        2       ///< �������������������Ŀ
#define MAX_VD_EXPORT_PD_NUM        2       ///< �������������������Ŀ
#define MAX_PD_NUM_ADD              4       ///< ������ÿ��������ӵ����������Ŀ
#define MAX_VOL_NUM_ON_VD           32
#define MAX_NUM_ONEVOLTYPE_ON_VD           128 /*vd��ָ�����͵ľ����Ϊ128��*/

#define MAX_ALLTYPE_VOL_NUM_ON_VD        (MAX_NUM_ONEVOLTYPE_ON_VD*2+MAX_VOL_NUM_ON_VD)  /*vd��ָ�����͵ĸ������͵ľ����Ϊ128+128+32��*/

/********** �������� add by yaodong *********/
#define VD_DISK_DEFRAG_TYPE         (WORD32)0  /* �����̴�����Ƭ���� */
#define VD_RECOVERY_TYPE            (WORD32)1  /* �����ָ̻����ؽ��� */
#define VD_EXPAND_TYPE              (WORD32)2  /* ��������չ */
#define VD_RELEVEL_TYPE             (WORD32)3  /* �����̱伶 */
#define VOL_EXPAND_TYPE             (WORD32)4  /* ������ */
#define VOL_CHUNK_CHANGE_TYPE       (WORD32)5  /* ������ */
#define CLONE_INIT_TYPE             (WORD32)6  /* �������ʼ�� */
#define CLONE_SYNC_TYPE             (WORD32)7  /* ������ͬ�� */
#define CLONE_RSYNC_TYPE            (WORD32)8  /* ������ͬ�� */
#define DISK_SCAN_TYPE              (WORD32)9  /* ����ɨ�� */
/* add by huangan */
#define FILE_UPLOAD_TYPE            (WORD32)10   /* �ļ��ϴ� */
#define MIRROR_SYNC_TYPE            (WORD32)11   /* Զ�̾���ͬ�� */
#define DATA_REDISTRIBUTE_TYPE      (WORD32)12 /*�����طֲ�*/
#define SNAP_RESTORE_TYPE           (WORD32)13  /* Դ�����ݻع� */
#define SYS_MSG_COLLECT_TYPE        (WORD32)14

/* for vol */
#define MAX_VOL_ID                  32
#define MIN_VD_ID                   0
#define MAX_VD_ID                   32
#define MIN_CHUNK_SIZE              4
#define MAX_CHUNK_SIZE              512

#define MAX_VOL_NUM_IN_SYS         (MAX_VOL_NUM_ON_VD *  MAX_VD_NUM_IN_SYS)         //xygang 2010-11-19

/* for snapshot*/
#define MAX_SVOL_NUM_IN_SYS         128     ///< ϵͳ����������
#define MAX_SVOL_NUM_IN_VOL         8      ///< ������������
#define MAX_COW_CHUNKSIZE           (2*1024)  /*cow_chunk���Ϊ2*1024KB*/

/* for clone */
#define MAX_CLONE_IN_VOL           8
#define MAX_CLONE_IN_SYS         128
#define MIN_PRIVATE_CAP            4  /* ˽�о����С���� һ��page 4KB */

/* ͬ����ͬ�������ȼ�*/
#define SYNC_LOW     0
#define SYNC_MEDIUM  1
#define SYNC_HIGH    2

#define CLONE_INIT          0  /* ��ʼ��״̬, ����clone ���״̬, ��̨д0��д1*/
#define CONSISTENT          1  /* �������Դ����һ��������, ����һ����*/
#define SYNCING             2  /* ���ڽ���ͬ��*/
#define REVERSE_SYNCING     3  /* ���ڽ��з�ͬ��*/
#define SYNCED              4  /* �������Դ����ͬ����, �������һ�� */
#define OUT_OF_SYNC         5  /* ͬ��ʱʧ��*/
#define OUT_OF_REVERSE_SYNC 6  /* ��ͬ��ʱʧ��*/
#define OUT_OF_INIT         7  /*  ��ʼ��ʧ��*/


/* Դ���clone��Ĺ���״̬ */
#define FRACTURED   0       /* ���� */
#define UNFRACTURED 1       /* ���� */

#define NOT_INIT_SYNC   0     /* ������ʼͬ�� */
#define IS_INIT_SYNC    1     /* ����ʼͬ�� */

#define MANUAL_POLICY   0     /* �ֶ��ָ����� */
#define AUTO_POLICY     1     /* �Զ��ָ����� */

/* for MapGrp*/
#define MAX_MAPGRP_NAME_LEN         32

/*BLK�������Ƿ�������� */
#define BLK_TASK_EXIST      1    /* ������ڱ��  */

/*************** used by iSCSI/SCS/CLI,iSCSI������ ****************************/
#define MAX_IQN_LEN                 ((WORD32)223)    /* ����Э�����֧��iqn����(ͬʱ������initiator�˺�target��) */
#define MAX_TGT_NAME_LEN_OLD        ((WORD32)64)     /* ���֧��targetname���� */
#define MAX_TGT_NAME_LEN            ((WORD32)224)     /* ���֧��targetname���� */
#define MAX_INI_NAME_LEN_OLD        ((WORD32)128)    /* ���֧��initiator���ֳ��� */
#define MAX_INI_NAME_LEN            ((WORD32)224)     /* ���֧��initiator���� */

#define MAX_REGISTER_INI_NAME_LEN   (WORD32)(MAX_INI_NAME_LEN + 3 + 15 + 1)
#define MAX_REGISTER_TGT_NAME_LEN   (WORD32)(MAX_TGT_NAME_LEN + 3 + 5 + 1)
/* ������Ϊ256;��Ϊ256ʱ��tcs_common.h�еĽṹT_IscsiTidInfo����̫��(19548)����ʹ��_IOת��IOCTLNUMʱʧ��(size��14bit,16384);
   ����ʱ�ᱨ�� " case label does not reduce to an integer constant " */
#define MAX_INITIATOR_NUM           ((WORD32)32)    /* ���֧��initiator���� */
#define ISCSI_MAX_REC_INITIATOR_NUM ((WORD32)32)    /* ���֧�ּ�¼������initiator���� */
#define ISCSI_MAX_REC_SESSION_NUM   ((WORD32)1024)    /* ���֧�ֲ鿴��Session���� */
#if 0
#define CHAP_ACCOUNT_NAME_MAX       32  /* ��δʹ�� */
#define CHAP_ACCOUNT_PASS_MAX       14  /* ��δʹ�� */
#endif

#define MAX_ISCSI_PORT_NUM_PER_CTL  6
#define MAX_ISCSI_SESSION_COUNT     1024
#define MAX_ISCSI_SESSION_NUM_ONCE  10//5/* ÿ�λ�ȡ�Ự���� */
#define MAX_FC_GPN_NUM_ONCE  60//5/* ÿ�λ�ȡFC WWPN���� */
enum
{
    key_initial_r2t,          /* 0  */
    key_immediate_data,       /* 1  */
    key_max_connections,      /* 2  */
    key_max_recv_data_length, /* 3  */
    key_max_xmit_data_length, /* 4  */
    key_max_burst_length,     /* 5  */
    key_first_burst_length,   /* 6  */
    key_default_wait_time,    /* 7  */
    key_default_retain_time,  /* 8  */
    key_max_outstanding_r2t,  /* 9  */
    key_data_pdu_inorder,     /* 10 */
    key_data_sequence_inorder,/* 11 */
    key_error_recovery_level, /* 12 */
    key_header_digest,        /* 13 */
    key_data_digest,          /* 14 */
    key_ofmarker,             /* 15 */
    key_ifmarker,             /* 16 */
    key_ofmarkint,            /* 17 */
    key_ifmarkint,            /* 18 */
    session_key_last          /* 19 */
};
enum
{
    key_wthreads,             /* 0  */
    key_target_type,          /* 1  */
    key_queued_cmnds,         /* 2  */
    target_key_last           /* 3  */
};
/*******************************************************************************/
#define MAX_FC_PORT_NUM_PER_CTL     0
#define MAX_PORT_NUM_PER_CTL        (MAX_FC_PORT_NUM_PER_CTL + MAX_ISCSI_PORT_NUM_PER_CTL)
#define MAX_CTL_NUM                 2
#define MAX_ISCSI_PORT_NUM          (MAX_ISCSI_PORT_NUM_PER_CTL * MAX_CTL_NUM)
//#define MAX_FC_PORT_NUM             (MAX_FC_PORT_NUM_PER_CTL * MAX_CTL_NUM)
#define MAX_PORT_NUM                (MAX_PORT_NUM_PER_CTL * MAX_CTL_NUM)
/********************************************************************************/

#define ENC_ALL_CTL                 (2)
#define MAIN_ENC_EXPANDER           (-1)

#define MAX_DEV_PATH_LEN            32
#define MAX_DEV_UUID_LEN            16

#define MAX_FILE_NAME               20
/***************************************enum************************************/
/* eSwitch */
#define OFF                         0                  /* ��*/
#define ON                          1                   /* ��*/
#define RST                         2                  /*  ����*/

#define INVALID_VALUE               (-1)

#define DEFAULT_VALUE             0XFFFF                         //ȱʡֵ

/* ePdSrc */
#define PD_LOCAL                    0
#define PD_FOREIGN                  2

/* ePdState */
#define PD_CAN_NOT_READ             1
#define PD_NORMAL_RW                0

/* ePdType */
#define PD_UNKNOWN                  3
#define PD_UNUSED                   0
#define PD_HOTSPARE                 4
#define PD_DATA                     5

/* ���̵Ľ���״̬��أ���SMART״̬ */
#define PD_GOOD                     1
#define PD_BROKEN                   0

#define PD_SUPPORT_SMART            1
#define PD_NOT_SUPPORT_SMART        0
#define PD_SMART_ON                 1
#define PD_SMART_OFF                0

#define PD_FIT        1
#define PD_UNFIT      2

/* ����ɨ����صĿ��Ʊ�־ */
#define PD_SCAN_START               0                        //��ʼɨ��
#define PD_SCAN_PAUSE               1                        //��ͣɨ��
#define PD_SCAN_STOP                2                        // ֹͣɨ��

/* ��̨����������״̬ */
#define TASK_RUNNING                0                       //����״̬
#define TASK_PAUSE                  1                       //��ͣ״̬
#define TASK_DELAY                  2                       // ����״̬

/* ��̨����Ĳ��� */
#define TASK_CTRL_RUN               0                        //���б���ͣ�ĺ�̨����
#define TASK_CTRL_PAUSE             1                        //��ͣ��̨����
#define TASK_CTRL_DEL               2                        //ɾ����̨����

/* ϵͳ����̨������Ŀ */
#define MAX_TASK_NUM_IN_SYS         512                     //ϵͳ������̨������Ŀ���ݶ�
#define TASK_UPPER_LIMIT            16                     //ϵͳ�����е�����̨������Ŀ���ݶ�
#define DM_TASK_UPPER_LIMIT         8                      //ϵͳ�����е����ɨ��ɨ��������
#define TASK_PER_VD                 2                      //ÿ��vd��������еĺ�̨����
#define MAX_TASK_PRARM_LEN          128                    // �ƻ�/��̨�����������󳤶�
#define MAX_OBJECT_NAME_LEN         32                     // (�ƻ�/��̨����)�������Ƶ���󳤶�

#define MAX_TASK_OBJECT_NAME_LEN            80                     // ��̨����������Ƶ���󳤶�
#define MAX_SCHEDULE_OBJECT_NAME_LEN        32                     // �ƻ�����������Ƶ���󳤶�

/* eVdState */
#define VD_GOOD                     0
#define VD_FAULT                    1
#define VD_DEGRADE                  2
#define VD_RECING                   3
#define VD_RECERR                   4
#define VD_SCSFAULT                 5
#define VD_SCSINITING                6
/* vd->local */
#define ISFREEZE                  1         /*��ʾ�����Ѿ����ᣬams�ɸ������ֵ����������web�ɲ���ѡ��Ϊ:��ѯ�ͽⶳ����*/
#define ISLOCAL                   0          /*��ʾ���أ�ִ��Ǩ��ʵ���ϰ����ⶳ������ams�ɸ������ֵ�������еĲ���*/
#define FOREIGN                   2           /*��ʾ��أ�ams�ɸ������ֵ����������web�ɲ���ѡ��Ϊ:��ѯ��Ǩ������*/

/* eVolIOtype */
#define CHUNK_CUSTOM                0  /* custom chunksize */
#define CHUNK_FILESYSTEM            1  /* set chunksize 128K*/
#define CHUNK_DATABASE              2  /* set chunksize 64K */
#define CHUNK_MULTIMEDIA            3  /* set chunksize 512K */
#define CHUNK_INVALID               0xffff

/*eRvolNotify*/
#define FAIL_SVOL                  0           /*Fail snapshot volume */
#define FAIL_WRITE_TO_VOL          1           /*Fail write to base volume*/

#define TCS_PORT_NUM                    4     /* ����˿��� */

/* eVolType */
#define set_vol_type(var, type)    do {   \
    (var) |= (1 << (type)) ;                  \
}while(0)

#define clear_vol_type(var, type)    do {   \
    (var) &= (~(1 << (type)));                  \
}while(0)

#define is_vol_type(var, type)     ( (var) & (1 << (type)) )

/* note:  λģʽ, ʹ������ĺ�������*/
#define NORM_VOL                   0
#define SNAP_SOURCE                1     /* note:  blk�ڲ�ʹ��,����IO�� */
#define SNAPSHOT_VOL               2
#define SNAP_ENTITY_VOL            3
#define CLONE_VOL                  4
#define PRIVATE_VOL                5
#define INVALID_VOL                6     /* ÿ�������vol����ʱ, ���¸�ֵ */


/* eRaidlevel */
#define RAID0                      0
#define RAID1                      1
#define RAID3                      3
#define RAID5                      5
#define RAID6                      6
#define RAIDTP                     7
#define RAID10                     10
#define RAID30                     30
#define RAID50                     50
#define RAID60                     60
#define NRAID                      200
#define RAID_LEVEL_INVALID         0xffff

/* eVolState */
#define VOL_S_GOOD                 0
#define VOL_S_FAULT                1
#define VOL_S_ERR                  2
#define VOL_S_CHUNKSIZE_MIGRATION  3
#define VOL_S_EXPAND               4
#define VOL_S_UNREADY              5
#define VOL_S_INVALID              1000
#define VOL_PARTIAL      10       /* ֻ��ǰ��lun��û�к��vol����� */
#define VOL_ENTIRE    11         /* ȫ�ˣ���˶����������ֻ��scs�ɼ� */
#define VOL_S_RESTORING    7         /* ���ڿ��ջָ��в��������ɾ������ */


/* eVolMapState */
#define MAPPED_VOL                 1
#define NO_MAPPED_VOL              0

/* eVolTaskType */
#define VOL_TASK_CHUNK             0    /* segment migration */
#define VOL_TASK_RESHAPE           1

/* eSvolState */
#define SVOL_S_ACTIVE              1
#define SVOL_S_STOP                0
#define SVOL_S_FAULTY              2

/* eCtlAction */
#define CTL_SINGLE_REBOOT          2
#define CTL_ONLINE_REBOOT          1
//#define CTL_CLOSE                  0

/* eCtlState */
#define CTL_POWEROFF             (-1)   /* �Զ��µ�״̬ */
#define CTL_NOEXIST                0    /* �Զ˲�����״̬����ȡ�Զ�״̬ʱ�Ż���ִ�ֵ */
#define CTL_EXIST                  1    /* �Զ���λ */
#define CTL_BOOTING                2    /* ����״̬ */
#define CTL_RUNNING                3    /* ����״̬ */
#define CTL_UNKNOWN                4    /* �Զ�δ֪״̬*/

/* ϵͳ״̬ */
#define SYS_STATE_UNKOWN           0
#define SYS_SINGLE_BOOTING         1
#define SYS_SINGLE_RUNNING         2
#define SYS_DUAL_BOOTING           3
#define SYS_DUAL_RUNNING           4
#define SYS_FAIL_OVER              5
#define SYS_FAIL_BACK              6

/* ͨ��״̬ */
#define CTL_MON_RCC_STATE_FAULT    0
#define CTL_MON_RCC_STATE_SETUP    1
/* eCtlRole */
#define CTL_SLAVE                  0
#define CTL_MASTER                 1

/* ��־�澯�ʼ����ʼ��û�������ַ���� */
#define MAX_ALARMAIL_USER          3
#define MAX_MAILTO_LEN             128
#define DEFUALT_ALARM_MAIL_PERIOD  30
#define DEFUALT_ALARM_MAIL_TO      "uspsystem@storage.usp.com"
#define LOG_INQUIRE_ABOVE_LEV_FLAG 0
#define LOG_INQUIRE_ONE_LEV_FLAG   1
#define LOG_USER_NONE_DEFAULT_NAME "<unknown>"
/* �ʼ�������غ� */
#define MAIL_SINGLE_ACCOUNT_LEN    128                  /* ��ĵ����ʼ��˺ŵ������������� */
#define MAIL_MAX_PASSLEN           64                   /* ����׼���� */
#define MAIL_MAX_HOST_LEN          128                  /* ׼������������Ƴ��� */
#define MAIL_SEND_ENABLE           (BYTE)1              /* �ʼ����߸澯�ʼ�ʹ�ܺ� */
#define MAIL_SEND_DISABLE          (BYTE)0              /* �ʼ����߸澯�ʼ����ú� */
#define MAIL_SMTP_PORT             25                   /* SMTP�ʼ�Ĭ�϶˿� */
#define MAIL_HOST_ADDRESS_TYPE     0                    /* Ĭ��ΪIP��ַ */
#define DEFUALT_MAILBOX_PASSDWD    "123456"             /* Ĭ�ϵ��������� */
#define DEFUALT_CODEC_MAIL_PASSWD  "******"             /* ���ܵ����� */
#define DEFUALT_MAIL_HOST_NAME     "192.168.1.1"        /* Ĭ�ϵ�������ַ */

/* eConfigFileType */
#define HTML_FILE_TYPE             1
#define BIN_FILE_TYPE              2

/* E_LogLevel ����Խ�� ��ֵԽС */
#define E_LOG_NOTICE               5
#define E_LOG_WARNING              4
#define E_LOG_ERROR                3
#define E_LOG_CRITICAL             2

/* д�ڴ��дԪ�������ı�־ */
#define WRITE_MDA_FLG               (SWORD32)0
#define WRITE_MEM_FLG               (SWORD32)1

/* Ӳ����ϵͳ�еĿɼ�״̬�궨�� */
#define CTRL0                       (SWORD32)0               //���ػ����£�������0�ɼ�
#define CTRL1                       (SWORD32)1               //���ػ����£�������1�ɼ�
#define CTRL_DUAL                   (SWORD32)2               //˫�ػ����£��������������ɼ�
#define CTRL0_ABNORMAL              (SWORD32)3               //˫�ػ����£���������0�ɼ����쳣״̬
#define CTRL1_ABNORMAL              (SWORD32)4               //˫�ػ����£���������1�ɼ����쳣״̬

/* �û�������صĺ궨�� */
#define MAX_USER_NAME_LEN           24
#define MAX_USER_EMAIL_LEN          128

#define MAX_USER_PSW_LEN            16

#define MAX_ADMIN_NUM               1
#define MAX_COMMON_USER_NUM         3
#define MAX_GUEST_USER_NUM          5
#define MAX_USER_NUM                (MAX_ADMIN_NUM + MAX_COMMON_USER_NUM + MAX_GUEST_USER_NUM)

/* ����/ �رտ�����ʹ�õĺ궨�� */
#define CTRL_LOCAL               0                              //���˱�־
#define CTRL_PEER                1                              //�Զ˱�־
#define CTRL_DEFAULT             0XFFFF                         //��������Ϣ��ȱʡĬ�ϱ�־


/* ScanBox Expander ���˽ṹ��� */
#define SCANBOX_MAX_EXP_NUM      2        /* ScanBox����Expander��Ŀ */
#define SCANBOX_MAX_DISK_NUM     28       /* ScanBox��������Ŀ */

#define ENC_SCANBOX              1        /* ScanBox���� */
#define ENC_USP                  3        /* USP���� */


/* ϵͳ������صĺ궨�� */
#define  MAX_NET_DEVICE_NUM           6                             /*ϵͳ�п����õ������豸�������*/
#define  DUAL_MAX_NET_DEVICE_NUM      (MAX_NET_DEVICE_NUM*2)        /*˫�������豸��Ŀ�����Ŀ*/

#define  NET_ROLE_BUSINESS            0        /*ҵ�����͵������豸*/
#define  NET_ROLE_MANAGEMENT          1        /*�������͵������豸*/
#define  MAX_NET_DEVICE_NAME_LEN      6        /* �����豸������󳤶� */

#define  SPR10_ISCSI_PORT_NUM         4                           /*ISCSI �˿���Ŀ*/
#define  SPR10_DUAL_ISCSI_PORT_NUM    (SPR10_ISCSI_PORT_NUM*2)    /*˫��ISCSI �˿�����Ŀ*/

#define  SCSI_MAX_STATICROUTE_NUM     128          /* ISCSI ���̬·����Ŀ */

#define  ISCSI_PING_TIMEOUT              1               /* Ping ��ʱ���*/
#define  ISCSI_MAC_ISSET              0xA5


#define  BROAD_UNKNOWN                0          /*δ֪����*/
#define  BROAD_SBCJ                   1          /*SBCJ��*/
#define  BROAD_SPR10                  2          /*SPR10����*/
#define  BROAD_SPR11                  3          /*SPR11����*/
#define  BROAD_PC                     4          /*PC ����*/


/* ʱ������ */
#define  NTP_HOST_NAME_LEN          (WORD32)64  /* max host name len */
#define  TIME_ZONE_CITY_NAME_LEN    (WORD32)64
#define  TIME_ZONE_CITY_NUM         (WORD32)100

#define  NTP_DEBUG            (WORD32)1
#define  NTP_AUTO             (BYTE)1
#define  NTP_MANUAL           (BYTE)0

/* �����嶨�� */
#define USP_BT_PC          (0xFFFF)   /* PC���� */
#define USP_BT_SBCW        (0x0214)   /* SBCW���� */
#define USP_BT_SBCJ        (0x0219)   /* SBCJ���� */
#define USP_BT_SPR10       (0x6701)   /* SPR10���� */
#define USP_BT_SPR11       (0x6718)   /* SPR11���� */

/* �汾��غ� */
#define  MEDIA_MOUNT_DIR "/IDE0"
#define  VERSION_DIR     "/IDE0/localver/"
#define  CONFIG_DIR      "/IDE0/uspcfg/"
#define  LOG_EXP_DIR     "/IDE0/log/"
#define  WORK_DIR        "/usr/sbin/"
#define  SCS_TMP_DIR     "/usr/sbin/tmp/"
#define  USP_VER_TYPE    1
#define  BOOT_VER_TYPE   8
#define  EXP_VER_TYPE    8500
#define  VPD_VER_TYPE    8501
#define  POW_VER_TYPE    8502
#define  UPDATE_EXP_RUNNING     (BYTE)(0x1A)
#define  UPDATE_EXP_NORUNNING   (BYTE)(0xA1)
#define  UPDATE_BOOT_RUNNING    (BYTE)(0x1B)
#define  UPDATE_BOOT_NORUNNING  (BYTE)(0xB1)
#define  UPDATE_USP_START       (BYTE)(0x11)
#define  UPDATE_USP_ERROR       (BYTE)(0x22)
#define  UPDATE_USP_STOP        (BYTE)(0x33)
#define  UPDATE_USP_INIT        (BYTE)(0x44)
#define  ALLOW_USP_UPDATE       (BYTE)(1)
#define  QUESTION_USP_UPDATE    (BYTE)(2)

#define  FILE_EXIST_DUAL        1
#define  FILE_EXIST_LOCAL       2
#define  FILE_EXIST_PEER        3
#define  FILE_NOT_EXIST_DUAL    4
#define  FILE_EXIST_SINGLE      5
#define  FILE_NOT_EXIST_SINGLE  6
#define  CUR_OR_STAND_NAME_SAME 7

#define  VERSION_FILE_TYPE     1
#define  CONFIG_FILE_TYPE      2
#define  OTHER_FILE_TYPE       3
#define  LOG_FILE_TYPE         4

/* FTP��غ� */
#define  FILE_NAME_LEN         (BYTE)(52)
#define  FILE_PATH_LEN         (BYTE)(80)
#define  USR_NAME_LEN          (BYTE)(20)
#define  PASSWD_LEN            (BYTE)(20)
#define  MAX_FW_FILE_NAME_LEN  FILE_NAME_LEN

/* ϵͳ�����غ궨�� */
#define  MAX_DIAG_DEVICE_NAME_LEN    64   /* �����豸���Ƴ��� */
#define  MAX_DIAG_DEVICE_NUM         32   /* �����豸��Ŀ */

/********* BBU��� ***********/

/* BBU�Ƿ���λ */
#define  DIAG_BBU_PRESENT            0    /* BBU��λ */
#define  DIAG_BBU_NOPRESENT          1    /* BBU����λ */

/* BBU�ܷ�ͨ�� */
#define  BBU_COMMUNICATE_OK          0    /* BBU������ͨ�� */
#define  BBU_COMMUNICATE_FAIL        2    /* BBU��������ͨ�� */

/* BBU�ܷ��� */
#define  BBU_CHARGE_OK               0    /* BBU��������� */
#define  BBU_CHARGE_FAIL             3    /* BBU����������� */

/* BBU�ܷ�ŵ� */
#define  BBU_DISCHARGE_OK            0    /* BBU�������ŵ� */
#define  BBU_DISCHARGE_FAIL          4    /* BBU���������ŵ� */

/* BBU�Ƿ���� */
#define BBU_NORMAL_TEMPERATURE       0    /* BBU�¶����� */
#define BBU_OVER_TEMPERATURE         5    /* BBU�¶ȹ��� */

/* BBU�Ƿ�ʧЧ */
#define BBU_VALID                    0    /* BBUδʧЧ */
#define BBU_INVALID                  6    /* BBUʧЧ�������������:һ��BBU�����Բ��ɻָ�����;���ǳ��ﲻ��USP������Ҫ�����͵���Ҫ�� */


/* BBU������״̬ */
#define BBU_NORMAL                   0    /* BBU���� */
#define BBU_ABNORMAL                 1    /* BBU�쳣 */
#define BBU_LOW_CAPACITY             2    /* BBU�������� */



#define  DIAG_BBU_CHARGING           0    /* BBU���ڳ�ŵ� */
#define  DIAG_BBU_NOT_CHARGING       1    /* BBU���ڳ�ŵ� */

#define  DIAG_ALL_EXPANDER           0    /* ��ȡ�������������Ϣ */
#define  DIAG_SINGLE_EXPANDER        1    /* ��ȡ�������������Ϣ */

#define  DEVICE_STATUS_OK            0    /* �豸״̬���� */
#define  DEVICE_STATUS_FALSE         1    /* �豸״̬�쳣 */
#define  DEVICE_STATUS_DOWN         2    /* �豸δ���� */

#define  DIAG_FC_PRESENT             3    /* FC�ӿ�  ��λ */
#define  DIAG_FC_ABSENT              4    /* FC�ӿ�����λ */
#define  DIAG_SOURCE_USER            0    /* �û��·���ϵͳ��� */
#define  DIAG_SOURCE_SCS             1    /* ϵͳ��ʱ��� */

#define  MAX_PROP_REC_NUM            48   /* ��϶������������¼�� */
#define  MAX_PROP_NAME_LEN           48   /* ��϶��������������󳤶� */
#define  MAX_PROP_VALUE_LEN          48   /* ��϶��������ֵ��󳤶�*/
#define  DIAG_DEVICE_BBU             0    /* ���BBU */
#define  DIAG_DEVICE_EPLD            1    /* ���EPLD */
#define  DIAG_DEVICE_EEPROM          2    /* ���EEPROM */
#define  DIAG_DEVICE_CRYSTAL         3    /* ��Ͼ��� */
#define  DIAG_DEVICE_RTC             4    /* ���RTC */
#define  DIAG_DEVICE_CLOCK           5    /* ������໷ */
#define  DIAG_DEVICE_PCIE            6    /* ���PCIE */
#define  DIAG_DEVICE_PM              7    /* ���PM8001 */
#define  DIAG_DEVICE_FC              8    /* ���FC */
#define  DIAG_DEVICE_IBMC            9    /* ���IBMC */
#define  DIAG_DEVICE_GE              10   /* ���GE */

#define  DIAG_ECC_SWITCH_OPEN        1    /* ��ECC��⿪�� */
#define  DIAG_ECC_SWITCH_CLOSE       2    /* �ر�ECC��⿪�� */



/* ϵͳ�����Ϣ�궨�� */
#define  USP_SYS_NAME                     "ZXUSP"                    /* ϵͳ���� ,  Ĭ��ֵΪ ZXATCA_USP*/
#define  USP_VENDOR                       "ZTE"                           /* ��Ӧ��,     Ĭ��ֵΪ ZTE*/
#define  USP_MODEL                        "ZXUSP"                   /* ��Ʒ�ͺ�,   Ĭ��ֵΪ ZXATCA(USP) */
#define  USP_VERSION_NAME                 "V1.0"                          /* ��Ʒ�汾,   Ĭ��ֵΪ V1.0 */
#define  USP_SERIAL_NUM                   "2012-0001"                     /* ��Ʒ���к�, Ĭ��ֵΪ 2012-0001 */
#define  USP_SYS_STATE_SINGLE             "Single Running"                /* ϵͳ״̬, ��������, Ĭ��ֵΪSingle Running */
#define  USP_SYS_STATE_DUAL               "Dual Running"                  /* ϵͳ״̬, ˫������, Ĭ��ֵΪDual Running */
#define  USP_SYS_INFO_CFG_FILE            "/IDE0/uspcfg/sys_info.conf"    /* ϵͳ�����Ϣ�����ļ� */
#define  USP_SYS_INFO_CFG_SIZE            256                             /* ϵͳ�����Ϣ�����ļ���С */

/* ϵͳ�����Ϣ���Ⱥ궨�� */
#define  USP_SYSTEM_NAME_MAX_LEN          32       /* ϵͳ�����ַ�����󳤶� */
#define  USP_SYSTEM_VENDOR_MAX_LEN        32       /* ��Ӧ���ַ�����󳤶� */
#define  USP_SYSTEM_MODEL_MAX_LEN         32       /* ��Ʒ�ͺ��ַ�����󳤶� */
#define  USP_SYSTEM_VERSION_MAX_LEN       32       /* ��Ʒ�汾�ַ�����󳤶� */
#define  USP_SYSTEM_SERIAL_NUM_MAX_LEN    32       /* ��Ʒ���к��ַ�����󳤶� */
#define  USP_SYSTEM_STATE_MAX_LEN         32       /* ϵͳ״̬�ַ�����󳤶� */

/* ���������кź����ֳ��Ⱥ궨�� */
#define  USP_CONTROLLER_SERIAL_MAX_LEN    32       /* ���������к��ַ�����󳤶�*/
#define  USP_CONTROLLER_NAME_MAX_LEN      32       /* �����������ַ�����󳤶�*/

/* ��չ����Ϣ��غ궨��*/
#define  MAX_DISKNAMELEN       12   /*�����豸��������������*/

#define  MAX_BUSIDLEN          20   /*BUSID����*/
#define  MAX_FAN_NUM           4    /*�������֧�ָ���*/
#define  MAX_PS_NUM            2    /*��Դ���֧�ֵĸ���*/
#define  MAX_VOLT_NUM          6    /*��ѹ������֧�ֵĸ���*/
#define  MAX_TEMP_NUM          16   /*�¶ȴ�����֧�ֵĸ���*/
#define  MAX_DISK_NUM          28   /*����֧�ֵĸ���*/
#define  MAX_UP_SUB_ENCL       24   /*����֧�ּ�������*/
#define  SUBID_LEN             16   /*��չ��ID����*/

#define  WARNING_RECOVER       6    /*�澯�ָ�*/
#define  USP_EM_FAIL           5    /*�豸����*/
#define  OUT_FAILURE           1    /*�����������*/
#define  OUT_WARNING           2    /*������߾���*/
#define  UNDER_FAILURE         3    /*�����������*/
#define  UNDER_WARNING         4    /*������͸澯*/
#define  USP_EM_FAN            0x03 /*��������*/
#define  USP_EM_PS             0x02 /*��Դ����*/
#define  USP_EM_VOLT           0x12 /*��ѹ����*/
#define  USP_EM_TEMP           0x04 /*�¶�����*/
/*DM��غ궨��*/
#define  DM_SMART_SHORT_SELFTEST       0x1
#define  DM_SMART_LONG_SELFTEST        0x2
#define  SCS_DM_DISK_POWER_ACTIVE      0x00
#define  SCS_DM_DISK_POWER_IDLE        0x01
#define  SCS_DM_DISK_POWER_STANDBY     0x02
#define  SCS_DM_DISK_POWER_STOP_SLEEP  0x03


/*�ƻ���������ֵ*/
#define  MAX_SCHEDULE_NUM        32   //ϵͳ��֧�ֵ����ƻ�������Ŀ���ݶ�

#define  SCHED_IMMEDIATELY       0    // execute IMMEDIATELY
#define  SCHED_ONCE              1    // execute once
#define  SCHED_DAILY             2    // dayly task
#define  SCHED_WEEKLY            3    // weekly task
#define  SCHED_MONTHLY           4    // monthly task

/* �ƻ��������*/
#define  SCHED_DESC_DISK_SCAN    "Scan disk for bad blocks."

/* ������� */
#define  TYPE_VOL      0    /* Դ�� */
#define  TYPE_CVOL     1    /* ������ */
#define  TYPE_SVOL     2    /* ���վ� */

/* ��Ա���б�����---1:local,2:peer,3:dual */
#define  MD_LOCAL      1    /* ��Ա���ڱ��� */
#define  MD_PEER       2    /* ��Ա���ڶԶ� */
#define  MD_DUAL       3    /* ��Ա�����˶����� */

/* �˿�״̬ */
#define  PORT_STATUS_UP               (1)                /* Link״̬֮����OK */
#define  PORT_STATUS_DOWN             (0)                /* Link״̬֮���Ӵ��� */
#define  USP_PORT_STATUS_UP           PORT_STATUS_UP     /* Link״̬֮����OK */
#define  USP_PORT_STATUS_DOWN         PORT_STATUS_DOWN   /* Link״̬֮���Ӵ��� */


/*�˿�����״̬��Ϣ���*/

#define PORT_VALUE_UNKNOWN                 0                  /* δ֪��Ϣ*/

#define PORT_DUPLEX_FULL                   1                  /* ȫ˫��ģʽ*/
#define PORT_DUPLEX_HALF                   2                  /* ��ȫ˫��ģʽ*/

#define PORT_NEG_FORCE                     1                  /* ǿ��Э��ģʽ*/
#define PORT_NEG_AUTO                      2                  /* ��Э��ģʽ*/

#define PORT_SPEED_10M                     1                  /* 10M ���ʶ˿�*/
#define PORT_SPEED_100M                    2                  /* 100M ���ʶ˿�*/
#define PORT_SPEED_1G                      3                  /* 1G ���ʶ˿�*/
#define PORT_SPEED_10G                     4                  /* 10G ���ʶ˿�*/
#define PORT_SPEED_12G                     5                  /* 12G ���ʶ˿�*/
#define PORT_SPEED_16G                     6                  /* 16G ���ʶ˿�*/

#define PORT_STATE_INVALID                 0                  /* ��Ч�˿�״̬*/
#define PORT_STATE_UP                      1                  /* �˿�״̬ UP*/
#define PORT_STATE_DOWN                    2                  /* �˿�״̬ DOWN*/

#define PORT_MAC_DATA_SIZE                 6                  /* �˿�MAC ���ݶδ�С*/

/* snmp ��� */
#define MAX_SNMP_COMMUNITY_NAME_LEN  16
#define MAX_SNMP_NOTIFY_NUM   5


/* Զ�̾�����صĺ� */
#define MAX_MIRROR_IN_SYS             32             //ϵͳ�о���������Ŀ
#define MAX_MIRROR_CONNECT_NUM        8              //Զ�����ӵ������Ŀ
#define MAX_MIRROR_REMOTE_DISK_NUM    64             //Զ�̴��̵������Ŀ

#define MAX_MIRROR_REMOTE_DISK_NAME_LEN    16        //Զ�̴������Ƶ���󳤶�

#define REMOTE_DISK_UNUSED                   1       //Զ�̴���δ��ʹ��
#define REMOTE_DISK_USED                     2       //Զ�̴����ѱ�ʹ��

#define MIRROR_SYNCHRONOUS      0     /* ͬ������ */
#define MIRROR_ASYNCHRONOUS     1     /* �첽���� */

#define MIRROR_NOT_INIT_SYNC   NOT_INIT_SYNC     /* ������ʼͬ�� */
#define MIRROR_IS_INIT_SYNC    IS_INIT_SYNC      /* ����ʼͬ�� */

#define MIRROR_MANUAL_POLICY   MANUAL_POLICY     /* �ֶ��ָ����� */
#define MIRROR_AUTO_POLICY     AUTO_POLICY       /* �Զ��ָ����� */

/* FC ��صĺ� */
#define MAX_FC_WWN_LENGTH            8              //FC �˿�WWN(����WWPN��WWNN)����󳤶�
#define MAX_FC_PORT_NUM              4              //FC �˿ڵ������Ŀ
#define MAX_FC_PORT_ID_LENGTH        3              //FC �˿�ID ����󳤶�

#define FC_PORT_SPEED_INVALID        0                 /* ��Ч����ֵ */
#define FC_PORT_SPEED_2G             1                 /* 2G ����FC �˿� */
#define FC_PORT_SPEED_4G             2                 /* 4G ����FC �˿� */
#define FC_PORT_SPEED_8G             3                 /* 8G ����FC �˿�*/

#define FC_TOPO_INVALID              0                 /*  FC�˿����˽ṹ��Ч */
#define FC_TOPO_PRIVATE_LOOP         1                 /*  FC�˿�����Ϊ˽�л� */
#define FC_TOPO_PUBLIC_LOOP          2                 /*  FC�˿�����Ϊ���л� */
#define FC_TOPO_FABRIC               3                 /*  FC�˿�����ΪFabric  */
#define FC_TOPO_P2P                  4                 /*  FC�˿�����Ϊ��Ե� */

#define FC_PORT_INVALID              0                 /* FC �˿���Ч */
#define FC_PORT_VALID                1                 /* FC �˿���Ч */




/********************************************************************************************
*************************************MTS��غ궨��*******************************************
********************************************************************************************/

/* ���ӱ�ǩ����󳤶� */
#define  MAX_INFO_LABLE_LENGTH     29                 /* ͬBSP_SODA_SP_LSSP�к궨�� */

/*  �������� */
#define  MTS_HT                    0X00                  /* ���²��� */
#define  MTS_FT                    0X01                  /* ���ܲ��� */

/* EPLD�Լ� */
#define MTS_EPLDSELF_GOOD          0X02
#define MTS_EPLDSELF_ERROR         0X03


/* ���˵�����λ�źŲ��� */
#define  MTS_BOARD_POWER_ON        0X04                  /* ������λ */
#define  MTS_BOARD_POWER_OFF       0X05                  /* ���岻��λ */

/* �汾FLASH״̬ */
#define  MTS_FLASH_GOOD            0X06                  /* ������汾һ�� */
#define  MTS_FLASH_NOT_GOOD        0X07                  /* ��������汾��һ�� */

/* DDR�ڴ���Ϣ��� */
#define  MTS_BOARD_DDR_OK          0X08
#define  MTS_BOARD_DDR_ERROR       0X09

/* NVRAM��Ϣ��� */
#define  MTS_NVRAM_GOOD            0X0A
#define  MTS_NVRAM_ERROR           0X0B

/* CLOCK��Ϣ��� */
#define  MTS_CLOCK_GOOD            0X0C
#define  MTS_CLOCK_ERROR           0X0D

#define  MTS_MO_COMPA              0X0E                  /* �汾FLASH�Ƚ� */

/* �����źż�� */
#define  MTS_INPUTIO_GOOD          0X0F
#define  MTS_INPUTIO_ERROR         0X10

/* ����źż�� */
#define  MTS_OUTPUTIO_GOOD         0X11
#define  MTS_OUTPUTIO_ERROR        0X12

/* ��Դ�źŵƼ�� */
#define  MTS_POWER_LED_GOOD        0X13
#define  MTS_POWER_LED_ERROR       0X14



/* ����ʱ������ */
#define  MTS_CLOCK_25M             0X15
#define  MTS_CLOCK_33M             0X16
#define  MTS_CLOCK_75M             0X17
#define  MTS_CLOCK_100M            0X18
#define  MTS_CLOCK_125M            0X19
#define  MTS_CLOCK_133M            0X1A

/* ���Ź�ʹ��, ����, ι�� */
#define  MTS_DISABLE_WTG           0X1B
#define  MTS_ENABLE_WTG            0X1C
#define  MTS_FEDD_WTG              0X1D

/* RTCʱ�ӵ�· */
#define  MTS_RTC_GOOD              0X1E
#define  MTS_RTC_ERROR             0X1F

/* Link status */
#define  MTS_LINK_GOOD             0X20
#define  MTS_LINK_ERROR            0X21

/* pcie cacheͬ���źŲ��� */
#define  MTS_PCIE_CACHE_GOOD       0X22
#define  MTS_PCIE_CACHE_ERROR      0X23

/* ���GE�ڻ��ز��� */
#define  MTS_LOOP_BACK_GOOD        0X24
#define  MTS_LOOP_BACK_NOT_GOOD    0X25

/* 8005��λ�ɹ���־ */
#define  MTS_EZZF_RESET_GOOD       0X26
#define  MTS_EZZF_RESET_NOT_GOOD   0X27

/* minisas�ӿ��Ի����� */
#define  MTS_MINISAS_SPEED_GOOD      0X28
#define  MTS_MINISAS_SPEED_NOT_GOOD  0X29

/* ����SAS�ӿ��Ի����� */
#define  MTS_BBSAS_SPEED_GOOD        0X2A
#define  MTS_BBSAS_SPEED_NOT_GOOD    0X2B


/* TW1, TW2, TW3 �ź� */
#define  MTS_TWSIG_GOOD             0X2C
#define  MTS_TWSIG_NOT_GOOD         0X2D

/* Ӳ�̵�ƻ���� */
#define  MTS_HDDLIGHT_ON            0X2E
#define  MTS_HDDLIGHT_OFF           0X2F

/* PHY�Լ� */
#define  MTS_PHYSELF_GOOD             0X30
#define  MTS_PHYSELF_NOTGOOD          0X31

/* PCIE�Լ��� */
#define  MTS_PCIESELF_GOOD            0X32
#define  MTS_PCIESELF_NOTGOOD         0X33


/* PM8001�Լ��� */
#define  MTS_PMSELF_GOOD              0X34
#define  MTS_PMSELF_NOTGOOD           0X35

/* BBU�Լ��� */
#define  MTS_BBU_GOOD                 0X36
#define  MTS_BBU_NOTGOOD              0X37

/* ������״̬ */
#define MTS_SENOR_GOOD                0X38
#define MTS_SENOR_NOT_GOOD            0X39

#define MTS_8001           0X00
#define MTS_8005           0X01

/* �ڴ�����һ���ԣ��ڴ�������ȷ�� */
#define MTS_DDR_COHERENCE_CORRECT_GOOD    0X3A
#define MTS_DDR_COHERENCE_CORRECT_NOTGOOD 0X3B

/* �԰�״̬ */
#define MTS_PEER_POWERON                  0X3C
#define MTS_PEER_POWEROFF                 0X3D

/* SPR10�����  */
#define  MTS_SPR10_CLASSSISID      0   /* �̶�ֵΪ0 */

#define  CMT_CLASS_LEVEL           6

#define  MTS_WORKEPLD_VER          0
#define  MTS_BOOTEPLD_VER          1
#define  MTS_USP_VER               2
#define  MTS_EXP_VER               3
#define  MTS_ZBOOT_VER             4

#define   FC_MAX_PORT_NUM          (4)                  /* ���FC�˿��� */
#define   FC_MAC_LENGTH             (6)        /* MAC��ַ���ȶ��� */
#define   FC_WWN_LENGTH             (8)        /* FC World Wide���� */
#define   FC_PORT_ID_LENGTH         (3)                  /* FC�˿�ID�ĳ���(D_ID/S_ID) */

#define  MTS_82580          0
#define  MTS_I350          1

#define  MTS_CK420          0
#define  MTS_DB1900          1
#define  MTS_RTC          2
#define  MTS_CRYSTAL          3

#define  MTS_LED_CACHE          0
#define  MTS_LED_HOTSWAP                1
#define  MTS_LED_ALM          2
#define  MTS_LED_BBU          3
#define  MTS_LED_POWER     4
#define  MTS_LED_MINISAS_STA1    5
#define  MTS_LED_MINISAS_EXC1    6
#define  MTS_LED_MINISAS_STA2    7
#define  MTS_LED_MINISAS_EXC2    8
#define  MTS_LED_DISK    9

#define  MTS_LED_COLOR_GREEN         0
#define  MTS_LED_COLOR_BLUE                1
#define  MTS_LED_COLOR_RED          2
#define  MTS_LED_COLOR_ORANGE          3
#define  MTS_LED_MULTI_COLOR          4

#define  MTS_DISK_TYPE_ACTIVE        0
#define  MTS_DISK_TYPE_FAULT                1

#define  MTS_LED_OFF        0
#define  MTS_LED_ON               1

/********************************************************************************************
*************************************SIC��غ궨��*******************************************
********************************************************************************************/

#define MAX_SIC_FILE_NAME_LEN        64  /* �ļ�������(����·��) */

#define SIC_TASK_NAME                "system information collect"
#define TASK_DUAL_OWNER              (SWORD32)2

/* ����ID:
 * SCS_COLLECT_SYS_INFO: ϵͳ��Ϣ�ɼ�����
 * SCS_SIS_TASK_COMPLETE: ϵͳ��Ϣ�ɼ������������
 */
#define SCS_SIC_COLLECT_SYS_INFO     (BYTE)0x00
#define SCS_SIS_TASK_COMPLETE        (BYTE)0x01
#define SCS_SIC_GET_TASK_PROC        (BYTE)0x02
#define SCS_SIC_GET_TARBALL_INFO     (BYTE)0x03
#define SCS_SIC_SET_EMAIL_CFG        (BYTE)0x04
#define SCS_SIC_GET_EMAIL_CFG        (BYTE)0x05

/* ��ϵͳ���� */
#define SIC_SAS                      (WORD32)1
#define SIC_DM                       (WORD32)2
#define SIC_BLK                      (WORD32)3
#define SIC_STS                      (WORD32)4
#define SIC_CBS                      (WORD32)5
#define SIC_TCS                      (WORD32)6
/* SCS��ϵͳ�ڲ��ֶ�������ļ�: BLK��STS��DM���澯��־ */
#define SIC_SCS                      (WORD32)7    /* SCS�������� */
#define SIC_BLK_CFG                  (WORD32)8    /* BLK�������� */
#define SIC_STS_CFG                  (WORD32)9    /* STS�������� */
#define SIC_DM_CFG                   (WORD32)10   /* DM�������� */
#define SIC_WARN_LOG                 (WORD32)11   /* �澯��־ */
#define SIC_All_TASK                 (WORD32)12   /* ��̨����ƻ�������Ϣ */

/* ��Ϣ���� */
#define SIC_CMD                      (BYTE)1
#define SIC_GRAND                    (BYTE)2

/* ------------------------- FC��غ궨�� ------------------------------ */
#define FC_ALLOW_PLOGI_CNT               (256)
#define FC_RESERVE_GPN_FT_LIST_CNT        (5000)
#define  FC_ALLOW_LIRR_CNT                (1024)
#define FC_WWN_LENGTH               (8)


#define     ALARM_ADDINFO_UNION_MAX     (WORD16)256
#define     ALARM_PER_PACKET_MAX        (BYTE)20
#define     ALARM_NAME_LENGTH                   256         /* �澯���Ƴ��� */

#define     ALARM_TYPE_ALARM            (BYTE)1 /*�澯*/
#define     ALARM_TYPE_RESTORE          (BYTE)2 /*�ָ�*/

#define     ALARM_LASTPACKET_YES         (BYTE)1 /*���һ��*/
#define     ALARM_LASTPACKET_NO          (BYTE)0 /*�����һ��*/

#define     ALARM_LAN_EN            (BYTE)0 /*Ӣ��*/
#define     ALARM_LAN_CH            (BYTE)1 /*����*/

#endif
