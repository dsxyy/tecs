/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：mac_addr.h
* 文件标识：
* 内容摘要：CMACAddr类的定义文件
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

#if !defined(MAC_ADDR_INCLUDE_H__)
#define MAC_ADDR_INCLUDE_H__

namespace zte_tecs
{
    typedef struct tagUInt32MAC
    {
        uint32 dwOUI;   /* MAC0~MAC2,最高字节为00; */
        uint32 dwData;  /* MAC3~MAC5,最高字节为00; */
    }TUInt32MAC;

    typedef struct tagByteMAC
    {
        uint8 aucMAC[VNET_MAC_LEN];
    }TByteMAC;
    
    class CMACAddr
    {
    public:
        explicit CMACAddr();
        virtual ~CMACAddr();

    public:      
        int32 GetByteMAC(uint8 aucMAC[], uint8 ucMACLen = VNET_MAC_LEN) const;         
        int32 GetUInt32MAC(TUInt32MAC &tUInt32MAC) const;
        string GetStringMAC(void) const;
       
        int32 Set(const string &cstrMAC);
        int32 Set(uint8 aucMAC[], uint8 ucMACLen = VNET_MAC_LEN);
        int32 Set(const TUInt32MAC &tUInt32MAC);

        int32 IsValidOUI(void) const;
        
        static uint32 N2HL(uint32 dwData);
        static uint32 H2NL(uint32 dwData);        
        static int32 IsBigendianHost(void); 
        static uint32 GetZTEMACOUI1(void);
        static uint32 GetZTEMACOUI2(void);
        
    protected:
        int32 String2MAC(const string &cstrMAC, uint8 aucMac[]);        
        int32 Uint32MAC2MAC(const TUInt32MAC &tUInt32MAC, uint8 *pMAC);        
        int32 MAC2Uint32MAC(TUInt32MAC &tUInt32MAC) const;
        
    private:
        
    private:
        TByteMAC m_tMACAddr;

        DISALLOW_COPY_AND_ASSIGN(CMACAddr);
    };
}// namespace zte_tecs

#endif

