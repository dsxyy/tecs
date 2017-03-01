/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：phy_port_pub.h
* 文件标识：
* 内容摘要：物理端口相关定义头文件
* 当前版本：V1.0
* 作    者：张绍满
* 完成日期：2011年8月20日
*
* 修改记录1：
*     修改日期：2011/8/20
*     版 本 号：V1.0
*     修 改 人：张绍满
*     修改内容：创建
*******************************************************************************/


#ifndef  PHY_PORT_PUB_H
#define  PHY_PORT_PUB_H


/*
* 端口相关定义
*/
#define  PORT_SPEED_UNKNOWN                                 (int32)0        // 端口速率未知
#define  PORT_SPEED_10M                                     (int32)1        // 端口速率10M
#define  PORT_SPEED_100M                                    (int32)2        // 端口速率100M
#define  PORT_SPEED_1000M                                   (int32)3        // 端口速率1000M
#define  PORT_SPEED_10G                                     (int32)4        // 端口速率10G

#define  PORT_USED_YES                                      (int32)0        // 端口使用
#define  PORT_USED_NO                                       (int32)1        // 端口禁用

#define  PORT_NEGOTIATE_AUTO                                (int32)0        // 端口自协商
#define  PORT_NEGOTIATE_FORCE                               (int32)1        // 端口强制

#define  PORT_DUPLEX_UNKNOWN                                (int32)0        // 端口未指定双工模式
#define  PORT_DUPLEX_FULL                                   (int32)1        // 端口全双工
#define  PORT_DUPLEX_HALF                                   (int32)2        // 端口半双工

#define  PORT_CONN_UNKNOWN                                  (int32)0        // 端口连接类型未知
#define  PORT_CONN_TP                                       (int32)1        // 端口为TP
#define  PORT_CONN_AUI                                      (int32)2        // 端口为AUI
#define  PORT_CONN_MII                                      (int32)3        // 端口为MII
#define  PORT_CONN_FIBRE                                    (int32)4        // 端口为光口
#define  PORT_CONN_BNC                                      (int32)5        // 端口为BNC

#define  PORT_MASTER_UNKNOWN                                (int32)0        // 端口主从模式未知
#define  PORT_MASTER_AUTO                                   (int32)1
#define  PORT_MASTER_MASTER                                 (int32)2        // 端口主模式
#define  PORT_MASTER_SLAVE                                  (int32)3        // 端口从模式

#define  PORT_STATUS_DOWN                                   (int32)0        // 端口DOWN
#define  PORT_STATUS_UP                                     (int32)1        // 端口UP

#define  PORT_LINKED_UNKNOWN                                  ((int32)0)      // 端口LINK状态未知
#define  PORT_LINKED_YES                                      ((int32)1)      // 端口己连接
#define  PORT_LINKED_NO                                       ((int32)2)      // 端口未连接

#define  PORT_PROMISC_YES                                   (int32)1        // 端口为混杂模式
#define  PORT_PROMISC_NO                                    (int32)2        // 端口为非混杂模式

#define  PORT_LOOPBACK_NO                                 (int32)0        // 非自环模式
#define  PORT_LOOPBACK_YES                                (int32)1        // 自环模式

/*
* TRUNK相关定义
*/
#define  TRUNK_MODE_SHARE                                   (int32)0        // 负荷分担模式
#define  TRUNK_MODE_MS                                      (int32)1        // 主备模式
#define  TRUNK_MODE_PROTOCOL                                (int32)4        // 协议模式

/*
* NETPLANE priority定义
*/
#define  HC_HIGH_PRIORITY                                   (int32)2        // HC主动上报
#define  TC_LOW_PRIORITY                                   (int32)1        // 后台配置


#endif  // PHY_PORT_PUB_H

