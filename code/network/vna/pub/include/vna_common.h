/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vna_common.h
* 文件标识：见配置管理计划书
* 内容摘要：VNA公共功能函数申明文件
* 当前版本：1.0
* 作    者：lvech
* 完成日期：2012年12月15日
*
* 修改记录1：
*     修改日期：2012/12/15
*     版 本 号：V1.0
*     修 改 人：lvech
*     修改内容：创建
*******************************************************************************/
#if !defined(VNA_COMMON__INCLULDE_H_)
#define VNA_COMMON__INCLULDE_H_

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

/******************************************************************************/
//主控消息处理相关定义
/******************************************************************************/
typedef struct tagMsgProcReg
{
    string mod_name;
    uint32  msg_id;
}TMsgProcReg;
    
class TPortInfo
{
public:
    TPortInfo()
    {
        m_nVifPortOfId = 0;
        m_nTapPortOfId = 0;
        m_nEmuPortOfId = 0;
    };

public:
    string m_strVifPort;
    int32 m_nVifPortOfId;
    string m_strTapPort;
    int32 m_nTapPortOfId;
    string m_strEmuPort;
    int32 m_nEmuPortOfId;    
    string m_strMacAddr;
};    

#define VNET_MODULE_BRMGR  "BRMGR"
#define VNET_MODULE_VNICMGR  "VNICMGR"
#define VNET_MODULE_VNETAGENT "VNETAGENT"
#define VNET_MODULE_MONITOR "VNETMONITOR"
#define VNET_MODULE_SWITCH_AGENT "SWITCHAGENT"
#define VNET_MODULE_PORT_AGENT "PORTAGENT"
#define VNET_MODULE_VXLAN_AGENT "VXLANAGENT"
/******************************************************************************/

/******************************************************************************/
//定时器处理相关定义
/******************************************************************************/
#define TIMER_INTERVAL_VNA_HEARTBEAT_REPORT  5*1000          //VNA向VNM心跳定时器
#define TIMER_INTERVAL_VNA_REGISTERED_VNM 50 * 1000           //VNA向VNM注册成功后，通知 VNA相关进程
//#define TIMER_INTERVAL_VNA_REGISTER 5 * 1000                        //VNA向VNM注册和上报流程共用的定时器
//#define TIMER_INTERVAL_VNA_REGISTER (60 * 1000)                        //VNA向VNM注册和上报流程共用的定时器
#define TIMER_INTERVAL_VNA_REGISTER (180 * 1000)                        //VNA向VNM注册和上报流程共用的定时器
#define TIMER_INTERVAL_MODULE_STATE_CHECK 5 * 1000            //检查MODULE状态定时器
#define TIMER_INTERVAL_VNA_CHECK_VM_VSI_INFO 3 * 1000       //VNA检查VSI是否已从VNM上获取VSI详细信息

/******************************************************************************/

#define HYPE_VISOR_TYPE_XEN         0
#define HYPE_VISOR_TYPELIBVIRT      1
#define HYPE_VISOR_TYPE_KVM         2
#define HYPE_VISOR_TYPE_VMWARE      3
#define HYPE_VISOR_TYPE_SIMULATE    4
#define MAX_DRIVER_TYPE             5

#define VNA_UUID_FILE   "/etc/vna_uuid_file"
#define VNA_UUID_DIR   "/etc/"

int GetVNAUUIDByVNA(string &strVNAUUID);          //获取VNA的application，供VNA进程调用
int GetVNAUUIDByLib(string &strVNAUUID);            //获取VNA的application，供业务进程调用

int GetUUID(string & strUUID);
int GetHypeVisorType();      //获取当前虚拟化机制类型




#endif

