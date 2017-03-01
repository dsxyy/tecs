/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：event.h
* 文件标识：见配置管理计划书
* 内容摘要：定义系统的所有消息号
* 当前版本：1.0
* 作    者：张文剑
* 完成日期：2011年8月15日
*
* 修改记录1：
*     修改日期：2011/8/15
*     版 本 号：V1.0
*     修 改 人：张文剑
*     修改内容：创建
* 修改记录2：
*     修改日期：2011/8/18
*     版 本 号：V1.0
*     修 改 人：袁进坤
*     修改内容：增加HostManager/HostAgent/HostHandler等模块的消息号
* 修改记录3：
*     修改日期：2011/9/11
*     版 本 号：V1.0
*     修 改 人：梁庆永
*     修改内容：增加TCFileStorage、CCFileStorage、HCStorageAgnet等模块的消息号
修改记录4：
*     修改日期：2013/4/11
*     版 本 号：V1.0
*     修 改 人：Bob
*     修改内容：重新划分各子系统的消息号段
******************************************************************************/
#ifndef COMM_EVENT_H
#define COMM_EVENT_H
#include "message.h"

namespace zte_tecs
{
/****************************    子系统级划分   ********************************/

/*******************************************************************************
   [    0,  9999]  : sky库以及公共模块使用。
                      0: 无效ID. [1,999]: sky使用. [1000, 9999]TECS公共模块
   [10000, 19999]  : 计算
   [20000, 29999]  : 存储
   [30000, 39999]  : 网络
   [40000, 49999]  : 镜像                                                     
*******************************************************************************/

//计算子系统 
#define TECS_COMPUTE_EVENT_BEGIN        (10000)  //[10000,19999]
#define TECS_COMPUTE_EVENT_END          (19999)

//存储子系统  
#define TECS_STORAGE_EVENT_BEGIN	    (20000)  //[20000,29999]
#define TECS_STORAGE_EVENT_END	        (29999)

//网络子系统 
#define TECS_VNET_EVENT_BEGIN           (30000)  //[30000,39999]
#define TECS_VNET_EVENT_END             (39999)

//镜像子系统 
#define TECS_IMAGE_EVENT_BEGIN	        (40000)  //[40000,49999]
#define TECS_IMAGE_EVENT_END	        (49999)


/**************************      模块级划分    ********************************/

/******************************************************************************/
/***                          TECS公共模块划分                              ***/
/******************************************************************************/
#define TECS_EVENT_ID_BEGIN             (SKY_MESSAGE_ID_END + 1) // 1000
#define TECS_COMMON_BEGIN               (TECS_EVENT_ID_BEGIN + 0)
#define TECS_USER_EVENT_BEGIN           (TECS_COMMON_BEGIN + 100) // [1100, 1199]
#define TECS_ALARM_EVENT_BEGIN          (TECS_COMMON_BEGIN + 200) // [1200, 1299]
#define TECS_VERSION_MANAGER_BEGIN      (TECS_COMMON_BEGIN + 300) // [1300, 1399]
#define TECS_NAT_EVENT_BEGIN            (TECS_COMMON_BEGIN + 400) // [1400, 1499]

/******************************************************************************/
/***                         计算子系统内模块划分                           ***/
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
/***                           存储子系统内模块划分                         ***/
/******************************************************************************/
//对外接口
#define TECS_STORAGE_OUT_BEGIN          (TECS_STORAGE_EVENT_BEGIN + 0)   //[20000, 20099]
//内部管理
#define TECS_STORAGE_INNER_BEGIN        (TECS_STORAGE_EVENT_BEGIN + 100) //[20100, 29999]
#define TECS_STORAGE_TMP_BEGIN        (TECS_STORAGE_EVENT_BEGIN + 200) //[20100, 29999]

/******************************************************************************/
/***                           网络子系统内模块划分                         ***/
/******************************************************************************/
// 在vnet_event.h中定义

/******************************************************************************/
/***                           镜像子系统内模块划分                         ***/
/******************************************************************************/
// 内部不再分模块






/**************************      模块内定义    ********************************/

/******************************************************************************/
/***                          TECS公共模块相关                              ***/
/******************************************************************************/
// TECS中所有模块共同用到的事件  
#define EV_MESSAGE_UNIT_STARTUP         (TECS_COMMON_BEGIN + 0)
#define EV_POWER_ON                     (TECS_COMMON_BEGIN + 1)
#define EV_STARTUP                      (TECS_COMMON_BEGIN + 2)
#define EV_SYS_LOG                      (TECS_COMMON_BEGIN + 3)
#define EV_EXC_LOG                      (TECS_COMMON_BEGIN + 4)
#define EV_LASTWORDS_LOG                (TECS_COMMON_BEGIN + 5)

//  用户管理事件
#define EV_AUTHORIZE                    (TECS_USER_EVENT_BEGIN + 0)
#define EV_AUTHENTICATE                 (TECS_USER_EVENT_BEGIN + 1) 
#define EV_SESSION                      (TECS_USER_EVENT_BEGIN + 2)

/******************************************************************************/
//告警管理
// cc.AlarmAgent -> tc.AlarmManager:告警上报消息
#define EV_ALARM_REPORT                 (TECS_ALARM_EVENT_BEGIN + 1)
// cc.AlarmAgent -> tc.AlarmManager:通知上报消息
#define EV_INFORM_REPORT                (TECS_ALARM_EVENT_BEGIN + 2)
// tc.AlarmManager -> cc.AlarmAgent:告警同步消息请求
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
//版本管理事件
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
//NAT模块 
#define EV_NAT_MANAGER_POWER_ON    (TECS_NAT_EVENT_BEGIN+1)   /* NAT MANAGER进程上电消息 */
#define EV_NAT_RULE_TO_NATMANAGE   (TECS_NAT_EVENT_BEGIN+2)  /* NAT规则，从CC到NATMANAGE  */
#define EV_NATMANAGE_ACK_CC_NATRULE (TECS_NAT_EVENT_BEGIN+3)  /* NAT网关进程回应CC NAT规则成功设置*/






/******************************************************************************/
//集群管理模块 
/* 下面事件是API或者CLUSTER_AGENT向Manager交互的消息 */
#define EV_FILTER_CLUSTER_BY_COMPUTE_REQ       (TECS_CLUSTER_MANGER_BEGIN + 0)     /* agent向manager发送注册请求消息。611002946040取消手动注册 */
#define EV_FILTER_CLUSTER_BY_COMPUTE_ACK       (TECS_CLUSTER_MANGER_BEGIN + 1)     /* manager向agent发送注册应答消息。611002946040取消手动注册 */

//#define EV_CLUSTER_ACTION_UNREG_REQ     (TECS_CLUSTER_MANGER_BEGIN + 2)     /* agent向manager发送删除注册请求。611002946040取消手动注册 */
//#define EV_CLUSTER_ACTION_UNREG_ACK     (TECS_CLUSTER_MANGER_BEGIN + 3)     /* manager向agent发送删除注册应答。611002946040取消手动注册 */

#define EV_CLUSTER_ACTION_INFO_REQ      (TECS_CLUSTER_MANGER_BEGIN + 4)     /* agent向manager发送查询请求 */
#define EV_CLUSTER_ACTION_INFO_ACK      (TECS_CLUSTER_MANGER_BEGIN + 5)     /* manager向agent发送查询应答 */

//#define EV_CLUSTER_ACTION_CONFIG_REQ    (TECS_CLUSTER_MANGER_BEGIN + 6)     /* agent向manager发送配置请求 */
//#define EV_CLUSTER_ACTION_CONFIG_ACK    (TECS_CLUSTER_MANGER_BEGIN + 7)     /* manager向agent发送配置请求应答 */

//#define EV_CLUSTER_ACTION_TCU_CFG_REQ (TECS_CLUSTER_MANGER_BEGIN + 8)     /* agent向manager发送TCU配置请求 */
//#define EV_CLUSTER_ACTION_TCU_CFG_ACK (TECS_CLUSTER_MANGER_BEGIN + 9)     /* manager向agent发送TCU配置应答 */

#define EV_CLUSTER_ACTION_FIND_REQ      (TECS_CLUSTER_MANGER_BEGIN + 10)    /* agent向manager发送集群预分配请求 */
#define EV_CLUSTER_ACTION_FIND_ACK      (TECS_CLUSTER_MANGER_BEGIN + 11)    /* manager向agent发送集群预分配应答 */

#define EV_CLUSTER_ACTION_STOP_REQ      (TECS_CLUSTER_MANGER_BEGIN + 12)    /* agent向manager发送集群停止分配请求 */
#define EV_CLUSTER_ACTION_STOP_ACK      (TECS_CLUSTER_MANGER_BEGIN + 13)    /* manager向agent发送集群停止分配应答 */

#define EV_CLUSTER_ACTION_START_REQ      (TECS_CLUSTER_MANGER_BEGIN + 14)    /* agent向manager发送集群启动分配请求 */
#define EV_CLUSTER_ACTION_START_ACK      (TECS_CLUSTER_MANGER_BEGIN + 15)    /* manager向agent发送集群启动分配应答 */

#define EV_CLUSTER_ACTION_QUERY_CORE_TCU        (TECS_CLUSTER_MANGER_BEGIN + 16)        /* agent向manager发送查询CPU核数和TCU的最大值 */
#define EV_CLUSTER_ACTION_QUERY_CORE_TCU_ACK    (TECS_CLUSTER_MANGER_BEGIN + 17)    /* 上面消息的应答消息 */

#define EV_CLUSTER_ACTION_QUERY_NETPLAN     (TECS_CLUSTER_MANGER_BEGIN + 18)        /* manager向agent发送查询集群的网络平面值 */
#define EV_CLUSTER_ACTION_QUERY_NETPLAN_ACK (TECS_CLUSTER_MANGER_BEGIN + 19)     /* 上面消息的应答消息 */

#define EV_CLUSTER_ACTION_SET_REQ       (TECS_CLUSTER_MANGER_BEGIN + 20)     /* agent向manager发送设置请求消息。611002946040取消手动注册 */
#define EV_CLUSTER_ACTION_SET_ACK       (TECS_CLUSTER_MANGER_BEGIN + 21)     /* manager向agent发送设置应答消息。611002946040取消手动注册 */

#define EV_CLUSTER_ACTION_FORGET_REQ    (TECS_CLUSTER_MANGER_BEGIN + 22)     /* agent向manager发送设置请求消息。611002946040取消手动注册 */
#define EV_CLUSTER_ACTION_FORGET_ACK    (TECS_CLUSTER_MANGER_BEGIN + 23)     /* manager向agent发送设置应答消息。611002946040取消手动注册 */

/******************************************************************************/
// ClusterAgent
// tc.api -> cc.HostManager: 集群参数信息查询的请求
#define EV_CLUSTER_CONFIG_QUERY_REQ     (TECS_CLUSTER_AGENT_BEGIN + 0)

// tc.api <- cc.HostManager: 集群参数信息查询的应答
#define EV_CLUSTER_CONFIG_QUERY_ACK     (TECS_CLUSTER_AGENT_BEGIN + 1)

// tc.api -> cc.HostManager: 集群TCU配置信息的查询
#define EV_CLUSTER_TCU_QUERY_REQ        (TECS_CLUSTER_AGENT_BEGIN + 2)

// tc.api <- cc.HostManager: 集群TCU配置信息的应答
#define EV_CLUSTER_TCU_QUERY_ACK        (TECS_CLUSTER_AGENT_BEGIN + 3)

// tc.api -> cc.HostManager: 集群物理cpu信息的查询
#define EV_CLUSTER_CPU_INFO_QUERY_REQ   (TECS_CLUSTER_AGENT_BEGIN + 4)

// tc.api <- cc.HostManager: 集群物理cpu信息的查询
#define EV_CLUSTER_CPU_INFO_QUERY_ACK   (TECS_CLUSTER_AGENT_BEGIN + 5)
// tc.ClusterManager -> cc.HostManager: 将集群注册到TECS系统的请求
#define EV_CLUSTER_REGISTER_REQ         (TECS_CLUSTER_AGENT_BEGIN + 6)

// tc.ClusterManager -> cc.HostManager: 将集群从TECS系统注销的请求
#define EV_CLUSTER_UNREGISTER_REQ       (TECS_CLUSTER_AGENT_BEGIN + 7)

// tc.ClusterManager -> cc.HostManager: CC收到TC对集群参数进行配置的请求
#define EV_CLUSTER_CONFIG_REQ           (TECS_CLUSTER_AGENT_BEGIN + 8)

// tc.ClusterManager -> cc.HostManager: CC收到TC对集群参数进行配置的应答
#define EV_CLUSTER_CONFIG_ACK           (TECS_CLUSTER_AGENT_BEGIN + 9)

// tc.ClusterManager -> cc.HostManager: CC收到TC对集群TCU进行配置的请求
#define EV_CLUSTER_TCU_CONFIG_REQ       (TECS_CLUSTER_AGENT_BEGIN + 10)

// tc.ClusterManager -> cc.HostManager: CC收到TC对集群TCU进行配置的应答
#define EV_CLUSTER_TCU_CONFIG_ACK       (TECS_CLUSTER_AGENT_BEGIN + 11)

// tc.ClusterManager -> cc.HostManager: CC收到TC查询集群资源状态请求消息
#define EV_CLUSTER_INFO_REQ             (TECS_CLUSTER_AGENT_BEGIN + 12)

// tc.ClusterManager <- cc.HostManager: CC向TC上报或应答集群资源状态消息
#define EV_CLUSTER_INFO_REPORT          (TECS_CLUSTER_AGENT_BEGIN + 13)

// tc.ClusterManager <- cc.HostManager: 集群发现主动上报
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
// 主机管理模块 
// cc.HostManager -> hc.HostAgent: 将主机注册到集群的请求
#define EV_HOST_REGISTER_REQ            (TECS_HOST_MANAGER_BEGIN + 0)

// cc.HostManager -> hc.HostAgent: 将主机从集群中注销的请求
#define EV_HOST_UNREGISTER_REQ          (TECS_HOST_MANAGER_BEGIN + 1)

// tc.api -> cc.HostManager: 修改主机的描述信息请求
#define EV_HOST_SET_REQ                 (TECS_HOST_MANAGER_BEGIN + 2)

// tc.api <- cc.HostManager: 修改主机的描述信息应答
#define EV_HOST_SET_ACK                 (TECS_HOST_MANAGER_BEGIN + 3)

// tc.api -> cc.HostManager: 将Forbidden的主机从系统中"遗忘"掉的请求
#define EV_HOST_FORGET_REQ              (TECS_HOST_MANAGER_BEGIN + 4)

// tc.api <- cc.HostManager: 将Forbidden的主机从系统中"遗忘"掉的应答
#define EV_HOST_FORGET_ACK              (TECS_HOST_MANAGER_BEGIN + 5)

// tc.api -> cc.HostManager: 设置主机进入维护模式的请求
#define EV_HOST_DISABLE_REQ             (TECS_HOST_MANAGER_BEGIN + 6)

// tc.api <- cc.HostManager: 设置主机进入维护模式的应答
#define EV_HOST_DISABLE_ACK             (TECS_HOST_MANAGER_BEGIN + 7)

// tc.api -> cc.HostManager: 设置主机退出维护模式的请求
#define EV_HOST_ENABLE_REQ              (TECS_HOST_MANAGER_BEGIN + 8)

// tc.api <- cc.HostManager: 设置主机退出维护模式的应答
#define EV_HOST_ENABLE_ACK              (TECS_HOST_MANAGER_BEGIN + 9)

// tc.api -> cc.HostManager: 将主机进行重启的请求
#define EV_HOST_REBOOT_REQ              (TECS_HOST_MANAGER_BEGIN + 10)

// tc.api <- cc.HostManager: 将主机进行重启的应答
#define EV_HOST_REBOOT_ACK              (TECS_HOST_MANAGER_BEGIN + 11)

// tc.api -> cc.HostManager: 将主机进行关机的请求
#define EV_HOST_SHUTDOWN_REQ            (TECS_HOST_MANAGER_BEGIN + 12)

// tc.api <- cc.HostManager: 将主机进行关机的应答
#define EV_HOST_SHUTDOWN_ACK            (TECS_HOST_MANAGER_BEGIN + 13)

// cc.HostManager <- hc.HostAgent: 主机信息主动上报
#define EV_HOST_INFO_REPORT             (TECS_HOST_MANAGER_BEGIN + 16)

// hc.VmHandler -> cc.VmManager: 主机上的虚拟机信息上报
#define EV_VM_INFO_REPORT               (TECS_HOST_MANAGER_BEGIN + 17)

// cc.HostManager <- hc.HostAgent: 主机发现主动上报
#define EV_HOST_DISCOVER                (TECS_HOST_MANAGER_BEGIN + 18)


// tc.api <- cc.HostManager: 主机的TRUNK口配置应答

// cc.HostManager -> cc.HighAvailable: 主机故障或离线
#define EV_HOST_FAULT                   (TECS_HOST_MANAGER_BEGIN + 30)
#define EV_HOST_OFFLINE                 (TECS_HOST_MANAGER_BEGIN + 31)

// cc.HostManager -> cc.HighAvailable: 主机物理网口故障
#define EV_HOST_ENET_ERROR              (TECS_HOST_MANAGER_BEGIN + 42)

// cc.HostManager -> cc.HighAvailable: 主机本地存储故障
#define EV_HOST_STORAGE_ERROR           (TECS_HOST_MANAGER_BEGIN + 43)

// cc.HostManager -> cc.HighAvailable: 主机上运行的虚拟机故障
#define EV_HOST_VM_FAULT                (TECS_HOST_MANAGER_BEGIN + 44)

// cc.VmManager <- cc.HighAvailable: 向调度模块发送虚拟机的故障处理策略
#define EV_VM_FAULT_PROCESS             (TECS_HOST_MANAGER_BEGIN + 45)

// cc.HostManager -> cc.HighAvailable: 主机上运行的cpu占用太高
#define EV_HOST_CPU_BYSY                (TECS_HOST_MANAGER_BEGIN + 46)

// tc.api -> cc.HostManager: 在主机上执行命令的请求
#define EV_HOST_COMMAND_REQ             (TECS_HOST_MANAGER_BEGIN + 47)

// tc.api <- cc.HostManager: 在主机上执行命令的结果
#define EV_HOST_COMMAND_ACK             (TECS_HOST_MANAGER_BEGIN + 48)

// tc.api -> cc.HostManager: 查询主机信息的请求
#define EV_HOST_QUERY_REQ               (TECS_HOST_MANAGER_BEGIN + 49)

// tc.api <- cc.HostManager: 查询主机信息的应答
#define EV_HOST_QUERY_ACK               (TECS_HOST_MANAGER_BEGIN + 50)

// host_admin -> HostAgent / cluster_admin -> HostManager: 归属的系统信息的请求
#define EV_REGISTERED_SYSTEM_QUERY      (TECS_HOST_MANAGER_BEGIN + 57)

// HostAgent -> host_admin / HostManager -> cluster_admin: 归属系统信息
#define EV_REGISTERED_SYSTEM            (TECS_HOST_MANAGER_BEGIN + 58)

// tc.api -> cc.HostManager: 主机的网络平面属性的配置请求
#define EV_NETPLANE_CFG_REQ             (TECS_HOST_MANAGER_BEGIN + 59)

// tc.api -> cc.HostManager: 主机的网络平面属性的配置应答
#define EV_NETPLANE_CFG_ACK             (TECS_HOST_MANAGER_BEGIN + 60)

// cc.HostManager <- hc.HostAgent: 主机信息主动上报
#define EV_HOST_HEARTBEAT_REPORT        (TECS_HOST_MANAGER_BEGIN + 61)

// cc.HostManager <- hc.HostAgent: 主机端口信息主动上报

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

// cc.HostManager -> hc.PortManager: 物理端口/Trunk口的配置信息变化通知
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
//  VmCfg模块
#define EV_VMCFG_ALLOCATE_REQ         (TECS_VMCFG_MANAGER_BEGIN + 0) //直接创建
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

#define EV_VMCFG_INDIRECT_CREATE_REQ  (TECS_VMCFG_MANAGER_BEGIN + 18)//通过模板创建

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
//  VmTemplate模块
#define EV_VMTEMPLATE_ALLOCATE_REQ    (TECS_VMTEMPLATE_MANAGER_BEGIN + 0)
#define EV_VMTEMPLATE_DELETE_REQ      (TECS_VMTEMPLATE_MANAGER_BEGIN + 1)
#define EV_VMTEMPLATE_ENABLE_REQ      (TECS_VMTEMPLATE_MANAGER_BEGIN + 2)
#define EV_VMTEMPLATE_PUBLISH_REQ     (TECS_VMTEMPLATE_MANAGER_BEGIN + 3)
#define EV_VMTEMPLATE_MODIFY_REQ      (TECS_VMTEMPLATE_MANAGER_BEGIN + 4)
#define EV_VMTEMPLATE_QUERY_REQ       (TECS_VMTEMPLATE_MANAGER_BEGIN + 5)
#define EV_VMTEMPLATE_OPERATE_ACK     (TECS_VMTEMPLATE_MANAGER_BEGIN + 6)
#define EV_VMTEMPLATE_SET_REQ         (TECS_VMTEMPLATE_MANAGER_BEGIN + 7)

/******************************************************************************/
// Porject 模块
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
//  虚拟机管理操作事件
#define EV_VM_DEPLOY                    (TECS_VM_MANGER_BEGIN + 0)
#define EV_VM_OP                        (TECS_VM_MANGER_BEGIN + 1)      /* 虚拟机操作请求 */
#define EV_VM_OP_ACK                    (TECS_VM_MANGER_BEGIN + 2)      /* 虚拟机操作应答 */
#define EV_VM_QUERY                     (TECS_VM_MANGER_BEGIN + 3)      /* 虚拟机运行信息查询请求 */
#define EV_VM_QUERY_ACK                 (TECS_VM_MANGER_BEGIN + 4)      /* 虚拟机运行信息查询应答 */
#define EV_VM_PREPARE                   (TECS_VM_MANGER_BEGIN + 5)      /* 虚拟机迁移前准备请求 */
#define EV_VM_MIGRATE                   (TECS_VM_MANGER_BEGIN + 6)      /* 虚拟机迁移开始请求 */
#define EV_VM_MIGRATE_SUCC              (TECS_VM_MANGER_BEGIN + 7)      /* 虚拟机迁移成功，通知HC */
#define EV_VM_MIGRATE_SUCC_ACK          (TECS_VM_MANGER_BEGIN + 8)      /* HC的应答 */

#define EV_VM_SCHEDULE_SINGLE           (TECS_VM_MANGER_BEGIN+10)
#define EV_VM_SCHEDULE_BATCH            (TECS_VM_MANGER_BEGIN+11)
#define EV_VM_RESCHEDULE_SINGLE         (TECS_VM_MANGER_BEGIN+12)

#define EV_VM_DISK_URL_PREPARE_REQ     (TECS_VM_MANGER_BEGIN+13)
#define EV_VM_DISK_URL_PREPARE_ACK     (TECS_VM_MANGER_BEGIN+14)
#define EV_IMAGE_CACHE_REQ              (TECS_VM_MANGER_BEGIN+15)

//#define EV_VM_SYN_INFO_REQ              (TECS_VM_MANGER_BEGIN+16)        /* 虚拟机动态信息查询 */
//#define EV_VM_SYN_INFO_ACK              (TECS_VM_MANGER_BEGIN+17)        /* 虚拟机动态信息查询应答 */

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

#define EV_VM_COLD_MIGRATE              (TECS_VM_MANGER_BEGIN + 28)      /* 虚拟机冷迁移 */

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

#define EV_VM_ALLOCATE_REQ              (TECS_VM_MANGER_BEGIN + 40)      /* 虚拟机操作请求 */
#define EV_VM_ALLOCATE_ACK              (TECS_VM_MANGER_BEGIN + 41)      /* 虚拟机操作请求 */

#define EV_VM_ALLOCATE_AFFREG_REQ       (TECS_VM_MANGER_BEGIN + 42)
#define EV_VM_ALLOCATE_AFFREG_ACK       (TECS_VM_MANGER_BEGIN + 43)

#define EV_UNBIND_VM_FROM_HOST          (TECS_VM_MANGER_BEGIN + 44)

#define EV_ANSWER_UPSTREAM              (TECS_VM_MANGER_BEGIN + 45)

/******************************************************************************/
//  虚拟机代理
//  vmhandler->cc的虚拟机配置请求
#define EV_VMAGT_CFG_REQ                (TECS_VMAGT_EVENT_BEGIN + 0) 
// CC-->vmhandler的虚拟机配置请求应答
#define EV_VMAGT_CFG_ACK                (TECS_VMAGT_EVENT_BEGIN + 1)


/* VmHandler与VmInstance或InstanceManager之间的消息 */
/******************************************************************************/
//  虚拟机实例操作事件
#define EV_TIMER_VM_MONI               (TECS_VM_HANDLER_BEGIN +1)     /**<  虚拟机状态监控定时器 */
#define EV_DEL_VM_INSTANCE_REQ         (TECS_VM_HANDLER_BEGIN +2)     /**<  VmInstance 通知 VmHandler删除本实例*/

/******************************************************************************/
// 特殊设备模块
#define EV_LIGHT_INFO_REQ  (TECS_SPECIAL_DEVICE_BEGIN+0)
#define EV_LIGHT_INFO_ACK  (TECS_SPECIAL_DEVICE_BEGIN+1)


/******************************************************************************/
// PhyPortManager模块
#define TIMER_PHY_PORT_CFG_INFO_REQ     (TECS_PHY_PORT_MANAGER_BEGIN + 0)
#define TIMER_PHY_PORT_ROUTINE          (TECS_PHY_PORT_MANAGER_BEGIN + 1)


/******************************************************************************/
//  File管理模块
#define EV_FILE_GET_URL_REQ                  (TECS_FILE_MANAGER_BEGIN + 0)
#define EV_FILE_GET_URL_ACK                  (TECS_FILE_MANAGER_BEGIN + 1)



/******************************************************************************/
/***                              存储子系统                                ***/
/******************************************************************************/
/******************************************************************************/
//对外接口
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
//内部接口
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
/***                              网络子系统                                ***/
/******************************************************************************/
// 此处不定义 ，放在vnet_event.h中定义

/******************************************************************************/
/***                              镜像子系统                                ***/
/******************************************************************************/
//  Image模块
#define EV_IMAGE_REGISTER_REQ         (TECS_IMAGE_EVENT_BEGIN + 0)
#define EV_IMAGE_DEREGISTER_REQ       (TECS_IMAGE_EVENT_BEGIN + 1)
#define EV_IMAGE_ENABLE_REQ           (TECS_IMAGE_EVENT_BEGIN + 2)
#define EV_IMAGE_PUBLISH_REQ          (TECS_IMAGE_EVENT_BEGIN + 3)
#define EV_IMAGE_MODIFY_REQ           (TECS_IMAGE_EVENT_BEGIN + 4)
#define EV_IMAGE_QUERY_REQ            (TECS_IMAGE_EVENT_BEGIN + 5)
#define EV_IMAGE_OPERATE_ACK          (TECS_IMAGE_EVENT_BEGIN + 6)
#define EV_IMAGE_FILE_QUERY_REQ       (TECS_IMAGE_EVENT_BEGIN + 7)//通过image模块查询存储里的文件信息
#define EV_IMAGE_FILE_QUERY_ACK       (TECS_IMAGE_EVENT_BEGIN + 8)//通过image模块查询存储里的文件信息
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
/***                              电源管理代理子系统                                 ***/
/******************************************************************************/
//  power_agent模块
#define EV_HOST_RESET_REQ      (TECS_POWER_AGENT_BEGIN + 0)
#define EV_HOST_RESET_ACK      (TECS_POWER_AGENT_BEGIN + 1)

}
using namespace zte_tecs;
#endif //#ifndef COMM_EVENT_H

