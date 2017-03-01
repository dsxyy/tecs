/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�segment_range.cpp
* �ļ���ʶ��
* ����ժҪ��CSegmentRange��ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�
* ������ڣ�2013��09��05��
*
* �޸ļ�¼1��
*     �޸����ڣ�2013/09/05
*     �� �� �ţ�V1.0
*     �� �� �ˣ�zhao.jin
*     �޸����ݣ�����
******************************************************************************/

#include "vnet_comm.h"
#include "range_arithm.h"
#include "segment_range.h"

namespace zte_tecs
{
    CSegmentRange::CSegmentRange()
    {
    }
    
    CSegmentRange::~CSegmentRange()
    {
        //free vector container
        if(!m_vecSegmentRange.empty())
        {
            m_vecSegmentRange.clear();
        }
    }
    
    // �˽ӿڱ������ݿ��м�����Ч������ʱ����;
    int32 CSegmentRange::AddValidRange(const TSegmentRangeItem &tSegmentRange)
    {
        m_vecSegmentRange.push_back(tSegmentRange);
        return 0;
    }
    
    int32 CSegmentRange::AddRange(const TSegmentRangeItem &tSegmentRange)
    {
        if(!IsValidSegmentRange(tSegmentRange))
        {            
            VNET_LOG(VNET_LOG_ERROR, "CSegmentRange::AddRange (%d, %d), the Segment range parameter is invalid.\n", 
                tSegmentRange.dwSegmentIDStart, tSegmentRange.dwSegmentIDEnd);
            return -1;
        }

        if(m_vecSegmentRange.empty())
        {
            m_vecSegmentRange.push_back(tSegmentRange);
            return 0;
        }
        
        CRangeArithmetic *pRangeArith = CRangeArithmetic::GetInstance();
        if(NULL == pRangeArith)
        {
            VNET_LOG(VNET_LOG_ERROR, "CSegmentRange::AddRange: can't get the CRangeArithmetic instance!\n");
            return -1;
        }        
        pRangeArith->RemoveAll();
        vector<TSegmentRangeItem>::iterator itr = m_vecSegmentRange.begin();        
        TRange tRange = {0};
        for(; itr != m_vecSegmentRange.end(); ++itr)
        {  
            tRange.dwStart = itr->dwSegmentIDStart;
            tRange.dwEnd = itr->dwSegmentIDEnd;
            pRangeArith->AddRange(tRange);
        }        
        tRange.dwStart = tSegmentRange.dwSegmentIDStart;
        tRange.dwEnd = tSegmentRange.dwSegmentIDEnd;
        pRangeArith->AddRange(tRange);  
        
        pRangeArith->MergeSortColl();
        if(pRangeArith->GetMergedRangeColl().empty())
        {
            VNET_LOG(VNET_LOG_ERROR, "CSegmentRange::AddRange: failed to add the Segment Range[%d-%d]!\n", 
                    tSegmentRange.dwSegmentIDStart, tSegmentRange.dwSegmentIDEnd);
            return -1;
        }
        
        m_vecSegmentRange.clear();
        vector<TRange>::iterator itrRange =  pRangeArith->GetMergedRangeColl().begin();
        for(;itrRange != pRangeArith->GetMergedRangeColl().end(); ++itrRange)
        {
            TSegmentRangeItem tSegmentRangeItem = {0};
            tSegmentRangeItem.dwSegmentIDStart = itrRange->dwStart;
            tSegmentRangeItem.dwSegmentIDEnd = itrRange->dwEnd;
            m_vecSegmentRange.push_back(tSegmentRangeItem);
        }
        return 0;
    }
    
    int32 CSegmentRange::DelRange(const TSegmentRangeItem &tSegmentRange)
    {
        if(!IsValidSegmentRange(tSegmentRange))
        {            
            VNET_LOG(VNET_LOG_ERROR, "CSegmentRange::DelRange (%d, %d), the Segment range parameter is invalid.\n", 
                tSegmentRange.dwSegmentIDStart, tSegmentRange.dwSegmentIDEnd);
            return -1;
        }

        CRangeArithmetic *pRangeArith = CRangeArithmetic::GetInstance();
        if(NULL == pRangeArith)
        {
            VNET_LOG(VNET_LOG_ERROR, "CSegmentRange::DelRange: can't get the CRangeArithmetic instance!\n");
            return -1;
        }
        pRangeArith->RemoveAll();
        vector<TSegmentRangeItem>::iterator itr = m_vecSegmentRange.begin();        
        TRange tRange = {0};
        for(; itr != m_vecSegmentRange.end(); ++itr)
        {  
            tRange.dwStart = itr->dwSegmentIDStart;
            tRange.dwEnd = itr->dwSegmentIDEnd;
            pRangeArith->AddRange(tRange);
        }
        
        tRange.dwStart = tSegmentRange.dwSegmentIDStart;
        tRange.dwEnd = tSegmentRange.dwSegmentIDEnd;
        
        if(0 != pRangeArith->DeleteRange(tRange))
        {  
            VNET_LOG(VNET_LOG_ERROR, "CSegmentRange::DelRange:Delete the Segment range(%d, %d) failed.\n", 
                tSegmentRange.dwSegmentIDStart, tSegmentRange.dwSegmentIDEnd);
            return -1;
        }
        
        m_vecSegmentRange.clear();
        if(pRangeArith->GetMergedRangeColl().empty())
        {  
            return 0;
        }
        
        vector<TRange>::iterator itrRange =  pRangeArith->GetMergedRangeColl().begin();
        for(;itrRange != pRangeArith->GetMergedRangeColl().end(); ++itrRange)
        {
            TSegmentRangeItem tSegmentRangeItem = {0};
            tSegmentRangeItem.dwSegmentIDStart = itrRange->dwStart;
            tSegmentRangeItem.dwSegmentIDEnd = itrRange->dwEnd;
            m_vecSegmentRange.push_back(tSegmentRangeItem);
        } 
        
        return 0;
    }

    int32 CSegmentRange::IsValidSegmentRange(const TSegmentRangeItem &tSegmentRange)
    {
        if ((tSegmentRange.dwSegmentIDStart < VNET_MIN_SEGMENT_ID) || (tSegmentRange.dwSegmentIDStart > VNET_MAX_MAP_SEGMENT_ID)
            ||(tSegmentRange.dwSegmentIDEnd < VNET_MIN_SEGMENT_ID) || (tSegmentRange.dwSegmentIDEnd > VNET_MAX_MAP_SEGMENT_ID)
            ||(tSegmentRange.dwSegmentIDStart > tSegmentRange.dwSegmentIDEnd))
        {
            return 0;
        }
        
        return 1;
    }  

    void CSegmentRange::DbgShowSegmentRange(void)
    {
        if(!m_vecSegmentRange.empty())
        {   
            cout << "Range Collection: " << endl;            
            vector<TSegmentRangeItem>::iterator itr = m_vecSegmentRange.begin();
            for(; itr != m_vecSegmentRange.end(); ++itr)
            {       
                cout << "{" << itr->dwSegmentIDStart << "," 
                     << itr->dwSegmentIDEnd <<"}" << endl;      
            }
        } 
        
        return ;
    } 
}// end namespace zte_tecs

