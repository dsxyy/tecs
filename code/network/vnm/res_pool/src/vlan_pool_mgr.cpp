/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vlan_pool_mgr.cpp
* 文件标识：
* 内容摘要：CVlanPoolMgr类实现文件
* 当前版本：1.0
* 作    者：
* 完成日期：2013年1月18日
*
* 修改记录1：
*     修改日期：2013/1/18
*     版 本 号：V1.0
*     修 改 人：Wang.Lule
*     修改内容：创建
******************************************************************************/

#include "vnet_comm.h"
#include "vnet_mid.h"
#include "vnet_error.h"
#include "vnetlib_event.h"
#include "vnetlib_msg.h"

#include "vnet_db_proc.h"
#include "vnet_db_mgr.h"
#include "vnet_db_netplane_vlanrange.h"
#include "vnet_db_vlan_map.h"
#include "vnet_db_netplane_vlanpool.h"

#include "ipv4_addr.h"
#include "ipv4_addr_range.h"

#include "vnet_vnic.h"

#include "logic_network.h"
#include "logic_network_mgr.h"

#include "port_group_mgr.h"

#include "vlan_range.h"
#include "vlan_pool.h"
#include "vlan_pool_mgr.h"


namespace zte_tecs
{
    CVlanPoolMgr *CVlanPoolMgr::s_pInstance = NULL;
    
    CVlanPoolMgr::CVlanPoolMgr()
    {
        m_pMU = NULL;
    }
    
    CVlanPoolMgr::~CVlanPoolMgr()
    {
        m_pMU = NULL;
    }

    int32 CVlanPoolMgr::AllocVlanForVnic(CVNetVmMem & cVNetVmMem)
    {
        CPortGroupMgr *pPGMgr = CPortGroupMgr::GetInstance();
        if(NULL == pPGMgr)
        {        
            VNET_LOG(VNET_LOG_ERROR, "CVlanPoolMgr::AllocVlanForVnic: CPortGroupMgr instance is NULL.\n");
            return -1;
        }      
        
        // Need transaction;
        vector<CVNetVnicMem> vecVNetVnic;
        cVNetVmMem.GetVecVnicMem(vecVNetVnic);
        int64 nProjID = cVNetVmMem.GetProjectId();
        vector<CVNetVnicMem>::iterator itr = vecVNetVnic.begin();
        for( ; itr != vecVNetVnic.end(); ++itr)
        {
            string strNetplaneUUID;
            string strPGID = itr->GetVmPgUuid();         

            int32 nIsolationNo = 0;   
            uint32 dwVlanID;
            if(PORTGROUP_OPER_SUCCESS == pPGMgr->IsNeedVlanMap(strPGID, nIsolationNo)
                && FALSE == pPGMgr->IsPortGroupSdn(strPGID))
            {
                // vlan map;
                if(PORTGROUP_OPER_SUCCESS != pPGMgr->GetPGNetplane(strPGID, strNetplaneUUID))
                {        
                    VNET_LOG(VNET_LOG_ERROR, "CVlanPoolMgr::AllocVlanForVnic: Call GetPGNetplane(%s) failed.\n", 
                        strPGID.c_str());            
                    return -1;
                }                
                
                CVlanRange cVlanRange;
                if(0 != GetVlanRange(strNetplaneUUID, cVlanRange))
                {
                    VNET_LOG(VNET_LOG_ERROR, "CVlanPoolMgr::AllocVlanForVnic: Call GetVlanRange(%s) failed.\n", 
                        strNetplaneUUID.c_str());  
                    return -1;
                }
                CVlanPool cVlanPool;
                if(0 != cVlanPool.Init(&cVlanRange, strNetplaneUUID))
                {
                    VNET_LOG(VNET_LOG_ERROR, "CVlanPoolMgr::AllocVlanForVnic: Call Init(%s) vlan pool failed.\n", 
                        strNetplaneUUID.c_str()); 
                    return -1;
                }
                
                if(0 != cVlanPool.AllocVlanID(strNetplaneUUID, nProjID, strPGID, nIsolationNo, dwVlanID))
                {
                    return -1;
                }
                cVNetVmMem.SetVecVnicMem(vecVNetVnic);
                VNET_LOG(VNET_LOG_INFO, "CVlanPoolMgr::AllocVlanForVNic: (ProjID:0x%llx, PGID:%s, IsoNo:%d) is mapped vlan[%d].\n", 
                    nProjID, strPGID.c_str(), nIsolationNo, dwVlanID); 
            }                   
        }
      //  cVNetVmMem.SetVecVnicMem(vecVNetVnic);
        return 0;
    }
    
    int32 CVlanPoolMgr::FreeVlanForVnic(CVNetVmMem & cVNetVmMem)
    {
        CPortGroupMgr *pPGMgr = CPortGroupMgr::GetInstance();
        if(NULL == pPGMgr)
        {        
            VNET_LOG(VNET_LOG_ERROR, "CVlanPoolMgr::FreeVlanForVnic: CPortGroupMgr instance is NULL.\n");
            return -1;
        }      
        
        // Need transaction;
        int32 nRet = 0;
        vector<CVNetVnicMem> vecVNetVnic;
        cVNetVmMem.GetVecVnicMem(vecVNetVnic);       
        vector<CVNetVnicMem>::iterator itr = vecVNetVnic.begin();
        int64 nProjID = cVNetVmMem.GetProjectId();
        for( ; itr != vecVNetVnic.end(); ++itr)
        {
            string strNetplaneUUID;
            string strPGID = itr->GetVmPgUuid();          
            int32 nIsolationNo = 0;   
            uint32 dwVlanID = 0;
            if(PORTGROUP_OPER_SUCCESS == pPGMgr->IsNeedVlanMap(strPGID, nIsolationNo))
            {
                // vlan map;
                if(PORTGROUP_OPER_SUCCESS != pPGMgr->GetPGNetplane(strPGID, strNetplaneUUID))
                {        
                    VNET_LOG(VNET_LOG_ERROR, "CVlanPoolMgr::FreeVlanForVnic: Call GetPGNetplane(%s) failed.\n", 
                        strPGID.c_str());            
                    return -1;
                }                
                
                CVlanRange cVlanRange;

                nRet = GetVlanRange(strNetplaneUUID, cVlanRange);
                // Netplane 没有VLAN RANGE 
                if(VNET_DB_IS_ITEM_NO_EXIST(nRet))
                {
                    VNET_LOG(VNET_LOG_ERROR, "CVlanPoolMgr::FreeVlanForVnic: Call GetVlanRange(%s) no existed.\n", 
                        strNetplaneUUID.c_str());
                    continue;
                }
                
                if(0 != nRet)
                {
                    VNET_LOG(VNET_LOG_ERROR, "CVlanPoolMgr::FreeVlanForVnic: Call GetVlanRange(%s) failed.\n", 
                        strNetplaneUUID.c_str());  
                    return -1;
                }
                CVlanPool cVlanPool;
                if(0 != cVlanPool.Init(strNetplaneUUID))
                {
                    VNET_LOG(VNET_LOG_ERROR, "CVlanPoolMgr::FreeVlanForVnic: Call Init(%s) vlan pool failed.\n", 
                        strNetplaneUUID.c_str()); 
                    return -1;
                }
                
                if(0 != cVlanPool.FreeVlanID(nProjID, strPGID, nIsolationNo, dwVlanID))
                {
                    return -1;
                }
                
                VNET_LOG(VNET_LOG_INFO, "CVlanPoolMgr::FreeVlanForVnic: Free the vlan(ProjID:0x%llx, PGID:%s, IsoNo:%d vlan[%d]) successfully.\n", 
                    nProjID, strPGID.c_str(), nIsolationNo, dwVlanID); 
            } 
        }
        cVNetVmMem.SetVecVnicMem(vecVNetVnic); 
        return 0;
    }

    int32 CVlanPoolMgr::GetVlanDetail(int64 nProjID, vector<CVNetVnicDetail> &vecVnicDetail)
    {
        CPortGroupMgr *pPGMgr = CPortGroupMgr::GetInstance();
        if(NULL == pPGMgr)
        {        
            VNET_LOG(VNET_LOG_ERROR, "CVlanPoolMgr::GetVlanDetail: CPortGroupMgr instance is NULL.\n");
            return -1;
        }  
                
        vector<CVNetVnicDetail>::iterator itr = vecVnicDetail.begin();
        for( ; itr != vecVnicDetail.end(); ++itr)
        {
            // 不是VLAN隔离的过滤掉;             
            if (EN_ISOLATE_TYPE_VLAN!=itr->GetIsolateType())
            {
                continue;                
            }
            
            cout << "EN_ISOLATE_TYPE_VLAN==itr->GetIsolateType() process ok!!" <<endl;        

            string strPGID = itr->GetPGUuid(); 
            // Query PG vlan config;
            if(EN_SWITCHPORT_MODE_TRUNK == itr->GetSwitchportMode())
            {
                int32 nNativeVlan = 0;
                if(0 != pPGMgr->GetPGTrunkNativeVlan(strPGID, nNativeVlan))
                {
                    return -1;
                }
                vector<CTrunkVlanRange> vecVlanRange;
                if(0 != pPGMgr->GetPGTrunkVlanRange(strPGID, vecVlanRange))
                {
                    return -1;
                }                
                itr->SetNativeVlan(nNativeVlan);
                itr->SetTrunkVlan(vecVlanRange);
            }
            // Query Vlan Map; 
            else if(EN_SWITCHPORT_MODE_ACCESS == itr->GetSwitchportMode())
            {
                int32 nAccessVlan = 0;
                if(0 != pPGMgr->GetPGAccessVlan(nProjID, strPGID, nAccessVlan))
                {
                    VNET_LOG(VNET_LOG_INFO, "CVlanPoolMgr::GetVlanDetail: Get the VSI(ProjID:0x%llx, PGID:%s, VSIID:%s) access vlan failed.\n", 
                        nProjID, strPGID.c_str(), itr->GetVSIIDValue().c_str()); 
                    return -1;
                }
                itr->SetAccessCvlan(nAccessVlan);
            }
            else
            {
                VNET_LOG(VNET_LOG_INFO, "CVlanPoolMgr::GetVlanDetail: Get the VSI(ProjID:0x%llx, PGID:%s, VSIID:%s) vlan failed.\n", 
                    nProjID, strPGID.c_str(), itr->GetVSIIDValue().c_str()); 
                return -1;
            }
        }
        
        return 0;
    }
    
    int32 CVlanPoolMgr::IsRangeCrossRanges(uint32 dwVlanStart, uint32 dwVlanEnd, const string &cstrNetplaneID)
    {    
        if(dwVlanStart > dwVlanEnd)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVlanPoolMgr::IsRangeCrossRanges: The vlan Range(%d,%d) is invalid.\n", 
                dwVlanStart, dwVlanEnd); 
            VNET_ASSERT(0);
            return 1;
        }
        
        CVlanRange cVlanRange;
        if(0 != GetVlanRange(cstrNetplaneID, cVlanRange))
        {
            VNET_LOG(VNET_LOG_ERROR, "CVlanPoolMgr::IsRangeCrossRanges: Call GetVlanRange(%s) failed.\n", 
                cstrNetplaneID.c_str());  
            return 1;
        }
        
        vector<TVlanRangeItem> &vecVlanRange = cVlanRange.GetVlanRange();
        vector<TVlanRangeItem>::iterator itr = vecVlanRange.begin();
        for(; itr != vecVlanRange.end(); ++itr)
        {
            if(dwVlanEnd >= itr->dwVlanIDStart && dwVlanStart <= itr->dwVlanIDEnd)
            {
                return 1;
            }
        }
        
        return 0;
    }
    
    int32 CVlanPoolMgr::IsInVlanMapRanges(uint32 dwVlanNum, const string &cstrNetplaneID)
    {
        CVlanRange cVlanRange;
        if(0 != GetVlanRange(cstrNetplaneID, cVlanRange))
        {
            VNET_LOG(VNET_LOG_ERROR, "CVlanPoolMgr::IsInVlanMapRanges: Call GetVlanRange(%s) failed.\n", 
                cstrNetplaneID.c_str());  
            return 1;
        }
        
        vector<TVlanRangeItem> &vecVlanRange = cVlanRange.GetVlanRange();
        vector<TVlanRangeItem>::iterator itr = vecVlanRange.begin();
        for(; itr != vecVlanRange.end(); ++itr)
        {
            if(dwVlanNum >= itr->dwVlanIDStart && dwVlanNum <= itr->dwVlanIDEnd)
            {
                return 1;
            }
        }

        return 0;
    }    

    int32 CVlanPoolMgr::IsExistVlanRange(const string &cstrNetplaneID)
    {
        if(cstrNetplaneID.empty())
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CVlanPoolMgr::IsExistVlanRange: Netplane is NULL.\n");
            return 0;
        }
        
        CDBOperateNetplaneVlanRange *pOper = GetDbINetplaneVlanRange();
        if(NULL == pOper)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVlanPoolMgr::IsExistVlanRange: GetDbINetplaneVlanRange() is NULL.\n");
            return 0;
        }
        
        vector<CDbNetplaneVlanRange> outVInfo;
        int32 nRet = pOper->Query(cstrNetplaneID.c_str(), outVInfo);
        if(VNET_DB_SUCCESS != nRet)
        {
            if(VNET_DB_IS_ITEM_NO_EXIST(nRet))
            {
                VNET_LOG(VNET_LOG_ERROR, 
                    "CVlanPoolMgr::IsExistVlanRange: Netplane's(UUID: %s) vlan range is not exist, ret: %d.\n",
                    cstrNetplaneID.c_str(), nRet);
                return 0;
            }
            
            VNET_LOG(VNET_LOG_ERROR, 
                "CVlanPoolMgr::IsExistVlanRange: DB Query Netplane's(UUID: %s) vlan range failed, ret: %d.\n",
                cstrNetplaneID.c_str(), nRet);
            return 0;
        }

        if(outVInfo.empty())
        {
            return 0;
        }
        
        return 1;
    }
    
    int32 CVlanPoolMgr::Init(MessageUnit *pMu)
    {
        if (pMu == NULL)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVlanPoolMgr::Init: pMu is NULL.\n");
            return -1;
        }
        
        m_pMU = pMu;
        
        return 0;
    }
        
    void CVlanPoolMgr::MessageEntry(const MessageFrame &message)
    {
        switch (message.option.id())
        {
        case EV_VNETLIB_VLAN_RANGE_CFG:
            {
                ProcVlanRangeMsg(message);
            }
            break;
        default:
            {
                VNET_ASSERT(0);
            }
        }
        
        return ;
    }

    int32 CVlanPoolMgr::ProcVlanRangeMsg(const MessageFrame &message)
    {
        if(NULL == m_pMU)
        {
            VNET_ASSERT(0);
            return -1;
        }
        
        CVlanRangeMsg cMsg;
        cMsg.deserialize(message.data);
        string strRetInfo;
        int32 nRet = 0;
        MessageOption option(message.option.sender(), EV_VNETLIB_VLAN_RANGE_CFG, 0, 0);
        
        if(m_bOpenDbgInf)
        {
            cMsg.Print();
        }       
        
        if(!cMsg.IsValidOper())
        {
            VNET_LOG(VNET_LOG_WARN, "CVlanPoolMgr::ProcVlanRangeMsg: Msg operation code (%d) is invalid.\n", 
                cMsg.m_dwOper);
            VNetFormatString(strRetInfo, "Msg operation code (%d) is invalid.\n", cMsg.m_dwOper);
            cMsg.SetResult(VLAN_RANGE_CFG_PARAM_ERROR);
            cMsg.SetResultinfo(strRetInfo);
            m_pMU->Send(cMsg, option);
            return -1;
        }
        
        switch(cMsg.m_dwOper)
        {
        case EN_ADD_VLAN_RANGE:
            {
                nRet = AddVlanRange(cMsg, strRetInfo);
            }
            break;
        case EN_DEL_VLAN_RANGE:
            {
                nRet = DelVlanRange(cMsg, strRetInfo);
            }
            break;
        default:
            {
                VNET_ASSERT(0);
                strRetInfo = "The vlan range config param is invalid.";
                nRet = VLAN_RANGE_CFG_PARAM_ERROR;
                
            }
        }
        
        cMsg.SetResult(nRet);
        cMsg.SetResultinfo(strRetInfo);
        m_pMU->Send(cMsg, option);
        
        return 0;
    }
    
    int32 CVlanPoolMgr::IsConflictWithShareNetAccessVlan(const CVlanRangeMsg &cMsg, int32 &nConflictVlan)
    {
        CDBOperateNetplaneVlanPool * pOper = GetDbINetplaneVlanPool();
        if(NULL == pOper)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVlanPoolMgr::IsConflictWithShareNetAccessVlan: GetDbINetplaneVlanPool() is NULL.\n");            
            return 1;
        }
        vector<int32> outVInfo;
        int32 nRet = pOper->QueryShareNetAccessVlan(cMsg.m_strNetplaneUUID.c_str(), outVInfo);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CVlanPoolMgr::IsConflictWithShareNetAccessVlan: DB Query Netplane's(UUID: %s) share access vlan failed, ret = %d.\n",
                cMsg.m_strNetplaneUUID.c_str(), nRet);
            return 1;
        } 
        for(vector<int32>::iterator itr = outVInfo.begin(); itr != outVInfo.end(); ++itr)
        {
            if((*itr) >= cMsg.m_nVlanStart && (*itr) <= cMsg.m_nVlanEnd)
            {
                nConflictVlan = *itr;
                return 1;
            }
        }
        return 0;
    }

    int32 CVlanPoolMgr::IsConflictWithShareNetTrunkVlan(const CVlanRangeMsg &cMsg, 
                                                        int32 &nConflictVlanStart, 
                                                        int32 &nConflictVlanEnd)
    {
        CDBOperateNetplaneVlanPool * pOper = GetDbINetplaneVlanPool();
        if(NULL == pOper)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVlanPoolMgr::IsConflictWithShareNetTrunkVlan: GetDbINetplaneVlanPool() is NULL.\n");            
            return 1;
        }
        
        vector<CDbNetplaneTrunkVlan> outVInfo;
        int32 nRet = pOper->QueryShareNetTrunkVlan(cMsg.m_strNetplaneUUID.c_str(), outVInfo);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CVlanPoolMgr::IsConflictWithShareNetTrunkVlan: DB Query Netplane's(UUID: %s) share access vlan failed, ret = %d.\n",
                cMsg.m_strNetplaneUUID.c_str(), nRet);
            return 1;
        } 
        
        for(vector<CDbNetplaneTrunkVlan>::iterator itr = outVInfo.begin(); itr != outVInfo.end(); ++itr)
        {
            int32 nVlanBegin = itr->GetVlanBegin();
            int32 nVlanEnd = itr->GetVlanEnd();
            if(cMsg.m_nVlanEnd >= nVlanBegin && cMsg.m_nVlanStart <= nVlanEnd)
            {
                nConflictVlanStart = nVlanBegin;
                nConflictVlanEnd = nVlanEnd;
                return 1;
            }
        }
        
        return 0;
    }
    
    int32 CVlanPoolMgr::GetIsolateNo(const CVlanRangeMsg &cMsg, vector<int32> &vecIsolateNo)                        
    {
        CDBOperateNetplaneVlanPool * pOper = GetDbINetplaneVlanPool();
        if(NULL == pOper)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVlanPoolMgr::GetIsolateNo: GetDbINetplaneVlanPool() is NULL.\n");            
            return -1;
        }
        
        int32 nRet = pOper->QueryPrivateNetIsolateNo(cMsg.m_strNetplaneUUID.c_str(), vecIsolateNo);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CVlanPoolMgr::GetIsolateNo: DB Query Netplane's(UUID: %s) share access vlan failed, ret = %d.\n",
                cMsg.m_strNetplaneUUID.c_str(), nRet);
            return -1;
        } 
        
        return 0;
    }
    
    int32 CVlanPoolMgr::AddVlanRange(const CVlanRangeMsg &cMsg, string &strRetInfo)
    {
        if(cMsg.m_strNetplaneUUID.empty())
        {
            VNET_ASSERT(0);
            strRetInfo = "The vlan range config param is invalid.";
            return VLAN_RANGE_CFG_PARAM_ERROR;
        }

        if(!cMsg.IsValidRange())
        {            
            VNET_LOG(VNET_LOG_WARN, "CVlanPoolMgr::AddVlanRange: The vlan range(%d-%d) is invalid.\n", 
                cMsg.m_nVlanStart, cMsg.m_nVlanEnd);
            VNetFormatString(strRetInfo, "The vlan range(%d-%d) is invalid.\n", 
                cMsg.m_nVlanStart, cMsg.m_nVlanEnd);
            return VLAN_RANGE_CFG_PARAM_ERROR;
        }

        int32 nAccessVlan = 0;
        if(IsConflictWithShareNetAccessVlan(cMsg, nAccessVlan))
        {
            VNET_LOG(VNET_LOG_WARN, "CVlanPoolMgr::AddVlanRange: The vlan range(%d-%d) is in conflict with share net access vlan(%d).\n", 
                cMsg.m_nVlanStart, cMsg.m_nVlanEnd, nAccessVlan);
            VNetFormatString(strRetInfo, "The vlan range(%d-%d) is in conflict with share net access vlan(%d).\n", 
                cMsg.m_nVlanStart, cMsg.m_nVlanEnd, nAccessVlan);
            return VLAN_RANGE_CFG_PARAM_ERROR;
        }
        
        int32 nTrunkVlanStart = 0;
        int32 nTrunkVlanEnd = 0;        
        if(IsConflictWithShareNetTrunkVlan(cMsg, nTrunkVlanStart, nTrunkVlanEnd))
        {
            VNET_LOG(VNET_LOG_WARN, "CVlanPoolMgr::AddVlanRange: The vlan range(%d-%d) is in conflict with share net trunk vlan(%d-%d).\n", 
                cMsg.m_nVlanStart, cMsg.m_nVlanEnd, nTrunkVlanStart, nTrunkVlanEnd);
            VNetFormatString(strRetInfo, "The vlan range(%d-%d) is in conflict with share net trunk vlan(%d-%d).\n", 
                cMsg.m_nVlanStart, cMsg.m_nVlanEnd, nTrunkVlanStart, nTrunkVlanEnd);
            return VLAN_RANGE_CFG_PARAM_ERROR;
        }
        
        // DB range Load;
        CDBOperateNetplaneVlanRange *pOper = GetDbINetplaneVlanRange();
        if(NULL == pOper)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVlanPoolMgr::AddVlanRange: GetDbINetplaneVlanRange() is NULL.\n");            
            strRetInfo = "The VLAN range database interface is NULL.";
            return VLAN_RANGE_GET_DB_INTF_ERROR;
        }
        vector<CDbNetplaneVlanRange> outVInfo;
        int32 nRet = pOper->Query(cMsg.m_strNetplaneUUID.c_str(), outVInfo);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CVlanPoolMgr::AddVlanRange: DB Query Netplane's(UUID: %s) vlan range failed, ret = %d.\n",
                cMsg.m_strNetplaneUUID.c_str(), nRet);
            VNetFormatString(strRetInfo, "Get the net plane's(UUID: %s) vlan range failed.\n",
                cMsg.m_strNetplaneUUID.c_str());
            return nRet;
        }         
        CVlanRange cVlanRange;
        CDbNetplaneVlanRange cDbIPRange;
        vector<CDbNetplaneVlanRange>::iterator itrDBRange;
        if(!outVInfo.empty())
        {
            for(itrDBRange = outVInfo.begin(); itrDBRange != outVInfo.end(); ++itrDBRange)
            {
                TVlanRangeItem tItem;
                tItem.dwVlanIDStart = itrDBRange->GetBeginNum();
                tItem.dwVlanIDEnd = itrDBRange->GetEndNum();
                cVlanRange.AddValidRange(tItem);
            }
        }
        else
        {// only one record, save it to DB;
            cDbIPRange.SetUuid(cMsg.m_strNetplaneUUID.c_str());
            cDbIPRange.SetBeginNum(cMsg.m_nVlanStart);
            cDbIPRange.SetEndNum(cMsg.m_nVlanEnd);
            nRet = pOper->Add(cDbIPRange);
            if(VNET_DB_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, 
                    "CVlanPoolMgr::AddVlanRange: DB Add Netplane's(UUID: %s) vlan range[%d-%d] failed, nRet = %d.\n",
                    cMsg.m_strNetplaneUUID.c_str(),
                    cMsg.m_nVlanStart,
                    cMsg.m_nVlanEnd,
                    nRet);
                VNetFormatString(strRetInfo, "Add the net plane's(UUID: %s) vlan range[%d-%d] to the database failed.\n",
                    cMsg.m_strNetplaneUUID.c_str(),
                    cMsg.m_nVlanStart,
                    cMsg.m_nVlanEnd);
                return nRet;
            } 
            return VLAN_RANGE_OPER_SUCCESS;
        }      
        // range add;
        TVlanRangeItem tNewItem;
        tNewItem.dwVlanIDStart = cMsg.m_nVlanStart;
        tNewItem.dwVlanIDEnd = cMsg.m_nVlanEnd;
        if(0 != cVlanRange.AddRange(tNewItem))
        {            
            VNET_LOG(VNET_LOG_ERROR, 
                "CVlanPoolMgr::AddVlanRange: vlan range[%d-%d] calculate failed.\n",
                cMsg.m_nVlanStart, 
                cMsg.m_nVlanEnd);
            VNetFormatString(strRetInfo, "Combining the calculation of vlan range[%d-%d] failed.\n",
                cMsg.m_nVlanStart,
                cMsg.m_nVlanEnd);
            return VLAN_RANGE_CALC_ERROR;
        }

        // DB Compare and save;Need Transaction;
        vector<TVlanRangeItem> &vecCalcRange = cVlanRange.GetVlanRange();
        vector<TVlanRangeItem>::iterator itrCalcRange = vecCalcRange.begin();
        for(; itrCalcRange != vecCalcRange.end(); ++itrCalcRange)
        {            
            int32 bFind = 0;
            for(itrDBRange = outVInfo.begin(); itrDBRange != outVInfo.end(); ++itrDBRange)
            {
                TVlanRangeItem tIPRange;
                tIPRange.dwVlanIDStart = itrDBRange->GetBeginNum();
                tIPRange.dwVlanIDEnd = itrDBRange->GetEndNum();
                if(tIPRange.dwVlanIDStart == itrCalcRange->dwVlanIDStart &&
                   tIPRange.dwVlanIDEnd == itrCalcRange->dwVlanIDEnd)
                {
                    bFind = 1;
                    outVInfo.erase(itrDBRange);
                    break;
                }
            }
            
            if(0 == bFind)
            {                
                // Add To DB
                cDbIPRange.SetUuid(cMsg.m_strNetplaneUUID.c_str());
                cDbIPRange.SetBeginNum(itrCalcRange->dwVlanIDStart);
                cDbIPRange.SetEndNum(itrCalcRange->dwVlanIDEnd);
                nRet = pOper->Add(cDbIPRange);
                if(VNET_DB_SUCCESS != nRet)
                {
                    VNET_LOG(VNET_LOG_ERROR, 
                        "CVlanPoolMgr::AddVlanRange: DB Merge Netplane's(UUID: %s) vlan range[%d-%d] failed, nRet = %d.\n",
                        cMsg.m_strNetplaneUUID.c_str(),
                        cMsg.m_nVlanStart, 
                        cMsg.m_nVlanEnd,
                        nRet);
                    VNetFormatString(strRetInfo, "Add the net plane's(UUID: %s) vlan range[%d-%d] to the database failed.\n",
                        cMsg.m_strNetplaneUUID.c_str(),
                        cMsg.m_nVlanStart, 
                        cMsg.m_nVlanEnd);
                    return nRet;           
                }             
            }
        }
        // delete invalid record from db;
        for(itrDBRange = outVInfo.begin(); itrDBRange != outVInfo.end(); ++itrDBRange)
        {
            nRet = pOper->Del(*itrDBRange);
            if(VNET_DB_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, 
                    "CVlanPoolMgr::AddVlanRange: DB Merge Netplane's(UUID: %s) vlan range[%d-%d] failed, nRet = %d.\n",
                    cMsg.m_strNetplaneUUID.c_str(),
                    cMsg.m_nVlanStart,
                    cMsg.m_nVlanEnd,
                    nRet);
                VNetFormatString(strRetInfo, "Combining the calculation of net plane's(UUID: %s) vlan range[%d-%d] to the database failed.\n",
                    cMsg.m_strNetplaneUUID.c_str(),
                    cMsg.m_nVlanStart, 
                    cMsg.m_nVlanEnd);
                return nRet;
            }             
        }

        return VLAN_RANGE_OPER_SUCCESS;
    }
    
    int32 CVlanPoolMgr::DelVlanRange(const CVlanRangeMsg &cMsg, string &strRetInfo)
    {
        if(cMsg.m_strNetplaneUUID.empty())
        {
            VNET_ASSERT(0);
            strRetInfo = "The vlan range config param is invalid.";
            return VLAN_RANGE_CFG_PARAM_ERROR;
        }

        if(!cMsg.IsValidRange())
        {            
            VNET_LOG(VNET_LOG_WARN, "CVlanPoolMgr::DelVlanRange: The vlan range(%d-%d) is invalid.\n", 
                cMsg.m_nVlanStart, cMsg.m_nVlanEnd);
            VNetFormatString(strRetInfo, "The vlan range(%d-%d) is invalid.\n",
                cMsg.m_nVlanStart, cMsg.m_nVlanEnd);
            return VLAN_RANGE_CFG_PARAM_ERROR;
        }
        
        // DB range Load;
        CDBOperateNetplaneVlanRange *pOper = GetDbINetplaneVlanRange();
        if(NULL == pOper)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVlanPoolMgr::DelVlanRange: GetDbINetplaneVlanRange() is NULL.\n");
            strRetInfo = "The VLAN range database interface is NULL.";
            return VLAN_RANGE_GET_DB_INTF_ERROR;
        }
        vector<CDbNetplaneVlanRange> outVInfo;
        int32 nRet = pOper->Query(cMsg.m_strNetplaneUUID.c_str(), outVInfo);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CVlanPoolMgr::DelVlanRange: DB Query Netplane's(UUID: %s) vlan range failed, ret: %d.\n",
                cMsg.m_strNetplaneUUID.c_str(), nRet);
            VNetFormatString(strRetInfo, "Get the net plane's(UUID: %s) vlan range failed.\n",
                cMsg.m_strNetplaneUUID.c_str());
            return nRet;
        }         
        CVlanRange cVlanRange;
        CDbNetplaneVlanRange cDbIPRange;
        vector<CDbNetplaneVlanRange>::iterator itrDBRange;
        if(outVInfo.empty())
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CVlanPoolMgr::DelVlanRange: Netplane's(UUID: %s) vlan range(%d-%d) is not exist.\n",
                cMsg.m_strNetplaneUUID.c_str(),
                cMsg.m_nVlanStart,
                cMsg.m_nVlanEnd);
            VNetFormatString(strRetInfo, "The net plane's(UUID: %s) vlan range is not configured.\n",
                cMsg.m_strNetplaneUUID.c_str());
            return VLAN_RANGE_NOT_EXIST;
        }
        
        // Check the range is used by vlan pool or not.
        int32 nOccNum = 0;
        nRet = pOper->QueryAllocNum(cMsg.m_strNetplaneUUID.c_str(), 
                             cMsg.m_nVlanStart,
                             cMsg.m_nVlanEnd,
                             nOccNum);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CVlanPoolMgr::DelVlanRange: call QueryAllocNum (%s, %d, %d) failed, ret = %d.\n",
                cMsg.m_strNetplaneUUID.c_str(),
                cMsg.m_nVlanStart,
                cMsg.m_nVlanEnd,
                nRet);
            VNetFormatString(strRetInfo, "Query the net plane's(UUID: %s) vlan range used state failed.\n",
                cMsg.m_strNetplaneUUID.c_str());
            return nRet;
        }
        if(0 != nOccNum)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CVlanPoolMgr::DelVlanRange: The net plane's(UUID: %s) vlan range[%d-%d] is occupied,num:%d.\n",
                cMsg.m_strNetplaneUUID.c_str(),
                cMsg.m_nVlanStart,
                cMsg.m_nVlanEnd,
                nOccNum);
            VNetFormatString(strRetInfo, "The net plane's(UUID: %s) vlan range[%d-%d] is used.\n",
                cMsg.m_strNetplaneUUID.c_str(),
                cMsg.m_nVlanStart,
                cMsg.m_nVlanEnd);
            return VLAN_RANGE_OCCUPIED;
        }        

        for(itrDBRange = outVInfo.begin(); itrDBRange != outVInfo.end(); ++itrDBRange)
        {
            TVlanRangeItem tItem;
            tItem.dwVlanIDStart = itrDBRange->GetBeginNum();
            tItem.dwVlanIDEnd = itrDBRange->GetEndNum();
            cVlanRange.AddValidRange(tItem);
        }
     
        // range del;
        TVlanRangeItem tNewItem;
        tNewItem.dwVlanIDStart = cMsg.m_nVlanStart;
        tNewItem.dwVlanIDEnd = cMsg.m_nVlanEnd;
        if(0 != cVlanRange.DelRange(tNewItem))
        {            
            VNET_LOG(VNET_LOG_ERROR, 
                "CVlanPoolMgr::DelVlanRange: vlan range[%d-%d] calculate failed.\n",
                cMsg.m_nVlanStart, 
                cMsg.m_nVlanEnd);
            VNetFormatString(strRetInfo, "Combining the calculation of vlan range[%d-%d] failed.\n",
                cMsg.m_nVlanStart,
                cMsg.m_nVlanEnd);
            return VLAN_RANGE_CALC_ERROR;
        }

        // 如果存在虚拟机网络私有类型端口组，需要限制其不能为空范围;        
        vector<TVlanRangeItem> &vecCalcRange = cVlanRange.GetVlanRange();
        vector<TVlanRangeItem>::iterator itrCalcRange = vecCalcRange.begin();
        if(vecCalcRange.empty())
        {
            vector<int32> vecIsolateNo;
            nRet = GetIsolateNo(cMsg, vecIsolateNo);
            if(nRet != 0)
            {
                VNET_LOG(VNET_LOG_ERROR, 
                    "CVlanPoolMgr::DelVlanRange: Get net plane(%s) private net isolate no list failed.\n",
                    cMsg.m_strNetplaneUUID.c_str());
                VNetFormatString(strRetInfo, "Get net plane(%s) private net isolate no list failed.\n",
                    cMsg.m_strNetplaneUUID.c_str());
                return VLAN_RANGE_CALC_ERROR;
            }
            if(!vecIsolateNo.empty())
            {
                VNET_LOG(VNET_LOG_ERROR, 
                    "CVlanPoolMgr::DelVlanRange: The net plane's(UUID: %s) vlan range[%d-%d] is occupied.\n",
                    cMsg.m_strNetplaneUUID.c_str(),
                    cMsg.m_nVlanStart, 
                    cMsg.m_nVlanEnd);
                VNetFormatString(strRetInfo, "The net plane's(UUID: %s) vlan range[%d-%d] is used.\n",
                    cMsg.m_strNetplaneUUID.c_str(),
                    cMsg.m_nVlanStart,
                    cMsg.m_nVlanEnd);
                return VLAN_RANGE_OCCUPIED;
            }
        }
        // DB Compare and save;Need Transaction;
        for(; itrCalcRange != vecCalcRange.end(); ++itrCalcRange)
        {            
            int32 bFind = 0;
            for(itrDBRange = outVInfo.begin(); itrDBRange != outVInfo.end(); ++itrDBRange)
            {
                TVlanRangeItem tIPRange;
                tIPRange.dwVlanIDStart = itrDBRange->GetBeginNum();
                tIPRange.dwVlanIDEnd = itrDBRange->GetEndNum();
                if(tIPRange.dwVlanIDStart == itrCalcRange->dwVlanIDStart &&
                   tIPRange.dwVlanIDEnd == itrCalcRange->dwVlanIDEnd)
                {
                    bFind = 1;
                    outVInfo.erase(itrDBRange);
                    break;
                }
            }
            
            if(0 == bFind)
            {                
                // Add To DB
                cDbIPRange.SetUuid(cMsg.m_strNetplaneUUID.c_str());
                cDbIPRange.SetBeginNum(itrCalcRange->dwVlanIDStart);
                cDbIPRange.SetEndNum(itrCalcRange->dwVlanIDEnd);
                nRet = pOper->Add(cDbIPRange);
                if(VNET_DB_SUCCESS != nRet)
                {
                    VNET_LOG(VNET_LOG_ERROR, 
                        "CVlanPoolMgr::AddVlanRange: DB Merge Netplane's(UUID: %s) vlan range[%d-%d] failed, ret = %d.\n",
                        cMsg.m_strNetplaneUUID.c_str(),
                        cMsg.m_nVlanStart, 
                        cMsg.m_nVlanEnd,
                        nRet);
                    VNetFormatString(strRetInfo, "Merge the net plane's(UUID: %s) vlan range[%d-%d] to the data base failed.\n",
                        cMsg.m_nVlanStart,
                        cMsg.m_nVlanEnd);
                    return nRet;                
                }             
            }
        }
        // delete invalid record from db;
        for(itrDBRange = outVInfo.begin(); itrDBRange != outVInfo.end(); ++itrDBRange)
        {
            nRet = pOper->Del(*itrDBRange);
            if(VNET_DB_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, 
                    "CVlanPoolMgr::DelVlanRange: Netplane's(UUID: %s) vlan range(%d-%d) is not exist, ret = %d.\n",
                    cMsg.m_strNetplaneUUID.c_str(),
                    cMsg.m_nVlanStart,
                    cMsg.m_nVlanEnd,
                    nRet);
                VNetFormatString(strRetInfo, "Combining the calculation of net plane's(UUID: %s) vlan range[%d-%d] to the data base failed.\n",
                    cMsg.m_strNetplaneUUID.c_str(),
                    cMsg.m_nVlanStart,
                    cMsg.m_nVlanEnd);
                return nRet;  
            }           
        }
        return VLAN_RANGE_OPER_SUCCESS;
    } 
    
    int32 CVlanPoolMgr::GetVlanRange(const string &cstrNetplaneID, CVlanRange &cVlanRange)
    {
        if(cstrNetplaneID.empty())
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CVlanPoolMgr::GetVlanRange: Netplane is NULL.\n");
            return -1;
        }
        
        CDBOperateNetplaneVlanRange *pOper = GetDbINetplaneVlanRange();
        if(NULL == pOper)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVlanPoolMgr::GetVlanRange: GetDbINetplaneVlanRange() is NULL.\n");
            return -1;
        }
        vector<CDbNetplaneVlanRange> outVInfo;
        int32 nRet = pOper->Query(cstrNetplaneID.c_str(), outVInfo);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CVlanPoolMgr::GetVlanRange: DB Query Netplane's(UUID: %s) vlan range failed, ret: %d.\n",
                cstrNetplaneID.c_str(), nRet);
            return nRet;
        }         
        CDbNetplaneVlanRange cDbIPRange;
        vector<CDbNetplaneVlanRange>::iterator itrDBRange;
        if(outVInfo.empty())
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CVlanPoolMgr::DelVlanRange: Netplane's(UUID: %s) vlan range is not exist.\n",
                cstrNetplaneID.c_str());
            return -1;
        }

        for(itrDBRange = outVInfo.begin(); itrDBRange != outVInfo.end(); ++itrDBRange)
        {
            TVlanRangeItem tItem;
            tItem.dwVlanIDStart = itrDBRange->GetBeginNum();
            tItem.dwVlanIDEnd = itrDBRange->GetEndNum();
            cVlanRange.AddValidRange(tItem);
        }

        return 0;
    }

    int32 CVlanPoolMgr::DbgShowVlanPool(const string &cstrLogicNetworkID)
    {
        return 0;
    }

    void VNetDbgTestNPVlanRangeOper(const char* cstrNetplaneUUID, 
                                 int32 nVlanStart, 
                                 int32 nVlanEnd, 
                                 uint32 dwOper)
    {
        if(NULL == cstrNetplaneUUID) 
        {
            cout << "Input param error." << endl;
            return ;
        }

        if((dwOper <= EN_INVALID_VLAN_RANGE_OPER) || (dwOper >= EN_VLAN_RANGE_OPER_ALL))
        {
            cout << "Operation Code: Add[" << EN_ADD_VLAN_RANGE
                 << "], Del[" << EN_DEL_VLAN_RANGE
                 << "]." <<endl;
            return ;
        }

        MessageUnit tempmu(TempUnitName("TestNPVlanRangeOper"));
        tempmu.Register();
        CVlanRangeMsg cMsg;
        string strTmp;
        strTmp.assign(cstrNetplaneUUID);
        cMsg.m_strNetplaneUUID = strTmp;
        cMsg.m_nVlanStart = nVlanStart;
        cMsg.m_nVlanEnd = nVlanEnd;
        cMsg.m_dwOper = dwOper;
        MessageOption option(MU_VNM, EV_VNETLIB_VLAN_RANGE_CFG, 0, 0);
        tempmu.Send(cMsg,option);
        
        return ;
    }
    DEFINE_DEBUG_FUNC(VNetDbgTestNPVlanRangeOper);

    void VNetDbgTestAllocVlan(int32 nProjID, const char *cstrLogicNetworkID)
    {
        if(NULL == cstrLogicNetworkID)
        {
            cout << "Input param error." << endl;
            return ;
        }

        CLogicNetworkMgr *pLogiNetMgr = CLogicNetworkMgr::GetInstance();
        if(NULL == pLogiNetMgr)
        {
            cout << "No LogicNetwork instance." << endl;
            return ;
        }

        CVlanPoolMgr *pVlanMgr = CVlanPoolMgr::GetInstance();
        if(NULL == pVlanMgr)
        {
            cout << "No Vlan pool instance." << endl;
            return ;
        }

        string strTmp;
        strTmp.assign(cstrLogicNetworkID);
        CLogicNetwork cLogicNetwork(""); 
        string strLogiNetworkID = strTmp;
        if(0 != pLogiNetMgr->GetLogicNetwork(strLogiNetworkID, cLogicNetwork))
        {
            cout << "Can't find " << strLogiNetworkID << " logic network." << endl;
            return ;
        }
        string strPGID = cLogicNetwork.GetPortGroupUUID(); 

        CVNetVnicMem cVNic;
        strTmp.assign(cstrLogicNetworkID);
        cVNic.SetLogicNetworkUuid(strLogiNetworkID);
        cVNic.SetVmPgUuid(strPGID);
        vector<CVNetVnicMem> vecVNetVnic;
        CVNetVmMem cVNetVmMem;
        for(int32 i = 1; i <= 10; ++i)
        {            
            cVNic.SetNicIndex(i);
            vecVNetVnic.push_back(cVNic);
        }
        cVNetVmMem.SetProjectId(nProjID);
        cVNetVmMem.SetVecVnicMem(vecVNetVnic);
        pVlanMgr->AllocVlanForVnic(cVNetVmMem);
        return ;
    }
    DEFINE_DEBUG_FUNC(VNetDbgTestAllocVlan);

    void VNetDbgTestFreeVlan(int32 nProjID, const char *cstrLogicNetworkID)
    {
        if(NULL == cstrLogicNetworkID)
        {
            cout << "Input param error." << endl;
            return ;
        }

        CLogicNetworkMgr *pLogiNetMgr = CLogicNetworkMgr::GetInstance();
        if(NULL == pLogiNetMgr)
        {
            cout << "No LogicNetwork instance." << endl;
            return ;
        }

        CVlanPoolMgr *pVlanMgr = CVlanPoolMgr::GetInstance();
        if(NULL == pVlanMgr)
        {
            cout << "No Vlan pool instance." << endl;
            return ;
        }

        string strTmp;
        strTmp.assign(cstrLogicNetworkID);
        CLogicNetwork cLogicNetwork(""); 
        string strLogiNetworkID = strTmp;
        if(0 != pLogiNetMgr->GetLogicNetwork(strLogiNetworkID, cLogicNetwork))
        {
            cout << "Can't find " << strLogiNetworkID << " logic network." << endl;
            return ;
        }
        string strPGID = cLogicNetwork.GetPortGroupUUID();

        CVNetVnicMem cVNic;
        cVNic.SetLogicNetworkUuid(strLogiNetworkID);
        cVNic.SetVmPgUuid(strPGID);
        cVNic.SetNicIndex(1);        
        vector<CVNetVnicMem> vecVNetVnic;                
        vecVNetVnic.push_back(cVNic);
        
        CVNetVmMem cVNetVmMem;
        cVNetVmMem.SetProjectId(nProjID);
        cVNetVmMem.SetVecVnicMem(vecVNetVnic);
        pVlanMgr->FreeVlanForVnic(cVNetVmMem);

        return ;
    }
    DEFINE_DEBUG_FUNC(VNetDbgTestFreeVlan);

    void VNetDbgShowVlanPool(const char *cstrLogiNetUUID)
    {
        if(NULL == cstrLogiNetUUID)
        {
            cout << "Input param error." << endl;
            return ;
        }
        
        CVlanPoolMgr *pMgr = CVlanPoolMgr::GetInstance();
        if(pMgr)
        {
            string strTmp;
            strTmp.assign(cstrLogiNetUUID);
            pMgr->DbgShowVlanPool(strTmp);
        }
        
        return ;
    }
    DEFINE_DEBUG_FUNC(VNetDbgShowVlanPool);  

    void VNetDbgSetVlanPoolPrint(BOOL bOpen)
    {
        CVlanPoolMgr *pMgr = CVlanPoolMgr::GetInstance();
        if(pMgr)
        {
            pMgr->SetDbgInfFlg(bOpen);
        }
        
        return ;
    }
    DEFINE_DEBUG_FUNC(VNetDbgSetVlanPoolPrint);    

}// end namespace zte_tecs

