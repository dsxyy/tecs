/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：segment_pool.cpp
* 文件标识：
* 内容摘要：CSegmentPool类实现文件
* 当前版本：1.0
* 作    者：
* 完成日期：2013年1月18日
*
* 修改记录1：
*     修改日期：2013/09/05
*     版 本 号：V1.0
*     修 改 人：zhao.jin
*     修改内容：创建
******************************************************************************/

#include "vnet_comm.h"

#include "vnet_db_error.h"
#include "vnet_db_proc.h"
#include "vnet_db_mgr.h"
#include "vnet_db_netplane_segmentpool.h"
#include "vnet_db_segment_map.h"

#include "segment_range.h"
#include "segment_pool.h"

#define SEGMENT_POOL_SIZE  (16000000)

namespace zte_tecs
{
    CSegmentPool::CSegmentPool()
    {
        m_pSegmentRange = NULL;
    }

    CSegmentPool::~CSegmentPool()
    {
        m_vecSegmentStatus.clear();
        m_pSegmentRange = NULL;
    }

    
    int32 CSegmentPool::Init(const string &cstrNetplaneID)
    {
        if(cstrNetplaneID.empty())
        {
            VNET_LOG(VNET_LOG_ERROR, "CSegmentPool::Init: The netplane ID is empty.\n");
            return -1;
        }
        m_strNetplaneID = cstrNetplaneID;
        return 0;
    }
    
    int32 CSegmentPool::Init(CSegmentRange *pSegmentRange, const string &cstrNetplaneID)
    {
        if(NULL == pSegmentRange)
        {
            VNET_LOG(VNET_LOG_ERROR, "CSegmentPool::Init: Segment range is NULL.\n");            
            VNET_ASSERT(0);            
            return -1;
        }
        
        if(cstrNetplaneID.empty())
        {
            VNET_LOG(VNET_LOG_ERROR, "CSegmentPool::Init: The netplane ID is empty.\n");
            return -1;
        }

        m_pSegmentRange = pSegmentRange;
        m_strNetplaneID = cstrNetplaneID;

        CDBOperateNetplaneSegmentPool *pOperSegmentPool = GetDbINetplaneSegmentPool();
        if(NULL == pOperSegmentPool)
        {
            VNET_LOG(VNET_LOG_ERROR, "CSegmentPool::Init: CDBOperateNetplaneSegmentPool is NULL.\n");
            return -1;
        }

        // Load Segment pool data;
        vector<CDbNetplaneSegmentPool> vecSegmentPool;
        int32 nRet = pOperSegmentPool->Query(m_strNetplaneID.c_str(), vecSegmentPool);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CSegmentPool::Init: DB Query Netplane's(UUID: %s) Segment range failed.ret: %d.\n",
                m_strNetplaneID.c_str(), nRet);
            return -1;
        } 
        
        m_vecSegmentStatus.assign(SEGMENT_POOL_SIZE, VNET_SEGMENT_STATUS_UNUSABLE);
        vector<TSegmentRangeItem> &vecSegmentRange = m_pSegmentRange->GetSegmentRange();
        vector<TSegmentRangeItem>::iterator itr = vecSegmentRange.begin();
        for(; itr != vecSegmentRange.end(); ++itr)
        {
            for(uint32 i = itr->dwSegmentIDStart; i <= itr->dwSegmentIDEnd; i++)
            {
                m_vecSegmentStatus[i] = VNET_SEGMENT_STATUS_USABLE;
            }
        }
        // Query db, m_vecSegmentStatus[i] = VNET_Segment_STATUS_USED;
        vector<CDbNetplaneSegmentPool>::iterator itrDBSegment = vecSegmentPool.begin();
        for(; itrDBSegment != vecSegmentPool.end(); ++itrDBSegment)
        {
            m_vecSegmentStatus[itrDBSegment->GetSegmentNum()] = VNET_SEGMENT_STATUS_USED;
        }
        
        return 0;
    }

    int32 CSegmentPool::AllocSegmentID(const string &cstrNetplaneID,
                                 int64 nProjID, 
                                 const string &cstrPortGroupID,                                 
                                 int32 nIsolationNo, 
                                 uint32 &dwAllocSegmentID)
    {
        if(NULL == m_pSegmentRange || m_strNetplaneID.empty())
        {
            VNET_LOG(VNET_LOG_ERROR, "CSegmentPool::Init: Pool is not inited.\n");            
            return -1;
        }
        
        if(cstrPortGroupID.empty())
        {
            VNET_LOG(VNET_LOG_ERROR, "CSegmentPool::AllocSegmentID: cstrPortGroupID is empty.\n");
            return -1;
        }
        
        if(cstrNetplaneID.empty())
        {
            VNET_LOG(VNET_LOG_ERROR, "CSegmentPool::AllocSegmentID: cstrNetplaneID is empty.\n");
            return -1;
        }
        
        // DB Query Segment Map Record;
        CDBOperateSegmentMap *pOperSegmentMap = GetDbISegmentMap();
        if(NULL == pOperSegmentMap)
        {
            VNET_LOG(VNET_LOG_ERROR, "CSegmentPool::AllocSegmentID: GetDbISegmentMap is NULL .\n");
            return -1;
        }
        vector<CDbSegmentMap> vecSegmentMap;
        int32 nRet = pOperSegmentMap->QueryByNpProjectIdIsolateNo(cstrNetplaneID.c_str(),
                                                               nProjID,
                                                               nIsolationNo, 
                                                               vecSegmentMap);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CSegmentPool::Init: DB Query Segment Map(Proj:%d, PG:%s, NP:%s) failed.ret: %d.\n",
                nProjID, cstrPortGroupID.c_str(), cstrNetplaneID.c_str(), nRet);
            return -1;
        } 
        vector<CDbSegmentMap>::iterator itrDBSegmentMap = vecSegmentMap.begin();
        if(!vecSegmentMap.empty())
        {
            dwAllocSegmentID = itrDBSegmentMap->GetSegmentNum();
            // Segment is mapped; Only operate Segment map table;
            for(; itrDBSegmentMap != vecSegmentMap.end(); ++itrDBSegmentMap)
            {
                if(itrDBSegmentMap->GetProjectId() == nProjID &&
                   itrDBSegmentMap->GetPgUuid() == cstrPortGroupID &&
                   itrDBSegmentMap->GetIsolateNo() == nIsolationNo)
                {
                    // Reallocate CSegment;
                    return 0;
                }         
            }
            // Operate Segment map table;
            nRet = pOperSegmentMap->CheckAdd(nProjID, cstrPortGroupID.c_str(), dwAllocSegmentID, nIsolationNo); 
            if(VNET_DB_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, 
                    "CSegmentPool::AllocSegmentID: DB check add Segment map (Proj:%d, PG:%s, Segment:%d) failed.ret: %d.\n",
                    nProjID, cstrPortGroupID.c_str(), dwAllocSegmentID, nRet);
                return -1;
            }
            nRet = pOperSegmentMap->Add(nProjID, cstrPortGroupID.c_str(), dwAllocSegmentID, nIsolationNo); 
            if(VNET_DB_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, 
                    "CSegmentPool::AllocSegmentID: DB Add Segment map (Proj:%d, PG:%s, Segment:%d) failed.ret: %d.\n",
                    nProjID, cstrPortGroupID.c_str(), dwAllocSegmentID, nRet);
                return -1;
            } 
            
            return 0;            
        }
        
        // alloc Segment条目        
        CDBOperateNetplaneSegmentPool *pOperSegmentPool = GetDbINetplaneSegmentPool();
        if(NULL == pOperSegmentPool)
        {
            VNET_LOG(VNET_LOG_ERROR, "CSegmentPool::AllocSegmentID: CDBOperateNetplaneSegmentPool is NULL.\n");
            return -1;
        }        
        dwAllocSegmentID = (uint32)(-1);
        for (int i = VNET_MIN_MAP_SEGMENT_ID; i <= VNET_MAX_MAP_SEGMENT_ID; i++)
        {
            if (VNET_SEGMENT_STATUS_USABLE == m_vecSegmentStatus[i])
            {
                dwAllocSegmentID = i;
                m_vecSegmentStatus[i] = VNET_SEGMENT_STATUS_USED;
                break;
            }
        }
        
        if((uint32)(-1) == dwAllocSegmentID)
        {        
            VNET_LOG(VNET_LOG_ERROR, "CSegmentPool::AllocSegmentID: no Segment resouce to be allocated.\n");
            return -1;
        }
        // DB存储 需要事务支持;         
        // 必须先增加pool中的记录,然后增加map记录;       
        CDbNetplaneSegmentPool cDBSegmentPoolItem;
        cDBSegmentPoolItem.SetUuid(m_strNetplaneID.c_str());
        cDBSegmentPoolItem.SetSegmentNum(dwAllocSegmentID);
        nRet = pOperSegmentPool->CheckAdd(cDBSegmentPoolItem);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CSegmentPool::AllocSegmentID: DB check add Segment (NP:%s, Segment:%d) failed.ret: %d.\n",
                m_strNetplaneID.c_str(), dwAllocSegmentID, nRet);
            return -1;
        } 
        nRet = pOperSegmentPool->Add(cDBSegmentPoolItem);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CSegmentPool::AllocSegmentID: DB Add Segment (NP:%s, Segment:%d) failed.ret: %d.\n",
                m_strNetplaneID.c_str(), dwAllocSegmentID, nRet);
            return -1;
        } 
        
        nRet = pOperSegmentMap->CheckAdd(nProjID, cstrPortGroupID.c_str(), dwAllocSegmentID, nIsolationNo); 
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CSegmentPool::AllocSegmentID: DB Checkadd the new Segment map (Proj:%d, PG:%s, Segment:%d) failed.ret: %d.\n",
                nProjID, cstrPortGroupID.c_str(), dwAllocSegmentID, nRet);
            return -1;
        }
        nRet = pOperSegmentMap->Add(nProjID, cstrPortGroupID.c_str(), dwAllocSegmentID, nIsolationNo); 
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CSegmentPool::AllocSegmentID: DB Add the new Segment map (Proj:%d, PG:%s, Segment:%d) failed.ret: %d.\n",
                nProjID, cstrPortGroupID.c_str(), dwAllocSegmentID, nRet);
            return -1;
        } 
        return 0;
    }    
    
    int32 CSegmentPool::FreeSegmentID(int64 nProjID, 
                     const string &cstrPortGroupID, 
                     int32 nIsolationNo,
                     uint32 &dwFreeSegmentID)
    {
        if(m_strNetplaneID.empty())
        {
            VNET_LOG(VNET_LOG_ERROR, "CSegmentPool::Init: Pool is not inited.\n");            
            return -1;
        }
        
        if(cstrPortGroupID.empty())
        {
            VNET_LOG(VNET_LOG_ERROR, "CSegmentPool::FreeSegmentID: cstrPortGroupID is empty.\n");
            return -1;
        }
        
        CDBOperateSegmentMap *pOperSegmentMap = GetDbISegmentMap();
        if(NULL == pOperSegmentMap)
        {
            VNET_LOG(VNET_LOG_ERROR, "CSegmentPool::FreeSegmentID: GetDbISegmentMap is NULL .\n");
            return -1;
        }

        CDBOperateNetplaneSegmentPool *pOperSegmentPool = GetDbINetplaneSegmentPool();
        if(NULL == pOperSegmentPool)
        {
            VNET_LOG(VNET_LOG_ERROR, "CSegmentPool::FreeSegmentID: CDBOperateNetplaneSegmentPool is NULL.\n");
            return -1;
        } 
        // Get the Segment num;
        int32 nRet = pOperSegmentMap->QueryByProjectIdPGIsolateNo(nProjID, 
                                                               cstrPortGroupID.c_str(), 
                                                               nIsolationNo, 
                                                               (int32 &)dwFreeSegmentID);
        if(VNET_DB_SUCCESS != nRet)
        {
            if(VNET_DB_IS_ITEM_NO_EXIST(nRet))
            {
                return 0;
            } 
            VNET_LOG(VNET_LOG_ERROR, 
                "CSegmentPool::Init: DB Query Segment Map(Proj:%d, PG:%s, nIsolationNo: %d) failed.ret: %d.\n",
                nProjID, cstrPortGroupID.c_str(), nIsolationNo, nRet);
            return -1;
        }      
        
        // 必须先删除map记录,然后删除pool中的记录;
        // Check;
        nRet = pOperSegmentMap->CheckDel(nProjID, cstrPortGroupID.c_str(), (int32)dwFreeSegmentID, nIsolationNo);
        if(VNET_DB_SUCCESS != nRet)
        {
            if(VNET_DB_IS_RESOURCE_USING(nRet) || VNET_DB_IS_ITEM_NO_EXIST(nRet))
            {
                return 0;
            }            
            VNET_LOG(VNET_LOG_ERROR, 
                "CSegmentPool::FreeSegmentID: DB CheckDel Segment map (Proj:%d, PG:%s, Segment:%d, nIsoNo: %d) failed.ret: %d.\n",
                nProjID, cstrPortGroupID.c_str(), dwFreeSegmentID, nIsolationNo, nRet);
            return -1;
        }         
        // Delete;
        nRet = pOperSegmentMap->Del(nProjID, cstrPortGroupID.c_str(), (int32)(dwFreeSegmentID), nIsolationNo);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CSegmentPool::FreeSegmentID: DB Del Segment map (Proj:%d, PG:%s, Segment:%d) failed.ret: %d.\n",
                nProjID, cstrPortGroupID.c_str(), dwFreeSegmentID, nRet);
            return -1;
        }         
        // Check;
        CDbNetplaneSegmentPool cDBSegment;
        cDBSegment.SetUuid(m_strNetplaneID.c_str());
        cDBSegment.SetSegmentNum((int32)(dwFreeSegmentID));        
        nRet = pOperSegmentPool->CheckDel(cDBSegment);
        if(VNET_DB_SUCCESS != nRet)
        {
            if(VNET_DB_IS_RESOURCE_USING(nRet) || VNET_DB_IS_ITEM_NO_EXIST(nRet))
            {
                return 0;
            }
            
            VNET_LOG(VNET_LOG_ERROR, 
                "CSegmentPool::FreeSegmentID: DB CheckDel Segment map (Proj:%d, PG:%s, Segment:%d) failed.ret: %d.\n",
                nProjID, cstrPortGroupID.c_str(), dwFreeSegmentID, nRet);
            return -1;
        } 
        // Delete;
        nRet = pOperSegmentPool->Del(cDBSegment);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CSegmentPool::FreeSegmentID: DB Del Segment (Proj:%d, Netplane:%s, Segment:%d) failed.ret: %d.\n",
                nProjID, m_strNetplaneID.c_str(), dwFreeSegmentID, nRet);
            return -1;
        } 
        
        return 0;
    }    
    
}// end namespace zte_tecs

