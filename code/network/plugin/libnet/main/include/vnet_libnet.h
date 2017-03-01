/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：vnet_libnet.h
* 文件标识：
* 内容摘要：vnet_libnet.h类的定义文件
* 当前版本：1.0
* 作    者：
* 完成日期：2012年12月16日
*
* 修改记录1：
*     修改日期：2012/1/8
*     版 本 号：V1.0
*     修 改 人：chenzhiwei
*     修改内容：创建
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

//LIB库与VNA间链路状态
#define VNA_LINK_UP    0           // 链路UP 并且RUNNING
#define VNA_LINK_FAIL  1           // 链路刚刚down
#define VNA_LINK_DOWN  2           // 链路断了一段时间
#define VNA_LINK_BACK  3           // 链路恢复中

#define VNA_REGISTER_STATE_NOREG    0 //未注册
#define VNA_REGISTER_STATE_REG      1 //已经注册 

#define VNA_NOT_NEED_SEND_VMNIC     0  //不再需要发送
#define VNA_NEED_SEND_VMNIC         1  //没有确认，还需要再发

//组播组名后缀，组播组名= application +后缀
//#define MUTIGROUP_REG_SYSTEM       "_reg_system"


//定时器间隔
#define  VNETLIB_LOOP_INTERVAL   (2000)
#define  VNETLIB_KEEPLIVE_INTERRVAL (5)

#define MU_VNET_LIBNET    "vnet_libnet"

//VM NET 操作宏
#define VNET_VM_DEPLOY   (0)  /* 普通部署 */
#define VNET_VM_PREPARE  (1)  /* 预部署 */
#define VNET_VM_MIGRATE  (2)  /* 迁移 */
#define VNET_VM_SUSPEND  (3)  /* 暂不用，保留 */
#define VNET_VM_CANCEL   (4)  /* 撤销 */

//VM NET 操作结果
#define VNET_VM_SUCCESS  (1)
#define VNET_VM_FAILED   (0)

//VNET_LIB 状态
#define VNETLIB_STATE_MACHINE_START  (1)
#define VNETLIB_STATE_MACHINE_WORK   (2)

typedef struct tagModuleInfo
{
    string module_application;    //模块application
    string vnm_application ;      //VNM的application
    string vna_application ;      //VNA的application
    string cluster_application;   //cluster的application,仅当角色是hc才有效
    int32  module_role;           //模块的角色,hc,cc,app,other
    int32  register_state;        //注册状态，是否注册
    int32  send_vnics_state;      //是否还需要发送虚拟网卡信息到VNA
    int32  timeout;               //超时值，暂定6个没收到就超时
    int32  keepalive_state;       //与VNA心跳状态
}ModuleInfo;

class CVNetLibNet : public MessageHandler
{
public:
    virtual ~CVNetLibNet();

    static CVNetLibNet *GetInstance( )
    {
        SkyAssert(NULL != s_pInstance);
        /* 暂且修改过来测试，2013-05-29 
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


    //注册事件号和消息单元
    int RegisterEvent(uint32 event,MID mid);
    
    //调用者application;角色信息HC,CC,APP等，必须填角色信息
    int32 Init(string application,int role);
    
    //网络是否可用
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
    //非HC可不带角色参数调用
    int32 Init(string application);

    //再提供一个不带参数的调用
    int32 Init();
  #endif  
  
    //对外接口，XMLRPC上调用
    //配置网络平面
    int32 do_cfg_netplane(CNetplaneMsg& message);

    //配置网络平面IP
    int32 do_cfg_netplaneIP(CNetplaneIPv4RangeMsg& message);

    //配置网络平面MAC
    int32 do_cfg_netplaneMAC(CMACRangeMsg& message);

    //配置网络平面VLAN
    int32 do_cfg_netplaneVLAN(CVlanRangeMsg& message);
    
    //配置网络平面SEGMENT
    int32 do_cfg_netplaneSEGMENT(CSegmentRangeMsg& message);

    //配置端口组
    int32 do_cfg_portgroup(CPortGroupMsg& cfgmessage);

    int32 do_cfg_portgroupVlan(CPGTrunkVlanRangeMsg& cfgmessage);

    //配置逻辑网络
    int32 do_cfg_logicnet(CLogicNetworkMsg& cfgmessage);

    //配置逻辑网络IP
    int32 do_cfg_logicnet_ip(CLogicNetworkIPv4RangeMsg& cfgmessage);

    //配置交换
    int32 do_cfg_switch_base(const CSwitchCfgMsg &cfgmessage,CSwitchCfgAckMsg &ackmesg);

    //配置交换端口
    int32 do_cfg_switch_port(const CSwitchPortCfgMsg &cfgmessage,CSwitchCfgAckMsg &ackmesg);

    //配置kernal端口
    int32 do_cfg_kernal_port(CKerNalPortMsg& cfgmessage);

    //配置loop端口
    int32 do_cfg_loop_port(CLoopPortMsg& cfgmessage);

    //查询虚拟网卡信息
    int32 do_query_vnics(const MessageFrame& message);
    int32 do_ack_query_vnics(const MessageFrame& message);
    
    //通配相关
    int32 do_cfg_wildcast_loop_port(CWildcastLoopbackCfgMsg& cfgmessage);
    int32 do_cfg_wildcast_switch_base(const CWildcastSwitchCfgMsg &cfgmessage,CWildcastSwitchCfgAckMsg &ackmesg);
    int32 do_cfg_wildcast_switch_port(const CWildcastSwitchPortCfgMsg &cfgmessage,CWildcastSwitchCfgAckMsg &ackmesg);
    int32 do_cfg_wildcast_vport(const CWildcastCreateVPortCfgMsg &cfgmessage,CWildcastCreateVPortCfgAckMsg &ackmesg);
    int32 do_cfg_wildcast_delTask(CWildcastTaskDelMsg &cfgmessage,CWildcastTaskDelAckMsg &ackmesg, int32 type);
    
    //看门狗相关操作
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

    //VNA delete操作
    int32 do_vna_clear(CVNetVnaDelete& cfgmessage,CVNetVnaDeleteAck& cfgackmessage);
  
    int32 do_cfg_logicnet_id2name(CVNetLGNetID2Name& cfgmessage);

    //SDN配置
    int32 do_cfg_sdn(CSdnCfgMsg& cfgmessage);
    int32 do_portgroup_query(CVNetPortGroupQuery& cfgmessage, CVnetPortGroupLists& ackmessge);

    //获取vnm application
    string & GetVnmApplication() {return m_tmoduleinfo.vnm_application;}

protected:
    int32 InitNetLib(void);
    STATUS Receive(const MessageFrame& message);
    void JoinGroup(const string& strMG);
    void ExitGroup(const string& strMG);
    void MessageEntry(const MessageFrame& message);

    //vnm组播的application
    void do_get_vnmapp(const MessageFrame& message);

    //获取vna的application
    int32 do_get_vna_application(string &vna_application);

    //与VNA注册的处理
    void do_vna_reg(void);
    void do_ack_vna_reg(const MessageFrame& message);

    //VNA重启后给VNA发送所有的虚拟网卡信息
    void do_vna_send_vnics(void);

    //LIB心跳处理
    void do_heartbeat(void);
    void do_heartbeatAck(const MessageFrame& message);

    //HOST归属CLUSET的处理
    void do_registercluster(const MessageFrame& message);

    //部署虚拟机时，TC调度到CC的处理
    void do_schedule_cc(const MessageFrame& message);
    void do_ack_schedule_cc(const MessageFrame& message);

    //部署虚拟机时，CC调度到HC的处理
    void do_schedule_hc(const MessageFrame& message);
    void do_ack_schedule_hc(const MessageFrame& message);

    //部署虚拟机时,给CC分配和释放网络资源
    void do_getresource_cc(const MessageFrame& message);
    void do_ack_getresource_cc(const MessageFrame& message);
    void do_freeresource_cc(const MessageFrame& message);
    void do_ack_freeresource_cc(const MessageFrame& message);

    //部署虚拟机时,给hc分配和释放网络资源
    void do_getresource_hc(const MessageFrame& message);
    void do_ack_getresource_hc(const MessageFrame& message);
    void do_freeresource_hc(const MessageFrame& message);
    void do_ack_freeresource_hc(const MessageFrame& message);

    //startnetwork资源校验
    void do_startnetwork(const MessageFrame& message);
    void do_ack_startnetwork(const MessageFrame& message);

    //撤销时候stopnetwork通知vna清理
    void do_stopnetwork(const MessageFrame& message);
    void do_ack_stopnetwork(const MessageFrame& message);
    
    //设置网卡参数
    void do_notify_domid(const MessageFrame& message);
    void do_ack_notify_domid(const MessageFrame& message);

    //热迁移接口
    int32 do_vm_migrate_begin(const MessageFrame& message);
    int32 do_vm_migrate_begin_ack(const MessageFrame& message);
    int32 do_vm_migrate_end(const MessageFrame& message);   
    int32 do_vm_migrate_end_ack(const MessageFrame& message);
    int32 do_select_migrate_hc(const MessageFrame& message);                
    int32 do_select_migrate_hc_ack(const MessageFrame& message);
            
private:
    CVNetLibNet();
    STATUS StartMu(const string& name);

    //内部使用的，消息映射
    int GetMidByEvent(uint32 event, MID &mid);
    map<uint32,MID> Event_MID;
    
    static CVNetLibNet *s_pInstance;
    MessageUnit *mu;
    pthread_mutex_t m_mutex;

    //缓存下虚拟网卡列表
    CVNetAllVMVSIMsg m_vec_allvnics;
    
    TIMER_ID m_tHeartBeatTimer;
    ModuleInfo m_tmoduleinfo;
    BOOL m_bOpenDbgInf;
    DISALLOW_COPY_AND_ASSIGN(CVNetLibNet);
};
}//end namespace
#endif
