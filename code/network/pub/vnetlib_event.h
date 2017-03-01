/*********************************************************************
* 版权所有 (C)2009, 中兴通讯股份有限公司。
*
* 文件名称： vnetlib_event.h
* 文件标识：
* 其它说明： 本文件定义了vnetlib库对外事件接口
* 当前版本： V1.0
* 作    者： chen.zhiwei
* 完成日期：
*
* 修改记录1：
*    修改日期：2013年01月31日
*    版 本 号：V1.0
*    修 改 人：陈志伟
*    修改内容：创建
**********************************************************************/

/***********************************************************
 *                    其它条件编译选项                     *
***********************************************************/
#ifndef    _VNETLIB_EVENT_H_
#define    _VNETLIB_EVENT_H_

/***********************************************************
 *                   标准、非标准头文件                    *
***********************************************************/
#include "../../sky/include/sky.h"
#include "../../common/include/event.h"

//消息
#define EV_VNETLIB_POWER_ON                     (EV_POWER_ON)
#define EV_VNETLIB_BEGIN                        (TECS_VNET_EVENT_BEGIN) /* VNETLIB消息号范围30000~30500 */

//TC上消息接口,预留，总共20个
#define EV_VNETLIB_SELECT_CC                    (EV_VNETLIB_BEGIN+1)
#define EV_VNETLIB_SELECT_CC_ACK                (EV_VNETLIB_BEGIN+2)
#define EV_VNETLIB_GETRESOURCE_CC               (EV_VNETLIB_BEGIN+3)
#define EV_VNETLIB_GETRESOURCE_CC_ACK           (EV_VNETLIB_BEGIN+4)
#define EV_VNETLIB_FREERESOURCE_CC              (EV_VNETLIB_BEGIN+5)
#define EV_VNETLIB_FREERESOURCE_CC_ACK          (EV_VNETLIB_BEGIN+6)
#define EV_VNETLIB_SELECT_MIGRATE_CC            (EV_VNETLIB_BEGIN+7)

//CC上消息接口,预留，总共20个
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

//HC上消息接口,预留，总共20个
#define EV_VNETLIB_STARTNETWORK                 (EV_VNETLIB_BEGIN+41)
#define EV_VNETLIB_STARTNETWORK_ACK             (EV_VNETLIB_BEGIN+42)
#define EV_VNETLIB_STOPNETWORK                  (EV_VNETLIB_BEGIN+43)
#define EV_VNETLIB_STOPNETWORK_ACK              (EV_VNETLIB_BEGIN+44)
#define EV_VNETLIB_NOTIFY_DOMID                 (EV_VNETLIB_BEGIN+45)
#define EV_VNETLIB_NOTIFY_DOMID_ACK             (EV_VNETLIB_BEGIN+46)
#define EV_VNETLIB_NOTIFY_ALL_VMNIC             (EV_VNETLIB_BEGIN+47)
#define EV_VNETLIB_NOTIFY_ALL_VMNIC_ACK         (EV_VNETLIB_BEGIN+48)


//VNA心跳消息,预留，总共10个
#define EV_VNETLIB_HEART_BEAT_MSG               (EV_VNETLIB_BEGIN+61)
#define EV_VNETLIB_HEART_BEAT_MSG_ACK           (EV_VNETLIB_BEGIN+62) 

//VNA注册消息,预留，总共10个
#define EV_VNETLIB_REGISTER_MSG                 (EV_VNETLIB_BEGIN+71)
#define EV_VNETLIB_REGISTER_MSG_ACK             (EV_VNETLIB_BEGIN+72)

//VNA 清理
#define EV_VNETLIB_DEL_VNA                      (EV_VNETLIB_BEGIN+73)
#define EV_VNETLIB_DEL_VNA_ACK                  (EV_VNETLIB_BEGIN+74)

//通知VNA HOST归属集群消息            
#define EV_VNETLIB_HC_REGISTER_MSG              (EV_VNETLIB_BEGIN+81)

// Netplane模块
#define EV_VNETLIB_NETPLANE_CFG                 (EV_VNETLIB_BEGIN+91)

// Logic Network 模块
#define EV_VNETLIB_LOGIC_NETWORK_CFG            (EV_VNETLIB_BEGIN+101)
#define EV_VNETLIB_LOGIC_NETWORK_NAME2UUID      (EV_VNETLIB_BEGIN+102)

// 资源池
#define EV_VNETLIB_LOGI_NET_IPV4_RANGE_CFG      (EV_VNETLIB_BEGIN+111) // logic network;
#define EV_VNETLIB_NP_IPV4_RANGE_CFG            (EV_VNETLIB_BEGIN+112) // netplane;
#define EV_VNETLIB_MAC_RANGE_CFG                (EV_VNETLIB_BEGIN+113)
#define EV_VNETLIB_VLAN_RANGE_CFG               (EV_VNETLIB_BEGIN+114)
#define EV_VNETLIB_SEGMENT_RANGE_CFG            (EV_VNETLIB_BEGIN+115)


//Port Group模块
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

//PORT模块
#define EV_VNETLIB_PORT_CFG                     (EV_VNETLIB_BEGIN+141)
#define EV_VNETLIB_PORT_CFG_ACK                 (EV_VNETLIB_BEGIN+142)

//KERNAL PORT模块
#define EV_VNETLIB_KERNALPORT_CFG               (EV_VNETLIB_BEGIN+151)
#define EV_VNETLIB_KERNALPORT_CFG_ACK           (EV_VNETLIB_BEGIN+152)

//watchdog模块，计算与VNA之间交互的事件
#define EV_VNETLIB_WATCHDOG_OPER                (EV_VNETLIB_BEGIN+161)      //计算向VNA发起的看门狗定时器恢复和暂停操作
#define EV_VNETLIB_WATCHDOG_OPER_ACK            (EV_VNETLIB_BEGIN+162)      //VNA响应计算发起的看门狗定时器恢复和暂停操作
#define EV_VNETLIB_WATCHDOG_NOTIFY_HC_RESTART   (EV_VNETLIB_BEGIN+163)      //VNA通知计算重启虚拟机
#define EV_VNETLIB_WATCHDOG_NOTIFY_HC_RESTART_ACK   (EV_VNETLIB_BEGIN+164)  //计算响应VNA发起的重启虚拟机操作

//查询虚拟机网卡
#define EV_VNETLIB_QUERY_VNICS                  (EV_VNETLIB_BEGIN+170)
#define EV_VNETLIB_QUERY_VNICS_ACK              (EV_VNETLIB_BEGIN+171)

//配置LOOPBACK口
#define EV_VNETLIB_CFG_LOOPBACK                 (EV_VNETLIB_BEGIN+180)
#define EV_VNETLIB_CFG_LOOPBACK_ACK             (EV_VNETLIB_BEGIN+181)

//watchdog模块，看门狗与VNA之间交互的事件
//下面暂且保留，为了编译通过，后面删除
#define EV_VNETLIB_START_WDG                    (EV_VNETLIB_BEGIN+190)
#define EV_VNETLIB_START_WDG_ACK                (EV_VNETLIB_BEGIN+191)
#define EV_VNETLIB_STOP_WDG                     (EV_VNETLIB_BEGIN+192)
#define EV_VNETLIB_STOP_WDG_ACK                 (EV_VNETLIB_BEGIN+193)
#define EV_VNETLIB_START_WDGTIMER               (EV_VNETLIB_BEGIN+194)
#define EV_VNETLIB_START_WDGTIMER_ACK           (EV_VNETLIB_BEGIN+195)
#define EV_VNETLIB_STOP_WDGTIMER                (EV_VNETLIB_BEGIN+196)
#define EV_VNETLIB_STOP_WDGTIMER_ACK            (EV_VNETLIB_BEGIN+197)

#define EV_VNETLIB_NOTIFY_VM_RR                 (EV_VNETLIB_BEGIN+198)  //看门狗通知VNA重启虚拟机
#define EV_VNETLIB_NOTIFY_VM_RR_ACK             (EV_VNETLIB_BEGIN+199)  //VNA响应看门狗发起的重启虚拟机操作
#define EV_VNETLIB_WDG_OPER                     (EV_VNETLIB_BEGIN+200)  //VNA向看门狗发起的启动、停止看门狗操作
#define EV_VNETLIB_WDG_OPER_ACK                 (EV_VNETLIB_BEGIN+201)  //看门狗响应VNA发起的启动、停止看门狗操作
#define EV_VNETLIB_WDG_TIMER_OPER               (EV_VNETLIB_BEGIN+202)  //VNA向看门狗发起的恢复、暂停看门狗定时器操作
#define EV_VNETLIB_WDG_TIMER_OPER_ACK           (EV_VNETLIB_BEGIN+203)  //看门狗响应VNA发起的恢复、暂停看门狗定时器操作


//通配模块
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

//SDN网络相关配置
#define EV_VNETLIB_SDN_CFG                               (EV_VNETLIB_BEGIN+280)
#define EV_VNETLIB_SDN_CFG_ACK                           (EV_VNETLIB_BEGIN+281)





#endif

