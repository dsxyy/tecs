/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：range_arithm.h
* 文件标识：
* 内容摘要：CRangeArithmetic类的定义文件
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

#if !defined(RANGE_ARITHM_INCLUDE_H__)
#define RANGE_ARITHM_INCLUDE_H__

namespace zte_tecs
{
#define UINT32_MAX_VALUE	(0xFFFFFFFF)
#define UINT32_MIN_VALUE	(0x0)
    
    typedef struct tagRange
    {
        uint32 dwStart;
        uint32 dwEnd;
    }TRange;
    
    class CRangeArithmetic
    {
    public:
        static CRangeArithmetic *GetInstance()
        {
            if(NULL == s_pInstance)
            {
                s_pInstance = new CRangeArithmetic();
            }
            return s_pInstance;
        };
    
    public:
        int32 AddRange(const TRange &tRange);
        int32 MergeSortColl(void);
        int32 DeleteRange(const TRange &tRange);
        int32 RemoveAll(void);
    
        vector<TRange> &GetRangeColl(void);
        vector<TRange> &GetMergedRangeColl(void);
        
        void DbgShowProcessData(bool bOpen);
        void DbgShowData(void);
        
    protected:
        typedef enum tagENRangePointRole
        {
            EN_START_POINT = 0,
            EN_END_POINT,
        }ENRangePointRole;
        
        typedef struct tagRangePoint
        {
            uint8 ucRole;
            uint8 ucPad[3];
            uint32 dwValue;
        }TRangePoint;  
        
    protected:
        CRangeArithmetic() {m_bDbgShowProcessData = false;}
        virtual ~CRangeArithmetic() {m_bDbgShowProcessData = false;}
    
        int32 SortColl(void);
    
        
        void DbgShowRangeVector(vector<TRange> &vecRange);
        void DbgShowRangePoint(vector<TRangePoint> &vecRangePoint);
    
        static bool CompareRange(const TRange &tRange1, const TRange &tRange2); 
        static bool CompareRangePoint(const TRangePoint &tPoint1, const TRangePoint &tPoint2);  
        static bool IsAssociationRange(const TRange &tRange1, const TRange &tRange2);
        static int32 DiscoverAssocRange(const TRange &tRange, vector<TRange> &vecRangeSet, vector<TRange> &vecAssocRange);
    
    private:
        vector<TRange> m_vecRange;
        vector<TRange> m_vecMergedRange;
        static CRangeArithmetic * s_pInstance;
        bool m_bDbgShowProcessData;
        DISALLOW_COPY_AND_ASSIGN(CRangeArithmetic);
    };

}// namespace zte_tecs

#endif

