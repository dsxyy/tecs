/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vnm_vm_mgr.cpp
* 文件标识：
* 内容摘要：CVNMVmMgr类实现文件
* 当前版本：1.0
* 作    者：
* 完成日期：2013年2月22日
*
* 修改记录1：
*     修改日期：2013/2/22
*     版 本 号：V1.0
*     修 改 人：lverchun
*     修改内容：创建
******************************************************************************/
#include "vnet_comm.h"
#include "vnet_error.h"
#include "vnet_mid.h"
#include "vm_messages.h"
#include "vnet_db_mgr.h"
#include "vnetlib_event.h"
#include "vnetlib_msg.h"
#include "vnet_event.h"
#include "vna_reg_mod.h"
#include "vnet_msg.h"
#include "vna_info.h"
#include "vna_mgr.h"
#include "ipv4_addr.h"
#include "ipv4_addr_range.h"
#include "ipv4_pool_mgr.h"
#include "vlan_range.h"
#include "vlan_pool_mgr.h"
#include "mac_addr.h"
#include "mac_range.h"
#include "mac_pool_mgr.h"
#include "logic_network.h"
#include "logic_network_mgr.h"
#include "vnet_vnic.h"
#include "vnet_portmgr.h"
#include "vnm_switch_manager.h"
#include "vnet_db_vm.h"
#include "vnet_db_vsi.h"
#include "vnm_vsi_mgr.h"
#include "vnm_vm_db.h"
#include "vnm_vnic_db.h"
#include "vnet_db_schedule.h"
#include "vnm_schedule.h"
#include "vnm_vm_mgr.h"
#include "vm_migrate_mgr.h"

namespace zte_tecs
{
CVNetVmMgr *CVNetVmMgr::s_pInstance = NULL;

CVNetVmMgr::CVNetVmMgr()
{
    m_pMU = NULL;
    m_pVNetVmDB = NULL;
    m_pVNetVnicDB = NULL;
    m_pVSIProfileMgr = NULL;
    m_pSwitchMgr = NULL;
    m_pVNAMgr = NULL;
    m_bOpenDbgInf = TRUE;
};

CVNetVmMgr::~CVNetVmMgr()
{
    m_pMU = NULL;
    m_pVNetVmDB = NULL;
    m_pVNetVnicDB = NULL;
    m_pVSIProfileMgr = NULL;
    m_pSwitchMgr = NULL;
    m_pVNAMgr = NULL;
};

/************************************************************
* 函数名称： Init
* 功能描述： 初始化函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2012/5         V1.0       lvech         创建
***************************************************************/
int32 CVNetVmMgr::Init(MessageUnit *mu)
{
    if (mu == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetVmMgr::Init: mu is NULL.\n");
        return -1;
    }
    
    m_pMU = mu;

    m_pVNetVmDB = CVNetVmDB::GetInstance();
    if ( NULL == m_pVNetVmDB)
    {
        cout << "CVNetVmDB::GetInstance() is NULL" << endl;
        return -1;
    }
    
    m_pVNetVnicDB = CVNetVnicDB::GetInstance();
    if( NULL == m_pVNetVnicDB)
    {
        cout << "CVNetVnicDB::GetInstance() is NULL" << endl;
        return -1;
    }

    //vsi
    m_pVSIProfileMgr = CVSIProfileMgr::GetInstance();
    if (NULL == m_pVSIProfileMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetManager::InitVNetManager: NULL == m_pVSIProfileMgr.\n");
        return -1;
    }
        
    m_pSwitchMgr = CSwitchMgr::GetInstance();
    if( NULL == m_pSwitchMgr)
    {
        cout << "CSwitchMgr::GetInstance() is NULL" << endl;
        return -1;
    }

    m_pVNAMgr = CVNAManager::GetInstance();
    if( NULL == m_pVNAMgr)
    {
        cout << "CVNAManager::GetInstance() is NULL" << endl;
        return -1;
    }

    VNET_LOG(VNET_LOG_WARN, "CVNetVnicDB::Init: Call  successfully.\n");

    return 0;
}

/************************************************************
* 函数名称： MessageEntry
* 功能描述： 消息主入口函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2012/5         V1.0       lvech         创建
***************************************************************/
void CVNetVmMgr::MessageEntry(const MessageFrame &message)
{
    //根据收到的消息id进行处理
    switch (message.option.id())
    {
        //处理TC向VNM获取满足VM部署条件的CC调度列表消息
        case EV_VNETLIB_SELECT_CC:
        {
            SelectCCListForVM(message);
        }
        break;

        //处理从schedule模块收到CC调度列表消息
        case EV_VNM_SCHEDULE_CC_ACK:
        {
            RcvSelectCCAckFromSchedule(message);
        }
        break;

        //TC为VM选定部署CC后，向VNM申请分配网络资源的请求消息
        case EV_VNETLIB_GETRESOURCE_CC:
        {
            GetNetResourceOnCC(message);
        }
        break;

        //TC为VM选定部署CC后，向VNM申请分配网络资源的请求消息
        case EV_VNM_GET_RESOURCE_FOR_CC_ACK:
        {
            RcvGetResCCAckFromSchedule(message);
        }
        break;
    
        //TC向VNM申请为VM释放网络资源的请求消息
        case EV_VNETLIB_FREERESOURCE_CC:
        {
            FreeNetResourceOnCC(message);
        }
        break;

        case EV_VNM_FREE_RESOURCE_FOR_CC_ACK:
        {
            RcvFreeResCCAckFromSchedule(message);
        }
        break;
    
        //处理CC向VNM获取满足VM部署条件的HC调度列表消息
        case EV_VNETLIB_SELECT_HC:
        {
            SelectHCListForVM(message);
        }
        break;

        case EV_VNM_SCHEDULE_HC_ACK:
        {
            RcvSelectHCAckFromSchedule(message);
        }
        break;
            
        //CC为VM选定部署HC后，向VNM申请分配PCI资源的请求消息
        case EV_VNETLIB_GETRESOURCE_HC:
        {
            GetNetResourceOnHC(message);
        }
        break;

        case EV_VNM_GET_RESOURCE_FOR_HC_ACK:
        {
            RcvGetResHCAckFromSchedule(message);
        }
        break;
        
        //CC向VNM申请为VM释放网络资源的请求消息
        case EV_VNETLIB_FREERESOURCE_HC:
        {
            FreeNetResourceOnHC(message);
        }
        break;

        case EV_VNM_FREE_RESOURCE_FOR_HC_ACK:
        {
            RcvFreeResHCAckFromSchedule(message);
        }
        break;
        
        default:
            break;
    }

}

/************************************************************
* 函数名称： SelectCCListForVM
* 功能描述： 处理TC向VNM获取满足VM部署条件的CC调度列表消息
* 访问的表： 无
* 修改的表： 无
* 输入参数： vmniccfg
* 输出参数： cc_list
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1         V1.0       lvech         创建
***************************************************************/
void CVNetVmMgr::SelectCCListForVM(const MessageFrame& message)
{
    int32 iRet = 0;
    vector<string> vec_cc;
    CVNetSelectCCListMsg cMsg;
    
    cMsg.deserialize(message.data);

    if (m_bOpenDbgInf)
    {
        cout << "CVNetVmMgr: Receive SelectCCListForVM message:" <<endl;
        cout << cMsg.serialize();
    }


    //已有任务在执行
    if (m_cVMSchTaskInfo.m_nVmID)
    {
        //任务正在执行
        if ((m_cVMSchTaskInfo.m_strActionID == cMsg.action_id) 
            && (VNM_VM_NETWORK_OPERATE_SELECT_CC == m_cVMSchTaskInfo.m_nCurrentOP))
        {
            iRet = ERROR_ACTION_RUNNING;
            goto SEND_ACK;
        }
        else
        {
            return;                
        }
    }
    else
    {        
        m_cVMSchTaskInfo.m_nVmID = cMsg.m_vm_id;
        m_cVMSchTaskInfo.m_nCurrentOP = VNM_VM_NETWORK_OPERATE_SELECT_CC;
        m_cVMSchTaskInfo.m_strActionID = cMsg.action_id;
        m_cVMSchTaskInfo.m_cSenderMId = message.option.sender();

        //向schedule模块转发消息
        MID mid;
        mid._application = ApplicationName();
        mid._process = PROC_VNM;
        mid._unit = MU_VNM_SCHEDULE;
        
        MessageOption option(mid, EV_VNM_SCHEDULE_CC,0,0);
        m_pMU->Send(cMsg,option);
        
        return;
    }

SEND_ACK:

    //填写返回消息
    CVNetSelectCCListMsgAck cMsgAck(cMsg.action_id,
                                    iRet,
                                    0,
                                    GetVnetErrorInfo(iRet),
                                    vec_cc);
    cMsgAck.m_vm_id = cMsg.m_vm_id;
    cMsgAck.progress = (iRet == SUCCESS) ? 100 : 0;        
    VNET_SET_ACKMSG_REQUEST_INFO(cMsg, cMsgAck);
    MessageOption option(message.option.sender(),EV_VNETLIB_SELECT_CC_ACK,0,0);
    m_pMU->Send(cMsgAck,option);

    if (m_bOpenDbgInf)
    {
        cout << "CVNetVmMgr: Send SelectCCListForVM ack message:" <<endl;
        cout << cMsgAck.serialize();
    }

}

/************************************************************
* 函数名称： RcvSelectCCAckFromSchedule
* 功能描述： 处理从schedule收到CC调度列表应答消息
* 访问的表： 无
* 修改的表： 无
* 输入参数： vmniccfg
* 输出参数： cc_list
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1         V1.0       lvech         创建
***************************************************************/
void CVNetVmMgr::RcvSelectCCAckFromSchedule(const MessageFrame& message)
{
    CVNetSelectCCListMsgAck cMsgAck;    
    cMsgAck.deserialize(message.data);

    if (m_bOpenDbgInf)
    {
        cout << "CVNetVmMgr: Receive SelectCCListForVM ack message from schedule:" <<endl;
        cout << cMsgAck.serialize();
    }
   
    MessageOption option(m_cVMSchTaskInfo.m_cSenderMId,EV_VNETLIB_SELECT_CC_ACK,0,0);
    m_pMU->Send(cMsgAck,option);

    m_cVMSchTaskInfo.Reset();

    if (m_bOpenDbgInf)
    {
        cout << "CVNetVmMgr: Send SelectCCListForVM ack message to compute:" <<endl;
        cout << cMsgAck.serialize();
    }
}

/************************************************************
* 函数名称： GetNetResourceOnCC
* 功能描述： TC为VM选定部署CC后，向VNM申请分配网络资源的请求消息
* 访问的表： 无
* 修改的表： 无
* 输入参数： vmniccfg、HC信息
* 输出参数： pci信息
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1         V1.0       lvech         创建
***************************************************************/
void CVNetVmMgr::GetNetResourceOnCC(const MessageFrame& message)
{
    int32 iRet = SUCCESS; 
    vector<CVNetVnicConfig> vecVnicCfg;      //由VNM填写，分配的VLAN,MAC,IP等信息
    
    CVNetGetResourceCCMsg cMsg;
    cMsg.deserialize(message.data);

    vecVnicCfg = cMsg.m_VmNicCfgList;
    
    if (m_bOpenDbgInf)
    {
        cout << "CVNetVmMgr: Receive GetNetResourceOnCC message:" <<endl;
        cout << cMsg.serialize();
    }

    if (m_cVMSchTaskInfo.m_nVmID)
    {
        //任务正在执行
        if ((m_cVMSchTaskInfo.m_strActionID == cMsg.action_id) 
            && (VNM_VM_NETWORK_OPERATE_SELECT_CC == m_cVMSchTaskInfo.m_nCurrentOP))
        {
            iRet = ERROR_ACTION_RUNNING;
            goto SEND_ACK;
        }
        else
        {
            return;                
        }
    }
    else
    {        
        m_cVMSchTaskInfo.m_nVmID = cMsg.m_vm_id;
        m_cVMSchTaskInfo.m_nCurrentOP = VNM_VM_NETWORK_OPERATE_GET_RES_CC;
        m_cVMSchTaskInfo.m_strActionID = cMsg.action_id;
        m_cVMSchTaskInfo.m_cSenderMId = message.option.sender();

        //向schedule模块转发消息
        MID mid;
        mid._application = ApplicationName();
        mid._process = PROC_VNM;
        mid._unit = MU_VNM_SCHEDULE;
        
        MessageOption option(mid, EV_VNM_GET_RESOURCE_FOR_CC,0,0);
        m_pMU->Send(cMsg,option);
        
        return;
    }
    
SEND_ACK:

    CVNetGetResourceCCMsgAck cMsgAck;

    cMsgAck.m_vm_id = cMsg.m_vm_id;
    VNET_SET_ACKMSG_REQUEST_INFO(cMsg, cMsgAck);
    cMsgAck.state = iRet;
    cMsgAck.detail = GetVnetErrorInfo(cMsgAck.state);
    cMsgAck.progress = (iRet == SUCCESS) ? 100 : 0;        
    cMsgAck.action_id = cMsg.action_id;
    cMsgAck.m_VmNicCfgList = vecVnicCfg;

    MessageOption option(message.option.sender(),EV_VNETLIB_GETRESOURCE_CC_ACK,0,0);
    m_pMU->Send(cMsgAck,option);

    if (m_bOpenDbgInf)
    {
        cout << "CVNetVmMgr: Send GetNetResourceOnCC ack message:" <<endl;
        cout << cMsgAck.serialize();
    }
}

/************************************************************
* 函数名称： RcvGetResCCAckFromSchedule
* 功能描述： 处理从schedule收到TC资源分配应答消息
* 访问的表： 无
* 修改的表： 无
* 输入参数： vmniccfg
* 输出参数： cc_list
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1         V1.0       lvech         创建
***************************************************************/
void CVNetVmMgr::RcvGetResCCAckFromSchedule(const MessageFrame& message)
{
    CVNetGetResourceCCMsgAck cMsgAck;    
    cMsgAck.deserialize(message.data);

    if (m_bOpenDbgInf)
    {
        cout << "CVNetVmMgr: Receive GetNetResourceOnCC ack message:" <<endl;
        cout << cMsgAck.serialize();
    }

    MessageOption option(m_cVMSchTaskInfo.m_cSenderMId,EV_VNETLIB_GETRESOURCE_CC_ACK,0,0);
    m_pMU->Send(cMsgAck,option);

    m_cVMSchTaskInfo.Reset();

    if (m_bOpenDbgInf)
    {
        cout << "CVNetVmMgr: Send GetNetResourceOnCC ack message:" <<endl;
        cout << cMsgAck.serialize();
    }
}


/************************************************************
* 函数名称： FreeNetResourceOnCC
* 功能描述： TC向VNM申请为VM释放网络资源的请求消息
* 访问的表： 无
* 修改的表： 无
* 输入参数： vmniccfg、HC信息
* 输出参数： pci信息
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1         V1.0       lvech         创建
***************************************************************/
void CVNetVmMgr::FreeNetResourceOnCC(const MessageFrame& message)
{
    int32 iRet = 0;
    
    CVNetFreeResourceCCMsg cMsg;
    cMsg.deserialize(message.data);

    if (m_bOpenDbgInf)
    {
        cout << "CVNetVmMgr: Receive FreeNetResourceOnCC message:" <<endl;
        cout << cMsg.serialize();
    }

    if (m_cVMSchTaskInfo.m_nVmID)
    {
        //任务正在执行
        if ((m_cVMSchTaskInfo.m_strActionID == cMsg.action_id) 
            && (VNM_VM_NETWORK_OPERATE_SELECT_CC == m_cVMSchTaskInfo.m_nCurrentOP))
        {
            iRet = ERROR_ACTION_RUNNING;
            goto SEND_ACK;
        }
        else
        {
            return;                
        }
    }
    else
    {        
        m_cVMSchTaskInfo.m_nVmID = cMsg.m_vm_id;
        m_cVMSchTaskInfo.m_nCurrentOP = VNM_VM_NETWORK_OPERATE_FREE_RES_CC;
        m_cVMSchTaskInfo.m_strActionID = cMsg.action_id;
        m_cVMSchTaskInfo.m_cSenderMId = message.option.sender();

        //向schedule模块转发消息
        MID mid;
        mid._application = ApplicationName();
        mid._process = PROC_VNM;
        mid._unit = MU_VNM_SCHEDULE;

        MessageOption option(mid, EV_VNM_FREE_RESOURCE_FOR_CC,0,0);
        m_pMU->Send(cMsg,option);

        return;
    }


SEND_ACK:
    //计算要求撤销VM流程中失败情况不返回消息
    if (0 == iRet)
    {
        CVNetFreeResourceCCMsgAck cMsgAck;
        cMsgAck.m_vm_id = cMsg.m_vm_id;
        VNET_SET_ACKMSG_REQUEST_INFO(cMsg, cMsgAck);
        cMsgAck.state = iRet;
        cMsgAck.detail = GetVnetErrorInfo(cMsgAck.state);
        cMsgAck.progress = 100;   
        cMsgAck.action_id = cMsg.action_id;
        MessageOption option(message.option.sender(),EV_VNETLIB_FREERESOURCE_CC_ACK,0,0);
        m_pMU->Send(cMsgAck,option);
    
        if (m_bOpenDbgInf)
        {
            cout << "CVNetVmMgr: Send FreeNetResourceOnCC ack message:" <<endl;
            cout << cMsgAck.serialize();
        }
    }

}

/************************************************************
* 函数名称： RcvFreeResCCAckFromSchedule
* 功能描述： 处理从schedule收到TC资源释放应答消息
* 访问的表： 无
* 修改的表： 无
* 输入参数： vmniccfg
* 输出参数： cc_list
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1         V1.0       lvech         创建
***************************************************************/
void CVNetVmMgr::RcvFreeResCCAckFromSchedule(const MessageFrame& message)
{
    CVNetFreeResourceCCMsgAck cMsgAck;    
    cMsgAck.deserialize(message.data);

    if (m_bOpenDbgInf)
    {
        cout << "CVNetVmMgr: Receive FreeNetResourceOnCC ack message:" <<endl;
        cout << cMsgAck.serialize();
    }


    if (cMsgAck.state == SUCCESS)
    {
        MessageOption option(m_cVMSchTaskInfo.m_cSenderMId,EV_VNETLIB_FREERESOURCE_CC_ACK,0,0);
        m_pMU->Send(cMsgAck,option);

        if (m_bOpenDbgInf)
        {
            cout << "CVNetVmMgr: Send FreeNetResourceOnCC ack message:" <<endl;
            cout << cMsgAck.serialize();
        }
    }

    m_cVMSchTaskInfo.Reset();
}

/************************************************************
* 函数名称： SelectCCListForVM
* 功能描述： 处理CC向VNM获取满足VM部署条件的HC调度列表消息
* 访问的表： 无
* 修改的表： 无
* 输入参数： vmniccfg、CC信息
* 输出参数： hc_list
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1         V1.0       lvech         创建
***************************************************************/
void CVNetVmMgr::SelectHCListForVM(const MessageFrame& message)
{
    int32 iRet = 0;
    vector<string> vec_cc;
    CVNetSelectHCListMsgAck cMsgAck;
    CVNetSelectHCListMsg cMsg;
        
    cMsg.deserialize(message.data);

    if (m_bOpenDbgInf)
    {
        cout << "CVNetVmMgr: Receive CVNetSelectHCListMsg message:" <<endl;
        cout << cMsg.serialize();
    }

    if (m_cVMSchTaskInfo.m_nVmID)
    {
        //任务正在执行
        if ((m_cVMSchTaskInfo.m_strActionID == cMsg.action_id) 
            && (VNM_VM_NETWORK_OPERATE_SELECT_CC == m_cVMSchTaskInfo.m_nCurrentOP))
        {
            iRet = ERROR_ACTION_RUNNING;
            goto SEND_ACK;
        }
        else
        {
            return;                
        }
    }
    else
    {        
        m_cVMSchTaskInfo.m_nVmID = cMsg.m_vm_id;
        m_cVMSchTaskInfo.m_nCurrentOP = VNM_VM_NETWORK_OPERATE_SELECT_HC;
        m_cVMSchTaskInfo.m_strActionID = cMsg.action_id;
        m_cVMSchTaskInfo.m_cSenderMId = message.option.sender();

        //向schedule模块转发消息
        MID mid;
        mid._application = ApplicationName();
        mid._process = PROC_VNM;
        mid._unit = MU_VNM_SCHEDULE;
        
        MessageOption option(mid, EV_VNM_SCHEDULE_HC,0,0);
        m_pMU->Send(cMsg,option);
        
        return;
    }

SEND_ACK:

    cMsgAck.m_vm_id = cMsg.m_vm_id;
    VNET_SET_ACKMSG_REQUEST_INFO(cMsg, cMsgAck);
    cMsgAck.state = iRet;
    cMsgAck.detail = GetVnetErrorInfo(cMsgAck.state);
    cMsgAck.progress = (iRet == SUCCESS) ? 100 : 0;
    cMsgAck.action_id = cMsg.action_id;
    MessageOption option(message.option.sender(),EV_VNETLIB_SELECT_HC_ACK,0,0);
    m_pMU->Send(cMsgAck,option);

    if (m_bOpenDbgInf)
    {
        cout << "CVNetVmMgr: Send SelectHCListForVM ack message:" <<endl;
        cout << cMsgAck.serialize();
    }
}

/************************************************************
* 函数名称： RcvSelectHCAckFromSchedule
* 功能描述： 处理从schedule收到CC资源分配应答消息
* 访问的表： 无
* 修改的表： 无
* 输入参数： vmniccfg
* 输出参数： cc_list
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1         V1.0       lvech         创建
***************************************************************/
void CVNetVmMgr::RcvSelectHCAckFromSchedule(const MessageFrame& message)
{
    CVNetSelectHCListMsgAck cMsgAck;    
    cMsgAck.deserialize(message.data);

    if (m_bOpenDbgInf)
    {
        cout << "CVNetVmMgr: Receive CVNetSelectHCListMsg ack message:" <<endl;
        cout << cMsgAck.serialize();
    }

    MessageOption option(m_cVMSchTaskInfo.m_cSenderMId,EV_VNETLIB_SELECT_HC_ACK,0,0);
    m_pMU->Send(cMsgAck,option);

    m_cVMSchTaskInfo.Reset();

    if (m_bOpenDbgInf)
    {
        cout << "CVNetVmMgr: Send CVNetSelectHCListMsg ack message:" <<endl;
        cout << cMsgAck.serialize();
    }
}

/************************************************************
* 函数名称： GetNetResourceOnHC
* 功能描述： CC为VM选定部署HC后，向VNM申请分配PCI资源的请求消息
* 访问的表： 无
* 修改的表： 无
* 输入参数： vmniccfg、HC信息
* 输出参数： pci信息
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1         V1.0       lvech         创建
***************************************************************/
void CVNetVmMgr::GetNetResourceOnHC(const MessageFrame& message)
{
    int32 iRet = 0;
    string strVNAUuid;

    CVNetGetResourceHCMsg cMsg;
    cMsg.deserialize(message.data);

    if (m_bOpenDbgInf)
    {
        cout << "CVNetVmMgr: Receive GetNetResourceOnHC message:" <<endl;
        cout << cMsg.serialize();
    }

    if (m_cVMSchTaskInfo.m_nVmID)
    {
        //任务正在执行
        if ((m_cVMSchTaskInfo.m_strActionID == cMsg.action_id) 
            && (VNM_VM_NETWORK_OPERATE_SELECT_CC == m_cVMSchTaskInfo.m_nCurrentOP))
        {
            iRet = ERROR_ACTION_RUNNING;
            goto SEND_ACK;
        }
        else
        {
            return;                
        }
    }
    else
    {        
        m_cVMSchTaskInfo.m_nVmID = cMsg.m_vm_id;
        m_cVMSchTaskInfo.m_nCurrentOP = VNM_VM_NETWORK_OPERATE_GET_RES_HC;
        m_cVMSchTaskInfo.m_strActionID = cMsg.action_id;
        m_cVMSchTaskInfo.m_cSenderMId = message.option.sender();

        //向schedule模块转发消息
        MID mid;
        mid._application = ApplicationName();
        mid._process = PROC_VNM;
        mid._unit = MU_VNM_SCHEDULE;

        MessageOption option(mid, EV_VNM_GET_RESOURCE_FOR_HC,0,0);
        m_pMU->Send(cMsg,option);
        
        return;
    }
        

SEND_ACK:
    //向CC回应答消息
    CVNetGetResourceHCMsgAck cMsgAck;
    cMsgAck.m_vm_id = cMsg.m_vm_id;
    VNET_SET_ACKMSG_REQUEST_INFO(cMsg, cMsgAck);
    cMsgAck.state = iRet;   
    cMsgAck.detail = GetVnetErrorInfo(cMsgAck.state);
    cMsgAck.progress = (iRet == SUCCESS) ? 100 : 0;
    cMsgAck.action_id = cMsg.action_id;
    
    MessageOption cc_option(message.option.sender(), EV_VNETLIB_GETRESOURCE_HC_ACK,0,0);
    m_pMU->Send(cMsgAck,cc_option);

    if (m_bOpenDbgInf)
    {
        cout << "CVNetVmMgr: Send GetNetResourceOnHC ack message:" <<endl;
        cout << cMsgAck.serialize();
    }
}

/************************************************************
* 函数名称： RcvGetResHCAckFromSchedule
* 功能描述： 处理从schedule收到CC资源分配应答消息
* 访问的表： 无
* 修改的表： 无
* 输入参数： vmniccfg
* 输出参数： cc_list
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1         V1.0       lvech         创建
***************************************************************/
void CVNetVmMgr::RcvGetResHCAckFromSchedule(const MessageFrame& message)
{
    CVNetGetResourceHCMsgAck cMsgAck;    
    cMsgAck.deserialize(message.data);

    if (m_bOpenDbgInf)
    {
        cout << "CVNetVmMgr: Receive GetNetResourceOnHC ack message:" <<endl;
        cout << cMsgAck.serialize();
    }


    MessageOption option(m_cVMSchTaskInfo.m_cSenderMId,EV_VNETLIB_GETRESOURCE_HC_ACK,0,0);
    m_pMU->Send(cMsgAck,option);

    m_cVMSchTaskInfo.Reset();            

    if (m_bOpenDbgInf)
    {
        cout << "CVNetVmMgr: Send GetNetResourceOnHC ack message:" <<endl;
        cout << cMsgAck.serialize();
    }
}
/************************************************************
* 函数名称： FreeNetResourceOnHC
* 功能描述： CC向VNM申请为VM释放网络资源的请求消息
* 访问的表： 无
* 修改的表： 无
* 输入参数： vmniccfg、HC信息
* 输出参数： pci信息
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1         V1.0       lvech         创建
***************************************************************/
void CVNetVmMgr::FreeNetResourceOnHC(const MessageFrame& message)
{
    int32 iRet = 0;
    CVNetVmMem cVnetVmMem;
    
    CVNetFreeResourceHCMsg cMsg;
    cMsg.deserialize(message.data);

    if (m_bOpenDbgInf)
    {
        cout << "CVNetVmMgr: Receive FreeNetResourceOnHC message:" <<endl;
        cout << cMsg.serialize();
    }

    if (m_cVMSchTaskInfo.m_nVmID)
    {
        //任务正在执行
        if ((m_cVMSchTaskInfo.m_strActionID == cMsg.action_id) 
            && (VNM_VM_NETWORK_OPERATE_SELECT_CC == m_cVMSchTaskInfo.m_nCurrentOP))
        {
            iRet = ERROR_ACTION_RUNNING;
            goto SEND_ACK;
        }
        else
        {
            return;                
        }
    }
    else
    {        
        m_cVMSchTaskInfo.m_nVmID = cMsg.m_vm_id;
        m_cVMSchTaskInfo.m_nCurrentOP = VNM_VM_NETWORK_OPERATE_FREE_RES_HC;
        m_cVMSchTaskInfo.m_strActionID = cMsg.action_id;
        m_cVMSchTaskInfo.m_cSenderMId = message.option.sender();

        //向schedule模块转发消息
        MID mid;
        mid._application = ApplicationName();
        mid._process = PROC_VNM;
        mid._unit = MU_VNM_SCHEDULE;

        MessageOption option(mid, EV_VNM_FREE_RESOURCE_FOR_HC,0,0);
        m_pMU->Send(cMsg,option);
        
        return;
    }

SEND_ACK:
    //计算要求撤销VM流程中失败情况不返回消息
    if (0 == iRet)
    {
        //向CC回应答消息
        CVNetFreeResourceHCMsgAck cMsgAck;
        VNET_SET_ACKMSG_REQUEST_INFO(cMsg, cMsgAck);
        cMsgAck.m_vm_id = cMsg.m_vm_id;
        cMsgAck.state = iRet;   
        cMsgAck.detail = GetVnetErrorInfo(cMsgAck.state);
        cMsgAck.progress = (iRet == SUCCESS) ? 100 : 0;
        cMsgAck.action_id = cMsg.action_id;

        MessageOption cc_option(message.option.sender(),EV_VNETLIB_FREERESOURCE_HC_ACK,0,0);
        m_pMU->Send(cMsgAck,cc_option);

        if (m_bOpenDbgInf)
        {
            cout << "CVNetVmMgr: Send FreeNetResourceOnHC ack:" <<endl;
            cout << cMsgAck.serialize();
        }
    }
}

/************************************************************
* 函数名称： RcvFreeResHCAckFromSchedule
* 功能描述： 处理从schedule收到CC资源释放应答消息
* 访问的表： 无
* 修改的表： 无
* 输入参数： vmniccfg
* 输出参数： cc_list
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1         V1.0       lvech         创建
***************************************************************/
void CVNetVmMgr::RcvFreeResHCAckFromSchedule(const MessageFrame& message)
{
    CVNetFreeResourceHCMsgAck cMsgAck;    
    cMsgAck.deserialize(message.data);

    if (m_bOpenDbgInf)
    {
        cout << "CVNetVmMgr: Receive FreeNetResourceOnHC ack message:" <<endl;
        cout << cMsgAck.serialize();
    }

    if (cMsgAck.state == SUCCESS)
    {
        MessageOption option(m_cVMSchTaskInfo.m_cSenderMId,EV_VNETLIB_FREERESOURCE_HC_ACK,0,0);
        m_pMU->Send(cMsgAck,option);

        if (m_bOpenDbgInf)
        {
            cout << "CVNetVmMgr: Send FreeNetResourceOnHC ack message:" <<endl;
            cout << cMsgAck.serialize();
        }
    }
    
    m_cVMSchTaskInfo.Reset();  
}

/************************************************************
* 函数名称： DbgShowData
* 功能描述： 调试信息打印
* 访问的表： 无
* 修改的表： 无
* 输入参数： 
* 输出参数： 
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1         V1.0       lvech         创建
***************************************************************/
void CVNetVmMgr::DbgShowData()
{    cout << endl;    
    cout << "++++++++++++++++++++++VNM Schedule Task Information BEGIN ++++++++++++++++++++++++" << endl;

    m_cVMSchTaskInfo.Print();

    cout << "++++++++++++++++++++++VNM Schedule Task Information END ++++++++++++++++++++++++" << endl;
};

int VNM_DBG_SHOW_SCH_TASK_INFO(void)
{
    CVNetVmMgr *pVNetHandle = CVNetVmMgr::GetInstance();
    if (pVNetHandle)
    {
        pVNetHandle->DbgShowData();
        cout << endl;
    }
    return 0;
}
DEFINE_DEBUG_FUNC(VNM_DBG_SHOW_SCH_TASK_INFO);

void VNM_DBG_SET_VM_MGR_DBG_FLAG(int bSet)
{    
    if ((0 != bSet) && (1 != bSet))
    {
        cout << "Invalid flag parameter, please input 0 or 1." <<endl;
        return ;
    }

    CVNetVmMgr *pVmMgr = CVNetVmMgr::GetInstance();
    if (!pVmMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "SetModeDbgFlag call CVNetVmMgrExt::GetInstance() failed.\n");
        return;
    }

    pVmMgr->SetDbgFlag(bSet);
};
DEFINE_DEBUG_FUNC(VNM_DBG_SET_VM_MGR_DBG_FLAG);

}
