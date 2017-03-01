/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：register_xmlrpc_methods.cpp
* 文件标识：见配置管理计划书
* 内容摘要：XMLRPC方法注册的实现文件
* 当前版本：1.0
* 作    者：马兆勉
* 完成日期：2011年7月22日
*
* 修改记录1：
*     修改日期：2011/7/22
*     版 本 号：V1.0
*     修 改 人：马兆勉
*     修改内容：创建
*******************************************************************************/
#include "request_manager.h"
#include "userapi.h"
#include "tecs_system_methods.h"
#include "cluster_manager_methods.h"
#include "image_manager_methods.h"
#include "project_manager_methods.h"
#include "vmcfg_manager_methods.h"
#include "vmtemplate_manager_methods.h"
#include "host_manager_methods.h"
#include "version_manager_methods.h"
#include "current_alarm_info_method.h"
#include "file_manager_methods.h"
#include "user_manager_methods.h"
#include "snmp_info_method.h"
#include "cluster_agent_methods.h"
#include "sa_manager_methods.h"
#include "storage_control_methods.h"
#include "power_manager_methods.h"
#include "vnet_xmlrpc_netplane_method.h"
#include "vnet_xmlrpc_logicnet_method.h"
#include "vnet_xmlrpc_portgroup_method.h"
#include "vnet_xmlrpc_switch_method.h"
#include "vnet_xmlrpc_kernalport_method.h"
#include "vnet_xmlrpc_loopport_method.h"
#include "affinity_region_manager_methods.h"
#include "vnet_xmlrpc_sdn_method.h"
#include "workflow_methods.h"

namespace zte_tecs
{
/******************************************************************************/
void RequestManager::RegisterXmlrpcMethods()
{
#define ADD_METHOD(NAME, OBJECT) AddMethod(NAME, new OBJECT(_upool))
    //注册XMLRPC方法，示例如下:
    //ADD_METHOD("tecs.test.query", TecsRpcTestMethod);
    //ADD_METHOD("tecs.test.test", TecsRpcTestMethod);
    /* 注册tecs系统管理相关的API调用*/
    ADD_METHOD("tecs.system.runtime.query", SysRuntimeInfoQueryMethod);

    /* 注册tecs许可证查询接口 */
    ADD_METHOD("tecs.system.license.query", LicenseQueryMethod);

    /* 注册tecs硬件指纹查询接口 */
    ADD_METHOD("tecs.system.license.hardware_fingerprint.query", HardwareFingerprintQueryMethod);

    /* 注册集群管理的API调用 */
    // 集群注册取消，修改为设置集群
    ADD_METHOD("tecs.cluster.set", ClusterSet);

    // 集群信息请求
    ADD_METHOD("tecs.cluster.query", ClusterQuery);

    // 集群注销取消，修改为遗忘集群
    ADD_METHOD("tecs.cluster.forget", ClusterForget);

    // 集群停止
    ADD_METHOD("tecs.cluster.disable", ClusterPreserveStart);

    // 集群启动
    ADD_METHOD("tecs.cluster.enable", ClusterPreserveStop);

    //ADD_METHOD("tecs.cluster.query_netplane", ClusterQueryNetPlane);

    ADD_METHOD("tecs.cluster.query_core_tcu", ClusterQueryCoreAndTcu);

    // 物理机管理模块注册的XMLRPC方法
    ADD_METHOD("tecs.host.set", HostSetMethod);

    ADD_METHOD("tecs.host.forget", HostForgetMethod);

    ADD_METHOD("tecs.host.disable", HostDisableMethod);

    ADD_METHOD("tecs.host.enable", HostEnableMethod);

    ADD_METHOD("tecs.host.reboot", HostRebootMethod);

    ADD_METHOD("tecs.host.shutdown", HostShutdownMethod);

    //xml
    ADD_METHOD("tecs.host.extern_ctrl_cmd", HostExternCtrlCmdMethod);

    ADD_METHOD("tecs.host.extern_ctrl.query",HostExternCtrlQueryMethod);

    //ADD_METHOD("tecs.host.extern_ctrl_cfg_set",HostExternCtrlCfgSetMethod);

    //ADD_METHOD("tecs.host.extern_ctrl_cfg_get",HostExternCtrlCfgGetMethod);

#if 0
    ADD_METHOD("tecs.host.command", HostCommandMethod);
#endif


    ADD_METHOD("tecs.host.query", HostQueryMethod);

    ADD_METHOD("tecs.tcu.set", TcuConfigSetMethod);

    ADD_METHOD("tecs.tcu.delete", TcuConfigDelMethod);

    ADD_METHOD("tecs.tcu.query", TcuConfigQueryMethod);

    ADD_METHOD("tecs.cmm.add", CmmConfigAddMethod);

    ADD_METHOD("tecs.cmm.set", CmmConfigSetMethod);

    ADD_METHOD("tecs.cmm.delete", CmmConfigDeleteMethod);

    ADD_METHOD("tecs.cmm.query", CmmConfigQueryMethod);

    ADD_METHOD("tecs.cmm.fantray_rpmctl",FantrayRPMCtlMethod);

    ADD_METHOD("tecs.save_energy.set", ClusterConfigSetSaveEnergyParaMethod);

    ADD_METHOD("tecs.save_energy.query", ClusterConfigSaveEnergyParaQueryMethod);

    ADD_METHOD("tecs.scheduler.set_policy", SchedulerPolicySetMethod);
    ADD_METHOD("tecs.scheduler.query_policy", SchedulerPolicyQueryMethod);

    ADD_METHOD("tecs.vm_ha.set_policy", VMHAPolicySetMethod);
    ADD_METHOD("tecs.vm_ha.query_policy", VMHAPolicyQueryMethod);

#if 0
    ADD_METHOD("tecs.cpuinfo.query", CpuInfoQueryMethod);
#endif

    /************************    映像管理相关   *************************/
    // 映像注册
    // ADD_METHOD("tecs.image.register", ImageRegisterMethod);

    // 映像注销
    // ADD_METHOD("tecs.image.unregister", ImageDeregisterMethod);

    // 映像删除
    ADD_METHOD("tecs.image.delete", ImageDeleteMethod);

    // 映像修改
    ADD_METHOD("tecs.image.set", ImageModifyMethod);

    // 映像查询
    ADD_METHOD("tecs.image.query", ImageQueryMethod);

    // 映像使能
    ADD_METHOD("tecs.image.enable", ImageEnableMethod);

    // 映像publish
    ADD_METHOD("tecs.image.publish", ImagePublishMethod);

    // 获取映像ftp服务
    ADD_METHOD("tecs.image.ftp_svr_info_get", ImageFtpSvrInfoGetMethod);


    // 映像部署
    ADD_METHOD("tecs.image.deploy", ImageDeployMethod);

    // 映像撤销
    ADD_METHOD("tecs.image.cancel", ImageCancelMethod);

    // 映像服务器空间查询
    ADD_METHOD("tecs.image.getspace", ImageGetSpaceMethod);

     /************************    文件管理相关   *************************/
    // 文件查询
    //文件管理的coredump文件保存URL地址查询接口
    ADD_METHOD("tecs.file.query_coredump_url", FileGetCoredumpUrlMethod);

    /************************    工程管理相关   *************************/
    // 工程创建
    ADD_METHOD("tecs.project.allocate", ProjectAllocateMethod);

    // 工程删除
    ADD_METHOD("tecs.project.delete", ProjectDeleteMethod);

    // 工程修改
    ADD_METHOD("tecs.project.set", ProjectModifyMethod);
    // 工程操作
    ADD_METHOD("tecs.project.action", ProjectActionMethod);
    // 工程克隆
    ADD_METHOD("tecs.project.clone", ProjectCloneMethod);
    // 工程查询
    ADD_METHOD("tecs.project.query", ProjectQueryMethod);

    // 查询工程下所有的虚拟机
    ADD_METHOD("tecs.project.query_vm_by_project", ProjectShowVmMethod);

    // 工程静态数据查询
    ADD_METHOD("tecs.project.statistics.static", ProjectStatisticsStaticMethod);

    // 工程动态数据查询
    //    ADD_METHOD("tecs.project.statistics.dynamic", ProjectStatisticsDynamicMethod);
	
    //工程保存为镜像
    ADD_METHOD("tecs.project.save_as_image", ProjectSaveAsImageMethod);

    //从镜像创建 工程
    ADD_METHOD("tecs.project.create_by_image", ProjectCreateByImageMethod);

     /************************ 亲和域管理相关   *************************/
    // 创建
    ADD_METHOD("tecs.vmcfg.affinity_region.allocate", AffinityRegionAllocateMethod);

    // 删除
    ADD_METHOD("tecs.vmcfg.affinity_region.delete", AffinityRegionDeleteMethod);

    // 修改
    ADD_METHOD("tecs.vmcfg.affinity_region.set", AffinityRegionModifyMethod);

    // 查询
    ADD_METHOD("tecs.vmcfg.affinity_region.query", AffinityRegionQueryMethod);

    /************************    虚拟机配置管理相关   *************************/

    // 虚拟机配置创建(直接方式)
    ADD_METHOD("tecs.vmcfg.allocate", VmCfgAllocateMethod);

     // 虚拟机配置创建(直接方式)
    ADD_METHOD("tecs.vmcfg.allocate_by_vt", VmCfgAllocateByVtMethod);

     // 虚拟机配置克隆(直接方式)
    ADD_METHOD("tecs.vmcfg.clone", VmCfgCloneMethod);

    // 设置虚拟磁盘
    ADD_METHOD("tecs.vmcfg.set_disk", VmCfgSetDiskMethod);

    // 设置映像磁盘
    ADD_METHOD("tecs.vmcfg.set_image_disk", VmCfgSetImageDiskMethod);

    // 设置虚拟网卡
    ADD_METHOD("tecs.vmcfg.set_nic", VmCfgSetNicMethod);

    // 整体修改虚拟机配置
    ADD_METHOD("tecs.vmcfg.set", VmCfgSetMethod);

    //修改虚拟机部分配置
    ADD_METHOD("tecs.vmcfg.modify", VmCfgModifyMethod);

    // 虚拟机配置查询
    ADD_METHOD("tecs.vmcfg.staticinfo.query", VmStaticInfoQueryMethod);

    ADD_METHOD("tecs.vmcfg.runninginfo.query", VmRunningInfoQueryMethod);

   // ADD_METHOD("tecs.vmcfg.full.query", VmCfgFullQueryMethod);

    ADD_METHOD("tecs.vmcfg.get_id_by_name", VmCfgGetIdByNameMethod);

    // 虚拟机操作
    ADD_METHOD("tecs.vmcfg.action", VmCfgActionMethod);

    //设置虚拟机互斥关系
    ADD_METHOD("tecs.vmcfg.relation.set", VmCfgRelationMethod);

    //查询虚拟机互斥关系
    ADD_METHOD("tecs.vmcfg.query_relation", VmCfgRelationQueryMethod);

    // 虚拟机迁移
    ADD_METHOD("tecs.vmcfg.migrate", VmCfgMigrateMethod);

    // 虚拟机USB信息查询
    ADD_METHOD("tecs.vmcfg.query_usb_device", VmCfgUsbDeviceQueryMethod);

    // 虚拟机USB动作命令
    ADD_METHOD("tecs.vmcfg.operate_usb_device", VmCfgUsbDeviceOpMethod);

    // 虚拟机附载移动盘命令
    ADD_METHOD("tecs.vmcfg.portable_disk.attach", VmCfgAttachPortableDiskMethod);

    // 虚拟机分离移动盘命令
    ADD_METHOD("tecs.vmcfg.portable_disk.detach", VmCfgDetachPortableDiskMethod);

    // 虚拟机镜像备份修改
    ADD_METHOD("tecs.vmcfg.image_backup.modify", VmCfgImageBackupModifyMethod);

    // 虚拟机镜像恢复命令
    ADD_METHOD("tecs.vmcfg.image_backup.restore", VmCfgRestoreImageBackupMethod);

    // 虚拟机镜像创建命令
    ADD_METHOD("tecs.vmcfg.image_backup.create", VmCfgCreateImageBackupMethod);

    // 虚拟机镜像备份删除命令
    ADD_METHOD("tecs.vmcfg.image_backup.delete", VmCfgDeleteImageBackupMethod);

    //虚拟机加入亲和域
    ADD_METHOD("tecs.vmcfg.affinity_region.member.add", VmCfgAffinityRegionAddMethod);

    //虚拟机退出亲和域
    ADD_METHOD("tecs.vmcfg.affinity_region.member.delete", VmCfgAffinityRegionDelMethod);

    /************************    虚拟机模板置管理相关   *************************/

    // 虚拟机模板创建(一次传入所有参数)
    ADD_METHOD("tecs.vmtemplate.allocate", VmTemplateAllocateMethod);

    // 设置虚拟磁盘
    ADD_METHOD("tecs.vmtemplate.set_disk", VmTemplateSetDiskMethod);

    // 设置映像磁盘
    ADD_METHOD("tecs.vmtemplate.set_image_disk", VmTemplateSetImageDiskMethod);

    // 设置虚拟网卡
    ADD_METHOD("tecs.vmtemplate.set_nic", VmTemplateSetNicMethod);

    // 虚拟机模板删除
    ADD_METHOD("tecs.vmtemplate.delete", VmTemplateDeleteMethod);

    // 虚拟机模板修改
    ADD_METHOD("tecs.vmtemplate.set", VmTemplateModifyMethod);

    // 虚拟机模板查询
    ADD_METHOD("tecs.vmtemplate.query", VmTemplateQueryMethod);

    // 虚拟机模板发布
    ADD_METHOD("tecs.vmtemplate.publish", VmTemplatePublishMethod);

    /************************    虚拟机虚拟网络相关配置   *************************/
/**************************************VNET2.0新加***********************************/ 
    ADD_METHOD("tecs.vnet.netplane.cfg", VNetConfigNetplaneDataXMLRPC);
    ADD_METHOD("tecs.vnet.netplaneip.cfg", VNetConfigNetplaneIPResoueceXMLRPC);
    ADD_METHOD("tecs.vnet.netplanemac.cfg", VNetConfigNetplaneMACResoueceXMLRPC);
    ADD_METHOD("tecs.vnet.netplanevlan.cfg", VNetConfigNetplaneVLANResoueceXMLRPC);
    ADD_METHOD("tecs.vnet.netplanesegment.cfg", VNetConfigNetplaneSEGMENTResoueceXMLRPC);     
    ADD_METHOD("tecs.vnet.logicnet.cfg", VNetConfigLogicNetDataXMLRPC);
    ADD_METHOD("tecs.vnet.logicnetip.cfg", VNetConfigLogicNetIPXMLRPC);
    ADD_METHOD("tecs.vnet.portgroup.cfg", VNetConfigPortGroupDataXMLRPC);
    ADD_METHOD("tecs.vnet.portgroupvlan.cfg", VNetConfigPortGroupVlanXMLRPC);
    ADD_METHOD("tecs.vnet.switch.cfg", VNetConfigSwitchXMLRPC);
    ADD_METHOD("tecs.vnet.switchport.cfg", VNetConfigSwitchPortXMLRPC);
    ADD_METHOD("tecs.vnet.kernalport.cfg", VNetConfigKernalPortXMLRPC);
    ADD_METHOD("tecs.vnet.loopport.cfg",   VNetConfigLoopPortXMLRPC);
    ADD_METHOD("tecs.vnet.wcloopport.cfg",   VNetConfigWildCastLoopPortXMLRPC);
    ADD_METHOD("tecs.vnet.wcswitch.base.cfg",   VNetConfigWildCastSwitchXMLRPC);
    ADD_METHOD("tecs.vnet.wcswitch.port.cfg",   VNetConfigWildcastSwitchPortXMLRPC);
    ADD_METHOD("tecs.vnet.wcvport.cfg",         VNetConfigWildCastVirtualPortXMLRPC);
    ADD_METHOD("tecs.vnet.wcdeltask.cfg",       VNetConfigWildcastTaskSwitchXMLRPC);
    ADD_METHOD("tecs.vnet.sdn.cfg", VNetConfigSdnDataXMLRPC);
    ADD_METHOD("tecs.vnet.clearvna.cfg",       VNetVNAClearXMLRPC);
    ADD_METHOD("tecs.vnet.lgnetname2id.cfg",       VNetLogicNetID2NameXMLRPC);
    ADD_METHOD("tecs.vnet.portgroup.query",       VNetPortGroupQueryXMLRPC);

    /*******************************用户管理*********************************/
    ADD_METHOD("tecs.user.allocate", UserAllocateMethod);

    ADD_METHOD("tecs.user.delete", UserDeleteMethod);

    ADD_METHOD("tecs.user.set", UserUpdateMethod);

    ADD_METHOD("tecs.user.query", UserInfoMethod);

    ADD_METHOD("tecs.user.full.query", UserFullQueryMethod);

    ADD_METHOD("tecs.usergroup.allocate", UserGroupAllocateMethod);

    ADD_METHOD("tecs.usergroup.delete", UserGroupDeleteMethod);

    ADD_METHOD("tecs.usergroup.set", UserGroupUpdateMethod);

    ADD_METHOD("tecs.usergroup.query", UserGroupInfoMethod);

    ADD_METHOD("tecs.session.login", SessionLoginMethod);

    ADD_METHOD("tecs.session.logout", SessionLogoutMethod);

    ADD_METHOD("tecs.session.query", SessionQueryMethod);

    /************************ 版本管理   相关配置   *************************/
    ADD_METHOD("tecs.version.get_repo_addr", VersionGetRepoAddr);

    ADD_METHOD("tecs.version.set_repo_addr", VersionSetRepoAddr);

    ADD_METHOD("tecs.version.update", VersionUpdate);

    ADD_METHOD("tecs.version.query", VersionQuery);

    ADD_METHOD("tecs.version.cancel", VersionCancel);


    ADD_METHOD("tecs.currentalarm.query", CurrentAlarmPoolInfoMethod);

    ADD_METHOD("tecs.historyalarm.query", HistoryAlarmPoolInfoMethod);

    ADD_METHOD("tecs.snmpconfiguration.query", SnmpConfigurationPoolInfoMethod);

    ADD_METHOD("tecs.trapaddress.query", TrapAddressPoolInfoMethod);

    ADD_METHOD("tecs.snmpuser.query", SnmpXmlRpcUserPoolGetMethod);

    ADD_METHOD("tecs.snmpconfiguration.set", SnmpConfigurationPoolInfoSetMethod);

    ADD_METHOD("tecs.trapaddress.set", TrapAddressPoolInfoSetMethod);

    ADD_METHOD("tecs.snmpuser.set", SnmpXmlRpcUserPoolSetMethod);

    ADD_METHOD("tecs.trapaddress.delete", TrapAddressPoolInfoDeleteMethod);

    /************************ 磁阵管理   相关配置   *************************/

    ADD_METHOD("tecs.sa.disable", SaDisableMethod);
    ADD_METHOD("tecs.sa.enable", SaEnableMethod);
    ADD_METHOD("tecs.sa.forgetbyid", SaForgetIdMethod);
    ADD_METHOD("tecs.sa.clustermap", SaMapClusterMethod);

    ADD_METHOD("tecs.portabledisk.create", PortableDiskCreateMethod);
    ADD_METHOD("tecs.portabledisk.delete", PortableDiskDeleteMethod);


    ADD_METHOD("tecs.portabledisk.attachhost", PortableDiskAttachHostMethod);
    ADD_METHOD("tecs.portabledisk.detachhost", PortableDiskDetachHostMethod);
    ADD_METHOD("tecs.portabledisk.blockinfo.get", PortableDiskBlockInfoGetMethod);
	ADD_METHOD("tecs.workflow.query", WorkflowQuery);


}

}
/******************************************************************************/

