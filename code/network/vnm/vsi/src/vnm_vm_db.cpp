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
#include "vnet_db_vm.h"
#include "vnet_db_vsi.h"
#include "vnm_vnic_db.h"
#include "vna_reg_mod.h"
#include "vnet_msg.h"
#include "vna_info.h"
#include "vnet_vnic.h"
#include "ipv4_addr.h"
#include "ipv4_addr_range.h"
#include "ipv4_addr_pool.h"
#include "ipv4_pool_mgr.h"
#include "logic_network.h"
#include "logic_network_mgr.h"
#include "vnet_portmgr.h"
#include "vna_mgr.h"
#include "vnm_vm_db.h"

namespace zte_tecs
{
CVNetVmDB* CVNetVmDB::_instance = NULL;

CVNetVmDB::CVNetVmDB()
{
    m_pVNetDbMgr = NULL;
};

CVNetVmDB::~CVNetVmDB()
{
    m_pVNetDbMgr = NULL;
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
int32 CVNetVmDB::Init()
{
    m_pVNetDbMgr = CVNetDbMgr::GetInstance();
    if( NULL == m_pVNetDbMgr)
    {
        cout << "CVNetDbMgr::GetInstance() is NULL" << endl;
        return -1;
    }

    m_pDbOper = GetDbIVm();
    if (NULL == m_pDbOper)
    {
        cout << "GetDbIVm() is NULL" << endl;
        return -1;
    }
    
    VNET_LOG(VNET_LOG_WARN, "CVNetVmMgr::Init: Call  successfully.\n");

    return 0;
} ;

/************************************************************
* �������ƣ� GetVm
* ���������� ���vm�����Ϣ
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
int32 CVNetVmDB::GetVm(CDbVm &cDbVm)
{    
    int iRet = m_pDbOper->Query(cDbVm);
    if (VNET_DB_IS_ITEM_NO_EXIST(iRet))
    {
        return 0;
    }
    else if (iRet != 0)
    {
        cout << "CDBOperateVm->Query failed." << iRet << endl;
        return iRet;
    }
    
    return 0;
};

/************************************************************
* �������ƣ� GetVm
* ���������� ���vm�����Ϣ,CVNetVmMem����Ҫ����������Ϣ���ǰ����������˳�������
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
int32 CVNetVmDB::GetVm(CVNetVmMem &cVmMem)
{
    CDbVm cDbVm;
    cDbVm.SetVmId(cVmMem.GetVmId());
    
    int iRet = m_pDbOper->Query(cDbVm);
    if (VNET_DB_IS_ITEM_NO_EXIST(iRet))
    {
        return iRet;
    }
    else if (iRet != 0)
    {
        cout << "CDBOperateVm->Query failed." << iRet << endl;
        return iRet;
    }

    cVmMem.SetProjectId(cDbVm.GetProjectId());
    
    if (cDbVm.GetIsEnableWdg())
    {
        cVmMem.SetIsHasWatchdog(TRUE);
        cVmMem.SetWatchdogTimeout(cDbVm.GetWdgTimeout());
    }
    else
    {
        cVmMem.SetIsHasWatchdog(FALSE);
    }
    
    if (cDbVm.GetIsHasHost())
    {
        cVmMem.SetHostName(cDbVm.GetHostName());
    }
       
    if (cDbVm.GetIsHasCluster())
    {
        cVmMem.SetClusterName(cDbVm.GetClusterName());
    }
    
    vector<CVNetVnicMem> vecVnicMem;
    cVmMem.GetVecVnicMem(vecVnicMem);

    CVNetVnicDB* pVNetVnicDB = CVNetVnicDB::GetInstance();
    if( NULL == pVNetVnicDB)
    {
        cout << "CVNetVnicDB::GetInstance() is NULL" << endl;
        return -1;
    }
    
    iRet = pVNetVnicDB->GetVmVnic(cVmMem.GetVmId(),vecVnicMem);
    if (VNET_DB_IS_ITEM_NO_EXIST(iRet))
    {
        return 0;
    }
    else if (iRet != 0)
    {
        cout << "CVNetVnicDB->GetVmVnic failed." << iRet << endl;
        return iRet;
    }
    
    cVmMem.SetVecVnicMem(vecVnicMem);
    
    return 0;
};

/************************************************************
* �������ƣ� GetVm
* ���������� ���vm�����Ϣ,cVMVSIInfo����Ҫ����������Ϣ���ǰ����������˳�������
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
int32 CVNetVmDB::GetVm(CVMVSIInfo &cVMVSIInfo)
{
    CDbVm CDbVm;
    CDbVm.SetVmId(cVMVSIInfo.m_nVMID);
    
    int iRet = m_pDbOper->Query(CDbVm);
    if (VNET_DB_IS_ITEM_NO_EXIST(iRet))
    {
        return 0;
    }
    else if (iRet != 0)
    {
        cout << "CDBOperateVm->Query failed." << iRet << endl;
        return iRet;
    }

    cVMVSIInfo.m_nProjectID = CDbVm.GetProjectId();         // m_nProjectID
    if (CDbVm.GetIsHasCluster())
    {
        cVMVSIInfo.m_strClusterApp = CDbVm.GetClusterName();
    }
    if (CDbVm.GetIsHasHost())
    {
        cVMVSIInfo.m_strHostApp = CDbVm.GetHostName();
    }
        
    cVMVSIInfo.m_cWatchDogInfo.m_nIsWDGEnable = CDbVm.GetIsEnableWdg();
    if (CDbVm.GetIsEnableWdg())
    {
        cVMVSIInfo.m_cWatchDogInfo.m_nTimeOut = CDbVm.GetWdgTimeout();
    }
    
    CVNetVnicDB* pVNetVnicDB = CVNetVnicDB::GetInstance();
    if( NULL == pVNetVnicDB)
    {
        cout << "CVNetVnicDB::GetInstance() is NULL" << endl;
        return -1;
    }

    iRet = pVNetVnicDB->GetVmVnic(cVMVSIInfo.m_nVMID, cVMVSIInfo.m_vecVnicDetail);
    if (iRet != 0)
    {
        cout << "CVNetVnicDB->GetVmVnic failed." << iRet << endl;
        return iRet;
    }
 
    return 0;
}

/************************************************************
* �������ƣ� InsertVm
* ���������� ����vm�����Ϣ
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
int32 CVNetVmDB::InsertVm(CDbVm &cDbVm)
{   
    int iRet = m_pDbOper->Add(cDbVm);
    if (iRet != 0)
    {
        cout << "Add failed." << iRet << endl;
        return iRet;
    }
    
    return 0;
};

/************************************************************
* �������ƣ� InsertVm
* ���������� ����vm�����Ϣ
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
int32 CVNetVmDB::InsertVm(CVNetVmMem &cVnetVmMem)
{
    CDbVm cDbVmInfo;
    cDbVmInfo.SetVmId(cVnetVmMem.GetVmId());
    cDbVmInfo.SetProjectId(cVnetVmMem.GetProjectId());
    if (cVnetVmMem.GetIsHasWatchdog())
    {
        cDbVmInfo.SetIsEnableWdg(TRUE);
        cDbVmInfo.SetWdgTimeout(cVnetVmMem.GetWatchdogTimeout());
    }
    else
    {
        cDbVmInfo.SetIsEnableWdg(FALSE);
    }

    if ("" != cVnetVmMem.GetClusterName())
    {
        cDbVmInfo.SetIsHasCluster(TRUE);
        cDbVmInfo.SetClusterName(cVnetVmMem.GetClusterName().c_str());
    }
    else
    {
        cDbVmInfo.SetIsHasCluster(FALSE);
    }

    if ("" != cVnetVmMem.GetHostName())
    {
        cDbVmInfo.SetIsHasHost(TRUE);
        cDbVmInfo.SetHostName(cVnetVmMem.GetHostName().c_str());
    }
    else
    {
        cDbVmInfo.SetIsHasHost(FALSE);
    }

    int iRet = m_pDbOper->Query(cDbVmInfo);
    if ((iRet != 0) &&(! VNET_DB_IS_ITEM_NO_EXIST(iRet)))
    {
        cout << "Query failed." << iRet << endl;
        return iRet;
    }

    if (VNET_DB_IS_ITEM_NO_EXIST(iRet))
    {
        iRet = m_pDbOper->Add(cDbVmInfo);
    }
    else
    {
        iRet = m_pDbOper->Modify(cDbVmInfo);
    }
    
    if (iRet != 0)
    {
        cout << "operate failed." << iRet << endl;
        return iRet;
    }

    vector<CVNetVnicMem> vecVnicMem;
    cVnetVmMem.GetVecVnicMem(vecVnicMem);

    if (0 == vecVnicMem.size())
    {
        return 0;
    }
                
    CVNetVnicDB* pVNetVnicDB = CVNetVnicDB::GetInstance();
    if( NULL == pVNetVnicDB)
    {
        cout << "CVNetVnicDB::GetInstance() is NULL" << endl;
        return -1;
    }

    iRet = pVNetVnicDB->InsertVmVnic(vecVnicMem);
    if (iRet != 0)
    {
        cout << "CVNetVnicDB->InsertVmVnic failed." << iRet << endl;
        return iRet;
    }

    cVnetVmMem.SetVecVnicMem(vecVnicMem);

    return 0;
}

/************************************************************
* �������ƣ� UpdateVm
* ���������� ����vm�����Ϣ
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
int32 CVNetVmDB::UpdateVm(CDbVm &cDbVm)
{    
    int iRet = m_pDbOper->Modify(cDbVm);
    if (iRet != 0)
    {
        cout << "Modify failed." << iRet << endl;
        return iRet;
    }

    return 0;
}

/************************************************************
* �������ƣ� UpdateVm
* ���������� ����vm�����Ϣ
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
int32 CVNetVmDB::UpdateVm(CVNetVmMem &cVnetVmMem)
{
    CDbVm cDbVm;
    cDbVm.SetVmId(cVnetVmMem.GetVmId());
 
    cDbVm.SetProjectId(cVnetVmMem.GetProjectId());
    if (cVnetVmMem.GetIsHasWatchdog())
    {
        cDbVm.SetIsEnableWdg(TRUE);
        cDbVm.SetWdgTimeout(cVnetVmMem.GetWatchdogTimeout());
    }
    else
    {
        cDbVm.SetIsEnableWdg(FALSE);
    }
    if ("" != cVnetVmMem.GetClusterName())
    {
        cDbVm.SetIsHasCluster(TRUE);
        cDbVm.SetClusterName(cVnetVmMem.GetClusterName().c_str());
    }
    else
    {
        cDbVm.SetIsHasCluster(FALSE);
    }

    if ("" != cVnetVmMem.GetHostName())
    {
        cDbVm.SetIsHasHost(TRUE);
        cDbVm.SetHostName(cVnetVmMem.GetHostName().c_str());
    }
    else
    {
        cDbVm.SetIsHasHost(FALSE);
    }
    
    cDbVm.DbgShow();
    int iRet = m_pDbOper->Modify(cDbVm);
    if (iRet != 0)
    {
        cout << "Modify failed." << iRet << endl;
        return iRet;
    }

    vector<CVNetVnicMem> vecVnicMem;
    cVnetVmMem.GetVecVnicMem(vecVnicMem);

    if (0 == vecVnicMem.size())
    {
        return 0;
    }

    CVNetVnicDB* pVNetVnicDB = CVNetVnicDB::GetInstance();
    if( NULL == pVNetVnicDB)
    {
        cout << "CVNetVnicDB::GetInstance() is NULL" << endl;
        return -1;
    }

    iRet = pVNetVnicDB->UpdateVmVnic(vecVnicMem);
    if (iRet != 0)
    {
        cout << "CVNetVnicDB->GetVmVnic failed." << iRet << endl;
        return iRet;
    }

    cVnetVmMem.SetVecVnicMem(vecVnicMem);

    return 0;
};



/************************************************************
* �������ƣ� DeleteVm
* ���������� ɾ��vm�����Ϣ
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
int32 CVNetVmDB::DeleteVm(const int64 &nVmID)
{        
    CVNetVnicDB* pVNetVnicDB = CVNetVnicDB::GetInstance();
    if( NULL == pVNetVnicDB)
    {
        cout << "CVNetVnicDB::GetInstance() is NULL" << endl;
        return -1;
    }
    
    int iRet = pVNetVnicDB->DeleteVmVnic(nVmID);
    if (iRet != 0)
    {
        cout << "CVNetVnicDB->UpdateVmVnic failed." << iRet << endl;
        return iRet;
    }
    
    iRet = m_pDbOper->Del(nVmID);
    if ((iRet != 0) && (!VNET_DB_IS_ITEM_NO_EXIST(iRet)))
    {
        cout << "DelByVmId failed." << iRet << endl;
        return iRet;
    }

    return 0;
};

/************************************************************
* �������ƣ� QueryAllVm
* ���������� ��ѯ����VM�����Ϣ
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
int32 CVNetVmDB::QueryAllVm( vector<CDbVm> & vecAllVm)
{    
    int iRet = m_pDbOper->QuerySummary(vecAllVm);
    if (iRet != 0)
    {
        cout << "QuerySummary failed." << iRet << endl;
        return iRet;
    }    
    
    return 0;
};

void VNM_DBG_QUERY_ALL_VM()
{
#if 0
    CVNetVmDB *pVnicDB = CVNetVmDB::GetInstance();

    vector<CDbVm> vecAllVm;
    int iRet = pVnicDB->QueryAllVm(vecAllVm);
    if (0 != iRet)
    {
        cout << "call QueryAllVnic failed, return=" << iRet <<endl;
    }

    CDBOperateVm * m_pDbOper = GetDbIVm();
    if (NULL == m_pDbOper)
    {
        cout << "GetDbIVm() is NULL" << endl;
        return;
    }
    
    vector<CDbVsiInfoSummary>::iterator itrVM = vecAllVnic.begin();
    for ( ; itrVM != vecAllVnic.end(); ++itrVM)
    {
        CDbVsiInfo CDbVm;
        CDbVm.SetVmId(itrVM->GetVmId());
        CDbVm.SetNicIndex(itrVM->GetNicIndex());
        
        int iRet = m_pDbOper->QueryByVmVnic(CDbVm);
        if (iRet != 0)
        {
            cout << "QueryByVmVnic failed." << iRet << endl;
            return;
        }
        else
        {
            CDbVm.DbgShow();
        }
    }
#endif
    return;
}
DEFINE_DEBUG_FUNC(VNM_DBG_QUERY_ALL_VM);


}

