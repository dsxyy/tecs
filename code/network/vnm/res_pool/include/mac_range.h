/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：mac_range.h
* 文件标识：
* 内容摘要：CMACRange类的定义文件
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

