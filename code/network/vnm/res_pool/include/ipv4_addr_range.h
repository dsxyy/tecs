/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：ipv4_addr_range.h
* 文件标识：
* 内容摘要：CIPv4Range类的定义文件
* 当前版本：1.0
* 作    者：
* 完成日期：2012年1月18日
*
* 修改记录1：
*     修改日期：2013/1/18
*     版 本 号：V1.0
*     修 改 人：Wang.Lule
*     修改内容：创建
*
******************************************************************************/

#if !defined(IPV4_ADDR_RANGE_INCLUDE_H__)
#define IPV4_ADDR_RANGE_INCLUDE_H__

#include "ipv4_addr.h"

namespace zte_tecs
{
    typedef struct tagIPv4AddrRangeItem
    {
        TIPv4Addr tIPStart;
        TIPv4Addr tIPEnd;
    }TIPV4AddrRangeItem;

    typedef struct tagIPv4RangeUsedInfo
    {
        TIPV4AddrRangeItem tRange;
        uint32 nUsedCnt;
        uint32 nFreeCnt;
    }TIPv4RangeUsedInfo; 

    class CIPv4AddrRange
    {
    public:
        explicit CIPv4AddrRange();
        virtual ~CIPv4AddrRange();

    public:
        int32 AddValidRange(const TIPV4AddrRangeItem &tIPv4AddrRange);
        int32 AddRange(const TIPV4AddrRangeItem &tIPv4AddrRange);
        int32 DelRange(const TIPV4AddrRangeItem &tIPv4AddrRange);  
      
        static int32 IsValidIPv4Range(const TIPV4AddrRangeItem &tIPv4AddrRange);
        static int32 IsSameSubnet(const TIPV4AddrRangeItem &tIPv4AddrRange);
       
        vector<TIPV4AddrRangeItem> &GetIPv4AddrRange(void) {return m_vecIPv4AddrRange;}       
        
        void  DbgShowIPv4AddrRange(void);
        
    private:
        vector<TIPV4AddrRangeItem> m_vecIPv4AddrRange;
        DISALLOW_COPY_AND_ASSIGN(CIPv4AddrRange);
    };
}// namespace zte_tecs

#endif

