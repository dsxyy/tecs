/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：msg_proc_reg.cpp
* 文件标识：
* 内容摘要：vnm 模块消息登记实现文件
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

#include "vnet_comm.h"
#include "vnet_event.h"
#include "vnetlib_event.h"
#include "msg_proc_reg.h"

namespace zte_tecs
{
    TMsgProcReg s_VNetMsgProcReg[] = 
    {
        /* VNA_MGR消息 */
        {VNA_MGR_MODULE, EV_VNA_REGISTER_TO_VNM},
        {VNA_MGR_MODULE, EV_VNA_REGISTER_REQ},
        {VNA_MGR_MODULE, EV_VNA_INFO_REPORT},
        {VNA_MGR_MODULE, EV_VNA_MONI_TIMER},
        {VNA_MGR_MODULE, EV_VNA_HEARTBEAT_REPORT},
        
        /* 清除vna */
        {VNA_MGR_MODULE, EV_VNETLIB_DEL_VNA},
        
        /* VLAN资源池 */
        {VNET_VLAN_RES_MODULE, EV_VNETLIB_VLAN_RANGE_CFG},

        /* SEGMENT资源池 */
        {VNET_SEGMENT_RES_MODULE, EV_VNETLIB_SEGMENT_RANGE_CFG},
        
        /* MAC资源池 */        
        {VNET_MAC_RES_MODULE, EV_VNETLIB_MAC_RANGE_CFG},
    
        /* IP资源池 */
        {VNET_IP_RES_MODULE, EV_VNETLIB_LOGI_NET_IPV4_RANGE_CFG},        
        {VNET_IP_RES_MODULE, EV_VNETLIB_NP_IPV4_RANGE_CFG},      
        
        /* VNM Ctrl */
        {VNM_CTRL_MODULE, EV_MC_VNM_INFO_TIMER}, 
        {VNM_CTRL_MODULE, EV_MG_VNM_INFO}, 
        {VNM_CTRL_MODULE, EV_VNETLIB_SDN_CFG}, 
        
        /* Netplane */
        {VNM_NETPLANE_MODULE, EV_VNETLIB_NETPLANE_CFG},

        /* Logic network */
        {VNM_LOGIC_NETWORK_MODULE, EV_VNETLIB_LOGIC_NETWORK_CFG},        
        {VNM_LOGIC_NETWORK_MODULE, EV_VNETLIB_LOGIC_NETWORK_NAME2UUID},           

        /* CVNetVmMgr模块*/
        {VNM_VM_MGR_MODULE, EV_VNETLIB_SELECT_CC}, 
        {VNM_VM_MGR_MODULE, EV_VNETLIB_GETRESOURCE_CC}, 
        {VNM_VM_MGR_MODULE, EV_VNETLIB_FREERESOURCE_CC}, 
        {VNM_VM_MGR_MODULE, EV_VNETLIB_SELECT_HC}, 
        {VNM_VM_MGR_MODULE, EV_VNETLIB_GETRESOURCE_HC}, 
        {VNM_VM_MGR_MODULE, EV_VNETLIB_FREERESOURCE_HC}, 

        {VNM_VM_MGR_MODULE, EV_VNM_SCHEDULE_CC_ACK}, 
        {VNM_VM_MGR_MODULE, EV_VNM_GET_RESOURCE_FOR_CC_ACK}, 
        {VNM_VM_MGR_MODULE, EV_VNM_FREE_RESOURCE_FOR_CC_ACK}, 
        {VNM_VM_MGR_MODULE, EV_VNM_SCHEDULE_HC_ACK}, 
        {VNM_VM_MGR_MODULE, EV_VNM_GET_RESOURCE_FOR_HC_ACK}, 
        {VNM_VM_MGR_MODULE, EV_VNM_FREE_RESOURCE_FOR_HC_ACK}, 

        /* CVNetVmMgrExt模块*/        
        {VNM_VM_MGR_EXT_MODULE, EV_VNA_REQUEST_VMVSI}, 
        {VNM_VM_MGR_EXT_MODULE, EV_VNETLIB_QUERY_VNICS}, 
        {VNM_VM_MGR_EXT_MODULE, EV_VNETLIB_SELECT_MIGRATE_HC},

        // vm migrate 两个消息暂时放在vnic模块中 后续规划整改
        {VNM_VM_MGR_EXT_MODULE, EV_VNETLIB_VM_MIGRATE_BEGIN},
        {VNM_VM_MGR_EXT_MODULE, EV_VNETLIB_VM_MIGRATE_END},
        
        
        /*SWITCHAGENT模块消息*/
        {VNM_SWITCH_MODULE, EV_VNETLIB_SWITCH_CFG},
        {VNM_SWITCH_MODULE, EV_VNETLIB_SWITCH_CFG_ACK},
        {VNM_SWITCH_MODULE, EV_VNETLIB_SWITCH_PORT_CFG},
        {VNM_SWITCH_MODULE, EV_VNETLIB_SWITCH_PORT_CFG_ACK},
        {VNM_SWITCH_MODULE, EV_SWITCH_UPDATE_VNA_TIMER},
        {VNM_SWITCH_MODULE, EV_SWITCH_MODIFY_TIMER},

        /*PORT GROUP模块消息*/
        {VNM_PORTGROUP_MODULE, EV_VNETLIB_PORT_GROUP_CFG},
        {VNM_PORTGROUP_MODULE, EV_VNETLIB_PORT_GROUP_CFG_ACK},
        {VNM_PORTGROUP_MODULE, EV_VNETLIB_PG_TRUNK_VLANRANGE_CFG},
        {VNM_PORTGROUP_MODULE, EV_VNETLIB_PG_TRUNK_VLANRANGE_CFG_ACK},
        {VNM_PORTGROUP_MODULE, EV_VNETLIB_PG_QUERY},
        {VNM_PORTGROUP_MODULE, EV_VNETLIB_PG_QUERY_ACK},

        /*PORT管理模块消息*/
        {VNM_PORT_MODULE, EV_VNETLIB_KERNALPORT_CFG},
        {VNM_PORT_MODULE, EV_VNETLIB_KERNALPORT_CFG_ACK},
        {VNM_PORT_MODULE, EV_VNETLIB_CFG_LOOPBACK},
        {VNM_PORT_MODULE, EV_VNETLIB_CFG_LOOPBACK_ACK},
        {VNM_PORT_MODULE, TIMER_VNA_RESET_PORTINFO},
        {VNM_PORT_MODULE, TIMER_VNA_RESET_PORTINFO_ACK},

        /*通配模块消息*/
        {VNM_WILDCAST_MODULE, EV_VNM_WILDCAST_TIMER},
        {VNM_WILDCAST_MODULE, EV_VNETLIB_WILDCAST_SWITCH_CFG},
        {VNM_WILDCAST_MODULE, EV_VNETLIB_WILDCAST_SWITCH_PORT_CFG},
        {VNM_WILDCAST_MODULE, EV_VNETLIB_WILDCAST_CREATE_VPORT_CFG},
        {VNM_WILDCAST_MODULE, EV_VNETLIB_WILDCAST_LOOPBACK_CFG},
        {VNM_WILDCAST_MODULE, EV_VNETLIB_WILDCAST_SWITCH_TASK_DEL},
        {VNM_WILDCAST_MODULE, EV_VNETLIB_WILDCAST_CREATE_VPORT_TASK_DEL},
        {VNM_WILDCAST_MODULE, EV_VNETLIB_WILDCAST_LOOPBACK_TASK_DEL}, 

        /*通配模块消息*/
        {VNM_VXLAN_MODULE, EV_VXLAN_MCGROUP_REQ},


        
	
        /* 外部接口消息 */
    };
    
    TMsgProcReg *GetMsgProcRegInfo(int32 &nSize)
    {
        nSize = sizeof(s_VNetMsgProcReg)/sizeof(TMsgProcReg);
        return s_VNetMsgProcReg;
    }

    void VNetDbgShowMsgProcReg(void)
    {
        int32 nSize = 0;
        TMsgProcReg *p = GetMsgProcRegInfo(nSize);
        
        if(0 == nSize)
        {
            return ;
        }

        cout << "----------------------------------------------------------------------------" << endl;        
        
        for(int32 i = 0; i < nSize; ++i)
        { 
            cout << "Module Name: " << p[i].mod_name << ", MsgID: " << p[i].msg_id <<endl;
        }

        cout << "----------------------------------------------------------------------------" << endl;
        
        return ;
    }
    DEFINE_DEBUG_FUNC(VNetDbgShowMsgProcReg);
}// end namespace zte_tecs

