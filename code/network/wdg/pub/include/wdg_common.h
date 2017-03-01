/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�wdg_common.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��WDG�������ܺ��������ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�liuhx
* ������ڣ�2012��12��15��
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

