/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vna_vm_agent.cpp
* 文件标识：
* 内容摘要：CVNetVmAgent类实现文件
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
#include "registered_system.h"
#include "vm_messages.h"
#include "vna_common.h"
#include "dev_monitor.h"
#include "dev_mgr.h"
#include "dev_mon_reg_func.h"
#include "vnet_vnic.h"
#include "vna_switch_module.h"
#include "vna_switch_agent.h"
#include "vna_agent.h"
#include "vna_vnic_pool.h"
#include "vna_vm_agent.h"
#include "vna_vxlan_agent.h"
#include "vnet_function.h"

namespace zte_tecs
{
extern string g_strVNAApplication;

/************************************************************
* 函数名称： CVNetVmAgent
* 功能描述： CVNetVmAgent构造函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       lvech         创建
***************************************************************/
CVNetVmAgent::CVNetVmAgent()
{
    m_bOpenDbgInf = VNET_FALSE;
    m_pMU = NULL;
    m_pSwitchAgent = NULL;
    m_pEvbMgr = NULL;
    m_pVNAVnicPool = NULL;
};

/************************************************************
* 函数名称： ~CVNetVmAgent
* 功能描述： CVNetVmAgent析构函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       lvech         创建
***************************************************************/
CVNetVmAgent::~CVNetVmAgent()
{
    m_pMU = NULL;
    m_pVNetAgent = NULL;
    m_pSwitchAgent = NULL;
    m_pEvbMgr = NULL;
    m_pVNAVnicPool = NULL;
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
int32 CVNetVmAgent::Init(MessageUnit *mu)
{
    if (NULL == mu)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetVmAgent::Init: mu is NULL.\n");
        return -1;
    }
    
    m_pMU = mu;

    m_pVNetAgent =  CVNetAgent::GetInstance();
    if (NULL == m_pVNetAgent)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetVmAgent::Init: call CVNetAgent::GetInstance() failed.\n");
        return -1;
    }
    
    m_pSwitchAgent =  CSwitchAgent::GetInstance();
    if (NULL == m_pSwitchAgent)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetVmAgent::Init: call CVnetBrMgr::GetInstance() failed.\n");
        return -1;
    }

    m_pEvbMgr =  CEvbMgr::GetInstance();
    if (NULL == m_pEvbMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetVmAgent::Init: call CEvbMgr::GetInstance() failed.\n");
        return -1;
    }

    m_pVNAVnicPool =  CVNAVnicPool::GetInstance();
    if (NULL == m_pVNAVnicPool)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetVmAgent::Init: call CVNAVnicPool::GetInstance() failed.\n");
        return -1;
    }

    VNET_LOG(VNET_LOG_INFO, "CVNetVmAgent::Init: Call  successfully.\n");

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
void CVNetVmAgent::MessageEntry(const MessageFrame &message)
{
    //根据收到的消息id进行处理
    switch (message.option.id())
    {
        //HC向VNA申请相关网络资源
        case EV_VNETLIB_STARTNETWORK:
        {
            StartNetwork(message);
        }
        break;

        //HC向VNA释放相关网络资源
        case EV_VNETLIB_STOPNETWORK:
        {
            StopNetwork(message);
        }
        break;
        
        //HC向VNA通告VM的DOM ID变化情况
        case EV_VNETLIB_NOTIFY_DOMID:
        {
            NotifyDomID(message);
        }
        break;

        //VNM向VNA下发增加VM相关的VSI信息
        case EV_VNM_NOTIFY_VNA_ADD_VMVSI:
        {
            AddVMVSIInfo(message);
        }
        break;

        //VNM向VNA下发删除VM相关的VSI信息
        case EV_VNM_NOTIFY_VNA_DEL_VMVSI:
        {
            DelVMVSIInfo(message);
        }
        break;

        case EV_VNA_REQUEST_VMVSI_ACK:
        {
            RcvVsiReqAckFromVNM(message);
        }
        break;
        
        //VNA进程重启后，vnet_lib库向VNA下发所记录的所有已部署的VM相关的VSI信息
        case EV_VNETLIB_NOTIFY_ALL_VMNIC:
        {
            RcvAllVMVnicInfoFromHC(message);
        }
        break;

        //VNA进程重启后，VNM向VNA下发所记录的所有已部署的VM相关的VSI信息
        case EV_VNM_NOTIFY_VNA_ALL_VMVSI:
        {
            RcvAllVMVSIInfoFromVNM(message);
        }
        break;

        //定时检查VM新生成的端口并进行相关设置
        case EV_VNA_CHECK_VM_VSI_INFO:
        {
            m_pVNAVnicPool->CheckVMVSIInfo();
        }
        break;

        //从WDG收到看门狗操作应答消息
        case  EV_VNETLIB_WDG_OPER_ACK:
        {
            RcvWdgOperAckMsgFromWdg(message);
        }
        break;

        //从计算收到看门狗定时器操作消息
        case  EV_VNETLIB_WATCHDOG_OPER:
        {
            RcvWdgTimerOperMsgFromHC(message);
        }
        break;

        //从wdg收到看门狗定时器操作应答消息
        case  EV_VNETLIB_WDG_TIMER_OPER_ACK:
        {
            RcvWdgTimerOperAckMsgFromWdg(message);
        }
        break;

        //从wdg收到重启VM消息
        case  EV_VNETLIB_NOTIFY_VM_RR:
        {
            RcvVMRestartMsgFromWDG(message);
        }
        break;

        //从计算收到重启VM应答消息
        case  EV_VNETLIB_WATCHDOG_NOTIFY_HC_RESTART_ACK:
        {
            RcvVMRestartAckMsgFromHC(message);
        }
        break;
    
        default:
            break;
    }

}

/************************************************************
* 函数名称： StartNetwork
* 功能描述： HC向VNA申请相关网络资源
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
void CVNetVmAgent::StartNetwork(const MessageFrame& message)
{
    int32 iRet = 0;

    CVNetStartNetworkMsg cMsg;
    cMsg.deserialize(message.data);

    if (m_bOpenDbgInf)
    {
        cout << "Reveive CVNetStartNetworkMsg message:\n" << endl;  
        cMsg.Print();
    }
        
    //查询内存中是否有对应VNIC配置，没有则需要向VNM请求对应的VSI详细信息
    CVNAVMVnicInfo *pVMVnicInfo;
    pVMVnicInfo = m_pVNAVnicPool->AddVMVnicInfo(cMsg);
    //如果已有VSI详细信息则继续部署流程
    if (VNA_VM_GET_VSI_STATUS_OK(pVMVnicInfo) \
      && VNA_VM_WDG_STATUS_OK(pVMVnicInfo) \
      && VNA_VM_EVB_STATUS_OK(pVMVnicInfo))
    {
        iRet = StartNetworkProc(*pVMVnicInfo);
    }
    //VM部署相关信息还没有准备好
    else
    {
        if (VNA_VM_GET_VSI_STATUS_NOT_OK(pVMVnicInfo))
        {
            string strVnmName;
            //如果没有注册信息，直接返回失败
            if (ERROR == GetRegisteredSystem(strVnmName))
            {
                VNET_LOG(VNET_LOG_ERROR, "CVNetVmAgent::StartNetwork: call GetRegisteredSystem failed.\n");
                pVMVnicInfo->SetQueryVSIStatus(VNA_GET_VSI_DETAIL_STATUS_FAIL);
                iRet = ERROR_VNA_VNIC_GET_REGED_VNM_FAIL;
                goto SEND_ACK;
            }
                
            //向VNM请求VSI详细信息
            CVMVSIInfoReqMsg cVSIReqMsg;
            cVSIReqMsg.m_nVMID = cMsg.m_vm_id;
            cVSIReqMsg.m_vecVnicCfg = cMsg.m_VmNicCfgList;
                
            MID receiver;
            receiver._application = strVnmName;
            receiver._process = PROC_VNM;
            receiver._unit = MU_VNM;
            MessageOption option(receiver, EV_VNA_REQUEST_VMVSI, 0, 0);
            m_pMU->Send(cVSIReqMsg, option);

            pVMVnicInfo->SetQueryVSITimes(pVMVnicInfo->GetQueryVSITimes() + 1);

            if (m_bOpenDbgInf)
            {
                cout << "Send CVMVSIInfoReqMsg message:\n" << endl;  
                cVSIReqMsg.Print();
            }
        }
        
        //开启看门狗还没有应答
        if (VNA_VM_WDG_STATUS_NOT_OK(pVMVnicInfo))
        {
            //向看门狗发送消息时，需要把本次action_id带上，看门狗回应答消息时把action_id带回来
            //这样可以检查这个而应答消息是否是本次任务的应答消息，否则丢弃          
            TModuleInfo cModule;
            iRet = m_pVNetAgent->GetModuleInfo(VNA_MODULE_ROLE_WATCHDOG, cModule);
            if (0 != iRet)
            {
                VNET_LOG(VNET_LOG_ERROR, "CVNetVmAgent::StartNetwork: call GetModuleInfo(WDG) failed.ret=%d\n", iRet);
                pVMVnicInfo->SetWdgOperStatus(VNA_VM_WDG_OP_STATUS_FAIL);
                goto SEND_ACK;
            }

            CVNetWdgMsg cWdgMsg;
            cWdgMsg.action_id = cMsg.action_id;
            cWdgMsg.m_qwVmId = cMsg.m_vm_id;
            cWdgMsg.m_nTimerOut = cMsg.m_WatchDogInfo.m_nTimeOut;
            cWdgMsg.m_nOperator = EN_START_WDG;
            
            MessageOption option(cModule.GetModuleLibMid(), EV_VNETLIB_WDG_OPER, 0, 0);
            m_pMU->Send(cWdgMsg, option);

            pVMVnicInfo->SetWdgOperTimes(pVMVnicInfo->GetWdgOperTimes() + 1);

            if (m_bOpenDbgInf)
            {
                cout << "Send CVNetWdgMsg message:\n" << endl;  
                cWdgMsg.Print();
            }
        }

        // evb tlv 必须等待vsi 准备就绪才能执行
        if (VNA_VM_GET_VSI_STATUS_OK(pVMVnicInfo))
        {
            if (VNA_VM_VSI_EVB_STATUS_NOT_OK(pVMVnicInfo))
            {
                // 继续查询vsi evb status 
                int32 noNeedEvbTlv = FALSE;
                // 如果不是hc触发的，无需查看evb tlv协商结果
                if ("" == pVMVnicInfo->GetActionID())
                {
                    noNeedEvbTlv = TRUE;
                }                
                iRet = StartEvbProc(*pVMVnicInfo,noNeedEvbTlv);
    
                if( pVMVnicInfo->GetQueryVSIEvbStatus() == VNA_VM_VSI_EVB_STATUS_SUCCESS )
                {
                    // 其他资源还需要调用 pci 
    
                    // 判断下是否需要执行 startnetworkproc
                    if (VNA_VM_GET_VSI_STATUS_OK(pVMVnicInfo) \
                      && VNA_VM_WDG_STATUS_OK(pVMVnicInfo) \
                      && VNA_VM_EVB_STATUS_OK(pVMVnicInfo))
                    {
                        iRet = StartNetworkProc(*pVMVnicInfo);
                        goto SEND_ACK;
                    }
                }
    
                if( pVMVnicInfo->GetQueryVSIEvbStatus() == VNA_VM_VSI_EVB_STATUS_FAILED)
                {                
                    // 将vm vnic 部署状态置为FAIL
                    pVMVnicInfo->SetDeployStatus(VNA_VM_DEPLOY_STATUS_NETWORK_START_FAIL);
                    iRet = ERROR_VNA_VNIC_VSI_EVB_STATUS_FAIL;
                    goto SEND_ACK;
                }            
            }
        }

        return;
    }

SEND_ACK:
    //注意考虑迁移流程
    CVNetStartNetworkMsgAck cAck;
    cAck.m_vmid =  cMsg.m_vm_id;
    cAck.action_id =  cMsg.action_id;
    cAck.state = iRet;
    cAck.detail = GetVnetErrorInfo(cAck.state);
    cAck.progress = (cAck.state == SUCCESS) ? 100 : 0;
    VNET_SET_ACKMSG_REQUEST_INFO(cMsg, cAck);

    if (0 == iRet)
    {
        //获取返回给HC相关信息
        pVMVnicInfo->GetVnicXMLInfo(cAck.m_vsiinfo);
        pVMVnicInfo->GetWdgVnicXMLInfo(cAck.m_wdg_info);
        pVMVnicInfo->SetDeployStatus(VNA_VM_DEPLOY_STATUS_NETWORK_RUNNING);
        pVMVnicInfo->SetActionID("");
    }
    else
    {
        cout <<"CVNetVmAgent::StartNetwork:" <<iRet <<endl;

        pVMVnicInfo->SetDeployStatus(VNA_VM_DEPLOY_STATUS_NETWORK_START_FAIL);
    }

    MessageOption option(message.option.sender(), EV_VNETLIB_STARTNETWORK_ACK,0,0);
    m_pMU->Send(cAck,option);

    if (m_bOpenDbgInf)
    {
        cout << "Send CVNetStartNetworkMsgAck message:\n" << endl;  
        cAck.Print();
    }
}

/************************************************************
* 函数名称： StartNetworkProc
* 功能描述： StartNetwork处理流程
* 访问的表： 无
* 修改的表： 无
* 输入参数： 
* 输出参数： 
* 返   回   值： 无
* 其它说明：  调用此函数的前提条件是VNA已获得
                                 的该VM相关的VSI详细信息
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1         V1.0       lvech         创建
***************************************************************/
int32 CVNetVmAgent::StartNetworkProc(CVNAVMVnicInfo& cVMVnicInfo)
{
    int32 iRet = 0;

    vector<CVNetVnicDetail> vecVnicInfo;
    cVMVnicInfo.GetVnicDetailInfo(vecVnicInfo);

    //检查网桥和PCI设备是否存在
    iRet = m_pSwitchAgent->AllocDVSRes(vecVnicInfo);
    if (0 != iRet)
    {
       VNET_LOG(VNET_LOG_ERROR, 
            "CVNetVmAgent::StartNetworkProc: call AllocDVSRes failed, ret: %d.\n", iRet);
       return ERROR_VNA_VNIC_ALLOC_DVS_FAIL;
    }

    return 0;
}

void CVNetVmAgent::CheckEvbStatus(CVNAVMVnicInfo* pVMVnicInfo)
{
    if(NULL == pVMVnicInfo)
    {
        return;
    }

    // evb tlv 必须等待vsi 准备就绪才能执行
    if (!VNA_VM_GET_VSI_STATUS_OK(pVMVnicInfo))
    {
        return;
    }

    int32 noNeedEvbTlv = FALSE;
    // 如果不是hc触发的，无需查看evb tlv协商结果
    if ("" == pVMVnicInfo->GetActionID())
    {
        noNeedEvbTlv = TRUE;
    }
    
    int32 iRet = 0;
    time_t cur_time = time(0);

    // 目前只有startnetwork 需要查询evb status 
    if (VNA_VM_DEPLOY_STATUS_NETWORK_STARTING == pVMVnicInfo->GetDeployStatus())
    {
        if (VNA_VM_VSI_EVB_STATUS_NOT_OK(pVMVnicInfo))
        {
            pVMVnicInfo->SetQueryVSIEvbLastTime(cur_time);
            pVMVnicInfo->SetQueryVSIEvbTimes(pVMVnicInfo->GetQueryVSIEvbTimes() + 1);

            // 继续查询vsi evb status 
            iRet = StartEvbProc(*pVMVnicInfo,noNeedEvbTlv);
        
            if( pVMVnicInfo->GetQueryVSIEvbStatus() == VNA_VM_VSI_EVB_STATUS_SUCCESS )
            {
                // 其他资源还需要调用 pci 
        
                // 判断下是否需要执行 startnetworkproc
                if (VNA_VM_GET_VSI_STATUS_OK(pVMVnicInfo) \
                  && VNA_VM_WDG_STATUS_OK(pVMVnicInfo) \
                  && VNA_VM_EVB_STATUS_OK(pVMVnicInfo))
                {
                    iRet = StartNetworkProc(*pVMVnicInfo);
                    // 更新vm vnic 部署状态 
                    if (0 == iRet)
                    {
                        pVMVnicInfo->SetDeployStatus(VNA_VM_DEPLOY_STATUS_NETWORK_RUNNING);
                    }
                    else
                    {
                        pVMVnicInfo->SetDeployStatus(VNA_VM_DEPLOY_STATUS_NETWORK_START_FAIL);
                    }    
            
                    goto SEND_ACK;
                }
            }
            else if( pVMVnicInfo->GetQueryVSIEvbStatus() == VNA_VM_VSI_EVB_STATUS_FAILED)
            {                
                // 将vm vnic 部署状态置为FAIL
                pVMVnicInfo->SetDeployStatus(VNA_VM_DEPLOY_STATUS_NETWORK_START_FAIL);
                iRet = ERROR_VNA_VNIC_VSI_EVB_STATUS_FAIL;
                goto SEND_ACK;
            }            
        }
    }
    return;

SEND_ACK:
    // 如果不是hc触发的，无需回应答
    if ("" == pVMVnicInfo->GetActionID())
    {
        return;
    }

    //向HC发送startnetwork应答消息
    TModuleInfo cModule;
    int32 hcRet = m_pVNetAgent->GetModuleInfo(VNA_MODULE_ROLE_HC, cModule);
    if (0 != hcRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetVmAgent::AddVMVSIInfo: call GetModuleInfo failed.\n");
        return;
    }
    
    CVNetStartNetworkMsgAck cAck;
    cAck.m_vmid =  pVMVnicInfo->GetVMID();
    cAck.m_mid = pVMVnicInfo->GetDeployMID();
    cAck.action_id =  pVMVnicInfo->GetActionID();
    MID mid;
    pVMVnicInfo->SetDeployMID(mid);
    pVMVnicInfo->SetActionID("");
    cAck.state = iRet;
    cAck.detail = GetVnetErrorInfo(cAck.state);
    cAck.progress = (iRet == SUCCESS) ? 100 : 0;

    if (0 == iRet)
    {
        //获取返回给HC相关信息
        pVMVnicInfo->GetVnicXMLInfo(cAck.m_vsiinfo);
        pVMVnicInfo->GetWdgVnicXMLInfo(cAck.m_wdg_info);
    }

    MessageOption option(cModule.GetModuleLibMid(), EV_VNETLIB_STARTNETWORK_ACK,0,0);
    m_pMU->Send(cAck,option);

    if (m_bOpenDbgInf)
    {
        cout << "Send CVNetStartNetworkMsgAck message:\n" << endl;  
        cAck.Print();
    }
    return;
}


int32 CVNetVmAgent::StartEvbProc(CVNAVMVnicInfo& cVMVnicInfo, int32 noNeedEvbTlv)
{
    int32 iRet = 0;

    if (m_bOpenDbgInf)
    {
        cout << "call StartEvbProc function:\n" << endl;  
        cout << "noNeedEvbTlv : " << noNeedEvbTlv << endl;
        cVMVnicInfo.Print();
    }

    vector<CVNetVnicDetail> vecVnicInfo;
    cVMVnicInfo.GetVnicDetailInfo(vecVnicInfo);

    if (m_bOpenDbgInf)
    {
        cout << "cVMVnicInfo.GetDeployOption() : " << cVMVnicInfo.GetDeployOption() << endl;
    }

    iRet = m_pEvbMgr->StartVPortDiscovery(cVMVnicInfo.GetVMID(), 
                                    vecVnicInfo, 
                                    cVMVnicInfo.GetDeployOption(), noNeedEvbTlv);

    // 将evb status 更新
    if( iRet == START_VPORT_DISCOVERY_RUNNING )
    {
        if (m_bOpenDbgInf)
        {
            cout << "call StartEvbProc function return values : VNA_VM_VSI_EVB_STATUS_RUNNING" << endl;  
        }
    
        cVMVnicInfo.SetQueryVSIEvbStatus(VNA_VM_VSI_EVB_STATUS_RUNNING);
        return iRet;
    }
    
    if( iRet == START_VPORT_DISCOVERY_SUCCESS )
    {
        if (m_bOpenDbgInf)
        {
            cout << "call StartEvbProc function return values : VNA_VM_VSI_EVB_STATUS_SUCCESS" << endl;  
        }
        
        // 其他资源还需要调用 pci 
        cVMVnicInfo.SetQueryVSIEvbStatus(VNA_VM_VSI_EVB_STATUS_SUCCESS);
        return iRet;
    }
    else
    {
        if (m_bOpenDbgInf)
        {
            cout << "call StartEvbProc function return values : VNA_VM_VSI_EVB_STATUS_FAILED" << endl;  
        }
        
        cVMVnicInfo.SetQueryVSIEvbStatus(VNA_VM_VSI_EVB_STATUS_FAILED);
        VNET_LOG(VNET_LOG_ERROR, 
            "CVNetVmAgent::StartEvbProc: call StartVPortDiscovery failed, ret: %d.\n", iRet);
        return iRet;
    }            
    return 0;
}

int32 CVNetVmAgent::StopEvbProc(CVNAVMVnicInfo& cVMVnicInfo)
{
    int32 iRet = 0;

    if (m_bOpenDbgInf)
    {
        cout << "call StopEvbProc function:\n" << endl;  
        cVMVnicInfo.Print();
    }
    
    vector<CVNetVnicDetail> vecVnicInfo;
    cVMVnicInfo.GetVnicDetailInfo(vecVnicInfo);

    if (m_bOpenDbgInf)
    {
        cout << "cVMVnicInfo.GetDeployOption() : " << cVMVnicInfo.GetDeployOption() << endl;
    }

    iRet = m_pEvbMgr->StopVPortDiscovery(cVMVnicInfo.GetVMID(), 
                                    vecVnicInfo, 
                                    cVMVnicInfo.GetDeployOption());

    if(START_VPORT_DISCOVERY_RUNNING == iRet )
    {        
        if (m_bOpenDbgInf)
        {
            cout << "call StopEvbProc function return values : VNA_VM_VSI_EVB_STATUS_SUCCESS" << endl;  
        }
        
        cVMVnicInfo.SetQueryVSIEvbStatus(VNA_VM_VSI_EVB_STATUS_RUNNING);
        return iRet;
    }

    // 将evb status 更新
    if( iRet == START_VPORT_DISCOVERY_SUCCESS )
    {
        if (m_bOpenDbgInf)
        {
            cout << "call StopEvbProc function return values : VNA_VM_VSI_EVB_STATUS_SUCCESS" << endl;  
        }
        
        cVMVnicInfo.SetQueryVSIEvbStatus(VNA_VM_VSI_EVB_STATUS_SUCCESS);
        return iRet;
    }
    else
    {
        if (m_bOpenDbgInf)
        {
            cout << "call StopEvbProc function return value : VNA_VM_VSI_EVB_STATUS_FAILED" << endl;  
        }
        
        cVMVnicInfo.SetQueryVSIEvbStatus(VNA_VM_VSI_EVB_STATUS_FAILED);
       VNET_LOG(VNET_LOG_ERROR, 
            "CVNetVmAgent::StopEvbProc: call StartVPortDiscovery failed, ret: %d.\n", iRet);
       return iRet;
    }
    return 0;
}

/************************************************************
* 函数名称： StopNetwork
* 功能描述： HC向VNA释放相关网络资源
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
void CVNetVmAgent::StopNetwork(const MessageFrame& message)
{
    int32 iRet = 0;
    int32 evbRet = 0;
    CVnicPortDev portdev;
    vector<CVNetVnicDetail> vecVnicInfo;
    vector<CVnicPortDev> vecVnicPortDev;
    vector<CVnicPortDev>::iterator iterVnicPort;
    
    CVNetStopNetworkMsg cMsg;
    cMsg.deserialize(message.data);
    
    if (m_bOpenDbgInf)
    {
        cout << "Reveive CVNetStopNetworkMsg message:\n" << endl;  
        cMsg.Print();
    }
    
    //查询内存中VSI详细信息
    CVNAVMVnicInfo* pVMVnicInfo = NULL;
    pVMVnicInfo = m_pVNAVnicPool->GetVMVSIInfo(cMsg.m_vm_id);
    if (NULL == pVMVnicInfo)
    {
        iRet = 0;   //可重入要求
        goto SEND_ACK;
    }

    //检查当前状态
    if (VNA_VM_DEPLOY_STATUS_NETWORK_STOP_FAIL == pVMVnicInfo->GetDeployStatus())
    {
        //上次stop失败，本次重试，需要先清除上次操作信息
        pVMVnicInfo->SetDeployStatus(VNA_VM_DEPLOY_STATUS_NETWORK_STOPING);        

        if (VNA_VM_WDG_ENABLE_YES == pVMVnicInfo->GetIsWdgEnable())
        {
            pVMVnicInfo->SetWdgOperStatus(VNA_VM_WDG_OP_STATUS_INIT);
            pVMVnicInfo->SetWdgOperTimes(0);
        }

        // 需将EVB清除
        pVMVnicInfo->SetQueryVSIEvbStatus(VNA_VM_VSI_EVB_STATUS_INIT);
        pVMVnicInfo->SetQueryVSIEvbLastTime(0);
        pVMVnicInfo->SetQueryVSIEvbTimes(0);
    }
    else if (VNA_VM_DEPLOY_STATUS_NETWORK_INIT == pVMVnicInfo->GetDeployStatus())
    {
        //已stop成功，本次直接返回成功
        iRet = 0;   //可重入要求
        goto SEND_ACK;
    }
    else if (VNA_VM_DEPLOY_STATUS_NETWORK_STOPING == pVMVnicInfo->GetDeployStatus())
    {
        //正在撤销
        iRet = ERROR_ACTION_RUNNING;   //可重入要求
        goto SEND_ACK;
    }

    pVMVnicInfo->GetVnicDetailInfo(vecVnicInfo);

    //检查网桥和PCI设备是否存在
    iRet = m_pSwitchAgent->FreeDVSRes(vecVnicInfo);
    if (0 != iRet)
    {
        VNET_LOG(VNET_LOG_ERROR, 
            "CVNetVmAgent::StopNetwork: call FreeDVSRes failed, ret: %d.\n", iRet);
        iRet = ERROR_VNA_VNIC_FREE_DVS_FAIL;
        goto SEND_ACK;
    }
    
    // 释放已分配的evb资源
    evbRet = StopEvbProc(*pVMVnicInfo);
    if( pVMVnicInfo->GetQueryVSIEvbStatus() == VNA_VM_VSI_EVB_STATUS_FAILED )
    {
        iRet = ERROR_VNA_VNIC_VSI_EVB_STATUS_FAIL;
        VNET_LOG(VNET_LOG_WARN, 
            "CVNetVmAgent::StopNetwork: call StopVPortDiscovery(vm_id:%d, option:%d) failed, ret: %d.\n", 
            cMsg.m_vm_id, cMsg.m_option, iRet);
        goto SEND_ACK;
    }
    
    //更新vepa 流表信息
    pVMVnicInfo->GetVnicInfo(vecVnicPortDev);
        
    iterVnicPort = vecVnicPortDev.begin();
    for ( ; iterVnicPort != vecVnicPortDev.end(); ++iterVnicPort)
    {
        portdev = (CVnicPortDev)(*iterVnicPort);
        CheckVepaFlowOfPort(&portdev, 1);
    }

    if (VNA_VM_DEPLOY_STATUS_NETWORK_INIT != pVMVnicInfo->GetDeployStatus())
    {
        if (VNA_VM_WDG_ENABLE_YES == pVMVnicInfo->GetIsWdgEnable())
        {
            pVMVnicInfo->SetActionID(cMsg.action_id);
            pVMVnicInfo->SetDeployOption(cMsg.m_option);
            pVMVnicInfo->SetDeployStatus(VNA_VM_DEPLOY_STATUS_NETWORK_STOPING);
            pVMVnicInfo->SetDeployMID(cMsg.m_mid);

            //向WDG模块发送关闭WDG消息
            TModuleInfo cModule;
            iRet = m_pVNetAgent->GetModuleInfo(VNA_MODULE_ROLE_WATCHDOG, cModule);
            if (0 != iRet)
            {
                VNET_LOG(VNET_LOG_ERROR, "CVNetVmAgent::StartNetwork: call GetModuleInfo failed.\n");
                goto SEND_ACK;
            }
            
            CVNetWdgMsg cWdgMsg;
            cWdgMsg.action_id = cMsg.action_id;
            cWdgMsg.m_qwVmId = cMsg.m_vm_id;
            cWdgMsg.m_nOperator = EN_STOP_WDG;
            
            MessageOption option(cModule.GetModuleLibMid(), EV_VNETLIB_WDG_OPER, 0, 0);
            m_pMU->Send(cWdgMsg, option);

            if (m_bOpenDbgInf)
            {
                cout << "Send CVNetWdgMsg message:\n" << endl;  
                cWdgMsg.Print();
            }
            
            return;
        }
        else
        {
            pVMVnicInfo->SetDeployStatus(VNA_VM_DEPLOY_STATUS_NETWORK_INIT);
            pVMVnicInfo->SetWdgOperStatus(VNA_VM_WDG_OP_STATUS_INIT);
        }
    }              

SEND_ACK:

    if (0 == iRet)
    {
        //清除相关VSI信息
        m_pVNAVnicPool->DelVMVSIInfo(cMsg.m_vm_id);
    }
    
    //计算要求撤销VM流程中失败情况不返回消息
    if ((0 == iRet) || (ERROR_ACTION_RUNNING == iRet))
    {
        CVNetStopNetworkMsgAck cMsgAck;
        VNET_SET_ACKMSG_REQUEST_INFO(cMsg, cMsgAck);
        cMsgAck.m_vmid =  cMsg.m_vm_id;
        cMsgAck.action_id =  cMsg.action_id;
        cMsgAck.state = iRet;
        cMsgAck.detail = GetVnetErrorInfo(cMsgAck.state);
        cMsgAck.progress = (cMsgAck.state == SUCCESS) ? 100 : 0;

        MessageOption option(message.option.sender(),EV_VNETLIB_STOPNETWORK_ACK,0,0);
        m_pMU->Send(cMsgAck,option);

        if (m_bOpenDbgInf)
        {
            cout << "Send CVNetStopNetworkMsgAck message:\n" << endl;  
            cMsgAck.Print();
        }
    }
}
        
/************************************************************
* 函数名称： NotifyDomID
* 功能描述： HC向VNA通告VM的DOM ID变化情况
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
void CVNetVmAgent::NotifyDomID(const MessageFrame& message)
{
    int32 iRet = 0;

    CVNetNotifyDomIDMsg cMsg;
    cMsg.deserialize(message.data);

    if (m_bOpenDbgInf)
    {
        cout << "Reveive CVNetNotifyDomIDMsg message:\n" << endl;  
        cMsg.Print();
    }
    
    //更新内存中vnic的domid
    m_pVNAVnicPool->UpdateDomID(cMsg.m_vm_id, cMsg.m_dom_id);

    // 设置external-id, update quantum port
    SetExternalId(cMsg.m_vm_id, cMsg.m_dom_id);
    
    //返回消息
    CVNetNotifyDomIDMsgAck cMsgAck;    
    cMsgAck.action_id =  cMsg.action_id;
    VNET_SET_ACKMSG_REQUEST_INFO(cMsg, cMsgAck);
    cMsgAck.state = iRet;
    cMsgAck.detail = GetVnetErrorInfo(cMsgAck.state);
    cMsgAck.progress = (cMsgAck.state == SUCCESS) ? 100 : 0;
  
    MessageOption option(message.option.sender(), EV_VNETLIB_NOTIFY_DOMID_ACK,0,0);
        
    m_pMU->Send(cMsgAck,option);

    if (m_bOpenDbgInf)
    {
        cout << "Send CVNetNotifyDomIDMsgAck message:\n" << endl;  
        cMsg.Print();
    }
}

void CVNetVmAgent::SetExternalId(const int64 Vmid, const int Domid)
{
    vector<CVNetVnicDetail> vecVnicDetail;
    CVNAVMVnicInfo* pVnicInfo = m_pVNAVnicPool->GetVMVSIInfo(Vmid);
    ostringstream vif,tap,vifemu;
    stringstream strVmId;
    int i = 0;

    if(NULL == pVnicInfo)
    {
        return;
    }

    strVmId << Vmid;

    pVnicInfo->GetVnicDetailInfo(vecVnicDetail);

    vector<CVNetVnicDetail>::iterator itr = vecVnicDetail.begin();
    for( ; itr != vecVnicDetail.end(); ++itr)
    {
        if(!itr->GetIsSriov())
        {
            if(!(itr->GetQuantumPortUuid().empty()))
            {
                string strMacAddress;
                if (0 != itr->GetAssMac().size())
                {
                    strMacAddress = itr->GetAssMac();
                }
                else
                {
                    strMacAddress = itr->GetMacAddress();
                }
                
                vif.str("");
                vif<< "vif" << Domid << "." << i;
                if(vnet_chk_dev(vif.str()))
                {
                    if(VNET_PLUGIN_SUCCESS != VNetSetExternalid(itr->GetSwitchName(), vif.str(), itr->GetQuantumPortUuid(), strMacAddress, strVmId.str()))
                    {
                        VNET_LOG(VNET_LOG_ERROR, "CVNetVmAgent::SetExternalId: Setting SDVS external-id failed.\n"); 
                        continue;
                    }
                }

                tap.str("");
                tap<< "tap" << Domid << "." << i;
                if(vnet_chk_dev(tap.str()))
                {
                    if(VNET_PLUGIN_SUCCESS != VNetSetExternalid(itr->GetSwitchName(), tap.str(), itr->GetQuantumPortUuid(), strMacAddress, strVmId.str()))
                    {
                        VNET_LOG(VNET_LOG_ERROR, "CVNetVmAgent::SetExternalId: Setting SDVS external-id failed.\n"); 
                        continue;
                    }
                }

                vifemu.str("");
                vifemu<< "vif" << Domid << "." << i << "-emu";
                if(vnet_chk_dev(vifemu.str()))
                {
                    if(VNET_PLUGIN_SUCCESS != VNetSetExternalid(itr->GetSwitchName(), vifemu.str(), itr->GetQuantumPortUuid(), strMacAddress, strVmId.str()))
                    {
                        VNET_LOG(VNET_LOG_ERROR, "CVNetVmAgent::SetExternalId: Setting SDVS external-id failed.\n"); 
                        continue;
                    }
                }
                
            }

            i++;
        }
    }
}

/************************************************************
* 函数名称： AddVMVSIInfo
* 功能描述： 处理VNM向VNA下发增加VM相关的VSI信息
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
void CVNetVmAgent::AddVMVSIInfo(const MessageFrame& message)
{
    int32 iRet = 0;

    CAddVMVSIInfoMsg cMsg;
    cMsg.deserialize(message.data);

    if (m_bOpenDbgInf)
    {
        cout << "Receive CAddVMVSIInfoMsg message:\n" << endl;  
        cMsg.Print();
    }
    
    //增加或者更新VMVSI信息
    CVNAVMVnicInfo* pVMVnicInfo = NULL;
    pVMVnicInfo = m_pVNAVnicPool->AddVMVSIInfo(cMsg.m_cVMVSIInfo);

    //检查是否有startnetwork部署流程在等待
    if (VNA_VM_DEPLOY_STATUS_NETWORK_STARTING == pVMVnicInfo->GetDeployStatus()) 
    {
        iRet = StartNetworkProc(*pVMVnicInfo);
        if ((0 != iRet) || \
           ( VNA_VM_WDG_STATUS_OK(pVMVnicInfo) && \
             VNA_VM_EVB_STATUS_OK(pVMVnicInfo)) )
        {
            //重置VM网络部署状态
            if (0 != iRet)
            {
                pVMVnicInfo->SetDeployStatus(VNA_VM_DEPLOY_STATUS_NETWORK_INIT);
            }
            else
            {
                pVMVnicInfo->SetDeployStatus(VNA_VM_DEPLOY_STATUS_NETWORK_RUNNING);
            }

            if ("" == pVMVnicInfo->GetActionID())
            {
                return;
            }
                    
            //向HC发送startnetwork应答消息
            TModuleInfo cModule;
            iRet = m_pVNetAgent->GetModuleInfo(VNA_MODULE_ROLE_HC, cModule);
            if (0 != iRet)
            {
                VNET_LOG(VNET_LOG_ERROR, "CVNetVmAgent::AddVMVSIInfo: call GetModuleInfo failed.\n");
                return;
            }
                
            CVNetStartNetworkMsgAck cAck;
            cAck.m_vmid =  pVMVnicInfo->GetVMID();
            cAck.action_id = pVMVnicInfo->GetActionID();
            cAck.state = iRet;
            cAck.detail = GetVnetErrorInfo(cAck.state);
            cAck.progress = (cAck.state == SUCCESS) ? 100 : 0;
            cAck.m_mid = pVMVnicInfo->GetDeployMID();

            if (0 == iRet)
            {
                //获取返回给HC相关信息
                pVMVnicInfo->GetVnicXMLInfo(cAck.m_vsiinfo);
                pVMVnicInfo->GetWdgVnicXMLInfo(cAck.m_wdg_info);
            }

            MessageOption option(cModule.GetModuleLibMid(), EV_VNETLIB_STARTNETWORK_ACK,0,0);
            m_pMU->Send(cAck,option);

            if (m_bOpenDbgInf)
            {
                cout << "Send CVNetStartNetworkMsgAck message:\n" << endl;  
                cMsg.Print();
            }
        }
    }
}
/************************************************************
* 函数名称： AddVMVSIInfo
* 功能描述： 处理VNM向VNA下发增加VM相关的VSI信息
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
void CVNetVmAgent::RcvVsiReqAckFromVNM(const MessageFrame& message)
{
    int32 iRet = 0;
    CVNAVMVnicInfo* pVMVnicInfo = NULL;

    CVMVSIInfoReqAckMsg cMsg;
    cMsg.deserialize(message.data);

    if (m_bOpenDbgInf)
    {
        cout << "Reveive CVMVSIInfoReqAckMsg message:\n" << endl;  
        cMsg.Print();
    }
    
    if (0 == cMsg.m_nReturnCode)
    {
        //增加或者更新VMVSI信息
        pVMVnicInfo = m_pVNAVnicPool->AddVMVSIInfo(cMsg.m_cVMVSIInfo);
        pVMVnicInfo->SetVnicDevName();

        //检查是否有startnetwork部署流程在等待
        if (VNA_VM_DEPLOY_STATUS_NETWORK_STARTING == pVMVnicInfo->GetDeployStatus()) 
        {
            iRet = StartNetworkProc(*pVMVnicInfo);
        }
    }
    else
    {
        pVMVnicInfo = m_pVNAVnicPool->GetVMVSIInfo(cMsg.m_cVMVSIInfo.m_nVMID);
        if (pVMVnicInfo)
        {
            pVMVnicInfo->SetQueryVSIStatus(VNA_GET_VSI_DETAIL_STATUS_FAIL);
            iRet = ERROR_VNA_VNIC_GET_VSI_FROM_VNM_FAIL;
        }
        else
        {
            cout << "" <<endl;
            return ;
        }
    }

    if ((VNA_VM_DEPLOY_STATUS_NETWORK_STARTING == pVMVnicInfo->GetDeployStatus())
        && ((0 != iRet) || \
            (VNA_VM_WDG_STATUS_OK(pVMVnicInfo) && \
             VNA_VM_EVB_STATUS_OK(pVMVnicInfo))) )
    { 
        if ("" == pVMVnicInfo->GetActionID())
        {
            return;
        }
            
        //向HC发送startnetwork应答消息
        TModuleInfo cModule;
        iRet = m_pVNetAgent->GetModuleInfo(VNA_MODULE_ROLE_HC, cModule);
        if (0 != iRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetVmAgent::AddVMVSIInfo: call GetModuleInfo failed.\n");
            return;
        }
        
        CVNetStartNetworkMsgAck cAck;
        cAck.m_vmid =  pVMVnicInfo->GetVMID();
        cAck.m_mid = pVMVnicInfo->GetDeployMID();
        cAck.action_id =  pVMVnicInfo->GetActionID();
        MID mid;
        pVMVnicInfo->SetDeployMID(mid);
        pVMVnicInfo->SetActionID("");
        cAck.state = iRet;
        cAck.detail = GetVnetErrorInfo(cAck.state);
        cAck.progress = (cAck.state == SUCCESS) ? 100 : 0;

        if (0 == iRet)
        {
            //获取返回给HC相关信息
            pVMVnicInfo->GetVnicXMLInfo(cAck.m_vsiinfo);
            pVMVnicInfo->GetWdgVnicXMLInfo(cAck.m_wdg_info);
            pVMVnicInfo->SetDeployStatus(VNA_VM_DEPLOY_STATUS_NETWORK_RUNNING);
        }
        else
        {
            pVMVnicInfo->SetDeployStatus(VNA_VM_DEPLOY_STATUS_NETWORK_START_FAIL);
        }

        MessageOption option(cModule.GetModuleLibMid(), EV_VNETLIB_STARTNETWORK_ACK,0,0);
        m_pMU->Send(cAck,option);

        if (m_bOpenDbgInf)
        {
            cout << "Send CVNetStartNetworkMsgAck message:\n" << endl;  
            cAck.Print();
        }
    }

}

/************************************************************
* 函数名称： RcvWdgOperAckMsgFromWdg
* 功能描述： 处理从wdg收到的看门狗操作消息
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
void CVNetVmAgent::RcvWdgOperAckMsgFromWdg(const MessageFrame& message)
{
    int32 iRet = 0;
    
    CVNetWdgAckMsg cMsg;
    cMsg.deserialize(message.data);

    if (m_bOpenDbgInf)
    {
        cout << "Reveive CVNetWdgAckMsg message:\n" << endl;  
        cMsg.Print();
    }
    
    CVNAVMVnicInfo* pVMVnicInfo = NULL;
    pVMVnicInfo = m_pVNAVnicPool->GetVMVSIInfo(cMsg.m_qwVmId);
    if (NULL != pVMVnicInfo)
    {
        m_pVNAVnicPool->SetWdgStatusInfo(cMsg);
    }
    else
    {
        cout << "m_pVNAVnicPool->GetVMVSIInfo is NULL" <<endl;
        return ;
    }

    //如果已有VSI详细信息则继续部署流程
    if (VNA_VM_GET_VSI_STATUS_OK(pVMVnicInfo) && \
       VNA_VM_WDG_STATUS_OK(pVMVnicInfo) && \
       VNA_VM_EVB_STATUS_OK(pVMVnicInfo))
    {
        iRet = 0;
    }
    else if (VNA_VM_WDG_STATUS_FAIL(pVMVnicInfo))
    {
        iRet = ERROR_VNA_VNIC_WDG_OPER_FAIL;
    }
    //既没有全成功也没有失败
    else
    {
        return;
    }

    //重置VM网络部署状态
    if (VNA_VM_DEPLOY_STATUS_NETWORK_STARTING == pVMVnicInfo->GetDeployStatus())
    {
        if (0 != iRet)
        {
            pVMVnicInfo->SetDeployStatus(VNA_VM_DEPLOY_STATUS_NETWORK_START_FAIL);
        }
        else
        {
            iRet = StartNetworkProc(*pVMVnicInfo);
            if (0 == iRet)
            {
                pVMVnicInfo->SetDeployStatus(VNA_VM_DEPLOY_STATUS_NETWORK_RUNNING);
            }
            else
            {
                pVMVnicInfo->SetDeployStatus(VNA_VM_DEPLOY_STATUS_NETWORK_START_FAIL);
            }    
        }

        if ("" == pVMVnicInfo->GetActionID())
        {
            return;
        }

        //向HC发送startnetwork应答消息
        TModuleInfo cModule;
        int32 hcRet = m_pVNetAgent->GetModuleInfo(VNA_MODULE_ROLE_HC, cModule);
        if (0 != hcRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetVmAgent::AddVMVSIInfo: call GetModuleInfo failed.\n");
            return;
        }
        
        CVNetStartNetworkMsgAck cAck;
        cAck.m_vmid =  pVMVnicInfo->GetVMID();
        cAck.m_mid = pVMVnicInfo->GetDeployMID();
        cAck.action_id =  pVMVnicInfo->GetActionID();
        MID mid;
        pVMVnicInfo->SetDeployMID(mid);
        pVMVnicInfo->SetActionID("");
        cAck.state = iRet;
        cAck.detail = GetVnetErrorInfo(cAck.state);
        cAck.progress = (iRet == SUCCESS) ? 100 : 0;

        if (0 == iRet)
        {
            //获取返回给HC相关信息
            pVMVnicInfo->GetVnicXMLInfo(cAck.m_vsiinfo);
            pVMVnicInfo->GetWdgVnicXMLInfo(cAck.m_wdg_info);
        }

        MessageOption option(cModule.GetModuleLibMid(), EV_VNETLIB_STARTNETWORK_ACK,0,0);
        m_pMU->Send(cAck,option);

        if (m_bOpenDbgInf)
        {
            cout << "Send CVNetStartNetworkMsgAck message:\n" << endl;  
            cAck.Print();
        }
    }
    else if (VNA_VM_DEPLOY_STATUS_NETWORK_STOPING == pVMVnicInfo->GetDeployStatus())
    {
        if (0 != iRet)
        {
            pVMVnicInfo->SetDeployStatus(VNA_VM_DEPLOY_STATUS_NETWORK_STOP_FAIL);
        }
        else
        {
            pVMVnicInfo->SetDeployStatus(VNA_VM_DEPLOY_STATUS_NETWORK_INIT);
            pVMVnicInfo->SetWdgOperStatus(VNA_VM_WDG_OP_STATUS_INIT);
        }

        if ("" == pVMVnicInfo->GetActionID())
        {
            return;
        }
        
        //向HC发送stoptnetwork应答消息
        TModuleInfo cModule;
        iRet = m_pVNetAgent->GetModuleInfo(VNA_MODULE_ROLE_HC, cModule);
        if (0 != iRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetVmAgent::AddVMVSIInfo: call GetModuleInfo failed.\n");
            return;
        }
        
        CVNetStopNetworkMsgAck cAck;
        cAck.m_vmid =  cMsg.m_qwVmId;
        cAck.m_mid = pVMVnicInfo->GetDeployMID();
        cAck.action_id =  pVMVnicInfo->GetActionID();
        MID mid;
        pVMVnicInfo->SetDeployMID(mid);
        pVMVnicInfo->SetActionID("");
        cAck.state = iRet;
        cAck.detail = GetVnetErrorInfo(cAck.state);
        cAck.progress = (iRet == SUCCESS) ? 100 : 0;
        
        MessageOption option(cModule.GetModuleLibMid(), EV_VNETLIB_STOPNETWORK_ACK,0,0);
        m_pMU->Send(cAck,option);

        //清除相关VSI信息
        m_pVNAVnicPool->DelVMVSIInfo(cMsg.m_qwVmId);

        if (m_bOpenDbgInf)
        {
            cout << "Send CVNetStopNetworkMsgAck message:\n" << endl;  
            cAck.Print();
        }
    }
    else
    {
        return;
    }
}

/************************************************************
* 函数名称： RcvWdgTimerOperMsgFromHC
* 功能描述： 处理从计算收到的看门狗操作消息
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
void CVNetVmAgent::RcvWdgTimerOperMsgFromHC(const MessageFrame& message)
{
    int32 iRet = 0;
    CVNetWdgMsg cWdgMsg;

    CVNetWatchDogMsg cMsg;
    cMsg.deserialize(message.data);
    
    if (m_bOpenDbgInf)
    {
        cout << "Reveive CVNetWatchDogMsg message:\n" << endl;  
        cMsg.Print();
    }
    
    TModuleInfo cModule;
    iRet = m_pVNetAgent->GetModuleInfo(VNA_MODULE_ROLE_WATCHDOG, cModule);
    if (0 != iRet)
    {
        goto RETURN_PROC;
    }
    
    //向watchdog模块转发消息
    cWdgMsg.action_id = cMsg.action_id;
    cWdgMsg.m_qwVmId = cMsg.m_vm_id;
    cWdgMsg.m_cMid = message.option.sender();
    cWdgMsg.m_cVnaWdgMid = cMsg.m_mid;
    if (EN_START_TIMER_WATCH_DOG == cMsg.m_operator)
    {
        cWdgMsg.m_nOperator = EN_START_WDGTIMER;
    }
    else if (EN_STOP_TIMER_WATCH_DOG == cMsg.m_operator)
    {
        cWdgMsg.m_nOperator = EN_STOP_WDGTIMER;
    }
    else
    {
        iRet = ERROR_VNA_VNIC_WDG_INPUT_PARAM_INVALID;
        goto RETURN_PROC;
    }

RETURN_PROC:
    //向watchdog模块转发消息
    if (iRet == 0)
    {
        MessageOption option(cModule.GetModuleLibMid(), EV_VNETLIB_WDG_TIMER_OPER, 0, 0);
        m_pMU->Send(cWdgMsg, option);

        if (m_bOpenDbgInf)
        {
            cout << "Send CVNetWdgMsg message:\n" << endl;  
            cWdgMsg.Print();
        }
    }
    //向计算回应答消息
    else
    {
        CVNetWatchDogMsgAck cAck(cMsg.action_id,iRet,0,/*cMsg.m_mid, */cMsg.m_vm_id);
       /* VNET_SET_ACKMSG_REQUEST_INFO(cMsg, cAck);
        cAck.action_id = cMsg.action_id;
        cAck.state = iRet; */
        cAck.m_mid = cMsg.m_mid;

        MessageOption option(message.option.sender(), EV_VNETLIB_WATCHDOG_OPER_ACK,0,0);
        m_pMU->Send(cAck,option);

        if (m_bOpenDbgInf)
        {
            cout << "Send CVNetWatchDogMsgAck message:\n" << endl;  
            cAck.Print();
        }
    }

}

/************************************************************
* 函数名称： RcvWdgTimerOperAckMsgFromWdg
* 功能描述： 处理从watchdog收到的看门狗操作应答消息
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
void CVNetVmAgent::RcvWdgTimerOperAckMsgFromWdg(const MessageFrame& message)
{
    int32 iRet = 0;
    
    CVNetWdgAckMsg cMsg;
    cMsg.deserialize(message.data);
    
    if (m_bOpenDbgInf)
    {
        cout << "Reveive CVNetWdgAckMsg message: \n " << cMsg.serialize().c_str() <<endl;
    }
    
    TModuleInfo cModule;
    iRet = m_pVNetAgent->GetModuleInfo(VNA_MODULE_ROLE_HC, cModule);
    if (0 != iRet)
    {
        return;
    }
    
    //向计算模块转发消息
    CVNetWatchDogMsgAck cWdgAckMsg(cMsg.action_id,cMsg.state, cMsg.progress, cMsg.m_qwVmId);
    cWdgAckMsg.m_mid = cMsg.m_cVnaWdgMid;

    MessageOption option(cModule.GetModuleLibMid(), EV_VNETLIB_WATCHDOG_OPER_ACK, 0, 0);
    m_pMU->Send(cWdgAckMsg, option);

    if (m_bOpenDbgInf)
    {
        cout << "Send CVNetWatchDogMsgAck message:\n " << cWdgAckMsg.serialize().c_str() <<endl; 
    }
}

/************************************************************
* 函数名称： RcvVMRestartMsgFromWDG
* 功能描述： 从WDG收到重启VM消息后，通知HC进程
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
void CVNetVmAgent::RcvVMRestartMsgFromWDG(const MessageFrame& message)
{
    int32 iRet = 0;

    CVNetWdgNotifyVMRRMsg cMsg;
    cMsg.deserialize(message.data);
    
    if (m_bOpenDbgInf)
    {
        cout << "Reveive CVNetWdgNotifyVMRRMsg message:\n" << endl;  
        cMsg.Print();
    }
    
    //检查HC模块状态
    TModuleInfo cModule;
    iRet = m_pVNetAgent->GetModuleInfo(VNA_MODULE_ROLE_HC, cModule);
    if (0 != iRet)
    {
        return;
    }

    //向HC发送消息
    CVNetNotifyVmRestatMsg cHCMsg;
    //cHCMsg.m_mid = message.option.sender();
    cHCMsg.action_id = cMsg.action_id;
    cHCMsg.m_vm_id = cMsg.m_qwVmId;

    MessageOption option(cModule.GetModuleLibMid(), EV_VNETLIB_WATCHDOG_NOTIFY_HC_RESTART, 0, 0);
    m_pMU->Send(cHCMsg, option);

    if (m_bOpenDbgInf)
    {
        cout << "Send CVNetNotifyVmRestatMsg message:\n" << endl;  
        cHCMsg.Print();
    }
}

/************************************************************
* 函数名称： RcvVMRestartAckMsgFromHC
* 功能描述： 从HC收到重启VM应答消息后，通知WDG进程
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
void CVNetVmAgent::RcvVMRestartAckMsgFromHC(const MessageFrame& message)
{
    int32 iRet = 0;

    CVNetNotifyHCrestatMsgAck cMsg;
    cMsg.deserialize(message.data);
    
    if (m_bOpenDbgInf)
    {
        cout << "Reveive CVNetNotifyHCrestatMsgAck message:\n" << endl;  
        cMsg.Print();
    }
    
    //检查wdg模块状态
    TModuleInfo cModule;
    iRet = m_pVNetAgent->GetModuleInfo(VNA_MODULE_ROLE_WATCHDOG, cModule);
    if (0 != iRet)
    {
        return;
    }

    //向wdg发送消息
    CVNetWdgNotifyVMRRAckMsg cWdgMsg;
    cWdgMsg.action_id = cMsg.action_id;
    cWdgMsg.m_qwVmId= cMsg.m_vm_id;
    cWdgMsg.state = cMsg.state;
    cWdgMsg.detail = cMsg.detail;
    cWdgMsg.progress = cMsg.progress;

    MessageOption option(cModule.GetModuleLibMid(), EV_VNETLIB_NOTIFY_VM_RR_ACK, 0, 0);
    m_pMU->Send(cWdgMsg, option);

    if (m_bOpenDbgInf)
    {
        cout << "Send CVNetWdgNotifyVMRRAckMsg message:\n" << endl;  
        cWdgMsg.Print();
    }
}

/************************************************************
* 函数名称： DelVMVSIInfo
* 功能描述： 处理VNM向VNA下发删除VM相关的VSI信息
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
void CVNetVmAgent::DelVMVSIInfo(const MessageFrame& message)
{
    CDelVMVSIInfoMsg cMsg;
    cMsg.deserialize(message.data);
    
    if (m_bOpenDbgInf)
    {
        cout << "Reveive CVMVSIInfoReqAckMsg message:\n" << endl;  
        cMsg.Print();
    }
    
    //删除VMVSI信息
    m_pVNAVnicPool->DelVMVSIInfo(cMsg.m_nVMID);
}

/************************************************************
* 函数名称： RcvAllVMVnicInfoFromHC
* 功能描述： VNA进程重启后，vnet_lib库向VNA下发所记录的所有已部署的VM相关的VSI信息
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
void CVNetVmAgent::RcvAllVMVnicInfoFromHC(const MessageFrame& message)
{
    CVNetAllVMVSIMsg cMsg;
    cMsg.deserialize(message.data);

    if (m_bOpenDbgInf)
    {
        cout << "Reveive CVNetAllVMVSIMsg message:\n" << endl;  
        cMsg.Print();
    }

    //增加或者更新所有VMVSI信息
    //更新bScheduleFlag标记

    vector<CVNetVMVSIMsg>::iterator itrVM = cMsg.m_allvnics.begin();
    for ( ; itrVM != cMsg.m_allvnics.end(); ++itrVM)
    {
        //增加或者更新VMVSI信息
        m_pVNAVnicPool->AddVMVnicInfo(*itrVM);
    }

    //返回消息
    CVNetAllVMVSIMsgAck cAck;
    cAck.m_result = 0;
    
    MessageOption option(message.option.sender(), EV_VNETLIB_NOTIFY_ALL_VMNIC_ACK,0,0);
    m_pMU->Send(cAck,option);

    if (m_bOpenDbgInf)
    {
        cout << "Send CVNetAllVMVSIMsgAck message:\n" << endl;  
        cAck.Print();
    }
}

/************************************************************
* 函数名称： RcvAllVMVSIInfoFromVNM
* 功能描述： VNA进程重启后，VNM向VNA下发所记录的所有已部署的VM相关的VSI信息
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
void CVNetVmAgent::RcvAllVMVSIInfoFromVNM(const MessageFrame& message)
{
    CVNMAddAllVMVSIInfoMsg cMsg;
    cMsg.deserialize(message.data);
    
    if (m_bOpenDbgInf)
    {
        cout << "Reveive CVNMAddAllVMVSIInfoMsg message:\n" << endl;  
        cMsg.Print();
    }
    
    //增加或者更新所有VMVSI信息
    //更新bScheduleFlag标记

    vector<CVMVSIInfo>::iterator itrVM = cMsg.m_vecVMVSIInfo.begin();
    for ( ; itrVM != cMsg.m_vecVMVSIInfo.end(); ++itrVM)
    {
        //增加或者更新VMVSI信息
        m_pVNAVnicPool->AddVMVSIInfo(*itrVM);
    }

    MessageOption option(message.option.sender(),EV_VNM_NOTIFY_VNA_ALL_VMVSI_ACK,0,0);
    m_pMU->Send("",option);

    if (m_bOpenDbgInf)
    {
        cout << "Send EV_VNM_NOTIFY_VNA_ALL_VMVSI_ACK message:\n" << endl;  
    }
};

void CVNetVmAgent::DbgShowData(void)
{
    cout << "CVNetVmAgent::DbgShowData:" << endl;
    m_pVNAVnicPool->DbgShowData(0);   
    cout << endl;
    return;
}

/************************************************************
* 函数名称： SetDbgFlag
* 功能描述： 调试信息开关
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
void CVNetVmAgent::SetDbgFlag(int bDbgFlag)
{
    m_bOpenDbgInf = bDbgFlag;
};


int32   g_nNicNum = 2;
int32   g_nIsSriov1;
int32   g_nIsLoop1;
string g_strLogicNetworkID1;
string g_strVsiUuid1;
int32   g_nIsSriov2;
int32   g_nIsLoop2;
string g_strLogicNetworkID2;
string g_strVsiUuid2;
int32   g_nIsSriov3;
int32   g_nIsLoop3;
string g_strLogicNetworkID3;
string g_strVsiUuid3;

void VNM_SET_VM_VNIC_NUM( int32   nNicNum)
{
    g_nNicNum = nNicNum;
};
DEFINE_DEBUG_FUNC(VNM_SET_VM_VNIC_NUM);

void VNA_SET_VM_VNIC1( int32   nIsSriov,
                            int32   nIsLoop,
                            char * strLogicNetworkID,
                            char * strVsiUuid)
{
    g_nIsSriov1 = nIsSriov;
    g_nIsLoop1 = nIsLoop;
    g_strLogicNetworkID1 = strLogicNetworkID;
    g_strVsiUuid1 = strVsiUuid;
};
DEFINE_DEBUG_FUNC(VNA_SET_VM_VNIC1);

void VNA_SET_VM_VNIC2( int32   nIsSriov,
                            int32   nIsLoop,
                            char * strLogicNetworkID,
                            char * strVsiUuid)
{
    g_nIsSriov2 = nIsSriov;
    g_nIsLoop2 = nIsLoop;
    g_strLogicNetworkID2 = strLogicNetworkID;
    g_strVsiUuid2 = strVsiUuid;
};
DEFINE_DEBUG_FUNC(VNA_SET_VM_VNIC2);

void VNA_SET_VM_VNIC3( int32   nIsSriov,
                            int32   nIsLoop,
                            char * strLogicNetworkID,
                            char * strVsiUuid)
{
    g_nIsSriov3 = nIsSriov;
    g_nIsLoop3 = nIsLoop;
    g_strLogicNetworkID3 = strLogicNetworkID;
    g_strVsiUuid3 = strVsiUuid;
};
DEFINE_DEBUG_FUNC(VNA_SET_VM_VNIC3);

void VNA_TEST_ADD_VSIINFO(int64 nVMID, int nic_num)
{
    string vna_app;
    GetVNAUUIDByLib(vna_app);

    CAddVMVSIInfoMsg cVsiMsg;
    cVsiMsg.m_cVMVSIInfo.m_nVMID = nVMID;

    for (int i = 0; i <nic_num; i++)
    {
        CVNetVnicDetail vecVnicDetail;
        vecVnicDetail.SetNicIndex(i);
        string vsiid = "11112222";
        vecVnicDetail.SetVSIIDValue(vsiid);

        cVsiMsg.m_cVMVSIInfo.m_vecVnicDetail.push_back(vecVnicDetail);
    }

    MessageUnit temp_mu(TempUnitName("add_vsi_info"));
    temp_mu.Register();
    MID cVnaMID;
    cVnaMID._application = vna_app;
    cVnaMID._process = PROC_VNA;
    cVnaMID._unit = MU_VNA_CONTROLLER;

    MessageOption vna_option(cVnaMID, EV_VNM_NOTIFY_VNA_ADD_VMVSI,0,0);
    temp_mu.Send(cVsiMsg,vna_option);
    MessageFrame message;

    if (SUCCESS == temp_mu.Wait(&message, 1000))
    {
        cout << "send msg success" << endl;
    }

    return;
};
DEFINE_DEBUG_FUNC(VNA_TEST_ADD_VSIINFO);

void VNA_TEST_DEL_VSIINFO(int64 nVMID)
{
    string vna_app;
    GetVNAUUIDByLib(vna_app);

    CDelVMVSIInfoMsg cVsiMsg;
    cVsiMsg.m_nVMID = nVMID;

    MessageUnit temp_mu(TempUnitName("del_vsi_info"));
    temp_mu.Register();
    MID cVnaMID;
    cVnaMID._application = vna_app;
    cVnaMID._process = PROC_VNA;
    cVnaMID._unit = MU_VNA_CONTROLLER;

    MessageOption vna_option(cVnaMID, EV_VNM_NOTIFY_VNA_DEL_VMVSI,0,0);
    temp_mu.Send(cVsiMsg,vna_option);
    MessageFrame message;

    if (SUCCESS == temp_mu.Wait(&message, 1000))
    {
        cout << "send msg success" << endl;
    }

    return;
};
DEFINE_DEBUG_FUNC(VNA_TEST_DEL_VSIINFO);


void VNA_TEST_SEND_ALL_HC_VSIINFO(int vm_num, int nic_num)
{

    string vna_app;
    GetVNAUUIDByLib(vna_app);

    CVNetAllVMVSIMsg cMsg;

    for (int i = 1; i <=vm_num; i++)
    {
        CVNetVMVSIMsg vmvsimsg;

        vmvsimsg.m_vm_id = 100 + i;
        vmvsimsg.m_domain_id = i;
        vmvsimsg.m_project_id = 100;
        for (int j = 0; j <nic_num; j++)
        {
            CVNetVnicConfig vnic;
            vnic.m_nNicIndex = i;
            vnic.m_strVSIProfileID = "11112222";

            vmvsimsg.m_vnics.push_back(vnic);
        }
            
        cMsg.m_allvnics.push_back(vmvsimsg);
    }

    MessageUnit temp_mu(TempUnitName("send_hc_all_vsi_info"));
    temp_mu.Register();
    MID cVnaMID;
    cVnaMID._application = vna_app;
    cVnaMID._process = PROC_VNA;
    cVnaMID._unit = MU_VNA_CONTROLLER;

    MessageOption vna_option(cVnaMID, EV_VNETLIB_NOTIFY_ALL_VMNIC,0,0);
    temp_mu.Send(cMsg,vna_option);
    MessageFrame message;

    if (SUCCESS == temp_mu.Wait(&message, 1000))
    {
        cout << "send msg success" << endl;
    }

    return;
}
DEFINE_DEBUG_FUNC(VNA_TEST_SEND_ALL_HC_VSIINFO);


void VNA_TEST_SEND_ALL_VNM_VSIINFO(int vm_num, int nic_num)
{

    string vna_app;
    GetVNAUUIDByLib(vna_app);

    CVNMAddAllVMVSIInfoMsg cMsg;

    for (int i = 1; i <=vm_num; i++)
    {
        CVMVSIInfo cVMVSIInfo;
        
        cVMVSIInfo.m_nVMID = i;

        for (int j = 0; j <nic_num;j++)
        {
            CVNetVnicDetail vecVnicDetail;
            vecVnicDetail.SetNicIndex(j);
            string vsiid = "11112222";
            vecVnicDetail.SetVSIIDValue(vsiid);

            cVMVSIInfo.m_vecVnicDetail.push_back(vecVnicDetail);
        }
        cMsg.m_vecVMVSIInfo.push_back(cVMVSIInfo);
    
        }

    MessageUnit temp_mu(TempUnitName("send_hc_all_vsi_info"));
    temp_mu.Register();
    MID cVnaMID;
    cVnaMID._application = vna_app;
    cVnaMID._process = PROC_VNA;
    cVnaMID._unit = MU_VNA_CONTROLLER;

    MessageOption vna_option(cVnaMID, EV_VNM_NOTIFY_VNA_ALL_VMVSI,0,0);
    temp_mu.Send(cMsg,vna_option);
    MessageFrame message;

    if (SUCCESS == temp_mu.Wait(&message, 1000))
    {
        cout << "send msg success" << endl;
    }

    return;
}
DEFINE_DEBUG_FUNC(VNA_TEST_SEND_ALL_VNM_VSIINFO);

void VNA_DBG_SEND_START_NETWORK(int64 nVMID, 
                            int64 nProjectID,
                            int32 nIsWdgEnable,
                            int32   nOption)
{
    CVNetStartNetworkMsg cMsg;
    cMsg.m_vm_id = nVMID;
    cMsg.m_option = nOption;

    for (int i = 0; i <g_nNicNum; i++)
    {
        CVNetVnicConfig cVnicCfg;

        cVnicCfg.m_nNicIndex = i;
        if (0 == i)
        {
            cVnicCfg.m_nIsSriov = g_nIsSriov1;
            cVnicCfg.m_nIsLoop = g_nIsLoop1;
            cVnicCfg.m_strLogicNetworkID = g_strLogicNetworkID1;
            cVnicCfg.m_strVSIProfileID = g_strVsiUuid1;
        }
        else if(1 == i)
        {
            cVnicCfg.m_nIsSriov = g_nIsSriov2;
            cVnicCfg.m_nIsLoop = g_nIsLoop2;
            cVnicCfg.m_strLogicNetworkID = g_strLogicNetworkID2;
            cVnicCfg.m_strVSIProfileID = g_strVsiUuid2;
        }
        else
        {
            cVnicCfg.m_nIsSriov = g_nIsSriov3;
            cVnicCfg.m_nIsLoop = g_nIsLoop3;
            cVnicCfg.m_strLogicNetworkID = g_strLogicNetworkID3;
            cVnicCfg.m_strVSIProfileID = g_strVsiUuid3;
        }

        cMsg.m_VmNicCfgList.push_back(cVnicCfg);
    }

    if (nIsWdgEnable)
    {
        cMsg.m_WatchDogInfo.m_nIsWDGEnable = 1;
        cMsg.m_WatchDogInfo.m_nTimeOut = 30;
    }
    
    MessageUnit temp_mu(TempUnitName("START_NETWORK"));
    temp_mu.Register();
    MID cVnaMID;
    cVnaMID._application = ApplicationName();
    cVnaMID._process = PROC_VNA;
    cVnaMID._unit = MU_VNA_CONTROLLER;

    MessageOption vna_option(cVnaMID, EV_VNETLIB_STARTNETWORK,0,0);
    temp_mu.Send(cMsg,vna_option);
    MessageFrame message;

    if (SUCCESS == temp_mu.Wait(&message, 2000))
    {
        CVNetStartNetworkMsgAck ack;
        ack.deserialize(message.data);

        cout << "Receive ack: m_result :" << ack.state<< endl;
        ack.Print();
    }

    return;
};
DEFINE_DEBUG_FUNC(VNA_DBG_SEND_START_NETWORK);

void VNA_DBG_SEND_STOP_NETWORK(int64 nVMID, 
                            int64   nProjectID,
                            int32   nIsWdgEnable,
                            int32   nOption)
{
    CVNetStopNetworkMsg cMsg;
    cMsg.m_vm_id = nVMID;

    for (int i = 0; i <g_nNicNum; i++)
    {
        CVNetVnicConfig cVnicCfg;
        cVnicCfg.m_nNicIndex = i;
        if (0 == i)
        {
            cVnicCfg.m_nIsSriov = g_nIsSriov1;
            cVnicCfg.m_nIsLoop = g_nIsLoop1;
            cVnicCfg.m_strLogicNetworkID = g_strLogicNetworkID1;
            cVnicCfg.m_strVSIProfileID = g_strVsiUuid1;
        }
        else if(1 == i)
        {
            cVnicCfg.m_nIsSriov = g_nIsSriov2;
            cVnicCfg.m_nIsLoop = g_nIsLoop2;
            cVnicCfg.m_strLogicNetworkID = g_strLogicNetworkID2;
            cVnicCfg.m_strVSIProfileID = g_strVsiUuid2;
        }
        else
        {
            cVnicCfg.m_nIsSriov = g_nIsSriov3;
            cVnicCfg.m_nIsLoop = g_nIsLoop3;
            cVnicCfg.m_strLogicNetworkID = g_strLogicNetworkID3;
            cVnicCfg.m_strVSIProfileID = g_strVsiUuid3;
        }

        cMsg.m_VmNicCfgList.push_back(cVnicCfg);
    }
    
    if (nIsWdgEnable)
    {
        cMsg.m_WatchDogInfo.m_nIsWDGEnable = 1;
        cMsg.m_WatchDogInfo.m_nTimeOut = 30;
    }

    MessageUnit temp_mu(TempUnitName("STOP_NETWORK"));
    temp_mu.Register();
    MID cVnaMID;
    cVnaMID._application = ApplicationName();
    cVnaMID._process = PROC_VNA;
    cVnaMID._unit = MU_VNA_CONTROLLER;

    MessageOption vna_option(cVnaMID, EV_VNETLIB_STOPNETWORK,0,0);
    temp_mu.Send(cMsg,vna_option);
    MessageFrame message;

    if (SUCCESS == temp_mu.Wait(&message, 2000))
    {
        CVNetStopNetworkMsgAck ack;
        ack.deserialize(message.data);

        cout << "Receive ack: m_result :" << ack.state<< endl;
        ack.Print();
    }

    return;
};
DEFINE_DEBUG_FUNC(VNA_DBG_SEND_STOP_NETWORK);

void VNA_DBG_SEND_NOTIFY_DOMID(int64 nVMID, int nDomianID)
{
    CVNetNotifyDomIDMsg cMsg;
    cMsg.m_vm_id = nVMID;
    cMsg.m_dom_id= nDomianID;

    MessageUnit temp_mu(TempUnitName("NOTIFY_DOMID"));
    temp_mu.Register();
    MID cVnaMID;
    cVnaMID._application = ApplicationName();
    cVnaMID._process = PROC_VNA;
    cVnaMID._unit = MU_VNA_CONTROLLER;

    MessageOption vna_option(cVnaMID, EV_VNETLIB_NOTIFY_DOMID,0,0);
    temp_mu.Send(cMsg,vna_option);
    MessageFrame message;

    if (SUCCESS == temp_mu.Wait(&message, 2000))
    {
        CVNetNotifyDomIDMsgAck ack;
        ack.deserialize(message.data);

        cout << "Receive ack: m_result :" << ack.state<< endl;
        ack.Print();
    }

    return;
};
DEFINE_DEBUG_FUNC(VNA_DBG_SEND_NOTIFY_DOMID);

}


