/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：vnet_libnet.cpp
* 文件标识：
* 内容摘要：net库的实现
* 当前版本：1.0
* 作    者：
* 完成日期：2012年12月16日
*
* 修改记录1：
*     修改日期：2012/12/16
*     版 本 号：V1.0
*     修 改 人：chenzhiwei
*     修改内容：创建
******************************************************************************/
#include "vnet_libnet.h"


namespace zte_tecs
{

#define WAITE_MSG_ACK(req, msg_id, tmp_mu_name) \
        MessageUnit temp_mu(TempUnitName(tmp_mu_name)); \
        temp_mu.Register(); \
        MID receiver; \
        receiver._application = m_tmoduleinfo.vnm_application; \
        receiver._process = PROC_VNM; \
        receiver._unit = MU_VNM; \
        MessageOption option(receiver, (msg_id), 0, 0); \
        temp_mu.Send(req, option); \
        MessageFrame message; \
        if(SUCCESS == temp_mu.Wait(&message, 5000))

#define WAITE_MSG_ACK_TIMES(req, msg_id, tmp_mu_name,times) \
    MessageUnit temp_mu(TempUnitName(tmp_mu_name)); \
    temp_mu.Register(); \
    MID receiver; \
    receiver._application = m_tmoduleinfo.vnm_application; \
    receiver._process = PROC_VNM; \
    receiver._unit = MU_VNM; \
    MessageOption option(receiver, (msg_id), 0, 0); \
    temp_mu.Send(req, option); \
    MessageFrame message; \
    if(SUCCESS == temp_mu.Wait(&message, times))


#define TEMP_MU_CFG_NETPLANE         "VnetCfgNetplane"
#define TEMP_MU_CFG_NETPLANE_IP      "VnetCfgNetplaneIP"
#define TEMP_MU_CFG_NETPLANE_MAC     "VnetCfgNetplaneMAC"
#define TEMP_MU_CFG_NETPLANE_VLAN    "VnetCfgNetplaneVLAN"
#define TEMP_MU_CFG_NETPLANE_SEGMENT "VnetCfgNetplaneSEGMENT"
#define TEMP_MU_CFG_PORTGROUP        "VnetCfgportgroup"
#define TEMP_MU_CFG_PORTGROUP_VLAN   "VnetCfgportgroupVLAN"
#define TEMP_MU_PORTGROUP_QUERY   "VnetPortGroupQuery"
#define TEMP_MU_CFG_LOGICNET         "VnetCfglogicnet"
#define TEMP_MU_CFG_LOGICNET_IP      "VnetCfglogicnetIP"
#define TEMP_MU_CFG_SWITCH_BASE      "VnetCfgswitchBASE"
#define TEMP_MU_CFG_SWITCH_PORT      "VnetCfgswitchPORT"
#define TEMP_MU_CFG_KERNA_PORT       "VnetCfgkernalPORT"
#define TEMP_MU_CFG_LOOP_PORT        "VnetCfgLooplPORT"

#define TEMP_MU_CFG_WC_LOOP_PORT        "VnetCfgwcLooplPORT"
#define TEMP_MU_CFG_WC_SWITCH_BASE      "VnetCfgwcswitchBASE"
#define TEMP_MU_CFG_WC_SWITCH_PORT      "VnetCfgwcswitchPORT"
#define TEMP_MU_CFG_WC_VIRTUAL_PORT     "VnetCfgwcsvirtualPORT"
#define TEMP_MU_CFG_WC_DEL_TASK         "VnetCfgwcDelTask"
#define TEMP_MU_VNA_DELETE              "VnetVNADelete"
#define TEMP_MU_LGNET_NAME2ID           "VnetLGNetID2Name"
#define TEMP_MU_CFG_SDN                 "VnetCfgSDN"


     
CVNetLibNet* CVNetLibNet::s_pInstance = NULL;

CVNetLibNet::CVNetLibNet()
{
    m_bOpenDbgInf = 0;
    m_tHeartBeatTimer= INVALID_TIMER_ID;
    mu = NULL;
}

CVNetLibNet::~CVNetLibNet()
{
    if (INVALID_TIMER_ID != m_tHeartBeatTimer)
    {
        mu->KillTimer(m_tHeartBeatTimer);
        m_tHeartBeatTimer = INVALID_TIMER_ID;
    }
    delete mu;
    delete s_pInstance;
    pthread_mutex_destroy(&m_mutex);
}

STATUS CVNetLibNet::StartMu(const string& name)
{
    if(mu)
    {
        SkyAssert(0);
        return ERROR_DUPLICATED_OP;
    }
    
    //消息单元的创建和初始化
    mu = new MessageUnit(name);
    if (!mu)
    {
        cout << "Create mu failed mu name "<<name <<endl;
        return ERROR_NO_MEMORY;
    }

    STATUS ret = mu->SetHandler(this);
    if (SUCCESS != ret)
    {
        cout <<"mu->SetHandler failed! ret "<<ret<<" mu name "<<name<<endl;
        return ret;
    }

    ret = mu->Run();
    if (SUCCESS != ret)
    {
        cout<<"mu->Run failed ret "<<ret<<" mu name"<<name<<endl;
        return ret;
    }

    ret = mu->Register();
    if (SUCCESS != ret)
    {
        cout<<"mu->Register failed ret "<<ret<<" mu name "<<name<<endl;
        return ret;
    }

    //给自己发送上电消息，促使消息单元状态切换到工作态
    MessageFrame message(SkyInt32(0), EV_VNETLIB_POWER_ON);
    ret = mu->Receive(message);
    if (SUCCESS != ret)
    {
        SkyAssert(0);
        return ret;
    }
    mu->Print();
    return SUCCESS;
}

STATUS CVNetLibNet::Receive(const MessageFrame& message)
{
    return mu->Receive(message);
}

void CVNetLibNet::JoinGroup(const string& strMG)
{
    //加入一个组播组
    STATUS nRet = mu->JoinMcGroup(strMG);
    if (SUCCESS == nRet)
    {
        cout <<"JoinGroup "<<strMG<<" successful!"<<endl;
    }
    else
    {
        cout <<"JoinGroup "<<strMG<<" failed!"<<" ret "<<nRet<<endl;
    }
    return;
}

void CVNetLibNet::ExitGroup(const string& strMG)
{

}

void CVNetLibNet::do_get_vnmapp(const MessageFrame& message)
{
    CVNMInfoMsg cmsg;
    cmsg.deserialize(message.data);
    if(VNM_MASTER_STATE == cmsg.m_ucMasterOrSlave)
    {
        m_tmoduleinfo.vnm_application = cmsg.m_strApp;
    }
    return;
}

int32 CVNetLibNet::do_get_vna_application(string &vna_application)
{

    vna_application.clear();
    string tmpVNAUUID;

   // if (0 != access(VNETLIB_VNA_UUID_FILE, 0))
   // {
   //     return -1;
   // }
            
    ostringstream file;
    file << VNETLIB_VNA_UUID_FILE ;
    ifstream fin(file.str().c_str());
    fin >> tmpVNAUUID; 

    vna_application = tmpVNAUUID;
  
    return 0;
}

void CVNetLibNet::do_vna_reg()
{
    if(m_tmoduleinfo.vna_application.empty())
    {
       do_get_vna_application(m_tmoduleinfo.vna_application);

       if(m_tmoduleinfo.vna_application.empty())
       {
           return;
       }

    }
   if(m_bOpenDbgInf == 1)
   {
       cout << "vna_app        :"<<m_tmoduleinfo.vna_application <<endl;
       cout << "module app     :"<<m_tmoduleinfo.module_application <<endl;
       cout << "module role    :"<<m_tmoduleinfo.module_role<<endl;
       cout << "cluster_app    :"<<m_tmoduleinfo.cluster_application<<endl;
       cout << "register state :"<<m_tmoduleinfo.register_state<<endl;
    }
    CVNetModuleRegMsg cMsg;
    cMsg.set_module_app(m_tmoduleinfo.module_application);
    cMsg.set_module_role(m_tmoduleinfo.module_role);
    if( m_tmoduleinfo.module_role == VNA_MODULE_ROLE_HC )
    {
        cMsg.set_module_cluster(m_tmoduleinfo.cluster_application);
    }
    
    MID receiver(m_tmoduleinfo.vna_application,PROC_VNA, MU_VNA_AGENT);
    MessageOption option(receiver,EV_VNETLIB_REGISTER_MSG, 0, 0);
    
    if (SUCCESS != mu->Send(cMsg,option))
    {
        cout << "send heart beat message to vna failed \n"<<endl;
    }
    return;
}

void CVNetLibNet::do_vna_send_vnics(void)
{
    if(m_tmoduleinfo.vna_application.empty())
    {
        cout<<"in do_ack_vna_reg vna application is null"<<endl;
        do_get_vna_application(m_tmoduleinfo.vna_application);
        if(m_tmoduleinfo.vna_application.empty())
        {
           return;
        }
    }
    
    MID receiver(m_tmoduleinfo.vna_application,PROC_VNA, MU_VNA_CONTROLLER);
    MessageOption option(receiver,EV_VNETLIB_NOTIFY_ALL_VMNIC, 0, 0);
    
    if (SUCCESS != mu->Send(m_vec_allvnics,option))
    {
        cout << "send EV_VNETLIB_NOTIFY_ALL_VMNIC to vna failed \n"<<endl;
    }
    return;
}

void CVNetLibNet::do_ack_vna_reg(const MessageFrame& message)
{
    if(m_tmoduleinfo.vna_application.empty())
    {
        cout<<"in do_ack_vna_reg vna application is null"<<endl;
        return;
    }
    CVNetModuleACKRegMsg cmsg;
    cmsg.deserialize(message.data);

    //修改注册状态
    if(m_tmoduleinfo.module_application == cmsg.get_module_app())
    {
        m_tmoduleinfo.register_state = VNA_REGISTER_STATE_REG;
    }
    else
    {
        cout<<"do_ack_vna_reg receive an unknow ack "<<cmsg.get_module_app()<<endl;
        return;
    }
}

void CVNetLibNet::do_heartbeat()
{
    if(m_bOpenDbgInf == 1)
    {
        cout << "vna_app        :"<<m_tmoduleinfo.vna_application <<endl;
        cout << "module app     :"<<m_tmoduleinfo.module_application <<endl;
        cout << "modue role     :"<<m_tmoduleinfo.module_role<<endl;
        cout << "register state :"<<m_tmoduleinfo.register_state<<endl;
        cout << "keepalive_state  :"<<m_tmoduleinfo.keepalive_state<<endl;
    }
    
    // TC 不需要和VNA注册 
    if( m_tmoduleinfo.module_role == VNA_MODULE_ROLE_TC )
    {
        return;
    }
    
    if(m_tmoduleinfo.vna_application.empty())
    {
        cout<<"vna application is null"<<endl;
        do_get_vna_application(m_tmoduleinfo.vna_application);
        if(m_tmoduleinfo.vna_application.empty())
        {
            cout<<"do_hearbeat vna application is null"<<endl;
            return;           
        }
    }
    
    if(VNA_REGISTER_STATE_NOREG == m_tmoduleinfo.register_state)
    {
        cout<<"will do_vna_reg"<<endl;
        //每次心跳都会判断是否和VNA注册,并发送存储的虚拟网卡配置
        do_vna_reg(); 
        do_vna_send_vnics();
    }
    
    if( --m_tmoduleinfo.timeout == 0)
    {
        m_tmoduleinfo.keepalive_state = VNA_LINK_DOWN;
        m_tmoduleinfo.register_state = VNA_REGISTER_STATE_NOREG;
    }
    
    CVNetVNAHeartBeatMsg cMsg;
    cMsg.set_module_app(m_tmoduleinfo.module_application);
    cMsg.set_module_role(m_tmoduleinfo.module_role);
    
    MID receiver(m_tmoduleinfo.vna_application,PROC_VNA, MU_VNA_AGENT);
    MessageOption option(receiver,EV_VNETLIB_HEART_BEAT_MSG, 0, 0);
    
    if (SUCCESS != mu->Send(cMsg,option))
    {
        cout << "send heart beat message to vna failed \n"<<endl;
    }
    return;
}

void CVNetLibNet::do_heartbeatAck(const MessageFrame& message)
{
    if(m_tmoduleinfo.vna_application.empty())
    {
        //之前已经有了VNA的application才能发出去，这时候没有肯定是异常了
        //SkyAssert(false);
        cout <<"in do_heartbeatAck vna_application is null"<<endl;
        return;
    }
    
    CVNetVNAHeartBeatAckMsg cAckMsg;
    cAckMsg.deserialize(message.data);
    if( m_tmoduleinfo.module_application != cAckMsg.get_module_app())
    {
        cout << "do_heartbeatAck receive a message application unkonw"<<endl;
        return;
    }
    m_tmoduleinfo.timeout =  VNETLIB_KEEPLIVE_INTERRVAL;
    m_tmoduleinfo.keepalive_state = VNA_LINK_UP;

    if(cAckMsg.m_ret_code != 0)
    {
        m_tmoduleinfo.register_state = VNA_REGISTER_STATE_NOREG;
    }
    return;
}

void CVNetLibNet::do_registercluster(const MessageFrame& message)
{
    MessageRegisteredSystem msg;
    msg.deserialize(message.data);
    m_tmoduleinfo.cluster_application = msg._system_name;

    CvnetVNAClusterMsg cMsg;
    cMsg.set_module_app(m_tmoduleinfo.module_application);
    if(m_tmoduleinfo.module_role == VNA_MODULE_ROLE_HC)
    {
        cMsg.set_cluster_app(m_tmoduleinfo.cluster_application);
    }
    else
    {
        cout<<"receive a cluster mesg ,but role is not host"<<endl;
        return;
    }
    
    MID receiver(m_tmoduleinfo.vna_application,PROC_VNA, MU_VNA_AGENT);
    MessageOption option(receiver,EV_VNETLIB_HC_REGISTER_MSG, 0, 0);
    
    if (SUCCESS != mu->Send(cMsg,option))
    {
        cout << "send heart register cluster message to vna failed \n"<<endl;
    }
    return;
}

void CVNetLibNet::do_schedule_cc(const MessageFrame& message)
{
    //此时还没有获取到vnm的application,和AB模型无关，因为不涉及申请资源,所以直接给TC返回失败,
    if(m_tmoduleinfo.vnm_application.empty())
    {
        cout<< "do_schedule_cc vnm app is empty"<<endl;
        CVNetSelectCCListMsg cmsg;
        cmsg.deserialize(message.data);

        MID sender_mid = message.option.sender();
        if(sender_mid._application.empty())
        {
            cout<<"sender's application is null,cant ack select cc message" <<endl;
            //SkyAssert(false);
            return;
        }

        MessageOption option(sender_mid,EV_VNETLIB_SELECT_CC_ACK, 0, 0);

        CVNetSelectCCListMsgAck cackmsg;
        cackmsg.state = ERROR;
        cackmsg.detail = "vnm application is null";
        if (SUCCESS != mu->Send(cackmsg,option))
        {
            cout << "do_schedule_cc send msg to tc failed \n"<<endl;
        }
        return;
    }

    MID sender_mid = message.option.sender();
    CVNetSelectCCListMsg cmsg;
    cmsg.deserialize(message.data);
    cmsg.m_mid = sender_mid;

    //将消息转发给VNM
    MID receiver(m_tmoduleinfo.vnm_application,PROC_VNM, MU_VNM);
    MessageOption option(receiver,EV_VNETLIB_SELECT_CC, 0, 0);
    
    if (SUCCESS != mu->Send(cmsg,option))
    {
        cout << "send select cc message to vnm failed \n"<<endl;
    }
    return;      
}

void CVNetLibNet::do_ack_schedule_cc(const MessageFrame& message)
{
    CVNetSelectCCListMsgAck cmsg;
    cmsg.deserialize(message.data);

    MID receiver =cmsg.m_mid;
    if(receiver._application.empty())
    {
        cout<<"sender's application is null,cant ack select cc message" <<endl;
        //SkyAssert(false);
        return;
    }
    MessageOption option(receiver,EV_VNETLIB_SELECT_CC_ACK, 0, 0);

     //转发vnm的调度到的CC消息给TC
    if (SUCCESS != mu->Send(message.data,option))
    {
        cout << "do_ack_schedule_cc send msg to tc failed \n"<<endl;
    }
    return;      
}

void CVNetLibNet::do_schedule_hc(const MessageFrame& message)
{
    //此时还没有获取到vnm的application,和AB模型无关，因为不涉及申请资源,所以直接给CC返回失败,
    if(m_tmoduleinfo.vnm_application.empty())
    {
        cout<< "do_schedule_cc vnm app is empty"<<endl;
        CVNetSelectHCListMsg cmsg;
        cmsg.deserialize(message.data);

        MID sender_mid = message.option.sender();
        if(sender_mid._application.empty())
        {
            cout<<"sender's application is null,cant ack select hc message" <<endl;
            //SkyAssert(false);
            return;
        }
        MessageOption option(sender_mid,EV_VNETLIB_SELECT_HC_ACK, 0, 0);

        CVNetSelectHCListMsgAck cackmsg;
        cackmsg.state = ERROR;
        cackmsg.detail = "vnm application is null";
        if (SUCCESS != mu->Send(cackmsg,option))
        {
            cout << "do_schedule_hc send msg to cc failed \n"<<endl;
        }
        return;
    }

    //将消息转发给VNM
    MID receiver(m_tmoduleinfo.vnm_application,PROC_VNM, MU_VNM);
    MessageOption option(receiver,EV_VNETLIB_SELECT_HC, 0, 0);

    CVNetSelectHCListMsg cmsg;
    cmsg.deserialize(message.data);

    cmsg.m_mid = message.option.sender();
    
    if (SUCCESS != mu->Send(cmsg,option))
    {
        cout << "send select cc message to vnm failed \n"<<endl;
    }
    return;      
}

void CVNetLibNet::do_ack_schedule_hc(const MessageFrame& message)
{
    CVNetSelectHCListMsgAck cmsg;
    cmsg.deserialize(message.data);

    MID receiver =cmsg.m_mid;
    if(receiver._application.empty())
    {
        cout<<"sender's application is null,cant ack select HC message" <<endl;
        //SkyAssert(false);
        return;
    }
    MessageOption option(receiver,EV_VNETLIB_SELECT_HC_ACK, 0, 0);

     //转发vnm的调度到的HC消息给CC
    if (SUCCESS != mu->Send(message.data,option))
    {
        cout << "do_ack_schedule_hc  send msg to cc failed \n"<<endl;
    }
    return;      
}

void CVNetLibNet::do_getresource_cc(const MessageFrame& message)
{
    if(m_tmoduleinfo.vnm_application.empty())
    {
        cout<< "do_getresource_cc vnm app is empty"<<endl;
        //这里根据AB模型，默认占用资源，不回应A端也可以，省去A端对这个回应消息的处理
#if 0
        CVNetGetResourceCC cmsg;
        cmsg.deserialize(message.data);
        MID acker = cmsg.VmNicCfgList.m_mid;
        
        if(acker._application.empty())
        {
            cout<<"sender's application is null,cant ack getresource cc message" <<endl;
            return;
        }
        MessageOption option(acker,EV_VNETLIB_GETRESOURCE_CC_ACK, 0, 0);

        CVNetGetResourceCC cackmsg;
        cackmsg.VmNicCfgList.m_result = VNETLIB_RESULT_LINK_NOREADY;
        if (SUCCESS != mu->Send(cackmsg,option))
        {
            cout << "do_getresource_cc send msg to tc failed \n"<<endl;
        }
#endif
        return;
    }

    //转发申请资源的消息到VNM
    MID receiver(m_tmoduleinfo.vnm_application,PROC_VNM, MU_VNM);
    MessageOption option(receiver,EV_VNETLIB_GETRESOURCE_CC, 0, 0);

    CVNetGetResourceCCMsg cmsg;
    cmsg.deserialize(message.data);
    cmsg.m_mid = message.option.sender();

    if (SUCCESS != mu->Send(cmsg,option))
    {
        cout << "send EV_VNETLIB_GETRESOURCE_CC message to vnm failed \n"<<endl;
    }
    return;      
}

void CVNetLibNet::do_ack_getresource_cc(const MessageFrame& message)
{
    CVNetGetResourceCCMsgAck cmsg;
    cmsg.deserialize(message.data);

    MID receiver = cmsg.m_mid;
    
    if(receiver._application.empty())
    {
        cout<<"sender's application is null,cant do_ack_getresource_cc" <<endl;
        //SkyAssert(false);
        return;
    }

    //转发VNM应答的申请CC上资源的消息
    MessageOption option(receiver,EV_VNETLIB_GETRESOURCE_CC_ACK, 0, 0);
    if (SUCCESS != mu->Send(message.data,option))
    {
        cout << "do_ack_getresource_cc send msg to tc failed \n"<<endl;
    }
    return;      
}

void CVNetLibNet::do_freeresource_cc(const MessageFrame& message)
{
    if(m_tmoduleinfo.vnm_application.empty())
    {
        cout<< "do_freeresource_cc vnm app is empty"<<endl;
       //这里根据AB模型，不回应A端也可以，省去A端对这个回应消息的处理
#if 0
        CVNetFreeResourceCC cmsg;
        cmsg.deserialize(message.data);
        
        MID acker = cmsg.VmNicCfgList.m_mid;

        if(acker._application.empty())
        {
            cout<<"sender's application is null,cant ack freeresource cc message" <<endl;
            return;
        }
        MessageOption option(acker,EV_VNETLIB_FREERESOURCE_CC_ACK, 0, 0);

        CVNetGetResourceCC cackmsg;
        cackmsg.VmNicCfgList.m_result = VNETLIB_RESULT_LINK_NOREADY;
        if (SUCCESS != mu->Send(cackmsg,option))
        {
            cout << "do_ack_getresource_cc send msg to tc failed \n"<<endl;
        }
#endif   
        return;
    }
    
    MID receiver(m_tmoduleinfo.vnm_application,PROC_VNM, MU_VNM);
    MessageOption option(receiver,EV_VNETLIB_FREERESOURCE_CC, 0, 0);

    CVNetFreeResourceCCMsg cmsg;
    cmsg.deserialize(message.data);
    cmsg.m_mid = message.option.sender();

    //转发释放CC上资源的消息给TC
    if (SUCCESS != mu->Send(cmsg,option))
    {
        cout << "send EV_VNETLIB_FREERESOURCE_CC message to vnm failed \n"<<endl;
    }
    return;      
}

void CVNetLibNet::do_ack_freeresource_cc(const MessageFrame& message)
{
    CVNetFreeResourceCCMsgAck cmsg;
    cmsg.deserialize(message.data);

    MID receiver = cmsg.m_mid;
    if(receiver._application.empty())
    {
        cout<<"sender's application is null,cant ack select cc message" <<endl;
        //SkyAssert(false);
        return;
    }
    MessageOption option(receiver,EV_VNETLIB_FREERESOURCE_CC_ACK, 0, 0);

    //转发TC释放CC上资源的消息到TC
    if (SUCCESS != mu->Send(message.data,option))
    {
        cout << "do_ack_freeresource_cc send msg to tc failed \n"<<endl;
    }
    return;      
}
    
void CVNetLibNet::do_getresource_hc(const MessageFrame& message)
{
    if(m_tmoduleinfo.vnm_application.empty())
    {
        cout<< "do_getresource_hc vnm app is empty"<<endl;
        //这里根据AB模型，不回应A端也可以，省去A端对这个回应消息的处理
#if 0
        CVNetGetResourceHC cmsg;
        cmsg.deserialize(message.data);
        MID acker = cmsg.VmNicCfgList.m_mid;
        
        if(acker._application.empty())
        {
            cout<<"sender's application is null,cant ack getresource cc message" <<endl;
            return;
        }
        MessageOption option(acker,EV_VNETLIB_GETRESOURCE_HC_ACK, 0, 0);

        CVNetGetResourceHC cackmsg;
        cackmsg.VmNicCfgList.m_result = VNETLIB_RESULT_LINK_NOREADY;
        if (SUCCESS != mu->Send(cackmsg,option))
        {
            cout << "do_getresource_hc send msg to tc failed \n"<<endl;
        }
#endif
        return;
    }

    //转发申请资源的消息到VNM
    MID receiver(m_tmoduleinfo.vnm_application,PROC_VNM, MU_VNM);
    MessageOption option(receiver,EV_VNETLIB_GETRESOURCE_HC, 0, 0);

    CVNetGetResourceHCMsg cmsg;
    cmsg.deserialize(message.data);
    cmsg.m_mid = message.option.sender();

    if (SUCCESS != mu->Send(cmsg,option))
    {
        cout << "send EV_VNETLIB_GETRESOURCE_HC message to vnm failed \n"<<endl;
    }
    return;      
}
    
void CVNetLibNet::do_ack_getresource_hc(const MessageFrame& message)
{
    CVNetGetResourceHCMsgAck cmsg;
    cmsg.deserialize(message.data);

    MID receiver = cmsg.m_mid;
    
    if(receiver._application.empty())
    {
        cout<<"sender's application is null,cant do_ack_getresource_cc" <<endl;
        //SkyAssert(false);
        return;
    }

    //转发VNM应答的申请HC上资源的消息
    MessageOption option(receiver,EV_VNETLIB_GETRESOURCE_HC_ACK, 0, 0);
    if (SUCCESS != mu->Send(message.data,option))
    {
        cout << "do_ack_getresource_hc send msg to tc failed \n"<<endl;
    }
    return;      
}

void CVNetLibNet::do_freeresource_hc(const MessageFrame& message)
{
    if(m_tmoduleinfo.vnm_application.empty())
    {
        cout<< "do_freeresource_hc vnm app is empty"<<endl;
       //这里根据AB模型，不回应A端也可以，省去A端对这个回应消息的处理
#if 0
        CVNetFreeResourceHC cmsg;
        cmsg.deserialize(message.data);
        
        MID acker = cmsg.VmNicCfgList.m_mid;

        if(acker._application.empty())
        {
            cout<<"sender's application is null,cant ack do_freeresource_hc message" <<endl;
            return;
        }
        MessageOption option(acker,EV_VNETLIB_FREERESOURCE_HC_ACK, 0, 0);

        CVNetGetResourceHC cackmsg;
        cackmsg.VmNicCfgList.m_result = VNETLIB_RESULT_LINK_NOREADY;
        if (SUCCESS != mu->Send(cackmsg,option))
        {
            cout << "do_freeresource_hc send msg to tc failed \n"<<endl;
        }
#endif   
        return;
    }
    
    MID receiver(m_tmoduleinfo.vnm_application,PROC_VNM, MU_VNM);
    MessageOption option(receiver,EV_VNETLIB_FREERESOURCE_HC, 0, 0);

    CVNetFreeResourceHCMsg cmsg;
    cmsg.deserialize(message.data);
    cmsg.m_mid = message.option.sender();

    //转发释放hc上资源的消息给cc
    if (SUCCESS != mu->Send(cmsg,option))
    {
        cout << "send EV_VNETLIB_FREERESOURCE_HC message to vnm failed \n"<<endl;
    }
    return;      
}

void CVNetLibNet::do_ack_freeresource_hc(const MessageFrame& message)
{
    CVNetFreeResourceHCMsgAck cmsg;
    cmsg.deserialize(message.data);

    MID receiver = cmsg.m_mid;
    if(receiver._application.empty())
    {
        cout<<"sender's application is null,cant ack select cc message" <<endl;
        //SkyAssert(false);
        return;
    }
    MessageOption option(receiver,EV_VNETLIB_FREERESOURCE_HC_ACK, 0, 0);

    //转发cc释放hc上资源的消息
    if (SUCCESS != mu->Send(message.data,option))
    {
        cout << "do_ack_freeresource_hc send msg to tc failed \n"<<endl;
    }
    return;      
}       

//startnetwork资源校验
void CVNetLibNet::do_startnetwork(const MessageFrame& message)
{
    if(m_tmoduleinfo.vna_application.empty())
    {
       do_get_vna_application(m_tmoduleinfo.vna_application);
       if(m_tmoduleinfo.vna_application.empty())
       {
           cout<<"in do_startnetwork vna application is null"<<endl;
           //SkyAssert(false);
           return;
       }
    }
    
    if(m_tmoduleinfo.keepalive_state == VNA_LINK_DOWN )
    {
        cout<< "do_startnetwork link is down"<<endl;

        MessageOption option(message.option.sender(),EV_VNETLIB_STARTNETWORK_ACK, 0, 0);

        CVNetStartNetworkMsgAck cackmsg;
        cackmsg.state = ERROR;
        cackmsg.detail = "the link to vna is down";
        if (SUCCESS != mu->Send(cackmsg,option))
        {
            cout << "do_startnetwork send msg to tc failed \n"<<endl;
        }
        return;
    }
    
    //转发startnetwork消息到VNA
    MID receiver(m_tmoduleinfo.vna_application,PROC_VNA, MU_VNA_CONTROLLER);
    MessageOption option(receiver,EV_VNETLIB_STARTNETWORK, 0, 0);

    CVNetStartNetworkMsg cmsg;
    cmsg.deserialize(message.data);
    cmsg.m_mid = message.option.sender();

    if(m_bOpenDbgInf ==4 )
    {
        cout<<"do_startnetwork mid application :"<<cmsg.m_mid._application;
        cout<<"do_startnetwork mid process :"<<cmsg.m_mid._process;
        cout<<"do_startnetwork mid unit :"<<cmsg.m_mid._unit;
    }

    if (SUCCESS != mu->Send(cmsg,option))
    {
        cout << "send EV_VNETLIB_STARTNETWORK message to VNA failed \n"<<endl;
    }
    return;      
}

//startnetwork资源校验的应答
void CVNetLibNet::do_ack_startnetwork(const MessageFrame& message)
{
    CVNetStartNetworkMsgAck cmsg;

    cmsg.deserialize(message.data);
    MID receiver = cmsg.m_mid;

    if(receiver._application.empty())
    {
       cout<<"in do_ack_startnetwork receiver application is null"<<endl;
       //SkyAssert(false);
       return;
    }
    if(m_bOpenDbgInf ==4 )
    {
        cout<<"mid application : "<<receiver._application;
        cout<<"mid _process : "<<receiver._process;
        cout<<"mid _unit : "<<receiver._unit;
    }
   
    if(m_tmoduleinfo.keepalive_state == VNA_LINK_DOWN )
    {
        cout<< "do_ack_startnetwork link is down"<<endl;
        MessageOption option(receiver,EV_VNETLIB_STARTNETWORK_ACK, 0, 0);

        CVNetStartNetworkMsgAck cackmsg;
        cackmsg.state = ERROR;
        cackmsg.detail = "the link to vna is down";
        if (SUCCESS != mu->Send(cackmsg,option))
        {
            cout << "do_startnetwork send msg to tc failed \n"<<endl;
        }
        return;
    }
           
    //转发startnetwork ack消息到HC
    MessageOption option(receiver,EV_VNETLIB_STARTNETWORK_ACK, 0, 0);

    if (SUCCESS != mu->Send(message.data,option))
    {
        cout << "send EV_VNETLIB_STARTNETWORK_ACK message  failed \n"<<endl;
    }
    return;  
}

//stopnetwork通知VNA回收资源
void CVNetLibNet::do_stopnetwork(const MessageFrame& message)
{
    if(m_tmoduleinfo.vna_application.empty())
    {
       do_get_vna_application(m_tmoduleinfo.vna_application);
       if(m_tmoduleinfo.vna_application.empty())
       {
           //SkyAssert(0);
           cout <<"in do_stopnetwork vna_application is null"<<endl;
           return;
       }
    }

    if(m_tmoduleinfo.keepalive_state == VNA_LINK_DOWN )
    {
        cout<< "do_stopnetwork link is down"<<endl;

        MessageOption option(message.option.sender(),EV_VNETLIB_STOPNETWORK_ACK, 0, 0);
        CVNetStopNetworkMsgAck cackmsg;
        cackmsg.state = ERROR;
        cackmsg.detail = "the link to vna is down";
        if (SUCCESS != mu->Send(cackmsg,option))
        {
            cout << "do_startnetwork send msg to tc failed \n"<<endl;
        }
        return;
    }
    
    //转发stopnetwork消息到VNA
    MID receiver(m_tmoduleinfo.vna_application,PROC_VNA, MU_VNA_CONTROLLER);
    MessageOption option(receiver,EV_VNETLIB_STOPNETWORK, 0, 0);

    CVNetStopNetworkMsg cmsg;
    cmsg.deserialize(message.data);
    cmsg.m_mid = message.option.sender();

    if (SUCCESS != mu->Send(cmsg,option))
    {
        cout << "send EV_VNETLIB_STOPNETWORK message to VNA failed \n"<<endl;
    }
    return;      
}

//stopnetwork资源校验的应答
void CVNetLibNet::do_ack_stopnetwork(const MessageFrame& message)
{
    CVNetStopNetworkMsgAck cmsg;
    cmsg.deserialize(message.data);
    
    MID receiver = cmsg.m_mid;
    if(receiver._application.empty())
    {
        cout<<"sender's application is null,cant ack stopnetwork message" <<endl;
        //SkyAssert(false);
        return;
    }

    //删除保存的虚拟网卡信息
    int64 delete_vmid = cmsg.m_vmid;
    m_vec_allvnics.DeleteVSInfo(delete_vmid);
   
    if(m_tmoduleinfo.keepalive_state == VNA_LINK_DOWN )
    {
        cout<< "do_ack_stopnetwork link is down"<<endl;
        MessageOption option(receiver,EV_VNETLIB_STOPNETWORK_ACK, 0, 0);

        cmsg.state = ERROR;
        cmsg.detail = "the link to vna is down";
        if (SUCCESS != mu->Send(cmsg,option))
        {
            cout << "do_startnetwork send msg to tc failed \n"<<endl;
        }
        return;
    }
    
    //转发stponetworkack消息到vna
    MessageOption option(receiver,EV_VNETLIB_STOPNETWORK_ACK, 0, 0);

    if (SUCCESS != mu->Send(message.data,option))
    {
        cout << "send EV_VNETLIB_STOPNETWORK_ACK message  failed \n"<<endl;
    }
    return;  
}

//通知VNA  domid
void CVNetLibNet::do_notify_domid(const MessageFrame& message)
{
    int32 needpusuback=1;
    
    if(m_tmoduleinfo.vna_application.empty())
    {
       do_get_vna_application(m_tmoduleinfo.vna_application);
       if(m_tmoduleinfo.vna_application.empty())
       {
           //SkyAssert(0);
           cout <<"in do_notify_domid vna_application is null"<<endl;
           return;
       }
    }
    CVNetNotifyDomIDMsg cmsg;

    cmsg.deserialize(message.data);
    MID receiver = message.option.sender();
    if(receiver._application.empty())
    {
       cout<<"in do_notify_domid receiver application is null"<<endl;
       //SkyAssert(false);
       return;
    }

    //保存下网卡信息
    CVNetVMVSIMsg cnic;
    cnic.m_domain_id = cmsg.m_dom_id;
    cnic.m_project_id = cmsg.m_project_id;
    cnic.m_vm_id = cmsg.m_vm_id;
    cnic.m_vnics = cmsg.m_VmNicCfgList;

    vector<CVNetVMVSIMsg>::iterator itorvsi=m_vec_allvnics.m_allvnics.begin();
    for(;itorvsi!=m_vec_allvnics.m_allvnics.end();itorvsi++)
    {
        if(itorvsi->m_vm_id == cmsg.m_vm_id)
        {
            itorvsi->m_domain_id = cmsg.m_dom_id;
            itorvsi->m_project_id = cmsg.m_project_id;
            itorvsi->m_vnics = cmsg.m_VmNicCfgList;
            needpusuback =0;
            break;
        }
    }
    
    if(needpusuback == 1)
    {
        vector<CVNetVnicConfig>::iterator itor;
        for(itor=cnic.m_vnics.begin();itor!=cnic.m_vnics.end();)
        {                                  
            if(itor->m_strVSIProfileID == string(STR_VSIID_INIT_VALUE))
            {
                cout <<"remove this watch dog vnic"<<endl;
                itor = cnic.m_vnics.erase(itor);
            }
            else
            {
                itor++;
            }
        }
        m_vec_allvnics.m_allvnics.push_back(cnic);
    }
    
    if(m_tmoduleinfo.keepalive_state == VNA_LINK_DOWN )
    {
        cout<< "do_notify_domid link is down"<<endl;

        MessageOption option(receiver,EV_VNETLIB_NOTIFY_DOMID, 0, 0);

        CVNetNotifyDomIDMsgAck cackmsg;
        cackmsg.state = ERROR;
        cackmsg.detail = "the link to vna is down";
        if (SUCCESS != mu->Send(cackmsg,option))
        {
            cout << "do_startnetwork send msg to tc failed \n"<<endl;
        }
        return;
    }
        
    //转发EV_VNETLIB_NOTIFY_DOMID消息到VNA
    MID receiver2(m_tmoduleinfo.vna_application,PROC_VNA, MU_VNA_CONTROLLER);
    MessageOption option(receiver2,EV_VNETLIB_NOTIFY_DOMID, 0, 0);

    CVNetNotifyDomIDMsg csendmsg;
    csendmsg.deserialize(message.data);
    csendmsg.m_mid = receiver;
    
    if (SUCCESS != mu->Send(csendmsg,option))
    {
        cout << "send EV_VNETLIB_NOTIFY_DOMID message to VNA failed \n"<<endl;
    }
    return;      
}

void CVNetLibNet::do_ack_notify_domid(const MessageFrame& message)
{
    CVNetNotifyDomIDMsgAck cmsg;

    cmsg.deserialize(message.data);
    MID receiver = cmsg.m_mid;

    if(receiver._application.empty())
    {
       cout<<"in do_ack_notify_domid receiver application is null"<<endl;
       //SkyAssert(0);
       return;
    }

    if(m_tmoduleinfo.keepalive_state == VNA_LINK_DOWN )
    {
        cout<< "do_notify_domid link is down"<<endl;

        MessageOption option(receiver,EV_VNETLIB_NOTIFY_DOMID_ACK, 0, 0);
        CVNetNotifyDomIDMsgAck cackmsg;
        cackmsg.state = ERROR;
        cackmsg.detail ="the link to vna is down";
        if (SUCCESS != mu->Send(cackmsg,option))
        {
            cout << "do_ack_notify_domid send msg to tc failed \n"<<endl;
        }
        return;
    }
    //转发EV_VNETLIB_NOTIFY_DOMID_ACK消息到hc
    MessageOption option(receiver,EV_VNETLIB_NOTIFY_DOMID_ACK, 0, 0);
    
    if (SUCCESS != mu->Send(message.data,option))
    {
        cout << "send EV_VNETLIB_NOTIFY_DOMID_ACK message  failed \n"<<endl;
    }
    return;  
}


int32 CVNetLibNet::do_query_vnics(const MessageFrame& message)
{
    //此时还没有获取到vnm的application,和AB模型无关，因为不涉及申请资源,所以直接给TC返回失败,
    if(m_tmoduleinfo.vnm_application.empty())
    {
        cout<< "do_query_vnics vnm app is empty"<<endl;
        //返回 计算的工作流会跟催
        return ERROR;
    }
    CQueryVmNicInfoMsg cmsg = CQueryVmNicInfoMsg(0) ;
    cmsg.deserialize(message.data);
    cmsg.m_mid = message.option.sender();

    //转发EV_VNETLIB_QUERY_VNICS消息到VNM
    MID receiver(m_tmoduleinfo.vnm_application,PROC_VNM, MU_VNM);
    MessageOption option(receiver,EV_VNETLIB_QUERY_VNICS, 0, 0);

    if (SUCCESS != mu->Send(cmsg,option))
    {
        cout << "send EV_VNETLIB_QUERY_VNICS message  failed \n"<<endl;
    }
    return SUCCESS;
}


int32 CVNetLibNet::do_ack_query_vnics(const MessageFrame& message)
{
    CQueryVmNicInfoMsgAck cmsg;

    cmsg.deserialize(message.data);
    MID receiver = cmsg.m_mid;

    if(receiver._application.empty())
    {
       cout<<"do_ack_query_vnics receiver application is null"<<endl;
       //SkyAssert(0);
       return ERROR;
    }
    
    //转发EV_VNETLIB_QUERY_VNICS_ACK消息到tc
    MessageOption option(receiver,EV_VNETLIB_QUERY_VNICS_ACK, 0, 0);
    
    if (SUCCESS != mu->Send(message.data,option))
    {
        cout << "send EV_VNETLIB_QUERY_VNICS_ACK message  failed \n"<<endl;
    }
    return SUCCESS;  
}


/**********************热迁移接口************************************/
int32 CVNetLibNet::do_vm_migrate_begin(const MessageFrame& message)
{
    //此时还没有获取到vnm的application,和AB模型无关，因为不涉及申请资源,所以直接给TC返回失败,
    if(m_tmoduleinfo.vnm_application.empty())
    {
        cout<< "do_vm_migrate_begin"<<endl;
        //返回 计算的工作流会跟催
        return ERROR;
    }
    CVNetVmMigrateBeginMsg cmsg ;
    cmsg.deserialize(message.data);
    cmsg.m_mid = message.option.sender();

    //转发EV_VNETLIB_VM_MIGRATE_BEGIN消息到VNM
    MID receiver(m_tmoduleinfo.vnm_application,PROC_VNM, MU_VNM);
    MessageOption option(receiver,EV_VNETLIB_VM_MIGRATE_BEGIN, 0, 0);

    if (SUCCESS != mu->Send(cmsg,option))
    {
        cout << "send EV_VNETLIB_VM_MIGRATE_BEGIN message  failed \n"<<endl;
    }
    return SUCCESS;
}


int32 CVNetLibNet::do_vm_migrate_begin_ack(const MessageFrame& message)
{
    CVNetVmMigrateMsgAck cmsg;
    cmsg.deserialize(message.data);
    
    MID receiver = cmsg.m_mid;
    if(receiver._application.empty())
    {
        cout<<"sender's application is null,cant ack EV_VNETLIB_VM_MIGRATE_BEGIN_ACK message" <<endl;
        //SkyAssert(false);
        return ERROR;
    }

    //转发EV_VNETLIB_VM_MIGRATE_BEGIN_ACK消息到cc
    MessageOption option(receiver,EV_VNETLIB_VM_MIGRATE_BEGIN_ACK, 0, 0);

    if (SUCCESS != mu->Send(message.data,option))
    {
        cout << "send EV_VNETLIB_VM_MIGRATE_BEGIN_ACK message  failed \n"<<endl;
    }
    return SUCCESS;  
}


int32 CVNetLibNet::do_vm_migrate_end(const MessageFrame& message)
{
    //此时还没有获取到vnm的application,和AB模型无关，因为不涉及申请资源,所以直接给TC返回失败,
    if(m_tmoduleinfo.vnm_application.empty())
    {
        cout<< "do_vm_migrate_end"<<endl;
        //返回 计算的工作流会跟催
        return ERROR;
    }
    CVNetVmMigrateEndMsg cmsg ;
    cmsg.deserialize(message.data);
    cmsg.m_mid = message.option.sender();

    //转发EV_VNETLIB_VM_MIGRATE_END消息到VNM
    MID receiver(m_tmoduleinfo.vnm_application,PROC_VNM, MU_VNM);
    MessageOption option(receiver,EV_VNETLIB_VM_MIGRATE_END, 0, 0);

    if (SUCCESS != mu->Send(cmsg,option))
    {
        cout << "send EV_VNETLIB_VM_MIGRATE_END message  failed \n"<<endl;
    }
    return SUCCESS;
}


int32 CVNetLibNet::do_vm_migrate_end_ack(const MessageFrame& message)
{
    CVNetVmMigrateMsgAck cmsg;
    cmsg.deserialize(message.data);
    
    MID receiver = cmsg.m_mid;
    if(receiver._application.empty())
    {
        cout<<"sender's application is null,cant ack EV_VNETLIB_VM_MIGRATE_END_ACK message" <<endl;
        //SkyAssert(false);
        return ERROR;
    }

    //转发EV_VNETLIB_VM_MIGRATE_END_ACK消息到vnm
    MessageOption option(receiver,EV_VNETLIB_VM_MIGRATE_END_ACK, 0, 0);

    if (SUCCESS != mu->Send(message.data,option))
    {
        cout << "send EV_VNETLIB_VM_MIGRATE_END_ACK message  failed \n"<<endl;
    }
    return SUCCESS;  
}


int32 CVNetLibNet::do_select_migrate_hc(const MessageFrame& message)
{
    //此时还没有获取到vnm的application,和AB模型无关，因为不涉及申请资源,所以直接给TC返回失败,
    if(m_tmoduleinfo.vnm_application.empty())
    {
        cout<< "do_vm_migrate_begin"<<endl;
        //返回 计算的工作流会跟催
        return ERROR;
    }
    CVNetSelectHCListMsg cmsg ;
    cmsg.deserialize(message.data);
    cmsg.m_mid = message.option.sender();

    //转发EV_VNETLIB_SELECT_MIGRATE_HC消息到VNM
    MID receiver(m_tmoduleinfo.vnm_application,PROC_VNM, MU_VNM);
    MessageOption option(receiver,EV_VNETLIB_SELECT_MIGRATE_HC, 0, 0);

    if (SUCCESS != mu->Send(cmsg,option))
    {
        cout << "send EV_VNETLIB_SELECT_MIGRATE_HC message  failed \n"<<endl;
    }
    return SUCCESS;
}


int32 CVNetLibNet::do_select_migrate_hc_ack(const MessageFrame& message)
{
    CVNetSelectHCListMsgAck  cmsg;
    cmsg.deserialize(message.data);
    
    MID receiver = cmsg.m_mid;
    if(receiver._application.empty())
    {
        cout<<"sender's application is null,cant ack EV_VNETLIB_SELECT_MIGRATE_HC_ACK message" <<endl;
        //SkyAssert(false);
        return ERROR;
    }

    //转发EV_VNETLIB_SELECT_MIGRATE_HC_ACK消息到vna
    MessageOption option(receiver,EV_VNETLIB_SELECT_MIGRATE_HC_ACK, 0, 0);

    if (SUCCESS != mu->Send(message.data,option))
    {
        cout << "send EV_VNETLIB_SELECT_MIGRATE_HC_ACK message  failed \n"<<endl;
    }
    return SUCCESS;  
}


int32  CVNetLibNet::do_cfg_netplane(CNetplaneMsg& netplane_msg)
{
    //此时还没有获取到vnm的application,和AB模型无关，因为不涉及申请资源,所以直接给TC返回失败,
    if(m_tmoduleinfo.vnm_application.empty())
    {
        cout<< "do_cfg_netplane vnm app is empty"<<endl;

        netplane_msg.SetResult(VNET_XMLRPC_ERROR);
        netplane_msg.SetResultinfo("vnm application is null");
        return ERROR;
    }

    //将消息发到VNM
    WAITE_MSG_ACK(netplane_msg, EV_VNETLIB_NETPLANE_CFG, TEMP_MU_CFG_NETPLANE)
    {
        CNetplaneMsg cAckMsg;
        cAckMsg.deserialize(message.data);
        netplane_msg.SetResult(cAckMsg.GetResult());
        netplane_msg.SetResultinfo(cAckMsg.GetResultinfo());
        netplane_msg.m_strUUID = cAckMsg.m_strUUID;
        
        if(m_bOpenDbgInf == 3)
        {
            cout<<"do_cfg_netplane result : "<<netplane_msg.GetResult()<<endl;
            cout<<"do_cfg_netplane resultInfo : "<<netplane_msg.GetResultinfo()<<endl;
        }
        return SUCCESS;
    }
    else
    {
        netplane_msg.SetResult(VNET_XMLRPC_TIMEOUT);
        netplane_msg.SetResultinfo("mu->Wait timeout");
        if(m_bOpenDbgInf == 3)
        {
            cout<<"do_cfg_netplane result : "<<netplane_msg.GetResult()<<endl;
            cout<<"do_cfg_netplane resultInfo : "<<netplane_msg.GetResultinfo()<<endl;
        }
        return ERROR;
    }
}

int32  CVNetLibNet::do_cfg_netplaneIP(CNetplaneIPv4RangeMsg& netplane_msg)
{
 
    //此时还没有获取到vnm的application,和AB模型无关，因为不涉及申请资源,所以直接给TC返回失败,
    if(m_tmoduleinfo.vnm_application.empty())
    {
        cout<< "do_cfg_netplaneIP vnm app is empty"<<endl;

        netplane_msg.SetResult(VNET_XMLRPC_ERROR);
        netplane_msg.SetResultinfo("do_cfg_netplaneIP vnm app is empty");
        return ERROR;
    }

    //将消息发到VNM
    WAITE_MSG_ACK(netplane_msg, EV_VNETLIB_NP_IPV4_RANGE_CFG, TEMP_MU_CFG_NETPLANE_IP) 
    {
        CNetplaneIPv4RangeMsg cAckMsg;
        cAckMsg.deserialize(message.data);
        netplane_msg.SetResult(cAckMsg.GetResult());
        netplane_msg.SetResultinfo(cAckMsg.GetResultinfo());
        netplane_msg.m_strNetplaneUUID = cAckMsg.m_strNetplaneUUID;

        if(m_bOpenDbgInf == 3)
        {
            cout<<"do_cfg_netplaneIP result : "<<netplane_msg.GetResult()<<endl;
            cout<<"do_cfg_netplaneIP resultInfo : "<<netplane_msg.GetResultinfo()<<endl;
        }
        return SUCCESS;
    }
    else
    {
        netplane_msg.SetResult(VNET_XMLRPC_TIMEOUT);
        netplane_msg.SetResultinfo("mu->Wait timeout");

        if(m_bOpenDbgInf == 3)
        {
            cout<<"do_cfg_netplaneIP result : "<<netplane_msg.GetResult()<<endl;
            cout<<"do_cfg_netplaneIP resultInfo : "<<netplane_msg.GetResultinfo()<<endl;
        }
        return ERROR;
    }
}

int32  CVNetLibNet::do_cfg_netplaneMAC(CMACRangeMsg& netplane_msg)
{
    //此时还没有获取到vnm的application,和AB模型无关，因为不涉及申请资源,所以直接给TC返回失败,
    if(m_tmoduleinfo.vnm_application.empty())
    {
        cout<< "do_cfg_netplaneMAC vnm app is empty"<<endl;

        netplane_msg.SetResult(VNET_XMLRPC_ERROR);
        netplane_msg.SetResultinfo("do_cfg_netplaneMAC vnm app is empty");
        return ERROR;
    }

    //将消息发到VNM
    WAITE_MSG_ACK(netplane_msg, EV_VNETLIB_MAC_RANGE_CFG, TEMP_MU_CFG_NETPLANE_MAC) 
    {
        CMACRangeMsg cAckMsg;
        cAckMsg.deserialize(message.data);
        netplane_msg.SetResult(cAckMsg.GetResult());
        netplane_msg.SetResultinfo(cAckMsg.GetResultinfo());
        netplane_msg.m_strNetplaneUUID = cAckMsg.m_strNetplaneUUID;

        if(m_bOpenDbgInf == 3)
        {
            cout<<"do_cfg_netplaneMAC result : "<<netplane_msg.GetResult()<<endl;
            cout<<"do_cfg_netplaneMAC resultInfo : "<<netplane_msg.GetResultinfo()<<endl;
        }
        return SUCCESS;
    }
    else
    {
        netplane_msg.SetResult(VNET_XMLRPC_TIMEOUT);
        netplane_msg.SetResultinfo("mu->Wait timeout!");

        if(m_bOpenDbgInf == 3)
        {
            cout<<"do_cfg_netplaneMAC result : "<<netplane_msg.GetResult()<<endl;
            cout<<"do_cfg_netplaneMAC resultInfo : "<<netplane_msg.GetResultinfo()<<endl;
        }
        
        return ERROR;
    }
}

int32  CVNetLibNet::do_cfg_netplaneVLAN(CVlanRangeMsg& netplane_msg)
{
    //此时还没有获取到vnm的application,和AB模型无关，因为不涉及申请资源,所以直接给TC返回失败,
    if(m_tmoduleinfo.vnm_application.empty())
    {
        cout<< "do_cfg_netplaneVLAN vnm app is empty"<<endl;

        netplane_msg.SetResult(VNET_XMLRPC_ERROR);
        netplane_msg.SetResultinfo("do_cfg_netplaneVLAN vnm app is empty");
        return ERROR;
    }

    //将消息发到VNM
    WAITE_MSG_ACK(netplane_msg, EV_VNETLIB_VLAN_RANGE_CFG, TEMP_MU_CFG_NETPLANE_VLAN) 
    {
        CVlanRangeMsg cAckMsg;
        cAckMsg.deserialize(message.data);
        netplane_msg.SetResult(cAckMsg.GetResult());
        netplane_msg.SetResultinfo(cAckMsg.GetResultinfo());
        netplane_msg.m_strNetplaneUUID = cAckMsg.m_strNetplaneUUID;

        if(m_bOpenDbgInf == 3)
        {
            cout<<"do_cfg_netplaneVLAN result : "<<netplane_msg.GetResult()<<endl;
            cout<<"do_cfg_netplaneVLAN resultInfo : "<<netplane_msg.GetResultinfo()<<endl;
        }
        return SUCCESS;
    }
    else
    {
        netplane_msg.SetResult(VNET_XMLRPC_TIMEOUT);
        netplane_msg.SetResultinfo("mu->Wait timeout");

        if(m_bOpenDbgInf == 3)
        {
            cout<<"do_cfg_netplaneVLAN result : "<<netplane_msg.GetResult()<<endl;
            cout<<"do_cfg_netplaneVLAN resultInfo : "<<netplane_msg.GetResultinfo()<<endl;
        }
        return ERROR;
    }
}

int32  CVNetLibNet::do_cfg_netplaneSEGMENT(CSegmentRangeMsg& netplane_msg)
{
    //此时还没有获取到vnm的application,和AB模型无关，因为不涉及申请资源,所以直接给TC返回失败,
    if(m_tmoduleinfo.vnm_application.empty())
    {
        cout<< "do_cfg_netplaneSEGMENT vnm app is empty"<<endl;

        netplane_msg.SetResult(VNET_XMLRPC_ERROR);
        netplane_msg.SetResultinfo("do_cfg_netplaneSEGMENT vnm app is empty");
        return ERROR;
    }

    //将消息发到VNM
    WAITE_MSG_ACK(netplane_msg, EV_VNETLIB_SEGMENT_RANGE_CFG, TEMP_MU_CFG_NETPLANE_SEGMENT) 
    {
        CSegmentRangeMsg cAckMsg;
        cAckMsg.deserialize(message.data);
        netplane_msg.SetResult(cAckMsg.GetResult());
        netplane_msg.SetResultinfo(cAckMsg.GetResultinfo());
        netplane_msg.m_strNetplaneUUID = cAckMsg.m_strNetplaneUUID;

        if(m_bOpenDbgInf == 3)
        {
            cout<<"do_cfg_netplaneSEGMENT result : "<<netplane_msg.GetResult()<<endl;
            cout<<"do_cfg_netplaneSEGMENT resultInfo : "<<netplane_msg.GetResultinfo()<<endl;
        }
        return SUCCESS;
    }
    else
    {
        netplane_msg.SetResult(VNET_XMLRPC_TIMEOUT);
        netplane_msg.SetResultinfo("mu->Wait timeout");

        if(m_bOpenDbgInf == 3)
        {
            cout<<"do_cfg_netplaneSEGMENT result : "<<netplane_msg.GetResult()<<endl;
            cout<<"do_cfg_netplaneSEGMENT resultInfo : "<<netplane_msg.GetResultinfo()<<endl;
        }
        return ERROR;
    }
}

int32 CVNetLibNet::do_cfg_portgroup(CPortGroupMsg& cfgmessage)
{
    //此时还没有获取到vnm的application,和AB模型无关，因为不涉及申请资源,所以直接给TC返回失败,
    if(m_tmoduleinfo.vnm_application.empty())
    {
        cout<< "do_cfg_portgroup vnm app is empty"<<endl;

        cfgmessage.SetResult(VNET_XMLRPC_ERROR);
        cfgmessage.SetResultinfo("vnm application is null");
        return ERROR;
    }

    //将消息发到VNM
    WAITE_MSG_ACK(cfgmessage, EV_VNETLIB_PORT_GROUP_CFG, TEMP_MU_CFG_PORTGROUP
) 
    {
        CPortGroupMsg cAckMsg;
        cAckMsg.deserialize(message.data);
        cfgmessage.SetResult(cAckMsg.GetResult());
        cfgmessage.SetResultinfo(cAckMsg.GetResultinfo());
        cfgmessage.m_strUUID = cAckMsg.m_strUUID;

        if(m_bOpenDbgInf == 3)
        {
            cout<<"do_cfg_portgroup result : "<<cfgmessage.GetResult()<<endl;
            cout<<"do_cfg_portgroup resultInfo : "<<cfgmessage.GetResultinfo()<<endl;
        }
        return SUCCESS;
    }
    else
    {
        cfgmessage.SetResult(VNET_XMLRPC_TIMEOUT);
        cfgmessage.SetResultinfo("mu->Wait timeout");

        if(m_bOpenDbgInf == 3)
        {
            cout<<"do_cfg_portgroup result : "<<cfgmessage.GetResult()<<endl;
            cout<<"do_cfg_portgroup resultInfo : "<<cfgmessage.GetResultinfo()<<endl;
        }
        return ERROR;
    }
}

int32 CVNetLibNet::do_cfg_portgroupVlan(CPGTrunkVlanRangeMsg& cfgmessage)
{
    //此时还没有获取到vnm的application,和AB模型无关，因为不涉及申请资源,所以直接给TC返回失败,
    if(m_tmoduleinfo.vnm_application.empty())
    {
        cout<< "do_cfg_portgroupVlan vnm app is empty"<<endl;
        cfgmessage.SetResult(VNET_XMLRPC_ERROR); 
        cfgmessage.SetResultinfo("vnm application is null");
        return ERROR;
    }

    //将消息发到VNM
    WAITE_MSG_ACK(cfgmessage, EV_VNETLIB_PG_TRUNK_VLANRANGE_CFG, TEMP_MU_CFG_PORTGROUP_VLAN) 
    {
        CPGTrunkVlanRangeMsg cAckMsg;
        cAckMsg.deserialize(message.data);
        cfgmessage.SetResult(cAckMsg.GetResult());
        cfgmessage.SetResultinfo(cAckMsg.GetResultinfo());
        cfgmessage.m_strPGUuid= cAckMsg.m_strPGUuid;

        if(m_bOpenDbgInf == 3)
        {
            cout<<"do_cfg_portgroupVlan result : "<<cfgmessage.GetResult()<<endl;
            cout<<"do_cfg_portgroupVlan resultInfo : "<<cfgmessage.GetResultinfo()<<endl;
        }
        return SUCCESS;
    }
    else
    {
        cfgmessage.SetResult(VNET_XMLRPC_TIMEOUT);
        cfgmessage.SetResultinfo("mu->Wait timeout");

        if(m_bOpenDbgInf == 3)
        {
            cout<<"do_cfg_portgroupVlan result : "<<cfgmessage.GetResult()<<endl;
            cout<<"do_cfg_portgroupVlan resultInfo : "<<cfgmessage.GetResultinfo()<<endl;
        }
        return ERROR;
    }
}

int32 CVNetLibNet::do_cfg_logicnet(CLogicNetworkMsg& cfgmessage)
{
    //此时还没有获取到vnm的application,和AB模型无关，因为不涉及申请资源,所以直接给TC返回失败,
    if(m_tmoduleinfo.vnm_application.empty())
    {
        cout<< "do_cfg_logicnet vnm app is empty"<<endl;
        cfgmessage.SetResult(VNET_XMLRPC_ERROR); 
        cfgmessage.SetResultinfo("do_cfg_logicnet vnm app is empty"); 
        return ERROR;
    }

    //将消息发到VNM
    WAITE_MSG_ACK(cfgmessage, EV_VNETLIB_LOGIC_NETWORK_CFG, TEMP_MU_CFG_LOGICNET) 
    {
        CLogicNetworkMsg cAckMsg;
        cAckMsg.deserialize(message.data);
        cfgmessage.SetResult(cAckMsg.GetResult());
        cfgmessage.SetResultinfo(cAckMsg.GetResultinfo());
        cfgmessage.m_strUUID= cAckMsg.m_strUUID;

        if(m_bOpenDbgInf == 3)
        {
            cout<<"do_cfg_logicnet result : "<<cfgmessage.GetResult()<<endl;
            cout<<"do_cfg_logicnet resultInfo : "<<cfgmessage.GetResultinfo()<<endl;
        }
        return SUCCESS;
    }
    else
    {
        cfgmessage.SetResult(VNET_XMLRPC_TIMEOUT);
        cfgmessage.SetResultinfo("mu->Wait timeout!");

        if(m_bOpenDbgInf == 3)
        {
            cout<<"do_cfg_logicnet result : "<<cfgmessage.GetResult()<<endl;
            cout<<"do_cfg_logicnet resultInfo : "<<cfgmessage.GetResultinfo()<<endl;
        }
        return ERROR;
    }
}


int32 CVNetLibNet::do_cfg_logicnet_ip(CLogicNetworkIPv4RangeMsg& cfgmessage)
{
    //此时还没有获取到vnm的application,和AB模型无关，因为不涉及申请资源,所以直接给TC返回失败,
    if(m_tmoduleinfo.vnm_application.empty())
    {
        cout<< "do_cfg_logicnet_ip vnm app is empty"<<endl;
        cfgmessage.SetResult(VNET_XMLRPC_ERROR); 
        cfgmessage.SetResultinfo("do_cfg_logicnet_ip vnm app is empty"); 
        return ERROR;
    }

    //将消息发到VNM
    WAITE_MSG_ACK(cfgmessage, EV_VNETLIB_LOGI_NET_IPV4_RANGE_CFG, TEMP_MU_CFG_LOGICNET_IP)
    {
        CLogicNetworkIPv4RangeMsg cAckMsg;
        cAckMsg.deserialize(message.data);
        cfgmessage.SetResult(cAckMsg.GetResult());
        cfgmessage.SetResultinfo(cAckMsg.GetResultinfo());
        cfgmessage.m_strLogicNetworkUUID = cAckMsg.m_strLogicNetworkUUID;

        if(m_bOpenDbgInf == 3)
        {
            cout<<"do_cfg_logicnet_ip result : "<<cfgmessage.GetResult()<<endl;
            cout<<"do_cfg_logicnet_ip resultInfo : "<<cfgmessage.GetResultinfo()<<endl;
        }
        return SUCCESS;
    }
    else
    {
        cfgmessage.SetResult(VNET_XMLRPC_TIMEOUT);
        cfgmessage.SetResultinfo("mu->Wait timeout!");

        if(m_bOpenDbgInf == 3)
        {
            cout<<"do_cfg_logicnet_ip result : "<<cfgmessage.GetResult()<<endl;
            cout<<"do_cfg_logicnet_ip resultInfo : "<<cfgmessage.GetResultinfo()<<endl;
        }        
        return ERROR;
    }
}

//配置交换
int32 CVNetLibNet::do_cfg_switch_base(const CSwitchCfgMsg &cfgmessage,CSwitchCfgAckMsg &ackmesg)
{
    //此时还没有获取到vnm的application,和AB模型无关，因为不涉及申请资源,所以直接给TC返回失败,
    if(m_tmoduleinfo.vnm_application.empty())
    {
        cout<< "do_cfg_switch_base vnm app is empty"<<endl;
        ackmesg.SetResult(VNET_XMLRPC_ERROR); 
        ackmesg.SetResultinfo("do_cfg_switch_base vnm app is empty"); 
        return ERROR;
    }

    //将消息发到VNM
    WAITE_MSG_ACK_TIMES(cfgmessage, EV_VNETLIB_SWITCH_CFG, TEMP_MU_CFG_SWITCH_BASE,30000)
    {
        CSwitchCfgAckMsg cAckMsg;
        cAckMsg.deserialize(message.data);
        ackmesg.SetResult(cAckMsg.GetResult());
        ackmesg.SetResultinfo(cAckMsg.GetResultinfo());
        ackmesg.m_strUuid= cAckMsg.m_strUuid;
        ackmesg.m_nProgress = cAckMsg.m_nProgress;

        if(m_bOpenDbgInf == 3)
        {
            cout<<"do_cfg_switch_base result : "<<ackmesg.GetResult()<<endl;
            cout<<"do_cfg_switch_base resultInfo : "<<ackmesg.GetResultinfo()<<endl;
        }
        return SUCCESS;
    }
    else
    {
        ackmesg.SetResult(VNET_XMLRPC_TIMEOUT);
        ackmesg.SetResultinfo("mu->Wait timeout!");

        if(m_bOpenDbgInf == 3)
        {
            cout<<"do_cfg_switch_base result : "<<ackmesg.GetResult()<<endl;
            cout<<"do_cfg_switch_base resultInfo : "<<ackmesg.GetResultinfo()<<endl;
        }
        return ERROR;
    }
}

//配置交换端口
int32 CVNetLibNet::do_cfg_switch_port(const CSwitchPortCfgMsg &cfgmessage,
CSwitchCfgAckMsg &ackmesg)
{
    //此时还没有获取到vnm的application,和AB模型无关，因为不涉及申请资源,所以直接给TC返回失败,
    if(m_tmoduleinfo.vnm_application.empty())
    {
        cout<< "do_cfg_switch_port vnm app is empty"<<endl;
        ackmesg.SetResult(VNET_XMLRPC_ERROR); 
        ackmesg.SetResultinfo("do_cfg_switch_port vnm app is empty");
        return ERROR;
    }

    //将消息发到VNM
    WAITE_MSG_ACK_TIMES(cfgmessage, EV_VNETLIB_SWITCH_PORT_CFG, TEMP_MU_CFG_SWITCH_PORT,30000)
    {
        CSwitchCfgAckMsg cAckMsg;
        cAckMsg.deserialize(message.data);
        ackmesg.SetResult(cAckMsg.GetResult());
        ackmesg.SetResultinfo(cAckMsg.GetResultinfo());
        ackmesg.m_strUuid= cAckMsg.m_strUuid;
        ackmesg.m_nProgress = cAckMsg.m_nProgress;

        if(m_bOpenDbgInf == 3)
        {
            cout<<"do_cfg_switch_port result : "<<ackmesg.GetResult()<<endl;
            cout<<"do_cfg_switch_port resultInfo : "<<ackmesg.GetResultinfo()<<endl;
        }
        return SUCCESS;
    }
    else
    {
        ackmesg.SetResult(VNET_XMLRPC_TIMEOUT);
        ackmesg.SetResultinfo("mu->Wait timeout!");

        if(m_bOpenDbgInf == 3)
        {
            cout<<"do_cfg_switch_port result : "<<ackmesg.GetResult()<<endl;
            cout<<"do_cfg_switch_port resultInfo : "<<ackmesg.GetResultinfo()<<endl;
        }
        return ERROR;
    }
}

int32 CVNetLibNet::do_cfg_sdn(CSdnCfgMsg& cfgmessage)
{
    //此时还没有获取到vnm的application,和AB模型无关，因为不涉及申请资源,所以直接给TC返回失败,
    if(m_tmoduleinfo.vnm_application.empty())
    {
        cout<< "do_cfg_sdn vnm app is empty"<<endl;
        cfgmessage.SetResult(VNET_XMLRPC_ERROR); 
        cfgmessage.SetResultinfo("do_cfg_sdn vnm app is empty"); 
        return ERROR;
    }

    //将消息发到VNM
    WAITE_MSG_ACK(cfgmessage, EV_VNETLIB_SDN_CFG, TEMP_MU_CFG_SDN) 
    {
        CSdnCfgMsg cAckMsg;
        cAckMsg.deserialize(message.data);
        cfgmessage.SetResult(cAckMsg.GetResult());
        cfgmessage.SetResultinfo(cAckMsg.GetResultinfo());
        cfgmessage.m_strSdnUuid = cAckMsg.m_strSdnUuid;

        if(m_bOpenDbgInf == 3)
        {
            cout<<"do_cfg_sdn result : "<<cfgmessage.GetResult()<<endl;
            cout<<"do_cfg_sdn resultInfo : "<<cfgmessage.GetResultinfo()<<endl;
        }
        return SUCCESS;
    }
    else
    {
        cfgmessage.SetResult(VNET_XMLRPC_TIMEOUT);
        cfgmessage.SetResultinfo("mu->Wait timeout!");

        if(m_bOpenDbgInf == 3)
        {
            cout<<"do_cfg_sdn result : "<<cfgmessage.GetResult()<<endl;
            cout<<"do_cfg_sdn resultInfo : "<<cfgmessage.GetResultinfo()<<endl;
        }
        return ERROR;
    }
}

int32 CVNetLibNet::do_cfg_kernal_port(CKerNalPortMsg& cfgmessage)
{
    //此时还没有获取到vnm的application,和AB模型无关，因为不涉及申请资源,所以直接给TC返回失败,
    if(m_tmoduleinfo.vnm_application.empty())
    {
        cout<< "do_cfg_kernal_port vnm app is empty"<<endl;
        cfgmessage.SetResult(VNET_XMLRPC_ERROR); 
        cfgmessage.SetResultinfo("do_cfg_kernal_port vnm app is empty");
        return ERROR;
    }

    cout <<"will send message to vnm to config kernel port!"<<endl;
    
    //将消息发到VNM
    WAITE_MSG_ACK_TIMES(cfgmessage, EV_VNETLIB_KERNALPORT_CFG, TEMP_MU_CFG_KERNA_PORT,20000)
    {
        CKerNalPortMsg cAckMsg;
        cAckMsg.deserialize(message.data);
        cfgmessage.SetResult(cAckMsg.GetResult());
        cfgmessage.SetResultinfo(cAckMsg.GetResultinfo());
        cfgmessage.m_strUUID= cAckMsg.m_strUUID;

        if(m_bOpenDbgInf == 3)
        {
            cout<<"do_cfg_kernal_port result : "<<cfgmessage.GetResult()<<endl;
            cout<<"do_cfg_kernal_port resultInfo : "<<cfgmessage.GetResultinfo()<<endl;
        }
        return SUCCESS;
    }
    else
    {
        cfgmessage.SetResult(VNET_XMLRPC_TIMEOUT);
        cfgmessage.SetResultinfo("mu->Wait timeout!");

        if(m_bOpenDbgInf == 3)
        {
            cout<<"do_cfg_kernal_port result : "<<cfgmessage.GetResult()<<endl;
            cout<<"do_cfg_kernal_port resultInfo : "<<cfgmessage.GetResultinfo()<<endl;
        }
        return ERROR;
    }
}

int32 CVNetLibNet::do_cfg_loop_port(CLoopPortMsg& cfgmessage)
{
    if(m_tmoduleinfo.vna_application.empty())
    {
       do_get_vna_application(m_tmoduleinfo.vna_application);
       if(m_tmoduleinfo.vna_application.empty())
       {
           //SkyAssert(0);
           cout <<"in do_cfg_loop_port vna_application is null"<<endl;
           return ERROR;
       }
    }

    //将消息发到VNM
    WAITE_MSG_ACK_TIMES(cfgmessage, EV_VNETLIB_CFG_LOOPBACK, TEMP_MU_CFG_LOOP_PORT,15000)
    {
        CLoopPortMsg cAckMsg;
        cAckMsg.deserialize(message.data);
        cfgmessage.SetResult(cAckMsg.GetResult());
        cfgmessage.SetResultinfo(cAckMsg.GetResultinfo());
        cfgmessage.m_strportUUID= cAckMsg.m_strportUUID;

        if(m_bOpenDbgInf == 3)
        {
            cout<<"do_cfg_loop_port result : "<<cfgmessage.GetResult()<<endl;
            cout<<"do_cfg_loop_port resultInfo : "<<cfgmessage.GetResultinfo()<<endl;
        }
        return SUCCESS;
    }
    else
    {
        cfgmessage.SetResult(VNET_XMLRPC_TIMEOUT);
        cfgmessage.SetResultinfo("mu->Wait timeout!");


        if(m_bOpenDbgInf == 3)
        {
            cout<<"do_cfg_loop_port result : "<<cfgmessage.GetResult()<<endl;
            cout<<"do_cfg_loop_port resultInfo : "<<cfgmessage.GetResultinfo()<<endl;
        }
        return ERROR;
    }
    return SUCCESS;
}

//loop口通配
int32 CVNetLibNet::do_cfg_wildcast_loop_port(CWildcastLoopbackCfgMsg& cfgmessage)
{
    if(m_tmoduleinfo.vna_application.empty())
    {
       do_get_vna_application(m_tmoduleinfo.vna_application);
       if(m_tmoduleinfo.vna_application.empty())
       {
           //SkyAssert(0);
           cout <<"in do_cfg_wildcast_loop_port vna_application is null"<<endl;
           return ERROR;
       }
    }

    //将消息发到VNM
    WAITE_MSG_ACK_TIMES(cfgmessage, EV_VNETLIB_WILDCAST_LOOPBACK_CFG, TEMP_MU_CFG_WC_LOOP_PORT,10000)
    {
        CWildcastLoopbackCfgAckMsg cAckMsg;
        cAckMsg.deserialize(message.data);
        cfgmessage.SetResult(cAckMsg.GetResult());
        cfgmessage.SetResultinfo(cAckMsg.GetResultinfo());
        cfgmessage.m_strUuid= cAckMsg.m_strUuid;

        if(m_bOpenDbgInf == 3)
        {
            cout<<"do_cfg_wildcast_loop_port result : "<<cfgmessage.GetResult()<<endl;
            cout<<"do_cfg_wildcast_loop_port resultInfo : "<<cfgmessage.GetResultinfo()<<endl;
        }
        return SUCCESS;
    }
    else
    {
        cfgmessage.SetResult(VNET_XMLRPC_TIMEOUT);
        cfgmessage.SetResultinfo("mu->Wait timeout!");

        if(m_bOpenDbgInf == 3)
        {
            cout<<"do_cfg_wildcast_loop_port result : "<<cfgmessage.GetResult()<<endl;
            cout<<"do_cfg_wildcast_loop_port resultInfo : "<<cfgmessage.GetResultinfo()<<endl;
        }
        return ERROR;
    }
    return SUCCESS;
}

//交换基本信息通配
int32 CVNetLibNet::do_cfg_wildcast_switch_base(const CWildcastSwitchCfgMsg &cfgmessage,
                                              CWildcastSwitchCfgAckMsg &ackmesg)
{
        //此时还没有获取到vnm的application,和AB模型无关，因为不涉及申请资源,所以直接给TC返回失败,
        if(m_tmoduleinfo.vnm_application.empty())
        {
            cout<< "do_cfg_wildcast_switch_base vnm app is empty"<<endl;
            ackmesg.SetResult(VNET_XMLRPC_ERROR); 
            ackmesg.SetResultinfo("do_cfg_wildcast_switch_base vnm app is empty"); 
            return ERROR;
        }
    
        //将消息发到VNM
        WAITE_MSG_ACK_TIMES(cfgmessage, EV_VNETLIB_WILDCAST_SWITCH_CFG, TEMP_MU_CFG_WC_SWITCH_BASE,20000)
        {
            CWildcastSwitchCfgAckMsg cAckMsg;
            cAckMsg.deserialize(message.data);
            ackmesg.SetResult(cAckMsg.GetResult());
            ackmesg.SetResultinfo(cAckMsg.GetResultinfo());
            ackmesg.m_strUuid= cAckMsg.m_strUuid;
    
            if(m_bOpenDbgInf == 3)
            {
                cout<<"do_cfg_wildcast_switch_base  result : "<<ackmesg.GetResult()<<endl;
                cout<<"do_cfg_wildcast_switch_base  resultInfo : "<<ackmesg.GetResultinfo()<<endl;
            }
            return SUCCESS;
        }
        else
        {
            ackmesg.SetResult(VNET_XMLRPC_TIMEOUT);
            ackmesg.SetResultinfo("mu->Wait timeout!");
    
            if(m_bOpenDbgInf == 3)
            {
                cout<<"do_cfg_wildcast_switch_base  result : "<<ackmesg.GetResult()<<endl;
                cout<<"do_cfg_wildcast_switch_base  resultInfo : "<<ackmesg.GetResultinfo()<<endl;
            }
            return ERROR;
        }
    }

//交换端口通配
int32 CVNetLibNet::do_cfg_wildcast_switch_port(const CWildcastSwitchPortCfgMsg &cfgmessage,
                                 CWildcastSwitchCfgAckMsg &ackmesg)
{
        //此时还没有获取到vnm的application,和AB模型无关，因为不涉及申请资源,所以直接给TC返回失败,
        if(m_tmoduleinfo.vnm_application.empty())
        {
            cout<< "do_cfg_wildcast_switch_port vnm app is empty"<<endl;
            ackmesg.SetResult(VNET_XMLRPC_ERROR); 
            ackmesg.SetResultinfo("do_cfg_wildcast_switch_port vnm app is empty"); 
            return ERROR;
        }
    
        //将消息发到VNM
        WAITE_MSG_ACK_TIMES(cfgmessage, EV_VNETLIB_WILDCAST_SWITCH_PORT_CFG, TEMP_MU_CFG_WC_SWITCH_PORT,10000)
        {
            CWildcastSwitchCfgAckMsg cAckMsg;
            cAckMsg.deserialize(message.data);
            ackmesg.SetResult(cAckMsg.GetResult());
            ackmesg.SetResultinfo(cAckMsg.GetResultinfo());
            ackmesg.m_strUuid= cAckMsg.m_strUuid;

    
            if(m_bOpenDbgInf == 3)
            {
                cout<<"do_cfg_wildcast_switch_port  result : "<<ackmesg.GetResult()<<endl;
                cout<<"do_cfg_wildcast_switch_port  resultInfo : "<<ackmesg.GetResultinfo()<<endl;
            }
            return SUCCESS;
        }
        else
        {
            ackmesg.SetResult(VNET_XMLRPC_TIMEOUT);
            ackmesg.SetResultinfo("mu->Wait timeout!");
    
            if(m_bOpenDbgInf == 3)
            {
                cout<<"do_cfg_wildcast_switch_port  result : "<<ackmesg.GetResult()<<endl;
                cout<<"do_cfg_wildcast_switch_port  resultInfo : "<<ackmesg.GetResultinfo()<<endl;
            }
            return ERROR;
        }
    }

int32 CVNetLibNet::do_cfg_wildcast_vport(const CWildcastCreateVPortCfgMsg &cfgmessage,CWildcastCreateVPortCfgAckMsg &ackmesg)
{
        //此时还没有获取到vnm的application,和AB模型无关，因为不涉及申请资源,所以直接给TC返回失败,
        if(m_tmoduleinfo.vnm_application.empty())
        {
            cout<< "do_cfg_wildcast_vport vnm app is empty"<<endl;
            ackmesg.SetResult(VNET_XMLRPC_ERROR); 
            ackmesg.SetResultinfo("do_cfg_wildcast_vport vnm app is empty"); 
            return ERROR;
        }
    
        //将消息发到VNM
        WAITE_MSG_ACK_TIMES(cfgmessage, EV_VNETLIB_WILDCAST_CREATE_VPORT_CFG, TEMP_MU_CFG_WC_VIRTUAL_PORT,10000)
        {
            CWildcastCreateVPortCfgAckMsg cAckMsg;
            cAckMsg.deserialize(message.data);
            ackmesg.SetResult(cAckMsg.GetResult());
            ackmesg.SetResultinfo(cAckMsg.GetResultinfo());
            ackmesg.m_strUuid= cAckMsg.m_strUuid;
    
            if(m_bOpenDbgInf == 3)
            {
                cout<<"do_cfg_wildcast_vport  result : "<<ackmesg.GetResult()<<endl;
                cout<<"do_cfg_wildcast_vport  resultInfo : "<<ackmesg.GetResultinfo()<<endl;
            }
            return SUCCESS;
        }
        else
        {
            ackmesg.SetResult(VNET_XMLRPC_TIMEOUT);
            ackmesg.SetResultinfo("mu->Wait timeout!");
    
            if(m_bOpenDbgInf == 3)
            {
                cout<<"do_cfg_wildcast_vport  result : "<<ackmesg.GetResult()<<endl;
                cout<<"do_cfg_wildcast_vport  resultInfo : "<<ackmesg.GetResultinfo()<<endl;
            }
            return ERROR;
        }
    }


int32 CVNetLibNet:: do_cfg_wildcast_delTask(CWildcastTaskDelMsg &cfgmessage,
      CWildcastTaskDelAckMsg &ackmesg, int32 type)
{
    int message_id;
    
    //此时还没有获取到vnm的application,和AB模型无关，因为不涉及申请资源,所以直接给TC返回失败,
    if(m_tmoduleinfo.vnm_application.empty())
    {
        cout<< "do_cfg_wildcast_delTask vnm app is empty"<<endl;
        ackmesg.SetResult(VNET_XMLRPC_ERROR); 
        ackmesg.SetResultinfo("do_cfg_wildcast_delTask vnm app is empty"); 
        return ERROR;
    }

    if(type == VNET_LIB_WC_TASK_SWITCH)
    {
        message_id = EV_VNETLIB_WILDCAST_SWITCH_TASK_DEL;
    }
    else if(type == VNET_LIB_WC_TASK_VPORT)
    {
        message_id = EV_VNETLIB_WILDCAST_CREATE_VPORT_TASK_DEL;
    }
    else if(type == VNET_LIB_WC_TASK_LOOP)
    {
        message_id = EV_VNETLIB_WILDCAST_LOOPBACK_TASK_DEL;
    }
    /* 只有这三类消息 其他的assert */
    else
    {
        assert(0);
        return ERROR;
    }
    
    //将消息发到VNM
    WAITE_MSG_ACK_TIMES(cfgmessage, message_id, TEMP_MU_CFG_WC_DEL_TASK,10000)
    {
        CWildcastTaskDelAckMsg cAckMsg;
        cAckMsg.deserialize(message.data);
        ackmesg.SetResult(cAckMsg.GetResult());
        ackmesg.SetResultinfo(cAckMsg.GetResultinfo());
        ackmesg.m_strUuid= cAckMsg.m_strUuid;


        if(m_bOpenDbgInf == 3)
        {
            cout<<"do_cfg_wildcast_delTask  result : "<<ackmesg.GetResult()<<endl;
            cout<<"do_cfg_wildcast_delTask  resultInfo : "<<ackmesg.GetResultinfo()<<endl;
        }
        return SUCCESS;
    }
    else
    {
        ackmesg.SetResult(VNET_XMLRPC_TIMEOUT);
        ackmesg.SetResultinfo("mu->Wait timeout!");

        if(m_bOpenDbgInf == 3)
        {
            cout<<"do_cfg_wildcast_delTask  result : "<<ackmesg.GetResult()<<endl;
            cout<<"do_cfg_wildcast_delTask  resultInfo : "<<ackmesg.GetResultinfo()<<endl;
        }
        return ERROR;
    }
}

//看门狗相关操作
int32 CVNetLibNet::do_watch_dog_oprator(const MessageFrame& message)
{
   cout << "[HC --> VNA]:send EV_VNETLIB_WATCHDOG_OPER message"<<endl;

   if (m_tmoduleinfo.vna_application.empty())
    {
        do_get_vna_application(m_tmoduleinfo.vna_application);
        if (m_tmoduleinfo.vna_application.empty())
        {
            cout << "send EV_VNETLIB_WATCHDOG_OPER message to vna failed vna_application.empt"<<endl;
            //SkyAssert(0);
            return ERROR;
        }
    }
   
    CVNetWatchDogMsg cMsg;
    cMsg.deserialize(message.data);
    cMsg.m_mid = message.option.sender();
   
    //将消息发到VNA
    MID receiver(m_tmoduleinfo.vna_application,PROC_VNA, MU_VNA_CONTROLLER);
    MessageOption option(receiver,EV_VNETLIB_WATCHDOG_OPER, 0, 0);

    if (SUCCESS != mu->Send(cMsg,option))
    {
        cout << "send EV_VNETLIB_WATCHDOG_OPER message to VNA failed"<<endl;
        return ERROR;
    }
    return SUCCESS;
}

int32 CVNetLibNet::do_ack_watch_dog_oprator(const MessageFrame& message)
{
   cout << "[VNA --> HC]:send EV_VNETLIB_WATCHDOG_OPER_ACK message "<<endl;

   if (m_tmoduleinfo.vna_application.empty())
    {
        do_get_vna_application(m_tmoduleinfo.vna_application);
        if (m_tmoduleinfo.vna_application.empty())
        {
            cout << "send EV_VNETLIB_WATCHDOG_OPER_ACK message to HC failed vna_application.empty"<<endl;
            //SkyAssert(0);
            return ERROR;
        }
    }

    //MID cMid;
    CVNetWatchDogMsgAck cMsg;
    cMsg.deserialize(message.data);

    /*if(SUCCESS != GetMidByEvent(EV_VNETLIB_WATCHDOG_OPER_ACK, cMid))
    {
        cout << "send EV_VNETLIB_WATCHDOG_OPER_ACK message to HC GetMidByEvent failed"<<endl;
        //SkyAssert(0);
        return ERROR;
    }*/

    MessageOption option(cMsg.m_mid/*cMid*/, EV_VNETLIB_WATCHDOG_OPER_ACK, 0, 0);

    if (SUCCESS != mu->Send(message.data,option))
    {
        cout << "send EV_VNETLIB_WATCHDOG_OPER_ACK message to HC failed"<<endl;
        return ERROR;
    }

    return SUCCESS;
}

int32 CVNetLibNet::do_watch_dog_rr(const MessageFrame& message)
{
    cout << "[VNA --> HC]:send EV_VNETLIB_WATCHDOG_NOTIFY_HC_RESTART message "<<endl;

    //消息反序列化
    CVNetNotifyVmRestatMsg cMsg;
    cMsg.deserialize(message.data);

    VmRRReq req(cMsg.m_vm_id, RR, 0);
    req.action_id = cMsg.action_id;

    if (m_tmoduleinfo.vna_application.empty())
    {
        do_get_vna_application(m_tmoduleinfo.vna_application);
        if (m_tmoduleinfo.vna_application.empty())
        {
            cout << "send EV_VNETLIB_WATCHDOG_NOTIFY_HC_RESTART message to HC failed vna_application.empty"<<endl;
            //SkyAssert(0);
            return ERROR;
        }
    }

    MID cMid;
    if(SUCCESS != GetMidByEvent(EV_VM_OP , cMid))
    {
        cout << "send EV_VNETLIB_WATCHDOG_NOTIFY_HC_RESTART message to HC GetMidByEvent failed"<<endl;
        //SkyAssert(0);
        return ERROR;
    }

    MessageOption option(cMid, EV_VM_OP, 0, 0);

    if (SUCCESS != mu->Send(req,option))
    {
        cout << "send EV_VNETLIB_WATCHDOG_NOTIFY_HC_RESTART message to HC failed \n"<<endl;
        return ERROR;
    }

    return SUCCESS;
}

int32 CVNetLibNet::do_ack_watch_dog_rr(const MessageFrame& message)
{
    cout << "[HC--> VNA]:send EV_VNETLIB_WATCHDOG_NOTIFY_HC_RESTART_ACK message "<<endl;

    //消息反序列化
    VmOperationAck cMsg;
    cMsg.deserialize(message.data);

    CVNetNotifyHCrestatMsgAck req;	
    req.m_vm_id = cMsg._vid;
    req.state = cMsg.state;
    req.m_operator = cMsg._operation;

    if (m_tmoduleinfo.vna_application.empty())
    {
        do_get_vna_application(m_tmoduleinfo.vna_application);
        if (m_tmoduleinfo.vna_application.empty())
        {
            cout << "send EV_VNETLIB_WATCHDOG_NOTIFY_HC_RESTART_ACK message to vna failed vna_application.empty"<<endl;
            //SkyAssert(0);
            return ERROR;
        }
    }

    //将消息发到VNA
    MID receiver(m_tmoduleinfo.vna_application,PROC_VNA, MU_VNA_CONTROLLER);
    MessageOption option(receiver,EV_VNETLIB_WATCHDOG_NOTIFY_HC_RESTART_ACK, 0, 0);

    if (SUCCESS != mu->Send(req,option))
    {
        cout << "send EV_VNETLIB_WATCHDOG_NOTIFY_HC_RESTART_ACK message to VNA failed \n"<<endl;
        return ERROR;
    }
    return SUCCESS;
}


int32 CVNetLibNet::do_wdg_oprator(const MessageFrame& message)
{
   cout << "[VNA --> WDG]:send EV_VNETLIB_WDG_OPER message"<<endl;

   if (m_tmoduleinfo.vna_application.empty())
    {
        do_get_vna_application(m_tmoduleinfo.vna_application);
        if (m_tmoduleinfo.vna_application.empty())
        {
            cout << "send EV_VNETLIB_WDG_OPER message to wdg failed vna_application.empty"<<endl;
            //SkyAssert(0);
            return ERROR;
        }
    }

    MID cMid;
    if(SUCCESS != GetMidByEvent(EV_VNETLIB_WDG_OPER, cMid))
    {
        cout << "send EV_VNETLIB_WDG_OPER message to wdg GetMidByEvent failed"<<endl;
        //SkyAssert(0);
        return ERROR;
    }

    MessageOption option(cMid, EV_VNETLIB_WDG_OPER, 0, 0);

    if (SUCCESS != mu->Send(message.data,option))
    {
        cout << "send EV_VNETLIB_WDG_OPER message to wdg failed"<<endl;
        return ERROR;
    }
    return SUCCESS;
}

int32 CVNetLibNet::do_ack_wdg_oprator(const MessageFrame& message)
{
     cout << "[WDG --> VNA]:send EV_VNETLIB_WDG_OPER_ACK message"<<endl;

   if (m_tmoduleinfo.vna_application.empty())
    {
        do_get_vna_application(m_tmoduleinfo.vna_application);
        if (m_tmoduleinfo.vna_application.empty())
        {
            cout << "send EV_VNETLIB_WDG_OPER_ACK message to vna failed vna_application.empty"<<endl;
            //SkyAssert(0);
            return ERROR;
        }
    }

    //将消息发到VNA
    MID receiver(m_tmoduleinfo.vna_application,PROC_VNA, MU_VNA_CONTROLLER);
    MessageOption option(receiver,EV_VNETLIB_WDG_OPER_ACK, 0, 0);

    if (SUCCESS != mu->Send(message.data,option))
    {
        cout << "send EV_VNETLIB_WDG_OPER_ACK message to VNA failed"<<endl;
        return ERROR;
    }
    return SUCCESS;
}

int32 CVNetLibNet::do_wdg_oprator_timer(const MessageFrame& message)
{
   cout << "[VNA --> WDG]:send EV_VNETLIB_WDG_TIMER_OPER message"<<endl;

   if (m_tmoduleinfo.vna_application.empty())
    {
        do_get_vna_application(m_tmoduleinfo.vna_application);
        if (m_tmoduleinfo.vna_application.empty())
        {
            cout << "send EV_VNETLIB_WDG_TIMER_OPER message to wdg failed vna_application.empty"<<endl;
            //SkyAssert(0);
            return ERROR;
        }
    }

    MID cMid;
    if(SUCCESS != GetMidByEvent(EV_VNETLIB_WDG_TIMER_OPER, cMid))
    {
        cout << "send EV_VNETLIB_WDG_TIMER_OPER message to wdg GetMidByEvent failed"<<endl;
        //SkyAssert(0);
        return ERROR;
    }

    MessageOption option(cMid, EV_VNETLIB_WDG_TIMER_OPER, 0, 0);

    if (SUCCESS != mu->Send(message.data,option))
    {
        cout << "send EV_VNETLIB_WDG_TIMER_OPER message to wdg failed"<<endl;
        return ERROR;
    }
    return SUCCESS;
}

int32 CVNetLibNet::do_ack_wdg_oprator_timer(const MessageFrame& message)
{
   cout << "[WDG --> VNA]:send EV_VNETLIB_WDG_TIMER_OPER_ACK message"<<endl;

   if (m_tmoduleinfo.vna_application.empty())
    {
        do_get_vna_application(m_tmoduleinfo.vna_application);
        if (m_tmoduleinfo.vna_application.empty())
        {
            cout << "send EV_VNETLIB_WDG_TIMER_OPER_ACK message to vna failed"<<endl;
            //SkyAssert(0);
            return ERROR;
        }
    }

    //将消息发到VNA
    MID receiver(m_tmoduleinfo.vna_application,PROC_VNA, MU_VNA_CONTROLLER);
    MessageOption option(receiver,EV_VNETLIB_WDG_TIMER_OPER_ACK, 0, 0);

    if (SUCCESS != mu->Send(message.data,option))
    {
        cout << "send EV_VNETLIB_WDG_TIMER_OPER_ACK message to VNA failed"<<endl;
        return ERROR;
    }
    return SUCCESS;
}

int32 CVNetLibNet::do_wdg_oprator_rr(const MessageFrame& message)
{
    cout << "[WDG --> VNA]:send EV_VNETLIB_NOTIFY_VM_RR message"<<endl;

   if (m_tmoduleinfo.vna_application.empty())
    {
        do_get_vna_application(m_tmoduleinfo.vna_application);
        if (m_tmoduleinfo.vna_application.empty())
        {
            cout << "send EV_VNETLIB_NOTIFY_VM_RR message to vna failed vna_application.empty"<<endl;
            //SkyAssert(0);
            return ERROR;
        }
    }

    //将消息发到VNA
    MID receiver(m_tmoduleinfo.vna_application,PROC_VNA, MU_VNA_CONTROLLER);
    MessageOption option(receiver,EV_VNETLIB_NOTIFY_VM_RR, 0, 0);

    if (SUCCESS != mu->Send(message.data,option))
    {
        cout << "send EV_VNETLIB_NOTIFY_VM_RR message to VNA failed"<<endl;
        return ERROR;
    }
    return SUCCESS;
}

int32 CVNetLibNet::do_ack_wdg_oprator_rr(const MessageFrame& message)
{
   cout << "[VNA --> WDG]:send EV_VNETLIB_NOTIFY_VM_RR_ACK message"<<endl;

   if (m_tmoduleinfo.vna_application.empty())
    {
        do_get_vna_application(m_tmoduleinfo.vna_application);
        if (m_tmoduleinfo.vna_application.empty())
        {
            cout << "send EV_VNETLIB_NOTIFY_VM_RR_ACK message to wdg failed vna_application.empty"<<endl;
            //SkyAssert(0);
            return ERROR;
        }
    }

    MID cMid;
    if(SUCCESS != GetMidByEvent(EV_VNETLIB_NOTIFY_VM_RR_ACK, cMid))
    {
        cout << "send EV_VNETLIB_NOTIFY_VM_RR_ACK message to wdg GetMidByEvent failed"<<endl;
        //SkyAssert(0);
        return ERROR;
    }

    MessageOption option(cMid, EV_VNETLIB_NOTIFY_VM_RR_ACK, 0, 0);

    if (SUCCESS != mu->Send(message.data,option))
    {
        cout << "send EV_VNETLIB_NOTIFY_VM_RR_ACK message to wdg failed \n"<<endl;
        return ERROR;
    }
    return SUCCESS;
}


int32 CVNetLibNet::do_vna_clear(CVNetVnaDelete& cfgmessage,CVNetVnaDeleteAck& cfgackmessage)
{
    //此时还没有获取到vnm的application,和AB模型无关，因为不涉及申请资源,所以直接给TC返回失败,
    if(m_tmoduleinfo.vnm_application.empty())
    {
        cout<< "do_vna_clear vnm app is empty"<<endl;
        cfgackmessage.SetResult(VNET_XMLRPC_ERROR); 
        cfgackmessage.SetResultinfo("do_vna_clear vnm app is empty"); 
        return ERROR;
    }

    //将消息发到VNM
    WAITE_MSG_ACK_TIMES(cfgmessage, EV_VNETLIB_DEL_VNA , TEMP_MU_VNA_DELETE,10000)
    {
        CVNetVnaDeleteAck   cAckMsg;
        cAckMsg.deserialize(message.data);
        cfgackmessage.SetResult(cAckMsg.GetResult());
        cfgackmessage.SetResultinfo(cAckMsg.GetResultinfo());

        if(m_bOpenDbgInf == 3)
        {
            cout<<"do_vna_clear  result : "<<cfgackmessage.GetResult()<<endl;
            cout<<"do_vna_clear  resultInfo : "<<cfgackmessage.GetResultinfo()<<endl;
        }
        return SUCCESS;
    }
    else
    {
        cfgackmessage.SetResult(VNET_XMLRPC_TIMEOUT);
        cfgackmessage.SetResultinfo("mu->Wait timeout!");

        if(m_bOpenDbgInf == 3)
        {
            cout<<"do_vna_clear  result : "<<cfgackmessage.GetResult()<<endl;
            cout<<"do_vna_clear  resultInfo : "<<cfgackmessage.GetResultinfo()<<endl;
        }
        return ERROR;
    }
}

int32 CVNetLibNet::do_cfg_logicnet_id2name(CVNetLGNetID2Name& cfgmessage)
{
    //此时还没有获取到vnm的application,和AB模型无关，因为不涉及申请资源,所以直接给TC返回失败,
    if(m_tmoduleinfo.vnm_application.empty())
    {
        cout<< "do_cfg_logicnet_id2name vnm app is empty"<<endl;
        cfgmessage.SetResult(VNET_XMLRPC_ERROR); 
        cfgmessage.SetResultinfo("do_cfg_logicnet_id2name vnm app is empty"); 
        return ERROR;
    }

    //将消息发到VNM
    WAITE_MSG_ACK_TIMES(cfgmessage, EV_VNETLIB_LOGIC_NETWORK_NAME2UUID , TEMP_MU_LGNET_NAME2ID,10000)
    {
        CVNetLGNetID2Name   cAckMsg;
        cAckMsg.deserialize(message.data);
		cfgmessage.m_strUuid = cAckMsg.m_strUuid;
        cfgmessage.SetResult(cAckMsg.GetResult());
        cfgmessage.SetResultinfo(cAckMsg.GetResultinfo());

        if(m_bOpenDbgInf == 3)
        {
            cout<<"do_cfg_logicnet_id2name  result : "<<cfgmessage.GetResult()<<endl;
            cout<<"do_cfg_logicnet_id2name  resultInfo : "<<cfgmessage.GetResultinfo()<<endl;
			cout<<"do_cfg_logicnet_id2name  uuid : "<<cfgmessage.m_strUuid<<endl;
        }
        return SUCCESS;
    }
    else
    {
        cfgmessage.SetResult(VNET_XMLRPC_TIMEOUT);
        cfgmessage.SetResultinfo("mu->Wait timeout!");

        if(m_bOpenDbgInf == 3)
        {
            cout<<"do_cfg_logicnet_id2name  result : "<<cfgmessage.GetResult()<<endl;
            cout<<"do_cfg_logicnet_id2name  resultInfo : "<<cfgmessage.GetResultinfo()<<endl;
        }
        return ERROR;
    }
}

int32 CVNetLibNet::do_portgroup_query(CVNetPortGroupQuery& cfgmessage, 
CVnetPortGroupLists & ackmessge)
{
    //此时还没有获取到vnm的application,和AB模型无关，因为不涉及申请资源,所以直接给TC返回失败,
    if(m_tmoduleinfo.vnm_application.empty())
    {
        cout<< "do_portgroup_query vnm app is empty"<<endl;
        cfgmessage.SetResult(VNET_XMLRPC_ERROR); 
        cfgmessage.SetResultinfo("vnm application is null");
        return ERROR;
    }

    //将消息发到VNM
    WAITE_MSG_ACK(cfgmessage, EV_VNETLIB_PG_QUERY, TEMP_MU_PORTGROUP_QUERY) 
    {
        //CVnetPortGroupLists cAckMsg;
        ackmessge.deserialize(message.data);
        cfgmessage.SetResult(ackmessge.GetResult());
        cfgmessage.SetResultinfo(ackmessge.GetResultinfo());

        if(m_bOpenDbgInf == 3)
        {
            cout<<"do_portgroup_query result : "<<cfgmessage.GetResult()<<endl;
            cout<<"do_portgroup_query resultInfo : "<<cfgmessage.GetResultinfo()<<endl;
        }
        return SUCCESS;
    }
    else
    {
        cfgmessage.SetResult(VNET_XMLRPC_TIMEOUT);
        cfgmessage.SetResultinfo("mu->Wait timeout");

        if(m_bOpenDbgInf == 3)
        {
            cout<<"do_portgroup_query result : "<<cfgmessage.GetResult()<<endl;
            cout<<"do_portgroup_query resultInfo : "<<cfgmessage.GetResultinfo()<<endl;
        }
        return ERROR;
    }
}


void CVNetLibNet::MessageEntry(const MessageFrame& message)
{
    switch (mu->get_state())
    {
        case S_Startup:
        {
            switch (message.option.id())
            {
                case EV_VNETLIB_POWER_ON:
                {
                    if(0 != InitNetLib())
                    {
                        SkyExit(-1,"CVNetController power on InitVNetCtl failed!");
                    }
                    mu->set_state(S_Work);
                    cout<<mu->name()<<"vnetlib mu  power on! "<<endl;
                }
                break;
            }
        }
        break;
        case S_Work:
        {
            if (0 != pthread_mutex_lock(&m_mutex))
            {
                cout <<"MessageEntry: pthread_mutex_lock failed"<<endl;
                break;
            }
            //根据收到的消息id进行处理
            switch (message.option.id())
            {
                //VNM组播application消息
                case EV_MG_VNM_INFO:
                {
                    if(2 == m_bOpenDbgInf)
                    {
                        cout<<"receive message EV_MG_VNM_INFO"<<endl;
                    }
                    do_get_vnmapp(message);
                }
                break;
                
                //VNA回应注册成功消息
                case EV_VNETLIB_REGISTER_MSG_ACK:
                {
                    if(2 == m_bOpenDbgInf)
                    {
                        cout<<"receive message EV_VNETLIB_REGISTER_MSG_ACK"<<endl;
                    }
                    do_ack_vna_reg(message);
                };
                break;
                
                //与VNA的心跳处理
                case EV_VNETLIB_HEART_BEAT_MSG:
                {
                    do_heartbeat();
                }
                break;

                //VNA回应LIB的心跳信息
                case EV_VNETLIB_HEART_BEAT_MSG_ACK:
                {
                    if(2 == m_bOpenDbgInf) 
                    {
                        cout<<"receive message EV_VNETLIB_HEART_BEAT_MSG_ACK"<<endl;
                    }
                    do_heartbeatAck(message);
                }
                break;

                //HC组播过来的集群信息
                case EV_REGISTERED_SYSTEM:
                {
                    if(2 == m_bOpenDbgInf)
                    {
                        cout<<"receive message EV_REGISTERED_SYSTEM"<<endl;
                    }
                    do_registercluster(message);
                }
                break;
                
                //TC调度到CC列表
                case EV_VNETLIB_SELECT_CC:
                {
                    do_schedule_cc(message);
                }
                break;

                //VNM回应TC调度到CC列表
                case EV_VNETLIB_SELECT_CC_ACK:
                {
                    do_ack_schedule_cc(message);
                }
                break;

                //TC给CC分配网络资源(VSI)
                case EV_VNETLIB_GETRESOURCE_CC:
                {
                    do_getresource_cc(message);
                }
                break;

               //VNM回应的TC分配CC上的网络资源
                case EV_VNETLIB_GETRESOURCE_CC_ACK:
                {
                   do_ack_getresource_cc(message);
                }
                break;

                //TC释放CC上网络资源
                case EV_VNETLIB_FREERESOURCE_CC:
                {
                    do_freeresource_cc(message);
                }
                break;

                //VNM回应的TC释放CC上网络资源
                case EV_VNETLIB_FREERESOURCE_CC_ACK:
                {
                    do_ack_freeresource_cc(message);
                }
                break;
                
                //CC调度到HC列表
                case EV_VNETLIB_SELECT_HC:
                {
                    do_schedule_hc(message);
                }
                break;

                //VNM回应CC调度到HC列表
                case EV_VNETLIB_SELECT_HC_ACK:
                {
                    do_ack_schedule_hc(message);
                }
                break;
                
                //CC给HC分配网络资源(PCI/bridge)
                case EV_VNETLIB_GETRESOURCE_HC:
                {
                    do_getresource_hc(message);
                }
                break;    

                //给HC分配网络资源(PCI/bridge)的应答
                case EV_VNETLIB_GETRESOURCE_HC_ACK:
                {
                    do_ack_getresource_hc(message);
                }
                break; 

                //HC上释放网络资源
                case EV_VNETLIB_FREERESOURCE_HC:
                {
                    do_freeresource_hc(message);
                }
                break;

                //HC上释放网络资源的应答
                case EV_VNETLIB_FREERESOURCE_HC_ACK:
                {
                    do_ack_freeresource_hc(message);
                }
                break;

                //startnetwork资源校验
                case EV_VNETLIB_STARTNETWORK:
                {
                    do_startnetwork(message);
                }
                break;

                //startnetwork资源校验应答
                case EV_VNETLIB_STARTNETWORK_ACK:
                {
                    do_ack_startnetwork(message);
                }
                break;

                //stopnetwork资源校验
                case EV_VNETLIB_STOPNETWORK:
                {
                    do_stopnetwork(message);
                }
                break;

                //startnetwork资源校验应答
                case EV_VNETLIB_STOPNETWORK_ACK:
                {
                    do_ack_stopnetwork(message);
                }
                break; 
                
                //通知VNA domid
                case EV_VNETLIB_NOTIFY_DOMID:
                {
                    do_notify_domid(message);
                }
                break;

                //通知VNA domid的应答
                case EV_VNETLIB_NOTIFY_DOMID_ACK:
                {
                    do_ack_notify_domid(message);
                }
                break;

                case EV_VNETLIB_QUERY_VNICS:
                {
                    do_query_vnics(message);
                }
                break;

                case EV_VNETLIB_QUERY_VNICS_ACK:
                {
                    do_ack_query_vnics(message);
                } 
                break;

                /*************WDG Begin******************/
                //HC<---------->VNA   start/stop wdg timer
                case EV_VNETLIB_WATCHDOG_OPER:
                {
                    do_watch_dog_oprator(message);
                }
                break;
                
                case EV_VNETLIB_WATCHDOG_OPER_ACK:
                {
                    do_ack_watch_dog_oprator(message);
                }
                break;

                //HC<---------->VNA   RR Oper
                case EV_VNETLIB_WATCHDOG_NOTIFY_HC_RESTART:
                {
                    do_watch_dog_rr(message);
                }
                break;
                
                //case EV_VNETLIB_WATCHDOG_NOTIFY_HC_RESTART_ACK:
                case EV_VM_OP_ACK:
                {
                    do_ack_watch_dog_rr(message);
                }
                break;


                //WDG<---------->VNA   start/stop wdg
                case EV_VNETLIB_WDG_OPER:
                {
                    do_wdg_oprator(message);
                }
                break;

                case EV_VNETLIB_WDG_OPER_ACK:
                {
                    do_ack_wdg_oprator(message);
                }
                break;
                
                //WDG<---------->VNA   start/stop wdg timer
                case EV_VNETLIB_WDG_TIMER_OPER:
                {
                    do_wdg_oprator_timer(message);
                }
                break;

                case EV_VNETLIB_WDG_TIMER_OPER_ACK:
                {
                    do_ack_wdg_oprator_timer(message);
                }
                break;

                //WDG<---------->VNA   RR Oper
                case EV_VNETLIB_NOTIFY_VM_RR:
                {
                    do_wdg_oprator_rr(message);
                }
                break;

                case EV_VNETLIB_NOTIFY_VM_RR_ACK:
                {
                    do_ack_wdg_oprator_rr(message);
                }
                break;
                /*************WDG End******************/

                /*****************热迁移接口***********/
                case EV_VNETLIB_VM_MIGRATE_BEGIN:
                {
                    do_vm_migrate_begin(message);
                }
                break;

                case EV_VNETLIB_VM_MIGRATE_BEGIN_ACK:
                {
                    do_vm_migrate_begin_ack(message);
                }
                break;

                case EV_VNETLIB_VM_MIGRATE_END:
                {
                    do_vm_migrate_end(message);
                }
                break;

                case EV_VNETLIB_VM_MIGRATE_END_ACK:
                {
                    do_vm_migrate_end_ack(message);
                }
                break;

                case EV_VNETLIB_SELECT_MIGRATE_HC:
                {
                    do_select_migrate_hc(message);
                }
                break;

                case EV_VNETLIB_SELECT_MIGRATE_HC_ACK:
                {
                    do_select_migrate_hc_ack(message);
                }
                break;

                default:
                    break;
            }

            if (0 != pthread_mutex_unlock(&m_mutex))
            {
                cout <<"MessageEntry: pthread_mutex_unlock failed"<<endl;
                break;
            }
        } // POWERON
        break;

        default:
           break;
    }
}

int CVNetLibNet::RegisterEvent(uint32 event,MID mid)
{
    Event_MID.insert(make_pair(event, mid));
    return SUCCESS;
}

int CVNetLibNet::GetMidByEvent(uint32 event,MID &mid)
{
    map<uint32, MID>::iterator mid_iter = Event_MID.begin();
    for( ; mid_iter != Event_MID.end(); ++mid_iter)
    {
        if(mid_iter->first == event)
        {
            mid = mid_iter->second;
            return SUCCESS;
        }
    }
    return ERROR;
}

int32 CVNetLibNet::Init(string application,int role)
{
    //防止重复初始化
    if(mu && S_Work == mu->get_state())
    {
        return SUCCESS ;
    }
    string vna_app;
    m_tmoduleinfo.module_application = application;
    m_tmoduleinfo.module_role = role;

    int32 ret = StartMu(MU_VNET_LIBNET);
    if(ret != SUCCESS )
    {
        return ERROR;
    }
    if(VNA_MODULE_ROLE_HC == role)
    {
        JoinGroup(ApplicationName() + MUTIGROUP_SUF_REG_SYSTEM);
    }
    JoinGroup(MUTIGROUP_VNM_MS);
    do_get_vna_application(vna_app);
    m_tmoduleinfo.vna_application = vna_app;
    return SUCCESS;
 }

#if 0
int32 CVNetLibNet::Init(string application)
{
    //防止重复初始化
    if(mu && S_Work == mu->get_state())
    {
        return SUCCESS ;
    }
    m_tmoduleinfo.module_application = application;
    int32 ret = StartMu(MU_VNET_LIBNET);
    if(ret != SUCCESS )
    {
        return ERROR;
    }
    JoinGroup(MUTIGROUP_VNM_MS);
    string vna_app;
    do_get_vna_application(vna_app);
    m_tmoduleinfo.vna_application = vna_app;
    m_tmoduleinfo.module_role = VNA_MODULE_ROLE_OTHER;
    return SUCCESS;
 }

int32 CVNetLibNet::Init()
{
    //防止重复初始化
    if(mu && S_Work == mu->get_state())
    {
        return SUCCESS ;
    }
    string application ="abc";
    m_tmoduleinfo.module_application = application;
    m_tmoduleinfo.module_role = VNA_MODULE_ROLE_OTHER;
    int32 ret = StartMu(MU_VNET_LIBNET);
    if(ret != SUCCESS )
    {
        return ERROR;
    }
    JoinGroup(MUTIGROUP_VNM_MS);
    string vna_app;
    do_get_vna_application(vna_app);
    m_tmoduleinfo.vna_application = vna_app;
    return SUCCESS;
 }

#endif

int32 CVNetLibNet::InitNetLib()
{
    cout <<"enter CVNetLibNet::InitNetLib "<<endl;    
    mu->set_state(S_Startup);
    
    // 初始化对外函数接口锁
    if (0 != pthread_mutex_init(&m_mutex, NULL))
    {
        cout << "InitVNetLib: call pthread_mutex_init failed"<<endl;
        return ERROR;
    }
    
    if(m_tmoduleinfo.module_application.empty())
    {
        //SkyAssert(false);
        cout<<"in InitNetLib module_application is null"<<endl;
        return ERROR;
    }
    m_tmoduleinfo.keepalive_state = VNA_LINK_UP;
    m_tmoduleinfo.register_state = VNA_REGISTER_STATE_NOREG;

    //创建心跳定时器
    TimeOut tTmOut;
    m_tHeartBeatTimer = mu->CreateTimer();
    if (INVALID_TIMER_ID == m_tHeartBeatTimer)
    {
        cout <<"in InitNetLib CreateTimer failed\n"<<endl;
        //SkyAssert(false);
        return ERROR;
    }
    tTmOut.duration = VNETLIB_LOOP_INTERVAL;
    tTmOut.msgid = EV_VNETLIB_HEART_BEAT_MSG;
    tTmOut.type = LOOP_TIMER;
    tTmOut.arg = 0;
    if (SUCCESS != mu->SetTimer(m_tHeartBeatTimer, tTmOut))
    {
        cout<<"successful construct InitNetLib, loop timer id: "<<m_tHeartBeatTimer<<endl;
    }
    m_tmoduleinfo.timeout = VNETLIB_KEEPLIVE_INTERRVAL;
    return SUCCESS;
}

void CVNetLibNet::ShowMidEvent()
{
    if(Event_MID.empty())
    {
        cout << "mid_event map is null"<<endl;
        return ;
    }
    map<uint32, MID>::iterator mid_iter = Event_MID.begin();

    for(;mid_iter !=Event_MID.end();mid_iter++)
    {
        cout<<"EVENT is    : "<<mid_iter->first<<endl;
        cout<<"Mid info    =================="<<endl;
        cout<<"application : "<<mid_iter->second._application<<endl;
        cout<<"process     : "<<mid_iter->second._process<<endl;
        cout<<"unit        : "<<mid_iter->second._unit<<endl;
    }
    return ;
}

void CVNetLibNet::ShowVnics()
{
    if(m_vec_allvnics.m_allvnics.empty())
    {
        cout << "vnics  is null"<<endl;
        return ;
    }
    m_vec_allvnics.Print();
    return ;
}


int VNET_DBG_SET_PLUGIN_ON(int32 nFlag)
{
    CVNetLibNet *pVNetlib = CVNetLibNet::GetInstance();
    if (pVNetlib)
    {
        pVNetlib->SetDbgInfFlg(nFlag);
    }
    return 0;
}
DEFINE_DEBUG_FUNC(VNET_DBG_SET_PLUGIN_ON);

int VNET_DBG_SHOW_MID_REG()
{
    CVNetLibNet *pVNetlib = CVNetLibNet::GetInstance();
    if (pVNetlib)
    {
        pVNetlib->ShowMidEvent();
    }
    return 0;
}
DEFINE_DEBUG_FUNC(VNET_DBG_SHOW_MID_REG);

int VNET_DBG_SHOW_VNICS()
{
    CVNetLibNet *pVNetlib = CVNetLibNet::GetInstance();
    if (pVNetlib)
    {
        pVNetlib->ShowVnics();
    }
    return 0;
}
DEFINE_DEBUG_FUNC(VNET_DBG_SHOW_VNICS);


}//end namespace

