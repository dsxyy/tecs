/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vnm_vnic_db.cpp
* 文件标识：
* 内容摘要：CVNetVnicDB类实现文件
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
* 函数名称： GetVm
* 功能描述： 获得vm相关信息
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
* 函数名称： GetVm
* 功能描述： 获得vm相关信息,CVNetVmMem中需要已有网卡信息且是按计算的网卡顺序排序的
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
* 函数名称： GetVm
* 功能描述： 获得vm相关信息,cVMVSIInfo中需要已有网卡信息且是按计算的网卡顺序排序的
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
* 函数名称： InsertVm
* 功能描述： 插入vm相关信息
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
* 函数名称： InsertVm
* 功能描述： 插入vm相关信息
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
* 函数名称： UpdateVm
* 功能描述： 更新vm相关信息
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
* 函数名称： UpdateVm
* 功能描述： 更新vm相关信息
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
* 函数名称： DeleteVm
* 功能描述： 删除vm相关信息
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
* 函数名称： QueryAllVm
* 功能描述： 查询所有VM相关信息
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

