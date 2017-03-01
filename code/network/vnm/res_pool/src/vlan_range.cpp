/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�vlan_range.cpp
* �ļ���ʶ��
* ����ժҪ��CVlanRange��ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�
* ������ڣ�2013��1��18��
*
* �޸ļ�¼1��
*     �޸����ڣ�2013/1/18
*     �� �� �ţ�V1.0
*     �� �� �ˣ�Wang.Lule
*     �޸����ݣ�����
******************************************************************************/

#include "vnet_comm.h"
#include "range_arithm.h"
#include "vlan_range.h"

namespace zte_tecs
{
    CVlanRange::CVlanRange()
    {
    }
    
    CVlanRange::~CVlanRange()
    {
        //free vector container
        if(!m_vecVlanRange.empty())
        {
            m_vecVlanRange.clear();
        }
    }
    
    // �˽ӿڱ������ݿ��м�����Ч������ʱ����;
    int32 CVlanRange::AddValidRange(const TVlanRangeItem &tVlanRange)
    {
        m_vecVlanRange.push_back(tVlanRange);
        return 0;
    }
    
    int32 CVlanRange::AddRange(const TVlanRangeItem &tVlanRange)
    {
        if(!IsValidVlanRange(tVlanRange))
        {            
            VNET_LOG(VNET_LOG_ERROR, "CVlanRange::AddRange (%d, %d), the vlan range parameter is invalid.\n", 
                tVlanRange.dwVlanIDStart, tVlanRange.dwVlanIDEnd);
            return -1;
        }

        if(m_vecVlanRange.empty())
        {
            m_vecVlanRange.push_back(tVlanRange);
            return 0;
        }
        
        CRangeArithmetic *pRangeArith = CRangeArithmetic::GetInstance();
        if(NULL == pRangeArith)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVlanRange::AddRange: can't get the CRangeArithmetic instance!\n");
            return -1;
        }        
        pRangeArith->RemoveAll();
        vector<TVlanRangeItem>::iterator itr = m_vecVlanRange.begin();        
        TRange tRange = {0};
        for(; itr != m_vecVlanRange.end(); ++itr)
        {  
            tRange.dwStart = itr->dwVlanIDStart;
            tRange.dwEnd = itr->dwVlanIDEnd;
            pRangeArith->AddRange(tRange);
        }        
        tRange.dwStart = tVlanRange.dwVlanIDStart;
        tRange.dwEnd = tVlanRange.dwVlanIDEnd;
        pRangeArith->AddRange(tRange);  
        
        pRangeArith->MergeSortColl();
        if(pRangeArith->GetMergedRangeColl().empty())
        {
            VNET_LOG(VNET_LOG_ERROR, "CVlanRange::AddRange: failed to add the vlan Range[%d-%d]!\n", 
                    tVlanRange.dwVlanIDStart, tVlanRange.dwVlanIDEnd);
            return -1;
        }
        
        m_vecVlanRange.clear();
        vector<TRange>::iterator itrRange =  pRangeArith->GetMergedRangeColl().begin();
        for(;itrRange != pRangeArith->GetMergedRangeColl().end(); ++itrRange)
        {
            TVlanRangeItem tVlanRangeItem = {0};
            tVlanRangeItem.dwVlanIDStart = itrRange->dwStart;
            tVlanRangeItem.dwVlanIDEnd = itrRange->dwEnd;
            m_vecVlanRange.push_back(tVlanRangeItem);
        }
        return 0;
    }
    
    int32 CVlanRange::DelRange(const TVlanRangeItem &tVlanRange)
    {
        if(!IsValidVlanRange(tVlanRange))
        {            
            VNET_LOG(VNET_LOG_ERROR, "CVlanRange::DelRange (%d, %d), the vlan range parameter is invalid.\n", 
                tVlanRange.dwVlanIDStart, tVlanRange.dwVlanIDEnd);
            return -1;
        }

        CRangeArithmetic *pRangeArith = CRangeArithmetic::GetInstance();
        if(NULL == pRangeArith)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVlanRange::DelRange: can't get the CRangeArithmetic instance!\n");
            return -1;
        }
        pRangeArith->RemoveAll();
        vector<TVlanRangeItem>::iterator itr = m_vecVlanRange.begin();        
        TRange tRange = {0};
        for(; itr != m_vecVlanRange.end(); ++itr)
        {  
            tRange.dwStart = itr->dwVlanIDStart;
            tRange.dwEnd = itr->dwVlanIDEnd;
            pRangeArith->AddRange(tRange);
        }
        
        tRange.dwStart = tVlanRange.dwVlanIDStart;
        tRange.dwEnd = tVlanRange.dwVlanIDEnd;
        
        if(0 != pRangeArith->DeleteRange(tRange))
        {  
            VNET_LOG(VNET_LOG_ERROR, "CVlanRange::DelRange:Delete the VLAN range(%d, %d) failed.\n", 
                tVlanRange.dwVlanIDStart, tVlanRange.dwVlanIDEnd);
            return -1;
        }
        
        m_vecVlanRange.clear();
        if(pRangeArith->GetMergedRangeColl().empty())
        {  
            return 0;
        }
        
        vector<TRange>::iterator itrRange =  pRangeArith->GetMergedRangeColl().begin();
        for(;itrRange != pRangeArith->GetMergedRangeColl().end(); ++itrRange)
        {
            TVlanRangeItem tVlanRangeItem = {0};
            tVlanRangeItem.dwVlanIDStart = itrRange->dwStart;
            tVlanRangeItem.dwVlanIDEnd = itrRange->dwEnd;
            m_vecVlanRange.push_back(tVlanRangeItem);
        } 
        
        return 0;
    }

    int32 CVlanRange::IsValidVlanRange(const TVlanRangeItem &tVlanRange)
    {
        if ((tVlanRange.dwVlanIDStart < VNET_MIN_VLAN_ID) || (tVlanRange.dwVlanIDStart > VNET_MAX_MAP_VLAN_ID)
            ||(tVlanRange.dwVlanIDEnd < VNET_MIN_VLAN_ID) || (tVlanRange.dwVlanIDEnd > VNET_MAX_MAP_VLAN_ID)
            ||(tVlanRange.dwVlanIDStart > tVlanRange.dwVlanIDEnd))
        {
            return 0;
        }
        
        return 1;
    }  

    void CVlanRange::DbgShowVlanRange(void)
    {
        if(!m_vecVlanRange.empty())
        {   
            cout << "Range Collection: " << endl;            
            vector<TVlanRangeItem>::iterator itr = m_vecVlanRange.begin();
            for(; itr != m_vecVlanRange.end(); ++itr)
            {       
                cout << "{" << itr->dwVlanIDStart << "," 
                     << itr->dwVlanIDEnd <<"}" << endl;      
            }
        } 
        
        return ;
    } 
}// end namespace zte_tecs

