/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：range_arithm.cpp
* 文件标识：
* 内容摘要：CRangeArithmetic类实现文件
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
#include "range_arithm.h"


namespace zte_tecs
{
    CRangeArithmetic * CRangeArithmetic::s_pInstance = NULL;
    
    int32 CRangeArithmetic::AddRange(const TRange &tRange)
    {
        if(tRange.dwStart > tRange.dwEnd)
        {
            VNET_ASSERT(0);
            return -1;
        }
    
        m_vecRange.push_back(tRange);
        return 0;
    }
    
    int32 CRangeArithmetic::DeleteRange(const TRange &tRange)
    {
        if(tRange.dwStart > tRange.dwEnd)
        {
            VNET_ASSERT(0);
            return -1;
        }
    
        if(m_vecRange.empty())
        {
            VNET_LOG(VNET_LOG_WARN, "CRangeArithmetic::DeleteRange: no range to be deleted.\n");
            return 0;
        }
        
        if(!m_vecMergedRange.empty())
        {
            m_vecMergedRange.clear();
        }
    
        m_vecMergedRange.assign(m_vecRange.begin(), m_vecRange.end());
    
        // find range
        vector<TRange>::iterator itr = m_vecMergedRange.begin();
        for(; itr != m_vecMergedRange.end(); ++itr)
        {
            if(itr->dwStart <= tRange.dwStart && itr->dwEnd >= tRange.dwEnd)
            {   
                if(itr->dwStart == tRange.dwStart && itr->dwEnd == tRange.dwEnd)
                {
                    itr = m_vecMergedRange.erase(itr);
                    goto DEL_RANGE_SUCC;
                }
    
                if(itr->dwStart == tRange.dwStart)
                {
                    itr->dwStart = tRange.dwEnd + 1;
                    goto DEL_RANGE_SUCC;
                }
    
                if(itr->dwEnd == tRange.dwEnd)
                {
                    itr->dwEnd = tRange.dwStart - 1;
                    goto DEL_RANGE_SUCC;
                }
    
                TRange tNewRange = {0};
                tNewRange.dwStart = itr->dwStart;
                tNewRange.dwEnd = tRange.dwStart - 1;
    
                itr->dwStart = tRange.dwEnd+1;
    
                m_vecMergedRange.insert(itr, tNewRange);
                goto DEL_RANGE_SUCC;
            }   
        }
        
        return -1;
DEL_RANGE_SUCC:
        if(m_bDbgShowProcessData)
        {
            DbgShowData();
        }
        return 0;
    }
    
    vector<TRange> &CRangeArithmetic::GetRangeColl(void)
    {
        return m_vecRange;
    }
    
    vector<TRange> &CRangeArithmetic::GetMergedRangeColl(void)
    {
        return m_vecMergedRange;
    }
    
    int32 CRangeArithmetic::RemoveAll(void)
    {
        m_vecRange.clear();
        m_vecMergedRange.clear();
        return 0;
    }
    
    void CRangeArithmetic::DbgShowData(void)
    {
        if(m_vecRange.empty())
        {
            return ;
        }
        vector<TRange>::iterator itr = m_vecRange.begin();
        cout << "Original range collection:";
        for(; itr != m_vecRange.end(); ++itr)
        {
            cout << " {" << itr->dwStart << ", " << itr->dwEnd << "}";
        }
        cout << endl;
    
        if(m_vecMergedRange.empty())
        {
            return ;
        }
        itr = m_vecMergedRange.begin();
        cout << "Merged range collection:";
        for(; itr != m_vecMergedRange.end(); ++itr)
        {
            cout << " {" << itr->dwStart << ", " << itr->dwEnd << "}";
        }
        cout << endl;
    
        return ;
    }
    
    void CRangeArithmetic::DbgShowRangeVector(vector<TRange> &vecRange)
    {
        vector<TRange>::iterator itr = vecRange.begin();
        cout << "range set:";
        for(; itr != vecRange.end(); ++itr)
        {
            cout << " {" << itr->dwStart << ", " << itr->dwEnd << "}";
        }
        cout << endl;
        return ;
    }
    
    void CRangeArithmetic::DbgShowRangePoint(vector<TRangePoint> &vecRangePoint)
    {
        vector<TRangePoint>::iterator itr = vecRangePoint.begin();
        cout << "range point set:";
        for(; itr != vecRangePoint.end(); ++itr)
        {
            string strRole = "";
            switch(itr->ucRole)
            {
            case EN_START_POINT:
                {
                    strRole = "SP";
                }
                break;
            case EN_END_POINT:
                {
                    strRole = "EP";
                }
                break;
            default:
                //VNET_ASSERT(0);
                break;
            }
            cout << " {" << strRole << "," << itr->dwValue << "}";
        }
        cout << endl;
        return ;
    }
    
    void CRangeArithmetic::DbgShowProcessData(bool bOpen)
    {
        m_bDbgShowProcessData = bOpen;
    }
    
    bool CRangeArithmetic::CompareRange(const TRange &tRange1, const TRange &tRange2)
    {
        if(tRange1.dwEnd <= tRange2.dwStart || tRange1.dwStart <= tRange2.dwStart)
        {
            return true;
        }   
        
        return false;
    }
    
    bool CRangeArithmetic::CompareRangePoint(const TRangePoint &tPoint1, const TRangePoint &tPoint2)
    {
        if(tPoint1.dwValue < tPoint2.dwValue)
        {
            return true;
        }
        if(tPoint1.dwValue == tPoint2.dwValue)
        {
            if(EN_START_POINT == tPoint1.ucRole)
            {
                return true;
            }       
        }
    
        return false;
    }   
    
    int32 CRangeArithmetic::SortColl(void)
    {
        sort(m_vecRange.begin(), m_vecRange.end(), CompareRange);
        return 0;
    }

    bool CRangeArithmetic::IsAssociationRange(const TRange &tRange1, const TRange &tRange2)
    {
#ifdef __NEED_OPENED_INTERVAL_
        // opened interval association;
        if(tRange1.dwEnd < tRange2.dwStart || tRange1.dwStart > tRange2.dwEnd)
        {
            return false;
        }
#endif
        // closed interval association;
        if((tRange1.dwEnd+1) < tRange2.dwStart || tRange1.dwStart > (tRange2.dwEnd+1))
        {
            return false;
        }
    
        return true;
    }
    
    int32 CRangeArithmetic::DiscoverAssocRange(const TRange &tRange, vector<TRange> &vecRangeSet, vector<TRange> &vecAssocRange)
    {
        int32 bDiscover = 0;
        TRange tAssocRange = {0};
        vector<TRange>::iterator itrRange = vecRangeSet.begin();
        while(itrRange != vecRangeSet.end())
        {
            if(CRangeArithmetic::IsAssociationRange(tRange, *itrRange))
            {
                bDiscover = 1;
                tAssocRange = *itrRange;
                itrRange = vecRangeSet.erase(itrRange);
                break;
            }
            else
            {
                ++itrRange;
            }
        }
    
        if(bDiscover)
        {
            vecAssocRange.push_back(tAssocRange);
            return DiscoverAssocRange(tAssocRange, vecRangeSet, vecAssocRange);
        }
    
        return 0;   
    }

    /*
    * 1. 所有的range 进行按照起点大小排序;
    * 2. 进行关联range合并;
    */
    int32 CRangeArithmetic::MergeSortColl(void)
    {
        if(m_vecRange.empty())
        {
            return -1;
        }
        // sort
        SortColl();
        // need optimising;
        vector<TRange>::iterator itrRange = m_vecRange.begin();
        TRange tMergedRange = *itrRange++;
        for(; itrRange != m_vecRange.end(); ++itrRange)
        {
            if(CRangeArithmetic::IsAssociationRange(tMergedRange, *itrRange))
            {
                // merge;
                if(tMergedRange.dwStart > itrRange->dwStart)
                {
                    tMergedRange.dwStart = itrRange->dwStart;
                }
                if(tMergedRange.dwEnd < itrRange->dwEnd)
                {
                    tMergedRange.dwEnd = itrRange->dwEnd;
                }

                continue;
            }
            else
            {
                // Save the merged range;
                m_vecMergedRange.push_back(tMergedRange);
                tMergedRange = *itrRange;
            }
        }
        m_vecMergedRange.push_back(tMergedRange);
        
        if(m_bDbgShowProcessData)
        {
            DbgShowData();
        }
        
        return 0;
    }

#ifdef __ALGORITHM_FUN_2__
    /*
    * 1. 所有的range 进行按照起点大小排序;
    * 2. 选择合适的range起点;
    * 3. 遍历筛选出相关联的range; 
    * 4. 进行关联range合并;
    */
    int32 CRangeArithmetic::MergeSortColl(void)
    {
        if(m_vecRange.empty())
        {
            return -1;
        }
        // sort
        SortColl();
        vector<TRange>::iterator itrRange = m_vecRange.begin();
        uint32 dwMinStartPoint = 0;
        for(; itrRange != m_vecRange.end(); ++itrRange)
        {
            TRange tRange = *itrRange;
            if(dwMinStartPoint >= tRange.dwStart)
            {
                continue;
            }
            vector<TRange> vecAssocRange;
            // filter association ranges;
            vector<TRange> vecRangeCopy;
            vecRangeCopy.assign(m_vecRange.begin(), m_vecRange.end());
            // need optimising;
            DiscoverAssocRange(tRange, vecRangeCopy, vecAssocRange);
            // Merge Association Ranges;
            TRange tMergedRange = {0};
            tMergedRange.dwStart = (uint32)(-1);
            vector<TRange>::iterator itrRange1 = vecAssocRange.begin();
            for(; itrRange1 != vecAssocRange.end(); ++itrRange1)
            {
                if(tMergedRange.dwStart > itrRange1->dwStart)
                {
                    tMergedRange.dwStart = itrRange1->dwStart;
                }
                if(tMergedRange.dwEnd < itrRange1->dwEnd)
                {
                    tMergedRange.dwEnd = itrRange1->dwEnd;
                }
            }
            // Save the merged range;
            m_vecMergedRange.push_back(tMergedRange);
            dwMinStartPoint = tMergedRange.dwEnd;
        }
    
        if(m_bDbgShowProcessData)
        {
            DbgShowData();
        }
        
        return 0;
    }
#endif

    void VNetDbgShowRangeArithm(bool bOpened)
    {
        CRangeArithmetic *p = CRangeArithmetic::GetInstance();
        if(p)
        {
            p->DbgShowProcessData(bOpened);
        }
        
        return ;
    }
    DEFINE_DEBUG_FUNC(VNetDbgShowRangeArithm);
    
}// end namespace zte_tecs

