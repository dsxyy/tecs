/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：vnet_mid.h
* 文件标识：
* 内容摘要：VNET 进程和消息单元定义
* 当前版本：1.0
* 作    者：
* 完成日期：2013年1月18日
*
* 修改记录1：
*     修改日期：2013/1/18
*     版 本 号：V1.0
*     修 改 人：Wang.Lule
*     修改内容：创建
*
******************************************************************************/
#if !defined(COMMON_VNET_MID_INCLUDE_H__)
#define COMMON_VNET_MID_INCLUDE_H__
namespace zte_tecs
{

/* VNM 进程及消息单元 */
#define PROC_VNM                            "proc_vnm"
#define MU_VNM                              "vnm"
#define MU_SWITCH_MANAGER                   "switch_manager"
#define MU_VNM_SCHEDULE                     "vnm_schedule"
#define MU_VNM_QUERY                        "vnm_query"       // vnm 提供给外部xmlrpc接口消息
//#define MU_VNM_MGR                          "vnm_mgr"         // vnm 上接收vna上报消息

#define MU_VNM_VNA_REPORT                   "vnm_vna_report"  // vnm 上接收vna上报消息

/* VNA 进程及消息单元 */
#define PROC_VNA                            "proc_vna"
#define MU_VNA_CONTROLLER                   "vna_controller"
#define MU_VNA_AGENT                        "vna_agent"

#define MU_VNA_HEARTBEAT                    "vna_heartbeat"
#define MU_VNET_WDG                         "wdg"
#define PROC_WDG                            "proc_wdg"

#define MU_VNET_PORAGENT                    "portagent"

/* 组播组 */
#define MUTIGROUP_VNA_REG                   "mg_vna_reg"        // 所有VNM都加入，接收主机发现消息
#define MUTIGROUP_VNA_REG_SYSTEM            "_vna_reg_system"   
#define MUTIGROUP_VNA_OF_VNM                "mg_vna_of_vnm"     // VNM下所有归属的VNA都加入
#define MUTIGROUP_VNM_MS                    "mg_vnm_ms"         // VNM主备竞争且通告APP群

}
using namespace zte_tecs;
#endif // ifnedf COMMON_MID_H

