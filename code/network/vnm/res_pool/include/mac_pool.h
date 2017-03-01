/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�mac_pool.h
* �ļ���ʶ��
* ����ժҪ��CMACPool��Ķ����ļ�
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

