/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�register_xmlrpc_methods.cpp
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��XMLRPC����ע���ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�������
* ������ڣ�2011��7��22��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/7/22
*     �� �� �ţ�V1.0
*     �� �� �ˣ�������
*     �޸����ݣ�����
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
    //ע��XMLRPC������ʾ������:
    //ADD_METHOD("tecs.test.query", TecsRpcTestMethod);
    //ADD_METHOD("tecs.test.test", TecsRpcTestMethod);
    /* ע��tecsϵͳ������ص�API����*/
    ADD_METHOD("tecs.system.runtime.query", SysRuntimeInfoQueryMethod);

    /* ע��tecs���֤��ѯ�ӿ� */
    ADD_METHOD("tecs.system.license.query", LicenseQueryMethod);

    /* ע��tecsӲ��ָ�Ʋ�ѯ�ӿ� */
    ADD_METHOD("tecs.system.license.hardware_fingerprint.query", HardwareFingerprintQueryMethod);

    /* ע�ἯȺ�����API���� */
    // ��Ⱥע��ȡ�����޸�Ϊ���ü�Ⱥ
    ADD_METHOD("tecs.cluster.set", ClusterSet);

    // ��Ⱥ��Ϣ����
    ADD_METHOD("tecs.cluster.query", ClusterQuery);

    // ��Ⱥע��ȡ�����޸�Ϊ������Ⱥ
    ADD_METHOD("tecs.cluster.forget", ClusterForget);

    // ��Ⱥֹͣ
    ADD_METHOD("tecs.cluster.disable", ClusterPreserveStart);

    // ��Ⱥ����
    ADD_METHOD("tecs.cluster.enable", ClusterPreserveStop);

    //ADD_METHOD("tecs.cluster.query_netplane", ClusterQueryNetPlane);

    ADD_METHOD("tecs.cluster.query_core_tcu", ClusterQueryCoreAndTcu);

    // ���������ģ��ע���XMLRPC����
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

    /************************    ӳ��������   *************************/
    // ӳ��ע��
    // ADD_METHOD("tecs.image.register", ImageRegisterMethod);

    // ӳ��ע��
    // ADD_METHOD("tecs.image.unregister", ImageDeregisterMethod);

    // ӳ��ɾ��
    ADD_METHOD("tecs.image.delete", ImageDeleteMethod);

    // ӳ���޸�
    ADD_METHOD("tecs.image.set", ImageModifyMethod);

    // ӳ���ѯ
    ADD_METHOD("tecs.image.query", ImageQueryMethod);

    // ӳ��ʹ��
    ADD_METHOD("tecs.image.enable", ImageEnableMethod);

    // ӳ��publish
    ADD_METHOD("tecs.image.publish", ImagePublishMethod);

    // ��ȡӳ��ftp����
    ADD_METHOD("tecs.image.ftp_svr_info_get", ImageFtpSvrInfoGetMethod);


    // ӳ����
    ADD_METHOD("tecs.image.deploy", ImageDeployMethod);

    // ӳ����
    ADD_METHOD("tecs.image.cancel", ImageCancelMethod);

    // ӳ��������ռ��ѯ
    ADD_METHOD("tecs.image.getspace", ImageGetSpaceMethod);

     /************************    �ļ��������   *************************/
    // �ļ���ѯ
    //�ļ������coredump�ļ�����URL��ַ��ѯ�ӿ�
    ADD_METHOD("tecs.file.query_coredump_url", FileGetCoredumpUrlMethod);

    /************************    ���̹������   *************************/
    // ���̴���
    ADD_METHOD("tecs.project.allocate", ProjectAllocateMethod);

    // ����ɾ��
    ADD_METHOD("tecs.project.delete", ProjectDeleteMethod);

    // �����޸�
    ADD_METHOD("tecs.project.set", ProjectModifyMethod);
    // ���̲���
    ADD_METHOD("tecs.project.action", ProjectActionMethod);
    // ���̿�¡
    ADD_METHOD("tecs.project.clone", ProjectCloneMethod);
    // ���̲�ѯ
    ADD_METHOD("tecs.project.query", ProjectQueryMethod);

    // ��ѯ���������е������
    ADD_METHOD("tecs.project.query_vm_by_project", ProjectShowVmMethod);

    // ���̾�̬���ݲ�ѯ
    ADD_METHOD("tecs.project.statistics.static", ProjectStatisticsStaticMethod);

    // ���̶�̬���ݲ�ѯ
    //    ADD_METHOD("tecs.project.statistics.dynamic", ProjectStatisticsDynamicMethod);
	
    //���̱���Ϊ����
    ADD_METHOD("tecs.project.save_as_image", ProjectSaveAsImageMethod);

    //�Ӿ��񴴽� ����
    ADD_METHOD("tecs.project.create_by_image", ProjectCreateByImageMethod);

     /************************ �׺���������   *************************/
    // ����
    ADD_METHOD("tecs.vmcfg.affinity_region.allocate", AffinityRegionAllocateMethod);

    // ɾ��
    ADD_METHOD("tecs.vmcfg.affinity_region.delete", AffinityRegionDeleteMethod);

    // �޸�
    ADD_METHOD("tecs.vmcfg.affinity_region.set", AffinityRegionModifyMethod);

    // ��ѯ
    ADD_METHOD("tecs.vmcfg.affinity_region.query", AffinityRegionQueryMethod);

    /************************    ��������ù������   *************************/

    // ��������ô���(ֱ�ӷ�ʽ)
    ADD_METHOD("tecs.vmcfg.allocate", VmCfgAllocateMethod);

     // ��������ô���(ֱ�ӷ�ʽ)
    ADD_METHOD("tecs.vmcfg.allocate_by_vt", VmCfgAllocateByVtMethod);

     // ��������ÿ�¡(ֱ�ӷ�ʽ)
    ADD_METHOD("tecs.vmcfg.clone", VmCfgCloneMethod);

    // �����������
    ADD_METHOD("tecs.vmcfg.set_disk", VmCfgSetDiskMethod);

    // ����ӳ�����
    ADD_METHOD("tecs.vmcfg.set_image_disk", VmCfgSetImageDiskMethod);

    // ������������
    ADD_METHOD("tecs.vmcfg.set_nic", VmCfgSetNicMethod);

    // �����޸����������
    ADD_METHOD("tecs.vmcfg.set", VmCfgSetMethod);

    //�޸��������������
    ADD_METHOD("tecs.vmcfg.modify", VmCfgModifyMethod);

    // ��������ò�ѯ
    ADD_METHOD("tecs.vmcfg.staticinfo.query", VmStaticInfoQueryMethod);

    ADD_METHOD("tecs.vmcfg.runninginfo.query", VmRunningInfoQueryMethod);

   // ADD_METHOD("tecs.vmcfg.full.query", VmCfgFullQueryMethod);

    ADD_METHOD("tecs.vmcfg.get_id_by_name", VmCfgGetIdByNameMethod);

    // ���������
    ADD_METHOD("tecs.vmcfg.action", VmCfgActionMethod);

    //��������������ϵ
    ADD_METHOD("tecs.vmcfg.relation.set", VmCfgRelationMethod);

    //��ѯ����������ϵ
    ADD_METHOD("tecs.vmcfg.query_relation", VmCfgRelationQueryMethod);

    // �����Ǩ��
    ADD_METHOD("tecs.vmcfg.migrate", VmCfgMigrateMethod);

    // �����USB��Ϣ��ѯ
    ADD_METHOD("tecs.vmcfg.query_usb_device", VmCfgUsbDeviceQueryMethod);

    // �����USB��������
    ADD_METHOD("tecs.vmcfg.operate_usb_device", VmCfgUsbDeviceOpMethod);

    // ����������ƶ�������
    ADD_METHOD("tecs.vmcfg.portable_disk.attach", VmCfgAttachPortableDiskMethod);

    // ����������ƶ�������
    ADD_METHOD("tecs.vmcfg.portable_disk.detach", VmCfgDetachPortableDiskMethod);

    // ��������񱸷��޸�
    ADD_METHOD("tecs.vmcfg.image_backup.modify", VmCfgImageBackupModifyMethod);

    // ���������ָ�����
    ADD_METHOD("tecs.vmcfg.image_backup.restore", VmCfgRestoreImageBackupMethod);

    // ��������񴴽�����
    ADD_METHOD("tecs.vmcfg.image_backup.create", VmCfgCreateImageBackupMethod);

    // ��������񱸷�ɾ������
    ADD_METHOD("tecs.vmcfg.image_backup.delete", VmCfgDeleteImageBackupMethod);

    //����������׺���
    ADD_METHOD("tecs.vmcfg.affinity_region.member.add", VmCfgAffinityRegionAddMethod);

    //������˳��׺���
    ADD_METHOD("tecs.vmcfg.affinity_region.member.delete", VmCfgAffinityRegionDelMethod);

    /************************    �����ģ���ù������   *************************/

    // �����ģ�崴��(һ�δ������в���)
    ADD_METHOD("tecs.vmtemplate.allocate", VmTemplateAllocateMethod);

    // �����������
    ADD_METHOD("tecs.vmtemplate.set_disk", VmTemplateSetDiskMethod);

    // ����ӳ�����
    ADD_METHOD("tecs.vmtemplate.set_image_disk", VmTemplateSetImageDiskMethod);

    // ������������
    ADD_METHOD("tecs.vmtemplate.set_nic", VmTemplateSetNicMethod);

    // �����ģ��ɾ��
    ADD_METHOD("tecs.vmtemplate.delete", VmTemplateDeleteMethod);

    // �����ģ���޸�
    ADD_METHOD("tecs.vmtemplate.set", VmTemplateModifyMethod);

    // �����ģ���ѯ
    ADD_METHOD("tecs.vmtemplate.query", VmTemplateQueryMethod);

    // �����ģ�巢��
    ADD_METHOD("tecs.vmtemplate.publish", VmTemplatePublishMethod);

    /************************    ��������������������   *************************/
/**************************************VNET2.0�¼�***********************************/ 
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

    /*******************************�û�����*********************************/
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

    /************************ �汾����   �������   *************************/
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

    /************************ �������   �������   *************************/

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

