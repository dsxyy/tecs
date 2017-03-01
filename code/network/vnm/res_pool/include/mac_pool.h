/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：mac_pool.h
* 文件标识：
* 内容摘要：CMACPool类的定义文件
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

#if !defined(MAC_POOL_INCLUDE_H__)
#define MAC_POOL_INCLUDE_H__

typedef struct tagMACItem
{
    uint8 aucMAC[VNET_MAC_LEN];
}TMACItem;

namespace zte_tecs
{
    class CMACPool
    {
    public:
        explicit CMACPool();
        virtual ~CMACPool();

    public:
        int32 Init(const string &cstrNetplaneUUID);
        int32 Init(const string &cstrNetplaneUUID, CMACRange *pOUI1MACRange, CMACRange *pOUI2MACRange);
        int32 AllocMAC(int32 nKey, string &strMAC);
        int32 FreeMAC(const string &strMAC);

        int32 DbgShowData(void);

    private:
        int32 CalcMACAddr(int32 nKey, uint32 dwMACEnd, uint32 dwMACStart, uint32 &dwMACKey);
        int32 GetMACCnt(const TMACRangeItem &tRange, uint32 &dwCnt);
        int32 SaveMAC2DB(const CMACAddr &cMACAddr);
        int32 AllocMACAtRange(vector<TMACRangeItem> &vecMACRange, 
                              uint32 dwOUI, int32 nKey, string &strMAC);
        static bool CompareRange(const TMACRangeUsedInfo &tRange1, 
                                 const TMACRangeUsedInfo &tRange2);
        int32 GetAvailableRange(vector<TMACRangeItem> &vecMACRange, 
                                vector<TMACRangeUsedInfo> &vecAvailableRange);
        int32 ScanAllocMAC(uint32 dwOUI, uint32 dwMACStart, uint32 dwMACEnd, TUInt32MAC &tMAC);
        
    private:
        CMACRange *m_pOUI1MACRange;
        CMACRange *m_pOUI2MACRange;
        string m_strNetplaneUUID;
        DISALLOW_COPY_AND_ASSIGN(CMACPool);
    };
}// namespace zte_tecs

#endif

