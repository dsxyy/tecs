/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�ipv4_addr_range.h
* �ļ���ʶ��
* ����ժҪ��CIPv4Range��Ķ����ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�
* ������ڣ�2012��1��18��
*
* �޸ļ�¼1��
*     �޸����ڣ�2013/1/18
*     �� �� �ţ�V1.0
*     �� �� �ˣ�Wang.Lule
*     �޸����ݣ�����
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

