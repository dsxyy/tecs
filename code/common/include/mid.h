/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：mid.h
* 文件标识：见配置管理计划书
* 内容摘要：定义系统的所有进程名称、消息单元名、组播组
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
*     修改内容：增加HostManager/HostAgent/HostHandler等模块的消息单元名和组播
* 修改记录3：
*     修改日期：2011/8/26
*     版 本 号：V1.0
*     修 改 人：缪红保
*     修改内容：增加VmHandler/InstanceManager
* 修改记录4：
*     修改日期：2011/9/26
*     版 本 号：V1.0
*     修 改 人：梁庆永
*     修改内容：增加TCFileStorage进程、CCFileStorage进程、FTPServer进程、HCStorageAgent代理
******************************************************************************/
#ifndef COMMON_MID_H
#define COMMON_MID_H
namespace zte_tecs
{
//common模块的消息单元名称
#define ALARM_AGENT                     "alarm_agent"
#define MU_VACUUM_MANAGER               "vacuum_manager"

//TC上各个进程名，各个进程中的消息单元名称
#define PROC_APISVR                 "apisvr"
#define PROC_TC                     "tc"
#define MU_CLOUD_NODE_MANAGER           "cloud_node_manager"
#define MU_CLUSTER_MANAGER              "cluster_manager"
#define MU_PROJECT_MANAGER              "project_manager"
#define MU_VMTEMPLATE_MANAGER           "vmtemplate_manager"
#define MU_VMCFG_MANAGER                "vmcfg_manager"
#define ALARM_MANAGER                   "alarm_manager"
#define MU_VNET_CONFIG                  "mu_vnet_config"
#define SNMP_AGENT                      "snmp_agent"
#define OP_LOG_AGENT                    "operator_log_agent"
#define LOG_AGENT                       "log_agent"
#define INSTANCE_MANAGER                "Instance_manager"
#define MU_SA_MANAGER                   "storage_adaptor_manager"
#define MU_SC_CONTROL                   "storage_control"
#define  ALARM_GROUP          "alarm_grp"
#define AUTHMANAGER   "AuthManager"
#define DRIVERMANAGER "AuthDriverManager"
#define AUTHREQUEST   "AuthRequest"
#define MU_PROJECT_MANAGER              "project_manager"


//TC上运行的IMAGE_MANAGER进程名及消息单元名称
#define PROC_IMAGE_MANAGER             "image_manager"
#define MU_IMAGE_MANAGER               "image_manager"
#define MU_PROJECT_IMAGE_AGENT        "project_image_agent"


//FILE_MANAGER进程名称和消息单元名称
#define PROC_FILE_MANAGER              "file_manager"
#define MU_FILE_MANAGER                "file_manager"

//SC上各个进程名，各个进程中的消息单元名称
#define PROC_SC                        "sc"
#define PROC_SA                        "sa"

//CC上各个进程名，各个进程中的消息单元名称
#define PROC_CC                     "cc"
#define MU_VM_SCHEDULER                 "scheduler"
#define MU_VM_LIFECYCLE_MANAGER         "lc_manager"
#define MU_VM_MANAGER                   "vm_manager"
#define MU_VM_HA                        "vm_ha"
#define MU_VM_OPERATOR                  "vm_operator"
#define MU_HOST_MANAGER                 "host_manager"
#define MU_CLUSTER_AGENT                "cluster_agent"
#define MU_HOST_HIGHAVAILABLE           "host_highavaible"
#define MU_VNET_MANAGER                 "mu_vnet_manager"
#define MU_NETADAPTER                   "mu_netadapter"
#define MU_MACIP_SYN_MGR                "mu_macip_syn_mgr"
#define MU_DISK_AGENT                   "mu_disk_agent"
#define MU_POWER_AGENT                  "power_agent"
#define MU_POWER_MANAGER                "power_manager"

#define PROC_IMAGE_AGENT                "image_agent"
#define MU_IMAGE_AGENT                  "image_agent"
#define MU_IMAGE_DOWNLOAD_AGENT         "image_download_agent"

#define EXC_LOG_AGENT "exc_log_agent"

/* NAT网关进程及消息单元名称*/
#define PROC_NAT                   "nat"
#define MU_NAT_RULE_MANAGER        "mu_cc_net_manager"

//版本管理的进程名和消息单元名
#define PROC_VERSION_MANAGER        "proc_version"
#define PROC_VERSION_AGENT          "proc_version_agent"
#define MU_VERSION_MANAGER              "version_manager"
#define MU_VERSION_AGENT                "version_agent"
#define GROUP_VERSION_CC                "version_group_cc"
#define GROUP_VERSION_HC                "version_group_hc"

//HC上各个进程名，各个进程中的消息单元名称
#define PROC_HC                         "hc"
#define MU_HOST_HANDLER                 "host_handler"
#define MU_HOST_AGENT                   "host_agent"
#define MU_HOST_HEARTBEAT               "host_heartbeat"

#define MU_VM_HANDLER                   "vm_handler"
#define MU_INSTANCE_MANAGER             "instance_manager"
#define MU_VNET_CONTROLLER              "mu_vnet_controller"
#define MU_HC_FILE_STORAGE              "hc_file_storage"  //HC 上存储代理
#define VNET_HC_NETPLANE_INFO_MG        "hc_net_info_mg" // HC网络平面配置组播组
#define MU_VNET_WD                      "mu_vnet_WD"     //看门狗消息单元
#define MU_VNET_20MS_TIMER              "mu_vnet_20ms_timer"    // 20ms定时器消息单元
#define MU_VNET_MONITOR_MGR             "mu_vnet_monitor_mgr"

// HC上所有进程都可以有的
#define MU_PROC_ADMIN                   "proc_admin"
#define MU_STORAGE_VOLTMP                "storage_voltmp"
#define MU_STORAGE_VOLUME               "storae_volume"
#define MU_SSH_TRUST                     "ssh_trust"

//sa
#define MU_STORAGE_ADAPTOR              "storage_adaptor_"
#define MU_STORAGE_SAMAIN               "mu_storage_samain"

/******************************************************************************/
#define MUTIGROUP_HOST_DISCOVER     "host_discover"         // 所有CC都加入，接收主机发现消息
#define MUTIGROUP_CLUSTER_DISCOVER  "cluster_discover"      // TC上需要加入，接收集群发现消息
#define MUTIGROUP_SUF_HOST_HANDLE   "_host_handler"         // HostName_host_handler
#define MUTIGROUP_SUF_REG_SYSTEM    "_reg_system"           // ApplicationName_reg_system

#define MUTIGROUP_SA_DISCOVER       "sa_discover"      // TC上需要加入，接收集群发现消息
#define MUTIGROUP_SSH_TRUST         "_ssh_trust"           
/******************************************************************************/
#define PROC_SPECIAL_DEVICE         "special_device"



/******************************************************************************/
#define MS_SERVER_VER_TC            0
#define MS_SERVER_VER_CC            1

#define MS_SERVER_TC                100
#define MS_SERVER_IMAGE_MANAGER     101
#define MS_SERVER_IMAGE_PROXY       103
#define MS_SERVER_API               102
#define MS_SERVER_SC                104
#define MS_SERVER_FILE_MANAGER      105

#define MS_SERVER_CC                200
#define MS_SERVER_CC_STORAGE        201
}
using namespace zte_tecs;
#endif // ifnedf COMMON_MID_H

