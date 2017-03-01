/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�ipv4_addr_pool.h
* �ļ���ʶ��
* ����ժҪ��CIPv4AddrPool��Ķ����ļ�
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

