/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�phy_port_pub.h
* �ļ���ʶ��
* ����ժҪ������˿���ض���ͷ�ļ�
* ��ǰ�汾��V1.0
* ��    �ߣ�������
* ������ڣ�2011��8��20��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/8/20
*     �� �� �ţ�V1.0
*     �� �� �ˣ�������
*     �޸����ݣ�����
*******************************************************************************/


#ifndef  PHY_PORT_PUB_H
#define  PHY_PORT_PUB_H


/*
* �˿���ض���
*/
#define  PORT_SPEED_UNKNOWN                                 (int32)0        // �˿�����δ֪
#define  PORT_SPEED_10M                                     (int32)1        // �˿�����10M
#define  PORT_SPEED_100M                                    (int32)2        // �˿�����100M
#define  PORT_SPEED_1000M                                   (int32)3        // �˿�����1000M
#define  PORT_SPEED_10G                                     (int32)4        // �˿�����10G

#define  PORT_USED_YES                                      (int32)0        // �˿�ʹ��
#define  PORT_USED_NO                                       (int32)1        // �˿ڽ���

#define  PORT_NEGOTIATE_AUTO                                (int32)0        // �˿���Э��
#define  PORT_NEGOTIATE_FORCE                               (int32)1        // �˿�ǿ��

#define  PORT_DUPLEX_UNKNOWN                                (int32)0        // �˿�δָ��˫��ģʽ
#define  PORT_DUPLEX_FULL                                   (int32)1        // �˿�ȫ˫��
#define  PORT_DUPLEX_HALF                                   (int32)2        // �˿ڰ�˫��

#define  PORT_CONN_UNKNOWN                                  (int32)0        // �˿���������δ֪
#define  PORT_CONN_TP                                       (int32)1        // �˿�ΪTP
#define  PORT_CONN_AUI                                      (int32)2        // �˿�ΪAUI
#define  PORT_CONN_MII                                      (int32)3        // �˿�ΪMII
#define  PORT_CONN_FIBRE                                    (int32)4        // �˿�Ϊ���
#define  PORT_CONN_BNC                                      (int32)5        // �˿�ΪBNC

#define  PORT_MASTER_UNKNOWN                                (int32)0        // �˿�����ģʽδ֪
#define  PORT_MASTER_AUTO                                   (int32)1
#define  PORT_MASTER_MASTER                                 (int32)2        // �˿���ģʽ
#define  PORT_MASTER_SLAVE                                  (int32)3        // �˿ڴ�ģʽ

#define  PORT_STATUS_DOWN                                   (int32)0        // �˿�DOWN
#define  PORT_STATUS_UP                                     (int32)1        // �˿�UP

#define  PORT_LINKED_UNKNOWN                                  ((int32)0)      // �˿�LINK״̬δ֪
#define  PORT_LINKED_YES                                      ((int32)1)      // �˿ڼ�����
#define  PORT_LINKED_NO                                       ((int32)2)      // �˿�δ����

#define  PORT_PROMISC_YES                                   (int32)1        // �˿�Ϊ����ģʽ
#define  PORT_PROMISC_NO                                    (int32)2        // �˿�Ϊ�ǻ���ģʽ

#define  PORT_LOOPBACK_NO                                 (int32)0        // ���Ի�ģʽ
#define  PORT_LOOPBACK_YES                                (int32)1        // �Ի�ģʽ

/*
* TRUNK��ض���
*/
#define  TRUNK_MODE_SHARE                                   (int32)0        // ���ɷֵ�ģʽ
#define  TRUNK_MODE_MS                                      (int32)1        // ����ģʽ
#define  TRUNK_MODE_PROTOCOL                                (int32)4        // Э��ģʽ

/*
* NETPLANE priority����
*/
#define  HC_HIGH_PRIORITY                                   (int32)2        // HC�����ϱ�
#define  TC_LOW_PRIORITY                                   (int32)1        // ��̨����


#endif  // PHY_PORT_PUB_H

