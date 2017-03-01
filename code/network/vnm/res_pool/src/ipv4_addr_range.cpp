/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：ipv4_range.cpp
* 文件标识：
* 内容摘要：CIPv4Range类实现文件
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
#include "ipv4_addr.h"
#include "ipv4_addr_range.h"

namespace zte_tecs
{
    CIPv4AddrRange::CIPv4AddrRange()
    {
    }
    
    CIPv4AddrRange::~CIPv4AddrRange()
    {
        //free vector container
        if(!m_vecIPv4AddrRange.empty())
        {
            m_vecIPv4AddrRange.clear();
        }
    }

    // 此接口被从数据库中加载有效的数据时调用;
    int32 CIPv4AddrRange::AddValidRange(const TIPV4AddrRangeItem &tIPv4AddrRange)
    {
        m_vecIPv4AddrRange.push_back(tIPv4AddrRange);
        return 0;
    }

    int32 CIPv4AddrRange::AddRange(const TIPV4AddrRangeItem &tIPv4AddrRange)
    {
        CIPv4Addr cIPStart;
        CIPv4Addr cIPEnd;
        cIPStart.Set(tIPv4AddrRange.tIPStart);
        cIPEnd.Set(tIPv4AddrRange.tIPEnd);
        
        if(!IsValidIPv4Range(tIPv4AddrRange))
        {            
            VNET_LOG(VNET_LOG_ERROR, "CIPv4AddrRange::AddRange (%s, %s), the IP address range parameter is invalid.\n", 
                cIPStart.GetIPAndMask().c_str(), cIPEnd.GetIPAndMask().c_str());
            return -1;
        }

        if(m_vecIPv4AddrRange.empty())
        {
            m_vecIPv4AddrRange.push_back(tIPv4AddrRange);
            return 0;
        }
        
        CRangeArithmetic *pRangeArith = CRangeArithmetic::GetInstance();
        if(NULL == pRangeArith)
        {
            VNET_LOG(VNET_LOG_ERROR, "CIPv4AddrRange::AddRange: can't get the CRangeArithmetic instance!\n");
            return -1;
        }        
        pRangeArith->RemoveAll();
        vector<TIPV4AddrRangeItem>::iterator itr = m_vecIPv4AddrRange.begin();        
        TRange tRange = {0};
        for(; itr != m_vecIPv4AddrRange.end(); ++itr)
        {  
            tRange.dwStart = CIPv4Addr::N2HL(itr->tIPStart.uIP.dwAddr);
            tRange.dwEnd = CIPv4Addr::N2HL(itr->tIPEnd.uIP.dwAddr);
            pRangeArith->AddRange(tRange);
        }        
        tRange.dwStart = CIPv4Addr::N2HL(tIPv4AddrRange.tIPStart.uIP.dwAddr);
        tRange.dwEnd = CIPv4Addr::N2HL(tIPv4AddrRange.tIPEnd.uIP.dwAddr);
        pRangeArith->AddRange(tRange);  
        
        pRangeArith->MergeSortColl();
        if(pRangeArith->GetMergedRangeColl().empty())
        {
            VNET_LOG(VNET_LOG_ERROR, "CIPv4AddrRange::AddRange: failed to add the IP address Range[%s-%s]!\n", 
                    cIPStart.GetIPAndMask().c_str(), cIPEnd.GetIPAndMask().c_str());
            return -1;
        }
        
        m_vecIPv4AddrRange.clear();
        vector<TRange>::iterator itrRange =  pRangeArith->GetMergedRangeColl().begin();
        for(;itrRange != pRangeArith->GetMergedRangeColl().end(); ++itrRange)
        {
            TIPV4AddrRangeItem tIPv4Item;
            tIPv4Item.tIPStart.uIP.dwAddr = CIPv4Addr::H2NL(itrRange->dwStart);
            tIPv4Item.tIPStart.uMask.dwAddr = tIPv4AddrRange.tIPStart.uMask.dwAddr;
            tIPv4Item.tIPEnd.uIP.dwAddr = CIPv4Addr::H2NL(itrRange->dwEnd);            
            tIPv4Item.tIPEnd.uMask.dwAddr = tIPv4AddrRange.tIPEnd.uMask.dwAddr;
            m_vecIPv4AddrRange.push_back(tIPv4Item);
        }
        return 0;
    }
    
    int32 CIPv4AddrRange::DelRange(const TIPV4AddrRangeItem &tIPv4AddrRange)
    {
        CIPv4Addr cIPStart;
        CIPv4Addr cIPEnd;
        cIPStart.Set(tIPv4AddrRange.tIPStart);
        cIPEnd.Set(tIPv4AddrRange.tIPEnd);
        
        if(!IsValidIPv4Range(tIPv4AddrRange))
        {            
            VNET_LOG(VNET_LOG_ERROR, "CIPv4AddrRange::DelRange (%s, %s), the IP address range parameter is invalid.\n", 
                cIPStart.GetIPAndMask().c_str(), cIPEnd.GetIPAndMask().c_str());
            return -1;
        }

        CRangeArithmetic *pRangeArith = CRangeArithmetic::GetInstance();
        if(NULL == pRangeArith)
        {
            VNET_LOG(VNET_LOG_ERROR, "CIPv4AddrRange::DelRange: can't get the CRangeArithmetic instance!\n");
            return -1;
        }
        pRangeArith->RemoveAll();
        vector<TIPV4AddrRangeItem>::iterator itr = m_vecIPv4AddrRange.begin();        
        TRange tRange = {0};
        for(; itr != m_vecIPv4AddrRange.end(); ++itr)
        {  
            tRange.dwStart = CIPv4Addr::N2HL(itr->tIPStart.uIP.dwAddr);
            tRange.dwEnd = CIPv4Addr::N2HL(itr->tIPEnd.uIP.dwAddr);
            pRangeArith->AddRange(tRange);
        }
        
        tRange.dwStart = CIPv4Addr::N2HL(tIPv4AddrRange.tIPStart.uIP.dwAddr);
        tRange.dwEnd = CIPv4Addr::N2HL(tIPv4AddrRange.tIPEnd.uIP.dwAddr);

        if(0 != pRangeArith->DeleteRange(tRange))
        {  
            VNET_LOG(VNET_LOG_ERROR, "CIPv4AddrRange::DelRange:Delete IP Range(%s, %s) failed.\n", 
                cIPStart.GetIPAndMask().c_str(), cIPEnd.GetIPAndMask().c_str());
            return -1;
        }        
        
        m_vecIPv4AddrRange.clear();
        if(pRangeArith->GetMergedRangeColl().empty())
        {  
            return 0;
        }
        
        vector<TRange>::iterator itrRange =  pRangeArith->GetMergedRangeColl().begin();
        for(;itrRange != pRangeArith->GetMergedRangeColl().end(); ++itrRange)
        {
            TIPV4AddrRangeItem tIPv4Item;
            tIPv4Item.tIPStart.uIP.dwAddr = CIPv4Addr::H2NL(itrRange->dwStart);
            tIPv4Item.tIPStart.uMask.dwAddr = tIPv4AddrRange.tIPStart.uMask.dwAddr;
            tIPv4Item.tIPEnd.uIP.dwAddr = CIPv4Addr::H2NL(itrRange->dwEnd);            
            tIPv4Item.tIPEnd.uMask.dwAddr = tIPv4AddrRange.tIPEnd.uMask.dwAddr;
            m_vecIPv4AddrRange.push_back(tIPv4Item);
        } 

        return 0;
    }

    int32 CIPv4AddrRange::IsValidIPv4Range(const TIPV4AddrRangeItem &tIPv4AddrRange)
    {
        CIPv4Addr cIPStart;
        cIPStart.Set(tIPv4AddrRange.tIPStart);
        if(!cIPStart.IsValid())
        {
            return 0;
        }

        CIPv4Addr cIPEnd;
        cIPEnd.Set(tIPv4AddrRange.tIPEnd);
        if(!cIPEnd.IsValid())
        {
            return 0;
        }

        if(cIPStart.GetMaskPrefixLen() != cIPEnd.GetMaskPrefixLen())
        {
            return 0;
        }
        
        uint32 dwStart = CIPv4Addr::N2HL(tIPv4AddrRange.tIPStart.uIP.dwAddr);
        uint32 dwEnd = CIPv4Addr::N2HL(tIPv4AddrRange.tIPEnd.uIP.dwAddr);
        if(dwStart > dwEnd)
        {
            return 0;
        }
        
        return 1;
    }  

    int32 CIPv4AddrRange::IsSameSubnet(const TIPV4AddrRangeItem &tIPv4AddrRange)
    {
        TIPv4Addr tStartSubNet;
        tStartSubNet.uIP.dwAddr = tIPv4AddrRange.tIPStart.uIP.dwAddr & tIPv4AddrRange.tIPStart.uMask.dwAddr;
        tStartSubNet.uMask.dwAddr = tIPv4AddrRange.tIPStart.uMask.dwAddr;
        TIPv4Addr tEndSubNet;
        tEndSubNet.uIP.dwAddr = tIPv4AddrRange.tIPEnd.uIP.dwAddr & tIPv4AddrRange.tIPEnd.uMask.dwAddr;
        tEndSubNet.uMask.dwAddr = tIPv4AddrRange.tIPEnd.uMask.dwAddr;
        
        if(tStartSubNet.uIP.dwAddr == tEndSubNet.uIP.dwAddr)
        {
            return 1;
        }
        return 0;
    }

    void CIPv4AddrRange::DbgShowIPv4AddrRange(void)
    {
        if(!m_vecIPv4AddrRange.empty())
        {   
            cout << "Range Collection: " << endl;            
            vector<TIPV4AddrRangeItem>::iterator itr = m_vecIPv4AddrRange.begin();
            for(; itr != m_vecIPv4AddrRange.end(); ++itr)
            {  
                CIPv4Addr cIPStart;
                CIPv4Addr cIPEnd;
                cIPStart.Set(itr->tIPStart);
                cIPEnd.Set(itr->tIPEnd);
                cout << "{" << cIPStart.GetIPAndMask() << "," 
                     << cIPEnd.GetIPAndMask() <<"}" << endl;      
            }
        } 
        
        return ;
    } 
}// end namespace zte_tecs

