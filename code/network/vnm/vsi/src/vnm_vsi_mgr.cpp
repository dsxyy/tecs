/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vnm_vsi_mgr.cpp
* 文件标识：
* 内容摘要：CVSIProfileMgr类实现文件
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
#include "vnet_db_mgr.h"
#include "vnet_error.h"
#include "vnet_vnic.h"
#include "ipv4_addr.h"
#include "ipv4_addr_range.h"
#include "ipv4_pool_mgr.h"
#include "logic_network.h"
#include "logic_network_mgr.h"
#include "vlan_range.h"
#include "vlan_pool_mgr.h"
#include "mac_addr.h"
#include "mac_range.h"
#include "mac_pool_mgr.h"
#include "vnet_msg.h"
#include "vnet_db_vm.h"
#include "vnet_db_vsi.h"
#include "vnm_vm_db.h"
#include "vnm_vnic_db.h"
#include "vna_reg_mod.h"
#include "vna_info.h"
#include "vnet_portmgr.h"
#include "vna_mgr.h"
#include "port_group_mgr.h"
#include "vnm_switch_manager.h"
#include "vnm_vsi_mgr.h"

namespace zte_tecs
{
CVSIProfileMgr* CVSIProfileMgr::_instance = NULL;

CVSIProfileMgr::CVSIProfileMgr()
{
    m_pMACPoolMgr = NULL;
    m_pVlanPoolMgr = NULL;
    m_pSegmentPoolMgr = NULL;
    m_pIPv4PoolMgr = NULL;
    m_pPortGroupMgr = NULL;
    m_pSwitchMgr= NULL;
};

CVSIProfileMgr::~CVSIProfileMgr()
{
    m_pMACPoolMgr = NULL;
    m_pVlanPoolMgr = NULL;
    m_pSegmentPoolMgr = NULL;    
    m_pIPv4PoolMgr = NULL;
    m_pPortGroupMgr = NULL;
    m_pSwitchMgr= NULL;
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
int32 CVSIProfileMgr::Init()
{
    m_pMACPoolMgr =  CMACPoolMgr::GetInstance();
    if (NULL == m_pMACPoolMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVSIProfileMgr::Init: call CMACPoolMgr::GetInstance() failed.\n");
        return -1;
    }

    m_pVlanPoolMgr =  CVlanPoolMgr::GetInstance();
    if (NULL == m_pVlanPoolMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVSIProfileMgr::Init: call CVlanPoolMgr::GetInstance() failed.\n");
        return -1;
    }

    m_pSegmentPoolMgr =  CSegmentPoolMgr::GetInstance();
    if (NULL == m_pSegmentPoolMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVSIProfileMgr::Init: call CSegmentPoolMgr::GetInstance() failed.\n");
        return -1;
    }

    m_pIPv4PoolMgr =  CIPv4PoolMgr::GetInstance();
    if (NULL == m_pIPv4PoolMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVSIProfileMgr::Init: call CIPv4PoolMgr::GetInstance() failed.\n");
        return -1;
    }

    m_pPortGroupMgr =  CPortGroupMgr::GetInstance();
    if (NULL == m_pPortGroupMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVSIProfileMgr::Init: call CPortGroupMgr::GetInstance() failed.\n");
        return -1;
    }
    
    m_pSwitchMgr =  CSwitchMgr::GetInstance();
    if (NULL == m_pSwitchMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVSIProfileMgr::Init: call CSwitchMgr::GetInstance() failed.\n");
        return -1;
    }

    return 0;
}

/************************************************************
* 函数名称： GetVSIResForVnic
* 功能描述： 获得VM相关的VSI资源信息
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
int CVSIProfileMgr::GetVSIResForVnic(vector<CVNetVnicMem> &vecNicMem)
{
    int32 iRet = 0;

    //iRet = m_pVNetVSIDB->GetVMVSI(vecNicMem);
    if (0 != iRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVSIProfileMgr::GetVSIResForVnic: call GetVMVSI failed, ret=%d.\n", iRet);
        return iRet;
    }

    return 0;
}

/************************************************************
* 函数名称： AllocVSIResForVnic
* 功能描述： 为VNIC分配VSI相关的网络资源，包括vsi、vlan、mac、IP等
* 访问的表： 无
* 修改的表： 无
* 输入参数： vm_id
                                project_id
                                m_strLogicNetworkID
* 输出参数：vlan_map_id
                                mac_id
                                logic_network_ip_id
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2012/5         V1.0       lvech         创建
***************************************************************/
int CVSIProfileMgr::AllocVSIResForVnic(CVNetVmMem &cVnetVmMem)
{
    int32 iRet = 0;

    vector<CVNetVnicMem> vecVNetVnic;
    cVnetVmMem.GetVecVnicMem(vecVNetVnic);
    if (0 == vecVNetVnic.size())
    {
        return 0;
    }

    //先分配VLAN资源, 填写到CVNetVSIProfile中
    iRet = m_pVlanPoolMgr->AllocVlanForVnic(cVnetVmMem);
    if (0 != iRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVSIProfileMgr::AllocVSIResForVnic: call AllocVlanForVnic failed, ret=%d.\n", iRet);
        iRet =  ERR_VNMVNIC_ALLOC_VLAN_FAIL;
        goto FREE_PROC;
    }

    //分配SEGMENT资源, 填写到CVNetVSIProfile中
    iRet = m_pSegmentPoolMgr->AllocSegmentForVnic(cVnetVmMem);
    if (0 != iRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVSIProfileMgr::AllocVSIResForVnic: call AllocSegmentForVnic failed, ret=%d.\n", iRet);
        iRet =  ERR_VNMVNIC_ALLOC_SEGMENT_FAIL;
        goto FREE_PROC;
    }    
    
    //分配MAC资源，MAC_ID填写到CVNetVSIProfile中
    iRet = m_pMACPoolMgr->AllocMACForVnic(cVnetVmMem);
    if (0 != iRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVSIProfileMgr::AllocVSIResForVnic: call AllocMACForVnic failed, ret=%d.\n", iRet);
        iRet =  ERR_VNMVNIC_ALLOC_MAC_FAIL;
        goto FREE_PROC;
    }
    
    //分配IP资源，logic_network_ip_id填写到CVNetVSIProfile中
    iRet = m_pIPv4PoolMgr->AllocIPAddrForVnic(cVnetVmMem);
    if (0 != iRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVSIProfileMgr::AllocVSIResForVnic: call AllocIPAddrForVnic failed, ret=%d.\n", iRet);
        iRet =  ERR_VNMVNIC_ALLOC_IP_FAIL;
        goto FREE_PROC;
    }

FREE_PROC:
    if (0 != iRet)
    {
        if(0 != FreeVSIResForVnic(cVnetVmMem))
        {
            VNET_LOG(VNET_LOG_ERROR, "CVSIProfileMgr::AllocVSIResForVnic: call FreeVSIResForVnic failed.\n");
        }
    }
    
    return iRet;
};

/************************************************************
* 函数名称： FreeVSIResForVnic
* 功能描述： 释放VNIC相关VSI资源，包括vsi、vlan、mac、IP等资源
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
int CVSIProfileMgr::FreeVSIResForVnic(CVNetVmMem &cVnetVmMem)
{
    int32 iRet = 0;

    vector<CVNetVnicMem> vecVNetVnic;
    cVnetVmMem.GetVecVnicMem(vecVNetVnic);
    if (0 == vecVNetVnic.size())
    {
        return 0;
    }

    //释放VLAN资源, 填写到CVNetVSIProfile中
    iRet = m_pVlanPoolMgr->FreeVlanForVnic(cVnetVmMem);
    if (0 != iRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVSIProfileMgr::FreeVSIResForVnic: call FreeVlanForVnic failed, ret=%d.\n", iRet);
        return ERR_VNMVNIC_FREE_VLAN_FAIL;
    }

    //释放SEGMENT资源, 填写到CVNetVSIProfile中
    iRet = m_pSegmentPoolMgr->FreeSegmentForVnic(cVnetVmMem);
    if (0 != iRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVSIProfileMgr::FreeVSIResForVnic: call FreeSegmentForVnic failed, ret=%d.\n", iRet);
        return ERR_VNMVNIC_FREE_SEGMENT_FAIL;
    }

    //释放MAC资源，MAC_ID填写到CVNetVSIProfile中
    iRet = m_pMACPoolMgr->FreeMACForVnic(cVnetVmMem);
    if (0 != iRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVSIProfileMgr::FreeVSIResForVnic: call FreeMacForVnic failed, ret=%d.\n", iRet);
        return ERR_VNMVNIC_FREE_MAC_FAIL;
    }
    
    //释放IP资源，logic_network_ip_id填写到CVNetVSIProfile中
    iRet = m_pIPv4PoolMgr->FreeIPAddrForVnic(cVnetVmMem);
    if (0 != iRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVSIProfileMgr::FreeVSIResForVnic: call FreeIPForVnic failed, ret=%d.\n", iRet);
        return ERR_VNMVNIC_FREE_IP_FAIL;
    }

    return 0;
};
       
/************************************************************
* 函数名称： GetVSIDetailInfo
* 功能描述： 获取一个VM的VSI详细信息
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
int32 CVSIProfileMgr::GetVSIDetailInfo(int64 nVmID, CVMVSIInfo &cVMVSIInfo)
{
    int32 iRet = 0;
    string strVnaUuid;


    CVNAManager *pVNAMgr = CVNAManager::GetInstance();
    if( NULL == pVNAMgr)
    {
        cout << "CVNAManager::GetInstance() is NULL" << endl;
        return -1;
    }

    CVNetVmDB *pVNetVmDB = CVNetVmDB::GetInstance();
    if (NULL == pVNetVmDB)
    {
        cout << "pVNetVmDB::GetInstance() is NULL" << endl;
        return ERR_VNMVNIC_GET_VM_DB_HANDLE_FAIL;
    }
    
    cVMVSIInfo.m_nVMID = nVmID;
    iRet = pVNetVmDB->GetVm(cVMVSIInfo);
    if (0 != iRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVSIProfileMgr::GetNetResourceOnCC: call GetVmVnic failed, ret=%d.\n", iRet);
        return ERR_VNMVNIC_GET_VNIC_FROM_DB_FAIL;
    }

    //获取VNA uuid
    iRet = pVNAMgr->QueryVNAUUID(cVMVSIInfo.m_strClusterApp, cVMVSIInfo.m_strHostApp, strVnaUuid);
    if (0 != iRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetVnicDB::GetVmVnic: call QueryVNAUUID(%s,%s) failed, ret=%d.\n", 
            cVMVSIInfo.m_strClusterApp.c_str(),
            cVMVSIInfo.m_strHostApp.c_str(), 
            iRet);
        return iRet;
    }

    iRet = GetVSIDetailInfo(strVnaUuid, nVmID, cVMVSIInfo);
    if (0 != iRet)
    {
        return iRet;
    }

    return iRet;
}

/************************************************************
* 函数名称： GetVSIDetailInfo
* 功能描述： 获取一个VM的VSI详细信息
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
int32 CVSIProfileMgr::GetVSIDetailInfo(string strVNAUUID,int64 nVmID, CVMVSIInfo &cVMVSIInfo)
{
    int32 iRet = 0;

    if (0 == cVMVSIInfo.m_vecVnicDetail.size())
    {
        return 0;
    }

    CVNetVmDB *pVNetVmDB = CVNetVmDB::GetInstance();
    if (NULL == pVNetVmDB)
    {
        VNET_LOG(VNET_LOG_ERROR, "pVNetVmDB::GetInstance() is NULL\n");
        return ERR_VNMVNIC_GET_VM_DB_HANDLE_FAIL;
    }
    
    cVMVSIInfo.m_nVMID = nVmID;
    iRet = pVNetVmDB->GetVm(cVMVSIInfo);
    if (0 != iRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVSIProfileMgr::GetNetResourceOnCC: call GetVmVnic failed, ret=%d.\n", iRet);
        return ERR_VNMVNIC_GET_VNIC_FROM_DB_FAIL;
    }

    //获取PG详细信息
    iRet = m_pPortGroupMgr->GetVMPGDetail(cVMVSIInfo.m_vecVnicDetail);
    if (0 != iRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVSIProfileMgr::GetVSIDetailInfo: call GetVMPGDetail failed, ret=%d.\n", iRet);
        return ERR_VNMVNIC_GET_PG_DETAIL_FAIL;
    }

    //获取VLAN详细信息
    iRet = m_pVlanPoolMgr->GetVlanDetail(cVMVSIInfo.m_nProjectID, cVMVSIInfo.m_vecVnicDetail);
    if (0 != iRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVSIProfileMgr::GetVSIDetailInfo: call GetVlanDetail failed, ret=%d.\n", iRet);
        return ERR_VNMVNIC_GET_VLAN_DETAIL_FAIL;
    }

    //获取SEGMENT详细信息
    iRet = m_pSegmentPoolMgr->GetSegmentDetail(cVMVSIInfo.m_nProjectID, cVMVSIInfo.m_vecVnicDetail);
    if (0 != iRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVSIProfileMgr::GetVSIDetailInfo: call GetSegmentDetail failed, ret=%d.\n", iRet);
        return ERR_VNMVNIC_GET_SEGMENT_DETAIL_FAIL;
    }    

    //获取DVS详细信息
    iRet = m_pSwitchMgr->GetDVSDetail(strVNAUUID, cVMVSIInfo.m_vecVnicDetail);
    if (0 != iRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVSIProfileMgr::GetVSIDetailInfo: call GetDVSDetail failed, ret=%d.\n", iRet);
        return ERR_VNMVNIC_GET_DVS_DETAIL_FAIL;
    }

    return 0;
};

/************************************************************
* 函数名称： SetVSIDetailFromVnicMem
* 功能描述： 获取一个VM的VSI详细信息
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
void CVSIProfileMgr::SetVSIDetailFromVnicMem(vector<CVNetVnicMem> &vecNicMem, vector<CVNetVnicDetail> &vecVnicDetail)
{
    vecVnicDetail.clear();
    
    vector<CVNetVnicMem>::iterator itrVnic = vecNicMem.begin();
    for ( ; itrVnic != vecNicMem.end(); ++itrVnic)
    {
        CVNetVnicDetail cVnicDetail;
        
        cVnicDetail.SetNicIndex(itrVnic->GetNicIndex());  
        cVnicDetail.SetIsSriov(itrVnic->GetIsSriov());  
        cVnicDetail.SetIsLoop(itrVnic->GetIsLoop());  
        cVnicDetail.SetLogicNetworkUuid(itrVnic->GetLogicNetworkUuid()); 
        cVnicDetail.SetPortType(itrVnic->GetPortType()); 
        cVnicDetail.SetVSIIDValue(itrVnic->GetVsiIdValue()); 
        cVnicDetail.SetVSIIDFormat(itrVnic->GetVsiIdFormat()); 
        cVnicDetail.SetPGUuid(itrVnic->GetVmPgUuid()); 
        cVnicDetail.SetMacAddress(itrVnic->GetMac()); 
        cVnicDetail.SetIPAddress(itrVnic->GetIp()); 
        cVnicDetail.SetNetmask(itrVnic->GetMask()); 
        cVnicDetail.SetSwitchUuid(itrVnic->GetVSwitchUuid());    
        cVnicDetail.SetVportUuid(itrVnic->GetSriovVfPortUuid());  

        vecVnicDetail.push_back(cVnicDetail);
    }
};

/**************************************************************
*调试函数定义
***************************************************************/

void VNM_DBG_QUERY_VSI_DETAIL(int64 nVmID)
{
    CVSIProfileMgr *pVsiMgr = CVSIProfileMgr::GetInstance();
    if (NULL == pVsiMgr)
    {
        cout << "pVsiMgr is NULL" <<endl;
        return;
    }

    CVMVSIInfo cVMVSIInfo;
    int32 iRet = pVsiMgr->GetVSIDetailInfo(nVmID, cVMVSIInfo);
    if (0!=iRet)
    {
        cout << "call GetVSIDetailInfo failed!" <<endl;
        return;
    }
    
    cVMVSIInfo.Print();

    return;
}
DEFINE_DEBUG_FUNC(VNM_DBG_QUERY_VSI_DETAIL);       
}
