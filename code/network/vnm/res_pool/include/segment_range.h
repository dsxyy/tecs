/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�segment_range.h
* �ļ���ʶ��
* ����ժҪ��CSegmentRange��Ķ����ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�
* ������ڣ�2013��09��05��
*
* �޸ļ�¼1��
*     �޸����ڣ�2013/09/05
*     �� �� �ţ�V1.0
*     �� �� �ˣ�zhao.jin
*     �޸����ݣ�����
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

