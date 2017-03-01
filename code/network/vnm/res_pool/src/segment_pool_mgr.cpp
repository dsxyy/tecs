/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：segment_pool_mgr.cpp
* 文件标识：
* 内容摘要：CSegmentPoolMgr类实现文件
* 当前版本：1.0
* 作    者：
* 完成日期：2013年09月05日
*
* 修改记录1：
*     修改日期：2013/09/05
*     版 本 号：V1.0
*     修 改 人：zhao.jin
*     修改内容：创建
******************************************************************************/

#include "vnet_comm.h"
#include "vnet_mid.h"
#include "vnet_error.h"
#include "vnetlib_event.h"
#include "vnetlib_msg.h"

#include "vnet_db_proc.h"
#include "vnet_db_mgr.h"
#include "vnet_db_netplane_segmentrange.h"
#include "vnet_db_segment_map.h"
#include "vnet_db_netplane_segmentpool.h"

#include "ipv4_addr.h"
#include "ipv4_addr_range.h"

#include "vnet_vnic.h"

#include "logic_network.h"
#include "logic_network_mgr.h"

#include "port_group_mgr.h"

#include "segment_range.h"
#include "segment_pool.h"
#include "segment_pool_mgr.h"

namespace zte_tecs
{
    
CSegmentPoolMgr *CSegmentPoolMgr::s_pInstance = NULL;

CSegmentPoolMgr::CSegmentPoolMgr()
{
    m_pMU = NULL;
}

CSegmentPoolMgr::~CSegmentPoolMgr()
{
    m_pMU = NULL;
}

int32 CSegmentPoolMgr::AllocSegmentForVnic(CVNetVmMem & cVNetVmMem)
{
    CPortGroupMgr *pPGMgr = CPortGroupMgr::GetInstance();
    if(NULL == pPGMgr)
    {        
        VNET_LOG(VNET_LOG_ERROR, "CSegmentPoolMgr::AllocSegmentForVnic: CPortGroupMgr instance is NULL.\n");
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
        uint32 dwSegmentID;
        if(PORTGROUP_OPER_SUCCESS == pPGMgr->IsNeedSegmentMap(strPGID, nIsolationNo))
        {
            // Segment map;
            if(PORTGROUP_OPER_SUCCESS != pPGMgr->GetPGNetplane(strPGID, strNetplaneUUID))
            {        
                VNET_LOG(VNET_LOG_ERROR, "CSegmentPoolMgr::AllocSegmentForVnic: Call GetPGNetplane(%s) failed.\n", 
                    strPGID.c_str());
                if(m_bOpenDbgInf)
                {
                    cout << "GetPGNetplane fail!" << endl;
                }  
                return -1;
            }                
            
            CSegmentRange cSegmentRange;
            if(0 != GetSegmentRange(strNetplaneUUID, cSegmentRange))
            {
                VNET_LOG(VNET_LOG_ERROR, "CSegmentPoolMgr::AllocSegmentForVnic: Call GetSegmentRange(%s) failed.\n", 
                    strNetplaneUUID.c_str());  
                if(m_bOpenDbgInf)
                {
                    cout << "GetSegmentRange fail!" << endl;
                }                  
                return -1;
            }
            CSegmentPool cSegmentPool;
            if(0 != cSegmentPool.Init(&cSegmentRange, strNetplaneUUID))
            {
                VNET_LOG(VNET_LOG_ERROR, "CSegmentPoolMgr::AllocSegmentForVnic: Call Init(%s) Segment pool failed.\n", 
                    strNetplaneUUID.c_str()); 
                if(m_bOpenDbgInf)
                {
                    cout << "cSegmentPool.Init fail! NetplaneUUID:" << strNetplaneUUID << endl;
                }                  
                
                return -1;
            }
            
            if(0 != cSegmentPool.AllocSegmentID(strNetplaneUUID, nProjID, strPGID, nIsolationNo, dwSegmentID))
            {
                if(m_bOpenDbgInf)
                {
                    cout << "cSegmentPool.AllocSegmentID fail! NetplaneUUID:" << strNetplaneUUID << endl;
                }                  
                
                return -1;
            }
            
            VNET_LOG(VNET_LOG_INFO, "CSegmentPoolMgr::AllocSegmentForVNic: (ProjID:0x%llx, PGID:%s, IsoNo:%d) is mapped Segment[%d].\n", 
                nProjID, strPGID.c_str(), nIsolationNo, dwSegmentID); 
        }            
    }
    cVNetVmMem.SetVecVnicMem(vecVNetVnic);
    return 0;
}

int32 CSegmentPoolMgr::FreeSegmentForVnic(CVNetVmMem & cVNetVmMem)
{
    CPortGroupMgr *pPGMgr = CPortGroupMgr::GetInstance();
    if(NULL == pPGMgr)
    {        
        VNET_LOG(VNET_LOG_ERROR, "CSegmentPoolMgr::FreeSegmentForVnic: CPortGroupMgr instance is NULL.\n");
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
        uint32 dwSegmentID = 0;
        if(PORTGROUP_OPER_SUCCESS == pPGMgr->IsNeedSegmentMap(strPGID, nIsolationNo))
        {
            // Segment map;
            if(PORTGROUP_OPER_SUCCESS != pPGMgr->GetPGNetplane(strPGID, strNetplaneUUID))
            {        
                VNET_LOG(VNET_LOG_ERROR, "CSegmentPoolMgr::FreeSegmentForVnic: Call GetPGNetplane(%s) failed.\n", 
                    strPGID.c_str());            
                return -1;
            }                
            
            CSegmentRange cSegmentRange;

            nRet = GetSegmentRange(strNetplaneUUID, cSegmentRange);
            // Netplane 没有Segment RANGE 
            if(VNET_DB_IS_ITEM_NO_EXIST(nRet))
            {
                VNET_LOG(VNET_LOG_ERROR, "CSegmentPoolMgr::FreeSegmentForVnic: Call GetSegmentRange(%s) no existed.\n", 
                    strNetplaneUUID.c_str());
                continue;
            }
            
            if(0 != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, "CSegmentPoolMgr::FreeSegmentForVnic: Call GetSegmentRange(%s) failed.\n", 
                    strNetplaneUUID.c_str());  
                return -1;
            }
            CSegmentPool cSegmentPool;
            if(0 != cSegmentPool.Init(strNetplaneUUID))
            {
                VNET_LOG(VNET_LOG_ERROR, "CSegmentPoolMgr::FreeSegmentForVnic: Call Init(%s) Segment pool failed.\n", 
                    strNetplaneUUID.c_str()); 
                return -1;
            }
            
            if(0 != cSegmentPool.FreeSegmentID(nProjID, strPGID, nIsolationNo, dwSegmentID))
            {
                return -1;
            }
            
            VNET_LOG(VNET_LOG_INFO, "CSegmentPoolMgr::FreeSegmentForVnic: Free the Segment(ProjID:0x%llx, PGID:%s, IsoNo:%d Segment[%d]) successfully.\n", 
                nProjID, strPGID.c_str(), nIsolationNo, dwSegmentID); 
        } 
    }
    cVNetVmMem.SetVecVnicMem(vecVNetVnic); 
    return 0;
}

int32 CSegmentPoolMgr::GetSegmentDetail(int64 nProjID, vector<CVNetVnicDetail> &vecVnicDetail)
{
    CPortGroupMgr *pPGMgr = CPortGroupMgr::GetInstance();
    if(NULL == pPGMgr)
    {        
        VNET_LOG(VNET_LOG_ERROR, "CSegmentPoolMgr::GetSegmentDetail: CPortGroupMgr instance is NULL.\n");
        return -1;
    }  
       
    vector<CVNetVnicDetail>::iterator itr = vecVnicDetail.begin();
    for( ; itr != vecVnicDetail.end(); ++itr)
    {       
        // 不是VXLAN隔离的过滤掉;             
        if (EN_ISOLATE_TYPE_VXLAN!=itr->GetIsolateType())
        {
            continue;                
        }

        cout << "EN_ISOLATE_TYPE_VXLAN==itr->GetIsolateType() process ok!!" <<endl;        
        
        // Query SegmentId; 
        int32 nSegmentId = 0;        
        string strPGID = itr->GetPGUuid(); 
        if(0 != pPGMgr->GetPGSegmentId(nProjID, strPGID, nSegmentId))
        {
            VNET_LOG(VNET_LOG_INFO, "CSegmentPoolMgr::GetSegmentDetail: Get the VSI(ProjID:0x%llx, PGID:%s, VSIID:%s) segmentid failed.\n", 
                nProjID, strPGID.c_str(), itr->GetVSIIDValue().c_str()); 
            return -1;
        }
        itr->SetSegmentId(nSegmentId);
    }
    
    return 0;
}

int32 CSegmentPoolMgr::IsRangeCrossRanges(uint32 dwSegmentStart, uint32 dwSegmentEnd, const string &cstrNetplaneID)
{    
    if(dwSegmentStart > dwSegmentEnd)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSegmentPoolMgr::IsRangeCrossRanges: The Segment Range(%d,%d) is invalid.\n", 
            dwSegmentStart, dwSegmentEnd); 
        VNET_ASSERT(0);
        return 1;
    }
    
    CSegmentRange cSegmentRange;
    if(0 != GetSegmentRange(cstrNetplaneID, cSegmentRange))
    {
        VNET_LOG(VNET_LOG_ERROR, "CSegmentPoolMgr::IsRangeCrossRanges: Call GetSegmentRange(%s) failed.\n", 
            cstrNetplaneID.c_str());  
        return 1;
    }
    
    vector<TSegmentRangeItem> &vecSegmentRange = cSegmentRange.GetSegmentRange();
    vector<TSegmentRangeItem>::iterator itr = vecSegmentRange.begin();
    for(; itr != vecSegmentRange.end(); ++itr)
    {
        if(dwSegmentEnd >= itr->dwSegmentIDStart && dwSegmentStart <= itr->dwSegmentIDEnd)
        {
            return 1;
        }
    }
    
    return 0;
}

int32 CSegmentPoolMgr::IsInSegmentMapRanges(uint32 dwSegmentNum, const string &cstrNetplaneID)
{
    CSegmentRange cSegmentRange;
    if(0 != GetSegmentRange(cstrNetplaneID, cSegmentRange))
    {
        VNET_LOG(VNET_LOG_ERROR, "CSegmentPoolMgr::IsInSegmentMapRanges: Call GetSegmentRange(%s) failed.\n", 
            cstrNetplaneID.c_str());  
        return 1;
    }
    
    vector<TSegmentRangeItem> &vecSegmentRange = cSegmentRange.GetSegmentRange();
    vector<TSegmentRangeItem>::iterator itr = vecSegmentRange.begin();
    for(; itr != vecSegmentRange.end(); ++itr)
    {
        if(dwSegmentNum >= itr->dwSegmentIDStart && dwSegmentNum <= itr->dwSegmentIDEnd)
        {
            return 1;
        }
    }

    return 0;
}    

int32 CSegmentPoolMgr::IsExistSegmentRange(const string &cstrNetplaneID)
{
    if(cstrNetplaneID.empty())
    {
        VNET_LOG(VNET_LOG_ERROR, 
            "CSegmentPoolMgr::IsExistSegmentRange: Netplane is NULL.\n");
        return 0;
    }
    
    CDBOperateNetplaneSegmentRange *pOper = GetDbINetplaneSegmentRange();
    if(NULL == pOper)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSegmentPoolMgr::IsExistSegmentRange: GetDbINetplaneSegmentRange() is NULL.\n");
        return 0;
    }
    
    vector<CDbNetplaneSegmentRange> outVInfo;
    int32 nRet = pOper->Query(cstrNetplaneID.c_str(), outVInfo);
    if(VNET_DB_SUCCESS != nRet)
    {
        if(VNET_DB_IS_ITEM_NO_EXIST(nRet))
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CSegmentPoolMgr::IsExistSegmentRange: Netplane's(UUID: %s) Segment range is not exist, ret: %d.\n",
                cstrNetplaneID.c_str(), nRet);
            return 0;
        }
        
        VNET_LOG(VNET_LOG_ERROR, 
            "CSegmentPoolMgr::IsExistSegmentRange: DB Query Netplane's(UUID: %s) Segment range failed, ret: %d.\n",
            cstrNetplaneID.c_str(), nRet);
        return 0;
    }

    if(outVInfo.empty())
    {
        return 0;
    }
    
    return 1;
}

int32 CSegmentPoolMgr::Init(MessageUnit *pMu)
{
    if (pMu == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSegmentPoolMgr::Init: pMu is NULL.\n");
        return -1;
    }
    
    m_pMU = pMu;
    
    return 0;
}
    
void CSegmentPoolMgr::MessageEntry(const MessageFrame &message)
{
    switch (message.option.id())
    {
    case EV_VNETLIB_SEGMENT_RANGE_CFG:
        {
            ProcSegmentRangeMsg(message);
        }
        break;
    default:
        {
            VNET_ASSERT(0);
        }
    }
    
    return ;
}

int32 CSegmentPoolMgr::ProcSegmentRangeMsg(const MessageFrame &message)
{
    if(NULL == m_pMU)
    {
        VNET_ASSERT(0);
        return -1;
    }
    
    CSegmentRangeMsg cMsg;
    cMsg.deserialize(message.data);
    string strRetInfo;
    int32 nRet = 0;
    MessageOption option(message.option.sender(), EV_VNETLIB_SEGMENT_RANGE_CFG, 0, 0);
    
    if(m_bOpenDbgInf)
    {
        cMsg.Print();
    }       
    
    if(!cMsg.IsValidOper())
    {
        VNET_LOG(VNET_LOG_WARN, "CSegmentPoolMgr::ProcSegmentRangeMsg: Msg operation code (%d) is invalid.\n", 
            cMsg.m_dwOper);
        VNetFormatString(strRetInfo, "Msg operation code (%d) is invalid.\n", cMsg.m_dwOper);
        cMsg.SetResult(SEGMENT_RANGE_CFG_PARAM_ERROR);
        cMsg.SetResultinfo(strRetInfo);
        m_pMU->Send(cMsg, option);
        return -1;
    }
    
    switch(cMsg.m_dwOper)
    {
    case EN_ADD_SEGMENT_RANGE:
        {
            nRet = AddSegmentRange(cMsg, strRetInfo);
        }
        break;
    case EN_DEL_SEGMENT_RANGE:
        {
            nRet = DelSegmentRange(cMsg, strRetInfo);
        }
        break;
    default:
        {
            VNET_ASSERT(0);
            strRetInfo = "The Segment range config param is invalid.";
            nRet = SEGMENT_RANGE_CFG_PARAM_ERROR;
            
        }
    }
    
    cMsg.SetResult(nRet);
    cMsg.SetResultinfo(strRetInfo);
    m_pMU->Send(cMsg, option);
    
    return 0;
}

int32 CSegmentPoolMgr::IsConflictWithShareNetSegment(const CSegmentRangeMsg &cMsg, int32 &nConflictSegment)
{
    CDBOperateNetplaneSegmentPool * pOper = GetDbINetplaneSegmentPool();
    if(NULL == pOper)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSegmentPoolMgr::IsConflictWithShareNetAccessSegment: GetDbINetplaneSegmentPool() is NULL.\n");            
        return 1;
    }
    vector<int32> outVInfo;
    int32 nRet = pOper->QueryShareNetSegment(cMsg.m_strNetplaneUUID.c_str(), outVInfo);
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, 
            "CSegmentPoolMgr::IsConflictWithShareNetSegment: DB Query Netplane's(UUID: %s) share Segment failed, ret = %d.\n",
            cMsg.m_strNetplaneUUID.c_str(), nRet);
        return 1;
    } 
    for(vector<int32>::iterator itr = outVInfo.begin(); itr != outVInfo.end(); ++itr)
    {
        if((*itr) >= cMsg.m_nSegmentStart && (*itr) <= cMsg.m_nSegmentEnd)
        {
            nConflictSegment = *itr;
            return 1;
        }
    }
    return 0;
}
#if 0
int32 CSegmentPoolMgr::IsConflictWithShareNetTrunkSegment(const CSegmentRangeMsg &cMsg, 
                                                    int32 &nConflictSegmentStart, 
                                                    int32 &nConflictSegmentEnd)
{
    CDBOperateNetplaneSegmentPool * pOper = GetDbINetplaneSegmentPool();
    if(NULL == pOper)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSegmentPoolMgr::IsConflictWithShareNetTrunkSegment: GetDbINetplaneSegmentPool() is NULL.\n");            
        return 1;
    }
    
    vector<CDbNetplaneTrunkSegment> outVInfo;
    int32 nRet = pOper->QueryShareNetTrunkSegment(cMsg.m_strNetplaneUUID.c_str(), outVInfo);
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, 
            "CSegmentPoolMgr::IsConflictWithShareNetTrunkSegment: DB Query Netplane's(UUID: %s) share access Segment failed, ret = %d.\n",
            cMsg.m_strNetplaneUUID.c_str(), nRet);
        return 1;
    } 
    
    for(vector<CDbNetplaneTrunkSegment>::iterator itr = outVInfo.begin(); itr != outVInfo.end(); ++itr)
    {
        int32 nSegmentBegin = itr->GetSegmentBegin();
        int32 nSegmentEnd = itr->GetSegmentEnd();
        if(cMsg.m_nSegmentEnd >= nSegmentBegin && cMsg.m_nSegmentStart <= nSegmentEnd)
        {
            nConflictSegmentStart = nSegmentBegin;
            nConflictSegmentEnd = nSegmentEnd;
            return 1;
        }
    }
    
    return 0;
}
#endif
int32 CSegmentPoolMgr::GetIsolateNo(const CSegmentRangeMsg &cMsg, vector<int32> &vecIsolateNo)                        
{
    CDBOperateNetplaneSegmentPool * pOper = GetDbINetplaneSegmentPool();
    if(NULL == pOper)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSegmentPoolMgr::GetIsolateNo: GetDbINetplaneSegmentPool() is NULL.\n");            
        return -1;
    }
    
    int32 nRet = pOper->QueryPrivateNetIsolateNo(cMsg.m_strNetplaneUUID.c_str(), vecIsolateNo);
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, 
            "CSegmentPoolMgr::GetIsolateNo: DB Query Netplane's(UUID: %s) share access Segment failed, ret = %d.\n",
            cMsg.m_strNetplaneUUID.c_str(), nRet);
        return -1;
    } 
    
    return 0;
}

int32 CSegmentPoolMgr::AddSegmentRange(const CSegmentRangeMsg &cMsg, string &strRetInfo)
{
    if(cMsg.m_strNetplaneUUID.empty())
    {
        VNET_ASSERT(0);
        strRetInfo = "The Segment range config param is invalid.";
        return SEGMENT_RANGE_CFG_PARAM_ERROR;
    }

    if(!cMsg.IsValidRange())
    {            
        VNET_LOG(VNET_LOG_WARN, "CSegmentPoolMgr::AddSegmentRange: The Segment range(%d-%d) is invalid.\n", 
            cMsg.m_nSegmentStart, cMsg.m_nSegmentEnd);
        VNetFormatString(strRetInfo, "The Segment range(%d-%d) is invalid.\n", 
            cMsg.m_nSegmentStart, cMsg.m_nSegmentEnd);
        return SEGMENT_RANGE_CFG_PARAM_ERROR;
    }

    int32 nSegment = 0;
    if(IsConflictWithShareNetSegment(cMsg, nSegment))
    {
        VNET_LOG(VNET_LOG_WARN, "CSegmentPoolMgr::AddSegmentRange: The Segment range(%d-%d) is in conflict with share net Segment(%d).\n", 
            cMsg.m_nSegmentStart, cMsg.m_nSegmentEnd, nSegment);
        VNetFormatString(strRetInfo, "The Segment range(%d-%d) is in conflict with share net access Segment(%d).\n", 
            cMsg.m_nSegmentStart, cMsg.m_nSegmentEnd, nSegment);
        return SEGMENT_RANGE_CFG_PARAM_ERROR;
    }

    #if 0
    int32 nTrunkSegmentStart = 0;
    int32 nTrunkSegmentEnd = 0;        
    if(IsConflictWithShareNetTrunkSegment(cMsg, nTrunkSegmentStart, nTrunkSegmentEnd))
    {
        VNET_LOG(VNET_LOG_WARN, "CSegmentPoolMgr::AddSegmentRange: The Segment range(%d-%d) is in conflict with share net trunk Segment(%d-%d).\n", 
            cMsg.m_nSegmentStart, cMsg.m_nSegmentEnd, nTrunkSegmentStart, nTrunkSegmentEnd);
        VNetFormatString(strRetInfo, "The Segment range(%d-%d) is in conflict with share net trunk Segment(%d-%d).\n", 
            cMsg.m_nSegmentStart, cMsg.m_nSegmentEnd, nTrunkSegmentStart, nTrunkSegmentEnd);
        return SEGMENT_RANGE_CFG_PARAM_ERROR;
    }
    #endif
    
    // DB range Load;
    CDBOperateNetplaneSegmentRange *pOper = GetDbINetplaneSegmentRange();
    if(NULL == pOper)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSegmentPoolMgr::AddSegmentRange: GetDbINetplaneSegmentRange() is NULL.\n");            
        strRetInfo = "The Segment range database interface is NULL.";
        return SEGMENT_RANGE_GET_DB_INTF_ERROR;
    }
    vector<CDbNetplaneSegmentRange> outVInfo;
    int32 nRet = pOper->Query(cMsg.m_strNetplaneUUID.c_str(), outVInfo);
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, 
            "CSegmentPoolMgr::AddSegmentRange: DB Query Netplane's(UUID: %s) Segment range failed, ret = %d.\n",
            cMsg.m_strNetplaneUUID.c_str(), nRet);
        VNetFormatString(strRetInfo, "Get the net plane's(UUID: %s) Segment range failed.\n",
            cMsg.m_strNetplaneUUID.c_str());
        return nRet;
    }         
    CSegmentRange cSegmentRange;
    CDbNetplaneSegmentRange cDbSegmentRange;
    vector<CDbNetplaneSegmentRange>::iterator itrDBRange;
    if(!outVInfo.empty())
    {
        for(itrDBRange = outVInfo.begin(); itrDBRange != outVInfo.end(); ++itrDBRange)
        {
            TSegmentRangeItem tItem;
            tItem.dwSegmentIDStart = itrDBRange->GetBeginNum();
            tItem.dwSegmentIDEnd = itrDBRange->GetEndNum();
            cSegmentRange.AddValidRange(tItem);
        }
    }
    else
    {// only one record, save it to DB;
        cDbSegmentRange.SetUuid(cMsg.m_strNetplaneUUID.c_str());
        cDbSegmentRange.SetBeginNum(cMsg.m_nSegmentStart);
        cDbSegmentRange.SetEndNum(cMsg.m_nSegmentEnd);
        nRet = pOper->Add(cDbSegmentRange);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CSegmentPoolMgr::AddSegmentRange: DB Add Netplane's(UUID: %s) Segment range[%d-%d] failed, nRet = %d.\n",
                cMsg.m_strNetplaneUUID.c_str(),
                cMsg.m_nSegmentStart,
                cMsg.m_nSegmentEnd,
                nRet);
            VNetFormatString(strRetInfo, "Add the net plane's(UUID: %s) Segment range[%d-%d] to the database failed.\n",
                cMsg.m_strNetplaneUUID.c_str(),
                cMsg.m_nSegmentStart,
                cMsg.m_nSegmentEnd);
            return nRet;
        } 
        return SEGMENT_RANGE_OPER_SUCCESS;
    }      
    // range add;
    TSegmentRangeItem tNewItem;
    tNewItem.dwSegmentIDStart = cMsg.m_nSegmentStart;
    tNewItem.dwSegmentIDEnd = cMsg.m_nSegmentEnd;
    if(0 != cSegmentRange.AddRange(tNewItem))
    {            
        VNET_LOG(VNET_LOG_ERROR, 
            "CSegmentPoolMgr::AddSegmentRange: Segment range[%d-%d] calculate failed.\n",
            cMsg.m_nSegmentStart, 
            cMsg.m_nSegmentEnd);
        VNetFormatString(strRetInfo, "Combining the calculation of Segment range[%d-%d] failed.\n",
            cMsg.m_nSegmentStart,
            cMsg.m_nSegmentEnd);
        return SEGMENT_RANGE_CALC_ERROR;
    }

    // DB Compare and save;Need Transaction;
    vector<TSegmentRangeItem> &vecCalcRange = cSegmentRange.GetSegmentRange();
    vector<TSegmentRangeItem>::iterator itrCalcRange = vecCalcRange.begin();
    for(; itrCalcRange != vecCalcRange.end(); ++itrCalcRange)
    {            
        int32 bFind = 0;
        for(itrDBRange = outVInfo.begin(); itrDBRange != outVInfo.end(); ++itrDBRange)
        {
            TSegmentRangeItem tIPRange;
            tIPRange.dwSegmentIDStart = itrDBRange->GetBeginNum();
            tIPRange.dwSegmentIDEnd = itrDBRange->GetEndNum();
            if(tIPRange.dwSegmentIDStart == itrCalcRange->dwSegmentIDStart &&
               tIPRange.dwSegmentIDEnd == itrCalcRange->dwSegmentIDEnd)
            {
                bFind = 1;
                outVInfo.erase(itrDBRange);
                break;
            }
        }
        
        if(0 == bFind)
        {                
            // Add To DB
            cDbSegmentRange.SetUuid(cMsg.m_strNetplaneUUID.c_str());
            cDbSegmentRange.SetBeginNum(itrCalcRange->dwSegmentIDStart);
            cDbSegmentRange.SetEndNum(itrCalcRange->dwSegmentIDEnd);
            nRet = pOper->Add(cDbSegmentRange);
            if(VNET_DB_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, 
                    "CSegmentPoolMgr::AddSegmentRange: DB Merge Netplane's(UUID: %s) Segment range[%d-%d] failed, nRet = %d.\n",
                    cMsg.m_strNetplaneUUID.c_str(),
                    cMsg.m_nSegmentStart, 
                    cMsg.m_nSegmentEnd,
                    nRet);
                VNetFormatString(strRetInfo, "Add the net plane's(UUID: %s) Segment range[%d-%d] to the database failed.\n",
                    cMsg.m_strNetplaneUUID.c_str(),
                    cMsg.m_nSegmentStart, 
                    cMsg.m_nSegmentEnd);
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
                "CSegmentPoolMgr::AddSegmentRange: DB Merge Netplane's(UUID: %s) Segment range[%d-%d] failed, nRet = %d.\n",
                cMsg.m_strNetplaneUUID.c_str(),
                cMsg.m_nSegmentStart,
                cMsg.m_nSegmentEnd,
                nRet);
            VNetFormatString(strRetInfo, "Combining the calculation of net plane's(UUID: %s) Segment range[%d-%d] to the database failed.\n",
                cMsg.m_strNetplaneUUID.c_str(),
                cMsg.m_nSegmentStart, 
                cMsg.m_nSegmentEnd);
            return nRet;
        }             
    }

    return SEGMENT_RANGE_OPER_SUCCESS;
}

int32 CSegmentPoolMgr::DelSegmentRange(const CSegmentRangeMsg &cMsg, string &strRetInfo)
{
    if(cMsg.m_strNetplaneUUID.empty())
    {
        VNET_ASSERT(0);
        strRetInfo = "The Segment range config param is invalid.";
        return SEGMENT_RANGE_CFG_PARAM_ERROR;
    }

    if(!cMsg.IsValidRange())
    {            
        VNET_LOG(VNET_LOG_WARN, "CSegmentPoolMgr::DelSegmentRange: The Segment range(%d-%d) is invalid.\n", 
            cMsg.m_nSegmentStart, cMsg.m_nSegmentEnd);
        VNetFormatString(strRetInfo, "The Segment range(%d-%d) is invalid.\n",
            cMsg.m_nSegmentStart, cMsg.m_nSegmentEnd);
        return SEGMENT_RANGE_CFG_PARAM_ERROR;
    }
    
    // DB range Load;
    CDBOperateNetplaneSegmentRange *pOper = GetDbINetplaneSegmentRange();
    if(NULL == pOper)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSegmentPoolMgr::DelSegmentRange: GetDbINetplaneSegmentRange() is NULL.\n");
        strRetInfo = "The Segment range database interface is NULL.";
        return SEGMENT_RANGE_GET_DB_INTF_ERROR;
    }
    vector<CDbNetplaneSegmentRange> outVInfo;
    int32 nRet = pOper->Query(cMsg.m_strNetplaneUUID.c_str(), outVInfo);
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, 
            "CSegmentPoolMgr::DelSegmentRange: DB Query Netplane's(UUID: %s) Segment range failed, ret: %d.\n",
            cMsg.m_strNetplaneUUID.c_str(), nRet);
        VNetFormatString(strRetInfo, "Get the net plane's(UUID: %s) Segment range failed.\n",
            cMsg.m_strNetplaneUUID.c_str());
        return nRet;
    }         
    CSegmentRange cSegmentRange;
    CDbNetplaneSegmentRange cDbIPRange;
    vector<CDbNetplaneSegmentRange>::iterator itrDBRange;
    if(outVInfo.empty())
    {
        VNET_LOG(VNET_LOG_ERROR, 
            "CSegmentPoolMgr::DelSegmentRange: Netplane's(UUID: %s) Segment range(%d-%d) is not exist.\n",
            cMsg.m_strNetplaneUUID.c_str(),
            cMsg.m_nSegmentStart,
            cMsg.m_nSegmentEnd);
        VNetFormatString(strRetInfo, "The net plane's(UUID: %s) Segment range is not configured.\n",
            cMsg.m_strNetplaneUUID.c_str());
        return SEGMENT_RANGE_NOT_EXIST;
    }
    
    // Check the range is used by Segment pool or not.
    int32 nOccNum = 0;
    nRet = pOper->QueryAllocNum(cMsg.m_strNetplaneUUID.c_str(), 
                         cMsg.m_nSegmentStart,
                         cMsg.m_nSegmentEnd,
                         nOccNum);
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, 
            "CSegmentPoolMgr::DelSegmentRange: call QueryAllocNum (%s, %d, %d) failed, ret = %d.\n",
            cMsg.m_strNetplaneUUID.c_str(),
            cMsg.m_nSegmentStart,
            cMsg.m_nSegmentEnd,
            nRet);
        VNetFormatString(strRetInfo, "Query the net plane's(UUID: %s) Segment range used state failed.\n",
            cMsg.m_strNetplaneUUID.c_str());
        return nRet;
    }
    if(0 != nOccNum)
    {
        VNET_LOG(VNET_LOG_ERROR, 
            "CSegmentPoolMgr::DelSegmentRange: The net plane's(UUID: %s) Segment range[%d-%d] is occupied,num:%d.\n",
            cMsg.m_strNetplaneUUID.c_str(),
            cMsg.m_nSegmentStart,
            cMsg.m_nSegmentEnd,
            nOccNum);
        VNetFormatString(strRetInfo, "The net plane's(UUID: %s) Segment range[%d-%d] is used.\n",
            cMsg.m_strNetplaneUUID.c_str(),
            cMsg.m_nSegmentStart,
            cMsg.m_nSegmentEnd);
        return SEGMENT_RANGE_OCCUPIED;
    }        

    for(itrDBRange = outVInfo.begin(); itrDBRange != outVInfo.end(); ++itrDBRange)
    {
        TSegmentRangeItem tItem;
        tItem.dwSegmentIDStart = itrDBRange->GetBeginNum();
        tItem.dwSegmentIDEnd = itrDBRange->GetEndNum();
        cSegmentRange.AddValidRange(tItem);
    }
 
    // range del;
    TSegmentRangeItem tNewItem;
    tNewItem.dwSegmentIDStart = cMsg.m_nSegmentStart;
    tNewItem.dwSegmentIDEnd = cMsg.m_nSegmentEnd;
    if(0 != cSegmentRange.DelRange(tNewItem))
    {            
        VNET_LOG(VNET_LOG_ERROR, 
            "CSegmentPoolMgr::DelSegmentRange: Segment range[%d-%d] calculate failed.\n",
            cMsg.m_nSegmentStart, 
            cMsg.m_nSegmentEnd);
        VNetFormatString(strRetInfo, "Combining the calculation of Segment range[%d-%d] failed.\n",
            cMsg.m_nSegmentStart,
            cMsg.m_nSegmentEnd);
        return SEGMENT_RANGE_CALC_ERROR;
    }

    // 如果存在虚拟机网络私有类型端口组，需要限制其不能为空范围;        
    vector<TSegmentRangeItem> &vecCalcRange = cSegmentRange.GetSegmentRange();
    vector<TSegmentRangeItem>::iterator itrCalcRange = vecCalcRange.begin();
    if(vecCalcRange.empty())
    {
        vector<int32> vecIsolateNo;
        nRet = GetIsolateNo(cMsg, vecIsolateNo);
        if(nRet != 0)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CSegmentPoolMgr::DelSegmentRange: Get net plane(%s) private net isolate no list failed.\n",
                cMsg.m_strNetplaneUUID.c_str());
            VNetFormatString(strRetInfo, "Get net plane(%s) private net isolate no list failed.\n",
                cMsg.m_strNetplaneUUID.c_str());
            return SEGMENT_RANGE_CALC_ERROR;
        }
        if(!vecIsolateNo.empty())
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CSegmentPoolMgr::DelSegmentRange: The net plane's(UUID: %s) Segment range[%d-%d] is occupied.\n",
                cMsg.m_strNetplaneUUID.c_str(),
                cMsg.m_nSegmentStart, 
                cMsg.m_nSegmentEnd);
            VNetFormatString(strRetInfo, "The net plane's(UUID: %s) Segment range[%d-%d] is used.\n",
                cMsg.m_strNetplaneUUID.c_str(),
                cMsg.m_nSegmentStart,
                cMsg.m_nSegmentEnd);
            return SEGMENT_RANGE_OCCUPIED;
        }
    }
    // DB Compare and save;Need Transaction;
    for(; itrCalcRange != vecCalcRange.end(); ++itrCalcRange)
    {            
        int32 bFind = 0;
        for(itrDBRange = outVInfo.begin(); itrDBRange != outVInfo.end(); ++itrDBRange)
        {
            TSegmentRangeItem tIPRange;
            tIPRange.dwSegmentIDStart = itrDBRange->GetBeginNum();
            tIPRange.dwSegmentIDEnd = itrDBRange->GetEndNum();
            if(tIPRange.dwSegmentIDStart == itrCalcRange->dwSegmentIDStart &&
               tIPRange.dwSegmentIDEnd == itrCalcRange->dwSegmentIDEnd)
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
            cDbIPRange.SetBeginNum(itrCalcRange->dwSegmentIDStart);
            cDbIPRange.SetEndNum(itrCalcRange->dwSegmentIDEnd);
            nRet = pOper->Add(cDbIPRange);
            if(VNET_DB_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, 
                    "CSegmentPoolMgr::AddSegmentRange: DB Merge Netplane's(UUID: %s) Segment range[%d-%d] failed, ret = %d.\n",
                    cMsg.m_strNetplaneUUID.c_str(),
                    cMsg.m_nSegmentStart, 
                    cMsg.m_nSegmentEnd,
                    nRet);
                VNetFormatString(strRetInfo, "Merge the net plane's(UUID: %s) Segment range[%d-%d] to the data base failed.\n",
                    cMsg.m_nSegmentStart,
                    cMsg.m_nSegmentEnd);
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
                "CSegmentPoolMgr::DelSegmentRange: Netplane's(UUID: %s) Segment range(%d-%d) is not exist, ret = %d.\n",
                cMsg.m_strNetplaneUUID.c_str(),
                cMsg.m_nSegmentStart,
                cMsg.m_nSegmentEnd,
                nRet);
            VNetFormatString(strRetInfo, "Combining the calculation of net plane's(UUID: %s) Segment range[%d-%d] to the data base failed.\n",
                cMsg.m_strNetplaneUUID.c_str(),
                cMsg.m_nSegmentStart,
                cMsg.m_nSegmentEnd);
            return nRet;  
        }           
    }
    return SEGMENT_RANGE_OPER_SUCCESS;
} 

int32 CSegmentPoolMgr::GetSegmentRange(const string &cstrNetplaneID, CSegmentRange &cSegmentRange)
{
    if(cstrNetplaneID.empty())
    {
        VNET_LOG(VNET_LOG_ERROR, 
            "CSegmentPoolMgr::GetSegmentRange: Netplane is NULL.\n");
        return -1;
    }
    
    CDBOperateNetplaneSegmentRange *pOper = GetDbINetplaneSegmentRange();
    if(NULL == pOper)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSegmentPoolMgr::GetSegmentRange: GetDbINetplaneSegmentRange() is NULL.\n");
        return -1;
    }
    vector<CDbNetplaneSegmentRange> outVInfo;
    int32 nRet = pOper->Query(cstrNetplaneID.c_str(), outVInfo);
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, 
            "CSegmentPoolMgr::GetSegmentRange: DB Query Netplane's(UUID: %s) Segment range failed, ret: %d.\n",
            cstrNetplaneID.c_str(), nRet);
        return nRet;
    }         
    CDbNetplaneSegmentRange cDbIPRange;
    vector<CDbNetplaneSegmentRange>::iterator itrDBRange;
    if(outVInfo.empty())
    {
        VNET_LOG(VNET_LOG_ERROR, 
            "CSegmentPoolMgr::DelSegmentRange: Netplane's(UUID: %s) Segment range is not exist.\n",
            cstrNetplaneID.c_str());
        return -1;
    }

    for(itrDBRange = outVInfo.begin(); itrDBRange != outVInfo.end(); ++itrDBRange)
    {
        TSegmentRangeItem tItem;
        tItem.dwSegmentIDStart = itrDBRange->GetBeginNum();
        tItem.dwSegmentIDEnd = itrDBRange->GetEndNum();
        cSegmentRange.AddValidRange(tItem);
    }

    return 0;
}

int32 CSegmentPoolMgr::DbgShowSegmentPool(const string &cstrLogicNetworkID)
{
    return 0;
}

void VNetDbgTestNPSegmentRangeOper(const char* cstrNetplaneUUID, 
                             int32 nSegmentStart, 
                             int32 nSegmentEnd, 
                             uint32 dwOper)
{
    if(NULL == cstrNetplaneUUID) 
    {
        cout << "Input param error." << endl;
        return ;
    }

    if((dwOper <= EN_INVALID_SEGMENT_RANGE_OPER) || (dwOper >= EN_SEGMENT_RANGE_OPER_ALL))
    {
        cout << "Operation Code: Add[" << EN_ADD_SEGMENT_RANGE
             << "], Del[" << EN_DEL_SEGMENT_RANGE
             << "]." <<endl;
        return ;
    }

    MessageUnit tempmu(TempUnitName("TestNPSegmentRangeOper"));
    tempmu.Register();
    CSegmentRangeMsg cMsg;
    string strTmp;
    strTmp.assign(cstrNetplaneUUID);
    cMsg.m_strNetplaneUUID = strTmp;
    cMsg.m_nSegmentStart = nSegmentStart;
    cMsg.m_nSegmentEnd = nSegmentEnd;
    cMsg.m_dwOper = dwOper;
    MessageOption option(MU_VNM, EV_VNETLIB_SEGMENT_RANGE_CFG, 0, 0);
    tempmu.Send(cMsg,option);
    
    return ;
}
DEFINE_DEBUG_FUNC(VNetDbgTestNPSegmentRangeOper);

void VNetDbgTestAllocSegment(int32 nProjID, const char *cstrLogicNetworkID)
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

    CSegmentPoolMgr *pSegmentMgr = CSegmentPoolMgr::GetInstance();
    if(NULL == pSegmentMgr)
    {
        cout << "No Segment pool instance." << endl;
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
    pSegmentMgr->AllocSegmentForVnic(cVNetVmMem);
    return ;
}
DEFINE_DEBUG_FUNC(VNetDbgTestAllocSegment);

void VNetDbgTestFreeSegment(int32 nProjID, const char *cstrLogicNetworkID)
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

    CSegmentPoolMgr *pSegmentMgr = CSegmentPoolMgr::GetInstance();
    if(NULL == pSegmentMgr)
    {
        cout << "No Segment pool instance." << endl;
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
    pSegmentMgr->FreeSegmentForVnic(cVNetVmMem);

    return ;
}
DEFINE_DEBUG_FUNC(VNetDbgTestFreeSegment);

void VNetDbgShowSegmentPool(const char *cstrLogiNetUUID)
{
    if(NULL == cstrLogiNetUUID)
    {
        cout << "Input param error." << endl;
        return ;
    }
    
    CSegmentPoolMgr *pMgr = CSegmentPoolMgr::GetInstance();
    if(pMgr)
    {
        string strTmp;
        strTmp.assign(cstrLogiNetUUID);
        pMgr->DbgShowSegmentPool(strTmp);
    }
    
    return ;
}
DEFINE_DEBUG_FUNC(VNetDbgShowSegmentPool);  

void VNetDbgSetSegmentPoolPrint(BOOL bOpen)
{
    CSegmentPoolMgr *pMgr = CSegmentPoolMgr::GetInstance();
    if(pMgr)
    {
        pMgr->SetDbgInfFlg(bOpen);
    }
    
    return ;
}
DEFINE_DEBUG_FUNC(VNetDbgSetSegmentPoolPrint);    

}// end namespace zte_tecs

