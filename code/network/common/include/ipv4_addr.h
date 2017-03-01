/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：ipv4_addr.h
* 文件标识：
* 内容摘要：CIPv4Addr类的定义文件
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

#if !defined(IPV4_ADDR_INCLUDE_H__)
#define IPV4_ADDR_INCLUDE_H__

namespace zte_tecs
{
#define IPV4_ADDR_LEN   (4)

    typedef union tagUNIPv4AddrType
    {
        uint32 dwAddr;
        uint8  aucAddr[IPV4_ADDR_LEN]; 
    }UNIPv4AddrType;
    // IP is in NETWORK-BYTE ORDER;
    typedef struct tagIPv4Addr
    {
        UNIPv4AddrType uIP;
        UNIPv4AddrType uMask;
    }TIPv4Addr;

    typedef enum tagENChkIPv4AddrCode
    {
        IPV4_ADDR_ALL_ZEROS = 0,
        IPV4_ADDR_ALL_ONES,
        IPV4_ADDR_MULTICAST,
        IPV4_ADDR_LOOPBACK,
        IPV4_ADDR_FIRST_ZERO,
        IPV4_ADDR_LAST_ONES,
        IPV4_ADDR_RESERVED,
    }ENChkIPv4AddrCode;

    class CIPv4Addr
    {
    public:
        explicit CIPv4Addr();
        virtual ~CIPv4Addr();

    public:      
        int32 Get(TIPv4Addr &tAddr) const;         
        string GetIPAndMask(void) const;
        string GetIP(void) const;
        string GetMask(void) const;
        int32 IP2String(string &strIP, string &strMask) const;
        int32 IsValid(void) const;
        int32 IsValidMask(void) const;   
        int32 IsValidHostAddr(void) const;        
        int32 GetMaskPrefixLen(void) const;
        int32 CheckAddr(int32 nCode, const UNIPv4AddrType &uAddr) const;
        
        int32 Set(const TIPv4Addr &tAddr);
        int32 Set(const string &cstrIP, const string &cstrMask);
        
        static uint32 N2HL(uint32 dwData);
        static uint32 H2NL(uint32 dwData);        
        static int32 IsBigendianHost(void); 
        
    protected:
        int32 String2IP(const string &cstrIP, uint32 &dwIP);

    private:
        
    private:
        TIPv4Addr m_tIPAddr;
        
        DISALLOW_COPY_AND_ASSIGN(CIPv4Addr);
    };
}// namespace zte_tecs

#endif

