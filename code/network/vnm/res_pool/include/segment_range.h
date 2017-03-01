/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：segment_range.h
* 文件标识：
* 内容摘要：CSegmentRange类的定义文件
* 当前版本：1.0
* 作    者：
* 完成日期：2013年09月05日
*
* 修改记录1：
*     修改日期：2013/09/05
*     版 本 号：V1.0
*     修 改 人：zhao.jin
*     修改内容：创建
*
******************************************************************************/

#if !defined(SEGMENT_RANGE_INCLUDE_H__)
#define SEGMENT_RANGE_INCLUDE_H__

namespace zte_tecs
{
    typedef struct tagSegmentRangeItem
    {
        uint32 dwSegmentIDStart;
        uint32 dwSegmentIDEnd;
    }TSegmentRangeItem;

    class CSegmentRange
    {
    public:
        explicit CSegmentRange();
        virtual ~CSegmentRange();

    public:  
        int32 AddValidRange(const TSegmentRangeItem &tSegmentRange);
        int32 AddRange(const TSegmentRangeItem &tSegmentRange);
        int32 DelRange(const TSegmentRangeItem &tSegmentRange);  
        int32 RemoveAll();
       
        vector<TSegmentRangeItem> &GetSegmentRange(void) {return m_vecSegmentRange;}       
        
        void  DbgShowSegmentRange(void);
        
    protected:
        int32 IsValidSegmentRange(const TSegmentRangeItem &tSegmentRange);
        
    private:
        
    private:
        vector<TSegmentRangeItem> m_vecSegmentRange;
        DISALLOW_COPY_AND_ASSIGN(CSegmentRange);
    };
}// namespace zte_tecs

#endif

