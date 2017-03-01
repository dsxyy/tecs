/*********************************************************************
* ��Ȩ���� (C)2009, ����ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ� vnetlib_event.h
* �ļ���ʶ��
* ����˵���� ���ļ�������vnetlib������¼��ӿ�
* ��ǰ�汾�� V1.0
* ��    �ߣ� chen.zhiwei
* ������ڣ�
*
* �޸ļ�¼1��
*    �޸����ڣ�2013��01��31��
*    �� �� �ţ�V1.0
*    �� �� �ˣ���־ΰ
*    �޸����ݣ�����
**********************************************************************/

/***********************************************************
 *                    ������������ѡ��                     *
***********************************************************/
#ifndef    _VNETLIB_EVENT_H_
#define    _VNETLIB_EVENT_H_

/***********************************************************
 *                   ��׼���Ǳ�׼ͷ�ļ�                    *
***********************************************************/
#include "../../sky/include/sky.h"
#include "../../common/include/event.h"

//��Ϣ
#define EV_VNETLIB_POWER_ON                     (EV_POWER_ON)
#define EV_VNETLIB_BEGIN                        (TECS_VNET_EVENT_BEGIN) /* VNETLIB��Ϣ�ŷ�Χ30000~30500 */

//TC����Ϣ�ӿ�,Ԥ�����ܹ�20��
#define EV_VNETLIB_SELECT_CC                    (EV_VNETLIB_BEGIN+1)
#define EV_VNETLIB_SELECT_CC_ACK                (EV_VNETLIB_BEGIN+2)
#define EV_VNETLIB_GETRESOURCE_CC               (EV_VNETLIB_BEGIN+3)
#define EV_VNETLIB_GETRESOURCE_CC_ACK           (EV_VNETLIB_BEGIN+4)
#define EV_VNETLIB_FREERESOURCE_CC              (EV_VNETLIB_BEGIN+5)
#define EV_VNETLIB_FREERESOURCE_CC_ACK          (EV_VNETLIB_BEGIN+6)
#define EV_VNETLIB_SELECT_MIGRATE_CC            (EV_VNETLIB_BEGIN+7)

//CC����Ϣ�ӿ�,Ԥ�����ܹ�20��
#define EV_VNETLIB_SELECT_HC                    (EV_VNETLIB_BEGIN+21)
#define EV_VNETLIB_SELECT_HC_ACK                (EV_VNETLIB_BEGIN+22)
#define EV_VNETLIB_GETRESOURCE_HC               (EV_VNETLIB_BEGIN+23)
#define EV_VNETLIB_GETRESOURCE_HC_ACK           (EV_VNETLIB_BEGIN+24)
#define EV_VNETLIB_FREERESOURCE_HC              (EV_VNETLIB_BEGIN+25)
#define EV_VNETLIB_FREERESOURCE_HC_ACK          (EV_VNETLIB_BEGIN+26)
#define EV_VNETLIB_SELECT_MIGRATE_HC            (EV_VNETLIB_BEGIN+27)
#define EV_VNETLIB_SELECT_MIGRATE_HC_ACK        (EV_VNETLIB_BEGIN+28)

#define EV_VNETLIB_VM_MIGRATE_BEGIN             (EV_VNETLIB_BEGIN+29)
#define EV_VNETLIB_VM_MIGRATE_BEGIN_ACK         (EV_VNETLIB_BEGIN+30)
#define EV_VNETLIB_VM_MIGRATE_END               (EV_VNETLIB_BEGIN+31)
#define EV_VNETLIB_VM_MIGRATE_END_ACK           (EV_VNETLIB_BEGIN+32)

//HC����Ϣ�ӿ�,Ԥ�����ܹ�20��
#define EV_VNETLIB_STARTNETWORK                 (EV_VNETLIB_BEGIN+41)
#define EV_VNETLIB_STARTNETWORK_ACK             (EV_VNETLIB_BEGIN+42)
#define EV_VNETLIB_STOPNETWORK                  (EV_VNETLIB_BEGIN+43)
#define EV_VNETLIB_STOPNETWORK_ACK              (EV_VNETLIB_BEGIN+44)
#define EV_VNETLIB_NOTIFY_DOMID                 (EV_VNETLIB_BEGIN+45)
#define EV_VNETLIB_NOTIFY_DOMID_ACK             (EV_VNETLIB_BEGIN+46)
#define EV_VNETLIB_NOTIFY_ALL_VMNIC             (EV_VNETLIB_BEGIN+47)
#define EV_VNETLIB_NOTIFY_ALL_VMNIC_ACK         (EV_VNETLIB_BEGIN+48)


//VNA������Ϣ,Ԥ�����ܹ�10��
#define EV_VNETLIB_HEART_BEAT_MSG               (EV_VNETLIB_BEGIN+61)
#define EV_VNETLIB_HEART_BEAT_MSG_ACK           (EV_VNETLIB_BEGIN+62) 

//VNAע����Ϣ,Ԥ�����ܹ�10��
#define EV_VNETLIB_REGISTER_MSG                 (EV_VNETLIB_BEGIN+71)
#define EV_VNETLIB_REGISTER_MSG_ACK             (EV_VNETLIB_BEGIN+72)

//VNA ����
#define EV_VNETLIB_DEL_VNA                      (EV_VNETLIB_BEGIN+73)
#define EV_VNETLIB_DEL_VNA_ACK                  (EV_VNETLIB_BEGIN+74)

//֪ͨVNA HOST������Ⱥ��Ϣ            
#define EV_VNETLIB_HC_REGISTER_MSG              (EV_VNETLIB_BEGIN+81)

// Netplaneģ��
#define EV_VNETLIB_NETPLANE_CFG                 (EV_VNETLIB_BEGIN+91)

// Logic Network ģ��
#define EV_VNETLIB_LOGIC_NETWORK_CFG            (EV_VNETLIB_BEGIN+101)
#define EV_VNETLIB_LOGIC_NETWORK_NAME2UUID      (EV_VNETLIB_BEGIN+102)

// ��Դ��
#define EV_VNETLIB_LOGI_NET_IPV4_RANGE_CFG      (EV_VNETLIB_BEGIN+111) // logic network;
#define EV_VNETLIB_NP_IPV4_RANGE_CFG            (EV_VNETLIB_BEGIN+112) // netplane;
#define EV_VNETLIB_MAC_RANGE_CFG                (EV_VNETLIB_BEGIN+113)
#define EV_VNETLIB_VLAN_RANGE_CFG               (EV_VNETLIB_BEGIN+114)
#define EV_VNETLIB_SEGMENT_RANGE_CFG            (EV_VNETLIB_BEGIN+115)


//Port Groupģ��
#define EV_VNETLIB_PORT_GROUP_CFG               (EV_VNETLIB_BEGIN+121)
#define EV_VNETLIB_PORT_GROUP_CFG_ACK           (EV_VNETLIB_BEGIN+122)
#define EV_VNETLIB_PG_TRUNK_VLANRANGE_CFG       (EV_VNETLIB_BEGIN+123)
#define EV_VNETLIB_PG_TRUNK_VLANRANGE_CFG_ACK   (EV_VNETLIB_BEGIN+124)
#define EV_VNETLIB_PG_QUERY   (EV_VNETLIB_BEGIN+125)
#define EV_VNETLIB_PG_QUERY_ACK   (EV_VNETLIB_BEGIN+126)

#define EV_VNETLIB_SWITCH_CFG                   (EV_VNETLIB_BEGIN+131)
#define EV_VNETLIB_SWITCH_CFG_ACK               (EV_VNETLIB_BEGIN+132)
#define EV_VNETLIB_SWITCH_PORT_CFG              (EV_VNETLIB_BEGIN+133)
#define EV_VNETLIB_SWITCH_PORT_CFG_ACK          (EV_VNETLIB_BEGIN+134)

//PORTģ��
#define EV_VNETLIB_PORT_CFG                     (EV_VNETLIB_BEGIN+141)
#define EV_VNETLIB_PORT_CFG_ACK                 (EV_VNETLIB_BEGIN+142)

//KERNAL PORTģ��
#define EV_VNETLIB_KERNALPORT_CFG               (EV_VNETLIB_BEGIN+151)
#define EV_VNETLIB_KERNALPORT_CFG_ACK           (EV_VNETLIB_BEGIN+152)

//watchdogģ�飬������VNA֮�佻�����¼�
#define EV_VNETLIB_WATCHDOG_OPER                (EV_VNETLIB_BEGIN+161)      //������VNA����Ŀ��Ź���ʱ���ָ�����ͣ����
#define EV_VNETLIB_WATCHDOG_OPER_ACK            (EV_VNETLIB_BEGIN+162)      //VNA��Ӧ���㷢��Ŀ��Ź���ʱ���ָ�����ͣ����
#define EV_VNETLIB_WATCHDOG_NOTIFY_HC_RESTART   (EV_VNETLIB_BEGIN+163)      //VNA֪ͨ�������������
#define EV_VNETLIB_WATCHDOG_NOTIFY_HC_RESTART_ACK   (EV_VNETLIB_BEGIN+164)  //������ӦVNA������������������

//��ѯ���������
#define EV_VNETLIB_QUERY_VNICS                  (EV_VNETLIB_BEGIN+170)
#define EV_VNETLIB_QUERY_VNICS_ACK              (EV_VNETLIB_BEGIN+171)

//����LOOPBACK��
#define EV_VNETLIB_CFG_LOOPBACK                 (EV_VNETLIB_BEGIN+180)
#define EV_VNETLIB_CFG_LOOPBACK_ACK             (EV_VNETLIB_BEGIN+181)

//watchdogģ�飬���Ź���VNA֮�佻�����¼�
//�������ұ�����Ϊ�˱���ͨ��������ɾ��
#define EV_VNETLIB_START_WDG                    (EV_VNETLIB_BEGIN+190)
#define EV_VNETLIB_START_WDG_ACK                (EV_VNETLIB_BEGIN+191)
#define EV_VNETLIB_STOP_WDG                     (EV_VNETLIB_BEGIN+192)
#define EV_VNETLIB_STOP_WDG_ACK                 (EV_VNETLIB_BEGIN+193)
#define EV_VNETLIB_START_WDGTIMER               (EV_VNETLIB_BEGIN+194)
#define EV_VNETLIB_START_WDGTIMER_ACK           (EV_VNETLIB_BEGIN+195)
#define EV_VNETLIB_STOP_WDGTIMER                (EV_VNETLIB_BEGIN+196)
#define EV_VNETLIB_STOP_WDGTIMER_ACK            (EV_VNETLIB_BEGIN+197)

#define EV_VNETLIB_NOTIFY_VM_RR                 (EV_VNETLIB_BEGIN+198)  //���Ź�֪ͨVNA���������
#define EV_VNETLIB_NOTIFY_VM_RR_ACK             (EV_VNETLIB_BEGIN+199)  //VNA��Ӧ���Ź�������������������
#define EV_VNETLIB_WDG_OPER                     (EV_VNETLIB_BEGIN+200)  //VNA���Ź������������ֹͣ���Ź�����
#define EV_VNETLIB_WDG_OPER_ACK                 (EV_VNETLIB_BEGIN+201)  //���Ź���ӦVNA�����������ֹͣ���Ź�����
#define EV_VNETLIB_WDG_TIMER_OPER               (EV_VNETLIB_BEGIN+202)  //VNA���Ź�����Ļָ�����ͣ���Ź���ʱ������
#define EV_VNETLIB_WDG_TIMER_OPER_ACK           (EV_VNETLIB_BEGIN+203)  //���Ź���ӦVNA����Ļָ�����ͣ���Ź���ʱ������


//ͨ��ģ��
#define EV_VNETLIB_WILDCAST_SWITCH_CFG                   (EV_VNETLIB_BEGIN+250)
#define EV_VNETLIB_WILDCAST_SWITCH_CFG_ACK               (EV_VNETLIB_BEGIN+251)
#define EV_VNETLIB_WILDCAST_SWITCH_PORT_CFG              (EV_VNETLIB_BEGIN+252)
#define EV_VNETLIB_WILDCAST_SWITCH_PORT_CFG_ACK          (EV_VNETLIB_BEGIN+253)

#define EV_VNETLIB_WILDCAST_CREATE_VPORT_CFG             (EV_VNETLIB_BEGIN+254)
#define EV_VNETLIB_WILDCAST_CREATE_VPORT_CFG_ACK         (EV_VNETLIB_BEGIN+255)

#define EV_VNETLIB_WILDCAST_LOOPBACK_CFG                 (EV_VNETLIB_BEGIN+256)
#define EV_VNETLIB_WILDCAST_LOOPBACK_CFG_ACK             (EV_VNETLIB_BEGIN+257)

#define EV_VNETLIB_WILDCAST_SWITCH_TASK_DEL              (EV_VNETLIB_BEGIN+258)
#define EV_VNETLIB_WILDCAST_SWITCH_TASK_DEL_ACK          (EV_VNETLIB_BEGIN+259)
#define EV_VNETLIB_WILDCAST_CREATE_VPORT_TASK_DEL        (EV_VNETLIB_BEGIN+260)
#define EV_VNETLIB_WILDCAST_CREATE_VPORT_TASK_DEL_ACK    (EV_VNETLIB_BEGIN+261)
#define EV_VNETLIB_WILDCAST_LOOPBACK_TASK_DEL            (EV_VNETLIB_BEGIN+262)
#define EV_VNETLIB_WILDCAST_LOOPBACK_TASK_DEL_ACK        (EV_VNETLIB_BEGIN+263)

//SDN�����������
#define EV_VNETLIB_SDN_CFG                               (EV_VNETLIB_BEGIN+280)
#define EV_VNETLIB_SDN_CFG_ACK                           (EV_VNETLIB_BEGIN+281)





#endif

