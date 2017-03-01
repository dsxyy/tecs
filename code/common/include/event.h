/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�event.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ������ϵͳ��������Ϣ��
* ��ǰ�汾��1.0
* ��    �ߣ����Ľ�
* ������ڣ�2011��8��15��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/8/15
*     �� �� �ţ�V1.0
*     �� �� �ˣ����Ľ�
*     �޸����ݣ�����
* �޸ļ�¼2��
*     �޸����ڣ�2011/8/18
*     �� �� �ţ�V1.0
*     �� �� �ˣ�Ԭ����
*     �޸����ݣ�����HostManager/HostAgent/HostHandler��ģ�����Ϣ��
* �޸ļ�¼3��
*     �޸����ڣ�2011/9/11
*     �� �� �ţ�V1.0
*     �� �� �ˣ�������
*     �޸����ݣ�����TCFileStorage��CCFileStorage��HCStorageAgnet��ģ�����Ϣ��
�޸ļ�¼4��
*     �޸����ڣ�2013/4/11
*     �� �� �ţ�V1.0
*     �� �� �ˣ�Bob
*     �޸����ݣ����»��ָ���ϵͳ����Ϣ�Ŷ�
******************************************************************************/
#ifndef COMM_EVENT_H
#define COMM_EVENT_H
#include "message.h"

namespace zte_tecs
{
/****************************    ��ϵͳ������   ********************************/

/*******************************************************************************
   [    0,  9999]  : sky���Լ�����ģ��ʹ�á�
                      0: ��ЧID. [1,999]: skyʹ��. [1000, 9999]TECS����ģ��
   [10000, 19999]  : ����
   [20000, 29999]  : �洢
   [30000, 39999]  : ����
   [40000, 49999]  : ����                                                     
*******************************************************************************/

//������ϵͳ 
#define TECS_COMPUTE_EVENT_BEGIN        (10000)  //[10000,19999]
#define TECS_COMPUTE_EVENT_END          (19999)

//�洢��ϵͳ  
#define TECS_STORAGE_EVENT_BEGIN	    (20000)  //[20000,29999]
#define TECS_STORAGE_EVENT_END	        (29999)

//������ϵͳ 
#define TECS_VNET_EVENT_BEGIN           (30000)  //[30000,39999]
#define TECS_VNET_EVENT_END             (39999)

//������ϵͳ 
#define TECS_IMAGE_EVENT_BEGIN	        (40000)  //[40000,49999]
#define TECS_IMAGE_EVENT_END	        (49999)


/**************************      ģ�鼶����    ********************************/

/******************************************************************************/
/***                          TECS����ģ�黮��                              ***/
/******************************************************************************/
#define TECS_EVENT_ID_BEGIN             (SKY_MESSAGE_ID_END + 1) // 1000
#define TECS_COMMON_BEGIN               (TECS_EVENT_ID_BEGIN + 0)
#define TECS_USER_EVENT_BEGIN           (TECS_COMMON_BEGIN + 100) // [1100, 1199]
#define TECS_ALARM_EVENT_BEGIN          (TECS_COMMON_BEGIN + 200) // [1200, 1299]
#define TECS_VERSION_MANAGER_BEGIN      (TECS_COMMON_BEGIN + 300) // [1300, 1399]
#define TECS_NAT_EVENT_BEGIN            (TECS_COMMON_BEGIN + 400) // [1400, 1499]

/******************************************************************************/
/***                         ������ϵͳ��ģ�黮��                           ***/
/******************************************************************************/
#define TECS_CLUSTER_MANGER_BEGIN       (TECS_COMPUTE_EVENT_BEGIN + 0)   //[10000, 10099]
#define TECS_CLUSTER_AGENT_BEGIN        (TECS_COMPUTE_EVENT_BEGIN + 100) //[10100, 10199]
#define TECS_HOST_MANAGER_BEGIN         (TECS_COMPUTE_EVENT_BEGIN + 200) //[10200, 10299]
#define TECS_HOST_HANDLER_BEGIN         (TECS_COMPUTE_EVENT_BEGIN + 300) //[10300, 10399]
#define TECS_VMCFG_MANAGER_BEGIN        (TECS_COMPUTE_EVENT_BEGIN + 400) //[10400, 10499]
#define TECS_VMTEMPLATE_MANAGER_BEGIN   (TECS_COMPUTE_EVENT_BEGIN + 500) //[10500, 10299]
#define TECS_PROJECT_MANAGER_BEGIN      (TECS_COMPUTE_EVENT_BEGIN + 600) //[10600, 10299]
#define TECS_VM_MANGER_BEGIN            (TECS_COMPUTE_EVENT_BEGIN + 700) //[10700, 10299]
#define TECS_VM_HANDLER_BEGIN           (TECS_COMPUTE_EVENT_BEGIN + 800) //[10800, 10299]
#define TECS_VMAGT_EVENT_BEGIN          (TECS_COMPUTE_EVENT_BEGIN + 900) //[10900, 10299]
#define TECS_PHY_PORT_MANAGER_BEGIN     (TECS_COMPUTE_EVENT_BEGIN + 1000)//[101000, 10299]
#define TECS_SPECIAL_DEVICE_BEGIN       (TECS_COMPUTE_EVENT_BEGIN + 1100)//[101100, 10299]
#define TECS_FILE_MANAGER_BEGIN         (TECS_COMPUTE_EVENT_BEGIN + 1200)//[10120, 10299]
#define TECS_POWER_MANAGER_BEGIN        (TECS_COMPUTE_EVENT_BEGIN + 1300) //[101300, 101349]
#define TECS_POWER_AGENT_BEGIN          (TECS_COMPUTE_EVENT_BEGIN + 1350) //[101350, 101399]

/******************************************************************************/
/***                           �洢��ϵͳ��ģ�黮��                         ***/
/******************************************************************************/
//����ӿ�
#define TECS_STORAGE_OUT_BEGIN          (TECS_STORAGE_EVENT_BEGIN + 0)   //[20000, 20099]
//�ڲ�����
#define TECS_STORAGE_INNER_BEGIN        (TECS_STORAGE_EVENT_BEGIN + 100) //[20100, 29999]
#define TECS_STORAGE_TMP_BEGIN        (TECS_STORAGE_EVENT_BEGIN + 200) //[20100, 29999]

/******************************************************************************/
/***                           ������ϵͳ��ģ�黮��                         ***/
/******************************************************************************/
// ��vnet_event.h�ж���

/******************************************************************************/
/***                           ������ϵͳ��ģ�黮��                         ***/
/******************************************************************************/
// �ڲ����ٷ�ģ��






/**************************      ģ���ڶ���    ********************************/

/******************************************************************************/
/***                          TECS����ģ�����                              ***/
/******************************************************************************/
// TECS������ģ�鹲ͬ�õ����¼�  
#define EV_MESSAGE_UNIT_STARTUP         (TECS_COMMON_BEGIN + 0)
#define EV_POWER_ON                     (TECS_COMMON_BEGIN + 1)
#define EV_STARTUP                      (TECS_COMMON_BEGIN + 2)
#define EV_SYS_LOG                      (TECS_COMMON_BEGIN + 3)
#define EV_EXC_LOG                      (TECS_COMMON_BEGIN + 4)
#define EV_LASTWORDS_LOG                (TECS_COMMON_BEGIN + 5)

//  �û������¼�
#define EV_AUTHORIZE                    (TECS_USER_EVENT_BEGIN + 0)
#define EV_AUTHENTICATE                 (TECS_USER_EVENT_BEGIN + 1) 
#define EV_SESSION                      (TECS_USER_EVENT_BEGIN + 2)

/******************************************************************************/
//�澯����
// cc.AlarmAgent -> tc.AlarmManager:�澯�ϱ���Ϣ
#define EV_ALARM_REPORT                 (TECS_ALARM_EVENT_BEGIN + 1)
// cc.AlarmAgent -> tc.AlarmManager:֪ͨ�ϱ���Ϣ
#define EV_INFORM_REPORT                (TECS_ALARM_EVENT_BEGIN + 2)
// tc.AlarmManager -> cc.AlarmAgent:�澯ͬ����Ϣ����
#define EV_ALARM_M2A_SYNC_REQ           (TECS_ALARM_EVENT_BEGIN + 3)
// tc.AlarmManager timer
#define EV_ALARM_TIMER_SYNC             (TECS_ALARM_EVENT_BEGIN + 4)
// tc.AlarmManager broadcast
#define EV_ALARM_BROADCAST              (TECS_ALARM_EVENT_BEGIN + 5)
// tc.AlarmAgent timer send inform
#define EV_ALARM_TIMER_SEND_INFORM      (TECS_ALARM_EVENT_BEGIN + 6)
// tc.AlarmAgent timer power on sync
#define EV_ALARM_TIMER_POWERON_SYNC      (TECS_ALARM_EVENT_BEGIN + 7)
// tc.AlarmAgent timer sync to manager
#define EV_ALARM_TIMER_A2MSYNC      (TECS_ALARM_EVENT_BEGIN + 8)

/*******************************************************************************/
//�汾�����¼�
#define EV_VERSION_MANAGER_LOAD_REQ              (TECS_VERSION_MANAGER_BEGIN+0)
#define EV_VERSION_MANAGER_LOAD_ACK              (TECS_VERSION_MANAGER_BEGIN+1)
#define EV_VERSION_MANAGER_INSTALL_REQ           (TECS_VERSION_MANAGER_BEGIN+2)
#define EV_VERSION_MANAGER_INSTALL_ACK           (TECS_VERSION_MANAGER_BEGIN+3)
#define EV_VERSION_MANAGER_QUERY_REQ             (TECS_VERSION_MANAGER_BEGIN+4)
#define EV_VERSION_MANAGER_QUERY_ACK             (TECS_VERSION_MANAGER_BEGIN+5)
#define EV_TIMER_VERSION_MSG_TIMEOUT             (TECS_VERSION_MANAGER_BEGIN+6)
#define EV_VERSION_MANAGER_UNINSTALL_REQ         (TECS_VERSION_MANAGER_BEGIN+7)
#define EV_VERSION_MANAGER_UNINSTALL_ACK         (TECS_VERSION_MANAGER_BEGIN+8)
#define EV_VERSION_QUERY_REQ                     (TECS_VERSION_MANAGER_BEGIN+11)
#define EV_VERSION_QUERY_ACK                     (TECS_VERSION_MANAGER_BEGIN+12)
#define EV_VERSION_GET_REPOADDR_REQ              (TECS_VERSION_MANAGER_BEGIN+13)
#define EV_VERSION_GET_REPOADDR_ACK              (TECS_VERSION_MANAGER_BEGIN+14)
#define EV_VERSION_UPDATE_REQ                    (TECS_VERSION_MANAGER_BEGIN+15)
#define EV_VERSION_UPDATE_ACK                    (TECS_VERSION_MANAGER_BEGIN+16)
#define EV_VERSION_CANCEL_REQ                    (TECS_VERSION_MANAGER_BEGIN+17)
#define EV_VERSION_CANCEL_ACK                    (TECS_VERSION_MANAGER_BEGIN+18)
#define EV_VERSION_INSTALL_REQ                   (TECS_VERSION_MANAGER_BEGIN+19)
#define EV_VERSION_INSTALL_ACK                   (TECS_VERSION_MANAGER_BEGIN+20)
#define EV_VERSION_SET_REPOADDR_REQ                (TECS_VERSION_MANAGER_BEGIN+21)
#define EV_VERSION_SET_REPOADDR_ACK                (TECS_VERSION_MANAGER_BEGIN+22)
#define EV_PUBLISH_SSH_PUBKEY                    (TECS_VERSION_MANAGER_BEGIN+23)

/******************************************************************************/
//NATģ�� 
#define EV_NAT_MANAGER_POWER_ON    (TECS_NAT_EVENT_BEGIN+1)   /* NAT MANAGER�����ϵ���Ϣ */
#define EV_NAT_RULE_TO_NATMANAGE   (TECS_NAT_EVENT_BEGIN+2)  /* NAT���򣬴�CC��NATMANAGE  */
#define EV_NATMANAGE_ACK_CC_NATRULE (TECS_NAT_EVENT_BEGIN+3)  /* NAT���ؽ��̻�ӦCC NAT����ɹ�����*/






/******************************************************************************/
//��Ⱥ����ģ�� 
/* �����¼���API����CLUSTER_AGENT��Manager��������Ϣ */
#define EV_FILTER_CLUSTER_BY_COMPUTE_REQ       (TECS_CLUSTER_MANGER_BEGIN + 0)     /* agent��manager����ע��������Ϣ��611002946040ȡ���ֶ�ע�� */
#define EV_FILTER_CLUSTER_BY_COMPUTE_ACK       (TECS_CLUSTER_MANGER_BEGIN + 1)     /* manager��agent����ע��Ӧ����Ϣ��611002946040ȡ���ֶ�ע�� */

//#define EV_CLUSTER_ACTION_UNREG_REQ     (TECS_CLUSTER_MANGER_BEGIN + 2)     /* agent��manager����ɾ��ע������611002946040ȡ���ֶ�ע�� */
//#define EV_CLUSTER_ACTION_UNREG_ACK     (TECS_CLUSTER_MANGER_BEGIN + 3)     /* manager��agent����ɾ��ע��Ӧ��611002946040ȡ���ֶ�ע�� */

#define EV_CLUSTER_ACTION_INFO_REQ      (TECS_CLUSTER_MANGER_BEGIN + 4)     /* agent��manager���Ͳ�ѯ���� */
#define EV_CLUSTER_ACTION_INFO_ACK      (TECS_CLUSTER_MANGER_BEGIN + 5)     /* manager��agent���Ͳ�ѯӦ�� */

//#define EV_CLUSTER_ACTION_CONFIG_REQ    (TECS_CLUSTER_MANGER_BEGIN + 6)     /* agent��manager������������ */
//#define EV_CLUSTER_ACTION_CONFIG_ACK    (TECS_CLUSTER_MANGER_BEGIN + 7)     /* manager��agent������������Ӧ�� */

//#define EV_CLUSTER_ACTION_TCU_CFG_REQ (TECS_CLUSTER_MANGER_BEGIN + 8)     /* agent��manager����TCU�������� */
//#define EV_CLUSTER_ACTION_TCU_CFG_ACK (TECS_CLUSTER_MANGER_BEGIN + 9)     /* manager��agent����TCU����Ӧ�� */

#define EV_CLUSTER_ACTION_FIND_REQ      (TECS_CLUSTER_MANGER_BEGIN + 10)    /* agent��manager���ͼ�ȺԤ�������� */
#define EV_CLUSTER_ACTION_FIND_ACK      (TECS_CLUSTER_MANGER_BEGIN + 11)    /* manager��agent���ͼ�ȺԤ����Ӧ�� */

#define EV_CLUSTER_ACTION_STOP_REQ      (TECS_CLUSTER_MANGER_BEGIN + 12)    /* agent��manager���ͼ�Ⱥֹͣ�������� */
#define EV_CLUSTER_ACTION_STOP_ACK      (TECS_CLUSTER_MANGER_BEGIN + 13)    /* manager��agent���ͼ�Ⱥֹͣ����Ӧ�� */

#define EV_CLUSTER_ACTION_START_REQ      (TECS_CLUSTER_MANGER_BEGIN + 14)    /* agent��manager���ͼ�Ⱥ������������ */
#define EV_CLUSTER_ACTION_START_ACK      (TECS_CLUSTER_MANGER_BEGIN + 15)    /* manager��agent���ͼ�Ⱥ��������Ӧ�� */

#define EV_CLUSTER_ACTION_QUERY_CORE_TCU        (TECS_CLUSTER_MANGER_BEGIN + 16)        /* agent��manager���Ͳ�ѯCPU������TCU�����ֵ */
#define EV_CLUSTER_ACTION_QUERY_CORE_TCU_ACK    (TECS_CLUSTER_MANGER_BEGIN + 17)    /* ������Ϣ��Ӧ����Ϣ */

#define EV_CLUSTER_ACTION_QUERY_NETPLAN     (TECS_CLUSTER_MANGER_BEGIN + 18)        /* manager��agent���Ͳ�ѯ��Ⱥ������ƽ��ֵ */
#define EV_CLUSTER_ACTION_QUERY_NETPLAN_ACK (TECS_CLUSTER_MANGER_BEGIN + 19)     /* ������Ϣ��Ӧ����Ϣ */

#define EV_CLUSTER_ACTION_SET_REQ       (TECS_CLUSTER_MANGER_BEGIN + 20)     /* agent��manager��������������Ϣ��611002946040ȡ���ֶ�ע�� */
#define EV_CLUSTER_ACTION_SET_ACK       (TECS_CLUSTER_MANGER_BEGIN + 21)     /* manager��agent��������Ӧ����Ϣ��611002946040ȡ���ֶ�ע�� */

#define EV_CLUSTER_ACTION_FORGET_REQ    (TECS_CLUSTER_MANGER_BEGIN + 22)     /* agent��manager��������������Ϣ��611002946040ȡ���ֶ�ע�� */
#define EV_CLUSTER_ACTION_FORGET_ACK    (TECS_CLUSTER_MANGER_BEGIN + 23)     /* manager��agent��������Ӧ����Ϣ��611002946040ȡ���ֶ�ע�� */

/******************************************************************************/
// ClusterAgent
// tc.api -> cc.HostManager: ��Ⱥ������Ϣ��ѯ������
#define EV_CLUSTER_CONFIG_QUERY_REQ     (TECS_CLUSTER_AGENT_BEGIN + 0)

// tc.api <- cc.HostManager: ��Ⱥ������Ϣ��ѯ��Ӧ��
#define EV_CLUSTER_CONFIG_QUERY_ACK     (TECS_CLUSTER_AGENT_BEGIN + 1)

// tc.api -> cc.HostManager: ��ȺTCU������Ϣ�Ĳ�ѯ
#define EV_CLUSTER_TCU_QUERY_REQ        (TECS_CLUSTER_AGENT_BEGIN + 2)

// tc.api <- cc.HostManager: ��ȺTCU������Ϣ��Ӧ��
#define EV_CLUSTER_TCU_QUERY_ACK        (TECS_CLUSTER_AGENT_BEGIN + 3)

// tc.api -> cc.HostManager: ��Ⱥ����cpu��Ϣ�Ĳ�ѯ
#define EV_CLUSTER_CPU_INFO_QUERY_REQ   (TECS_CLUSTER_AGENT_BEGIN + 4)

// tc.api <- cc.HostManager: ��Ⱥ����cpu��Ϣ�Ĳ�ѯ
#define EV_CLUSTER_CPU_INFO_QUERY_ACK   (TECS_CLUSTER_AGENT_BEGIN + 5)
// tc.ClusterManager -> cc.HostManager: ����Ⱥע�ᵽTECSϵͳ������
#define EV_CLUSTER_REGISTER_REQ         (TECS_CLUSTER_AGENT_BEGIN + 6)

// tc.ClusterManager -> cc.HostManager: ����Ⱥ��TECSϵͳע��������
#define EV_CLUSTER_UNREGISTER_REQ       (TECS_CLUSTER_AGENT_BEGIN + 7)

// tc.ClusterManager -> cc.HostManager: CC�յ�TC�Լ�Ⱥ�����������õ�����
#define EV_CLUSTER_CONFIG_REQ           (TECS_CLUSTER_AGENT_BEGIN + 8)

// tc.ClusterManager -> cc.HostManager: CC�յ�TC�Լ�Ⱥ�����������õ�Ӧ��
#define EV_CLUSTER_CONFIG_ACK           (TECS_CLUSTER_AGENT_BEGIN + 9)

// tc.ClusterManager -> cc.HostManager: CC�յ�TC�Լ�ȺTCU�������õ�����
#define EV_CLUSTER_TCU_CONFIG_REQ       (TECS_CLUSTER_AGENT_BEGIN + 10)

// tc.ClusterManager -> cc.HostManager: CC�յ�TC�Լ�ȺTCU�������õ�Ӧ��
#define EV_CLUSTER_TCU_CONFIG_ACK       (TECS_CLUSTER_AGENT_BEGIN + 11)

// tc.ClusterManager -> cc.HostManager: CC�յ�TC��ѯ��Ⱥ��Դ״̬������Ϣ
#define EV_CLUSTER_INFO_REQ             (TECS_CLUSTER_AGENT_BEGIN + 12)

// tc.ClusterManager <- cc.HostManager: CC��TC�ϱ���Ӧ��Ⱥ��Դ״̬��Ϣ
#define EV_CLUSTER_INFO_REPORT          (TECS_CLUSTER_AGENT_BEGIN + 13)

// tc.ClusterManager <- cc.HostManager: ��Ⱥ���������ϱ�
#define EV_CLUSTER_DISCOVER             (TECS_CLUSTER_AGENT_BEGIN + 14)

#define EV_CLUSTER_CMM_ADD_REQ          (TECS_CLUSTER_AGENT_BEGIN + 15)
#define EV_CLUSTER_CMM_ADD_ACK          (TECS_CLUSTER_AGENT_BEGIN + 16)

#define EV_CLUSTER_CMM_DELETE_REQ       (TECS_CLUSTER_AGENT_BEGIN + 17)
#define EV_CLUSTER_CMM_DELETE_ACK       (TECS_CLUSTER_AGENT_BEGIN + 18)

#define EV_CLUSTER_CMM_SET_REQ          (TECS_CLUSTER_AGENT_BEGIN + 18)
#define EV_CLUSTER_CMM_SET_ACK          (TECS_CLUSTER_AGENT_BEGIN + 19)

#define EV_CLUSTER_CMM_QUERY_REQ        (TECS_CLUSTER_AGENT_BEGIN + 20)
#define EV_CLUSTER_CMM_QUERY_ACK        (TECS_CLUSTER_AGENT_BEGIN + 21)

#define EV_CLUSTER_SAVE_ENERGY_CONFIG_REQ (TECS_CLUSTER_AGENT_BEGIN + 22)
#define EV_CLUSTER_SAVE_ENERGY_CONFIG_ACK (TECS_CLUSTER_AGENT_BEGIN + 23)

#define EV_CLUSTER_SAVE_ENERGY_QUERY_REQ (TECS_CLUSTER_AGENT_BEGIN + 24)
#define EV_CLUSTER_SAVE_ENERGY_QUERY_ACK (TECS_CLUSTER_AGENT_BEGIN + 25)

#define EV_CLUSTER_SAVE_ENERGY_CONFIG_CHG (TECS_CLUSTER_AGENT_BEGIN + 26)

/******************************************************************************/
// ��������ģ�� 
// cc.HostManager -> hc.HostAgent: ������ע�ᵽ��Ⱥ������
#define EV_HOST_REGISTER_REQ            (TECS_HOST_MANAGER_BEGIN + 0)

// cc.HostManager -> hc.HostAgent: �������Ӽ�Ⱥ��ע��������
#define EV_HOST_UNREGISTER_REQ          (TECS_HOST_MANAGER_BEGIN + 1)

// tc.api -> cc.HostManager: �޸�������������Ϣ����
#define EV_HOST_SET_REQ                 (TECS_HOST_MANAGER_BEGIN + 2)

// tc.api <- cc.HostManager: �޸�������������ϢӦ��
#define EV_HOST_SET_ACK                 (TECS_HOST_MANAGER_BEGIN + 3)

// tc.api -> cc.HostManager: ��Forbidden��������ϵͳ��"����"��������
#define EV_HOST_FORGET_REQ              (TECS_HOST_MANAGER_BEGIN + 4)

// tc.api <- cc.HostManager: ��Forbidden��������ϵͳ��"����"����Ӧ��
#define EV_HOST_FORGET_ACK              (TECS_HOST_MANAGER_BEGIN + 5)

// tc.api -> cc.HostManager: ������������ά��ģʽ������
#define EV_HOST_DISABLE_REQ             (TECS_HOST_MANAGER_BEGIN + 6)

// tc.api <- cc.HostManager: ������������ά��ģʽ��Ӧ��
#define EV_HOST_DISABLE_ACK             (TECS_HOST_MANAGER_BEGIN + 7)

// tc.api -> cc.HostManager: ���������˳�ά��ģʽ������
#define EV_HOST_ENABLE_REQ              (TECS_HOST_MANAGER_BEGIN + 8)

// tc.api <- cc.HostManager: ���������˳�ά��ģʽ��Ӧ��
#define EV_HOST_ENABLE_ACK              (TECS_HOST_MANAGER_BEGIN + 9)

// tc.api -> cc.HostManager: ��������������������
#define EV_HOST_REBOOT_REQ              (TECS_HOST_MANAGER_BEGIN + 10)

// tc.api <- cc.HostManager: ����������������Ӧ��
#define EV_HOST_REBOOT_ACK              (TECS_HOST_MANAGER_BEGIN + 11)

// tc.api -> cc.HostManager: ���������йػ�������
#define EV_HOST_SHUTDOWN_REQ            (TECS_HOST_MANAGER_BEGIN + 12)

// tc.api <- cc.HostManager: ���������йػ���Ӧ��
#define EV_HOST_SHUTDOWN_ACK            (TECS_HOST_MANAGER_BEGIN + 13)

// cc.HostManager <- hc.HostAgent: ������Ϣ�����ϱ�
#define EV_HOST_INFO_REPORT             (TECS_HOST_MANAGER_BEGIN + 16)

// hc.VmHandler -> cc.VmManager: �����ϵ��������Ϣ�ϱ�
#define EV_VM_INFO_REPORT               (TECS_HOST_MANAGER_BEGIN + 17)

// cc.HostManager <- hc.HostAgent: �������������ϱ�
#define EV_HOST_DISCOVER                (TECS_HOST_MANAGER_BEGIN + 18)


// tc.api <- cc.HostManager: ������TRUNK������Ӧ��

// cc.HostManager -> cc.HighAvailable: �������ϻ�����
#define EV_HOST_FAULT                   (TECS_HOST_MANAGER_BEGIN + 30)
#define EV_HOST_OFFLINE                 (TECS_HOST_MANAGER_BEGIN + 31)

// cc.HostManager -> cc.HighAvailable: �����������ڹ���
#define EV_HOST_ENET_ERROR              (TECS_HOST_MANAGER_BEGIN + 42)

// cc.HostManager -> cc.HighAvailable: �������ش洢����
#define EV_HOST_STORAGE_ERROR           (TECS_HOST_MANAGER_BEGIN + 43)

// cc.HostManager -> cc.HighAvailable: ���������е����������
#define EV_HOST_VM_FAULT                (TECS_HOST_MANAGER_BEGIN + 44)

// cc.VmManager <- cc.HighAvailable: �����ģ�鷢��������Ĺ��ϴ������
#define EV_VM_FAULT_PROCESS             (TECS_HOST_MANAGER_BEGIN + 45)

// cc.HostManager -> cc.HighAvailable: ���������е�cpuռ��̫��
#define EV_HOST_CPU_BYSY                (TECS_HOST_MANAGER_BEGIN + 46)

// tc.api -> cc.HostManager: ��������ִ�����������
#define EV_HOST_COMMAND_REQ             (TECS_HOST_MANAGER_BEGIN + 47)

// tc.api <- cc.HostManager: ��������ִ������Ľ��
#define EV_HOST_COMMAND_ACK             (TECS_HOST_MANAGER_BEGIN + 48)

// tc.api -> cc.HostManager: ��ѯ������Ϣ������
#define EV_HOST_QUERY_REQ               (TECS_HOST_MANAGER_BEGIN + 49)

// tc.api <- cc.HostManager: ��ѯ������Ϣ��Ӧ��
#define EV_HOST_QUERY_ACK               (TECS_HOST_MANAGER_BEGIN + 50)

// host_admin -> HostAgent / cluster_admin -> HostManager: ������ϵͳ��Ϣ������
#define EV_REGISTERED_SYSTEM_QUERY      (TECS_HOST_MANAGER_BEGIN + 57)

// HostAgent -> host_admin / HostManager -> cluster_admin: ����ϵͳ��Ϣ
#define EV_REGISTERED_SYSTEM            (TECS_HOST_MANAGER_BEGIN + 58)

// tc.api -> cc.HostManager: ����������ƽ�����Ե���������
#define EV_NETPLANE_CFG_REQ             (TECS_HOST_MANAGER_BEGIN + 59)

// tc.api -> cc.HostManager: ����������ƽ�����Ե�����Ӧ��
#define EV_NETPLANE_CFG_ACK             (TECS_HOST_MANAGER_BEGIN + 60)

// cc.HostManager <- hc.HostAgent: ������Ϣ�����ϱ�
#define EV_HOST_HEARTBEAT_REPORT        (TECS_HOST_MANAGER_BEGIN + 61)

// cc.HostManager <- hc.HostAgent: �����˿���Ϣ�����ϱ�

// HOST EXTERN CTRL
#define EV_HOST_EXTERN_CTRL_REQ         (TECS_HOST_MANAGER_BEGIN + 76)
#define EV_HOST_EXTERN_CTRL_ACK         (TECS_HOST_MANAGER_BEGIN + 77)
#define EV_HOST_EXTERN_QUERY_REQ        (TECS_HOST_MANAGER_BEGIN + 78)
#define EV_HOST_EXTERN_QUERY_ACK        (TECS_HOST_MANAGER_BEGIN + 79)
#define EV_HOST_EXTERN_CFG_SET_REQ      (TECS_HOST_MANAGER_BEGIN + 80)
#define EV_HOST_EXTERN_CFG_SET_ACK      (TECS_HOST_MANAGER_BEGIN + 81)
#define EV_HOST_EXTERN_CFG_GET_REQ      (TECS_HOST_MANAGER_BEGIN + 82)
#define EV_HOST_EXTERN_CFG_GET_ACK      (TECS_HOST_MANAGER_BEGIN + 83)

#define EV_FANTRAY_RPMCTL_REQ           (TECS_HOST_MANAGER_BEGIN + 84)
#define EV_FANTRAY_RPMCTL_ACK           (TECS_HOST_MANAGER_BEGIN + 85)

// cc.HostManager -> hc.PortManager: ����˿�/Trunk�ڵ�������Ϣ�仯֪ͨ
#define EV_HOST_PORT_INFO_SET_ACK       (TECS_HOST_MANAGER_BEGIN + 90)



/******************************************************************************/
// host_handle
#define EV_HOST_RAW_HANDLE_REQ          (TECS_HOST_HANDLER_BEGIN + 0)
#define EV_HOST_RAW_HANDLE_ACK          (TECS_HOST_HANDLER_BEGIN + 1)
#define EV_HOST_DEF_HANDLE_REQ          (TECS_HOST_HANDLER_BEGIN + 2)
#define EV_HOST_DEF_HANDLE_ACK          (TECS_HOST_HANDLER_BEGIN + 3)
#define EV_HOST_LOOPBACK_SET_REQ        (TECS_HOST_HANDLER_BEGIN + 4)
#define EV_HOST_LOOPBACK_SET_ACK        (TECS_HOST_HANDLER_BEGIN + 5)

#define EV_SET_PORT_NETPLANE_REQ        (TECS_HOST_HANDLER_BEGIN + 6) 
#define EV_SET_PORT_NETPLANE_ACK        (TECS_HOST_HANDLER_BEGIN + 7) 

#define EV_SET_TRUNK_NETPLANE_REQ       (TECS_HOST_HANDLER_BEGIN + 8) 
#define EV_SET_TRUNK_NETPLANE_ACK       (TECS_HOST_HANDLER_BEGIN + 9) 


/******************************************************************************/
//  VmCfgģ��
#define EV_VMCFG_ALLOCATE_REQ         (TECS_VMCFG_MANAGER_BEGIN + 0) //ֱ�Ӵ���
#define EV_VMCFG_DELETE_REQ           (TECS_VMCFG_MANAGER_BEGIN + 1)
#define EV_VMCFG_DEPLOY_REQ           (TECS_VMCFG_MANAGER_BEGIN + 2)
#define EV_VMCFG_CANCEL_REQ           (TECS_VMCFG_MANAGER_BEGIN + 3)
#define EV_VMCFG_ACTION_REQ           (TECS_VMCFG_MANAGER_BEGIN + 4)
#define EV_VMCFG_QUERY_REQ            (TECS_VMCFG_MANAGER_BEGIN + 5)
#define EV_VMCFG_OPERATE_ACK          (TECS_VMCFG_MANAGER_BEGIN + 6)

#define EV_VMCFG_IMAGE_ACQUIRE_REQ    (TECS_VMCFG_MANAGER_BEGIN + 7)
#define EV_VMCFG_IMAGE_ACQUIRE_ACK    (TECS_VMCFG_MANAGER_BEGIN + 8)

#define EV_VMCFG_IMAGE_RELEASE_REQ    (TECS_VMCFG_MANAGER_BEGIN + 9)
#define EV_VMCFG_IMAGE_RELEASE_ACK    (TECS_VMCFG_MANAGER_BEGIN + 10)

#define EV_VMCFG_PROJECT_VERIFY_REQ   (TECS_VMCFG_MANAGER_BEGIN + 11)
#define EV_VMCFG_PROJECT_VERIFY_ACK   (TECS_VMCFG_MANAGER_BEGIN + 12)

#define EV_VMCFG_DEPLOY_DSIPATCH_REQ  (TECS_VMCFG_MANAGER_BEGIN + 13)
#define EV_VMCFG_DEPLOY_DSIPATCH_ACK  (TECS_VMCFG_MANAGER_BEGIN + 14)

#define EV_VMCFG_CANCEL_DSIPATCH_REQ  (TECS_VMCFG_MANAGER_BEGIN + 15)
#define EV_VMCFG_CANCEL_DSIPATCH_ACK  (TECS_VMCFG_MANAGER_BEGIN + 16)

#define EV_VMCFG_ACTION_DSIPATCH_REQ  (TECS_VMCFG_MANAGER_BEGIN + 17)

#define EV_VMCFG_INDIRECT_CREATE_REQ  (TECS_VMCFG_MANAGER_BEGIN + 18)//ͨ��ģ�崴��

#define EV_VMCFG_VMTEMPLATE_ACQUIRE_REQ    (TECS_VMCFG_MANAGER_BEGIN + 19)
#define EV_VMCFG_VMTEMPLATE_ACQUIRE_ACK    (TECS_VMCFG_MANAGER_BEGIN + 20)

#define EV_VMCFG_MODIFY_REQ                (TECS_VMCFG_MANAGER_BEGIN + 21)
#define EV_VMCFG_SET_REQ                   (TECS_VMCFG_MANAGER_BEGIN + 22)
#define EV_VMCFG_MANAGER_RELATION          (TECS_VMCFG_MANAGER_BEGIN + 23) 
#define EV_VMCFG_QUERY_RELATION_REQ        (TECS_VMCFG_MANAGER_BEGIN + 24) 
#define EV_VMCFG_QUERY_RELATION_ACK        (TECS_VMCFG_MANAGER_BEGIN + 25) 
#define EV_VMCFG_QUERY_VM_SYN_STATE_REQ    (TECS_VMCFG_MANAGER_BEGIN + 26) 
#define EV_VMCFG_QUERY_VM_SYN_STATE_ACK    (TECS_VMCFG_MANAGER_BEGIN + 27) 
#define EV_VMCFG_SYNC_REQ                  (TECS_VMCFG_MANAGER_BEGIN + 28) 
#define EV_VMCFG_SYNC_ACK                  (TECS_VMCFG_MANAGER_BEGIN + 29)

#define EV_VMCFG_USB_INFO_QUERY     (TECS_VMCFG_MANAGER_BEGIN + 30)
#define EV_VMCFG_USB_INFO_QUERY_ACK  (TECS_VMCFG_MANAGER_BEGIN + 31)

#define EV_VMCFG_USB_OPRATE             (TECS_VMCFG_MANAGER_BEGIN + 32)
#define EV_VMCFG_USB_OPRATE_ACK          (TECS_VMCFG_MANAGER_BEGIN + 33)

#define EV_VMCFG_PREPARE_DEPLOY_MSG_REQ       (TECS_VM_MANGER_BEGIN+34)
#define EV_VMCFG_ALLOCATE_MAC_REQ             (TECS_VM_MANGER_BEGIN+35)
#define EV_VMCFG_RELEASE_MAC_REQ              (TECS_VM_MANGER_BEGIN+36)
#define EV_SELECT_CLUSTER_FROM_CANDIDATES_REQ (TECS_VMCFG_MANAGER_BEGIN + 37)

/******************************************************************************/
//  VmTemplateģ��
#define EV_VMTEMPLATE_ALLOCATE_REQ    (TECS_VMTEMPLATE_MANAGER_BEGIN + 0)
#define EV_VMTEMPLATE_DELETE_REQ      (TECS_VMTEMPLATE_MANAGER_BEGIN + 1)
#define EV_VMTEMPLATE_ENABLE_REQ      (TECS_VMTEMPLATE_MANAGER_BEGIN + 2)
#define EV_VMTEMPLATE_PUBLISH_REQ     (TECS_VMTEMPLATE_MANAGER_BEGIN + 3)
#define EV_VMTEMPLATE_MODIFY_REQ      (TECS_VMTEMPLATE_MANAGER_BEGIN + 4)
#define EV_VMTEMPLATE_QUERY_REQ       (TECS_VMTEMPLATE_MANAGER_BEGIN + 5)
#define EV_VMTEMPLATE_OPERATE_ACK     (TECS_VMTEMPLATE_MANAGER_BEGIN + 6)
#define EV_VMTEMPLATE_SET_REQ         (TECS_VMTEMPLATE_MANAGER_BEGIN + 7)

/******************************************************************************/
// Porject ģ��
#define EV_PROJECT_ALLOCATE_REQ       (TECS_PROJECT_MANAGER_BEGIN + 0)
#define EV_PROJECT_DELETE_REQ         (TECS_PROJECT_MANAGER_BEGIN + 1)
#define EV_PROJECT_MODIFY_REQ         (TECS_PROJECT_MANAGER_BEGIN + 2)
#define EV_PROJECT_QUERY_REQ          (TECS_PROJECT_MANAGER_BEGIN + 3)
#define EV_PROJECT_VERIFY_REQ         (TECS_PROJECT_MANAGER_BEGIN + 4)
#define EV_PROJECT_OPERATE_ACK        (TECS_PROJECT_MANAGER_BEGIN + 5)
#define EV_PROJECT_SHOW_VM_REQ        (TECS_PROJECT_MANAGER_BEGIN + 6)
#define EV_PROJECT_SHOW_VM_ACK        (TECS_PROJECT_MANAGER_BEGIN + 7)

#define EV_PROJECT_VMCFG_SHOW_VM_REQ  (TECS_PROJECT_MANAGER_BEGIN + 8)
#define EV_PROJECT_VMCFG_SHOW_VM_ACK  (TECS_PROJECT_MANAGER_BEGIN + 9)
#define EV_PROJECT_SAVE_AS_IMAGE_REQ  (TECS_PROJECT_MANAGER_BEGIN + 10)
#define EV_PROJECT_SAVE_AS_IMAGE_ACK  (TECS_PROJECT_MANAGER_BEGIN + 11)
#define EV_PROJECT_CREATE_BY_IMAGE_REQ (TECS_PROJECT_MANAGER_BEGIN + 12)
#define EV_PROJECT_CREATE_BY_IMAGE_ACK (TECS_PROJECT_MANAGER_BEGIN + 13)
#define EV_PROJECT_DEPLOY_REQ         (TECS_PROJECT_MANAGER_BEGIN + 14)
#define EV_PROJECT_DEPLOY_ACK         (TECS_PROJECT_MANAGER_BEGIN + 15)
#define EV_PROJECT_REBOOT_REQ         (TECS_PROJECT_MANAGER_BEGIN + 16)
#define EV_PROJECT_REBOOT_ACK         (TECS_PROJECT_MANAGER_BEGIN + 17)
#define EV_PROJECT_CANCEL_REQ         (TECS_PROJECT_MANAGER_BEGIN + 18)
#define EV_PROJECT_CANCEL_ACK         (TECS_PROJECT_MANAGER_BEGIN + 19)
#define EV_PROJECT_CLONE_REQ          (TECS_PROJECT_MANAGER_BEGIN + 20)
#define EV_PROJECT_CLONE_ACK          (TECS_PROJECT_MANAGER_BEGIN + 21)
#define EV_PROJECT_OPERATE_END        (TECS_PROJECT_MANAGER_BEGIN + 22)
#define EV_PROJECT_DYNAMIC_DATA_REQ    (TECS_PROJECT_MANAGER_BEGIN + 23)
#define EV_PROJECT_DYNAMIC_DATA_ACK    (TECS_PROJECT_MANAGER_BEGIN + 24)
#define EV_PROJECT_OPERATE_REQ         (TECS_PROJECT_MANAGER_BEGIN + 25)



/******************************************************************************/
//  �������������¼�
#define EV_VM_DEPLOY                    (TECS_VM_MANGER_BEGIN + 0)
#define EV_VM_OP                        (TECS_VM_MANGER_BEGIN + 1)      /* ������������� */
#define EV_VM_OP_ACK                    (TECS_VM_MANGER_BEGIN + 2)      /* ���������Ӧ�� */
#define EV_VM_QUERY                     (TECS_VM_MANGER_BEGIN + 3)      /* �����������Ϣ��ѯ���� */
#define EV_VM_QUERY_ACK                 (TECS_VM_MANGER_BEGIN + 4)      /* �����������Ϣ��ѯӦ�� */
#define EV_VM_PREPARE                   (TECS_VM_MANGER_BEGIN + 5)      /* �����Ǩ��ǰ׼������ */
#define EV_VM_MIGRATE                   (TECS_VM_MANGER_BEGIN + 6)      /* �����Ǩ�ƿ�ʼ���� */
#define EV_VM_MIGRATE_SUCC              (TECS_VM_MANGER_BEGIN + 7)      /* �����Ǩ�Ƴɹ���֪ͨHC */
#define EV_VM_MIGRATE_SUCC_ACK          (TECS_VM_MANGER_BEGIN + 8)      /* HC��Ӧ�� */

#define EV_VM_SCHEDULE_SINGLE           (TECS_VM_MANGER_BEGIN+10)
#define EV_VM_SCHEDULE_BATCH            (TECS_VM_MANGER_BEGIN+11)
#define EV_VM_RESCHEDULE_SINGLE         (TECS_VM_MANGER_BEGIN+12)

#define EV_VM_DISK_URL_PREPARE_REQ     (TECS_VM_MANGER_BEGIN+13)
#define EV_VM_DISK_URL_PREPARE_ACK     (TECS_VM_MANGER_BEGIN+14)
#define EV_IMAGE_CACHE_REQ              (TECS_VM_MANGER_BEGIN+15)

//#define EV_VM_SYN_INFO_REQ              (TECS_VM_MANGER_BEGIN+16)        /* �������̬��Ϣ��ѯ */
//#define EV_VM_SYN_INFO_ACK              (TECS_VM_MANGER_BEGIN+17)        /* �������̬��Ϣ��ѯӦ�� */

#define EV_VM_DISK_URL_RELEASE_REQ     (TECS_VM_MANGER_BEGIN+18)
#define EV_VM_DISK_URL_RELEASE_ACK     (TECS_VM_MANGER_BEGIN+19)

#define EV_VM_IMAGE_URL_PREPARE_REQ     (TECS_VM_MANGER_BEGIN+20)
#define EV_VM_IMAGE_URL_PREPARE_ACK     (TECS_VM_MANGER_BEGIN+21)

#define EV_VM_IMAGE_URL_RELEASE_REQ     (TECS_VM_MANGER_BEGIN+22)
#define EV_VM_IMAGE_URL_RELEASE_ACK     (TECS_VM_MANGER_BEGIN+23)

#define EV_VM_UNSELECT_HOST             (TECS_VM_MANGER_BEGIN+24)
#define EV_VM_MODIFY_CREATE_VM          (TECS_VM_MANGER_BEGIN+25)

#define EV_VM_ATTACH_PDISK              (TECS_VM_MANGER_BEGIN+26)

#define EV_VM_UNSELECT_HOST_NEXT        (TECS_VM_MANGER_BEGIN+27)

#define EV_VM_COLD_MIGRATE              (TECS_VM_MANGER_BEGIN + 28)      /* �������Ǩ�� */

#define EV_VM_RESTORE_REQ               (TECS_VM_MANGER_BEGIN + 29)
#define EV_VM_RESTORE_ACK               (TECS_VM_MANGER_BEGIN + 30)

#define EV_VM_DESTROY_ALL_VMS           (TECS_VM_MANGER_BEGIN + 31)

#define EV_VM_HA_POLICY_SET_REQ         (TECS_VM_MANGER_BEGIN + 32)
#define EV_VM_HA_POLICY_QUERY_REQ       (TECS_VM_MANGER_BEGIN + 33)
#define EV_VM_HA_POLICY_QUERY_ACK       (TECS_VM_MANGER_BEGIN + 34)

#define EV_VM_SCH_POLICY_SET_REQ        (TECS_VM_MANGER_BEGIN + 35)
#define EV_VM_SCH_POLICY_QUERY_REQ      (TECS_VM_MANGER_BEGIN + 36)
#define EV_VM_SCH_POLICY_QUERY_ACK      (TECS_VM_MANGER_BEGIN + 37)

#define EV_VM_FINISH_FREEZE             (TECS_VM_MANGER_BEGIN + 38)

#define EV_VM_MODIFY_NEXT_ACTION        (TECS_VM_MANGER_BEGIN+39)

#define EV_VM_ALLOCATE_REQ              (TECS_VM_MANGER_BEGIN + 40)      /* ������������� */
#define EV_VM_ALLOCATE_ACK              (TECS_VM_MANGER_BEGIN + 41)      /* ������������� */

#define EV_VM_ALLOCATE_AFFREG_REQ       (TECS_VM_MANGER_BEGIN + 42)
#define EV_VM_ALLOCATE_AFFREG_ACK       (TECS_VM_MANGER_BEGIN + 43)

#define EV_UNBIND_VM_FROM_HOST          (TECS_VM_MANGER_BEGIN + 44)

#define EV_ANSWER_UPSTREAM              (TECS_VM_MANGER_BEGIN + 45)

/******************************************************************************/
//  ���������
//  vmhandler->cc���������������
#define EV_VMAGT_CFG_REQ                (TECS_VMAGT_EVENT_BEGIN + 0) 
// CC-->vmhandler���������������Ӧ��
#define EV_VMAGT_CFG_ACK                (TECS_VMAGT_EVENT_BEGIN + 1)


/* VmHandler��VmInstance��InstanceManager֮�����Ϣ */
/******************************************************************************/
//  �����ʵ�������¼�
#define EV_TIMER_VM_MONI               (TECS_VM_HANDLER_BEGIN +1)     /**<  �����״̬��ض�ʱ�� */
#define EV_DEL_VM_INSTANCE_REQ         (TECS_VM_HANDLER_BEGIN +2)     /**<  VmInstance ֪ͨ VmHandlerɾ����ʵ��*/

/******************************************************************************/
// �����豸ģ��
#define EV_LIGHT_INFO_REQ  (TECS_SPECIAL_DEVICE_BEGIN+0)
#define EV_LIGHT_INFO_ACK  (TECS_SPECIAL_DEVICE_BEGIN+1)


/******************************************************************************/
// PhyPortManagerģ��
#define TIMER_PHY_PORT_CFG_INFO_REQ     (TECS_PHY_PORT_MANAGER_BEGIN + 0)
#define TIMER_PHY_PORT_ROUTINE          (TECS_PHY_PORT_MANAGER_BEGIN + 1)


/******************************************************************************/
//  File����ģ��
#define EV_FILE_GET_URL_REQ                  (TECS_FILE_MANAGER_BEGIN + 0)
#define EV_FILE_GET_URL_ACK                  (TECS_FILE_MANAGER_BEGIN + 1)



/******************************************************************************/
/***                              �洢��ϵͳ                                ***/
/******************************************************************************/
/******************************************************************************/
//����ӿ�
#define EV_STORAGE_CREATE_BLOCK_REQ                (TECS_STORAGE_OUT_BEGIN +0) 
#define EV_STORAGE_CREATE_BLOCK_ACK                (TECS_STORAGE_OUT_BEGIN +1) 
#define EV_STORAGE_DELETE_BLOCK_REQ                (TECS_STORAGE_OUT_BEGIN +2) 
#define EV_STORAGE_DELETE_BLOCK_ACK                (TECS_STORAGE_OUT_BEGIN +3) 
#define EV_STORAGE_CREATE_SNAPSHOT_REQ             (TECS_STORAGE_OUT_BEGIN +4) 
#define EV_STORAGE_CREATE_SNAPSHOT_ACK             (TECS_STORAGE_OUT_BEGIN +5) 
#define EV_STORAGE_DELETE_SNAPSHOT_REQ             (TECS_STORAGE_OUT_BEGIN +6) 
#define EV_STORAGE_DELETE_SNAPSHOT_ACK             (TECS_STORAGE_OUT_BEGIN +7)
#define EV_STORAGE_AUTH_BLOCK_REQ                  (TECS_STORAGE_OUT_BEGIN +8) 
#define EV_STORAGE_AUTH_BLOCK_ACK                  (TECS_STORAGE_OUT_BEGIN +9) 
#define EV_STORAGE_DEAUTH_BLOCK_REQ                (TECS_STORAGE_OUT_BEGIN +10) 
#define EV_STORAGE_DEAUTH_BLOCK_ACK                (TECS_STORAGE_OUT_BEGIN +11) 
#define EV_STORAGE_GET_CLUSTERS_REQ                (TECS_STORAGE_OUT_BEGIN +12)  
#define EV_STORAGE_GET_CLUSTERS_ACK                (TECS_STORAGE_OUT_BEGIN +13) 
#define EV_STORAGE_GET_BLOCKINFO_REQ                (TECS_STORAGE_OUT_BEGIN +14)  
#define EV_STORAGE_GET_BLOCKINFO_ACK                (TECS_STORAGE_OUT_BEGIN +15) 


/******************************************************************************/
//�ڲ��ӿ�
#define EV_SA_DISCOVER                  (TECS_STORAGE_INNER_BEGIN + 0)
#define EV_SA_REGISTER_REQ              (TECS_STORAGE_INNER_BEGIN + 1)
#define EV_SA_INFO_REPORT               (TECS_STORAGE_INNER_BEGIN + 2)
#define EV_SA_UNREGISTER_REQ            (TECS_STORAGE_INNER_BEGIN + 3)
#define EV_SA_REGISTER_ACK              (TECS_STORAGE_INNER_BEGIN + 4)
#define EV_SC_REGISTERED_SYSTEM         (TECS_STORAGE_INNER_BEGIN + 5)
#define EV_SA_ACTION_INFO_REQ           (TECS_STORAGE_INNER_BEGIN + 6)     
#define EV_SA_ACTION_INFO_ACK           (TECS_STORAGE_INNER_BEGIN + 7)    
#define EV_SA_ENABLE_REQ                (TECS_STORAGE_INNER_BEGIN + 8)
#define EV_SA_ENABLE_ACK                (TECS_STORAGE_INNER_BEGIN + 9)
#define EV_SA_FORGET_ID_REQ             (TECS_STORAGE_INNER_BEGIN + 10)
#define EV_SA_FORGET_ID_ACK             (TECS_STORAGE_INNER_BEGIN + 11)
#define EV_SA_FORGET_NAME_REQ           (TECS_STORAGE_INNER_BEGIN + 12)
#define EV_SA_CLUSTER_MAP_REQ           (TECS_STORAGE_INNER_BEGIN + 13)
#define EV_SA_CLUSTER_MAP_ACK           (TECS_STORAGE_INNER_BEGIN + 14)


#define EV_SA_CREATE_SNAPSHOT                  (TECS_STORAGE_INNER_BEGIN +15) 
#define EV_SA_DELETE_SNAPSHOT                  (TECS_STORAGE_INNER_BEGIN +16)
#define EV_SA_CREATE_VOL                       (TECS_STORAGE_INNER_BEGIN +17) 
#define EV_SA_DELETE_VOL                       (TECS_STORAGE_INNER_BEGIN +18) 
#define EV_SA_CREATE_TARGET                    (TECS_STORAGE_INNER_BEGIN +19) 
#define EV_SA_DELETE_TARGET                    (TECS_STORAGE_INNER_BEGIN +20) 
#define EV_SA_ADDVOLTO_TARGET                  (TECS_STORAGE_INNER_BEGIN +21) 
#define EV_SA_ADDHOSTTO_TARGET                 (TECS_STORAGE_INNER_BEGIN +22) 
#define EV_SA_DELVOLFROM_TARGET                (TECS_STORAGE_INNER_BEGIN +23) 
#define EV_SA_DELHOSTFROM_TARGET               (TECS_STORAGE_INNER_BEGIN +24) 
#define EV_SA_OPERATE_ACK                      (TECS_STORAGE_INNER_BEGIN +25) 




/******************************************************************************/
/***                              ������ϵͳ                                ***/
/******************************************************************************/
// �˴������� ������vnet_event.h�ж���

/******************************************************************************/
/***                              ������ϵͳ                                ***/
/******************************************************************************/
//  Imageģ��
#define EV_IMAGE_REGISTER_REQ         (TECS_IMAGE_EVENT_BEGIN + 0)
#define EV_IMAGE_DEREGISTER_REQ       (TECS_IMAGE_EVENT_BEGIN + 1)
#define EV_IMAGE_ENABLE_REQ           (TECS_IMAGE_EVENT_BEGIN + 2)
#define EV_IMAGE_PUBLISH_REQ          (TECS_IMAGE_EVENT_BEGIN + 3)
#define EV_IMAGE_MODIFY_REQ           (TECS_IMAGE_EVENT_BEGIN + 4)
#define EV_IMAGE_QUERY_REQ            (TECS_IMAGE_EVENT_BEGIN + 5)
#define EV_IMAGE_OPERATE_ACK          (TECS_IMAGE_EVENT_BEGIN + 6)
#define EV_IMAGE_FILE_QUERY_REQ       (TECS_IMAGE_EVENT_BEGIN + 7)//ͨ��imageģ���ѯ�洢����ļ���Ϣ
#define EV_IMAGE_FILE_QUERY_ACK       (TECS_IMAGE_EVENT_BEGIN + 8)//ͨ��imageģ���ѯ�洢����ļ���Ϣ
#define EV_IMAGE_METADATA_REQ         (TECS_IMAGE_EVENT_BEGIN + 9)
#define EV_IMAGE_METADATA_ACK         (TECS_IMAGE_EVENT_BEGIN + 10)
#define EV_IMAGE_DELETE_REQ           (TECS_IMAGE_EVENT_BEGIN + 11)
#define EV_IMAGE_GET_URL_REQ          (TECS_IMAGE_EVENT_BEGIN + 12)
#define EV_IMAGE_GET_URL_ACK          (TECS_IMAGE_EVENT_BEGIN + 13)
#define EV_IMAGE_DEPLOY_REQ           (TECS_IMAGE_EVENT_BEGIN + 14)
#define EV_IMAGE_CANCEL_REQ           (TECS_IMAGE_EVENT_BEGIN + 15)
#define EV_IMAGE_CACHE_ACK            (TECS_IMAGE_EVENT_BEGIN + 16)
#define EV_IMAGE_FTP_SVR_INFO_REQ     (TECS_IMAGE_EVENT_BEGIN + 17)
#define EV_IMAGE_FTP_SVR_INFO_ACK     (TECS_IMAGE_EVENT_BEGIN + 18)
#define EV_IMAGE_PREPARE_REQ          (TECS_IMAGE_EVENT_BEGIN + 19)
#define EV_IMAGE_PREPARE_ACK          (TECS_IMAGE_EVENT_BEGIN + 20)
#define EV_IMAGE_DEPLOY_ACK           (TECS_IMAGE_EVENT_BEGIN + 21)
#define EV_IMAGE_CANCEL_ACK           (TECS_IMAGE_EVENT_BEGIN + 22)
#define EV_IMAGE_DOWNLOAD_REQ         (TECS_IMAGE_EVENT_BEGIN + 23)
#define EV_IMAGE_DOWNLOAD_ACK         (TECS_IMAGE_EVENT_BEGIN + 24)
#define EV_IMAGE_CANCEL_DOWNLOAD_REQ  (TECS_IMAGE_EVENT_BEGIN + 25)
#define EV_IMAGE_CANCEL_DOWNLOAD_ACK  (TECS_IMAGE_EVENT_BEGIN + 26)
#define EV_IMAGE_GET_SPACE_REQ        (TECS_IMAGE_EVENT_BEGIN + 27)
#define EV_IMAGE_GET_SPACE_ACK        (TECS_IMAGE_EVENT_BEGIN + 28)
#define EV_IMAGE_URL_PREPARE_REQ      (TECS_IMAGE_EVENT_BEGIN + 29)
#define EV_IMAGE_URL_PREPARE_ACK      (TECS_IMAGE_EVENT_BEGIN + 30)
#define EV_IMAGE_TPI_EXPORT_REQ       (TECS_IMAGE_EVENT_BEGIN + 31)
#define EV_IMAGE_TPI_EXPORT_ACK       (TECS_IMAGE_EVENT_BEGIN + 32)
#define EV_IMAGE_TPI_IMPORT_REQ       (TECS_IMAGE_EVENT_BEGIN + 33)
#define EV_IMAGE_TPI_IMPORT_ACK       (TECS_IMAGE_EVENT_BEGIN + 34)



/******************************************************************************/
/***                              ��Դ���������ϵͳ                                 ***/
/******************************************************************************/
//  power_agentģ��
#define EV_HOST_RESET_REQ      (TECS_POWER_AGENT_BEGIN + 0)
#define EV_HOST_RESET_ACK      (TECS_POWER_AGENT_BEGIN + 1)

}
using namespace zte_tecs;
#endif //#ifndef COMM_EVENT_H

