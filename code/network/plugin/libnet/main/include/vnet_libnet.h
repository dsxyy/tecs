/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�vnet_libnet.h
* �ļ���ʶ��
* ����ժҪ��vnet_libnet.h��Ķ����ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�
* ������ڣ�2012��12��16��
*
* �޸ļ�¼1��
*     �޸����ڣ�2012/1/8
*     �� �� �ţ�V1.0
*     �� �� �ˣ�chenzhiwei
*     �޸����ݣ�����
******************************************************************************/
#ifndef __VNET_LIBNET_H
#define __VNET_LIBNET_H

#include "../../../../../sky/include/sky.h"
#include "../../../../pub/vnetlib_msg.h"
#include "../../../../pub/vnetlib_event.h"
#include "../../../../pub/vnetlib_common.h"

#include "../../../../../common/include/event.h"
#include "../../../../../common/include/registered_system.h"

#include "../../../../common/include/vnet_mid.h"
#include "../../../../common/include/vnet_event.h"
#include "../../../../common/include/vnet_msg.h"

#include "registered_system.h"

using namespace std;

using namespace zte_tecs;
namespace zte_tecs
{
#ifndef FALSE
#define FALSE               0
#endif

#ifndef TRUE
#define TRUE                1
#endif

#define VNETLIB_VNA_UUID_FILE   "/etc/vna_uuid_file"

//LIB����VNA����·״̬
#define VNA_LINK_UP    0           // ��·UP ����RUNNING
#define VNA_LINK_FAIL  1           // ��·�ո�down
#define VNA_LINK_DOWN  2           // ��·����һ��ʱ��
#define VNA_LINK_BACK  3           // ��·�ָ���

#define VNA_REGISTER_STATE_NOREG    0 //δע��
#define VNA_REGISTER_STATE_REG      1 //�Ѿ�ע�� 

#define VNA_NOT_NEED_SEND_VMNIC     0  //������Ҫ����
#define VNA_NEED_SEND_VMNIC         1  //û��ȷ�ϣ�����Ҫ�ٷ�

//�鲥������׺���鲥����= application +��׺
//#define MUTIGROUP_REG_SYSTEM       "_reg_system"


//��ʱ�����
#define  VNETLIB_LOOP_INTERVAL   (2000)
#define  VNETLIB_KEEPLIVE_INTERRVAL (5)

#define MU_VNET_LIBNET    "vnet_libnet"

//VM NET ������
#define VNET_VM_DEPLOY   (0)  /* ��ͨ���� */
#define VNET_VM_PREPARE  (1)  /* Ԥ���� */
#define VNET_VM_MIGRATE  (2)  /* Ǩ�� */
#define VNET_VM_SUSPEND  (3)  /* �ݲ��ã����� */
#define VNET_VM_CANCEL   (4)  /* ���� */

//VM NET �������
#define VNET_VM_SUCCESS  (1)
#define VNET_VM_FAILED   (0)

//VNET_LIB ״̬
#define VNETLIB_STATE_MACHINE_START  (1)
#define VNETLIB_STATE_MACHINE_WORK   (2)

typedef struct tagModuleInfo
{
    string module_application;    //ģ��application
    string vnm_application ;      //VNM��application
    string vna_application ;      //VNA��application
    string cluster_application;   //cluster��application,������ɫ��hc����Ч
    int32  module_role;           //ģ��Ľ�ɫ,hc,cc,app,other
    int32  register_state;        //ע��״̬���Ƿ�ע��
    int32  send_vnics_state;      //�Ƿ���Ҫ��������������Ϣ��VNA
    int32  timeout;               //��ʱֵ���ݶ�6��û�յ��ͳ�ʱ
    int32  keepalive_state;       //��VNA����״̬
}ModuleInfo;

class CVNetLibNet : public MessageHandler
{
public:
    virtual ~CVNetLibNet();

    static CVNetLibNet *GetInstance( )
    {
        SkyAssert(NULL != s_pInstance);
        /* �����޸Ĺ������ԣ�2013-05-29 
        if(NULL == s_pInstance)
        {
            s_pInstance = new CVNetLibNet( );
        } */

        return s_pInstance;
    };

    static CVNetLibNet *CreateInstance( )
    {
        if(NULL == s_pInstance)
        {
            s_pInstance = new CVNetLibNet( );
        }
        
        return s_pInstance;
    };
    
    void SetDbgInfFlg(BOOL bOpen)
    {
        m_bOpenDbgInf = bOpen;
        return ;
    }
    
    void ShowMidEvent();

    void ShowVnics();


    //ע���¼��ź���Ϣ��Ԫ
    int RegisterEvent(uint32 event,MID mid);
    
    //������application;��ɫ��ϢHC,CC,APP�ȣ��������ɫ��Ϣ
    int32 Init(string application,int role);
    
    //�����Ƿ����
    BOOL GetNetWorkStatus()
    {
        if(m_tmoduleinfo.register_state == VNA_REGISTER_STATE_REG &&
           m_tmoduleinfo.keepalive_state == VNA_LINK_UP)
        {
            return TRUE;
        }
        return FALSE;
    };
    
  #if 0  
    //��HC�ɲ�����ɫ��������
    int32 Init(string application);

    //���ṩһ�����������ĵ���
    int32 Init();
  #endif  
  
    //����ӿڣ�XMLRPC�ϵ���
    //��������ƽ��
    int32 do_cfg_netplane(CNetplaneMsg& message);

    //��������ƽ��IP
    int32 do_cfg_netplaneIP(CNetplaneIPv4RangeMsg& message);

    //��������ƽ��MAC
    int32 do_cfg_netplaneMAC(CMACRangeMsg& message);

    //��������ƽ��VLAN
    int32 do_cfg_netplaneVLAN(CVlanRangeMsg& message);
    
    //��������ƽ��SEGMENT
    int32 do_cfg_netplaneSEGMENT(CSegmentRangeMsg& message);

    //���ö˿���
    int32 do_cfg_portgroup(CPortGroupMsg& cfgmessage);

    int32 do_cfg_portgroupVlan(CPGTrunkVlanRangeMsg& cfgmessage);

    //�����߼�����
    int32 do_cfg_logicnet(CLogicNetworkMsg& cfgmessage);

    //�����߼�����IP
    int32 do_cfg_logicnet_ip(CLogicNetworkIPv4RangeMsg& cfgmessage);

    //���ý���
    int32 do_cfg_switch_base(const CSwitchCfgMsg &cfgmessage,CSwitchCfgAckMsg &ackmesg);

    //���ý����˿�
    int32 do_cfg_switch_port(const CSwitchPortCfgMsg &cfgmessage,CSwitchCfgAckMsg &ackmesg);

    //����kernal�˿�
    int32 do_cfg_kernal_port(CKerNalPortMsg& cfgmessage);

    //����loop�˿�
    int32 do_cfg_loop_port(CLoopPortMsg& cfgmessage);

    //��ѯ����������Ϣ
    int32 do_query_vnics(const MessageFrame& message);
    int32 do_ack_query_vnics(const MessageFrame& message);
    
    //ͨ�����
    int32 do_cfg_wildcast_loop_port(CWildcastLoopbackCfgMsg& cfgmessage);
    int32 do_cfg_wildcast_switch_base(const CWildcastSwitchCfgMsg &cfgmessage,CWildcastSwitchCfgAckMsg &ackmesg);
    int32 do_cfg_wildcast_switch_port(const CWildcastSwitchPortCfgMsg &cfgmessage,CWildcastSwitchCfgAckMsg &ackmesg);
    int32 do_cfg_wildcast_vport(const CWildcastCreateVPortCfgMsg &cfgmessage,CWildcastCreateVPortCfgAckMsg &ackmesg);
    int32 do_cfg_wildcast_delTask(CWildcastTaskDelMsg &cfgmessage,CWildcastTaskDelAckMsg &ackmesg, int32 type);
    
    //���Ź���ز���
    int32 do_watch_dog_oprator(const MessageFrame& message);
    int32 do_ack_watch_dog_oprator(const MessageFrame& message);
    int32 do_wdg_oprator(const MessageFrame& message);
    int32 do_ack_wdg_oprator(const MessageFrame& message);
    int32 do_watch_dog_rr(const MessageFrame& message);
    int32 do_ack_watch_dog_rr(const MessageFrame& message);
    int32 do_wdg_oprator_timer(const MessageFrame& message);
    int32 do_ack_wdg_oprator_timer(const MessageFrame& message);
    int32 do_wdg_oprator_rr(const MessageFrame& message);
    int32 do_ack_wdg_oprator_rr(const MessageFrame& message);

    //VNA delete����
    int32 do_vna_clear(CVNetVnaDelete& cfgmessage,CVNetVnaDeleteAck& cfgackmessage);
  
    int32 do_cfg_logicnet_id2name(CVNetLGNetID2Name& cfgmessage);

    //SDN����
    int32 do_cfg_sdn(CSdnCfgMsg& cfgmessage);
    int32 do_portgroup_query(CVNetPortGroupQuery& cfgmessage, CVnetPortGroupLists& ackmessge);

    //��ȡvnm application
    string & GetVnmApplication() {return m_tmoduleinfo.vnm_application;}

protected:
    int32 InitNetLib(void);
    STATUS Receive(const MessageFrame& message);
    void JoinGroup(const string& strMG);
    void ExitGroup(const string& strMG);
    void MessageEntry(const MessageFrame& message);

    //vnm�鲥��application
    void do_get_vnmapp(const MessageFrame& message);

    //��ȡvna��application
    int32 do_get_vna_application(string &vna_application);

    //��VNAע��Ĵ���
    void do_vna_reg(void);
    void do_ack_vna_reg(const MessageFrame& message);

    //VNA�������VNA�������е�����������Ϣ
    void do_vna_send_vnics(void);

    //LIB��������
    void do_heartbeat(void);
    void do_heartbeatAck(const MessageFrame& message);

    //HOST����CLUSET�Ĵ���
    void do_registercluster(const MessageFrame& message);

    //���������ʱ��TC���ȵ�CC�Ĵ���
    void do_schedule_cc(const MessageFrame& message);
    void do_ack_schedule_cc(const MessageFrame& message);

    //���������ʱ��CC���ȵ�HC�Ĵ���
    void do_schedule_hc(const MessageFrame& message);
    void do_ack_schedule_hc(const MessageFrame& message);

    //���������ʱ,��CC������ͷ�������Դ
    void do_getresource_cc(const MessageFrame& message);
    void do_ack_getresource_cc(const MessageFrame& message);
    void do_freeresource_cc(const MessageFrame& message);
    void do_ack_freeresource_cc(const MessageFrame& message);

    //���������ʱ,��hc������ͷ�������Դ
    void do_getresource_hc(const MessageFrame& message);
    void do_ack_getresource_hc(const MessageFrame& message);
    void do_freeresource_hc(const MessageFrame& message);
    void do_ack_freeresource_hc(const MessageFrame& message);

    //startnetwork��ԴУ��
    void do_startnetwork(const MessageFrame& message);
    void do_ack_startnetwork(const MessageFrame& message);

    //����ʱ��stopnetwork֪ͨvna����
    void do_stopnetwork(const MessageFrame& message);
    void do_ack_stopnetwork(const MessageFrame& message);
    
    //������������
    void do_notify_domid(const MessageFrame& message);
    void do_ack_notify_domid(const MessageFrame& message);

    //��Ǩ�ƽӿ�
    int32 do_vm_migrate_begin(const MessageFrame& message);
    int32 do_vm_migrate_begin_ack(const MessageFrame& message);
    int32 do_vm_migrate_end(const MessageFrame& message);   
    int32 do_vm_migrate_end_ack(const MessageFrame& message);
    int32 do_select_migrate_hc(const MessageFrame& message);                
    int32 do_select_migrate_hc_ack(const MessageFrame& message);
            
private:
    CVNetLibNet();
    STATUS StartMu(const string& name);

    //�ڲ�ʹ�õģ���Ϣӳ��
    int GetMidByEvent(uint32 event, MID &mid);
    map<uint32,MID> Event_MID;
    
    static CVNetLibNet *s_pInstance;
    MessageUnit *mu;
    pthread_mutex_t m_mutex;

    //���������������б�
    CVNetAllVMVSIMsg m_vec_allvnics;
    
    TIMER_ID m_tHeartBeatTimer;
    ModuleInfo m_tmoduleinfo;
    BOOL m_bOpenDbgInf;
    DISALLOW_COPY_AND_ASSIGN(CVNetLibNet);
};
}//end namespace
#endif
