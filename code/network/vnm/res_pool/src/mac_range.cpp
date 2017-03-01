/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：mac_range.cpp
* 文件标识：
* 内容摘要：CMACRange类实现文件
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
#include "mac_addr.h"
#include "mac_range.h"

namespace zte_tecs
{
    CMACRange::CMACRange()
    {
    }
    
    CMACRange::~CMACRange()
    {
        //free vector container
        if(!m_vecMACRange.empty())
        {
            m_vecMACRange.clear();
        }
    }

    int32 CMACRange::Init(uint32 dwOUI)
    {
        if(CMACAddr::GetZTEMACOUI1() != dwOUI &&
           CMACAddr::GetZTEMACOUI2() != dwOUI)
        {
            VNET_LOG(VNET_LOG_ERROR, "CMACRange::Init: The MAC Range OUI[0x%x] is invalid.\n",
                    dwOUI);
            return -1;
        }
        
        m_dwOUI = dwOUI;
        return 0;
    }
    
    int32 CMACRange::GetMACOUI(void) const
    {
        return m_dwOUI;
    }
    
    // 此接口被从数据库中加载有效的数据时调用;
    int32 CMACRange::AddValidRange(const TMACRangeItem &tMACRange)
    {
        if(tMACRange.tStartMAC.dwOUI != m_dwOUI ||
           tMACRange.tEndMAC.dwOUI != m_dwOUI)
        {
            VNET_LOG(VNET_LOG_ERROR, "CMACRange::AddValidRange: The DB MAC OUI[0x%x:0x%x] and Local OUI[0x%x] is not same.\n",
                    tMACRange.tStartMAC.dwOUI, tMACRange.tEndMAC.dwOUI, m_dwOUI);
            VNET_ASSERT(0);
            return -1;
        }        
        m_vecMACRange.push_back(tMACRange);
        return 0;
    }
    
    int32 CMACRange::AddRange(const TMACRangeItem &tMACRange)
    {
        if(tMACRange.tStartMAC.dwOUI != m_dwOUI ||
           tMACRange.tEndMAC.dwOUI != m_dwOUI)
        {
            VNET_LOG(VNET_LOG_ERROR, "CMACRange::AddRange: The added MAC OUI[0x%x:0x%x] and Local OUI[0x%x] is not same.\n",
                    tMACRange.tStartMAC.dwOUI, tMACRange.tEndMAC.dwOUI, m_dwOUI);
            return -1;
        }
        
        if(m_vecMACRange.empty())
        {
            m_vecMACRange.push_back(tMACRange);
        }
        else
        {
            CRangeArithmetic *pRangeArith = CRangeArithmetic::GetInstance();
            if(NULL == pRangeArith)
            {
                VNET_LOG(VNET_LOG_ERROR, "CMACRange::AddRange: can't get the CRangeArithmetic instance!\n");
                return -1;
            }
            pRangeArith->RemoveAll();
            vector<TMACRangeItem>::iterator itrMAC = m_vecMACRange.begin();
            TRange tRange = {0};
            for(; itrMAC != m_vecMACRange.end(); ++itrMAC)
            { 
                tRange.dwStart = itrMAC->tStartMAC.dwData;
                tRange.dwEnd = itrMAC->tEndMAC.dwData;
                pRangeArith->AddRange(tRange);
            }
            tRange.dwStart = tMACRange.tStartMAC.dwData;
            tRange.dwEnd = tMACRange.tEndMAC.dwData;
            pRangeArith->AddRange(tRange);
            pRangeArith->MergeSortColl();
            if(pRangeArith->GetMergedRangeColl().empty())
            { 
                CMACAddr cMACStart;
                CMACAddr cMACEnd;
                cMACStart.Set(tMACRange.tStartMAC);
                cMACEnd.Set(tMACRange.tEndMAC);
                VNET_LOG(VNET_LOG_ERROR, "CMACRange::AddRange: failed to add the MAC Range[%s-%s]!\n", 
                    cMACStart.GetStringMAC().c_str(),
                    cMACStart.GetStringMAC().c_str());
                return -1;
            }
            
            m_vecMACRange.clear();
            vector<TRange>::iterator itrRange =  pRangeArith->GetMergedRangeColl().begin();            
            TUInt32MAC tUInt32MAC = {0};
            tUInt32MAC.dwOUI = m_dwOUI;
            for(;itrRange != pRangeArith->GetMergedRangeColl().end(); ++itrRange)
            {
                TMACRangeItem tItem;
                tUInt32MAC.dwData = itrRange->dwStart;
                tItem.tStartMAC = tUInt32MAC;
                tUInt32MAC.dwData = itrRange->dwEnd;
                tItem.tEndMAC = tUInt32MAC;
                m_vecMACRange.push_back(tItem);
            }
        }
        
        return 0;
    }
    
    int32 CMACRange::DelRange(const TMACRangeItem &tMACRange)
    {
        if(tMACRange.tStartMAC.dwOUI != m_dwOUI || 
           tMACRange.tEndMAC.dwOUI != m_dwOUI)
        {
            VNET_LOG(VNET_LOG_ERROR, "CMACRange::DelRange: The added MAC OUI[0x%x:0x%x] and Local OUI[0x%x] is not same.\n",
                    tMACRange.tStartMAC.dwOUI, tMACRange.tEndMAC.dwOUI, m_dwOUI);
            return -1;
        }

        if(m_vecMACRange.empty())
        {
            VNET_LOG(VNET_LOG_ERROR, "CMACRange::DelRange: no record to be deleted!\n");
            return -1;
        }

        CRangeArithmetic *pRangeArith = CRangeArithmetic::GetInstance();
        if(NULL == pRangeArith)
        {
            VNET_LOG(VNET_LOG_ERROR, "CMACRange::DelRange: can't get the CRangeArithmetic instance!\n");
            return -1;
        }
        pRangeArith->RemoveAll();
        vector<TMACRangeItem>::iterator itrMAC = m_vecMACRange.begin();
        TRange tRange = {0};
        for(; itrMAC != m_vecMACRange.end(); ++itrMAC)
        { 
            tRange.dwStart = itrMAC->tStartMAC.dwData;
            tRange.dwEnd = itrMAC->tEndMAC.dwData;
            pRangeArith->AddRange(tRange);
        }
        tRange.dwStart = tMACRange.tStartMAC.dwData;
        tRange.dwEnd = tMACRange.tEndMAC.dwData;
        if(0 == pRangeArith->DeleteRange(tRange))
        {
            m_vecMACRange.clear();            
            if(pRangeArith->GetMergedRangeColl().empty())
            {             
                return 0;
            }         
            TMACRangeItem tItem; 
            TUInt32MAC tUInt32MAC = {0};
            vector<TRange>::iterator itrRange =  pRangeArith->GetMergedRangeColl().begin();
            tUInt32MAC.dwOUI = m_dwOUI;
            for(;itrRange != pRangeArith->GetMergedRangeColl().end(); ++itrRange)
            {
                tUInt32MAC.dwData = itrRange->dwStart;
                tItem.tStartMAC = tUInt32MAC;
                tUInt32MAC.dwData = itrRange->dwEnd;
                tItem.tEndMAC = tUInt32MAC;
                m_vecMACRange.push_back(tItem);
            }
        }
        else
        {        
            CMACAddr cMACStart;
            CMACAddr cMACEnd;
            cMACStart.Set(tMACRange.tStartMAC);
            cMACEnd.Set(tMACRange.tEndMAC);
            VNET_LOG(VNET_LOG_ERROR, "CMACRange::DelRange: failed to add the MAC Range[%s-%s]!\n", 
                cMACStart.GetStringMAC().c_str(),
                cMACStart.GetStringMAC().c_str());
            return -1;
        }
        return 0;
    } 

    int32 CMACRange::IsValidMACRange(const TMACRangeItem &tMACRange)
    {
        CMACAddr cMACStart;
        CMACAddr cMACEnd;
        cMACStart.Set(tMACRange.tStartMAC);
        cMACEnd.Set(tMACRange.tEndMAC);
        if(!cMACStart.IsValidOUI())
        {
            return 0;
        }
        if(!cMACEnd.IsValidOUI())
        {
            return 0;
        }
        
        if(tMACRange.tStartMAC.dwOUI != tMACRange.tEndMAC.dwOUI)
        {
            return 0;
        } 
        uint32 dwTmp = tMACRange.tStartMAC.dwData & 0x00FFFFFF;
        if(0 == dwTmp || 0x00FFFFFF == dwTmp)
        {
            return 0;
        }
        
        dwTmp = tMACRange.tEndMAC.dwData & 0x00FFFFFF;
        if(0 == dwTmp || 0x00FFFFFF == dwTmp)
        {
            return 0;
        }        

        if(tMACRange.tStartMAC.dwData > tMACRange.tEndMAC.dwData)
        {
            return 0;
        }
        
        return 1;
    }   

    void CMACRange::DbgShowMACRange(void)
    {
        vector<TMACRangeItem>::iterator itrMAC = (vector<TMACRangeItem>::iterator)NULL;

        CMACAddr cMACStart;
        CMACAddr cMACEnd;
        if(!m_vecMACRange.empty())
        {   
            cout << "Range Collection: " << endl;
            for(itrMAC = m_vecMACRange.begin(); itrMAC != m_vecMACRange.end(); ++itrMAC)
            { 
                cMACStart.Set(itrMAC->tStartMAC);
                cMACEnd.Set(itrMAC->tEndMAC);
                cout << "{" << cMACStart.GetStringMAC() << "," 
                     << cMACEnd.GetStringMAC() <<"}" << endl;      
            }
        }     
        return ;
    }     

}// end namespace zte_tecs

