/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�vna_common.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��VNA�������ܺ��������ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�lvech
* ������ڣ�2012��12��15��
*
* �޸ļ�¼1��
*     �޸����ڣ�2012/12/15
*     �� �� �ţ�V1.0
*     �� �� �ˣ�lvech
*     �޸����ݣ�����
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
//������Ϣ������ض���
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
//��ʱ��������ض���
/******************************************************************************/
#define TIMER_INTERVAL_VNA_HEARTBEAT_REPORT  5*1000          //VNA��VNM������ʱ��
#define TIMER_INTERVAL_VNA_REGISTERED_VNM 50 * 1000           //VNA��VNMע��ɹ���֪ͨ VNA��ؽ���
//#define TIMER_INTERVAL_VNA_REGISTER 5 * 1000                        //VNA��VNMע����ϱ����̹��õĶ�ʱ��
//#define TIMER_INTERVAL_VNA_REGISTER (60 * 1000)                        //VNA��VNMע����ϱ����̹��õĶ�ʱ��
#define TIMER_INTERVAL_VNA_REGISTER (180 * 1000)                        //VNA��VNMע����ϱ����̹��õĶ�ʱ��
#define TIMER_INTERVAL_MODULE_STATE_CHECK 5 * 1000            //���MODULE״̬��ʱ��
#define TIMER_INTERVAL_VNA_CHECK_VM_VSI_INFO 3 * 1000       //VNA���VSI�Ƿ��Ѵ�VNM�ϻ�ȡVSI��ϸ��Ϣ

/******************************************************************************/

#define HYPE_VISOR_TYPE_XEN         0
#define HYPE_VISOR_TYPELIBVIRT      1
#define HYPE_VISOR_TYPE_KVM         2
#define HYPE_VISOR_TYPE_VMWARE      3
#define HYPE_VISOR_TYPE_SIMULATE    4
#define MAX_DRIVER_TYPE             5

#define VNA_UUID_FILE   "/etc/vna_uuid_file"
#define VNA_UUID_DIR   "/etc/"

int GetVNAUUIDByVNA(string &strVNAUUID);          //��ȡVNA��application����VNA���̵���
int GetVNAUUIDByLib(string &strVNAUUID);            //��ȡVNA��application����ҵ����̵���

int GetUUID(string & strUUID);
int GetHypeVisorType();      //��ȡ��ǰ���⻯��������




#endif

