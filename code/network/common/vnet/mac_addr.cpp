/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：mac_addr.cpp
* 文件标识：
* 内容摘要：CMACAddr类的实现文件
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
#include "mac_addr.h"

#define MAX_MAC_DIGITS_LENGTH   sizeof("ff:ff:ff:ff:ff:ff...")
#define MAX_MAC_PUNCT_LENGTH    sizeof(":::::...")
#define MAC_PUNCT_NUL           (0x10)
#define MAC_PUNCT_DOT           (0x11)

const uint32 c_dwZTEOUI1 = ((0x00 << 16) | (0xd0 << 8) | (0xd0)) & 0x00FFFFFF;
const uint32 c_dwZTEOUI2 = ((0x00 << 16) | (0x19 << 8) | (0xc6)) & 0x00FFFFFF;

namespace zte_tecs
{
    CMACAddr::CMACAddr()
    {
        memset(&m_tMACAddr, 0x0, sizeof(m_tMACAddr));
    }
    
    CMACAddr::~CMACAddr()
    {
        memset(&m_tMACAddr, 0x0, sizeof(m_tMACAddr));
    }

    int32 CMACAddr::GetByteMAC(uint8 aucMAC[], uint8 ucMACLen) const
    {
        if(VNET_MAC_LEN != ucMACLen)
        {
            VNET_ASSERT(0);
            return -1;
        }
        
        memcpy(aucMAC, m_tMACAddr.aucMAC, sizeof(m_tMACAddr.aucMAC));
        return 0;
    }

    string CMACAddr::GetStringMAC(void) const
    {
        stringstream ss;    
        ss  << hex << setfill('0') << setw(2) << (int32)m_tMACAddr.aucMAC[0] << ":"
            << hex << setfill('0') << setw(2) << (int32)m_tMACAddr.aucMAC[1] << ":"
            << hex << setfill('0') << setw(2) << (int32)m_tMACAddr.aucMAC[2] << ":"
            << hex << setfill('0') << setw(2) << (int32)m_tMACAddr.aucMAC[3] << ":"
            << hex << setfill('0') << setw(2) << (int32)m_tMACAddr.aucMAC[4] << ":"
            << hex << setfill('0') << setw(2) << (int32)m_tMACAddr.aucMAC[5];

        return ss.str();
    }
    
    int32 CMACAddr::GetUInt32MAC(TUInt32MAC &tUInt32MAC) const
    {
        return MAC2Uint32MAC(tUInt32MAC);
    }

    int32 CMACAddr::Set(const string &cstrMAC)
    {
        if(cstrMAC.empty())
        {
            VNET_ASSERT(0);
            return -1;
        }

        return String2MAC(cstrMAC, m_tMACAddr.aucMAC);        
    }
    
    int32 CMACAddr::Set(uint8 aucMAC[], uint8 ucMACLen)
    {
        if(VNET_MAC_LEN != ucMACLen)
        {
            VNET_ASSERT(0);
            return -1;
        }
        
        memcpy(m_tMACAddr.aucMAC, aucMAC, sizeof(m_tMACAddr.aucMAC));
        return 0;
    }
    
    int32 CMACAddr::Set(const TUInt32MAC &tUInt32MAC)
    {
        return Uint32MAC2MAC(tUInt32MAC, m_tMACAddr.aucMAC);
    }

    int32 CMACAddr::IsValidOUI(void) const
    {
        TUInt32MAC tMAC = {0};
        MAC2Uint32MAC(tMAC);
        
        if(c_dwZTEOUI1 != tMAC.dwOUI && 
           c_dwZTEOUI2 != tMAC.dwOUI)
        {
            return 0; 
        }
        
        return 1;
    }

    uint32 CMACAddr::GetZTEMACOUI1(void)
    {
        return c_dwZTEOUI1;
    }
    
    uint32 CMACAddr::GetZTEMACOUI2(void)
    {
        return c_dwZTEOUI2;
    }

    int32 CMACAddr::String2MAC(const string &cstrMAC, unsigned char aucMac[])
    {
        if(cstrMAC.empty())
        {
            return -1;
        }
        
        string strMAC = cstrMAC;
        transform(strMAC.begin(), strMAC.end(), strMAC.begin(), ::tolower);  
        const char *s = strMAC.c_str();
        char  digits[MAX_MAC_DIGITS_LENGTH];
        char  punct[MAX_MAC_PUNCT_LENGTH];
        memset(digits, 0, sizeof(digits[0]) * MAX_MAC_DIGITS_LENGTH);         
        memset(punct, 0, sizeof(punct[0]) * MAX_MAC_PUNCT_LENGTH);       
        
        /* Skip leading whitespace */
        while ((*s == ' ') || (*s == '\t'))
        {
            s++;
        }
        /*
        * Scan the digits and legitimate punctuation out of the string.
        * Don't assume ASCII, the ISO owns the C specification now.
        */
        unsigned int ndigits = 0;
        unsigned int npunct = 0;
        unsigned int idigits = 0;
        unsigned int ipunct = 0;
        unsigned int ibits = 0;
        
        do
        {
            VNET_ASSERT(ndigits<MAX_MAC_DIGITS_LENGTH);
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
            case 'a': digits[ndigits++] = 0xa; break;
            case 'b': digits[ndigits++] = 0xb; break;
            case 'c': digits[ndigits++] = 0xc; break;
            case 'd': digits[ndigits++] = 0xd; break;
            case 'e': digits[ndigits++] = 0xe; break;
            case 'f': digits[ndigits++] = 0xf; break;
            case ':': punct[npunct++] = digits[ndigits++] = MAC_PUNCT_DOT; break;
            case '\0':
                {
                    punct[npunct++] = digits[ndigits++] = MAC_PUNCT_NUL; 
                    s = NULL;
                }
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
        if ((npunct - ipunct) != VNET_MAC_LEN)
        {
            return -1;
        }
        
        VNET_ASSERT(ipunct<MAX_MAC_PUNCT_LENGTH);
        while (punct[ipunct] == MAC_PUNCT_DOT || punct[ipunct] == MAC_PUNCT_NUL)
        {
            byte bytData = 0;
            VNET_ASSERT(idigits<MAX_MAC_DIGITS_LENGTH);
            int32 nMACBits = 0;
            while (digits[idigits] <= 0xf)
            {
                bytData = (bytData << 4) | digits[idigits++];
                ++nMACBits;
            }
            if(nMACBits > 2)
            {
                return -1;
            }
            VNET_ASSERT(idigits<MAX_MAC_DIGITS_LENGTH);
            VNET_ASSERT(ipunct<MAX_MAC_PUNCT_LENGTH);
            if (digits[idigits] != punct[ipunct])
            {
                return -1;
            }
            VNET_ASSERT(ibits < VNET_MAC_LEN);
            aucMac[ibits++] = bytData;
            VNET_ASSERT(ipunct < MAX_MAC_PUNCT_LENGTH);
            if (punct[ipunct] == MAC_PUNCT_NUL)
            {
                return 0;
            }
            ipunct++;
            idigits++;
        }       
        return -1;
    }

    int32 CMACAddr::Uint32MAC2MAC(const TUInt32MAC &tUInt32MAC, uint8 *pMAC)
    {
        if(NULL == pMAC)
        {
            VNET_ASSERT(0);
            return -1;
        }
        pMAC[0] = (uint8)((tUInt32MAC.dwOUI >> 16) & 0xFF);
        pMAC[1] = (uint8)((tUInt32MAC.dwOUI >> 8) & 0xFF);
        pMAC[2] = (uint8)(tUInt32MAC.dwOUI & 0xFF);
        pMAC[3] = (uint8)((tUInt32MAC.dwData >> 16) & 0xFF);
        pMAC[4] = (uint8)((tUInt32MAC.dwData >> 8) & 0xFF);
        pMAC[5] = (uint8)(tUInt32MAC.dwData& 0xFF);
        
        return 0;
    }

    int32 CMACAddr::MAC2Uint32MAC(TUInt32MAC &tUInt32MAC) const
    {
        tUInt32MAC.dwOUI = ((m_tMACAddr.aucMAC[0] << 16) | (m_tMACAddr.aucMAC[1] << 8) | (m_tMACAddr.aucMAC[2])) & 0x00FFFFFF;                            
        tUInt32MAC.dwData = ((m_tMACAddr.aucMAC[3] << 16) |(m_tMACAddr.aucMAC[4] << 8) | (m_tMACAddr.aucMAC[5])) & 0x00FFFFFF;
        
        return 0;
    }  

    int32 CMACAddr::IsBigendianHost(void)
    {
        typedef union tagUNInt32
        {
            uint32 dwAddr;
            uint8  aucAddr[4]; 
        }UNInt32Type;
        UNInt32Type uAddr;
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
    
    uint32 CMACAddr::N2HL(uint32 dwData)
    {
        if(CMACAddr::IsBigendianHost())
        {
            return dwData;
        }

        return (uint32)( \
                        ((((uint32)(dwData)) & 0x000000ff) << 24) | \
                        ((((uint32)(dwData)) & 0x0000ff00) <<  8) | \
                        ((((uint32)(dwData)) & 0x00ff0000) >>  8) | \
                        ((((uint32)(dwData)) & 0xff000000) >> 24));

    }
    
    uint32 CMACAddr::H2NL(uint32 dwData)
    {
        if(CMACAddr::IsBigendianHost())
        {
            return dwData;
        }
       
        return (uint32)( \
                        ((((uint32)(dwData)) & 0x000000ff) << 24) | \
                        ((((uint32)(dwData)) & 0x0000ff00) <<  8) | \
                        ((((uint32)(dwData)) & 0x00ff0000) >>  8) | \
                        ((((uint32)(dwData)) & 0xff000000) >> 24));
    }
    
    
}// namespace zte_tecs


