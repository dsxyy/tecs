/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�vnet_vnic_mgr.cpp
* �ļ���ʶ��
* ����ժҪ��CVNetVnicMgr��ʵ���ļ�
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
#include "vnm_vm_mgr_ext.h"
#include "vm_migrate_mgr.h"

namespace zte_tecs
{
CVNetVmMgrExt *CVNetVmMgrExt::s_pInstance = NULL;

#define VNM_VNIC_CHECK_VSI_SYN_TIMER_INTERVAL    (2000)  // 5sɨ������һ��VNM��Ϣ;
#define VNM_VNIC_CHECK_VSI_SYN_TIMER_MAX             (10)

CVNetVmMgrExt::CVNetVmMgrExt()
{
    m_pMU = NULL;
    m_pVNetVmDB = NULL;
    m_pVNetVnicDB = NULL;
    m_pVSIProfileMgr = NULL;
    m_pSwitchMgr = NULL;
    m_pVNAMgr = NULL;
    m_bOpenDbgInf = FALSE;
};

CVNetVmMgrExt::~CVNetVmMgrExt()
{
    m_pMU = NULL;
    m_pVNetVmDB = NULL;
    m_pVNetVnicDB = NULL;
    m_pVSIProfileMgr = NULL;
    m_pSwitchMgr = NULL;
    m_pVNAMgr = NULL;
    m_bOpenDbgInf = FALSE;
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
int32 CVNetVmMgrExt::Init(MessageUnit *mu)
{
    if (mu == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetVmMgrExt::Init: mu is NULL.\n");
        return -1;
    }
    
    m_pMU = mu;

    m_pVNetVmDB = CVNetVmDB::GetInstance();
    if( NULL == m_pVNetVmDB)
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
void CVNetVmMgrExt::MessageEntry(const MessageFrame &message)
{
    //�����յ�����Ϣid���д���
    switch (message.option.id())
    {


        //VNA��VNM����VSI��ϸ��Ϣ
        case EV_VNA_REQUEST_VMVSI:
        {
            VMVSIInfoReq(message);
        }
        break;

        //VNA����ע���VNM��VNA�·�VSI��ϸ��Ϣ��ʱ������ʱδ��
        case TIMER_VNM_VNIC_CHECK_VSI_SYN:
        {
            VMVSISynTimerProc(message);
        }
        break;
        
        //VNA����ע���VNM��VNA�·�VSI��ϸ��Ϣ����ʱδ��
        case EV_VNM_NOTIFY_VNA_ALL_VMVSI_ACK:
        {
            VMVSIInfoSynAck(message);
        }
        break;

        //TC��VNM����Vnic��ϸ��Ϣ
        case EV_VNETLIB_QUERY_VNICS:
        {
            VMVnicInfoReq(message);
        }
        break;

        //�����Ǩ�ƣ�CC��VNM�����Ǩ��HC�б�
        case EV_VNETLIB_SELECT_MIGRATE_HC:
        {
            SelectHCListToMigrate(message);
        }
        break;

        //�����Ǩ�ƣ�CC->VNM ����Ǩ�ƿ�ʼ
        case EV_VNETLIB_VM_MIGRATE_BEGIN:
        {
            VmMigrateBegin(message);
        }
        break;
        case EV_VNETLIB_VM_MIGRATE_END:
        {
            VmMigrateEnd(message);
        }
        break;
        
        default:
            break;
    }

}

/************************************************************
* �������ƣ� SelectHCListToMigrate
* ���������� ����CC��VNM��ȡǨ�Ƶ�HC�б�
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
void CVNetVmMgrExt::SelectHCListToMigrate(const MessageFrame& message)
{
    int32 iRet = 0;
    CVNetSelectHCListMsg    cMsg;
    CVNetSelectHCListMsgAck cMsgAck;

    cMsg.deserialize(message.data);
    if (m_bOpenDbgInf)
    {
        cout << "Receive SelectHCListToMigrate message:" <<endl;
        cMsg.Print();
    }
    
    iRet = SelectMigrateHC(cMsg, cMsgAck.m_select_hc_list);
    //��д������Ϣ
    VNET_SET_ACKMSG_REQUEST_INFO(cMsg, cMsgAck);
    cMsgAck.state     = iRet;
    cMsgAck.detail = GetVnetErrorInfo(cMsgAck.state);
    cMsgAck.progress  = (iRet == SCHEDULE_OPER_SUCCESS) ? 100 : 0;
    cMsgAck.action_id = cMsg.action_id;
    MessageOption option(message.option.sender(),EV_VNETLIB_SELECT_HC_ACK,0,0);
    m_pMU->Send(cMsgAck,option);

    if (m_bOpenDbgInf)
    {
        cout << "Send SelectHCListToMigrate ack message:" <<endl;
        cMsgAck.Print();
    }    
}

/************************************************************
* �������ƣ� SelectMigrateHC
* ���������� ���ȿ���Ǩ�Ƶ�HC��Ϣ
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
int32 CVNetVmMgrExt::SelectMigrateHC(CVNetSelectHCListMsg &cMsg, vector<string> &vecMigrateHC)
{
    int32 nRet = SCHEDULE_OPER_SUCCESS;

    //��Ϣ�����Ϸ��Լ��
    if (cMsg.m_VmNicList.empty() && (true != cMsg.m_WatchDogInfo.m_nIsWDGEnable))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::ScheduleMigrateHC: m_VmNicList is NULL.\n");
        return SCHEDULE_OPER_SUCCESS;
    }

    //����������Ӧ�����VNA
    set<string> setVNAUUID;
    nRet = GetMigrateShareVNA(cMsg, setVNAUUID);
    if(0 != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::ScheduleMigrateHC: GetMigrateShareVNA failed.\n");
        return -1;
    }

    //����VNA�Ҷ�ӦHC�б�
    string strCCApp;
    string strHCApp;
    set<string>::iterator iter = setVNAUUID.begin();
    for(; iter != setVNAUUID.end(); ++iter)
    {
        nRet = m_pVNAMgr->QueryVNAModInfo(*iter, strHCApp, strCCApp);
        if(0 != nRet)
        {
            continue;
        }
        if(strCCApp == cMsg.m_cc_application)
        {
            vecMigrateHC.push_back(strHCApp);
        }
    }
    return 0;
}


/************************************************************
* �������ƣ� VmMigrateBegin
* ���������� CC��VNM ����Ǩ��VM BEGIN��Ϣ
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
void CVNetVmMgrExt::VmMigrateBegin(const MessageFrame& message)
{
    int32 iRet = 0;
    
    CVNetVmMigrateBeginMsg cMsg;
    cMsg.deserialize(message.data);

    if (m_bOpenDbgInf)
    {
        cout << "Receive VMMigrateBegin message:" <<endl;
        cMsg.serialize();
    }

    // ���濼���Ƶ�������migrate���� 
    CVNetVmMigrateMgr *pVmMigrate = CVNetVmMigrateMgr::GetInstance();
    if( NULL == pVmMigrate )
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetVmMgrExt::VmMigrateBegin: CVNetVmMigrateMgr::GetInstance() is NULL.\n");
        iRet= ERROR_VNM_VM_MIGRATE_INSTANCE_IS_NULL;
        goto RETURN_PROC;
    }

    iRet = pVmMigrate->MigrateBegin(cMsg);
    if( iRet != 0 )
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetVmMgrExt::VmMigrateBegin: pVmMigrate->MigrateBegin() failed.\n");
        goto RETURN_PROC;
    }

RETURN_PROC:
    //��CC��Ӧ����Ϣ
    CVNetVmMigrateMsgAck cMsgAck;
    cMsgAck.m_vm_id = cMsg.m_vm_id;
    VNET_SET_ACKMSG_REQUEST_INFO(cMsg, cMsgAck);
    cMsgAck.state = iRet;   
    if(0 == iRet)
    {
        cMsgAck.progress = 100;
    }
    else
    {
        cMsgAck.progress = 0;
    }
    cMsgAck.action_id = cMsg.action_id;

    // ������Ϣ
    cMsgAck.detail = GetVnetErrorInfo(iRet);
    
    MessageOption cc_option(message.option.sender(), EV_VNETLIB_VM_MIGRATE_BEGIN_ACK,0,0);
    m_pMU->Send(cMsgAck,cc_option);

    if (m_bOpenDbgInf)
    {
        cout << "Send VmMigrateBegin ack message:" <<endl;
        cMsgAck.serialize();
    }

    // migrate Ǩ��BEGIN �ɹ�������dst host ���� vsi��Ϣ
    //���Ӧ��VNA�·����Ӹ�VM��ص�VSI��Ϣ
    if (0 == iRet)
    {
        CAddVMVSIInfoMsg cVsiMsg;
        cVsiMsg.m_cVMVSIInfo.m_nVMID = cMsg.m_vm_id;
        // cVsiMsg.m_cVMVSIInfo.m_cWatchDogInfo = cMsg.m_WatchDogInfo;
        // �����ȷ�ϣ�û������������£��Ƿ�����ÿ��Ź� 

        CVNetVnicDB* pVNetVnicDB = CVNetVnicDB::GetInstance();
        if( NULL == pVNetVnicDB)
        {
            cout << "CVNetVnicDB::GetInstance() is NULL" << endl;
            return;
        }

        iRet = pVNetVnicDB->GetVmVnicFromDb(cMsg.m_vm_id, cVsiMsg.m_cVMVSIInfo.m_vecVnicDetail);
        if ((iRet != 0) && (!VNET_DB_IS_ITEM_NO_EXIST(iRet)))
        {
            cout << "CVNetVnicDB->GetVmVnic failed." << iRet << endl;
            return;
        }
        
        iRet = m_pVSIProfileMgr->GetVSIDetailInfo(cMsg.m_vm_id, cVsiMsg.m_cVMVSIInfo);
        if (0 != iRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetVmMgrExt::VmMigrateEnd: call GetVSIDetailInfo failed, ret=%d.\n", iRet);
            return;
        }

        MID cVnaMID;
        cVnaMID._application = cMsg.m_dst_hc_application;
        cVnaMID._process = PROC_VNA;
        cVnaMID._unit = MU_VNA_CONTROLLER;
            
        MessageOption vna_option(cVnaMID, EV_VNM_NOTIFY_VNA_ADD_VMVSI,0,0);
        m_pMU->Send(cVsiMsg,vna_option);

        if (m_bOpenDbgInf)
        {
            cout << "Send CAddVMVSIInfoMsg message to VNA(" << cMsg.m_dst_hc_application <<"):" <<endl;
            cVsiMsg.Print();
        }
    }    
}



/************************************************************
* �������ƣ� VmMigrateEnd
* ���������� CC��VNM ����Ǩ��VM END��Ϣ
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
void CVNetVmMgrExt::VmMigrateEnd(const MessageFrame& message)
{
    int32 iRet = 0;
    
    CVNetVmMigrateEndMsg cMsg;
    cMsg.deserialize(message.data);

    if (m_bOpenDbgInf)
    {
        cout << "Receive VmMigrateEnd message:" <<endl;
        cMsg.serialize();
    }

    // ���濼���Ƶ�������migrate���� 
    CVNetVmMigrateMgr *pVmMigrate = CVNetVmMigrateMgr::GetInstance();
    if( NULL == pVmMigrate )
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetVmMgrExt::VmMigrateEnd: CVNetVmMigrateMgr::GetInstance() is NULL.\n");
        iRet= ERROR_VNM_VM_MIGRATE_INSTANCE_IS_NULL;
        goto RETURN_PROC;
    }

    iRet = pVmMigrate->MigrateEnd(cMsg);
    if( iRet != 0 )
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetVmMgrExt::VmMigrateEnd: pVmMigrate->MigrateEnd() failed.\n");
        goto RETURN_PROC;
    }

RETURN_PROC:
    //��CC��Ӧ����Ϣ
    CVNetVmMigrateMsgAck cMsgAck;
    cMsgAck.m_vm_id = cMsg.m_vm_id;
    VNET_SET_ACKMSG_REQUEST_INFO(cMsg, cMsgAck);
    cMsgAck.state = iRet;   
    if(0 == iRet)
    {
        cMsgAck.progress = 100;
    }
    else
    {
        cMsgAck.progress = 0;
    }
    cMsgAck.action_id = cMsg.action_id;
    
    // ������Ϣ
    cMsgAck.detail = GetVnetErrorInfo(iRet);
    
    MessageOption cc_option(message.option.sender(), EV_VNETLIB_VM_MIGRATE_END_ACK,0,0);
    m_pMU->Send(cMsgAck,cc_option);

    if (m_bOpenDbgInf)
    {
        cout << "Send VmMigrateEnd ack message:" <<endl;
        cMsgAck.serialize();
    }

    // migrate Ǩ��BEGIN �ɹ�������dst host ���� vsi��Ϣ
    //���Ӧ��VNA�·����Ӹ�VM��ص�VSI��Ϣ
    if (0 == iRet)
    {
        CAddVMVSIInfoMsg cVsiMsg;
        cVsiMsg.m_cVMVSIInfo.m_nVMID = cMsg.m_vm_id;
        // cVsiMsg.m_cVMVSIInfo.m_cWatchDogInfo = cMsg.m_WatchDogInfo;
        // �����ȷ�ϣ�û������������£��Ƿ�����ÿ��Ź� 

        CVNetVnicDB* pVNetVnicDB = CVNetVnicDB::GetInstance();
        if( NULL == pVNetVnicDB)
        {
            cout << "CVNetVnicDB::GetInstance() is NULL" << endl;
            return;
        }

        iRet = pVNetVnicDB->GetVmVnicFromDb(cMsg.m_vm_id, cVsiMsg.m_cVMVSIInfo.m_vecVnicDetail);
        if ((iRet != 0) && (!VNET_DB_IS_ITEM_NO_EXIST(iRet)))
        {
            cout << "CVNetVnicDB->GetVmVnic failed." << iRet << endl;
            return;
        }

        iRet = m_pVSIProfileMgr->GetVSIDetailInfo(cMsg.m_vm_id, cVsiMsg.m_cVMVSIInfo);
        if (0 != iRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetVmMgrExt::VmMigrateEnd: call GetVSIDetailInfo failed, ret=%d.\n", iRet);
            return;
        }

        MID cVnaMID;

        //����Ǩ�ƽ��������ɾ���Ǹ�VNA VSI��Ϣ
        if( 0 == cMsg.m_result )
        {
            cVnaMID._application = cMsg.m_src_hc_application;
        }
        else
        {
            cVnaMID._application = cMsg.m_dst_hc_application;
        }
        cVnaMID._process = PROC_VNA;
        cVnaMID._unit = MU_VNA_CONTROLLER;
            
        MessageOption vna_option(cVnaMID, EV_VNM_NOTIFY_VNA_DEL_VMVSI,0,0);
        m_pMU->Send(cVsiMsg,vna_option);

        if (m_bOpenDbgInf)
        {
            cout << "Send CDelVMVSIInfoMsg message to VNA(" << cVnaMID._application <<"):" <<endl;
            cVsiMsg.Print();
        }
    }    
}


/************************************************************
* �������ƣ� GetMigrateShareVNA
* ���������� ��ȡ��Ǩ��������ͬ��VNA�б�
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����      �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1        V1.0       lvech         ����
***************************************************************/
int32 CVNetVmMgrExt::GetMigrateShareVNA(CVNetSelectHCListMsg &cMsg, set<string> &setVNAUuid)
{
    int32 nRet;
    set<string> setFinal;
    CVNetVmMem cVnetVmMem;

    //��ʽת��
    nRet = VnicCfgTrans2Mem(cMsg.m_vm_id,cMsg.m_project_id, cMsg.m_VmNicList, cMsg.m_WatchDogInfo, cVnetVmMem);
    if (0 != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetVmMgrExt::GetNetResourceOnCC: call VnicCfgTrans2Mem failed, ret=%d.\n", nRet);
        return -1;
    }

    //�Ȳ�ѯVM��ز�����Ϣ
    cVnetVmMem.SetVmId(cMsg.m_vm_id);
    nRet = m_pVNetVmDB->GetVm(cVnetVmMem);
    if (0 != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetVmMgrExt::GetNetResourceOnCC: call GetVm failed, ret=%d.\n", nRet);
        return -1;
    }

    //ԭVNA����Ǩ���б��У���Ҫ�޳�
    string strOldVNA;
    nRet = m_pVNAMgr->QueryVNAUUID(cVnetVmMem.GetClusterName(), cVnetVmMem.GetHostName(), strOldVNA);
    if (0 != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetVmMgrExt::GetMigrateShareVNA: call QueryVNAUUID failed, ret=%d.\n", nRet);
        return -1;
    }
        
    vector<CVNetVnicConfig>::iterator iter = cMsg.m_VmNicList.begin();
    for(; iter != cMsg.m_VmNicList.end(); ++iter)
    {
        //����VSI�ҵ�DVS��Ϣ
        CVNetVnicMem cVnicMem;
        cVnicMem.SetVmId(cMsg.m_vm_id);
        cVnicMem.SetNicIndex(iter->m_nNicIndex);
        nRet = m_pVNetVnicDB->GetVnic(cVnicMem);
        if(0 != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetVmMgrExt::GetMigrateShareVNA: GetVnic Failed, ret =%d\n", nRet);
            return -1;
        }

        //����DVS�ҵ���ӦVNA��Ϣ
        set<string> setCurVNA;
        nRet = m_pSwitchMgr->GetSwitchOnlineVNA(cVnicMem.GetVSwitchUuid(), setCurVNA);
        if(0 != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetVmMgrExt::GetMigrateShareVNA: GetSwitchOnlineVNA Failed, ret =%d\n", nRet);
            return -1;
        }
        
        set<string>::iterator itr_del = setCurVNA.begin();
        for(; itr_del != setCurVNA.end(); ++itr_del)
        {
            if((*itr_del) == strOldVNA)
            {
                setCurVNA.erase(itr_del);
                break;  //setԪ�ز��ظ�
            }
        }

        //ÿ��������ӦVNA�󽻼�
        if(iter == cMsg.m_VmNicList.begin())
        {
            setFinal = setCurVNA;
        }
        else
        {
            set<string>::iterator itr = setFinal.begin();
            set_intersection(setFinal.begin(),setFinal.end(),setCurVNA.begin(),setCurVNA.end(),inserter(setFinal,itr));
        }
    }
    
    setVNAUuid = setFinal;
    
    //�Ƿ������Ź�
    set<string> setWdFinal;
    if(cMsg.m_WatchDogInfo.m_nIsWDGEnable)
    {
        set<string>::iterator it_set = setFinal.begin();
        for(; it_set != setFinal.end(); ++it_set)
        {
            //VNA�Ƿ�ע��WatchDog����
            nRet = m_pVNAMgr->QueryVNAModRole(*it_set, VNA_MODULE_ROLE_WATCHDOG);
            if(0 == nRet)
            {
                setWdFinal.insert(*it_set);
                continue;
            }
        }
        setVNAUuid = setWdFinal;
    }   
    return 0;
}

/************************************************************
* �������ƣ� VMVSIInfoReq
* ���������� VNA��VNM����VSI��ϸ��Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� 
* ��������� pci��Ϣ
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
***************************************************************/
void CVNetVmMgrExt::VMVSIInfoReq(const MessageFrame& message)
{
    int32 iRet = 0;
    CVMVSIInfoReqMsg cReqMsg;
    cReqMsg.deserialize(message.data);

    if (m_bOpenDbgInf)
    {
        cout << "Receive VMVSIInfoReq message:" <<endl;
        cReqMsg.Print();
    }
    
    //�ȴ�DB��ȡVSI�����Ϣ���cMsgAck��
    CVMVSIInfoReqAckMsg cMsg;
    cMsg.m_cVMVSIInfo.m_nVMID = cReqMsg.m_nVMID;
    cMsg.m_cVMVSIInfo.SetVnicInfo(cReqMsg.m_vecVnicCfg);

    iRet = m_pVSIProfileMgr->GetVSIDetailInfo(message.option.sender()._application, cReqMsg.m_nVMID, cMsg.m_cVMVSIInfo);
    if (0 != iRet)
    {
        return;
    }

    cMsg.m_nReturnCode = iRet;
    
    MessageOption vna_option(message.option.sender(), EV_VNA_REQUEST_VMVSI_ACK,0,0);
    m_pMU->Send(cMsg,vna_option);

    if (m_bOpenDbgInf)
    {
        cout << "Send VMVSIInfoReq ack:" <<endl;
        cMsg.Print();
    }
};

/************************************************************
* �������ƣ� VMVnicInfoReq
* ���������� TC��VNM����Vnic��ϸ��Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� 
* ��������� pci��Ϣ
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
***************************************************************/
void CVNetVmMgrExt::VMVnicInfoReq(const MessageFrame& message)
{
    // ����Ϣת����vnm query��Ϣ��Ԫ����     
//  int32 iRet = 0;
    CQueryVmNicInfoMsg cMsg;
    cMsg.deserialize(message.data);

    if (m_bOpenDbgInf)
    {
        cout << "CVNetVmMgr: Receive SelectCCListForVM message:" <<endl;
        cout << cMsg.serialize();
    }
    
    //��scheduleģ��ת����Ϣ
    MID mid;
    mid._application = ApplicationName();
    mid._process = PROC_VNM;
    mid._unit = MU_VNM_QUERY;

    MessageOption option(mid, EV_VNETLIB_QUERY_VNICS,0,0);
    m_pMU->Send(cMsg,option);

    return;

};

int32 CVNetVmMgrExt::AddVMVSISynTask(const string & strVNAUuid)
{
    T_VSISynTimerInfo sCheckVMTimerInfo;
    TIMER_ID tCheckSynTimerId;
    
    //���Ҷ�Ӧ�Ķ�ʱ��
    vector<T_VSISynTimerInfo>::iterator itr = m_vecVMVSISynTimer.begin();
    for ( ; itr != m_vecVMVSISynTimer.end(); )
    {
        if (itr->strVNAUuid == strVNAUuid)
        {
            itr->nTimerOutCnt = 0;
            return 0;
        }
        else
        {
            ++itr;
        }
    }
    
    /*������ʱ��ɨ��VM���������ȴ�VM���������ɺ󣬶�������VLAN��Ϣ*/
    tCheckSynTimerId = m_pMU->CreateTimer();
    if (INVALID_TIMER_ID == tCheckSynTimerId)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetVmMgrExt::StartVMVSISynTimer: Create del vnet timer failed.\n");

        return -1;
    }

    TimeOut  tTmOut;
    tTmOut.duration = VNM_VNIC_CHECK_VSI_SYN_TIMER_INTERVAL;
    tTmOut.msgid = TIMER_VNM_VNIC_CHECK_VSI_SYN;
    tTmOut.type = LOOP_TIMER;
    tTmOut.str_arg = strVNAUuid;
    if (SUCCESS != m_pMU->SetTimer(tCheckSynTimerId, tTmOut))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetVmMgrExt::StartVMVSISynTimer: Set timer(id:%d) failed.\n", tCheckSynTimerId);
        return -1;
    }

    memset(&sCheckVMTimerInfo, 0, sizeof(sCheckVMTimerInfo));
    sCheckVMTimerInfo.tTimerID = tCheckSynTimerId;
    sCheckVMTimerInfo.nTimerOutCnt = 0;
    sCheckVMTimerInfo.strVNAUuid = strVNAUuid;

    m_vecVMVSISynTimer.push_back(sCheckVMTimerInfo);

    return 0;
}

/************************************************************
* �������ƣ� VMVSISynTimerProc
* ���������� VNM��VNA�·�����¼�������Ѳ����VM��ص�VSI��Ϣ��ʱ��
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
void CVNetVmMgrExt::VMVSISynTimerProc(const MessageFrame& message)
{
    //���VM�Ƿ��Ѿ�Ǩ�ƹ���
    TIMER_ID    timer_id  = static_cast<TIMER_ID>(m_pMU->GetTimerId());

    //���Ҷ�Ӧ�Ķ�ʱ��
    vector<T_VSISynTimerInfo>::iterator itr = m_vecVMVSISynTimer.begin();
    for ( ; itr != m_vecVMVSISynTimer.end(); ++itr)
    {
        if (itr->tTimerID == timer_id)
        {
            break;
        }
    }

    if (itr != m_vecVMVSISynTimer.end())
    {
        itr->nTimerOutCnt++;

        SynVMVSIInfoToVNA(itr->strVNAUuid);

        //����Ƿ񵽴����ʱ����
        if (itr->nTimerOutCnt > VNM_VNIC_CHECK_VSI_SYN_TIMER_MAX)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetVmMgrExt::VMVSISynTimerProc: VNA(UUID:%s) timer reach the max times, delete it.\n", itr->strVNAUuid.c_str());
            m_pMU->KillTimer(timer_id);
            m_vecVMVSISynTimer.erase(itr);
            return;
        }
    }
    else
    {
        m_pMU->KillTimer(timer_id);
        VNET_LOG(VNET_LOG_ERROR, "CVNetVmMgrExt::VMVSISynTimerProc: Find timer(id:%d) failed.\n", timer_id);
        return;
    }    

    return;
}

/************************************************************
* �������ƣ� SynVMVSIInfoToVNA
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
void CVNetVmMgrExt::SynVMVSIInfoToVNA(const string& strVNAUuid)
{
    CVMVSIInfo cVMVnicInfo;

    CVNMAddAllVMVSIInfoMsg cMsg;

    //���ӻ��߸�������VMVSI��Ϣ
    CVNetVnicDB *pVnicDB = CVNetVnicDB::GetInstance();

    vector<CDbVsiInfoSummary> vecAllVnic;
    int iRet = pVnicDB->QueryAllVnic(vecAllVnic);
    if (0 != iRet)
    {
        cout << "call QueryAllVnic failed, return=" << iRet <<endl;
    }

    CDBOperateVsi * pOper = GetDbIVsi();
    if (NULL == pOper)
    {
        cout << "GetDbIVsi() is NULL" << endl;
        return;
    }
    
    vector<CDbVsiInfoSummary>::iterator itrVM = vecAllVnic.begin();
    for ( ; itrVM != vecAllVnic.end(); ++itrVM)
    {
        CDbVsiInfo cDbVsiInfo;
        cDbVsiInfo.SetVmId(itrVM->GetVmId());
        cDbVsiInfo.SetNicIndex(itrVM->GetNicIndex());
        
        int iRet = pOper->QueryByVmVnic(cDbVsiInfo);
        if (iRet != 0)
        {
            cout << "QueryByVmVnic failed." << iRet << endl;
            return;
        }
        else
        {
            cDbVsiInfo.DbgShow();
        }
    }

    MID cVnaMID;
    cVnaMID._application = strVNAUuid;
    cVnaMID._process = PROC_VNA;
    cVnaMID._unit = MU_VNA_CONTROLLER;
    MessageOption option(cVnaMID,EV_VNM_NOTIFY_VNA_ALL_VMVSI,0,0);
    m_pMU->Send("",option);
};


/************************************************************
* �������ƣ� VMVSIInfoSynAck
* ���������� �յ� VNA��VSIͬ����ϢӦ������
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
void CVNetVmMgrExt::VMVSIInfoSynAck(const MessageFrame& message)
{
    //���Ҷ�Ӧ�Ķ�ʱ��
    vector<T_VSISynTimerInfo>::iterator itr = m_vecVMVSISynTimer.begin();
    for ( ; itr != m_vecVMVSISynTimer.end(); ++itr)
    {
        if (itr->strVNAUuid == message.option.sender()._application)
        {
            m_pMU->KillTimer(itr->tTimerID);
            m_vecVMVSISynTimer.erase(itr);
            break;
        }
    }
}



void CVNetVmMgrExt::DbgShowData(void)
{
    cout << "VM interface timer process information begin:" << endl;

    cout << "-----------------------------------------------" << endl;
    cout << "VM interface timer process information end" << endl;
    cout << endl;
    return;
};

/************************************************************
* �������ƣ� VnicCfgTrans2Mem
* ���������� vnic��ʽת��
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
int32 VnicCfgTrans2Mem(const int64 nVmID, 
                    const int64 nProjectID,  
                    vector<CVNetVnicConfig> &vecVnicCfg, 
                    CWatchDogInfo   &cWatchDogInfo, 
                    CVNetVmMem &cVnetVmMem)
{
    CLogicNetworkMgr *pMgr = CLogicNetworkMgr::GetInstance();
    if (NULL == pMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetVmMgrExt::GetPgUuidForVnic: CLogicNetworkMgr::GetInstance() == NULL.\n");
        return ERR_VNMVNIC_GET_LN_MGR_INST_FAIL;
    }
    
    cVnetVmMem.SetVmId(nVmID);
    cVnetVmMem.SetProjectId(nProjectID);
    cVnetVmMem.SetIsHasWatchdog(cWatchDogInfo.m_nIsWDGEnable);
    cVnetVmMem.SetWatchdogTimeout(cWatchDogInfo.m_nTimeOut);

    vector<CVNetVnicMem> vecVnicMem;

    vector< CVNetVnicConfig>::iterator itrVM = vecVnicCfg.begin();
    for ( ; itrVM != vecVnicCfg.end(); ++itrVM)
    {
        CVNetVnicMem cVnetVnicMem;
        //transfor
        cVnetVnicMem.SetVmId (nVmID) ;   
        cVnetVnicMem.SetNicIndex(itrVM->m_nNicIndex);
        cVnetVnicMem.SetIsSriov(itrVM->m_nIsSriov) ;   
        cVnetVnicMem.SetIsLoop(itrVM->m_nIsLoop) ;  
        cVnetVnicMem.SetLogicNetworkUuid(itrVM->m_strLogicNetworkID) ; 
        cVnetVnicMem.SetAssIP(itrVM->m_strIP);
        cVnetVnicMem.SetAssMask(itrVM->m_strNetmask);
        cVnetVnicMem.SetAssGateway(itrVM->m_strGateway);
        cVnetVnicMem.SetAssMac(itrVM->m_strMac);
        cVnetVnicMem.SetPortType(itrVM->m_strAdapterModel) ;  
        cVnetVnicMem.SetVsiIdValue(itrVM->m_strVSIProfileID) ;  
        
        CLogicNetwork cLogicNetwork("");  
        if (0 != pMgr->GetLogicNetwork(itrVM->m_strLogicNetworkID, cLogicNetwork))
        {
            VNET_LOG(VNET_LOG_ERROR, "VnicCfgTrans2Mem: Can't get LogicNetwork{UUID: %s} data.\n",
                itrVM->m_strLogicNetworkID.c_str());
            return ERR_VNMVNIC_GET_LN_INST_FAIL;
        }  
        
        cVnetVnicMem.SetVmPgUuid(cLogicNetwork.GetPortGroupUUID());
        
        vecVnicMem.push_back(cVnetVnicMem);
    }

    cVnetVmMem.SetVecVnicMem(vecVnicMem);
    
    return 0;
};

/************************************************************
* �������ƣ� VnicMemTrans2Cfg
* ���������� vnic��ʽת��
* ���ʵı� ��
* �޸ĵı� ��
* ��������� 
* ��������� pci��Ϣ
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
***************************************************************/
void VnicMemTrans2Cfg( vector<CVNetVnicMem> &vecVmVnicMem, vector<CVNetVnicConfig> &vecVnicCfg)
{
    vecVnicCfg.clear();

    vector<CVNetVnicMem>::iterator itrVM = vecVmVnicMem.begin();
    for ( ; itrVM != vecVmVnicMem.end(); ++itrVM)
    {
        CVNetVnicConfig cVmVnic;
        //transfor
        cVmVnic.m_nNicIndex = itrVM->GetNicIndex();
        cVmVnic.m_nIsSriov = itrVM->GetIsSriov();   
        cVmVnic.m_nIsLoop = itrVM->GetIsLoop();  
        cVmVnic.m_strLogicNetworkID = itrVM->GetLogicNetworkUuid();
        cVmVnic.m_strIP = itrVM->GetAssIP();
        cVmVnic.m_strNetmask = itrVM->GetAssMask();
        cVmVnic.m_strGateway = itrVM->GetAssGateway();
        cVmVnic.m_strMac = itrVM->GetAssMac();
        cVmVnic.m_strAdapterModel = itrVM->GetPortType();  
        cVmVnic.m_strVSIProfileID = itrVM->GetVsiIdValue();  
        
        vecVnicCfg.push_back(cVmVnic);
    }
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

void VNM_SET_VM_VNIC1( int32   nIsSriov,
                            int32   nIsLoop,
                            char * strLogicNetworkID,
                            char * strVsiUuid)
{
    g_nIsSriov1 = nIsSriov;
    g_nIsLoop1 = nIsLoop;
    g_strLogicNetworkID1 = strLogicNetworkID;
    g_strVsiUuid1 = strVsiUuid;
};
DEFINE_DEBUG_FUNC(VNM_SET_VM_VNIC1);

void VNM_SET_VM_VNIC2( int32   nIsSriov,
                            int32   nIsLoop,
                            char * strLogicNetworkID,
                            char * strVsiUuid)
{
    g_nIsSriov2 = nIsSriov;
    g_nIsLoop2 = nIsLoop;
    g_strLogicNetworkID2 = strLogicNetworkID;
    g_strVsiUuid2 = strVsiUuid;
};
DEFINE_DEBUG_FUNC(VNM_SET_VM_VNIC2);

void VNM_SET_VM_VNIC3( int32   nIsSriov,
                            int32   nIsLoop,
                            char * strLogicNetworkID,
                            char * strVsiUuid)
{
    g_nIsSriov3 = nIsSriov;
    g_nIsLoop3 = nIsLoop;
    g_strLogicNetworkID3 = strLogicNetworkID;
    g_strVsiUuid3 = strVsiUuid;
};
DEFINE_DEBUG_FUNC(VNM_SET_VM_VNIC3);

void VNM_DBG_SEND_SELECT_CC(int64 nVMID, 
                            int64 nProjectID,
                            int32 nIsWdgEnable,
                            char * strAppointCC)
{
    CVNetSelectCCListMsg cMsg;
    cMsg.m_vm_id= nVMID;
    cMsg.m_project_id= nProjectID;
    cMsg.m_appointed_cc = strAppointCC;

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

        cMsg.m_VmNicList.push_back(cVnicCfg);
    }

    if (nIsWdgEnable)
    {
        cMsg.m_WatchDogInfo.m_nIsWDGEnable = 1;
        cMsg.m_WatchDogInfo.m_nTimeOut = 30;
    }

    MessageUnit temp_mu(TempUnitName("select_cc"));
    temp_mu.Register();
    MID cVnaMID;
    cVnaMID._application = ApplicationName();
    cVnaMID._process = PROC_VNM;
    cVnaMID._unit = MU_VNM;

    MessageOption vna_option(cVnaMID, EV_VNETLIB_SELECT_CC,0,0);
    temp_mu.Send(cMsg,vna_option);
    MessageFrame message;

    if (SUCCESS == temp_mu.Wait(&message, 2000))
    {
        CVNetSelectCCListMsgAck ack;
        ack.deserialize(message.data);

        cout << "Receive ack: ret=" <<ack.state << endl;
        ack.Print();
    }

    return;
};
DEFINE_DEBUG_FUNC(VNM_DBG_SEND_SELECT_CC);

void VNM_DBG_GET_SOURCE_ON_CC(int64 nVMID, 
                            int64 nProjectID,
                            int32 nIsWdgEnable,
                            char * strClusterName)
{
    CVNetGetResourceCCMsg cMsg;
    cMsg.m_vm_id = nVMID;
    cMsg.m_project_id = nProjectID;
    cMsg.m_cc_application = strClusterName;

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
    
    MessageUnit temp_mu(TempUnitName("get_source_cc"));
    temp_mu.Register();
    MID cVnaMID;
    cVnaMID._application = ApplicationName();
    cVnaMID._process = PROC_VNM;
    cVnaMID._unit = MU_VNM;

    MessageOption vna_option(cVnaMID, EV_VNETLIB_GETRESOURCE_CC,0,0);
    temp_mu.Send(cMsg,vna_option);
    MessageFrame message;

    if (SUCCESS == temp_mu.Wait(&message, 2000))
    {
        CVNetGetResourceCCMsgAck ack;
        ack.deserialize(message.data);

        cout << "Receive ack: ret=" <<ack.state << endl;
        ack.Print();
    }

    return;
};
DEFINE_DEBUG_FUNC(VNM_DBG_GET_SOURCE_ON_CC);

void VNM_DBG_FREE_SOURCE_ON_CC(int64 nVMID, 
                            int64 nProjectID,
                            int32 nIsWdgEnable,
                            char * strClusterName)
{
    CVNetFreeResourceCCMsg cMsg;
    cMsg.m_vm_id = nVMID;
    cMsg.m_project_id = nProjectID;
    cMsg.m_cc_application = strClusterName;

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
    

    MessageUnit temp_mu(TempUnitName("free_source_cc"));
    temp_mu.Register();
    MID cVnaMID;
    cVnaMID._application = ApplicationName();
    cVnaMID._process = PROC_VNM;
    cVnaMID._unit = MU_VNM;

    MessageOption vna_option(cVnaMID, EV_VNETLIB_FREERESOURCE_CC,0,0);
    temp_mu.Send(cMsg,vna_option);
    MessageFrame message;

    if (SUCCESS == temp_mu.Wait(&message, 2000))
    {
        CVNetFreeResourceCCMsgAck ack;
        ack.deserialize(message.data);

        cout << "Receive ack: m_result" << ack.state<< endl;
        ack.Print();
    }

    return;
};
DEFINE_DEBUG_FUNC(VNM_DBG_FREE_SOURCE_ON_CC);

void VNM_DBG_SEND_SELECT_HC(int64 nVMID, 
                            int64 nProjectID,
                            int32 nIsWdgEnable,
                            char * strClusterName,
                            char * strAppointHC)
                            
{
    CVNetSelectHCListMsg cMsg;
    cMsg.m_vm_id = nVMID;
    cMsg.m_project_id = nProjectID;
    cMsg.m_cc_application = strClusterName;
    cMsg.m_appointed_hc = strAppointHC;

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

        cMsg.m_VmNicList.push_back(cVnicCfg);
    }

    if (nIsWdgEnable)
    {
        cMsg.m_WatchDogInfo.m_nIsWDGEnable = 1;
        cMsg.m_WatchDogInfo.m_nTimeOut = 30;
    }
    

    MessageUnit temp_mu(TempUnitName("get_source_cc"));
    temp_mu.Register();
    MID cVnaMID;
    cVnaMID._application = ApplicationName();
    cVnaMID._process = PROC_VNM;
    cVnaMID._unit = MU_VNM;

    MessageOption vna_option(cVnaMID, EV_VNETLIB_SELECT_HC,0,0);
    temp_mu.Send(cMsg,vna_option);
    MessageFrame message;

    if (SUCCESS == temp_mu.Wait(&message, 2000))
    {
        CVNetSelectHCListMsgAck ack;
        ack.deserialize(message.data);

        cout << "Receive ack: ret=" <<ack.state << endl;
        ack.Print();
    }

    return;
}
DEFINE_DEBUG_FUNC(VNM_DBG_SEND_SELECT_HC);


void VNM_DBG_GET_SOURCE_ON_HC(int64 nVMID, 
                            int64 nProjectID,
                            int32 nIsWdgEnable,
                            char * strClusterName,
                            char * strAppointHC)
{
    CVNetGetResourceHCMsg cMsg;
    cMsg.m_vm_id = nVMID;
    cMsg.m_project_id = nProjectID;
    cMsg.m_cc_application = strClusterName;
    cMsg.m_hc_application = strAppointHC;

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
    

    MessageUnit temp_mu(TempUnitName("get_source_cc"));
    temp_mu.Register();
    MID cVnaMID;
    cVnaMID._application = ApplicationName();
    cVnaMID._process = PROC_VNM;
    cVnaMID._unit = MU_VNM;

    MessageOption vna_option(cVnaMID, EV_VNETLIB_GETRESOURCE_HC,0,0);
    temp_mu.Send(cMsg,vna_option);
    MessageFrame message;

    if (SUCCESS == temp_mu.Wait(&message, 2000))
    {
        CVNetGetResourceHCMsgAck ack;
        ack.deserialize(message.data);

        cout << "Receive ack: ret=" <<ack.state << endl;
        ack.Print();
    }

    return;
}
DEFINE_DEBUG_FUNC(VNM_DBG_GET_SOURCE_ON_HC);

void VNM_DBG_FREE_SOURCE_ON_HC(int64 nVMID, 
                            int64 nProjectID,
                            int32 nIsWdgEnable,
                            char * strClusterName,
                            char * strAppointHC)
{
    CVNetFreeResourceHCMsg cMsg;
    cMsg.m_vm_id = nVMID;
    cMsg.m_project_id = nProjectID;
    cMsg.m_cc_application = strClusterName;
    cMsg.m_hc_application = strAppointHC;

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
    
    MessageUnit temp_mu(TempUnitName("get_source_cc"));
    temp_mu.Register();
    MID cVnaMID;
    cVnaMID._application = ApplicationName();
    cVnaMID._process = PROC_VNM;
    cVnaMID._unit = MU_VNM;

    MessageOption vna_option(cVnaMID, EV_VNETLIB_FREERESOURCE_HC,0,0);
    temp_mu.Send(cMsg,vna_option);
    MessageFrame message;

    if (SUCCESS == temp_mu.Wait(&message, 2000))
    {
        CVNetFreeResourceHCMsgAck ack;
        ack.deserialize(message.data);

        cout << "Receive ack: ret=" <<ack.state << endl;
        ack.Print();
    }

    return;
}
DEFINE_DEBUG_FUNC(VNM_DBG_FREE_SOURCE_ON_HC);

void VNM_DBG_SEND_QUERY_VNIC(int64 nVMID)
{
    CQueryVmNicInfoMsg cMsg;
    cMsg.m_vm_id= nVMID;

    MessageUnit temp_mu(TempUnitName("query_vnic"));
    temp_mu.Register();
    MID cVnmMID;
    cVnmMID._application = ApplicationName();
    cVnmMID._process = PROC_VNM;
    cVnmMID._unit = MU_VNM;

    MessageOption vna_option(cVnmMID, EV_VNETLIB_QUERY_VNICS,0,0);
    temp_mu.Send(cMsg,vna_option);
    MessageFrame message;

    if (SUCCESS == temp_mu.Wait(&message, 2000))
    {
        CQueryVmNicInfoMsgAck ack;
        ack.deserialize(message.data);

        cout << "Receive ack: ret=" <<ack.state << endl;
        ack.Print();
    }

    return;
};
DEFINE_DEBUG_FUNC(VNM_DBG_SEND_QUERY_VNIC);

void VNM_DBG_SET_VM_MGR_EXT_DBG_FLAG(int bSet)
{    
    if ((0 != bSet) && (1 != bSet))
    {
        cout << "Invalid flag parameter, please input 0 or 1." <<endl;
        return ;
    }

    CVNetVmMgrExt *pVmMgr = CVNetVmMgrExt::GetInstance();
    if (!pVmMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "SetModeDbgFlag call CVNetVmMgrExt::GetInstance() failed.\n");
        return;
    }

    pVmMgr->SetDbgFlag(bSet);
};
DEFINE_DEBUG_FUNC(VNM_DBG_SET_VM_MGR_EXT_DBG_FLAG);

}

