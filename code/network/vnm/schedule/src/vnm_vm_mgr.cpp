/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�vnm_vm_mgr.cpp
* �ļ���ʶ��
* ����ժҪ��CVNMVmMgr��ʵ���ļ�
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
void CVNetVmMgr::MessageEntry(const MessageFrame &message)
{
    //�����յ�����Ϣid���д���
    switch (message.option.id())
    {
        //����TC��VNM��ȡ����VM����������CC�����б���Ϣ
        case EV_VNETLIB_SELECT_CC:
        {
            SelectCCListForVM(message);
        }
        break;

        //�����scheduleģ���յ�CC�����б���Ϣ
        case EV_VNM_SCHEDULE_CC_ACK:
        {
            RcvSelectCCAckFromSchedule(message);
        }
        break;

        //TCΪVMѡ������CC����VNM�������������Դ��������Ϣ
        case EV_VNETLIB_GETRESOURCE_CC:
        {
            GetNetResourceOnCC(message);
        }
        break;

        //TCΪVMѡ������CC����VNM�������������Դ��������Ϣ
        case EV_VNM_GET_RESOURCE_FOR_CC_ACK:
        {
            RcvGetResCCAckFromSchedule(message);
        }
        break;
    
        //TC��VNM����ΪVM�ͷ�������Դ��������Ϣ
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
    
        //����CC��VNM��ȡ����VM����������HC�����б���Ϣ
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
            
        //CCΪVMѡ������HC����VNM�������PCI��Դ��������Ϣ
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
        
        //CC��VNM����ΪVM�ͷ�������Դ��������Ϣ
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
* �������ƣ� SelectCCListForVM
* ���������� ����TC��VNM��ȡ����VM����������CC�����б���Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� vmniccfg
* ��������� cc_list
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
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


    //����������ִ��
    if (m_cVMSchTaskInfo.m_nVmID)
    {
        //��������ִ��
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

        //��scheduleģ��ת����Ϣ
        MID mid;
        mid._application = ApplicationName();
        mid._process = PROC_VNM;
        mid._unit = MU_VNM_SCHEDULE;
        
        MessageOption option(mid, EV_VNM_SCHEDULE_CC,0,0);
        m_pMU->Send(cMsg,option);
        
        return;
    }

SEND_ACK:

    //��д������Ϣ
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
* �������ƣ� RcvSelectCCAckFromSchedule
* ���������� �����schedule�յ�CC�����б�Ӧ����Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� vmniccfg
* ��������� cc_list
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
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
* �������ƣ� GetNetResourceOnCC
* ���������� TCΪVMѡ������CC����VNM�������������Դ��������Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� vmniccfg��HC��Ϣ
* ��������� pci��Ϣ
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
***************************************************************/
void CVNetVmMgr::GetNetResourceOnCC(const MessageFrame& message)
{
    int32 iRet = SUCCESS; 
    vector<CVNetVnicConfig> vecVnicCfg;      //��VNM��д�������VLAN,MAC,IP����Ϣ
    
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
        //��������ִ��
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

        //��scheduleģ��ת����Ϣ
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
* �������ƣ� RcvGetResCCAckFromSchedule
* ���������� �����schedule�յ�TC��Դ����Ӧ����Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� vmniccfg
* ��������� cc_list
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
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
* �������ƣ� FreeNetResourceOnCC
* ���������� TC��VNM����ΪVM�ͷ�������Դ��������Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� vmniccfg��HC��Ϣ
* ��������� pci��Ϣ
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
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
        //��������ִ��
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

        //��scheduleģ��ת����Ϣ
        MID mid;
        mid._application = ApplicationName();
        mid._process = PROC_VNM;
        mid._unit = MU_VNM_SCHEDULE;

        MessageOption option(mid, EV_VNM_FREE_RESOURCE_FOR_CC,0,0);
        m_pMU->Send(cMsg,option);

        return;
    }


SEND_ACK:
    //����Ҫ����VM������ʧ�������������Ϣ
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
* �������ƣ� RcvFreeResCCAckFromSchedule
* ���������� �����schedule�յ�TC��Դ�ͷ�Ӧ����Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� vmniccfg
* ��������� cc_list
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
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
* �������ƣ� SelectCCListForVM
* ���������� ����CC��VNM��ȡ����VM����������HC�����б���Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� vmniccfg��CC��Ϣ
* ��������� hc_list
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
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
        //��������ִ��
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

        //��scheduleģ��ת����Ϣ
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
* �������ƣ� RcvSelectHCAckFromSchedule
* ���������� �����schedule�յ�CC��Դ����Ӧ����Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� vmniccfg
* ��������� cc_list
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
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
* �������ƣ� GetNetResourceOnHC
* ���������� CCΪVMѡ������HC����VNM�������PCI��Դ��������Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� vmniccfg��HC��Ϣ
* ��������� pci��Ϣ
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
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
        //��������ִ��
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

        //��scheduleģ��ת����Ϣ
        MID mid;
        mid._application = ApplicationName();
        mid._process = PROC_VNM;
        mid._unit = MU_VNM_SCHEDULE;

        MessageOption option(mid, EV_VNM_GET_RESOURCE_FOR_HC,0,0);
        m_pMU->Send(cMsg,option);
        
        return;
    }
        

SEND_ACK:
    //��CC��Ӧ����Ϣ
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
* �������ƣ� RcvGetResHCAckFromSchedule
* ���������� �����schedule�յ�CC��Դ����Ӧ����Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� vmniccfg
* ��������� cc_list
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
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
* �������ƣ� FreeNetResourceOnHC
* ���������� CC��VNM����ΪVM�ͷ�������Դ��������Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� vmniccfg��HC��Ϣ
* ��������� pci��Ϣ
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
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
        //��������ִ��
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

        //��scheduleģ��ת����Ϣ
        MID mid;
        mid._application = ApplicationName();
        mid._process = PROC_VNM;
        mid._unit = MU_VNM_SCHEDULE;

        MessageOption option(mid, EV_VNM_FREE_RESOURCE_FOR_HC,0,0);
        m_pMU->Send(cMsg,option);
        
        return;
    }

SEND_ACK:
    //����Ҫ����VM������ʧ�������������Ϣ
    if (0 == iRet)
    {
        //��CC��Ӧ����Ϣ
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
* �������ƣ� RcvFreeResHCAckFromSchedule
* ���������� �����schedule�յ�CC��Դ�ͷ�Ӧ����Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� vmniccfg
* ��������� cc_list
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
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
* �������ƣ� DbgShowData
* ���������� ������Ϣ��ӡ
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
