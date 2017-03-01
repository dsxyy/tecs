/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�msg_proc_reg.h
* �ļ���ʶ��
* ����ժҪ��vnm ģ����Ϣ�ǼǶ����ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�
* ������ڣ�2012��12��12��
*
* �޸ļ�¼1��
*     �޸����ڣ�2012/12/12
*     �� �� �ţ�V1.0
*     �� �� �ˣ�Wang.Lule
*     �޸����ݣ�����
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
    
    /* ��Ϥ��Ϣ�ǼǱ���Ϣ�ʹ�С */
    TMsgProcReg *GetMsgProcRegInfo(int32 &nSize);

}// namespace zte_tecs

#endif

