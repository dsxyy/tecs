/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�vnm_pub.h
* �ļ���ʶ��
* ����ժҪ����ģ�����ͷ�ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�
* ������ڣ�2012��1��18��
*
* �޸ļ�¼1��
*     �޸����ڣ�2013/1/18
*     �� �� �ţ�V1.0
*     �� �� �ˣ�Wang.Lule
*     �޸����ݣ�����
*
******************************************************************************/

#if !defined(VNM_PUB_INCLUDE_H__)
#define VNM_PUB_INCLUDE_H__
//common
#include "tecs_pub.h"
#include "vnet_comm.h"

// db
#include "vnet_db_proc.h"
#include "vnet_db_mgr.h"
#include "vnet_db_logicnetwork_iprange.h"

// msg
#include "vnet_event.h"
#include "vnet_mid.h"
#include "vnet_msg.h"
#include "vnetlib_common.h"
#include "vnetlib_event.h"
#include "vnetlib_msg.h"
#include "vm_messages.h"
//vnic
#include "vnet_vnic.h"

// resource pool
#include "mac_addr.h"
#include "mac_range.h"
#include "mac_pool.h"
#include "mac_pool_mgr.h"

#include "vlan_range.h"
#include "vlan_pool.h"
#include "vlan_pool_mgr.h"

#include "ipv4_addr.h"
#include "ipv4_addr_range.h"
#include "ipv4_addr_pool.h"
#include "ipv4_pool_mgr.h"

//portgroup
#include "port_group.h"
#include "port_group_mgr.h"

// logic network
#include "logic_network.h"
#include "logic_network_mgr.h"


//port
#include "vnet_portmgr.h"



//vnm_switch_manager
#include "vnm_switch_manager.h"

//vsi
#include "vnm_vsi_mgr.h"



//vmnic
#include "vnet_db_vm.h"
#include "vnet_db_vsi.h"
#include "vnm_vm_db.h"
#include "vnm_vnic_db.h"
#include "vnm_vm_mgr.h"
#include "vnm_vm_mgr_ext.h"

//
#include "vnet_db_schedule.h"
#include "vnm_schedule.h"


// vna manger
#include "vna_reg_mod.h"
#include "vna_info.h"
#include "vna_mgr.h"

// vnm controller
#include "vnm_controller.h"

// netplane
#include "netplane.h"
#include "netplane_mgr.h"





//wildcast 
#include "wildcast_mgr.h" 

//vm migrate 
#include "vm_migrate_mgr.h" 

//vxlan mcgroup
#include "vnm_vxlan_mgr.h" 

#endif

