/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�mac_range.h
* �ļ���ʶ��
* ����ժҪ��CMACRange��Ķ����ļ�
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

#if !defined(MAC_RANGE_INCLUDE_H__)
#define MAC_RANGE_INCLUDE_H__

namespace zte_tecs
{
    typedef struct tagMACRangeItem
    {
        TUInt32MAC tStartMAC;
        TUInt32MAC tEndMAC;
    }TMACRangeItem;

    typedef struct tagMACRangeUsedInfo
    {
        TMACRangeItem tRange;
        uint32 nUsedCnt;
        uint32 nFreeCnt;
    }TMACRangeUsedInfo; 
    
    class CMACRange
    {
    public:
        explicit CMACRange();
        virtual ~CMACRange();

    public: 
        int32 Init(uint32 dwOUI);
        int32 GetMACOUI(void) const;
        int32 AddValidRange(const TMACRangeItem &tMACRange);
        int32 AddRange(const TMACRangeItem &tMACRange);
        int32 DelRange(const TMACRangeItem &tMACRange);  
        static int32 IsValidMACRange(const TMACRangeItem &tMACRange);
       
        vector<TMACRangeItem> &GetMACRange(void) {return m_vecMACRange;}        
        void  DbgShowMACRange(void);
        
    protected:
        
    private:
        
    private:
        uint32 m_dwOUI;
        vector<TMACRangeItem> m_vecMACRange;
        
        DISALLOW_COPY_AND_ASSIGN(CMACRange);
    };
}// namespace zte_tecs

#endif

