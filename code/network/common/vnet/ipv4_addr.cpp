/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：ipv4_addr.cpp
* 文件标识：
* 内容摘要：CIPv4Addr类的实现文件
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
#include "vnet_comm.h"
#include "ipv4_addr.h"

#define MAX_DIGITS_LENGTH   sizeof("ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255")
#define MAX_PUNCT_LENGTH    sizeof("::::::...")
#define IP_TO_STRING_SIZE   (4)
#define MAX_IP_ADDR_LEN     IPV4_ADDR_LEN
#define PUNCT_NUL           (0x10)
#define PUNCT_COL           (0x11)
#define PUNCT_DOT           (0x12)

namespace zte_tecs
{
    CIPv4Addr::CIPv4Addr()
    {
        memset(&m_tIPAddr, 0x0, sizeof(TIPv4Addr));
    }
    
    CIPv4Addr::~CIPv4Addr()
    {
        memset(&m_tIPAddr, 0x0, sizeof(TIPv4Addr));
    }
       
    int32 CIPv4Addr::Set(const TIPv4Addr &tAddr)
    {
        m_tIPAddr.uIP.dwAddr = tAddr.uIP.dwAddr;
        m_tIPAddr.uMask.dwAddr = tAddr.uMask.dwAddr;
        return 0;
    }

    int32 CIPv4Addr::Set(const string &cstrIP, const string &cstrMask)
    {
        uint32 dwIP = 0;
        uint32 dwMask = 0;
        if(0 != String2IP(cstrIP, dwIP) || 
           0 != String2IP(cstrMask, dwMask))
        {
            return -1;
        }
        
        m_tIPAddr.uIP.dwAddr = dwIP;
        m_tIPAddr.uMask.dwAddr = dwMask;
        
        return 0;
    }

    
    int32 CIPv4Addr::String2IP(const string &cstrIP, uint32 &dwIP)
    {
        if(cstrIP.empty())
        {
            return -1;
        }
        
        char  digits[MAX_DIGITS_LENGTH];
        char  punct[MAX_PUNCT_LENGTH];
        unsigned int ndigits = 0, npunct = 0, idigits = 0, ipunct = 0, ibits = 0;
        uint32 value = 0;
        const char *s = cstrIP.c_str();
        char temp_ip[MAX_IP_ADDR_LEN]={0};         
        memset(digits, 0, sizeof(digits[0]) * MAX_DIGITS_LENGTH);         
        memset(punct, 0, sizeof(punct[0]) * MAX_PUNCT_LENGTH);         

        //IPADDR_ZERO(temp_ip);
        memset((temp_ip), 0,  MAX_IP_ADDR_LEN);    
        /* Skip leading whitespace */
        while ((*s == ' ') || (*s == '\t'))
        {
            s++;
        }
        /*
        * Scan the digits and legitimate punctuation out of the string.
        * Don't assume ASCII, the ISO owns the C specification now.
        */
        do
        {
            VNET_ASSERT(ndigits<MAX_DIGITS_LENGTH);
            switch (*s++)
            {
                case '0': digits[ndigits++] = 0x0; break;
                case '1': digits[ndigits++] = 0x1; break;
                case '2': digits[ndigits++] = 0x2; break;
                case '3': digits[ndigits++] = 0x3; break;
                case '4': digits[ndigits++] = 0x4; break;
                case '5': digits[ndigits++] = 0x5; break;
                case '6': digits[ndigits++] = 0x6; break;
                case '7': digits[ndigits++] = 0x7; break;
                case '8': digits[ndigits++] = 0x8; break;
                case '9': digits[ndigits++] = 0x9; break;
                case '.': 
                    punct[npunct++] = digits[ndigits++] = PUNCT_DOT; break;
                case '\0': 
                    punct[npunct++] = digits[ndigits++] = PUNCT_NUL; 
                    s = 0;
                    break;
                default: 
                    return -1;
            }
            if(ndigits >= (int)(sizeof(digits)/sizeof(*digits)) ||
               npunct >= (int)(sizeof(punct)/sizeof(*punct)))
            {
                return -1;
            }
        } while (s);  
        /*
        * If we get here, we're looking at an IPv4 address, the IPv4
        * compatability-mode tail of an IPv6 address, or an error.
        */    
        if (npunct - ipunct != 4)
        {
            return -1;
        }
        VNET_ASSERT(ipunct<MAX_PUNCT_LENGTH);
        while (punct[ipunct] == PUNCT_DOT || punct[ipunct] == PUNCT_NUL)
        {
            value = 0;
            VNET_ASSERT(idigits<MAX_DIGITS_LENGTH);
            while (digits[idigits] <= 9)
            {
                if ((value = (value * 10) + digits[idigits++]) > 255)
                {
                    return -1;
                }
            }
            VNET_ASSERT(idigits<MAX_DIGITS_LENGTH);
            VNET_ASSERT(ipunct<MAX_PUNCT_LENGTH);
            if (digits[idigits] != punct[ipunct])
            {
                return -1;
            }
            VNET_ASSERT(ibits<MAX_IP_ADDR_LEN);
            (temp_ip)[ibits++] = (uint8) ( value & 0xFF);
            VNET_ASSERT(ipunct<MAX_PUNCT_LENGTH);
            if (punct[ipunct] == PUNCT_NUL)
            {
                memcpy(&dwIP, temp_ip, sizeof(dwIP));
                return 0;
            }
            ipunct++;
            idigits++;
        }       
        return -1;
    }
    
    int32 CIPv4Addr::Get(TIPv4Addr &tAddr) const
    {
        tAddr.uIP.dwAddr = m_tIPAddr.uIP.dwAddr;
        tAddr.uMask.dwAddr = m_tIPAddr.uMask.dwAddr;
        return 0;
    }

    int32 CIPv4Addr::IP2String(string &strIP, string &strMask) const
    {        
        stringstream ss;    
        ss  << (int32)m_tIPAddr.uIP.aucAddr[0] << "."
            << (int32)m_tIPAddr.uIP.aucAddr[1] << "."
            << (int32)m_tIPAddr.uIP.aucAddr[2] << "."
            << (int32)m_tIPAddr.uIP.aucAddr[3];
        strIP.assign(ss.str());
        
        ss.str("");
        ss  << (int32)m_tIPAddr.uMask.aucAddr[0] << "."
            << (int32)m_tIPAddr.uMask.aucAddr[1] << "."
            << (int32)m_tIPAddr.uMask.aucAddr[2] << "."
            << (int32)m_tIPAddr.uMask.aucAddr[3];
        strMask.assign(ss.str());
        
        return 0;
    }

    string CIPv4Addr::GetIPAndMask(void) const
    {
        string strIP;
        stringstream ss;    
        ss  << (int32)m_tIPAddr.uIP.aucAddr[0] << "."
            << (int32)m_tIPAddr.uIP.aucAddr[1] << "."
            << (int32)m_tIPAddr.uIP.aucAddr[2] << "."
            << (int32)m_tIPAddr.uIP.aucAddr[3] << "/"
            << GetMaskPrefixLen();
        strIP.assign(ss.str());
        return strIP;
    }
    
    string CIPv4Addr::GetIP(void) const
    {
        string strIP;
        stringstream ss;    
        ss  << (int32)m_tIPAddr.uIP.aucAddr[0] << "."
            << (int32)m_tIPAddr.uIP.aucAddr[1] << "."
            << (int32)m_tIPAddr.uIP.aucAddr[2] << "."
            << (int32)m_tIPAddr.uIP.aucAddr[3];
        strIP.assign(ss.str());
        return strIP;
    }
    
    string CIPv4Addr::GetMask(void) const
    {
        string strIP;
        stringstream ss;    
        ss  << (int32)m_tIPAddr.uMask.aucAddr[0] << "."
            << (int32)m_tIPAddr.uMask.aucAddr[1] << "."
            << (int32)m_tIPAddr.uMask.aucAddr[2] << "."
            << (int32)m_tIPAddr.uMask.aucAddr[3];
        strIP.assign(ss.str());
        return strIP;
    }

    int32 CIPv4Addr::CheckAddr(int32 nCode, const UNIPv4AddrType &uAddr) const
    {
        int32 i = 0;
        switch(nCode)
        {
        case IPV4_ADDR_ALL_ZEROS:
            {
                for(i = 0; i < IPV4_ADDR_LEN; i++)
                {
                    if(0x00 != uAddr.aucAddr[i])
                    {
                        return 0;
                    }
                }
                return 1;
            }
            break;
        case IPV4_ADDR_ALL_ONES:
            {
                for(i = 0; i < IPV4_ADDR_LEN; i++)
                {
                    if(0xFF != uAddr.aucAddr[i])
                    {
                        return 0;
                    }
                }
                return 1;
            }
            break;
        case IPV4_ADDR_MULTICAST:
            {
                /* A: 1.0.0.0 到126.0.0.0
                 * B: 128.0.0.0到191.255.255.255
                 * C: 192.0.0.0到223.255.255.255
                 * D: 地址用于多点广播（Multicast）。 D类IP地址第一个字节以“lll0”开始
                 * E: 地址以“llll0”开始，为将来使用保留
                 */  
                // 224.0.0.0/4 D:
                return 0xE0 <= (uAddr.aucAddr[0] & 0xF0);
            }
            break;
        case IPV4_ADDR_RESERVED:
            {
                // E:
                return 0xF0 <= (uAddr.aucAddr[0] & 0xF0);
            }
            break;
        case IPV4_ADDR_LOOPBACK:
            {
                return 0x7F == uAddr.aucAddr[0]; // 127.x.x.x
            }
            break;
        case IPV4_ADDR_FIRST_ZERO:
            {
                return 0x00 == uAddr.aucAddr[0];
            }
            break;
        case IPV4_ADDR_LAST_ONES:
            {
                return 0x00 == uAddr.aucAddr[3];
            }
            break;
        default:
            {
                VNET_LOG(VNET_LOG_ERROR, "CIPv4Addr::CheckAddr: Input unknown param(%d)\n",nCode);
            }
            break;
        }
        
        return 0;
    }

    int32 CIPv4Addr::IsValid(void) const
    {
        if(!IsValidMask())
        {
            return 0;
        }
        
        if(!IsValidHostAddr())
        {
            return 0;
        }
        
        /*
         * 主机部分不能为全0, IP的网络部分不能全为1;
         */
        uint32 uiIP = CIPv4Addr::N2HL(m_tIPAddr.uIP.dwAddr);
        uint32 uiMask = CIPv4Addr::N2HL(m_tIPAddr.uMask.dwAddr);        
        if ((0x0 == (uiIP & (~uiMask)))||
            (0xFFFFFFFFL == (uiIP |(~uiMask))))
        {
            return 0;
        }

        // Is broadcast check;
        if((uiIP & (~uiMask)) == (~uiMask))
        {
            return 0;
        }
        
        return 1;
    }
    
    int32 CIPv4Addr::IsValidMask(void) const
    {
        if(CheckAddr(IPV4_ADDR_ALL_ZEROS, m_tIPAddr.uMask) || 
           CheckAddr(IPV4_ADDR_ALL_ONES, m_tIPAddr.uMask))
        {
            return 0;
        }
        
        // 掩码连续1判断
        uint32 uiCheckBit = 0x80000000;
        uint32 uiMask = CIPv4Addr::N2HL(m_tIPAddr.uMask.dwAddr);        
        for ( ; uiMask; )
        {
            if ((uiCheckBit & uiMask) == 0)
            {
                return 0;
            }
            uiMask = uiMask << 1;
        }
        
        return 1;
    }
    
    int32 CIPv4Addr::IsValidHostAddr(void) const
    {
        if(CheckAddr(IPV4_ADDR_ALL_ZEROS, m_tIPAddr.uIP) || 
           CheckAddr(IPV4_ADDR_ALL_ONES, m_tIPAddr.uIP)  ||
           CheckAddr(IPV4_ADDR_LOOPBACK, m_tIPAddr.uIP)  || 
           CheckAddr(IPV4_ADDR_FIRST_ZERO, m_tIPAddr.uIP)||
           CheckAddr(IPV4_ADDR_MULTICAST, m_tIPAddr.uIP) ||
           CheckAddr(IPV4_ADDR_RESERVED, m_tIPAddr.uIP))
        {
            return 0;
        }
        return 1;
    }

    int32 CIPv4Addr::GetMaskPrefixLen(void) const
    {
        uint8 ucPfxlen = 0;
        uint32 dwMask = m_tIPAddr.uMask.dwAddr;

        dwMask = CIPv4Addr::H2NL(dwMask);

        for (ucPfxlen = 0; ucPfxlen < (IPV4_ADDR_LEN * 8); ucPfxlen++)
        {
            if (0 == dwMask)
            {
                break;
            }
            dwMask = dwMask << 1;
        }
        return ucPfxlen;
    }

    int32 CIPv4Addr::IsBigendianHost(void)
    {
        UNIPv4AddrType uAddr;
        uAddr.aucAddr[0] = 1;
        uAddr.aucAddr[1] = 0;
        uAddr.aucAddr[2] = 0;
        uAddr.aucAddr[3] = 0;

        if ( uAddr.dwAddr == 1)
        {
            return 0;
        }

        return 1;
    }
    
    uint32 CIPv4Addr::N2HL(uint32 dwData)
    {
        if(CIPv4Addr::IsBigendianHost())
        {
            return dwData;
        }

        return (uint32)( \
                        ((((uint32)(dwData)) & 0x000000ff) << 24) | \
                        ((((uint32)(dwData)) & 0x0000ff00) <<  8) | \
                        ((((uint32)(dwData)) & 0x00ff0000) >>  8) | \
                        ((((uint32)(dwData)) & 0xff000000) >> 24));

    }
    
    uint32 CIPv4Addr::H2NL(uint32 dwData)
    {
        if(CIPv4Addr::IsBigendianHost())
        {
            return dwData;
        }
       
        return (uint32)( \
                        ((((uint32)(dwData)) & 0x000000ff) << 24) | \
                        ((((uint32)(dwData)) & 0x0000ff00) <<  8) | \
                        ((((uint32)(dwData)) & 0x00ff0000) >>  8) | \
                        ((((uint32)(dwData)) & 0xff000000) >> 24));
    }

    void VNetDbgTestIP(const char *cstrIP, const char *cstrMask)
    {
        if(NULL == cstrIP || cstrMask == NULL)
        {
            cout << "Input param error." << endl;
            return ;
        }

        string strIP;
        string strMask;
        strIP.assign(cstrIP);
        strMask.assign(cstrMask);
        
        CIPv4Addr cIP; 
        
        if(0 != cIP.Set(strIP, strMask))
        {
            cout << strIP << " " << strMask <<" format is invalid." << endl;
            return ;
        }

        if(!cIP.IsValid())
        {
            cout << strIP << " " << strMask <<" is invalid." << endl;
        }
        
        return ;
    }
    DEFINE_DEBUG_FUNC(VNetDbgTestIP);
    
    
}// namespace zte_tecs


