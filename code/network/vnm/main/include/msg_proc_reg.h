/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：msg_proc_reg.h
* 文件标识：
* 内容摘要：vnm 模块消息登记定义文件
* 当前版本：1.0
* 作    者：
* 完成日期：2012年12月12日
*
* 修改记录1：
*     修改日期：2012/12/12
*     版 本 号：V1.0
*     修 改 人：Wang.Lule
*     修改内容：创建
******************************************************************************/

#if !defined(MSG_PROC_REG_INCLUDE_H__)
#define MSG_PROC_REG_INCLUDE_H__

namespace zte_tecs
{
#define VNET_MAC_RES_MODULE         "MAC_RESOURCE"
#define VNET_VLAN_RES_MODULE        "VLAN_RESOURCE"
#define VNET_SEGMENT_RES_MODULE     "SEGMENT_RESOURCE"
#define VNET_IP_RES_MODULE          "IP_RESOURCE"
#define VNA_MGR_MODULE              "VNA_MANAGER"
#define VNM_CTRL_MODULE             "VNM_CTRL"
#define VNM_NETPLANE_MODULE         "VNM_NETPLANE"
#define VNM_LOGIC_NETWORK_MODULE    "VNM_LOGIC_NETWORK"
#define VNM_VM_MGR_MODULE           "VNM_MGR"
#define VNM_VM_MGR_EXT_MODULE       "VNM_MGR_EXT"
#define VNM_SWITCH_MODULE           "VNM_SWITCH"
#define VNM_PORTGROUP_MODULE        "VNM_PORTGROUP"
#define VNM_PORT_MODULE             "VNM_PORTMGR"
#define VNM_WILDCAST_MODULE         "VNM_WILDCAST"
#define VNM_VXLAN_MODULE            "VNM_VXLAN"

    typedef struct tagMsgProcReg
    {
        string mod_name;
        uint32  msg_id;
    }TMsgProcReg;
    
    /* 获悉消息登记表信息和大小 */
    TMsgProcReg *GetMsgProcRegInfo(int32 &nSize);

}// namespace zte_tecs

#endif

