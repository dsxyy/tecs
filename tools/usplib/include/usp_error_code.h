/*********************************************************************
* ��Ȩ���� (C)2010, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ� usp_error_code.h
* �ļ���ʶ�� �����ù���ƻ���
* ����ժҪ�� USP��Ŀ�����붨��
* ����˵���� ��
* ��ǰ�汾�� 1.0
* ��    �ߣ� liu.yong31@zte.com.cn
* ������ڣ� 2010-10-19
**********************************************************************/
#ifndef USP_ERROR_CODE_H
#define USP_ERROR_CODE_H

#include "usp_common_typedef.h"
#include "usp_capi.h"

/*******************************************************************
** ����USP��Ŀ��ϵͳ������
********************************************************************/
#define USP_ERROR_CODE_BASE           (0x01000000)
#define AMS_ERROR_CODE_BASE           (USP_ERROR_CODE_BASE | 0x00010000)
#define SCS_ERROR_CODE_BASE           (USP_ERROR_CODE_BASE | 0x00020000)
#define STS_ERROR_CODE_BASE           (USP_ERROR_CODE_BASE | 0x00030000)
#define CBS_ERROR_CODE_BASE           (USP_ERROR_CODE_BASE | 0x00040000)
#define VDS_ERROR_CODE_BASE           (USP_ERROR_CODE_BASE | 0x00050000)
#define DM_ERROR_CODE_BASE            (USP_ERROR_CODE_BASE | 0x00060000)
#define SAS_ERROR_CODE_BASE           (USP_ERROR_CODE_BASE | 0x00070000)
#define JBODC_ERROR_CODE_BASE         (USP_ERROR_CODE_BASE | 0x00080000)
#define ISCSI_ERROR_CODE_BASE         (USP_ERROR_CODE_BASE | 0x00090000)
#define MTS_ERROR_CODE_BASE           (USP_ERROR_CODE_BASE | 0x000F0000)
#define TCS_ERROR_CODE_BASE           ISCSI_ERROR_CODE_BASE

/*******************************************************************
** ����AMS��ϵͳ�ڲ�������
********************************************************************/
#define AMS_ERROR_CODE_XXXX          (AMS_ERROR_CODE_BASE | 0x00010000)

/*******************************************************************
** ����SCS��ϵͳ�ڲ�������
********************************************************************/
#define SCS_CAPI_ERROR_CODE_BASE            (SCS_ERROR_CODE_BASE | 0x00001000)
#define SCS_SMA_ERROR_CODE_BASE             (SCS_ERROR_CODE_BASE | 0x00002000)
#define SCS_RCC_ERROR_CODE_BASE             (SCS_ERROR_CODE_BASE | 0x00003000)
#define SCS_EVT_ERROR_CODE_BASE             (SCS_ERROR_CODE_BASE | 0x00004000)
#define SCS_LOG_ERROR_CODE_BASE             (SCS_ERROR_CODE_BASE | 0x00005000)
#define SCS_MON_ERROR_CODE_BASE             (SCS_ERROR_CODE_BASE | 0x00006000)
#define SCS_BOOT_ERROR_CODE_BASE            (SCS_ERROR_CODE_BASE | 0x00007000)
#define SCS_BANDIN_ERROR_CODE_BASE          (SCS_ERROR_CODE_BASE | 0x00008000)
#define SCS_MAIL_ERROR_CODE_BASE            (SCS_ERROR_CODE_BASE | 0x00009000)
#define SCS_CFG_FILE_ERROR_CODE_BASE        (SCS_ERROR_CODE_BASE | 0x0000A000)
#define SCS_VMM_ERROR_CODE_BASE             (SCS_ERROR_CODE_BASE | 0x0000B000)
#define SCS_DIAG_ERROR_CODE_BASE            (SCS_ERROR_CODE_BASE | 0x0000C000)

#define SCS_SIC_ERROR_CODE_BASE             (SCS_ERROR_CODE_BASE | 0x0000D000)

/* SCS  ͨ�õ�һЩ������Ķ��� */
#define E_SCS_MEMORY_MALLOC                                  (SCS_ERROR_CODE_BASE | 0x0000)            /* �ڴ�ռ�����ʧ�� */
#define E_SCS_PARAMETER_ERROR                                (SCS_ERROR_CODE_BASE | 0x0001)            /* ��������*/
#define E_SCS_SIG_PARAMETER_ERROR                            (SCS_ERROR_CODE_BASE | 0x0002)            /* �����µĲ�������*/
#define E_SCS_DUAL_PARAMETER_ERROR                           (SCS_ERROR_CODE_BASE | 0x0003)            /* ˫���µĲ�������*/
#define E_SCS_EXPANDER_NOTFIND                               (SCS_ERROR_CODE_BASE | 0x0004)            /* δ�ҵ�EXPADNER*/
#define E_SCS_SYSTEM_ABNORMAL                                (SCS_ERROR_CODE_BASE | 0x0005)            /* ϵͳ�쳣������ʧ��*/

/* BOOT ��Ҫ��������͹ر� */
#define E_SCS_BOOT_PEER_NOT_RUNNING                          (SCS_BOOT_ERROR_CODE_BASE | 0x0001)       /* �Զ˲�������״̬ */

/*CAPI*/

#define E_SCS_CAPI_CONNECT_CREATE                            (SCS_CAPI_ERROR_CODE_BASE | 0x0001)       /* ��������ʧ��*/
#define E_SCS_CAPI_CONNECT_FAIL                              (SCS_CAPI_ERROR_CODE_BASE | 0x0002)       /* Socket ���ӽ���ʧ�� */
#define E_SCS_CAPI_SEND_REQ_FAIL                             (SCS_CAPI_ERROR_CODE_BASE | 0x0003)       /* ������Ϣ����ʧ�� */
#define E_SCS_CAPI_RECV_RSP_ERROR                            (SCS_CAPI_ERROR_CODE_BASE | 0x0007)       /* ����Ӧ����Ϣ����*/
#define E_SCS_CAPI_RECV_MSG_TIMEOUT                          (SCS_CAPI_ERROR_CODE_BASE | 0x000B)       /* ����Ӧ��֡��ʱ*/
#define E_SCS_CAPI_ENDIAN_REVERSE_FAIL                       (SCS_CAPI_ERROR_CODE_BASE | 0x0004)       /* ��Ϣ�ֽ���ת��ʧ��*/
#define E_SCS_CAPI_SET_SVRSOCK_FAIL                          (SCS_CAPI_ERROR_CODE_BASE | 0x0005)       /* ����capi��洢�豸���ӵ�ipʧ��*/
#define E_SCS_CAPI_GET_SVRSOCK_FAIL                          (SCS_CAPI_ERROR_CODE_BASE | 0x0006)       /* ��ȡcapi��洢�豸���ӵ�ipʧ��*/
#define E_SCS_CAPI_PARAMETER_ERROR                           (SCS_CAPI_ERROR_CODE_BASE | 0x0008)       /* ��������*/

/*SMA*/
/************************SMA_ ERROR *******************/

//SMA�����쳣
#define E_SCS_SMA_MSG_GENERATE                                (SCS_SMA_ERROR_CODE_BASE | 0x0000)       /* ��Ϣ�ڵ�����ʧ�� */
#define E_SCS_SMA_SOCKET_CONNECT                              (SCS_SMA_ERROR_CODE_BASE | 0x0001)       /* �����߳��У�Socket���ӽ���ʧ��*/
#define E_SCS_SMA_THREAD_CREATE                               (SCS_SMA_ERROR_CODE_BASE | 0x0002)       /* SMA �̴߳���ʧ�� */
#define E_SCS_SMA_PEER_TIMEOUT                                (SCS_SMA_ERROR_CODE_BASE | 0x0003)       /* �Զ˴���ʱ */
#define E_SCS_SMA_SEND_REQ_TO_PEER                            (SCS_SMA_ERROR_CODE_BASE | 0x0004)       /* ����������Ϣ���Զ�ʧ�� */
#define E_SCS_SMA_PEER_NORUNNING                              (SCS_SMA_ERROR_CODE_BASE | 0x0005)       /* �Զ˿�������������������״̬ */
#define E_SCS_SMA_CMD_NO_EXIST                                (SCS_SMA_ERROR_CODE_BASE | 0x0006)       /* ����������� */
#define E_SCS_SMA_CMD_NO_OWNER                                (SCS_SMA_ERROR_CODE_BASE | 0x0007)       /* BLK���ò������ڷ�Owner �˿���������*/
#define E_SCS_SMA_SYSTEM_BUSY                                 (SCS_SMA_ERROR_CODE_BASE | 0x0008)       /* ϵͳæ������������*/
#define E_SCS_SMA_CMD_NO_SUPPORT                              (SCS_SMA_ERROR_CODE_BASE | 0x0009)       /* �����֧�� */
#define E_SCS_SMA_RCC_INSTABILITY                             (SCS_SMA_ERROR_CODE_BASE | 0x000A)       /* RCCͨ�����ȶ� */
#define E_SCS_SMA_MSG_MAXIMUM                                 (SCS_SMA_ERROR_CODE_BASE | 0x000B)       /* ������Ϣ����ֱ�ﳤ��*/

//SCS API�쳣
#define E_SCS_API_MAPGRP_LUN                                  (SCS_SMA_ERROR_CODE_BASE | 0x0200)       /* ��LUN  ���������κ�ӳ����*/
#define E_SCS_API_MAPGRP_MAX_NUM                              (SCS_SMA_ERROR_CODE_BASE | 0x0201)       /* ϵͳ��ӳ�������Ŀ�Ѿ��ﵽ���ֵ */
#define E_SCS_API_MAPGRP_EXIST                                (SCS_SMA_ERROR_CODE_BASE | 0x0202)       /* ӳ�����Ѿ����� */
#define E_SCS_API_MAPGRP_WRITE_CFG                            (SCS_SMA_ERROR_CODE_BASE | 0x0203)       /* ӳ������Ϣд�ļ�ʧ�� */
#define E_SCS_API_MAPGRP_LUN_OR_HOST                          (SCS_SMA_ERROR_CODE_BASE | 0x0204)       /* ӳ�����д���LUN����HOST��������ɾ�� */
#define E_SCS_API_MAPGRP_NOEXIST                              (SCS_SMA_ERROR_CODE_BASE | 0x0205)       /* ӳ���鲻���� */
#define E_SCS_API_MAPGRP_LUN_MAX_NUM                          (SCS_SMA_ERROR_CODE_BASE | 0x0207)       /* ӳ������LUN��Ŀ�ﵽ��� */
#define E_SCS_API_MAPGRP_VOL_EXIST                            (SCS_SMA_ERROR_CODE_BASE | 0x0208)       /* ӳ�����иþ��Ѿ����� */
#define E_SCS_API_MAPGRP_LUN_NOEXIST                          (SCS_SMA_ERROR_CODE_BASE | 0x0209)       /* ӳ������LUN������ */
#define E_SCS_API_MAPGRP_HOST_EXIST                           (SCS_SMA_ERROR_CODE_BASE | 0x020A)       /* ��Host�Ѿ�������ĳ��ӳ������ */
#define E_SCS_API_MAPGRP_HOST_MAX_NUM                         (SCS_SMA_ERROR_CODE_BASE | 0x020B)       /* ӳ������HOST��Ŀ�ﵽ���  */
#define E_SCS_API_MAPGRP_HOST_NOEXIST                         (SCS_SMA_ERROR_CODE_BASE | 0x020C)       /* ӳ������HOST������*/
#define E_SCS_API_MAPGRP_HOST                                 (SCS_SMA_ERROR_CODE_BASE | 0x020D)       /* ��HOST ���������κ�ӳ���� */
#define E_SCS_API_MAPGRP_ID_INVALID                           (SCS_SMA_ERROR_CODE_BASE | 0x020E)       /* ӳ����ID �Ƿ�*/
#define E_SCS_API_MAPGRP_NAME_EXIST                           (SCS_SMA_ERROR_CODE_BASE | 0x020F)       /* ӳ���������Ѿ�����*/
#define E_SCS_API_MAPGRP_MAX_LUN_EXIST                        (SCS_SMA_ERROR_CODE_BASE | 0x0210)       /*������������ӳ������Ŀ�Ѵ����ֵ*/

#define E_SCS_API_NAME_INVALID                                (SCS_SMA_ERROR_CODE_BASE | 0x0215)       /* �������Ʋ��Ϸ�*/
#define E_SCS_API_VOL_ADD_MAPGRP                              (SCS_SMA_ERROR_CODE_BASE | 0x0216)       /* �����ӳ����ʱ��ֹ���� */

#define E_SCS_API_LUN_EXIST                                   (SCS_SMA_ERROR_CODE_BASE | 0x0220)       /* LUN�Ѿ����� */
#define E_SCS_API_LUN_WRITE_CFG                               (SCS_SMA_ERROR_CODE_BASE | 0x0221)       /* LUN��Ϣд�����ļ�ʧ�� */
#define E_SCS_API_LUN_NOEXIST                                 (SCS_SMA_ERROR_CODE_BASE | 0x0222)       /* LUN������  */
#define E_SCS_API_LUN_GLOBAL_ID_INVALID                       (SCS_SMA_ERROR_CODE_BASE | 0x0223)       /* LUNȫ��ID ��Ч*/
#define E_SCS_API_LUN_ID_INVALID                              (SCS_SMA_ERROR_CODE_BASE | 0x0224)       /* LUN��ӳ�����е�ID ��Ч*/
#define E_SCS_API_LUN_ID_EXIST                                (SCS_SMA_ERROR_CODE_BASE | 0x0225)       /* LUN��ӳ�����е�ID �Ѿ�����*/
#define E_SCS_API_LUN_ADDED_TO_MAPGRP                         (SCS_SMA_ERROR_CODE_BASE | 0x0226)       /* LUN�Ѿ������뵽ӳ����*/
#define E_SCS_API_LUN_VOL_OFFLINE                             (SCS_SMA_ERROR_CODE_BASE | 0x0227)       /* LUN��Ӧ�ľ�����*/

#define E_SCS_API_VOL_MAP_ABNORMAL                            (SCS_SMA_ERROR_CODE_BASE | 0x0230)       /* VOL��LUNӳ���쳣(SCS ����������VOL������)  */
#define E_SCS_API_VOL_NUM_MAXIMUM                             (SCS_SMA_ERROR_CODE_BASE | 0x0231)       /* VOL(�������վ���¡��)�����ﵽ��� */
#define E_SCS_API_VOL_ADDED_TO_MAPGRP                         (SCS_SMA_ERROR_CODE_BASE | 0x0232)       /* Vol�Ѿ������뵽ӳ����*/
#define E_SCS_API_VOL_DEVICE_OPEN_FAILED                      (SCS_SMA_ERROR_CODE_BASE | 0x0233)       /* Vol�豸��ʧ��(����LUNʧ��)*/
#define E_SCS_API_VOL_DEVICE_CLOSE_FAILED                     (SCS_SMA_ERROR_CODE_BASE | 0x0234)       /* Vol�豸�ر�ʧ��(ɾ��LUNʧ��)*/
#define E_SCS_API_VOL_SINGLE_WRITE_BACK                       (SCS_SMA_ERROR_CODE_BASE | 0x0235)       /* ������д�ز��ԣ���֧�ִ���vol*/
#define E_SCS_API_VOL_MIRROR_OFF_WRITE_BACK                   (SCS_SMA_ERROR_CODE_BASE | 0x0236)       /* ������Ϊ�ؾ��񣬲������ٴ���д�ز��Ե�vol*/
#define E_SCS_API_VOL_WRITE_BACK_IN_SINGLE                    (SCS_SMA_ERROR_CODE_BASE | 0x0237)       /* ��������£�����д�ز���ʱ����֧�� */
#define E_SCS_API_VOL_MIRROR_OFF_WRITE_BACK_ERROR             (SCS_SMA_ERROR_CODE_BASE | 0x0238)       /* ������Ϊ�ؾ��񣬲���������д�ز��Ե�vol*/
#define E_SCS_API_VOL_VD_FREEZE                               (SCS_SMA_ERROR_CODE_BASE | 0x0239)       /* ������vd���ᣬ������vol����ӳ����*/
#define E_SCS_API_SET_VOL_CACHE_ATTR_FAILED                   (SCS_SMA_ERROR_CODE_BASE | 0x023A)       /* ���þ��cache����ʧ�� */
#define E_SCS_API_SEND_SNAPSHOT_AGENT_FAILED                  (SCS_SMA_ERROR_CODE_BASE | 0x023B)       /* ���Ϳ��մ���ʧ�� */
#define E_SCS_API_VOL_EXIST_ABNORMAL                          (SCS_SMA_ERROR_CODE_BASE | 0x023C)       /* ˫�ػ����о���󵥶˿ɼ�*/
#define E_SCS_API_FIND_AGENT_NODE_FAILED                      (SCS_SMA_ERROR_CODE_BASE | 0x023D)       /* ���ҿ��մ���ڵ�ʧ�� */
#define E_SCS_API_RCV_AGENT_RSP_FAILED                        (SCS_SMA_ERROR_CODE_BASE | 0x023E)       /* ���տ��մ�����Ӧʧ�� */
#define E_SCS_API_AGENT_EXECUTE_FAILED                        (SCS_SMA_ERROR_CODE_BASE | 0x023F)       /* ���մ���˲���ʧ�� */

#define E_SCS_API_OTHER_WRITE_CFG                             (SCS_SMA_ERROR_CODE_BASE | 0x0240)       /* ��������Ϣд�ļ�ʧ�� */
#define E_SCS_API_OTHER_READ_CFG                              (SCS_SMA_ERROR_CODE_BASE | 0x0241)       /* ���ļ�ʧ�� */
#define E_SCS_API_OTHER_GET_NET_INFO                          (SCS_SMA_ERROR_CODE_BASE | 0x0242)       /* ��ȡ�����豸��Ϣʧ��*/
#define E_SCS_API_OTHER_NET_IP_ERROR                          (SCS_SMA_ERROR_CODE_BASE | 0x0243)       /* IP ��ַ����*/
#define E_SCS_API_OTHER_NET_MASK_ERROR                        (SCS_SMA_ERROR_CODE_BASE | 0x0244)       /* ����������� */
#define E_SCS_API_PEERSTATE_NO_RUNNING                        (SCS_SMA_ERROR_CODE_BASE | 0x0245)       /* ����״̬������������*/
#define E_SCS_API_OTHER_NET_IP_EXIST                          (SCS_SMA_ERROR_CODE_BASE | 0x0246)       /*IP ��ϵͳ�Ѿ�����*/
#define E_SCS_API_OTHER_NET_MAC_EXIST                         (SCS_SMA_ERROR_CODE_BASE | 0x0247)       /*Mac ��ϵͳ���Ѿ�����*/
#define E_SCS_API_IP_OR_NETMASK_ERROR                         (SCS_SMA_ERROR_CODE_BASE | 0x0248)       /*IP��ַ����λ����������������*/
#define E_SCS_API_ROLETYPE_ERROR                              (SCS_SMA_ERROR_CODE_BASE | 0x0249)       /*�豸���ʹ���*/
#define E_SCS_API_DEVICEID_ERROR_FOR_ROLETYPE                 (SCS_SMA_ERROR_CODE_BASE | 0x024A)       /*�ý�ɫ�����£��豸��Ŵ���*/
#define E_SCS_API_GATEWAY_ERROR                               (SCS_SMA_ERROR_CODE_BASE | 0x024B)       /*���ش���*/
#define E_SCS_API_NETCFG_ERROR                                (SCS_SMA_ERROR_CODE_BASE | 0x024C)       /*����������Ϣ����Ϊ��*/
#define E_SCS_API_DEVICE_ERROR                                (SCS_SMA_ERROR_CODE_BASE | 0x024D)       /*�豸��Ŵ���*/
#define E_SCS_API_IP_MASK_BOTH_ZERO                           (SCS_SMA_ERROR_CODE_BASE | 0x024E)       /*ҵ��� ��IP���������ͬʱ����*/
#define E_SCS_API_IP_HOSTBITS_ERROR                           (SCS_SMA_ERROR_CODE_BASE | 0x024F)       /*IP��ַ������λ����ȫ0��ȫ1*/
#define E_SCS_API_NETWORKSEGMENT_ERROR                        (SCS_SMA_ERROR_CODE_BASE | 0x0250)       /* �������ô��� */
#define E_SCS_API_NETWORKSEGMENT_MANAGEMENT                   (SCS_SMA_ERROR_CODE_BASE | 0x0251)       /* ������������ */
#define E_SCS_API_NETWORKSEGMENT_BUSYNESS                     (SCS_SMA_ERROR_CODE_BASE | 0x0252)       /* ҵ���������� */

#define E_SCS_API_OTHER_MAC_INVALID                           (SCS_SMA_ERROR_CODE_BASE | 0x0253)       /*Mac �Ƿ�*/
#define E_SCS_API_OTHER_SET_MAC_BY_SRV                        (SCS_SMA_ERROR_CODE_BASE | 0x0254)       /*����soda��Mac ������ӿ�����Mac ʧ��*/
#define E_SCS_API_OTHER_WRITE_MAC_EEPROM_ERROR                (SCS_SMA_ERROR_CODE_BASE | 0x0255)       /*����soda��LSSP������ӿ�дMac ��EEPROMʧ��*/
#define E_SCS_API_OTHER_GET_PORT_MTU_MAX_VALUE                (SCS_SMA_ERROR_CODE_BASE | 0x0256)       /*��ȡ�˿ڵ�MTU ���ֵʧ��*/
#define E_SCS_API_OTHER_PORT_MTU_INVALID                      (SCS_SMA_ERROR_CODE_BASE | 0x0257)       /*�˿ڵ�MTUֵ�Ƿ�*/
#define E_SCS_API_OTHER_ROUTE_GATEWAY_ERROR                   (SCS_SMA_ERROR_CODE_BASE | 0x0258)       /* ·�ɵ����ز��Ϸ�*/
#define E_SCS_API_OTHER_ROUTE_NO_EXIST                        (SCS_SMA_ERROR_CODE_BASE | 0x0259)       /* ·�ɲ�����*/

#define E_SCS_API_OTHER_NET_MANA_IP_NULL                      (SCS_SMA_ERROR_CODE_BASE | 0x025A)       /* �����IP ��������Ϊ��*/
#define E_SCS_API_OTHER_NET_IP_RESERVED                       (SCS_SMA_ERROR_CODE_BASE | 0x025B)       /* Ԥ��IP(�Զ˹���ڵ�Ĭ��IP),����������*/

#define E_SCS_API_HDD_ABNORMAL                                (SCS_SMA_ERROR_CODE_BASE | 0x0260)       /* ϵͳ�У������쳣 */
#define E_SCS_API_HDD_NOEXIST                                 (SCS_SMA_ERROR_CODE_BASE | 0x0261)       /* ������Ϣ������*/
#define E_SCS_API_HDD_UNUSED                                  (SCS_SMA_ERROR_CODE_BASE | 0x0262)       /* ���̷ǿ���״̬ */
#define E_SCS_API_HDD_HOTSPARE                                (SCS_SMA_ERROR_CODE_BASE | 0x0263)       /* ���̷��ȱ�״̬  */
#define E_SCS_API_HDD_DATA                                    (SCS_SMA_ERROR_CODE_BASE | 0x0264)       /* ����Ϊ������*/
#define E_SCS_API_HDD_NO_LOCAL                                (SCS_SMA_ERROR_CODE_BASE | 0x0265)       /* ���̲��Ǳ�����*/
#define E_SCS_API_HDD_HEALTH_STATE                            (SCS_SMA_ERROR_CODE_BASE | 0x0266)       /* ���̽���״̬�쳣*/
#define E_SCS_API_HDD_NO_FOREIGN                              (SCS_SMA_ERROR_CODE_BASE | 0x0267)       /* ���̲���������*/
#define E_SCS_API_HDD_SET_POWER_MODE_ERROR                    (SCS_SMA_ERROR_CODE_BASE | 0x0268)       /* ���ô����ܺ�ģʽʧ��*/
#define E_SCS_API_HDD_CAPACITY                                (SCS_SMA_ERROR_CODE_BASE | 0x0269)       /* ��������С��VD��С��Ա������ */
#define E_SCS_API_HDD_TYPE_UNFIT                              (SCS_SMA_ERROR_CODE_BASE | 0x026A)       /* �������Ͳ����� */
#define E_SCS_API_HDD_DATA_IMPORT                             (SCS_SMA_ERROR_CODE_BASE | 0x026B)       /* ���������̲������룬��ͨ��VD����Ǩ�봦�� */
#define E_SCS_API_HDD_SCANNING                                (SCS_SMA_ERROR_CODE_BASE | 0x026C)       /* ������ɨ�裬��������д�������ת�� */
#define E_SCS_API_HDD_NOT_SCANNING                            (SCS_SMA_ERROR_CODE_BASE | 0x026D)       /* ����ɨ������δ���� */
#define E_SCS_API_ENOUGH_FIT_DISK                             (SCS_SMA_ERROR_CODE_BASE | 0x026E)       /* û���㹻�ĺ��ʴ����������������� */
#define E_SCS_API_HDD_ABNORMAL_SCAN                           (SCS_SMA_ERROR_CODE_BASE | 0x026F)       /* �����쳣����������ɨ��*/

#define E_SCS_API_PD_SCANNING                                 (SCS_SMA_ERROR_CODE_BASE | 0x0270)       /* ������ɨ�裬�������������� */
#define E_SCS_API_PD_SCAN_DATA_ERROR                          (SCS_SMA_ERROR_CODE_BASE | 0x0271)       /* �����̲�����ɨ��*/
#define E_SCS_API_PD_SCAN_UNKNOWN_ERROR                       (SCS_SMA_ERROR_CODE_BASE | 0x0272)       /* δ֪�̲�����ɨ��*/
#define E_SCS_API_HDD_NOT_READY                               (SCS_SMA_ERROR_CODE_BASE | 0x0273)       /* ����δ׼������*/
#define E_SCS_API_HDD_SLOT_ABNORMAL                           (SCS_SMA_ERROR_CODE_BASE | 0x0274)       /* ���̲�λ�쳣(˫����Ϣ��һ��)*/
#define E_SCS_API_DISK_SCAN                                   (SCS_SMA_ERROR_CODE_BASE | 0x0275)       /* ��������ɨ�����񣬲�����ִ�иò��� */

#define E_SCS_API_VD_NO_DEGRADE                               (SCS_SMA_ERROR_CODE_BASE | 0x0280)       /* VD�ǽ���״̬���������ؽ� */
#define E_SCS_API_VD_MEMBER_LIST_INCOMPATIBLE                 (SCS_SMA_ERROR_CODE_BASE | 0x0281)       /* VD���˿�������Ա���б�һ��Ҳ�����Ӽ���ϵ */
#define E_SCS_API_VD_MEMBER_LIST_NOT_SAME                     (SCS_SMA_ERROR_CODE_BASE | 0x0282)       /* VD���˿�������Ա���б�һ�� */
#define E_SCS_API_VD_STATE_NOT_SAME                           (SCS_SMA_ERROR_CODE_BASE | 0x0283)       /* VD���˿�����״̬����ͬ */
#define E_SCS_API_VD_LOCAL_NOEXIST                            (SCS_SMA_ERROR_CODE_BASE | 0x0284)       /* VD���˲����� */
#define E_SCS_API_VD_PEER_NOEXIST                             (SCS_SMA_ERROR_CODE_BASE | 0x0285)       /* VD�Զ˲����� */
#define E_SCS_API_VD_FAULT                                    (SCS_SMA_ERROR_CODE_BASE | 0x0286)       /* VDΪfault״̬���������޸Ļ������ */
#define E_SCS_API_VD_IS_RECING                                (SCS_SMA_ERROR_CODE_BASE | 0x0287)       /* VD���ڽ����ؽ��� */
#define E_SCS_API_VD_HOTSPARE_MAX                             (SCS_SMA_ERROR_CODE_BASE | 0x0288)       /* һ���������ĸ��ȱ��̲μ��ؽ�*/
#define E_SCS_API_VD_NOEXIST                                  (SCS_SMA_ERROR_CODE_BASE | 0x0289)       /* VD���˲����� */
#define E_SCS_API_VD_LEVEL                                    (SCS_SMA_ERROR_CODE_BASE | 0x028A)       /* VD����Ϊ0���������ؽ� */
#define E_SCS_API_VD_DELAY_RECOVERING                         (SCS_SMA_ERROR_CODE_BASE | 0x028B)       /* VD�ӳٻָ������������� */
#define E_SCS_NO_ENOUGH_UNUSED_DISKS                          (SCS_SMA_ERROR_CODE_BASE | 0x028C)       /* û���㹻�Ŀ����̣��޷�����������(�洢��) */
#define E_SCS_NO_ENOUGH_UNUSED_PD                             (SCS_SMA_ERROR_CODE_BASE | 0x028D)       /* û���㹻�Ŀ����� */
#define E_SCS_NO_ENOUGH_RIGHT_UNUSED                          (SCS_SMA_ERROR_CODE_BASE | 0x028E)       /* û���㹻���õĿ����� */

#define E_SCS_API_MIRROR_DISK_USED                            (SCS_SMA_ERROR_CODE_BASE | 0x0290)       /* Զ�˴����Ѿ���ʹ�� */
#define E_SCS_API_MIRROR_DISK_NOEXIST                         (SCS_SMA_ERROR_CODE_BASE | 0x0291)       /* Զ�˴��̲�����*/
#define E_SCS_API_MIRROR_CONNECT_NOEXIST                      (SCS_SMA_ERROR_CODE_BASE | 0x0292)       /* �������Ӳ�����*/
#define E_SCS_API_MIRROR_CONNECT_MAX                          (SCS_SMA_ERROR_CODE_BASE | 0x0293)       /* ����������Ŀ�ﵽ���ֵ*/
#define E_SCS_API_MIRROR_DISK_MAX                             (SCS_SMA_ERROR_CODE_BASE | 0x0294)       /* Զ�˴�����Ŀ�ﵽ���ֵ*/
#define E_SCS_API_NEXUS_MIRROR_EXIST                          (SCS_SMA_ERROR_CODE_BASE | 0x0295)       /* Զ���������о�����ڣ�������ɾ��*/
#define E_SCS_API_FC_CONNECT_EXIST                            (SCS_SMA_ERROR_CODE_BASE | 0x0296)       /* ���������Ѿ�����*/
#define E_SCS_API_FC_CONNECT_MAX_CTRL                         (SCS_SMA_ERROR_CODE_BASE | 0x0297)       /* �ÿ������ϵľ���������Ŀ�ﵽ���ֵ*/
#define E_SCS_API_MIRROR_DISK_ABNORMAL                        (SCS_SMA_ERROR_CODE_BASE | 0x0298)       /* Զ�˴���״̬�쳣*/

#define E_SCS_API_VOL_MAPPED                                  (SCS_SMA_ERROR_CODE_BASE | 0x02A0)       /* VOL�Ѿ���ӳ��,������ִ��ɾ������ */
#define E_SCS_API_VOL_NOT_GOOD                                (SCS_SMA_ERROR_CODE_BASE | 0x02A1)       /* VOL�ǡ�GOOD��״̬ */
#define E_SCS_API_VOL_NOT_NORMAL_VOL                          (SCS_SMA_ERROR_CODE_BASE | 0x02A2)       /* ����ͨ��*/
#define E_SCS_API_CVOL_INIT_RENAME_ERROR                      (SCS_SMA_ERROR_CODE_BASE | 0x02A3)       /* ��¡���ڳ�ʼͬ��״̬�У��������������˿�¡��*/
#define E_SCS_API_CVOL_SYNCING_RENAME_ERROR                   (SCS_SMA_ERROR_CODE_BASE | 0x02A4)       /* ��¡����ͬ���У��������������˿�¡��*/
#define E_SCS_API_CVOL_REVERSE_SYNCING_RENAME_ERROR           (SCS_SMA_ERROR_CODE_BASE | 0x02A5)       /* ��¡���ڷ�ͬ���У��������������˿�¡��*/
#define E_SCS_API_VOL_STATE_NOT_SAME                          (SCS_SMA_ERROR_CODE_BASE | 0x02A6)       /* Vol���˿�����״̬����ͬ */

#define E_SCS_API_SVOL_NOT_ACTIVE                             (SCS_SMA_ERROR_CODE_BASE | 0x02B0)       /* SVOL�ǡ�ACTIVE��״̬ */
#define E_SCS_API_CVOL_RELATION                               (SCS_SMA_ERROR_CODE_BASE | 0x02B1)       /* ��¡���ǹ���״̬ */
#define E_SCS_API_CVOL_VD_FREEZE                              (SCS_SMA_ERROR_CODE_BASE | 0x02B2)       /* ��¡������vd�Ƕ���״̬��������clone����ӳ���� */
#define E_SCS_API_SVOL_VD_FREEZE                              (SCS_SMA_ERROR_CODE_BASE | 0x02B3)       /* ���վ�����vd�Ƕ���״̬�������� svol ����ӳ���� */
#define E_SCS_API_CVOL_ADDED_TO_MAPGRP                        (SCS_SMA_ERROR_CODE_BASE | 0x02B4)       /* CVol�Ѿ������뵽ӳ����*/

#define E_SCS_API_VOL_VD_FOREIGN                              (SCS_SMA_ERROR_CODE_BASE | 0x02B5)       /* ����vd �еľ�������vol����ӳ����*/
#define E_SCS_API_CVOL_VD_FOREIGN                             (SCS_SMA_ERROR_CODE_BASE | 0x02B6)       /*  ����vd �еĿ�¡��������clone����ӳ���� */
#define E_SCS_API_SVOL_VD_FOREIGN                             (SCS_SMA_ERROR_CODE_BASE | 0x02B7)       /*  ����vd �еĿ��գ������� svol ����ӳ���� */


#define E_SCS_API_ISCSI_INIT_MAX_NUM                          (SCS_SMA_ERROR_CODE_BASE | 0x02C0)       /* Initiator ����Ŀ�������ֵ */
#define E_FC_SUBCARD_NOT_EXIST                                (SCS_SMA_ERROR_CODE_BASE | 0x02C1)       /* FC�ӿ�������*/

#define E_SCS_API_CACHE_STATUS_ERROR                          (SCS_SMA_ERROR_CODE_BASE | 0x02D0)       /* Cache ͳ��״̬���� */
#define E_SCS_API_IOLUN_STATUS_ERROR                          (SCS_SMA_ERROR_CODE_BASE | 0x02D1)       /* IOLun ͳ��״̬���� */
#define E_SCS_API_ISCSI_STATUS_ERROR                          (SCS_SMA_ERROR_CODE_BASE | 0x02D2)       /* ISCSI ͳ��״̬���� */
#define E_SCS_API_CACHE_STATUS_OFF                            (SCS_SMA_ERROR_CODE_BASE | 0x02D3)       /* Cache ͳ��״̬ΪOFF */
#define E_SCS_API_IOLUN_STATUS_OFF                            (SCS_SMA_ERROR_CODE_BASE | 0x02D4)       /* IOLun ͳ��״̬ΪOFF */
#define E_SCS_API_ISCSI_STATUS_OFF                            (SCS_SMA_ERROR_CODE_BASE | 0x02D5)       /* ISCSI ͳ��״̬ΪOFF */
#define E_SCS_API_ISCSI_NEWNAME_INVALID                       (SCS_SMA_ERROR_CODE_BASE | 0x02D6)       /* New ISCSI Name �Ƿ�*/
#define E_SCS_ISCSI_NEWNAME_SAME_PERR                         (SCS_SMA_ERROR_CODE_BASE | 0x02D7)       /* New ISCSI Name ��Զ�һ��*/

/* add by tao 2012-05-22 */
#define E_SCS_API_VOL_WRITE_BACK_FAN_FAULT                    (SCS_SMA_ERROR_CODE_BASE | 0x02D9)
#define E_SCS_API_VOL_WRITE_BACK_BBU_FAULT                    (SCS_SMA_ERROR_CODE_BASE | 0x02DA)
#define E_SCS_API_VOL_WRITE_BACK_POWER_FAULT                  (SCS_SMA_ERROR_CODE_BASE | 0x02DB)

/* �汾��������� added by wangjing 2011-08-22 */
#define E_SCS_API_SYNC_FILE_FAILED                            (SCS_VMM_ERROR_CODE_BASE | 0x0400)       /* ͬ���ļ����Զ�ʧ�� */
#define E_SCS_API_GET_UPLOAD_PROGRESS_FAILED                  (SCS_VMM_ERROR_CODE_BASE | 0x0401)       /* ��ѯ�ļ��ϴ�����ʧ�� */
#define E_SCS_API_CANCEL_UPLOAD_FAILED                        (SCS_VMM_ERROR_CODE_BASE | 0x0402)       /* ȡ���ļ��ϴ�ʧ�� */
#define E_SCS_API_VER_FILE_CHK_FAILED                         (SCS_VMM_ERROR_CODE_BASE | 0x0403)       /* �汾�ļ�У��ʧ�� */
#define E_SCS_API_UPDATE_FILE_FAILED                          (SCS_VMM_ERROR_CODE_BASE | 0x0404)       /* �����ļ��汾ʧ�� */
#define E_SCS_API_GET_VERSION_FAILED                          (SCS_VMM_ERROR_CODE_BASE | 0x0405)       /* ��ѯ�ļ��汾ʧ�� */
#define E_SCS_API_VERSION_FILE_NOT_EXIST                      (SCS_VMM_ERROR_CODE_BASE | 0x0406)       /* �汾�ļ������� */
#define E_SCS_API_FILE_PATH_NOT_EXIST                         (SCS_VMM_ERROR_CODE_BASE | 0x0407)       /* �ļ�·�������� */
#define E_SCS_API_NETWORK_NOT_REACHABLE                       (SCS_VMM_ERROR_CODE_BASE | 0x0408)       /* ���粻�ɴ� */
#define E_SCS_API_USER_PASSWORD_NOT_CORRECT                   (SCS_VMM_ERROR_CODE_BASE | 0x0409)       /* �û��������벻��ȷ */
#define E_SCS_API_INVALID_FILE_SIZE                           (SCS_VMM_ERROR_CODE_BASE | 0x040A)       /* �ļ���С���Ϸ� */
#define E_SCS_API_FILENAME_REPEATED                           (SCS_VMM_ERROR_CODE_BASE | 0x040B)       /* �ļ����ظ�������ȷ */
#define E_SCS_API_WRONG_VERTYPE                               (SCS_VMM_ERROR_CODE_BASE | 0x040C)       /* ������ļ����� */
#define E_SCS_API_FTP_GETFILEDATA_FAIL                        (SCS_VMM_ERROR_CODE_BASE | 0x040D)       /* FTP��ȡ����ʱʧ�� */
#define E_SCS_API_FTP_PUTFILEDATA_FAIL                        (SCS_VMM_ERROR_CODE_BASE | 0x040E)       /* FTP��ȡ����ʱʧ�� */
#define E_SCS_API_FILE_EXIST_DUAL                             (SCS_VMM_ERROR_CODE_BASE | 0x040F)       /* �ļ��ڱ��˺ͶԶ˾��Ѵ��� */
#define E_SCS_API_FILE_EXIST_LOCAL                            (SCS_VMM_ERROR_CODE_BASE | 0x0410)       /* �ļ��ڱ����Ѵ��� */
#define E_SCS_API_FILE_EXIST_PEER                             (SCS_VMM_ERROR_CODE_BASE | 0x0411)       /* �ļ��ڶԶ��Ѵ��� */
#define E_SCS_API_FILE_SIZE_TOO_BIG                           (SCS_VMM_ERROR_CODE_BASE | 0x0412)       /* �ļ����̫�� */
#define E_SCS_API_VERSION_NUMBER_NOT_MATCH                    (SCS_VMM_ERROR_CODE_BASE | 0x0413)       /* ���˰汾�Ų�ƥ�� */
#define E_SCS_API_FILE_SIZE_NOT_MATCH                         (SCS_VMM_ERROR_CODE_BASE | 0x0414)       /* �����ļ���С��ƥ�� */
#define E_SCS_API_UPLOAD_TASK_IS_RUNNING                      (SCS_VMM_ERROR_CODE_BASE | 0x0415)       /* �Ѿ���FTP���������� */
#define E_SCS_API_GET_DOWNLOAD_PROGRESS_FAILED                (SCS_VMM_ERROR_CODE_BASE | 0x0416)       /* ��ѯ�ļ����ؽ���ʧ�� */
#define E_SCS_API_GET_VERSION_BUSY                            (SCS_VMM_ERROR_CODE_BASE | 0x0417)       /* �汾�����У���ѯæ����*/
#define E_SCS_API_PEER_FILE_SIZE_TOO_BIG                      (SCS_VMM_ERROR_CODE_BASE | 0x0418)       /* �ļ���С̫�󣬳���FLASHʣ��ռ� */
#define E_SCS_API_VERSION_NETBOOT_REWRITE                     (SCS_VMM_ERROR_CODE_BASE | 0x0419)       /* ���ڰ汾��д�򱸷ݲ���---��Ϊ:ϵͳæ�����Ժ��� */
#define E_SCS_API_TOPO_ERROR                                  (SCS_VMM_ERROR_CODE_BASE | 0x0420)       /* ���˲����������������8005�̼�*/
#define E_SCS_API_WRONG_PM8005_TYPE                           (SCS_VMM_ERROR_CODE_BASE | 0x0421)       /* �����8005�汾���ͣ�ʹ����prokit�汾���� */

#define E_SCS_API_FILE_NAME_INVAILD                      (SCS_SMA_ERROR_CODE_BASE | 0x0460)          /* �ļ������Ϸ� */
#define E_SCS_API_FILE_INCONSISTENCY                     (SCS_SMA_ERROR_CODE_BASE | 0x0461)          /* ���˴����°汾�ļ����Ʋ�һ�� */
#define E_SCS_API_FILE_NAME_TOO_LONG                     (SCS_SMA_ERROR_CODE_BASE | 0x0462)          /* �ļ���̫�� */
#define E_SCS_API_SAMENAME_WITH_CURVER                   (SCS_SMA_ERROR_CODE_BASE | 0x0463)          /* �뵱ǰ�汾ͬ�� */
#define E_SCS_API_BGTASK_RUNNING                         (SCS_SMA_ERROR_CODE_BASE | 0x0464)          /* �к�̨��������,��ֹ�汾���� */
/*��̨����ͼƻ�������صĴ�����*/
#define   E_SCS_API_TASK_SET_NOT_ALLOWED                 (SCS_SMA_ERROR_CODE_BASE | 0x0420)           /*���������̨�����״̬��ͻ��������������*/
#define   E_SCS_API_TASK_SET_NOT_SUPPORTED               (SCS_SMA_ERROR_CODE_BASE | 0x0421)           /*��������������ò�֧��*/
#define   E_SCS_API_TASK_TYPE_NOT_EXIST                  (SCS_SMA_ERROR_CODE_BASE | 0x0422)           /* �������Ͳ�֧��*/
#define   E_SCS_API_TASK_EXIST                           (SCS_SMA_ERROR_CODE_BASE | 0x0423)           /* ���������Ѿ����� */
#define   E_SCS_API_TASK_NOT_EXIST                       (SCS_SMA_ERROR_CODE_BASE | 0x0424)           /* �������񲻴��� */
#define   E_SCS_API_TASK_NOT_RUNNING                     (SCS_SMA_ERROR_CODE_BASE | 0x0425)           /* ���������״̬ */
#define   E_SCS_API_SCHEDULE_OUTDATE                     (SCS_SMA_ERROR_CODE_BASE | 0x0426)           /* �ƻ�������� */
#define   E_SCS_API_SCHEDULE_TASKID_INVALID              (SCS_SMA_ERROR_CODE_BASE | 0x0427)           /* �ƻ����������Ч�������񲻴���*/
#define   E_SCS_API_SCHEDULE_MAX_NUM                     (SCS_SMA_ERROR_CODE_BASE | 0x0428)           /* �ƻ�����ﵽ�����Ŀ*/
#define   E_SCS_API_TASK_RUNNING_NUM_UP_LIMIT            (SCS_SMA_ERROR_CODE_BASE | 0x0429)           /* �������е������Ѵ�������� */
#define   E_SCS_API_SCHEDULE_EXIST                       (SCS_SMA_ERROR_CODE_BASE | 0x042B)           /* ����ƻ������Ѿ����� */
#define   E_SCS_API_TASK_MAX_NUM                         (SCS_SMA_ERROR_CODE_BASE | 0x042C)           /* ��̨����ﵽ�����Ŀ*/
#define   E_SCS_API_STOP_HOTSPARE_SCAN_FAIL              (SCS_SMA_ERROR_CODE_BASE | 0x042D)           /* ֹͣ�ȱ��̵�ɨ������ʧ��*/

/* �����ļ����뵼�������� add by tao 2011-08-15 */
#define E_SCS_API_FILE_EXIST                             (SCS_SMA_ERROR_CODE_BASE | 0x0500)
#define E_SCS_API_FILE_NO_EXIST                          (SCS_SMA_ERROR_CODE_BASE | 0x0501)
#define E_SCS_API_EXPORT_CFG_FILE_FAILED                 (SCS_SMA_ERROR_CODE_BASE | 0x0502)
#define E_SCS_API_IMPORT_CFG_FILE_FAILED                 (SCS_SMA_ERROR_CODE_BASE | 0x0503)
#define E_SCS_API_FILE_CP_FAILED                          (SCS_SMA_ERROR_CODE_BASE | 0x0504)


/* �����ļ������Ӳ������ add by tao 2011-08-15 */
#define E_SCS_CFG_FILE_CHECK_ERR                (SCS_CFG_FILE_ERROR_CODE_BASE | 0x0001)
#define E_SCS_CFG_FILE_OPEN_FAILED              (SCS_CFG_FILE_ERROR_CODE_BASE | 0x0002)

//SCS �û�& ��Ȩ�����쳣
//GENERAL_ERROR
#define E_SMA_PARAM_WRONG    (SCS_SMA_ERROR_CODE_BASE | 0x0600)      // param invalid
#define E_SMA_MALLOC_ERROR   (SCS_SMA_ERROR_CODE_BASE | 0x042A)

//USER_LOGIN
#define E_SMA_USER_WRONG          (SCS_SMA_ERROR_CODE_BASE | 0X0601) // username or pwd is incorrect
#define E_SMA_USER_LOGINED        (SCS_SMA_ERROR_CODE_BASE | 0X0602) // user is logined by other
#define E_SMA_USER_EXIST          (SCS_SMA_ERROR_CODE_BASE | 0X0603) // username or pwd is exist
#define E_SMA_USER_NEXIST         (SCS_SMA_ERROR_CODE_BASE | 0X0604)  // username or pwd is not exit
#define E_SMA_ADMIN               (SCS_SMA_ERROR_CODE_BASE | 0X0605) // supervisor level user has login
#define E_SMA_COMMON              (SCS_SMA_ERROR_CODE_BASE | 0X0606) // supervisor level user has login
//USER_AUTHOR
#define E_SMA_USER_AUTHOR         (SCS_SMA_ERROR_CODE_BASE | 0X0607)  // user does not have enough authority
#define E_SMA_AUTHEN              (SCS_SMA_ERROR_CODE_BASE | 0X0608)  // user has not been authenticated
#define E_SMA_PASSWD              (SCS_SMA_ERROR_CODE_BASE | 0X0609)  // Old password not match.
//USER_MGT
#define E_SMA_UNKNOW              (SCS_SMA_ERROR_CODE_BASE | 0X060A)  // unknow command or packet
#define E_SMA_COMM_USER_OVERFLOW  (SCS_SMA_ERROR_CODE_BASE | 0X060B)  // user number overflow
#define E_SMA_GUEST_USER_OVERFLOW (SCS_SMA_ERROR_CODE_BASE | 0X060C)  // user number overflow
#define E_SMA_USER_OVERFLOW       (SCS_SMA_ERROR_CODE_BASE | 0X060D)  // user number overflow
#define E_SMA_ADMIN_DEL           (SCS_SMA_ERROR_CODE_BASE | 0X060E)  // cannot delete admin
#define E_SMA_ONLINE_DEL          (SCS_SMA_ERROR_CODE_BASE | 0X060F)  // cannot delete online account
//USER_EMAIL
#define E_SMA_SMTP_BUSY           (SCS_SMA_ERROR_CODE_BASE | 0X0610)   //SMTP is used by other process. Try again later.
#define E_SMA_USER_NPWDMAIL       (SCS_SMA_ERROR_CODE_BASE | 0X0611)   // PWD MAIL not set
#define E_SMA_NSMTP               (SCS_SMA_ERROR_CODE_BASE | 0X0612)    // SMTP not set
#define E_SMA_FAIL_SEND           (SCS_SMA_ERROR_CODE_BASE | 0X0613)   //  Fail to send mail
//USER_CONNECT
#define E_SMA_CONN_OVERFLOW       (SCS_SMA_ERROR_CODE_BASE | 0X0614)   // Too many connects exit
#define E_SMA_CONN_NEXIT          (SCS_SMA_ERROR_CODE_BASE | 0X0615)   // the connect is not exit
#define E_SMA_CONN_SEARCH_INIT    (SCS_SMA_ERROR_CODE_BASE | 0X0616)  // capi search sma udp init failed
#define E_SMA_CONN_SEARCH_SEND    (SCS_SMA_ERROR_CODE_BASE | 0X0617)   // capi search sma udp send data failed

#define E_SMA_CONN_MSG_HANDLE_SOCKETFD  (SCS_SMA_ERROR_CODE_BASE | 0X0618)   // create Message Handle Socket fd failed
#define E_SMA_CONN_MSG_HANDLE_BLOCK     (SCS_SMA_ERROR_CODE_BASE | 0X0619)   // set Message Handle Socket fd block failed
#define E_SMA_CONN_MSG_HANDLE_SELECT    (SCS_SMA_ERROR_CODE_BASE | 0X061A)   // connect Message Handle process, select failed
#define E_SMA_CONN_MSG_HANDLE_TIMEOUT   (SCS_SMA_ERROR_CODE_BASE | 0X061B)  // connect Message Handle process, timeout
#define E_SMA_CONN_MSG_HANDLE_CONNECT   (SCS_SMA_ERROR_CODE_BASE | 0X061C)    // connect Message Handle process, connect failed
#define E_SMA_CONN_MSG_HANDLE_SEND      (SCS_SMA_ERROR_CODE_BASE | 0X061D)   // send Message to Messgage handle process failed
#define E_SMA_CONN_MSG_HANDLE_RECV      (SCS_SMA_ERROR_CODE_BASE | 0X061E)    // receive the message of Messgage handle process responsing failed
//USER_SESSION
#define E_SMA_SESSION_ATHOR             (SCS_SMA_ERROR_CODE_BASE | 0X061F)   // user does not have enough authority,not login
#define E_SMA_SESSION_NEXIST            (SCS_SMA_ERROR_CODE_BASE | 0X0620)    // Session not exist,maybe timeout
#define E_SMA_ADMIN_EXIST               (SCS_SMA_ERROR_CODE_BASE | 0X0621)    // can't add admin
#define E_SMA_SYS_NOT_FINISH_STARTUP    (SCS_SMA_ERROR_CODE_BASE | 0X0622)    // System have not finish Startup.
#define E_SMA_USER_NEWNAME_HAVE_EXIT    (SCS_SMA_ERROR_CODE_BASE | 0X0623)    // New name is illegal,it has exist in system.

//time

#define E_SCS_TIME_NTP_SERVER_ADDR_NULL          (SCS_SMA_ERROR_CODE_BASE | 0X0700)    // NTP server address is null
#define E_SCS_TIME_NTP_SERVER_ADDR_INVALID       (SCS_SMA_ERROR_CODE_BASE | 0X0701)    // NTP server address is invalid
#define E_SCS_TIME_GET_SYSTEM_TIME_ZONE_FAILED   (SCS_SMA_ERROR_CODE_BASE | 0X0702)    // get system time zone failed
#define E_SCS_TIME_SET_HARDWARE_CLOCK_FAILED     (SCS_SMA_ERROR_CODE_BASE | 0X0703)    // set hardware clock failed
#define E_SCS_TIME_THE_CITY_IS_INVALID           (SCS_SMA_ERROR_CODE_BASE | 0X0704)    // the city is invalid
#define E_SCS_TIME_NO_VALID_CONTINENT_NAME       (SCS_SMA_ERROR_CODE_BASE | 0X0705)    // did not input valid continent name
#define E_SCS_TIME_SET_TIME_ZONE_FAILED          (SCS_SMA_ERROR_CODE_BASE | 0X0706)    // set time zone by index failed
#define E_SCS_TIME_GET_NTP_SERVER_CONFIG_FAILED  (SCS_SMA_ERROR_CODE_BASE | 0X0707)    // get NTP server config failed
#define E_SCS_TIME_SET_NTP_CONFIG_FAILED         (SCS_SMA_ERROR_CODE_BASE | 0X0708)    // set NTP config failed
#define E_SCS_TIME_SET_AUTO_NTP_FAILED           (SCS_SMA_ERROR_CODE_BASE | 0X0709)    // set NTP date script failed
#define E_SCS_TIME_ENTER_TIME_INVALID            (SCS_SMA_ERROR_CODE_BASE | 0X070A)    // enter time is invalid
#define E_SCS_TIME_SYNC_TIME_BY_NTP_FAILED       (SCS_SMA_ERROR_CODE_BASE | 0X070B)    // synchronize system time by ntp failed
#define E_SCS_TIME_WRITE_CFG_REC_FAILED          (SCS_SMA_ERROR_CODE_BASE | 0x070C)    // write config record failed
#define E_SCS_TIME_READ_CFG_REC_FAILED           (SCS_SMA_ERROR_CODE_BASE | 0x070D)    // read config record failed
#define E_SCS_TIME_FORK_FAILED                   (SCS_SMA_ERROR_CODE_BASE | 0X070E)    // fork a new process failed

/* **************************LOG********************************** */
#define E_LOG_PARA_NULL                 (SCS_LOG_ERROR_CODE_BASE | 0X0101)    // point parameters null
#define E_LOG_PARAM_ILLEAG              (SCS_LOG_ERROR_CODE_BASE | 0X0102)    // parameters passed in or passed out illeagal
#define E_LOG_GET_FAILED                (SCS_LOG_ERROR_CODE_BASE | 0X0103)    // log inquiring failed
#define E_LOG_CLEAR_FAILED              (SCS_LOG_ERROR_CODE_BASE | 0X0104)    // log clear failed
#define E_LOG_EXPORT_FAILED             (SCS_LOG_ERROR_CODE_BASE | 0X0105)    // log exporting failed
#define E_LOG_EXPORT_ZERO               (SCS_LOG_ERROR_CODE_BASE | 0X0106)    // log number is 0
#define E_LOG_GETNUM_FAILED             (SCS_LOG_ERROR_CODE_BASE | 0X0107)    // log account obtain failed
#define E_LOG_SET_WMAIL_FAILED          (SCS_LOG_ERROR_CODE_BASE | 0X0108)    // log alarm mail config file setting failed
#define E_LOG_GET_WMAIL_FAILED          (SCS_LOG_ERROR_CODE_BASE | 0X0109)    // log alarm mail config file inquiring failed
#define E_LOG_GET_USERLOG_AUTH          (SCS_LOG_ERROR_CODE_BASE | 0X010a)
#define E_LOG_GET_USERLOG_USERINV       (SCS_LOG_ERROR_CODE_BASE | 0X010b)
#define E_LOG_PEER_ABSENT               (SCS_LOG_ERROR_CODE_BASE | 0X010c)
#define E_LOG_GET_ALARM_FAILED          (SCS_LOG_ERROR_CODE_BASE | 0X010d)    // alarm inquiring failed
#define E_LOG_DEL_ALARM_FAILED          (SCS_LOG_ERROR_CODE_BASE | 0X010e)    // alarm deleting failed

/* **************************MAIL********************************* */
#define E_MAIL_PARA_NULL                (SCS_MAIL_ERROR_CODE_BASE | 0X0100)    // point parameters null
#define E_MAIL_PARAM_ILLEAG             (SCS_MAIL_ERROR_CODE_BASE | 0X0101)    // parameters passed in or passed out illeagal
#define E_MAIL_LINK_HOST_FAILED         (SCS_MAIL_ERROR_CODE_BASE | 0X0102)    // link host failed
#define E_MAIL_LOGIN_FAILED             (SCS_MAIL_ERROR_CODE_BASE | 0X0103)    // login mail host failed
#define E_MAIL_CONFFILE_CREAT_FAILED    (SCS_MAIL_ERROR_CODE_BASE | 0X0104)    // mail config file create failed
#define E_MAIL_CONF_GET_FAILED          (SCS_MAIL_ERROR_CODE_BASE | 0X0105)    // mail config inquiring failed.
#define E_MAIL_CONF_SET_FAILED          (SCS_MAIL_ERROR_CODE_BASE | 0X0106)    // set mail config failed
#define E_MAIL_SET_ENABLE_FAILED        (SCS_MAIL_ERROR_CODE_BASE | 0X0107)    // enable mail-sending failed
#define E_MAIL_GET_ENABLE_FAILED        (SCS_MAIL_ERROR_CODE_BASE | 0X0108)    // inquiring mail-sending switch failed
#define E_MAIL_UNABLED_MAIL_ERR         (SCS_MAIL_ERROR_CODE_BASE | 0X0109)    // mail sending error: mail switch is not enabled
#define E_MAIL_CONFIG_VERIMAIL_FAILED   (SCS_MAIL_ERROR_CODE_BASE | 0X010a)    // verifying test mail sending error, check configuration
#define E_MAIL_SENDING_FAILED           (SCS_MAIL_ERROR_CODE_BASE | 0X010b)    // mail sending failed

/* *************************DIAG�������رյ���ص�********************************** */
#define E_DIAG_GET_INFO_FAILED          (SCS_DIAG_ERROR_CODE_BASE | 0X0100)    //��ȡ�����Ϣʧ��
#define E_DIAG_PARA_NULL                (SCS_DIAG_ERROR_CODE_BASE | 0X0101)    // point parameters null
#define E_DIAG_SODA_FAILED              (SCS_DIAG_ERROR_CODE_BASE | 0X0102)    // SODA�ӿڵ���ʧ��
#define E_DIAG_ECC_SWITCH_SET_FAILED    (SCS_DIAG_ERROR_CODE_BASE | 0X0104)    // set ecc check switch failed
#define E_DIAG_GET_INFO_NOTSUPPORT      (SCS_DIAG_ERROR_CODE_BASE | 0X0105)    //�豸��Ϣ��֧��
/* *************************MTS********************************** */
#define E_MTS_GET_INFO_FAILED           (MTS_ERROR_CODE_BASE | 0X0100)         // ��ȡ��Ϣʧ��

/****************************TRAP***********************************/
#define E_SMA_TRAP_OVERFLOW             (SCS_DIAG_ERROR_CODE_BASE | 0X0120)    // Trap Cfg Is Over Flow
#define E_SCS_TRAP_ID_NOT_EXIST         (SCS_DIAG_ERROR_CODE_BASE | 0X0121)    // Trap Cfg ID IS Not EXist


/**************************** SIC **********************************/

#define E_SIC_PARAM_NULL                (SCS_SIC_ERROR_CODE_BASE | 0x0300)
#define E_SIC_PARAM_ILLEAG              (SCS_SIC_ERROR_CODE_BASE | 0x0301)
#define E_SIC_COLLECT_SYS_INFO_FAILED   (SCS_SIC_ERROR_CODE_BASE | 0x0302)
#define E_SIC_GET_TASK_PROC_FAILED      (SCS_SIC_ERROR_CODE_BASE | 0x0303)
#define E_SIC_SET_EMAIL_ENABLE_FAILED   (SCS_SIC_ERROR_CODE_BASE | 0x0304)
#define E_SIC_GET_TARBALL_INFO_FAILED   (SCS_SIC_ERROR_CODE_BASE | 0x0305)
#define E_SIC_GET_EMAIL_ENABLE_FAILED   (SCS_SIC_ERROR_CODE_BASE | 0x0306)
#define E_SIC_TAR_BALL_NOT_EXIST        (SCS_SIC_ERROR_CODE_BASE | 0x0307)
#define E_SIC_TASK_IS_EXIST             (SCS_SIC_ERROR_CODE_BASE | 0x0308)
#define E_SIC_EXPORT_SYSINFO_TARBALL_FAILED  (SCS_SIC_ERROR_CODE_BASE | 0x0309)

/*******************************************************************
** ����STS��ϵͳ�ڲ�������
********************************************************************/
#define E_STS_SEQNO_NUM_ERR           (STS_ERROR_CODE_BASE | 0x0001)
#define E_STS_LUNDEV_NO_EXIST_ERR     (STS_ERROR_CODE_BASE | 0x0002)
#define E_STS_INITIATOR_NO_EXIST_ERR  (STS_ERROR_CODE_BASE | 0x0003)
#define E_STS_LUN_NO_EXIST_ERR        (STS_ERROR_CODE_BASE | 0x0004)
#define E_STS_LOCALLUN_NUM_ERR        (STS_ERROR_CODE_BASE | 0x0005)
#define E_STS_AHEAD_READ_SIZE_ERR     (STS_ERROR_CODE_BASE | 0x0006)
#define E_STS_CACHE_POLICY_ERR        (STS_ERROR_CODE_BASE | 0x0007)
#define E_STS_REFRESH_CACHE_ERR       (STS_ERROR_CODE_BASE | 0x0008)
#define E_STS_LUNDEV_IS_MAPPING_ERR   (STS_ERROR_CODE_BASE | 0x0009)
#define E_STS_OWNER_PARAM_ERR         (STS_ERROR_CODE_BASE | 0x0010)
#define E_STS_PREFER_PARAM_ERR        (STS_ERROR_CODE_BASE | 0x0011)
#define E_STS_DISCARD_CACHE_ERR        (STS_ERROR_CODE_BASE | 0x0012)
#define E_STS_REMOVE_MAP_TIMEOUT_ERR   (STS_ERROR_CODE_BASE | 0x0013)
#define E_STS_WPFLAG_PARAM_ERR         (STS_ERROR_CODE_BASE | 0x0014)
#define E_STS_ASYMMETRIC_ISTRANSITING_ERR  (STS_ERROR_CODE_BASE | 0x0015)

#define E_STS_STSCREATELUNDEV_ERR     (STS_ERROR_CODE_BASE | 0x0100)
#define E_STS_STSCREATELUNDEV_ALLOC_LUNDEV_FAILED_ERR  (STS_ERROR_CODE_BASE | 0x0101)
#define E_STS_STSCREATELUNDEV_OPEN_LUNDEV_FAILED_ERR   (STS_ERROR_CODE_BASE | 0x0102)
#define E_STS_STSCREATELUNDEV_SEQNO_EXIST_ERR          (STS_ERROR_CODE_BASE | 0x0103)
#define E_STS_STSCREATELUNDEV_LUNDEV_EXIST_ERR         (STS_ERROR_CODE_BASE | 0x0104)

#define E_STS_STSREMOVELUNDEV_ERR  (STS_ERROR_CODE_BASE | 0x0200)
#define E_STS_STSREMOVELUNDEV_BUSY_ERR  (STS_ERROR_CODE_BASE | 0x0201)

#define E_STS_STSLISTLUNDEV_ERR    (STS_ERROR_CODE_BASE | 0x0300)
#define E_STS_STSLISTLUNDEV_LUNDEV_REFCNT_ERR  (STS_ERROR_CODE_BASE | 0x0301)

#define E_STS_STSMODIFYLUNDEVOWNER_ERR  (STS_ERROR_CODE_BASE | 0x0400)

#define E_STS_STSCREATELUNMAP_ERR                         (STS_ERROR_CODE_BASE | 0x0500)
#define E_STS_STSCREATELUNMAP_ALLOC_INITIATOR_FAILED_ERR  (STS_ERROR_CODE_BASE | 0x0501)
#define E_STS_STSCREATELUNMAP_ALLOC_LUN_FAILED_ERR        (STS_ERROR_CODE_BASE | 0x0502)
#define E_STS_STSCREATELUNMAP_LUN_EXIST_ERR               (STS_ERROR_CODE_BASE | 0x0503)
#define E_STS_STSCREATELUNMAP_OPEN_LUNDEV_ERR             (STS_ERROR_CODE_BASE | 0x0504)

#define E_STS_STSREMOVELUNMAP_ERR  (STS_ERROR_CODE_BASE | 0x0600)

#define E_STS_STSLISTLUNMAP_ERR    (STS_ERROR_CODE_BASE | 0x0700)

#define E_STS_STSSTOPLUN_ERR       (STS_ERROR_CODE_BASE | 0x0800)

#define E_STS_STSSTARTLUN_ERR      (STS_ERROR_CODE_BASE | 0x0900)

#define E_STS_STSSETSTATSTATUS_ERR        (STS_ERROR_CODE_BASE | 0x0A00)
#define E_STS_STSSETSTATSTATUS_PARAM_ERR  (STS_ERROR_CODE_BASE | 0x0A01)

#define E_STS_STSGETSTATSTATUS_ERR        (STS_ERROR_CODE_BASE | 0x0B00)

#define E_STS_STSGETSTATDATA_ERR          (STS_ERROR_CODE_BASE | 0x0C00)

#define E_STS_STSPARAMINIT_ERR                (STS_ERROR_CODE_BASE | 0x0D00)
#define E_STS_STSPARAMINIT_CONTROLLER_ID_ERR  (STS_ERROR_CODE_BASE | 0x0D01)

#define E_STS_STSSETCONTGTINFO_ERR                          (STS_ERROR_CODE_BASE | 0x0E00)
#define E_STS_STSSETCONTGTINFO_WAKEUP_THREAD_FLAG_NULL_ERR  (STS_ERROR_CODE_BASE | 0x0E01)

#define E_STS_STSMODIFYLUNDEVERRORFLAG_ERR        (STS_ERROR_CODE_BASE | 0x0F00)
#define E_STS_STSMODIFYLUNDEVERRORFLAG_PARAM_ERR  (STS_ERROR_CODE_BASE | 0x0F01)

#define E_STS_STSLISTLUNSEQNOONSESSION_ERR        (STS_ERROR_CODE_BASE | 0x1000)
#define E_STS_STSLISTLUNSEQNOONSESSION_PARAM_ERR  (STS_ERROR_CODE_BASE | 0x1001)

#define E_STS_STSMODIFYLUNDEVPORTSTATUS_ERR       (STS_ERROR_CODE_BASE | 0x1100)
#define E_STS_STSMODIFYLUNDEVPORTSTATUS_PARAM_ERR (STS_ERROR_CODE_BASE | 0x1101)

#define E_STS_STSMODIFYLUNDEVPREFER_ERR         (STS_ERROR_CODE_BASE | 0x1200)

#define E_STS_STSSEARCHITONCONTORL_ERR          (STS_ERROR_CODE_BASE | 0x1300)
#define E_STS_STSSEARCHITONCONTORL_PARAM_ERR    (STS_ERROR_CODE_BASE | 0x1301)
#define E_STS_STSSEARCHITONCONTORL_IT_ERR       (STS_ERROR_CODE_BASE | 0x1302)

#define E_STS_STSWAKEUPSTPG_ERR                 (STS_ERROR_CODE_BASE | 0x1400)

#define E_STS_STSMODIFYLUNDEVRSTFLAG_ERR        (STS_ERROR_CODE_BASE | 0x1500)
#define E_STS_STSMODIFYLUNDEVRSTFLAG_PARAM_ERR  (STS_ERROR_CODE_BASE | 0x1501)

#define E_STS_STSMODIFYLUNDEVCACHE_ERR          (STS_ERROR_CODE_BASE | 0x1600)

#define E_STS_STSSETALLSTATSTATUS_ERR        (STS_ERROR_CODE_BASE | 0x1700)
#define E_STS_STSSETALLSTATSTATUS_PARAM_ERR  (STS_ERROR_CODE_BASE | 0x1701)

#define E_STS_STSREFRESHCACHE_ERR            (STS_ERROR_CODE_BASE | 0x1800)

#define E_STS_LUNDEV_WAIT_CMDS_TIMEOUT_ERR            (STS_ERROR_CODE_BASE | 0x1900)

/*******************************************************************
** ����CBS��ϵͳ�ڲ�������
********************************************************************/
#define E_CBS_OPEN_FILE_ERROR        (CBS_ERROR_CODE_BASE | 0x0001)
#define E_CBS_BAD_ADDR               (CBS_ERROR_CODE_BASE | 0x0002)
#define E_CBS_ERR_CACHE_SIZE         (CBS_ERROR_CODE_BASE | 0x0003)
#define E_CBS_ERR_DIRTY_RATIO        (CBS_ERROR_CODE_BASE | 0x0004)
#define E_CBS_ERR_PCIE_NO_LINK       (CBS_ERROR_CODE_BASE | 0x0005)
#define E_CBS_ERR_OPEN_MIRROR_PARM   (CBS_ERROR_CODE_BASE | 0x0006)
#define E_CBS_ERR_MONITOR_PARM       (CBS_ERROR_CODE_BASE | 0x0007)
#define E_CBS_MONITOR_STATUS_OFF     (CBS_ERROR_CODE_BASE | 0x0008)
#define E_CBS_ERR_FLUSH_LEVEL        (CBS_ERROR_CODE_BASE | 0x0009)
#define E_CBS_ERR_FLUSH_LOW_LEVEL    (CBS_ERROR_CODE_BASE | 0x000A)
#define E_CBS_ERR_TIME_INTERV        (CBS_ERROR_CODE_BASE | 0x000B)
#define E_CBS_ERR_MIRR_ENABLE        (CBS_ERROR_CODE_BASE | 0x000C)

/*******************************************************************
** ����VDS��ϵͳ�ڲ�������
********************************************************************/

/*
�������£�ͨ�õġ�vd��vol�Ĵ������
(VDS_ERROR_CODE_BASE|0x1001)��ʼ

���մ�
(VDS_ERROR_CODE_BASE|0x2001)��ʼ

��¡���
(VDS_ERROR_CODE_BASE|0x3001)��ʼ

���δ�
(VDS_ERROR_CODE_BASE|0x4001)��ʼ

Զ�̾����
(VDS_ERROR_CODE_BASE|0x5001)��ʼ

����ش�
(VDS_ERROR_CODE_BASE|0x6001)��ʼ

һ��������15������Ĺ��ܣ�Ӧ�ù����ˡ�ÿ����������4095�������룬Ӧ��Ҳ�����ˡ�

���������룬����Ҫ���Ǹ�ֵΪ���٣�ֻҪ��ȷ����ӵ���Ӧ���ܵĺ���Ϳ����ˣ���ס��Ҫ�Ӹ�Ӣ�Ķ��š�

Ϊ�˷��㿴�����ÿһ��������ÿ��10�������������ָ���

*/

#define E_VDS_NOERROR                       0
enum {
/* ͨ�õ� */
E_VDS_NULLP = (VDS_ERROR_CODE_BASE|0x1001),
E_VDS_PARAM_INVALID,
E_VDS_NO_VD_MOD,
E_VDS_NO_VOL_MOD,
E_VDS_VD_EXIST,
E_VDS_NO_VD,
E_VDS_NO_VOL,
E_VDS_OBJECT_EXIST,
E_VDS_VD_EXCEED,
E_VDS_VOL_EXCEED,
/* -----------10 -----------*/
E_VDS_VD_DEGRADE,
E_VDS_WRONG_CHUNKSIZE,
E_VDS_OPEN_FAIL,
E_VDS_MK_NOD_FAIL,
E_VDS_VOL_BUSY,
E_VDS_VOL_NO_READY,
E_VDS_VD_REJECTED_FREEZED,
E_VDS_VOL_DEV_BUSY,
E_VDS_VD_NOT_GOOD,
E_VDS_VOL_NOT_GOOD,
/* -----------20 -----------*/
E_VDS_ZERO_DISK_AREA_FAIL,
E_VDS_SOCKET_CREATE,
E_VDS_INIT_FAIL,
E_VDS_MSG_LENGTH,
E_VDS_VOL_TYPE,
E_VDS_CAPACITY,
E_VDS_NAME_INVALID,
E_VDS_RAID_LEVEL,
E_VDS_BAD_REBUILD_MODE,
E_VDS_VD_DEL_REJECT,
/* -----------30 -----------*/
E_VDS_PD_SET_INVALID,
E_VDS_USER_MEM_ALLOC,
E_VDS_KERNEL_MEM_ALLOC,
E_VDS_DEV_NOT_EXIST,
E_VDS_SYS_NOT_EXIST,
E_VDS_NO_PERMISSION,
E_VDS_DISK_TYPE_INVAL,
E_VDS_VD_NOSPC,
E_VDS_VD_SYS_REGISTER,
E_VDS_RAID_NO_MOD,
/* -----------40 -----------*/
E_VDS_RAID_RUN_FAIL,
E_VDS_PTHREAD_CREATE,
E_VDS_PTHREAD_DESTROY,
E_VDS_THREAD_NOTEXIST,
E_VDS_THREAD_EXIST,
E_VDS_THREAD_CREATE,
E_VDS_THREAD_SYNC,
E_VDS_DEVNO_TO_UUID,
E_VDS_UUID_TO_DEVNO,
E_VDS_WAIT_TIME_OUT,
/* -----------50 -----------*/
E_VDS_CDEV_REGISTER,
E_VDS_CDEV_INIT,
E_VDS_TYPE_INVALID,
E_VDS_NOT_CONSISTENT,
E_VDS_RVOL_NOT_EXIST,
E_VDS_SAME_VD,
E_VDS_WRONG_REBUILD_LEVEL,
E_VDS_DISK_RAID0,
E_VDS_DISK_RAID1,
E_VDS_DISK_RAID10,
/* -----------60 -----------*/
E_VDS_DISK_RAID5,
E_VDS_DISK_RAID6,
E_VDS_AUTOREBUILD_ALREADY_OFF,
E_VDS_VOL_DEL_REJECT,
E_VDS_VD_FAULT,
E_VDS_NOTSUPPORT,
E_VDS_NOT_LOCAL,
E_VDS_VD_RECING,
E_VDS_VD_CREATE_FAIL,
E_VDS_VOL_CREATE_FAIL,
/* -----------70 -----------*/
E_VDS_VD_DELETE_FAIL,
E_VDS_VOL_DELETE_FAIL,
E_VDS_B_VD_RECING,
E_VDS_OPEN_DIR_FAIL,
E_VDS_BVOL_RSYNC,
E_VDS_VD_NOT_RECING,
E_VDS_DEVNO_INVALID,
E_VDS_DEV_ALREADY_EXIST,
E_VDS_PEER_TIMEOUT,
E_VDS_PEER_FAIL,
/* -----------80 -----------*/
E_VDS_VD_NO_DEGRADE,
E_VDS_SPAREDISK_NUM,
E_VDS_GET_VD_INFO_BY_NAME_FAIL,
E_VDS_SET_VD_NAME_FAIL,
E_VDS_SET_VD_PRECTL_FAIL,
E_VDS_SET_VD_OWNERCTL_FAIL,
E_VDS_SET_VD_AUTO_REBUILD_FAIL,
E_VDS_VOL_RENAME_FAIL,
E_VDS_GET_VOL_INFO_FAIL,
E_VDS_GET_VOL_STAT_FAIL,
/* -----------90 -----------*/
E_VDS_CLEAN_VOL_STAT_FAIL,
E_VDS_SET_VOL_PRECTL_FAIL,
E_VDS_SET_VOL_OWNERCTL_FAIL,
E_VDS_VD_CAPACITY_NOT_ENOUTH,
E_VDS_WRITE_FILE_FAIL,
E_VDS_VD_SETFLAG_FAIL,
E_VDS_VD_LOAD_ERROR,





/* ���� */
E_VDS_NO_SNAP_MOD = (VDS_ERROR_CODE_BASE|0x2001),
E_VDS_NO_SNAP,
E_VDS_SNAP_EXCEED,
E_VDS_SNAP_EXIST,
E_VDS_SNAP_RUN_ERROR,
E_VDS_SNAP_CREATE_FAIL,
E_VDS_SNAP_DELETE_FAIL,
E_VDS_GET_SVOL_INFO_FAIL,
E_VDS_SET_SVOL_NAME_FAIL,
E_VDS_SET_RVOL_FAIL,
/* -----------10 -----------*/
E_VDS_SVOL_RESTORE_FAIL,
E_VDS_BVOL_NO_EXIST,
E_VDS_SVOL_INIT_FAIL,
E_VDS_RVOL_INIT_FAIL,
E_VDS_EXCEPTIONSTORE_CREATE_FAIL,
E_VDS_HASHTABLE_INIT_FAIL,
E_VDS_SVOL_INVALID,
E_VDS_SVOL_WRITE_STATE,
E_VDS_PENDING_EXCEPTIONS_RUNNING,
E_VDS_SVOL_REGISTERTHREAD_FAIL,
/* -----------20 -----------*/
E_VDS_PSDMIOCLIENT_CREATE_FAIL,
E_VDS_PS_AREA_ALLOC_FAIL,
E_VDS_PS_HEAD_AREA_ALLOC_FAIL,
E_VDS_PS_CALLBACKS_ALLOC_FAIL,
E_VDS_SVOL_RESTORING_STATE,
E_VDS_RVOL_NO_SPACE,
E_VDS_B_VD_NOTGOOD,
E_VDS_RVOL_ALREADY_EXIST,
E_VDS_RVOL_CREATE_FAIL,
E_VDS_NO_RVOL,
/* -----------30 -----------*/
E_VDS_RVOL_DELETE_FAIL,
E_VDS_RVOL_DEL_REJECT,
E_VDS_GET_RVOLINFO_FAIL,
E_VDS_RVOL_CAPACITY_PERCENTAGE,
E_VDS_RVOL_THR,
E_VDS_BVOL_RESTORING_STATE,


/* ��¡ */
E_VDS_CLONE_MAX = (VDS_ERROR_CODE_BASE|0x3001),
E_VDS_CLONE_UNBIND,
E_VDS_CLONE_UNFRACTURED,
E_VDS_CLONE_CREATE_FAIL,
E_VDS_CLONE_DELETE_FAIL,
E_VDS_CLONE_BUSY,
E_VDS_GET_CLONE_INFO_FAIL,
E_VDS_SET_CLONE_PRIORITY_FAIL,
E_VDS_SET_CLONE_PROTECT_FAIL,
E_VDS_SET_CLONE_NAME_FAIL,
/* -----------10 -----------*/
E_VDS_CLONE_FRACTURE_FAIL,
E_VDS_CLONE_SYNC_FAIL,
E_VDS_CLONE_REVERSE_SYNC_FAIL,
E_VDS_CLONE_FRACTURED,


/* ���� */
E_VDS_NOT_FOREIGN = (VDS_ERROR_CODE_BASE|0x4001),
E_VDS_IMMIGRATE_FAIL,
E_VDS_NOT_IMMIGRATE,
E_VDS_VD_REJECTED_IMMIGRATED,
E_VDS_VD_LOCALREJECTED_IMMIGRATED,
E_VDS_VD_FREEZE_FAIL,
E_VDS_VD_UNFREEZE_FAIL,
E_VDS_VD_NOT_FREEZED,
E_VDS_VD_RELATION_EXIST,

/* Զ�̾��� */
E_VDS_MIRROR_CREATE_FAIL = (VDS_ERROR_CODE_BASE|0x5001),
E_VDS_MIRROR_NOT_EXIST,
E_VDS_MIRROR_GETINFO_FAIL,
E_VDS_MIRROR_EXIST,
E_VDS_MIRROR_NO_SRC,
E_VDS_MIRROR_CAP,
E_VDS_MIRROR_SRC_HAS,
E_VDS_MIRROR_SETNAME_FAIL,
E_VDS_MIRROR_SETPOLICY_FAIL,
E_VDS_MIRROR_SETPRIORITY_FAIL,
/* -----------10 -----------*/
E_VDS_MIRROR_SETPERIOD_FAIL,
E_VDS_MIRROR_SRC_TYPE,
E_VDS_MIRROR_BUSY,
E_VDS_MIRROR_SYNCING,
E_VDS_MIRROR_DEST,
E_VDS_MIRROR_MAXNUM,
E_VDS_MIRROR_TYPE,


/* ����ء��� */
E_VDS_NO_POOL_MOD = (VDS_ERROR_CODE_BASE|0x6001),
E_VDS_POOL_EXCEED,
E_VDS_KERNEL_POOL_CREATE_FAIL,
E_VDS_KERNEL_POOL_DELETE_FAIL,
E_VDS_POOL_NOT_EXIST,
E_VDS_POOL_DEL_REJECT,
E_VDS_POOL_EXIST,
E_VDS_KERNEL_POOL_MODIFY_FAIL,
E_VDS_POOL_CANNOT_REMOVE_DISK,
E_VDS_KERNEL_POOL_GETINFO_FAIL,
/*--------------10---------------*/
E_VDS_POOL_ALARMTHRESHOLD_INVALID,
E_VDS_POOL_CAPACITY_NOT_ENOUTH,
E_VDS_VOL_EXCEED_IN_SYS,
E_VDS_POOL_STATE_INVALID,
E_VDS_POOL_REMOVE_DISK_INVALID,
E_VDS_VOL_CAPACITY_EXCEED,
};


/*******************************************************************
** ����DM��ϵͳ�ڲ�������
********************************************************************/

#define E_DM_INPUT_PARAM_INVALID              (DM_ERROR_CODE_BASE|0x1001)
#define E_DM_OPEN_DM_DIR_FAIL                 (DM_ERROR_CODE_BASE|0x1002)
#define E_DM_ALLOC_RESOURCE_FAIL              (DM_ERROR_CODE_BASE|0x1003)
#define E_DM_DISK_ABSENT                      (DM_ERROR_CODE_BASE|0x1004)
#define E_DM_SOCKET_FAIL                      (DM_ERROR_CODE_BASE|0x1005)

#define E_DM_GET_WARN_SWITCH_FAIL             (DM_ERROR_CODE_BASE|0x1101)
#define E_DM_GET_BBLK_LIMIT_FAIL              (DM_ERROR_CODE_BASE|0x1102)
#define E_DM_GET_RUNNING_STATE_FAIL           (DM_ERROR_CODE_BASE|0x1103)
#define E_DM_GET_CTRL_UUID_FAIL               (DM_ERROR_CODE_BASE|0x1104)
#define E_DM_GET_DEBUG_LEVEL_FAIL             (DM_ERROR_CODE_BASE|0x1105)
#define E_DM_SET_WARN_SWITCH_FAIL             (DM_ERROR_CODE_BASE|0x1201)
#define E_DM_SET_BBLK_LIMIT_FAIL              (DM_ERROR_CODE_BASE|0x1202)
#define E_DM_SET_RUNNING_STATE_FAIL           (DM_ERROR_CODE_BASE|0x1203)
#define E_DM_SET_CTRL_UUID_FAIL               (DM_ERROR_CODE_BASE|0x1204)
#define E_DM_SET_DEBUG_LEVEL_FAIL             (DM_ERROR_CODE_BASE|0x1205)

#define E_DM_GET_TYPE_FAIL                    (DM_ERROR_CODE_BASE|0x2101)
#define E_DM_GET_USEABLE_FAIL                 (DM_ERROR_CODE_BASE|0x2102)
#define E_DM_GET_ACCESSABLE_FAIL              (DM_ERROR_CODE_BASE|0x2103)
#define E_DM_GET_PD_SCAN_RATE_FAIL            (DM_ERROR_CODE_BASE|0x2104)
#define E_DM_GET_SMART_FAIL                   (DM_ERROR_CODE_BASE|0x2105)
#define E_DM_GET_GUID_FAIL                    (DM_ERROR_CODE_BASE|0x2111)
#define E_DM_GET_DEVNAME_FAIL                 (DM_ERROR_CODE_BASE|0x2112)
#define E_DM_GET_CAPACITY_FAIL                (DM_ERROR_CODE_BASE|0x2113)
#define E_DM_GET_PHYID_FAIL                   (DM_ERROR_CODE_BASE|0x2114)
#define E_DM_GET_JBODCADDR_FAIL               (DM_ERROR_CODE_BASE|0x2115)
#define E_DM_GET_BUSID_FAIL                   (DM_ERROR_CODE_BASE|0x2116)
#define E_DM_GET_SASADDR_FAIL                 (DM_ERROR_CODE_BASE|0x2121)
#define E_DM_GET_CACHE_FAIL                   (DM_ERROR_CODE_BASE|0x2122)
#define E_DM_GET_MODEL_FAIL                   (DM_ERROR_CODE_BASE|0x2123)
#define E_DM_GET_PHY_TYPE_FAIL                (DM_ERROR_CODE_BASE|0x2124)
#define E_DM_GET_FW_REV_FAIL                  (DM_ERROR_CODE_BASE|0x2125)
#define E_DM_GET_SERIAL_NO_FAIL               (DM_ERROR_CODE_BASE|0x2126)
#define E_DM_GET_STD_VERSION_FAIL             (DM_ERROR_CODE_BASE|0x2127)

#define E_DM_SET_PD_TYPE_FAIL                 (DM_ERROR_CODE_BASE|0x2201)
#define E_DM_SET_PD_USEABLE_FAIL              (DM_ERROR_CODE_BASE|0x2202)
#define E_DM_SET_PD_ACCESS_FAIL               (DM_ERROR_CODE_BASE|0x2203)
#define E_DM_SET_CONTROL_PD_SCAN_FAIL         (DM_ERROR_CODE_BASE|0x2204)
#define E_DM_SET_SMART_SELFTEST_FAIL          (DM_ERROR_CODE_BASE|0x2211)
#define E_DM_SET_CACHE_FAIL                   (DM_ERROR_CODE_BASE|0x2221)

/* add by kangzhenhua 2012.02.21 for IO statistics */
#define E_DM_GET_IO_STATISTICS_FAIL           (DM_ERROR_CODE_BASE|0x2301)
#define E_DM_SET_IO_STATISTICS_FAIL           (DM_ERROR_CODE_BASE|0x2302)
#define E_DM_RESET_IO_STATISTICS_FAIL         (DM_ERROR_CODE_BASE|0x2303)


#define E_DM_NO_RESOURCE         (DM_ERROR_CODE_BASE|0x3001)   /* ����Դ���ã����������ںˡ�ҳ��ʧ�� */
#define E_DM_INVALID_INPUT       (DM_ERROR_CODE_BASE|0x3002)   /* ��������Ƿ� */
#define E_DM_CMD_VALUE_ERROR     (DM_ERROR_CODE_BASE|0x3003)   /* �������зǷ��ֶ� */

#define E_DM_MDA_RW_ERROR        (DM_ERROR_CODE_BASE|0x3004)   /* Ԫ��������д���� */
#define E_DM_MDA_CHECKSUM_ERROR  (DM_ERROR_CODE_BASE|0x3005)   /* Ԫ����У��ʹ��� */
#define E_DM_MDA_SELRCT_ERROR    (DM_ERROR_CODE_BASE|0x3006)   /* Ԫ������ѡ����� */
#define E_DM_PD_IN_LIST          (DM_ERROR_CODE_BASE|0x3007)   /* ��ǰ�ڵ�����ڴ��������� */
#define E_DM_PD_NOT_IN_LIST      (DM_ERROR_CODE_BASE|0x3008)   /* ��ǰ�ڵ㲻�����ڴ��������� */
#define E_DM_BEYOND_MAX_PD_NUM   (DM_ERROR_CODE_BASE|0x3009)   /* ����ϵͳ���������� */
#define E_DM_START_ERROR         (DM_ERROR_CODE_BASE|0x300A)  /* ϵͳ����ʧ�� */
#define E_DM_STOP_ERROR          (DM_ERROR_CODE_BASE|0x300B)   /* ϵͳֹͣʧ�� */
#define E_DM_MODIFY_PD_SYSINFO_ERROR       (DM_ERROR_CODE_BASE|0x300C)   /* �޸�ϵͳ�������ʧ�� */

#define E_DM_GET_STD_VER_ERROR             (DM_ERROR_CODE_BASE|0x300D)   /*  ��ȡ��׼�汾��ʧ�� */
#define E_DM_GET_CAPACITY_VALUE_ERROR      (DM_ERROR_CODE_BASE|0x300E)   /*��ȡ��������ʧ�� */
#define E_DM_GET_FW_VALUE_ERROR            (DM_ERROR_CODE_BASE|0x301F)   /* ��ȡFW�汾��ʧ��*/
#define E_DM_GET_CATCH_VALUE_ERROR         (DM_ERROR_CODE_BASE|0x3010)   /* ��ȡ����״ֵ̬ʧ�� */
#define E_DM_GET_MODEL_VALUE_ERROR         (DM_ERROR_CODE_BASE|0x3011)   /* ��ȡMODEL��ʧ�� */
#define E_DM_GET_SERIAL_VALUE_ERROR        (DM_ERROR_CODE_BASE|0x3012)   /* ��ȡ�������к�ʧ�� */
#define E_DM_GET_SAS_VALUE_ERROR           (DM_ERROR_CODE_BASE|0x3013)   /* ��ȡ����SASֵʧ��*/

#define E_DM_SOCKET_INIT_ERROR             (DM_ERROR_CODE_BASE|0x3014)   /* SOCKET��ʼ��ʧ�� */
#define E_DM_SOCKET_SET_NUM_ERROR          (DM_ERROR_CODE_BASE|0x3015)   /* ���ÿͻ���SOCKET����ʧ�� */
#define E_DM_SOCKET_BIND_ERROR             (DM_ERROR_CODE_BASE|0x3016)   /* SOCKET��ʧ�� */
#define E_DM_SOCKET_SET_ERROR              (DM_ERROR_CODE_BASE|0x3017)   /* SOCKET��������ʧ�� */
#define E_DM_SOCKET_SEND_ERROR             (DM_ERROR_CODE_BASE|0x3018)   /* SOCKET���ݷ���ʧ�� */
#define E_DM_SOCKET_RECEV_ERROR            (DM_ERROR_CODE_BASE|0x3019)   /* SOCKET���ݽ���ʧ�� */
#define E_DM_PD_GET_SMART_ERROR            (DM_ERROR_CODE_BASE|0x301A)   /* ���SMART��Ϣʧ�� */
#define E_DM_SET_SMART_SELFTEST_ERROR      (DM_ERROR_CODE_BASE|0x301B)   /* SMART�Լ��ʧ�� */

#define E_DM_PD_GET_SMART_DURING_SCAN_ERROR            (DM_ERROR_CODE_BASE|0x301C)   /* ���SMART��Ϣʧ�� */
#define E_DM_SET_SMART_SELFTEST_DURING_SCAN_ERROR      (DM_ERROR_CODE_BASE|0x301D)   /* SMART�Լ��ʧ�� */
#define E_DM_SET_DISK_DATA_TYPE_DURING_SCAN_ERROR                (DM_ERROR_CODE_BASE|0x301E)
#define E_DM_SET_DISK_UNKNOWN_TYPE_DURING_SCAN_ERROR                (DM_ERROR_CODE_BASE|0x301F)
#define E_DM_SCAN_DATA_PD_FAIL                 (DM_ERROR_CODE_BASE|0x3020)
#define E_DM_SCAN_UNKNOWN_PD_FAIL                 (DM_ERROR_CODE_BASE|0x3021)
#define E_DM_SMART_DATA_PD_FAIL                 (DM_ERROR_CODE_BASE|0x3022)

/*******************************************************************
** ����SAS��ϵͳ�ڲ�������
********************************************************************/


/*******************************************************************
** ����JBODC��ϵͳ�ڲ�������
********************************************************************/

/*******************************************************************
** ����TCS��ϵͳ�ڲ�������
* TCS�������� 0x01090000 ��ʼ ����֧�ֵ� 0x0109FFFF
********************************************************************/
#define E_TCS_COPY_FROM_USER_FAIL                   (TCS_ERROR_CODE_BASE|0x0000) /* 0x01090000,17367040 */
#define E_TCS_COPY_TO_USER_FAIL                     (TCS_ERROR_CODE_BASE|0x0001) /* 0x01090001,17367041 */
#define E_TCS_KMALLOC_FAIL                          (TCS_ERROR_CODE_BASE|0x0002) /* 0x01090002,17367042 */
#define E_TCS_DOWN_SEM_FAIL                         (TCS_ERROR_CODE_BASE|0x0003) /* 0x01090003,17367043 */

#define E_TCS_TGT_NOT_EXIST                         (TCS_ERROR_CODE_BASE|0x0004) /* 0x01090004,17367044 */
#define E_TCS_TGT_ALREADY_EXIST                     (TCS_ERROR_CODE_BASE|0x0005) /* 0x01090005,17367045 */
#define E_TCS_TGT_SESSION_BUSY                      (TCS_ERROR_CODE_BASE|0x0006) /* 0x01090006,17367046 */
#define E_TCS_MAX_TGT_NUM_REACHED                   (TCS_ERROR_CODE_BASE|0x0007) /* 0x01090007,17367047 */

#define E_TCS_SESSION_NOT_EXIST                     (TCS_ERROR_CODE_BASE|0x0008) /* 0x01090008,17367048 */
#define E_TCS_SESSION_ALREADY_EXIST                 (TCS_ERROR_CODE_BASE|0x0009) /* 0x01090009,17367049 */
#define E_TCS_SESSION_CONN_BUSY                     (TCS_ERROR_CODE_BASE|0x000A) /* 0x0109000A,17367050 */

#define E_TCS_INVAL_IOCTL_CMD                       (TCS_ERROR_CODE_BASE|0x000B) /* 0x0109000B,17367051 */
#define E_TCS_PORTGRP_NOT_EXIST                     (TCS_ERROR_CODE_BASE|0x0017)
#define E_TCS_ALL_PORTGRP_BE_USED                   (TCS_ERROR_CODE_BASE|0x0018)

#define E_TCS_DIR_OPEN_FAIL                         (TCS_ERROR_CODE_BASE|0x002A)      /* 0x109002a,17367082 */
#define E_TCS_FILE_OPEN_FAIL                        (TCS_ERROR_CODE_BASE|0x002B)      /* 0x109002b,17367083 */
#define E_TCS_SYSCALL_FAIL                          (TCS_ERROR_CODE_BASE|0x002C)      /* 0x109002c,17367084 */
#define E_TCS_SYSPROC_FAIL                          (TCS_ERROR_CODE_BASE|0x002D)      /* 0x109002d,17367085 */
#define E_TCS_PARAM_NULL                            (TCS_ERROR_CODE_BASE|0x002E)      /* 0x109002e,17367086 */
#define E_TCS_PORTNUM_ERR                           (TCS_ERROR_CODE_BASE|0x002F)      /* 0x109002f,17367087 */
#define E_TCS_CFG_PARAM_NULL                        (TCS_ERROR_CODE_BASE|0x0030)      /* 0x1090030,17367088 */
#define E_TCS_CFG_PORTNUM_ERR                       (TCS_ERROR_CODE_BASE|0x0031)      /* 0x1090031,17367089 */
#define E_TCS_CFG_SYS_PROC_FAIL                     (TCS_ERROR_CODE_BASE|0x0032)      /* 0x1090032,17367090 */
#define E_TCS_CFG_INFO_DIFFER                       (TCS_ERROR_CODE_BASE|0x0033)      /* 0x1090033,17367091 */
#define E_TCS_CFG_IP_FAIL                           (TCS_ERROR_CODE_BASE|0x0034)      /* 0x1090034,17367092 */
#define E_TCS_CFG_ARP_FAIL                          (TCS_ERROR_CODE_BASE|0x0035)      /* 0x1090035,17367093 */
#define E_TCS_CFG_MAC_FAIL                          (TCS_ERROR_CODE_BASE|0x0036)      /* 0x1090036,17367094 */
#define E_TCS_CFG_MTU_FAIL                          (TCS_ERROR_CODE_BASE|0x0037)      /* 0x1090037,17367095 */
#define E_TCS_CFG_ROUTE_FAIL                        (TCS_ERROR_CODE_BASE|0x0038)      /* 0x1090038,17367096 */
#define E_TCS_CFG_ISCSI_FAIL                        (TCS_ERROR_CODE_BASE|0x0039)      /* 0x1090039,17367097 */
#define E_TCS_CFG_IPADD_FAIL                        (TCS_ERROR_CODE_BASE|0x003A)      /* 0x109003a,17367098 */
#define E_TCS_CFG_IPDEL_FAIL                        (TCS_ERROR_CODE_BASE|0x003B)      /* 0x109003b,17367099 */
#define E_TCS_CFG_ROUTE_ADD_FAIL                    (TCS_ERROR_CODE_BASE|0x003C)      /* 0x109003c,17367100 */
#define E_TCS_CFG_ROUTE_DEL_FAIL                    (TCS_ERROR_CODE_BASE|0x003D)      /* 0x109003d,17367101 */
#define E_TCS_CFG_IPADDR_ZERO                       (TCS_ERROR_CODE_BASE|0x003E)      /* 0x109003e,17367102 */
#define E_TCS_CFG_IPMASK_ZERO                       (TCS_ERROR_CODE_BASE|0x003F)      /* 0x109003f,17367103 */
#define E_TCS_CFG_IPADDR_IPMASK_ONE_ZERO            (TCS_ERROR_CODE_BASE|0x0040)      /* 0x1090040,17367104 */
#define E_TCS_CFG_IPMASK_FORMAT_ERR                 (TCS_ERROR_CODE_BASE|0x0041)      /* 0x1090041,17367105 */
#define E_TCS_CFG_IPMASK_LEN_ERR                    (TCS_ERROR_CODE_BASE|0x0042)      /* 0x1090042,17367106 */
#define E_TCS_CFG_IPADDR_AREA_ERR                   (TCS_ERROR_CODE_BASE|0x0043)      /* 0x1090043,17367107 */
#define E_TCS_CFG_PORTIP_EXIST                      (TCS_ERROR_CODE_BASE|0x0044)      /* 0x1090044,17367108 */
#define E_TCS_CFG_PORTIP_SAMEDOMAIN                 (TCS_ERROR_CODE_BASE|0x0045)      /* 0x1090045,17367109 */
#define E_TCS_CFG_IPADDR_NOMATCH                    (TCS_ERROR_CODE_BASE|0x0046)      /* 0x1090046,17367110 */
#define E_TCS_CFG_IPADDR_NOCFG                      (TCS_ERROR_CODE_BASE|0x0047)      /* 0x1090047,17367111 */
#define E_TCS_CFG_ROUTE_EXIST                       (TCS_ERROR_CODE_BASE|0x0048)      /* 0x1090048,17367112 */
#define E_TCS_CFG_ROUTE_NOT_EXIST                   (TCS_ERROR_CODE_BASE|0x0049)      /* 0x1090049,17367113 */
#define E_TCS_CFG_ROUTE_CLASH                       (TCS_ERROR_CODE_BASE|0x004A)      /* 0x109004a,17367114 */
#define E_TCS_CFG_ROUTE_NEXHOP_NODIRECT             (TCS_ERROR_CODE_BASE|0x004B)      /* 0x109004b,17367115 */
#define E_TCS_CFG_ROUTE_STATICNUM_MAX               (TCS_ERROR_CODE_BASE|0x004C)      /* 0x109004c,17367116 */
#define E_TCS_CFG_ROUTE_STATICNUM_ZERO              (TCS_ERROR_CODE_BASE|0x004D)      /* 0x109004d,17367117 */
#define E_TCS_CFG_MTU_AREA_ERR                      (TCS_ERROR_CODE_BASE|0x004E)      /* 0x109004e,17367118 */
#define E_TCS_CFG_MAC_FORMAT_ERR                    (TCS_ERROR_CODE_BASE|0x004F)      /* 0x109004f,17367119 */
#define E_TCS_CFG_MAC_REPEAT                        (TCS_ERROR_CODE_BASE|0x0050)      /* 0x1090050,17367120 */
#define E_TCS_CFG_ROUTE_MASK_ERR                    (TCS_ERROR_CODE_BASE|0x0051)      /* 0x1090051,17367121 */
#define E_TCS_CFG_HAVE_STATICROUTE                  (TCS_ERROR_CODE_BASE|0x0052)      /* 0x1090052,17367122 */
#define E_TCS_TEST_LENGTH_ERR                       (TCS_ERROR_CODE_BASE|0x0053)      /* 0x1090053,17367123 */
#define E_TCS_TEST_INTER_ERR                        (TCS_ERROR_CODE_BASE|0x0054)      /* 0x1090054,17367124 */
#define E_TCS_TEST_NUM_ERR                          (TCS_ERROR_CODE_BASE|0x0055)      /* 0x1090055,17367125 */
#define E_TCS_CFG_CTRLID_ERR                        (TCS_ERROR_CODE_BASE|0x0056)      /* 0x1090055,17367126 */
#define E_TCS_PM_ID                                 (TCS_ERROR_CODE_BASE|0x0100)      /* 0x1090100,17367296 */
#define E_TCS_PM_PORTSTAT_FUNCFLAG                  (TCS_ERROR_CODE_BASE|0x0101)      /* 0x1090101,17367297 */
#define E_TCS_PM_PORTSTAT_CTRLFLAG                  (TCS_ERROR_CODE_BASE|0x0102)      /* 0x1090102,17367298 */
#define E_TCS_PM_PORTSTAT_MEMSIZE                   (TCS_ERROR_CODE_BASE|0x0103)      /* 0x1090103,17367299 */



typedef struct
{
    SWORD32 sdwErrID;       /* the error id */
    const CHAR *pcErrTag;   /* the tag of the error */
    const CHAR *pcErrDes;   /* the description of the error */
    const CHAR *pcErrDesCH;
}_PACKED_1_ T_ErrorPack;

#define USE_COMMON_ERROR_PACK               /*add by maxiaopeng 04.21*/
#ifdef USE_COMMON_ERROR_PACK

static const  T_ErrorPack sg_tCommonError[] =
    {
        //  { E_GENERIC_REPEAT,                 "E_GENERIC_REPEAT",                 "Repeat setting the system value." },                   //����

        /*******************************AMS ��ϵͳ************************************/
        {AMS_ERROR_CODE_XXXX,"AMS_ERROR_CODE_XXXX","Description of the error code.","������������Ϣ��"},

        /*******************************SCS ��ϵͳ************************************/
        {E_SCS_MEMORY_MALLOC,           "E_SCS_MEMORY_MALLOC",        "Failed to malloc memory space.", "�ڴ�ռ�����ʧ��"},
        {E_SCS_PARAMETER_ERROR,         "E_SCS_PARAMETER_ERROR",      "Parameter error.","��������"},
        {E_SCS_EXPANDER_NOTFIND,         "E_SCS_EXPANDER_NOTFIND",      "EXPANDER not find.","δ�ҵ���Ӧ��EXPANDER"},
        {E_SCS_SIG_PARAMETER_ERROR,     "E_SCS_SIG_PARAMETER_ERROR",  "Single ctrl  on running, Parameter error.","�����µĲ�������"},
        {E_SCS_DUAL_PARAMETER_ERROR,    "E_SCS_DUAL_PARAMETER_ERROR", "Dual   ctrls on running, Parameter error.","˫���µĲ�������"},
        {E_SCS_BOOT_PEER_NOT_RUNNING,   "E_SCS_BOOT_PEER_NOT_RUNNING","Peer is not running.","�Զ˲�������״̬"},
        {E_SCS_SYSTEM_ABNORMAL,   "E_SCS_SYSTEM_ABNORMAL","System abnormal,the operation failed.","ϵͳ�쳣������ʧ��"},


        {E_SCS_CAPI_CONNECT_CREATE,     "E_SCS_CAPI_CONNECT_CREATE",  "Failed to create connect.","��������ʧ��"},
        {E_SCS_CAPI_CONNECT_FAIL,       "E_SCS_CAPI_CONNECT_FAIL",    "Connect server failed.","���ӽ���ʧ��"},
        {E_SCS_CAPI_SEND_REQ_FAIL,      "E_SCS_CAPI_SEND_REQ_FAIL",   "Request message sending failed.","������Ϣ����ʧ��"},
        {E_SCS_CAPI_RECV_RSP_ERROR,     "E_SCS_CAPI_RECV_RSP_ERROR",  "Response data receiving error.","����Ӧ����Ϣ����"},
        {E_SCS_CAPI_RECV_MSG_TIMEOUT,   "E_SCS_CAPI_RECV_MSG_TIMEOUT","Response data receiving timeout,please try again later.",  "����Ӧ��ʱ�����Ժ�����"},
        {E_SCS_CAPI_ENDIAN_REVERSE_FAIL,"E_SCS_CAPI_ENDIAN_REVERSE_FAIL","Message endian reverse failed.",  "��Ϣ�ֽ���ת��ʧ��"},
        {E_SCS_CAPI_SET_SVRSOCK_FAIL,   "E_SCS_CAPI_SET_SVRSOCK_FAIL",   "Set server socket failed.",  "���÷�������ַʧ��"},
        {E_SCS_CAPI_GET_SVRSOCK_FAIL,   "E_SCS_CAPI_GET_SVRSOCK_FAIL",   "Get server socket failed.",  "��ȡ��������ַʧ��"},
        {E_SCS_CAPI_PARAMETER_ERROR,    "E_SCS_CAPI_PARAMETER_ERROR",     "Parameter error.",  "��������"},

        {E_SCS_SMA_MSG_GENERATE,        "E_SCS_SMA_MSG_GENERATE",     "Sma message node generate error.",   "��Ϣ�ڵ�����ʧ��"},
        {E_SCS_SMA_MSG_MAXIMUM,         "E_SCS_SMA_MSG_MAXIMUM",      "Request message exceeds the maximum length.",  "������Ϣ�ĳ��ȳ������ֵ"},
        {E_SCS_SMA_SOCKET_CONNECT,      "E_SCS_SMA_SOCKET_CONNECT",   "Failed to create socket(server) connect.",   "�����߳��У����ӽ���ʧ��"},
        {E_SCS_SMA_THREAD_CREATE,       "E_SCS_SMA_THREAD_CREATE",    "Failed to create SMA processing thread.",  "�̴߳���ʧ��"},
        {E_SCS_SMA_PEER_TIMEOUT,        "E_SCS_SMA_PEER_TIMEOUT",     "The peer process the request message timeout.",  "�Զ˴���������Ϣ��ʱ"},
        {E_SCS_SMA_SEND_REQ_TO_PEER,    "E_SCS_SMA_SEND_REQ_TO_PEER", "Send request message to peer failure.",  "����������Ϣ���Զ�ʧ��"},
        {E_SCS_SMA_PEER_NORUNNING,      "E_SCS_SMA_PEER_NORUNNING",   "The state of peer is not \"RUNNING\" or \"BOOTING\".",   "�Զ˿�����������������״̬"},
        {E_SCS_SMA_CMD_NO_EXIST,        "E_SCS_SMA_CMD_NO_EXIST",     "The request command(sma) does not exist.",  "�����������"},
        {E_SCS_SMA_CMD_NO_OWNER,        "E_SCS_SMA_CMD_NO_OWNER",     "This controller is not the owner of object(Block).",  "���ò������ڷǹ�������������"},
        {E_SCS_SMA_CMD_NO_SUPPORT,      "E_SCS_SMA_CMD_NO_SUPPORT",   "This command is not supported in the environment",  "�����ڸû����в�֧��"},


        {E_SCS_API_MAPGRP_LUN,          "E_SCS_API_MAPGRP_LUN",        "The LUN does not exist in any MapGrp.",  "���߼���Ԫ���������κ�ӳ����"},
        {E_SCS_API_MAPGRP_HOST,         "E_SCS_API_MAPGRP_HOST",       "The Host does not exist in any MapGrp.",  "���������������κ�ӳ����"},
        {E_SCS_API_MAPGRP_MAX_NUM,      "E_SCS_API_MAPGRP_MAX_NUM",    "The numble of MapGrp reached the maximum in system.",   "ϵͳ�е�ӳ������Ŀ�Ѿ��ﵽ���ֵ"},
        {E_SCS_API_MAPGRP_EXIST,        "E_SCS_API_MAPGRP_EXIST",      "The MapGrp already exists in system.",  "ӳ�����Ѿ�����"},
        {E_SCS_API_MAPGRP_WRITE_CFG,    "E_SCS_API_MAPGRP_WRITE_CFG",  "Failed to write information(MapGrp) to the storage media.",  "ӳ������Ϣд�ļ�ʧ��"},
        {E_SCS_API_MAPGRP_LUN_OR_HOST,  "E_SCS_API_MAPGRP_LUN_OR_HOST","There are LUNs or Hosts in the MapGrp.",  "ӳ�����д����߼���Ԫ��������������ɾ��"},
        {E_SCS_API_MAPGRP_NOEXIST,      "E_SCS_API_MAPGRP_NOEXIST",    "The MapGrp does not exist in system.",  "ӳ���鲻����"},
        {E_SCS_API_MAPGRP_LUN_MAX_NUM,  "E_SCS_API_MAPGRP_LUN_MAX_NUM","The numble of LUN reached the maximum in the MapGrp.",  "ӳ�������߼���Ԫ����Ŀ�ﵽ���ֵ"},
        {E_SCS_API_MAPGRP_VOL_EXIST,    "E_SCS_API_MAPGRP_VOL_EXIST",  "The Vol already exists in the MapGrp.",  "ӳ�������Ѿ����ڸþ�"},
        {E_SCS_API_MAPGRP_LUN_NOEXIST,  "E_SCS_API_MAPGRP_LUN_NOEXIST","The LUN does not exist in the MapGrp.",  "ӳ�����и��߼���Ԫ������"},
        {E_SCS_API_MAPGRP_HOST_EXIST,   "E_SCS_API_MAPGRP_HOST_EXIST", "The Host already exists in a MapGrp.",  "�������Ѿ�������ĳ��ӳ������"},
        {E_SCS_API_MAPGRP_HOST_MAX_NUM, "E_SCS_API_MAPGRP_HOST_MAX_NUM","The numble of Host reached the maximum in the MapGrp.",  "ӳ������������Ŀ�ﵽ���"},
        {E_SCS_API_MAPGRP_HOST_NOEXIST, "E_SCS_API_MAPGRP_HOST_NOEXIST","The Host does not exist in the MapGrp.",  "ӳ�����в���������"},
        {E_SCS_API_MAPGRP_ID_INVALID,   "E_SCS_API_MAPGRP_ID_INVALID",  "The MapGrp id is invalid.",  "ӳ����ID�Ƿ�"},
        {E_SCS_API_MAPGRP_NAME_EXIST,   "E_SCS_API_MAPGRP_NAME_EXIST",  "The new name of MapGrp already exists in system.",  "ӳ���������Ѿ�����"},
        {E_SCS_API_MAPGRP_MAX_LUN_EXIST,"E_SCS_API_MAPGRP_MAX_LUN_EXIST","The number of the group that the vol(snap/clone) can be added to reached a maximum",  "�þ��������������ӳ������Ŀ�Ѵ����ֵ"},

        {E_SCS_API_NAME_INVALID,        "E_SCS_API_NAME_INVALID",       "The name is invalid.",  "�������ƷǷ�"},
        {E_SCS_API_VOL_ADD_MAPGRP,      "E_SCS_API_VOL_ADD_MAPGRP",     "Freezing VD is forbidden when vol added to MapGrp.",  "�о����ӳ����ʱ��ֹ����������"},

        {E_SCS_API_LUN_EXIST,             "E_SCS_API_LUN_EXIST",          "The LUN already exists in system.",  "�߼���Ԫ�Ѿ�����"},
        {E_SCS_API_LUN_WRITE_CFG,         "E_SCS_API_LUN_WRITE_CFG",      "Failed to write information(Vol) to the storage media.",  "����Ϣд�����ļ�ʧ��"},
        {E_SCS_API_LUN_NOEXIST,           "E_SCS_API_LUN_NOEXIST",        "The LUN does not exist in system.",  "�߼���Ԫ������"},
        {E_SCS_API_LUN_GLOBAL_ID_INVALID, "E_SCS_API_LUN_ID_INVALID",     "The LUN global id is invalid.",  "�߼���Ԫȫ�ֱ�ʶ����Ч"},
        {E_SCS_API_LUN_ID_INVALID,        "E_SCS_API_LUN_ID_INVALID",     "The LUN id is invalid.",    "�߼���Ԫ��ӳ�����еı�ʶ����Ч"},
        {E_SCS_API_LUN_ID_EXIST,          "E_SCS_API_LUN_ID_EXIST",       "In MapGrp, the LUN id already exists.",  "�߼���Ԫ��ӳ�����еı�ʶ���Ѿ�����"},
        {E_SCS_API_LUN_ADDED_TO_MAPGRP,   "E_SCS_API_LUN_ADDED_TO_MAPGRP","The LUN has been added to MapGrp, can not be deleted.",  "�߼���Ԫ�Ѿ������뵽ӳ���飬�޷�ɾ��"},
        {E_SCS_API_LUN_VOL_OFFLINE,       "E_SCS_API_LUN_VOL_OFFLINE",    "The corresponding vol of the LUN is offline",  "�߼���Ԫ��Ӧ�ľ�����"},

        {E_SCS_API_ROLETYPE_ERROR,              "E_SCS_API_ROLETYPE_ERROR",              "Role type of the net device is error.",  "�豸���ʹ���"},
        {E_SCS_API_DEVICEID_ERROR_FOR_ROLETYPE, "E_SCS_API_DEVICEID_ERROR_FOR_ROLETYPE", "The device id is invalid for the roletype you have chosen.",  "�ý�ɫ�����£��豸��Ŵ���"},
        {E_SCS_API_GATEWAY_ERROR,               "E_SCS_API_GATEWAY_ERROR",               "Gateway error.",  "���ش���"},
        {E_SCS_API_NETCFG_ERROR,                "E_SCS_API_NETCFG_ERROR",                "The information of netconfig should not be null.",  "����������Ϣ����Ϊ��"},
        {E_SCS_API_DEVICE_ERROR,                "E_SCS_API_DEVICE_ERROR",                "The device id is invalid.",  "�豸��Ŵ���"},
        {E_SCS_API_IP_MASK_BOTH_ZERO,           "E_SCS_API_IP_MASK_BOTH_ZERO",           "Ip and netmask of the business must be configured at the same time.",  "ҵ��� ��IP���������ͬʱ����"},
        {E_SCS_API_IP_HOSTBITS_ERROR,           "E_SCS_API_IP_HOSTBITS_ERROR",           "The hostbits of ip should not be all 0 or 1.",  "IP��ַ������λ����ȫ0��ȫ1"},
        {E_SCS_API_NETWORKSEGMENT_ERROR,        "E_SCS_API_NETWORKSEGMENT_ERROR",        "Ip of business and management should not be in the same network segment.",  "����ں�ҵ��ڵ�IP��������ͬһ����"},
        {E_SCS_API_NETWORKSEGMENT_MANAGEMENT,   "E_SCS_API_NETWORKSEGMENT_MANAGEMENT",   "Ip of management and management should not be in the same network segment.",  "����������ڵ�IP��������ͬһ����"},
        {E_SCS_API_NETWORKSEGMENT_BUSYNESS,     "E_SCS_API_NETWORKSEGMENT_BUSYNESS",     "Ip of business and business should not be in the same network segment.",  "ҵ�����ҵ��ڵ�IP��������ͬһ����"},




        {E_SCS_API_OTHER_WRITE_CFG,      "E_SCS_API_OTHER_WRITE_CFG",      "Failed to write information to the storage media.",  "��������Ϣд�ļ�ʧ��"},
        {E_SCS_API_OTHER_READ_CFG,       "E_SCS_API_OTHER_READ_CFG",       "Failed to read information from the storage media.",  "��������Ϣ���ļ�ʧ��"},
        {E_SCS_API_OTHER_GET_NET_INFO,   "E_SCS_API_OTHER_GET_NET_INFO",   "Failed to get the information of netwok device.",   "��ȡ�����豸��Ϣʧ��"},
        {E_SCS_API_OTHER_NET_IP_ERROR,   "E_SCS_API_OTHER_NET_IP_ERROR",   "IP address is invalid.",  "IP��ַ��Ч"},
        {E_SCS_API_OTHER_NET_MASK_ERROR, "E_SCS_API_OTHER_NET_MASK_ERROR", "Subnet mask is invalid.",  "�����������"},
        {E_SCS_API_OTHER_NET_IP_EXIST,   "E_SCS_API_OTHER_NET_IP_EXIST",   "The IP already exists in the system.",  "IP ��ϵͳ���Ѿ�����"},
        {E_SCS_API_OTHER_NET_MAC_EXIST,  "E_SCS_API_OTHER_NET_MAC_EXIST",  "The MAC already exists in the system.",  "MAC��ϵͳ���Ѿ�����"},
        {E_SCS_API_OTHER_ROUTE_GATEWAY_ERROR,   "E_SCS_API_OTHER_ROUTE_GATEWAY_ERROR",   "GateWay is invalid.",  "���ش���"},
        {E_SCS_API_OTHER_ROUTE_NO_EXIST,   "E_SCS_API_OTHER_ROUTE_NO_EXIST",   "The route does not exist.",  "·�ɲ�����"},
        {E_SCS_API_OTHER_NET_MANA_IP_NULL, "E_SCS_API_OTHER_NET_MANA_IP_NULL", "The ip address of management is invalid.",  "�����IP��ַ��Ч"},
        {E_SCS_API_OTHER_NET_IP_RESERVED,"E_SCS_API_OTHER_NET_IP_RESERVED", "The ip address is reserved, the configuration is not allowed..",  "��IPΪϵͳ����������������"},

        {E_SCS_API_OTHER_MAC_INVALID,     "E_SCS_API_OTHER_MAC_INVALID",      "The MAC address is not valid.",  "MAC��ֵַ�Ƿ�"},
        {E_SCS_API_OTHER_SET_MAC_BY_SRV,  "E_SCS_API_OTHER_SET_MAC_BY_SRV.",  "Failed to set MAC address.",  "����MAC ��ַʧ��"},
        {E_SCS_API_OTHER_WRITE_MAC_EEPROM_ERROR,  "E_SCS_API_OTHER_WRITE_MAC_EEPROM_ERROR",  "Failed to write MAC information to the EEPROM.",  "MAC ��Ϣд��EEPROM ʧ��"},
        {E_SCS_API_OTHER_GET_PORT_MTU_MAX_VALUE,  "E_SCS_API_OTHER_GET_PORT_MTU_MAX_VALUE.",  "Failed to get max MTU value of the port.",  "��ȡ�ö˿ڵ����MTUֵʧ��"},
        {E_SCS_API_OTHER_PORT_MTU_INVALID,  "E_SCS_API_OTHER_PORT_MTU_INVALID.",  "The MTU is not valid.",  "MTUֵ�Ƿ�"},

        {E_SCS_API_TASK_SET_NOT_ALLOWED, "E_SCS_API_TASK_SET_NOT_ALLOWED", "Conflict with the status of the task, it is not allowed to perform this operation.",  "�ò���������������״̬���ͻ"},
        {E_SCS_API_TASK_SET_NOT_SUPPORTED, "E_SCS_API_TASK_SET_NOT_SUPPORTED", "The task of this operation is not supported.",  "������ĸ�������ݲ�֧��"},
        {E_SCS_API_TASK_TYPE_NOT_EXIST, "E_SCS_API_TASK_TYPE_NOT_EXIST", "The task type is not supported.",  "���������ݲ�֧��"},
        {E_SCS_API_TASK_EXIST, "E_SCS_API_TASK_EXIST", "The task already exists",  "�������Ѿ�����"},
        {E_SCS_API_TASK_NOT_EXIST, "E_SCS_API_TASK_NOT_EXIST", "The task does not exist, the operation is not allowed.",  "���񲻴��ڣ�������������"},
        {E_SCS_API_TASK_NOT_RUNNING, "E_SCS_API_TASK_NOT_RUNNING", "The task is not running, do not allow the operation.",  "���������״̬��������������"},
        {E_SCS_API_TASK_MAX_NUM, "E_SCS_API_TASK_MAX_NUM", "The number of tasks is already the largest.",  "ϵͳ�еĺ�̨������Ŀ�Ѿ��ﵽ���"},
        {E_SCS_API_STOP_HOTSPARE_SCAN_FAIL, "E_SCS_API_STOP_HOTSPARE_SCAN_FAIL", "Failed to stop the scan task of hotspare disk.",  "ֹͣ�ȱ��̵�ɨ����������ʧ��"},

        {E_SCS_API_SCHEDULE_EXIST, "E_SCS_API_SCHEDULE_EXIST", "The same schedule task already exists.",  "��ͬ�ļƻ������Ѿ�����"},
        {E_SCS_API_SCHEDULE_OUTDATE, "E_SCS_API_SCHEDULE_OUTDATE", "The schedule task is out of date.",  "�üƻ��������"},
        {E_SCS_API_SCHEDULE_TASKID_INVALID, "E_SCS_API_SCHEDULE_TASKID_INVALID", "The schedule task ID is invalid, the task does not exist.",  " �ƻ����������Ч�������񲻴���"},
        {E_SCS_API_SCHEDULE_MAX_NUM, "E_SCS_API_SCHEDULE_MAX_NUM", "The number of schedule tasks is already the largest.",  "ϵͳ�еļƻ�������Ŀ�Ѿ��ﵽ���ֵ"},
        {E_SCS_API_TASK_RUNNING_NUM_UP_LIMIT, "E_SCS_API_TASK_RUNNING_NUM_UP_LIMIT", "The number of running task  exceeds limits, do not allow the operation.",  "�������е������Ѵ����ޣ�������������"},

        {E_SCS_API_VOL_MAP_ABNORMAL,     "E_SCS_API_VOL_MAP_ABNORMAL", "The vol(snap/clone) does not exist in system.",  "ϵͳ�иþ�(���߿��ա���¡��)������"},
        {E_SCS_API_VOL_EXIST_ABNORMAL,   "E_SCS_API_VOL_EXIST_ABNORMAL", "Abnormal vol(snap/clone) is not allowed to perform the operation.",  "�����˿ɼ��ľ�(���߿��ա���¡��)������ִ���������"},
        {E_SCS_API_PEERSTATE_NO_RUNNING, "E_SCS_API_PEERSTATE_NO_RUNNING", "Can not set when single control.",  "����״̬����������"},


        {E_SCS_API_HDD_ABNORMAL,       "E_SCS_API_HDD_ABNORMAL",       "The hard disk is abnormal,it is not visible in a controller .",  "�����쳣����һ�˿������в��ɼ�"},
        {E_SCS_API_HDD_HEALTH_STATE,   "E_SCS_API_HDD_HEALTH_STATE",   "The health status of disk is abnormal.",  "���̽���״̬�쳣"},
        {E_SCS_API_HDD_NOEXIST,        "E_SCS_API_HDD_NOEXIST",        "The hard disk does not exist.",  "������Ϣ������"},
        {E_SCS_API_HDD_UNUSED,         "E_SCS_API_HDD_UNUSED",         "The state of hard disk is not \"UNUSED\".",  "���̷ǿ���״̬"},
        {E_SCS_API_HDD_DATA,           "E_SCS_API_HDD_DATA",           "The state of hard disk is \"DATA\".",  "����Ϊ������"},
        {E_SCS_API_HDD_HOTSPARE,       "E_SCS_API_HDD_HOTSPARE",       "The state of hard disk is not \"HOTSPARE\".",  "���̷��ȱ�״̬"},
        {E_SCS_API_HDD_NO_LOCAL,       "E_SCS_API_HDD_NO_LOCAL",       "The hard disk is not local,this operation is not allowed.",  "���̲��Ǳ����̣����������"},
        {E_SCS_API_HDD_NO_FOREIGN,     "E_SCS_API_HDD_NO_FOREIGN",     "The hard disk is not foreign,this operation is not allowed.",  "���̲��������̣����������"},
        {E_SCS_API_HDD_SET_POWER_MODE_ERROR,     "E_SCS_API_HDD_SET_POWER_MODE_ERROR",     "Set the disk power mode failed.",  "���ô��̵��ܺ�ģʽʧ��"},
        {E_SCS_API_HDD_CAPACITY,       "E_SCS_API_HDD_CAPACITY",       "hotspare disk capacity is smaller than the minimum member disk capacity in VD.",  "�ȱ�������С����������С��Ա������"},
        {E_SCS_API_HDD_TYPE_UNFIT,     "E_SCS_API_HDD_TYPE_UNFIT",     "The type of disk is inconsistent in two controllers.",  "˫���ϴ������Ͳ�һ��"},
        {E_SCS_API_HDD_DATA_IMPORT,    "E_SCS_API_HDD_DATA_IMPORT",    "Foreign Data disk can not import,please execute VD roaming immigrate",  "���������̲������룬��ͨ������������Ǩ�봦��"},
        {E_SCS_API_HDD_SCANNING,       "E_SCS_API_HDD_SCANNING",       "Refused to set the disk type during scanning the disk",  "�����ϴ���ɨ�����񣬲��������ô�������"},
        {E_SCS_API_HDD_NOT_SCANNING,   "E_SCS_API_HDD_NOT_SCANNING",   "Disk scanning task have not begun",  "����ɨ��������δ����"},
        {E_SCS_API_ENOUGH_FIT_DISK,    "E_SCS_API_ENOUGH_FIT_DISK",    "No enough suitable disks(disk capacity is not same or disk is scanning) to create virtual disk.",  "û���㹻���ʴ���(����������һ�»������ɨ��)���޷�����������"},
        {E_SCS_API_HDD_ABNORMAL_SCAN,  "E_SCS_API_HDD_ABNORMAL_SCAN",  "The hard disk is abnormal,failed to perform a scan.",  "�����쳣��ִ��ɨ��ʧ��"},
        {E_SCS_API_PD_SCANNING,        "E_SCS_API_PD_SCANNING",         "Refused to create VD(Pool) during scanning disk.",  "��������ɨ�����񣬲�������������(�洢��)"  },
        {E_SCS_API_DISK_SCAN,          "E_SCS_API_DISK_SCAN",         "Refused to execute this operation during scanning disk.",  "��������ɨ�����񣬲�����ִ�б�����"  },
        {E_SCS_API_PD_SCAN_DATA_ERROR, "E_SCS_API_PD_SCAN_DATA_ERROR",  "Refused to scan the data disk.",  "�����̲�����ɨ��"  },
        {E_SCS_API_PD_SCAN_UNKNOWN_ERROR, "E_SCS_API_PD_SCAN_UNKNOWN_ERROR", "Refused to scan the unknown disk.",  "δ֪�̲�����ɨ��"  },
        {E_SCS_NO_ENOUGH_UNUSED_DISKS, "E_SCS_NO_ENOUGH_UNUSED_DISKS", "No enough unused disks to create virtual disk or pool.",  "û���㹻�Ŀ����̣��޷�����������(�洢��)"  },
        {E_SCS_NO_ENOUGH_UNUSED_PD,    "E_SCS_NO_ENOUGH_UNUSED_PD",    "No enough unused disks.",                                 "û���㹻�Ŀ�����"  },
        {E_SCS_NO_ENOUGH_RIGHT_UNUSED, "E_SCS_NO_ENOUGH_RIGHT_UNUSED", "No enough suitable unused disks.",                        "û���㹻���õĿ�����"  },
        {E_SCS_API_HDD_SLOT_ABNORMAL, "E_SCS_API_HDD_SLOT_ABNORMAL", "The slot of disk is abnormal, the operation is not allowed.",  "���̲�λ��Ϣ�쳣��������ִ�иò���"  },
        {E_SCS_API_HDD_NOT_READY,     "E_SCS_API_HDD_NOT_READY",     "The disk is not ready, the operation is not allowed.",  "����δ׼��������������ִ�иò���"  },


        {E_SCS_API_VD_NO_DEGRADE,                "E_SCS_API_VD_NO_DEGRADE",               "The state of VD is not \"DEGRADE\".",  "�����̷ǽ���״̬���������ؽ�"},
        {E_SCS_API_VD_LOCAL_NOEXIST,             "E_SCS_API_VD_LOCAL_NOEXIST",            "The VD does not exist in local.",  "���������̲����ڣ�������ִ�иò���"},
        {E_SCS_API_VD_PEER_NOEXIST,              "E_SCS_API_VD_PEER_NOEXIST",             "The VD does not exist in peer.",  "�Զ������̲����ڣ�������ִ�иò���"},
        {E_SCS_API_VD_NOEXIST,                   "E_SCS_API_VD_NOEXIST",                  "The VD does not exist.",  "�����̲�����"},
        {E_SCS_API_VD_LEVEL,                     "E_SCS_API_VD_LEVEL",                    "VD raid level is 0, not allow to rebulid.",  "�����̼���Ϊ0���������ؽ�"},
        {E_SCS_API_VD_MEMBER_LIST_INCOMPATIBLE,  "E_SCS_API_VD_MEMBER_LIST_INCOMPATIBLE", "The member list of VD in two controller is not same and not in a subset.",  "���������˿�������Ա���б�һ��Ҳ�����Ӽ���ϵ"},
        {E_SCS_API_VD_MEMBER_LIST_NOT_SAME,      "E_SCS_API_VD_MEMBER_LIST_NOT_SAME",     "The member list of VD in two controller is not same .",  "���������˿�������Ա���б�һ��"},
        {E_SCS_API_VD_STATE_NOT_SAME,            "E_SCS_API_VD_STATE_NOT_SAME",           "The VD state on two controller is not same .",  "���������˿�����״̬��һ��"},
        {E_SCS_API_VD_HOTSPARE_MAX,              "E_SCS_API_VD_HOTSPARE_MAX",             "At most 4 spare disks can be added each time",  "һ���������ĸ��ȱ��̲μ��ؽ�"},
        {E_SCS_API_VD_DELAY_RECOVERING,          "E_SCS_API_VD_DELAY_RECOVERING",         "Vd delay recover is running, operation is not allowed",  "VD�ӳٻָ���,��ֹ����"},


        {E_SCS_API_MIRROR_DISK_ABNORMAL,     "E_SCS_API_MIRROR_DISK_ABNORMAL",      "The state of remote disk is abnormal.",  "Զ�̴���״̬�쳣"},
        {E_SCS_API_MIRROR_DISK_USED,         "E_SCS_API_MIRROR_DISK_USED",          "The remote disk has been used.",  "Զ�̴����Ѿ���ʹ��"},
        {E_SCS_API_MIRROR_DISK_NOEXIST,      "E_SCS_API_MIRROR_DISK_NOEXIST",       "The remote disk does not exist.", "Զ�̴��̲�����"},
        {E_SCS_API_MIRROR_CONNECT_NOEXIST,   "E_SCS_API_MIRROR_CONNECT_NOEXIST",    "The mirror connection does not exist.",  "�������Ӳ�����"},
        {E_SCS_API_MIRROR_CONNECT_MAX,       "E_SCS_API_MIRROR_CONNECT_MAX",        "The number of connections has reached the maximum.",  "����������Ŀ�Ѿ��ﵽ���ֵ"},
        {E_SCS_API_MIRROR_DISK_MAX,          "E_SCS_API_MIRROR_DISK_MAX",           "The number of remote disks has reached the maximum.",  "Զ�̴�����Ŀ�Ѿ��ﵽ���ֵ"},
        {E_SCS_API_NEXUS_MIRROR_EXIST,       "E_SCS_API_NEXUS_MIRROR_EXIST",        "Mirrors exist, the Fc connection is allowed to delete.",  "Զ�̾�����ڣ�FC ���Ӳ�����ɾ��"},
        {E_SCS_API_FC_CONNECT_EXIST,         "E_SCS_API_FC_CONNECT_EXIST",          "The mirror Connection already exists.",  "���������Ѿ�����"},
        {E_SCS_API_FC_CONNECT_MAX_CTRL,      "E_SCS_API_FC_CONNECT_MAX_CTRL",       "The number of connection on the controller has reached the maximum.",  "�ÿ������ϵ�������Ŀ�Ѿ��ﵽ���ֵ"},


        {E_SCS_API_VOL_MAPPED,         "E_SCS_API_VOL_MAPPED",         "The VOL has been mapped.",  "���Ѿ���ӳ�䣬������ִ��ɾ������"},
        {E_SCS_API_VOL_NOT_GOOD,       "E_SCS_API_VOL_NOT_GOOD",       "The state of VOL is not \"GOOD\".",  "��״̬�쳣"},
        {E_SCS_API_VOL_NOT_NORMAL_VOL, "E_SCS_API_VOL_NOT_NORMAL_VOL", "It is not normal Vol,this operation is not allowed.",  "������ͨ��������ִ�иò���"},
        {E_SCS_API_VD_FAULT,           "E_SCS_API_VD_FAULT",           "The state of VD is  fault,the cache policy of the vol on this vd is not allowed to modify .",  "������Ϊ����״̬���������޸Ļ������"},
        {E_SCS_API_VD_IS_RECING,       "E_SCS_API_VD_IS_RECING",       "The state of VD is \"RECING\"",  "���������ڽ����ؽ��У����������"},
        {E_SCS_API_VOL_STATE_NOT_SAME, "E_SCS_API_VOL_STATE_NOT_SAME", "The state of Vol on two controller is not same.",  "�þ��״̬��˫�����Ѿ��쳣���������޸Ļ������"},

        {E_SCS_API_CVOL_INIT_RENAME_ERROR,       "E_SCS_API_CVOL_INIT_RENAME_ERROR",       "If the state of the clone is init , not allow to rename this clone.",  "��¡���ڳ�ʼͬ��״̬�У��������������˿�¡��"},
        {E_SCS_API_CVOL_SYNCING_RENAME_ERROR,       "E_SCS_API_CVOL_SYNCING_RENAME_ERROR",       "If the state of the clone is syncing , not allow to rename this clone.",  "��¡����ͬ��״̬�У��������������˿�¡��"},
        {E_SCS_API_CVOL_REVERSE_SYNCING_RENAME_ERROR,       "E_SCS_API_CVOL_REVERSE_SYNCING_RENAME_ERROR",       "If the state of the clone is reverse syncing , not allow to rename this clone.",  "��¡���ڳ�ʼ��ͬ��״̬�У��������������˿�¡��"},

        {E_SCS_API_SVOL_NOT_ACTIVE,    "E_SCS_API_SVOL_NOT_ACTIVE",    "The state of Svol is not \"ACTIVE\".",  "���վ���\"ACTIVE\"״̬"},
        {E_SCS_API_VOL_NUM_MAXIMUM,    "E_SCS_API_VOL_NUM_MAXIMUM",    "The total number of Vol(Svol) has reached maximum(1024).",  "��(�������վ���¡��)�����ﵽ���"},
        {E_SCS_API_VOL_ADDED_TO_MAPGRP,"E_SCS_API_VOL_ADDED_TO_MAPGRP","The Vol has been added to MapGrp, this operation is not allowed.",  "���Ѿ����뵽ӳ�����У�������ִ�иò���"},
        {E_SCS_API_VOL_DEVICE_OPEN_FAILED, "E_SCS_API_VOL_DEVICE_OPEN_FAILED",  "Failed to open the Vol(Svol/Cvol) device.",  "���豸��ʧ��(�����߼���Ԫʧ��)"},
        {E_SCS_API_VOL_DEVICE_CLOSE_FAILED,"E_SCS_API_VOL_DEVICE_CLOSE_FAILED", "Failed to close the Vol(Svol/Cvol) device.",  "���豸�ر�ʧ��(ɾ���߼���Ԫʧ��)"},
        {E_SCS_API_CVOL_RELATION,"E_SCS_API_CVOL_RELATION", "The CloneVol is unfractured, and not allowed to add to Grp.",  "��¡����Դ����������������ӳ����"},
        {E_SCS_API_VOL_SINGLE_WRITE_BACK,"E_SCS_API_VOL_SINGLE_WRITE_BACK", "Not allow to create vol with write back policy, while the system is single ctrl.",  "������д�ز��ԣ���֧�ִ�����"},
        {E_SCS_API_VOL_MIRROR_OFF_WRITE_BACK,"E_SCS_API_VOL_MIRROR_OFF_WRITE_BACK", "Not allow to create vol with write back policy ,when Mirror state is off .",  "������Ϊ�ؾ��񣬲������ٴ���д�ز��Եľ�"},
        {E_SCS_API_VOL_WRITE_BACK_IN_SINGLE,"E_SCS_API_VOL_WRITE_BACK_IN_SINGLE", "Not allow to set vol to  write back policy ,while the system being in single ctrl .",  "��������£����������þ�Ϊд�ز���"},

        /* ��������add by tao 2012-05-22 */
        {E_SCS_API_VOL_WRITE_BACK_FAN_FAULT,"E_SCS_API_VOL_WRITE_BACK_FAN_FAULT", "fan fault, can only be write through.",  "FAN����ʱ�Զ�ֱдԤ���������ã�cache����ֻ��Ϊд��"},
        {E_SCS_API_VOL_WRITE_BACK_BBU_FAULT,"E_SCS_API_VOL_WRITE_BACK_BBU_FAULT", "bbu fault, can only be through.",  "BBU����ʱ�Զ�ֱдԤ���������ã�cache����ֻ��Ϊд��"},
        {E_SCS_API_VOL_WRITE_BACK_POWER_FAULT,"E_SCS_API_VOL_WRITE_BACK_POWER_FAULT", "power fault, can only be write through.",  "POWER����ʱ�Զ�ֱдԤ���������ã�cache����ֻ��Ϊд��"},


        {E_SCS_API_VOL_MIRROR_OFF_WRITE_BACK_ERROR,"E_SCS_API_VOL_MIRROR_OFF_WRITE_BACK_ERROR", "Not allow to set vol with write back policy ,when Mirror state is off .",  "������Ϊ�ؾ��񣬲���������д�ز��Եľ�"},
        {E_SCS_API_SET_VOL_CACHE_ATTR_FAILED,  "E_SCS_API_SET_VOL_CACHE_ATTR_FAILED",  "Set vol cache policy failed.", "����Դ���cache����ʧ��"},
        {E_SCS_API_SEND_SNAPSHOT_AGENT_FAILED, "E_SCS_API_SEND_SNAPSHOT_AGENT_FAILED", "Send snapshot notice failed.", "���Ϳ���֪ͨʧ��"},
        {E_SCS_API_FIND_AGENT_NODE_FAILED, "E_SCS_API_FIND_AGENT_NODE_FAILED", "Can not find the snapshot agent node.", "�Ҳ������մ���ڵ�"},
        {E_SCS_API_RCV_AGENT_RSP_FAILED, "E_SCS_API_RCV_AGENT_RSP_FAILED", "Receive snapshot agent respond failed.", "���տ��մ�����Ӧʧ��"},
        {E_SCS_API_AGENT_EXECUTE_FAILED, "E_SCS_API_AGENT_EXECUTE_FAILED", "Snapshot agent execute failed.", "���մ���˲���ʧ��"},



        {E_SCS_API_CVOL_VD_FREEZE,"E_SCS_API_CVOL_VD_FREEZE", "The clone in the frozen vd, not allowed it to add to mapping group.",  "��¡�������������Ƕ���״̬��������ÿ�¡�����ӳ����"},
        {E_SCS_API_VOL_VD_FREEZE,"E_SCS_API_VOL_VD_FREEZE", "The vol in the frozen vd, not allowed it to add to mapping group.",  "�������������Ƕ���״̬��������˾����ӳ����"},
        {E_SCS_API_SVOL_VD_FREEZE,"E_SCS_API_SVOL_VD_FREEZE", "The snap in the frozen vd, not allowed it to add to mapping group.",  "���վ�������������Ƕ���״̬��������˿��վ����ӳ����"},
        {E_SCS_API_CVOL_ADDED_TO_MAPGRP,"E_SCS_API_CVOL_ADDED_TO_MAPGRP","This Clone has been added to MapGrp, this operation is not allowed.",  "��¡���Ѿ����뵽ӳ�����У�������ִ�иò���"},

        {E_SCS_API_VOL_VD_FOREIGN,"E_SCS_API_VOL_VD_FOREIGN", "The vol in the foreign vd, not allowed it to add to mapping group.",  "����vd �еľ������������ӳ����"},
        {E_SCS_API_CVOL_VD_FOREIGN,"E_SCS_API_CVOL_VD_FOREIGN", "The clone in the foreign vd, not allowed it to add to mapping group.",  "����vd �еĿ�¡�������������ӳ����"},
        {E_SCS_API_SVOL_VD_FOREIGN,"E_SCS_API_SVOL_VD_FOREIGN", "The snap in the foreign vd, not allowed it to add to mapping group.",  "����vd �еĿ��գ������������ӳ����"},

        {E_SCS_API_ISCSI_INIT_MAX_NUM, "E_SCS_API_ISCSI_INIT_MAX_NUM", "The number of Initiator is more than the maximum.",  "����������Ŀ�������ֵ"},

        {E_FC_SUBCARD_NOT_EXIST, "E_FC_SUBCARD_NOT_EXIST", "The FC SubCard is not exist.",  "FC�ӿ�������"},
        {E_SCS_API_CACHE_STATUS_OFF,    "E_SCS_API_CACHE_STATUS_OFF",     "Cache IO statistics switch is off.",  "���ٻ���ͳ��״̬Ϊ��"},
        {E_SCS_API_IOLUN_STATUS_OFF,    "E_SCS_API_IOLUN_STATUS_OFF",     "System statistics switch is off.",   "ϵͳͳ��״̬Ϊ��"},
        {E_SCS_API_ISCSI_STATUS_OFF,    "E_SCS_API_ISCSI_STATUS_OFF",     "ISCSI IO statistics switch is off.",  "ISCSIͳ��״̬Ϊ��"},

        {E_SCS_API_ISCSI_NEWNAME_INVALID, "E_SCS_API_ISCSI_NEWNAME_INVALID", "New iscsi name is illegal.",  "�µ�ISCSI���ƷǷ�"},


        /* �汾��������� added by wangjing 2011-08-22 */
        {E_SCS_API_SYNC_FILE_FAILED,           "E_SCS_API_SYNC_FILE_FAILED",           "synchronize file to peer failed.",  "ͬ���ļ����Զ�ʧ��"},
        {E_SCS_API_GET_UPLOAD_PROGRESS_FAILED, "E_SCS_API_GET_UPLOAD_PROGRESS_FAILED", "get upload progress failed.",       "��ѯ�ļ��ϴ�����ʧ��"},
        {E_SCS_API_CANCEL_UPLOAD_FAILED,       "E_SCS_API_CANCEL_UPLOAD_FAILED",       "cancel file upload failed.",        "ȡ���ļ��ϴ�ʧ��"},
        {E_SCS_API_VER_FILE_CHK_FAILED,        "E_SCS_API_VER_FILE_CHK_FAILED",        "version file check failed.",        "�汾�ļ�У�����"},
        {E_SCS_API_UPDATE_FILE_FAILED,         "E_SCS_API_UPDATE_FILE_FAILED",         "update file verison failed.",       "�����ļ��汾ʧ��"},
        {E_SCS_API_GET_VERSION_FAILED,         "E_SCS_API_GET_VERSION_FAILED",         "get file version failed.",          "��ѯ�ļ��汾ʧ��"},
        {E_SCS_API_VERSION_FILE_NOT_EXIST,     "E_SCS_API_VERSION_FILE_NOT_EXIST",     "file does not exist.",              "�汾�ļ�������"},
        {E_SCS_API_FILE_PATH_NOT_EXIST,        "E_SCS_API_FILE_PATH_NOT_EXIST",        "file path not exist.",              "�ļ�·��������"},
        {E_SCS_API_NETWORK_NOT_REACHABLE,      "E_SCS_API_NETWORK_NOT_REACHABLE",      "network is not reachable.",         "���粻�ɴ�"},
        {E_SCS_API_USER_PASSWORD_NOT_CORRECT,  "E_SCS_API_USER_PASSWORD_NOT_CORRECT",  "username or password not correct.", "�û��������벻��ȷ"},
        {E_SCS_API_INVALID_FILE_SIZE,          "E_SCS_API_INVALID_FILE_SIZE",          "invalid file size.",                "�ļ���С���Ϸ�"},
        {E_SCS_API_FILENAME_REPEATED,          "E_SCS_API_FILENAME_REPEATED",          "filename exist, please change it.", "�ļ����Ѵ���"},
        {E_SCS_API_WRONG_VERTYPE,              "E_SCS_API_WRONG_VERTYPE",              "wrong version type.",               "����İ汾�ļ�����"},
        {E_SCS_API_FTP_GETFILEDATA_FAIL,       "E_SCS_API_FTP_GETFILEDATA_FAIL",       "ftp get file data fail.",           "�ϴ��ļ�����ʧ��"},
        {E_SCS_API_FTP_PUTFILEDATA_FAIL,       "E_SCS_API_FTP_PUTFILEDATA_FAIL",       "ftp put file data fail.",           "�����ļ�����ʧ��"},
        {E_SCS_API_FILE_EXIST_DUAL,            "E_SCS_API_FILE_EXIST_DUAL",            "file has already existed in local and peer, please rename file and try again.",  "�ļ��ڱ��˺ͶԶ˾��Ѵ���,�����������ٳ���"},
        {E_SCS_API_FILE_EXIST_LOCAL,           "E_SCS_API_FILE_EXIST_LOCAL",           "file has already existed in local, please rename file and try again.",           "�ļ��ڱ����Ѵ���,�����������ٳ���"},
        {E_SCS_API_FILE_EXIST_PEER,            "E_SCS_API_FILE_EXIST_PEER",            "file has already existed in peer, please rename file and try again.",            "�ļ��ڶԶ��Ѵ���,�����������ٳ���"},
        {E_SCS_API_VERSION_NUMBER_NOT_MATCH,   "E_SCS_API_VERSION_NUMBER_NOT_MATCH",   "version number not match in local and peer.",   "���˰汾�Ų�ƥ��"},
        {E_SCS_API_FILE_SIZE_NOT_MATCH,        "E_SCS_API_FILE_EXIST_PEER",            "file size not match in local and peer.",        "�����ļ���С��ƥ��"},
        {E_SCS_API_FILE_SIZE_TOO_BIG,          "E_SCS_API_FILE_SIZE_TOO_BIG",          "file size is too large for local",   "�ļ�����������˽���ʣ��ռ��С"},
        {E_SCS_API_PEER_FILE_SIZE_TOO_BIG,     "E_SCS_API_PEER_FILE_SIZE_TOO_BIG",     "file size is too large for peer",    "�ļ���������Զ˽���ʣ��ռ��С"},
        {E_SCS_API_UPLOAD_TASK_IS_RUNNING,     "E_SCS_API_UPLOAD_TASK_IS_RUNNING",     "upload file task is already running, not finish", "���ļ��ϴ������Ѿ������У�δ����"},
        {E_SCS_API_GET_DOWNLOAD_PROGRESS_FAILED,"E_SCS_API_GET_DOWNLOAD_PROGRESS_FAILED",  "get download progress failed.",   "��ѯ�ļ����ؽ���ʧ��"},
        {E_SCS_API_GET_VERSION_BUSY,           "E_SCS_API_GET_VERSION_BUSY",           "version is updating, operation is not allowed.",   "�汾�����У���ֹ����"},
        {E_SCS_API_VERSION_NETBOOT_REWRITE,    "E_SCS_API_VERSION_NETBOOT_REWRITE",    "system is busy, please try later.", "ϵͳæ�����Ժ���"},
        {E_SCS_API_FILE_NAME_INVAILD,          "E_SCS_API_FILE_NAME_INVAILD",          "file name is invaild(the first character must be underline,letter or figure.From the second character can also be dot and transverse line,but the transverse line can not as the last character)", "�ļ������Ϸ�(��һ���ַ��������»��ߡ���ĸ�����֣������ַ��������ǵ�ͺ��ߣ������߲��������һ���ַ�)"},
        {E_SCS_API_FILE_INCONSISTENCY,         "E_SCS_API_FILE_UNCONSISTENCY",         "update file name is inconsistency in dual.", "���˴����°汾�ļ����Ʋ�һ��"},
        {E_SCS_API_FILE_NAME_TOO_LONG,         "E_SCS_API_FILE_NAME_TOO_LONG",         "file name too long,please check", "�ļ���̫��������"},
        {E_SCS_API_SAMENAME_WITH_CURVER,       "E_SCS_API_SAMENAME_WITH_CURVER",       "file name is same with main version or standby version,please check", "�ļ����뵱ǰ�汾�����ð汾���ظ�������"},
        {E_SCS_API_BGTASK_RUNNING,             "E_SCS_API_BGTASK_RUNNING",             "system task is running, please wait and try later", "��ϵͳ����̨������������,���Ժ�����"},
        {E_SCS_API_TOPO_ERROR,                 "E_SCS_API_TOPO_ERROR",                 "topo error update forbidden",   "���˽ṹ������������������8005"},
        {E_SCS_API_WRONG_PM8005_TYPE,          "E_SCS_API_WRONG_PM8005_TYPE",          "wrong expander version type",   "EXP�汾���Ͳ���ȷ����ʹ����ȷ�Ĵ��ڰ汾"},
        /* �����ļ����뵼����ش����� add by tao 2011-08-16 */
        {E_SCS_API_FILE_EXIST,             "E_SCS_API_FILE_EXIST", "the file already exists, please rename and try again.",  "���ļ��Ѵ���,������������"},
        {E_SCS_API_FILE_NO_EXIST,          "E_SCS_API_FILE_NO_EXIST", "the file does not exists.",  "���ļ�������"},
        {E_SCS_API_FILE_CP_FAILED,         "E_SCS_API_FILE_CP_FAILED", "copy file failed.", "�����ļ�ʧ��"},
        {E_SCS_API_EXPORT_CFG_FILE_FAILED, "E_SCS_API_EXPORT_CFG_FILE_FAILED", "export config file failed.",  "�����ļ�����ʧ��"},
        {E_SCS_API_IMPORT_CFG_FILE_FAILED, "E_SCS_API_IMPORT_CFG_FILE_FAILED", "import config file failed.",  "�����ļ�����ʧ��"},
        {E_SCS_CFG_FILE_CHECK_ERR,         "E_SCS_CFG_FILE_CHECK_ERR", "the config file is illegal",  "�����ļ����Ϸ�"},
        {E_SCS_CFG_FILE_OPEN_FAILED,       "E_SCS_CFG_FILE_OPEN_FAILED", "open the config file failed.",  "�����ļ��޷���"},


        {E_SCS_ISCSI_NEWNAME_SAME_PERR, "E_SCS_ISCSI_NEWNAME_SAME_PERR", "New ISCSI name is same with peer iqn.", "�µ�ISCSI ������Զ���ͬ"},

        {E_SMA_PARAM_WRONG,           "E_SMA_PARAM_WRONG",          "Invalid parameter.",                "������Ч"},
        {E_SMA_MALLOC_ERROR,          "E_SMA_MALLOC_ERROR",         "Memory malloc failed.",             "�ڴ����ʧ��"},
        {E_SMA_USER_WRONG,            "E_SMA_USER_WRONG",           "Username or password is incorrect.","�û����������벻��ȷ"},
        {E_SMA_USER_LOGINED,          "E_SMA_USER_LOGINED",         "This user have logined in.",        "���û��Ѿ���¼"},
        {E_SMA_USER_EXIST,            "E_SMA_USER_EXIST",           "Username already exists.",          "�û����Ѵ���"},
        {E_SMA_USER_NEXIST,           "E_SMA_USER_NEXIST",          "Username does not exist.",          "�û���������"},
        {E_SMA_ADMIN,                 "E_SMA_ADMIN",                "Super admin has logined in.",       "��������Ա�Ѿ���¼"},
        {E_SMA_COMMON,                "E_SMA_COMMON",               "Other admin level user has logined in.","��������Ա�û��Ѿ���¼"},
        {E_SMA_USER_AUTHOR,           "E_SMA_USER_AUTHOR",          "This user does not have enough authority.","���û�û���㹻Ȩ��"},
        {E_SMA_AUTHEN,                "E_SMA_AUTHEN",               "This user has not been authenticated.","���û�û����Ȩ"},
        {E_SMA_PASSWD,                "E_SMA_PASSWD",               "Old password does not match.",      "�����벻ƥ��"},
        {E_SMA_COMM_USER_OVERFLOW,    "E_SMA_COMM_USER_OVERFLOW",   "Admin user number exceeds limits.", "����Ա�û��Ѿ��������ޣ����������"},
        {E_SMA_GUEST_USER_OVERFLOW,   "E_SMA_GUEST_USER_OVERFLOW",  "Viewer user number exceeds limits.","�鿴�û��Ѿ��������ޣ����������"},
        {E_SMA_USER_OVERFLOW,         "E_SMA_USER_OVERFLOW",        "Total user number exceeds limits.", "�û���Ŀ�Ѿ��������ޣ����������"},
        {E_SMA_ADMIN_DEL,             "E_SMA_ADMIN_DEL",            "Cannot delete super admin.",        "��������Ա���ܱ�ɾ��"},
        {E_SMA_ONLINE_DEL,            "E_SMA_ONLINE_DEL",           "Cannot delete online account.",     "����ɾ�������û�"},
        {E_SMA_CONN_OVERFLOW,         "E_SMA_CONN_OVERFLOW",        "Too many connects exist.",          "�������Ѵ�����"},
        {E_SMA_SESSION_ATHOR,         "E_SMA_SESSION_ATHOR",        "User does not have enough authority,can not login.","�û�Ȩ�޲���,���ܵ�¼"},
        {E_SMA_SESSION_NEXIST,        "E_SMA_SESSION_NEXIST",       "Session does not exist.",           "�Ự������"},
        {E_SMA_ADMIN_EXIST,           "E_SMA_ADMIN_EXIST",          "Can not add super admin type user.","������ӳ����û�"},

            {E_SMA_FAIL_SEND,             "E_SMA_FAIL_SEND",            "Failed to send email.",             "�����ʼ�ʧ��"},
            {E_SMA_NSMTP,                     "E_SMA_NSMTP",                "SMTP does not set ok.",             "�������û������"},
            {E_SMA_USER_NPWDMAIL,           "E_SMA_USER_NPWDMAIL",  "Password mail does not set.",       "������������û������"},

        {E_SMA_SYS_NOT_FINISH_STARTUP,"E_SMA_SYS_NOT_FINISH_STARTUP",  "System have not finished startup.",  "ϵͳ����δ���"},
        {E_SCS_SMA_SYSTEM_BUSY,       "E_SCS_SMA_SYSTEM_BUSY",      "System is busy, please try again later.",  "ϵͳæ�����Ժ�����"},
        {E_SCS_SMA_RCC_INSTABILITY,       "E_SCS_SMA_RCC_INSTABILITY",      "Channel is instability.",  "ͨ��Ŀǰ���ȶ�����ֹ���"},
        {E_SMA_USER_NEWNAME_HAVE_EXIT,"E_SMA_USER_NEWNAME_HAVE_EXIT",  "New name is illegal,it has exist in system.",  "�µ��û����Ƿ���ϵͳ���Ѵ��ڴ��û���"},

        /******************************* TIME **********************************/
        {E_SCS_TIME_NTP_SERVER_ADDR_NULL,         "E_SCS_TIME_NTP_SERVER_ADDR_NULL",         "NTP server address is NULL.",       "NTP��������ַΪ��"},
        {E_SCS_TIME_NTP_SERVER_ADDR_INVALID,      "E_SCS_TIME_NTP_SERVER_ADDR_INVALID",      "NTP server address is invalid.",     "NTP��������ַ�Ƿ�"},
        {E_SCS_TIME_GET_SYSTEM_TIME_ZONE_FAILED , "E_SCS_TIME_GET_SYSTEM_TIME_ZONE_FAILED",   "Get system time zone failed.",       "��ȡϵͳʱ��ʧ��"},
        {E_SCS_TIME_SET_HARDWARE_CLOCK_FAILED,    "E_SCS_TIME_SET_HARDWARE_CLOCK_FAILED",    "Set hardware clock faiked.",         "����Ӳ��ʱ��ʧ��"},
        {E_SCS_TIME_THE_CITY_IS_INVALID,          "E_SCS_TIME_THE_CITY_IS_INVALID",          "The city is invalid.",               "�ó�����Ч"},
        {E_SCS_TIME_NO_VALID_CONTINENT_NAME,      "E_SCS_TIME_NO_VALID_CONTINENT_NAME",      "Did not input continent name.",      "δ����������"},
        {E_SCS_TIME_SET_TIME_ZONE_FAILED,         "E_SCS_TIME_SET_TIME_ZONE_FAILED",         "Set time zone by index failed.",     "���ݳ�����������ʱ��ʧ��"},
        {E_SCS_TIME_GET_NTP_SERVER_CONFIG_FAILED, "E_SCS_TIME_GET_NTP_SERVER_CONFIG_FAILED",  "Get NTP server config failed.",     "��ȡNTP����������ʧ��"},
        {E_SCS_TIME_SET_NTP_CONFIG_FAILED,        "E_SCS_TIME_SET_NTP_CONFIG_FAILED",        "Set NTP config failed.",            "����NTP����ʧ��"},
        {E_SCS_TIME_SET_AUTO_NTP_FAILED,          "E_SCS_TIME_SET_AUTO_NTP_FAILED",          "Set auto NTP synchronize failed.",   "�����Զ�ʱ��ͬ��ʧ��"},
        {E_SCS_TIME_ENTER_TIME_INVALID,           "E_SCS_TIME_ENTER_TIME_INVALID",           "Enter time is invalid.",             "�����ʱ����Ч"},
        {E_SCS_TIME_SYNC_TIME_BY_NTP_FAILED,      "E_SCS_TIME_SYNC_TIME_BY_NTP_FAILED",      "Synchronize system time by ntp failed.", "��ntp������ͬ��ʱ��ʧ��"},
        {E_SCS_TIME_WRITE_CFG_REC_FAILED,         "E_SCS_TIME_WRITE_CFG_REC_FAILED",         "Write config record failed.",        "д�����ļ���¼ʧ��"},
        {E_SCS_TIME_READ_CFG_REC_FAILED,          "E_SCS_TIME_READ_CFG_REC_FAILED",          "Read config record failed.",         "�������ļ���¼ʧ��"},
        {E_SCS_TIME_FORK_FAILED,                  "E_SCS_TIME_FORK_FAILED",                 "Operate failed.",                   "����ʧ��"},


        /******************************************************************/

        /* *******************************  LOG & MAILSET  ************************************* */
        {E_LOG_PARA_NULL,              "E_LOG_PARA_NULL",             "Point parameters null.",                       "ָ�����Ϊ�ա�"},
        {E_LOG_PARAM_ILLEAG,           "E_LOG_PARAM_ILLEAG",          "Parameters passed in or passed out illeagal.", "����򴫳��Ĳ����Ƿ���"},
        {E_LOG_GET_FAILED,             "E_LOG_GET_FAILED",            "Log inquiring failed.",                        "��־��ѯ����ʧ�ܡ�"},
        {E_LOG_CLEAR_FAILED,           "E_LOG_CLEAR_FAILED",          "Log clear failed.",                            "��־�������ʧ�ܡ�"},
        {E_LOG_EXPORT_FAILED,          "E_LOG_EXPORT_FAILED",         "Log exporting failed.",                        "��־��������ʧ�ܡ�"},
        {E_LOG_EXPORT_ZERO,            "E_LOG_EXPORT_ZERO",           "Log number is 0, you could not export.",       "��־����Ϊ�㣬�޷�������"},
        {E_LOG_GETNUM_FAILED,          "E_LOG_GETNUM_FAILED",         "Log account obtain failed.",                   "��ȡ��־��������"},
        {E_LOG_SET_WMAIL_FAILED,       "E_LOG_SET_WMAIL_FAILED",      "Log alarm mail configuration setting failed.", "�澯�ʼ�����������ʧ�ܡ�"},
        {E_LOG_GET_WMAIL_FAILED,       "E_LOG_GET_WMAIL_FAILED",      "Inquiring log alarm mail configuration failed.","��ѯ�澯�ʼ�������ʧ�ܡ�"},
        {E_LOG_GET_USERLOG_AUTH,       "E_LOG_GET_USERLOG_AUTH",      "User-log inquiring permission denied.","��ѯ�û�������־Ȩ�޲��㡣"},
        {E_LOG_GET_USERLOG_USERINV,    "E_LOG_GET_USERLOG_USERINV",   "Username invalid in user-log inquiring.","�û�������־��ѯ�����û������á�"},
        {E_LOG_PEER_ABSENT,            "E_LOG_PEER_ABSENT",           "Peer controler is absent or booting.",          "�Զ˿���������λ��������״̬��"},
        {E_LOG_GET_ALARM_FAILED,       "E_LOG_GET_ALARM_FAILED",      "Alarm inquiring failed.",                       "�澯��ѯ����ʧ�ܡ�"},
        {E_LOG_DEL_ALARM_FAILED,       "E_LOG_DEL_ALARM_FAILED",      "Alarm deleting failed.",                        "�澯ɾ������ʧ�ܡ�"},

        {E_MAIL_PARA_NULL,             "E_MAIL_PARA_NULL",            "Point parameters null.",                       "ָ�����Ϊ�ա�"},
        {E_MAIL_PARAM_ILLEAG,          "E_MAIL_PARAM_ILLEAG",         "Parameters illeagal.",                         "�����Ƿ���"},
        {E_MAIL_LINK_HOST_FAILED,      "E_MAIL_LINK_HOST_FAILED",     "Link host failed.",                            "�����ʼ�����������ʧ�ܡ�"},
        {E_MAIL_LOGIN_FAILED,          "E_MAIL_LOGIN_FAILED",         "Login mail host failed.",                      "��½�ʼ���������������"},
        {E_MAIL_CONFFILE_CREAT_FAILED, "E_MAIL_CONFFILE_CREAT_FAILED","Mail config file create failed.",              "�ʼ������ļ�����ʧ�ܡ�"},
        {E_MAIL_SET_ENABLE_FAILED,     "E_MAIL_SET_ENABLE_FAILED",    "Enable mail sending failed.",                  "�ʼ�����ʧ�ܡ�"},
        {E_MAIL_UNABLED_MAIL_ERR,      "E_MAIL_UNABLED_MAIL_ERR",     "Mail sending error: mail switch is not enabled.","�ʼ����ʹ���:�ʼ����ط�ʹ�ܡ�"},
        {E_MAIL_CONFIG_VERIMAIL_FAILED,"E_MAIL_CONFIG_VERIMAIL_FAILED","Verifying test mail sending error, check configuration.","��֤�����ʼ�����ʧ�ܣ��������á�"},
        {E_MAIL_CONF_GET_FAILED,       "E_MAIL_CONF_GET_FAILED",      "Mail config inquiring failed.",                "�ʼ���ѯʧ�ܣ��������á�"},
        {E_MAIL_CONF_SET_FAILED,       "E_MAIL_CONF_SET_FAILED",      "Set mail config failed.",                      "������������ʧ�ܡ�"},
        {E_MAIL_GET_ENABLE_FAILED,     "E_MAIL_GET_ENABLE_FAILED",    "Inquiring mail-sending switch failed.",        "��ѯ����������Ϣʧ�ܡ�"},
        {E_MAIL_SENDING_FAILED,        "E_MAIL_SENDING_FAILED",       "Mail sending failed.",                         "�ʼ�����ʧ�ܡ�"},

        /* *************************************** SIC ***********************************************/
        {E_SIC_PARAM_NULL,             "E_SIC_PARAM_NULL",            "Parameters is null.",                          "����Ϊ�ա�"},
        {E_SIC_PARAM_ILLEAG,           "E_SIC_PARAM_ILLEAG",          "Parameters is illeagal.",                      "�����Ƿ���"},
        {E_SIC_COLLECT_SYS_INFO_FAILED, "E_SIC_COLLECT_SYS_INFO_FAILED", "Collect system information failed.",        "�ɼ�ϵͳ��Ϣʧ�ܡ�"},
        {E_SIC_GET_TASK_PROC_FAILED,   "E_SIC_GET_TASK_PROC_FAILED",  "Get task process failed.",                     "��ȡϵͳ��Ϣ�ɼ��������ʧ�ܡ�"},
        {E_SIC_SET_EMAIL_ENABLE_FAILED, "E_SIC_SET_EMAIL_ENABLE_FAILED", "Set email config failed.",                  "�����ʼ�����ʧ�ܡ�"},
        {E_SIC_GET_TARBALL_INFO_FAILED, "E_SIC_GET_TARBALL_INFO_FAILED", "Get tar ball information failed.",          "��ȡϵͳ��Ϣѹ������Ϣʧ�ܡ�"},
        {E_SIC_GET_EMAIL_ENABLE_FAILED, "E_SIC_GET_EMAIL_ENABLE_FAILED", "Get email config failed.",                  "��ȡ�ʼ�����ʧ�ܡ�"},
        {E_SIC_TAR_BALL_NOT_EXIST,     "E_SIC_TAR_BALL_NOT_EXIST",    "The tar ball does not exist.",                 "ϵͳ��Ϣѹ���������ڡ�"},
        {E_SIC_TASK_IS_EXIST,          "E_SIC_TASK_IS_EXIST",         "The task to collect system information already exists.", "ϵͳ��Ϣ�ɼ������Ѿ����ڡ�"},
        {E_SIC_EXPORT_SYSINFO_TARBALL_FAILED, "E_SIC_EXPORT_SYSINFO_TARBALL_FAILED", "Export system information tar ball failed.", "����ϵͳ��Ϣѹ����ʧ�ܡ�"},


        /* *************************************** DIAG ***********************************************/
        {E_DIAG_GET_INFO_FAILED,        "E_DIAG_GET_INFO_FAILED",       "Diag get info failed .",                     "��ȡ��Ϣʧ�ܡ�"    },
        {E_DIAG_PARA_NULL,              "E_DIAG_GET_INFO_FAILED",       "Point parameters null.",                     "��������ȷ��"      },
        {E_DIAG_SODA_FAILED,            "E_DIAG_SODA_FAILED",           "Board contrl interface failed.",             "�ײ�ӿڵ���ʧ�ܡ�"},
        {E_DIAG_ECC_SWITCH_SET_FAILED,  "E_DIAG_ECC_SWITCH_SET_FAILED", "Set ecc check switch failed.",               "����ECC��⿪��ʧ��"},
        {E_DIAG_GET_INFO_NOTSUPPORT,    "E_DIAG_GET_INFO_NOTSUPPORT",   "No such device.",                            "�����ڸ��豸��"    },
        /* ***************************************TRAP ***********************************************/
        {E_SMA_TRAP_OVERFLOW,           "E_DIAG_GET_INFO_FAILED",      "Trap config number exceeds limits.", "Trap�����Ѿ��������ޣ����������"},
        {E_SCS_TRAP_ID_NOT_EXIST,       "E_SCS_TRAP_ID_NOT_EXIST",      "Trap config id is not exist.", "Trap ����Id ������"},

         /*******************************STS ��ϵͳ************************************/
        {E_STS_LUNDEV_NO_EXIST_ERR,     "E_STS_LUNDEV_NO_EXIST_ERR",   "The device does not exist.",    "�豸������."},
        {E_STS_SEQNO_NUM_ERR,             "E_STS_SEQNO_NUM_ERR",          "Illegal device number .",      " �豸����������Ƿ�."},
        {E_STS_INITIATOR_NO_EXIST_ERR,       "E_STS_INITIATOR_NO_EXIST_ERR",   "Host name does not exist.",     " ��ϵͳ���Ҳ�����������."},
        {E_STS_CACHE_POLICY_ERR,         "E_STS_CACHE_POLICY_ERR",    "Illegal cache policy.", " ���ٻ��������������Ƿ�."},
        {E_STS_AHEAD_READ_SIZE_ERR,    "E_STS_AHEAD_READ_SIZE_ERR",   "Illegal ahead_read_size.",      " Ԥ��ϵ����������Ƿ�."},
        {E_STS_LOCALLUN_NUM_ERR,             "E_STS_LOCALLUN_NUM_ERR",           "Illegal locallun .",     " �����߼���Ԫ����������Ƿ�."},
        {E_STS_LUN_NO_EXIST_ERR,             "E_STS_LUN_NO_EXIST_ERR",       "Locallun does not exist.",      "��ӳ�������Ҳ������߼���Ԫ"},
        {E_STS_REFRESH_CACHE_ERR,             "E_STS_REFRESH_CACHE_ERR",       "Refresh cache failed.",     "ˢ��cacheʧ��."},
        {E_STS_LUNDEV_IS_MAPPING_ERR,       "E_STS_LUNDEV_IS_MAPPING_ERR",       "Device is mapping, can not refresh cache.",     "�豸���滹����ӳ�䣬����ˢ��cache."},
        {E_STS_OWNER_PARAM_ERR,         "E_STS_OWNER_PARAM_ERR",   "Illegal owner.",        "��������������Ƿ�."},
        {E_STS_PREFER_PARAM_ERR,         "E_STS_PREFER_PARAM_ERR",   "Illegal prefer .",       "���ȿ���������Ƿ�."},
        {E_STS_DISCARD_CACHE_ERR,             "E_STS_DISCARD_CACHE_ERR",               "Discard cache failed.",      " ����cacheʧ��."},
        {E_STS_REMOVE_MAP_TIMEOUT_ERR,             "E_STS_REMOVE_MAP_TIMEOUT_ERR",               "Remove MAP timeout.",      " ɾ��ӳ�䳬ʱ."},
        {E_STS_WPFLAG_PARAM_ERR,             "E_STS_WPFLAG_PARAM_ERR",    "Illegal Write Protect flag.",      " д������־����Ƿ�."},


        {E_STS_STSCREATELUNDEV_ALLOC_LUNDEV_FAILED_ERR,   "E_STS_STSCREATELUNDEV_ALLOC_LUNDEV_FAILED_ERR",    "Alloc  lun device  failed.",     "���߼���Ԫ�豸����ռ�ʧ��."  },
        {E_STS_STSCREATELUNDEV_OPEN_LUNDEV_FAILED_ERR,  "E_STS_STSCREATELUNDEV_OPEN_LUNDEV_FAILED_ERR" ,  "Open lun device failed.",     "���߼���Ԫ�豸ʧ��."},
        {E_STS_STSCREATELUNDEV_SEQNO_EXIST_ERR,     "E_STS_STSCREATELUNDEV_SEQNO_EXIST_ERR",     "Device number has already existed.",     "�豸���Ѵ���."   },
        {E_STS_STSCREATELUNDEV_LUNDEV_EXIST_ERR,    "E_STS_STSCREATELUNDEV_LUNDEV_EXIST_ERR",    "The device  has already opened." ,    "�豸�Ѿ���."},
        {E_STS_STSREMOVELUNDEV_BUSY_ERR,    "E_STS_STSREMOVELUNDEV_BUSY_ERR",    "The device  is busy now." ,    "�豸æ���ر�ʧ��."},
        {E_STS_STSLISTLUNDEV_LUNDEV_REFCNT_ERR,      "E_STS_STSLISTLUNDEV_LUNDEV_REFCNT_ERR"    ,    "Error lun device refcnt.",     "�߼���Ԫ�豸�����ü�������."},
        {E_STS_STSCREATELUNMAP_ALLOC_INITIATOR_FAILED_ERR,   "E_STS_STSCREATELUNMAP_ALLOC_INITIATOR_FAILED_ERR", "Alloc initiator struct failed.",        "�����������ṹ���ڴ�ʧ��"},
        {E_STS_STSCREATELUNMAP_ALLOC_LUN_FAILED_ERR,         "E_STS_STSCREATELUNMAP_ALLOC_LUN_FAILED_ERR",       "Alloc LUN struct failed.",         "�����߼���Ԫ�ṹ���ڴ�ʧ��"},
        {E_STS_STSCREATELUNMAP_LUN_EXIST_ERR,                "E_STS_STSCREATELUNMAP_LUN_EXIST_ERR",              "Locallun has already existed.",    "�����߼���Ԫ���Ѿ���ռ��"},
        {E_STS_STSCREATELUNMAP_OPEN_LUNDEV_ERR,              "E_STS_STSCREATELUNMAP_OPEN_LUNDEV_ERR",            "Open this device failed.",      "���豸ʧ��"},
        {E_STS_STSSETSTATSTATUS_PARAM_ERR,              "E_STS_STSSETSTATSTATUS_PARAM_ERR",              "Illegal statistics status param,  it should be 0(OFF) OR  1(ON)  OR 2(RESET).",     " ͳ�ƿ�����������Ƿ���ֻ����0(�ر�)��1(��)��2(����)."},
        {E_STS_STSPARAMINIT_CONTROLLER_ID_ERR,          "E_STS_STSPARAMINIT_CONTROLLER_ID_ERR",         "Illegal controller Id param,  it should be 0(CONTROLLER_A) OR 1(CONTROLLER_B).",   " ��������ʶ����������Ƿ���ֻ����0(������A)����1(������B)."},
        {E_STS_STSSETCONTGTINFO_WAKEUP_THREAD_FLAG_NULL_ERR,    "E_STS_STSSETCONTGTINFO_WAKEUP_THREAD_FLAG_NULL_ERR",   "RGPT cmd overtimed, the opposite  port info has not sent to local port in time.",   "��ѯĿ��˿�����Ϣ����ִ�г�ʱ���Զ˵Ķ˿���Ϣû�м�ʱ��������."},
        {E_STS_STSMODIFYLUNDEVERRORFLAG_PARAM_ERR,          "E_STS_STSMODIFYLUNDEVERRORFLAG_PARAM_ERR",           "Illegal error flag param .",     "�豸�����־λ��������Ƿ�."},
        {E_STS_STSMODIFYLUNDEVPORTSTATUS_PARAM_ERR,          "E_STS_STSMODIFYLUNDEVPORTSTATUS_PARAM_ERR",          "Illegal port status param .",     "�豸�˿�״̬��־λ��������Ƿ�."},
        {E_STS_STSLISTLUNSEQNOONSESSION_PARAM_ERR,           "E_STS_STSLISTLUNSEQNOONSESSION_PARAM_ERR",           "Illegal IT&session param , session struct is NULL.",      "IT���Ự��������Ƿ����Ự�ṹ��Ϊ��"},
        {E_STS_STSSEARCHITONCONTORL_PARAM_ERR,          "E_STS_STSSEARCHITONCONTORL_PARAM_ERR",       "Illegal IT&session param , session struct is NULL.",      "IT���Ự��������Ƿ����Ự�ṹ��Ϊ��"},
        {E_STS_STSSEARCHITONCONTORL_IT_ERR,                "E_STS_STSSEARCHITONCONTORL_IT_ERR",                "Error IT Nexus.",     "IT  ���ӹ�ϵ����."},
        {E_STS_STSMODIFYLUNDEVRSTFLAG_PARAM_ERR,          "E_STS_STSMODIFYLUNDEVRSTFLAG_PARAM_ERR",            "Illegal LUN reset status param, .",    "�߼���Ԫ������־λ��������Ƿ�."},
        {E_STS_STSSETALLSTATSTATUS_PARAM_ERR,             "E_STS_STSSETALLSTATSTATUS_PARAM_ERR",               "Illegal statistics status param .",      " ͳ�ƿ�����������Ƿ�."},
        {E_STS_LUNDEV_WAIT_CMDS_TIMEOUT_ERR,             "E_STS_LUNDEV_WAIT_CMDS_TIMEOUT_ERR",               "Wait cmds timeout .",      " �ȴ����ʱ."},
        {E_STS_ASYMMETRIC_ISTRANSITING_ERR, "E_STS_ASYMMETRIC_ISTRANSITING_ERR", "The Lun status is transiting.", " LUN״̬�л���."},


     /*******************************CBS ��ϵͳ************************************/
        {E_CBS_OPEN_FILE_ERROR,         "E_CBS_OPEN_FILE_ERROR",        "Open dev error.",                      "���豸�ļ�ʧ��"},
        {E_CBS_BAD_ADDR,                 "E_CBS_BAD_ADDR",              "Error address.",                       "�����ַ��Ч"},
        {E_CBS_ERR_CACHE_SIZE,           "E_CBS_ERR_CACHE_SIZE",        "Error cache size.",                    "���ٻ��浥Ԫ��С��Ч"},
        {E_CBS_ERR_DIRTY_RATIO,          "E_CBS_ERR_DIRTY_RATIO",       "Error write ratio.",                   "д������Ч"},
        {E_CBS_ERR_PCIE_NO_LINK,         "E_CBS_ERR_PCIE_NO_LINK",      "PCIe channel linking is not up",       "PCIe������·��δ����ǰ������򿪾�����"},
        {E_CBS_ERR_OPEN_MIRROR_PARM,     "E_CBS_ERR_OPEN_MIRROR_PARM",  "Error mirror parameter.",              "���������Ч"},
        {E_CBS_ERR_MONITOR_PARM,         "E_CBS_ERR_MONITOR_PARM",      "Error monitor parameter.",             "��ز�����Ч"},
        {E_CBS_MONITOR_STATUS_OFF,       "E_CBS_MONITOR_STATUS_OFF",    "Monitor status is off.",               "ͳ�ƿ����ǹرյ�"},

        /*******************************DM ��ϵͳ************************************/
        {E_DM_INPUT_PARAM_INVALID,   "E_DM_INPUT_PARAM_INVALID",      "Can not finish request.",        "�޷��������"},
        {E_DM_OPEN_DM_DIR_FAIL,      "E_DM_OPEN_DM_DIR_FAIL",         "open dm fail.",                  "�޷��������"},
        {E_DM_ALLOC_RESOURCE_FAIL,   "E_DM_ALLOC_RESOURCE_FAIL",      "No system resource.",            "�޷��������"},
        {E_DM_DISK_ABSENT,           "E_DM_DISK_ABSENT",              "No system resource.",            "�޷��������"},
        {E_DM_SOCKET_FAIL,           "E_DM_SOCKET_FAIL",              "System internal error.",         "ϵͳ�ڲ�����"},
        {E_DM_GET_WARN_SWITCH_FAIL,  "E_DM_GET_WARN_SWITCH_FAIL",     "Get disk warn switch fail.",     "��ȡ���̸澯����ʧ��"},
        {E_DM_GET_BBLK_LIMIT_FAIL,   "E_DM_GET_BBLK_LIMIT_FAIL",      "Get disk bad block limit fail.", "��ȡ���̸澯����ֵʧ��"},
        {E_DM_GET_RUNNING_STATE_FAIL,"E_DM_GET_RUNNING_STATE_FAIL",   "Get dm running state fail.",     "��ȡϵͳ����״̬ʧ��"},
        {E_DM_GET_CTRL_UUID_FAIL,    "E_DM_GET_CTRL_UUID_FAIL",       "Get system ctrl UUID fail.",     "��ȡϵͳ����״̬ʧ��"},
        {E_DM_GET_DEBUG_LEVEL_FAIL,  "E_DM_GET_DEBUG_LEVEL_FAIL",     "Get system debug state fail.",   "��ȡϵͳ���Լ���ʧ��"},
        {E_DM_SET_WARN_SWITCH_FAIL,  "E_DM_SET_WARN_SWITCH_FAIL",     "Set disk warn switch fail.",     "���ô��̸澯����ʧ��"},
        {E_DM_SET_BBLK_LIMIT_FAIL,   "E_DM_SET_WARN_SWITCH_FAIL",     "Set disk bad block limit fail.", "���ô��̸澯����ֵʧ��"},
        {E_DM_SET_RUNNING_STATE_FAIL,"E_DM_SET_RUNNING_STATE_FAIL",   "Set dm running state fail.",     "����ϵͳ����״̬ʧ��"},
        {E_DM_SET_CTRL_UUID_FAIL,    "E_DM_SET_CTRL_UUID_FAIL",       "Set system ctrl UUID fail.",     "����ϵͳ����״̬ʧ��"},
        {E_DM_SET_DEBUG_LEVEL_FAIL,  "E_DM_SET_DEBUG_LEVEL_FAIL",     "Set system debug state fail.",   "����ϵͳ���Լ���ʧ��"},
        {E_DM_GET_TYPE_FAIL,         "E_DM_GET_TYPE_FAIL",            "Get disk type fail.",            "��ȡ��������ʧ��"},
        {E_DM_GET_USEABLE_FAIL,      "E_DM_GET_USEABLE_FAIL",         "Get disk useable fail.",         "��ȡ���̿�����ʧ��"},
        {E_DM_GET_ACCESSABLE_FAIL,   "E_DM_GET_ACCESSABLE_FAIL",      "Get disk accessable fail.",      "��ȡ���̿ɷ�����ʧ��"},
        {E_DM_GET_PD_SCAN_RATE_FAIL, "E_DM_GET_PD_SCAN_RATE_FAIL",    "Get disk scan rate fail.",       "��ȡ����ɨ�����ʧ��"},
        {E_DM_GET_SMART_FAIL,        "E_DM_GET_SMART_FAIL",           "Get disk SMART fail.",           "��ȡ����SMART��Ϣʧ��"},
        {E_DM_GET_GUID_FAIL,         "E_DM_GET_GUID_FAIL",            "Get disk UUID fail.",            "��ȡ����UUIDʧ��"},
        {E_DM_GET_DEVNAME_FAIL,      "E_DM_GET_DEVNAME_FAIL",         "Get disk name fail.",            "��ȡ��������ʧ��"},
        {E_DM_GET_CAPACITY_FAIL,     "E_DM_GET_CAPACITY_FAIL",        "Get disk capacity fail.",        "��ȡ��������ʧ��"},
        {E_DM_GET_PHYID_FAIL,        "E_DM_GET_PHYID_FAIL",           "Get disk phy id fail.",          "��ȡ����Phy��ʶ��ʧ��"},
        {E_DM_GET_JBODCADDR_FAIL,    "E_DM_GET_JBODCADDR_FAIL",       "Get disk jbodc address fail.",   "��ȡ����Jbodc��ַʧ��"},
        {E_DM_GET_BUSID_FAIL,        "E_DM_GET_BUSID_FAIL",           "Get disk bus id fail.",          "��ȡ����bus��ַʧ��"},
        {E_DM_GET_SASADDR_FAIL,      "E_DM_GET_SASADDR_FAIL",         "Get disk SAS address fail.",     "��ȡ����SAS��ַʧ��"},
        {E_DM_GET_CACHE_FAIL,        "E_DM_GET_CACHE_FAIL",           "Get disk cache state fail.",     "��ȡ���̻���״̬ʧ��"},
        {E_DM_GET_MODEL_FAIL,        "E_DM_GET_MODEL_FAIL",           "Get disk model fail.",           "��ȡ�����ͺ�ʧ��"},
        {E_DM_GET_PHY_TYPE_FAIL,     "E_DM_GET_PHY_TYPE_FAIL",        "Get disk phycial type fail.",    "��ȡ��������ӿ�����ʧ��"},
        {E_DM_GET_FW_REV_FAIL,       "E_DM_GET_FW_REV_FAIL",          "Get disk Firmware version fail.", "��ȡ���̹̼��汾��ʧ��"},
        {E_DM_GET_SERIAL_NO_FAIL,    "E_DM_GET_SERIAL_NO_FAIL",       "Get disk serial Number fail.",   "��ȡ�������к�ʧ��"},
        {E_DM_GET_STD_VERSION_FAIL,  "E_DM_GET_STD_VERSION_FAIL",     "Get disk standard version fail.", "��ȡ���̱�׼�汾��ʧ��"},
        {E_DM_SET_PD_TYPE_FAIL,      "E_DM_SET_PD_TYPE_FAIL",         "Set disk type fail.",             "���ô�������ʧ��"},
        {E_DM_SET_PD_USEABLE_FAIL,   "E_DM_SET_PD_USEABLE_FAIL",      "Import disk fail.",               "�������ʧ��"},
        {E_DM_SET_PD_ACCESS_FAIL,    "E_DM_SET_PD_ACCESS_FAIL",       "Set disk accessable fail.",       "���ô��̿ɷ�����ʧ��"},
        {E_DM_SET_CONTROL_PD_SCAN_FAIL, "E_DM_SET_CONTROL_PD_SCAN_FAIL",       "Control disk scan fail.",       "���ƴ���ɨ��ʧ��"},
        {E_DM_SET_SMART_SELFTEST_FAIL,  "E_DM_SET_SMART_SELFTEST_FAIL",        "Exec SMART selftest fail.",      "ִ�д����Լ�ʧ��"},
        {E_DM_SET_CACHE_FAIL,           "E_DM_SET_CACHE_FAIL",                 "Set disk cache state fail.",     "���ô��̻���״̬ʧ��"},
        {E_DM_GET_IO_STATISTICS_FAIL,   "E_DM_GET_IO_STATISTICS_FAIL",       "Get disk IO statistics info fail.", "��ȡ����IOͳ����Ϣʧ��"},
        {E_DM_SET_IO_STATISTICS_FAIL,   "E_DM_SET_IO_STATISTICS_FAIL",       "Set disk IO statistics fail.",      "���ô���IOͳ�ƿ���ʧ��"},
        {E_DM_RESET_IO_STATISTICS_FAIL, "E_DM_RESET_IO_STATISTICS_FAIL",     "Reset disk IO statistics fail.",    "���ô���IOͳ�ƿ���ʧ��"},



        {E_DM_NO_RESOURCE ,               "E_DM_NO_RESOURCE ",                  "No system resource can be used.",          "�Ҳ���������Դ"},
        {E_DM_INVALID_INPUT,              "E_DM_INVALID_INPUT",                 "The input parameter is invalid.",          "���������Ч"},
        {E_DM_CMD_VALUE_ERROR,            "E_DM_CMD_VALUE_ERROR",               "The command value is wrong.",              "������������"},
        {E_DM_MDA_RW_ERROR,               "E_DM_MDA_RW_ERROR",                  "Error in read or write MDA data.",         "��ȡ/д��Ԫ���ݳ���"},
        {E_DM_MDA_CHECKSUM_ERROR,         "E_DM_MDA_CHECKSUM_ERROR",            "Error in get MDA checksun.",               "��ȡ���̸澯����ֵʧ��"},
        {E_DM_MDA_SELRCT_ERROR,           "E_DM_MDA_SELRCT_ERROR",              "Get dm running state fail.",               "��ȡԪ����У��ֵ����"},
        {E_DM_PD_IN_LIST,                 "E_DM_PD_IN_LIST",                    "The PD is found in list.",                 "��ϵͳ���������з��ִ��̽ڵ�"},
        {E_DM_PD_NOT_IN_LIST,             "E_DM_PD_NOT_IN_LIST",                "The PD can not be found in list.",         "�޷��ڴ����������ҵ���Ӧ���̽ڵ�"},
        {E_DM_BEYOND_MAX_PD_NUM ,         "E_DM_BEYOND_MAX_PD_NUM ",            "The number of PD beyond the max value.",   "������Ŀ��������������������ֵ"},
        {E_DM_START_ERROR,                "E_DM_START_ERROR",                   "The dm system start error.",               "Dm ��ϵͳ����ʧ��"},
        {E_DM_STOP_ERROR,                 "E_DM_STOP_ERROR",                    "Set dm stop state fail.",                  "����ϵͳ����ֹͣ״̬ʧ��"},
        {E_DM_MODIFY_PD_SYSINFO_ERROR,    "E_DM_MODIFY_PD_SYSINFO_ERROR",       "Modify PD sysinfo fail.",                  "�޸Ĵ��������Ϣʧ��"},
        {E_DM_GET_STD_VER_ERROR,          "E_DM_GET_STD_VER_ERROR",             "Get system standard version num  fail.",   "���ϵͳ��׼�汾��ʧ��"},
        {E_DM_GET_CAPACITY_VALUE_ERROR,   "E_DM_GET_CAPACITY_VALUE_ERROR",      "Get disk capacity value fail.",            "��ȡ��������ʧ��"},
        {E_DM_GET_FW_VALUE_ERROR ,        "E_DM_GET_FW_VALUE_ERROR ",           "Get disk FW value fail.",                  "��ȡ����FWֵʧ��"},
        {E_DM_GET_CATCH_VALUE_ERROR,      "E_DM_GET_CATCH_VALUE_ERROR",         "Get disk catch state fail.",               "��ȡ���̻���״̬ʧ��"},
        {E_DM_GET_MODEL_VALUE_ERROR,      "E_DM_GET_MODEL_VALUE_ERROR",         "Get disk model name fail.",                "��ȡ��������ʧ��"},
        {E_DM_GET_SERIAL_VALUE_ERROR,     "E_DM_GET_SERIAL_VALUE_ERROR",        "Get disk serial num fail.",                "��ȡ�������к�ʧ��"},
        {E_DM_GET_SAS_VALUE_ERROR,        "E_DM_GET_SAS_VALUE_ERROR",           "Get disk SAS address fail.",               "��ȡ����SAS ��ַʧ��"},
        {E_DM_SOCKET_INIT_ERROR,          "E_DM_SOCKET_INIT_ERROR",             "Socket init fail.",                        "Socket ��ʼ��ʧ��"},
        {E_DM_SOCKET_SET_NUM_ERROR  ,     "E_DM_SOCKET_SET_NUM_ERROR  ",        "Set socket serial number fail.",           "���ÿͻ���Socket�������к�ʧ��"},
        {E_DM_SOCKET_BIND_ERROR,          "E_DM_SOCKET_BIND_ERROR",             "Bind socket address fail.",                "��socket��ַʧ��"},
        {E_DM_SOCKET_SET_ERROR,           "E_DM_SOCKET_SET_ERROR",              "Set socket attribute fail.",               "����socket ����ʧ��"},
        {E_DM_SOCKET_SEND_ERROR,          "E_DM_SOCKET_SEND_ERROR",             "Send value through socket fail.",          "����socket����ʧ��"},
        {E_DM_SOCKET_RECEV_ERROR ,        "E_DM_SOCKET_RECEV_ERROR ",           "Receive socket value fail.",               "��ȡ����SAS��ַʧ��"},
        {E_DM_PD_GET_SMART_ERROR,         "E_DM_PD_GET_SMART_ERROR",            "Get disk SMART fail.",                     "��ȡ����SMART��Ϣʧ��"},
        {E_DM_SET_SMART_SELFTEST_ERROR,   "E_DM_SET_SMART_SELFTEST_ERROR",      "Set disk SMART fail.",                     "���ô���SMART��Ϣʧ��"},

        {E_DM_PD_GET_SMART_DURING_SCAN_ERROR,     "E_DM_PD_GET_SMART_DURING_SCAN_ERROR",        "Refusing Get Smart Info during disk scanning.",                "��������ɨ�裬�������ȡSMART��Ϣ"},
        {E_DM_SET_SMART_SELFTEST_DURING_SCAN_ERROR,        "E_DM_SET_SMART_SELFTEST_DURING_SCAN_ERROR",           "Refusing Executing Smart SelfTest command  during scanning disk.",               "��������ɨ�裬������ִ��SMART�Լ�"},
        {E_DM_SCAN_DATA_PD_FAIL   ,     "E_DM_SCAN_DATA_PD_FAIL ",        "Refusing scanning disk of data type.",           "�������������ִ�д���ɨ��"},
        {E_DM_SCAN_UNKNOWN_PD_FAIL   ,     "E_DM_SCAN_UNKNOWN_PD_FAIL",        "Refusing scanning disk of unknown  type.",           "�������δ֪��ִ�д���ɨ��"},
        {E_DM_SMART_DATA_PD_FAIL,          "E_DM_SMART_DATA_PD_FAIL",             "Refusing smart operation with disk of data type.",                "�������������ִ��SMART�Լ��"},
        {E_DM_SET_DISK_DATA_TYPE_DURING_SCAN_ERROR,           "E_DM_SET_DISK_DATA_TYPE_DURING_SCAN_ERROR",              "Refusing set disk type to data type during disk scanning",               "��������ɨ��,�������޸Ĵ���Ϊ������"},
        {E_DM_SET_DISK_UNKNOWN_TYPE_DURING_SCAN_ERROR,           "E_DM_SET_DISK_UNKNOWN_TYPE_DURING_SCAN_ERROR",              "Refusing set disk type to unknown type during disk scanning",               "��������ɨ��,�������޸Ĵ���Ϊδ֪��"},

        /*******************************BLK ��ϵͳ************************************/
        {E_VDS_NULLP,           "E_VDS_NULLP",      "There is no effective resources.", "û����Ч����Դ��"},    /*��ָ�� */
        {E_VDS_PARAM_INVALID,    "E_VDS_PARAM_INVALID",     "Invalid parameters.",  "��Ч������"},          /*�����Ƿ� */
        {E_VDS_NO_VD_MOD,     "E_VDS_NO_VD_MOD",     "The module of virtual disk has not been loaded.", "������ģ��δ���ء�"},          /*VDģ��δ���� */
        {E_VDS_NO_VOL_MOD,     "E_VDS_NO_VOL_MOD",     "The module of volume has not been loaded.",  "��ģ��δ���ء�"},          /*VOL ģ��δ����*/
        {E_VDS_NO_SNAP_MOD,     "E_VDS_NO_SNAP_MOD",     "The module of snapshot has not been loaded",  "����ģ��δ���ء�"},          /*SNAP ģ��δ����*/
        {E_VDS_NO_VD,      "E_VDS_NO_VD",      "The specified virtual disk does not exist.",    "ָ���������̲����ڡ�"},          /*ָ����vd ������ */
        {E_VDS_NO_VOL,      "E_VDS_NO_VOL",      "The specified volume does not exist.",      "ָ���ľ����ڡ�"},          /*ָ����vol ������ */
        {E_VDS_NO_SNAP,     "E_VDS_NO_SNAP",      "The specified snapshot does not exist.",      "ָ���Ŀ��ղ����ڡ�"},          /*ָ����snap ������ */
        {E_VDS_VD_EXCEED,     "E_VDS_VD_EXCEED",     "Virtual disk number exceeds the allowed limit system.", "�����̵���������ϵͳ��������ޡ�"},          /*vd ���� ����������� */
        {E_VDS_VOL_EXCEED,     "E_VDS_VOL_EXCEED",     "Volume number exceeds the allowed limit system.",  "�������Ͼ����������ϵͳ��������ޡ�"},          /*vol ���� ����������� */
        {E_VDS_SNAP_EXCEED,    "E_VDS_SNAP_EXCEED",     "Snapshot number exceeds the allowed limit system.",  "������������ϵͳ��������ޡ�"},          /*snap ���� ����������� */
        {E_VDS_VD_EXIST,     "E_VDS_VD_EXIST",     "Virtual Disk with the same name already exists, please change the name and retry.",   "ͬ�����������Ѵ��ڣ�������������´�����"},           /*ָ����vd �Ѵ��ڣ����ڴ���ʱ */

        {E_VDS_OBJECT_EXIST,     "E_VDS_OBJECT_EXIST",    "The  name used here already exists, please change the name and retry.",    "�˴�ʹ�õ������Ѵ��ڣ������ԡ�"},         /*����clone��snap��volʱϵͳ�д���ͬ����ͬ����� */
        {E_VDS_SNAP_EXIST,     "E_VDS_SNAP_EXIST",     "Snapshot with the same name already exists, please change the name and retry.",    "ͬ���Ŀ����Ѵ��ڣ�������������´�����"},         /*ָ����snap �Ѵ��ڣ����ڴ���ʱ */
        {E_VDS_VOL_BUSY,     "E_VDS_VOL_BUSY",     "Volume is busy. Unable to execute the command.",   "����æ���޷�ִ�����"},          /*vol ��æ, �޷�ִ������  */
        {E_VDS_VOL_NO_READY,    "E_VDS_VOL_NO_READY",    "Volume is not ready.",        "��δ׼��������"},          /*vol û׼���ã�һ��ָ״̬���������*/

        {E_VDS_VD_RELATION_EXIST,    "E_VDS_VD_RELATION_EXIST",     "The device has relation with others,not allowed to freeze", "�������������������й�������������"},
        {E_VDS_VD_REJECTED_FREEZED,    "E_VDS_VD_REJECTED_FREEZED",     "Virtual Disk is not at the state of good or degrade,not allowed to freeze", "�����̵�ǰ״̬�������ᣬֻ��״̬Ϊ���û��߽����������̲�������"},         /*vd״̬�������ͽ��� */
        {E_VDS_VOL_DEV_BUSY,   "E_VDS_VOL_DEV_BUSY",    "The device of Volume is busy.",     "���豸��æ��"},         /*vol�豸����æ */
        {E_VDS_VD_NOT_GOOD,   "E_VDS_VD_NOT_GOOD",    "Operation not supported on this state of Virtual Disk.",  "�����̵�ǰ״̬��֧�ָò�����"},         /*VD״̬��ΪGOOD */
        {E_VDS_VOL_NOT_GOOD,   "E_VDS_VOL_NOT_GOOD",    "The state of Volume is not good,not allowed to do  the operation.",    "�������״̬��������ò�����"},         /*VOL״̬��ΪGOOD */
        {E_VDS_VD_DEGRADE,    "E_VDS_VD_DEGRADE",     "VD is degraded, unable to execute cmd.",  "�����̽������޷�ִ������"},         /*VD �����˽��� */
        {E_VDS_WRONG_CHUNKSIZE,   "E_VDS_WRONG_CHUNKSIZE",   "Invalid chunk-size.",       "��Ч��������С��"},         /*chunk_size��С�Ƿ� */
        {E_VDS_OPEN_FAIL,    "E_VDS_OPEN_FAIL",     "Cannot open this file.",     "���ļ�ʧ�ܡ�"},         /*���ļ�ʧ�� */
        {E_VDS_MK_NOD_FAIL,    "E_VDS_MK_NOD_FAIL",    "Failed to create node.",      "mknod�����ļ�ʧ�ܡ�"},         /*����mknoʧ��   */
        {E_VDS_VOL_TYPE,    "E_VDS_VOL_TYPE",     "Invalid Volume type.",     "��Ч�ľ����͡�"},         /*�����ͷǷ� */
        {E_VDS_CAPACITY,     "E_VDS_CAPACITY",     "Invalid capacity.",     "��Ч��������"},         /*�����Ƿ� */
        {E_VDS_NAME_INVALID,    "E_VDS_NAME_INVALID",     "Invalid name specified.",    "����У�鲻ͨ����"},         /*����У�鲻ͨ�� */
        {E_VDS_RAID_LEVEL,    "E_VDS_RAID_LEVEL",     "Operation not supported on Virtual Disk of current level.","��������raid����֧�ֵ�ǰ������"},         /*������֧�ֶ�Ӧ��raid ���� */

        {E_VDS_BAD_REBUILD_MODE,  "E_VDS_BAD_REBUILD_MODE",   "Invalid parameter of auto-build mode.",    "��Ч���ؽ�ģʽ��"},         /*������ؽ�ģʽ����*/
        {E_VDS_VD_DEL_REJECT,   "E_VDS_VD_DEL_REJECT",    "Volumes exist. Unable to delete Virtual Disk.", "���������Դ��ھ�������ɾ��ʧ�ܡ�"},         /*�����������߼��̣�������ɾ��*/
        {E_VDS_PD_SET_INVALID,   "E_VDS_PD_SET_INVALID",    "Invalid number of disks for Virtual Disk.",  "���ڴ��������̵Ĵ����������󡣡�"},         /*������������*/
        {E_VDS_USER_MEM_ALLOC,   "E_VDS_USER_MEM_ALLOC",    "User memory allocation error.",     "�û�̬�ڴ����ʧ�ܡ�"},         /*�û�̬�ڴ����ʧ��*/
        {E_VDS_KERNEL_MEM_ALLOC,  "E_VDS_KERNEL_MEM_ALLOC",   "Kernel memory allocation error.",     "�ں�̬�ڴ����ʧ�ܡ�"},         /*�ں�̬�ڴ����ʧ��*/
        {E_VDS_DEV_NOT_EXIST,   "E_VDS_DEV_NOT_EXIST",    "The device does not exist.",      "���豸�����ڡ�"},         /*�豸������ */
        {E_VDS_SYS_NOT_EXIST,   "E_VDS_SYS_NOT_EXIST",    "The sysfs file does not exist.",    "��sysfs�ļ������ڡ�"},         /*sysfs�ļ������� */
        {E_VDS_NO_PERMISSION,   "E_VDS_NO_PERMISSION",    "Permission denied.",       "��Ȩ�ޡ�"},         /*��Ȩ�� */
        {E_VDS_DISK_TYPE_INVAL,   "E_VDS_DISK_TYPE_INVAL",    "Invalid type of member disk.",   "��Ч�ĳ�Ա�����͡�"},         /*��Ա�����ͷǷ� */
        {E_VDS_VD_NOSPC,    "E_VDS_VD_NOSPC",     "The rops of Virtual Disk is not NULL.",   "�����̵�rops��Ϊ�ա�"},         /*vd->rops ��Ϊ��*/

        {E_VDS_VD_SYS_REGISTER,   "E_VDS_VD_SYS_REGISTER",    "Failed to register in sysfs.",   "��sysfsע��ʧ�ܡ�"},         /* ��sysfs ע��ʧ�� */
        {E_VDS_RAID_NO_MOD,    "E_VDS_RAID_NO_MOD",     "The module of Virtual Disk of current level has not been loaded.",    "raidģ��δ���ء�"},         /*raid ģ��δ���� */
        {E_VDS_RAID_RUN_FAIL,   "E_VDS_RAID_RUN_FAIL",    "Failed to run Virtual Disk of current level.", "raidģ������ʧ�ܡ�"},         /*raid ģ������ʧ��*/
        {E_VDS_PTHREAD_CREATE,   "E_VDS_PTHREAD_CREATE",    "Failed to create pthread.",     "�����߳�ʧ�ܡ�"},         /*����pthreadʧ�� */
        {E_VDS_PTHREAD_DESTROY,   "E_VDS_PTHREAD_DESTROY",    "Failed to destroy pthread.",    "�����߳�ʧ�ܡ�"},         /*����pthreadʧ�� */
        {E_VDS_THREAD_NOTEXIST,   "E_VDS_THREAD_NOTEXIST",    "The thread does not exist.",    "���̲߳����ڡ�"},         /*�����̲߳����� */
        {E_VDS_THREAD_EXIST,    "E_VDS_THREAD_EXIST",    "The thread task already exists related to this operation,not allowed this command here.", "��ò�����ص������Ѵ��ڣ�������ò�����"},         /*�߳��Ѵ��ڣ������ظ����� */
        {E_VDS_THREAD_CREATE,   "E_VDS_THREAD_CREATE",    "Failed to register the thread.",     "ע���߳�ʧ�ܡ�"},         /*�̴߳����ǳ��� */
        {E_VDS_THREAD_SYNC,    "E_VDS_THREAD_SYNC",     "Failed to synchronize the thread.",   "�߳�ͬ��ʧ�ܡ�"},         /*�߳�ͬ������ */
        {E_VDS_DEVNO_TO_UUID,   "E_VDS_DEVNO_TO_UUID",    "Failed to change the device-number into uuid.", "�豸��ת��uuidʧ�ܡ�"},         /*�豸��ת����uuid����*/
        {E_VDS_UUID_TO_DEVNO,   "E_VDS_UUID_TO_DEVNO",    "Failed to change the uuid into device-number.", "uuidת���豸��ʧ�ܡ�"},         /*uuidת�����豸�Ŵ���*/
        {E_VDS_WAIT_TIME_OUT,   "E_VDS_WAIT_TIME_OUT",    "The peer response timeout.",     "�ȴ��Զ˳�ʱ��"},         /*�ȴ��Զ˳�ʱ*/
        {E_VDS_RVOL_CAPACITY_PERCENTAGE, "E_VDS_RVOL_CAPACITY_PERCENTAGE", "The percentage of capacity of repository volume to base volume should be 10%~100%.", "������Դ�ռ�Ĵ�СӦ����Դ���С��10%~100%֮�䡣"},         /*ʵ���Ӧ��ռԴ��İٷֱ���10%~100%֮��*/
        {E_VDS_MSG_LENGTH,    "E_VDS_MSG_LENGTH",     "Invalid message length.",      "��Ч����Ϣ���ȡ�"},         /*��Ϣ��������*/

        {E_VDS_BVOL_NO_EXIST,   "E_VDS_BVOL_NO_EXIST",    "Base volume does not exist.",      "Դ�����ڡ�"},         /*Դ������*/
        {E_VDS_SVOL_INIT_FAIL,   "E_VDS_SVOL_INIT_FAIL",    "Failed to initiate snapshot volume.",   "���վ��ʼ��ʧ�ܡ�"},         /*���վ��ʼ��ʧ��*/
        {E_VDS_RVOL_INIT_FAIL,   "E_VDS_RVOL_INIT_FAIL",    "Failed to initiate repository volume.",   "������Դ�ռ��ʼ��ʧ�ܡ�"},         /*ʵ����ʼ��ʧ��*/
        {E_VDS_EXCEPTIONSTORE_CREATE_FAIL, "E_VDS_EXCEPTIONSTORE_CREATE_FAIL",   "Failed to create exception store.", "����ֿⴴ��ʧ�ܡ�"},         /*����ֿⴴ��ʧ��*/
        {E_VDS_HASHTABLE_INIT_FAIL,  "E_VDS_HASHTABLE_INIT_FAIL",   "Failed to initiate hash table.",   "��ϣ���ʼ��ʧ�ܡ�"},         /*��ϣ���ʼ��ʧ��*/
        {E_VDS_SVOL_INVALID,    "E_VDS_SVOL_INVALID",    "Invalid snapshot volume.",      "��Ч�Ŀ��վ�"},         /*���վ���Ч*/
        {E_VDS_SVOL_WRITE_STATE,  "E_VDS_SVOL_WRITE_STATE",   "The snapshot volume has been written.",    "�ÿ��վ��ѱ���д��"},         /*���վ�д��*/
        {E_VDS_PENDING_EXCEPTIONS_RUNNING,  "E_VDS_PENDING_EXCEPTIONS_RUNNING",   "The pending exceptions are being processed.",   "pending�����¼�����ڴ����С�"},         /*PENDING�����¼�����ڴ�����*/
        {E_VDS_SVOL_REGISTERTHREAD_FAIL,   "E_VDS_SVOL_REGISTERTHREAD_FAIL",   "Failed to register thread for snapshot volume.", "���վ�ע���߳�ʧ�ܡ�"},         /*ע���߳�ʧ��*/
        {E_VDS_PSDMIOCLIENT_CREATE_FAIL,   "E_VDS_PSDMIOCLIENT_CREATE_FAIL",   "Failed to create dm_io_client for pstore.",  "pstore�ṹ���ԱDM_IO_CLIENT��ַ����ʧ�ܡ�"},         /*pstore�ṹ�� ��ԱDM_IO_CLIENT�����ַ����*/
        {E_VDS_PS_AREA_ALLOC_FAIL,    "E_VDS_PS_AREA_ALLOC_FAIL",    "Failed to allocate area for pstore.", "pstore�ṹ���Աarea�ռ����ʧ�ܡ�"},         /*pstore�ṹ�� ��Աarea ����ռ�ʧ��*/
        {E_VDS_PS_HEAD_AREA_ALLOC_FAIL,    "E_VDS_PS_HEAD_AREA_ALLOC_FAIL",    "Failed to allocate header_area for pstore.", "pstore�ṹ���Աheader_area����ʧ�ܡ�"},         /*pstore�ṹ�� ��Աheader_area����ռ�ʧ��*/
        {E_VDS_PS_CALLBACKS_ALLOC_FAIL,   "E_VDS_PS_CALLBACKS_ALLOC_FAIL",   "Failed to allocate callbacks for pstore.",  "pstore�ṹ���Աcallbacks�ռ����ʧ�ܡ�"},         /*pstore�ṹ�� ��Աcallbacks����ռ�ʧ��*/
        {E_VDS_SNAP_RUN_ERROR,     "E_VDS_SNAP_RUN_ERROR",     "Failed to modify the access port for base volume.", "�޸�Դ����ʽӿ�ʧ�ܡ�"},         /*�޸�Դ����ʽӿ�ʧ��*/
        {E_VDS_ZERO_DISK_AREA_FAIL,    "E_VDS_ZERO_DISK_AREA_FAIL",    "Failed to clear the area for COW recording.",  "����COW��¼������ʧ�ܡ�"},         /*����cow��¼������ʧ��*/

        {E_VDS_SOCKET_CREATE,     "E_VDS_SOCKET_CREATE",     "Failed to create socket.",       "�׽��ִ���ʧ�ܡ�"},         /*socket ����ʧ�� */
        {E_VDS_INIT_FAIL,       "E_VDS_INIT_FAIL",      "Failed to initiate user mode of block.",   "block�û�̬��ʼ��ʧ�ܡ�"},         /*blk �û�̬��ʼ��ʧ��*/
        {E_VDS_CDEV_REGISTER,    "E_VDS_CDEV_REGISTER",     "Failed to register the char-device of vdk.",   "ע��vdk�ַ��豸ʧ�ܡ�"},         /*vdk���ַ��豸ע��ʧ�� */
        {E_VDS_CDEV_INIT,      "E_VDS_CDEV_INIT",      "Failed to initiate the char-device of vdk.",   "��ʼ��vdk�ַ��豸ʧ�ܡ�"},         /*vdk���ַ��豸��ʼ��ʧ��*/
        {E_VDS_TYPE_INVALID,                 "E_VDS_TYPE_INVALID",           "obj(vd,vol,snap) type invalid.",   "���������̣������գ����ʹ���"},
        {E_VDS_NOT_CONSISTENT,               "E_VDS_NOT_CONSISTENT",         "dual control data not consistent.",  "˫�����ݲ�һ�¡�"},
        {E_VDS_RVOL_NOT_EXIST,   "E_VDS_RVOL_NOT_EXIST",    "The repository volume does not exist.",   "������Դ�ռ䲻���ڡ�"},
        {E_VDS_SAME_VD,                  "E_VDS_SAME_VD",                 "The base volume and clone volume use the same Virtual Disk.The base volume and clone volume cannot create on the same Virtual Disk.",      "Դ��Ϳ�¡��������������ͬ��Դ��Ϳ�¡������ͬһ��VD�ϴ�����"},
        {E_VDS_CLONE_MAX,               "E_VDS_CLONE_MAX",                 "The number of clone exceeds the maximum number.",     "�þ�Ŀ�¡�������������ޡ�"},
        {E_VDS_CLONE_UNBIND,            "E_VDS_CLONE_UNBIND",              "There is no clone bind",       "û�б��󶨵Ŀ�¡��"},
        {E_VDS_CLONE_UNFRACTURED,       "E_VDS_CLONE_UNFRACTURED",        "The clone is UNFRACTURED and operation not supported.",      "��¡��ǰΪ������ϵ����֧�ָò�����"},
        {E_VDS_WRONG_REBUILD_LEVEL,    "E_VDS_WRONG_REBUILD_LEVEL",          "The Virtual Disk with level 0 cannot rebuild itself.",    "raid0������������޷��Զ��ؽ���"},
        {E_VDS_DISK_RAID0 ,   "E_VDS_DISK_RAID0",    "The disk number must be between 1 and 32 when creating raid0 vd.",    "����raid0����������ʱ����������Ӧ����1~32֮�䡣"},
        {E_VDS_DISK_RAID1 ,   "E_VDS_DISK_RAID1",    "The disk number must be 2 when creating raid1 vd.",       "����raid1����������ʱ����������ӦΪ2��"},
        {E_VDS_DISK_RAID10 ,   "E_VDS_DISK_RAID10",    "The disk number must be an even number between 4 and 32 when creating raid10 vd.","����raid10����������ʱ����������ӦΪ����4~32��ż����"},
        {E_VDS_DISK_RAID5 ,   "E_VDS_DISK_RAID5",    "The disk number must be between 3 and 16 when creating raid5 vd.",    "����raid5����������ʱ����������Ӧ����3~16֮�䡣"},
        {E_VDS_DISK_RAID6 ,   "E_VDS_DISK_RAID6",    "The disk number must be between 4 and 16 when creating raid6 vd.",    "����raid6����������ʱ����������Ӧ����4~16֮�䡣"},
        {E_VDS_RVOL_THR ,   "E_VDS_RVOL_THR",    "The repository volume thr must be between 50% and 95%.",    "ʵ���ֵ����50%~95%֮�䡣"},
        {E_VDS_BVOL_RESTORING_STATE ,   "E_VDS_BVOL_RESTORING_STATE",    "The base volume has been restoring.",    "Դ�����ڻع��У��Ժ����ԡ�"},
        {E_VDS_AUTOREBUILD_ALREADY_OFF ,   "E_VDS_AUTOREBUILD_ALREADY_OFF",    "Vd autorebuild is already OFF.",    "�����̵��Զ��ؽ�״̬��Ϊ�رա�"},
        {E_VDS_VOL_DEL_REJECT ,   "E_VDS_VOL_DEL_REJECT",    "Snapshot,Clone volume  or rvol exist. Unable to delete base volume.", "Դ�����п��վ���¡�����Դ�ռ�ʱ��Դ��ɾ��ʧ�ܡ�"},
        {E_VDS_VD_FAULT,   "E_VDS_VD_FAULT",     "VD is fault, unable to execute cmd.", "�����̹��ϣ��޷�ִ�����"},
        {E_VDS_NOTSUPPORT,   "E_VDS_NOTSUPPORT",     "Now not support this operation.", "Ŀǰ��֧�ָò�����"},
        {E_VDS_NOT_LOCAL,   "E_VDS_NOT_LOCAL",   "The virtual disk is frozen.", "���������Ѿ����ᡣ"},
        {E_VDS_NOT_FOREIGN,   "E_VDS_NOT_FOREIGN",   "The virtual disk is not foreign.", "�������̲��������ġ�"},
        {E_VDS_IMMIGRATE_FAIL,   "E_VDS_IMMIGRATE_FAIL",     "The virtual disk immigrate fail.", "��������Ǩ��ʧ�ܡ�"},
        {E_VDS_NOT_IMMIGRATE,   "E_VDS_NOT_IMMIGRATE",   "The virtual disk has not immigrate.", "�������̻�û��Ǩ�룬��ʱ�޷�ʹ�á�"},
        {E_VDS_VD_RECING  ,   "E_VDS_VD_RECING ",    "Virtual Disk is rebuilding, not support delete operation.", "�����������ؽ�����֧��ɾ��������"},
        {E_VDS_VD_CREATE_FAIL,   "E_VDS_VD_CREATE_FAIL",    "Failed to create Virtual Disk.",  "����������ʧ�ܡ�"},
        {E_VDS_VOL_CREATE_FAIL,   "E_VDS_VOL_CREATE_FAIL",    "Failed to create Volume.",  "������ʧ�ܡ�"},
        {E_VDS_SNAP_CREATE_FAIL,   "E_VDS_SNAP_CREATE_FAIL",    "Failed to create SnapShot Volume.",  "�������վ�ʧ�ܡ�"},
        {E_VDS_CLONE_CREATE_FAIL,   "E_VDS_CLONE_CREATE_FAIL",    "Failed to create Clone Volume.",  "������¡��ʧ�ܡ�"},
        {E_VDS_VD_DELETE_FAIL,   "E_VDS_VD_DELETE_FAIL",    "Failed to delete Virtual Diskl.",  "ɾ��������ʧ�ܡ�"},
        {E_VDS_VOL_DELETE_FAIL,   "E_VDS_VOL_DELETE_FAIL",    "Failed to delete Volume.",  "ɾ����ʧ�ܡ�"},
        {E_VDS_SNAP_DELETE_FAIL,   "E_VDS_SNAP_DELETE_FAIL",    "Failed to delete SnapShot Volume.",  "ɾ�����վ�ʧ�ܡ�"},
        {E_VDS_CLONE_DELETE_FAIL,   "E_VDS_CLONE_DELETE_FAIL",    "Failed to delete Clone Volume.",  "ɾ����¡��ʧ�ܡ�"},
        {E_VDS_B_VD_RECING  ,   "E_VDS_B_VD_RECING ",  "Virtual Disk of base volume is rebuilding, cannot delete clone.", "Դ��������������ؽ�������ɾ���ÿ�¡��"},
        {E_VDS_CLONE_BUSY,     "E_VDS_CLONE_BUSY",       "The clone is synchronizing or reverse-synchronizing. Unable to execute this command.",  "��¡������ͬ����ͬ�����޷�ִ�и����"},
        {E_VDS_OPEN_DIR_FAIL,    "E_VDS_OPEN_DIR_FAIL",     "Cannot open this folder.",     "���ļ���ʧ�ܡ�"},         /*���ļ���ʧ�� */
        {E_VDS_BVOL_RSYNC,    "E_VDS_BVOL_RSYNC",    "The base volume is reverse-synchronizing. Unable to execute this command.",   "Դ�����ڽ��з�ͬ�����޷�ִ�и����"},
        {E_VDS_VD_NOT_RECING, "E_VDS_VD_NOT_RECING", "The Virtual Disk is not recovering!", "�����̲����ؽ�״̬��"},
        {E_VDS_DEVNO_INVALID,"E_VDS_DEVNO_INVALID","The devno is repetitive ","�����ظ����豸��"},
        {E_VDS_DEV_ALREADY_EXIST,"E_VDS_DEV_ALREADY_EXIST","The devicefile has been already created! ","��ͬid ���豸�ļ��Ѿ���������"},
        {E_VDS_PEER_TIMEOUT,    "E_VDS_PEER_TIMEOUT",    "The peer ack is timeout.",   "�Զ˳�ʱ��"},
        {E_VDS_PEER_FAIL,    "E_VDS_PEER_FAIL",    "The peer operation is faild.",   "�Զ˲���ʧ�ܡ�"},
        {E_VDS_VD_REJECTED_IMMIGRATED,    "E_VDS_VD_REJECTED_IMMIGRATED",     "Virtual Disk is not at the state of good or degrade,not allowed to be immigrated", "�����̵�ǰ״̬�ǽ����������ã�������ִ��Ǩ�����"},         /*vd״̬�������ͽ��� */

    {E_VDS_VD_LOCALREJECTED_IMMIGRATED,    "E_VDS_VD_LOCALREJECTED_IMMIGRATED",     "Virtual Disk is a local freezed vd,not allowed to be immigrated", "�������Ǳ��ض��������̣�������ִ��Ǩ�����"},
        {E_VDS_VD_NO_DEGRADE,    "E_VDS_VD_NO_DEGRADE",     "The state of the Virtual Disk is not DEGRADED, cannot be rebuilded.", "�����̷ǽ���״̬���������ؽ�"},
        {E_VDS_SPAREDISK_NUM,    "E_VDS_SPAREDISK_NUM",     "The number of hotspare disks for recovery is wrong.", "Ϊ�ؽ����ṩ���ȱ�����Ŀ����"},

        {E_VDS_GET_CLONE_INFO_FAIL,    "E_VDS_GET_CLONE_INFO_FAIL",     "Get clone info failed.", "��ȡ��¡����Ϣʧ��"},
        {E_VDS_SET_CLONE_PRIORITY_FAIL,    "E_VDS_SET_CLONE_PRIORITY_FAIL",     "Set clone priority failed.", "���ÿ�¡��priorityʧ��"},
        {E_VDS_SET_CLONE_PROTECT_FAIL,    "E_VDS_SET_CLONE_PROTECT_FAIL",     "Set clone protect failed.", "���ÿ�¡��protectʧ��"},
        {E_VDS_SET_CLONE_NAME_FAIL,    "E_VDS_SET_CLONE_NAME_FAIL",     "Set clone name failed.", "��¡��������ʧ��"},
        {E_VDS_CLONE_FRACTURE_FAIL,    "E_VDS_CLONE_FRACTURE_FAIL",     "Clone fracture failed.", "��¡�����ʧ��"},
        {E_VDS_CLONE_SYNC_FAIL,    "E_VDS_CLONE_SYNC_FAIL",     "Clone sync failed.", "��¡��ͬ��ʧ��"},
        {E_VDS_CLONE_REVERSE_SYNC_FAIL,    "E_VDS_CLONE_REVERSE_SYNC_FAIL",     "Clone reverse sync failed.", "��¡��ͬ��ʧ��"},
        {E_VDS_GET_SVOL_INFO_FAIL,    "E_VDS_GET_SVOL_INFO_FAIL",     "Get snap info failed.", "��ȡ���վ���Ϣʧ��"},
        {E_VDS_SET_SVOL_NAME_FAIL,    "E_VDS_SET_SVOL_NAME_FAIL",     "Set snap name failed.", "���վ�������ʧ��"},
        {E_VDS_SET_RVOL_FAIL,    "E_VDS_SET_RVOL_FAIL",     "Set snap repository failed.", "���մ洢�ռ�澯��ֵ����ʧ��"},
        {E_VDS_SVOL_RESTORE_FAIL,    "E_VDS_SVOL_RESTORE_FAIL",     "Snap restore failed.", "���ݻָ�ʧ��"},
        {E_VDS_GET_VD_INFO_BY_NAME_FAIL,    "E_VDS_GET_VD_INFO_BY_NAME_FAIL",     "Get vd info failed.", "��ȡ��������Ϣʧ��"},
        {E_VDS_SET_VD_NAME_FAIL,    "E_VDS_SET_VD_NAME_FAIL",     "Set vd name failed.", "������������ʧ��"},
        {E_VDS_SET_VD_PRECTL_FAIL,    "E_VDS_SET_VD_PRECTL_FAIL",     "Set vd prectl failed.", "������prefer ctl����ʧ��"},
        {E_VDS_SET_VD_OWNERCTL_FAIL,    "E_VDS_SET_VD_OWNERCTL_FAIL",     "Set vd ownerctl failed.", "������owner ctl����ʧ��"},
        {E_VDS_SET_VD_AUTO_REBUILD_FAIL,    "E_VDS_SET_VD_AUTO_REBUILD_FAIL",     "Set vd auto rebuild failed.", "�������Զ��ؽ�����ʧ��"},
        {E_VDS_VD_FREEZE_FAIL,    "E_VDS_VD_FREEZE_FAIL",     "Vd freeze failed.", "�����̶���ʧ��"},
        {E_VDS_VD_UNFREEZE_FAIL,    "E_VDS_VD_UNFREEZE_FAIL",     "Vd unfreeze failed.", "�����̽ⶳʧ��"},
        {E_VDS_VOL_RENAME_FAIL,    "E_VDS_VOL_RENAME_FAIL",     "Vol rename failed.", "��������ʧ��"},
        {E_VDS_GET_VOL_INFO_FAIL,    "E_VDS_GET_VOL_INFO_FAIL",     "Get vol info failed.", "��ȡ����Ϣʧ��"},
        {E_VDS_GET_VOL_STAT_FAIL,    "E_VDS_GET_VOL_STAT_FAIL",     "Get vol stat failed.", "���þ�״̬ʧ��"},
        {E_VDS_CLEAN_VOL_STAT_FAIL,    "E_VDS_CLEAN_VOL_STAT_FAIL",     "Clean vol stat failed.", "�����״̬ʧ��"},
        {E_VDS_SET_VOL_PRECTL_FAIL,    "E_VDS_SET_VOL_PRECTL_FAIL",     "Set vol prectl failed.", "���þ�prefer ctlʧ��"},
        {E_VDS_SET_VOL_OWNERCTL_FAIL,    "E_VDS_SET_VOL_OWNERCTL_FAIL",     "Set vol ownerctl failed.", "���þ�owner ctlʧ��"},
        {E_VDS_VD_CAPACITY_NOT_ENOUTH,    "E_VDS_VD_CAPACITY_NOT_ENOUTH",     "The capacity of vd is not enouth.", "��������������"},
        {E_VDS_WRITE_FILE_FAIL,    "E_VDS_WRITE_FILE_FAIL",     "Write file failed.", "д�ļ�ʧ��"},
        {E_VDS_CLONE_FRACTURED,    "E_VDS_CLONE_FRACTURED",     "Clone is fractured.", "��¡���ڷ���״̬"},
         {E_VDS_SVOL_RESTORING_STATE,  "E_VDS_SVOL_RESTORING_STATE",   "The snapshot volume has been restoring,not allowed to restore again.",    "�ÿ��վ����ڻָ��У��������ٴλָ���"},
         {E_VDS_RVOL_NO_SPACE,  "E_VDS_RVOL_NO_SPACE",   "The snap repository no space!",    "������Դ�ռ�������"},
        {E_VDS_B_VD_NOTGOOD,   "E_VDS_B_VD_NOTGOOD ",  "Virtual Disk of base volume is not good, cannot create clone.", "Դ���������״̬�������ã����ܴ�����¡��"},
        {E_VDS_MIRROR_CREATE_FAIL, "E_VDS_MIRROR_CREATE_FAIL", "Failed to create mirror.", "����Զ�̾���ʧ��"},
        {E_VDS_MIRROR_NOT_EXIST, "E_VDS_MIRROR_NOT_EXIST", "mirror not exist.", "Զ�̾��񲻴���"},
        {E_VDS_MIRROR_GETINFO_FAIL, "E_VDS_MIRROR_GETINFO_FAIL", "Failed to get mirror info", "��ȡԶ�̾�����Ϣʧ��"},
        {E_VDS_MIRROR_EXIST, "E_VDS_MIRROR_EXIST", "mirror is already exist.", "Զ�̾����Ѿ�����"},
        {E_VDS_MIRROR_NO_SRC, "E_VDS_MIRROR_NO_SRC", "Failed to get mirror source vol", "��ȡԶ�̾���Դ��ʧ��"},
        {E_VDS_MIRROR_CAP, "E_VDS_MIRROR_CAP", "capacity of mirror dest vol is not equal to src vol", "Զ�̾����Ŀ�ľ�������С��Դ�����"},
        {E_VDS_MIRROR_SRC_HAS, "E_VDS_MIRROR_SRC_HAS", "src vol already has mirror.", "Դ���ѽ�����Զ�̾���"},
        {E_VDS_MIRROR_SETNAME_FAIL, "E_VDS_MIRROR_SETNAME_FAIL", "Failed to set mirror name.", "Զ�̾���������ʧ��"},
        {E_VDS_MIRROR_SETPOLICY_FAIL, "E_VDS_MIRROR_SETPOLICY_FAIL", "Failed to set mirror policy.", "����Զ�̾���ָ�����ʧ��"},
        {E_VDS_MIRROR_SETPRIORITY_FAIL, "E_VDS_MIRROR_SETPRIORITY_FAIL", "Failed to set mirror priority.", "����Զ�̾���ͬ�����ȼ�ʧ��"},
        {E_VDS_MIRROR_SETPERIOD_FAIL, "E_VDS_MIRROR_SETPERIOD_FAIL", "Failed to set mirror period.", "����Զ�̾���ͬ������ʧ��"},
        {E_VDS_MIRROR_SRC_TYPE, "E_VDS_MIRROR_SRC_TYPE", "This vol can not be mirror src vol.", "�þ�����ΪԴ�����Դ��"},

        {E_VDS_RVOL_ALREADY_EXIST,"E_VDS_RVOL_ALREADY_EXIST","The snap repository of bvol has been already created ,not allowed to create again!","Դ�����Դ�ռ��Ѿ��������ˣ��������ٴδ�����"},
        {E_VDS_RVOL_CREATE_FAIL,   "E_VDS_RVOL_CREATE_FAIL",    "Failed to create snap repository.",  "������Դ�ռ�ʧ�ܡ�"},
        {E_VDS_NO_RVOL,     "E_VDS_NO_RVOL",      "Snap repository does not exist.",      "ָ���Ŀ�����Դ�ռ䲻���ڡ�"} ,
        {E_VDS_RVOL_DELETE_FAIL,   "E_VDS_RVOL_DELETE_FAIL",    "Failed to delete snap repository .",  "ɾ��������Դ�ռ�ʧ�ܡ�"},
        {E_VDS_RVOL_DEL_REJECT,   "E_VDS_RVOL_DEL_REJECT",    "Snapshot volume exist. Unable to delete snap repository .",  "������Դ�ռ����п��վ�ʱ��ɾ��ʧ�ܡ�"},
        {E_VDS_GET_RVOLINFO_FAIL,   "E_VDS_GET_RVOLINFO_FAIL",    "Cannot get snap repository info.",  "��ȡ������Դ�ռ���Ϣʧ��"},
        {E_VDS_VD_NOT_FREEZED,   "E_VDS_VD_NOT_FREEZED",   "The virtual disk is not frozen,not allowed to unfreeze.", "��������δ���ᣬ������ⶳ��"},
        {E_VDS_MIRROR_BUSY,     "E_VDS_MIRROR_BUSY",     "Mirror is busy. Unable to execute this command.",  "������æ���޷�ִ�����"},
        {E_VDS_MIRROR_SYNCING, "E_VDS_MIRROR_SYNCING", "Mirror is syncing. Unable to execute this command.", "��������ͬ�����޷�ִ�����"},
        {E_VDS_MIRROR_DEST, "E_VDS_MIRROR_DEST", "remote disk is not exist.", "����Զ�˴��̲�����"},
        {E_VDS_MIRROR_MAXNUM, "E_VDS_MIRROR_MAXNUM", "The number of mirror exceeds the maximum number.", "����������ﵽ����"},
        {E_VDS_MIRROR_TYPE, "E_VDS_MIRROR_TYPE", "only support synchronous mirror now.", "Ŀǰֻ֧��ͬ������"},
        {E_VDS_VD_SETFLAG_FAIL, "E_VDS_VD_SETFLAG_FAIL", "failed to set vd flag.", "����VD״̬ʧ��"},

        {E_VDS_POOL_EXCEED, "E_VDS_POOL_EXCEED", "The number of pool exceeds the maximum number.", "�洢�ص������ﵽ���ޡ�"},
        {E_VDS_KERNEL_POOL_CREATE_FAIL, "E_VDS_KERNEL_POOL_CREATE_FAIL", "create pool failed.", "�����洢��ʧ�ܡ�"},
        {E_VDS_KERNEL_POOL_DELETE_FAIL, "E_VDS_KERNEL_POOL_DELETE_FAIL", "delete pool failed.", "ɾ���洢��ʧ�ܡ�"},
        {E_VDS_POOL_NOT_EXIST, "E_VDS_POOL_NOT_EXIST", "the pool is not exist.", "�洢�ز����ڡ�"},
        {E_VDS_POOL_DEL_REJECT, "E_VDS_POOL_DEL_REJECT", "vol exist.Unable to delete the pool.", "�洢������vol���޷�ɾ����"},
        {E_VDS_POOL_EXIST, "E_VDS_POOL_EXIST", "the pool has the same name exist.", "ͬ���Ĵ洢���Ѿ����ڡ�"},
        {E_VDS_KERNEL_POOL_MODIFY_FAIL, "E_VDS_KERNEL_POOL_MODIFY_FAIL", "modify pool failed.", "�޸Ĵ洢��ʧ�ܡ�"},
        {E_VDS_POOL_CANNOT_REMOVE_DISK, "E_VDS_POOL_CANNOT_REMOVE_DISK", "vol exist,can not remove member disk.", "�洢������vol������ɾ�����̡�"},
        {E_VDS_KERNEL_POOL_GETINFO_FAIL, "E_VDS_KERNEL_POOL_GETINFO_FAIL", "get the pool info failed.", "��ȡ�洢����Ϣʧ�ܡ�"},
        {E_VDS_POOL_ALARMTHRESHOLD_INVALID, "E_VDS_POOL_ALARMTHRESHOLD_INVALID", "pool AlarmThreshold error.", "�����澯��ֵ������"},
        {E_VDS_POOL_CAPACITY_NOT_ENOUTH, "E_VDS_POOL_CAPACITY_NOT_ENOUTH", "pool spare capacity is not enough.", "�洢����������"},
        {E_VDS_VOL_EXCEED_IN_SYS, "E_VDS_VOL_EXCEED_IN_SYS,", "Volume number exceeds the allowed limit system.", "ϵͳ�е�vol�����Ѵﵽ�������"},
        {E_VDS_POOL_STATE_INVALID, "E_VDS_POOL_STATE_INVALID", "can not execute this command on pool INSUFCAPAC.", "�������������С��8�Ĵ洢�����޷�ִ�и�����"},
        {E_VDS_POOL_REMOVE_DISK_INVALID, "E_VDS_POOL_REMOVE_DISK_INVALID", "pool member disks will be less than 8 after execute this command,can not remove member disk.", "ִ�и������洢�س�Ա����������8������ִ�и�����"},
        {E_VDS_VOL_CAPACITY_EXCEED, "E_VDS_VOL_CAPACITY_EXCEED", "vol capacity will exceed the maximum after execute this command,can not execute this command.", "ִ�и������vol�����ᳬ���������������ִ�и�����"},

        /*******************************SAS ��ϵͳ************************************/


        /*******************************JBODC ��ϵͳ************************************/


        /*******************************TCS ��ϵͳ************************************/
        {E_TCS_COPY_FROM_USER_FAIL,         "E_TCS_COPY_FROM_USER_FAIL",            "Failed to copy info from user ",                        "���û�̬��¡���ݵ��ں�̬ʧ��"},
        {E_TCS_COPY_TO_USER_FAIL,           "E_TCS_COPY_TO_USER_FAIL",              "Failed to copy info to user ",                          "���ں�̬��¡���ݵ��û�̬ʧ��"},
        {E_TCS_KMALLOC_FAIL,                "E_TCS_KMALLOC_FAIL",                   "Failed to kmalloc memory ",                             "��ȡ�ں˴洢�ռ�ʧ��"},
        {E_TCS_DOWN_SEM_FAIL,               "E_TCS_DOWN_SEM_FAIL",                  "Failed to get semaphore var ",                          "��ȡ�ź���ʧ��"},
        {E_TCS_TGT_NOT_EXIST,               "E_TCS_TGT_NOT_EXIST",                  "Target not exist ",                                     "Ŀ����������"},
        {E_TCS_TGT_ALREADY_EXIST,           "E_TCS_TGT_ALREADY_EXIST",              "Target Already Exist ",                                 "Ŀ�����Ѵ���"},
        {E_TCS_TGT_SESSION_BUSY,            "E_TCS_TGT_SESSION_BUSY",               "Target session list is busy ",                          "�Ự����æµ״̬"},
        {E_TCS_MAX_TGT_NUM_REACHED,         "E_TCS_MAX_TGT_NUM_REACHED",            "Max Target number reached ",                            "Ŀ������Ŀ�Ѵ������"},
        {E_TCS_SESSION_NOT_EXIST,           "E_TCS_SESSION_NOT_EXIST",              "Session not exist ",                                    "�Ự������"},
        {E_TCS_SESSION_ALREADY_EXIST,       "E_TCS_SESSION_ALREADY_EXIST",          "Session Already Exist ",                                "�Ự�Ѵ���"},
        {E_TCS_SESSION_CONN_BUSY,           "E_TCS_SESSION_CONN_BUSY",              "Session still have connections ",                       "�Ự�л���������"},
        {E_TCS_INVAL_IOCTL_CMD,             "E_TCS_INVAL_IOCTL_CMD",                "Invalid ioctl command ",                                "����ʶ�������"},
        {E_TCS_PORTGRP_NOT_EXIST,           "E_TCS_PORTGRP_NOT_EXIST",              "This portal group not exist ",                          "�ö˿��鲻����"},
        {E_TCS_ALL_PORTGRP_BE_USED,         "E_TCS_ALL_PORTGRP_BE_USED",            "All portal group have been used "                       "���ж˿��鶼��ʹ��"},

        {E_TCS_DIR_OPEN_FAIL,               "E_TCS_DIR_OPEN_FAIL",                  "Dir open fail ",                                        "Ŀ¼��ʧ��"},
        {E_TCS_FILE_OPEN_FAIL,              "E_TCS_FILE_OPEN_FAIL",                 "File open fail ",                                       "�ļ���ʧ��"},
        {E_TCS_SYSCALL_FAIL,                "E_TCS_SYSCALL_FAIL",                   "Syscall failed ",                                       "ϵͳ����ʧ��"},
        {E_TCS_SYSPROC_FAIL,                "E_TCS_SYSPROC_FAIL",                   "Sysproc failed ",                                       "ϵͳ����ʧ��"},
        {E_TCS_PARAM_NULL,                  "E_TCS_PARAM_NULL",                     "Param is null ",                                        "����Ϊ��"},
        {E_TCS_PORTNUM_ERR,                 "E_TCS_PORTNUM_ERR",                    "Err portnum ",                                          "�˿ںŴ���"},
        {E_TCS_CFG_PARAM_NULL,              "E_TCS_CFG_PARAM_NULL",                 "Param is null ",                                        "����Ϊ��"},
        {E_TCS_CFG_PORTNUM_ERR,             "E_TCS_CFG_PORTNUM_ERR",                "Err portnum ",                                          "�˿ںŴ���"},
        {E_TCS_CFG_SYS_PROC_FAIL,           "E_TCS_CFG_SYS_PROC_FAIL",              "System process failed ",                                "ϵͳ����ʧ��"},
        {E_TCS_CFG_INFO_DIFFER,             "E_TCS_CFG_INFO_DIFFER",                "Infomation if differ ",                                 "�ں�̬���û�̬��Ϣ��һ��"},
        {E_TCS_CFG_IP_FAIL,                 "E_TCS_CFG_IP_FAIL",                    "Failed to config ip ",                                  "����IPʧ��"},
        {E_TCS_CFG_ARP_FAIL,                "E_TCS_CFG_ARP_FAIL",                   "Failed to config arp ",                                 "����ARPʧ��"},
        {E_TCS_CFG_MAC_FAIL,                "E_TCS_CFG_MAC_FAIL",                   "Failed to config mac ",                                 "����MACʧ��"},
        {E_TCS_CFG_MTU_FAIL,                "E_TCS_CFG_MTU_FAIL",                   "Failed to config mtu ",                                 "����MTUʧ��"},
        {E_TCS_CFG_ROUTE_FAIL,              "E_TCS_CFG_ROUTE_FAIL",                 "Failed to config route ",                               "����·��ʧ��"},
        {E_TCS_CFG_ISCSI_FAIL,              "E_TCS_CFG_ISCSI_FAIL",                 "Failed to config iscsi ",                               "����iSCSIʧ��"},
        {E_TCS_CFG_IPADD_FAIL,              "E_TCS_CFG_IPADD_FAIL",                 "Failed to add ip ",                                     "��Ӷ˿�IPʧ��"},
        {E_TCS_CFG_IPDEL_FAIL,              "E_TCS_CFG_IPDEL_FAIL",                 "Failed to delete ip ",                                  "ɾ���˿�IPʧ��"},
        {E_TCS_CFG_ROUTE_ADD_FAIL,          "E_TCS_CFG_ROUTE_ADD_FAIL",             "Failed to add route ",                                  "���·��ʧ��"},
        {E_TCS_CFG_ROUTE_DEL_FAIL,          "E_TCS_CFG_ROUTE_DEL_FAIL",             "Failed to delete route ",                               "ɾ��·��ʧ��"},
        {E_TCS_CFG_IPADDR_ZERO,             "E_TCS_CFG_IPADDR_ZERO",                "Ip address is zero ",                                   "IP��ַΪ0"},
        {E_TCS_CFG_IPMASK_ZERO,             "E_TCS_CFG_IPMASK_ZERO",                "Mask is zero ",                                         "����Ϊ0"},
        {E_TCS_CFG_IPADDR_IPMASK_ONE_ZERO,  "E_TCS_CFG_IPADDR_IPMASK_ONE_ZERO",     "IP or mask is zero ",                                   "IP��������һ��Ϊ0"},
        {E_TCS_CFG_IPMASK_FORMAT_ERR,       "E_TCS_CFG_IPMASK_FORMAT_ERR",          "Err mask format ",                                      "����������ʽ"},
        {E_TCS_CFG_IPMASK_LEN_ERR,          "E_TCS_CFG_IPMASK_LEN_ERR",             "Mask len err ",                                         "���볤�ȴ���"},
        {E_TCS_CFG_IPADDR_AREA_ERR,         "E_TCS_CFG_IPADDR_AREA_ERR",            "Err ip address ",                                       "IP��ַ��Χ����"},
        {E_TCS_CFG_PORTIP_EXIST,            "E_TCS_CFG_PORTIP_EXIST",               "PortIp is already existed ",                            "�˿�IP�Ѵ���"},
        {E_TCS_CFG_PORTIP_SAMEDOMAIN,       "E_TCS_CFG_PORTIP_SAMEDOMAIN",          "Ip address is in the same domain ",                     "�Ͷ˿�IPͬһ����"},
        {E_TCS_CFG_IPADDR_NOMATCH,          "E_TCS_CFG_IPADDR_NOMATCH",             "Ip address is not match ",                              "IP��ַ��ƥ��"},
        {E_TCS_CFG_IPADDR_NOCFG,            "E_TCS_CFG_IPADDR_NOCFG",               "Ip address is not config ",                             "IP��ַû������"},
        {E_TCS_CFG_ROUTE_EXIST,             "E_TCS_CFG_ROUTE_EXIST",                "Route existed ",                                        "·���Ѵ���"},
        {E_TCS_CFG_ROUTE_NOT_EXIST,         "E_TCS_CFG_ROUTE_NOT_EXIST",            "Route not existed ",                                    "·�ɲ�����"},
        {E_TCS_CFG_ROUTE_CLASH,             "E_TCS_CFG_ROUTE_CLASH",                "Route clashed ",                                        "·�ɳ�ͻ"},
        {E_TCS_CFG_ROUTE_NEXHOP_NODIRECT,   "E_TCS_CFG_ROUTE_NEXHOP_NODIRECT",      "Nexthop is not direct ",                                "��һ����ֱ��"},
        {E_TCS_CFG_ROUTE_STATICNUM_MAX,     "E_TCS_CFG_ROUTE_STATICNUM_MAX",        "Static route num is over MAX ",                         "��̬·�ɸ����Ѵﵽ����"},
        {E_TCS_CFG_ROUTE_STATICNUM_ZERO,    "E_TCS_CFG_ROUTE_STATICNUM_ZERO",       "Static route num is zero ",                             "��̬·�ɸ���Ϊ0"},
        {E_TCS_CFG_MTU_AREA_ERR,            "E_TCS_CFG_MTU_AREA_ERR",               "Err Mtu value ",                                        "MTUȡֵ����"},
        {E_TCS_CFG_MAC_FORMAT_ERR,          "E_TCS_CFG_MAC_FORMAT_ERR",             "Mac format err ",                                       "MAC��ַ��ʽ����"},
        {E_TCS_CFG_MAC_REPEAT,              "E_TCS_CFG_MAC_REPEAT",                 "Mac address repeated ",                                 "MAC��ַ�ظ�"},
        {E_TCS_CFG_ROUTE_MASK_ERR,          "E_TCS_CFG_ROUTE_MASK_ERR",             "Destination or mask is invalid,(Destination & Mask) != Destination.",                  "Ŀ�ĵ�ַ�������д�,(Ŀ�ĵ�ַ&����) != Ŀ�ĵ�ַ"},
        {E_TCS_CFG_HAVE_STATICROUTE,        "E_TCS_CFG_HAVE_STATICROUTE",           "Have static routes existed,please delete first.",       "�о�̬·�ɴ���,����ɾ��"},
        {E_TCS_TEST_LENGTH_ERR,             "E_TCS_TEST_LENGTH_ERR",                "Data Length error(0 or >1472).",                        "���ݾ��ɳ��ȴ���(0 ����>1472)"},
        {E_TCS_TEST_INTER_ERR,              "E_TCS_TEST_INTER_ERR",                 "time interval error(0 or >1s).",                        "���������(0 ����>1s)"},
        {E_TCS_TEST_NUM_ERR,                "E_TCS_TEST_NUM_ERR",                   "Data number error(=0).",                                "���͸�������(=0)"},
        {E_TCS_CFG_CTRLID_ERR,              "E_TCS_CFG_CTRLID_ERR",                 "Ctrl ID is invalid.",                                   "������ID����"},
        {E_TCS_PM_ID,                       "E_TCS_PM_ID",                          "Err PM ID. ",                                           "�����tcs����ͳ�ƺ�"},
        {E_TCS_PM_PORTSTAT_FUNCFLAG,        "E_TCS_PM_PORTSTAT_FUNCFLAG",           "Err PortStat function flag.",                           "�˿�����ͳ�ƹ��ܱ�־����"},
        {E_TCS_PM_PORTSTAT_CTRLFLAG,        "E_TCS_PM_PORTSTAT_CTRLFLAG",           "Err PortStat ctrl flag.",                               "�˿�����ͳ�ƿ��Ʊ�־����"},
        {E_TCS_PM_PORTSTAT_MEMSIZE,         "E_TCS_PM_PORTSTAT_MEMSIZE",            "PortStat memsize err.",                                 "�˿�����ͳ���ڴ��С����"},
        { -1, NULL,   NULL,     NULL}


    };

#endif



#endif
