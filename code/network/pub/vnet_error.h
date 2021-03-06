/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vnet_error.h
* 文件标识：见配置管理计划书
* 内容摘要：定义网络系统的所有操作返回码,SUCCESS必须为0。
* 当前版本：1.0
* 作    者：Wang.Lule
* 完成日期：2013年2月26日
*
* 修改记录1：
*     修改日期：2013/2/26
*     版 本 号：V1.0
*     修 改 人：Wang.Lule
*     修改内容：创建
******************************************************************************/
#if !defined(VNET_ERROR_INCLUDE_H__)
#define VNET_ERROR_INCLUDE_H__

#include "tecs_errcode.h"

namespace zte_tecs
{
/* ERR_VNET_BEGIN == TECS_ERRCODE_START, 1001 ~ 2000 */
#define VNET_SUCCESS                            (0)
// DB 模块
#define DB_ERROR_BEGIN                          (ERR_VNET_BEGIN)
#define DB_ERROR_GET_INST_FAILED                (DB_ERROR_BEGIN+1)              // 不能获取到DB管理器句柄

// netplane 模块
#define NETPLANE_OPER_SUCCESS                   (VNET_SUCCESS)
#define NETPLANE_ERROR_BEGIN                    (ERR_VNET_BEGIN+100)
#define NETPLANE_GET_DB_INTF_ERROR              (NETPLANE_ERROR_BEGIN+1)        // 获取DB接口ERROR
#define NETPLANE_DATA_NOT_EXIST                 (NETPLANE_ERROR_BEGIN+2)        // 网络平面数据不存在
#define NETPLANE_DATA_ALREADY_EXIST             (NETPLANE_ERROR_BEGIN+3)        // 网络平面数据已经存在
#define NETPLANE_CFG_PARAM_ERROR                (NETPLANE_ERROR_BEGIN+4)        // 网络平面配置参数错误
#define NETPLANE_GET_NETPLANEID_ERROR           (NETPLANE_ERROR_BEGIN+5)        // 网络平面获取NETPLANEID失败


// Logic network 模块
#define LOGIC_NETWORK_OPER_SUCCESS              (VNET_SUCCESS)
#define LOGIC_NETWORK_ERROR_BEGIN               (ERR_VNET_BEGIN+110)
#define LOGIC_NETWORK_GET_DB_INTF_ERROR         (LOGIC_NETWORK_ERROR_BEGIN+1)   // 获取DB接口ERROR
#define LOGIC_NETWORK_DATA_NOT_EXIST            (LOGIC_NETWORK_ERROR_BEGIN+2)   // 逻辑网络数据不存在
#define LOGIC_NETWORK_DATA_ALREADY_EXIST        (LOGIC_NETWORK_ERROR_BEGIN+3)   // 逻辑网络数据已经存在
#define LOGIC_NETWORK_CFG_PARAM_ERROR           (LOGIC_NETWORK_ERROR_BEGIN+4)   // 逻辑网络配置参数错误


// VNA 管理模块

// 资源池
#define VNET_RESOURCE_ERROR_BEGIN               (ERR_VNET_BEGIN+120)
// IP Resource Pool 模块;
#define IPV4_RANGE_OPER_SUCCESS                 (VNET_SUCCESS)
#define IPV4_RANGE_ERROR_BEGIN                  (VNET_RESOURCE_ERROR_BEGIN)
#define IPV4_RANGE_CFG_PARAM_ERROR              (IPV4_RANGE_ERROR_BEGIN+1)      // 配置参数错误
#define IPV4_RANGE_GET_LG_NET_DB_INTF_ERROR     (IPV4_RANGE_ERROR_BEGIN+2)      // 逻辑网络IP RANGE DB接口获取错误
#define IPV4_RANGE_CALC_ERROR                   (IPV4_RANGE_ERROR_BEGIN+3)      // IP RANGE合并计算参数错误
#define IPV4_RANGE_NOT_EXIST                    (IPV4_RANGE_ERROR_BEGIN+4)      // IP RANGE不存在
#define IPV4_RANGE_NOT_SAME_SUBNET              (IPV4_RANGE_ERROR_BEGIN+5)      // IP RANGE不在同一子网
#define IPV4_RANGE_INVALID                      (IPV4_RANGE_ERROR_BEGIN+6)      // IP RANGE无效
#define IPV4_RANGE_OCCUPIED                     (IPV4_RANGE_ERROR_BEGIN+7)      // IP RANGE被使用
#define IPV4_RANGE_GET_NET_PLANE_DB_INTF_ERROR  (IPV4_RANGE_ERROR_BEGIN+8)      // 网络平面IP RANGE DB接口获取错误

// MAC Resource Pool 模块;
#define MAC_RANGE_OPER_SUCCESS                  (VNET_SUCCESS)
#define MAC_RANGE_ERROR_BEGIN                   (VNET_RESOURCE_ERROR_BEGIN+10)
#define MAC_RANGE_CFG_PARAM_ERROR               (MAC_RANGE_ERROR_BEGIN+1)       // 配置参数错误
#define MAC_RANGE_CALC_ERROR                    (MAC_RANGE_ERROR_BEGIN+2)       // MAC RANGE合并计算参数错误
#define MAC_RANGE_NOT_EXIST                     (MAC_RANGE_ERROR_BEGIN+3)       // MAC RANGE不存在
#define MAC_RANGE_INVALID                       (MAC_RANGE_ERROR_BEGIN+4)       // MAC RANGE无效
#define MAC_OUI_INVALID                         (MAC_RANGE_ERROR_BEGIN+5)       // MAC OUI无效
#define MAC_PARSE_ERROR                         (MAC_RANGE_ERROR_BEGIN+6)       // MAC 解析失败
#define MAC_RANGE_OCCUPIED                      (MAC_RANGE_ERROR_BEGIN+7)       // MAC RANGE被使用
#define MAC_RANGE_GET_DB_INTF_ERROR             (MAC_RANGE_ERROR_BEGIN+8)       // MAC RANGE DB接口获取错误


// VLAN Resource Pool 模块;
#define VLAN_RANGE_OPER_SUCCESS                 (VNET_SUCCESS)
#define VLAN_RANGE_ERROR_BEGIN                  (VNET_RESOURCE_ERROR_BEGIN+20)
#define VLAN_RANGE_CFG_PARAM_ERROR              (VLAN_RANGE_ERROR_BEGIN+1)      // 配置参数错误
#define VLAN_RANGE_CALC_ERROR                   (VLAN_RANGE_ERROR_BEGIN+2)      // VLAN RANGE合并计算参数错误
#define VLAN_RANGE_NOT_EXIST                    (VLAN_RANGE_ERROR_BEGIN+3)      // VLAN RANGE不存在
#define VLAN_RANGE_OCCUPIED                     (VLAN_RANGE_ERROR_BEGIN+4)      // VLAN RANGE被使用
#define VLAN_RANGE_GET_DB_INTF_ERROR            (VLAN_RANGE_ERROR_BEGIN+5)      // VLAN RANGE DB接口获取错误

// SEGMENT Resource Pool 模块;
#define SEGMENT_RANGE_OPER_SUCCESS              (VNET_SUCCESS)
#define SEGMENT_RANGE_ERROR_BEGIN               (VNET_RESOURCE_ERROR_BEGIN+30)
#define SEGMENT_RANGE_CFG_PARAM_ERROR           (SEGMENT_RANGE_ERROR_BEGIN+1)      // 配置参数错误
#define SEGMENT_RANGE_CALC_ERROR                (SEGMENT_RANGE_ERROR_BEGIN+2)      // SEGMENT RANGE合并计算参数错误
#define SEGMENT_RANGE_NOT_EXIST                 (SEGMENT_RANGE_ERROR_BEGIN+3)      // SEGMENT RANGE不存在
#define SEGMENT_RANGE_OCCUPIED                  (SEGMENT_RANGE_ERROR_BEGIN+4)      // SEGMENT RANGE被使用
#define SEGMENT_RANGE_GET_DB_INTF_ERROR         (SEGMENT_RANGE_ERROR_BEGIN+5)      // SEGMENT RANGE DB接口获取错误


// Port Group 模块
#define PORTGROUP_OPER_SUCCESS                  (VNET_SUCCESS)
#define ERROR_PORTGROUP_BEGIN                   (ERR_VNET_BEGIN+160)
#define ERROR_PORTGROUP_OPERCODE_INVALID        (ERROR_PORTGROUP_BEGIN+1)
#define ERROR_PORTGROUP_PG_INVALID              (ERROR_PORTGROUP_BEGIN+2)
#define ERROR_PORTGROUP_INPARAM_INVALID         (ERROR_PORTGROUP_BEGIN+3)
#define ERROR_PORTGROUP_GET_INSTANCE_FAIL       (ERROR_PORTGROUP_BEGIN+4) 
#define ERROR_PORTGROUP_GET_VLANRANGE_FAIL     (ERROR_PORTGROUP_BEGIN+5)
#define ERROR_PORTGROUP_NOT_EXIST               (ERROR_PORTGROUP_BEGIN+6)
#define ERROR_PORTGROUP_IS_EXIST                (ERROR_PORTGROUP_BEGIN+7)
#define ERROR_PORTGROUP_IS_INUSING              (ERROR_PORTGROUP_BEGIN+8)
#define ERROR_PORTGROUP_OPER_DB_FAIL            (ERROR_PORTGROUP_BEGIN+9)
#define ERROR_PORTGROUP_OPER_DEFAULT_PG_FAIL   (ERROR_PORTGROUP_BEGIN+10)
#define ERROR_PORTGROUP_ACCESS_VLAN_INVALID     (ERROR_PORTGROUP_BEGIN+11)
#define ERROR_PORTGROUP_NATIVE_VLAN_INVALID     (ERROR_PORTGROUP_BEGIN+12)
#define ERROR_PORTGROUP_PRIVATE_VLAN_INVALID    (ERROR_PORTGROUP_BEGIN+13)

#define ERROR_PG_VLANRANGE_INVLAID              (ERROR_PORTGROUP_BEGIN+14)
#define ERROR_PG_VLANRANGE_IS_EXIST             (ERROR_PORTGROUP_BEGIN+15)
#define ERROR_PG_VLANRANGE_NOT_EXIST            (ERROR_PORTGROUP_BEGIN+16)
#define ERROR_PG_VLANRANGE_OPER_DB_FAIL         (ERROR_PORTGROUP_BEGIN+17)
#define ERROR_PG_VLANRANGE_CALC_ERROR           (ERROR_PORTGROUP_BEGIN+18)
#define ERROR_PG_VLANRANGE_IS_CONFLICT          (ERROR_PORTGROUP_BEGIN+19)
#define ERROR_PORTGROUP_SEGMENT_INVALID         (ERROR_PORTGROUP_BEGIN+20)
#define ERROR_PORTGROUP_PRIVATE_SEGMENT_INVALID (ERROR_PORTGROUP_BEGIN+21)



//portmgr
#define PORT_OPER_SUCCESS                       (VNET_SUCCESS) 
#define ERROR_PORT_BEGIN                        (ERR_VNET_BEGIN+220)
#define ERROR_PORT_NAME_EMPTY                   (ERROR_PORT_BEGIN+1)
#define ERROR_PORT_OPERCODE_INVALID             (ERROR_PORT_BEGIN+2)
#define ERROR_PORT_PARA_INVALID                 (ERROR_PORT_BEGIN+3)
#define ERROR_PORT_OPER_FAIL                    (ERROR_PORT_BEGIN+4)
#define ERROR_PORT_OPERDB_FAIL                  (ERROR_PORT_BEGIN+5)
#define ERROR_PORT_WC_CONFLICT                  (ERROR_PORT_BEGIN+6)
#define ERROR_PORT_CHECKDB_CONFLICT             (ERROR_PORT_BEGIN+7)


//schedule 模块
#define SCHEDULE_OPER_SUCCESS                   (VNET_SUCCESS)
#define ERROR_SCHEDULE_BEGIN                    (ERR_VNET_BEGIN+240)
#define ERR_SCHEDULE_FAILED_NO_RESOURCE         (ERROR_SCHEDULE_BEGIN+1)
#define ERR_SCHEDULE_CHECK_MAC_FAIL             (ERROR_SCHEDULE_BEGIN+3)//MAC资源调度失败
#define ERR_SCHEDULE_CHECK_IP_FAIL              (ERROR_SCHEDULE_BEGIN+4)   //IP资源调度失败
#define ERR_SCHEDULE_CC_APP_IS_NULL             (ERROR_SCHEDULE_BEGIN+5)  //进行HC调度时CC为空
#define ERR_SCHEDULE_APPOINTED_HC_INVALID       (ERROR_SCHEDULE_BEGIN+6)  //指定部署的HC信息非法，无对应的VNA信息
#define ERR_SCHEDULE_GET_DB_SCHEDULE_HANDLE_FAILED (ERROR_SCHEDULE_BEGIN+7)     //获取SCHEDULE DB句柄失败
#define ERR_SCHEDULE_GET_LG_NETWORK_FAILED      (ERROR_SCHEDULE_BEGIN+8) 
#define ERR_SCHEDULE_GET_PORT_GROUP_FAILED      (ERROR_SCHEDULE_BEGIN+9)
#define ERR_SCHEDULE_GET_PG_DETAIL_FAILED       (ERROR_SCHEDULE_BEGIN+10)
#define ERR_SCHEDULE_GET_LOOP_DEPLOYED_FAILED   (ERROR_SCHEDULE_BEGIN+11)     //从DB查询已部署LOOP虚拟机信息失败
#define ERR_SCHEDULE_LOOP_DEPLOYED_INFO_INVALID (ERROR_SCHEDULE_BEGIN+12)//已部署LOOP虚拟机信息非法
#define ERR_SCHEDULE_LOOP_DEPLOYED_CONFLICT     (ERROR_SCHEDULE_BEGIN+13)    //虚拟机不同网卡已部署LOOP信息冲突,不在相同VNA上
#define ERR_SCHEDULE_LOOP_DEPLOYED_CC_CONFLICT  (ERROR_SCHEDULE_BEGIN+14)//已部署LOOP虚拟机CC信息与指定CC冲突
#define ERR_SCHEDULE_LOOP_DEPLOYED_HC_CONFLICT  (ERROR_SCHEDULE_BEGIN+15)//已部署LOOP虚拟机HC信息与指定HC冲突
#define ERR_SCHEDULE_CALL_SCHEDULE_DB_FAIL      (ERROR_SCHEDULE_BEGIN+17)    //查询调度DB接口失败
#define ERR_SCHEDULE_GET_LN_MGR_INST_FAIL       (ERROR_SCHEDULE_BEGIN+18)  
#define ERR_SCHEDULE_GET_PG_MGR_INST_FAIL       (ERROR_SCHEDULE_BEGIN+19)  
#define ERR_SCHEDULE_CREATE_SCHEDULE_TASK_FAIL  (ERROR_SCHEDULE_BEGIN+20)  
#define ERR_SCHEDULE_SCHEDULE_TASK_INIT_FAIL    (ERROR_SCHEDULE_BEGIN+21)  
#define ERR_SCHEDULE_CHECK_SRIOV_CFG_FAIL       (ERROR_SCHEDULE_BEGIN+22)  
#define ERR_SCHEDULE_CHECK_SRIOV_CFG_FAIL_OTHER (ERROR_SCHEDULE_BEGIN+23)  
#define ERR_SCHEDULE_ALLOT_SDVSRES_FAIL         (ERROR_SCHEDULE_BEGIN+24) 
#define ERR_SCHEDULE_NO_EDVS_RES                (ERROR_SCHEDULE_BEGIN+25) 
#define ERR_SCHEDULE_NO_EDVS_BOND_RES           (ERROR_SCHEDULE_BEGIN+26) 

//vnm_vnic 模块
#define VNMVNIC_OPER_SUCCESS                    (VNET_SUCCESS)
#define ERROR_VNMVNIC_BEGIN                     (ERR_VNET_BEGIN+280)
#define ERR_VNMVNIC_GET_LN_MGR_INST_FAIL        (ERROR_VNMVNIC_BEGIN+1)
#define ERR_VNMVNIC_GET_LN_INST_FAIL            (ERROR_VNMVNIC_BEGIN+2)
#define ERR_VNMVNIC_GET_VNIC_DB_HANDLE_FAIL     (ERROR_VNMVNIC_BEGIN+3)
#define ERR_VNMVNIC_GET_VNIC_FROM_DB_FAIL       (ERROR_VNMVNIC_BEGIN+4)
#define ERR_VNMVNIC_DEL_VNIC_FROM_DB_FAIL       (ERROR_VNMVNIC_BEGIN+5)
#define ERR_VNMVNIC_ADD_VNIC_TO_DB_FAIL         (ERROR_VNMVNIC_BEGIN+6)
#define ERR_VNMVNIC_UPDATE_VNIC_TO_DB_FAIL      (ERROR_VNMVNIC_BEGIN+7)
#define ERR_VNMVNIC_ALLOC_VLAN_FAIL             (ERROR_VNMVNIC_BEGIN+8)
#define ERR_VNMVNIC_ALLOC_MAC_FAIL              (ERROR_VNMVNIC_BEGIN+9)
#define ERR_VNMVNIC_ALLOC_IP_FAIL               (ERROR_VNMVNIC_BEGIN+10)
#define ERR_VNMVNIC_ALLOC_DVS_FAIL              (ERROR_VNMVNIC_BEGIN+11)
#define ERR_VNMVNIC_FREE_VLAN_FAIL              (ERROR_VNMVNIC_BEGIN+12)
#define ERR_VNMVNIC_FREE_MAC_FAIL               (ERROR_VNMVNIC_BEGIN+13)
#define ERR_VNMVNIC_FREE_IP_FAIL                (ERROR_VNMVNIC_BEGIN+14)
#define ERR_VNMVNIC_FREE_DVS_FAIL               (ERROR_VNMVNIC_BEGIN+15)
#define ERR_VNMVNIC_GET_PG_DETAIL_FAIL          (ERROR_VNMVNIC_BEGIN+16)
#define ERR_VNMVNIC_GET_VLAN_DETAIL_FAIL        (ERROR_VNMVNIC_BEGIN+17)
#define ERR_VNMVNIC_GET_DVS_DETAIL_FAIL         (ERROR_VNMVNIC_BEGIN+18)
#define ERR_VNMVNIC_GET_VNA_UUID_FAIL           (ERROR_VNMVNIC_BEGIN+19)
#define ERR_VNMVNIC_GET_VM_DB_HANDLE_FAIL       (ERROR_VNMVNIC_BEGIN+20)
#define ERR_VNMVNIC_GET_VM_FROM_DB_FAIL         (ERROR_VNMVNIC_BEGIN+21)
#define ERR_VNMVNIC_DEL_VM_FROM_DB_FAIL         (ERROR_VNMVNIC_BEGIN+22)
#define ERR_VNMVNIC_ADD_VM_TO_DB_FAIL           (ERROR_VNMVNIC_BEGIN+23)
#define ERR_VNMVNIC_UPDATE_VM_TO_DB_FAIL        (ERROR_VNMVNIC_BEGIN+24)
#define ERR_VNMVNIC_CC_APP_CONFLICT             (ERROR_VNMVNIC_BEGIN+25)
#define ERR_VNMVNIC_ALLOC_SEGMENT_FAIL          (ERROR_VNMVNIC_BEGIN+26)
#define ERR_VNMVNIC_FREE_SEGMENT_FAIL           (ERROR_VNMVNIC_BEGIN+27)
#define ERR_VNMVNIC_GET_SEGMENT_DETAIL_FAIL     (ERROR_VNMVNIC_BEGIN+28)
#define ERR_VNMVNIC_GET_PG_MGR_INST_FAIL        (ERROR_VNMVNIC_BEGIN+29)



//wdg
#define WDG_SUCCESS                             (VNET_SUCCESS) 
#define ERROR_WDG_BEGIN                         (ERR_VNET_BEGIN+320)
#define ERROR_WDG_TIMEROUT                      (ERROR_WDG_BEGIN+1)
#define ERROR_WDG_MAC                           (ERROR_WDG_BEGIN+2)
#define ERROR_WDG_START                         (ERROR_WDG_BEGIN+3)
#define ERROR_WDG_STOP                          (ERROR_WDG_BEGIN+4)
#define ERROR_WDG_STARTTIMER                    (ERROR_WDG_BEGIN+5)
#define ERROR_WDG_STOPTIMER                     (ERROR_WDG_BEGIN+6)
#define ERROR_WDG_LOCK                          (ERROR_WDG_BEGIN+7)
#define ERROR_WDG_TIMER                         (ERROR_WDG_BEGIN+8)

//vna_agent模块
#define ERROR_VNA_AGENT_BEGIN                   (ERR_VNET_BEGIN+360)
#define ERROR_VNA_AGENT_MODULE_OFFLINE          (ERROR_VNA_AGENT_BEGIN+1)       //vna模块状态是OFFLINE
#define ERROR_VNA_AGENT_MODULE_NOT_REG          (ERROR_VNA_AGENT_BEGIN+2)       //vna模块还没有注册
#define ERROR_VNA_DELETE_FAILED                 (ERROR_VNA_AGENT_BEGIN+3)       //vna删除失败
#define ERROR_VNA_DELETE_DB_OPER_FAILED         (ERROR_VNA_AGENT_BEGIN+4)       //vna删除db操作失败
#define ERROR_VNA_AGENT_MODULE_ONLINE           (ERROR_VNA_AGENT_BEGIN+5)         //vna模块状态是ONLINE


//vna_vnic模块
#define ERROR_VNA_VNIC_BEGIN                    (ERR_VNET_BEGIN+400)
#define ERROR_VNA_VNIC_WDG_INPUT_PARAM_INVALID  (ERROR_VNA_VNIC_BEGIN+1)       //输入参数非法
#define ERROR_VNA_VNIC_GET_VSI_FROM_VNM_FAIL    (ERROR_VNA_VNIC_BEGIN+2)       //向VNM请求VSI详细信息失败
#define ERROR_VNA_VNIC_GET_REGED_VNM_FAIL       (ERROR_VNA_VNIC_BEGIN+3)       //获取所注册的VNM信息失败
#define ERROR_VNA_VNIC_WDG_OPER_FAIL            (ERROR_VNA_VNIC_BEGIN+4)       //获取所注册的VNM信息失败
#define ERROR_VNA_VNIC_ALLOC_DVS_FAIL           (ERROR_VNA_VNIC_BEGIN+5)       //获取所注册的VNM信息失败
#define ERROR_VNA_VNIC_FREE_DVS_FAIL            (ERROR_VNA_VNIC_BEGIN+6)       //获取所注册的VNM信息失败
#define ERROR_VNA_VNIC_VSI_EVB_STATUS_FAIL      (ERROR_VNA_VNIC_BEGIN+7)       //EVB 状态失败


// wildcast switch模块
#define WILDCAST_OPER_SUCCESS                   (VNET_SUCCESS)
#define WC_SWITCH_OPER_SUCCESS                  (WILDCAST_OPER_SUCCESS)   
#define ERROR_WC_SWITCH_BEGIN                   (ERR_VNET_BEGIN+410)
#define ERROR_WC_SWITCH_OPERCODE_INVALID        (ERROR_WC_SWITCH_BEGIN+1)
#define ERROR_WC_SWITCH_INSTANCE_FAIL           (ERROR_WC_SWITCH_BEGIN+2)    

#define ERROR_WC_SWITCH_NAME_EMPTY              (ERROR_WC_SWITCH_BEGIN+3)
#define ERROR_WC_SWITCH_PG_EMPTY                (ERROR_WC_SWITCH_BEGIN+4)
#define ERROR_WC_SWITCH_PGTYPE_INVALID          (ERROR_WC_SWITCH_BEGIN+5)
#define ERROR_WC_SWITCH_EVBMODE_INVALID         (ERROR_WC_SWITCH_BEGIN+6)
#define ERROR_WC_SWITCH_ALREADY_EXIST           (ERROR_WC_SWITCH_BEGIN+7)

#define ERROR_WC_SWITCH_GET_DB_FAIL             (ERROR_WC_SWITCH_BEGIN+8)
#define ERROR_WC_SWITCH_SET_DB_FAIL             (ERROR_WC_SWITCH_BEGIN+9)
#define ERROR_WC_SWITCH_NOTEXIST_PORT           (ERROR_WC_SWITCH_BEGIN+10)
#define ERROR_WC_SWITCH_PARAM_INVALID           (ERROR_WC_SWITCH_BEGIN+11)
#define ERROR_WC_SWITCH_UPLINKPORT_IS_CONFLICT  (ERROR_WC_SWITCH_BEGIN+12)
#define ERROR_WC_SWITCH_MIX_UPLINKPG_MUST_SAME  (ERROR_WC_SWITCH_BEGIN+13)
#define ERROR_WC_SWITCH_MIX_UPLINK_BONDMODE_MUST_SAME  (ERROR_WC_SWITCH_BEGIN+14)
#define ERROR_WC_SWITCH_MIX_EVBMODE_MUST_SAME  (ERROR_WC_SWITCH_BEGIN+15)

// wildcast create vport模块
#define WC_CREATE_VPORT_OPER_SUCCESS            (VNET_SUCCESS)   
#define ERROR_WC_CREATE_VPORT_BEGIN             (ERR_VNET_BEGIN+430)
#define ERROR_WC_CREATE_VPORT_GET_DB_FAIL       (ERROR_WC_CREATE_VPORT_BEGIN+1)
#define ERROR_WC_CREATE_VPORT_SET_DB_FAIL       (ERROR_WC_CREATE_VPORT_BEGIN+2)
#define ERROR_WC_CREATE_VPORT_ALREADY_EXIST     (ERROR_WC_CREATE_VPORT_BEGIN+3)
#define ERROR_WC_CREATE_VPORT_NAME_EMPTY        (ERROR_WC_CREATE_VPORT_BEGIN+4)
#define ERROR_WC_CREATE_VPORT_OPERCODE_INVALID  (ERROR_WC_CREATE_VPORT_BEGIN+5)
#define ERROR_WC_GET_PORT_INSTANCE_NULL         (ERROR_WC_CREATE_VPORT_BEGIN+6)
#define ERROR_WC_GET_SWTICH_INSTANCE_NULL       (ERROR_WC_CREATE_VPORT_BEGIN+7)
#define ERROR_WC_UPLINKPG_MTU_LESS_KNPG_MTU     (ERROR_WC_CREATE_VPORT_BEGIN+8)



// wildcast loopback 模块
#define WC_LOOPBACK_OPER_SUCCESS            (VNET_SUCCESS)   
#define ERROR_WC_LOOPBACK_BEGIN             (ERR_VNET_BEGIN+440)
#define ERROR_WC_LOOPBACK_GET_DB_FAIL       (ERROR_WC_LOOPBACK_BEGIN+1)
#define ERROR_WC_LOOPBACK_SET_DB_FAIL       (ERROR_WC_LOOPBACK_BEGIN+2)
#define ERROR_WC_LOOPBACK_ALREADY_EXIST     (ERROR_WC_LOOPBACK_BEGIN+3)
#define ERROR_WC_LOOPBACK_NAME_EMPTY        (ERROR_WC_LOOPBACK_BEGIN+4)
#define ERROR_WC_LOOPBACK_OPERCODE_INVALID  (ERROR_WC_LOOPBACK_BEGIN+5)

// vm migrate 模块
#define VNM_VM_MIGRATE_OPER_SUCCESS                (VNET_SUCCESS)   
#define ERROR_VNM_VM_MIGRATE_BEGIN                 (ERR_VNET_BEGIN+450)
#define ERROR_VNM_VM_MIGRATE_GET_DB_FAIL           (ERROR_VNM_VM_MIGRATE_BEGIN+1)
#define ERROR_VNM_VM_MIGRATE_SET_DB_FAIL           (ERROR_VNM_VM_MIGRATE_BEGIN+2)
#define ERROR_VNM_VM_MIGRATE_HAS_EXISTED           (ERROR_VNM_VM_MIGRATE_BEGIN+3)
#define ERROR_VNM_VM_MIGRATE_PARAM_INVALID         (ERROR_VNM_VM_MIGRATE_BEGIN+4)
#define ERROR_VNM_VM_MIGRATE_INPUTPARAM_DB_NOT_SAME (ERROR_VNM_VM_MIGRATE_BEGIN+5)
#define ERROR_VNM_VM_MIGRATE_INSTANCE_IS_NULL       (ERROR_VNM_VM_MIGRATE_BEGIN+6)
#define ERROR_VNM_VM_MIGRATE_IS_CONFLICT            (ERROR_VNM_VM_MIGRATE_BEGIN+7)

// vxlan 模块
#define VXLAN_SUCCESS                           (VNET_SUCCESS)   
#define VXLAN_ERROR                             (ERR_VNET_BEGIN+460)
#define ERROR_VXLAN_BRIDGE_CREATE_FAIL          (VXLAN_ERROR+1)
#define ERROR_VXLAN_BRIDGE_DELETE_FAIL          (VXLAN_ERROR+2)
#define ERROR_VXLAN_REQ_MCGROUP_FAIL            (VXLAN_ERROR+3)

//sdn 模块
#define SDN_OPER_SUCCESS                    (VNET_SUCCESS)
#define ERROR_SDN_BEGIN                     (ERR_VNET_BEGIN+470)
#define SDN_CFG_PARAM_ERROR                 (ERROR_SDN_BEGIN+1)
#define SDN_GET_DB_INTF_ERROR               (ERROR_SDN_BEGIN+2)
#define SDN_DATA_ALREADY_EXIST              (ERROR_SDN_BEGIN+3)
#define SDN_DATA_NOT_EXIST                  (ERROR_SDN_BEGIN+4)

//switch 模块
#define SWITCH_OPER_SUCCESS                     (VNET_SUCCESS)
#define ERROR_SWITCH_BEGIN                      (ERR_VNET_BEGIN+500)
#define ERROR_SWITCH_OPERCODE_INVALID           (ERROR_SWITCH_BEGIN+1)
#define ERROR_SWITCH_INSTANCE_FAIL              (ERROR_SWITCH_BEGIN+2)    
#define ERROR_SWITCH_GET_PORTMGR_FAIL           (ERROR_SWITCH_BEGIN+3)
#define ERROR_SWITCH_GET_VSWITCH_DB_FAIL        (ERROR_SWITCH_BEGIN+4)
#define ERROR_SWITCH_GET_VSWITCHMAP_DB_FAIL     (ERROR_SWITCH_BEGIN+5)
#define ERROR_SWTICH_CONFLICT_WITH_WILDCAST     (ERROR_SWITCH_BEGIN+6)
#define ERROR_SWTICH_PORT_INVALID               (ERROR_SWITCH_BEGIN+7)
#define ERROR_SWTICH_GET_PGINFO_FAIL            (ERROR_SWITCH_BEGIN+8)
#define ERROR_SWITCH_NAME_INVALID               (ERROR_SWITCH_BEGIN+9)
#define ERROR_SWITCH_ALREADY_EXIST              (ERROR_SWITCH_BEGIN+10)
#define ERROR_SWITCH_IS_USING                   (ERROR_SWITCH_BEGIN+11)
#define ERROR_SWITCH_OPER_DB_FAIL               (ERROR_SWITCH_BEGIN+12) 
#define ERROR_SWITCH_PORT_OPER_DB_FAIL          (ERROR_SWITCH_BEGIN+13) 
#define ERROR_SWITCH_DEL_NOT_FINISH             (ERROR_SWITCH_BEGIN+14) 
#define ERROR_SWITCH_GET_PORTINFO_FAIL          (ERROR_SWITCH_BEGIN+15)
#define ERROR_SWITCH_ADD_BOND_FAIL              (ERROR_SWITCH_BEGIN+16)  
#define ERROR_SWITCH_DEL_BOND_FAIL              (ERROR_SWITCH_BEGIN+17)  
#define ERROR_SWITCH_DEL_BONDSLAVE_FAIL         (ERROR_SWITCH_BEGIN+18)
#define ERROR_SWITCH_GET_BONDSLAVE_FAIL         (ERROR_SWITCH_BEGIN+19)
#define ERROR_SWITCH_ADD_BRIDGE_FAIL            (ERROR_SWITCH_BEGIN+20)   
#define ERROR_SWITCH_DEL_BRIDGE_FAIL            (ERROR_SWITCH_BEGIN+21)  
#define ERROR_SWITCH_SET_ATTR_FAIL              (ERROR_SWITCH_BEGIN+22)
#define ERROR_SWITCH_PORT_ISCONFLICT            (ERROR_SWITCH_BEGIN+23)
#define ERROR_SWITCH_PORT_ISMIXED               (ERROR_SWITCH_BEGIN+24)
#define ERROR_SWITCH_OPER_KERNEL_DVS_FAIL       (ERROR_SWITCH_BEGIN+25)  
#define ERROR_SWITCH_QUERY_VNAUUID_FAIL         (ERROR_SWITCH_BEGIN+26)   
#define ERROR_SWITCH_MERGE_REQINFO_FAIL         (ERROR_SWITCH_BEGIN+27)   
#define ERROR_SWITCH_GET_SRIOV_VFS_FAIL         (ERROR_SWITCH_BEGIN+28) 
#define ERROR_SWITCH_ADD_UPLINK_FAIL            (ERROR_SWITCH_BEGIN+29) 
#define ERROR_SWITCH_ADD_EVBPORT_FAIL           (ERROR_SWITCH_BEGIN+30)
#define ERROR_SWITCH_DEL_EVBPORT_FAIL           (ERROR_SWITCH_BEGIN+31)
#define ERROR_SWITCH_RENAME_BRIDGE_FAIL         (ERROR_SWITCH_BEGIN+32)
#define ERROR_SWITCH_CHECK_PARAM_FAIL           (ERROR_SWITCH_BEGIN+33)
#define ERROR_SWITCH_TIMER_CONFIG_FAIL          (ERROR_SWITCH_BEGIN+34)
#define ERROR_SWITCH_GET_VNAINFO_FAIL           (ERROR_SWITCH_BEGIN+35)
#define ERROR_SWITCH_GET_SDN_CFG                (ERROR_SWITCH_BEGIN+36)
#define ERROR_SWITCH_MODIFY_FAIL                (ERROR_SWITCH_BEGIN+37)
#define ERROR_SWITCH_ADD_VTEP_FAIL              (ERROR_SWITCH_BEGIN+38)
#define ERROR_SWITCH_DEL_VTEP_FAIL              (ERROR_SWITCH_BEGIN+39)
#define ERROR_SWITCH_QUERY_VTEP_FAIL            (ERROR_SWITCH_BEGIN+40)

}

#endif 

