/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�vnm_vnic_db.cpp
* �ļ���ʶ��
* ����ժҪ��CVNetVnicDB��ʵ���ļ�
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
#include "vnet_db_mgr.h"
#include "vnet_vnic.h"
#include "vnet_db_vsi.h"
#include "vna_reg_mod.h"
#include "vnet_msg.h"
#include "vna_info.h"
#include "ipv4_addr.h"
#include "ipv4_addr_range.h"
#include "ipv4_addr_pool.h"
#include "ipv4_pool_mgr.h"
#include "logic_network.h"
#include "logic_network_mgr.h"
#include "vnet_portmgr.h"
#include "vna_mgr.h"
#include "vnm_vnic_db.h"

namespace zte_tecs
{
CVNetVnicDB* CVNetVnicDB::_instance = NULL;

CVNetVnicDB::CVNetVnicDB()
{
    m_pVNetDbMgr = NULL;
    m_pOper = NULL;
};

CVNetVnicDB::~CVNetVnicDB()
{
    m_pVNetDbMgr = NULL;
    m_pOper = NULL;
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
int32 CVNetVnicDB::Init()
{
    m_pVNetDbMgr = CVNetDbMgr::GetInstance();
    if( NULL == m_pVNetDbMgr)
    {
        cout << "CVNetDbMgr::GetInstance() is NULL" << endl;
        return -1;
    }

    m_pOper = GetDbIVsi();
    if (NULL == m_pOper)
    {
        cout << "GetDbIVsi() is NULL" << endl;
        return -1;
    }
    
    VNET_LOG(VNET_LOG_WARN, "CVNetVmMgr::Init: Call  successfully.\n");

    return 0;
} ;

/************************************************************
* �������ƣ� GetVnic
* ���������� ���vnic�����Ϣ
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
int32 CVNetVnicDB::GetVnic(CVNetVnicMem &cNicMem)
{
    CDbVsiInfo cDbVsiInfo;
    cDbVsiInfo.SetVmId(cNicMem.GetVmId());
    cDbVsiInfo.SetNicIndex(cNicMem.GetNicIndex());
    
    int iRet = m_pOper->QueryByVmVnic(cDbVsiInfo);
    if (VNET_DB_IS_ITEM_NO_EXIST(iRet))
    {
        return 0;
    }
    else if (iRet != 0)
    {
        cout << "QueryByVmVnic failed." << iRet << endl;
        return iRet;
    }
   
    cNicMem.SetNicIndex(cDbVsiInfo.GetNicIndex());
    cNicMem.SetIsSriov(cDbVsiInfo.GetIsSriov());
    cNicMem.SetIsLoop(cDbVsiInfo.GetIsLoop());
    cNicMem.SetLogicNetworkUuid(cDbVsiInfo.GetLogicNetworkUuid());
    cNicMem.SetPortType(cDbVsiInfo.GetPortType());   
    cNicMem.SetVsiIdValue(cDbVsiInfo.GetVsiIdValue());
    cNicMem.SetVsiIdFormat(cDbVsiInfo.GetVsiIdFormat());
    cNicMem.SetVmPgUuid(cDbVsiInfo.GetVmPgUuid());
    cNicMem.SetMac(cDbVsiInfo.GetMac());
    if (cDbVsiInfo.GetIsHasIp())
    {
        cNicMem.SetIp(cDbVsiInfo.GetIp());
        cNicMem.SetMask(cDbVsiInfo.GetMask());
    }

    if (cDbVsiInfo.GetIsHasVSwitch())
    {
        cNicMem.SetVSwitchUuid(cDbVsiInfo.GetVSwitchUuid());
    }
    
    if (cDbVsiInfo.GetIsHasSriovVf())
    {
        cNicMem.SetSriovVfPortUuid(cDbVsiInfo.GetSriovVfPortUuid());
    }
    
    return 0;
};


/************************************************************
* �������ƣ� GetVmVnic
* ���������� ���VM��vnic�����Ϣ��Ҫ�����������vecNicMem����������Ϣ��ֻ��ѯ����������еĲ���
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
int32 CVNetVnicDB::GetVmVnic(int64 nVmID, vector<CVNetVnicMem> &vecNicMem)
{
    int32 iRet = 0;

    vector<CVNetVnicMem>::iterator itrVnicMem = vecNicMem.begin();
    for ( ; itrVnicMem != vecNicMem.end(); ++itrVnicMem)
    {
        CDbVsiInfo cDbVsiInfo;
        cDbVsiInfo.SetVmId(nVmID);
        cDbVsiInfo.SetNicIndex(itrVnicMem->GetNicIndex());
        
        iRet = m_pOper->QueryByVmVnic(cDbVsiInfo);
        if (iRet != 0)
        {
            cout << "QueryByVmVnic failed." << iRet << endl;
            return iRet;
        }
		
        itrVnicMem->SetVmId(nVmID);
        itrVnicMem->SetNicIndex(cDbVsiInfo.GetNicIndex());
        itrVnicMem->SetIsSriov(cDbVsiInfo.GetIsSriov());  
        itrVnicMem->SetIsLoop(cDbVsiInfo.GetIsLoop());  
        itrVnicMem->SetLogicNetworkUuid(cDbVsiInfo.GetLogicNetworkUuid()); 
        itrVnicMem->SetPortType(cDbVsiInfo.GetPortType()); 
        itrVnicMem->SetVsiIdValue(cDbVsiInfo.GetVsiIdValue()); 
        itrVnicMem->SetVsiIdFormat(cDbVsiInfo.GetVsiIdFormat()); 
        itrVnicMem->SetVmPgUuid(cDbVsiInfo.GetVmPgUuid()); 
        itrVnicMem->SetAssMac(cDbVsiInfo.GetAssMac());
        itrVnicMem->SetMac(cDbVsiInfo.GetMac());
        itrVnicMem->SetAssIP(cDbVsiInfo.GetAssIP());
        itrVnicMem->SetAssMask(cDbVsiInfo.GetAssMask());
        itrVnicMem->SetQuantumNetworkUuid(cDbVsiInfo.GetQuantumNetworkUuid());
        itrVnicMem->SetQuantumPortUuid(cDbVsiInfo.GetQuantumPortUuid());
        if (cDbVsiInfo.GetIsHasIp())
        {
            itrVnicMem->SetIp(cDbVsiInfo.GetIp()); 
            itrVnicMem->SetMask(cDbVsiInfo.GetMask()); 
        }
        if (cDbVsiInfo.GetIsHasVSwitch())
        {
            itrVnicMem->SetVSwitchUuid(cDbVsiInfo.GetVSwitchUuid());   
        }
        if (cDbVsiInfo.GetIsHasSriovVf())
        {
            itrVnicMem->SetSriovVfPortUuid(cDbVsiInfo.GetSriovVfPortUuid());    
        }
		
    }
                
    return 0;
}

/************************************************************
* �������ƣ� GetVmVnic
* ���������� ���VM��vnic�����Ϣ��Ҫ�����������vecNicDetail����������Ϣ��ֻ��ѯ����������еĲ���
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
int32 CVNetVnicDB::GetVmVnic(int64 nVmID, vector<CVNetVnicDetail> &vecNicDetail)
{
    int32 iRet = 0;

    vector<CVNetVnicDetail>::iterator itrVnic = vecNicDetail.begin();
    for ( ; itrVnic != vecNicDetail.end(); ++itrVnic)
    {
        CDbVsiInfo cDbVsiInfo;
        cDbVsiInfo.SetVmId(nVmID);
        cDbVsiInfo.SetNicIndex(itrVnic->GetNicIndex());
        
        iRet = m_pOper->QueryByVmVnic(cDbVsiInfo);
        if (iRet != 0)
        {
            cout << "QueryByVmVnic failed." << iRet << endl;
            return iRet;
        }

        itrVnic->SetNicIndex(cDbVsiInfo.GetNicIndex());
        itrVnic->SetIsSriov(cDbVsiInfo.GetIsSriov());  
        itrVnic->SetIsLoop(cDbVsiInfo.GetIsLoop());  
        itrVnic->SetLogicNetworkUuid(cDbVsiInfo.GetLogicNetworkUuid()); 
        itrVnic->SetPortType(cDbVsiInfo.GetPortType()); 
        itrVnic->SetVSIIDValue(cDbVsiInfo.GetVsiIdValue()); 
        itrVnic->SetVSIIDFormat(cDbVsiInfo.GetVsiIdFormat()); 
        itrVnic->SetPGUuid(cDbVsiInfo.GetVmPgUuid());
        itrVnic->SetAssMac(cDbVsiInfo.GetAssMac());
        itrVnic->SetMacAddress(cDbVsiInfo.GetMac());
        itrVnic->SetAssIp(cDbVsiInfo.GetAssIP());
        itrVnic->SetAssMask(cDbVsiInfo.GetAssMask());
        itrVnic->SetAssGateway(cDbVsiInfo.GetAssGateway());
        itrVnic->SetQuantumNetworkUuid(cDbVsiInfo.GetQuantumNetworkUuid());
        itrVnic->SetQuantumPortUuid(cDbVsiInfo.GetQuantumPortUuid());
        if (cDbVsiInfo.GetIsHasIp())
        {
            itrVnic->SetIPAddress(cDbVsiInfo.GetIp()); 
            itrVnic->SetNetmask(cDbVsiInfo.GetMask()); 
        }
        else
        {
            itrVnic->SetIPAddress(""); 
            itrVnic->SetNetmask(""); 
        }
        
        if (cDbVsiInfo.GetIsHasVSwitch())
        {
            itrVnic->SetSwitchUuid(cDbVsiInfo.GetVSwitchUuid());   
        }
        else
        {
            itrVnic->SetSwitchUuid("");   
        }
        
        if (cDbVsiInfo.GetIsHasSriovVf())
        {
            itrVnic->SetVportUuid(cDbVsiInfo.GetSriovVfPortUuid());    
        }
        else
        {
            itrVnic->SetVportUuid("");    
        }
    }
                
    return 0;
}

/************************************************************
* �������ƣ� GetVmVnic
* ���������� ���VM��vnic�����Ϣ��vecNicDetailֻΪ�������������nVmID��ѯ
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
int32 CVNetVnicDB::GetVmVnicFromDb(int64 nVmID, vector<CVNetVnicDetail> &vecNicDetail)
{
    int32 iRet = 0;

    vector<CDbVsiInfoSummary>  vecVsiInfoSum;
    iRet = m_pOper->QuerySummaryByVm(nVmID, vecVsiInfoSum);
    if (iRet != 0)
    {
        cout << "QuerySummaryByVm failed." << iRet << endl;
        return iRet;
    }

    vector<CDbVsiInfoSummary>::iterator itrVnic = vecVsiInfoSum.begin();
    for ( ; itrVnic != vecVsiInfoSum.end(); ++itrVnic)
    {
        CDbVsiInfo cDbVsiInfo;
        cDbVsiInfo.SetVmId(itrVnic->GetVmId());
        cDbVsiInfo.SetNicIndex(itrVnic->GetNicIndex());
        
        int iRet = m_pOper->QueryByVmVnic(cDbVsiInfo);
        if (iRet != 0)
        {
            cout << "QueryByVmVnic failed." << iRet << endl;
            return iRet;
        }

        CVNetVnicDetail cVnicDetail;  

        cVnicDetail.SetNicIndex(cDbVsiInfo.GetNicIndex());
        cVnicDetail.SetIsSriov(cDbVsiInfo.GetIsSriov());  
        cVnicDetail.SetIsLoop(cDbVsiInfo.GetIsLoop());  
        cVnicDetail.SetLogicNetworkUuid(cDbVsiInfo.GetLogicNetworkUuid()); 
        cVnicDetail.SetPortType(cDbVsiInfo.GetPortType()); 
        cVnicDetail.SetVSIIDValue(cDbVsiInfo.GetVsiIdValue()); 
        cVnicDetail.SetVSIIDFormat(cDbVsiInfo.GetVsiIdFormat()); 
        cVnicDetail.SetPGUuid(cDbVsiInfo.GetVmPgUuid()); 
        cVnicDetail.SetMacAddress(cDbVsiInfo.GetMac()); 
        if (cDbVsiInfo.GetIsHasIp())
        {
            cVnicDetail.SetIPAddress(cDbVsiInfo.GetIp()); 
            cVnicDetail.SetNetmask(cDbVsiInfo.GetMask()); 
        }
        if (cDbVsiInfo.GetIsHasVSwitch())
        {
            cVnicDetail.SetSwitchUuid(cDbVsiInfo.GetVSwitchUuid());   
        }
        if (cDbVsiInfo.GetIsHasSriovVf())
        {
            cVnicDetail.SetVportUuid(cDbVsiInfo.GetSriovVfPortUuid());    
        }
		
	    vecNicDetail.push_back(cVnicDetail);
    }
                
    return 0;
}
/************************************************************
* �������ƣ� InsertVnic
* ���������� ����vnic�����Ϣ
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
int32 CVNetVnicDB::InsertVnic(CVNetVnicMem &cNicMem)
{
    CDbVsiInfo cDbVsiInfo;
    cDbVsiInfo.SetVmId(cNicMem.GetVmId());
    cDbVsiInfo.SetNicIndex(cNicMem.GetNicIndex());    
    cDbVsiInfo.SetIsSriov(cNicMem.GetIsSriov());
    cDbVsiInfo.SetIsLoop(cNicMem.GetIsLoop());
    cDbVsiInfo.SetLogicNetworkUuid(cNicMem.GetLogicNetworkUuid().c_str());
    cDbVsiInfo.SetAssIP(cNicMem.GetAssIP().c_str());
    cDbVsiInfo.SetAssMask(cNicMem.GetAssMask().c_str());
    cDbVsiInfo.SetAssGateway(cNicMem.GetAssGateway().c_str());
    cDbVsiInfo.SetAssMac(cNicMem.GetAssMac().c_str());
    cDbVsiInfo.SetPortType(cNicMem.GetPortType().c_str());    
    cDbVsiInfo.SetVsiIdValue(cNicMem.GetVsiIdValue().c_str());
    cDbVsiInfo.SetVsiIdFormat(cNicMem.GetVsiIdFormat());
    cDbVsiInfo.SetVmPgUuid(cNicMem.GetVmPgUuid().c_str());
    cDbVsiInfo.SetMac(cNicMem.GetMac().c_str());
    cDbVsiInfo.SetQuantumNetworkUuid(cNicMem.GetQuantumNetworkUuid().c_str());
    cDbVsiInfo.SetQuantumPortUuid(cNicMem.GetQuantumPortUuid().c_str());

    if ("" != cNicMem.GetIp())
    {
        cDbVsiInfo.SetIsHasIp(TRUE);
        cDbVsiInfo.SetIp(cNicMem.GetIp().c_str());
        cDbVsiInfo.SetMask(cNicMem.GetMask().c_str());
    }
    else
    {
        cDbVsiInfo.SetIsHasIp(FALSE);
    }

    if ("" != cNicMem.GetVSwitchUuid())
    {
        cDbVsiInfo.SetIsHasVSwitch(TRUE);
        cDbVsiInfo.SetVSwitchUuid(cNicMem.GetVSwitchUuid().c_str());
    }
    else
    {
        cDbVsiInfo.SetIsHasVSwitch(FALSE);
    }

    if ("" != cNicMem.GetSriovVfPortUuid())
    {
        cDbVsiInfo.SetIsHasSriovVf(TRUE);
        cDbVsiInfo.SetSriovVfPortUuid(cNicMem.GetSriovVfPortUuid().c_str());
    }
    else
    {
        cDbVsiInfo.SetIsHasSriovVf(FALSE);
    }
    
    cDbVsiInfo.DbgShow();
    
    int iRet = m_pOper->Add(cDbVsiInfo);
    if (iRet != 0)
    {
        cout << "Add failed." << iRet << endl;
        return iRet;
    }

    cNicMem.SetVsiIdValue(cDbVsiInfo.GetVsiIdValue());
    
    return 0;
};

/************************************************************
* �������ƣ� InsertVmVnic
* ���������� ����VM��vnic�����Ϣ
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
int32 CVNetVnicDB::InsertVmVnic( vector<CVNetVnicMem> &vecNicMem)
{
    int32 iRet = 0;
    CDbVsiInfo cDbVsiInfo;
    
    vector<CVNetVnicMem>::iterator itrVnic = vecNicMem.begin();
    for ( ; itrVnic != vecNicMem.end(); ++itrVnic)
    {
        cDbVsiInfo.SetVmId(itrVnic->GetVmId());
        cDbVsiInfo.SetNicIndex(itrVnic->GetNicIndex());
        
        iRet = m_pOper->QueryByVmVnic(cDbVsiInfo);
        if ((iRet != 0) && (!VNET_DB_IS_ITEM_NO_EXIST(iRet)))
        {
            cout << "QueryByVmVnic failed." << iRet << endl;
            return iRet;
        }
    
        if (VNET_DB_IS_ITEM_NO_EXIST(iRet))
        {
            iRet = InsertVnic(*itrVnic);            
        }
        else
        {
            iRet = UpdateVnic(*itrVnic);
        }
        
        if (0 != iRet)
        {
            return iRet;
        }
    }
    
    return 0;
};

/************************************************************
* �������ƣ� UpdateVnic
* ���������� ����vnic�����Ϣ
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
int32 CVNetVnicDB::UpdateVnic(const CVNetVnicMem &cNicMem)
{
    CDbVsiInfo cDbVsiInfo;
    cDbVsiInfo.SetVmId(cNicMem.GetVmId());
    cDbVsiInfo.SetNicIndex(cNicMem.GetNicIndex());
    cDbVsiInfo.SetIsSriov(cNicMem.GetIsSriov());
    cDbVsiInfo.SetIsLoop(cNicMem.GetIsLoop());
    cDbVsiInfo.SetLogicNetworkUuid(cNicMem.GetLogicNetworkUuid().c_str());
    cDbVsiInfo.SetAssIP(cNicMem.GetAssIP().c_str());
    cDbVsiInfo.SetAssMask(cNicMem.GetAssMask().c_str());
    cDbVsiInfo.SetAssGateway(cNicMem.GetAssGateway().c_str());
    cDbVsiInfo.SetAssMac(cNicMem.GetAssMac().c_str());
    cDbVsiInfo.SetPortType(cNicMem.GetPortType().c_str());
    cDbVsiInfo.SetVsiIdValue(cNicMem.GetVsiIdValue().c_str());
    cDbVsiInfo.SetVsiIdFormat(cNicMem.GetVsiIdFormat());
    cDbVsiInfo.SetVmPgUuid(cNicMem.GetVmPgUuid().c_str());
    cDbVsiInfo.SetMac(cNicMem.GetMac().c_str());
    cDbVsiInfo.SetQuantumNetworkUuid(cNicMem.GetQuantumNetworkUuid().c_str());
    cDbVsiInfo.SetQuantumPortUuid(cNicMem.GetQuantumPortUuid().c_str());

    if ("" != cNicMem.GetIp())
    {
        cDbVsiInfo.SetIsHasIp(TRUE);
        cDbVsiInfo.SetIp(cNicMem.GetIp().c_str());
        cDbVsiInfo.SetMask(cNicMem.GetMask().c_str());
    }
    else
    {
        cDbVsiInfo.SetIsHasIp(FALSE);
    }

    if ("" != cNicMem.GetVSwitchUuid())
    {
        cDbVsiInfo.SetIsHasVSwitch(TRUE);
        cDbVsiInfo.SetVSwitchUuid(cNicMem.GetVSwitchUuid().c_str());
    }
    else
    {
        cDbVsiInfo.SetIsHasVSwitch(FALSE);
    }

    if ("" != cNicMem.GetSriovVfPortUuid())
    {
        cDbVsiInfo.SetIsHasSriovVf(TRUE);
        cDbVsiInfo.SetSriovVfPortUuid(cNicMem.GetSriovVfPortUuid().c_str());
    }
    else
    {
        cDbVsiInfo.SetIsHasSriovVf(FALSE);
    }
    
    cDbVsiInfo.DbgShow();
    int iRet = m_pOper->Modify(cDbVsiInfo);
    if (iRet != 0)
    {
        cout << "Modify failed." << iRet << endl;
        return iRet;
    }

    return 0;
};

/************************************************************
* �������ƣ� UpdateVmVnic
* ���������� ����vm��vnic�����Ϣ
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
int32 CVNetVnicDB::UpdateVmVnic( vector<CVNetVnicMem> &vecNicMem)
{
    int32 iRet = 0;

    vector<CVNetVnicMem>::iterator itrVnic = vecNicMem.begin();
    for ( ; itrVnic != vecNicMem.end(); ++itrVnic)
    {
        iRet = UpdateVnic(*itrVnic);
        if (0 != iRet)
        {
            return iRet;
        }
    }
    
    return 0;
};

/************************************************************
* �������ƣ� DeleteVnic
* ���������� ɾ��vnic�����Ϣ
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
int32 CVNetVnicDB::DeleteVnic(const string &strVsiUuid)
{
    int iRet = m_pOper->Del(strVsiUuid.c_str());
    if ((iRet != 0) && (!VNET_DB_IS_ITEM_NO_EXIST(iRet)))
    {
        cout << "DelByVmId failed." << iRet << endl;
        return iRet;
    }
    
    return 0;
};

/************************************************************
* �������ƣ� DeleteVmVnic
* ���������� ɾ��VM��vnic�����Ϣ
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
int32 CVNetVnicDB::DeleteVmVnic(int64 vm_id)
{
    int32 iRet = 0;
    
    iRet = m_pOper->DelByVmId(vm_id);
    if ((iRet != 0) && (!VNET_DB_IS_ITEM_NO_EXIST(iRet)))
    {
        cout << "DelByVmId failed." << iRet << endl;
        return iRet;
    }

    return 0;
};


/************************************************************
* �������ƣ� QueryAllVnic
* ���������� ��ѯ����VM��vnic�����Ϣ
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
int32 CVNetVnicDB::QueryAllVnic( vector<CDbVsiInfoSummary> & vecAllVnic)
{    
    int iRet = m_pOper->QuerySummary(vecAllVnic);
    if (iRet != 0)
    {
        cout << "QuerySummary failed." << iRet << endl;
        return iRet;
    }    
    
    return 0;
};

void VNM_DBG_QUERY_ALL_VNIC()
{
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

    return;
}
DEFINE_DEBUG_FUNC(VNM_DBG_QUERY_ALL_VNIC);


}

