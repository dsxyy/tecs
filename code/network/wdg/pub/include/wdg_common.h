/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：wdg_common.h
* 文件标识：见配置管理计划书
* 内容摘要：WDG公共功能函数申明文件
* 当前版本：1.0
* 作    者：liuhx
* 完成日期：2012年12月15日
*******************************************************************************/
#if !defined(WDG_COMMON__INCLULDE_H_)
#define WDG_COMMON__INCLULDE_H_

//common
#include "tecs_pub.h"
#include "vnet_comm.h"
#include "vna_common.h"
#include "vnet_error.h"

//msg
#include "vnet_event.h"
#include "vnet_mid.h"
#include "vnet_msg.h"
#include "vm_messages.h"
#include "vnetlib_common.h"
#include "vnetlib_msg.h"
#include "vnetlib_event.h"
#include "vnet_evb_mgr.h"

#define WDG_UUID_FILE   "/etc/wdg_uuid_file"
#define WDG_UUID_DIR   "/etc/"

int GetWDGUUID(string &strWDGUUID);
int GetWDGUUIDByLib(string &strWDGUUID);
int GetUUID(string & strUUID);

#endif

