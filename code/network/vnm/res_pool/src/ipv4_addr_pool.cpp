/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：ipv4_addr_pool.cpp
* 文件标识：
* 内容摘要：CIPv4AddrPool类实现文件
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
#include "vnet_db_mgr.h"
#include "vnet_db_proc.h"
#include "vnet_db_logicnetwork_ippool.h"
#include "vnet_db_logicnetwork_iprange.h"
#include "vnet_db_netplane_ippool.h"
#include "vnet_db_netplane_iprange.h"

#include "ipv4_addr.h"
#include "ipv4_addr_range.h"
#include "ipv4_addr_pool.h"


namespace zte_tecs
{
static int32 s_nOutputIPDbgInfFlag = 0;

    CIPv4AddrPool::CIPv4AddrPool()
    {
        m_pIPv4AddrRange = NULL;
        m_tNetType = LOGIC_NET;
    }

    CIPv4AddrPool::~CIPv4AddrPool()
    {
        m_pIPv4AddrRange = NULL;
    }
    
    int32 CIPv4AddrPool::Init(const string &cstrNetworkUUID, T_Ipv4NetType tIpv4NetType)
    {
        if(cstrNetworkUUID.empty())
        {
            VNET_LOG(VNET_LOG_ERROR, "CIPv4AddrPool::Init: the LogicNetwork ID is empty!\n");
            return -1;
        }

        m_strNetworkUUID = cstrNetworkUUID;
        m_tNetType       = tIpv4NetType;
        return 0;
    }
    
    int32 CIPv4AddrPool::Init(const string &cstrNetworkUUID, CIPv4AddrRange *pIPv4AddrRange, T_Ipv4NetType tIpv4NetType)
    {
        if(NULL == pIPv4AddrRange)
        {
            VNET_ASSERT(0);
            return -1;
        }
        
        if(cstrNetworkUUID.empty())
        {
            VNET_LOG(VNET_LOG_ERROR, "CIPv4AddrPool::Init: the LogicNetwork ID is empty!\n");
            return -1;
        }
        
        m_pIPv4AddrRange = pIPv4AddrRange;
        m_strNetworkUUID = cstrNetworkUUID;
        m_tNetType       = tIpv4NetType;        
        
        return 0;
    }

    bool CIPv4AddrPool::CompareRange(const TIPv4RangeUsedInfo &tRange1, 
                                     const TIPv4RangeUsedInfo &tRange2)
    {
        /* todo: 
         * 后续可以对区域按照资源分配其他方式排序，
         * 进行资源分配方式可配
         */
        return (tRange1.nFreeCnt > tRange2.nFreeCnt) ? true : false;
    }
    
    int32 CIPv4AddrPool::GetAvailableRange(vector<TIPV4AddrRangeItem> &vecIPv4Range, 
                                      vector<TIPv4RangeUsedInfo> &vecAvailableRange)
    {
        vector<TIPV4AddrRangeItem>::iterator itr = vecIPv4Range.begin();
        uint32 dwStart = 0;
        uint32 dwEnd = 0;
        for(; itr != vecIPv4Range.end(); ++itr)
        {
            uint32 nCnt = 0;

            if(0 != GetIPv4AddrCnt(*itr, nCnt))
            {
                // query failed;
                continue;
            }
            
            dwStart = CIPv4Addr::N2HL(itr->tIPStart.uIP.dwAddr);
            dwEnd = CIPv4Addr::N2HL(itr->tIPEnd.uIP.dwAddr);
            uint32 nTotalCnt = dwEnd - dwStart + 1;
            if(nCnt >= nTotalCnt)
            {
                // no free resources;
                continue;
            }
            
            TIPv4RangeUsedInfo tIPv4RangeUsedInfo;
            tIPv4RangeUsedInfo.nUsedCnt = nCnt;
            tIPv4RangeUsedInfo.nFreeCnt = nTotalCnt - nCnt;
            tIPv4RangeUsedInfo.tRange = *itr;
            vecAvailableRange.push_back(tIPv4RangeUsedInfo);
        }

        if(!vecAvailableRange.empty())
        {
            sort(vecAvailableRange.begin(), vecAvailableRange.end(), CompareRange);
        }

        if(s_nOutputIPDbgInfFlag)
        {
            cout << "-------------------Original Ranges----------------------" << endl;
            for(itr = vecIPv4Range.begin(); itr != vecIPv4Range.end(); ++itr)
            {
                dwStart = CIPv4Addr::N2HL(itr->tIPStart.uIP.dwAddr);
                dwEnd = CIPv4Addr::N2HL(itr->tIPEnd.uIP.dwAddr);
                cout << "MASK:" << hex 
                     << CIPv4Addr::N2HL(itr->tIPStart.uMask.dwAddr) << ",{" 
                     << dwStart << "," 
                     << dwEnd << "},Size:" << dec
                     << dwEnd-dwStart+1 << "."
                     << endl;
            }
            cout << "-------------------Sorted Ranges------------------------" << endl;            
            if(!vecAvailableRange.empty())
            {
                vector<TIPv4RangeUsedInfo>::iterator itr1 = vecAvailableRange.begin();
                for(; itr1 != vecAvailableRange.end(); ++itr1)
                {
                    dwStart = CIPv4Addr::N2HL(itr1->tRange.tIPStart.uIP.dwAddr);
                    dwEnd = CIPv4Addr::N2HL(itr1->tRange.tIPEnd.uIP.dwAddr);
                    cout << "MASK:" << hex 
                         << itr1->tRange.tIPStart.uMask.dwAddr << ",{" 
                         << dwStart << "," 
                         << dwEnd << "},Used:" << dec
                         << itr1->nUsedCnt << ",Free:"
                         << itr1->nFreeCnt << "."
                         << endl;
                }
            }
            else
            {
                cout << "No available IPv4 ranges." << endl;
            }
            cout.unsetf(ios::hex);
        }
        
        return 0;
    }

    class CVecFindIP
    {
    public:
        CVecFindIP(int64 nIPv4Addr, int64 nMask)
        {
            m_nIPv4Addr = nIPv4Addr;
            m_nMask     = nMask;
        }
        ~CVecFindIP(){}
    public:
         bool operator ()(CDbLogicNetworkIpPool &cDBIP)
         {
            if((cDBIP.GetIpNum() == m_nIPv4Addr) && 
               (cDBIP.GetMaskNum() == m_nMask))
            {
                return true;
            }
            return false;     
         }
         bool operator ()(CDbNetplaneIpPool &cDBIP)
         {
            if((cDBIP.GetIpNum() == m_nIPv4Addr) && 
               (cDBIP.GetMaskNum() == m_nMask))
            {
                return true;
            }
            return false;     
         }
         
    private:
        int64 m_nIPv4Addr;
        int64 m_nMask;
    };

    int32 CIPv4AddrPool::ScanAllocIP(const TIPV4AddrRangeItem &tIPRange, TIPv4Addr &tItem)
    {
        VNET_LOG(VNET_LOG_INFO, "CIPv4AddrPool::ScanAllocIP: Free space in IP pool are few. Alloc IP need more time...\n");

        uint32 dwIPBegin = CIPv4Addr::N2HL(tIPRange.tIPStart.uIP.dwAddr);
        uint32 dwIPEnd = CIPv4Addr::N2HL(tIPRange.tIPEnd.uIP.dwAddr);
        uint32 dwMask = CIPv4Addr::N2HL(tIPRange.tIPStart.uMask.dwAddr);

        if (LOGIC_NET == m_tNetType)
        {
            vector<CDbLogicNetworkIpPool> vecIPOut;
            CDBOperateLogicNetworkIpPool *pOper = GetDbILogicNetworkIpPool();
            if(NULL == pOper)
            {
                VNET_LOG(VNET_LOG_ERROR, "CIPv4PoolMgr::ScanAllocIP: GetDbILogicNetworkIpRange() is NULL.\n");
                return -1;
            }        
            int32 nRet = pOper->QueryByLnRange(m_strNetworkUUID.c_str(), (int64)dwIPBegin, (int64)dwIPEnd, vecIPOut);
            if(VNET_DB_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, "CIPv4PoolMgr::ScanAllocIP: call QueryByLnRange(%s, %x, %x) failed, nRet=%d.\n", 
                    m_strNetworkUUID.c_str(),
                    dwIPBegin,
                    dwIPEnd,
                    nRet);
                return -1;
            }

            for(uint32 i = dwIPBegin; i <= dwIPEnd; i++)
            {
                vector<CDbLogicNetworkIpPool>::iterator pos = 
                    find_if(vecIPOut.begin(), vecIPOut.end(), CVecFindIP((int64)i, (int64)dwMask));
                if(pos != vecIPOut.end())
                {
                    // Is allocated.
                    continue;
                }
                else
                {
                    tItem.uIP.dwAddr = CIPv4Addr::H2NL((int32)i);
                    tItem.uMask.dwAddr = tIPRange.tIPStart.uMask.dwAddr;
                    return 0;
                }
            }             
        }
        else
        {
            vector<CDbNetplaneIpPool> vecIPOut;
            CDBOperateNetplaneIpPool *pOper = GetDbINetplaneIpPool();
            if(NULL == pOper)
            {
                VNET_LOG(VNET_LOG_ERROR, "CIPv4PoolMgr::ScanAllocIP: GetDbINetplaneIpPool() is NULL.\n");
                return -1;
            }     
            int32 nRet = pOper->QueryByNpRange(m_strNetworkUUID.c_str(), (int64)dwIPBegin, (int64)dwIPEnd, vecIPOut);
            if(VNET_DB_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, "CIPv4PoolMgr::ScanAllocIP: call QueryByNpRange(%s, %x, %x) failed, nRet=%d.\n", 
                    m_strNetworkUUID.c_str(),
                    dwIPBegin,
                    dwIPEnd,
                    nRet);
                return -1;
            }

            for(uint32 i = dwIPBegin; i <= dwIPEnd; i++)
            {
                vector<CDbNetplaneIpPool>::iterator pos = 
                    find_if(vecIPOut.begin(), vecIPOut.end(), CVecFindIP((int64)i, (int64)dwMask));
                if(pos != vecIPOut.end())
                {
                    // Is allocated.
                    continue;
                }
                else
                {
                    tItem.uIP.dwAddr = CIPv4Addr::H2NL((int32)i);
                    tItem.uMask.dwAddr = tIPRange.tIPStart.uMask.dwAddr;
                    return 0;
                }
            }             
        }

        return -1;

    }
    
    int32 CIPv4AddrPool::AllocIPv4Addr(int32 nKey, TIPv4Addr &tItem)
    {
        if(NULL == m_pIPv4AddrRange)
        {
            VNET_LOG(VNET_LOG_ERROR, "CIPv4AddrPool::AllocIPv4Addr: IP Pool is not initialized, the IP Range is NULL!\n");
            return -1;
        }

        if(m_strNetworkUUID.empty())
        {
            VNET_LOG(VNET_LOG_ERROR, "CIPv4AddrPool::AllocIPv4Addr: IP Pool is not initialized, the LogicNetwork ID is empty!\n");
            return -1;
        }

        vector<TIPv4RangeUsedInfo> vecAvailableRange;
        GetAvailableRange(m_pIPv4AddrRange->GetIPv4AddrRange(), vecAvailableRange);
        if(vecAvailableRange.empty())
        {
            VNET_LOG(VNET_LOG_ERROR, "CIPv4AddrPool::AllocIPv4Addr:No available IPv4 ranges.\n");
            return -1;
        }

        vector<TIPv4RangeUsedInfo>::iterator itr = vecAvailableRange.begin();
        for(; itr != vecAvailableRange.end(); ++itr)
        {
            uint32 dwStart = CIPv4Addr::N2HL(itr->tRange.tIPStart.uIP.dwAddr);
            uint32 dwEnd = CIPv4Addr::N2HL(itr->tRange.tIPEnd.uIP.dwAddr);
            uint32 dwInterval = dwEnd - dwStart + 1;
            TIPv4Addr tIPAddr;
            CalcIPv4Addr(itr->tRange, nKey, tIPAddr);
            int nTryCnt = 0; //每个range尝试15次分配;            
            CIPv4Addr cIPv4Addr;
            while(1)
            {
                // DB存储 包括条目冲突检测分配;
                cIPv4Addr.Set(tIPAddr);
                // DB Save;
                if(0 != SaveIPv4Addr2DB(cIPv4Addr))
                {
                    nTryCnt++;
                    if(nTryCnt > 15)
                    {
                        break;
                    }
                    
                    if(0 == (nTryCnt % 5))
                    {
                        // 增加随机性;
                        CalcIPv4Addr(itr->tRange, nKey, tIPAddr);
                        continue;
                    }
                    
                    tIPAddr.uIP.dwAddr = CIPv4Addr::N2HL(tIPAddr.uIP.dwAddr);
                    if(nTryCnt > 10)
                    {
                        tIPAddr.uIP.dwAddr += 100000;
                        tIPAddr.uIP.dwAddr = (tIPAddr.uIP.dwAddr % dwInterval) + dwStart;
                    }
                    else
                    {
                        tIPAddr.uIP.dwAddr += 1;
                        tIPAddr.uIP.dwAddr = (tIPAddr.uIP.dwAddr % dwInterval) + dwStart;
                    }
                    tIPAddr.uIP.dwAddr = CIPv4Addr::H2NL(tIPAddr.uIP.dwAddr);
                    continue;
                }
                else
                {
                    cIPv4Addr.Get(tItem);
                    return 0;
                }
            }// end while(1)...;

            // 由于冲突尝试失败，只能遍历分配;
            if(0 == ScanAllocIP(itr->tRange, tIPAddr))
            {                
                cIPv4Addr.Set(tIPAddr);
                if(s_nOutputIPDbgInfFlag)
                {
                    VNET_LOG(VNET_LOG_INFO, "CIPv4AddrPool::AllocIPv4Addr: allocated IP[%s] successfullyby scanning mode.\n", 
                        cIPv4Addr.GetIPAndMask().c_str());
                }
                // DB Save;
                if(0 == SaveIPv4Addr2DB(cIPv4Addr))
                {
                    cIPv4Addr.Get(tItem);
                    return 0;
                }
            }
            
        }// end for(; 
        
        return -1;
    }    

    int32 CIPv4AddrPool::CalcIPv4Addr(const TIPV4AddrRangeItem &tIPRange, int32 nKey, TIPv4Addr &tItem)
    {
        BYTE aucUUID[16];
        VNetGetUUIDValue(aucUUID);
        int32 nFactorA = *(int32*)(aucUUID);
        int32 nFactorB = *(int32*)(aucUUID+4);
        int32 nFactorC = *(int32*)(aucUUID+8);
        int16 nNode0   = *(int16*)(aucUUID+12);
        int16 nNode1   = *(int16*)(aucUUID+14);
        
        int32 nFactorD = nNode0 * 102317 
            + nNode1 * 8317 
            + nFactorA * 3331 
            + nFactorB * 2179 
            + nFactorC * 9973 
            + nKey * 5711 
            + 382457; 
            
        uint8 aucNum[4];
        aucNum[0] = (uint8)((nFactorA) % (tIPRange.tIPEnd.uIP.aucAddr[0] - tIPRange.tIPStart.uIP.aucAddr[0] + 1));        
        aucNum[1] = (uint8)((nFactorB) % (tIPRange.tIPEnd.uIP.aucAddr[1] - tIPRange.tIPStart.uIP.aucAddr[1] + 1));
        aucNum[2] = (uint8)((nFactorC) % (tIPRange.tIPEnd.uIP.aucAddr[2] - tIPRange.tIPStart.uIP.aucAddr[2] + 1));
        aucNum[3] = (uint8)((nFactorD) % (tIPRange.tIPEnd.uIP.aucAddr[3] - tIPRange.tIPStart.uIP.aucAddr[3] + 1));

        tItem.uMask.dwAddr = tIPRange.tIPStart.uMask.dwAddr;        
        tItem.uIP.aucAddr[0] = tIPRange.tIPStart.uIP.aucAddr[0] + aucNum[0];
        tItem.uIP.aucAddr[1] = tIPRange.tIPStart.uIP.aucAddr[1] + aucNum[1];
        tItem.uIP.aucAddr[2] = tIPRange.tIPStart.uIP.aucAddr[2] + aucNum[2];
        tItem.uIP.aucAddr[3] = tIPRange.tIPStart.uIP.aucAddr[3] + aucNum[3];
        
        return 0;
    }

    int32 CIPv4AddrPool::GetIPv4AddrCnt(const TIPV4AddrRangeItem &tRange, uint32 &dwCnt)
    {
        if (LOGIC_NET == m_tNetType)
        {
            CDBOperateLogicNetworkIpRange *pOper = GetDbILogicNetworkIpRange();
            
            if(NULL == pOper)
            {
                VNET_LOG(VNET_LOG_ERROR, "CIPv4PoolMgr::GetIPv4AddrCnt: GetDbILogicNetworkIpRange() is NULL.\n");
                return -1;
            }
            int32 nNum = 0;
            int32 nRet = pOper->QueryAllocNum(m_strNetworkUUID.c_str(), 
                                 CIPv4Addr::N2HL(tRange.tIPStart.uIP.dwAddr),
                                 CIPv4Addr::N2HL(tRange.tIPEnd.uIP.dwAddr),
                                 nNum);
            if(VNET_DB_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, 
                    "CIPv4PoolMgr::GetIPv4AddrCnt: call QueryAllocNum (%s, 0x%x, 0x%x) failed, ret:%d.\n",
                    m_strNetworkUUID.c_str(),
                    CIPv4Addr::N2HL(tRange.tIPStart.uIP.dwAddr),
                    CIPv4Addr::N2HL(tRange.tIPEnd.uIP.dwAddr),
                    nRet);
                return nRet;
            }
            dwCnt = (uint32)nNum;
        }
        else
        {
            CDBOperateNetplaneIpRange  *pOper = GetDbINetplaneIpRange();    
            if(NULL == pOper)
            {
                VNET_LOG(VNET_LOG_ERROR, "CIPv4PoolMgr::GetIPv4AddrCnt: GetDbINetplaneIpRange() is NULL.\n");
                return -1;
            }

            /*需要数据库提供接口查询网络平面的iprange*/
            int32 nNum = 0;
            int32 nRet = pOper->QueryAllocNum(m_strNetworkUUID.c_str(), 
                                 CIPv4Addr::N2HL(tRange.tIPStart.uIP.dwAddr),
                                 CIPv4Addr::N2HL(tRange.tIPEnd.uIP.dwAddr),
                                 nNum);
            if(VNET_DB_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, 
                    "CIPv4PoolMgr::GetIPv4AddrCnt: call QueryAllocNum (%s, 0x%x, 0x%x) failed, ret:%d.\n",
                    m_strNetworkUUID.c_str(),
                    CIPv4Addr::N2HL(tRange.tIPStart.uIP.dwAddr),
                    CIPv4Addr::N2HL(tRange.tIPEnd.uIP.dwAddr),
                    nRet);
                return nRet;
            }
            dwCnt = (uint32)nNum; 
            
        }
        return 0;
    }
    
    int32 CIPv4AddrPool::SaveIPv4Addr2DB(const CIPv4Addr &cIPv4Addr)
    {
        if (LOGIC_NET == m_tNetType)
        {
            // Get DB oper;
            CDBOperateLogicNetworkIpPool *pOper = GetDbILogicNetworkIpPool();
            if(NULL == pOper)
            {
                VNET_LOG(VNET_LOG_ERROR, "CIPv4AddrPool::SaveIPv4Addr2DB: GetDbILogicNetworkIpPool() is NULL.\n");
                return -1;
            }

            CDbLogicNetworkIpPool cDBIPAddr;
            cDBIPAddr.SetUuid(m_strNetworkUUID.c_str());        
            cDBIPAddr.SetIpStr(cIPv4Addr.GetIP().c_str());
            cDBIPAddr.SetMaskStr(cIPv4Addr.GetMask().c_str());
            TIPv4Addr tItem;
            cIPv4Addr.Get(tItem);
            cDBIPAddr.SetIpNum((int64)CIPv4Addr::N2HL(tItem.uIP.dwAddr));
            cDBIPAddr.SetMaskNum((int64)CIPv4Addr::N2HL(tItem.uMask.dwAddr));
            int32 nRet = pOper->CheckAdd(cDBIPAddr);
            if(VNET_DB_SUCCESS != nRet)
            {
                if(s_nOutputIPDbgInfFlag)
                {
                    cout << "CIPv4AddrPool::SaveIPv4Addr2DB: check IP(LogicNetwork:" << m_strNetworkUUID
                         << ", IP: " << cIPv4Addr.GetIPAndMask() << ",conflict in DB failed, ret:"
                         << nRet <<"." << endl;
                }
                return -1;
            }
            
            nRet = pOper->Add(cDBIPAddr);
            if(VNET_DB_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_WARN, 
                    "CIPv4AddrPool::SaveIPv4Addr2DB: Add IP(LogicNetwork:%s, IP: %s) to DB failed, ret:%d.\n",
                    m_strNetworkUUID.c_str(),
                    cIPv4Addr.GetIPAndMask().c_str(),
                    nRet);
                return -1;
            }
        }
        else
        {
            
            CDBOperateNetplaneIpPool *pOper = GetDbINetplaneIpPool();
            if(NULL == pOper)
            {
                VNET_LOG(VNET_LOG_ERROR, "CIPv4AddrPool::SaveIPv4Addr2DB: GetDbINetplaneIpPool() is NULL.\n");
                return -1;
            }
           
            CDbNetplaneIpPool cDBIPAddr;
            cDBIPAddr.SetUuid(m_strNetworkUUID.c_str());        
            cDBIPAddr.SetIpStr(cIPv4Addr.GetIP().c_str());
            cDBIPAddr.SetMaskStr(cIPv4Addr.GetMask().c_str());
            TIPv4Addr tItem;
            cIPv4Addr.Get(tItem);
            cDBIPAddr.SetIpNum((int64)CIPv4Addr::N2HL(tItem.uIP.dwAddr));
            cDBIPAddr.SetMaskNum((int64)CIPv4Addr::N2HL(tItem.uMask.dwAddr));
            int32 nRet = pOper->CheckAdd(cDBIPAddr);
            if(VNET_DB_SUCCESS != nRet)
            {
                if(s_nOutputIPDbgInfFlag)
                {
                    cout << "CIPv4AddrPool::SaveIPv4Addr2DB: check IP(LogicNetwork:" << m_strNetworkUUID
                         << ", IP: " << cIPv4Addr.GetIPAndMask() << ",conflict in DB failed, ret:"
                         << nRet <<"." << endl;
                }
                return -1;
            }

            nRet = pOper->Add(cDBIPAddr);
            if(VNET_DB_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_WARN, 
                    "CIPv4AddrPool::SaveIPv4Addr2DB: Add IP(Netplane:%s, IP: %s) to DB failed, ret:%d.\n",
                    m_strNetworkUUID.c_str(),
                    cIPv4Addr.GetIPAndMask().c_str(),
                    nRet);
                return -1;
            }
        }
        
        return 0;        
    }
    
    int32 CIPv4AddrPool::FreeIPv4Addr(const TIPv4Addr &tItem)
    {
        if(m_strNetworkUUID.empty())
        {
            VNET_LOG(VNET_LOG_ERROR, "CIPv4AddrPool::FreeIPv4Addr: IP Pool is not initialized, the LogicNetwork ID is empty!\n");
            return -1;
        }

        if (LOGIC_NET == m_tNetType)
        {
            // Get DB oper;
            CDBOperateLogicNetworkIpPool *pOper = GetDbILogicNetworkIpPool();
            if(NULL == pOper)
            {
                VNET_LOG(VNET_LOG_ERROR, "CIPv4AddrPool::FreeIPv4Addr: GetDbILogicNetworkIpPool() is NULL.\n");
                return -1;
            }
            CIPv4Addr cIPv4Addr;
            cIPv4Addr.Set(tItem);
            CDbLogicNetworkIpPool cDBIPAddr;
            cDBIPAddr.SetUuid(m_strNetworkUUID.c_str());        
            cDBIPAddr.SetIpStr(cIPv4Addr.GetIP().c_str());
            cDBIPAddr.SetMaskStr(cIPv4Addr.GetMask().c_str());
            cDBIPAddr.SetIpNum((int64)CIPv4Addr::N2HL(tItem.uIP.dwAddr));
            cDBIPAddr.SetMaskNum((int64)CIPv4Addr::N2HL(tItem.uMask.dwAddr));
            int32 nRet = pOper->CheckDel(cDBIPAddr);
            if(VNET_DB_SUCCESS != nRet)
            {
                if(VNET_DB_IS_ITEM_NO_EXIST(nRet))
                {
                    return 0;
                }
                if(s_nOutputIPDbgInfFlag)
                {
                    cout << "CIPv4AddrPool::FreeIPv4Addr: check IP(LogicNetwork:" << m_strNetworkUUID
                        << ", IP: " << cIPv4Addr.GetIPAndMask() << ") conflict in DB failed, ret:"
                        << nRet <<"." << endl;
                }
                return -1;
            }
            
            nRet = pOper->Del(cDBIPAddr);
            if(VNET_DB_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_WARN, 
                     "CIPv4AddrPool::FreeIPv4Addr: Delete IP(LogicNetwork:%s, IP: %s) from DB failed, ret:%d.\n",
                     m_strNetworkUUID.c_str(),
                     cIPv4Addr.GetIPAndMask().c_str(),
                     nRet);
                return -1;
            }
        }
        else
        {
            CDBOperateNetplaneIpPool *pOper = GetDbINetplaneIpPool();
            if(NULL == pOper)
            {
                VNET_LOG(VNET_LOG_ERROR, "CIPv4AddrPool::FreeIPv4Addr: GetDbINetplaneIpPool() is NULL.\n");
                return -1;
            }
            CIPv4Addr cIPv4Addr;
            cIPv4Addr.Set(tItem);
            CDbNetplaneIpPool cDBIPAddr;
            cDBIPAddr.SetUuid(m_strNetworkUUID.c_str());        
            cDBIPAddr.SetIpStr(cIPv4Addr.GetIP().c_str());
            cDBIPAddr.SetMaskStr(cIPv4Addr.GetMask().c_str());
            cDBIPAddr.SetIpNum((int64)CIPv4Addr::N2HL(tItem.uIP.dwAddr));
            cDBIPAddr.SetMaskNum((int64)CIPv4Addr::N2HL(tItem.uMask.dwAddr));
            int32 nRet = pOper->CheckDel(cDBIPAddr);
            if(VNET_DB_SUCCESS != nRet)
            {
                if(VNET_DB_IS_ITEM_NO_EXIST(nRet))
                {
                    return 0;
                }
                if(s_nOutputIPDbgInfFlag)
                {
                    cout << "CIPv4AddrPool::FreeIPv4Addr: check IP(Netplane:" << m_strNetworkUUID
                        << ", IP: " << cIPv4Addr.GetIPAndMask() << ") conflict in DB failed, ret:"
                        << nRet <<"." << endl;
                }
                return -1;
            }
            
            nRet = pOper->Del(cDBIPAddr);
            if(VNET_DB_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_WARN, 
                     "CIPv4AddrPool::FreeIPv4Addr: Delete IP(Netplane:%s, IP: %s) from DB failed, ret:%d.\n",
                     m_strNetworkUUID.c_str(),
                     cIPv4Addr.GetIPAndMask().c_str(),
                     nRet);
                return -1;
            }
        }
        return 0;
    }

    int32 CIPv4AddrPool::DbgShowData(void)
    {
        if (LOGIC_NET == m_tNetType)
        {
            CDBOperateLogicNetworkIpPool *pOper = GetDbILogicNetworkIpPool();
            if(NULL == pOper)
            {
                cout << "CIPv4AddrPool::DbgShowData: GetDbILogicNetworkIpPool() is NULL." << endl;
                return -1;
            }

            vector<CDbLogicNetworkIpPool> outVInfo;
            int32 nRet = pOper->Query(m_strNetworkUUID.c_str(),outVInfo);
            if(VNET_DB_SUCCESS != nRet)
            {
                cout << "CIPv4AddrPool::DbgShowData: DB query failed." << endl;
                return -1;
            }
            if(outVInfo.empty())
            {
                cout << "LG_NET:" << m_strNetworkUUID << ", NO allocated IP address." << endl;
                return 0;
            }
            
            vector<CDbLogicNetworkIpPool>::iterator iter = outVInfo.begin();
            for(; iter != outVInfo.end(); ++iter)
            {
                cout << "LNID:" << iter->GetUuid() 
                     << ", {" << iter->GetIpStr()
                     << "-" << iter->GetMaskStr()
                     << "}, hex:{0x" << hex << iter->GetIpNum()
                     << "-0x" << hex << iter->GetMaskNum() << "}."
                     << endl; 
            }
            cout.unsetf(ios::hex);
        }
        else
        {
            CDBOperateNetplaneIpPool *pOper = GetDbINetplaneIpPool();
            if(NULL == pOper)
            {
                cout << "CIPv4AddrPool::DbgShowData: GetDbINetplaneIpPool() is NULL." << endl;
                return -1;
            }

            vector<CDbNetplaneIpPool> outVInfo;
            int32 nRet = pOper->Query(m_strNetworkUUID.c_str(),outVInfo);
            if(VNET_DB_SUCCESS != nRet)
            {
                cout << "CIPv4AddrPool::DbgShowData: DB query failed." << endl;
                return -1;
            }
            if(outVInfo.empty())
            {
                cout << "LG_NET:" << m_strNetworkUUID << ", NO allocated IP address." << endl;
                return 0;
            }
            
            vector<CDbNetplaneIpPool>::iterator iter = outVInfo.begin();
            for(; iter != outVInfo.end(); ++iter)
            {
                cout << "LNID:" << iter->GetUuid() 
                     << ", {" << iter->GetIpStr()
                     << "-" << iter->GetMaskStr()
                     << "}, hex:{0x" << hex << iter->GetIpNum()
                     << "-0x" << hex << iter->GetMaskNum() << "}."
                     << endl; 
            }
            cout.unsetf(ios::hex);
        }
        return 0;
    }

    void VNetDbgSetIPAllocPrint(void)
    {
        if(s_nOutputIPDbgInfFlag)
        {
            s_nOutputIPDbgInfFlag = 0;
            cout << "IP Alloc print is closed." << endl;
        }
        else
        {
            s_nOutputIPDbgInfFlag = 1;
            cout << "IP Alloc print is opened." << endl;
        }
        return ;
    }
    DEFINE_DEBUG_FUNC(VNetDbgSetIPAllocPrint);
    
}// end namespace zte_tecs

