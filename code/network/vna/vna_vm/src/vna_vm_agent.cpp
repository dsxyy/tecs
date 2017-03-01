/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�vna_vm_agent.cpp
* �ļ���ʶ��
* ����ժҪ��CVNetVmAgent��ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�
* ������ڣ�2013��2��22��
*
* �޸ļ�¼1��
*     �޸����ڣ�2013/2/22
*     �� �� �ţ�V1.0
*     �� �� �ˣ�lverchun
*     �޸����ݣ�����
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
* �������ƣ� CVNetVmAgent
* ���������� CVNetVmAgent���캯��
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1        V1.0       lvech         ����
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
* �������ƣ� ~CVNetVmAgent
* ���������� CVNetVmAgent��������
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1        V1.0       lvech         ����
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
* �������ƣ� Init
* ���������� ��ʼ������
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2012/5         V1.0       lvech         ����
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
* �������ƣ� MessageEntry
* ���������� ��Ϣ����ں���
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2012/5         V1.0       lvech         ����
***************************************************************/
void CVNetVmAgent::MessageEntry(const MessageFrame &message)
{
    //�����յ�����Ϣid���д���
    switch (message.option.id())
    {
        //HC��VNA�������������Դ
        case EV_VNETLIB_STARTNETWORK:
        {
            StartNetwork(message);
        }
        break;

        //HC��VNA�ͷ����������Դ
        case EV_VNETLIB_STOPNETWORK:
        {
            StopNetwork(message);
        }
        break;
        
        //HC��VNAͨ��VM��DOM ID�仯���
        case EV_VNETLIB_NOTIFY_DOMID:
        {
            NotifyDomID(message);
        }
        break;

        //VNM��VNA�·�����VM��ص�VSI��Ϣ
        case EV_VNM_NOTIFY_VNA_ADD_VMVSI:
        {
            AddVMVSIInfo(message);
        }
        break;

        //VNM��VNA�·�ɾ��VM��ص�VSI��Ϣ
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
        
        //VNA����������vnet_lib����VNA�·�����¼�������Ѳ����VM��ص�VSI��Ϣ
        case EV_VNETLIB_NOTIFY_ALL_VMNIC:
        {
            RcvAllVMVnicInfoFromHC(message);
        }
        break;

        //VNA����������VNM��VNA�·�����¼�������Ѳ����VM��ص�VSI��Ϣ
        case EV_VNM_NOTIFY_VNA_ALL_VMVSI:
        {
            RcvAllVMVSIInfoFromVNM(message);
        }
        break;

        //��ʱ���VM�����ɵĶ˿ڲ������������
        case EV_VNA_CHECK_VM_VSI_INFO:
        {
            m_pVNAVnicPool->CheckVMVSIInfo();
        }
        break;

        //��WDG�յ����Ź�����Ӧ����Ϣ
        case  EV_VNETLIB_WDG_OPER_ACK:
        {
            RcvWdgOperAckMsgFromWdg(message);
        }
        break;

        //�Ӽ����յ����Ź���ʱ��������Ϣ
        case  EV_VNETLIB_WATCHDOG_OPER:
        {
            RcvWdgTimerOperMsgFromHC(message);
        }
        break;

        //��wdg�յ����Ź���ʱ������Ӧ����Ϣ
        case  EV_VNETLIB_WDG_TIMER_OPER_ACK:
        {
            RcvWdgTimerOperAckMsgFromWdg(message);
        }
        break;

        //��wdg�յ�����VM��Ϣ
        case  EV_VNETLIB_NOTIFY_VM_RR:
        {
            RcvVMRestartMsgFromWDG(message);
        }
        break;

        //�Ӽ����յ�����VMӦ����Ϣ
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
* �������ƣ� StartNetwork
* ���������� HC��VNA�������������Դ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� 
* ��������� 
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
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
        
    //��ѯ�ڴ����Ƿ��ж�ӦVNIC���ã�û������Ҫ��VNM�����Ӧ��VSI��ϸ��Ϣ
    CVNAVMVnicInfo *pVMVnicInfo;
    pVMVnicInfo = m_pVNAVnicPool->AddVMVnicInfo(cMsg);
    //�������VSI��ϸ��Ϣ�������������
    if (VNA_VM_GET_VSI_STATUS_OK(pVMVnicInfo) \
      && VNA_VM_WDG_STATUS_OK(pVMVnicInfo) \
      && VNA_VM_EVB_STATUS_OK(pVMVnicInfo))
    {
        iRet = StartNetworkProc(*pVMVnicInfo);
    }
    //VM���������Ϣ��û��׼����
    else
    {
        if (VNA_VM_GET_VSI_STATUS_NOT_OK(pVMVnicInfo))
        {
            string strVnmName;
            //���û��ע����Ϣ��ֱ�ӷ���ʧ��
            if (ERROR == GetRegisteredSystem(strVnmName))
            {
                VNET_LOG(VNET_LOG_ERROR, "CVNetVmAgent::StartNetwork: call GetRegisteredSystem failed.\n");
                pVMVnicInfo->SetQueryVSIStatus(VNA_GET_VSI_DETAIL_STATUS_FAIL);
                iRet = ERROR_VNA_VNIC_GET_REGED_VNM_FAIL;
                goto SEND_ACK;
            }
                
            //��VNM����VSI��ϸ��Ϣ
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
        
        //�������Ź���û��Ӧ��
        if (VNA_VM_WDG_STATUS_NOT_OK(pVMVnicInfo))
        {
            //���Ź�������Ϣʱ����Ҫ�ѱ���action_id���ϣ����Ź���Ӧ����Ϣʱ��action_id������
            //�������Լ�������Ӧ����Ϣ�Ƿ��Ǳ��������Ӧ����Ϣ��������          
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

        // evb tlv ����ȴ�vsi ׼����������ִ��
        if (VNA_VM_GET_VSI_STATUS_OK(pVMVnicInfo))
        {
            if (VNA_VM_VSI_EVB_STATUS_NOT_OK(pVMVnicInfo))
            {
                // ������ѯvsi evb status 
                int32 noNeedEvbTlv = FALSE;
                // �������hc�����ģ�����鿴evb tlvЭ�̽��
                if ("" == pVMVnicInfo->GetActionID())
                {
                    noNeedEvbTlv = TRUE;
                }                
                iRet = StartEvbProc(*pVMVnicInfo,noNeedEvbTlv);
    
                if( pVMVnicInfo->GetQueryVSIEvbStatus() == VNA_VM_VSI_EVB_STATUS_SUCCESS )
                {
                    // ������Դ����Ҫ���� pci 
    
                    // �ж����Ƿ���Ҫִ�� startnetworkproc
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
                    // ��vm vnic ����״̬��ΪFAIL
                    pVMVnicInfo->SetDeployStatus(VNA_VM_DEPLOY_STATUS_NETWORK_START_FAIL);
                    iRet = ERROR_VNA_VNIC_VSI_EVB_STATUS_FAIL;
                    goto SEND_ACK;
                }            
            }
        }

        return;
    }

SEND_ACK:
    //ע�⿼��Ǩ������
    CVNetStartNetworkMsgAck cAck;
    cAck.m_vmid =  cMsg.m_vm_id;
    cAck.action_id =  cMsg.action_id;
    cAck.state = iRet;
    cAck.detail = GetVnetErrorInfo(cAck.state);
    cAck.progress = (cAck.state == SUCCESS) ? 100 : 0;
    VNET_SET_ACKMSG_REQUEST_INFO(cMsg, cAck);

    if (0 == iRet)
    {
        //��ȡ���ظ�HC�����Ϣ
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
* �������ƣ� StartNetworkProc
* ���������� StartNetwork��������
* ���ʵı� ��
* �޸ĵı� ��
* ��������� 
* ��������� 
* ��   ��   ֵ�� ��
* ����˵����  ���ô˺�����ǰ��������VNA�ѻ��
                                 �ĸ�VM��ص�VSI��ϸ��Ϣ
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
***************************************************************/
int32 CVNetVmAgent::StartNetworkProc(CVNAVMVnicInfo& cVMVnicInfo)
{
    int32 iRet = 0;

    vector<CVNetVnicDetail> vecVnicInfo;
    cVMVnicInfo.GetVnicDetailInfo(vecVnicInfo);

    //������ź�PCI�豸�Ƿ����
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

    // evb tlv ����ȴ�vsi ׼����������ִ��
    if (!VNA_VM_GET_VSI_STATUS_OK(pVMVnicInfo))
    {
        return;
    }

    int32 noNeedEvbTlv = FALSE;
    // �������hc�����ģ�����鿴evb tlvЭ�̽��
    if ("" == pVMVnicInfo->GetActionID())
    {
        noNeedEvbTlv = TRUE;
    }
    
    int32 iRet = 0;
    time_t cur_time = time(0);

    // Ŀǰֻ��startnetwork ��Ҫ��ѯevb status 
    if (VNA_VM_DEPLOY_STATUS_NETWORK_STARTING == pVMVnicInfo->GetDeployStatus())
    {
        if (VNA_VM_VSI_EVB_STATUS_NOT_OK(pVMVnicInfo))
        {
            pVMVnicInfo->SetQueryVSIEvbLastTime(cur_time);
            pVMVnicInfo->SetQueryVSIEvbTimes(pVMVnicInfo->GetQueryVSIEvbTimes() + 1);

            // ������ѯvsi evb status 
            iRet = StartEvbProc(*pVMVnicInfo,noNeedEvbTlv);
        
            if( pVMVnicInfo->GetQueryVSIEvbStatus() == VNA_VM_VSI_EVB_STATUS_SUCCESS )
            {
                // ������Դ����Ҫ���� pci 
        
                // �ж����Ƿ���Ҫִ�� startnetworkproc
                if (VNA_VM_GET_VSI_STATUS_OK(pVMVnicInfo) \
                  && VNA_VM_WDG_STATUS_OK(pVMVnicInfo) \
                  && VNA_VM_EVB_STATUS_OK(pVMVnicInfo))
                {
                    iRet = StartNetworkProc(*pVMVnicInfo);
                    // ����vm vnic ����״̬ 
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
                // ��vm vnic ����״̬��ΪFAIL
                pVMVnicInfo->SetDeployStatus(VNA_VM_DEPLOY_STATUS_NETWORK_START_FAIL);
                iRet = ERROR_VNA_VNIC_VSI_EVB_STATUS_FAIL;
                goto SEND_ACK;
            }            
        }
    }
    return;

SEND_ACK:
    // �������hc�����ģ������Ӧ��
    if ("" == pVMVnicInfo->GetActionID())
    {
        return;
    }

    //��HC����startnetworkӦ����Ϣ
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
        //��ȡ���ظ�HC�����Ϣ
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

    // ��evb status ����
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
        
        // ������Դ����Ҫ���� pci 
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

    // ��evb status ����
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
* �������ƣ� StopNetwork
* ���������� HC��VNA�ͷ����������Դ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� 
* ��������� 
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
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
    
    //��ѯ�ڴ���VSI��ϸ��Ϣ
    CVNAVMVnicInfo* pVMVnicInfo = NULL;
    pVMVnicInfo = m_pVNAVnicPool->GetVMVSIInfo(cMsg.m_vm_id);
    if (NULL == pVMVnicInfo)
    {
        iRet = 0;   //������Ҫ��
        goto SEND_ACK;
    }

    //��鵱ǰ״̬
    if (VNA_VM_DEPLOY_STATUS_NETWORK_STOP_FAIL == pVMVnicInfo->GetDeployStatus())
    {
        //�ϴ�stopʧ�ܣ��������ԣ���Ҫ������ϴβ�����Ϣ
        pVMVnicInfo->SetDeployStatus(VNA_VM_DEPLOY_STATUS_NETWORK_STOPING);        

        if (VNA_VM_WDG_ENABLE_YES == pVMVnicInfo->GetIsWdgEnable())
        {
            pVMVnicInfo->SetWdgOperStatus(VNA_VM_WDG_OP_STATUS_INIT);
            pVMVnicInfo->SetWdgOperTimes(0);
        }

        // �轫EVB���
        pVMVnicInfo->SetQueryVSIEvbStatus(VNA_VM_VSI_EVB_STATUS_INIT);
        pVMVnicInfo->SetQueryVSIEvbLastTime(0);
        pVMVnicInfo->SetQueryVSIEvbTimes(0);
    }
    else if (VNA_VM_DEPLOY_STATUS_NETWORK_INIT == pVMVnicInfo->GetDeployStatus())
    {
        //��stop�ɹ�������ֱ�ӷ��سɹ�
        iRet = 0;   //������Ҫ��
        goto SEND_ACK;
    }
    else if (VNA_VM_DEPLOY_STATUS_NETWORK_STOPING == pVMVnicInfo->GetDeployStatus())
    {
        //���ڳ���
        iRet = ERROR_ACTION_RUNNING;   //������Ҫ��
        goto SEND_ACK;
    }

    pVMVnicInfo->GetVnicDetailInfo(vecVnicInfo);

    //������ź�PCI�豸�Ƿ����
    iRet = m_pSwitchAgent->FreeDVSRes(vecVnicInfo);
    if (0 != iRet)
    {
        VNET_LOG(VNET_LOG_ERROR, 
            "CVNetVmAgent::StopNetwork: call FreeDVSRes failed, ret: %d.\n", iRet);
        iRet = ERROR_VNA_VNIC_FREE_DVS_FAIL;
        goto SEND_ACK;
    }
    
    // �ͷ��ѷ����evb��Դ
    evbRet = StopEvbProc(*pVMVnicInfo);
    if( pVMVnicInfo->GetQueryVSIEvbStatus() == VNA_VM_VSI_EVB_STATUS_FAILED )
    {
        iRet = ERROR_VNA_VNIC_VSI_EVB_STATUS_FAIL;
        VNET_LOG(VNET_LOG_WARN, 
            "CVNetVmAgent::StopNetwork: call StopVPortDiscovery(vm_id:%d, option:%d) failed, ret: %d.\n", 
            cMsg.m_vm_id, cMsg.m_option, iRet);
        goto SEND_ACK;
    }
    
    //����vepa ������Ϣ
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

            //��WDGģ�鷢�͹ر�WDG��Ϣ
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
        //������VSI��Ϣ
        m_pVNAVnicPool->DelVMVSIInfo(cMsg.m_vm_id);
    }
    
    //����Ҫ����VM������ʧ�������������Ϣ
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
* �������ƣ� NotifyDomID
* ���������� HC��VNAͨ��VM��DOM ID�仯���
* ���ʵı� ��
* �޸ĵı� ��
* ��������� 
* ��������� 
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
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
    
    //�����ڴ���vnic��domid
    m_pVNAVnicPool->UpdateDomID(cMsg.m_vm_id, cMsg.m_dom_id);

    // ����external-id, update quantum port
    SetExternalId(cMsg.m_vm_id, cMsg.m_dom_id);
    
    //������Ϣ
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
* �������ƣ� AddVMVSIInfo
* ���������� ����VNM��VNA�·�����VM��ص�VSI��Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� 
* ��������� 
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
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
    
    //���ӻ��߸���VMVSI��Ϣ
    CVNAVMVnicInfo* pVMVnicInfo = NULL;
    pVMVnicInfo = m_pVNAVnicPool->AddVMVSIInfo(cMsg.m_cVMVSIInfo);

    //����Ƿ���startnetwork���������ڵȴ�
    if (VNA_VM_DEPLOY_STATUS_NETWORK_STARTING == pVMVnicInfo->GetDeployStatus()) 
    {
        iRet = StartNetworkProc(*pVMVnicInfo);
        if ((0 != iRet) || \
           ( VNA_VM_WDG_STATUS_OK(pVMVnicInfo) && \
             VNA_VM_EVB_STATUS_OK(pVMVnicInfo)) )
        {
            //����VM���粿��״̬
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
                    
            //��HC����startnetworkӦ����Ϣ
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
                //��ȡ���ظ�HC�����Ϣ
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
* �������ƣ� AddVMVSIInfo
* ���������� ����VNM��VNA�·�����VM��ص�VSI��Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� 
* ��������� 
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
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
        //���ӻ��߸���VMVSI��Ϣ
        pVMVnicInfo = m_pVNAVnicPool->AddVMVSIInfo(cMsg.m_cVMVSIInfo);
        pVMVnicInfo->SetVnicDevName();

        //����Ƿ���startnetwork���������ڵȴ�
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
            
        //��HC����startnetworkӦ����Ϣ
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
            //��ȡ���ظ�HC�����Ϣ
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
* �������ƣ� RcvWdgOperAckMsgFromWdg
* ���������� �����wdg�յ��Ŀ��Ź�������Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� 
* ��������� 
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
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

    //�������VSI��ϸ��Ϣ�������������
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
    //��û��ȫ�ɹ�Ҳû��ʧ��
    else
    {
        return;
    }

    //����VM���粿��״̬
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

        //��HC����startnetworkӦ����Ϣ
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
            //��ȡ���ظ�HC�����Ϣ
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
        
        //��HC����stoptnetworkӦ����Ϣ
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

        //������VSI��Ϣ
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
* �������ƣ� RcvWdgTimerOperMsgFromHC
* ���������� ����Ӽ����յ��Ŀ��Ź�������Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� 
* ��������� 
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
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
    
    //��watchdogģ��ת����Ϣ
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
    //��watchdogģ��ת����Ϣ
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
    //������Ӧ����Ϣ
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
* �������ƣ� RcvWdgTimerOperAckMsgFromWdg
* ���������� �����watchdog�յ��Ŀ��Ź�����Ӧ����Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� 
* ��������� 
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
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
    
    //�����ģ��ת����Ϣ
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
* �������ƣ� RcvVMRestartMsgFromWDG
* ���������� ��WDG�յ�����VM��Ϣ��֪ͨHC����
* ���ʵı� ��
* �޸ĵı� ��
* ��������� 
* ��������� 
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
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
    
    //���HCģ��״̬
    TModuleInfo cModule;
    iRet = m_pVNetAgent->GetModuleInfo(VNA_MODULE_ROLE_HC, cModule);
    if (0 != iRet)
    {
        return;
    }

    //��HC������Ϣ
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
* �������ƣ� RcvVMRestartAckMsgFromHC
* ���������� ��HC�յ�����VMӦ����Ϣ��֪ͨWDG����
* ���ʵı� ��
* �޸ĵı� ��
* ��������� 
* ��������� 
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
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
    
    //���wdgģ��״̬
    TModuleInfo cModule;
    iRet = m_pVNetAgent->GetModuleInfo(VNA_MODULE_ROLE_WATCHDOG, cModule);
    if (0 != iRet)
    {
        return;
    }

    //��wdg������Ϣ
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
* �������ƣ� DelVMVSIInfo
* ���������� ����VNM��VNA�·�ɾ��VM��ص�VSI��Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� 
* ��������� 
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
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
    
    //ɾ��VMVSI��Ϣ
    m_pVNAVnicPool->DelVMVSIInfo(cMsg.m_nVMID);
}

/************************************************************
* �������ƣ� RcvAllVMVnicInfoFromHC
* ���������� VNA����������vnet_lib����VNA�·�����¼�������Ѳ����VM��ص�VSI��Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� 
* ��������� 
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
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

    //���ӻ��߸�������VMVSI��Ϣ
    //����bScheduleFlag���

    vector<CVNetVMVSIMsg>::iterator itrVM = cMsg.m_allvnics.begin();
    for ( ; itrVM != cMsg.m_allvnics.end(); ++itrVM)
    {
        //���ӻ��߸���VMVSI��Ϣ
        m_pVNAVnicPool->AddVMVnicInfo(*itrVM);
    }

    //������Ϣ
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
* �������ƣ� RcvAllVMVSIInfoFromVNM
* ���������� VNA����������VNM��VNA�·�����¼�������Ѳ����VM��ص�VSI��Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� 
* ��������� 
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
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
    
    //���ӻ��߸�������VMVSI��Ϣ
    //����bScheduleFlag���

    vector<CVMVSIInfo>::iterator itrVM = cMsg.m_vecVMVSIInfo.begin();
    for ( ; itrVM != cMsg.m_vecVMVSIInfo.end(); ++itrVM)
    {
        //���ӻ��߸���VMVSI��Ϣ
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
* �������ƣ� SetDbgFlag
* ���������� ������Ϣ����
* ���ʵı� ��
* �޸ĵı� ��
* ��������� 
* ��������� 
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
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


