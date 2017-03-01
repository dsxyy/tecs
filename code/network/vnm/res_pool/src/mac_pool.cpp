/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：mac_pool.cpp
* 文件标识：
* 内容摘要：CMACPool类实现文件
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
#include "vnet_db_netplane_macpool.h"
#include "vnet_db_netplane_macrange.h"

#include "mac_addr.h"
#include "mac_range.h"
#include "mac_pool.h"


namespace zte_tecs
{
static int32 s_nOutputDbgInfFlag = 0;

    CMACPool::CMACPool()
    {
        m_pOUI1MACRange = NULL;
        m_pOUI2MACRange = NULL;
    }

    CMACPool::~CMACPool()
    {
        m_pOUI1MACRange = NULL;
        m_pOUI2MACRange = NULL;
    }

    int32 CMACPool::Init(const string &cstrNetplaneUUID)
    {
        if(cstrNetplaneUUID.empty())
        {
            VNET_LOG(VNET_LOG_ERROR, "CMACPool::Init: cstrNetplaneUUID is empty.\n");
            return -1;
        }
        
        m_strNetplaneUUID = cstrNetplaneUUID;       
        return 0;
    }
    
    int32 CMACPool::Init(const string &cstrNetplaneUUID, CMACRange *pOUI1MACRange, CMACRange *pOUI2MACRange)
    {
        if(NULL == pOUI1MACRange || NULL == pOUI2MACRange)
        {
            VNET_ASSERT(0);
            return -1;
        }
        if(cstrNetplaneUUID.empty())
        {
            VNET_LOG(VNET_LOG_ERROR, "CMACPool::Init: cstrNetplaneUUID is empty.\n");
            return -1;
        }
        
        m_strNetplaneUUID = cstrNetplaneUUID;
        m_pOUI1MACRange = pOUI1MACRange;
        m_pOUI2MACRange = pOUI2MACRange;        
        return 0;
    }
    
    int32 CMACPool::AllocMAC(int32 nKey, string &strMAC)
    {
        if(NULL == m_pOUI1MACRange || NULL == m_pOUI2MACRange)
        {
            VNET_LOG(VNET_LOG_ERROR, "CMACPool::AllocMAC: CMACPool is not initialized, the m_pOUI1MACRange[0x%x] and m_pOUI2MACRange[0x%x] is invalid.\n",
                m_pOUI1MACRange, m_pOUI2MACRange);
            return -1;
        }
        
        if(m_strNetplaneUUID.empty())
        {
            VNET_LOG(VNET_LOG_ERROR, "CMACPool::AllocMAC: CMACPool is not initialized, netplane is empty.\n");
            return -1;
        }        


        CVNetDbMgr * pMgr = CVNetDbMgr::GetInstance();
        if(NULL == pMgr)
        {
            VNET_LOG(VNET_LOG_ERROR, "CMACPool::AllocMAC: CVNetDbMgr::GetInstance() is NULL.\n");
            return -1;
        }        

        
        // OUI1 MAC POOL;        
        vector<TMACRangeItem>::iterator itr = (vector<TMACRangeItem>::iterator)NULL;
        if(!m_pOUI1MACRange->GetMACRange().empty())
        {
            if(0 == AllocMACAtRange(m_pOUI1MACRange->GetMACRange(), m_pOUI1MACRange->GetMACOUI(), nKey, strMAC))
            {
                return 0;
            }             
        } 
        
        if(!m_pOUI2MACRange->GetMACRange().empty())
        {
            if(0 == AllocMACAtRange(m_pOUI2MACRange->GetMACRange(), m_pOUI2MACRange->GetMACOUI(), nKey, strMAC))
            {
                return 0;
            }             
        } 
        
        return -1;
    }   

    bool CMACPool::CompareRange(const TMACRangeUsedInfo &tRange1, 
                                const TMACRangeUsedInfo &tRange2)
    {
        /* todo: 
         * 后续可以对区域按照资源分配其他方式排序，
         * 进行资源分配方式可配
         */
        return (tRange1.nFreeCnt > tRange2.nFreeCnt) ? true : false;
    }
    
    int32 CMACPool::GetAvailableRange(vector<TMACRangeItem> &vecMACRange, 
                                      vector<TMACRangeUsedInfo> &vecAvailableRange)
    {
        vector<TMACRangeItem>::iterator itr = vecMACRange.begin();
        uint32 dwMACStart = 0;
        uint32 dwMACEnd = 0;
        for(; itr != vecMACRange.end(); ++itr)
        {
            uint32 nCnt = 0;
            if(0 != GetMACCnt(*itr, nCnt))
            {
                // query failed;
                continue;
            }
            
            dwMACStart = itr->tStartMAC.dwData;
            dwMACEnd = itr->tEndMAC.dwData;
            uint32 nTotalCnt = dwMACEnd - dwMACStart + 1;
            if(nCnt >= nTotalCnt)
            {
                // no free resources;
                continue;
            }
            
            TMACRangeUsedInfo tMACRangeUsedInfo;
            tMACRangeUsedInfo.nUsedCnt = nCnt;
            tMACRangeUsedInfo.nFreeCnt = nTotalCnt - nCnt;
            tMACRangeUsedInfo.tRange = *itr;
            vecAvailableRange.push_back(tMACRangeUsedInfo);
        }

        if(!vecAvailableRange.empty())
        {
            sort(vecAvailableRange.begin(), vecAvailableRange.end(), CompareRange);
        }

        if(s_nOutputDbgInfFlag)
        {
            cout << "-------------------Original Ranges----------------------" << endl;
            for(itr = vecMACRange.begin(); itr != vecMACRange.end(); ++itr)
            {
                cout << "OUI:" << hex 
                     << itr->tStartMAC.dwOUI << ",{" 
                     << itr->tStartMAC.dwData << "," 
                     << itr->tEndMAC.dwData << "},Size:" << dec
                     << itr->tEndMAC.dwData-itr->tStartMAC.dwData+1 << "."
                     << endl;
            }
            cout << "-------------------Sorted Ranges------------------------" << endl;            
            if(!vecAvailableRange.empty())
            {
                vector<TMACRangeUsedInfo>::iterator itr1 = vecAvailableRange.begin();
                for(; itr1 != vecAvailableRange.end(); ++itr1)
                {
                    cout << "OUI:" << hex 
                         << itr1->tRange.tStartMAC.dwOUI << ",{" 
                         << itr1->tRange.tStartMAC.dwData << "," 
                         << itr1->tRange.tEndMAC.dwData << "},Used:" << dec
                         << itr1->nUsedCnt << ",Free:"
                         << itr1->nFreeCnt << "."
                         << endl;
                }
            }
            else
            {
                cout << "No available MAC ranges." << endl;
            }
            cout.unsetf(ios::hex);
        }
        
        return 0;
    }

    class CVecFindMAC
    {
    public:
        CVecFindMAC(int64 nValue){m_nMAC = nValue;}
        ~CVecFindMAC(){}
    public:
         bool operator ()(CDbNetplaneMacPool &cDBMAC)
         {
            return m_nMAC == cDBMAC.GetMacNum();     
         }
    private:
        int64 m_nMAC;
    };

    
    int32 CMACPool::ScanAllocMAC(uint32 dwOUI, uint32 dwMACStart, uint32 dwMACEnd, TUInt32MAC &tMAC)
    {
        VNET_LOG(VNET_LOG_INFO, "CMACPool::ScanAllocMAC: Free space in MAC pool are few. Alloc MAC need more time...\n");

        CDBOperateNetplaneMacPool *pOper = GetDbINetplaneMacPool();
        if(NULL == pOper)
        {
            VNET_LOG(VNET_LOG_ERROR, "CMACPool::ScanAllocMAC: GetDbINetplaneMacPool() is NULL.\n");
            return -1;
        }

        int64 nMACBegin = dwOUI;
        nMACBegin = (nMACBegin << 32) | dwMACStart;
        int64 nMACEnd = dwOUI;
        nMACEnd = (nMACEnd << 32) | dwMACEnd;
        vector<CDbNetplaneMacPool> vecMACOut;
        int32 nRet = pOper->QueryByNpRange(m_strNetplaneUUID.c_str(), nMACBegin, nMACEnd, vecMACOut);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CMACPool::ScanAllocMAC: call QueryByNpRange(%s, %lx, %lx) failed, nRet=%d.\n", 
                m_strNetplaneUUID.c_str(),
                nMACBegin,
                nMACEnd,
                nRet);
            return -1;
        }
        
        if(s_nOutputDbgInfFlag)
        {
            cout << "-------------------MAC POOL INFO----------------------" << endl;            
            cout << "Netplane UUID:" << m_strNetplaneUUID << "." << endl;
            cout << "Range:[" << dwMACStart <<","<< dwMACEnd << "]." << endl;
            vector<CDbNetplaneMacPool>::iterator itr = vecMACOut.begin();
            for(; itr != vecMACOut.end(); ++itr)
            {
                cout << itr->GetMacStr()<< endl;
            }
        }

        for(uint32 i = dwMACStart; i <= dwMACEnd; i++)
        {
            int64 nValue = dwOUI;
            nValue = (nValue << 32) | i;
            vector<CDbNetplaneMacPool>::iterator pos = 
                find_if(vecMACOut.begin(), vecMACOut.end(), CVecFindMAC(nValue));
            if(pos != vecMACOut.end())
            {
                // Is allocated.
                continue;
            }
            else
            {
                tMAC.dwOUI = dwOUI;
                tMAC.dwData = i;
                return 0;
            }
        }  

        return -1;
    }

    int32 CMACPool::AllocMACAtRange(vector<TMACRangeItem> &vecMACRange, uint32 dwOUI, int32 nKey, string &strMAC)
    {
        if(vecMACRange.empty())
        {
            return -1;
        }

        TUInt32MAC tUInt32MAC = {0};
        tUInt32MAC.dwOUI = dwOUI; 
        vector<TMACRangeUsedInfo> vecAvailableRange;
        GetAvailableRange(vecMACRange, vecAvailableRange);
        if(vecAvailableRange.empty())
        {
            VNET_LOG(VNET_LOG_ERROR, "CMACPool::AllocMACAtRange:No available MAC ranges.\n");
            return -1;
        }
        
        vector<TMACRangeUsedInfo>::iterator itr = vecAvailableRange.begin();
        uint32 dwMACStart = 0;
        uint32 dwMACEnd = 0;
        for(; itr != vecAvailableRange.end(); ++itr)
        {
            dwMACStart = itr->tRange.tStartMAC.dwData;
            dwMACEnd = itr->tRange.tEndMAC.dwData;         
            CalcMACAddr(nKey, dwMACEnd, dwMACStart, tUInt32MAC.dwData);
            int nTryCnt = 0; //每个range尝试15次分配;
            CMACAddr cMACAddr;
            while(1)
            {
                // DB存储 包括条目冲突检测分配;   
                cMACAddr.Set(tUInt32MAC);
                // DB Save;
                if(0 != SaveMAC2DB(cMACAddr))
                {
                    nTryCnt++;
                    if(nTryCnt > 15)
                    {
                        break;
                    }
                    
                    if(0 == (nTryCnt % 5))
                    {
                        // 增加随机性;
                        CalcMACAddr(nKey, dwMACEnd, dwMACStart, tUInt32MAC.dwData);
                        continue;
                    }
                    
                    if(nTryCnt > 10)
                    {
                        tUInt32MAC.dwData += 100000;
                        tUInt32MAC.dwData = (tUInt32MAC.dwData % (dwMACEnd - dwMACStart)) + dwMACStart;
                    }
                    else
                    {
                        tUInt32MAC.dwData += 1;
                        tUInt32MAC.dwData = (tUInt32MAC.dwData % (dwMACEnd - dwMACStart)) + dwMACStart;
                    }
                    continue;
                }
                else
                {
                    strMAC = cMACAddr.GetStringMAC();
                    return 0;
                }
            }// end while(1)...;

            // 由于冲突尝试失败，只能遍历分配;
            if(0 == ScanAllocMAC(dwOUI, dwMACStart, dwMACEnd, tUInt32MAC))
            {
                cMACAddr.Set(tUInt32MAC);
                if(s_nOutputDbgInfFlag)
                {
                    VNET_LOG(VNET_LOG_INFO, "CMACPool::AllocMACAtRange: allocated MAC[%s] successfully by scanning mode.\n", 
                        cMACAddr.GetStringMAC().c_str());
                }
                // DB Save;
                if(0 == SaveMAC2DB(cMACAddr))
                {
                    strMAC = cMACAddr.GetStringMAC();
                    return 0;
                }
            }
            
        }// end for(itr...; 
        return -1;
    }
    
    int32 CMACPool::SaveMAC2DB(const CMACAddr &cMACAddr)
    {
        CDBOperateNetplaneMacPool *pOper = GetDbINetplaneMacPool();
        if(NULL == pOper)
        {
            VNET_LOG(VNET_LOG_ERROR, "CMACPool::SaveMAC2DB: GetDbINetplaneMacPool() is NULL.\n");
            return -1;
        }
        
        CDbNetplaneMacPool cDBMAC;
        cDBMAC.SetUuid(m_strNetplaneUUID.c_str());
        TUInt32MAC tUInt32MAC = {0};
        cMACAddr.GetUInt32MAC(tUInt32MAC);
        int64 nValue = tUInt32MAC.dwOUI;
        nValue = (nValue << 32) | tUInt32MAC.dwData;
        cDBMAC.SetMacNum(nValue);
        cDBMAC.SetMacStr(cMACAddr.GetStringMAC().c_str());
        int32 nRet = pOper->CheckAdd(cDBMAC);
        if(VNET_DB_SUCCESS != nRet)
        {
            if(s_nOutputDbgInfFlag)
            {
                cout << "CMACPoolMgr::SaveMAC2DB: check MAC(Netplane:" << m_strNetplaneUUID
                     << ", MAC: " << cMACAddr.GetStringMAC() << ") conflict in DB failed, ret:"
                     << nRet <<"." << endl;
            }
            return -1;
        }

        nRet = pOper->Add(cDBMAC);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_WARN, 
                "CMACPoolMgr::SaveMAC2DB: add MAC(Netplane:%s, MAC: %s) to DB  failed, ret:%d.\n",
                m_strNetplaneUUID.c_str(),
                cMACAddr.GetStringMAC().c_str(),
                nRet);
            return -1;
        }
        
        return 0;
    }
    
    int32 CMACPool::CalcMACAddr(int32 nKey, uint32 dwMACEnd, uint32 dwMACStart, uint32 &dwMACKey)
    {
        if(dwMACEnd == dwMACStart)
        {
            dwMACKey = dwMACStart;
        }
        else
        {
            BYTE aucUUID[16];
            VNetGetUUIDValue(aucUUID);
            int32 nFactorA = *(int32*)(aucUUID);
            int32 nFactorB = *(int32*)(aucUUID+4);
            int32 nFactorC = *(int32*)(aucUUID+8);
            int32 nFactorD = *(int16*)(aucUUID+12);
            int32 nFactorE = *(int16*)(aucUUID+14);
            int32 nFactorF = nKey;

            dwMACKey = nFactorA*102317 
                + nFactorB * 8317 
                + nFactorC * 3331 
                + nFactorD * 2179 
                + nFactorE * 9973 
                + nFactorF * 5711 
                + 382457; 
            
            dwMACKey = (dwMACKey % (dwMACEnd - dwMACStart)) + dwMACStart;
        }
        
        if(s_nOutputDbgInfFlag)
        {
            cout << "CMACPoolMgr::CalcMACAddr: dwMACKey:" << hex << dwMACKey
                 << ", Range:{ " << dwMACStart << ","
                 << dwMACEnd <<"}." << endl;
            cout.unsetf(ios::hex);
        }

        return 0;
    }
    
    int32 CMACPool::FreeMAC(const string &strMAC)
    {
        if(m_strNetplaneUUID.empty())
        {
            VNET_LOG(VNET_LOG_ERROR, "CMACPool::FreeMAC: CMACPool is not initialized, netplane is empty.\n");
            return -1;
        } 
        
        if(strMAC.empty())
        {
            VNET_LOG(VNET_LOG_ERROR, "CMACPool::FreeMAC: strMAC is empty.\n");
            return -1;
        }       

        CDBOperateNetplaneMacPool *pOper = GetDbINetplaneMacPool();
        if(NULL == pOper)
        {
            VNET_LOG(VNET_LOG_ERROR, "CMACPool::FreeMAC: GetDbINetplaneMacPool() is NULL.\n");
            return -1;
        }
        
        CDbNetplaneMacPool cDBMAC;
        cDBMAC.SetUuid(m_strNetplaneUUID.c_str());
        cDBMAC.SetMacStr(strMAC.c_str());
        CMACAddr cMACAddr;
        if(0 != cMACAddr.Set(strMAC))
        {
            VNET_LOG(VNET_LOG_ERROR, "CMACPool::FreeMAC: strMAC(%s) format is invalid.\n", strMAC.c_str());
            return -1;
        }
        TUInt32MAC tUInt32MAC = {0};
        cMACAddr.GetUInt32MAC(tUInt32MAC);
        int64 nValue = tUInt32MAC.dwOUI;
        nValue = (nValue << 32) | tUInt32MAC.dwData;
        cDBMAC.SetMacNum(nValue);
        int32 nRet = pOper->CheckDel(cDBMAC);
        if(VNET_DB_SUCCESS != nRet)
        {
            if(VNET_DB_IS_ITEM_NO_EXIST(nRet))
            {
                return 0;
            }
            
            if(s_nOutputDbgInfFlag)
            {
                cout << "CMACPoolMgr::FreeMAC: check MAC(Netplane:" << m_strNetplaneUUID
                     << ", MAC: " << strMAC << ")deleting operation in DB failed, ret:"
                     << nRet <<"." << endl;
            }
            
            return -1;
        }

        nRet = pOper->Del(cDBMAC);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_WARN, 
                "CMACPoolMgr::FreeMAC: Del MAC(Netplane:%s, MAC: %s) to DB  failed, ret:%d.\n",
                m_strNetplaneUUID.c_str(),
                strMAC.c_str(),
                nRet);
            return -1;
        }

        return 0;
    }

    
    int32 CMACPool::GetMACCnt(const TMACRangeItem &tRange, uint32 &dwCnt)
    {
        CDBOperateNetplaneMacRange *pOper = GetDbINetplaneMacRange();
        if(NULL == pOper)
        {
            VNET_LOG(VNET_LOG_ERROR, "CMACPool::GetMACCnt: GetDbINetplaneMacRange() is NULL.\n");
            return -1;
        }
        
        if(!CMACRange::IsValidMACRange(tRange))
        {
            CMACAddr cMACStart;
            CMACAddr cMACEnd;
            cMACStart.Set(tRange.tStartMAC);
            cMACEnd.Set(tRange.tEndMAC);
            VNET_LOG(VNET_LOG_ERROR, "CMACPool::GetMACCnt: the MAC Range[%s-%s] is invalid!\n", 
                cMACStart.GetStringMAC().c_str(),
                cMACStart.GetStringMAC().c_str());
            return -1;
        }
        
        int64 nValue = 0;
        nValue = tRange.tStartMAC.dwOUI;
        nValue = (nValue << 32) | tRange.tStartMAC.dwData;
        int64 nBegin = nValue;
        nValue = tRange.tEndMAC.dwOUI;
        nValue = (nValue << 32) | tRange.tEndMAC.dwData;
        int32 nCnt = 0;
        int32 nRet = pOper->QueryAllocNum(m_strNetplaneUUID.c_str(), nBegin, nValue, nCnt);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CMACPoolMgr::GetMACCnt: call QueryAllocNum(%s, 0x%lx, 0x%lx) failed, ret:%d.\n",
                m_strNetplaneUUID.c_str(),
                nBegin,
                nValue,
                nRet);
            return -1;
        }
        dwCnt = (uint32)nCnt;
        return 0; 
    }

    int32 CMACPool::DbgShowData(void)
    {
        CDBOperateNetplaneMacPool *pOper = GetDbINetplaneMacPool();
        if(NULL == pOper)
        {
            cout << "CMACPool::DbgShowData: GetDbINetplaneMacPool() is NULL." << endl;
            return -1;
        }
        
        vector<CDbNetplaneMacPool> outVInfo;
        int32 nRet = pOper->Query(m_strNetplaneUUID.c_str(), outVInfo);
        if(VNET_DB_SUCCESS != nRet)
        {
            cout << "CMACPool::DbgShowData Query failed. ret: " << nRet << "." << endl;
            return -1;
        }
        vector<CDbNetplaneMacPool>::iterator itr = outVInfo.begin();
        for(; itr != outVInfo.end(); ++itr)
        {
            cout << "NPID:" << itr->GetUuid() 
                 << ", MAC:" << itr->GetMacStr()
                 << ", Num:0x" << hex << itr->GetMacNum() << endl; 
        }
        cout.unsetf(ios::hex);
        return 0;
    }

    void VNetDbgSetMACAllocPrint(void)
    {
        if(s_nOutputDbgInfFlag)
        {
            s_nOutputDbgInfFlag = 0;
            cout << "MAC Alloc print is closed." << endl;
        }
        else
        {
            s_nOutputDbgInfFlag = 1;
            cout << "MAC Alloc print is opened." << endl;
        }
        
        return ;
    }
    DEFINE_DEBUG_FUNC(VNetDbgSetMACAllocPrint);

    uint32 TestRandomMACKey(int nKey)
    {
        BYTE aucUUID[16];
        VNetGetUUIDValue(aucUUID);
        int32 nFactorA = *(int32*)(aucUUID);
        int32 nFactorB = *(int32*)(aucUUID+4);
        int32 nFactorC = *(int32*)(aucUUID+8);
        int32 nFactorD = *(int16*)(aucUUID+12);
        int32 nFactorE = *(int16*)(aucUUID+14);
        int32 nFactorF = nKey;
        
        uint32 dwMACKey = nFactorA*102317 
            + nFactorB * 8317 
            + nFactorC * 3331 
            + nFactorD * 2179 
            + nFactorE * 9973 
            + nFactorF * 5711 
                + 382457; 
        uint32 dwS = 0x00010203;
        uint32 dwE = 0x00040506;
        dwMACKey = dwS+(dwMACKey)%(dwE-dwS);
        return dwMACKey;
    }

    void VNetDbgTestMacKey(void)
    {
        int32 i = 0;
        vector<int32> vecA;
        for(i = 0; i < 100; i++)
        {
            uint32 dwT = TestRandomMACKey(i);
            cout << hex << dwT << endl;
            vecA.push_back(dwT);
        }
        cout << "-------------------------" << endl;
        vector<int>::iterator itr = vecA.begin();
        for(; itr != vecA.end(); ++itr)
        {
            vector<int>::iterator itr1 = vecA.begin();
            int32 nCnt = 0;
            for(; itr1 != vecA.end(); ++itr1)
            {
                if(*itr == *itr1)
                {
                    nCnt++;
                    if(nCnt > 1)
                    {
                        cout << *itr << endl;
                    }
                }
            }
        }
        return ;
    }
    DEFINE_DEBUG_FUNC(VNetDbgTestMacKey);
    
}// end namespace zte_tecs

