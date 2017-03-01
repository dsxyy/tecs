/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vnet_event.h
* 文件标识：见配置管理计划书
* 内容摘要：定义网络系统的所有消息号
* 当前版本：1.0
* 作    者：lvech
* 完成日期：2013年1月15日
*
* 修改记录1：
*     修改日期：2013/1/15
*     版 本 号：V1.0
*     修 改 人：lvech
*     修改内容：创建
******************************************************************************/
#ifndef VNET_EVENT_H
#define VNET_EVENT_H
#include "message.h"

namespace zte_tecs
{
#define EV_VNET_POWER_ON                (EV_POWER_ON)   /* POWER ON MSG */  

#define EV_VNET_BEGIN                   (TECS_VNET_EVENT_BEGIN+501) /* VNET 内部消息号范围30501~31000 */

/* VNM<-->VNA */

//SWITCH模块
#define EV_SWITCH_EVENT_BEGIN           (EV_VNET_BEGIN)
#define EV_SWITCH_ADD_REQ               (EV_SWITCH_EVENT_BEGIN)      /* 交换添加请求 */
#define EV_SWITCH_ADD_ACK               (EV_SWITCH_EVENT_BEGIN+1)    /* 交换添加应答 */
#define EV_SWITCH_DEL_REQ               (EV_SWITCH_EVENT_BEGIN+2)    /* 交换删除请求 */
#define EV_SWITCH_DEL_ACK               (EV_SWITCH_EVENT_BEGIN+3)    /* 交换删除应答 */
#define EV_SWITCH_SET_REQ               (EV_SWITCH_EVENT_BEGIN+4)    /* 交换属性设置请求 */
#define EV_SWITCH_SET_ACK               (EV_SWITCH_EVENT_BEGIN+5)    /* 交换属性设置应答 */
#define EV_SWITCH_PORT_ADD_REQ          (EV_SWITCH_EVENT_BEGIN+6)    /* 主机端口添加请求 */
#define EV_SWITCH_PORT_ADD_ACK          (EV_SWITCH_EVENT_BEGIN+7)    /* 主机端口添加应答 */ 
#define EV_SWITCH_PORT_DEL_REQ          (EV_SWITCH_EVENT_BEGIN+8)    /* 主机端口删除请求 */
#define EV_SWITCH_PORT_DEL_ACK          (EV_SWITCH_EVENT_BEGIN+9)    /* 主机端口删除应答 */
#define EV_SWITCH_UPDATE_VNA_TIMER      (EV_SWITCH_EVENT_BEGIN+10)
#define EV_SWITCH_MODIFY_TIMER          (EV_SWITCH_EVENT_BEGIN+11)

//VXLAN模块
#define EV_VXLAN_EVENT_BEGIN            (EV_VNET_BEGIN+21)
#define EV_VXLAN_MCGROUP_REQ            (EV_VXLAN_EVENT_BEGIN+1)     /* VNA请求VXLAN组播组信息消息 */
#define EV_VXLAN_MCGROUP_ACK            (EV_VXLAN_EVENT_BEGIN+2)     /* VNM应答VNA VXLAN组播组信息消息 */


/* VNM 消息定义*/
#define EV_VNM_EVENT_BEGIN              (EV_VNET_BEGIN+50)
#define EV_VNA_MONI_TIMER               (EV_VNM_EVENT_BEGIN)
#define EV_MC_VNM_INFO_TIMER            (EV_VNM_EVENT_BEGIN+1)      // 组播VNM信息定时器消息
#define EV_MG_VNM_INFO                  (EV_VNM_EVENT_BEGIN+2)      // 定时组播VNM信息消息
#define EV_VNM_NOTIFY_VNA_ADD_VMVSI     (EV_VNM_EVENT_BEGIN+3)      // 定时组播VNM信息消息
#define EV_VNM_NOTIFY_VNA_DEL_VMVSI     (EV_VNM_EVENT_BEGIN+4)      // 定时组播VNM信息消息
#define EV_VNM_NOTIFY_VNA_ALL_VMVSI     (EV_VNM_EVENT_BEGIN+5)      // 定时组播VNM信息消息
#define EV_VNM_NOTIFY_VNA_ALL_VMVSI_ACK (EV_VNM_EVENT_BEGIN+6)      // 定时组播VNM信息消息
#define EV_VNM_WILDCAST_TIMER           (EV_VNM_EVENT_BEGIN+7)      // VNM 通配模块定时器消息
#define EV_VNM_SCHEDULE_CC              (EV_VNM_EVENT_BEGIN+8)      // VNM向调度模块转发计算的CC调度请求
#define EV_VNM_SCHEDULE_CC_ACK          (EV_VNM_EVENT_BEGIN+9)      // 调度模块向VNM回应CC调度结果
#define EV_VNM_GET_RESOURCE_FOR_CC      (EV_VNM_EVENT_BEGIN+10)      // VNM向调度模块转发计算的CC调度请求
#define EV_VNM_GET_RESOURCE_FOR_CC_ACK  (EV_VNM_EVENT_BEGIN+11)      // 调度模块向VNM回应CC调度结果
#define EV_VNM_FREE_RESOURCE_FOR_CC     (EV_VNM_EVENT_BEGIN+12)      // VNM向调度模块转发计算的CC调度请求
#define EV_VNM_FREE_RESOURCE_FOR_CC_ACK (EV_VNM_EVENT_BEGIN+13)      // 调度模块向VNM回应CC调度结果
#define EV_VNM_SCHEDULE_HC              (EV_VNM_EVENT_BEGIN+14)      // VNM向调度模块转发计算的CC调度请求
#define EV_VNM_SCHEDULE_HC_ACK          (EV_VNM_EVENT_BEGIN+15)      // 调度模块向VNM回应HC调度结果
#define EV_VNM_GET_RESOURCE_FOR_HC      (EV_VNM_EVENT_BEGIN+16)      // VNM向调度模块转发计算的CC调度请求
#define EV_VNM_GET_RESOURCE_FOR_HC_ACK  (EV_VNM_EVENT_BEGIN+17)      // 调度模块向VNM回应CC调度结果
#define EV_VNM_FREE_RESOURCE_FOR_HC     (EV_VNM_EVENT_BEGIN+18)      // VNM向调度模块转发计算的CC调度请求
#define EV_VNM_FREE_RESOURCE_FOR_HC_ACK (EV_VNM_EVENT_BEGIN+19)      // 调度模块向VNM回应CC调度结果
#define EV_VNM_NEWVNA_NOTIFY            (EV_VNM_EVENT_BEGIN+20)      // VNM通知通配模块new vna 注册
#define EV_VNM_CFGINFO_TO_VNA           (EV_VNM_EVENT_BEGIN+21)      // VNM配置信息下发给VNA，VNA第一次上线时发送

/* VNA 消息定义*/
#define EV_VNA_EVENT_BEGIN              (EV_VNET_BEGIN+100)
#define EV_VNA_INFO_REPORT              (EV_VNA_EVENT_BEGIN)        // VNA向VNM上报信息消息
#define EV_VNA_REGISTER_TO_VNM          (EV_VNA_EVENT_BEGIN+1)      // VNA向VNM发送注册请求消息
#define EV_VNA_REGISTER_REQ             (EV_VNA_EVENT_BEGIN+2)      // VNM向VNA响应注册成功消息
#define EV_VNA_UNREGISTER_REQ           (EV_VNA_EVENT_BEGIN+3)      // VNM向VNA发送取消VNA注册消息
#define EV_REGISTERED_VNM_QUERY         (EV_VNA_EVENT_BEGIN+4)      // 其他进程查询VNA所注册的VNM信息
#define EV_REGISTERED_VNM               (EV_VNA_EVENT_BEGIN+5)      // VNA广播通告自己注册的VNM信息
#define EV_VNA_HEARTBEAT_REPORT         (EV_VNA_EVENT_BEGIN+6)      // VNA向VNM发送的心跳消息
#define EV_VNA_CHECK_VM_VSI_INFO        (EV_VNA_EVENT_BEGIN+7)      // VNA检查VSI是否已从VNM上获取VSI详细信息
#define EV_VNA_REQUEST_VMVSI            (EV_VNM_EVENT_BEGIN+8)      // VNA向VNM请求VSI详细信息
#define EV_VNA_REQUEST_VMVSI_ACK            (EV_VNM_EVENT_BEGIN+9)      // VNA向VNM请求VSI详细信息

/*VNET <--> MODULE 消息定义*/
#define EV_MODULE_EVENT_BEGIN           (EV_VNET_BEGIN+130)

#define EV_MODULE_REGISTER_TO_VNA       (EV_MODULE_EVENT_BEGIN      // MODULE向VNA发送的注册消息
#define EV_MODULE_UNREGISTER_TO_VNA     (EV_MODULE_EVENT_BEGIN+1)   // MODULE向VNA发送的取消注册消息
#define EV_MODULE_HEARTBEAT_REPORT      (EV_MODULE_EVENT_BEGIN+2)   // MODULE向VNA发送的心跳消息
#define EV_NOTIFY_VNA_HC_REGISTERED     (EV_MODULE_EVENT_BEGIN+3)   // 当HC向CC注册成功，由NETWORK API中NotifyRegisteredInfoToVNA接口通知VNA相关进程

/* VNM定时器消息定义*/
#define EV_VNM_TIMER_BEGIN              (EV_VNET_BEGIN+150)
#define TIMER_VNM_VNIC_CHECK_VSI_SYN    (EV_VNM_TIMER_BEGIN+1)      // VNA定时检查监控任务时间间隔

/* VNA定时器消息定义*/
#define EV_VNA_TIMER_BEGIN              (EV_VNET_BEGIN + 160)

#define TIMER_VNA_INFO_REPORT           (EV_VNA_TIMER_BEGIN)        // VNA周期采集信息并上报给VNM
#define TIMER_VNA_REGISTERED_VNM        (EV_VNA_TIMER_BEGIN+1)      // VNA周期通告自己注册的VNM信息
#define TIMER_VNA_HEARTBEAT_REPORT      (EV_VNA_TIMER_BEGIN+2)      // VNA周期上报心跳消息给VNM
#define TIMER_MODULE_STATE_CHECK        (EV_VNA_TIMER_BEGIN+3)      // VNA周期检查MODULE的心跳状态
#define TIMER_VNA_REGISTER              (EV_VNA_TIMER_BEGIN+4)      // VNA周期发送注册消息给VNM
#define TIMER_VNA_DEV_MON               (EV_VNA_TIMER_BEGIN+5)      // VNA定时检查监控任务时间间隔
#define EV_VNET_20MS_TIMER              (EV_VNA_TIMER_BEGIN+6)      // 20ms定时器消息，用于EVB.
#define TIMER_VNA_RESET_PORTINFO        (EV_VNA_TIMER_BEGIN+7)      // VNA重启端口信息下发定时器
#define TIMER_VNA_RESET_PORTINFO_ACK    (EV_VNA_TIMER_BEGIN+8)
#define TIMER_VNA_MONITOR_MGR_TIMER     (EV_VNA_TIMER_BEGIN+9)
#define EV_VNET_WDG_TIMER               (EV_VNA_TIMER_BEGIN+10)     // WDG定时器消息[VM重启]
#define EV_VNET_VNA_REPORT_INFO         (EV_VNA_TIMER_BEGIN+11)     // vna_agent通知vna_controller上报
#define TIMER_VNA_INFO_REPORT_FIRST     (EV_VNA_TIMER_BEGIN+12)     // VNA周期采集信息并上报给VNM(第一次)
}
#endif //#ifndef COMM_EVENT_H

