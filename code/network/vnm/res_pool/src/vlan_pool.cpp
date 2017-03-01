/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vlan_pool.cpp
* 文件标识：
* 内容摘要：CVlanPool类实现文件
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

#include "vnet_db_error.h"
#include "vnet_db_proc.h"
#include "vnet_db_mgr.h"
#include "vnet_db_netplane_vlanpool.h"
#include "vnet_db_vlan_map.h"

#include "vlan_range.h"
#include "vlan_pool.h"

#define VLAN_POOL_SIZE  (4096)

namespace zte_tecs
{
    CVlanPool::CVlanPool()
    {
        m_pVlanRange = NULL;
    }

    CVlanPool::~CVlanPool()
    {
        m_vecVlanStatus.clear();
        m_pVlanRange = NULL;
    }

    
    int32 CVlanPool::Init(const string &cstrNetplaneID)
    {
        if(cstrNetplaneID.empty())
        {
            VNET_LOG(VNET_LOG_ERROR, "CVlanPool::Init: The netplane ID is empty.\n");
            return -1;
        }
        m_strNetplaneID = cstrNetplaneID;
        return 0;
    }
    
    int32 CVlanPool::Init(CVlanRange *pVlanRange, const string &cstrNetplaneID)
    {
        if(NULL == pVlanRange)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVlanPool::Init: Vlan range is NULL.\n");            
            VNET_ASSERT(0);            
            return -1;
        }
        
        if(cstrNetplaneID.empty())
        {
            VNET_LOG(VNET_LOG_ERROR, "CVlanPool::Init: The netplane ID is empty.\n");
            return -1;
        }

        m_pVlanRange = pVlanRange;
        m_strNetplaneID = cstrNetplaneID;

        CDBOperateNetplaneVlanPool *pOperVlanPool = GetDbINetplaneVlanPool();
        if(NULL == pOperVlanPool)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVlanPool::Init: CDBOperateNetplaneVlanPool is NULL.\n");
            return -1;
        }

        // Load vlan pool data;
        vector<CDbNetplaneVlanPool> vecVlanPool;
        int32 nRet = pOperVlanPool->Query(m_strNetplaneID.c_str(), vecVlanPool);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CVlanPool::Init: DB Query Netplane's(UUID: %s) vlan range failed.ret: %d.\n",
                m_strNetplaneID.c_str(), nRet);
            return -1;
        } 
        
        m_vecVlanStatus.assign(VLAN_POOL_SIZE, VNET_VLAN_STATUS_UNUSABLE);
        vector<TVlanRangeItem> &vecVlanRange = m_pVlanRange->GetVlanRange();
        vector<TVlanRangeItem>::iterator itr = vecVlanRange.begin();
        for(; itr != vecVlanRange.end(); ++itr)
        {
            for(uint32 i = itr->dwVlanIDStart; i <= itr->dwVlanIDEnd; i++)
            {
                m_vecVlanStatus[i] = VNET_VLAN_STATUS_USABLE;
            }
        }
        // Query db, m_vecVlanStatus[i] = VNET_VLAN_STATUS_USED;
        vector<CDbNetplaneVlanPool>::iterator itrDBVlan = vecVlanPool.begin();
        for(; itrDBVlan != vecVlanPool.end(); ++itrDBVlan)
        {
            m_vecVlanStatus[itrDBVlan->GetVlanNum()] = VNET_VLAN_STATUS_USED;
        }
        
        return 0;
    }

    int32 CVlanPool::AllocVlanID(const string &cstrNetplaneID,
                                 int64 nProjID, 
                                 const string &cstrPortGroupID,                                 
                                 int32 nIsolationNo, 
                                 uint32 &dwAllocVlanID)
    {
        if(NULL == m_pVlanRange || m_strNetplaneID.empty())
        {
            VNET_LOG(VNET_LOG_ERROR, "CVlanPool::Init: Pool is not inited.\n");            
            return -1;
        }
        
        if(cstrPortGroupID.empty())
        {
            VNET_LOG(VNET_LOG_ERROR, "CVlanPool::AllocVlanID: cstrPortGroupID is empty.\n");
            return -1;
        }
        
        if(cstrNetplaneID.empty())
        {
            VNET_LOG(VNET_LOG_ERROR, "CVlanPool::AllocVlanID: cstrNetplaneID is empty.\n");
            return -1;
        }
        
        // DB Query Vlan Map Record;
        CDBOperateVlanMap *pOperVlanMap = GetDbIVlanMap();
        if(NULL == pOperVlanMap)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVlanPool::AllocVlanID: GetDbIVlanMap is NULL .\n");
            return -1;
        }
        vector<CDbVlanMap> vecVlanMap;
        int32 nRet = pOperVlanMap->QueryByNpProjectIdIsolateNo(cstrNetplaneID.c_str(),
                                                               nProjID,
                                                               nIsolationNo, 
                                                               vecVlanMap);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CVlanPool::Init: DB Query Vlan Map(Proj:%d, PG:%s, NP:%s) failed.ret: %d.\n",
                nProjID, cstrPortGroupID.c_str(), cstrNetplaneID.c_str(), nRet);
            return -1;
        } 
        vector<CDbVlanMap>::iterator itrDBVlanMap = vecVlanMap.begin();
        if(!vecVlanMap.empty())
        {
            dwAllocVlanID = itrDBVlanMap->GetVlanNum();
            // Vlan is mapped; Only operate vlan map table;
            for(; itrDBVlanMap != vecVlanMap.end(); ++itrDBVlanMap)
            {
                if(itrDBVlanMap->GetProjectId() == nProjID &&
                   itrDBVlanMap->GetPgUuid() == cstrPortGroupID &&
                   itrDBVlanMap->GetIsolateNo() == nIsolationNo)
                {
                    // Reallocate CVlan;
                    return 0;
                }         
            }
            // Operate vlan map table;
            nRet = pOperVlanMap->CheckAdd(nProjID, cstrPortGroupID.c_str(), dwAllocVlanID, nIsolationNo); 
            if(VNET_DB_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, 
                    "CVlanPool::AllocVlanID: DB check add Vlan map (Proj:%d, PG:%s, vlan:%d) failed.ret: %d.\n",
                    nProjID, cstrPortGroupID.c_str(), dwAllocVlanID, nRet);
                return -1;
            }
            nRet = pOperVlanMap->Add(nProjID, cstrPortGroupID.c_str(), dwAllocVlanID, nIsolationNo); 
            if(VNET_DB_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, 
                    "CVlanPool::AllocVlanID: DB Add Vlan map (Proj:%d, PG:%s, vlan:%d) failed.ret: %d.\n",
                    nProjID, cstrPortGroupID.c_str(), dwAllocVlanID, nRet);
                return -1;
            } 
            
            return 0;            
        }
        
        // alloc vlan条目        
        CDBOperateNetplaneVlanPool *pOperVlanPool = GetDbINetplaneVlanPool();
        if(NULL == pOperVlanPool)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVlanPool::AllocVlanID: CDBOperateNetplaneVlanPool is NULL.\n");
            return -1;
        }        
        dwAllocVlanID = (uint32)(-1);
        for (int i = VNET_MIN_MAP_VLAN_ID; i <= VNET_MAX_MAP_VLAN_ID; i++)
        {
            if (VNET_VLAN_STATUS_USABLE == m_vecVlanStatus[i])
            {
                dwAllocVlanID = i;
                m_vecVlanStatus[i] = VNET_VLAN_STATUS_USED;
                break;
            }
        }
        
        if((uint32)(-1) == dwAllocVlanID)
        {        
            VNET_LOG(VNET_LOG_ERROR, "CVlanPool::AllocVlanID: no vlan resouce to be allocated.\n");
            return -1;
        }
        // DB存储 需要事务支持;         
        // 必须先增加pool中的记录,然后增加map记录;       
        CDbNetplaneVlanPool cDBVlanPoolItem;
        cDBVlanPoolItem.SetUuid(m_strNetplaneID.c_str());
        cDBVlanPoolItem.SetVlanNum(dwAllocVlanID);
        nRet = pOperVlanPool->CheckAdd(cDBVlanPoolItem);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CVlanPool::AllocVlanID: DB check add Vlan (NP:%s, vlan:%d) failed.ret: %d.\n",
                m_strNetplaneID.c_str(), dwAllocVlanID, nRet);
            return -1;
        } 
        nRet = pOperVlanPool->Add(cDBVlanPoolItem);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CVlanPool::AllocVlanID: DB Add Vlan (NP:%s, vlan:%d) failed.ret: %d.\n",
                m_strNetplaneID.c_str(), dwAllocVlanID, nRet);
            return -1;
        } 
        
        nRet = pOperVlanMap->CheckAdd(nProjID, cstrPortGroupID.c_str(), dwAllocVlanID, nIsolationNo); 
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CVlanPool::AllocVlanID: DB Checkadd the new vlan map (Proj:%d, PG:%s, vlan:%d) failed.ret: %d.\n",
                nProjID, cstrPortGroupID.c_str(), dwAllocVlanID, nRet);
            return -1;
        }
        nRet = pOperVlanMap->Add(nProjID, cstrPortGroupID.c_str(), dwAllocVlanID, nIsolationNo); 
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CVlanPool::AllocVlanID: DB Add the new vlan map (Proj:%d, PG:%s, vlan:%d) failed.ret: %d.\n",
                nProjID, cstrPortGroupID.c_str(), dwAllocVlanID, nRet);
            return -1;
        } 
        return 0;
    }    
    
    int32 CVlanPool::FreeVlanID(int64 nProjID, 
                     const string &cstrPortGroupID, 
                     int32 nIsolationNo,
                     uint32 &dwFreeVlanID)
    {
        if(m_strNetplaneID.empty())
        {
            VNET_LOG(VNET_LOG_ERROR, "CVlanPool::Init: Pool is not inited.\n");            
            return -1;
        }
        
        if(cstrPortGroupID.empty())
        {
            VNET_LOG(VNET_LOG_ERROR, "CVlanPool::FreeVlanID: cstrPortGroupID is empty.\n");
            return -1;
        }
        
        CDBOperateVlanMap *pOperVlanMap = GetDbIVlanMap();
        if(NULL == pOperVlanMap)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVlanPool::FreeVlanID: GetDbIVlanMap is NULL .\n");
            return -1;
        }

        CDBOperateNetplaneVlanPool *pOperVlanPool = GetDbINetplaneVlanPool();
        if(NULL == pOperVlanPool)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVlanPool::FreeVlanID: CDBOperateNetplaneVlanPool is NULL.\n");
            return -1;
        } 
        // Get the vlan num;
        int32 nRet = pOperVlanMap->QueryByProjectIdPGIsolateNo(nProjID, 
                                                               cstrPortGroupID.c_str(), 
                                                               nIsolationNo, 
                                                               (int32 &)dwFreeVlanID);
        if(VNET_DB_SUCCESS != nRet)
        {
            if(VNET_DB_IS_ITEM_NO_EXIST(nRet))
            {
                return 0;
            } 
            VNET_LOG(VNET_LOG_ERROR, 
                "CVlanPool::Init: DB Query Vlan Map(Proj:%d, PG:%s, nIsolationNo: %d) failed.ret: %d.\n",
                nProjID, cstrPortGroupID.c_str(), nIsolationNo, nRet);
            return -1;
        }      
        
        // 必须先删除map记录,然后删除pool中的记录;
        // Check;
        nRet = pOperVlanMap->CheckDel(nProjID, cstrPortGroupID.c_str(), (int32)dwFreeVlanID, nIsolationNo);
        if(VNET_DB_SUCCESS != nRet)
        {
            if(VNET_DB_IS_RESOURCE_USING(nRet) || VNET_DB_IS_ITEM_NO_EXIST(nRet))
            {
                return 0;
            }            
            VNET_LOG(VNET_LOG_ERROR, 
                "CVlanPool::FreeVlanID: DB CheckDel Vlan map (Proj:%d, PG:%s, vlan:%d, nIsoNo: %d) failed.ret: %d.\n",
                nProjID, cstrPortGroupID.c_str(), dwFreeVlanID, nIsolationNo, nRet);
            return -1;
        }         
        // Delete;
        nRet = pOperVlanMap->Del(nProjID, cstrPortGroupID.c_str(), (int32)(dwFreeVlanID), nIsolationNo);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CVlanPool::FreeVlanID: DB Del Vlan map (Proj:%d, PG:%s, vlan:%d) failed.ret: %d.\n",
                nProjID, cstrPortGroupID.c_str(), dwFreeVlanID, nRet);
            return -1;
        }         
        // Check;
        CDbNetplaneVlanPool cDBVlan;
        cDBVlan.SetUuid(m_strNetplaneID.c_str());
        cDBVlan.SetVlanNum((int32)(dwFreeVlanID));        
        nRet = pOperVlanPool->CheckDel(cDBVlan);
        if(VNET_DB_SUCCESS != nRet)
        {
            if(VNET_DB_IS_RESOURCE_USING(nRet) || VNET_DB_IS_ITEM_NO_EXIST(nRet))
            {
                return 0;
            }
            
            VNET_LOG(VNET_LOG_ERROR, 
                "CVlanPool::FreeVlanID: DB CheckDel Vlan map (Proj:%d, PG:%s, vlan:%d) failed.ret: %d.\n",
                nProjID, cstrPortGroupID.c_str(), dwFreeVlanID, nRet);
            return -1;
        } 
        // Delete;
        nRet = pOperVlanPool->Del(cDBVlan);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CVlanPool::FreeVlanID: DB Del Vlan (Proj:%d, Netplane:%s, vlan:%d) failed.ret: %d.\n",
                nProjID, m_strNetplaneID.c_str(), dwFreeVlanID, nRet);
            return -1;
        } 
        
        return 0;
    }    
    
}// end namespace zte_tecs

