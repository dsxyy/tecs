/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vna_pub.h
* 文件标识：见配置管理计划书
* 内容摘要：VNA进程需要包含的头文件
* 当前版本：1.0
* 作    者：lvech
* 完成日期：2013年1月9日
*
* 修改记录1：
*     修改日期：2013/1/9
*     版 本 号：V1.0
*     修 改 人：lvech
*     修改内容：创建
*******************************************************************************/
#ifndef VNA_PUB_H
#define VNA_PUB_H


//monitor
#include "dev_monitor.h"
#include "dev_mgr.h"
#include "dev_mon_reg_func.h"
#include "dev_mon_mgr.h"
#include "vnet_monitor.h"

//port
#include "vnet_phyport_class.h"
#include "vnet_phyport_manager.h"
#include "vnet_trunkport_class.h"
#include "vnet_trunkport_manager.h"
#include "vnet_portagent.h"

//switch
#include "vna_switch_module.h"
#include "vna_switch_agent.h"

#include "vna_agent.h"

//vnic
#include "vnet_vnic.h"
#include "vna_vnic_pool.h"
#include "vna_vm_agent.h"

#include "vna_heartbeat.h"
#include "vnetlib_common.h"
#include "vna_controler.h"

#endif // #ifndef VNA_PUB_H

