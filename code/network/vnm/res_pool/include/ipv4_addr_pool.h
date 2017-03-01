/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：ipv4_addr_pool.h
* 文件标识：
* 内容摘要：CIPv4AddrPool类的定义文件
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

#if !defined(IPV4_ADDR_POOL_INCLUDE_H__)
#define IPV4_ADDR_POOL_INCLUDE_H__

namespace zte_tecs
{
    typedef enum tagIpv4NetType
    {
        LOGIC_NET = 0,
        NETPLANE,     
    }T_Ipv4NetType;
    
    class CIPv4AddrPool
    {
    public:
        explicit CIPv4AddrPool();
        virtual ~CIPv4AddrPool();

    public:
        int32 Init(const string &cstrNetworkUUID, T_Ipv4NetType tIpv4NetType = LOGIC_NET);
        int32 Init(const string &cstrNetworkUUID, CIPv4AddrRange *pIPv4AddrRange, T_Ipv4NetType tIpv4NetType = LOGIC_NET);
        int32 AllocIPv4Addr(int32 nKey, TIPv4Addr &tItem);
        int32 FreeIPv4Addr(const TIPv4Addr &tItem);

        int32 DbgShowData(void);        
    protected:        
        int32 CalcIPv4Addr(const TIPV4AddrRangeItem &tIPRange, int32 nKey, TIPv4Addr &tItem);
        int32 GetIPv4AddrCnt(const TIPV4AddrRangeItem &tRange, uint32 &dwCnt);
        int32 SaveIPv4Addr2DB(const CIPv4Addr &cIPv4Addr); 
        int32 ScanAllocIP(const TIPV4AddrRangeItem &tIPRange, TIPv4Addr &tItem);
        int32 GetAvailableRange(vector<TIPV4AddrRangeItem> &vecIPv4Range, 
                                vector<TIPv4RangeUsedInfo> &vecAvailableRange);
                                
    private:        
        static bool CompareRange(const TIPv4RangeUsedInfo &tRange1, const TIPv4RangeUsedInfo &tRange2);     
        
    private:
        CIPv4AddrRange    *m_pIPv4AddrRange;
        vector<TIPv4Addr>  m_vecIPv4Addr;
        string             m_strNetworkUUID;
        T_Ipv4NetType      m_tNetType;
        
        DISALLOW_COPY_AND_ASSIGN(CIPv4AddrPool);
    };
}// namespace zte_tecs

#endif

