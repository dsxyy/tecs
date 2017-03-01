/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�mid.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ������ϵͳ�����н������ơ���Ϣ��Ԫ�����鲥��
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
*     �޸����ݣ�����HostManager/HostAgent/HostHandler��ģ�����Ϣ��Ԫ�����鲥
* �޸ļ�¼3��
*     �޸����ڣ�2011/8/26
*     �� �� �ţ�V1.0
*     �� �� �ˣ��Ѻ챣
*     �޸����ݣ�����VmHandler/InstanceManager
* �޸ļ�¼4��
*     �޸����ڣ�2011/9/26
*     �� �� �ţ�V1.0
*     �� �� �ˣ�������
*     �޸����ݣ�����TCFileStorage���̡�CCFileStorage���̡�FTPServer���̡�HCStorageAgent����
******************************************************************************/
#ifndef COMMON_MID_H
#define COMMON_MID_H
namespace zte_tecs
{
//commonģ�����Ϣ��Ԫ����
#define ALARM_AGENT                     "alarm_agent"
#define MU_VACUUM_MANAGER               "vacuum_manager"

//TC�ϸ��������������������е���Ϣ��Ԫ����
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


//TC�����е�IMAGE_MANAGER����������Ϣ��Ԫ����
#define PROC_IMAGE_MANAGER             "image_manager"
#define MU_IMAGE_MANAGER               "image_manager"
#define MU_PROJECT_IMAGE_AGENT        "project_image_agent"


//FILE_MANAGER�������ƺ���Ϣ��Ԫ����
#define PROC_FILE_MANAGER              "file_manager"
#define MU_FILE_MANAGER                "file_manager"

//SC�ϸ��������������������е���Ϣ��Ԫ����
#define PROC_SC                        "sc"
#define PROC_SA                        "sa"

//CC�ϸ��������������������е���Ϣ��Ԫ����
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

/* NAT���ؽ��̼���Ϣ��Ԫ����*/
#define PROC_NAT                   "nat"
#define MU_NAT_RULE_MANAGER        "mu_cc_net_manager"

//�汾����Ľ���������Ϣ��Ԫ��
#define PROC_VERSION_MANAGER        "proc_version"
#define PROC_VERSION_AGENT          "proc_version_agent"
#define MU_VERSION_MANAGER              "version_manager"
#define MU_VERSION_AGENT                "version_agent"
#define GROUP_VERSION_CC                "version_group_cc"
#define GROUP_VERSION_HC                "version_group_hc"

//HC�ϸ��������������������е���Ϣ��Ԫ����
#define PROC_HC                         "hc"
#define MU_HOST_HANDLER                 "host_handler"
#define MU_HOST_AGENT                   "host_agent"
#define MU_HOST_HEARTBEAT               "host_heartbeat"

#define MU_VM_HANDLER                   "vm_handler"
#define MU_INSTANCE_MANAGER             "instance_manager"
#define MU_VNET_CONTROLLER              "mu_vnet_controller"
#define MU_HC_FILE_STORAGE              "hc_file_storage"  //HC �ϴ洢����
#define VNET_HC_NETPLANE_INFO_MG        "hc_net_info_mg" // HC����ƽ�������鲥��
#define MU_VNET_WD                      "mu_vnet_WD"     //���Ź���Ϣ��Ԫ
#define MU_VNET_20MS_TIMER              "mu_vnet_20ms_timer"    // 20ms��ʱ����Ϣ��Ԫ
#define MU_VNET_MONITOR_MGR             "mu_vnet_monitor_mgr"

// HC�����н��̶������е�
#define MU_PROC_ADMIN                   "proc_admin"
#define MU_STORAGE_VOLTMP                "storage_voltmp"
#define MU_STORAGE_VOLUME               "storae_volume"
#define MU_SSH_TRUST                     "ssh_trust"

//sa
#define MU_STORAGE_ADAPTOR              "storage_adaptor_"
#define MU_STORAGE_SAMAIN               "mu_storage_samain"

/******************************************************************************/
#define MUTIGROUP_HOST_DISCOVER     "host_discover"         // ����CC�����룬��������������Ϣ
#define MUTIGROUP_CLUSTER_DISCOVER  "cluster_discover"      // TC����Ҫ���룬���ռ�Ⱥ������Ϣ
#define MUTIGROUP_SUF_HOST_HANDLE   "_host_handler"         // HostName_host_handler
#define MUTIGROUP_SUF_REG_SYSTEM    "_reg_system"           // ApplicationName_reg_system

#define MUTIGROUP_SA_DISCOVER       "sa_discover"      // TC����Ҫ���룬���ռ�Ⱥ������Ϣ
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

