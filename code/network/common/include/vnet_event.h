/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�vnet_event.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ����������ϵͳ��������Ϣ��
* ��ǰ�汾��1.0
* ��    �ߣ�lvech
* ������ڣ�2013��1��15��
*
* �޸ļ�¼1��
*     �޸����ڣ�2013/1/15
*     �� �� �ţ�V1.0
*     �� �� �ˣ�lvech
*     �޸����ݣ�����
******************************************************************************/
#ifndef VNET_EVENT_H
#define VNET_EVENT_H
#include "message.h"

namespace zte_tecs
{
#define EV_VNET_POWER_ON                (EV_POWER_ON)   /* POWER ON MSG */  

#define EV_VNET_BEGIN                   (TECS_VNET_EVENT_BEGIN+501) /* VNET �ڲ���Ϣ�ŷ�Χ30501~31000 */

/* VNM<-->VNA */

//SWITCHģ��
#define EV_SWITCH_EVENT_BEGIN           (EV_VNET_BEGIN)
#define EV_SWITCH_ADD_REQ               (EV_SWITCH_EVENT_BEGIN)      /* ����������� */
#define EV_SWITCH_ADD_ACK               (EV_SWITCH_EVENT_BEGIN+1)    /* �������Ӧ�� */
#define EV_SWITCH_DEL_REQ               (EV_SWITCH_EVENT_BEGIN+2)    /* ����ɾ������ */
#define EV_SWITCH_DEL_ACK               (EV_SWITCH_EVENT_BEGIN+3)    /* ����ɾ��Ӧ�� */
#define EV_SWITCH_SET_REQ               (EV_SWITCH_EVENT_BEGIN+4)    /* ���������������� */
#define EV_SWITCH_SET_ACK               (EV_SWITCH_EVENT_BEGIN+5)    /* ������������Ӧ�� */
#define EV_SWITCH_PORT_ADD_REQ          (EV_SWITCH_EVENT_BEGIN+6)    /* �����˿�������� */
#define EV_SWITCH_PORT_ADD_ACK          (EV_SWITCH_EVENT_BEGIN+7)    /* �����˿����Ӧ�� */ 
#define EV_SWITCH_PORT_DEL_REQ          (EV_SWITCH_EVENT_BEGIN+8)    /* �����˿�ɾ������ */
#define EV_SWITCH_PORT_DEL_ACK          (EV_SWITCH_EVENT_BEGIN+9)    /* �����˿�ɾ��Ӧ�� */
#define EV_SWITCH_UPDATE_VNA_TIMER      (EV_SWITCH_EVENT_BEGIN+10)
#define EV_SWITCH_MODIFY_TIMER          (EV_SWITCH_EVENT_BEGIN+11)

//VXLANģ��
#define EV_VXLAN_EVENT_BEGIN            (EV_VNET_BEGIN+21)
#define EV_VXLAN_MCGROUP_REQ            (EV_VXLAN_EVENT_BEGIN+1)     /* VNA����VXLAN�鲥����Ϣ��Ϣ */
#define EV_VXLAN_MCGROUP_ACK            (EV_VXLAN_EVENT_BEGIN+2)     /* VNMӦ��VNA VXLAN�鲥����Ϣ��Ϣ */


/* VNM ��Ϣ����*/
#define EV_VNM_EVENT_BEGIN              (EV_VNET_BEGIN+50)
#define EV_VNA_MONI_TIMER               (EV_VNM_EVENT_BEGIN)
#define EV_MC_VNM_INFO_TIMER            (EV_VNM_EVENT_BEGIN+1)      // �鲥VNM��Ϣ��ʱ����Ϣ
#define EV_MG_VNM_INFO                  (EV_VNM_EVENT_BEGIN+2)      // ��ʱ�鲥VNM��Ϣ��Ϣ
#define EV_VNM_NOTIFY_VNA_ADD_VMVSI     (EV_VNM_EVENT_BEGIN+3)      // ��ʱ�鲥VNM��Ϣ��Ϣ
#define EV_VNM_NOTIFY_VNA_DEL_VMVSI     (EV_VNM_EVENT_BEGIN+4)      // ��ʱ�鲥VNM��Ϣ��Ϣ
#define EV_VNM_NOTIFY_VNA_ALL_VMVSI     (EV_VNM_EVENT_BEGIN+5)      // ��ʱ�鲥VNM��Ϣ��Ϣ
#define EV_VNM_NOTIFY_VNA_ALL_VMVSI_ACK (EV_VNM_EVENT_BEGIN+6)      // ��ʱ�鲥VNM��Ϣ��Ϣ
#define EV_VNM_WILDCAST_TIMER           (EV_VNM_EVENT_BEGIN+7)      // VNM ͨ��ģ�鶨ʱ����Ϣ
#define EV_VNM_SCHEDULE_CC              (EV_VNM_EVENT_BEGIN+8)      // VNM�����ģ��ת�������CC��������
#define EV_VNM_SCHEDULE_CC_ACK          (EV_VNM_EVENT_BEGIN+9)      // ����ģ����VNM��ӦCC���Ƚ��
#define EV_VNM_GET_RESOURCE_FOR_CC      (EV_VNM_EVENT_BEGIN+10)      // VNM�����ģ��ת�������CC��������
#define EV_VNM_GET_RESOURCE_FOR_CC_ACK  (EV_VNM_EVENT_BEGIN+11)      // ����ģ����VNM��ӦCC���Ƚ��
#define EV_VNM_FREE_RESOURCE_FOR_CC     (EV_VNM_EVENT_BEGIN+12)      // VNM�����ģ��ת�������CC��������
#define EV_VNM_FREE_RESOURCE_FOR_CC_ACK (EV_VNM_EVENT_BEGIN+13)      // ����ģ����VNM��ӦCC���Ƚ��
#define EV_VNM_SCHEDULE_HC              (EV_VNM_EVENT_BEGIN+14)      // VNM�����ģ��ת�������CC��������
#define EV_VNM_SCHEDULE_HC_ACK          (EV_VNM_EVENT_BEGIN+15)      // ����ģ����VNM��ӦHC���Ƚ��
#define EV_VNM_GET_RESOURCE_FOR_HC      (EV_VNM_EVENT_BEGIN+16)      // VNM�����ģ��ת�������CC��������
#define EV_VNM_GET_RESOURCE_FOR_HC_ACK  (EV_VNM_EVENT_BEGIN+17)      // ����ģ����VNM��ӦCC���Ƚ��
#define EV_VNM_FREE_RESOURCE_FOR_HC     (EV_VNM_EVENT_BEGIN+18)      // VNM�����ģ��ת�������CC��������
#define EV_VNM_FREE_RESOURCE_FOR_HC_ACK (EV_VNM_EVENT_BEGIN+19)      // ����ģ����VNM��ӦCC���Ƚ��
#define EV_VNM_NEWVNA_NOTIFY            (EV_VNM_EVENT_BEGIN+20)      // VNM֪ͨͨ��ģ��new vna ע��
#define EV_VNM_CFGINFO_TO_VNA           (EV_VNM_EVENT_BEGIN+21)      // VNM������Ϣ�·���VNA��VNA��һ������ʱ����

/* VNA ��Ϣ����*/
#define EV_VNA_EVENT_BEGIN              (EV_VNET_BEGIN+100)
#define EV_VNA_INFO_REPORT              (EV_VNA_EVENT_BEGIN)        // VNA��VNM�ϱ���Ϣ��Ϣ
#define EV_VNA_REGISTER_TO_VNM          (EV_VNA_EVENT_BEGIN+1)      // VNA��VNM����ע��������Ϣ
#define EV_VNA_REGISTER_REQ             (EV_VNA_EVENT_BEGIN+2)      // VNM��VNA��Ӧע��ɹ���Ϣ
#define EV_VNA_UNREGISTER_REQ           (EV_VNA_EVENT_BEGIN+3)      // VNM��VNA����ȡ��VNAע����Ϣ
#define EV_REGISTERED_VNM_QUERY         (EV_VNA_EVENT_BEGIN+4)      // �������̲�ѯVNA��ע���VNM��Ϣ
#define EV_REGISTERED_VNM               (EV_VNA_EVENT_BEGIN+5)      // VNA�㲥ͨ���Լ�ע���VNM��Ϣ
#define EV_VNA_HEARTBEAT_REPORT         (EV_VNA_EVENT_BEGIN+6)      // VNA��VNM���͵�������Ϣ
#define EV_VNA_CHECK_VM_VSI_INFO        (EV_VNA_EVENT_BEGIN+7)      // VNA���VSI�Ƿ��Ѵ�VNM�ϻ�ȡVSI��ϸ��Ϣ
#define EV_VNA_REQUEST_VMVSI            (EV_VNM_EVENT_BEGIN+8)      // VNA��VNM����VSI��ϸ��Ϣ
#define EV_VNA_REQUEST_VMVSI_ACK            (EV_VNM_EVENT_BEGIN+9)      // VNA��VNM����VSI��ϸ��Ϣ

/*VNET <--> MODULE ��Ϣ����*/
#define EV_MODULE_EVENT_BEGIN           (EV_VNET_BEGIN+130)

#define EV_MODULE_REGISTER_TO_VNA       (EV_MODULE_EVENT_BEGIN      // MODULE��VNA���͵�ע����Ϣ
#define EV_MODULE_UNREGISTER_TO_VNA     (EV_MODULE_EVENT_BEGIN+1)   // MODULE��VNA���͵�ȡ��ע����Ϣ
#define EV_MODULE_HEARTBEAT_REPORT      (EV_MODULE_EVENT_BEGIN+2)   // MODULE��VNA���͵�������Ϣ
#define EV_NOTIFY_VNA_HC_REGISTERED     (EV_MODULE_EVENT_BEGIN+3)   // ��HC��CCע��ɹ�����NETWORK API��NotifyRegisteredInfoToVNA�ӿ�֪ͨVNA��ؽ���

/* VNM��ʱ����Ϣ����*/
#define EV_VNM_TIMER_BEGIN              (EV_VNET_BEGIN+150)
#define TIMER_VNM_VNIC_CHECK_VSI_SYN    (EV_VNM_TIMER_BEGIN+1)      // VNA��ʱ���������ʱ����

/* VNA��ʱ����Ϣ����*/
#define EV_VNA_TIMER_BEGIN              (EV_VNET_BEGIN + 160)

#define TIMER_VNA_INFO_REPORT           (EV_VNA_TIMER_BEGIN)        // VNA���ڲɼ���Ϣ���ϱ���VNM
#define TIMER_VNA_REGISTERED_VNM        (EV_VNA_TIMER_BEGIN+1)      // VNA����ͨ���Լ�ע���VNM��Ϣ
#define TIMER_VNA_HEARTBEAT_REPORT      (EV_VNA_TIMER_BEGIN+2)      // VNA�����ϱ�������Ϣ��VNM
#define TIMER_MODULE_STATE_CHECK        (EV_VNA_TIMER_BEGIN+3)      // VNA���ڼ��MODULE������״̬
#define TIMER_VNA_REGISTER              (EV_VNA_TIMER_BEGIN+4)      // VNA���ڷ���ע����Ϣ��VNM
#define TIMER_VNA_DEV_MON               (EV_VNA_TIMER_BEGIN+5)      // VNA��ʱ���������ʱ����
#define EV_VNET_20MS_TIMER              (EV_VNA_TIMER_BEGIN+6)      // 20ms��ʱ����Ϣ������EVB.
#define TIMER_VNA_RESET_PORTINFO        (EV_VNA_TIMER_BEGIN+7)      // VNA�����˿���Ϣ�·���ʱ��
#define TIMER_VNA_RESET_PORTINFO_ACK    (EV_VNA_TIMER_BEGIN+8)
#define TIMER_VNA_MONITOR_MGR_TIMER     (EV_VNA_TIMER_BEGIN+9)
#define EV_VNET_WDG_TIMER               (EV_VNA_TIMER_BEGIN+10)     // WDG��ʱ����Ϣ[VM����]
#define EV_VNET_VNA_REPORT_INFO         (EV_VNA_TIMER_BEGIN+11)     // vna_agent֪ͨvna_controller�ϱ�
#define TIMER_VNA_INFO_REPORT_FIRST     (EV_VNA_TIMER_BEGIN+12)     // VNA���ڲɼ���Ϣ���ϱ���VNM(��һ��)
}
#endif //#ifndef COMM_EVENT_H

